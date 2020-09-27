/* @(#)File: ios3.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests format control function setf(b,f) as well     >
    < as the functions controlling the format components "width"    >
    < "precision" and "fill".                   >

>HOW:   < These functions either set or return the value of a format    >
    < variable of the class ios.                    >
    < The general test strategy is to use a value setting function  >
    < to set the variable to each of the possible values, and check >
    < with the value returning function that the variable actually  >
    < has the value we set it too.  This procedure allows to check  >
    < the two values against one another.               >

    < 0. Use setf(b,f) to set the format and flags() to check it.   >
    < 1. Check the return value of setf(b,f) is the old format.     >

    < 2. Use fill(c) to set the fill char and fill() to check it.   >
    < 3. Check the return value of fill(c) is the old fill char.    >

    < 4. Use width(c) to set the width and width() to check it. >
    < 5. Check the return value of width(c) is the old width.   >

    < 6. Use precision(c) to set the precision and precision()  >
    <    to check it.                       >
    < 7. Check the return value of precision(c) is the old      >
    <    precision.                         >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ios3()";         

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
static streambuf * sb;     //creates an empty buffer
static ios s(sb);      // creates a stream using the empty buffer

static long old_format, new_format;
/*--------------------------------------------------------------------*/
ios3_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test function setf(b, f)");

    s.flags(0l);        // clear all bits of format
    old_format = 0l;
    long setf_val;

    for (int i= 0; i<31; i++)
        {setf_val = s.setf(~0l, 1l<< i);
        if (setf_val != old_format)
            {local_flag = FAILED;
            DbgPrint("\tsetf(b,f) returnd %lx instead of %lx\n",
                    setf_val, old_format);
            }
        new_format = old_format | (1l << i);
        if (s.flags() != new_format)
            {local_flag = FAILED;
            DbgPrint( "\tformat is %lx instead of %lx\n",
                    s.flags(), new_format);
            }
        old_format = new_format;
        }

#endif 

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test function setf(b, f)");

    s.flags(~0l);       // set all bits of format
    old_format = ~0l;
    long unsetf_val;

    for (i= 0; i<31; i++)
        {unsetf_val = s.setf(0l, 1l << i);
        if (unsetf_val != old_format)
            {local_flag = FAILED;
            DbgPrint("\tsetf(b,f) returnd %lx instead of %lx\n",
                    unsetf_val, old_format);
            }
        new_format = old_format & ~(1l << i);
        if (s.flags() != new_format)
            {local_flag = FAILED;
            DbgPrint( "\tformat is %lx instead of %lx\n",
                    s.flags(), new_format);
            }
        old_format = new_format;
        }

#endif 

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check fill(c) against fill()");

    s.fill(0);
    if (s.fill() != 0)
        fail("could not set fill to all zeroes");

    s.fill(~0);
    if (s.fill() != ~0)
        fail("could not set fill to all ones");

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check the return value of fill(c) is the previous value");

    s.fill(0);
    if (s.fill(~0) != 0)
        fail("unexpected high bit in return value");
    if (s.fill(0) != ~0)
        fail("unexpected low bit in return value");

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check width(c) against width()");

    s.width(0);
    if (s.width() != 0)
        fail("could not set width to all zeroes");

    s.width(~0);
    if (s.width() != ~0)
        fail("could not set width to all ones");

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check the return value of width(c) is the previous value");

    s.width(0);
    if (s.width(~0) != 0)
        fail("unexpected high bit in return value");
    if (s.width(0) != ~0)
        fail("unexpected low bit in return value");

        blexit();
/*--------------------------------------------------------------------*/
block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check precision(c) against precision()");

    s.precision(0);
    if (s.precision() != 0)
        fail("could not set precision to all zeroes");

    s.precision(~0);
    if (s.precision() != ~0)
        fail("could not set precision to all ones");

        blexit();
/*--------------------------------------------------------------------*/
block7: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check the return value of precision(c) is the previous value");

    s.precision(0);
    if (s.precision(~0) != 0)
        fail("unexpected high bit in return value");
    if (s.precision(0) != ~0)
        fail("unexpected low bit in return value");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
        return 0;
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
