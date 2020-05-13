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


typedef unsigned long ui5r;
typedef unsigned short ui4r;
typedef unsigned char ui3r;

typedef signed long si5r;
typedef signed short si4r;
typedef signed char si3r;

/*
	define the largest supported
	representation types.
*/
typedef unsigned long uimr;
typedef signed long simr;

typedef unsigned char blnr;
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
