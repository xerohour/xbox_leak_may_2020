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
//  $Workfile: qevent.h $
//
//  Copyright (c) 1999  Quadrant International, Inc.
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Tproj/Apps/xbDVD/INC/qevent.h $
// $Author: Thermanson $
// $Modtime: 9/12/00 8:29a $
// $Date: 1/22/01 3:56p $
// $Archive: /Tproj/Apps/xbDVD/INC/qevent.h $
// $Revision: 1 $
//
////////////////////////////////////$NoKeywords: $////////////////////////////


#ifndef QEvent_h
#define QEvent_h

#include "evcode.h"

// UOP definitions
#define MKFLAG(x)					(1UL << x)

#define UOP_TIME_PLAY_SEARCH			MKFLAG(0)
#define UOP_PTT_PLAY_SEARCH			MKFLAG(1)
#define UOP_TITLE_PLAY				MKFLAG(2)
#define UOP_STOP					MKFLAG(3)
#define UOP_GO_UP					MKFLAG(4)
#define UOP_TIME_PTT_SEARCH			MKFLAG(5)
#define UOP_PREV_TOP_PG_SEARCH		MKFLAG(6)
#define UOP_NEXT_PG_SEARCH			MKFLAG(7)
#define UOP_FORWARD_SCAN				MKFLAG(8)
#define UOP_BACKWARD_SCAN			MKFLAG(9)
#define UOP_MENU_CALL_TITLE			MKFLAG(10)
#define UOP_MENU_CALL_ROOT			MKFLAG(11)
#define UOP_MENU_CALL_SUB_PICTURE		MKFLAG(12)
#define UOP_MENU_CALL_AUDIO			MKFLAG(13)
#define UOP_MENU_CALL_ANGLE			MKFLAG(14)
#define UOP_MENU_CALL_PTT			MKFLAG(15)
#define UOP_RESUME					MKFLAG(16)
#define UOP_BUTTON					MKFLAG(17)
#define UOP_STILL_OFF				MKFLAG(18)
#define UOP_PAUSE_ON				MKFLAG(19)
#define UOP_AUDIO_STREAM_CHANGE		MKFLAG(20)
#define UOP_SUB_PICTURE_STREAM_CHANGE	MKFLAG(21)
#define UOP_ANGLE_CHANGE				MKFLAG(22)
#define UOP_KARAOKE_MODE_CHANGE		MKFLAG(23)
#define UOP_VIDEO_MODE_CHANGE			MKFLAG(24)

//
// Event Codes
//

// EC_QI_AUTOSCAN_PERCENTAGE
//
// data is the percentage complete of the automatic channel scan.
//
// Periodically updates the application as to the percentage complete of the
// automatic channel scan.
#define EC_QI_AUTOSCAN_PERCENTAGE		(EC_USER|1)

// EC_QI_AUTOSCAN_DONE
//
// no data
//
// Indicates the automatic channel scan is complete.
#define EC_QI_AUTOSCAN_DONE			(EC_USER|2)

// EC_QI_AUTOSCAN_ABORTED
//
// no data
//
// Indicates that the automatic channel scan has been aborted.  If requested,
// this indicates the request has successfully completed.
#define EC_QI_AUTOSCAN_ABORTED		(EC_USER|3)



#endif
