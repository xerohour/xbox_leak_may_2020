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

#include "dscommon.h"

#ifdef USEDPF

#ifdef BUILDING_DSOUND

DWORD &CDebug::m_dwDpfLevel = g_dwDirectSoundDebugLevel;
DWORD &CDebug::m_dwDpfBreakLevel = g_dwDirectSoundDebugBreakLevel;
BOOL &CDebug::m_fDebugBreak = g_fDirectSoundDebugBreak;

#else // BUILDING_DSOUND

DWORD CDebug::m_dwDpfLevel = DPFLVL_DEFAULT;
DWORD CDebug::m_dwDpfBreakLevel = DPFLVL_DEFAULT_BREAK;
BOOL CDebug::m_fDebugBreak = FALSE;

#endif // BUILDING_DSOUND

LPFNDPFCALLBACK CDebug::m_pfnDpfCallback = NULL;
DPFCONTEXT CDebug::m_aContext[DEBUG_CONTEXT_COUNT] = { 0 };

#endif // USEDPF

#ifdef USEDEBUGLOG

CHAR CDebug::m_aszLog[DEBUGLOG_ENTRY_COUNT][DEBUGLOG_ENTRY_LENGTH] = { 0 };
DWORD CDebug::m_dwLogIndex = 0;

#endif // USEDEBUGLOG


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
 *  PrintStaticV
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

#ifdef USEDPF

void __cdecl 
CDebug::PrintStaticV
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

    if(dwLevel <= m_dwDpfLevel)
    {
        pszWorking = szDebug;

#ifdef DEBUG

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

        if(dwLevel && (dwLevel <= m_dwDpfBreakLevel))
        {
            Break();
        }

#endif // DEBUG

        //
        // Hand off to the callback routine
        //

        if(m_pfnDpfCallback)
        {
            m_pfnDpfCallback(dwLevel, szText);
        }
    }
}

#endif // USEDPF


/****************************************************************************
 *
 *  PrintStatic
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

#ifdef USEDPF

void __cdecl 
CDebug::PrintStatic
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
    PrintStaticV(dwFlags, dwLevel, pszFile, nLine, pszFunction, pszLibrary, pszFormat, va);
    va_end(va);
}

#endif // USEDPF


/****************************************************************************
 *
 *  SetContext
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

#ifdef USEDPF

void __cdecl 
CDebug::SetContext
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

    m_aContext[irql].dwFlags = dwFlags;
    m_aContext[irql].dwLevel = dwLevel;
    m_aContext[irql].pszFile = pszFile;
    m_aContext[irql].nLine = nLine;
    m_aContext[irql].pszFunction = pszFunction;
    m_aContext[irql].pszLibrary = pszLibrary;
}

#endif // USEDPF


/****************************************************************************
 *
 *  Print
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

#ifdef USEDPF

void __cdecl 
CDebug::Print
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
    PrintStaticV(m_aContext[irql].dwFlags, m_aContext[irql].dwLevel, m_aContext[irql].pszFile, m_aContext[irql].nLine, m_aContext[irql].pszFunction, m_aContext[irql].pszLibrary, pszFormat, va);
    va_end(va);
}

#endif // USEDPF


/****************************************************************************
 *
 *  Assert
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

#ifdef USEDPF

void __cdecl 
CDebug::Assert
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
        Break();
    }
}

#endif // USEDPF


/****************************************************************************
 *
 *  Break
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

#ifdef USEDPF

void __cdecl 
CDebug::Break
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
        m_fDebugBreak = TRUE;
    }

#endif // _XBOX

}

#endif // USEDPF


/****************************************************************************
 *
 *  Log
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

void __cdecl 
CDebug::Log
(
    LPCSTR                  pszFormat, 
    ...
)
{
    va_list                 va;
    int                     i;

    va_start(va, pszFormat);
    i = vsprintf(m_aszLog[m_dwLogIndex], pszFormat, va);
    va_end(va);
    
    for(i = i + 1; i < NUMELMS(m_aszLog[m_dwLogIndex]); i++)
    {
        m_aszLog[m_dwLogIndex][i] = 0;
    }

    m_dwLogIndex = (m_dwLogIndex + 1) % NUMELMS(m_aszLog);
}

#endif // USEDEBUGLOG


