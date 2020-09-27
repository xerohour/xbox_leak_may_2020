/* @(#)File: istream8.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream and ostream
    < classes. ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the istream manipulators.
    <
    < 0. ws extracts whitespace characters
    <
    < 1. dec sets the conversion base format flag to 10.
    <
    < 2. oct sets the conversion base format flag to 8.
    <
    < 3. hex sets the conversion base format flag to 16.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0. Write a string with leading whitespace to a buffer.
    <    Extract the whitespace with the manipulator. Then
    <    verify that the next character is correct.
    <
    < 1. Clear the basefield bits. Then set them to 10 with the
    <    dec manipulator and check them.
    <
    < 2. Set the base to 8 with the oct manipulator and check it.
    <
    < 3. Set the base to 16 with the hex manipulator and check it.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/4/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    <
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istream8()"; 

// Write a null-terminated string into a strstreambuf
// Don't write the NULL.
static int write(strstreambuf & sb, const char * s)
{
    while (*s)
        if (sb.sputc(*s++) == EOF)
            return EOF;
    return 0;
}

/*--------------------------------------------------------------------*/
void istream8_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: Manipulators.\n");
    DbgPrint("Testing the ws manipulator.\n");

    {
        char * ptr = " \t\nabc";
        strstreambuf sb;
        write(sb,ptr);
        istream is(&sb);
        if (!is)
            failure("Can not attach stream to buffer.");
        else
        {
            is >> ws;
            char c;
            is >> c;
            if (c == ' ' || c == '\t' || c == '\n')
                failure("Didn't extract whitespace.");
            else if (c != 'a')
                failure("Got wrong character.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: Manipulators.\n");
    DbgPrint("Testing the dec manipulator.\n");

    {
        strstreambuf sb;
        istream is(&sb);
        if (!is)
            failure("Can not attach stream to buffer.");
        else
        {
            is.setf(0,ios::basefield);
            is >> dec;
            if (! (is.setf(0,ios::basefield) & ios::dec))
            {
            // A failure here means one of two things:
            //   1) the bit was not set, or,
            //   2) setf() returned the wrong value.
                failure("ios::dec apparently not set.");
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: Manipulators.\n");
    DbgPrint("Testing the oct manipulator.\n");

    {
        strstreambuf sb;
        istream is(&sb);
        if (!is)
            failure("Can not attach stream to buffer.");
        else
        {
            is >> oct;
            if (! (is.setf(0,ios::basefield) & ios::oct))
                failure("ios::oct apparently not set.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: Manipulators.\n");
    DbgPrint("Testing the hex manipulator.\n");

    {
        strstreambuf sb;
        istream is(&sb);
        if (!is)
            failure("Can not attach stream to buffer.");
        else
        {
            is >> hex;
            if (! (is.setf(0,ios::basefield) & ios::hex))
                failure("ios::hex apparently not set.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
