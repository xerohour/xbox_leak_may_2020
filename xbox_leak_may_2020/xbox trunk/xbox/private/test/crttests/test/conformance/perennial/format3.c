/* @(#)File: format3.C      Version: 1.2        Date: 01/02/91  */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, 1990, Perennial, All Rights Reserved         */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the effects of some output related bits in    >
    < the format control word.  These effects are observed on   >
    < values inserted into a strstreambuf.              >

    < The bits tested here are scientific and fixed.        >

>HOW:   < The method used is to insert some value into an output    >
    < strstreambuf and check that the value that was written to     >
    < the buffer has the proper format.             >

    < The flags scientific and fixed control the fomat a floating   >
    < point variable is converted for insertion into a stream.  >

    < 0. Set scientific and check the valueis converted using   >
    <    scientific notation.                   >

    < 1. Set the precision to n.  Check the scientific notation >
    <    has n digits to the right of the decimal point.        >

    < 2. Set fixed and check the value is converted to decimal  >
    <    notation.                          >

    < 3. Set the precision to n.  Check the decimal notation    >
    <    has n digits to the right of the decimal point.        >

    < Blocks 4 thru 6 test the floating point conversion when   >
    < neither scientific nor fixed are set.             >

    < 4. Unset both scientific and fixed.  Check the value is   >
    <    converted to scientific when the exponent is less than -4. >

    < 5. Unset both scientific and fixed.  Check the value is   >
    <    converted to decimal when the exponent is between -4 and   >
    <    the precision.                     >

    < 6. Unset both scientific and fixed.  Check the value is   >
    <    converted to scientific when the exponent is more than     >
    <    the precision.                     >

>MODS:  < Ver 1.3 01/02/91 RFG - Substantial corrections and refinements.
    < Ver 1.1, 07/18/90, DRE: 
    <    The previous version used the precision incorrectly. Fixed it.
    < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < No bugs or system dependencies known.
========================================================================*/
#include "testhead.h"     

static char progname[] = "format3()";

#define MYBUFSIZE   512

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
format3_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test scientific flag causes conversion to scientific notation");

    {static char buf[MYBUFSIZE] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.setf(ios::scientific, ios::floatfield);

    s << '{';
    s << 10.1;
    s << '}';

#undef EXPECT
#define EXPECT "{1.010000e+001}"

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
    print("Check the scientific notation has \"precision\" digits to");
    print("of the decimal point");

    {static char buf[MYBUFSIZE] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.setf(ios::scientific, ios::floatfield);
    s.precision(1);

    s << '{';
    s << 220.0;
    s << '}';

#undef EXPECT
#define EXPECT "{2.2e+002}"

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
    print("Test fixed flag causes conversion to decimal notation");

    {static char buf[MYBUFSIZE] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.setf(ios::fixed, ios::floatfield);

    s << '{';
    s << 3.3;
    s << '}';

#undef EXPECT
#define EXPECT "{3.300000}"

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
    print("Check the decimal notation has \"precision\" digits to");
    print("of the decimal point");

    {static char buf[MYBUFSIZE] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.precision(1);
    s.setf(ios::fixed, ios::floatfield);

    s << '{';
    s << 4.4e2;
    s << '}';

#undef EXPECT
#define EXPECT "{440.0}"

    if (strncmp(buf, EXPECT, strlen (EXPECT)))
        {DbgPrint( "expected = \"%s\", got = \"%s\"\n",
            EXPECT, buf);
        fail ("unexpected result");
        }
    }

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("When both scientific and fixed are unset, the value is");
    print("converted to scientific notation if the would-be exponent");
    print("is less than - 4");

    {char buf[MYBUFSIZE];
    ostrstream s(buf, 18);  // create a stream
    s.precision(2);
    s.setf(ios::showpoint);
    s.unsetf(ios::fixed && ios::scientific);

    s << '{';
    s << 0.0000051;
    s << '}';

#undef EXPECT
#define EXPECT "{5.1e-006}"

    if (strncmp(buf, EXPECT, strlen (EXPECT)))
        {DbgPrint( "expected = \"%s\", got = \"%s\"\n",
            EXPECT, buf);
        fail ("unexpected result");
        }
    }

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("When both scientific and fixed are unset, the value is");
    print("converted to decimal notation if the would-be exponent");
    print("is between -4 and the precision");

    {static char buf[MYBUFSIZE] = "                   ";
    ostrstream s(buf, 18);  // create a stream
    s.precision(3);     // three **SIGNIFICANT** digits
    s.setf(ios::showpoint);
    s.unsetf(ios::fixed && ios::scientific);

    s << '{';
    s << 0.006;
    s << '}';

#undef EXPECT
#define EXPECT "{0.00600}"

    if (strncmp(buf, EXPECT, strlen (EXPECT)))
        {DbgPrint( "expected = \"%s\", got = \"%s\"\n",
            EXPECT, buf);
        fail ("unexpected result");
        }
    }

        blexit();
/*--------------------------------------------------------------------*/
block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("When both scientific and fixed are unset, the value is");
    print("converted to scientific notation if the would-be exponent");
    print("is larger than the precision");

    {char buf[MYBUFSIZE];
    ostrstream s(buf, 18);  // create a stream
    s.precision(3);
    s.setf(ios::showpoint);
    s.unsetf(ios::fixed && ios::scientific);

    s << '{';
    s << 70700.0;
    s << '}';

#undef EXPECT
#define EXPECT "{7.07e+004}"

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
