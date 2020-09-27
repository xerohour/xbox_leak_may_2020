/* @(#)File: ssbuf1.C       Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, SSBUF(3C++)      >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < Testing the strstreambuf() constructor            >

>HOW:   < strstreambuf() constructs an empty buffer in dynamic mode.    >
    
    < 0. Check the get area is empty using in_avail()       >
    < 1. Check the put area is empty using out_waiting()        >
    < 2. Get and put a string to check the buffer is functional >
    
>MODS:  < Version 1.0 created by HK.                    >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ssbuf1()";         

inline void print(char str[]) { DbgPrint( "%s\n", str);}
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

static strstreambuf sb;
/*--------------------------------------------------------------------*/
void ssbuf1_main()                                 /***** BEGINNING OF MAIN. *****/
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SSBUF(3C++)");
    print("Check that a buffer created by constructor strstreambuf()");
    print("has an empty get area.");

    if (sb.in_avail() != 0)
        fail("get area is not empty");

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();                      
#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, SSBUF(3C++)");
    print("Check that a buffer created by constructor strstreambuf()");
    print("has an empty put area.");

    if (sb.out_waiting() != 0)
        fail("put area is not empty");
#endif 
    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SSBUF(3C++)");
    print("Check that a buffer created by constructor strstreambuf()");
    print("is functional.");

    static char in[5] = "test";

    if (sb.sputn(in, 4) != 4)
        fail ("unexpected put error");

    static char out[5] = "";

    if ((sb.sgetn(out, 4) != 4) || (strncmp(out, in, 4)))
        fail ("unexpected get error");

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();      
}                                       /******** END OF MAIN. ********/



#include "peren_another.c"
