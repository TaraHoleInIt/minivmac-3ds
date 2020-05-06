/*
	SPOTHRCF.i
	Copyright (C) 2008 Paul C. Pratt

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
	program SPecific write OTHer ConFiguration files
*/

LOCALPROC WriteAppCNFGGLOBContents(void)
{
	WriteCommonCNFGGLOBContents();

	if (cur_mIIorIIX) {
		Write64bitConfig();
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn(
		"/* capabilities provided by platform specific code */");

	WriteBlankLineToDestFile();

	WriteCompCondBool("MySoundEnabled", MySoundEnabled);

	WriteAppSpecificCNFGGLOBoptions();
}

LOCALPROC WriteBldOpts(void)
{
	Branch = MajorVersion;

	WrtOptGNSettings();
	WrtOptSPSettings();
}

LOCALPROC WriteAppCNFGRAPIContents(void)
{
	WriteCommonCNFGRAPIContents();

	WriteAppSpecificCNFGRAPIoptions();

	WriteBlankLineToDestFile();
	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kBldOpts \"");
	WriteBldOpts();
	WriteCStrToDestFile("\"");
	WriteEndDestFileLn();
}

LOCALPROC WriteOneWire(char *a, char *b)
{
	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("\tWire_");
	WriteCStrToDestFile(a);
	WriteCStrToDestFile("_");
	WriteCStrToDestFile(b);
	WriteCStrToDestFile(",");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define ");
	WriteCStrToDestFile(b);
	WriteCStrToDestFile(" (Wires[Wire_");
	WriteCStrToDestFile(a);
	WriteCStrToDestFile("_");
	WriteCStrToDestFile(b);
	WriteCStrToDestFile("])");
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define ");
	WriteCStrToDestFile(a);
	WriteCStrToDestFile(" (Wires[Wire_");
	WriteCStrToDestFile(a);
	WriteCStrToDestFile("_");
	WriteCStrToDestFile(b);
	WriteCStrToDestFile("])");
	WriteEndDestFileLn();
}

LOCALPROC WriteConfigureWires(void)
{
	WriteDestFileLn(
		"/* the Wire variables are 1/0, not true/false */");
	WriteBlankLineToDestFile();
	WriteDestFileLn("enum {");
	if (cur_mIIorIIX) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundDisable,");
		WriteDestFileLn(
			"#define SoundDisable (Wires[Wire_unknown_SoundDisable])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundVolb0,");
		WriteDestFileLn(
			"#define SoundVolb0 (Wires[Wire_unknown_SoundVolb0])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundVolb1,");
		WriteDestFileLn(
			"#define SoundVolb1 (Wires[Wire_unknown_SoundVolb1])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_unknown_SoundVolb2,");
		WriteDestFileLn(
			"#define SoundVolb2 (Wires[Wire_unknown_SoundVolb2])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA0_unknown,");
		WriteDestFileLn(
			"#define VIA1_iA0 (Wires[Wire_VIA1_iA0_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA1_unknown,");
		WriteDestFileLn(
			"#define VIA1_iA1 (Wires[Wire_VIA1_iA1_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA2_unknown,");
		WriteDestFileLn(
			"#define VIA1_iA2 (Wires[Wire_VIA1_iA2_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn(
			"\tWire_VIA1_iB7_unknown,"
			" /* for compatibility with SoundDisable */");
		WriteDestFileLn(
			"#define VIA1_iB7 (Wires[Wire_VIA1_iB7_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_InterruptRequest,");
		WriteDestFileLn(
			"#define VIA2_InterruptRequest"
			" (Wires[Wire_VIA2_InterruptRequest])");
		WriteDestFileLn(
			"#define VIA2_interruptChngNtfy VIAorSCCinterruptChngNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iA7_unknown,");
		WriteDestFileLn(
			"#define VIA2_iA7 (Wires[Wire_VIA2_iA7_unknown])");
		WriteDestFileLn(
			"#define VIA2_iA7_ChangeNtfy Addr32_ChangeNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iA6_unknown,");
		WriteDestFileLn(
			"#define VIA2_iA6 (Wires[Wire_VIA2_iA6_unknown])");
		WriteDestFileLn(
			"#define VIA2_iA6_ChangeNtfy Addr32_ChangeNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iB7_unknown,");
		WriteDestFileLn(
			"#define VIA2_iB7 (Wires[Wire_VIA2_iB7_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iCB2_unknown,");
		WriteDestFileLn(
			"#define VIA2_iCB2 (Wires[Wire_VIA2_iCB2_unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iB2_PowerOff,");
		WriteDestFileLn(
			"#define VIA2_iB2 (Wires[Wire_VIA2_iB2_PowerOff])");
		WriteDestFileLn(
			"#define VIA2_iB2_ChangeNtfy PowerOff_ChangeNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA2_iB3_Addr32,");
		WriteDestFileLn(
			"#define VIA2_iB3 (Wires[Wire_VIA2_iB3_Addr32])");
		WriteDestFileLn(
			"#define Addr32 (Wires[Wire_VIA2_iB3_Addr32])");
		WriteDestFileLn(
			"#define VIA2_iB3_ChangeNtfy Addr32_ChangeNtfy");
	}

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteOneWire("VIA1_iA0", "SoundVolb0");
		WriteOneWire("VIA1_iA1", "SoundVolb1");
		WriteOneWire("VIA1_iA2", "SoundVolb2");
	}

	if ((cur_mdl <= gbk_mdl_Plus) || cur_mIIorIIX)
	{
		WriteOneWire("VIA1_iA4", "MemOverlay");
		WriteDestFileLn(
			"#define VIA1_iA4_ChangeNtfy MemOverlay_ChangeNtfy");
	} else {
		if (cur_mdl <= gbk_mdl_Classic) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iA4_DriveSel,");
			WriteDestFileLn(
				"#define VIA1_iA4 (Wires[Wire_VIA1_iA4_DriveSel])");
		}
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_MemOverlay,");
		WriteDestFileLn("#define MemOverlay (Wires[Wire_MemOverlay])");
	}

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteOneWire("VIA1_iA6", "SCRNvPage2");
	}

	if (gbk_mdl_PB100 == cur_mdl) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_SCCwaitrq,");
		WriteDestFileLn("#define SCCwaitrq (Wires[Wire_SCCwaitrq])");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iA0_PmuBus0,");
		WriteDestFileLn("\tWire_VIA1_iA1_PmuBus1,");
		WriteDestFileLn("\tWire_VIA1_iA2_PmuBus2,");
		WriteDestFileLn("\tWire_VIA1_iA3_PmuBus3,");
		WriteDestFileLn("\tWire_VIA1_iA4_PmuBus4,");
		WriteDestFileLn("\tWire_VIA1_iA5_PmuBus5,");
		WriteDestFileLn("\tWire_VIA1_iA6_PmuBus6,");
		WriteDestFileLn("\tWire_VIA1_iA7_PmuBus7,");
		WriteBlankLineToDestFile();
		WriteDestFileLn(
			"#define VIA1_iA0 (Wires[Wire_VIA1_iA0_PmuBus0])");
		WriteDestFileLn(
			"#define VIA1_iA1 (Wires[Wire_VIA1_iA1_PmuBus1])");
		WriteDestFileLn(
			"#define VIA1_iA2 (Wires[Wire_VIA1_iA2_PmuBus2])");
		WriteDestFileLn(
			"#define VIA1_iA3 (Wires[Wire_VIA1_iA3_PmuBus3])");
		WriteDestFileLn(
			"#define VIA1_iA4 (Wires[Wire_VIA1_iA4_PmuBus4])");
		WriteDestFileLn(
			"#define VIA1_iA5 (Wires[Wire_VIA1_iA5_PmuBus5])");
		WriteDestFileLn(
			"#define VIA1_iA6 (Wires[Wire_VIA1_iA6_PmuBus6])");
		WriteDestFileLn(
			"#define VIA1_iA7 (Wires[Wire_VIA1_iA7_PmuBus7])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB0_PmuToReady,");
		WriteDestFileLn(
			"#define VIA1_iB0 (Wires[Wire_VIA1_iB0_PmuToReady])");
		WriteDestFileLn(
			"#define PmuToReady (Wires[Wire_VIA1_iB0_PmuToReady])");
		WriteDestFileLn(
			"#define VIA1_iB0_ChangeNtfy PmuToReady_ChangeNtfy");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB1_PmuFromReady,");
		WriteDestFileLn(
			"#define VIA1_iB1 (Wires[Wire_VIA1_iB1_PmuFromReady])");
		WriteDestFileLn(
			"#define PmuFromReady (Wires[Wire_VIA1_iB1_PmuFromReady])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB2_Unknown,");
		WriteDestFileLn(
			"#define VIA1_iB2 (Wires[Wire_VIA1_iB2_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB3_Unknown,");
		WriteDestFileLn(
			"#define VIA1_iB3 (Wires[Wire_VIA1_iB3_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB4_Unknown,");
		WriteDestFileLn(
			"#define VIA1_iB4 (Wires[Wire_VIA1_iB4_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB5_Unknown,");
		WriteDestFileLn(
			"#define VIA1_iB5 (Wires[Wire_VIA1_iB5_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB6_Unknown,");
		WriteDestFileLn(
			"#define VIA1_iB6 (Wires[Wire_VIA1_iB6_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB7_Unknown,");
		WriteDestFileLn(
			"#define VIA1_iB7 (Wires[Wire_VIA1_iB7_Unknown])");
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iCB2_Unknown,");
		WriteDestFileLn(
			"#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_Unknown])");
	} else {
		WriteOneWire("VIA1_iA5", "IWMvSel");

		WriteOneWire("VIA1_iA7", "SCCwaitrq");

		WriteOneWire("VIA1_iB0", "RTCdataLine");
		WriteDestFileLn(
			"#define VIA1_iB0_ChangeNtfy RTCdataLine_ChangeNtfy");

		WriteOneWire("VIA1_iB1", "RTCclock");
		WriteDestFileLn(
			"#define VIA1_iB1_ChangeNtfy RTCclock_ChangeNtfy");

		WriteOneWire("VIA1_iB2", "RTCunEnabled");
		WriteDestFileLn(
			"#define VIA1_iB2_ChangeNtfy RTCunEnabled_ChangeNtfy");

		if (cur_mdl <= gbk_mdl_Plus) {
			WriteOneWire("VIA1_iA3", "SoundBuffer");
			WriteOneWire("VIA1_iB3", "MouseBtnUp");
			WriteOneWire("VIA1_iB4", "MouseX2");
			WriteOneWire("VIA1_iB5", "MouseY2");

			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iCB2_KybdDat,");
			WriteDestFileLn(
				"#define VIA1_iCB2 (Wires[Wire_VIA1_iCB2_KybdDat])");
			WriteDestFileLn(
				"#define VIA1_iCB2_ChangeNtfy Kybd_DataLineChngNtfy");
		} else {
			WriteBlankLineToDestFile();
			WriteDestFileLn("\tWire_VIA1_iA3_SCCvSync,");
			WriteDestFileLn(
				"#define VIA1_iA3 (Wires[Wire_VIA1_iA3_SCCvSync])");

			WriteOneWire("VIA1_iB3", "ADB_Int");

			WriteOneWire("VIA1_iB4", "ADB_st0");
			WriteDestFileLn(
				"#define VIA1_iB4_ChangeNtfy ADBstate_ChangeNtfy");

			WriteOneWire("VIA1_iB5", "ADB_st1");
			WriteDestFileLn(
				"#define VIA1_iB5_ChangeNtfy ADBstate_ChangeNtfy");

			WriteOneWire("VIA1_iCB2", "ADB_Data");
			WriteDestFileLn(
				"#define VIA1_iCB2_ChangeNtfy ADB_DataLineChngNtfy");
		}
	}

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteOneWire("VIA1_iB6", "SCRNbeamInVid");
	} else if (cur_mdl <= gbk_mdl_Classic) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VIA1_iB6_SCSIintenable,");
		WriteDestFileLn(
			"#define VIA1_iB6 (Wires[Wire_VIA1_iB6_SCSIintenable])");
	}

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteOneWire("VIA1_iB7", "SoundDisable");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("\tWire_VIA1_InterruptRequest,");
	WriteDestFileLn(
		"#define VIA1_InterruptRequest"
		" (Wires[Wire_VIA1_InterruptRequest])");
	WriteDestFileLn(
		"#define VIA1_interruptChngNtfy VIAorSCCinterruptChngNtfy");

	WriteBlankLineToDestFile();
	WriteDestFileLn("\tWire_SCCInterruptRequest,");
	WriteDestFileLn(
		"#define SCCInterruptRequest"
		" (Wires[Wire_SCCInterruptRequest])");
	WriteDestFileLn(
		"#define SCCinterruptChngNtfy VIAorSCCinterruptChngNtfy");

	if (cur_mdl <= gbk_mdl_Plus) {
	} else {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_ADBMouseDisabled,");
		WriteDestFileLn(
			"#define ADBMouseDisabled (Wires[Wire_ADBMouseDisabled])");
	}

	if (cur_mIIorIIX) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VBLinterrupt,");
		WriteDestFileLn(
			"#define Vid_VBLinterrupt (Wires[Wire_VBLinterrupt])");
		WriteDestFileLn("#define VIA2_iA0 (Wires[Wire_VBLinterrupt])");

		WriteBlankLineToDestFile();
		WriteDestFileLn("\tWire_VBLintunenbl,");
		WriteDestFileLn(
			"#define Vid_VBLintunenbl (Wires[Wire_VBLintunenbl])");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("\tkNumWires");
	WriteDestFileLn("};");
}

LOCALPROC WriteConfigureVIA1(void)
{
	WriteDestFileLn("/* VIA configuration */");

	if (gbk_mdl_Classic == cur_mdl) {
		WriteDestFileLn("#define VIA1_ORA_FloatVal 0xF7");
	} else if (cur_mIIorIIX) {
		WriteDestFileLn("#define VIA1_ORA_FloatVal 0xBF");
		WriteDestFileLn(
			"\t/* bit 6 used to check version of hardware */");
	} else {
		WriteDestFileLn("#define VIA1_ORA_FloatVal 0xFF");
	}

	WriteDestFileLn("#define VIA1_ORB_FloatVal 0xFF");

	if (cur_mdl <= gbk_mdl_Classic) {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0x80");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0x7F");
	} else if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0xFF");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0xFF");
	} else if (cur_mIIorIIX) {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0x80");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0x3F");
	} else {
		WriteDestFileLn("#define VIA1_ORA_CanIn 0x00");
		WriteDestFileLn("#define VIA1_ORA_CanOut 0x00");
	}

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x79");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0x87");
	} else if (cur_mdl <= gbk_mdl_Classic) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x09");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0xF7");
	} else if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x02");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0xFD");
	} else if (cur_mIIorIIX) {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x09");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0xB7");
	} else {
		WriteDestFileLn("#define VIA1_ORB_CanIn 0x00");
		WriteDestFileLn("#define VIA1_ORB_CanOut 0x00");
	}

	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define VIA1_IER_Never0 (1 << 1)");
		WriteDestFileLn(
			"#define VIA1_IER_Never1 ((1 << 3) | (1 << 4))");
	} else if (cur_mdl <= gbk_mdl_Classic) {
		WriteDestFileLn("#define VIA1_IER_Never0 0x00");
		WriteDestFileLn(
			"#define VIA1_IER_Never1 ((1 << 3) | (1 << 4))");
	} else if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_IER_Never0 0x00");
		WriteDestFileLn("#define VIA1_IER_Never1 0x0C");
	} else if (cur_mIIorIIX) {
		WriteDestFileLn("#define VIA1_IER_Never0 0x00");
		WriteDestFileLn("#define VIA1_IER_Never1 0x58");
	} else {
		WriteDestFileLn("#define VIA1_IER_Never0 0xFF");
		WriteDestFileLn("#define VIA1_IER_Never1 0xFF");
	}

	if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define VIA1_CB2modesAllowed 0x03");
		WriteDestFileLn("#define VIA1_CA2modesAllowed 0x03");
	} else {
		WriteDestFileLn("#define VIA1_CB2modesAllowed 0x01");
		WriteDestFileLn("#define VIA1_CA2modesAllowed 0x01");
	}
}

LOCALPROC WriteConfigureVIA2(void)
{
	WriteDestFileLn("/* VIA 2 configuration */");

	WriteDestFileLn("#define VIA2_ORA_FloatVal 0xFF");
	WriteDestFileLn("#define VIA2_ORB_FloatVal 0xFF");

	if (cur_mIIorIIX) {
		WriteDestFileLn("#define VIA2_ORA_CanIn 0x01");
		WriteDestFileLn("#define VIA2_ORA_CanOut 0xC0");
	} else {
		WriteDestFileLn("#define VIA2_ORA_CanIn 0x00");
		WriteDestFileLn("#define VIA2_ORA_CanOut 0x00");
	}

	if (cur_mIIorIIX) {
		WriteDestFileLn("#define VIA2_ORB_CanIn 0x00");
		WriteDestFileLn("#define VIA2_ORB_CanOut 0x8C");
	} else {
		WriteDestFileLn("#define VIA2_ORB_CanIn 0x00");
		WriteDestFileLn("#define VIA2_ORB_CanOut 0x00");
	}

	if (cur_mIIorIIX) {
		WriteDestFileLn("#define VIA2_IER_Never0 0x00");
		WriteDestFileLn("#define VIA2_IER_Never1 0xED");
	} else {
		WriteDestFileLn("#define VIA2_IER_Never0 0xFF");
		WriteDestFileLn("#define VIA2_IER_Never1 0xFF");
	}

	WriteDestFileLn("#define VIA2_CB2modesAllowed 0x01");
	WriteDestFileLn("#define VIA2_CA2modesAllowed 0x01");
}

LOCALPROC WriteAppSTRCONSTcontents(void)
{
	char *s;

	switch (gbo_lang) {
		case gbk_lang_eng:
			s = "ENG";
			break;
		case gbk_lang_fre:
			s = "FRE";
			break;
		case gbk_lang_ita:
			s = "ITA";
			break;
		case gbk_lang_ger:
			s = "GER";
			break;
		case gbk_lang_dut:
			s = "DUT";
			break;
		case gbk_lang_spa:
			s = "SPA";
			break;
		case gbk_lang_pol:
			s = "POL";
			break;
		case gbk_lang_ptb:
			s = "PTB";
			break;
		case gbk_lang_cat:
			s = "CAT";
			break;
		case gbk_lang_cze:
			s = "CZE";
			break;
		case gbk_lang_srl:
			s = "SRL";
			break;
		default:
			s = "???";
			break;
	}

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#include ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("STRCN");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".h");
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteAppSOUNDGLUcontents(void)
{
	char *s;

	switch (gbo_sndapi) {
		case gbk_sndapi_alsa:
			s = "ALSA";
			break;
		case gbk_sndapi_ddsp:
			s = "DDSP";
			break;
		default:
			s = "???";
			break;
	}

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#include ");
	WriteQuoteToDestFile();
	WriteCStrToDestFile("SGLU");
	WriteCStrToDestFile(s);
	WriteCStrToDestFile(".h");
	WriteQuoteToDestFile();
	WriteEndDestFileLn();
}

LOCALPROC WriteAppEMCONFIGcontents(void)
{
	WriteDestFileLn("/*");
	++DestFileIndent;
		WriteDestFileLn(
			"Configuration options used by platform independent code.");
		WriteConfigurationWarning();
	--DestFileIndent;
	WriteDestFileLn("*/");

	WriteBlankLineToDestFile();

	WriteCompCondBool("EmClassicKbrd", EmClassicKbrd);
	WriteCompCondBool("EmADB", EmADB);
	WriteCompCondBool("EmRTC", EmRTC);
	WriteCompCondBool("EmPMU", EmPMU);
	WriteCompCondBool("EmVIA2", EmVIA2);
	WriteCompCondBool("Use68020", em_cpu_vers >= 2);
	WriteCompCondBool("EmFPU",
		cur_mIIorIIX);
	WriteCompCondBool("EmMMU", falseblnr);
	WriteCompCondBool("EmASC", EmASC);

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define CurEmMd kEmMd_");
	WriteCStrToDestFile(GetModelName(cur_mdl));
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kMyClockMult ");
	if (cur_mIIorIIX) {
		WriteCStrToDestFile("2");
	} else {
		WriteCStrToDestFile("1");
	}
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();
	WriteCompCondBool("WantCycByPriOp", timingacc != 0);
	WriteCompCondBool("WantCloserCyc", timingacc >= 2);

	if (gbk_ide_mvc == cur_ide) {
		if (gbk_cpufam_x64 == gbo_cpufam) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("#define r_pc_p \"r15\"");
			WriteDestFileLn("#define r_MaxCyclesToGo \"r14\"");
			WriteDestFileLn("#define r_pc_pHi \"r13\"");
		}

		if (gbk_cpufam_ppc == gbo_cpufam) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("#define r_regs \"r14\"");
			WriteDestFileLn("#define r_pc_p \"r15\"");
			WriteDestFileLn("#define r_MaxCyclesToGo \"r16\"");
			WriteDestFileLn("#define r_pc_pHi \"r17\"");
		}

		if (gbk_cpufam_arm == gbo_cpufam) {
			WriteBlankLineToDestFile();
			WriteDestFileLn("#define r_regs \"r4\"");
			WriteDestFileLn("#define r_pc_p \"r5\"");
			if (gbk_targ_wcar != cur_targ) {
				WriteDestFileLn("#define r_MaxCyclesToGo \"r6\"");
				WriteDestFileLn("#define r_pc_pHi \"r7\"");
			}
		}
	}

	WriteBlankLineToDestFile();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kRAMa_Size ");
	WriteCStrToDestFile("0x");
	WriteHexLongToOutput(1 << RAMa_Size);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define kRAMb_Size ");
	if (0 == RAMb_Size) {
		WriteCStrToDestFile("0");
	} else {
		WriteCStrToDestFile("0x");
		WriteHexLongToOutput(1 << RAMb_Size);
	}
	WriteEndDestFileLn();

	WriteBlankLineToDestFile();

	if (NeedScrnHack) {
		WriteCompCondBool("UseLargeScreenHack", NeedScrnHack);
	}
	WriteCompCondBool("IncludeVidMem", NeedVidMem);
	if (NeedVidMem) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define kVidMemRAM_Size ");
		WriteCStrToDestFile("0x");
		WriteHexLongToOutput(VidMemSize);
		WriteEndDestFileLn();
	}

	WriteBlankLineToDestFile();

	WriteCompCondBool("EmVidCard", EmVidCard);
	if (EmVidCard) {
		WriteDestFileLn("#define kVidROM_Size 0x000800");
	}

	WriteBlankLineToDestFile();

	if (cur_mIIorIIX) {
		WriteDestFileLn("#define MaxATTListN 20");
	} else {
		WriteDestFileLn("#define MaxATTListN 16");
	}

	WriteCompCondBool("IncludeExtnPbufs",
		(! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam)
		&& (gbk_apifam_nds != gbo_apifam)
		&& (gbk_apifam_3ds != gbo_apifam));
	WriteCompCondBool("IncludeExtnHostTextClipExchange",
		(! WantMinExtn) && (gbk_apifam_gtk != gbo_apifam)
		&& (gbk_apifam_sdl != gbo_apifam)
		&& (gbk_apifam_nds != gbo_apifam)
		&& (gbk_apifam_3ds != gbo_apifam));

	WriteBlankLineToDestFile();

	WriteCompCondBool("Sony_SupportDC42", SonySupportDC42);
	WriteCompCondBool("Sony_SupportTags", SonySupportTags);
	WriteCompCondBool("Sony_WantChecksumsUpdated",
		SonyWantChecksumsUpdated);
	WriteDestFileLn("#define Sony_VerifyChecksums 0");

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define CaretBlinkTime 0x");
	WriteHexByteToOutput(cur_CaretBlinkTime);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define SpeakerVol 0x");
	WriteHexByteToOutput(cur_SpeakerVol);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define DoubleClickTime 0x");
	WriteHexByteToOutput(cur_DoubleClickTime);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define MenuBlink 0x");
	WriteHexByteToOutput(cur_MenuBlink);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define AutoKeyThresh 0x");
	WriteHexByteToOutput(cur_AutoKeyThresh);
	WriteEndDestFileLn();

	WriteBgnDestFileLn();
	WriteCStrToDestFile("#define AutoKeyRate 0x");
	WriteHexByteToOutput(cur_AutoKeyRate);
	WriteEndDestFileLn();

	if (cur_mIIorIIX) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define pr_HilColRed 0x");
		WriteHexWordToOutput(cur_HilColRed);
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define pr_HilColGreen 0x");
		WriteHexWordToOutput(cur_HilColGreen);
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define pr_HilColBlue 0x");
		WriteHexWordToOutput(cur_HilColBlue);
		WriteEndDestFileLn();
	}

	if (! WantAutoLocation) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define CurMacLatitude 0x");
		WriteHexLongToOutput(cur_InitLatitude);
		WriteEndDestFileLn();

		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define CurMacLongitude 0x");
		WriteHexLongToOutput(cur_InitLongitude);
		WriteEndDestFileLn();
	}
	if (! WantAutoTimeZone) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define CurMacDelta 0x");
		WriteHexLongToOutput(((WantTzDST ? 1UL : 0) << 31)
			| (((ui5r)cur_TzDeltS) & 0x00FFFFFF));
		WriteEndDestFileLn();
	}

	WriteBlankLineToDestFile();
	WriteBlankLineToDestFile();

	WriteConfigureWires();

	WriteBlankLineToDestFile();



	WriteBlankLineToDestFile();

	WriteConfigureVIA1();


	if (EmVIA2) {
		WriteBlankLineToDestFile();
		WriteConfigureVIA2();
	}

	WriteBlankLineToDestFile();
	if (cur_mdl <= gbk_mdl_Plus) {
		WriteDestFileLn("#define Mouse_Enabled SCC_InterruptsEnabled");
	} else {
		WriteDestFileLn("#define Mouse_Enabled() (! ADBMouseDisabled)");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn(
		"#define VIA1_iCA1_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy");
	WriteDestFileLn(
		"#define Sixtieth_PulseNtfy VIA1_iCA1_Sixtieth_PulseNtfy");

	WriteBlankLineToDestFile();
	WriteDestFileLn(
		"#define VIA1_iCA2_PulseNtfy"
		" VIA1_iCA2_RTC_OneSecond_PulseNtfy");
	WriteDestFileLn(
		"#define RTC_OneSecond_PulseNtfy"
		" VIA1_iCA2_RTC_OneSecond_PulseNtfy");

	if (cur_mIIorIIX) {
		WriteBlankLineToDestFile();
		WriteDestFileLn(
			"#define VIA2_iCA1_PulseNtfy"
			" VIA2_iCA1_Vid_VBLinterrupt_PulseNtfy");
		WriteDestFileLn(
			"#define Vid_VBLinterrupt_PulseNotify"
			" VIA2_iCA1_Vid_VBLinterrupt_PulseNtfy");

		WriteBlankLineToDestFile();
		WriteDestFileLn(
			"#define VIA2_iCB1_PulseNtfy"
			" VIA2_iCB1_ASC_interrupt_PulseNtfy");
		WriteDestFileLn(
			"#define ASC_interrupt_PulseNtfy"
			" VIA2_iCB1_ASC_interrupt_PulseNtfy");
	}

	WriteBlankLineToDestFile();
	WriteDestFileLn("#define GetSoundInvertTime VIA1_GetT1InvertTime");

	if (EmClassicKbrd) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define KYBD_ShiftInData VIA1_ShiftOutData");
		WriteDestFileLn("#define KYBD_ShiftOutData VIA1_ShiftInData");
	}
	if (EmADB) {
		WriteBlankLineToDestFile();
		WriteDestFileLn("#define ADB_ShiftInData VIA1_ShiftOutData");
		WriteDestFileLn("#define ADB_ShiftOutData VIA1_ShiftInData");
	}

	if (! WantDisableRomCheck) {
		WriteCompCondBool("DisableRomCheck", WantDisableRomCheck);
	}
	if (! WantDisableRamTest) {
		WriteCompCondBool("DisableRamTest", WantDisableRamTest);
	}

	if (gbk_AHM_none != cur_AltHappyMac) {
		WriteBgnDestFileLn();
		WriteCStrToDestFile("#define CurAltHappyMac kAHM_");
		WriteCStrToDestFile(GetAltHappyMacName(cur_AltHappyMac));
		WriteEndDestFileLn();
	}

	WriteBlankLineToDestFile();
	if (cur_mIIorIIX) {
		WriteDestFileLn("#define kExtn_Block_Base 0x50F0C000");
	} else {
		WriteDestFileLn("#define kExtn_Block_Base 0x00F40000");
	}
	WriteDestFileLn("#define kExtn_ln2Spc 5");

	WriteBlankLineToDestFile();
	if (gbk_mdl_PB100 == cur_mdl) {
		WriteDestFileLn("#define kROM_Base 0x00900000");
	} else if (cur_mIIorIIX) {
		WriteDestFileLn("#define kROM_Base 0x00800000");
	} else {
		WriteDestFileLn("#define kROM_Base 0x00400000");
	}
	WriteDestFileLn("#define kROM_ln2Spc 20");

	WriteBlankLineToDestFile();
	WriteCompCondBool("WantDisasm", WantDisasm);
	WriteCompCondBool("ExtraAbnormalReports", falseblnr);
}

LOCALPROC WriteAppSpecificConfigFiles(void)
{
	WriteADstFile1("my_config_d",
		"CNFGGLOB", ".h", "C Configuration file",
		WriteAppCNFGGLOBContents);
	WriteADstFile1("my_config_d",
		"CNFGRAPI", ".h", "C API Configuration file",
		WriteAppCNFGRAPIContents);
	WriteADstFile1("my_config_d",
		"EMCONFIG", ".h", "C Platform Independent Configuration file",
		WriteAppEMCONFIGcontents);
	WriteADstFile1("my_config_d",
		"STRCONST", ".h", "Language Configuration file",
		WriteAppSTRCONSTcontents);

	if (gbk_sndapi_none != gbo_sndapi) {
		WriteADstFile1("my_config_d",
			"SOUNDGLU", ".h", "Sound Configuration file",
			WriteAppSOUNDGLUcontents);
	}
}
