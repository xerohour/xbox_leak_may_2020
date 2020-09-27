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


#include "SPUBitStream.h"

DWORD		SPUStreamBuffer[SPU_STREAM_BUFFER_SIZE+1];
int		SPUSBits;

SPUBitStream::SPUBitStream(void)
	: BitStream(SPUStreamBuffer, SPU_STREAM_BUFFER_SIZE, SPUSBits, NULL, TRUE)
	{
	}

SPUBitStream::~SPUBitStream(void)
	{
	}

BYTE SPUBitStream::GetByte(void)
	{
	BYTE b;

	b = ((BYTE *)buffer)[(bsbits & SPU_STREAM_BUFFER_MASK)>> 3];
	bsbits += 8;

	return b;
	}

void SPUBitStream::GetBytes(BYTE * buffer, int num)
	{
	while (num)
		{
		*buffer++ = GetByte();
		num--;
		}
	}

int SPUBitStream::AvailBits(void)
	{
	int avail;

	first = ((bsbits & mask) >> 3);

	if (first > last)
		avail = (size * 4 - (first - last)) * 8;
	else
		avail = (last - first) * 8;

	if (avail > lastAvail) avail = 0;
	else lastAvail = avail;

	return avail;
	}

void SPUBitStream::Advance(int bits)
	{
	bsbits += bits;
	}

int SPUBitStream::AddData(BYTE * ptr, int num)
	{
	int done;

	first = ((bsbits & mask) >> 5) << 2;

	if (first > last)
		{
		done = first-last-4;
		if (done > 0)
			{
			if (done > num) done = num;
			memcpy(((char *)buffer)+last, ptr, done);
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
			memcpy(((char *)buffer)+last, ptr, done);
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

				memcpy(buffer, ptr, num);
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
			memcpy(((char *)buffer)+last, ptr, done);
			last += done;
			}
		else
			done = 0;
		}

	buffer[size] = buffer[0];
	lastAvail = size * 32;

	return done;
	}


