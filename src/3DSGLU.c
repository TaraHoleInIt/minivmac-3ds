/*
	MYOSGLUE.c

	Copyright (C) 2012 Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
 * Operating system glue for the Nintendo 3DS
 * 2016-2017 Tara Keeling
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

#include "STRCONST.h"

/* Uncomment to use debug console as a texture.
 * Press and hold X to see it.
 */
#define DEBUG_CONSOLE

LOCALVAR u32 Keys_Down = 0;
LOCALVAR u32 Keys_Up = 0;
LOCALVAR u32 Keys_Held = 0;

LOCALVAR blnr IsNew3DS = falseblnr;

/* --- control mode and internationalization --- */

#define NeedCell2PlainAsciiMap 1

#include "INTLCHAR.h"

#if dbglog_HAVE

#define dbglog_ToStdErr 1

#if ! dbglog_ToStdErr
LOCALVAR FILE *dbglog_File = NULL;
#endif

LOCALFUNC blnr dbglog_open0(void)
{
#if dbglog_ToStdErr
    return trueblnr;
#else
    dbglog_File = fopen("dbglog.txt", "w");
    return (NULL != dbglog_File);
#endif
}

LOCALPROC dbglog_write0(char *s, uimr L)
{
#if dbglog_ToStdErr
    (void) fwrite(s, 1, L, stderr);
#else
    if (dbglog_File != NULL) {
        (void) fwrite(s, 1, L, dbglog_File);
    }
#endif
}

LOCALPROC dbglog_close0(void)
{
#if ! dbglog_ToStdErr
    if (dbglog_File != NULL) {
        fclose(dbglog_File);
        dbglog_File = NULL;
    }
#endif
}

#endif

/* --- debug settings and utilities --- */

#if ! dbglog_HAVE
#define WriteExtraErr(s)
#else
LOCALPROC WriteExtraErr(char *s)
{
    dbglog_writeCStr("*** error: ");
    dbglog_writeCStr(s);
    dbglog_writeReturn();
}
#endif

/* --- information about the environment --- */

#define WantColorTransValid 0

#include "COMOSGLU.h"
#include "CONTROLM.h"

// Used to transfer the final rendered display to the framebuffer
#define DISPLAY_TRANSFER_FLAGS \
(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#define TEXTURE_TRANSFER_FLAGS \
(GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB565) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB565) | \
GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#define TEXTURE32_TRANSFER_FLAGS \
(GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

#define MyScreenWidth 400
#define MyScreenHeight 240

#define MySubScreenWidth 320
#define MySubScreenHeight 240

/*
 * Lookup table used for indexed->RGB565
 */
static u16 FBConvTable[ 256 ][ 8 ];

C3D_RenderTarget* MainRenderTarget = NULL;
C3D_RenderTarget* SubRenderTarget = NULL;

static DVLB_s* Shader = NULL;
static shaderProgram_s Program;

int LocProjectionUniforms = 0;

C3D_Tex KeyboardTex;
C3D_Tex FBTexture;
C3D_Tex FontTex;

C3D_Mtx ProjectionMain;
C3D_Mtx ProjectionSub;

typedef union {
    u32 Bits;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} rgba32;

/* Courtesy of Bit Twiddling Hacks By Sean Eron Anderson */
u32 NextPowerOf2( unsigned int Value ) {
    // compute the next highest power of 2 of 32-bit v
    Value--;
    Value |= Value >> 1;
    Value |= Value >> 2;
    Value |= Value >> 4;
    Value |= Value >> 8;
    Value |= Value >> 16;
    
    return ++Value;
}

/* Allocate space for an image of With * Height and 32 bit colour depth */
rgba32* AllocImageSpace( int Width, int Height ) {
    return ( rgba32* ) linearMemAlign( Width * Height * sizeof( rgba32 ), 0x80 );
}

/* Check if the PNG signature is valid, returns > 0 on success */
int IsGoodPNG( FILE* fp ) {
    u8 Buffer[ 8 ];
    return ( fread( Buffer, 1, sizeof( Buffer ), fp ) == 8 && png_check_sig( Buffer, 8 ) );
}

/* In theory this should set libpng to give us 32bit RGBA image data */
void ModifyPNGIfWeHaveTo( png_structp PNGHandle, png_infop PNGInfo ) {
    int BitDepth = png_get_bit_depth( PNGHandle, PNGInfo );
    
    /* Unpack packed bit depths */
    if ( BitDepth < 8 )
        png_set_packing( PNGHandle );
    
    /* Convert transparency to alpha */
    if ( png_get_valid( PNGHandle, PNGInfo, PNG_INFO_tRNS ) )
        png_set_tRNS_to_alpha( PNGHandle );
    
    /* Convert to 32bit RGBA */
    switch ( png_get_color_type( PNGHandle, PNGInfo ) ) {
        case PNG_COLOR_TYPE_GRAY: {
            png_set_gray_to_rgb( PNGHandle );
            break;
        }
        case PNG_COLOR_TYPE_GRAY_ALPHA: {
            png_set_gray_to_rgb( PNGHandle );
            break;
        }
        case PNG_COLOR_TYPE_RGB_ALPHA: {
        	png_set_swap_alpha( PNGHandle );
        	break;
        }
        case PNG_COLOR_TYPE_PALETTE: {
            png_set_expand( PNGHandle );
            break;
        }
        case PNG_COLOR_TYPE_RGB: {
            /* What?
             * Why does this work but not PNG_FILLER_BEFORE?!
             */
            png_set_filler( PNGHandle, 0xFF, PNG_FILLER_BEFORE );
            break;
        }
        default: {
        	printf( "%s: png_get_color_type unhandled: %d\n", __FUNCTION__, ( int ) png_get_color_type( PNGHandle, PNGInfo ) );
        	break;
        }
    };
    
    png_read_update_info( PNGHandle, PNGInfo );
}

/* Handles the allocation of the image data and reading of the PNG data into it. */
rgba32* FinishPNGRead( png_structp PNGHandle, int Width, int Height, int RowBytes ) {
    png_bytep RowPointers[ Height ];
    rgba32* ImageData = NULL;
    int i = 0;
    
    ImageData = AllocImageSpace( Width, Height );
 
    if ( ImageData ) {
        for ( i = 0; i < Height; i++ ) {
            RowPointers[ i ] = ( png_bytep ) &ImageData[ i * ( RowBytes / sizeof( rgba32 ) ) ];
        }

        png_read_image( PNGHandle, RowPointers );
    }
    
    return ImageData;
}

/* Loads a PNG image using libpng
 *
 * Path:        Path to PNG image we should load
 * OutWidth:    Pointer to integer which will receive image width to the next power of 2
 * OutHeight:   Pointer to integer which will receive image height to the next power of 2
 * OutData:     Pointer to pointer which will receive pointer to image data. Pointer.
 *
 * Returns 1 on success.
 */
rgba32* LoadPNG( const char* Path, int* OutWidth, int* OutHeight ) {
    png_structp PNGHandle = NULL;
    png_infop PNGInfo = NULL;
    rgba32* ImageData = NULL;
    FILE* fp = NULL;
    int Height = 0;
    int Width = 0;
    
    if ( ( fp = fopen( Path, "rb" ) ) != NULL ) {
        if ( IsGoodPNG( fp ) ) {
            PNGHandle = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
            
            if ( PNGHandle ) {
                PNGInfo = png_create_info_struct( PNGHandle );
                
                if ( PNGInfo ) {
                    if ( setjmp( png_jmpbuf( PNGHandle ) ) ) {
                        png_destroy_read_struct( &PNGHandle, &PNGInfo, NULL );
                        
                        if ( fp )
                            fclose( fp );

                        return 0;
                    }
                    
                    png_init_io( PNGHandle, fp );
                    png_set_sig_bytes( PNGHandle, 8 );
                    png_read_info( PNGHandle, PNGInfo );
                    
                    ModifyPNGIfWeHaveTo( PNGHandle, PNGInfo );
                    
                    /* Since we're loading this for a GPU texture, align it to the next power of 2 */
                    Width = png_get_image_width( PNGHandle, PNGInfo );
                    Height = png_get_image_height( PNGHandle, PNGInfo );
                    
                    Width = ( int ) NextPowerOf2( ( u32 ) Width );
                    Height = ( int ) NextPowerOf2( ( u32 ) Height );
                    
                    ImageData = FinishPNGRead( PNGHandle, Width, Height, Width * sizeof( rgba32 ) );
                    
                    if ( ImageData ) {
                        /* Cleanup and stuff */
                        png_read_end( PNGHandle, NULL );
                        png_destroy_read_struct( &PNGHandle, &PNGInfo, NULL );
                        
                        if ( OutWidth ) *OutWidth = Width;
                        if ( OutHeight ) *OutHeight = Height;
                        
                        fclose( fp );
                        return ImageData;
                    }
                }
                
                png_destroy_read_struct( &PNGHandle, NULL, NULL );
            }
        }
        
        fclose( fp );
    }
    
    return NULL;
}

static u16* TempTextureBuffer = NULL;

void UI_UploadTexture32( void* ImageData, C3D_Tex* Texture, int Width, int Height ) {
    GSPGPU_FlushDataCache( ImageData, Width * Height * sizeof( rgba32 ) );
    C3D_SafeDisplayTransfer( ( u32* ) ImageData, GX_BUFFER_DIM( Width, Height ), ( u32* ) Texture->data, GX_BUFFER_DIM( Width, Height ), TEXTURE32_TRANSFER_FLAGS );
}

/* Hack, here for cleaner code later on */
#if vMacScreenDepth == 0
	blnr UseColorMode = falseblnr;
#endif

int VideoGetBPP( void ) {
	return UseColorMode ? ( 1 << vMacScreenDepth ) : 1;
}

/*
 * Converts a 1bpp packed image and outputs it in RGB555A1 format.
 */
void Convert1BPP( u8* Src, u32* Dest, int Size ) {
	while ( Size-- ) {
		memcpy( Dest, FBConvTable[ *Src ], sizeof( u32 ) * 4 );
		
		Dest+= 4;
		Src++;
	}
}

/*
 * Sets up the 1BPP->RGB565 conversion table.
 */
LOCALPROC MakeTable1BPP( void ) {
	u16* Temp = NULL;
	int i = 0;
	
	for ( i = 0; i < 256; i++ ) {	
		Temp = ( u16* ) FBConvTable[ i ];
		
		Temp[ 0 ] = ( i & BIT( 7 ) ) ? 0 : 0xFFFF;
		Temp[ 1 ] = ( i & BIT( 6 ) ) ? 0 : 0xFFFF;
		Temp[ 2 ] = ( i & BIT( 5 ) ) ? 0 : 0xFFFF;
		Temp[ 3 ] = ( i & BIT( 4 ) ) ? 0 : 0xFFFF;
		Temp[ 4 ] = ( i & BIT( 3 ) ) ? 0 : 0xFFFF;
		Temp[ 5 ] = ( i & BIT( 2 ) ) ? 0 : 0xFFFF;
		Temp[ 6 ] = ( i & BIT( 1 ) ) ? 0 : 0xFFFF;
		Temp[ 7 ] = ( i & BIT( 0 ) ) ? 0 : 0xFFFF;
	}
}

/*
 * Sets up the 8bpp->16bpp paletted to RGB565 conversion table.
 */
void MakeTable8BPP( u16* Reds, u16* Greens, u16* Blues ) {
    int i = 0;
    
    for ( i = 0; i < 256; i++ ) {
        FBConvTable[ i ][ 0 ] = RGB8_to_565( Reds[ i ] >> 8, Greens[ i ] >> 8, Blues[ i ] >> 8 );
    }
}

/*
 * Converts an 8bpp paletted image and outputs it in RGB565 format.
 */
static void Convert8BPP( u8* Src, u16* Dest, int Size ) {
    while ( Size-- ) {
        *Dest++ = FBConvTable[ *Src++ ][ 0 ];
    }
}

/*
 * Sets up the 4bpp->16bpp paletted to RGB565 conversion table.
 */
void MakeTable4BPP( u16* Reds, u16* Greens, u16* Blues ) {
    u8 l;
    u8 h;
    int i = 0;
    
    for ( i = 0; i < 256; i++ ) {
        l = i & 0x0F;
        h = i >> 4;
        
        FBConvTable[ i ][ 1 ] = RGB8_to_565( Reds[ l ] >> 8, Greens[ l ] >> 8, Blues[ l ] >> 8 );
        FBConvTable[ i ][ 0 ] = RGB8_to_565( Reds[ h ] >> 8, Greens[ h ] >> 8, Blues[ h ] >> 8 );
    }
}

/*
 * Converts an 4bpp packed image and outputs it in RGB565 format.
 */
static void Convert4BPP( u8* Src, u16* Dest, int Size ) {
    while ( Size-- ) {
        *Dest++ = FBConvTable[ *Src ][ 0 ];
        *Dest++ = FBConvTable[ *Src++ ][ 1 ];
    }
}

void ConvertFBTo565( u8* Src, u16* Dest, int Size ) {
	switch ( VideoGetBPP( ) ) {
		case 1: {
			Convert1BPP( Src, ( u32* ) Dest, Size );
			break;
		}
		case 4: {
			Convert4BPP( Src, Dest, Size );
			break;
		}
		case 8: {
			Convert8BPP( Src, Dest, Size );
			break;
		}
		default: break;
	};
}

void Video_UpdateTexture( u8* Src, int Left, int Right, int Top, int Bottom ) {
	u16* TempBuffer = ( u16* ) TempTextureBuffer;
	int Offset = 0;
	int Depth = 0;
	static u32 Longest = 0;
	u32 Start, End, Taken = 0;
	
	Start = osGetTime( );
	Depth = VideoGetBPP( );
	
	if ( Depth == 1 ) {
		/* 1BPP: Make sure Left and Right are on an 8 pixel boundary */
		Left = ( Left & ~0x07 );
		Right = ( ( Right + 8 ) & ~0x07 );
	} else if ( Depth == 4 ) {
		// 4BPP: Align to a 2pixel boundary */
		Left = ( Left & ~1 );
		Right = ( Right + 1 ) & ~1;
	}

#if vMacScreenDepth == 0
	MakeTable1BPP( );
#elif vMacScreenDepth == 2
	if ( UseColorMode == trueblnr )
		MakeTable4BPP( CLUT_reds, CLUT_greens, CLUT_blues );
	else
		MakeTable1BPP( );
#elif vMacScreenDepth == 3
	if ( UseColorMode == trueblnr )
		MakeTable8BPP( CLUT_reds, CLUT_greens, CLUT_blues );
	else
		MakeTable1BPP( );
#else
	#error Bit depth unsupported (yet/at all)
#endif

	if ( Left < 0 ) Left = 0;
	if ( Left > vMacScreenWidth ) Left = vMacScreenWidth;
	
	if ( Right < 0 ) Right = 0;
	if ( Right > vMacScreenWidth ) Right = vMacScreenWidth;
	
	if ( Top < 0 ) Top = 0;
	if ( Top > vMacScreenHeight ) Top = vMacScreenHeight;
	
	if ( Bottom < 0 ) Bottom = 0;
	if ( Bottom > vMacScreenHeight ) Bottom = vMacScreenHeight;
	
	for ( ; Top < Bottom; Top++ ) {
		TempBuffer =  ( u16* ) &TempTextureBuffer[ ( ( Top * 512 )  + Left ) ];
		Offset = ( ( Top * vMacScreenWidth ) / ( 8 / Depth ) ) + ( Left / ( 8 / Depth ) );
					
		ConvertFBTo565( &Src[ Offset ], TempBuffer, ( Right - Left ) );
	}
	
	End = osGetTime( );
	Taken = End - Start;
	
	if ( Taken > Longest )
		Longest = Taken;
		
	iprintf( "\x1b[2J" );
	iprintf( "FB Took %dms, longest: %dms\n", ( int ) Taken, ( int ) Longest );
}

struct Vertex {
	float Position[ 3 ];
	float Texcoords[ 2 ];
	float Color[ 4 ];
};

/* =======================================
 * == Beginning of font support section ==
 * =======================================
 */

#define FontTex_Width 256
#define FontTex_Height 128
#define Cell_Width 8
#define Cell_Height 16

#define Font_Max_Vertex 1024

float ColorBlack[ 4 ] = { 0.0, 0.0, 0.0, 1.0 };
float ColorGreen[ 4 ] = { 0.0, 1.0, 0.0, 1.0 };
float ColorAqua[ 4 ] = { 0.0, 1.0, 1.0, 1.0 };

LOCALVAR struct Vertex* FontVertexList = NULL;
LOCALVAR blnr HasFontLoaded = falseblnr;

LOCALVAR float GlyphTexCoords[ 256 ][ 8 ];
LOCALVAR int VertexCount = 0;

LOCALVAR rgba32* FontSheetImage = NULL;

/*
 * Just a little helper to make vertex setup a bit less messy looking.
 *
 * Params:
 * Ptr		: Pointer to vertex
 * X		: X Coordinate
 * Y		: Y Coordinate
 * Z		: Z Coordinate
 * u		: Texcoord 0
 * v		: Texcoord 1
 * Color	: Pointer to an array of 4 floating point color values
 */
LOCALPROC SetVertex( struct Vertex* Ptr, float X, float Y, float Z, float u, float v, float* Color ) {
	Ptr->Position[ 0 ] = X;
	Ptr->Position[ 1 ] = Y;
	Ptr->Position[ 2 ] = Z;

	Ptr->Texcoords[ 0 ] = u;
	Ptr->Texcoords[ 1 ] = v;

	memcpy( Ptr->Color, Color, sizeof( float ) * 4 );
}

LOCALFUNC int FontDrawChar( int X, int Y, int Glyph, float R, float G, float B, float A ) {
	float Colors[ 4 ] = { R, G, B, A };
	struct Vertex* Ptr = NULL;

	if ( Glyph >= 0 && Glyph < 256 && ( VertexCount + 6 ) < Font_Max_Vertex ) {
		Ptr = &FontVertexList[ VertexCount ];
		
		/* Top left */
		SetVertex( &Ptr[ 0 ],
					( float ) X,
					( float ) Y,
					0.5f,
					GlyphTexCoords[ Glyph ][ 0 ],
					GlyphTexCoords[ Glyph ][ 1 ],
					Colors );
		
		/* Bottom right */
		SetVertex( &Ptr[ 1 ],
					( float ) ( X + Cell_Width ),
					( float ) ( Y + Cell_Height ),
					0.5f,
					GlyphTexCoords[ Glyph ][ 2 ],
					GlyphTexCoords[ Glyph ][ 3 ],
					Colors );

		/* Top right */
		SetVertex( &Ptr[ 2 ],
					( float ) ( X + Cell_Width ),
					( float ) Y,
					0.5f,
					GlyphTexCoords[ Glyph ][ 4 ],
					GlyphTexCoords[ Glyph ][ 5 ],
					Colors );
		
		/* Top left */
		SetVertex( &Ptr[ 3 ],
					( float ) X,
					( float ) Y,
					0.5f,
					GlyphTexCoords[ Glyph ][ 0 ],
					GlyphTexCoords[ Glyph ][ 1 ],
					Colors );
		
		/* Bottom left */
		SetVertex( &Ptr[ 4 ],
					( float ) X,
					( float ) ( Y + Cell_Height ),
					0.5f,
					GlyphTexCoords[ Glyph ][ 6 ],
					GlyphTexCoords[ Glyph ][ 7 ],
					Colors );
		
		/* Bottom right */
		SetVertex( &Ptr[ 5 ],
					( float ) ( X + Cell_Width ),
					( float ) ( Y + Cell_Height ),
					0.5f,
					GlyphTexCoords[ Glyph ][ 2 ],
					GlyphTexCoords[ Glyph ][ 3 ],
					Colors );
		
		VertexCount+= 6;
		return 1;
	}
	
	return 0;
}

LOCALFUNC int FontDrawString( int X, int Y, const char* Str, float FGColor[ 4 ], float BGColor[ 4 ] ) {
	struct Vertex* Ptr = &FontVertexList[ VertexCount ];
	int Length = 0;
	int i = 0;
	
	Length = strlen( Str );

	/* Draw background color as 2 triangles with the texture being the full block glyph. */
	if ( Length > 0 && ( VertexCount + 6 ) < Font_Max_Vertex ) {
		/* Top left */
		SetVertex( &Ptr[ 0 ],
					( float ) X,
					( float ) Y,
					0.6f,
					GlyphTexCoords[ 0 ][ 0 ],
					GlyphTexCoords[ 0 ][ 1 ],
					BGColor );
	
		/* Bottom right */
		SetVertex( &Ptr[ 1 ],
					( float ) ( X + ( Length * Cell_Width ) ),
					( float ) ( Y + Cell_Height ),
					0.6f,
					GlyphTexCoords[ 0 ][ 2 ],
					GlyphTexCoords[ 0 ][ 3 ],
					BGColor );
	
		/* Top right */
		SetVertex( &Ptr[ 2 ],
					( float ) ( X + ( Length * Cell_Width ) ),
					( float ) Y,
					0.6f,
					GlyphTexCoords[ 0 ][ 4 ],
					GlyphTexCoords[ 0 ][ 5 ],
					BGColor );
		
		/* Top left */
		SetVertex( &Ptr[ 3 ],
					( float ) X,
					( float ) Y,
					0.6f,
					GlyphTexCoords[ 0 ][ 0 ],
					GlyphTexCoords[ 0 ][ 1 ],
					BGColor );		
	
		/* Bottom left */
		SetVertex( &Ptr[ 4 ],
					( float ) X,
					( float ) ( Y + Cell_Height ),
					0.6f,
					GlyphTexCoords[ 0 ][ 6 ],
					GlyphTexCoords[ 0 ][ 7 ],
					BGColor );
			
		/* Bottom right */
		SetVertex( &Ptr[ 5 ],
					( float ) ( X + ( Length * Cell_Width ) ),
					( float ) ( Y + Cell_Height ),
					0.6f,
					GlyphTexCoords[ 0 ][ 2 ],
					GlyphTexCoords[ 0 ][ 3 ],
					BGColor );	
		
		VertexCount+= 6;
	}
	
	for ( Length = strlen( Str ), i = 0; i < Length; i++ ) {
		if ( FontDrawChar( X, Y, Str[ i ], FGColor[ 0 ], FGColor[ 1 ], FGColor[ 2 ], FGColor[ 3 ] ) ) X+= Cell_Width;
		else break;
	}
	
	return i;
}

LOCALPROC FontRenderAll( blnr Clear ) {
	C3D_TexBind( 0, &FontTex );

	if ( VertexCount >= 6 )
		C3D_DrawArrays( GPU_TRIANGLES, 0, VertexCount );
	
	if ( Clear == trueblnr )
		VertexCount = 0;
}

LOCALFUNC int SetupFontVBuffer( void ) {
	C3D_BufInfo* BufInfo = NULL;

	FontVertexList = ( struct Vertex* ) linearAlloc( sizeof( struct Vertex ) * Font_Max_Vertex );
	
	if ( FontVertexList ) {
		memset( FontVertexList, 0, sizeof( struct Vertex ) * Font_Max_Vertex );
		
		BufInfo = C3D_GetBufInfo( );
		
		if ( BufInfo ) {
			BufInfo_Init( BufInfo );
			BufInfo_Add( BufInfo, FontVertexList, sizeof( struct Vertex ), 3, 0x210 );
			
			return 1;
		}
	}

	return 0;
}

LOCALPROC BuildGlyphCoords( void ) {
	int Glyph = 0;
	int x = 0;
	int y = 0;

	memset( GlyphTexCoords, 0, sizeof( GlyphTexCoords ) );

	for ( y = 0; y < FontTex_Height; y+= Cell_Height ) {
		for ( x = 0; x < FontTex_Width; x+= Cell_Width, Glyph++ ) {
			/* Pls no overflow ;_; */
			if ( Glyph > 255 )
				break;		
		
			/* Top left */
			GlyphTexCoords[ Glyph ][ 0 ] = ( ( float ) x / ( float ) FontTex_Width );
			GlyphTexCoords[ Glyph ][ 1 ] = ( ( float ) y / ( float ) FontTex_Height );
			
			/* Bottom right */
			GlyphTexCoords[ Glyph ][ 2 ] = ( ( float ) ( x + Cell_Width ) / ( float ) FontTex_Width );
			GlyphTexCoords[ Glyph ][ 3 ] = ( ( float ) ( y + Cell_Height ) / ( float ) FontTex_Height );
			
			/* Top right */
			GlyphTexCoords[ Glyph ][ 4 ] = ( ( float ) ( x + Cell_Width ) / ( float ) FontTex_Width );
			GlyphTexCoords[ Glyph ][ 5 ] = ( ( float ) y / ( float ) FontTex_Height );
			
			/* Bottom left */
			GlyphTexCoords[ Glyph ][ 6 ] = ( ( float ) x / ( float ) FontTex_Width );
			GlyphTexCoords[ Glyph ][ 7 ] = ( ( float ) ( y + Cell_Height ) / ( float ) FontTex_Height );
		}
	}
}

/*
 * Loads the bitmap font sheet.
 */
LOCALPROC LoadFont( void ) {
	int Height = 0;
	int Width = 0;
	
	if ( ( FontSheetImage = LoadPNG( "gfx/ui_font.png", &Width, &Height ) ) != NULL ) {
		if ( Width == FontTex_Width && Height == FontTex_Height ) {
			UI_UploadTexture32( FontSheetImage, &FontTex, Width, Height );
			
			if ( SetupFontVBuffer( ) ) {
				HasFontLoaded = trueblnr;
				BuildGlyphCoords( );
			}
		} else {
			MacMsg( "Invalid resource", "Font size must be 256x128", falseblnr );
		}
	} else {
		MacMsg( "Missing resource", "Missing ui_font.png in gfx folder", falseblnr );
	}
}

LOCALPROC FreeFont( void ) {
	C3D_TexDelete( &FontTex );
	
	if ( FontSheetImage )
		linearFree( FontSheetImage );
	
	if ( FontVertexList )
		linearFree( FontVertexList );
}

/* =================================
 * == End of font support section ==
 * =================================
 */

void DrawTexture( C3D_Tex* Texture, int Width, int Height, float X, float Y, float ScaleX, float ScaleY ) {
    float u = 1.0;
    float v = 1.0;
    
    Width*= ScaleX;
    Height*= ScaleY;
    
    C3D_TexBind( 0, Texture );
        
    C3D_ImmDrawBegin( GPU_TRIANGLES );
    // 1st triangle
        C3D_ImmSendAttrib( X, Y, 1.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
        
        C3D_ImmSendAttrib( X + Width, Y + Height, 1.0, 0.0 );
        C3D_ImmSendAttrib( u, v, 0.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
        
        C3D_ImmSendAttrib( X + Width, Y, 1.0, 0.0 );
        C3D_ImmSendAttrib( u, 0.0, 0.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
        
        // 2nd triangle
        C3D_ImmSendAttrib( X, Y, 1.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
        
        C3D_ImmSendAttrib( X, Y + Height, 1.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, v, 0.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
        
        C3D_ImmSendAttrib( X + Width, Y + Height, 1.0, 0.0 );
        C3D_ImmSendAttrib( u, v, 0.0, 0.0 );
        C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
    C3D_ImmDrawEnd( );
}

static int Video_SetupRenderTarget( void ) {
    MainRenderTarget = C3D_RenderTargetCreate( 240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8 );
    SubRenderTarget = C3D_RenderTargetCreate( 240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8 );
    
    if ( MainRenderTarget && SubRenderTarget ) {
        C3D_RenderTargetSetClear( MainRenderTarget, C3D_CLEAR_ALL, 0x000000FF, 0 );
        C3D_RenderTargetSetOutput( MainRenderTarget, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS );
        
        C3D_RenderTargetSetClear( SubRenderTarget, C3D_CLEAR_ALL, 0x000000FF, 0 );
        C3D_RenderTargetSetOutput( SubRenderTarget, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS );
        
        return 1;
    }
    
    return 0;
}

LOCALVAR const unsigned char vshader_shbin[ ] = {
	0x44, 0x56, 0x4c, 0x42, 0x01, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x44, 0x56, 0x4c, 0x50, 
	0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x4c, 0x00, 0x00, 0x00, 
	0x07, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x01, 0xf0, 0x07, 0x4e, 0x02, 0x08, 0x02, 0x08, 
	0x03, 0x18, 0x02, 0x08, 0x04, 0x28, 0x02, 0x08, 0x05, 0x38, 0x02, 0x08, 0x06, 0x10, 0x20, 0x4c, 
	0x06, 0x20, 0x40, 0x4c, 0x00, 0x00, 0x00, 0x88, 0x6e, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xa1, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0xc3, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x64, 0xc3, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xc3, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x61, 0xc3, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x44, 0x56, 0x4c, 0x45, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
	0x02, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xbf, 0x00, 
	0x99, 0x99, 0x3b, 0x00, 0x02, 0x00, 0x5e, 0x00, 0x33, 0x33, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0x01, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x0f, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x13, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x78, 0x00, 0x78, 0x00, 
	0x70, 0x72, 0x6f, 0x6a, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x74, 0x65, 0x73, 0x74, 0x00
};

static int Video_SetupShader( void ) {
    C3D_AttrInfo* Info = NULL;
    
    Shader = DVLB_ParseFile( ( u32* ) vshader_shbin, ( u32 ) sizeof( vshader_shbin ) );
    
    if ( Shader ) {
        shaderProgramInit( &Program );
        shaderProgramSetVsh( &Program, &Shader->DVLE[ 0 ] );
        
        C3D_BindProgram( &Program );
        
        LocProjectionUniforms = shaderInstanceGetUniformLocation( Program.vertexShader, "projection" );
        Info = C3D_GetAttrInfo( );
        
        if ( Info ) {
            AttrInfo_Init( Info );
            AttrInfo_AddLoader( Info, 0, GPU_FLOAT, 3 );
            AttrInfo_AddLoader( Info, 1, GPU_FLOAT, 2 );
            AttrInfo_AddLoader( Info, 2, GPU_FLOAT, 4 );
            
            //BufInfo_Init( C3D_GetBufInfo( ) );
            return 1;
        }
    }
    
    return 0;
}

static int Video_CreateTextures( void ) {
    C3D_TexEnv* Env = NULL;
    
    C3D_TexInit( &FBTexture, 512, 512, GPU_RGB565 );
    C3D_TexInit( &KeyboardTex, 512, 256, GPU_RGBA8 );
    C3D_TexInit( &FontTex, FontTex_Width, FontTex_Height, GPU_RGBA8 );
    
    C3D_TexSetFilter( &FBTexture, GPU_NEAREST, GPU_NEAREST );
    C3D_TexSetFilter( &KeyboardTex, GPU_NEAREST, GPU_NEAREST );
    C3D_TexSetFilter( &FontTex, GPU_NEAREST, GPU_NEAREST );
    
    Env = C3D_GetTexEnv( 0 );
    
    if ( Env ) {
        //C3D_TexEnvSrc( Env, C3D_Both, GPU_TEXTURE0, 0, 0 );
        //C3D_TexEnvOp( Env, C3D_Both, 0, 0, 0 );
        //C3D_TexEnvFunc( Env, C3D_Both, GPU_REPLACE );
        
		C3D_TexEnvSrc( Env, C3D_RGB, GPU_PRIMARY_COLOR, GPU_TEXTURE0, GPU_TEXTURE0 );
		C3D_TexEnvSrc( Env, C3D_Alpha, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0 );
		C3D_TexEnvOp( Env, C3D_Both, 0, 0, 0 );
		C3D_TexEnvFunc( Env, C3D_RGB, GPU_ADD );
		C3D_TexEnvFunc( Env, C3D_Alpha, GPU_MODULATE );
    }
    
    LoadFont( );
    
    return 1;
}

#ifdef DEBUG_CONSOLE
#include <sys/iosupport.h>

#define CONSOLE_TEXTURE_TRANSFER_FLAGS \
(GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB565) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB565) | \
GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

extern ssize_t con_write(struct _reent *r,void* fd,const char *ptr, size_t len);
extern u8 default_font_bin[ ];

static u16* ConsoleBuffer240x320 = NULL;
static u16* ConsoleTextureBuffer = NULL;

static int ConsoleDirty = 0;

static C3D_Tex ConsoleTex;

static ssize_t MyConsoleWrite( struct _reent* r, void* fd, const char* ptr, size_t len ) {
    ConsoleDirty = 1;
    return con_write( r, fd, ptr, len );
}

static PrintConsole C3DConsole = {
    {
        default_font_bin,
        0,
        256
    },
    NULL,
    0,
    0,
    0,
    0,
    40,
    30,
    0,
    0,
    40,
    30,
    3,
    7,
    0,
    0,
    0,
    false
};

static const devoptab_t MyDevOpTab = {
    "con",
    0,
    NULL,
    NULL,
    MyConsoleWrite,
    NULL,
    NULL,
    NULL
};

void DebugConsoleFree( void ) {
    consoleSelect( consoleGetDefault( ) );
    
    if ( ConsoleBuffer240x320 )
        free( ConsoleBuffer240x320 );
    
    if ( ConsoleTextureBuffer )
        linearFree( ConsoleTextureBuffer );
    
    ConsoleBuffer240x320 = NULL;
    ConsoleTextureBuffer = NULL;
}

int DebugConsoleInit( void ) {
    ConsoleBuffer240x320 = ( u16* ) malloc( 240 * 320 * 2 );
    ConsoleTextureBuffer = ( u16* ) linearMemAlign( 256 * 512 * 2, 0x80 );
    
    if ( ConsoleBuffer240x320 && ConsoleTextureBuffer ) {
        C3D_TexInit( &ConsoleTex, 256, 512, GPU_RGB565 );
        
        devoptab_list[ STD_OUT ] = &MyDevOpTab;
        devoptab_list[ STD_ERR ] = &MyDevOpTab;
        
        setvbuf( stdout, NULL, _IONBF, 0 );
        setvbuf( stderr, NULL, _IONBF, 0 );
        
        C3DConsole.frameBuffer = ConsoleBuffer240x320;
        C3DConsole.consoleInitialised = true;
        
        consoleSelect( &C3DConsole );
        consoleSetWindow( &C3DConsole, 0, 0, MySubScreenWidth / 8, MySubScreenHeight / 8 );
        consoleClear( );
        
        return 1;
    }
    
    DebugConsoleFree( );
    return 0;
}

void DebugConsoleUpdate( void ) {
    u16* Src = ( u16* ) ConsoleBuffer240x320;
    u16* Dst = ( u16* ) ConsoleTextureBuffer;
    int x = 0;
    int y = 0;
    
    if ( ConsoleDirty ) {
        /*
         * TODO:
         * Speed up later, this is gonna be slow as BALLS.
         */
        for ( y = 0; y < 320; y++ ) {
            for ( x = 0; x < 240; x++ ) {
                Dst[ x + ( y * 256 ) ] = Src[ x + ( y * 240 ) ];
            }
        }
    
        GSPGPU_FlushDataCache( ConsoleTextureBuffer, 256 * 512 * 2 );
        C3D_SafeDisplayTransfer( ( u32* ) ConsoleTextureBuffer, GX_BUFFER_DIM( 256, 512 ), ( u32* ) ConsoleTex.data, GX_BUFFER_DIM( 256, 512 ), CONSOLE_TEXTURE_TRANSFER_FLAGS );
        
        ConsoleDirty = 0;
    }
}

void DebugConsoleDraw( void ) {
    C3D_TexBind( 0, &ConsoleTex );
    
    /*
     * Sorcery!
     * I don't know it either.
     */
    C3D_ImmDrawBegin( GPU_TRIANGLES );
    // 1st triangle
    C3D_ImmSendAttrib( 0, 0, 0.5, 0.0 );
    C3D_ImmSendAttrib( 1.0, 0.0, 0.0, 0.0 );
    C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
    
    C3D_ImmSendAttrib( 512, 256, 0.5, 0.0 );
    C3D_ImmSendAttrib( 0.0, 1.0, 0.0, 0.0 );
    C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
    
    C3D_ImmSendAttrib( 512, 0, 0.5, 0.0 );
    C3D_ImmSendAttrib( 1.0, 1.0, 0.0, 0.0 );
    C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
    
    // 2nd triangle
    C3D_ImmSendAttrib( 0, 0, 0.5, 0.0 );
    C3D_ImmSendAttrib( 1.0, 0.0, 0.0, 0.0 );
    C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
    
    C3D_ImmSendAttrib( 0, 256, 0.5, 0.0 );
    C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 0.0 );
    C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
    
    C3D_ImmSendAttrib( 512, 256, 0.5, 0.0 );
    C3D_ImmSendAttrib( 0.0, 1.0, 0.0, 0.0 );
    C3D_ImmSendAttrib( 0.0, 0.0, 0.0, 1.0 );
    C3D_ImmDrawEnd( );
}
#endif

int Video_Init( void ) {
    gfxInitDefault( );
    //consoleInit( GFX_BOTTOM, NULL );
    //printf( "Hi!\n" );
    
    C3D_Init( C3D_DEFAULT_CMDBUF_SIZE );
    
    Video_SetupRenderTarget( );
    Video_SetupShader( );
    
    Mtx_OrthoTilt( &ProjectionMain, 0.0, 400.0, 240.0, 0.0, 0.0, 1.0, true );
    Mtx_OrthoTilt( &ProjectionSub, 0.0, 320.0, 240.0, 0.0, 0.0, 1.0, true );
    
    C3D_DepthTest( true, GPU_GEQUAL, GPU_WRITE_ALL );
    
    TempTextureBuffer = linearMemAlign( 512 * 512 * 2, 0x80 );
    
#ifdef DEBUG_CONSOLE
    DebugConsoleInit( );
#endif

    Video_CreateTextures( );
    
    return TempTextureBuffer ? 1 : 0;
}

void Video_Close( void ) {
    if ( TempTextureBuffer )
        linearFree( TempTextureBuffer );
    
    if ( Shader ) {
        shaderProgramFree( &Program );
        DVLB_Free( Shader );
    }
    
    if ( MainRenderTarget )
        C3D_RenderTargetDelete( MainRenderTarget );
    
    if ( SubRenderTarget )
        C3D_RenderTargetDelete( SubRenderTarget );
    
#ifdef DEBUG_CONSOLE
    DebugConsoleFree( );
#endif

	FreeFont( );
    
    C3D_TexDelete( &FBTexture );
    C3D_TexDelete( &KeyboardTex );
    
    C3D_Fini( );
    
    gfxExit( );
}

/* --- some simple utilities --- */

GLOBALPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- parameter buffers --- */

#if IncludePbufs
LOCALVAR void *PbufDat[NumPbufs];
#endif

#if IncludePbufs
LOCALFUNC tMacErr PbufNewFromPtr(void *p, ui5b count, tPbuf *r)
{
	tPbuf i;
	tMacErr err;

	if (! FirstFreePbuf(&i)) {
		free(p);
		err = mnvm_miscErr;
	} else {
		*r = i;
		PbufDat[i] = p;
		PbufNewNotify(i, count);
		err = mnvm_noErr;
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALFUNC tMacErr PbufNew(ui5b count, tPbuf *r)
{
	tMacErr err = mnvm_miscErr;

	void *p = calloc(1, count);
	if (NULL != p) {
		err = PbufNewFromPtr(p, count, r);
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALPROC PbufDispose(tPbuf i)
{
	free(PbufDat[i]);
	PbufDisposeNotify(i);
}
#endif

#if IncludePbufs
LOCALPROC UnInitPbufs(void)
{
	tPbuf i;

	for (i = 0; i < NumPbufs; ++i) {
		if (PbufIsAllocated(i)) {
			PbufDispose(i);
		}
	}
}
#endif

#if IncludePbufs
GLOBALPROC PbufTransfer(ui3p Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	void *p = ((ui3p)PbufDat[i]) + offset;
	if (IsWrite) {
		(void) memcpy(p, Buffer, count);
	} else {
		(void) memcpy(Buffer, p, count);
	}
}
#endif

/* --- text translation --- */

LOCALPROC NativeStrFromCStr(char *r, char *s)
{
	ui3b ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i) {
		r[i] = Cell2PlainAsciiMap[ps[i]];
	}

	r[L] = 0;
}

/* --- drives --- */

#define NotAfileRef NULL

LOCALVAR FILE *Drives[NumDrives]; /* open disk image files */

LOCALPROC InitDrives(void)
{
	/*
		This isn't really needed, Drives[i] and DriveNames[i]
		need not have valid values when not vSonyIsInserted[i].
	*/
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
}

GLOBALFUNC tMacErr vSonyTransfer(blnr IsWrite, ui3p Buffer,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count,
	ui5r *Sony_ActCount)
{
	tMacErr err = mnvm_miscErr;
	FILE *refnum = Drives[Drive_No];
	ui5r NewSony_Count = 0;

	if (0 == fseek(refnum, Sony_Start, SEEK_SET)) {
		if (IsWrite) {
			NewSony_Count = fwrite(Buffer, 1, Sony_Count, refnum);
		} else {
			NewSony_Count = fread(Buffer, 1, Sony_Count, refnum);
		}

		if (NewSony_Count == Sony_Count) {
			err = mnvm_noErr;
		}
	}

	if (nullpr != Sony_ActCount) {
		*Sony_ActCount = NewSony_Count;
	}

	return err; /*& figure out what really to return &*/
}

GLOBALFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	tMacErr err = mnvm_miscErr;
	FILE *refnum = Drives[Drive_No];
	long v;

	if (0 == fseek(refnum, 0, SEEK_END)) {
		v = ftell(refnum);
		if (v >= 0) {
			*Sony_Count = v;
			err = mnvm_noErr;
		}
	}

	return err; /*& figure out what really to return &*/
}

LOCALFUNC tMacErr vSonyEject0(tDrive Drive_No, blnr deleteit)
{
	FILE *refnum = Drives[Drive_No];

	DiskEjectedNotify(Drive_No);

	fclose(refnum);
	Drives[Drive_No] = NotAfileRef; /* not really needed */

	return mnvm_noErr;
}

GLOBALFUNC tMacErr vSonyEject(tDrive Drive_No)
{
	printf( "%s\n", __FUNCTION__ );
	return vSonyEject0(Drive_No, falseblnr);
}

LOCALPROC UnInitDrives(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

LOCALFUNC blnr Sony_Insert0(FILE *refnum, blnr locked,
	char *drivepath)
{
	tDrive Drive_No;
	blnr IsOk = falseblnr;

	if (! FirstFreeDisk(&Drive_No)) {
		MacMsg(kStrTooManyImagesTitle, kStrTooManyImagesMessage,
			falseblnr);
	} else {
		/* printf("Sony_Insert0 %d\n", (int)Drive_No); */
		{
			Drives[Drive_No] = refnum;
			DiskInsertNotify(Drive_No, locked);

			IsOk = trueblnr;
		}
	}

	if (! IsOk) {
		fclose(refnum);
	}

	return IsOk;
}

LOCALFUNC blnr Sony_Insert1(char *drivepath, blnr silentfail)
{
	blnr locked = falseblnr;
	/* printf("Sony_Insert1 %s\n", drivepath); */
	FILE *refnum = fopen(drivepath, "rb+");
	if (NULL == refnum) {
		locked = trueblnr;
		refnum = fopen(drivepath, "rb");
	}
	if (NULL == refnum) {
		if (! silentfail) {
			MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
		}
	} else {
		return Sony_Insert0(refnum, locked, drivepath);
	}
	return falseblnr;
}

LOCALFUNC blnr Sony_Insert2(char *s)
{
	return Sony_Insert1(s, trueblnr);
}

LOCALFUNC blnr LoadInitialImages(void)
{
	if (! AnyDiskInserted()) {
		int n = NumDrives > 9 ? 9 : NumDrives;
		int i;
		char s[] = "disk?.dsk";

		for (i = 1; i <= n; ++i) {
			s[4] = '0' + i;
			if (! Sony_Insert2(s)) {
				/* stop on first error (including file not found) */
				return trueblnr;
			}
		}
	}

	return trueblnr;
}

/* --- ROM --- */

LOCALVAR char *rom_path = NULL;

LOCALFUNC tMacErr LoadMacRomFrom(char *path)
{
	tMacErr err;
	FILE *ROM_File;
	int File_Size;

	ROM_File = fopen(path, "rb");
	if (NULL == ROM_File) {
		err = mnvm_fnfErr;
	} else {
		File_Size = fread(ROM, 1, kROM_Size, ROM_File);
		if (File_Size != kROM_Size) {
			if (feof(ROM_File)) {
				err = mnvm_eofErr;
			} else {
				err = mnvm_miscErr;
			}
		} else {
			err = mnvm_noErr;
		}
		fclose(ROM_File);
	}

	return err;
}

LOCALFUNC blnr LoadMacRom(void)
{
	tMacErr err;

	if ((NULL == rom_path)
		|| (mnvm_fnfErr == (err = LoadMacRomFrom(rom_path))))
	if (mnvm_fnfErr == (err = LoadMacRomFrom(RomFileName)))
	{
	}

	if (mnvm_noErr != err) {
		if (mnvm_fnfErr == err) {
			MacMsg(kStrNoROMTitle, kStrNoROMMessage, trueblnr);
		} else if (mnvm_eofErr == err) {
			MacMsg(kStrShortROMTitle, kStrShortROMMessage,
				trueblnr);
		} else {
			MacMsg(kStrNoReadROMTitle, kStrNoReadROMMessage,
				trueblnr);
		}

		SpeedStopped = trueblnr;
	}

	return trueblnr; /* keep launching Mini vMac, regardless */
}

/* --- video out --- */

#if VarFullScreen
LOCALVAR blnr UseFullScreen = (WantInitFullScreen != 0);
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = (WantInitMagnify != 0);
#endif

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

#if EnableMagnify
#define MaxScale MyWindowScale
#else
#define MaxScale 1
#endif

LOCALPROC HaveChangedScreenBuff(ui4r top, ui4r left,
	ui4r bottom, ui4r right)
{
	Video_UpdateTexture( ( u8* ) GetCurDrawBuff( ), left, right, top, bottom );
}

LOCALPROC MyDrawChangesAndClear(void)
{
	if (ScreenChangedBottom > ScreenChangedTop) {
		HaveChangedScreenBuff(ScreenChangedTop, ScreenChangedLeft,
			ScreenChangedBottom, ScreenChangedRight);
		ScreenClearChanges();
	}
}

GLOBALPROC DoneWithDrawingForTick(void)
{
#if EnableMouseMotion && MayFullScreen
	if (HaveMouseMotion) {
		AutoScrollScreen();
	}
#endif
	MyDrawChangesAndClear();
}

/* --- mouse --- */


/* cursor state */
LOCALPROC CheckMouseState(void)
{
}

/* --- keyboard input --- */
#define Map_Width 40
#define Map_Height 30

const int Keyboard_Map[ Map_Height ][ Map_Width ] = {
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x60,  0x60,  0x31,  0x31,  0x32,  0x32,  0x33,  0x33,  0x34,
        0x34,  0x35,  0x35,  0x36,  0x36,  0x37,  0x37,  0x38,  0x38,  0x39,
        0x39,  0x30,  0x30,  0x2D,  0x2D,  0x3D,  0x3D,  0x08,  0x08,  0x08,
        0x00,  0xF2,  0xF2,  0x3D,  0x3D,  0xFE,  0xFE,  0xFD,  0xFD,  0x00, },
    {
        0x00,  0x60,  0x60,  0x31,  0x31,  0x32,  0x32,  0x33,  0x33,  0x34,
        0x34,  0x35,  0x35,  0x36,  0x36,  0x37,  0x37,  0x38,  0x38,  0x39,
        0x39,  0x30,  0x30,  0x2D,  0x2D,  0x3D,  0x3D,  0x08,  0x08,  0x08,
        0x00,  0xF2,  0xF2,  0x3D,  0x3D,  0xFE,  0xFE,  0xFD,  0xFD,  0x00, },
    {
        0x00,  0x09,  0x09,  0x09,  0x71,  0x71,  0x77,  0x77,  0x65,  0x65,
        0x72,  0x72,  0x74,  0x74,  0x79,  0x79,  0x75,  0x75,  0x69,  0x69,
        0x6F,  0x6F,  0x70,  0x70,  0x5B,  0x5B,  0x5D,  0x5D,  0x13,  0x13,
        0x00,  0x37,  0x37,  0x38,  0x38,  0x39,  0x39,  0xFC,  0xFC,  0x00, },
    {
        0x00,  0x09,  0x09,  0x09,  0x71,  0x71,  0x77,  0x77,  0x65,  0x65,
        0x72,  0x72,  0x74,  0x74,  0x79,  0x79,  0x75,  0x75,  0x69,  0x69,
        0x6F,  0x6F,  0x70,  0x70,  0x5B,  0x5B,  0x5D,  0x5D,  0x13,  0x13,
        0x00,  0x37,  0x37,  0x38,  0x38,  0x39,  0x39,  0xFC,  0xFC,  0x00, },
    {
        0x00,  0xFA,  0xFA,  0xFA,  0xFA,  0x61,  0x61,  0x73,  0x73,  0x64,
        0x64,  0x66,  0x66,  0x67,  0x67,  0x68,  0x68,  0x6A,  0x6A,  0x6B,
        0x6B,  0x6C,  0x6C,  0x3B,  0x3B,  0x27,  0x27,  0x13,  0x13,  0x13,
        0x00,  0x34,  0x34,  0x35,  0x35,  0x36,  0x36,  0xFB,  0xFB,  0x00, },
    {
        0x00,  0xFA,  0xFA,  0xFA,  0xFA,  0x61,  0x61,  0x73,  0x73,  0x64,
        0x64,  0x66,  0x66,  0x67,  0x67,  0x68,  0x68,  0x6A,  0x6A,  0x6B,
        0x6B,  0x6C,  0x6C,  0x3B,  0x3B,  0x27,  0x27,  0x13,  0x13,  0x13,
        0x00,  0x34,  0x34,  0x35,  0x35,  0x36,  0x36,  0xFB,  0xFB,  0x00, },
    {
        0x00,  0xF9,  0xF9,  0xF9,  0xF9,  0xF9,  0x7A,  0x7A,  0x78,  0x78,
        0x63,  0x63,  0x76,  0x76,  0x62,  0x62,  0x6E,  0x6E,  0x6D,  0x6D,
        0x2C,  0x2C,  0x2E,  0x2E,  0x2F,  0x2F,  0xF9,  0xF9,  0xF6,  0xF6,
        0x00,  0x31,  0x31,  0x32,  0x32,  0x33,  0x33,  0x13,  0x13,  0x00, },
    {
        0x00,  0xF9,  0xF9,  0xF9,  0xF9,  0xF9,  0x7A,  0x7A,  0x78,  0x78,
        0x63,  0x63,  0x76,  0x76,  0x62,  0x62,  0x6E,  0x6E,  0x6D,  0x6D,
        0x2C,  0x2C,  0x2E,  0x2E,  0x2F,  0x2F,  0xF9,  0xF9,  0xF6,  0xF6,
        0x00,  0x31,  0x31,  0x32,  0x32,  0x33,  0x33,  0x13,  0x13,  0x00, },
    {
        0x00,  0xF8,  0xF8,  0xF8,  0xF8,  0xF7,  0xF7,  0xF7,  0xF7,  0xF7,
        0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,
        0x20,  0x20,  0x5C,  0x5C,  0xF5,  0xF5,  0xF4,  0xF4,  0xF3,  0xF3,
        0x00,  0x30,  0x30,  0x30,  0x30,  0x2E,  0x2E,  0x13,  0x13,  0x00, },
    {
        0x00,  0xF8,  0xF8,  0xF8,  0xF8,  0xF7,  0xF7,  0xF7,  0xF7,  0xF7,
        0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,  0x20,
        0x20,  0x20,  0x5C,  0x5C,  0xF5,  0xF5,  0xF4,  0xF4,  0xF3,  0xF3,
        0x00,  0x30,  0x30,  0x30,  0x30,  0x2E,  0x2E,  0x13,  0x13,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    {
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    { 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, },
    { 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, 
        0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, }
};

/* Special keys in the keymap */
#define TKP_Clear 0xF2
#define TKP_Div 0xFE
#define TKP_Mul 0xFD
#define TKP_Sub 0xFC
#define TKP_Add 0xFB
#define TKP_CapsLock 0xFA
#define TKP_Shift 0xF9
#define TKP_Option 0xF8
#define TKP_Command 0xF7
#define TKP_UpArrow 0xF6
#define TKP_LeftArrow 0xF5
#define TKP_RightArrow 0xF4
#define TKP_DownArrow 0xF3
#define TKP_F1 0xF2
#define TKP_F2 0xF1
#define TKP_F3 0xF0
#define TKP_F4 0xEF
#define TKP_F5 0xEE
#define TKP_F6 0xED
#define TKP_F7 0xEC
#define TKP_F8 0xEB
#define TKP_F9 0xEA
#define TKP_F10 0xE9
#define TKP_F11 0xE8
#define TKP_F12 0xE7
#define TKP_Close 0xE6
#define TKP_InsertDisk 0xE5
#define TKP_Scale 0xE4
#define TKP_ControlMode 0xE3
#define TKP_Bind 0xE2

typedef enum {
    Keyboard_State_Normal = 0,
    Keyboard_State_Shifted,
    Keyboard_State_Capslock
} KeyboardState;

enum {
	DSKey_Left = 0,
	DSKey_Right,
	DSKey_Up,
	DSKey_Down,
	DSKey_Start,
	DSKey_Select,
	DSKey_L,
	DSKey_R,
	DSKey_ZL,
	DSKey_ZR,
	DSKey_A,
	DSKey_B,
	DSKey_X,
	DSKey_Y,
	NumDSKeys
};

LOCALVAR const char* const DSKeyNames[ NumDSKeys ] = {
	"Left",
	"Right",
	"Up",
	"Down",
	"Start",
	"Select",
	"L Trigger",
	"R Trigger",
	"ZL Trigger",
	"ZR Trigger",
	"A",
	"B",
	"X",
	"Y"
};

LOCALVAR KeyboardState KeyboardCurrentState = Keyboard_State_Normal;

/* Set default key bindings to just the arrow keys->dpad */
LOCALVAR int DSKeyMapping[ NumDSKeys ] = {
	TKP_LeftArrow,
	TKP_RightArrow,
	TKP_UpArrow,
	TKP_DownArrow,
	0,	// Start
	0,	// Select
	0,	// L
	0,	// R
	0,	// L2
	0,	// R2
	0x20,
	TKP_Shift
};

LOCALVAR int CurrentKeyDown = 0;

LOCALVAR blnr KeyboardIsInBindMode = falseblnr;
LOCALVAR blnr KeyboardIsUppercase = falseblnr;
LOCALVAR blnr KeyboardIsActive = falseblnr;

rgba32* Keyboard_Lowercase_Image = NULL;
rgba32* Keyboard_Uppercase_Image = NULL;
rgba32* Keyboard_Shift_Image = NULL;
rgba32* Keyboard_Current_Image = NULL;

blnr HaveKeyboardLoaded = falseblnr;

blnr CapsLockState = falseblnr;
blnr ShiftState = falseblnr;
blnr OptionState = falseblnr;
blnr CommandState = falseblnr;

LOCALPROC Keyboard_OnPenDown( touchPosition* TP );
LOCALPROC Keyboard_OnPenUp( touchPosition* TP );
LOCALPROC DoKeyCode( int Key, blnr Down );

LOCALPROC KeyboardStartBind( void ) {
}

LOCALPROC KeyboardBind3DSKey( int DSKey, ui3b TouchKey ) {
	DSKeyMapping[ DSKey ] = TouchKey;
}

LOCALPROC DoBoundKey( int DSKey, int KeyMask ) {
	if ( ( Keys_Down & KeyMask ) && DSKeyMapping[ DSKey ] != 0 )
		DoKeyCode( DSKeyMapping[ DSKey ], trueblnr );
		
	if ( ( Keys_Up & KeyMask ) && DSKeyMapping[ DSKey ] != 0 )
		DoKeyCode( DSKeyMapping[ DSKey ], falseblnr );
}

LOCALPROC KeyboardHandle3DSKeyBinds( void ) {
	DoBoundKey( DSKey_Left, KEY_DLEFT );
	DoBoundKey( DSKey_Right, KEY_DRIGHT );
	DoBoundKey( DSKey_Up, KEY_DUP );
	DoBoundKey( DSKey_Down, KEY_DDOWN );
	DoBoundKey( DSKey_Start, KEY_START );
	DoBoundKey( DSKey_Select, KEY_SELECT );
	DoBoundKey( DSKey_L, KEY_L );
	DoBoundKey( DSKey_R, KEY_R );
	DoBoundKey( DSKey_ZL, KEY_ZL );
	DoBoundKey( DSKey_ZR, KEY_ZR );
	DoBoundKey( DSKey_A, KEY_A );
	DoBoundKey( DSKey_B, KEY_B );
	DoBoundKey( DSKey_X, KEY_X );
	DoBoundKey( DSKey_Y, KEY_Y );
}

LOCALFUNC rgba32* KeyboardGetImage( KeyboardState State ) {
    switch ( State ) {
        case Keyboard_State_Normal:
            return Keyboard_Lowercase_Image;
            break;
        case Keyboard_State_Capslock:
            return Keyboard_Uppercase_Image;
            break;
        case Keyboard_State_Shifted:
            return Keyboard_Shift_Image;
            break;
        default:
            return Keyboard_Lowercase_Image;
            break;
    };
    
    return NULL;
}

LOCALPROC KeyboardSetTexture( rgba32* Image ) {
    memcpy( Keyboard_Current_Image, Image, 512 * 256 * sizeof( rgba32 ) );
    UI_UploadTexture32( Keyboard_Current_Image, &KeyboardTex, 512, 256 );
}

LOCALPROC KeyboardUpdateTexture( void ) {
    KeyboardSetTexture( Keyboard_Current_Image );
}

LOCALPROC KeyboardSetState( KeyboardState State ) {
    KeyboardSetTexture( KeyboardGetImage( State ) );
    KeyboardCurrentState = State;
}

LOCALFUNC blnr Keyboard_Init( void ) {
    int Width = 0;
    int Height = 0;
    
    Keyboard_Uppercase_Image = LoadPNG( "gfx/ui_kb_uc.png", &Width, &Height );
    Keyboard_Lowercase_Image = LoadPNG( "gfx/ui_kb_lc.png", &Width, &Height );
    Keyboard_Shift_Image = LoadPNG( "gfx/ui_kb_shift.png", &Width, &Height );
    Keyboard_Current_Image = AllocImageSpace( 512, 256 );
    
    if ( Keyboard_Lowercase_Image && Keyboard_Uppercase_Image && Keyboard_Shift_Image && Keyboard_Current_Image )
        HaveKeyboardLoaded = trueblnr;
    
    KeyboardSetState( Keyboard_State_Normal );
    
    return trueblnr;
}

LOCALPROC Keyboard_DeInit( void ) {
    if ( Keyboard_Uppercase_Image ) linearFree( Keyboard_Uppercase_Image );
    if ( Keyboard_Lowercase_Image ) linearFree( Keyboard_Lowercase_Image );
    if ( Keyboard_Shift_Image ) linearFree( Keyboard_Shift_Image );
    if ( Keyboard_Current_Image ) linearFree( Keyboard_Current_Image );
    
    C3D_TexDelete( &KeyboardTex );
}

/*
 * This handles mapping the DPAD to the Mac arrow keys.
 * Should be useful for some games.
 */
#if 0
LOCALPROC Keyboard_HandleDPAD( void ) {
    if ( Keys_Down & KEY_DLEFT ) DoKeyCode( TKP_LeftArrow, trueblnr );
    if ( Keys_Up & KEY_DLEFT ) DoKeyCode( TKP_LeftArrow, falseblnr );
    
    if ( Keys_Down & KEY_DRIGHT ) DoKeyCode( TKP_RightArrow, trueblnr );
    if ( Keys_Up & KEY_DRIGHT ) DoKeyCode( TKP_RightArrow, falseblnr );
    
    if ( Keys_Down & KEY_DUP ) DoKeyCode( TKP_UpArrow, trueblnr );
    if ( Keys_Up & KEY_DUP ) DoKeyCode( TKP_UpArrow, falseblnr );
    
    if ( Keys_Down & KEY_DDOWN ) DoKeyCode( TKP_DownArrow, trueblnr );
    if ( Keys_Up & KEY_DDOWN ) DoKeyCode( TKP_DownArrow, falseblnr );
}
#endif

LOCALPROC Keyboard_Update( void ) {
    touchPosition TP;
    
    touchRead( &TP );
    
    if ( Keys_Down & KEY_TOUCH )
        Keyboard_OnPenDown( &TP );
    
    if ( Keys_Up & KEY_TOUCH )
        Keyboard_OnPenUp( &TP );
}

LOCALPROC Keyboard_Toggle( void ) {
    if ( KeyboardIsActive ) {
        /* We don't need to do anything,
         * yet.
         */
    } else {
        /* Reset keyboard to lowercase and unpress any held keys. */
        KeyboardSetState( Keyboard_State_Normal );
        Keyboard_OnPenUp( NULL );
    }
    
    KeyboardIsActive = ! KeyboardIsActive;
}

/* Inverts the colours of the given region of pixels while leaving the alpha channel alone.
 */
LOCALPROC InvertKeyboardPixels( rgba32* Image, int Left, int Right, int Top, int Bottom ) {
    int X = 0;
    int Y = 0;
    
    for ( Y = Top; Y < Bottom; Y++ ) {
        for ( X = Left; X < Right; X++ ) {
            Image[ X + ( Y * 512 ) ].Bits ^= 0xFFFFFF00;
        }
    }
}

LOCALPROC InvertKeyboardTiles( int TileToInvert ) {
    int TileLeftPx = 0;
    int TileTopPx = 0;
    int TileX = 0;
    int TileY = 0;
    
    for ( TileY = 0; TileY < Map_Height; TileY++ ) {
        for ( TileX = 0; TileX < Map_Width; TileX++ ) {
            if ( Keyboard_Map[ TileY ][ TileX ] == TileToInvert ) {
                TileLeftPx = TileX * 8;
                TileTopPx = TileY * 8;
                
                InvertKeyboardPixels( Keyboard_Current_Image, TileLeftPx, TileLeftPx + 8, TileTopPx, TileTopPx + 8 );
            }
        }
    }
    
    KeyboardUpdateTexture( );
}

/* Returns a character from the on screen keyboard map from where
 * the user touched the screen.
 */
LOCALFUNC int KeyFromTouchPoint( int TouchX, int TouchY ) {
    TouchX/= 8;
    TouchY/= 8;
    
    if ( TouchX > 0 && TouchX < Map_Width && TouchY > 0 && TouchY < Map_Height )
        return ( int ) Keyboard_Map[ TouchY ][ TouchX ];
    
    return 0;
}

LOCALVAR int TouchKeyToMac[ 256 ];

LOCALPROC AssignTouchKeyToMac( int TK, si3b MacKey ) {
    TouchKeyToMac[ TK ] = MacKey;
}

LOCALFUNC blnr InitTouchKeyToMac( void ) {
    int i = 0;
    
    for ( i = 0; i < 256; i++ )
        TouchKeyToMac[ i ] = -1;
    
    /* A to Z but not in that order. */
    AssignTouchKeyToMac( 'q', MKC_Q );
    AssignTouchKeyToMac( 'w', MKC_W );
    AssignTouchKeyToMac( 'e', MKC_E );
    AssignTouchKeyToMac( 'r', MKC_R );
    AssignTouchKeyToMac( 't', MKC_T );
    AssignTouchKeyToMac( 'y', MKC_Y );
    AssignTouchKeyToMac( 'u', MKC_U );
    AssignTouchKeyToMac( 'i', MKC_I );
    AssignTouchKeyToMac( 'o', MKC_O );
    AssignTouchKeyToMac( 'p', MKC_P );
    
    AssignTouchKeyToMac( 'a', MKC_A );
    AssignTouchKeyToMac( 's', MKC_S );
    AssignTouchKeyToMac( 'd', MKC_D );
    AssignTouchKeyToMac( 'f', MKC_F );
    AssignTouchKeyToMac( 'g', MKC_G );
    AssignTouchKeyToMac( 'h', MKC_H );
    AssignTouchKeyToMac( 'j', MKC_J );
    AssignTouchKeyToMac( 'k', MKC_K );
    AssignTouchKeyToMac( 'l', MKC_L );
    
    AssignTouchKeyToMac( 'z', MKC_Z );
    AssignTouchKeyToMac( 'x', MKC_X );
    AssignTouchKeyToMac( 'c', MKC_C );
    AssignTouchKeyToMac( 'v', MKC_V );
    AssignTouchKeyToMac( 'b', MKC_B );
    AssignTouchKeyToMac( 'n', MKC_N );
    AssignTouchKeyToMac( 'm', MKC_M );
    
    /* 0 to 9 */
    AssignTouchKeyToMac( '0', MKC_0 );
    AssignTouchKeyToMac( '1', MKC_1 );
    AssignTouchKeyToMac( '2', MKC_2 );
    AssignTouchKeyToMac( '3', MKC_3 );
    AssignTouchKeyToMac( '4', MKC_4 );
    AssignTouchKeyToMac( '5', MKC_5 );
    AssignTouchKeyToMac( '6', MKC_6 );
    AssignTouchKeyToMac( '7', MKC_7 );
    AssignTouchKeyToMac( '8', MKC_8 );
    AssignTouchKeyToMac( '9', MKC_9 );
    
    /* Other keys that probably have a real name */
    AssignTouchKeyToMac( '`', MKC_Grave );
    AssignTouchKeyToMac( '-', MKC_Minus );
    AssignTouchKeyToMac( '=', MKC_Equal );
    AssignTouchKeyToMac( 0x09, MKC_Tab );
    AssignTouchKeyToMac( '[', MKC_LeftBracket );
    AssignTouchKeyToMac( ']', MKC_RightBracket );
    AssignTouchKeyToMac( '\\', MKC_BackSlash );
    AssignTouchKeyToMac( ';', MKC_SemiColon );
    AssignTouchKeyToMac( '\'', MKC_SingleQuote );
    AssignTouchKeyToMac( ',', MKC_Comma );
    AssignTouchKeyToMac( '.', MKC_Period );
    AssignTouchKeyToMac( '/', MKC_Slash );
    
    /* Special keys */
    AssignTouchKeyToMac( ' ', MKC_Space );
    AssignTouchKeyToMac( 0x08, MKC_BackSpace );
    AssignTouchKeyToMac( 0x13, MKC_Return );
    
    /* Arrow keys */
    AssignTouchKeyToMac( TKP_LeftArrow, MKC_Left );
    AssignTouchKeyToMac( TKP_RightArrow, MKC_Right );
    AssignTouchKeyToMac( TKP_UpArrow, MKC_Up );
    AssignTouchKeyToMac( TKP_DownArrow, MKC_Down );
    
    /* Numpad keys */
    AssignTouchKeyToMac( TKP_Add, MKC_KPAdd );
    AssignTouchKeyToMac( TKP_Sub, MKC_KPSubtract );
    AssignTouchKeyToMac( TKP_Mul, MKC_KPMultiply );
    AssignTouchKeyToMac( TKP_Div, MKC_KPDevide );
    AssignTouchKeyToMac( TKP_Clear, MKC_Clear );
    
    /* State keys */
    AssignTouchKeyToMac( TKP_Shift, MKC_Shift );
    AssignTouchKeyToMac( TKP_CapsLock, MKC_CapsLock );
    AssignTouchKeyToMac( TKP_Option, MKC_Option );
    AssignTouchKeyToMac( TKP_Command, MKC_Command );
    
    InitKeyCodes( );
    
    return trueblnr;
}

LOCALVAR blnr KeyboardShiftState = falseblnr;
LOCALVAR blnr KeyboardCapsState = falseblnr;
LOCALVAR blnr KeyboardOptionState = falseblnr;
LOCALVAR blnr KeyboardCommandState = falseblnr;

LOCALPROC ToggleStickyKey( si3b MacKey, blnr Down, blnr* KeyState ) {
    if ( ! KeyState )
        return;
    
    if ( Down == trueblnr ) {
        if ( *KeyState == falseblnr ) Keyboard_UpdateKeyMap2( MacKey, trueblnr );
        else Keyboard_UpdateKeyMap2( MacKey, falseblnr );
        
        *KeyState = ! *KeyState;
    }
}

LOCALPROC ResetSpecialKeys( void ) {
    Keyboard_UpdateKeyMap2( MKC_Shift, falseblnr );
    Keyboard_UpdateKeyMap2( MKC_CapsLock, falseblnr );
    Keyboard_UpdateKeyMap2( MKC_Option, falseblnr );
    Keyboard_UpdateKeyMap2( MKC_Command, falseblnr );
    
    KeyboardShiftState = falseblnr;
    KeyboardCapsState = falseblnr;
    KeyboardOptionState = falseblnr;
    KeyboardCommandState = falseblnr;
}

LOCALPROC DoShift( blnr Down ) {
    if ( Down == trueblnr ) {
        if ( KeyboardShiftState == falseblnr ) {
            KeyboardSetState( Keyboard_State_Shifted );
            InvertKeyboardTiles( TKP_Shift );
            
            ToggleStickyKey( MKC_Shift, trueblnr, &KeyboardShiftState );
        } else {
            KeyboardSetState( Keyboard_State_Normal );
            ToggleStickyKey( MKC_Shift, falseblnr, &KeyboardShiftState );
            
            ResetSpecialKeys( );
        }
    }
}

LOCALPROC DoCapsLock( blnr Down ) {
    if ( Down == trueblnr ) {
        if ( KeyboardCapsState == falseblnr ) {
            KeyboardSetState( Keyboard_State_Capslock );
            InvertKeyboardTiles( TKP_CapsLock );
            
            ToggleStickyKey( MKC_CapsLock, trueblnr, &KeyboardCapsState );
        } else {
            KeyboardSetState( Keyboard_State_Normal );
            ToggleStickyKey( MKC_CapsLock, falseblnr, &KeyboardCapsState );
            
            ResetSpecialKeys( );
        }
    }
}

LOCALPROC DoKeyCode( int Key, blnr Down ) {
    int MacKey = -1;
    
    if ( Key > -1 )
        MacKey = TouchKeyToMac[ Key ];
    
    if ( MacKey >= 0 ) {
        /* Handle keyboard special and state changing keys */
        switch ( MacKey ) {
            case MKC_Shift: {
                DoShift( Down );
                return;
            }
            case MKC_CapsLock: {
                DoCapsLock( Down );
                return;
            }
            case MKC_Option: {
                if ( Down == trueblnr )
                    InvertKeyboardTiles( TKP_Option );
                
                ToggleStickyKey( MKC_Option, Down, &KeyboardOptionState );
                return;
            }
            case MKC_Command: {
                 if ( Down == trueblnr )
                    InvertKeyboardTiles( TKP_Command );
                
                ToggleStickyKey( MKC_Command, Down, &KeyboardCommandState );
                return;
            }
            default: break;
        };
        
        Keyboard_UpdateKeyMap2( MacKey, Down );
        InvertKeyboardTiles( Key );
    }
}

LOCALPROC Keyboard_OnPenDown( touchPosition* TP ) {
    int MapEntry = KeyFromTouchPoint( TP->px, TP->py );
    
    if ( MapEntry != 0 ) {
        DoKeyCode( MapEntry, trueblnr );
        CurrentKeyDown = MapEntry;
    }
}

LOCALPROC Keyboard_OnPenUp( touchPosition* TP ) {
    if ( CurrentKeyDown != -1 && CurrentKeyDown != 0 ) {
        DoKeyCode( CurrentKeyDown, falseblnr );
        CurrentKeyDown = 0;
    }
}

LOCALPROC DisableKeyRepeat(void)
{
}

LOCALPROC RestoreKeyRepeat(void)
{
}

LOCALPROC ReconnectKeyCodes3(void)
{
}

LOCALPROC DisconnectKeyCodes3(void)
{
	DisconnectKeyCodes2();
	MyMouseButtonSet(falseblnr);
}

/* --- time, date, location --- */

#define dbglog_TimeStuff (1 && dbglog_HAVE)

LOCALVAR ui5b TrueEmulatedTime = 0;

#define MyInvTimeDivPow 16
#define MyInvTimeDiv (1 << MyInvTimeDivPow)
#define MyInvTimeDivMask (MyInvTimeDiv - 1)
#define MyInvTimeStep 1089590 /* 1000 / 60.14742 * MyInvTimeDiv */

typedef unsigned int Uint32;

LOCALVAR Uint32 LastTime;

LOCALVAR Uint32 NextIntTime;
LOCALVAR ui5b NextFracTime;

LOCALPROC IncrNextTime(void)
{
	NextFracTime += MyInvTimeStep;
	NextIntTime += (NextFracTime >> MyInvTimeDivPow);
	NextFracTime &= MyInvTimeDivMask;
}

LOCALPROC InitNextTime(void)
{
	NextIntTime = LastTime;
	NextFracTime = 0;
	IncrNextTime();
}

LOCALVAR ui5b NewMacDateInSeconds;

u64 MSAtAppStart = 0;

/*
 * Returns the time in milliseconds since the start of the app
 */
LOCALFUNC u32 GetMS( void ) {
    return ( u32 ) osGetTime( );
}

LOCALFUNC blnr UpdateTrueEmulatedTime(void)
{
	Uint32 LatestTime;
	si5b TimeDiff;

    LatestTime = GetMS( ); //osGetTime( );
	if (LatestTime != LastTime) {

		NewMacDateInSeconds = LatestTime / 1000;
			/* no date and time api in SDL */

		LastTime = LatestTime;
		TimeDiff = (LatestTime - NextIntTime);
			/* this should work even when time wraps */
		if (TimeDiff >= 0) {
			if (TimeDiff > 256) {
				/* emulation interrupted, forget it */
				++TrueEmulatedTime;
				InitNextTime();

#if dbglog_TimeStuff
				dbglog_writelnNum("emulation interrupted",
					TrueEmulatedTime);
#endif
			} else {
				do {
					++TrueEmulatedTime;
					IncrNextTime();
					TimeDiff = (LatestTime - NextIntTime);
				} while (TimeDiff >= 0);
			}
			return trueblnr;
		} else {
			if (TimeDiff < -256) {
#if dbglog_TimeStuff
				dbglog_writeln("clock set back");
#endif
				/* clock goofed if ever get here, reset */
				InitNextTime();
			}
		}
	}
	return falseblnr;
}


LOCALFUNC blnr CheckDateTime(void)
{
	if (CurMacDateInSeconds != NewMacDateInSeconds) {
		CurMacDateInSeconds = NewMacDateInSeconds;
		return trueblnr;
	} else {
		return falseblnr;
	}
}

LOCALPROC StartUpTimeAdjust(void)
{
    LastTime = GetMS( );//osGetTime( );
	InitNextTime();
}

LOCALFUNC blnr InitLocationDat(void)
{
    LastTime = GetMS( ); //osGetTime( );
	InitNextTime();
	NewMacDateInSeconds = LastTime / 1000;
	CurMacDateInSeconds = NewMacDateInSeconds;

	return trueblnr;
}

/* --- sound --- */

#if MySoundEnabled

#define kLn2SoundBuffers 4 /* kSoundBuffers must be a power of two */
#define kSoundBuffers (1 << kLn2SoundBuffers)
#define kSoundBuffMask (kSoundBuffers - 1)

#define DesiredMinFilledSoundBuffs 3
	/*
		if too big then sound lags behind emulation.
		if too small then sound will have pauses.
	*/

#define kLnOneBuffLen 9
#define kLnAllBuffLen (kLn2SoundBuffers + kLnOneBuffLen)
#define kOneBuffLen (1UL << kLnOneBuffLen)
#define kAllBuffLen (1UL << kLnAllBuffLen)
#define kLnOneBuffSz (kLnOneBuffLen + kLn2SoundSampSz - 3)
#define kLnAllBuffSz (kLnAllBuffLen + kLn2SoundSampSz - 3)
#define kOneBuffSz (1UL << kLnOneBuffSz)
#define kAllBuffSz (1UL << kLnAllBuffSz)
#define kOneBuffMask (kOneBuffLen - 1)
#define kAllBuffMask (kAllBuffLen - 1)
#define dbhBufferSize (kAllBuffSz + kOneBuffSz)

#define dbglog_SoundStuff (0 && dbglog_HAVE)
#define dbglog_SoundBuffStats (0 && dbglog_HAVE)

LOCALVAR tpSoundSamp TheSoundBuffer = nullpr;
volatile static ui4b ThePlayOffset;
volatile static ui4b TheFillOffset;
volatile static ui4b MinFilledSoundBuffs;
#if dbglog_SoundBuffStats
LOCALVAR ui4b MaxFilledSoundBuffs;
#endif
LOCALVAR ui4b TheWriteOffset;

LOCALPROC MySound_Init0(void)
{
	ThePlayOffset = 0;
	TheFillOffset = 0;
	TheWriteOffset = 0;
}

LOCALPROC MySound_Start0(void)
{
	/* Reset variables */
	MinFilledSoundBuffs = kSoundBuffers + 1;
#if dbglog_SoundBuffStats
	MaxFilledSoundBuffs = 0;
#endif
}

GLOBALFUNC tpSoundSamp MySound_BeginWrite(ui4r n, ui4r *actL)
{
	ui4b ToFillLen = kAllBuffLen - (TheWriteOffset - ThePlayOffset);
	ui4b WriteBuffContig =
		kOneBuffLen - (TheWriteOffset & kOneBuffMask);

	if (WriteBuffContig < n) {
		n = WriteBuffContig;
	}
	if (ToFillLen < n) {
		/* overwrite previous buffer */
#if dbglog_SoundStuff
		dbglog_writeln("sound buffer over flow");
#endif
		TheWriteOffset -= kOneBuffLen;
	}

	*actL = n;
	return TheSoundBuffer + (TheWriteOffset & kAllBuffMask);
}

#if 4 == kLn2SoundSampSz
LOCALPROC ConvertSoundBlockToNative(tpSoundSamp p)
{
	int i;

	for (i = kOneBuffLen; --i >= 0; ) {
		*p++ -= 0x8000;
	}
}
#else
#define ConvertSoundBlockToNative(p)
#endif

LOCALPROC MySound_WroteABlock(void)
{
#if (4 == kLn2SoundSampSz)
	ui4b PrevWriteOffset = TheWriteOffset - kOneBuffLen;
	tpSoundSamp p = TheSoundBuffer + (PrevWriteOffset & kAllBuffMask);
#endif

#if dbglog_SoundStuff
	dbglog_writeln("enter MySound_WroteABlock");
#endif

	ConvertSoundBlockToNative(p);

	TheFillOffset = TheWriteOffset;

#if dbglog_SoundBuffStats
	{
		ui4b ToPlayLen = TheFillOffset
			- ThePlayOffset;
		ui4b ToPlayBuffs = ToPlayLen >> kLnOneBuffLen;

		if (ToPlayBuffs > MaxFilledSoundBuffs) {
			MaxFilledSoundBuffs = ToPlayBuffs;
		}
	}
#endif
}

LOCALFUNC blnr MySound_EndWrite0(ui4r actL)
{
	blnr v;

	TheWriteOffset += actL;

	if (0 != (TheWriteOffset & kOneBuffMask)) {
		v = falseblnr;
	} else {
		/* just finished a block */

		MySound_WroteABlock();

		v = trueblnr;
	}

	return v;
}

LOCALPROC MySound_SecondNotify0(void)
{
	if (MinFilledSoundBuffs <= kSoundBuffers) {
		if (MinFilledSoundBuffs > DesiredMinFilledSoundBuffs) {
#if dbglog_SoundStuff
			dbglog_writeln("MinFilledSoundBuffs too high");
#endif
			IncrNextTime();
		} else if (MinFilledSoundBuffs < DesiredMinFilledSoundBuffs) {
#if dbglog_SoundStuff
			dbglog_writeln("MinFilledSoundBuffs too low");
#endif
			++TrueEmulatedTime;
		}
#if dbglog_SoundBuffStats
		dbglog_writelnNum("MinFilledSoundBuffs",
			MinFilledSoundBuffs);
		dbglog_writelnNum("MaxFilledSoundBuffs",
			MaxFilledSoundBuffs);
		MaxFilledSoundBuffs = 0;
#endif
		MinFilledSoundBuffs = kSoundBuffers + 1;
	}
}

typedef ui4r trSoundTemp;

#define kCenterTempSound 0x8000

#define AudioStepVal 0x0040

#if 3 == kLn2SoundSampSz
#define ConvertTempSoundSampleFromNative(v) ((v) << 8)
#elif 4 == kLn2SoundSampSz
#define ConvertTempSoundSampleFromNative(v) ((v) + kCenterSound)
#else
#error "unsupported kLn2SoundSampSz"
#endif

#if 3 == kLn2SoundSampSz
#define ConvertTempSoundSampleToNative(v) ((v) >> 8)
#elif 4 == kLn2SoundSampSz
#define ConvertTempSoundSampleToNative(v) ((v) - kCenterSound)
#else
#error "unsupported kLn2SoundSampSz"
#endif

LOCALPROC SoundRampTo(trSoundTemp *last_val, trSoundTemp dst_val,
	tpSoundSamp *stream, int *len)
{
	trSoundTemp diff;
	tpSoundSamp p = *stream;
	int n = *len;
	trSoundTemp v1 = *last_val;

	while ((v1 != dst_val) && (0 != n)) {
		if (v1 > dst_val) {
			diff = v1 - dst_val;
			if (diff > AudioStepVal) {
				v1 -= AudioStepVal;
			} else {
				v1 = dst_val;
			}
		} else {
			diff = dst_val - v1;
			if (diff > AudioStepVal) {
				v1 += AudioStepVal;
			} else {
				v1 = dst_val;
			}
		}

		--n;
		*p++ = ConvertTempSoundSampleToNative(v1);
	}

	*stream = p;
	*len = n;
	*last_val = v1;
}

struct MySoundR {
	tpSoundSamp fTheSoundBuffer;
	volatile ui4b (*fPlayOffset);
	volatile ui4b (*fFillOffset);
	volatile ui4b (*fMinFilledSoundBuffs);

	volatile trSoundTemp lastv;

	blnr wantplaying;
	blnr HaveStartedPlaying;
};
typedef struct MySoundR MySoundR;

static void my_audio_callback(void *udata, Uint8 *stream, int len)
{
	ui4b ToPlayLen;
	ui4b FilledSoundBuffs;
	int i;
	MySoundR *datp = (MySoundR *)udata;
	tpSoundSamp CurSoundBuffer = datp->fTheSoundBuffer;
	ui4b CurPlayOffset = *datp->fPlayOffset;
	trSoundTemp v0 = datp->lastv;
	trSoundTemp v1 = v0;
	tpSoundSamp dst = (tpSoundSamp)stream;

#if kLn2SoundSampSz > 3
	len >>= (kLn2SoundSampSz - 3);
#endif

#if dbglog_SoundStuff
	dbglog_writeln("Enter my_audio_callback");
	dbglog_writelnNum("len", len);
#endif

label_retry:
	ToPlayLen = *datp->fFillOffset - CurPlayOffset;
	FilledSoundBuffs = ToPlayLen >> kLnOneBuffLen;

	if (! datp->wantplaying) {
#if dbglog_SoundStuff
		dbglog_writeln("playing end transistion");
#endif

		SoundRampTo(&v1, kCenterTempSound, &dst, &len);

		ToPlayLen = 0;
	} else if (! datp->HaveStartedPlaying) {
#if dbglog_SoundStuff
		dbglog_writeln("playing start block");
#endif

		if ((ToPlayLen >> kLnOneBuffLen) < 8) {
			ToPlayLen = 0;
		} else {
			tpSoundSamp p = datp->fTheSoundBuffer
				+ (CurPlayOffset & kAllBuffMask);
			trSoundTemp v2 = ConvertTempSoundSampleFromNative(*p);

#if dbglog_SoundStuff
			dbglog_writeln("have enough samples to start");
#endif

			SoundRampTo(&v1, v2, &dst, &len);

			if (v1 == v2) {
#if dbglog_SoundStuff
				dbglog_writeln("finished start transition");
#endif

				datp->HaveStartedPlaying = trueblnr;
			}
		}
	}

	if (0 == len) {
		/* done */

		if (FilledSoundBuffs < *datp->fMinFilledSoundBuffs) {
			*datp->fMinFilledSoundBuffs = FilledSoundBuffs;
		}
	} else if (0 == ToPlayLen) {

#if dbglog_SoundStuff
		dbglog_writeln("under run");
#endif

		for (i = 0; i < len; ++i) {
			*dst++ = ConvertTempSoundSampleToNative(v1);
		}
		*datp->fMinFilledSoundBuffs = 0;
	} else {
		ui4b PlayBuffContig = kAllBuffLen
			- (CurPlayOffset & kAllBuffMask);
		tpSoundSamp p = CurSoundBuffer
			+ (CurPlayOffset & kAllBuffMask);

		if (ToPlayLen > PlayBuffContig) {
			ToPlayLen = PlayBuffContig;
		}
		if (ToPlayLen > len) {
			ToPlayLen = len;
		}

		for (i = 0; i < ToPlayLen; ++i) {
			*dst++ = *p++;
		}
		v1 = ConvertTempSoundSampleFromNative(p[-1]);

		CurPlayOffset += ToPlayLen;
		len -= ToPlayLen;

		*datp->fPlayOffset = CurPlayOffset;

		goto label_retry;
	}

	datp->lastv = v1;
}

LOCALVAR MySoundR cur_audio;

LOCALVAR blnr HaveSoundOut = falseblnr;

LOCALPROC MySound_Stop(void)
{
#if dbglog_SoundStuff
	dbglog_writeln("enter MySound_Stop");
#endif

	if (cur_audio.wantplaying && HaveSoundOut) {
		ui4r retry_limit = 50; /* half of a second */

		cur_audio.wantplaying = falseblnr;

label_retry:
		if (kCenterTempSound == cur_audio.lastv) {
#if dbglog_SoundStuff
			dbglog_writeln("reached kCenterTempSound");
#endif

			/* done */
		} else if (0 == --retry_limit) {
#if dbglog_SoundStuff
			dbglog_writeln("retry limit reached");
#endif
			/* done */
		} else
		{
			/*
				give time back, particularly important
				if got here on a suspend event.
			*/

#if dbglog_SoundStuff
			dbglog_writeln("busy, so sleep");
#endif

			(void) SDL_Delay(10);

			goto label_retry;
		}

		SDL_PauseAudio(1);
	}

#if dbglog_SoundStuff
	dbglog_writeln("leave MySound_Stop");
#endif
}

LOCALPROC MySound_Start(void)
{
	if ((! cur_audio.wantplaying) && HaveSoundOut) {
		MySound_Start0();
		cur_audio.lastv = kCenterTempSound;
		cur_audio.HaveStartedPlaying = falseblnr;
		cur_audio.wantplaying = trueblnr;

		SDL_PauseAudio(0);
	}
}

LOCALPROC MySound_UnInit(void)
{
	if (HaveSoundOut) {
		SDL_CloseAudio();
	}
}

#define SOUND_SAMPLERATE 22255 /* = round(7833600 * 2 / 704) */

LOCALFUNC blnr MySound_Init(void)
{
	SDL_AudioSpec desired;

	MySound_Init0();

	cur_audio.fTheSoundBuffer = TheSoundBuffer;
	cur_audio.fPlayOffset = &ThePlayOffset;
	cur_audio.fFillOffset = &TheFillOffset;
	cur_audio.fMinFilledSoundBuffs = &MinFilledSoundBuffs;
	cur_audio.wantplaying = falseblnr;

	desired.freq = SOUND_SAMPLERATE;

#if 3 == kLn2SoundSampSz
	desired.format = AUDIO_U8;
#elif 4 == kLn2SoundSampSz
	desired.format = AUDIO_S16SYS;
#else
#error "unsupported audio format"
#endif

	desired.channels = 1;
	desired.samples = 1024;
	desired.callback = my_audio_callback;
	desired.userdata = (void *)&cur_audio;

	/* Open the audio device */
	if (SDL_OpenAudio(&desired, NULL) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	} else {
		HaveSoundOut = trueblnr;

		MySound_Start();
			/*
				This should be taken care of by LeaveSpeedStopped,
				but since takes a while to get going properly,
				start early.
			*/
	}

	return trueblnr; /* keep going, even if no sound */
}

GLOBALPROC MySound_EndWrite(ui4r actL)
{
	if (MySound_EndWrite0(actL)) {
	}
}

LOCALPROC MySound_SecondNotify(void)
{
	if (HaveSoundOut) {
		MySound_SecondNotify0();
	}
}

#endif

/* --- basic dialogs --- */

LOCALPROC CheckSavedMacMsg(void)
{
	/* called only on quit, if error saved but not yet reported */

	if (nullpr != SavedBriefMsg) {
		char briefMsg0[ClStrMaxLength + 1];
		char longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg);
		NativeStrFromCStr(longMsg0, SavedLongMsg);

		fprintf(stderr, "%s\n", briefMsg0);
		fprintf(stderr, "%s\n", longMsg0);

		SavedBriefMsg = nullpr;
	}
}

/* --- clipboard --- */

#define UseMotionEvents 1

#if UseMotionEvents
LOCALVAR blnr CaughtMouse = falseblnr;
#endif

#define MouseMaxDelta 10
#define MouseMinDelta -10
#define CPadMaxDelta 3
#define CPadAcceleration 1.5f

LOCALFUNC blnr GetCPadDelta( int* DeltaX, int* DeltaY, blnr UseCPadPro ) {
	circlePosition CPadPos;
	float fdx = 0.0f;
	float fdy = 0.0f;
	
	if ( UseCPadPro == trueblnr )
		irrstCstickRead( &CPadPos );
	else
		hidCircleRead( &CPadPos );
	
	if ( CPadPos.dx > 50 || CPadPos.dx < -50 )
		fdx = ( ( float ) CPadPos.dx ) / 50.0f;
		
	if ( CPadPos.dy > 50 || CPadPos.dy < -50 )
		fdy = ( ( float ) CPadPos.dy ) / 50.0f;
	
	*DeltaX = ( int ) ( fdx * CPadAcceleration );
	*DeltaY = ( int ) -( fdy * CPadAcceleration );
	
	if ( *DeltaX < -CPadMaxDelta ) *DeltaX = -CPadMaxDelta;
	else if ( *DeltaX > CPadMaxDelta ) *DeltaX = CPadMaxDelta;
	
	if ( *DeltaY < -CPadMaxDelta ) *DeltaY = -CPadMaxDelta;
	else if ( *DeltaY > CPadMaxDelta ) *DeltaY = CPadMaxDelta;
	
	return ( *DeltaX != 0 ) || ( *DeltaY != 0 ) ? trueblnr : falseblnr;
}

LOCALFUNC blnr GetTouchAbsolute( int* ABSX, int* ABSY ) {
    float MouseX = 0;
    float MouseY = 0;
    touchPosition TP;
    
    if ( Keys_Held & KEY_TOUCH ) {
        touchRead( &TP );
        
        MouseX = ( ( float ) TP.px / ( float ) MySubScreenWidth ) * ( float ) vMacScreenWidth;
        MouseY = ( ( float ) TP.py / ( float ) MySubScreenHeight ) * ( float ) vMacScreenHeight;
        
        if ( MouseX < 0.0 ) MouseX = 0.0;
        if ( MouseX >= ( float ) vMacScreenWidth ) MouseX = ( float ) vMacScreenWidth - 1.0;
        
        if ( MouseY < 0.0 ) MouseY = 0.0;
        if ( MouseY >= ( float ) vMacScreenWidth ) MouseY = ( float ) vMacScreenHeight - 1.0;
        
        *ABSX = ( int ) MouseX;
        *ABSY = ( int ) MouseY;
        
        return trueblnr;
    }
    
    return falseblnr;
} 

LOCALFUNC blnr IsMouseKeyDown( void ) {
    return ( Keys_Held & KEY_L ) || ( Keys_Held & KEY_R ) ? trueblnr : falseblnr;
}

LOCALPROC HandleMouseMovement( void ) {
    blnr IsDelta = falseblnr;
    int X = 0;
    int Y = 0;
    
    if ( KeyboardIsActive == trueblnr ) {
        /* Keyboard is active, only accept mouse input from the CPad */
        HaveMouseMotion = GetCPadDelta( &X, &Y, falseblnr );
        IsDelta = trueblnr;
        
        /* If no motion from the CirclePad, check the CirclePadPro */
        if ( HaveMouseMotion == falseblnr )
        	HaveMouseMotion = GetCPadDelta( &X, &Y, trueblnr );
    } else {
    	HaveMouseMotion = GetTouchAbsolute( &X, &Y );
        IsDelta = falseblnr;
        
        /* If no touchscreen activity, try the circle pad */
        if ( HaveMouseMotion == falseblnr ) {
            HaveMouseMotion = GetCPadDelta( &X, &Y, falseblnr );
            IsDelta = trueblnr;
            
            if ( HaveMouseMotion == falseblnr )
            	HaveMouseMotion = GetCPadDelta( &X, &Y, trueblnr );
        }
    }
    
    /* Clamp deltas and set mouse movement */
    if ( HaveMouseMotion == trueblnr ) {
        if ( IsDelta == falseblnr ) {
            MyMousePositionSet( X, Y );
        }
        else {
            if ( X < MouseMinDelta ) X = MouseMinDelta;
            if ( X > MouseMaxDelta ) X = MouseMaxDelta;
            
            if ( Y < MouseMinDelta ) Y = MouseMinDelta;
            if ( Y > MouseMaxDelta ) Y = MouseMaxDelta;
            
            MyMousePositionSetDelta( X, Y );
        }
        
        HaveMouseMotion = falseblnr;
    }
}

typedef enum {
    ScaleMode_1to1 = 0,     // No scaling applied
    ScaleMode_FitToWidth,   // Scale to fill the screen horizontally
    ScaleMode_FitToHeight,  // Scale to fill the screen vertically
    ScaleMode_Stretch,      // Stretch display to fit screen horizontally and vertically
    NumScaleModes
} ScreenScaleMode;

#define ScreenCenterX ( MyScreenWidth / 2 )
#define ScreenCenterY ( MyScreenHeight / 2 )

#define MacScreenCenterX ( vMacScreenWidth / 2 )
#define MacScreenCenterY ( vMacScreenHeight / 2 )

int ScreenScrollX = 0;
int ScreenScrollY = 0;

ScreenScaleMode ScaleMode = ScaleMode_1to1;

/* Screen scale factors */
float ScreenScaleW = 1.0f;
float ScreenScaleH = 1.0f;

LOCALPROC ToggleScreenScaleMode( void ) {
    ScaleMode++;
    
    if ( ScaleMode >= NumScaleModes )
        ScaleMode = ScaleMode_1to1;
    
    switch ( ScaleMode ) {
        case ScaleMode_1to1: {
            ScreenScaleW = 1.0f;
            ScreenScaleH = 1.0f;
            
            break;
        }
        case ScaleMode_FitToWidth: {
            ScreenScaleW = ( float ) MyScreenWidth / ( float ) vMacScreenWidth;
            ScreenScaleH = ( float ) MyScreenWidth / ( float ) vMacScreenWidth;
            
            break;
        }
        case ScaleMode_FitToHeight: {
            ScreenScaleW = ( float ) MyScreenHeight / ( float ) vMacScreenHeight;
            ScreenScaleH = ( float ) MyScreenHeight / ( float ) vMacScreenHeight;
            
            break;
        }
        case ScaleMode_Stretch: {
            ScreenScaleW = ( float ) MyScreenWidth / ( float ) vMacScreenWidth;
            ScreenScaleH = ( float ) MyScreenHeight / ( float ) vMacScreenHeight;
            
            break;
        }
        default: {
            ScreenScaleW = 1.0f;
            ScreenScaleH = 1.0f;
            
            break;
        }
    }
    
    /* Linear filtering makes unscaled mode look like crap for some reason.
     * Disable it for this scale mode only.
     */
    if ( ScaleMode == ScaleMode_1to1 ) C3D_TexSetFilter( &FBTexture, GPU_NEAREST, GPU_NEAREST );
    else C3D_TexSetFilter( &FBTexture, GPU_LINEAR, GPU_LINEAR );
    
    /* Reset scrolling offsets */
    ScreenScrollX = 0;
    ScreenScrollY = 0;
    
    // printf( "m: %d w: %.1f h: %.1f\n", ScaleMode, ScreenScaleW, ScreenScaleH );
}

/*
 * TODO:
 * Properly center screen in fit to height mode
 */
LOCALPROC UpdateScreenScroll( void ) {
    float MaxScrollX = ( ( ( float ) vMacScreenWidth ) * ScreenScaleW ) - MyScreenWidth;
    float MaxScrollY = ( ( ( float ) vMacScreenHeight ) * ScreenScaleH ) - MyScreenHeight;
    
    ScreenScrollX = ( ( MyScreenWidth / 2 ) - CurMouseH );
    ScreenScrollY = ( ( MyScreenHeight / 2 ) - CurMouseV );
    
    /* Clamp to the edges of the screen */
    if ( ScaleMode != ScaleMode_FitToHeight ) {
        if ( ScreenScrollX > 0 ) ScreenScrollX = 0;
        if ( ScreenScrollX < -MaxScrollX ) ScreenScrollX = -MaxScrollX;
    
        if ( ScreenScrollY < -MaxScrollY ) ScreenScrollY = -MaxScrollY;
        if ( ScreenScrollY > 0 ) ScreenScrollY = 0;
    } else {
        /* I'm done fiddling with this for now, but at least it's centered */
        ScreenScrollX = 20;
        ScreenScrollY = 0;
    }
    
    //printf( "SW: %d SH: %d\n", ScreenScrollX, ScreenScrollY );
}

LOCALPROC DrawMainScreen( void ) {
    /* Make sure to use nearest filtering for unscaled mode and linear
     * for every other mode.
     */
    if ( ScaleMode == ScaleMode_1to1 ) C3D_TexSetFilter( &FBTexture, GPU_NEAREST, GPU_NEAREST );
    else C3D_TexSetFilter( &FBTexture, GPU_LINEAR, GPU_LINEAR );
    
    GSPGPU_FlushDataCache( TempTextureBuffer, 512 * 512 * 2 );
	C3D_SafeDisplayTransfer( ( u32* ) TempTextureBuffer, GX_BUFFER_DIM( 512, 512 ), ( u32* ) FBTexture.data, GX_BUFFER_DIM( 512, 512 ), TEXTURE_TRANSFER_FLAGS );
    
    C3D_FrameDrawOn( MainRenderTarget );
    C3D_FVUnifMtx4x4( GPU_VERTEX_SHADER, LocProjectionUniforms, &ProjectionMain );
    DrawTexture( &FBTexture, 512, 512, ScreenScrollX, ScreenScrollY, ScreenScaleW, ScreenScaleH );
}

LOCALPROC DrawSubScreen( void ) {
    float SubScaleX = ( ( float ) MySubScreenWidth ) / ( ( float ) vMacScreenWidth );
    float SubScaleY = ( ( float ) MySubScreenHeight ) / ( ( float ) vMacScreenHeight );
    
    /* Always use linear texture filtering for scaled version of main screen. */
    C3D_TexSetFilter( &FBTexture, GPU_LINEAR, GPU_LINEAR );
    
    C3D_FrameDrawOn( SubRenderTarget );
    C3D_FVUnifMtx4x4( GPU_VERTEX_SHADER, LocProjectionUniforms, &ProjectionSub );
    
    if ( KeyboardIsActive ) DrawTexture( &KeyboardTex, 512, 256, 0, 0, 1.0f, 1.0f );
    else DrawTexture( &FBTexture, 512, 512, 0, 0, SubScaleX, SubScaleY );

#ifdef DEBUG_CONSOLE
    if ( Keys_Held & KEY_X ) {
    	DebugConsoleUpdate( );
        DebugConsoleDraw( );
    }
#endif
}

/* --- event handling for main window --- */

LOCALPROC HandleControlMode( void ) {
    static blnr ToggleState = falseblnr;
    
    if ( Keys_Down & KEY_START ) {
        if ( ToggleState == falseblnr ) Keyboard_UpdateKeyMap2( MKC_Control, trueblnr );
        else Keyboard_UpdateKeyMap2( MKC_Control, falseblnr );
        
        ToggleState = ! ToggleState;
    }
}

LOCALPROC Handle3FingerSalute( void ) {
    if ( ( Keys_Held & KEY_L ) && ( Keys_Held & KEY_R ) && ( Keys_Held & KEY_START ) )
   		ForceMacOff = trueblnr;
}

LOCALPROC HandleTheEvent( void ) {
    if ( aptMainLoop( ) ) {
        hidScanInput( );
        irrstScanInput( );
        
        Keys_Down = hidKeysDown( );
        Keys_Up = hidKeysUp( );
        Keys_Held = hidKeysHeld( );
        
        HandleMouseMovement( );
        MyMouseButtonSet( IsMouseKeyDown( ) );
        
        Handle3FingerSalute( );
        HandleControlMode( );
        
        if ( KeyboardIsActive )
            Keyboard_Update( );
        
        if ( Keys_Down & KEY_SELECT )
            ToggleScreenScaleMode( );
        
        /* Only switch to keyboard mode if the graphics were loaded */
        if ( ( Keys_Down & KEY_Y ) && HaveKeyboardLoaded == trueblnr )
            Keyboard_Toggle( );
            
        /* Pressing X should dismiss all emulator messages */
        if ( ( Keys_Down & KEY_X ) ) {
        	MacMsgDisplayOff( );
        }
        
        #if 0
        /* Handle the DPAD arrow keys regardless of if the keyboard is shown */
        Keyboard_HandleDPAD( );
        #endif
        
        /* Handle 3DS->Mac key bindings */
        KeyboardHandle3DSKeyBinds( );
        
        UpdateScreenScroll( );
        
        C3D_FrameBegin( C3D_FRAME_SYNCDRAW );
            DrawMainScreen( );
            DrawSubScreen( );
        C3D_FrameEnd( 0 );
        
        // printf( "dx: %d, dy: %d\n", dx, dy );
        
        //printf( "%d, %d\n", MouseX, MouseY );
        //printf( "time: %d\n", osGetTime( ) );
    } else {
        /* If we're force closing, make sure the emulator exits.
         */
        ForceMacOff = trueblnr;
    }
}

/* --- main window creation and disposal --- */

LOCALVAR int my_argc;
LOCALVAR char **my_argv;

LOCALFUNC blnr Screen_Init(void)
{
    return trueblnr;
}

#if MayFullScreen
LOCALVAR blnr GrabMachine = falseblnr;
#endif

#if MayFullScreen
LOCALPROC GrabTheMachine(void)
{
}
#endif

#if MayFullScreen
LOCALPROC UngrabMachine(void)
{
}
#endif

#if EnableMouseMotion && MayFullScreen
LOCALPROC MyMouseConstrain(void)
{
}
#endif

LOCALFUNC blnr CreateMainWindow(void)
{
#if vMacScreenDepth != 0
    ColorModeWorks = trueblnr;
#endif

    return trueblnr;
}

#if EnableMagnify || VarFullScreen
LOCALFUNC blnr ReCreateMainWindow(void)
{
	return trueblnr;
}
#endif

LOCALPROC ZapWinStateVars(void)
{
}

#if VarFullScreen
LOCALPROC ToggleWantFullScreen(void)
{
	WantFullScreen = ! WantFullScreen;
}
#endif

/* --- SavedTasks --- */

LOCALPROC LeaveBackground(void)
{
	ReconnectKeyCodes3();
	DisableKeyRepeat();
}

LOCALPROC EnterBackground(void)
{
	RestoreKeyRepeat();
	DisconnectKeyCodes3();
}

LOCALPROC LeaveSpeedStopped(void)
{
#if MySoundEnabled
	MySound_Start();
#endif

	StartUpTimeAdjust();
}

LOCALPROC EnterSpeedStopped(void)
{
#if MySoundEnabled
	MySound_Stop();
#endif
}

LOCALPROC CheckForSavedTasks(void)
{
	if (MyEvtQNeedRecover) {
		MyEvtQNeedRecover = falseblnr;

		/* attempt cleanup, MyEvtQNeedRecover may get set again */
		MyEvtQTryRecoverFromFull();
	}

#if EnableMouseMotion && MayFullScreen
	if (HaveMouseMotion) {
		MyMouseConstrain();
	}
#endif

	if (RequestMacOff) {
		RequestMacOff = falseblnr;
		if (AnyDiskInserted()) {
			MacMsgOverride(kStrQuitWarningTitle,
				kStrQuitWarningMessage);
		} else {
			ForceMacOff = trueblnr;
		}
	}

	if (ForceMacOff) {
		return;
	}

	if (gTrueBackgroundFlag != gBackgroundFlag) {
		gBackgroundFlag = gTrueBackgroundFlag;
		if (gTrueBackgroundFlag) {
			EnterBackground();
		} else {
			LeaveBackground();
		}
	}

	if (CurSpeedStopped != (SpeedStopped ||
		(gBackgroundFlag && ! RunInBackground
#if EnableAutoSlow && 0
			&& (QuietSubTicks >= 4092)
#endif
		)))
	{
		CurSpeedStopped = ! CurSpeedStopped;
		if (CurSpeedStopped) {
			EnterSpeedStopped();
		} else {
			LeaveSpeedStopped();
		}
	}

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

#if EnableMagnify || VarFullScreen
	if (0
#if EnableMagnify
		|| (UseMagnify != WantMagnify)
#endif
#if VarFullScreen
		|| (UseFullScreen != WantFullScreen)
#endif
		)
	{
		(void) ReCreateMainWindow();
	}
#endif

#if MayFullScreen
	if (GrabMachine != (
#if VarFullScreen
		UseFullScreen &&
#endif
		! (gTrueBackgroundFlag || CurSpeedStopped)))
	{
		GrabMachine = ! GrabMachine;
		if (GrabMachine) {
			GrabTheMachine();
		} else {
			UngrabMachine();
		}
	}
#endif

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		ScreenChangedAll();
	}
}

/* --- command line parsing --- */

LOCALFUNC blnr ScanCommandLine(void)
{
	return trueblnr;
}

/* --- main program flow --- */

GLOBALFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

LOCALPROC WaitForTheNextEvent(void)
{
}

LOCALPROC CheckForSystemEvents(void)
{
    HandleTheEvent( );
}

void MyDelay( u32 TimeToDelay ) {
    u64 TimeWhenDone = osGetTime( ) + TimeToDelay;
    
    while ( osGetTime( ) < TimeWhenDone );
}

GLOBALPROC WaitForNextTick(void)
{
label_retry:
	CheckForSystemEvents();
	CheckForSavedTasks();

	if (ForceMacOff) {
		return;
	}

	if (CurSpeedStopped) {
		DoneWithDrawingForTick();
		WaitForTheNextEvent();
		goto label_retry;
	}

	if (ExtraTimeNotOver()) {
		MyDelay(NextIntTime - LastTime);
		goto label_retry;
	}

	if (CheckDateTime()) {
#if MySoundEnabled
		MySound_SecondNotify();
#endif
#if EnableDemoMsg
		DemoModeSecondNotify();
#endif
	}

	if ((! gBackgroundFlag)
#if UseMotionEvents
		&& (! CaughtMouse)
#endif
		)
	{
		CheckMouseState();
	}

	OnTrueTime = TrueEmulatedTime;

#if dbglog_TimeStuff
	dbglog_writelnNum("WaitForNextTick, OnTrueTime", OnTrueTime);
#endif
}

/* --- platform independent code can be thought of as going here --- */

#include "PROGMAIN.h"

LOCALPROC ZapOSGLUVars(void)
{
	InitDrives();
	ZapWinStateVars();
}

LOCALPROC ReserveAllocAll(void)
{
#if dbglog_HAVE
	dbglog_ReserveAlloc();
#endif
	ReserveAllocOneBlock(&ROM, kROM_Size, 5, falseblnr);

	ReserveAllocOneBlock(&screencomparebuff,
		vMacScreenNumBytes, 5, trueblnr);
#if UseControlKeys
	ReserveAllocOneBlock(&CntrlDisplayBuff,
		vMacScreenNumBytes, 5, falseblnr);
#endif

#if MySoundEnabled
	ReserveAllocOneBlock((ui3p *)&TheSoundBuffer,
		dbhBufferSize, 5, falseblnr);
#endif

	EmulationReserveAlloc();
}

LOCALFUNC blnr AllocMyMemory(void)
{
	uimr n;
	blnr IsOk = falseblnr;

	ReserveAllocOffset = 0;
	ReserveAllocBigBlock = nullpr;
	ReserveAllocAll();
	n = ReserveAllocOffset;
	ReserveAllocBigBlock = (ui3p)calloc(1, n);
	if (NULL == ReserveAllocBigBlock) {
		MacMsg(kStrOutOfMemTitle, kStrOutOfMemMessage, trueblnr);
	} else {
		ReserveAllocOffset = 0;
		ReserveAllocAll();
		if (n != ReserveAllocOffset) {
			/* oops, program error */
		} else {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}

LOCALPROC UnallocMyMemory(void)
{
	if (nullpr != ReserveAllocBigBlock) {
		free((char *)ReserveAllocBigBlock);
	}
}

LOCALPROC DoN3DSSpeedup( void ) {
	bool Result = false;

    APT_CheckNew3DS( &Result );
    
    if ( Result ) {
        osSetSpeedupEnable( true );
    }
    
    IsNew3DS = ( blnr ) Result;
}

LOCALFUNC blnr InitOSGLU(void)
{
    chdir( "sdmc:/3ds/vmac/" );

    MSAtAppStart = osGetTime( );

    if ( Video_Init( ) )
    if ( Keyboard_Init( ) )
	if (AllocMyMemory())
#if dbglog_HAVE
	if (dbglog_open())
#endif
	if (ScanCommandLine())
	if (LoadInitialImages())
	if (LoadMacRom())
    if ( InitTouchKeyToMac( ) )
	if (InitLocationDat())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (Screen_Init())
	if (CreateMainWindow())
	{
		DoN3DSSpeedup( );
		return trueblnr;
	}

	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

	RestoreKeyRepeat();
#if MayFullScreen
	UngrabMachine();
#endif
#if MySoundEnabled
	MySound_Stop();
#endif
#if MySoundEnabled
	MySound_UnInit();
#endif
#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

#if dbglog_HAVE
	dbglog_close();
#endif

	UnallocMyMemory();

	CheckSavedMacMsg();

    Keyboard_DeInit( );
    Video_Close( );
    
    /* Wait a bit before exiting so we don't accidentally trigger the camera
     * when the home menu comes back up if we're still holding L + R.
     */
    MyDelay( 250 );
}

int main(int argc, char **argv)
{
	my_argc = argc;
	my_argv = argv;

	ZapOSGLUVars();
	if (InitOSGLU()) {
		ProgramMain();
	}
	UnInitOSGLU();

	return 0;
}
