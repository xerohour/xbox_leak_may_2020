
 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/***************************** EDID includes *******************************\
*                                                                           *
* Module: EDID.H                                                            *
*   This module contains structure and prototype definitions used in the    *
*   EDID utility functions.                                                 *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#ifndef _EDID_H_
#define _EDID_H_

RM_STATUS StopDDC1(PHWINFO, U032, U032);
RM_STATUS IsMonitorDDC(PHWINFO, U032, U032, U008 *);
RM_STATUS edidReadDevEDID(PHWINFO, U032, U008 *);
BOOL edidLogicalEDIDIsValid(U008 **, U032 *, U032 *);
RM_STATUS EDIDRead(PHWINFO, U032, U032);
RM_STATUS EDIDGetMaxRefreshRate(PHWINFO, U032, U032, U032 *);
RM_STATUS EDIDIsMonitorGTF (PHWINFO, BOOL *);
RM_STATUS ReadVDIF(PHWINFO, U032, U032, U008 *, U008, U008);
RM_STATUS EDIDCopy(PHWINFO, U008 *, U032 *, U032);
RM_STATUS ParseEDID2(PHWINFO, U032);
RM_STATUS ParseEDID12(PHWINFO, U032);
RM_STATUS ParseEDID11(PHWINFO, U032);
RM_STATUS fpParseEDID(PHWINFO, U032);
U032 edidParseDetailedTimingBlock(PHWINFO, U008, PDACFPTIMING, U032);
U032 edidParseDetailed1(PHWINFO, U008, PDACFPTIMING, U032);
U032 edidParseDetailed2(PHWINFO, U008, PDACFPTIMING, U032);
U032 edidGetVersion(PHWINFO, U032);
BOOL edidIsAnalogDisplay(PHWINFO, U032); 
BOOL edidIsDigitalDisplay(PHWINFO, U032); 
RM_STATUS EDIDDetect(PHWINFO, U032, U032);
RM_STATUS edidConstructMobileInfo(PHWINFO, U032, U008 *);
U032 getI2CPort(PHWINFO, U032, U032 );
U032 edidGetDDCCaps(PHWINFO);

#define DDC_RETRIES     3
#define NUM_DET_TIM		4

// DDC device address
#define DDC_CRT_ADR1 0x50
#define DDC_CRT_ADR2 0x51
#define DDC_CRT_ADR3 0x53

// EDID versions
#define EDID_VERSION_10	0x10
#define EDID_VERSION_11	0x11
#define EDID_VERSION_12	0x12
#define EDID_VERSION_13	0x13
#define EDID_VERSION_20	0x20
#define EDID_VERSION_14	0x14    // non-existant, but we should work if this gets added

// Flags byte, sync polarities (bits 2,1)
#define VSYNC_POS	1<<2
#define VSYNC_NEG	0
#define HSYNC_POS	1<<1
#define HSYNC_NEG	0

// EDID Version 1 header defines.
#define EDID_V1_SIZE 128
#define EDID_V1_VENDOR_ID_INDEX 0x08
#define EDID_V1_VERSION_NUMBER_INDEX  0x12
#define EDID_V1_REVISION_NUMBER_INDEX 0x13
#define EDID_V1_VIDEO_INPUT_DEF_INDEX 0x14
#define EDID_V1_FEATURE_SUPPORT_INDEX        0x18
#define EDID_V1_ESTABLISHED_TIMING_1_INDEX   0x23
#define EDID_V1_ESTABLISHED_TIMING_2_INDEX   0x24
#define EDID_V1_ESTABLISHED_TIMING_3_INDEX   0x25
#define EDID_V1_STANDARD_TIMING_1_INDEX      0x26
#define EDID_V1_NUM_STANDARD_TIMINGS        0x36-0x26
#define EDID_ESTABLISHED_TIMING_640x480_ANY 0x38
#define EDID_ESTABLISHED_TIMING_640x480_60  0x20
#define EDID_ESTABLISHED_TIMING_640x480_67  0x10
#define EDID_ESTABLISHED_TIMING_640x480_72  0x08
#define EDID_ESTABLISHED_TIMING_640x480_75  0x04
#define EDID_ESTABLISHED_TIMING_800x600_ANY 0xC0
#define EDID_ESTABLISHED_TIMING_800x600_56  0x02
#define EDID_ESTABLISHED_TIMING_800x600_60  0x01
#define EDID_ESTABLISHED_TIMING_800x600_72  0x80
#define EDID_ESTABLISHED_TIMING_800x600_75  0x40
#define EDID_ESTABLISHED_TIMING_1024x768_ANY 0x1E
#define EDID_ESTABLISHED_TIMING_1024x768_87  0x10
#define EDID_ESTABLISHED_TIMING_1024x768_60  0x08
#define EDID_ESTABLISHED_TIMING_1024x768_70  0x04
#define EDID_ESTABLISHED_TIMING_1024x768_75  0x02
#define EDID_ESTABLISHED_TIMING_1280x1024_75  0x01

// Standard timings start at bytes index 0x26. Each standard timing is 2 bytes.
// There are 8 standard timings entries.
#define EDID_V1_STANDARD_TIMINGS_START_INDEX 0x26

// Detailed timing section start at index 0x36. Each entry is 18 bytes.
// There are 4 detailed timing entries.
#define EDID_V1_DETAILED_TIMINGS_START_INDEX 0x36
#define EDID_V1_DETAILED_TIMINGS_SIZE 18


// EDID Version 2 header defines.
#define EDID_V2_SIZE 256
#define EDID_V2_VERSION_NUMBER_INDEX  0x0
#define EDID_V2_DISPLAY_TECHNOLOGY_INDEX 0x4f
#define DISPLAY_TECHNOLOGY_TYPE_MASK  0xf0
#define EDID_V2_GTF_SUPPORT_INDEX     0x7d
#define EDID_V2_TIMING_MAP1_INDEX     0x7e  // map of timing info
#define EDID_V2_TIMING_MAP2_INDEX     0x7f
#define EDID_V2_TIMING_SECTION_START_INDEX 0x80  // offset 80: start of timing info
#define EDID_V2_LUM_TABLE_EXIST		  0x20	// map bit 5
#define TRIPLE_LUM_ENTRIES			  0x80	// offset 80 bit 7: lum table white (x1) or RGB (x3)
#define NUM_LUM_ENTRIES				  0x1f	// size of lum table (x1 or x3)
#define	EDID_V2_FREQ_RANGES			  0x1c
#define EDID_V2_FREQ_RANGE_SHIFT	  0x02
#define EDID_V2_DETAILED_RANGE_LIMITS 0x03
#define EDID_V2_TIMING_CODES		  0xf8
#define EDID_V2_TIMING_CODES_SHIFT	  0x03
#define EDID_V2_DETAILED_TIMINGS	  0x07
#endif // _EDID_H_

