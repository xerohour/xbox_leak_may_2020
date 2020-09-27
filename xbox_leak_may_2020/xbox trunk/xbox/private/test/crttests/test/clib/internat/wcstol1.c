
/***************************************************************************

		   Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcstol

	  long int wcstol(const wchar_t *nptr, wchar_t **endptr, int base);

OS Version:
CPU:
Dependencies:           NONE
Environment Vars:       NONE
Verification Method:    TBD
Priority:               1/2/3/4
Notes:                  NONE
Products:


Revision History:

    Date        emailname   description
----------------------------------------------------------------------------
    28-Jan-93   xiangjun    created
----------------------------------------------------------------------------
    27-May-93   a-marca     changed out of range -2147483648 to -2147483649
			    for call wcstol( L"", &endptr, 10 ) != LONG_MIN
----------------------------------------------------------------------------

						     
EndHeader:

****************************************************************************/

#include <string.h>
#include <wchar.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include "test.h"

/* MAY CHANGE IN FUTURE RELEASESES, hard-coded to get this test to run */
#define ABS_LONG_MIN    2147483648

#define STRINGIZE0(x) L#x
#define STRINGIZE1(x) STRINGIZE0(x)
#define STRINGIZE(x) STRINGIZE1(x)

static char test[] = "wcstol";
static int perfect = 0;

typedef struct {
   wchar_t * string;
   long expected;
   int base;
   } DATA;

static DATA data1[] = {
		L" \f\n\r\t\v123456long constant", 123456, 0,
		L" \f\n\r\t\v+123456long constant", 123456, 0,
		L" \f\n\r\t\v-123456long constant", -123456, 0,
		L" \f\n\r\t\v" STRINGIZE(LONG_MAX) L"long constant", LONG_MAX, 0,
		L" \f\n\r\t\v" STRINGIZE(-ABS_LONG_MIN) L"long constant", LONG_MIN, 0,


		L" \f\n\r\t\v123456long constant", 123456, 10,
		L" \f\n\r\t\v+123456long constant", 123456, 10,
		L" \f\n\r\t\v-123456long constant", -123456, 10,
		L" \f\n\r\t\v" STRINGIZE(LONG_MAX) L"long constant", LONG_MAX, 10,
		L" \f\n\r\t\v" STRINGIZE(-ABS_LONG_MIN) L"long constant", LONG_MIN, 10,

		L" \f\n\r\t\v0x123456long constant", 0x123456, 0,
		L" \f\n\r\t\v+0X123456long constant", 0x123456, 0,
		L" \f\n\r\t\v-0x123456long constant", -0X123456, 0,


		L" \f\n\r\t\v0x123456long constant", 0x123456, 16,
		L" \f\n\r\t\v+0X123456long constant", 0x123456, 16,
		L" \f\n\r\t\v-0x123456long constant", -0X123456, 16,
		};


static DATA data2[] = {
		L" \f\n\r\t\v123456", 123456, 0,
		L" \f\n\r\t\v+123456", 123456, 0,
		L" \f\n\r\t\v-123456", -123456, 0,
		L" \f\n\r\t\v" STRINGIZE(LONG_MAX), LONG_MAX, 0,
		L" \f\n\r\t\v" STRINGIZE(-ABS_LONG_MIN), LONG_MIN, 0,


		L" \f\n\r\t\v123456", 123456, 10,
		L" \f\n\r\t\v+123456", 123456, 10,
		L" \f\n\r\t\v-123456", -123456, 10,
		L" \f\n\r\t\v" STRINGIZE(LONG_MAX), LONG_MAX, 10,
		L" \f\n\r\t\v" STRINGIZE(-ABS_LONG_MIN), LONG_MIN, 10,

		L" \f\n\r\t\v0x123456", 0x123456, 0,
		L" \f\n\r\t\v+0X123456", 0x123456, 0,
		L" \f\n\r\t\v-0x123456", -0X123456, 0,

		L" \f\n\r\t\v0x123456", 0x123456, 16,
		L" \f\n\r\t\v+0X123456", 0x123456, 16,
		L" \f\n\r\t\v-0x123456", -0X123456, 16,
		};

		
static DATA specialData[] = {
		L" \f\n\r\t\v", 0, 2,
		L"", 0, 3,
		L" \f\n\r\t\vhello3.1415926535", 0, 4,
		L"hello3.1415926535", 0, 5,
		L"?3.1415926535", 0, 6,
		L" \f\n\r\t\v-+123456long constant", 0, 0,
		};

int wcstol1Entry(){
int i;
wchar_t * endptr;
long int long_number;
wchar_t long_number_string[32];

/* Begin */

   startest();

/* with final string */

for( i = 0; i < sizeof(data1)/sizeof(data1[0]); i++ ) {

   if( data1[i].expected != wcstol( data1[i].string, &endptr, data1[i].base ))
   {
      fail( 1 );
   }
   else
   {
      //Adjust comparison for 'L' in LIMITS constants
      if (*endptr == (wchar_t)'L')
        endptr++;

      if (endptr != wcsstr(data1[i].string, L"long constant" ) )
      {
        fail( 1 );
      }
   }
}

/* without final string */

for( i = 0; i < sizeof(data2)/sizeof(data2[0]); i++ ) {

   if( data2[i].expected != wcstol( data2[i].string, &endptr, data2[i].base ))
   {
     fail( 12 );
   }
   else
   {
      //Adjust comparison for 'L' in LIMITS constants
      if (*endptr == (wchar_t)'L')
        endptr++;

      if (endptr != data2[i].string+wcslen( data2[i].string ) )
	fail( 12 );
   }
}

/* without sequence string */

for( i = 0; i < sizeof(specialData)/sizeof(specialData[0]); i++ ) {

   if( specialData[i].expected != wcstol( specialData[i].string, &endptr, specialData[i].base ) 
       ||
       endptr != specialData[i].string )
      fail( 2 );
}

/* out of range */

errno = 0;

if( wcstol( STRINGIZE(LONG_MAX), &endptr, 10 ) != LONG_MAX 
   ||
    errno != 0
   ||
    wcstol( L"2147483649", &endptr, 10 ) != LONG_MAX
   ||
    errno != ERANGE )
   fail( 3 );

errno = 0;

if( wcstol( STRINGIZE(-ABS_LONG_MIN), &endptr, 10 ) != LONG_MIN
   ||
    errno != 0
   ||
    wcstol( L"-2147483649", &endptr, 10 ) != LONG_MIN
   ||
    errno != ERANGE )
   fail( 4 );

errno = 0;

/* NULL endptr */

if( wcstol( L"-123456789", NULL, 10 ) != -123456789 )
   fail( 5 );


/* test for number base 2-32 */

long_number = LONG_MAX;

for( i = 2; i < 33; i++ ) {

   long l;
   int j;
   int remainder;

   for( l = long_number, j = 0 ;
	l > 0;
	 ) {
      remainder = l%i;
      l/=i;

      long_number_string[j] =  remainder < 10 ?
			       remainder+'0' : remainder-10+'a';
      j++;
      }

   long_number_string[j] = 0;

   _wcsrev( long_number_string );

   if( wcstol( long_number_string, &endptr, i ) != long_number ) {
      fail(6);
      printf( "failed when base = %d\n", i );
      printf( "wcstol() = %ld, from %ld\n",
	 wcstol( long_number_string, &endptr, i ),
	 long_number );
      }
    
   }
  

   finish();
}
