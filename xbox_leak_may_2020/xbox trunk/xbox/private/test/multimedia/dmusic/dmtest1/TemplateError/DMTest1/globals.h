#pragma once

#undef UNICODE  //Ugh, UNICODE on XBox is like tar on pancakes.

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <stdlib.h>
#include <malloc.h>
#include <malloc.h>
#include <float.h>
#include <dmusicc.h>
#include <dmusici.h>

#include <dmth.h>

#ifdef NOLOG
    #include "nakedmacros.h"
#else
    #include <macros.h>
#endif 

#include <stdio.h>
#include "logging.h"
#include "fakestuff.h"
#pragma warning( disable : 4056 4756 4005)

#include "helpers.h"

extern "C" HANDLE g_hLog;
extern "C" LPSTR g_szComponent;
extern "C" LPSTR g_szSubComp;
extern "C" LPSTR g_szFunction;
extern "C" LPSTR g_szVariation;

// Disable warning messages about floating point overflow.
#pragma warning( disable : 4056 4756)
#define INFINITY (FLT_MAX * FLT_MAX)       
#define ALMOST_FLT_MAX (FLT_MAX * 0.99f) //so we don't run up against MAXDISTANCE.


//BUGBUG: This can go away.
#define TESTPARAMS CtIDirectMusicPerformance8 *ptPerf8, DWORD dwParam1, DWORD dwParam2, LPARAM dwParam3, LPARAM dwParam4

extern CHAR *g_szDefaultMedia;  //defined in DMTEST1.CPP

