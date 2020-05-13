/*
	SPCNFGGL.i
	Copyright (C) 2007 Paul C. Pratt

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
	program SPecific CoNFiGuration GLobals
*/

LOCALPROC WriteAppSpecificCNFGGLOBoptions(void)
{
	WriteBlankLineToDestFile();

	WriteCompCondBool("MySoundRecenterSilence", falseblnr);

	WriteDefineUimr("kLn2SoundSampSz", cur_SoundSampSz);

	WriteBlankLineToDestFile();

#if 0 /* not used currently */
	WriteCompCondBool("Debug", gbk_dbg_off != gbo_dbg);
#endif

	WriteCompCondBool("dbglog_HAVE", DbgLogHAVE);

	WriteCompCondBool("WantAbnormalReports", gbo_AbnormalReports);

	WriteBlankLineToDestFile();

	WriteDefineUimr("NumDrives", cur_numdrives);

	WriteCompCondBool("NonDiskProtect", NonDiskProtect);

	WriteCompCondBool("IncludeSonyRawMode", (! WantMinExtn)
		&& (gbk_apifam_nds != gbo_apifam)
		&& (gbk_apifam_3ds != gbo_apifam));
	WriteCompCondBool("IncludeSonyGetName",
		(! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam)
		&& (gbk_apifam_nds != gbo_apifam)
		&& (gbk_apifam_3ds != gbo_apifam)
		&& (gbk_apifam_sdl != gbo_apifam)
		&& (gbk_apifam_sd2 != gbo_apifam));
	WriteCompCondBool("IncludeSonyNew",
		(! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam)
		&& (gbk_apifam_sdl != gbo_apifam)
		&& (gbk_apifam_sd2 != gbo_apifam)
		&& (gbk_apifam_nds != gbo_apifam)
		&& (gbk_apifam_3ds != gbo_apifam));
	WriteCompCondBool("IncludeSonyNameNew",
		(! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam)
		&& (gbk_apifam_sdl != gbo_apifam)
		&& (gbk_apifam_sd2 != gbo_apifam)
		&& (gbk_apifam_nds != gbo_apifam)
		&& (gbk_apifam_3ds != gbo_apifam));

	WriteBlankLineToDestFile();

	WriteDefineUimr("vMacScreenHeight", cur_vres);
	WriteDefineUimr("vMacScreenWidth", cur_hres);
	WriteDefineUimr("vMacScreenDepth", cur_ScrnDpth);


	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kROM_Size ");
	WriteCStrToDestFile("0x");
	WriteHexLongToOutput(1UL << cur_RomSize);
	WriteEndDestFileLn();


	WriteBlankLineToDestFile();

	WriteCompCondBool("IncludePbufs",
		1 /* ((! WantMinExtn) || WantActvCode || WantDemoMsg) */
			&& (gbk_apifam_gtk != gbo_apifam)
			&& (gbk_apifam_nds != gbo_apifam)
			&& (gbk_apifam_3ds != gbo_apifam));

	WriteDefineUimr("NumPbufs", 4);


	WriteBlankLineToDestFile();

	WriteCompCondBool("EnableMouseMotion", MyMouseMotion);

	WriteBlankLineToDestFile();

	WriteCompCondBool("IncludeHostTextClipExchange",
		1 /* ((! WantMinExtn) || WantActvCode || WantDemoMsg) */
			&& (gbk_apifam_gtk != gbo_apifam)
			&& (gbk_apifam_sdl != gbo_apifam)
			&& (gbk_apifam_nds != gbo_apifam)
			&& (gbk_apifam_3ds != gbo_apifam));

	WriteDestFileLn("#define EnableAutoSlow 1");
	WriteCompCondBool("EmLocalTalk", WantLocalTalk);
	if (WantLocalTalk) {
		WriteDestFileLn("#define LT_MayHaveEcho 1");
	}

	WriteCompCondBool("AutoLocation", WantAutoLocation);
	WriteCompCondBool("AutoTimeZone", WantAutoTimeZone);
}
