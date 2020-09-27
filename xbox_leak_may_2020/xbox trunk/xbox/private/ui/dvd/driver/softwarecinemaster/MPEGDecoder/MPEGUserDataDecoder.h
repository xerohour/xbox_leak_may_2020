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

#ifndef MPEGUSERDATADECODER_H
#define MPEGUSERDATADECODER_H

#include "Huffdec.h"

class Line21Decoder
	{
	public:
		virtual void SendLine21Data(int displayTime, BYTE * data, int size) = 0;

		virtual bool NeedsPureData(void) {return false;}
	};

class MPEGUserDataDecoder
	{
	protected:
		Line21Decoder * line21Decoder;
		VideoBitStream * bitStream;
		bool					fieldID;
	public:
		MPEGUserDataDecoder(VideoBitStream * bitStream);

		void SetLine21Decoder(Line21Decoder * line21Decoder) {this->line21Decoder = line21Decoder;}

		bool ParseUserData(void);
	};

#endif
