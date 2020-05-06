/*
	OSGLUWIN.c

	Copyright (C) 2009 Philip Cummins, Weston Pawlowski,
	Bradford L. Barrett, Paul C. Pratt, Fabio Concas

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
	Operating System GLUe for microsoft WINdows

	All operating system dependent code for the
	Microsoft Windows platform should go here.

	This code is descended from Weston Pawlowski's Windows
	port of vMac, by Philip Cummins.
	Adapted by Fabio Concas to run on Pocket PC 2003 devices.

	The main entry point '_tWinMain' is at the end of this file.
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"


/* --- adapting to API/ABI version differences --- */

#ifdef UNICODE
#define MyUseUni 1
#else
#define MyUseUni 0
#endif

#ifdef _WIN32_WCE
#define UseWinCE 1
#else
#define UseWinCE 0
#endif


#define My_CSIDL_APPDATA 0x001a

typedef BOOL (WINAPI *SHGetSpecialFolderPathProcPtr) (
	HWND hwndOwner,
	LPTSTR lpszPath,
	int nFolder,
	BOOL fCreate
);
LOCALVAR SHGetSpecialFolderPathProcPtr MySHGetSpecialFolderPath = NULL;
LOCALVAR blnr DidSHGetSpecialFolderPath = falseblnr;

LOCALFUNC blnr HaveMySHGetSpecialFolderPath(void)
{
	if (! DidSHGetSpecialFolderPath) {
		HMODULE hLibModule = LoadLibrary(TEXT("shell32.dll"));
		if (NULL != hLibModule) {
			MySHGetSpecialFolderPath =
				(SHGetSpecialFolderPathProcPtr)
				GetProcAddress(hLibModule,
#if MyUseUni
					TEXT("SHGetSpecialFolderPathW")
#else
					TEXT("SHGetSpecialFolderPathA")
#endif
				);
			/* FreeLibrary(hLibModule); */
		}
		DidSHGetSpecialFolderPath = trueblnr;
	}
	return (MySHGetSpecialFolderPath != NULL);
}


/* --- end of adapting to API/ABI version differences --- */

#include "STRCONST.h"

#if MyUseUni
#define NeedCell2UnicodeMap 1
#else
#define NeedCell2WinAsciiMap 1
#endif

#include "INTLCHAR.h"


LOCALPROC NativeStrFromCStr(LPTSTR r, char *s, blnr AddEllipsis)
{
	ui3b ps[ClStrMaxLength];
	int i;
	int L;

	ClStrFromSubstCStr(&L, ps, s);

	for (i = 0; i < L; ++i) {
		r[i] = (TCHAR)
#if MyUseUni
			Cell2UnicodeMap[ps[i]];
#else
			Cell2WinAsciiMap[ps[i]];
#endif
	}

	if (AddEllipsis) {
#if MyUseUni
		r[L] = 0x2026;
		++L;
#else
		r[L] = '.';
		++L;
		r[L] = '.';
		++L;
		r[L] = '.';
		++L;
#endif
	}

	r[L] = 0;
}

LOCALFUNC LPTSTR FindLastTerm(LPTSTR s, TCHAR delim)
{
	TCHAR c;
	LPTSTR p0 = s;
	LPTSTR p = (LPTSTR)nullpr;

	while ((c = *p0++) != (TCHAR)('\0')) {
		if (c == delim) {
			p = p0;
		}
	}

	return p;
}

LOCALVAR HINSTANCE AppInstance;

LOCALFUNC blnr GetAppDir(LPTSTR pathName)
/* be sure at least _MAX_PATH long! */
{
	if (GetModuleFileName(AppInstance, pathName, _MAX_PATH) == 0) {
		/* MacMsg("error", "GetModuleFileName failed", falseblnr); */
	} else {
		LPTSTR p = FindLastTerm(pathName,
			(TCHAR)('\\'));
		if (p == nullpr) {
			/* MacMsg("error", "strrchr failed", falseblnr); */
		} else {
			*--p = (TCHAR)('\0');
			return trueblnr;
		}
	}
	return falseblnr;
}

/* --- sending debugging info to file --- */

#if dbglog_HAVE

LOCALVAR HANDLE dbglog_File = INVALID_HANDLE_VALUE;

LOCALFUNC blnr dbglog_open0(void)
{
	TCHAR pathName[_MAX_PATH];
	TCHAR Child0[] = TEXT("\\dbglog.txt");
	size_t newlen;

	if (GetAppDir(pathName)) {
		newlen = _tcslen(pathName) + _tcslen(Child0);
		if (newlen + 1 < _MAX_PATH) {
			_tcscat(pathName, Child0);

			dbglog_File = CreateFile(
				pathName, /* pointer to name of the file */
				GENERIC_READ + GENERIC_WRITE,
					/* access (read-write) mode */
				0, /* share mode */
				NULL, /* pointer to security descriptor */
				OPEN_ALWAYS, /* how to create */
				FILE_ATTRIBUTE_NORMAL, /* file attributes */
				NULL /* handle to file with attributes to copy */
			);
			if (INVALID_HANDLE_VALUE == dbglog_File) {
				/* report error (how?) */
			} else if (SetFilePointer(
				dbglog_File, /* handle of file */
				0, /* number of bytes to move file pointer */
				nullpr,
					/* address of high-order word of distance to move */
				FILE_BEGIN /* how to move */
				) != 0)
			{
				/* report error (how?) */
			}
		}
	}

	return (INVALID_HANDLE_VALUE != dbglog_File);
}

LOCALPROC dbglog_write0(char *s, uimr L)
{
	DWORD BytesWritten;

	if (INVALID_HANDLE_VALUE != dbglog_File) {
		if (! WriteFile(dbglog_File, /* handle of file to read */
			(LPVOID)s, /* address of buffer that receives data */
			(DWORD)L, /* number of bytes to read */
			&BytesWritten, /* address of number of bytes read */
			nullpr) /* address of structure for data */
			|| ((ui5b)BytesWritten != L))
		{
			/* report error (how?) */
		}
	}
}

LOCALPROC dbglog_close0(void)
{
	if (INVALID_HANDLE_VALUE != dbglog_File) {
		if (! SetEndOfFile(dbglog_File)) {
			/* report error (how?) */
		}
		(void) CloseHandle(dbglog_File);
		dbglog_File = INVALID_HANDLE_VALUE;
	}
}

#endif

#include "COMOSGLU.h"

#ifndef InstallFileIcons
#define InstallFileIcons 0
#endif

/* Resource Ids */

#define IDI_VMAC      256
#if InstallFileIcons
#define IDI_ROM       257
#define IDI_DISK      258
#endif

/* --- some simple utilities --- */

#define TestBit(i, p) (((unsigned long)(i) & PowOf2(p)) != 0)

GLOBALOSGLUPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
/*
	must work even if blocks overlap in memory
*/
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

/* --- Parameter buffers --- */

#if IncludePbufs
LOCALVAR HGLOBAL PbufDat[NumPbufs];
#endif

#if IncludePbufs
LOCALFUNC tMacErr PbufNewFromHandle(HGLOBAL h, ui5b count, tPbuf *r)
{
	tPbuf i;
	tMacErr err;

	if (! FirstFreePbuf(&i)) {
		(void) GlobalFree(h);
		err = mnvm_miscErr;
	} else {
		*r = i;
		PbufDat[i] = h;
		PbufNewNotify(i, count);
		err = mnvm_noErr;
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALOSGLUFUNC tMacErr PbufNew(ui5b count, tPbuf *r)
{
	HGLOBAL h;
	tMacErr err = mnvm_miscErr;

	h = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, count);
	if (h != NULL) {
		/* need to clear h */
		err = PbufNewFromHandle(h, count, r);
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALOSGLUPROC PbufDispose(tPbuf i)
{
	(void) GlobalFree(PbufDat[i]);
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
#define PbufHaveLock 1
#endif

#if IncludePbufs
LOCALFUNC ui3p PbufLock(tPbuf i)
{
	HGLOBAL h = PbufDat[i];
	return (ui3p)GlobalLock(h);
}
#endif

#if IncludePbufs
LOCALPROC PbufUnlock(tPbuf i)
{
	(void) GlobalUnlock(PbufDat[i]);
}
#endif

#if IncludePbufs
GLOBALOSGLUPROC PbufTransfer(ui3p Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	HGLOBAL h = PbufDat[i];
	ui3p p0 = GlobalLock(h);
	if (p0 != NULL) {
		void *p = p0 + offset;
		if (IsWrite) {
			(void) memcpy(p, Buffer, count);
		} else {
			(void) memcpy(Buffer, p, count);
		}
	}
	(void) GlobalUnlock(h);
}
#endif

/* --- control mode and internationalization --- */

#include "CONTROLM.h"

/* --- main window info --- */

LOCALVAR HWND MainWnd = NULL;

LOCALVAR int WndX;
LOCALVAR int WndY;

#if UseWinCE
LOCALVAR short oldOrientation;
LOCALVAR unsigned long oldDisplayOrientation;
#endif

#if VarFullScreen
LOCALVAR blnr UseFullScreen = (WantInitFullScreen != 0);
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = (WantInitMagnify != 0);
#endif

#if MayFullScreen
LOCALVAR short hOffset;
LOCALVAR short vOffset;
#endif

/* cursor hiding */

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		(void) ShowCursor(TRUE);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

/* cursor moving */

LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	POINT NewMousePos;
	ui5b difftime;
	blnr IsOk;
	DWORD StartTime = GetTickCount();
	LONG x = h;
	LONG y = v;

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		x -= ViewHStart;
		y -= ViewVStart;
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		x *= MyWindowScale;
		y *= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		x += hOffset;
		y += vOffset;
	}
#endif

	x += WndX;
	y += WndY;

	do {
		(void) SetCursorPos(x, y);
		if (! GetCursorPos(&NewMousePos)) {
			IsOk = falseblnr;
		} else {
			IsOk = (x == NewMousePos.x) && (y == NewMousePos.y);
		}
		difftime = (ui5b)(GetTickCount() - StartTime);
	} while ((! IsOk) && (difftime < 100));
	return IsOk;
}

#if EnableFSMouseMotion
LOCALPROC StartSaveMouseMotion(void)
{
	if (! HaveMouseMotion) {
		if (MyMoveMouse(ViewHStart + (ViewHSize / 2),
				ViewVStart + (ViewVSize / 2)))
		{
			SavedMouseH = ViewHStart + (ViewHSize / 2);
			SavedMouseV = ViewVStart + (ViewVSize / 2);
			HaveMouseMotion = trueblnr;
		}
	}
}
#endif

#if EnableFSMouseMotion
LOCALPROC StopSaveMouseMotion(void)
{
	if (HaveMouseMotion) {
		(void) MyMoveMouse(CurMouseH, CurMouseV);
		HaveMouseMotion = falseblnr;
	}
}
#endif

LOCALVAR blnr MyMouseCaptured = falseblnr;

LOCALPROC MyMouseCaptureSet(blnr v)
{
	if (v != MyMouseCaptured) {
		if (v) {
			(void) SetCapture(MainWnd);
		} else {
			(void) ReleaseCapture();
		}
		MyMouseCaptured = v;
	}
}

LOCALPROC SetCurMouseButton(blnr v)
{
	MyMouseButtonSet(v);
	MyMouseCaptureSet(v);
}

/* keyboard */

/* these constants weren't in the header files I have */
#define myVK_Subtract 0xBD
#define myVK_Equal 0xBB
#define myVK_BackSlash 0xDC
#define myVK_Comma 0xBC
#define myVK_Period 0xBE
#define myVK_Slash 0xBF
#define myVK_SemiColon 0xBA
#define myVK_SingleQuote 0xDE
#define myVK_LeftBracket 0xDB
#define myVK_RightBracket 0xDD
#define myVK_Grave 0xC0

/* some new ones, need to check if in all header versions */
#define myVK_PRIOR 0x21
#define myVK_NEXT 0x22
#define myVK_END 0x23
#define myVK_HOME 0x24
#define myVK_INSERT 0x2D
#define myVK_DELETE 0x2E
#define myVK_HELP 0x2F
#define myVK_SCROLL 0x91
#define myVK_SNAPSHOT 0x2C
#define myVK_PAUSE 0x13
#define myVK_CLEAR 0x0C

#define myVK_OEM_8 0xDF
#define myVK_OEM_102 0xE2

#ifndef ItnlKyBdFix
#define ItnlKyBdFix 0
#endif

#if ItnlKyBdFix
LOCALVAR ui3b MyVkMapA[256];
#endif

#if ItnlKyBdFix
LOCALPROC MyVkSwapZY(void)
{
	MyVkMapA['Z'] = 'Y';
	MyVkMapA['Y'] = 'Z';
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkSwapGraveQuote(void)
{
	MyVkMapA[myVK_Grave] = myVK_SingleQuote;
	MyVkMapA[myVK_SingleQuote] = myVK_Grave;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkSwapSlashSubtract(void)
{
	MyVkMapA[myVK_Slash] = myVK_Subtract;
	MyVkMapA[myVK_Subtract] = myVK_Slash;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkSwapAQZWGraveQuote(void)
{
	MyVkSwapGraveQuote();
	MyVkMapA['A'] = 'Q';
	MyVkMapA['Q'] = 'A';
	MyVkMapA['Z'] = 'W';
	MyVkMapA['W'] = 'Z';
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapBelgian(void)
{
	MyVkSwapAQZWGraveQuote();
	MyVkMapA['M'] = myVK_SemiColon;
	MyVkMapA[myVK_SemiColon] = myVK_RightBracket;
	MyVkMapA[myVK_RightBracket] = myVK_LeftBracket;
	MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
	MyVkMapA[myVK_Subtract] = myVK_Equal;
	MyVkMapA[myVK_Equal] = myVK_Slash;
	MyVkMapA[myVK_Slash] = myVK_Period;
	MyVkMapA[myVK_Period] = myVK_Comma;
	MyVkMapA[myVK_Comma] = 'M';
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapSwiss(void)
{
	MyVkSwapZY();
	MyVkMapA[myVK_OEM_8] = myVK_BackSlash;
	MyVkMapA[myVK_BackSlash] = myVK_SingleQuote;
	MyVkMapA[myVK_SingleQuote] = myVK_SemiColon;
	MyVkMapA[myVK_SemiColon] = myVK_LeftBracket;
	MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
	MyVkMapA[myVK_Subtract] = myVK_Slash;
	MyVkMapA[myVK_Slash] = myVK_Grave;
	MyVkMapA[myVK_Grave] = myVK_RightBracket;
	MyVkMapA[myVK_RightBracket] = myVK_Equal;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapDanish(void)
{
	MyVkMapA[myVK_Equal] = myVK_Subtract;
	MyVkMapA[myVK_Subtract] = myVK_Slash;
	MyVkMapA[myVK_Slash] = myVK_BackSlash;
	MyVkMapA[myVK_BackSlash] = myVK_Grave;
	MyVkMapA[myVK_Grave] = myVK_SemiColon;
	MyVkMapA[myVK_SemiColon] = myVK_RightBracket;
	MyVkMapA[myVK_RightBracket] = myVK_LeftBracket;
	MyVkMapA[myVK_LeftBracket] = myVK_Equal;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapBritish(void)
{
	MyVkMapA[myVK_OEM_8] = myVK_Grave;
	MyVkMapA[myVK_Grave] = myVK_SingleQuote;
	MyVkMapA[myVK_SingleQuote] = myVK_BackSlash;
	MyVkMapA[myVK_BackSlash] = myVK_OEM_102;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapSpanish(void)
{
	MyVkMapA[myVK_SemiColon] = myVK_LeftBracket;
	MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
	MyVkMapA[myVK_Subtract] = myVK_Slash;
	MyVkMapA[myVK_Slash] = myVK_BackSlash;
	MyVkMapA[myVK_BackSlash] = myVK_Grave;
	MyVkMapA[myVK_Grave] = myVK_SemiColon;

	MyVkMapA[myVK_RightBracket] = myVK_Equal;
	MyVkMapA[myVK_Equal] = myVK_RightBracket;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapDutch(void)
{
	MyVkSwapGraveQuote();
	MyVkMapA[myVK_SemiColon] = myVK_RightBracket;
	MyVkMapA[myVK_RightBracket] = myVK_LeftBracket;
	MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
	MyVkMapA[myVK_Subtract] = myVK_Slash;
	MyVkMapA[myVK_Slash] = myVK_Equal;
	MyVkMapA[myVK_Equal] = myVK_SemiColon;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapGreekIBM(void)
{
	MyVkSwapSlashSubtract();
	MyVkMapA[myVK_LeftBracket] = myVK_Equal;
	MyVkMapA[myVK_Equal] = myVK_LeftBracket;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapFrench(void)
{
	MyVkSwapAQZWGraveQuote();
	MyVkMapA['M'] = myVK_SemiColon;
	MyVkMapA[myVK_SemiColon] = myVK_RightBracket;
	MyVkMapA[myVK_RightBracket] = myVK_LeftBracket;
	MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
	MyVkMapA[myVK_Comma] = 'M';
	MyVkMapA[myVK_Period] = myVK_Comma;
	MyVkMapA[myVK_Slash] = myVK_Period;
	MyVkMapA[myVK_OEM_8] = myVK_Slash;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapGerman(void)
{
	MyVkSwapZY();
	MyVkMapSpanish();
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapBosnian(void)
{
	MyVkSwapZY();
	/* not in Windows 95 */
	MyVkSwapSlashSubtract();
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapBulgarian(void)
{
	MyVkMapA[myVK_OEM_8] = myVK_Comma;
	MyVkMapA[myVK_Comma] = 'Q';
	MyVkMapA['Q'] = myVK_Period;
	MyVkMapA[myVK_Period] = myVK_Equal;
}
#endif

#if ItnlKyBdFix
LOCALPROC MyVkMapFromLayout(uimr sv)
{
	int i;

	for (i = 0; i < 256; ++i) {
		MyVkMapA[i] = i;
	}

	switch (sv) {
		case 0x00000409:
			/* United States 101 */
			break;
		case 0x0000041c:
			/* Albanian; */
			MyVkSwapZY();
			break;
		case 0x0000042B:
			/* Armenian Eastern; */
			MyVkMapDutch();
			break;
		case 0x0001042B:
			/* Armenian Western; */
			MyVkMapDutch();
			break;
		case 0x0000042C:
			/* not in Windows 95 */
			/* Azeri Latin */
			MyVkMapBritish();
			break;
		case 0x0001080C:
			/* Belgian (comma) */
			MyVkMapBelgian();
			break;
		case 0x0000080c:
			/* Belgian French */
			MyVkMapBelgian();
			break;
		case 0x00000813:
			/* not in Windows 95 */
			/* Belgian (period); */
			MyVkMapBelgian();
			break;
		case 0x0000141A:
			/* not in Windows 95 */
			/* Bosnian */
			MyVkMapBosnian();
			break;
		case 0x00000809:
			/* British / United Kingdom */
			MyVkMapBritish();
			break;
		case 0x00000452:
			/* not in Windows 95 */
			/* United Kingdom Extended */
			MyVkMapBritish();
			break;
		case 0x00000402:
			/* Bulgarian */
			/* not same in Windows 95 */
			MyVkMapBulgarian();
			break;
		case 0x00030402:
			/* Bulgarian */
			MyVkMapBulgarian();
			break;
		case 0x00020402:
			/* Bulgarian (Phonetic) */
			MyVkMapBosnian();
			break;
		case 0x00001009:
			/* Canadian Multilingual */
			/* not in Windows 95 */
			MyVkSwapGraveQuote();
			break;
		case 0x00011009:
			/* Canadian Standard */
			MyVkSwapGraveQuote();
			break;
		case 0x0000041a:
			/* Croatian */
			MyVkMapBosnian();
			break;
		case 0x00000405:
			/* Czech */
			MyVkMapBosnian();
#if 0
			/* but Windows 7 gives */
			MyVkSwapZY();
			MyVkMapA[myVK_Equal] = myVK_Subtract;
			MyVkMapA[myVK_Subtract] = myVK_Slash;
			MyVkMapA[myVK_Slash] = myVK_Equal;
#endif
			break;
		case 0x00020405:
			/* Czech (Programmers) */
			/* only in Windows 95 */
			/* MyVkSwapZY(); */
			break;
		case 0x00010405:
			/* Czech (Qwerty) */
			/* only in Windows 95 */
			/* MyVkSwapZY(); */
			break;
		case 0x00000406:
			/* Danish */
			MyVkMapDanish();
			break;
		case 0x00000413:
			/* Dutch */
			MyVkMapDutch();
			break;
		case 0x00000425:
			/* Estonian */
			MyVkMapA[myVK_Grave] = myVK_LeftBracket;
			MyVkMapA[myVK_LeftBracket] = myVK_RightBracket;
			MyVkMapA[myVK_RightBracket] = myVK_Slash;
			MyVkMapA[myVK_Slash] = myVK_SingleQuote;
			MyVkMapA[myVK_SingleQuote] = myVK_Grave;
			/* only in Windows 95 ? */
			/* MyVkMapA[VK_DECIMAL] = VK_DELETE; */
			break;
		case 0x00000438:
			/* Faeroe Islands */
			MyVkMapDanish();
			break;
		case 0x0000040b:
			/* Finnish */
			MyVkMapDanish();
			break;
		case 0x0001083B:
			/* not in Windows 95 */
			/* Finnish with Sami */
			MyVkMapDanish();
			break;
		case 0x0000040c:
			/* v = kMyKbdFrench; */
			/* French */
			MyVkMapFrench();
			break;
		case 0x00000c0c:
			/* French Canadian */
			MyVkSwapGraveQuote();
			break;
		case 0x00011809:
			/* not in Windows 95 */
			/* Gaelic */
			MyVkMapBritish();
			break;
		case 0x00010407:
			/* German (IBM) */
			MyVkMapGerman();
			break;
		case 0x00000407:
			/* German (Standard) */
			MyVkMapGerman();
			break;
		case 0x00010408:
			/* Greek IBM 220 */
			/* not in Windows 95 */
			MyVkMapGreekIBM();
			break;
		case 0x00030408:
			/* Greek IBM 319 */
			/* not in Windows 95 */
			MyVkMapGreekIBM();
			break;
		case 0x00020408:
			/* Greek Latin IBM 220 */
			/* not in Windows 95 */
			MyVkSwapSlashSubtract();
			break;
		case 0x00040408:
			/* Greek Latin IBM 319 */
			/* not in Windows 95 */
			MyVkSwapSlashSubtract();
			break;
		case 0x0000040e:
			/* Hungarian */
			MyVkMapBosnian();
			MyVkMapA[myVK_Grave] = '0';
			MyVkMapA['0'] = myVK_Grave;
			break;
		case 0x0001040E:
			/* Hungarian (101 Keys) */
			MyVkMapA[myVK_Grave] = '0';
			MyVkMapA['0'] = myVK_Grave;
			break;
		case 0x0000040f:
			/* Icelandic */
			MyVkMapDanish();
			break;
		case 0x00001809:
			/* Irish */
			MyVkMapBritish();
			break;
		case 0x00000410:
			/* Italian */
			MyVkMapSpanish();
			break;
		case 0x00010410:
			/* Italian 142 */
			MyVkMapSpanish();
			break;
		case 0x0000080a:
			/* Latin American */
			MyVkMapSpanish();
			break;
		case 0x0000046E:
			/* Luxembourgish */
			MyVkMapSwiss();
			break;
		case 0x00000414:
			/* Norwegian */
			MyVkMapDanish();
			break;
		case 0x0000043B:
			/* Norwegian with Sami */
			MyVkMapDanish();
			break;
		case 0x00010415:
			/* Polish (214) */
			MyVkSwapZY();
			/* not in windows 95 */
			MyVkMapA[myVK_Equal] = myVK_Subtract;
			MyVkMapA[myVK_Subtract] = myVK_Slash;
			MyVkMapA[myVK_Slash] = myVK_Equal;
			break;
		case 0x00010416:
			/* Porguguese (Brazilian ABNT2) */
			/* MyVkMapA[myVK_OEM_8] = ??; */
			/* MyVkMapA[VK_SEPARATOR] = ??; */
			break;
		case 0x00000816:
			/* Porguguese (Standard) */
			MyVkMapA[myVK_SemiColon] = myVK_RightBracket;
			MyVkMapA[myVK_RightBracket] = myVK_Equal;
			MyVkMapA[myVK_Equal] = myVK_LeftBracket;
			MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
			MyVkMapA[myVK_Subtract] = myVK_Slash;
			MyVkMapA[myVK_Slash] = myVK_BackSlash;
			MyVkMapA[myVK_BackSlash] = myVK_Grave;
			MyVkMapA[myVK_Grave] = myVK_SemiColon;
			break;
		case 0x00000418:
			/* Romanian (Legacy) */
			MyVkSwapZY();
			/* only in Windows 95 */
			/* MyVkSwapSlashSubtract(); */
			break;
		case 0x0002083B:
			/* Sami Extended Finland-Sweden */
			MyVkMapDanish();
			break;
		case 0x0001043B:
			/* Sami Extended Norway */
			MyVkMapDanish();
			break;
		case 0x00010C1A:
			/* in Windows 95 */
			/* Serbian (Latin) */
			MyVkSwapZY();
			break;
		case 0x0000081A:
			/* not in Windows 95 */
			/* Serbian (Latin) */
			MyVkMapBosnian();
			break;
		case 0x0000041b:
			/* Slovak */
			MyVkMapBosnian();
			/* not in Windows 95 */
			MyVkMapA[myVK_OEM_8] = myVK_Equal;
			break;
		case 0x00000424:
			/* Slovenian */
			MyVkMapBosnian();
			break;
		case 0x0000040A:
			/* Spanish, not windows 95 */
			MyVkMapSpanish();
			break;
		case 0x0001040A:
			/* Spanish Variation, not windows 95 */
			MyVkMapA[myVK_OEM_8] = myVK_Slash;
			MyVkMapA[myVK_Slash] = myVK_BackSlash;
			MyVkMapA[myVK_BackSlash] = myVK_Grave;
			MyVkMapA[myVK_Grave] = myVK_SemiColon;
			MyVkMapA[myVK_SemiColon] = myVK_RightBracket;
			MyVkMapA[myVK_RightBracket] = myVK_LeftBracket;
			MyVkMapA[myVK_LeftBracket] = myVK_Equal;
			break;
		case 0x00000c0a:
			/* kMyKbdSpanish; */
			/* Spanish Modern, windows 95 */
			MyVkMapSpanish();
			break;
		case 0x00000403:
			/* Spanish Traditional */
			MyVkMapSpanish();
			break;
		case 0x0000041d:
			/* Swedish */
			MyVkMapDanish();
			break;
		case 0x0000083B:
			/* not in windows 95 */
			/* Swedish with Sami */
			MyVkMapDanish();
			break;
		case 0x0000100c:
			/* Swiss French */
			MyVkMapSwiss();
			break;
		case 0x00000807:
			/* Swiss German */
			MyVkMapSwiss();
			break;
		case 0x0000085D:
			/* Inuktitut Latin */
			/* in windows 7, not XP */
			MyVkMapBritish();
			break;
		case 0x0001045D:
			/* Inuktitut - Naqittaut */
			MyVkMapBritish();
			break;
		case 0x0000046F:
			/* Greenlandic */
			MyVkMapDanish();
			break;
		case 0x00020427:
			/* Lithuanian Standard */
			MyVkMapDanish();
			break;
		case 0x0000042f:
			/* Macedonian (FYROM) - Standard */
			MyVkMapBosnian();
			break;
		case 0x0000042E:
			/* Sorbian Standard (Legacy) */
			MyVkMapGerman();
			break;
		case 0x0001042E:
			/* Sorbian Extended */
			MyVkMapGerman();
			break;
		case 0x0002042E:
			/* Sorbian Standard */
			MyVkMapGerman();
			break;
		case 0x00000488:
			/* Wolof */
			MyVkMapFrench();
			break;
		case 0x0000041f:
			/* Turkish (Q type) */
			/* windows 95 */
			/* MyVkMapA[myVK_Equal] = myVK_Subtract; */
			/* MyVkMapA[myVK_Subtract] = myVK_Equal; */
			/* not windows 95 */
			MyVkMapA[myVK_OEM_8] = myVK_Subtract;
			MyVkMapA[myVK_Subtract] = myVK_Equal;

			MyVkMapA[myVK_Comma] = myVK_BackSlash;
			MyVkMapA[myVK_BackSlash] = myVK_Period;
			MyVkMapA[myVK_Period] = myVK_Slash;
			MyVkMapA[myVK_Slash] = myVK_Comma;
			break;
		case 0x00010409:
			/* United States Dvorak */
			MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
			MyVkMapA[myVK_RightBracket] = myVK_Equal;
			MyVkMapA[myVK_SingleQuote] = 'Q';
			MyVkMapA[myVK_Comma] = 'W';
			MyVkMapA[myVK_Period] = 'E';
			MyVkMapA['P'] = 'R';
			MyVkMapA['Y'] = 'T';
			MyVkMapA['F'] = 'Y';
			MyVkMapA['G'] = 'U';
			MyVkMapA['C'] = 'I';
			MyVkMapA['R'] = 'O';
			MyVkMapA['L'] = 'P';
			MyVkMapA[myVK_Slash] = myVK_LeftBracket;
			MyVkMapA[myVK_Equal] = myVK_RightBracket;
			MyVkMapA['O'] = 'S';
			MyVkMapA['E'] = 'D';
			MyVkMapA['U'] = 'F';
			MyVkMapA['I'] = 'G';
			MyVkMapA['D'] = 'H';
			MyVkMapA['H'] = 'J';
			MyVkMapA['T'] = 'K';
			MyVkMapA['N'] = 'L';
			MyVkMapA['S'] = myVK_SemiColon;
			MyVkMapA[myVK_Subtract] = myVK_SingleQuote;
			MyVkMapA[myVK_SemiColon] = 'Z';
			MyVkMapA['Q'] = 'X';
			MyVkMapA['J'] = 'C';
			MyVkMapA['K'] = 'V';
			MyVkMapA['X'] = 'B';
			MyVkMapA['B'] = 'N';
			MyVkMapA['W'] = myVK_Comma;
			MyVkMapA['V'] = myVK_Period;
			MyVkMapA['Z'] = myVK_Slash;
			break;
#if 0
		/* too complicated, don't bother with */
		case 0x0x00000426:
			/* Latvian */
			MyVkMapA['F'] = myVK_Equal;
			MyVkMapA['G'] = 'W';
			MyVkMapA['J'] = 'E';
			MyVkMapA['M'] = 'T';
			MyVkMapA['V'] = 'Y';
			MyVkMapA['N'] = 'U';
			MyVkMapA['Z'] = 'I';
			MyVkMapA['W'] = 'O';
			MyVkMapA['X'] = 'P';
			MyVkMapA['Y'] = myVK_LeftBracket;
			MyVkMapA['H'] = myVK_RightBracket;
			MyVkMapA[myVK_SemiColon] = 'A';
			MyVkMapA['U'] = 'S';
			MyVkMapA['S'] = 'D';
			MyVkMapA['I'] = 'F';
			MyVkMapA['L'] = 'G';
			MyVkMapA['D'] = 'H';
			MyVkMapA['A'] = 'J';
			MyVkMapA['T'] = 'K';
			MyVkMapA['E'] = 'L';
			MyVkMapA['C'] = myVK_SemiColon;
			MyVkMapA[myVK_LeftBracket] = 'Z';
			MyVkMapA['B'] = 'X';
			MyVkMapA[myVK_RightBracket] = 'C';
			MyVkMapA['K'] = 'V';
			MyVkMapA['P'] = 'B';
			MyVkMapA['O'] = 'N';
			MyVkMapA[myVK_OEM_8] = 'M';
			break;
		case 0x0001041F:
			/* Turkish (F type) */

			MyVkMapA[myVK_Equal] = myVK_Subtract;
			MyVkMapA[myVK_Subtract] = myVK_Equal;
			MyVkMapA['F'] = 'Q';
			MyVkMapA['G'] = 'W';
			MyVkMapA[myVK_SemiColon] = 'E';
			MyVkMapA['I'] = 'R';
			MyVkMapA['O'] = 'T';
			MyVkMapA['D'] = 'Y';
			MyVkMapA['R'] = 'U';
			MyVkMapA['N'] = 'I';
			MyVkMapA['H'] = 'O';
			MyVkMapA['Q'] = myVK_LeftBracket;
			MyVkMapA['W'] = myVK_RightBracket;
			MyVkMapA['U'] = 'A';
			MyVkMapA[myVK_LeftBracket] = 'S';
			MyVkMapA['E'] = 'D';
			MyVkMapA['A'] = 'F';
			MyVkMapA[myVK_RightBracket] = 'G';
			MyVkMapA['T'] = 'H';
			MyVkMapA['K'] = 'J';
			MyVkMapA['M'] = 'K';
			MyVkMapA['Y'] = myVK_SemiColon;
			MyVkMapA['X'] = myVK_BackSlash;
			MyVkMapA['J'] = 'Z';
			MyVkMapA[myVK_BackSlash] = 'X';
			MyVkMapA['V'] = 'C';
			MyVkMapA['C'] = 'V';
			MyVkMapA[myVK_Slash] = 'B';
			MyVkMapA['Z'] = 'N';
			MyVkMapA['S'] = 'M';
			MyVkMapA['B'] = myVK_Comma;
			MyVkMapA[myVK_Comma] = myVK_Slash;
			break;
		case 0x00030409:
			/* United States LH Dvorak */
			MyVkMapA[myVK_LeftBracket] = '1';
			MyVkMapA[myVK_RightBracket] = '2';
			MyVkMapA[myVK_Slash] = '3';
			MyVkMapA['P'] = '4';
			MyVkMapA['F'] = '5';
			MyVkMapA['M'] = '6';
			MyVkMapA['L'] = '7';
			MyVkMapA['J'] = '8';
			MyVkMapA['4'] = '9';
			MyVkMapA['3'] = '0';
			MyVkMapA['2'] = myVK_Subtract;
			MyVkMapA['1'] = myVK_Equal;
			MyVkMapA[myVK_SemiColon] = 'Q';
			MyVkMapA['Q'] = 'W';
			MyVkMapA['B'] = 'E';
			MyVkMapA['Y'] = 'R';
			MyVkMapA['U'] = 'T';
			MyVkMapA['R'] = 'Y';
			MyVkMapA['S'] = 'U';
			MyVkMapA['O'] = 'I';
			MyVkMapA[myVK_Period] = 'O';
			MyVkMapA['6'] = 'P';
			MyVkMapA['5'] = myVK_LeftBracket;
			MyVkMapA[myVK_Equal] = myVK_RightBracket;
			MyVkMapA[myVK_Subtract] = 'A';
			MyVkMapA['K'] = 'S';
			MyVkMapA['C'] = 'D';
			MyVkMapA['D'] = 'F';
			MyVkMapA['T'] = 'G';
			MyVkMapA['E'] = 'J';
			MyVkMapA['A'] = 'K';
			MyVkMapA['Z'] = 'L';
			MyVkMapA['8'] = myVK_SemiColon;
			MyVkMapA['7'] = myVK_SingleQuote;
			MyVkMapA[myVK_SingleQuote] = 'Z';
			MyVkMapA['G'] = 'C';
			MyVkMapA['W'] = 'B';
			MyVkMapA['I'] = 'M';
			MyVkMapA['0'] = myVK_Period;
			MyVkMapA['9'] = myVK_Slash;
			break;
		case 0x00040409:
			/* United States RH Dvorak */
			MyVkMapA['J'] = '5';
			MyVkMapA['L'] = '6';
			MyVkMapA['M'] = '7';
			MyVkMapA['F'] = '8';
			MyVkMapA['P'] = '9';
			MyVkMapA[myVK_Slash] = '0';
			MyVkMapA[myVK_LeftBracket] = myVK_Subtract;
			MyVkMapA[myVK_RightBracket] = myVK_Equal;
			MyVkMapA['5'] = 'Q';
			MyVkMapA['6'] = 'W';
			MyVkMapA['Q'] = 'E';
			MyVkMapA[myVK_Period] = 'R';
			MyVkMapA['O'] = 'T';
			MyVkMapA['R'] = 'Y';
			MyVkMapA['S'] = 'U';
			MyVkMapA['U'] = 'I';
			MyVkMapA['Y'] = 'O';
			MyVkMapA['B'] = 'P';
			MyVkMapA[myVK_SemiColon] = myVK_LeftBracket;
			MyVkMapA[myVK_Equal] = myVK_RightBracket;
			MyVkMapA['7'] = 'A';
			MyVkMapA['8'] = 'S';
			MyVkMapA['Z'] = 'D';
			MyVkMapA['A'] = 'F';
			MyVkMapA['E'] = 'G';
			MyVkMapA['T'] = 'J';
			MyVkMapA['D'] = 'K';
			MyVkMapA['C'] = 'L';
			MyVkMapA['K'] = myVK_SemiColon;
			MyVkMapA[myVK_Subtract] = myVK_SingleQuote;
			MyVkMapA['9'] = 'Z';
			MyVkMapA['0'] = 'X';
			MyVkMapA['X'] = 'C';
			MyVkMapA[myVK_Comma] = 'V';
			MyVkMapA['I'] = 'B';
			MyVkMapA['W'] = 'M';
			MyVkMapA['V'] = myVK_Comma;
			MyVkMapA['G'] = myVK_Period;
			MyVkMapA[myVK_SingleQuote] = myVK_Slash;
			break;
#endif
#if 0
		case 0x0000082C:
			/* not in Windows 95 */
			/* Azeri Cyrillic */
			break;
		case 0x00000423:
			/* Belarusian */
			break;
		case 0x00000445:
			/* not in Windows 95 */
			/* Bengali */
			break;
		case 0x00010445:
			/* not in Windows 95 */
			/* Bengali (Inscript) */
			break;
		case 0x0000201A:
			/* not in Windows 95 */
			/* Bosnian Cyrillic*/
			break;
		case 0x00010402:
			/* Bulgarian Latin */
#if 0 /* Only in Windows 95 */
			MyVkMapA['J'] = 'Q';
			MyVkMapA['C'] = 'W';
			MyVkMapA['U'] = 'E';
			MyVkMapA['K'] = 'R';
			MyVkMapA['E'] = 'T';
			MyVkMapA['N'] = 'Y';
			MyVkMapA['G'] = 'U';
			MyVkMapA[myVK_SemiColon] = 'I';
			MyVkMapA[myVK_OEM_102] = 'O';
			MyVkMapA['Z'] = 'P';
			MyVkMapA['H'] = myVK_LeftBracket;
			MyVkMapA['F'] = 'A';
			MyVkMapA['Y'] = 'S';
			MyVkMapA['W'] = 'D';
			MyVkMapA['A'] = 'F';
			MyVkMapA['P'] = 'G';
			MyVkMapA['R'] = 'H';
			MyVkMapA['O'] = 'J';
			MyVkMapA['L'] = 'K';
			MyVkMapA['D'] = 'L';
			MyVkMapA['V'] = myVK_SemiColon;
			MyVkMapA[myVK_LeftBracket] = 'Z';
			MyVkMapA['S'] = 'X';
			MyVkMapA['M'] = 'C';
			MyVkMapA['I'] = 'V';
			MyVkMapA['T'] = 'B';
			MyVkMapA['X'] = 'N';
			MyVkMapA['B'] = 'M';
			MyVkMapA['Q'] = myVK_OEM_102;
#endif
			break;
		case 0x00000408:
			/* Greek */
			break;
		case 0x00050408:
			/* Greek Latin */
			break;
		case 0x00060408:
			/* Greek Polytonic */
			break;
		case 0x0000043F:
			/* Kazakh */
			break;
		case 0x00000440:
			/* Kyrgyz Cyrillic */
			break;
		case 0x00010426:
			/* Latvian Latin */
			break;
		case 0x00010427:
			/* Lithuanian */
			break;
		case 0x00000427:
			/* Lithuanian (IBM) */
			break;
		case 0x0000044C:
			/* Malayalam */
			break;
		case 0x0000042f:
			/* Macedonian (FYROM) */
			break;
		case 0x0000043A:
			/* Maltese 47-key */
			break;
		case 0x0001043A:
			/* Maltese 48-key */
			break;
		case 0x00000481:
			/* Maori */
			break;
		case 0x00000450:
			/* Mongolian Cyrillic */
			break;
		case 0x00000461:
			/* Nepali */
			break;
		case 0x00000463:
			/* Pashto */
			break;
		case 0x00000415:
			/* Polish (Programmers) */
			break;
		case 0x00000416:
			/* Porguguese (Brazilian standard) */
			break;
		case 0x00000419:
			/* Russian */
			break;
		case 0x00010419:
			/* Russian (Typewriter) */
			break;
		case 0x00000c1a:
			/* Serbian */
			break;
		case 0x0001041B:
			/* Slovak (Qwerty) */
			break;
		case 0x00000444:
			/* Tatar */
			break;
		case 0x00000422:
			/* Ukrainian */
			break;
		case 0x00020409:
			/* United States International */
			break;
		case 0x00000843:
			/* Uzbek Cyrillic */
			break;
		case 0x00010418:
			/* Romanian (Standard) */
			break;
		case 0x00020418:
			/* Romanian (Programmers) */
			break;
		case 0x00000401:
			/* Arabic (101) */
			break;
		case 0x00010401:
			/* Arabic (102) */
			break;
		case 0x0000044D:
			/* Assamese - INSCRIPT */
			break;
		case 0x0000046D:
			/* Bashkir */
			break;
		case 0x00040402:
			/* Bulgarian (Phonetic Traditional) */
			break;
		case 0x00000404:
			/* Chinese (Traditional) */
			break;
		case 0x00000804:
			/* Chinese (Simplified) */
			break;
		case 0x00000C04:
			/* Chinese (Traditional, Hong Kong S.A.R.) */
			break;
		case 0x00001004:
			/* Chinese (Simplified, Singapore) */
			break;
		case 0x00001404:
			/* Chinese (Traditional, Macao S.A.R.) */
			break;
		case 0x0000040D:
			/* Hebrew */
			break;
		case 0x00000447:
			/* Gujarati */
			break;
		case 0x00000468:
			/* Hausa */
			break;
		case 0x00010439:
			/* Hindi Traditional */
			break;
		case 0x00000439:
			/* Devanagari - INSCRIPT */
			break;
		case 0x00000465:
			/* Divehi Phonetic */
			break;
		case 0x00010465:
			/* Divehi Typewriter */
			break;
		case 0x00000437:
			/* Georgian */
			break;
		case 0x00010437:
			/* Georgian (QWERTY) */
			break;
		case 0x00020437:
			/* Georgian (Ergonomic) */
			break;
		case 0x00000470:
			/* Igbo */
			break;
		case 0x00000411:
			/* Japanese */
			/* MyVkMapA[??] = ??; */
			break;
		case 0x00000412:
			/* Korean */
			/* MyVkMapA[VK_ZOOM] = ??; */
			/* MyVkMapA[VK_HELP] = VK_ZOOM; */
			/* MyVkMapA[??] = VK_HELP; */
			/* MyVkMapA[??] = ??; */
			break;
		case 0x0000044B:
			/* Kannada */
			break;
		case 0x00000453:
			/* Khmer */
			break;
		case 0x00000454:
			/* Lao */
			break;
		case 0x00000448:
			/* Oriya */
			break;
		case 0x0000044E:
			/* Marathi */
			break;
		case 0x00000850:
			/* Mongolian (Mongolian Script) */
			break;
		case 0x00000429:
			/* Persion */
			break;
		case 0x00000446:
			/* Punjabi */
			break;
		case 0x0000046C:
			/* Sesotho sa Leboa */
			break;
		case 0x00000432:
			/* Setswana */
			break;
		case 0x0000045B:
			/* Sinhala */
			break;
		case 0x0001045B:
			/* Sinhala - Wij 9 */
			break;
		case 0x0000045A:
			/* Syriac */
			break;
		case 0x0001045A:
			/* Syriac Phonetic */
			break;
		case 0x00000428:
			/* Tajik */
			break;
		case 0x00000449:
			/* Tamil */
			break;
		case 0x0000044A:
			/* Telugu */
			break;
		case 0x0000041E:
			/* Thai Kedmanee */
			break;
		case 0x0001041E:
			/* Thai Pattachote */
			break;
		case 0x0002041E:
			/* Thai Kedmanee (non-ShiftLock) */
			break;
		case 0x0003041E:
			/* Thai Pattachote (non-ShiftLock) */
			break;
		case 0x00000451:
			/* Tibetan (PRC) */
			break;
		case 0x00000442:
			/* Turkmen */
			break;
		case 0x00020422:
			/* Ukrainian (Enhanced) */
			break;
		case 0x00000420:
			/* Urdu */
			break;
		case 0x00050409:
			/* US English Table for IBM Arabic 238_L */
			break;
		case 0x00000480:
			/* Uyghur (Legacy) */
			break;
		case 0x00010480:
			/* Uyghur */
			break;
		case 0x0000042A:
			/* Vietnamese */
			break;
		case 0x00000485:
			/* Yakut */
			break;
		case 0x0000046A:
			/* Yoruba */
			break;
#endif
	}
}
#endif

#if ItnlKyBdFix
LOCALVAR uimr CurKyBdLytNm = 0;
#endif

#if ItnlKyBdFix
LOCALFUNC blnr tStrIsHex(TCHAR *s, int n, uimr *r)
{
	short i;
	TCHAR c1;
	TCHAR *p = s;
	uimr v = 0;

	for (i = n; --i >= 0; ) {
		v <<= 4;
		c1 = *p++;
		if ((c1 >= '0') && (c1 <= '9')) {
			v += c1 - '0';
		} else if ((c1 >= 'A') && (c1 <= 'F')) {
			v += c1 - ('A' - 10);
		} else if ((c1 >= 'a') && (c1 <= 'f')) {
			v += c1 - ('a' - 10);
		} else {
			return falseblnr;
		}
	}

	*r = v;
	return trueblnr;
}
#endif

#if ItnlKyBdFix
LOCALFUNC blnr MyGetKeyboardLayoutHex(uimr *r)
{
	TCHAR s[KL_NAMELENGTH];
	blnr IsOk = falseblnr;

	if (! GetKeyboardLayoutName(s)) {
		/* ReportWinLastError(); */
	} else {
		size_t n = _tcslen(s);

		if (8 != n) {
			/* fail */
		} else {
			IsOk = tStrIsHex(s, n, r);
		}
	}

	return IsOk;
}
#endif

#if ItnlKyBdFix && ! UseWinCE
LOCALPROC MyCheckKeyboardLayout(void)
{
	uimr sv;

	if (! MyGetKeyboardLayoutHex(&sv)) {
	} else if (sv == CurKyBdLytNm) {
		/* no change */
	} else {
		CurKyBdLytNm = sv;

		MyVkMapFromLayout(sv);
	}
}
#endif

#if ItnlKyBdFix
LOCALPROC MyInitCheckKeyboardLayout(void)
{
	uimr sv;

	if (! MyGetKeyboardLayoutHex(&sv)) {
		sv = 0x00000409;
	}

	CurKyBdLytNm = sv;

	MyVkMapFromLayout(sv);
}
#endif

LOCALVAR ui3b WinKey2Mac[256];

LOCALPROC AssignOneMacKey(ui3b WinKey, ui3r MacKey)
{
	WinKey2Mac[WinKey] = MacKey;
}

LOCALFUNC blnr InitWinKey2Mac(void)
{
	int i;

	for (i = 0; i < 256; ++i) {
		WinKey2Mac[i] = MKC_None;
	}

	AssignOneMacKey('A', MKC_A);
	AssignOneMacKey('S', MKC_S);
	AssignOneMacKey('D', MKC_D);
	AssignOneMacKey('F', MKC_F);
	AssignOneMacKey('H', MKC_H);
	AssignOneMacKey('G', MKC_G);
	AssignOneMacKey('Z', MKC_Z);
	AssignOneMacKey('X', MKC_X);
	AssignOneMacKey('C', MKC_C);
	AssignOneMacKey('V', MKC_V);
	AssignOneMacKey('B', MKC_B);
	AssignOneMacKey('Q', MKC_Q);
	AssignOneMacKey('W', MKC_W);
	AssignOneMacKey('E', MKC_E);
	AssignOneMacKey('R', MKC_R);
	AssignOneMacKey('Y', MKC_Y);
	AssignOneMacKey('T', MKC_T);
	AssignOneMacKey('1', MKC_1);
	AssignOneMacKey('2', MKC_2);
	AssignOneMacKey('3', MKC_3);
	AssignOneMacKey('4', MKC_4);
	AssignOneMacKey('6', MKC_6);
	AssignOneMacKey('5', MKC_5);
	AssignOneMacKey(myVK_Equal, MKC_Equal);
	AssignOneMacKey('9', MKC_9);
	AssignOneMacKey('7', MKC_7);
	AssignOneMacKey(myVK_Subtract, MKC_Minus);
	AssignOneMacKey('8', MKC_8);
	AssignOneMacKey('0', MKC_0);
	AssignOneMacKey(myVK_RightBracket, MKC_RightBracket);
	AssignOneMacKey('O', MKC_O);
	AssignOneMacKey('U', MKC_U);
	AssignOneMacKey(myVK_LeftBracket, MKC_LeftBracket);
	AssignOneMacKey('I', MKC_I);
	AssignOneMacKey('P', MKC_P);
	AssignOneMacKey(VK_RETURN, MKC_Return);
	AssignOneMacKey('L', MKC_L);
	AssignOneMacKey('J', MKC_J);
	AssignOneMacKey(myVK_SingleQuote, MKC_SingleQuote);
	AssignOneMacKey('K', MKC_K);
	AssignOneMacKey(myVK_SemiColon, MKC_SemiColon);
	AssignOneMacKey(myVK_BackSlash, MKC_formac_BackSlash);
	AssignOneMacKey(myVK_Comma, MKC_Comma);
	AssignOneMacKey(myVK_Slash, MKC_formac_Slash);
	AssignOneMacKey('N', MKC_N);
	AssignOneMacKey('M', MKC_M);
	AssignOneMacKey(myVK_Period, MKC_Period);

	AssignOneMacKey(VK_TAB, MKC_Tab);
	AssignOneMacKey(VK_SPACE, MKC_Space);
	AssignOneMacKey(myVK_Grave, MKC_formac_Grave);
	AssignOneMacKey(VK_BACK, MKC_BackSpace);
	AssignOneMacKey(VK_ESCAPE, MKC_formac_Escape);

	AssignOneMacKey(VK_MENU, MKC_formac_Command);

	AssignOneMacKey(VK_LMENU, MKC_formac_Command);

	AssignOneMacKey(VK_RMENU, MKC_formac_RCommand);

	AssignOneMacKey(VK_SHIFT, MKC_formac_Shift);
	AssignOneMacKey(VK_LSHIFT, MKC_formac_Shift);
	AssignOneMacKey(VK_RSHIFT, MKC_formac_RShift);

	AssignOneMacKey(VK_CAPITAL, MKC_formac_CapsLock);

	AssignOneMacKey(VK_APPS, MKC_formac_ROption);

	AssignOneMacKey(VK_LWIN, MKC_formac_Option);

	AssignOneMacKey(VK_RWIN, MKC_formac_ROption);

	AssignOneMacKey(VK_CONTROL, MKC_formac_Control);

	AssignOneMacKey(VK_LCONTROL, MKC_formac_Control);

	AssignOneMacKey(VK_RCONTROL, MKC_formac_RControl);

	AssignOneMacKey(VK_F1, MKC_formac_F1);
	AssignOneMacKey(VK_F2, MKC_formac_F2);
	AssignOneMacKey(VK_F3, MKC_formac_F3);
	AssignOneMacKey(VK_F4, MKC_formac_F4);
	AssignOneMacKey(VK_F5, MKC_formac_F5);
	AssignOneMacKey(VK_F6, MKC_F6);
	AssignOneMacKey(VK_F7, MKC_F7);
	AssignOneMacKey(VK_F8, MKC_F8);
	AssignOneMacKey(VK_F9, MKC_F9);
	AssignOneMacKey(VK_F10, MKC_F10);
	AssignOneMacKey(VK_F11, MKC_F11);
	AssignOneMacKey(VK_F12, MKC_F12);

	AssignOneMacKey(VK_DECIMAL, MKC_Decimal);
	AssignOneMacKey(VK_DELETE, MKC_Decimal);
	/* AssignOneMacKey(VK_RIGHT, 0x42); */
	AssignOneMacKey(VK_MULTIPLY, MKC_KPMultiply);
	AssignOneMacKey(VK_ADD, MKC_KPAdd);
	/* AssignOneMacKey(VK_LEFT, 0x46); */
	AssignOneMacKey(VK_NUMLOCK, MKC_Clear);

	/* AssignOneMacKey(VK_DOWN, 0x48); */
	AssignOneMacKey(VK_DIVIDE, MKC_KPDevide);
	/* AssignOneMacKey(VK_RETURN, MKC_formac_Enter); */
	/* AssignOneMacKey(VK_UP, 0x4D); */
	AssignOneMacKey(VK_DIVIDE, MKC_KPDevide);
	AssignOneMacKey(VK_SUBTRACT, MKC_KPSubtract);

	AssignOneMacKey(VK_SEPARATOR, MKC_KPEqual);
	AssignOneMacKey(VK_NUMPAD0, MKC_KP0);
	AssignOneMacKey(VK_NUMPAD1, MKC_KP1);
	AssignOneMacKey(VK_NUMPAD2, MKC_KP2);
	AssignOneMacKey(VK_NUMPAD3, MKC_KP3);
	AssignOneMacKey(VK_NUMPAD4, MKC_KP4);
	AssignOneMacKey(VK_NUMPAD5, MKC_KP5);

	AssignOneMacKey(VK_NUMPAD6, MKC_KP6);
	AssignOneMacKey(VK_NUMPAD7, MKC_KP7);
	AssignOneMacKey(VK_NUMPAD8, MKC_KP8);
	AssignOneMacKey(VK_NUMPAD9, MKC_KP9);

	AssignOneMacKey(VK_LEFT, MKC_Left);
	AssignOneMacKey(VK_RIGHT, MKC_Right);
	AssignOneMacKey(VK_DOWN, MKC_Down);
	AssignOneMacKey(VK_UP, MKC_Up);

	AssignOneMacKey(myVK_PRIOR, MKC_formac_PageUp);
	AssignOneMacKey(myVK_NEXT, MKC_formac_PageDown);
	AssignOneMacKey(myVK_END, MKC_formac_End);
	AssignOneMacKey(myVK_HOME, MKC_formac_Home);
	AssignOneMacKey(myVK_INSERT, MKC_formac_Help);
	AssignOneMacKey(myVK_DELETE, MKC_formac_ForwardDel);
	AssignOneMacKey(myVK_HELP, MKC_formac_Help);
	AssignOneMacKey(myVK_SNAPSHOT, MKC_Print);
	AssignOneMacKey(myVK_SCROLL, MKC_ScrollLock);
	AssignOneMacKey(myVK_PAUSE, MKC_Pause);

	AssignOneMacKey(myVK_OEM_102, MKC_AngleBracket);

	InitKeyCodes();

#if ItnlKyBdFix
	MyInitCheckKeyboardLayout();
#endif

	return trueblnr;
}

LOCALPROC DoKeyCode(int i, blnr down)
{
	ui3r key = WinKey2Mac[
#if ItnlKyBdFix
		MyVkMapA[i]
#else
		i
#endif
		];
	if (MKC_None != key) {
		Keyboard_UpdateKeyMap2(key, down);
	}
}

#ifndef EnableGrabSpecialKeys
#if UseWinCE
#define EnableGrabSpecialKeys 0
#else
#define EnableGrabSpecialKeys (MayFullScreen && GrabKeysFullScreen)
#endif
#endif /* EnableGrabSpecialKeys */

#if EnableGrabSpecialKeys
LOCALVAR blnr HaveSetSysParam = falseblnr;
#endif

LOCALPROC CheckTheCapsLock(void)
{
	DoKeyCode(VK_CAPITAL, (GetKeyState(VK_CAPITAL) & 1) != 0);
}

#if EnableGrabSpecialKeys
LOCALVAR blnr VK_LWIN_pressed = falseblnr;
LOCALVAR blnr VK_RWIN_pressed = falseblnr;
#endif

#if EnableGrabSpecialKeys
LOCALPROC CheckForLostKeyUps(void)
{
	if (HaveSetSysParam) {
		/* check for lost key ups */
		if (VK_LWIN_pressed) {
			if ((GetAsyncKeyState(VK_LWIN) & 0x8000) == 0) {
				DoKeyCode(VK_LWIN, falseblnr);
				VK_LWIN_pressed = falseblnr;
			}
		}
		if (VK_RWIN_pressed) {
			if ((GetAsyncKeyState(VK_RWIN) & 0x8000) == 0) {
				DoKeyCode(VK_RWIN, falseblnr);
				VK_RWIN_pressed = falseblnr;
			}
		}
	}
}
#endif

LOCALPROC DoVKcode0(int i, blnr down)
{
#if EnableGrabSpecialKeys
	if (HaveSetSysParam) {
		/* will need to check for lost key ups */
		if (VK_LWIN == i) {
			VK_LWIN_pressed = down;
		} else if (VK_RWIN == i) {
			VK_RWIN_pressed = down;
		}
	}
#endif
	DoKeyCode(i, down);
}

LOCALPROC DoVKcode(int i, ui3r flags, blnr down)
{
	switch (i) {
#if MKC_formac_Control != MKC_formac_RControl
		case VK_CONTROL:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_RControl : MKC_formac_Control,
				down);
			break;
#endif
#if MKC_formac_RCommand != MKC_formac_Command
		case VK_MENU:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_RCommand : MKC_formac_Command,
				down);
			break;
#endif
		case VK_RETURN:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_Enter : MKC_Return,
				down);
			break;
		case myVK_HOME:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_Home : MKC_KP7,
				down);
			break;
		case VK_UP:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_Up : MKC_KP8,
				down);
			break;
		case myVK_PRIOR:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_PageUp : MKC_KP9,
				down);
			break;
		case VK_LEFT:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_Left : MKC_KP4,
				down);
			break;
		case myVK_CLEAR:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_Clear : MKC_KP5,
				down);
			break;
		case VK_RIGHT:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_Right : MKC_KP6,
				down);
			break;
		case myVK_END:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_End : MKC_KP1,
				down);
			break;
		case VK_DOWN:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_Down : MKC_KP2,
				down);
			break;
		case myVK_NEXT:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_PageDown : MKC_KP3,
				down);
			break;
		case myVK_INSERT:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_Help : MKC_KP0,
				down);
			break;
		case myVK_DELETE:
			Keyboard_UpdateKeyMap2(TestBit(flags, 0)
				? MKC_formac_ForwardDel : MKC_Decimal,
				down);
			break;
		case VK_CAPITAL:
			CheckTheCapsLock();
			break;
		default:
			if ((i >= 0) && (i < 256)) {
				DoVKcode0(i, down);
			}
			break;
	}
}

LOCALVAR blnr WantCmdOptOnReconnect = falseblnr;

LOCALPROC ReconnectKeyCodes3(void)
{
	int i;

	CheckTheCapsLock();

	if (WantCmdOptOnReconnect) {
		WantCmdOptOnReconnect = falseblnr;

		for (i = 0; i < 256; ++i) {
			if ((GetKeyState(i) & 0x8000) != 0) {
				if ((VK_CAPITAL != i)
					&& (VK_RETURN != i))
				{
					DoVKcode0(i, trueblnr);
				}
			}
		}
	}
}

LOCALPROC DisconnectKeyCodes3(void)
{
	DisconnectKeyCodes2();
	SetCurMouseButton(falseblnr);
}

#if EnableGrabSpecialKeys
static HHOOK hKeyHook = NULL;
#endif

#if EnableGrabSpecialKeys
typedef struct {
	DWORD   vkCode;
	DWORD   scanCode;
	DWORD   flags;
	DWORD   time;
	DWORD   dwExtraInfo;
} My_KBDLLHOOKSTRUCT;
#endif

#if EnableGrabSpecialKeys
LRESULT CALLBACK LowLevelKeyboardProc(
	int nCode,     /* hook code */
	WPARAM wParam, /* message identifier */
	LPARAM lParam  /* pointer to structure with message data */
);
#endif

#if EnableGrabSpecialKeys
LRESULT CALLBACK LowLevelKeyboardProc(
	int nCode,     /* hook code */
	WPARAM wParam, /* message identifier */
	LPARAM lParam  /* pointer to structure with message data */
)
{
	if (nCode == HC_ACTION) {
		My_KBDLLHOOKSTRUCT *p = (My_KBDLLHOOKSTRUCT *)lParam;
		if (p->vkCode != VK_CAPITAL) {
			switch (wParam) {
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
					DoVKcode(p->vkCode, p->flags, trueblnr);
					return 1;
					break;
				case WM_KEYUP:
				case WM_SYSKEYUP:
					DoVKcode(p->vkCode, p->flags, falseblnr);
					return 1;
					break;
			}
		}
	}
	return CallNextHookEx(hKeyHook, /* handle to current hook */
		nCode, /* hook code passed to hook procedure */
		wParam, /* value passed to hook procedure */
		lParam /* value passed to hook procedure */
	);

}
#endif

#if EnableGrabSpecialKeys
#define My_WH_KEYBOARD_LL 13
#define My_SPI_SETSCREENSAVERRUNNING 0x0061
#endif

#if EnableGrabSpecialKeys
LOCALVAR UINT nPreviousState;
#endif

#if EnableGrabSpecialKeys
LOCALPROC GrabSpecialKeys(void)
{
	if ((hKeyHook == NULL) && ! HaveSetSysParam) {
		/* this works on Windows XP */
		hKeyHook = SetWindowsHookEx(
			My_WH_KEYBOARD_LL, /* type of hook to install */
			(HOOKPROC)LowLevelKeyboardProc,
				/* address of hook procedure */
			AppInstance,    /* handle to application instance */
			0   /* identity of thread to install hook for */
		);
		if (hKeyHook == NULL) {
			/* this works on Windows 95/98 */
			SystemParametersInfo(My_SPI_SETSCREENSAVERRUNNING, TRUE,
				&nPreviousState, 0);
			HaveSetSysParam = trueblnr;
		}
	}
}
#endif

#if EnableGrabSpecialKeys
LOCALPROC UnGrabSpecialKeys(void)
{
	if (hKeyHook != NULL) {
		(void) UnhookWindowsHookEx(hKeyHook);
		hKeyHook = NULL;
	}
	if (HaveSetSysParam) {
		SystemParametersInfo(My_SPI_SETSCREENSAVERRUNNING, FALSE,
			&nPreviousState, 0);
		HaveSetSysParam = falseblnr;
	}
}
#endif

/* --- priority --- */

#ifndef EnableChangePriority
#if UseWinCE
#define EnableChangePriority 0
#else
#define EnableChangePriority MayFullScreen
#endif
#endif /* EnableChangePriority */

#if EnableChangePriority
LOCALVAR blnr MyPriorityRaised = falseblnr;
#endif

#if EnableChangePriority
LOCALPROC RaiseMyPriority(void)
{
	if (! MyPriorityRaised) {
		if (! SetPriorityClass(
			GetCurrentProcess(), /* handle to the process */
			HIGH_PRIORITY_CLASS
				/* REALTIME_PRIORITY_CLASS (not, killer) */
				/* priority class value */
			))
		{
			/*
				not recursive:
				MacMsg("SetPriorityClass failed",
					"Sorry, Mini vMac encountered errors"
					" and cannot continue.", trueblnr);
			*/
		}
		MyPriorityRaised = trueblnr;
	}
}
#endif

#if EnableChangePriority
LOCALPROC LowerMyPriority(void)
{
	if (MyPriorityRaised) {
		if (! SetPriorityClass(
			GetCurrentProcess(),        /* handle to the process */
			NORMAL_PRIORITY_CLASS /* priority class value */
			))
		{
			/*
				not recursive:
				MacMsg("SetPriorityClass failed",
					"Sorry, Mini vMac encountered errors"
					" and cannot continue.", trueblnr);
			*/
		}
		MyPriorityRaised = falseblnr;
	}
}
#endif


/* --- time, date, location --- */

#define dbglog_TimeStuff (0 && dbglog_HAVE)

LOCALVAR ui5b TrueEmulatedTime = 0;

#define MyInvTimeDivPow 16
#define MyInvTimeDiv (1 << MyInvTimeDivPow)
#define MyInvTimeDivMask (MyInvTimeDiv - 1)
#define MyInvTimeStep 1089590 /* 1000 / 60.14742 * MyInvTimeDiv */

LOCALVAR DWORD LastTime;

LOCALVAR DWORD NextIntTime;
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

LOCALFUNC blnr UpdateTrueEmulatedTime(void)
{
	DWORD LatestTime;
	si5b TimeDiff;

	LatestTime = timeGetTime();
	if (LatestTime != LastTime) {
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
		} else if (TimeDiff < -256) {
			/* clock goofed if ever get here, reset */
#if dbglog_TimeStuff
			dbglog_writeln("clock set back");
#endif

			InitNextTime();
		}
	}
	return falseblnr;
}

LOCALVAR ui5b TimeSecBase;
LOCALVAR DWORD TimeMilliBase;

#include "DATE2SEC.h"

LOCALFUNC blnr CheckDateTime(void)
{
	ui5b NewMacDateInSecond;

	NewMacDateInSecond =
		((ui5b)(LastTime - TimeMilliBase)) / 1000 + TimeSecBase;
	if (CurMacDateInSeconds != NewMacDateInSecond) {
		CurMacDateInSeconds = NewMacDateInSecond;

		return trueblnr;
	} else {
		return falseblnr;
	}
}

LOCALFUNC blnr Init60thCheck(void)
{
	SYSTEMTIME s;
#if AutoTimeZone
	TIME_ZONE_INFORMATION r;
	DWORD v;
#endif
	DWORD t;

	GetLocalTime(&s);
	t = timeGetTime();
	TimeSecBase = Date2MacSeconds(s.wSecond, s.wMinute, s.wHour,
		s.wDay, s.wMonth, s.wYear);
	TimeMilliBase = t - s.wMilliseconds;

#if AutoTimeZone
	v = GetTimeZoneInformation(&r);
	if ((v != 0xFFFFFFFF) && (v != TIME_ZONE_ID_UNKNOWN)) {
		si5b dlsBias = (v != TIME_ZONE_ID_DAYLIGHT)
			? r.StandardBias : r.DaylightBias;
		CurMacDelta = (((ui5b)(- (r.Bias + dlsBias) * 60))
			& 0x00FFFFFF)
			| (((v != TIME_ZONE_ID_DAYLIGHT) ? 0 : 0x80)
				<< 24);
	}
#endif

	LastTime = timeGetTime();
	InitNextTime();

	OnTrueTime = TrueEmulatedTime;

	(void) CheckDateTime();

	return trueblnr;
}

#ifndef MyTimeResolution
#define MyTimeResolution 3
#endif
	/*
		Setting MyTimeResolution to 1 seems to drastically slow down
		the clock in Virtual PC 7.0.2 for Mac. Using 3 is more polite
		anyway, and should not cause much observable difference.
	*/

#if (MyTimeResolution != 0)
LOCALVAR blnr HaveSetTimeResolution = falseblnr;
#endif

#if (MyTimeResolution != 0)
LOCALPROC MyTimer_Suspend(void)
{
	if (HaveSetTimeResolution) {
		(void) timeEndPeriod(MyTimeResolution);
		HaveSetTimeResolution = falseblnr;
	}
}
#endif

#if (MyTimeResolution != 0)
LOCALPROC MyTimer_Resume(void)
{
	TIMECAPS tc;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS))
		== TIMERR_NOERROR)
	{
		if ((MyTimeResolution >= tc.wPeriodMin)
			&& (MyTimeResolution <= tc.wPeriodMax))
		{
			if (timeBeginPeriod(MyTimeResolution)
				== TIMERR_NOERROR)
			{
				HaveSetTimeResolution = trueblnr;
			}
		}
	}
}
#endif

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
LOCALVAR ui4b ThePlayOffset;
LOCALVAR ui4b TheFillOffset;
LOCALVAR blnr wantplaying;
LOCALVAR ui4b MinFilledSoundBuffs;
LOCALVAR ui4b TheWriteOffset;

#define SOUND_SAMPLERATE /* 22050 */ 22255
	/* = round(7833600 * 2 / 704) */


LOCALPROC FillWithSilence(tpSoundSamp p, int n, trSoundSamp v)
{
	int i;

	for (i = n; --i >= 0; ) {
		*p++ = v;
	}
}


LOCALVAR HWAVEOUT hWaveOut = NULL;
LOCALVAR WAVEHDR whdr[kSoundBuffers];


LOCALPROC MySound_BeginPlaying(void)
{
#if dbglog_SoundStuff
	fprintf(stderr, "MySound_BeginPlaying\n");
#endif
}

LOCALPROC MySound_Start(void)
{
	if (hWaveOut == NULL) {
		WAVEFORMATEX wfex;
		MMRESULT mmr;
		int i;
		tpSoundSamp p;
		WAVEHDR *pwh;

		wfex.wFormatTag = WAVE_FORMAT_PCM;
		wfex.nChannels = 1;
		wfex.nSamplesPerSec = SOUND_SAMPLERATE;
		wfex.nAvgBytesPerSec = SOUND_SAMPLERATE;
#if 3 == kLn2SoundSampSz
		wfex.nBlockAlign = 1;
		wfex.wBitsPerSample = 8;
#elif 4 == kLn2SoundSampSz
		wfex.nBlockAlign = 2;
		wfex.wBitsPerSample = 16;
#else
#error "unsupported audio format"
#endif
		wfex.cbSize = 0;
		mmr = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfex, 0,
			0 /* (DWORD) AppInstance */, CALLBACK_NULL);
		if (mmr != MMSYSERR_NOERROR) {
			/*
				not recursive:
				MacMsg("waveOutOpen failed",
					"Sorry, Mini vMac encountered errors"
					" and cannot continue.", trueblnr);
			*/
		} else {
			p = TheSoundBuffer;
			pwh = whdr;
			for (i = 0; i < kSoundBuffers; ++i) {
				pwh->lpData = (LPSTR)p;
				pwh->dwBufferLength = kOneBuffSz;
				pwh->dwBytesRecorded = 0;
				pwh->dwUser = 0;
				pwh->dwFlags = 0;
				pwh->dwLoops = 0;
				mmr = waveOutPrepareHeader(hWaveOut, pwh,
					sizeof(WAVEHDR));
				if (mmr != MMSYSERR_NOERROR) {
					/*
						not recursive:
						MacMsg("waveOutPrepareHeader failed",
							"Sorry, Mini vMac encountered errors"
							" and cannot continue.", trueblnr);
					*/
				} else {
					pwh->dwFlags |= WHDR_DONE;
				}
				p += kOneBuffLen;
				++pwh;
			}

			TheFillOffset = 0;
			ThePlayOffset = 0;
			TheWriteOffset = 0;
			MinFilledSoundBuffs = kSoundBuffers;
			wantplaying = falseblnr;
		}
	}
}

LOCALPROC MySound_Stop(void)
{
	MMRESULT mmr;
	int i;

	wantplaying = falseblnr;
	if (hWaveOut != NULL) {
		DWORD StartTime = GetTickCount();
		for (i = 0; i < kSoundBuffers; ++i) {
			while (((whdr[i].dwFlags & WHDR_DONE) == 0)
				&& ((ui5b)(GetTickCount() - StartTime) < 1000))
			{
				Sleep(1);
			}

			mmr = waveOutUnprepareHeader(hWaveOut, &whdr[i],
				sizeof(WAVEHDR));
			if (mmr != MMSYSERR_NOERROR) {
				/*
					not recursive:
					MacMsg("waveOutUnprepareHeader failed",
						"Sorry, Mini vMac encountered errors"
						" and cannot continue.", trueblnr);
				*/
			}
		}

		mmr = waveOutClose(hWaveOut);
		if (mmr != MMSYSERR_NOERROR) {
			/*
				MacMsg("waveOutClose failed",
					"Sorry, Mini vMac encountered errors"
					" and cannot continue.", trueblnr);
			*/
		}
		hWaveOut = NULL;
	}
}

LOCALPROC SoundCheckVeryOften(void)
{
	if ((hWaveOut != NULL) && (wantplaying)) {
label_retry:
		{
			ui4b FilledSoundBuffs;
			ui4b ToPlaySize = TheFillOffset - ThePlayOffset;
			ui4b CurPlayBuffer =
				(ThePlayOffset >> kLnOneBuffLen) & kSoundBuffMask;

			if ((ToPlaySize > kOneBuffLen)
				&& ((whdr[CurPlayBuffer].dwFlags & WHDR_DONE) != 0))
			{
				ThePlayOffset += kOneBuffLen;
				goto label_retry;
			}
			FilledSoundBuffs = ToPlaySize >> kLnOneBuffLen;

			if (FilledSoundBuffs < MinFilledSoundBuffs) {
				MinFilledSoundBuffs = FilledSoundBuffs;
			}

			if (FilledSoundBuffs < 2) {
				MMRESULT mmr;
				ui4b PrevPlayOffset = ThePlayOffset - kOneBuffLen;
				ui4b PrevPlayBuffer =
					(PrevPlayOffset >> kLnOneBuffLen) & kSoundBuffMask;
				ui4b LastPlayedOffset =
					((TheFillOffset >> kLnOneBuffLen) << kLnOneBuffLen)
						- 1;

				FillWithSilence(
					TheSoundBuffer + (PrevPlayOffset & kAllBuffMask),
					kOneBuffLen,
					*(TheSoundBuffer
						+ (LastPlayedOffset & kAllBuffMask)));
				mmr = waveOutWrite(
					hWaveOut, &whdr[PrevPlayBuffer], sizeof(WAVEHDR));
				if (mmr != MMSYSERR_NOERROR) {
					whdr[PrevPlayBuffer].dwFlags |= WHDR_DONE;
					/*
						not recursive:
						MacMsg("waveOutWrite failed",
							"Sorry, Mini vMac encountered errors"
							" and cannot continue.", trueblnr);
					*/
				}
				ThePlayOffset = PrevPlayOffset;
				goto label_retry;
			}
		}
	}
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

LOCALPROC MySound_FilledBlocks(void)
{
	while (0 != ((TheWriteOffset - TheFillOffset) >> kLnOneBuffLen)) {
		ui4b CurFillBuffer =
			(TheFillOffset >> kLnOneBuffLen) & kSoundBuffMask;
		blnr IsOk = falseblnr;

		ConvertSoundBlockToNative((tpSoundSamp)
			whdr[CurFillBuffer].lpData);

		if (hWaveOut != NULL) {
			MMRESULT mmr = waveOutWrite(hWaveOut,
				&whdr[CurFillBuffer], sizeof(WAVEHDR));
			if (mmr == MMSYSERR_NOERROR) {
				IsOk = trueblnr;
			}
		}

		if (! IsOk) {
			/*
				not recursive:
				MacMsg("waveOutWrite failed",
					"Sorry, Mini vMac encountered errors"
					" and cannot continue.", trueblnr);
			*/
			whdr[CurFillBuffer].dwFlags |= WHDR_DONE;
		}

		TheFillOffset += kOneBuffLen;
	}
}

LOCALPROC MySound_WroteABlock(void)
{
	if (wantplaying) {
		MySound_FilledBlocks();
	} else if (((TheWriteOffset - ThePlayOffset) >> kLnOneBuffLen) < 12)
	{
		/* just wait */
	} else {
		MySound_FilledBlocks();
		wantplaying = trueblnr;
		MySound_BeginPlaying();
	}
}

GLOBALOSGLUPROC MySound_EndWrite(ui4r actL)
{
	TheWriteOffset += actL;

	if (0 == (TheWriteOffset & kOneBuffMask)) {
		/* just finished a block */

		MySound_WroteABlock();
	}
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
		TheWriteOffset -= kOneBuffLen;
	}

	*actL = n;
	return TheSoundBuffer + (TheWriteOffset & kAllBuffMask);
}

LOCALPROC MySound_SecondNotify(void)
{
	if (hWaveOut != NULL) {
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
		MinFilledSoundBuffs = kSoundBuffers;
	}
}

#endif

/* --- overall grab --- */

#if MayFullScreen
LOCALPROC GrabTheMachine(void)
{
#if EnableFSMouseMotion
	StartSaveMouseMotion();
#endif
#if EnableChangePriority
	if ((ui3b) -1 == SpeedValue) {
		RaiseMyPriority();
	}
#endif
#if EnableGrabSpecialKeys
	GrabSpecialKeys();
#endif
}
#endif

#if MayFullScreen
LOCALPROC UnGrabTheMachine(void)
{
#if EnableGrabSpecialKeys
	UnGrabSpecialKeys();
#endif
#if EnableFSMouseMotion
	StopSaveMouseMotion();
#endif
#if EnableChangePriority
	LowerMyPriority();
#endif
}
#endif

#if MayFullScreen
LOCALVAR blnr GrabMachine = falseblnr;
#endif

#if MayFullScreen
LOCALPROC AdjustMachineGrab(void)
{
	if (GrabMachine) {
		if (MainWnd != NULL) {
			GrabTheMachine();
		}
	} else {
		UnGrabTheMachine();
	}
}
#endif

/* --- basic dialogs --- */

LOCALPROC MyBeginDialog(void)
{
	DisconnectKeyCodes3();
#if MayFullScreen
	GrabMachine = falseblnr;
	UnGrabTheMachine();
#endif
	ForceShowCursor();
}

LOCALPROC MyEndDialog(void)
{
	ReconnectKeyCodes3();
}

LOCALPROC CheckSavedMacMsg(void)
{
	if (nullpr != SavedBriefMsg) {
		TCHAR briefMsg0[ClStrMaxLength + 1];
		TCHAR longMsg0[ClStrMaxLength + 1];

		NativeStrFromCStr(briefMsg0, SavedBriefMsg, falseblnr);
		NativeStrFromCStr(longMsg0, SavedLongMsg, falseblnr);

		MessageBox(MainWnd, longMsg0, briefMsg0,
			MB_APPLMODAL | MB_OK | (SavedFatalMsg ? MB_ICONSTOP : 0));

		SavedBriefMsg = nullpr;
	}
}

/* --- main window --- */

enum {
	ID_MENU_NULL = 256,
	ID_FILE_INSERTDISK1,
	ID_FILE_QUIT,
	ID_SPECIAL_MORECOMMANDS,
	ID_HELP_ABOUT,

	kNum_ID_MENU
};


#if (1 == vMacScreenDepth) || (vMacScreenDepth >= 4)
#define EnableScalingBuff 1
#else
#define EnableScalingBuff (1 && EnableMagnify && (MyWindowScale == 2))
#endif

#if EnableScalingBuff
LOCALVAR ui3p ScalingBuff = NULL;
#endif

LOCALVAR HDC MainWndDC = NULL;

LOCALVAR si5b CmdShow;

LOCALVAR TCHAR WndTitle[_MAX_PATH];
LOCALVAR const TCHAR WndClassName[] = TEXT("minivmac");

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALPROC GetWndTitle(void)
{
	TCHAR pathName[_MAX_PATH];
	WIN32_FIND_DATA fd;
	blnr IsOk = falseblnr;

	if (GetModuleFileName(AppInstance, pathName, _MAX_PATH) != 0) {
		HANDLE hf = FindFirstFile(pathName, &fd);

		if (hf != INVALID_HANDLE_VALUE) {
			/* get rid of extension, presumably '.exe' */
			LPTSTR p = FindLastTerm(fd.cFileName,
				(TCHAR)('.'));
			if (p != nullpr) {
				*--p = (TCHAR)('\0');
			}

			_tcscpy(WndTitle, fd.cFileName);
			IsOk = trueblnr;
			FindClose(hf);
		}
	}
	if (! IsOk) {
		_tcscpy(WndTitle, TEXT(kStrAppName));
	}
}

LOCALPROC DisposeMainWindow(void)
{
#if UseWinCE
	/* Show the taskbar */
	SHFullScreen(MainWnd, SHFS_SHOWTASKBAR);
#endif

	if (MainWndDC != NULL) {
		ReleaseDC(MainWnd, MainWndDC);
	}
	if (MainWnd != NULL) {
		DestroyWindow(MainWnd);
		MainWnd = NULL; /* so MacMsg will still work */
	}
}

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
LOCALVAR POINT WinPositionWins[kNumMagStates];
#endif

#if MayNotFullScreen
LOCALPROC MyAppendConvertMenuItem(HMENU hMenu,
	UINT uIDNewItem, char *s, blnr AddEllipsis)
{
	TCHAR ts[ClStrMaxLength + 1];

	NativeStrFromCStr(ts, s, AddEllipsis);

	(void) AppendMenu(hMenu, MF_ENABLED + MF_STRING,
		uIDNewItem, ts);
}
#endif

#if MayNotFullScreen
LOCALPROC MyAppendSubmenuConvertName(HMENU hMenu,
	HMENU hSubMenu, char *s)
{
	TCHAR ts[ClStrMaxLength + 1];
	MENUITEMINFO mii;

	NativeStrFromCStr(ts, s, falseblnr);

#if 0
	(void) InsertMenu(hMenu, 0xFFFFFFFF,
		MF_BYPOSITION + MF_POPUP + MF_STRING + MF_ENABLED,
		(UINT)hSubMenu, ts);
#endif

	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_TYPE | MIIM_SUBMENU;
	mii.fType = MFT_STRING;
	mii.hSubMenu = hSubMenu;
	mii.dwTypeData = ts;
	mii.cch = (UINT)_tcslen(ts);
	(void) InsertMenuItem(hMenu, (UINT) -1, TRUE,
		&mii);
}
#endif

#ifndef kStrMenuFile_win
#define kStrMenuFile_win kStrMenuFile
#endif

LOCALFUNC blnr ReCreateMainWindow(void)
{
#if MayNotFullScreen
	HMENU m;
	int DfltWndX;
	int DfltWndY;
	int WinIndx;
#endif
	HMENU mb;
	HWND NewMainWindow;
	HDC NewMainWndDC = NULL;
	int ScreenX = GetSystemMetrics(SM_CXSCREEN);
	int ScreenY = GetSystemMetrics(SM_CYSCREEN);
	short NewWindowHeight = vMacScreenHeight;
	short NewWindowWidth = vMacScreenWidth;
	HWND OldMainWindow = MainWnd;
	HDC OldMainWndDC = MainWndDC;
	RECT NewWinR;
	DWORD MyWStyle;
	DWORD MyWExStyle;

#if VarFullScreen
	if (! UseFullScreen)
#endif
#if MayNotFullScreen
	{
		/* save old position */
		if (OldMainWindow != NULL) {
			WinPositionWins[CurWinIndx].x = WndX;
			WinPositionWins[CurWinIndx].y = WndY;
		}
	}
#endif

#if MayNotFullScreen
#if EnableMagnify
	if (WantMagnify) {
		WinIndx = kMagStateMagnifgy;
	} else
#endif
	{
		WinIndx = kMagStateNormal;
	}
#endif

#if EnableMagnify
	if (WantMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (WantFullScreen)
#endif
#if MayFullScreen
	{
		MyWStyle = WS_VISIBLE | WS_POPUP;
		MyWExStyle = WS_EX_TOPMOST;

		hOffset = (ScreenX - NewWindowWidth) / 2;
		vOffset = (ScreenY - NewWindowHeight) / 2;
		if (hOffset < 0) {
			hOffset = 0;
		}
		if (vOffset < 0) {
			vOffset = 0;
		}

		NewWinR.left = 0;
		NewWinR.top = 0;
		NewWinR.right = ScreenX;
		NewWinR.bottom = ScreenY;
	}
#endif
#if VarFullScreen
	else
#endif
#if MayNotFullScreen
	{
		MyWStyle = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
			| WS_MINIMIZEBOX;
		MyWExStyle = WS_EX_ACCEPTFILES;

		DfltWndX = (ScreenX - NewWindowWidth) / 2;
		DfltWndY = (ScreenY - NewWindowHeight) / 2;

		if (DfltWndX < 0) {
			DfltWndX = 0;
		}
		if (DfltWndY < 0) {
			DfltWndY = 0;
		}

		if (! HavePositionWins[WinIndx]) {
			WinPositionWins[WinIndx].x = DfltWndX;
			WinPositionWins[WinIndx].y = DfltWndY;
			HavePositionWins[WinIndx] = trueblnr;
		}

		NewWinR.left = WinPositionWins[WinIndx].x;
		NewWinR.top = WinPositionWins[WinIndx].y;
		NewWinR.right = NewWinR.left + NewWindowWidth;
		NewWinR.bottom = NewWinR.top + NewWindowHeight;

		(void) AdjustWindowRectEx(&NewWinR, MyWStyle, TRUE, MyWExStyle);

		if ((NewWinR.right <= 0)
			|| (NewWinR.left >= ScreenX)
			|| (NewWinR.bottom <= 0)
			|| (NewWinR.top >= ScreenY))
		{
			NewWinR.left = DfltWndX;
			NewWinR.top = DfltWndY;
			NewWinR.right = DfltWndX + NewWindowWidth;
			NewWinR.bottom = DfltWndY + NewWindowHeight;

			(void) AdjustWindowRectEx(&NewWinR,
				MyWStyle, TRUE, MyWExStyle);
		}
	}
#endif

	if ((OldMainWindow == NULL)
#if VarFullScreen
		|| (WantFullScreen != UseFullScreen)
#endif
		)
	{

#if VarFullScreen
		if (WantFullScreen)
#endif
#if MayFullScreen
		{
			mb = NULL;
		}
#endif
#if VarFullScreen
		else
#endif
#if MayNotFullScreen
		{
			mb = CreateMenu();
			if (mb != NULL) {
				m = CreateMenu();
				if (m != NULL) {
					MyAppendConvertMenuItem(m, ID_FILE_INSERTDISK1,
						kStrMenuItemOpen, trueblnr);
					(void) AppendMenu(m, MF_SEPARATOR, 0, NULL);
					MyAppendConvertMenuItem(m, ID_FILE_QUIT,
						kStrMenuItemQuit, falseblnr);
					MyAppendSubmenuConvertName(mb, m, kStrMenuFile_win);
				}
				m = CreateMenu();
				if (m != NULL) {
					MyAppendConvertMenuItem(m, ID_SPECIAL_MORECOMMANDS,
						kStrMenuItemMore, trueblnr);
					MyAppendSubmenuConvertName(mb, m, kStrMenuSpecial);
				}
				m = CreateMenu();
				if (m != NULL) {
					MyAppendConvertMenuItem(m, ID_HELP_ABOUT,
						kStrMenuItemAbout, trueblnr);
					MyAppendSubmenuConvertName(mb, m, kStrMenuHelp);
				}
			}
		}
#endif

		NewMainWindow = CreateWindowEx(
			MyWExStyle,
			WndClassName,
			WndTitle,
			MyWStyle,
			NewWinR.left, NewWinR.top,
			NewWinR.right - NewWinR.left, NewWinR.bottom - NewWinR.top,
			NULL,
			mb,
			AppInstance, NULL);
		if (NewMainWindow == NULL) {
			MacMsg("CreateWindow failed",
				"Sorry, Mini vMac encountered errors"
				" and cannot continue.", trueblnr);
			return falseblnr;
		}

		NewMainWndDC = GetDC(NewMainWindow);
		if (NewMainWndDC == NULL) {
			MacMsg("GetDC failed",
				"Sorry, Mini vMac encountered errors"
				" and cannot continue.", trueblnr);
			DestroyWindow(NewMainWindow);
			return falseblnr;
		}
	} else {
		NewMainWndDC = OldMainWndDC;
		NewMainWindow = OldMainWindow;
		(void) MoveWindow(NewMainWindow, NewWinR.left, NewWinR.top,
			NewWinR.right - NewWinR.left, NewWinR.bottom - NewWinR.top,
			TRUE);
	}

#if 0 != vMacScreenDepth
	ColorModeWorks = trueblnr;
#endif

	{
		POINT p;

		/*
			Find out where the window really went, on
			the off chance that the WM_MOVE message wasn't
			called on CreateWindowEx/MoveWindow, or that
			the window wasn't put where asked for.
		*/
		p.x = 0;
		p.y = 0;
		(void) MapWindowPoints(NewMainWindow, NULL, &p, 1);
		WndX = (si4b)p.x;
		WndY = (si4b)p.y;
	}

#if MayFullScreen
	GrabMachine = falseblnr;
	UnGrabTheMachine();
#endif

#if UseWinCE && 0
	/* Show the taskbar */
	SHFullScreen(MainWnd, SHFS_SHOWTASKBAR);
#endif

#if MayNotFullScreen
	CurWinIndx = WinIndx;
#endif

	MainWnd = NewMainWindow;
	MainWndDC = NewMainWndDC;
	gTrueBackgroundFlag = falseblnr;
#if VarFullScreen
	UseFullScreen = WantFullScreen;
#endif
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		ViewHSize = ScreenX;
		ViewVSize = ScreenY;
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
	}
#endif

	if (NewMainWindow != OldMainWindow) {
		ShowWindow(NewMainWindow, SW_SHOW /* CmdShow */);
		if (OldMainWndDC != NULL) {
			ReleaseDC(MainWnd, OldMainWndDC);
		}
		if (OldMainWindow != NULL) {
			/* ShowWindow(OldMainWindow, SW_HIDE); */
			DestroyWindow(OldMainWindow);
		}

		DisconnectKeyCodes3();
			/* since key events per window */
	} else {
		(void) InvalidateRgn(MainWnd, NULL, FALSE);
	}

#if UseWinCE
	/* Create and set logical palette for this window */
	{
		HPALETTE hpal;
		LOGPALETTE *lppal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +
			sizeof(PALETTEENTRY) * 2);

		if (! lppal)
		{
			MacMsg("CreateWindow failed",
				"Sorry, Mini vMac encountered errors"
				" and cannot continue.", trueblnr);
			return falseblnr;
		}

		lppal->palNumEntries = 2;
		lppal->palVersion = 0x0300;
		lppal->palPalEntry[0].peRed   = 255;
		lppal->palPalEntry[0].peGreen = 255;
		lppal->palPalEntry[0].peBlue  = 255;
		lppal->palPalEntry[0].peFlags = 0;
		lppal->palPalEntry[1].peRed   = 0;
		lppal->palPalEntry[1].peGreen = 0;
		lppal->palPalEntry[1].peBlue  = 0;
		lppal->palPalEntry[1].peFlags = 0;

		hpal = CreatePalette(lppal);

		if (hpal == NULL) {
			free(lppal);
			MacMsg("CreateWindow failed",
				"Sorry, Mini vMac encountered errors"
				" and cannot continue.", trueblnr);
			return falseblnr;
		}

		if (SelectPalette(MainWndDC, hpal, FALSE) == NULL) {
			free(lppal);
			MacMsg("CreateWindow failed",
				"Sorry, Mini vMac encountered errors"
				" and cannot continue.", trueblnr);
			return falseblnr;
		}

		if (RealizePalette(MainWndDC) == GDI_ERROR) {
			free(lppal);
			MacMsg("CreateWindow failed",
				"Sorry, Mini vMac encountered errors"
				" and cannot continue.", trueblnr);
			return falseblnr;
		}

		free(lppal);
	}
#endif

#if UseWinCE
	/* Hide the taskbar */
	SHFullScreen(MainWnd, SHFS_HIDETASKBAR);
	(void) MoveWindow(MainWnd, 0, 0,
			ScreenX, ScreenY, TRUE);
#endif

	if (HaveCursorHidden) {
		(void) MyMoveMouse(CurMouseH, CurMouseV);
		WantCursorHidden = trueblnr;
	}

	return trueblnr;
}

#if UseWinCE
LOCALFUNC blnr AlreadyRunningCheck(void)
{
	/*
		Adapted from example program from Microsoft eMbedded Visual C++
	*/

	/* If it is already running, then focus on the window */
	HWND hWnd = FindWindow(WndClassName, WndTitle);
	if (hWnd == NULL) {
		return falseblnr;
	} else {
		/*
			Set focus to foremost child window.
			The "| 0x01" is used to bring any owned
			windows to the foreground and activate them.
		*/
		SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
		return trueblnr;
	}
}
#endif

typedef struct BITMAPINFOHEADER256 {
	BITMAPINFOHEADER bmi;
#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)
	RGBQUAD colors[CLUT_size];
#else
	RGBQUAD colors[2];
#endif
} BITMAPINFOHEADER256;

#if EnableMagnify
#define MyScaledHeight (MyWindowScale * vMacScreenHeight)
#define MyScaledWidth (MyWindowScale * vMacScreenWidth)
#endif

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left,
	si4b bottom, si4b right)
{
	BITMAPINFOHEADER256 bmh;
	ui3b *cdb = GetCurDrawBuff();
	int XDest;
	int YDest;

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

#if 0
	{ /* testing code */
		if (PatBlt(MainWndDC,
			(int)left - 1,
			(int)top - 1,
			(int)right - left + 2,
			(int)bottom - top + 2, PATCOPY)) {
		}
	}
#endif
#if 0 != vMacScreenDepth
	if (UseColorMode) {
		int i;
		int nDestWidth = (right - left);
		int nDestHeight = (bottom - top);
#if 1 == vMacScreenDepth
		ui3b *p
			= ScalingBuff + ((ui5r)vMacScreenWidth / 4) * top;
#elif vMacScreenDepth >= 4
		ui3b *p = ScalingBuff + (ui5r)vMacScreenByteWidth * top;
#else
		ui3b *p = cdb + (ui5r)vMacScreenByteWidth * top;
#endif

		memset(&bmh, 0, sizeof (bmh));
		bmh.bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmh.bmi.biWidth = vMacScreenWidth;
		bmh.bmi.biHeight = - (bottom - top);
		bmh.bmi.biPlanes = 1;
#if 1 == vMacScreenDepth
		bmh.bmi.biBitCount = 4;
#else
		bmh.bmi.biBitCount = (1 << vMacScreenDepth);
#endif
		bmh.bmi.biCompression= BI_RGB;
		bmh.bmi.biSizeImage = 0;
		bmh.bmi.biXPelsPerMeter = 0;
		bmh.bmi.biYPelsPerMeter = 0;
#if 1 == vMacScreenDepth
		bmh.bmi.biClrUsed = 4;
#else
		bmh.bmi.biClrUsed = 0;
#endif
		bmh.bmi.biClrImportant = 0;

#if vMacScreenDepth < 4
		for (i = 0; i < CLUT_size; ++i) {
			bmh.colors[i].rgbRed = CLUT_reds[i] >> 8;
			bmh.colors[i].rgbGreen = CLUT_greens[i] >> 8;
			bmh.colors[i].rgbBlue = CLUT_blues[i] >> 8;
			bmh.colors[i].rgbReserved = 0;
		}
#endif

#if 1 == vMacScreenDepth
		{
			int j;
			ui3b *p1 = (ui3b *)(cdb + (ui5r)vMacScreenByteWidth * top);
			ui4b *p2 = (ui4b *)p;
			for (i = bottom - top; --i >= 0; ) {
				for (j = vMacScreenWidth / 4; --j >= 0; ) {
					ui4r t0 = *p1++;
					*p2 ++
						= ((t0 & 0xC0) >> 2)
						| ((t0 & 0x30) >> 4)
						| ((t0 & 0x0C) << 10)
						| ((t0 & 0x03) << 8);
				}
			}
		}
#elif 4 == vMacScreenDepth
		{
			int j;
			ui4b *p1 = (ui4b *)(cdb + (ui5r)vMacScreenByteWidth * top);
			ui4b *p2 = (ui4b *)p;
			for (i = bottom - top; --i >= 0; ) {
				for (j = vMacScreenWidth; --j >= 0; ) {
					ui4r t0 = *p1++;
					*p2 ++ =
						((t0 & 0xFF00) >> 8) | ((t0 & 0x00FF) << 8);
				}
			}
		}
#elif 5 == vMacScreenDepth
		{
			int j;
			ui5b *p1 = (ui5b *)(cdb + (ui5r)vMacScreenByteWidth * top);
			ui5b *p2 = (ui5b *)p;
			for (i = bottom - top; --i >= 0; ) {
				for (j = vMacScreenWidth; --j >= 0; ) {
					ui5r t0 = *p1++;
					*p2++
						= ((t0 & 0xFF000000) >> 24)
						| ((t0 & 0x00FF0000) >> 8)
						| ((t0 & 0x0000FF00) << 8)
						| ((t0 & 0x000000FF) << 24);
				}
			}
		}
#endif

#if EnableMagnify
		if (UseMagnify) {
			nDestWidth *= MyWindowScale;
			nDestHeight *= MyWindowScale;
		}
#endif

		if (StretchDIBits(
			MainWndDC, /* handle of device context */
			XDest,
				/* x-coordinate of upper-left corner of dest. rect. */
			YDest,
				/* y-coordinate of upper-left corner of dest. rect. */
			nDestWidth, /* dest. rectangle width */
			nDestHeight, /* dest. rectangle height */
			left,
				/* x-coordinate of lower-left corner of source rect. */
			0, /* y-coordinate of lower-left corner of source rect. */
			(right - left), /* source rectangle width */
			(bottom - top), /* source rectangle height */
			(CONST VOID *)p, /* address of array with DIB bits */
			(const struct tagBITMAPINFO *)&bmh,
				/* address of structure with bitmap info. */
			DIB_RGB_COLORS, /* RGB or palette indices */
			SRCCOPY
		) == 0) {
			/* ReportWinLastError(); */
		}
	} else
#endif
	{
		ui3b *p = cdb + (ui5r)vMacScreenMonoByteWidth * top;

		memset(&bmh, 0, sizeof (bmh));
		bmh.bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmh.bmi.biWidth = vMacScreenWidth;
		bmh.bmi.biHeight = - (bottom - top);
		bmh.bmi.biPlanes = 1;
		bmh.bmi.biBitCount = 1;
		bmh.bmi.biCompression= BI_RGB;
		bmh.bmi.biSizeImage = 0;
		bmh.bmi.biXPelsPerMeter = 0;
		bmh.bmi.biYPelsPerMeter = 0;
		bmh.bmi.biClrUsed = 0;
		bmh.bmi.biClrImportant = 0;
#if ! UseWinCE
		bmh.colors[0].rgbRed = 255;
		bmh.colors[0].rgbGreen = 255;
		bmh.colors[0].rgbBlue = 255;
		bmh.colors[0].rgbReserved = 0;
		bmh.colors[1].rgbRed = 0;
		bmh.colors[1].rgbGreen = 0;
		bmh.colors[1].rgbBlue = 0;
		bmh.colors[1].rgbReserved = 0;
#endif

#if EnableMagnify
		if (UseMagnify) {
#if EnableScalingBuff
			if (ScalingBuff != NULL) {
				int i;
				int j;
				int k;
				ui4r left1 = left & (~ 7);
				ui4r right1 = (right + 7) & (~ 7);
				ui4r jn = (right1 - left1) / 8;
				ui3b *p1 =
					cdb + ((left1 + vMacScreenWidth * (ui5r)top) / 8);
				ui3b *p2 = ScalingBuff
					/*
						+ ((left1 + vMacScreenWidth * MyWindowScale
								* (ui5r)top)
							* MyWindowScale / 8)
					*/
					;
				ui3b *p3;
				ui3b t0;
				ui3b t1;
				ui3b t2;
				ui3b m;

				for (i = bottom - top; --i >= 0; ) {
					p3 = p2;
					for (j = jn; --j >= 0; ) {
						t0 = *p1++;
						t1 = t0;
						m = 0x80;
						t2 = 0;
						for (k = 4; --k >= 0; ) {
							t2 |= t1 & m;
							t1 >>= 1;
							m >>= 2;
						}
						*p2++ = t2 | (t2 >> 1);

						t1 = t0 << 4;
						m = 0x80;
						t2 = 0;
						for (k = 4; --k >= 0; ) {
							t2 |= t1 & m;
							t1 >>= 1;
							m >>= 2;
						}
						*p2++ = t2 | (t2 >> 1);
					}
					p1 += vMacScreenWidth / 8 - jn;
					p2 += MyScaledWidth / 8 - (MyWindowScale * jn);
					for (j = MyWindowScale * jn; --j >= 0; ) {
						*p2++ = *p3++;
					}
					p2 += MyScaledWidth / 8 - (MyWindowScale * jn);
				}

				bmh.bmi.biWidth = vMacScreenWidth * MyWindowScale;
				bmh.bmi.biHeight = - ((bottom - top) * MyWindowScale);
				if (SetDIBitsToDevice(
					MainWndDC, /* handle of device context */
					XDest,
						/*
							x-coordinate of upper-left corner
							of dest. rect.
						*/
					YDest,
						/*
							y-coordinate of upper-left corner
							of dest. rect.
						*/
					(right - left) * MyWindowScale,
						/* source rectangle width */
					(bottom - top) * MyWindowScale,
						/* source rectangle height */
					(left & 7) * MyWindowScale,
						/*
							x-coordinate of lower-left corner
							of source rect.
						*/
					0,
						/*
							y-coordinate of lower-left corner
							of source rect.
						*/
					0, /* first scan line in array */
					(bottom - top) * MyWindowScale,
						/* number of scan lines */
					(CONST VOID *)ScalingBuff,
						/* address of array with DIB bits */
					(const struct tagBITMAPINFO *)&bmh,
						/* address of structure with bitmap info. */
#if ! UseWinCE
					DIB_RGB_COLORS /* RGB or palette indices */
#else
					DIB_PAL_COLORS /* palette indices */
#endif
				) == 0) {
					/* ReportWinLastError(); */
				}
			}
#else
			if (StretchDIBits(
				MainWndDC, /* handle of device context */
				XDest,
					/*
						x-coordinate of upper-left corner of dest. rect.
					*/
				YDest,
					/*
						y-coordinate of upper-left corner of dest. rect.
					*/
				(right - left) * MyWindowScale,
					/* dest. rectangle width */
				(bottom - top) * MyWindowScale,
					/* dest. rectangle height */
				left,
					/*
						x-coordinate of lower-left corner
						of source rect.
					*/
				0,
					/*
						y-coordinate of lower-left corner
						of source rect.
					*/
				(right - left), /* source rectangle width */
				(bottom - top), /* source rectangle height */
				(CONST VOID *)p, /* address of array with DIB bits */
				(const struct tagBITMAPINFO *)&bmh,
					/* address of structure with bitmap info. */
#if ! UseWinCE
				DIB_RGB_COLORS, /* RGB or palette indices */
#else
				DIB_PAL_COLORS, /* palette indices */
#endif
				SRCCOPY
			) == 0) {
				/* ReportWinLastError(); */
			}
#endif
		} else
#endif

		{
			if (SetDIBitsToDevice(
				MainWndDC, /* handle of device context */
				XDest,
					/*
						x-coordinate of upper-left corner of dest. rect.
					*/
				YDest,
					/*
						y-coordinate of upper-left corner of dest. rect.
					*/
				(right - left), /* source rectangle width */
				(bottom - top), /* source rectangle height */
				left,
					/*
						x-coordinate of lower-left corner
						of source rect.
					*/
				0,
					/*
						y-coordinate of lower-left corner
						of source rect.
					*/
				0, /* first scan line in array */
				(bottom - top), /* number of scan lines */
				(CONST VOID *)p, /* address of array with DIB bits */
				(const struct tagBITMAPINFO *)&bmh,
					/* address of structure with bitmap info. */
#if ! UseWinCE
				DIB_RGB_COLORS /* RGB or palette indices */
#else
				DIB_PAL_COLORS /* palette indices */
#endif
			) == 0) {
				/* ReportWinLastError(); */
			}
		}
	}

#if MayFullScreen
label_exit:
	;
#endif
}

LOCALPROC Screen_DrawAll(void)
{
	HaveChangedScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
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

LOCALFUNC blnr InitTheCursor(void)
{
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return trueblnr;
}

#if EnableFSMouseMotion
LOCALPROC MyMouseConstrain(void)
{
	si4b shiftdh;
	si4b shiftdv;

	if (SavedMouseH < ViewHStart + (ViewHSize / 4)) {
		shiftdh = ViewHSize / 2;
	} else if (SavedMouseH > ViewHStart + ViewHSize - (ViewHSize / 4))
	{
		shiftdh = - ViewHSize / 2;
	} else {
		shiftdh = 0;
	}
	if (SavedMouseV < ViewVStart + (ViewVSize / 4)) {
		shiftdv = ViewVSize / 2;
	} else if (SavedMouseV > ViewVStart + ViewVSize - (ViewVSize / 4))
	{
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

LOCALPROC MousePositionNotify(LONG NewMousePosx, LONG NewMousePosy)
{
	blnr ShouldHaveCursorHidden = trueblnr;

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewMousePosx -= hOffset;
		NewMousePosy -= vOffset;
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		NewMousePosx /= MyWindowScale;
		NewMousePosy /= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewMousePosx += ViewHStart;
		NewMousePosy += ViewVStart;
	}
#endif

#if EnableFSMouseMotion
	if (HaveMouseMotion) {
		MyMousePositionSetDelta(NewMousePosx - SavedMouseH,
			NewMousePosy - SavedMouseV);
		SavedMouseH = NewMousePosx;
		SavedMouseV = NewMousePosy;
	} else
#endif
	{
		if (NewMousePosx < 0) {
			NewMousePosx = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePosx > vMacScreenWidth) {
			NewMousePosx = vMacScreenWidth - 1;
			ShouldHaveCursorHidden = falseblnr;
		}
		if (NewMousePosy < 0) {
			NewMousePosy = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePosy > vMacScreenHeight) {
			NewMousePosy = vMacScreenHeight - 1;
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

#if ! UseWinCE
		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/*
			for a game like arkanoid, would like mouse to still
			move even when outside window in one direction
		*/
#endif
		MyMousePositionSet(NewMousePosx, NewMousePosy);
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

#if ! UseWinCE
LOCALPROC CheckMouseState(void)
{
	POINT NewMousePos;

	GetCursorPos(&NewMousePos);
	NewMousePos.x -= WndX;
	NewMousePos.y -= WndY;
	MousePositionNotify(NewMousePos.x, NewMousePos.y);
}
#endif

LOCALVAR const ui3b Native2MacRomanTab[] = {
	0xAD, 0xB0, 0xE2, 0xC4, 0xE3, 0xC9, 0xA0, 0xE0,
	0xF6, 0xE4, 0xB6, 0xDC, 0xCE, 0xB2, 0xB3, 0xB7,
	0xB8, 0xD4, 0xD5, 0xD2, 0xD3, 0xA5, 0xD0, 0xD1,
	0xF7, 0xAA, 0xC5, 0xDD, 0xCF, 0xB9, 0xC3, 0xD9,
	0xCA, 0xC1, 0xA2, 0xA3, 0xDB, 0xB4, 0xBA, 0xA4,
	0xAC, 0xA9, 0xBB, 0xC7, 0xC2, 0xBD, 0xA8, 0xF8,
	0xA1, 0xB1, 0xC6, 0xD7, 0xAB, 0xB5, 0xA6, 0xE1,
	0xFC, 0xDA, 0xBC, 0xC8, 0xDE, 0xDF, 0xF0, 0xC0,
	0xCB, 0xE7, 0xE5, 0xCC, 0x80, 0x81, 0xAE, 0x82,
	0xE9, 0x83, 0xE6, 0xE8, 0xED, 0xEA, 0xEB, 0xEC,
	0xF5, 0x84, 0xF1, 0xEE, 0xEF, 0xCD, 0x85, 0xF9,
	0xAF, 0xF4, 0xF2, 0xF3, 0x86, 0xFA, 0xFB, 0xA7,
	0x88, 0x87, 0x89, 0x8B, 0x8A, 0x8C, 0xBE, 0x8D,
	0x8F, 0x8E, 0x90, 0x91, 0x93, 0x92, 0x94, 0x95,
	0xFD, 0x96, 0x98, 0x97, 0x99, 0x9B, 0x9A, 0xD6,
	0xBF, 0x9D, 0x9C, 0x9E, 0x9F, 0xFE, 0xFF, 0xD8
};

#if IncludePbufs
LOCALFUNC tMacErr NativeTextToMacRomanPbuf(HGLOBAL x, tPbuf *r)
{
#if MyUseUni
#define MyUnsignedChar ui4b
#else
#define MyUnsignedChar ui3b
#endif
	HGLOBAL h;
	LPTSTR p1;
	ui5b n;
	MyUnsignedChar v;
	tMacErr err = mnvm_miscErr;

	p1 = GlobalLock(x);
	if (p1 != NULL) {
		n = 0;
		while ((v = *p1++) != 0) {
			if (v != 10) {
				++n;
			}
		}
		(void) GlobalUnlock(x);

		h = GlobalAlloc(GMEM_DDESHARE, n);
		if (h != NULL) {
			p1 = GlobalLock(x);
			if (p1 != NULL) {
				ui3b *p2 = GlobalLock(h);
				if (p2 != NULL) {
					while ((v = (MyUnsignedChar)*p1++) != 0) {
						if (v >= 128) {
							*p2++ = Native2MacRomanTab[v & 0x7F];
								/*
									if MyUseUni, then for gives
									garbage for v > 256.
								*/
						} else if (v != 10) {
							*p2++ = v;
						}
					}

					err = mnvm_noErr;

					(void) GlobalUnlock(h);
				}
				(void) GlobalUnlock(x);
			}

			if (mnvm_noErr != err) {
				(void) GlobalFree(h);
			} else {
				err = PbufNewFromHandle(h, n, r);
			}
		}
	}

	return err;
}
#endif

LOCALVAR const ui3b MacRoman2NativeTab[] = {
	0xC4, 0xC5, 0xC7, 0xC9, 0xD1, 0xD6, 0xDC, 0xE1,
	0xE0, 0xE2, 0xE4, 0xE3, 0xE5, 0xE7, 0xE9, 0xE8,
	0xEA, 0xEB, 0xED, 0xEC, 0xEE, 0xEF, 0xF1, 0xF3,
	0xF2, 0xF4, 0xF6, 0xF5, 0xFA, 0xF9, 0xFB, 0xFC,
	0x86, 0xB0, 0xA2, 0xA3, 0xA7, 0x95, 0xB6, 0xDF,
	0xAE, 0xA9, 0x99, 0xB4, 0xA8, 0x80, 0xC6, 0xD8,
	0x81, 0xB1, 0x8D, 0x8E, 0xA5, 0xB5, 0x8A, 0x8F,
	0x90, 0x9D, 0xA6, 0xAA, 0xBA, 0xAD, 0xE6, 0xF8,
	0xBF, 0xA1, 0xAC, 0x9E, 0x83, 0x9A, 0xB2, 0xAB,
	0xBB, 0x85, 0xA0, 0xC0, 0xC3, 0xD5, 0x8C, 0x9C,
	0x96, 0x97, 0x93, 0x94, 0x91, 0x92, 0xF7, 0xB3,
	0xFF, 0x9F, 0xB9, 0xA4, 0x8B, 0x9B, 0xBC, 0xBD,
	0x87, 0xB7, 0x82, 0x84, 0x89, 0xC2, 0xCA, 0xC1,
	0xCB, 0xC8, 0xCD, 0xCE, 0xCF, 0xCC, 0xD3, 0xD4,
	0xBE, 0xD2, 0xDA, 0xDB, 0xD9, 0xD0, 0x88, 0x98,
	0xAF, 0xD7, 0xDD, 0xDE, 0xB8, 0xF0, 0xFD, 0xFE
};

#if IncludePbufs
LOCALFUNC blnr MacRomanTextToNativeHand(tPbuf Pbuf_no,
	blnr IsFileName, HGLOBAL *r)
{
	HGLOBAL h;
	ui5b i;
	ui5b rn = 0;
	HGLOBAL bh = PbufDat[Pbuf_no];
	ui5b L = PbufSize[Pbuf_no];
	blnr IsOk = falseblnr;

	if (IsFileName) {
		if (L > 255) {
			L = 255;
		}
	} else {
		ui3b *Buffer = (ui3b *)GlobalLock(bh);
		if (Buffer != NULL) {
			for (i = 0; i < L; ++i) {
				if (Buffer[i] == 13) {
					++rn;
				}
			}
			(void) GlobalUnlock(bh);
		}
	}

	h = GlobalAlloc(GMEM_DDESHARE, (L + rn + 1) * sizeof(TCHAR));
	if (h != NULL) {
		ui3b *Buffer = (ui3b *)GlobalLock(bh);
		if (Buffer != NULL) {
			LPTSTR p1 = GlobalLock(h);
			if (p1 != NULL) {
				for (i = 0; i < L; ++i) {
					TCHAR y;
					ui3b x = ((ui3b *)Buffer)[i];
					if (x >= 128) {
						y = (TCHAR)MacRoman2NativeTab[x - 128];
					} else {
						if (IsFileName) {
							if ((x < 32)
								|| ('\\' == x) || ('/' == x)
								|| (':' == x) || ('*' == x)
								|| ('?' == x) || ('"' == x)
								|| ('<' == x) || ('>' == x)
								|| ('|' == x))
							{
								y = (TCHAR)('-');
							} else {
								y = (TCHAR)x;
							}
						} else {
							if (13 == x) {
								*p1++ = (TCHAR)(13);
								y = (TCHAR)(10);
							} else {
								y = (TCHAR)x;
							}
						}
					}
					*p1++ = y;
				}
				*p1++ = (TCHAR) 0; /* null character */

				*r = h;
				IsOk = trueblnr;

				(void) GlobalUnlock(h);
			}
			(void) GlobalUnlock(bh);
		}
		if (! IsOk) {
			(void) GlobalFree(h);
		}
	}

	return IsOk;
}
#endif

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEexport(tPbuf i)
{
	HGLOBAL h;
	tMacErr err = mnvm_miscErr;

	if (MacRomanTextToNativeHand(i, falseblnr, &h)) {
		if (! OpenClipboard(MainWnd)) {
			/* ReportGetLastError(); */
		} else {
			if (! EmptyClipboard()) {
				/* ReportGetLastError(); */
			}
			if (SetClipboardData(CF_TEXT, h) == NULL) {
				/* ReportGetLastError(); */
			} else {
				err = mnvm_noErr;
			}
			h = NULL;
			if (! CloseClipboard()) {
				/* ReportGetLastError(); */
			}
		}
		if (h != NULL) {
			(void) GlobalFree(h);
		}
	}

	PbufDispose(i);

	return err;
}
#endif

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEimport(tPbuf *r)
{
	tMacErr err = mnvm_miscErr;

	if (IsClipboardFormatAvailable(CF_TEXT)) {
		if (! OpenClipboard(MainWnd)) {
			/* ReportGetLastError(); */
		} else {
			HGLOBAL h = GetClipboardData(CF_TEXT);
			if (h == NULL) {
				/* ReportGetLastError(); */
			} else {
				err = NativeTextToMacRomanPbuf(h, r);
			}
			if (! CloseClipboard()) {
				/* ReportGetLastError(); */
			}
		}
	}

	return err;
}
#endif

/* --- drives --- */

#define NotAfileRef INVALID_HANDLE_VALUE

LOCALVAR HANDLE Drives[NumDrives]; /* open disk image files */

#define NeedDriveNames (IncludeSonyGetName || IncludeSonyNew)

#if NeedDriveNames
LOCALVAR HGLOBAL DriveNames[NumDrives];
	/*
		It is supposed to be possible to use
		GetMappedFileName to get name of open file,
		but that seems ugly kludge, so instead
		save the name on open.
	*/
#endif

LOCALPROC InitDrives(void)
{
	/*
		This isn't really needed, Drives[i] and DriveNames[i]
		need not have valid values when not vSonyIsInserted[i].
	*/
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
#if NeedDriveNames
		DriveNames[i] = NULL;
#endif
	}
}

GLOBALOSGLUFUNC tMacErr vSonyTransfer(blnr IsWrite, ui3p Buffer,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count,
	ui5r *Sony_ActCount)
{
	HANDLE refnum;
	DWORD newL;
	tMacErr result;
	DWORD BytesTransferred = 0;

	refnum = Drives[Drive_No];
	newL = SetFilePointer(
		refnum, /* handle of file */
		Sony_Start, /* number of bytes to move file pointer */
		nullpr, /* address of high-order word of distance to move */
		FILE_BEGIN /* how to move */
	);
	if (newL == 0xFFFFFFFF) {
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else if (Sony_Start != (ui5b)newL) {
		/* not supposed to get here */
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else {
		if (IsWrite) {
			if (! WriteFile(refnum, /* handle of file to read */
				(LPVOID)Buffer
					, /* address of buffer that receives data */
				(DWORD)Sony_Count, /* number of bytes to read */
				&BytesTransferred, /* address of number of bytes read */
				nullpr)) /* address of structure for data */
			{
				result = mnvm_miscErr;
					/*& figure out what really to return &*/
			} else if ((ui5b)BytesTransferred != Sony_Count) {
				result = mnvm_miscErr;
					/*& figure out what really to return &*/
			} else {
				result = mnvm_noErr;
			}
		} else {
			if (! ReadFile(refnum, /* handle of file to read */
				(LPVOID)Buffer,
					/* address of buffer that receives data */
				(DWORD)Sony_Count, /* number of bytes to read */
				&BytesTransferred,
					/* address of number of bytes read */
				nullpr)) /* address of structure for data */
			{
				result = mnvm_miscErr;
					/*& figure out what really to return &*/
			} else if ((ui5b)BytesTransferred != Sony_Count) {
				result = mnvm_miscErr;
					/*& figure out what really to return &*/
			} else {
				result = mnvm_noErr;
			}
		}
	}

	if (nullpr != Sony_ActCount) {
		*Sony_ActCount = BytesTransferred;
	}

	return result;
}

GLOBALOSGLUFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	tMacErr result;
	DWORD L;

	L = GetFileSize(Drives[Drive_No], nullpr);
	if (L == 0xFFFFFFFF) {
		result = mnvm_miscErr; /*& figure out what really to return &*/
	} else {
		*Sony_Count = L;
		result = mnvm_noErr;
	}

	return result;
}

LOCALFUNC tMacErr vSonyEject0(tDrive Drive_No, blnr deleteit)
{
	HANDLE refnum = Drives[Drive_No];

#if ! NeedDriveNames
	UnusedParam(deleteit);
#endif

	Drives[Drive_No] = NotAfileRef; /* not really needed */

	DiskEjectedNotify(Drive_No);

	(void) FlushFileBuffers(refnum);
	(void) CloseHandle(refnum);

#if NeedDriveNames
	{
		HGLOBAL h = DriveNames[Drive_No];
		if (NULL != h) {
			if (deleteit) {
				LPTSTR drivepath = GlobalLock(h);
				if (drivepath != NULL) {
					(void) DeleteFile(drivepath);
					(void) GlobalUnlock(h);
				}
			}
			(void) GlobalFree(h);
			DriveNames[Drive_No] = NULL; /* not really needed */
		}
	}
#endif

	return mnvm_noErr;
}

GLOBALOSGLUFUNC tMacErr vSonyEject(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, falseblnr);
}

#if IncludeSonyNew
GLOBALOSGLUFUNC tMacErr vSonyEjectDelete(tDrive Drive_No)
{
	return vSonyEject0(Drive_No, trueblnr);
}
#endif

LOCALPROC UnInitDrives(void)
{
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		if (vSonyIsInserted(i)) {
			(void) vSonyEject(i);
		}
	}
}

#if NeedDriveNames
LOCALFUNC blnr LPTSTRtoHand(LPTSTR s, HGLOBAL *r)
{
	blnr IsOk = falseblnr;

	size_t L = _tcslen(s);
	HGLOBAL h = GlobalAlloc(GMEM_DDESHARE,
		(L + 1) * sizeof(TCHAR));
	if (h != NULL) {
		LPTSTR p = GlobalLock(h);
		if (p != NULL) {
			_tcscpy(p, s);
			IsOk = trueblnr;
			(void) GlobalUnlock(h);
		}
		if (! IsOk) {
			(void) GlobalFree(h);
		} else {
			*r = h;
		}
	}

	return IsOk;
}
#endif

#if IncludeSonyGetName
GLOBALOSGLUFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r)
{
	WIN32_FIND_DATA fd;
	tMacErr err = mnvm_miscErr;
	HGLOBAL ph = DriveNames[Drive_No];
	if (NULL != ph) {
		LPTSTR drivepath = GlobalLock(ph);
		if (drivepath != NULL) {
			HANDLE hf = FindFirstFile(drivepath, &fd);
			(void) GlobalUnlock(ph);

			if (hf != INVALID_HANDLE_VALUE) {
				HGLOBAL h;
				if (LPTSTRtoHand(fd.cFileName, &h)) {
					err = NativeTextToMacRomanPbuf(h, r);
				}
				FindClose(hf);
			}
		}
	}

	return err;
}
#endif

LOCALFUNC blnr Sony_Insert0(HANDLE refnum, blnr locked,
	LPTSTR drivepath)
{
	tDrive Drive_No;

#if ! NeedDriveNames
	UnusedParam(drivepath);
#endif

	if (! FirstFreeDisk(&Drive_No)) {
		(void) CloseHandle(refnum);
		MacMsg(kStrTooManyImagesTitle,
			kStrTooManyImagesMessage, falseblnr);
		return falseblnr;
	} else {
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);
#if NeedDriveNames
		{
			HGLOBAL h;

			if (! LPTSTRtoHand(drivepath, &h)) {
				h = NULL;
			}

			DriveNames[Drive_No] = h;
		}
#endif
		return trueblnr;
	}
}

LOCALFUNC blnr Sony_Insert1(LPTSTR drivepath, blnr SilentOnMissing)
{
	blnr locked = falseblnr;
	HANDLE refnum = CreateFile(
		drivepath, /* pointer to name of the file */
		GENERIC_READ + GENERIC_WRITE, /* access (read-write) mode */
		0, /* share mode */
		nullpr, /* pointer to security descriptor */
		OPEN_EXISTING, /* how to create */
		FILE_ATTRIBUTE_NORMAL, /* file attributes */
		nullpr /* handle to file with attributes to copy */
	);
	if (refnum == INVALID_HANDLE_VALUE) {
		if (ERROR_ACCESS_DENIED == GetLastError()) {
			locked = trueblnr;
			refnum = CreateFile(
				drivepath, /* pointer to name of the file */
				GENERIC_READ, /* access (read-write) mode */
				FILE_SHARE_READ, /* share mode */
				nullpr, /* pointer to security descriptor */
				OPEN_EXISTING, /* how to create */
				FILE_ATTRIBUTE_NORMAL, /* file attributes */
				nullpr /* handle to file with attributes to copy */
			);
		}
	}
	if (refnum == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		if (ERROR_SHARING_VIOLATION == err) {
			MacMsg(kStrImageInUseTitle,
				kStrImageInUseMessage, falseblnr);
		} else if ((ERROR_FILE_NOT_FOUND == err) && SilentOnMissing) {
			/* ignore it */
		} else {
			MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
		}
	} else {
		return Sony_Insert0(refnum, locked, drivepath);
	}
	return falseblnr;
}

LOCALFUNC blnr LoadMacRomFromPath(LPTSTR drivepath)
{
	HANDLE refnum = INVALID_HANDLE_VALUE;
	blnr IsOk = falseblnr;

	refnum = CreateFile(
		drivepath, /* pointer to name of the file */
		GENERIC_READ, /* access (read-write) mode */
		FILE_SHARE_READ, /* share mode */
		nullpr, /* pointer to security descriptor */
		OPEN_EXISTING, /* how to create */
		FILE_ATTRIBUTE_NORMAL, /* file attributes */
		nullpr /* handle to file with attributes to copy */
	);

	if (refnum == INVALID_HANDLE_VALUE) {
		/* MacMsg(kStrNoROMTitle, kStrNoROMMessage, trueblnr); */
	} else {
		DWORD BytesRead;

		if (! ReadFile(refnum, /* handle of file to read */
			(LPVOID)ROM, /* address of buffer that receives data */
			(DWORD)kROM_Size, /* number of bytes to read */
			&BytesRead, /* address of number of bytes read */
			nullpr)) /* address of structure for data */
		{
			MacMsgOverride(kStrNoReadROMTitle, kStrNoReadROMMessage);
		} else
		if ((ui5b)BytesRead != kROM_Size) {
			MacMsgOverride(kStrShortROMTitle, kStrShortROMMessage);
		} else
		{
			IsOk = (mnvm_noErr == ROM_IsValid());
		}
		(void) CloseHandle(refnum);
	}

	return IsOk;
}

#ifndef EnableShellLinks
#define EnableShellLinks 1
#endif

#if EnableShellLinks
LOCALVAR blnr COMinited = falseblnr;
LOCALVAR blnr COMinitedOK;
#endif

#if EnableShellLinks
LOCALPROC MyUninitCOM(void)
{
	if (COMinited) {
		CoUninitialize();
	}
}
#endif

#if EnableShellLinks
LOCALFUNC blnr MyNeedCOM(void)
{
	HRESULT hres;

	if (! COMinited) {
		COMinitedOK = falseblnr;
		hres = CoInitialize(NULL);
		if (SUCCEEDED(hres)) {
			COMinitedOK = trueblnr;
		}

		COMinited = trueblnr;
	}
	return COMinitedOK;
}
#endif

#if EnableShellLinks
LOCALFUNC blnr MyResolveShortcut(LPTSTR FilePath, blnr *directory)
/* adapted from Microsoft example code */
{
	HRESULT hres;
	IShellLink *psl;
	IPersistFile* ppf;
	TCHAR szGotPath[MAX_PATH];
	WIN32_FIND_DATA wfd;
	blnr IsOk = falseblnr;

	if (MyNeedCOM()) {
		hres = CoCreateInstance(&CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER, &IID_IShellLink,
			(LPVOID *)(void *)&psl);
			/*
				the (void *) prevents a compiler warning
				from gcc
			*/
		if (SUCCEEDED(hres)) {
			/* Get a pointer to the IPersistFile interface. */
			hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile,
				(void **)(void *)&ppf);
			if (SUCCEEDED(hres)) {
				/* Ensure that the string is Unicode. */
#if MyUseUni
#define wsz FilePath
#else
				WORD wsz[MAX_PATH];
				MultiByteToWideChar(CP_ACP, 0, FilePath, -1, wsz,
					MAX_PATH);
#endif

				/* Load the shortcut. */
				hres = ppf->lpVtbl->Load(ppf, wsz, STGM_READ);
				if (SUCCEEDED(hres)) {
					/* Resolve the link. */
					hres = psl->lpVtbl->Resolve(psl, MainWnd,
						SLR_ANY_MATCH);
					if (SUCCEEDED(hres)) {
						/* Get the path to the link target. */
						hres = psl->lpVtbl->GetPath(psl, szGotPath,
							MAX_PATH, &wfd,
							SLGP_SHORTPATH);
						if (SUCCEEDED(hres)) {
							/*
								This is in the sample code, but doesn't
								seem to be needed:
								Get the description of the target.
								char szDescription[MAX_PATH];
								hres = psl->lpVtbl->GetDescription(psl,
									szDescription, MAX_PATH);
								if (SUCCEEDED(hres)) {
								}
							*/
							lstrcpy(FilePath, szGotPath);
							if (NULL != directory) {
								*directory = (0 != (wfd.dwFileAttributes
									& FILE_ATTRIBUTE_DIRECTORY));
							}
							IsOk = trueblnr;
						}
					}
				}

				ppf->lpVtbl->Release(ppf);
			}
			psl->lpVtbl->Release(psl);
		}
	}
	return IsOk;
}
#endif

#if EnableShellLinks
LOCALFUNC blnr FileIsLink(LPTSTR drivepath)
{
	LPTSTR p = FindLastTerm(drivepath, (TCHAR)('.'));

	if (p != nullpr) {
		if (_tcscmp(p, TEXT("lnk")) == 0) {
			return trueblnr;
		}
	}
	return falseblnr;
}
#endif

LOCALFUNC blnr InsertDiskOrAlias(LPTSTR drivepath,
	blnr MaybeROM, blnr MaybeAlias)
{
#if EnableShellLinks
	if (MaybeAlias && FileIsLink(drivepath)) {
		if (! MyResolveShortcut(drivepath, NULL)) {
			return falseblnr;
		}
	}
#endif

	if (MaybeROM && ! ROM_loaded) {
		return LoadMacRomFromPath(drivepath);
	} else {
		return Sony_Insert1(drivepath, falseblnr);
	}
}

LOCALFUNC blnr MyFileExists(LPTSTR pathName, blnr *directory)
{
	WIN32_FIND_DATA fd;
	HANDLE hf = FindFirstFile(pathName, &fd);
	blnr IsOk = falseblnr;

	if (hf != INVALID_HANDLE_VALUE) {
		if (NULL != directory) {
			*directory =
				(0 != (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
		}
		IsOk = trueblnr;
		FindClose(hf);
	}

	return IsOk;
}

LOCALFUNC tMacErr ResolveNamedChild0(LPTSTR pathName,
	LPTSTR Child, blnr *directory)
{
	size_t newlen;
	size_t oldlen = _tcslen(pathName);
	tMacErr err = mnvm_miscErr;

	newlen = oldlen + 1 + _tcslen(Child);
	if (newlen + 1 < _MAX_PATH) {
		_tcscat(pathName, TEXT("\\"));
		_tcscat(pathName, Child);

		if (MyFileExists(pathName, directory)) {
			err = mnvm_noErr;
		} else {
			err = mnvm_fnfErr;
#if EnableShellLinks
			if (newlen + 5 < _MAX_PATH) {
				_tcscat(pathName, TEXT(".lnk"));
				if (MyFileExists(pathName, NULL))
				if (MyResolveShortcut(pathName, directory))
				{
					err = mnvm_noErr;
				}
				if (mnvm_noErr != err) {
					pathName[newlen] = (TCHAR)('\0');
				}
			}
#endif
		}
	}

	return err;
}

LOCALFUNC tMacErr ResolveNamedChild(LPTSTR pathName,
	char *Child, blnr *directory)
{
	TCHAR Child0[ClStrMaxLength + 1];

	NativeStrFromCStr(Child0, Child, falseblnr);

	return ResolveNamedChild0(pathName, Child0, directory);
}

LOCALFUNC blnr ResolveNamedChildDir(LPTSTR pathName, char *Child)
{
	blnr directory;

	return (mnvm_noErr == ResolveNamedChild(
		pathName, Child, &directory))
		&& directory;
}

LOCALFUNC blnr ResolveNamedChildFile(LPTSTR pathName, char *Child)
{
	blnr directory;

	return (mnvm_noErr == ResolveNamedChild(
		pathName, Child, &directory))
		&& ! directory;
}

#if UseActvFile || (IncludeSonyNew && ! SaveDialogEnable)
LOCALFUNC blnr MakeNamedChildDir(LPTSTR pathName, char *Child)
{
	blnr directory;
	blnr IsOk = falseblnr;
	tMacErr err = ResolveNamedChild(pathName, Child, &directory);

	if (mnvm_noErr == err) {
		IsOk = directory;
	} else if (mnvm_fnfErr == err) {
		if (CreateDirectory(pathName, NULL)) {
			IsOk = trueblnr;
		}
	}

	return IsOk;
}
#endif

LOCALFUNC blnr MyGetAppDataPath(LPTSTR lpszPath,
	BOOL fCreate)
{
	blnr IsOk = falseblnr;

	if (HaveMySHGetSpecialFolderPath())
	if (MySHGetSpecialFolderPath(
		NULL /* HWND hwndOwner */,
		lpszPath, My_CSIDL_APPDATA, fCreate))
	{
		IsOk = trueblnr;
	}
	/*
		if not available, could perhaps
		use GetWindowsDirectory.
	*/
	/*
		SHGetFolderPath is more recent,
		could perhaps check for it first.
		might also be in "SHFolder.dll".

		SHGetKnownFolderPath is even
		more recent.
	*/

	return IsOk;
}

#if UseWinCE
/* Are we in control mode? */
/* Needed because you can't hold down a key with the virtual keyboard */
LOCALVAR blnr CtrlMode = falseblnr;
#endif

LOCALPROC InsertADisk0(void)
{
	OPENFILENAME ofn;
	TCHAR szDirName[256];
	TCHAR szFile[256];
#if ! UseWinCE
	TCHAR szFileTitle[256];
#endif
	UINT i;
	size_t cbString;
	TCHAR chReplace;
	TCHAR szFilter[256];
	blnr IsOk;

	szDirName[0] = (TCHAR)('\0');
	szFile[0] = (TCHAR)('\0');
	_tcscpy(szFilter,
		TEXT("Disk images|*.dsk;*.HF?;*.IMG;*.IMA;*.IMAGE")
		TEXT("|All files (*.*)|*.*|\0"));

	cbString = _tcslen(szFilter);

	chReplace = szFilter[cbString - 1];

	for (i = 0; szFilter[i] != (TCHAR)('\0'); ++i)
	{
		if (szFilter[i] == chReplace) {
			szFilter[i] = (TCHAR)('\0');
		}
	}

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = MainWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 2;
	ofn.lpstrFile= szFile;
	ofn.nMaxFile = sizeof(szFile);
#if ! UseWinCE
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = sizeof(szFileTitle);
#endif
	ofn.lpstrInitialDir = szDirName;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST
		| OFN_HIDEREADONLY;

	MyBeginDialog();
	IsOk = GetOpenFileName(&ofn);
	MyEndDialog();

	if(! IsOk) {
		/* report error */
#if UseWinCE
		if (szFile[0]) {
			char wMsg[1024];
			sprintf(wMsg, "Couldn't open %ls", szFile);
			MacMsg("error", wMsg, falseblnr);
		}
#endif
	} else {
		(void) InsertDiskOrAlias(ofn.lpstrFile,
			trueblnr, falseblnr);
	}

#if UseWinCE
	CtrlMode = falseblnr;
#endif
}

LOCALFUNC blnr LoadInitialImageFromName(char *ImageName)
{
	TCHAR ImageFile[_MAX_PATH];

	if (GetAppDir(ImageFile))
	if (ResolveNamedChildFile(ImageFile, ImageName))
	if (Sony_Insert1(ImageFile, trueblnr))
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALFUNC blnr Sony_InsertIth(int i)
{
	blnr v;

	if ((i > 9) || ! FirstFreeDisk(nullpr)) {
		v = falseblnr;
	} else {
		char s[] = "disk?.dsk";

		s[4] = '0' + i;

		/* stop on first error (including file not found) */
		v = LoadInitialImageFromName(s);
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

#if UseActvFile

#define ActvCodeFileName "act_1"

LOCALFUNC tMacErr ActvCodeFileLoad(ui3p p)
{
	TCHAR pathName[_MAX_PATH];
	DWORD BytesRead;
	HANDLE refnum = INVALID_HANDLE_VALUE;
	blnr IsOk = falseblnr;

	if (MyGetAppDataPath(pathName, FALSE))
	if (ResolveNamedChildDir(pathName, "Gryphel"))
	if (ResolveNamedChildDir(pathName, "mnvm_act"))
	if (ResolveNamedChildFile(pathName, ActvCodeFileName))
	{
		refnum = CreateFile(
			pathName, /* pointer to name of the file */
			GENERIC_READ, /* access (read-write) mode */
			FILE_SHARE_READ, /* share mode */
			NULL, /* pointer to security descriptor */
			OPEN_EXISTING, /* how to create */
			FILE_ATTRIBUTE_NORMAL, /* file attributes */
			NULL /* handle to file with attributes to copy */
		);
		if (INVALID_HANDLE_VALUE == refnum) {
			/* report error */
		} else {
			if (SetFilePointer(
				refnum, /* handle of file */
				0, /* number of bytes to move file pointer */
				nullpr,
					/* address of high-order word of distance to move */
				FILE_BEGIN /* how to move */
				) != 0)
			{
				/* report error */
			} else if (! ReadFile(refnum, /* handle of file to read */
				(LPVOID)p, /* address of buffer that receives data */
				(DWORD)ActvCodeFileLen, /* number of bytes to read */
				&BytesRead, /* address of number of bytes read */
				nullpr) /* address of structure for data */
				|| ((ui5b)BytesRead != ActvCodeFileLen))
			{
				/* report error */
			} else {
				IsOk = trueblnr;
			}
			(void) CloseHandle(refnum);
		}
	}

	return IsOk ? mnvm_noErr : mnvm_miscErr;
}

LOCALFUNC blnr NewNamedChildFile(LPTSTR pathName, char *Child)
{
	blnr directory;
	blnr IsOk = falseblnr;
	tMacErr err = ResolveNamedChild(pathName, Child, &directory);

	if (mnvm_noErr == err) {
		IsOk = ! directory;
	} else if (mnvm_fnfErr == err) {
		IsOk = trueblnr;
	}

	return IsOk;
}

LOCALFUNC tMacErr ActvCodeFileSave(ui3p p)
{
	TCHAR pathName[_MAX_PATH];
	DWORD BytesWritten;
	HANDLE refnum = INVALID_HANDLE_VALUE;
	blnr IsOk = falseblnr;

	if (MyGetAppDataPath(pathName, TRUE))
	if (MakeNamedChildDir(pathName, "Gryphel"))
	if (MakeNamedChildDir(pathName, "mnvm_act"))
	if (NewNamedChildFile(pathName, ActvCodeFileName))
	{
		refnum = CreateFile(
			pathName, /* pointer to name of the file */
			GENERIC_READ + GENERIC_WRITE, /* access (read-write) mode */
			0, /* share mode */
			NULL, /* pointer to security descriptor */
			CREATE_ALWAYS, /* how to create */
			FILE_ATTRIBUTE_NORMAL, /* file attributes */
			NULL /* handle to file with attributes to copy */
		);
		if (INVALID_HANDLE_VALUE == refnum) {
			/* report error */
		} else {
			if (SetFilePointer(
				refnum, /* handle of file */
				0, /* number of bytes to move file pointer */
				nullpr,
					/* address of high-order word of distance to move */
				FILE_BEGIN /* how to move */
				) != 0)
			{
				/* report error */
			} else if (! WriteFile(refnum, /* handle of file to read */
				(LPVOID)p, /* address of buffer that receives data */
				(DWORD)ActvCodeFileLen, /* number of bytes to read */
				&BytesWritten, /* address of number of bytes read */
				nullpr) /* address of structure for data */
				|| ((ui5b)BytesWritten != ActvCodeFileLen))
			{
				/* report error */
			} else {
				IsOk = trueblnr;
			}
			(void) CloseHandle(refnum);
			if (! IsOk) {
				(void) DeleteFile(pathName);
			}
		}
	}

	return IsOk ? mnvm_noErr : mnvm_miscErr;
}

#endif /* UseActvFile */

#if IncludeSonyNew
LOCALFUNC blnr WriteZero(HANDLE refnum, ui5b L)
{
	if (SetFilePointer(
		refnum, /* handle of file */
		0, /* number of bytes to move file pointer */
		nullpr, /* address of high-order word of distance to move */
		FILE_BEGIN /* how to move */
		) != 0)
	{
		return falseblnr;
	} else {
#define ZeroBufferSize 2048
		ui5b i;
		ui3b buffer[ZeroBufferSize];
		DWORD BytesWritten;

		memset(&buffer, 0, ZeroBufferSize);

		while (L > 0) {
			i = (L > ZeroBufferSize) ? ZeroBufferSize : L;
			if (! WriteFile(refnum, /* handle of file to read */
				(LPVOID)buffer,
					/* address of buffer that receives data */
				(DWORD)i, /* number of bytes to read */
				&BytesWritten, /* address of number of bytes read */
				nullpr) /* address of structure for data */
				|| ((ui5b)BytesWritten != i))
			{
				return falseblnr;
			}
			L -= i;
		}
		return trueblnr;
	}
}
#endif

#define MaxSavePathSize MAX_PATH

#if IncludeSonyNew
LOCALPROC MakeNewDisk0(ui5b L, LPTSTR pathName)
{
	blnr IsOk = falseblnr;
	HANDLE newrefNum;

	IsOk = falseblnr;
	newrefNum = CreateFile(
		pathName, /* pointer to name of the file */
		GENERIC_READ + GENERIC_WRITE, /* access (read-write) mode */
		0, /* share mode */
		NULL, /* pointer to security descriptor */
		CREATE_ALWAYS, /* how to create */
		FILE_ATTRIBUTE_NORMAL, /* file attributes */
		NULL /* handle to file with attributes to copy */
	);
	if (newrefNum == INVALID_HANDLE_VALUE) {
		/* report error */
	} else {
		if (SetFilePointer(
			newrefNum, /* handle of file */
			L, /* number of bytes to move file pointer */
			nullpr,
				/* address of high-order word of distance to move */
			FILE_BEGIN /* how to move */
			) != L)
		{
			/* report error */
		} else if (! SetEndOfFile(newrefNum)) {
			/* report error */
		} else if (! WriteZero(newrefNum, L)) {
			/* report error */
		} else {
			IsOk =
				Sony_Insert0(newrefNum, falseblnr, pathName);
			newrefNum = INVALID_HANDLE_VALUE;
		}
		if (INVALID_HANDLE_VALUE != newrefNum) {
			(void) CloseHandle(newrefNum);
		}
		if (! IsOk) {
			(void) DeleteFile(pathName);
		}
	}
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, HGLOBAL NewDiskNameDat)
{
#if SaveDialogEnable
	OPENFILENAME ofn;
	blnr IsOk = falseblnr;
	TCHAR szFile[MaxSavePathSize];
	TCHAR szFileTitle[MaxSavePathSize];

	memset(&ofn, 0, sizeof(OPENFILENAME));
	szFile[0] = 0;
	szFileTitle[0] = 0;

#if IncludeSonyGetName
	if (NewDiskNameDat != NULL) {
		LPTSTR p = GlobalLock(NewDiskNameDat);
		if (p != NULL) {
			_tcscpy(szFile, p);
			(void) GlobalUnlock(NewDiskNameDat);
		}
	} else
#endif
	{
		NativeStrFromCStr(szFile, "untitled", falseblnr);
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = szFile;
	ofn.hwndOwner = MainWnd;
	/* ofn.lpstrFilter = "All\0*.*\0Text\0*.txt\0Datafile\0*.dsk\0"; */
	/* ofn.lpstrFilter = NULL; */ /* szFilter */
	ofn.nMaxFile = MaxSavePathSize;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = MaxSavePathSize;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT + OFN_HIDEREADONLY;
		/* + OFN_SHOWHELP */

	MyBeginDialog();
	IsOk = GetSaveFileName(&ofn);
	MyEndDialog();

	if (! IsOk) {
		/* report error */
	} else {
		MakeNewDisk0(L, ofn.lpstrFile);
	}
#else /* SaveDialogEnable */
	TCHAR pathName[MaxSavePathSize];

	if (GetAppDir(pathName))
	if (MakeNamedChildDir(pathName, "out"))
	{
		blnr directory;
		LPTSTR p = GlobalLock(NewDiskNameDat);

		if (p != NULL) {
			tMacErr err = ResolveNamedChild0(pathName, p,
				&directory);

			if (mnvm_fnfErr == err) {
				err = mnvm_noErr;
			} else if (mnvm_noErr == err) {
				if (directory) {
					err = mnvm_miscErr;
				}
			}

			if (mnvm_noErr == err) {
				MakeNewDisk0(L, pathName);
			}

			(void) GlobalUnlock(NewDiskNameDat);
		}
	}
#endif /* SaveDialogEnable */
}
#endif

LOCALFUNC blnr LoadMacRom(void)
{
	TCHAR ROMFile[_MAX_PATH];
	blnr IsOk = falseblnr;

	if (GetAppDir(ROMFile))
	if (ResolveNamedChildFile(ROMFile, RomFileName))
	{
		IsOk = trueblnr;
	}

	if (! IsOk) {
		if (MyGetAppDataPath(ROMFile, FALSE))
		if (ResolveNamedChildDir(ROMFile, "Gryphel"))
		if (ResolveNamedChildDir(ROMFile, "mnvm_rom"))
		if (ResolveNamedChildFile(ROMFile, RomFileName))
		{
			IsOk = trueblnr;
		}

	}

	if (IsOk) {
		IsOk = LoadMacRomFromPath(ROMFile);
	}

	return trueblnr;
}

#if InstallFileIcons
LOCALPROC MySetRegKey(HKEY hKeyRoot,
	LPTSTR strRegKey, LPTSTR strRegValue)
{
	HKEY RegKey;
	DWORD dwDisposition;

	if (ERROR_SUCCESS == RegCreateKeyEx(hKeyRoot, strRegKey, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
		NULL, &RegKey, &dwDisposition))
	{
		RegSetValueEx(RegKey, NULL, 0, REG_SZ,
			(CONST BYTE *)strRegValue,
			(DWORD)((_tcslen(strRegValue) + 1) * sizeof(TCHAR)));
		RegCloseKey(RegKey);
	}
}

LOCALPROC RegisterShellFileType(LPTSTR AppPath, LPTSTR strFilterExt,
	LPTSTR strFileTypeId, LPTSTR strFileTypeName,
	LPTSTR strIconId, blnr CanOpen)
{
	TCHAR strRegKey[_MAX_PATH];
	TCHAR strRegValue[_MAX_PATH + 2];
		/* extra room for ","{strIconId} */

	MySetRegKey(HKEY_CLASSES_ROOT, strFileTypeId, strFileTypeName);
	MySetRegKey(HKEY_CLASSES_ROOT, strFilterExt, strFileTypeId);

	_tcscpy(strRegKey, strFileTypeId);
	_tcscat(strRegKey, TEXT("\\DefaultIcon"));
	_tcscpy(strRegValue, TEXT("\""));
	_tcscat(strRegValue, AppPath);
	_tcscat(strRegValue, TEXT("\","));
	_tcscat(strRegValue, strIconId);
	MySetRegKey(HKEY_CLASSES_ROOT, strRegKey, strRegValue);

	if (CanOpen) {
		_tcscpy(strRegKey, strFileTypeId);
		_tcscat(strRegKey, TEXT("\\shell\\open\\command"));
		_tcscpy(strRegValue, TEXT("\""));
		_tcscat(strRegValue, AppPath);
		_tcscat(strRegValue, TEXT("\" \"%1\""));
		MySetRegKey(HKEY_CLASSES_ROOT, strRegKey, strRegValue);
	}
}

LOCALFUNC blnr RegisterInRegistry(void)
{
	TCHAR AppPath[_MAX_PATH];

	GetModuleFileName(NULL, AppPath, _MAX_PATH);
#if 0
	GetShortPathName(AppPath, AppPath, _MAX_PATH);
#endif

	RegisterShellFileType(AppPath, TEXT(".rom"), TEXT("minivmac.rom"),
		TEXT("Mini vMac ROM Image"), TEXT("1"), falseblnr);
	RegisterShellFileType(AppPath, TEXT(".dsk"), TEXT("minivmac.dsk"),
		TEXT("Mini vMac Disk Image"), TEXT("2"), trueblnr);

	return trueblnr;
}
#endif

LOCALVAR LPTSTR CommandLine;

LOCALFUNC blnr ScanCommandLine(void)
{
	TCHAR *p = CommandLine;
	TCHAR *p1;
	TCHAR *p2;
	TCHAR v;
	size_t L;

	v = *p;
	while (0 != v) {
		if (' ' == v) {
			v = *++p;
		} else {
			if ('\"' == v) {
				v = *++p;
				p1 = p;
				while (('\"' != v) && (0 != v)) {
					v = *++p;
				}
				p2 = p;
				if ('\"' == v) {
					v = *++p;
				}
			} else {
				p1 = p;
				while ((' ' != v) && (0 != v)) {
					v = *++p;
				}
				p2 = p;
			}
			L = p2 - p1;
			if (L + 1 <= _MAX_PATH) {
				TCHAR fileName[_MAX_PATH];
				TCHAR *filePtr = fileName;
				size_t i = L;

				while (i > 0) {
					*filePtr++ = *p1++;
					--i;
				}
				*filePtr = (char)0;

				if ((L > 0)
					&& (('/' == fileName[0]) || ('-' == fileName[0])))
				{
#if 0
					TCHAR *p3 = &fileName[1];
					if (0 == _tcscmp(p3, TEXT("l"))) {
						SpeedValue = 0;
					} else
#endif
					{
						MacMsg(kStrBadArgTitle, kStrBadArgMessage,
							falseblnr);
					}
				} else {
					(void) InsertDiskOrAlias(fileName,
						falseblnr, trueblnr);
				}
			}
		}
	}

	return trueblnr;
}

#if EnableRecreateW
LOCALPROC CheckMagnifyAndFullScreen(void)
{
	if (
#if EnableMagnify
		(UseMagnify != WantMagnify)
#endif
#if EnableMagnify && VarFullScreen
		||
#endif
#if VarFullScreen
		(UseFullScreen != WantFullScreen)
#endif
		)
	{
		(void) ReCreateMainWindow();
	}
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
				if ((GetSystemMetrics(SM_CXSCREEN)
						>= vMacScreenWidth * MyWindowScale)
					&& (GetSystemMetrics(SM_CYSCREEN)
						>= vMacScreenHeight * MyWindowScale)
					)
				{
					WantMagnify = trueblnr;
				}
			}
		}
	}
#endif
}
#endif

#if EnableDragDrop
LOCALPROC DragFunc(HDROP hDrop)
{
	WORD n;
	WORD i;
	TCHAR a[_MAX_PATH];

	n = DragQueryFile(hDrop, (UINT) -1, NULL, 0);
	for (i = 0; i < n; ++i) {
		if (DragQueryFile(hDrop, i, NULL, 0) < _MAX_PATH - 1) {
			(void) DragQueryFile(hDrop, i, a, _MAX_PATH);
			(void) InsertDiskOrAlias(a, trueblnr, trueblnr);
		}
	}

	DragFinish(hDrop);

	if (gTrueBackgroundFlag) {
		if (! SetForegroundWindow(MainWnd)) {
			/* error message here ? */
		}

		WantCmdOptOnReconnect = trueblnr;
	}
}
#endif

GLOBALOSGLUFUNC blnr ExtraTimeNotOver(void)
{
#if MySoundEnabled
	SoundCheckVeryOften();
#endif
	(void) UpdateTrueEmulatedTime();
	return (TrueEmulatedTime == OnTrueTime);
}

/* --- platform independent code can be thought of as going here --- */

LOCALPROC LeaveBackground(void)
{
	ReconnectKeyCodes3();
}

LOCALPROC EnterBackground(void)
{
	DisconnectKeyCodes3();

#if VarFullScreen
	if (WantFullScreen) {
		ToggleWantFullScreen();
	}
#endif
}

LOCALPROC LeaveSpeedStopped(void)
{
#if MySoundEnabled
	MySound_Start();
#endif
#if (MyTimeResolution != 0)
	MyTimer_Resume();
#endif
}

LOCALPROC EnterSpeedStopped(void)
{
#if (MyTimeResolution != 0)
	MyTimer_Suspend();
#endif
#if MySoundEnabled
	MySound_Stop();
#endif
}

LOCALPROC CheckForSavedTasks(void)
{
	/*
		Check for things to do that rather wouldn't
		have done at an awkward time.
	*/

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

#if EnableRecreateW
	if (! (gTrueBackgroundFlag)) {
		CheckMagnifyAndFullScreen();
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
		AdjustMachineGrab();
	}
#endif

	if (gTrueBackgroundFlag) {
		/*
			wait til later
		*/
	} else {
#if IncludeSonyNew
		if (vSonyNewDiskWanted) {
#if IncludeSonyNameNew
			if (vSonyNewDiskName != NotAPbuf) {
				HGLOBAL NewDiskNameDat;
				if (MacRomanTextToNativeHand(vSonyNewDiskName, trueblnr,
					&NewDiskNameDat))
				{
					MakeNewDisk(vSonyNewDiskSize, NewDiskNameDat);
					GlobalFree(NewDiskNameDat);
				}
				PbufDispose(vSonyNewDiskName);
				vSonyNewDiskName = NotAPbuf;
			} else
#endif
			{
				MakeNewDisk(vSonyNewDiskSize, NULL);
			}
			vSonyNewDiskWanted = falseblnr;
				/* must be done after may have gotten disk */
		}
#endif
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

	if (HaveCursorHidden != (WantCursorHidden
		&& ! (gTrueBackgroundFlag || CurSpeedStopped)))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		if (HaveCursorHidden) {
			(void) ShowCursor(FALSE);
		} else {
			(void) ShowCursor(TRUE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		ScreenChangedAll();
	}
}

#if UseWinCE
/* Sip Status ON/OFF */
LOCALVAR blnr SipOn = falseblnr;
#endif

LRESULT CALLBACK Win32WMProc(HWND hwnd,
	UINT uMessage, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK Win32WMProc(HWND hwnd,
	UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	switch (uMessage)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;

				BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
#if VarFullScreen
				if (UseFullScreen)
#endif
#if MayFullScreen
				{
					if (! FillRect(ps.hdc,
						&ps.rcPaint,
						GetStockObject(BLACK_BRUSH)))
					{
						/* ReportGetLastError(); */
					}
				}
#endif
				if (MainWnd == hwnd) {
					Screen_DrawAll();
				}
				EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
			}
			break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
#if UseWinCE
			SipOn = falseblnr;

			{
				SIPINFO r;

				memset(&r, 0 , sizeof(SIPINFO));
				r.cbSize = sizeof(SIPINFO);
				if (SipGetInfo(&r)) {
					SipOn = 0 != (r.fdwFlags & SIPF_ON);
				}
			}

			if (wparam == 0xAE) {
				break;
			} else if ((! SipOn) && (wparam == VK_RETURN)) {
				break;
			} else if ((! SipOn)
				&& (wparam >= VK_LEFT) && (wparam <= VK_DOWN))
			{
				break;
			} else if (wparam == VK_CONTROL && CtrlMode) {
				DoVKcode0(wparam, falseblnr);
				CtrlMode = falseblnr;
				break;
			} else if (wparam == VK_CONTROL) {
				DoVKcode0(wparam, trueblnr);
				CtrlMode = trueblnr;
				break;
			}
#endif
			if (! TestBit(lparam, 30)) { /* ignore repeats */
				DoVKcode(wparam, lparam >> 24, trueblnr);
			}

#if UseWinCE
			return TRUE;
				/*
					So that hardware keys won't be
					processed by the default handler
				*/
#endif

			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
#if UseWinCE
			SipOn = falseblnr;

			{
				SIPINFO r;

				memset(&r, 0 , sizeof(SIPINFO));
				r.cbSize = sizeof(SIPINFO);
				if (SipGetInfo(&r)) {
					SipOn = 0 != (r.fdwFlags & SIPF_ON);
				}
			}

			if (wparam == 0xAE) { /* to hide SoftInput panel */
				SipShowIM(SIPF_OFF);
				break;
			} else if ((! SipOn) && (wparam == VK_RETURN)) {
				/* DPad Action to show SIP */
				/* Show SoftInput Panel */
				SipShowIM(SIPF_ON);
				break;
			} else if ((! SipOn)
				&& (wparam >= VK_LEFT) && (wparam <= VK_DOWN))
			{
				switch (wparam) {
					case VK_LEFT:
						if (ViewHStart < (ViewHSize / 2)) {
							ViewHStart = 0;
						} else {
							ViewHStart -= (ViewHSize / 2);
						}
						break;
					case VK_UP:
						if (ViewVStart < (ViewVSize / 2)) {
							ViewVStart = 0;
						} else {
							ViewVStart -= (ViewVSize / 2);
						}
						break;
					case VK_RIGHT:
						ViewHStart += (ViewHSize / 2);
						if (ViewHStart >= (vMacScreenWidth - ViewHSize))
						{
							ViewHStart = vMacScreenWidth - ViewHSize;
						}
						break;
					case VK_DOWN:
						ViewVStart += (ViewVSize / 2);
						if (ViewVStart
							>= (vMacScreenHeight - ViewVSize))
						{
							ViewVStart = vMacScreenHeight - ViewVSize;
						}
						break;
				}
				Screen_DrawAll();
			} else
			if (wparam == VK_CONTROL && CtrlMode) {
				break;
			}
#endif
			DoVKcode(wparam, lparam >> 24, falseblnr);

#if UseWinCE
			return TRUE;
				/*
					So that hardware keys won't be
					processed by the default handler
				*/
#endif

			break;
#if ItnlKyBdFix && ! UseWinCE
		case WM_INPUTLANGCHANGE:
			MyCheckKeyboardLayout();
			return TRUE;
			break;
#endif

		case WM_CLOSE:
			RequestMacOff = trueblnr;
			break;
#if ! UseWinCE
		case WM_QUERYENDSESSION:
			if (AnyDiskInserted()) {
				RequestMacOff = trueblnr;
				return FALSE;
			} else {
				return TRUE;
			}
			break;
#endif
		case WM_ACTIVATE:
			if (MainWnd == hwnd) {
				gTrueBackgroundFlag = (LOWORD(wparam) == WA_INACTIVE);
			}
			break;
		case WM_COMMAND:
			switch(LOWORD(wparam))
			{
				case ID_FILE_INSERTDISK1:
					RequestInsertDisk = trueblnr;
					break;
				case ID_FILE_QUIT:
					RequestMacOff = trueblnr;
					break;
				case ID_SPECIAL_MORECOMMANDS:
					DoMoreCommandsMsg();
					break;
				case ID_HELP_ABOUT:
					DoAboutMsg();
					break;
			}
			break;
		case WM_MOVE:
			WndX = (si4b) LOWORD(lparam);
			WndY = (si4b) HIWORD(lparam);
			break;
		case WM_SYSCHAR:
		case WM_CHAR:
			/* prevent any further processing */
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			MousePositionNotify(LOWORD (lparam), HIWORD (lparam));
			SetCurMouseButton(trueblnr);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			MousePositionNotify(LOWORD (lparam), HIWORD (lparam));
			SetCurMouseButton(falseblnr);
			break;
		case WM_MOUSEMOVE:
#if UseWinCE
			MousePositionNotify(LOWORD (lparam), HIWORD (lparam));
#endif
			/* windows may have messed up cursor */
			/*
				there is no notification when the mouse moves
				outside the window, and the cursor is automatically
				changed
			*/
			if (! HaveCursorHidden) {
				/* SetCursor(LoadCursor(NULL, IDC_ARROW)); */
			}
			break;
#if EnableDragDrop
		case WM_CREATE:
			DragAcceptFiles(hwnd, TRUE);
			break;
		case WM_DROPFILES:
			DragFunc((HDROP) wparam);
			break;
		case WM_DESTROY:
			DragAcceptFiles(hwnd, FALSE);
			break;
#endif
		default:
			return DefWindowProc(hwnd, uMessage, wparam, lparam);
	}
	return 0;
}

LOCALFUNC blnr RegisterOurClass(void)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW
#if ! UseWinCE
		| CS_OWNDC
#endif
		;
	wc.lpfnWndProc   = (WNDPROC)Win32WMProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = AppInstance;
	wc.hIcon         = LoadIcon(AppInstance, MAKEINTRESOURCE(IDI_VMAC));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WndClassName;

	if (! RegisterClass(&wc)) {
		MacMsg("RegisterClass failed",
			"Sorry, Mini vMac encountered errors"
			" and cannot continue.", trueblnr);
		return falseblnr;
	} else {
		return trueblnr;
	}
}

LOCALPROC WaitForTheNextEvent(void)
{
	MSG msg;

	if (-1 != GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}
}

LOCALPROC CheckForSystemEvents(void)
{
	MSG msg;
	ui3r i = 0;

	while ((i < 32) && (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))) {
		DispatchMessage(&msg);
		++i;
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
		Sleep(NextIntTime - LastTime);
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

	if (! (gBackgroundFlag)) {
#if ! UseWinCE
		CheckMouseState();
#endif

#if EnableGrabSpecialKeys
		CheckForLostKeyUps();
#endif
	}

	OnTrueTime = TrueEmulatedTime;

#if dbglog_TimeStuff
	dbglog_writelnNum("WaitForNextTick, OnTrueTime", OnTrueTime);
#endif
}

#if UseWinCE
LOCALFUNC blnr Init_ChangeOrientation(void)
{
	DEVMODE dm;

	/* initialize the DEVMODE structure */
	ZeroMemory(&dm, sizeof (dm));
	dm.dmSize = sizeof (dm);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	/* Backup old values */
	oldOrientation = dm.dmOrientation;
	oldDisplayOrientation = dm.dmDisplayOrientation;


	/* Hide SIP (you can never tell...) */
	SipShowIM(SIPF_OFF);

	/* Switch to Landscape mode if possible */
	dm.dmOrientation = DMORIENT_LANDSCAPE;
	dm.dmDisplayOrientation = DMDO_90;
	dm.dmFields = DM_ORIENTATION | DM_DISPLAYORIENTATION;
	(void) ChangeDisplaySettingsEx(NULL, &dm, NULL, 0, 0);
	/*
		if (ChangeDisplaySettingsEx(NULL, &dm, NULL, 0, 0) !=
			DISP_CHANGE_SUCCESSFUL)
		{
			MacMsg ("warning",
				"Couldn't switch to Landscape mode.", falseblnr);
		}
	*/

	return trueblnr;
}
#endif

#if UseWinCE
LOCALPROC Uninit_ChangeOrientation(void)
{
	DEVMODE dm;

	/* Restore old display orientation */
	ZeroMemory(&dm, sizeof (dm));
	dm.dmSize = sizeof(dm);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	dm.dmOrientation = oldOrientation;
	dm.dmDisplayOrientation = oldDisplayOrientation;
	dm.dmFields = DM_ORIENTATION | DM_DISPLAYORIENTATION;

	ChangeDisplaySettingsEx(NULL, &dm, 0, 0, 0);
}
#endif


/* ** code for handling hardware keys in Pocket PC devices ** */

#if UseWinCE
typedef BOOL (__stdcall *UnregisterFunc1Proc)(UINT, UINT);
LOCALVAR HINSTANCE hCoreDLL = NULL;
#endif

#if UseWinCE
LOCALFUNC blnr InitHotKeys(void)
{
	UnregisterFunc1Proc procUndergisterFunc;
	int i;

	hCoreDLL = LoadLibrary(TEXT("coredll.dll"));
	if (! hCoreDLL) {
		MacMsg ("Fatal", "Could not load coredll.dll", trueblnr);
	} else {
		procUndergisterFunc =
			(UnregisterFunc1Proc) GetProcAddress(hCoreDLL,
			TEXT("UnregisterFunc1"));
		if (! procUndergisterFunc) {
			MacMsg ("Fatal",
				"Could not get UnregisterFunc1 procedure", trueblnr);
		} else {
			for (i = 0xc1; i <= 0xcf; ++i) {
				procUndergisterFunc(MOD_WIN, i);
				RegisterHotKey(MainWnd, i, MOD_WIN, i);
			}
		}
	}
	return trueblnr;
}
#endif

#if UseWinCE
LOCALPROC UninitHotKeys(void)
{
	if (! hCoreDLL) {
		FreeLibrary(hCoreDLL);
	}
}
#endif

#include "PROGMAIN.h"

/* ************************ */

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
#if EnableScalingBuff
	{
		ui5r n = vMacScreenMonoNumBytes
#if EnableMagnify
			* MyWindowScale * MyWindowScale
#endif
			;
#if 1 == vMacScreenDepth
		if (vMacScreenNumBytes * 2 > n) {
			n = vMacScreenNumBytes * 2;
		}
#elif vMacScreenDepth >= 4
		if (vMacScreenNumBytes > n) {
			n = vMacScreenNumBytes;
		}
#endif
		ReserveAllocOneBlock(&ScalingBuff, n, 5, falseblnr);
	}
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
	ReserveAllocBigBlock =
		(ui3p)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, n);
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
		if (GlobalFree(ReserveAllocBigBlock) != NULL) {
			MacMsg("error", "GlobalFree failed", falseblnr);
		}
	}
}

LOCALFUNC blnr InitOSGLU(void)
{
	if (AllocMyMemory())
#if dbglog_HAVE
	if (dbglog_open())
#endif
	if (RegisterOurClass())
	if (ScanCommandLine())
	if (LoadInitialImages())
#if InstallFileIcons
	if (RegisterInRegistry())
#endif
	if (LoadMacRom())
#if UseActvCode
	if (ActvCodeInit())
#endif
#if UseWinCE
	if (Init_ChangeOrientation())
#endif
	if (ReCreateMainWindow())
	if (InitWinKey2Mac())
	if (InitTheCursor())
#if UseWinCE
	if (InitHotKeys())
#endif
	if (Init60thCheck())
	if (WaitForRom())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
#if (MyTimeResolution != 0)
	MyTimer_Suspend();
#endif
	MyMouseCaptureSet(falseblnr);

	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

#if MayFullScreen
	UnGrabTheMachine();
#endif
#if MySoundEnabled
	MySound_Stop();
#endif
#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

	ForceShowCursor();
#if UseWinCE
	Uninit_ChangeOrientation();
	UninitHotKeys();
#endif

#if EnableShellLinks
	MyUninitCOM();
#endif

	if (! gTrueBackgroundFlag) {
		CheckSavedMacMsg();
	}

	DisposeMainWindow();

#if dbglog_HAVE
	dbglog_close();
#endif

	UnallocMyMemory();
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	UnusedParam(hPrevInstance);
	AppInstance = hInstance;
	CmdShow = nCmdShow;
	CommandLine = lpCmdLine;

	GetWndTitle();
#if UseWinCE
	if (AlreadyRunningCheck()) {
		return 0;
	}
#endif

	ZapOSGLUVars();
	if (InitOSGLU()) {
		ProgramMain();
	}
	UnInitOSGLU();

	return(0);
}
