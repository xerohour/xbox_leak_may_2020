
/***************************************************************************

		   Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: _wcsicoll

          int _wcsicoll(const wchar_t *, const wchar_t *);

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

static char test[] = "_wcsicoll";
static int perfect = 0;

static wchar_t wcs_lowest[] = L"\1";
static wchar_t wcs_highest[] = {65033, 0};
static wchar_t wcs_empty[] = L"";

int _WCSICOLEntry(){

/* for C LOCATE only for now */

/* Begin */

   startest();

/* Less */

   if( !(_wcsicoll( wcs_lowest, wcs_highest ) < 0) )
      fail( 0 );

   if( !(_wcsicoll( L"ABC", L"ABD" ) < 0) )
      fail( 1 );

   if( !(_wcsicoll( L"AB[", L"ABC" ) < 0) )
      fail( 2 );

   if( !(_wcsicoll( L"ab[", L"abc" ) < 0) )
      fail( 3 );

   if( !(_wcsicoll( L"", L"A" ) < 0) )
      fail( 4 );


/* Greater */

   if( !(_wcsicoll( wcs_highest, wcs_lowest ) > 0) )
      fail( 10 );

   if( !(_wcsicoll( L"ABD", L"ABC" ) > 0) )
      fail( 11 );

   if( !(_wcsicoll( L"ABC", L"AB`" ) > 0) )
      fail( 12 );

   if( !(_wcsicoll( L"abc", L"ab`" ) > 0) )
      fail( 13 );

   if( !(_wcsicoll( L"A", L"" ) > 0) )
      fail( 14 );

/* Equal */

   if( !(_wcsicoll( wcs_lowest, L"\1" ) == 0) )
      fail( 20 );

   if( !(_wcsicoll( wcs_highest, wcs_highest ) == 0) )
      fail( 21 );

   if( !(_wcsicoll( L"aBc", L"aBc" ) == 0) )
      fail( 22 );

/* end */

   finish();
}
