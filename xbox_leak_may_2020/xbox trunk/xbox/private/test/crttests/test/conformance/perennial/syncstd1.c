/* @(#)File: syncstd1.C    Version: 1.0    Date: 10/19/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ios class,
    < IOS(3C++) page dated June 14, 1989.

>WHAT:  < Testing the sync_with_stdio member for mixing stream and stdio I/O.
    < 
    < 0. Input from stdin and cin can be mixed.
    <
    < 1. Output through stdout and cout can be mixed.
    <

>HOW:   < 0. Give a temp file known contents. Read the file in
    <    pieces and verify that the data come in correctly.
    <
    < 1. Write to a temp file through both packages. Read back the
    <    contents and check that they are corret.

>MODS:  < 07/02/90, DRE: Changed definition of failure() to a prototype.
    <    Included "testhead.h" instead of "testhead.h"
    < Vers 1.0, date: 5/04/90: Created: dre

>BUGS:  < ``cin >> (char*)'' reads characters until it hits a whitespace
    < and leaves the whitespace in the istream. Such an extraction
    < (under cfront 2.0 and its libraries)
    < following by a stdio fgets loses the whitespace. The whitespace
    < must be read with a stream character input such as  cin >> c.
    < This is tested in another file to see if it is symptomatic of 
    < a general problem involving putting back characters when streams
    < are synchronized with stdio.
*/
/*====================================================================*/
/*                              syncstd1.C                            */
/*====================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#endif
#include <stdio.h>
#include "remove.h"

#include "testhead.h"


static char progname[] = "syncstd1()";

static char namebuf[L_tmpnam];
static const char * fname = tmpnam(namebuf);

const int maxlen = 128;
const int maxstrings = 8;

static const char Rbuf[] = "abc\ndef\n12345\ncab\nqwerty \t\towef\n e 230f hello\n";
static const char * Rdata = Rbuf;
static char Wbuf[maxlen];
static char * Wdata = Wbuf;

static FILE * fpw;
static FILE * fpr;

/*--------------------------------------------------------------------*/
void syncstd1_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS(3C++) Manual Page, class ios.\n");
    DbgPrint("Testing sync_with_stdio member, mixed input.\n");

    ios::sync_with_stdio();

    // Put known contents in file.
    fpw = fopen(fname, "w");
    fputs(Rbuf, fpw);
    fclose(fpw);

    // Read back the data with mixed library calls
    freopen(fname, "r", stdin);
    rewind(stdin);
    char * writep = Wbuf;
    cin.flags(cin.flags() & ~ios::skipws);
    while (1)
    {
        // Do some mixed character I/O.
        int c;
        if ((c = getchar()) == EOF)
            break;
        *writep++ = c;
        if (!(cin >> *writep++))
            break;
        // Do some string I/O
        if (!(cin >> writep))
            break;
        writep += strlen(writep); // advance beyond what we just read
        if (*writep)
            DbgPrint( "Bad synchronization.\n");
        // Need to get the whitespace character
        // *** This should not be necessary but it is.
        if (!(cin >> *writep++))
            break;
        if (fgets(writep, maxlen, stdin) == NULL)
            break;
        writep += strlen(writep); // advance beyond what we just read
        if (*writep)
            DbgPrint( "Bad synchronization.\n");
        // And some more character I/O
        if ((c = getchar()) == EOF)
            break;
        *writep++ = c;
        if (!(cin >> *writep++))
            break;
    }
    if (strcmp(Rbuf,Wbuf))
    {
        failure("Data compares wrong.");
        DbgPrint( "String R(%d) '%s'\n\n", strlen(Rbuf),Rbuf);
        DbgPrint( "String W(%d) '%s'\n\n", strlen(Wbuf),Wbuf);
    }

    fclose(stdin);
    remove(fname);

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("IOS(3C++) Manual Page, class ios.\n");
    DbgPrint("Testing sync_with_stdio member, mixed output.\n");

    // Do mixed writes to file, then read it back and check it.
    freopen(fname, "w", stdout);
    ios::sync_with_stdio();
    const char * readp = Rbuf;
    size_t i; /*LX:10131999*/
    while (1)
    {
        // Do some mixed character I/O.
        if (!*readp) break;
        putchar(*readp++);
        if (!*readp) break;
        cout << *readp++;

        // Do some multicharacter I/P
        i = strlen(readp);
        if (i>3)
            i=3;
        if (i)
            cout.write(readp,(streamsize) i); /*LX:1013199*/
        else
            break;
        readp += i;

        i = strlen(readp);
        if (i>3)
            i=3;
        if (i)
            fwrite(readp, sizeof(*readp),i,stdout);
        else
            break;
        readp += i;
    }

    // Read the data back
    fpr = fopen(fname, "r");
    fread(Wbuf,sizeof(*Wbuf),sizeof(Wbuf),fpr);
    fclose(fpr);

    if (strcmp(Rbuf,Wbuf))
    {
        failure("Data compares wrong.");
        DbgPrint( "String R(%d) '%s'\n\n", strlen(Rbuf),Rbuf);
        DbgPrint( "String W(%d) '%s'\n\n", strlen(Wbuf),Wbuf);
    }

    fclose(stdout);
    remove(fname);

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   

#include "peren_another.c"
