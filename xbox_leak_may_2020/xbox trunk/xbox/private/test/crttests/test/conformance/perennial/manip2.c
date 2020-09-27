/* @(#)File: manip2.C    Version: 1.0    Date: 09/27/90               */
/* CVS-CC, C++ Validation Suite                                       */
/* Copyright 1989, Perennial, All Rights Reserved                     */
/*======================================================================
	=================== TESTPLAN SEGMENT ===================
>KEYS:	< AT&T C++ Library Reference, Feb 90, Appendix A-4, Stream
	< I/O Library Manual pages, In particular user-defined
	< manipulators. MANIP(3C++) manual page dated 14 June 1989

>WHAT:  < Testing the ability to define manipulators
	<
	< 0. A parameterized istream manipulator.
	<    Define an istream manipulator to read an int that may
	<    have a suffix. Taken from Iostream examples, Manipulators,
	<    AT&T Document 307-145 Chapter 3. 
	<
>HOW:	< 0. Define the manipulator and use it to read two ints
        <    out of a strstreambuf, verify the resulting values. 
	<    One should have been multiplied by 1024.

>MODS:  < 07/02/90, DRE: Change definition of failure() to a prototype.
	<    Include "testhead.h" rather than "testhead.h"
	< 5/5/90: dre: changed some comments
	< Vers 1.0, 3/6/90: dre: created

>BUGS:  < Some 2.1 cfronts fail to compile this.

======================================================================*/

#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#endif

#include "testhead.h"


static char progname[] = "manip2()";

/**********************************************************************/
/*                                                                    */
/* in_t(int&)                                                         */
/*                                                                    */
/* An istream manipulator to read an int with an optional suffix      */
/* of 'k' to mean multiply by 1024. See the banner to this            */
/* file for the source of this manipulator.                           */
/*                                                                    */
/**********************************************************************/

typedef int & Iref;

IOMANIPdeclare(Iref) ;

istream & in_k(istream & i, int & n)
{
    // Extract an integer, if suffix is present multiply by 1024
    i >> n;
    if (i.peek() == 'k') {
	i.ignore(1);
	n *= 1024;
    }
    return i;
}

IAPP(Iref) in_k = in_k;

/*--------------------------------------------------------------------*/
manip2_main()					
{
	setup();
/*--------------------------------------------------------------------*/
block0:	blenter();			

	DbgPrint("REF: AT&T Library Reference February 1990\n");
	DbgPrint("Appendix A-4: Testing I/O Streams.\n");
	DbgPrint("MANIP(3C++) manual page dated 14 June 1989.\n");
	DbgPrint("Use an istream manipulator to read an int\n");
	DbgPrint("with a suffix.\n");

	int n;
	{
		streambuf sb;
		istream is(&sb);
		sb.sputn("123 ",5);
		is >> in_k(n);
		if (n != 123)
			failure("Read wrong number without suffix.");
	}
	{
		streambuf sb;
		istream is(&sb);
		sb.sputn("123k ",5);
		is >> in_k(n);
		if (n != 123*1024)
			failure("Read wrong number with suffix.");
	}

	blexit();
/*--------------------------------------------------------------------*/
	anyfail();	
}					

#include "peren_another.c"
