/* @(#)File: istreamC.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream class:
    < ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the unformatted input operations.
    < 
    < 0. With the exception listed below extraction operators
    <    that use format state do not change the format state.
    <
    < 1. Extraction operators that use width reset it.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0. Extract with format and check that format is not unset.
    <
    < 1. Verify that width gets reset after use.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 5/12/90 Created: DRE

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


static char progname[] = "istreamC()";

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
void istreamc_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("Testing that format is not changed when used.\n");

    {
        strstreambuf sb;
        int i;

        write(sb,"0x1f 0x2e\n");
        istream is(&sb);

#if defined(USING_OLD_IOS)
        is.flags(is.flags() | ios::hex);
#else
        is.setf(ios::hex, ios::basefield); // otherwise both ios::dec and
                                           // ios::hex would be set.
#endif 
        if ((is.flags() & ios::hex) == 0)
            failure("Hex flag not set.");
        is >> i;
        if (i != 0x1f)
            failure("Extracted wrong value for first integer.");
        if ((is.flags() & ios::hex) == 0)
            failure("Hex flag reset.");
        is >> i;
        if (i != 0x2e)
            failure("Extracted wrong value for second integer.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: formatted input.\n");
    DbgPrint("After use the width flag is reset to zero.\n");

    {
        char ptr[10];
        strstreambuf sb;
        write(sb,"abcdefg");
        istream is(&sb);

        is.width(1);
        if (is.width(1) != 1)
            failure("Width not set to 1.");
        is >> ptr;
        if (is.width(1) != 0)
            failure("Width was reset.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
