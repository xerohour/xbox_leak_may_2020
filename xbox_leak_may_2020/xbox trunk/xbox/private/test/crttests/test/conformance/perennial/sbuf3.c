/* @(#)File: sbuf3.C        Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)       >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < Testing the "put" related functions of class strstreambuf.    >
    < This class is derived from the base class streambuf in which  >
    < the tested functions are originally defined.  The tested  >
    < functions are not redefined in strstreambuf, so this file >
    < is actually testing functions of streambuf.           >

>HOW:   < Declare a strtreambuf object and initialize the buffer with   >
    < a given test pattern.  Each block is used to test the     >
    < behaviour of a "put" related function.            >
    
    < 0. Test out_waiting() on empty buffer             >
    < 1. Test sputc()                       >
    < 2. Test sputn()                       >
    < 3. Test out_waiting() on full buffer              >
    < 4. Test sputc() at the end of buffer (no space left)      >
    < 5. Test sputn() at the end of buffer (no space left)      >
    
>MODS:  < Version 1.0 created by HK.                    >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "sbuf3()";         

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

static char msg[] = "Hi there!";    // test pattern
static char * bmsg = msg;          // marks base of pattern
static char * emsg = msg - 1 + strlen(msg);    // marks end of pattern

static char buf[10];
static strstreambuf out(buf, (int) strlen(msg), buf);
/*LX:10131999 added cast to (int) for 64-bit friendliness */

/*--------------------------------------------------------------------*/
void sbuf3_main()                                  /***** BEGINNING OF MAIN. *****/
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();                      

#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing out_waiting() on empty buffer");
    if (out.out_waiting() != 0)
        fail("unexpected return value");
#endif
    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sputc()");
    if (out.sputc(*msg) == EOF)
        fail ("unexpected error");
    if (*buf != *msg)
        fail ("unexpected char");

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sputn()");
    if (out.sputn(msg+1, (streamsize) strlen(msg) -1) == EOF)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
        fail ("unexpected error");
    if (strcmp(buf, msg))
        fail ("unexpected string");

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();                      

#if defined(USING_OLD_IOS)

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing out_waiting()");
    if (out.out_waiting() != strlen(msg))
        fail("unexpected return value");
#endif
    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sputc() at end of buffer");
    if (out.sputc( '$') != EOF)
        fail ("unexpected success of sputc()");

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sputn() at end of buffer");
    if (out.sputn( "$$$", 2) != 0)
        fail ("unexpected success of sputn()");

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();      
}                                       /******** END OF MAIN. ********/



#include "peren_another.c"
