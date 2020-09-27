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



void	_cdecl CFncNearArray(void)
{
    short i;

    for( i = 0; i < NEARSIZE; i++ )
	GShortNearArray[i]=i;

}
