
/****************************************************************************
              Copyright (C) 1992, Microsoft Corporation
Method:              cl strlwr2.c
                     strlwr2
Switches:            NONE
Hard/Soft Needs:
Abstract:            This test verifies the _strlwr() function.
OS Version:          REAL/PROT/ALL
CPU:                 8086
Dependencies:        NONE
Environment Vars:    NONE
Verification Method: COMPILE/EXECUTE/COMPARE
Notes:               must include STRING.H
                     Tests described in the testplan STRING.DOC
Products:            C800
Test Type:       DEPTH

Revision History:
 Date              emailname      description
---------------------------------------------------------------------------
 [ 0] 19-May-92    edv            created

---------------------------------------------------------------------------
EndHeader:
*****************************************************************************/

#include <string.h>
#include <test.h>

static char test[] = __FILE__;
static int perfect;

static char string1[70] = { "ABC DEF GHI JKL MNO PQR STU VWX YZA BCD EFG HIJ "
                     "KLM NOP QRS TUV WXY Z\0"
                   };


static char baseln1[70] = { "abc def ghi jkl mno pqr stu vwx yza bcd efg hij "
                     "klm nop qrs tuv wxy z\0"
                   };

static char string2[100] = { "1A2B3C4D5E6F7G8H9I0J!K@L#M$N%O^P&Q*R(S)T_U+V-W=X :"
                      "; [Y]Z1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ<><>?/'\0"
                    };

static char baseln2[100] = { "1a2b3c4d5e6f7g8h9i0j!k@l#m$n%o^p&q*r(s)t_u+v-w=x :"
                      "; [y]z1234567890abcdefghijklmnopqrstuvwxyz<><>?/'\0"
                    };

static char string3[200] = { "ABC DEF GHI JKL MNO PQR STU VWX YZA BCD EFG HIJ kl"
                      "; [Y]Z1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ<><>?/'x"
                      "1a2b3c4d5e6f7g8h9i0j!k@l#m$n%o^p&q*r(s)t_u+v-w=x :"
                      "; [y]z1234567890abcdefghijklmnopqrstuvwxyz<><>?/'\0"
                    };

static char baseln3[200] = { "abc def ghi jkl mno pqr stu vwx yza bcd efg hij kl"
                      "; [y]z1234567890abcdefghijklmnopqrstuvwxyz<><>?/'x"
                      "1a2b3c4d5e6f7g8h9i0j!k@l#m$n%o^p&q*r(s)t_u+v-w=x :"
                      "; [y]z1234567890abcdefghijklmnopqrstuvwxyz<><>?/'\0"
                    };


int strlwr2Entry(){
    char *p;
    int i;


    startest();



/*  Scenario 4: Verify that the function works correctly with
                larger strings.
*/

    p = _strlwr( string1 );
    checke( p, string1 );

    for (i = 0; i < sizeof( string1 ); i++)
        checke( string1[i], baseln1[i] );




    p = _strlwr( string2 );
    checke( p, string2 );

    for (i = 0; i < sizeof( string2 ); i++)
        checke( string2[i], baseln2[i] );




    p = _strlwr( string3 );
    checke( p, string3 );

    for (i = 0; i < sizeof( string3 ); i++)
        checke( string3[i], baseln3[i] );





    finish();
}
