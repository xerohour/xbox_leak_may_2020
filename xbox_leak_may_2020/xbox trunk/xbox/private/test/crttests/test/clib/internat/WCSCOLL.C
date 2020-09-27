
/***************************************************************************

		   Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcscoll

          int wcscoll(const wchar_t *, const wchar_t *);

OS Version:
CPU:
Dependencies:		NONE
Environment Vars:	NONE
Verification Method:	TBD
Priority:		1/2/3/4
Notes:			NONE
Products:


Revision History:

    Date	emailname   description
----------------------------------------------------------------------------
    28-May-92	xiangjun    created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/


#include <string.h>
#include <stdio.h>
#include "test.h"

static char test[] = "wcscoll";
static int perfect = 0;

static wchar_t wcs_lowest[] = L"\1";
static wchar_t wcs_highest[] = {65033,0};
static wchar_t wcs_empty[] = L"";

int WCSCOLLEntry(){

/* Begin */

   startest();

/* For C LOCALE only now */

/* Less */

   if( !(wcscoll( wcs_lowest, wcs_highest ) < 0) )
      fail( 0 );

   if( !(wcscoll( L"ABC", L"ABD" ) < 0) )
      fail( 1 );

   if( !(wcscoll( L"ABC", L"AbC" ) < 0) )
      fail( 2 );

   if( !(wcscoll( L"", L"A" ) < 0) )
      fail( 3 );


/* Greater */

   if( !(wcscoll( wcs_highest, wcs_lowest ) > 0) )
      fail( 10 );

   if( !(wcscoll( L"ABD", L"ABC" ) > 0) )
      fail( 11 );

   if( !(wcscoll( L"AbC", L"ABC" ) > 0) )
      fail( 12 );

   if( !(wcscoll( L"A", L"" ) > 0) )
      fail( 13 );

/* Equal */

   if( !(wcscoll( wcs_lowest, L"\1" ) == 0) )
      fail( 20 );

   if( !(wcscoll( wcs_highest, wcs_highest ) == 0) )
      fail( 21 );

   if( !(wcscoll( L"aBc", L"aBc" ) == 0) )
      fail( 22 );

/* end */

   finish();
}
