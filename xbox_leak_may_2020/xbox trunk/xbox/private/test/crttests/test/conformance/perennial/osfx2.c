/* @(#)File: osfx2.C    Version: 1.0    Date: 09/27/90                */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ios and ostream
    < classes: IOS(3C++), OSTREAM(3C++) pages dated June 14, 1989.

>WHAT:  < Testing buffering and buffer flushing of the osfx() member
    < function and the stdio flag.
    < 
    < 0. If ios::stdio is set on an ostream then stdout and stderr 
    <    are flushed after every insertion.

>HOW:   < 0. Reopen stdout on a file. Open an ostream on another file.
    <    Set ios::stdio in the ostream. Write a few bytes to stdout.
    <    Insert into the stream. Read the file and check that the
    <    data was flushed.

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 4/30/90: Created: dre

>BUGS:  < The cfront 2.0 library fails this test. Stdout is not 
    < flushed upon the insertion. Adding a flush() confirms this 
    < fact.
    <
*/
/*====================================================================*/
/*                              osfx2.C                               */
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


static char progname[] = "osfx2()";

static char * _string = "abc";
const int buflen = 256;
static char buf[buflen];
static FILE * fpw;
static FILE * fpr;

/*--------------------------------------------------------------------*/
void osfx2_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

#if defined(USING_OLD_IOS)

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page, class ostream.\n");
    DbgPrint("Checking semantics of the osfx member function.\n");
    DbgPrint("Testing ``stdio'' buffering.\n");

    /* We do the check with an ofstream */

    char * _string = "abc";
    char nam1[L_tmpnam];
    char nam2[L_tmpnam];
    tmpnam(nam1);
    tmpnam(nam2);

    freopen(nam1, "w", stdout);
    ofstream os(nam2);
    os.flags(os.flags() | ios::stdio);
    DbgPrint(_string);
    os << "garbage";
    /* Stdout should have been flushed. */
    FILE * fpr = fopen(nam1, "r");
    /* fflush(stdout); */
    if (fread(buf, sizeof(*buf), strlen(_string), fpr) != strlen(_string))
        failure("Failed to read the data.");
    else
        if (strcmp(buf,_string))
            failure("Read the wrong data.");
    fclose(fpr);
    remove(nam1);
    remove(nam2);
#endif
    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   

#include "peren_another.c"
