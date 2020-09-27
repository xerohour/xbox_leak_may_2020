/* @(#)File: istream1.C    Version: 1.1    Date: 09/27/90             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular istream and ostream
    < classes. ISTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the input/output operations of the stream classes.
    < 
    < 0. get() returns a single character....
    <
    < 1. get() returns EOF. ios::failbit is never set.
    <
    < 2. gcount() returns the number of characters read in the
    <    last unformatted input.
    <
    < 3. gcount() returns the number of characters read in the
    <    last unformatted input. In this test we try 
    <    to read more bytes than are present.
    <
    < 4. get(char *, int, char) extracts characters stopping
    <    at a delimiter. The delimiter defaults to '\n'.
    <
    < 5. get(char *, int, char) extracts characters stopping
    <    at a delimiter. The delimiter may be specified.
    <
    < 6. get(char &) gets a single character.
    <

>HOW:   < The program uses a strstreambuf to hold data.
    <
    < 0.  Write a known string to a buffer. Then read the first
    <     character of the buffer with get. Verify the character.
    <
    < 1. Cause the buffer to be empty. Try and read the first 
    <    character. Verify that get() returns EOF.
    <    Verify that ios::failbit is not set.
    < 
    < 2. Compare gcount() to the length of a string read into 
    <    a buffer.
    < 
    < 3. Compare gcount() to the length of a string read into 
    <    a buffer, noting that we tried to read more bytes than
    <    were present.
    <
    < 4. Use get to read a string and verify that the string
    <    is correct.
    <
    < 5. Same as 4 but with a different delimiter.
    <
    < 6. Use get(char &) and verify the character.
    <

>MODS:  < Changed "testhead.h" to "testhead.h"
    <    Changed definition of failure() to a prototype.
    < Vers 1.1, date: 5/31/90: DRE
    <   Added the check to block 1 that ios::failbit is not set.
    < Vers 1.0, date: 3/2/90 Created: DRE

>BUGS:  < No bugs or system dependencies known.
    <
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif
#include "testhead.h"    


static char progname[] = "istream1()";

    // The character buffer used by the streambuf.
static const int len = 1024;
static char strbuf[len];
    // A second buffer for I/O
static char buf[len];
static const char * ptr;

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
void istream1_main()                 
{
    setup();
/*--------------------------------------------------------------------*/
block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page.\n");
    DbgPrint("Testing get(), not at EOF.\n");

    {
        strstreambuf sb(strbuf,len,strbuf);
        ptr = "Hello";
        if (write(sb,ptr) != EOF) {
            istream is(&sb);
            char c;
            if (!is.good())
                failure("Couldn't open stream buffer.");
            else 
                if ((c = is.get()) == EOF) 
                    failure("Couldn't read character.");
                else if (c != *ptr)
                    failure("Read wrong character.");
        }
        else
            failure("Write to test-buffer failed.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page.\n");
    DbgPrint("Testing get() from a strstreambuf at EOF.\n");

    {
        strstreambuf sb;
        istream is(&sb);
        if (!is.good())
        failure("Couldn't open data file.");
        else {
        char c;
        if ((c=is.get()) != EOF) {
            failure("Didn't return EOF.");
            DbgPrint("Got %c(%d).",c,c);
        }
#if defined(USING_OLD_IOS)
        if (is.rdstate() & ios::failbit)
            failure("ios::failbit was set.");
#else
        if (!(is.rdstate() & ios::failbit))
            failure("ios::failbit was not set.");
#endif 
        }
    }
    
    
    blexit();
/*--------------------------------------------------------------------*/
block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page.\n");
    DbgPrint("Testing gcount() after a read.\n");
    
    {
        strstreambuf sb(strbuf,len,strbuf);
        ptr = "Hello";
        if (write(sb,ptr) != EOF) {
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data buffer.");
        else {
            *buf = 0;
            is.read(buf,5);
            if (!is)
            failure("Couldn't read string.");
            else
            if (((size_t)is.gcount()) != strlen(ptr)) {
                failure("gcount() gives wrong count.");
                DbgPrint( "gcount(%d), strlen(%d)\n",
                    is.gcount(), strlen(ptr));
            }
        }
        }
        else
            failure("Write to test-buffer failed.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page.\n");
    DbgPrint("Testing gcount() after a too large read.\n");
    
    {
        strstreambuf sb;
        ptr = "Hello";
        if (write(sb,ptr) != EOF) {
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data file.");
        else {
            *buf = '\0';
            is.read(buf,len);
            if (((size_t)is.gcount()) != strlen(ptr)) {
                failure("gcount() gives wrong count.");
                DbgPrint( "gcount(%d), strlen(%d)\n",
                    is.gcount(), strlen(ptr));
            }
        }
        }
        else
        failure("Write to test-buffer failed.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block4: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page.\n");
    DbgPrint("get(char *, int, char) reads to a delimiter.\n");
    
    {
        strstreambuf sb(strbuf,len,strbuf);
        ptr = "word1 word2 word3\n";
        if (write(sb,ptr) != EOF) {
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data file.");
        else {
            *buf = 0;
            is.get(buf,len,'\n');
            strcat(buf,"\n");
            if (strcmp(buf,ptr)) {
            failure("Got the wrong string.");
            DbgPrint("Expected '%s', got '%s'.\n",
                ptr, buf);
            }
        }
        }
        else
        failure("Write to test-buffer failed.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block5: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page.\n");
    DbgPrint("get(char *, int, char) reads to a delimiter.\n");
    DbgPrint("This time change the delimiter.\n");
    
    {
        strstreambuf sb(strbuf,len,strbuf);
        ptr = "word1 word2 word3\n";
        if (write(sb,ptr) != EOF) {
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data file.");
        else {
            *buf = 0;
            is.get(buf,len,' ');  // new delimiter ' '
            if (strcmp(buf,"word1")) {
            failure("Got the wrong string.");
            DbgPrint("Expected '%s', got '%s'.\n",
                "word1", buf);
            }
        }
        }
        else
        failure("Write to test-buffer failed.");
    }

    blexit();
/*--------------------------------------------------------------------*/
block6: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("ISTREAM(3C++) Manual Page.\n");
    DbgPrint("get(char &) extracts a single character.\n");
    
    {
        strstreambuf sb(strbuf,len,strbuf);
        ptr = "Goodbye\n";
        if (write(sb,ptr) != EOF) {
        istream is(&sb);
        if (!is.good())
            failure("Couldn't open data file.");
        else {
            char c;
            is.get(c);
            if (c != *ptr)
                failure("Got the wrong character.");
        }
        }
        else
        failure("Write to test-buffer failed.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   

#include "peren_another.c"
