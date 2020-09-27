//---------------------------------------------------------------------------
// DBGOUT.H
//
// This header file defines macros
//---------------------------------------------------------------------------
#pragma once
#ifndef _DBGOUT_INCLUDED
#define _DBGOUT_INCLUDED

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#include "VsAssert.H" // common assert mechanism

#ifdef DEBUG

VOID TDAssert (CHAR *, CHAR *, UINT);
VOID __cdecl TDDebugOutput (LPSTR, ...);
VOID __cdecl TDOutString (LPSTR, ...);
VOID __cdecl TDOutIfAux (LPSTR, ...);
VOID __cdecl TDDebugPrintf (LPSTR, ...);
VOID __cdecl TDOutLine (LPSTR, ...);

extern BOOL         auxport;
extern BOOL         *g_pfEnableDebugOutput;

//#define Assert(exp) ((exp)?(void)0:TDAssert (#exp, __FILE__, __LINE__))
//#ifndef ASSERT
//#define ASSERT(exp) ((exp)?(void)0:TDAssert (#exp, __FILE__, __LINE__))
//#endif

#define Assert(exp) VSASSERT(exp, "")
#undef ASSERT
#define ASSERT(exp) VSASSERT(exp, "")
#undef VERIFY
#define VERIFY(exp) VSASSERT(exp, "")

#define Output(exp) TDDebugOutput exp
#define Out(exp) TDOutString exp
#define AuxOut(exp) TDOutIfAux exp
#define DPrintf(exp) TDDebugPrintf exp
#define DBGOUT(exp) TDOutLine exp
#define DEBUG__(exp) exp

#else // DEBUG

#define Assert(exp) ((void)0)
#undef ASSERT
#define ASSERT(exp) ((void)0)
#undef VERIFY
#define VERIFY(exp) exp
#define Output(exp)
#define Out(exp)
#define AuxOut(exp)
#define DPrintf(exp)
#define DBGOUT(exp)
#define DEBUG__(exp)

#endif                              // ifdef DEBUG

#endif // _DBGOUT_INCLUDED
