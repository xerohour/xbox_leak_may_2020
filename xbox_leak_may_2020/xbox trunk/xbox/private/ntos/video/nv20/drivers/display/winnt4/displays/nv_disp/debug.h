#ifndef _DEBUG_H
#define _DEBUG_H
//******************************************************************************
//
// Module Name: debug.h
//
// Commonly used debugging macros.
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

extern
VOID
DebugPrint(
    LONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );

#if DBG

#ifndef _WIN32_WINNT     // If this is NT 3.5
#define OutputDebugString OutputDebugStringA
#define EngDebugBreak() DebugBreak()
#define EngDebugPrint(s1, s2, s3) \
    OutputDebugString(s1);  \
    OutputDebugString(s2);  \
    OutputDebugString(s3)
#endif                  // If this is NT 3.5

VOID DebugLog(LONG, CHAR*, ...);

#define DISPDBG(arg) DebugPrint arg
#define DISPDBGCOND(cond, arg) if (cond) DISPDBG(arg)
#define STATEDBG(level) DebugState(level)


// FNicklisch 28.09.2000: more comfortable Assert and debug functions
#ifdef i386
#define DBG_HW_INDEP_BREAK { __asm {int 3} }
#else
#define DBG_HW_INDEP_BREAK EngDebugBreak()
#endif

//old version #define RIP(x) { DebugPrint(0, x); EngDebugBreak(); }
#define RIP(text)                                                                                                            \
  {                                                                                                                          \
    ENG_TIME_FIELDS localTime;                                                                                               \
    EngQueryLocalTime(&localTime);                                                                                           \
    DebugPrint(0, "RIP %s in %s, %d at %02d:%02d on %02d.%02d.%04d",                                                         \
      text?text:"",__FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
    DBG_HW_INDEP_BREAK;                                                                                                      \
    DebugPrint(0, "RIP %s in %s, %d at %02d:%02d on %02d.%02d.%04d",                                                         \
      text?text:"",__FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
  }

//old version #define ASSERTDD(x, y) if (!(x)) RIP(y)
#define ASSERTDD(condition, text)                                                            \
  if (!(condition))                                                                          \
  {                                                                                          \
    DebugPrint(0, "ASSERTION \"%s\" failed: %s (%s,%d)",#condition, text,__FILE__,__LINE__); \
    DBGBREAK(); /* No more output (text) needed! */                                          \
    DebugPrint(0, "ASSERTION \"%s\" failed: %s (%s,%d)",#condition, text,__FILE__,__LINE__); \
  }

#define DBG_ERROR(a) DISPDBG((0, "ERROR, %s", a));

#define DBGBREAK()                                                                                              \
  {                                                                                                             \
    ENG_TIME_FIELDS localTime;                                                                                  \
    EngQueryLocalTime(&localTime);                                                                              \
    DebugPrint(0, "BREAK in %s, %d at %02d:%02d on %02d.%02d.%04d",                                             \
      __FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
    DBG_HW_INDEP_BREAK;                                                                                         \
    DebugPrint(0, "BREAK in %s, %d at %02d:%02d on %02d.%02d.%04d",                                             \
      __FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
  }

#undef ASSERT // remove if defined!
#define ASSERT(condition) ASSERTDD(condition,"")

#define NVDBG(x, y) \
    if(!(x)){\
        extern BOOL g_bNvbreak;\
        DebugPrint(0, y);\
        if(g_bNvbreak) {EngDebugBreak();}\
    }

//*************************************************************************
// DBG_NAMEINDEX stuff:
// 
// Little helper code that allows us to easily convert enums or defines to 
// strings.
//*************************************************************************

//
// Basic structure conatining a index and the corresponding string
//
typedef struct _DBG_NAMEINDEX
{
  ULONG ulIndex;
  char *szName;
} DBG_NAMEINDEX;

// This macro simplifies the presetting of nameindex arrays
// Example aDbg_DrvEscapeIEsc in debug.c 
#define DBG_MAKE_NAMEINDEX(id) {(ULONG)id, #id}

// returns the string matching to index using lEntries out of the array pNameIndex
char *szDbg_GetDBG_NAMEINDEX(IN const struct _DBG_NAMEINDEX *pNameIndex, IN LONG lEntries, IN ULONG ulIndex);

// define to simplify the call to szDbg_GetDBG_NAMEINDEX
// Use it withing the specific strinizer function: Example szDbg_GetDrvEscapeIEsc in debug.c
#define DBG_GETDBG_NAMEINDEX(array, ulIndex) szDbg_GetDBG_NAMEINDEX((array), sizeof(array)/sizeof((array)[0]), ulIndex)

// Use Instead of DISPDBG if a 
VOID vDbg_PrintDBG_NAMEINDEX(LONG DebugPrintLevel,const PCHAR pcString,char *(szFlCaps)(ULONG),ULONG ul);
#define DBG_PRINT_DBG_NAMEINDEX(lvl,str,pfn,ul)    vDbg_PrintDBG_NAMEINDEX(lvl,str,pfn,ul)

VOID vDbg_PrintDBG_NAMEINDEXfl(LONG DebugPrintLevel,const PCHAR pcString,char *(szFlCaps)(ULONG),FLONG fl);
#define DBG_PRINT_DBG_NAMEINDEX_FL(lvl,str,pfn,fl) vDbg_PrintDBG_NAMEINDEXfl(lvl,str,pfn,fl)

char *szDbg_iEscGet(ULONG iEsc);
char *szDbg_NV_OPENGL_ESCAPE(ULONG iEsc);
char *szDbg_SURFOBJiBitmapFormat(ULONG iBitmapFormat);
char *szDbg_SURFOBJiType(USHORT iType);
char *szDbg_SURFOBJ_surf(SURFOBJ *pso);
char *szDbg_CLIPOBJiDComplexity(BYTE iDComplexity);
char *szDbg_XLATEOBJflXlate(FLONG flXlate);

// If we are not in a debug environment, we want all of the debug
// information to be stripped out.

// Translate ELSA trace code:
#define DBG_TRACE_IN(a, b)  DISPDBG((a,"%s {", #b))
#define DBG_TRACE_OUT(a, b) DISPDBG((a,"}: 0x%x", (unsigned long)b))
#define DBG_LVL_SUBENTRY  5
#define DBG_LVL_RECT      15

#else

#define EngDebugBreak()
#define DISPDBG(arg)
#define DISPDBGCOND(cond, arg)
#define STATEDBG(level)
#define LOGDBG(arg)
#define RIP(x)
#define ASSERTDD(x, y)

#define DBG_ERROR(a)
#define DBGBREAK()
#define ASSERT(x)


#define NVDBG(x, y)

#define DBG_PRINT_DBG_NAMEINDEX(lvl,str,pfn,ul)
#define DBG_PRINT_DBG_NAMEINDEX_FL(lvl,str,pfn,fl)

#define DBG_TRACE_IN(a,b)
#define DBG_TRACE_OUT(a,b)
#endif

#endif // _DEBUG_H
