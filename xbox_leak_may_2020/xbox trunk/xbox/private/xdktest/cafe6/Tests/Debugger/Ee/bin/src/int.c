/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1991, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 3
 * DATE:    91/07/01
 * The "ANSI" mode of the Suite corresponds to the official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#include "defs.h"
#include "windows.h"

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
	
	{
	auto unsigned int ui;
	static unsigned int * pui;
	auto unsigned int ** ppui;
	auto int i;
	static int * pi;
	auto int ** ppi;
	auto unsigned short us;
	static unsigned short * pus;
	auto unsigned short ** ppus;
	auto short s;
	static short * ps;
	auto short ** pps;
	auto unsigned char uc;
	static unsigned char * puc;
	auto unsigned char ** ppuc;
	auto char c;
	static char * pc;
	auto char ** ppc;
	auto unsigned long ul;
	static unsigned long * pul;
	auto unsigned long ** ppul;
	auto long l;
	static long * pl;
	auto long ** ppl;
	register int rint1;
	register int rint2;
	register int rint3;
	register int rint4;
	register int rint5;

	ui = ivalue(100);
	pui = &ui;
	ppui = &pui;
	i = ivalue(10);
	pi = &i;
	ppi = &pi;
	us = lvalue(33000);
	pus = &us;
	ppus = &pus;
	s = ivalue(13);
	ps = &s;
	pps = &ps;
	uc = ivalue(128);
	puc = &uc;
	ppuc = &puc;
	c = ivalue(20);
	pc = &c;
	ppc = &pc;
	ul = lvalue(65000L);
	pul = &ul;
	ppul = &pul;
	l = lvalue(97L);
	pl = &l;
	ppl = &pl;
	rint1 = ivalue(1);
	rint2 = ivalue(2);
	rint3 = ivalue(3);
	rint4 = ivalue(4);
	rint5 = ivalue(5);

	break_here:
	return 0;
	}
