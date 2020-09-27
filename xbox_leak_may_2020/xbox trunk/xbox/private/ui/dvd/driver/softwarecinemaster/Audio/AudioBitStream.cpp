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


#include "AudioBitStream.h"

int highmask32[33] = {0x00000000, 0x80000000, 0xc0000000, 0xe0000000,
		                 0xf0000000, 0xf8000000, 0xfc000000, 0xfe000000,
		                 0xff000000, 0xff800000, 0xffc00000, 0xffe00000,
		                 0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000,
		                 0xffff0000, 0xffff8000, 0xffffc000, 0xffffe000,
		                 0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00,
		                 0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0,
		                 0xfffffff0, 0xfffffff8, 0xfffffffc, 0xfffffffe,
							  0xffffffff};

DWORD		AudioStreamBuffer[AUDIO_STREAM_BUFFER_SIZE+1];
int		ABSBits;

AudioBitStream::AudioBitStream(StreamFlipCopy * streamFlipCopy)
	: BitStream(AudioStreamBuffer, AUDIO_STREAM_BUFFER_SIZE, ABSBits, streamFlipCopy)
	{
	}

AudioBitStream::~AudioBitStream(void)
	{
	}


