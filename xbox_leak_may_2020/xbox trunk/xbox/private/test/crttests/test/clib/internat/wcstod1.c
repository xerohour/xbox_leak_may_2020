
/***************************************************************************

		   Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcstod

          double wcstod(const wchar_t *nptr, wchar_t **endptr);

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
    27-Jan-93	xiangjun    created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/


#include <string.h>
#include <wchar.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include "test.h"

#define STRINGIZE0(x) L#x
#define STRINGIZE(x) STRINGIZE0(x)


static char test[] = "wcstod";
static int perfect = 0;

typedef struct {
   wchar_t * string;
   double expected;
   } DATA;

static DATA data1[] = {
                L" \f\n\r\t\v123456.789final string", 123456.789,
                L" \f\n\r\t\v+123456.789final string", 123456.789,
                L" \f\n\r\t\v-123456.789final string", -123456.789,

                L" \f\n\r\t\v" STRINGIZE(DBL_MAX) L"final string", DBL_MAX,
                L" \f\n\r\t\v" STRINGIZE(-DBL_MAX) L"final string", -DBL_MAX,
                L" \f\n\r\t\v" STRINGIZE(DBL_MIN) L"final string", DBL_MIN,
                L" \f\n\r\t\v" STRINGIZE(-DBL_MIN) L"final string", -DBL_MIN,

                L" \f\n\r\t\v-4.000000e+000final string", -4.0,
                L" \f\n\r\t\v4.000000E+000final string", 4.0,
                L" \f\n\r\t\v-6.85E+001final string", -68.5,
                L" \f\n\r\t\v-7.45E+001final string", -74.5,
                L" \f\n\r\t\v 6.7844324e+004final string", 67844.324,
                L" \f\n\r\t\v  +6.7844324e+004final string", 67844.324,
                L" \f\n\r\t\v    0.000000e+000final string", 0.0,
                L" \f\n\r\t\v-00007.4673e-001final string", -0.74673,

                L"-4.000000final string", -4.0,
                L"4.000000final string", 4.0,
                L"0.400000final string",  0.4,
                L"-69final string", -69,
                L"-74final string", -74.,
                L" 67844.324final string", 67844.324,
                L"   +67844.324final string", 67844.324,
                L"         0.000000final string", 0.0,
                L"-0000000000.74673final string", -0.74673,
                };

static DATA data2[] = {
                L" \f\n\r\t\v0.0009", 0.0009,

                L" \f\n\r\t\v3.1415926535E-005", 0.000031415926535,
                L" \f\n\r\t\v0.00031415926535", 0.00031415926535,
                L" \f\n\r\t\v0.0031415926535", 0.0031415926535,
                L" \f\n\r\t\v0.031415926535", 0.031415926535,
                L" \f\n\r\t\v0.31415926535", 0.31415926535,
                L" \f\n\r\t\v3.1415926535", 3.1415926535,
                L" \f\n\r\t\v31.415926535", 31.415926535,
                L" \f\n\r\t\v314.15926535", 314.15926535,
                L" \f\n\r\t\v3141.5926535", 3141.5926535,
                L" \f\n\r\t\v31415.926535", 31415.926535,
                L" \f\n\r\t\v314159.26535", 314159.26535,
                L" \f\n\r\t\v3.1415926535e+006", 3141592.6535,
                L" \f\n\r\t\v3", 3.0,
                L" \f\n\r\t\v3e+006", 3000000.0,
                L" \f\n\r\t\v   3.1402", 3.1402,
                L" \f\n\r\t\v 3.1402", 3.1402,
                L" \f\n\r\t\v+0023.000", 23.0,
                L" \f\n\r\t\v23.", 23.0,
                L" \f\n\r\t\v23",  23.0,
                };

static DATA specialData[] = {
                L" \f\n\r\t\v", 0,
                L"", 0,
                L" \f\n\r\t\vhello3.1415926535", 0,
                L"hello3.1415926535", 0,
                L"?3.1415926535", 0,
                L" \f\n\r\t\v-+123456.789final string", 0,
                };






int wcstod1Entry(){
int i;
wchar_t * endptr;

/* Begin */

   startest();


/* with final string */

for( i = 0; i < sizeof(data1)/sizeof(data1[0]); i++ ) {

   if( data1[i].expected != wcstod( data1[i].string, &endptr )
      ||
       endptr != wcsstr(data1[i].string, L"final string" ) )
{
      fail( 1 );
DbgPrint( "%g %g", wcstod( data1[i].string, &endptr ), data1[i].expected );
}
   }

/* without final string */

for( i = 0; i < sizeof(data2)/sizeof(data2[0]); i++ ) {

   if( data2[i].expected != wcstod( data2[i].string, &endptr )
      ||
       endptr != data2[i].string+wcslen( data2[i].string ) )
      fail( 12 );
   }

/* without sequence string */

for( i = 0; i < sizeof(specialData)/sizeof(specialData[0]); i++ ) {

   if( specialData[i].expected != wcstod( specialData[i].string, &endptr ) 
       ||
       endptr != specialData[i].string )
      fail( 2 );
}

/* out of range */

errno = 0;

if( wcstod( L"123456789e500", &endptr ) != HUGE_VAL
   ||
    errno != ERANGE )
   fail( 3 );

errno = 0;

if( wcstod( L"-123456789e500", &endptr ) != -HUGE_VAL
   ||
    errno != ERANGE )
   fail( 4 );

errno = 0;

if( wcstod( L"-123456789e-500", &endptr ) != 0
   ||
    errno != ERANGE )
   fail( 5 );

/* NULL endptr */

if( wcstod( L"-123456789", NULL ) != -123456789 )
   fail( 6 );

   finish();
}
