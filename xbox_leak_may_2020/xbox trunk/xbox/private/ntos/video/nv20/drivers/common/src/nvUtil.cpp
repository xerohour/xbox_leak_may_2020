 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#include "nvprecomp.h" // Needed for winddi etc.

#include "nvUtil.h"
#include "nvDbg.h"
#include <ctype.h>  // For tolower()
#include <math.h>

EXTERN_C void nvSpin(NvU32 loopCount)
{
    __asm
    {
    push ecx
        mov ecx,[loopCount]
xxx:
        loop xxx
    pop ecx
    }
}

//-------------------------------------------------------------------------
// NV STRING UTILITIES
//-------------------------------------------------------------------------

EXTERN_C int __cdecl nvStrCmp (char *szStr1, char *szStr2)
{
    while (*szStr1 != '\0' && *szStr2 != '\0') {
        if (*szStr1 != *szStr2) {
            return *szStr1 - *szStr2;
        }
        szStr1++;
        szStr2++;
    }
    return *szStr1 - *szStr2;
}

//-------------------------------------------------------------------------

EXTERN_C int __cdecl nvStrLen (char *szStr)
{
    int dwLen = 0;
    while (*szStr != '\0') {
        dwLen ++;
        szStr ++;
    }
    return (dwLen);
}

//-------------------------------------------------------------------------

// strlen up to a maximum of n characters

EXTERN_C int __cdecl nvStrNLen (char *szStr, int n)
{
    int dwLen = 0;

    // normal NULL terminated string
    if (n == 0) return nvStrLen(szStr);

    // length limited string
    while ((*szStr != '\0') && (dwLen < n)) {
        dwLen ++;
        szStr ++;
    }
    return (dwLen);
}

//-------------------------------------------------------------------------

// NV analog to strcpy

EXTERN_C void __cdecl nvStrCpy (char *szDst, char *szSrc)
{
    while (*szSrc != '\0') {
        *szDst = *szSrc;
        szDst ++;
        szSrc ++;
    }
    *szDst = '\0';
}

//-------------------------------------------------------------------------

// NV analog to strncpy

EXTERN_C void __cdecl nvStrNCpy (char *szDst, char *szSrc, int n)
{
    int dwCount=0;
    while ((*szSrc != '\0') && (dwCount < n)) {
        *szDst = *szSrc;
        szDst ++;
        szSrc ++;
        dwCount ++;
    }
    if (dwCount < n) *szDst = '\0';
}

//-------------------------------------------------------------------------

// NV analog to strcat

EXTERN_C void __cdecl nvStrCat (char *szStr1, char *szStr2)
{
    szStr1 += nvStrLen (szStr1);
    nvStrCpy (szStr1, szStr2);
}

//-------------------------------------------------------------------------

// NV analog to strchr

EXTERN_C char* __cdecl nvStrChr(char *szStr, NvU8 c)
{
    while (szStr[0])
    {
        if (szStr[0] == c)
            return szStr;
        szStr++;
    }
    return NULL;
}

//-------------------------------------------------------------------------

// NV analog to strrchr

EXTERN_C char* __cdecl nvStrRChr(char *szStr, unsigned char c)
{
    int dwLen = nvStrLen(szStr);
    char *szStr1 = szStr + dwLen - 1;
    while (szStr1 >= szStr)
    {
        if (szStr1[0] == c)
            return szStr1;
        szStr1--;
    }
    return NULL;
}

//-------------------------------------------------------------------------

void nvPrintPad (char **pDst, char cPad, int iLen)
{
    int i;
    for (i=0; i < iLen; i++) {
         **pDst = cPad;
         (*pDst) ++;
    }
}

//-------------------------------------------------------------------------

// print a formatted string or number (decimal or hex)
// allowed format types are: %[0][1-9]{d,u,x,c,s}

// *pDst is a pointer to the destination string
// *pFormatStr is a pointer to the format string (positioned AFTER the %)
// dwData is the numerical data or a pointer to a constant string

// at function exit, *pFormatStr and *pData will both have been advanced

typedef enum _varType {vtInt, vtDouble, vtChar, vtString, vtPercent, vtUnknown} varType;

static varType nvPrintFormattedThing (char **pDst, char **pFormatStr, NvU8 *pData)
{
    // formatting variables
    BOOL    bLeftJustify;
    char    cFillChar;
    int     iFieldSize;
    int     iPrecision;
    // things to which pData might point
    NvU32   dwData;
    double  dData;
    char    cData;
    char   *szData;
    // other helpers
    NvU32   dwBase, dwDigit, dwInt, dwFrac;
    int     iIndex, i, iStrLen, iSize;
    char    szNumStr[64], szFmt[8];
    char    *pFmt;
    float   fData;
    varType vt;

    // check for left justification
    if (**pFormatStr == '-') {
        bLeftJustify = TRUE;
        (*pFormatStr) ++;   // yes, ++ binds tighter than *. #@&!$^!
    }
    else {
        bLeftJustify = FALSE;
    }

    // determine fill character
    if (**pFormatStr == '0') {
        cFillChar = '0';
        (*pFormatStr) ++;
    }
    else {
        cFillChar = ' ';
    }

    // get field size
    if ((**pFormatStr >= '1') && (**pFormatStr <= '9')) {
        iFieldSize = (int)(**pFormatStr) - (int)('0');
        (*pFormatStr) ++;
    }
    else {
        iFieldSize = 0;
    }

    // check for precision specifier
    iPrecision = 0;
    if (**pFormatStr == '.') {
        (*pFormatStr) ++;
        if ((**pFormatStr >= '1') && (**pFormatStr <= '9')) {
            iPrecision = (int)(**pFormatStr) - (int)('0');
            (*pFormatStr) ++;
        }
    }

    switch (tolower(**pFormatStr)) {

        case 'i':
        case 'd':
        case 'u':
        case 'x':
        case 'p':
            dwData = *(NvU32 *)pData;
            if (**pFormatStr == 'd') {
                if ((int)dwData < 0) {
                    // emit the '-' sign
                    **pDst = '-';
                    (*pDst) ++;
                    dwData = -((int)dwData);
                }
            }
            dwBase = ((**pFormatStr == 'x') || (**pFormatStr == 'p')) ? 16 : 10;
            iIndex = 0;
            if (dwData) {
                while (dwData) {
                    dwDigit = dwData % dwBase;
                    szNumStr[iIndex] = (dwDigit < 10) ?
                                       '0' + (char)dwDigit :
                                       'a' + (char)dwDigit - (char)10;
                    iIndex ++;
                    dwData /= dwBase;
                }
            }
            else {
                // 0 is a special case
                szNumStr[0] = '0';
                iIndex = 1;
            }
            // left fill as needed
            if (!bLeftJustify) {
                nvPrintPad (pDst, cFillChar, iFieldSize-iIndex);
            }
            // copy the number into the destination
            for (i=iIndex-1; i>=0; i--) {
                **pDst = szNumStr[i];
                (*pDst) ++;
            }
            // right fill as needed
            if (bLeftJustify) {
                nvPrintPad (pDst, cFillChar, iFieldSize-iIndex);
            }
            vt = vtInt;
            break;

        case 'f':
            dData = *(double *)pData;
            fData = (float)dData;
            // check for special floating point values first
            if ((FP_EXP_VAL(fData) == 0) && (FP_MAN_VAL(fData) != 0)) {
                // denormal
                nvStrCpy (*pDst, "denormal  ");
                (*pDst) += nvStrLen("denormal  ");
            }
            else if (DWORD_FROM_FLOAT(fData) == FP_INFINITY_BITS) {
                // infinity
                nvStrCpy (*pDst, "infinity  ");
                (*pDst) += nvStrLen("infinity  ");
            }
            else if (DWORD_FROM_FLOAT(fData) == FP_MIN_INFINITY_BITS) {
                // infinity
                nvStrCpy (*pDst, "-infinity ");
                (*pDst) += nvStrLen("-infinity ");
            }
            else if ((FP_EXP_VAL(fData) == 0xff) && (FP_MAN_VAL(fData) != 0)) {
                // NaN
                nvStrCpy (*pDst, "NaN       ");
                (*pDst) += nvStrLen("NaN       ");
            }
            else {
                // a 'normal' floating point number
                if (fData < 0.0) {
                    // emit the '-' sign
                    **pDst = '-';
                    (*pDst) ++;
                    fData = -fData;
                }
                dwInt  = (NvU32)fData;
                dwFrac = (NvU32)(100000000.0*(fData-dwInt)+0.5);
                if (dwFrac==100000000) { dwInt+=1; dwFrac=0; }
                nvStrCpy (szFmt, "d");
                pFmt = szFmt;
                nvPrintFormattedThing (pDst, &pFmt, (NvU8 *)(&dwInt));
                **pDst = '.';
                (*pDst) ++;
                nvStrCpy (szFmt, "08d");
                pFmt = szFmt;
                nvPrintFormattedThing (pDst, &pFmt, (NvU8 *)(&dwFrac));
            }
            vt = vtDouble;
            break;

        case 'c':
            cData = *(char *)(pData);
            // left fill as needed
            if (!bLeftJustify) {
                nvPrintPad (pDst, cFillChar, iFieldSize-1);
            }
            **pDst = cData;
            (*pDst) ++;
            // right fill as needed
            if (bLeftJustify) {
                nvPrintPad (pDst, cFillChar, iFieldSize-1);
            }
            vt = vtChar;
            break;

        case 's':
            szData = *(char **)pData;
            iStrLen = (int) nvStrNLen (szData, iPrecision);
            iSize = iPrecision ? MIN(iPrecision,iStrLen) : iStrLen;
            // left fill as needed
            if (!bLeftJustify) {
                nvPrintPad (pDst, cFillChar, iFieldSize-iSize);
            }
            for (i=0; i < iSize; i++) {
                **pDst = *szData;
                (*pDst) ++;
                szData ++;
            }
            // right fill as needed
            if (bLeftJustify) {
                nvPrintPad (pDst, cFillChar, iFieldSize-iSize);
            }
            vt = vtString;
            break;

        case '%':
            **pDst = '%';
            (*pDst) ++;
            vt = vtPercent;
            break;

        default:
            DPF ("got unknown format type '%c' in nvPrintFormattedThing", **pFormatStr);
            dbgError("NV Error!");
            vt = vtUnknown;
            break;
    }

    (*pFormatStr) ++;

    return (vt);
}

//-------------------------------------------------------------------------

// NV analog to sprintf in two flavors: va_list and ...

EXTERN_C void __cdecl nvSprintfVAL (char *szDest, char *szFormat, va_list vaList)
{
    char   *pDst, *pSrc;
    varType vt;

    pDst = szDest;
    pSrc = szFormat;

    while (*pSrc != '\0') {

        if (*pSrc == '%') {
            pSrc ++;
            vt = nvPrintFormattedThing (&pDst, &pSrc, (NvU8 *)vaList);
            // advance to the next argument
            switch (vt) {
                case vtInt:
                    va_arg (vaList, int);
                    break;
                case vtDouble:
                    va_arg (vaList, double);
                    break;
                case vtChar:
                    va_arg (vaList, char);
                    break;
                case vtString:
                    va_arg (vaList, char *);
                    break;
                case vtPercent:
                    break;
                case vtUnknown:
                    *pDst = '\0';
                    return;
                default:
                    DPF ("got unknown return type from nvPrintFormattedThing");
                    dbgError("NV Error!");
                    *pDst = '\0';
                    return;
            }
        }
        else {
            *pDst = *pSrc;
            pSrc ++;
            pDst ++;
        }
    }

    *pDst = '\0';
}

EXTERN_C void __cdecl nvSprintf (char *szDest, char *szFormat, ...)
{
    va_list vaList;
    va_start (vaList, szFormat);
    nvSprintfVAL (szDest, szFormat, vaList);
    va_end (vaList);
}

#pragma warning(disable: 4035)  // no return value
EXTERN_C NvU32 nvGetCurrentProcessId()
{
#if IS_WINNT4
  __asm
  {
    mov eax, fs:[0x18]
    mov eax, [eax + 0x20] // return value in eax
  }
#elif IS_WINNT5
  return (NvU32)EngGetCurrentProcessId();
#elif IS_WIN9X
  return GetCurrentProcessId();
#endif
}
#pragma warning(default: 4035)  // no return value

EXTERN_C void nvQueryPerformanceCounter(__int64  *pPerformanceCount)
{
#if IS_WINNT4 || IS_WINNT5
  EngQueryPerformanceCounter(pPerformanceCount);
#elif IS_WIN9X
  QueryPerformanceCounter((LARGE_INTEGER *)pPerformanceCount);
#endif
}
