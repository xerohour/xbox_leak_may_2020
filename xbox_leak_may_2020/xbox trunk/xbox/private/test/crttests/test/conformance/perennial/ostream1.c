/* @(#)File: ostream1.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class, 
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the output operations of the stream classes.
    < 
    < 0. put() writes a single character....
    <
    < 1. write(const char *, int) writes the characters 
    <
    < 2. write(const char *, int) can write NULL characters '\0'
    <

>HOW:   < The program uses a strstreambuf to hold data.
    <
    < 0.  Write a known string to a buffer. Then read the first
    <     character of the buffer with get. Verify the character.
    <
    < 1. Write a string and verify it's in the buffer.
    <
    < 2. Same as 1 but include '\0' characters in the string.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/5/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    <
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <strstrea.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "ostream1()";

    // The character buffer used by the streambuf.
static const int len = 1024;
static char strbuf[len];
    // A second buffer for I/O
static char buf[len];
static const char * ptr;

    // Read n bytes out of the strstreambuf.
int read(strstreambuf & sb, char * p, int n)
{
    return sb.sgetn(p,n) == n;
}

/*--------------------------------------------------------------------*/
void ostream1_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page.\n");
    DbgPrint("Testing put().\n");

    {
        strstreambuf sb(strbuf,len,strbuf);
        ostream os(&sb);
        char out_c = 'X';

        if (!os.good()) 
        failure("Cannot attach ostream to strstreambuf.");
        else 
        if (!os.put(out_c))
            failure("Cannot insert byte into ostream.");
        else 
            if (sb.sgetn(buf,1) != 1)
            failure("Can't extract the byte from the streambuf.");
            else
            if (*buf != out_c)
                failure("Extracted wrong character.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page.\n");
    DbgPrint("Testing write() of a string.\n");

    {
        strstreambuf sb(strbuf,len,strbuf);
        ostream os(&sb);
        char * ptr = "This is a test string.\n";

        if (!os.good()) 
        failure("Cannot attach ostream to strstreambuf.");
        else 
        if (!os.write(ptr,(streamsize) strlen(ptr)+1)) /*LX:10131999 cast to (streamsize)   */
            failure("Cannot insert string into ostream.");
        else 
            if (((size_t)sb.sgetn(buf,(streamsize) strlen(ptr)+1)) != strlen(ptr)+1)
            failure("Can't extract the string from the streambuf.");
            else
            if (strcmp(buf,ptr))
                failure("Extracted wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page.\n");
    DbgPrint("Testing write() of a buffer with NULL chars.\n");

    {
        strstreambuf sb(strbuf,len,strbuf);
        ostream os(&sb);
        const int ptr_len = 50;
        static char ptr[len] = "This \0 is \0 a \0 test \0 string.\n";

        if (!os.good()) 
        failure("Cannot attach ostream to strstreambuf.");
        else 
        if (!os.write(ptr,len))
            failure("Cannot insert string into ostream.");
        else 
            if (sb.sgetn(buf,len) != len)
            failure("Can't extract the string from the streambuf.");
            else
            if (memcmp(buf,ptr,len))
                failure("Extracted wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   

#include "peren_another.c"
