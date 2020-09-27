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

//
// FILE:      library\common\winports.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   22.03.95
//
// PURPOSE:   Class for windows as ports.
//
// HISTORY:
//

#ifndef WINPORTS_H
#define WINPORTS_H

#include "library\common\prelude.h"

////////////////////////////////////////////////////////////////////
//
//  Some defines
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Generic Message Port Class
//
////////////////////////////////////////////////////////////////////

class GenericMsgPort
	{
	public:
		// Send a message to the port and return immediately. The result is TRUE for success.
		virtual BOOL SendMessage (WPARAM wParam, LPARAM lParam) = 0;
		virtual BOOL SendMessageIfQueueEmpty (WPARAM wParam, LPARAM lParam) = 0;

		// Send a message to the port and wait until it has been processed.
		virtual void DoMessage (WPARAM wParam, LPARAM dParam) = 0;

		// Schedule a message to be posted after some delay. The call returns immediately.
		// Note that you cannot call this during interrupt.
		virtual void ScheduleTimer (WPARAM wParam, DWORD millisecondDelay) = 0;

		// Cancel any delayed message.
		virtual void CancelTimer (void) = 0;

		// Remove all messages from the queue.
		virtual void FlushMessages (void) = 0;
	};

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\common\krnlsync.h"

////////////////////////////////////////////////////////////////////
//
//  ST20 WinPort Classes
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Windows WinPorts
//
////////////////////////////////////////////////////////////////////



//
//  WinPortServer
//

class EventPortMessage;
class EventPort;

class WinPortServer
	{
	friend class EventPort;
	friend class EventPortWorkerThread;
	protected:
		EventPortMessage	*	messages;
		EventPortMessage	*	timeMessages;
		HANDLE					event;
		VDLocalMutex			lock;
		bool						terminate;

		void EnterQueueHead(EventPortMessage * & queue, EventPortMessage * message);
		void EnterQueueTime(EventPortMessage * & queue, EventPortMessage * message);
		EventPortMessage * GetQueuePort(EventPortMessage * & queue, EventPort * port);
		EventPortMessage * PeekQueueHead(EventPortMessage * queue);
		EventPortMessage * GetQueueHead(EventPortMessage * & queue);

	public:
		WinPortServer(void);
		~WinPortServer(void);

		BOOL ProcessMessages(void);

		BOOL SendMessage(EventPort * port, WPARAM wParam, LPARAM lParam, HANDLE event = NULL);
		void ScheduleTimer(EventPort * port, WPARAM wParam, DWORD delay);
		void CancelTimer(EventPort * port);

		void FlushMessages(EventPort * port);
	};




//
//  WinPortWorkerThread
//

class EventPort : public GenericMsgPort
	{
	protected:
		WinPortServer *	server;
		VDLocalMutex		lock;
		HANDLE				event;
		void InternalMessage(WPARAM wParam, LPARAM lParam);
	public:
		EventPort(WinPortServer * server);
		virtual ~EventPort(void);

		//
		// Send a message to the port.
		//
		BOOL SendMessage(WPARAM wParam, LPARAM lParam);
		BOOL SendMessageIfQueueEmpty (WPARAM wParam, LPARAM lParam);
		void DoMessage(WPARAM wParam, LPARAM dParam);

		void ScheduleTimer(WPARAM wParam, DWORD delay);
		void CancelTimer(void);

		//
		// Method, that receives incoming messages asynchronously.
		//
		virtual void Message(WPARAM wParam, LPARAM lParam) = 0;

		void FlushMessages(void);

		void ClosePort(void);

		HWND GetHWND(void) {return NULL;}
	};

class EventPortWorkerThread : protected WinPortServer
	{
	friend class WorkerThreadEventPort;
	friend DWORD WINAPI EventWorkerThreadEntry(void * param);
	protected:
		EventPort	*	port;
		HANDLE			workerThread;

		void ThreadEntry(void);
	public:
		EventPortWorkerThread(WinPortServer * server, int priority);
		virtual ~EventPortWorkerThread(void);

		BOOL SendWorkMessage(WPARAM wParam, LPARAM lParam) {return port->SendMessage(wParam, lParam);}
		void DoWorkMessage(WPARAM wParam, LPARAM lParam) {port->DoMessage(wParam, lParam);}

		virtual void WorkMessage(WPARAM wParam, LPARAM lParam) = 0;

		void FlushWorkMessages(void) {port->FlushMessages();}

		void TerminateWorker(void);

		HWND GetHWND(void) {return NULL;}

		GenericMsgPort * GetPort(void) {return port;}
	};



typedef EventPort						WinPort;
typedef EventPortWorkerThread		WinPortWorkerThread;


#endif

#endif
