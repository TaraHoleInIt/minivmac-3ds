/*
	CNFGOPTS.i
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
	CoNFiGuration OPTionS

	choices that can be used in CONFIGUR.i
*/


/* choices for cur_ide */

#define gbk_ide_mpw 1 /* Macintosh Programmers Workshop */
#define gbk_ide_mw8 2 /* Metrowerks CodeWarrior */
#define gbk_ide_bgc 3 /* Gnu tools */
#define gbk_ide_snc 4 /* Sun tools */
#define gbk_ide_msv 5 /* Microsoft Visual C++ */
#define gbk_ide_lcc 6 /* lcc-win32 - Jacob Navia */
#define gbk_ide_dvc 7 /* Bloodshed Dev-C++ */
#define gbk_ide_xcd 8 /* Apple XCode */
	/* previously Apple Project Builder */
#define gbk_ide_dmc 9 /* Digital Mars Compiler */
#define gbk_ide_plc 10 /* Pelles C Compiler */
#define gbk_ide_mgw 11 /* MinGW */
#define gbk_ide_cyg 12 /* Cygwin */
#define gbk_ide_dkp 13 /* devkitpro */
#define gbk_ide_ccc 14 /* Generic command line c compiler */
#define gbk_ide_mvc 15 /* Mini vMac C (a specific version of gcc) */

/* choices for gbo_script */

#define gbk_script_mpw 1
#define gbk_script_applescript 2
#define gbk_script_bash 3
#define gbk_script_xp 4
#define gbk_script_vbscript 5
