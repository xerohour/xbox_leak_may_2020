/* @(#)File: ios4.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file test the following member functions of the class    >
    < ios: bitalloc(), xalloc(), iword(int), pword(int), rdbuf().   >

>HOW:   < The function bitalloc() returns a long with a single      >
    < previously unallocated bit set in the format variable.    >

    < 0. Ask for as many bits as possible and check that it does    >
    <    not return more than the total number of bits in a long    >
    <    variable (generally 32).  Make the requests using the  >
    <    syntax ios::bitalloc().                    >

    < 1. The function bitalloc() is a static member function.   >
    <    Check that when no more bits are available following the   >
    <    requests made in block 0, requests made from any object    >
    <    of class ios cannot be honored either.         >

    < 2. The function xalloc() returns a previously unassigned  >
    <    index into an array of words for use as a format variable. >
    <    Check that 2 consecutive calls to xalloc() do not return   >
    <    the same value.                        >

    < 3. Create a derived class of ios called ios2.  Check that >
    <    requesting a new format word usingg ios2::xalloc() does    >
    <    not return one of the values returned by ios::xalloc() >
    <    in block 2.                        >

    < 4. iword(int i) and pword(int i) both return a reference to   >
    <    the user defined word whose index is i.  The return type   >
    <    is the only difference.  Check that a type cast of the     >
    <    return value of iword(i) yields the same value as pword(i).>

    < 5. Check that rdbuf() returns a pointer to the streambuf  >
    <    associated with the ios s when s was constructed.      >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ios4()";         

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

/*--------------------------------------------------------------------*/
ios4_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test function bitalloc()");

    int long_size = 8 * sizeof (long);

    for (int i= 0; i<long_size; i++)
        ios::bitalloc();

    if (ios::bitalloc())
        fail ("unexpected unallocated bit in format variable");

#endif 

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test function bitalloc()");

    if (s.bitalloc())
        fail ("unexpected unallocated bit in format variable");

#endif 

    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check function xalloc() does not return twice the same value");

    int i1, i2;

    i1 = ios::xalloc();
    i2 = ios::xalloc();

    if (i1 == i2)
        fail ("reallocation of the same format word");

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check function xalloc() does not return twice the same value");
    print("even if you call it from a different derived class of ios");

    class ios2 : public ios {};

    int i3 = ios2::xalloc();

    if ((i3 == i1) || (i3 == i2))
        fail ("reallocation of the same format word");

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that iword(i) and pword(i) return the same value");

    if (s.pword(i1) != ((void *) s.pword(i1)))
        fail ("iword() and pword() return different values");

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that s.rdbuf() returns the streambuf coupled with s at");
    print("the time of construction of s");

    if (s.rdbuf() != sb)
        fail("unexpected streambuf pointer");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
        return 0;
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
