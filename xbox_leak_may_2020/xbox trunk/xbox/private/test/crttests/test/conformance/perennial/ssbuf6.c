/* @(#)File: ssbuf6.C       Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, SSBUF(3C++)      >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < The member function str() of class strstreambuf       >

    < This function returns a pointer to the first char of the  >
    < current buffer.                       >

>HOW:   < Create a strstreambuf in static mode.  The buffer is a char   >
    < array passed to the constructor at the time of creation.  >

    < 0. Call str() and check it returns a pointer to the buffer.   >

>MODS:  < Version 1.0 by HK.                        >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ssbuf6()";         

inline void print(char str[]) { DbgPrint( "%s\n", str);}
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
#define NULL 0

// global variable declarations

static char buf[15] = "Hi there!";
static strstreambuf ssb(buf, 8, NULL);

/*--------------------------------------------------------------------*/
void ssbuf6_main()                                 /***** BEGINNING OF MAIN. *****/
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();                      

    print("AT&T Library Manual, Appendix A, SSBUF(3C++)");
    print("Check that str() returns a pointer to the first character of");
    print("the array the strstreambuf currently uses.");

    if (ssb.str() != buf)
        fail ("unexpected return value");

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();
}

#include "peren_another.c"
