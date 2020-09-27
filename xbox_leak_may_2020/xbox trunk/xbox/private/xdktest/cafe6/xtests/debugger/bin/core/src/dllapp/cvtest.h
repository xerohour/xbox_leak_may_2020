/*
** cvtest.h - definitions/declarations for standard codeview testing
** Copyright (c) 1992-1992, Microsoft Corporation. All rights reserved.
**
**Purpose:
**	This file defines the structures, values, macros, and functions
**	used by the codeview tests.
**
**
*/

#if !defined( _INC_CVTEST )

#define CVMAXMSG (80)

#if !defined CDECL 
#define CDECL _cdecl
#endif

#if !defined FASTCALL
#define FASTCALL _fastcall
#endif

#if !defined( SAVELABEL )
#define SAVELABEL( l )	if (0) goto l
#endif

/* prototypes */
void InitCVTest( void );
void EndCVTest( void );
int ChkEQ( int x, int y);
int ChkTrue( int x );
int ChkFalse( int x );
char *PrintIsFalse( int );
char *PrintIsTrue( int );
char *PrintIsEqual( int, int );

#define _INC_CVTEST
#endif	/* _INC_CVTEST */
