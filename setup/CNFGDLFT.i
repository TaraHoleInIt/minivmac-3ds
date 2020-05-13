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


/* ---- Current IDE ---- */

#ifdef have_cur_ide
#error "have_cur_ide should not be defined"
#endif

/* Macintosh Programmers Workshop */
#ifdef cur_ide_mpw
#if 1 != cur_ide_mpw
#error "cur_ide_mpw should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_mpw 0
#endif

/* Metrowerks CodeWarrior */
#ifdef cur_ide_mw8
#if 1 != cur_ide_mw8
#error "cur_ide_mw8 should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_mw8 0
#endif

/* Gnu tools */
#ifdef cur_ide_bgc
#if 1 != cur_ide_bgc
#error "cur_ide_bgc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_bgc 0
#endif

#ifdef cur_ide_snc
#if 1 != cur_ide_snc
#error "cur_ide_snc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_snc 0
#endif

/* Microsoft Visual C++ */
#ifdef cur_ide_msv
#if 1 != cur_ide_msv
#error "cur_ide_msv should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_msv 0
#endif

/* lcc-win32 - Jacob Navia */
#ifdef cur_ide_lcc
#if 1 != cur_ide_lcc
#error "cur_ide_lcc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_lcc 0
#endif

/* Bloodshed Dev-C++ */
#ifdef cur_ide_dvc
#if 1 != cur_ide_dvc
#error "cur_ide_dvc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_dvc 0
#endif

/* Apple XCode */
	/* previously Apple Project Builder */
#ifdef cur_ide_xcd
#if 1 != cur_ide_xcd
#error "cur_ide_xcd should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_xcd 0
#endif

/* Digital Mars Compiler */
#ifdef cur_ide_dmc
#if 1 != cur_ide_dmc
#error "cur_ide_dmc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_dmc 0
#endif

/* Pelles C Compiler */
#ifdef cur_ide_plc
#if 1 != cur_ide_plc
#error "cur_ide_plc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_plc 0
#endif

/* MinGW */
#ifdef cur_ide_mgw
#if 1 != cur_ide_mgw
#error "cur_ide_mgw should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_mgw 0
#endif

/* Cygwin */
#ifdef cur_ide_cyg
#if 1 != cur_ide_cyg
#error "cur_ide_cyg should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_cyg 0
#endif

/* devkitpro */
#ifdef cur_ide_dkp
#if 1 != cur_ide_dkp
#error "cur_ide_dkp should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_dkp 0
#endif

/* Generic command line c compiler */
#ifdef cur_ide_ccc
#if 1 != cur_ide_ccc
#error "cur_ide_ccc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_ccc 0
#endif

/* Mini vMac C (a specific version of gcc) */
#ifdef cur_ide_mvc
#if 1 != cur_ide_mvc
#error "cur_ide_mvc should be defined as 1"
#endif
#ifdef have_cur_ide
#error "two cur_ide_ definitions"
#else
#define have_cur_ide 1
#endif
#else
#define cur_ide_mvc 0
#endif


#ifndef have_cur_ide

#ifdef applec
#undef cur_ide_mpw
#define cur_ide_mpw 1
#else
#undef cur_ide_bgc
#define cur_ide_bgc 1
#endif

#define have_cur_ide 1

#endif



#ifndef ide_vers

#if cur_ide_xcd
#define ide_vers 9410
#elif cur_ide_msv
#define ide_vers 15000
#else
#define ide_vers 1
#endif

#endif /* ide_vers */


#ifndef UseCmndLine
#define UseCmndLine 0
#endif


#ifndef NeedSegmenting

#if cur_ide_mpw
#define NeedSegmenting 1
#else
#define NeedSegmenting 0
#endif

#endif /* NeedSegmenting */


/* ---- Current script language ---- */

#ifdef have_cur_script
#error "have_cur_script should not be defined"
#endif

/* Macintosh Programmers Workshop */
#ifdef cur_script_mpw
#if 1 != cur_script_mpw
#error "cur_script_mpw should be defined as 1"
#endif
#ifdef have_cur_script
#error "two cur_script_ definitions"
#else
#define have_cur_script 1
#endif
#else
#define cur_script_mpw 0
#endif

/* AppleScript */
#ifdef cur_script_applescript
#if 1 != cur_script_applescript
#error "cur_script_applescript should be defined as 1"
#endif
#ifdef have_cur_script
#error "two cur_script_ definitions"
#else
#define have_cur_script 1
#endif
#else
#define cur_script_applescript 0
#endif

/* bash */
#ifdef gbk_script_bash
#if 1 != gbk_script_bash
#error "gbk_script_bash should be defined as 1"
#endif
#ifdef have_cur_script
#error "two cur_script_ definitions"
#else
#define have_cur_script 1
#endif
#else
#define gbk_script_bash 0
#endif

/* VBScript */
#ifdef gbk_script_vbscript
#if 1 != gbk_script_vbscript
#error "gbk_script_vbscript should be defined as 1"
#endif
#ifdef have_cur_script
#error "two cur_script_ definitions"
#else
#define have_cur_script 1
#endif
#else
#define gbk_script_vbscript 0
#endif

/* XP */
#ifdef gbk_script_xp
#if 1 != gbk_script_xp
#error "gbk_script_xp should be defined as 1"
#endif
#ifdef have_cur_script
#error "two cur_script_ definitions"
#else
#define have_cur_script 1
#endif
#else
#define gbk_script_xp 0
#endif

#ifndef have_cur_script

#if cur_ide_mpw
#define cur_script_mpw 1
#else
#define cur_script_bash 1
#endif

#define have_cur_script

#endif /* have_cur_script */


#ifndef WantWriteVarName
#define WantWriteVarName 0
#endif


#ifndef WantWriteBldOpts
#define WantWriteBldOpts 0
#endif
