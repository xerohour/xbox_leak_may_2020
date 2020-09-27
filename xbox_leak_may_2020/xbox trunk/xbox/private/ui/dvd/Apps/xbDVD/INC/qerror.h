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

//////////////////////////////////////////////////////////////////////////////
//
//  $Workfile: qerror.h $
//
//  Copyright (c) 1999  Quadrant International, Inc.
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Tproj/Apps/xbDVD/INC/qerror.h $
// $Author: Mpeteritas $
// $Modtime: 3/21/01 6:24p $
// $Date: 3/21/01 6:24p $
// $Archive: /Tproj/Apps/xbDVD/INC/qerror.h $
// $Revision: 2 $
//
////////////////////////////////////$NoKeywords: $////////////////////////////


#ifndef QI_Errors_H
#define QI_Errors_H

#include "winerror.h"

#define FACILITY_QI		FACILITY_ITF

#define QI_ERROR    3
#define QI_WARNING  2
#define QI_INFO     1
#define QI_OK		0

#define MAKE_HR( a, b, c ) MAKE_HRESULT(a, b, c+0x0200)

// ERRORs
#define QI_E_NOTINITIALIZED			MAKE_HR( QI_ERROR, FACILITY_QI, 1 )
#define QI_E_INVALIDPROPERTY		MAKE_HR( QI_ERROR, FACILITY_QI, 2 )
#define QI_E_MEDIA_NOT_RECOGNIZED	MAKE_HR( QI_ERROR, FACILITY_QI, 3 )
#define QI_E_DSHOW_NODVD			MAKE_HR( QI_ERROR, FACILITY_QI, 4 )
#define QI_E_DSHOW_RENDERFAIL		MAKE_HR( QI_ERROR, FACILITY_QI, 5 )
#define QI_E_CREATIONFAILED			MAKE_HR( QI_ERROR, FACILITY_QI, 6 )
#define QI_E_MEMORYERROR			MAKE_HR( QI_ERROR, FACILITY_QI, 7 )
#define QI_E_INVALID_TYPE_SPECIFIED	MAKE_HR( QI_ERROR, FACILITY_QI, 8 )
#define QI_E_NO_OBJECTS_LOADED		MAKE_HR( QI_ERROR, FACILITY_QI, 9 )
#define QI_E_DRIVE_EMPTY			MAKE_HR( QI_ERROR, FACILITY_QI, 10 )
#define QI_E_RESOURCE				MAKE_HR( QI_ERROR, FACILITY_QI, 11 )
#define QI_E_LARGERBUFFERSIZE		MAKE_HR( QI_ERROR, FACILITY_QI, 12 )
#define QI_E_AUTOSCAN_ABORT			MAKE_HR( QI_ERROR, FACILITY_QI, 13 )
#define QI_E_INVALID_REGION			MAKE_HR( QI_ERROR, FACILITY_QI, 14 )
#define QI_E_INSUFFICIENT_RIGHTS	MAKE_HR( QI_ERROR, FACILITY_QI, 15 )

// INFOs
#define QI_I_NOTAPPLICABLE			MAKE_HR( QI_INFO, FACILITY_QI, 1 )
#define QI_I_CHANGEPENDING			MAKE_HR( QI_INFO, FACILITY_QI, 2 )
#define QI_I_NOTVALIDNOW			MAKE_HR( QI_INFO, FACILITY_QI, 3 )

// OKs

// WARNINGs
#define QI_W_BOOL_EXPECTED			MAKE_HR( QI_WARNING, FACILITY_QI, 1 )
#define QI_W_DATAOUTOFRANGE			MAKE_HR( QI_WARNING, FACILITY_QI, 2 )
#define QI_W_READONLY				MAKE_HR( QI_WARNING, FACILITY_QI, 3 )
#define QI_W_SETONLY				MAKE_HR( QI_WARNING, FACILITY_QI, 4 )
#define QI_W_INPROCESS				MAKE_HR( QI_WARNING, FACILITY_QI, 5 )

// Message Based Errors
enum {
	QERROR_UNKNOWN = 100,	// don't tread on DShow errors
	QERROR_VIDEOSTREAM,		// the video stream couldn't be rendered
	QERROR_AUDIOSTREAM,		// the audio stream couldn't be rendered
	QERROR_SUBPICSTREAM,	// the subpicture stream couldn't be rendered
	QERROR_VPE,				// there was a problem creating the vpe surface
	QERROR_VOL_INVALID,		// the DVD volume was deemed invalid
	QERROR_VOL_UNKNOWN,		// the DVD volume is of an unknown type
	QERROR_CAPTIONS,		// closed captions don't appear to be properly encoded
	QERROR_REGION,			// the DVD is not of a playable region code
	QERROR_COPYPROTECTION,	// the DVD's copy protection could not be authenticated
	QERROR_FORMAT,			// does not meet DVD 1.0 specs
	QERROR_MACROVISION,		// the DVD's macrovision protection could not be authenticated
	QERROR_SYS_DECODER,		// system region and decoder region don't match
	QERROR_DISC_DECODER,	// disc not authored for decoder region
	QERROR_COUNT
	};

#endif
