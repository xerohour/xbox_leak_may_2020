// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* NV Common ******************************
//
//  Module: nvDbg.cpp
//      Shared debugging routines for printing, tracing, etc.
//
// **************************************************************************
//
//  History:
//      Matt Lavoie             18Nov00         extracted from d3d
//      Craig Duttweiler        21Nov00         major rehash
//
// **************************************************************************

#include "CompileControl.h"

#if (IS_WINNT5 || IS_WIN9X)
    #include "nvprecomp.h"
    #pragma hdrstop
    // need to port nvFile to NT4
    #define HAVE_NV_FILE_FACILITY
#endif  // (IS_WINNT5 || IS_WIN9X)

#include "nvUtil.h"
#include "nvDbg.h"
#include <stdarg.h>
#include <minmax.h>

//-------------------------------------------------------------------------
// GLOBAL VARIABLES
//---------------------------------------------------------------------------

#ifdef DEBUG

    int        dbgTrace    = 0;
    NvU32      dbgLevel    = 0;
    NvU32      dbgLog      = 0;
    int        dbgDP2      = 0;

    static int iTraceLevel = 0;

#endif

//-------------------------------------------------------------------------
// DEBUG PRINTING
//---------------------------------------------------------------------------

#if (IS_WINNT4)
    extern "C" void EngDebugPrint( char *, char *, va_list ap );
#endif  // (IS_WINNT4)

static __inline void nvOutputDebugString (char *szStr)
{
#if (IS_WINNT5 || IS_WINNT4)
    EngDebugPrint("", szStr, (va_list)(NULL));
#elif (IS_WIN9X)
    OutputDebugStringA(szStr);
#endif
}
#if 0
//---------------------------------------------------------------------------

void __cdecl PF (char * szFormat, ...)
{
    char str[256];

    nvStrCpy (str, DPF_START_STR);
    nvSprintfVAL (str+nvStrLen(str), szFormat, (va_list)(&szFormat+1));
    nvStrCat (str, DPF_END_STR);
    nvOutputDebugString (str);
}
#endif
//---------------------------------------------------------------------------

#ifdef DEBUG

void __cdecl DPF (char * szFormat, ...)
{
    char str[256];

    nvStrCpy (str, DPF_START_STR);
    nvSprintfVAL (str+nvStrLen(str), szFormat, (va_list)(&szFormat+1));
    nvStrCat (str, DPF_END_STR);

    DPF_PLAIN ("%s", str);
}

#ifdef HAVE_NV_FILE_FACILITY
HANDLE dpf_file;
//-------------------------------------------------------------------------

void __cdecl createDPFLog()
{
    if (dbgLog)
    {
        dpf_file = NvCreateFile(DPF_LOG_FILE, GENERIC_WRITE, 0,0, CREATE_ALWAYS,0,0);
        if (dpf_file != INVALID_HANDLE_VALUE)
        {
            return;
        }

    }
    else
    {
        dpf_file = 0;
    }
}

//-------------------------------------------------------------------------

void __cdecl closeDPFLog()
{
    if (dbgLog)
    {
        if (dpf_file != INVALID_HANDLE_VALUE)
        {
            NvCloseHandle(dpf_file);
            dpf_file = 0;
        }

    }
    else
    {
        dpf_file = 0;
    }
}
#endif

//-------------------------------------------------------------------------

void __cdecl DPF_PLAIN (char * szFormat, ...)
{
    static NvU8 bFirst = TRUE;
    char   str[256];

    nvSprintfVAL (str, szFormat, (va_list)(&szFormat+1));
    nvOutputDebugString (str);

#ifdef HAVE_NV_FILE_FACILITY
    // echo output to root of current working drive
    if (dbgLog) {
        NvU32 dw;
        NvWriteFile (dpf_file,str,nvStrLen(str),&dw,NULL);
    }
#endif
}

//-------------------------------------------------------------------------

void __cdecl DPF_LEVEL (NvU32 dwLevel, char *szFormat, ...)
{
    if (dbgLevel & dwLevel) {
        char str[256];
        nvStrCpy (str, DPF_START_STR);
        nvSprintfVAL (str+nvStrLen(str), szFormat, (va_list)(&szFormat+1));
        nvStrCat (str, DPF_END_STR);
        DPF_PLAIN ("%s", str);
    }
}

//-------------------------------------------------------------------------

void __cdecl DPF_LEVEL_PLAIN (NvU32 dwLevel, char * szFormat, ...)
{
    if (dbgLevel & dwLevel) {
        char str[256];
        nvSprintfVAL (str, szFormat, (va_list)(&szFormat+1));
        DPF_PLAIN ("%s", str);
    }
}

#endif  // DEBUG

//-------------------------------------------------------------------------
// TRACING
//-------------------------------------------------------------------------

#ifdef DEBUG

void __cdecl dbgResetTraceLevel(void)
{
    iTraceLevel = 0;
}

//-------------------------------------------------------------------------

void __cdecl dbgTracePush (char * szFormat, ...)
{
    char szIndent[2*NV_TRACE_MAX_DEPTH+1];
    char szFullStr[512];
    char szSubStr[NV_TRACE_MAX_COLS+1];
    char szOutStr[NV_TRACE_MAX_COLS+1];
    char szDP2[512];
    int  i;

#if IS_WIN9X
    if (global.b16BitCode) { return; }
#endif

    //print only dp2 calls
    nvStrCpy(szDP2,szFormat);
    szDP2[5] = '\0';
    if (dbgDP2 && nvStrCmp(szDP2,"nvDP2")) {
        iTraceLevel++;
        return;
    }
    
    
    
    // Make sure we want to print this level
    if (iTraceLevel >= (dbgTrace & ~NVDBG_TRACE_EXIT)) {
        iTraceLevel++;
        return;
    }

    // get the proper indent
    szIndent[0] = '\0';
    for (i = 0; (i < iTraceLevel) && (i < NV_TRACE_MAX_DEPTH); i++) {
        nvStrCat (szIndent, "  ");
    }
    

    // generate the full string.
    nvStrCpy (szFullStr, NV_TRACE_START_STR);
    nvSprintfVAL (szFullStr+nvStrLen(szFullStr), szFormat, (va_list)(&szFormat+1));
    NvU32 dwLength = nvStrLen (szFullStr);

    // dole it out in chunks
    NvU32 dwMaxCharsPerLine = NV_TRACE_MAX_COLS - nvStrLen(DPF_START_STR) - nvStrLen(szIndent);
    i = 0;
    while (dwLength) {
        NvU32 dwThisLength = min (dwLength,dwMaxCharsPerLine);
        nvStrNCpy (szSubStr, szFullStr+i, dwThisLength);
        szSubStr[dwThisLength] = '\0';
        nvSprintf (szOutStr, "%s%s", szIndent, szSubStr);
        DPF (szOutStr);
        dwLength -= dwThisLength;
        i += dwThisLength;
    }

    iTraceLevel++;
}

//-------------------------------------------------------------------------

void __cdecl dbgTracePop (void)
{
    char szIndent[2 * NV_TRACE_MAX_DEPTH + 1];
    int i;

#if IS_WIN9X
    if (global.b16BitCode) { return; }
#endif


    iTraceLevel--;

    if ((dbgTrace & NVDBG_TRACE_EXIT) && (iTraceLevel < (dbgTrace & ~NVDBG_TRACE_EXIT))) {
        // get the proper indent
        szIndent[0] = '\0';
        for (i = 0; (i < iTraceLevel) && (i < NV_TRACE_MAX_DEPTH); i++) {
            nvStrCat (szIndent, "  ");
        }
        DPF("%sTRACE: [exit]", szIndent);
    }

    // sanity check
    if (iTraceLevel < 0) {
        dbgError("error: iTraceLevel < 0");
    }
}

#elif INSTRUMENT_ALL_CALLS

#define MAXSTACK 1024   // keep track this many levels deep.
char  aszProcName[MAXSTACK][256];
DWORD adwTimeStack[MAXSTACK];
DWORD dwTimeStackIndex = 0;
BOOL  bTimedBefore     = FALSE;

//-------------------------------------------------------------------------

void __cdecl dbgTracePush (LPSTR szFormat, ...)
{
    if (global.b16BitCode) { return; }

    if (dwTimeStackIndex < MAXSTACK) {
        // for now we just keep the format string (no expantion)
        strcpy (&aszProcName[dwTimeStackIndex][10],szFormat);
        // start timer
        adwTimeStack[dwTimeStackIndex] = GetTickCount();
    }
    dwTimeStackIndex++;
}

//-------------------------------------------------------------------------

void __cdecl dbgTracePop (void)
{
    if (global.b16BitCode) { return; }

    // stop timer
    dwTimeStackIndex--;

    if (dwTimeStackIndex < MAXSTACK) {

        DWORD dwDelta = GetTickCount() - adwTimeStack[dwTimeStackIndex];

        if (dwDelta >= IAC_THRESHOLD) {
            // open file
            HANDLE h = NvCreateFile("\\timeinfo.txt",
                                    GENERIC_WRITE,
                                    0,
                                    NULL,
                                    bTimedBefore ? OPEN_ALWAYS : CREATE_ALWAYS, // delete old file on new runs
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
            if (!h) {
                __asm int 3 // cannot create file
            }
            SetFilePointer (h,0,NULL,FILE_END);

            // log time
            DWORD dw;
            aszProcName[dwTimeStackIndex][0] = char('0' + (dwTimeStackIndex / 100) % 10);
            aszProcName[dwTimeStackIndex][1] = char('0' + (dwTimeStackIndex / 10) % 10);
            aszProcName[dwTimeStackIndex][2] = char('0' + (dwTimeStackIndex / 1) % 10);
            aszProcName[dwTimeStackIndex][3] = char(':');
            aszProcName[dwTimeStackIndex][4] = char('0' + (dwDelta / 10000) % 10);
            aszProcName[dwTimeStackIndex][5] = char('0' + (dwDelta / 1000) % 10);
            aszProcName[dwTimeStackIndex][6] = char('0' + (dwDelta / 100) % 10);
            aszProcName[dwTimeStackIndex][7] = char('0' + (dwDelta / 10) % 10);
            aszProcName[dwTimeStackIndex][8] = char('0' + (dwDelta / 1) % 10);
            aszProcName[dwTimeStackIndex][9] = char(':');
            strcat (&aszProcName[dwTimeStackIndex][10],"\r\n");
            NvWriteFile (h,aszProcName[dwTimeStackIndex],strlen(aszProcName[dwTimeStackIndex]),&dw,NULL);

            // done
            NvCloseHandle (h);
            bTimedBefore = TRUE;
        }
    }
}

#endif // INSTRUMENT_ALL_CALLS

//-------------------------------------------------------------------------
// ERROR HANDLING
//---------------------------------------------------------------------------

#ifdef DEBUG

int _nvAssertReport(const char *pszExpr, const char *pszFile, const int nLine)
{
    DPF("Assertion failed!");
    DPF("File: %s", pszFile);
    DPF("Line: %d", nLine);
    DPF("Expression: %s", pszExpr);
    return TRUE;
}

#endif  // DEBUG

