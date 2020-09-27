/* @(#)File: istream2.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream and ostream
    < classes. ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the input operations of the istream classes.
    < 
    < 0. getline(char *, int, char) reads characters and a delimiter
    <
    < 1. same but with a different delimiter
    <
    < 2. When the delimiter and the trailing \0 character would
    <    overflow the array the delimiter is not read. The 
    <    delimiter is left in the stream.
    <

>HOW:   < The input tests work by writing something to a strstreambuf 
    < with non-stream I/O followed by reading it back with stream I/O.
    < The output tests work in the reverse direction.
    <
    < 0. Use getline to read a string up to and including a \n.
    <    However getline() does not appear to store the 
    <    delimiter in the buffer. So do a get() to get the
    <    next character and verify that it is the character
    <    following the delimiter.
    <
    < 1. Same as 1 but with a non-default delimiter.
    < 
    < 2. Read a string where the delimiter would not leave room in
    <    the array for the trailing \0, then verify that a get()
    <    returns the delimiter.
    < 

>MODS:  < Vers 1.0, date: 3/2/90 Created: DRE

>BUGS:  < None known.
    
======================================================================*/

/***************************************************************************
Micorosoft Revision History:

    Date        emailname       description
----------------------------------------------------------------------------
    02-23-95    a-timke         Conflict with var 'small' due from system
                                header rpcndr.h.
----------------------------------------------------------------------------
****************************************************************************/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <strstrea.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"    


static char progname[] = "istream2()";

    // Write a null-terminated string into a strstreambuf
    // Don't write the NULL.
static int write(strstreambuf & sb, const char * s)
{
    while (*s)
    if (sb.sputc(*s++) == EOF) 
        return EOF;
    return 0;
}

    // The character buffer used for I/O.
static const int len = 1024;
static char buf[len];
static const char * ptr;

/*--------------------------------------------------------------------*/
void istream2_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: getline().\n");
    DbgPrint("Testing getline(), read string and delimiter.\n");

    {
        strstreambuf sb;
        ptr = "Hello Goodbye \n1\n";
        const char * part = "Hello Goodbye ";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data buffer.");
        else  {
        *buf = 0;
        is.getline(buf,len);    // default delimiter '\n'
        if (strcmp(buf,part) || is.get() != '1')
            failure("Extracted wrong string.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: getline().\n");
    DbgPrint("Testing getline(), read string and delimiter.\n");
    DbgPrint("Specify non-default delimiter.\n");

    {
        ptr = "Hello Goodbye \n";
        const char * part = "Hello";
        strstreambuf sb;
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data buffer.");
        else  {
        *buf = 0;
        is.getline(buf,len,' '); // delimiter ' '
        if (strcmp(buf,part) || is.get() != 'G') {
            failure("Extracted wrong string.");
            DbgPrint("Got '%s', expected '%s'\n",
            buf,part);
        }
        }
    }
    
    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: getline().\n");
    DbgPrint("Testing getline() filling the buffer.\n");
    
    {
        ptr = "Hello\nGoodbye\n";
        const int k_nSmall = 6;
        const char * smallptr = "Hello";    // 5 chars plus '\0'
        strstreambuf sb;
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data buffer.");
        else  {
        *buf = '\0';
        is.getline(buf,k_nSmall); 
#if defined(USING_OLD_IOS)
        if (strcmp(buf,smallptr) || is.get() != '\n') {
#else
            if (strcmp(buf,smallptr)) {         // '\n' is eaten.
#endif 
            failure("Extracted wrong string.");
            DbgPrint("Got '%s', expected '%s'\n",
            buf,smallptr);
        }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/

    anyfail();  
}                   


#include "peren_another.c"
