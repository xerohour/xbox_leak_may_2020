/* @(#)File: ostream6.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class.
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the formatted output operations.
    < 
    < 0. operator<<(void *) 
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < These tests write a known value into the buffer,
    < then read it back and verify it. They write the float
    < to a second, local buffer and then compare the buffers.
    <
    < 0. The manual page says that values of type void * are
    <    converted to integers and written in hexadecimal. So
    <    after reading the value into a string it is compared to
    <    the pointer value written as a %x into a buffer.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/5/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "ostream6()";

static char * ptr;
const int len = 512;
static char buf[len];
static char buf2[len];

/*--------------------------------------------------------------------*/
void ostream6_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(void *).\n");

    {
        strstreambuf sb;
        void * p = (void*) 0x1234;
#if defined(USING_OLD_IOS)
        sprintf(buf2,"0x%x",p);
#else
        sprintf(buf2,"%p",p);
#endif 
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << p << '\0'))
            failure("Could not insert the pointer.");
        else if (((size_t)sb.sgetn(buf,(int)strlen(buf2)+1)) != strlen(buf2)+1)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Could not read back the pointer plus '\\0'.");
        else
        {
            if (buf[1] == 'X') buf[1] = 'x';
            if (strcmp(buf,buf2))
            {
                failure("Wrote wrong string.");
                DbgPrint("Got %s, wanted %s\n",buf,buf2);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
