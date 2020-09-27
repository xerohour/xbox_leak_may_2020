////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//
// FILE:      library\common\lockedQueue.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 2000 Viona Development.  All Rights Reserved.
// CREATED:
//
// PURPOSE:   template for single locked queue that can be accessed from multiple threads
//				  uses a windows critical section
//
// HISTORY:
//
//


#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <windows.h>

template < class T>
class LockedQueue
	{
	protected:
		CRITICAL_SECTION	lock;
		HANDLE				henter, hremove;
		int					in, out, num, max;
		bool					terminate;
		T					*	queue;

	public:
		// constructor - create locked queue with a maximum of max entries
		LockedQueue(int max)
			{
			::InitializeCriticalSection(&lock);
			this->max = max;
			in = out = num = 0;
			queue = new T[max];
			henter = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			hremove = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			this->terminate = false;
			}

		// desctructor
		// deletes a locked queue. The entries in the queue are NOT deleted
		~LockedQueue(void)
			{
			delete[] queue;
			::CloseHandle(henter);
			::CloseHandle(hremove);
			::DeleteCriticalSection(&lock);
			}


		// Add new element to locked queue.
		bool Enqueue(T t, bool block = true)
			{
			bool success = false;

			::EnterCriticalSection(&lock);
			while (block && !terminate && num == max)
				{
				::LeaveCriticalSection(&lock);
				::WaitForSingleObject(henter, INFINITE);
				::EnterCriticalSection(&lock);
				}

			if (!terminate && num < max)
				{
				queue[in] = t;
				in++;
				num++;
				if (in == max) in = 0;
				::SetEvent(hremove);
				success = true;
				}

			::LeaveCriticalSection(&lock);

			return success;
			}

		// remove element from locked queue. Do not use during flush. Use DequeueFlush instead.
		bool Dequeue(T & t, bool block = true)
			{
			bool success = false;

			::EnterCriticalSection(&lock);
			while (block && !terminate && num == 0)
				{
				::LeaveCriticalSection(&lock);
				::WaitForSingleObject(hremove, INFINITE);
				::EnterCriticalSection(&lock);
				}

			if (!terminate && num != 0)
				{
				t = queue[out];
				out++;
				num--;
				if (out == max) out = 0;
				::SetEvent(henter);
				success = true;
				}

			::LeaveCriticalSection(&lock);

			return success;
			}


		// signal begin flush operation. After BeginFlush(), remove all elements with DequeueFlush.
		// at the end, call EndFlush();
		// during Flush, no other operations than DequeueFlush() can be performed, all other would block.
		// begin flush holds critical section till next EndFlush().
		bool BeginFlush(void)
			{
			::EnterCriticalSection(&lock);

			return true;
			}

		// signal end flush. Critical section hold since BeginFlush() will be released.
		bool EndFlush(void)
			{
			::SetEvent(henter);
			::LeaveCriticalSection(&lock);

			return true;
			}

		// remove element form queue, without critical section. Only call during flush.
		bool DequeueFlush(T & t)
			{
			if (num != 0)
				{
				t = queue[out];
				out++;
				num--;
				if (out == max) out = 0;

				return true;
				}
			else
				return false;
			}

		// terminate queue operations. All blocked operations will be unblocked when calling
		// Terminate()
		bool Terminate(void)
			{
			this->terminate = true;
			::SetEvent(henter);
			::SetEvent(hremove);

			return true;
			}
	};

#endif
