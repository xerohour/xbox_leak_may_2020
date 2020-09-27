
/***************************************************************************

         Copyright (C) Microsoft Corp. 1993


Test:   strcoll

Abstract:   Vefifies that strcoll is effected by a change in the locale,
            and handles chars >127.  Also verifies that setting LC_COLLATE
            alone performs correct sort order for graphic chars.
            
===========================================================================
Syntax

===========================================================================
Priority:               1
Products:               WIN NT


Revision History:

    Date        emailname       description
----------------------------------------------------------------------------
    11-03-93    lhanson         created
    11-22-94    a-timke         Modified for NT3.5 results.
    01-28-98    a-hemalk        64bit warning (C4311) fix
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <string.h>
#include <locale.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


int strcoll4Entry(){
    unsigned char *psz1 = "AaBbC\x8A\x8B\xC0\xCF\xD0Zz";
    unsigned char *psz2 = "aabbc\x9A\x8B\xE0\xEF\xF0zz";
    unsigned char *psz3 = "\x9A\x8B\xE0\xEF\xF0zzAaBbC";

    startest();

    check(strcoll(psz1, psz2) >= 0);

    // French locale with LC_COLLATE only
    
    checknNULL(setlocale(LC_ALL, "French_France.OCP"));
#if defined(TEST_WIN95)
    check(strcoll(psz1, psz2) <= 0);
#else
    check(strcoll(psz1, psz2) > 0);
#endif
    check(strcoll(psz1, psz3) >= 0);  // Should sort alpha before graphic

    // French locale with LC_ALL

    checknNULL(setlocale(LC_ALL, "French_France.OCP"));
#if defined(TEST_WIN95)
    check(strcoll(psz1, psz2) <= 0);
#else
    check(strcoll(psz1, psz2) > 0);
#endif
    check(strcoll(psz1, psz3) >= 0);  // Should sort alpha before graphic  

    finish();
}
