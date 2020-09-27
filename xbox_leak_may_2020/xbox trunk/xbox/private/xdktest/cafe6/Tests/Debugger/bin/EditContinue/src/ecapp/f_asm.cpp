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

extern char		Gc;
extern uchar   Guc;
extern short	Gs;
extern ushort  Gus;
extern long		Gl;
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



void _cdecl CFncAsm(void)
{
    ushort us = 0;

    _asm
	{
	    push ax
	    push bx
	    push cx
	    mov ax, 0xFFFF
	    mov bx, 0x0000
	    mov cx, 0xAAAA
	    mov us, 0xFFFF
	    int 3
	    pop cx
	    pop bx
	    pop ax
	}
}
