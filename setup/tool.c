/*
	app.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "COREDEFS.i"

#include "CONFIGUR.i"
#include "CNFGDLFT.i"


#define MyMoveBytes(src, dst, n) \
	(void) memmove((void *)(dst), (void *)(src), n)

#define kMyErr_noErr 0 /* no error */

#define kMyErrReported 1029
	/* already notified user, no further report needed */
#define kMyErrNoMatch 1030
	/* (so try something else) always should be handled, not reported */

#if NeedSegmenting
#pragma segment Utilities
#endif

#include "STRUTILS.i"

#include "CMDARGT1.i"

#include "WRTEXTFL.i"

#if NeedSegmenting
#pragma segment Body
#endif

#include "SPBASDEF.i"

#include "GNBLDOPT.i"
#ifdef Have_SPBLDOPT
#include "SPBLDOPT.i"
#endif

#if NeedSegmenting
#pragma segment Body1
#endif

#include "BLDUTIL3.i"

#include "DFFILDEF.i"
#include "SPFILDEF.i"

#if NeedSegmenting
#pragma segment Body2
#endif

#include "USFILDEF.i"
#include "WRMACRES.i"
#include "WRMPLIST.i"
#include "WRCNFGGL.i"
#include "WRCNFGAP.i"

#if NeedSegmenting
#pragma segment Body3
#endif

#if cur_ide_mpw
#include "WRMPWFLS.i"
#endif

#if cur_ide_mw8
#include "WRMW8FLS.i"
#endif

#if cur_ide_mvc
#include "WRMVCFLS.i"
#endif

#if cur_ide_bgc \
	|| cur_ide_cyg \
	|| cur_ide_mgw \
	|| cur_ide_dkp \
	|| cur_ide_dvc \
	|| cur_ide_xcd
#include "WRBGCFLS.i"
#endif

#if cur_ide_snc
#include "WRSNCFLS.i"
#endif

#if cur_ide_msv
#include "WRMSCFLS.i"
#endif

#if cur_ide_lcc
#include "WRLCCFLS.i"
#endif

#if cur_ide_dvc
#include "WRDVCFLS.i"
#endif

#if cur_ide_xcd
#include "WRXCDFLS.i"
#endif

#if cur_ide_dmc
#include "WRDMCFLS.i"
#endif

#if cur_ide_plc
#include "WRPLCFLS.i"
#endif

#if cur_ide_ccc
#include "WRCCCFLS.i"
#endif

#if NeedSegmenting
#pragma segment Body4
#endif

#ifdef Have_SPCNFGGL
#include "SPCNFGGL.i"
#endif
#ifdef Have_SPCNFGAP
#include "SPCNFGAP.i"
#endif
#include "SPOTHRCF.i"

#if NeedSegmenting
#pragma segment Main
#endif

#include "BLDUTIL4.i"

int main(int argc, char *argv[])
{
	tMyErr err;
	int return_code = 1;

	BeginParseCommandLineArguments(argc, argv);

	err = DoTheCommand();

	if (kMyErr_noErr == err) {
		return_code = 0;
	} else {
		if (kMyErrReported != err) {
			fprintf(stderr, "Unknown Error in %s", argv[0]);
		}
	}

	return return_code;
}
