#include "typetest.h"

extern "C"
{
	void Types_Tests( int );
}

const char     GCc = (char)1;
const uchar   GCuc = (uchar)1;
const short    GCs = (short)1;
const ushort  GCus = (ushort)1;
const long     GCl = (long)1;
const ulong   GCul = (ulong)1;
const float    GCf = (float)1;
const double   GCd = (double)1;
const ldouble GCld = (ldouble)1;
const int	  GCi  = (int)1;
const uint	  GCui = (uint)1;
const __int64 GCi64 = (__int64)1;

ushort GShortNearArray[NEARSIZE];

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
int		Gi;
uint	Gui;
__int64	Gi64;

struct Gstruct
{
    short   Gs;
    uchar   Guc10[10];
}Gst, *Gpst = &Gst;

union Gunion
{
    short   Gs2[2];
    ulong   Gul;
}Gun, *Gpun = &Gun;


void Types_Tests(int nLoop)
{
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
	int		i;
	uint	ui;
	__int64	i64;

    for( counter = 0; counter < nLoop; counter++)
    {
		Gc  = (char)counter;
		Guc = (uchar)counter;
		Gs  = (short)counter;
		Gus = (ushort)counter;
		Gl  = (long)counter;
		Gul = (ulong)counter;
		Gf  = (float)counter;
		Gd  = (double)counter;
		Gi  = (int)counter;
		Gui = (uint)counter;
		Gi64 = (__int64)counter;

		CFncVV ();
		c = CFncCC (c);
	    uc = CFncUCUC(uc);
		i = CFncInt(i);
		ui = CFncUInt(ui);
		i64 = CFncInt64(i64);
		s = CFncSS(s);
	    us = CFncUSUS(us);
		l = CFncLL(l);
	    ul = CFncULUL(ul);
		f = CFncFF(f);
	    d = CFncDD(d);
		ld = CFncLDLD(ld);
	    s = CFncSSRecurse(10);

		Gst.Gs = (short)counter;
		Gun.Gs2[0] = (short)counter;
		Gun.Gs2[1] = 9 - (short)counter;
    }

	CFncAsm();

}

