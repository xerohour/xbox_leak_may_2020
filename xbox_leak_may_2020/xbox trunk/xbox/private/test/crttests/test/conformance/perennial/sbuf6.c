/* @(#)File: sbuf6.C        Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)      >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < Testing the default definitions of of the following virtual   >
    < functions of class streambuf: pbackfail, seekoff, seekpos,    >
    < setbuf, sync.                         >

>HOW:   < Constructs an empty buffer with streambuf().          > 
    < Check the default definitions of each function.       >
    
    < 0. pbackfail(c) returns EOF.                  >

    < 1. seekoff(off, dir, mode) returns EOF.           >

    < 2. seekpos(pos, mode) returns seekoff(streamoff(pos),     >
    <    ios::beg, mode).  Since seekoff returns EOF, seekpos   >
    <    should also return EOF.                    >

    < 3. setbuf(ptr, len) is used to set up a reserve area and  >
    <    always honors the request when there is no reserve area.   >
    <    the return value is then the address of the streambuf. >

    < 4. sync() returns 0 if the get area is empty and there are    >
    <    no unconsumed characters.  Otherwise it returns EOF.   >
    
>MODS:  < Version 1.0 created by HK.                    >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "sbuf6()";         

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

//char buf[10]="hi!";

#if defined(USING_OLD_IOS)
static streambuf sb;//(buf, 6);
#else
static char buf[10]="hi!";
static strstreambuf sb(buf, 6, buf);
#endif
/*--------------------------------------------------------------------*/
void sbuf6_main()                                 /***** BEGINNING OF MAIN. *****/
{
    setup();
#if defined(USING_OLD_IOS)

/*--------------------------------------------------------------------*/
    block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The default definition of pbackfail(c) returns EOF");

    if (sb.pbackfail(0) != EOF)
        fail("unexpected return value");
    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The default definition of seekoff(off, dir, mode) returns EOF");

    if (sb.seekoff(0, ios::beg, 0) != EOF)
        fail("unexpected return value");

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The default definition of seekpos(pos, mode) returns EOF");

    if (sb.seekpos(0, 0) != EOF)
        fail("unexpected return value");

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The buffer created by the constructor streambuf() has no");
    print("reserve area.  Use setbuf(char *, int) to set up a reserve");
    print("area.  The default definition of setbuf honors the request");
    print("when there is no previous reserve area.");

    char buf3[10];

    if (sb.setbuf(buf3, 10) != &sb)
        fail("unexpected setbuf failure");

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PROT(3C++)");
    print("The default definition of sync() returns 0 when the get");
    print("area is empty and there are no unconsumed characters.");

    int s = sb.sync();

    if ((s != EOF) && (s != 0))
        fail("unexpected return value");

    /*
    if (sb.in_avail()+sb.out_waiting())
        {if (sb.sync() != EOF)
            fail("sync() doesn't return expected value EOF");
        }
    else
        {if (sb.sync() != 0)
            fail("sync() doesn't return expected value 0");
        }
    */

    blexit();
/*--------------------------------------------------------------------*/
#endif
    anyfail();      
}                                       /******** END OF MAIN. ********/



#include "peren_another.c"
