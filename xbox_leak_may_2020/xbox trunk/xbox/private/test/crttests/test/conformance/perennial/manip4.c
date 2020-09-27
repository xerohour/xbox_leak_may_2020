/* @(#)File: manip4.C		Version: 1.0		Date:09/27/90	*/
/* CVS-CC, C++ Validation Suite						*/
/* Copyright 1989, Perennial, All Rights Reserved			*/
/*========================================================================
        =================== TESTPLAN SEGMENT ===================
>KEYS:  < AT&T C++ Library Manual, Appendix A, MANIP(3C++)		>
	< in AT&T C++ Language System 2.0, May 89.  Select Code 307-145	>

>WHAT:  < This file tests format control manipulators setiosflags(long)	>
	< and resetiosflags(long) as well as the manipulators that	>
	< control the format components "width", "fill" and "precision".>
	< In this file, all the manipulators are tested using the <<	>
	< operator.  The tests with operator >> are in file manip1.C	>

>HOW:   < These manipulators can set the format variables to any given	>
	< value.  The tests use the corresponding function that allows	>
	< to get the value of the format variables to check they were	>
	< set to the proper values by the manipulators.			>

	< 0. Check manipulator setiosflags(long).			>
	< 1. Check manipulator resetiosflags(long).			>
	< 2. Check manipulator setfill(int).				>
	< 3. Check manipulator setw(int).				>
	< 4. Check manipulator setprecision(int).			>

>MODS:  < Version 1.0 created by HK on 3/4/90.				>

>BUGS:  < None known.							>
========================================================================*/
#include "testhead.h"     

static char progname[] = "manip4()";         

inline void print(char str[]) { DbgPrint( "%s\n", str); }
static void fail (char str[]) {
	local_flag =FAILED;
	DbgPrint( "\tFAILURE: %s\n", str);
	}

/*--------------------------------------------------------------------*/
#if defined(USING_OLD_IOS)
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#endif 

// global variable declarations
streambuf * sb;		//creates an empty buffer sb
istream i(sb);		//creates an input stream using sb

long new_format;
/*--------------------------------------------------------------------*/
manip4_main()                                  /***** BEGINNING OF MAIN. *****/
{

        setup();
/*--------------------------------------------------------------------*/
block0: blenter();                      

	print("REF: AT&T C++ Library Manual, Appendix A, MANIP(3C++)");
	print("Test manipulator setiosflags(long) with operator >>");

	i.flags(0l);		// clear all bits of format
	new_format = 0l;

	for (int j= 0; j<31; j++)
		{i >> setiosflags(1l<< j);
		new_format = new_format | (1l << j);
		if (i.flags() != new_format)
			{local_flag = FAILED;
			DbgPrint( "\tformat is %lx instead of %lx\n",
					i.flags(), new_format);
			}
		}

        blexit();
/*--------------------------------------------------------------------*/
block1: blenter();                      

	print("REF: AT&T C++ Library Manual, Appendix A, MANIP(3C++)");
	print("Test manipulator resetiosflags(long) with operator >>");

	i.flags(~0l);		// set all bits of format
	new_format = ~0l;

	for (j= 0; j<31; j++)
		{i >> resetiosflags(1l<< j);
		new_format = new_format & ~(1l << j);
		if (i.flags() != new_format)
			{local_flag = FAILED;
			DbgPrint( "\tformat is %lx instead of %lx\n",
					i.flags(), new_format);
			}
		}

        blexit();
/*--------------------------------------------------------------------*/
block2: blenter();                      

	print("REF: AT&T C++ Library Manual, Appendix A, MANIP(3C++)");
	print("Test manipulator setfill(int) with operator >>");

	i >> setfill(0);
	if (i.fill() != 0)
		fail("could not set fill to all zeroes");

	i >> setfill(~0);
	if (i.fill() != ~0)
		fail("could not set fill to all ones");

        blexit();
/*--------------------------------------------------------------------*/
block3: blenter();                      

	print("REF: AT&T C++ Library Manual, Appendix A, MANIP(3C++)");
	print("Test manipulator setw(int) with operator >>");

	i >> setw(0);
	if (i.width() != 0)
		fail("could not set width to all zeroes");

	i >> setw(~0);
	if (i.width() != ~0)
		fail("could not set width to all ones");

        blexit();
/*--------------------------------------------------------------------*/
block4: blenter();                      

	print("REF: AT&T C++ Library Manual, Appendix A, MANIP(3C++)");
	print("Test manipulator setprecision(int) with operator >>");

	i >> setprecision(0);
	if (i.precision() != 0)
		fail("could not set precision to all zeroes");

	i >> setprecision(~0);
	if (i.precision() != ~0)
		fail("could not set precision to all ones");

        blexit();
/*--------------------------------------------------------------------*/
        anyfail();      
}                                       /******** END OF MAIN. ********/


#include "peren_another.c"
