
/***************************************************************************

         Copyright (C) Microsoft Corp. 1993


Test:   _stricol

Abstract:   Vefifies that _stricoll is effected by a change in the locale,
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
    11-22-94    a-timke         Modified for NT3.5 results.
    11-07-2000  a-antra         Modified to match characters with locales.
----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <string.h>
#include <locale.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;


int _stricolEntry(){
//    unsigned char *psz1 = "AaBbC\x8A\x8B\xC0\xCF\xD0Zz";
//    unsigned char *psz2 = "aabbc\x9A\x8B\xE0\xEF\xF0zz";

    unsigned char *psz1 = "AaBbC\x82\x8B\xEC\x8C\xA3Zz";
    unsigned char *psz2 = "aabbc\x90\x8B\xED\xD7\xE9zz";

    startest();

    checkne(_stricoll(psz1, psz2), 0);

    checkne(setlocale( LC_ALL, "French_France.OCP" ), NULL);
#if defined(TEST_WIN95)  /*prior to NLS work of 2/97. */
    checke(_stricoll(psz1, psz2), 0);
    checke(_stricoll(psz2, psz1), 0);
#else
    checke(_stricoll(psz1, psz2), 0);
    checke(_stricoll(psz2, psz1), 0);
#endif
    finish();
}
