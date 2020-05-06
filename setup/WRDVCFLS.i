/*
	WRDVCFLS.i
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
	WRite bloodshed DeV-C++ specific FiLeS
*/


static void DoSrcFileDvcAddFile(void)
{
	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("[Unit");
	WriteUnsignedToOutput(FileCounter + 1);
	WriteCStrToDestFile("]");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("FileName=");
	WriteSrcFileFilePath();
	WriteEndDestFileLn();

	WriteDestFileLn("CompileCpp=0");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Folder=");
	WriteAppVariationStr();
	WriteEndDestFileLn();
	WriteDestFileLn("Compile=1");
	WriteDestFileLn("Link=1");
	WriteDestFileLn("Priority=1000");
	WriteDestFileLn("OverrideBuildCmd=0");
	WriteDestFileLn("BuildCmd=");
}

LOCALPROC WriteDevCProjectFile(void)
{
	WriteDestFileLn("[Project]");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("FileName=");
	WriteStrAppAbbrev();
	WriteCStrToDestFile(".dev");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Name=");
	WriteAppVariationStr();
	WriteEndDestFileLn();

	DoAllSrcFilesWithSetup(NullProc);
	++FileCounter; /* main.rc */

	WriteBgnDestFileLn();
	WriteCStrToDestFile("UnitCount=");
	WriteUnsignedToOutput(FileCounter);
	WriteEndDestFileLn();

	WriteDestFileLn("Type=0");
	WriteDestFileLn("Ver=1");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("Includes=");
	Write_cfg_d_ToDestFile();
	WriteCStrToDestFile(" ");
	Write_src_d_ToDestFile();
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("PrivateResource=");
	WriteAppVariationStr();
	WriteCStrToDestFile("_private.rc");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("ResourceIncludes=");
	Write_src_d_ToDestFile();
	WriteEndDestFileLn();

	WriteDestFileLn("IsCpp=0");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("ObjectOutput=");
	Write_obj_d_ToDestFile();
	WriteEndDestFileLn();
	WriteDestFileLn("OverrideOutput=1");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("OverrideOutputName=");
	WriteAppNamePath();
	WriteEndDestFileLn();

	WriteDestFileLn("IncludeVersionInfo=0");
	WriteDestFileLn("CompilerSet=0");
	if (gbk_dbg_on == gbo_dbg) {
		WriteDestFileLn("CompilerSettings=000000000000000100");
		WriteDestFileLn(
			"Compiler= -Wall -Wstrict-prototypes"
			" -Wno-uninitialized -O0_@@_");
	} else {
		WriteDestFileLn("CompilerSettings=000000000000000000");
		WriteDestFileLn(
			"Compiler= -Wall -Wstrict-prototypes"
			" -Wno-uninitialized -Os_@@_");
	}
	WriteDestFileLn("Linker=-lwinmm -lole32 -luuid_@@_");
	WriteBlankLineToDestFile();
	WriteDestFileLn("[VersionInfo]");
	WriteDestFileLn("Major=0");
	WriteDestFileLn("Minor=1");
	WriteDestFileLn("Release=1");
	WriteDestFileLn("Build=1");
	WriteDestFileLn("LanguageID=1033");
	WriteDestFileLn("CharsetID=1252");
	WriteDestFileLn("CompanyName=");
	WriteDestFileLn("FileVersion=");
	WriteDestFileLn("FileDescription=Developed using the Dev-C++ IDE");
	WriteDestFileLn("InternalName=");
	WriteDestFileLn("LegalCopyright=");
	WriteDestFileLn("LegalTrademarks=");
	WriteDestFileLn("OriginalFilename=");
	WriteDestFileLn("ProductName=");
	WriteDestFileLn("ProductVersion=");
	WriteDestFileLn("AutoIncBuildNr=0");

	DoAllSrcFilesWithSetup(DoSrcFileDvcAddFile);

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("[Unit");
	WriteUnsignedToOutput(++FileCounter);
	WriteCStrToDestFile("]");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("FileName=");
	WriteMainRsrcSrcPath();
	WriteEndDestFileLn();

	WriteDestFileLn("CompileCpp=0");
	WriteBgnDestFileLn();
	WriteCStrToDestFile("Folder=");
	WriteAppVariationStr();
	WriteEndDestFileLn();
	WriteDestFileLn("Compile=1");
	WriteDestFileLn("Link=0");
	WriteDestFileLn("Priority=1000");
	WriteDestFileLn("OverrideBuildCmd=0");
	WriteDestFileLn("BuildCmd=");

	WriteBlankLineToDestFile();
}

LOCALPROC WriteDevCSpecificFiles(void)
{
	WriteADstFile1("my_project_d",
		vStrAppAbbrev, ".dev", "Project file",
		WriteDevCProjectFile);
}
