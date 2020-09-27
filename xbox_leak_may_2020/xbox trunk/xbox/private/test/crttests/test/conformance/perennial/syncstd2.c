/* @(#)File: syncstd2.C    Version: 1.0    Date: 10/19/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ios class,
    < IOS(3C++) page dated June 14, 1989.

>WHAT:  < Testing the sync_with_stdio member for mixing stream and stdio I/O.
    < Testing ungetting characters.
    < 
    < 0. Unget a character through stdio and see if stream I/O
    <    reads it.
    <
    < 1. Unget a character through stream I/O and see if stdio 
    <    reads it.
    <

>HOW:   < 0-1. Do the manipulations in a temporary disk file.
    <

>MODS:  < 07/02/90, DRE: Changed definition of failure() to a prototype.
    <    Included "testhead.h" instead of "testhead.h"
    < Vers 1.0, date: 5/12/90: Created: dre

>BUGS:  < Under cfront 2.0 and its stream libraries, when we putback
    < a character, the stdio library fails to read the character.
    < This appears to be a library bug.
    <
*/
/*====================================================================*/
/*                              syncstd2.C                            */
/*====================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#endif

#include <stdio.h>
#include "remove.h"

#include "testhead.h"


static char progname[] = "syncstd2()";

static char namebuf[L_tmpnam];
static const char * fname = tmpnam(namebuf);

static const char Rbuf[] = "abc";
static const char * Rdata = Rbuf;

static FILE * fpw;

/*--------------------------------------------------------------------*/
void syncstd2_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS(3C++) Manual Page, class ios.\n");
    DbgPrint("Testing sync_with_stdio member, ungetting.\n");
    DbgPrint("Ungetting through stream I/O.\n");

    ios::sync_with_stdio();

    // Put known contents in file.
    fpw = fopen(fname, "w");
    fputs(Rbuf, fpw);
    fclose(fpw);

    // Read back the data with mixed library calls
    freopen(fname, "r", stdin);
    rewind(stdin);

    char c, cc;

    cin >> c;
    cin.putback(c);
    if ((cc = getchar()) != c)
    {
        failure("Stdio didn't get the char the stream put back.");
        DbgPrint( "c='%c', cc='%c'\n",c,cc);
    }

    fclose(stdin);

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS(3C++) Manual Page, class ios.\n");
    DbgPrint("Testing sync_with_stdio member, ungetting.\n");
    DbgPrint("Ungetting through stdio.\n");

    // Read back the data with mixed library calls
    freopen(fname, "r", stdin);
    rewind(stdin);

    c = getchar();
    cc = c + 1;
    ungetc(c,stdin);
    cin >> cc;
    if (cc != c)
        failure("Stream I/O didn't get the char that stdio put back.");

    fclose(stdin);

    blexit();
/*--------------------------------------------------------------------*/
    remove(fname);
    anyfail();  
}                   

#include "peren_another.c"
