//
// INTEL CONFIDENTIAL.  Copyright (c) 1997-2000 Intel Corp. All rights reserved.
//
#include "..\stdafx.h"
//
// Used by the MFC framework to help find memory leaks.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable:4035)	// no return value


//******************************************************************************
// Function name	: AsmDwordMemCmp
// Description	    : Compares two buffers for equality.  Maintains possible
//                    misalignment of buffers.
// Return type		: LPDWORD - virtual address of failing DWORD in destination buffer.
// Argument         : LPVOID pDst   - destination buffer
// Argument         : LPVOID pSrc   - source buffer
// Argument         : DWORD  dwLen  - length of buffers.
//******************************************************************************
LPDWORD AsmDwordMemCmp(LPVOID pDst,LPVOID pSrc,DWORD dwLen)
{
#if defined(WIN64)
	LPDWORD s = (LPDWORD)pSrc;
	LPDWORD d = (LPDWORD)pDst;

	while(dwLen--)
		if(*s++ != *d++)
			return --d;

	return NULL;
#else
	_asm
	{
			pushfd
			cld
			mov		edi,pDst
			mov		esi,pSrc
			mov		ecx,dwLen
	repe	cmpsd
			jne		failed		
			mov		edi,4	// return zero if successful
failed:		sub		edi,4	// edi contains 4 (success) or failing address + 4
			mov		eax,edi
			popfd
	}
#endif
}


//******************************************************************************
// Function name	: AsmWordMemCmp
// Description	    : Compares two buffers for equality.  Maintains possible
//                    misalignment of buffers.
// Return type		: LPWORD - virtual address of failing WORD in destination buffer.
// Argument         : LPVOID pDst   - destination buffer
// Argument         : LPVOID pSrc   - source buffer
// Argument         : DWORD  dwLen  - length of buffers.
//******************************************************************************
LPWORD AsmWordMemCmp(LPVOID pDst,LPVOID pSrc,DWORD dwLen)
{
#if defined(WIN64)
	LPWORD s = (LPWORD)pSrc;
	LPWORD d = (LPWORD)pDst;

	while(dwLen--)
		if(*s++ != *d++)
			return --d;

	return NULL;
#else
	_asm
	{
			pushfd
			cld
			mov		edi,pDst
			mov		esi,pSrc
			mov		ecx,dwLen
	repe	cmpsw
			jne		failed		
			mov		edi,2	// return zero if successful
failed:		sub		edi,2	// edi contains 2 (success) or failing address + 2
			mov		eax,edi
			popfd
	}
#endif
}


//******************************************************************************
// Function name	: AsmByteMemCmp
// Description	    : Compares two buffers for equality.
// Return type		: LPWORD - virtual address of failing BYTE in destination buffer.
// Argument         : LPVOID pDst   - destination buffer
// Argument         : LPVOID pSrc   - source buffer
// Argument         : DWORD  dwLen  - length of buffers.
//******************************************************************************
LPBYTE AsmByteMemCmp(LPVOID pDst,LPVOID pSrc,DWORD dwLen)
{
#if defined(WIN64)
	LPBYTE s = (LPBYTE)pSrc;
	LPBYTE d = (LPBYTE)pDst;

	while(dwLen--)
		if(*s++ != *d++)
			return --d;

	return NULL;
#else
	_asm
	{
			pushfd
			cld
			mov		edi,pDst
			mov		esi,pSrc
			mov		ecx,dwLen
	repe	cmpsb
			jne		failed		
			mov		edi,1	// return zero if successful
failed:		sub		edi,1	// edi contains 4 (success) or failing address + 1
			mov		eax,edi
			popfd
	}
#endif
}
#pragma warning(default:4035)	// no return value


//******************************************************************************
// Function name	: AsmDwordMemMov
// Description	    : Copies source buffer to destination in DWORDs.  Maintains
//                    possible misalignment of buffers during transfer.
// Return type		: void
// Argument         : LPVOID pDst   - destination buffer
// Argument         : LPVOID pSrc   - source buffer
// Argument         : DWORD  dwLen  - length of buffers.
//******************************************************************************
void AsmDwordMemMov(LPVOID pDst,LPVOID pSrc,DWORD dwLen)
{
#if defined(WIN64)
	LPDWORD s = (LPDWORD)pSrc;
	LPDWORD d = (LPDWORD)pDst;

	while(dwLen--)
		*d++ = *s++;
#else
	_asm
	{
		pushfd
		mov  edi,pDst
		mov  esi,pSrc
		mov  ecx,dwLen
		cld
		rep  movsd
		popfd
	}
#endif
}


//******************************************************************************
// Function name	: AsmWordMemMov
// Description	    : Copies source buffer to destination in WORDs.  Maintains
//                    possible misalignment of buffers during transfer.
// Return type		: void
// Argument         : LPVOID pDst   - destination buffer
// Argument         : LPVOID pSrc   - source buffer
// Argument         : DWORD  dwLen  - length of buffers.
//******************************************************************************
void AsmWordMemMov(LPVOID pDst,LPVOID pSrc,DWORD dwLen)
{
#if defined(WIN64)
	LPWORD s = (LPWORD)pSrc;
	LPWORD d = (LPWORD)pDst;

	while(dwLen--)
		*d++ = *s++;
#else
	_asm
	{
		pushfd
		mov  edi,pDst
		mov  esi,pSrc
		mov  ecx,dwLen
		cld
		rep  movsw
		popfd
	}
#endif
}


//******************************************************************************
// Function name	: AsmByteMemMov
// Description	    : Copies source buffer to destination in BYTEs.
// Return type		: void
// Argument         : LPVOID pDst   - destination buffer
// Argument         : LPVOID pSrc   - source buffer
// Argument         : DWORD  dwLen  - length of buffers.
//******************************************************************************
void AsmByteMemMov(LPVOID pDst,LPVOID pSrc,DWORD dwLen)
{
#if defined(WIN64)
	LPBYTE s = (LPBYTE)pSrc;
	LPBYTE d = (LPBYTE)pDst;

	while(dwLen--)
		*d++ = *s++;
#else
	_asm
	{
		pushfd
		mov  edi,pDst
		mov  esi,pSrc
		mov  ecx,dwLen
		cld
		rep  movsb
		popfd
	}
#endif
}
