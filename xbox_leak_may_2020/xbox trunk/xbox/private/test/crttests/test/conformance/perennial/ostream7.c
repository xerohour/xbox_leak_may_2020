/* @(#)File: ostream7.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class.
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the format flags of formatted output.
    < 
    < 0. operator<<(int) with hex format
    <
    < 1. operator<<(int) with oct format
    <
    < 2. operator<<(int) with hex format set through a manipulator
    <
    < 3. operator<<(int) with oct format set through a manipulator
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < These tests write a known value into the buffer,
    < then read it back and verify it. They write the float
    < to a second, local buffer and then compare the buffers.
    <
    < 0.   Set the mode format to hex. Then,
    <      write an int into the buffer as a hex. Then read it
    <      out into a string and string compare with the same 
    <      value written into another buffer.
    <
    < 1.   Set the mode format to octal, then,
    <      Write an int into the buffer as a hex. Then read it
    <      out into a string and string compare with the same 
    <      value written into another buffer.
    <
    < 2,3. These tests are identical to 0 and 1 except that the
    <      base is changed through a manipulator rather than
    <      by through the ios::setf() function.

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/5/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <strstrea.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "ostream7()";

static char * ptr;
const int len = 512;
static char buf[len];
static char buf2[len];

/*--------------------------------------------------------------------*/
void ostream7_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(int) in hex.\n");

    {
        strstreambuf sb;
        int x = 56780;
        sprintf(buf2,"%x",x);
        ostream os(&sb);
        os.setf(ios::hex,ios::basefield);
        if (!os.good())
            failure("Cannot open data buffer.");
        else if (!(os << x << '\0'))
            failure("Cannot insert the value.");
        else if (((size_t)sb.sgetn(buf,(int)strlen(buf2)+1)) != strlen(buf2)+1)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Cannot read back the value plus '\\0'.");
        else
            if (strcmp(buf,buf2))
        {
            failure("Wrote wrong string.");
            DbgPrint("Got %s, wanted %s\n",buf,buf2);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(int) in octal.\n");

    {
        strstreambuf sb;
        int x = 65432;
        sprintf(buf2,"%o",x);
        ostream os(&sb);
        os.setf(ios::oct,ios::basefield);
        if (!os.good())
            failure("Cannot open data buffer.");
        else if (!(os << x << '\0'))
            failure("Cannot insert the value.");
        else if (((size_t)sb.sgetn(buf,(int)strlen(buf2)+1)) != strlen(buf2)+1)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Cannot read back the value plus '\\0'.");
        else
            if (strcmp(buf,buf2))
        {
            failure("Wrote wrong string.");
            DbgPrint("Got %s, wanted %s\n",buf,buf2);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(int) with hex manipulator.\n");

    {
        strstreambuf sb;
        int x = 56780;
        sprintf(buf2,"%x",x);
        ostream os(&sb);
        if (!os.good())
            failure("Cannot open data buffer.");
        else if (!(os << hex << x << '\0'))
            failure("Cannot insert the value.");
        else if (((size_t)sb.sgetn(buf,(int)strlen(buf2)+1)) != strlen(buf2)+1)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Cannot read back the value plus '\\0'.");
        else
            if (strcmp(buf,buf2))
        {
            failure("Wrote wrong string.");
            DbgPrint("Got %s, wanted %s\n",buf,buf2);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(int) with oct manipulator.\n");

    {
        strstreambuf sb;
        int x = 65432;
        sprintf(buf2,"%o",x);
        ostream os(&sb);
        if (!os.good())
            failure("Cannot open data buffer.");
        else if (!(os << oct << x << '\0'))
            failure("Cannot insert the value.");
        else if (((size_t)sb.sgetn(buf,(int)strlen(buf2)+1)) != strlen(buf2)+1)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Cannot read back the value plus '\\0'.");
        else
            if (strcmp(buf,buf2))
        {
            failure("Wrote wrong string.");
            DbgPrint("Got %s, wanted %s\n",buf,buf2);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
