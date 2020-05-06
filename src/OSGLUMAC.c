/*
	OSGLUMAC.c

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
	Operating System GLUe for MACintosh

	All operating system dependent code for the
	Macintosh (pre OS X) platform should go here.

	This is also the "reference" implementation. General
	comments about what the platform dependent code
	does should go here, and not be repeated for each
	platform.

	This code is descended from Richard F. Bannister's Macintosh
	port of vMac, by Philip Cummins.

	The main entry point 'main' is at the end of this file.
*/

#include "CNFGRAPI.h"
#include "SYSDEPNS.h"
#include "ENDIANAC.h"

#include "MYOSGLUE.h"

#include "STRCONST.h"

#ifndef NavigationAvail
#define NavigationAvail 1
#endif

#ifndef AppearanceAvail
#define AppearanceAvail 1
#endif

#ifndef NewNamesAvail
#define NewNamesAvail 1
#endif

#ifndef CALL_NOT_IN_CARBON
#define CALL_NOT_IN_CARBON 1
#endif /* !defined(CALL_NOT_IN_CARBON) */


/* --- information about the environment --- */

/*
	code to determine characteristics
	of the current run time environment.
*/

#define TestBit(i, p) (((uimr)(i) & ((uimr)1 << (p))) != 0)

#ifndef HaveCPUfamM68K
#define HaveCPUfamM68K 0
#endif

#if HaveCPUfamM68K

/* MACENVRN.i */

#ifndef Support64kROM
#define Support64kROM 1
#endif

#define Have64kROM() (LMGetROM85() < 0)

/* -- basic environment checking -- */

#ifndef DebugTrapAvailableChecks
#define DebugTrapAvailableChecks 0
#endif

LOCALFUNC blnr LowToolTrapAvailable(short trap_num)
{
#if DebugTrapAvailableChecks
	if ((trap_num & 0x0800) == 0) {
		DebugStr("\pOSTrap in LowToolTrapAvailable");
		return falseblnr;
	}
	if ((trap_num & 0x07ff) >= 0x200) {
		DebugStr("\pHiToolTrap in LowToolTrapAvailable");
		return falseblnr;
	}
#endif
	return GetToolTrapAddress(trap_num) !=
		GetToolTrapAddress(_Unimplemented);
}

LOCALFUNC blnr HiToolTrapAvailable(short trap_num)
{
#if DebugTrapAvailableChecks
	if ((trap_num & 0x0800) == 0) {
		DebugStr("\pOSTrap in HiToolTrapAvailable");
		return falseblnr;
	}
	if (((trap_num & 0x07ff) < 0x200)
		|| ((trap_num & 0x07ff) >= 0x400))
	{
		DebugStr("\pLowToolTrap in HiToolTrapAvailable");
		return falseblnr;
	}
#endif
	if (GetToolTrapAddress(_InitGraf)
		== GetToolTrapAddress(0xAA6E))
	{
		return falseblnr;
	} else {
		return GetToolTrapAddress(trap_num) !=
			GetToolTrapAddress(_Unimplemented);
	}
}

LOCALFUNC blnr OSTrapAvailable(short trap_num)
{
#if DebugTrapAvailableChecks
	if ((trap_num & 0x0800) != 0) {
		DebugStr("\pToolTrap in ToolTrapAvailable");
		return falseblnr;
	}
#endif
	return GetOSTrapAddress(trap_num) !=
		GetToolTrapAddress(_Unimplemented);
}

LOCALVAR blnr MyEnvrAttrWaitNextEventAvail;
LOCALVAR blnr HaveEnvrAttrWaitNextEventAvail = falseblnr;

LOCALFUNC blnr HaveWaitNextEventAvail(void)
{
	if (! HaveEnvrAttrWaitNextEventAvail) {
		MyEnvrAttrWaitNextEventAvail =
			LowToolTrapAvailable(_WaitNextEvent);
		HaveEnvrAttrWaitNextEventAvail = trueblnr;
	}
	return MyEnvrAttrWaitNextEventAvail;
}

LOCALVAR blnr MyEnvrAttrGestaltAvail;
LOCALVAR blnr HaveEnvrAttrGestaltAvail = falseblnr;

LOCALFUNC blnr HaveGestaltAvail(void)
{
	if (! HaveEnvrAttrGestaltAvail) {
		MyEnvrAttrGestaltAvail =
			(! Have64kROM()) &&
			OSTrapAvailable(_Gestalt);
			/*
				contrary to all the documentation,
				TrapAvailable check by itself doesn't
				work on 64k ROM, because a tool box trap
				has the same trap number, and both
				GetOSTrapAddress and GetToolTrapAddress
				are the same as GetTrapAddress on a 64k ROM.
			*/
		HaveEnvrAttrGestaltAvail = trueblnr;
	}
	return MyEnvrAttrGestaltAvail;
}

#else

/* for PowerPC, assume always have these routines */

#define HaveWaitNextEventAvail() trueblnr
#define HaveGestaltAvail() trueblnr

#endif

LOCALVAR blnr MyEnvrAttrAliasMgrAvail;
LOCALVAR blnr HaveEnvrAttrAliasMgrAvail = falseblnr;

LOCALFUNC blnr HaveAliasMgrAvail(void)
{
	if (! HaveEnvrAttrAliasMgrAvail) {
		long reply;

		MyEnvrAttrAliasMgrAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltAliasMgrAttr, &reply))
			&& TestBit(reply, gestaltAliasMgrPresent);
		HaveEnvrAttrAliasMgrAvail = trueblnr;
	}
	return MyEnvrAttrAliasMgrAvail;
}

LOCALVAR blnr MyEnvrAttrAppleEvtMgrAvail;
LOCALVAR blnr HaveEnvrAttrAppleEvtMgrAvail = falseblnr;

LOCALFUNC blnr HaveAppleEvtMgrAvail(void)
{
	if (! HaveEnvrAttrAppleEvtMgrAvail) {
		long reply;

		MyEnvrAttrAppleEvtMgrAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltAppleEventsAttr, &reply))
			&& TestBit(reply, gestaltAppleEventsPresent);
		HaveEnvrAttrAppleEvtMgrAvail = trueblnr;
	}
	return MyEnvrAttrAppleEvtMgrAvail;
}

#if EnableDragDrop

LOCALVAR blnr MyEnvrAttrDragMgrAvail;
LOCALVAR blnr HaveEnvrAttrDragMgrAvail = falseblnr;

LOCALFUNC blnr HaveDragMgrAvail(void)
{
	if (! HaveEnvrAttrDragMgrAvail) {
		long reply;

		MyEnvrAttrDragMgrAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltDragMgrAttr, &reply))
			&& TestBit(reply, gestaltDragMgrPresent);
		HaveEnvrAttrDragMgrAvail = trueblnr;
	}
	return MyEnvrAttrDragMgrAvail;
}

#endif

#ifndef Windows85APIAvail
#define Windows85APIAvail 1
#endif

#if Windows85APIAvail
LOCALVAR blnr MyEnvrAttrHideShowMenuAvail;
LOCALVAR blnr HaveEnvrAttrHideShowMenuAvail = falseblnr;

LOCALFUNC blnr HaveHideShowMenuAvail(void)
{
	if (! HaveEnvrAttrHideShowMenuAvail) {
		long reply;

		MyEnvrAttrHideShowMenuAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltMenuMgrAttr, &reply));
		HaveEnvrAttrHideShowMenuAvail = trueblnr;
	}
	return MyEnvrAttrHideShowMenuAvail;
}
#endif

#if AppearanceAvail
LOCALVAR blnr MyEnvrAttrAppearanceAvail;
LOCALVAR blnr HaveEnvrAttrAppearanceAvail = falseblnr;

LOCALFUNC blnr HaveAppearanceAvail(void)
{
	if (! HaveEnvrAttrAppearanceAvail) {
		long reply;

		MyEnvrAttrAppearanceAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltAppearanceAttr, &reply));
		HaveEnvrAttrAppearanceAvail = trueblnr;
	}
	return MyEnvrAttrAppearanceAvail;
}
#endif

#if HaveCPUfamM68K
LOCALVAR blnr MyEnvrAttrSndMngrAvail;
LOCALVAR blnr HaveEnvrAttrSndMngrAvail = falseblnr;

LOCALFUNC blnr HaveSndMngrAvail(void)
{
	if (! HaveEnvrAttrSndMngrAvail) {
		MyEnvrAttrSndMngrAvail = LowToolTrapAvailable(_SndNewChannel);
		HaveEnvrAttrSndMngrAvail = trueblnr;
	}
	return MyEnvrAttrSndMngrAvail;
}
#endif

#if NavigationAvail
LOCALVAR blnr MyEnvrAttrNavServicesAvail;
LOCALVAR blnr HaveEnvrAttrNavServicesAvail = falseblnr;

LOCALFUNC blnr HaveNavServicesAvail(void)
{
	if (! HaveEnvrAttrNavServicesAvail) {
		MyEnvrAttrNavServicesAvail = NavServicesAvailable();
		HaveEnvrAttrNavServicesAvail = trueblnr;
	}
	return MyEnvrAttrNavServicesAvail;
}
#endif

#if HaveCPUfamM68K
LOCALVAR blnr MyEnvrAttrFSSpecCallsAvail;
LOCALVAR blnr HaveEnvrAttrFSSpecCallsAvail = falseblnr;

LOCALFUNC blnr HaveFSSpecCallsAvail(void)
{
	if (! HaveEnvrAttrFSSpecCallsAvail) {
		long reply;

		MyEnvrAttrFSSpecCallsAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltFSAttr, &reply))
			&& TestBit(reply, gestaltHasFSSpecCalls);
		HaveEnvrAttrFSSpecCallsAvail = trueblnr;
	}
	return MyEnvrAttrFSSpecCallsAvail;
}
#endif

#if Windows85APIAvail
LOCALVAR blnr MyEnvrAttrNewWndMgrAvail;
LOCALVAR blnr HaveEnvrAttrNewWndMgrAvail = falseblnr;

LOCALFUNC blnr HaveNewWndMgrAvail(void)
{
	if (! HaveEnvrAttrNewWndMgrAvail) {
		long reply;

		MyEnvrAttrNewWndMgrAvail =
			HaveGestaltAvail()
			&& (noErr == Gestalt(gestaltWindowMgrAttr, &reply))
			&& TestBit(reply, gestaltWindowMgrPresentBit);
		HaveEnvrAttrNewWndMgrAvail = trueblnr;
	}
	return MyEnvrAttrNewWndMgrAvail;
}
#endif

/* --- initial initialization --- */

#if defined(__SC__) || ((defined(powerc) || defined(__powerc)) \
	&& ! defined(__MWERKS__))

/* GLOBALVAR */ QDGlobals qd;
#endif

LOCALFUNC blnr InitMacManagers(void)
{
	MaxApplZone();

	{
		int i;

		for (i = 7; --i >= 0; ) {
			MoreMasters();
		}
	}

	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
	InitCursor();
	return trueblnr;
}


/* --- mac style errors --- */

#define CheckSavetMacErr(result) (mnvm_noErr == (err = (result)))
	/*
		where 'err' is a variable of type tMacErr in the function
		this is used in
	*/

#define To_tMacErr(result) ((tMacErr)(ui4b)(result))

#define CheckSaveMacErr(result) (CheckSavetMacErr(To_tMacErr(result)))


/*
	define NotAfileRef to some value that is different
	from any valid open file reference.
*/
#define NotAfileRef (-1)

struct MyDir_R {
	long DirId;
	short VRefNum;
};
typedef struct MyDir_R MyDir_R;

LOCALFUNC tMacErr OpenNamedFileInFolder(MyDir_R *d,
	ps3p fileName, short *refnum)
{
	tMacErr err;

#if HaveCPUfamM68K
	if (! HaveFSSpecCallsAvail()) {
		err = To_tMacErr(FSOpen(fileName, d->VRefNum, refnum));
	} else
#endif
	{
		Boolean isFolder;
		Boolean isAlias;
		FSSpec spec;

		if (CheckSaveMacErr(
			FSMakeFSSpec(d->VRefNum, d->DirId, fileName, &spec)))
		if (CheckSaveMacErr(
			ResolveAliasFile(&spec, trueblnr, &isFolder, &isAlias)))
		if (CheckSaveMacErr(
			FSpOpenDF(&spec, fsRdPerm, refnum)))
		{
		}
	}

	return err;
}

/* --- sending debugging info to file --- */

#if dbglog_HAVE

#define Support64kROM 0
#define tMyErr tMacErr

typedef unsigned char * MyPtr;

LOCALFUNC tMyErr MyHGetDir_v2(MyDir_R *d)
{
	tMyErr err;
	WDPBRec r;

	r.ioCompletion = NULL;
	r.ioNamePtr = NULL;

#if Support64kROM
	if (Have64kROM()) {
		err = PBGetVolSync((ParamBlockRec *)&r);
		d->VRefNum = r.ioVRefNum;
		d->DirId = 0;
	} else
#endif
	{
		err = PBHGetVolSync(&r);
		d->VRefNum = r.ioWDVRefNum;
		d->DirId = r.ioWDDirID;
	}

	return err;
}

LOCALFUNC tMyErr MyWriteBytes_v2(short refNum, MyPtr p, uimr L)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;
	r.ioParam.ioBuffer = (Ptr)p;
	r.ioParam.ioReqCount = L;
	r.ioParam.ioPosMode = (short) fsFromMark;
	r.ioParam.ioPosOffset = 0;

	return PBWriteSync(&r);
}

LOCALFUNC tMyErr MyCloseFile_v2(short refNum)
{
	ParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioRefNum = refNum;

	return PBCloseSync(&r);
#if 0
	return (tMyErr)FSClose(refNum);
#endif
}

#define NotAfileRef (-1)

/*
	Probably should use PBHOpenDF instead
	of PBHOpen when it is available.
	(System 7 according to Technical Note FL515)
*/

LOCALFUNC tMyErr MyFileOpen_v2(MyDir_R *d, StringPtr s,
	char Permssn, short *refnum)
{
	tMyErr err;
	HParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = Permssn;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.ioParam.ioVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBOpenSync((ParamBlockRec *)&r);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHOpenSync(&r);
	}

	if (noErr == err) {
		*refnum = r.ioParam.ioRefNum;
		/*
			Don't change *refnum unless file opened,
			so can initialize to NotAfileRef, and
			compare later before closing in uninit.
		*/
	}
	return err;
}

LOCALFUNC tMyErr MyFileOpenWrite_v2(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpen_v2(d, s, (char)fsWrPerm, refnum);
}

LOCALFUNC tMyErr MyDeleteFile_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioNamePtr = s;
	r.fileParam.ioFVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBDeleteSync((ParamBlockRec *)&r);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHDeleteSync(&r);
	}

	return err;
}

LOCALFUNC tMyErr MyCreateFile_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;
	HParamBlockRec r;

	r.fileParam.ioFlVersNum = 0;
		/*
			Think reference says to do this,
			but not Inside Mac IV
		*/

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioNamePtr = s;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioFVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBCreateSync((ParamBlockRec *)&r);
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = PBHCreateSync(&r);
	}

	return err;
}

LOCALFUNC tMyErr MyFileGetInfo_v2(MyDir_R *d, StringPtr s,
	HParamBlockRec *r)
{
	tMyErr err;

	r->fileParam.ioCompletion = NULL;
	r->fileParam.ioNamePtr = s;
	r->fileParam.ioVRefNum = d->VRefNum;
	r->fileParam.ioFVersNum = (char)0; /* needed if MFS volume */
	r->fileParam.ioFDirIndex = (short)0;

#if Support64kROM
	if (Have64kROM()) {
		err = PBGetFInfoSync((ParamBlockRec *)r);
	} else
#endif
	{
		r->fileParam.ioDirID = d->DirId;
		err = PBHGetFInfoSync(r);
	}

	return err;
}

LOCALFUNC tMyErr MyFileSetInfo_v2(MyDir_R *d, StringPtr s,
	HParamBlockRec *r)
{
	tMyErr err;

	r->fileParam.ioCompletion = NULL;
	r->fileParam.ioNamePtr = s;
	r->fileParam.ioVRefNum = d->VRefNum;
	r->fileParam.ioFVersNum = (char)0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = PBSetFInfoSync((ParamBlockRec *)r);
	} else
#endif
	{
		r->fileParam.ioDirID = d->DirId;
		err = PBHSetFInfoSync(r);
	}

	return err;
}

LOCALFUNC tMyErr MyFileSetTypeCreator_v2(MyDir_R *d, StringPtr s,
	OSType creator, OSType fileType)
{
	tMyErr err;
	HParamBlockRec r;

	if (noErr == (err = MyFileGetInfo_v2(d, s, &r))) {
		r.fileParam.ioFlFndrInfo.fdType = fileType;
		r.fileParam.ioFlFndrInfo.fdCreator = creator;
		err = MyFileSetInfo_v2(d, s, &r);
	}

	return err;
}

LOCALFUNC tMyErr MyCreateFileOverWrite_v2(MyDir_R *d, StringPtr s)
{
	tMyErr err;

	err = MyCreateFile_v2(d, s);
	if (dupFNErr == err) {
		if (noErr == (err = MyDeleteFile_v2(d, s))) {
			err = MyCreateFile_v2(d, s);
		}
	}

	return err;
}

LOCALFUNC tMyErr MyOpenNewFile_v3(MyDir_R *d, StringPtr s,
	OSType creator, OSType fileType,
	short *refnum)
/*
	Deletes old file if already exists.
*/
{
	tMyErr err;

	err = MyCreateFileOverWrite_v2(d, s);
	if (noErr == err) {
		err = MyFileSetTypeCreator_v2(d, s,
			creator, fileType);
		if (noErr == err) {
			err = MyFileOpenWrite_v2(d, s, refnum);
		}

		if (noErr != err) {
			(void) MyDeleteFile_v2(d, s);
				/* ignore any error, since already got one */
		}
	}

	return err;
}


LOCALVAR short dbglog_File = NotAfileRef;
LOCALVAR tMyErr dbglog_err = noErr;

LOCALFUNC blnr dbglog_open0(void)
{
	tMacErr err;
	MyDir_R d;

	if (noErr == (err = MyHGetDir_v2(&d))) {
		err = MyFileOpen_v2(&d, (StringPtr)"\pdbglog",
				(char)fsWrPerm, &dbglog_File);
		if (mnvm_noErr /* fnfErr */ == err) {
			err = SetEOF(dbglog_File, 0);
		} else {
			err = MyOpenNewFile_v3(&d, (StringPtr)"\pdbglog",
				'MPS ', 'TEXT', &dbglog_File);
			err = mnvm_noErr;
		}

	}

	return (mnvm_noErr == err);
}

LOCALPROC dbglog_write0(char *s, uimr L)
{
	if (NotAfileRef != dbglog_File)
	if (noErr == dbglog_err)
	{
		dbglog_err = MyWriteBytes_v2(dbglog_File, (MyPtr)s, L);
	}
}

LOCALPROC dbglog_close0(void)
{
	if (NotAfileRef != dbglog_File) {
		(void)  MyCloseFile_v2(dbglog_File);
		dbglog_File = NotAfileRef;
	}
}

#endif /* dbglog_HAVE */


/* --- control mode and internationalization --- */

#define NeedCell2MacAsciiMap 1

#define WantColorTransValid 1

#include "INTLCHAR.h"

#include "COMOSGLU.h"

#define WantKeyboard_RemapMac 1

#include "CONTROLM.h"

/* --- some simple utilities --- */

GLOBALOSGLUPROC MyMoveBytes(anyp srcPtr, anyp destPtr, si5b byteCount)
{
	BlockMove((Ptr)srcPtr, (Ptr)destPtr, byteCount);
}

/* don't want to include c libraries, so: */
LOCALFUNC si5b CStrLen(char *src)
{
	char *p = src;
	while (*p++ != 0) {
	}
	return ((si5b)p) - ((si5b)src) - 1;
}

#define PStrMaxLength 255

LOCALPROC PStrFromCStr(ps3p r, /* CONST */ char *s)
{
	unsigned short L;

	L = CStrLen(s);
	if (L > PStrMaxLength) {
		L = PStrMaxLength;
	}
	*r++ = L;
	MyMoveBytes((anyp)s, (anyp)r, L);
}

LOCALPROC PStrFromChar(ps3p r, char x)
{
	r[0] = 1;
	r[1] = (char)x;
}

LOCALPROC PStrFromHandle(ps3p r, Handle h, ui5b MaxL)
{
	ui5b L = GetHandleSize(h);

	if (L > MaxL) {
		L = MaxL;
	}

	*r++ = L;
	BlockMove(*h, (Ptr)r, L);
}

LOCALFUNC tMacErr PStrToHand(ps3p s, Handle *r)
{
	return To_tMacErr(PtrToHand((Ptr)(s + 1), r, s[0]));
}

/* --- utilities for adapting to the environment --- */

#ifndef MightNotHaveAppearanceMgrAvail
#define MightNotHaveAppearanceMgrAvail 1
#endif

#ifndef MightNotHaveWindows85Avail
#define MightNotHaveWindows85Avail MightNotHaveAppearanceMgrAvail
#endif

#ifndef LowMemAPIAvail
#define LowMemAPIAvail 1
#endif

#ifndef My_LMGetTime
#if LowMemAPIAvail
#define My_LMGetTime LMGetTime
#else
#define My_LMGetTime() (*(SInt32 *)(0x020C))
#endif
#endif

#ifndef My_LMGetMBarHeight
#if LowMemAPIAvail
#define My_LMGetMBarHeight LMGetMBarHeight
#else
#define My_LMGetMBarHeight() (*(short *)(0x0BAA))
#endif
#endif

#ifndef My_GetGrayRgn
#if /* LowMemAPIAvail */ 0
#define My_GetGrayRgn LMGetGrayRgn
#else
#define My_GetGrayRgn() (*(RgnHandle *)(0x9EE))
#endif
#endif

#ifndef My_LMGetCurApName
#if LowMemAPIAvail
#define My_LMGetCurApName LMGetCurApName
#else
#define My_LMGetCurApName() ((StringPtr) 0x0910)
#endif
#endif

#define MyGetScreenBitsBounds(r) (*r) = qd.screenBits.bounds

#define My_GetRegionBounds(region, bounds) *(bounds) = \
	(**(region)).rgnBBox

#define My_GetPortPixMap(p) ((p)->portPixMap)

#ifndef My_WindowRef
#if NewNamesAvail
#define My_WindowRef WindowRef
#else
#define My_WindowRef WindowPtr
#endif
#endif

#define My_SetPortWindowPort(w) SetPort(w)

LOCALPROC My_InvalWindowRect(My_WindowRef mw, Rect *r)
{
	GrafPtr SavePort;

	GetPort(&SavePort);
	My_SetPortWindowPort(mw);
	InvalRect(r);
	SetPort(SavePort);
}

#define My_GetWindowPortBounds(w, r) *(r) = ((w)->portRect)

LOCALPROC InvalWholeWindow(My_WindowRef mw)
{
	Rect bounds;

	My_GetWindowPortBounds(mw, &bounds);
	My_InvalWindowRect(mw, &bounds);
}

LOCALPROC MySetMacWindContRect(My_WindowRef mw, Rect *r)
{
#if Windows85APIAvail
	if (HaveNewWndMgrAvail()) {
		(void) SetWindowBounds (mw, kWindowContentRgn, r);
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		MoveWindow(mw, r->left, r->top, falseblnr);
		SizeWindow(mw, r->right - r->left, r->bottom - r->top,
			trueblnr);
#endif
	}
	InvalWholeWindow(mw);
}

LOCALFUNC blnr MyGetWindowTitleBounds(My_WindowRef mw, Rect *r)
{
#if Windows85APIAvail
	if (HaveNewWndMgrAvail()) {
		return (noErr == GetWindowBounds(mw,
				kWindowTitleBarRgn, r));
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		My_GetRegionBounds(((WindowPeek)mw)->strucRgn, r);
		r->bottom = r->top + 15;
		r->left += 4;
		r->right -= 4;
#endif
		return trueblnr;
	}
}

#define topLeft(r) (((Point *) &(r))[0])
#define botRight(r) (((Point *) &(r))[1])

LOCALFUNC blnr MyGetWindowContBounds(My_WindowRef mw, Rect *r)
{
#if Windows85APIAvail
	if (HaveNewWndMgrAvail()) {
		return (noErr == GetWindowBounds(mw,
				kWindowContentRgn, r));
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		GrafPtr oldPort;
		GetPort(&oldPort);
		My_SetPortWindowPort(mw);
		My_GetWindowPortBounds(mw, r);
		LocalToGlobal(&topLeft(*r));
		LocalToGlobal(&botRight(*r));
		SetPort(oldPort);
#endif
		return trueblnr;
	}
}

LOCALPROC MyGetGrayRgnBounds(Rect *r)
{
	My_GetRegionBounds(My_GetGrayRgn(), (Rect *)r);
}

/* --- main window data --- */

LOCALVAR WindowPtr gMyMainWindow = NULL;

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

#if EnableMagnify
#define MyScaledHeight (MyWindowScale * vMacScreenHeight)
#define MyScaledWidth (MyWindowScale * vMacScreenWidth)
#endif

#if EnableMagnify
LOCALVAR ui3p ScalingBuff = nullpr;
#endif

#if EnableMagnify

LOCALVAR ui3p ScalingTabl = nullpr;
#define ScalingTablsz (256 * MyWindowScale)

#define ScrnMapr_DoMap UpdateScaledBWCopy
#define ScrnMapr_Src GetCurDrawBuff()
#define ScrnMapr_Dst ScalingBuff
#define ScrnMapr_SrcDepth 0
#define ScrnMapr_DstDepth 0
#define ScrnMapr_Map ScalingTabl
#define ScrnMapr_Scale MyWindowScale

#include "SCRNMAPR.h"

#endif

#if EnableMagnify
LOCALPROC SetUpScalingTabl(void)
{
	ui3b *p4;
	int i;
	int j;
	int k;
	ui3r bitsRemaining;
	ui3b t1;
	ui3b t2;

	p4 = ScalingTabl;
	for (i = 0; i < 256; ++i) {
		bitsRemaining = 8;
		t2 = 0;
		for (j = 8; --j >= 0; ) {
			t1 = (i >> j) & 1;
			for (k = MyWindowScale; --k >= 0; ) {
				t2 = (t2 << 1) | t1;
				if (--bitsRemaining == 0) {
					*p4++ = t2;
					bitsRemaining = 8;
					t2 = 0;
				}
			}
		}
	}
}
#endif

LOCALPROC DefaultDrawScreenBuff(si4b top, si4b left,
	si4b bottom, si4b right)
{
	BitMap src;
	Rect SrcRect;
	Rect DstRect;

	SrcRect.left = left;
	SrcRect.right = right;
	SrcRect.top = top;
	SrcRect.bottom = bottom;

	src.rowBytes = vMacScreenMonoByteWidth;
	SetRect(&src.bounds, 0, 0, vMacScreenWidth, vMacScreenHeight);
#if EnableMagnify
	if (UseMagnify) {

		if (! ColorTransValid) {
			SetUpScalingTabl();
			ColorTransValid = trueblnr;
		}

		UpdateScaledBWCopy(top, left, bottom, right);

		MyScaleRect(&SrcRect);
		MyScaleRect(&src.bounds);

		src.baseAddr = (Ptr)ScalingBuff;
		src.rowBytes *= MyWindowScale;
	} else
#endif
	{
		src.baseAddr = (Ptr)GetCurDrawBuff();
	}
	SetScrnRectFromCoords(&DstRect, top, left, bottom, right);
	CopyBits(&src,
		&gMyMainWindow->portBits,
		&SrcRect, &DstRect, srcCopy, NULL);
	/* FrameRect(&SrcRect); for testing */
}

LOCALPROC Update_Screen(void)
{
	GrafPtr savePort;

	GetPort(&savePort);
	My_SetPortWindowPort(gMyMainWindow);

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		PaintRect(&gMyMainWindow->portRect);
	}
#endif

	DefaultDrawScreenBuff(0, 0, vMacScreenHeight, vMacScreenWidth);
	SetPort(savePort);
}

LOCALPROC HaveChangedScreenBuff(si4b top, si4b left,
	si4b bottom, si4b right)
{
#if 0 /* experimental code in progress */
	if (UseFullScreen)
	{
		{
			PixMapHandle pm= (**GetMainDevice()).gdPMap;

			/* LockPixels(pm); */
#if EnableMagnify
			if (! UseMagnify) {
#define PixelT ui5b
				PixelT *p1 = (PixelT *)GetPixBaseAddr(pm);
				int i;
				int j;
				int k;
				ui5b *p0 = (ui5b *)GetCurDrawBuff();
				ui5b SkipBytes = GetPixRowBytes(pm)
					- sizeof(PixelT) * vMacScreenWidth;
				ui5b t0;
				PixelT a[2];

				((Ptr)p1) += (long)GetPixRowBytes(pm) * (top + vOffset);
				p1 += hOffset;
				p0 += (long)top * vMacScreenWidth / 32;

				a[0] = (PixelT) -1;
				a[1] = 0;

#if 1
				for (i = bottom - top; --i >= 0; ) {
					for (j = vMacScreenWidth / 32; --j >= 0; ) {
						t0 = *p0++;

						for (k = 32; --k >= 0; ) {
							PixelT v = a[(t0 >> k) & 1];
							*p1++ = v;
						}
					}
					((Ptr)p1) += SkipBytes;
				}
#endif
			} else {
#define PixelT ui5b
				PixelT *p1 = (PixelT *)GetPixBaseAddr(pm);
				int i;
				int j;
				int k;
				ui5b *p0 = (ui5b *)GetCurDrawBuff();
				PixelT *p2;
				ui5b t0;
				PixelT a[2];

				p1 += vOffset * MyScaledWidth;
				p1 += (long)MyWindowScale * (long)MyScaledWidth * top;
				p0 += (long)top * vMacScreenWidth / 32;

				a[0] = (PixelT) -1;
				a[1] = 0;

#if 1
				for (i = bottom - top; --i >= 0; ) {
					p2 = p1;
					for (j = vMacScreenWidth / 32; --j >= 0; ) {
						t0 = *p0++;

						for (k = 32; --k >= 0; ) {
							PixelT v = a[(t0 >> k) & 1];
								/* ((t0 >> k) & 1) - 1 */
							*p1++ = v;
							*p1++ = v;
						}
					}
					for (j = MyScaledWidth; --j >= 0; ) {
						*p1++ = *p2++;
					}
				}
#endif
			}
#endif
			/* UnlockPixels(pm); */
		}
	} else
#endif
	{
		GrafPtr savePort;

		GetPort(&savePort);
		My_SetPortWindowPort(gMyMainWindow);
		DefaultDrawScreenBuff(top, left, bottom, right);
		SetPort(savePort);
	}
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

/* --- keyboard --- */

LOCALVAR ui5b LastEmKeys[4];

LOCALPROC ZapEmKeys(void)
{
	LastEmKeys[0] = 0;
	LastEmKeys[1] = 0;
	LastEmKeys[2] = 0;
	LastEmKeys[3] = 0;
}

LOCALPROC CheckKeyBoardState(void)
{
	int i;
	int j;
	ui5b NewKeys[4];

	GetKeys(*(KeyMap *)NewKeys);

	for (j = 0; j < 16; ++j) {
		ui3b k1 = ((ui3b *)NewKeys)[j];
		ui3b k2 = ((ui3b *)LastEmKeys)[j];
		ui3b k3 = k1 ^ k2;

		if (k3 != 0) {
			for (i = 0; i < 8; ++i) {
				if ((k3 & (1 << i)) != 0) {
					Keyboard_UpdateKeyMap2(Keyboard_RemapMac(j * 8 + i),
						(k1 & (1 << i)) != 0);
				}
			}
		}
	}
	for (i = 0; i < 4; ++i) {
		LastEmKeys[i] = NewKeys[i];
	}
}

LOCALVAR WantCmdOptOnReconnect = falseblnr;

#define KeyMap_TestBit(m, key) \
	((((ui3b *)m)[(key) / 8] & (1 << ((key) & 7))) != 0)

LOCALPROC ReconnectKeyCodes3(void)
/* so keys already pressed will be ignored */
{
	int i;
	int j;
	blnr oldv;
	blnr newv;
	ui5b NewKeys[4];

	GetKeys(*(KeyMap *)NewKeys);

	/* except check CapsLock */
	oldv = KeyMap_TestBit(LastEmKeys, MKC_CapsLock);
	newv = KeyMap_TestBit(NewKeys, MKC_CapsLock);
	if (oldv != newv) {
		Keyboard_UpdateKeyMap2(MKC_formac_CapsLock, newv);
	}

	/* and except for command/option on receive drop */
	if (WantCmdOptOnReconnect) {
		WantCmdOptOnReconnect = falseblnr;

		for (i = 0; i < 16; ++i) {
			ui3b v = ((ui3b *)NewKeys)[i];
			for (j = 0; j < 8; ++j) {
				if (0 != ((1 << j) & v)) {
					ui3r k = i * 8 + j;
					if (MKC_CapsLock != k) {
						Keyboard_UpdateKeyMap2(Keyboard_RemapMac(k),
							trueblnr);
					}
				}
			}
		}
	}

	for (i = 0; i < 4; ++i) {
		LastEmKeys[i] = NewKeys[i];
	}
}

/* --- cursor hiding --- */

LOCALVAR blnr HaveCursorHidden = falseblnr;
LOCALVAR blnr WantCursorHidden = falseblnr;

LOCALPROC ForceShowCursor(void)
{
	if (HaveCursorHidden) {
		HaveCursorHidden = falseblnr;
		ShowCursor();
	}
}

LOCALPROC SetCursorArrow(void)
{
	SetCursor(&qd.arrow);
}

/* --- cursor moving --- */

/*
	When "EnableFSMouseMotion" the platform
	specific code can get relative mouse
	motion, instead of absolute coordinates
	on the emulated screen. It should
	set HaveMouseMotion to true when
	it is doing this (normally when in
	full screen mode.)

	This can usually be implemented by
	hiding the platform specific cursor,
	and then keeping it within a box,
	moving the cursor back to the center whenever
	it leaves the box. This requires the
	ability to move the cursor (in MyMoveMouse).
*/

/*
	mouse moving code (non OS X) adapted from
	MoveMouse.c by Dan Sears, which says that
	"Based on code from Jon Wtte, Denis Pelli,
	Apple, and a timely suggestion from Bo Lindbergh."
	Also says 'For documentation of the CDM, see Apple
	Tech Note "HW 01 - ADB (The Untold Story: Space Aliens
	ate my mouse)"'
*/

#ifndef TARGET_CPU_PPC
#error "TARGET_CPU_PPC undefined"
#endif

#if TARGET_CPU_PPC
enum {
	glueUppCursorDeviceMoveToProcInfo =
		kD0DispatchedPascalStackBased |
		DISPATCHED_STACK_ROUTINE_SELECTOR_SIZE(kTwoByteCode) |
		RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(1,
			SIZE_CODE(sizeof(CursorDevicePtr))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(long))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(long))),
	glueUppCursorDeviceNextDeviceProcInfo =
		kD0DispatchedPascalStackBased |
		DISPATCHED_STACK_ROUTINE_SELECTOR_SIZE(kTwoByteCode) |
		RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
		DISPATCHED_STACK_ROUTINE_PARAMETER(1,
			SIZE_CODE(sizeof(CursorDevicePtr *)))
};
#endif

#if TARGET_CPU_PPC
LOCALFUNC OSErr
CallCursorDeviceMoveTo(
	CursorDevicePtr ourDevice,
	long absX,
	long absY)
{
	return CallUniversalProc(
		GetToolboxTrapAddress(_CursorDeviceDispatch),
		glueUppCursorDeviceMoveToProcInfo,
		1, ourDevice, absX, absY);
}
#else
#define CallCursorDeviceMoveTo CursorDeviceMoveTo
#endif

#if TARGET_CPU_PPC
LOCALFUNC OSErr
CallCursorDeviceNextDevice(
	CursorDevicePtr *ourDevice)
{
	return CallUniversalProc(
		GetToolboxTrapAddress(_CursorDeviceDispatch),
		glueUppCursorDeviceNextDeviceProcInfo,
		0xB, ourDevice);
}
#else
#define CallCursorDeviceNextDevice CursorDeviceNextDevice
#endif

#if ! TARGET_CPU_PPC
pascal void CallCursorTask(void) =
{
	0x2078, 0x08EE,  /* MOVE.L jCrsrTask,A0 */
	0x4E90           /* JSR (A0) */
};
#endif

/*
	Low memory globals for the mouse
*/

#define MyRawMouse   0x082C
	/* low memory global that has current mouse loc */
#define MyMTemp      0x0828
	/* low memory global that has current mouse loc */
#define MyCrsrNew    0x08CE
	/* set after you change mtemp and rawmouse */
#define MyCrsrCouple 0x08CF
	/* true if the cursor is tied to the mouse */

LOCALFUNC blnr MyMoveMouse(si4b h, si4b v)
{
	/*
		Move the cursor to the point h, v
		on the emulated screen.
		if detect that this fails return falseblnr,
		otherwise return trueblnr.
		(on some platforms it is possible to
		move the curser, but there is no
		way to detect failure.)
	*/
	GrafPtr oldPort;
	Point CurMousePos;
	Point NewMousePos;
	ui5b difftime;
	blnr IsOk;
	long int StartTime = TickCount();

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

	CurMousePos.h = h;
	CurMousePos.v = v;

	GetPort(&oldPort);
	My_SetPortWindowPort(gMyMainWindow);
	LocalToGlobal(&CurMousePos);

	do {

#if HaveCPUfamM68K
		if (! HiToolTrapAvailable(_CursorDeviceDispatch)) {
			*(Point *)MyRawMouse = CurMousePos;
			*(Point *)MyMTemp = CurMousePos;
			*(Ptr)MyCrsrNew = *(Ptr)MyCrsrCouple;
#if ! TARGET_CPU_PPC
			CallCursorTask();
#endif
		} else
#endif
		{
			CursorDevice *firstMouse = NULL;
			CallCursorDeviceNextDevice(&firstMouse);
			if (firstMouse != NULL) {
				CallCursorDeviceMoveTo(firstMouse,
					(long) CurMousePos.h,
					(long) CurMousePos.v);
			}
		}

		GetMouse(&NewMousePos);
		IsOk = (h == NewMousePos.h) && (v == NewMousePos.v);
		difftime = (ui5b)(TickCount() - StartTime);
	} while ((! IsOk) && (difftime < 5));

	SetPort(oldPort);
	return IsOk;
}

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

LOCALPROC MousePositionNotify(Point NewMousePos)
{
	blnr ShouldHaveCursorHidden = trueblnr;

#if VarFullScreen
	if (UseFullScreen)
#endif
#if MayFullScreen
	{
		NewMousePos.h -= hOffset;
		NewMousePos.v -= vOffset;
	}
#endif
#if VarFullScreen
	else
#endif
#if MayNotFullScreen
	{
		if (! PtInRgn(NewMousePos, gMyMainWindow->visRgn)) {
			ShouldHaveCursorHidden = falseblnr;
		}
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
		MyMousePositionSetDelta(
			NewMousePos.h - SavedMouseH, NewMousePos.v - SavedMouseV);
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
		MyMousePositionSet(NewMousePos.h, NewMousePos.v);
	}

	WantCursorHidden = ShouldHaveCursorHidden;
}

LOCALPROC MousePositionNotifyFromGlobal(Point NewMousePos)
{
	GrafPtr oldPort;

	GetPort(&oldPort);
	My_SetPortWindowPort(gMyMainWindow);
	GlobalToLocal(&NewMousePos);
	SetPort(oldPort);

	MousePositionNotify(NewMousePos);
}

LOCALPROC CheckMouseState(void)
{
	Point NewMousePos;
	GrafPtr oldPort;

	GetPort(&oldPort);
	My_SetPortWindowPort(gMyMainWindow);
	GetMouse(&NewMousePos);
	SetPort(oldPort);

	MousePositionNotify(NewMousePos);
}

LOCALPROC DisconnectKeyCodes3(void)
{
	DisconnectKeyCodes2();

	MyMouseButtonSet(falseblnr);

	ForceShowCursor();
}


/* --- time, date, location --- */

#define dbglog_TimeStuff (0 && dbglog_HAVE)

/*
	be sure to avoid getting confused if TickCount
	overflows and wraps.
*/

LOCALVAR ui5b TrueEmulatedTime = 0;
	/*
		The amount of time the program has
		been running, measured in Macintosh
		"ticks". There are 60.14742 ticks per
		second.

		(time when the emulation is
		stopped for more than a few ticks
		should not be counted.)
	*/

LOCALVAR long int LastTime;

LOCALPROC StartUpTimeAdjust(void)
{
	/*
		prepare to call UpdateTrueEmulatedTime.

		will be called again when haven't been
		regularly calling UpdateTrueEmulatedTime,
		(such as the emulation has been stopped).
	*/
	LastTime = TickCount();
}

LOCALPROC UpdateTrueEmulatedTime(void)
{
	/*
		Update TrueEmulatedTime. usually
		need to convert between how the
		host operating system measures
		time and Macintosh ticks. But
		not for this port.
	*/
	long int LatestTime = TickCount();
	si5b TimeDiff = LatestTime - LastTime;

	if (TimeDiff != 0) {
		LastTime = LatestTime;

		if (TimeDiff >= 0) {
			if (TimeDiff > 16) {
				/* emulation interrupted, forget it */
				++TrueEmulatedTime;

#if dbglog_TimeStuff
				dbglog_writelnNum("emulation interrupted",
					TrueEmulatedTime);
#endif
			} else {
				TrueEmulatedTime += TimeDiff;
			}
		}
	}
}

LOCALFUNC blnr CheckDateTime(void)
{
	/*
		Update CurMacDateInSeconds, the number
		of seconds since midnight January 1, 1904.

		return true if CurMacDateInSeconds is
		different than it was on the last
		call to CheckDateTime.
	*/
	ui5b NewMacDateInSecond;

	NewMacDateInSecond = My_LMGetTime();
	if (CurMacDateInSeconds != NewMacDateInSecond) {
		CurMacDateInSeconds = NewMacDateInSecond;
		return trueblnr;
	} else {
		return falseblnr;
	}
}

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

	(void) CheckDateTime();

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
			++LastTime;
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
	if (NULL == sndChannel)
#if HaveCPUfamM68K
	if (HaveSndMngrAvail())
#endif
	{
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

#define IgnorableEventMask \
	(mUpMask | keyDownMask | keyUpMask | autoKeyMask)

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
			EventRecord theEvent;

#if dbglog_SoundStuff
			dbglog_writeln("busy, so sleep");
#endif

#if HaveCPUfamM68K
			if (! HaveWaitNextEventAvail()) {
				(void) GetNextEvent(IgnorableEventMask, &theEvent);
			} else
#endif
			{
				(void) WaitNextEvent(IgnorableEventMask,
					&theEvent, 1, NULL);
			}
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

#if MayFullScreen
LOCALPROC AdjustMachineGrab(void)
{
#if EnableFSMouseMotion
	AdjustMouseMotionGrab();
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

/* --- basic dialogs --- */

LOCALPROC NativeStrFromCStr(ps3p r, char *s, blnr AddEllipsis)
{
	int i;
	int L;
	ui3b ps[ClStrMaxLength];

	ClStrFromSubstCStr(&L, ps, s);
	if (AddEllipsis) {
		ClStrAppendChar(&L, ps, kCellEllipsis);
	}

	if (L > 255) {
		L = 255;
	}

	for (i = 0; i < L; ++i) {
		r[i + 1] = Cell2MacAsciiMap[ps[i]];
	}

	r[0] = L;
}

#ifndef HogCPU
#define HogCPU 1
#endif

#if HogCPU
LOCALVAR long NoEventsCounter = 0;
#endif

LOCALVAR blnr gBackgroundFlag = falseblnr;
LOCALVAR blnr gTrueBackgroundFlag = falseblnr;
LOCALVAR blnr CurSpeedStopped = trueblnr;

LOCALVAR blnr ADialogIsUp = falseblnr;

LOCALPROC MyBeginDialog(void)
{
	DisconnectKeyCodes3();
	ADialogIsUp = trueblnr;
#if MayFullScreen
	UngrabMachine();
#endif
}

LOCALPROC MyEndDialog(void)
{
#if HogCPU
	NoEventsCounter = 0;
#endif
	ADialogIsUp = falseblnr;
	ReconnectKeyCodes3();
}


#define kMyStandardAlert 128

LOCALPROC CheckSavedMacMsg(void)
{
	/*
		This is currently only used in the
		rare case where there is a message
		still pending as the program quits.
	*/
	Str255 briefMsgp;
	Str255 longMsgp;

	if (nullpr != SavedBriefMsg) {
		NativeStrFromCStr(briefMsgp, SavedBriefMsg, falseblnr);
		NativeStrFromCStr(longMsgp, SavedLongMsg, falseblnr);
#if AppearanceAvail
		if (HaveAppearanceAvail()) {
			AlertStdAlertParamRec param;
			short itemHit;

			param.movable = 0;
			param.filterProc = nil;
			param.defaultText = "\pOK";
			param.cancelText = nil;
			param.otherText = nil;
			param.helpButton = false;
			param.defaultButton = kAlertStdAlertOKButton;
			param.cancelButton = 0;
			param.position = kWindowDefaultPosition;

			StandardAlert(
				(SavedFatalMsg) ? kAlertStopAlert : kAlertCautionAlert,
				briefMsgp, longMsgp, &param, &itemHit);
		} else
#endif
		{
			ParamText(briefMsgp, longMsgp, "\p", "\p");
			if (SavedFatalMsg) {
				while (StopAlert(kMyStandardAlert, NULL) != 1) {
				}
			} else {
				while (CautionAlert(kMyStandardAlert, NULL) != 1) {
				}
			}
			/* Alert (kMyStandardAlert, 0L); */
		}

		SavedBriefMsg = nullpr;
	}
}

/* --- hide/show menubar --- */

#if MayFullScreen
#if MightNotHaveWindows85Avail
LOCALVAR RgnHandle GrayRgnSave = NULL;
LOCALVAR short mBarHeightSave;
#endif
#endif

#if MayFullScreen
LOCALPROC My_HideMenuBar(void)
{
#if Windows85APIAvail
	if (HaveHideShowMenuAvail()) {
		if (IsMenuBarVisible()) {
			HideMenuBar();
		}
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		if (NULL == GrayRgnSave) {
			RgnHandle mBarRgn = NewRgn();
			if (mBarRgn != NULL) {
				GrayRgnSave = NewRgn();
				if (GrayRgnSave != NULL) {
					CopyRgn(My_GetGrayRgn(), GrayRgnSave);
					RectRgn(mBarRgn, &qd.screenBits.bounds);
					DiffRgn(mBarRgn, My_GetGrayRgn(), mBarRgn);
						/*
							menu bar rgn, plus corner areas
							of main screen
						*/
					mBarHeightSave = My_LMGetMBarHeight();
					LMSetMBarHeight(0);
					UnionRgn(My_GetGrayRgn(), mBarRgn, My_GetGrayRgn());
					PaintBehind(LMGetWindowList(), mBarRgn);
					CalcVisBehind(LMGetWindowList(), mBarRgn);
#if 0
					controlStripHidden = false;
					if (noErr == Gestalt(
						gestaltControlStripVersion, &result))
					{
						if (SBIsControlStripVisible()) {
							controlStripHidden = true;
							SBShowHideControlStrip(false);
						}
					}
#endif
				}
				DisposeRgn(mBarRgn);
			}
		}
#endif
	}
}
#endif

#if MayFullScreen
LOCALPROC My_ShowMenuBar(void)
{
#if Windows85APIAvail
	if (HaveHideShowMenuAvail()) {
		if (! IsMenuBarVisible()) {
			ShowMenuBar();
		}
	} else
#endif
	{
#if MightNotHaveWindows85Avail
		if (GrayRgnSave != NULL) {
			LMSetMBarHeight(mBarHeightSave);
			CopyRgn(GrayRgnSave, My_GetGrayRgn());
			/*
				PaintBehind(LMGetWindowList(), GrayRgnSave);
				CalcVisBehind(LMGetWindowList(), GrayRgnSave);
			*/
			DisposeRgn(GrayRgnSave);
			DrawMenuBar();
			GrayRgnSave = NULL;
#if 0
			if (controlStripHidden) {
				controlStripHidden = falseblnr;
				if (noErr ==
					Gestalt(gestaltControlStripVersion, &result))
				{
					SBShowHideControlStrip(true);
				}
			}
#endif
		}
#endif
	}
}
#endif

#if IncludePbufs
LOCALVAR Handle PbufDat[NumPbufs];
#endif

#if IncludePbufs
LOCALFUNC tMacErr PbufNewFromHandle(Handle h, ui5b count, tPbuf *r)
{
	tPbuf i;
	tMacErr err;

	if (! FirstFreePbuf(&i)) {
		DisposeHandle(h);
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
	Handle h;
	tMacErr err = mnvm_miscErr;

	h = NewHandleClear(count);
	if (h != NULL) {
		err = PbufNewFromHandle(h, count, r);
	}

	return err;
}
#endif

#if IncludePbufs
GLOBALOSGLUPROC PbufDispose(tPbuf i)
{
	DisposeHandle(PbufDat[i]);
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
	ui3p p;

	Handle h = PbufDat[i];

	if (NULL == h) {
		p = nullpr;
	} else {
		HLock(h);
		p = (ui3p)*h;
	}

	return p;
}
#endif

#if IncludePbufs
LOCALPROC PbufUnlock(tPbuf i)
{
	HUnlock(PbufDat[i]);
}
#endif

#if IncludePbufs
GLOBALOSGLUPROC PbufTransfer(ui3p Buffer,
	tPbuf i, ui5r offset, ui5r count, blnr IsWrite)
{
	Handle h = PbufDat[i];

	HLock(h);
	{
		void *p = ((ui3p)*h) + offset;
		if (IsWrite) {
			BlockMove(Buffer, p, count);
		} else {
			BlockMove(p, Buffer, count);
		}
	}
	HUnlock(h);
}
#endif

/* --- clipboard --- */

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEexport(tPbuf i)
{
	/*
		PBuf i is an array of macintosh
		style characters. (using the
		MacRoman character set.)

		Should export this Buffer to the
		native clipboard, performing character
		set translation, and eof character translation
		as needed. (Not needed for this port.)

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
	OSErr err;

	err = ZeroScrap();
	if (noErr == err) {
		ui5b L = PbufSize[i];
		Handle h = PbufDat[i];
		HLock(h);
		err = PutScrap(L, 'TEXT', *h);
		HUnlock(h);
	}

	PbufDispose(i);

	return (tMacErr)err;
}
#endif

#if IncludeHostTextClipExchange
GLOBALOSGLUFUNC tMacErr HTCEimport(tPbuf *r)
{
	/*
		Import the native clipboard as text,
		and convert it to Macintosh format,
		in a Pbuf.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/

	long off;
	long v;
	Handle h;
	OSErr err = (OSErr)mnvm_miscErr;

	h = NewHandle(0);
	if (h != NULL) {
		v = GetScrap(h, 'TEXT', &off);
		if (v < 0) {
			err = v;
		} else {
			err = (OSErr)PbufNewFromHandle(h, v, r);
			h = NULL;
		}
		if (NULL != h) {
			DisposeHandle(h);
		}
	}

	return (tMacErr)err;
}
#endif

/* --- drives --- */

LOCALVAR short Drives[NumDrives]; /* open disk image files */
#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
LOCALVAR Handle DriveNames[NumDrives];
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
#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
		DriveNames[i] = NULL;
#endif
	}
}

GLOBALOSGLUFUNC tMacErr vSonyTransfer(blnr IsWrite, ui3p Buffer,
	tDrive Drive_No, ui5r Sony_Start, ui5r Sony_Count,
	ui5r *Sony_ActCount)
{
	/*
		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
	tMacErr result;
	ui5r NewSony_Count = Sony_Count;

	result =
		(tMacErr)SetFPos(Drives[Drive_No], fsFromStart, Sony_Start);
	if (mnvm_noErr == result) {
		if (IsWrite) {
			/*
				write Sony_Count bytes from Buffer, to disk image
				number Drive_No, starting at offset Sony_Start.
			*/

			result = (tMacErr)FSWrite(Drives[Drive_No],
				(long *)&NewSony_Count, Buffer);
		} else {
			/*
				read Sony_Count bytes into Buffer, from disk image
				number Drive_No, starting at offset Sony_Start.
			*/

			result = (tMacErr)FSRead(Drives[Drive_No],
				(long *)&NewSony_Count, Buffer);
		}
	}

	if (nullpr != Sony_ActCount) {
		*Sony_ActCount = NewSony_Count;
	}

	return result;
}

GLOBALOSGLUFUNC tMacErr vSonyGetSize(tDrive Drive_No, ui5r *Sony_Count)
{
	/*
		set Sony_Count to the size of disk image number Drive_No.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
	return GetEOF(Drives[Drive_No], (long *)Sony_Count);
}

LOCALFUNC OSErr vSonyEject0(tDrive Drive_No)
{
	/*
		close disk image number Drive_No.

		return 0 if it succeeds, nonzero (a
		Macintosh style error code, but -1
		will do) on failure.
	*/
	short refnum = Drives[Drive_No];
	Drives[Drive_No] = NotAfileRef; /* not really needed */

	DiskEjectedNotify(Drive_No);

#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
	{
		Handle h = DriveNames[Drive_No];
		if (NULL != h) {
			DisposeHandle(h);
			DriveNames[Drive_No] = NULL; /* not really needed */
		}
	}
#endif

	return FSClose(refnum);
}

GLOBALOSGLUFUNC tMacErr vSonyEject(tDrive Drive_No)
{
	OSErr result;
	short vRefNum;
	blnr DidEject = falseblnr;
	short refnum = Drives[Drive_No];

	result = GetVRefNum(refnum, &vRefNum);
	if (noErr == result) {
		DidEject = trueblnr;
		result = vSonyEject0(Drive_No);
		(void) FlushVol(NULL, vRefNum);
	}

	if (! DidEject) {
		result = vSonyEject0(Drive_No);
	}

	return (tMacErr)result;
}

#if IncludeSonyNew
GLOBALOSGLUFUNC tMacErr vSonyEjectDelete(tDrive Drive_No)
{
	OSErr result;
	Str255 s;
	blnr DidEject = falseblnr;
	short refnum = Drives[Drive_No];

#if HaveCPUfamM68K
	if (! HaveFSSpecCallsAvail()) {
		Handle h = DriveNames[Drive_No];
		if (NULL != h) {
			short vRefNum;
			result = GetVRefNum(refnum, &vRefNum);
			if (noErr == result) {
				PStrFromHandle(s, h, 255);
				result = vSonyEject0(Drive_No);
				DidEject = trueblnr;
				(void) FSDelete(s, vRefNum);
			}
		}
	} else
#endif
	{
		FCBPBRec b;

		b.ioCompletion = NULL;
		b.ioNamePtr = (StringPtr)s;
		b.ioVRefNum = 0;
		b.ioRefNum = refnum;
		b.ioFCBIndx = 0;
		result = PBGetFCBInfoSync(&b);
		if (noErr == result) {
			FSSpec spec;
			result = FSMakeFSSpec(b.ioFCBVRefNum, b.ioFCBParID,
				s, &spec);
			if (noErr == result) {
				result = vSonyEject0(Drive_No);
				DidEject = trueblnr;
				(void) FSpDelete(&spec);
			}
		}
	}

	if (! DidEject) {
		(void) vSonyEject0(Drive_No);
	}

	return (tMacErr)result;
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

#if IncludeSonyGetName
GLOBALOSGLUFUNC tMacErr vSonyGetName(tDrive Drive_No, tPbuf *r)
{
	FCBPBRec b;
	Str255 s;
	tMacErr err = mnvm_miscErr;

#if HaveCPUfamM68K
	if (! HaveFSSpecCallsAvail()) {
		Handle h = DriveNames[Drive_No];
		if (NULL != h) {
			PStrFromHandle(s, h, 255);
			err = mnvm_noErr;
		}
	} else
#endif
	{
		b.ioCompletion = NULL;
		b.ioNamePtr = (StringPtr)s;
		b.ioVRefNum = 0;
		b.ioRefNum = Drives[Drive_No];
		b.ioFCBIndx = 0;
		err = To_tMacErr(PBGetFCBInfoSync(&b));
	}

	if (mnvm_noErr == err) {
		Handle h;
		err = PStrToHand(s, &h);
		if (noErr == err) {
			err = PbufNewFromHandle(h, s[0], r);
		}
	}

	return err;
}
#endif

LOCALFUNC tMacErr Sony_Insert0(short refnum, blnr locked, ps3p s)
{
	/*
		Given reference to open file, mount it as
		a disk image file. if "locked", then mount
		it as a locked disk.
	*/

	tDrive Drive_No;

#if ! ((IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K)
	UnusedParam(s);
#endif

	if (! FirstFreeDisk(&Drive_No)) {
		(void) FSClose(refnum);
		return mnvm_tmfoErr; /* too many files open */
	} else {
		Drives[Drive_No] = refnum;
		DiskInsertNotify(Drive_No, locked);
#if (IncludeSonyGetName || IncludeSonyNew) && HaveCPUfamM68K
		if (s != NULL) {
			Handle h;

			if (mnvm_noErr != PStrToHand(s, &h)) {
				h = NULL;
			}
			DriveNames[Drive_No] = h;
		}
#endif
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

LOCALFUNC tMacErr InsertADiskFromFileRef(FSSpec *spec)
{
	short refnum;
	tMacErr err;
	blnr locked = falseblnr;

	err = To_tMacErr(FSpOpenDF(spec, fsRdWrPerm, &refnum));
	switch (err) {
		case mnvm_permErr:
		case mnvm_wrPermErr:
		case mnvm_afpAccessDenied:
			locked = trueblnr;
			err = To_tMacErr(FSpOpenDF(spec, fsRdPerm, &refnum));
			break;
		default:
			break;
	}
	if (mnvm_noErr == err) {
		err = Sony_Insert0(refnum, locked, NULL);
	}

	return err;
}

#if HaveCPUfamM68K
LOCALFUNC tMacErr InsertADiskFromNamevRef(ConstStr255Param fileName,
	short vRefNum)
{
	ParamBlockRec R;
	tMacErr err;
	blnr locked = falseblnr;

	R.ioParam.ioCompletion = NULL;
	R.ioParam.ioNamePtr = (StringPtr)fileName;
	R.ioParam.ioVRefNum = vRefNum;
	R.ioParam.ioVersNum = 0;
	R.ioParam.ioPermssn = fsRdWrPerm;
	R.ioParam.ioMisc = NULL;
	err = To_tMacErr(PBOpen(&R, false));
	switch (err) {
		case mnvm_permErr:
		case mnvm_wrPermErr:
		case mnvm_afpAccessDenied:
			locked = trueblnr;
			R.ioParam.ioPermssn = fsRdPerm;
			err = To_tMacErr(PBOpen(&R, false));
			break;
		default:
			break;
	}
	if (mnvm_noErr == err) {
		err = Sony_Insert0(R.ioParam.ioRefNum, locked, (ps3p)fileName);
	}

	return err;
}
#endif

LOCALFUNC tMacErr LoadMacRomFromRefNum(short refnum)
{
	/*
		load the ROM image file into ptr ROM
	*/
	tMacErr err;
	long count = kROM_Size;

	if (mnvm_noErr != (err = To_tMacErr(
		FSRead(refnum, &count, ROM))))
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

LOCALFUNC tMacErr LoadMacRomFromFSSpec(FSSpec *spec)
{
	tMacErr err;
	short refnum;

	if (mnvm_noErr == (err =
		To_tMacErr(FSpOpenDF(spec, fsRdPerm, &refnum))))
	{
		err = LoadMacRomFromRefNum(refnum);
		(void) FSClose(refnum);
	}

	return err;
}

#if HaveCPUfamM68K
LOCALFUNC tMacErr LoadMacRomFromNamevRef(ConstStr255Param fileName,
	short vRefNum)
{
	tMacErr err;
	ParamBlockRec R;

	R.ioParam.ioCompletion = NULL;
	R.ioParam.ioNamePtr = (StringPtr)fileName;
	R.ioParam.ioVRefNum = vRefNum;
	R.ioParam.ioVersNum = 0;
	R.ioParam.ioPermssn = fsRdPerm;
	R.ioParam.ioMisc = NULL;
	if (mnvm_noErr == (err = To_tMacErr(PBOpen(&R, false)))) {
		err = LoadMacRomFromRefNum(R.ioParam.ioRefNum);
		(void) FSClose(R.ioParam.ioRefNum);
	}

	return err;
}
#endif

#if HaveCPUfamM68K
LOCALFUNC tMacErr InsertADiskFromNamevRef1(ConstStr255Param fileName,
	short vRefNum)
{
	tMacErr err;

	if (! ROM_loaded) {
		err = LoadMacRomFromNamevRef(fileName, vRefNum);
	} else {
		err = InsertADiskFromNamevRef(fileName, vRefNum);
	}

	return err;
}
#endif

LOCALFUNC tMacErr InsertADiskOrAliasFromSpec(FSSpec *spec,
	blnr MaybeROM, blnr MaybeAlias)
{
	Boolean isFolder;
	Boolean isAlias;
	tMacErr err;

	if ((! MaybeAlias)
		|| CheckSaveMacErr(ResolveAliasFile(spec, true,
			&isFolder, &isAlias)))
	{
		if (MaybeROM && ! ROM_loaded) {
			err = LoadMacRomFromFSSpec(spec);
		} else {
			err = InsertADiskFromFileRef(spec);
		}
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
	FSSpec spec;
	Size actualSize;

	if (CheckSaveMacErr(AECountItems(docList, &itemsInList))) {
		for (index = 1; index <= itemsInList; ++index) {
			if (CheckSaveMacErr(AEGetNthPtr(docList, index, typeFSS,
				&keyword, &typeCode, (Ptr)&spec, sizeof(FSSpec),
				&actualSize)))
			if (CheckSavetMacErr(InsertADiskOrAliasFromSpec(&spec,
				trueblnr, falseblnr)))
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

LOCALFUNC tMacErr InsertADiskFromNameEtc(MyDir_R *d,
	ConstStr255Param fileName)
{
	tMacErr err;

#if HaveCPUfamM68K
	if (! HaveFSSpecCallsAvail()) {
		err = InsertADiskFromNamevRef(fileName, d->VRefNum);
	} else
#endif
	{
		FSSpec spec;

		if (CheckSaveMacErr(
			FSMakeFSSpec(d->VRefNum, d->DirId, fileName, &spec)))
		{
			err = InsertADiskOrAliasFromSpec(&spec,
				falseblnr, trueblnr);
		}
	}

	return err;
}

#if NavigationAvail
pascal Boolean NavigationFilterProc(
	AEDesc* theItem, void* info, void* NavCallBackUserData,
	NavFilterModes theNavFilterModes);
pascal Boolean NavigationFilterProc(
	AEDesc* theItem, void* info, void* NavCallBackUserData,
	NavFilterModes theNavFilterModes)
{
	Boolean display = true;
	NavFileOrFolderInfo* theInfo = (NavFileOrFolderInfo*)info;
	UnusedParam(theNavFilterModes);
	UnusedParam(NavCallBackUserData);

	if (typeFSS == theItem->descriptorType) {
		if (! theInfo->isFolder) {
			/*
				use:
					'theInfo->fileAndFolder.fileInfo.finderInfo.fdType'
				to check for the file type you want to filter.
			*/
		}
	}
	return display;
}
#endif


#if NavigationAvail
pascal void NavigationEventProc(
	NavEventCallbackMessage callBackSelector,
	NavCBRecPtr callBackParms, void *NavCallBackUserData);
pascal void NavigationEventProc(
	NavEventCallbackMessage callBackSelector,
	NavCBRecPtr callBackParms, void *NavCallBackUserData)
{
	UnusedParam(NavCallBackUserData);

	if (kNavCBEvent == callBackSelector) {
		switch (callBackParms->eventData.eventDataParms.event->what) {
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
	}
}
#endif

#define PStrConstBlank ((ps3p)"\000")

LOCALPROC InsertADisk0(void)
{
#if NavigationAvail
#define MyDisposeNavEventUPP(userUPP) \
	DisposeRoutineDescriptor(userUPP)
#define MyDisposeNavObjectFilterUPP(userUPP) \
	DisposeRoutineDescriptor(userUPP)
#define MyNewNavObjectFilterUPP NewNavObjectFilterProc
#define MyNewNavEventUPP NewNavEventProc

	if (HaveNavServicesAvail()) {
		NavReplyRecord theReply;
		NavDialogOptions dialogOptions;
		OSErr theErr = noErr;
		NavTypeListHandle openList = NULL;
		NavObjectFilterUPP filterUPP =
			MyNewNavObjectFilterUPP(
				/* (NavObjectFilterProcPtr) */ NavigationFilterProc);
		NavEventUPP eventUPP = MyNewNavEventUPP(
			/* (NavEventProcPtr) */ NavigationEventProc);

		theErr = NavGetDefaultDialogOptions(&dialogOptions);

		dialogOptions.dialogOptionFlags |= kNavDontAutoTranslate;
		/*
			dialogOptions.dialogOptionFlags &= ~ kNavAllowMultipleFiles;
		*/
		dialogOptions.dialogOptionFlags &= ~ kNavAllowPreviews;

		MyBeginDialog();
		theErr = NavGetFile(NULL,
						&theReply,
						&dialogOptions,
						/* NULL */ eventUPP,
						NULL,
						filterUPP,
						(NavTypeListHandle)openList,
						NULL);
		MyEndDialog();

		MyDisposeNavObjectFilterUPP(filterUPP);
		MyDisposeNavEventUPP(eventUPP);


		if (noErr == theErr) {
			if (theReply.validRecord) {
				ReportStandardOpenDiskError(InsertDisksFromDocList(
					&theReply.selection));
			}

			NavDisposeReply(&theReply);
		}

	} else
#endif
#if HaveCPUfamM68K
	if (! HaveFSSpecCallsAvail()) {
		Point where;
		SFReply reply;

		where.h = 50;
		where.v = 50;
		MyBeginDialog();
		SFGetFile(*(Point *)&where, PStrConstBlank, NULL,
			-1 /* kNumFileTypes */, NULL /* fileTypes */,
			NULL, &reply);
		MyEndDialog();
		if (reply.good) {
			ReportStandardOpenDiskError(
				InsertADiskFromNamevRef1(reply.fName, reply.vRefNum));
		}
	} else
#endif
	{
		StandardFileReply reply;

		MyBeginDialog();
		StandardGetFile(0L, -1, 0L, &reply);
		MyEndDialog();
		if (reply.sfGood) {
			ReportStandardOpenDiskError(
				InsertADiskOrAliasFromSpec(&reply.sfFile,
					trueblnr, falseblnr));
		}
	}
}

#ifndef MyAppIsBundle
#define MyAppIsBundle 0
#endif

LOCALVAR MyDir_R MyDatDir;

#if MyAppIsBundle
LOCALFUNC blnr DirectorySpec2DirId(FSSpec *spec, long *dirID)
{
	CInfoPBRec b;

	b.hFileInfo.ioCompletion = NULL;
	b.hFileInfo.ioNamePtr = (StringPtr)spec->name;
	b.hFileInfo.ioVRefNum = spec->vRefNum;
	b.dirInfo.ioFDirIndex = 0;
	b.dirInfo.ioDrDirID = spec->parID;
	if (noErr == PBGetCatInfo(&b, false)) {
		*dirID = b.dirInfo.ioDrDirID;
		return trueblnr;
	} else {
		return falseblnr;
	}
}
#endif

#if MyAppIsBundle
LOCALFUNC blnr FindNamedChildDirId(short TrueParentVol,
	long ParentDirId, StringPtr ChildName,
	short *TrueChildVol, long *ChildDirId)
{

	FSSpec temp_spec;
	Boolean isFolder;
	Boolean isAlias;

	if (noErr == FSMakeFSSpec(TrueParentVol, ParentDirId,
		ChildName, &temp_spec))
	if (noErr == ResolveAliasFile(&temp_spec, true,
		&isFolder, &isAlias))
	if (isFolder)
	if (DirectorySpec2DirId(&temp_spec, ChildDirId))
	{
		*TrueChildVol = temp_spec.vRefNum;
		return trueblnr;
	}
	return falseblnr;
}
#endif

LOCALFUNC blnr InitMyApplInfo(void)
{
#if HaveCPUfamM68K
	if (! HaveFSSpecCallsAvail()) {
		if (noErr == GetVol(NULL, &MyDatDir.VRefNum)) {
			MyDatDir.DirId = 0;
			return trueblnr;
		}
	} else
#endif
	{
		FCBPBRec pb;
		Str255 fileName;

		pb.ioCompletion = NULL;
		pb.ioNamePtr = fileName;
		pb.ioVRefNum = 0;
		pb.ioRefNum = CurResFile();
		pb.ioFCBIndx = 0;
		if (noErr == PBGetFCBInfoSync(&pb)) {
			MyDatDir.VRefNum = pb.ioFCBVRefNum;
			MyDatDir.DirId = pb.ioFCBParID;
			return trueblnr;
		}
	}

	return falseblnr;
}

LOCALFUNC tMacErr MyDirFromWD_v2(short VRefNum, MyDir_R *d)
{
	tMacErr err;
	Str63 s;
	WDPBRec pb;

#if Support64kROM
	if (Have64kROM()) {
		d->VRefNum = VRefNum;
		d->DirId = 0;
		err = mnvm_noErr;
	} else
#endif
	{
		pb.ioCompletion = NULL;
		pb.ioNamePtr = s;
		pb.ioVRefNum = VRefNum;
		pb.ioWDIndex = 0;
		pb.ioWDProcID = 0;
		err = To_tMacErr(PBGetWDInfoSync(&pb));
		if (mnvm_noErr == err) {
			d->VRefNum = pb.ioWDVRefNum;
			d->DirId = pb.ioWDDirID;
		}
	}

	return err;
}

LOCALFUNC tMacErr FindPrefFolder(MyDir_R *d)
{
	tMacErr err;
	long reply;

	if (HaveGestaltAvail()
		&& (noErr == Gestalt(gestaltFindFolderAttr, &reply))
		&& TestBit(reply, gestaltFindFolderPresent)
		)
	{
		err = To_tMacErr(FindFolder(
			kOnSystemDisk,
			kPreferencesFolderType,
			kDontCreateFolder,
			&d->VRefNum,
			&d->DirId));
	} else {
		SysEnvRec info;

		err = To_tMacErr(SysEnvirons(1, &info));
		if (mnvm_noErr == err) {
			err = MyDirFromWD_v2(info.sysVRefNum, d);
		}
	}

	return err;
}

#define CatInfoIsFolder(cPB) \
	(((cPB)->hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0)

#define PStrLength(s) (*(s))
#define SizeOfListMyChar(n) (n)
#define PStrToTotSize(s) (SizeOfListMyChar(PStrLength(s) + 1))
	/* + 1 for length byte */

LOCALPROC PStrCopy(ps3p r, ps3p s)
{
	MyMoveBytes((anyp)s, (anyp)r, PStrToTotSize(s));
}

LOCALFUNC tMacErr MyFindNamedChildDir_v2(MyDir_R *src_d, StringPtr s,
	MyDir_R *dst_d)
{
	tMacErr err;
	Str255 NameBuffer;
	CInfoPBRec cPB;

	cPB.hFileInfo.ioCompletion = NULL;
	cPB.hFileInfo.ioVRefNum = src_d->VRefNum;
	cPB.dirInfo.ioDrDirID = src_d->DirId;
	cPB.hFileInfo.ioNamePtr = NameBuffer;
	PStrCopy(NameBuffer, s);
	cPB.dirInfo.ioFDirIndex = 0;

	err = To_tMacErr(PBGetCatInfoSync(&cPB));

	if (mnvm_noErr == err) {
		if (! CatInfoIsFolder(&cPB)) {
			err = mnvm_dirNFErr;
		} else {
			dst_d->VRefNum = cPB.hFileInfo.ioVRefNum;
			dst_d->DirId = cPB.dirInfo.ioDrDirID;
		}
	}

	return err;
}

LOCALFUNC tMacErr MyResolveAliasDir_v2(MyDir_R *src_d, StringPtr s,
	MyDir_R *dst_d)
{
	tMacErr err;
	FSSpec spec;
	Boolean isFolder;
	Boolean isAlias;
	MyDir_R src2_d;

	spec.vRefNum = src_d->VRefNum;
	spec.parID = src_d->DirId;
	PStrCopy(spec.name, s);
	err = To_tMacErr(
		ResolveAliasFile(&spec, true, &isFolder, &isAlias));
	if (mnvm_noErr == err) {
		if (! isAlias) {
			err = mnvm_dirNFErr;
		} else {
			src2_d.VRefNum = spec.vRefNum;
			src2_d.DirId = spec.parID;
			err = MyFindNamedChildDir_v2(&src2_d, spec.name, dst_d);
		}
	}

	return err;
}

LOCALFUNC tMacErr MyResolveNamedChildDir_v2(MyDir_R *src_d, StringPtr s,
	MyDir_R *dst_d)
{
	tMacErr err;

	err = MyFindNamedChildDir_v2(src_d, s, dst_d);
	if (mnvm_dirNFErr == err) {
		if (HaveAliasMgrAvail()) {
			err = MyResolveAliasDir_v2(src_d, s, dst_d);
		}
	}

	return err;
}

LOCALFUNC tMacErr OpenNamedFileInFolderCStr(MyDir_R *d,
	char *s, short *refnum)
{
	Str255 fileName;

	PStrFromCStr(fileName, s);
	return OpenNamedFileInFolder(d, fileName, refnum);
}

LOCALFUNC tMacErr MyResolveNamedChildDirCStr(MyDir_R *src_d,
	char *s, MyDir_R *dst_d)
{
	Str255 fileName;

	PStrFromCStr(fileName, s);
	return MyResolveNamedChildDir_v2(src_d, fileName, dst_d);
}

LOCALFUNC tMacErr LoadMacRomFromNameFolder(MyDir_R *d,
	char *s)
{
	tMacErr err;
	short refnum;

	if (mnvm_noErr == (err =
		OpenNamedFileInFolderCStr(d, s, &refnum)))
	{
		err = LoadMacRomFromRefNum(refnum);
		(void) FSClose(refnum);
	}

	return err;
}

LOCALFUNC tMacErr LoadMacRomFromPrefDir(void)
{
	tMacErr err;
	MyDir_R PrefRef;
	MyDir_R GryphelRef;
	MyDir_R ROMsRef;

	if (mnvm_noErr == (err = FindPrefFolder(&PrefRef)))
	if (mnvm_noErr == (err = MyResolveNamedChildDirCStr(&PrefRef,
		"Gryphel", &GryphelRef)))
	if (mnvm_noErr == (err = MyResolveNamedChildDirCStr(&GryphelRef,
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
		LoadMacRomFromNameFolder(&MyDatDir, RomFileName)))
	if (mnvm_fnfErr == (err =
		LoadMacRomFromPrefDir()))
	{
	}

	return trueblnr; /* keep launching Mini vMac, regardless */
}

LOCALFUNC blnr Sony_InsertIth(int i)
{
	if ((i > 9) || ! FirstFreeDisk(nullpr)) {
		return falseblnr;
	} else {
		Str255 s;
		tMacErr err = mnvm_noErr;

		PStrFromCStr(s, "disk?.dsk");

		s[5] = '0' + i;
		if (! CheckSavetMacErr(InsertADiskFromNameEtc(&MyDatDir, s))) {
			if (mnvm_fnfErr != err) {
				ReportStandardOpenDiskError(err);
			}
			return falseblnr;
		}

		return trueblnr;
	}
}

LOCALFUNC blnr LoadInitialImages(void)
{
	int i;

	for (i = 1; Sony_InsertIth(i); ++i) {
		/* stop on first error (including file not found) */
	}

	return trueblnr;
}

#if IncludeSonyNew
LOCALFUNC tMacErr WriteZero(SInt16 refnum, ui5b L)
{
#define ZeroBufferSize 2048
	tMacErr err;
	ui5b i;
	ui3b buffer[ZeroBufferSize];

	if (CheckSaveMacErr(SetFPos(refnum, fsFromStart, 0))) {

		for (i = 0; i < ZeroBufferSize; ++i) {
			buffer[i] = 0;
		}
		while (L > 0) {
			i = (L > ZeroBufferSize) ? ZeroBufferSize : L;
			err = To_tMacErr(FSWrite(refnum, (long *)&i, buffer));
			if (mnvm_noErr != err) {
				goto label_fail;
			}
			L -= i;
		}
	}

label_fail:
	return err;
}
#endif

#if HaveCPUfamM68K && IncludeSonyNew
LOCALPROC MakeNewDiskFromNamevRef(ps3p Name, short vRefNum,
	ui5b L)
{
	short refNum;
	tMacErr err;

	err = To_tMacErr(Create(Name, vRefNum, '????', '????'));
	if (mnvm_dupFNErr == err) {
		if (CheckSaveMacErr(FSDelete(Name, vRefNum))) {
			err = To_tMacErr(Create(Name, vRefNum, '????', '????'));
		}
	}
	if (mnvm_noErr == err) {
		if (CheckSaveMacErr(FSOpen(Name, vRefNum, &refNum))) {
			if (CheckSaveMacErr(SetEOF(refNum, L))) {
				if (CheckSavetMacErr(WriteZero(refNum, L))) {
					err = Sony_Insert0(refNum, falseblnr, Name);
					ReportStandardOpenDiskError(err);
					refNum = NotAfileRef;
				}
			}
			if (NotAfileRef != refNum) {
				(void) FSClose(refNum);
			}
		}
		if (mnvm_noErr != err) {
			(void) FSDelete(Name, vRefNum);
		}
	}
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDiskFromSpec(FSSpec *NewFileSpec,
	ui5b L)
{
	short refNum;
	tMacErr err;

	err = To_tMacErr(FSpCreate(NewFileSpec,
		'????', '????', smSystemScript));
	if (mnvm_dupFNErr == err) {
		err = To_tMacErr(FSpDelete(NewFileSpec));
		if (mnvm_noErr == err) {
			err = To_tMacErr(FSpCreate(NewFileSpec,
				'????', '????', smSystemScript));
		}
	}
	if (mnvm_noErr == err) {
		if (CheckSaveMacErr(
			FSpOpenDF(NewFileSpec, fsRdWrPerm, &refNum)))
		{
			if (CheckSaveMacErr(SetEOF(refNum, L))) {
				if (CheckSavetMacErr(WriteZero(refNum, L))) {
					err = Sony_Insert0(refNum, falseblnr, NULL);
					ReportStandardOpenDiskError(err);
					refNum = NotAfileRef;
				}
			}
			if (NotAfileRef != refNum) {
				(void) FSClose(refNum);
			}
		}
		if (mnvm_noErr != err) {
			(void) FSpDelete(NewFileSpec);
		}
	}
}
#endif

#if UseActvFile || (IncludeSonyNew && ! SaveDialogEnable)
LOCALFUNC tMacErr MyMakeNamedDir_v2(MyDir_R *d, StringPtr s,
	MyDir_R *new_d)
{
	tMacErr err;
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioDirID = d->DirId;
	r.fileParam.ioNamePtr = s;
	err = To_tMacErr(PBDirCreateSync(&r));
	if (mnvm_noErr == err) {
		new_d->VRefNum = d->VRefNum;
		new_d->DirId = r.fileParam.ioDirID;
	}

	return err;
}
#endif

#if UseActvFile || (IncludeSonyNew && ! SaveDialogEnable)
LOCALFUNC tMacErr FindOrMakeMakeNamedDir_v2(MyDir_R *new_d,
	MyDir_R *d, StringPtr s)
{
	tMacErr err;

	err = MyResolveNamedChildDir_v2(d, s, new_d);
	if (mnvm_fnfErr == err) {
		err = MyMakeNamedDir_v2(d, s, new_d);
	}

	return err;
}
#endif

#if UseActvFile || (IncludeSonyNew && ! SaveDialogEnable)
LOCALFUNC tMacErr FindOrMakeChildDirCStr(MyDir_R *new_d,
	MyDir_R *d, char *name)
{
	Str255 s;

	PStrFromCStr(s, name);
	return FindOrMakeMakeNamedDir_v2(new_d, d, s);
}
#endif

#if IncludeSonyNew
LOCALPROC MakeNewDisk(ui5b L, Handle NewDiskName)
{
#if SaveDialogEnable
	OSErr theErr;

#if NavigationAvail
	if (HaveNavServicesAvail()) {
		NavReplyRecord theReply;
		NavDialogOptions dialogOptions;
		NavEventUPP eventUPP = MyNewNavEventUPP(
			/* (NavEventProcPtr) */ NavigationEventProc);

		theErr = NavGetDefaultDialogOptions(&dialogOptions);
		dialogOptions.dialogOptionFlags |= kNavNoTypePopup;
#if IncludeSonyNameNew
		if (NewDiskName != NULL) {
			PStrFromHandle(dialogOptions.savedFileName,
				NewDiskName, 255);
		}
#endif
		MyBeginDialog();
		theErr = NavPutFile(NULL, &theReply, &dialogOptions,
			/* NULL */ eventUPP, '????', '????', NULL);
		MyEndDialog();

		MyDisposeNavEventUPP(eventUPP);

		if (noErr == theErr) {
			if (theReply.validRecord) {
				long itemsInList;
				AEKeyword keyword;
				DescType typeCode;
				Size actualSize;
				FSSpec NewFileSpec;

				if (noErr ==
					AECountItems(&theReply.selection, &itemsInList))
				if (1 == itemsInList)
				if (noErr == AEGetNthPtr(&theReply.selection,
					1, typeFSS, &keyword, &typeCode,
					(Ptr)&NewFileSpec, sizeof(FSSpec), &actualSize))
				{
					MakeNewDiskFromSpec(&NewFileSpec, L);
				}
			}
			NavDisposeReply(&theReply);
		}
	} else
#endif
	{
		Str255 Title;
		Str255 prompt;

#if IncludeSonyNameNew
		if (NewDiskName != NULL) {
			PStrFromHandle(Title, NewDiskName, 255);
		} else
#endif
		{
			NativeStrFromCStr(Title, "untitled", falseblnr);
		}
		NativeStrFromCStr(prompt, "Please select a file", falseblnr);

#if HaveCPUfamM68K
		if (! HaveFSSpecCallsAvail()) {
			Point where;
			SFReply reply;

			where.h = 50;
			where.v = 50;
			MyBeginDialog();
			SFPutFile(*(Point *)&where, prompt, Title, NULL, &reply);
			MyEndDialog();

			if (reply.good) {
				MakeNewDiskFromNamevRef(reply.fName,
					reply.vRefNum, L);
			}
		} else
#endif
		{
			StandardFileReply reply;

			MyBeginDialog();
			StandardPutFile(prompt, Title, &reply);
			MyEndDialog();

			if (reply.sfGood) {
				MakeNewDiskFromSpec(&reply.sfFile, L);
			}
		}
	}
#else /* SaveDialogEnable */
	tMacErr err;
	Str255 Title;
	MyDir_R OutDir;
	FSSpec spec;

#if IncludeSonyNameNew
	if (NewDiskName != NULL) {
		PStrFromHandle(Title, NewDiskName, 255);
	} else
#endif
	{
		NativeStrFromCStr(Title, "untitled", falseblnr);
	}

	if (mnvm_noErr == (err = FindOrMakeChildDirCStr(&OutDir,
		&MyDatDir, "out")))
	{
#if HaveCPUfamM68K
		if (! HaveFSSpecCallsAvail()) {
			MakeNewDiskFromNamevRef(Title, OutDir.VRefNum, L);
		} else
#endif
		{
			err = To_tMacErr(FSMakeFSSpec(OutDir.VRefNum, OutDir.DirId,
				Title, &spec));
			if ((mnvm_noErr == err) || (mnvm_fnfErr == err)) {
				MakeNewDiskFromSpec(&spec, L);
			}
		}
	}
#endif /* SaveDialogEnable */
}
#endif

#if UseActvFile

LOCALFUNC tMacErr MyCreateFile_v2(MyDir_R *d, StringPtr s)
{
	tMacErr err;
	HParamBlockRec r;

	r.fileParam.ioFlVersNum = 0;
		/*
			Think reference says to do this,
			but not Inside Mac IV
		*/

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioNamePtr = s;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioFVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = To_tMacErr(PBCreateSync((ParamBlockRec *)&r));
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = To_tMacErr(PBHCreateSync(&r));
	}

	return err;
}

LOCALFUNC tMacErr MyDeleteFile_v2(MyDir_R *d, StringPtr s)
{
	tMacErr err;
	HParamBlockRec r;

	r.fileParam.ioCompletion = NULL;
	r.fileParam.ioVRefNum = d->VRefNum;
	r.fileParam.ioNamePtr = s;
	r.fileParam.ioFVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = To_tMacErr(PBDeleteSync((ParamBlockRec *)&r));
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = To_tMacErr(PBHDeleteSync(&r));
	}

	return err;
}

LOCALFUNC tMacErr MyCreateFileOverWrite_v2(MyDir_R *d, StringPtr s)
{
	tMacErr err;

	err = MyCreateFile_v2(d, s);
	if (mnvm_dupFNErr == err) {
		if (mnvm_noErr == (err = MyDeleteFile_v2(d, s))) {
			err = MyCreateFile_v2(d, s);
		}
	}

	return err;
}

LOCALFUNC tMacErr MyFileOpen_v2(MyDir_R *d, StringPtr s,
	char Permssn, short *refnum)
{
	tMacErr err;
	HParamBlockRec r;

	r.ioParam.ioCompletion = NULL;
	r.ioParam.ioNamePtr = s;
	r.ioParam.ioVRefNum = d->VRefNum;
	r.ioParam.ioPermssn = Permssn;
	r.ioParam.ioMisc = 0; /* use volume buffer */
	r.ioParam.ioVersNum = 0; /* needed if MFS volume */

#if Support64kROM
	if (Have64kROM()) {
		err = To_tMacErr(PBOpenSync((ParamBlockRec *)&r));
	} else
#endif
	{
		r.fileParam.ioDirID = d->DirId;
		err = To_tMacErr(PBHOpenSync(&r));
	}

	if (noErr == err) {
		*refnum = r.ioParam.ioRefNum;
		/*
			Don't change *refnum unless file opened,
			so can initialize to NotAfileRef, and
			compare later before closing in uninit.
		*/
	}
	return err;
}

LOCALFUNC tMacErr MyFileOpenWrite_v2(MyDir_R *d, StringPtr s,
	short *refnum)
{
	return MyFileOpen_v2(d, s, (char)fsWrPerm, refnum);
}

LOCALFUNC tMacErr MyOpenOverWriteFile_v2(MyDir_R *d, StringPtr s,
	short *refnum)
{
	tMacErr err;

	err = MyCreateFileOverWrite_v2(d, s);
	if (mnvm_noErr == err) {
		err = MyFileOpenWrite_v2(d, s, refnum);

		if (mnvm_noErr != err) {
			(void) MyDeleteFile_v2(d, s);
				/* ignore any error, since already got one */
		}
	}

	return err;
}

LOCALFUNC tMacErr MyOpenOverWriteFileCStr(MyDir_R *d, char *name,
	short *refnum)
{
	Str255 s;

	PStrFromCStr(s, name);
	return MyOpenOverWriteFile_v2(d, s, refnum);
}

#define ActvCodeFileName "act_1"

LOCALFUNC tMacErr OpenActvCodeFile(short *refnum)
{
	tMacErr err;
	MyDir_R PrefRef;
	MyDir_R GryphelRef;
	MyDir_R ActRef;

	if (mnvm_noErr == (err = FindPrefFolder(&PrefRef)))
	if (mnvm_noErr == (err = MyResolveNamedChildDirCStr(&PrefRef,
		"Gryphel", &GryphelRef)))
	if (mnvm_noErr == (err = MyResolveNamedChildDirCStr(&GryphelRef,
		"mnvm_act", &ActRef)))
	if (mnvm_noErr == (err = OpenNamedFileInFolderCStr(&ActRef,
		ActvCodeFileName, refnum)))
	{
		/* ok */
	}

	return err;
}

LOCALFUNC tMacErr ActvCodeFileLoad(ui3p p)
{
	tMacErr err;
	short refnum;

	if (CheckSavetMacErr(OpenActvCodeFile(&refnum))) {
		long count = ActvCodeFileLen;
		err = To_tMacErr(FSRead(refnum, &count, p));
		(void) FSClose(refnum);
	}

	return err;
}

LOCALFUNC tMacErr ActvCodeFileSave(ui3p p)
{
	tMacErr err;
	short refnum;
	MyDir_R PrefRef;
	MyDir_R GryphelRef;
	MyDir_R ActRef;
	long count = ActvCodeFileLen;

	if (mnvm_noErr == (err = FindPrefFolder(&PrefRef)))
	if (mnvm_noErr == (err = FindOrMakeChildDirCStr(&GryphelRef,
		&PrefRef, "Gryphel")))
	if (mnvm_noErr == (err = FindOrMakeChildDirCStr(&ActRef,
		&GryphelRef, "mnvm_act")))
	if (mnvm_noErr == (err = MyOpenOverWriteFileCStr(&ActRef,
		ActvCodeFileName, &refnum)))
	{
		err = To_tMacErr(FSWrite(refnum, &count, p));
		(void) FSClose(refnum);
	}

	return err;
	/* return mnvm_miscErr; */
}

#endif /* UseActvFile */

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
	if (noErr == (AEGetParamDesc(theAppleEvent,
		keyDirectObject, typeAEList, &docList)))
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

#define MyNewAEEventHandlerUPP NewAEEventHandlerProc

LOCALFUNC blnr MyInstallEventHandler(AEEventClass theAEEventClass,
	AEEventID theAEEventID, ProcPtr p,
	long handlerRefcon, blnr isSysHandler)
{
	return noErr == (AEInstallEventHandler(theAEEventClass,
		theAEEventID,
#if /* useUPP */ 1
		MyNewAEEventHandlerUPP((AEEventHandlerProcPtr)p),
#else
		(AEEventHandlerUPP)p,
#endif
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

#if EnableDragDrop
static pascal OSErr GlobalTrackingHandler(short message,
	WindowRef pWindow, void *handlerRefCon, DragReference theDragRef)
{
	RgnHandle hilightRgn;
	Rect Bounds;

	UnusedParam(pWindow);
	UnusedParam(handlerRefCon);
	if (! ADialogIsUp) {
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
	}

	return noErr;
}
#endif

#if EnableDragDrop
static DragTrackingHandlerUPP gGlobalTrackingHandler = NULL;
#endif

#if EnableDragDrop
static pascal OSErr GlobalReceiveHandler(WindowRef pWindow,
	void *handlerRefCon, DragReference theDragRef)
{
	unsigned short items;
	unsigned short index;
	ItemReference theItem;
	Size SentSize;
	HFSFlavor r;

	UnusedParam(pWindow);
	UnusedParam(handlerRefCon);
	if (! ADialogIsUp)
	if (noErr == CountDragItems(theDragRef, &items))
	{
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
					InsertADiskOrAliasFromSpec(&r.fileSpec,
						trueblnr, trueblnr));
			}
		}

		if (gTrueBackgroundFlag) {
			ProcessSerialNumber currentProcess = {0, kCurrentProcess};

			(void) SetFrontProcess(&currentProcess);

			WantCmdOptOnReconnect = trueblnr;
		}
	}

	return noErr;
}
#endif

#if EnableDragDrop
static DragReceiveHandlerUPP gGlobalReceiveHandler = NULL;
#endif

#if EnableDragDrop
#define MyNewDragTrackingHandlerUPP NewDragTrackingHandlerProc
#define MyNewDragReceiveHandlerUPP NewDragReceiveHandlerProc
#if ! OPAQUE_UPP_TYPES
#define MyDisposeDragReceiveHandlerUPP(userUPP) \
	DisposeRoutineDescriptor(userUPP)
#define MyDisposeDragTrackingHandlerUPP(userUPP) \
	DisposeRoutineDescriptor(userUPP)
#else
#define MyDisposeDragReceiveHandlerUPP DisposeDragReceiveHandlerUPP
#define MyDisposeDragTrackingHandlerUPP DisposeDragTrackingHandlerUPP
#endif
#endif

#if EnableDragDrop
LOCALPROC UnPrepareForDragging(void)
{
	if (NULL != gGlobalReceiveHandler) {
		RemoveReceiveHandler(gGlobalReceiveHandler, gMyMainWindow);
		MyDisposeDragReceiveHandlerUPP(gGlobalReceiveHandler);
		gGlobalReceiveHandler = NULL;
	}
	if (NULL != gGlobalTrackingHandler) {
		RemoveTrackingHandler(gGlobalTrackingHandler, gMyMainWindow);
		MyDisposeDragTrackingHandlerUPP(gGlobalTrackingHandler);
		gGlobalTrackingHandler = NULL;
	}
}
#endif

#if EnableDragDrop
LOCALFUNC blnr PrepareForDragging(void)
{
	blnr IsOk = falseblnr;

	gGlobalTrackingHandler = MyNewDragTrackingHandlerUPP(
		GlobalTrackingHandler);
	if (gGlobalTrackingHandler != NULL) {
		gGlobalReceiveHandler = MyNewDragReceiveHandlerUPP(
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
#endif

#if EnableMagnify
#define ScaleBuffSzMult (MyWindowScale * MyWindowScale)
#endif

LOCALFUNC blnr MyCreateNewWindow(Rect *Bounds, WindowPtr *theWindow)
{
	WindowPtr ResultWin;
	blnr IsOk = falseblnr;

	ResultWin = NewWindow(
		0L, Bounds, LMGetCurApName() /* "\pMini vMac" */, false,
		noGrowDocProc, /* Could use kWindowSimpleProc for Full Screen */
		(WindowPtr) -1, true, 0);
	if (ResultWin != NULL) {
		*theWindow = ResultWin;

		IsOk = trueblnr;
	}

	return IsOk;
}

LOCALPROC ZapMyWState(void)
{
	gMyMainWindow = NULL;
	gGlobalReceiveHandler = NULL;
	gGlobalTrackingHandler = NULL;
}

LOCALPROC CloseMainWindow(void)
{
	/*
		Dispose of anything set up by CreateMainWindow.
	*/

#if EnableDragDrop
	UnPrepareForDragging();
#endif

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
	/*
		Set up somewhere for us to draw the emulated screen and
		receive mouse input. i.e. usually a window, as is the case
		for this port.

		The window should not be resizeable.

		Should look at the current value of UseMagnify and
		UseFullScreen.

	*/
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
	}  else {
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
		ShowWindow(gMyMainWindow);

		/* check if window rect valid */
#if VarFullScreen
		if (! UseFullScreen)
#endif
#if MayNotFullScreen
		{
			Rect tr;

			if (MyGetWindowTitleBounds(gMyMainWindow, &tr)) {
				if (! RectInRgn(&tr, My_GetGrayRgn())) {
					MySetMacWindContRect(gMyMainWindow,
						&MainScrnBounds);
					if (MyGetWindowTitleBounds(gMyMainWindow, &tr)) {
						if (! RectInRgn(&tr, My_GetGrayRgn())) {
							OffsetRect(&MainScrnBounds,
								0, AllScrnBounds.top - tr.top);
							MySetMacWindContRect(gMyMainWindow,
								&MainScrnBounds);
						}
					}
				}
			}
		}
#endif

#if EnableDragDrop
		if (HaveDragMgrAvail()) {
			(void) PrepareForDragging();
		}
#endif

		IsOk = trueblnr;
	}

	return IsOk;
}

struct MyWState {
	WindowPtr f_MainWindow;
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
	DragTrackingHandlerUPP f_gGlobalTrackingHandler;
	DragReceiveHandlerUPP f_gGlobalReceiveHandler;
};
typedef struct MyWState MyWState;

LOCALPROC GetMyWState(MyWState *r)
{
	r->f_MainWindow = gMyMainWindow;
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
	r->f_gGlobalTrackingHandler = gGlobalTrackingHandler;
	r->f_gGlobalReceiveHandler = gGlobalReceiveHandler;
}

LOCALPROC SetMyWState(MyWState *r)
{
	gMyMainWindow = r->f_MainWindow;
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
	gGlobalTrackingHandler = r->f_gGlobalTrackingHandler;
	gGlobalReceiveHandler = r->f_gGlobalReceiveHandler;
}

LOCALFUNC blnr ReCreateMainWindow(void)
{
	/*
		Like CreateMainWindow (which it calls), except may be
		called when already have window, without CloseMainWindow
		being called first. (Usually with different
		values of WantMagnify and WantFullScreen than
		on the previous call.)

		If there is existing window, and fail to create
		the new one, then existing window must be left alone,
		in valid state. (and return falseblnr. otherwise,
		if succeed, return trueblnr)

		i.e. can allocate the new one before disposing
		of the old one.
	*/
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

	GetMyWState(&old_state);

	ZapMyWState();

#if VarFullScreen
	UseFullScreen = WantFullScreen;
#endif
#if EnableMagnify
	UseMagnify = WantMagnify;
#endif

	ColorTransValid = falseblnr;

	if (! CreateMainWindow()) {
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
		SetMyWState(&new_state);

		if (HaveCursorHidden) {
			(void) MyMoveMouse(CurMouseH, CurMouseV);
			WantCursorHidden = trueblnr;
		}

		return trueblnr;
	}
}

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
				Rect r;

				MyGetScreenBitsBounds(&r);
				if (((r.right - r.left)
					>= vMacScreenWidth * MyWindowScale)
					&& ((r.bottom - r.top)
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

LOCALPROC LeaveBackground(void)
{
#if HogCPU
	NoEventsCounter = 0;
#endif

	SetCursorArrow();
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
#if HogCPU
			NoEventsCounter = 0;
#endif
		}
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

	if ((nullpr != SavedBriefMsg) & ! MacMsgDisplayed) {
		MacMsgDisplayOn();
	}

	if (NeedWholeScreenDraw) {
		NeedWholeScreenDraw = falseblnr;
		ScreenChangedAll();
	}

	if (gTrueBackgroundFlag) {
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
				MakeNewDisk(vSonyNewDiskSize,
					PbufDat[vSonyNewDiskName]);
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
			HideCursor();
		} else {
			ShowCursor();
		}
	}
}

GLOBALOSGLUFUNC blnr ExtraTimeNotOver(void)
{
	UpdateTrueEmulatedTime();
	return TrueEmulatedTime == OnTrueTime;
}

#define CheckItem CheckMenuItem

/* Menu Constants */

#define kAppleMenu   128
#define kFileMenu    129
#define kSpecialMenu 130

/* Apple */

enum {
	kAppleNull,

	kAppleAboutItem,
	kAppleSep1,

	kNumAppleItems
};

/* File */

enum {
	kFileNull,

	kFileOpenDiskImage,
	kFileSep1,
	kFileQuitItem,

	kNumFileItems
};

/* Special */

enum {
	kSpecialNull,

	kSpecialMoreCommandsItem,

	kNumSpecialItems
};

LOCALPROC DoOpenDA(short menuItem)
{
	Str32 name;
	GrafPtr savePort;

	GetPort(&savePort);
	GetMenuItemText(GetMenuHandle(kAppleMenu), menuItem, name);
	OpenDeskAcc(name);
	SystemTask();
	SetPort(savePort);
}

LOCALPROC MacOS_HandleMenu(short menuID, short menuItem)
{
	switch (menuID) {
		case kAppleMenu:
			if (kAppleAboutItem == menuItem) {
				DoAboutMsg();
			} else {
				DoOpenDA(menuItem);
			}
			break;

		case kFileMenu:
			switch (menuItem) {
				case kFileOpenDiskImage:
					RequestInsertDisk = trueblnr;
					break;

				case kFileQuitItem:
					RequestMacOff = trueblnr;
					break;
			}
			break;

		case kSpecialMenu:
			switch (menuItem) {
				case kSpecialMoreCommandsItem:
					DoMoreCommandsMsg();
					break;
			}
			break;

		default:
			/* if 0 == menuID, then no command chosen from menu */
			/* do nothing */
			break;
	}
}

LOCALPROC HandleMacEvent(EventRecord *theEvent)
{
	WindowPtr whichWindow;
	GrafPtr savePort;

	switch(theEvent->what) {
		case mouseDown:
			switch (FindWindow(theEvent->where, &whichWindow)) {
				case inSysWindow:
					SystemClick(theEvent, whichWindow);
					break;
				case inMenuBar:
					ForceShowCursor();
					{
						long menuSelection =
							MenuSelect(theEvent->where);
						MacOS_HandleMenu(HiWord(menuSelection),
							LoWord(menuSelection));
					}
					HiliteMenu(0);
					break;

				case inDrag:
					{
						Rect r;

						MyGetScreenBitsBounds(&r);
						DragWindow(whichWindow, theEvent->where, &r);
					}
					break;

				case inContent:
					if (FrontWindow() != whichWindow) {
						SelectWindow(whichWindow);
					}
					if (whichWindow == gMyMainWindow) {
						MousePositionNotifyFromGlobal(theEvent->where);
						MyMouseButtonSet(trueblnr);
					}
					break;

				case inGoAway:
					if (TrackGoAway(whichWindow, theEvent->where)) {
						RequestMacOff = trueblnr;
					}
					break;

				case inZoomIn:
				case inZoomOut:
					/* Zoom Boxes */
					break;
			}
			break;
		case mouseUp:
			MousePositionNotifyFromGlobal(theEvent->where);
			MyMouseButtonSet(falseblnr);
			break;

		case updateEvt:
			GetPort(&savePort);
			BeginUpdate((WindowPtr) theEvent->message);

			if ((WindowPtr)theEvent->message == gMyMainWindow) {
				Update_Screen();
			}

			EndUpdate((WindowPtr) theEvent->message);
			SetPort(savePort);
			break;

		case keyDown:
		case autoKey:
		case keyUp:
			/* ignore it */
			break;
		case osEvt:
			if ((theEvent->message >> 24) & suspendResumeMessage) {
				if (theEvent->message & 1) {
					gTrueBackgroundFlag = falseblnr;
				} else {
					gTrueBackgroundFlag = trueblnr;
				}
			}
			break;
		case kHighLevelEvent:
			if (kCoreEventClass == (AEEventClass)theEvent->message) {
				if (/* CheckSysCode */ noErr ==
					(AEProcessAppleEvent(theEvent)))
				{
				}
			} else {
				/* vCheckSysCode(errAENotAppleEvent); */
			}
			break;
	}
}

LOCALPROC WaitForTheNextEvent(void)
{
	/*
		Wait for the next event
		from the operating system, we have nothing better
		to do. Call HandleTheEvent and return (only
		wait for one event).
	*/

	EventRecord theEvent;

	if (
#if HaveCPUfamM68K
		(! HaveWaitNextEventAvail()) ?
		GetNextEvent(everyEvent, &theEvent) :
#endif
		WaitNextEvent(everyEvent, &theEvent,
			(gTrueBackgroundFlag && ! RunInBackground)
				? 5 * 60 * 60
				: 5,
				/*
					still need to check for
					control key when SpeedStopped,
					don't get event
				*/
			NULL))
	{
		HandleMacEvent(&theEvent);
	}
}

LOCALPROC DontWaitForEvent(void)
{
	/* we're busy, but see what system wants */

	EventRecord theEvent;
	int i = 0;

#if 0 /* this seems to cause crashes on some machines */
	if (EventAvail(everyEvent, &theEvent)) {
		NoEventsCounter = 0;
#endif

		while ((
#if HaveCPUfamM68K
			(! HaveWaitNextEventAvail()) ?
			GetNextEvent(everyEvent, &theEvent) :
#endif
			WaitNextEvent(everyEvent, &theEvent, 0, NULL))
			&& (i < 10))
		{
			HandleMacEvent(&theEvent);
#if HogCPU
			NoEventsCounter = 0;
#endif
			++i;
		}
#if 0
	}
#endif
}

#define PrivateEventMask \
	(mDownMask | mUpMask | keyDownMask | keyUpMask | autoKeyMask)

#define IsPowOf2(x) (0 == ((x) & ((x) - 1)))

LOCALPROC CheckForSystemEvents(void)
{
	/*
		Handle any events that are waiting for us.
		Return immediately when no more events
		are waiting, don't wait for more.
	*/

#if HogCPU && MayFullScreen
	/*
		only hog cpu in full screen mode
	*/
	if (
#if VarFullScreen
		UseFullScreen &&
#endif
		((ui3b) -1 == SpeedValue) && ! CurSpeedStopped)
	{
		EventRecord theEvent;

		if (! OSEventAvail(everyEvent, &theEvent)) {
			/*
				if no OSEvent now, and not looking for aftermath of
				event, assume there is no event of any kind we need
				to look at
			*/
			if (NoEventsCounter < 256) {
				++NoEventsCounter;
				if (IsPowOf2(NoEventsCounter)) {
					DontWaitForEvent();
				}
			}
		} else {
			WindowPtr whichWindow;

			blnr PrivateEvent = falseblnr;
			switch (theEvent.what) {
				case keyDown:
				case autoKey:
				case keyUp:
				case mouseUp:
					PrivateEvent = trueblnr;
					break;
				case mouseDown:
					if ((inContent ==
							FindWindow(theEvent.where, &whichWindow))
						&& (whichWindow == gMyMainWindow)
						&& (FrontWindow() == whichWindow))
					{
						PrivateEvent = trueblnr;
					}
					break;
			}
			if (PrivateEvent) {
				/*
					if event can effect only us, and not looking out
					for aftermath of another event, then hog the cpu
				*/
				if (GetOSEvent(PrivateEventMask, &theEvent)) {
					HandleMacEvent(&theEvent);
				}
			} else {
				NoEventsCounter = 0;
				/*
					Have an Event, so reset NoEventsCounter, no matter
					what. WaitNextEvent can return false, even if it did
					handle an event. Such as a click in the collapse
					box. In this case we need to look out for update
					events.
				*/
				DontWaitForEvent();
			}
		}
	} else
#endif
	{
		DontWaitForEvent();
	}

	if (! gBackgroundFlag) {
		CheckKeyBoardState();
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

	/*
		Wait until the end of the current
		tick, then emulate the next tick.
	*/

	if (ExtraTimeNotOver()) {
#if HaveCPUfamM68K
		if (HaveWaitNextEventAvail())
#endif
		{
			EventRecord theEvent;

			if (WaitNextEvent(everyEvent, &theEvent, 1, NULL)) {
				HandleMacEvent(&theEvent);
#if HogCPU
				NoEventsCounter = 0;
#endif
			}
		}
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
		CheckMouseState();
	}

	OnTrueTime = TrueEmulatedTime;

#if dbglog_TimeStuff
	dbglog_writelnNum("WaitForNextTick, OnTrueTime", OnTrueTime);
#endif
}

#include "PROGMAIN.h"

LOCALPROC AppendMenuCStr(MenuHandle menu, char *s)
{
	Str255 t;

	PStrFromCStr(t, s);
	AppendMenu(menu, t);
}

LOCALPROC AppendMenuConvertCStr(MenuHandle menu,
	char *s, blnr WantEllipsis)
{
	Str255 t;

	NativeStrFromCStr(t, s, WantEllipsis);
	AppendMenu(menu, t);
}

LOCALPROC AppendMenuSep(MenuHandle menu)
{
	AppendMenuCStr(menu, "(-");
}

LOCALFUNC MenuHandle NewMenuFromConvertCStr(short menuID, char *s)
{
	Str255 r;

	NativeStrFromCStr(r, s, falseblnr);
	return NewMenu(menuID, r);
}

LOCALFUNC blnr InstallOurMenus(void)
{
	MenuHandle menu;
	Str255 s;

	PStrFromChar(s, (char)20);
	menu = NewMenu(kAppleMenu, s);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kStrMenuItemAbout, trueblnr);
		AppendMenuSep(menu);
		AppendResMenu(menu, 'DRVR');
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kFileMenu, kStrMenuFile);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kStrMenuItemOpen, trueblnr);
		{
			AppendMenuSep(menu);
			AppendMenuConvertCStr(menu,
				kStrMenuItemQuit, falseblnr);
		}
		InsertMenu(menu, 0);
	}

	menu = NewMenuFromConvertCStr(kSpecialMenu, kStrMenuSpecial);
	if (menu != NULL) {
		AppendMenuConvertCStr(menu,
			kStrMenuItemMore, trueblnr);
		InsertMenu(menu, 0);
	}

	DrawMenuBar();

	return trueblnr;
}

#if AppearanceAvail
LOCALFUNC blnr InstallOurAppearanceClient(void)
{
	if (HaveAppearanceAvail()) {
		RegisterAppearanceClient();
	}
	return trueblnr;
}
#endif

LOCALFUNC blnr InstallOurEventHandlers(void)
{
	InitKeyCodes();

	if (HaveAppleEvtMgrAvail()) {
		InstallAppleEventHandlers();
	}
	return trueblnr;
}

LOCALPROC ZapOSGLUVars(void)
{
	/*
		Set initial values of variables for
		platform dependent code, where not
		done using c initializers. (such
		as for arrays.)
	*/

	ZapEmKeys();
	InitDrives();
	ZapWinStateVars();
}

LOCALPROC ReserveAllocAll(void)
{
	/* !! must match ChooseTotMemSize in build system !! */

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
#if EnableMagnify
	ReserveAllocOneBlock(&ScalingBuff,
		vMacScreenNumBytes * (ScaleBuffSzMult), 5, falseblnr);
	ReserveAllocOneBlock(&ScalingTabl,
		ScalingTablsz, 5, falseblnr);
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
	ReserveAllocBigBlock = (ui3p)NewPtr(n);
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
	/*
		run all the initializations
		needed for the program.
	*/

	if (InitMacManagers())
	if (AllocMyMemory())
	if (InitMyApplInfo())
#if dbglog_HAVE
	if (dbglog_open())
#endif
#if AppearanceAvail
	if (InstallOurAppearanceClient())
#endif
	if (InstallOurEventHandlers())
	if (InstallOurMenus())
#if MySoundEnabled
	if (MySound_Init())
#endif
	if (ReCreateMainWindow())
	if (LoadMacRom())
	if (LoadInitialImages())
#if UseActvCode
	if (ActvCodeInit())
#endif
	if (InitLocationDat())
	if (WaitForRom())
	{
		return trueblnr;
	}
	return falseblnr;
}

LOCALPROC UnInitOSGLU(void)
{
	/*
		Do all clean ups needed
		before the program quits.
	*/

	if (MacMsgDisplayed) {
		MacMsgDisplayOff();
	}

#if MayFullScreen
	UngrabMachine();
#endif

#if MySoundEnabled
	MySound_Stop();
#endif

	CloseMainWindow();

#if MayFullScreen
	My_ShowMenuBar();
#endif

#if IncludePbufs
	UnInitPbufs();
#endif
	UnInitDrives();

#if dbglog_HAVE
	dbglog_close();
#endif

	ForceShowCursor();

	if (! gTrueBackgroundFlag) {
		CheckSavedMacMsg();
	}
}

#ifndef MainReturnsInt
#define MainReturnsInt 0
#endif

#ifndef NeedLongGlue
#define NeedLongGlue 0
#endif

#if NeedLongGlue
#define main long_main
#endif

#if MainReturnsInt
int
#else
void
#endif
main(void)
{
	ZapOSGLUVars();
	if (InitOSGLU()) {
		ProgramMain();
	}
	UnInitOSGLU();

#if MainReturnsInt
	return 0;
#endif
}
