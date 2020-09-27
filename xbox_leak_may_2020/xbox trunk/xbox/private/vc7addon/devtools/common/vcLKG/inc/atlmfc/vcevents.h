//-------------------------------------------------------------------------
//	VcEvents.h - classes for implementing vc7 unified events
//-------------------------------------------------------------------------
//	todo:
//		- [8] BuildVTable: do exact signature checking (structs, etc)
//		- [2] BuildVTable: support non-4 byte param sizes (structs, etc)
//		- [3] SINK_MAP_ENTRY_MFCAXC
//		- [16] native/managed interop
//-------------------------------------------------------------------------
//  cleanup/optimizations:
//		- [2] check into SLM
//		- [5] get rid of vcevents.lib 
//		- [4] turn temp. "throw <string>" calls into real exception classes
//		- [4] minimize reuse of IDispImpl code in CClassicDelegate
//		- [8] optimize typelib access for CClassicDelegate operation
//		- [2] check for: loss of resources/locks thru exceptions
//		- [4] verify that all DTR's cleanup (& punk->Release) all entries
//		- [2] make sure all sources support LIFO (maintain order on remove)
//		- [2] final pass: assert's, const's, code walkthru
//		- [2] final pass: check all locking logic (watch for exceptions)
//-------------------------------------------------------------------------
//  recently completed:
//		- [3] split this file in 2: vcevents.h and vcevent2.h 
//		- [4] remove ATL dependencies from vcevents.h
//		- [3] group all asm into pseudo intrinsics: ComDualThunk
//		- [3] group all asm into pseudo intrinsics: BuildVTable
//		- [6] __unhookall() support
//		- [3] convert "managed" to use event-specific AH/RH routines
//		- [3] make all AH, RH routines "void" (throw exception if failed)
//		- [4] change spec & code of hook/unhook to throw exceptions...(void)
//-------------------------------------------------------------------------
// This is a part of the Active Template Library.
// Copyright (C) 1996-1999 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.
//-------------------------------------------------------------------------
#pragma once
//-------------------------------------------------------------------------
//	compiler-injected helper classes
//
//		CNativeEventSource			- support for sourcing (raising) native C++ events
//		CClassicDispatchEventSource	- support for sourcing classic COM (IConnectionPoint) events 
//		CClassicVtableEventSource	- support for sourcing classic COM (IConnectionPoint) events 
//		CManagedEventSource			- support for sourcing managed (COM+) events
//
//		CEventReceiverUNative	- support for consuming native events in native/classic class
//		CEventReceiverUClassic	- support for consuming classic events in native/classic class
//		CEventReceiverUManaged	- support for consuming managed events in native/classic class
//
//		CEventReceiverMNative	- support for consuming native events in managed class
//		CEventReceiverMManaged	- support for consuming managed events in managed class
//-------------------------------------------------------------------------
//	Note: when the compiler encounters the [event_source} attribute or the 
//		first "event" reference in a class, it adds 1 of the 3 above event 
//		sourcing classes as a baseclass or a class member (assuming it doesn't 
//		already exist in the class.
//-------------------------------------------------------------------------
//	Note: when the compiler encounters the [event_reciever] attribute on 
//		a class, it adds 1 or more of the 3 "CEventReceiverUXXX" classes above
//		(or for managed classes, 1 or 2 of the "CEventReceiverMXXX" classes above).
//-------------------------------------------------------------------------
#ifndef MANAGED_CODE		// compiler cannot support mixed mode yet...
//-------------------------------------------------------------------------
#ifndef __ATLBASE_H__		// add minimal ATL support
#include <atlbase.h>
#endif
//-------------------------------------------------------------------------
//---- PSEUDO-INTRINSIC - to be replaced by compiler/backend intrinsics ----
//---------------------------------------------------------------------------
void __DynamicMethodCall(void *thisptr, void *methaddr, BOOL stdcall, int *paramptr, int stackcnt)
{
	//-----------------------------------------------------------------------
	// this code executes when we are ready to call a class method.  We only
	// support "default" and __stdcall cc's.  Note that we support both static and 
	// non-static methods (default cc: __cdecl and "thiscall").  "paramptr" is
	// the ptr to the actual params (lower on our stack) and "stackcnt" is the
	// # of stack entries the actual params occupy.
	//-----------------------------------------------------------------------

	int paramlen = stackcnt*4;

	//---- copy arguments to tos ----
	for (int p=stackcnt-1; p >= 0; p--)
	{
		int val = paramptr[p];
		_asm push val;
	}

	//---- "this" ptr ----
	if (thisptr)
	{
		if (stdcall)
		{
			_asm mov eax, thisptr;
			_asm push eax;
		}
		else		// "thiscall"
			_asm mov ecx, thisptr;
	}

	//---- call handler now ----
	_asm call methaddr;

	//---- fixup stack for __cdecl calls ----
	if ((! thisptr) && (! stdcall))
 		_asm add esp, [paramlen]
}
//---------------------------------------------------------------------------
//---- PSEUDO-INTRINSIC - to be replaced by compiler/backend intrinsics ----
//---------------------------------------------------------------------------
int __EmitCodeForCallAdjust(void *thisptr, void *methaddr, BOOL stdcall, BOOL indirect,
	int buffsize, UCHAR *codebuff)
{
	//-----------------------------------------------------------------------
	// this code executes when: tos=<return addr> and tos-1=<invalid "thisptr">.
	// goal is to adjust stack and ecx for stdcall, "thiscall", or 
	// __cdecl call being done and then jump to "methaddr".
	//-----------------------------------------------------------------------
	// if buffer is large enough, returns length of bytes emitted; otherwise,
	// returns -x (where "x" is the size of buffer required
	//-----------------------------------------------------------------------

	if (buffsize < 17)				// max code we ever emit
		return -17;

	union
	{
		UCHAR *byteEmit;
		USHORT *shortEmit;
		ULONG *longEmit;
	} u;						// for ease of code emiting

	u.byteEmit = (UCHAR *)codebuff;

	if ((thisptr) && (stdcall))		// __stdcall and non-static method
	{
		*u.longEmit++ = 0x042444c7;			// mov [sp+4], thisptr
		*u.longEmit++ = (ULONG)thisptr;		// con't
	}
	else							// "thiscall" or static method
	{
		*u.byteEmit++ = 0x58;				// pop eax
		*u.byteEmit++ = 0x59;				// pop ecx
		*u.byteEmit++ = 0x50;				// push eax
		*u.byteEmit++ = 0xb9;				// mov ecx, this
		*u.longEmit++= (ULONG)thisptr;		// con't
	}

	*u.byteEmit++ = 0xb8;					// mov eax, methaddr
	*u.longEmit++ = (ULONG)methaddr;		// con't

	if (indirect)
		*u.shortEmit++ = 0x008b;			// mov eax, [eax]

	*u.shortEmit++ = 0xe0ff;				// jmp eax

	return (u.byteEmit - (UCHAR *)codebuff);		// number of bytes generated
}
//---------------------------------------------------------------------------
//---- PSEUDO-INTRINSIC - to be replaced by compiler/backend intrinsics ----
//---------------------------------------------------------------------------
int __EmitCodeForReturnAdjust(int stacklen, int buffsize, UCHAR *codebuff)
{
	//-----------------------------------------------------------------------
	// this code executes as the end of STDCALL method.  The stack needs to 
	// be adjusted and a return needs to be preformed.
	//-----------------------------------------------------------------------
	// if buffer is large enough, returns length of bytes emitted; otherwise,
	// returns -x (where "x" is the size of buffer required
	//-----------------------------------------------------------------------

	if (buffsize < 3)
		return -3;

	union
	{
		UCHAR *byteEmit;
		USHORT *shortEmit;
		ULONG *longEmit;
	} u;						// for ease of code emiting

	u.byteEmit = (UCHAR *)codebuff;

	*u.byteEmit++ = 0xc2;		// ret <stack len> 
	*u.shortEmit++ = stacklen;	// con't		

	return (u.byteEmit - (UCHAR *)codebuff);		// number of bytes generated
}
//---------------------------------------------------------------------------
//---- PSEUDO-INTRINSIC - to be replaced by compiler/backend intrinsics ----
//---------------------------------------------------------------------------
#define __MethAddr(thisptr, method)	\
	GetMethodAddr<MethAddrClass> ((void (MethAddrClass::*)())method)

template <class T>
inline void *GetMethodAddr(void (T::*method)())
{
	union
	{
		void *methaddr;
		void (T::*mfp)();
	} u;
	u.mfp = (void (T::*)()) method;
	return u.methaddr;
}
//-------------------------------------------------------------------------
class Delegate;		// temp: for unmanaged compiles
class String;		// temp: for unmanaged compiles
class Object;		// temp: for unmanaged compiles

#define STRUCT struct
//-------------------------------------------------------------------------
#else		// managed compile
//-------------------------------------------------------------------------
#include <atlmin.h>			// this will go away when compiler supports full mixing
[managed] class BugWorkaroundForStringType
{
};

#define STRUCT [value_type] struct
//-------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------
// Note: program/framework should have 1 call to "HookEvents()" and
//		 1 call to __unhookall().
//-------------------------------------------------------------------------
#define BEGIN_HOOK_MAP(thisclass)		\
	virtual void HookEvents()   \
	{					

#define BEGIN_HOOK_MAP_BASE1(thisclass, base1)		\
	virtual void HookEvents()   \
	{							\
		base1::HookEvents();					

#define BEGIN_HOOK_MAP_BASE2(thisclass, base1, base2)		\
	virtual void HookEvents()   \
	{							\
		base1::HookEvents();	\
		base2::HookEvents();					

#define HOOK_ENTRY(srcobj, eventid, handler)	\
	__hook(srcobj, eventid, handler);

#define HOOK_ENTRY_ATLAXC(controlid, eventid, handler, objguid)	\
	{												\
		HWND h = pT->GetDlgItem(controlid);			\
		ATLASSERT(h != NULL);						\
		if (h != NULL)								\
		{											\
			CComPtr<IUnknown> spUnk;				\
			AtlAxGetControl(h, &spUnk);				\
			ATLASSERT(spUnk != NULL);				\
			if (spUnk != NULL)						\
				__hook(spUnk, eventid, handler);	\
		}											\
	}

#define END_HOOK_MAP()	\
	}
//-------------------------------------------------------------------------
struct UnmanagedDelegate	
{
	IUnknown *Punk;		// NULL if pointing to a native class method
	void *ThisPtr;		
	void *CodeAddr;

	bool operator == (const UnmanagedDelegate &rhs) const
	{
		return ((rhs.Punk == Punk) && (rhs.ThisPtr == ThisPtr) && (rhs.CodeAddr == CodeAddr));
	}
};
//-------------------------------------------------------------------------
#ifndef MANAGED_CODE
//-------------------------------------------------------------------------
class INativeEventSource		// not IUnknown based
{
public:
	virtual void AddHandler(UnmanagedDelegate dlg, int eventid) = 0;
	virtual void RemoveHandler(UnmanagedDelegate dlg, int eventid) = 0;
	virtual void AddHandler(Delegate *dlg, int eventid) = 0;
	virtual void RemoveHandler(Delegate *dlg, int eventid) = 0;
};
//-------------------------------------------------------------------------
struct SrcNatUnmEntry		// unmanaged delegate tracked in native source object
{
	UnmanagedDelegate Dlg;
	int EventId;

	bool operator == (const SrcNatUnmEntry &rhs) const
	{
		return ((rhs.Dlg == Dlg) && (rhs.EventId == EventId));
	}
};
//-------------------------------------------------------------------------
struct SrcNatManEntry		// managed delegate tracked in native source object
{
	Delegate *Dlg;
	int EventId;			// managed caller must pass native event id

	bool operator == (const SrcNatManEntry &rhs) const
	{
		return ((rhs.Dlg == Dlg) && (rhs.EventId == EventId));
	}
};
//-------------------------------------------------------------------------
struct RcvUnmNatEntry		// native events hooked by Unmanaged receiver object
{
	RcvUnmNatEntry()
	{
	}

	RcvUnmNatEntry(INativeEventSource *src, UnmanagedDelegate dlg, int eventid)
	{
		Src = src;
		Dlg = dlg;
		EventId = eventid;
	}

	bool operator == (const RcvUnmNatEntry &rhs) const
	{
		return ((rhs.Src == Src) && (rhs.Dlg == Dlg) && (rhs.EventId == EventId));
	}

	INativeEventSource *Src;		// source object
	UnmanagedDelegate Dlg;
	int EventId;
};
//-------------------------------------------------------------------------
struct RcvUnmManEntry		// managed events hooked by an Unmanaged receiver object
{
	RcvUnmManEntry(Object *src, Delegate *dlg, String *eventid)
	{
		Src = src;
		Dlg = dlg;
		EventId = eventid;
	}

	Object *Src;		// source object
	Delegate *Dlg;
	String *EventId;
};
//-------------------------------------------------------------------------
template <class TCriticalSection>
class CNativeEventSource : public TCriticalSection
{
protected:
	CSimpleArray<SrcNatUnmEntry> UnmanagedEntries;
	CSimpleArray<SrcNatManEntry> ManagedEntries;

public:
	void AddHandler(const UnmanagedDelegate dlg, int eventid)
	{
		ATLASSERT(dlg.CodeAddr != NULL);
		ATLASSERT(eventid >= 0);

		if (dlg.Punk)
			dlg.Punk->AddRef();

		SrcNatUnmEntry entry = {dlg.Punk, dlg.ThisPtr, dlg.CodeAddr, eventid};
		
		Lock();
		BOOL added = UnmanagedEntries.Add(entry);
		Unlock();

		if (! added)
			throw "Cannot add more entries";		// todo: convert to a real exception
	}

	void RemoveHandler(const UnmanagedDelegate dlg, int eventid)
	{
		SrcNatUnmEntry entry = {dlg.Punk, dlg.ThisPtr, dlg.CodeAddr, eventid};

		Lock();
		BOOL removed = UnmanagedEntries.Remove(entry);
		Unlock();

		if (! removed)
			throw "Remove Error - Event not in source delegate list";		// todo: convert to a real exception

		if (dlg.Punk)
			dlg.Punk->Release();
	}

	void AddHandler(Delegate *dlg, int eventid)
	{
		ATLASSERT(dlg != NULL);

		SrcNatManEntry entry = {dlg, eventid};

		Lock();
		BOOL added = ManagedEntries.Add(entry);
		Unlock();

		if (! added)
			throw "Cannot add more entries";		// todo: convert to a real exception
	}

	void RemoveHandler(Delegate *dlg, int eventid)
	{
		SrcNatManEntry entry = {dlg, eventid};

		Lock();
		BOOL removed = ManagedEntries.Remove(entry);
		Unlock();

		if (! removed)
			throw "Remove Error - Event not in source delegate list";		// todo: convert to a real exception
	}

	void Raise(int eventid, BOOL stdcall, int paramLen, int stackcnt, ...)
	{
		ATLASSERT(stackcnt >= 0);

		//---- call the unmanaged delegates ----
		int i = 0;
		SrcNatUnmEntry *entry = NULL;
		void *thisval;
		void *codeaddr;
		IUnknown *punk;

		while (1)		// for each unmanaged entry
		{
			entry = NULL;

			//---- protect access of next element ----
			Lock();
			while (i < UnmanagedEntries.GetSize()) 
			{
				if (UnmanagedEntries[i].EventId == eventid)
				{
					entry = &UnmanagedEntries[i];
					thisval = entry->Dlg.ThisPtr;
					codeaddr = entry->Dlg.CodeAddr;
					punk = entry->Dlg.Punk;

					if (punk)
						punk->AddRef();			// don't let it go away while we are raising event
					break;
				}
				i++;
			}
			Unlock();

			if (! entry)
				break;
			
			try
			{
				//---- call next handler ----
				int *base;
				base = &stackcnt + 1; 

				__DynamicMethodCall(thisval, codeaddr, stdcall, base, stackcnt);
			}
			catch (...)
			{
			}

			if (punk)
				punk->Release();			// we still hold a refcnt from our list

			i++;		// get next item
		}

#if 0			// for now, these should be done inline by code that does "raise"
		entry = NULL;
		i = 0;
		SrcNatManEntry *mentry;

		while (1)		// for each managed entry
		{
			mentry = NULL;

			//---- protect access of next element ----
			Lock();
			if (i < ManagedEntries.GetSize())
			{
				if (ManagedEntries[i].EventId == eventid)
					mentry = &ManagedEntries[i];
			}
			Unlock();

			if (! mentry)
				break;

			try
			{
				//---- call managed code via delegate's invoke ----
			}
			catch (...)
			{
			}
		}
#endif

	}
};
//-------------------------------------------------------------------------
template <class TCriticalSection>
class CEventReceiverUNative : public CSimpleArray<RcvUnmNatEntry>, public TCriticalSection
{
public:
	void Hook(INativeEventSource *src, IUnknown *punk, 
		void *thisptr, void *methaddr, int eventid)
	{
		UnmanagedDelegate dlg = {punk, thisptr, methaddr};
		src->AddHandler(dlg, eventid);

		RcvUnmNatEntry entry(src, dlg, eventid);

		Lock();
		BOOL added = Add(entry);
		Unlock();

		if (! added)
			throw "Cannot add more entries";		// todo: convert to a real exception
	}

	void Unhook(INativeEventSource *src, IUnknown *punk, 
		void *thisptr, void *methaddr, int eventid)
	{
		UnmanagedDelegate dlg = {punk, thisptr, methaddr};
		src->RemoveHandler(dlg, eventid);

		RcvUnmNatEntry entry(src, dlg, eventid);

		Lock();
		BOOL removed = Remove(entry);
		Unlock();

		if (! removed)
			throw "Event not in receiver event list";		// todo: convert to a real exception
	}

	void UnhookAll()
	{
		BOOL gotone;
		RcvUnmNatEntry entry;

		while (1)
		{
			//---- protect access of next element ----
			gotone = FALSE;
			BOOL removed = FALSE;

			Lock();
			int size = GetSize();
			if (size)
			{
				gotone = TRUE;
				entry = m_aT[size-1];
				removed = RemoveAt(size-1);
			}
			Unlock();
			
			if (! gotone)
				break;
			
			if (! removed)
				throw "Cannot remove entry";		// todo: convert to a real exception

			entry.Src->RemoveHandler(entry.Dlg, entry.EventId);
		}
	}
};
//-------------------------------------------------------------------------
template <class TCriticalSection>
class CEventReceiverUManaged : public CSimpleArray<RcvUnmManEntry>, public TCriticalSection
{
public:
	void Hook(Object *src, void *thisptr, String *eventid, Delegate *dlg)
	{
		src->AddHandler(dlg, eventid);

		RcvUnmManEntry entry(src, dlg, eventid);

		Lock();
		BOOL added = Add(entry);
		Unlock();

		if (! added)
			throw "Cannot add more entries";		// todo: convert to a real exception

		return retval;
	}

	void Unhook(Object *src, void *thisptr, String *eventid, String *removehandler)
	{
		BOOL found = FALSE;
		RcvUnmManEntry entry;

		// todo: set "entry" to matching list entry

		if (found)
		{
			Lock();
			BOOL removed = Remove(entry);
			Unlock();
			
			if (! removed)
				throw "Cannot remove entry";		// todo: convert to a real exception

			src->RemoveHandler(entry.Dlg, eventid);
		}
	}

	void UnhookAll()
	{
		BOOL gotone;
		RcvUnmManEntry entry;

		while (1)
		{
			//---- protect access of next element ----
			gotone = FALSE;
			BOOL removed = FALSE;

			Lock();
			int size = GetSize();
			if (size)
			{
				gotone = TRUE;
				entry = m_aT[size-1];
				removed = RemoveAt(size-1);
			}
			Unlock();
			
			if (! gotone)
				break;
		
			if (! removed)
				throw "Cannot remove entry";		// todo: convert to a real exception
			
			src->RemoveHandler(dlg, eventid);
		}
	}
};
//-------------------------------------------------------------------------
#endif
#ifdef MANAGED_CODE
//-------------------------------------------------------------------------
int StringToWide(String *str, wchar_t *wbuff, int wlen)
{
	ATLASSERT(str != NULL);
	ATLASSERT(wbuff != NULL);

	int len = str->GetLength();
	
	if (len >= wlen)
		return -1;

	for (int i=0; i < len; i++)
		wbuff[i] = str->GetChar(i);

	wbuff[len] = 0;
	return len;
}
//-------------------------------------------------------------------------
STRUCT VManagedSrcEntry
{
	Delegate *Dlg;
	int EventId;
};
//-------------------------------------------------------------------------
STRUCT RcvManNatEntry		// native events hooked by a Managed receiver object
{
	void *Src;		// source object
	UnmanagedDelegate Dlg;
	int EventId;
};
//-------------------------------------------------------------------------
STRUCT RcvManManEntry		// managed events hooked by a Managed receiver object
{
	Object *Src;
	Delegate *Dlg;
	String *EventName;
	String *RemoveHandler;
};
//-------------------------------------------------------------------------
[managed] class CManagedEventSource
{
public:
	CManagedEventSource(bool multithread)
	{
		bool bLocking = multithread;

		//if (bLocking)
			//InitializeCriticalSection(&m_sec);
	}

	~CManagedEventSource()
	{
		//if (bLocking)
			//DeleteCriticalSection(&m_sec);
	}

	void AddHandler(Delegate *dlg, int eventid)
	{
		ATLASSERT(dlg != NULL);
		ATLASSERT(eventid > 0);

		Lock();

		if (! Entries)
			Entries = new [managed] VManagedSrcEntry[1];

		int size = Entries->GetLength();
		if (Count >= size)					// grow array
		{
			//printf("growing array from %d to %d\n", size, 2*size);
			[managed] VManagedSrcEntry temp[];
			temp = new [managed] VManagedSrcEntry[2*size];
			Array::Copy(Entries, 0, temp, 0, size);
			Entries = temp;
		}

		//printf("AddHandler: dlg=0x%08x, event=%d\n", dlg, eventid);

		Entries[Count].Dlg = dlg;
		Entries[Count].EventId = eventid;
		Count++;

		Unlock();
	}

	void RemoveHandler(Delegate *dg, int eventid)
	{
		ATLASSERT(dg != NULL);
		ATLASSERT(eventid > 0);
		BOOL removed = FALSE;

		Lock();

		for (int i=0; i < Count; i++)
		{
			if ((Entries[i].Dlg->Equals(dg)) && (Entries[i].EventId == eventid))
			{
				//---- delete entry at "i", preserving their order ----
				Count--;
				for (int j=i; j < Count; j++)
					Entries[j] = Entries[j+1];
				printf("source: removed managed delegate\n");
				removed = TRUE;
				break;
			}
		}

		Unlock();

#if 0		// managed cannot yet support
		if (! removed)
			throw "Remove Error - Event not in source delegate list";
#endif
	}

	Delegate *SafeMatchDlg(int eventid, int *pi)
	{
		int i = *pi;
		Delegate *retval = NULL;

		Lock();

		while (i < Count)
		{
			if (Entries[i].EventId == eventid)
			{
				retval = Entries[i].Dlg;
				break;
			}
			i++;
		}

		Unlock();

		*pi = i+1;
		return retval;
	}
	
	void Lock()
	{
		//if (bLocking)
			//EnterCriticalSection(&m_sec);
	}

	void Unlock()
	{
		//if (bLocking)
			//LeaveCriticalSection(&m_sec);
	}

public:			// temp - remove when "raise()" code returns to this helper class
	[managed] VManagedSrcEntry Entries[];
	int Count;

protected:
	CRITICAL_SECTION m_sec;
	bool bLocking;
};
//-------------------------------------------------------------------------
#define BUG_WORKAROUND	\
{	\
	char buff[100];		\
	strcpy(buff, "dummy string");	\
}
//-------------------------------------------------------------------------
[managed] class CEventReceiverMManaged 
{
public:
	[managed] RcvManManEntry Entries[];
	int Count;

	CEventReceiverMManaged(bool multithread)
	{
		bool bLocking = multithread;

		//if (bLocking)
			//InitializeCriticalSection(&m_sec);
	}

	~CEventReceiverMManaged()
	{
		//if (bLocking)
			//DeleteCriticalSection(&m_sec);
	}

	void Add(Object *src, Delegate *dlg, String *eventname, String *removehandler)
	{
		ATLASSERT(src != NULL);
		ATLASSERT(eventname != NULL);
		ATLASSERT(dlg != NULL);

		// src->AddHandler(dlg, eventname);

		if (! Entries)
			Entries = new [managed] RcvManManEntry [1];

		int size = Entries->GetLength();
		if (Count >= size)					// grow array
		{
			//printf("growing array from %d to %d\n", size, 2*size);
			[managed] RcvManManEntry temp[];
			temp = new [managed] RcvManManEntry[2*size];
			Array::Copy(Entries, 0, temp, 0, size);
			Entries = temp;
		}
		
		//---- add new entry ----
		Lock();

		Entries[Count].Src = src;
		Entries[Count].Dlg = dlg;
		Entries[Count].EventName = eventname;
		Entries[Count].RemoveHandler = removehandler;

		BUG_WORKAROUND;

		//printf("ADDED managed delegate\n");
		Count++;

		Unlock();
	}

	void Remove(Object *src, String *eventname, Delegate *tdlg)
	{
		for (int i=0; i < Count; i++)
		{
			//---- removing below line causes COM+ exception ----
			BUG_WORKAROUND;
			if ((Entries[i].Src == src) && 
				(Entries[i].EventName->CompareTo(eventname)==0) &&
				(Entries[i].Dlg->Equals(tdlg)))
			{
				//---- call src->RemoveHandler() using Dynamic Invocation ----
				CallRemoveHandler(Entries[i]);
				
				//---- delete entry at "i" ----
				wchar_t wbuff[100];
				StringToWide(eventname, wbuff, 100);

				//printf("deleting managed delegate: event=%S\n", wbuff);
				//printf("deleting managed delegate\n");
				Count--;

				for (int j=i; j < Count; j++)
					Entries[j] = Entries[j+1];
				printf("sink: removed managed delegate\n");
				return;
			}
		}

#if 0		// managed cannot yet support
		throw "Internal error - event not in receiver hook list";		// todo: convert to a real exception
#endif

	}

	void UnhookAll()
	{
		BOOL gotone;
		RcvManManEntry entry;

		while (1)
		{
			//---- protect access of next element ----
			gotone = FALSE;

			Lock();
			if (Count)
			{
				int i = Count-1;

				gotone = TRUE;
				entry = Entries[i];

				//---- remove entry at "i" ----
				Count--;

				for (int j=i; j < Count; j++)
					Entries[j] = Entries[j+1];
				printf("sink: removed managed delegate\n");
			}
			Unlock();
			
			if (! gotone)
				break;
			
			//---- call src->RemoveHandler() using Dynamic Invocation ----
			CallRemoveHandler(entry);
		}
	}

protected:
	void CallRemoveHandler(RcvManManEntry entry)
	{
		//---- get MethodInfo ----
		Object *src = entry.Src;
		System::Reflection::MethodInfo* m;
		m = src->GetType()->GetMethod(entry.RemoveHandler);

		//---- build params ----
		[managed] Variant v[];
		v = new [managed] Variant[1];   //   Argument list
		v[0] = entry.Dlg;

		//---- call it dynamically ----
		m->Invoke(src, v);
	}

	void Lock()
	{
		//if (bLocking)
			//EnterCriticalSection(&m_sec);
	}

	void Unlock()
	{
		//if (bLocking)
			//LeaveCriticalSection(&m_sec);
	}

protected:
	CRITICAL_SECTION m_sec;
	bool bLocking;
};
//-------------------------------------------------------------------------
#endif
