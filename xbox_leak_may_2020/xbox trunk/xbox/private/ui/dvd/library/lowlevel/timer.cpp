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


// FILE:      library\lowlevel\timer.cpp
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   17.03.95
//
// PURPOSE: The timer class.
//
// HISTORY:

#include "timer.h"








// Global Timer Variable
#if VXD_VERSION
TimerClass * globalTimer;
#else
TimerClass Timer;
#endif

// Global variables for static data members of TimerClass
int TimerClass::timerClassInited;
int TimerClass::nanoDelay;

#if !NT_KERNEL

// Software delay loops have different speed on different processors. Accurate
// timing can be achieved by measuring the time delays in the following way:
// The loop in "nanoSleep" is very short and accurate. We time a lot of runs
// through it and look how long it took. Based on that information, we can
// calculate the parameter we need to provide "nanoSleep" to have it last for
// one microsecond.

static void nanoSleep (int delay)
	{
	_asm
		{
			mov		ecx, delay
		l1:	loop	l1
		}
	}

#endif




// Pause for a specified number of microseconds.

void TimerClass::WaitMicroSecs (DWORD micros)
	{
#if NT_KERNEL
	KeStallExecutionProcessor(micros);
#else
	int nanoDelay = this->nanoDelay;
	__asm
		{
				mov	edx, DWORD PTR [micros]
				mov	ebx, nanoDelay;
				sub	edx, 1
				jc		w0
		l0:	mov	ecx, ebx
		l1:   sub	ecx, 1
				jne	l1
				sub	edx, 1
				jnc	l0
		w0:
		}
#endif
	}

#if NT_KERNEL

static DWORD GetMillis(void)
	{
	LARGE_INTEGER val;
	DWORD freq;

	KeQueryTickCount(&val);
	freq = KeQueryTimeIncrement();

//	val = KeQueryPerformanceCounter(&freq);

	return (DWORD) (val.QuadPart * freq / 10000);
	}

static DWORD GetMicros(void)
	{
	LARGE_INTEGER val, freq;

	val = KeQueryPerformanceCounter(&freq);

	return (DWORD) (val.QuadPart * 1000000 / freq.QuadPart);
	}

void TimerClass::WaitMilliSecs (DWORD millis)
	{
	LARGE_INTEGER delay;

	DWORD startTime = GetMillis() + millis;

	delay.QuadPart = - (LONGLONG) millis * (LONGLONG) 10000; // unit is 100 ns

	if (STATUS_SUCCESS != KeDelayExecutionThread(KernelMode, FALSE, &delay))
		{
		// Do it the bad way if KeDelayExecutionThread was interrupted by APC.
		// We are mean.
		while (startTime > GetMillis()) WaitMicroSecs(500);
		}
	}

DWORD TimerClass::GetMilliTicks (void)
	{
	return GetMillis();
	}

DWORD TimerClass::GetMicroTicks (void)
	{
	return GetMicros();
	}


// The constructor. We measure how long loops on this processor take.
// Thus we can time short intervals in the microsecond range.

TimerClass::TimerClass (void)
	{
	}

#else

#if VXD_VERSION

DWORD	TimerClass::GetMilliTicks (void)
	{
	return Get_System_Time();
	}

// Sometimes milliseconds are enough resolution
void TimerClass::WaitMilliSecs (DWORD millis)
	{
	}

void VxDTimerClass::WaitMilliSecs(DWORD millis)
	{
	_VWIN32_WaitSingleObject(vxdTimerHandle, millis, FALSE);
	}

#else
// Get the number of microseconds elapsed since Windows started. Note that
// there's no DWORD wrap around since Windows can't run stable for 49 days.

DWORD TimerClass::GetMilliTicks (void)
	{
	return ::timeGetTime();
	}


// Sometimes milliseconds are enough resolution
void TimerClass::WaitMilliSecs (DWORD millis)
	{
	Sleep(millis);
	}

#endif

DWORD TimerClass::GetMicroTicks (void)
	{
	LARGE_INTEGER	count, freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&count);
	return (DWORD)(count.QuadPart * 1000 / (freq.QuadPart / 1000));
	}

// The constructor. We measure how long loops on this processor take.
// Thus we can time short intervals in the microsecond range.

TimerClass::TimerClass (void)
	{
	DWORD start, end, delta, delay;

	if (timerClassInited != TRUE)
		{
		timerClassInited = TRUE;
		nanoDelay = 1000;

		delay = 100;

		do {
			delay *= 10;

			start = GetMilliTicks();
			WaitMicroSecs(delay);
			end = GetMilliTicks();
			} while (end - start < 10);

		nanoDelay = (int)( delay / (end-start) );

		start = GetMilliTicks();
		WaitMicroSecs(40000);
		delta = GetMilliTicks() - start;

		if (nanoDelay && (delta < 20 || delta > 80))
			{
			nanoDelay = nanoDelay * 40 / (int)delta;
			}

		if (delta < 40)
			{
			do
				{
				nanoDelay++;
				start = GetMilliTicks();
				WaitMicroSecs(40000);
				delta = GetMilliTicks() - start;
				} while (delta < 40);
			}
		else if (delta > 40)
			{
			do
				{
				nanoDelay--;
				start = GetMilliTicks();
				WaitMicroSecs(40000);
				delta = GetMilliTicks() - start;
				} while (delta > 40);
			nanoDelay++;
			}
		}
	}

#endif // not NT_KERNEL branch



