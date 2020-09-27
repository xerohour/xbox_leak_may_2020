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
// FILE:      library\common\prelude.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:   Central prelude for all modules, defining types and
//				  often used macros.
//
// HISTORY:
//
//


// ATTENTION: THE INCLUDE OF "RESOURCE.H" EXPERIMENTALY REMOVED : Uli (II)

#ifndef PRELUDE_H
#define PRELUDE_H



#if   LINUX
////////////////////////////////////////////////////////////////////
//
//	LINUX
//
////////////////////////////////////////////////////////////////////

// #define NULL	0

#define __far
#define __huge
#define __cdecl
#define __pascal
#define __export
#define __loadds
#define FAR
#define cdecl
#define WINAPI
#define DLLCALL

typedef unsigned int HANDLE;
typedef signed long LONG;

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;


#include <stdlib.h>
#include <malloc.h>
#include <string.h>

enum POOL_TYPE
	{
	NonPagedPool, PagedPool
	};

void * operator new (unsigned int nSize, POOL_TYPE iType);
void * operator new (unsigned int nSize);
void operator delete (void *p);

#define _fstrcpy	strcpy
#define _fstrcat	strcat
#define _fstrlen	strlen
#define _fstrcmp	strcmp
#define _fmemcpy  memcpy
#define _fmalloc	malloc
#define _ffree		free

/* just like below in the #ifdef MMXMEMORY section */
inline void * __cdecl operator new(unsigned int nSize)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

inline void * __cdecl operator new(unsigned int nSize, POOL_TYPE iType)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

inline void __cdecl operator delete(void* p, POOL_TYPE iType)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}

inline void __cdecl operator delete(void* p)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}

inline void __cdecl operator delete[](void* p)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}


#elif MACINTOSH
////////////////////////////////////////////////////////////////////
//
//	Apple Macintosh
//
////////////////////////////////////////////////////////////////////

//	You should manually define MACINTOSH and also one of:
//	TARGET_API_MAC_OS8 ||ÊTARGET_API_MAC_OSX ||ÊTARGET_API_MAC_CARBON
//	TARGET_CPU_PPC is default


//	undefine Windows-isms
#define __far
#define __huge
#define __cdecl
#define __pascal
#define __export
#define __loadds
#define WINAPI
#define DLLCALL


//	Memory management

typedef unsigned long	size_t;

enum POOL_TYPE
	{
	NonPagedPool, PagedPool
	};

void *  operator new(size_t nSize);
void *  operator new(size_t nSize, POOL_TYPE iType);
void	operator delete(void* p);

void *  operator new[](size_t nSize);
void *  operator new[](size_t nSize, POOL_TYPE iType);
void	operator delete[](void* p);

//	core MacOS includes
#include <ConditionalMacros.h>


#else
////////////////////////////////////////////////////////////////////
//
//	 Windows and XBox
//
////////////////////////////////////////////////////////////////////

//
// Disable warnings for "inline assembler", "lost debugging information"
// and "inline functions".
//
#pragma warning(disable : 4505)
#pragma warning(disable : 4704)
#pragma warning(disable : 4791)

#define DLLCALL

#if VXD_VERSION

// Take these as C includes as we are a .cpp file!
#define WANTVXDWRAPS
#define IS_32

#define NULL	0

extern "C"
	{
	#include <basedef.h>
	#include <vmm.h>
	#include <vmmreg.h>
	#include <vxdwraps.h>
	#include <debug.h>
	#include <vwin32.h>
	#include "library/support/w95vxds/vwinwrap.h"
	#include <winerror.h>
	}

extern int __cdecl _purecall(void);

#ifdef _DEBUG
#define HARDBREAK __asm int 1;
#else
#define HARDBREAK
#endif

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

#endif // VXD_VERSION



#if NT_KERNEL


#if WDM_VERSION

extern "C"
{
#include <wdm.h>
#include <windef.h>
}


#else


#ifndef CALLBACK
#define CALLBACK    __stdcall
#endif


extern "C"
{
#include <ntddk.h>
}

#endif // not WDM_VERSION branch

void * __cdecl operator new(size_t nSize, POOL_TYPE iType);
void * __cdecl operator new(size_t nSize);
void __cdecl operator delete(void *p);


#elif VXD_VERSION

enum POOL_TYPE
	{
	NonPagedPool = 0,
	PagedPool = HEAPSWAP
	};

void * __cdecl operator new(unsigned int nSize, POOL_TYPE iType);
void * __cdecl operator new(unsigned int nSize);
void __cdecl operator delete(void *p);


#else


#include <stdlib.h>
#include <malloc.h>

enum POOL_TYPE
	{
	PagedPool = 0,
	NonPagedPool = 1
	};


//void * __cdecl operator new(unsigned int nSize);
//void __cdecl operator delete(void *p);

#include <stdlib.h>
#include <malloc.h>

static inline void * __cdecl operator new(unsigned int nSize)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

static inline void * __cdecl operator new[](unsigned int nSize)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

static inline void * __cdecl operator new(unsigned int nSize, POOL_TYPE iType)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

static inline void * __cdecl operator new[](unsigned int nSize, POOL_TYPE iType)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

static inline void __cdecl operator delete(void* p, POOL_TYPE iType)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}

static inline void __cdecl operator delete(void* p)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}

static inline void __cdecl operator delete[](void* p)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}


#endif // not NT_KERNEL and not VXD_VERSION branch



#endif // not ST20LITE branch


////////////////////////////////////////////////////////////////////
//
//	SECOND PART: Common definitions
//
////////////////////////////////////////////////////////////////////



#include <xtl.h>
#include <winnt.h>
#define ASSERT(x)

#define __far
#define __huge
#define __loadds

#define _fmemcpy	memcpy
#if UNICODE
#define _fstrcpy	wcscpy
#define _fstrcat	wcscat
#define _fstrlen  wcslen
#define _fstrcmp	wcscmp
#else
#define _fstrcpy	strcpy
#define _fstrcat	strcat
#define _fstrlen	strlen
#define _fstrcmp	strcmp
#endif
#define _fmalloc	malloc
#define _ffree		free
#define _halloc(x,y)	new BYTE[x*y]
#define _hfree(x) delete[] x

typedef int			BOOL;
typedef char	 * LPSTR;

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;

#ifndef VXD_VERSION
typedef unsigned __int64	QWORD;
#endif

typedef unsigned int		UINT;
typedef signed long		LONG;


#ifndef LOBYTE
#define LOBYTE(w)	    	((BYTE)(w))
#endif

#ifndef HIBYTE
#define HIBYTE(w)     	((BYTE)(((UINT)(w) >> 8) & 0xFF))
#endif

#ifndef LOWORD
#define LOWORD(l)     	((WORD)(DWORD)(l))
#endif

#ifndef HIWORD
#define HIWORD(l)     	((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#endif

#ifndef MAKELONG
#define MAKELONG(low, high) ((DWORD)(WORD)(low) | ((DWORD)(WORD)(high) << 16))
#endif

#ifndef max
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif

#if (NT_KERNEL || VXD_VERSION) && !WDM_VERSION
struct RECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
};
#endif





#if VXD_VERSION

#define TCHAR	char
#define __TEXT(x)	x

#endif



//
// Some extended definitions for word/dword arithmetic
//
#ifndef MAKEWORD
#define MAKEWORD(low, high) ((WORD)(BYTE)(low) | ((WORD)(BYTE)(high) << 8))
#endif

#define MAKELONG4(low, lmid, hmid, high) ((DWORD)(BYTE)(low) | ((DWORD)(BYTE)(lmid) << 8) | ((DWORD)(BYTE)(hmid) << 16) | ((DWORD)(BYTE)(high) << 24))
#define LBYTE0(w) ((BYTE)((w) & 0xff))
#define LBYTE1(w) ((BYTE)(((DWORD)(w) >> 8) & 0xff))
#define LBYTE2(w) ((BYTE)(((DWORD)(w) >> 16) & 0xff))
#define LBYTE3(w) ((BYTE)(((DWORD)(w) >> 24) & 0xff))

inline DWORD FLIPENDIAN(DWORD x)
	{
	return MAKELONG4(LBYTE3(x), LBYTE2(x), LBYTE1(x), LBYTE0(x));
	}

//
// Some standard pointer types
//

typedef void 		 	*	APTR;			// local pointer
typedef void __far	*	FPTR;			// far pointer
typedef void __huge	*	HPTR;			//	huge pointer
typedef BYTE __huge 	*	HBPTR;		// huge byte pointer



//
// We need a memory copy for huge memory; will use the windows version
// when available.
//

#if NT_KERNEL

#define _hmemcpy RtlMoveMemory

#else

#if _WINDOWS

#define _hmemcpy memcpy

#else


inline void _hmemcpy(HPTR dst, HPTR src, DWORD size)
	{
	DWORD i, quad, rest;
	quad = size / 4;
	rest = size % 4;

	for(i=0;i<quad;i++) *((DWORD __huge * &)dst)++ = *((DWORD __huge * &)src)++;
	for(i=0;i<rest;i++) *((BYTE __huge * &)dst)++ = *((BYTE __huge * &)src)++;
	}


#endif // not _WINDOWS branch

#endif // not NT_KERNEL branch



//
// Some more stuff
//
typedef BOOL				BIT;

#define HIGH				TRUE
#define LOW					FALSE

#if !NT_KERNEL && !VXD_VERSION
#ifndef FP_OFF
#define FP_OFF(x)			LOWORD(x)
#define FP_SEG(x)			HIWORD(x)
#endif
#endif



//
// Flag construction macro
//
#define MKFLAG(x)		(1UL << (x))

//
// DWORD Bitfield construction and extraction functions
//
inline DWORD MKBF(int bit, int num, DWORD val) {return (((DWORD)val & ((1UL << num) -1)) << bit);}
inline DWORD MKBF(int bit, BOOL val) {return (val ? (1UL << bit) : 0);}
inline DWORD XTBF(int bit, int num, DWORD bf) {return ((bf >> bit) & ((1UL << num) -1));}
inline BOOL XTBF(int bit, DWORD bf) {return ((bf & (1UL << bit)) != 0);}
inline DWORD WRBF(DWORD bf, int bit, int num, DWORD val)
	{
	DWORD mask = ((1UL << num)-1) << bit;
	return (bf & ~mask) | ((val << bit) & mask);
	}
inline DWORD WRBF(DWORD bf, int bit, BOOL val) {return (val ? (bf | (1UL << bit)) : (bf & ~(1UL << bit)));}

//
// WORD Bitfield construction and extraction functions
//
inline WORD MKBFW(int bit, int num, WORD val) {return (((WORD)val & ((1 << num) -1)) << bit);}
inline WORD MKBFW(int bit, BOOL val) {return (val ? (1 << bit) : 0);}
inline WORD XTBFW(int bit, int num, WORD bf) {return ((bf >> bit) & ((1 << num) -1));}
inline BOOL XTBFW(int bit, WORD bf) {return ((bf & (1 << bit)) != 0);}
inline WORD WRBFW(WORD bf, int bit, int num, WORD val)
	{
	WORD mask = ((1 << num)-1) << bit;
	return (bf & ~mask) | ((val << bit) & mask);
	}
inline WORD WRBFW(WORD bf, int bit, BOOL val) {return (val ? (bf | (1 << bit)) : (bf & ~(1 << bit)));}

//
// BYTE Bitfield construction and extraction functions
//
inline BYTE MKBFB(int bit, int num, BYTE val) {return (((BYTE)val & ((1 << num) -1)) << bit);}
inline BYTE MKBFB(int bit, BOOL val) {return (val ? (1 << bit) : 0);}
inline BYTE XTBFB(int bit, int num, BYTE bf) {return ((bf >> bit) & ((1 << num) -1));}
inline BOOL XTBFB(int bit, BYTE bf) {return ((bf & (1 << bit)) != 0);}
inline BYTE WRBFB(BYTE bf, int bit, int num, BYTE val)
	{
	BYTE mask = ((1 << num)-1) << bit;
	return (bf & ~mask) | ((val << bit) & mask);
	}
inline BYTE WRBFB(BYTE bf, int bit, BOOL val) {return (val ? (bf | (1 << bit)) : (bf & ~(1 << bit)));}

//
// Find the most significate one bit or the least significant one bit in a double word
//
inline int FindMSB(DWORD bf) {int i; for(i=31; i>=0; i--) {if (XTBF(31, bf)) return i; bf <<= 1;} return -1;}
inline int FindLSB(DWORD bf) {int i; for(i=0; i<=31; i++) {if (XTBF( 0, bf)) return i; bf >>= 1;} return 32;}



//
// Scaling of values of WORD range to values of WORD range
//
inline WORD ScaleWord(WORD op, WORD from, WORD to) {return (WORD)((DWORD)op * (DWORD) to / (DWORD) from);}
	DWORD ScaleDWord(DWORD op, DWORD from, DWORD to);
	long ScaleLong(long op, long from, long to);
	void MUL32x32(DWORD op1, DWORD op2, DWORD __far & upper, DWORD __far & lower);
	DWORD DIV64x32(DWORD upper, DWORD lower, DWORD op);
//
// Most values in drivers etc. are scaled in a range from 0 to 10000, this functions transfer from and
// to this range
//
inline WORD ScaleFrom10k(WORD op, WORD to) {return ScaleWord(op, 10000, to);}
inline BYTE ScaleByteFrom10k(WORD op, BYTE to) {return (BYTE)ScaleWord(op, 10000, to);}
inline WORD ScaleTo10k(WORD op, WORD from) {return ScaleWord(op, from, 10000);}



#ifndef ONLY_EXTERNAL_VISIBLE

#define FAND(x, y) ScaleWord(x, 10000, y)
#define FAND3(x, y, z) FAND(FAND(x, y), z)

//
// Fix a value inside a boundary
//
inline DWORD BoundTo(DWORD op, DWORD lower, DWORD upper) {if (op<lower) return lower; else if (op>upper) return upper; else return op;}

#if !NT_KERNEL && !VXD_VERSION
//
// Alloc Dos (real) and Windows (protected) memory
//
void AllocDouble(FPTR __far &rmode, FPTR __far &pmode, DWORD size);

//
// Free this memory
//
void FreeDouble(FPTR rmode, FPTR pmode);
#endif

#endif // ONLY_EXTERNAL_VISIBLE



#endif // PRELUDE_H
