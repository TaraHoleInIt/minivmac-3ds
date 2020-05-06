/*
	WRMSCFLS.i
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
	WRite Microsoft C specific FiLeS
*/


LOCALPROC WriteMSVCQuotedDefine(char *s)
{
	WriteCStrToDestFile(" /D ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
}

LOCALPROC WriteCLexeFlags(void)
{
	blnr WinCE = (gbk_targfam_wnce == gbo_targfam);
	WriteCStrToDestFile("/nologo");

#if 0
	if (gbk_dbg_on != gbo_dbg) {
		/* Optimizations : Minimize Size */
		WriteCStrToDestFile(" /O1");
	} else
#endif
	{
		/* Maximize chance of correct compile */
		/* Optimizations : Disabled */
		WriteCStrToDestFile(" /Od");
	}
	if (WinCE) {
		WriteCStrToDestFile(
			" /D _WIN32_WCE=420 /D WIN32_PLATFORM_PSPC=400"
			" /D UNDER_CE=420");
		if (gbk_cpufam_arm == gbo_cpufam) {
			WriteCStrToDestFile(
				" /D \"ARM\" /D \"_ARM_\" /D \"ARMV4\"");
		} else {
			WriteCStrToDestFile(
				" /D \"_i386_\" /D \"_X86_\" /D \"x86\" /D \"i_386_\"");
		}
	} else {
		WriteMSVCQuotedDefine("WIN32");
		WriteMSVCQuotedDefine("_WINDOWS");
	}
	if (gbk_dbg_on == gbo_dbg) {
		WriteMSVCQuotedDefine("_DEBUG");
	} else {
		WriteMSVCQuotedDefine("NDEBUG");
	}
	if (WinCE) {
		WriteMSVCQuotedDefine("UNICODE");
		WriteMSVCQuotedDefine("_UNICODE");
	} else {
		if (ide_vers >= 6000) {
			WriteMSVCQuotedDefine("_MBCS");
		}
	}

	if ((gbk_dbg_on == gbo_dbg) && (gbk_targ_wx86 == cur_targ)) {
		WriteCStrToDestFile(" /Gm"); /* minimal rebuild */
	}

	/* WriteCStrToDestFile(" /GX"); enable exception handling */

	if (WinCE) {
		if (gbk_cpufam_x86 == gbo_cpufam) {
			WriteCStrToDestFile(" /Gs8192");
				/* in the default template. why? */
		}
	}

	if (gbk_dbg_on == gbo_dbg) {
		if (ide_vers >= 7000) {
			WriteCStrToDestFile(" /RTC1");
		} else {
			WriteCStrToDestFile(" /GZ");
		}
	} else {
		WriteCStrToDestFile(" /GF"); /* string pooling */
	}

	if (WinCE) {
		if (gbk_cpufam_arm == gbo_cpufam) {
			if (gbk_dbg_on == gbo_dbg) {
				WriteCStrToDestFile(" /M$(CECrtMTDebug)");
			} else {
				WriteCStrToDestFile(" /MC");
			}
		}
		/*
			default template doesn't do this for
			x86. why not?
		*/
	} else {
		if (gbk_dbg_on == gbo_dbg) {
			if ((ide_vers >= 8000)
				|| (gbk_cpufam_x64 == gbo_cpufam))
			{
				WriteCStrToDestFile(" /MTd");
			} else {
				WriteCStrToDestFile(" /MLd");
			}
		} else {
			if ((ide_vers >= 8000)
				|| (gbk_cpufam_x64 == gbo_cpufam))
			{
				WriteCStrToDestFile(" /MT");
			} else {
				WriteCStrToDestFile(" /ML");
			}
		}
	}

#if 0
	if (ide_vers < 8000) {
		WriteCStrToDestFile(" /GS");
			/* became default later */
		/*
			perhaps instead use /GS- in later versions
			maybe this should be an option set in SPBASDEF
		*/
	}
#endif

	WriteCStrToDestFile(" /W4");

	WriteCStrToDestFile(" /c");

	if ((ide_vers >= 7000) && (ide_vers < 9000)) {
		/* Detect 64-bit Portability Issues */
		WriteCStrToDestFile(" /Wp64");
	}

	if (ide_vers >= 7000) {
		/* and probably earlier, at least back to 6.0 */
		/* Enable Function-Level Linking */
		WriteCStrToDestFile(" /Gy");
	}

	if (gbk_dbg_on == gbo_dbg) {
		/* Debug Information Format */
		if (WinCE || (gbk_targ_wx64 == cur_targ)) {
			WriteCStrToDestFile(" /Zi");
		} else {
			WriteCStrToDestFile(" /ZI");
		}
	}
}

LOCALPROC WriteRCexeFlags(void)
{
	blnr WinCE = (gbk_targfam_wnce == gbo_targfam);

	WriteCStrToDestFile("/l 0x409 /d ");
	WriteQuoteToDestFile();
	if (gbk_dbg_on == gbo_dbg) {
		WriteCStrToDestFile("_DEBUG");
	} else {
		WriteCStrToDestFile("NDEBUG");
	}
	WriteQuoteToDestFile();
	if (WinCE) {
		WriteCStrToDestFile(
			" /d UNDER_CE=420 /d _WIN32_WCE=420 /d \"UNICODE\""
			" /d \"_UNICODE\" /d WIN32_PLATFORM_PSPC=400 /r");
		if (gbk_cpufam_arm == gbo_cpufam) {
			WriteCStrToDestFile(
				" /d \"ARM\" /d \"_ARM_\" /d \"ARMV4\"");
		} else {
			WriteCStrToDestFile(
				" /d \"_i386_\" /d \"_X86_\" /d \"x86\"");
		}
	}
}

LOCALPROC WriteFileToMSVCSource(MyProc p)
{
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("SOURCE=");
	p();
	WriteEndDestFileLn();

	WriteDestFileLn("# End Source File");
}

LOCALPROC WriteDocTypeMSVCresource(void)
{
	WriteFileToMSVCSource(WriteDocTypeIconFilePath);
}

LOCALPROC WriteMSVCBeginGroup(char *group, char *filter)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Begin Group ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(group);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# PROP Default_Filter ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(filter);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteMSVCEndGroup(void)
{
	WriteDestFileLn("# End Group");
}

LOCALPROC WriteMSVCdbgLevelName(void)
{
	char *s;

	switch (gbo_dbg) {
		case gbk_dbg_on:
			s = "Debug";
			break;
		case gbk_dbg_test:
			s = "Test";
			break;
		case gbk_dbg_off:
			s = "Release";
			break;
		default:
			s = "(unknown Debug Level)";
			break;
	}

	WriteCStrToDestFile(s);
}

LOCALPROC WriteMSVCTargetName00(void)
{
	blnr WinCE = (gbk_targfam_wnce == gbo_targfam);
	WriteCStrToDestFile("Win32");
	if (WinCE) {
		if (gbk_cpufam_arm == gbo_cpufam) {
			WriteCStrToDestFile(" (WCE ARMV4)");
		} else {
			WriteCStrToDestFile(" (WCE emulator)");
		}
	} else {
		WriteCStrToDestFile(" (x86)");
	}
}

LOCALPROC WriteMSVCTargetName0(void)
{
	WriteStrAppAbbrev();
	WriteCStrToDestFile(" - ");
	WriteMSVCTargetName00();
	WriteSpaceToDestFile();
	WriteMSVCdbgLevelName();
}

LOCALPROC WriteMSVCTargetName(void)
{
	WriteQuoteToDestFile();
	WriteMSVCTargetName0();
	WriteQuoteToDestFile();
}

LOCALPROC WriteMSVCMakefileName(void)
{
	WriteQuoteToDestFile();
	WriteStrAppAbbrev();
	if (gbk_targfam_wnce == gbo_targfam) {
		WriteCStrToDestFile(".vcn");
	} else {
		WriteCStrToDestFile(".mak");
	}
	WriteQuoteToDestFile();
}

LOCALPROC WriteMSVCQuotedProp(char *p, char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("# PROP ");
	WriteCStrToDestFile(p);
	WriteSpaceToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC DoSrcFileMSVCAddFile(void)
{
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("SOURCE=");
	WriteSrcFileFilePath();
	WriteEndDestFileLn();
#if 0
	if (gbk_dbg_on != gbo_dbg) {
		if (fast) {
			WriteDestFileLn("# ADD CPP /O2 /Ob2");
		}
	}
#endif
	WriteDestFileLn("# End Source File");
}

LOCALPROC DoSrcFileMSVCAddHeader(void)
{
	if ((DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader) == 0) {
		WriteDestFileLn("# Begin Source File");
		WriteBlankLineToDestFile();
		WriteBgnDestFileLn();
		WriteCStrToDestFile("SOURCE=");
		WriteSrcFileHeaderPath();
		WriteEndDestFileLn();
		WriteDestFileLn("# End Source File");
	}
}

LOCALPROC DoExtraHeaderMSVCAdd(void)
{
	WriteDestFileLn("# Begin Source File");
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("SOURCE=");
	WriteExtraHeaderFilePath();
	WriteEndDestFileLn();
	WriteDestFileLn("# End Source File");
}

LOCALFUNC char * MSVCWorkspaceExt(void)
{
	return (gbk_targfam_wnce == gbo_targfam)
		? ".vcw" : ".dsw";
}

LOCALFUNC char * MSVCProjectExt(void)
{
	return (gbk_targfam_wnce == gbo_targfam)
		? ".vcp" : ".dsp";
}

LOCALPROC WriteMSVCWorkSpaceFile(void)
{
	blnr WinCE = (gbk_targfam_wnce == gbo_targfam);
	if (WinCE) {
		WriteDestFileLn(
			"Microsoft eMbedded Visual Tools Workspace File,"
			" Format Version 4.00");
	} else {
		WriteDestFileLn(
			"Microsoft Developer Studio Workspace File,"
			" Format Version 6.00");
	}
	WriteDestFileLn(
		"# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!");
	WriteBlankLineToDestFile();
	WriteDestFileLn(
		"############################################################"
		"###################");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Project: ");
	WriteQuoteToDestFile();
	WriteStrAppAbbrev();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("=.");
	WriteBackSlashToDestFile();
	WriteStrAppAbbrev();
	WriteCStrToDestFile(MSVCProjectExt());
	WriteCStrToDestFile(" - Package Owner=<4>");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<5>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<4>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn(
		"############################################################"
		"###################");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Global:");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<5>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn("Package=<3>");
	WriteDestFileLn("{{{");
	WriteDestFileLn("}}}");
	WriteBlankLineToDestFile();
	WriteDestFileLn(
		"############################################################"
		"###################");
	WriteBlankLineToDestFile();
}

LOCALPROC WriteMSVCProjectFile(void)
{
	blnr WinCE = (gbk_targfam_wnce == gbo_targfam);

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Microsoft ");
	if (WinCE) {
		WriteCStrToDestFile("eMbedded Visual Tools");
	} else {
		WriteCStrToDestFile("Developer Studio");
	}
	WriteCStrToDestFile(" Project File - Name=");
	WriteQuoteToDestFile();
	WriteStrAppAbbrev();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" - Package Owner=<4>");
	WriteEndDestFileLn();

	if (WinCE) {
		WriteDestFileLn(
			"# Microsoft eMbedded Visual Tools Generated Build File,"
			" Format Version 6.02");
	} else {
		WriteDestFileLn(
			"# Microsoft Developer Studio Generated Build File,"
			" Format Version 6.00");
	}
	WriteDestFileLn("# ** DO NOT EDIT **");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# TARGTYPE ");
	WriteQuoteToDestFile();
	WriteMSVCTargetName00();
	WriteCStrToDestFile(" Application");
	WriteQuoteToDestFile();
	if (WinCE) {
		if (gbk_cpufam_arm == gbo_cpufam) {
			WriteCStrToDestFile(" 0xa301");
		} else {
			WriteCStrToDestFile(" 0xa601");
		}
	} else {
		WriteCStrToDestFile(" 0x0101");
	}
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("CFG=");
	/* WriteAppVariationStr(); */
	WriteMSVCTargetName0();
	WriteEndDestFileLn();

	WriteDestFileLn(
		"!MESSAGE This is not a valid makefile."
		" To build this project using NMAKE,");
	WriteDestFileLn("!MESSAGE use the Export Makefile command and run");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToDestFile(".");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn(
		"!MESSAGE You can specify a configuration when running NMAKE");
	WriteDestFileLn(
		"!MESSAGE by defining the macro CFG on the command line."
		" For example:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE NMAKE /f ");
	WriteMSVCMakefileName();
	WriteCStrToDestFile(" CFG=");
	WriteMSVCTargetName();
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteDestFileLn("!MESSAGE Possible choices for configuration are:");
	WriteDestFileLn("!MESSAGE ");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("!MESSAGE ");
	WriteMSVCTargetName();
	WriteCStrToDestFile(" (based on ");
	WriteQuoteToDestFile();
	WriteMSVCTargetName00();
	WriteCStrToDestFile(" Application");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(")");
	WriteEndDestFileLn();

	WriteDestFileLn("!MESSAGE ");
	WriteBlankLineToDestFile();
	WriteDestFileLn("# Begin Project");
	WriteDestFileLn("# PROP AllowPerConfigDependencies 0");

	WriteMSVCQuotedProp("Scc_ProjName", "");
	WriteMSVCQuotedProp("Scc_LocalPath", "");

	if (WinCE) {
		WriteDestFileLn("# PROP ATL_Project 2"); /* not needed ? */
	}

	WriteBgnDestFileLn();
	WriteCStrToDestFile("CPP=");
	WriteCompileCExec();
	WriteEndDestFileLn();

	WriteDestFileLn("MTL=midl.exe");
	WriteDestFileLn("RSC=rc.exe");
	WriteDestFileLn("# PROP BASE Use_MFC 0");
	WriteDestFileLn("# PROP BASE Use_Debug_Libraries 1");

	WriteMSVCQuotedProp("BASE Output_Dir", "Debug");
	WriteMSVCQuotedProp("BASE Intermediate_Dir", "Debug");
	WriteMSVCQuotedProp("BASE Target_Dir", "");

	WriteDestFileLn("# PROP Use_MFC 0");
	if (gbk_dbg_on == gbo_dbg) {
		WriteDestFileLn("# PROP Use_Debug_Libraries 1");
	} else {
		WriteDestFileLn("# PROP Use_Debug_Libraries 0");
	}

	WriteMSVCQuotedProp("Output_Dir", obj_d_name);
	WriteMSVCQuotedProp("Intermediate_Dir", obj_d_name);

	WriteDestFileLn("# PROP Ignore_Export_Lib 0");

	WriteMSVCQuotedProp("Target_Dir", "");

	WriteDestFileLn("# ADD BASE CPP");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD CPP ");
	WriteCLexeFlags();
	if (! WinCE) {
		WriteCStrToDestFile(" /FD");
	}
	WriteCStrToDestFile(" /I ");
	WriteQuoteToDestFile();
	Write_cfg_d_ToDestFile();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteDestFileLn("# ADD BASE MTL");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD MTL /nologo /D ");
	WriteQuoteToDestFile();
	if (gbk_dbg_on == gbo_dbg) {
		WriteCStrToDestFile("_DEBUG");
	} else {
		WriteCStrToDestFile("NDEBUG");
	}
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" /mktyplib203 /win32");
	if (WinCE) {
		WriteCStrToDestFile(" /o \"NUL\"");
	}
	WriteEndDestFileLn();

	WriteDestFileLn("# ADD BASE RSC");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD RSC ");
	WriteRCexeFlags();
	WriteCStrToDestFile(" /I ");
	WriteQuoteToDestFile();
	Write_src_d_ToDestFile();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();

	WriteDestFileLn("BSC32=bscmake.exe");
	WriteDestFileLn("# ADD BASE BSC32");
	WriteDestFileLn("# ADD BSC32 /nologo");
	WriteDestFileLn("LINK32=link.exe");
	WriteDestFileLn("# ADD BASE LINK32");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# ADD LINK32");
	if (WinCE) {
		WriteCStrToDestFile(
			" commctrl.lib coredll.lib aygshell.lib Mmtimer.lib");
	} else {
		WriteCStrToDestFile(
			" kernel32.lib user32.lib gdi32.lib"
			" winspool.lib comdlg32.lib advapi32.lib shell32.lib"
			" ole32.lib oleaut32.lib uuid.lib winmm.lib");
	}
	WriteCStrToDestFile(" /nologo");
	if (WinCE) {
		WriteCStrToDestFile(
			" /base:\"0x00010000\" /stack:0x10000,0x1000"
			" /entry:\"WinMainCRTStartup\""
			" /nodefaultlib:\"$(CENoDefaultLib)\"");
	}

	WriteCStrToDestFile(" /subsystem:");
	if (WinCE) {
		WriteCStrToDestFile("$(CESubsystem)");
	} else {
		WriteCStrToDestFile("windows");
	}

	if (gbk_dbg_on == gbo_dbg) {
		WriteCStrToDestFile(" /debug");
	} else {
		WriteCStrToDestFile(" /incremental:no");
	}
	WriteCStrToDestFile(" /machine:");
	if (gbk_cpufam_arm == gbo_cpufam) {
		WriteCStrToDestFile("ARM");
	} else {
		if (WinCE) {
			WriteCStrToDestFile("IX86");
		} else {
			WriteCStrToDestFile("I386"); /* maybe should be IX86 ? */
		}
	}

	WriteCStrToDestFile(" /out:");
	WriteQuoteToDestFile();
	WriteAppNameStr();
	WriteQuoteToDestFile();
	if (WinCE) {
		if (gbk_cpufam_arm == gbo_cpufam) {
			WriteCStrToDestFile(" /align:\"4096\"");
				/* is this really needed ? */
		} else {
			WriteCStrToDestFile(
				" $(CEx86Corelibc) /nodefaultlib:\"OLDNAMES.lib\"");
		}
	}
	if (gbk_dbg_on == gbo_dbg) {
		WriteCStrToDestFile(" /pdbtype:sept");
	}
	WriteEndDestFileLn();

	WriteDestFileLn("# Begin Target");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("# Name ");
	WriteMSVCTargetName();
	WriteEndDestFileLn();

	WriteMSVCBeginGroup("Source Files",
		"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat");
		DoAllSrcFilesWithSetup(
			DoSrcFileMSVCAddFile);
		WriteFileToMSVCSource(WriteMainRsrcSrcPath);
	WriteMSVCEndGroup();

	WriteMSVCBeginGroup("Header Files", "h;hpp;hxx;hm;inl");
		DoAllSrcFilesWithSetup(
			DoSrcFileMSVCAddHeader);
	WriteMSVCEndGroup();

	WriteMSVCBeginGroup("Resource Files",
		"ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe");
		DoAllDocTypesWithSetup(WriteDocTypeMSVCresource);
	WriteMSVCEndGroup();

	WriteMSVCBeginGroup("Include Files", "h;hpp;hxx;hm;inl");
		DoAllExtraHeaders2WithSetup(DoExtraHeaderMSVCAdd);
	WriteMSVCEndGroup();

	WriteDestFileLn("# End Target");
	WriteDestFileLn("# End Project");
}

LOCALPROC WriteMSVCSpecificFiles(void)
{
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, MSVCWorkspaceExt(), "Workspace file",
		WriteMSVCWorkSpaceFile);
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, MSVCProjectExt(), "Project file",
		WriteMSVCProjectFile);
}

LOCALPROC WriteXMLQuotedProp(char *s, MyProc p)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile("=");
	WriteQuoteToDestFile();
	p();
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteMSVCPlatformName(void)
{
	if (gbk_targ_wx64 == cur_targ) {
		WriteCStrToDestFile("x64");
	} else {
		WriteCStrToDestFile("Win32");
	}
}

LOCALPROC WriteMSVCXMLPlatformProps(void)
{
	WriteXMLQuotedProp("Name", WriteMSVCPlatformName);
}

LOCALPROC WriteMSVCXMLPlatforms(void)
{
	WriteXMLtaglinewithprops("Platform", WriteMSVCXMLPlatformProps);
}

LOCALPROC WriteMSVCXMLConfigurationName(void)
{
	WriteMSVCdbgLevelName();
	WriteCStrToDestFile("|");
	WriteMSVCPlatformName();
}

LOCALPROC WriteMSVCXMLConfigurationProps(void)
{
	WriteXMLQuotedProp("Name", WriteMSVCXMLConfigurationName);
	WriteXMLQuotedProp("OutputDirectory", Write_obj_d_ToDestFile);
	WriteXMLQuotedProp("IntermediateDirectory", Write_obj_d_ToDestFile);
	WriteDestFileLn("ConfigurationType=\"1\"");
	WriteDestFileLn("CharacterSet=\"2\"");
	if (gbk_dbg_on != gbo_dbg) {
		WriteDestFileLn("WholeProgramOptimization=\"0\"");
	}
}

LOCALPROC WriteMSVCToolConfig(char *s, MyProc p)
{
	WriteDestFileLn("<Tool");
	++DestFileIndent;
		WriteBgnDestFileLn();
		WriteCStrToDestFile("Name=");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(s);
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
		if (NULL != p) {
			p();
		}
	--DestFileIndent;
	WriteDestFileLn("/>");
}

LOCALPROC WriteMSVCCompilerToolConfig(void)
{
#if 0
	if (gbk_dbg_on != gbo_dbg) {
		WriteDestFileLn("Optimization=\"1\"");
		WriteDestFileLn("FavorSizeOrSpeed=\"0\"");
		WriteDestFileLn("WholeProgramOptimization=\"false\"");
		WriteDestFileLn("OmitFramePointers=\"true\"");
	} else
#endif
	{
		/* Maximize chance of correct compile */
		WriteDestFileLn("Optimization=\"0\"");
	}

	if (gbk_dbg_on == gbo_dbg) {
		WriteDestFileLn(
			"PreprocessorDefinitions=\"WIN32;_DEBUG;_WINDOWS\"");
		WriteDestFileLn("MinimalRebuild=\"true\"");
	} else {
		WriteDestFileLn(
			"PreprocessorDefinitions=\"WIN32;NDEBUG;_WINDOWS\"");
		WriteDestFileLn("StringPooling=\"true\"");
	}

	WriteXMLQuotedProp("AdditionalIncludeDirectories",
		Write_cfg_d_ToDestFile);

	WriteDestFileLn("ExceptionHandling=\"0\"");
	if (gbk_dbg_on == gbo_dbg) {
		WriteDestFileLn("BasicRuntimeChecks=\"3\"");
		if (ide_vers >= 8000) {
			WriteDestFileLn("RuntimeLibrary=\"1\"");
		} else {
			WriteDestFileLn("RuntimeLibrary=\"5\"");
		}
	} else {
		if (ide_vers >= 8000) {
			WriteDestFileLn("RuntimeLibrary=\"0\"");
		} else {
			WriteDestFileLn("RuntimeLibrary=\"4\"");
		}
	}
	if (ide_vers < 8000) {
		WriteDestFileLn("BufferSecurityCheck=\"false\"");
			/* perhaps later versions also */
			/* maybe this should be an option set in SPBASDEF */
	}
	WriteDestFileLn("EnableFunctionLevelLinking=\"true\"");
	WriteDestFileLn("UsePrecompiledHeader=\"0\"");
	WriteDestFileLn("WarningLevel=\"4\"");
	if (ide_vers < 9000) {
		WriteDestFileLn("Detect64BitPortabilityProblems=\"true\"");
	}
	if (gbk_dbg_on == gbo_dbg) {
		WriteDestFileLn("DebugInformationFormat=\"4\"");
	} else {
		WriteDestFileLn("DebugInformationFormat=\"0\"");
	}
	WriteDestFileLn("CompileAs=\"0\"");
}

LOCALPROC WriteMSVCResourceCompilerToolConfig(void)
{
	WriteXMLQuotedProp("AdditionalIncludeDirectories",
		Write_src_d_ToDestFile);
}

LOCALPROC WriteMSVCLinkerToolConfig(void)
{
	WriteDestFileLn("AdditionalDependencies=\"winmm.lib\"");
	WriteXMLQuotedProp("OutputFile", WriteAppNamePath);
	if (gbk_dbg_on == gbo_dbg) {
		WriteDestFileLn("LinkIncremental=\"2\"");
	} else {
		WriteDestFileLn("LinkIncremental=\"1\"");
	}
	if (ide_vers >= 8000) {
		WriteDestFileLn("GenerateManifest=\"false\"");
	}
	if (gbk_dbg_on == gbo_dbg) {
		WriteDestFileLn("GenerateDebugInformation=\"true\"");
	} else {
		WriteDestFileLn("GenerateDebugInformation=\"false\"");
	}
	WriteDestFileLn("SubSystem=\"2\"");
	if (gbk_dbg_on != gbo_dbg) {
		WriteDestFileLn("OptimizeReferences=\"2\"");
		WriteDestFileLn("EnableCOMDATFolding=\"2\"");
	}
	if (ide_vers >= 9000) {
		WriteDestFileLn("RandomizedBaseAddress=\"1\"");
		WriteDestFileLn("DataExecutionPrevention=\"0\"");
	}
	if (gbk_targ_wx64 == cur_targ) {
		WriteDestFileLn("TargetMachine=\"17\"");
	} else {
		WriteDestFileLn("TargetMachine=\"1\"");
	}
}

LOCALPROC WriteMSVCXMLConfigurationBody(void)
{
	WriteMSVCToolConfig("VCPreBuildEventTool", NULL);
	WriteMSVCToolConfig("VCCustomBuildTool", NULL);
	if (ide_vers >= 7100) {
		WriteMSVCToolConfig("VCXMLDataGeneratorTool", NULL);
	}
	WriteMSVCToolConfig("VCWebServiceProxyGeneratorTool", NULL);
	WriteMSVCToolConfig("VCMIDLTool", NULL);
	WriteMSVCToolConfig("VCCLCompilerTool",
		WriteMSVCCompilerToolConfig);
	if (ide_vers >= 8000) {
		WriteMSVCToolConfig("VCManagedResourceCompilerTool", NULL);
	}
	WriteMSVCToolConfig("VCResourceCompilerTool",
		WriteMSVCResourceCompilerToolConfig);
	WriteMSVCToolConfig("VCPreLinkEventTool", NULL);
	WriteMSVCToolConfig("VCLinkerTool", WriteMSVCLinkerToolConfig);
	if (ide_vers >= 8000) {
		WriteMSVCToolConfig("VCALinkTool", NULL);
		WriteMSVCToolConfig("VCManifestTool", NULL);
		WriteMSVCToolConfig("VCXDCMakeTool", NULL);
		WriteMSVCToolConfig("VCBscMakeTool", NULL);
		WriteMSVCToolConfig("VCFxCopTool", NULL);
		WriteMSVCToolConfig("VCAppVerifierTool", NULL);
	} else if (ide_vers >= 7100) {
		WriteMSVCToolConfig("VCManagedWrapperGeneratorTool", NULL);
		WriteMSVCToolConfig("VCAuxiliaryManagedWrapperGeneratorTool",
			NULL);
	}
	if (ide_vers < 9000) {
		WriteMSVCToolConfig("VCWebDeploymentTool", NULL);
	}
	WriteMSVCToolConfig("VCPostBuildEventTool", NULL);
}

LOCALPROC WriteMSVCXMLConfigurations(void)
{
	WriteXMLtaggedLinesWithProps("Configuration",
		WriteMSVCXMLConfigurationProps,
		WriteMSVCXMLConfigurationBody);
}

LOCALPROC WriteMSVCXMLSourceFilesProps(void)
{
	WriteDestFileLn("Name=\"Source Files\"");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Filter=\"cpp;c");
	if (ide_vers >= 8000) {
		WriteCStrToDestFile(";cc");
	}
	WriteCStrToDestFile(";cxx;def;odl;idl;hpj;bat;asm");
	if (ide_vers >= 7100) {
		WriteCStrToDestFile(";asmx");
	}
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
	if (ide_vers >= 7100) {
		WriteDestFileLn(
			"UniqueIdentifier=\"{"
			"00020000-0000-0000-0000-000000000000}\"");
	}
}

LOCALPROC DoMSVCXMLAddFile(MyProc p)
{
	WriteDestFileLn("<File");
	++DestFileIndent;
		WriteXMLQuotedProp("RelativePath", p);
		WriteDestFileLn(">");
	--DestFileIndent;
	WriteDestFileLn("</File>");
}

LOCALPROC DoSrcFileMSVCXMLAddFile(void)
{
	DoMSVCXMLAddFile(WriteSrcFileFilePath);
}

LOCALPROC WriteMSVCXMLSourceFilesBody(void)
{
	DoAllSrcFilesWithSetup(DoSrcFileMSVCXMLAddFile);
}

LOCALPROC WriteMSVCXMLHeaderFilesProps(void)
{
	WriteDestFileLn("Name=\"Header Files\"");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Filter=\"h;hpp;hxx;hm;inl;inc");
	if (ide_vers >= 7100) {
		WriteCStrToDestFile(";xsd");
	}
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
	if (ide_vers >= 7100) {
		WriteDestFileLn(
			"UniqueIdentifier=\"{"
			"00030000-0000-0000-0000-000000000000}\"");
	}
}

LOCALPROC DoSrcFileMSVCXMLAddHeader(void)
{
	if ((DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader) == 0) {
		DoMSVCXMLAddFile(WriteSrcFileHeaderPath);
	}
}

LOCALPROC WriteMSVCXMLHeaderFilesBody(void)
{
	DoAllSrcFilesWithSetup(DoSrcFileMSVCXMLAddHeader);
}

LOCALPROC WriteMSVCXMLResourceFilesProps(void)
{
	WriteDestFileLn("Name=\"Resource Files\"");
	WriteBgnDestFileLn();
	WriteCStrToDestFile(
		"Filter=\"rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;"
		"jpeg;jpe");
	if (ide_vers >= 7100) {
		WriteCStrToDestFile(";resx");
	}
	if (ide_vers >= 8000) {
		WriteCStrToDestFile(";tiff;tif;png;wav");
	}
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
	if (ide_vers >= 7100) {
		WriteDestFileLn(
			"UniqueIdentifier=\"{"
			"00040000-0000-0000-0000-000000000000}\"");
	}
}

LOCALPROC WriteDocTypeMSVCXMLresource(void)
{
	DoMSVCXMLAddFile(WriteDocTypeIconFilePath);
}

LOCALPROC WriteMSVCXMLResourceFilesBody(void)
{
	DoMSVCXMLAddFile(WriteMainRsrcSrcPath);
	DoAllDocTypesWithSetup(WriteDocTypeMSVCXMLresource);
}

LOCALPROC WriteMSVCXMLIncludeFilesProps(void)
{
	WriteDestFileLn("Name=\"Include Files\"");
	WriteDestFileLn("Filter=\"h;hpp;hxx;hm;inl;inc;xsd\"");
}

LOCALPROC DoMSVCXMLAddAddExtraHeader(void)
{
	DoMSVCXMLAddFile(WriteExtraHeaderFilePath);
}

LOCALPROC WriteMSVCXMLIncludeFilesBody(void)
{
	DoAllExtraHeaders2WithSetup(DoMSVCXMLAddAddExtraHeader);
}

LOCALPROC WriteMSVCXMLFiles(void)
{
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLSourceFilesProps,
		WriteMSVCXMLSourceFilesBody);
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLHeaderFilesProps,
		WriteMSVCXMLHeaderFilesBody);
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLResourceFilesProps,
		WriteMSVCXMLResourceFilesBody);
	WriteXMLtaggedLinesWithProps("Filter",
		WriteMSVCXMLIncludeFilesProps,
		WriteMSVCXMLIncludeFilesBody);
}

LOCALPROC WriteMSVCXMLSolutionFile(void)
{
	if (ide_vers >= 8000) {
		WriteDestFileLn("\357\273\277"); /* UTF-8 byte-order mark */
	}

	if (ide_vers >= 11000) {
		WriteDestFileLn(
			"Microsoft Visual Studio Solution File,"
			" Format Version 12.00");
		if (ide_vers >= 15000) {
			WriteDestFileLn("# Visual Studio 15");
		} else if (ide_vers >= 14000) {
			WriteDestFileLn("# Visual Studio 14");
		} else if (ide_vers >= 12000) {
			WriteDestFileLn("# Visual Studio 2013");
		} else {
			WriteDestFileLn("# Visual Studio 2012");
		}
	} else if (ide_vers >= 10000) {
		WriteDestFileLn(
			"Microsoft Visual Studio Solution File,"
			" Format Version 11.00");
		WriteDestFileLn("# Visual Studio 2010");
	} else if (ide_vers >= 9000) {
		WriteDestFileLn(
			"Microsoft Visual Studio Solution File,"
			" Format Version 10.00");
		/* WriteDestFileLn("# Visual C++ Express 2008"); */
		WriteDestFileLn("# Visual Studio 2008");
	} else if (ide_vers >= 8000) {
		WriteDestFileLn(
			"Microsoft Visual Studio Solution File,"
			" Format Version 9.00");
		/* WriteDestFileLn("# Visual C++ Express 2005"); */
		WriteDestFileLn("# Visual Studio 2005");
	} else if (ide_vers >= 7100) {
		WriteDestFileLn(
			"Microsoft Visual Studio Solution File,"
			" Format Version 8.00");
	} else {
		WriteDestFileLn(
			"Microsoft Visual Studio Solution File,"
			" Format Version 7.00");
	}
	WriteBgnDestFileLn();
	WriteCStrToDestFile(
		"Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"");
	WriteStrAppAbbrev();
	WriteCStrToDestFile("\", \"");
	WriteStrAppAbbrev();
	if (ide_vers >= 10000) {
		WriteCStrToDestFile(".vcxproj");
	} else {
		WriteCStrToDestFile(".vcproj");
	}
	WriteCStrToDestFile(
		"\", \"{00010000-0000-0000-0000-000000000000}\"");
	WriteEndDestFileLn();
	if ((ide_vers >= 7100) && (ide_vers < 8000)) {
		++DestFileIndent;
			WriteDestFileLn(
				"ProjectSection(ProjectDependencies)"" = postProject");
			WriteDestFileLn("EndProjectSection");
		--DestFileIndent;
	}
	WriteDestFileLn("EndProject");
	WriteDestFileLn("Global");
	++DestFileIndent;
		if (ide_vers >= 8000) {
			WriteDestFileLn(
				"GlobalSection(SolutionConfigurationPlatforms)"
				" = preSolution");
			++DestFileIndent;
				WriteBgnDestFileLn();
				WriteMSVCXMLConfigurationName();
				WriteCStrToDestFile(" = ");
				WriteMSVCXMLConfigurationName();
				WriteEndDestFileLn();
			--DestFileIndent;
			WriteDestFileLn("EndGlobalSection");
			WriteDestFileLn(
				"GlobalSection(ProjectConfigurationPlatforms)"
				" = postSolution");
			++DestFileIndent;
				WriteBgnDestFileLn();
				WriteCStrToDestFile(
					"{00010000-0000-0000-0000-000000000000}.");
				WriteMSVCXMLConfigurationName();
				WriteCStrToDestFile(".ActiveCfg = ");
				WriteMSVCXMLConfigurationName();
				WriteEndDestFileLn();
				WriteBgnDestFileLn();
				WriteCStrToDestFile(
					"{00010000-0000-0000-0000-000000000000}.");
				WriteMSVCXMLConfigurationName();
				WriteCStrToDestFile(".Build.0 = ");
				WriteMSVCXMLConfigurationName();
				WriteEndDestFileLn();
			--DestFileIndent;
			WriteDestFileLn("EndGlobalSection");
			WriteDestFileLn(
				"GlobalSection(SolutionProperties) = preSolution");
			++DestFileIndent;
				WriteDestFileLn("HideSolutionNode = FALSE");
			--DestFileIndent;
			WriteDestFileLn("EndGlobalSection");
		} else {
			WriteDestFileLn(
				"GlobalSection(SolutionConfiguration) = preSolution");
			++DestFileIndent;
				WriteBgnDestFileLn();
				if (ide_vers >= 7100) {
					WriteMSVCdbgLevelName();
				} else {
					WriteCStrToDestFile("ConfigName.0");
				}
				WriteCStrToDestFile(" = ");
				WriteMSVCdbgLevelName();
				WriteEndDestFileLn();
			--DestFileIndent;
			WriteDestFileLn("EndGlobalSection");
			if (ide_vers < 7100) {
				WriteCStrToDestFile(
					"GlobalSection(ProjectDependencies)"
					" = postSolution");
				WriteCStrToDestFile("EndGlobalSection");
			}
			WriteDestFileLn(
				"GlobalSection(ProjectConfiguration) = postSolution");
			++DestFileIndent;
				WriteBgnDestFileLn();
				WriteCStrToDestFile(
					"{00010000-0000-0000-0000-000000000000}.");
				WriteMSVCdbgLevelName();
				WriteCStrToDestFile(".ActiveCfg = ");
				WriteMSVCXMLConfigurationName();
				WriteEndDestFileLn();

				WriteBgnDestFileLn();
				WriteCStrToDestFile(
					"{00010000-0000-0000-0000-000000000000}.");
				WriteMSVCdbgLevelName();
				WriteCStrToDestFile(".Build.0 = ");
				WriteMSVCXMLConfigurationName();
				WriteEndDestFileLn();
			--DestFileIndent;
			WriteDestFileLn("EndGlobalSection");
			WriteDestFileLn(
				"GlobalSection(ExtensibilityGlobals) = postSolution");
			WriteDestFileLn("EndGlobalSection");
			WriteDestFileLn(
				"GlobalSection(ExtensibilityAddIns) = postSolution");
			WriteDestFileLn("EndGlobalSection");
		}
	--DestFileIndent;
	WriteDestFileLn("EndGlobal");
}

LOCALPROC WriteMSVCXMLProjectProps(void)
{
	WriteDestFileLn("ProjectType=\"Visual C++\"");
	if (ide_vers >= 9000) {
		WriteDestFileLn("Version=\"9.00\"");
	} else if (ide_vers >= 8000) {
		WriteDestFileLn("Version=\"8.00\"");
	} else if (ide_vers >= 7100) {
		WriteDestFileLn("Version=\"7.10\"");
	} else {
		WriteDestFileLn("Version=\"7.00\"");
	}
	WriteXMLQuotedProp("Name", WriteStrAppAbbrev);
	WriteDestFileLn(
		"ProjectGUID=\"{00010000-0000-0000-0000-000000000000}\"");
	if (ide_vers >= 8000) {
		WriteXMLQuotedProp("RootNamespace", WriteStrAppAbbrev);
	}
	WriteDestFileLn("Keyword=\"Win32Proj\"");
	if (ide_vers >= 9000) {
		WriteDestFileLn("TargetFrameworkVersion=\"131072\"");
	}
}

LOCALPROC WriteMSVCXMLProjectBody(void)
{
	WriteXMLtaggedLines("Platforms", WriteMSVCXMLPlatforms);
	if (ide_vers >= 8000) {
		WriteXMLtaggedLines("ToolFiles", NULL);
	}
	WriteXMLtaggedLines("Configurations", WriteMSVCXMLConfigurations);
	if (ide_vers >= 7100) {
		WriteXMLtaggedLines("References", NULL);
	}
	WriteXMLtaggedLines("Files", WriteMSVCXMLFiles);
	WriteXMLtaggedLines("Globals", NULL);
}

LOCALPROC WriteMSVCXMLProjectFile(void)
{
	WriteDestFileLn(
		"<?xml version=\"1.0\" encoding=\"Windows-1252\"?>");
	WriteXMLtaggedLinesWithProps("VisualStudioProject",
		WriteMSVCXMLProjectProps,
		WriteMSVCXMLProjectBody);
}

LOCALPROC WriteMSVCXMLSpecificFiles(void)
{
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, ".sln", "Solutions file",
		WriteMSVCXMLSolutionFile);
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, ".vcproj", "Project file",
		WriteMSVCXMLProjectFile);
}

LOCALPROC DoSrcFileNMakeAddObjFile(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("\"");
	WriteSrcFileObjPath();
	WriteCStrToDestFile("\" \\");
	WriteEndDestFileLn();
}

LOCALPROC WriteMainRsrcObjMSCbuild(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("rc.exe ");
	WriteRCexeFlags();
	WriteCStrToDestFile(" /fo\"");
	WriteMainRsrcObjPath();
	WriteCStrToDestFile("\"");
	WriteCStrToDestFile(" /i");
	WriteQuoteToDestFile();
	Write_src_d_ToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" \"");
	WriteMainRsrcSrcPath();
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
}

LOCALPROC WriteNMakeMakeFile(void)
{
	blnr WinCE = (gbk_targfam_wnce == gbo_targfam);

	WriteDestFileLn("# make file generated by gryphel build system");
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptions=");
	WriteCLexeFlags();
	WriteCStrToDestFile(" /I\"");
	Write_cfg_d_ToDestFile();
	WriteCStrToDestFile("\\\\\"");
		/* yes, a double backslash is what is needed */
	WriteCStrToDestFile(" /Fo\"");
	Write_obj_d_ToDestFile();
	WriteCStrToDestFile("\\\\\"");
	WriteCStrToDestFile(" /Fd\"");
	Write_obj_d_ToDestFile();
	WriteCStrToDestFile("\\\\\"");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("TheDefaultOutput :");
	WriteMakeDependFile(WriteAppNamePath);
	WriteEndDestFileLn();
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	DoAllSrcFilesWithSetup(DoSrcFileMakeCompile);
	WriteBlankLineToDestFile();
	WriteDestFileLn("ObjFiles= \\");
	++DestFileIndent;
		DoAllSrcFilesSortWithSetup(DoSrcFileNMakeAddObjFile);
	--DestFileIndent;
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteMakeRule(WriteMainRsrcObjPath,
		WriteMainRsrcObjMSCdeps, WriteMainRsrcObjMSCbuild);
	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("\"");
	WriteAppNamePath();
	WriteCStrToDestFile("\" : $(ObjFiles) \"");
	WriteMainRsrcObjPath();
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
	++DestFileIndent;
		WriteDestFileLn("link.exe @<<");
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToDestFile("/out:\"");
			WriteAppNamePath();
			WriteCStrToDestFile("\"");
			WriteEndDestFileLn();
			WriteBgnDestFileLn();
			WriteCStrToDestFile("/nologo");
			if (WinCE) {
				WriteCStrToDestFile(" /subsystem:windowsce,4.20");
			} else {
				WriteCStrToDestFile(" /subsystem:windows");
			}
			if (gbk_dbg_on == gbo_dbg) {
				WriteCStrToDestFile(" /debug");
			} else {
				WriteCStrToDestFile(" /incremental:no");
			}
			WriteCStrToDestFile(" /opt:ref /opt:icf");
				/*
					more or less default, but putting this in
					makes difference at least in visual studio 2005
				*/
			WriteEndDestFileLn();
			WriteBgnDestFileLn();
			WriteCStrToDestFile("/pdb:\"");
			Write_obj_d_ToDestFile();
			WriteCStrToDestFile("\\");
			WriteStrAppAbbrev();
			WriteCStrToDestFile(".pdb\"");
			WriteCStrToDestFile(" /machine:");
			if (gbk_cpufam_arm == gbo_cpufam) {
				WriteCStrToDestFile("ARM");
			} else if (gbk_cpufam_x64 == gbo_cpufam) {
				if (ide_vers >= 7100) {
					WriteCStrToDestFile("X64");
				} else {
					WriteCStrToDestFile("AMD64");
				}
			} else {
				if (ide_vers >= 7100) {
					WriteCStrToDestFile("X86");
				} else {
					WriteCStrToDestFile("I386");
				}
			}
			WriteEndDestFileLn();
			if (WinCE) {
				WriteDestFileLn("/base:\"0x00010000\"");
				WriteDestFileLn("/stack:0x10000,0x1000");
				WriteDestFileLn("/entry:WinMainCRTStartup");
				WriteDestFileLn("/align:4096");
			}
			if (ide_vers >= 9000) {
				WriteDestFileLn("/dynamicbase:no");
					/* smaller exe, maybe not as secure */
			}
			if (WinCE) {
				WriteDestFileLn("commctrl.lib");
				WriteDestFileLn("coredll.lib");
				WriteDestFileLn("aygshell.lib");
				WriteDestFileLn("Mmtimer.lib");
				WriteDestFileLn("/nodefaultlib:libc.lib");
				WriteDestFileLn("/nodefaultlib:libcd.lib");
				WriteDestFileLn("/nodefaultlib:libcmt.lib");
				WriteDestFileLn("/nodefaultlib:libcmtd.lib");
				WriteDestFileLn("/nodefaultlib:msvcrt.lib");
				WriteDestFileLn("/nodefaultlib:msvcrtd.lib");
			} else {
				WriteDestFileLn("winmm.lib");
				WriteDestFileLn("kernel32.lib");
				WriteDestFileLn("user32.lib");
				WriteDestFileLn("gdi32.lib");
				WriteDestFileLn("winspool.lib");
				WriteDestFileLn("comdlg32.lib");
				WriteDestFileLn("advapi32.lib");
				WriteDestFileLn("shell32.lib");
				WriteDestFileLn("ole32.lib");
				WriteDestFileLn("oleaut32.lib");
				WriteDestFileLn("uuid.lib");
				if ((gbk_cpufam_x64 == gbo_cpufam)
					&& (ide_vers >= 7000)
					&& (ide_vers < 7100))
				{
					WriteDestFileLn("bufferoverflowU.lib");
				}
			}
			WriteBgnDestFileLn();
			WriteCStrToDestFile("\"");
			WriteMainRsrcObjPath();
			WriteCStrToDestFile("\"");
			WriteEndDestFileLn();
			WriteDestFileLn("$(ObjFiles)");
		--DestFileIndent;
	--DestFileIndent;
	WriteDestFileLn("<<");
	WriteBlankLineToDestFile();


	WriteBlankLineToDestFile();
	WriteDestFileLn("CLEAN :");
	++DestFileIndent;
		DoAllSrcFilesStandardErase();
		WriteRmFile(WriteMainRsrcObjPath);
		WriteRmFile(WriteAppNamePath);
	--DestFileIndent;
}

LOCALPROC WriteNMakeSpecificFiles(void)
{
	WriteADstFile1("my_project_d",
		"MAKEFILE", "", "Make file",
		WriteNMakeMakeFile);
}

LOCALPROC DoSrcFileMSVC10XMLAddFile(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<ClCompile Include=\"");
	WriteSrcFileFilePath();
	WriteCStrToDestFile("\" />");
	WriteEndDestFileLn();
}

LOCALPROC DoSrcFileMSVC10XMLAddHeaderFile(void)
{
	if ((DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader) == 0) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("<ClInclude Include=\"");
		WriteSrcFileHeaderPath();
		WriteCStrToDestFile("\" />");
		WriteEndDestFileLn();
	}
}

LOCALPROC DoMSVC10XMLAddAddExtraHeader(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<ClInclude Include=\"");
	WriteExtraHeaderFilePath();
	WriteCStrToDestFile("\" />");
	WriteEndDestFileLn();
}

LOCALPROC DoMSVC10XMLAddDocType(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<None Include=\"");
	WriteDocTypeIconFilePath();
	WriteCStrToDestFile("\" />");
	WriteEndDestFileLn();
}

LOCALPROC WriteMSVC10OutDir(void)
{
	WriteCStrToDestFile(".\\");
}

LOCALPROC WriteMSVC10IntDir(void)
{
	Write_obj_d_ToDestFile();
	WriteCStrToDestFile("\\");
}

LOCALPROC WriteMSVCXML10ProjectFile(void)
{
	WriteCStrToDestFile("\357\273\277"); /* UTF-8 byte-order mark */
	WriteDestFileLn("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	WriteBgnDestFileLn();
	WriteCStrToDestFile(
		"<Project DefaultTargets=\"Build\"");
	if (ide_vers >= 15000) {
		WriteCStrToDestFile(
			" ToolsVersion=\"15.0\"");
	} else if (ide_vers >= 14000) {
		WriteCStrToDestFile(
			" ToolsVersion=\"14.0\"");
	} else if (ide_vers >= 12000) {
		WriteCStrToDestFile(
			" ToolsVersion=\"12.0\"");
	} else {
		WriteCStrToDestFile(
			" ToolsVersion=\"4.0\"");
	}
	WriteCStrToDestFile(
		" xmlns="
		"\"http://schemas.microsoft.com/developer/msbuild/2003\">"
		);
	WriteEndDestFileLn();
	++DestFileIndent;
		WriteDestFileLn("<ItemGroup Label=\"ProjectConfigurations\">");
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToDestFile("<ProjectConfiguration Include=\"");
			WriteMSVCXMLConfigurationName();
			WriteCStrToDestFile("\">");
			WriteEndDestFileLn();
			++DestFileIndent;
				WriteXMLtagBeginProcValEndLine("Configuration",
					WriteMSVCdbgLevelName);
				WriteXMLtagBeginProcValEndLine("Platform",
					WriteMSVCPlatformName);
			--DestFileIndent;
			WriteDestFileLn("</ProjectConfiguration>");
		--DestFileIndent;
		WriteDestFileLn("</ItemGroup>");

		WriteDestFileLn("<PropertyGroup Label=\"Globals\">");
		++DestFileIndent;
			WriteXMLtagBeginValEndLine("ProjectGuid",
				"{00010000-0000-0000-0000-000000000000}");
			WriteXMLtagBeginValEndLine("Keyword", "Win32Proj");
			WriteXMLtagBeginProcValEndLine("RootNamespace",
				WriteStrAppAbbrev);
			if (ide_vers >= 15000) {
				WriteXMLtagBeginValEndLine(
					"WindowsTargetPlatformVersion", "10.0.14393.0");
			}
		--DestFileIndent;
		WriteDestFileLn("</PropertyGroup>");

		WriteDestFileLn("<Import Project=\""
			"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />");

		WriteBgnDestFileLn();
		WriteCStrToDestFile("<PropertyGroup Condition=\""
			"'$(Configuration)|$(Platform)'=='");
		WriteMSVCXMLConfigurationName();
		WriteCStrToDestFile("'\" Label=\"Configuration\">");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteXMLtagBeginValEndLine("ConfigurationType",
				"Application");
			if (gbk_dbg_on != gbo_dbg) {
				WriteXMLtagBeginValEndLine("WholeProgramOptimization",
					"false");
			}
			if (ide_vers >= 11000) {
				if (ide_vers >= 14000) {
					WriteXMLtagBeginValEndLine("PlatformToolset",
						"v141");
				} else if (ide_vers >= 14000) {
					WriteXMLtagBeginValEndLine("PlatformToolset",
						"v140");
				} else if (ide_vers >= 12000) {
					WriteXMLtagBeginValEndLine("PlatformToolset",
						"v120");
				} else {
					WriteXMLtagBeginValEndLine("PlatformToolset",
						"v110");
				}
			}
			WriteXMLtagBeginValEndLine("CharacterSet", "MultiByte");
		--DestFileIndent;
		WriteDestFileLn("</PropertyGroup>");

		WriteDestFileLn("<Import Project=\""
			"$(VCTargetsPath)\\Microsoft.Cpp.props\" />");
		WriteDestFileLn("<ImportGroup Label=\"ExtensionSettings\">");
		WriteDestFileLn("</ImportGroup>");

		WriteBgnDestFileLn();
		WriteCStrToDestFile("<ImportGroup");
		WriteCStrToDestFile(" Label=\"PropertySheets\"");
		WriteCStrToDestFile(
			" Condition=\"'$(Configuration)|$(Platform)'=='");
		WriteMSVCXMLConfigurationName();
		WriteCStrToDestFile("'\">");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteDestFileLn("<Import Project=\""
				"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\""
				" Condition=\"exists('$(UserRootDir)\\"
					"Microsoft.Cpp.$(Platform).user.props')\""
				" Label=\"LocalAppDataPlatform\" />");
		--DestFileIndent;
		WriteDestFileLn("</ImportGroup>");

		WriteDestFileLn("<PropertyGroup Label=\"UserMacros\" />");

		WriteBgnDestFileLn();
		WriteCStrToDestFile("<PropertyGroup");
		WriteCStrToDestFile(
			" Condition=\"'$(Configuration)|$(Platform)'=='");
		WriteMSVCXMLConfigurationName();
		WriteCStrToDestFile("'\"");
		WriteCStrToDestFile(">");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteXMLtagBeginProcValEndLine("OutDir", WriteMSVC10OutDir);
			WriteXMLtagBeginProcValEndLine("IntDir", WriteMSVC10IntDir);

			if (gbk_dbg_on == gbo_dbg) {
				WriteXMLtagBeginValEndLine("LinkIncremental", "true");
			} else {
				WriteXMLtagBeginValEndLine("LinkIncremental", "false");
			}

			WriteXMLtagBeginValEndLine("GenerateManifest", "false");
		WriteEndXMLtagLine("PropertyGroup");

		WriteBgnDestFileLn();
		WriteCStrToDestFile("<ItemDefinitionGroup"
			" Condition=\"'$(Configuration)|$(Platform)'=='");
		WriteMSVCXMLConfigurationName();
		WriteCStrToDestFile("'\">");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteBeginXMLtagLine("ClCompile");
				WriteXMLtagBeginValEndLine("WarningLevel", "Level4");
				WriteXMLtagBeginValEndLine("PrecompiledHeader",
					"NotUsing");

#if 0
				if (gbk_dbg_on != gbo_dbg) {
					WriteXMLtagBeginValEndLine("Optimization",
						"MinSpace");
					WriteXMLtagBeginValEndLine("FavorSizeOrSpeed",
						"Neither");
					WriteXMLtagBeginValEndLine(
						"WholeProgramOptimization", "false");
					WriteXMLtagBeginValEndLine("OmitFramePointers",
						"true");
				} else
#endif
				{
					/* Maximize chance of correct compile */
					WriteXMLtagBeginValEndLine("Optimization",
						"Disabled");
				}

				if (gbk_dbg_on == gbo_dbg) {
					WriteDestFileLn("<PreprocessorDefinitions>"
						"WIN32;_DEBUG;_WINDOWS;"
						"%(PreprocessorDefinitions)"
						"</PreprocessorDefinitions>");
					if (gbk_targ_wx64 != cur_targ) {
						WriteXMLtagBeginValEndLine("MinimalRebuild",
							"true");
					}
					WriteXMLtagBeginValEndLine("BasicRuntimeChecks",
						"EnableFastChecks");
					WriteXMLtagBeginValEndLine("RuntimeLibrary",
						"MultiThreadedDebug");
					if (gbk_targ_wx64 == cur_targ) {
						WriteXMLtagBeginValEndLine(
							"DebugInformationFormat",
							"ProgramDatabase");
					} else {
						WriteXMLtagBeginValEndLine(
							"DebugInformationFormat",
							"EditAndContinue");
					}
				} else {
					WriteDestFileLn("<PreprocessorDefinitions>"
						"WIN32;NDEBUG;_WINDOWS;"
						"%(PreprocessorDefinitions)"
						"</PreprocessorDefinitions>");
					WriteXMLtagBeginValEndLine("StringPooling", "true");
					WriteXMLtagBeginValEndLine("RuntimeLibrary",
						"MultiThreaded");
						/*
							MultiThreadedDLL makes smaller exe,
							maybe more compatibility issues
						*/
				}
				WriteBgnDestFileLn();
				WriteXMLtagBegin("AdditionalIncludeDirectories");
				Write_cfg_d_ToDestFile();
				WriteCStrToDestFile(";%(AdditionalIncludeDirectories)");
				WriteXMLtagEnd("AdditionalIncludeDirectories");
				WriteEndDestFileLn();

				WriteXMLtagBeginValEndLine("FunctionLevelLinking",
					"true");
				WriteXMLtagBeginValEndLine("ExceptionHandling",
					"false");
			WriteEndXMLtagLine("ClCompile");

			WriteBeginXMLtagLine("ResourceCompile");
				WriteBgnDestFileLn();
				WriteXMLtagBegin("AdditionalIncludeDirectories");
				Write_src_d_ToDestFile();
				WriteCStrToDestFile(";%(AdditionalIncludeDirectories)");
				WriteXMLtagEnd("AdditionalIncludeDirectories");
				WriteEndDestFileLn();
			WriteEndXMLtagLine("ResourceCompile");

			WriteBeginXMLtagLine("Link");
				WriteXMLtagBeginValEndLine("SubSystem", "Windows");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagBeginValEndLine(
						"GenerateDebugInformation", "true");
				} else {
					WriteXMLtagBeginValEndLine(
						"GenerateDebugInformation", "false");
				}
				if (gbk_dbg_on != gbo_dbg) {
					WriteXMLtagBeginValEndLine("EnableCOMDATFolding",
						"true");
					WriteXMLtagBeginValEndLine("OptimizeReferences",
						"true");
				}
				WriteDestFileLn("<AdditionalDependencies>"
					"winmm.lib;%(AdditionalDependencies)"
					"</AdditionalDependencies>");
				WriteXMLtagBeginProcValEndLine("OutputFile",
					WriteAppNamePath);
				WriteXMLtagBeginValEndLine("RandomizedBaseAddress",
					"false");
			WriteEndXMLtagLine("Link");
		--DestFileIndent;
		WriteDestFileLn("</ItemDefinitionGroup>");

		WriteBeginXMLtagLine("ItemGroup");
			DoAllDocTypesWithSetup(DoMSVC10XMLAddDocType);
		WriteEndXMLtagLine("ItemGroup");

		WriteBeginXMLtagLine("ItemGroup");
			DoAllSrcFilesWithSetup(
				DoSrcFileMSVC10XMLAddHeaderFile);
			DoAllExtraHeaders2WithSetup(
				DoMSVC10XMLAddAddExtraHeader);
		WriteEndXMLtagLine("ItemGroup");

		WriteBeginXMLtagLine("ItemGroup");
			DoAllSrcFilesWithSetup(
				DoSrcFileMSVC10XMLAddFile);
		WriteEndXMLtagLine("ItemGroup");

		WriteBeginXMLtagLine("ItemGroup");
			WriteBgnDestFileLn();
			WriteCStrToDestFile("<ResourceCompile Include=\"");
			WriteMainRsrcSrcPath();
			WriteCStrToDestFile("\" />");
			WriteEndDestFileLn();
		WriteEndXMLtagLine("ItemGroup");

		WriteDestFileLn("<Import Project=\""
			"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />");
		WriteDestFileLn("<ImportGroup Label=\"ExtensionTargets\">");
		WriteDestFileLn("</ImportGroup>");
	--DestFileIndent;
	WriteDestFileLn("</Project>");
}

LOCALPROC DoSrcFileMSVC10XMLAddClCompile(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<ClCompile Include=\"");
	WriteSrcFileFilePath();
	WriteCStrToDestFile("\">");
	WriteEndDestFileLn();
	++DestFileIndent;

		WriteDestFileLn("<Filter>Source Files</Filter>");

	--DestFileIndent;
	WriteDestFileLn("</ClCompile>");
}

LOCALPROC DoSrcFileMSVC10XMLAddClInclude(void)
{
	if ((DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader) == 0) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("<ClInclude Include=\"");
		WriteSrcFileHeaderPath();
		WriteCStrToDestFile("\">");
		WriteEndDestFileLn();
		++DestFileIndent;

			WriteDestFileLn("<Filter>Header Files</Filter>");

		--DestFileIndent;
		WriteDestFileLn("</ClInclude>");
	}
}

LOCALPROC DoExtraHeaderMSVC10XMLAddClInclude(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<ClInclude Include=\"");
	WriteExtraHeaderFilePath();
	WriteCStrToDestFile("\">");
	WriteEndDestFileLn();
	++DestFileIndent;

		WriteDestFileLn("<Filter>Include Files</Filter>");

	--DestFileIndent;
	WriteDestFileLn("</ClInclude>");
}

LOCALPROC WriteDocTypeMSVC10resource(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<None Include=\"");
	WriteDocTypeIconFilePath();
	WriteCStrToDestFile("\">");
	WriteEndDestFileLn();
	++DestFileIndent;
		WriteDestFileLn("<Filter>Resource Files</Filter>");
	--DestFileIndent;
	WriteDestFileLn("</None>");
}

LOCALPROC WriteMSVC10XMLProjectFiltersProps(void)
{
	WriteCStrToDestFile(" ToolsVersion=\"4.0\" xmlns="
		"\"http://schemas.microsoft.com/developer/msbuild/2003\"");
}

LOCALPROC WriteMSVC10XMLProjectFiltersBodyFilters(void)
{
	WriteDestFileLn("<Filter Include=\"Header Files\">");
	++DestFileIndent;
		WriteDestFileLn("<UniqueIdentifier>"
			"{00030000-0000-0000-0000-000000000000}"
			"</UniqueIdentifier>");
		WriteDestFileLn("<Extensions>"
			"h;hpp;hxx;hm;inl;inc;xsd"
			"</Extensions>");
	--DestFileIndent;
	WriteDestFileLn("</Filter>");

	WriteDestFileLn("<Filter Include=\"Resource Files\">");
	++DestFileIndent;
		WriteDestFileLn("<UniqueIdentifier>"
			"{00040000-0000-0000-0000-000000000000}"
			"</UniqueIdentifier>");
		WriteDestFileLn("<Extensions>"
			"rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;"
			"resx;tiff;tif;png;wav"
			"</Extensions>");
	--DestFileIndent;
	WriteDestFileLn("</Filter>");

	WriteDestFileLn("<Filter Include=\"Include Files\">");
	++DestFileIndent;
		WriteDestFileLn("<UniqueIdentifier>"
			"{00050000-0000-0000-0000-000000000000}"
			"</UniqueIdentifier>");
		WriteDestFileLn("<Extensions>"
			"h;hpp;hxx;hm;inl;inc;xsd"
			"</Extensions>");
	--DestFileIndent;
	WriteDestFileLn("</Filter>");

	WriteDestFileLn("<Filter Include=\"Source Files\">");
	++DestFileIndent;
		WriteDestFileLn("<UniqueIdentifier>"
			"{00020000-0000-0000-0000-000000000000}"
			"</UniqueIdentifier>");
		WriteDestFileLn("<Extensions>"
			"cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
			"</Extensions>");
	--DestFileIndent;
	WriteDestFileLn("</Filter>");
}

LOCALPROC WriteMSVC10XMLProjectFiltersBodyClCompiles(void)
{
	DoAllSrcFilesWithSetup(
		DoSrcFileMSVC10XMLAddClCompile);
}

LOCALPROC WriteMSVC10XMLProjectFiltersBodyClIncludes(void)
{
	DoAllSrcFilesWithSetup(
		DoSrcFileMSVC10XMLAddClInclude);
	DoAllExtraHeaders2WithSetup(
		DoExtraHeaderMSVC10XMLAddClInclude);
}

LOCALPROC WriteMSVC10XMLProjectFiltersBodyResourceCompileProp(void)
{
	WriteCStrToDestFile(" Include=\"");
	WriteMainRsrcSrcPath();
	WriteCStrToDestFile("\"");
}

LOCALPROC WriteMSVC10XMLProjectFiltersBodyResourceCompileBody(void)
{
	WriteXMLtagBeginValEndLine("Filter", "Resource Files");
}

LOCALPROC WriteMSVC10XMLProjectFiltersBodyResourceCompiles(void)
{
	WriteXMLtaggedLinesWith1LnProps("ResourceCompile",
		WriteMSVC10XMLProjectFiltersBodyResourceCompileProp,
		WriteMSVC10XMLProjectFiltersBodyResourceCompileBody);
}

LOCALPROC WriteMSVC10XMLProjectFiltersBodyResourceFiles(void)
{
	DoAllDocTypesWithSetup(WriteDocTypeMSVC10resource);
}

LOCALPROC WriteMSVC10XMLProjectFiltersItemGroup(MyProc p)
{
	WriteXMLtaggedLines("ItemGroup", p);
}

LOCALPROC WriteMSVC10XMLProjectFiltersBody(void)
{
	WriteMSVC10XMLProjectFiltersItemGroup(
		WriteMSVC10XMLProjectFiltersBodyFilters);
	WriteMSVC10XMLProjectFiltersItemGroup(
		WriteMSVC10XMLProjectFiltersBodyClCompiles);
	WriteMSVC10XMLProjectFiltersItemGroup(
		WriteMSVC10XMLProjectFiltersBodyClIncludes);
	WriteMSVC10XMLProjectFiltersItemGroup(
		WriteMSVC10XMLProjectFiltersBodyResourceCompiles);
	WriteMSVC10XMLProjectFiltersItemGroup(
		WriteMSVC10XMLProjectFiltersBodyResourceFiles);
}

LOCALPROC WriteMSVCXML10ProjectFiltersFile(void)
{
	WriteCStrToDestFile("\357\273\277"); /* UTF-8 byte-order mark */
	WriteDestFileLn("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	WriteXMLtaggedLinesWith1LnProps("Project",
		WriteMSVC10XMLProjectFiltersProps,
		WriteMSVC10XMLProjectFiltersBody);
}

LOCALPROC WriteMSVCXML10SpecificFiles(void)
{
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, ".sln", "Solutions file",
		WriteMSVCXMLSolutionFile);
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, ".vcxproj", "Project file",
		WriteMSVCXML10ProjectFile);
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, ".vcxproj.filters", "Project Filters file",
		WriteMSVCXML10ProjectFiltersFile);
}

LOCALPROC WriteMsvSpecificFiles(void)
{
	if (UseCmndLine) {
		WriteNMakeSpecificFiles();
	} else {
		if (ide_vers >= 10000) {
			WriteMSVCXML10SpecificFiles();
		} else if (ide_vers >= 7000) {
			WriteMSVCXMLSpecificFiles();
		} else {
			WriteMSVCSpecificFiles();
		}
	}
}
