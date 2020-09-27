// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* NV Common ******************************
//
//  Module: nvDbg.h
//      Shared debugging routines for printing, tracing, etc.
//
// **************************************************************************
//
//  History:
//      Matt Lavoie             18Nov00         extracted from d3d
//      Craig Duttweiler        21Nov00         major rehash
//
// **************************************************************************

#ifndef _NVDBG_H_
#define _NVDBG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "NvTypes.h"

//-------------------------------------------------------------------------
// GLOBAL VARIABLES
//-------------------------------------------------------------------------

#ifdef DEBUG

    extern  int       dbgTrace;   // function call tracing
    extern  NvU32     dbgLevel;   // DPF printing control
    extern  NvU32     dbgLog;     // log DPFs to a file (TRUE/FALSE)
    #define NVDBG_TRACE_EXIT 0x10000000  // prints '.' on function exit

#else

    #define dbgTrace  0
    #define dbgLevel  0
    #define dbgLog    0

#endif

//-------------------------------------------------------------------------
// DEFINTIONS
//-------------------------------------------------------------------------

// printing defaults

#define DPF_START_STR  "NV: "
#define DPF_END_STR    "\r\n"
#define DPF_LOG_FILE   "\\nv.log"

// tracing defaults

#define NV_TRACE_MAX_COLS  76
#define NV_TRACE_MAX_DEPTH 20
#define NV_TRACE_START_STR "TRACE: "

//-------------------------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------------------------

// debug printing

void __cdecl PF (char *szFormat, ...);      // Available in Debug and Non-Debug Builds

#ifdef DEBUG

    void __cdecl DPF             (char *szFormat, ...);
    void __cdecl DPF_PLAIN       (char *szFormat, ...);
    void __cdecl DPF_LEVEL       (NvU32 dwLevel, char * szFormat, ...);
    void __cdecl DPF_LEVEL_PLAIN (NvU32 dwLevel, char * szFormat, ...);

#else  // !DEBUG

    #define DPF                  1 ? (void)0 : (void)
    #define DPF_PLAIN            1 ? (void)0 : (void)
    #define DPF_LEVEL            1 ? (void)0 : (void)
    #define DPF_LEVEL_PLAIN      1 ? (void)0 : (void)

#endif  // !DEBUG

// tracing

#ifdef DEBUG

    void __cdecl dbgTracePush       (char *szFormat, ...);
    void __cdecl dbgTracePop        (void);
    void __cdecl dbgResetTraceLevel (void);

#elif INSTRUMENT_ALL_CALLS

    void __cdecl dbgTracePush       (char *szFormat, ...);
    void __cdecl dbgTracePop        (void);
    #define      dbgResetTraceLevel()

#else  // !DEBUG && !INSTRUMENT_ALL_CALLS

    #define      dbgTracePush       1 ? (void)0 : (void)
    #define      dbgTracePop()
    #define      dbgResetTraceLevel()

#endif  // !DEBUG && !INSTRUMENT_ALL_CALLS

#ifdef HAVE_NV_FILE_FACILITY
void __cdecl createDPFLog();
void __cdecl closeDPFLog();
#endif


//---------------------------------------------------------------------------
// ERROR HANDLING
//---------------------------------------------------------------------------

#ifdef DEBUG

    int _nvAssertReport(const char *pszExpr, const char *pszFile, const int nLine);

    // The do-while is there so that an nvAssert w/o a semi will not compile
    #define nvAssert(expr)                                                      \
    do {                                                                        \
        if (!(expr) && (_nvAssertReport(#expr, __FILE__, __LINE__)))    \
            _asm int 3                                                          \
    } while(0)

#else  // !DEBUG

    #define nvAssert(expr) ((void)0)

#endif  // !DEBUG

// override normal assert
#undef  assert
#define assert nvAssert

//---------------------------------------------------------------------------

#ifdef DEBUG

    #define dbgError(string)                                                    \
    {                                                                           \
        DPF (string);                                                           \
        _asm int 3                                                              \
    }

#else  // !DEBUG

    #define dbgError(a)

#endif  // !DEBUG

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVDBG_H_

