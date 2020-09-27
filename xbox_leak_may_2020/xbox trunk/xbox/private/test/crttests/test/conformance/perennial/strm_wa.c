/* @(#)File: strm_wa.C    Version: 1.0    Date: 09/27/90              */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream class.

>WHAT:  < Testing the *stream_withassign class.
    <
    < 0. Assign one istream to another and check that it works.
    <
    < 0. Assign one ostream to another and check that it works.
    <
    < 0. Assign one iostream to another and check that it works.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0. Write a string with leading whitespace to a buffer.
    <    Create an istream on the strbuf. Create another istream.
    <    Perform an assignment, extract and check the data.
    <
    < 1. Construct an ostream_withassign on a strstreambuf.
    <    Assign the ostream to another. Through the second ostream
    <    write into the buffer. Then check that the data can
    <    be read.
    <
    < 2. Combine blocks 0 and 1, do both the reading and writing
    <    through the iostream.

>MODS:  < 07/02/90, DRE: Changed definition of failure() to a prototype.
    <    Included "testhead.h" instead of "testhead.h"
    < Vers 1.0, date: 3/6/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "strm_wa()";

/*--------------------------------------------------------------------*/
void strm_wa_main()                  
{
    setup();
#if defined(USING_OLD_IOS)

/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: istream_withassign.\n");

    {
        char * ptr = "abc";
        char buf[128];
        strstreambuf sb;
        sb.sputn(ptr,strlen(ptr));
        istream is(&sb);
        istream_withassign is2;
        is2 = is;
        if (!is2)
            failure("Assignment failed.");
        else
        {
            is2.width(128);
            is2 >> buf;
            if (strcmp(buf,ptr))
                failure("Got wrong string.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: ostream_withassign.\n");

    {
        char * ptr = "def";
        char buf[128];
        strstreambuf sb;
        ostream os(&sb);
        ostream_withassign os2;
        os2 = os;
        if (!(os2 << ptr << '\0'))
            failure("Could not insert into assigned ostream.");
        else if (sb.sgetn(buf,strlen(ptr)+1) != strlen(ptr)+1)
            failure("Could not read string out of buffer");
        else if (strcmp(ptr,buf))
            failure("Read wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS.INTRO(3C++) Manual Page: iostream_withassign.\n");
    DbgPrint("Read test.\n");

    {
        char * ptr = "ghi";
        char buf[128];
        strstreambuf sb;
        sb.sputn(ptr,strlen(ptr));
        iostream ios(&sb);
        iostream_withassign ios2;
        ios2 = ios;
        if (!ios2)
            failure("Assignment failed.");
        else
        {
            ios2.width(128);
            ios2 >> buf;
            if (strcmp(buf,ptr))
                failure("Got wrong string.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS.INTRO(3C++) Manual Page: iostream_withassign.\n");
    DbgPrint("Write test.\n");

    {
        char * ptr = "jkl";
        char buf[128];
        strstreambuf sb;
        iostream ios(&sb);
        iostream_withassign ios2;
        ios2 = ios;
        if (!(ios2 << ptr << '\0'))
            failure("Could not insert into assigned ostream.");
        else if (sb.sgetn(buf,strlen(ptr)+1) != strlen(ptr)+1)
            failure("Could not read string out of buffer");
        else if (strcmp(ptr,buf))
            failure("Read wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
#endif 
    anyfail();  
}                   


#include "peren_another.c"
