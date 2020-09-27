/* @(#)File: istream9.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream class:
    < ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the formatted input operations.
    < 
    < 0.  operator>>(long &), forcing hex representation
    <     octal number
    <
    < 1. operator>>(unsigned long &), forcing octal
    <
    < 2. operator>>(float &)
    <
    < 3. operator>>(double &)
    <
    < 4. operator>>(streambuf *) takes characters out of the 
    <     istream and inserts them into the streambuf.
    <
    < 5. operator>>(int) sets ios::failbit if there are no digits
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < All these tests put a known string in the buffer,
    <       then extract it an verify its value.
    <
    < 0. Extract and check a hex long. Turn on hex base.
    <
    < 1. Extract and check an octal unsigned long. Turn on octal base.
    <
    < 2. Extract and check a float. We must not use the float as a 
    <    constant in the comparison with the extracted value. Instead
    <    we must use another float variable. Otherwise, the float
    <    constant is promoted to double and we might possibly compare
    <    incorrectly. This happens if the particular float chosen is
    <    not exactly representable in a float on the current architecture.
    <
    < 3. Extract and check a double. 
    <
    < 4. Extract into a strstreambuf and then get the characters out of the
    <    buffer.
    <
    < 5. Try and extract an int when there are no digits, then check that
    <    failbit is set.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/4/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istream9()";

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
void istream9_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(long &) formatted in hex.\n");
    DbgPrint("Turning on hex format bit.\n");

    {
        strstreambuf sb;
        long l;
        ptr = "01234 654 321";
        write(sb,ptr);
        istream is(&sb);
        // Turn on hex format, could also use a manipulator
        is.setf(ios::hex,ios::basefield);    
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> l;
            if (l != 0x01234)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %d not %d\n", l, 0x01234);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(unsigned long &) formatted in octal.\n");
    DbgPrint("Turning on octal format bit.\n");

    {
        strstreambuf sb;
        unsigned long l;
        ptr = "678 654 321";
        write(sb,ptr);
        istream is(&sb);
        // Turn on hex format, could also use a manipulator
        is.setf(ios::oct,ios::basefield);    
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> l;
            if (l != 067)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %d not %d\n", l, 067);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(float &).\n");

    {
        strstreambuf sb;
        float result = (float)-678.123;
        float f;
        ptr = "-678.123 654.0 321";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> f;
            if (f != result)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %f not %f\n", f, result);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(double &).\n");

    {
        strstreambuf sb;
        double result = +6.78e2;
        double d;
        ptr = "+6.78e2 654.0 321";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> d;
            if (d != result)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %l not %l\n", d, result);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(streambuf *).\n");

    {
        strstreambuf sb;
        strstreambuf dest;
        char buf[10];
        ptr = "abc";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> &dest;
            dest.sgetn(buf,3);
            buf[3] = 0;
            if (strcmp(buf,ptr))
                failure("Failed to extract right string.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(int&) on bad data.\n");

    {
        strstreambuf sb;
        ptr = "a123 xyz\n";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            int i;
            is >> i;
            if (!is.fail())
                failure("Failed to set ios::failbit.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
