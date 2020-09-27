//**********************************************************************
//  Main source module for Testbp01 app : Testing of Breakpoints
//***********************************************************************

#include "testbp.h"

const char     GCc = (char)1;
const uchar   GCuc = (uchar)1;
const short    GCs = (short)1;
const ushort  GCus = (ushort)1;
const long     GCl = (long)1;
const ulong   GCul = (ulong)1;
const float    GCf = (float)1;
const double   GCd = (double)1;
const ldouble GCld = (ldouble)1;

ushort GShortNearArray[NEARSIZE];
//ushort _near GShortNearArray[NEARSIZE];
//ushort _far GShortFarArray[FARSIZE];
//ushort _huge GShortHugeArray[HUGESIZE];

char	 Gc;
uchar	Guc;
short	 Gs;
ushort	Gus;
long	 Gl;
ulong	Gul;
float	 Gf;
double	 Gd;
ldouble Gld;
short PassTest;

struct Gst
{
    short   Gs;
    uchar   Guc10[10];
}Gst, *Gpst = &Gst;
union Gun
{
    short   Gs2[2];
    ulong   Gul;
}Gun, *Gpun = &Gun;

int TestBP( void );


int TestBP()
{
#ifdef _M_ALPHA
    MIPS_DMY_PROLOG
#endif
    int counter;

    char     c;
    uchar   uc;
    short    s;
    ushort  us;
    long     l;
    ulong   ul;
    float    f;
    double   d;
    ldouble ld;

	 CFncVV ();
     c = CFncCC (c);
    uc = CFncUCUC(uc);
     s = CFncSS(s);
    us = CFncUSUS(us);
     l = CFncLL(l);
    ul = CFncULUL(ul);
     f = CFncFF(f);
     d = CFncDD(d);
    ld = CFncLDLD(ld);
     s = CFncSSRecurse(10);
	 CFncNearArray();

    for( counter = 0; counter < 10; counter++)
    {
		Gc  = (char)counter;
		Guc = (uchar)counter;
		Gs  = (short)counter;
		Gus = (ushort)counter;
		Gl  = (long)counter;
		Gul = (ulong)counter;
		Gf  = (float)counter;
		Gd  = (double)counter;
label:	c  = (char)counter;
		uc = (uchar)counter;
		s  = (short)counter;
		us = (ushort)counter;
		l  = (long)counter;
		ul = (ulong)counter;
		f  = (float)counter;
		d  = (double)counter;
		ld = (ldouble)counter;
		Gst.Gs = (short)counter;
		Gun.Gs2[0] = (short)counter;
		Gun.Gs2[1] = 9 - (short)counter;
    }
	return (1);
}
#include <windows.h>
int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
{
	int iBP = TestBP();
	return 0;
}
