/* @(#)File: istream6.C    Version: 1.0    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream and ostream
    < classes. ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the seek/tell operations of the istream classes.
    < 
    < These tests use the seekg positioning member to adjust
    < the get pointer of the streambuf. 
    <
    < 0. Positioning at the beginning of the buffer after reading
    <    a few bytes.
    < 
    < 1. Positioning at the beginning of the buffer after EOF.
    < 
    < 2. Positioning at the end of the buffer before any reads.
    <
    < 2. Positioning at the end of the buffer after reading a few 
    <    characters.
    <
    < 4. Positioning at a known spot in the buffer.
    <
    < 5. Positioning at an offset from the beginning.
    <
    < 6. Positioning at an offset from the end.
    <
    < 7. Positioning at an offset from the end. This time without
    <    extracting any characters before seeking.
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < 0. Put something into the buffer. Read a few bytes remembering
    <    the first one read. Seek to the beginning, reread and check
    <    the first byte.
    <
    < 1. Put something into the buffer. Remember the first byte and
    <    extract to EOF. Seek to the beginning, reread and check
    <    the first byte. This block uses an istrstream.
    <
    < 2. Jump to the end of the buffer and try to read. Should return
    <    EOF. Uses an istrstream.
    < 
    < 3. Same as 2 but read a few characters first.
    < 
    < 4. Read a few characters. Use tellg() to record the current 
    <    position. Extract and remember the next character. Extract
    <    a few more characters. Jump to the recorded position. Extract
    <    a character and verify it's the same as the remembered one.
    <
    < 5. Read three characters and save the third. Then read several
    <    more. Then reposition to 2 off the beginning and try to read
    <    the saved character.
    <
    < 6. Jump to -1 off the end and check what character we find.
    <
    < 7. Same as 6 except we don't extract a character before the
    <    seek. Use an istrstream.

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
    <    Include "testhead.h" rather than "testhead.h"
    < 3/4/90: Modified three blocks to use istrstreams instead of
    < istreams. One still fails, block 1, the other 2 succeed.
    < Vers 1.0, date: 3/3/90 Created: DRE

>BUGS:  < The Stream library fails block 1. After reading a strstreambuf
    < the EOF and seeking to the beginning, the next read still
    < yields EOF.
    <
    
======================================================================*/

#if defined(USING_OLD_IOS)
    #include <iostream.h>
#else
    #include <iostream>
    #include <strstream>
#endif

#include "testhead.h"


static char progname[] = "istream6()";

// Write a null-terminated string into a strstreambuf
// Don't write the NULL.
static int write(strstreambuf & sb, const char * s)
{
    while (*s)
        if (sb.sputc(*s++) == EOF)
            return EOF;
    return 0;
}

static char * ptr;
static char c;

/*--------------------------------------------------------------------*/
void istream6_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: seekg().\n");
    DbgPrint("Seeking to the start of the buffer after extracting.\n");

    {
        strstreambuf sb;
        char remembered_char;
        ptr = "0123456789";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> remembered_char;
            if (remembered_char != *ptr)
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
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: seekg().\n");
    DbgPrint("Seeking to the start of the buffer after EOF.\n");

    {
        char remembered_char;
        ptr = "0123456789";
        istrstream is(ptr);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> remembered_char;
            if (remembered_char != *ptr)
                failure("Didn't extract correct character.");
            else
            {
            // Extract a few extra characters
                while ((c = is.get()) != EOF)
                    ;
#if !defined(USING_OLD_IOS)
                is.clear();
#endif 
                if (is.seekg(0,ios::beg) == NULL)
                    failure("Error on seek.");
                else
                    if ((c = is.get()) != remembered_char)
                {
                    failure("After seek got wrong character.");
                    DbgPrint("Got %c(%d), not %c\n",c,c,*ptr);
                }
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: seekg().\n");
    DbgPrint("Seek to the end of the buffer and try to read.\n");

    {
        ptr = "0123456789";
        istrstream is(ptr);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is.seekg(0,ios::end);
            if ((c = is.get()) != EOF)
            {
                failure("After seeking to end didn't read EOF.");
                DbgPrint( "Got character %d(%c).\n",c,c);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: seekg().\n");
    DbgPrint("Read a few, then seek to the end of the buffer\n");
    DbgPrint("and try to read.\n");

    {
        strstreambuf sb;
        ptr = "0123456789";
        write(sb,ptr);
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
                DbgPrint( "Got character %d(%c).\n",c,c);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: tellg().\n");
    DbgPrint("Use tellg() to return to a position.\n");

    {
        strstreambuf sb;
        streampos savepos;
        char savechar;
        ptr = "0123456789";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> c;
            is >> c;
            savepos = is.tellg();
            is >> savechar;
            is >> c;
            is >> c;
            is.seekg(savepos);
            is >> c;
            if (c != savechar)
                failure("After seek to saved position read different char.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: seekg().\n");
    DbgPrint("Jump to an offset from the beginning.\n");

    {
        strstreambuf sb;
        char savechar;
        ptr = "0123456789";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> c;
            is >> c;
            is >> savechar;
            is >> c;
            is >> c;
            is >> c;
            is.seekg(2,ios::beg);
            is >> c;
            if (c != savechar)
                failure("After seek to 2 after beginning read wrong char.");
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block6: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: seekg().\n");
    DbgPrint("Jump to an offset from the end.\n");

    {
        strstreambuf sb;
        ptr = "0123456789";
        write(sb,ptr);
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is >> c;
            is.seekg(-1,ios::end);
            is >> c;
            if (c != ptr[strlen(ptr)-1])
            {
                failure("After seek to 1 before end read wrong char.");
                DbgPrint("Read %c(%d), wanted %c\n",
                        c, c, ptr[strlen(ptr)-1]);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    block7: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page: seekg().\n");
    DbgPrint("Jump to an offset from the end.\n");
    DbgPrint("Don't extract any characters first.\n");

    {
        ptr = "0123456789";
        istrstream is(ptr);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else
        {
            is.seekg(-1,ios::end);
            is >> c;
            if (c != ptr[strlen(ptr)-1])
            {
                failure("After seek to 1 before end read wrong char.");
                DbgPrint("Read %c(%d), wanted %c\n",
                        c, c, ptr[strlen(ptr)-1]);
            }
        }
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
