
/***************************************************************************

           Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: strcspn

          size_t strcspn(const char *, const char *);

OS Version:
CPU:
Dependencies:       NONE
Environment Vars:   NONE
Verification Method:    TBD
Priority:       1/2/3/4
Notes:          NONE
Products:


Revision History:

    Date    emailname   description
----------------------------------------------------------------------------
    18-Feb-93   xiangjun    created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect = 0;

static char string0[] = "1234567890abcdefghijABCDEFGHIJ";
static char string1[] = "abcde";
static char string2[] = "ABCDEFGHIJLKMNOPQ";
static char string3[] = "abcdeABCD";
static char string4[] = "uvxwzy";
static char string5[] = "abcdefghiABCDEFGHI";

int strcspn4Entry(){

/* Begin */

   startest();

/* normal cases */

   if( strcspn( string0, string1 ) != 10 )
      fail(0);

   if( strcspn( string0, string2 ) != 20 )
      fail(1);

   if( strcspn( string0, string3 ) != 10 )
      fail(2);

   if( strcspn( string0, string4 ) != 30 )
      fail(3);

/* return 0 case */

   if( strcspn( string5, string1 ) != 0 )
      fail(4);

/* end */

   finish();
}
