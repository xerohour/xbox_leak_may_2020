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
//  Event Disptacher Class
//
//  - All of the real work about event handling is actually happening here
//
////////////////////////////////////////////////////////////////////

#ifndef EVENTDISPATCHER_H

#define EVENTDISPATCHER_H

#include "DVDTime.h"
#include "library/common/krnlsync.h"
#include "library/common/vddebug.h"

class EventDispatcher
	{
	protected:
		VDLocalMutex eventDispatcherLock;

		struct
			{
			DNEEventHandler		handler;
			void					*	userData;
			} eventHandler[DNE_EVENT_NUMBER];

	public:
		EventDispatcher(void);
		virtual ~EventDispatcher(void) {}

		virtual Error SetEventHandler(DWORD event, DNEEventHandler handler, void * userData)
			{
			if (event < DNE_EVENT_NUMBER)
				{
				eventDispatcherLock.Enter();
				eventHandler[event].handler = handler;
				eventHandler[event].userData = userData;
				eventDispatcherLock.Leave();
				GNRAISE_OK;
				}
			else
				GNRAISE(GNR_RANGE_VIOLATION);
			}

		BOOL EventHasHandler(DWORD event) {return eventHandler[event].handler != NULL;}

		Error SendEvent(DWORD event, DWORD info)
			{
			eventDispatcherLock.Enter();


			if (EventHasHandler(event))
				{
				eventHandler[event].handler(event, eventHandler[event].userData, info);
				}
//			else
//				DP("No handler for %ld", event);

			eventDispatcherLock.Leave();

			GNRAISE_OK;
			}
	};

#endif
