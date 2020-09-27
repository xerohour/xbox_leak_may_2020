/* @(#)File: istreamB.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream class:
    < ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the unformatted input operations.
    < 
    < 0. ins.get(ptr,len,delim) sets failbit if it hits EOF 
    <    before extracting any characters.
    <
    < 1. Assignment of a streambuf* to an istream_withassign
    <    associates the buffer with the stream and initializes
    <    the entire buffer's state.
    <
    < 2. Assignment of an istream* to an istream_withassign 
    <    associates the formers streambuf with the latter,
    <    and reinitializes the latter's state.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0. Extract from an empty buffer with get and look for failbit.
    <
    < 1. Assign a streambuf* to an istream_withassign and check 
    <    the state.
    <
    < 2. Perform the assignment and check the state.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 5/12/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istreamB()";

/*--------------------------------------------------------------------*/
void istreamb_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>> returns the stream.\n");

    {
        strstreambuf sb;
        char ptr[10];
        istream is(&sb);
        is.get(ptr,10,'\n');
        if ((is.rdstate() & ios::failbit) == 0)
            failure("Failbit not set.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          
#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Assigning a streambuf* to an istream_withassign.\n");

    {
        char ptr[10];
        strstreambuf sb;
        strstreambuf sb2;
        istream_withassign is;
        long oldflags;

        is = &sb;
        if (is.rdbuf() != &sb)
            failure("strstreambuf not associated with istream.");
        oldflags = is.flags();
        // Set the error flag
        is.get(ptr,10,'\n');
        // Verify the error state
        if ((is.rdstate() & ios::failbit) == 0)
            failure("Failbit not set.");
        // Change some flags
        is.flags(oldflags | ios::right | ios::showbase | ios::unitbuf | ios::showpos);
        is = &sb2;
        // Error state should be cleared.
        if (is.rdstate() & ios::failbit)
            failure("Failbit not reset.");
        // Flags should be reset
        if (is.flags() != oldflags)
            failure("Flags not reset.");
    }
#endif 
    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          
#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Assigning an istream* to an istream_withassign.\n");

    {
        char ptr[10];
        strstreambuf sb;
        strstreambuf sb2;
        istream src(&sb);
        istream src2(&sb2);
        istream_withassign is;
        long oldflags;

        is = src;
        if (is.rdbuf() != src.rdbuf())
            failure("correct streambuf not associated with istream.");
        oldflags = is.flags();
        // Set the error flag
        is.get(ptr,10,'\n');
        // Verify the error state
        if ((is.rdstate() & ios::failbit) == 0)
            failure("Failbit not set.");
        // Change some flags
        is.flags(oldflags | ios::right | ios::showbase | ios::unitbuf | ios::showpos);
        is = src2;
        if (is.rdbuf() != src2.rdbuf())
            failure("second correct streambuf not associated with istream.");
        // Error state should be cleared.
        if (is.rdstate() & ios::failbit)
            failure("Failbit not reset.");
        // Flags should be reset
        if (is.flags() != oldflags)
            failure("Flags not reset.");
    }
#endif 
    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
