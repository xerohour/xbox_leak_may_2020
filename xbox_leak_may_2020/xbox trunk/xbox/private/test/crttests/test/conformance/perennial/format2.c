/* @(#)File: format2.C      Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the effects of some output related bits in    >
    < the format control word.  These effects are observed on   >
    < values inserted into a strstreambuf.              >

    < The bits tested here are showpos and  uppercase.      >

>HOW:   < The method used is to insert some value into an output    >
    < strstreambuf and check that the value that was written to     >
    < the buffer has the proper format.             >

    < Blocks 0 thru 3 test showpos.  When showpos is set, a "+" >
    < is inserted into the decimal conversion of a positive     >
    < integral value.                       >

    < 0. Set the base field to dec.  Write a positive integer.      >
    <    Check it comes out preceded by a "+" sign.         >

    < 1. Set the base field to oct.  Write a positive integer.      >
    <    Check it doesn't come out preceded by a "+" sign.      >

    < 2. Set the base field to hex.  Write a positive integer.      >
    <    Check it doesn't come out preceded by a "+" sign.      >

    < 3. Set the base field to dec.  Unset the showpos flag.    >
    <    Write a positive integer.  Check it comes out preceded >
    <    by a "+" sign.                     >

    < Blocks 4 thru 7 test uppercase.  When uppercase is set, an    >
    < uppercase "X" is used for hexadecimal conversions and an  >
    < uppercase "E" for floating point numbers in scientific    >
    < notation.                         >

    < 4. Set uppercase and check that a "X" comes out when you  >
    <    output a hexadecimal integer.              >

    < 5. Unset uppercase and check that a "x" comes out when you    >
    <    output a hexadecimal integer.              >

    < 6. Set uppercase and check that a "E" comes out when you  >
    <    output a floating point number in scientific notation. >

    < 7. Unset uppercase and check that a "e" comes out when you    >
    <    output a floating point number in scientific notation. >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "format2()";         

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
#include <strstream>
using namespace std;
#endif

// global variable declarations

/*--------------------------------------------------------------------*/
format2_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showpos flag with decimal output");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.setf(ios::showpos);   // set the flag
    s.setf(ios::dec, ios::basefield);   //set to base 10

    s << 9;         // write to stream
    if (strncmp(buf, "+9", 2))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showpos flag with oct output");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.setf(ios::showpos);   // set the flag
    s.setf(ios::oct, ios::basefield);   //set to base 8

    s << 9;         // write to stream
    if (strncmp(buf, "11", 2))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showpos flag with hex output");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.setf(ios::showpos);   // set the flag
    s.setf(ios::hex, ios::basefield);   //set to base 16

    s << 16;            // write to stream
    if (strncmp(buf, "10", 2))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showpos unset with decimal output");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.unsetf(ios::showpos); // set the flag
    s.setf(ios::dec, ios::basefield);   //set to base 16

    s << 16;            // write to stream
    if (strncmp(buf, "16", 2))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test uppercase set with a hex output");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.setf(ios::uppercase); // set the flag
    s.setf(ios::showbase);  // set the show base flag
    s.setf(ios::hex, ios::basefield);   //set to base 16

    s << 16;            // write to stream
    if (strncmp(buf, "0X10", 4))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test uppercase unset with a hex output");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.unsetf(ios::uppercase);   // set the flag
    s.setf(ios::showbase);  // set the show base flag
    s.setf(ios::hex, ios::basefield);   //set to base 16

    s << 16;            // write to stream
    if (strncmp(buf, "0x10", 4))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test uppercase set with a scientific notation");

    {char buf[30];
    ostrstream s(buf, 8);   // create a stream
    s.precision(1);     // set the precision to 1
    s.setf(ios::uppercase); // set the flag
    s.setf(ios::scientific, ios::floatfield);   //set to scientific not.

    s << 0.005;         // write to stream
    if (strncmp(buf, "5.0E-003", 7))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block7: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test uppercase unset with scientific notation");

    {char buf[30];
    ostrstream s(buf, 8);   // create a stream
    s.unsetf(ios::uppercase);   // set the flag
    s.precision(1);     // set the precision to 1
    s.setf(ios::scientific, ios::floatfield);   //set to scientific not.

    s << 0.005;         // write to stream
    if (strncmp(buf, "5.0e-003", 7))
    {
        fail ("unexpected result");
        DbgPrint("\t%s != 5.0e-003\n", buf);
    }
    }

        blexit();
/*--------------------------------------------------------------------*/
    anyfail();
    return 0;
}

#include "peren_another.c"
