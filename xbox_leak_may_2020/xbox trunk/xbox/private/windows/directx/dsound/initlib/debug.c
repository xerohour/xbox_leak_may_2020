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

#include "dsoundi.h"

#ifdef USEDPF

//
// Debug levels
//

DWORD g_dwDirectSoundDebugLevel = DPFLVL_DEFAULT;
DWORD g_dwDirectSoundDebugBreakLevel = DPFLVL_DEFAULT_BREAK;

//
// Deferred breakpoint
//

BOOL g_fDirectSoundBreakPoint = FALSE;

//
// Debug context data
//

BEGIN_DEFINE_STRUCT()
    DWORD   dwFlags;
    DWORD   dwLevel;
    LPCSTR  pszFile;
    UINT    nLine;
    LPCSTR  pszFunction;
    LPCSTR  pszLibrary;
END_DEFINE_STRUCT(DPFCONTEXT);

DPFCONTEXT g_DpfContext[HIGH_LEVEL + 1];


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
    CHAR                    szFinal[0x400];
    LPSTR                   pszWorking;
    DWORD                   dwWritten;

    if(dwLevel <= g_dwDirectSoundDebugLevel)
    {
        pszWorking = szFinal;

        //
        // Add the library name
        //

        if((dwFlags & DPRINTF_LIBRARY) && pszLibrary && *pszLibrary)
        {
            pszWorking += sprintf(pszWorking, "%s: ", pszLibrary);
        }

        //
        // Add the source file and line number
        //

        if((dwFlags & DPRINTF_FILELINE) && pszFile && *pszFile)
        {
            pszWorking += sprintf(pszWorking, "%hs(%lu): ", pszFile, nLine);
        }

        //
        // Add the function name
        //

        if((dwFlags & DPRINTF_FUNCTIONNAME) && pszFunction && *pszFunction)
        {
            pszWorking += sprintf(pszWorking, "%hs: ", pszFunction);
        }

        //
        // Add process and thread id
        //

        if(dwFlags & DPRINTF_PROCESSTHREADID)
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

        //
        // Add the debug string
        //

        pszWorking += vsprintf(pszWorking, pszFormat, va);

        //
        // Add a carriage-return since OutputDebugString doesn't
        //

        pszWorking = dstrcpy(pszWorking, "\n");

        //
        // Output to the debugger
        //

        OutputDebugStringA(szFinal);

        //
        // Break into the debugger
        //

        if(dwLevel && (dwLevel <= g_dwDirectSoundDebugBreakLevel))
        {
            DwDbgBreak();
        }
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
    const KIRQL             irql    = KeGetCurrentIrql();

    g_DpfContext[irql].dwFlags = dwFlags;
    g_DpfContext[irql].dwLevel = dwLevel;
    g_DpfContext[irql].pszFile = pszFile;
    g_DpfContext[irql].nLine = nLine;
    g_DpfContext[irql].pszFunction = pszFunction;
    g_DpfContext[irql].pszLibrary = pszLibrary;
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
    const KIRQL             irql    = KeGetCurrentIrql();
    va_list                 va;

    va_start(va, pszFormat);
    DwDbgPrintStaticV(g_DpfContext[irql].dwFlags, g_DpfContext[irql].dwLevel, g_DpfContext[irql].pszFile, g_DpfContext[irql].nLine, g_DpfContext[irql].pszFunction, g_DpfContext[irql].pszLibrary, pszFormat, va);
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
    
#ifdef DPF_LIBRARY

        pszMessage = dstrcpy(pszMessage, DPF_LIBRARY ": ");

#endif // DPF_LIBRARY
    
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

    //
    // If we're at DISPATCH_LEVEL or higher, the VC debugger won't catch
    // the break.
    //
    
    if(KeGetCurrentIrql() >= DISPATCH_LEVEL)
    {
        g_fDirectSoundBreakPoint = TRUE;
    }
}


#endif // USEDPF


