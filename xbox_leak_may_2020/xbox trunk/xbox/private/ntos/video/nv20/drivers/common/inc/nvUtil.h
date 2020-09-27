 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
 /***************************************************************************\
|*                                                                           *|
|*                       General nVidia Utilties                             *|
|*                                                                           *|
|* Includes a variety of miscelaneous but commonly useful functions & macros *|
|*                                                                           *|
|* This file is OS independent.  Use ONLY nVidia types and names.            *|
|*                                [eg. No DWORD, use NvU32]                  *|
|*                                                                           *|
 \***************************************************************************/

#ifndef _NVUTIL_H_
#define _NVUTIL_H_

#include "nvTypes.h"
#include <StdArg.h>

#ifdef __cplusplus
    #define EXTERN_C    extern "C"
#else
    #define EXTERN_C    extern
#endif

//---------------------------------------------------------------------------
//
//  Common definitions
//
//---------------------------------------------------------------------------

#ifndef ABS
#define ABS(a)  (a>0?a:-a)
#endif
#ifndef MIN
#define MIN(x,y) (((x)<(y))?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef VOID
#define VOID            void
#endif
#ifndef TRUE
#define TRUE            ~0
#endif
#ifndef FALSE
#define FALSE           0
#endif
#ifndef NULL
#define NULL            0L
#endif

// ================================================================
// floating point bit munging
// ================================================================

// convert an FP representation into a DWORD or vice-versa
#define DWORD_FROM_FLOAT(fp) (*(NvU32 *)&(fp))
#define FLOAT_FROM_DWORD(dw) (*(float *)&(dw))

#define FP_SIGN_BIT(fp) (DWORD_FROM_FLOAT(fp)&0x80000000)
#define FP_ABS_BITS(fp) (DWORD_FROM_FLOAT(fp)&0x7FFFFFFF)
#define FP_EXP_BITS(fp) (DWORD_FROM_FLOAT(fp)&0x7F800000)
#define FP_MAN_BITS(fp) (DWORD_FROM_FLOAT(fp)&0x007FFFFF)

#define FP_EXP_VAL(fp)  (FP_EXP_BITS(fp) >> 27)
#define FP_MAN_VAL(fp)  (FP_MAN_BITS(fp) >>  0)

#define FP_ONE_BITS          0x3F800000
#define FP_INFINITY_BITS     0x7F800000
#define FP_MIN_INFINITY_BITS 0xFF800000

// ================================================================
// Fun with Powers of 2
// ================================================================

// returns the index of the least significant bit = log2(x) if x=2^n
#define ASM_LOG2(x)     __asm mov eax,[x]   __asm bsf ecx,eax   __asm mov [x],ecx
// returns the index of the most significant bit = log2(x) if x=2^n
#define ASM_LOG2_R(x)   __asm mov eax,[x]   __asm bsr ecx,eax   __asm mov [x],ecx

// ================================================================
// Our very own string utils
// ================================================================
// NV string routines necessary because the analogs don't exist under NT. ugh.

EXTERN_C int   __cdecl nvStrCmp     (char *szStr1, char *szStr2);
EXTERN_C int   __cdecl nvStrLen     (char *szStr);
EXTERN_C int   __cdecl nvStrNLen    (char *szStr, int n);
EXTERN_C void  __cdecl nvStrCpy     (char *szDst, char *szSrc);
EXTERN_C void  __cdecl nvStrNCpy    (char *szDst, char *szSrc, int n);
EXTERN_C void  __cdecl nvStrCat     (char *szStr1, char *szStr2);
EXTERN_C char* __cdecl nvStrChr     (char *szStr, NvU8 c);
EXTERN_C char* __cdecl nvStrRChr    (char *szStr, NvU8 c);
EXTERN_C void  __cdecl nvSprintfVAL (char *szDest, char *szFormat, va_list vaArgs);
EXTERN_C void  __cdecl nvSprintf    (char *szDest, char *szFormat, ...);

EXTERN_C NvU32 nvGetCurrentProcessId();
EXTERN_C void  nvQueryPerformanceCounter(__int64  *pPerformanceCount);

// This is the same as nvDelay, but takes a parameter instead of referencing a global
EXTERN_C void nvSpin(NvU32 loopCount);

#endif  // !_NVUTIL_H_

