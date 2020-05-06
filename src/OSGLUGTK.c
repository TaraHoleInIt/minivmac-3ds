/*
	OSGLUGTK.c

	Copyright (C) 2009 Paul C. Pratt

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
	Operating System GLUe for GTK

	All operating system dependent code for the
	GIMP Toolkit should go here.
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

#include "STRCONST.h"

#include "COMOSGLU.h"

/* --- some simple utilities --- */

GLOBALOSGLUPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- control mode and internationalization --- */

#define NeedCell2PlainAsciiMap 1

#include "INTLCHAR.h"

#include "CONTROLM.h"

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

/* --- debug settings and utilities --- */

#define MyDbgEvents (dbglog_HAVE && 1)

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

/* --- text translation --- */

LOCALPROC NativeStrFromCStr(char *r, char *s, blnr AddEllipsis)
{
	ui3b ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i) {
		r[i] = Cell2PlainAsciiMap[ps[i]];
	}

	if (AddEllipsis) {
		r[L] = '.';
		++L;
		r[L] = '.';
		++L;
		r[L] = '.';
		++L;
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
	ui5r NewSony_Count = Sony_Count;

	fseek(Drives[Drive_No], Sony_Start, SEEK_SET);

	if (IsWrite) {
		fwrite(Buffer, 1, NewSony_Count, Drives[Drive_No]);
	} else {
		fread(Buffer, 1, NewSony_Count, Drives[Drive_No]);
	}

	if (nullpr != Sony_ActCount) {
		*Sony_ActCount = NewSony_Count;
	}

	return mnvm_noErr; /*& figure out what really to return &*/
}

GLOBALOSGLUFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	fseek(Drives[Drive_No], 0, SEEK_END);
	*Sony_Count = ftell(Drives[Drive_No]);
	return mnvm_noErr; /*& figure out what really to return &*/
}

LOCALFUNC tMacErr vSonyEject0(tDrive Drive_No, blnr deleteit)
{
	DiskEjectedNotify(Drive_No);

	fclose(Drives[Drive_No]);
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

	if (! FirstFreeDisk(&Drive_No)) {
		fclose(refnum);
		MacMsg(kStrTooManyImagesTitle,
			kStrTooManyImagesMessage, falseblnr);
		return falseblnr;
	} else {
		/* printf("Sony_Insert0 %d\n", (int)Drive_No); */
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);

		return trueblnr;
	}
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
		if (kROM_Size != File_Size) {
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

LOCALFUNC blnr Sony_InsertIth(int i)
{
	blnr v;

	if ((i > 9) || ! FirstFreeDisk(nullpr)) {
		v = falseblnr;
	} else {
		char s[] = "disk?.dsk";

		s[4] = '0' + i;

		v = Sony_Insert1(s, trueblnr);
	}

	return v;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	int i;

	for (i = 1; Sony_InsertIth(i); ++i) {
		/* stop on first error (including file not found) */
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

	return trueblnr;
}

/* --- video out --- */

static GtkWidget *drawing_area;

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left,
	si4b bottom, si4b right)
{
	guchar graybuf[vMacScreenWidth * vMacScreenHeight];

	{
		int i;
		int j;
		int k;
		ui3b *p1 = GetCurDrawBuff()
			+ (ui5r)vMacScreenWidth / 8 * top;
		ui3b *p2 = (ui3b *)graybuf + (ui5r)vMacScreenWidth * top;
		ui5b t0;

		UnusedParam(left);
		UnusedParam(right);
		for (i = bottom - top; --i >= 0; ) {
			for (j = vMacScreenWidth / 8; --j >= 0; ) {
				t0 = *p1++;
				for (k = 8; --k >= 0; ) {
					*p2++ = ((t0 >> k) & 0x01) - 1;
				}
			}
		}
	}

	gdk_draw_gray_image(drawing_area->window,
		drawing_area->style->fg_gc[GTK_WIDGET_STATE(drawing_area)],
		left /* x */,
		top /* y */,
		right - left /* width */,
		bottom - top /* height */,
		GDK_RGB_DITHER_NONE,
		graybuf + left + (ui5r)vMacScreenWidth * top,
		vMacScreenWidth);
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
	MyDrawChangesAndClear();
}

/* --- mouse --- */

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

static GdkCursor *blank_cursor;
static GtkWidget *window = NULL;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;

		if (window) {
			gdk_window_set_cursor(window->window, NULL);
		}
	}
}

/* cursor state */

LOCALPROC MousePositionNotify(int NewMousePosh, int NewMousePosv)
{
	blnr ShouldHaveCursorHidden = trueblnr;

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


		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/*
			for a game like arkanoid, would like mouse to still
			move even when outside window in one direction
		*/
		MyMousePositionSet(NewMousePosh, NewMousePosv);
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

LOCALPROC CheckMouseState(void)
{
#if 0
	gint x;
	gint y;
	gint x0;
	gint y0;

	gdk_display_get_pointer(gdk_drawable_get_display(window),
		NULL, &x, &y, NULL);
	(void) gdk_window_get_origin(window, &x0, &y0);
	MousePositionNotify(x - x0, y - y0);
#endif
	gint x;
	gint y;

	gtk_widget_get_pointer(drawing_area, &x, &y);
	/* (void) gdk_window_get_pointer(window, &x, &y, NULL); */
	MousePositionNotify(x, y);
}

/* --- keyboard input --- */

#define MaxNumKeycode 256
#define KeyCodeMask (MaxNumKeycode - 1)
	/*
		assume keycodes >= 0 and < MaxNumKeycode, which
		isn't promised by gtk documentation.
	*/


LOCALVAR ui3b KC2MKC[MaxNumKeycode];

LOCALPROC KC2MKCAssignOne(guint keyval, ui3r key)
{
	GdkKeymapKey *keys;
	gint n_keys;
	int i;

	if (gdk_keymap_get_entries_for_keyval(NULL,
		keyval, &keys, &n_keys))
	{
		for (i = 0; i < n_keys; i++) {
			KC2MKC[keys[i].keycode & KeyCodeMask] = key;
		}
		g_free(keys);
	}
#if 0
	fprintf(stderr, "%d %d %d\n", (int)ks, key, (int)code);
#endif
}

LOCALFUNC blnr KC2MKCInit(void)
{
	int i;

	for (i = 0; i < 256; ++i) {
		KC2MKC[i] = MKC_None;
	}

#ifdef GDK_KP_Insert
	KC2MKCAssignOne(GDK_KP_Insert, MKC_KP0);
#endif
#ifdef GDK_KP_End
	KC2MKCAssignOne(GDK_KP_End, MKC_KP1);
#endif
#ifdef GDK_KP_Down
	KC2MKCAssignOne(GDK_KP_Down, MKC_KP2);
#endif
#ifdef GDK_KP_Next
	KC2MKCAssignOne(GDK_KP_Next, MKC_KP3);
#endif
#ifdef GDK_KP_Left
	KC2MKCAssignOne(GDK_KP_Left, MKC_KP4);
#endif
#ifdef GDK_KP_Begin
	KC2MKCAssignOne(GDK_KP_Begin, MKC_KP5);
#endif
#ifdef GDK_KP_Right
	KC2MKCAssignOne(GDK_KP_Right, MKC_KP6);
#endif
#ifdef GDK_KP_Home
	KC2MKCAssignOne(GDK_KP_Home, MKC_KP7);
#endif
#ifdef GDK_KP_Up
	KC2MKCAssignOne(GDK_KP_Up, MKC_KP8);
#endif
#ifdef GDK_KP_Prior
	KC2MKCAssignOne(GDK_KP_Prior, MKC_KP9);
#endif
#ifdef GDK_KP_Delete
	KC2MKCAssignOne(GDK_KP_Delete, MKC_Decimal);
#endif

	KC2MKCAssignOne(GDK_asciitilde, MKC_formac_Grave);
	KC2MKCAssignOne(GDK_underscore, MKC_Minus);
	KC2MKCAssignOne(GDK_plus, MKC_Equal);
	KC2MKCAssignOne(GDK_braceleft, MKC_LeftBracket);
	KC2MKCAssignOne(GDK_braceright, MKC_RightBracket);
	KC2MKCAssignOne(GDK_bar, MKC_formac_BackSlash);
	KC2MKCAssignOne(GDK_colon, MKC_SemiColon);
	KC2MKCAssignOne(GDK_quotedbl, MKC_SingleQuote);
	KC2MKCAssignOne(GDK_less, MKC_Comma);
	KC2MKCAssignOne(GDK_greater, MKC_Period);
	KC2MKCAssignOne(GDK_question, MKC_formac_Slash);

	KC2MKCAssignOne(GDK_a, MKC_A);
	KC2MKCAssignOne(GDK_b, MKC_B);
	KC2MKCAssignOne(GDK_c, MKC_C);
	KC2MKCAssignOne(GDK_d, MKC_D);
	KC2MKCAssignOne(GDK_e, MKC_E);
	KC2MKCAssignOne(GDK_f, MKC_F);
	KC2MKCAssignOne(GDK_g, MKC_G);
	KC2MKCAssignOne(GDK_h, MKC_H);
	KC2MKCAssignOne(GDK_i, MKC_I);
	KC2MKCAssignOne(GDK_j, MKC_J);
	KC2MKCAssignOne(GDK_k, MKC_K);
	KC2MKCAssignOne(GDK_l, MKC_L);
	KC2MKCAssignOne(GDK_m, MKC_M);
	KC2MKCAssignOne(GDK_n, MKC_N);
	KC2MKCAssignOne(GDK_o, MKC_O);
	KC2MKCAssignOne(GDK_p, MKC_P);
	KC2MKCAssignOne(GDK_q, MKC_Q);
	KC2MKCAssignOne(GDK_r, MKC_R);
	KC2MKCAssignOne(GDK_s, MKC_S);
	KC2MKCAssignOne(GDK_t, MKC_T);
	KC2MKCAssignOne(GDK_u, MKC_U);
	KC2MKCAssignOne(GDK_v, MKC_V);
	KC2MKCAssignOne(GDK_w, MKC_W);
	KC2MKCAssignOne(GDK_x, MKC_X);
	KC2MKCAssignOne(GDK_y, MKC_Y);
	KC2MKCAssignOne(GDK_z, MKC_Z);

	/*
		main mappings
	*/

	KC2MKCAssignOne(GDK_F1, MKC_formac_F1);
	KC2MKCAssignOne(GDK_F2, MKC_formac_F2);
	KC2MKCAssignOne(GDK_F3, MKC_formac_F3);
	KC2MKCAssignOne(GDK_F4, MKC_formac_F4);
	KC2MKCAssignOne(GDK_F5, MKC_formac_F5);
	KC2MKCAssignOne(GDK_F6, MKC_F6);
	KC2MKCAssignOne(GDK_F7, MKC_F7);
	KC2MKCAssignOne(GDK_F8, MKC_F8);
	KC2MKCAssignOne(GDK_F9, MKC_F9);
	KC2MKCAssignOne(GDK_F10, MKC_F10);
	KC2MKCAssignOne(GDK_F11, MKC_F11);
	KC2MKCAssignOne(GDK_F12, MKC_F12);

#ifdef GDK_Delete
	KC2MKCAssignOne(GDK_Delete, MKC_formac_ForwardDel);
#endif
#ifdef GDK_Insert
	KC2MKCAssignOne(GDK_Insert, MKC_formac_Help);
#endif
#ifdef GDK_Help
	KC2MKCAssignOne(GDK_Help, MKC_formac_Help);
#endif
#ifdef GDK_Home
	KC2MKCAssignOne(GDK_Home, MKC_formac_Home);
#endif
#ifdef GDK_End
	KC2MKCAssignOne(GDK_End, MKC_formac_End);
#endif

#ifdef GDK_Page_Up
	KC2MKCAssignOne(GDK_Page_Up, MKC_formac_PageUp);
#else
#ifdef GDK_Prior
	KC2MKCAssignOne(GDK_Prior, MKC_formac_PageUp);
#endif
#endif

#ifdef GDK_Page_Down
	KC2MKCAssignOne(GDK_Page_Down, MKC_formac_PageDown);
#else
#ifdef GDK_Next
	KC2MKCAssignOne(GDK_Next, MKC_formac_PageDown);
#endif
#endif

#ifdef GDK_Print
	KC2MKCAssignOne(GDK_Print, MKC_Print);
#endif
#ifdef GDK_Scroll_Lock
	KC2MKCAssignOne(GDK_Scroll_Lock, MKC_ScrollLock);
#endif
#ifdef GDK_Pause
	KC2MKCAssignOne(GDK_Pause, MKC_Pause);
#endif

	KC2MKCAssignOne(GDK_KP_Add, MKC_KPAdd);
	KC2MKCAssignOne(GDK_KP_Subtract, MKC_KPSubtract);
	KC2MKCAssignOne(GDK_KP_Multiply, MKC_KPMultiply);
	KC2MKCAssignOne(GDK_KP_Divide, MKC_KPDevide);
	KC2MKCAssignOne(GDK_KP_Enter, MKC_formac_Enter);
	KC2MKCAssignOne(GDK_KP_Equal, MKC_KPEqual);

	KC2MKCAssignOne(GDK_KP_0, MKC_KP0);
	KC2MKCAssignOne(GDK_KP_1, MKC_KP1);
	KC2MKCAssignOne(GDK_KP_2, MKC_KP2);
	KC2MKCAssignOne(GDK_KP_3, MKC_KP3);
	KC2MKCAssignOne(GDK_KP_4, MKC_KP4);
	KC2MKCAssignOne(GDK_KP_5, MKC_KP5);
	KC2MKCAssignOne(GDK_KP_6, MKC_KP6);
	KC2MKCAssignOne(GDK_KP_7, MKC_KP7);
	KC2MKCAssignOne(GDK_KP_8, MKC_KP8);
	KC2MKCAssignOne(GDK_KP_9, MKC_KP9);
	KC2MKCAssignOne(GDK_KP_Decimal, MKC_Decimal);

	KC2MKCAssignOne(GDK_Left, MKC_Left);
	KC2MKCAssignOne(GDK_Right, MKC_Right);
	KC2MKCAssignOne(GDK_Up, MKC_Up);
	KC2MKCAssignOne(GDK_Down, MKC_Down);

	KC2MKCAssignOne(GDK_grave, MKC_formac_Grave);
	KC2MKCAssignOne(GDK_minus, MKC_Minus);
	KC2MKCAssignOne(GDK_equal, MKC_Equal);
	KC2MKCAssignOne(GDK_bracketleft, MKC_LeftBracket);
	KC2MKCAssignOne(GDK_bracketright, MKC_RightBracket);
	KC2MKCAssignOne(GDK_backslash, MKC_formac_BackSlash);
	KC2MKCAssignOne(GDK_semicolon, MKC_SemiColon);
	KC2MKCAssignOne(GDK_apostrophe, MKC_SingleQuote);
	KC2MKCAssignOne(GDK_comma, MKC_Comma);
	KC2MKCAssignOne(GDK_period, MKC_Period);
	KC2MKCAssignOne(GDK_slash, MKC_formac_Slash);

	KC2MKCAssignOne(GDK_Escape, MKC_formac_Escape);

	KC2MKCAssignOne(GDK_Tab, MKC_Tab);
	KC2MKCAssignOne(GDK_Return, MKC_Return);
	KC2MKCAssignOne(GDK_space, MKC_Space);
	KC2MKCAssignOne(GDK_BackSpace, MKC_BackSpace);

	KC2MKCAssignOne(GDK_Caps_Lock, MKC_formac_CapsLock);
	KC2MKCAssignOne(GDK_Num_Lock, MKC_Clear);

	KC2MKCAssignOne(GDK_Meta_L, MKC_formac_Command);

	KC2MKCAssignOne(GDK_Meta_R, MKC_formac_RCommand);

	KC2MKCAssignOne(GDK_Mode_switch, MKC_formac_Option);
	KC2MKCAssignOne(GDK_Menu, MKC_formac_Option);
	KC2MKCAssignOne(GDK_Super_L, MKC_formac_Option);
	KC2MKCAssignOne(GDK_Super_R, MKC_formac_ROption);
	KC2MKCAssignOne(GDK_Hyper_L, MKC_formac_Option);
	KC2MKCAssignOne(GDK_Hyper_R, MKC_formac_ROption);

	KC2MKCAssignOne(GDK_F13, MKC_formac_Option);
		/*
			seen being used in Mandrake Linux 9.2
			for windows key
		*/

	KC2MKCAssignOne(GDK_Shift_L, MKC_formac_Shift);
	KC2MKCAssignOne(GDK_Shift_R, MKC_formac_RShift);

	KC2MKCAssignOne(GDK_Alt_L, MKC_formac_Command);

	KC2MKCAssignOne(GDK_Alt_R, MKC_formac_RCommand);

	KC2MKCAssignOne(GDK_Control_L, MKC_formac_Control);

	KC2MKCAssignOne(GDK_Control_R, MKC_formac_RControl);

	KC2MKCAssignOne(GDK_1, MKC_1);
	KC2MKCAssignOne(GDK_2, MKC_2);
	KC2MKCAssignOne(GDK_3, MKC_3);
	KC2MKCAssignOne(GDK_4, MKC_4);
	KC2MKCAssignOne(GDK_5, MKC_5);
	KC2MKCAssignOne(GDK_6, MKC_6);
	KC2MKCAssignOne(GDK_7, MKC_7);
	KC2MKCAssignOne(GDK_8, MKC_8);
	KC2MKCAssignOne(GDK_9, MKC_9);
	KC2MKCAssignOne(GDK_0, MKC_0);

	KC2MKCAssignOne(GDK_A, MKC_A);
	KC2MKCAssignOne(GDK_B, MKC_B);
	KC2MKCAssignOne(GDK_C, MKC_C);
	KC2MKCAssignOne(GDK_D, MKC_D);
	KC2MKCAssignOne(GDK_E, MKC_E);
	KC2MKCAssignOne(GDK_F, MKC_F);
	KC2MKCAssignOne(GDK_G, MKC_G);
	KC2MKCAssignOne(GDK_H, MKC_H);
	KC2MKCAssignOne(GDK_I, MKC_I);
	KC2MKCAssignOne(GDK_J, MKC_J);
	KC2MKCAssignOne(GDK_K, MKC_K);
	KC2MKCAssignOne(GDK_L, MKC_L);
	KC2MKCAssignOne(GDK_M, MKC_M);
	KC2MKCAssignOne(GDK_N, MKC_N);
	KC2MKCAssignOne(GDK_O, MKC_O);
	KC2MKCAssignOne(GDK_P, MKC_P);
	KC2MKCAssignOne(GDK_Q, MKC_Q);
	KC2MKCAssignOne(GDK_R, MKC_R);
	KC2MKCAssignOne(GDK_S, MKC_S);
	KC2MKCAssignOne(GDK_T, MKC_T);
	KC2MKCAssignOne(GDK_U, MKC_U);
	KC2MKCAssignOne(GDK_V, MKC_V);
	KC2MKCAssignOne(GDK_W, MKC_W);
	KC2MKCAssignOne(GDK_X, MKC_X);
	KC2MKCAssignOne(GDK_Y, MKC_Y);
	KC2MKCAssignOne(GDK_Z, MKC_Z);

	InitKeyCodes();

	return trueblnr;
}

LOCALPROC CheckTheCapsLock(void)
{
	GdkModifierType mask;

	(void) gdk_window_get_pointer(window->window, NULL, NULL, &mask);

	Keyboard_UpdateKeyMap2(MKC_formac_CapsLock,
		(mask & GDK_LOCK_MASK) != 0);
}

LOCALPROC DoKeyCode(guint keycode, blnr down)
{
	if (GDK_Caps_Lock == keycode) {
		CheckTheCapsLock();
	} else {
		ui3r key = KC2MKC[keycode & KeyCodeMask];

		if (MKC_None != key) {
			Keyboard_UpdateKeyMap2(key, down);
		}
	}
}

/* --- time, date, location --- */

LOCALVAR ui5b TrueEmulatedTime = 0;

#include "DATE2SEC.h"

#define TicksPerSecond 1000000

LOCALVAR blnr HaveTimeDelta = falseblnr;
LOCALVAR ui5b TimeDelta;

LOCALVAR ui5b NewMacDateInSeconds;

LOCALVAR ui5b LastTimeSec;
LOCALVAR ui5b LastTimeUsec;

LOCALPROC GetCurrentTicks(void)
{
	GTimeVal t;

	g_get_current_time(&t);
	if (! HaveTimeDelta) {
		time_t Current_Time;
		struct tm *s;

#if 0
		GDate *date;
		date = g_date_new();
		g_date_set_time_val(date, &t);
		g_date_free(date);
#endif
		(void) time(&Current_Time);
		s = localtime(&Current_Time);
		TimeDelta = Date2MacSeconds(s->tm_sec, s->tm_min, s->tm_hour,
			s->tm_mday, 1 + s->tm_mon, 1900 + s->tm_year) - t.tv_sec;
#if 0 && AutoTimeZone /* how portable is this ? */
		CurMacDelta = ((ui5b)(s->tm_gmtoff) & 0x00FFFFFF)
			| ((s->tm_isdst ? 0x80 : 0) << 24);
#endif
		HaveTimeDelta = trueblnr;
	}

	NewMacDateInSeconds = t.tv_sec + TimeDelta;
	LastTimeSec = (ui5b)t.tv_sec;
	LastTimeUsec = (ui5b)t.tv_usec;
}

#define MyInvTimeStep 16626 /* TicksPerSecond / 60.14742 */

LOCALVAR ui5b NextTimeSec;
LOCALVAR ui5b NextTimeUsec;

LOCALPROC IncrNextTime(void)
{
	NextTimeUsec += MyInvTimeStep;
	if (NextTimeUsec >= TicksPerSecond) {
		NextTimeUsec -= TicksPerSecond;
		NextTimeSec += 1;
	}
}

LOCALPROC InitNextTime(void)
{
	NextTimeSec = LastTimeSec;
	NextTimeUsec = LastTimeUsec;
	IncrNextTime();
}

LOCALPROC StartUpTimeAdjust(void)
{
	GetCurrentTicks();
	InitNextTime();
}

LOCALFUNC si5b GetTimeDiff(void)
{
	return ((si5b)(LastTimeSec - NextTimeSec)) * TicksPerSecond
		+ ((si5b)(LastTimeUsec - NextTimeUsec));
}

LOCALPROC UpdateTrueEmulatedTime(void)
{
	si5b TimeDiff;

	GetCurrentTicks();

	TimeDiff = GetTimeDiff();
	if (TimeDiff >= 0) {
		if (TimeDiff > 4 * MyInvTimeStep) {
			/* emulation interrupted, forget it */
			++TrueEmulatedTime;
			InitNextTime();
		} else {
			do {
				++TrueEmulatedTime;
				IncrNextTime();
				TimeDiff -= TicksPerSecond;
			} while (TimeDiff >= 0);
		}
	} else if (TimeDiff < - 2 * MyInvTimeStep) {
		/* clock goofed if ever get here, reset */
		InitNextTime();
	}
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

LOCALFUNC blnr InitLocationDat(void)
{
	GetCurrentTicks();
	CurMacDateInSeconds = NewMacDateInSeconds;

	return trueblnr;
}

/* --- basic dialogs --- */

LOCALPROC CheckSavedMacMsg(void)
{
	if (nullpr != SavedBriefMsg) {
		char briefMsg0[ClStrMaxLength + 1];
		char longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg, falseblnr);
		NativeStrFromCStr(longMsg0, SavedLongMsg, falseblnr);

		fprintf(stderr, "%s\n", briefMsg0);
		fprintf(stderr, "%s\n", longMsg0);

		SavedBriefMsg = nullpr;
	}
}

LOCALVAR blnr CaughtMouse = falseblnr;

/* --- main window creation and disposal --- */

LOCALVAR int my_argc;
LOCALVAR char **my_argv;

/* Create a new backing pixmap of the appropriate size */
static gboolean configure_event(GtkWidget *widget,
	GdkEventConfigure *event)
{
	return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gboolean expose_event(GtkWidget *widget,
	GdkEventExpose *event,
	gpointer user_data)
{
	int x0 = event->area.x;
	int y0 = event->area.y;
	int x1 = x0 + event->area.width;
	int y1 = y0 + event->area.height;

#if 0 && MyDbgEvents
	fprintf(stderr, "- event - Expose\n");
#endif

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

	return FALSE;
}

static gboolean button_press_event(GtkWidget *widget,
	GdkEventButton *event,
	gpointer user_data)
{
	MousePositionNotify(event->x, event->y);
	MyMouseButtonSet(trueblnr);

	return TRUE;
}

static gboolean button_release_event(GtkWidget *widget,
	GdkEventButton *event,
	gpointer user_data)
{
	MousePositionNotify(event->x, event->y);
	MyMouseButtonSet(falseblnr);

	return TRUE;
}

static gboolean motion_notify_event(GtkWidget *widget,
	GdkEventMotion *event,
	gpointer user_data)
{
	int x;
	int y;
	GdkModifierType state;

	if (event->is_hint) {
		gdk_window_get_pointer(event->window, &x, &y, &state);
	} else {
		x = event->x;
		y = event->y;
		state = event->state;
	}

	MousePositionNotify(x, y);
	MyMouseButtonSet((state & GDK_BUTTON1_MASK) != 0);

	return TRUE;
}

static gboolean enter_notify_event(GtkWidget *widget,
	GdkEventCrossing *event,
	gpointer user_data)
{
	CaughtMouse = trueblnr;

	MousePositionNotify(event->x, event->y);
	MyMouseButtonSet((event->state & GDK_BUTTON1_MASK) != 0);

	return TRUE;
}

static gboolean leave_notify_event(GtkWidget *widget,
	GdkEventCrossing *event,
	gpointer user_data)
{
	MousePositionNotify(event->x, event->y);
	MyMouseButtonSet((event->state & GDK_BUTTON1_MASK) != 0);

	CaughtMouse = falseblnr;

	return TRUE;
}

static gboolean delete_event(GtkWidget *widget,
	GdkEventMotion *event)
{
	RequestMacOff = trueblnr;

	return TRUE;
}

LOCALPROC ReconnectKeyCodes3(void)
{
	CheckTheCapsLock();

#if 0
	if (WantCmdOptOnReconnect) {
		WantCmdOptOnReconnect = falseblnr;

		GetTheDownKeys();
	}
#endif
}

LOCALPROC DisconnectKeyCodes3(void)
{
	DisconnectKeyCodes2();
	MyMouseButtonSet(falseblnr);
}

LOCALVAR blnr ADialogIsUp = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	DisconnectKeyCodes3();
	ADialogIsUp = trueblnr;
	ForceShowCursor();
}

LOCALPROC MyEndDialog(void)
{
	ADialogIsUp = falseblnr;
	ReconnectKeyCodes3();
}

/* --- SavedTasks --- */

LOCALPROC LeaveBackground(void)
{
	ReconnectKeyCodes3();
#if 0
	DisableKeyRepeat();
#endif
}

LOCALPROC EnterBackground(void)
{
#if 0
	RestoreKeyRepeat();
#endif
	DisconnectKeyCodes3();

	ForceShowCursor();
}

LOCALPROC LeaveSpeedStopped(void)
{
	StartUpTimeAdjust();
}

LOCALPROC EnterSpeedStopped(void)
{
}

static void InsertADisk0(void)
{
	char ts[ClStrMaxLength + 1];
	GtkWidget *dialog;

	NativeStrFromCStr(ts, kStrMenuItemOpen, falseblnr);

	MyBeginDialog();
	dialog = gtk_file_chooser_dialog_new(ts,
		GTK_WINDOW(window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);
	MyEndDialog();

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;

		filename =
			gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		(void) Sony_Insert1a(filename, falseblnr);
		g_free(filename);
	}

	gtk_widget_destroy(dialog);
}

LOCALPROC CheckForSavedTasks(void)
{
	if (MyEvtQNeedRecover) {
		MyEvtQNeedRecover = falseblnr;

		/* attempt cleanup, MyEvtQNeedRecover may get set again */
		MyEvtQTryRecoverFromFull();
	}

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

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		ScreenChangedAll();
	}

	if (HaveCursorHidden != (WantCursorHidden && CaughtMouse
		&& ! (gTrueBackgroundFlag || ADialogIsUp || CurSpeedStopped)))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		if (HaveCursorHidden) {
			gdk_window_set_cursor(window->window, blank_cursor);
		} else {
			gdk_window_set_cursor(window->window, NULL);
		}
	}

	if (gTrueBackgroundFlag || ADialogIsUp) {
	} else {
		if (RequestInsertDisk) {
			RequestInsertDisk = falseblnr;
			InsertADisk0();
		}
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
	int i;

	for (i = 1; i < my_argc; ++i) {
		if ('-' == my_argv[i][0]) {
#if 0
			if ((0 == strcmp(my_argv[i], "--display")) ||
				(0 == strcmp(my_argv[i], "-display")))
			{
				++i;
				if (i < my_argc) {
					display_name = my_argv[i];
				}
			} else
#endif
			if ((0 == strcmp(my_argv[i], "--rom")) ||
				(0 == strcmp(my_argv[i], "-r")))
			{
				++i;
				if (i < my_argc) {
					rom_path = my_argv[i];
				}
			} else
#if 0
			if (0 == strcmp(my_argv[i], "-l")) {
				SpeedValue = 0;
			} else
#endif
			{
				MacMsg(kStrBadArgTitle, kStrBadArgMessage, falseblnr);
			}
		} else {
			(void) Sony_Insert1(my_argv[i], falseblnr);
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
	(void) gtk_main_iteration_do(TRUE);
}

LOCALPROC CheckForSystemEvents(void)
{
	int i = 10;

	while (gtk_events_pending() && (--i >= 0)) {
		(void) gtk_main_iteration_do(FALSE);
	}
#if 0
	XFlush(x_display);
#endif
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
		si5b TimeDiff = GetTimeDiff();
		if (TimeDiff < 0) {
			g_usleep(- TimeDiff);
		}
		goto label_retry;
	}

	if (CheckDateTime()) {
#if EnableDemoMsg
		DemoModeSecondNotify();
#endif
	}

	if ((! gBackgroundFlag || ADialogIsUp)
		&& (! CaughtMouse)
		)
	{
		CheckMouseState();
	}

	OnTrueTime = TrueEmulatedTime;
}

#include "PROGMAIN.h"

static gboolean
MainEventLoop0(gpointer data)
{
	(void) data;
	fprintf(stderr, "hello from MainEventLoop0\n");
	ProgramMain();
	if (ForceMacOff) {
		goto Label_01;
	}
#if 0
	while (! gtk_main_iteration_do(FALSE)) {
		if (! gtk_events_pending()) {
			fprintf(stderr, "sleep\n");
			g_usleep(1000000);
		}
	}
#endif
Label_01:
	fprintf(stderr, "leaving MainEventLoop0\n");

	gtk_main_quit();

	return FALSE;
}

static gboolean
focus_in_event(GtkWidget *widget, GdkEvent *event,
	gpointer data)
{
	gTrueBackgroundFlag = falseblnr;

	CheckMouseState();

	return FALSE;
}

static gboolean
focus_out_event(GtkWidget *widget, GdkEvent *event,
	gpointer data)
{
	gTrueBackgroundFlag = trueblnr;
	return FALSE;
}

static gboolean key_press_event(GtkWidget *widget,
	GdkEventKey *event,
	gpointer user_data)
{
#if 0
	fprintf(stderr, "hello from key_press_event\n");
	fprintf(stderr, "keyval %d\n", event->keyval);
	fprintf(stderr, "hardware_keycode %d\n", event->hardware_keycode);
#endif
#if 0
	{
		GdkKeymapKey *keys;
		gint n_keys;
		int i;

		if (gdk_keymap_get_entries_for_keyval(NULL,
			event->keyval, &keys, &n_keys))
		{
			for (i = 0; i < n_keys; i++) {
				fprintf(stderr, "keycode %d\n", keys[i].keycode);
			}
			g_free(keys);
		}
	}
#endif
	DoKeyCode(event->hardware_keycode, trueblnr);

	return TRUE;
}

static gboolean key_release_event(GtkWidget *widget,
	GdkEventKey *event,
	gpointer user_data)
{
#if 0
	fprintf(stderr, "hello from key_release_event\n");
	fprintf(stderr, "keyval %d\n", event->keyval);
	fprintf(stderr, "hardware_keycode %d\n", event->hardware_keycode);
#endif
	DoKeyCode(event->hardware_keycode, falseblnr);

	return TRUE;
}

static void drag_data_received(GtkWidget *widget,
	GdkDragContext *drag_context,
	gint x,
	gint y,
	GtkSelectionData *data,
	guint info,
	guint time,
	gpointer user_data)
{
	char **uris;
	char *file;
	int i;
	gboolean handled = FALSE;

	uris = g_strsplit((char *)data->data, "\r\n", -1);
	if (uris != NULL) {
		for (i = 0; uris[i] != NULL; i++) {
			file = g_filename_from_uri(uris[i], NULL, NULL);
			/* file = gnome_vfs_get_local_path_from_uri(uris[i]); */
			if (file != NULL) {
				(void) Sony_Insert1a(file, falseblnr);
				handled = TRUE;
				g_free(file);
			}
		}
		g_strfreev(uris);
	}
	gtk_drag_finish(drag_context, handled, FALSE, time);
	if (handled) {
		gtk_window_present_with_time(GTK_WINDOW(window), time);
	}
}

static void do_more_commands_item(GtkAction *action, gpointer user_data)
{
	DoMoreCommandsMsg();
}

static void do_about_item(GtkAction *action, gpointer user_data)
{
	DoAboutMsg();
}

static void do_quit_item(GtkAction *action, gpointer user_data)
{
	RequestMacOff = trueblnr;
}

static void do_open_item(GtkAction *action, gpointer user_data)
{
	RequestInsertDisk = trueblnr;
}

LOCALPROC MyAppendConvertMenuItem(GtkWidget *the_menu,
	GCallback c_handler, gpointer gobject, char *s, blnr AddEllipsis)
{
	char ts[ClStrMaxLength + 1];
	GtkWidget *the_item;

	NativeStrFromCStr(ts, s, AddEllipsis);
	the_item = gtk_menu_item_new_with_label(ts);
	g_signal_connect(G_OBJECT(the_item), "activate",
		c_handler, gobject);
	gtk_menu_shell_append(GTK_MENU_SHELL(the_menu), the_item);
}

LOCALPROC MyAppendSubmenuConvertName(GtkWidget *menubar,
	GtkWidget *the_menu, char *s)
{
	char ts[ClStrMaxLength + 1];
	GtkWidget *the_item;

	NativeStrFromCStr(ts, s, falseblnr);
	the_item = gtk_menu_item_new_with_label(ts);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(the_item), the_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), the_item);
}

static GdkPixmap *blank_pixmap;
static GdkColor blank_color = {
	0, 0, 0, 0
};

static gchar blank_cursor_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GtkTargetEntry dnd_target =
{
	"text/uri-list", 0, 0
};

LOCALPROC ZapOSGLUVars(void)
{
	InitDrives();
#if 0
	{
		int i;

		for (i = 0; i < kNumMagStates; ++i) {
			HavePositionWins[i] = falseblnr;
		}
	}
#endif
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

LOCALFUNC blnr InitOSGLU(void)
{
	if (AllocMyMemory())
#if dbglog_HAVE
	if (dbglog_open())
#endif
	if (ScanCommandLine())
	if (LoadMacRom())
	if (LoadInitialImages())
	if (InitLocationDat())
	/* if (ReCreateMainWindow()) */
	if (KC2MKCInit())
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

	UnInitDrives();

	ForceShowCursor();

#if dbglog_HAVE
	dbglog_close();
#endif

	UnallocMyMemory();

	CheckSavedMacMsg();
}

int main(int argc, char *argv[])
{
	GtkWidget *vbox;

#if 0
	GtkWidget *button;
#endif

	GtkWidget *menubar;
	GtkWidget *the_menu;
	GtkWidget *the_item;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(window, "Test Input");
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);

	g_signal_connect(G_OBJECT(window), "delete-event",
		G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "focus-out-event",
		(GCallback)focus_out_event,
		NULL);
	g_signal_connect(G_OBJECT(window), "focus-in-event",
		(GCallback)focus_in_event, NULL);

	g_signal_connect(G_OBJECT(window), "key-press-event",
		G_CALLBACK(key_press_event), NULL);
	g_signal_connect(G_OBJECT(window), "key-release-event",
		G_CALLBACK(key_release_event), NULL);

	menubar = gtk_menu_bar_new();

	the_menu = gtk_menu_new();

	MyAppendConvertMenuItem(the_menu,
		G_CALLBACK(do_open_item), NULL, kStrMenuItemOpen, trueblnr);

	the_item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(the_menu), the_item);

	MyAppendConvertMenuItem(the_menu,
		G_CALLBACK(do_quit_item), NULL, kStrMenuItemQuit, falseblnr);

	MyAppendSubmenuConvertName(menubar, the_menu, kStrMenuFile);

	the_menu = gtk_menu_new();

	MyAppendConvertMenuItem(the_menu, G_CALLBACK(do_more_commands_item),
		NULL, kStrMenuItemMore, trueblnr);

	MyAppendSubmenuConvertName(menubar, the_menu, kStrMenuSpecial);

	the_menu = gtk_menu_new();

	MyAppendConvertMenuItem(the_menu,
		G_CALLBACK(do_about_item), NULL, kStrMenuItemAbout, trueblnr);

	MyAppendSubmenuConvertName(menubar, the_menu, kStrMenuHelp);

	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

	gtk_widget_show_all(menubar);


	/* Create the drawing area */

	drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(GTK_WIDGET(drawing_area),
		vMacScreenWidth, vMacScreenHeight);
	gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);

	gtk_widget_show(drawing_area);

	/* Signals used to handle backing pixmap */

	g_signal_connect(G_OBJECT(drawing_area), "expose-event",
		G_CALLBACK(expose_event), NULL);
	g_signal_connect(G_OBJECT(drawing_area), "configure-event",
		G_CALLBACK(configure_event), NULL);

	/* Event signals */

	g_signal_connect(G_OBJECT(drawing_area), "motion-notify-event",
		G_CALLBACK(motion_notify_event), NULL);
	g_signal_connect(G_OBJECT(drawing_area), "enter-notify-event",
		G_CALLBACK(enter_notify_event), NULL);
	g_signal_connect(G_OBJECT(drawing_area), "leave-notify-event",
		G_CALLBACK(leave_notify_event), NULL);

	g_signal_connect(G_OBJECT(drawing_area), "button-press-event",
		G_CALLBACK(button_press_event), NULL);
	g_signal_connect(G_OBJECT(drawing_area), "button-release-event",
		G_CALLBACK(button_release_event), NULL);

	gtk_widget_add_events(window,
		GDK_KEY_PRESS_MASK
		| GDK_KEY_RELEASE_MASK
		);

	gtk_widget_set_events(drawing_area, GDK_EXPOSURE_MASK
		| GDK_LEAVE_NOTIFY_MASK
		| GDK_BUTTON_PRESS_MASK
		| GDK_BUTTON_RELEASE_MASK
		| GDK_POINTER_MOTION_MASK
		| GDK_ENTER_NOTIFY_MASK
		| GDK_LEAVE_NOTIFY_MASK
		| GDK_FOCUS_CHANGE_MASK
		| GDK_KEY_PRESS_MASK
		| GDK_KEY_RELEASE_MASK
		/* | GDK_POINTER_MOTION_HINT_MASK */
		);

#if 0
	/* .. And a quit button */
	button = gtk_button_new_with_label("Quit");
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

	g_signal_connect_swapped(G_OBJECT(button), "clicked",
		G_CALLBACK(delete_event), NULL
		/*
			G_CALLBACK(gtk_widget_destroy),
			G_OBJECT(window)
		*/
		);
	gtk_widget_show(button);
#endif

	gtk_drag_dest_set(drawing_area, GTK_DEST_DEFAULT_ALL,
		&dnd_target, 1,
		GDK_ACTION_COPY);
	g_signal_connect(GTK_OBJECT(drawing_area), "drag-data-received",
		G_CALLBACK(drag_data_received), NULL);

	gtk_widget_show(window);

	blank_pixmap = gdk_bitmap_create_from_data(NULL,
		blank_cursor_bits, 16, 16);
	blank_cursor = gdk_cursor_new_from_pixmap(blank_pixmap,
		blank_pixmap, &blank_color, &blank_color, 8, 8);
	gdk_pixmap_unref(blank_pixmap);

	gdk_window_set_cursor(window->window, blank_cursor);

	g_idle_add(MainEventLoop0, NULL);

	my_argc = argc;
	my_argv = argv;

	ZapOSGLUVars();
	if (InitOSGLU()) {
		gtk_main ();
	}
	UnInitOSGLU();

	return 0;
}
