/*
	DFFILDEF.i
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
	DEFinitions for program SPecific FILe DEFinitions
*/

#define kDepDirCSrc 0
#define kDepDirCnfg 1

typedef void (*tDoOneDepends)(int DepDir, char *s);

typedef void (*tDoDependsForC)(tDoOneDepends p);

typedef void (*tDoOneCFile)(char *s, int DepDir, long Flgm,
	tDoDependsForC depends);

typedef void (*tWriteOneExtension)(char *s);

typedef void (*tWriteExtensionList)(tWriteOneExtension p);

typedef void (*tWriteOneDocType)(
	char *ShortName,
	char *MacType,
	char *LongName,
	tWriteExtensionList WriteExtensionList);

#if 0
#define kCSrcFlagAsmAvail 0
#define kCSrcFlagAltSrc 1
#endif
#define kCSrcFlagSkip 1
#define kCSrcFlagUseAPI 2
#define kCSrcFlagSortFirst 3
#define kCSrcFlagNoSource 4
#define kCSrcFlagNoHeader 5
#define kCSrcFlagOjbc 6

#define kCSrcFlgmNone 0
#if 0
#define kCSrcFlgmAltSrc (1 << kCSrcFlagAltSrc)
#endif
#define kCSrcFlgmSkip (1 << kCSrcFlagSkip)
#define kCSrcFlgmUseAPI (1 << kCSrcFlagUseAPI)
#define kCSrcFlgmSortFirst (1 << kCSrcFlagSortFirst)
#define kCSrcFlgmNoSource (1 << kCSrcFlagNoSource)
#define kCSrcFlgmNoHeader (1 << kCSrcFlagNoHeader)
#define kCSrcFlgmOjbc (1 << kCSrcFlagOjbc)

#define CSrcFlagsUseIf(b) ((b) ? kCSrcFlgmNone : kCSrcFlgmSkip)
#define CSrcFlagsUseHdrIf(b) (CSrcFlagsUseIf(b) | kCSrcFlgmNoSource)
#define CSrcFlagsUseSrcIf(b) (CSrcFlagsUseIf(b) | kCSrcFlgmNoHeader)
