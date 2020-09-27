/* @(#)File: iosenum4.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyuppercase 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the values of some enumerators in the class   >
    < ios's format control enumeration.  The rest of the        >
    < enumerators in this enumeration are tested in iosenum3.C  >

>HOW:   < Each block tests the value of one enumerator.         >

    < 0. showbase is 0200                       >
    < 1. showpoint is 0400                      >
    < 2. uppercase is 01000                     >
    < 3. showpos is 02000                       >
    < 4. scientific is 04000                    >
    < 5. fixed is 010000                        >
    < 6. unitbuf is 020000                      >
    < 7. stdio is 040000                        >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "iosenum4()";         

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
void iosenum4_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();

#if defined(USING_OLD_IOS)

/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator showbase of the format control enumeration");

    if (ios::showbase!= 0200)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator showpoint of the format control enumeration");

    if (ios::showpoint != 0400)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator uppercase of the format control enumeration");

    if (ios::uppercase != 01000)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator showpos of the format control enumeration");

    if (ios::showpos != 02000)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator scientific of the format control enumeration");

    if (ios::scientific != 04000)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator fixed of the format control enumeration");

    if (ios::fixed != 010000)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator unitbuf of the format control enumeration");

    if (ios::unitbuf != 020000)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block7: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator stdio of the format control enumeration");

    if (ios::stdio != 040000)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
#endif 
        anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
