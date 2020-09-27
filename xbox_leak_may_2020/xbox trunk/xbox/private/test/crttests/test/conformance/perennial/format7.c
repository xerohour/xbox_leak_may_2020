/* @(#)File: format7.C      Version: 1.0        Date:11/08/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the effects of some input related bits in >
    < the format control word.  These effects are observed on   >
    < values extracted from a strstreambuf.             >

    < The bit tested here is skipws.                >

>HOW:   < 0. Set skipws.  Place an integer by whitespace in the buffer. >
    <    Check the whitespace is skipped by extracting the integer. >

    < 1. Unset skipws and extract whitespace.                       >

    < 2. Unset skipws and try to extract a number.                  >

>MODS:  < 11/8/90, DRE: Added 2.1 bugs comment and fixed HOW comments.  >
    < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < None known for cfront 2.0.                                    >
    < Cfront 2.1 fails block 1.                                     >
========================================================================*/
#include "testhead.h"     

static char progname[] = "format7()";         

inline void print(char str[]) { DbgPrint( "%s\n", str); }
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
static int i;
static char c;
/*--------------------------------------------------------------------*/
void format7_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test skipws flag causes whitespace to be skipped");

    {static char buf[10] = "  \t a";
    istrstream s(buf, 8);   // create a stream
    s.setf(ios::skipws);

    s >> c;
    if (c != 'a')
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test skipws unset causes whitespace to be extracted");

    {static char buf[10] = " \ta";
    istrstream s(buf, 8);   // create a stream
    s.unsetf(ios::skipws);

    s >> c;
    if (c != ' ') {
        fail ("unexpected result");
        DbgPrint( "Got `%c'(%d), expected `%c'(%d)\n", 
            c, c, ' ', ' ');
    }
    }

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test that when skipws is not set, if a whitespace is");
    print("encountered by a numeric extractor, an error results");

    {static char buf[10] = "  \t 10";
    istrstream s(buf, 8);   // create a stream
    s.unsetf(ios::skipws);

    s >> i;
    if (s.good())
        fail ("no error signalled");
    }

        blexit();
/*--------------------------------------------------------------------*/
    anyfail();
}

#include "peren_another.c"
