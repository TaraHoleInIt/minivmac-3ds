/*
	OSGLUSDL.c

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
	Operating System GLUe for SDL library

	All operating system dependent code for the
	SDL Library should go here.
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

#include "STRCONST.h"

#include <stdint.h>
#include <unistd.h>

GLOBALPROC DoKeyCode( int Key, blnr Down );

/* Input globals */

LOCALVAR circlePosition CStick;
LOCALVAR touchPosition TouchPad;

LOCALVAR uint32_t Keys_Down = 0;
LOCALVAR uint32_t Keys_Held = 0;
LOCALVAR uint32_t Keys_Up = 0;

/* --- 3DS Framebuffer conversion --- */

#define MainTextureWidth 512
#define MainTextureHeight 512
#define MainTextureSize ( MainTextureWidth * MainTextureHeight * sizeof( uint16_t ) )

#define MainScreenWidth 400
#define MainScreenHeight 240

#define SubScreenWidth 320
#define SubScreenHeight 240

// Cobbled together main framebuffer sprite
LOCALVAR C2D_Sprite ScreenSprite = {
	.image = ( C2D_Image ) {
		.tex = &( C3D_Tex ) {
			// Gets initialized later
		},
		.subtex = &( const Tex3DS_SubTexture ) {
			.width = MainTextureWidth,
			.height = MainTextureHeight,
			.left = 0.0f,
			.right = 1.0f,
			.top = 1.0f,
			.bottom = 0.0f
		}
	},
	.params = ( C2D_DrawParams ) {
		.pos.x = 0.0f,
		.pos.y = 0.0f,
		.pos.w = MainTextureWidth,
		.pos.h = MainTextureHeight,
		.center.x = 1.0f,
		.center.y = 1.0f,
		.depth = 1.0f,
		.angle = 0.0f
	}
};

LOCALVAR C2D_Sprite ScreenSpriteSub = {
	.image = ( C2D_Image ) {
		.tex = &( C3D_Tex ) {
			// Gets initialized later
		},
		.subtex = &( const Tex3DS_SubTexture ) {
			.width = MainTextureWidth,
			.height = MainTextureHeight,
			.left = 0.0f,
			.right = 1.0f,
			.top = 1.0f,
			.bottom = 0.0f
		}
	},
	.params = ( C2D_DrawParams ) {
		.pos.x = 0.0f,
		.pos.y = 0.0f,
		.pos.w = MainTextureWidth,
		.pos.h = MainTextureHeight,
		.center.x = 1.0f,
		.center.y = 1.0f,
		.depth = 1.0f,
		.angle = 0.0f
	}
};

LOCALVAR C3D_RenderTarget* MainRenderTarget = NULL;
LOCALVAR uint16_t* MainScreenBuffer = NULL;

// Copies framebuffer data into the main sprite texture
LOCALPROC UpdateFBTexture( size_t Size ) {
	GSPGPU_FlushDataCache( MainScreenBuffer, Size );

	C3D_SyncDisplayTransfer( 
		( uint32_t* ) MainScreenBuffer, 
		GX_BUFFER_DIM( MainTextureWidth, MainTextureHeight ), 
		ScreenSprite.image.tex->data, 
		GX_BUFFER_DIM( MainTextureWidth, MainTextureHeight ), 
		GX_TRANSFER_OUT_FORMAT( GX_TRANSFER_FMT_RGB565 ) |
		GX_TRANSFER_IN_FORMAT( GX_TRANSFER_FMT_RGB565 ) |
		GX_TRANSFER_OUT_TILED( 1 ) |
		GX_TRANSFER_FLIP_VERT( 0 )
	);	
}

LOCALVAR uint16_t UnpackTable_1BPP[ 256 ][ 8 ];

#if vMacScreenDepth == 1
LOCALVAR uint16_t UnpackTable_2BPP[ 256 ][ 4 ];
#elif vMacScreenDepth == 2
LOCALVAR uint32_t UnpackTable_4BPP[ 256 ];
#elif vMacScreenDepth == 3
LOCALVAR uint16_t ConversionTable_8BPP[ 256 ];
#endif

// Creates a 256 entry lookup table for each combination of pixels within one byte
LOCALPROC Init_1BPP( void ) {
	unsigned int i = 0;

	for ( i = 0; i < 256; i++ ) {
		UnpackTable_1BPP[ i ][ 0 ] = ! ( i & 0x80 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
		UnpackTable_1BPP[ i ][ 1 ] = ! ( i & 0x40 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
		UnpackTable_1BPP[ i ][ 2 ] = ! ( i & 0x20 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
		UnpackTable_1BPP[ i ][ 3 ] = ! ( i & 0x10 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
		UnpackTable_1BPP[ i ][ 4 ] = ! ( i & 0x08 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
		UnpackTable_1BPP[ i ][ 5 ] = ! ( i & 0x04 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
		UnpackTable_1BPP[ i ][ 6 ] = ! ( i & 0x02 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
		UnpackTable_1BPP[ i ][ 7 ] = ! ( i & 0x01 ) ? RGB565( 31, 63, 31 ) : RGB565( 0, 0, 0 );
	}
}

// Converts incoming 1BPP image to RGB565 image data
LOCALPROC Unpack_1BPP( const uint8_t* PixelsIn, uint16_t* PixelsOut, size_t InputLength ) {
	while ( InputLength-- ) {
		memcpy( PixelsOut, UnpackTable_1BPP[ *PixelsIn ], 8 * sizeof( uint16_t ) );

		PixelsOut+= 8;
		PixelsIn++;
	}
}

// Udates the main screen texture with newly converted 1bpp image data
LOCALPROC UpdateMainScreen_1BPP( const uint8_t* Src, int Top, int Bottom ) {
    size_t Size = ( ( Bottom - Top ) * ( vMacScreenWidth / 8 ) );

	Unpack_1BPP(
		&Src[ Top * ( vMacScreenWidth / 8 ) ],
		&MainScreenBuffer[ Top * MainTextureWidth ],
		Size
	);

	UpdateFBTexture( MainTextureSize );
}

#if vMacScreenDepth == 1

// Generates a lookup table which gives 4 RGB565 pixels
// for every 4 2bit pixels given per byte
LOCALPROC Init_2BPP( void ) {
	int r = 0;
	int g = 0;
	int b = 0;
	int i = 0;

	for ( i = 0; i < 256; i++ ) {
		r = CLUT_reds[ i >> 6 ] >> 11;
		g = CLUT_greens[ i >> 6 ] >> 10;
		b = CLUT_blues[ i >> 6 ] >> 11;

		UnpackTable_2BPP[ i ][ 0 ] = RGB565( r, g, b );

		r = CLUT_reds[ ( i >> 4 ) & 0x03 ] >> 11;
		g = CLUT_greens[ ( i >> 4 ) & 0x03 ] >> 10;
		b = CLUT_blues[ ( i >> 4 ) & 0x03 ] >> 11;

		UnpackTable_2BPP[ i ][ 1 ] = RGB565( r, g, b );

		r = CLUT_reds[ ( i >> 2 ) & 0x03 ] >> 11;
		g = CLUT_greens[ ( i >> 2 ) & 0x03 ] >> 10;
		b = CLUT_blues[ ( i >> 2 ) & 0x03 ] >> 11;

		UnpackTable_2BPP[ i ][ 2 ] = RGB565( r, g, b );

		r = CLUT_reds[ i & 0x03 ] >> 11;
		g = CLUT_greens[ i & 0x03 ] >> 10;
		b = CLUT_blues[ i & 0x03 ] >> 11;

		UnpackTable_2BPP[ i ][ 3 ] = RGB565( r, g, b );
	}
}

// Converts incoming 2BPP image data to RGB565
LOCALPROC Unpack2BPP( const uint8_t* PixelsIn, uint16_t* PixelsOut, size_t InputLength ) {
	while ( InputLength-- ) {
		memcpy( PixelsOut, &UnpackTable_2BPP[ *PixelsIn ], 4 * sizeof( uint16_t ) );

		PixelsOut+= 4;
		PixelsIn++;
	}
}

// Updates the main screen texture with a 2bpp framebuffer
LOCALPROC UpdateMainScreen_2BPP( const uint8_t* Src, int Top, int Bottom ) {
	size_t Size = ( ( Bottom - Top ) * ( vMacScreenWidth / 4 ) );

	Unpack2BPP(
		&Src[ Top * ( vMacScreenWidth / 4 ) ],
		&MainScreenBuffer[ Top * MainTextureWidth ],
		Size
	);

	UpdateFBTexture( MainTextureSize );
}
#endif

#if vMacScreenDepth == 2
// Creates a lookup table which converts 2 4bit backed pixels
// per byte into 2 RGB565 pixels
LOCALPROC Init_4BPP( void ) {
	int r = 0;
	int g = 0;
	int b = 0;
	int Lo = 0;
	int Hi = 0;
	int i = 0;

	for ( i = 0; i < 256; i++ ) {
		Lo = ( i & 0x0F );
		Hi = ( i >> 4 );

		r = CLUT_reds[ Lo ] >> 11;
		g = CLUT_greens[ Lo ] >> 10;
		b = CLUT_blues[ Lo ] >> 11;

		UnpackTable_4BPP[ i ] = RGB565( r, g, b ) << 16;

		r = CLUT_reds[ Hi ] >> 11;
		g = CLUT_greens[ Hi ] >> 10;
		b = CLUT_blues[ Hi ] >> 11;

		UnpackTable_4BPP[ i ] |= RGB565( r, g, b );
	}
}

// Converts a 4bit packed framebuffer to RGB565
LOCALPROC Unpack4BPP( const uint8_t* PixelsIn, uint32_t* PixelsOut, size_t InputLength ) {
	while ( InputLength-- ) {
		*PixelsOut++ = UnpackTable_4BPP[ *PixelsIn++ ];
	}
}

// Updates the main screen texture with a 4bpp packed framebuffer
LOCALPROC UpdateMainScreen_4BPP( const uint8_t* Src, int Top, int Bottom ) {
	size_t Size = ( Bottom - Top ) * ( vMacScreenWidth / 2 );

	Unpack4BPP(
		&Src[ Top * ( vMacScreenWidth / 2 ) ],
		&MainScreenBuffer[ Top * MainTextureWidth ],
		Size
	);

	UpdateFBTexture( MainTextureSize );
}
#endif

#if vMacScreenDepth == 3
// Creates the framebuffer conversion table used to convert indexed 8BPP image data to RGB565
LOCALPROC Init_8BPP( void ) {
	int r = 0;
	int g = 0;
	int b = 0;
	int i = 0;

	for ( i = 0; i < 256; i++ ) {
		r = CLUT_reds[ i ] >> 11;
		g = CLUT_greens[ i ] >> 10;
		b = CLUT_blues[ i ] >> 11;

		ConversionTable_8BPP[ i ] = RGB565( r, g, b );
	}
}

// Converts 8bit palettized image data to RGB585
LOCALPROC Unpack8BPP( const uint8_t* PixelsIn, uint16_t* PixelsOut, size_t InputLength ) {
	while ( InputLength-- ) {
		*PixelsOut++ = ConversionTable_8BPP[ *PixelsIn++ ];
	}
}

// Updates the main screen with converted 8bit image data
LOCALPROC UpdateMainScreen_8BPP( const uint8_t* Src, int Top, int Bottom ) {
	size_t Size = ( Bottom - Top ) * vMacScreenWidth;

	Unpack8BPP(
		&Src[ Top * vMacScreenWidth ],
		&MainScreenBuffer[ Top * MainTextureWidth ],
		Size
	);

	UpdateFBTexture( MainTextureSize );
}
#endif

/* --- End of framebuffer conversion */

/* --- Main screen creation, destruction, and drawing --- */
LOCALPROC UpdateScroll( void );

LOCALFUNC bool CreateMainScreen( void ) {
	bool Result = false;

	MainRenderTarget = C2D_CreateScreenTarget( GFX_TOP, GFX_LEFT );

	if ( MainRenderTarget ) {
		MainScreenBuffer = ( uint16_t* ) linearAlloc( MainTextureSize );

		if ( MainScreenBuffer ) {
			memset( MainScreenBuffer, 0, MainTextureSize );

			Result = C3D_TexInit( ScreenSprite.image.tex, 512, 512, GPU_RGB565 );

			if ( Result ) {
				// Point to the main screen image
				memcpy( ScreenSpriteSub.image.tex, ScreenSprite.image.tex, sizeof( C3D_Tex ) );

				// Starts in unscaled mode, this is needed to prevent artifacts
				C3D_TexSetFilter( ScreenSprite.image.tex, GPU_NEAREST, GPU_NEAREST );
				C3D_TexSetFilter( ScreenSpriteSub.image.tex, GPU_LINEAR, GPU_LINEAR );

				C2D_SpriteSetScale( 
					&ScreenSpriteSub,
					( ( float ) SubScreenWidth ) / ( ( float ) vMacScreenWidth ),
					( ( float ) SubScreenHeight ) / ( ( float ) vMacScreenHeight )
				);
			}
		}
	}

	return Result;
}

LOCALPROC DestroyMainScreen( void ) {
	if ( MainScreenBuffer ) {
		linearFree( MainScreenBuffer );
	}

	C3D_TexDelete( ScreenSprite.image.tex );
}

enum {
	Scale_1to1 = 0,
	Scale_Aspect,
	Scale_Fill,
	NumScales
};

int ScaleMode = Scale_1to1;

float ScaleX = 1.0f;
float ScaleY = 1.0f;

LOCALPROC UpdateScale( void );

LOCALPROC DrawMainScreen( void ) {
	UpdateScroll( );
	UpdateScale( );

	C2D_TargetClear( MainRenderTarget, C2D_Color32( 0, 255, 255, 255 ) );
	C2D_SceneBegin( MainRenderTarget );
	C2D_DrawSprite( &ScreenSprite );
}

/* --- End of main screen creation, drawing, and destruction --- */

/* --- Start of sub screen UI --- */

#include "icons_t3x.h"
#include "keyboard_t3x.h"
#include "chicagoflf_bin.h"
#include "titlebar_t3x.h"

#include "touchmap_bin.h"

#define Static_C2D_Color32( r, g, b, a ) ( r | ( g << 8 ) | ( b << 16 ) | ( a << 24 ) )

#define BGColor Static_C2D_Color32( 205, 205, 238, 255 )

#define Icon_Width 48.0f
#define Icons_Start_X 31.0f
#define Icons_Y 31.0f

#define Icon_Menu_X Icons_Start_X
#define Icon_Scale_X Icon_Menu_X + ( Icon_Width * 2 )
#define Icon_Insert_X Icon_Scale_X + ( Icon_Width * 2 )

#define Keyboard_Width 256.0f
#define Keyboard_Height 128.0f

#define Keyboard_Y 97.0f
#define Keyboard_X 32.0f

#define TouchmapWidth ( SubScreenWidth / 8 )
#define TouchmapHeight ( SubScreenHeight / 8 )

enum {
    TM_Scale = 0xC9,
    TM_Insert = 0xCA
};

LOCALVAR C3D_RenderTarget* SubRenderTarget = NULL;

LOCALVAR C2D_SpriteSheet IconSheet;
LOCALVAR C2D_SpriteSheet KeyboardSheet;
LOCALVAR C2D_SpriteSheet TitlebarSheet;

LOCALVAR C2D_Sprite TitlebarSprite;

LOCALVAR C2D_Sprite KB_Lowercase_Sprite;
LOCALVAR C2D_Sprite KB_Uppercase_Sprite;
LOCALVAR C2D_Sprite KB_Shift_Sprite;

LOCALVAR C2D_Sprite ScaleSprite;
LOCALVAR C2D_Sprite MenuSprite;
LOCALVAR C2D_Sprite InsertSprite;

LOCALVAR C2D_Font ChicagoFLF;

LOCALVAR C2D_TextBuf TextBuffer;

LOCALVAR C2D_Text MenubarText;

LOCALVAR float MenubarTextX = 0.0f;
LOCALVAR float MenubarTextY = 0.0f;

LOCALVAR int TouchKeyDown = 0xFF;

LOCALVAR blnr ShiftHeld = falseblnr;
LOCALVAR blnr CommandHeld = falseblnr;
LOCALVAR blnr OptionHeld = falseblnr;
LOCALVAR blnr ControlHeld = falseblnr;
LOCALVAR blnr CapsHeld = falseblnr;
LOCALVAR blnr ScaleHeld = falseblnr;

LOCALVAR blnr HideUI = falseblnr;

#define ScrollMinX 0
#define ScrollMinY 0

#define ScrollMaxX ( int ) -( ( ( float ) vMacScreenWidth * ScaleX ) - MainScreenWidth )
#define ScrollMaxY ( int ) -( ( ( float ) vMacScreenHeight * ScaleY ) - MainScreenHeight )

int ScrollX = 0;
int ScrollY = 0;

LOCALPROC UpdateScroll( void ) {
	ScrollX = ( MainScreenWidth / 2 ) - CurMouseH;
	ScrollY = ( MainScreenHeight / 2 ) - CurMouseV;

	ScrollX = ( ScrollX > 0 ) ? 0 : ScrollX;
	ScrollY = ( ScrollY > 0 ) ? 0 : ScrollY;

	ScrollX = ( ScrollX <= ScrollMaxX ) ? ScrollMaxX : ScrollX;
	ScrollY = ( ScrollY <= ScrollMaxY ) ? ScrollMaxY : ScrollY;

	C2D_SpriteSetPos( &ScreenSprite, ScrollX, ScrollY );
}

LOCALPROC UpdateScale( void ) {
	switch ( ScaleMode ) {
		case Scale_1to1: {
			C3D_TexSetFilter( ScreenSprite.image.tex, GPU_NEAREST, GPU_NEAREST );
			break;
		}
		case Scale_Aspect: {
			C3D_TexSetFilter( ScreenSprite.image.tex, GPU_LINEAR, GPU_LINEAR );
			break;
		}
		case Scale_Fill: {
			C3D_TexSetFilter( ScreenSprite.image.tex, GPU_LINEAR, GPU_LINEAR );
			break;
		}
		default: break;
	};
}

LOCALPROC ToggleScale( void ) {
	ScaleMode++;
	ScaleMode = ( ScaleMode >= NumScales ) ? Scale_1to1 : ScaleMode;

	switch ( ScaleMode ) {
		case Scale_1to1: {
			ScaleX = 1.0f;
			ScaleY = 1.0f;
			break;
		}
		case Scale_Aspect: {
			ScaleX = ( ( float ) MainScreenWidth ) / ( ( float ) vMacScreenWidth );
			ScaleY = ScaleX;
			break;
		}
		case Scale_Fill: {
			ScaleX = ( ( float ) MainScreenWidth ) / ( ( float ) vMacScreenWidth );
			ScaleY = ( ( float ) MainScreenHeight ) / ( ( float ) vMacScreenHeight );
			break;
		}
		default: break;
	};

	C2D_SpriteSetScale( &ScreenSprite, ScaleX, ScaleY );
}

LOCALPROC InsertDisk( void ) {
}

LOCALFUNC bool CreateSubScreen( void ) {
    float w = 0.0f;
    float h = 0.0f;

    SubRenderTarget = C2D_CreateScreenTarget( GFX_BOTTOM, GFX_LEFT );

    if ( SubRenderTarget ) {
        IconSheet = C2D_SpriteSheetLoadFromMem( icons_t3x, icons_t3x_size );
        KeyboardSheet = C2D_SpriteSheetLoadFromMem( keyboard_t3x, keyboard_t3x_size );
        TitlebarSheet = C2D_SpriteSheetLoadFromMem( titlebar_t3x, titlebar_t3x_size );

        if ( IconSheet ) {
            C2D_SpriteFromSheet( &ScaleSprite, IconSheet, 0 );
            C2D_SpriteFromSheet( &MenuSprite, IconSheet, 1 );
            C2D_SpriteFromSheet( &InsertSprite, IconSheet, 2 );

            if ( KeyboardSheet ) {
                C2D_SpriteFromSheet( &KB_Lowercase_Sprite, KeyboardSheet, 0 );
                C2D_SpriteFromSheet( &KB_Uppercase_Sprite, KeyboardSheet, 1 );
                C2D_SpriteFromSheet( &KB_Shift_Sprite, KeyboardSheet, 2 );

                C2D_SpriteSetPos( &MenuSprite, Icon_Menu_X, Icons_Y );
                C2D_SpriteSetPos( &ScaleSprite, Icon_Scale_X, Icons_Y );
                C2D_SpriteSetPos( &InsertSprite, Icon_Insert_X, Icons_Y );

                C2D_SpriteSetPos( &KB_Lowercase_Sprite, Keyboard_X, Keyboard_Y );
                C2D_SpriteSetPos( &KB_Uppercase_Sprite, Keyboard_X, Keyboard_Y );
                C2D_SpriteSetPos( &KB_Shift_Sprite, Keyboard_X, Keyboard_Y );

                if ( TitlebarSheet ) {
                    C2D_SpriteFromSheet( &TitlebarSprite, TitlebarSheet, 0 );

                    ChicagoFLF = C2D_FontLoadFromMem( chicagoflf_bin, chicagoflf_bin_size );

                    if ( ChicagoFLF ) {
                        TextBuffer = C2D_TextBufNew( 4096 );

                        if ( TextBuffer ) {
                            C2D_TextFontParse( &MenubarText, ChicagoFLF, TextBuffer, "Mini vMac 3DS @ " __DATE__ " at " __TIME__ );
                            C2D_TextGetDimensions( &MenubarText, 1.0f, 1.0f, &w, &h );

                            MenubarTextX = ( SubScreenWidth / 2 ) - ( w / 2 );
                            MenubarTextY = 1.0f;

                            C2D_TextOptimize( &MenubarText );

                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

LOCALPROC DestroySubScreen( void ) {
    if ( IconSheet ) {
        C2D_SpriteSheetFree( IconSheet );
    }

    if ( KeyboardSheet ) {
        C2D_SpriteSheetFree( KeyboardSheet );
    }

    if ( TitlebarSheet ) {
        C2D_SpriteSheetFree( TitlebarSheet );
    }

    if ( TextBuffer ) {
        C2D_TextBufDelete( TextBuffer );
    }

    if ( ChicagoFLF ) {
        C2D_FontFree( ChicagoFLF );
    }
}

LOCALPROC DrawKeyHighlight( int Key ) {
    int x = 0;
    int y = 0;

    if ( Key != 0xFF ) {
        for ( x = 0; x < TouchmapWidth; x++ ) {
            for ( y = 0; y < TouchmapHeight; y++ ) {
                if ( touchmap_bin[ x + ( y * TouchmapWidth ) ] == Key ) {
                    C2D_DrawRectangle( 
                        x * 8.0f, 
                        y * 8.0f, 
                        0.5f, 
                        8.0f, 
                        8.0f,
                        C2D_Color32( 0, 0, 0, 64 ),
                        C2D_Color32( 0, 0, 0, 64 ),
                        C2D_Color32( 0, 0, 0, 64 ),
                        C2D_Color32( 0, 0, 0, 64 )
                    );
                }
            }
        }
    }
}

// Forward declaration needed
LOCALVAR ui5b theKeys[ ];

// Highlights all keys that the emulated macintosh sees as down
LOCALPROC HighlightTheDownKeys( void ) {
	uint8_t* KeyboardPtr = ( uint8_t* ) theKeys;
	int MacKey = 0;
	int i = 0;
	int j = 0;

	for ( i = 0; i < 16; i++ ) {
		for ( j = 0; j < 8; j++ ) {
			if ( KeyboardPtr[ i ] & ( 1 << j ) ) {
				MacKey = ( i ) << 3 | j;

				DrawKeyHighlight( MacKey );
			}
		}
	}
}

LOCALPROC DrawSubScreen( void ) {
    C2D_TargetClear( SubRenderTarget, BGColor );
    C2D_SceneBegin( SubRenderTarget );

	if ( HideUI == falseblnr ) {
		C2D_DrawSprite( &TitlebarSprite );

		C2D_DrawSprite( &MenuSprite );
		C2D_DrawSprite( &ScaleSprite );
		C2D_DrawSprite( &InsertSprite );

		if ( ShiftHeld ) {
			C2D_DrawSprite( &KB_Shift_Sprite );
		} else if ( CapsHeld ) {
			C2D_DrawSprite( &KB_Uppercase_Sprite );
		} else {
			C2D_DrawSprite( &KB_Lowercase_Sprite );
		}

		C2D_DrawText( &MenubarText, 0, MenubarTextX, MenubarTextY, 0.5f, 1.0f, 1.0f );

		// This needs special handling?
		DrawKeyHighlight( ControlHeld ? MKC_CM : 0xFF );
		DrawKeyHighlight( ScaleHeld ? TM_Scale : 0xFF );

		HighlightTheDownKeys( );
	} else {
		C2D_DrawSprite( &ScreenSpriteSub );
	}

/*
    DrawKeyHighlight( ShiftHeld ? MKC_Shift : 0xFF );
    DrawKeyHighlight( CommandHeld ? MKC_Command : 0xFF );
    DrawKeyHighlight( OptionHeld ? MKC_Option : 0xFF );
    DrawKeyHighlight( ControlHeld ? MKC_CM : 0xFF );
    DrawKeyHighlight( CapsHeld ? MKC_CapsLock : 0xFF );
*/
}

LOCALVAR int MouseX = 0;
LOCALVAR int MouseY = 0;

LOCALPROC SubScreenUpdateInput( void ) {
    int TouchX = 0;
    int TouchY = 0;
    
	if ( HideUI == false ) {
		if ( Keys_Down & KEY_TOUCH ) {
			TouchX = TouchPad.px / 8;
			TouchY = TouchPad.py / 8;

			TouchX = ( TouchX < 0 ) ? 0 : TouchX;
			TouchY = ( TouchY < 0 ) ? 0 : TouchY;

			TouchX = ( TouchX >= TouchmapWidth ) ? TouchmapWidth - 1 : TouchX;
			TouchY = ( TouchY >= TouchmapHeight ) ? TouchmapHeight - 1 : TouchY;

			TouchKeyDown = touchmap_bin[ TouchX + ( TouchY * TouchmapWidth ) ];

			switch ( TouchKeyDown ) {
				case 0xFF: {
					break;
				}
				case TM_Scale: {
					ToggleScale( );

					ScaleHeld = trueblnr;
					break;
				}
				default: {
					DoKeyCode( TouchKeyDown, trueblnr );
					break;
				}
			};
		}

		if ( Keys_Up & KEY_TOUCH ) {
			switch ( TouchKeyDown ) {
				case 0xFF: {
					break;
				}
				case TM_Scale: {
					ScaleHeld = falseblnr;
					break;
				}
				case TM_Insert: {
					InsertDisk( );
					break;
				}
				case MKC_Shift: {
					ShiftHeld = ! ShiftHeld;

					DoKeyCode( TouchKeyDown, ShiftHeld );
					break;
				}
				case MKC_Option: {
					OptionHeld = ! OptionHeld;

					DoKeyCode( TouchKeyDown, OptionHeld );
					break;
				}
				case MKC_Command: {
					CommandHeld = ! CommandHeld;

					DoKeyCode( TouchKeyDown, CommandHeld );
					break;
				}
				case MKC_CapsLock: {
					CapsHeld = ! CapsHeld;

					DoKeyCode( TouchKeyDown, CapsHeld );
					break;
				}
				case MKC_CM: {
					ControlHeld = ! ControlHeld;

					DoKeyCode( TouchKeyDown, ControlHeld );
					break;
				}
				default: {
					DoKeyCode( TouchKeyDown, falseblnr );
					break;
				}
			};

			TouchKeyDown = 0xFF;
		}
	} else {
		// Mouse mode
		if ( Keys_Held & KEY_TOUCH ) {
			MouseX = TouchPad.px * ( ( ( float ) vMacScreenWidth ) / ( ( float ) SubScreenWidth ) );
			MouseY = TouchPad.py * ( ( ( float ) vMacScreenHeight ) / ( ( float ) SubScreenHeight ) );
		}
	}
}

/* --- End of sub screen UI --- */

/* --- some simple utilities --- */

GLOBALOSGLUPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- control mode and internationalization --- */

#define NeedCell2PlainAsciiMap 1

#include "INTLCHAR.h"

/* --- sending debugging info to file --- */

#if dbglog_HAVE

#define dbglog_ToStdErr 0

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

/* --- information about the environment --- */

#define WantColorTransValid 0

#include "COMOSGLU.h"

#include "PBUFSTDC.h"

#include "CONTROLM.h"

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

GLOBALOSGLUFUNC tMacErr vSonyTransfer(blnr IsWrite, ui3p Buffer,
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

GLOBALOSGLUFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
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

GLOBALOSGLUFUNC tMacErr vSonyEject(tDrive Drive_No)
{
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
				MacMsgOverride(kStrShortROMTitle,
					kStrShortROMMessage);
				err = mnvm_eofErr;
			} else {
				MacMsgOverride(kStrNoReadROMTitle,
					kStrNoReadROMMessage);
				err = mnvm_miscErr;
			}
		} else {
			err = ROM_IsValid();
		}
		fclose(ROM_File);
	}

	return err;
}

#if 0 /* no drag and drop to make use of this */
LOCALFUNC blnr Sony_Insert1a(char *drivepath, blnr silentfail)
{
	blnr v;

	if (! ROM_loaded) {
		v = (mnvm_noErr == LoadMacRomFrom(drivepath));
	} else {
		v = Sony_Insert1(drivepath, silentfail);
	}

	return v;
}
#endif

LOCALFUNC blnr Sony_Insert2(char *s)
{
	return Sony_Insert1(s, trueblnr);
}

LOCALFUNC blnr Sony_InsertIth(int i)
{
	blnr v;

	if ((i > 9) || ! FirstFreeDisk(nullpr)) {
		v = falseblnr;
	} else {
		char s[] = "disk?.dsk";

		s[4] = '0' + i;

		v = Sony_Insert2(s);
	}

	return v;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	if (! AnyDiskInserted()) {
		int i;

		for (i = 1; Sony_InsertIth(i); ++i) {
			/* stop on first error (including file not found) */
		}
	}

	return trueblnr;
}

/* --- ROM --- */

LOCALVAR char *rom_path = NULL;

LOCALFUNC blnr LoadMacRom(void)
{
	tMacErr err;

	if ((NULL == rom_path)
		|| (mnvm_fnfErr == (err = LoadMacRomFrom(rom_path))))
	if (mnvm_fnfErr == (err = LoadMacRomFrom(RomFileName)))
	{
	}

	return trueblnr; /* keep launching Mini vMac, regardless */
}

/* --- video out --- */
LOCALVAR blnr CurSpeedStopped = trueblnr;

#if vMacScreenDepth == 0
	#define UpdateMainScreen UpdateMainScreen_1BPP
#elif vMacScreenDepth == 1
	#define UpdateMainScreen UpdateMainScreen_2BPP
#elif vMacScreenDepth == 2
	#define UpdateMainScreen UpdateMainScreen_4BPP
#elif vMacScreenDepth == 3
	#define UpdateMainScreen UpdateMainScreen_8BPP
#else
	#error Unsupported color depth
#endif

#if vMacScreenDepth == 0
	#define UpdateColormap Init_1BPP
#elif vMacScreenDepth == 1
	#define UpdateColormap Init_2BPP
#elif vMacScreenDepth == 2
	#define UpdateColormap Init_4BPP
#elif vMacScreenDepth == 3
	#define UpdateColormap Init_8BPP
#else
	#error Unsupported color depth
#endif

#if vMacScreenDepth > 0
LOCALVAR blnr LastUseColorMode = 0xFF; // force a change

LOCALVAR uint32_t LastColormapSum = 0;

uint32_t GetColormapChecksum( void ) {
	uint32_t Sum = 0;
#if vMacScreenDepth > 0
	int i = 0;

	for ( i = 0; i < CLUT_size; i++ ) {
		Sum+= CLUT_reds[ i ];
		Sum+= CLUT_greens[ i ];
		Sum+= CLUT_blues[ i ];
	}
#endif
	return Sum;
}

LOCALPROC CheckAndUpdateColormap( void ) {
	uint32_t Sum = 0;

	if ( UseColorMode ) {
		Sum = GetColormapChecksum( );

		if ( Sum != LastColormapSum || ColorMappingChanged ) {
			LastColormapSum = Sum;
			NeedWholeScreenDraw = trueblnr;

			UpdateColormap( );
		}
	}
}
#endif

LOCALPROC HaveChangedScreenBuff(ui4r top, ui4r left,
	ui4r bottom, ui4r right)
{
	const uint8_t* Src = ( const uint8_t* ) GetCurDrawBuff( );

#if vMacScreenDepth > 0
	if ( UseColorMode == false ) {
		UpdateMainScreen_1BPP( Src, top, bottom );
	} else {
		CheckAndUpdateColormap( );
		UpdateMainScreen( Src, top, bottom );
	}
#else
	UpdateMainScreen( Src, top, bottom );
#endif
}

LOCALPROC MyDrawChangesAndClear(void)
{
	if (ScreenChangedBottom > ScreenChangedTop) {
		HaveChangedScreenBuff(ScreenChangedTop, ScreenChangedLeft,
			ScreenChangedBottom, ScreenChangedRight);
		ScreenClearChanges();
	}
}

GLOBALOSGLUPROC DoneWithDrawingForTick(void)
{
#if EnableFSMouseMotion
	if (HaveMouseMotion) {
		AutoScrollScreen();
	}
#endif
	MyDrawChangesAndClear();
}

/* --- mouse --- */

/* cursor moving */

LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	//tara SDL_WarpMouse(h, v);

	return trueblnr;
}

/* cursor state */

LOCALPROC MousePositionNotify(int NewMousePosh, int NewMousePosv)
{
	{
		if (NewMousePosh < 0) {
			NewMousePosh = 0;
		} else if (NewMousePosh >= vMacScreenWidth) {
			NewMousePosh = vMacScreenWidth - 1;
		}
		if (NewMousePosv < 0) {
			NewMousePosv = 0;
		} else if (NewMousePosv >= vMacScreenHeight) {
			NewMousePosv = vMacScreenHeight - 1;
		}

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/*
			for a game like arkanoid, would like mouse to still
			move even when outside window in one direction
		*/
		MyMousePositionSet(NewMousePosh, NewMousePosv);
	}
}

LOCALPROC CheckMouseState(void)
{
	/*
		this doesn't work as desired, doesn't get mouse movements
		when outside of our window.
	*/
	//int x;
	//int y;
	float x = 0.0f;
	float y = 0.0f;

	//(void) SDL_GetMouseState(&x, &y);
	//MousePositionNotify(x, y);

	if ( fabs( CStick.dx ) >= 15.0f || fabs( CStick.dy ) >= 15.0f ) {
		x = CStick.dx / 127.0f;
		y = CStick.dy / 127.0f;

		MouseX+= x * 4;
		MouseY-= y * 4;

		MouseX = ( MouseX < 0 ) ? 0 : MouseX;
		MouseY = ( MouseY < 0 ) ? 0 : MouseY;

		MouseX = ( MouseX >= vMacScreenWidth ) ? vMacScreenWidth - 1 : MouseX;
		MouseY = ( MouseY >= vMacScreenHeight ) ? vMacScreenHeight - 1 : MouseY;
	}

	MousePositionNotify( MouseX, MouseY );
	MyMouseButtonSet( ( Keys_Held & KEY_L ) || ( Keys_Held & KEY_R ) );
}

/* --- keyboard input --- */

GLOBALPROC ToggleWantFullScreen( void ) {
	// This does nothing
}

GLOBALPROC DoKeyCode( int Key, blnr Down )
{
	if ( Key != MKC_None ) {
		Keyboard_UpdateKeyMap2( Key, Down );
	}
}

/* --- time, date, location --- */

#define dbglog_TimeStuff (0 && dbglog_HAVE)

LOCALVAR ui5b TrueEmulatedTime = 0;

#define MyInvTimeDivPow 16
#define MyInvTimeDiv (1 << MyInvTimeDivPow)
#define MyInvTimeDivMask (MyInvTimeDiv - 1)
#define MyInvTimeStep 1089590 /* 1000 / 60.14742 * MyInvTimeDiv */

LOCALVAR uint64_t LastTime;

LOCALVAR uint64_t NextIntTime;
LOCALVAR uint64_t NextFracTime;

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

LOCALFUNC blnr UpdateTrueEmulatedTime(void)
{
	uint32_t LatestTime;
	si5b TimeDiff;

	LatestTime = osGetTime( );
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
	LastTime = osGetTime( );
	InitNextTime();
}

LOCALFUNC blnr InitLocationDat(void)
{
	LastTime = osGetTime( );
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

#define DesiredMinFilledSoundBuffs 2
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

GLOBALOSGLUFUNC tpSoundSamp MySound_BeginWrite(ui4r n, ui4r *actL)
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
#if 0
#define ConvertSoundBlockToNative(p)
#else
LOCALPROC ConvertSoundBlockToNative( tpSoundSamp Ptr ) {
	int i = 0;

	for ( i = 0; i < kOneBuffLen; i++ ) {
		*Ptr++-= 0x80;
	}
}
#endif
#endif

LOCALPROC MySound_WroteABlock(void)
{
#if (4 == kLn2SoundSampSz || 1)
	ui4b PrevWriteOffset = TheWriteOffset - kOneBuffLen;
	tpSoundSamp p = TheSoundBuffer + (PrevWriteOffset & kAllBuffMask);
#endif

#if dbglog_SoundStuff
	dbglog_writeln("enter MySound_WroteABlock");
#endif

	ConvertSoundBlockToNative( ( tpSoundSamp ) p );

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

static void my_audio_callback(void *udata, uint8_t *stream, int len)
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

#define SampleCount 2048

LOCALVAR ndspWaveBuf WaveBuffers[ 2 ];
LOCALVAR int CurrentWaveBuf = 0;

LOCALPROC MySound_Stop(void)
{
#if 0
	ndspChnWaveBufClear( &WaveBuffers[ 0 ] );
	ndspChnWaveBufClear( &WaveBuffers[ 1 ] );
#endif

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

			//(void) SDL_Delay(10);

			goto label_retry;
		}

		//SDL_PauseAudio(1);
		ndspChnSetPaused( 0, true );
	}

#if dbglog_SoundStuff
	dbglog_writeln("leave MySound_Stop");
#endif
}

LOCALVAR void* SoundBuffer = NULL;

LOCALPROC MySound_Start(void)
{
	if ((! cur_audio.wantplaying) && HaveSoundOut) {
		MySound_Start0();
		cur_audio.lastv = kCenterTempSound;
		cur_audio.HaveStartedPlaying = falseblnr;
		cur_audio.wantplaying = trueblnr;

		ndspChnWaveBufAdd( 0, &WaveBuffers[ 0 ] );
		ndspChnWaveBufAdd( 0, &WaveBuffers[ 1 ] );
	}
}

LOCALPROC MySound_UnInit(void)
{
	ndspChnWaveBufClear( 0 );

	if ( SoundBuffer ) {
		linearFree( SoundBuffer );
	}

	ndspExit( );
}

#define SOUND_SAMPLERATE 22255 /* = round(7833600 * 2 / 704) */

LOCALPROC DSPCallback( void* Param ) {
#if kLn2SoundSampSz == 3
	const size_t Size = SampleCount;
#else
	const size_t Size = SampleCount * 2;
#endif

	if ( HaveSoundOut ) {
		if ( WaveBuffers[ CurrentWaveBuf ].status == NDSP_WBUF_DONE ) {
			my_audio_callback( Param, ( uint8_t* ) WaveBuffers[ CurrentWaveBuf ].data_vaddr, Size );
			DSP_FlushDataCache( WaveBuffers[ CurrentWaveBuf ].data_vaddr, Size );

			ndspChnWaveBufAdd( 0, &WaveBuffers[ CurrentWaveBuf ] );
			CurrentWaveBuf = ! CurrentWaveBuf;
		}
	}	
}

LOCALFUNC blnr MySound_Init(void)
{
#if kLn2SoundSampSz == 3
	const size_t Size = SampleCount * 2;
	const uint16_t Format = NDSP_FORMAT_MONO_PCM8;
#else
	const size_t Size = SampleCount * 2 * 2;
	const uint16_t Format = NDSP_FORMAT_MONO_PCM16;
#endif

	float Mix[ 12 ] = { 
		1.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0
	};

	MySound_Init0();

	cur_audio.fTheSoundBuffer = TheSoundBuffer;
	cur_audio.fPlayOffset = &ThePlayOffset;
	cur_audio.fFillOffset = &TheFillOffset;
	cur_audio.fMinFilledSoundBuffs = &MinFilledSoundBuffs;
	cur_audio.wantplaying = falseblnr;

	if ( R_SUCCEEDED( ndspInit( ) ) ) {
		SoundBuffer = ( uint8_t* ) linearAlloc( Size );

		if ( SoundBuffer != NULL ) {
			memset( SoundBuffer, 0, Size );
			memset( WaveBuffers, 0, sizeof( WaveBuffers ) );

			ndspSetOutputMode( NDSP_OUTPUT_MONO );
			ndspChnSetInterp( 0, NDSP_INTERP_LINEAR );
			ndspChnSetRate( 0, SOUND_SAMPLERATE );
			ndspChnSetFormat( 0, Format );
			ndspChnSetMix( 0, Mix );

			WaveBuffers[ 0 ].data_vaddr = SoundBuffer;
			WaveBuffers[ 0 ].nsamples = SampleCount;

#if kLn2SoundSampSz == 3
			WaveBuffers[ 1 ].data_vaddr = ( ( uint8_t* ) SoundBuffer ) + SampleCount;
#else
			WaveBuffers[ 1 ].data_vaddr = ( ( uint16_t* ) SoundBuffer ) + SampleCount;
#endif
			WaveBuffers[ 1 ].nsamples = SampleCount;

			HaveSoundOut = trueblnr;
			MySound_Start( );

			ndspSetCallback( DSPCallback, &cur_audio );
		} else {
			MacMsg( "Sound", "Failed to allocate sample buffer", trueblnr );
		}
	} else {
		MacMsg( "Sound", "Missing DSP firmware dump. Sound is disabled.", falseblnr );
	}
#ifdef TARA
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
#endif
	return trueblnr; /* keep going, even if no sound */
}

GLOBALOSGLUPROC MySound_EndWrite(ui4r actL)
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

/* --- event handling for main window --- */

/* --- main window creation and disposal --- */

LOCALVAR int my_argc;
LOCALVAR char **my_argv;

LOCALFUNC blnr Screen_Init(void)
{
	gfxInitDefault( );
	//consoleInit( GFX_BOTTOM, GFX_LEFT );

	C3D_Init( C3D_DEFAULT_CMDBUF_SIZE );
	C2D_Init( C2D_DEFAULT_MAX_OBJECTS );
	C2D_Prepare( );

	// Always build 1bpp conversion table
	Init_1BPP( );

#if vMacScreenDepth > 0
	// Force build colormap
	ColorMappingChanged = trueblnr;
	ColorModeWorks = trueblnr;

	CheckAndUpdateColormap( );
#endif

	return trueblnr;
}

#if EnableFSMouseMotion
LOCALPROC MyMouseConstrain(void)
{
	si4b shiftdh;
	si4b shiftdv;

	if (SavedMouseH < ViewHStart + (ViewHSize / 4)) {
		shiftdh = ViewHSize / 2;
	} else if (SavedMouseH > ViewHStart + ViewHSize - (ViewHSize / 4)) {
		shiftdh = - ViewHSize / 2;
	} else {
		shiftdh = 0;
	}
	if (SavedMouseV < ViewVStart + (ViewVSize / 4)) {
		shiftdv = ViewVSize / 2;
	} else if (SavedMouseV > ViewVStart + ViewVSize - (ViewVSize / 4)) {
		shiftdv = - ViewVSize / 2;
	} else {
		shiftdv = 0;
	}
	if ((shiftdh != 0) || (shiftdv != 0)) {
		SavedMouseH += shiftdh;
		SavedMouseV += shiftdv;
		if (! MyMoveMouse(SavedMouseH, SavedMouseV)) {
			HaveMouseMotion = falseblnr;
		}
	}
}
#endif

LOCALFUNC blnr CreateMainWindow(void)
{
	return CreateMainScreen( ) && CreateSubScreen( );
}

LOCALPROC ZapWinStateVars(void)
{
}

/* --- SavedTasks --- */

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

#if EnableFSMouseMotion
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

	if (CurSpeedStopped != (SpeedStopped ||
		(0 && ! 1
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

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		ScreenChangedAll();
	}

#if NeedRequestIthDisk
	if (0 != RequestIthDisk) {
		Sony_InsertIth(RequestIthDisk);
		RequestIthDisk = 0;
	}
#endif
}

/* --- command line parsing --- */

LOCALFUNC blnr ScanCommandLine(void)
{
	char *pa;
	int i = 1;

label_retry:
	if (i < my_argc) {
		pa = my_argv[i++];
		if ('-' == pa[0]) {
			if ((0 == strcmp(pa, "--rom"))
				|| (0 == strcmp(pa, "-r")))
			{
				if (i < my_argc) {
					rom_path = my_argv[i++];
					goto label_retry;
				}
			} else
			{
				MacMsg(kStrBadArgTitle, kStrBadArgMessage, falseblnr);
			}
		} else {
			(void) Sony_Insert1(pa, falseblnr);
			goto label_retry;
		}
	}

	return trueblnr;
}

/* --- main program flow --- */

GLOBALOSGLUFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

LOCALPROC WaitForTheNextEvent(void)
{
}

LOCALPROC HandleDPADKey( int DSKey, int MacKey ) {
	if ( Keys_Held & DSKey ) {
		DoKeyCode( MacKey, trueblnr );
	} else {
		DoKeyCode( MacKey, falseblnr );
	}
}

LOCALPROC CheckForSystemEvents(void)
{
	if ( aptMainLoop( ) ) {
		hidScanInput( );

		Keys_Held = hidKeysHeld( );
		Keys_Down = hidKeysDown( );
		Keys_Up = hidKeysUp( );

		hidTouchRead( &TouchPad );
		hidCircleRead( &CStick );

		HandleDPADKey( KEY_DLEFT, MKC_Left );
		HandleDPADKey( KEY_DRIGHT, MKC_Right );
		HandleDPADKey( KEY_DUP, MKC_Up );
		HandleDPADKey( KEY_DDOWN, MKC_Down );

		SubScreenUpdateInput( );

		if ( ( Keys_Up & KEY_START ) && ( Keys_Up && KEY_SELECT ) ) {
			ForceMacOff = trueblnr;
		}

		if ( Keys_Up & KEY_SELECT ) {
			HideUI = ! HideUI;
		}

		if ( C3D_FrameBegin( C3D_FRAME_NONBLOCK ) == true ) {
			DrawMainScreen( );
			DrawSubScreen( );
			C3D_FrameEnd( 0 );
		}
	}
}

GLOBALOSGLUPROC WaitForNextTick(void)
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
		// tara
		//svcSleepThread( ( NextIntTime - LastTime ) * 1000000 );
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

	CheckMouseState( );

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

LOCALFUNC void VBlankEvent( void* Param ) {
}

LOCALFUNC blnr InitOSGLU(void)
{
	chdir( "/3ds/minivmac/" );
	osSetSpeedupEnable( true );

	InitKeyCodes( );

	if (AllocMyMemory())
#if dbglog_HAVE
	if (dbglog_open())
#endif
	if (ScanCommandLine())
	if (LoadMacRom())
	if (LoadInitialImages())
	if (InitLocationDat())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (Screen_Init())
	if (CreateMainWindow())
	if (WaitForRom())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

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

	DestroyMainScreen( );
	DestroySubScreen( );

	C2D_Fini( );
	C3D_Fini( );

	gfxExit( );
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
