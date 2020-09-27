
/***************************************************************************

		   Copyright (c) 1993 Microsoft Corporation

Test:	    mbstowcs

Abstract:   Verify functionality of mbstowcs in the "C" locale.

===========================================================================
Syntax

===========================================================================
Priority:		1
Products:		WIN NT

Revision History:
    Date		emailname   description
----------------------------------------------------------------------------
  03-May-93		kevinboy    created
  17-April-95           a-timke     Enabled for Mac
----------------------------------------------------------------------------

EndHeader:
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

static unsigned char string[] = { "a z" };

static char 	test[] = "mbstowcs";
static int 	perfect=0;

#if defined(TEST_PMAC) || defined(TEST_M68K)

#define wcsncmp WCSNCMP
//Implement the wcscmp() function, without using APIs.
static int WCSNCMP(wchar_t *pwchS1, wchar_t *pwchS2, int nCount)
{
  if (nCount==0)
    return 0;

  while (--nCount > 0 && *pwchS1 && *pwchS2)
  {
    if (*pwchS1 != *pwchS2)
      break;
    pwchS1++;
    pwchS2++;
  }
  return *pwchS1 - *pwchS2;
}
#endif //TEST_PMAC || TEST_M68K

int mbstowcsEntry ( ){
	wchar_t		outstr[500];
	
	startest ();

	/* Verify handling of string with a char > 128 */

	string[1] = 0x82;

	if ( mbstowcs ( outstr, string, 500 ) != 3 ) 
		fail ( 26 );

	if ( ( outstr[0] != 97 ) || ( outstr[1] != 0x82 ) || ( outstr[2] != 122 ) )
		fail ( 27 );

	/* Scene 1A: Verify passing a single-byte NULL string returns the 
							necessary size */

	if ( mbstowcs ( NULL, "string1", 500 ) != 7 ) 
		fail ( 1 );

	/* Scene 1: Verify passing a wide NULL string returns the 
							necessary size */

	if ( mbstowcs ( L'\0', "string1", 500 ) != 7 ) 
		fail ( 1 );

	/* Scene 2: Verify passing a size 0 fails */

	if ( mbstowcs ( outstr, "string2", 0 ) != 0 ) 
		fail ( 2 );

	/* Scene 3: Verify passing a size of 1 passes */

	if ( mbstowcs ( outstr, "a", 1 ) != 1 ) 
		fail ( 3 );

	if ( wcsncmp ( outstr, L"a", 1 ) ) 
		fail ( 4 );

	/* Scene 4: Verify passing a size < necessary size by one passes */

	if ( mbstowcs ( outstr, "string2", 6 ) != 6 ) 
		fail ( 5 );

	if ( wcsncmp ( outstr, L"string", 6 ) ) 
		fail ( 6 );

	/* Scene 5: Pass size = necessary size works */

	if ( mbstowcs ( outstr, "string3", 7 ) != 7 ) 
		fail ( 7 );

	if ( wcsncmp ( outstr, L"string3", 7 ) ) 
		fail ( 8 );

	/* Scene 6: Pass size > necessary size by one works */

	if ( mbstowcs ( outstr, "string3", 8 ) != 7 ) 
		fail ( 9 );

	if ( wcsncmp ( outstr, L"string3", 7 ) ) 
		fail ( 10 );

	/* Scene 8: Passing a "\n" as the from string works */


	if ( mbstowcs ( outstr, "\n", 8 ) != 1 ) 
		fail ( 13 );

	if ( outstr[0] != L'\n' ) 
		fail ( 14 );

	/* Scene 9: Passing a single character works */

	if ( mbstowcs ( outstr, "a", 20 ) != 1 ) 
		fail ( 15 );

	if ( outstr[0] != L'a' ) 
		fail ( 16 );

	/* Scene 10: Passing two chars works */

	if ( mbstowcs ( outstr, "ab", 40 ) != 2 ) 
		fail ( 17 );

	if ( wcsncmp ( outstr, L"ab", 2 ) ) 
		fail ( 18 );

	/* Scene 11: Passing three chars works */

	if ( mbstowcs ( outstr, "abc", 40 ) != 3 ) 
		fail ( 19 );

	if ( wcsncmp ( outstr, L"abc", 3 ) ) 
		fail ( 20 );

	/* Scene 12: Passing a string with a NULL */

	if ( mbstowcs ( outstr, "foo\0", 500 ) != 3 ) 
		fail ( 21 );

	if ( wcsncmp ( outstr, L"foo\0", 4 ) ) 
		fail ( 22 );

	/* Scene 13: Passing a large string works */

	if ( mbstowcs ( outstr, "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 500 ) != 150 ) 
		fail ( 23 );

	if ( wcsncmp ( outstr, L"abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 150 ) ) 
		fail ( 24 );

	/* Scene 14: Passing a large string containing "\n" works */

	if ( mbstowcs ( outstr, "abcdefghijklmnopqrstuvwxyz1234\nabcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 500 ) != 151 ) 
		fail ( 25 );

	if ( wcsncmp ( outstr, L"abcdefghijklmnopqrstuvwxyz1234\nabcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234", 151 ) ) 
		fail ( 26 );

	finish ();

}
