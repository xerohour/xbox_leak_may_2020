/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    WmaOS.c

Abstract:

    Functions that should work under any generic OS.

Author:

    Raymond Cheng (raych)       September 27, 1999

Revision History:


*************************************************************************/

#include "bldsetup.h"

#include "msaudio.h"
#include "macros.h"
#include "AutoProfile.h"
#include <stdio.h>
#if defined(_DEBUG) && defined(WMA_MONITOR)
#	include <math.h>
#endif
#if defined(HEAP_DEBUG_TEST) && defined(_DEBUG)
#include <malloc.h>
#endif
#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
#include <stdarg.h>
#endif

#ifndef PLATFORM_SPECIFIC_PERFTIMER

#include <time.h>

typedef struct tagPERFTIMERINFO
{
    int     fFirstTime;         // Used to record total time spent in decode loop
    clock_t cDecodeTime;        // Time spent decoding only (running total)
    clock_t cTotalDecLoopTime;  // Total time spent in decode loop
    clock_t cDecodeStart;       // Could be easily optimized out but it's not worth it
    long    lSamplesPerSec;     // Samples per second, counting all channels
    long    lPlaybackTime;      // Time required for playback (running total)
} PERFTIMERINFO;

PERFTIMERINFO *PerfTimerNew(long lSamplesPerSecOutput)
{
    PERFTIMERINFO  *pInfo;

    pInfo = (PERFTIMERINFO*)(wmvalloc(sizeof(PERFTIMERINFO)));
    if (NULL == pInfo)
        return NULL;

    pInfo->fFirstTime = WMAB_TRUE;
    pInfo->cDecodeTime = 0;
    pInfo->cTotalDecLoopTime = 0;
    pInfo->cDecodeStart = 0;
    pInfo->lSamplesPerSec = lSamplesPerSecOutput;
    pInfo->lPlaybackTime = 0;

    return pInfo;
}

void PerfTimerStart(PERFTIMERINFO *pInfo)
{
    pInfo->cDecodeStart = clock();

    if (pInfo->fFirstTime)
    {
        pInfo->cTotalDecLoopTime = pInfo->cDecodeStart;
        pInfo->fFirstTime = WMAB_FALSE;
    }
}

void PerfTimerStop(PERFTIMERINFO *pInfo, long lSamplesDecoded)
{
    clock_t cDecodeTime;

    cDecodeTime = clock() - pInfo->cDecodeStart;
    pInfo->cDecodeTime += cDecodeTime;

    // Record output playback time from this decode call, in clock() ticks
    pInfo->lPlaybackTime += lSamplesDecoded * CLOCKS_PER_SEC / pInfo->lSamplesPerSec;
}

void PerfTimerStopElapsed(PERFTIMERINFO *pInfo)
{
    pInfo->cTotalDecLoopTime = clock() - pInfo->cTotalDecLoopTime;
}

void PerfTimerReport(PERFTIMERINFO *pInfo)
{
    char    sz[256];
    float   fltDecodeTime;
    float   fltEntireDecodeTime;
    float   fltDecodeTimeFraction;
    float   fltPlaybackTime;

    sprintf(sz, "\n\n** Ticks per second (clock resolution): %ld.\n", CLOCKS_PER_SEC);
    OUTPUT_DEBUG_STRING(sz);

    fltDecodeTime = (float) pInfo->cDecodeTime / (float) CLOCKS_PER_SEC;
    fltEntireDecodeTime = (float) pInfo->cTotalDecLoopTime /
        (float) CLOCKS_PER_SEC;
    sprintf(sz, "** Decode time: %f sec. Entire decode time: %f sec.\n",
        fltDecodeTime, fltEntireDecodeTime);
    OUTPUT_DEBUG_STRING(sz);

    fltPlaybackTime = (float) pInfo->lPlaybackTime / (float) CLOCKS_PER_SEC;
    sprintf(sz, "** Playback time : %f sec.\n",
        fltPlaybackTime);
    OUTPUT_DEBUG_STRING(sz);

    fltDecodeTimeFraction = (float)pInfo->cDecodeTime / (float) pInfo->lPlaybackTime;
    sprintf(sz, "** Percentage of playback time spent decoding: %f%%.\n",
        fltDecodeTimeFraction * 100);
    OUTPUT_DEBUG_STRING(sz);
#	if defined(WMA_MONITOR)
		fprintf(stderr,"%s\n",sz);
#	endif

    sprintf(sz, "** Minimum MHz for realtime playback: %f of current CPU speed.\n",
        fltDecodeTimeFraction);
    OUTPUT_DEBUG_STRING(sz);

    sprintf(sz, "** This CPU is %f times faster than required.\n\n",
        (float)1.0 / fltDecodeTimeFraction);
    OUTPUT_DEBUG_STRING(sz);

}

float fltPerfTimerDecodeTime(PERFTIMERINFO *pInfo)
{
    return (float) pInfo->cDecodeTime / (float) CLOCKS_PER_SEC;
}

void PerfTimerFree(PERFTIMERINFO *pInfo)
{
    wmvfree(pInfo);
}

#endif  // PLATFORM_SPECIFIC_PERFTIMER


#if !defined(PLATFORM_SPECIFIC_DEBUGINFRASTRUCTURE) && defined(_DEBUG)
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
    OUTPUT_DEBUG_STRING(sz);
}

WMARESULT MyOutputDbgStr(WMARESULT wmaReturn, const char *pszFmt, ...)
{
    char    sz[512];
    va_list vargs;

    va_start(vargs, pszFmt);
    vsprintf(sz, pszFmt, vargs);
    va_end(vargs);

    OUTPUT_DEBUG_STRING(sz);
    return wmaReturn;
}
#endif  // !!defined(PLATFORM_SPECIFIC_DEBUGINFRASTRUCTURE) && defined(_DEBUG)



#if defined(_DEBUG) && defined(WMA_MONITOR)

#define DECL_MONITOR_RANGE(a) extern double a[4] = { 1.0e30, -1.0e30, 0, 0 };  extern long lc##a = 0;

// declare and define some range monitors in _DEBUG mode when WMA_MONITOR is defined
DECL_MONITOR_RANGE(gMR_iQuantStepSize)
DECL_MONITOR_RANGE(gMR_qstQuantStep)
DECL_MONITOR_RANGE(gMR_iCoefQ);
DECL_MONITOR_RANGE(gMC_0CoefQ);
DECL_MONITOR_RANGE(gMR_rgiMaskQ)
DECL_MONITOR_RANGE(gMR_rgiNoisePower)				// only for LowRate and MidRate
DECL_MONITOR_RANGE(gMR_rgiLspFreqQ)					// only for LowRate, e.g. LPC
DECL_MONITOR_RANGE(gMR_rgfltLspFreq)				// only for LowRate, e.g. LPC
DECL_MONITOR_RANGE(gMR_fltLPC_F)					// only for LowRate, e.g. LPC
DECL_MONITOR_RANGE(gMR_fltLPC_F1)					// only for LowRate, e.g. LPC
DECL_MONITOR_RANGE(gMR_fltLPC_F2)					// only for LowRate, e.g. LPC
DECL_MONITOR_RANGE(gMR_fltLPC_F3)					// only for LowRate, e.g. LPC
DECL_MONITOR_RANGE(gMR_rgfltWeightFactor)
DECL_MONITOR_RANGE(gMR_WeightRatio)
DECL_MONITOR_RANGE(gMR_rgfltBandWeight)				// only for MidRate and LowRate
DECL_MONITOR_RANGE(gMR_rgffltSqrtBWRatio)
DECL_MONITOR_RANGE(gMR_CoefRecon)
DECL_MONITOR_RANGE(gMC_zeroCoefRecon)
DECL_MONITOR_RANGE(gMR_DCTCoefIntoFFT)
DECL_MONITOR_RANGE(gMR_DCTCoefOut)
DECL_MONITOR_RANGE(gMC_ScalePowerToRMS)
DECL_MONITOR_RANGE(gMC_ScaleInverseQuadRoot)
DECL_MONITOR_RANGE(gMC_IQ)
//DECL_MONITOR_RANGE(gMC_IQ_iFractionBitsQuantStep)
DECL_MONITOR_RANGE(gMC_IQ_cQSIMWFracBits)
DECL_MONITOR_RANGE(gMR_QuantStepXInvMaxWeight)
DECL_MONITOR_RANGE(gMR_QuantStepXMaxWeightXWeightFactor)
DECL_MONITOR_RANGE(gMC_IQ_cWFFracBits)
DECL_MONITOR_RANGE(gMR_weightFactor)
DECL_MONITOR_RANGE(gMC_IQ_scale)
DECL_MONITOR_RANGE(gMC_IQ_scale2)
DECL_MONITOR_RANGE(gMC_IQ_Float)
DECL_MONITOR_RANGE(gMC_IQ_fltNoisePowCount)
DECL_MONITOR_RANGE(gMC_GBW_floats)

#if defined(WMA_MONITOR_FILE)
#	if defined(OUTPUT_DEBUG_STRING)
#		undef DUMP_MONITOR_RANGE
#		define DUMP_MONITOR_RANGE(a) if (lc##a>0) { sprintf(sz,"%14.6g %14.6g %14.6g %14.6g %8ld " #a "\n", a[0], a[1], a[2]/lc##a, sqrt((lc##a*a[3]-a[2]*a[2])/(((double)lc##a)*(lc##a-1))), lc##a ); OUTPUT_DEBUG_STRING(sz); }
#	else
#		undef DUMP_MONITOR_RANGE
#		define DUMP_MONITOR_RANGE(a) if (lc##a>0) fprintf(fp,"%14.6g %14.6g %14.6g %14.6g %8ld " #a "\n", a[0], a[1], a[2]/lc##a, sqrt((lc##a*a[3]-a[2]*a[2])/(((double)lc##a)*(lc##a-1))), lc##a );
#	endif
#endif

void DumpMonitorRanges(int fAppend) {
	FILE* fp = NULL;
#	if defined(WMA_MONITOR_FILE)
#		if defined(OUTPUT_DEBUG_STRING)
			char sz[256];
			int bODX = 0;
			if ( stricmp(WMA_MONITOR_FILE,"OutputDebugString")
				bODS = 1;
			else 
#		endif	// defined(OUTPUT_DEBUG_STRING)
		if ( stricmp(WMA_MONITOR_FILE,"stdout")
			fp = stdout;
		else
			fp = fopen(WMA_MONITOR_FILE,fAppend ? "w+" : "w");
#	else
		fp = stdout;
#	endif		

#	if defined(OUTPUT_DEBUG_STRING)
		if ( bODX )
			OUTPUT_DEBUG_STRING("\n       Minimum        Maximum           Mean StandDeviation    Count Variable\n");
		else
#	endif
		fprintf(fp,"\n       Minimum        Maximum           Mean StandDeviation    Count Variable\n");

	DUMP_MONITOR_RANGE(gMR_iQuantStepSize);
	DUMP_MONITOR_RANGE(gMR_qstQuantStep);
	DUMP_MONITOR_RANGE(gMR_iCoefQ);
	DUMP_MONITOR_RANGE(gMC_0CoefQ);
	DUMP_MONITOR_RANGE(gMR_rgiMaskQ);
#	ifdef ENABLE_LPC
		DUMP_MONITOR_RANGE(gMR_rgiLspFreqQ);
#		ifndef BUILD_INTEGER
			DUMP_MONITOR_RANGE(gMR_rgfltLspFreq);
#		endif
		DUMP_MONITOR_RANGE(gMR_fltLPC_F1);
		DUMP_MONITOR_RANGE(gMR_fltLPC_F2);
		DUMP_MONITOR_RANGE(gMR_fltLPC_F3);
#	endif
	DUMP_MONITOR_RANGE(gMR_rgiNoisePower);
	DUMP_MONITOR_RANGE(gMR_rgfltWeightFactor);
	DUMP_MONITOR_RANGE(gMR_WeightRatio);
	DUMP_MONITOR_RANGE(gMR_rgfltBandWeight);
	DUMP_MONITOR_RANGE(gMR_rgffltSqrtBWRatio);
	DUMP_MONITOR_RANGE(gMR_CoefRecon);
	DUMP_MONITOR_RANGE(gMC_zeroCoefRecon);
	DUMP_MONITOR_RANGE(gMR_DCTCoefIntoFFT);
	DUMP_MONITOR_RANGE(gMR_DCTCoefOut);
	DUMP_MONITOR_RANGE(gMR_QuantStepXInvMaxWeight);
	DUMP_MONITOR_RANGE(gMR_QuantStepXMaxWeightXWeightFactor);

	DUMP_MONITOR_RANGE(gMR_weightFactor);

	DUMP_MONITOR_RANGE(gMC_ScalePowerToRMS);
	DUMP_MONITOR_RANGE(gMC_ScaleInverseQuadRoot);
	DUMP_MONITOR_RANGE(gMC_IQ);
	//DUMP_MONITOR_RANGE(gMC_IQ_iFractionBitsQuantStep);
	DUMP_MONITOR_RANGE(gMC_IQ_cQSIMWFracBits);
	DUMP_MONITOR_RANGE(gMC_IQ_cWFFracBits);
	DUMP_MONITOR_RANGE(gMC_IQ_scale);
	DUMP_MONITOR_RANGE(gMC_IQ_scale2);
	DUMP_MONITOR_RANGE(gMC_IQ_Float);
	DUMP_MONITOR_RANGE(gMC_IQ_fltNoisePowCount);
	DUMP_MONITOR_RANGE(gMC_GBW_floats);
	if ( fp != NULL && fp != stdout )
		fclose(fp);
}
#	pragma COMPILER_MESSAGE(__FILE__ "(302) : Warning - WMA_MONITOR Debug Code Enabled.")
#endif


#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)

static FILE* fileShowFrames = NULL;

void prvWmaShowFrames(CAudioObject* pau, const char* pszID, const char* pszFmt, ...) {
#ifdef UNDER_CE
    if ( fileShowFrames == NULL )
        fileShowFrames = fopen("ShowFrames.txt","wt");
    if ( fileShowFrames == NULL )
        return;
#else
	fileShowFrames = stderr;
#endif
    fprintf(fileShowFrames,"%7s %3d.%d [%4d %4d %4d]",
        pszID,
        pau->m_iFrameNumber,
        pau->m_iCurrSubFrame,
        pau->m_cFrameSampleAdjusted,
        pau->m_cSubFrameSampleAdjusted,
        pau->m_cSubbandAdjusted
    );
    if ( pszFmt != NULL )
    {
        char    sz[512];
        va_list vargs;

        va_start(vargs, pszFmt);
        vsprintf(sz, pszFmt, vargs);
        va_end(vargs);

        fprintf(fileShowFrames,"%s",sz);
    }
    fprintf(fileShowFrames,"\n" );
}

#	pragma COMPILER_MESSAGE(__FILE__ "(341) : Warning - SHOW_FRAMES Debug Code Enabled.")
#endif

#if defined(BUILD_INT_FLOAT) && ( defined(WMA_TARGET_SH3) || defined(WMA_TARGET_MIPS) )
#	pragma COMPILER_MESSAGE(__FILE__ "(345) : Warning - Integer Float Build.")
#endif
#ifdef PROFILE
#	pragma COMPILER_MESSAGE(__FILE__ "(348) : Warning - PROFILE Enabled.")
#endif
#if defined(SDE_WANTS_ASSERTS) && defined(_DEBUG)
#	pragma COMPILER_MESSAGE(__FILE__ "(351) : Warning - Asserts are enabled - for SDE use only!")
#endif

#if defined(HEAP_DEBUG_TEST) && defined(_DEBUG)
// check the heap - see macros in msaudio.h
// _heapchk does not seem to be available under WinCE
void HeapDebugCheck()
	{ int  heapstatus;
	   /* Check heap status */
	   heapstatus = _heapchk();
	   switch( heapstatus )
	   {
	   case _HEAPOK:
		  //printf(" OK - heap is fine\n" );
		  break;
	   case _HEAPEMPTY:
		  printf(" OK - heap is empty\n" );
		  break;
	   case _HEAPBADBEGIN:
		  printf( "ERROR - bad start of heap\n" );
		  break;
	   case _HEAPBADNODE:
		  printf( "ERROR - bad node in heap\n" );
		  break;
	   }
	}
#endif

#ifdef UNDER_CE
// se need to define at least one public symbol
extern void wmaos_pacify(void) {}
#endif
