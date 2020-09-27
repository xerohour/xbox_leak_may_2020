#ifndef _NV_DBG_H_
#define _NV_DBG_H_

 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/
/******************* Common Debug & Trace Defines ***************************\
*                                                                           *
* Module: NV_DBG.H                                                          *
*                                                                           *
\***************************************************************************/

#if defined(DEBUG) || defined(__DJGPP__) || defined(_XBOX)

//
// Types and defines
//
#include <nvtypes.h>

#ifndef VOID
#define VOID    void
#endif
#ifndef TRUE
#define TRUE    !0L
#endif
#ifndef FALSE
#define FALSE   0L
#endif

#if !defined(__DJGPP__) && !defined(_XBOX) && !defined(__GNUC__)
#ifndef va_list
#define va_list char*
#endif // va_list

#ifndef _INTSIZEOF
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#endif // _INTSIZEOF

#ifndef va_start
#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#endif // va_start

#ifndef va_arg
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#endif // va_arg

#ifndef va_end
#define va_end(ap)      ( ap = (va_list) 0 )
#endif // va_end
#else
#include <stdarg.h>   // define va_*
#endif // !__DJGPP__ && !_XBOX

//
// Debug Modules
//
#define DBG_MODULE_GLOBAL       0x0
#define DBG_MODULE_ARCH         0x1     // common/src/nv/nvapi.c
#define DBG_MODULE_OS           0x2     // arch/nvalloc/*
#define DBG_MODULE_DAC          0x3     // kernel/dac/*
#define DBG_MODULE_FIFO         0x4     // kernel/fifo/*
#define DBG_MODULE_GR           0x5     // kernel/gr/*
#define DBG_MODULE_HEAP         0x6     // kernel/heap/nv/heap.c
#define DBG_MODULE_VIDEO        0x7     // kernel/video/*
#define DBG_MODULE_MP           0x8     // kernel/mp/*
#define DBG_MODULE_DACCLASS     0x9     // kernel/modular/nv/class{46,56,67,7c).c
#define DBG_MODULE_VIDEOCLASS   0xA     // kernel/modular/nv/class{047,07a}.c
#define DBG_MODULE_POWER        0xB     // kernel/mc/nv/mcpower.c, mcnv*.c

//
// Debug Level values
//
#define DBG_LEVEL_INFO          0x0         // For informational debug trace info
#define DBG_LEVEL_SETUPINFO     0x1         // For informational debug setup info
#define DBG_LEVEL_USERERRORS    0x2         // For debug info on app level errors
#define DBG_LEVEL_WARNINGS      0x3         // For RM debug warning info
#define DBG_LEVEL_ERRORS        0x4         // For RM debug error info
#define DBG_LEVEL_DEFAULTS      0xFFFFFFFF  // All modules set to warning level

//
// These are the debug levels that were used before
//

#define DEBUGLEVEL_TRACEINFO    0       // For informational debug trace info
#define DEBUGLEVEL_SETUPINFO    1       // For informational debug setup info
#define DEBUGLEVEL_USERERRORS   2       // For debug info on app level errors
#define DEBUGLEVEL_WARNINGS     3       // For RM debug warning info
#define DEBUGLEVEL_ERRORS       4       // For RM debug error info


//
// Prototypes
//
VOID osDbgBreakPoint();
VOID osDbgPrintf_va(int module, int debuglevel, const char* s, va_list arglist);
VOID nvDbg_Printf(int module, int debuglevel, const char* s, ...);
VOID global_DbgPrintf(int debuglevel, const char* s, ...);
VOID arch_DbgPrintf(int debuglevel, const char* s, ...);
VOID os_DbgPrintf(int debuglevel, const char* s, ...);
VOID dac_DbgPrintf(int debuglevel, const char* s, ...);
VOID fifo_DbgPrintf(int debuglevel, const char* s, ...);
VOID gr_DbgPrintf(int debuglevel, const char* s, ...);
VOID heap_DbgPrintf(int debuglevel, const char* s, ...);
VOID video_DbgPrintf(int debuglevel, const char* s, ...);
VOID mp_DbgPrintf(int debuglevel, const char* s, ...);
VOID dacClass_DbgPrintf(int debuglevel, const char* s, ...);
VOID videoClass_DbgPrintf(int debuglevel, const char* s, ...);
VOID power_DbgPrintf(int debuglevel, const char* s, ...);


//
// Defines
//
#define DBG_BREAKPOINT()                        osDbgBreakPoint()
#define DBG_PRINTF(format_and_stuff)            nvDbg_Printf format_and_stuff
#define GLOBAL_PRINTF(format_and_stuff)         global_DbgPrintf format_and_stuff
#define ARCH_PRINTF(format_and_stuff)           arch_DbgPrintf format_and_stuff
#define OS_PRINTF(format_and_stuff)             os_DbgPrintf format_and_stuff
#define DAC_PRINTF(format_and_stuff)            dac_DbgPrintf format_and_stuff
#define FIFO_PRINTF(format_and_stuff)           fifo_DbgPrintf format_and_stuff
#define GR_PRINTF(format_and_stuff)             gr_DbgPrintf format_and_stuff
#define HEAP_PRINTF(format_and_stuff)           heap_DbgPrintf format_and_stuff
#define VIDEO_PRINTF(format_and_stuff)          video_DbgPrintf format_and_stuff
#define MP_PRINTF(format_and_stuff)             mp_DbgPrintf format_and_stuff
#define DACCLASS_PRINTF(format_and_stuff)       dacClass_DbgPrintf format_and_stuff  
#define VIDEOCLASS_PRINTF(format_and_stuff)     videoClass_DbgPrintf format_and_stuff
#define POWER_PRINTF(format_and_stuff)          power_DbgPrintf format_and_stuff


//
// Legacy Defines
//
#define DBG_PRINT_STRING(level,str)             nvDbg_Printf(DBG_MODULE_GLOBAL, level, "%s", str)
#define DBG_PRINT_CR(level,value)               nvDbg_Printf(DBG_MODULE_GLOBAL, level, "\n")
#define DBG_PRINT_VALUE(level,n)                nvDbg_Printf(DBG_MODULE_GLOBAL, level, "0x%x", n)
#define DBG_PRINT_STRING_VALUE(level,str,n)     nvDbg_Printf(DBG_MODULE_GLOBAL, level, "%s 0x%x\n", str, n)
#define DBG_PRINT_STRING_DEC(level,str,n)       nvDbg_Printf(DBG_MODULE_GLOBAL, level, "%s %d\n", str, n)
#define DBG_PRINT_STRING_PTR(level,str,ptr)     nvDbg_Printf(DBG_MODULE_GLOBAL, level, "%s 0x%p\n", str, ptr)

/* XXX hack */
#define DBG_PRINT_STRING_VAL64(level,str,v)     nvDbg_Printf(DBG_MODULE_GLOBAL, level, "%s 0x%x\n", (str), *(U032*) &(v))

#if (!defined(__DJGPP__) && !defined(_XBOX)) || defined(CHECK_HEAP)
#define DBG_VAL_PTR(p)                                                                          \
do {                                                                                            \
    U032 *ptr = (void *) (p);                                                                   \
    if (ptr[-1] != NV_MARKER1){                                                                 \
        DBG_PRINTF((DBG_MODULE_GLOBAL, DBG_LEVEL_ERRORS, "NVRM: Invalid pointer!\n"));          \
        DBG_PRINTF((DBG_MODULE_GLOBAL, DBG_LEVEL_ERRORS, "%s: line %d\n", __FILE__, __LINE__)); \
        DBG_BREAKPOINT();                                                                       \
    }                                                                                           \
    /* pick out size and check last word */                                                     \
    /* note using '/' to clear out V or K malloc type bits */                                   \
    else if (ptr[ptr[-2]/4 - 3] != NV_MARKER2) {                                                \
        DBG_PRINTF((DBG_MODULE_GLOBAL, DBG_LEVEL_ERRORS, "NVRM: Trashed buffer end!\n"));       \
        DBG_PRINTF((DBG_MODULE_GLOBAL, DBG_LEVEL_ERRORS, "%s: line %d\n", __FILE__, __LINE__)); \
        DBG_BREAKPOINT();                                                                       \
    }                                                                                           \
} while(0)
#else
#define DBG_VAL_PTR(p)                    
#endif // (!__DJGPP__ && !_XBOX) || CHECK_HEAP


//
// Prototypes
//
int nvDbgSprintf(U008 *dest, const U008 *fmt, va_list args);

// The following are utility routines used by nvDbgSprintf() to format different types
// of data values.  Most of the time you won't need to use them directly, but they're
// listed here for completeness.  Consult the code in debug.c for sprintf() for
// examples of how to use them.
int nvDbg_int32todecfmtstr(S032 s32val, U008 *dest, int fieldwidth, int flags);
int nvDbg_uint32tohexfmtstr(U032 u32val,  U008 *dest, int fieldwidth, int flags);
// int float64todecfmtstr(F064 f64val, U008 *dest, int fieldwidth, int precision, int flags);
int nvDbg_strtofmtstr(U008 *src, U008 *dest, int fieldwidth, int flags);

//
// Numeric & string conversion flags (used if you call the 'XtoYfmtstr' routines directly)
//
enum {
    DONTTERMINATE = 1,  // Don't null-terminate the string if this flag is set
    UNSIGNED_F = 2,     // Force an unsigned number conversion (other sign options are ignored)
    PLUSSIGN_F = 4,     // For signed numbers >= 0, force a '+' in the sign position
    SPACESIGN_F = 8,    // For signed numbers >= 0, force a space in the sign position
    LEFTALIGN_F = 16,   // Left-justify the result in the destination field (overrides zero fill)
    ZEROFILL_F = 32,    // Use leading zeros for padding to a field width
    LOWERCASE_F = 64    // Use lower case hex digits: a-f instead of A-F
};

#define MAX_ERROR_STRING 256

#else

//
// Define away these for non-debug
//
#define DBG_BREAKPOINT()
#define DBG_PRINTF(format_and_stuff)
#define DBG_PRINT_STRING(level,str)
#define DBG_PRINT_STRING_VALUE(level,str,n)
#define DBG_PRINT_CR(level,value)
#define DBG_PRINT_VALUE(level,n)
#define DBG_PRINT_STRING_DEC(level,str,n)
#define DBG_PRINT_STRING_PTR(level,str,ptr)
#define DBG_PRINT_STRING_VAL64(level,str,v)
#define DBG_VAL_PTR(p)                    

#define GLOBAL_PRINTF(format_and_stuff)
#define ARCH_PRINTF(format_and_stuff)
#define OS_PRINTF(format_and_stuff)
#define DAC_PRINTF(format_and_stuff)
#define FIFO_PRINTF(format_and_stuff)
#define GR_PRINTF(format_and_stuff)
#define HEAP_PRINTF(format_and_stuff)
#define VIDEO_PRINTF(format_and_stuff)
#define MP_PRINTF(format_and_stuff)
#define DACCLASS_PRINTF(format_and_stuff)
#define VIDEOCLASS_PRINTF(format_and_stuff)
#define POWER_PRINTF(format_and_stuff)

#endif // DEBUG || __DJGPP__ || _XBOX

//********************************************************************************
//
//  NVRM_TRACE support
//    low-overhead runtime state capture
//    to enable, define USE_NVRM_TRACE (retail or debug builds)
//
//********************************************************************************

#ifdef USE_NVRM_TRACE

U032 NVRM_TRACE_INIT(void);
U032 NVRM_TRACE_DISABLE(void);
void NVRM_TRACE_ENABLE(void);
void NVRM_TRACE_DUMP(void);
void NVRM_TRACE(U032);
void NVRM_TRACE1(U032);
void NVRM_TRACE2(U032, U032);
void NVRM_TRACE3(U032, U032, U032);
void NVRM_TRACE4(U032, U032, U032, U032);
void NVRM_TRACE5(U032, U032, U032, U032, U032);
// versions of reg read/write that log to trace buffer
U032 NVRM_TRACE_REG_RD32(PHWINFO, U032);
void NVRM_TRACE_REG_WR32(PHWINFO, U032, U032);

#else  // ! USE_NVRM_TRACE

#define NVRM_TRACE_INIT()
#define NVRM_TRACE_DISABLE() 0
#define NVRM_TRACE_ENABLE()
#define NVRM_TRACE_DUMP()
#define NVRM_TRACE(c0)
#define NVRM_TRACE1(c0)
#define NVRM_TRACE2(c0, c1)
#define NVRM_TRACE3(c0, c1, c2)
#define NVRM_TRACE4(c0, c1, c2, c3)
#define NVRM_TRACE5(c0, c1, c2, c3, c4)

#endif  // ! USE_NVRM_TRACE

#define NVRM_TRACE_ERROR(code, status)   NVRM_TRACE3('EEEE', (code), (status))
#define NVRM_TRACE_API(code, p0, p1, p2) NVRM_TRACE5('API ', (code), (p0), (p1), (p2))

#endif // _NV_DBG_H_
