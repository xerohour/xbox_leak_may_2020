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

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
    
    {
    auto struct {
        UINT b1: 1;
        UINT b2: 2;
        UINT b3: 3;
        UINT b4: 4;
        UINT b5: 5;
        UINT b6: 6;
        UINT b7: 7;
        UINT b8: 5;
        UINT b9: 4;
        UINT b10: 3;
        UINT b11: 2;
        UINT b12: 1;
        } K;
    K.b1 = 1;
    K.b2 = 2;
    K.b3 = 3;
    K.b4 = 4;
    K.b5 = 5;
    K.b6 = 6;
    K.b7 = 7;
    K.b8 = 5;
    K.b9 = 4;
    K.b10 = 3;
    K.b11 = 2;
    K.b12 = 1;
    {
    static struct H{
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
        struct H *pH;
        } H, *ppH = &H;
    H.pH = &H;
    ppH->pH->c = 7;
    ppH->pH->sc = 8;
    ppH->pH->s = 9;
    ppH->pH->i = 10;
    ppH->pH->uc = 11;
    ppH->pH->us = 12;
    ppH->pH->ui = 13;
    ppH->pH->l = 14;
    ppH->pH->ul = 15;
    ppH->pH->f = 16;
    ppH->pH->d = 17;
    ppH->pH->ld = 18;
    {
	break_here:
	return 0;
    }}
    }

