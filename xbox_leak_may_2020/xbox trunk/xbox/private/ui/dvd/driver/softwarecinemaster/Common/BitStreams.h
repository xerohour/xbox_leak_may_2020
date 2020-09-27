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

#ifndef BITSTREAMS_H
#define BITSTREAMS_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"

#define NUM_TIME_STAMPS			128
#define INVALID_TIME_STAMP		0x80000000

class StreamFlipCopy
	{
	public:
		virtual void CopySwapMemory(BYTE * d, BYTE * s, int num) = 0;
		virtual void SwapMemory(DWORD * p, int num);
	};

class MMXStreamFlipCopy : public StreamFlipCopy
	{
	public:
		virtual void CopySwapMemory(BYTE * d, BYTE * s, int num);
	};

class XMMXStreamFlipCopy : public StreamFlipCopy
	{
	public:
		void CopySwapMemory(BYTE * d, BYTE * s, int num);
	};

class StreamFlipWordCopy : public StreamFlipCopy
	{
	public:
		void CopySwapMemory(BYTE * d, BYTE * s, int num);
		void SwapMemory(DWORD * p, int num);
	};

class BitStream
	{
	protected:
		DWORD				*	buffer;
		volatile	int	&	bsbits;

		__int64			bufferStart;
		int				size;
		int				first, last, scrambleIn, scrambleOut;
		int				mask;
		int				lastAvail;
		bool				*	check;

		int				resetPosition;
		bool				validResetPosition, flushed;

		struct
			{
			__int64	pos;
			int		time;
			} stamps[NUM_TIME_STAMPS];

		int				firstStamp, lastStamp;
		int				minPTSDistance;
		__int64			prevPTSPosition;
		int				prevPTSStamp;

		StreamFlipCopy	*	streamFlipCopy;

		CRITICAL_SECTION	lock;

		virtual bool InternalRefillBuffer(int requestBits, bool lowDataPanic) = 0;
		virtual bool InternalIsBufferComplete(void) = 0;
	public:
		BitStream(DWORD * buffer, int size, int & mbsbits, StreamFlipCopy * streamFlipCopy, bool allTimeStamps = FALSE);
		virtual ~BitStream(void);

		virtual int AddData(BYTE * ptr, int num);

		virtual void StuffBuffer(void);

		virtual int AvailBits(void);

		virtual int AvailBufferSpace(void);

		virtual bool RefillBuffer(int requestBits = 65536, bool lowDataPanic = FALSE);

		virtual bool HasBufferEnoughData(int requestBits);
		virtual bool IsBufferUnderflow(void);

		virtual void FlushBuffer(void);

		void ByteAllign(void);

		virtual __int64 CurrentLocation(void);
		virtual __int64 CurrentInputLocation(void);

		virtual bool SetCurrentLocation(__int64 pos);
		virtual bool SetFillLocation(__int64 pos);

		virtual void MarkResetPosition(void);
		virtual void RemoveResetPosition(void);

		virtual void AddTimeStamp(int stamp);

		virtual int GetCurrentTimeStamp(void);
		virtual int GetTimeOfCurrentLocation(void);

		virtual void ScrambleBuffer(void);

		virtual void DescrambleBuffer(int numBits);

		virtual void SetStreamFlipCopy(StreamFlipCopy * streamFlipCopy);
		virtual void GetStreamFlipCopy(StreamFlipCopy * & streamFlipCopy);
	};

#endif
