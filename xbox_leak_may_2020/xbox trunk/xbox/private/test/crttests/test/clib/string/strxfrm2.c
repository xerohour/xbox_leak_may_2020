
/***************************************************************************

         Copyright (C) Microsoft Corp. 1993


Test:   strxfrm

Abstract:   Vefifies that strxfrm is effected by a change in the locale,
            and handles chars >127.

===========================================================================
Syntax

===========================================================================
Priority:               1
Products:               WIN NT


Revision History:

    Date        emailname       description
----------------------------------------------------------------------------
    11-03-93    lhanson         created
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <string.h>
#include <locale.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

#define MAX_LEN     32

int strxfrm2Entry(){
    unsigned char sz1[MAX_LEN] = "Ab";
    unsigned char sz2[MAX_LEN] = { (unsigned char)'A', 
                            (unsigned char)0x85,  // lower a grave
                            (unsigned char)'\0' };
    unsigned char szNew1[MAX_LEN],
                  szNew2[MAX_LEN];

    startest();

    checkne(setlocale( LC_ALL, "French_France.OCP" ), NULL);
    check(strxfrm(szNew1, sz1, MAX_LEN) == (size_t)-1);
    check(strxfrm(szNew2, sz2, MAX_LEN) == (size_t)-1);

    check(strcmp(szNew1, szNew2) <= 0);

    finish();
}
