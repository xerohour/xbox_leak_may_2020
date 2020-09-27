
/***************************************************************************

		   Copyright (c) 1993 Microsoft Corporation

Test:	    wctomb

Abstract:   Verify functionality of wctomb in the "C" locale.

===========================================================================
Syntax

===========================================================================
Priority:		1
Products:		WIN NT

Revision History:
    Date		emailname   description
----------------------------------------------------------------------------
  03-May-93		kevinboy    created
----------------------------------------------------------------------------

EndHeader:
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

static char 	test[] = "wctomb";
static int 	perfect=0;

int wctombEntry ( ){
	char		outstr;
	wchar_t		in;	


	startest ();

	/* Verify for char > 128 */
	in = 0x82;
	if ( wctomb ( &outstr, in ) != 1 )
		fail ( 7 );

	if ( outstr != '\x82' )
		fail ( 8 );

	/* Scene 1: Passing null as the from string works */

	if ( wctomb ( &outstr, '\0' ) != 1 ) 
		fail ( 2 );

	/* Scene 2: Passing a L"\n" as the from string works */

	if ( wctomb ( &outstr, L'\n' ) != 1 ) 
		fail ( 3 );

	if ( outstr != '\n' ) 
		fail ( 4 );

	/* Scene 3: Passing a single character works */

	if ( wctomb ( &outstr, L'a' ) != 1 ) 
		fail ( 5 );

	if ( outstr != 'a' ) 
		fail ( 6 );

	finish ();

}
