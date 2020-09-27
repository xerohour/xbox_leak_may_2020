
/***************************************************************************

           Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: strpbrk

          char * strpbrk(const char *, const char *);

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

static char string1[] = "ABCDEFGHijklmnopq";
static char string2[] = {'a','b',
               /* 1st break */     '?',    'c','d',
               /* 2nd break */     ',',
               /* 3nd break */     1,        'e',
               /* 4nd break */     'z',     'Z',0};
static char str_brkset[] = {',',1,'?','z',0};

int strpbrk4Entry(){

static char * str_ptr;

/* Begin */

   startest();

   /* expect NULL resulte */

   if( NULL != (str_ptr = strpbrk( string1, str_brkset )) )
      fail(0);

   /* other normal cases */

   if( NULL == (str_ptr = strpbrk( string2, str_brkset )) ||
       *str_ptr != '?' )
      fail(1);

   if( NULL == (str_ptr = strpbrk( str_ptr+1, str_brkset )) ||
       *str_ptr != ',' )
      fail(2);

   if( NULL == (str_ptr = strpbrk( str_ptr+1, str_brkset )) ||
       *str_ptr != 1 )
      fail(3);

   if( NULL == (str_ptr = strpbrk( str_ptr+1, str_brkset )) ||
       *str_ptr != 'z' )
      fail(4);

   if( str_ptr = strpbrk( str_ptr+1, str_brkset ) )
      fail(5);

/* end */

   finish();
}
