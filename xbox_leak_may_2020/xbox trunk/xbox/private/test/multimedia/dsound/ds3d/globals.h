#pragma once


#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <stdlib.h>
#include <waveldr.h>
#include <float.h>
#include <dsstdfx.h>

#ifdef NOLOG
    #include "nakedmacros.h"
#else
    #include <macros.h>
#endif 

#include <stdio.h>

enum TestApplies {TESTAPPLY_DEFERRED_NOUPDATE=0x5678, TESTAPPLY_DEFERRED_UPDATE, TESTAPPLY_IMMEDIATE};

#include "util.h"

extern HANDLE g_hLog;
extern "C" LPSTR g_szComponent;
extern "C" LPSTR g_szSubComp;
extern "C" LPSTR g_szFunction;
extern "C" LPSTR g_szVariation;


// Disable messages about floating point overflow.
#pragma warning( disable : 4056 4756)
#define INFINITY (FLT_MAX * FLT_MAX)       
#define ALMOST_FLT_MAX (FLT_MAX * 0.99f) //so we don't run up against MAXDISTANCE.

static const ULONG MIN_FILE_NUM					= 1;
static const ULONG MAX_FILE_NUM					= 2846; 



