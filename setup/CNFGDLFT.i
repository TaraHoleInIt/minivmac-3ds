/*
	CNFGDLFT.i
	Copyright (C) 2018 Paul C. Pratt

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
	CoNFiGuration DeFauLTs

	set default values for things not set in CONFIGUR.i
*/


#ifndef kMaintainerName
#define kMaintainerName "unknown"
#endif


#ifndef kStrHomePage
#define kStrHomePage "(unknown)"
#endif


#ifndef cur_ide

#ifdef applec
#define cur_ide gbk_ide_mpw
#else
#define cur_ide gbk_ide_bgc
#endif

#endif /* cur_ide */


#ifndef ide_vers

#if gbk_ide_xcd == cur_ide
#define ide_vers 9410
#elif gbk_ide_msv == cur_ide
#define ide_vers 15000
#else
#define ide_vers 1
#endif

#endif /* ide_vers */


#ifndef UseCmndLine
#define UseCmndLine 0
#endif


#ifndef NeedSegmenting

#if gbk_ide_mpw == cur_ide
#define NeedSegmenting 1
#else
#define NeedSegmenting 0
#endif

#endif /* NeedSegmenting */


#ifndef gbo_script

#if gbk_ide_mpw == cur_ide
#define gbo_script gbk_script_mpw
#else
#define gbo_script gbk_script_bash
#endif

#endif /* gbo_script */


#ifndef WantWriteVarName
#define WantWriteVarName 0
#endif


#ifndef WantWriteBldOpts
#define WantWriteBldOpts 0
#endif
