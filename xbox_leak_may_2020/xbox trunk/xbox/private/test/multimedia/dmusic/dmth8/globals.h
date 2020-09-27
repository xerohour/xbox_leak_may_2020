#pragma once


#undef UNICODE  //Ugh, UNICODE on XBox is like tar on pancakes.
#define INITGUID

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
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
#endif 

#include "logging.h"
#include "fakestuff.h"
#pragma warning( disable : 4056 4756 4005)

#include "helpers.h"

extern  HANDLE g_hLog;
extern  LPSTR g_szComponent;
extern  LPSTR g_szSubComp;
extern  LPSTR g_szFunction;
extern  LPSTR g_szVariation;

// Disable warning messages about floating point overflow.
#pragma warning( disable : 4056 4756)
#define INFINITY (FLT_MAX * FLT_MAX)       
#define ALMOST_FLT_MAX (FLT_MAX * 0.99f) //so we don't run up against MAXDISTANCE.

extern CHAR *g_szDefaultMedia;  //defined in DMTEST1.CPP

