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
	auto CHAR Bc = 7;
	auto CHAR *pBc = &Bc;
	auto SCHAR Bsc = 8;
	auto SCHAR *pBsc = &Bsc;
	auto SHORT Bs = 9;
	auto SHORT *pBs = &Bs;
	auto INT Bi = 10;
	auto INT *pBi = &Bi;
	auto UCHAR Buc = 11;
	auto UCHAR *pBuc = &Buc;
	auto USHORT Bus = 12;
	auto USHORT *pBus = &Bus;
	auto UINT Bui = 13;
	auto UINT *pBui = &Bui;
	auto LONG Bl = 14;
	auto LONG *pBl = &Bl;
	auto ULONG Bul = 15;
	auto ULONG *pBul = &Bul;
	auto FLOAT Bf = 16;
	auto FLOAT *pBf = &Bf;
	auto DOUBLE Bd = 17;
	auto DOUBLE *pBd = &Bd;
	auto LDOUBLE Bld = 18;
	auto LDOUBLE *pBld = &Bld;
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
	break_here:
	return 0;
	}}
	}
