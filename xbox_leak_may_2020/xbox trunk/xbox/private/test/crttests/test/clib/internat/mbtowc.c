
/***************************************************************************

		   Copyright (c) 1993 Microsoft Corporation

Test:	    mbtowc

Abstract:   Verify functionality of mbtowc in the "C" locale.

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

static char 	test[] = "mbtowc";
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

int mbtowcEntry ( ){
	wchar_t			outstr[50];
	unsigned int	tmp;

	startest ();

	/* Scene 1A: Verify passing a single-byte NULL string returns one */

	if ( mbtowc ( NULL, "s", 1 ) != 1 ) 
		fail ( 1 );

	/* Scene 1: Verify passing a wide NULL string returns one */

	if ( mbtowc ( L'\0', "s", 1 ) != 1 ) 
		fail ( 1 );

	/* Scene 3: Passing a "\n" as the from string works */

	if ( mbtowc ( outstr, "\n", 1 ) != 1 ) 
		fail ( 3 );

	if ( outstr[0] != L'\n' ) 
		fail ( 4 );

	/* Scene 4: Passing a single character works */

	if ( mbtowc ( outstr, "a", 1 ) != 1 ) 
		fail ( 5 );

	if ( outstr[0] != L'a' ) 
		fail ( 6 );

	/* Scene 5: Passing two chars works */

	if ( mbtowc ( outstr, "ab", 1 ) != 1 ) 
		fail ( 7 );

	if ( wcsncmp ( outstr, L"a", 1 ) ) 
		fail ( 8 );

	/* Verify 0 is returned if mbchar is NULL */

	if ( mbtowc ( outstr, '\0', 1 ) != 0 ) 
		fail ( 20 );

	if ( mbtowc ( outstr, NULL, 1 ) != 0 ) 
		fail ( 21 );

// Repeat the above with count > 1

	/* Scene 1A: Verify passing a single-byte NULL string returns the 
							necessary size */

	if ( mbtowc ( NULL, "s", 20 ) != 1 ) 
		fail ( 9 );

	/* Scene 1: Verify passing a wide NULL string returns the 
							necessary size */

	if ( mbtowc ( L'\0', "s", 20 ) != 1 ) 
		fail ( 10 );

	/* Scene 3: Passing a "\n" as the from string works */

	if ( mbtowc ( outstr, "\n", 20 ) != 1 ) 
		fail ( 12 );

	if ( outstr[0] != L'\n' ) 
		fail ( 13 );

	/* Scene 4: Passing a single character works */

	if ( mbtowc ( outstr, "a", 20 ) != 1 ) 
		fail ( 14 );

	if ( outstr[0] != L'a' ) 
		fail ( 15 );

	/* Scene 5: Passing two chars works */

	if ( mbtowc ( outstr, "ab", 20 ) != 1 ) 
		fail ( 16 );

	if ( wcsncmp ( outstr, L"a", 1 ) ) 
		fail ( 17 );

	/* Verify 0 is returned if mbchar is NULL */

	if ( mbtowc ( outstr, '\0', 20 ) != 0 ) 
		fail ( 22 );

	if ( mbtowc ( outstr, NULL, 20 ) != 0 ) 
		fail ( 23 );

// With a count of zero

	if ( mbtowc ( outstr, "s", 0 ) != 0 ) 
		fail ( 18 );

	/* Verify 
	if ( mbtowc ( outstr, "s", 0 ) != -1 ) 
		fail ( 18 );

	/* Verify 0 is returned if mbchar is NULL */

	if ( mbtowc ( outstr, '\0', 0 ) != 0 ) 
		fail ( 24 );

	if ( mbtowc ( outstr, NULL, 0 ) != 0 ) 
		fail ( 25 );

	/* Attempt with a char > 128 */

	tmp = 0x82;

	if ( mbtowc ( outstr, (char *)&tmp, 1 ) != 1  )
		fail ( 26 );

	if ( outstr[0] != 0x82 )
		fail ( 27 );

	finish ();

}
