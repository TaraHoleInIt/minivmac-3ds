/*
	USFILDEF.i
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
	USe program SPecific FILe DEFinitions
*/

LOCALPROC NullProc(void)
{
}

LOCALVAR unsigned int FileCounter;

struct DoSrcFile_r
{
	MyPtr SavepDt;
	char *s;
	long Flgm;
	int DepDir;
	tDoDependsForC depends;
	MyProc p;
};

typedef struct DoSrcFile_r DoSrcFile_r;

#define DoSrcFile_gd() ((DoSrcFile_r *)(pDt))

LOCALPROC DoASrcFileWithSetupProc0(
	char *s, int DepDir, long Flgm, tDoDependsForC depends)
{
	DoSrcFile_gd()->s = s;
	DoSrcFile_gd()->Flgm = Flgm;
	DoSrcFile_gd()->DepDir = DepDir;
	DoSrcFile_gd()->depends = depends;

	DoSrcFile_gd()->p();

	++FileCounter;
}

LOCALPROC DoASrcFileWithSetupProc(
	char *s, int DepDir, long Flgm, tDoDependsForC depends)
{
	if (0 == (Flgm & kCSrcFlgmNoSource))
	if (0 == (Flgm & kCSrcFlgmSkip))
	{
		DoASrcFileWithSetupProc0(s, DepDir, Flgm, depends);
	}
}

LOCALPROC DoAllSrcFilesWithSetup(MyProc p)
{
	DoSrcFile_r r;

	r.SavepDt = pDt;
	r.p = p;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllSrcFiles(DoASrcFileWithSetupProc);

	pDt = r.SavepDt;
}

LOCALPROC DoAllSrcFilesSortWithSetup1(
	char *s, int DepDir, long Flgm, tDoDependsForC depends)
{
	if (0 == (Flgm & kCSrcFlgmNoSource))
	if (0 == (Flgm & kCSrcFlgmSkip))
	{
		if (0 != (Flgm & kCSrcFlgmSortFirst)) {
			DoASrcFileWithSetupProc0(s, DepDir, Flgm, depends);
		} else {
			++FileCounter;
		}
	}
}

LOCALPROC DoAllSrcFilesSortWithSetup2(
	char *s, int DepDir, long Flgm, tDoDependsForC depends)
{
	if (0 == (Flgm & kCSrcFlgmNoSource))
	if (0 == (Flgm & kCSrcFlgmSkip))
	{
		if (0 == (Flgm & kCSrcFlgmSortFirst)) {
			DoASrcFileWithSetupProc0(s, DepDir, Flgm, depends);
		} else {
			++FileCounter;
		}
	}
}

LOCALPROC DoAllSrcFilesSortWithSetup(MyProc p)
{
	DoSrcFile_r r;

	r.SavepDt = pDt;
	r.p = p;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllSrcFiles(DoAllSrcFilesSortWithSetup1);
	FileCounter = 0;
	DoAllSrcFiles(DoAllSrcFilesSortWithSetup2);

	pDt = r.SavepDt;
}

LOCALFUNC char * GetSrcFileFileXtns(void)
{
	char *s;
	blnr UseObjc = ((DoSrcFile_gd()->Flgm & kCSrcFlgmOjbc) != 0);

	if (UseObjc) {
		s = ".m";
	} else {
		s = ".c";
	}

	return s;
}

LOCALPROC WriteSrcFileFileName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	WriteCStrToDestFile(GetSrcFileFileXtns());
}

LOCALPROC WriteSrcFileFilePath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile,
		WriteSrcFileFileName);
}

LOCALPROC WriteSrcFileHeaderName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	WriteCStrToDestFile(".h");
}

LOCALPROC WriteSrcFileHeaderPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile,
		WriteSrcFileHeaderName);
}

LOCALPROC WriteSrcFileObjName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	switch (cur_ide) {
		case gbk_ide_msv:
		case gbk_ide_dmc:
		case gbk_ide_plc:
			WriteCStrToDestFile(".obj");
			break;
		default:
			WriteCStrToDestFile(".o");
			break;
	}
}

LOCALPROC WriteSrcFileObjPath(void)
{
	WriteFileInDirToDestFile0(Write_obj_d_ToDestFile,
		WriteSrcFileObjName);
}

LOCALPROC DoAllExtraHeaders2WithSetupProc(
	char *s, int DepDir, long Flgm, tDoDependsForC depends)
{
	if (0 == (Flgm & kCSrcFlgmNoHeader))
	if (0 != (Flgm & kCSrcFlgmNoSource))
	if (0 == (Flgm & kCSrcFlgmSkip))
	{
		DoASrcFileWithSetupProc0(s, DepDir, Flgm, depends);
	}
}

LOCALPROC DoAllExtraHeaders2WithSetup(MyProc p)
{
	DoSrcFile_r r;

	r.SavepDt = pDt;
	r.p = p;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllSrcFiles(DoAllExtraHeaders2WithSetupProc);

	pDt = r.SavepDt;
}

LOCALPROC WriteExtraHeaderFileName(void)
{
	WriteCStrToDestFile(DoSrcFile_gd()->s);
	WriteCStrToDestFile(".h");
}

LOCALPROC WriteExtraHeaderFilePath(void)
{
	WriteFileInDirToDestFile0(
		((kDepDirCnfg == DoSrcFile_gd()->DepDir)
			? Write_cfg_d_ToDestFile
			: Write_src_d_ToDestFile),
		WriteExtraHeaderFileName);
}

LOCALVAR unsigned int DocTypeCounter;

struct DoDocType_r
{
	MyPtr SavepDt;
	char *ShortName;
	char *MacType;
	char *LongName;
	tWriteExtensionList WriteExtensionList;
	MyProc p;
};

typedef struct DoDocType_r DoDocType_r;

#define DoDocType_gd() ((DoDocType_r *)(pDt))

LOCALPROC DoAllDocTypesWithSetupProc(char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList)
{
	DoDocType_gd()->ShortName = ShortName;
	DoDocType_gd()->MacType = MacType;
	DoDocType_gd()->LongName = LongName;
	DoDocType_gd()->WriteExtensionList = WriteExtensionList;

	DoDocType_gd()->p();

	++DocTypeCounter;
}

LOCALPROC DoAppAndAllDocTypes0(tWriteOneDocType p)
{
	p("APP", "APPL", "Application", NULL);
	DoAllDocTypes(p);
}

LOCALPROC DoAppAndAllDocTypes(tWriteOneDocType p)
{
	p("APP", "APPL", "Application", NULL);
	if (WantIconMaster) {
		DoAllDocTypes(p);
	}
}

LOCALPROC DoAllDocTypesWithSetup(MyProc p)
{
	DoDocType_r r;

	r.SavepDt = pDt;
	r.p = p;
	pDt = (MyPtr)&r;

	DocTypeCounter = 0;
	DoAppAndAllDocTypes(DoAllDocTypesWithSetupProc);

	pDt = r.SavepDt;
}

LOCALPROC WriteDocTypeIconShortName(void)
{
	WriteCStrToDestFile(DoDocType_gd()->ShortName);
}

LOCALPROC WriteDocTypeIconFileName(void)
{
	WriteCStrToDestFile("ICON");
	WriteDocTypeIconShortName();
	switch (gbo_targfam) {
		case gbk_targfam_cmac:
			WriteCStrToDestFile("M.r");
			break;
		case gbk_targfam_mach:
		case gbk_targfam_carb:
			WriteCStrToDestFile("O.icns");
			break;
		case gbk_targfam_mswn:
		case gbk_targfam_wnce:
			WriteCStrToDestFile("W.ico");
			break;
	}
}

LOCALPROC WriteDocTypeIconFilePath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile,
		WriteDocTypeIconFileName);
}

LOCALPROC WriteDocTypeIconMacType(void)
{
	WriteCStrToDestFile(DoDocType_gd()->MacType);
}

LOCALPROC WriteDocTypeCopyMachoFile(void)
{
	WriteCopyFile(WriteDocTypeIconFilePath,
		Write_tmachores_d_ToDestFile);
}

typedef void (*tWriteOneFrameWorkType)(char *s);

static void DoAllFrameWorks(tWriteOneFrameWorkType p)
{
	if (gbk_apifam_cco == gbo_apifam) {
		p("AppKit");
		p("AudioUnit");
#if UseOpenGLinOSX
		p("OpenGL");
#endif
	} else {
		p("Carbon");
#if UseOpenGLinOSX
		p("OpenGL");
		p("AGL");
#endif
	}
}

struct DoFrameWork_r
{
	MyPtr SavepDt;
	char *s;
	MyProc p;
};

typedef struct DoFrameWork_r DoFrameWork_r;

#define DoFrameWork_gd() ((DoFrameWork_r *)(pDt))

LOCALPROC DoAllFrameWorksWithSetupProc(char *s)
{
	DoFrameWork_gd()->s = s;

	DoFrameWork_gd()->p();

	++FileCounter;
}

LOCALPROC DoAllFrameWorksWithSetup(MyProc p)
{
	DoFrameWork_r r;

	r.SavepDt = pDt;
	r.p = p;
	pDt = (MyPtr)&r;

	FileCounter = 0;
	DoAllFrameWorks(DoAllFrameWorksWithSetupProc);

	pDt = r.SavepDt;
}

LOCALPROC WriteFileToCFilesList(MyProc p)
{
	WriteBgnDestFileLn();
	p();
	WriteEndDestFileLn();
}

LOCALPROC DoSrcExtraHeaderFile(void)
{
	WriteFileToCFilesList(WriteExtraHeaderFilePath);
}

LOCALPROC DoSrcFileAddToList(void)
{
	WriteFileToCFilesList(WriteSrcFileHeaderPath);
	WriteFileToCFilesList(WriteSrcFileFilePath);
}

LOCALPROC WriteCFilesListContents(void)
{
	DoAllExtraHeaders2WithSetup(DoSrcExtraHeaderFile);
	DoAllSrcFilesWithSetup(DoSrcFileAddToList);
}

LOCALPROC WriteCFilesList(void)
{
	/* list of c files */

	WriteADstFile1("my_project_d",
		"c_files", "", "list of c files",
		WriteCFilesListContents);
}

LOCALPROC Write_tmachoShell(void)
{
	WriteRmDir(WriteAppNamePath);
	WriteRmDir(Write_tmachobun_d_ToDestFile);
	WriteMkDir(Write_tmachobun_d_ToDestFile);
	WriteMkDir(Write_tmachocontents_d_ToDestFile);
	WriteMkDir(Write_tmachomac_d_ToDestFile);
	WriteMkDir(Write_tmachores_d_ToDestFile);
	WriteMkDir(Write_tmacholang_d_ToDestFile);
	DoAllDocTypesWithSetup(WriteDocTypeCopyMachoFile);
	WriteCopyFile(WriteInfoPlistFilePath,
		Write_tmachocontents_d_ToDestFile);
	WriteEchoToNewFile(Write_tmachoLangDummyContents,
		Write_tmachoLangDummyPath, trueblnr);
	WriteEchoToNewFile(Write_tmachoPkgInfoContents,
		Write_tmachoPkgInfoPath, falseblnr);
	WriteMoveDir(Write_tmachobun_d_ToDestFile, WriteAppNamePath);
}

LOCALPROC Write_tmachoShellDeps(void)
{
	WriteMakeDependFile(Write_srcAppIconPath);
}

LOCALPROC WritepDtSrcPath(void)
{
	WriteFileInDirToDestFile0(Write_src_d_ToDestFile, WritepDtString);
}

LOCALPROC WritepDtCfgPath(void)
{
	WriteFileInDirToDestFile0(Write_cfg_d_ToDestFile, WritepDtString);
}

LOCALPROC DoSrcDependsMakeCompile(int DepDir, char *s)
{
	MyPtr SavepDt = pDt;
	pDt = (MyPtr)s;
	WriteMakeDependFile((kDepDirCnfg == DepDir)
		? WritepDtCfgPath
		: WritepDtSrcPath);
	pDt = SavepDt;
}

LOCALPROC DoSrcFileMakeCompileDeps(void)
{
	WriteMakeDependFile(WriteSrcFileFilePath);
	if (DoSrcFile_gd()->depends != nullpr) {
		DoSrcFile_gd()->depends(DoSrcDependsMakeCompile);
	}
	WriteMakeDependFile(WriteCNFGGLOBPath);
}

LOCALPROC DoSrcFileMakeCompileBody(void)
{
	WriteCompileC(WriteSrcFileFilePath, WriteSrcFileObjPath,
		(DoSrcFile_gd()->Flgm & kCSrcFlgmUseAPI) != 0);
}

LOCALPROC DoSrcFileMakeCompile(void)
{
	WriteMakeRule(WriteSrcFileObjPath,
		DoSrcFileMakeCompileDeps,
		DoSrcFileMakeCompileBody);
}

LOCALPROC DoSrcFileStandardMakeObjects(void)
{
	WriteBgnDestFileLn();
	WriteSrcFileObjPath();
	WriteSpaceToDestFile();
	WriteBackSlashToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC DoAllSrcFilesStandardMakeObjects(void)
{
	DoAllSrcFilesSortWithSetup(DoSrcFileStandardMakeObjects);
}

LOCALPROC DoSrcFileStandardEraseFile(void)
{
	WriteRmFile(WriteSrcFileObjPath);
}

LOCALPROC DoAllSrcFilesStandardErase(void)
{
	DoAllSrcFilesWithSetup(DoSrcFileStandardEraseFile);
}
