/* @(#)File: stdiobuf.C    Version: 1.0    Date: 01/05/91             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular stdiostream and
    < stdiobuf classes, STDIOBUF(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the seek/tell operations of the stdiostream class.
    < 
    < These tests use the seekg positioning member to adjust
    < the get pointer of the streambuf. 
    <
    < 0. Positioning at the beginning of the buffer after reading
    <    a few bytes.
    < 
    < 1. Positioning at the end of the buffer before any reads.
    <
    < 2. Positioning at the end of the buffer after reading a few 
    <    characters.
    <
    < 3. Positioning at an offset from the beginning.
    <

>HOW:   < The program uses a stdiobuf as its stream buffer. It creates
    < and opens a Unix file and operates on the file. Then removes it.
    <
    < 0. Put something into the buffer. Read a few bytes remembering
    <    the first one read. Seek to the beginning, reread and check
    <    the first byte.
    <
    < 1. Jump to the end of the buffer and try to read. Should return
    <    EOF. 
    < 
    < 2. Same as 2 but read a few characters first.
    < 
    < 3. Read three characters and save the third. Then read several
    <    more. Then reposition to 2 off the beginning and try to read
    <    the saved character.

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < Vers 1.0, date: 3/4/90 Created: DRE

>BUGS:  < This test is dependent on the ANSI C stdio libraries
    < because it uses the tmpfile() library call.
    <
    < Block 3, the seek to a saved position, is failed by the 2.0
    < stream library, and some cfront 2.1.
    <
    
======================================================================*/
#define USING_OLD_IOS
#pragma warning(disable:4995)
#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <stdiostr.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <stdio.h>
#include "testhead.h"


static char progname[] = "stdiobuf()";

const int COUNT = 3;

static const char * file_data[COUNT] = {
    "abc 987",
    "line 2",
    "zyxw"
};

/* Put known contents in the data file, then seek back to beginning */
void writefile(FILE * tmpf);

static char * ptr;
static char c;

/*--------------------------------------------------------------------*/
void stdiobuf_main()                  
{
    setup();
#if defined(USING_OLD_IOS)

/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("STDIOBUF(3C++) Manual Page: seekg().\n");
    DbgPrint("Seeking to the start of the buffer after extracting.\n");

    {
        FILE * fp = tmpfile();
        if (fp == NULL)
            failure("Could not open temp file.");
        else
        {
        // Put known contents in file
            writefile(fp);
            stdiobuf sb(fp);
            istream is(&sb);
            char remembered_char;
            if (!is.good())
                failure("Couldn't open data buffer.");
            else
            {
                is >> remembered_char;
                if (remembered_char != file_data[0][0])
                    failure("Didn't extract correct character.");
                else
                {
            // Extract a few extra characters
                    is >> c;
                    is >> c;
                    is >> c;
                    if (!is)
                        failure("Errors during extraction.");
                    else
                    {
                        is.seekg(0,ios::beg);
                        if (is.get() != remembered_char)
                            failure("After seek got wrong character.");
                    }
                }
            }
            fclose(fp);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("STDIOBUF(3C++) Manual Page: seekg().\n");
    DbgPrint("Seek to the end of the buffer and try to read.\n");

    {
        FILE * fp = tmpfile();
        if (fp == NULL)
            failure("Could not open temp file.");
        else
        {
        // Put known contents in file
            writefile(fp);
            stdiobuf sb(fp);
            istream is(&sb);
            if (!is.good())
                failure("Couldn't open data buffer.");
            else
            {
                is.seekg(0,ios::end);
                if ((c = is.get()) != EOF)
                {
                    failure("After seeking to end didn't read EOF.");
                    DbgPrint( "Got character %d(%c).\n", c, c);
                }
            }
            fclose(fp);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("STDIOBUF(3C++) Manual Page: seekg().\n");
    DbgPrint("Read a few, then seek to the end of the buffer\n");
    DbgPrint("and try to read.\n");

    {
        FILE * fp = tmpfile();
        if (fp == NULL)
            failure("Can not open temp file.");
        else
        {
            stdiobuf sb(fp);
            istream is(&sb);
            if (!is.good())
                failure("Couldn't open data buffer.");
            else
            {
                is >> c;
                is >> c;
                is.seekg(0,ios::end);
                if ((c = is.get()) != EOF)
                {
                    failure("After seeking to end didn't read EOF.");
                    DbgPrint( "Got character %d(%c).\n", c, c);
                }
            }
            fclose(fp);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("STDIOBUF(3C++) Manual Page: seekg().\n");
    DbgPrint("Jump to an offset from the beginning.\n");

    {
        FILE * fp = tmpfile();
        if (fp == NULL)
            failure("Can not open temp file.");
        else
        {
            writefile(fp);
            stdiobuf sb(fp);
            istream is(&sb);
            if (!is.good())
                failure("Couldn't open data buffer.");
            else
            {
                char savechar;
                is >> c;
                is >> c;
                is >> savechar;
                if (savechar != file_data[0][2])
                {
                    failure("Got wrong data.");
                    DbgPrint( "Wanted %c(%d), got %c(%d)\n",
                            savechar,savechar, 
                            file_data[0][2], file_data[0][2]);
                }
                else
                {
                    is >> c;
                    is >> c;
                    is >> c;
                    is.seekg(2,ios::beg);
                    is >> c;
                    if (c != savechar)
                        failure("After seek to beginning+2 read wrong char.");
                }
            }
            fclose(fp);
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
#endif
    anyfail();  
}                   


/**********************************************************************/
/*                                                                    */
/* writefile                                                          */
/*                                                                    */
/* This routine writes the known data into the data file.             */
/* It seeks back to the beginning of the file after writing.          */
/*                                                                    */
/* Return:                                                            */
/*                                                                    */
/*    1   success                                                     */
/*    0   failure for some reason                                     */
/*                                                                    */
/**********************************************************************/

void writefile(FILE * tmpf)
{
    rewind(tmpf);
    const char ** pp = file_data;
    while (pp < file_data + COUNT)
        fprintf(tmpf, "%s\n", *pp++);
    fflush(tmpf);
    rewind(tmpf);
}

#include "peren_another.c"
