/*
	WRMPLIST.i
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
	WRite Macintosh PLIST
*/


enum {
	kPListRaw, /* native plist data */
	kPListPLC, /* metrowerks property list compiler */
	kNumPListFormats
};

LOCALVAR int CurPListFormat = kPListPLC;

LOCALPROC WritePListProcString(MyProc p)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginProcValEndLine("string", p);
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}

LOCALPROC WritePListString(char *s)
{
	MyPtr SavepDt = pDt;

	pDt = (MyPtr)s;
	WritePListProcString(WritepDtString);
	pDt = SavepDt;
}

LOCALPROC WritePListKeyProcString(char *k, MyProc p)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteXMLtagBeginProcValEndLine("string", p);
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("key ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(k);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" value string ");
		WriteQuoteToDestFile();
		p();
		WriteQuoteToDestFile();
		WriteEndDestFileLn();
	}
}

LOCALPROC WritePListKeyString(char *k, char *s)
{
	MyPtr SavepDt = pDt;

	pDt = (MyPtr)s;
	WritePListKeyProcString(k, WritepDtString);
	pDt = SavepDt;
}

LOCALPROC WritePListBeginKeyArray(char *k)
{
	if (CurPListFormat == kPListRaw) {
		WriteXMLtagBeginValEndLine("key", k);
		WriteBeginXMLtagLine("array");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("key ");
		WriteQuoteToDestFile();
		WriteCStrToDestFile(k);
		WriteQuoteToDestFile();
		WriteCStrToDestFile(" value array");
		WriteEndDestFileLn();
		WriteDestFileLn("[");
		++DestFileIndent;
	}
}

LOCALPROC WritePListEndKeyArray(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("array");
	} else {
		--DestFileIndent;
		WriteDestFileLn("]");
	}
}

LOCALPROC WritePListBeginDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteBeginXMLtagLine("dict");
	} else {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("dictionary");
		WriteEndDestFileLn();
		WriteDestFileLn("{");
		++DestFileIndent;
	}
}

LOCALPROC WritePListEndDict(void)
{
	if (CurPListFormat == kPListRaw) {
		WriteEndXMLtagLine("dict");
	} else {
		--DestFileIndent;
		WriteDestFileLn("}");
	}
}

LOCALPROC WriteInfoPList(MyProc p)
{
	CurPListFormat = kPListRaw;

	WriteDestFileLn("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
#if 0
	WriteDestFileLn(
		"<!DOCTYPE plist SYSTEM \"file://"
		"localhost/System/Library/DTDs/PropertyList.dtd\">");
	WriteDestFileLn("<plist version=\"0.9\">");
#else
	if ((gbk_ide_xcd == cur_ide) && (ide_vers >= 3100)) {
		WriteDestFileLn(
			"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\""
			" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">");
	} else {
		WriteDestFileLn(
			"<!DOCTYPE plist PUBLIC \"-//"
			"Apple Computer//DTD PLIST 1.0//EN\""
			" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">");
	}
	WriteDestFileLn("<plist version=\"1.0\">");
#endif

	p();

	WriteDestFileLn("</plist>");
}

LOCALPROC WriteMainPLC(MyProc p)
{
	CurPListFormat = kPListPLC;

	WriteDestFileLn("plist");
	WriteDestFileLn("{");
	++DestFileIndent;

	p();

	--DestFileIndent;
	WriteDestFileLn("}");
}

LOCALPROC WritepDtIconTypeName(void)
{
	WriteStrAppUnabrevName();
	WriteSpaceToDestFile();
	WriteCStrToDestFile(DoDocType_gd()->LongName);
}

LOCALPROC WriteOneExtension2Array(char *s)
{
	WritePListString(s);
}

LOCALPROC WriteOneCFBundleDocumentType(void)
{
	if (0 != DocTypeCounter) {
		WritePListBeginDict();
			if (DoDocType_gd()->WriteExtensionList != nullpr) {
				WritePListBeginKeyArray("CFBundleTypeExtensions");
					DoDocType_gd()->WriteExtensionList(
						WriteOneExtension2Array);
				WritePListEndKeyArray();
			}
			WritePListKeyProcString("CFBundleTypeIconFile",
				WriteDocTypeIconFileName);
			WritePListKeyProcString("CFBundleTypeName",
				WritepDtIconTypeName);
			WritePListBeginKeyArray("CFBundleTypeOSTypes");
				WritePListProcString(WriteDocTypeIconMacType);
			WritePListEndKeyArray();
			WritePListKeyString("CFBundleTypeRole", "Editor");
		WritePListEndDict();
	}
}

LOCALPROC WriteTheBundleIdentifier(void)
{
	WriteCStrToDestFile(kBundleIdentifier);
	if (WantIconMaster) {
		WriteCStrToDestFile(".im");
	}
}

LOCALPROC WriteMyInfoPListContents(void)
{
	WritePListBeginDict();

		/* in order preferred by latest xcode (alphabetical) */

		WritePListKeyString("CFBundleDevelopmentRegion", "English");

		WritePListBeginKeyArray("CFBundleDocumentTypes");
			WritePListBeginDict();
				WritePListBeginKeyArray("CFBundleTypeOSTypes");
					WritePListString("****");
				WritePListEndKeyArray();
				WritePListKeyString("CFBundleTypeRole", "Editor");
			WritePListEndDict();
			DoAllDocTypesWithSetup(WriteOneCFBundleDocumentType);
		WritePListEndKeyArray();

		WritePListKeyString("CFBundleExecutable", vStrAppAbbrev);
		WritePListKeyProcString("CFBundleGetInfoString",
			WriteGetInfoString);
		WritePListKeyString("CFBundleIconFile", "ICONAPPO.icns");
		WritePListKeyProcString("CFBundleIdentifier",
			WriteTheBundleIdentifier);
		WritePListKeyString("CFBundleInfoDictionaryVersion", "6.0");
		WritePListKeyProcString("CFBundleName", WriteStrAppUnabrevName);
		WritePListKeyString("CFBundlePackageType", "APPL");
		WritePListKeyProcString("CFBundleShortVersionString",
			WriteVersionStr);
		WritePListKeyProcString("CFBundleSignature",
			Write_MacCreatorSigOrGeneric);
		WritePListKeyProcString("CFBundleVersion", WriteVersionStr);
		WritePListKeyString("LSRequiresCarbon", "1");
		if (gbk_apifam_cco == gbo_apifam) {
			WritePListKeyString("NSHighResolutionCapable", "1");
		}
		if (WantGraphicsSwitching) {
			WritePListKeyString("NSSupportsAutomaticGraphicsSwitching",
				"1");
		}
		if (gbk_apifam_sd2 == gbo_apifam) {
			WritePListKeyString("SDL_FILESYSTEM_BASE_DIR_TYPE",
				"parent");
		}

	WritePListEndDict();
}

LOCALPROC WriteMainPLCData(void)
{
	/* plist source */
	WriteMainPLC(WriteMyInfoPListContents);
}

LOCALPROC WriteInfoPListData(void)
{
	/* Info.plist file */
	WriteInfoPList(WriteMyInfoPListContents);
}

LOCALPROC WritePListData(void)
{
	if (gbk_ide_mw8 == cur_ide) {
		WriteADstFile1("my_config_d",
			"main", ".plc", "plist source",
			WriteMainPLCData);
	} else
	{
		WriteADstFile1("my_config_d",
			"Info", ".plist", "plist source",
			WriteInfoPListData);
	}
}

LOCALPROC WriteEntitlementsData(void)
{
	WriteOpenDestFile("my_config_d",
		vStrAppAbbrev, ".entitlements", "entitlements");

	WriteDestFileLn("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	WriteDestFileLn(
		"<!DOCTYPE plist PUBLIC"
		" \"-//Apple//DTD PLIST 1.0//EN\""
		" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">");
	WriteDestFileLn("<plist version=\"1.0\">");
	WriteDestFileLn("<dict>");
	++DestFileIndent;
		WriteDestFileLn("<key>com.apple.security.app-sandbox</key>");
		WriteDestFileLn("<true/>");
		WriteDestFileLn("<key>"
			"com.apple.security.files.user-selected.read-write"
			"</key>");
		WriteDestFileLn("<true/>");
	--DestFileIndent;
	WriteDestFileLn("</dict>");
	WriteDestFileLn("</plist>");

	WriteCloseDestFile();
}
