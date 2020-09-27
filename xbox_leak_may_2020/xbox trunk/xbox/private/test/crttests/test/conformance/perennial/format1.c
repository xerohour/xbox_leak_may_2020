/* @(#)File: format1.C      Version: 1.0        Date:11/08/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file tests the effects of some output related bits in    >
    < the format control word.  These effects are observed on   >
    < values inserted into a strstreambuf.              >

    < The bits tested here are : left, right and internal.      >
    < These flags control the padding of a value.  The width of >
    < the padding is set by the function width(), and the fill  >
    < character is controlled by the function fill().       >

>HOW:   < Set the width to 4, and the fill character to '!' .       >
    < Write some integer to the stream.  Check that the output  >
    < is properly padded in the the output buffer.          >

    < 0. When left is set, the fill character is added after the    >
    <    value.                         >

    < 1. When right is set, the fill character is added before the  >
    <    value.                         >

    < 2. When internal is set, the fill character is added after    >
    <    the leading sign or base but before the value.  Check  >
    <    this rule with a sign.                 >
    
    < 3. When internal is set, the fill character is added after    >
    <    the leading sign or base but before the value.  Check  >
    <    this rule with a base.                 >

    < 4. Right adjustement is the default if all three flags are    >
    <    cleared.                           >

>MODS:  < 11/08/90, DRE: Added bugs comment for cfront 2.1.             >
    < 07/02/90, DRE: Include "testhead.h" not "../../testhead.h".   >
    < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < No cfront 2.0 bugs are known.    
    < Cfront 2.1 fails block 2. It ends up with
    < ``-!!1'' in the buffer rather than ``-!!2''. 
========================================================================*/

#include "testhead.h"     

static char progname[] = "format1()";         

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
using namespace std;
#endif

// global variable declarations

/*--------------------------------------------------------------------*/
format1_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test left padding.");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.fill('!');        // set fill character to '!'
    s.width(4);     // set width to 4
    s.setf(ios::left, ios::adjustfield);    // set flag

    s << 0;         // write to stream
    if (strncmp(buf, "0!!!", 4))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test right padding.");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.fill('!');        // set fill character to '!'
    s.width(4);     // set width to 4
    s.setf(ios::right, ios::adjustfield);   // set flag

    s << 1;         // write to stream
    if (strncmp(buf, "!!!1", 4))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test internal padding with a signed integer");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.fill('!');        // set fill character to '!'
    s.width(4);     // set width to 4
    s.setf(ios::internal, ios::adjustfield);    // set flag

    s << - 2;           // write to stream
    if (strncmp(buf, "-!!2", 4))
        fail ("unexpected result");
        DbgPrint( "got `%s', expected `-!!2'.\n", buf);
    }

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Test internal padding with a hex integer");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.fill('!');        // set fill character to '!'
    s.width(5);     // set width to 4
    s.setf(ios::internal, ios::adjustfield);    // set flag
    s.setf(ios::hex, ios::basefield);       // set hex flag
    s.setf(ios::showbase);              // make base show

    s << 10;            // write to stream
    if (strncmp(buf, "0x!!a", 5))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Verify default is right padding when no flag is set");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.fill('!');        // set fill character to '!'
    s.width(4);     // set width to 4
    s.unsetf(ios::adjustfield); // unset all flags

    s << 4;         // write to stream
    if (strncmp(buf, "!!!4", 4))
        fail ("unexpected result");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();      
    return 0;
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
