/* @(#)File: ostream3.C    Version: 1.1    Date: 01/02/91             */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular ostream class.
    < OSTREAM(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the formatted output operations.
    < 
    < 0.  operator<<(char *) writes characters until it reaches
    <     a NULL. It does not write the '\0' character.
    < 
    < 1.  operator<<(unsigned char *) same as previous block.
    < 
    < 2.  operator<<(char)
    < 
    < 3.  operator<<(unsigned char)
    < 
    < 4.  operator<<(short) 
    <
    < 5.  operator<<(short), the variable may be negative
    <
    < 6.  operator<<(unsigned short)
    <

>HOW:   < The program uses a strstreambuf as its stream buffer.
    <
    < Tests 0-6 all put a known value in the buffer,
    <       then read out and verify it.
    <
    < 0.  Write some words into a buffer then use the writeion
    <     operator to remove the first one. Then use get() to 
    <     read the whitespace following the first word. Verify
    <     the word read.
    <
    < 1.  Same as previous block but with a buffer pointer of
    <     type unsigned char *.
    <
    < 2.  Write a couple characters then read them back and verify.
    <
    < 3.  Same as 2 with unsigned characters.
    <
    < 4.  Write and check a positive short.
    <
    < 5.  Write and check a negative short.
    <
    < 6.  Write and check an unsigned short.
    <

>MODS:  < Vers 1.1, 01/02/91: RFG - Added code to pinpoint Cfront bug.
    < 07/02/90, DRE: Change definition of failure() to a prototype.
    < 3/12/90: dre: The initial revision had neglected to explain
    <     why the library fails block 1. Added the paragraph below to
    <     explain the problem.
    < Vers 1.0, date: 3/5/90 Created: DRE

>BUGS:  < The library fails block 1. This block tests the insertion
    < on a pointer of type (unsigned char *). The operator should
    < insert the string into the buffer, treating the pointer just
    < as if it were a (char *). Instead it treats the pointer the 
    < same way it treats a (void *). It writes the value of the 
    < pointer, i.e., the address of the string, into the buffer in
    < hexadecimal.
    
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "ostream3()";

static char * ptr;
const int len = 512;
static char buf[len];

/*--------------------------------------------------------------------*/
void ostream3_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(char *).\n");

    {
        strstreambuf sb;
        ptr = "word1\tword2\tword3\n";  // tabs are whitespace
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << ptr))
            failure("Could not insert string.");
        else if (((size_t)sb.sgetn(buf,(int) strlen(ptr))) != strlen(ptr))
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Could not read back string.");
        else if (memcmp(ptr,buf,strlen(ptr)))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(unsigned char *).\n");

    {
        size_t i; /*LX:10131999 int/size_t for 64-bit friendliness */
        strstreambuf sb;
        ptr = "word1\t1word2\tword3\n"; // tabs are whitespace
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << (unsigned char *) ptr))
            failure("Could not insert string.");
        else if ((i = sb.sgetn(buf,(int)strlen(ptr))) != strlen(ptr))
/*LX:10131999 added cast to (int) for 64-bit friendliness */
        {
            DbgPrint( "Count = %d\n", i);
            failure("Could not read back string.");
        }
        else if (memcmp(ptr,buf,strlen(ptr)))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(char).\n");

    {
        strstreambuf sb;
        ptr = "abc";
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << ptr[0]))
            failure("Could not insert first character.");
        else if (!(os << ptr[1]))
            failure("Could not insert second character.");
        else if (!(os << ptr[2]))
            failure("Could not insert third character.");
        else if (!(os << ptr[3]))
            failure("Could not insert '\\0' character.");
        else if (((size_t)sb.sgetn(buf,(int)strlen(ptr)+1)) != strlen(ptr)+1)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Could not read back string plus '\\0'.");
        else if (memcmp(ptr,buf,strlen(ptr)+1))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block3: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(unsigned char).\n");

    {
        strstreambuf sb;
        ptr = "abc";
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << (unsigned char) (ptr[0])))
            failure("Could not insert first character.");
        else if (!(os << (unsigned char) (ptr[1])))
            failure("Could not insert second character.");
        else if (!(os << (unsigned char) (ptr[2])))
            failure("Could not insert third character.");
        else if (!(os << (unsigned char) (ptr[3])))
            failure("Could not insert '\\0' character.");
        else if (((size_t)sb.sgetn(buf,(int)strlen(ptr)+1)) != strlen(ptr)+1)
/*LX:10131999 added cast to (int) for 64-bit friendliness */
            failure("Could not read back string plus '\\0'.");
        else if (memcmp(ptr,buf,strlen(ptr)+1))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block4: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(short).\n");

    {
        strstreambuf sb;
        short s = 123;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the short.");
        else if (sb.sgetn(buf,4) != 4)
            failure("Could not read back short plus '\\0'.");
        else if (memcmp("123",buf,4))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block5: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(short) with a negative.\n");

    {
        strstreambuf sb;
        short s = -123;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the short.");
        else if (sb.sgetn(buf,5) != 5)
            failure("Could not read back short plus '\\0'.");
        else if (memcmp("-123",buf,5))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    block6: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("OSTREAM(3C++) Manual Page: formatted output.\n");
    DbgPrint("Testing operator<<(unsigned short).\n");

    {
        strstreambuf sb;
        unsigned short s = 123;
        ostream os(&sb);
        if (!os.good())
            failure("Couldn't open data buffer.");
        else if (!(os << s << '\0'))
            failure("Could not insert the short.");
        else if (sb.sgetn(buf,4) != 4)
            failure("Could not read back short plus '\\0'.");
        else if (memcmp("123",buf,4))
            failure("Wrote wrong string.");
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   


#include "peren_another.c"
