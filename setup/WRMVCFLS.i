/*
	WRMVCFLS.i
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
	WRite Mini vMac C specific FiLeS
*/


LOCALPROC WriteMVCCompileAsmLinkCommonOptions(void)
{
	{
		switch (cur_targ) {
			case gbk_targ_mach:
			case gbk_targ_imch:
			case gbk_targ_mc64:
			case gbk_targ_mx11:
			case gbk_targ_mi11:
			case gbk_targ_mx64:
				if (gbk_cpufam_x86 == gbo_cpufam) {
					WriteCStrToDestFile(" -arch i386");
				} else if (gbk_cpufam_x64 == gbo_cpufam) {
					WriteCStrToDestFile(" -arch x86_64");
				} else {
					WriteCStrToDestFile(" -arch ppc");
				}
				break;
			default:
				break;
		}
	}

	if ((gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam))
	{
		if (gbk_cpufam_ppc == gbo_cpufam) {
			WriteCStrToDestFile(" -mmacosx-version-min=10.1");
		} else
		if (gbk_cpufam_x64 == gbo_cpufam) {
			WriteCStrToDestFile(" -mmacosx-version-min=10.5");
		} else
		{
			WriteCStrToDestFile(" -mmacosx-version-min=10.4");
		}
	}
}

LOCALPROC WriteMVCLinkOSGlucompileCommonOptions(void)
{
	if ((gbk_apifam_osx == gbo_apifam)
		|| (gbk_apifam_cco == gbo_apifam))
	{
		WriteCStrToDestFile(" -isysroot");
		if (ide_vers >= 3200) {
			WriteCStrToDestFile(" /Developer/SDKs/MacOSX10.6.sdk");
		} else if ((ide_vers >= 3100)
			|| (gbk_cpufam_x64 == gbo_cpufam))
		{
			WriteCStrToDestFile(" /Developer/SDKs/MacOSX10.5.sdk");
		} else {
			WriteCStrToDestFile(" /Developer/SDKs/MacOSX10.4u.sdk");
		}
	}
}

LOCALPROC WriteMVCCompileLinkCommonOptions(void)
{
#if 0
	WriteCStrToDestFile(" -Werror");
#endif
	if (gbk_dbg_on == gbo_dbg) {
		WriteCStrToDestFile(" -g");
	}
}

LOCALPROC WriteMVCCOptions(void)
{
	WriteCStrToDestFile(
		" -Wall -Wmissing-prototypes -Wno-uninitialized");

	WriteCStrToDestFile(" -Wundef -Wstrict-prototypes");

	if (gbk_apifam_osx == gbo_apifam) {
		WriteCStrToDestFile(" -Wno-deprecated-declarations");
	}
	WriteCStrToDestFile(" -fomit-frame-pointer");
	WriteCStrToDestFile(" -fno-strict-aliasing");
	WriteCStrToDestFile(" -fno-asynchronous-unwind-tables");
	WriteCStrToDestFile(
		" -Winline --param large-function-growth=1000");
	if (gbk_targ_wcar == cur_targ) {
		WriteCStrToDestFile(" -fno-leading-underscore");
	}
	switch (cur_targ) {
		case gbk_targ_mach:
		case gbk_targ_imch:
		case gbk_targ_mc64:
		case gbk_targ_mx11:
		case gbk_targ_mi11:
		case gbk_targ_mx64:
			WriteCStrToDestFile(" -mdynamic-no-pic");
			break;
	}
	WriteMVCCompileAsmLinkCommonOptions();
	WriteMVCCompileLinkCommonOptions();

	if (1 /* WantConfigDir */) {
		WriteCStrToDestFile(" -I./");
		Write_cfg_d_Name();
		WriteCStrToDestFile(" -I./");
		Write_src_d_Name();
	}
}

LOCALPROC WriteMVCCOptOptions(void)
{
	if (gbk_dbg_on != gbo_dbg) {
		WriteCStrToDestFile(" -Os");
	} else {
		WriteCStrToDestFile(" -O0");
	}
}

LOCALPROC DoFrameWorkMVCaddFile(void)
{
	WriteCStrToDestFile(" -framework ");
	WriteCStrToDestFile(DoFrameWork_gd()->s);
}

LOCALPROC Write_machoRsrcMVCDeps(void)
{
	WriteMakeDependFile(WriteMainRsrcSrcPath);
	WriteMakeDependFile(Write_machoAppIconPath);
}

LOCALPROC Write_machoRsrcMVCBuild(void)
{
	WriteDestFileLn("/Developer/Tools/Rez \\");
	++DestFileIndent;
		WriteDestFileLn("-i /Developer/Headers/FlatCarbon \\");
		WriteBgnDestFileLn();
			WriteCStrToDestFile("\"");
			WriteMainRsrcSrcPath();
			WriteCStrToDestFile("\" \\");
		WriteEndDestFileLn();
		WriteBgnDestFileLn();
			WriteCStrToDestFile("-o \"");
			Write_machoRsrcPath();
			WriteCStrToDestFile("\" \\");
		WriteEndDestFileLn();
		WriteDestFileLn("-useDF");
	--DestFileIndent;
}

LOCALPROC WriteMVCSrcFileAsmName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	WriteCStrToDestFile(".s");
}

LOCALPROC WriteMVCSrcFileAsmPath(void)
{
	WriteFileInDirToDestFile0(Write_obj_d_ToDestFile,
		WriteMVCSrcFileAsmName);
}

LOCALPROC DoMVCSrcFileMakeCompileBody(void)
{
	blnr UseAPI = (DoSrcFile_gd()->Flgm & kCSrcFlgmUseAPI) != 0;
	blnr Fast = (DoSrcFile_gd()->Flgm & kCSrcFlgmSortFirst) != 0;

	WriteBgnDestFileLn();

	WriteCStrToDestFile("$(my_prefix)gcc -S");
	WritePathArgInMakeCmnd(WriteSrcFileFilePath);
	WriteCStrToDestFile(" -o");
	WritePathArgInMakeCmnd(WriteMVCSrcFileAsmPath);
	WriteSpaceToDestFile();
	if (! UseAPI) {
		if (Fast) {
			WriteMakeVar("mk_COptionsFast");
		} else {
			WriteMakeVar("mk_COptions");
		}
	} else {
		WriteMakeVar("mk_COptionsOSGLU");
	}

	WriteEndDestFileLn();
}

LOCALPROC DoMVCSrcFileMakeAsmBody(void)
{
	WriteBgnDestFileLn();

	WriteCStrToDestFile("$(my_prefix)gcc -c");
	WritePathArgInMakeCmnd(WriteMVCSrcFileAsmPath);
	WriteCStrToDestFile(" -o");
	WritePathArgInMakeCmnd(WriteSrcFileObjPath);

	WriteEndDestFileLn();
}

LOCALPROC DoMVCSrcFileMakeAsmDeps(void)
{
	WriteMakeDependFile(WriteMVCSrcFileAsmPath);
}

LOCALPROC DoMVCSrcFileMakeCompile(void)
{
	WriteMakeRule(WriteMVCSrcFileAsmPath,
		DoSrcFileMakeCompileDeps,
		DoMVCSrcFileMakeCompileBody);
}

LOCALPROC DoMVCSrcFileMakeAsm(void)
{
	WriteMakeRule(WriteSrcFileObjPath,
		DoMVCSrcFileMakeAsmDeps,
		DoMVCSrcFileMakeAsmBody);
}

LOCALPROC DoMVCSrcFileStandardMakeAsms(void)
{
	WriteBgnDestFileLn();
	WriteMVCSrcFileAsmPath();
	WriteSpaceToDestFile();
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC DoMVCAllSrcFilesStandardMakeAsms(void)
{
	DoAllSrcFilesSortWithSetup(DoMVCSrcFileStandardMakeAsms);
}

LOCALPROC WriteMVCMakeFile(void)
{
	WriteDestFileLn("# make file generated by gryphel build system");

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptionsCommon =");
	WriteMVCCOptions();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptionsOSGLU = $(mk_COptionsCommon)");
	if (gbk_apifam_osx == gbo_apifam) {
		WriteCStrToDestFile(" -Wno-multichar");
	}
	WriteMVCLinkOSGlucompileCommonOptions();
	WriteMVCCOptOptions();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptionsNotOS = $(mk_COptionsCommon)");
	if (gbk_cpufam_x86 == gbo_cpufam) {
		WriteCStrToDestFile(" -mpreferred-stack-boundary=2");
	}
	WriteCStrToDestFile(" -fno-toplevel-reorder");
	if (gbk_cpufam_x86 == gbo_cpufam) {
		WriteCStrToDestFile(" -mtune=generic -march=i386");
	}
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptions = $(mk_COptionsNotOS)");
	WriteMVCCOptOptions();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptionsFast = $(mk_COptionsNotOS)");
	if (gbk_dbg_on != gbo_dbg) {
		WriteCStrToDestFile(
			" -O2 -fno-align-functions -fno-align-labels");
	} else {
		WriteMVCCOptOptions();
	}
	WriteEndDestFileLn();

	switch (cur_targ) {
		case gbk_targ_mach:
			WriteDestFileLn("my_prefix = powerpc-apple-darwin9-");
			break;
		case gbk_targ_imch:
			WriteDestFileLn("my_prefix = i686-apple-darwin11-");
			break;
		case gbk_targ_mc64:
			WriteDestFileLn("my_prefix = x86_64-apple-darwin11-");
			break;
		case gbk_targ_wx86:
			WriteDestFileLn("my_prefix = i686-w64-mingw32-");
			break;
		case gbk_targ_wx64:
			WriteDestFileLn("my_prefix = x86_64-w64-mingw32-");
			break;
		case gbk_targ_lx86:
			WriteDestFileLn("my_prefix = i386-pc-linux-");
			break;
		case gbk_targ_lx64:
			WriteDestFileLn("my_prefix = x86_64-linux-gnu-");
			break;
		case gbk_targ_larm:
			WriteDestFileLn("my_prefix = arm-linux-gnueabi-");
			break;
		case gbk_targ_lppc:
			WriteDestFileLn("my_prefix = powerpc-linux-gnu-");
			break;
		case gbk_targ_lspr:
			WriteDestFileLn("my_prefix = sparc-linux-gnu-");
			break;
		case gbk_targ_fbsd:
			WriteDestFileLn("my_prefix = i386-pc-freebsd9-");
			break;
		case gbk_targ_fb64:
			WriteDestFileLn("my_prefix = x86_64-pc-freebsd9-");
			break;
		case gbk_targ_nbsd:
			WriteDestFileLn("my_prefix = i386-pc-netbsdelf-");
			break;
		case gbk_targ_nb64:
			WriteDestFileLn("my_prefix = x86_64-pc-netbsdelf-");
			break;
		case gbk_targ_oind:
			WriteDestFileLn("my_prefix = i386-pc-solaris2.11-");
			break;
		case gbk_targ_oi64:
			WriteDestFileLn("my_prefix = x86_64-pc-solaris2.11-");
			break;
		case gbk_targ_wcar:
			WriteDestFileLn("my_prefix = arm-wince-pe-");
			break;
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn(".PHONY: TheDefaultOutput clean");

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("TheDefaultOutput :");
	WriteMakeDependFile(Write_machobinpath_ToDestFile);
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	DoAllSrcFilesWithSetup(DoMVCSrcFileMakeCompile);

	WriteBlankLineToDestFile();
	DoAllSrcFilesWithSetup(DoMVCSrcFileMakeAsm);

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("AsmFiles = ");
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
	++DestFileIndent;
		DoMVCAllSrcFilesStandardMakeAsms();
		WriteBlankLineToDestFile();
	--DestFileIndent;

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("ObjFiles = ");
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
	++DestFileIndent;
		DoAllSrcFilesStandardMakeObjects();
		WriteBlankLineToDestFile();
	--DestFileIndent;

	if (HaveMacBundleApp) {
		WriteBlankLineToDestFile();
		WriteMakeRule(Write_machoAppIconPath,
			Write_tmachoShellDeps,
			Write_tmachoShell);
	}
	if (gbk_apifam_win == gbo_apifam) {
		WriteBlankLineToDestFile();
		WriteBgnDestFileLn();
		WriteMainRsrcObjPath();
		WriteCStrToDestFile(": ");
		WriteMainRsrcSrcPath();
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToDestFile("$(my_prefix)windres");
			WriteCStrToDestFile(" -i");
			WritePathArgInMakeCmnd(WriteMainRsrcSrcPath);
			WriteCStrToDestFile(" --input-format=rc -o");
			WritePathArgInMakeCmnd(WriteMainRsrcObjPath);
			WriteCStrToDestFile(" -O coff  --include-dir SRC");
			WriteEndDestFileLn();
		--DestFileIndent;
		WriteBlankLineToDestFile();
	}

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	Write_machobinpath_ToDestFile();
	WriteCStrToDestFile(" : $(AsmFiles) $(ObjFiles)");
	if (HaveMacBundleApp) {
		WriteMakeDependFile(Write_machoAppIconPath);
	}
	if (HaveMacRrscs) {
		WriteMakeDependFile(Write_machoRsrcPath);
	}
	if (gbk_apifam_win == gbo_apifam) {
		WriteMakeDependFile(WriteMainRsrcObjPath);
	}
	WriteEndDestFileLn();
	++DestFileIndent;
		WriteBgnDestFileLn();
		switch (cur_targ) {
			case gbk_targ_mach:
			case gbk_targ_imch:
			case gbk_targ_mc64:
			case gbk_targ_mx11:
			case gbk_targ_mi11:
			case gbk_targ_mx64:
				WriteCStrToDestFile("gcc");
				break;
			default:
				WriteCStrToDestFile("$(my_prefix)gcc");
				break;
		}
		WriteCStrToDestFile(" \\");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToDestFile("-o");
			WritePathArgInMakeCmnd(Write_machobinpath_ToDestFile);
			WriteCStrToDestFile(" \\");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("$(ObjFiles)");
			if ((gbk_apifam_osx == gbo_apifam)
				|| (gbk_apifam_cco == gbo_apifam))
			{
				DoAllFrameWorksWithSetup(DoFrameWorkMVCaddFile);
				if (gbk_targ_mach == cur_targ) {
					WriteCStrToDestFile(
						" /usr/local/mvmc/mach"
						"/lib/darwin-gpsave.o");
				}
				if (ide_vers >= 4000) {
					WriteCStrToDestFile(" -Wl,-no_pie");
				}
				WriteCStrToDestFile(" -nodefaultlibs -lSystem");
				if ((gbk_targ_mach == cur_targ) && WantLocalTalk) {
					WriteCStrToDestFile(" -lSystemStubs");
				}
			} else if (gbk_apifam_win == gbo_apifam) {
				WritePathArgInMakeCmnd(WriteMainRsrcObjPath);
				if (gbk_targ_wcar == cur_targ) {
					WriteCStrToDestFile(
						" /usr/local/mvmc/wcar/lib/divlib.o"
						" -lmingw32");
					WriteCStrToDestFile(
						" -lcommctrl -lcoredll -laygshell -lmmtimer");
					WriteCStrToDestFile(" -static-libgcc");
				} else {
					WriteCStrToDestFile(
						" -mwindows -lwinmm -lole32 -luuid");
				}
			} else {
				if (gbk_targfam_slrs == gbo_targfam) {
					WriteCStrToDestFile(" -lposix4");
				}
#if MayUseSound
				if (gbk_sndapi_alsa == gbo_sndapi) {
					WriteCStrToDestFile(" -ldl");
#if 0
					WriteCStrToDestFile(" -lasound");
#endif
				} else if (gbk_sndapi_ddsp == gbo_sndapi) {
					if ((gbk_targfam_nbsd == gbo_targfam)
						|| (gbk_targfam_obsd == gbo_targfam))
					{
						WriteCStrToDestFile(" -lossaudio");
					}
				}
#endif
#if 0
				WriteCStrToDestFile(" -lXext");
#endif
				if (gbk_targfam_nbsd == gbo_targfam) {
					WriteCStrToDestFile(" -R/usr/X11R7/lib");
				}
				WriteCStrToDestFile(" -lX11");
				switch (cur_targ) {
					case gbk_targ_lx86:
					case gbk_targ_lx64:
					case gbk_targ_larm:
					case gbk_targ_lppc:
					case gbk_targ_lspr:
					case gbk_targ_fbsd:
					case gbk_targ_fb64:
					case gbk_targ_nbsd:
					case gbk_targ_nb64:
					case gbk_targ_oind:
					case gbk_targ_oi64:
					case gbk_targ_wcar:
						WriteCStrToDestFile(" -static-libgcc");
						break;
				}
			}
			WriteMVCCompileAsmLinkCommonOptions();
			WriteMVCLinkOSGlucompileCommonOptions();
			WriteMVCCompileLinkCommonOptions();
			WriteEndDestFileLn();
		--DestFileIndent;
		if (gbk_dbg_on != gbo_dbg) {
			WriteBgnDestFileLn();
			switch (cur_targ) {
				case gbk_targ_mach:
				case gbk_targ_imch:
				case gbk_targ_mc64:
				case gbk_targ_mx11:
				case gbk_targ_mi11:
				case gbk_targ_mx64:
					WriteCStrToDestFile("strip -u -r");
					break;
				default:
					WriteCStrToDestFile("$(my_prefix)strip");
					break;
			}
			WritePathArgInMakeCmnd(
				Write_machobinpath_ToDestFile);
			WriteEndDestFileLn();
		}
	--DestFileIndent;

	if (HaveMacRrscs) {
		WriteBlankLineToDestFile();
		WriteMakeRule(Write_machoRsrcPath,
			Write_machoRsrcMVCDeps,
			Write_machoRsrcMVCBuild);
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("clean :");
	++DestFileIndent;
		WriteDestFileLn("rm -f $(AsmFiles) $(ObjFiles)");
		if (HaveMacBundleApp) {
			WriteRmDir(WriteAppNamePath);
		} else {
			if (gbk_apifam_win == gbo_apifam) {
				WriteRmFile(WriteMainRsrcObjPath);
			}
			WriteRmFile(WriteAppNamePath);
		}
	--DestFileIndent;
}

LOCALPROC WriteWantSigningFlag(void)
{
	WriteOpenDestFile("my_config_d",
		"codesign", ".txt", "code signing flag");

	WriteDestFileLn("1");

	WriteCloseDestFile();
}

LOCALPROC WriteMVCSpecificFiles(void)
{
	if (HaveMacBundleApp) {
		WritePListData();
	}

	if (WantSigning) {
		WriteWantSigningFlag();
	}

	if (WantSandbox) {
		WriteEntitlementsData();
	}

	WriteADstFile1("my_project_d",
		"Makefile", "", "Make file",
		WriteMVCMakeFile);
}
