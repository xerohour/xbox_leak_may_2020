/* @(#)File: ios5.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the manipulator that set the format control   >
    < enumerators dec, hex and oct.                 >

>HOW:   < In each block, use the manipulator to set the format flag     >
    < to one of the enumerators, then use flags() to check the  >
    < manipulator worked properly.                  >

    < 0. Set the dec enumerator with >>.                >
    < 1. Set the dec enumerator with <<.                >
    < 2. Set the hex enumerator with >>.                >
    < 3. Set the hex enumerator with <<.                >
    < 4. Set the oct enumerator with >>.                >
    < 5. Set the oct enumerator with <<.                >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ios5()";         

inline void print(char str[]) { DbgPrint( "%s\n", str); }
static void fail (char str[]) {
    local_flag =FAILED;
    DbgPrint( "\tFAILURE: %s\n", str);
    }

/*--------------------------------------------------------------------*/
#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#endif

// global variable declarations
#if defined(USING_OLD_IOS)

static streambuf * sb;     //creates an empty buffer
static iostream  s(sb);        // creates a stream using the empty buffer

#else
static streambuf * sb;     //creates an empty buffer
istream     is(sb);
ostream     os(sb);
#endif

/*--------------------------------------------------------------------*/
void ios5_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Set the format flag to ios::dec with the manipulator >>");

#if defined(USING_OLD_IOS)

    s.unsetf(~0l);      // clear all bits of the flag.
    s>>dec;         // set the format field
    if (s.flags() != ios::dec)
        fail ("manipulator failed");

#else

    is.unsetf(~0l);     // clear all bits of the flag.
    is >> dec;          // set the format field
    if (is.flags() != ios::dec)
        fail ("manipulator failed");

#endif
        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Set the format flag to ios::dec with the manipulator <<");

#if defined(USING_OLD_IOS)

    s.unsetf(~0l);      // clear all bits of the flag.
    s<<dec;         // set the format field
    if (s.flags() != ios::dec)
        fail ("manipulator failed");

#else

    os.unsetf(~0l);     // clear all bits of the flag.
    os<<dec;            // set the format field
    if (os.flags() != ios::dec)
        fail ("manipulator failed");

#endif 
        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Set the format flag to ios::hex with the manipulator >>");

#if defined(USING_OLD_IOS)
    s.unsetf(~0l);      // clear all bits of the flag.
    s>>hex;         // set the format field
    if (s.flags() != ios::hex)
        fail ("manipulator failed");
#else
    is.unsetf(~0l);     // clear all bits of the flag.
    is>>hex;            // set the format field
    if (is.flags() != ios::hex)
        fail ("manipulator failed");
#endif
        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Set the format flag to ios::hex with the manipulator <<");

#if defined(USING_OLD_IOS)
    s.unsetf(~0l);      // clear all bits of the flag.
    s<<hex;         // set the format field
    if (s.flags() != ios::hex)
        fail ("manipulator failed");
#else
    os.unsetf(~0l);     // clear all bits of the flag.
    os<<hex;            // set the format field
    if (os.flags() != ios::hex)
        fail ("manipulator failed");
#endif 
        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Set the format flag to ios::oct with the manipulator >>");

#if defined(USING_OLD_IOS)
    s.unsetf(~0l);      // clear all bits of the flag.
    s>>oct;         // set the format field
    if (s.flags() != ios::oct)
        fail ("manipulator failed");
#else
    is.unsetf(~0l);     // clear all bits of the flag.
    is>>oct;            // set the format field
    if (is.flags() != ios::oct)
        fail ("manipulator failed");
#endif 
        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Set the format flag to ios::oct with the manipulator <<");

#if defined(USING_OLD_IOS)
    s.unsetf(~0l);      // clear all bits of the flag.
    s<<oct;         // set the format field
    if (s.flags() != ios::oct)
        fail ("manipulator failed");
#else
    os.unsetf(~0l);     // clear all bits of the flag.
    os<<oct;            // set the format field
    if (os.flags() != ios::oct)
        fail ("manipulator failed");
#endif 
        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
