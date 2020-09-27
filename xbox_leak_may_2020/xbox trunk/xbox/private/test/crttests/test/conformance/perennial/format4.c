/* @(#)File: format4.C      Version: 1.1        Date:01/05/91   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the effects of some output related bits in    >
    < the format control word.  These effects are observed on   >
    < values inserted into a strstreambuf.              >

    < The bit tested here is showpoint.             >

>HOW:   < The method used is to insert some value into an output    >
    < strstreambuf and check that the value that was written to     >
    < the buffer has the proper format.             >

    < If showpoint is set, trailing zeroes and decimal points   >
    < appear in the result of a floating point conversion.      >

    < 0. Set showpoint and check trailing zeroes appear in the  >
    <    scientific notation.                   >

    < 1. Set showpoint and check trailing zeroes appear in the  >
    <    decimal notation.                      >

    < If showpoint is not set, the trailing zeroes are removed from >
    < the result and a decimal point only appear if it is followed  >
    < by a digit.                           >

    < 2. Unset showpoint and check trailing zeroes do not appear    >
    <    in the scientific notation.                >

    < 3. Unset showpoint and check trailing zeroes do not appear    >
    <    in the scientific notation.                >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >
    < 1.1 - 01/02/91: RFG - Substantial corrections and refinements.

>BUGS:  < None known.

========================================================================*/
#include "testhead.h"     

static char progname[] = "format4()";         

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
format4_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test that showpoint flag set causes display of trailing ");
    print("zeroes in scientific notation");

    {static char buf[20] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.setf(ios::showpoint);

    s << '{';
    s << 0.0000101;
    s << '}';

#undef EXPECT
#define EXPECT "{1.01000e-005}"

    if (strncmp(buf, EXPECT, strlen (EXPECT)))
        {DbgPrint( "expected = \"%s\", got = \"%s\"\n",
            EXPECT, buf);
        fail ("unexpected result");
        }
    }

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test that showpoint flag set causes display of trailing zeroes");
    print("in decimal notation");

    {static char buf[20] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.setf(ios::showpoint);

    s << '{';
    s << 3.3;
    s << '}';

#undef EXPECT
#define EXPECT "{3.30000}"

    if (strncmp(buf, EXPECT, strlen (EXPECT)))
        {DbgPrint( "expected = \"%s\", got = \"%s\"\n",
            EXPECT, buf);
        fail ("unexpected result");
        }
    }

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that when showpoint is unset, trailing zeroes and");
    print("decimal points do not appear in the scientific notation");

    {static char buf[20] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.unsetf(ios::showpoint);

    s << '{';
    s << 800000000.0;
    s << '}';

#undef EXPECT
#define EXPECT "{8e+008}"

    if (strncmp(buf, EXPECT, strlen (EXPECT)))
        {DbgPrint( "expected = \"%s\", got = \"%s\"\n",
            EXPECT, buf);
        fail ("unexpected result");
        }
    }

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that when showpoint is unset, trailing zeroes and");
    print("decimal points do not appear in the decimal notation");

    {static char buf[20] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.unsetf(ios::showpoint);

    s << '{';
    s << 9.9e3;
    s << '}';

#undef EXPECT
#define EXPECT "{9900}"

    if (strncmp(buf, EXPECT, strlen (EXPECT)))
        {DbgPrint( "expected = \"%s\", got = \"%s\"\n",
            EXPECT, buf);
        fail ("unexpected result");
        }
    }

        blexit();
/*--------------------------------------------------------------------*/
    anyfail();
    return 0;
}

#include "peren_another.c"
