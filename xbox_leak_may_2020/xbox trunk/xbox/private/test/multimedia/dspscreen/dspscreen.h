/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

	dspserver.h

--*/

#ifndef __DSPSCREEN_H__
#define __DSPSCREEN_H__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <xtl.h>
#include <xdbg.h>

#define STUCK_THRESHOLD (DOUBLE) 0.006  // seconds
#define MAX_EP_IDLE_CYCLES  709333 // 256 samples at 133Mhz

#define EP_ROM_SIZE     0x800   //dwords
#define EP_ROM_CRC      0x9d00  //known good crc

EXTERN_C BOOL XTestAudioDspScreening(LPDIRECTSOUND pDSound,DOUBLE maxTime);
EXTERN_C BOOL XTestAudioDspScreeningCRC(WORD *pwCrc, WORD *pwXramCrc, WORD *pwCopyXramCrc);

#endif
