/* @(#)File: iosenum1.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the values of the enumerators in the class    >
    < ios's "io_state" and "seek_dir" enumerations.         >

>HOW:   < Each block tests the value of one enumerator.         >

    < Check the io_state enumerators:               >
    < 0. goodbit is 0                       >
    < 1. eofbit  is 1                       >
    < 2. failbit is 2                       >
    < 3. badbit  is 4                       >

    < Check the seek_dir enumerators:               >
    < 4. beg is 0                           >
    < 5. cur is 1                           >
    < 6. end is 2                           >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < In the June 9 version of the ATT documentation, the io_state  >
    < enumeration is improperly documented  in the IOS(3C++)    >
    < man page.  The proper documentation on io_state is to be  >
    < found on page 3-5 of the library manual's section 3 on    >
    < iostream examples.                        >
========================================================================*/
#include "testhead.h"     

static char progname[] = "iosenum1()";         

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
void iosenum1_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator goodbit of the io_state enumeration");

    if (ios::goodbit != 0)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator eofbit of the io_state enumeration");

    if (ios::eofbit != 1)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator failbit of the io_state enumeration");

    if (ios::failbit != 2)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator badbit of the io_state enumeration");

    if (ios::badbit != 4)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator beg of the seek_dir enumeration");

    if (ios::beg != 0)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator cur of the seek_dir enumeration");

    if (ios::cur != 1)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test enumerator end of the seek_dir enumeration");

    if (ios::end != 2)
        fail ("incorrect enumerator value");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
