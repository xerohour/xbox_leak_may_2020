/* @(#)File: format6.C      Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the effects of some input related bits in >
    < the format control word.  These effects are observed on   >
    < values extracted from a strstreambuf.             >

    < The bits tested here are dec, oct, and hex.           >

>HOW:   < In blocks 0 thru 2, the method used is to extract a value     >
    < from an input strstreambuf and check that the value was   >
    < properly converted from the specified base.           >

    < 0. Set the base field to dec.  Extract a positive integer.    >
    <    Check it was left in base 10.              >

    < 1. Set the base field to oct.  Extract a positive integer.    >
    <    Check it was converted from base 8 to base 10.     >

    < 2. Set the base field to hex.  Extract a positive integer.    >
    <    Check it was converted from base 16 to base 10.        >

    < Blocks 3 and 4 Check the conversion mechanism when all the    >
    < base flags are unset.                     >

    < 3. Unset all the base flags.  Extract a decimal integer.  >
    <    Check it is left in decimal.               >

    < 4. Unset all the base flags.  Extract an octal integer.   >
    <    Check it is converted to decimal.              >

    < 5. Unset all the base flags.  Extract hex integer.        >
    <    Check it is converted to decimal.              >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "format6()";         

inline void print(char str[]) { DbgPrint( "%s\n", str); }
static void fail (char str[]) {
    local_flag =FAILED;
    DbgPrint( "\tFAILURE: %s\n", str);
    }

/*--------------------------------------------------------------------*/
#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <strstrea.h>
#else
#include <iostream>
#include <strstream>
#endif

// global variable declarations
static int i;
/*--------------------------------------------------------------------*/
void format6_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test extraction with decimal conversion");

    {static char buf[10] = "10";
    istrstream s(buf, 8);   // create a stream
    s.setf(ios::dec, ios::basefield);   //set to base 10

    s >> i;
    if (i != 10)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test extraction with oct conversion");

    {static char buf[10] = "10";
    istrstream s(buf, 8);   // create a stream
    s.setf(ios::oct, ios::basefield);   //set to base 8

    s >> i;
    if (i != 010)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test extraction with hex conversion");

    {static char buf[10] = "10";
    istrstream s(buf, 8);   // create a stream
    s.setf(ios::hex, ios::basefield);   //set to base 16

    s >> i;
    if (i != 0x10)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test decimal extraction when base flags are unset");

    {static char buf[10] = "10";
    istrstream s(buf, 8);   // create a stream
    s.unsetf(ios::basefield);   // unset the flag
    
    s >> i;
    if (i != 10)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test octal extraction when base flags are unset");

    {static char buf[10] = "010";
    istrstream s(buf, 8);   // create a stream
    s.unsetf(ios::basefield);   // unset the flag
    
    s >> i;
    if (i != 010)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test hexadecimal extraction when base flags are unset");

    {static char buf[10] = "0x10";
    istrstream s(buf, 8);   // create a stream
    s.unsetf(ios::basefield);   // unset the flag
    
    s >> i;
    if (i != 0x10)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
    anyfail();
}

#include "peren_another.c"
