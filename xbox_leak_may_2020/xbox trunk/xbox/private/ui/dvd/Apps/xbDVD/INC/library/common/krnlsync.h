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
// FILE:      library\common\krnlsync.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#ifndef KRNLSYNC_H
#define KRNLSYNC_H

#include "prelude.h"





class IRQMutex
	{
	private:
#if NT_KERNEL
		KSPIN_LOCK	lock;
		KEVENT		event;
		BOOL			block;
		KIRQL			irql;
		BOOL			atIRQLevel;
#else
		volatile	BOOL	lock;
		BOOL				irq;
#endif
	public:
		IRQMutex(void);

		void Enter(void);
		void Leave(void);

		void EnterAtIRQLevel(void);
		void LeaveAtIRQLevel(void);
	};



class VDMutex
	{
	private:
#if   !NT_KERNEL && !VXD_VERSION
		int		count;
		int		requested;
		HANDLE	mutex;
#endif
	public:
		VDMutex(void);
		VDMutex(LPCTSTR name);
		~VDMutex(void);

		void Enter(void);
		void Leave(void);
	};

#if !NT_KERNEL && !VXD_VERSION
class VDLocalMutex
	{
	private:
		CRITICAL_SECTION	lock;
	public:
		VDLocalMutex(void) {InitializeCriticalSection(&lock);}
		~VDLocalMutex(void) {DeleteCriticalSection(&lock);}

		void Enter(void) {EnterCriticalSection(&lock);}
		void Leave(void) {LeaveCriticalSection(&lock);}
	};
#else
typedef VDMutex	VDLocalMutex;
#endif

class VDAutoMutex
	{
	protected:
		VDLocalMutex	*	mutex;
	public:
		VDAutoMutex(VDLocalMutex	*	mutex)
			{
			this->mutex = mutex;
			mutex->Enter();
			}

		~VDAutoMutex(void)
			{
			mutex->Leave();
			}
	};



class VDSemaphore
	{
	protected:
#if !NT_KERNEL && !VXD_VERSION
		HANDLE	sema;
#endif
	public:
		VDSemaphore (void);
		virtual ~VDSemaphore (void);

		virtual void Reset (void);
		virtual void Signal (void);
		virtual void Wait (void);
	};

class VDTimedSemaphore : public VDSemaphore
	{
	public:
		VDTimedSemaphore (void);
		virtual ~VDTimedSemaphore (void);

		virtual void Reset (void);
		virtual void Wait (void);

		void WaitTimeout (DWORD time, BOOL & timeout); // time in microsecs
	};



class VDCriticalSection
	{
	private:
		BOOL		scheduled;
		WORD		cnt;
#if NT_KERNEL
		KMUTEX	mutex;
#endif
	protected:
		virtual void CriticalSection(void) = 0;
	public:
		VDCriticalSection(void);

		void EnterSection(void);
		void LeaveSection(void);

		void ScheduleSection(void);
	};



#if !NT_KERNEL && !VXD_VERSION

extern VDMutex	VDWin16Mutex;

#endif



// This class provides multitasking- and multiprocessing-safe variables.

class InterlockedLong
	{
	private:
		LONG value;


	public:
		InterlockedLong (void)
			{
			value = 0;
			}

		InterlockedLong (LONG value)
			{
			this->value = value;
			}

		~InterlockedLong (void)
			{
			}

		void Increment (void)
			{
#if NT_KERNEL
			InterlockedIncrement (&value);
#else
			InterlockedIncrement (&value);
#endif
			}

		void Decrement (void)
			{
#if NT_KERNEL
			InterlockedDecrement (&value);
#else
			InterlockedDecrement (&value);
#endif
			}

		void SetValue (LONG newValue) {value = newValue;}

		LONG GetValue (void)
			{
			return value;
			}
	};



class VDIRQSafeSpinLock
	{
	private:
#if NT_KERNEL
		KIRQL			oldIRQLevel;
		LONG			lockVar;
#elif !VXD_VERSION
		int			count;
		BOOL			irqDisabled;
#endif
		BOOL			inIRQ;
	protected:
#if !NT_KERNEL && !VXD_VERSION
		void EnableLock(void);
		BOOL DisableLock(void);
#endif
	public:
		VDIRQSafeSpinLock(void);

		void EnterLock(void);
		void LeaveLock(void);

		void SetInIRQ(BOOL inIRQ) {this->inIRQ = inIRQ;}
		BOOL InIRQ(void) {return inIRQ;}
	};



inline VDIRQSafeSpinLock::VDIRQSafeSpinLock(void)
	{
#if NT_KERNEL
	lockVar = 0;
#elif !VXD_VERSION
	count = 0;
#endif
	}

inline void VDIRQSafeSpinLock::EnterLock()
	{
#if NT_KERNEL
	KIRQL oldIRQLLocal;

	// Multi-processor safe lock that can be executed at > DISPATCH_LEVEL

	KeRaiseIrql(HIGH_LEVEL, &oldIRQLLocal);				// prevents us from being interrupted by any IRQ
	do {;} while (InterlockedExchange(&lockVar, 1));

	oldIRQLevel = oldIRQLLocal;

#elif !VXD_VERSION
	if (!inIRQ)
		{
		count++;

		if (count == 1)
			irqDisabled = DisableLock();
		}
#endif
	}

inline void VDIRQSafeSpinLock::LeaveLock()
	{
#if NT_KERNEL
	KIRQL	oldIRQLLocal;

	oldIRQLLocal = oldIRQLevel;
	InterlockedExchange(&lockVar, 0);
	KeLowerIrql(oldIRQLLocal);
#elif !VXD_VERSION
	if (!inIRQ)
		{
		count--;

		if (count == 0)
			{
			if (irqDisabled)
				EnableLock();
			}
		}
#endif
	}




#if NT_KERNEL || VXD_VERSION || DRVDLL

class VDSpinLock
	{
	private:
#if NT_KERNEL
		KSPIN_LOCK	spinLock;
		KIRQL			oldIRQLevel;
#elif !VXD_VERSION
		int			count;
		BOOL			irqDisabled;
#endif
		BOOL			inIRQ;
	protected:
#if !NT_KERNEL && !VXD_VERSION
		void EnableLock(void);
		BOOL DisableLock(void);
#endif
	public:
		VDSpinLock(void);

		void EnterLock(void);
		void LeaveLock(void);

		void SetInIRQ(BOOL inIRQ) {this->inIRQ = inIRQ;}
		BOOL InIRQ(void) {return inIRQ;}
	};


inline VDSpinLock::VDSpinLock(void)
	{
#if NT_KERNEL
	KeInitializeSpinLock(&spinLock);
#elif !VXD_VERSION
	count = 0;
#endif
	}

inline void VDSpinLock::EnterLock()
	{
#if NT_KERNEL
	KeAcquireSpinLock(&spinLock, &oldIRQLevel);
#elif !VXD_VERSION
	if (!inIRQ)
		{
		count++;

		if (count == 1)
			irqDisabled = DisableLock();
		}
#endif
	}

inline void VDSpinLock::LeaveLock()
	{
#if NT_KERNEL
	KeReleaseSpinLock(&spinLock, oldIRQLevel);
#elif !VXD_VERSION
	if (!inIRQ)
		{
		count--;

		if (count == 0)
			{
			if (irqDisabled)
				EnableLock();
			}
		}
#endif
	}

#if !NT_KERNEL && !VXD_VERSION

inline BOOL VDSpinLock::DisableLock(void)
	{
	if (inIRQ)
		return FALSE;
	else
		{
		BOOL	result;
		_asm {pushf
			   pop   ax
			   and   ax, 0x0200
			   jz   	notSet
				mov	ax, TRUE
				cli
		notSet:
				mov	[result], ax
				}
		return result;
		}
	}

inline void VDSpinLock::EnableLock(void)
	{
	_asm sti;
	}

#endif

#endif

#endif
