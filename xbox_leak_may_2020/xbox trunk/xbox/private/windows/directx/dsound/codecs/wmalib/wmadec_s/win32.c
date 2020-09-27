/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    Win32.c

Abstract:

    Win32-specific functions.

Author:

    Raymond Cheng (raych)       Aug 25, 1999

Revision History:
  
    Thumpudi Naveen (tnaveen)   May 9, 2000    Added NT specific timer.

*************************************************************************/

#include "..\common\macros.h"
#include "..\..\..\dsound\dsndver.h"


// Only build these functions for WIN32
#ifdef WMA_OSTARGET_WIN32

// **************************************************************************
// COMMON FUNCTIONS
// **************************************************************************

#ifdef WMA_TARGET_ANSI
#include "ansiwin.h"
#else   // WMA_TARGET_ANSI

#ifndef _WIN32
#define _WIN32  // We need this when including windows.h
#endif  // !_WIN32

#include "xtl.h"
#endif  // WMA_TARGET_ANSI


#ifdef PLATFORM_SPECIFIC_OUTPUTDEBUGSTRING
#define OUTPUT_DEBUG_STRING(sz) OutputDebugString(sz)
#endif  // PLATFORM_SPECIFIC_OUTPUTDEBUGSTRING

#ifdef UNICODE
#define SPRINTF swprintf
#else   // UNICODE
#define SPRINTF sprintf
#endif  // UNICODE

#ifdef PLATFORM_SPECIFIC_PERFTIMER

#include <stdio.h>

typedef struct tagPERFTIMERINFO
{
    int             fFirstTime;         // Used to record total time spent in decode loop
    LARGE_INTEGER   cDecodeTime;        // Time spent decoding only (running total)
    LARGE_INTEGER   cTotalDecLoopTime;  // Total time spent in decode loop
    LARGE_INTEGER   cDecodeStart;       // Could be easily optimized out but it's not worth it
    long            lSamplesPerSec;     // Samples per second, counting all channels
    LARGE_INTEGER   cPlaybackTime;      // Time required for playback (running total)
    LARGE_INTEGER   cPerfFrequency;     // Frequency of PerformanceCounter
    
    Bool            bIsWindowsNT;       // GetProcessTime available
} PERFTIMERINFO;


PERFTIMERINFO *PerfTimerNew(long lSamplesPerSecOutput)
{
    PERFTIMERINFO  *pInfo;
    Bool            fResult;
    
    pInfo = (PERFTIMERINFO*) malloc(sizeof(PERFTIMERINFO));
    if (NULL == pInfo)
        return NULL;
    
    pInfo->fFirstTime = WMAB_TRUE;
    pInfo->cDecodeTime.QuadPart = 0;
    pInfo->cTotalDecLoopTime.QuadPart  = 0;
    pInfo->cDecodeStart.QuadPart  = 0;
    pInfo->lSamplesPerSec = lSamplesPerSecOutput;
    pInfo->cPlaybackTime.QuadPart  = 0;
    
    pInfo->bIsWindowsNT = WMAB_TRUE;
    pInfo->cPerfFrequency.QuadPart = 10000000; // Ticks are hard coded to be 100ns
    
    return pInfo;
}

void PerfTimerStart(PERFTIMERINFO *pInfo)
{
    Bool     fResult;
    
    //
    // georgioc - removed from xbox
    //


}

void PerfTimerStop(PERFTIMERINFO *pInfo, long lSamplesDecoded)
{
    LARGE_INTEGER   cEndTime;
    Bool            fResult;
    
    //
    // georgioc - removed from xbox
    //

}

void PerfTimerStopElapsed(PERFTIMERINFO *pInfo)
{
    LARGE_INTEGER   cEndElapsedTime;
    Bool            fResult;
    
    //
    // georgioc - removed from xbox
    //

}

void PerfTimerReport(PERFTIMERINFO *pInfo)
{
    TCHAR           sz[256];
    float           fltDecodeTime;
    float           fltEntireDecodeTime;
    float           fltDecodeTimeFraction;
    float           fltPlaybackTime;
    
    if (pInfo->bIsWindowsNT) {
        SPRINTF(sz, TEXT("\n** Reporting aggregate of user time in all threads (NT)\n"));
        OUTPUT_DEBUG_STRING(sz);
    }
    
    SPRINTF(sz, TEXT("\n\n** Ticks per second (clock resolution): %f.\n"),
        (float)pInfo->cPerfFrequency.QuadPart);
    OUTPUT_DEBUG_STRING(sz);
    
    fltDecodeTime = (float) pInfo->cDecodeTime.QuadPart /
        (float) pInfo->cPerfFrequency.QuadPart;
    fltEntireDecodeTime = (float) pInfo->cTotalDecLoopTime.QuadPart /
        (float) pInfo->cPerfFrequency.QuadPart;
    SPRINTF(sz, TEXT("** Decode time: %f sec. Entire decode time: %f sec.\n"),
        fltDecodeTime, fltEntireDecodeTime);
    OUTPUT_DEBUG_STRING(sz);
    
    fltPlaybackTime = (float) pInfo->cPlaybackTime.QuadPart /
        (float) pInfo->cPerfFrequency.QuadPart;
    SPRINTF(sz, TEXT("** Playback time : %f sec.\n"),
        fltPlaybackTime);
    OUTPUT_DEBUG_STRING(sz);
    
    fltDecodeTimeFraction = (float)pInfo->cDecodeTime.QuadPart /
        (float) pInfo->cPlaybackTime.QuadPart;
    SPRINTF(sz, TEXT("** Percentage of playback time spent decoding: %f%%.\n"),
        fltDecodeTimeFraction * 100);
    OUTPUT_DEBUG_STRING(sz);
#       if defined(WMA_MONITOR)
    fprintf(stderr,"%s\n",sz);
#       endif
    
    SPRINTF(sz, TEXT("** Minimum MHz for realtime playback: %f of current CPU speed.\n"),
        fltDecodeTimeFraction);
    OUTPUT_DEBUG_STRING(sz);
    
    SPRINTF(sz, TEXT("** This CPU is %f times faster than required.\n\n"),
        (float)1.0 / fltDecodeTimeFraction);
    OUTPUT_DEBUG_STRING(sz);
    
#ifdef WRITE_TIME_TO_FILE
    {
        FILE*   fOut;
        
        fOut = fopen("time.txt", "wt");
        if (fOut != NULL) 
        { 
            fprintf(fOut, "%f\n", (float) fltDecodeTime);
            fclose(fOut);
            fOut = NULL;
        }
    }
#endif  //WRITE_TIME_TO_FILE
    
}


float fltPerfTimerDecodeTime(PERFTIMERINFO *pInfo)
{
    return (float) pInfo->cDecodeTime.QuadPart /
        (float) pInfo->cPerfFrequency.QuadPart;
}


void PerfTimerFree(PERFTIMERINFO *pInfo)
{
    free(pInfo);
}

#endif  // PLATFORM_SPECIFIC_PERFTIMER



//---------------------------------------------------------------------------
// Debug Infrastructure
//---------------------------------------------------------------------------
#if defined(PLATFORM_SPECIFIC_DEBUGINFRASTRUCTURE) && defined(_DEBUG)
#include <stdarg.h>
void TraceInfoHelper(const char *pszFile, int iLine, const char *pszFmt, ...)
{
    char    sz[512];
    va_list vargs;
    Int     iCharsWritten;

    // Prepend file/line identifier and "*** INFO ***" to get attention
    iCharsWritten = sprintf(sz, "%s(%d) : *** INFO *** ", pszFile, iLine);

    va_start(vargs, pszFmt);
    iCharsWritten += vsprintf(sz + iCharsWritten, pszFmt, vargs);
    va_end(vargs);

    // Append a newline and print it!
    sprintf(sz + iCharsWritten, "\n");
#if defined(_UNICODE) || defined(UNICODE)
    // let this do the string to unicode conversion
    MyOutputDbgStr(0,sz);
#else
    OUTPUT_DEBUG_STRING(sz);
#endif
}

#endif  // defined(PLATFORM_SPECIFIC_DEBUGINFRASTRUCTURE) && defined(_DEBUG)




// **************************************************************************
// ENCODER FUNCTIONS
// **************************************************************************
#ifdef ENCODER

// Following functions are for non-serialized code only
#ifndef WMA_SERIALIZE
#include "msaudioenc.h"

// Defines
#define SAFE_CLOSEHANDLE(x) if((x)) { CloseHandle((x));  (x) = NULL; }


void threadFuncEnc0(DWORD threadParam);
void threadFuncEnc1(DWORD threadParam);


void osInitThreadInfo(THREADINFO *pInfo)
{
    pInfo->m_hEncEvent0 = NULL;
    pInfo->m_hEncDone0 = NULL;
    pInfo->m_hEncEvent1 = NULL;
    pInfo->m_hEncDone1 = NULL;
    pInfo->m_hThreadEnc0 = NULL;
    pInfo->m_hThreadEnc1 = NULL;
    
    pInfo->m_bShutdownThreads = WMAB_FALSE;
    pInfo->m_fMultiProc = WMAB_FALSE;
    pInfo->m_pauenc = NULL;
}


void osCloseThreads(THREADINFO *pInfo)
{
    pInfo->m_bShutdownThreads = WMAB_TRUE;
    
    // signal all threads to stop...
    if (pInfo->m_hEncEvent0)  
        SetEvent (pInfo->m_hEncEvent0);
    if (pInfo->m_hEncEvent1)  
        SetEvent (pInfo->m_hEncEvent1);
    
    if (pInfo->m_hThreadEnc0)  
        WaitForSingleObject (pInfo->m_hThreadEnc0, INFINITE);
    if (pInfo->m_hThreadEnc1)  
        WaitForSingleObject (pInfo->m_hThreadEnc1, INFINITE);
    
    // clean up handles...
    
    SAFE_CLOSEHANDLE(pInfo->m_hThreadEnc0);
    SAFE_CLOSEHANDLE(pInfo->m_hEncEvent0);
    SAFE_CLOSEHANDLE(pInfo->m_hEncDone0);
    SAFE_CLOSEHANDLE(pInfo->m_hThreadEnc1);
    SAFE_CLOSEHANDLE(pInfo->m_hEncEvent1);
    SAFE_CLOSEHANDLE(pInfo->m_hEncDone1);
}

WMARESULT osInitThreads(CAudioObjectEncoder *pauenc)
{
    U32 uiThredEnc0, uiThredEnc1;
    
    THREADINFO *pInfo = &pauenc->m_rThreadInfo;
    SYSTEM_INFO rSysInfo;
    int iPriority = GetThreadPriority(GetCurrentThread());
    
    // Running dual-threaded on a single-proc machine is 7.4% slower than single-threaded
    pInfo->m_pauenc = pauenc;
    GetSystemInfo(&rSysInfo);
    if (rSysInfo.dwNumberOfProcessors == 1)
    {
        assert(WMAB_FALSE == pInfo->m_fMultiProc);
        return WMA_OK;
    }
    else
        pInfo->m_fMultiProc = WMAB_TRUE;
    
    pInfo->m_hEncEvent0 = CreateEvent(NULL, WMAB_FALSE, WMAB_FALSE, NULL);
    pInfo->m_hEncDone0 = CreateEvent(NULL, WMAB_FALSE, WMAB_FALSE, NULL);
    pInfo->m_hEncEvent1 = CreateEvent(NULL, WMAB_FALSE, WMAB_FALSE, NULL);
    pInfo->m_hEncDone1 = CreateEvent(NULL, WMAB_FALSE, WMAB_FALSE, NULL);
    
    pInfo->m_hThreadEnc0 = CreateThread (
        NULL,
        0,
        (LPTHREAD_START_ROUTINE) threadFuncEnc0,
        (LPVOID) pauenc,
        0,
        &uiThredEnc0
        );
    if (pInfo->m_hThreadEnc0 == NULL) 
        goto lerror;
    SetThreadPriority(pInfo->m_hThreadEnc0, iPriority);
    
    pInfo->m_hThreadEnc1 = CreateThread (
        NULL,
        0,
        (LPTHREAD_START_ROUTINE) threadFuncEnc1,
        (LPVOID) pauenc,
        0,
        &uiThredEnc1
        );
    if (pInfo->m_hThreadEnc1 == NULL) 
        goto lerror;
    SetThreadPriority(pInfo->m_hThreadEnc1, iPriority);
    
    return WMA_OK;
    
lerror:
    osCloseThreads (pInfo);
    return TraceResult(WMA_E_FAIL);
}

void threadFuncEnc0 (DWORD threadParam)
{
    CAudioObjectEncoder* pauenc = (CAudioObjectEncoder*) threadParam;

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)
    // Initialize Intel FFT for this thread
    if (pauenc->pau->m_fIntelFFT)
    {
        WMARESULT   wmaResult;

        wmaResult = auInitIntelFFT(pauenc->pau, INTELFFT_INIT_THREAD);
        TraceError(wmaResult);
    }
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)

    for (;;) {

        while (WaitForSingleObject (pauenc->m_rThreadInfo.m_hEncEvent0,
            INFINITE) != WAIT_OBJECT_0);

        if (pauenc->m_rThreadInfo.m_bShutdownThreads)
            break;

        switch (pauenc->tEncStatus) {
        case PROCESS_A:
            prvProcess0A (pauenc);
            break;
        case PROCESS_B:
            prvProcess0B (pauenc);
            break;
        case PROCESS_C:
            prvProcess0C (pauenc);
            break;
        case QUANTIZE_ENTROPY:
            prvQuantizeAndEntropy0 (pauenc);
            break;
        case SEND_STREAM:
            prvSendStream0 (pauenc);
            break;
        case TRANSIENT_DETECTION:
            prvDetectTransientChannel (pauenc, 0);
            break;
        }
        SetEvent (pauenc->m_rThreadInfo.m_hEncDone0);
    }

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)
    // Free Intel FFT for this thread
    if (pauenc->pau->m_fIntelFFT)
    {
        WMARESULT   wmaResult;

        wmaResult = auFreeIntelFFT(pauenc->pau, INTELFFT_FREE_THREAD);
        TraceError(wmaResult);
    }
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)

}

void threadFuncEnc1 (DWORD threadParam)
{
    CAudioObjectEncoder* pauenc = (CAudioObjectEncoder*) threadParam;

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)
    // Initialize Intel FFT for this thread
    if (pauenc->pau->m_fIntelFFT)
    {
        WMARESULT   wmaResult;

        wmaResult = auInitIntelFFT(pauenc->pau, INTELFFT_INIT_THREAD);
        TraceError(wmaResult);
    }
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)

    for (;;) {
        while (WaitForSingleObject (pauenc->m_rThreadInfo.m_hEncEvent1,
            INFINITE) != WAIT_OBJECT_0);

        if (pauenc->m_rThreadInfo.m_bShutdownThreads)
            break;

        switch (pauenc->tEncStatus) {
        case PROCESS_A:
            prvProcess1A (pauenc);
            break;
        case PROCESS_B:
            prvProcess1B (pauenc);
            break;
        case PROCESS_C:
            prvProcess1C (pauenc);
            break;
        case QUANTIZE_ENTROPY:
            prvQuantizeAndEntropy1 (pauenc);
            break;
        case SEND_STREAM:
            prvSendStream1 (pauenc);
            break;
        case TRANSIENT_DETECTION:
            prvDetectTransientChannel (pauenc, 1);
            break;
        }
        SetEvent (pauenc->m_rThreadInfo.m_hEncDone1);
    }

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)
    // Free Intel FFT for this thread
    if (pauenc->pau->m_fIntelFFT)
    {
        WMARESULT   wmaResult;

        wmaResult = auFreeIntelFFT(pauenc->pau, INTELFFT_FREE_THREAD);
        TraceError(wmaResult);
    }
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)

}


void osThreadEncAll (THREADINFO *pInfo)
{
    if (pInfo->m_fMultiProc)
    {
        SetEvent (pInfo->m_hEncEvent0);
        SetEvent (pInfo->m_hEncEvent1);
        
        WaitForSingleObject (pInfo->m_hEncDone0, INFINITE);
        WaitForSingleObject (pInfo->m_hEncDone1, INFINITE);
    }
    else
    {
        CAudioObjectEncoder *pauenc = pInfo->m_pauenc;
        
        switch (pauenc->tEncStatus)
        {
        case PROCESS:
            prvProcess0 (pauenc);
            prvProcess1 (pauenc);
            break;

        case PROCESS_A:
            prvProcess0A (pauenc);
            prvProcess1A (pauenc);
            break;

        case PROCESS_B:
            prvProcess0B (pauenc);
            prvProcess1B (pauenc);
            break;

        case PROCESS_C:
            prvProcess0C (pauenc);
            prvProcess1C (pauenc);
            break;
            
        case QUANTIZE_ENTROPY:
            prvQuantizeAndEntropy0 (pauenc);
            prvQuantizeAndEntropy1 (pauenc);
            break;
            
        case SEND_STREAM:
            prvSendStream0 (pauenc);
            prvSendStream1 (pauenc);
            break;
            
        case TRANSIENT_DETECTION:
            prvDetectTransientChannel (pauenc, 0);
            prvDetectTransientChannel (pauenc, 1);
            break;
            
        default:
            assert(WMAB_FALSE);
            break;
        } // switch
        
    } // else
    
} // osThreadEncAll

#else
// We need a placeholder because otherwise MSVC complains about error
// C2183 (the source file is empty after preprocessing).
static void fooWin32(void);
#endif  //!WMA_SERIALIZE
#endif  // ENCODER

#ifdef UNDER_CE
#	ifndef _CLOCK_T_DEFINED
		typedef long clock_t;
#		define _CLOCK_T_DEFINED
#	endif

	clock_t __cdecl clock(void) { 
		return (clock_t)GetTickCount(); 
	}

#	ifndef _TIME_T_DEFINED
		typedef long time_t;        /* time value */
#		define _TIME_T_DEFINED     /* avoid multiple def's of time_t */
#	endif

	time_t __cdecl time(time_t * pt) {
		SYSTEMTIME stNowTime;
		SYSTEMTIME st1970 = { 1970, 1, 0, 1, 0, 0, 0, 0 };
		FILETIME   ftNowTime, ft1970;
		ULARGE_INTEGER ulNowTime, ul1970;
		time_t tNowTime;	// seconds since 1/1/1970
		GetSystemTime( &stNowTime );
		if ( SystemTimeToFileTime( &stNowTime, &ftNowTime ) && SystemTimeToFileTime( &st1970, &ft1970 ) )
		{
			ulNowTime.HighPart = ftNowTime.dwHighDateTime;
			ulNowTime.LowPart  = ftNowTime.dwLowDateTime;
			ul1970.HighPart    = ft1970.dwHighDateTime;
			ul1970.LowPart     = ft1970.dwLowDateTime;
			tNowTime = (time_t)( (ulNowTime.QuadPart - ul1970.QuadPart)/10000000 );
		} else 
		{
			tNowTime = (time_t)clock();
		}
		if ( pt != NULL )
			*pt = tNowTime;
		return tNowTime;
	}
#endif


#endif  // WMA_OSTARGET_WIN32

