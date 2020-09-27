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

#ifndef SPUBITSTREAM_H
#define SPUBITSTREAM_H


#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "..\common\bitstreams.h"

#define SPU_STREAM_BUFFER_SIZE	(1 << 14)
#define SPU_STREAM_BUFFER_MASK	(SPU_STREAM_BUFFER_SIZE * 32 - 1)

extern DWORD SPUStreamBuffer[SPU_STREAM_BUFFER_SIZE+1];
extern int SPUSBits;

class SPUBitStream	: public BitStream
	{
	public:
		SPUBitStream(void);

		virtual ~SPUBitStream(void);

		int AddData(BYTE * ptr, int num);

		void StuffBuffer(void) {}

		virtual int AvailBits(void);

		BYTE GetByte(void);

		void GetBytes(BYTE * buffer, int num);

		void Advance(int bits);

		void ScrambleBuffer(void) {}

		void DescrambleBuffer(int numBits) {}
	};


#endif
