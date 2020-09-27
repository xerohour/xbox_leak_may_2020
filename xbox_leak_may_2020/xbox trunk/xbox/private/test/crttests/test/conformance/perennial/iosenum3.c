/* @(#)File: iosenum3.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the values of some enumerators in the class   >
    < ios's format control enumeration.  The rest of the        >
    < enumerators in this enumeration are tested in iosenum4.C  >

>HOW:   < Each block tests the value of one enumerator.         >

    < 0. skipws is 1                        >
    < 1. left is 2                          >
    < 2. right is 4                         >
    < 3. internal is 010                        >
    < 4. dec is 020                         >
    < 5. oct is 040                         >
    < 6. hex is 0100                        >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "iosenum3()";         

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
void iosenum3_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
#if defined(USING_OLD_IOS)

/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator skipws of the format control enumeration");

    if (ios::skipws!= 1)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator left of the format control enumeration");

    if (ios::left != 2)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator right of the format control enumeration");

    if (ios::right != 4)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator internal of the format control enumeration");

    if (ios::internal != 010)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator dec of the format control enumeration");

    if (ios::dec != 020)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator oct of the format control enumeration");

    if (ios::oct != 040)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator hex of the format control enumeration");

    if (ios::hex != 0100)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
#endif
        anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
