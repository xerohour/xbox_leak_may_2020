
/***************************************************************************

		   Copyright (c) 1993 Microsoft Corporation

Method: Calling the function with the test string.


Switches:   NONE


Abstract:   Test for _mbstrlen under the "C" locale.
	    (There are no muliple byte characters under the "C" locale
	    so this test just verifies the single byte functionality 
	    works.)


===========================================================================
Syntax

===========================================================================
OS Version:
CPU:
Dependencies:           
Environment Vars:       
Verification Method:    
Priority:               1
Notes:                  
Products:               WIN NT


Revision History:

    Date        emailname   description
----------------------------------------------------------------------------
    17-Mar-93   kevinboy    created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "test.h"

static char test[] = "_mbslen";
static int perfect;

int _mbslenEntry ( ){

	char    zero [1] = { '\0' };
	char    one [2] = { 'a', '\0' };
	char    two [3] = { 'a', 'b', '\0' };
	char    three [4] = { 'a', 'b', 'c', '\0' };

	unsigned int great[] = { 0x82 };

	startest();

	if ( _mbstrlen ( zero ) != 0 )
		fail( 0 );

	if (  _mbstrlen ( one ) != 1 )
		fail( 1 );
	
	if (  _mbstrlen ( two ) != 2 )
		fail( 3 );

	if (  _mbstrlen ( three ) != 3 )
		fail( 4 );

	if ( _mbstrlen ( (char *)great ) != 1 )
		fail( 5 );

	finish();
}
