////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//
// FILE:      library\common\prelude.cpp
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:
//
// HISTORY:
//
//



#if   LINUX
////////////////////////////////////////////////////////////////////
//
//	LINUX
//
////////////////////////////////////////////////////////////////////


#define MUNGWALL	0

#include <stdlib.h>
#include <malloc.h>
#include "prelude.h"

#if MUNGWALL
#include "vddebug.h"
#include <debug.h>
DWORD	memAllocated = 0;
DWORD maxAllocated = 0;
DWORD numAllocated[1024];
DWORD newAllocated[1024];
#endif

void PrintFreeMemory (void);


void MUL32x32(DWORD op1, DWORD op2, DWORD & upper, DWORD & lower)
	{
	DWORD u, l;

	__asm__ (
		".intel_syntax \n"
		"movl		%%eax, %0 \n"
		"movl		%%edx, %1 \n"
		"mul		%%edx \n"
		".att_syntax \n"
		:"=r"(l), "=r"(u)
		:"0"(l), "1"(u)
// 	:"eax", "edx"
		);

	upper = u;
	lower = l;
	}

DWORD DIV64x32(DWORD upper, DWORD lower, DWORD op)
	{
	DWORD res;

	if(op)
		{
		__asm__ (
			".intel_syntax \n"
			"movl		%%edx, %0 \n"
			"movl		%%eax, %1 \n"
			"movl		%%ecx, %2 \n"
			"divl		%%ecx \n"
			".att_syntax \n"
			:"=r"(res)
			:"r"(lower), "r"(upper)
			);
		return res;
		}
	else
		return 0;
	}

#elif MACINTOSH
////////////////////////////////////////////////////////////////////
//
//	Apple Macintosh
//
////////////////////////////////////////////////////////////////////

#include "prelude.h"


#include <stdlib.h>


void *  operator new(size_t nSize)
	{
	return malloc(nSize);
	}

void *  operator new(size_t nSize, POOL_TYPE iType)
	{
	return malloc(nSize);
	}

void	operator delete(void* p)
	{
	free(p);
	}


void *  operator new[](size_t nSize)
	{
	return malloc(nSize);
	}

void *  operator new[](size_t nSize, POOL_TYPE iType)
	{
	return malloc(nSize);
	}

void	operator delete[](void* p)
	{
	free(p);
	}


#else
////////////////////////////////////////////////////////////////////
//
//	WIN32 || NT_KERNEL || VXD_VERSION
//
////////////////////////////////////////////////////////////////////

#include "prelude.h"

#if NT_KERNEL


#ifndef POOL_ALLOCATION_TAG
// Default tag: "VddV"
#define POOL_ALLOCATION_TAG 0x56646456
#endif

void * __cdecl operator new(size_t nSize, POOL_TYPE iType)
	{
	return ExAllocatePoolWithTag(iType, nSize, (DWORD) POOL_ALLOCATION_TAG);
	}

void * __cdecl operator new(size_t nSize)
	{
	return ExAllocatePoolWithTag(NonPagedPool, nSize, (DWORD) POOL_ALLOCATION_TAG);
	}

void __cdecl operator delete(void* p)
	{
	if (p) ExFreePool(p);
	}

#endif



#if VXD_VERSION

void * __cdecl operator new(unsigned int nSize, POOL_TYPE iType)
	{
	return _HeapAllocate(nSize, iType);
	}

void * __cdecl operator new(unsigned int nSize)
	{
	return _HeapAllocate(nSize, NonPagedPool);
	}

void __cdecl operator delete(void* p)
	{
	if (p) _HeapFree(p, 0);
	}


#endif

#if VXD_VERSION || WDM_VERSION

//
// This is normally in the run time libraries, so we define it here.
// It is there to detect calls of pure virtual functions.
//

int __cdecl _purecall(void)
	{
	// Insert your own error reporting code here
	return 0;
	}

#endif // VXD_VERSION



#if WDM_VERSION

void MUL32x32(DWORD op1, DWORD op2, DWORD __far & upper, DWORD __far & lower)
	{
	unsigned __int64 ui;

	ui = (unsigned __int64) op1 * (unsigned __int64) op2;

	lower = (DWORD) ui;
	upper = (DWORD) (ui >> 32);
	}

DWORD DIV64x32(DWORD upper, DWORD lower, DWORD op)
	{
	unsigned __int64 ui;

	ui = ((unsigned __int64) upper << 32) | lower;

	return (DWORD) (ui / op);
	}

DWORD ScaleDWord(DWORD op, DWORD from, DWORD to)
	{
	unsigned __int64 ui;

	if (to && op)
		{
		ui = (unsigned __int64) op * (unsigned __int64) to;

		return (DWORD) (ui / (unsigned __int64) from);
		}
	else
		return 0;
	}

long ScaleLong(long op, long from, long to)
	{
	__int64 i;

	if (to && op)
		{
		i = (__int64) op * (__int64) to;

		return (long) (i / (__int64) from);
		}
	else
		return 0;
	}

#else

void MUL32x32(DWORD op1, DWORD op2, DWORD __far & upper, DWORD __far & lower)
	{
	DWORD u, l;

	__asm {
			mov	eax, op1
			mov	edx, op2
			mul	edx
			mov	u, edx
			mov	l, eax
			}

	upper = u;
	lower = l;
	}

DWORD DIV64x32(DWORD upper, DWORD lower, DWORD op)
	{
	DWORD res;

	if (op)
		{
		__asm {
				mov	edx, upper
				mov	eax, lower
				mov	ecx, op
				div	ecx
				mov	res, eax
				};
		return res;
		}
	else
		return 0;
	}

DWORD ScaleDWord(DWORD op, DWORD from, DWORD to)
	{
	DWORD	res;

	if (to && op)
		{
		__asm {
				mov	eax, op
				mov	edx, to
				mul	edx
				mov	ecx, from
				cmp	edx, ecx
				jge	done
				div	ecx
				mov	res, eax
			done:
				};

		return res;
		}
	else
		return 0;
	}

long ScaleLong(long op, long from, long to)
	{
	long	res;

	if (to && op)
		{
		__asm {
				mov	eax, op
				mov	edx, to
				imul	edx
				mov	ecx, from
				cmp	edx, ecx
				jge	done
				idiv	ecx
				mov	res, eax
			done:
				};

		return res;
		}
	else
		return 0;
	}

#endif	// WDM_VERSION

#endif	// of !WIN32 && !NT_KERNEL && !VXD_VERSION

