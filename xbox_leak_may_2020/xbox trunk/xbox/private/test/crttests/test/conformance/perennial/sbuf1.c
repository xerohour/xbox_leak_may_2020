/* @(#)File: sbuf1.C        Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)       >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < Testing the "get" related functions of class strstreambuf.    >
    < This class is derived from the base class streambuf in which  >
    < the tested functions are originally defined.  The tested  >
    < functions are not redefined in strstreambuf, so this file >
    < is actually testing functions of streambuf.           >

>HOW:   < Declare a strtreambuf object and initialize the buffer with   >
    < a given test pattern.  Each block is used to test the     >
    < behaviour of a "get" related function.            >
    
    < 0. Test in_avail()                        >
    < 1. Test sgetc()                       >
    < 2. Test sbumpc()                      >
    < 3. Test snextc()                      >
    < 4. Test stossc()                      >
    < 5. Test sgetn() on a partial fetch (leave some chars in buf)  >
    < 6. Test sgetn() on a complete fetch (no chars left)       >
    
>MODS:  < Version 1.0 created by HK.                    >

>BUGS:  < None known.                           >
========================================================================*/
#include "testhead.h"     

static char progname[] = "sbuf1()";         

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

// global variable declarations
static char msg[] = "Hi there!";   // test pattern
static char * bmsg = msg;          // marks base of pattern
static char * emsg = msg - 1 + strlen(msg);    // marks end of pattern

static char buf[20]="Hi there!";   // array used as stream's buffer
                // initialized with test pattern
static char tmp[20];   // array used to get strings into
static char *cp = msg;

static strstreambuf in(buf, (int) strlen(buf), 0);   // create strstreambuf
/*LX:10131999 added cast to (int) for 64-bit friendliness. Should be safe. */
/*--------------------------------------------------------------------*/
void sbuf1_main()                                  /***** BEGINNING OF MAIN. *****/
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing in_avail()");
    if ((size_t)(in.in_avail()) != strlen(buf))
        fail("unexpected return value");

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sgetc()");
    if (in.sgetc() != *bmsg)
        fail ("unexpected char");

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sbumpc()");
    if (in.sbumpc() != *bmsg)
        fail ("unexpected char");

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing snextc()");
    if (in.snextc() != *(bmsg+2))
        fail ("unexpected char");

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing stossc()");
    in.stossc();
    if (in.sgetc() != *(bmsg+3))
        fail ("unexpected char");

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sgetn()");
    strcpy(tmp, "");
    in.sgetn(tmp, 3);
    if (strncmp( msg+3, tmp, 3))
        fail("unexpected string");

    blexit();
/*--------------------------------------------------------------------*/
    block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sgetn() when requesting more char's than are available");
    in.sgetn(tmp+3, (streamsize) strlen(msg));
/*LX:10131999 added cast to (int) for 64-bit friendliness */
    if (strcmp(msg+3, tmp))
        fail("unexpected string");

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
