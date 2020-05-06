/*
	BLDUTIL4.i
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

/*
	BuiLD system UTILities part 4
*/

LOCALPROC WriteOutDummyContents(void)
{
	WriteDestFileLn(
		"This file is here because some archive extraction");
	WriteDestFileLn("software will not create an empty directory.");
}

LOCALPROC WriteMakeOutputDirectories(void)
{
	if ((gbk_ide_xcd == cur_ide) && (! UseCmndLine)) {
	} else
	if (gbk_ide_mw8 == cur_ide) {
	} else {
		WriteSectionCommentDestFile("make output directory");

		MakeSubDirectory("my_obj_d", "my_project_d", obj_d_name, "");

		WriteADstFile1("my_obj_d",
			"dummy", ".txt", "Dummy",
			WriteOutDummyContents);
	}
}

LOCALPROC WriteIdeSpecificFiles(void)
{
#if gbk_ide_mpw == cur_ide
	WriteMPWSpecificFiles();
#endif

#if gbk_ide_mvc == cur_ide
	WriteMVCSpecificFiles();
#endif

#if (gbk_ide_bgc == cur_ide) \
	|| (gbk_ide_cyg == cur_ide) \
	|| (gbk_ide_mgw == cur_ide) \
	|| (gbk_ide_dkp == cur_ide)
	WriteBashGccSpecificFiles();
#endif

#if gbk_ide_mw8 == cur_ide
	WriteMetrowerksSpecificFiles();
#endif

#if gbk_ide_snc == cur_ide
	WriteSncSpecificFiles();
#endif

#if gbk_ide_msv == cur_ide
	WriteMsvSpecificFiles();
#endif

#if gbk_ide_lcc == cur_ide
	if (UseCmndLine) {
		WriteLccW32clSpecificFiles();
	} else {
		WriteLccW32SpecificFiles();
	}
#endif

#if gbk_ide_dvc == cur_ide
	if (UseCmndLine) {
		WriteBashGccSpecificFiles();
	} else {
		WriteDevCSpecificFiles();
	}
#endif

#if gbk_ide_xcd == cur_ide
	if (UseCmndLine) {
		WriteBashGccSpecificFiles();
	} else {
		WriteXCDSpecificFiles();
	}
#endif

#if gbk_ide_dmc == cur_ide
	WriteDMCSpecificFiles();
#endif

#if gbk_ide_plc == cur_ide
	if (UseCmndLine) {
		WritePLCclSpecificFiles();
	} else {
		WritePLCSpecificFiles();
	}
#endif

#if gbk_ide_ccc == cur_ide
	WriteCccSpecificFiles();
#endif
}

LOCALPROC ResetAllCommandLineParameters(void)
{
	GNResetCommandLineParameters();
	GNDevResetCommandLineParameters();
#ifdef Have_SPBLDOPT
	SPResetCommandLineParameters();
#endif
	olv_cur = 1;
	OnlyUserOptions = falseblnr;
}

LOCALFUNC tMyErr TryAsAtOptionNot(void)
{
	tMyErr err;

	if (! CurArgIsCStr_v2("@")) {
		err = kMyErrNoMatch;
	} else
	if (OnlyUserOptions) {
		err = ReportParseFailure("Already have @");
	} else
	if (kMyErr_noErr != (err = AdvanceTheArg())) {
		/* fail */
	} else
	{
		OnlyUserOptions = trueblnr;
		err = kMyErr_noErr;
	}

	return err;
}

LOCALFUNC tMyErr TryAsXClmOptionNot(void)
{
	tMyErr err;

	if (! CurArgIsCStr_v2("!")) {
		err = kMyErrNoMatch;
	} else
	if (kMyErr_noErr != (err = AdvanceTheArg())) {
		/* fail */
	} else
	{
		err = kMyErr_noErr;
		++olv_cur;
	}

	return err;
}

LOCALFUNC tMyErr ReportUnknownSwitch(void)
{
	MyPStr t0;
	MyPStr t;

	GetCurArgAsPStr(t0);
	PStrFromCStr(t, "unknown switch : ");
	PStrAppend(t, t0);

	return ReportParseFailPStr(t);
}

LOCALFUNC tMyErr ProcessCommandLineArguments(void)
{
	tMyErr err;

	err = kMyErr_noErr;
	while ((! The_arg_end) && (kMyErr_noErr == err)) {
		if (kMyErrNoMatch == (err = TryAsGNOptionNot()))
		if (kMyErrNoMatch == (err = TryAsGNDevOptionNot()))
#ifdef Have_SPBLDOPT
		if (kMyErrNoMatch == (err = TryAsSPOptionNot()))
#endif
		if (kMyErrNoMatch == (err = TryAsAtOptionNot()))
		if (kMyErrNoMatch == (err = TryAsXClmOptionNot()))
		{
			err = ReportUnknownSwitch();
		}
	}

	return err;
}

LOCALPROC DoDocTypeAddToMainRC(void)
{
	WriteBgnDestFileLn();
	WriteUnsignedToOutput(256 + DocTypeCounter);
	WriteCStrToDestFile(
		"                     ICON    DISCARDABLE     ");
	WriteQuoteToDestFile();
	WriteDocTypeIconFileName();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteWinMainRCcontents(void)
{
	DoAllDocTypesWithSetup(DoDocTypeAddToMainRC);
}

LOCALPROC WriteWinMainRC(void)
{
	WriteADstFile1("my_config_d",
		"main", ".rc", "Resource Configuration file",
		WriteWinMainRCcontents);
}

LOCALPROC WriteConfigFiles(void)
{
	WriteAppSpecificConfigFiles();

	if (HaveMacRrscs) {
		WriteCommonCNFGRSRC();
	}

	if (gbk_apifam_win == gbo_apifam) {
		WriteWinMainRC();
	}
}


LOCALPROC MakeConfigFolder(void)
{
	WriteSectionCommentDestFile("make configuration folder");

	MakeSubDirectory("my_config_d", "my_project_d", cfg_d_name, "");
}

#if WantWriteVarName
LOCALPROC WriteAppVariationStr1(void)
{
	WriteBgnDestFileLn();
	WriteAppVariationStr();
	WriteEndDestFileLn();
}
#endif

#if WantWriteBldOpts
LOCALPROC WriteBldOpts1(void)
{
	WriteBgnDestFileLn();
	WriteBldOpts();
	WriteEndDestFileLn();
}
#endif

LOCALFUNC tMyErr DoTheCommand(void)
{
	tMyErr err;

	ResetAllCommandLineParameters();

	if (kMyErr_noErr == (err = ProcessCommandLineArguments()))
	if (kMyErr_noErr == (err = AutoChooseGNSettings()))
	if (kMyErr_noErr == (err = AutoChooseGNDevSettings()))
#ifdef Have_SPBLDOPT
	if (kMyErr_noErr == (err = AutoChooseSPSettings()))
#endif
	{
		WriteScriptLangHeader();

		WriteMakeOutputDirectories();

		MakeConfigFolder();

		WriteConfigFiles();

		if (CurPrintCFiles) {
			WriteCFilesList();
		}

#if WantWriteVarName
		WriteADstFile1("my_project_d",
			"var_name", "", "variation name",
			WriteAppVariationStr1);
#endif
#if WantWriteBldOpts
		WriteADstFile1("my_project_d",
			"bld_opts", "", "build options",
			WriteBldOpts1);
#endif

		WriteIdeSpecificFiles();
	}

	return err;
}
