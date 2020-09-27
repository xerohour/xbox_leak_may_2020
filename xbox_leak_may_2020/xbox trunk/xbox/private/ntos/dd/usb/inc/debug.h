/*++

Copyright (c) 1990 Microsoft Corporation


Module Name:

    debug.h

Abstract:

    Debug related definitions and declarations used in uhcd, usbd, usbn, and usbh.

    The following compiler definitions are effective

    DBG - Debug function, only works in debug builds.
    RAISE_TODO_AND_BUGBUG - causes USB_TODO() and USB_BUGBUG statments to compile
    DBG_MAX     - Changes the default Traceout Level to Maximum (does NOT include RAISE_TODO_AND_BUGBUG)
    DBG_CALL    - Changes the default Traceout Level to include function entry and exit

Environment:

    XBOX kernel mode only

Notes:

Revision History:

    12-27-99 created by Mitchell Dernis (mitchd)

--*/

#ifndef DEBUG_H
#define DEBUG_H

#pragma warning(push, 4)
#pragma warning(disable:4244) //This warning seems to be broken.
#pragma warning(disable:4505) //XUSBDbg* functions should be discarded if not used
#include <xdbg.h>


/*
**  Definitions for compiler warnings.
**
*/
#define QUOTE0(a)  #a
#define QUOTE1(a)  QUOTE0(a)
#define MESSAGE(a) message(__FILE__ "(" QUOTE1(__LINE__) "): " a)
#define TODO(a)    MESSAGE("TODO: " a)
#define BUGBUG(a)  MESSAGE("BUGBUG: " a)

/*
**  Definitions for DEBUG BUILDS
**
**
*/
#if DBG

//
//  Declaration for debug module
//

#define DEFINE_USB_DEBUG_FUNCTIONS(Module) \
    static VOID XUSBDbgErr(PCHAR Format, ...) \
    { va_list args; va_start(args, Format); \
      vXDebugPrint(XDBG_ERROR, Module, Format, args); va_end(args); } \
    static VOID XUSBDbgWrn(PCHAR Format, ...) \
    { va_list args; va_start(args, Format); \
      vXDebugPrint(XDBG_WARNING, Module, Format, args); va_end(args); } \
    static VOID XUSBDbgTrc(PCHAR Format, ...) \
    { va_list args; va_start(args, Format); \
      vXDebugPrint(XDBG_TRACE, Module, Format, args); va_end(args); } \
    static VOID XUSBDbgEnt(PCHAR Format, ...) \
    { va_list args; va_start(args, Format); \
      vXDebugPrint(XDBG_ENTRY, Module, Format, args); va_end(args); } \
    static VOID XUSBDbgExt(PCHAR Format, ...) \
    { va_list args; va_start(args, Format); \
      vXDebugPrint(XDBG_EXIT, Module, Format, args); va_end(args); }

//
//  Conditional debug output procedures
//

#if (XDBG_COMPILE_LEVEL >= XDBG_ENTRY)
#define USB_DBG_ENTRY_PRINT(__x__) XUSBDbgEnt __x__;
#else
#define USB_DBG_ENTRY_PRINT(__x__)
#endif
    
#if (XDBG_COMPILE_LEVEL >= XDBG_EXIT)
#define USB_DBG_EXIT_PRINT(__x__) XUSBDbgExt __x__;
#else
#define USB_DBG_EXIT_PRINT(__x__)
#endif
    
#if (XDBG_COMPILE_LEVEL >= XDBG_WARNING)
#define USB_DBG_WARN_PRINT(__x__) XUSBDbgWrn __x__;
#else
#define USB_DBG_WARN_PRINT(__x__)
#endif

#ifdef PROMOTE_TRACE_TO_WARN
#define USB_DBG_TRACE_PRINT(__x__) XUSBDbgWrn __x__;
#else
#if (XDBG_COMPILE_LEVEL >= XDBG_TRACE)
#define USB_DBG_TRACE_PRINT(__x__) XUSBDbgTrc __x__;
#else
#define USB_DBG_TRACE_PRINT(__x__)
#endif
#endif
    
#if (XDBG_COMPILE_LEVEL >= XDBG_ERROR)
#define USB_DBG_ERROR_PRINT(__x__) XUSBDbgErr __x__;
#else
#define USB_DBG_ERROR_PRINT(__x__)
#endif
    
#define DBG_BREAK() DbgBreakPoint()

#undef  PAGED_CODE
#define PAGED_CODE() \
    if (KeGetCurrentIrql() > APC_LEVEL) \
    {\
        USB_DBG_CRITICAL_PRINT(("Pageable code called at IRQL %ld (file: %s, line:#%ld)\n", KeGetCurrentIrql(),__FILE__,__LINE__))\
        ASSERT(FALSE);\
    }

#define ASSERT_LESS_THAN_DISPATCH_LEVEL() \
    if (KeGetCurrentIrql() >= DISPATCH_LEVEL)   \
    {\
        DbgPrint("%s(%ld): Assertion that IRQL was below DISPATCH_LEVEL failed: IRQL %ld .\n", __FILE__, __LINE__, KeGetCurrentIrql());\
        DbgBreakPoint();\
    }

#define ASSERT_LESS_THAN_OR_EQUAL_DISPATCH_LEVEL() \
    if (KeGetCurrentIrql() > DISPATCH_LEVEL)    \
    {\
        DbgPrint("%s(%ld): Assertion that IRQL was less than or equal to DISPATCH_LEVEL failed: IRQL %ld .\n", __FILE__, __LINE__, KeGetCurrentIrql());\
        DbgBreakPoint();\
    }

#define ASSERT_PASSIVE_LEVEL()\
{\
    if(KeGetCurrentIrql() != PASSIVE_LEVEL)\
    {\
        DbgPrint("%s(%ld): Routine which must be called at PASSIVE_LEVEL was called at IRQL %ld .\n", __FILE__, __LINE__, KeGetCurrentIrql());\
        DbgBreakPoint();\
    }\
}

#define ASSERT_DISPATCH_LEVEL()\
{\
    if(KeGetCurrentIrql() != DISPATCH_LEVEL)\
    {\
        DbgPrint("%s(%ld): Routine which must be called at DISPATCH_LEVEL was called at IRQL %ld .\n", __FILE__, __LINE__, KeGetCurrentIrql());\
        DbgBreakPoint();\
    }\
}

#define USING_CASE_FALLTHROUGH_TRACE    ULONG macro_ulTraceoutSentAlready = FALSE;
#define START_CASE_FALLTHROUGH_TRACE    macro_ulTraceoutSentAlready = FALSE;
#define TRACEOUT_THIS_CASE_ONLY         if(!macro_ulTraceoutSentAlready && (macro_ulTraceoutSentAlready=TRUE) )



//#define RTL_ALLOCATE_HEAP(_size_) ExAllocatePoolWithTag(_size_,MODULE_POOL_TAG)
//#define RTL_FREE_HEAP(_block_) ExFreePool(_block_)

//
//  Beefed up allocate pool with traceout
//
#ifdef USB_TRACE_MEMORY_ALLOCATE_FREE
static PVOID pvAllocateTemp;
#define RTL_ALLOCATE_HEAP(_size_)\
    ( \
        (pvAllocateTemp = ExAllocatePoolWithTag((_size_),MODULE_POOL_TAG)),\
        DbgPrint( "%s(%d): RTL_ALLOCATE_HEAP(%d) returning 0x%0.8x\n",\
                    __FILE__,\
                    __LINE__,\
                    _size_,\
                    pvAllocateTemp\
        ),\
        pvAllocateTemp\
    )

#define RTL_FREE_HEAP(_block_)\
    (\
        DbgPrint("%s(%d): RTL_FREE_HEAP(0x%0.8x)\n", __FILE__, __LINE__, _block_),\
        ExFreePool(_block_)\
    )

#else  //not defined USB_TRACE_MEMORY_ALLOCATE_FREE

#define RTL_ALLOCATE_HEAP(_size_) ExAllocatePoolWithTag((_size_),MODULE_POOL_TAG)
#define RTL_FREE_HEAP(_block_) ExFreePool(_block_)

#endif //USB_TRACE_MEMORY_ALLOCATE_FREE

/*
**  Definitions for RELEASE builds
**
**
*/

#else       // DBG=0

#define USB_DBG_ENTRY_PRINT(__x__)
#define USB_DBG_EXIT_PRINT(__x__)
#define USB_DBG_TRACE_PRINT(__x__)
#define USB_DBG_WARN_PRINT(__x__)
#define USB_DBG_ERROR_PRINT(__x__)
#define DEFINE_USB_DEBUG_FUNCTIONS(Module)
#define DBG_BREAK()
#undef  PAGED_CODE
#define PAGED_CODE()
#define ASSERT_LESS_THAN_DISPATCH_LEVEL()
#define ASSERT_LESS_THAN_OR_EQUAL_DISPATCH_LEVEL()
#define ASSERT_DISPATCH_LEVEL()
#define ASSERT_PASSIVE_LEVEL()
#define DECLARE_MODULE_DEBUG_LEVEL(__x__)
#define SET_MODULE_DEBUG_LEVEL(__x__)
#define USING_CASE_FALLTHROUGH      
#define START_CASE_FALLTHROUGH_TRACE
#define TRACEOUT_THIS_CASE_ONLY     

#define EX_ALLOCATE_POOL(_size_) ExAllocatePool(_size_)
#define EX_FREE_POOL(_block_) ExFreePool(_block_)
#define RTL_ALLOCATE_HEAP(_size_) ExAllocatePool(_size_)
#define RTL_FREE_HEAP(_block_) ExFreePool(_block_)

#endif  // DBG=?

//===========================================================================
//  Profiling things
//===========================================================================
#ifdef PERFORM_PROFILING


//
//  Some macros for neatly adding profiling checks and traceouts
//  in the future we may choose to log these instead.
//
#define PROFILE_DECLARE_TIME_STAMP(_Timer_)\
            LARGE_INTEGER _Timer_;
#define PROFILE_BEGIN_TIMING(_Timer_)\
            _Timer_ = KeQueryPerformanceCounter();
#define PROFILE_END_TIMING(_Timer_)\
        {\
            LARGE_INTEGER _TempTimeDiff_;\
            LARGE_INTEGER _TempTimerFreq_;\
            ULONG         _TempTimeDiffUs_;\
            _TempTimeDiff_ = (_Timer_ - KeQueryPerformanceCounter(&_TempTimerFreq_);\
            _TempTimerFreq_.QuadPart /= 100000; /*Convert to ticks per us*/\
            _TempTimeDiffUs_ = (ULONG)(_TempTimeDiff_.QuadPart / _TempTimerFreq_.QuadPart);
            DbgPrint( USB_TRACE_NAME );\
            DbgPrint( ": " );\
            DbgPrint("\'%s\' took %d us.\n", #_Timer_, _TempTimeDiffUs_);\
        }


#else //PERFORM_PROFILING

//
//  These are all NOPs
//
#define PROFILE_DECLARE_TIME_STAMP(_Timer_)
#define PROFILE_BEGIN_TIMING(_Timer_)
#define PROFILE_END_TIMING(_Timer_)

#endif //PERFORM_PROFILING


//===========================================================================
//          End
//===========================================================================

#endif  // DEBUG_H
