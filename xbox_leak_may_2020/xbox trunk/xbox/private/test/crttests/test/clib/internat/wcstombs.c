
/***************************************************************************

		   Copyright (c) 1993 Microsoft Corporation

Test:	    wcstombs

Abstract:   Verify functionality of wcstombs in the "C" locale.

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
#include <locale.h>
#include "test.h"

static wchar_t string[] = { L"a z" };

static char 	test[] = "wcstombs";
static int 	perfect=0;

int wcstombsEntry ( ){
	char		outstr[500];
	
	startest ();

	/* Verify case where a char in the string is > 128 */
	string[1] = 0x82;

	if ( wcstombs ( outstr, string, 50 ) != 3 )
		fail ( 27 );

	if ( ( outstr[0] != 'a' ) || ( outstr[1] != '\x82' ) || ( outstr[2] != 'z' ) )
		fail ( 28 );

	/* Scene 1: Verify passing a NULL string returns the necessary size */

	if ( wcstombs ( NULL, L"string1", 500 ) != 7 ) 
		fail ( 1 );

	/* Scene 2: Verify passing a size 0 fails */

	if ( wcstombs ( outstr, L"string1", 0 ) != 0 ) 
		fail ( 2 );

	/* Scene 3: Verify passing a size of 1 passes */

	if ( wcstombs ( outstr, L"a", 1 ) != 1 ) 
		fail ( 3 );

	if ( strncmp ( outstr, "a", 1 ) ) 
		fail ( 4 );

	/* Scene 4: Verify passing a size < necessary size by one passes */

	if ( wcstombs ( outstr, L"string2", 6 ) != 6 ) 
		fail ( 5 );

	if ( strncmp ( outstr, "string", 6 ) ) 
		fail ( 6 );

	/* Scene 5: Pass size = necessary size works */

	if ( wcstombs ( outstr, L"string3", 7 ) != 7 ) 
		fail ( 7 );

	if ( strncmp ( outstr, "string3", 7 ) ) 
		fail ( 8 );

	/* Scene 6: Pass size > necessary size by one works */

	if ( wcstombs ( outstr, L"string3", 8 ) != 7 ) 
		fail ( 9 );

	if ( strncmp ( outstr, "string3", 7 ) ) 
		fail ( 10 );

	/* Scene 7: Passing null as the from string works */

	if ( wcstombs ( outstr, L"\0", 8 ) != 0 ) 
		fail ( 11 );

	if ( outstr[0] != '\0' ) 
		fail ( 12 );

	/* Scene 8: Passing a "\n" as the from string works */

	if ( wcstombs ( outstr, L"\n", 8 ) != 1 ) 
		fail ( 13 );

	if ( outstr[0] != '\n' ) 
		fail ( 14 );

	/* Scene 9: Passing a single character works */

	if ( wcstombs ( outstr, L"a", 20 ) != 1 ) 
		fail ( 15 );

	if ( outstr[0] != 'a' ) 
		fail ( 16 );

	/* Scene 10: Passing two chars works */

	if ( wcstombs ( outstr, L"ab", 40 ) != 2 ) 
		fail ( 17 );

	if ( strncmp ( outstr, "ab", 2 ) ) 
		fail ( 18 );

	/* Scene 11: Passing three chars works */

	if ( wcstombs ( outstr, L"abc", 40 ) != 3 ) 
		fail ( 19 );

	if ( strncmp ( outstr, "abc", 3 ) ) 
		fail ( 20 );

	/* Scene 12: Passing a large string works */

	if ( wcstombs ( outstr, L"foo\0", 500 ) != 3 ) 
		fail ( 21 );

	if ( strncmp ( outstr, "foo\0", 4 ) ) 
		fail ( 22 );

	/* Scene 13: Passing a large string works */

	if ( wcstombs ( outstr, L"abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 500 ) != 150 ) 
		fail ( 23 );

	if ( strncmp ( outstr, "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 150 ) ) 
		fail ( 24 );

	/* Scene 14: Passing a large string containing a "\n" works */

	if ( wcstombs ( outstr, L"abcdefghijklmnopqrstuvwxyz1234\nabcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 500 ) != 151 ) 
		fail ( 25 );

	if ( strncmp ( outstr, "abcdefghijklmnopqrstuvwxyz1234\nabcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 151 ) ) 
		fail ( 26 );

	finish ();

}
