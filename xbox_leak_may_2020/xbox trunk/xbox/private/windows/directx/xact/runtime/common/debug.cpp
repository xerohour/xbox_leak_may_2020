/***************************************************************************
 *
 *  Copyright (C) 1995-1998 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       debug.cpp
 *  Content:    Debugger helper object.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  05/23/00    dereks  Created.
 *
 ***************************************************************************/

#include "common.h"

namespace XACT {

#ifdef USEDPF

//
// Globals
//

DWORD g_dwXactDebugLevel = DPFLVL_DEFAULT;
DWORD g_dwXactDebugBreakLevel = DPFLVL_DEFAULT_BREAK;

DPFCONTEXT g_XactDebugContext[HIGH_LEVEL + 1];
BOOL g_fXactDebugBreak = FALSE;



/***************************************************************************
 *
 *  dstrcpy
 *
 *  Description:
 *      Copies one string to another.
 *
 *  Arguments:
 *      LPSTR [in/out]: destination string.
 *      LPCSTR [in]: source string.
 *
 *  Returns:  
 *      LPSTR: pointer to the end of the string.
 *
 ***************************************************************************/

__inline LPSTR 
dstrcpy
(
    LPSTR                   dst, 
    LPCSTR                  src
)
{
    while(*dst = *src)
    {
        dst++;
        src++;
    }
    
    return dst;
}


/****************************************************************************
 *
 *  DwDbgPrintStaticV
 *
 *  Description:
 *      Prints a string to the debugger.
 *
 *  Arguments:
 *      DWORD [in]: option flags.
 *      DWORD [in]: debug level.
 *      LPCSTR [in]: file called from.
 *      UINT [in]: line called from.
 *      LPCSTR [in]: function called from.
 *      LPCSTR [in]: format string.
 *      va_list [in]: format arguments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl 
DwDbgPrintStaticV
(
    DWORD                   dwFlags, 
    DWORD                   dwLevel,     
    LPCSTR                  pszFile, 
    UINT                    nLine, 
    LPCSTR                  pszFunction, 
    LPCSTR                  pszLibrary,
    LPCSTR                  pszFormat,
    va_list                 va
)
{
    CHAR                    szDebug[0x400];
    CHAR                    szText[0x400];
    LPSTR                   pszWorking;
    DWORD                   dwWritten;

    if(dwLevel <= g_dwXactDebugLevel)
    {

#ifdef DEBUG

        pszWorking = szDebug;

        //
        // Add the library name
        //

        if((dwFlags & DPF_FLAGS_LIBRARY) && pszLibrary && *pszLibrary)
        {
            pszWorking += sprintf(pszWorking, "%s: ", pszLibrary);
        }

        //
        // Add the source file and line number
        //

        if((dwFlags & DPF_FLAGS_FILELINE) && pszFile && *pszFile)
        {
            pszWorking += sprintf(pszWorking, "%hs(%lu): ", pszFile, nLine);
        }

        //
        // Add the function name
        //

        if((dwFlags & DPF_FLAGS_FUNCTIONNAME) && pszFunction && *pszFunction)
        {
            pszWorking += sprintf(pszWorking, "%hs: ", pszFunction);
        }

        //
        // Add process and thread id
        //

        if(dwFlags & DPF_FLAGS_PROCESSTHREADID)
        {
            pszWorking += sprintf(pszWorking, "%.8lx: ", GetCurrentThreadId());
        }

        //
        // Add the type of message it is (i.e. error or warning)
        //

        switch(dwLevel)
        {
            case DPFLVL_ERROR:
                pszWorking = dstrcpy(pszWorking, "Error: ");
                break;

            case DPFLVL_RESOURCE:
                pszWorking = dstrcpy(pszWorking, "Resource failure: ");
                break;

            case DPFLVL_WARNING:
                pszWorking = dstrcpy(pszWorking, "Warning: ");
                break;
        }

#endif // DEBUG

        //
        // Add the debug string
        //

        vsprintf(szText, pszFormat, va);

#ifdef DEBUG
        
        pszWorking = dstrcpy(pszWorking, szText);

        //
        // Add a carriage-return since OutputDebugString doesn't
        //

        pszWorking = dstrcpy(pszWorking, "\n");

        //
        // Output to the debugger
        //

        OutputDebugStringA(szDebug);

        //
        // Break into the debugger
        //

        if(dwLevel && (dwLevel <= g_dwXactDebugBreakLevel))
        {
            DwDbgBreak();
        }

#endif // DEBUG

    }
}


/****************************************************************************
 *
 *  DwDbgPrintStatic
 *
 *  Description:
 *      Prints a string to the debugger.
 *
 *  Arguments:
 *      DWORD [in]: option flags.
 *      DWORD [in]: debug level.
 *      LPCSTR [in]: file called from.
 *      UINT [in]: line called from.
 *      LPCSTR [in]: function called from.
 *      LPCSTR [in]: format string.
 *      va_list [in]: format arguments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl 
DwDbgPrintStatic
(
    DWORD                   dwFlags, 
    DWORD                   dwLevel,     
    LPCSTR                  pszFile, 
    UINT                    nLine, 
    LPCSTR                  pszFunction, 
    LPCSTR                  pszLibrary,
    LPCSTR                  pszFormat,
    ...
)
{
    va_list                 va;

    va_start(va, pszFormat);
    DwDbgPrintStaticV(dwFlags, dwLevel, pszFile, nLine, pszFunction, pszLibrary, pszFormat, va);
    va_end(va);
}


/****************************************************************************
 *
 *  DwDbgSetContext
 *
 *  Description:
 *      Sets context for a DPF.
 *
 *  Arguments:
 *      DWORD [in]: option flags.
 *      DWORD [in]: debug level.
 *      LPCSTR [in]: file called from.
 *      UINT [in]: line called from.
 *      LPCSTR [in]: function called from.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl 
DwDbgSetContext
(
    DWORD                   dwFlags, 
    DWORD                   dwLevel,     
    LPCSTR                  pszFile, 
    UINT                    nLine, 
    LPCSTR                  pszFunction,
    LPCSTR                  pszLibrary
)
{

#ifdef _XBOX

    const KIRQL             irql    = KeGetCurrentIrql();

#else // _XBOX

    static const UINT       irql    = 0;

#endif // _XBOX

    g_XactDebugContext[irql].dwFlags = dwFlags;
    g_XactDebugContext[irql].dwLevel = dwLevel;
    g_XactDebugContext[irql].pszFile = pszFile;
    g_XactDebugContext[irql].nLine = nLine;
    g_XactDebugContext[irql].pszFunction = pszFunction;
    g_XactDebugContext[irql].pszLibrary = pszLibrary;
}


/****************************************************************************
 *
 *  DwDbgPrint
 *
 *  Description:
 *      Prints a string to the debugger.
 *
 *  Arguments:
 *      LPCSTR [in]: format string.
 *      ... [in]: format arguments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl 
DwDbgPrint
(
    LPCSTR                  pszFormat,
    ...
)
{

#ifdef _XBOX

    const KIRQL             irql    = KeGetCurrentIrql();

#else // _XBOX

    static const UINT       irql    = 0;

#endif // _XBOX

    va_list                 va;

    va_start(va, pszFormat);
    DwDbgPrintStaticV(g_XactDebugContext[irql].dwFlags, g_XactDebugContext[irql].dwLevel, g_XactDebugContext[irql].pszFile, g_XactDebugContext[irql].nLine, g_XactDebugContext[irql].pszFunction, g_XactDebugContext[irql].pszLibrary, pszFormat, va);
    va_end(va);
}


/****************************************************************************
 *
 *  DwDbgAssert
 *
 *  Description:
 *      Breaks into the debugger.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to assert.
 *      LPCSTR [in]: expression string.
 *      LPCSTR [in]: file called from.
 *      UINT [in]: line number called from.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl 
DwDbgAssert
(
    BOOL                    fAssert,
    LPCSTR                  pszExpression,
    LPCSTR                  pszFile,
    UINT                    nLine
)
{
    static const LPCSTR     pszBanner           = "******************************************************************************";
    CHAR                    szMessage[0x400];
    LPSTR                   pszMessage;

    if(fAssert)
    {
        pszMessage = szMessage;

        pszMessage = dstrcpy(pszMessage, pszBanner);
        pszMessage = dstrcpy(pszMessage, "\n");
        pszMessage = dstrcpy(pszMessage, "Assertion failed in ");
        pszMessage = dstrcpy(pszMessage, pszFile);
        pszMessage = dstrcpy(pszMessage, ", line ");
    
        _itoa(nLine, pszMessage, 10);
        pszMessage += strlen(pszMessage);

        pszMessage = dstrcpy(pszMessage, ":\n");
        pszMessage = dstrcpy(pszMessage, pszExpression);
        pszMessage = dstrcpy(pszMessage, "\n");
        pszMessage = dstrcpy(pszMessage, pszBanner);
        pszMessage = dstrcpy(pszMessage, "\n");

        OutputDebugStringA(szMessage);
        DwDbgBreak();
    }
}


/****************************************************************************
 *
 *  DwDbgBreak
 *
 *  Description:
 *      Breaks into the debugger.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl 
DwDbgBreak
(
    void
)
{
    //
    // Break in the debugger
    //
    
    __asm int 3;

#ifdef _XBOX

    //
    // If we're at DISPATCH_LEVEL or higher, the VC debugger won't catch
    // the break.
    //
    
    if(KeGetCurrentIrql() >= DISPATCH_LEVEL)
    {
        g_fXactDebugBreak = TRUE;
    }

#endif // _XBOX

}


#endif // USEDPF


/****************************************************************************
 *
 *  DwDbgLog
 *
 *  Description:
 *      Adds an entry to the debug log.
 *
 *  Arguments:
 *      LPCSTR [in]: format string.
 *      ...
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef USEDEBUGLOG

CHAR g_aszDebugLog[DLOG_ENTRY_COUNT][DLOG_ENTRY_LENGTH] = { 0 };
DWORD g_dwXactDebugLogIndex = 0;

#undef DPF_FNAME
#define DPF_FNAME "DwDbgLog"

void __cdecl 
DwDbgLog
(
    LPCSTR                  pszFormat, 
    ...
)
{
    va_list                 va;
    int                     i;

    va_start(va, pszFormat);
    i = vsprintf(g_aszDebugLog[g_dwXactDebugLogIndex], pszFormat, va);
    va_end(va);
    
    for(i = i + 1; i < NUMELMS(g_aszDebugLog[g_dwXactDebugLogIndex]); i++)
    {
        g_aszDebugLog[g_dwXactDebugLogIndex][i] = 0;
    }

    g_dwXactDebugLogIndex = (g_dwXactDebugLogIndex + 1) % NUMELMS(g_aszDebugLog);
}

#endif // USEDEBUGLOG

}
