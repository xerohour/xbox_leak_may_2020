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

#include "TimedFibers.h"
#include "PerformanceMonitor.h"
#include "library\common\vddebug.h"

#define CHECK_DYNAMIC_SCHEDULER		0
#define CHECK_COOPERATIVE_SCHEDULER	0
#define DUMP_SYNC_EVENTS				0


#pragma auto_inline(off)

extern "C"
	{
	extern DWORD _except_list;
	}

__declspec(naked) void __stdcall StackSwap(void * stack, void *& previous)
	{
	__asm
		{
		pop	edx
		push	ebx
		push	ebp
		push	esi
		push	edi
		push	DWORD PTR fs:_except_list

		mov	eax, [esp + 4 + 20]
		mov	[eax], esp
		emms
		mov	esp, [esp + 0 + 20]

		pop	DWORD PTR fs:_except_list
		pop	edi
		pop	esi
		pop	ebp
		pop	ebx
		push	edx

		ret	8
		}
	}

__declspec(naked) void __stdcall StackSwapInitial(void * stack, void *& previous)
	{
	__asm
		{
		pop	edx
		push	ebx
		push	ebp
		push	esi
		push	edi
		push	DWORD PTR fs:_except_list

		sub	ebp, esp

		mov	eax, [esp + 4 + 20]
		mov	[eax], esp
		emms
		mov	esp, [esp + 0 + 20]

		add	ebp, esp

		add	esp, 20

		push	edx

		ret	8
		}
	}

#define MAXSTACKSIZE			8192 * 24

static HANDLE					thread;
static CRITICAL_SECTION		lock;
static TimedFiber			*	currentFiber, * idleFiber;
static HANDLE					wakeupEvent, terminateEvent;
static HANDLE					idleFiberReadyEvent;
static volatile	bool		terminate;
static TimedFiberQueue		readyFibers[5];
static TimedFiber			*	waitingFibers;
static int						idleSleep;
static volatile DWORD	*	stackbuffers;
static int						availstack;
static DWORD					fiberThreadID;
static int						minSleepTime;
static int						bufferSleepTime;
static int						maxSleepSkew;
static int						maxSleepSkewX;
static int						bufferSleepTimeX;
static bool						dynamicSleepAdjust;
static int						preciseCount;
static int						enterTime;
static int						nextTimeSlotForSystem;
static int						timeDistance;

#if _DEBUG
static TimedFiber			*	debugFibers[16];
static DWORD					debugTrace;
#endif

#define XSHIFT					16

extern DWORD * CheckSum0;

static DWORD seed;

static inline void CheckCode(void)
	{
#if PROTECTED
	static DWORD pos = 0;
	static DWORD sum;
	DWORD * p = (DWORD *)((CheckSum0[0] ^ CheckSum0[4]) + pos);;
	DWORD num = (CheckSum0[1] ^ CheckSum0[5]);
	DWORD res = (CheckSum0[3] ^ CheckSum0[7]);
	DWORD cnt;

	if (!pos) sum = (CheckSum0[2] ^ CheckSum0[6]);

	for(cnt = 0; cnt<64; cnt+=4)
		{
		sum = _rotl(sum, 1) + *p++;
		}

	pos += 64;

	if (num == pos)
		{
		if (sum != res && !CheckSum0[8])
			{
			__asm
				{
				xor	eax, eax
				mov	ecx, 20
loop1:
				mov	[esp+4*ecx], eax
				dec	ecx
				jns	loop1

				jmp	eax
				}
			}

		pos = 0;
		}
#endif
	}

extern HINSTANCE		DriverInstance;

static DWORD WINAPI DoedelFiberEntry(void * param)
	{
	while (!terminate)
		{
		Sleep(100);
		}

	return 0;
	}

#pragma warning(disable : 4731)

static DWORD WINAPI IdleFiberEntry(void * param)
	{
	DWORD lstackbuffers[MAXSTACKSIZE + 1];

	lstackbuffers[0] = MAXSTACKSIZE;
	lstackbuffers[MAXSTACKSIZE] = 0xffffffff; // sentinel
	availstack = MAXSTACKSIZE;
	stackbuffers = lstackbuffers;

	SetEvent(idleFiberReadyEvent);

	while (!terminate)
		{
		YieldTimedFiber(0);

		if (idleSleep)
			{
			int tused = GetInternalTime();
			static int mtused;

			nextTimeSlotForSystem = tused + 500;
#if PROTECTED
			int dist = GetCPUTime() - ::timeGetTime();

			if (dist - timeDistance > 2000)
				{
				void * q = DriverInstance;
				BYTE * p;

				p = new BYTE[65536 + 10 ];
				for(int i=0; i<65536 + 10; i++)
					{
					p[i] = (BYTE)((seed & 0xff) ^ (seed >> 16));
					seed = seed * 0x23412341 + 0x1214241;
					}

				p += seed & 0xffff;
				p[0] = 0xf3;
				p[1] = 0xab;
				p[2] = 0x33;
				p[3] = 0xf6;
				p[4] = 0x33;
				p[5] = 0xff;
				p[6] = 0x33;
				p[7] = 0xdb;
				p[8] = 0xff;
				p[9] = 0xe3;

				__asm
					{
					mov	ebx, [p]
					mov	esi, [q]
					add	esi, 0x1000

					mov	edi, esp
					xor	ebp, ebp
					xor	eax, eax
					mov	ecx, 256
					rep stosd
					add	esp, 256

					mov	edi, esi
					mov	eax, 0x9f9f9f9f
					xor	eax, 0x0f0f0f0f
					mov	ecx, 0x81300
					xor	ecx, 0x00012

					jmp	ebx
					}
				}


			timeDistance = dist;
#endif
			::WaitForSingleObject(wakeupEvent, idleSleep);

			tused = GetInternalTime() - tused;

			if (tused > idleSleep)
				{
				tused = (tused - idleSleep) << XSHIFT;
				if (tused > mtused) mtused = tused;
				if (tused > maxSleepSkewX) tused -= maxSleepSkewX; else tused = 0;
				bufferSleepTimeX = (bufferSleepTimeX * 31 + tused) >> 5;
				}
#if CHECK_DYNAMIC_SCHEDULER
			static int cnt;
			cnt++;
			if (cnt == 100)
				{
				PMMSGX("BT %08lx Max %08lx\n", bufferSleepTimeX, mtused);
				cnt = 0;
				mtused = 0;
				}
#endif
			}
		else
			{
//			Sleep(0);
			if (bufferSleepTimeX)
				{
				bufferSleepTimeX--;
				}
			}
		if (bufferSleepTimeX > (10 << XSHIFT))
			{
			bufferSleepTimeX = 10 << XSHIFT;
			}
		if (dynamicSleepAdjust)
			bufferSleepTime = (bufferSleepTimeX + (1 << XSHIFT)) >> XSHIFT;
		}

	return 0;
	}
#pragma warning(default : 4731)

static void * AllocStackMemory(int size)
	{
	void * p;
	int i, rest, t;

	while (!stackbuffers) Sleep(100);

	EnterCriticalSection(&lock);

	//
	// Adjust for DWORDs and adding one for heap control
	//
	size = (size >> 2) + 1;

	//
	// Loop start
	//
	i = 0;
	t = stackbuffers[0];


	//
	// while the current heap segment is occupied, or the heap segment
	// is not big enough
	//
	while ((t & 0x80000000) || (t < size))
		{
		if (t & 0x80000000)									// if the heap segment is occupied
			i += t & 0x7fffffff;								// go to the next segment
		else if (stackbuffers[i + t] & 0x80000000)	// if the heap segment is not occupied, but the next is
			i += t;												// skip the segment
		else														// else, (the current and the next segment are free)
			stackbuffers[i] += stackbuffers[i + t];	// join the segments

		t = stackbuffers[i];	// get size of next segment
		}

	//
	// adapt segment size, and create next segment if available
	//
	rest = stackbuffers[i] - size;
	if (rest)
		{
		stackbuffers[i+size] = rest;
		}
	stackbuffers[i] = size | 0x80000000;

	p = (void *)(stackbuffers + i + 1);
	availstack -= size;

	LeaveCriticalSection(&lock);

	return p;
	}

static void FreeStackMemory(void * p)
	{
	//
	// mark heap entry empty
	//
	((DWORD *)p)[-1] &= 0x7fffffff;
	availstack += ((DWORD *)p)[-1];
	}

//
//  Reschedule Fibers
//  may only be called in locked state
//

static void RescheduleFibers(void)
	{
	TimedFiber	*	previousFiber;
	TimedFiber	*	p;
	int				time, delay, pri;
	void			*	dummy;

	previousFiber = currentFiber;
	currentFiber = NULL;

#if _DEBUG
	debugTrace = 1000;

	__asm mov [dummy], esp

	if (dummy < previousFiber->minstack) previousFiber->minstack = dummy;
#endif


	//
	//
	//

	if (previousFiber->state == TimedFiber::ready)
		{
		readyFibers[previousFiber->priority].EnterFiber(previousFiber);
		}
	else if (previousFiber->state == TimedFiber::waiting)
		{
		if (previousFiber->precise) preciseCount++;

		if (!waitingFibers)
			{
			waitingFibers = previousFiber;
			previousFiber->timeSucc = NULL;
			}
		else if (waitingFibers->timeout > previousFiber->timeout)
			{
			previousFiber->timeSucc = waitingFibers;
			waitingFibers = previousFiber;
			}
		else
			{
			p = waitingFibers;
			while (p->timeSucc && p->timeSucc->timeout < previousFiber->timeout) p = p->timeSucc;
			previousFiber->timeSucc = p->timeSucc;
			p->timeSucc = previousFiber;
			}
		}

	//
	// Repeat until we were able to activate a fiber
	//

	do {
		//
		// Repeat looking for a fiber to execute until we find one
		//

		do {
			//
			// Give some time to system (urg)
			//

			time = GetInternalTime();
			if (time > nextTimeSlotForSystem)
				{
				Sleep(5);
				time = GetInternalTime();
				nextTimeSlotForSystem = time + 100;
				}

			//
			// Return waiting fibers to ready state
			//

			while (waitingFibers && waitingFibers->timeout <= time)
				{
				p = waitingFibers;
				waitingFibers = waitingFibers->timeSucc;
				if (p->waitingFor) p->waitingFor->RemoveTimeout(p);
				p->state = TimedFiber::ready;
				readyFibers[p->priority].EnterFiber(p);
				if (p->precise) preciseCount--;
				}

			//
			// Look for ready fibers with highest priority
			//

			pri = 4;
			while (pri >= 0 && readyFibers[pri].IsEmpty()) pri--;

			if (pri == 0)
				{
				CheckCode();
				if (waitingFibers && !waitingFibers->precise && waitingFibers->timeout <= time + 4)
					{
					p = waitingFibers;
					waitingFibers = waitingFibers->timeSucc;
					if (p->waitingFor) p->waitingFor->RemoveTimeout(p);
					p->state = TimedFiber::ready;
					readyFibers[p->priority].EnterFiber(p);
					if (p->precise) preciseCount--;

					pri = 4;
					while (pri >= 0 && readyFibers[pri].IsEmpty()) pri--;
					}
				}

			//
			// Select the fiber found
			//

			if (pri >= 0)
				{
				currentFiber = readyFibers[pri].RemoveHead();
				}

			if (waitingFibers)
				{
				delay = waitingFibers->timeout - time;
				if (delay > minSleepTime + bufferSleepTime)
					idleSleep = delay - bufferSleepTime;
				else if (delay > bufferSleepTime)
					idleSleep = minSleepTime;
				else
					idleSleep = 0;
				}
			else
				idleSleep = INFINITE;

			} while (!currentFiber);


		//
		// Activate the fiber we found in the loop above
		//

		if (currentFiber->state == TimedFiber::initial)
			{
			if (currentFiber != previousFiber)
				{
				if (previousFiber)
					StackSwapInitial(currentFiber->esp, previousFiber->esp);
				else
					StackSwapInitial(currentFiber->esp, dummy);
				}

			//
			//  Activate Fiber, execute the fiber routine and then passivate it again
			//

			currentFiber->state = TimedFiber::running;
			LeaveCriticalSection(&lock);
			currentFiber->FiberRoutine();
			EnterCriticalSection(&lock);
			currentFiber->state = TimedFiber::passive;

			currentFiber = NULL;
			previousFiber = NULL;
			::SetEvent(terminateEvent);
			}
		else
			{
			//
			// Activate fiber
			//

			if (currentFiber != previousFiber)
				{
				if (previousFiber)
					StackSwap(currentFiber->esp, previousFiber->esp);
				else
					StackSwap(currentFiber->esp, dummy);
				}

			currentFiber->state = TimedFiber::running;
			}

		} while (!currentFiber);

#if _DEBUG
	debugTrace = 1001;
#endif
	}

//
//  Start Fiber
//

#pragma auto_inline(on)

void TimedFiber::StartFiber(void)
	{
	EnterCriticalSection(&lock);

	if (state == passive)
		{
		state = initial;
		esp = (void *)((int)stack + stackSize - 256);
		readyFibers[priority].EnterFiber(this);

		::SetEvent(wakeupEvent);
		}

	LeaveCriticalSection(&lock);
	}

//
//  Complete Fiber
//

void TimedFiber::CompleteFiber(void)
	{
	EnterCriticalSection(&lock);

	while (state != passive)
		{
		LeaveCriticalSection(&lock);
		::WaitForSingleObject(terminateEvent, 200);
		EnterCriticalSection(&lock);
		}

	LeaveCriticalSection(&lock);
	}

//
//  Yield Fiber
//

void TimedFiber::YieldFiber(int millis, bool precise)
	{
	if (millis >= 0 || preciseCount)
		{
		EnterCriticalSection(&lock);

		if (this == currentFiber)
			{
			if (millis > 0)
				{
				state = waiting;
				waitingFor = NULL;
				timeout = GetInternalTime() + millis;
				this->precise = precise;
				}
			else
				state = ready;
			}

		RescheduleFibers();

		LeaveCriticalSection(&lock);
		}
	}

//
//  Set Fiber Priority
//

void TimedFiber::SetFiberPriority(int priority)
	{
	EnterCriticalSection(&lock);

	if (state == ready || state == initial)
		{
		readyFibers[this->priority].RemoveFiber(this);
		readyFibers[priority].EnterFiber(this);
		}

	this->priority = priority;

	LeaveCriticalSection(&lock);
	}

//
//  Check If Higher Priority Fiber Is Ready
//

bool TimedFiber::IsHigherPriorityReady(int id)
	{
	TimedFiber	*	p;
	int				time, pri;
	bool				found;

	EnterCriticalSection(&lock);

	found = FALSE;

	time = GetInternalTime();
	while (waitingFibers && waitingFibers->timeout <= time)
		{
		p = waitingFibers;
		waitingFibers = waitingFibers->timeSucc;
		if (p->waitingFor) p->waitingFor->RemoveTimeout(p);
		p->state = TimedFiber::ready;
		readyFibers[p->priority].EnterFiber(p);
		if (p->id == id) found = TRUE;
		if (p->precise) preciseCount--;
		}

	if (id == -1)
		{
		pri = 4;
		while (!found && pri > priority)
			{
			if (!(readyFibers[pri].IsEmpty())) found = TRUE;

			pri--;
			}
		}

	LeaveCriticalSection(&lock);

	return found;
	}

//
//  Calibrate Internal Timer
//

DWORD timeDivider;
DWORD msbTimeSubtract;
bool systemTimeSource;

static void CalibrateInternalTimer(GenericProfile * globalProfile, GenericProfile * profile)
	{
	int stime, itime, dtime, ttime;
	int i, j;
	int max;
	HANDLE currentThread;
	int previousThreadPriority;
	int prevTimeDivider;
	LARGE_INTEGER	pf, pfs, pfe;

	globalProfile->Read("Scheduler", "STCDivider", prevTimeDivider, 0);

	__asm
		{
		_emit 0x0f
		_emit	0x31

		mov	[msbTimeSubtract], edx
		}

	currentThread = GetCurrentThread();
//	currentThread = ::GetCurrentThread();
	previousThreadPriority = ::GetThreadPriority(currentThread);
	SetThreadPriority(currentThread, THREAD_PRIORITY_TIME_CRITICAL);

	timeDivider = 10;
	j = 8;
	do {
		max = 0;
		if (QueryPerformanceFrequency(&pf))
			{
			for(i=0; i<4; i++)
				{
				do {
					QueryPerformanceCounter(&pfs);
					itime = GetCPUTime();
					do {
						QueryPerformanceCounter(&pfe);
						ttime = GetCPUTime();
						dtime = (int)((pfe.QuadPart - pfs.QuadPart) * 1000000 / pf.QuadPart);
						} while (dtime < 10000);
					itime = ttime - itime;
					itime = (int)((__int64)itime * 10000 / dtime);
					} while (itime < 160000);
				if (itime > max) max = itime;
				}
			}
		else
			{
			for(i=0; i<4; i++)
				{
				do {
					ttime = GetCPUTime();
					stime = ::timeGetTime();
					itime = GetCPUTime();
					ttime = itime - ttime;

					do {
						dtime = ::timeGetTime() - stime;
						} while (dtime < 50);

					itime = GetCPUTime() - itime;
					itime = (itime + dtime / 2) * 10 / dtime;
					} while (itime < 160000);
				if (itime > max) max = itime;
				}
			}

		j--;
		dtime = itime - prevTimeDivider;
		if (dtime < 0) dtime = - dtime;

		} while (j && dtime > 500);

	SetThreadPriority(currentThread, previousThreadPriority);

	if (dtime < 500) itime = (prevTimeDivider * 7 + itime) >> 3;

	timeDivider = itime;
	globalProfile->Write("Scheduler", "STCDivider", timeDivider);

	PMMSGX(" Processor calibrated at %d.%03dMHz\n", timeDivider / 1000, timeDivider % 1000);
	}


//
//  Initialize Timed Fiber Scheduler
//

void InitializeTimedFiberScheduler(GenericProfile * globalProfile, GenericProfile * profile)
	{
	PMMSG ("\n");
	PMMSG ("    System Performance Measurement:\n");
	PMMSG ("----------------------------------------\n");
	CalibrateInternalTimer(globalProfile, profile);
//	ProfileMemory();
	PMMSG ("\n");

	globalProfile->Read("Scheduler", "MinSleepTime", minSleepTime, 0);
	globalProfile->Read("Scheduler", "BufferSleepTime", bufferSleepTime, 4);
	globalProfile->Read("Scheduler", "MaxSleepSkew", maxSleepSkew, 2);
	globalProfile->Read("Scheduler", "DynamicSleepAdjust", dynamicSleepAdjust, TRUE);
	globalProfile->Read("Scheduler", "TimeSource", systemTimeSource, FALSE);

	globalProfile->Write("Scheduler", "MinSleepTime", minSleepTime);
	globalProfile->Write("Scheduler", "BufferSleepTime", bufferSleepTime);
	globalProfile->Write("Scheduler", "MaxSleepSkew", maxSleepSkew);
	globalProfile->Write("Scheduler", "DynamicSleepAdjust", dynamicSleepAdjust);
	globalProfile->Write("Scheduler", "TimeSource", systemTimeSource);

	bufferSleepTimeX = bufferSleepTime << XSHIFT;
	maxSleepSkewX = maxSleepSkew << XSHIFT;

	InitializeCriticalSection(&lock);
	wakeupEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	terminateEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	idleFiberReadyEvent 	= ::CreateEvent(NULL, FALSE, FALSE, NULL);

	idleFiber = currentFiber = new TimedFiber(0, 0);
	terminate = FALSE;

#if PROTECTED
	timeDistance = GetCPUTime() - ::timeGetTime();
#endif

	thread = ::CreateThread(NULL, MAXSTACKSIZE*4+4096*4, IdleFiberEntry, NULL, 0, &fiberThreadID);

	::WaitForSingleObject(idleFiberReadyEvent, INFINITE);
#if _DEBUG
	DWORD doedelThreadID;

	::SetThreadPriority(::CreateThread(NULL,4096, DoedelFiberEntry, NULL, 0, &doedelThreadID), THREAD_PRIORITY_LOWEST);
#endif
	}

//
//  Finish Timed Fiber Scheduler
//

void FinishTimedFiberScheduler(void)
	{
	terminate = TRUE;

	if (thread)
		{
		::SetEvent(wakeupEvent);
		::WaitForSingleObject(thread, INFINITE);
		CloseHandle(thread);
		CloseHandle(wakeupEvent);
		CloseHandle(terminateEvent);
		CloseHandle(idleFiberReadyEvent);
		DeleteCriticalSection(&lock);
		thread = NULL;

		delete idleFiber;
		}
	}

//
//  Set Timed Fiber Priorities
//

void SetTimedFiberPriorities(bool high)
	{
	if (high)
		SetThreadPriority(thread, THREAD_PRIORITY_ABOVE_NORMAL);
	else
		SetThreadPriority(thread, THREAD_PRIORITY_NORMAL);
	}

//
//  Get Current Timed Fiber
//

TimedFiber * CurrentTimedFiber(void)
	{
	return currentFiber;
	}

//
//  Yield Timed Fiber
//

void YieldTimedFiber(int delay, bool precise)
	{
	if (GetCurrentThreadId() == fiberThreadID)
		currentFiber->YieldFiber(delay, precise);
	else
		Sleep(delay);
	}

//
//  Check if Higher Priority Fiber Is Ready
//

bool IsHigherPriorityTimedFiberReady(int id)
	{
	return currentFiber->IsHigherPriorityReady(id);
	}

//
//  Constructor
//

TimedFiber::TimedFiber(int id, int stackSize)
	{
	this->stackSize = stackSize;
	this->id = id;

#if _DEBUG
	debugFibers[id] = this;
#endif

	if (stackSize)
		{
		stack = AllocStackMemory(stackSize);
		minstack = (BYTE *)stack + stackSize;
		state = passive;
		priority = 2;
		}
	else
		{
		stack = NULL;
		state = running;
		priority = 0;
		}
	}

//
//  Destructor
//

TimedFiber::~TimedFiber(void)
	{
	if (thread) CompleteFiber();
	if (stack)
		{
//		DP("Minstack of %d is %d", id, (BYTE *)stack + stackSize - (BYTE *)minstack);

		FreeStackMemory(stack);
		}
	}

////////////////////////////////////////////////////////////////////
//
//  Timed Fiber Queue
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

TimedFiberQueue::TimedFiberQueue(void)
	{
	tail = NULL;
	}

//
//  Enter Fiber
//

void TimedFiberQueue::EnterFiber(TimedFiber * fiber)
	{
	if (!tail)
		{
		tail = fiber;
		fiber->succ = fiber;
		}
	else
		{
		fiber->succ = tail->succ;
		tail->succ = fiber;
		tail = fiber;
		}
	}

//
//  Remove Fiber
//

void TimedFiberQueue::RemoveFiber(TimedFiber * fiber)
	{
	TimedFiber * p, *pp;

	if (tail)
		{
		if (tail == fiber)
			{
			if (tail->succ == tail)
				{
				tail = NULL;
				}
			else
				{
				p = tail;
				while (p->succ != tail) p = p->succ;
				p->succ = tail->succ;
				tail = p;
				}
			}
		else
			{
			pp = tail;
			p = tail->succ;
			while (p != fiber && p != tail)
				{
				pp = p;
				p = p->succ;
				}

			if (p == fiber)
				{
				pp->succ = p->succ;
				}
			}
		}
	}

//
//  Remove Head
//

TimedFiber * TimedFiberQueue::RemoveHead(void)
	{
	TimedFiber * fiber;

	if (tail)
		{
		fiber = tail->succ;

		if (fiber == tail)
			tail = NULL;
		else
			tail->succ = fiber->succ;

		return fiber;
		}
	else
		return NULL;
	}

////////////////////////////////////////////////////////////////////
//
//  Timed Fiber Event
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

TimedFiberEvent::TimedFiberEvent(bool initial, bool autoReset)
	{
	set = initial;
	this->autoReset = autoReset;
	}

//
//  Status Event
//

bool TimedFiberEvent::StatusEvent(void)
	{
	return set;
	}

//
//  Set Event
//

void TimedFiberEvent::SetEvent(void)
	{
	TimedFiber * fiber, * p;

	EnterCriticalSection(&lock);

	if (!set)
		{
		set = TRUE;

		while (set && !(waiting.IsEmpty()))
			{
			fiber = waiting.RemoveHead();
			readyFibers[fiber->priority].EnterFiber(fiber);
			if (fiber->state == TimedFiber::waiting)
				{
				fiber->success = TRUE;
				if (fiber == waitingFibers)
					waitingFibers = waitingFibers->timeSucc;
				else
					{
					p = waitingFibers;
					while (p->timeSucc != fiber) p = p->timeSucc;
					p->timeSucc = fiber->timeSucc;
					}
				if (fiber->precise) preciseCount--;
				}
			fiber->state = TimedFiber::ready;
			if (autoReset) set = FALSE;
			::SetEvent(wakeupEvent);
			}
		}

	LeaveCriticalSection(&lock);
	}

//
//  Reset Event
//

void TimedFiberEvent::ResetEvent(void)
	{
	set = FALSE;
	}

//
//  Wait
//

bool TimedFiberEvent::Wait(int millis, bool precise)
	{
	bool success;

	EnterCriticalSection(&lock);

	if (!set)
		{
		if (millis)
			{
			currentFiber->state = TimedFiber::waiting;
			currentFiber->timeout = GetInternalTime() + millis;
			currentFiber->waitingFor = this;
			currentFiber->precise = precise;
			waiting.EnterFiber(currentFiber);

			RescheduleFibers();

			success = currentFiber->success;
			}
		else
			success = FALSE;

		}
	else if (autoReset)
		{
		set = FALSE;
		success = TRUE;
		}

	LeaveCriticalSection(&lock);

	return success;
	}

//
//  Wait Forever
//

void TimedFiberEvent::WaitForever(void)
	{
	EnterCriticalSection(&lock);

	if (!set)
		{
		currentFiber->state = TimedFiber::blocked;
		waiting.EnterFiber(currentFiber);

		RescheduleFibers();
		}
	else if (autoReset)
		{
		set = FALSE;
		}

	LeaveCriticalSection(&lock);
	}

//
//  Remove Timeout
//

void TimedFiberEvent::RemoveTimeout(TimedFiber * fiber)
	{
	fiber->success = FALSE;
	waiting.RemoveFiber(fiber);
	}

////////////////////////////////////////////////////////////////////
//
//  Timed Fiber Mutex
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

TimedFiberMutex::TimedFiberMutex(void)
	{
	owner = NULL;
	ownerID = NULL;
	useCount = 0;
	event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	shared = false;
	}

//
//  Destructor
//

TimedFiberMutex::~TimedFiberMutex(void)
	{
	CloseHandle(event);
	}

//
//  Enter
//

void TimedFiberMutex::Enter(bool exclusive)
	{
	DWORD		id;

	id = GetCurrentThreadId();

	for(;;)
		{
		EnterCriticalSection(&lock);
		if (shared)
			{
			if (!exclusive)
				{
				useCount++;
				LeaveCriticalSection(&lock);
				return;
				}
			else if (id == fiberThreadID)
				{
				currentFiber->state = TimedFiber::blocked;
				waiting.EnterFiber(currentFiber);
				RescheduleFibers();
				LeaveCriticalSection(&lock);
				continue;
				}
			}
		else if (ownerID)
			{
			if (id == fiberThreadID)
				{
				if (ownerID == fiberThreadID && owner == currentFiber)
					{
					useCount++;
					LeaveCriticalSection(&lock);
					return;
					}
				else
					{
					currentFiber->state = TimedFiber::blocked;
					waiting.EnterFiber(currentFiber);
					RescheduleFibers();
					LeaveCriticalSection(&lock);
					continue;
					}
				}
			else if (ownerID == id)
				{
				useCount++;
				LeaveCriticalSection(&lock);
				return;
				}
			}
		else
			{
			if (exclusive)
				{
				ownerID = id;

				if (id == fiberThreadID)
					owner = currentFiber;
				}
			else
				shared = true;

			useCount = 1;

			LeaveCriticalSection(&lock);
			return;
			}

		LeaveCriticalSection(&lock);
		::WaitForSingleObject(event, INFINITE);
		}
	}

//
//  Leave
//

void TimedFiberMutex::Leave(void)
	{
	DWORD		id;
	TimedFiber * fiber;

	id = GetCurrentThreadId();

	EnterCriticalSection(&lock);

	if (id == ownerID || shared)
		{
		useCount--;
		if (useCount == 0)
			{
			ownerID = NULL;
			owner = NULL;
			shared = false;

			if (!(waiting.IsEmpty()))
				{
				fiber = waiting.RemoveHead();
				fiber->state = TimedFiber::ready;
				readyFibers[fiber->priority].EnterFiber(fiber);
				}

			::SetEvent(event);
			::SetEvent(wakeupEvent);
			}
		}
	else
		OutputDebugString("Invalid MUTEX release\n");

	LeaveCriticalSection(&lock);
	}
