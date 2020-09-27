/* @(#)File: ostream4.C    Version: 1.0    Date: 11/08/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class.
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the formatted output operations.
    < 
    < 0. operator<<(int) 
    <
    < 1. operator<<(int) with a negative value
    <
    < 2. operator<<(unsigned int) 
    <
    < 3. operator<<(long) 
    <
    < 4. operator<<(long) with a negative value
    <
    < 5. operator<<(unsigned long) 
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < All these tests write a known value into the buffer,
    <       then read it back and verify it.

>MODS:  < 11/8/90, DRE: Added 2.1 BUGS comment.
    < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/5/90 Created: DRE

>BUGS:  < No bugs or system dependencies known as far as cfront 2.0
    < Cfront 2.1 fails block 1 and 4 of the test.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <strstrea.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "ostream4()";

static char * ptr;
const int len = 512;
static char buf[len];

/*--------------------------------------------------------------------*/
void ostream4_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(int).\n");

    {
        strstreambuf sb;
        int i = 1234;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << i << '\0'))
            failure("Could not insert the int.");
        else if (sb.sgetn(buf,4) != 4)
            failure("Could not read back int plus '\\0'.");
        else if (memcmp("1234",buf,4))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(int) with a negative.\n");

    {
        strstreambuf sb;
        int n, s = -5678;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the int.");
        else if ((n = sb.sgetn(buf,5)) != 5)
        {
            failure("Could not read back int plus '\\0'.");
            DbgPrint( "Wanted %d chars, got %d\n", 5, n);
        }
        else if (memcmp("-5678",buf,5))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(unsigned int).\n");

    {
        strstreambuf sb;
        unsigned int s = 4321;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the int.");
        else if (sb.sgetn(buf,4) != 4)
            failure("Could not read back int plus '\\0'.");
        else if (memcmp("4321",buf,4))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(long).\n");

    {
        strstreambuf sb;
        long s = 2345;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the long.");
        else if (sb.sgetn(buf,4) != 4)
            failure("Could not read back long plus '\\0'.");
        else if (memcmp("2345",buf,4))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(long) with a negative.\n");

    {
        strstreambuf sb;
        long s = -4567;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the long.");
        else if (sb.sgetn(buf,5) != 5)
            failure("Could not read back long plus '\\0'.");
        else if (memcmp("-4567",buf,5))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(unsigned long).\n");

    {
        strstreambuf sb;
        unsigned long s = 7654;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the long.");
        else if (sb.sgetn(buf,4) != 4)
            failure("Could not read back long plus '\\0'.");
        else if (memcmp("7654",buf,4))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
