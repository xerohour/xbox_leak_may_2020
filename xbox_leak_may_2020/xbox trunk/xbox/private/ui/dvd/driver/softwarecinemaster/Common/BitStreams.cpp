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

#include "BitStreams.h"
#include "library\common\vddebug.h"

#define CHECK_BUFFER_ENCRYPTION	0
#define DO_BUFFER_ENCRYPTION		0

void StreamFlipCopy::SwapMemory(DWORD * p, int num)
	{
	__asm
		{
		mov	ecx, [num]
		mov	eax, [p]
		test	ecx, ecx
		je		none
loop1:
		mov	edx, [eax]
		add	eax, 4

		bswap	edx

		dec	ecx
		mov	[eax-4], edx

		jne	loop1
none:
		}
	}

static void SwapDW(DWORD & dw)
	{

	__asm
		{
		mov	eax, [dw]
		mov	edx, [eax]
		bswap	edx
		mov	[eax], edx
		}
	}

#pragma warning (disable : 4799)

#include "..\common\katmaimacros.h"

void MMXStreamFlipCopy::CopySwapMemory(BYTE * d, BYTE * s, int num)
	{
	int startAllignment, startRest;
	int sourceAllignment;
	int finalRest;
	int sll, slr;

	startAllignment = (int)d & 7;

	if (startAllignment)
		{
		startRest = 8 - startAllignment;
		if (num >= startRest)
			{
			memcpy(d, s, startRest);
			SwapDW(((DWORD *)(d - startAllignment))[1]);
			if (startRest > 4)
				{
				SwapDW(((DWORD *)(d - startAllignment))[0]);
				}
			d += startRest;
			s += startRest;
			num -= startRest;
			}
		else if (num > 0)
			{
			memcpy(d, s, num);
			if (startRest > 4 && startRest - num <= 4)
				{
				SwapDW(((DWORD *)(d  - startAllignment))[0]);
				}
			num = 0;
			}
		}

	if (num > 0)
		{
		sourceAllignment = (int)s & 7;
		finalRest = num & 7;
		num = num >> 3;

		if (num > 0)
			{
			if (sourceAllignment) // source not alligned
				{
				s -= sourceAllignment;

				slr = sourceAllignment * 8;
				sll = 64 - slr;

				__asm
					{
					mov			ecx, [num]
					mov			edi, [d]
					mov			eax, [s]
					movq			mm0, [eax]
					movd			mm6, [sll]
					movd			mm7, [slr]
					movq			mm2, [eax+8]
					add			eax, 16
					psrlq			mm0, mm7

					movq			mm3, mm2
					psllq			mm2, mm6

					sub			ecx, 1
					je				done

	loop1:
					por			mm0, mm2

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16
					movq			mm2, [eax]
					por			mm1, mm0

					movq			mm4, mm1
					psllw			mm1, 8
					add			eax, 8

					psrlw			mm4, 8
					dec			ecx

					movq			mm0, mm3
					por			mm1, mm4

					psrlq			mm0, mm7
					movq			mm3, mm2

					movq			[edi], mm1
					psllq			mm2, mm6

					lea			edi, [edi + 8]
					jne			loop1
	done:
					por			mm0, mm2

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16

					por			mm1, mm0

					movq			mm4, mm1
					psllw			mm1, 8

					psrlw			mm4, 8

					por			mm1, mm4
					movq			[edi], mm1
					}
				}
			else // source perfectly alligned
				{
				__asm
					{
					mov			ecx, [num]
					mov			edi, [d]
					mov			esi, [s]
	loop2:
					movq			mm0, [esi]

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16

					por			mm0, mm1

					movq			mm1, mm0
					psllw			mm0, 8

					psrlw			mm1, 8

					por			mm0, mm1

					movq			[edi], mm0

					add			esi, 8
					add			edi, 8

					sub			ecx, 1
					jne			loop2
					}
				}

			s += 8 * num + sourceAllignment;
			d += 8 * num;
			}

		if (finalRest)
			{
			memcpy(d, s, finalRest);
			if (finalRest >= 4)
				{
				SwapDW(((DWORD *)d)[0]);
				}
			}
		}
	}

void XMMXStreamFlipCopy::CopySwapMemory(BYTE * d, BYTE * s, int num)
	{
	int startAllignment, startRest;
	int sourceAllignment;
	int finalRest;
	int sll, slr;

	startAllignment = (int)d & 7;

	if (startAllignment)
		{
		startRest = 8 - startAllignment;
		if (num >= startRest)
			{
			memcpy(d, s, startRest);
			SwapDW(((DWORD *)(d - startAllignment))[1]);
			if (startRest > 4)
				{
				SwapDW(((DWORD *)(d - startAllignment))[0]);
				}
			d += startRest;
			s += startRest;
			num -= startRest;
			}
		else if (num > 0)
			{
			memcpy(d, s, num);
			if (startRest > 4 && startRest - num <= 4)
				{
				SwapDW(((DWORD *)(d  - startAllignment))[0]);
				}
			num = 0;
			}
		}

	if (num > 0)
		{
		sourceAllignment = (int)s & 7;
		finalRest = num & 7;
		num = num >> 3;

		if (num > 0)
			{
			if (sourceAllignment) // source not alligned
				{
				s -= sourceAllignment;

				slr = sourceAllignment * 8;
				sll = 64 - slr;

				__asm
					{
					mov			ecx, [num]
					mov			edi, [d]
					mov			eax, [s]
					movq			mm0, [eax]
					movd			mm6, [sll]
					movd			mm7, [slr]
					movq			mm2, [eax+8]
					add			eax, 16
					psrlq			mm0, mm7

					movq			mm3, mm2
					psllq			mm2, mm6

					sub			ecx, 1
					je				done

	loop1:
					prefetch_REGpOFF8	(1, eax, 32)
					por			mm0, mm2

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16
					movq			mm2, [eax]
					por			mm1, mm0

					movq			mm4, mm1
					psllw			mm1, 8
					add			eax, 8

					psrlw			mm4, 8
					dec			ecx

					movq			mm0, mm3
					por			mm1, mm4

					psrlq			mm0, mm7
					movq			mm3, mm2

					movntq		(mm1, edi)
					psllq			mm2, mm6

					lea			edi, [edi + 8]
					jne			loop1
	done:
					por			mm0, mm2

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16

					por			mm1, mm0

					movq			mm4, mm1
					psllw			mm1, 8

					psrlw			mm4, 8

					por			mm1, mm4
					movq			[edi], mm1
					}
				}
			else // source perfectly alligned
				{
				__asm
					{
					mov			ecx, [num]
					mov			edi, [d]
					mov			esi, [s]
	loop2:
					movq			mm0, [esi]

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16

					por			mm0, mm1

					movq			mm1, mm0
					psllw			mm0, 8

					psrlw			mm1, 8

					por			mm0, mm1

					movq			[edi], mm0

					add			esi, 8
					add			edi, 8

					sub			ecx, 1
					jne			loop2
					}
				}

			s += 8 * num + sourceAllignment;
			d += 8 * num;
			}

		if (finalRest)
			{
			memcpy(d, s, finalRest);
			if (finalRest >= 4)
				{
				SwapDW(((DWORD *)d)[0]);
				}
			}
		}
	}

void StreamFlipWordCopy::SwapMemory(DWORD *p, int num)
	{
	__asm
		{
		mov	ecx, [num]
		mov	eax, [p]
		test	ecx, ecx
		je		none
loop1:
		mov	edx, [eax]
		add	eax, 4

		ror	edx, 16

		dec	ecx
		mov	[eax-4], edx

		jne	loop1
none:
		}

	}


static void SwapWordDW(DWORD & dw)
	{

	__asm
		{
		mov	eax, [dw]
		mov	edx, [eax]
		ror	edx, 16
		mov	[eax], edx
		}
	}


void StreamFlipWordCopy::CopySwapMemory(BYTE * d, BYTE * s, int num)
	{
	int startAllignment, startRest;
	int sourceAllignment;
	int finalRest;
	int sll, slr;

	startAllignment = (int)d & 7;

	if (startAllignment)
		{
		startRest = 8 - startAllignment;
		if (num >= startRest)
			{
			memcpy(d, s, startRest);
			SwapWordDW(((DWORD *)(d - startAllignment))[1]);
			if (startRest > 4)
				{
				SwapWordDW(((DWORD *)(d - startAllignment))[0]);
				}
			d += startRest;
			s += startRest;
			num -= startRest;
			}
		else if (num > 0)
			{
			memcpy(d, s, num);
			if (startRest > 4 && startRest - num <= 4)
				{
				SwapWordDW(((DWORD *)(d  - startAllignment))[0]);
				}
			num = 0;
			}
		}

	if (num > 0)
		{
		sourceAllignment = (int)s & 7;
		finalRest = num & 7;
		num = num >> 3;

		if (num > 0)
			{
			if (sourceAllignment) // source not alligned
				{
				// not using that often so unalligned penalty not so big
				s -= sourceAllignment;

				slr = sourceAllignment * 8;
				sll = 64 - slr;

				__asm
					{
					mov			ecx, [num]
					mov			edi, [d]
					mov			eax, [s]
					movq			mm0, [eax]
					movd			mm6, [sll]
					movd			mm7, [slr]
					movq			mm2, [eax+8]
					add			eax, 16
					psrlq			mm0, mm7

					movq			mm3, mm2
					psllq			mm2, mm6

					sub			ecx, 1
					je				done

	loop1:
					por			mm0, mm2

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16
					movq			mm2, [eax]
					por			mm1, mm0

					add			eax, 8

					dec			ecx

					movq			mm0, mm3

					psrlq			mm0, mm7
					movq			mm3, mm2

					movq			[edi], mm1
					psllq			mm2, mm6

					lea			edi, [edi + 8]
					jne			loop1
	done:
					por			mm0, mm2

					movq			mm1, mm0
					pslld			mm0, 16

					psrld			mm1, 16

					por			mm1, mm0

					movq			[edi], mm1
					}
				}
			else // source perfectly alligned
				{
				__asm
					{
					mov			esi, [s]
					mov			ecx, [num]
					mov			edi, [d]
	loop2:
					movq			mm0, [esi]

					movq			mm1, mm0
					psrld			mm0, 16
					pslld			mm1, 16
					por			mm0, mm1

					movq			[edi], mm0

					add			esi, 8
					add			edi, 8

					sub			ecx, 1
					jne			loop2
					}
				}

			s += 8 * num + sourceAllignment;
			d += 8 * num;
			}

		if (finalRest)
			{
			memcpy(d, s, finalRest);
			if (finalRest >= 4)
				{
				SwapWordDW(((DWORD *)d)[0]);
				}
			}
		}

	}

#pragma warning (default : 4799)


static void SwapMemory(DWORD * p, int num)
	{
	__asm
		{
		mov	ecx, [num]
		mov	eax, [p]
		test	ecx, ecx
		je		none
loop1:
		mov	edx, [eax]
		add	eax, 4

		bswap	edx

		dec	ecx
		mov	[eax-4], edx

		jne	loop1
none:
		}
	}

BitStream::BitStream(DWORD * buffer, int size, int & mbsbits, StreamFlipCopy * streamFlipCopy, bool allTimeStamps) : bsbits(mbsbits)
	{
	this->buffer = buffer;
	this->size = size;

	mask = (32 * size) - 1;

	::InitializeCriticalSection(&lock);

#if CHECK_BUFFER_ENCRYPTION
	check = new bool[size * 4];
#endif

	if (allTimeStamps)
		minPTSDistance = 1;
	else
		minPTSDistance = 32 * size / NUM_TIME_STAMPS;

	last = 1;
	FlushBuffer();

	this->streamFlipCopy = streamFlipCopy;
	}

BitStream::~BitStream(void)
	{
//	::DeleteCriticalSection(&lock);
	DeleteCriticalSection(&lock);
	}

bool BitStream::IsBufferUnderflow(void)
	{
	int bits = bsbits;

	first = ((bits & mask) >> 5) << 2;

	if (first > last)
		return bufferStart == 0;
	else
		return FALSE;
	}

__int64 BitStream::CurrentLocation(void)
	{
	int bits = bsbits;

	first = ((bits & mask) >> 5) << 2;

	if (first > last)
		return bufferStart + (bits & mask) - size * 32;
	else
		return bufferStart + (bits & mask);
	}

__int64 BitStream::CurrentInputLocation(void)
	{
	return bufferStart + last * 8;
	}

bool BitStream::SetCurrentLocation(__int64 pos)
	{
	int ffirst;

	::EnterCriticalSection(&lock);

	ffirst = ((bsbits & mask) >> 5) << 2;

	lastAvail = size * 32;


	if (pos >= bufferStart)
		{
		pos -= bufferStart;

		bsbits = (int)(pos - bufferStart);
		}
	else
		{
		pos -= bufferStart - size * 32;

		bsbits = (int)(pos + size * 32 - bufferStart);
		}


	first = ((bsbits & mask) >> 5) << 2;
	::LeaveCriticalSection(&lock);

	return TRUE;
	}

bool BitStream::SetFillLocation(__int64 pos)
	{
	int llast;

	::EnterCriticalSection(&lock);

	lastAvail = size * 32;
	llast = last;

	if (pos >= bufferStart)
		{
		last = (int)(pos - bufferStart + 7)	>> 3;
		scrambleIn = (last & ~3);
		if (scrambleOut > last && scrambleOut <= llast)
			scrambleOut = scrambleIn;
		}
	else
		{
		bufferStart -= size * 32;

		last = (int)(pos - bufferStart + 7)	>> 3;
		scrambleIn = (last & ~3);
		if (scrambleOut < llast || scrambleOut >= last)
			scrambleOut = scrambleIn;
		}

	::LeaveCriticalSection(&lock);

	return TRUE;
	}

void BitStream::MarkResetPosition(void)
	{
	int bits = bsbits;

	first = ((bits & mask) >> 5) << 2;
	resetPosition = first;
	validResetPosition = true;
	}

void BitStream::RemoveResetPosition(void)
	{
	validResetPosition = false;
	}


void BitStream::AddTimeStamp(int stamp)
	{
	__int64 pos = CurrentInputLocation();

	if (!firstStamp ||
		 pos > prevPTSPosition + minPTSDistance ||
		 stamp > prevPTSStamp + 400)
		{
		int next = (firstStamp + 1) % NUM_TIME_STAMPS;

		if (next == lastStamp)
			lastStamp = (lastStamp + 1) % NUM_TIME_STAMPS;

		stamps[firstStamp].pos = pos;
		stamps[firstStamp].time = stamp;

		firstStamp = next;

		prevPTSPosition = pos;
		prevPTSStamp = stamp;
		}
	}

int BitStream::GetCurrentTimeStamp(void)
	{
	__int64 pos;
	int p, pp;

	pos = CurrentLocation();
	p = lastStamp;
	pp = -1;

	while (p != firstStamp && pos >= stamps[p].pos)
		{
		pp = p;
		p = (p + 1) % NUM_TIME_STAMPS;
		}

	if (pp < 0)
		return INVALID_TIME_STAMP;
	else
		{
		return stamps[pp].time;
		}
	}

int BitStream::GetTimeOfCurrentLocation(void)
	{
	__int64 pos;
	int p, pp;

	pos =  CurrentLocation();
	p =  lastStamp;
	pp = -1;

	while (p !=  firstStamp && pos >=  stamps[p].pos)
		{
		pp = p;
		p = (p + 1) % NUM_TIME_STAMPS;
		}

	if (pp < 0)
		{
//		return INVALID_TIME_STAMP;
		return (0);
		}
	else if(p == firstStamp)
		{
		return stamps[pp].time;
		}
	else
		{
		//crude interpolation of time
		__int64 deltaPos = stamps[p].pos - stamps[pp].pos;
		int deltaTime = stamps[p].time - stamps[pp].time;
		int retTime = stamps[pp].time + ScaleDWord((DWORD)(pos - stamps[pp].pos), (DWORD)deltaPos, deltaTime);
//		DP("returning 0x%08x :: pp 0x%03x - fStmp 0x%04x lStmp 0x%04x",retTime, pp, firstStamp, lastStamp);
		return (retTime);
		}
	}

int BitStream::AddData(BYTE * ptr, int num)
	{
	int done;

	::EnterCriticalSection(&lock);

	if (validResetPosition)
		first = resetPosition;
	else
		first = ((bsbits & mask) >> 5) << 2;

	if (first > last)
		{
		done = first-last-4;
		if (done > 0)
			{
			if (done > num) done = num;
			streamFlipCopy->CopySwapMemory((BYTE *)buffer + last, ptr, done);
			last += done;
			}
		else
			done = 0;
		}
	else if (first)
		{
		done = size * 4 - last;
		if (done)
			{
			if (done > num) done = num;
			streamFlipCopy->CopySwapMemory((BYTE *)buffer + last, ptr, done);
			last += done;
			}
		if (done < num)
			{
			ptr += done;
			num -= done;
			if (num > first-4) num = first-4;
			if (num > 0)
				{
				bufferStart += size * 32;

				streamFlipCopy->CopySwapMemory((BYTE *)buffer, ptr, num);
				last = num;
				done += num;
				}
			}
		else if (last == size * 4)
			{
			bufferStart += size * 32;
			last = 0;
			}
		}
	else
		{
		done = size * 4 - last - 4;
		if (done > 0)
			{
			if (done > num) done = num;
			streamFlipCopy->CopySwapMemory((BYTE *)buffer + last, ptr, done);
			last += done;
			}
		else
			done = 0;
		}

	buffer[size] = buffer[0];
	lastAvail = size * 32;

	ScrambleBuffer();

	::LeaveCriticalSection(&lock);

	return done;
	}

void BitStream::StuffBuffer(void)
	{
	::EnterCriticalSection(&lock);

	streamFlipCopy->SwapMemory(buffer + (last >> 2), ((last + 3) >> 2) - (last >> 2));
	buffer[size] = buffer[0];

	::LeaveCriticalSection(&lock);
	}

int BitStream::AvailBits(void)
	{
	int avail;

	::EnterCriticalSection(&lock);

	first = ((bsbits & mask) >> 5) << 2;

	if (first > last)
		avail = (size * 4 - (first - last)) * 8;
	else
		avail = (last - first) * 8;

	avail -= (bsbits & 0x1f);

	if (avail > lastAvail) avail = 0;
	else lastAvail = avail;

	::LeaveCriticalSection(&lock);

	return avail;
	}

int BitStream::AvailBufferSpace(void)
	{
	return size * 32 - AvailBits();
	}

void BitStream::ByteAllign(void)
	{
	bsbits += -bsbits & 7;
	}

#define SCRAMBLESTEP		2048

void BitStream::FlushBuffer(void)
	{
	int i;

	::EnterCriticalSection(&lock);

	if (bufferStart)
		{
		for(i=0; i<size; i+=4)
			{
			buffer[i+0] = 0x55555555;
			buffer[i+1] = 0x55555556;
			buffer[i+2] = 0x00000000;
			buffer[i+3] = 0x000001b7;
			}
		}
	else if (last)
		{
		last = (last + 3) >> 2;
		for(i=0; i<last; i+=4)
			{
			buffer[i+0] = 0x55555555;
			buffer[i+1] = 0x55555556;
			buffer[i+2] = 0x00000000;
			buffer[i+3] = 0x000001b7;
			}
		}

	first = last = 0;
	bsbits = 0;
	firstStamp = lastStamp = 0;
	lastAvail = size * 32;
	bufferStart = 0;
	scrambleIn = scrambleOut = 0;
	validResetPosition = false;

#if CHECK_BUFFER_ENCRYPTION
	for(i=0; i<size*4; i++)
		{
		check[i] = false;
		}
#endif

	::LeaveCriticalSection(&lock);
	}

bool BitStream::HasBufferEnoughData(int requestBits)
	{
	if (AvailBits() >= requestBits)
		return true;
	else
		return InternalIsBufferComplete();
	}

bool BitStream::RefillBuffer(int requestBits, bool lowDataPanic)
	{
	if (InternalRefillBuffer(requestBits, lowDataPanic))
		{
		DescrambleBuffer(requestBits);

		return TRUE;
		}
	else
		return FALSE;
	}

void BitStream::ScrambleBuffer(void)
	{
#if DO_BUFFER_ENCRYPTION
	int next;

	next = (scrambleIn + SCRAMBLESTEP - 1)  & ~(SCRAMBLESTEP - 1);

	if (scrambleIn > last)
		{
		while (next < size * 4)
			{
			((BYTE *)buffer)[next] ^= 0xaa;
#if CHECK_BUFFER_ENCRYPTION
			if (check[next]) __asm int 3
			check[next] = true;
#endif
			next += SCRAMBLESTEP;
			}
		next = 0;
		}

	while (next < (last & ~3))
		{
		((BYTE *)buffer)[next] ^= 0xaa;
#if CHECK_BUFFER_ENCRYPTION
		if (check[next]) __asm int 3
		check[next] = true;
#endif
		next += SCRAMBLESTEP;
		}

	scrambleIn = (last & ~3);
#endif
	}

void BitStream::DescrambleBuffer(int requestBits)
	{
#if DO_BUFFER_ENCRYPTION
	int next;

	::EnterCriticalSection(&lock);

	first = ((bsbits & mask) >> 5) << 2;

	int final = first + ((requestBits + 7) >> 3);

	if (scrambleOut < first)
		{
#if CHECK_BUFFER_ENCRYPTION
		if (scrambleOut > last || first < last) __asm int 3
#endif
		final -= size * 4;
		}

	if (last >= scrambleOut)
		{
		if (final > (last & ~3)) final = (last & ~3);
		}
	else
		{
		if (final > (last & ~3) + size * 4) final = (last & ~3) + size * 4;
		}

	if (scrambleOut < final)
		{
		next = (scrambleOut + SCRAMBLESTEP - 1)  & ~(SCRAMBLESTEP - 1);

		if (final > size * 4)
			{
			while (next < size * 4)
				{
				((BYTE *)buffer)[next] ^= 0xaa;
#if CHECK_BUFFER_ENCRYPTION
				if (!check[next]) __asm int 3
				check[next] = false;
#endif
				next += SCRAMBLESTEP;
				}
			next = 0;
			final -= size * 4;
			}

		while (next < final)
			{
			((BYTE *)buffer)[next] ^= 0xaa;
#if CHECK_BUFFER_ENCRYPTION
			if (!check[next]) __asm int 3
			check[next] = false;
#endif
			next += SCRAMBLESTEP;
			}

		scrambleOut = final;

		buffer[size] = buffer[0];
		}

	::LeaveCriticalSection(&lock);
#endif
	}

void BitStream::SetStreamFlipCopy(StreamFlipCopy * streamFlipCopy)
	{
	if (streamFlipCopy)
		this->streamFlipCopy = streamFlipCopy;
	}

void BitStream::GetStreamFlipCopy(StreamFlipCopy * & streamFlipCopy)
	{
	streamFlipCopy = this->streamFlipCopy;
	}
