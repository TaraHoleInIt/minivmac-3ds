/*
	COREDEFS.i
	Copyright (C) 2018 Paul C. Pratt

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
	CORE DEFinitionS
*/


typedef unsigned long ui5b;
typedef unsigned short ui4b;
typedef unsigned char ui3b;

typedef signed long si5b;
typedef signed short si4b;
typedef signed char si3b;

typedef ui5b ui5r;
typedef ui4b ui4r;
typedef ui3b ui3r;

typedef si5b si5r;
typedef si4b si4r;
typedef si3b si3r;

typedef si3b *si3p;
typedef ui3b *ui3p;
typedef si4b *si4p;
typedef ui4b *ui4p;
typedef si5b *si5p;
typedef ui5b *ui5p;

typedef ui5p *ui5h;
typedef ui4p *ui4h;
typedef ui3p *ui3h;

/*
	define the largest supported
	representation types.
*/
#define uimbl2sz 5
typedef si5r simr;
typedef ui5r uimr;

typedef ui3b blnb;
typedef ui3r blnr;
#define trueblnr 1
#define falseblnr 0

typedef si4r tMyErr;

typedef unsigned char MyPStr[256];
typedef unsigned char *ps3p;
typedef unsigned char MyCharR;
typedef MyCharR *MyCharPtr;

typedef unsigned char MyByte;
typedef MyByte *MyPtr;

#define nullpr ((void *) 0)

#define DISCARDVAL (void)


#define LOCALFUNC static
#define GLOBALFUNC static
#define EXPORTFUNC static
#define TYPEDEFFUNC typedef

#define LOCALPROC LOCALFUNC void
#define GLOBALPROC GLOBALFUNC void
#define EXPORTPROC EXPORTFUNC void
#define TYPEDEFPROC TYPEDEFFUNC void

#define LOCALVAR static
#define GLOBALVAR static
#define EXPORTVAR static

GLOBALVAR MyPtr pDt;
