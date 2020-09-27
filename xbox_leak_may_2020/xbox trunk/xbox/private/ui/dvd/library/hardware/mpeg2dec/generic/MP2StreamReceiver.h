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

#ifndef MP2STREAMRECEIVER_H
#define MP2STREAMRECEIVER_H

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"

enum MPEG2PESType
	{
	MP2PES_ELEMENTARY,
	MP2PES_DVD,
	MP2PES_MPEG
	};

#define MP2SR_SCALE_BITS		0x80000L
#define MP2SR_SCALE_BYTES		0x10000L
#define MP2SR_SCALE_WORDS		0x08000L
#define MP2SR_SCALE_DWORDS		0x04000L
#define MP2SR_SCALE_DVDPES		0x00020L

struct MPEGDataSizePair
	{
	HBPTR		data;
	DWORD		size;
	int		timeStamp;
	DWORD		pad0;
	};

#ifndef ONLY_EXTERNAL_VISIBLE

class MPEG2StreamReceiver
	{
	public:
		virtual DWORD SendData(HPTR data, DWORD size) = 0;
		virtual DWORD SendDataMultiple (MPEGDataSizePair * data, DWORD size);
			//
			// Scale factor is fixed point number 16:16 bits
			//
			// e.g. Scale = 0x8000 is in WORDs
			//
		virtual DWORD LastTransferLocation(DWORD scale = MP2SR_SCALE_BYTES) = 0;
		virtual void CompleteData(void) = 0;
		virtual void FlushData(void) {};

		virtual void PutPTS(DWORD pts) {};

		virtual MPEG2PESType NeedsPES(void) {return MP2PES_ELEMENTARY;}
		virtual Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used) {GNRAISE(GNR_UNIMPLEMENTED);}
		virtual Error VOBUBorderReached(void) {GNRAISE_OK;}
	};

inline DWORD XScaleDWord(DWORD pos, DWORD from, DWORD to) {return pos == 0xffffffff ? pos : ScaleDWord(pos, from, to);}

#endif

#endif
