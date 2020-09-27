/* @(#)File: sbuf5.C        Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)      >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < Testing the streambuf(char *, int) constructor        >

>HOW:   < streambuf(char * b, int len) constructs an empty buffer   >
    < and then sets up the reserve area to be the len bytes     >
    < starting at b.                        >
    
    < 0. Check the get area is empty using in_avail()       >
    < 1. Check the put area is empty using out_waiting()        >
    < 2. Get and put a string to check the buffer is functional >
    
>MODS:  < Version 1.0 created by HK.                    >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "sbuf5()";         

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

static char buf[10]="hi!";

#if defined(USING_OLD_IOS)
static streambuf sb(buf, 6);
#else
static strstreambuf sb(buf, 6, buf);
#endif 
/*--------------------------------------------------------------------*/
void sbuf5_main()                                  /***** BEGINNING OF MAIN. *****/
{
        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The buffer created by the constructor streambuf(char *, int)");
    print("has an empty get area.");

    if (sb.in_avail() != 0)
        fail("get area is not empty");

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      
#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The buffer created by the constructor streambuf(char *, int)");
    print("has an empty put area.");

    if (sb.out_waiting() != 0)
        fail("put area is not empty");
#endif 
        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The buffer created by the constructor streambuf(char *, int)");
    print("is functional. Try to put and get data.");

    static char in[5] = "test";

    if ((sb.sputn(in, 4) != 4) || (strncmp(buf, in, 4)))
        fail ("unexpected put error");

    static char out[5] = "";

    if ((sb.sgetn(out, 4) != 4) || (strncmp(out, in, 4)))
        fail ("unexpected get error");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
}                                       /******** END OF MAIN. ********/



#include "peren_another.c"
