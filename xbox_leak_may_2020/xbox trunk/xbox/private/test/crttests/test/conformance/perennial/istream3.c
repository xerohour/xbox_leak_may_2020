/* @(#)File: istream3.C    Version: 1.1    Date: 11/20/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream class.
    < ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the input operations of the istream classes.
    < 
    < 0. ignore(int n, int delim) extracts and throws away up to
    <    n characters.
    <
    < 1. ignore(int n, int delim) stops if delim is extracted.
    <

>HOW:   < The program uses a strstreambuf for its stream buffer.
    <
    < 0. Ignore some characters and check that a subsequent
    <    get() operation returns the correct character.
    <
    < 1. Ignore some characters but have a delimiter in the input
    <    before the limit is reached. Verify that the next character
    <    read is the correct one.
    < 

>MODS:  < Vers 1.1, date: 05/31/90 DRE Corrected some printfs
    < Vers 1.0, date: 03/02/90 DRE created

>BUGS:  < None known.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istream3()";

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
static const char * ptr;

/*--------------------------------------------------------------------*/
void istream3_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: ignore().\n");
    DbgPrint("Testing ignore(), extract/discard characters.\n");

    int count = 6;
    ptr = "Hello Goodbye \n1\n";
    char c = ptr[count];

    {
        strstreambuf sb;
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data file.");
        else  {
        is.ignore(count,'\n');  
        if (is.get() != c)
            failure("Ignored wrong characters.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: ignore().\n");
    DbgPrint("Testing ignore(), extract/discard characters.\n");
    DbgPrint("Ignore should stop after a delimiter.\n");

    ptr = "Hello Goodbye \n1\n";
    char delim = 'o';
    c = ' ';

    {
        strstreambuf sb;
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data file.");
        else  {
        is.ignore(count,delim); 
        if (is.get() != c)
            failure("Ignored wrong characters.");
        }
    }
    
    blexit();
/*--------------------------------------------------------------------*/

    anyfail();  
}                   


#include "peren_another.c"
