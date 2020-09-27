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
// PURPOSE:   Class for windows as ports
//
// HISTORY:
//
//

#include "winports.h"
#include "library/common/prelude.h"
#include "library/common/vddebug.h"
#include "library/lowlevel/timer.h"

////////////////////////////////////////////////////////////////////
//
//  ST20LITE WinPort Classes
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Windows WinPort Classes
//
////////////////////////////////////////////////////////////////////



//
// Class name
//

static TCHAR PWClassName[] = __TEXT("VDWinPort");

//
// Static class variables, represent the window class
//

////////////////////////////////////////////////////////////////////
//
//  WorkerThreadWinPort Class
//
////////////////////////////////////////////////////////////////////


class EventPortMessage
	{
	public:
		EventPortMessage	*	succ;
		EventPort			*	port;
		WPARAM					wParam;
		LPARAM					lParam;
		DWORD						time;
		HANDLE					event;

		EventPortMessage(EventPort * port, WPARAM wParam, LPARAM lParam, DWORD time, HANDLE event)
			{
			this->port = port;
			this->wParam = wParam;
			this->lParam = lParam;
			this->time = time;
			this->event = event;
			this->succ = NULL;
			}
	};

WinPortServer::WinPortServer(void)
	{
	messages = NULL;
	timeMessages = NULL;
	event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	terminate = false;
	}

WinPortServer::~WinPortServer(void)
	{
	::CloseHandle(event);
	}

void WinPortServer::EnterQueueHead(EventPortMessage * & queue, EventPortMessage * message)
	{
	if (queue)
		{
		message->succ = queue->succ;
		queue->succ = message;
		queue = message;
		}
	else
		{
		message->succ = message;
		queue = message;
		}
	}

EventPortMessage * WinPortServer::GetQueuePort(EventPortMessage * & queue, EventPort * port)
	{
	EventPortMessage * msg, * rmsg;

	if (queue)
		{
		msg = queue;
		while (msg->succ != queue && msg->succ->port != port)
			{
			msg = msg->succ;
			}
		if (msg->succ->port == port)
			{
			rmsg = msg->succ;
			if (msg->succ == queue)
				{
				if (queue == queue->succ)
					queue = NULL;
				else
					{
					queue = msg;
					msg->succ = msg->succ->succ;
					}
				}
			else
				msg->succ = msg->succ->succ;

			return rmsg;
			}
		else
			return NULL;
		}
	else
		return NULL;
	}

void WinPortServer::EnterQueueTime(EventPortMessage * & queue, EventPortMessage * message)
	{
	EventPortMessage * mq;

	if (queue)
		{
		mq = queue;
		while (mq->succ != queue && mq->succ->time < message->time)
			mq = mq->succ;

		if (mq->succ->time < message->time)
			{
			message->succ = mq->succ;
			mq->succ = message;
			}
		else
			{
			message->succ = queue->succ;
			queue->succ = message;
			queue = queue->succ;
			}
		}
	else
		{
		message->succ = message;
		queue = message;
		}
	}

EventPortMessage * WinPortServer::PeekQueueHead(EventPortMessage * queue)
	{
	if (queue)
		{
		return queue->succ;
		}
	else
		return NULL;
	}

EventPortMessage * WinPortServer::GetQueueHead(EventPortMessage * & queue)
	{
	EventPortMessage * message;

	if (queue)
		{
		message = queue->succ;

		if (queue->succ != queue)
			{
			queue->succ = queue->succ->succ;
			}
		else
			{
			queue = NULL;
			}

		return message;
		}
	else
		return NULL;
	}

BOOL WinPortServer::ProcessMessages(void)
	{
	HANDLE	events[1];
	DWORD		result;
	DWORD		time;
	EventPortMessage *	message;

	events[0] = event;
	while (!terminate)
		{
		lock.Enter();
		time = Timer.GetMilliTicks();
		message = PeekQueueHead(timeMessages);
		while (message && time >= message->time)
			{
			GetQueueHead(timeMessages);
			lock.Leave();
			message->port->Message(message->wParam, message->lParam);
			delete message;
			lock.Enter();
			time = Timer.GetMilliTicks();
			message = PeekQueueHead(timeMessages);
			}

		if (message)
			time = message->time - time;
		else
			time = INFINITE;

		message = PeekQueueHead(messages);
		while (message)
			{
			GetQueueHead(messages);
			lock.Leave();
			message->port->Message(message->wParam, message->lParam);
			if (message->event) ::SetEvent(message->event);
			delete message;
			lock.Enter();
			message = PeekQueueHead(messages);
			}
		lock.Leave();
		result = WaitForMultipleObjects(1, events, FALSE, time);
		}

	return TRUE;
	}

BOOL WinPortServer::SendMessage(EventPort * port, WPARAM wParam, LPARAM lParam, HANDLE event)
	{
	lock.Enter();
	EnterQueueHead(messages, new EventPortMessage(port, wParam, lParam, 0, event));
	::SetEvent(this->event);
	lock.Leave();

	return TRUE;
	}

void WinPortServer::ScheduleTimer(EventPort * port, WPARAM wParam, DWORD delay)
	{
	lock.Enter();
	CancelTimer(port);
	EnterQueueTime(timeMessages, new EventPortMessage(port, wParam, 0, Timer.GetMilliTicks() + delay, NULL));
	::SetEvent(this->event);
	lock.Leave();
	}

void WinPortServer::CancelTimer(EventPort * port)
	{
	EventPortMessage * message;

	lock.Enter();
	while (message = GetQueuePort(timeMessages, port))
		delete message;
	lock.Leave();
	}


void WinPortServer::FlushMessages(EventPort * port)
	{
	EventPortMessage * message;

	lock.Enter();
	while (message = GetQueuePort(timeMessages, port))
		delete message;
	while (message = GetQueuePort(messages, port))
		delete message;
	lock.Leave();
	}

void EventPort::InternalMessage(WPARAM wParam, LPARAM lParam)
	{
	Message(wParam, lParam);
	}

EventPort::EventPort(WinPortServer * server)
	{
	this->server = server;
	event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}

EventPort::~EventPort(void)
	{
	FlushMessages();
	CloseHandle(event);
	}

BOOL EventPort::SendMessage(WPARAM wParam, LPARAM lParam)
	{
	return server->SendMessage(this, wParam, lParam);
	}

BOOL EventPort::SendMessageIfQueueEmpty (WPARAM wParam, LPARAM lParam)
	{
	return server->SendMessage(this, wParam, lParam);
	}

void EventPort::DoMessage(WPARAM wParam, LPARAM lParam)
	{
	lock.Enter();
	if (server->SendMessage(this, wParam, lParam, event))
		::WaitForSingleObject(event, INFINITE);
	lock.Leave();
	}

void EventPort::ScheduleTimer(WPARAM wParam, DWORD delay)
	{
	server->ScheduleTimer(this, wParam, delay);
	}

void EventPort::CancelTimer(void)
	{
	server->CancelTimer(this);
	}

void EventPort::FlushMessages(void)
	{
	server->FlushMessages(this);
	}

void EventPort::ClosePort(void)
	{
	FlushMessages();
	}

//////////////////////////////////////////////////////////////

class WorkerThreadEventPort : public EventPort
	{
	protected:
		EventPortWorkerThread	*	thread;
	public:
		WorkerThreadEventPort(EventPortWorkerThread * thread)
			: EventPort(thread)
			{
			this->thread = thread;
			}

		void Message(WPARAM wParam, LPARAM lParam)
			{
			thread->WorkMessage(wParam, lParam);
			}
	};

static DWORD WINAPI EventWorkerThreadEntry(void * param)
	{
	((EventPortWorkerThread *)param)->ThreadEntry();

	return 0;
	}


void EventPortWorkerThread::ThreadEntry(void)
	{
	ProcessMessages();
	}

EventPortWorkerThread::EventPortWorkerThread(WinPortServer * server, int priority)
	{
	DWORD id;

	port = new WorkerThreadEventPort(this);

	workerThread = CreateThread(NULL, 100000, EventWorkerThreadEntry, this, 0, &id);
	SetThreadPriority(workerThread, priority);
	}

EventPortWorkerThread::~EventPortWorkerThread(void)
	{
	TerminateWorker();
	}

void EventPortWorkerThread::TerminateWorker(void)
	{
	if (port)
		{
		terminate = true;
		::SetEvent(event);
		WaitForSingleObject(workerThread, INFINITE);
		CloseHandle(workerThread);
		workerThread = NULL;
		delete port;
		port = NULL;
		}
	}


