/*
	WRMW8FLS.i
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
	WRite MetroWerks 8 specific FiLeS
*/



static void WriteXMLtagSettingNameProcVal(char *n, MyProc v)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("SETTING");
	WriteXMLtagBegin("NAME");
	WriteCStrToDestFile(n);
	WriteXMLtagEnd("NAME");
	WriteXMLtagBegin("VALUE");
	v();
	WriteXMLtagEnd("VALUE");
	WriteXMLtagEnd("SETTING");
	WriteEndDestFileLn();
}

static void WriteXMLtagSettingNameVal(char *n, char *v)
{
	MyPtr SavepDt = pDt;

	pDt = (MyPtr)v;
	WriteXMLtagSettingNameProcVal(n, WritepDtString);
	pDt = SavepDt;
}

static void WriteBeginNamedSettingXMLtagLine(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("SETTING");
	WriteXMLtagBegin("NAME");
	WriteCStrToDestFile(s);
	WriteXMLtagEnd("NAME");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAllMWTargetName(void)
{
	WriteXMLtagBeginProcValEndLine("TARGETNAME", WriteAppVariationStr);
}

typedef void (*tWriteMWLib)(char *s);

static void WriteMWLibs(tWriteMWLib p)
{
	if (gbk_targfam_mswn == gbo_targfam) {
		p("kernel32.lib");
		p("user32.lib");
		p("gdi32.lib");
		p("comdlg32.lib");
		p("shell32.lib");
		p("ole32.lib");
		p("uuid.lib");
		p("winmm.lib");
		if (WantIconMaster) {
			p("advapi32.lib");
		}
		p("MSL_All_x86.lib");
	} else if (gbk_targfam_mach == gbo_targfam) {
		p("crt1.o");
		p("MSL_All_Mach-O_D.lib");
	} else {
		if (gbk_targfam_carb == gbo_targfam) {
			p("CarbonLib");
#if UseOpenGLinOSX
				p("OpenGLLibraryStub");
#endif
		} else {
			p("InterfaceLib");
			p("MathLib");
			p("AppearanceLib");
			p("MenusLib");
			p("NavigationLib");
			p("DragLib");
			p("WindowsLib");
		}
		p("MSL_Runtime_PPC.Lib");
	}
}

static void WriteMWLibAddFile(char *s)
{
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
		if (gbk_targfam_mswn == gbo_targfam) {
			WriteXMLtagBeginValEndLine("FILEKIND", "Unknown");
		} else {
			WriteXMLtagBeginValEndLine("FILEKIND", "Library");
		}
		WriteXMLtagBeginValEndLine("FILEFLAGS", "");
	WriteEndXMLtagLine("FILE");
}

static void WriteMWLibMakeObjects(char *s)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void WriteMWLibGroupList(char *s)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginValEndLine("PATH", s);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void WriteMWProjRelSearchPath(MyProc p)
{
	WriteBeginXMLtagLine("SETTING");
		WriteBeginNamedSettingXMLtagLine("SearchPath");
			WriteBgnDestFileLn();
			WriteXMLtagBegin("SETTING");
			WriteXMLtagBegin("NAME");
			WriteCStrToDestFile("Path");
			WriteXMLtagEnd("NAME");
			WriteXMLtagBegin("VALUE");
			p();
			WriteXMLtagEnd("VALUE");
			WriteXMLtagEnd("SETTING");
			WriteEndDestFileLn();

			WriteXMLtagSettingNameVal("PathFormat", "MacOS");
			WriteXMLtagSettingNameVal("PathRoot", "Project");
		WriteEndXMLtagLine("SETTING");
		WriteXMLtagSettingNameVal("Recursive", "true");
		WriteXMLtagSettingNameVal("FrameworkPath", "false");
		WriteXMLtagSettingNameVal("HostFlags", "All");
	WriteEndXMLtagLine("SETTING");
}

static void WriteMWDrvRelSearchPath(char *s)
{
	WriteBeginXMLtagLine("SETTING");
		WriteBeginNamedSettingXMLtagLine("SearchPath");
			WriteXMLtagSettingNameVal("Path", s);
			WriteXMLtagSettingNameVal("PathFormat", "MacOS");
			WriteXMLtagSettingNameVal("PathRoot", "Project");
		WriteEndXMLtagLine("SETTING");
		WriteXMLtagSettingNameVal("Recursive", "true");
		WriteXMLtagSettingNameVal("FrameworkPath", "false");
		WriteXMLtagSettingNameVal("HostFlags", "All");
	WriteEndXMLtagLine("SETTING");
}

static void WriteBeginXMLMWGroup(char *s)
{
	WriteBgnDestFileLn();
	WriteXMLtagBegin("GROUP");
	WriteXMLtagBegin("NAME");
	WriteCStrToDestFile(s);
	WriteXMLtagEnd("NAME");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteMWSettingsPanelComment(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<!-- Settings for ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(s);
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" panel -->");
	WriteEndDestFileLn();
}

static void WriteDocTypeMWAddFile(void)
{
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH",
			WriteDocTypeIconFileName);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
		WriteXMLtagBeginValEndLine("FILEKIND", "Unknown");
		WriteXMLtagBeginValEndLine("FILEFLAGS", "");
	WriteEndXMLtagLine("FILE");
}

static void WriteDocTypeMWMakeObjects(void)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH",
			WriteDocTypeIconFileName);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void WriteDocTypeMWGroupList(void)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH",
			WriteDocTypeIconFileName);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

static void DoSrcFileMW8AddFile(void)
{
	WriteBeginXMLtagLine("FILE");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH", WriteSrcFileFileName);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
		WriteXMLtagBeginValEndLine("FILEKIND", "Text");
		if (gbk_dbg_on == gbo_dbg) {
			WriteXMLtagBeginValEndLine("FILEFLAGS", "Debug");
		} else {
			WriteXMLtagBeginValEndLine("FILEFLAGS", "");
		}
	WriteEndXMLtagLine("FILE");
}

static void DoSrcFileMW8sMakeObjects(void)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH", WriteSrcFileFileName);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

LOCALPROC DoSrcFileMW8GroupList(void)
{
	WriteBeginXMLtagLine("FILEREF");
		WriteAllMWTargetName();
		WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
		WriteXMLtagBeginProcValEndLine("PATH", WriteSrcFileFileName);
		WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
	WriteEndXMLtagLine("FILEREF");
}

LOCALPROC WriteMetrowerksProjectFile(void)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile("<?xml version=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("1.0");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" encoding=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("UTF-8");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" standalone=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("yes");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ?>");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<?codewarrior exportversion=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("1.0.1");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ideversion=");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("5.0");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(" ?>");
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteBeginXMLtagLine("PROJECT");
	WriteBeginXMLtagLine("TARGETLIST");
	WriteBeginXMLtagLine("TARGET");
		WriteXMLtagBeginProcValEndLine("NAME", WriteAppVariationStr);
		WriteBeginXMLtagLine("SETTINGLIST");
			WriteMWSettingsPanelComment("Access Paths");
			if (gbk_targfam_mach == gbo_targfam) {
				WriteXMLtagSettingNameVal(
					"InterpretDOSAndUnixPaths", "true");
				WriteXMLtagSettingNameVal(
					"RequireFrameworkStyleIncludes", "true");
			}
			WriteBeginNamedSettingXMLtagLine("UserSearchPaths");
				WriteMWProjRelSearchPath(Write_src_d_ToDestFile);
				WriteMWProjRelSearchPath(Write_cfg_d_ToDestFile);
				if (gbk_targfam_mach == gbo_targfam) {
					/* seems to be wanted by property list compiler */
					WriteMWDrvRelSearchPath(":");
				}
			WriteEndXMLtagLine("SETTING");
			WriteBeginNamedSettingXMLtagLine("SystemSearchPaths");
				if (gbk_targfam_mswn == gbo_targfam) {
					WriteBeginXMLtagLine("SETTING");
						WriteBeginNamedSettingXMLtagLine("SearchPath");
							WriteXMLtagSettingNameVal("Path", ":MSL:");
							WriteXMLtagSettingNameVal("PathFormat",
								"MacOS");
							WriteXMLtagSettingNameVal("PathRoot",
								"CodeWarrior");
						WriteEndXMLtagLine("SETTING");
						WriteXMLtagSettingNameVal("Recursive", "true");
						WriteXMLtagSettingNameVal("FrameworkPath",
							"false");
						WriteXMLtagSettingNameVal("HostFlags", "All");
					WriteEndXMLtagLine("SETTING");

					WriteBeginXMLtagLine("SETTING");
						WriteBeginNamedSettingXMLtagLine("SearchPath");
							WriteXMLtagSettingNameVal("Path",
								":Win32-x86 Support:");
							WriteXMLtagSettingNameVal("PathFormat",
								"MacOS");
							WriteXMLtagSettingNameVal("PathRoot",
								"CodeWarrior");
						WriteEndXMLtagLine("SETTING");
						WriteXMLtagSettingNameVal("Recursive", "true");
						WriteXMLtagSettingNameVal("FrameworkPath",
							"false");
						WriteXMLtagSettingNameVal("HostFlags", "All");
					WriteEndXMLtagLine("SETTING");
				} else {
					WriteBeginXMLtagLine("SETTING");
						WriteBeginNamedSettingXMLtagLine("SearchPath");
							WriteXMLtagSettingNameVal("Path",
								":MSL:MSL_C:");
							WriteXMLtagSettingNameVal("PathFormat",
								"MacOS");
							WriteXMLtagSettingNameVal("PathRoot",
								"CodeWarrior");
						WriteEndXMLtagLine("SETTING");
						WriteXMLtagSettingNameVal("Recursive", "true");
						WriteXMLtagSettingNameVal("FrameworkPath",
							"false");
						WriteXMLtagSettingNameVal("HostFlags", "All");
					WriteEndXMLtagLine("SETTING");

					if (gbk_targfam_mach == gbo_targfam) {
						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine(
								"SearchPath");
								WriteXMLtagSettingNameVal("Path",
									":MacOS X Support:");
								WriteXMLtagSettingNameVal("PathFormat",
									"MacOS");
								WriteXMLtagSettingNameVal("PathRoot",
									"CodeWarrior");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive",
								"true");
							WriteXMLtagSettingNameVal("FrameworkPath",
								"false");
							WriteXMLtagSettingNameVal("HostFlags",
								"All");
						WriteEndXMLtagLine("SETTING");

						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine(
								"SearchPath");
								WriteXMLtagSettingNameVal("Path",
									"/usr/include");
								WriteXMLtagSettingNameVal("PathFormat",
									"Unix");
								WriteXMLtagSettingNameVal("PathRoot",
									"Absolute");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive",
								"false");
							WriteXMLtagSettingNameVal("FrameworkPath",
								"false");
							WriteXMLtagSettingNameVal("HostFlags",
								"All");
						WriteEndXMLtagLine("SETTING");

						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine(
								"SearchPath");
								WriteXMLtagSettingNameVal("Path",
									"/usr/lib");
								WriteXMLtagSettingNameVal("PathFormat",
									"Unix");
								WriteXMLtagSettingNameVal("PathRoot",
									"Absolute");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive",
								"false");
							WriteXMLtagSettingNameVal("FrameworkPath",
								"false");
							WriteXMLtagSettingNameVal("HostFlags",
								"All");
						WriteEndXMLtagLine("SETTING");

						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine(
								"SearchPath");
								WriteXMLtagSettingNameVal("Path",
									"System/Library/Frameworks");
								WriteXMLtagSettingNameVal("PathFormat",
									"Unix");
								WriteXMLtagSettingNameVal("PathRoot",
									"OS X Volume");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive",
								"false");
							WriteXMLtagSettingNameVal("FrameworkPath",
								"true");
							WriteXMLtagSettingNameVal("HostFlags",
								"All");
						WriteEndXMLtagLine("SETTING");
					} else {
						WriteBeginXMLtagLine("SETTING");
							WriteBeginNamedSettingXMLtagLine(
								"SearchPath");
								WriteXMLtagSettingNameVal("Path",
									":MacOS Support:");
								WriteXMLtagSettingNameVal("PathFormat",
									"MacOS");
								WriteXMLtagSettingNameVal("PathRoot",
									"CodeWarrior");
							WriteEndXMLtagLine("SETTING");
							WriteXMLtagSettingNameVal("Recursive",
								"true");
							WriteXMLtagSettingNameVal("FrameworkPath",
								"false");
							WriteXMLtagSettingNameVal("HostFlags",
								"All");
						WriteEndXMLtagLine("SETTING");
					}
				}

			WriteEndXMLtagLine("SETTING");
			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("Target Settings");
			if (gbk_targ_wx86 == cur_targ) {
				WriteXMLtagSettingNameVal("Linker",
					"Win32 x86 Linker");
			} else if (gbk_targ_mach == cur_targ) {
				WriteXMLtagSettingNameVal("Linker",
					"MacOS X PPC Linker");
			} else {
				WriteXMLtagSettingNameVal("Linker",
					"MacOS PPC Linker");
			}
			WriteXMLtagSettingNameProcVal("Targetname",
				WriteAppVariationStr);

			WriteBeginNamedSettingXMLtagLine("OutputDirectory");
				WriteXMLtagSettingNameVal("Path", ":");
				WriteXMLtagSettingNameVal("PathFormat", "MacOS");
				WriteXMLtagSettingNameVal("PathRoot", "Project");
			WriteEndXMLtagLine("SETTING");

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("Build Extras");
			WriteXMLtagSettingNameVal("BrowserGenerator", "0");

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("Debugger Target");
			WriteXMLtagSettingNameVal("StopAtTempBPOnLaunch", "false");

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("C/C++ Compiler");
			WriteXMLtagSettingNameVal("MWFrontEnd_C_checkprotos", "1");
			WriteXMLtagSettingNameVal("MWFrontEnd_C_enableexceptions",
				"0");
			if ((gbk_dbg_on == gbo_dbg)
				|| (gbk_targfam_mswn == gbo_targfam))
			{
				/* inlining seems to give bad code for x86 version */
				WriteXMLtagSettingNameVal("MWFrontEnd_C_dontinline",
					"1");
			} else {
				WriteXMLtagSettingNameVal("MWFrontEnd_C_dontinline",
					"0");
			}
			WriteXMLtagSettingNameVal("MWFrontEnd_C_useRTTI", "0");
			if (gbk_dbg_on == gbo_dbg) {
				WriteXMLtagSettingNameVal("MWFrontEnd_C_autoinline",
					"0");
			} else {
				WriteXMLtagSettingNameVal("MWFrontEnd_C_autoinline",
					"1");
			}

			WriteBlankLineToDestFile();
			WriteMWSettingsPanelComment("C/C++ Warnings");
			WriteXMLtagSettingNameVal(
				"MWWarning_C_warn_illpragma", "1");
			WriteXMLtagSettingNameVal(
				"MWWarning_C_warn_emptydecl", "1");
			WriteXMLtagSettingNameVal(
				"MWWarning_C_warn_possunwant", "1");
			WriteXMLtagSettingNameVal(
				"MWWarning_C_warn_unusedvar", "1");
			WriteXMLtagSettingNameVal(
				"MWWarning_C_warn_unusedarg", "1");
			WriteXMLtagSettingNameVal(
				"MWWarning_C_warn_extracomma", "1");
			WriteXMLtagSettingNameVal(
				"MWWarning_C_pedantic", "1");

			WriteBlankLineToDestFile();
			if (gbk_targfam_mswn == gbo_targfam) {
				WriteMWSettingsPanelComment("x86 CodeGen");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_X86_intrinsics", "0");
				} else {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_X86_intrinsics", "1");
				}
			} else if (gbk_targ_mach == cur_targ) {
				WriteMWSettingsPanelComment("PPC CodeGen Mach-O");
				WriteXMLtagSettingNameVal(
					"MWCodeGen_MachO_structalignment", "PPC");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_MachO_peephole", "0");
					WriteXMLtagSettingNameVal(
						"MWCodeGen_MachO_schedule", "0");
				} else {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_MachO_peephole", "1");
					WriteXMLtagSettingNameVal(
						"MWCodeGen_MachO_schedule", "1");
				}
			} else {
				WriteMWSettingsPanelComment("PPC CodeGen");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_PPC_tracebacktables", "Inline");
				} else {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_PPC_tracebacktables", "None");
				}
				WriteXMLtagSettingNameVal(
					"MWCodeGen_PPC_vectortocdata", "0");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_PPC_peephole", "0");
					WriteXMLtagSettingNameVal(
						"MWCodeGen_PPC_schedule", "0");
				} else {
					WriteXMLtagSettingNameVal(
						"MWCodeGen_PPC_peephole", "1");
					WriteXMLtagSettingNameVal(
						"MWCodeGen_PPC_schedule", "1");
				}
			}

			WriteBlankLineToDestFile();
			if (gbk_targfam_mswn == gbo_targfam) {
				WriteMWSettingsPanelComment("x86 Global Optimizer");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"GlobalOptimizer_X86_optimizationlevel",
						"Level0");
				} else {
					WriteXMLtagSettingNameVal(
						"GlobalOptimizer_X86_optimizationlevel",
						"Level1");
					/*
						Level4 (and Level3 and Level2)
						generates bade code.
					*/
				}
				WriteXMLtagSettingNameVal(
					"GlobalOptimizer_X86_optfor", "Size");

				/*
					work around what is probably bug
					in windows version of mw8
				*/
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"GlobalOptimizer_X86__optimizationlevel",
						"Level0");
				} else {
					WriteXMLtagSettingNameVal(
						"GlobalOptimizer_X86__optimizationlevel",
						"Level1");
				}
				WriteXMLtagSettingNameVal(
					"GlobalOptimizer_X86__optfor", "Size");
			} else {
				WriteMWSettingsPanelComment("PPC Global Optimizer");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"GlobalOptimizer_PPC_optimizationlevel",
						"Level0");
				} else {
					WriteXMLtagSettingNameVal(
						"GlobalOptimizer_PPC_optimizationlevel",
						"Level4");
				}
				WriteXMLtagSettingNameVal(
					"GlobalOptimizer_PPC_optfor", "Size");
			}

			WriteBlankLineToDestFile();
			if (gbk_targfam_mswn == gbo_targfam) {
				WriteMWSettingsPanelComment("x86 Linker");
				WriteXMLtagSettingNameVal(
					"MWLinker_X86_subsystem", "WinGUI");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWLinker_X86_linkdebug", "true");
				} else {
					WriteXMLtagSettingNameVal(
						"MWLinker_X86_linkdebug", "false");
				}
				WriteXMLtagSettingNameVal(
					"MWLinker_X86_usedefaultlibs", "false");
			} else if (gbk_targ_mach == cur_targ) {
				WriteMWSettingsPanelComment("PPC Mac OS X Linker");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWLinker_MacOSX_linksym", "1");
				} else {
					WriteXMLtagSettingNameVal(
						"MWLinker_MacOSX_linksym", "0");
				}
				WriteXMLtagSettingNameVal(
					"MWLinker_MacOSX_symfullpath", "1");
				WriteXMLtagSettingNameVal(
					"MWLinker_MacOSX_permitmultdefs", "0");
				if (gbk_dbg_on != gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWLinker_MacOSX_strip_debug_symbols", "1");
				}
			} else {
				WriteMWSettingsPanelComment("PPC Linker");
				if (gbk_dbg_on == gbo_dbg) {
					WriteXMLtagSettingNameVal(
						"MWLinker_PPC_linksym", "1");
				} else {
					WriteXMLtagSettingNameVal(
						"MWLinker_PPC_linksym", "0");
				}
				WriteXMLtagSettingNameVal(
					"MWLinker_PPC_linkmode", "Normal");
			}

			WriteBlankLineToDestFile();
			if (gbk_targfam_mswn == gbo_targfam) {
				WriteMWSettingsPanelComment("x86 Project");
				WriteXMLtagSettingNameProcVal(
					"MWProject_X86_outfile", WriteAppNameStr);
			} else if (gbk_targ_mach == cur_targ) {
				WriteMWSettingsPanelComment("PPC Mac OS X Project");
				WriteXMLtagSettingNameVal(
					"MWProject_MacOSX_type", "ApplicationPackage");
				WriteXMLtagSettingNameProcVal(
					"MWProject_MacOSX_outfile", WriteAppNameStr);
				WriteXMLtagSettingNameVal(
					"MWProject_MacOSX_filecreator", kMacCreatorSig);
				WriteXMLtagSettingNameVal(
					"MWProject_MacOSX_filetype", "APPL");
				WriteXMLtagSettingNameVal(
					"MWProject_MacOSX_vmaddress", "0");
				WriteXMLtagSettingNameVal(
					"MWProject_MacOSX_flatrsrc", "1");
				WriteXMLtagSettingNameVal(
					"MWProject_MacOSX_flatrsrcfilename", "");
				WriteBeginNamedSettingXMLtagLine(
					"MWProject_MacOSX_flatrsrcoutputdir");
					WriteXMLtagSettingNameVal("Path", ":");
					WriteXMLtagSettingNameVal("PathFormat", "MacOS");
					WriteXMLtagSettingNameVal("PathRoot", "Project");
				WriteEndXMLtagLine("SETTING");
			} else {
				WriteMWSettingsPanelComment("PPC Project");
				WriteXMLtagSettingNameVal(
					"MWProject_PPC_outfile", vStrAppAbbrev);
				WriteXMLtagSettingNameVal(
					"MWProject_PPC_filecreator", kMacCreatorSig);
				WriteXMLtagSettingNameVal(
					"MWProject_PPC_size", "6000");
				WriteXMLtagSettingNameVal(
					"MWProject_PPC_minsize", "3000");
				WriteXMLtagSettingNameVal(
					"MWProject_PPC_flags", "22768");
			}
		WriteEndXMLtagLine("SETTINGLIST");

		WriteBeginXMLtagLine("FILELIST");
			WriteMWLibs(WriteMWLibAddFile);

			if (gbk_targfam_mswn == gbo_targfam) {
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "");
				WriteEndXMLtagLine("FILE");
			} else if (gbk_targfam_mach == gbo_targfam) {
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "Debug");
				WriteEndXMLtagLine("FILE");
				DoAllDocTypesWithSetup(WriteDocTypeMWAddFile);
			} else {
				WriteBeginXMLtagLine("FILE");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.r");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
					WriteXMLtagBeginValEndLine("FILEKIND", "Text");
					WriteXMLtagBeginValEndLine("FILEFLAGS", "");
				WriteEndXMLtagLine("FILE");
			}

			DoAllSrcFilesWithSetup(DoSrcFileMW8AddFile);
		WriteEndXMLtagLine("FILELIST");

		WriteBeginXMLtagLine("LINKORDER");
			WriteMWLibs(WriteMWLibMakeObjects);
			if (gbk_targfam_mswn == gbo_targfam) {
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
			} else if (gbk_targfam_mach == gbo_targfam) {
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.plc");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
				DoAllDocTypesWithSetup(WriteDocTypeMWMakeObjects);
			} else {
				WriteBeginXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
					WriteXMLtagBeginValEndLine("PATH", "main.r");
					WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
				WriteEndXMLtagLine("FILEREF");
			}
			DoAllSrcFilesSortWithSetup(DoSrcFileMW8sMakeObjects);
		WriteEndXMLtagLine("LINKORDER");

		if (gbk_targfam_mach == gbo_targfam) {
			WriteBeginXMLtagLine("FRAMEWORKLIST");
				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine(
							"PATH", "Carbon.framework");
						WriteXMLtagBeginValEndLine(
							"PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine(
						"DYNAMICLIBRARY", "Carbon");
				WriteEndXMLtagLine("FRAMEWORK");

#if UseOpenGLinOSX
				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine("PATH",
							"OpenGL.framework");
						WriteXMLtagBeginValEndLine(
							"PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine(
						"DYNAMICLIBRARY", "OpenGL");
				WriteEndXMLtagLine("FRAMEWORK");

				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine(
							"PATH", "AGL.framework");
						WriteXMLtagBeginValEndLine(
							"PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine("DYNAMICLIBRARY", "AGL");
				WriteEndXMLtagLine("FRAMEWORK");
#endif

				WriteBeginXMLtagLine("FRAMEWORK");
					WriteBeginXMLtagLine("FILEREF");
						WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
						WriteXMLtagBeginValEndLine(
							"PATH", "System.framework");
						WriteXMLtagBeginValEndLine(
							"PATHFORMAT", "MacOS");
					WriteEndXMLtagLine("FILEREF");
					WriteXMLtagBeginValEndLine(
						"DYNAMICLIBRARY", "System");
				WriteEndXMLtagLine("FRAMEWORK");
			WriteEndXMLtagLine("FRAMEWORKLIST");
		} /* (gbk_targfam_mach == gbo_targfam) */
	WriteEndXMLtagLine("TARGET");
	WriteEndXMLtagLine("TARGETLIST");

	WriteBlankLineToDestFile();
	WriteBeginXMLtagLine("TARGETORDER");
		WriteBgnDestFileLn();
		WriteXMLtagBegin("ORDEREDTARGET");
		WriteXMLtagBegin("NAME");
		WriteAppVariationStr();
		WriteXMLtagEnd("NAME");
		WriteXMLtagEnd("ORDEREDTARGET");
		WriteEndDestFileLn();
	WriteEndXMLtagLine("TARGETORDER");

	WriteBlankLineToDestFile();
	WriteBeginXMLtagLine("GROUPLIST");
		WriteBeginXMLMWGroup("Libraries");
			WriteMWLibs(WriteMWLibGroupList);
		WriteEndXMLtagLine("GROUP");
		WriteBeginXMLMWGroup("Resources");
			WriteBeginXMLtagLine("FILEREF");
				WriteAllMWTargetName();
				WriteXMLtagBeginValEndLine("PATHTYPE", "Name");
				if (gbk_targfam_mswn == gbo_targfam) {
					WriteXMLtagBeginValEndLine("PATH", "main.RC");
				} else {
					if (HaveMacBundleApp) {
						WriteXMLtagBeginValEndLine("PATH", "main.plc");
					} else {
						WriteXMLtagBeginValEndLine("PATH", "main.r");
					}
				}
				WriteXMLtagBeginValEndLine("PATHFORMAT", "MacOS");
			WriteEndXMLtagLine("FILEREF");
			if (HaveMacBundleApp) {
				DoAllDocTypesWithSetup(WriteDocTypeMWGroupList);
			}
		WriteEndXMLtagLine("GROUP");
		DoAllSrcFilesWithSetup(DoSrcFileMW8GroupList);
	WriteEndXMLtagLine("GROUPLIST");

	WriteBlankLineToDestFile();
	WriteEndXMLtagLine("PROJECT");
}

LOCALPROC WriteMetrowerksSpecificFiles(void)
{
	if (HaveMacBundleApp) {
		WritePListData();
	}

	WriteADstFile1("my_project_d",
		vStrAppAbbrev, ".mcp.xml", ".mcp.xml",
		WriteMetrowerksProjectFile);
}
