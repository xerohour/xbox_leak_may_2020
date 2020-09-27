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
// FILE:      library\common\hooks.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   15.05.95
//
// PURPOSE:   Generalized callback
//
// HISTORY:
//
//

#ifndef HOOKS_H
#define HOOKS_H

#include "prelude.h"
#include "gnerrors.h"

//
// A hook is a generalized callback.  The receiver of the callback
// gets two parameters, first the hook itself (this may be used for
// caller specific data), and a hook specific data element.
//
// The callback function is _NOT_ a virtual function, but a function
// pointer.  This has been done, to avoid incompatibilities with
// traditional 'C'.
//
// A typical use for a hook would be a refill request for a buffer
// in a buffer based stream:
//
// typedef struct RefillMsgStruct
//    {void __far * data, long num, long actual} RefillMsg;
//
// MKHOOK(Refill, RefillMsg);
//
// The implementor of the hook may then add some additional data,
// to customize the hook:
//
// class FileRefillHook : public RefillHook {
//		private:
//			Error Refill(void __far * data, long num, long &actual);
//			friend Error Refill(RefillHook * me, RefillMsg data);
//			FILE 	*	file;
//    public:
//       FileRefillHook(FILE * file) : RefillHook(Refill);
//		};
//
//	Error Refill(RefillHook * me, RefillMsg data)
//		{
//		return (FileRefillHook *)me->Refill(data.data, data.num, data.actual);
//		};
//
//
// Note that you have to do a MakeProcInstance if your callback function is not
// inside a DLL.
//


class KWorkerQueue;

class GenericHook
	{
	protected:
		KWorkerQueue	*	queue;
	public:
		void Register(KWorkerQueue * queue) {this->queue = queue;}
	};


#if NT_KERNEL

//#ifndef ONLY_EXTERNAL_VISIBLE

#include "krnlsync.h"

#define WORKER_QUEUE_SIZE	4096

#if WDM_VERSION
#define WORKER_HEADER_SIZE ((sizeof(void *) + sizeof(DWORD)) / sizeof(DWORD))
#endif

class KWorkerQueue
	{
	private:
		DWORD		buffer[WORKER_QUEUE_SIZE];
		int		bstart, bend, used;
		KEVENT	event, done;
		HANDLE	workerThread;
		IRQMutex	mutex;

		void PutDW(DWORD val);
		DWORD GetDW(void);

#if WDM_VERSION
		void PutDWP(DWORD_PTR);
		DWORD_PTR GetDWP(void);
#endif

	public:
		KWorkerQueue(void);
		~KWorkerQueue(void);
		void EnterWork(GenericHook * me, void * mem, int size);
		void GetWork(GenericHook * & me, void * mem, int & size);
		void RemoveHook(GenericHook * hook);
	};

#define MKHOOK(name, type) \
	class name##Hook : public GenericHook { \
		friend class name##HookHandle;	\
		public: \
			typedef Error (__far * CallType)(name##Hook * me, type & data); \
		private: \
			CallType	call; \
		public:	\
			name##Hook(CallType call) {this->call = call; queue = NULL;} \
	};	\
	class name##HookHandle {	\
		private:	\
			KWorkerQueue	*	queue;	\
			name##Hook	*	hook;		\
		public: \
			name##HookHandle(name##Hook * hook) {this->queue = hook->queue; this->hook = hook;}	\
			name##HookHandle(void) {hook = NULL; queue = NULL;}	\
			Error Call(type data) {if (!queue && hook) hook->call(hook, data); else queue->EnterWork(hook, &data, sizeof(type));GNRAISE_OK;} \
			operator name##Hook * (void) {return hook;}	\
			BOOL operator!= (name##Hook * hook) {return this->hook != hook;} \
			name##HookHandle & operator= (name##Hook * hook) \
				{	\
				if (hook)	\
					this->queue = hook->queue;	\
				else	\
					this->queue = NULL;	\
				this->hook = hook; return *this;	\
				} \
			name##HookHandle & operator= (int i) {this->queue = NULL; this->hook = NULL; return *this;}	\
	};


//#endif // of ONLY_EXTERNAL_VISIBLE

#elif VXD_VERSION

#ifndef ONLY_EXTERNAL_VISIBLE

#define KEVENT HANDLE

#define WORKER_QUEUE_SIZE	4096

class KWorkerQueue
	{
	private:
		DWORD		eventVxdHandle, doneVxdHandle;
		DWORD		buffer[WORKER_QUEUE_SIZE];
		volatile int bstart, bend, used;
//		KEVENT	event, done;
		HANDLE	workerThread;
		void PutDW(DWORD val);
		DWORD GetDW(void);
	public:
		KWorkerQueue(DWORD eventVxdHandle, DWORD doneVxdHandle);
		~KWorkerQueue(void);
		void EnterWork(GenericHook * me, void * mem, int size);
		void GetWork(GenericHook * & me, void * mem, int & size);
	};

#define MKHOOK(name, type) \
	class name##Hook : public GenericHook { \
		friend class name##HookHandle;	\
		public: \
			typedef Error (__far * CallType)(name##Hook * me, type & data); \
		private: \
			CallType	call; \
	};	\
	class name##HookHandle {	\
		private:	\
			KWorkerQueue	*	queue;	\
			name##Hook	*	hook;		\
		public: \
			name##HookHandle(name##Hook * hook) {this->queue = hook->queue; this->hook = hook;}	\
			name##HookHandle(void) {hook = NULL; queue = NULL;}	\
			Error Call(type data) {queue->EnterWork(hook, &data, sizeof(type));GNRAISE_OK;} \
			operator name##Hook * (void) {return hook;}	\
			name##HookHandle & operator= (name##Hook * hook) \
				{	\
				if (hook)	\
					this->queue = hook->queue;	\
				else	\
					this->queue = NULL;	\
				this->hook = hook; return *this;	\
				} \
			name##HookHandle & operator= (int i) {this->queue = NULL; this->hook = NULL; return *this;}	\
	};

#endif	// of ONLY_EXTERNAL_VISIBLE

#elif DRVDLL

#define MKHOOK(name, type) \
	class __far name##Hook : public GenericHook { \
		public: \
			typedef Error (WINAPI * CallType)(name##Hook __far * me, type & data); \
		private: \
			CallType	call; \
			class KWorkerQueue	*	queue;	\
		public: \
			name##Hook(CallType call) {this->call = call;} \
			Error Call(type &data) {return call(this, data);} \
	};

typedef DWORD DWBLK[8];

MKHOOK(Dummy, DWBLK);

#else

extern "C" {
DLLCALL void  WINAPI VDR_RegisterCallback(GenericHook * hook);
}

#define MKHOOK(name, type) \
	class __far name##Hook : public GenericHook { \
		public: \
			typedef Error (WINAPI * CallType)(name##Hook __far * me, type & data); \
		private: \
			CallType	call; \
			class KWorkerQueue	*	queue;	\
		public: \
			name##Hook(CallType call) {this->call = call;VDR_RegisterCallback(this);} \
	};

#endif // NTKERNEL




#endif
