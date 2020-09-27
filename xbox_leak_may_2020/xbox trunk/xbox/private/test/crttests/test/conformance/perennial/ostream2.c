/* @(#)File: ostream2.C    Version: 1.0    Date: 01/02/91             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class,
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the seek/tell operations of the ostream classes.
    < 
    < These tests use the seekp positioning member to adjust
    < the put pointer of the streambuf. 
    <
    < 0. Positioning at the beginning of the buffer and writing again
    <    after writing a few bytes. 
    < 
    < 1. Positioning at the end of the buffer and appending data.

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0. Write into the buffer, seek back to the beginning and write
    <    again. Verify that the data was overwritten.
    <
    < 1. Put data into the buffer independently of the ostream.
    <    Then seekp the ostream to the end of the buffer and write
    <    data. Verify it was appended.

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    < Vers 1.0, date: 3/5/90 Created: DRE

>BUGS:  < The library fails block 1. The seek to the end of the
    < strstreambuf does not succeed.
    <
    < These tests assume that no actual physical file be
    < associated with the ostream.  May be incorrect. - RFG
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "ostream2()"; 

const int len = 512;
static char buf[len];

static char * ptr;
static char c;

/*--------------------------------------------------------------------*/
void ostream2_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: seekp().\n");
    DbgPrint("Using seekp() to rewrite data.\n");

    {
        const char * s1 = "This is string 1";
        const char * s2 = "12345667889980923 This is string 2.\n";
        strstreambuf sb;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s1))
            failure("Couldn't write data first time.");
        else if (!os.seekp(0,ios::beg))
            failure("Couldn't seek to beginning of ostream.");
        else if (!(os << s2))
            failure("Couldn't write data second time.");
        // The string obtained by sgetn is not NULL terminated.
        else if (((size_t)sb.sgetn(buf,len)) != strlen(s2))
            failure("Read wrong amount of data from buffer.");
        else if (memcmp(buf,s2,strlen(s2)))
            failure("Read wrong string from buffer.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: seekp().\n");
    DbgPrint("Using seekp() to append data.\n");

    {
        const char * s1 = "This is string 1";
        const char * s2 = "12345667889980923 This is string 2.\n";
        strstreambuf sb(len);
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
#if defined(USING_OLD_IOS)
        else if (sb.sputn(s1,strlen(s1) != strlen(s1)))
#else
        else if (((size_t)sb.sputn(s1,(int) strlen(s1))) != strlen(s1))
	/* LX:10131999 added cast to (int) for 64-bit friendliness */
#endif 
            failure("Couldn't write first string.");
        else if (!os.seekp(0,ios::end))
            failure("Couldn't seek to end of ostream.");
        else if (!(os << s2))
            failure("Couldn't write second string.");
        // The string obtained by sgetn is not NULL terminated.
        else if (((size_t)sb.sgetn(buf,len)) != strlen(s1) + strlen(s2))
            failure("Read wrong amount of data from buffer.");
        else if (memcmp(buf,s1,strlen(s1)) || memcmp(buf+strlen(s1),s2,strlen(s2)))
            failure("Read wrong data from buffer.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
