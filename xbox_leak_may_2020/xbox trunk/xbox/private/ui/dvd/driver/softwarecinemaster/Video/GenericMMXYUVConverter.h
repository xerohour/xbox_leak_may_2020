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

#ifndef GENERICMMXYUVCONVERTER_H
#define GENERICMMXYUVCONVERTER_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"

enum VideoDownscaleFilterLevel
	{
	VSQL_BINARY,
	VSQL_DROP,
	VSQL_LINEAR,
	VSQL_FOUR_TAP,
	VSQL_EIGHT_TAP
	};

#define DIFEF_ENABLE_TOP_DEINTERLACE		1
#define DIFEF_ENABLE_TOP_COPY					2
#define DIFEF_ENABLE_BOTTOM_DEINTERLACE	4
#define DIFEF_ENABLE_BOTTOM_COPY				8

#define DIFEF_ENABLE_BOTH_DEINTERLACE		( DIFEF_ENABLE_TOP_DEINTERLACE | DIFEF_ENABLE_BOTTOM_DEINTERLACE )
#define DIFEF_ENABLE_BOTH_COPY				( DIFEF_ENABLE_TOP_COPY	| DIFEF_ENABLE_BOTTOM_COPY )
#define DIFEF_ENABLE_BOTH_ALL					( DIFEF_ENABLE_BOTH_DEINTERLACE | DIFEF_ENABLE_BOTH_COPY )

void InitFrameStoreScalerTables(void);

struct YUVVideoConverterParameter
	{
	int width, height, mwidth;

	int ystart, ystop, ycount, yfraction;
	int xscale, xfraction, xoffset;

	BYTE * ppy, * py, * ppuv, * puv, * pytb, * puvtb;
	int bpr;

	BYTE * pm, * ppm, * pmt, * pmb;
	int mbpr;

	int yosdstart, yosdstop, yosdoffset;
	int xosdstart, xosdstop;
	BYTE * po;
	int obpr;
	DDWORD * dd;
	DWORD * osdNonZeroMap;
	int lboffset;

	int stripeMask;
	bool topField;
	DWORD	deinterlaceFlags; // DIFEF_...
	};

class GenericMMXYUVVideoConverter
	{
	protected:
		PadOnQWord	pad0;
		BYTE			yuvcache[2880];
	public:
		typedef void (GenericMMXYUVVideoConverter::*ConvertStripeFunction)(const YUVVideoConverterParameter & p);

		// [bits][osd][x][y]
		ConvertStripeFunction	convertStripes[2][2][5][5];

		GenericMMXYUVVideoConverter(void);

		//////
		//
		// horizontal binary ratio scaler, vertical line drop
		//
		virtual void ConvertStripeToYUV_P7DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P7DS(const YUVVideoConverterParameter & p) {}



		virtual void ConvertStripeToYUV_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8DSY4(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8DSY4(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8F2Y2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8F2Y4(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8F2Y4(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8F4(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8F4(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8F4Y2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8F4Y2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV_P8F4Y4(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVThroughOSD_P8F4Y4(const YUVVideoConverterParameter & p) {}


		//////
		//
		// horizontal linear interpolation, vertical line drop and pan scan stretching
		//
		virtual void ConvertStripeToYUVPS_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSAI_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSAIThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSDI2_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSDI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSA2_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSA2ThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSSI2_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVPSSI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUVLB_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBThroughOSD_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBAI_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBAIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBDI2_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBDI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBA2_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBA2ThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBSI2_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLBSI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLB32PDFieldField_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLB32PDFieldFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLB32PDFrameField_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVLB32PDFrameFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUVDI_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDIThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDI_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDIThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDI_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDI_P8F2Y2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDIThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p) {}

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUVAI_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVAIThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVAI_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVAIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVAI_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVAIThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVAI_P8F2Y2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVAIThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p) {}

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUV32PDFieldField_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV32PDFieldFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV32PDFrameField_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV32PDFrameFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUV32PDFieldField_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV32PDFieldFieldThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV32PDFrameField_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUV32PDFrameFieldThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUVDI2_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDI2_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVDI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}


		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUVSI2_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVSI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVSI2_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVSI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		virtual void ConvertStripeToYUVA2_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVA2ThroughOSD_P8DS(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVA2_P8F2(const YUVVideoConverterParameter & p) {}
		virtual void ConvertStripeToYUVA2ThroughOSD_P8F2(const YUVVideoConverterParameter & p) {}



	};




#endif
