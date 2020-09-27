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

#include "bldsetup.h"

#include "macros.h"


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

#include "windows.h"
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
    OSVERSIONINFO   osVersionInfo;
    
    pInfo = (PERFTIMERINFO*) wmvalloc(sizeof(PERFTIMERINFO));
    if (NULL == pInfo)
        return NULL;
    
    pInfo->fFirstTime = WMAB_TRUE;
    pInfo->cDecodeTime.QuadPart = 0;
    pInfo->cTotalDecLoopTime.QuadPart  = 0;
    pInfo->cDecodeStart.QuadPart  = 0;
    pInfo->lSamplesPerSec = lSamplesPerSecOutput;
    pInfo->cPlaybackTime.QuadPart  = 0;
    
    // Determine if we are running on NT
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    // Default: Not NT
    pInfo->bIsWindowsNT = WMAB_FALSE;

#if !defined(USE_QUERY_TIMER)
    if (GetVersionEx(&osVersionInfo)) 
        pInfo->bIsWindowsNT = (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
#endif // USE_QUERY_TIMER
    
    if (pInfo->bIsWindowsNT) {
        pInfo->cPerfFrequency.QuadPart = 10000000; // Ticks are hard coded to be 100ns
    } else {
        fResult = QueryPerformanceFrequency(&pInfo->cPerfFrequency);
        if (WMAB_FALSE == fResult)
            pInfo->cPerfFrequency.QuadPart = -1;
    }
    
    return pInfo;
}

void PerfTimerStart(PERFTIMERINFO *pInfo)
{
    Bool     fResult;
    
#ifndef UNDER_CE
    if (pInfo->bIsWindowsNT) 
    {
        // Measures user time. Aggregate of all threads in the process. 
        // Should we measure wall clock time? Should we add kernel time
        // to the user time?
        // Get pseudo handle of the current process. No need to close
        // pseudo-handles.
        HANDLE   hProcess;
        FILETIME ftCreationTime, ftExitTime, ftKernelTime, ftUserTime;
        hProcess = GetCurrentProcess();
        fResult = GetProcessTimes(hProcess, &ftCreationTime, 
            &ftExitTime, &ftKernelTime, &ftUserTime);
        if (WMAB_FALSE == fResult) 
            pInfo->cDecodeStart.QuadPart = 0;
        else
        {
            pInfo->cDecodeStart.LowPart = ftUserTime.dwLowDateTime;
            pInfo->cDecodeStart.HighPart = ftUserTime.dwHighDateTime;
        }
    } 
    else 
    {
        fResult = QueryPerformanceCounter(&pInfo->cDecodeStart);
        if (WMAB_FALSE == fResult)
            pInfo->cDecodeStart.QuadPart = 0;
    }
#else   // UNDER_CE
    fResult = QueryPerformanceCounter(&pInfo->cDecodeStart);
    if (WMAB_FALSE == fResult)
        pInfo->cDecodeStart.QuadPart = 0;
#endif  // UNDER_CE
    
    if (pInfo->fFirstTime)
    {
        pInfo->cTotalDecLoopTime.QuadPart = pInfo->cDecodeStart.QuadPart;
        pInfo->fFirstTime = WMAB_FALSE;
    }
}

void PerfTimerStop(PERFTIMERINFO *pInfo, long lSamplesDecoded)
{
    LARGE_INTEGER   cEndTime;
    Bool            fResult;
    
#ifndef UNDER_CE
    if (pInfo->bIsWindowsNT) 
    { 
        // Measures user time. Aggregate of all threads in the process
        // Should we measure wall clock time? Should we add kernel time
        // to the user time?
        // Get pseudo handle of the current process. No need to close
        // pseudo-handles.
        HANDLE          hProcess;
        FILETIME        ftCreationTime, ftExitTime, ftKernelTime, ftUserTime;
        hProcess = GetCurrentProcess();
        fResult = GetProcessTimes(hProcess, &ftCreationTime, 
            &ftExitTime, &ftKernelTime, &ftUserTime);
        if (WMAB_FALSE == fResult) 
            cEndTime.QuadPart = 0;
        else
        {
            cEndTime.LowPart = ftUserTime.dwLowDateTime;
            cEndTime.HighPart = ftUserTime.dwHighDateTime;
        }
    }
    else 
    {
        fResult = QueryPerformanceCounter(&cEndTime);
        if (WMAB_FALSE == fResult)
            cEndTime.QuadPart = 0;
    }
#else   // UNDER_CE
    fResult = QueryPerformanceCounter(&cEndTime);
    if (WMAB_FALSE == fResult)
        cEndTime.QuadPart = 0;
#endif  // UNDER_CE
    
    pInfo->cDecodeTime.QuadPart += cEndTime.QuadPart -
        pInfo->cDecodeStart.QuadPart;
    
    // Record output playback time from this decode call, in clock() ticks
    pInfo->cPlaybackTime.QuadPart += lSamplesDecoded *
        pInfo->cPerfFrequency.QuadPart / pInfo->lSamplesPerSec;
}

void PerfTimerStopElapsed(PERFTIMERINFO *pInfo)
{
    LARGE_INTEGER   cEndElapsedTime;
    Bool            fResult;
    
#ifndef UNDER_CE
    if (pInfo->bIsWindowsNT) 
    { 
        // Measures user time. Aggregate of all threads in the process
        // Should we measure wall clock time? Should we add kernel time
        // to the user time?
        // Get pseudo handle of the current process. No need to close
        // pseudo-handles.
        HANDLE          hProcess;
        FILETIME        ftCreationTime, ftExitTime, ftKernelTime, ftUserTime;
        hProcess = GetCurrentProcess();
        fResult = GetProcessTimes(hProcess, &ftCreationTime, 
            &ftExitTime, &ftKernelTime, &ftUserTime);
        if (WMAB_FALSE == fResult) 
            cEndElapsedTime.QuadPart = 0;
        else
        {
            cEndElapsedTime.LowPart = ftUserTime.dwLowDateTime;
            cEndElapsedTime.HighPart = ftUserTime.dwHighDateTime;
        }
    }
    else 
    {
        fResult = QueryPerformanceCounter(&cEndElapsedTime);
        if (WMAB_FALSE == fResult)
            cEndElapsedTime.QuadPart = 0;
    }
#else   // UNDER_CE
    fResult = QueryPerformanceCounter(&cEndElapsedTime);
    if (WMAB_FALSE == fResult)
        cEndElapsedTime.QuadPart = 0;
#endif  // UNDER_CE
    pInfo->cTotalDecLoopTime.QuadPart = cEndElapsedTime.QuadPart -
        pInfo->cTotalDecLoopTime.QuadPart;
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
    wmvfree(pInfo);
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

WMARESULT MyOutputDbgStr(WMARESULT wmaReturn, const char *pszFmt, ...)
{
    TCHAR   sz[512];
    va_list vargs;
#if defined(_UNICODE) || defined(UNICODE)
    WCHAR wcsFmt[512];
    const WCHAR* pszFormat = wcsFmt;
    mbstowcs( wcsFmt, pszFmt, strlen(pszFmt)+1 );
#else
    const char* pszFormat = pszFmt;
#endif
    
    va_start(vargs, pszFmt);
    wvsprintf(sz, pszFormat, vargs);
    va_end(vargs);
    
    OUTPUT_DEBUG_STRING(sz);
    return wmaReturn;
}
#endif  // defined(PLATFORM_SPECIFIC_DEBUGINFRASTRUCTURE) && defined(_DEBUG)



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

