/*
	SPFILDEF.i
	Copyright (C) 2012 Paul C. Pratt

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
	program SPecific FILe DEFinitions
*/



/* --- list of source files --- */

static void DoMYOSGLUEdepends(tDoOneDepends p)
{
	{
		char *s = nullpr;

		switch (gbo_lang) {
			case gbk_lang_eng:
				s = "STRCNENG.h";
				break;
			case gbk_lang_fre:
				s = "STRCNFRE.h";
				break;
			case gbk_lang_ita:
				s = "STRCNITA.h";
				break;
			case gbk_lang_ger:
				s = "STRCNGER.h";
				break;
			case gbk_lang_dut:
				s = "STRCNDUT.h";
				break;
			case gbk_lang_spa:
				s = "STRCNSPA.h";
				break;
			case gbk_lang_pol:
				s = "STRCNPOL.h";
				break;
			case gbk_lang_ptb:
				s = "STRCNPTB.h";
				break;
			case gbk_lang_cat:
				s = "STRCNCAT.h";
				break;
			case gbk_lang_cze:
				s = "STRCNCZE.h";
				break;
			case gbk_lang_srl:
				s = "STRCNSRL.h";
				break;
		}

		if (nullpr != s) {
			p(kDepDirCSrc, s);
		}
	}

	p(kDepDirCnfg, "STRCONST.h");
	p(kDepDirCSrc, "INTLCHAR.h");
	p(kDepDirCSrc, "COMOSGLU.h");
	if (WantLocalTalk) {
		p(kDepDirCSrc, "BPFILTER.h");
	}
	if (WantAltKeysMode) {
		p(kDepDirCSrc, "ALTKEYSM.h");
	}
	p(kDepDirCSrc, "CONTROLM.h");
	if (gbk_sndapi_none != gbo_sndapi) {
		{
			char *s = nullpr;

			switch (gbo_sndapi) {
				case gbk_sndapi_alsa:
					s = "SGLUALSA.h";
					break;
				case gbk_sndapi_ddsp:
					s = "SGLUDDSP.h";
					break;
			}

			if (nullpr != s) {
				p(kDepDirCSrc, s);
			}
		}
		p(kDepDirCnfg, "SOUNDGLU.h");
	}
}

static void DoMINEM68Kdepends(tDoOneDepends p)
{
	if (cur_mIIorIIX) {
		p(kDepDirCSrc, "FPMATHEM.h");
		p(kDepDirCSrc, "FPCPEMDV.h");
	}
}

static void DoROMEMDEVdepends(tDoOneDepends p)
{
	if (NeedScrnHack) {
		p(kDepDirCSrc, "SCRNHACK.h");
	}
	if (gbk_AHM_none != cur_AltHappyMac) {
		p(kDepDirCSrc, "HPMCHACK.h");
	}
}

static void DoAllSrcFiles(tDoOneCFile p)
{
	blnr WantSCRNMAPR = (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		|| (gbk_apifam_xwn == gbo_apifam)
		|| (gbk_apifam_sdl == gbo_apifam)
		|| (gbk_apifam_sd2 == gbo_apifam);
	blnr WantSCRNTRNS = WantSCRNMAPR && (cur_ScrnDpth != 0);

	p("CNFGRAPI", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
	p("CNFGGLOB", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
	p("SYSDEPNS", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("ENDIANAC", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("MYOSGLUE", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);

	p("STRCNENG", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_eng == gbo_lang), nullpr);
	p("STRCNFRE", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_fre == gbo_lang), nullpr);
	p("STRCNITA", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_ita == gbo_lang), nullpr);
	p("STRCNGER", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_ger == gbo_lang), nullpr);
	p("STRCNDUT", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_dut == gbo_lang), nullpr);
	p("STRCNSPA", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_spa == gbo_lang), nullpr);
	p("STRCNPOL", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_pol == gbo_lang), nullpr);
	p("STRCNPTB", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_ptb == gbo_lang), nullpr);
	p("STRCNCAT", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_cat == gbo_lang), nullpr);
	p("STRCNCZE", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_cze == gbo_lang), nullpr);
	p("STRCNSRL", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_lang_srl == gbo_lang), nullpr);

	p("STRCONST", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
	p("INTLCHAR", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("COMOSGLU", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("BPFILTER", kDepDirCSrc,
		CSrcFlagsUseHdrIf(WantLocalTalk), nullpr);
	p("ALTKEYSM", kDepDirCSrc,
		CSrcFlagsUseHdrIf(WantAltKeysMode), nullpr);
	p("ACTVCODE", kDepDirCSrc,
		CSrcFlagsUseHdrIf(WantActvCode), nullpr);
	p("CONTROLM", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);
	p("PBUFSTDC", kDepDirCSrc,
		CSrcFlagsUseHdrIf((gbk_apifam_xwn == gbo_apifam)
			|| (gbk_apifam_sd2 == gbo_apifam)
			|| (gbk_apifam_sdl == gbo_apifam)
			),
		nullpr);
	p("SCRNMAPR", kDepDirCSrc,
		CSrcFlagsUseHdrIf(WantSCRNMAPR), nullpr);
	p("SCRNTRNS", kDepDirCSrc,
		CSrcFlagsUseHdrIf(WantSCRNTRNS), nullpr);
	p("DATE2SEC", kDepDirCSrc, kCSrcFlgmNoSource, nullpr);

	p("SGLUALSA", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_sndapi_alsa == gbo_sndapi), nullpr);
	p("SGLUDDSP", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_sndapi_ddsp == gbo_sndapi), nullpr);

	p("SOUNDGLU", kDepDirCnfg,
		CSrcFlagsUseHdrIf(gbk_sndapi_none != gbo_sndapi), nullpr);

	p("OSGLUMAC", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_mac == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUOSX", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_osx == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUWIN", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_win == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUXWN", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_xwn == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUNDS", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_nds == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUGTK", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_gtk == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUSDL", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_sdl == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUSD2", kDepDirCSrc,
		kCSrcFlgmUseAPI
			| CSrcFlagsUseSrcIf(gbk_apifam_sd2 == gbo_apifam),
		DoMYOSGLUEdepends);
	p("OSGLUCCO", kDepDirCSrc,
		kCSrcFlgmUseAPI | kCSrcFlgmOjbc
			| CSrcFlagsUseSrcIf(gbk_apifam_cco == gbo_apifam),
		DoMYOSGLUEdepends);

	p("EMCONFIG", kDepDirCnfg, kCSrcFlgmNoSource, nullpr);
	p("GLOBGLUE", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("M68KITAB", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("DISAM68K", kDepDirCSrc, CSrcFlagsUseIf(WantDisasm), nullpr);
	p("FPMATHEM", kDepDirCSrc,
		CSrcFlagsUseHdrIf(cur_mIIorIIX),
		nullpr);
	p("FPCPEMDV", kDepDirCSrc,
		CSrcFlagsUseHdrIf(cur_mIIorIIX),
		nullpr);

	p("MINEM68K", kDepDirCSrc, kCSrcFlgmSortFirst, DoMINEM68Kdepends);
		/*
			Put the most speed critical part of the
			program first, to help ensure consistent
			alignment for it, regardless of changes
			to rest of program.
			Speed can depend subtly, basically
			randomly, on how code is aligned.
		*/

	p("VIAEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("VIA2EMDV", kDepDirCSrc, CSrcFlagsUseIf(EmVIA2), nullpr);
	p("IWMEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("SCCEMDEV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("RTCEMDEV", kDepDirCSrc, CSrcFlagsUseIf(EmRTC), nullpr);
	p("SCRNHACK", kDepDirCSrc,
		CSrcFlagsUseHdrIf(NeedScrnHack), nullpr);
	p("HPMCHACK", kDepDirCSrc,
		CSrcFlagsUseHdrIf(gbk_AHM_none != cur_AltHappyMac), nullpr);
	p("ROMEMDEV", kDepDirCSrc, kCSrcFlgmNone, DoROMEMDEVdepends);
	p("SCSIEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("SONYEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("SCRNEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("VIDEMDEV", kDepDirCSrc, CSrcFlagsUseIf(EmVidCard), nullpr);
	p("MOUSEMDV", kDepDirCSrc, kCSrcFlgmNone, nullpr);
	p("KBRDEMDV", kDepDirCSrc, CSrcFlagsUseIf(EmClassicKbrd), nullpr);
	p("ADBSHARE", kDepDirCSrc,
		CSrcFlagsUseHdrIf(cur_mdl >= gbk_mdl_SE), nullpr);
	p("ADBEMDEV", kDepDirCSrc, CSrcFlagsUseIf(EmADB), nullpr);
	p("PMUEMDEV", kDepDirCSrc, CSrcFlagsUseIf(EmPMU), nullpr);
	p("ASCEMDEV", kDepDirCSrc, CSrcFlagsUseIf(EmASC), nullpr);
	p("SNDEMDEV", kDepDirCSrc,
		CSrcFlagsUseIf((! EmASC) && (gbk_mdl_PB100 != cur_mdl)
			&& MySoundEnabled),
		nullpr);
	p("PROGMAIN", kDepDirCSrc, kCSrcFlgmNone, nullpr);
}

/* --- list of document types --- */

static void WriteRomExtensions(tWriteOneExtension p)
{
	p("rom");
}

static void WriteDskExtensions(tWriteOneExtension p)
{
	p("dsk");
}

static void DoAllDocTypes(tWriteOneDocType p)
{
	p("ROM", "ROM!", "Rom image", WriteRomExtensions);
	p("DSK", "MvIm", "Disk image", WriteDskExtensions);
}
