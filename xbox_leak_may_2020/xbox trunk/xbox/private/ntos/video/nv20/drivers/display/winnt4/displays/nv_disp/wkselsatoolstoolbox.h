//*****************************Module*Header******************************
//
// Module Name: wkselsatoolstoolbox.h
//
// FNicklisch 09/14/2000: New, derived from toolbox.h and others
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
#ifndef _WKSELSATOOLSTOOLBOX_H
#define _WKSELSATOOLSTOOLBOX_H

#ifdef USE_WKS_ELSA_TOOLS
/*
** common code used in miniport and display driver
*/

ULONG ulStrLen(IN PSTR pString);
ULONG ulStrTrueLen(IN PSTR pString);
ULONG ulWStrLen(IN PWSTR pString);
ULONG ulWStrTrueLen(IN PWSTR pString);

PSTR  szStrCpy     (OUT PSTR  pTrg, IN const PSTR  pSrc);
PSTR  szStrCat     (OUT PSTR  pTrg, IN const PSTR  pSrc);
PSTR  szStrCpyWStr (OUT PSTR  pTrg, IN const PWSTR pSrc);
PSTR  szStrCatWStr (OUT PSTR  pTrg, IN const PWSTR pSrc);
PWSTR wszWStrCpy    (OUT PWSTR pTrg, IN const PWSTR pSrc);
PWSTR wszWStrCat    (OUT PWSTR pTrg, IN const PWSTR pSrc);
PWSTR wszWStrCpyStr (OUT PWSTR pTrg, IN const PSTR  pSrc);
PWSTR wszWStrCatStr (OUT PWSTR pTrg, IN const PSTR  pSrc);

#ifdef DBG
VOID vDbg_Check_c_ToolBox(VOID);
#else
#define vDbg_Check_c_ToolBox()
#endif

// FNicklis IA-64: Need a pointer size here
// calculate offset in 32 bit ULONG between two pointers
ULONG __inline ulPtrOffsetInByte(IN PVOID pv1, IN PVOID pv2)
{
  // FNicklis IA-64: Usage of 32 bit is save here
  return( (ULONG)((ULONG_PTR)((BYTE*)pv1 - (BYTE*)pv2)) );
}

// FNicklis 21.10.98 20:40:19: New
__inline VOID vBitsSet(IN OUT FLONG *pflValue, IN FLONG flSetBits)
{
  *pflValue |= flSetBits;
}

// FNicklis 21.10.98 20:40:19: New
__inline VOID vBitsClear(IN OUT FLONG *pflValue, IN FLONG flClearBits)
{
  *pflValue &= (~flClearBits);
}

__inline int bBitsTest(IN FLONG flValue, IN FLONG flTestBits)
{
  return ( (flValue & flTestBits) == flTestBits ); // exact match on each bit!
}

/*
** Memory allocation
*/
BOOL bAllocMem(PVOID *ppBuffer, ULONG nByte, ULONG ulTag);

BOOL bAllocMemPageAligned(IN OUT PVOID     *ppvAlign,
                      IN     ULONG      ulSize, // size to allocate
                      IN     ULONG      ulTag); // memory tag
BOOL bFreeMemPageAligned(IN OUT PVOID     *ppvAlign);

#ifdef DBG
BOOL bFreeMem(PVOID *ppBuffer, ULONG nDebugOnlyByteToClear);
#else
BOOL _bFreeMem(PVOID *ppBuffer); // one parameter only
#define bFreeMem(ppBuffer,nDebugOnlyByteToClear) _bFreeMem(ppBuffer)
#endif

__inline BOOL bRectAlloc(RECTL **pprcl, LONG c, ULONG ulTag)
{
  return (bAllocMem((PVOID *)pprcl, c*sizeof(RECTL), ulTag));
}

__inline BOOL bRectFree(RECTL **pprcl)
{
  return (bFreeMem((PVOID *)pprcl,0)); // don't know how much to free!
}

DWORD GetCurrentProcessID(VOID);
CHAR *szGetApplicationName(VOID);


#endif // USE_WKS_ELSA_TOOLS
#endif // _WKSELSATOOLSTOOLBOX_H
