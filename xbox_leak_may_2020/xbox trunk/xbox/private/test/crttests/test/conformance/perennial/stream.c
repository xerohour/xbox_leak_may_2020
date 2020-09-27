/* @(#)File: stream.C    Version: 1.0    Date: 09/27/90               */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream and ostream
    < classes. IOS.INTRO(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the types of the standard streams, cin, cout
    < cerr and clog are of ?stream_withassign types.
    < 
    < 0. cin is an istream_withassign
    <
    < 1. cout is an ostream_withassign
    <
    < 2. cerr is an ostream_withassign
    <
    < 3. clog is an ostream_withassign

>HOW:   < 0-3. Assign the standard streams to other variables
    <      of the same type.

>MODS:  < 07/02/90, DRE: Changed definition of failure() to a prototype.
    <    Included "testhead.h" instead of "testhead.h"
    < Vers 1.0, date: 3/6/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    <
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#endif

#include "testhead.h"


static char progname[] = "stream()";   

/*--------------------------------------------------------------------*/
void stream_main()                  
{
    setup();
#if defined(USING_OLD_IOS)

/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS.INTRO(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("cin is of type istream_withassign.\n");

    {
        // This is a compile test only
        istream_withassign i;
        i = cin;
        // If it compiles it passes
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS.INTRO(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("cout is of type ostream_withassign.\n");

    {
        // This is a compile test only
        ostream_withassign o;
        o = cout;
        // If it compiles it passes
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS.INTRO(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("cerr is of type ostream_withassign.\n");

    {
        // This is a compile test only
        ostream_withassign o2;
        o2 = cerr;
        // If it compiles it passes
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS.INTRO(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("clog is of type ostream_withassign.\n");

    {
        // This is a compile test only
        ostream_withassign o3;
        o3 = clog;
        // If it compiles it passes
    }

    blexit();
/*--------------------------------------------------------------------*/
#endif
    anyfail();  
}                   

#include "peren_another.c"
