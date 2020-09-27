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



float	_cdecl CFncFF(float PSf)
{
    int counter;

    const char	   Cc = (char)1;
    const uchar   Cuc = (uchar)1;
    const short    Cs = (short)1;
    const ushort  Cus = (ushort)1;
    const long	   Cl = (long)1;
    const ulong   Cul = (ulong)1;
    const float    Cf = (float)1;
    const double   Cd = (double)1;
    const ldouble Cld = (ldouble)1;

    char     c, c10[10];
    uchar   uc, uc10[10];
    short    s, s10[10];
    ushort  us, us10[10];
    long     l, l10[10];
    ulong   ul, ul10[10];
    float    f, f10[10];
    double   d, d10[10];
    ldouble ld, ld10[10];
    struct st
    {
	short	s;
	uchar	uc10[10];
    }st, *pst = &st;
    union un
    {
	short	s2[2];
	ulong	 ul;
    }un, *pun = &un;

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
	Gld = (ldouble)counter;
	c  = (char)counter;
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
	c  = (char)counter;
	uc = (uchar)counter;
	s  = (short)counter;
	us = (ushort)counter;
	l  = (long)counter;
	ul = (ulong)counter;
	f  = (float)counter;
	d  = (double)counter;
	ld = (ldouble)counter;
	st.s = (short)counter;
	un.s2[0] = (short)counter;
	un.s2[1] = 9 - (short)counter;
	c10[counter]  = (char)counter;
	uc10[counter] = (uchar)counter;
	s10[counter]  = (short)counter;
	us10[counter] = (ushort)counter;
	l10[counter]  = (long)counter;
	ul10[counter] = (ulong)counter;
	f10[counter]  = (float)counter;
	d10[counter]  = (double)counter;
	ld10[counter] = (ldouble)counter;
	pst->uc10[counter] = (uchar)counter;
	pun->ul = (ulong)counter;
	PSf = (float)counter;
    }
    return PSf;
}
