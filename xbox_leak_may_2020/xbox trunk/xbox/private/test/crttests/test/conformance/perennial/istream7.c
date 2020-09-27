/* @(#)File: istream7.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream class:
    < ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the formatted input operations.
    < 
    < 0.  operator>>(char *) extracts characters until it reaches
    <     a whitespace. It does not extract the whitespace. 
    < 
    < 1.  operator>>(unsigned char *) same as previous block.
    < 
    < 2.  operator>>(char *): if width() is nonzero it is taken to  
    <     to be the size of the array and no more than width-1
    <     characters are extracted.
    < 
    < 3.  operator>>(char &)
    < 
    < 4.  operator>>(unsigned char &)
    < 
    < 5.  operator>>(short &) The string may have a leading '-'
    <
    < 6.  operator>>(short &) The string may have a leading '+'
    <
    < 7.  operator>>(int &) The string may represent a hex number
    <
    < 8.  operator>>(unsigned int &) The string may represent an 
    <     octal number
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < Tests 0-8 all put a known string in the buffer,
    <       then extract it an verify its value.
    <
    < 0.  Write some words into a buffer then use the extraction
    <     operator to remove the first one. Then use get() to 
    <     read the whitespace following the first word. Verify
    <     the word read.
    <
    < 1.  Same as previous block but with a buffer pointer of
    <     type unsigned char *.
    <
    < 2.  Extract a string but first set the width of the istream
    <     so that it can't extract the whole string. Verify that
    <     it extracts the correct number of characters.
    <
    < 3.  Extract and check a character
    <
    < 4.  Extract and check an unsigned character
    <
    < 5.  Extract and check a short, make it negative
    <
    < 6.  Extract and check an unsigned short, make it positive.
    <     The manual page does not indicate that unsigned variables
    <     can not be converted from signed strings.
    <
    < 7.  Extract and check a hex integer. If none of the base bits
    <     are set the normal C++ number base conventions are followed.
    <     So the number is stored in the form 0x????. So we turn off all
    <     of the ios::basefield bits.
    <
    < 8.  Extract and check a octal int. The basefield bits are turned off.

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


static char progname[] = "istream7()";

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
void istream7_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(char *).\n");

    {
        strstreambuf sb;
        char inbuf[128];
        ptr = "word1\tword2\tword3\n";  // tabs are whitespace
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is.width(128);
            is >> inbuf;
            if (strcmp(inbuf,"word1"))
                failure("Extracted wrong word.");
            else
                // Extract a few extra characters
                if (is.get() != '\t')
                    failure("After extracting string didn't get() whitespace.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(unsigned char *).\n");

    {
        strstreambuf sb;
        unsigned char inbuf[128];
        ptr = "word3\tword4\tword5\n";  // tabs are whitespace
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is.width(128);
            is >> inbuf;
            if (strcmp((const char *)inbuf,"word3"))
                failure("Extracted wrong word.");
            else
                // Extract a few extra characters
                if (is.get() != '\t')
                failure("After extracting string didn't get() whitespace.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(char *) with width.\n");

    {
        strstreambuf sb;
        char inbuf[128];
        ptr = "word6\tword7\tword8\n";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is.width(2);
            is >> inbuf;
            if (strcmp(inbuf,"w"))
            {
                failure("Extracted wrong word.");
                DbgPrint("Got '%s' not '%s'\n",inbuf,"w");
            }
            else
                // Extract a few extra characters
                if (is.get() != 'o')
                failure("After extracting string didn't get() right character.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(char &).\n");

    {
        strstreambuf sb;
        char c;
        ptr = "abcdefghijklmnopqrstuvwxyz";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> c;
            if (c != *ptr)
            {
                failure("Extracted wrong character.");
                DbgPrint("Got '%c'(%d) not '%c'(%d)\n",
                        c, c, *ptr, *ptr);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(unsigned char &).\n");

    {
        strstreambuf sb;
        unsigned char c;
        ptr = "abcdefghijklmnopqrstuvwxyz";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> c;
            if (c != *ptr)
            {
                failure("Extracted wrong character.");
                DbgPrint("Got '%c'(%d) not '%c'(%d)\n",
                        c, c, *ptr, *ptr);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(short &).\n");

    {
        strstreambuf sb;
        short s;
        ptr = "-123 456 789";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> s;
            if (s != -123)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %d not %d\n", s, 123);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block6: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(unsigned short &).\n");

    {
        strstreambuf sb;
        unsigned short s;
        ptr = "+123 456 789";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> s;
            if (s != 123)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %d not %d\n", s, 123);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block7: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(int &) formatted in hex.\n");
    DbgPrint("All formatting bits turned off.\n");

    {
        strstreambuf sb;
        int i;
        ptr = "0x9f8A 654 321";
        write(sb,ptr);
        istream is(&sb);
        is.setf(0,ios::basefield);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> i;
            if (i != 0x9f8A)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %d not %d\n", i, 0x9f8A);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block8: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing operator>>(unsigned int &) formatted in octal.\n");
    DbgPrint("All formatting bits turned off.\n");

    {
        strstreambuf sb;
        unsigned int i;
        ptr = "01234 654 321";
        write(sb,ptr);
        istream is(&sb);
        is.setf(0,ios::basefield);   // Turn off formatting bits
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> i;
            if (i != 01234)
            {
                failure("Extracted wrong number.");
                DbgPrint("Got %d not %d\n", i, 01234);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
