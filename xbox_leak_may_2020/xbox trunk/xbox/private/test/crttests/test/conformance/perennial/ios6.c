/* @(#)File: ios6.C     Version: 1.0        Date:09/27/90   */
/* CVS-CC, C++ Validation Suite                     */
/* Copyright 1989, Perennial, All Rights Reserved           */
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, IOS(3C++)        >
    < in AT&T C++ Language System 2.0, May 89.  Select Code 307-145 >

>WHAT:  < This file checks the default values of the format components  >
    < "fill", "width" and "precision".              >

>HOW:   < These values are checked using the member functions of the    >
    < class ios which return the value of a format component.       >

    < 0. Check the default value of the precision component is 6    >
    <    using the member function precision().         >

    < 1. Check the default value of the width component is 0    >
    <    using the member function width().             >

    < 2. Check the default value of the fill character is the space >
    <    using the member function fill().              >

    < Block 3 and 4 test that as described in the AT&T manual,  >
    < the width component is reset to 0 after each insertion or     >
    < extraction.                           >

    < 3. Set the width to a non-zero value.  Output a value.    >
    <    Check that the width is reset to 0 following the write.    >

    < 4. Set the width to a non-zero value.  Output a value.    >
    <    Check that the width is reset to 0 following the write.    >

    < 5. Check that the numeric inserters do not truncate values    >
    <    so they will fit in the selected field width.      >

>MODS:  < Version 1.0 created by HK on 3/4/90.              >

>BUGS:  < AT&T library bug:  Block 4 fails.  The width does not reset   >
    < to the default value (zero) following an insertion from an    >
    < input stream.                         >
========================================================================*/
#include "testhead.h"     

static char progname[] = "ios6()";         

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

/*--------------------------------------------------------------------*/
ios6_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that the default precision is 6");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream

    if (s.precision() != 6)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that the default width is 0");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream

    if (s.width() != 0)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check the default fill character is the space");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream

    if (s.fill() != ' ')
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that the width is reset to 0 following an insertion");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.width(4);     // set the width;

    s << 4;         // first write should be padded
    if (s.width() != 0)
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that the width is reset to 0 following an extraction");

    {static char buf[10] = "1";;
    istrstream s(buf, 8);   // create an iostream
    s.width(4);     // set the width;

    int i;

    s >> i;         // do an extraction
#if defined(USING_OLD_IOS)
    if (s.width() != 0)
        fail ("unexpected result");
#endif 
    }

        blexit();
/*--------------------------------------------------------------------*/
block5: blenter();                      

    print("REF: AT&T C++ Library Manual, Appendix A, IOS(3C++)");
    print("Check that the numeric inserters do not truncate values"); 
    print("so they will fit in the selected field width");

    {char buf[10];
    ostrstream s(buf, 8);   // create a stream
    s.width(1);     // set the width

    s << 16;        // write to stream a string longer than width
    if (strncmp(buf, "16", 2))
        fail ("unexpected result");
    }

        blexit();
/*--------------------------------------------------------------------*/
    anyfail();
    return 0;
}

#include "peren_another.c"
