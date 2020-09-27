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

#ifndef TIMEDFIBERS_H
#define TIMEDFIBERS_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "library\common\profiles.h"

class TimedFiber
	{
	friend class TimedFiberQueue;
	friend class TimedFiberEvent;
	friend class TimedFiberMutex;
	protected:
		void					*	stack, * esp, * minstack;

		int						stackSize;

		TimedFiber			*	succ,	*	timeSucc;
		TimedFiberEvent	*	waitingFor;
		int						priority;
		int						timeout;
		bool						success;
		bool						precise;
		int						id;

		volatile enum State {passive, initial, running, ready, waiting, blocked} state;

		friend void RescheduleFibers(void);

		virtual void FiberRoutine(void) {};
	public:
		TimedFiber(int id, int stackSize = 32768);
		~TimedFiber(void);

		void StartFiber(void);
		void CompleteFiber(void);

		void YieldFiber(int millis, bool precise = FALSE);

		bool IsHigherPriorityReady(int id = -1);

		void SetFiberPriority(int priority);
	};

class TimedFiberQueue
	{
	protected:
		TimedFiber	*	tail;
	public:
		TimedFiberQueue(void);

		void EnterFiber(TimedFiber * fiber);
		void RemoveFiber(TimedFiber * fiber);
		TimedFiber * RemoveHead(void);

		bool IsEmpty(void) {return tail == NULL;}
	};

class TimedFiberEvent
	{
	protected:
		bool					set, autoReset;

		TimedFiberQueue	waiting;
	public:
		TimedFiberEvent(bool initial, bool autoReset);

		bool StatusEvent(void);

		void SetEvent(void);
		void ResetEvent(void);

		bool Wait(int millis, bool precise = FALSE);

		void WaitForever(void);

		void RemoveTimeout(TimedFiber * fiber);
	};

class TimedFiberMutex
	{
	protected:
		DWORD					ownerID;
		TimedFiber		*	owner;
		int					useCount;
		TimedFiberQueue	waiting;
		HANDLE				event;
		bool					shared;
	public:
		TimedFiberMutex(void);
		~TimedFiberMutex(void);

		void Enter(bool exclusive = true);

		void Leave(void);
	};

void InitializeTimedFiberScheduler(GenericProfile * globalProfile, GenericProfile * profile);

void FinishTimedFiberScheduler(void);

void SetTimedFiberPriorities(bool high);

TimedFiber * CurrentTimedFiber(void);

void YieldTimedFiber(int delay, bool precise = FALSE);

bool IsHigherPriorityTimedFiberReady(int id = -1);

extern DWORD timeDivider;
extern DWORD msbTimeSubtract;
extern bool systemTimeSource;

#pragma warning(disable : 4035)

__forceinline DWORD GetCPUTime(void)
	{
	__asm
		{
		_emit 0x0f
		_emit	0x31

		sub	edx, [msbTimeSubtract]
		div	[timeDivider]
		}
	}

inline void ReadPerformanceCounter(__int64 & val)
	{
	__asm
		{
		xor	eax, eax
		xor	edx, edx

		_emit 0x0f
		_emit	0x31

		mov	ecx, [val]
		mov	[ecx], eax
		mov	[ecx+4], edx
		}
	}

#pragma warning(default : 4035)

inline int GetInternalTime(void)
	{
	if (systemTimeSource)
		return (int)timeGetTime();
	else
		return (int)GetCPUTime();
	}

#endif
