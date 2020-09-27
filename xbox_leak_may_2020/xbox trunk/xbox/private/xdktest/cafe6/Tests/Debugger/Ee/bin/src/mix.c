/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1991, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 3
 * DATE:    91/07/01
 * The "ANSI" mode of the Suite corresponds to the official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#include <windows.h>
#include "defs.h"

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
	
	{
	static float f1;
	auto float * pf1;
	static long stlong1;
	static float f2;
	auto float * pf2;
	static long stlong2;
	static float f3;
	static float * pf3;
	auto long stlong3;
	static float f4;
	static float * pf4;
	static long stlong4;
	static double d1;
	auto double * pd1;
	static long stlong5;
	static double d2;
	static double * pd2;
	static long stlong6;
	auto double d3;
	static double * pd3;
	static long stlong7;
	static double d4;
	static double * pd4;
	auto long stlong8;

	f1 = fvalue(1.100000E+000);
	pf1 = &f1;
	stlong1 = lvalue(1L);
	f2 = fvalue(2.200000E+000);
	pf2 = &f2;
	stlong2 = lvalue(2L);
	f3 = fvalue(3.300000E+000);
	pf3 = &f3;
	stlong3 = lvalue(3L);
	f4 = fvalue(4.400000E+000);
	pf4 = &f4;
	stlong4 = lvalue(4L);
	d1 = dvalue(1.100000000000000E+000);
	pd1 = &d1;
	stlong5 = lvalue(5L);
	d2 = dvalue(2.200000000000000E+000);
	pd2 = &d2;
	stlong6 = lvalue(6L);
	d3 = dvalue(3.300000000000000E+000);
	pd3 = &d3;
	stlong7 = lvalue(7L);
	d4 = dvalue(4.400000000000000E+000);
	pd4 = &d4;
	stlong8 = lvalue(8L);

	break_here:
	return 0;
	}
