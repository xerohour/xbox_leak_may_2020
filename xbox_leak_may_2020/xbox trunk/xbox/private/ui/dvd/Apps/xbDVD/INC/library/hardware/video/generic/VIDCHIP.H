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


// FILE:      library\hardware\video\generic\vidchip.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   24.03.95
//
// PURPOSE: The abstract class for video decoder/encoder chip programming.
//
// HISTORY:

#ifndef VIDEOCHIP_H
#define VIDEOCHIP_H

#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "library\common\virtunit.h"
#endif

#include "vidtypes.h"


MKTAG (VID_VIDEOSTANDARD,				VIDEO_CHIP_UNIT, 0x0001, VideoStandard)
MKTAG (VID_MODE,							VIDEO_CHIP_UNIT, 0x0002, VideoMode    )
MKTAG (VID_EXTSYNC,						VIDEO_CHIP_UNIT, 0x0003, BOOL         )
MKTAG (VID_PAL_VIDEOSUBSTANDARD,		VIDEO_CHIP_UNIT, 0x0004, PALVideoSubStandard)
MKTAG (VID_NTSC_VIDEOSUBSTANDARD,	VIDEO_CHIP_UNIT, 0x0005, NTSCVideoSubStandard)


#ifndef ONLY_EXTERNAL_VISIBLE

class VideoChip : public PhysicalUnit {
	public:
		virtual Error SetVideoStandard(VideoStandard std) = 0;
		virtual Error SetMode(VideoMode mode) = 0;
		virtual Error SetExtSync(BOOL extsync) = 0;
		virtual Error SetPALVideoSubStandard(PALVideoSubStandard palSubStandard) {GNRAISE_OK;}
		virtual Error SetNTSCVideoSubStandard(NTSCVideoSubStandard ntscSubStandard) {GNRAISE_OK;}
	};

class VirtualVideoChip : public VirtualUnit {
	protected:
		VideoStandard			standard;
#if TEST_PAL_TO_NTSC
		MovingImageStandard	movingImageStandard;		// current moving Image standard
#endif
		PALVideoSubStandard	palSubStandard;
		NTSCVideoSubStandard	ntscSubStandard;
		VideoMode		mode;
		BOOL				extSync;
		VideoChip	*	chip;

		Error Preempt(VirtualUnit *previous);
	public:
		VirtualVideoChip (VideoChip *physical);
		Error Configure(TAG __far * tags);
	};



#endif // ONLY_EXTERNAL_VISIBLE




#endif
