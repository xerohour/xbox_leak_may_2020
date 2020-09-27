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

#ifndef HUFFDEC_H
#define HUFFDEC_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "..\common\bitstreams.h"

#define HUFF_PARTITION_BITS	8
#define HUFF_PARTITION_SIZE	(1 << HUFF_PARTITION_BITS)

struct HuffInit
	{
	char	*	str;
	long		val;
	};

class HuffmanTable
	{
	friend class VideoBitStream;
	private:
		class HuffXEntry
			{
			public:
				union {
						HuffXEntry	*	sub;
						long				val;} u;

				HuffXEntry(void)
					{
					u.val = 0x0f;
					}

				~HuffXEntry(void)
					{
					//if (!(u.val & 1)) delete[] u.sub;
					}

			} xroot[HUFF_PARTITION_SIZE];

		class HuffEntry
			{
			public:
				bool		final;
				bool		append;
				long		val;
				HuffEntry	*	zero;
				HuffEntry	*	one;

				HuffEntry(void)
					{
					final = FALSE;
					one = zero = NULL;
					}

				~HuffEntry(void)
					{
					if (!final)
						{
						delete zero;
						delete one;
						}
					}
				};
		HuffEntry * root;

		void BuildXRest(HuffXEntry * xentry, HuffEntry * entry, int predepth);

		void SecureXRest(HuffXEntry * xentry);

		HuffXEntry * MergeXRest(HuffXEntry * me, HuffXEntry * with);
	public:
		HuffmanTable(void);
		HuffmanTable(HuffInit * init);
		~HuffmanTable(void);

		DWORD FindSymbol(DWORD val);
		DWORD FindFlatSymbol(DWORD val);

		void AddEntry(const char * str, long val);

		void BuildXTable(void);

		void PrepareByteTable(int add);

		void Merge(HuffmanTable & table);

		void Secure(void);

		void BuildExtDCTTable(void);
		void BuildFlatDCTTable(void);
		void BuildExtPatternTable(void);

		DWORD		pad0[7];

		unsigned long extDCTTable[0x400];
		unsigned long extNIDCTTable[0x400];
		unsigned short extNIDCTTable2[0x200];
		unsigned char extPatternTable[32 + 128]; // SSCCCCCC
		unsigned long * flatDCTTable;		//	format XXX.LLLLLLLLLLLL.0 00SSSSSS TTRRRRRR
		unsigned long * flatNIDCTTable;	// format XXX.LLLLLLLLLLLL.F 00SSSSSS.TTRRRRRR
													// LLLLLLLLLLLL	: 12 bit level
													// XXX				: sign extension of level
													// F					: fraction bit for non intra values
													// SSSSSS			: size of huffman code in bits
													// TT					: 00 correct table, 10 end of block, 01 extended code
													//	RRRRRR			: run

		DWORD		pad1[6];
	};

#define VIDEO_STREAM_BUFFER_SIZE	(1 << 19)
#define VIDEO_STREAM_BUFFER_MASK	(VIDEO_STREAM_BUFFER_SIZE * 32 - 1)

extern DWORD VideoStreamBuffer[VIDEO_STREAM_BUFFER_SIZE+8];
extern int BSBits;

class VideoBitStream : public BitStream
	{
	protected:
		bool	sequenceEndFound;
	public:
		VideoBitStream(StreamFlipCopy * streamFlipCopy);

		~VideoBitStream(void);

		long GetSymbol(HuffmanTable * table);

		DWORD GetBits(int num);

		DWORD GetBit(void);

		bool GetBool(void);

		DWORD PeekBits(int num);

		DWORD PeekBit(void);

		bool PeekBool(void);

		int Peek(void);

		void Advance(int bits);

		void NextStartCode(void);

		long GetDCCoefficient(HuffmanTable * table);

		long GetCoefficient(HuffmanTable * table);

		int AddData(BYTE * ptr, int num);

		bool RefillBuffer(int requestBits = 65536, bool lowDataPanic = FALSE);

		void FlushBuffer(void);
	};


__forceinline void VideoBitStream::Advance(int bits)
	{
	BSBits += bits;
	}

#pragma warning(disable : 4035)

__forceinline int VideoBitStream::Peek(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]

		mov	ecx, eax
 		and	eax, VIDEO_STREAM_BUFFER_MASK

		shr	eax, 5

		mov	edx, [VideoStreamBuffer + 4 + eax * 4]
		mov	eax, [VideoStreamBuffer +     eax * 4]

		shld	eax, edx, cl
		}
#else
	DWORD * p = VideoStreamBuffer + ((BSBits & VIDEO_STREAM_BUFFER_MASK) >> 5);
	DWORD res = SHLD64(p[0], p[1], BSBits & 31);

	return (int)res;
#endif
	}

__forceinline DWORD VideoBitStream::PeekBits(int num)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]
		mov	ecx, [num]

 		and	eax, VIDEO_STREAM_BUFFER_MASK
		mov	ch, 32

		sub	ch, cl
		mov	cl, al

		shr	eax, 5

		mov	edx, [VideoStreamBuffer + 4 + eax * 4]
		mov	eax, [VideoStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		mov	cl, ch

		shr	eax, cl
		}
#else
	return (DWORD)(Peek()) >> (32 - num);
#endif
	}

__forceinline DWORD VideoBitStream::GetBits(int num)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]
		mov	ecx, [num]

		mov	edx, eax
		and	eax, VIDEO_STREAM_BUFFER_MASK

		add	edx, ecx
		mov	ch, 32

		sub	ch, cl
		mov	cl, al

		shr	eax, 5
		mov	[BSBits], edx

		mov	edx, [VideoStreamBuffer + 4 + eax * 4]
		mov	eax, [VideoStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		mov	cl, ch

		shr	eax, cl
		}
#else
	DWORD res;

	res = PeekBits(num);
	Advance(num);

	return res;
#endif
	}

inline DWORD HuffmanTable::FindSymbol(DWORD val)
	{
	DWORD sym;

	sym = xroot[(val >> 24) & 255].u.val;
	if (!(sym & 1))
		sym = ((HuffmanTable::HuffXEntry *)sym)[(val >> 16) & 255].u.val;

	return sym;
	}

inline DWORD HuffmanTable::FindFlatSymbol(DWORD val)
	{
	return xroot[(val >> 24) & 255].u.val;
	}

__forceinline long VideoBitStream::GetSymbol(HuffmanTable * table)
	{
#if _M_IX86
	__asm
		{
		mov	edx, [BSBits]
		mov	eax, [table]

		mov	ecx, edx
		and	edx, VIDEO_STREAM_BUFFER_MASK

		shr	edx, 5

		mov	ebx, [VideoStreamBuffer + 4 + edx * 4]
		mov	edx, [VideoStreamBuffer +     edx * 4]

		shld	edx, ebx, cl

		mov	ebx, edx
		mov	ecx, 0x000000ff

		shr	ebx, 24
		and	edx, 0x00ff0000

		shr	edx, 16
		mov	eax, [eax + 4 * ebx]

		test	eax, 1
		jne	hit1

		mov	eax, [eax + 4 * edx]
hit1:
		and	ecx, eax

		shr	ecx, 1
		mov	ebx, [BSBits]

		add	ebx, ecx

		sar	eax, 8
		mov	[BSBits], ebx
		}
#else
	DWORD res;
	DWORD sym;

	res = Peek();

	sym = table->FindSymbol(res);

	Advance((sym >> 1) & 0x7f);

	return ((int)sym) >> 8;
#endif
	}

__forceinline long VideoBitStream::GetCoefficient(HuffmanTable * table)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [table]
		mov	edx, [BSBits]

		mov	ecx, edx
		and	edx, VIDEO_STREAM_BUFFER_MASK

		shr	edx, 5

		mov	ebx, [VideoStreamBuffer + 4 + edx * 4]
		mov	edx, [VideoStreamBuffer +     edx * 4]

		shld	edx, ebx, cl

		mov	ecx, edx
		mov	ebx, edx

		shr	ecx, 24
		and	edx, 0x00ff0000

		shr	edx, 16

		mov	eax, [eax + 4 * ecx]
		mov	ecx, 0xff

		test	eax, 1
		je		notfirsthit

		test	eax, eax
		jns	firsthit

		test	eax, 0x0100
		je		firsthit

		mov	eax, ebx
		mov	edx, [BSBits]

		and	eax, 0x03ffffff
		add	edx, 24

		mov	[BSBits], edx
		jmp	escfinal

notfirsthit:
		mov	eax, [eax + 4 * edx]

firsthit:
		shr	ebx, 1
		and	ecx, eax

		shr	ecx, 1
		mov	edx, [BSBits]

		shl	ebx, cl

		sar	ebx, 31
		add	edx, ecx

		and	ebx, 0x000fff00
		mov	[BSBits], edx

		xor	eax, ebx
		and	ebx, 0x00000100

		add	eax, ebx
escfinal:
		}
#else
	DWORD * p = VideoStreamBuffer + ((BSBits & VIDEO_STREAM_BUFFER_MASK) >> 5);
	DWORD res = SHLD64(p[0], p[1], BSBits & 31);
	DWORD numbits;
	DWORD sym;

	sym = table->xroot[res & 255].u.val;
	if (sym & 1)
		{
		sym = ((HuffmanTable::HuffXEntry *)sym)[(res >> 8) & 255].u.val;
		}
	else if ((sym & 0x80000000) && (sym & 0x0100))
		{
		BSBits += 24;

		return res & 0x03ffffff;
		}

	numbits = (sym & 0xff) >> 1;

	if (res & (1 << (32 - numbits)))
		{
		sym ^= 0x000fff00;
		sym += 0x00000100;
		}

	Advance(numbits);

	return sym;
#endif
	}

__forceinline long VideoBitStream::GetDCCoefficient(HuffmanTable * table)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]
		mov	edx, [table]

		mov	ecx, eax
		and	eax, VIDEO_STREAM_BUFFER_MASK

		shr	eax, 5

		mov	ebx, [VideoStreamBuffer + 4 + eax * 4]
		mov	eax, [VideoStreamBuffer +     eax * 4]

		shld	eax, ebx, cl

		mov	ecx, eax
		mov	ebx, eax
loop1:
		shr	ebx, 32 - HUFF_PARTITION_BITS

		shl	ecx, HUFF_PARTITION_BITS
		mov	edx, [edx + 4 * ebx]

		test	edx, 1
		mov	ebx, ecx

		je		loop1

		shr	dl, 1
		test	dh, dh

		mov	cl, dl
		je		zero

		shl	eax, cl

		test	eax, eax

		mov	cl, 32
		jns	notPositive

		add	dl, dh
		sub	cl, dh

		shr	eax, cl
		jmp	signDone
zero:
		xor	eax, eax
		jmp	signDone

notPositive:

		shr	eax, 1
		mov	cl, 31

		or		eax, 0x80000000
		sub	cl, dh

		sar	eax, cl

		add	dl, dh
		add	eax, 0x00000001

signDone:
		and	edx, 0x000000ff
		mov	ebx, [BSBits]

		add	ebx, edx

		mov	[BSBits], ebx
		}
#else
	DWORD res = Peek();
	DWORD temp, sym, num, nummant;
	HuffmanTable::HuffXEntry * ltable = table->xroot;

	temp = res;
	do {
		ltable = ltable[temp >> (32 - HUFF_PARTITION_BITS)].u.sub;
		temp <<= HUFF_PARTITION_BITS;
		} while (!((DWORD)ltable & 1));

	sym = (DWORD)ltable;
	num = (sym >> 1) & 0x7f;
	nummant = (sym >> 8) & 0xff;

	if (nummant)
		{
		res <<= num;

		num += nummant;

		if (res & 0x80000000)
			{
			res >>= (32 - nummant);
			}
		else
			{
			res = (DWORD)-(int)(~res >> (32 - nummant));
			}
		}
	else
		{
		res = 0;
		}

	Advance(num);

	return res;
#endif
	}

__forceinline DWORD VideoBitStream::GetBit(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]
		mov	cl, 31

		mov	edx, eax
		and	eax, VIDEO_STREAM_BUFFER_MASK

		shr	eax, 5
		sub	cl, dl

		mov	eax, [VideoStreamBuffer + eax * 4]
		inc	edx

		shr	eax, cl

		and	eax, 1
		mov	[BSBits], edx
		}
#else
	return GetBits(1);
#endif
	}

__forceinline DWORD VideoBitStream::PeekBit(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]
		mov	cl, 31

		sub	cl, al
		and	eax, VIDEO_STREAM_BUFFER_MASK

		shr	eax, 5

		mov	eax, [VideoStreamBuffer + eax * 4]

		shr	eax, cl

		and	eax, 1
		}
#else
	return PeekBits(1);
#endif
	}

__forceinline bool VideoBitStream::GetBool(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]
		mov	cl, 31

		mov	edx, eax
		and	eax, VIDEO_STREAM_BUFFER_MASK

		shr	eax, 5
		sub	cl, dl

		mov	eax, [VideoStreamBuffer + eax * 4]
		inc	edx

		shr	eax, cl

		and	eax, 1
		mov	[BSBits], edx
		}
#else
	return GetBits(1);
#endif
	}

__forceinline bool VideoBitStream::PeekBool(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [BSBits]
		mov	cl, 31

		sub	cl, al
		and	eax, VIDEO_STREAM_BUFFER_MASK

		shr	eax, 5

		mov	eax, [VideoStreamBuffer + eax * 4]

		shr	eax, cl

		and	eax, 1
		}
#else
	return PeekBits(1);
#endif
	}

#pragma warning(default : 4035)

#endif

