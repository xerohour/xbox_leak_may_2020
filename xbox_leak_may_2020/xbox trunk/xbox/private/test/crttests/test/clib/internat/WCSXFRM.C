
/***************************************************************************

		   Copyright (c) 1992, Microsoft Corporation

Method:



Switches:   NONE


Abstract:   Test for: wcsxfrm

          wchar_t * wcsxfrm(const wchar_t *, const wchar_t *, size_t);

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

static char test[] = "wcsxfrm";
static int perfect = 0;

static wchar_t wcs_string0[100];
static wchar_t wcs_string1[] = {1,65033,L'A',L'z',0};
static wchar_t wcs_string2[] = L"ABCDEFGH['ijklmnopqrstuvwxyz";


int WCSXFRMEntry(){

int i, n;

/* Begin */

   startest();

/* testing for C LOCALE for now */

   n =  (int)wcsxfrm( wcs_string0, wcs_string1, 30 );
   i = 0;
   while( wcs_string1[i] ) {
      if( wcs_string0[i] !=  wcs_string1[i] )
         fail( i );
      i++;
      }
   if( n != i )
      fail( 100 );

   n = (int)wcsxfrm( wcs_string0, wcs_string1, 0 );
   if( n != i )
      fail( 200 );

   n = (int)wcsxfrm( wcs_string0, wcs_string2, 100 );
   i = 0;
   while( wcs_string2[i] ) {
      if( wcs_string0[i] !=  wcs_string2[i] )
         fail( i+300 );
      i++;
      }
   if( n != i )
      fail( 300 );


   n = (int)wcsxfrm( wcs_string0, wcs_string2, 13 );
   if( n != i )
      fail( 400 );


/* end */

   finish();
}
