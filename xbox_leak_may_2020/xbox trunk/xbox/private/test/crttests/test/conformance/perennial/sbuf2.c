/* @(#)File: sbuf2.C        Version: 1.0        Date:09/27/90   */
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
    < Most tests in this file check the functions behavior at the   >
    < of the buffer (No characters left to fetch).          >

>HOW:   < Declare a strtreambuf object and initialize the buffer with   >
    < a given test pattern.  Do an sgetn() to fetch all available   >
    < chars out of the buffer.  Each block is used to test the  >
    < behaviour of a "get" related function.            >
    
    < 0. Test in_avail() at the end of buffer           >
    < 1. Test sgetc() at the end of buffer              >
    < 2. Test sbumpc() at the end of buffer             >
    < 3. Test snextc() at the end of buffer             >
    < 4. Test sputbackc()                       >
    < 5. Test sputbackc() at the beginning of the buffer (no chars  >
    <    left to put back)                      >
    < 6. Test snextc() when only one char is left to fetch.  The    >
    <    expected return value is EOF.              >
    < 7. Test stossc() at the end of buffer.  It should have no >
    <    effect.                            >
    
>MODS:  < Version 1.0 created by HK.                    >

>BUGS:  < ATT library bug:                      >
    <   stossc() behaves incorrectly at the end of the buffer.  >
    <   It continues moving the get pointer ahead even though   >
    <   the end of buffer has been reached.  This behaviour >
    <   is contrary to what is described in the ATT library >
    <   manual (stossc() should have no effect).        >
    <   As a result of that, the ATT compiler fails block 7.    >
========================================================================*/
#include "testhead.h"     

static char progname[] = "sbuf2()";         

inline void print(char str[]) { DbgPrint( "%s\n", str);}
static void fail (char str[]) {
    local_flag =FAILED;
    DbgPrint( "\tFAILURE: %s\n", str);
}

/*--------------------------------------------------------------------*/
// global variable declarations
#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <strstrea.h>
#else
#include <iostream>
#include <strstream>
#endif

#define debug DbgPrint
#define NULL 0

static char msg[] = "Hi there!";    // test pattern
static char * bmsg = msg;          // marks base of pattern
static char * emsg = msg - 1 + strlen(msg);    // marks end of pattern

static char buf[20]= "Hi there!";
static strstreambuf in(buf, (int)strlen(buf));
/*LX:10131999 added cast to (int) for 64-bit friendliness */

static char tmp[20];
int c;
static char *cp = msg;

/*--------------------------------------------------------------------*/
void sbuf2_main()                                 /***** BEGINNING OF MAIN. *****/
{
    setup();
    in.sgetn(tmp, (int) strlen(msg)+5);   // read all char's to end of buffer
/*LX:10131999 added cast to (int) for 64-bit friendliness */

/*--------------------------------------------------------------------*/
    block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing in_avail() at end of buffer");
    if (in.in_avail() != 0)
        fail ("unexpected value");

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sgetc() at end of buffer");
    if (in.sgetc() != EOF)
        fail ("unexpected success of sgetc()");

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sbumpc() at end of buffer");
    if (EOF != (c= in.sbumpc()))
        fail ("unexpected sbumpc() success");

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing snextc() at end of buffer");
    if (EOF != (c= in.sbumpc()))
        fail ("unexpected sbumpc() success");

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sputbackc()");
    for (cp = emsg ; cp >= bmsg ; cp--)
    {if (in.sputbackc(*cp) == EOF)
            fail("unexpected EOF");
    }
    if (strcmp(buf, msg))
        fail ("unexpected string");

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing sputbackc() at beginning of string");
    if (EOF != in.sputbackc(*cp))
        fail ("unexpected sputbackc() success");

    blexit();
/*--------------------------------------------------------------------*/
    block6: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing snextc() when only one char is left to fetch");
    in.sgetn(tmp, (streamsize)strlen(msg) -1);
/*LX:10131999 added cast to (streamsize) for 64-bit friendliness */
    if (EOF != in.snextc())
        fail("unexpected success of snextc()");

    blexit();
/*--------------------------------------------------------------------*/
    block7: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, SBUF.PUB(3C++)");
    print("testing stossc() at end of string");
    in.sbumpc();
    in.stossc();
    in.stossc();
// PROBLEM: got out of the buffer area !
    for (cp = emsg ; cp >= bmsg ; cp--)
    {if (in.sputbackc(*cp) == EOF)
            fail("unexpected EOF");
    }
    in.sputbackc('*');
    if ((in.sputbackc('$') != EOF) || ((size_t)(in.in_avail()) > strlen(msg)))
        fail("unexpected string");

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
