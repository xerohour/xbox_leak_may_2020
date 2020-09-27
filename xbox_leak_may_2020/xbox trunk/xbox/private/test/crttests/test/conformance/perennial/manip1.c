/* @(#)File: manip1.C    Version: 1.0    Date: 01/02/91               */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
    =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
    < I/O Library Manual pages, In particular user-defined
    < manipulators. MANIP(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the ability to define manipulators
    < There was one other test in this file but it caused
    < a nonrecoverable compiler error. It was moved to
    < the file manip2.C so that these tests could still
    < compile and run.
    < 
    < 0. A non-parameterized istream manipulator.
    <    Define an istream manipulator to turn off skipws.
    <
    < 1. A non-parameterized ostream manipulator.
    <    Define an ostream manipulator to insert a space.
    <
    < 2. A parameterized ostream manipulator.
    <    Define an ostream manipulator to insert n spaces.
    <
>HOW:   < 0. Define the manipulator. The read from an istream
    <    one string that contains whitespace. Verify that the
    <    whitespace is removed by default. Use the manipulator to 
    <    turn off skipws. Then reread the string and verify the
    <    whitespace is present. The stream buffer used is a
    <    strstreambuf for convenience.
    <
    < 1. Use the manipulator to insert a space and verify the 
    <    result.
    <
    < 2. Use the manipulator to insert several spaces and verify 
    <    the result.

>MODS:  < 11/8/90, DRE: Updated BUGS comments for 2.1.
    < 07/02/90, DRE: Change definition of failure() to a prototype.
    < 5/5/90, dre, changed some comments
    < Vers 1.0, date: 3/6/90 Created: DRE

>BUGS:  < No bugs or system dependencies known in Cfront 2.0.
    < Cfront 2.1 fails block 0 of this test.
    <
======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#include <ostream>
#include <strstream>
#endif

#include "testhead.h"


static char progname[] = "manip1()";

/**********************************************************************/
/*                                                                    */
/* noskipws                                                           */
/*                                                                    */
/* An istream manipulator to turn off skipws.                         */
/*                                                                    */
/**********************************************************************/

istream & noskipws(istream & i)
{
    i.flags(i.flags() & ~ios::skipws);
    return i;
}

/**********************************************************************/
/*                                                                    */
/* sp                                                                 */
/*                                                                    */
/* An ostream manipulator to insert a space into the ostream.         */
/*                                                                    */
/**********************************************************************/
/*
ostream & sp(ostream & o)
{
    return o << ' ';
}
*/
/**********************************************************************/
/*                                                                    */
/* sp(n)                                                              */
/*                                                                    */
/* An ostream manipulator to insert n spaces into the ostream.        */
/*                                                                    */
/**********************************************************************/

#if defined(USING_OLD_IOS)
ostream & sp(ostream & o, int nspaces)
#else
void __cdecl _sp(ostream& o, int nspaces)
#endif 
{
    int i;

    for (i=0; i<nspaces; i++)
        o << ' ';

#if defined(USING_OLD_IOS)
    return o;
#endif 
}

#if defined(USING_OLD_IOS)
OMANIP (int) sp(int n)
{
    return OMANIP(int)(sp,n);
}
#else
//!mmalone: this doesn't work and should? monte carlo 6548.
#ifndef _XBOX
_Smanip<int> __cdecl sp(int n) {return _Smanip<int>(&_sp, n);}
#endif
#endif 
/*--------------------------------------------------------------------*/
void manip1_main()                  
{
    setup();
/*--------------------------------------------------------------------*/
    block0: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("MANIP(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("Use an istream manipulator to turn off skipws\n");

    {
        const char * ptr = "abc def ghi\n";
        const char * result = "abcdefghi";
        char buf[128];

        // First make sure that skipping whitespace works
        {
            strstreambuf sb;
            istream is(&sb);
            is.flags(is.flags() | ios::skipws);
            sb.sputn(ptr,strlen(ptr)+1);
            char * p = buf;
            while (is >> *p++)
                ;
            if (!strcmp(buf,ptr))
                failure("Istream is not skipping whitespace.");
            else if (strcmp(buf,result))
                failure("Istream is not yielding expected result.");
        }
#if defined(USING_OLD_IOS)

        // Now test the manipulator
        if (local_flag != FAILED)
        {
            strstreambuf sb;
            istream is(&sb);
            is.flags(is.flags() | ios::skipws);
            sb.sputn(ptr,strlen(ptr)+1);
            char * p = buf;
            is >> noskipws;

            while (is >> *p++)
                ;
            if (!strcmp(buf,result))
                failure("Manipulator did not work.");
            else if (strcmp(buf,ptr))
            {
                failure("Didn't get expected result.");
                DbgPrint("Wanted '%s', got '%s'.\n",ptr,buf);
            }
        }
#endif 
    }

    blexit();
/*--------------------------------------------------------------------*/
    block1: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("MANIP(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("Use an ostream manipulator to insert a space.\n");

    {
#if defined(USING_OLD_IOS)

        char buf[128];
        strstreambuf sb;
        ostream os(&sb);

        if (!(os << 'a' << sp << 'b' << '\0'))
            failure("Insertion failed.");
        else if (sb.sgetn(buf,4) != 4)
            failure("Couldn't read characters.");
        else if (strcmp(buf,"a b"))
            failure("Got wrong string.");
#endif 
    }

    blexit();
/*--------------------------------------------------------------------*/
    block2: blenter();          

    DbgPrint("REF: AT&T Library Reference February 1990\n");
    DbgPrint("Appendix A-4: Testing I/O Streams.\n");
    DbgPrint("MANIP(3C++) manual page dated 14 June 1989.\n");
    DbgPrint("Use an ostream manipulator to insert n spaces.\n");

    {
#if defined(USING_OLD_IOS)

        char buf[128];
        strstreambuf sb;
        ostream os(&sb);

        if (!(os << 'a' << sp(3) << 'b' << '\0'))
            failure("Insertion failed.");
        else if (sb.sgetn(buf,6) != 6)
            failure("Couldn't read characters.");
        else if (strcmp(buf,"a   b"))
            failure("Got wrong string.");
#endif 
    }

    blexit();
/*--------------------------------------------------------------------*/
    anyfail();  
}                   

#include "peren_another.c"
