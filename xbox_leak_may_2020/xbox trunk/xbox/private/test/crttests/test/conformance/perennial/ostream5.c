/* @(#)File: ostream5.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class.
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the formatted output operations.
    < 
    < 0. operator<<(float) 
    <
    < 1. operator<<(double)
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < These tests write a known value into the buffer,
    < then read it back and verify it. They write the float
    < to a second, local buffer and then compare the buffers.

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


static char progname[] = "ostream5()"; 

static char * ptr;
const int len = 512;
static char buf[len];
static char buf2[len];

/*--------------------------------------------------------------------*/
void ostream5_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(float).\n");

    {
        strstreambuf sb;
        float x = (float)1234.56;
        size_t n; /*LX:10131999 /int/size_t for 64-bit friendliness */
        sprintf(buf2,"%2.2f",x);
        ostream os(&sb);
        if (!os.good())
        failure("Couldn't open data buffer.");
        else if (!(os << x << '\0'))
        failure("Could not insert the float.");
        else if ((size_t)(n = sb.sgetn(buf,(streamsize) strlen(buf2)+1)) != strlen(buf2)+1) 
/*LX:10131999 added cast to (streamsize) for 64-bit friendliness */
        failure("Could not read back int plus '\\0'.");
        else if (memcmp(buf2,buf,strlen(buf2)+1)) 
        failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(double).\n");

    {
        strstreambuf sb;
        double x = 7890.12;
        size_t n; /*LX:10131999 /int/size_t for 64-bit friendliness */
        sprintf(buf2,"%3.2lf",x);
        ostream os(&sb);
        if (!os.good())
        failure("Couldn't open data buffer.");
        else if (!(os << x << '\0'))
        failure("Could not insert the double.");
        else if ((size_t)(n = sb.sgetn(buf,(int)strlen(buf2)+1)) != strlen(buf2)+1) {
/*LX:10131999 added cast to (int) for 64-bit friendliness */
        failure("Could not read back int plus '\\0'.");
        DbgPrint("Got %s(%d)\n",buf,n);
        DbgPrint("Wanted %s(%d)\n",buf2,strlen(buf2)+1);
        }
        else if (memcmp(buf2,buf,strlen(buf2)+1)) {
        failure("Wrote wrong string.");
        DbgPrint("Got %s(%d)\n",buf,n);
        DbgPrint("Wanted %s(%d)\n",buf2,strlen(buf2)+1);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
