/*
	WRBGCFLS.i
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
	WRite Bash Gnu C specific FiLeS
*/


#define WriteXCDcallgcc WriteCompileCExec
#if 0
LOCALPROC WriteXCDcallgcc(void)
{
#if 0
	if ((gbk_ide_xcd == cur_ide) && (ide_vers >= 2100)
		&& (ide_vers < 2200))
	{
		/*
			This doesn't seem to make any difference in default
			Xcode 2.1 install. Current guess is that default
			headers don't support MACOSX_DEPLOYMENT_TARGET,
			but the ide is supporting 10.4 sdk that is
			included but not installed by default.
		*/
		WriteCStrToDestFile("export MACOSX_DEPLOYMENT_TARGET=10.");
		if (gbk_cpufam_ppc == gbo_cpufam) {
			WriteCStrToDestFile("1");
		} else {
			WriteCStrToDestFile("4");
		}
		WriteCStrToDestFile(" ; ");
	}
#endif
	WriteCStrToDestFile("gcc");
}
#endif

LOCALPROC WriteBgcCompileAsmLinkCommonOptions(void)
{
	if ((gbk_ide_xcd == cur_ide) && (ide_vers >= 2100)) {
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
	if (gbk_targfam_oind == gbo_targfam) {
		if (gbk_cpufam_x64 == gbo_cpufam) {
			WriteCStrToDestFile(" -m64");
		}
	} else if (gbk_targfam_lnds == gbo_targfam) {
		WriteCStrToDestFile(" -marm -mthumb-interwork");
	} else if (gbk_targfam_lctru == gbo_targfam) {
		WriteCStrToDestFile(" -mfloat-abi=hard -mtp=soft");
	}
}

LOCALPROC WriteBgcLinkOSGlucompileCommonOptions(void)
{
	if ((gbk_ide_xcd == cur_ide) && (ide_vers >= 2200)) {
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
}

LOCALPROC WriteBgcCompileLinkCommonOptions(void)
{
	if (gbk_dbg_on == gbo_dbg) {
		WriteCStrToDestFile(" -g");
	}
}

LOCALPROC WriteBgcCOptions(void)
{
	WriteCStrToDestFile(
		" -Wall -Wmissing-prototypes -Wno-uninitialized");

	if (gbk_apifam_nds != gbo_apifam) {
		WriteCStrToDestFile(" -Wundef -Wstrict-prototypes");
	}
	if (gbk_ide_cyg == cur_ide) {
		if (gbk_targfam_cygw != gbo_targfam) {
			WriteCStrToDestFile(" -mno-cygwin");
		}
	}
	if (gbk_ide_xcd == cur_ide) {
		WriteCStrToDestFile(" -fpascal-strings");
	}
	if (gbk_ide_xcd == cur_ide) {
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
	}
	WriteBgcCompileAsmLinkCommonOptions();
	WriteBgcCompileLinkCommonOptions();

	if (gbk_apifam_nds == gbo_apifam) {
		WriteCStrToDestFile(" -march=armv5te -mtune=arm946e-s");
	} else if (gbk_apifam_3ds == gbo_apifam) {
		WriteCStrToDestFile(" -march=armv6k -mtune=mpcore");
	}

	if (1 /* WantConfigDir */) {
		WriteCStrToDestFile(" -I");
		Write_cfg_d_ToDestFile();
		WriteCStrToDestFile(" -I");
		Write_src_d_ToDestFile();
	}
}

LOCALPROC WriteBgcCOptOptions(void)
{
	if (gbk_dbg_on != gbo_dbg) {
		/* WriteCStrToDestFile(" -O3"); */
		if (gbk_targfam_lnds == gbo_targfam) {
			WriteCStrToDestFile(" -O2");
		} else {
			WriteCStrToDestFile(" -Os");
		}
	} else {
		WriteCStrToDestFile(" -O0");
	}
}

LOCALPROC DoFrameWorkBGCaddFile(void)
{
	WriteCStrToDestFile(" -framework ");
	WriteCStrToDestFile(DoFrameWork_gd()->s);
}

LOCALPROC Write_machoRsrcBgcDeps(void)
{
	WriteMakeDependFile(WriteMainRsrcSrcPath);
	WriteMakeDependFile(Write_machoAppIconPath);
}

LOCALPROC Write_machoRsrcBgcBuild(void)
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

LOCALPROC WriteBashGccMakeFile(void)
{
	WriteDestFileLn("# make file generated by gryphel build system");

	WriteBlankLineToDestFile();

	if (gbo_apifam == gbk_apifam_3ds) {
		WriteDestFileLn("include $(DEVKITARM)/3ds_rules");
		WriteBlankLineToDestFile( );
	}

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptionsCommon = -c");
	WriteBgcCOptions();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptionsOSGLU = $(mk_COptionsCommon)");
	WriteBgcLinkOSGlucompileCommonOptions();
	WriteBgcCOptOptions();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("mk_COptions = $(mk_COptionsCommon)");
	WriteBgcCOptOptions();
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteDestFileLn(".PHONY: TheDefaultOutput clean");

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("TheDefaultOutput :");
	WriteMakeDependFile(Write_machobinpath_ToDestFile);
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	DoAllSrcFilesWithSetup(DoSrcFileMakeCompile);

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
			WriteCStrToDestFile("windres.exe");
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
	WriteCStrToDestFile(" : $(ObjFiles)");
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
		WriteXCDcallgcc();
		WriteCStrToDestFile(" \\");
		WriteEndDestFileLn();
		++DestFileIndent;
			WriteBgnDestFileLn();
			WriteCStrToDestFile("-o");
			if (gbk_apifam_nds == gbo_apifam || gbk_apifam_3ds == gbo_apifam) {
				WritePathArgInMakeCmnd(WriteBinElfObjObjPath);
			} else {
				WritePathArgInMakeCmnd(Write_machobinpath_ToDestFile);
			}
			WriteCStrToDestFile(" \\");
			WriteEndDestFileLn();

			WriteBgnDestFileLn();
			WriteCStrToDestFile("$(ObjFiles)");
			if ((gbk_apifam_osx == gbo_apifam)
				|| (gbk_apifam_cco == gbo_apifam))
			{
				DoAllFrameWorksWithSetup(DoFrameWorkBGCaddFile);
				if (ide_vers >= 4000) {
					WriteCStrToDestFile(" -Wl,-no_pie");
				}
			} else if (gbk_apifam_win == gbo_apifam) {
				WritePathArgInMakeCmnd(WriteMainRsrcObjPath);
				if (gbk_targ_wcar == cur_targ) {
					WriteCStrToDestFile(
						" -lcommctrl -lcoredll -laygshell -lmmtimer");
				} else {
					WriteCStrToDestFile(
						" -mwindows -lwinmm -lole32 -luuid");
					if (gbk_ide_cyg == cur_ide) {
						WriteCStrToDestFile(" -mno-cygwin");
					}
				}
			} else if (gbk_apifam_gtk == gbo_apifam) {
				WriteCStrToDestFile(" `pkg-config --libs gtk+-2.0`");
			} else if (gbk_apifam_sdl == gbo_apifam) {
				if (gbk_targfam_mach == gbo_targfam) {
					WriteCStrToDestFile(" -L/usr/local/lib -lSDLmain"
						" -lSDL -Wl,-framework,Cocoa");
				} else {
					WriteCStrToDestFile(" -lSDL");
				}
			} else if (gbk_apifam_sd2 == gbo_apifam) {
				if (gbk_targfam_mach == gbo_targfam) {
					WriteCStrToDestFile(
						" -Wl,-framework,Cocoa,-framework,SDL2");
				} else {
					WriteCStrToDestFile(" -lSDL2");
				}
			} else if (gbk_apifam_nds == gbo_apifam) {
				WriteCStrToDestFile(" -L$(DEVKITPRO)/libnds/lib");
				WriteCStrToDestFile(" -lfilesystem -lfat -lnds9");
			} else if (gbk_apifam_3ds == gbo_apifam) {
				WriteCStrToDestFile(" -L$(DEVKITPRO)/libctru/lib");
				WriteCStrToDestFile(" -specs=3dsx.specs -lcitro2d -lcitro3d -lctru -lm");
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
					WriteCStrToDestFile(" -L/usr/X11R7/lib");
					WriteCStrToDestFile(" -R/usr/X11R7/lib");
				} else if (gbk_targfam_dbsd == gbo_targfam) {
					WriteCStrToDestFile(" -L/usr/pkg/lib");
				} else if (gbk_targfam_minx == gbo_targfam) {
					WriteCStrToDestFile(" -L/usr/pkg/X11R6/lib");
				} else if (gbk_targfam_irix == gbo_targfam) {
					WriteCStrToDestFile(" -L/usr/lib/X11");
				} else {
					WriteCStrToDestFile(" -L/usr/X11R6/lib");
				}
				WriteCStrToDestFile(" -lX11");
			}
			if (gbk_apifam_nds == gbo_apifam) {
				WriteCStrToDestFile(" -specs=ds_arm9.specs");
			}
			WriteBgcCompileAsmLinkCommonOptions();
			WriteBgcLinkOSGlucompileCommonOptions();
			WriteBgcCompileLinkCommonOptions();
			WriteEndDestFileLn();
		--DestFileIndent;
		if (gbk_dbg_on != gbo_dbg) {
			switch (cur_ide) {
				case gbk_ide_bgc:
					if ((gbk_targfam_minx == gbo_targfam)
						|| (gbk_targfam_linx == gbo_targfam)
						|| (gbk_targfam_oind == gbo_targfam)
							/*
								for oi64, strip makes it larger!
								but still compresses smaller.
							*/
						|| (gbk_targfam_fbsd == gbo_targfam)
						|| (gbk_targfam_obsd == gbo_targfam)
						|| (gbk_targfam_nbsd == gbo_targfam)
						|| (gbk_targfam_dbsd == gbo_targfam)
						)
					{
						WriteBgnDestFileLn();
						WriteCStrToDestFile("strip --strip-unneeded");
						WritePathArgInMakeCmnd(
							Write_machobinpath_ToDestFile);
						WriteEndDestFileLn();
					} else if (gbk_targfam_irix == gbo_targfam) {
						WriteBgnDestFileLn();
						WriteCStrToDestFile("strip -s");
						WritePathArgInMakeCmnd(
							Write_machobinpath_ToDestFile);
						WriteEndDestFileLn();
					}
					break;
				case gbk_ide_xcd:
					WriteBgnDestFileLn();
					WriteCStrToDestFile("strip -u -r");
					WritePathArgInMakeCmnd(
						Write_machobinpath_ToDestFile);
					WriteEndDestFileLn();
					break;
				case gbk_ide_dvc:
				case gbk_ide_mgw:
				case gbk_ide_cyg:
					WriteBgnDestFileLn();
					WriteCStrToDestFile("strip.exe");
					WritePathArgInMakeCmnd(WriteAppNamePath);
					WriteEndDestFileLn();
					break;
				default:
					break;
			}
		}
		if (gbk_apifam_nds == gbo_apifam) {
			WriteBgnDestFileLn();
				WriteCStrToDestFile(
					"$(DEVKITARM)/bin/arm-eabi-objcopy.exe -O binary");
				WritePathArgInMakeCmnd(WriteBinElfObjObjPath);
				WritePathArgInMakeCmnd(WriteBinArmObjObjPath);
			WriteEndDestFileLn();
			WriteBgnDestFileLn();
				WriteCStrToDestFile("$(DEVKITARM)/bin/ndstool.exe -c");
				WritePathArgInMakeCmnd(WriteAppNamePath);
				WriteCStrToDestFile(" -9");
				WritePathArgInMakeCmnd(WriteBinArmObjObjPath);
				WriteCStrToDestFile(" -b $(DEVKITPRO)/libnds/icon.bmp");
				WriteCStrToDestFile(
					" \";www.devkitpro.org;www.drunkencoders.com\"");
			WriteEndDestFileLn();
		}
	--DestFileIndent;

	if (HaveMacRrscs) {
		WriteBlankLineToDestFile();
		WriteMakeRule(Write_machoRsrcPath,
			Write_machoRsrcBgcDeps,
			Write_machoRsrcBgcBuild);
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("clean :");
	++DestFileIndent;
		WriteDestFileLn("rm -f $(ObjFiles)");
		if (HaveMacBundleApp) {
			WriteRmDir(WriteAppNamePath);
		} else {
			if (gbk_apifam_win == gbo_apifam) {
				WriteRmFile(WriteMainRsrcObjPath);
			} else if (gbk_apifam_nds == gbo_apifam) {
				WriteRmFile(WriteBinElfObjObjPath);
				WriteRmFile(WriteBinArmObjObjPath);
			} else if (gbk_apifam_3ds == gbo_apifam) {
				WriteRmFile(WriteBinElfObjObjPath);
				WriteRmFile(WriteBinArmObjObjPath);
			}
			WriteRmFile(WriteAppNamePath);
		}
	--DestFileIndent;
}

LOCALPROC WriteBashGccSpecificFiles(void)
{
	if (HaveMacBundleApp) {
		WritePListData();
	}

	if (WantSandbox) {
		WriteEntitlementsData();
	}

	WriteADstFile1("my_project_d",
		"Makefile", "", "Make file",
		WriteBashGccMakeFile);
}
