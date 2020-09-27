#include "typetest.h"

extern const char     GCc;
extern const uchar   GCuc;
extern const short    GCs;
extern const ushort  GCus;
extern const long     GCl;
extern const ulong   GCul;
extern const float    GCf;
extern const double   GCd;
extern const ldouble GCld;
extern const int	  GCi;
extern const uint	  GCui;
extern const __int64 GCi64;

extern ushort GShortNearArray[NEARSIZE];

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
extern int		Gi;
extern uint		Gui;
extern __int64	Gi64;

extern struct Gstruct
{
    short   Gs;
    uchar   Guc10[10];
}Gst, *Gpst;

extern union Gunion
{
    short   Gs2[2];
    ulong   Gul;
}Gun, *Gpun;



short	_cdecl CFncSSRecurse(short NTimes)
{

    const char	   Cc = (char)NTimes;
    const uchar   Cuc = (uchar)NTimes;
    const short    Cs = (short)NTimes;
    const ushort  Cus = (ushort)NTimes;
    const long	   Cl = (long)NTimes;
    const ulong   Cul = (ulong)NTimes;
    const float    Cf = (float)NTimes;
    const double   Cd = (double)NTimes;
    const ldouble Cld = (ldouble)NTimes;
	const int		Ci = (int)NTimes;
	const uint		Cui = (uint)NTimes;
	const __int64	Ci64 = (__int64)NTimes;

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

    c = (char)NTimes;
    uc = (uchar)NTimes;
    s = (short)NTimes;
    us = (ushort)NTimes;
    l = (long)NTimes;
    ul = (ulong)NTimes;
    f = (float)NTimes;
    d = (double)NTimes;
    ld = (ldouble)NTimes;
    i = (int)NTimes;
    ui = (uint)NTimes;
    i64 = (__int64)NTimes;

    if( NTimes != 0 )
	NTimes = (short)CFncSSRecurse(NTimes-1);
    return(NTimes);
}
