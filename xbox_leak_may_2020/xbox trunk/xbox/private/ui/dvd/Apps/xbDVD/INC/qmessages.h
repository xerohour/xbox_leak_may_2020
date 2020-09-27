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
//  $Workfile: qmessages.h $
//
//  Copyright (c) 1999  Quadrant International, Inc.
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Tproj/Apps/xbDVD/INC/qmessages.h $
// $Author: Thermanson $
// $Modtime: 9/12/00 8:29a $
// $Date: 1/22/01 3:56p $
// $Archive: /Tproj/Apps/xbDVD/INC/qmessages.h $
// $Revision: 1 $
//
////////////////////////////////////$NoKeywords: $////////////////////////////


#ifndef QIMessages_h
#define QIMessages_h

#include "winuser.h"

#define BASE	WM_APP + 100

// WM_QI_DSHOW		is sent to a window by DirectShow's IMediaEventEx interface.
//				The DirectShow IMediaEventEx::GetEvent method should then be
//				called to retrieve the event that took place and respond
//				to it.
//
// wParam			not used
//
// lParam			pointer to the IMediaEventEx interface
//
#define WM_QI_DSHOW						BASE + 5

// QMediaComponentEvent and QMediaComponentEvent2
//				sent to a window to indicate some sort of event took place,
//                  perhaps without user interaction.  The "2" is sent after the
//				regular to indicate	the same event with more data.
//
// wParam			QI event code or DShow event code (EC_*)
//
// lParam			data for the event, if any
//
#define QMediaComponentEvent	"QMediaComponentEvent"
#define QMediaComponentEvent2	"QMediaComponentEvent2"



#endif
