//*****************************Module*Header******************************
//
// Module Name: wkselsatoolstoolbox.c
//
// Toolbox with helper functions
//
// FNicklisch 09/14/2000: New, derived from toolbox.c
//
// This part of code was taken from the code bought from ELSA. Parts of 
// it is used to get the workstation tools, application and features up
// and running. It is implemented on a temporary base and will be replaced
// by a NVIDIA propritary interface as soon as possible.
// Don't care about the code not necessariliy used.
//
// Copyright (c) 1999-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//

// import header files
#include "precomp.h"
#include "excpt.h"
#ifdef USE_WKS_ELSA_TOOLS
#include "wkselsatoolsdebug.h"

// export header file
#include "wkselsatoolstoolbox.h"

// for shared memory issues we need some page aligned memory tiles
// to describe these, use the ALIGN_DESC struct AND the corresponding
// bAllocMemPageAligned and bFreeMemPageAligned routines in toolbox.c

// struct to describe an aligned pointer

typedef struct _ALIGN_DESC
{
  ULONG ulMagic;
  ULONG ulSizeTotal;          // size of the total allocated size
  ULONG ulSizeAligned;        // size of the aligned potrion of the mem chunk
  PVOID pvAligned;            // has to be the 2nd last element
  PVOID pvAnchor;             // has to be the last element
}
ALIGN_DESC;

//******************************Public*Data*********************************\
// MIX translation table
//
// Translates a mix 1-16, into an old style Rop 0-255.
//
//**************************************************************************/
// 220998 MSchwarz moved it here
BYTE ajMix[] =
{
    0xFF,  // R2_WHITE        - Allow rop = ajMix[mix & 0x0F]
    0x00,  // R2_BLACK
    0x05,  // R2_NOTMERGEPEN
    0x0A,  // R2_MASKNOTPEN
    0x0F,  // R2_NOTCOPYPEN
    0x50,  // R2_MASKPENNOT
    0x55,  // R2_NOT
    0x5A,  // R2_XORPEN
    0x5F,  // R2_NOTMASKPEN
    0xA0,  // R2_MASKPEN
    0xA5,  // R2_NOTXORPEN
    0xAA,  // R2_NOP
    0xAF,  // R2_MERGENOTPEN
    0xF0,  // R2_COPYPEN
    0xF5,  // R2_MERGEPENNOT
    0xFA,  // R2_MERGEPEN
    0xFF   // R2_WHITE        - Allow rop = ajMix[mix & 0xFF]
};

#if 0 // FNicklisch 14.09.00: unused
#ifdef _C_TOOLBOX_H

// enable  c_Toolbox.c
#define THIS_IS_A_VALID_TOOLBOX_C_FILE

//
// FNicklis 29.10.98:
//
// It seems to be a little bit strange, but I want to reuse the
// same code shared with the miniport.
//
#include "c_ToolBox.c"

// disable c_Toolbox.c
#undef THIS_IS_A_VALID_TOOLBOX_C_FILE
#endif //_C_TOOLBOX_H
#endif // FNicklisch 14.09.00: unused

#ifdef DBG
LONG lDbgAllocCount=0;
#endif

/*
** bAllocMem
** bFreeMem
**
** Wrapper functions to allocate and free memory where NTDDK.h 
** cannot be included
**
** Needs a pointer to a pointer to the buffer to be allocated, in both cases
**
** return: TRUE  success
**         FALSE error, bjBuffer isn't touched! Don't check for NULL then!
**
** Usage:
**   ...
**   BYTE *pjBuffer;
**
**   if ( !bAllocMem(&pjBuffer, ulSize) )
**   {
**     //  error: malloc failed!
**   }
**   else
**   {
**     // alloc succeded, do the work
**     ...
**     // free buffer again
**     bFreeMem(&pjBuffer);
**   }
**
** FNicklis 23.10.98: New
*/
// Own version!

BOOL bAllocMem(PVOID *ppBuffer, ULONG nByte, ULONG ulTag)
{
  BOOL  bRet = TRUE;
  PVOID pTemp;

  DBG_TRACE_IN(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bAllocMem);

  ASSERTDD(NULL!=ppBuffer,  "you should not call this function with invalid pointer!");
  ASSERTDD(0!=nByte,        "you should not call this function if you don't need memory!");
  ASSERTDD(0!=ulTag,        "please give me a valid ulTag, or ALLOC_TAG as default!");

  DISPDBG((DBG_LVL_FLOW2|DBG_LVL_NOPREFIX, " *ppBuffer:0x%x, nByte:%d, ulTag:0x%x",*ppBuffer,nByte,ulTag));

  pTemp = EngAllocMem(FL_ZERO_MEMORY, nByte, ulTag);

  // on error don't touch input value
  if (NULL==pTemp)
  {
    DBG_ERROR("EngAllocMem failed");
    DISPDBG((DBG_LVL_ERROR, "  didn't get %d byte, ulTag:0x%x",nByte, ulTag));
    bRet = FALSE;
    goto Exit;
  }

  #ifdef DBG
  ASSERTDD(0==((ULONG_PTR)pTemp & 3), "We need a DWORD aligned pointer!");
  lDbgAllocCount++;
  #endif

  // here is your buffer
  *ppBuffer = pTemp;

Exit:
  DBG_TRACE_OUT(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bRet);
  DISPDBG((DBG_LVL_FLOW2|DBG_LVL_NOPREFIX, " (*ppBuffer:0x%x)",*ppBuffer));
  return TRUE;
}

#ifdef DBG
BOOL bFreeMem(PVOID *ppBuffer, ULONG nDebugOnlyByteToClear)
#else
BOOL _bFreeMem(PVOID *ppBuffer)
#endif
{
  BOOL bRet=TRUE;

  DBG_TRACE_IN(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bFreeMem);
  DISPDBG((DBG_LVL_FLOW2|DBG_LVL_NOPREFIX, " *ppBuffer:0x%x, nDebugOnlyByteToClear:%d",*ppBuffer, nDebugOnlyByteToClear));

  ASSERTDD(NULL!=ppBuffer,  "you should not call this function with invalid pointer!");
  ASSERTDD(NULL!=*ppBuffer, "you should not call this function without alloctated memory!");

#ifdef DBG
  if (nDebugOnlyByteToClear)
  {
    memset(*ppBuffer, 0xCD, nDebugOnlyByteToClear);
    //RtlZeroMemory(*ppBuffer, nByteToClear);
    lDbgAllocCount--;
  }
  ASSERTDD(lDbgAllocCount>=0, "freed mored memory than allocated");
#endif

  // free and invalidate buffer
  EngFreeMem(*ppBuffer);
  *ppBuffer = NULL;

  DBG_TRACE_OUT(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bRet);
  DISPDBG((DBG_LVL_FLOW2|DBG_LVL_NOPREFIX, " (*ppBuffer:0x%x)",*ppBuffer));
  return (bRet);
}


/*
**  bAllocMemPageAligned allocates memory that begins at the start of a page 
**
**  ALIGN_DESC is the describer sturct
**  What it does:
**  - places a memory ALIGN_DESC in the gap between the aligned pointer and
**    the start of allocated memory
**  - to ensure that there is enough space to hold ALIGN_DESC it adds the size of 
**    ALIGN_DESC to the size of memory to allocate
**  - it adds one more mem page to the size to allocate cause at least W2K
**    never gives us page aligned memory and we have to use the succeeding page
**  - it allocates the chunk of memory (to get the anchor pointer)
**  - it increments this (anchor) by the size of ALIGN_DESC
**    and rounds the resulting pointer up to the begin of the next page (so we ensure that
**    there will be enough space for ALIGN_DESC) and get the aligned pointer (the result)
**  - it places the ALIGN_DESC directly before the alinged pointer so that we can
**    access it just by subtracting sizeof(ALIGN_DESC) from the align pointer
**  - it fills the ALIGN_DESC with a magic, the anchor and aligned pointer
**    and the total and aligned size
**  - it returns only the align pointer !!!
**
**  Author: Dietmar Bouge' @ ELSA AG 
**  Date  : 12.08.1999
*/

#if 0 // ELSA-FNicklis 04.07.00: unused
BOOL bAllocMemPageAligned(IN OUT PVOID     *ppvAlign, 
                          IN     ULONG      ulSize,   // size to allocate
                          IN     ULONG      ulTag)    // memory tag
{
  BOOL  bRet = TRUE;
  ALIGN_DESC *pAlignDesc;
  ULONG ulSizeTotal;
  BYTE  *pjAnchor;      // prefere BYTE type for arithmetic
  BYTE  *pjAligned;     // prefere BYTE type for arithmetic

  DBG_TRACE_IN(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bAllocMemPageAligned);
  ASSERTDD(ppvAlign != NULL, "");
  ASSERTDD(ulSize > 0, "");

  // space for the memory and our admin struct
  ulSizeTotal   = ROUND_TO_PAGES(ulSize + sizeof(ALIGN_DESC));

  // NOTE: on W2K after Beta 3 EngAllocMem returns pointers starting
  //       with an offset of 0x10, so we don't get page aligned memory
  //       just by allocating more than a page!!!
  // 
  ulSizeTotal   += PAGE_SIZE;

  pjAnchor    = (BYTE *)EngAllocMem(FL_ZERO_MEMORY, ulSizeTotal, ulTag);

  // calculate the closest possible pjAligned
  pjAligned   = pjAnchor + sizeof(ALIGN_DESC);

  // advance to next page aligned spot
  pjAligned   = (BYTE *)ROUND_TO_PAGES(pjAligned);

  // calc pAlignPtr so that pAlignPtr->pvAnchor is to be found directly before pjAligned 
  // -> &pAlignPtr->pvAnchor == (pjAligned - sizeof(PVOID))
  pAlignDesc  = (ALIGN_DESC *)(pjAligned - sizeof(ALIGN_DESC));
  ASSERTDD( (VOID *)(pjAligned - sizeof(PVOID)) == &pAlignDesc->pvAnchor, "");
  ASSERTDD((BYTE *)pAlignDesc >= pjAnchor, "");

  // FNicklis IA-64: Use macro for page size
  ASSERT(0 == (PtrToUlong(pjAligned) & PAGE_MASK));

  // on error don't touch input value
  if (NULL == pjAnchor)
  {
    DBG_ERROR("bAllocMemPageAligned failed");
    DISPDBG((DBG_LVL_ERROR, "  didn't get %d bytes, ulTag:0x%x",ulSize, ulTag));
    bRet = FALSE;
  }
  else
  {
#ifdef DBG
    lDbgAllocCount++;
#endif
    pAlignDesc->ulMagic        = 'ELSA';
    pAlignDesc->pvAnchor       = pjAnchor;
    pAlignDesc->pvAligned      = pjAligned;
    pAlignDesc->ulSizeTotal    = ulSizeTotal;
    pAlignDesc->ulSizeAligned  = ulSize;
  }
  *ppvAlign = pjAligned;

  DBG_TRACE_OUT(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bRet);
  DISPDBG((DBG_LVL_FLOW2|DBG_LVL_NOPREFIX, " *ppvAlign:0x%x",*ppvAlign));
  return bRet;
}


/*
** bFreeMemPageAligned frees page aligned memory chunks
**
**  ALIGN_DESC is the describer sturct
**  What it does:
**  - in debug builds it checks whether this pointer is valid by checking the alignment
**  - it decrements the incoming pointer by size of  ALIGN_DEST to get a pointer to the
**    ALIGN_DEST
**  - in debug builds it zeros the whole chunk of mem to avoid later use of invalid data
**  - it frees the memory using the anchor
**  - it zeros the incoming pointer
**
**  Author: Dietmar Bouge' @ ELSA AG 
**  Date  : 12.08.1999
*/ 
BOOL bFreeMemPageAligned(IN OUT PVOID     *ppvAlign) 
{
  BOOL  bRet = TRUE;
  ALIGN_DESC *pAlignDesc;
  BYTE *pjAnchor;
  ULONG ulSizeTotal;

  DBG_TRACE_IN(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bFreeMemPageAligned);
  ASSERTDD(ppvAlign != NULL, "");

  // incoming pointers have to be page aligned here
  // FNicklis IA-64: Use macro for page size
  ASSERT(0 == (PtrToUlong((BYTE *)(*ppvAlign)) & PAGE_MASK));

  pAlignDesc = (ALIGN_DESC *)(((BYTE *)(*ppvAlign)) - sizeof(ALIGN_DESC));
  ASSERTDD(pAlignDesc->ulMagic == 'ELSA', "");
  ASSERTDD(pAlignDesc->pvAligned == (*ppvAlign), "");

  pjAnchor = (BYTE *)(pAlignDesc->pvAnchor);
  ulSizeTotal = pAlignDesc->ulSizeTotal;


#ifdef DBG
  // zero out struct to avoid use of invalid data
  RtlZeroMemory(pjAnchor, ulSizeTotal);
#endif

  EngFreeMem(pjAnchor);

  // zero out struct to avoid use of invalid data
  *ppvAlign = NULL;

  DBG_TRACE_OUT(DBG_LVL_FLOW2|DBG_LVL_NOLINEFEED, bRet);
  DISPDBG((DBG_LVL_FLOW2|DBG_LVL_NOPREFIX, " *ppvAlign:0x%x",*ppvAlign));
  return bRet;
}
#endif // ELSA-FNicklis 04.07.00: unused



//
// CSchalle 30.10.98: helper routine - (disassembled NT 4.0 user mode function)
//
#if !i386 || _WIN32_WINNT >= 0x0500
typedef HANDLE (*P_PS_GET_CURRENT_PROCESS_ID)(void);
#endif

#pragma warning(disable: 4035)  // no return value
DWORD GetCurrentProcessID(VOID)
{
  #if i386 && _WIN32_WINNT >= 0x0400
  __asm
  {
    mov eax, fs:[0x18]
    mov eax, [eax + 0x20] // return value in eax
  }
  #else
  HANDLE hPID = NULL;
  HANDLE hDLL;

  hDLL = EngLoadImage(L"NTOSKRNL.EXE");
  if (hDLL)
  {
    P_PS_GET_CURRENT_PROCESS_ID pfPsGetCurrentProcessId;

    pfPsGetCurrentProcessId = (P_PS_GET_CURRENT_PROCESS_ID)EngFindImageProcAddress(hDLL, "PsGetCurrentProcessId");
    if (pfPsGetCurrentProcessId)
    {
      // does NOT work in FREE mode
      hPID = pfPsGetCurrentProcessId();
    }
    EngUnloadImage(hDLL);
  }
  return (DWORD)hPID;
  #endif

}
#pragma warning(default: 4035)  // no return value


/*
** szGetApplicationName
**
** Returns the name of the current process calling the driver
** or NULL, if not available.
** For now only in Debug mode, only on NT 4.0, only on I386
**
** 160797 KGysbers, FNicklis
*/
CHAR *szGetApplicationName(VOID)
{
  CHAR *szRet="unknown"; // default error exit

#if defined (i386)

  // Ask KGysbers for more information about the following asm code
  __asm 
  {
    mov eax,fs:[124h]
    mov eax,[eax+44h]
    add eax,1dch
    mov szRet,eax
  }

  __try 
  { // to read each byte of the string
    CHAR cDummy;
    CHAR *szHlp = szRet; // Don't destroy the original pointer

    while (*szHlp) 
    {
      cDummy = *szHlp++; // Access all bytes until we find a EOS
    }
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    // We failed, be sure there is a save exit
    DISPDBG((DBG_LVL_ERROR, "ERROR in szGetApplicationName: Exception failed with: %s",szDbg_GetExceptionCode(GetExceptionCode()) ));
    
    szRet="unknown"; // default error exit
  } 
#else
  // FNicklis IA-64: Cannot use IA-32 assembler on IA-64
  // FNicklis IA-64: ToDo - portation not complete!
  #pragma message ("  FNicklis IA-64: ToDo: szGetApplicationName "__FILE__)
  DISPDBG((DBG_LVL_IA64, "FNicklis IA-64: ToDo: szGetApplicationName %s %d",__FILE__,__LINE__));
#endif // (i386)

  // make sure, that we return a vailid pointer here!
  ASSERT(NULL!=szRet);
  ASSERT(strlen(szRet)<MAX_PATH);

  return szRet;
} // szGetApplicationName


#if 0 // FNicklisch 14.09.00: unused
ULONG ulLog2(ULONG ulValue)
{
  #if defined (i386)
    __asm 
    {
      mov ecx, [ulValue]
      bsf eax, ecx
    }
  #else
    int d = 1, iter = -1;
    do 
    {
      d *= 2;
      iter++;
    } 
    while (d <= ulValue);
    return iter;
  #endif
  // return value in eax
}
#endif // FNicklisch 14.09.00: unused

/*
** STRING FUNCTIONS
**
** The following routines are an extension or an alternate 
** inteface to the runtime library functions (Rtl*) to handle
** string, here at first of type UNICODE_STRING.
** 
** The name of each function is build using the scheme (without the blanks):
**
** E_ DestType Function [SrcType]
**
** with
** 
** DestTypes/SrcTypes:
**  UStr - UNICODE_STRING, each character of size WCHAR
**  WStr - WSTR, zero terminated string, each character of size WCHAR
**  AStr - ANSI_STRING same as STRING, each character of size CHAR
**  Str  - STR, zero terminated string, each character of size CHAR
**
** Function:
**  Len      - Returns Length in bytes 
**  TrueLen  - Returns Length in bytes including trailing 0
**  Init     - Initializes the string which includes memory allocation
**  InitMulti- Initializes the string which includes memory allocation and presetting
**  Free     - Frees the allocated mem and initializes all members to 0
**  Cat      - Concatenates a string and someting else (String, value...)
**
** All with strings handled here are delimited wihtin here with the 
** trailing zero.
**
** FN 21.01.97 New
*/

// Len returnes lenght in bytes (not including trailing zero)

ULONG ulStrLen(IN PSTR pString)
{
  PCHAR pTemp=pString;
  
  if (pTemp)
    while (*pTemp) 
      pTemp++;

  // FNicklis IA-64: Usage of 32 bit is save here
  return ((ULONG)(pTemp-pString));
}

ULONG ulWStrLen(IN PWSTR pString)
{
  PWCHAR pTemp=pString;
  
  if (pTemp)
    while (*pTemp) 
      pTemp++;

  // FNicklis IA-64: Usage of 32 bit is save here
  return ((ULONG)(pTemp-pString));
}

// TrueLen returnes lenght in bytes used by characters and trailing zero

ULONG ulStrTrueLen(IN PSTR pString)
{
  ULONG ulRet=ulStrLen(pString);

  if(ulRet)
    return (ulRet+1);
  else
    return (0);
}

ULONG ulWStrTrueLen(IN PWSTR pString)
{
  ULONG ulRet=ulWStrLen(pString);

  if(ulRet)
    return (ulRet+1);
  else
    return (0);
}


PSTR szStrCpy(OUT PSTR szTrg, IN const PSTR szSrc)
{
  RtlMoveMemory(szTrg, szSrc, ulStrTrueLen(szSrc)*sizeof(CHAR));
  return (szTrg);
}

PSTR szStrCat(OUT PSTR szTrg, IN const PSTR szSrc)
{
  return (szStrCpy(szTrg+ulStrLen(szTrg), szSrc));
}

PSTR szStrCpyWStr (OUT PSTR szTrg, IN const PWSTR wszSrc)
{
  PSTR  szRet  = szTrg;

  ASSERTDD(NULL != szTrg, "szStrCpyWStr");
  ASSERTDD(NULL != wszSrc,"szStrCpyWStr");

  if (szTrg && wszSrc) 
  {
    PWSTR wszRun = (PWSTR)wszSrc;
    while (*wszRun) 
    {
      *szTrg++ = (CHAR)(*wszRun++ & 0xff);
      ASSERTDD((szTrg-szRet)<2000, "running out of assumed limit of 2000 chars");
    }
    *szTrg = 0;
  }
  
  return szRet;
}

PSTR szStrCatWStr (OUT PSTR szTrg, IN const PWSTR wszSrc)
{
  return (szStrCpyWStr(szTrg+ulStrLen(szTrg), wszSrc));
}


PWSTR wszWStrCpy    (OUT PWSTR wszTrg, IN const PWSTR wszSrc)
{
  RtlMoveMemory(wszTrg, wszSrc, ulWStrTrueLen(wszSrc)*sizeof(WCHAR));
  return (wszTrg);
}

PWSTR wszWStrCat    (OUT PWSTR wszTrg, IN const PWSTR wszSrc)
{
  return (wszWStrCpy(wszTrg+ulWStrLen(wszTrg), wszSrc));
}

PWSTR wszWStrCpyStr (OUT PWSTR wszTrg, IN const PSTR  szSrc)
{
  PWSTR wszRet = wszTrg;

  ASSERTDD(NULL != wszTrg,"wszWStrCpyStr");
  ASSERTDD(NULL != szSrc,"wszWStrCpyStr");

  if (wszTrg && szSrc) 
  {
    PSTR szRun = (PSTR)szSrc;
    while (*szRun) 
    {
      *wszTrg++ = (WCHAR)(*szRun++ & 0xff);
      ASSERTDD((wszTrg-wszRet)<2000, "running out of assumed limit of 2000 chars");
    }
    *wszTrg = 0;
  }
  
  return wszRet;
}

PWSTR wszWStrCatStr(OUT PWSTR wszTrg, IN const PSTR  szSrc)
{
  return (wszWStrCpyStr(wszTrg+ulWStrLen(wszTrg), szSrc));
}

#if DBG
#ifndef BOOL
#define BOOL int
#endif

VOID vDbg_Check_c_ToolBox(VOID)
{
  static BOOL bFirstRun=TRUE;

  if (bFirstRun)
  {
    CHAR   sz[30];
    USHORT wsz[30];
    USHORT wsz2[30];
    
    bFirstRun=FALSE;

    // CHAR

    // staring with "012" == 3 chars
    szStrCpy(sz, "012");
    ASSERTDD(sz[0]=='0' && sz[1]=='1' && sz[2]=='2' && sz[3]=='\0', "szStrCpy not working");
    ASSERTDD(ulStrLen(sz)==3,     "ulStrLen not working");
    ASSERTDD(ulStrTrueLen(sz)==4, "ulStrTrueLen not working");

    // adding "345" -> "012345" == 6 chars
    szStrCat(sz, "345");
    ASSERTDD(sz[0]=='0' && sz[1]=='1' && sz[2]=='2' && sz[3]=='3' && sz[4]=='4' && sz[5]=='5' && sz[6]=='\0', "szStrCat not working");
    ASSERTDD(ulStrLen(sz)==6,     "ulStrLen after szStrCat not working");

    // WCHAR
    // staring with "ABC" == 3 chars
    wszWStrCpy(wsz, (const PWSTR)L"ABC");
    ASSERTDD(wsz[0]=='A' && wsz[1]=='B' && wsz[2]=='C' && wsz[3]=='\0', "wszWStrCpy not working");
    ASSERTDD(ulWStrLen(wsz)==3,     "ulWStrLen not working");
    ASSERTDD(ulWStrTrueLen(wsz)==4, "ulWStrTrueLen not working");

    // adding "DEF" -> "ABCDEF" == 6 chars
    wszWStrCat(wsz, (const PWSTR)L"DEF");
    ASSERTDD(wsz[0]=='A' && wsz[1]=='B' && wsz[2]=='C' && wsz[3]=='D' && wsz[4]=='E' && wsz[5]=='F' && wsz[6]=='\0', "wszWStrCat not working");
    ASSERTDD(ulWStrLen(wsz)==6,     "ulWStrLen after wszWStrCat not working");

    wszWStrCpy(wsz2, wsz);

    // WCHAR -> CHAR

    // staring with L"012" == 3 chars
    szStrCpyWStr(sz, (const PWSTR)L"012");
    ASSERTDD(sz[0]=='0' && sz[1]=='1' && sz[2]=='2' && sz[3]=='\0', "szStrCpyWStr not working");
    ASSERTDD(ulStrLen(sz)==3,     "ulStrLen after szStrCpyWStr not working");
    ASSERTDD(ulStrTrueLen(sz)==4, "ulStrTrueLen after szStrCpyWStr not working");

    // adding L"345" -> "012345" == 6 chars
    szStrCatWStr(sz, (const PWSTR)L"345");
    ASSERTDD(sz[0]=='0' && sz[1]=='1' && sz[2]=='2' && sz[3]=='3' && sz[4]=='4' && sz[5]=='5' && sz[6]=='\0', "szStrCatWStr not working");
    ASSERTDD(ulStrLen(sz)==6,     "ulStrLen after szStrCatWStr not working");

    // CHAR -> WCHAR
    // staring with "ABC" == 3 chars
    wszWStrCpyStr(wsz, "abc");
    ASSERTDD(wsz[0]=='a' && wsz[1]=='b' && wsz[2]=='c' && wsz[3]=='\0', "wszWStrCpyStr not working");
    ASSERTDD(ulWStrLen(wsz)==3,     "ulWStrLen after wszWStrCpyStr not working");
    ASSERTDD(ulWStrTrueLen(wsz)==4, "ulWStrTrueLen after wszWStrCpyStr not working");

    // adding "def" -> "abcdef" == 6 chars
    wszWStrCatStr(wsz, "def");
    ASSERTDD(wsz[0]=='a' && wsz[1]=='b' && wsz[2]=='c' && wsz[3]=='d' && wsz[4]=='e' && wsz[5]=='f' && wsz[6]=='\0', "wszWStrCatStr not working");
    ASSERTDD(ulWStrLen(wsz)==6,     "ulWStrLen after wszWStrCatStr not working");

    // WCHAR -> CHAR
    // adding L"abcdef" to "012345" -> "012345abcdef"
    szStrCatWStr(sz, wsz);
    ASSERTDD(ulStrLen(sz)==12,      "ulStrLen not working, final test");

    // WCHAR -> CHAR
    // adding "012345abcdef" to l"abcdef" -> l"abcdef012345abcdef"
    wszWStrCatStr(wsz, sz);
    ASSERTDD(ulWStrLen(wsz)==18,         "ulWStrLen not working, final test");
    ASSERTDD(ulWStrTrueLen(wsz)==19,     "ulWStrTrueLen not working, final test");

    ASSERTDD(wsz[ 0]=='a' && wsz[ 1]=='b' && wsz[ 2]=='c' && wsz[ 3]=='d' && wsz[ 4]=='e' && wsz[ 5]=='f', "wszWStrCatStr not working");
    ASSERTDD(wsz[ 6]=='0' && wsz[ 7]=='1' && wsz[ 8]=='2' && wsz[ 9]=='3' && wsz[10]=='4' && wsz[11]=='5', "wszWStrCatStr not working");
    ASSERTDD(wsz[12]=='a' && wsz[13]=='b' && wsz[14]=='c' && wsz[15]=='d' && wsz[16]=='e' && wsz[17]=='f', "wszWStrCatStr not working");

  }
}
#endif


#endif //USE_WKS_ELSA_TOOLS
// End of wkselsatoolstoolbox.c
