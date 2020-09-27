/* @(#)File: osfx.C    Version: 1.0    Date: 09/27/90                 */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ios and ostream
    < classes: IOS(3C++), OSTREAM(3C++) pages dated June 14, 1989.

>WHAT:  < Testing buffering and buffer flushing of the osfx() member
    < function and the unitbuf and stdio flags.
    < 
    < 0. Test that the unitbuf flag exists.
    < 
    < 1. Test that the stdio flag exists.
    <
    < 2. If ios::unitbuf is set then stream::osfx() flushes the
    <    stream after every insertion.
    <

>HOW:   < 0-1. Set the flag in stdio and verify it's set. This is really
    <      just a compile test to verify that the identifier is 
    <      defined. We test the behavior next.
    <
    < 2. Open a stream on a temporary file. Set the flag and 
    <    some data into the stream. Don't insert a newline and
    <    don't flush the stream. Open the file through a file 
    <    pointer and check the data.
    <

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 4/30/90: Created: dre

>BUGS:  < No bugs or system dependencies known.
    <
*/
/*====================================================================*/
/*                              osfx.C                                */
/*====================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include "remove.h"

#include "testhead.h"


static char progname[] = "osfx()";

static char * _string = "abc";
const int buflen = 256;
static char buf[buflen];
static FILE * fpw;
static FILE * fpr;

/*--------------------------------------------------------------------*/
void osfx_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS(3C++) Manual Page, class ios.\n");
    DbgPrint("Testing existance of the unitbuf flag.\n");

    long flags = cout.flags();
    cout.flags(flags | ios::unitbuf);
    if ((cout.flags() & ios::unitbuf) == 0)
        failure("ios::unitbuf flag not set.");
    /* Essentially if it compiles it succeeds */

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          
#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS(3C++) Manual Page, class ios.\n");
    DbgPrint("Testing existance of the stdio flag.\n");

    flags = cout.flags();
    cout.flags(flags | ios::stdio);
    if ((cout.flags() & ios::stdio) == 0)
        failure("ios::stdio flag not set.");
    /* Essentially if it compiles it succeeds */
#endif 

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page, class ostream.\n");
    DbgPrint("Checking semantics of the osfx member function.\n");
    DbgPrint("Testing ``unitbuf'' buffering.\n");

    /* We do the check with an ofstream */

    char * fname = tmpnam(0);
    { 
        ofstream os(fname);
        if (!os)
        {
            failure("Could not create ofstream on file.");
            goto end2;
        }

        flags = os.flags();
        os.flags(flags | ios::unitbuf);
        os << _string;
        if (!os)
        {
            failure("Could not insert data.");
            goto end2;
        }

        /* Reopen the file and verify the data was written */
        /* These are the important tests. */
        fpr = fopen(fname, "r");
        if (fread(buf, sizeof(*buf), strlen(_string), fpr) != strlen(_string))
            failure("Failed to read the data.");
        else
            if (strcmp(buf,_string))
            failure("Read the wrong data.");
        fclose(fpr);
    }
    end2:
    remove(fname);

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   

#include "peren_another.c"
