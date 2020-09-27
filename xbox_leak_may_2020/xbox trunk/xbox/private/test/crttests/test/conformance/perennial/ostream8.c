/* @(#)File: ostream8.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class.
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing formatted output.
    < 
    < 0. Test the endl manipulator.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0.   Write two integers separated by an endl and read
    <      it out of the buffer to be sure its correct.

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


static char progname[] = "ostream8()";

static char * ptr;
const int len = 512;
static char buf[len];
static char buf2[len];

/*--------------------------------------------------------------------*/
void ostream8_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing the endl manipulator.\n");

    {
        strstreambuf sb;
        int i1 = 17;
        int i2 = 34;
        sprintf(buf2,"%d\n%d",i1,i2);
        ostream os(&sb);
        if (!os.good())
            failure("Cannot open data buffer.");
        else if (!(os << i1 << endl << i2 << '\0'))
            failure("Cannot insert the values.");
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
