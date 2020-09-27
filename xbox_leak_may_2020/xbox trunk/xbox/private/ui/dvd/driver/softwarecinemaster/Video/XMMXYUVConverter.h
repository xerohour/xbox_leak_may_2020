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


#ifndef XMMXYUVCONVERTER_H
#define XMMXYUVCONVERTER_H


#include "GenericMMXYUVConverter.h"

	class XMMXYUVVideoConverter : public GenericMMXYUVVideoConverter
	{
	public:
		XMMXYUVVideoConverter(void) : GenericMMXYUVVideoConverter() {}

		//////
		//
		// horizontal binary ratio scaler, vertical line drop
		//
		void ConvertStripeToYUV_P7DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P7DS(const YUVVideoConverterParameter & p);


		void ConvertStripeToYUV_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV_P8DSY4(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8DSY4(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV_P8F2Y2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV_P8F2Y4(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8F2Y4(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV_P8F4(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8F4(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV_P8F4Y2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8F4Y2(const YUVVideoConverterParameter & p);

		void ConvertStripeToYUV_P8F4Y4(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVThroughOSD_P8F4Y4(const YUVVideoConverterParameter & p);

		//////
		//
		// horizontal linear interpolation, vertical line drop and pan scan stretching
		//
		void ConvertStripeToYUVPS_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSAI_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSAIThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		// new->	  Jan. 25.2000
		void ConvertStripeToYUVPSDI2_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSDI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSA2_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSA2ThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSSI2_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVPSSI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		// <-new

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUVLB_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBThroughOSD_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBAI_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBAIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p);
		// new->	  Jan. 25.2000
		void ConvertStripeToYUVLBDI2_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBDI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBA2_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBA2ThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBSI2_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLBSI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLB32PDFieldField_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLB32PDFieldFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLB32PDFrameField_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVLB32PDFrameFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		// <-new

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUVDI_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDIThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDI_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDIThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDI_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDI_P8F2Y2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDIThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p);

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUVAI_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVAIThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVAI_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVAIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVAI_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVAIThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVAI_P8F2Y2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVAIThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p);

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUV32PDFieldField_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV32PDFieldFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV32PDFrameField_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV32PDFrameFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p);

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUV32PDFieldField_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV32PDFieldFieldThroughOSD_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV32PDFrameField_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUV32PDFrameFieldThroughOSD_P8F2(const YUVVideoConverterParameter & p);

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUVDI2_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDI2_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVDI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p);


		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUVSI2_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVSI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVSI2_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVSI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p);

		//////
		//
		// horizontal binary ratio scaler, vertical linear interpolation
		//
		void ConvertStripeToYUVA2_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVA2ThroughOSD_P8DS(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVA2_P8F2(const YUVVideoConverterParameter & p);
		void ConvertStripeToYUVA2ThroughOSD_P8F2(const YUVVideoConverterParameter & p);


	};


#endif
