/*
	WRXCDFLS.i
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
	WRite XCoDe specific FiLeS

	(also support older Apple Project Builder)
*/

LOCALPROC WriteNextLineSameDent(void)
{
	WriteEndDestFileLn();
	WriteBgnDestFileLn();
}

static void WriteAPBXCDObjectId(unsigned int theClass, unsigned int v)
{
	WriteHexWordToOutput(theClass);
	WriteHexWordToOutput(v);
	WriteCStrToDestFile("0000000000000000");
}

enum {
	APBoclsSrcBld,
	APBoclsIcnsBld,
	APBoclsFramBld,
	APBospcLibStdcBld,
	APBospcMnRsrcBld, /* only if HaveMacRrscs */
	APBospcLangDummyBld,

	APBospcBuildStyle,

	APBoclsSrcRf,
	APBoclsHdr, /* only if gbk_ide_xcd == cur_ide */
	APBoclsInc, /* only if gbk_ide_xcd == cur_ide */
	APBoclsIcnsRf,
	APBoclsFramRf,
	APBospcLibStdcRf,
	APBospcProductRef,
	APBospcPlistRf,
	APBospcMainRsrcRf, /* only if HaveMacRrscs */
	APBospcLangRf,

	APBospcPhaseLibs,
	APBospcSources,
	APBospcResources, /* only if HaveMacBundleApp */
	APBospcLibraries,
	APBospcProducts,
	APBospcMainGroup,
	APBospcSrcHeaders, /* only if gbk_ide_xcd == cur_ide */
	APBospcIncludes, /* only if gbk_ide_xcd == cur_ide */

	APBospcTarget,
	APBospcRoot,
	APBospcBunRsrcs, /* only if HaveMacBundleApp */
	APBospcPhaseRsrc, /* only if HaveMacRrscs */
	APBospcHeaders,
	APBospcPhaseSrcs,

	APBospcLangDummyRf,

	APBospcNatCnfg, /* only if gbk_ide_xcd == cur_ide */
	APBospcPrjCnfg, /* only if gbk_ide_xcd == cur_ide */
	APBospcLstNatCnfg, /* only if gbk_ide_xcd == cur_ide */
	APBospcLstPrjCnfg, /* only if gbk_ide_xcd == cur_ide */

	kNumAPBocls
};

#define HaveAPBXCD_LangDummy (ide_vers >= 1000)
#define HaveAPBXCD_PlistFile (ide_vers >= 1000)
#define HaveAPBXCD_NameCmmnt (ide_vers >= 2100)
#define HaveAPBXCD_Headers (ide_vers >= 1000)
#define HaveAPBXCD_StdcLib (ide_vers < 1500)
#define HaveAPBXCD_IsaFirst (ide_vers >= 2100)

static void WriteAPBXCDBgnObjList(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" = (");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAPBXCDEndObjList(void)
{
	--DestFileIndent;
	WriteDestFileLn(");");
}

LOCALPROC WriteAPBXCDObjectIdAndComment(unsigned int theClass,
	unsigned int v, MyProc comment)
{
	WriteAPBXCDObjectId(theClass, v);
	if (HaveAPBXCD_NameCmmnt) {
		WriteCStrToDestFile(" /* ");
		comment();
		WriteCStrToDestFile(" */");
	}
}

static void WriteAPBXCDBeginObject(unsigned int theClass,
	unsigned int v, MyProc comment)
{
	WriteBgnDestFileLn();
	WriteAPBXCDObjectIdAndComment(theClass, v, comment);
	WriteCStrToDestFile(" = {");
	WriteEndDestFileLn();
	++DestFileIndent;
}

static void WriteAPBXCDEndObject(void)
{
	--DestFileIndent;
	WriteDestFileLn("};");
}

static void WriteAPBQuotedField(char *s, char *v)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(" = ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile(v);
	WriteQuoteToDestFile();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();
}

LOCALPROC WriteAPBXCDobjlistelmp(unsigned int theClass, unsigned int v,
	MyProc comment)
{
	WriteBgnDestFileLn();
	WriteAPBXCDObjectIdAndComment(theClass, v, comment);
	WriteCStrToDestFile(",");
	WriteEndDestFileLn();
}

LOCALVAR int APBXCDForceSameLine = 0;

LOCALPROC WriteAPBXCDSepA(void)
{
	if (0 == APBXCDForceSameLine) {
		WriteNextLineSameDent();
	} else {
		WriteSpaceToDestFile();
	}
}

LOCALPROC WriteAPBXCDDObjectAPropBgn(void)
{
	if (0 == APBXCDForceSameLine) {
		WriteBgnDestFileLn();
	}
}

LOCALPROC WriteAPBXCDDObjectAPropEnd(void)
{
	WriteCStrToDestFile(";");
	if (0 == APBXCDForceSameLine) {
		WriteEndDestFileLn();
	} else {
		WriteSpaceToDestFile();
	}
}

LOCALPROC WriteAPBXCDObjectAp(unsigned int theClass, unsigned int v,
	MyProc comment, MyProc body)
{
	WriteBgnDestFileLn();
	WriteAPBXCDObjectIdAndComment(theClass, v, comment);
	WriteCStrToDestFile(" = {");
	if (ide_vers < 2100) {
		WriteEndDestFileLn();
		++DestFileIndent;
			body();
		--DestFileIndent;
		WriteBgnDestFileLn();
	} else {
		++APBXCDForceSameLine;
		body();
		--APBXCDForceSameLine;
	}
	WriteCStrToDestFile("};");
	WriteEndDestFileLn();
}

LOCALPROC WriteAPBXCDDObjAProp_SS(char *ns, char *vs)
{
	WriteAPBXCDDObjectAPropBgn();
	WriteCStrToDestFile(ns);
	WriteCStrToDestFile(" = ");
	WriteCStrToDestFile(vs);
	WriteAPBXCDDObjectAPropEnd();
}

LOCALPROC WriteAPBXCDDObjAProp_SP(char *ns, MyProc p)
{
	WriteAPBXCDDObjectAPropBgn();
	WriteCStrToDestFile(ns);
	WriteCStrToDestFile(" = ");
	p();
	WriteAPBXCDDObjectAPropEnd();
}

LOCALPROC WriteAPBXCDDObjAProp_SO(char *ns,
	unsigned int theClass, unsigned int v,
	MyProc comment)
{
	WriteAPBXCDDObjectAPropBgn();
	WriteCStrToDestFile(ns);
	WriteCStrToDestFile(" = ");
	WriteAPBXCDObjectIdAndComment(theClass,
		v, comment);
	WriteAPBXCDDObjectAPropEnd();
}

LOCALPROC WriteAPBXCDDObjAPropIsa(char *s)
{
	WriteAPBXCDDObjAProp_SS("isa", s);
}

LOCALPROC WriteAPBXCDDObjAPropIsaBuildFile(void)
{
	WriteAPBXCDDObjAPropIsa("PBXBuildFile");
}

LOCALPROC WriteAPBXCDDObjAPropIsaFileReference(void)
{
	WriteAPBXCDDObjAPropIsa("PBXFileReference");
}

LOCALPROC WriteAPBXCDDObjAPropIsaGroup(void)
{
	WriteAPBXCDDObjAPropIsa("PBXGroup");
}

LOCALPROC WriteAPBXCDDObjAPropFileEncoding30(void)
{
	WriteAPBXCDDObjAProp_SS("fileEncoding", "30");
}

LOCALPROC WriteAPBXCDDObjAPropFileEncoding4(void)
{
	WriteAPBXCDDObjAProp_SS("fileEncoding", "4");
}

LOCALPROC WriteAPBXCDDObjAPropRefType(char *ns)
{
	if (ide_vers < 2100) {
		WriteAPBXCDDObjAProp_SS("refType", ns);
	}
}

LOCALPROC WriteAPBXCDDObjAPropRefType0(void)
{
	WriteAPBXCDDObjAPropRefType("0");
}

LOCALPROC WriteAPBXCDDObjAPropRefType2(void)
{
	WriteAPBXCDDObjAPropRefType("2");
}

LOCALPROC WriteAPBXCDDObjAPropRefType3(void)
{
	WriteAPBXCDDObjAPropRefType("3");
}

LOCALPROC WriteAPBXCDDObjAPropRefType4(void)
{
	WriteAPBXCDDObjAPropRefType("4");
}

LOCALPROC WriteAPBXCDDObjAPropName(MyProc p)
{
	WriteAPBXCDDObjAProp_SP("name", p);
}

LOCALPROC WriteAPBXCDDObjAPropPath(MyProc p)
{
	WriteAPBXCDDObjAProp_SP("path", p);
}

LOCALPROC WriteAPBXCDDObjAPropSourceTree(char *s)
{
	if (ide_vers >= 1000) {
		WriteAPBXCDDObjAProp_SS("sourceTree", s);
	}
}

LOCALPROC WriteAPBXCDDObjAPropSourceTreeRoot(void)
{
	WriteAPBXCDDObjAPropSourceTree("SOURCE_ROOT");
}

LOCALPROC WriteAPBXCDDObjAPropSourceTreeSDKRoot(void)
{
	WriteAPBXCDDObjAPropSourceTree("SDKROOT");
}

LOCALPROC WriteAPBXCDDObjAPropSourceTreeAbsolute(void)
{
	WriteAPBXCDDObjAPropSourceTree("\"<absolute>\"");
}

LOCALPROC WriteAPBXCDDObjAPropSourceTreeGroup(void)
{
	WriteAPBXCDDObjAPropSourceTree("\"<group>\"");
}

LOCALPROC WriteAPBXCDDObjAPropExpectedFileType(MyProc p)
{
	if ((ide_vers < 1500) && (ide_vers >= 1000)) {
		WriteAPBXCDDObjAProp_SP("expectedFileType", p);
	}
}

LOCALPROC WriteAPBXCDDObjAPropLastKnownFType(MyProc p)
{
	if (ide_vers >= 1500) {
		WriteAPBXCDDObjAProp_SP("lastKnownFileType", p);
	}
}

LOCALPROC WriteAPBXCDDObjAPropFileRef(
	unsigned int theClass, unsigned int v,
	MyProc comment)
{
	WriteAPBXCDDObjAProp_SO("fileRef",
		theClass, v, comment);
}

LOCALPROC WriteAPBXCDDObjAPropIncludeII0(void)
{
	if (ide_vers >= 1000) {
		WriteAPBXCDDObjAProp_SS("includeInIndex", "0");
	}
}

LOCALPROC WriteAPBXCDDObjAPropSettingsNull(void)
{
	if (ide_vers < 2100) {
		WriteAPBXCDDObjectAPropBgn();
		WriteCStrToDestFile("settings = {");
		WriteAPBXCDSepA();
		WriteCStrToDestFile("}");
		WriteAPBXCDDObjectAPropEnd();
	}
}

LOCALPROC WriteSrcFileAPBXCDNameInSources(void)
{
	WriteSrcFileFileName();
	WriteCStrToDestFile(" in Sources");
}

LOCALPROC DoSrcFileAPBXCDaddFileBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}

	WriteAPBXCDDObjAPropFileRef(APBoclsSrcRf,
		FileCounter, WriteSrcFileFileName);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}
	WriteAPBXCDDObjAPropSettingsNull();
}

LOCALPROC DoSrcFileAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsSrcBld, FileCounter,
		WriteSrcFileAPBXCDNameInSources, DoSrcFileAPBXCDaddFileBody);
}

LOCALPROC WriteSrcFileAPBXCDtype(void)
{
	char *s;
	blnr UseObjc = ((DoSrcFile_gd()->Flgm & kCSrcFlgmOjbc) != 0);

	if (UseObjc) {
		s = "sourcecode.c.objc";
	} else {
		s = "sourcecode.c.c";
	}
	WriteCStrToDestFile(s);
}

LOCALPROC DoSrcFileAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteSrcFileAPBXCDtype);
	WriteAPBXCDDObjAPropFileEncoding30();
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteSrcFileAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteSrcFileFileName);
	WriteAPBXCDDObjAPropPath(WriteSrcFileFilePath);
	WriteAPBXCDDObjAPropRefType2();
	WriteAPBXCDDObjAPropSourceTreeRoot();
}

LOCALPROC DoSrcFileAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsSrcRf, FileCounter,
		WriteSrcFileFileName,
		DoSrcFileAPBXCDaddFileRefBody);
}

LOCALPROC DoSrcFileAPBXCDaddToGroup(void)
{
	WriteAPBXCDobjlistelmp(APBoclsSrcRf, FileCounter,
		WriteSrcFileFileName);
}

LOCALPROC DoSrcFileAPBXCDaddToSources(void)
{
	WriteAPBXCDobjlistelmp(APBoclsSrcBld, FileCounter,
		WriteSrcFileAPBXCDNameInSources);
}

LOCALPROC WriteHeaderFileAPBXCDtype(void)
{
	WriteCStrToDestFile("sourcecode.c.h");
}

LOCALPROC DoHeaderFileXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteHeaderFileAPBXCDtype);
	WriteAPBXCDDObjAPropFileEncoding30();
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteHeaderFileAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteSrcFileHeaderName);
	WriteAPBXCDDObjAPropPath(WriteSrcFileHeaderPath);
	WriteAPBXCDDObjAPropRefType2();
	WriteAPBXCDDObjAPropSourceTreeRoot();
}

LOCALPROC DoHeaderFileXCDaddFileRef(void)
{
	if (0 == (DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader)) {
		WriteAPBXCDObjectAp(APBoclsHdr, FileCounter,
			WriteSrcFileHeaderName,
			DoHeaderFileXCDaddFileRefBody);
	}
}

LOCALPROC DoHeaderFileXCDaddToGroup(void)
{
	if (0 == (DoSrcFile_gd()->Flgm & kCSrcFlgmNoHeader)) {
		WriteAPBXCDobjlistelmp(APBoclsHdr, FileCounter,
			WriteSrcFileHeaderName);
	}
}

LOCALPROC WriteDocTypeAPBXCDIconFileInResources(void)
{
	WriteDocTypeIconFileName();
	WriteCStrToDestFile(" in Resources");
}

LOCALPROC DoDocTypeAPBXCDaddFileBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}

	WriteAPBXCDDObjAPropFileRef(APBoclsIcnsRf,
		DocTypeCounter, WriteDocTypeIconFileName);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}
	WriteAPBXCDDObjAPropSettingsNull();
}

LOCALPROC DoDocTypeAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsIcnsBld, DocTypeCounter,
		WriteDocTypeAPBXCDIconFileInResources,
		DoDocTypeAPBXCDaddFileBody);
}

LOCALPROC WriteDocTypeAPBXCDtype(void)
{
	WriteCStrToDestFile("image.icns");
}

LOCALPROC DoDocTypeAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteDocTypeAPBXCDtype);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteDocTypeAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteDocTypeIconFileName);
	WriteAPBXCDDObjAPropPath(WriteDocTypeIconFilePath);
	WriteAPBXCDDObjAPropRefType2();
	WriteAPBXCDDObjAPropSourceTreeRoot();
}

LOCALPROC DoDocTypeAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsIcnsRf, DocTypeCounter,
		WriteDocTypeIconFileName, DoDocTypeAPBXCDaddFileRefBody);
}

LOCALPROC DoDocTypeAPBXCDaddToGroup(void)
{
	WriteAPBXCDobjlistelmp(APBoclsIcnsRf, DocTypeCounter,
		WriteDocTypeIconFileName);
}

LOCALPROC DoDocTypeAPBXCDaddToSources(void)
{
	WriteAPBXCDobjlistelmp(APBoclsIcnsBld, DocTypeCounter,
		WriteDocTypeAPBXCDIconFileInResources);
}

LOCALPROC WriteFrameWorkAPBXCDFileName(void)
{
	WriteCStrToDestFile(DoFrameWork_gd()->s);
	WriteCStrToDestFile(".framework");
}

LOCALPROC WriteFrameWorkAPBXCDFilePath(void)
{
	if (ide_vers < 4000) {
		WriteCStrToDestFile("/");
	}
	WriteCStrToDestFile("System/Library/Frameworks/");
	WriteFrameWorkAPBXCDFileName();
}

LOCALPROC WriteFrameWorkAPBXCDileInFrameworks(void)
{
	WriteFrameWorkAPBXCDFileName();
	WriteCStrToDestFile(" in Frameworks");
}

LOCALPROC DoFrameWorkAPBXCDaddFileBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}

	WriteAPBXCDDObjAPropFileRef(APBoclsFramRf,
		FileCounter, WriteFrameWorkAPBXCDFileName);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}
	WriteAPBXCDDObjAPropSettingsNull();
}

LOCALPROC DoFrameWorkAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBoclsFramBld, FileCounter,
		WriteFrameWorkAPBXCDileInFrameworks,
		DoFrameWorkAPBXCDaddFileBody);
}

LOCALPROC WriteAPBXCDDObjAPropIsaFrameworkRef(void)
{
	if (ide_vers < 1000) {
		WriteAPBXCDDObjAPropIsa("PBXFrameworkReference");
	} else {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
}

LOCALPROC WriteFrameWorkAPBXCDtype(void)
{
	WriteCStrToDestFile("wrapper.framework");
}

LOCALPROC DoFrameWorkAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFrameworkRef();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteFrameWorkAPBXCDtype);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFrameworkRef();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteFrameWorkAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteFrameWorkAPBXCDFileName);
	WriteAPBXCDDObjAPropPath(WriteFrameWorkAPBXCDFilePath);
	WriteAPBXCDDObjAPropRefType0();
	if (ide_vers >= 4000) {
		WriteAPBXCDDObjAPropSourceTreeSDKRoot();
	} else {
		WriteAPBXCDDObjAPropSourceTreeAbsolute();
	}
}

LOCALPROC DoFrameWorkAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsFramRf, FileCounter,
		WriteFrameWorkAPBXCDFileName,
		DoFrameWorkAPBXCDaddFileRefBody);
}

LOCALPROC DoFrameworkAPBXCDaddToBuild(void)
{
	WriteAPBXCDobjlistelmp(APBoclsFramBld, FileCounter,
		WriteFrameWorkAPBXCDileInFrameworks);
}

LOCALPROC DoFrameworkAPBXCDaddToLibraries(void)
{
	WriteAPBXCDobjlistelmp(APBoclsFramRf, FileCounter,
		WriteFrameWorkAPBXCDFileName);
}

LOCALPROC DoExtraHeaderFileXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteHeaderFileAPBXCDtype);
	WriteAPBXCDDObjAPropFileEncoding30();
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteHeaderFileAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteExtraHeaderFileName);
	WriteAPBXCDDObjAPropPath(WriteExtraHeaderFilePath);
	WriteAPBXCDDObjAPropRefType2();
	WriteAPBXCDDObjAPropSourceTreeRoot();
}

LOCALPROC DoExtraHeaderFileXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBoclsInc, FileCounter,
		WriteExtraHeaderFileName,
		DoExtraHeaderFileXCDaddFileRefBody);
}

LOCALPROC DoExtraHeaderFileXCDaddToGroup(void)
{
	WriteAPBXCDobjlistelmp(APBoclsInc, FileCounter,
		WriteExtraHeaderFileName);
}

LOCALPROC WriteMainAPBXCDRsrcNameinRez(void)
{
	WriteMainRsrcName();
	WriteCStrToDestFile(" in Rez");
}

LOCALPROC DoRsrcAPBXCDaddFileBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}

	WriteAPBXCDDObjAPropFileRef(APBospcMainRsrcRf, 0,
		WriteMainRsrcName);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}
	WriteAPBXCDDObjAPropSettingsNull();
}

LOCALPROC DoRsrcAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBospcMnRsrcBld, 0,
		WriteMainAPBXCDRsrcNameinRez,
		DoRsrcAPBXCDaddFileBody);
}

LOCALPROC WriteRsrcAPBXCDtype(void)
{
	WriteCStrToDestFile("sourcecode.rez");
}

LOCALPROC DoRsrcAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteRsrcAPBXCDtype);
	WriteAPBXCDDObjAPropFileEncoding30();
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteRsrcAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteMainRsrcName);
	WriteAPBXCDDObjAPropPath(WriteMainRsrcSrcPath);
	WriteAPBXCDDObjAPropRefType2();
	WriteAPBXCDDObjAPropSourceTreeRoot();
}

LOCALPROC DoRsrcAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBospcMainRsrcRf, 0,
		WriteMainRsrcName,
		DoRsrcAPBXCDaddFileRefBody);
}

LOCALPROC WriteLibStdcName(void)
{
	WriteCStrToDestFile("libstdc++.a");
}

LOCALPROC DoLibStdcAPBXCDaddFileBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}

	WriteAPBXCDDObjAPropFileRef(APBospcLibStdcRf, 0,
		WriteLibStdcName);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}
	WriteAPBXCDDObjAPropSettingsNull();
}

LOCALPROC DoLibStdcAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBospcLibStdcBld, 0,
		WriteLibStdcName,
		DoLibStdcAPBXCDaddFileBody);
}

LOCALPROC WriteLibStdcFileName(void)
{
	WriteQuoteToDestFile();
	WriteCStrToDestFile("libstdc++.a");
	WriteQuoteToDestFile();
}

LOCALPROC WriteLibStdcFilePath(void)
{
	WriteQuoteToDestFile();
	WriteCStrToDestFile("/usr/lib/libstdc++.a");
	WriteQuoteToDestFile();
}

LOCALPROC WriteLibStdcAPBXCDtype(void)
{
	WriteCStrToDestFile("archive.ar");
}

LOCALPROC DoLibStdcAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteLibStdcAPBXCDtype);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteLibStdcAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteLibStdcFileName);
	WriteAPBXCDDObjAPropPath(WriteLibStdcFilePath);
	WriteAPBXCDDObjAPropRefType0();
	WriteAPBXCDDObjAPropSourceTreeAbsolute();
}

LOCALPROC DoLibStdcAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBospcLibStdcRf, 0,
		WriteLibStdcName,
		DoLibStdcAPBXCDaddFileRefBody);
}

LOCALPROC WriteDummyLangFileNameInResources(void)
{
	WriteDummyLangFileName();
	WriteCStrToDestFile(" in Resources");
}

LOCALPROC DoDummyLangAPBXCDaddFileBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}

	WriteAPBXCDDObjAPropFileRef(APBospcLangDummyRf, 0,
		WriteDummyLangFileName);
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaBuildFile();
	}
	WriteAPBXCDDObjAPropSettingsNull();
}

LOCALPROC DoDummyLangAPBXCDaddFile(void)
{
	WriteAPBXCDObjectAp(APBospcLangDummyBld, 0,
		WriteDummyLangFileNameInResources,
		DoDummyLangAPBXCDaddFileBody);
}

LOCALPROC WriteDummyLangFilePath(void)
{
	WriteFileInDirToDestFile0(WriteLProjFolderPath,
		WriteDummyLangFileName);
}

LOCALPROC WriteLangDummyAPBXCDtype(void)
{
	WriteCStrToDestFile("text");
}

LOCALPROC DoLangDummyAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteLangDummyAPBXCDtype);
	WriteAPBXCDDObjAPropFileEncoding30();
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteLangDummyAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteLProjName);
	WriteAPBXCDDObjAPropPath(WriteDummyLangFilePath);
	WriteAPBXCDDObjAPropRefType4();
	WriteAPBXCDDObjAPropSourceTreeRoot();
}

LOCALPROC DoLangDummyAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBospcLangRf, 0,
		WriteLProjName,
		DoLangDummyAPBXCDaddFileRefBody);
}

LOCALPROC DoLangDummyAPBXCDaddToSources(void)
{
	WriteAPBXCDobjlistelmp(APBospcLangDummyBld, 0,
		WriteDummyLangFileNameInResources);
}

LOCALPROC DoLangDummyAPBXCDaddVariant(void)
{
	WriteAPBXCDBeginObject(APBospcLangDummyRf,
		0, WriteDummyLangFileName);

		if (HaveAPBXCD_IsaFirst) {
			WriteAPBXCDDObjAPropIsa("PBXVariantGroup");
		}

		WriteAPBXCDBgnObjList("children");
			WriteAPBXCDobjlistelmp(APBospcLangRf, 0, WriteLProjName);
		WriteAPBXCDEndObjList();
		if (! HaveAPBXCD_IsaFirst) {
			WriteAPBXCDDObjAPropIsa("PBXVariantGroup");
		}
		WriteAPBXCDDObjAPropName(WriteDummyLangFileName);
		WriteAPBXCDDObjAPropRefType4();
		WriteAPBXCDDObjAPropSourceTreeGroup();
	WriteAPBXCDEndObject();
}

static void DoBeginSectionAPBXCD(char *Name)
{
	if (ide_vers >= 2100) {
		--DestFileIndent; --DestFileIndent;
		WriteBlankLineToDestFile();
		WriteBgnDestFileLn();
		WriteCStrToDestFile("/* Begin ");
		WriteCStrToDestFile(Name);
		WriteCStrToDestFile(" section */");
		WriteEndDestFileLn();
		++DestFileIndent; ++DestFileIndent;
	}
}

static void DoEndSectionAPBXCD(char *Name)
{
	if (ide_vers >= 2100) {
		--DestFileIndent; --DestFileIndent;
		WriteBgnDestFileLn();
		WriteCStrToDestFile("/* End ");
		WriteCStrToDestFile(Name);
		WriteCStrToDestFile(" section */");
		WriteEndDestFileLn();
		++DestFileIndent; ++DestFileIndent;
	}
}

LOCALPROC WriteXCDconfigname(void)
{
	char *s;

	switch (gbo_dbg) {
		case gbk_dbg_on:
			if (ide_vers < 2100) {
				s = "Development";
			} else {
				s = "Debug";
			}
			break;
		case gbk_dbg_test:
			s = "Test";
			break;
		case gbk_dbg_off:
			if (ide_vers < 2100) {
				s = "Deployment";
			} else {
				s = "Release";
			}
			break;
		default:
			s = "(unknown Debug Level)";
			break;
	}

	WriteCStrToDestFile(s);
}

LOCALPROC WriteAPBXCDDObjAPropIsaApplicationRef(void)
{
	if (ide_vers < 1000) {
		WriteAPBXCDDObjAPropIsa("PBXApplicationReference");
	} else {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
}

LOCALPROC WriteProductAPBXCDtype(void)
{
	if (HaveMacBundleApp) {
		WriteCStrToDestFile("wrapper.application");
	} else {
		WriteCStrToDestFile("\"compiled.mach-o.executable\"");
	}
}

LOCALPROC DoProductAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaApplicationRef();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WriteProductAPBXCDtype);
	WriteAPBXCDDObjAPropIncludeII0();
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaApplicationRef();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WriteProductAPBXCDtype);
	WriteAPBXCDDObjAPropPath(WriteAppNameStr);
	WriteAPBXCDDObjAPropRefType3();
	WriteAPBXCDDObjAPropSourceTree("BUILT_PRODUCTS_DIR");
}

LOCALPROC DoProductAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBospcProductRef, 0,
		WriteAppNameStr,
		DoProductAPBXCDaddFileRefBody);
}

LOCALPROC WritePlistAPBXCDtype(void)
{
	if (ide_vers >= 3100) {
		WriteCStrToDestFile("text.plist.xml");
	} else {
		WriteCStrToDestFile("text.xml");
	}
}

LOCALPROC DoPlistAPBXCDaddFileRefBody(void)
{
	if (HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}

	WriteAPBXCDDObjAPropExpectedFileType(WritePlistAPBXCDtype);
	WriteAPBXCDDObjAPropFileEncoding4();
	if (! HaveAPBXCD_IsaFirst) {
		WriteAPBXCDDObjAPropIsaFileReference();
	}
	WriteAPBXCDDObjAPropLastKnownFType(WritePlistAPBXCDtype);
	WriteAPBXCDDObjAPropName(WriteInfoPlistFileName);
	WriteAPBXCDDObjAPropPath(WriteInfoPlistFilePath);
	WriteAPBXCDDObjAPropRefType2();
	WriteAPBXCDDObjAPropSourceTreeRoot();
}

LOCALPROC DoPlistAPBXCDaddFileRef(void)
{
	WriteAPBXCDObjectAp(APBospcPlistRf, 0,
		WriteInfoPlistFileName,
		DoPlistAPBXCDaddFileRefBody);
}

LOCALPROC WriteAPBXCDBuildSettings(void)
{
	if (ide_vers >= 3100) {
		WriteDestFileLn("ALWAYS_SEARCH_USER_PATHS = NO;");
	}

	if (ide_vers >= 2100) {
		/* if (CrossCompile) */ {
			if (gbk_cpufam_x86 == gbo_cpufam) {
				WriteDestFileLn("ARCHS = i386;");
				/*
					may be preferred in later versions:
					WriteDestFileLn(
						"ARCHS = \"$(ARCHS_STANDARD_32_BIT)\";");
				*/
			} else if (gbk_cpufam_x64 == gbo_cpufam) {
				WriteDestFileLn("ARCHS = x86_64;");
			} else {
				WriteDestFileLn("ARCHS = ppc;");
			}
		}
	}
	if (ide_vers >= 2100) { /*^*/
		/*
			seems to work in Xcode 2.1, but doesn't
			really appear in settings user interface
			until Xcode 2.2
		*/
		WriteDestFileLn(
			"CONFIGURATION_BUILD_DIR = \"$(PROJECT_DIR)\";");
	}
	if (ide_vers >= 2200) { /*^*/
		WriteDestFileLn("COPY_PHASE_STRIP = NO;");
	} else {
		if (gbk_dbg_on != gbo_dbg) {
			WriteDestFileLn("COPY_PHASE_STRIP = YES;");
		} else {
			WriteDestFileLn("COPY_PHASE_STRIP = NO;");
		}
	}
	if (ide_vers >= 1500) {
		if (gbk_dbg_on != gbo_dbg) {
			WriteDestFileLn("DEPLOYMENT_POSTPROCESSING = YES;");
		}
	}
	if (ide_vers < 1500) {
		if (gbk_dbg_on != gbo_dbg) {
			WriteDestFileLn("DEBUGGING_SYMBOLS = NO;");
		}
	}
	if (ide_vers >= 1500) {
		WriteDestFileLn("GCC_CW_ASM_SYNTAX = NO;");
	}
	if (ide_vers >= 1000) {
		WriteDestFileLn("GCC_DYNAMIC_NO_PIC = YES;");
	}
	if (ide_vers < 1500) {
		WriteAPBQuotedField("FRAMEWORK_SEARCH_PATHS", "");
	}
	if ((ide_vers >= 1000) && (ide_vers < 4000)) {
		WriteDestFileLn("GCC_ENABLE_FIX_AND_CONTINUE = NO;");
	}
	if (ide_vers >= 1000) {
		if (gbk_dbg_on != gbo_dbg) {
			WriteDestFileLn("GCC_GENERATE_DEBUGGING_SYMBOLS = NO;");
		}
	}
	if ((ide_vers >= 1500) && (ide_vers < 4000)) {
		WriteDestFileLn("GCC_MODEL_TUNING = \"\";");
	}
	if (ide_vers >= 1000) {
		if (gbk_dbg_on == gbo_dbg) {
			WriteDestFileLn("GCC_OPTIMIZATION_LEVEL = 0;");
		} else {
			WriteDestFileLn("GCC_OPTIMIZATION_LEVEL = s;");
		}
	}
	if (ide_vers >= 2100) {
		WriteDestFileLn("GCC_PRECOMPILE_PREFIX_HEADER = NO;");
		WriteDestFileLn("GCC_PREFIX_HEADER = \"\";");
		WriteDestFileLn("GCC_SYMBOLS_PRIVATE_EXTERN = NO;");
	}
	if (ide_vers >= 3100) {
		if (ide_vers < 3200) {
			WriteDestFileLn("GCC_VERSION = 4.0;");
		}
	}
	if (ide_vers >= 1000) {
		WriteDestFileLn("GCC_WARN_ABOUT_MISSING_PROTOTYPES = YES;");
	}

	if (HaveMacBundleApp) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("INFOPLIST_FILE = ");
		WriteInfoPlistFilePath();
		WriteCStrToDestFile(";");
		WriteEndDestFileLn();
	}
	if (ide_vers >= 1000) {
		WriteDestFileLn("INSTALL_PATH = \"$(HOME)/Applications\";");
	}
	if (ide_vers < 1500) {
		WriteAPBQuotedField("LIBRARY_SEARCH_PATHS", "");
	}
	if (ide_vers >= 2100) {
		if (gbk_cpufam_ppc == gbo_cpufam) {
			WriteDestFileLn("MACOSX_DEPLOYMENT_TARGET = 10.1;");
		} else {
			if (ide_vers >= 9000) {
				WriteDestFileLn("MACOSX_DEPLOYMENT_TARGET = 10.6;");
			} else {
				WriteDestFileLn("MACOSX_DEPLOYMENT_TARGET = 10.4;");
			}
		}
	}
	if (ide_vers < 1500) {
		if (gbk_dbg_on == gbo_dbg) {
			WriteAPBQuotedField("OPTIMIZATION_CFLAGS", "-O0");
		}
	}
	if (ide_vers < 1500) {
		WriteAPBQuotedField("OTHER_LDFLAGS", "");
		WriteAPBQuotedField("OTHER_REZFLAGS", "");
	} else {
		if (! HaveMacBundleApp) {
			WriteAPBXCDBgnObjList("OTHER_LDFLAGS");
				WriteDestFileLn("\"-L/usr/X11R6/lib\",");
#if 0
				WriteDestFileLn("\"-lXext\",");
#endif
				WriteDestFileLn("\"-lX11\",");
			WriteAPBXCDEndObjList();
		}
	}
	if ((ide_vers >= 1500) && (ide_vers < 4000)) {
		WriteDestFileLn("PREBINDING = NO;");
	}
	WriteBgnDestFileLn();
	WriteCStrToDestFile("PRODUCT_NAME = ");
	WriteStrAppAbbrev();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();
	if (ide_vers >= 2200) {
		if (ide_vers >= 4300) {
			WriteDestFileLn("SDKROOT = macosx;");
		} else if (ide_vers >= 3200) {
			WriteDestFileLn("SDKROOT = macosx10.6;");
		} else if (ide_vers >= 3100) {
			WriteDestFileLn("SDKROOT = macosx10.5;");
		} else {
			WriteDestFileLn(
				"SDKROOT = /Developer/SDKs/MacOSX10.4u.sdk;");
		}
	}
	if (ide_vers < 1500) {
		WriteAPBQuotedField("SECTORDER_FLAGS", "");
	}
	if (ide_vers >= 2200) {
		if (gbk_dbg_on != gbo_dbg) {
			WriteDestFileLn("SEPARATE_STRIP = YES;");
			WriteDestFileLn("STRIPFLAGS = \"-u -r\";");
			WriteDestFileLn("STRIP_INSTALLED_PRODUCT = YES;");
		}
	}
	if ((ide_vers >= 1500) && (ide_vers < 2100)) {
		WriteDestFileLn("SYMROOT = \"$(PROJECT_DIR)\";");
	}
	WriteDestFileLn("USER_HEADER_SEARCH_PATHS = \"$(SRCROOT)/"
		cfg_d_name
		"\";");
	if (ide_vers >= 2100) {
		WriteAPBXCDBgnObjList("WARNING_CFLAGS");
			WriteDestFileLn("\"-Wall\",");
			WriteDestFileLn("\"-Wundef\",");
			WriteDestFileLn("\"-Wstrict-prototypes\",");
			WriteDestFileLn("\"-Wno-uninitialized\",");
		WriteAPBXCDEndObjList();
	} else {
		WriteAPBQuotedField("WARNING_CFLAGS",
			"-Wall -Wstrict-prototypes -Wno-uninitialized"
			" -Wno-four-char-constants -Wno-unknown-pragmas");
	}
	if ((HaveMacBundleApp) && (ide_vers < 3100)) {
		WriteDestFileLn("WRAPPER_EXTENSION = app;");
	}
	if ((ide_vers >= 1000) && (ide_vers < 3100)) {
		WriteDestFileLn("ZERO_LINK = NO;");
	}
}

LOCALPROC WriteAPBplist(void)
{
	int SaveDestFileIndent = DestFileIndent;

	DestFileIndent = 0;

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<!DOCTYPE plist PUBLIC ");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("-//Apple Computer//DTD PLIST 1.0//EN");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteSpaceToDestFile();
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(
		"http://www.apple.com/DTDs/PropertyList-1.0.dtd");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(">");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("<plist version=");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile("1.0");
	WriteBackSlashToDestFile();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(">");
	WriteEndDestFileLn();

	CurPListFormat = kPListRaw;

	WriteMyInfoPListContents();

	WriteDestFileLn("</plist>");
	WriteBgnDestFileLn();
	WriteQuoteToDestFile();
	WriteCStrToDestFile(";");
	WriteEndDestFileLn();

	DestFileIndent = SaveDestFileIndent;
}

LOCALPROC WriteStrFrameworks(void)
{
	WriteCStrToDestFile("Frameworks");
}

LOCALPROC WriteStrSources(void)
{
	WriteCStrToDestFile("Sources");
}

LOCALPROC WriteStrResources(void)
{
	WriteCStrToDestFile("Resources");
}

LOCALPROC WriteStrRez(void)
{
	WriteCStrToDestFile("Rez");
}

LOCALPROC WriteStrFrameworksLibraries(void)
{
	WriteCStrToDestFile("External Frameworks and Libraries");
}

LOCALPROC WriteStrQuoteFrameworksLibraries(void)
{
	WriteQuoteToDestFile();
	WriteStrFrameworksLibraries();
	WriteQuoteToDestFile();
}

LOCALPROC WriteStrProducts(void)
{
	WriteCStrToDestFile("Products");
}

LOCALPROC WriteStrHeaders(void)
{
	WriteCStrToDestFile("Headers");
}

LOCALPROC WriteStrIncludes(void)
{
	WriteCStrToDestFile("Includes");
}

LOCALPROC WriteStrProjectObject(void)
{
	WriteCStrToDestFile("Project object");
}

LOCALPROC WriteStrEmptyQuote(void)
{
	WriteQuoteToDestFile();
	WriteQuoteToDestFile();
}

LOCALPROC WriteAPBXCDDObjAPropPathNull(void)
{
	if (ide_vers < 2100) {
		WriteAPBXCDDObjAPropPath(WriteStrEmptyQuote);
	}
}

LOCALPROC WriteAPBXCDMainGroupName(void)
{
	if (ide_vers < 1000) {
		WriteQuoteToDestFile();
		WriteAppVariationStr();
		WriteQuoteToDestFile();
	} else {
		WriteStrAppAbbrev();
	}
}

LOCALPROC WriteAPBXCDDObjAPropIsaAppTarg(void)
{
	if (ide_vers < 1000) {
		WriteAPBXCDDObjAPropIsa("PBXApplicationTarget");
	} else {
		WriteAPBXCDDObjAPropIsa("PBXNativeTarget");
	}
}

LOCALPROC WriteStrConfListPBXProject(void)
{
	WriteCStrToDestFile("Build configuration list for PBXProject \"");
	WriteStrAppAbbrev();
	WriteCStrToDestFile("\"");
}

LOCALPROC WriteStrConfListPBXNativeTarget(void)
{
	WriteCStrToDestFile(
		"Build configuration list for PBXNativeTarget \"");
	WriteStrAppAbbrev();
	WriteCStrToDestFile("\"");
}

#define HaveFrameworks HaveMacBundleApp

LOCALPROC WriteXCDdummyfile(void)
{
	WriteDestFileLn("dummy");
}

LOCALPROC WriteXCDProjectFile(void)
{
	WriteDestFileLn("// !$*UTF8*$!");
	WriteDestFileLn("{");
	++DestFileIndent;
		WriteDestFileLn("archiveVersion = 1;");
		WriteDestFileLn("classes = {");
		WriteDestFileLn("};");
		if (ide_vers >= 3200) {
			WriteDestFileLn("objectVersion = 46;");
		} else if (ide_vers >= 3100) {
			WriteDestFileLn("objectVersion = 45;");
		} else if (ide_vers >= 2100) {
			WriteDestFileLn("objectVersion = 42;");
		} else if (ide_vers >= 1000) {
			WriteDestFileLn("objectVersion = 39;");
		} else {
			WriteDestFileLn("objectVersion = 38;");
		}
		WriteDestFileLn("objects = {");
	++DestFileIndent;

	DoBeginSectionAPBXCD("PBXBuildFile");
		DoAllSrcFilesWithSetup(DoSrcFileAPBXCDaddFile);
		if (HaveMacBundleApp) {
			DoAllDocTypesWithSetup(DoDocTypeAPBXCDaddFile);
		}

		if (HaveFrameworks) {
			DoAllFrameWorksWithSetup(DoFrameWorkAPBXCDaddFile);
		}
		if (HaveAPBXCD_StdcLib) {
			DoLibStdcAPBXCDaddFile();
		}

		if (HaveMacRrscs) {
			DoRsrcAPBXCDaddFile();
		}

		if (HaveMacBundleApp) {
			if (HaveAPBXCD_LangDummy) {
				DoDummyLangAPBXCDaddFile();
			}
		}
	DoEndSectionAPBXCD("PBXBuildFile");

	if (ide_vers < 2300) {
		DoBeginSectionAPBXCD("PBXBuildStyle");
			WriteAPBXCDBeginObject(APBospcBuildStyle,
				0, WriteXCDconfigname);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("PBXBuildStyle");
				}

				if (ide_vers < 1500) {
					WriteAPBXCDBgnObjList("buildRules");
					WriteAPBXCDEndObjList();
				}
				WriteDestFileLn("buildSettings = {");
				WriteDestFileLn("};");
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("PBXBuildStyle");
				}
				WriteAPBXCDDObjAPropName(WriteXCDconfigname);
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("PBXBuildStyle");
	}

	DoBeginSectionAPBXCD("PBXFileReference");
		DoAllSrcFilesWithSetup(DoSrcFileAPBXCDaddFileRef);

		if (HaveAPBXCD_Headers) {
			DoAllSrcFilesWithSetup(DoHeaderFileXCDaddFileRef);
			DoAllExtraHeaders2WithSetup(
				DoExtraHeaderFileXCDaddFileRef);
		}

		if (HaveMacBundleApp) {
			DoAllDocTypesWithSetup(DoDocTypeAPBXCDaddFileRef);
		}

		if (HaveFrameworks) {
			DoAllFrameWorksWithSetup(DoFrameWorkAPBXCDaddFileRef);
		}
		if (HaveAPBXCD_StdcLib) {
			DoLibStdcAPBXCDaddFileRef();
		}

		DoProductAPBXCDaddFileRef();

		if (HaveMacBundleApp) {
			if (HaveAPBXCD_PlistFile) {
				DoPlistAPBXCDaddFileRef();
			}
		}
		if (HaveMacRrscs) {
			DoRsrcAPBXCDaddFileRef();
		}
		if (HaveMacBundleApp) {
			if (HaveAPBXCD_LangDummy) {
				DoLangDummyAPBXCDaddFileRef();
			}
		}
	DoEndSectionAPBXCD("PBXFileReference");

	DoBeginSectionAPBXCD("PBXFrameworksBuildPhase");
		WriteAPBXCDBeginObject(APBospcPhaseLibs, 0, WriteStrFrameworks);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXFrameworksBuildPhase");
			}

			WriteDestFileLn("buildActionMask = 2147483647;");
			WriteAPBXCDBgnObjList("files");
				if (HaveFrameworks) {
					DoAllFrameWorksWithSetup(
						DoFrameworkAPBXCDaddToBuild);
				}
				if (HaveAPBXCD_StdcLib) {
					WriteAPBXCDobjlistelmp(APBospcLibStdcBld,
						0, WriteLibStdcName);
				}
			WriteAPBXCDEndObjList();
			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXFrameworksBuildPhase");
			}
			WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXFrameworksBuildPhase");

	DoBeginSectionAPBXCD("PBXGroup");
		WriteAPBXCDBeginObject(APBospcSources, 0, WriteStrSources);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaGroup();
			}

			WriteAPBXCDBgnObjList("children");
				DoAllSrcFilesWithSetup(DoSrcFileAPBXCDaddToGroup);
				if (HaveMacRrscs) {
					if (ide_vers >= 2100) {
						WriteAPBXCDobjlistelmp(APBospcMainRsrcRf, 0,
							WriteMainRsrcName);
					}
				}
			WriteAPBXCDEndObjList();
			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaGroup();
			}
			WriteAPBXCDDObjAPropName(WriteStrSources);
			WriteAPBXCDDObjAPropPathNull();
			WriteAPBXCDDObjAPropRefType4();
			WriteAPBXCDDObjAPropSourceTreeGroup();
		WriteAPBXCDEndObject();

		if (HaveMacBundleApp) {
			WriteAPBXCDBeginObject(APBospcResources,
				0, WriteStrResources);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}

				WriteAPBXCDBgnObjList("children");
					if (HaveMacRrscs) {
						if (ide_vers < 2100) {
							WriteAPBXCDobjlistelmp(APBospcMainRsrcRf,
								0, WriteMainRsrcName);
						}
					}
					DoAllDocTypesWithSetup(
						DoDocTypeAPBXCDaddToGroup);
					if (HaveAPBXCD_PlistFile) {
						WriteAPBXCDobjlistelmp(APBospcPlistRf,
							0, WriteInfoPlistFileName);
					}
					if (HaveAPBXCD_LangDummy) {
#if 0
						WriteAPBXCDobjlistelmp(APBospcLangRf,
							0, WriteLProjFolderName);
#endif
						WriteAPBXCDobjlistelmp(APBospcLangDummyRf,
							0, WriteDummyLangFileName);
					}
				WriteAPBXCDEndObjList();
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}
				WriteAPBXCDDObjAPropName(WriteStrResources);
				WriteAPBXCDDObjAPropPathNull();
				WriteAPBXCDDObjAPropRefType4();
				WriteAPBXCDDObjAPropSourceTreeGroup();
			WriteAPBXCDEndObject();
		}

		if (HaveFrameworks) {
			WriteAPBXCDBeginObject(APBospcLibraries,
				0, WriteStrFrameworksLibraries);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}

				WriteAPBXCDBgnObjList("children");
					DoAllFrameWorksWithSetup(
						DoFrameworkAPBXCDaddToLibraries);
					if (HaveAPBXCD_StdcLib) {
						WriteAPBXCDobjlistelmp(APBospcLibStdcRf,
							0, WriteLibStdcName);
					}
				WriteAPBXCDEndObjList();
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}
				WriteAPBXCDDObjAPropName(
					WriteStrQuoteFrameworksLibraries);
				WriteAPBXCDDObjAPropPathNull();
				WriteAPBXCDDObjAPropRefType4();
				WriteAPBXCDDObjAPropSourceTreeGroup();
			WriteAPBXCDEndObject();
		}

		WriteAPBXCDBeginObject(APBospcProducts, 0, WriteStrProducts);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaGroup();
			}

			WriteAPBXCDBgnObjList("children");
				WriteAPBXCDobjlistelmp(APBospcProductRef,
					0, WriteAppNameStr);
			WriteAPBXCDEndObjList();
			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaGroup();
			}
			WriteAPBXCDDObjAPropName(WriteStrProducts);
			WriteAPBXCDDObjAPropRefType4();
			WriteAPBXCDDObjAPropSourceTreeGroup();
		WriteAPBXCDEndObject();

		WriteAPBXCDBeginObject(APBospcMainGroup, 0, WriteStrAppAbbrev);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaGroup();
			}

			WriteAPBXCDBgnObjList("children");
				WriteAPBXCDobjlistelmp(APBospcSources,
					0, WriteStrSources);
				if (HaveAPBXCD_Headers) {
					WriteAPBXCDobjlistelmp(APBospcSrcHeaders,
						0, WriteStrHeaders);
					WriteAPBXCDobjlistelmp(APBospcIncludes,
						0, WriteStrIncludes);
				}
				if (HaveMacBundleApp) {
					WriteAPBXCDobjlistelmp(APBospcResources,
						0, WriteStrResources);
				}
				if (HaveFrameworks) {
					WriteAPBXCDobjlistelmp(APBospcLibraries,
						0, WriteStrFrameworksLibraries);
				}
				WriteAPBXCDobjlistelmp(APBospcProducts,
					0, WriteStrProducts);
			WriteAPBXCDEndObjList();

			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaGroup();
			}
			WriteAPBXCDDObjAPropName(WriteAPBXCDMainGroupName);
			WriteAPBXCDDObjAPropPathNull();
			WriteAPBXCDDObjAPropRefType4();
			WriteAPBXCDDObjAPropSourceTreeGroup();
		WriteAPBXCDEndObject();
		if (HaveAPBXCD_Headers) {
			WriteAPBXCDBeginObject(APBospcSrcHeaders,
				0, WriteStrHeaders);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}

				WriteAPBXCDBgnObjList("children");
					DoAllSrcFilesWithSetup(
						DoHeaderFileXCDaddToGroup);
				WriteAPBXCDEndObjList();
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}
				WriteAPBXCDDObjAPropName(WriteStrHeaders);
				WriteAPBXCDDObjAPropRefType4();
				WriteAPBXCDDObjAPropSourceTreeGroup();
			WriteAPBXCDEndObject();
			WriteAPBXCDBeginObject(APBospcIncludes,
				0, WriteStrIncludes);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}

				WriteAPBXCDBgnObjList("children");
					DoAllExtraHeaders2WithSetup(
						DoExtraHeaderFileXCDaddToGroup);
				WriteAPBXCDEndObjList();
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}
				WriteAPBXCDDObjAPropName(WriteStrIncludes);
				WriteAPBXCDDObjAPropRefType4();
				WriteAPBXCDDObjAPropSourceTreeGroup();
			WriteAPBXCDEndObject();
#if 0
			WriteAPBXCDBeginObject(APBospcLangRf,
				0, WriteLProjFolderName);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}

				WriteAPBXCDBgnObjList("children");
					WriteAPBXCDobjlistelmp(APBospcLangDummyRf,
						0, WriteDummyLangFileName);
				WriteAPBXCDEndObjList();
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsaGroup();
				}
				WriteAPBXCDDObjAPropName(WriteLProjFolderName);
				WriteAPBXCDDObjAPropPath(WriteLProjFolderPath);
				WriteAPBXCDDObjAPropRefType4();
				WriteAPBXCDDObjAPropSourceTreeGroup();
			WriteAPBXCDEndObject();
#endif
		}
	DoEndSectionAPBXCD("PBXGroup");

	DoBeginSectionAPBXCD("PBXNativeTarget");
		WriteAPBXCDBeginObject(APBospcTarget, 0, WriteStrAppAbbrev);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaAppTarg();
			}

			if (ide_vers >= 2100) {
				WriteAPBXCDDObjAProp_SO("buildConfigurationList",
					APBospcLstNatCnfg, 0,
					WriteStrConfListPBXNativeTarget);
			}
			WriteAPBXCDBgnObjList("buildPhases");
				if (ide_vers < 1500) {
					WriteAPBXCDobjlistelmp(APBospcHeaders,
						0, WriteStrHeaders);
				}
				if (HaveMacBundleApp) {
					WriteAPBXCDobjlistelmp(APBospcBunRsrcs,
						0, WriteStrResources);
				}
				WriteAPBXCDobjlistelmp(APBospcPhaseSrcs,
					0, WriteStrSources);
				WriteAPBXCDobjlistelmp(APBospcPhaseLibs,
					0, WriteStrFrameworks);
				if (HaveMacRrscs) {
					WriteAPBXCDobjlistelmp(APBospcPhaseRsrc,
						0, WriteStrRez);
				}
			WriteAPBXCDEndObjList();
			if (ide_vers >= 1000) {
				WriteAPBXCDBgnObjList("buildRules");
				WriteAPBXCDEndObjList();
			}
			if (ide_vers < 2300) {
				WriteDestFileLn("buildSettings = {");
				++DestFileIndent;
				if (ide_vers < 2100) {
					WriteAPBXCDBuildSettings();
				}
				--DestFileIndent;
				WriteDestFileLn("};");
			}
			WriteAPBXCDBgnObjList("dependencies");
			WriteAPBXCDEndObjList();

			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsaAppTarg();
			}

			WriteAPBXCDDObjAPropName(WriteStrAppAbbrev);

			if (ide_vers >= 1000) {
				WriteDestFileLn(
					"productInstallPath = \"$(HOME)/Applications\";");
			}

			WriteBgnDestFileLn();
			WriteCStrToDestFile("productName = ");
			if (ide_vers < 1000) {
				WriteQuoteToDestFile();
			}
			WriteStrAppAbbrev();
			if (ide_vers < 1000) {
				WriteQuoteToDestFile();
			}
			WriteCStrToDestFile(";");
			WriteEndDestFileLn();

			WriteAPBXCDDObjAProp_SO("productReference",
				APBospcProductRef, 0,
				WriteAppNameStr);

			if (ide_vers >= 1000) {
				if (HaveMacBundleApp) {
					WriteDestFileLn(
						"productType = "
						"\"com.apple.product-type.application\";");
				} else {
					WriteDestFileLn(
						"productType = "
						"\"com.apple.product-type.tool\";");
				}
			}
			if ((! HaveAPBXCD_PlistFile) && HaveMacBundleApp) {
				WriteBgnDestFileLn();
				WriteCStrToDestFile("productSettingsXML = ");
				WriteQuoteToDestFile();
				WriteCStrToDestFile("<?xml version=");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToDestFile("1.0");
				WriteBackSlashToDestFile();
				WriteQuoteToDestFile();
				WriteCStrToDestFile(" encoding=\\\"UTF-8\\\"?>");
				WriteEndDestFileLn();

				WriteAPBplist();
			}
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXNativeTarget");

	DoBeginSectionAPBXCD("PBXProject");
		WriteAPBXCDBeginObject(APBospcRoot, 0, WriteStrProjectObject);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXProject");
			}
			if (ide_vers >= 4300) {
				WriteDestFileLn("attributes = {");
				++DestFileIndent;
					WriteBgnDestFileLn();
					WriteCStrToDestFile("LastUpgradeCheck = ");
					WriteCharToDestFile('0'
						+ ((ide_vers / 10000) % 10));
					WriteCharToDestFile('0' + ((ide_vers / 1000) % 10));
					WriteCharToDestFile('0' + ((ide_vers / 100) % 10));
					WriteCStrToDestFile("0;");
					WriteEndDestFileLn();
				--DestFileIndent;
				WriteDestFileLn("};");
			}
			if (ide_vers >= 2100) {
				WriteAPBXCDDObjAProp_SO("buildConfigurationList",
					APBospcLstPrjCnfg, 0,
					WriteStrConfListPBXProject);
			}
			if (ide_vers < 2300) {
				if (ide_vers >= 1000) {
					WriteDestFileLn("buildSettings = {");
					WriteDestFileLn("};");
				}
				WriteAPBXCDBgnObjList("buildStyles");
					WriteAPBXCDobjlistelmp(APBospcBuildStyle,
						0, WriteXCDconfigname);
				WriteAPBXCDEndObjList();
			}
			if (ide_vers >= 3200) {
				WriteDestFileLn(
					"compatibilityVersion = \"Xcode 3.2\";");
			} else if (ide_vers >= 3100) {
				WriteDestFileLn(
					"compatibilityVersion = \"Xcode 3.1\";");
			}
			WriteDestFileLn("hasScannedForEncodings = 1;");

			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXProject");
			}

			WriteAPBXCDDObjAProp_SO("mainGroup",
				APBospcMainGroup, 0,
				WriteStrAppAbbrev);

			if (ide_vers >= 4000) {
				WriteAPBXCDDObjAProp_SO("productRefGroup",
					APBospcProducts, 0,
					WriteStrProducts);
			}

			WriteDestFileLn("projectDirPath = \"\";");

			if (ide_vers >= 3100) {
				WriteDestFileLn("projectRoot = \"\";");
			}

			WriteAPBXCDBgnObjList("targets");
				WriteAPBXCDobjlistelmp(APBospcTarget,
					0, WriteStrAppAbbrev);
			WriteAPBXCDEndObjList();
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXProject");

	if (HaveMacBundleApp) {
		DoBeginSectionAPBXCD("PBXResourcesBuildPhase");
			WriteAPBXCDBeginObject(APBospcBunRsrcs,
				0, WriteStrResources);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("PBXResourcesBuildPhase");
				}

				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBXCDBgnObjList("files");
					DoAllDocTypesWithSetup(
						DoDocTypeAPBXCDaddToSources);
					if (HaveAPBXCD_LangDummy) {
						DoLangDummyAPBXCDaddToSources();
					}
				WriteAPBXCDEndObjList();
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("PBXResourcesBuildPhase");
				}
				WriteDestFileLn(
					"runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("PBXResourcesBuildPhase");
	}
	if (HaveMacRrscs) {
		DoBeginSectionAPBXCD("PBXRezBuildPhase");
			WriteAPBXCDBeginObject(APBospcPhaseRsrc, 0, WriteStrRez);
				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("PBXRezBuildPhase");
				}

				WriteDestFileLn("buildActionMask = 2147483647;");
				WriteAPBXCDBgnObjList("files");
					WriteAPBXCDobjlistelmp(APBospcMnRsrcBld, 0,
						WriteMainAPBXCDRsrcNameinRez);
				WriteAPBXCDEndObjList();
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("PBXRezBuildPhase");
				}
				WriteDestFileLn(
					"runOnlyForDeploymentPostprocessing = 0;");
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("PBXRezBuildPhase");
	}

	if (ide_vers < 1500) {
		WriteAPBXCDBeginObject(APBospcHeaders, 0, WriteStrHeaders);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXHeadersBuildPhase");
			}

			WriteDestFileLn("buildActionMask = 2147483647;");
			WriteAPBXCDBgnObjList("files");
			WriteAPBXCDEndObjList();
			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXHeadersBuildPhase");
			}
			WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
		WriteAPBXCDEndObject();
	}

	DoBeginSectionAPBXCD("PBXSourcesBuildPhase");
		WriteAPBXCDBeginObject(APBospcPhaseSrcs, 0, WriteStrSources);
			if (HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXSourcesBuildPhase");
			}

			WriteDestFileLn("buildActionMask = 2147483647;");
			WriteAPBXCDBgnObjList("files");
				DoAllSrcFilesSortWithSetup(
					DoSrcFileAPBXCDaddToSources);
			WriteAPBXCDEndObjList();
			if (! HaveAPBXCD_IsaFirst) {
				WriteAPBXCDDObjAPropIsa("PBXSourcesBuildPhase");
			}
			WriteDestFileLn("runOnlyForDeploymentPostprocessing = 0;");
		WriteAPBXCDEndObject();
	DoEndSectionAPBXCD("PBXSourcesBuildPhase");

	if (HaveMacBundleApp) {
		if (HaveAPBXCD_LangDummy) {
			DoBeginSectionAPBXCD("PBXVariantGroup");
				DoLangDummyAPBXCDaddVariant();
			DoEndSectionAPBXCD("PBXVariantGroup");
		}
	}

	if (ide_vers >= 2100) {
		DoBeginSectionAPBXCD("XCBuildConfiguration");
			WriteAPBXCDBeginObject(APBospcNatCnfg,
				0, WriteXCDconfigname);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCBuildConfiguration");
				}

				WriteDestFileLn("buildSettings = {");
				++DestFileIndent;
					WriteAPBXCDBuildSettings();
				--DestFileIndent;
				WriteDestFileLn("};");
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCBuildConfiguration");
				}
				WriteAPBXCDDObjAPropName(WriteXCDconfigname);
			WriteAPBXCDEndObject();
			WriteAPBXCDBeginObject(APBospcPrjCnfg,
				0, WriteXCDconfigname);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCBuildConfiguration");
				}

				WriteDestFileLn("buildSettings = {");
				WriteDestFileLn("};");
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCBuildConfiguration");
				}
				WriteAPBXCDDObjAPropName(WriteXCDconfigname);
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("XCBuildConfiguration");
	}

	if (ide_vers >= 2100) {
		DoBeginSectionAPBXCD("XCConfigurationList");
			WriteAPBXCDBeginObject(APBospcLstNatCnfg, 0,
				WriteStrConfListPBXNativeTarget);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCConfigurationList");
				}

				WriteAPBXCDBgnObjList("buildConfigurations");
					WriteAPBXCDobjlistelmp(APBospcNatCnfg,
						0, WriteXCDconfigname);
				WriteAPBXCDEndObjList();
				WriteAPBXCDDObjAProp_SS(
					"defaultConfigurationIsVisible", "0");
				WriteAPBXCDDObjAProp_SP(
					"defaultConfigurationName", WriteXCDconfigname);
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCConfigurationList");
				}
			WriteAPBXCDEndObject();
			WriteAPBXCDBeginObject(APBospcLstPrjCnfg, 0,
				WriteStrConfListPBXProject);

				if (HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCConfigurationList");
				}

				WriteAPBXCDBgnObjList("buildConfigurations");
					WriteAPBXCDobjlistelmp(APBospcPrjCnfg,
						0, WriteXCDconfigname);
				WriteAPBXCDEndObjList();
				WriteAPBXCDDObjAProp_SS(
					"defaultConfigurationIsVisible", "0");
				WriteAPBXCDDObjAProp_SP(
					"defaultConfigurationName", WriteXCDconfigname);
				if (! HaveAPBXCD_IsaFirst) {
					WriteAPBXCDDObjAPropIsa("XCConfigurationList");
				}
			WriteAPBXCDEndObject();
		DoEndSectionAPBXCD("XCConfigurationList");
	}

	--DestFileIndent;
		WriteDestFileLn("};");
		WriteBgnDestFileLn();
		WriteCStrToDestFile("rootObject = ");
		WriteAPBXCDObjectIdAndComment(APBospcRoot,
			0, WriteStrProjectObject);
		WriteCStrToDestFile(";");
		WriteEndDestFileLn();
	--DestFileIndent;
	WriteDestFileLn("}");
}

LOCALPROC WriteOutDummyLangContents(void)
{
	WriteDestFileLn(
		"This file is here because some archive extraction");
	WriteDestFileLn("software will not create an empty directory.");
}

LOCALPROC WriteXCDSpecificFiles(void)
{
	MakeSubDirectory("my_proj_d", "my_project_d", vStrAppAbbrev,
		(ide_vers >= 2100) ? ".xcodeproj" : ".pbproj");

	WriteADstFile1("my_proj_d",
		"project", ".pbxproj", "project file",
		WriteXCDProjectFile);

	if (HaveMacBundleApp) {
		if (HaveAPBXCD_PlistFile) {
			WritePListData();
		}

		MakeSubDirectory("my_lang_d", "my_config_d",
			GetLProjName(gbo_lang), ".lproj");

		WriteADstFile1("my_lang_d",
			"dummy", ".txt", "Dummy",
			WriteOutDummyLangContents);
	}

	if (WantSandbox) {
		WriteEntitlementsData();
	}
}
