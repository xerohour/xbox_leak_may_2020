/* @(#)File: istream5.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream and ostream
    < classes. ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the input operations of the istream classes.
    < 
    < 0. Putback can be used to put back the last character
    <    extracted from a stream. Get character with get().
    < 
    < 1. Get character with read.
    <
    < 2. Get character with >>
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0-2. Get a character, put it back, and get it again.
    <      Verify the character is the same. The blocks differ
    <      in how they get the character, get(), read() or >>.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/3/90 Created: DRE

>BUGS:  < None known.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istream5()";

/*--------------------------------------------------------------------*/
void istream5_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: peek().\n");
    DbgPrint("Putback can unget the last extracted character.\n");
    DbgPrint("Extracting a character with istream::get().\n");

    {
        char c = 'A';
        strstreambuf sb;
        sb.sputc(c);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data buffer.");
        else  {
        if (is.get() != c)
            failure("Got wrong character.");
        else {
            is.putback(c);
            if (is.get() != c)
            failure("After putback() got wrong character.");
        }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: peek().\n");
    DbgPrint("Putback can unget the last extracted character.\n");
    DbgPrint("Extracting a character with istream::read().\n");

    {
        const char c = 'B';
        char new_c;
        strstreambuf sb;
        sb.sputc(c);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data buffer.");
        else  {
        is.read(&new_c,1);
        if (is.gcount() != 1)
            failure("Couldn't read a character.");
        else if (new_c != c)
            failure("Read wrong character.");
        else {
            is.putback(new_c);
            is.read(&new_c,1);
            if (is.gcount() != 1)
            failure("Couldn't read a character after putback.");
            else if (new_c != c)
            failure("After putback() got wrong character.");
        }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: peek().\n");
    DbgPrint("Putback can unget the last extracted character.\n");
    DbgPrint("Extracting a character with istream::operator>>(char &).\n");

    {
        const char c = 'B';
        char new_c;
        strstreambuf sb;
        sb.sputc(c);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data buffer.");
        else  {
        is >> new_c;
        if (!is)
            failure("Couldn't read a character.");
        else if (new_c != c)
            failure("Read wrong character.");
        else {
            is.putback(new_c);
            is >> new_c;
            if (!is)
            failure("Couldn't read a character after putback.");
            else if (new_c != c)
            failure("After putback() got wrong character.");
        }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/

    anyfail();  
}                   


#include "peren_another.c"
