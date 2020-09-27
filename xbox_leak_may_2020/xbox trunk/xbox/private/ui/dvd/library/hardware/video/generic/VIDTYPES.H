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


// FILE:      library\hardware\video\generic\vidtypes.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.04.95
//
// PURPOSE: Video related types.
//
// HISTORY:

#ifndef VIDEOTYPES_H
#define VIDEOTYPES_H

enum VideoStandard
	{
	VSTD_NTSC	 = 0,
	VSTD_PAL		 = 1,
	VSTD_SECAM	 = 2,
	VSTD_HDTV	 = 3,
	VSTD_UNKNOWN = 4,
	VIDEOSTANDARD_MIN = VSTD_NTSC,
	VIDEOSTANDARD_MAX = VSTD_UNKNOWN
	};


enum MovingImageStandard
	{
	MIS_576_25		= 0,
	MIS_480_29_97	= 1
	};

enum PALVideoSubStandard
	{
	PALVIDEOSUBSTANDARD_MIN	= 0,
		PALVSSTD_DEFAULT		= 0,
		PALVSSTD_BDGHI			= 1,
		PALVSSTD_N				= 2,
		PALVSSTD_NC				= 3,
		PALVSSTD_INVALID		= 4,
	PALVIDEOSUBSTANDARD_MAX	= PALVSSTD_NC
	};

enum NTSCVideoSubStandard
	{
	NTSCVIDEOSUBSTANDARD_MIN	= 0,
		NTSCVSSTD_DEFAULT			= 0,
		NTSCVSSTD_NORMAL			= 1,
		NTSCVSSTD_PALM				= 2,
		NTSCVSSTD_443				= 3,
		NTSCVSSTD_JAPAN			= 4,
		NTSCVSSTD_PAL60			= 5,
		NTSCVSSTD_INVALID			= 6,
	NTSCVIDEOSUBSTANDARD_MAX	= NTSCVSSTD_PAL60
	};

enum VideoSource
	{
	VIDEOSOURCE_MIN			= 0,
		VSRC_COMPOSITE			= 0,
		VSRC_SVIDEO				= 1,
		VSRC_SERIAL_DIGITAL	= 2,
		VSRC_RGB					= 3,
	VIDEOSOURCE_MAX	= VSRC_RGB
	};

enum VideoPresentationMode
	{
	VPRM_4_BY_3_NORMAL,
	VPRM_4_BY_3_LETTERBOXED,
	VPRM_16_BY_9_ANAMORPHIC,
	VPRM_RESERVED
	};

enum VideoFormat
	{
	VIDEOFORMAT_MIN = 0,
		VFMT_YUV_422 = 0,
		VFMT_YUV_411 = 1,
		VFMT_YUV_420 = 2,
		VFMT_YUV_444 = 3,
		VFMT_RGB_444 = 4,
	VIDEOFORMAT_MAX = VFMT_RGB_444
	};

enum VideoBusFormat
	{
	VIDEOBUSFORMAT_MIN = 0,
		FBFM_YC16 = 0,
		FBFM_YC8  = 1,
	VIDEOBUSFORMAT_MAX = FBFM_YC8
	};

enum VideoLineMode
	{
	VIDEOLINEMODE_MIN		= 0,
		VLM_PROGRESSIVE	= 0,
		VLM_INTERLACED		= 1,
	VIDEOLINEMODE_MAX		= VLM_INTERLACED
	};

enum VideoMode
	{
	VIDEOMODE_MIN = 0,
		VMOD_RESET    = 0,
		VMOD_PATTERN  = 1,
		VMOD_CAPTURE  = 2,
		VMOD_PLAYBACK = 3,
		VMOD_PATTERNSYNC = 4,
	VIDEOMODE_MAX = VMOD_PATTERNSYNC
//	ModeForceReprogramming   // this one internal use only
	};

enum VideoSampleMode
	{
	VIDEOSAMPLEMODE_MIN = 0,
		VSAMOD_CCIR	= 0,
		VSAMOD_SQP	= 1,
	VIDEOSAMPLEMODE_MAX = VSAMOD_SQP
	};

enum VideoField
	{
	VIDEOFIELD_MIN = 0,
		VFLD_SINGLE = 0,
		VFLD_ODD    = 1,
		VFLD_EVEN   = 2,
	VIDEOFIELD_MAX = VFLD_EVEN
	};

enum VideoMuxSource
	{
	VIDEOMUXSRC_MIN = 0,
		VIDEOMUXSRC_INTERNAL = 0,
		VIDEOMUXSRC_NONE = 1,
		VIDEOMUXSRC_EXTERNAL = 2,
	VIDEOMUXSRC_MAX = VIDEOMUXSRC_EXTERNAL
	};

enum VideoPixClockMode
	{
	VIDEOPIXCLK_MIN = 0,
		VIDEOPIXCLK_INTERNAL = 0,
		VIDEOPIXCLK_EXTERNAL = 1,
	VIDEOPIXCLK_MAX = VIDEOPIXCLK_EXTERNAL
	};

enum VideoCopyMode
	{
	VCPMD_DEFAULT,	// depends on implementation (e.g. "do not indicate on video output"). Use for
						// content that does not contain copyright protection information.

	// Use the following three when the content contains copyright protection information
	VCPMD_COPYING_PERMITTED,		// indicate that copying is always allowed
	VCPMD_ONE_COPY_PERMITTED,		// one copy allowed
	VCPMD_NO_COPYING_PERMITTED		// no copy allowed
	};


// The values of the following two enums are hardware-independent.

enum GrabFormat
	{
	GRABFORMAT_MIN = 0,
		VGRB_MJPEG    = 0,
		VGRB_RGB_888x = 1,
		VGRB_RGB_888  = 2,
		VGRB_RGB_565  = 3,
		VGRB_RGB_555  = 4,
		VGRB_YUV_422  = 5,
	GRABFORMAT_MAX = VGRB_YUV_422
	};

enum PIPFormat
	{
	PIPFORMAT_MIN = 0,
		PFMT_RGB_888x	= 0,
		PFMT_RGB_888	= 1,
		PFMT_RGB_565	= 2,
		PFMT_RGB_555	= 3,
		PFMT_YUV_422	= 4,
		PFMT_UVY_422	= 5,
		PFMT_PALETTE_8	= 6,
		PFMT_OTHER		= 7,
	PIPFORMAT_MAX = PFMT_OTHER
	};


struct HardVideoParams
	{
	int totalWidth,
	    totalHeight,
	    activeWidth,
	    activeHeight;   // activeHeight must be even (interlace)
	BIT hsPol,          // can be 0 or 1
	    vsPol;          // can be 0 or 1
	int hStart,
	    hEnd,           // hEnd = hStart + activeWidth - 1
	    vStart,
	    vEnd;           // vEnd = vStart + activeHeight/2 - 1
	BIT oddFirst;
	};


// These are defines for TV standard (CCIR) dependent sizes.

const HardVideoParams NTSCParams =
	{
	858, 525,
	720, 480,
	0,   0,
	123, 123+720-1,
	20,  20+480-1,
	TRUE
	};

const HardVideoParams PALParams =
	{
	864, 625,
	720, 576,
	0,   0,
	133, 133+720-1,
	46,  46+576-1,
	FALSE
	};


// These are defines for TV standard (square pixel) dependent sizes.

const HardVideoParams NTSCSquareParams =
	{
	780, 525,
	640, 480,
	1,   1,
	75,  75+640-1,
	20,  20+480-1,
	TRUE
	};

const HardVideoParams PALSquareParams =
	{
	944, 625,
	768, 576,
	1,   1,
	83,  83+768-1,
	34,  34+576-1,
	FALSE
	};


// Video Port specific definitions

// Possible video formats on video port (format field in VideoPortCaps struct)
#define VP_FMT_RGB_555		MKFLAG(0)
#define VP_FMT_RGB_565		MKFLAG(1)
#define VP_FMT_CCIR_422		MKFLAG(2)
#define VP_FMT_YUV_411		MKFLAG(3)
#define VP_FMT_YUV_420		MKFLAG(4)
#define VP_FMT_YONLY_400	MKFLAG(5)
#define VP_FMT_RGB_888_24	MKFLAG(6)
#define VP_FMT_RGB_888_32	MKFLAG(7)
#define VP_FMT_RGB_332		MKFLAG(8)
#define VP_FMT_ACCUPAK		MKFLAG(9)
#define VP_FMT_DYUV			MKFLAG(10)

// Protocols
#define VP_PORT_DI_8A		MKFLAG(0)
#define VP_PORT_DIV_8A		MKFLAG(1)
#define VP_PORT_DE_8A		MKFLAG(2)
#define VP_PORT_SI_8A		MKFLAG(3)
#define VP_PORT_SIV_8A		MKFLAG(4)
#define VP_PORT_SE_8A		MKFLAG(5)
#define VP_PORT_16A			MKFLAG(6)
#define VP_PORT_V_16A		MKFLAG(7)
#define VP_PORT_FIREWIRE	MKFLAG(8)
// Use this when creating a dummy Kernel Video Port PIP:
#define VP_PORT_DUMMY		MKFLAG(9)

// generate from the ones above!
//#define VP_PROT_BROOKTREE
//#define VP_PROT_PHILIPS

// Special connection features
#define VP_FEAT_INVERTPOLARITY	MKFLAG(0)
#define VP_FEAT_INTERLACED			MKFLAG(1)
#define VP_FEAT_TELETEXT			MKFLAG(2)
#define VP_FEAT_CLOSEDCAPTION		MKFLAG(3)
#define VP_FEAT_E_HREFH_VREFH		MKFLAG(4)
#define VP_FEAT_E_HREFH_VREFL		MKFLAG(5)
#define VP_FEAT_E_HREFL_VREFH		MKFLAG(6)
#define VP_FEAT_E_HREFL_VREFL		MKFLAG(7)
#define VP_FEAT_COLORCONTROL		MKFLAG(8)
#define VP_FEAT_BOB					MKFLAG(9)
#define VP_FEAT_WEAVE				MKFLAG(10)
// Use this when creating a dummy Kernel Video Port PIP:
#define VP_FEAT_DUMMY					MKFLAG(11)

struct VideoPortCaps
	{
	DWORD	videoFormat;		// Video format on port connection

	DWORD	protocol;			// Communication protocol(s)
	DWORD	connFeatures;		// Special features for the connection

	DWORD	fieldWidth;			// Maximum total transfer width of one field
	DWORD	vbiWidth;			// Maximum transfer width for VBI information
	DWORD	fieldHeight;		// Maximum total transfer height of one field

	DWORD	usPerField;			// Shortest possible time between two VREFs in microseconds
	DWORD	pixelsPerSecond;	// Number of pixels per second

	DWORD	displayOnly;		// The VGA can only display, but not capture.

	DWORD	clrControls;		// Will contain possible color controls
	};


#endif
