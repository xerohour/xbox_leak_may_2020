#pragma once

#undef UNICODE  //Ugh, UNICODE on XBox is like tar on pancakes.
#define INITGUID

#include <xtl.h>
#include <xdbg.h>
#include <xnetref.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xobjbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <float.h>
#include <dmusici.h>

#include <dmth.h>

#ifdef NOLOG
    #include "nakedmacros.h"
#else
    #include <macros.h>
#endif bz

#include "logging.h"
#include "fakestuff.h"
#pragma warning( disable : 4056 4756 4005)

#include "helpers.h"
#include "ToolTest.h"
#include "ToolEcho.h"
#include "ToolTrace.h"
#include "cicmusicx.h"
#include "medialoader.h"

//lint -e762
extern HANDLE g_hLog;
//lint +e762

#define PACK(x,y) (((DWORD)(x << 16)) + (DWORD)y)
#define UNPACK(a, x, y) {x=(a)>>16; y=(a) & 0x0000FFFF;}

// Disable messages about floating point overflow.
#pragma warning( disable : 4056 4756)



#define szInitPerformance_SIZE 10


struct TESTPARAMS
{
    BOOL bWait;
    BOOL bBVT;
    BOOL bValid;
    BOOL bInvalid;
    BOOL bFatal;
    BOOL bPerf;
    BOOL bStress;
    BOOL bWaitAtTestEnd;
    BOOL bSkipUserInput;
    BOOL bLogToScreen;
    BOOL bUseGM_DLS;
    BOOL bSuppressMemInfo;
    DWORD dwPerfWait;
    DWORD dwWaitBetweenTests;
    DWORD dwDebugLevel;
    DWORD dwRIPLevel;
    DWORD dwMLDebugLevel;
    DWORD dwLocalLogLevel;
    DWORD bDoWorkLocalThread;
    DWORD dwDoWorkFrequency;
    DWORD dwDMVoiceCount;
    DWORD dwDecreaseMemory;
    DWORD dwStressFileIODuration;
    CHAR szStressFileIOThreadPri[MAX_PATH];
    CHAR szDSScratchImage[MAX_PATH];
    CHAR szDSHRTF[MAX_PATH];
    CHAR szDefaultMedia[MAX_PATH];
    CHAR szInitPerformance[szInitPerformance_SIZE];
    CHAR szInitialTestCase[MAX_PATH];
    CHAR  szDMHeap[MAX_PATH];
    DWORD dwDMFixedHeapSizeNorm;
    DWORD dwDMFixedHeapSizePhys;

};

#define MAX_PROFILESECTION_SIZE 0xFFFF
extern TESTPARAMS g_TestParams;

#define TERMINATE NULL,NULL,NULL