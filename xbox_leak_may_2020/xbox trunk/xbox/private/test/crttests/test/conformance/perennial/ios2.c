/* @(#)File: ios2.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the format state functions of class ios   >

>HOW:   < These functions either set or return the value of the format  >
    < variable of class ios.                    >

    < 0. Use flags(long) to set format to a given value and use     >
    <    flags() to check format was set to that value.     >

    < 1. Check that the return value of flags(long) is the old  >
    <    value of format.                       >

    < 2. Check that the function setf can be used to set to one >
    <    each bit of the format variable.  Also check the return    >
    <    value of setf (old value of format) is correct.        >

    < 3. Check that the function unsetf can be used to set to zero  >
    <    each bit of the format variable.  Also check the return    >
    <    value of unsetf (old value of format) is correct.      >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ios2()";         

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
#if defined(USING_OLD_IOS)
static streambuf * sb;     //creates an empty buffer
static ios s(sb);      // creates a stream using the empty buffer
#else
static char        buf[10];
istrstream  s(buf);
#endif 

static long old_format, new_format;

#if defined(USING_OLD_IOS)
#define LONG_5 0x55555555l
#define LONG_A 0xaaaaaaaal
#else
#define LONG_5 0x5555
#define LONG_A 0x7aaa
#endif 
/*--------------------------------------------------------------------*/
void ios2_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Testing member function flags()");

    s.flags(LONG_5);
    s.flags(LONG_A);
    if (s.flags() != LONG_A)
        fail("unexpected value in test # 1");
    s.flags(LONG_5);
    if (s.flags() != LONG_5)
        fail("unexpected value in test # 2");

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Testing member function flags(long)");

    s.flags(LONG_5);
    if (s.flags(LONG_A) != LONG_5)
        fail("unexpected value in test # 1");
    if (s.flags(LONG_5) != LONG_A)
        fail("unexpected value in test # 2");

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Testing member function setf()");

    s.flags(0l);        // clear all bits of format
    old_format = 0l;
    long setf_val;

#if defined(USING_OLD_IOS)
    for (int i= 0; i<31; i++)
#else
    for (int i= 0; i<15; i++)
#endif 
        {setf_val = s.setf(1l << i);
        if (setf_val != old_format)
            {local_flag = FAILED;
            DbgPrint( "\tsetf() returned %lx instead of %lx\n",
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

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Testing member function unsetf()");

#if defined(USING_OLD_IOS)
    s.flags(~0l);       // set all bits of format
    old_format = ~0l;
#else
    s.flags(~0 & ios::_Fmtmask);        // set all bits of format
    old_format = ~0 & ios::_Fmtmask;
#endif 
    long unsetf_val;

#if defined(USING_OLD_IOS)
    for (i= 0; i<31; i++)
#else
    for (i= 0; i<15; i++)
#endif 
        {
#if defined(USING_OLD_IOS)
        unsetf_val = s.unsetf(1l << i);
#else
        unsetf_val = s.flags();
        s.unsetf(1l << i);
#endif 
        if (unsetf_val != old_format)
            {local_flag = FAILED;
            DbgPrint("\tunsetf() returned %lx instead of %lx\n",
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

        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
