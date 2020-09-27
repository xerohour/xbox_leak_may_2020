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

#ifndef SOFTDVDPRELUDE_H
#define SOFTDVDPRELUDE_H

#include <xtl.h>
#include "library\common\prelude.h"

#define NULL	0

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned __int64 QWORD;

struct DDWORD
	{
	DWORD l, h;
	};

template<int s>
struct MMXChar
	{
	union
		{
		char		a[s];
		__int64	b;
		};

	char & operator[](int x) {return a[x];};
	};

template<int s>
struct MMXShort
	{
	union
		{
		short		a[s];
		__int64	b;
		};

	short & operator[](int x) {return a[x];};
	};

typedef MMXShort<4> MMXQShort;

template<int s>
struct MMXUShort
	{
	union
		{
		unsigned short		a[s];
		__int64	b;
		};

	unsigned short & operator[](int x) {return a[x];};
	};

typedef MMXUShort<4> MMXQUShort;

struct MMXShort8
	{
	union
		{
		short		a[8];
		__int64	b;
		};

	short & operator[](int x) {return a[x];};
	};

struct MMXShort16
	{
	union
		{
		short		a[16];
		__int64	b;
		};

	short & operator[](int x) {return a[x];};
	};


template<int s>
struct MMXInt
	{
	union
		{
		int		a[s];
		__int64	b;
		};

	int & operator[](int x) {return a[x];};
	};

template<int s>
struct MMXBYTE
	{
	union
		{
		BYTE		a[s];
		__int64	b;
		};

	BYTE & operator[](int x) {return a[x];};
	};

template<int s>
struct MMXWORD
	{
	union
		{
		WORD		a[s];
		__int64	b;
		};

	WORD & operator[](int x) {return a[x];};
	};

typedef MMXWORD<4> MMXQWORD;

template<int s>
struct MMXDWORD
	{
	union
		{
		DWORD		a[s];
		__int64	b;
		};

	DWORD & operator[](int x) {return a[x];};
	};


typedef BYTE * BPTR;

typedef int BOOL;
typedef BOOL BIT;

#define FALSE	0
#define TRUE	1

class Exception {};
class ObjectInUse : public Exception {};
class RangeViolation : public Exception {};
class ObjectNotFound : public Exception {};
class FileNotFound : public Exception {};
class EndOfFile : public Exception {};

struct PadOnQWord {__int64 x;};
struct __declspec(align(16)) PadOn16Byte {__int64 x, y;};

#pragma warning(disable : 4035)

static inline DWORD SHLD64(DWORD highval, DWORD lowval, DWORD by)
	{
#if _M_IX86
	__asm
		{
		mov	edx, [lowval]
		mov	eax, [highval]
		mov	ecx, [by]

		shld	eax, edx, cl
		}
#else
	return (high << by) | (low >> (32 - by));
#endif
	}

#pragma warning(disable : 4799)

static inline void memclr64(void * p)
	{
#if _M_IX86
	__asm
		{
		pxor	mm0, mm0
		mov	edx, p

		movq	[edx], mm0
		movq	[edx + 8], mm0
		movq	[edx + 16], mm0
		movq	[edx + 24], mm0
		movq	[edx + 32], mm0
		movq	[edx + 40], mm0
		movq	[edx + 48], mm0
		movq	[edx + 56], mm0
		}
#else
	memset(p, 0, 64);
#endif
	}

static inline void memclr128(void * p)
	{
#if _M_IX86
	__asm
		{
		pxor	mm0, mm0
		mov	edx, p

		movq	[edx], mm0
		movq	[edx + 8], mm0
		movq	[edx + 16], mm0
		movq	[edx + 24], mm0
		movq	[edx + 32], mm0
		movq	[edx + 40], mm0
		movq	[edx + 48], mm0
		movq	[edx + 56], mm0
		movq	[edx + 64], mm0
		movq	[edx + 72], mm0
		movq	[edx + 80], mm0
		movq	[edx + 88], mm0
		movq	[edx + 96], mm0
		movq	[edx + 104], mm0
		movq	[edx + 112], mm0
		movq	[edx + 120], mm0
		}
#else
	memset(p, 0, 128);
#endif
	}

static inline void memclr256(void * p)
	{
#if _M_IX86
	__asm
		{
		pxor	mm0, mm0
		mov	edx, p

		movq	[edx], mm0
		movq	[edx + 8], mm0
		movq	[edx + 16], mm0
		movq	[edx + 24], mm0
		movq	[edx + 32], mm0
		movq	[edx + 40], mm0
		movq	[edx + 48], mm0
		movq	[edx + 56], mm0
		movq	[edx + 64], mm0
		movq	[edx + 72], mm0
		movq	[edx + 80], mm0
		movq	[edx + 88], mm0
		movq	[edx + 96], mm0
		movq	[edx + 104], mm0
		movq	[edx + 112], mm0
		movq	[edx + 120], mm0
		movq	[edx + 128], mm0
		movq	[edx + 128 + 8], mm0
		movq	[edx + 128 + 16], mm0
		movq	[edx + 128 + 24], mm0
		movq	[edx + 128 + 32], mm0
		movq	[edx + 128 + 40], mm0
		movq	[edx + 128 + 48], mm0
		movq	[edx + 128 + 56], mm0
		movq	[edx + 128 + 64], mm0
		movq	[edx + 128 + 72], mm0
		movq	[edx + 128 + 80], mm0
		movq	[edx + 128 + 88], mm0
		movq	[edx + 128 + 96], mm0
		movq	[edx + 128 + 104], mm0
		movq	[edx + 128 + 112], mm0
		movq	[edx + 128 + 120], mm0
		}
#else
	memset(p, 0, 256);
#endif
	}

static inline void memclr512(void * p)
	{
	memclr256(p);
	memclr256(((BYTE *)p)+256);
	}

static inline void memclr32(void * p, int num)
	{
#if _M_IX86
	__asm
		{
		mov	ecx, num
		pxor	mm0, mm0
		mov	edx, p
		lea	ecx, [ecx * 4]
loop1:
		movq	[edx - 32 + ecx * 8], mm0
		movq	[edx - 24 + ecx * 8], mm0
		movq	[edx - 16 + ecx * 8], mm0
		movq	[edx -  8 + ecx * 8], mm0
		sub	ecx, 4
		jne	loop1
		}
#else
	memset(p, 0, 32 * num);
#endif
	}

static inline void memclr16(void * p, int num)
	{
#if _M_IX86
	__asm
		{
		mov	ecx, num
		pxor	mm0, mm0
		mov	edx, p
loop1:
		movq	[edx], mm0
		movq	[edx +  8], mm0
		add	edx, 16
		dec	ecx
		jne	loop1
		}
#else
	memset(p, 0, 16 * num);
#endif
	}

#pragma warning(default : 4799)
#pragma warning(default : 4035)

#endif
