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
	auto CHAR Ac = 7;
	auto SCHAR Asc = 8;
	auto SHORT As = 9;
	auto INT Ai = 10;
	auto UCHAR Auc = 11;
	auto USHORT Aus = 12;
	auto UINT Aui = 13;
	auto LONG Al = 14;
	auto ULONG Aul = 15;
	auto FLOAT Af = 16;
	auto DOUBLE Ad = 17;
	auto LDOUBLE Ald = 18;
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
		} L, *pL = &L;
	pL->b1 = 1;
	pL->b2 = 2;
	pL->b3 = 3;
	pL->b4 = 4;
	pL->b5 = 5;
	pL->b6 = 6;
	pL->b7 = 7;
	pL->b8 = 5;
	pL->b9 = 4;
	pL->b10 = 3;
	pL->b11 = 2;
	pL->b12 = 1;
	{
	break_here:
	return 0;
	}}
	}
