/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1991, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 3
 * DATE:    91/07/01
 * The "ANSI" mode of the Suite corresponds to the official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#include <windows.h>
#include "types.h"
#include "m.h"

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)

    {
    typedef union {
        CHAR c;
        SHORT s;
        INT i;
        UCHAR uc;
        USHORT us;
        UINT ui;
        LONG l;
        ULONG ul;
        FLOAT f;
        DOUBLE d;
        SCHAR sc;
        LDOUBLE ld;
        } E;
    auto E E1, *pE1 = &E1;
    auto E E2, *pE2 = &E2;
    auto E E3, *pE3 = &E3;
    auto E E4, *pE4 = &E4;
    auto E E5, *pE5 = &E5;
    auto E E6, *pE6 = &E6;
    auto E E7, *pE7 = &E7;
    auto E E8, *pE8 = &E8;
    auto E E9, *pE9 = &E9;
    auto E E10, *pE10 = &E10;
    auto E E11, *pE11 = &E11;
    auto E E12, *pE12 = &E12;
    pE1->c = 7;
    pE2->sc = 8;
    pE3->s = 9;
    pE4->i = 10;
    pE5->uc = 11;
    pE6->us = 12;
    pE7->ui = 13;
    pE8->l = 14;
    pE9->ul = 15;
    pE10->f = 16;
    pE11->d = 17;
    pE12->ld = 18;
    {
    CHAR Mc();
    SCHAR Msc();
    SHORT Ms();
    INT Mi();
    UCHAR Muc();
    USHORT Mus();
    UINT Mui();
    LONG Ml();
    ULONG Mul();
    FLOAT Mf();
    DOUBLE Md();
    LDOUBLE Mld();
    {
	break_here:
	return 0;
    }}
    }
