//
// FILE:       library\hardware\video\specific\saa7113r.cpp
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		01.12.1999
//
// PURPOSE: 	9-bit video input processor SAA 7113 register definitions
//
// HISTORY:
//

#ifndef SAA7113R_H
#define SAA7113R_H

#define SAA7113_INIT_START_AT				0x02
#define SAA7113_IDX_MODE_0						0
#define SAA7113_IDX_MODE_1						1
#define SAA7113_IDX_MODE_2						2
#define SAA7113_IDX_MODE_3						3
#define SAA7113_IDX_GUDL_0						4
#define SAA7113_IDX_GUDL_1						5
#define SAA7113_IDX_FUSE_0						6
#define SAA7113_IDX_FUSE_1						7
#define SAA7113_HOR_SYNC_START			0x06
#define SAA7113_IDX_HSB_0						0
#define SAA7113_IDX_HSB_1						1
#define SAA7113_IDX_HSB_2						2
#define SAA7113_IDX_HSB_3						3
#define SAA7113_IDX_HSB_4						4
#define SAA7113_IDX_HSB_5						5
#define SAA7113_IDX_HSB_6						6
#define SAA7113_IDX_HSB_7						7
#define SAA7113_HOR_SYNC_STOP				0x07
#define SAA7113_SYNC_CONTROL				0x08
#define SAA7113_IDX_VNOI_0						0
#define SAA7113_IDX_VNOI_1						1
#define SAA7113_IDX_HPLL						2
#define SAA7113_IDX_HTC_0						3
#define SAA7113_IDX_HTC_1						4
#define SAA7113_IDX_FOET						5
#define SAA7113_IDX_FSEL						6
#define SAA7113_IDX_AUFD						7
#define SAA7113_LUMINANCE_CONTROL		0x09
#define SAA7113_BRIGHTNESS					0x0A
#define SAA7113_CONTRAST					0x0B
#define SAA7113_SATURATION					0x0C
#define SAA7113_HUE							0x0D
#define SAA7113_CHROMA_CONTROL			0x0E
#define SAA7113_CHROMA_GAIN_CONTROL		0x0F
#define SAA7113_FORMAT_DELAY_CONTROL	0x10
#define SAA7113_IDX_YDEL_0						0
#define SAA7113_IDX_YDEL_1						1
#define SAA7113_IDX_YDEL_2						2
#define SAA7113_IDX_VRLN						3
#define SAA7113_IDX_HDEL_0						4
#define SAA7113_IDX_HDEL_1						5
#define SAA7113_IDX_OFTS_0						6
#define SAA7113_IDX_OFTS_1						7
#define SAA7113_OUT_CONTROL_1				0x11
#define SAA7113_IDX_COLO						0
#define SAA7113_IDX_VIPB						1
#define SAA7113_IDX_OERT						2
#define SAA7113_IDX_OEYC						3
#define SAA7113_IDX_HLSEL						4
#define SAA7113_IDX_GPSW0						5
#define SAA7113_IDX_CM_99						6
#define SAA7113_IDX_GPSW_1						7
#define SAA7113_OUT_CONTROL_2				0x12
#define SAA7113_OUT_CONTROL_3				0x13
#define SAA7113_STATUS_VIDEO_DECODER	0x1F
#define SAA7113_IDX_RDCAP_CODE				0
#define SAA7113_IDX_COPRO_SLTCA				1
#define SAA7113_IDX_WIPA						2
#define SAA7113_IDX_GLIMB						3
#define SAA7113_IDX_GLIMT						4
#define SAA7113_IDX_FIDT						5
#define SAA7113_IDX_HLVLN_HLCK				6
#define SAA7113_IDX_INTL						7

#endif // SAA7113R_H
