/*
	WRTEXTFL.i
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
	WRite TEXT FiLe
*/



/* --- routines for writing text files --- */


#define WriteCharToOutput putchar

LOCALPROC WriteCStrToOutput(char *s)
{
	printf("%s", s);
}

LOCALPROC WriteSignedLongToOutput(long int v)
{
	printf("%ld", v);
}

LOCALPROC WriteUnsignedToOutput(unsigned int v)
{
	printf("%u", v);
}

LOCALPROC WriteDec2CharToOutput(int v)
{
	printf("%02u", v);
}

LOCALPROC WriteHexByteToOutput(unsigned int v)
{
	printf("%02X", v);
}

LOCALPROC WriteHexWordToOutput(unsigned int v)
{
	printf("%04X", v);
}

LOCALPROC WriteHexLongToOutput(ui5r v)
{
	printf("%08lX", v);
}

LOCALPROC WriteEolToOutput(void)
{
	printf("\n");
}

LOCALPROC WriteLnCStrToOutput(char *s)
{
	WriteCStrToOutput(s);
	WriteEolToOutput();
}


/* --- code specific to Scripting Language --- */

GLOBALPROC WriteScriptLangExtension(void)
{
#if (gbo_script == gbk_script_mpw)
	WriteCStrToOutput(".mpw");
#endif
#if (gbo_script == gbk_script_applescript)
	WriteCStrToOutput(".scpt");
#endif
#if (gbo_script == gbk_script_bash)
	WriteCStrToOutput(".sh");
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteCStrToOutput(".vbs");
#endif
#if (gbo_script == gbk_script_xp)
	WriteCStrToOutput(".bat");
#endif
}

GLOBALPROC WriteScriptLangHeader(void)
{
#if (gbo_script == gbk_script_bash)
	WriteLnCStrToOutput("#! /bin/bash");
	WriteEolToOutput();
#endif
#if (gbo_script == gbk_script_xp)
	WriteLnCStrToOutput("@echo off");
	WriteEolToOutput();
#endif
}

#if (gbo_script == gbk_script_bash)
#ifndef BashUsePrintf
#define BashUsePrintf 1
#endif
#endif

GLOBALPROC WriteSectionCommentDestFile(char * Description)
{
	WriteEolToOutput();
	WriteEolToOutput();
#if (gbo_script == gbk_script_mpw) || (gbo_script == gbk_script_bash)
	{
		WriteCStrToOutput("# ----- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif
#if (gbo_script == gbk_script_applescript)
	{
		WriteCStrToOutput("\t--- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif
#if (gbo_script == gbk_script_vbscript)
	{
		WriteCStrToOutput("' ----- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif
#if (gbo_script == gbk_script_xp)
	{
		WriteCStrToOutput("rem ----- ");
		WriteCStrToOutput(Description);
		WriteCStrToOutput(" -----");
	}
#endif

	WriteEolToOutput();
}

LOCALPROC WriteOpenDestFile(char *DirVar, char *FileName, char *FileExt,
	char * Description)
{
	WriteSectionCommentDestFile(Description);

	WriteEolToOutput();

#if (gbo_script == gbk_script_mpw)
	{
#if MPWOneEchoPerFile
		WriteCStrToOutput("Echo -n > \"{");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput("}");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\" \266");
		WriteEolToOutput();
#else
		WriteCStrToOutput("Set DestFile \"{");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput("}");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\"");
		WriteEolToOutput();
		WriteLnCStrToOutput("Echo -n > \"{DestFile}\"");
		WriteEolToOutput();
#endif
	}
#endif
#if (gbo_script == gbk_script_applescript)
	{
		WriteCStrToOutput("\tset DestFile to open for access file (");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput(" & \"");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\") with write permission");
		WriteEolToOutput();
		WriteEolToOutput();
		WriteLnCStrToOutput("\tset eof DestFile to 0");
	}
#endif
#if (gbo_script == gbk_script_bash)
	{
		WriteCStrToOutput("DestFile=\"${");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput("}");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\"");
		WriteEolToOutput();
#if BashUsePrintf
		WriteLnCStrToOutput("printf \"\" > \"${DestFile}\"");
#else
		/* WriteLnCStrToOutput("echo -n > \"${DestFile}\""); */
		WriteLnCStrToOutput("true > \"${DestFile}\"");
#endif
		WriteEolToOutput();
	}
#endif
#if (gbo_script == gbk_script_xp)
	{
		WriteCStrToOutput("set DestFile=%");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput("%");
		WriteCStrToOutput("\\");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteEolToOutput();
		WriteLnCStrToOutput("echo.>\"%DestFile%\"");
		WriteLnCStrToOutput("del \"%DestFile%\"");
	}
#endif
#if (gbo_script == gbk_script_vbscript)
	{
		WriteCStrToOutput("Set f = fso.CreateTextFile(");
		WriteCStrToOutput(DirVar);
		WriteCStrToOutput(" & \"\\");
		WriteCStrToOutput(FileName);
		WriteCStrToOutput(FileExt);
		WriteCStrToOutput("\", True)");
		WriteEolToOutput();
		WriteEolToOutput();
	}
#endif
}

LOCALPROC WriteCloseDestFile(void)
{
#if (gbo_script == gbk_script_mpw)
#if MPWOneEchoPerFile
	WriteLnCStrToOutput("''");
#endif
#endif /* (gbo_script == gbk_script_mpw) */
#if (gbo_script == gbk_script_applescript)
	WriteEolToOutput();
	WriteLnCStrToOutput("\tclose access DestFile");
#endif
#if (gbo_script == gbk_script_bash)
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteEolToOutput();
	WriteLnCStrToOutput("f.Close");
#endif
#if (gbo_script == gbk_script_xp)
#endif
}

TYPEDEFPROC (*MyProc)(void);

LOCALPROC WriteADstFile1(char *DirVar,
	char *FileName, char *FileExt, char * Description, MyProc p)
{
	WriteOpenDestFile(DirVar, FileName, FileExt, Description);
	p();
	WriteCloseDestFile();
}

LOCALPROC WriteBlankLineToDestFile(void)
{
#if (gbo_script == gbk_script_mpw)
#if MPWOneEchoPerFile
	WriteLnCStrToOutput("''\266n\266");
#else
	WriteLnCStrToOutput("Echo '' >> \"{DestFile}\"");
#endif
#endif /* (gbo_script == gbk_script_mpw) */
#if (gbo_script == gbk_script_applescript)
	WriteLnCStrToOutput("\twrite \"\" & return to DestFile");
#endif
#if (gbo_script == gbk_script_bash)
#if BashUsePrintf
	WriteLnCStrToOutput("printf \"\\n\" >> \"${DestFile}\"");
#else
	WriteLnCStrToOutput("echo '' >> \"${DestFile}\"");
#endif
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteLnCStrToOutput("f.WriteLine(\"\")");
#endif
#if (gbo_script == gbk_script_xp)
	WriteLnCStrToOutput("echo.>>\"%DestFile%\"");
#endif
}

LOCALVAR int DestFileIndent = 0;

LOCALPROC WriteBgnDestFileLn(void)
{
	int i;

#if (gbo_script == gbk_script_mpw)
#if MPWOneEchoPerFile
	WriteCStrToOutput("'");
#else
	WriteCStrToOutput("Echo '");
#endif
#endif /* (gbo_script == gbk_script_mpw) */
#if (gbo_script == gbk_script_applescript)
	WriteCStrToOutput("\twrite \"");
#endif
#if (gbo_script == gbk_script_bash)
#if BashUsePrintf
	WriteCStrToOutput("printf \"%s\\n\" '");
#else
	WriteCStrToOutput("echo '");
#endif
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteCStrToOutput("f.WriteLine(\"");
#endif
#if (gbo_script == gbk_script_xp)
	WriteCStrToOutput("echo ");
#endif

	for (i = 0; i < DestFileIndent; ++i) {
		WriteCStrToOutput("\t");
	}
}

LOCALPROC WriteEndDestFileLn(void)
{
#if (gbo_script == gbk_script_mpw)
#if MPWOneEchoPerFile
	WriteCStrToOutput("'\266n\266");
#else
	WriteCStrToOutput("' >> \"{DestFile}\"");
#endif
#endif /* (gbo_script == gbk_script_mpw) */
#if (gbo_script == gbk_script_applescript)
	WriteCStrToOutput("\" & return to DestFile");
#endif
#if (gbo_script == gbk_script_bash)
	WriteCStrToOutput("' >> \"${DestFile}\"");
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteCStrToOutput("\")");
#endif
#if (gbo_script == gbk_script_xp)
	WriteCStrToOutput(">>\"%DestFile%\"");
#endif

	WriteEolToOutput();
}

LOCALPROC WriteCharToDestFile(char c)
{
	switch (c) {
#if (gbo_script == gbk_script_mpw)
		case '\'':
			WriteCStrToOutput("'\266''");
			break;
#endif
#if (gbo_script == gbk_script_bash)
		case '\'':
			WriteCStrToOutput("'\\''");
			break;
#endif
#if (gbo_script == gbk_script_applescript)
		case '"':
			WriteCStrToOutput("\\\"");
			break;
#endif
#if (gbo_script == gbk_script_vbscript)
		case '"':
			WriteCStrToOutput("\"\"");
			break;
#endif
#if (gbo_script == gbk_script_xp)
		case '%':
			WriteCStrToOutput("%%");
			break;
		case '^':
		case '<':
		case '>':
		case '|':
		case '"':

			/*
				digit preceeding redirection
				modifies the redirection
			*/
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			WriteCStrToOutput("^");
			WriteCharToOutput(c);
			break;
#endif
		default:
			WriteCharToOutput(c);
			break;
	}
}

LOCALPROC WriteCStrToDestFile(char *s)
{
	char c;

	while ((c = *s++) != 0) {
		WriteCharToDestFile(c);
	}
}

LOCALPROC WriteScriptVarToDestFile(char *name)
{
#if (gbo_script == gbk_script_mpw)
	WriteCStrToOutput("'\"{");
	WriteCStrToOutput(name);
	WriteCStrToOutput("}\"'");
#endif
#if (gbo_script == gbk_script_applescript)
	WriteCStrToOutput("* not implemented yet *");
#endif
#if (gbo_script == gbk_script_bash)
	WriteCStrToOutput("'\"${");
	WriteCStrToOutput(name);
	WriteCStrToOutput("}\"'");
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteCStrToOutput("\" & ");
	WriteCStrToOutput(name);
	WriteCStrToOutput(" & \"");
#endif
#if (gbo_script == gbk_script_xp)
	WriteCStrToOutput("%");
	WriteCStrToOutput(name);
	WriteCStrToOutput("%");
#endif
}

LOCALPROC WriteBgnCommentBlock(void)
{
#if (gbo_script == gbk_script_applescript)
	WriteLnCStrToOutput("(*");
#endif
}

LOCALPROC WriteEndCommentBlock(void)
{
#if (gbo_script == gbk_script_applescript)
	WriteLnCStrToOutput("*)");
#endif
}

LOCALPROC WriteBgnCommentBlockLn(void)
{
#if (gbo_script == gbk_script_mpw) || (gbo_script == gbk_script_bash)
	WriteCStrToOutput("# ");
#endif
#if (gbo_script == gbk_script_applescript)
	WriteCStrToOutput("\t");
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteCStrToOutput("' ");
#endif
#if (gbo_script == gbk_script_xp)
	WriteCStrToOutput("rem ");
#endif
}

LOCALPROC WriteEndCommentBlockLn(void)
{
#if (gbo_script == gbk_script_mpw) \
	|| (gbo_script == gbk_script_applescript) \
	|| (gbo_script == gbk_script_bash) \
	|| (gbo_script == gbk_script_vbscript) \
	|| (gbo_script == gbk_script_xp)
	WriteEolToOutput();
#endif
}

LOCALPROC WriteCommentBlockLn(char *s)
{
	WriteBgnCommentBlockLn();
	WriteCStrToOutput(s);
	WriteEndCommentBlockLn();
}

LOCALPROC FindSubDirectory(char *new_d, char *parent_d, char *name)
{
	WriteEolToOutput();

#if (gbo_script == gbk_script_mpw)
	WriteCStrToOutput("Set ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(" \"{");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("}");
	WriteCStrToOutput(name);
	WriteCStrToOutput(":\"");
	WriteEolToOutput();


	WriteCStrToOutput("IF not \"`exists -d -q \"{");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("}\"`\"");
	WriteEolToOutput();

	WriteCStrToOutput("\tEcho \"{");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("} is not an existing directory\"");
	WriteEolToOutput();

	WriteLnCStrToOutput("\tExit 1");

	WriteLnCStrToOutput("END");
#endif
#if (gbo_script == gbk_script_applescript)
	WriteCStrToOutput("\tset ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(" to ");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput(" & \"");
	WriteCStrToOutput(name);
	WriteCStrToOutput(":\"");
	WriteEolToOutput();

	WriteCStrToOutput("\tif not (exists alias ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(") then");
	WriteEolToOutput();

	WriteCStrToOutput("\t\tdisplay dialog \"The folder '");
	WriteCStrToOutput(name);
	WriteCStrToOutput("' is missing from '\" & ");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput(" & \"'.\" buttons {\"OK\"} default button 1");
	WriteEolToOutput();

	WriteLnCStrToOutput("\t\treturn");
	WriteLnCStrToOutput("\tend if");
#endif
#if (gbo_script == gbk_script_bash)
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("=\"${");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("}");
	WriteCStrToOutput(name);
	WriteCStrToOutput("/\"");
	WriteEolToOutput();

	WriteCStrToOutput("if test ! -d \"${");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("}\" ; then");
	WriteEolToOutput();

	WriteCStrToOutput("\techo \"The folder '");
	WriteCStrToOutput(name);
	WriteCStrToOutput("' is missing from ${");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("}\"");
	WriteEolToOutput();

	WriteLnCStrToOutput("\texit 1");
	WriteLnCStrToOutput("fi");
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteCStrToOutput("dim ");
	WriteCStrToOutput(new_d);
	WriteEolToOutput();

	WriteCStrToOutput(new_d);
	WriteCStrToOutput(" = ");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput(" & \"\\");
	WriteCStrToOutput(name);
	WriteCStrToOutput("\"");
	WriteEolToOutput();
#endif
#if (gbo_script == gbk_script_xp)
	WriteCStrToOutput("set ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("=%");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("%");
	WriteCStrToOutput("\\");
	WriteCStrToOutput(name);
	WriteEolToOutput();

	WriteCStrToOutput("if not exist \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\" echo \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\" is missing");
	WriteEolToOutput();
#endif
}

LOCALPROC MakeSubDirectory(char *new_d, char *parent_d, char *name,
	char *FileExt)
{
	WriteEolToOutput();

#if (gbo_script == gbk_script_mpw)
	WriteCStrToOutput("Set ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(" \"{");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("}");
	WriteCStrToOutput(name);
	WriteCStrToOutput(FileExt);
	WriteCStrToOutput(":\"");
	WriteEolToOutput();

	WriteCStrToOutput("IF not \"`exists -q \"{");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("}\"`\"");
	WriteEolToOutput();

	WriteCStrToOutput("\tNewFolder \"{");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("}\"");
	WriteEolToOutput();

	WriteLnCStrToOutput("END");
#endif
#if (gbo_script == gbk_script_applescript)
	WriteCStrToOutput("\tset ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(" to ");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput(" & \"");
	WriteCStrToOutput(name);
	WriteCStrToOutput(FileExt);
	WriteCStrToOutput(":\"");
	WriteEolToOutput();

	WriteCStrToOutput("\tmake new folder at alias ");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput(" with properties {name:\"");
	WriteCStrToOutput(name);
	WriteCStrToOutput(FileExt);
	WriteCStrToOutput("\"}");
	WriteEolToOutput();
#endif
#if (gbo_script == gbk_script_bash)
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("=\"${");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("}");
	WriteCStrToOutput(name);
	WriteCStrToOutput(FileExt);
	WriteCStrToOutput("/\"");
	WriteEolToOutput();

	WriteCStrToOutput("if test ! -d \"${");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("}\" ; then");
	WriteEolToOutput();

	WriteCStrToOutput("\tmkdir \"${");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("}\"");
	WriteEolToOutput();

	WriteLnCStrToOutput("fi");
#endif
#if (gbo_script == gbk_script_vbscript)
	WriteCStrToOutput("dim ");
	WriteCStrToOutput(new_d);
	WriteEolToOutput();

	WriteCStrToOutput(new_d);
	WriteCStrToOutput(" = ");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput(" & \"\\");
	WriteCStrToOutput(name);
	WriteCStrToOutput(FileExt);
	WriteCStrToOutput("\"");
	WriteEolToOutput();

	WriteCStrToOutput("if (NOT fso.FolderExists(");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(")) Then");
	WriteEolToOutput();

	WriteCStrToOutput("\tfso.CreateFolder(");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput(")");
	WriteEolToOutput();

	WriteLnCStrToOutput("End If");
#endif
#if (gbo_script == gbk_script_xp)
	WriteCStrToOutput("set ");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("=%");
	WriteCStrToOutput(parent_d);
	WriteCStrToOutput("%");
	WriteCStrToOutput("\\");
	WriteCStrToOutput(name);
	WriteCStrToOutput(FileExt);
	WriteEolToOutput();

	WriteCStrToOutput("if not exist \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\" mkdir \"%");
	WriteCStrToOutput(new_d);
	WriteCStrToOutput("%\"");
	WriteEolToOutput();
#endif
}



/* ------- utilities for writing to text files -------- */

LOCALPROC WriteDestFileLn(char *s)
{
	WriteBgnDestFileLn();
	WriteCStrToDestFile(s);
	WriteEndDestFileLn();
}

LOCALPROC WriteSpaceToDestFile(void)
{
	WriteCharToDestFile(' ');
}

LOCALPROC WriteQuoteToDestFile(void)
{
	WriteCharToDestFile('\"');
}

LOCALPROC WriteSingleQuoteToDestFile(void)
{
	WriteCharToDestFile('\'');
}

LOCALPROC WriteBackSlashToDestFile(void)
{
	WriteCharToDestFile('\\');
}
