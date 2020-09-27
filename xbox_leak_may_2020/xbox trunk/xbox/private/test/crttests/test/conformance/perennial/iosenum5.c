/* @(#)File: iosenum5.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyuppercase 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the constant static members used to collec-   >
    < tively identify a collection of format bits.          >

>HOW:   < Each block tests one such static member.          >

    < 0. The manual states (p.4) that "the fields left, right and   >
    <    internal are collectively identified by the static member  >
    <    ios::adjustfield."  This statement is interpreted as   >
    <    saying that adjustfield == left | right | internal.  This  >
    <    block checks that is true.                 >

    < 1. The manual states (p.4) that "the fields dec, oct and  >
    <    hex are collectively identified by the static member   >
    <    ios::basefield."  This statement is interpreted as     >
    <    saying that basefield == dec | oct | hex.  This block  >
    <    checks that is true.                   >

    < 2. The manual states (p.5) that "the fields scientific and
    <    fixed are collectively identified by the static member >
    <    ios::floatfield."  This statement is interpreted as    >
    <    saying that floatfield == scientific | fixed.  This block  >
    <    checks that is true.                   >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "iosenum5()";         

inline void print(char str[]) { DbgPrint( "%s\n", str); }
static void fail (char str[]) {
    local_flag =FAILED;
    DbgPrint( "\tFAILURE: %s\n", str);
    }

/*--------------------------------------------------------------------*/
#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#endif

// global variable declarations
/*--------------------------------------------------------------------*/
void iosenum5_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test that ios:: adjustfield collectively identifies");
    print("ios::left, ios::right, ios::internal.");

    if (ios::adjustfield != (ios::left | ios::right | ios::internal))
        fail ("unexpected value");

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test that ios:: basefield collectively identifies");
    print("ios::dec, ios::oct, ios::hex.");

    if (ios::basefield != (ios::dec | ios::oct | ios::hex))
        fail ("unexpected value");

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test that ios:: floatfield collectively identifies");
    print("ios::scientific and ios::fixed.");

    if (ios::floatfield != (ios::scientific | ios::fixed))
        fail ("unexpected value");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
