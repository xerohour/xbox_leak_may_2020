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

#ifndef AUDIOBITSTREAM_H
#define AUDIOBITSTREAM_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "..\common\bitstreams.h"

#define AUDIO_STREAM_BUFFER_SIZE	(1 << 15)
#define AUDIO_STREAM_BUFFER_MASK	(AUDIO_STREAM_BUFFER_SIZE * 32 - 1)

extern DWORD AudioStreamBuffer[AUDIO_STREAM_BUFFER_SIZE+1];
extern int ABSBits;

class AudioBitStream	: public BitStream
	{
	public:
		AudioBitStream(StreamFlipCopy * streamFlipCopy);

		virtual ~AudioBitStream(void);

		WORD GetWord(void);

		WORD PeekWordAt(int bitoffset);

		DWORD GetHBits(int num);

		DWORD GetBits(int num);

		DWORD Get3Bits(void);

		DWORD Get4Bits(void);

		DWORD Get5Bits(void);

		DWORD Get7Bits(void);

		DWORD GetBit(void);

		bool GetBool(void);

		DWORD PeekBits(int num);

		DWORD PeekBit(void);

		bool PeekBool(void);

		void Advance(int bits);
	};

inline void AudioBitStream::Advance(int bits)
	{
	ABSBits += bits;
	}

#pragma warning(disable : 4035)

inline WORD PeekWordAt(int bitoffset)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]
		add	eax, [bitoffset]

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		shr	eax, 5
		mov	ecx, edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		shr	eax, 32-16
		}
#else
	DWORD pos = ABSBits + bitoffset;
	DWORD * p = AudioStreamBuffer + ((pos & AUDIO_STREAM_BUFFER_MASK) >> 5);
	DWORD res = SHLD64(p[0], p[1], pos & 31);

	return (WORD)(res >> 16);
#endif
	}

inline DWORD AudioBitStream::PeekBits(int num)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]
		mov	ecx, [num]

		and	eax, AUDIO_STREAM_BUFFER_MASK
		mov	ch, 32

		sub	ch, cl
		mov	cl, al

		shr	eax, 5

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		mov	cl, ch

		shr	eax, cl
		}
#else
	DWORD * p = AudioStreamBuffer + ((ABSBits & AUDIO_STREAM_BUFFER_MASK) >> 5);
	DWORD res = SHLD64(p[0], p[1], ABSBits & 31);

	return res >> (32 - num);
#endif
	}

inline DWORD AudioBitStream::GetBits(int num)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]
		mov	ecx, [num]

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		add	edx, ecx
		mov	ch, 32

		sub	ch, cl
		mov	cl, al

		shr	eax, 5
		mov	[ABSBits], edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		mov	cl, ch

		shr	eax, cl
		}
#else
	DWORD res = PeekBits(num);

	ABSBits += num;

	return res;
#endif
	}

inline WORD AudioBitStream::GetWord(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		mov	ecx, edx
		add	edx, 16

		shr	eax, 5
		mov	[ABSBits], edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		shr	eax, 32-16
		}
#else
	return (WORD)GetBits(16);
#endif
	}

inline DWORD AudioBitStream::Get3Bits(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		mov	ecx, edx
		add	edx, 3

		shr	eax, 5
		mov	[ABSBits], edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		shr	eax, 32-3
		}
#else
	return GetBits(3);
#endif
	}

inline DWORD AudioBitStream::Get4Bits(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		mov	ecx, edx
		add	edx, 4

		shr	eax, 5
		mov	[ABSBits], edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		shr	eax, 32-4
		}
#else
	return GetBits(4);
#endif
	}

inline DWORD AudioBitStream::Get5Bits(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		mov	ecx, edx
		add	edx, 5

		shr	eax, 5
		mov	[ABSBits], edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		shr	eax, 32-5
		}
#else
	return GetBits(5);
#endif
	}

inline DWORD AudioBitStream::Get7Bits(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		mov	ecx, edx
		add	edx, 7

		shr	eax, 5
		mov	[ABSBits], edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		shr	eax, 32-7
		}
#else
	return GetBits(7);
#endif
	}

extern int highmask32[33];

inline DWORD AudioBitStream::GetHBits(int num)
	{
#if _M_IX86
	__asm
		{
		mov	ecx, [ABSBits]
		mov	eax, AUDIO_STREAM_BUFFER_MASK

		and	eax, ecx
		mov	edx, [num]

		shr	eax, 5
		mov	ebx, [highmask32 + 4 * edx]

		add	edx, ecx

		mov	[ABSBits], edx

		mov	edx, [AudioStreamBuffer + 4 + eax * 4]
		mov	eax, [AudioStreamBuffer +     eax * 4]

		shld	eax, edx, cl

		and	eax, ebx
		}
#else
	return GetBits(num) << (32 - num);
#endif
	}

inline DWORD AudioBitStream::GetBit(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]
		mov	cl, 31

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		shr	eax, 5
		sub	cl, dl

		mov	eax, [AudioStreamBuffer + eax * 4]
		inc	edx

		shr	eax, cl

		and	eax, 1
		mov	[ABSBits], edx
		}
#else
	return GetBits(1);
#endif
	}

inline DWORD AudioBitStream::PeekBit(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]
		mov	cl, 31

		sub	cl, al
		and	eax, AUDIO_STREAM_BUFFER_MASK

		shr	eax, 5

		mov	eax, [AudioStreamBuffer + eax * 4]

		shr	eax, cl

		and	eax, 1
		}
#else
	return PeekBits(1);
#endif
	}

inline bool AudioBitStream::GetBool(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]
		mov	cl, 31

		mov	edx, eax
		and	eax, AUDIO_STREAM_BUFFER_MASK

		shr	eax, 5
		sub	cl, dl

		mov	eax, [AudioStreamBuffer + eax * 4]
		inc	edx

		shr	eax, cl

		and	eax, 1
		mov	[ABSBits], edx
		}
#else
	return GetBits(1);
#endif
	}

inline bool AudioBitStream::PeekBool(void)
	{
#if _M_IX86
	__asm
		{
		mov	eax, [ABSBits]
		mov	cl, 31

		sub	cl, al
		and	eax, AUDIO_STREAM_BUFFER_MASK

		shr	eax, 5

		mov	eax, [AudioStreamBuffer + eax * 4]

		shr	eax, cl

		and	eax, 1
		}
#else
	return PeekBits(1);
#endif
	}

#pragma warning(default : 4035)

#endif
