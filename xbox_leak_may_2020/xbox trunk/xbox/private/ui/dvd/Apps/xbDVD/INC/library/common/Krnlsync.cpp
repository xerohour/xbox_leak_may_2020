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
// FILE:      library\common\krnlsync.cpp
// AUTHOR:    Viona
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:

#include "krnlsync.h"

#include "vddebug.h"





#if NT_KERNEL


IRQMutex::IRQMutex(void)
	{
	KeInitializeSpinLock(&lock);
	KeInitializeEvent(&event, SynchronizationEvent, TRUE);
	block = TRUE;
	}


void IRQMutex::Enter(void)
	{
	LARGE_INTEGER	li;

	if (KeGetCurrentIrql() < DISPATCH_LEVEL)
		{
		li.QuadPart = 0x7fffffffffffffff;

		for(;;)
			{
			KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, &li);

			KeAcquireSpinLock(&lock, &irql);

			if (block) break;

			KeReleaseSpinLock(&lock, irql);
			}

		atIRQLevel = FALSE;
		}
	else
		{
		KeAcquireSpinLockAtDpcLevel(&lock);
		atIRQLevel = TRUE;
		}
	}


void IRQMutex::Leave(void)
	{
	if (!atIRQLevel)
		{
		KeReleaseSpinLock(&lock, irql);
		KeSetEvent(&event, 0, FALSE);
		}
	else
		KeReleaseSpinLockFromDpcLevel(&lock);
	}


void IRQMutex::EnterAtIRQLevel(void)
	{
	KeAcquireSpinLockAtDpcLevel(&lock);
	KeResetEvent(&event);
	block = FALSE;
	KeReleaseSpinLockFromDpcLevel(&lock);
	}


void IRQMutex::LeaveAtIRQLevel(void)
	{
	KeAcquireSpinLockAtDpcLevel(&lock);
	block = TRUE;
	KeSetEvent(&event, 0, FALSE);
	KeReleaseSpinLockFromDpcLevel(&lock);
	}


#else


VDMutex::VDMutex(LPCTSTR name)
	{
//	DP(__TEXT("*** Create: %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	mutex = ::CreateMutex(NULL, FALSE, name);
//	DP(__TEXT("*** C-    : %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	}

VDMutex::VDMutex(void)
	{
	VDMutex(NULL);
	}

VDMutex::~VDMutex(void)
	{
//	DP(__TEXT("*** Destroy: %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	if (mutex)
		CloseHandle(mutex);
//	DP(__TEXT("*** D-     : %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	}

void VDMutex::Enter(void)
	{
//	DP(__TEXT("*** Enter : %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	::WaitForSingleObject(mutex, INFINITE);
//	DP(__TEXT("*** E-,   : %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	}

void VDMutex::Leave(void)
	{
//	DP(__TEXT("*** Leave : %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	::ReleaseMutex(mutex);
//	DP(__TEXT("*** L-    : %lx, %lx"), ::GetCurrentProcessId(), ::GetCurrentThreadId());
	}

VDSemaphore::VDSemaphore (void)
	{
	sema = ::CreateSemaphore(NULL, NULL, 1000000, NULL);
	}

VDSemaphore::~VDSemaphore (void)
	{
	if (sema)
		CloseHandle(sema);
	}

void VDSemaphore::Reset (void)
	{
	do {} while (::WaitForSingleObject(sema, 0) != WAIT_TIMEOUT);
	}

void VDSemaphore::Signal (void)
	{
	::ReleaseSemaphore(sema, 1, NULL);
	}

void VDSemaphore::Wait (void)
	{
	::WaitForSingleObject(sema, INFINITE);
	}

VDTimedSemaphore::VDTimedSemaphore(void)
	: VDSemaphore()
	{
	}

VDTimedSemaphore::~VDTimedSemaphore(void)
	{
	}

void VDTimedSemaphore::Reset (void)
	{
	do {} while (::WaitForSingleObject(sema, 0) != WAIT_TIMEOUT);
	}

void VDTimedSemaphore::Wait (void)
	{
	::WaitForSingleObject(sema, INFINITE);
	}

void VDTimedSemaphore::WaitTimeout (DWORD time, BOOL & timeout)
	{
	timeout = ::WaitForSingleObject(sema, time / 1000) == WAIT_TIMEOUT;
	}

#endif	// of NT_KERNEL





VDCriticalSection::VDCriticalSection(void)
	{
	scheduled = FALSE;
	cnt = 0;
#if NT_KERNEL
	KeInitializeMutex (&mutex, 0);
#endif
	}


void VDCriticalSection::EnterSection(void)
	{
#if NT_KERNEL
	KeWaitForSingleObject (&mutex, Executive, KernelMode, FALSE, NULL);
	cnt++;
//	DP("ES %x %d", KeGetCurrentThread(), cnt);
#else
	cnt++;
#endif
	}

void VDCriticalSection::LeaveSection(void)
	{
#if NT_KERNEL
	cnt--;
	if (!cnt)
		{
//		DP("LS1 %x %d", KeGetCurrentThread(), cnt);
		KeReleaseMutex (&mutex, FALSE);
		while (scheduled)
			{
//			DP("SCHED1");
			KeWaitForSingleObject (&mutex, Executive, KernelMode, FALSE, NULL);
			cnt++;
			if (scheduled)
				{
				scheduled = FALSE;
				CriticalSection ();
				}
			cnt--;
			KeReleaseMutex (&mutex, FALSE);
			}
		}
	else
		{
//		DP("LS2 %x %d", KeGetCurrentThread(), cnt);
		KeReleaseMutex (&mutex, FALSE);
		}
#else
	cnt--;
	if (!cnt)
		{
		while (scheduled)
			{
			cnt++;
			if (scheduled)
				{
				scheduled = FALSE;
				CriticalSection();
				}
			cnt--;
			}
		}
#endif
	}

void VDCriticalSection::ScheduleSection(void)
	{
#if NT_KERNEL
	LARGE_INTEGER timeout;
	timeout.QuadPart = 0;
	BOOL success;
	success = (STATUS_SUCCESS == KeWaitForSingleObject (&mutex, Executive, KernelMode, FALSE, &timeout));
	if (! success)
		{
//		DP("SS1 %x %d", KeGetCurrentThread(), cnt);
		scheduled = TRUE;
		}
	else
		{
//		DP("SS2 %x %d", KeGetCurrentThread(), cnt);
		if (!cnt)
			{
			scheduled = FALSE;
			CriticalSection ();
			}
		else
			scheduled = TRUE;

		KeReleaseMutex (&mutex, FALSE);
		}
#else
	if (cnt)
		scheduled = TRUE;
	else
		{
		scheduled = FALSE;
		CriticalSection();
		}
#endif
	}
