//**********************************************************************
//  Source module for Testbp01 app : Testing of Breakpoints
//***********************************************************************

#include "testbp.h"

extern const char     GCc;
extern const uchar   GCuc;
extern const short    GCs;
extern const ushort  GCus;
extern const long     GCl;
extern const ulong   GCul;
extern const float    GCf;
extern const double   GCd;
extern const ldouble GCld;

extern ushort /*_near*/ GShortNearArray[NEARSIZE];
//extern ushort _far GShortFarArray[FARSIZE];
//extern ushort _huge GShortHugeArray[HUGESIZE];

extern char	Gc;
extern uchar   Guc;
extern short	Gs;
extern ushort  Gus;
extern long	Gl;
extern ulong   Gul;
extern float	Gf;
extern double	Gd;
extern ldouble Gld;
extern short PassTest;

extern struct Gst
{
    short   Gs;
    uchar   Guc10[10];
}Gst, *Gpst;

extern union Gun
{
    short   Gs2[2];
    ulong   Gul;
}Gun, *Gpun;



short	_cdecl CFncSSRecurse(short NTimes)
{	MIPS_DMY_PROLOG

    const char	   Cc = (char)NTimes;
    const uchar   Cuc = (uchar)NTimes;
    const short    Cs = (short)NTimes;
    const ushort  Cus = (ushort)NTimes;
    const long	   Cl = (long)NTimes;
    const ulong   Cul = (ulong)NTimes;
    const float    Cf = (float)NTimes;
    const double   Cd = (double)NTimes;
    const ldouble Cld = (ldouble)NTimes;

    char     c;
    uchar   uc;
    short    s;
    ushort  us;
    long     l;
    ulong   ul;
    float    f;
    double   d;
    ldouble ld;

    c = (char)NTimes;
    uc = (uchar)NTimes;
    s = (short)NTimes;
    us = (ushort)NTimes;
    l = (long)NTimes;
    ul = (ulong)NTimes;
    f = (float)NTimes;
    d = (double)NTimes;
    ld = (ldouble)NTimes;

    if( NTimes != 0 )
	NTimes = (short)CFncSSRecurse((short)(NTimes-1));
    return(NTimes);
}
