#ifndef __DBGBASIC_H__
#define __DBGBASIC_H__

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

void Test_Disasm_MMX();
void Test_Disasm_Katmai();

void Test_Threads();

void Test_Registers();
void Test_MMX_Registers();
void Test_SSE_Registers();

extern "C" { void Test_Stack(); }

void Test_ICF();
void Test_Breakpoints();

#ifdef TEST_DLL_SUPPORT
int WINAPI TestDllFunc(int, char);
#endif

#endif


