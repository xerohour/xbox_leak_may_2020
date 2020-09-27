/* @(#)File: istream4.C    Version: 1.0    Date: 11/20/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream and ostream
    < classes. ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the input operations of the istream classes.
    < 
    < 0. Peek returns the next character without extracting it.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0. Peek at a character and then extract it to verify that
    <    the peek did not extract the character.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/2/90 Created: DRE

>BUGS:  < None known.
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istream4()";

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
void istream4_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: peek().\n");
    DbgPrint("Peek() returns a byte without extracting it.\n");

    ptr = "0123456789\n";
    strstreambuf sb;
    write(sb,ptr);
    char c;
    istream is(&sb);
    if (!is.good())
        failure("Couldn't open data buffer.");
    else  {
        c = is.peek();
        if (is.get() != c || c != *ptr)
            failure("Didn't peek correctly.");
    }

    blexit();
/*--------------------------------------------------------------------*/

    anyfail();  
}                   


#include "peren_another.c"
