
/***************************************************************************

		   Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcstoul

          unsigned long int wcstoul(const wchar_t *nptr, wchar_t **endptr, int base);

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
    28-Jan-93	xiangjun    created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/


#include <string.h>
#include <wchar.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include "test.h"

#define STRINGIZE0(x) L#x
#define STRINGIZE1(x) STRINGIZE0(x)
#define STRINGIZE(x) STRINGIZE1(x)

static char test[] = "wcstoul";
static int perfect = 0;

typedef struct {
   wchar_t * string;
   unsigned long int expected;
   int base;
   } DATA;

static DATA data1[] = {
                L" \f\n\r\t\v123456unsigned long constant", 123456, 0,
                L" \f\n\r\t\v+123456unsigned long constant", 123456, 0,
                L" \f\n\r\t\v-123456unsigned long constant", -123456, 0,
                L" \f\n\r\t\v" STRINGIZE(ULONG_MAX) L"unsigned long constant", ULONG_MAX, 0,
                L" \f\n\r\t\v" STRINGIZE(-ULONG_MAX) L"unsigned long constant", -ULONG_MAX, 0,


                L" \f\n\r\t\v123456unsigned long constant", 123456, 10,
                L" \f\n\r\t\v+123456unsigned long constant", 123456, 10,
                L" \f\n\r\t\v-123456unsigned long constant", -123456, 10,

                L" \f\n\r\t\v0x123456unsigned long constant", 0x123456, 0,
                L" \f\n\r\t\v+0X123456unsigned long constant", 0x123456, 0,
                L" \f\n\r\t\v-0x123456unsigned long constant", -0X123456, 0,


                L" \f\n\r\t\v0x123456unsigned long constant", 0x123456, 16,
                L" \f\n\r\t\v+0X123456unsigned long constant", 0x123456, 16,
                L" \f\n\r\t\v-0x123456unsigned long constant", -0X123456, 16,
                L" \f\n\r\t\v" STRINGIZE(ULONG_MAX) L"unsigned long constant", ULONG_MAX, 16,
                L" \f\n\r\t\v" STRINGIZE(-ULONG_MAX) L"unsigned long constant", -ULONG_MAX, 16,
                };


static DATA data2[] = {
                L" \f\n\r\t\v123456", 123456, 0,
                L" \f\n\r\t\v+123456", 123456, 0,
                L" \f\n\r\t\v-123456", -123456, 0,
                L" \f\n\r\t\v" STRINGIZE(ULONG_MAX), ULONG_MAX, 0,
                L" \f\n\r\t\v" STRINGIZE(-ULONG_MAX), -ULONG_MAX, 0,


                L" \f\n\r\t\v123456", 123456, 10,
                L" \f\n\r\t\v+123456", 123456, 10,
                L" \f\n\r\t\v-123456", -123456, 10,

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
                L" \f\n\r\t\v-+123456unsigned long constant", 0, 0,
                };
int wcstoul1Entry(){
int i;
wchar_t * endptr;
unsigned long int ulong_number;
wchar_t ulong_number_string[33];

/* Begin */

   startest();


/* with final string */

for( i = 0; i < sizeof(data1)/sizeof(data1[0]); i++ ) {

   if( data1[i].expected != wcstoul( data1[i].string, &endptr, data1[i].base ))
   {
      fail( 1 );
   }
   else
   {
      //Adjust comparison for 'UL' in LIMITS constants
      if (*endptr == (wchar_t)'U' && *++endptr == (wchar_t)'L')
        endptr++;

      if (endptr != wcsstr(data1[i].string, L"unsigned long constant" ) )
      {
        fail( 1 );
      }
   }
}

/* without final string */

for( i = 0; i < sizeof(data2)/sizeof(data2[0]); i++ ) {

   if( data2[i].expected != wcstoul( data2[i].string, &endptr, data2[i].base ))
   {
     fail( 12 );
   }
   else
   {
      //Adjust comparison for 'UL' in LIMITS constants
      if (*endptr == (wchar_t)'U' && *++endptr == (wchar_t)'L')
        endptr++;

      if (endptr != data2[i].string+wcslen( data2[i].string ) )
	fail( 12 );
   }
}

/* without sequence string */

for( i = 0; i < sizeof(specialData)/sizeof(specialData[0]); i++ ) {

   if( specialData[i].expected != wcstoul( specialData[i].string, &endptr, specialData[i].base ) 
       ||
       endptr != specialData[i].string )
      fail( 2 );
}

/* out of range */

errno = 0;

if( wcstoul( STRINGIZE(ULONG_MAX), &endptr, 16 ) != ULONG_MAX 
   ||
    errno != 0 )
   fail( 3 );

errno = 0;

if(
//    wcstoul( L"4294967296", &endptr, 10 ) != ULONG_MAX
    wcstoul( L"4294967296", &endptr, 10 ) != ULONG_MAX
   ||
    errno != ERANGE )
   fail( 31 );

errno = 0;

if( wcstoul( STRINGIZE(-ULONG_MAX), &endptr, 16 ) != -ULONG_MAX
   ||
    errno != 0 )
   fail( 4 );

if(
    wcstoul( L"-4294967296", &endptr, 10 ) != -ULONG_MAX
   ||
    errno != ERANGE )
   fail( 41 );

errno = 0;

/* NULL endptr */

if( wcstoul( L"-123456789", NULL, 10 ) != -123456789 )
   fail( 5 );


/* test for number base 2-32 */

ulong_number = ULONG_MAX;

for( i = 2; i < 33; i++ ) {

   unsigned long int ul;
   int j;
   int remainder;

   for( ul = ulong_number, j = 0 ;
        ul > 0;
         ) {
      remainder = ul%i;
      ul/=i;

      ulong_number_string[j] =  remainder < 10 ?
                               remainder+'0' : remainder-10+'a';
      j++;
      }

   ulong_number_string[j] = 0;

   _wcsrev( ulong_number_string );

   if( wcstoul( ulong_number_string, &endptr, i ) != ulong_number ) {
      fail(6);
      DbgPrint( "failed when base = %d\n", i );
      DbgPrint( "wcstoul() = %xuld, from %xuld\n", wcstoul( ulong_number_string, &endptr, i ), ulong_number );
      }
   }
  

   finish();
}
