/* @(#)File: format5.C      Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the effects of some output related bits in    >
    < the format control word.  These effects are observed on   >
    < values inserted into a strstreambuf.              >

    < The bits tested here are dec, oct, hex, showbase.     >

>HOW:   < The method used is to insert some value into an output    >
    < strstreambuf and check that the value that was written to     >
    < the buffer has the proper format.             >

    < If showbase is set, insertions are converted to an external   >
    < form that can be read according to the C++ lexical        >
    < conventions for integral constants. In blocks 0 thru 2,   >
    < showbase is set.                      >

    < 0. Set the base field to dec.  Write a positive integer.      >
    <    Check it comes out in the decimal base.            >

    < 1. Set the base field to oct.  Write a positive integer.      >
    <    Check it comes out in octal form preceded by a 0.      >

    < 2. Set the base field to hex.  Write a positive integer.      >
    <    Check it comes out in hexadecimal preceded by 0x.      >

    < In blocks 3 and 4, showbase is unset.             >

    < 3. Set the base field to oct.  Write a positive integer.      >
    <    Check it comes out in octal form unpreceded by a 0.    >

    < 4. Set the base field to hex.  Write a positive integer.      >
    <    Check it comes out in hexadecimal unpreceded by 0x.    >

    < In block 5, showbase is left to the default value.        >

    < 5. Check the default value of showbase is unset.      >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "format5()";         

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
#endif

// global variable declarations

/*--------------------------------------------------------------------*/
format5_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showbase flag with decimal output");

    {char buf[10];
    memset(buf, 0, 10);
    ostrstream s(buf, 8);   // create a stream
    s.setf(ios::showbase);  // set the flag
    s.setf(ios::dec, ios::basefield);   //set to base 10

    s << 9;                 // write to stream
    if (strncmp(buf, "9", 2))
    {
        fail ("unexpected result");
        DbgPrint("%s != 9\n", buf);
    }
    }

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showbase flag with oct output");

    {char buf[10];
    memset(buf, 0, 10);
    ostrstream s(buf, 8);   // create a stream
    s.setf(ios::showbase);  // set the flag
    s.setf(ios::oct, ios::basefield);   //set to base 8

    s << 9;         // write to stream
    if (strncmp(buf, "011", 3))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showbase flag with hex output");

    {char buf[10];
    memset(buf, 0, 10);
    ostrstream s(buf, 8);   // create a stream
    s.setf(ios::showbase);  // set the flag
    s.setf(ios::hex, ios::basefield);   //set to base 16

    s << 16;            // write to stream
    if (strncmp(buf, "0x10", 4))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showbase unset with oct output");

    {char buf[10];
    memset(buf, 0, 10);
    ostrstream s(buf, 8);   // create a stream
    s.unsetf(ios::showbase);    // unset the flag
    s.setf(ios::oct, ios::basefield);   //set to base 8

    s << 9;         // write to stream
    if (strncmp(buf, "11", 2))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test showbase unset with a hex output");

    {char buf[10];
    memset(buf, 0, 10);
    ostrstream s(buf, 8);   // create a stream
    s.unsetf(ios::showbase);    // unset the flag
    s.setf(ios::hex, ios::basefield);   //set to base 16

    s << 16;            // write to stream
    if (strncmp(buf, "10", 4))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test default value of showbase is unset");

    {char buf[10];
    memset(buf, 0, 10);
    ostrstream s(buf, 8);   // create a stream

    if (s.flags() & ios::showbase)
        fail ("showbase is set by default");
    }

        blexit();
/*--------------------------------------------------------------------*/
    anyfail();
    return 0;
}

#include "peren_another.c"
