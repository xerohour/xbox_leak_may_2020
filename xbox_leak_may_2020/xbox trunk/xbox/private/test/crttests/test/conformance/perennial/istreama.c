/* @(#)File: istreamA.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream class:
    < ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the formatted input operations.
    < 
    < 0. ins >> x always returns ins.
    <
    < 1. ins >> x sets badbit if it can't extract any characters.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < All these tests put a known string in the buffer,
    <       then extract it an verify its value.
    <
    < 0. Extract a char. Check that the return value equals the stream.
    <
    < 1. Extract more characters than are available and look for badbit.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 5/12/90 Created: DRE

>BUGS:  < Block 1 is failed by cfront 2.0 and its libraries.
    < The ISTREAM(3C++) manual page dated June 14, 1989 says
    < with regards to `cin >> x': ``ios::badbit indicates that
    < attempts to extract characters failed.'' 
    < When the cause of the failure is EOF then that does not happen.
    < Instead it sets eofbit and failbit. This seems like appropriate
    < behavior, however it diverges from the written specification,
    < therefore users should be notified. For that reason this test
    < is included even though it is the manual page that is wrong,
    < not the software.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istreamA()";

// Write a null-terminated string into a strstreambuf
// Don't write the NULL.
static int write(strstreambuf & sb, const char * s)
{
    while (*s)
        if (sb.sputc(*s++) == EOF)
            return EOF;
    return 0;
}

static char * ptr;

/*--------------------------------------------------------------------*/
void istreama_main()                 
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
        char c;
        ptr = "01234 654 321";
        write(sb,ptr);
        istream is(&sb);
        if ((is >> c) != is)
            failure("Extraction expression has wrong value.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>> sets ios::badbit if extraction\n");
    DbgPrint("fails.\n");

    {
        strstreambuf sb;
        char c;
        ptr = "0";
        write(sb,ptr);
        istream is(&sb);
        is >> c;
        is >> c;
#if defined(USING_OLD_IOS)
        if ((is.rdstate() & ios::badbit) == 0)
        {
            failure("ios::badbit not set.");
            DbgPrint( "State is 0x%x\n", is.rdstate());
        }
#else 
        if ((is.rdstate() & ios::failbit) == 0)
        {
            failure("ios::failbit not set.");
            DbgPrint( "State is 0x%x\n", is.rdstate());
        }
#endif 
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
