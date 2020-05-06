/*
	OSGLUOSX.c

	Copyright (C) 2009 Philip Cummins, Richard F. Bannister,
		Paul C. Pratt

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
	Operating System GLUe for macintosh OS X

	All operating system dependent code for the
	Macintosh platform should go here.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.

	The main entry point 'main' is at the end of this file.
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

/* --- adapting to API/ABI version differences --- */

/*
	if UsingCarbonLib, instead of native Macho,
	then some APIs are missing
*/
#ifndef UsingCarbonLib
#define UsingCarbonLib 0
#endif

LOCALVAR CFBundleRef AppServBunRef;

LOCALVAR blnr DidApplicationServicesBun = falseblnr;

LOCALFUNC blnr HaveApplicationServicesBun(void)
{
	if (! DidApplicationServicesBun) {
		AppServBunRef = CFBundleGetBundleWithIdentifier(
			CFSTR("com.apple.ApplicationServices"));
		DidApplicationServicesBun = trueblnr;
	}
	return (AppServBunRef != NULL);
}

#if MayFullScreen || UsingCarbonLib

LOCALVAR CFBundleRef HIToolboxBunRef;

LOCALVAR blnr DidHIToolboxBunRef = falseblnr;

LOCALFUNC blnr HaveHIToolboxBunRef(void)
{
	if (! DidHIToolboxBunRef) {
		HIToolboxBunRef = CFBundleGetBundleWithIdentifier(
			CFSTR("com.apple.HIToolbox"));
		DidHIToolboxBunRef = trueblnr;
	}
	return (HIToolboxBunRef != NULL);
}

#endif

LOCALVAR CFBundleRef AGLBunRef;

LOCALVAR blnr DidAGLBunRef = falseblnr;

LOCALFUNC blnr HaveAGLBunRef(void)
{
	if (! DidAGLBunRef) {
		AGLBunRef = CFBundleGetBundleWithIdentifier(
			CFSTR("com.apple.agl"));
		DidAGLBunRef = trueblnr;
	}
	return (AGLBunRef != NULL);
}


#if MayFullScreen

/* SetSystemUIModeProcPtr API always not available */

typedef UInt32                          MySystemUIMode;
typedef OptionBits                      MySystemUIOptions;

enum {
	MykUIModeNormal                 = 0,
	MykUIModeAllHidden              = 3
};

enum {
	MykUIOptionAutoShowMenuBar      = 1 << 0,
	MykUIOptionDisableAppleMenu     = 1 << 2,
	MykUIOptionDisableProcessSwitch = 1 << 3,
	MykUIOptionDisableForceQuit     = 1 << 4,
	MykUIOptionDisableSessionTerminate = 1 << 5,
	MykUIOptionDisableHide          = 1 << 6
};

typedef OSStatus (*SetSystemUIModeProcPtr)
	(MySystemUIMode inMode, MySystemUIOptions inOptions);
LOCALVAR SetSystemUIModeProcPtr MySetSystemUIMode = NULL;
LOCALVAR blnr DidSetSystemUIMode = falseblnr;

LOCALFUNC blnr HaveMySetSystemUIMode(void)
{
	if (! DidSetSystemUIMode) {
		if (HaveHIToolboxBunRef()) {
			MySetSystemUIMode =
				(SetSystemUIModeProcPtr)
				CFBundleGetFunctionPointerForName(
					HIToolboxBunRef, CFSTR("SetSystemUIMode"));
		}
		DidSetSystemUIMode = trueblnr;
	}
	return (MySetSystemUIMode != NULL);
}

#endif

/* In 10.1 or later */

typedef OSStatus (*LSCopyDisplayNameForRefProcPtr)
	(const FSRef *inRef, CFStringRef *outDisplayName);
LOCALVAR LSCopyDisplayNameForRefProcPtr MyLSCopyDisplayNameForRef
	= NULL;
LOCALVAR blnr DidLSCopyDisplayNameForRef = falseblnr;

LOCALFUNC blnr HaveMyLSCopyDisplayNameForRef(void)
{
	if (! DidLSCopyDisplayNameForRef) {
		if (HaveApplicationServicesBun()) {
			MyLSCopyDisplayNameForRef =
				(LSCopyDisplayNameForRefProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("LSCopyDisplayNameForRef"));
		}
		DidLSCopyDisplayNameForRef = trueblnr;
	}
	return (MyLSCopyDisplayNameForRef != NULL);
}

/* In 10.5 or later */

typedef GLboolean (*aglSetWindowRefProcPtr)
	(AGLContext ctx, WindowRef window);
LOCALVAR aglSetWindowRefProcPtr MyaglSetWindowRef = NULL;
LOCALVAR blnr DidaglSetWindowRef = falseblnr;

LOCALFUNC blnr HaveMyaglSetWindowRef(void)
{
	if (! DidaglSetWindowRef) {
		if (HaveAGLBunRef()) {
			MyaglSetWindowRef =
				(aglSetWindowRefProcPtr)
				CFBundleGetFunctionPointerForName(
					AGLBunRef, CFSTR("aglSetWindowRef"));
		}
		DidaglSetWindowRef = trueblnr;
	}
	return (MyaglSetWindowRef != NULL);
}

/* Deprecated as of 10.5 */

typedef CGrafPtr My_AGLDrawable;
typedef GLboolean (*aglSetDrawableProcPtr)
	(AGLContext ctx, My_AGLDrawable draw);
LOCALVAR aglSetDrawableProcPtr MyaglSetDrawable = NULL;
LOCALVAR blnr DidaglSetDrawable = falseblnr;

LOCALFUNC blnr HaveMyaglSetDrawable(void)
{
	if (! DidaglSetDrawable) {
		if (HaveAGLBunRef()) {
			MyaglSetDrawable =
				(aglSetDrawableProcPtr)
				CFBundleGetFunctionPointerForName(
					AGLBunRef, CFSTR("aglSetDrawable"));
		}
		DidaglSetDrawable = trueblnr;
	}
	return (MyaglSetDrawable != NULL);
}

/* routines not in carbon lib */


#if UsingCarbonLib

typedef CGDisplayErr
(*CGGetActiveDisplayListProcPtr) (
	CGDisplayCount       maxDisplays,
	CGDirectDisplayID *  activeDspys,
	CGDisplayCount *     dspyCnt);
LOCALVAR CGGetActiveDisplayListProcPtr MyCGGetActiveDisplayList = NULL;
LOCALVAR blnr DidCGGetActiveDisplayList = falseblnr;

LOCALFUNC blnr HaveMyCGGetActiveDisplayList(void)
{
	if (! DidCGGetActiveDisplayList) {
		if (HaveApplicationServicesBun()) {
			MyCGGetActiveDisplayList =
				(CGGetActiveDisplayListProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGGetActiveDisplayList"));
		}
		DidCGGetActiveDisplayList = trueblnr;
	}
	return (MyCGGetActiveDisplayList != NULL);
}

#else

#define HaveMyCGGetActiveDisplayList() trueblnr
#define MyCGGetActiveDisplayList CGGetActiveDisplayList

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGRect
(*CGDisplayBoundsProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayBoundsProcPtr MyCGDisplayBounds = NULL;
LOCALVAR blnr DidCGDisplayBounds = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayBounds(void)
{
	if (! DidCGDisplayBounds) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayBounds =
				(CGDisplayBoundsProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayBounds"));
		}
		DidCGDisplayBounds = trueblnr;
	}
	return (MyCGDisplayBounds != NULL);
}

#else

#define HaveMyCGDisplayBounds() trueblnr
#define MyCGDisplayBounds CGDisplayBounds

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef size_t
(*CGDisplayPixelsWideProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayPixelsWideProcPtr MyCGDisplayPixelsWide = NULL;
LOCALVAR blnr DidCGDisplayPixelsWide = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayPixelsWide(void)
{
	if (! DidCGDisplayPixelsWide) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayPixelsWide =
				(CGDisplayPixelsWideProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayPixelsWide"));
		}
		DidCGDisplayPixelsWide = trueblnr;
	}
	return (MyCGDisplayPixelsWide != NULL);
}

#else

#define HaveMyCGDisplayPixelsWide() trueblnr
#define MyCGDisplayPixelsWide CGDisplayPixelsWide

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef size_t
(*CGDisplayPixelsHighProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayPixelsHighProcPtr MyCGDisplayPixelsHigh = NULL;
LOCALVAR blnr DidCGDisplayPixelsHigh = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayPixelsHigh(void)
{
	if (! DidCGDisplayPixelsHigh) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayPixelsHigh =
				(CGDisplayPixelsHighProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayPixelsHigh"));
		}
		DidCGDisplayPixelsHigh = trueblnr;
	}
	return (MyCGDisplayPixelsHigh != NULL);
}

#else

#define HaveMyCGDisplayPixelsHigh() trueblnr
#define MyCGDisplayPixelsHigh CGDisplayPixelsHigh

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGDisplayErr
(*CGDisplayHideCursorProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayHideCursorProcPtr MyCGDisplayHideCursor = NULL;
LOCALVAR blnr DidCGDisplayHideCursor = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayHideCursor(void)
{
	if (! DidCGDisplayHideCursor) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayHideCursor =
				(CGDisplayHideCursorProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayHideCursor"));
		}
		DidCGDisplayHideCursor = trueblnr;
	}
	return (MyCGDisplayHideCursor != NULL);
}

#else

#define HaveMyCGDisplayHideCursor() trueblnr
#define MyCGDisplayHideCursor CGDisplayHideCursor

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGDisplayErr
(*CGDisplayShowCursorProcPtr) (CGDirectDisplayID display);
LOCALVAR CGDisplayShowCursorProcPtr MyCGDisplayShowCursor = NULL;
LOCALVAR blnr DidCGDisplayShowCursor = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayShowCursor(void)
{
	if (! DidCGDisplayShowCursor) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayShowCursor =
				(CGDisplayShowCursorProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayShowCursor"));
		}
		DidCGDisplayShowCursor = trueblnr;
	}
	return (MyCGDisplayShowCursor != NULL);
}

#else

#define HaveMyCGDisplayShowCursor() trueblnr
#define MyCGDisplayShowCursor CGDisplayShowCursor

#endif /* ! UsingCarbonLib */


#if 0

typedef CGDisplayErr (*CGDisplayMoveCursorToPointProcPtr)
	(CGDirectDisplayID display, CGPoint point);
LOCALVAR CGDisplayMoveCursorToPointProcPtr MyCGDisplayMoveCursorToPoint
	= NULL;
LOCALVAR blnr DidCGDisplayMoveCursorToPoint = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayMoveCursorToPoint(void)
{
	if (! DidCGDisplayMoveCursorToPoint) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayMoveCursorToPoint =
				(CGDisplayMoveCursorToPointProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGDisplayMoveCursorToPoint"));
		}
		DidCGDisplayMoveCursorToPoint = trueblnr;
	}
	return (MyCGDisplayMoveCursorToPoint != NULL);
}

#endif /* 0 */


#if UsingCarbonLib

typedef CGEventErr
(*CGWarpMouseCursorPositionProcPtr) (CGPoint newCursorPosition);
LOCALVAR CGWarpMouseCursorPositionProcPtr MyCGWarpMouseCursorPosition
	= NULL;
LOCALVAR blnr DidCGWarpMouseCursorPosition = falseblnr;

LOCALFUNC blnr HaveMyCGWarpMouseCursorPosition(void)
{
	if (! DidCGWarpMouseCursorPosition) {
		if (HaveApplicationServicesBun()) {
			MyCGWarpMouseCursorPosition =
				(CGWarpMouseCursorPositionProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGWarpMouseCursorPosition"));
		}
		DidCGWarpMouseCursorPosition = trueblnr;
	}
	return (MyCGWarpMouseCursorPosition != NULL);
}

#else

#define HaveMyCGWarpMouseCursorPosition() trueblnr
#define MyCGWarpMouseCursorPosition CGWarpMouseCursorPosition

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef CGEventErr
(*CGSetLocalEventsSuppressionIntervalProcPtr) (CFTimeInterval seconds);
LOCALVAR CGSetLocalEventsSuppressionIntervalProcPtr
	MyCGSetLocalEventsSuppressionInterval = NULL;
LOCALVAR blnr DidCGSetLocalEventsSuppressionInterval = falseblnr;

LOCALFUNC blnr HaveMyCGSetLocalEventsSuppressionInterval(void)
{
	if (! DidCGSetLocalEventsSuppressionInterval) {
		if (HaveApplicationServicesBun()) {
			MyCGSetLocalEventsSuppressionInterval =
				(CGSetLocalEventsSuppressionIntervalProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef,
					CFSTR("CGSetLocalEventsSuppressionInterval"));
		}
		DidCGSetLocalEventsSuppressionInterval = trueblnr;
	}
	return (MyCGSetLocalEventsSuppressionInterval != NULL);
}

#else

#define HaveMyCGSetLocalEventsSuppressionInterval() trueblnr
#define MyCGSetLocalEventsSuppressionInterval \
	CGSetLocalEventsSuppressionInterval

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef OSStatus (*CreateStandardAlertProcPtr) (
	AlertType alertType,
	CFStringRef error,
	CFStringRef explanation,
	const AlertStdCFStringAlertParamRec * param,
	DialogRef * outAlert
);
LOCALVAR CreateStandardAlertProcPtr MyCreateStandardAlert = NULL;
LOCALVAR blnr DidCreateStandardAlert = falseblnr;

LOCALFUNC blnr HaveMyCreateStandardAlert(void)
{
	if (! DidCreateStandardAlert) {
		if (HaveHIToolboxBunRef()) {
			MyCreateStandardAlert =
				(CreateStandardAlertProcPtr)
				CFBundleGetFunctionPointerForName(
					HIToolboxBunRef, CFSTR("CreateStandardAlert"));
		}
		DidCreateStandardAlert = trueblnr;
	}
	return (MyCreateStandardAlert != NULL);
}

#else

#define HaveMyCreateStandardAlert() trueblnr
#define MyCreateStandardAlert CreateStandardAlert

#endif /* ! UsingCarbonLib */


#if UsingCarbonLib

typedef OSStatus (*RunStandardAlertProcPtr) (
	DialogRef inAlert,
	ModalFilterUPP filterProc,
	DialogItemIndex * outItemHit
);
LOCALVAR RunStandardAlertProcPtr MyRunStandardAlert = NULL;
LOCALVAR blnr DidRunStandardAlert = falseblnr;

LOCALFUNC blnr HaveMyRunStandardAlert(void)
{
	if (! DidRunStandardAlert) {
		if (HaveHIToolboxBunRef()) {
			MyRunStandardAlert =
				(RunStandardAlertProcPtr)
				CFBundleGetFunctionPointerForName(
					HIToolboxBunRef, CFSTR("RunStandardAlert"));
		}
		DidRunStandardAlert = trueblnr;
	}
	return (MyRunStandardAlert != NULL);
}

#else

#define HaveMyRunStandardAlert() trueblnr
#define MyRunStandardAlert RunStandardAlert

#endif /* ! UsingCarbonLib */


typedef CGDirectDisplayID (*CGMainDisplayIDProcPtr)(void);

LOCALVAR CGMainDisplayIDProcPtr MyCGMainDisplayID = NULL;
LOCALVAR blnr DidCGMainDisplayID = falseblnr;

LOCALFUNC blnr HaveMyCGMainDisplayID(void)
{
	if (! DidCGMainDisplayID) {
		if (HaveApplicationServicesBun()) {
			MyCGMainDisplayID =
				(CGMainDisplayIDProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGMainDisplayID"));
		}
		DidCGMainDisplayID = trueblnr;
	}
	return (MyCGMainDisplayID != NULL);
}

#ifndef kCGNullDirectDisplay /* not in MPW Headers */
#define kCGNullDirectDisplay ((CGDirectDisplayID)0)
#endif

typedef CGError
(*CGDisplayRegisterReconfigurationCallbackProcPtr) (
	CGDisplayReconfigurationCallBack proc,
	void *userInfo
	);
LOCALVAR CGDisplayRegisterReconfigurationCallbackProcPtr
	MyCGDisplayRegisterReconfigurationCallback = NULL;
LOCALVAR blnr DidCGDisplayRegisterReconfigurationCallback = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayRegisterReconfigurationCallback(void)
{
	if (! DidCGDisplayRegisterReconfigurationCallback) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayRegisterReconfigurationCallback =
				(CGDisplayRegisterReconfigurationCallbackProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef,
					CFSTR("CGDisplayRegisterReconfigurationCallback"));
		}
		DidCGDisplayRegisterReconfigurationCallback = trueblnr;
	}
	return (MyCGDisplayRegisterReconfigurationCallback != NULL);
}


typedef CGError
(*CGDisplayRemoveReconfigurationCallbackProcPtr) (
	CGDisplayReconfigurationCallBack proc,
	void *userInfo
	);
LOCALVAR CGDisplayRemoveReconfigurationCallbackProcPtr
	MyCGDisplayRemoveReconfigurationCallback = NULL;
LOCALVAR blnr DidCGDisplayRemoveReconfigurationCallback = falseblnr;

LOCALFUNC blnr HaveMyCGDisplayRemoveReconfigurationCallback(void)
{
	if (! DidCGDisplayRemoveReconfigurationCallback) {
		if (HaveApplicationServicesBun()) {
			MyCGDisplayRemoveReconfigurationCallback =
				(CGDisplayRemoveReconfigurationCallbackProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef,
					CFSTR("CGDisplayRemoveReconfigurationCallback"));
		}
		DidCGDisplayRemoveReconfigurationCallback = trueblnr;
	}
	return (MyCGDisplayRemoveReconfigurationCallback != NULL);
}


typedef boolean_t (*CGCursorIsVisibleProcPtr)(void);

LOCALVAR CGCursorIsVisibleProcPtr MyCGCursorIsVisible = NULL;
LOCALVAR blnr DidCGCursorIsVisible = falseblnr;

LOCALFUNC blnr HaveMyCGCursorIsVisible(void)
{
	if (! DidCGCursorIsVisible) {
		if (HaveApplicationServicesBun()) {
			MyCGCursorIsVisible =
				(CGCursorIsVisibleProcPtr)
				CFBundleGetFunctionPointerForName(
					AppServBunRef, CFSTR("CGCursorIsVisible"));
		}
		DidCGCursorIsVisible = trueblnr;
	}
	return (MyCGCursorIsVisible != NULL);
}


/* --- end of adapting to API/ABI version differences --- */

/* --- some simple utilities --- */

GLOBALOSGLUPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	(void) memcpy((char *)destPtr, (char *)srcPtr, byteCount);
}

LOCALPROC PStrFromChar(ps3p r, char x)
{
	r[0] = 1;
	r[1] = (char)x;
}

/* --- mac style errors --- */

#define CheckSavetMacErr(result) (mnvm_noErr == (err = (result)))
	/*
		where 'err' is a variable of type tMacErr in the function
		this is used in
	*/

#define To_tMacErr(result) ((tMacErr)(ui4b)(result))

#define CheckSaveMacErr(result) (CheckSavetMacErr(To_tMacErr(result)))


#include "STRCONST.h"

#define NeedCell2UnicodeMap 1

#include "INTLCHAR.h"

LOCALPROC UniCharStrFromSubstCStr(int *L, UniChar *x, char *s)
{
	int i;
	int L0;
	ui3b ps[ClStrMaxLength];

	ClStrFromSubstCStr(&L0, ps, s);

	for (i = 0; i < L0; ++i) {
		x[i] = Cell2UnicodeMap[ps[i]];
	}

	*L = L0;
}

#define NotAfileRef (-1)

LOCALFUNC tMacErr MyMakeFSRefUniChar(FSRef *ParentRef,
	UniCharCount fileNameLength, const UniChar *fileName,
	blnr *isFolder, FSRef *ChildRef)
{
	tMacErr err;
	Boolean isFolder0;
	Boolean isAlias;

	if (CheckSaveMacErr(FSMakeFSRefUnicode(ParentRef,
		fileNameLength, fileName, kTextEncodingUnknown,
		ChildRef)))
	if (CheckSaveMacErr(FSResolveAliasFile(ChildRef,
		TRUE, &isFolder0, &isAlias)))
	{
		*isFolder = isFolder0;
	}

	return err;
}

LOCALFUNC tMacErr MyMakeFSRefC(FSRef *ParentRef, char *fileName,
	blnr *isFolder, FSRef *ChildRef)
{
	int L;
	UniChar x[ClStrMaxLength];

	UniCharStrFromSubstCStr(&L, x, fileName);
	return MyMakeFSRefUniChar(ParentRef, L, x,
		isFolder, ChildRef);
}

#if UseActvFile
LOCALFUNC tMacErr OpenNamedFileInFolderRef(FSRef *ParentRef,
	char *fileName, short *refnum)
{
	tMacErr err;
	blnr isFolder;
	FSRef ChildRef;
	HFSUniStr255 forkName;

	if (CheckSavetMacErr(MyMakeFSRefC(ParentRef, fileName,
		&isFolder, &ChildRef)))
	if (CheckSaveMacErr(FSGetDataForkName(&forkName)))
	if (CheckSaveMacErr(FSOpenFork(&ChildRef, forkName.length,
		forkName.unicode, fsRdPerm, refnum)))
	{
		/* ok */
	}

	return err;
}
#endif

#if dbglog_HAVE || UseActvFile
LOCALFUNC tMacErr OpenWriteNamedFileInFolderRef(FSRef *ParentRef,
	char *fileName, short *refnum)
{
	tMacErr err;
	blnr isFolder;
	FSRef ChildRef;
	HFSUniStr255 forkName;
	int L;
	UniChar x[ClStrMaxLength];

	UniCharStrFromSubstCStr(&L, x, fileName);
	err = MyMakeFSRefUniChar(ParentRef, L, x, &isFolder, &ChildRef);
	if (mnvm_fnfErr == err) {
		err = To_tMacErr(FSCreateFileUnicode(ParentRef, L, x, 0, NULL,
			&ChildRef, NULL));
	}
	if (mnvm_noErr == err) {
		if (CheckSaveMacErr(FSGetDataForkName(&forkName)))
		if (CheckSaveMacErr(FSOpenFork(&ChildRef, forkName.length,
			forkName.unicode, fsRdWrPerm, refnum)))
		{
			/* ok */
		}
	}

	return err;
}
#endif

LOCALFUNC tMacErr FindNamedChildRef(FSRef *ParentRef,
	char *ChildName, FSRef *ChildRef)
{
	tMacErr err;
	blnr isFolder;

	if (CheckSavetMacErr(MyMakeFSRefC(ParentRef, ChildName,
		&isFolder, ChildRef)))
	{
		if (! isFolder) {
			err = mnvm_miscErr;
		}
	}

	return err;
}

#if UseActvFile || (IncludeSonyNew && ! SaveDialogEnable)
LOCALFUNC tMacErr FindOrMakeNamedChildRef(FSRef *ParentRef,
	char *ChildName, FSRef *ChildRef)
{
	tMacErr err;
	blnr isFolder;
	int L;
	UniChar x[ClStrMaxLength];

	UniCharStrFromSubstCStr(&L, x, ChildName);
	if (CheckSavetMacErr(MyMakeFSRefUniChar(ParentRef, L, x,
		&isFolder, ChildRef)))
	{
		if (! isFolder) {
			err = mnvm_miscErr;
		}
	}
	if (mnvm_fnfErr == err) {
		err = To_tMacErr(FSCreateDirectoryUnicode(
			ParentRef, L, x, kFSCatInfoNone, NULL,
			ChildRef, NULL, NULL));
	}

	return err;
}
#endif

LOCALVAR FSRef MyDatDirRef;

LOCALVAR CFStringRef MyAppName = NULL;

LOCALPROC UnInitMyApplInfo(void)
{
	if (MyAppName != NULL) {
		CFRelease(MyAppName);
	}
}

LOCALFUNC blnr InitMyApplInfo(void)
{
	ProcessSerialNumber currentProcess = {0, kCurrentProcess};
	FSRef fsRef;
	FSRef parentRef;

	if (noErr == GetProcessBundleLocation(&currentProcess,
		&fsRef))
	if (noErr == FSGetCatalogInfo(&fsRef, kFSCatInfoNone,
		NULL, NULL, NULL, &parentRef))
	{
		FSRef ContentsRef;
		FSRef DatRef;

		MyDatDirRef = parentRef;
		if (mnvm_noErr == FindNamedChildRef(&fsRef, "Contents",
			&ContentsRef))
		if (mnvm_noErr == FindNamedChildRef(&ContentsRef, "mnvm_dat",
			&DatRef))
		{
			MyDatDirRef = DatRef;
		}

		if (HaveMyLSCopyDisplayNameForRef()) {
			if (noErr == MyLSCopyDisplayNameForRef(&fsRef, &MyAppName))
			{
				return trueblnr;
			}
		}

		if (noErr == CopyProcessName(&currentProcess, &MyAppName)) {
			return trueblnr;
		}
	}
	return falseblnr;
}

/* --- sending debugging info to file --- */

#if dbglog_HAVE

LOCALVAR SInt16 dbglog_File = NotAfileRef;

LOCALFUNC blnr dbglog_open0(void)
{
	tMacErr err;

	err = OpenWriteNamedFileInFolderRef(&MyDatDirRef,
		"dbglog.txt", &dbglog_File);

	return (mnvm_noErr == err);
}

LOCALPROC dbglog_write0(char *s, uimr L)
{
	ByteCount actualCount;

	if (dbglog_File != NotAfileRef) {
		(void) FSWriteFork(
			dbglog_File,
			fsFromMark,
			0,
			L,
			s,
			&actualCount);
	}
}

LOCALPROC dbglog_close0(void)
{
	if (dbglog_File != NotAfileRef) {
		(void) FSSetForkSize(dbglog_File, fsFromMark, 0);
		(void) FSCloseFork(dbglog_File);
		dbglog_File = NotAfileRef;
	}
}

#endif

#if 1 /* (0 != vMacScreenDepth) && (vMacScreenDepth < 4) */
#define WantColorTransValid 1
#endif

#include "COMOSGLU.h"

/* --- time, date --- */

/*
	be sure to avoid getting confused if TickCount
	overflows and wraps.
*/

#define dbglog_TimeStuff (0 && dbglog_HAVE)

LOCALVAR ui5b TrueEmulatedTime = 0;

LOCALVAR EventTime NextTickChangeTime;

#define MyTickDuration (kEventDurationSecond / 60.14742)

LOCALPROC UpdateTrueEmulatedTime(void)
{
	EventTime LatestTime = GetCurrentEventTime();
	EventTime TimeDiff = LatestTime - NextTickChangeTime;

	if (TimeDiff >= 0.0) {
		if (TimeDiff > 16 * MyTickDuration) {
			/* emulation interrupted, forget it */
			++TrueEmulatedTime;
			NextTickChangeTime = LatestTime + MyTickDuration;

#if dbglog_TimeStuff
			dbglog_writelnNum("emulation interrupted",
				TrueEmulatedTime);
#endif
		} else {
			do {
				++TrueEmulatedTime;
				TimeDiff -= MyTickDuration;
				NextTickChangeTime += MyTickDuration;
			} while (TimeDiff >= 0.0);
		}
	}
}

GLOBALOSGLUFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

/* LOCALVAR EventTime ETimeBase; */
LOCALVAR CFAbsoluteTime ATimeBase;
LOCALVAR ui5b TimeSecBase;

LOCALFUNC blnr CheckDateTime(void)
{
	ui5b NewMacDateInSecond = TimeSecBase
		+ (ui5b)(CFAbsoluteTimeGetCurrent() - ATimeBase);
	/*
		ui5b NewMacDateInSecond = TimeSecBase
			+ (ui5b)(GetCurrentEventTime() - ETimeBase);
	*/
	/*
		ui5b NewMacDateInSecond = ((ui5b)(CFAbsoluteTimeGetCurrent()))
			+ 3061137600UL;
	*/

	if (CurMacDateInSeconds != NewMacDateInSecond) {
		CurMacDateInSeconds = NewMacDateInSecond;
		return trueblnr;
	} else {
		return falseblnr;
	}
}

/* --- parameter buffers --- */

#include "PBUFSTDC.h"

/* --- drives --- */

LOCALVAR SInt16 Drives[NumDrives]; /* open disk image files */

GLOBALOSGLUFUNC tMacErr vSonyTransfer(blnr IsWrite, ui3p Buffer,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count,
	ui5r *Sony_ActCount)
{
	ByteCount actualCount;
	tMacErr result;

	if (IsWrite) {
		result = To_tMacErr(FSWriteFork(
			Drives[Drive_No],
			fsFromStart,
			Sony_Start,
			Sony_Count,
			Buffer,
			&actualCount));
	} else {
		result = To_tMacErr(FSReadFork(
			Drives[Drive_No],
			fsFromStart,
			Sony_Start,
			Sony_Count,
			Buffer,
			&actualCount));
	}

	if (nullpr != Sony_ActCount) {
		*Sony_ActCount = actualCount;
	}

	return result;
}

GLOBALOSGLUFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	SInt64 forkSize;
	tMacErr err = To_tMacErr(
		FSGetForkSize(Drives[Drive_No], &forkSize));
	*Sony_Count = forkSize;
	return err;
}

GLOBALOSGLUFUNC tMacErr vSonyEject(tDrive Drive_No)
{
	SInt16 refnum = Drives[Drive_No];
	Drives[Drive_No] = NotAfileRef;

	DiskEjectedNotify(Drive_No);

	(void) FSCloseFork(refnum);

	return mnvm_noErr;
}

#if IncludeSonyNew
GLOBALOSGLUFUNC tMacErr vSonyEjectDelete(tDrive Drive_No)
{
	FSRef ref;
	tMacErr err0;
	tMacErr err;

	err0 = To_tMacErr(FSGetForkCBInfo(Drives[Drive_No], 0,
		NULL /* iterator */,
		NULL /* actualRefNum */,
		NULL /* forkInfo */,
		&ref /* ref */,
		NULL /* outForkName */));
	err = vSonyEject(Drive_No);

	if (mnvm_noErr != err0) {
		err = err0;
	} else {
		(void) FSDeleteObject(&ref);
	}

	return err;
}
#endif

#if IncludeSonyGetName
GLOBALOSGLUFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r)
{
	FSRef ref;
	HFSUniStr255 outName;
	CFStringRef DiskName;
	tMacErr err;

	if (CheckSaveMacErr(FSGetForkCBInfo(Drives[Drive_No], 0,
		NULL /* iterator */,
		NULL /* actualRefNum */,
		NULL /* forkInfo */,
		&ref /* ref */,
		NULL /* outForkName */)))
	if (CheckSaveMacErr(FSGetCatalogInfo(&ref,
		kFSCatInfoNone /* whichInfo */,
		NULL /* catalogInfo */,
		&outName /* outName */,
		NULL /* fsSpec */,
		NULL /* parentRef */)))
	{
		DiskName = CFStringCreateWithCharacters(
			kCFAllocatorDefault, outName.unicode,
			outName.length);
		if (NULL != DiskName) {
			tPbuf i;

			if (CheckSavetMacErr(PbufNew(outName.length, &i))) {
				if (CFStringGetBytes(DiskName,
					CFRangeMake(0, outName.length),
					kCFStringEncodingMacRoman,
					'?', false,
					PbufDat[i],
					outName.length,
					NULL) != outName.length)
				{
					err = mnvm_miscErr;
				}
				if (mnvm_noErr != err) {
					PbufDispose(i);
				} else {
					*r = i;
				}
			}
			CFRelease(DiskName);
		}
	}

	return err;
}
#endif

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEexport(tPbuf i)
{
	tMacErr err;
	ScrapRef scrapRef;

	if (CheckSaveMacErr(ClearCurrentScrap()))
	if (CheckSaveMacErr(GetCurrentScrap(&scrapRef)))
	if (CheckSaveMacErr(PutScrapFlavor(
		scrapRef,
		FOUR_CHAR_CODE('TEXT'),
		kScrapFlavorMaskNone,
		PbufSize[i],
		PbufDat[i])))
	{
		/* ok */
	}

	PbufDispose(i);

	return err;
}
#endif

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEimport(tPbuf *r)
{
	tMacErr err;
	ScrapRef scrap;
	ScrapFlavorFlags flavorFlags;
	Size byteCount;
	tPbuf i;

	if (CheckSaveMacErr(GetCurrentScrap(&scrap)))
	if (CheckSaveMacErr(GetScrapFlavorFlags(scrap,
		'TEXT', &flavorFlags)))
	if (CheckSaveMacErr(GetScrapFlavorSize(scrap,
		'TEXT', &byteCount)))
	if (CheckSavetMacErr(PbufNew(byteCount, &i)))
	{
		Size byteCount2 = byteCount;
		if (CheckSaveMacErr(GetScrapFlavorData(scrap,
			'TEXT', &byteCount2,
			PbufDat[i])))
		{
			if (byteCount != byteCount2) {
				err = mnvm_miscErr;
			}
		}
		if (mnvm_noErr != err) {
			PbufDispose(i);
		} else {
			*r = i;
		}
	}

	return err;
}
#endif


#if EmLocalTalk

#include "BPFILTER.h"

#endif


/* --- control mode and internationalization --- */

#define WantKeyboard_RemapMac 1

#include "CONTROLM.h"


/* --- video out --- */

LOCALVAR WindowPtr gMyMainWindow = NULL;
LOCALVAR WindowPtr gMyOldWindow = NULL;

#if MayFullScreen
LOCALVAR short hOffset;
LOCALVAR short vOffset;
#endif

#if MayFullScreen
LOCALVAR blnr GrabMachine = falseblnr;
#endif

#if VarFullScreen
LOCALVAR blnr UseFullScreen = (WantInitFullScreen != 0);
#endif

#if EnableMagnify
LOCALVAR blnr UseMagnify = (WantInitMagnify != 0);
#endif

#if EnableMagnify
LOCALPROC MyScaleRect(Rect *r)
{
	r->left *= MyWindowScale;
	r->right *= MyWindowScale;
	r->top *= MyWindowScale;
	r->bottom *= MyWindowScale;
}
#endif

LOCALPROC SetScrnRectFromCoords(Rect *r,
	si4b top, si4b left, si4b bottom, si4b right)
{
	r->left = left;
	r->right = right;
	r->top = top;
	r->bottom = bottom;

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		OffsetRect(r, - ViewHStart, - ViewVStart);
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		MyScaleRect(r);
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		OffsetRect(r, hOffset, vOffset);
	}
#endif
}

LOCALVAR ui3p ScalingBuff = nullpr;

LOCALVAR ui3p CLUT_final;

#define CLUT_finalsz1 (256 * 8)

#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)

#define CLUT_finalClrSz (256 << (5 - vMacScreenDepth))

#define CLUT_finalsz ((CLUT_finalClrSz > CLUT_finalsz1) \
	? CLUT_finalClrSz : CLUT_finalsz1)

#else
#define CLUT_finalsz CLUT_finalsz1
#endif


#define ScrnMapr_DoMap UpdateBWLuminanceCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 3
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"


#if (0 != vMacScreenDepth) && (vMacScreenDepth < 4)

#define ScrnMapr_DoMap UpdateMappedColorCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth vMacScreenDepth
#define ScrnMapr_DstDepth 5
#define ScrnMapr_Map CLUT_final

#include "SCRNMAPR.h"

#endif

#if vMacScreenDepth >= 4

#define ScrnTrns_DoTrans UpdateTransColorCopy
#define ScrnTrns_Src GetCurDrawBuff()
#define ScrnTrns_Dst ScalingBuff
#define ScrnTrns_SrcDepth vMacScreenDepth
#define ScrnTrns_DstDepth 5
#define ScrnTrns_DstZLo 1

#include "SCRNTRNS.h"

#endif

LOCALPROC UpdateLuminanceCopy(si4b top, si4b left,
	si4b bottom, si4b right)
{
	int i;

#if 0 != vMacScreenDepth
	if (UseColorMode) {

#if vMacScreenDepth < 4

		if (! ColorTransValid) {
			int j;
			int k;
			ui5p p4;

			p4 = (ui5p)CLUT_final;
			for (i = 0; i < 256; ++i) {
				for (k = 1 << (3 - vMacScreenDepth); --k >= 0; ) {
					j = (i >> (k << vMacScreenDepth)) & (CLUT_size - 1);
					*p4++ = (((long)CLUT_reds[j] & 0xFF00) << 16)
						| (((long)CLUT_greens[j] & 0xFF00) << 8)
						| ((long)CLUT_blues[j] & 0xFF00);
				}
			}
			ColorTransValid = trueblnr;
		}

		UpdateMappedColorCopy(top, left, bottom, right);

#else
		UpdateTransColorCopy(top, left, bottom, right);
#endif

	} else
#endif
	{
		if (! ColorTransValid) {
			int k;
			ui3p p4 = (ui3p)CLUT_final;

			for (i = 0; i < 256; ++i) {
				for (k = 8; --k >= 0; ) {
					*p4++ = ((i >> k) & 0x01) - 1;
				}
			}
			ColorTransValid = trueblnr;
		}

		UpdateBWLuminanceCopy(top, left, bottom, right);
	}
}

LOCALVAR AGLContext ctx = NULL;
LOCALVAR short GLhOffset;
LOCALVAR short GLvOffset;

#ifndef UseAGLdoublebuff
#define UseAGLdoublebuff 0
#endif

LOCALPROC MyDrawWithOpenGL(ui4r top, ui4r left, ui4r bottom, ui4r right)
{
	if (NULL == ctx) {
		/* oops */
	} else if (GL_TRUE != aglSetCurrentContext(ctx)) {
		/* err = aglReportError() */
	} else {
		si4b top2;
		si4b left2;

#if UseAGLdoublebuff
		/* redraw all */
		top = 0;
		left = 0;
		bottom = vMacScreenHeight;
		right = vMacScreenWidth;
#endif

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

		top2 = top;
		left2 = left;

#if VarFullScreen
		if (UseFullScreen)
#endif
#if MayFullScreen
		{
			left2 -= ViewHStart;
			top2 -= ViewVStart;
		}
#endif

#if EnableMagnify
		if (UseMagnify) {
			top2 *= MyWindowScale;
			left2 *= MyWindowScale;
		}
#endif

#if 0
		glClear(GL_COLOR_BUFFER_BIT);
		glBitmap(vMacScreenWidth,
			vMacScreenHeight,
			0,
			0,
			0,
			0,
			(const GLubyte *)GetCurDrawBuff());
#endif
#if 1
		UpdateLuminanceCopy(top, left, bottom, right);
		glRasterPos2i(GLhOffset + left2, GLvOffset - top2);
#if 0 != vMacScreenDepth
		if (UseColorMode) {
			glDrawPixels(right - left,
				bottom - top,
				GL_RGBA,
				GL_UNSIGNED_INT_8_8_8_8,
				ScalingBuff + (left + top * vMacScreenWidth) * 4
				);
		} else
#endif
		{
			glDrawPixels(right - left,
				bottom - top,
				GL_LUMINANCE,
				GL_UNSIGNED_BYTE,
				ScalingBuff + (left + top * vMacScreenWidth)
				);
		}
#endif

#if 0 /* a very quick and dirty check of where drawing */
		glDrawPixels(right - left,
			1,
			GL_RED,
			GL_UNSIGNED_BYTE,
			ScalingBuff + (left + top * vMacScreenWidth)
			);

		glDrawPixels(1,
			bottom - top,
			GL_RED,
			GL_UNSIGNED_BYTE,
			ScalingBuff + (left + top * vMacScreenWidth)
			);
#endif

#if UseAGLdoublebuff
		aglSwapBuffers(ctx);
#else
		glFlush();
#endif
	}

#if MayFullScreen
label_exit:
	;
#endif
}

LOCALPROC Update_Screen(void)
{
	MyDrawWithOpenGL(0, 0, vMacScreenHeight, vMacScreenWidth);
}

LOCALPROC MyDrawChangesAndClear(void)
{
	if (ScreenChangedBottom > ScreenChangedTop) {
		MyDrawWithOpenGL(ScreenChangedTop, ScreenChangedLeft,
			ScreenChangedBottom, ScreenChangedRight);
		ScreenClearChanges();
	}
}


LOCALVAR blnr MouseIsOutside = falseblnr;
	/*
		MouseIsOutside true if sure mouse outside our window. If in
		our window, or not sure, set false.
	*/

LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC MousePositionNotify(Point NewMousePos)
{
	/*
		Not MouseIsOutside includes in the title bar, etc, so have
		to check if in content area.
	*/

	Rect r;
	blnr ShouldHaveCursorHidden = ! MouseIsOutside;

	GetWindowBounds(gMyMainWindow, kWindowContentRgn, &r);

	NewMousePos.h -= r.left;
	NewMousePos.v -= r.top;

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewMousePos.h -= hOffset;
		NewMousePos.v -= vOffset;
	}
#endif

#if EnableMagnify
	if (UseMagnify) {
		NewMousePos.h /= MyWindowScale;
		NewMousePos.v /= MyWindowScale;
	}
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewMousePos.h += ViewHStart;
		NewMousePos.v += ViewVStart;
	}
#endif

#if EnableFSMouseMotion
	if (HaveMouseMotion) {
		MyMousePositionSetDelta(NewMousePos.h - SavedMouseH,
			NewMousePos.v - SavedMouseV);
		SavedMouseH = NewMousePos.h;
		SavedMouseV = NewMousePos.v;
	} else
#endif
	{
		if (NewMousePos.h < 0) {
			NewMousePos.h = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePos.h >= vMacScreenWidth) {
			NewMousePos.h = vMacScreenWidth - 1;
			ShouldHaveCursorHidden = falseblnr;
		}
		if (NewMousePos.v < 0) {
			NewMousePos.v = 0;
			ShouldHaveCursorHidden = falseblnr;
		} else if (NewMousePos.v >= vMacScreenHeight) {
			NewMousePos.v = vMacScreenHeight - 1;
			ShouldHaveCursorHidden = falseblnr;
		}

		/* if (ShouldHaveCursorHidden || CurMouseButton) */
		/*
			for a game like arkanoid, would like mouse to still
			move even when outside window in one direction
		*/
		MyMousePositionSet(NewMousePos.h, NewMousePos.v);
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

LOCALPROC CheckMouseState(void)
{
	Point NewMousePos;
	GetGlobalMouse(&NewMousePos);
		/*
			Deprecated, but haven't found usable replacement.
			Between window deactivate and then reactivate,
			mouse can move without getting kEventMouseMoved.
			Also no way to get initial position.
			(Also don't get kEventMouseMoved after
			using menu bar. Or while using menubar, but
			that isn't too important.)
		*/
	MousePositionNotify(NewMousePos);
}

LOCALVAR blnr gLackFocusFlag = falseblnr;
LOCALVAR blnr gWeAreActive = falseblnr;

GLOBALOSGLUPROC DoneWithDrawingForTick(void)
{
#if EnableFSMouseMotion
	if (HaveMouseMotion) {
		AutoScrollScreen();
	}
#endif
	MyDrawChangesAndClear();
}

LOCALVAR blnr CurSpeedStopped = trueblnr;

/* --- keyboard --- */

LOCALVAR UInt32 SavedModifiers = 0;

LOCALPROC MyUpdateKeyboardModifiers(UInt32 theModifiers)
{
	UInt32 ChangedModifiers = theModifiers ^ SavedModifiers;

	if (0 != ChangedModifiers) {
		if (0 != (ChangedModifiers & shiftKey)) {
			Keyboard_UpdateKeyMap2(MKC_formac_Shift,
				(shiftKey & theModifiers) != 0);
		}
		if (0 != (ChangedModifiers & cmdKey)) {
			Keyboard_UpdateKeyMap2(MKC_formac_Command,
				(cmdKey & theModifiers) != 0);
		}
		if (0 != (ChangedModifiers & alphaLock)) {
			Keyboard_UpdateKeyMap2(MKC_formac_CapsLock,
				(alphaLock & theModifiers) != 0);
		}
		if (0 != (ChangedModifiers & optionKey)) {
			Keyboard_UpdateKeyMap2(MKC_formac_Option,
				(optionKey & theModifiers) != 0);
		}
		if (0 != (ChangedModifiers & controlKey)) {
			Keyboard_UpdateKeyMap2(MKC_formac_Control,
				(controlKey & theModifiers) != 0);
		}

		SavedModifiers = theModifiers;
	}
}

LOCALPROC ReconnectKeyCodes3(void)
{
	/*
		turn off any modifiers (other than alpha)
		that were turned on by drag and drop,
		unless still being held down.
	*/

	UInt32 theModifiers = GetCurrentKeyModifiers();

	MyUpdateKeyboardModifiers(theModifiers
		& (SavedModifiers | alphaLock));

	SavedModifiers = theModifiers;
}

/* --- display utilities --- */

/* DoForEachDisplay adapted from Apple Technical Q&A QA1168 */

typedef void
(*ForEachDisplayProcPtr) (CGDirectDisplayID display);

LOCALPROC DoForEachDisplay0(CGDisplayCount dspCount,
	CGDirectDisplayID *displays, ForEachDisplayProcPtr p)
{
	CGDisplayCount i;

	if (noErr == MyCGGetActiveDisplayList(dspCount,
		displays, &dspCount))
	{
		for (i = 0; i < dspCount; ++i) {
			p(displays[i]);
		}
	}
}

LOCALPROC DoForEachDisplay(ForEachDisplayProcPtr p)
{
	CGDisplayCount dspCount = 0;

	if (HaveMyCGGetActiveDisplayList()
		&& (noErr == MyCGGetActiveDisplayList(0, NULL, &dspCount)))
	{
		if (dspCount <= 2) {
			CGDirectDisplayID displays[2];
			DoForEachDisplay0(dspCount, displays, p);
		} else {
			CGDirectDisplayID *displays =
				calloc((size_t)dspCount, sizeof(CGDirectDisplayID));
			if (NULL != displays) {
				DoForEachDisplay0(dspCount, displays, p);
				free(displays);
			}
		}
	}
}

LOCALVAR void *datp;

LOCALPROC MyMainDisplayIDProc(CGDirectDisplayID display)
{
	CGDirectDisplayID *p = (CGDirectDisplayID *)datp;

	if (kCGNullDirectDisplay == *p) {
		*p = display;
	}
}

LOCALFUNC CGDirectDisplayID MyMainDisplayID(void)
{
	if (HaveMyCGMainDisplayID()) {
		return MyCGMainDisplayID();
	} else {
		/* for before OS X 10.2 */
		CGDirectDisplayID r = kCGNullDirectDisplay;
		void *savedatp = datp;
		datp = (void *)&r;
		DoForEachDisplay(MyMainDisplayIDProc);
		datp = savedatp;
		return r;
	}
}

/* --- cursor hiding --- */

#if 0
LOCALPROC MyShowCursorProc(CGDirectDisplayID display)
{
	(void) CGDisplayShowCursor(display);
}
#endif

LOCALPROC MyShowCursor(void)
{
#if 0
	/* ShowCursor(); deprecated */
	DoForEachDisplay(MyShowCursorProc);
#endif
	if (HaveMyCGDisplayShowCursor()) {
		(void) MyCGDisplayShowCursor(MyMainDisplayID());
			/* documentation now claims argument ignored */
	}
}

#if 0
LOCALPROC MyHideCursorProc(CGDirectDisplayID display)
{
	(void) CGDisplayHideCursor(display);
}
#endif

LOCALPROC MyHideCursor(void)
{
#if 0
	/* HideCursor(); deprecated */
	DoForEachDisplay(MyHideCursorProc);
#endif
	if (HaveMyCGDisplayHideCursor()) {
		(void) MyCGDisplayHideCursor(MyMainDisplayID());
			/* documentation now claims argument ignored */
	}
}

LOCALVAR blnr HaveCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		MyShowCursor();
	}
}

/* --- cursor moving --- */

LOCALPROC SetCursorArrow(void)
{
	SetThemeCursor(kThemeArrowCursor);
}

#if EnableMoveMouse
LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	Point CurMousePos;
	Rect r;

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

	GetWindowBounds(gMyMainWindow, kWindowContentRgn, &r);
	CurMousePos.h = r.left + h;
	CurMousePos.v = r.top + v;

	/*
		This method from SDL_QuartzWM.m, "Simple DirectMedia Layer",
		Copyright (C) 1997-2003 Sam Lantinga
	*/
	if (HaveMyCGSetLocalEventsSuppressionInterval()) {
		if (noErr != MyCGSetLocalEventsSuppressionInterval(0.0)) {
			/* don't use MacMsg which can call MyMoveMouse */
		}
	}
	if (HaveMyCGWarpMouseCursorPosition()) {
		CGPoint pt;
		pt.x = CurMousePos.h;
		pt.y = CurMousePos.v;
		if (noErr != MyCGWarpMouseCursorPosition(pt)) {
			/* don't use MacMsg which can call MyMoveMouse */
		}
	}
#if 0
	if (HaveMyCGDisplayMoveCursorToPoint()) {
		CGPoint pt;
		pt.x = CurMousePos.h;
		pt.y = CurMousePos.v;
		if (noErr != MyCGDisplayMoveCursorToPoint(
			MyMainDisplayID(), pt))
		{
			/* don't use MacMsg which can call MyMoveMouse */
		}
	}
#endif

	return trueblnr;
}
#endif

#if EnableFSMouseMotion
LOCALPROC AdjustMouseMotionGrab(void)
{
	if (gMyMainWindow != NULL) {
#if MayFullScreen
		if (GrabMachine) {
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
		} else
#endif
		{
			if (HaveMouseMotion) {
				(void) MyMoveMouse(CurMouseH, CurMouseV);
				HaveMouseMotion = falseblnr;
			}
		}
	}
}
#endif

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

#if 0
LOCALFUNC blnr InitMousePosition(void)
{
	/*
		Since there doesn't seem to be any nondeprecated
		way to get initial cursor position, instead
		start by moving cursor to known position.
	*/

#if VarFullScreen
	if (! UseFullScreen)
#endif
#if MayNotFullScreen
	{
		CurMouseH = 16;
		CurMouseV = 16;
		WantCursorHidden = trueblnr;
		(void) MyMoveMouse(CurMouseH, CurMouseV);
	}
#endif

	return trueblnr;
}
#endif

/* --- time, date, location, part 2 --- */

#include "DATE2SEC.h"

LOCALFUNC blnr InitLocationDat(void)
{
#if AutoLocation || AutoTimeZone
	MachineLocation loc;

	ReadLocation(&loc);
#if AutoLocation
	CurMacLatitude = (ui5b)loc.latitude;
	CurMacLongitude = (ui5b)loc.longitude;
#endif
#if AutoTimeZone
	CurMacDelta = (ui5b)loc.u.gmtDelta;
#endif
#endif

	{
		CFTimeZoneRef tz = CFTimeZoneCopySystem();
		if (tz) {
			/* CFAbsoluteTime */ ATimeBase = CFAbsoluteTimeGetCurrent();
			/* ETimeBase = GetCurrentEventTime(); */
			{
				CFGregorianDate d = CFAbsoluteTimeGetGregorianDate(
					ATimeBase, tz);
				double floorsec = floor(d.second);
				ATimeBase -= (d.second - floorsec);
				/* ETimeBase -= (d.second - floorsec); */
				TimeSecBase = Date2MacSeconds(floorsec,
					d.minute, d.hour,
					d.day, d.month, d.year);

				(void) CheckDateTime();
			}
			CFRelease(tz);
		}
	}

	OnTrueTime = TrueEmulatedTime;

	return trueblnr;
}

LOCALPROC StartUpTimeAdjust(void)
{
	NextTickChangeTime = GetCurrentEventTime() + MyTickDuration;
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

LOCALPROC MySound_Start0(void)
{
	/* Reset variables */
	ThePlayOffset = 0;
	TheFillOffset = 0;
	TheWriteOffset = 0;
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
			NextTickChangeTime += MyTickDuration;
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

LOCALPROC RampSound(tpSoundSamp p,
	trSoundSamp BeginVal, trSoundSamp EndVal)
{
	int i;
	ui5r v = (((ui5r)BeginVal) << kLnOneBuffLen) + (kLnOneBuffLen >> 1);

	for (i = kOneBuffLen; --i >= 0; ) {
		*p++ = v >> kLnOneBuffLen;
		v = v + EndVal - BeginVal;
	}
}

#if 4 == kLn2SoundSampSz
#define ConvertSoundSampleFromNative(v) ((v) + 0x8000)
#else
#define ConvertSoundSampleFromNative(v) (v)
#endif

struct MySoundR {
	tpSoundSamp fTheSoundBuffer;
	volatile ui4b (*fPlayOffset);
	volatile ui4b (*fFillOffset);
	volatile ui4b (*fMinFilledSoundBuffs);

	volatile blnr PlayingBuffBlock;
	volatile trSoundSamp lastv;
	volatile blnr wantplaying;
	volatile blnr StartingBlocks;

	CmpSoundHeader /* ExtSoundHeader */ soundHeader;
};
typedef struct MySoundR   MySoundR;


/*
	Some of this code descended from CarbonSndPlayDB, an
	example from Apple, as found being used in vMac for Mac OS.
*/

LOCALPROC InsertSndDoCommand(SndChannelPtr chan, SndCommand * newCmd)
{
	if (-1 == chan->qHead) {
		chan->qHead = chan->qTail;
	}

	if (1 <= chan->qHead) {
		chan->qHead--;
	} else {
		chan->qHead = chan->qTail;
	}

	chan->queue[chan->qHead] = *newCmd;
}

/* call back */ static pascal void
MySound_CallBack(SndChannelPtr theChannel, SndCommand * theCallBackCmd)
{
	MySoundR *datp =
		(MySoundR *)(theCallBackCmd->param2);
	blnr wantplaying0 = datp->wantplaying;
	trSoundSamp v0 = datp->lastv;

#if dbglog_SoundStuff
	dbglog_writeln("Enter MySound_CallBack");
#endif

	if (datp->PlayingBuffBlock) {
		/* finish with last sample */
#if dbglog_SoundStuff
		dbglog_writeln("done with sample");
#endif

		*datp->fPlayOffset += kOneBuffLen;
		datp->PlayingBuffBlock = falseblnr;
	}

	if ((! wantplaying0) && (kCenterSound == v0)) {
#if dbglog_SoundStuff
		dbglog_writeln("terminating");
#endif
	} else {
		SndCommand playCmd;
		tpSoundSamp p;
		trSoundSamp v1 = v0;
		blnr WantRamp = falseblnr;
		ui4b CurPlayOffset = *datp->fPlayOffset;
		ui4b ToPlayLen = *datp->fFillOffset - CurPlayOffset;
		ui4b FilledSoundBuffs = ToPlayLen >> kLnOneBuffLen;

		if (FilledSoundBuffs < *datp->fMinFilledSoundBuffs) {
			*datp->fMinFilledSoundBuffs = FilledSoundBuffs;
		}

		if (! wantplaying0) {
#if dbglog_SoundStuff
			dbglog_writeln("playing end transistion");
#endif
			v1 = kCenterSound;

			WantRamp = trueblnr;
		} else
		if (datp->StartingBlocks) {
#if dbglog_SoundStuff
			dbglog_writeln("playing start block");
#endif

			if ((ToPlayLen >> kLnOneBuffLen) < 12) {
				datp->StartingBlocks = falseblnr;
#if dbglog_SoundStuff
				dbglog_writeln("have enough samples to start");
#endif

				p = datp->fTheSoundBuffer
					+ (CurPlayOffset & kAllBuffMask);
				v1 = ConvertSoundSampleFromNative(*p);
			}

			WantRamp = trueblnr;
		} else
		if (0 == FilledSoundBuffs) {
#if dbglog_SoundStuff
			dbglog_writeln("playing under run");
#endif

			WantRamp = trueblnr;
		} else
		{
			/* play next sample */
			p = datp->fTheSoundBuffer
				+ (CurPlayOffset & kAllBuffMask);
			datp->PlayingBuffBlock = trueblnr;
			v1 =
				ConvertSoundSampleFromNative(*(p + kOneBuffLen - 1));
#if dbglog_SoundStuff
			dbglog_writeln("playing sample");
#endif
		}

		if (WantRamp) {
			p = datp->fTheSoundBuffer + kAllBuffLen;

#if dbglog_SoundStuff
			dbglog_writelnNum("v0", v0);
			dbglog_writelnNum("v1", v1);
#endif

			RampSound(p, v0, v1);
			ConvertSoundBlockToNative(p);
		}

		datp->soundHeader.samplePtr = (Ptr)p;
		datp->soundHeader.numFrames =
			(unsigned long)kOneBuffLen;

		/* Insert our callback command */
		InsertSndDoCommand (theChannel, theCallBackCmd);

#if 0
		{
			int i;
			tpSoundSamp pS =
				(tpSoundSamp)datp->soundHeader.samplePtr;

			for (i = datp->soundHeader.numFrames; --i >= 0; )
			{
				fprintf(stderr, "%d\n", *pS++);
			}
		}
#endif

		/* Play the next buffer */
		playCmd.cmd = bufferCmd;
		playCmd.param1 = 0;
		playCmd.param2 = (long)&(datp->soundHeader);
		InsertSndDoCommand (theChannel, &playCmd);

		datp->lastv = v1;
	}
}

LOCALVAR MySoundR cur_audio;

LOCALVAR SndCallBackUPP gCarbonSndPlayDoubleBufferCallBackUPP = NULL;

LOCALVAR SndChannelPtr sndChannel = NULL; /* our sound channel */

LOCALPROC MySound_Start(void)
{
#if dbglog_SoundStuff
	dbglog_writeln("MySound_Start");
#endif

	if (NULL == sndChannel) {
		SndCommand callBack;
		SndChannelPtr chan = NULL;

		cur_audio.wantplaying = falseblnr;
		cur_audio.StartingBlocks = falseblnr;

		MySound_Start0();

		SndNewChannel(&chan, sampledSynth, initMono, nil);
		if (NULL != chan) {
			sndChannel = chan;

			cur_audio.PlayingBuffBlock = falseblnr;
			cur_audio.lastv = kCenterSound;
			cur_audio.StartingBlocks = trueblnr;
			cur_audio.wantplaying = trueblnr;

			callBack.cmd = callBackCmd;
			callBack.param1 = 0; /* unused */
			callBack.param2 = (long)&cur_audio;

			sndChannel->callBack =
				gCarbonSndPlayDoubleBufferCallBackUPP;

			(void) SndDoCommand (sndChannel, &callBack, true);
		}
	}
}

LOCALPROC MySound_Stop(void)
{
#if dbglog_SoundStuff
	dbglog_writeln("enter MySound_Stop");
#endif

	if (NULL != sndChannel) {
		ui4r retry_limit = 50; /* half of a second */
		SCStatus r;

		cur_audio.wantplaying = falseblnr;
#if dbglog_SoundStuff
		dbglog_writeln("cleared wantplaying");
#endif

label_retry:
		r.scChannelBusy = falseblnr; /* what is this for? */
		if (noErr != SndChannelStatus(sndChannel,
			sizeof(SCStatus), &r))
		{
			/* fail */
		} else
		if ((! r.scChannelBusy) && (kCenterSound == cur_audio.lastv)) {
			/* done */

			/*
				observed reporting not busy unexpectedly,
				so also check lastv.
			*/
		} else
		if (0 == --retry_limit) {
#if dbglog_SoundStuff
			dbglog_writeln("retry limit reached");
#endif
			/*
				don't trust SndChannelStatus, make
				sure don't get in infinite loop.
			*/

			/* done */
		} else
		{
			/*
				give time back, particularly important
				if got here on a suspend event.
			*/
			struct timespec rqt;
			struct timespec rmt;

#if dbglog_SoundStuff
			dbglog_writeln("busy, so sleep");
#endif

			rqt.tv_sec = 0;
			rqt.tv_nsec = 10000000;
			(void) nanosleep(&rqt, &rmt);

			goto label_retry;
		}

		SndDisposeChannel(sndChannel, true);
		sndChannel = NULL;
	}

#if dbglog_SoundStuff
	dbglog_writeln("leave MySound_Stop");
#endif
}

#define SOUND_SAMPLERATE rate22khz
	/* = 0x56EE8BA3 = (7833600 * 2 / 704) << 16 */

LOCALFUNC blnr MySound_Init(void)
{
#if dbglog_SoundStuff
	dbglog_writeln("enter MySound_Init");
#endif

	cur_audio.fTheSoundBuffer = TheSoundBuffer;

	cur_audio.fPlayOffset = &ThePlayOffset;
	cur_audio.fFillOffset = &TheFillOffset;
	cur_audio.fMinFilledSoundBuffs = &MinFilledSoundBuffs;
	cur_audio.wantplaying = falseblnr;

	/* Init basic per channel information */
	cur_audio.soundHeader.sampleRate = SOUND_SAMPLERATE;
		/* sample rate */
	cur_audio.soundHeader.numChannels = 1; /* one channel */
	cur_audio.soundHeader.loopStart = 0;
	cur_audio.soundHeader.loopEnd = 0;
	cur_audio.soundHeader.encode = cmpSH /* extSH */;
	cur_audio.soundHeader.baseFrequency = kMiddleC;
	cur_audio.soundHeader.numFrames =
		(unsigned long)kOneBuffLen;
	/* cur_audio.soundHeader.AIFFSampleRate = 0; */
		/* unused */
	cur_audio.soundHeader.markerChunk = nil;
	cur_audio.soundHeader.futureUse2 = 0;
	cur_audio.soundHeader.stateVars = nil;
	cur_audio.soundHeader.leftOverSamples = nil;
	cur_audio.soundHeader.compressionID = 0;
		/* no compression */
	cur_audio.soundHeader.packetSize = 0;
		/* no compression */
	cur_audio.soundHeader.snthID = 0;
	cur_audio.soundHeader.sampleSize = (1 << kLn2SoundSampSz);
		/* 8 or 16 bits per sample */
	cur_audio.soundHeader.sampleArea[0] = 0;
#if 3 == kLn2SoundSampSz
	cur_audio.soundHeader.format = kSoundNotCompressed;
#elif 4 == kLn2SoundSampSz
	cur_audio.soundHeader.format = k16BitNativeEndianFormat;
#else
#error "unsupported kLn2SoundSampSz"
#endif
	cur_audio.soundHeader.samplePtr = (Ptr)TheSoundBuffer;

	gCarbonSndPlayDoubleBufferCallBackUPP =
		NewSndCallBackUPP(MySound_CallBack);
	if (gCarbonSndPlayDoubleBufferCallBackUPP != NULL) {

		MySound_Start();
			/*
				This should be taken care of by LeaveSpeedStopped,
				but since takes a while to get going properly,
				start early.
			*/

		return trueblnr;
	}
	return falseblnr;
}

GLOBALOSGLUPROC MySound_EndWrite(ui4r actL)
{
	if (MySound_EndWrite0(actL)) {
	}
}

LOCALPROC MySound_SecondNotify(void)
{
	if (sndChannel != NULL) {
		MySound_SecondNotify0();
	}
}

#endif


LOCALPROC MyAdjustGLforSize(int h, int v)
{
	if (GL_TRUE != aglSetCurrentContext(ctx)) {
		/* err = aglReportError() */
	} else {

		glClearColor (0.0, 0.0, 0.0, 1.0);

#if 1
		glViewport(0, 0, h, v);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, h, 0, v, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
#endif

		glColor3f(0.0, 0.0, 0.0);
#if EnableMagnify
		if (UseMagnify) {
			glPixelZoom(MyWindowScale, - MyWindowScale);
		} else
#endif
		{
			glPixelZoom(1, -1);
		}
		glPixelStorei(GL_UNPACK_ROW_LENGTH, vMacScreenWidth);

		glClear(GL_COLOR_BUFFER_BIT);

		if (GL_TRUE != aglSetCurrentContext(NULL)) {
			/* err = aglReportError() */
		}

		ScreenChangedAll();
	}
}

LOCALVAR blnr WantScreensChangedCheck = falseblnr;

LOCALPROC MyUpdateOpenGLContext(void)
{
	if (NULL == ctx) {
		/* oops */
	} else if (GL_TRUE != aglSetCurrentContext(ctx)) {
		/* err = aglReportError() */
	} else {
		aglUpdateContext(ctx);
	}
}

#if 0 /* some experiments */
LOCALVAR CGrafPtr GrabbedPort = NULL;
#endif

#if 0
LOCALPROC AdjustMainScreenGrab(void)
{
	if (GrabMachine) {
		if (NULL == GrabbedPort) {
			/* CGDisplayCapture(MyMainDisplayID()); */
			CGCaptureAllDisplays();
			/* CGDisplayHideCursor( MyMainDisplayID() ); */
			GrabbedPort =
				CreateNewPortForCGDisplayID((UInt32)MyMainDisplayID());
			LockPortBits (GrabbedPort);
		}
	} else {
		if (GrabbedPort != NULL) {
			UnlockPortBits (GrabbedPort);
			/* CGDisplayShowCursor( MyMainDisplayID() ); */
			/* CGDisplayRelease(MyMainDisplayID()); */
			CGReleaseAllDisplays();
			GrabbedPort = NULL;
		}
	}
}
#endif

#if 0
typedef CGDisplayErr (*CGReleaseAllDisplaysProcPtr)
	(void);

LOCALPROC MyReleaseAllDisplays(void)
{
	if (HaveApplicationServicesBun()) {
		CGReleaseAllDisplaysProcPtr ReleaseAllDisplaysProc =
			(CGReleaseAllDisplaysProcPtr)
			CFBundleGetFunctionPointerForName(
				AppServBunRef, CFSTR("CGReleaseAllDisplays"));
		if (ReleaseAllDisplaysProc != NULL) {
			ReleaseAllDisplaysProc();
		}
	}
}

typedef CGDisplayErr (*CGCaptureAllDisplaysProcPtr)
	(void);

LOCALPROC MyCaptureAllDisplays(void)
{
	if (HaveApplicationServicesBun()) {
		CGCaptureAllDisplaysProcPtr CaptureAllDisplaysProc =
			(CGCaptureAllDisplaysProcPtr)
			CFBundleGetFunctionPointerForName(
				AppServBunRef, CFSTR("CGCaptureAllDisplays"));
		if (CaptureAllDisplaysProc != NULL) {
			CaptureAllDisplaysProc();
		}
	}
}
#endif

LOCALVAR AGLPixelFormat window_fmt = NULL;
LOCALVAR AGLContext window_ctx = NULL;

#ifndef UseAGLfullscreen
#define UseAGLfullscreen 0
#endif

#if UseAGLfullscreen
LOCALVAR AGLPixelFormat fullscreen_fmt = NULL;
LOCALVAR AGLContext fullscreen_ctx = NULL;
#endif

#if UseAGLfullscreen
LOCALPROC MaybeFullScreen(void)
{
	if (
#if VarFullScreen
		UseFullScreen &&
#endif
		(NULL == fullscreen_ctx)
		&& HaveMyCGDisplayPixelsWide()
		&& HaveMyCGDisplayPixelsHigh())
	{
		static const GLint fullscreen_attrib[] = {AGL_RGBA,
			AGL_NO_RECOVERY,
			AGL_FULLSCREEN,
			AGL_SINGLE_RENDERER, AGL_ACCELERATED,
#if UseAGLdoublebuff
			AGL_DOUBLEBUFFER,
#endif
			AGL_NONE};
		GDHandle theDevice;
		CGDirectDisplayID CurMainDisplayID = MyMainDisplayID();

		DMGetGDeviceByDisplayID(
			(DisplayIDType)CurMainDisplayID, &theDevice, false);
		fullscreen_fmt = aglChoosePixelFormat(
			&theDevice, 1, fullscreen_attrib);
		if (NULL == fullscreen_fmt) {
			/* err = aglReportError() */
		} else {
			fullscreen_ctx = aglCreateContext(fullscreen_fmt, NULL);
			if (NULL == fullscreen_ctx) {
				/* err = aglReportError() */
			} else {
				/* MyCaptureAllDisplays(); */
				if (GL_TRUE != aglSetFullScreen(fullscreen_ctx,
					0, 0, 0, 0))
				{
					/* err = aglReportError() */
				} else {
					Rect r;

					int h = MyCGDisplayPixelsWide(CurMainDisplayID);
					int v = MyCGDisplayPixelsHigh(CurMainDisplayID);

					GetWindowBounds(gMyMainWindow, kWindowContentRgn,
						&r);

					GLhOffset = r.left + hOffset;
					GLvOffset = v - (r.top + vOffset);

					ctx = fullscreen_ctx;
					MyAdjustGLforSize(h, v);
					return;
				}
				/* MyReleaseAllDisplays(); */

				if (GL_TRUE != aglDestroyContext(fullscreen_ctx)) {
					/* err = aglReportError() */
				}
				fullscreen_ctx = NULL;
			}

			aglDestroyPixelFormat (fullscreen_fmt);
			fullscreen_fmt = NULL;
		}
	}
}
#endif

#if UseAGLfullscreen
LOCALPROC NoFullScreen(void)
{
	if (fullscreen_ctx != NULL) {
		Rect r;
		int h;
		int v;

		GetWindowBounds(gMyMainWindow, kWindowContentRgn, &r);

		h = r.right - r.left;
		v = r.bottom - r.top;

		GLhOffset = hOffset;
		GLvOffset = v - vOffset;

		ctx = window_ctx;

		MyAdjustGLforSize(h, v);

		Update_Screen();

		if (fullscreen_ctx != NULL) {
			if (GL_TRUE != aglDestroyContext(fullscreen_ctx)) {
				/* err = aglReportError() */
			}
			fullscreen_ctx = NULL;
		}
		if (fullscreen_fmt != NULL) {
			aglDestroyPixelFormat(fullscreen_fmt);
			fullscreen_fmt = NULL;
		}
	}
}
#endif

#if UseAGLfullscreen
LOCALPROC AdjustOpenGLGrab(void)
{
	if (GrabMachine) {
		MaybeFullScreen();
	} else {
		NoFullScreen();
	}
}
#endif

#if MayFullScreen
LOCALPROC AdjustMachineGrab(void)
{
#if EnableFSMouseMotion
	AdjustMouseMotionGrab();
#endif
#if UseAGLfullscreen
	AdjustOpenGLGrab();
#endif
#if 0
	AdjustMainScreenGrab();
#endif
}
#endif

#if MayFullScreen
LOCALPROC UngrabMachine(void)
{
	GrabMachine = falseblnr;
	AdjustMachineGrab();
}
#endif

LOCALPROC ClearWeAreActive(void)
{
	if (gWeAreActive) {
		gWeAreActive = falseblnr;

		DisconnectKeyCodes2();

		SavedModifiers &= alphaLock;

		MyMouseButtonSet(falseblnr);

#if MayFullScreen
		UngrabMachine();
#endif

		ForceShowCursor();
	}
}

/* --- basic dialogs --- */

LOCALFUNC CFStringRef CFStringCreateFromSubstCStr(char *s)
{
	int L;
	UniChar x[ClStrMaxLength];

	UniCharStrFromSubstCStr(&L, x, s);

	return CFStringCreateWithCharacters(kCFAllocatorDefault, x, L);
}

#define kMyStandardAlert 128

LOCALPROC CheckSavedMacMsg(void)
{
	/* called only on quit, if error saved but not yet reported */

	if (nullpr != SavedBriefMsg) {
		if (HaveMyCreateStandardAlert() && HaveMyRunStandardAlert()) {
			CFStringRef briefMsgu = CFStringCreateFromSubstCStr(
				SavedBriefMsg);
			if (NULL != briefMsgu) {
				CFStringRef longMsgu = CFStringCreateFromSubstCStr(
					SavedLongMsg);
				if (NULL != longMsgu) {
					DialogRef TheAlert;
					OSStatus err = MyCreateStandardAlert(
						(SavedFatalMsg) ? kAlertStopAlert
							: kAlertCautionAlert,
						briefMsgu, longMsgu, NULL,
						&TheAlert);
					if (noErr == err) {
						(void) MyRunStandardAlert(TheAlert, NULL, NULL);
					}
					CFRelease(longMsgu);
				}
				CFRelease(briefMsgu);
			}
		}

		SavedBriefMsg = nullpr;
	}
}

LOCALVAR blnr gTrueBackgroundFlag = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	ClearWeAreActive();
}

LOCALPROC MyEndDialog(void)
{
}

/* --- hide/show menubar --- */

#if MayFullScreen
LOCALPROC My_HideMenuBar(void)
{
	if (HaveMySetSystemUIMode()) {
		(void) MySetSystemUIMode(MykUIModeAllHidden,
			MykUIOptionDisableAppleMenu
#if GrabKeysFullScreen
			| MykUIOptionDisableProcessSwitch
#if GrabKeysMaxFullScreen /* dangerous !! */
			| MykUIOptionDisableForceQuit
			| MykUIOptionDisableSessionTerminate
#endif
#endif
			);
	} else {
		if (IsMenuBarVisible()) {
			HideMenuBar();
		}
	}
}
#endif

#if MayFullScreen
LOCALPROC My_ShowMenuBar(void)
{
	if (HaveMySetSystemUIMode()) {
		(void) MySetSystemUIMode(MykUIModeNormal,
			0);
	} else {
		if (! IsMenuBarVisible()) {
			ShowMenuBar();
		}
	}
}
#endif

/* --- drives, part 2 --- */

LOCALPROC InitDrives(void)
{
	/*
		This isn't really needed, Drives[i]
		need not have valid value when not vSonyIsInserted[i].
	*/
	tDrive i;

	for (i = 0; i < NumDrives; ++i) {
		Drives[i] = NotAfileRef;
	}
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

LOCALFUNC tMacErr Sony_Insert0(SInt16 refnum, blnr locked)
{
	tDrive Drive_No;

	if (! FirstFreeDisk(&Drive_No)) {
		(void) FSCloseFork(refnum);
		return mnvm_tmfoErr;
	} else {
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);
		return mnvm_noErr;
	}
}

LOCALPROC ReportStandardOpenDiskError(tMacErr err)
{
	if (mnvm_noErr != err) {
		if (mnvm_tmfoErr == err) {
			MacMsg(kStrTooManyImagesTitle,
				kStrTooManyImagesMessage, falseblnr);
		} else if (mnvm_opWrErr == err) {
			MacMsg(kStrImageInUseTitle,
				kStrImageInUseMessage, falseblnr);
		} else {
			MacMsg(kStrOpenFailTitle, kStrOpenFailMessage, falseblnr);
		}
	}
}

LOCALFUNC tMacErr InsertADiskFromFSRef(FSRef *theRef)
{
	tMacErr err;
	HFSUniStr255 forkName;
	SInt16 refnum;
	blnr locked = falseblnr;

	if (CheckSaveMacErr(FSGetDataForkName(&forkName))) {
		err = To_tMacErr(FSOpenFork(theRef, forkName.length,
			forkName.unicode, fsRdWrPerm, &refnum));
		switch (err) {
			case mnvm_permErr:
			case mnvm_wrPermErr:
			case mnvm_afpAccessDenied:
				locked = trueblnr;
				err = To_tMacErr(FSOpenFork(theRef, forkName.length,
					forkName.unicode, fsRdPerm, &refnum));
				break;
			default:
				break;
		}
		if (mnvm_noErr == err) {
			err = Sony_Insert0(refnum, locked);
		}
	}

	return err;
}

LOCALFUNC tMacErr LoadMacRomFromRefNum(SInt16 refnum)
{
	tMacErr err;
	ByteCount actualCount;

	if (mnvm_noErr != (err = To_tMacErr(
		FSReadFork(refnum, fsFromStart, 0,
			kROM_Size, ROM, &actualCount))))
	{
		if (mnvm_eofErr == err) {
			MacMsgOverride(kStrShortROMTitle, kStrShortROMMessage);
		} else {
			MacMsgOverride(kStrNoReadROMTitle, kStrNoReadROMMessage);
		}
	} else
	{
		err = ROM_IsValid();
	}

	return err;
}

LOCALFUNC tMacErr LoadMacRomFromFSRef(FSRef *theRef)
{
	tMacErr err;
	HFSUniStr255 forkName;
	SInt16 refnum;

	if (mnvm_noErr == (err =
		To_tMacErr(FSGetDataForkName(&forkName))))
	if (mnvm_noErr == (err = To_tMacErr(
		FSOpenFork(theRef, forkName.length,
			forkName.unicode, fsRdPerm, &refnum))))
	{
		err = LoadMacRomFromRefNum(refnum);

		(void) FSCloseFork(refnum);
	}

	return err;
}

LOCALFUNC tMacErr InsertADiskFromFSRef1(FSRef *theRef)
{
	tMacErr err;

	if (! ROM_loaded) {
		err = LoadMacRomFromFSRef(theRef);
	} else {
		err = InsertADiskFromFSRef(theRef);
	}

	return err;
}

LOCALFUNC tMacErr InsertDisksFromDocList(AEDescList *docList)
{
	tMacErr err = mnvm_noErr;
	long itemsInList;
	long index;
	AEKeyword keyword;
	DescType typeCode;
	FSRef theRef;
	Size actualSize;

	if (CheckSaveMacErr(AECountItems(docList, &itemsInList))) {
		for (index = 1; index <= itemsInList; ++index) {
			if (CheckSaveMacErr(AEGetNthPtr(docList, index, typeFSRef,
				&keyword, &typeCode, (Ptr)&theRef,
				sizeof(FSRef), &actualSize)))
			if (CheckSavetMacErr(InsertADiskFromFSRef1(&theRef)))
			{
			}
			if (mnvm_noErr != err) {
				goto label_fail;
			}
		}
	}

label_fail:
	return err;
}

LOCALFUNC tMacErr InsertADiskFromNameEtc(FSRef *ParentRef,
	char *fileName)
{
	tMacErr err;
	blnr isFolder;
	FSRef ChildRef;

	if (CheckSavetMacErr(MyMakeFSRefC(ParentRef, fileName,
		&isFolder, &ChildRef)))
	{
		if (isFolder) {
			err = mnvm_miscErr;
		} else {
			if (CheckSavetMacErr(InsertADiskFromFSRef(&ChildRef))) {
				/* ok */
			}
		}
	}

	return err;
}

#if IncludeSonyNew
LOCALFUNC tMacErr WriteZero(SInt16 refnum, ui5b L)
{
#define ZeroBufferSize 2048
	tMacErr err = mnvm_noErr;
	ui5b i;
	ui3b buffer[ZeroBufferSize];
	ByteCount actualCount;
	ui5b offset = 0;

	memset(&buffer, 0, ZeroBufferSize);

	while (L > 0) {
		i = (L > ZeroBufferSize) ? ZeroBufferSize : L;
		if (! CheckSaveMacErr(FSWriteFork(refnum,
			fsFromStart,
			offset,
			i,
			buffer,
			&actualCount)))
		{
			goto label_fail;
		}
		L -= i;
		offset += i;
	}

label_fail:
	return err;
}
#endif

#if IncludeSonyNew
LOCALFUNC tMacErr MyCreateFileCFStringRef(FSRef *saveFileParent,
	CFStringRef saveFileName, FSRef *NewRef)
{
	tMacErr err;
	UniChar buffer[255];

	CFIndex len = CFStringGetLength(saveFileName);

	if (len > 255) {
		len = 255;
	}

	CFStringGetCharacters(saveFileName, CFRangeMake(0, len), buffer);

	err = To_tMacErr(FSMakeFSRefUnicode(saveFileParent, len,
		buffer, kTextEncodingUnknown, NewRef));
	if (mnvm_fnfErr == err) { /* file is not there yet - create it */
		err = To_tMacErr(FSCreateFileUnicode(saveFileParent,
			len, buffer, 0, NULL, NewRef, NULL));
	}

	return err;
}
#endif

#if IncludeSonyNew
LOCALFUNC tMacErr MakeNewDisk0(FSRef *saveFileParent,
	CFStringRef saveFileName, ui5b L)
{
	tMacErr err;
	FSRef NewRef;
	HFSUniStr255 forkName;
	SInt16 refnum;

	if (CheckSavetMacErr(
		MyCreateFileCFStringRef(saveFileParent, saveFileName, &NewRef)))
	{
		if (CheckSaveMacErr(FSGetDataForkName(&forkName)))
		if (CheckSaveMacErr(FSOpenFork(&NewRef, forkName.length,
			forkName.unicode, fsRdWrPerm, &refnum)))
		{
			SInt64 forkSize = L;
			if (CheckSaveMacErr(
				FSSetForkSize(refnum, fsFromStart, forkSize)))
			/*
				zero out fork. It looks like this is already done,
				but documentation says this isn't guaranteed.
			*/
			if (CheckSavetMacErr(WriteZero(refnum, L)))
			{
				err = Sony_Insert0(refnum, falseblnr);
				refnum = NotAfileRef;
			}
			if (NotAfileRef != refnum) {
				(void) FSCloseFork(refnum);
			}
		}
		if (mnvm_noErr != err) {
			(void) FSDeleteObject(&NewRef);
		}
	}

	return err;
}
#endif

#if IncludeSonyNameNew
LOCALFUNC CFStringRef CFStringCreateWithPbuf(tPbuf i)
{
	return CFStringCreateWithBytes(NULL,
		(UInt8 *)PbufDat[i], PbufSize[i],
		kCFStringEncodingMacRoman, false);
}
#endif

pascal Boolean NavigationFilterProc(
	AEDesc* theItem, void* info, void* NavCallBackUserData,
	NavFilterModes theNavFilterModes);
pascal Boolean NavigationFilterProc(
	AEDesc* theItem, void* info, void* NavCallBackUserData,
	NavFilterModes theNavFilterModes)
{
	/* NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*)info; */
	UnusedParam(theItem);
	UnusedParam(info);
	UnusedParam(theNavFilterModes);
	UnusedParam(NavCallBackUserData);

	return true; /* display all items */
}


pascal void NavigationEventProc(
	NavEventCallbackMessage callBackSelector,
	NavCBRecPtr callBackParms, void *NavCallBackUserData);
pascal void NavigationEventProc(
	NavEventCallbackMessage callBackSelector,
	NavCBRecPtr callBackParms, void *NavCallBackUserData)
{
	UnusedParam(NavCallBackUserData);

	switch (callBackSelector) {
		case kNavCBEvent: /* probably not needed in os x */
			switch (callBackParms->eventData.eventDataParms.event->what)
			{
				case updateEvt:
					{
						WindowPtr which =
							(WindowPtr)callBackParms
								->eventData.eventDataParms.event
								->message;

						BeginUpdate(which);

						if (which == gMyMainWindow) {
							Update_Screen();
						}

						EndUpdate(which);
					}
					break;
			}
			break;
#if 0
		case kNavCBUserAction:
			{
				NavUserAction userAction = NavDialogGetUserAction(
					callBackParms->context);
				switch (userAction) {
					case kNavUserActionOpen: {
						/* got something to open */
						break;
					}
				}
			}
			break;
		case kNavCBTerminate:
			break;
#endif
	}
}

LOCALPROC InsertADisk0(void)
{
	NavDialogRef theOpenDialog;
	NavDialogCreationOptions dialogOptions;
	NavReplyRecord theReply;
	NavTypeListHandle openList = NULL;
	NavEventUPP gEventProc = NewNavEventUPP(NavigationEventProc);
	NavObjectFilterUPP filterUPP =
		NewNavObjectFilterUPP(NavigationFilterProc);

	if (noErr == NavGetDefaultDialogCreationOptions(&dialogOptions)) {
		dialogOptions.modality = kWindowModalityAppModal;
		dialogOptions.optionFlags |= kNavDontAutoTranslate;
		dialogOptions.optionFlags &= ~ kNavAllowPreviews;
		if (noErr == NavCreateGetFileDialog(&dialogOptions,
			(NavTypeListHandle)openList,
			gEventProc, NULL,
			filterUPP, NULL, &theOpenDialog))
		{
			MyBeginDialog();
			(void) NavDialogRun(theOpenDialog);
			MyEndDialog();
			if (noErr == NavDialogGetReply(theOpenDialog,
				&theReply))
			{
				if (theReply.validRecord) {
					ReportStandardOpenDiskError(
						InsertDisksFromDocList(&theReply.selection));
				}
				(void) NavDisposeReply(&theReply);
			}
			NavDialogDispose(theOpenDialog);
		}
	}

	DisposeNavEventUPP(gEventProc);
	DisposeNavObjectFilterUPP(filterUPP);
}

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, CFStringRef NewDiskName)
{
#if SaveDialogEnable
	NavDialogRef theSaveDialog;
	NavDialogCreationOptions dialogOptions;
	NavReplyRecord theReply;
	NavEventUPP gEventProc = NewNavEventUPP(NavigationEventProc);

	if (noErr == NavGetDefaultDialogCreationOptions(&dialogOptions)) {
		dialogOptions.modality = kWindowModalityAppModal;
		dialogOptions.saveFileName = NewDiskName;
		if (noErr == NavCreatePutFileDialog(&dialogOptions,
			'TEXT', 'MPS ',
			gEventProc, NULL,
			&theSaveDialog))
		{
			MyBeginDialog();
			(void) NavDialogRun(theSaveDialog);
			MyEndDialog();
			if (noErr == NavDialogGetReply(theSaveDialog,
				&theReply))
			{
				if (theReply.validRecord) {
					long itemsInList;
					AEKeyword keyword;
					DescType typeCode;
					FSRef theRef;
					Size actualSize;

					if (noErr == AECountItems(
						&theReply.selection, &itemsInList))
					{
						if (itemsInList == 1) {
							if (noErr == AEGetNthPtr(
								&theReply.selection, 1, typeFSRef,
								&keyword, &typeCode, (Ptr)&theRef,
								sizeof(FSRef), &actualSize))
							{
								ReportStandardOpenDiskError(
									MakeNewDisk0(&theRef,
										theReply.saveFileName, L));
							}
						}
					}
				}
				(void) NavDisposeReply(&theReply);
			}
			NavDialogDispose(theSaveDialog);
		}
	}

	DisposeNavEventUPP(gEventProc);
#else /* SaveDialogEnable */
	FSRef OutRef;

	if (mnvm_noErr == FindOrMakeNamedChildRef(&MyDatDirRef,
		"out", &OutRef))
	{
		MakeNewDisk0(&OutRef, NewDiskName, L);
	}
#endif /* SaveDialogEnable */
}
#endif

LOCALFUNC tMacErr LoadMacRomFromNameFolder(FSRef *ParentRef,
	char *fileName)
{
	tMacErr err;
	blnr isFolder;
	FSRef ChildRef;

	if (mnvm_noErr == (err =
		MyMakeFSRefC(ParentRef, fileName, &isFolder, &ChildRef)))
	if (mnvm_noErr == (err =
		LoadMacRomFromFSRef(&ChildRef)))
	{
	}

	return err;
}

LOCALFUNC tMacErr LoadMacRomFromPrefDir(void)
{
	tMacErr err;
	FSRef PrefRef;
	FSRef GryphelRef;
	FSRef ROMsRef;

	if (mnvm_noErr == (err = To_tMacErr(FSFindFolder(kUserDomain,
		kPreferencesFolderType, kDontCreateFolder, &PrefRef))))
	if (mnvm_noErr == (err = FindNamedChildRef(&PrefRef,
		"Gryphel", &GryphelRef)))
	if (mnvm_noErr == (err = FindNamedChildRef(&GryphelRef,
		"mnvm_rom", &ROMsRef)))
	if (mnvm_noErr == (err = LoadMacRomFromNameFolder(&ROMsRef,
		RomFileName)))
	{
		/* ok */
	}

	return err;
}

LOCALFUNC blnr LoadMacRom(void)
{
	tMacErr err;

	if (mnvm_fnfErr == (err =
		LoadMacRomFromNameFolder(&MyDatDirRef, RomFileName)))
	if (mnvm_fnfErr == (err =
		LoadMacRomFromPrefDir()))
	{
	}

	return trueblnr; /* keep launching Mini vMac, regardless */
}

LOCALFUNC blnr Sony_InsertIth(int i)
{
	tMacErr err = mnvm_noErr;

	if ((i > 9) || ! FirstFreeDisk(nullpr)) {
		return falseblnr;
	} else {
		char s[16] = "disk?.dsk";

		s[4] = '0' + i;

		if (! CheckSavetMacErr(InsertADiskFromNameEtc(&MyDatDirRef, s)))
		{
			if (mnvm_fnfErr != err) {
				ReportStandardOpenDiskError(err);
			}
			/* stop on first error (including file not found) */
			return falseblnr;
		}
	}

	return trueblnr;
}

LOCALFUNC blnr LoadInitialImages(void)
{
	int i;

	for (i = 1; Sony_InsertIth(i); ++i) {
		/* stop on first error (including file not found) */
	}

	return trueblnr;
}

#if UseActvFile

#define ActvCodeFileName "act_1"

LOCALFUNC tMacErr OpenActvCodeFile(SInt16 *refnum)
{
	tMacErr err;
	FSRef PrefRef;
	FSRef GryphelRef;
	FSRef ActRef;

	if (CheckSaveMacErr(FSFindFolder(kUserDomain,
		kPreferencesFolderType, kDontCreateFolder, &PrefRef)))
	if (CheckSavetMacErr(FindNamedChildRef(&PrefRef,
		"Gryphel", &GryphelRef)))
	if (CheckSavetMacErr(FindNamedChildRef(&GryphelRef,
		"mnvm_act", &ActRef)))
	if (CheckSavetMacErr(OpenNamedFileInFolderRef(&ActRef,
		ActvCodeFileName, refnum)))
	{
		/* ok */
	}

	return err;
}

LOCALFUNC tMacErr ActvCodeFileLoad(ui3p p)
{
	tMacErr err;
	SInt16 refnum;

	if (CheckSavetMacErr(OpenActvCodeFile(&refnum))) {
		ByteCount actualCount;
		err = To_tMacErr(FSReadFork(refnum, fsFromStart, 0,
			ActvCodeFileLen, p, &actualCount));
		(void) FSCloseFork(refnum);
	}

	return err;
}

LOCALFUNC tMacErr ActvCodeFileSave(ui3p p)
{
	tMacErr err;
	SInt16 refnum;
	FSRef PrefRef;
	FSRef GryphelRef;
	FSRef ActRef;
	ByteCount count = ActvCodeFileLen;

	if (CheckSaveMacErr(FSFindFolder(kUserDomain,
		kPreferencesFolderType, kDontCreateFolder, &PrefRef)))
	if (CheckSavetMacErr(FindOrMakeNamedChildRef(&PrefRef,
		"Gryphel", &GryphelRef)))
	if (CheckSavetMacErr(FindOrMakeNamedChildRef(&GryphelRef,
		"mnvm_act", &ActRef)))
	if (CheckSavetMacErr(OpenWriteNamedFileInFolderRef(&ActRef,
		ActvCodeFileName, &refnum)))
	{
		ByteCount actualCount;
		err = To_tMacErr(FSWriteFork(refnum, fsFromStart, 0,
			count, p, &actualCount));
		(void) FSCloseFork(refnum);
	}

	return err;
}

#endif /* UseActvFile */

/* --- utilities for adapting to the environment --- */

LOCALPROC MyGetScreenBitsBounds(Rect *r)
{
	if (HaveMyCGDisplayBounds()) {
		CGRect cgr = MyCGDisplayBounds(MyMainDisplayID());

		r->left = cgr.origin.x;
		r->top = cgr.origin.y;
		r->right = cgr.origin.x + cgr.size.width;
		r->bottom = cgr.origin.y + cgr.size.height;
	}
}

#if MayNotFullScreen
LOCALPROC InvalWholeWindow(WindowRef mw)
{
	Rect bounds;

	GetWindowPortBounds(mw, &bounds);
	InvalWindowRect(mw, &bounds);
}
#endif

#if MayNotFullScreen
LOCALPROC MySetMacWindContRect(WindowRef mw, Rect *r)
{
	(void) SetWindowBounds (mw, kWindowContentRgn, r);
	InvalWholeWindow(mw);
}
#endif

#if MayNotFullScreen
LOCALFUNC blnr MyGetWindowTitleBounds(WindowRef mw, Rect *r)
{
	return (noErr == GetWindowBounds(mw, kWindowTitleBarRgn, r));
}
#endif

#if EnableRecreateW && MayNotFullScreen
LOCALFUNC blnr MyGetWindowContBounds(WindowRef mw, Rect *r)
{
	return (noErr == GetWindowBounds(mw, kWindowContentRgn, r));
}
#endif

LOCALPROC MyGetGrayRgnBounds(Rect *r)
{
	GetRegionBounds(GetGrayRgn(), (Rect *)r);
}

#define openOnly 1
#define openPrint 2

LOCALFUNC blnr GotRequiredParams(AppleEvent *theAppleEvent)
{
	DescType typeCode;
	Size actualSize;
	OSErr theErr;

	theErr = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr,
				typeWildCard, &typeCode, NULL, 0, &actualSize);
	if (errAEDescNotFound == theErr) { /* No more required params. */
		return trueblnr;
	} else if (noErr == theErr) { /* More required params! */
		return /* CheckSysCode(errAEEventNotHandled) */ falseblnr;
	} else { /* Unexpected Error! */
		return /* CheckSysCode(theErr) */ falseblnr;
	}
}

LOCALFUNC blnr GotRequiredParams0(AppleEvent *theAppleEvent)
{
	DescType typeCode;
	Size actualSize;
	OSErr theErr;

	theErr = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr,
				typeWildCard, &typeCode, NULL, 0, &actualSize);
	if (errAEDescNotFound == theErr) { /* No more required params. */
		return trueblnr;
	} else if (noErr == theErr) { /* More required params! */
		return trueblnr; /* errAEEventNotHandled; */ /*^*/
	} else { /* Unexpected Error! */
		return /* CheckSysCode(theErr) */ falseblnr;
	}
}

/* call back */ static pascal OSErr OpenOrPrintFiles(
	AppleEvent *theAppleEvent, AppleEvent *reply, long aRefCon)
{
	/*
		Adapted from IM VI: AppleEvent Manager:
		Handling Required AppleEvents
	*/
	AEDescList docList;

	UnusedParam(reply);
	UnusedParam(aRefCon);
	/* put the direct parameter (a list of descriptors) into docList */
	if (noErr == (AEGetParamDesc(theAppleEvent, keyDirectObject,
		typeAEList, &docList)))
	{
		if (GotRequiredParams0(theAppleEvent)) {
			/* Check for missing required parameters */
			/* printIt = (openPrint == aRefCon) */
			ReportStandardOpenDiskError(
				InsertDisksFromDocList(&docList));
		}
		/* vCheckSysCode */ (void) (AEDisposeDesc(&docList));
	}

	return /* GetASysResultCode() */ 0;
}

/* call back */ static pascal OSErr DoOpenEvent(
	AppleEvent *theAppleEvent, AppleEvent *reply, long aRefCon)
/*
	This is the alternative to getting an
	open document event on startup.
*/
{
	UnusedParam(reply);
	UnusedParam(aRefCon);
	if (GotRequiredParams0(theAppleEvent)) {
	}
	return /* GetASysResultCode() */ 0;
		/* Make sure there are no additional "required" parameters. */
}


/* call back */ static pascal OSErr DoQuitEvent(
	AppleEvent *theAppleEvent, AppleEvent *reply, long aRefCon)
{
	UnusedParam(reply);
	UnusedParam(aRefCon);
	if (GotRequiredParams(theAppleEvent)) {
		RequestMacOff = trueblnr;
	}

	return /* GetASysResultCode() */ 0;
}

LOCALFUNC blnr MyInstallEventHandler(AEEventClass theAEEventClass,
	AEEventID theAEEventID, ProcPtr p,
	long handlerRefcon, blnr isSysHandler)
{
	return noErr == (AEInstallEventHandler(theAEEventClass,
		theAEEventID, NewAEEventHandlerUPP((AEEventHandlerProcPtr)p),
		handlerRefcon, isSysHandler));
}

LOCALPROC InstallAppleEventHandlers(void)
{
	if (noErr == AESetInteractionAllowed(kAEInteractWithLocal))
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenApplication,
		(ProcPtr)DoOpenEvent, 0, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
		(ProcPtr)OpenOrPrintFiles, openOnly, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEPrintDocuments,
		(ProcPtr)OpenOrPrintFiles, openPrint, falseblnr))
	if (MyInstallEventHandler(kCoreEventClass, kAEQuitApplication,
		(ProcPtr)DoQuitEvent, 0, falseblnr))
	{
	}
}

static pascal OSErr GlobalTrackingHandler(DragTrackingMessage message,
	WindowRef theWindow, void *handlerRefCon, DragReference theDragRef)
{
	RgnHandle hilightRgn;
	Rect Bounds;

	UnusedParam(theWindow);
	UnusedParam(handlerRefCon);
	switch(message) {
		case kDragTrackingEnterWindow:
			hilightRgn = NewRgn();
			if (hilightRgn != NULL) {
				SetScrnRectFromCoords(&Bounds,
					0, 0, vMacScreenHeight, vMacScreenWidth);
				RectRgn(hilightRgn, &Bounds);
				ShowDragHilite(theDragRef, hilightRgn, true);
				DisposeRgn(hilightRgn);
			}
			break;
		case kDragTrackingLeaveWindow:
			HideDragHilite(theDragRef);
			break;
	}

	return noErr;
}

LOCALFUNC tMacErr InsertADiskOrAliasFromFSRef(FSRef *theRef)
{
	tMacErr err;
	Boolean isFolder;
	Boolean isAlias;

	if (CheckSaveMacErr(FSResolveAliasFile(theRef, true,
		&isFolder, &isAlias)))
	{
		err = InsertADiskFromFSRef1(theRef);
	}

	return err;
}

LOCALFUNC tMacErr InsertADiskOrAliasFromSpec(FSSpec *spec)
{
	tMacErr err;
	FSRef newRef;

	if (CheckSaveMacErr(FSpMakeFSRef(spec, &newRef)))
	if (CheckSavetMacErr(InsertADiskOrAliasFromFSRef(&newRef)))
	{
		/* ok */
	}

	return err;
}

static pascal OSErr GlobalReceiveHandler(WindowRef pWindow,
	void *handlerRefCon, DragReference theDragRef)
{
	SInt16 mouseUpModifiers;
	unsigned short items;
	unsigned short index;
	ItemReference theItem;
	Size SentSize;
	HFSFlavor r;

	UnusedParam(pWindow);
	UnusedParam(handlerRefCon);

	if (noErr == GetDragModifiers(theDragRef,
		NULL, NULL, &mouseUpModifiers))
	{
		MyUpdateKeyboardModifiers(mouseUpModifiers);
	}

	if (noErr == CountDragItems(theDragRef, &items)) {
		for (index = 1; index <= items; ++index) {
			if (noErr == GetDragItemReferenceNumber(theDragRef,
				index, &theItem))
			if (noErr == GetFlavorDataSize(theDragRef,
				theItem, flavorTypeHFS, &SentSize))
				/*
					On very old macs SentSize might only be big enough
					to hold the actual file name. Have not seen this
					in OS X, but still leave the check
					as '<=' instead of '=='.
				*/
			if (SentSize <= sizeof(HFSFlavor))
			if (noErr == GetFlavorData(theDragRef, theItem,
				flavorTypeHFS, (Ptr)&r, &SentSize, 0))
			{
				ReportStandardOpenDiskError(
					InsertADiskOrAliasFromSpec(&r.fileSpec));
			}
		}

#if 1
		if (gTrueBackgroundFlag) {
			ProcessSerialNumber currentProcess = {0, kCurrentProcess};

			(void) SetFrontProcess(&currentProcess);
		}
#endif
	}

	return noErr;
}

LOCALVAR DragTrackingHandlerUPP gGlobalTrackingHandler = NULL;
LOCALVAR DragReceiveHandlerUPP gGlobalReceiveHandler = NULL;

LOCALPROC UnPrepareForDragging(void)
{
	if (NULL != gGlobalReceiveHandler) {
		RemoveReceiveHandler(gGlobalReceiveHandler, gMyMainWindow);
		DisposeDragReceiveHandlerUPP(gGlobalReceiveHandler);
		gGlobalReceiveHandler = NULL;
	}
	if (NULL != gGlobalTrackingHandler) {
		RemoveTrackingHandler(gGlobalTrackingHandler, gMyMainWindow);
		DisposeDragTrackingHandlerUPP(gGlobalTrackingHandler);
		gGlobalTrackingHandler = NULL;
	}
}

LOCALFUNC blnr PrepareForDragging(void)
{
	blnr IsOk = falseblnr;

	gGlobalTrackingHandler = NewDragTrackingHandlerUPP(
		GlobalTrackingHandler);
	if (gGlobalTrackingHandler != NULL) {
		gGlobalReceiveHandler = NewDragReceiveHandlerUPP(
			GlobalReceiveHandler);
		if (gGlobalReceiveHandler != NULL) {
			if (noErr == InstallTrackingHandler(gGlobalTrackingHandler,
				gMyMainWindow, nil))
			{
				if (noErr == InstallReceiveHandler(
					gGlobalReceiveHandler, gMyMainWindow, nil))
				{
					IsOk = trueblnr;
				}
			}
		}
	}

	return IsOk;
}

LOCALPROC HandleEventLocation(EventRef theEvent)
{
	Point NewMousePos;

	if (! gWeAreActive) {
		return;
	}

	if (noErr == GetEventParameter(theEvent,
		kEventParamMouseLocation,
		typeQDPoint,
		NULL,
		sizeof(NewMousePos),
		NULL,
		&NewMousePos))
	{
		MousePositionNotify(NewMousePos);
	}
}

LOCALPROC HandleEventModifiers(EventRef theEvent)
{
	UInt32 theModifiers;

	if (gWeAreActive) {
		GetEventParameter(theEvent, kEventParamKeyModifiers,
			typeUInt32, NULL, sizeof(typeUInt32), NULL, &theModifiers);

		MyUpdateKeyboardModifiers(theModifiers);
	}
}

LOCALVAR blnr IsOurMouseMove;

static pascal OSStatus windowEventHandler(
	EventHandlerCallRef nextHandler, EventRef theEvent,
	void* userData)
{
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass = GetEventClass(theEvent);
	UInt32 eventKind = GetEventKind(theEvent);

	UnusedParam(nextHandler);
	UnusedParam(userData);
	switch(eventClass) {
		case kEventClassWindow:
			switch(eventKind) {
				case kEventWindowClose:
					RequestMacOff = trueblnr;
					result = noErr;
					break;
				case kEventWindowDrawContent:
					Update_Screen();
					result = noErr;
					break;
				case kEventWindowClickContentRgn:
					MouseIsOutside = falseblnr;
					HandleEventLocation(theEvent);
					HandleEventModifiers(theEvent);
					MyMouseButtonSet(trueblnr);
					result = noErr;
					break;
				case kEventWindowFocusAcquired:
					gLackFocusFlag = falseblnr;
					result = noErr;
					break;
				case kEventWindowFocusRelinquish:
					{
						WindowRef window;

						(void) GetEventParameter(theEvent,
							kEventParamDirectObject, typeWindowRef,
							NULL, sizeof(WindowRef), NULL, &window);
						if ((window == gMyMainWindow)
							&& (window != gMyOldWindow))
						{
							ClearWeAreActive();
							gLackFocusFlag = trueblnr;
						}
					}
					result = noErr;
					break;
			}
			break;
		case kEventClassMouse:
			switch(eventKind) {
				case kEventMouseMoved:
				case kEventMouseDragged:
					MouseIsOutside = falseblnr;
#if 0 /* don't bother, CheckMouseState will take care of it, better */
					HandleEventLocation(theEvent);
					HandleEventModifiers(theEvent);
#endif
					IsOurMouseMove = trueblnr;
					result = noErr;
					break;
			}
			break;
	}

	return result;
}

LOCALFUNC blnr MyCreateNewWindow(Rect *Bounds, WindowPtr *theWindow)
{
	WindowPtr ResultWin;
	blnr IsOk = falseblnr;

	if (noErr == CreateNewWindow(

#if VarFullScreen
		UseFullScreen ?
#endif
#if MayFullScreen
			/*
				appears not to work properly with aglSetWindowRef
				at least in OS X 10.5.5
				also doesn't seem to be needed. maybe dates from when
				didn't cover all screens in full screen mode.
			*/
			/*
				Oops, no, kDocumentWindowClass doesn't seem to
				work quite right if made immediately after
				launch, title bar gets moved onscreen. At least
				in Intel 10.5.6, doesn't happen in PowerPC 10.4.11.
				Oddly, this problem doesn't happen if icon
				already in dock before launch, but perhaps
				that is just a timing issue.
				So use kPlainWindowClass after all.
			*/
			kPlainWindowClass
#endif
#if VarFullScreen
			:
#endif
#if MayNotFullScreen
			kDocumentWindowClass
#endif
		,

#if VarFullScreen
		UseFullScreen ?
#endif
#if MayFullScreen
			/* kWindowStandardHandlerAttribute */ 0
#endif
#if VarFullScreen
			:
#endif
#if MayNotFullScreen
			kWindowCloseBoxAttribute
				| kWindowCollapseBoxAttribute
#endif
		,

		Bounds, &ResultWin
	))
	{
#if VarFullScreen
		if (! UseFullScreen)
#endif
#if MayNotFullScreen
		{
			SetWindowTitleWithCFString(ResultWin,
				MyAppName /* CFSTR("Mini vMac") */);
		}
#endif
		InstallStandardEventHandler(GetWindowEventTarget(ResultWin));
		{
			static const EventTypeSpec windowEvents[] =
				{
					{kEventClassWindow, kEventWindowClose},
					{kEventClassWindow, kEventWindowDrawContent},
					{kEventClassWindow, kEventWindowClickContentRgn},
					{kEventClassMouse, kEventMouseMoved},
					{kEventClassMouse, kEventMouseDragged},
					{kEventClassWindow, kEventWindowFocusAcquired},
					{kEventClassWindow, kEventWindowFocusRelinquish}
				};
			InstallWindowEventHandler(ResultWin,
				NewEventHandlerUPP(windowEventHandler),
				GetEventTypeCount(windowEvents),
				windowEvents, 0, NULL);
		}

		*theWindow = ResultWin;

		IsOk = trueblnr;
	}

	return IsOk;
}

LOCALPROC CloseAglCurrentContext(void)
{
	if (ctx != NULL) {
		/*
			Only because MyDrawWithOpenGL doesn't
			bother to do this. No one
			uses the CurrentContext
			without settting it first.
		*/
		if (GL_TRUE != aglSetCurrentContext(NULL)) {
			/* err = aglReportError() */
		}
		ctx = NULL;
	}
}

LOCALPROC CloseMainWindow(void)
{
	UnPrepareForDragging();

	if (window_ctx != NULL) {
		if (GL_TRUE != aglDestroyContext(window_ctx)) {
			/* err = aglReportError() */
		}
		window_ctx = NULL;
	}

	if (window_fmt != NULL) {
		aglDestroyPixelFormat(window_fmt);
		window_fmt = NULL;
	}

	if (gMyMainWindow != NULL) {
		DisposeWindow(gMyMainWindow);
		gMyMainWindow = NULL;
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
LOCALVAR Point WinPositionWins[kNumMagStates];
#endif

LOCALFUNC blnr CreateMainWindow(void)
{
#if MayNotFullScreen
	int WinIndx;
#endif
	Rect MainScrnBounds;
	Rect AllScrnBounds;
	Rect NewWinRect;
	short leftPos;
	short topPos;
	short NewWindowHeight = vMacScreenHeight;
	short NewWindowWidth = vMacScreenWidth;
	blnr IsOk = falseblnr;

#if VarFullScreen
	if (UseFullScreen) {
		My_HideMenuBar();
	} else {
		My_ShowMenuBar();
	}
#else
#if MayFullScreen
	My_HideMenuBar();
#endif
#endif

	MyGetGrayRgnBounds(&AllScrnBounds);
	MyGetScreenBitsBounds(&MainScrnBounds);

#if EnableMagnify
	if (UseMagnify) {
		NewWindowHeight *= MyWindowScale;
		NewWindowWidth *= MyWindowScale;
	}
#endif

	leftPos = MainScrnBounds.left
		+ ((MainScrnBounds.right - MainScrnBounds.left)
			- NewWindowWidth) / 2;
	topPos = MainScrnBounds.top
		+ ((MainScrnBounds.bottom - MainScrnBounds.top)
			- NewWindowHeight) / 2;
	if (leftPos < MainScrnBounds.left) {
		leftPos = MainScrnBounds.left;
	}
	if (topPos < MainScrnBounds.top) {
		topPos = MainScrnBounds.top;
	}

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		ViewHSize = MainScrnBounds.right - MainScrnBounds.left;
		ViewVSize = MainScrnBounds.bottom - MainScrnBounds.top;
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

	/* Create window rectangle and centre it on the screen */
	SetRect(&MainScrnBounds, 0, 0, NewWindowWidth, NewWindowHeight);
	OffsetRect(&MainScrnBounds, leftPos, topPos);

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewWinRect = AllScrnBounds;
	}
#endif
#if VarFullScreen
	else
#endif
#if MayNotFullScreen
	{
#if EnableMagnify
		if (UseMagnify) {
			WinIndx = kMagStateMagnifgy;
		} else
#endif
		{
			WinIndx = kMagStateNormal;
		}

		if (! HavePositionWins[WinIndx]) {
			WinPositionWins[WinIndx].h = leftPos;
			WinPositionWins[WinIndx].v = topPos;
			HavePositionWins[WinIndx] = trueblnr;
			NewWinRect = MainScrnBounds;
		} else {
			SetRect(&NewWinRect, 0, 0, NewWindowWidth, NewWindowHeight);
			OffsetRect(&NewWinRect,
				WinPositionWins[WinIndx].h, WinPositionWins[WinIndx].v);
		}
	}
#endif

#if MayNotFullScreen
	CurWinIndx = WinIndx;
#endif

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		hOffset = MainScrnBounds.left - AllScrnBounds.left;
		vOffset = MainScrnBounds.top - AllScrnBounds.top;
	}
#endif

	if (MyCreateNewWindow(&NewWinRect, &gMyMainWindow)) {
		static const GLint window_attrib[] = {AGL_RGBA,
#if UseAGLdoublebuff
			AGL_DOUBLEBUFFER,
#endif
			/* AGL_DEPTH_SIZE, 16,  */
			AGL_NONE};

#if 0 != vMacScreenDepth
		ColorModeWorks = trueblnr;
#endif

		window_fmt = aglChoosePixelFormat(NULL, 0, window_attrib);
		if (NULL == window_fmt) {
			/* err = aglReportError() */
		} else {
			window_ctx = aglCreateContext(window_fmt, NULL);
			if (NULL == window_ctx) {
				/* err = aglReportError() */
			} else {

				ShowWindow(gMyMainWindow);

				if (GL_TRUE != (
					/*
						aglSetDrawable is deprecated, but use it anyway
						if at all possible, because aglSetWindowRef
						doesn't seeem to work properly on a
						kPlainWindowClass window.
						Should move to Cocoa.
					*/
					HaveMyaglSetDrawable()
					? MyaglSetDrawable(window_ctx,
						GetWindowPort(gMyMainWindow))
					:
					HaveMyaglSetWindowRef()
					? MyaglSetWindowRef(window_ctx, gMyMainWindow)
					:
					GL_FALSE))
				{
					/* err = aglReportError() */
				} else {
					ctx = window_ctx;

#if VarFullScreen
					if (UseFullScreen)
#endif
#if MayFullScreen
					{
						int h = NewWinRect.right - NewWinRect.left;
						int v = NewWinRect.bottom - NewWinRect.top;

						GLhOffset = hOffset;
						GLvOffset = v - vOffset;
						MyAdjustGLforSize(h, v);
					}
#endif
#if VarFullScreen
					else
#endif
#if MayNotFullScreen
					{
						GLhOffset = 0;
						GLvOffset = NewWindowHeight;
						MyAdjustGLforSize(NewWindowWidth,
							NewWindowHeight);
					}
#endif

#if UseAGLdoublebuff && 1
					{
						GLint agSwapInterval = 1;
						if (GL_TRUE != aglSetInteger(
							window_ctx, AGL_SWAP_INTERVAL,
							&agSwapInterval))
						{
							MacMsg("oops", "aglSetInteger failed",
								falseblnr);
						} else {
							/*
								MacMsg("good", "aglSetInteger ok",
									falseblnr);
							*/
						}
					}
#endif

#if VarFullScreen
					if (! UseFullScreen)
#endif
#if MayNotFullScreen
					{
						/* check if window rect valid */
						Rect tr;

						if (MyGetWindowTitleBounds(gMyMainWindow, &tr))
						{
							if (! RectInRgn(&tr, GetGrayRgn())) {
								MySetMacWindContRect(gMyMainWindow,
									&MainScrnBounds);
								if (MyGetWindowTitleBounds(
									gMyMainWindow, &tr))
								{
									if (! RectInRgn(&tr, GetGrayRgn()))
									{
										OffsetRect(&MainScrnBounds, 0,
											AllScrnBounds.top - tr.top);
										MySetMacWindContRect(
											gMyMainWindow,
											&MainScrnBounds);
									}
								}
							}
						}
					}
#endif

					(void) PrepareForDragging();

					IsOk = trueblnr;
				}
			}
		}
	}

	return IsOk;
}

#if EnableRecreateW
LOCALPROC ZapMyWState(void)
{
	gMyMainWindow = NULL;
	window_fmt = NULL;
	window_ctx = NULL;
	gGlobalReceiveHandler = NULL;
	gGlobalTrackingHandler = NULL;
}
#endif

#if EnableRecreateW
struct MyWState {
	WindowPtr f_MainWindow;
	AGLPixelFormat f_fmt;
	AGLContext f_ctx;
#if MayFullScreen
	short f_hOffset;
	short f_vOffset;
	ui4r f_ViewHSize;
	ui4r f_ViewVSize;
	ui4r f_ViewHStart;
	ui4r f_ViewVStart;
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
	short f_GLhOffset;
	short f_GLvOffset;
	DragTrackingHandlerUPP f_gGlobalTrackingHandler;
	DragReceiveHandlerUPP f_gGlobalReceiveHandler;
};
typedef struct MyWState MyWState;
#endif

#if EnableRecreateW
LOCALPROC GetMyWState(MyWState *r)
{
	r->f_MainWindow = gMyMainWindow;
	r->f_fmt = window_fmt;
	r->f_ctx = window_ctx;
#if MayFullScreen
	r->f_hOffset = hOffset;
	r->f_vOffset = vOffset;
	r->f_ViewHSize = ViewHSize;
	r->f_ViewVSize = ViewVSize;
	r->f_ViewHStart = ViewHStart;
	r->f_ViewVStart = ViewVStart;
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
	r->f_GLhOffset = GLhOffset;
	r->f_GLvOffset = GLvOffset;
	r->f_gGlobalTrackingHandler = gGlobalTrackingHandler;
	r->f_gGlobalReceiveHandler = gGlobalReceiveHandler;
}
#endif

#if EnableRecreateW
LOCALPROC SetMyWState(MyWState *r)
{
	gMyMainWindow = r->f_MainWindow;
	window_fmt = r->f_fmt;
	window_ctx = r->f_ctx;
#if MayFullScreen
	hOffset = r->f_hOffset;
	vOffset = r->f_vOffset;
	ViewHSize = r->f_ViewHSize;
	ViewVSize = r->f_ViewVSize;
	ViewHStart = r->f_ViewHStart;
	ViewVStart = r->f_ViewVStart;
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
	GLhOffset = r->f_GLhOffset;
	GLvOffset = r->f_GLvOffset;
	gGlobalTrackingHandler = r->f_gGlobalTrackingHandler;
	gGlobalReceiveHandler = r->f_gGlobalReceiveHandler;

	ctx = window_ctx;
}
#endif

#if EnableRecreateW
LOCALFUNC blnr ReCreateMainWindow(void)
{
	MyWState old_state;
	MyWState new_state;

#if VarFullScreen
	if (! UseFullScreen)
#endif
#if MayNotFullScreen
	{
		/* save old position */
		if (gMyMainWindow != NULL) {
			Rect r;

			if (MyGetWindowContBounds(gMyMainWindow, &r)) {
				WinPositionWins[CurWinIndx].h = r.left;
				WinPositionWins[CurWinIndx].v = r.top;
			}
		}
	}
#endif

#if MayFullScreen
	UngrabMachine();
#endif

	CloseAglCurrentContext();

	gMyOldWindow = gMyMainWindow;

	GetMyWState(&old_state);

	ZapMyWState();

#if VarFullScreen
	UseFullScreen = WantFullScreen;
#endif
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif

	if (! CreateMainWindow()) {
		gMyOldWindow = NULL;
		CloseMainWindow();
		SetMyWState(&old_state);

#if VarFullScreen
		if (UseFullScreen) {
			My_HideMenuBar();
		} else {
			My_ShowMenuBar();
		}
#endif

		/* avoid retry */
#if VarFullScreen
		WantFullScreen = UseFullScreen;
#endif
#if EnableMagnify
		WantMagnify = UseMagnify;
#endif

		return falseblnr;
	} else {
		GetMyWState(&new_state);
		SetMyWState(&old_state);
		CloseMainWindow();
		gMyOldWindow = NULL;
		SetMyWState(&new_state);

		if (HaveCursorHidden) {
			(void) MyMoveMouse(CurMouseH, CurMouseV);
			WantCursorHidden = trueblnr;
		} else {
			MouseIsOutside = falseblnr; /* don't know */
		}

		return trueblnr;
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
			if (WantFullScreen
				&& HaveMyCGDisplayPixelsWide()
				&& HaveMyCGDisplayPixelsHigh())
			{
				CGDirectDisplayID CurMainDisplayID = MyMainDisplayID();

				if ((MyCGDisplayPixelsWide(CurMainDisplayID)
					>= vMacScreenWidth * MyWindowScale)
					&& (MyCGDisplayPixelsHigh(CurMainDisplayID)
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

LOCALPROC DoNotInBackgroundTasks(void)
{
#if 0
	if (HaveMyCGCursorIsVisible()) {
		HaveCursorHidden = ! MyCGCursorIsVisible();

		/*
			This shouldn't be needed, but have seen
			cursor state get messed up in 10.4.
			If the os x hide count gets off, this
			should fix it within a few ticks.

			oops, no, doesn't seem to be accurate,
			and makes things worse. particularly if
			cursor in obscured state after typing
			into dialog.
		*/
		/* trying a different approach further below */
	}
#endif

	if (HaveCursorHidden != (
#if MayNotFullScreen
		(WantCursorHidden
#if VarFullScreen
			|| UseFullScreen
#endif
		) &&
#endif
		gWeAreActive && ! CurSpeedStopped))
	{
		HaveCursorHidden = ! HaveCursorHidden;
		if (HaveCursorHidden) {
			MyHideCursor();
		} else {
			/*
				kludge for OS X, where mouse over Dock devider
				changes cursor, and never sets it back.
			*/
			SetCursorArrow();

			MyShowCursor();
		}
	}

	/* check if actual cursor visibility is what it should be */
	if (HaveMyCGCursorIsVisible()) {
		/* but only in OS X 10.3 and later */
		if (MyCGCursorIsVisible()) {
			if (HaveCursorHidden) {
				MyHideCursor();
				if (MyCGCursorIsVisible()) {
					/*
						didn't work, attempt undo so that
						hide cursor count won't get large
					*/
					MyShowCursor();
				}
			}
		} else {
			if (! HaveCursorHidden) {
				MyShowCursor();
				/*
					don't check if worked, assume can't decrement
					hide cursor count below 0
				*/
			}
		}
	}
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

	if (! gWeAreActive) {
		if (! (gTrueBackgroundFlag || gLackFocusFlag)) {
			gWeAreActive = trueblnr;
			ReconnectKeyCodes3();
			SetCursorArrow();
		}
	}

#if VarFullScreen
	if (gTrueBackgroundFlag && WantFullScreen) {
		ToggleWantFullScreen();
	}
#endif

	if (CurSpeedStopped != (SpeedStopped ||
		(gTrueBackgroundFlag && ! RunInBackground
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
	if (gWeAreActive) {
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
	}
#endif

#if MayFullScreen
	if (GrabMachine != (
#if VarFullScreen
		UseFullScreen &&
#endif
		gWeAreActive && ! CurSpeedStopped))
	{
		GrabMachine = ! GrabMachine;
		AdjustMachineGrab();
	}
#endif

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (WantScreensChangedCheck) {
		WantScreensChangedCheck = falseblnr;
		MyUpdateOpenGLContext();
	}

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		ScreenChangedAll();
	}

	if (! gWeAreActive) {
		/*
			dialog during drag and drop hangs if in background
				and don't want recursive dialogs
			so wait til later to display dialog
		*/
	} else {
#if IncludeSonyNew
		if (vSonyNewDiskWanted) {
#if IncludeSonyNameNew
			if (vSonyNewDiskName != NotAPbuf) {
				CFStringRef NewDiskName =
					CFStringCreateWithPbuf(vSonyNewDiskName);
				MakeNewDisk(vSonyNewDiskSize, NewDiskName);
				if (NewDiskName != NULL) {
					CFRelease(NewDiskName);
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

	if (gWeAreActive) {
		DoNotInBackgroundTasks();
	}
}

#define CheckItem CheckMenuItem

/* Menu Constants */

#define kAppleMenu   128
#define kFileMenu    129
#define kSpecialMenu 130

enum {
	kCmdIdNull,
	kCmdIdMoreCommands,

	kNumCmdIds
};

LOCALFUNC OSStatus MyProcessCommand(MenuCommand inCommandID)
{
	OSStatus result = noErr;

	switch (inCommandID) {
		case kHICommandAbout:
			DoAboutMsg();
			break;
		case kHICommandQuit:
			RequestMacOff = trueblnr;
			break;
		case kHICommandOpen:
			RequestInsertDisk = trueblnr;
			break;
		case kCmdIdMoreCommands:
			DoMoreCommandsMsg();
			break;
		default:
			result = eventNotHandledErr;
			break;
	}

	return result;
}

LOCALFUNC OSStatus Keyboard_UpdateKeyMap3(EventRef theEvent, blnr down)
{
	UInt32 uiKeyCode;

	HandleEventModifiers(theEvent);
	GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, NULL,
		sizeof(uiKeyCode), NULL, &uiKeyCode);
	Keyboard_UpdateKeyMap2(Keyboard_RemapMac(uiKeyCode & 0x000000FF),
		down);
	return noErr;
}

static pascal OSStatus MyEventHandler(EventHandlerCallRef nextHandler,
	EventRef theEvent, void * userData)
{
	OSStatus result = eventNotHandledErr;
	UInt32 eventClass = GetEventClass(theEvent);
	UInt32 eventKind = GetEventKind(theEvent);

	UnusedParam(userData);

	switch (eventClass) {
		case kEventClassMouse:
			switch (eventKind) {
				case kEventMouseDown:
#if MayFullScreen
					if (GrabMachine) {
						HandleEventLocation(theEvent);
						HandleEventModifiers(theEvent);
						MyMouseButtonSet(trueblnr);
						result = noErr;
					} else
#endif
					{
						result = CallNextEventHandler(
							nextHandler, theEvent);
					}
					break;
				case kEventMouseUp:
					HandleEventLocation(theEvent);
					HandleEventModifiers(theEvent);
					MyMouseButtonSet(falseblnr);
#if MayFullScreen
					if (GrabMachine) {
						result = noErr;
					} else
#endif
					{
						result = CallNextEventHandler(
							nextHandler, theEvent);
					}
					break;
				case kEventMouseMoved:
				case kEventMouseDragged:
					IsOurMouseMove = falseblnr;
					result = CallNextEventHandler(
						nextHandler, theEvent);
						/*
							Actually, mousing over window does't seem
							to go through here, it goes directly to
							the window routine. But since not documented
							either way, leave the check in case this
							changes.
						*/
					if (! IsOurMouseMove) {
						MouseIsOutside = trueblnr;
#if 0 /* don't bother, CheckMouseState will take care of it, better */
						HandleEventLocation(theEvent);
						HandleEventModifiers(theEvent);
#endif
					}
					break;
			}
			break;
		case kEventClassApplication:
			switch (eventKind) {
				case kEventAppActivated:
					gTrueBackgroundFlag = falseblnr;
					result = noErr;
					break;
				case kEventAppDeactivated:
					gTrueBackgroundFlag = trueblnr;
					result = noErr;
					ClearWeAreActive();
					break;
			}
			break;
		case kEventClassCommand:
			switch (eventKind) {
				case kEventProcessCommand:
					{
						HICommand hiCommand;

						GetEventParameter(theEvent,
							kEventParamDirectObject, typeHICommand,
							NULL, sizeof(HICommand), NULL, &hiCommand);
						result = MyProcessCommand(hiCommand.commandID);
					}
					break;
			}
			break;
		case kEventClassKeyboard:
			if (! gWeAreActive) {
				return CallNextEventHandler(nextHandler, theEvent);
			} else {
				switch (eventKind) {
					case kEventRawKeyDown:
						result = Keyboard_UpdateKeyMap3(
							theEvent, trueblnr);
						break;
					case kEventRawKeyUp:
						result = Keyboard_UpdateKeyMap3(
							theEvent, falseblnr);
						break;
					case kEventRawKeyModifiersChanged:
						HandleEventModifiers(theEvent);
						result = noErr;
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
	return result;
}

LOCALPROC AppendMenuConvertCStr(
	MenuRef menu,
	MenuCommand inCommandID,
	char *s)
{
	CFStringRef cfStr = CFStringCreateFromSubstCStr(s);
	if (NULL != cfStr) {
		AppendMenuItemTextWithCFString(menu, cfStr,
			0, inCommandID, NULL);
		CFRelease(cfStr);
	}
}

LOCALPROC AppendMenuSep(MenuRef menu)
{
	AppendMenuItemTextWithCFString(menu, NULL,
		kMenuItemAttrSeparator, 0, NULL);
}

LOCALFUNC MenuRef NewMenuFromConvertCStr(short menuID, char *s)
{
	MenuRef menu = NULL;

	CFStringRef cfStr = CFStringCreateFromSubstCStr(s);
	if (NULL != cfStr) {
		OSStatus err = CreateNewMenu(menuID, 0, &menu);
		if (err != noErr) {
			menu = NULL;
		} else {
			(void) SetMenuTitleWithCFString(menu, cfStr);
		}
		CFRelease(cfStr);
	}

	return menu;
}

LOCALPROC RemoveCommandKeysInMenu(MenuRef theMenu)
{
	MenuRef outHierMenu;
	int i;
	UInt16 n = CountMenuItems(theMenu);

	for (i = 1; i <= n; ++i) {
		SetItemCmd(theMenu, i, 0);
		if (noErr == GetMenuItemHierarchicalMenu(
			theMenu, i, &outHierMenu)
			&& (NULL != outHierMenu))
		{
			RemoveCommandKeysInMenu(outHierMenu);
		}
	}
}

LOCALFUNC blnr InstallOurMenus(void)
{
	MenuRef menu;
	Str255 s;

	PStrFromChar(s, (char)20);
	menu = NewMenu(kAppleMenu, s);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kHICommandAbout,
			kStrMenuItemAbout);
		AppendMenuSep(menu);
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kFileMenu, kStrMenuFile);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kHICommandOpen,
			kStrMenuItemOpen ";ll");
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kSpecialMenu, kStrMenuSpecial);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kCmdIdMoreCommands,
			kStrMenuItemMore ";ll");
		InsertMenu(menu, 0);
	}

	{
		MenuRef outMenu;
		MenuItemIndex outIndex;

		if (noErr == GetIndMenuItemWithCommandID(
			NULL, kHICommandQuit, 1, &outMenu, &outIndex))
		{
			RemoveCommandKeysInMenu(outMenu);
		}
	}

	DrawMenuBar();

	return trueblnr;
}

LOCALFUNC blnr InstallOurAppearanceClient(void)
{
	RegisterAppearanceClient(); /* maybe not needed ? */
	return trueblnr;
}

LOCALVAR blnr DisplayRegistrationCallBackSuccessful = falseblnr;

static void DisplayRegisterReconfigurationCallback(
	CGDirectDisplayID display, CGDisplayChangeSummaryFlags flags,
	void *userInfo)
{
	UnusedParam(display);
	UnusedParam(userInfo);

	if (0 != (flags & kCGDisplayBeginConfigurationFlag)) {
		/* fprintf(stderr, "kCGDisplayBeginConfigurationFlag\n"); */
	} else {
#if 0
		if (0 != (flags & kCGDisplayMovedFlag)) {
			fprintf(stderr, "kCGDisplayMovedFlag\n");
		}
		if (0 != (flags & kCGDisplaySetMainFlag)) {
			fprintf(stderr, "kCGDisplaySetMainFlag\n");
		}
		if (0 != (flags & kCGDisplaySetModeFlag)) {
			fprintf(stderr, "kCGDisplaySetModeFlag\n");
		}

		if (0 != (flags & kCGDisplayAddFlag)) {
			fprintf(stderr, "kCGDisplayAddFlag\n");
		}
		if (0 != (flags & kCGDisplayRemoveFlag)) {
			fprintf(stderr, "kCGDisplayRemoveFlag\n");
		}

		if (0 != (flags & kCGDisplayEnabledFlag)) {
			fprintf(stderr, "kCGDisplayEnabledFlag\n");
		}
		if (0 != (flags & kCGDisplayDisabledFlag)) {
			fprintf(stderr, "kCGDisplayDisabledFlag\n");
		}

		if (0 != (flags & kCGDisplayMirrorFlag)) {
			fprintf(stderr, "kCGDisplayMirrorFlag\n");
		}
		if (0 != (flags & kCGDisplayUnMirrorFlag)) {
			fprintf(stderr, "kCGDisplayMirrorFlag\n");
		}
#endif

		WantScreensChangedCheck = trueblnr;

#if VarFullScreen
		if (WantFullScreen) {
			ToggleWantFullScreen();
		}
#endif
	}
}

LOCALFUNC blnr InstallOurEventHandlers(void)
{
	EventTypeSpec eventTypes[] = {
		{kEventClassMouse, kEventMouseDown},
		{kEventClassMouse, kEventMouseUp},
		{kEventClassMouse, kEventMouseMoved},
		{kEventClassMouse, kEventMouseDragged},
		{kEventClassKeyboard, kEventRawKeyModifiersChanged},
		{kEventClassKeyboard, kEventRawKeyDown},
		{kEventClassKeyboard, kEventRawKeyUp},
		{kEventClassApplication, kEventAppActivated},
		{kEventClassApplication, kEventAppDeactivated},
		{kEventClassCommand, kEventProcessCommand}
	};

	InstallApplicationEventHandler(
		NewEventHandlerUPP(MyEventHandler),
		GetEventTypeCount(eventTypes),
		eventTypes, NULL, NULL);

	InitKeyCodes();

	InstallAppleEventHandlers();

	if (HaveMyCGDisplayRegisterReconfigurationCallback()) {
		if (kCGErrorSuccess ==
			MyCGDisplayRegisterReconfigurationCallback(
				DisplayRegisterReconfigurationCallback, NULL))
		{
			DisplayRegistrationCallBackSuccessful = trueblnr;
		}
	}

	/* (void) SetMouseCoalescingEnabled(false, NULL); */

	return trueblnr;
}

LOCALPROC UnInstallOurEventHandlers(void)
{
	if (DisplayRegistrationCallBackSuccessful) {
		if (HaveMyCGDisplayRemoveReconfigurationCallback()) {
			MyCGDisplayRemoveReconfigurationCallback(
				DisplayRegisterReconfigurationCallback, NULL);
		}
	}
}

GLOBALOSGLUPROC WaitForNextTick(void)
{
	OSStatus err;
	EventRef theEvent;
	ui3r NumChecks;
	EventTimeout inTimeout;
	EventTargetRef theTarget = GetEventDispatcherTarget();

	inTimeout = kEventDurationNoWait;

label_retry:
	NumChecks = 0;
	while ((NumChecks < 32) && (noErr == (err =
		ReceiveNextEvent(0, NULL, inTimeout,
			true, &theEvent))))
	{
		(void) SendEventToEventTarget(theEvent, theTarget);
		ReleaseEvent(theEvent);
		inTimeout = kEventDurationNoWait;
		++NumChecks;
	}

	CheckForSavedTasks();

	if (ForceMacOff) {
		return;
	}

	if (CurSpeedStopped) {
		DoneWithDrawingForTick();
		inTimeout = kEventDurationForever;
		goto label_retry;
	}

	if (ExtraTimeNotOver()) {
		inTimeout =
			NextTickChangeTime - GetCurrentEventTime();
		if (inTimeout > 0.0) {
#if 1
			struct timespec rqt;
			struct timespec rmt;

			rqt.tv_sec = 0;
			rqt.tv_nsec = inTimeout / kEventDurationNanosecond;
			(void) nanosleep(&rqt, &rmt);
			inTimeout = kEventDurationNoWait;
			goto label_retry;
#elif 1
			usleep(inTimeout / kEventDurationMicrosecond);
			inTimeout = kEventDurationNoWait;
			goto label_retry;
#else
			/*
				This has higher overhead.
			*/
			goto label_retry;
#endif
		}
	}

	if (CheckDateTime()) {
#if MySoundEnabled
		MySound_SecondNotify();
#endif
#if EnableDemoMsg
		DemoModeSecondNotify();
#endif
	}

	if (gWeAreActive) {
		CheckMouseState();
	}

	OnTrueTime = TrueEmulatedTime;

#if dbglog_TimeStuff
	dbglog_writelnNum("WaitForNextTick, OnTrueTime", OnTrueTime);
#endif
}

/* --- platform independent code can be thought of as going here --- */

#include "PROGMAIN.h"

LOCALPROC ReserveAllocAll(void)
{
#if dbglog_HAVE
	dbglog_ReserveAlloc();
#endif
	ReserveAllocOneBlock(&ROM, kROM_Size, 5, falseblnr);
	ReserveAllocOneBlock(&screencomparebuff,
		vMacScreenNumBytes, 5, trueblnr);
	ReserveAllocOneBlock(&CntrlDisplayBuff,
		vMacScreenNumBytes, 5, falseblnr);
	ReserveAllocOneBlock(&ScalingBuff, vMacScreenNumPixels
#if 0 != vMacScreenDepth
		* 4
#endif
		, 5, falseblnr);
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

LOCALFUNC blnr InitOSGLU(void)
{
	if (AllocMyMemory())
	if (InitMyApplInfo())
#if dbglog_HAVE
	if (dbglog_open())
#endif
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (InstallOurAppearanceClient())
	if (InstallOurEventHandlers())
	if (InstallOurMenus())
	if (CreateMainWindow())
	if (LoadMacRom())
	if (LoadInitialImages())
#if UseActvCode
	if (ActvCodeInit())
#endif
#if EmLocalTalk
	if (InitLocalTalk())
#endif
	if (InitLocationDat())
	if (WaitForRom())
	{
		return trueblnr;
	}
	return falseblnr;
}

#if dbglog_HAVE && 0
IMPORTPROC DoDumpTable(void);
#endif
#if dbglog_HAVE && 0
IMPORTPROC DumpRTC(void);
#endif

LOCALPROC UnInitOSGLU(void)
{
#if dbglog_HAVE && 0
	DoDumpTable();
#endif
#if dbglog_HAVE && 0
	DumpRTC();
#endif

	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

#if MayFullScreen
	UngrabMachine();
#endif

#if MySoundEnabled
	MySound_Stop();
#endif

	CloseAglCurrentContext();
	CloseMainWindow();

#if MayFullScreen
	My_ShowMenuBar();
#endif

#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

	if (! gTrueBackgroundFlag) {
		CheckSavedMacMsg();
	}

	UnInstallOurEventHandlers();

#if dbglog_HAVE
	dbglog_close();
#endif

	UnInitMyApplInfo();

	ForceShowCursor();
}

LOCALPROC ZapOSGLUVars(void)
{
	InitDrives();
	ZapWinStateVars();
}

/* adapted from Apple "Technical Q&A QA1061" */

static pascal OSStatus EventLoopEventHandler(
	EventHandlerCallRef inHandlerCallRef, EventRef inEvent,
	void *inUserData)
/*
	This code contains the standard Carbon event dispatch loop,
	as per "Inside Macintosh: Handling Carbon Events", Listing 3-10,
	except:

	o this loop supports yielding to cooperative threads based on the
		application maintaining the gNumberOfRunningThreads global
		variable, and

	o it also works around a problem with the Inside Macintosh code
		which unexpectedly quits when run on traditional Mac OS 9.

	See RunApplicationEventLoopWithCooperativeThreadSupport for
	an explanation of why this is inside a Carbon event handler.

	The code in Inside Mac has a problem in that it quits the
	event loop when ReceiveNextEvent returns an error.  This is
	wrong because ReceiveNextEvent can return eventLoopQuitErr
	when you call WakeUpProcess on traditional Mac OS.  So, rather
	than relying on an error from ReceiveNextEvent, this routine tracks
	whether the application is really quitting by installing a
	customer handler for the kEventClassApplication/kEventAppQuit
	Carbon event.  All the custom handler does is call through
	to the previous handler and, if it returns noErr (which indicates
	the application is quitting, it sets quitNow so that our event
	loop quits.

	Note that this approach continues to support
	QuitApplicationEventLoop, which is a simple wrapper that just posts
	a kEventClassApplication/kEventAppQuit event to the event loop.
*/
{
	UnusedParam(inHandlerCallRef);
	UnusedParam(inEvent);
	UnusedParam(inUserData);

	ProgramMain();
	QuitApplicationEventLoop();

	return noErr;
}

LOCALPROC RunApplicationEventLoopWithCooperativeThreadSupport(void)
/*
	A reimplementation of RunApplicationEventLoop that supports
	yielding time to cooperative threads.
*/
{
	static const EventTypeSpec eventSpec = {'KWIN', 'KWIN'};
	EventHandlerUPP theEventLoopEventHandlerUPP = nil;
	EventHandlerRef installedHandler = NULL;
	EventRef dummyEvent = nil;

/*
	Install EventLoopEventHandler, create a dummy event and post it,
	and then call RunApplicationEventLoop.  The rationale for this
	is as follows:  We want to unravel RunApplicationEventLoop so
	that we can can yield to cooperative threads.  In fact, the
	core code for RunApplicationEventLoop is pretty easy (you
	can see it above in EventLoopEventHandler).  However, if you
	just execute this code you miss out on all the standard event
	handlers.  These are relatively easy to reproduce (handling
	the quit event and so on), but doing so is a pain because
	a) it requires a bunch boilerplate code, and b) if Apple
	extends the list of standard event handlers, your application
	wouldn't benefit.  So, we execute our event loop from within
	a Carbon event handler that we cause to be executed by
	explicitly posting an event to our event loop.  Thus, the
	standard event handlers are installed while our event loop runs.
*/
	if (nil == (theEventLoopEventHandlerUPP
		= NewEventHandlerUPP(EventLoopEventHandler)))
	{
		/* fail */
	} else
	if (noErr != InstallEventHandler(GetApplicationEventTarget(),
		theEventLoopEventHandlerUPP, 1, &eventSpec, nil,
		&installedHandler))
	{
		/* fail */
	} else
	if (noErr != MacCreateEvent(nil, 'KWIN', 'KWIN',
		GetCurrentEventTime(), kEventAttributeNone,
		&dummyEvent))
	{
		/* fail */
	} else
	if (noErr != PostEventToQueue(GetMainEventQueue(),
		dummyEvent, kEventPriorityHigh))
	{
		/* fail */
	} else
	{
		RunApplicationEventLoop();
	}

	if (nil != dummyEvent) {
		ReleaseEvent(dummyEvent);
	}

	if (NULL != installedHandler) {
		(void) RemoveEventHandler(installedHandler);
	}
}

int main(void)
{
	ZapOSGLUVars();
	if (InitOSGLU()) {
		RunApplicationEventLoopWithCooperativeThreadSupport();
	}
	UnInitOSGLU();

	return 0;
}
