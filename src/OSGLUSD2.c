/*
	OSGLUSD2.c

	Copyright (C) 2012 Paul C. Pratt, Manuel Alfayate

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
	Operating System GLUe for SDl 2.0 library

	All operating system dependent code for the
	SDL Library should go here.
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

#include "STRCONST.h"

/* --- some simple utilities --- */

GLOBALOSGLUPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- control mode and internationalization --- */

#define NeedCell2PlainAsciiMap 1

#include "INTLCHAR.h"

#ifndef CanGetAppPath
#define CanGetAppPath 1
#endif

LOCALVAR char *d_arg = NULL;
LOCALVAR char *n_arg = NULL;

#if CanGetAppPath
LOCALVAR char *app_parent = NULL;
LOCALVAR char *pref_dir = NULL;
#endif

#ifdef _WIN32
#define MyPathSep '\\'
#else
#define MyPathSep '/'
#endif

LOCALFUNC tMacErr ChildPath(char *x, char *y, char **r)
{
	tMacErr err = mnvm_miscErr;
	int nx = strlen(x);
	int ny = strlen(y);
	{
		if ((nx > 0) && (MyPathSep == x[nx - 1])) {
			--nx;
		}
		{
			int nr = nx + 1 + ny;
			char *p = malloc(nr + 1);
			if (p != NULL) {
				char *p2 = p;
				(void) memcpy(p2, x, nx);
				p2 += nx;
				*p2++ = MyPathSep;
				(void) memcpy(p2, y, ny);
				p2 += ny;
				*p2 = 0;
				*r = p;
				err = mnvm_noErr;
			}
		}
	}

	return err;
}

LOCALPROC MyMayFree(char *p)
{
	if (NULL != p) {
		free(p);
	}
}

/* --- sending debugging info to file --- */

#if dbglog_HAVE

#ifndef dbglog_ToStdErr
#define dbglog_ToStdErr 0
#endif
#ifndef dbglog_ToSDL_Log
#define dbglog_ToSDL_Log 0
#endif

#if ! dbglog_ToStdErr
LOCALVAR FILE *dbglog_File = NULL;
#endif

LOCALFUNC blnr dbglog_open0(void)
{
#if dbglog_ToStdErr || dbglog_ToSDL_Log
	return trueblnr;
#else
	if (NULL == app_parent) {
		dbglog_File = fopen("dbglog.txt", "w");
	} else {
		char *t;

		if (mnvm_noErr == ChildPath(app_parent, "dbglog.txt", &t)) {
			dbglog_File = fopen(t, "w");
			free(t);
		}
	}

	return (NULL != dbglog_File);
#endif
}

LOCALPROC dbglog_write0(char *s, uimr L)
{
#if dbglog_ToStdErr
	(void) fwrite(s, 1, L, stderr);
#elif dbglog_ToSDL_Log
	char t[256 + 1];

	if (L > 256) {
		L = 256;
	}
	(void) memcpy(t, s, L);
	t[L] = 1;

	SDL_Log("%s", t);
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

#ifndef UseRWops
#define UseRWops 0
#endif

#if UseRWops
#define MyFilePtr SDL_RWops *
#define MySeek SDL_RWseek
#define MySeekSet RW_SEEK_SET
#define MySeekCur RW_SEEK_CUR
#define MySeekEnd RW_SEEK_END
#define MyFileRead(ptr, size, nmemb, stream) \
	SDL_RWread(stream, ptr, size, nmemb)
#define MyFileWrite(ptr, size, nmemb, stream) \
	SDL_RWwrite(stream, ptr, size, nmemb)
#define MyFileTell SDL_RWtell
#define MyFileClose SDL_RWclose
#define MyFileOpen SDL_RWFromFile
#else
#define MyFilePtr FILE *
#define MySeek fseek
#define MySeekSet SEEK_SET
#define MySeekCur SEEK_CUR
#define MySeekEnd SEEK_END
#define MyFileRead fread
#define MyFileWrite fwrite
#define MyFileTell ftell
#define MyFileClose fclose
#define MyFileOpen fopen
#define MyFileEof feof
#endif

LOCALVAR MyFilePtr Drives[NumDrives]; /* open disk image files */

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
	MyFilePtr refnum = Drives[Drive_No];
	ui5r NewSony_Count = 0;

	if (MySeek(refnum, Sony_Start, MySeekSet) >= 0) {
		if (IsWrite) {
			NewSony_Count = MyFileWrite(Buffer, 1, Sony_Count, refnum);
		} else {
			NewSony_Count = MyFileRead(Buffer, 1, Sony_Count, refnum);
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
	MyFilePtr refnum = Drives[Drive_No];
	long v;

	if (MySeek(refnum, 0, MySeekEnd) >= 0) {
		v = MyFileTell(refnum);
		if (v >= 0) {
			*Sony_Count = v;
			err = mnvm_noErr;
		}
	}

	return err; /*& figure out what really to return &*/
}

LOCALFUNC tMacErr vSonyEject0(tDrive Drive_No, blnr deleteit)
{
	MyFilePtr refnum = Drives[Drive_No];

	DiskEjectedNotify(Drive_No);

	MyFileClose(refnum);
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

LOCALFUNC blnr Sony_Insert0(MyFilePtr refnum, blnr locked,
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
		MyFileClose(refnum);
	}

	return IsOk;
}

LOCALFUNC blnr Sony_Insert1(char *drivepath, blnr silentfail)
{
	blnr locked = falseblnr;
	/* printf("Sony_Insert1 %s\n", drivepath); */
	MyFilePtr refnum = MyFileOpen(drivepath, "rb+");
	if (NULL == refnum) {
		locked = trueblnr;
		refnum = MyFileOpen(drivepath, "rb");
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
	MyFilePtr ROM_File;
	int File_Size;

	ROM_File = MyFileOpen(path, "rb");
	if (NULL == ROM_File) {
		err = mnvm_fnfErr;
	} else {
		File_Size = MyFileRead(ROM, 1, kROM_Size, ROM_File);
		if (File_Size != kROM_Size) {
#ifdef MyFileEof
			if (MyFileEof(ROM_File))
#else
			if (File_Size > 0)
#endif
			{
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
		MyFileClose(ROM_File);
	}

	return err;
}

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

LOCALFUNC blnr Sony_Insert2(char *s)
{
	char *d =
#if CanGetAppPath
		(NULL == d_arg) ? app_parent :
#endif
		d_arg;
	blnr IsOk = falseblnr;

	if (NULL == d) {
		IsOk = Sony_Insert1(s, trueblnr);
	} else {
		char *t;

		if (mnvm_noErr == ChildPath(d, s, &t)) {
			IsOk = Sony_Insert1(t, trueblnr);
			free(t);
		}
	}

	return IsOk;
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

#if CanGetAppPath
LOCALFUNC tMacErr LoadMacRomFromPrefDir(void)
{
	tMacErr err;
	char *t = NULL;
	char *t2 = NULL;

	if (NULL == pref_dir) {
		err = mnvm_fnfErr;
	} else
	if (mnvm_noErr != (err =
		ChildPath(pref_dir, "mnvm_rom", &t)))
	{
		/* fail */
	} else
	if (mnvm_noErr != (err =
		ChildPath(t, RomFileName, &t2)))
	{
		/* fail */
	} else
	{
		err = LoadMacRomFrom(t2);
	}

	MyMayFree(t2);
	MyMayFree(t);

	return err;
}
#endif

#if CanGetAppPath
LOCALFUNC tMacErr LoadMacRomFromAppPar(void)
{
	tMacErr err;
	char *d = (NULL == d_arg) ? app_parent : d_arg;
	char *t = NULL;

	if (NULL == d) {
		err = mnvm_fnfErr;
	} else
	if (mnvm_noErr != (err =
		ChildPath(d, RomFileName, &t)))
	{
		/* fail */
	} else
	{
		err = LoadMacRomFrom(t);
	}

	MyMayFree(t);

	return err;
}
#endif

LOCALFUNC blnr LoadMacRom(void)
{
	tMacErr err;

	if ((NULL == rom_path)
		|| (mnvm_fnfErr == (err = LoadMacRomFrom(rom_path))))
#if CanGetAppPath
	if (mnvm_fnfErr == (err = LoadMacRomFromAppPar()))
	if (mnvm_fnfErr == (err = LoadMacRomFromPrefDir()))
#endif
	if (mnvm_fnfErr == (err = LoadMacRomFrom(RomFileName)))
	{
	}

	return trueblnr; /* keep launching Mini vMac, regardless */
}

/* --- video out --- */

#if MayFullScreen
LOCALVAR int hOffset;
LOCALVAR int vOffset;
#endif

#if VarFullScreen
LOCALVAR blnr UseFullScreen = (WantInitFullScreen != 0);
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = (WantInitMagnify != 0);
#endif

#ifndef UseSDLscaling
#define UseSDLscaling 0
#endif

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

#if EnableMagnify && ! UseSDLscaling
#define MaxScale MyWindowScale
#else
#define MaxScale 1
#endif


LOCALVAR SDL_Window *my_main_wind = NULL;
LOCALVAR SDL_Renderer *my_renderer = NULL;
LOCALVAR SDL_Texture *my_texture = NULL;
LOCALVAR SDL_PixelFormat *my_format = NULL;

LOCALVAR ui3p ScalingBuff = nullpr;

LOCALVAR ui3p CLUT_final;

#define CLUT_finalsz (256 * 8 * 4 * MaxScale)
	/*
		256 possible values of one byte
		8 pixels per byte maximum (when black and white)
		4 bytes per destination pixel maximum
			multiplied by MyWindowScale if EnableMagnify
	*/

#define ScrnMapr_DoMap UpdateBWDepth3Copy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 3
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateBWDepth4Copy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 4
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateBWDepth5Copy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 5
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"

#if EnableMagnify && ! UseSDLscaling

#define ScrnMapr_DoMap UpdateBWDepth3ScaledCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 3
#define ScrnMapr_Map CLUT_final
#define ScrnMapr_Scale MyWindowScale

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateBWDepth4ScaledCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 4
#define ScrnMapr_Map CLUT_final
#define ScrnMapr_Scale MyWindowScale

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateBWDepth5ScaledCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 5
#define ScrnMapr_Map CLUT_final
#define ScrnMapr_Scale MyWindowScale

#include "SCRNMAPR.h"

#endif /* EnableMagnify && ! UseSDLscaling */


#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)

#define ScrnMapr_DoMap UpdateColorDepth3Copy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth vMacScreenDepth
#define ScrnMapr_DstDepth 3
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateColorDepth4Copy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth vMacScreenDepth
#define ScrnMapr_DstDepth 4
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateColorDepth5Copy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth vMacScreenDepth
#define ScrnMapr_DstDepth 5
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"

#if EnableMagnify && ! UseSDLscaling

#define ScrnMapr_DoMap UpdateColorDepth3ScaledCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth vMacScreenDepth
#define ScrnMapr_DstDepth 3
#define ScrnMapr_Map CLUT_final
#define ScrnMapr_Scale MyWindowScale

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateColorDepth4ScaledCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth vMacScreenDepth
#define ScrnMapr_DstDepth 4
#define ScrnMapr_Map CLUT_final
#define ScrnMapr_Scale MyWindowScale

#include "SCRNMAPR.h"

#define ScrnMapr_DoMap UpdateColorDepth5ScaledCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth vMacScreenDepth
#define ScrnMapr_DstDepth 5
#define ScrnMapr_Map CLUT_final
#define ScrnMapr_Scale MyWindowScale

#include "SCRNMAPR.h"

#endif /* EnableMagnify && ! UseSDLscaling */

#endif


LOCALPROC HaveChangedScreenBuff(ui4r top, ui4r left,
	ui4r bottom, ui4r right)
{
	int i;
	int j;
	ui3b *p;
	Uint32 pixel;
#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
	Uint32 CLUT_pixel[CLUT_size];
#endif
	Uint32 BWLUT_pixel[2];
	ui5r top2;
	ui5r left2;
	ui5r bottom2;
	ui5r right2;
	void *pixels;
	int pitch;
	SDL_Rect src_rect;
	SDL_Rect dst_rect;
	int XDest;
	int YDest;
	int DestWidth;
	int DestHeight;

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		if (top < ViewVStart) {
			top = ViewVStart;
		}
		if (left < ViewHStart) {
			left = ViewHStart;
		}
		if (bottom > ViewVStart + ViewVSize) {
			bottom = ViewVStart + ViewVSize;
		}
		if (right > ViewHStart + ViewHSize) {
			right = ViewHStart + ViewHSize;
		}

		if ((top >= bottom) || (left >= right)) {
			goto label_exit;
		}
	}
#endif

	XDest = left;
	YDest = top;
	DestWidth = (right - left);
	DestHeight = (bottom - top);

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		XDest -= ViewHStart;
		YDest -= ViewVStart;
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		XDest *= MyWindowScale;
		YDest *= MyWindowScale;
		DestWidth *= MyWindowScale;
		DestHeight *= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		XDest += hOffset;
		YDest += vOffset;
	}
#endif

	top2 = top;
	left2 = left;
	bottom2 = bottom;
	right2 = right;

#if EnableMagnify && ! UseSDLscaling
	if (UseMagnify) {
		top2 *= MyWindowScale;
		left2 *= MyWindowScale;
		bottom2 *= MyWindowScale;
		right2 *= MyWindowScale;
	}
#endif

	if (0 != SDL_LockTexture(my_texture, NULL, &pixels, &pitch)) {
		return;
	}

	{

	int bpp = my_format->BytesPerPixel;
	ui5r ExpectedPitch = vMacScreenWidth * bpp;

#if EnableMagnify && ! UseSDLscaling
	if (UseMagnify) {
		ExpectedPitch *= MyWindowScale;
	}
#endif

#if 0 != vMacScreenDepth
	if (UseColorMode) {
#if vMacScreenDepth < 4
		for (i = 0; i < CLUT_size; ++i) {
			CLUT_pixel[i] = SDL_MapRGB(my_format,
				CLUT_reds[i] >> 8,
				CLUT_greens[i] >> 8,
				CLUT_blues[i] >> 8);
		}
#endif
	} else
#endif
	{
		BWLUT_pixel[1] = SDL_MapRGB(my_format, 0, 0, 0);
			/* black */
		BWLUT_pixel[0] = SDL_MapRGB(my_format, 255, 255, 255);
			/* white */
	}

	if ((0 == ((bpp - 1) & bpp)) /* a power of 2 */
		&& (pitch == ExpectedPitch)
#if (vMacScreenDepth > 3)
		&& ! UseColorMode
#endif
		)
	{
		int k;
		Uint32 v;
#if EnableMagnify && ! UseSDLscaling
		int a;
#endif
		int PixPerByte =
#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
			UseColorMode ? (1 << (3 - vMacScreenDepth)) :
#endif
			8;
		Uint8 *p4 = (Uint8 *)CLUT_final;

		for (i = 0; i < 256; ++i) {
			for (k = PixPerByte; --k >= 0; ) {

#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
				if (UseColorMode) {
					v = CLUT_pixel[
#if 3 == vMacScreenDepth
						i
#else
						(i >> (k << vMacScreenDepth))
							& (CLUT_size - 1)
#endif
						];
				} else
#endif
				{
					v = BWLUT_pixel[(i >> k) & 1];
				}

#if EnableMagnify && ! UseSDLscaling
				for (a = UseMagnify ? MyWindowScale : 1; --a >= 0; )
#endif
				{
					switch (bpp) {
						case 1: /* Assuming 8-bpp */
							*p4++ = v;
							break;
						case 2: /* Probably 15-bpp or 16-bpp */
							*(Uint16 *)p4 = v;
							p4 += 2;
							break;
						case 4: /* Probably 32-bpp */
							*(Uint32 *)p4 = v;
							p4 += 4;
							break;
					}
				}
			}
		}

		ScalingBuff = (ui3p)pixels;

#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
		if (UseColorMode) {
#if EnableMagnify && ! UseSDLscaling
			if (UseMagnify) {
				switch (bpp) {
					case 1:
						UpdateColorDepth3ScaledCopy(
							top, left, bottom, right);
						break;
					case 2:
						UpdateColorDepth4ScaledCopy(
							top, left, bottom, right);
						break;
					case 4:
						UpdateColorDepth5ScaledCopy(
							top, left, bottom, right);
						break;
				}
			} else
#endif
			{
				switch (bpp) {
					case 1:
						UpdateColorDepth3Copy(top, left, bottom, right);
						break;
					case 2:
						UpdateColorDepth4Copy(top, left, bottom, right);
						break;
					case 4:
						UpdateColorDepth5Copy(top, left, bottom, right);
						break;
				}
			}
		} else
#endif
		{
#if EnableMagnify && ! UseSDLscaling
			if (UseMagnify) {
				switch (bpp) {
					case 1:
						UpdateBWDepth3ScaledCopy(
							top, left, bottom, right);
						break;
					case 2:
						UpdateBWDepth4ScaledCopy(
							top, left, bottom, right);
						break;
					case 4:
						UpdateBWDepth5ScaledCopy(
							top, left, bottom, right);
						break;
				}
			} else
#endif
			{
				switch (bpp) {
					case 1:
						UpdateBWDepth3Copy(top, left, bottom, right);
						break;
					case 2:
						UpdateBWDepth4Copy(top, left, bottom, right);
						break;
					case 4:
						UpdateBWDepth5Copy(top, left, bottom, right);
						break;
				}
			}
		}

	} else {
		ui3b *the_data = (ui3b *)GetCurDrawBuff();

		/* adapted from putpixel in SDL documentation */

		for (i = top2; i < bottom2; ++i) {
			for (j = left2; j < right2; ++j) {
				int i0 = i;
				int j0 = j;
				Uint8 *bufp = (Uint8 *)pixels
					+ i * pitch + j * bpp;

#if EnableMagnify && ! UseSDLscaling
				if (UseMagnify) {
					i0 /= MyWindowScale;
					j0 /= MyWindowScale;
				}
#endif

#if 0 != vMacScreenDepth
				if (UseColorMode) {
#if vMacScreenDepth < 4
					p = the_data + ((i0 * vMacScreenWidth + j0)
						>> (3 - vMacScreenDepth));
					{
						ui3r k = (*p >> (((~ j0)
								& ((1 << (3 - vMacScreenDepth)) - 1))
							<< vMacScreenDepth))
							& (CLUT_size - 1);
						pixel = CLUT_pixel[k];
					}
#elif 4 == vMacScreenDepth
					p = the_data + ((i0 * vMacScreenWidth + j0) << 1);
					{
						ui4r t0 = do_get_mem_word(p);
						pixel = SDL_MapRGB(my_format,
							((t0 & 0x7C00) >> 7)
								| ((t0 & 0x7000) >> 12),
							((t0 & 0x03E0) >> 2)
								| ((t0 & 0x0380) >> 7),
							((t0 & 0x001F) << 3)
								| ((t0 & 0x001C) >> 2));
					}
#elif 5 == vMacScreenDepth
					p = the_data + ((i0 * vMacScreenWidth + j0) << 2);
					pixel = SDL_MapRGB(my_format,
						p[1],
						p[2],
						p[3]);
#endif
				} else
#endif
				{
					p = the_data + ((i0 * vMacScreenWidth + j0) / 8);
					pixel = BWLUT_pixel[(*p >> ((~ j0) & 0x7)) & 1];
				}

				switch (bpp) {
					case 1: /* Assuming 8-bpp */
						*bufp = pixel;
						break;
					case 2: /* Probably 15-bpp or 16-bpp */
						*(Uint16 *)bufp = pixel;
						break;
					case 3:
						/* Slow 24-bpp mode, usually not used */
						if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
							bufp[0] = (pixel >> 16) & 0xff;
							bufp[1] = (pixel >> 8) & 0xff;
							bufp[2] = pixel & 0xff;
						} else {
							bufp[0] = pixel & 0xff;
							bufp[1] = (pixel >> 8) & 0xff;
							bufp[2] = (pixel >> 16) & 0xff;
						}
						break;
					case 4: /* Probably 32-bpp */
						*(Uint32 *)bufp = pixel;
						break;
				}
			}
		}
	}

	}

	SDL_UnlockTexture(my_texture);

	src_rect.x = left2;
	src_rect.y = top2;
	src_rect.w = right2 - left2;
	src_rect.h = bottom2 - top2;

	dst_rect.x = XDest;
	dst_rect.y = YDest;
	dst_rect.w = DestWidth;
	dst_rect.h = DestHeight;

	/* SDL_RenderClear(my_renderer); */
	SDL_RenderCopy(my_renderer, my_texture, &src_rect, &dst_rect);
	SDL_RenderPresent(my_renderer);

#if MayFullScreen
label_exit:
	;
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

/* cursor hiding */

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		(void) SDL_ShowCursor(SDL_ENABLE);
	}
}

/* cursor moving */

#ifndef HaveWorkingWarp
#define HaveWorkingWarp 1
#endif

#if EnableMoveMouse && HaveWorkingWarp
LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		h -= ViewHStart;
		v -= ViewVStart;
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		h *= MyWindowScale;
		v *= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		h += hOffset;
		v += vOffset;
	}
#endif

	SDL_WarpMouseInWindow(my_main_wind, h, v);

	return trueblnr;
}
#endif

/* cursor state */

LOCALPROC MousePositionNotify(int NewMousePosh, int NewMousePosv)
{
	blnr ShouldHaveCursorHidden = trueblnr;

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewMousePosh -= hOffset;
		NewMousePosv -= vOffset;
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		NewMousePosh /= MyWindowScale;
		NewMousePosv /= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewMousePosh += ViewHStart;
		NewMousePosv += ViewVStart;
	}
#endif

#if EnableFSMouseMotion
	if (HaveMouseMotion) {
		MyMousePositionSetDelta(NewMousePosh - SavedMouseH,
			NewMousePosv - SavedMouseV);
		SavedMouseH = NewMousePosh;
		SavedMouseV = NewMousePosv;
	} else
#endif
	{
		if (NewMousePosh < 0) {
			NewMousePosh = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePosh >= vMacScreenWidth) {
			NewMousePosh = vMacScreenWidth - 1;
			ShouldHaveCursorHidden = falseblnr;
		}
		if (NewMousePosv < 0) {
			NewMousePosv = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePosv >= vMacScreenHeight) {
			NewMousePosv = vMacScreenHeight - 1;
			ShouldHaveCursorHidden = falseblnr;
		}

#if VarFullScreen
		if (UseFullScreen)
#endif
#if MayFullScreen
		{
			ShouldHaveCursorHidden = trueblnr;
		}
#endif

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/*
			for a game like arkanoid, would like mouse to still
			move even when outside window in one direction
		*/
		MyMousePositionSet(NewMousePosh, NewMousePosv);
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

#if EnableFSMouseMotion && ! HaveWorkingWarp
LOCALPROC MousePositionNotifyRelative(int deltah, int deltav)
{
	blnr ShouldHaveCursorHidden = trueblnr;

#if EnableMagnify
	if (UseMagnify) {
		/*
			This is not really right. If only move one pixel
			each time, emulated mouse doesn't move at all.
		*/
		deltah /= MyWindowScale;
		deltav /= MyWindowScale;
	}
#endif

	MyMousePositionSetDelta(deltah,
		deltav);

	WantCursorHidden = ShouldHaveCursorHidden;
}
#endif

LOCALPROC CheckMouseState(void)
{
	/*
		this doesn't work as desired, doesn't get mouse movements
		when outside of our window.
	*/
	int x;
	int y;

	(void) SDL_GetMouseState(&x, &y);
	MousePositionNotify(x, y);
}

/* --- keyboard input --- */

LOCALFUNC ui3r SDLScan2MacKeyCode(SDL_Scancode i)
{
	ui3r v = MKC_None;

	switch (i) {
		case SDL_SCANCODE_BACKSPACE: v = MKC_BackSpace; break;
		case SDL_SCANCODE_TAB: v = MKC_Tab; break;
		case SDL_SCANCODE_CLEAR: v = MKC_Clear; break;
		case SDL_SCANCODE_RETURN: v = MKC_Return; break;
		case SDL_SCANCODE_PAUSE: v = MKC_Pause; break;
		case SDL_SCANCODE_ESCAPE: v = MKC_formac_Escape; break;
		case SDL_SCANCODE_SPACE: v = MKC_Space; break;
		case SDL_SCANCODE_APOSTROPHE: v = MKC_SingleQuote; break;
		case SDL_SCANCODE_COMMA: v = MKC_Comma; break;
		case SDL_SCANCODE_MINUS: v = MKC_Minus; break;
		case SDL_SCANCODE_PERIOD: v = MKC_Period; break;
		case SDL_SCANCODE_SLASH: v = MKC_formac_Slash; break;
		case SDL_SCANCODE_0: v = MKC_0; break;
		case SDL_SCANCODE_1: v = MKC_1; break;
		case SDL_SCANCODE_2: v = MKC_2; break;
		case SDL_SCANCODE_3: v = MKC_3; break;
		case SDL_SCANCODE_4: v = MKC_4; break;
		case SDL_SCANCODE_5: v = MKC_5; break;
		case SDL_SCANCODE_6: v = MKC_6; break;
		case SDL_SCANCODE_7: v = MKC_7; break;
		case SDL_SCANCODE_8: v = MKC_8; break;
		case SDL_SCANCODE_9: v = MKC_9; break;
		case SDL_SCANCODE_SEMICOLON: v = MKC_SemiColon; break;
		case SDL_SCANCODE_EQUALS: v = MKC_Equal; break;

		case SDL_SCANCODE_LEFTBRACKET: v = MKC_LeftBracket; break;
		case SDL_SCANCODE_BACKSLASH: v = MKC_formac_BackSlash; break;
		case SDL_SCANCODE_RIGHTBRACKET: v = MKC_RightBracket; break;
		case SDL_SCANCODE_GRAVE: v = MKC_formac_Grave; break;

		case SDL_SCANCODE_A: v = MKC_A; break;
		case SDL_SCANCODE_B: v = MKC_B; break;
		case SDL_SCANCODE_C: v = MKC_C; break;
		case SDL_SCANCODE_D: v = MKC_D; break;
		case SDL_SCANCODE_E: v = MKC_E; break;
		case SDL_SCANCODE_F: v = MKC_F; break;
		case SDL_SCANCODE_G: v = MKC_G; break;
		case SDL_SCANCODE_H: v = MKC_H; break;
		case SDL_SCANCODE_I: v = MKC_I; break;
		case SDL_SCANCODE_J: v = MKC_J; break;
		case SDL_SCANCODE_K: v = MKC_K; break;
		case SDL_SCANCODE_L: v = MKC_L; break;
		case SDL_SCANCODE_M: v = MKC_M; break;
		case SDL_SCANCODE_N: v = MKC_N; break;
		case SDL_SCANCODE_O: v = MKC_O; break;
		case SDL_SCANCODE_P: v = MKC_P; break;
		case SDL_SCANCODE_Q: v = MKC_Q; break;
		case SDL_SCANCODE_R: v = MKC_R; break;
		case SDL_SCANCODE_S: v = MKC_S; break;
		case SDL_SCANCODE_T: v = MKC_T; break;
		case SDL_SCANCODE_U: v = MKC_U; break;
		case SDL_SCANCODE_V: v = MKC_V; break;
		case SDL_SCANCODE_W: v = MKC_W; break;
		case SDL_SCANCODE_X: v = MKC_X; break;
		case SDL_SCANCODE_Y: v = MKC_Y; break;
		case SDL_SCANCODE_Z: v = MKC_Z; break;

		case SDL_SCANCODE_KP_0: v = MKC_KP0; break;
		case SDL_SCANCODE_KP_1: v = MKC_KP1; break;
		case SDL_SCANCODE_KP_2: v = MKC_KP2; break;
		case SDL_SCANCODE_KP_3: v = MKC_KP3; break;
		case SDL_SCANCODE_KP_4: v = MKC_KP4; break;
		case SDL_SCANCODE_KP_5: v = MKC_KP5; break;
		case SDL_SCANCODE_KP_6: v = MKC_KP6; break;
		case SDL_SCANCODE_KP_7: v = MKC_KP7; break;
		case SDL_SCANCODE_KP_8: v = MKC_KP8; break;
		case SDL_SCANCODE_KP_9: v = MKC_KP9; break;

		case SDL_SCANCODE_KP_PERIOD: v = MKC_Decimal; break;
		case SDL_SCANCODE_KP_DIVIDE: v = MKC_KPDevide; break;
		case SDL_SCANCODE_KP_MULTIPLY: v = MKC_KPMultiply; break;
		case SDL_SCANCODE_KP_MINUS: v = MKC_KPSubtract; break;
		case SDL_SCANCODE_KP_PLUS: v = MKC_KPAdd; break;
		case SDL_SCANCODE_KP_ENTER: v = MKC_formac_Enter; break;
		case SDL_SCANCODE_KP_EQUALS: v = MKC_KPEqual; break;

		case SDL_SCANCODE_UP: v = MKC_Up; break;
		case SDL_SCANCODE_DOWN: v = MKC_Down; break;
		case SDL_SCANCODE_RIGHT: v = MKC_Right; break;
		case SDL_SCANCODE_LEFT: v = MKC_Left; break;
		case SDL_SCANCODE_INSERT: v = MKC_formac_Help; break;
		case SDL_SCANCODE_HOME: v = MKC_formac_Home; break;
		case SDL_SCANCODE_END: v = MKC_formac_End; break;
		case SDL_SCANCODE_PAGEUP: v = MKC_formac_PageUp; break;
		case SDL_SCANCODE_PAGEDOWN: v = MKC_formac_PageDown; break;

		case SDL_SCANCODE_F1: v = MKC_formac_F1; break;
		case SDL_SCANCODE_F2: v = MKC_formac_F2; break;
		case SDL_SCANCODE_F3: v = MKC_formac_F3; break;
		case SDL_SCANCODE_F4: v = MKC_formac_F4; break;
		case SDL_SCANCODE_F5: v = MKC_formac_F5; break;
		case SDL_SCANCODE_F6: v = MKC_F6; break;
		case SDL_SCANCODE_F7: v = MKC_F7; break;
		case SDL_SCANCODE_F8: v = MKC_F8; break;
		case SDL_SCANCODE_F9: v = MKC_F9; break;
		case SDL_SCANCODE_F10: v = MKC_F10; break;
		case SDL_SCANCODE_F11: v = MKC_F11; break;
		case SDL_SCANCODE_F12: v = MKC_F12; break;

		case SDL_SCANCODE_NUMLOCKCLEAR:
			v = MKC_formac_ForwardDel; break;
		case SDL_SCANCODE_CAPSLOCK: v = MKC_formac_CapsLock; break;
		case SDL_SCANCODE_SCROLLLOCK: v = MKC_ScrollLock; break;
		case SDL_SCANCODE_RSHIFT: v = MKC_formac_RShift; break;
		case SDL_SCANCODE_LSHIFT: v = MKC_formac_Shift; break;
		case SDL_SCANCODE_RCTRL: v = MKC_formac_RControl; break;
		case SDL_SCANCODE_LCTRL: v = MKC_formac_Control; break;
		case SDL_SCANCODE_RALT: v = MKC_formac_ROption; break;
		case SDL_SCANCODE_LALT: v = MKC_formac_Option; break;
		case SDL_SCANCODE_RGUI: v = MKC_formac_RCommand; break;
		case SDL_SCANCODE_LGUI: v = MKC_formac_Command; break;
		/* case SDLK_LSUPER: v = MKC_formac_Option; break; */
		/* case SDLK_RSUPER: v = MKC_formac_ROption; break; */

		case SDL_SCANCODE_HELP: v = MKC_formac_Help; break;
		case SDL_SCANCODE_PRINTSCREEN: v = MKC_Print; break;

		case SDL_SCANCODE_UNDO: v = MKC_formac_F1; break;
		case SDL_SCANCODE_CUT: v = MKC_formac_F2; break;
		case SDL_SCANCODE_COPY: v = MKC_formac_F3; break;
		case SDL_SCANCODE_PASTE: v = MKC_formac_F4; break;

		case SDL_SCANCODE_AC_HOME: v = MKC_formac_Home; break;

		case SDL_SCANCODE_KP_A: v = MKC_A; break;
		case SDL_SCANCODE_KP_B: v = MKC_B; break;
		case SDL_SCANCODE_KP_C: v = MKC_C; break;
		case SDL_SCANCODE_KP_D: v = MKC_D; break;
		case SDL_SCANCODE_KP_E: v = MKC_E; break;
		case SDL_SCANCODE_KP_F: v = MKC_F; break;

		case SDL_SCANCODE_KP_BACKSPACE: v = MKC_BackSpace; break;
		case SDL_SCANCODE_KP_CLEAR: v = MKC_Clear; break;
		case SDL_SCANCODE_KP_COMMA: v = MKC_Comma; break;
		case SDL_SCANCODE_KP_DECIMAL: v = MKC_Decimal; break;

		default:
			break;
	}

	return v;
}

LOCALPROC DoKeyCode(SDL_Keysym *r, blnr down)
{
	ui3r v = SDLScan2MacKeyCode(r->scancode);
	if (MKC_None != v) {
		Keyboard_UpdateKeyMap2(v, down);
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

#define dbglog_TimeStuff (0 && dbglog_HAVE)

LOCALVAR ui5b TrueEmulatedTime = 0;

#define MyInvTimeDivPow 16
#define MyInvTimeDiv (1 << MyInvTimeDivPow)
#define MyInvTimeDivMask (MyInvTimeDiv - 1)
#define MyInvTimeStep 1089590 /* 1000 / 60.14742 * MyInvTimeDiv */

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

LOCALFUNC blnr UpdateTrueEmulatedTime(void)
{
	Uint32 LatestTime;
	si5b TimeDiff;

	LatestTime = SDL_GetTicks();
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
	LastTime = SDL_GetTicks();
	InitNextTime();
}

LOCALFUNC blnr InitLocationDat(void)
{
	LastTime = SDL_GetTicks();
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

		if (0 != SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			SavedBriefMsg,
			SavedLongMsg,
			my_main_wind
			))
		{
			fprintf(stderr, "%s\n", briefMsg0);
			fprintf(stderr, "%s\n", longMsg0);
		}

		SavedBriefMsg = nullpr;
	}
}

/* --- clipboard --- */

#if IncludeHostTextClipExchange
LOCALFUNC uimr MacRoman2UniCodeSize(ui3b *s, uimr L)
{
	uimr i;
	ui3r x;
	uimr n;
	uimr v = 0;

	for (i = 0; i < L; ++i) {
		x = *s++;
		if (x < 128) {
			n = 1;
		} else {
			switch (x) {
				case 0x80: n = 2; break;
					/* LATIN CAPITAL LETTER A WITH DIAERESIS */
				case 0x81: n = 2; break;
					/* LATIN CAPITAL LETTER A WITH RING ABOVE */
				case 0x82: n = 2; break;
					/* LATIN CAPITAL LETTER C WITH CEDILLA */
				case 0x83: n = 2; break;
					/* LATIN CAPITAL LETTER E WITH ACUTE */
				case 0x84: n = 2; break;
					/* LATIN CAPITAL LETTER N WITH TILDE */
				case 0x85: n = 2; break;
					/* LATIN CAPITAL LETTER O WITH DIAERESIS */
				case 0x86: n = 2; break;
					/* LATIN CAPITAL LETTER U WITH DIAERESIS */
				case 0x87: n = 2; break;
					/* LATIN SMALL LETTER A WITH ACUTE */
				case 0x88: n = 2; break;
					/* LATIN SMALL LETTER A WITH GRAVE */
				case 0x89: n = 2; break;
					/* LATIN SMALL LETTER A WITH CIRCUMFLEX */
				case 0x8A: n = 2; break;
					/* LATIN SMALL LETTER A WITH DIAERESIS */
				case 0x8B: n = 2; break;
					/* LATIN SMALL LETTER A WITH TILDE */
				case 0x8C: n = 2; break;
					/* LATIN SMALL LETTER A WITH RING ABOVE */
				case 0x8D: n = 2; break;
					/* LATIN SMALL LETTER C WITH CEDILLA */
				case 0x8E: n = 2; break;
					/* LATIN SMALL LETTER E WITH ACUTE */
				case 0x8F: n = 2; break;
					/* LATIN SMALL LETTER E WITH GRAVE */
				case 0x90: n = 2; break;
					/* LATIN SMALL LETTER E WITH CIRCUMFLEX */
				case 0x91: n = 2; break;
					/* LATIN SMALL LETTER E WITH DIAERESIS */
				case 0x92: n = 2; break;
					/* LATIN SMALL LETTER I WITH ACUTE */
				case 0x93: n = 2; break;
					/* LATIN SMALL LETTER I WITH GRAVE */
				case 0x94: n = 2; break;
					/* LATIN SMALL LETTER I WITH CIRCUMFLEX */
				case 0x95: n = 2; break;
					/* LATIN SMALL LETTER I WITH DIAERESIS */
				case 0x96: n = 2; break;
					/* LATIN SMALL LETTER N WITH TILDE */
				case 0x97: n = 2; break;
					/* LATIN SMALL LETTER O WITH ACUTE */
				case 0x98: n = 2; break;
					/* LATIN SMALL LETTER O WITH GRAVE */
				case 0x99: n = 2; break;
					/* LATIN SMALL LETTER O WITH CIRCUMFLEX */
				case 0x9A: n = 2; break;
					/* LATIN SMALL LETTER O WITH DIAERESIS */
				case 0x9B: n = 2; break;
					/* LATIN SMALL LETTER O WITH TILDE */
				case 0x9C: n = 2; break;
					/* LATIN SMALL LETTER U WITH ACUTE */
				case 0x9D: n = 2; break;
					/* LATIN SMALL LETTER U WITH GRAVE */
				case 0x9E: n = 2; break;
					/* LATIN SMALL LETTER U WITH CIRCUMFLEX */
				case 0x9F: n = 2; break;
					/* LATIN SMALL LETTER U WITH DIAERESIS */
				case 0xA0: n = 3; break;
					/* DAGGER */
				case 0xA1: n = 2; break;
					/* DEGREE SIGN */
				case 0xA2: n = 2; break;
					/* CENT SIGN */
				case 0xA3: n = 2; break;
					/* POUND SIGN */
				case 0xA4: n = 2; break;
					/* SECTION SIGN */
				case 0xA5: n = 3; break;
					/* BULLET */
				case 0xA6: n = 2; break;
					/* PILCROW SIGN */
				case 0xA7: n = 2; break;
					/* LATIN SMALL LETTER SHARP S */
				case 0xA8: n = 2; break;
					/* REGISTERED SIGN */
				case 0xA9: n = 2; break;
					/* COPYRIGHT SIGN */
				case 0xAA: n = 3; break;
					/* TRADE MARK SIGN */
				case 0xAB: n = 2; break;
					/* ACUTE ACCENT */
				case 0xAC: n = 2; break;
					/* DIAERESIS */
				case 0xAD: n = 3; break;
					/* NOT EQUAL TO */
				case 0xAE: n = 2; break;
					/* LATIN CAPITAL LETTER AE */
				case 0xAF: n = 2; break;
					/* LATIN CAPITAL LETTER O WITH STROKE */
				case 0xB0: n = 3; break;
					/* INFINITY */
				case 0xB1: n = 2; break;
					/* PLUS-MINUS SIGN */
				case 0xB2: n = 3; break;
					/* LESS-THAN OR EQUAL TO */
				case 0xB3: n = 3; break;
					/* GREATER-THAN OR EQUAL TO */
				case 0xB4: n = 2; break;
					/* YEN SIGN */
				case 0xB5: n = 2; break;
					/* MICRO SIGN */
				case 0xB6: n = 3; break;
					/* PARTIAL DIFFERENTIAL */
				case 0xB7: n = 3; break;
					/* N-ARY SUMMATION */
				case 0xB8: n = 3; break;
					/* N-ARY PRODUCT */
				case 0xB9: n = 2; break;
					/* GREEK SMALL LETTER PI */
				case 0xBA: n = 3; break;
					/* INTEGRAL */
				case 0xBB: n = 2; break;
					/* FEMININE ORDINAL INDICATOR */
				case 0xBC: n = 2; break;
					/* MASCULINE ORDINAL INDICATOR */
				case 0xBD: n = 2; break;
					/* GREEK CAPITAL LETTER OMEGA */
				case 0xBE: n = 2; break;
					/* LATIN SMALL LETTER AE */
				case 0xBF: n = 2; break;
					/* LATIN SMALL LETTER O WITH STROKE */
				case 0xC0: n = 2; break;
					/* INVERTED QUESTION MARK */
				case 0xC1: n = 2; break;
					/* INVERTED EXCLAMATION MARK */
				case 0xC2: n = 2; break;
					/* NOT SIGN */
				case 0xC3: n = 3; break;
					/* SQUARE ROOT */
				case 0xC4: n = 2; break;
					/* LATIN SMALL LETTER F WITH HOOK */
				case 0xC5: n = 3; break;
					/* ALMOST EQUAL TO */
				case 0xC6: n = 3; break;
					/* INCREMENT */
				case 0xC7: n = 2; break;
					/* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
				case 0xC8: n = 2; break;
					/* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
				case 0xC9: n = 3; break;
					/* HORIZONTAL ELLIPSIS */
				case 0xCA: n = 2; break;
					/* NO-BREAK SPACE */
				case 0xCB: n = 2; break;
					/* LATIN CAPITAL LETTER A WITH GRAVE */
				case 0xCC: n = 2; break;
					/* LATIN CAPITAL LETTER A WITH TILDE */
				case 0xCD: n = 2; break;
					/* LATIN CAPITAL LETTER O WITH TILDE */
				case 0xCE: n = 2; break;
					/* LATIN CAPITAL LIGATURE OE */
				case 0xCF: n = 2; break;
					/* LATIN SMALL LIGATURE OE */
				case 0xD0: n = 3; break;
					/* EN DASH */
				case 0xD1: n = 3; break;
					/* EM DASH */
				case 0xD2: n = 3; break;
					/* LEFT DOUBLE QUOTATION MARK */
				case 0xD3: n = 3; break;
					/* RIGHT DOUBLE QUOTATION MARK */
				case 0xD4: n = 3; break;
					/* LEFT SINGLE QUOTATION MARK */
				case 0xD5: n = 3; break;
					/* RIGHT SINGLE QUOTATION MARK */
				case 0xD6: n = 2; break;
					/* DIVISION SIGN */
				case 0xD7: n = 3; break;
					/* LOZENGE */
				case 0xD8: n = 2; break;
					/* LATIN SMALL LETTER Y WITH DIAERESIS */
				case 0xD9: n = 2; break;
					/* LATIN CAPITAL LETTER Y WITH DIAERESIS */
				case 0xDA: n = 3; break;
					/* FRACTION SLASH */
				case 0xDB: n = 3; break;
					/* EURO SIGN */
				case 0xDC: n = 3; break;
					/* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
				case 0xDD: n = 3; break;
					/* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
				case 0xDE: n = 3; break;
					/* LATIN SMALL LIGATURE FI */
				case 0xDF: n = 3; break;
					/* LATIN SMALL LIGATURE FL */
				case 0xE0: n = 3; break;
					/* DOUBLE DAGGER */
				case 0xE1: n = 2; break;
					/* MIDDLE DOT */
				case 0xE2: n = 3; break;
					/* SINGLE LOW-9 QUOTATION MARK */
				case 0xE3: n = 3; break;
					/* DOUBLE LOW-9 QUOTATION MARK */
				case 0xE4: n = 3; break;
					/* PER MILLE SIGN */
				case 0xE5: n = 2; break;
					/* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
				case 0xE6: n = 2; break;
					/* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
				case 0xE7: n = 2; break;
					/* LATIN CAPITAL LETTER A WITH ACUTE */
				case 0xE8: n = 2; break;
					/* LATIN CAPITAL LETTER E WITH DIAERESIS */
				case 0xE9: n = 2; break;
					/* LATIN CAPITAL LETTER E WITH GRAVE */
				case 0xEA: n = 2; break;
					/* LATIN CAPITAL LETTER I WITH ACUTE */
				case 0xEB: n = 2; break;
					/* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
				case 0xEC: n = 2; break;
					/* LATIN CAPITAL LETTER I WITH DIAERESIS */
				case 0xED: n = 2; break;
					/* LATIN CAPITAL LETTER I WITH GRAVE */
				case 0xEE: n = 2; break;
					/* LATIN CAPITAL LETTER O WITH ACUTE */
				case 0xEF: n = 2; break;
					/* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
				case 0xF0: n = 3; break;
					/* Apple logo */
				case 0xF1: n = 2; break;
					/* LATIN CAPITAL LETTER O WITH GRAVE */
				case 0xF2: n = 2; break;
					/* LATIN CAPITAL LETTER U WITH ACUTE */
				case 0xF3: n = 2; break;
					/* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
				case 0xF4: n = 2; break;
					/* LATIN CAPITAL LETTER U WITH GRAVE */
				case 0xF5: n = 2; break;
					/* LATIN SMALL LETTER DOTLESS I */
				case 0xF6: n = 2; break;
					/* MODIFIER LETTER CIRCUMFLEX ACCENT */
				case 0xF7: n = 2; break;
					/* SMALL TILDE */
				case 0xF8: n = 2; break;
					/* MACRON */
				case 0xF9: n = 2; break;
					/* BREVE */
				case 0xFA: n = 2; break;
					/* DOT ABOVE */
				case 0xFB: n = 2; break;
					/* RING ABOVE */
				case 0xFC: n = 2; break;
					/* CEDILLA */
				case 0xFD: n = 2; break;
					/* DOUBLE ACUTE ACCENT */
				case 0xFE: n = 2; break;
					/* OGONEK */
				case 0xFF: n = 2; break;
					/* CARON */
				default: n = 1; break;
					/* shouldn't get here */
			}
		}
		v += n;
	}

	return v;
}
#endif

#if IncludeHostTextClipExchange
LOCALPROC MacRoman2UniCodeData(ui3b *s, uimr L, char *t)
{
	uimr i;
	ui3r x;

	for (i = 0; i < L; ++i) {
		x = *s++;
		if (x < 128) {
			*t++ = x;
		} else {
			switch (x) {
				case 0x80: *t++ = 0xC3; *t++ = 0x84; break;
					/* LATIN CAPITAL LETTER A WITH DIAERESIS */
				case 0x81: *t++ = 0xC3; *t++ = 0x85; break;
					/* LATIN CAPITAL LETTER A WITH RING ABOVE */
				case 0x82: *t++ = 0xC3; *t++ = 0x87; break;
					/* LATIN CAPITAL LETTER C WITH CEDILLA */
				case 0x83: *t++ = 0xC3; *t++ = 0x89; break;
					/* LATIN CAPITAL LETTER E WITH ACUTE */
				case 0x84: *t++ = 0xC3; *t++ = 0x91; break;
					/* LATIN CAPITAL LETTER N WITH TILDE */
				case 0x85: *t++ = 0xC3; *t++ = 0x96; break;
					/* LATIN CAPITAL LETTER O WITH DIAERESIS */
				case 0x86: *t++ = 0xC3; *t++ = 0x9C; break;
					/* LATIN CAPITAL LETTER U WITH DIAERESIS */
				case 0x87: *t++ = 0xC3; *t++ = 0xA1; break;
					/* LATIN SMALL LETTER A WITH ACUTE */
				case 0x88: *t++ = 0xC3; *t++ = 0xA0; break;
					/* LATIN SMALL LETTER A WITH GRAVE */
				case 0x89: *t++ = 0xC3; *t++ = 0xA2; break;
					/* LATIN SMALL LETTER A WITH CIRCUMFLEX */
				case 0x8A: *t++ = 0xC3; *t++ = 0xA4; break;
					/* LATIN SMALL LETTER A WITH DIAERESIS */
				case 0x8B: *t++ = 0xC3; *t++ = 0xA3; break;
					/* LATIN SMALL LETTER A WITH TILDE */
				case 0x8C: *t++ = 0xC3; *t++ = 0xA5; break;
					/* LATIN SMALL LETTER A WITH RING ABOVE */
				case 0x8D: *t++ = 0xC3; *t++ = 0xA7; break;
					/* LATIN SMALL LETTER C WITH CEDILLA */
				case 0x8E: *t++ = 0xC3; *t++ = 0xA9; break;
					/* LATIN SMALL LETTER E WITH ACUTE */
				case 0x8F: *t++ = 0xC3; *t++ = 0xA8; break;
					/* LATIN SMALL LETTER E WITH GRAVE */
				case 0x90: *t++ = 0xC3; *t++ = 0xAA; break;
					/* LATIN SMALL LETTER E WITH CIRCUMFLEX */
				case 0x91: *t++ = 0xC3; *t++ = 0xAB; break;
					/* LATIN SMALL LETTER E WITH DIAERESIS */
				case 0x92: *t++ = 0xC3; *t++ = 0xAD; break;
					/* LATIN SMALL LETTER I WITH ACUTE */
				case 0x93: *t++ = 0xC3; *t++ = 0xAC; break;
					/* LATIN SMALL LETTER I WITH GRAVE */
				case 0x94: *t++ = 0xC3; *t++ = 0xAE; break;
					/* LATIN SMALL LETTER I WITH CIRCUMFLEX */
				case 0x95: *t++ = 0xC3; *t++ = 0xAF; break;
					/* LATIN SMALL LETTER I WITH DIAERESIS */
				case 0x96: *t++ = 0xC3; *t++ = 0xB1; break;
					/* LATIN SMALL LETTER N WITH TILDE */
				case 0x97: *t++ = 0xC3; *t++ = 0xB3; break;
					/* LATIN SMALL LETTER O WITH ACUTE */
				case 0x98: *t++ = 0xC3; *t++ = 0xB2; break;
					/* LATIN SMALL LETTER O WITH GRAVE */
				case 0x99: *t++ = 0xC3; *t++ = 0xB4; break;
					/* LATIN SMALL LETTER O WITH CIRCUMFLEX */
				case 0x9A: *t++ = 0xC3; *t++ = 0xB6; break;
					/* LATIN SMALL LETTER O WITH DIAERESIS */
				case 0x9B: *t++ = 0xC3; *t++ = 0xB5; break;
					/* LATIN SMALL LETTER O WITH TILDE */
				case 0x9C: *t++ = 0xC3; *t++ = 0xBA; break;
					/* LATIN SMALL LETTER U WITH ACUTE */
				case 0x9D: *t++ = 0xC3; *t++ = 0xB9; break;
					/* LATIN SMALL LETTER U WITH GRAVE */
				case 0x9E: *t++ = 0xC3; *t++ = 0xBB; break;
					/* LATIN SMALL LETTER U WITH CIRCUMFLEX */
				case 0x9F: *t++ = 0xC3; *t++ = 0xBC; break;
					/* LATIN SMALL LETTER U WITH DIAERESIS */
				case 0xA0: *t++ = 0xE2; *t++ = 0x80; *t++ = 0xA0; break;
					/* DAGGER */
				case 0xA1: *t++ = 0xC2; *t++ = 0xB0; break;
					/* DEGREE SIGN */
				case 0xA2: *t++ = 0xC2; *t++ = 0xA2; break;
					/* CENT SIGN */
				case 0xA3: *t++ = 0xC2; *t++ = 0xA3; break;
					/* POUND SIGN */
				case 0xA4: *t++ = 0xC2; *t++ = 0xA7; break;
					/* SECTION SIGN */
				case 0xA5: *t++ = 0xE2; *t++ = 0x80; *t++ = 0xA2; break;
					/* BULLET */
				case 0xA6: *t++ = 0xC2; *t++ = 0xB6; break;
					/* PILCROW SIGN */
				case 0xA7: *t++ = 0xC3; *t++ = 0x9F; break;
					/* LATIN SMALL LETTER SHARP S */
				case 0xA8: *t++ = 0xC2; *t++ = 0xAE; break;
					/* REGISTERED SIGN */
				case 0xA9: *t++ = 0xC2; *t++ = 0xA9; break;
					/* COPYRIGHT SIGN */
				case 0xAA: *t++ = 0xE2; *t++ = 0x84; *t++ = 0xA2; break;
					/* TRADE MARK SIGN */
				case 0xAB: *t++ = 0xC2; *t++ = 0xB4; break;
					/* ACUTE ACCENT */
				case 0xAC: *t++ = 0xC2; *t++ = 0xA8; break;
					/* DIAERESIS */
				case 0xAD: *t++ = 0xE2; *t++ = 0x89; *t++ = 0xA0; break;
					/* NOT EQUAL TO */
				case 0xAE: *t++ = 0xC3; *t++ = 0x86; break;
					/* LATIN CAPITAL LETTER AE */
				case 0xAF: *t++ = 0xC3; *t++ = 0x98; break;
					/* LATIN CAPITAL LETTER O WITH STROKE */
				case 0xB0: *t++ = 0xE2; *t++ = 0x88; *t++ = 0x9E; break;
					/* INFINITY */
				case 0xB1: *t++ = 0xC2; *t++ = 0xB1; break;
					/* PLUS-MINUS SIGN */
				case 0xB2: *t++ = 0xE2; *t++ = 0x89; *t++ = 0xA4; break;
					/* LESS-THAN OR EQUAL TO */
				case 0xB3: *t++ = 0xE2; *t++ = 0x89; *t++ = 0xA5; break;
					/* GREATER-THAN OR EQUAL TO */
				case 0xB4: *t++ = 0xC2; *t++ = 0xA5; break;
					/* YEN SIGN */
				case 0xB5: *t++ = 0xC2; *t++ = 0xB5; break;
					/* MICRO SIGN */
				case 0xB6: *t++ = 0xE2; *t++ = 0x88; *t++ = 0x82; break;
					/* PARTIAL DIFFERENTIAL */
				case 0xB7: *t++ = 0xE2; *t++ = 0x88; *t++ = 0x91; break;
					/* N-ARY SUMMATION */
				case 0xB8: *t++ = 0xE2; *t++ = 0x88; *t++ = 0x8F; break;
					/* N-ARY PRODUCT */
				case 0xB9: *t++ = 0xCF; *t++ = 0x80; break;
					/* GREEK SMALL LETTER PI */
				case 0xBA: *t++ = 0xE2; *t++ = 0x88; *t++ = 0xAB; break;
					/* INTEGRAL */
				case 0xBB: *t++ = 0xC2; *t++ = 0xAA; break;
					/* FEMININE ORDINAL INDICATOR */
				case 0xBC: *t++ = 0xC2; *t++ = 0xBA; break;
					/* MASCULINE ORDINAL INDICATOR */
				case 0xBD: *t++ = 0xCE; *t++ = 0xA9; break;
					/* GREEK CAPITAL LETTER OMEGA */
				case 0xBE: *t++ = 0xC3; *t++ = 0xA6; break;
					/* LATIN SMALL LETTER AE */
				case 0xBF: *t++ = 0xC3; *t++ = 0xB8; break;
					/* LATIN SMALL LETTER O WITH STROKE */
				case 0xC0: *t++ = 0xC2; *t++ = 0xBF; break;
					/* INVERTED QUESTION MARK */
				case 0xC1: *t++ = 0xC2; *t++ = 0xA1; break;
					/* INVERTED EXCLAMATION MARK */
				case 0xC2: *t++ = 0xC2; *t++ = 0xAC; break;
					/* NOT SIGN */
				case 0xC3: *t++ = 0xE2; *t++ = 0x88; *t++ = 0x9A; break;
					/* SQUARE ROOT */
				case 0xC4: *t++ = 0xC6; *t++ = 0x92; break;
					/* LATIN SMALL LETTER F WITH HOOK */
				case 0xC5: *t++ = 0xE2; *t++ = 0x89; *t++ = 0x88; break;
					/* ALMOST EQUAL TO */
				case 0xC6: *t++ = 0xE2; *t++ = 0x88; *t++ = 0x86; break;
					/* INCREMENT */
				case 0xC7: *t++ = 0xC2; *t++ = 0xAB; break;
					/* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
				case 0xC8: *t++ = 0xC2; *t++ = 0xBB; break;
					/* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
				case 0xC9: *t++ = 0xE2; *t++ = 0x80; *t++ = 0xA6; break;
					/* HORIZONTAL ELLIPSIS */
				case 0xCA: *t++ = 0xC2; *t++ = 0xA0; break;
					/* NO-BREAK SPACE */
				case 0xCB: *t++ = 0xC3; *t++ = 0x80; break;
					/* LATIN CAPITAL LETTER A WITH GRAVE */
				case 0xCC: *t++ = 0xC3; *t++ = 0x83; break;
					/* LATIN CAPITAL LETTER A WITH TILDE */
				case 0xCD: *t++ = 0xC3; *t++ = 0x95; break;
					/* LATIN CAPITAL LETTER O WITH TILDE */
				case 0xCE: *t++ = 0xC5; *t++ = 0x92; break;
					/* LATIN CAPITAL LIGATURE OE */
				case 0xCF: *t++ = 0xC5; *t++ = 0x93; break;
					/* LATIN SMALL LIGATURE OE */
				case 0xD0: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x93; break;
					/* EN DASH */
				case 0xD1: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x94; break;
					/* EM DASH */
				case 0xD2: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x9C; break;
					/* LEFT DOUBLE QUOTATION MARK */
				case 0xD3: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x9D; break;
					/* RIGHT DOUBLE QUOTATION MARK */
				case 0xD4: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x98; break;
					/* LEFT SINGLE QUOTATION MARK */
				case 0xD5: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x99; break;
					/* RIGHT SINGLE QUOTATION MARK */
				case 0xD6: *t++ = 0xC3; *t++ = 0xB7; break;
					/* DIVISION SIGN */
				case 0xD7: *t++ = 0xE2; *t++ = 0x97; *t++ = 0x8A; break;
					/* LOZENGE */
				case 0xD8: *t++ = 0xC3; *t++ = 0xBF; break;
					/* LATIN SMALL LETTER Y WITH DIAERESIS */
				case 0xD9: *t++ = 0xC5; *t++ = 0xB8; break;
					/* LATIN CAPITAL LETTER Y WITH DIAERESIS */
				case 0xDA: *t++ = 0xE2; *t++ = 0x81; *t++ = 0x84; break;
					/* FRACTION SLASH */
				case 0xDB: *t++ = 0xE2; *t++ = 0x82; *t++ = 0xAC; break;
					/* EURO SIGN */
				case 0xDC: *t++ = 0xE2; *t++ = 0x80; *t++ = 0xB9; break;
					/* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
				case 0xDD: *t++ = 0xE2; *t++ = 0x80; *t++ = 0xBA; break;
					/* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
				case 0xDE: *t++ = 0xEF; *t++ = 0xAC; *t++ = 0x81; break;
					/* LATIN SMALL LIGATURE FI */
				case 0xDF: *t++ = 0xEF; *t++ = 0xAC; *t++ = 0x82; break;
					/* LATIN SMALL LIGATURE FL */
				case 0xE0: *t++ = 0xE2; *t++ = 0x80; *t++ = 0xA1; break;
					/* DOUBLE DAGGER */
				case 0xE1: *t++ = 0xC2; *t++ = 0xB7; break;
					/* MIDDLE DOT */
				case 0xE2: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x9A; break;
					/* SINGLE LOW-9 QUOTATION MARK */
				case 0xE3: *t++ = 0xE2; *t++ = 0x80; *t++ = 0x9E; break;
					/* DOUBLE LOW-9 QUOTATION MARK */
				case 0xE4: *t++ = 0xE2; *t++ = 0x80; *t++ = 0xB0; break;
					/* PER MILLE SIGN */
				case 0xE5: *t++ = 0xC3; *t++ = 0x82; break;
					/* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
				case 0xE6: *t++ = 0xC3; *t++ = 0x8A; break;
					/* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
				case 0xE7: *t++ = 0xC3; *t++ = 0x81; break;
					/* LATIN CAPITAL LETTER A WITH ACUTE */
				case 0xE8: *t++ = 0xC3; *t++ = 0x8B; break;
					/* LATIN CAPITAL LETTER E WITH DIAERESIS */
				case 0xE9: *t++ = 0xC3; *t++ = 0x88; break;
					/* LATIN CAPITAL LETTER E WITH GRAVE */
				case 0xEA: *t++ = 0xC3; *t++ = 0x8D; break;
					/* LATIN CAPITAL LETTER I WITH ACUTE */
				case 0xEB: *t++ = 0xC3; *t++ = 0x8E; break;
					/* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
				case 0xEC: *t++ = 0xC3; *t++ = 0x8F; break;
					/* LATIN CAPITAL LETTER I WITH DIAERESIS */
				case 0xED: *t++ = 0xC3; *t++ = 0x8C; break;
					/* LATIN CAPITAL LETTER I WITH GRAVE */
				case 0xEE: *t++ = 0xC3; *t++ = 0x93; break;
					/* LATIN CAPITAL LETTER O WITH ACUTE */
				case 0xEF: *t++ = 0xC3; *t++ = 0x94; break;
					/* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
				case 0xF0: *t++ = 0xEF; *t++ = 0xA3; *t++ = 0xBF; break;
					/* Apple logo */
				case 0xF1: *t++ = 0xC3; *t++ = 0x92; break;
					/* LATIN CAPITAL LETTER O WITH GRAVE */
				case 0xF2: *t++ = 0xC3; *t++ = 0x9A; break;
					/* LATIN CAPITAL LETTER U WITH ACUTE */
				case 0xF3: *t++ = 0xC3; *t++ = 0x9B; break;
					/* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
				case 0xF4: *t++ = 0xC3; *t++ = 0x99; break;
					/* LATIN CAPITAL LETTER U WITH GRAVE */
				case 0xF5: *t++ = 0xC4; *t++ = 0xB1; break;
					/* LATIN SMALL LETTER DOTLESS I */
				case 0xF6: *t++ = 0xCB; *t++ = 0x86; break;
					/* MODIFIER LETTER CIRCUMFLEX ACCENT */
				case 0xF7: *t++ = 0xCB; *t++ = 0x9C; break;
					/* SMALL TILDE */
				case 0xF8: *t++ = 0xC2; *t++ = 0xAF; break;
					/* MACRON */
				case 0xF9: *t++ = 0xCB; *t++ = 0x98; break;
					/* BREVE */
				case 0xFA: *t++ = 0xCB; *t++ = 0x99; break;
					/* DOT ABOVE */
				case 0xFB: *t++ = 0xCB; *t++ = 0x9A; break;
					/* RING ABOVE */
				case 0xFC: *t++ = 0xC2; *t++ = 0xB8; break;
					/* CEDILLA */
				case 0xFD: *t++ = 0xCB; *t++ = 0x9D; break;
					/* DOUBLE ACUTE ACCENT */
				case 0xFE: *t++ = 0xCB; *t++ = 0x9B; break;
					/* OGONEK */
				case 0xFF: *t++ = 0xCB; *t++ = 0x87; break;
					/* CARON */
				default: *t++ = '?'; break;
					/* shouldn't get here */
			}
		}
	}
}
#endif

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEexport(tPbuf i)
{
	tMacErr err;
	char *p;
	ui3p s = PbufDat[i];
	uimr L = PbufSize[i];
	uimr sz = MacRoman2UniCodeSize(s, L);

	if (NULL == (p = malloc(sz + 1))) {
		err = mnvm_miscErr;
	} else {
		MacRoman2UniCodeData(s, L, p);
		p[sz] = 0;

		if (0 != SDL_SetClipboardText(p)) {
			err = mnvm_miscErr;
		} else {
			err = mnvm_noErr;
		}
		free(p);
	}

	return err;
}
#endif

#if IncludeHostTextClipExchange
LOCALFUNC tMacErr UniCodeStrLength(char *s, uimr *r)
{
	tMacErr err;
	ui3r t;
	ui3r t2;
	char *p = s;
	uimr L = 0;

label_retry:
	if (0 == (t = *p++)) {
		err = mnvm_noErr;
		/* done */
	} else
	if (0 == (0x80 & t)) {
		/* One-byte code */
		L += 1;
		goto label_retry;
	} else
	if (0 == (0x40 & t)) {
		/* continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (t2 = *p++)) {
		err = mnvm_miscErr;
	} else
	if (0x80 != (0xC0 & t2)) {
		/* not a continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (0x20 & t)) {
		/* two bytes */
		L += 2;
		goto label_retry;
	} else
	if (0 == (t2 = *p++)) {
		err = mnvm_miscErr;
	} else
	if (0x80 != (0xC0 & t2)) {
		/* not a continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (0x10 & t)) {
		/* three bytes */
		L += 3;
		goto label_retry;
	} else
	if (0 == (t2 = *p++)) {
		err = mnvm_miscErr;
	} else
	if (0x80 != (0xC0 & t2)) {
		/* not a continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (0x08 & t)) {
		/* four bytes */
		L += 5;
		goto label_retry;
	} else
	{
		err = mnvm_miscErr;
		/* longer code not supported yet */
	}

	*r = L;
	return err;
}
#endif

#if IncludeHostTextClipExchange
LOCALFUNC ui3r UniCodePoint2MacRoman(ui5r x)
{
/*
	adapted from
		http://www.unicode.org/Public/MAPPINGS/VENDORS/APPLE/ROMAN.TXT
*/
	ui3r y;

	if (x < 128) {
		y = x;
	} else {
		switch (x) {
			case 0x00C4: y = 0x80; break;
				/* LATIN CAPITAL LETTER A WITH DIAERESIS */
			case 0x00C5: y = 0x81; break;
				/* LATIN CAPITAL LETTER A WITH RING ABOVE */
			case 0x00C7: y = 0x82; break;
				/* LATIN CAPITAL LETTER C WITH CEDILLA */
			case 0x00C9: y = 0x83; break;
				/* LATIN CAPITAL LETTER E WITH ACUTE */
			case 0x00D1: y = 0x84; break;
				/* LATIN CAPITAL LETTER N WITH TILDE */
			case 0x00D6: y = 0x85; break;
				/* LATIN CAPITAL LETTER O WITH DIAERESIS */
			case 0x00DC: y = 0x86; break;
				/* LATIN CAPITAL LETTER U WITH DIAERESIS */
			case 0x00E1: y = 0x87; break;
				/* LATIN SMALL LETTER A WITH ACUTE */
			case 0x00E0: y = 0x88; break;
				/* LATIN SMALL LETTER A WITH GRAVE */
			case 0x00E2: y = 0x89; break;
				/* LATIN SMALL LETTER A WITH CIRCUMFLEX */
			case 0x00E4: y = 0x8A; break;
				/* LATIN SMALL LETTER A WITH DIAERESIS */
			case 0x00E3: y = 0x8B; break;
				/* LATIN SMALL LETTER A WITH TILDE */
			case 0x00E5: y = 0x8C; break;
				/* LATIN SMALL LETTER A WITH RING ABOVE */
			case 0x00E7: y = 0x8D; break;
				/* LATIN SMALL LETTER C WITH CEDILLA */
			case 0x00E9: y = 0x8E; break;
				/* LATIN SMALL LETTER E WITH ACUTE */
			case 0x00E8: y = 0x8F; break;
				/* LATIN SMALL LETTER E WITH GRAVE */
			case 0x00EA: y = 0x90; break;
				/* LATIN SMALL LETTER E WITH CIRCUMFLEX */
			case 0x00EB: y = 0x91; break;
				/* LATIN SMALL LETTER E WITH DIAERESIS */
			case 0x00ED: y = 0x92; break;
				/* LATIN SMALL LETTER I WITH ACUTE */
			case 0x00EC: y = 0x93; break;
				/* LATIN SMALL LETTER I WITH GRAVE */
			case 0x00EE: y = 0x94; break;
				/* LATIN SMALL LETTER I WITH CIRCUMFLEX */
			case 0x00EF: y = 0x95; break;
				/* LATIN SMALL LETTER I WITH DIAERESIS */
			case 0x00F1: y = 0x96; break;
				/* LATIN SMALL LETTER N WITH TILDE */
			case 0x00F3: y = 0x97; break;
				/* LATIN SMALL LETTER O WITH ACUTE */
			case 0x00F2: y = 0x98; break;
				/* LATIN SMALL LETTER O WITH GRAVE */
			case 0x00F4: y = 0x99; break;
				/* LATIN SMALL LETTER O WITH CIRCUMFLEX */
			case 0x00F6: y = 0x9A; break;
				/* LATIN SMALL LETTER O WITH DIAERESIS */
			case 0x00F5: y = 0x9B; break;
				/* LATIN SMALL LETTER O WITH TILDE */
			case 0x00FA: y = 0x9C; break;
				/* LATIN SMALL LETTER U WITH ACUTE */
			case 0x00F9: y = 0x9D; break;
				/* LATIN SMALL LETTER U WITH GRAVE */
			case 0x00FB: y = 0x9E; break;
				/* LATIN SMALL LETTER U WITH CIRCUMFLEX */
			case 0x00FC: y = 0x9F; break;
				/* LATIN SMALL LETTER U WITH DIAERESIS */
			case 0x2020: y = 0xA0; break;
				/* DAGGER */
			case 0x00B0: y = 0xA1; break;
				/* DEGREE SIGN */
			case 0x00A2: y = 0xA2; break;
				/* CENT SIGN */
			case 0x00A3: y = 0xA3; break;
				/* POUND SIGN */
			case 0x00A7: y = 0xA4; break;
				/* SECTION SIGN */
			case 0x2022: y = 0xA5; break;
				/* BULLET */
			case 0x00B6: y = 0xA6; break;
				/* PILCROW SIGN */
			case 0x00DF: y = 0xA7; break;
				/* LATIN SMALL LETTER SHARP S */
			case 0x00AE: y = 0xA8; break;
				/* REGISTERED SIGN */
			case 0x00A9: y = 0xA9; break;
				/* COPYRIGHT SIGN */
			case 0x2122: y = 0xAA; break;
				/* TRADE MARK SIGN */
			case 0x00B4: y = 0xAB; break;
				/* ACUTE ACCENT */
			case 0x00A8: y = 0xAC; break;
				/* DIAERESIS */
			case 0x2260: y = 0xAD; break;
				/* NOT EQUAL TO */
			case 0x00C6: y = 0xAE; break;
				/* LATIN CAPITAL LETTER AE */
			case 0x00D8: y = 0xAF; break;
				/* LATIN CAPITAL LETTER O WITH STROKE */
			case 0x221E: y = 0xB0; break;
				/* INFINITY */
			case 0x00B1: y = 0xB1; break;
				/* PLUS-MINUS SIGN */
			case 0x2264: y = 0xB2; break;
				/* LESS-THAN OR EQUAL TO */
			case 0x2265: y = 0xB3; break;
				/* GREATER-THAN OR EQUAL TO */
			case 0x00A5: y = 0xB4; break;
				/* YEN SIGN */
			case 0x00B5: y = 0xB5; break;
				/* MICRO SIGN */
			case 0x2202: y = 0xB6; break;
				/* PARTIAL DIFFERENTIAL */
			case 0x2211: y = 0xB7; break;
				/* N-ARY SUMMATION */
			case 0x220F: y = 0xB8; break;
				/* N-ARY PRODUCT */
			case 0x03C0: y = 0xB9; break;
				/* GREEK SMALL LETTER PI */
			case 0x222B: y = 0xBA; break;
				/* INTEGRAL */
			case 0x00AA: y = 0xBB; break;
				/* FEMININE ORDINAL INDICATOR */
			case 0x00BA: y = 0xBC; break;
				/* MASCULINE ORDINAL INDICATOR */
			case 0x03A9: y = 0xBD; break;
				/* GREEK CAPITAL LETTER OMEGA */
			case 0x00E6: y = 0xBE; break;
				/* LATIN SMALL LETTER AE */
			case 0x00F8: y = 0xBF; break;
				/* LATIN SMALL LETTER O WITH STROKE */
			case 0x00BF: y = 0xC0; break;
				/* INVERTED QUESTION MARK */
			case 0x00A1: y = 0xC1; break;
				/* INVERTED EXCLAMATION MARK */
			case 0x00AC: y = 0xC2; break;
				/* NOT SIGN */
			case 0x221A: y = 0xC3; break;
				/* SQUARE ROOT */
			case 0x0192: y = 0xC4; break;
				/* LATIN SMALL LETTER F WITH HOOK */
			case 0x2248: y = 0xC5; break;
				/* ALMOST EQUAL TO */
			case 0x2206: y = 0xC6; break;
				/* INCREMENT */
			case 0x00AB: y = 0xC7; break;
				/* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
			case 0x00BB: y = 0xC8; break;
				/* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
			case 0x2026: y = 0xC9; break;
				/* HORIZONTAL ELLIPSIS */
			case 0x00A0: y = 0xCA; break;
				/* NO-BREAK SPACE */
			case 0x00C0: y = 0xCB; break;
				/* LATIN CAPITAL LETTER A WITH GRAVE */
			case 0x00C3: y = 0xCC; break;
				/* LATIN CAPITAL LETTER A WITH TILDE */
			case 0x00D5: y = 0xCD; break;
				/* LATIN CAPITAL LETTER O WITH TILDE */
			case 0x0152: y = 0xCE; break;
				/* LATIN CAPITAL LIGATURE OE */
			case 0x0153: y = 0xCF; break;
				/* LATIN SMALL LIGATURE OE */
			case 0x2013: y = 0xD0; break;
				/* EN DASH */
			case 0x2014: y = 0xD1; break;
				/* EM DASH */
			case 0x201C: y = 0xD2; break;
				/* LEFT DOUBLE QUOTATION MARK */
			case 0x201D: y = 0xD3; break;
				/* RIGHT DOUBLE QUOTATION MARK */
			case 0x2018: y = 0xD4; break;
				/* LEFT SINGLE QUOTATION MARK */
			case 0x2019: y = 0xD5; break;
				/* RIGHT SINGLE QUOTATION MARK */
			case 0x00F7: y = 0xD6; break;
				/* DIVISION SIGN */
			case 0x25CA: y = 0xD7; break;
				/* LOZENGE */
			case 0x00FF: y = 0xD8; break;
				/* LATIN SMALL LETTER Y WITH DIAERESIS */
			case 0x0178: y = 0xD9; break;
				/* LATIN CAPITAL LETTER Y WITH DIAERESIS */
			case 0x2044: y = 0xDA; break;
				/* FRACTION SLASH */
			case 0x20AC: y = 0xDB; break;
				/* EURO SIGN */
			case 0x2039: y = 0xDC; break;
				/* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
			case 0x203A: y = 0xDD; break;
				/* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
			case 0xFB01: y = 0xDE; break;
				/* LATIN SMALL LIGATURE FI */
			case 0xFB02: y = 0xDF; break;
				/* LATIN SMALL LIGATURE FL */
			case 0x2021: y = 0xE0; break;
				/* DOUBLE DAGGER */
			case 0x00B7: y = 0xE1; break;
				/* MIDDLE DOT */
			case 0x201A: y = 0xE2; break;
				/* SINGLE LOW-9 QUOTATION MARK */
			case 0x201E: y = 0xE3; break;
				/* DOUBLE LOW-9 QUOTATION MARK */
			case 0x2030: y = 0xE4; break;
				/* PER MILLE SIGN */
			case 0x00C2: y = 0xE5; break;
				/* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
			case 0x00CA: y = 0xE6; break;
				/* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
			case 0x00C1: y = 0xE7; break;
				/* LATIN CAPITAL LETTER A WITH ACUTE */
			case 0x00CB: y = 0xE8; break;
				/* LATIN CAPITAL LETTER E WITH DIAERESIS */
			case 0x00C8: y = 0xE9; break;
				/* LATIN CAPITAL LETTER E WITH GRAVE */
			case 0x00CD: y = 0xEA; break;
				/* LATIN CAPITAL LETTER I WITH ACUTE */
			case 0x00CE: y = 0xEB; break;
				/* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
			case 0x00CF: y = 0xEC; break;
				/* LATIN CAPITAL LETTER I WITH DIAERESIS */
			case 0x00CC: y = 0xED; break;
				/* LATIN CAPITAL LETTER I WITH GRAVE */
			case 0x00D3: y = 0xEE; break;
				/* LATIN CAPITAL LETTER O WITH ACUTE */
			case 0x00D4: y = 0xEF; break;
				/* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
			case 0xF8FF: y = 0xF0; break;
				/* Apple logo */
			case 0x00D2: y = 0xF1; break;
				/* LATIN CAPITAL LETTER O WITH GRAVE */
			case 0x00DA: y = 0xF2; break;
				/* LATIN CAPITAL LETTER U WITH ACUTE */
			case 0x00DB: y = 0xF3; break;
				/* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
			case 0x00D9: y = 0xF4; break;
				/* LATIN CAPITAL LETTER U WITH GRAVE */
			case 0x0131: y = 0xF5; break;
				/* LATIN SMALL LETTER DOTLESS I */
			case 0x02C6: y = 0xF6; break;
				/* MODIFIER LETTER CIRCUMFLEX ACCENT */
			case 0x02DC: y = 0xF7; break;
				/* SMALL TILDE */
			case 0x00AF: y = 0xF8; break;
				/* MACRON */
			case 0x02D8: y = 0xF9; break;
				/* BREVE */
			case 0x02D9: y = 0xFA; break;
				/* DOT ABOVE */
			case 0x02DA: y = 0xFB; break;
				/* RING ABOVE */
			case 0x00B8: y = 0xFC; break;
				/* CEDILLA */
			case 0x02DD: y = 0xFD; break;
				/* DOUBLE ACUTE ACCENT */
			case 0x02DB: y = 0xFE; break;
				/* OGONEK */
			case 0x02C7: y = 0xFF; break;
				/* CARON */
			default: y = '?'; break;
				/* unrecognized */
		}
	}

	return y;
}
#endif

#if IncludeHostTextClipExchange
LOCALPROC UniCodeStr2MacRoman(char *s, char *r)
{
	tMacErr err;
	ui3r t;
	ui3r t2;
	ui3r t3;
	ui3r t4;
	ui5r v;
	char *p = s;
	char *q = r;

label_retry:
	if (0 == (t = *p++)) {
		err = mnvm_noErr;
		/* done */
	} else
	if (0 == (0x80 & t)) {
		*q++ = t;
		goto label_retry;
	} else
	if (0 == (0x40 & t)) {
		/* continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (t2 = *p++)) {
		err = mnvm_miscErr;
	} else
	if (0x80 != (0xC0 & t2)) {
		/* not a continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (0x20 & t)) {
		/* two bytes */
		v = t & 0x1F;
		v = (v << 6) | (t2 & 0x3F);
		*q++ = UniCodePoint2MacRoman(v);
		goto label_retry;
	} else
	if (0 == (t3 = *p++)) {
		err = mnvm_miscErr;
	} else
	if (0x80 != (0xC0 & t3)) {
		/* not a continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (0x10 & t)) {
		/* three bytes */
		v = t & 0x0F;
		v = (v << 6) | (t3 & 0x3F);
		v = (v << 6) | (t2 & 0x3F);
		*q++ = UniCodePoint2MacRoman(v);
		goto label_retry;
	} else
	if (0 == (t4 = *p++)) {
		err = mnvm_miscErr;
	} else
	if (0x80 != (0xC0 & t4)) {
		/* not a continuation code, error */
		err = mnvm_miscErr;
	} else
	if (0 == (0x08 & t)) {
		/* four bytes */
		v = t & 0x07;
		v = (v << 6) | (t4 & 0x3F);
		v = (v << 6) | (t3 & 0x3F);
		v = (v << 6) | (t2 & 0x3F);
		*q++ = UniCodePoint2MacRoman(v);
		goto label_retry;
	} else
	{
		err = mnvm_miscErr;
		/* longer code not supported yet */
	}
}
#endif

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEimport(tPbuf *r)
{
	tMacErr err;
	uimr L;
	char *s = NULL;
	tPbuf t = NotAPbuf;

	if (NULL == (s = SDL_GetClipboardText())) {
		err = mnvm_miscErr;
	} else
	if (mnvm_noErr != (err =
		UniCodeStrLength(s, &L)))
	{
		/* fail */
	} else
	if (mnvm_noErr != (err =
		PbufNew(L, &t)))
	{
		/* fail */
	} else
	{
		err = mnvm_noErr;

		UniCodeStr2MacRoman(s, PbufDat[t]);
		*r = t;
		t = NotAPbuf;
	}

	if (NotAPbuf != t) {
		PbufDispose(t);
	}
	if (NULL != s) {
		SDL_free(s);
	}

	return err;
}
#endif

/* --- event handling for main window --- */

#define UseMotionEvents 1

#if UseMotionEvents
LOCALVAR blnr CaughtMouse = falseblnr;
#endif

LOCALPROC HandleTheEvent(SDL_Event *event)
{
	switch (event->type) {
		case SDL_QUIT:
			RequestMacOff = trueblnr;
			break;
		case SDL_WINDOWEVENT:
			switch (event->window.event) {
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					gTrueBackgroundFlag = 0;
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					gTrueBackgroundFlag = 1;
					break;
				case SDL_WINDOWEVENT_ENTER:
					CaughtMouse = 1;
					break;
				case SDL_WINDOWEVENT_LEAVE:
					CaughtMouse = 0;
					break;
			}
			break;
		case SDL_MOUSEMOTION:
#if EnableFSMouseMotion && ! HaveWorkingWarp
			if (HaveMouseMotion) {
				MousePositionNotifyRelative(
					event->motion.xrel, event->motion.yrel);
			} else
#endif
			{
				MousePositionNotify(
					event->motion.x, event->motion.y);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			/* any mouse button, we don't care which */
#if EnableFSMouseMotion && ! HaveWorkingWarp
			if (HaveMouseMotion) {
				/* ignore position */
			} else
#endif
			{
				MousePositionNotify(
					event->button.x, event->button.y);
			}
			MyMouseButtonSet(trueblnr);
			break;
		case SDL_MOUSEBUTTONUP:
#if EnableFSMouseMotion && ! HaveWorkingWarp
			if (HaveMouseMotion) {
				/* ignore position */
			} else
#endif
			{
				MousePositionNotify(
					event->button.x, event->button.y);
			}
			MyMouseButtonSet(falseblnr);
			break;
		case SDL_KEYDOWN:
			DoKeyCode(&event->key.keysym, trueblnr);
			break;
		case SDL_KEYUP:
			DoKeyCode(&event->key.keysym, falseblnr);
			break;
		case SDL_MOUSEWHEEL:
			if (event->wheel.x < 0) {
				Keyboard_UpdateKeyMap2(MKC_Left, trueblnr);
				Keyboard_UpdateKeyMap2(MKC_Left, falseblnr);
			} else if (event->wheel.x > 0) {
				Keyboard_UpdateKeyMap2(MKC_Right, trueblnr);
				Keyboard_UpdateKeyMap2(MKC_Right, falseblnr);
			}
			if (event->wheel.y < 0) {
				Keyboard_UpdateKeyMap2(MKC_Down, trueblnr);
				Keyboard_UpdateKeyMap2(MKC_Down, falseblnr);
			} else if(event->wheel.y > 0) {
				Keyboard_UpdateKeyMap2(MKC_Up, trueblnr);
				Keyboard_UpdateKeyMap2(MKC_Up, falseblnr);
			}
			break;
		case SDL_DROPFILE:
			{
				char *s = event->drop.file;

				(void) Sony_Insert1a(s, falseblnr);
				SDL_RaiseWindow(my_main_wind);
				SDL_free(s);
			}
			break;
#if 0
		case Expose: /* SDL doesn't have an expose event */
			int x0 = event->expose.x;
			int y0 = event->expose.y;
			int x1 = x0 + event->expose.width;
			int y1 = y0 + event->expose.height;

			if (x0 < 0) {
				x0 = 0;
			}
			if (x1 > vMacScreenWidth) {
				x1 = vMacScreenWidth;
			}
			if (y0 < 0) {
				y0 = 0;
			}
			if (y1 > vMacScreenHeight) {
				y1 = vMacScreenHeight;
			}
			if ((x0 < x1) && (y0 < y1)) {
				HaveChangedScreenBuff(y0, x0, y1, x1);
			}
			break;
#endif
	}
}

/* --- main window creation and disposal --- */

LOCALVAR int my_argc;
LOCALVAR char **my_argv;

LOCALFUNC blnr Screen_Init(void)
{
	blnr v = falseblnr;

	InitKeyCodes();

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	} else {
		v = trueblnr;
	}

	return v;
}

#if MayFullScreen
LOCALVAR blnr GrabMachine = falseblnr;
#endif

#if MayFullScreen
LOCALPROC GrabTheMachine(void)
{
#if GrabKeysFullScreen
	SDL_SetWindowGrab(my_main_wind, SDL_TRUE);
#endif

#if EnableFSMouseMotion

#if HaveWorkingWarp
	/*
		if magnification changes, need to reset,
		even if HaveMouseMotion already true
	*/
	if (MyMoveMouse(ViewHStart + (ViewHSize / 2),
		ViewVStart + (ViewVSize / 2)))
	{
		SavedMouseH = ViewHStart + (ViewHSize / 2);
		SavedMouseV = ViewVStart + (ViewVSize / 2);
		HaveMouseMotion = trueblnr;
	}
#else
	if (0 == SDL_SetRelativeMouseMode(SDL_ENABLE)) {
		HaveMouseMotion = trueblnr;
	}
#endif

#endif /* EnableFSMouseMotion */
}
#endif

#if MayFullScreen
LOCALPROC UngrabMachine(void)
{
#if EnableFSMouseMotion

	if (HaveMouseMotion) {
#if HaveWorkingWarp
		(void) MyMoveMouse(CurMouseH, CurMouseV);
#else
		SDL_SetRelativeMouseMode(SDL_DISABLE);
#endif

		HaveMouseMotion = falseblnr;
	}

#endif /* EnableFSMouseMotion */

#if GrabKeysFullScreen
	SDL_SetWindowGrab(my_main_wind, SDL_FALSE);
#endif
}
#endif

#if EnableFSMouseMotion && HaveWorkingWarp
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

enum {
	kMagStateNormal,
#if EnableMagnify
	kMagStateMagnifgy,
#endif
	kNumMagStates
};

#define kMagStateAuto kNumMagStates

#if MayNotFullScreen
LOCALVAR int CurWinIndx;
LOCALVAR blnr HavePositionWins[kNumMagStates];
LOCALVAR int WinPositionsX[kNumMagStates];
LOCALVAR int WinPositionsY[kNumMagStates];
#endif

LOCALFUNC blnr CreateMainWindow(void)
{
	int NewWindowX;
	int NewWindowY;
	int NewWindowHeight = vMacScreenHeight;
	int NewWindowWidth = vMacScreenWidth;
	Uint32 flags = 0 /* SDL_WINDOW_HIDDEN */;
	blnr v = falseblnr;

#if EnableMagnify && 1
	if (UseMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		/*
			We don't want physical screen mode to be changed in modern
			displays, so we pass this _DESKTOP flag.
		*/
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		NewWindowX = SDL_WINDOWPOS_UNDEFINED;
		NewWindowY = SDL_WINDOWPOS_UNDEFINED;
	}
#endif
#if VarFullScreen
	else
#endif
#if MayNotFullScreen
	{
		int WinIndx;

#if EnableMagnify
		if (UseMagnify) {
			WinIndx = kMagStateMagnifgy;
		} else
#endif
		{
			WinIndx = kMagStateNormal;
		}

		if (! HavePositionWins[WinIndx]) {
			NewWindowX = SDL_WINDOWPOS_CENTERED;
			NewWindowY = SDL_WINDOWPOS_CENTERED;
		} else {
			NewWindowX = WinPositionsX[WinIndx];
			NewWindowY = WinPositionsY[WinIndx];
		}

		CurWinIndx = WinIndx;
	}
#endif

#if 0
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
#endif

	if (NULL == (my_main_wind = SDL_CreateWindow(
		(NULL != n_arg) ? n_arg : kStrAppName,
		NewWindowX, NewWindowY,
		NewWindowWidth, NewWindowHeight,
		flags)))
	{
		fprintf(stderr, "SDL_CreateWindow fails: %s\n",
			SDL_GetError());
	} else
	if (NULL == (my_renderer = SDL_CreateRenderer(
		my_main_wind, -1,
		0 /* SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC */
			/*
				SDL_RENDERER_ACCELERATED not needed
				"no flags gives priority to available
				SDL_RENDERER_ACCELERATED renderers"
			*/
			/* would rather not require vsync */
		)))
	{
		fprintf(stderr, "SDL_CreateRenderer fails: %s\n",
			SDL_GetError());
	} else
	if (NULL == (my_texture = SDL_CreateTexture(
		my_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
#if UseSDLscaling
		vMacScreenWidth, vMacScreenHeight
#else
		NewWindowWidth, NewWindowHeight
#endif
		)))
	{
		fprintf(stderr, "SDL_CreateTexture fails: %s\n",
			SDL_GetError());
	} else
	if (NULL == (my_format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888)))
	{
		fprintf(stderr, "SDL_AllocFormat fails: %s\n",
			SDL_GetError());
	} else
	{
		/* SDL_ShowWindow(my_main_wind); */

		SDL_RenderClear(my_renderer);

#if 0
		SDL_DisplayMode info;

		if (0 != SDL_GetCurrentDisplayMode(0, &info)) {
			fprintf(stderr, "SDL_GetCurrentDisplayMode fails: %s\n",
				SDL_GetError());

			return falseblnr;
		}
#endif

#if VarFullScreen
		if (UseFullScreen)
#endif
#if MayFullScreen
		{
			int wr;
			int hr;

			SDL_GL_GetDrawableSize(my_main_wind, &wr, &hr);

			ViewHSize = wr;
			ViewVSize = hr;
#if EnableMagnify
			if (UseMagnify) {
				ViewHSize /= MyWindowScale;
				ViewVSize /= MyWindowScale;
			}
#endif
			if (ViewHSize >= vMacScreenWidth) {
				ViewHStart = 0;
				ViewHSize = vMacScreenWidth;
			} else {
				ViewHSize &= ~ 1;
			}
			if (ViewVSize >= vMacScreenHeight) {
				ViewVStart = 0;
				ViewVSize = vMacScreenHeight;
			} else {
				ViewVSize &= ~ 1;
			}

			if (wr > NewWindowWidth) {
				hOffset = (wr - NewWindowWidth) / 2;
			} else {
				hOffset = 0;
			}
			if (hr > NewWindowHeight) {
				vOffset = (hr - NewWindowHeight) / 2;
			} else {
				vOffset = 0;
			}
		}
#endif

#if 0 != vMacScreenDepth
		ColorModeWorks = trueblnr;
#endif

		v = trueblnr;
	}

	return v;
}

LOCALPROC CloseMainWindow(void)
{
	if (NULL != my_format) {
		SDL_FreeFormat(my_format);
		my_format = NULL;
	}

	if (NULL != my_texture) {
		SDL_DestroyTexture(my_texture);
		my_texture = NULL;
	}

	if (NULL != my_renderer) {
		SDL_DestroyRenderer(my_renderer);
		my_renderer = NULL;
	}

	if (NULL != my_main_wind) {
		SDL_DestroyWindow(my_main_wind);
		my_main_wind = NULL;
	}
}

#if EnableRecreateW
LOCALPROC ZapMyWState(void)
{
	my_main_wind = NULL;
	my_renderer = NULL;
	my_texture = NULL;
	my_format = NULL;
}
#endif

#if EnableRecreateW
struct MyWState {
#if MayFullScreen
	ui4r f_ViewHSize;
	ui4r f_ViewVSize;
	ui4r f_ViewHStart;
	ui4r f_ViewVStart;
	int f_hOffset;
	int f_vOffset;
#endif
#if VarFullScreen
	blnr f_UseFullScreen;
#endif
#if EnableMagnify
	blnr f_UseMagnify;
#endif
#if MayNotFullScreen
	int f_CurWinIndx;
#endif
	SDL_Window *f_my_main_wind;
	SDL_Renderer *f_my_renderer;
	SDL_Texture *f_my_texture;
	SDL_PixelFormat *f_my_format;
};
typedef struct MyWState MyWState;
#endif

#if EnableRecreateW
LOCALPROC GetMyWState(MyWState *r)
{
#if MayFullScreen
	r->f_ViewHSize = ViewHSize;
	r->f_ViewVSize = ViewVSize;
	r->f_ViewHStart = ViewHStart;
	r->f_ViewVStart = ViewVStart;
	r->f_hOffset = hOffset;
	r->f_vOffset = vOffset;
#endif
#if VarFullScreen
	r->f_UseFullScreen = UseFullScreen;
#endif
#if EnableMagnify
	r->f_UseMagnify = UseMagnify;
#endif
#if MayNotFullScreen
	r->f_CurWinIndx = CurWinIndx;
#endif
	r->f_my_main_wind = my_main_wind;
	r->f_my_renderer = my_renderer;
	r->f_my_texture = my_texture;
	r->f_my_format = my_format;
}
#endif

#if EnableRecreateW
LOCALPROC SetMyWState(MyWState *r)
{
#if MayFullScreen
	ViewHSize = r->f_ViewHSize;
	ViewVSize = r->f_ViewVSize;
	ViewHStart = r->f_ViewHStart;
	ViewVStart = r->f_ViewVStart;
	hOffset = r->f_hOffset;
	vOffset = r->f_vOffset;
#endif
#if VarFullScreen
	UseFullScreen = r->f_UseFullScreen;
#endif
#if EnableMagnify
	UseMagnify = r->f_UseMagnify;
#endif
#if MayNotFullScreen
	CurWinIndx = r->f_CurWinIndx;
#endif
	my_main_wind = r->f_my_main_wind;
	my_renderer = r->f_my_renderer;
	my_texture = r->f_my_texture;
	my_format = r->f_my_format;
}
#endif

#if VarFullScreen && EnableMagnify
enum {
	kWinStateWindowed,
#if EnableMagnify
	kWinStateFullScreen,
#endif
	kNumWinStates
};
#endif

#if VarFullScreen && EnableMagnify
LOCALVAR int WinMagStates[kNumWinStates];
#endif

#if EnableRecreateW
LOCALFUNC blnr ReCreateMainWindow(void)
{
	MyWState old_state;
	MyWState new_state;
#if HaveWorkingWarp
	blnr HadCursorHidden = HaveCursorHidden;
#endif
#if VarFullScreen && EnableMagnify
	int OldWinState =
		UseFullScreen ? kWinStateFullScreen : kWinStateWindowed;
	int OldMagState =
		UseMagnify ? kMagStateMagnifgy : kMagStateNormal;

	WinMagStates[OldWinState] =
		OldMagState;
#endif

#if VarFullScreen
	if (! UseFullScreen)
#endif
#if MayNotFullScreen
	{
		SDL_GetWindowPosition(my_main_wind,
			&WinPositionsX[CurWinIndx],
			&WinPositionsY[CurWinIndx]);
		HavePositionWins[CurWinIndx] = trueblnr;
	}
#endif

	ForceShowCursor(); /* hide/show cursor api is per window */

#if MayFullScreen
	if (GrabMachine) {
		GrabMachine = falseblnr;
		UngrabMachine();
	}
#endif

	GetMyWState(&old_state);

	ZapMyWState();

#if EnableMagnify
	UseMagnify = WantMagnify;
#endif
#if VarFullScreen
	UseFullScreen = WantFullScreen;
#endif

	if (! CreateMainWindow()) {
		CloseMainWindow();
		SetMyWState(&old_state);

		/* avoid retry */
#if VarFullScreen
		WantFullScreen = UseFullScreen;
#endif
#if EnableMagnify
		WantMagnify = UseMagnify;
#endif

	} else {
		GetMyWState(&new_state);
		SetMyWState(&old_state);
		CloseMainWindow();
		SetMyWState(&new_state);

#if HaveWorkingWarp
		if (HadCursorHidden) {
			(void) MyMoveMouse(CurMouseH, CurMouseV);
		}
#endif
	}

	return trueblnr;
}
#endif

LOCALPROC ZapWinStateVars(void)
{
#if MayNotFullScreen
	{
		int i;

		for (i = 0; i < kNumMagStates; ++i) {
			HavePositionWins[i] = falseblnr;
		}
	}
#endif
#if VarFullScreen && EnableMagnify
	{
		int i;

		for (i = 0; i < kNumWinStates; ++i) {
			WinMagStates[i] = kMagStateAuto;
		}
	}
#endif
}

#if VarFullScreen
LOCALPROC ToggleWantFullScreen(void)
{
	WantFullScreen = ! WantFullScreen;

#if EnableMagnify
	{
		int OldWinState =
			UseFullScreen ? kWinStateFullScreen : kWinStateWindowed;
		int OldMagState =
			UseMagnify ? kMagStateMagnifgy : kMagStateNormal;
		int NewWinState =
			WantFullScreen ? kWinStateFullScreen : kWinStateWindowed;
		int NewMagState = WinMagStates[NewWinState];

		WinMagStates[OldWinState] = OldMagState;
		if (kMagStateAuto != NewMagState) {
			WantMagnify = (kMagStateMagnifgy == NewMagState);
		} else {
			WantMagnify = falseblnr;
			if (WantFullScreen) {
				SDL_Rect r;

				if (0 == SDL_GetDisplayBounds(0, &r)) {
					if ((r.w >= vMacScreenWidth * MyWindowScale)
						&& (r.h >= vMacScreenHeight * MyWindowScale)
						)
					{
						WantMagnify = trueblnr;
					}
				}
			}
		}
	}
#endif
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

	ForceShowCursor();
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

#if EnableFSMouseMotion && HaveWorkingWarp
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

#if EnableRecreateW
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

#if NeedRequestIthDisk
	if (0 != RequestIthDisk) {
		Sony_InsertIth(RequestIthDisk);
		RequestIthDisk = 0;
	}
#endif

	if (HaveCursorHidden != (WantCursorHidden
		&& ! (gTrueBackgroundFlag || CurSpeedStopped)))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		(void) SDL_ShowCursor(
			HaveCursorHidden ? SDL_DISABLE : SDL_ENABLE);
	}
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
			if (0 == strcmp(pa, "-n"))
			{
				if (i < my_argc) {
					n_arg = my_argv[i++];
					goto label_retry;
				}
			} else
			if (0 == strcmp(pa, "-d"))
			{
				if (i < my_argc) {
					d_arg = my_argv[i++];
					goto label_retry;
				}
			} else
			if (('p' == pa[1]) && ('s' == pa[2]) && ('n' == pa[3]))
			{
				/* seen in OS X. ignore */
				goto label_retry;
			} else
			{
				MacMsg(kStrBadArgTitle, kStrBadArgMessage, falseblnr);
#if dbglog_HAVE
				dbglog_writeln("bad command line argument");
				dbglog_writeln(pa);
#endif
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
	SDL_Event event;

	if (SDL_WaitEvent(&event)) {
		HandleTheEvent(&event);
	}
}

LOCALPROC CheckForSystemEvents(void)
{
	SDL_Event event;
	int i = 10;

	while ((--i >= 0) && SDL_PollEvent(&event)) {
		HandleTheEvent(&event);
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
		(void) SDL_Delay(NextIntTime - LastTime);
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

	ReserveAllocOneBlock(&CLUT_final, CLUT_finalsz, 5, falseblnr);
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

#if CanGetAppPath
LOCALFUNC blnr InitWhereAmI(void)
{
	app_parent = SDL_GetBasePath();

	pref_dir = SDL_GetPrefPath("gryphel", "minivmac");

	return trueblnr; /* keep going regardless */
}
#endif

#if CanGetAppPath
LOCALPROC UninitWhereAmI(void)
{
	SDL_free(pref_dir);

	SDL_free(app_parent);
}
#endif

LOCALFUNC blnr InitOSGLU(void)
{
	if (AllocMyMemory())
#if CanGetAppPath
	if (InitWhereAmI())
#endif
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

	ForceShowCursor();

#if dbglog_HAVE
	dbglog_close();
#endif

#if CanGetAppPath
	UninitWhereAmI();
#endif
	UnallocMyMemory();

	CheckSavedMacMsg();

	CloseMainWindow();

	SDL_Quit();
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
