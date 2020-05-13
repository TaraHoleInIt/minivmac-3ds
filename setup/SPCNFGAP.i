/*
	SPCNFGAP.i
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
	program SPecific CoNFiGuration of API users

	(that is, configuration settings that are used
	only by platform specific code)
*/


LOCALPROC WriteAppSpecificCNFGRAPIoptions(void)
{
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define RomFileName \"");
	switch (cur_mdl) {
		case gbk_mdl_Twig43:
			WriteCStrToDestFile("Twig43.ROM");
			break;
		case gbk_mdl_Twiggy:
			WriteCStrToDestFile("Twiggy.ROM");
			break;
		case gbk_mdl_128K:
			WriteCStrToDestFile("Mac128K.ROM");
			break;
		case gbk_mdl_SE:
			WriteCStrToDestFile("MacSE.ROM");
			break;
		case gbk_mdl_SEFDHD:
			WriteCStrToDestFile("SEFDHD.ROM");
			break;
		case gbk_mdl_Classic:
			WriteCStrToDestFile("Classic.ROM");
			break;
		case gbk_mdl_PB100:
			WriteCStrToDestFile("PB100.ROM");
			break;
		case gbk_mdl_II:
			WriteCStrToDestFile("MacII.ROM");
			break;
		case gbk_mdl_IIx:
			WriteCStrToDestFile("MacIIx.ROM");
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
		default:
			WriteCStrToDestFile("vMac.ROM");
			break;
	}
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kCheckSumRom_Size ");
	if (gbk_mdl_Classic == cur_mdl) {
		WriteCStrToDestFile("0x040000"); /* 256 KB */
	} else {
		WriteCStrToDestFile("0x");
		WriteHexLongToOutput(1UL << cur_RomSize);
	}
	WriteEndDestFileLn();

	switch (cur_mdl) {
		case gbk_mdl_Twig43:
			WriteDestFileLn("#define kRomCheckSum1 0x27F4E04B");
			break;
		case gbk_mdl_Twiggy:
			WriteDestFileLn("#define kRomCheckSum1 0x2884371D");
			break;
		case gbk_mdl_128K:
			WriteDestFileLn("#define kRomCheckSum1 0x28BA61CE");
			WriteDestFileLn("#define kRomCheckSum2 0x28BA4E50");
			break;
		case gbk_mdl_SE:
			WriteDestFileLn("#define kRomCheckSum1 0xB2E362A8");
			break;
		case gbk_mdl_SEFDHD:
			WriteDestFileLn("#define kRomCheckSum1 0xB306E171");
			break;
		case gbk_mdl_Classic:
			WriteDestFileLn("#define kRomCheckSum1 0xA49F9914");
			break;
		case gbk_mdl_PB100:
			WriteDestFileLn("#define kRomCheckSum1 0x96645F9C");
			break;
		case gbk_mdl_II:
			WriteDestFileLn("#define kRomCheckSum1 0x9779D2C4");
			WriteDestFileLn("#define kRomCheckSum2 0x97221136");
			break;
		case gbk_mdl_IIx:
			WriteDestFileLn("#define kRomCheckSum1 0x97221136");
			break;
		case gbk_mdl_512Ke:
		case gbk_mdl_Plus:
		default:
			WriteDestFileLn("#define kRomCheckSum1 0x4D1EEEE1");
				/* Mac Plus ROM v 1, 'Lonely Hearts' */
			WriteDestFileLn("#define kRomCheckSum2 0x4D1EEAE1");
				/* Mac Plus ROM v 2, 'Lonely Heifers' */
			WriteDestFileLn("#define kRomCheckSum3 0x4D1F8172");
				/* Mac Plus ROM v 3, 'Loud Harmonicas' */
			break;
	}

	if (! WantCheckRomCheckSum) {
		WriteCompCondBool("CheckRomCheckSum", WantCheckRomCheckSum);
	}

	WriteCompCondBool("RomStartCheckSum", (cur_mdl >= gbk_mdl_Twiggy));

	if (DbgLogHAVE) {
		WriteDefineUimr("dbglog_buflnsz", dbglog_buflnsz);
	}

	if (gbk_targfam_wnce == gbo_targfam) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define EnableShellLinks 0");
		WriteDestFileLn("#define EnableDragDrop 0");
		WriteDestFileLn("#define UseTimerThread 0");
	} else if (gbk_targfam_lnds == gbo_targfam || gbk_targfam_lctru == gbo_targfam) {
		WriteDestFileLn("#define EnableDragDrop 0");
	} else {
		WriteDestFileLn("#define EnableDragDrop 1");
	}
	WriteCompCondBool("SaveDialogEnable", gbo_SaveDialogEnable);
	WriteCompCondBool("EnableAltKeysMode", WantAltKeysMode);
	{
		uimr i;

		for (i = 0; i < kNumSrcKeyNames; ++i) {
			WriteBgnDestFileLn();
			WriteCStrToDestFile("#define MKC_formac_");
			WriteCStrToDestFile(GetSrcKeyMapName(i));
			WriteCStrToDestFile(" MKC_");
			WriteCStrToDestFile(GetDstKeyMapName(gbo_keymap[i]));
			WriteEndDestFileLn();
		}
	}

	if (gbk_keynam_Control == ControlModeKey) {
		/* kStrCntrlKyName */
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define kControlModeKey \"");
		WriteCStrToDestFile(GetSrcKeyMapName(ControlModeKey));
		WriteCStrToDestFile("\"");
		WriteEndDestFileLn();
	}

	if (gbk_keynam_Control == gbo_EKTMap) {
		/* kStrCntrlKyName */
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define kUnMappedKey \"");
		WriteCStrToDestFile(GetDstKeyMapName(gbo_EKTMap));
		WriteCStrToDestFile("\"");
		WriteEndDestFileLn();
	}

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define MKC_UnMappedKey ");
	WriteCStrToDestFile(" MKC_");
	WriteCStrToDestFile(GetDstKeyMapName(gbo_EKTMap));
	WriteEndDestFileLn();

	WriteCompCondBool("VarFullScreen", WantVarFullScreen);
	if (WantVarFullScreen) {
		WriteCompCondBool("WantInitFullScreen", WantInitFullScreen);
	}
	WriteCompCondBool("MayFullScreen",
		WantVarFullScreen || WantInitFullScreen);
	WriteCompCondBool("MayNotFullScreen",
		WantVarFullScreen || ! WantInitFullScreen);

	WriteCompCondBool("WantInitMagnify", WantInitMagnify);

	WriteCompCondBool("EnableMagnify", 1 != cur_MagFctr);
	if (1 != cur_MagFctr) {
		WriteDefineUimr("MyWindowScale", cur_MagFctr);
	}

	if (nanblnr != WantColorImage) {
		WriteCompCondBool("UseColorImage", WantColorImage);
	}

	WriteCompCondBool("WantInitRunInBackground", WantInitBackground);
	WriteCompCondBool("WantInitNotAutoSlow", ! WantInitAutoSlow);

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define WantInitSpeedValue ");
	if (gbk_speed_AllOut == CurInitSpeed) {
		WriteCStrToDestFile("-1");
	} else {
		WriteUnsignedToOutput(CurInitSpeed - 1);
	}
	WriteEndDestFileLn();

	if (WantScreenVSync) {
		WriteDestFileLn("#define UseAGLdoublebuff 1");
	}

	if (WantGraphicsSwitching) {
		WriteDestFileLn("#define WantGraphicsSwitching 1");
	}

	if (! WantGrabKeysFS) {
		WriteDestFileLn("#define GrabKeysFullScreen 0");
	}

	WriteCompCondBool("WantEnblCtrlInt", WantEnblCtrlInt);
	WriteCompCondBool("WantEnblCtrlRst", WantEnblCtrlRst);
	WriteCompCondBool("WantEnblCtrlKtg", WantEnblCtrlKtg);

	WriteCompCondBool("NeedRequestInsertDisk",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		);

	if (WantInsertIthDisk) {
		WriteDestFileLn("#define NeedRequestIthDisk 1");
	}

	WriteCompCondBool("NeedDoMoreCommandsMsg",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		);
	WriteCompCondBool("NeedDoAboutMsg",
		(gbk_apifam_gtk == gbo_apifam)
		|| (gbk_apifam_mac == gbo_apifam)
		|| (gbk_apifam_win == gbo_apifam)
		|| (gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam)
		);
	WriteCompCondBool("UseControlKeys", trueblnr);
	WriteCompCondBool("UseActvCode", WantActvCode);
	WriteCompCondBool("EnableDemoMsg", WantDemoMsg);

	if (WantActvCode) {
		int i;

		WriteBlankLineToDestFile();
		for (i = 0; i < NumKeyCon; ++i) {
			WriteBgnDestFileLn();
			WriteCStrToDestFile("#define KeyCon");
			WriteUnsignedToOutput(i);
			WriteSpaceToDestFile();
			WriteUnsignedToOutput(KeyCon[i]);
			WriteEndDestFileLn();
		}
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("/* version and other info to display to user */");

	WriteBlankLineToDestFile();

	WriteCompCondBool("NeedIntlChars",
		(gbk_lang_eng != gbo_lang) || NeedIntl);
	if (gbk_apifam_win == gbo_apifam) {
		WriteCompCondBool("ItnlKyBdFix", ItnlKyBdFix);
	}
	WriteCDefQuote("kStrAppName", WriteStrAppUnabrevName);
	WriteCDefQuote("kAppVariationStr", WriteAppVariationStr);
	WriteCDefQuote("kStrCopyrightYear", WriteAppCopyrightYearStr);
	WriteCDefQuote("kMaintainerName", WriteMaintainerName);
	WriteCDefQuote("kStrHomePage", WriteHomePage);
}
