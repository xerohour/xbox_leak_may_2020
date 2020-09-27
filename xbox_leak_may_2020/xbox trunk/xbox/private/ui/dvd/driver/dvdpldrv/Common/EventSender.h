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

////////////////////////////////////////////////////////////////////
//
//  Event Sender Class
//
////////////////////////////////////////////////////////////////////

#ifndef EVENTSENDER_H

#define EVENTSENDER_H

#include "DVDTime.h"
#include "EventDispatcher.h"
#include "library/common/krnlsync.h"
#include "library/common/vddebug.h"

class EventSender
	{
	protected:
		EventDispatcher* pEventDispatcher;

	public:
		EventSender(EventDispatcher* pNewEventDispatcher);
		virtual ~EventSender(void) {}

		virtual Error SetEventHandler(DWORD event, DNEEventHandler handler, void * userData)
			{
			if (pEventDispatcher)
				return pEventDispatcher->SetEventHandler(event, handler, userData);
			else
				GNRAISE(GNR_OBJECT_EMPTY);
			}

		BOOL EventHasHandler(DWORD event)
			{
			if (pEventDispatcher)
				return pEventDispatcher->EventHasHandler(event);
			else
				GNRAISE(GNR_OBJECT_EMPTY);
			}

		Error SendEvent(DWORD event, DWORD info)
			{
			if (pEventDispatcher)
				return pEventDispatcher->SendEvent(event, info);
			else
				GNRAISE(GNR_OBJECT_EMPTY);
			}

		void SetEventDispatcher(EventDispatcher* pNewEventDispatcher)
			{
			pEventDispatcher = pNewEventDispatcher;
			}

		EventDispatcher* GetEventDispatcher(){ return pEventDispatcher; }
	};

#define GNREASSERT_EVENT(cond, event, info) { Error e; if (e = (cond)) { SendEvent(event, info); GNRAISE(e); } }

#endif
