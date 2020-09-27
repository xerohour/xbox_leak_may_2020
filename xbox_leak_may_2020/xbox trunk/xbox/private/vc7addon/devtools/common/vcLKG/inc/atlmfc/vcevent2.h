//-------------------------------------------------------------------------
//	VcEvent2.h - helper classes for implementing vc7 unified events for 
//				 Classic COM Source and Receiver classes
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
#ifndef __ATLBASE_H__		// add minimal ATL support
#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#endif
//-------------------------------------------------------------------------
extern CComModule _Module;
extern void *__PullInAtlModule = &_Module;		// force a reference to pull in a _Module

#pragma comment(lib, "vcevents.lib")
//---------------------------------------------------------------------------
STRUCT CDispatchEntry
{
	DISPID Dispid;
	void *ThisPtr;
	void *MethAddr;
	BOOL StdCall;

	bool operator == (const CDispatchEntry &rhs) const
	{
		return ((rhs.Dispid == Dispid) && (rhs.ThisPtr == ThisPtr) 
			&& (rhs.MethAddr == MethAddr) && (rhs.StdCall == StdCall));
	}
};
//-------------------------------------------------------------------------
template <class T, const IID *guidref>
class CClassicDispatchEventSource : public IConnectionPointImpl<T, guidref, CComDynamicUnkArray>
{
public:
	void Raise(int dispid, int argcnt, ...)
	{
		//---- variable args are passed as VARIANTARG's by compiler ----
		//---- we only support "void" return types ----

		CComVariant varResult;
		T* pT = static_cast<T*>(this);
		int index;
		int size = m_vec.GetSize();
		char *args = 4 + (char *)&argcnt;
		
		for (index = 0; index < size; index++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(index);
			pT->Unlock();
			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
			if (pDispatch != NULL)
			{
				VariantClear(&varResult);
				DISPPARAMS params = { (VARIANTARG *)args, NULL, argcnt, 0 };

				//---- don't need try/catch here; done within Invoke ----
				pDispatch->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, 
					&params, &varResult, NULL, NULL);
			}
		}
	}
};
//-------------------------------------------------------------------------
template <class T, class InterfaceType>
class CClassicVTableEventSource : public IConnectionPointImpl<T, &__uuidof(InterfaceType), CComDynamicUnkArray>
{
public:
	void Raise(int vtableoffset, int stackcnt, ...)
	{
		ATLASSERT(stackcnt >= 0);

		T* pT = static_cast<T*>(this);
		int index;
		int size = m_vec.GetSize();
		
		for (index = 0; index < size; index++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(index);
			pT->Unlock();
			InterfaceType* pInterface = reinterpret_cast<InterfaceType*>(sp.p);
			if (pInterface != NULL)
			{
				//---- call pInterface ----
				try
				{
					int *base = &stackcnt+1;
					void *codeaddr = (void *)*(vtableoffset + *((long **)(pInterface)));

					//---- from "source" perspective, we only support ----
					//---- "stdcall" on normal (non-static) methods ----
					__DynamicMethodCall(pInterface, codeaddr, TRUE, base, stackcnt);
				}
				catch (...)
				{
				}
			}
		}
	}
};
//-------------------------------------------------------------------------
class CComDualThunk
{
public:
	void* pVtable;
	void* pFunc;
	UCHAR code[128];			// should be enough for all platforms

	void Init(DWORD funcaddr, void* pThis, BOOL stdcall)
	{
		ATLASSERT(funcaddr != NULL);

		pVtable = &pFunc;
		pFunc = code;

		int len = __EmitCodeForCallAdjust(pThis, (void *)funcaddr, stdcall, FALSE, 
			sizeof(code), code);
		ATLASSERT(len > 0);

		FlushInstructionCache(GetCurrentProcess(), this, sizeof(CComDualThunk));
	}
};
//-------------------------------------------------------------------------
template <class TCriticalSection>
class CClassicDelegate :
	public _IDispEventLocator<0, &GUID_NULL>,
	public TCriticalSection
{
public:
	CClassicDelegate(IUnknown *src, ITypeInfo *pTypeInfo, const GUID *guid, void *thisptr)
	{
		ATLASSERT(src != NULL);
		ATLASSERT(pTypeInfo != NULL);
		ATLASSERT(guid != NULL);

		//---- init these in case AtlGetObjectSourceInterface fails ----
		_tih.m_pguid = &GUID_NULL;
		_tih.m_plibid = &GUID_NULL;
		_tih.m_wMajor = 0;
		_tih.m_wMinor = 0;

		//---- set the "libid", etc. info for the dispatch case ----
		AtlGetObjectSourceInterface(src, &m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum);

		m_iid = *guid;			// override, in case we are after the non-default source interface

		_tih.m_pguid = &m_iid;
		_tih.m_plibid = &m_libid;
		_tih.m_wMajor = m_wMajorVerNum;
		_tih.m_wMinor = m_wMinorVerNum
			;
		_tih.m_pInfo = NULL;
		_tih.m_dwRef = 0;
		_tih.m_pMap = NULL;
		_tih.m_nCount = 0;

		Dispatch = FALSE;
		SlotCnt = 0;
		VTable = NULL;

		TYPEATTR *typeattr;
		int funccnt;
		HRESULT hr = pTypeInfo->GetTypeAttr(&typeattr);
		if (SUCCEEDED(hr))
		{
			funccnt = typeattr->cFuncs;

			if (typeattr->wTypeFlags & TYPEFLAG_FDUAL)			// dual
			{
				Dispatch = TRUE;
				SlotCnt = typeattr->cbSizeVft/4;
			}
			else if (typeattr->typekind == TKIND_DISPATCH)		// dispinterface
				Dispatch = TRUE;
			else
				SlotCnt = typeattr->cbSizeVft/4;			// custom


			pTypeInfo->ReleaseTypeAttr(typeattr);
		}

		if (SlotCnt)
		{
			BuildVTable(funccnt, pTypeInfo, thisptr);
			*(void **)this = VTable;		// replace my vtable with manually contructed one
		}
	}

	~CClassicDelegate()
	{
		if (VTable)
			delete [] VTable;
	}

	void BuildVTable(int funccnt, ITypeInfo *pTypeInfo, void *thisptr)
	{
		ATLASSERT(funccnt >= 0);
		ATLASSERT(pTypeInfo != NULL);

		//---- compute length of code we will be emiting (with fake params) ----
		UCHAR dummy[128];
		
		EntryCodeLen = __EmitCodeForCallAdjust(this, (void *)0x2000, TRUE, TRUE, sizeof(dummy), dummy);
		ATLASSERT(EntryCodeLen > 0);

		int exitCodeLen = __EmitCodeForReturnAdjust(8, sizeof(dummy), dummy);
		ATLASSERT(exitCodeLen > 0);

		int codeSlotCount = (EntryCodeLen + exitCodeLen + 3)/4;

		//---- VTable contents ----
		//	VTable[0-N]: act as vtable entries for object (SlotCnt entries)
		//  VTable[N+1-X]: currently hooked methods, if any (custcnt entries)
		//  VTable[X-Z]: entry/exit code (codeSlotCount*custcnt entries)

		int firstslot;
		if (Dispatch)
			firstslot = 7;
		else
			firstslot = 3;

		int custcnt = SlotCnt - firstslot;
		ATLASSERT(funccnt == custcnt);
		
		VTable = new void *[SlotCnt + (codeSlotCount+1)*custcnt];
		void **origvt = *(void ***)this;

		for (int i=0; i < firstslot; i++)
			VTable[i] = origvt[i];		// copy current vtable entries 

		for (int m=0; m < custcnt; m++)		// each custom method in vtable
		{

			FUNCDESC *funcdesc;
			HRESULT hr = pTypeInfo->GetFuncDesc(m, &funcdesc);

			ATLASSERT(SUCCEEDED(hr));
			if (FAILED(hr))
				continue;

			BOOL stdcall = (funcdesc->callconv == CC_STDCALL);
			ATLASSERT(stdcall == TRUE);		// only stdcall supported on source side

			int retlen = (1 + funcdesc->cParams)*4;		// add 1 for "this"; todo: support for structs, etc.
			int slotnum = funcdesc->oVft/4;

			if (slotnum < firstslot)
				continue;			// already setup thru origvt

			UCHAR *codeptr = (UCHAR *)&VTable[SlotCnt + custcnt + (slotnum-firstslot)*5];

			//---- add entry code for this method ----
			int len = __EmitCodeForCallAdjust(thisptr, VTable+slotnum+custcnt, stdcall, TRUE,
				EntryCodeLen, codeptr);
			ATLASSERT(len == EntryCodeLen);

			codeptr += len;
			VTable[slotnum] = codeptr;			// point at exit code (currently disabled)
			VTable[slotnum+custcnt] = NULL;		// currently hooked method

			//---- add exit code for this method ----
			len = __EmitCodeForReturnAdjust(retlen, exitCodeLen, codeptr);
			ATLASSERT(len > 0);

			pTypeInfo->ReleaseFuncDesc(funcdesc);
		}

		FlushInstructionCache(GetCurrentProcess(), &VTable[SlotCnt+custcnt], 20*custcnt);
	}

	BOOL IsEmpty() 
	{
		int start;
		if (Dispatch)
			start = 7;
		else
			start = 3;
		int custcnt = SlotCnt - start;

		BOOL retval = TRUE;

		Lock();

		for (int m=start; m < SlotCnt+1; m++)
		{
			if (VTable[m+custcnt])		// enabled - we are not empty
			{
				retval = FALSE;
				break;
			}
		}

		if (retval)
			retval = (DispatchList.GetSize() == 0);

		Unlock();
		
		return retval;
	}

	BOOL AddHandler(DISPID dispid, void *thisptr, void *methaddr, BOOL stdcall, int vtSlot, 
		int paramlen, LPCSTR signature)
	{
		ATLASSERT(methaddr != NULL);
		ATLASSERT((! VTable) || (vtSlot >= 3));

		BOOL retval = FALSE;

		Lock();

		if (Dispatch)
		{
			CDispatchEntry entry = {dispid, thisptr, methaddr, stdcall};
			retval = DispatchList.Add(entry);
		}

		if (VTable)
		{
			ATLASSERT(stdcall == TRUE);			// must match source cc
			ATLASSERT(thisptr != NULL);			// must be non-static handler

			int custcnt;
			if (Dispatch)
				custcnt = SlotCnt - 7;
			else
				custcnt = SlotCnt - 3;

			if (! VTable[vtSlot+custcnt])		// not currently set
			{
				// todo: verify that "paramlen" matches typelib paramlen
				// todo: verify that "signature" matches typelib signature
				VTable[vtSlot+custcnt] = methaddr;

				//---- adjust vtable to point to entry code ----
				ULONG *vtlong = (ULONG *)VTable;
				vtlong[vtSlot] -= EntryCodeLen;

				retval = TRUE;
			}
		}

		Unlock();

		return retval;
	}

	BOOL RemoveHandler(DISPID dispid, void *thisptr, void *methaddr, BOOL stdcall, int vtSlot)
	{
		ATLASSERT(methaddr != NULL);
		ATLASSERT((! VTable) || (vtSlot >= 3));

		BOOL retval = FALSE;

		Lock();

		if (Dispatch)
		{
			CDispatchEntry entry = {dispid, thisptr, methaddr, stdcall};
			retval = DispatchList.Remove(entry);
		}

		if (VTable)
		{
			int custcnt;
			if (Dispatch)
				custcnt = SlotCnt - 7;
			else
				custcnt = SlotCnt - 3;

			if (VTable[vtSlot+custcnt] == methaddr)		// currently set
			{
				VTable[vtSlot+custcnt] = NULL;

				//---- adjust vtable to point to exit code ----
				ULONG *vtlong = (ULONG *)VTable;
				vtlong[vtSlot] += EntryCodeLen;

				retval = TRUE;
			}
		}

		Unlock();

		return retval;
	}

	//---- below functions are same as in IDispEventImpl ----

	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
	{*pctinfo = 1; return S_OK;}

	STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
	{return _tih.GetTypeInfo(itinfo, lcid, pptinfo);}

	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
		LCID lcid, DISPID* rgdispid)
	{return _tih.GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);}

	HRESULT GetFuncInfoFromId(const IID& /*iid*/, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info)
	{
		CComPtr<ITypeInfo> spTypeInfo;
		if (InlineIsEqualGUID(*_tih.m_plibid, GUID_NULL))
		{
			_tih.m_plibid = &m_libid;
			_tih.m_pguid = &m_iid;
			_tih.m_wMajor = m_wMajorVerNum;
			_tih.m_wMinor = m_wMinorVerNum;
		}
		HRESULT hr = _tih.GetTI(lcid, &spTypeInfo);
		if (FAILED(hr))
			return hr;
		CComQIPtr<ITypeInfo2, &IID_ITypeInfo2> spTypeInfo2 = spTypeInfo;
		FUNCDESC* pFuncDesc = NULL;
		if (spTypeInfo2 != NULL)
		{
			UINT nIndex;
			hr = spTypeInfo2->GetFuncIndexOfMemId(dispidMember, INVOKE_FUNC, &nIndex);
			if (FAILED(hr))
				return hr;
			hr = spTypeInfo->GetFuncDesc(nIndex, &pFuncDesc);
			if (FAILED(hr))
				return hr;
		}
		else // search for funcdesc
		{
			TYPEATTR* pAttr;
			hr = spTypeInfo->GetTypeAttr(&pAttr);
			if (FAILED(hr))
				return hr;
			for (int i=0;i<pAttr->cFuncs;i++)
			{
				hr = spTypeInfo->GetFuncDesc(i, &pFuncDesc);
				if (FAILED(hr))
					return hr;
				if (pFuncDesc->memid == dispidMember)
					break;
				spTypeInfo->ReleaseFuncDesc(pFuncDesc);
				pFuncDesc = NULL;
			}
			spTypeInfo->ReleaseTypeAttr(pAttr);
			if (pFuncDesc == NULL)
				return E_FAIL;
		}

		// If this assert occurs, then add a #define _ATL_MAX_VARTYPES nnnn
		// before including atlcom.h
		ATLASSERT(pFuncDesc->cParams <= _ATL_MAX_VARTYPES);
		if (pFuncDesc->cParams > _ATL_MAX_VARTYPES)
			return E_FAIL;

		for (int i=0; i<pFuncDesc->cParams; i++)
		{
			info.pVarTypes[i] = pFuncDesc->lprgelemdescParam[pFuncDesc->cParams - i - 1].tdesc.vt;
			if (info.pVarTypes[i] == VT_PTR)
				info.pVarTypes[i] = pFuncDesc->lprgelemdescParam[pFuncDesc->cParams - i - 1].tdesc.lptdesc->vt | VT_BYREF;
			if (info.pVarTypes[i] == VT_USERDEFINED)
				info.pVarTypes[i] = GetUserDefinedType(spTypeInfo,pFuncDesc->lprgelemdescParam[pFuncDesc->cParams-i-1].tdesc.hreftype);
		}

		VARTYPE vtReturn = pFuncDesc->elemdescFunc.tdesc.vt;
		switch(vtReturn)
		{
		case VT_INT:
			vtReturn = VT_I4;
			break;
		case VT_UINT:
			vtReturn = VT_UI4;
			break;
		case VT_VOID:
			vtReturn = VT_EMPTY; // this is how DispCallFunc() represents void
			break;
		case VT_HRESULT:
			vtReturn = VT_ERROR;
			break;
		}
		info.vtReturn = vtReturn;
		info.cc = pFuncDesc->callconv;
		info.nParams = pFuncDesc->cParams;
		spTypeInfo->ReleaseFuncDesc(pFuncDesc);
		return S_OK;
	}
	
	VARTYPE GetUserDefinedType(ITypeInfo *pTI, HREFTYPE hrt)
	{
		CComPtr<ITypeInfo> spTypeInfo;
		VARTYPE vt = VT_USERDEFINED;
		HRESULT hr = E_FAIL;
		hr = pTI->GetRefTypeInfo(hrt, &spTypeInfo);
		if(FAILED(hr))
			return vt;
		TYPEATTR *pta=NULL;

		spTypeInfo->GetTypeAttr(&pta);
		if(pta && (pta->typekind == TKIND_ALIAS || pta->typekind == TKIND_ENUM))
		{
			if (pta->tdescAlias.vt == VT_USERDEFINED)
				vt = GetUserDefinedType(spTypeInfo,pta->tdescAlias.hreftype);
			else
			{
				switch (pta->typekind)
				{
				case TKIND_ENUM :
					vt = VT_I4;
					break;
				case TKIND_INTERFACE :
					vt = VT_UNKNOWN;
					break;
				case TKIND_DISPATCH :
					vt = VT_DISPATCH;
					break;
				default:
					vt = pta->tdescAlias.vt;
				}
			}				
		}
	
		if(pta)
			spTypeInfo->ReleaseTypeAttr(pta);
		return vt;

	}

	//---- customized functions ----

	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
		LCID lcid, WORD /*wFlags*/, DISPPARAMS* pdispparams, VARIANT* pvarResult,
		EXCEPINFO* /*pexcepinfo*/, UINT* /*puArgErr*/)
	{
		//---- call each matching "dispid" in our table ---
		//---- note: we are already unique by object & guid ----
		int i = 0;
		CDispatchEntry *entry;
		void *thisptr;
		void *methaddr;
		BOOL stdcall;

		while (1)
		{
			entry = NULL;

			//---- find next matching entry ----
			Lock();
			while (i < DispatchList.GetSize())
			{
				if (DispatchList[i].Dispid == dispidMember)
				{
					entry = &DispatchList[i];
					thisptr = entry->ThisPtr;
					methaddr = entry->MethAddr;
					stdcall = entry->StdCall;
					break;
				}
				i++;
			}
			Unlock();

			if (! entry)
				break;

			_ATL_FUNC_INFO info;

			HRESULT hr = GetFuncInfoFromId(m_iid, dispidMember, lcid, info);
			if (SUCCEEDED(hr))
				InvokeFromFuncInfo(thisptr, methaddr, stdcall, info, pdispparams, pvarResult);

			i++;
		}

		return S_OK;
	}

	//Helper for invoking the event
	HRESULT InvokeFromFuncInfo(void *thisptr, void *methaddr, BOOL stdcall,
		_ATL_FUNC_INFO& info, DISPPARAMS* pdispparams, VARIANT* pvarResult)
	{
		VARIANTARG** pVarArgs = info.nParams ? (VARIANTARG**)alloca(sizeof(VARIANTARG*)*info.nParams) : 0;
		for (int i=0; i<info.nParams; i++)
			pVarArgs[i] = &pdispparams->rgvarg[info.nParams - i - 1];

		CComVariant tmpResult;
		if (pvarResult == NULL)
			pvarResult = &tmpResult;

		HRESULT hr;

		//--------------------------------------------------------------------------
		//---- we need to use "CComDualThunk" here because DispCallFunc doesn't	 ---
		//---- take a method address; just a vtable offset.  Doesn't work for us ---
		//---- if target method is not virtual.  Also, doesn't support the		 ---
		//---- "thiscall" calling convention (which we need).					 ---
		//--------------------------------------------------------------------------
		CComDualThunk thunk;
		thunk.Init((DWORD)methaddr, thisptr, stdcall);
		CALLCONV cc = CC_STDCALL;

		if ((! stdcall) && (! thisptr))
			cc = CC_CDECL;

		hr = DispCallFunc(&thunk, 0, cc, info.vtReturn, info.nParams, 
			info.pVarTypes, pVarArgs, pvarResult);

		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}

	//---- below functions are same as in IDispEventSimpleImpl ----
		STDMETHOD(_LocDEQueryInterface)(REFIID riid, void ** ppvObject)
	{
		if (InlineIsEqualGUID(riid, m_iid) || 
			InlineIsEqualUnknown(riid) ||
			InlineIsEqualGUID(riid, IID_IDispatch) ||
			InlineIsEqualGUID(riid, m_iid))
		{
			if (ppvObject == NULL)
				return E_POINTER;
			*ppvObject = this;
			AddRef();
#ifdef _ATL_DEBUG_INTERFACES
			_pModule->AddThunk((IUnknown**)ppvObject, _T("IDispEventImpl"), riid);
#endif // _ATL_DEBUG_INTERFACES
			return S_OK;
		}
		else
			return E_NOINTERFACE;
	}

	// These are here only to support use in non-COM objects	
	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return 1;
	}
	
	virtual ULONG STDMETHODCALLTYPE Release()
	{
		return 1;
	}

	//Helpers for sinking events on random IUnknown*
	HRESULT DispEventAdvise(IUnknown* pUnk, const IID* piid)
	{
		ATLASSERT(m_dwEventCookie == 0xFEFEFEFE);
		return AtlAdvise(pUnk, (IUnknown*)this, *piid, &m_dwEventCookie);
	}

	HRESULT DispEventUnadvise(IUnknown* pUnk, const IID* piid)
	{
		HRESULT hr = AtlUnadvise(pUnk, *piid, m_dwEventCookie);
		m_dwEventCookie = 0xFEFEFEFE;
		return hr;
	}

	HRESULT DispEventAdvise(IUnknown* pUnk)
	{
		return _IDispEvent::DispEventAdvise(pUnk, &m_iid);
	}

	HRESULT DispEventUnadvise(IUnknown* pUnk)
	{
		return _IDispEvent::DispEventUnadvise(pUnk, &m_iid);
	}

protected:
	// static 
	CComTypeInfoHolder _tih;
	
	// static
	HRESULT GetTI(LCID lcid, ITypeInfo** ppInfo)
	{
		return _tih.GetTI(lcid, ppInfo);
	}

	CSimpleArray<CDispatchEntry> DispatchList;
	BOOL Dispatch;
	int SlotCnt;
	int EntryCodeLen;
	void **VTable;
};
//-------------------------------------------------------------------------
STRUCT CManagedDelegateEntry
{
	void *Src;
	CString EventName;
	Delegate *Dlg;
};
//-------------------------------------------------------------------------
template <class TCriticalSection>
STRUCT CClassicDelegateEntry
{
	IUnknown *Src;
	GUID Guid;
	CClassicDelegate<TCriticalSection> *Dlg;
};
//-------------------------------------------------------------------------
template <class TCriticalSection>
class CEventReceiverUClassic :
	public CSimpleArray<CClassicDelegateEntry<TCriticalSection> >,
	public TCriticalSection
{
	//---- this class has 2 levels of synchronization support ----
	//---- one at this outer level and one at the CClassicDelegate level ----
	
public:
	BOOL Hook(IUnknown *src, LPCOLESTR EventName, 
		void *thisptr, void *methaddr, BOOL stdcall, int paramlen, LPCSTR signature=NULL)
	{
		ATLASSERT(src != NULL);
		ATLASSERT(EventName != NULL);
		ATLASSERT(methaddr != NULL);
		
		GUID guid;
		DISPID dispid;
		int vtSlot;
		CComPtr<ITypeInfo> spTypeInfo;

		if (! GetEventInfo(src, EventName, &guid, &dispid, &spTypeInfo, &vtSlot))
			return FALSE;

		CClassicDelegate<TCriticalSection> *dlg = FindDelegate(src, &guid);
		if (! dlg)
		{
			dlg = new CClassicDelegate<TCriticalSection> (src, spTypeInfo, &guid, thisptr);
			Add(src, &guid, dlg);

			//---- do a single Advise to add this guy to CPC ----
			dlg->DispEventAdvise(src, &guid);
		}

		dlg->AddHandler(dispid, thisptr, methaddr, stdcall, vtSlot, paramlen, signature);

		return TRUE;
	}
			
	BOOL Unhook(IUnknown *src, LPCOLESTR EventName, 
		void *thisptr, void *methaddr, BOOL stdcall)
	{
		GUID guid;
		DISPID dispid;
		int vtSlot;

		if (! GetEventInfo(src, EventName, &guid, &dispid, NULL, &vtSlot))
			return FALSE;

		CClassicDelegate<TCriticalSection> *dlg = FindDelegate(src, &guid);
		if (! dlg)
			return FALSE;

		Lock();

		dlg->RemoveHandler(dispid, thisptr, methaddr, stdcall, vtSlot);
		if (dlg->IsEmpty())
		{
			Remove(src, &guid);
			dlg->DispEventUnadvise(src, &guid);
			dlg->Release();
		}

		Unlock();

		return TRUE;
	}

	BOOL UnhookAll()
	{
		BOOL gotone;
		CClassicDelegateEntry<TCriticalSection> entry;
		BOOL retval = TRUE;

		while (1)
		{
			//---- protect access of next element ----
			gotone = FALSE;

			Lock();
			int size = GetSize();
			if (size)
			{
				gotone = TRUE;
				entry = m_aT[size-1];
				RemoveAt(size-1);
			}
			Unlock();
			
			if (! gotone)
				break;
			
			entry.Dlg->DispEventUnadvise(entry.Src, &entry.Guid);
			entry.Dlg->Release();
		}

		return retval;
	}

protected:
	CClassicDelegate<TCriticalSection> *FindDelegate(IUnknown *src, const GUID *guid) 
	{
		ATLASSERT(src != NULL);
		ATLASSERT(guid != NULL);

		CClassicDelegate<TCriticalSection> *retval = NULL;

		Lock();
		for (int i = 0; i < m_nSize; i++)
		{
			if ((m_aT[i].Src == src) && (memcmp(&m_aT[i].Guid, guid, sizeof(GUID))==0))
			{
				retval = m_aT[i].Dlg;
				break;
			}
		}
		Unlock();

		return retval;  // not found
	}

	BOOL Add(IUnknown *src, const GUID *guid, CClassicDelegate<TCriticalSection> *dlg)
	{
		ATLASSERT(src != NULL);
		ATLASSERT(guid != NULL);
		ATLASSERT(dlg != NULL);

		CClassicDelegateEntry<TCriticalSection> entry;
		entry.Src = src;
		memcpy(&entry.Guid, guid, sizeof(GUID));
		entry.Dlg = dlg;

		//---- call our base class to do the real work ----
		Lock();
		BOOL retval = CSimpleArray<CClassicDelegateEntry<TCriticalSection> >::Add(entry);
		Unlock();

		return retval;
	}

	BOOL Remove(IUnknown *src, const GUID *guid)
	{
		ATLASSERT(src != NULL);
		ATLASSERT(guid != NULL);

		BOOL retval = FALSE;

		Lock();

		for (int i = 0; i < m_nSize; i++)
		{
			if ((m_aT[i].Src == src) && (memcpy(&m_aT[i].Guid, &guid, sizeof(GUID))==0))
			{
				RemoveAt(i);
				retval = TRUE;		// found
				break;
			}
		}

		Unlock();

		return retval;
	}

	static BOOL GetEventInfo(IUnknown *src, LPCOLESTR EventName,
		GUID *pGuid, DISPID *pDispid, ITypeInfo **ppTypeInfo, int *pSlotNum) 
	{
		ATLASSERT(src != NULL);
		ATLASSERT(EventName != NULL);
		ATLASSERT(pGuid != NULL);
		ATLASSERT(pDispid != NULL);

		//---- set default values ----
		BOOL gotguid = FALSE;
		GUID guid;

		char *event2;
		CComPtr<ITypeLib> spTypeLib;

		spTypeLib = GetTypeLib(src);
		if (! (spTypeLib))
			return FALSE;
				
		USES_CONVERSION;
		char *p = OLE2A(EventName);
		
		char *q = strstr(p, "::");
		if (q)
		{
			char ifname[512];
			strcpy(ifname, p);
			ifname[q-p] = 0;

			gotguid = GetNamedSourceInterface(src, spTypeLib, A2OLE(ifname), &guid);
			if (! gotguid)
				return FALSE;

			event2 = q+2;
		}
		else
		{
			event2 = p;
			gotguid = GetDefaultSourceGuid(src, spTypeLib, &guid);
		}

		if (gotguid)
		{
			DISPID dispid;
	
			//---- translate "event2" into a dispid ----
			ITypeInfo *pTypeInfo;
			HRESULT hr = spTypeLib->GetTypeInfoOfGuid(guid, &pTypeInfo);
			if (SUCCEEDED(hr))
			{
				// These sneaky dual interfaces have two ITypeInfos: 
				// a TKIND_DISPATCH and TKIND_INTERFACE.  We get the 
				// TKIND_DISPATCH by default.  This secret
				// code gets the TKIND_INTERFACE.

				TYPEATTR *typeattr;
				DWORD typeflags;

				HRESULT hr = pTypeInfo->GetTypeAttr(&typeattr);
				if (FAILED(hr))
					return FALSE;
				typeflags = typeattr->wTypeFlags;
				pTypeInfo->ReleaseTypeAttr(typeattr);
				
				if (typeflags & TYPEFLAG_FDUAL)			// dual
				{
					HREFTYPE hreftype;
					hr = pTypeInfo->GetRefTypeOfImplType((unsigned int)-1, &hreftype);
					if (FAILED(hr))
						return FALSE;

					ITypeInfo *pDualTypeInfo;
					hr = pTypeInfo->GetRefTypeInfo(hreftype, &pDualTypeInfo);
					if (FAILED(hr))
						return FALSE;

					pTypeInfo->Release();		// get rid of DISPATCH version
					pTypeInfo = pDualTypeInfo;
				}

				LPOLESTR wstr = A2OLE(event2);
				hr = pTypeInfo->GetIDsOfNames(&wstr, 1, &dispid);
				if (SUCCEEDED(hr))
				{
					memcpy(pGuid, &guid, sizeof(GUID));
					*pDispid = dispid;

					//---- get vtable offset of member ----
					hr = GetMemberVTableSlotFromId(pTypeInfo, dispid, pSlotNum);
					if (SUCCEEDED(hr))
					{
						//---- return typeinfo if all went well ----
						if (ppTypeInfo)
							*ppTypeInfo = pTypeInfo;
						else
							pTypeInfo->Release();
	
						return TRUE;
					}
				}
				pTypeInfo->Release();
			}
		}

		return FALSE;
	}

	static HRESULT GetMemberVTableSlotFromId(ITypeInfo *pTypeInfo, DISPID dispidMember, int *pSlot) 
	{
		ATLASSERT(pTypeInfo != NULL);
		ATLASSERT(pSlot != NULL);

		int memberid = -1;
		HRESULT hr;
		FUNCDESC *funcdesc;

		CComQIPtr<ITypeInfo2, &IID_ITypeInfo2> spTypeInfo2 = pTypeInfo;
		if (spTypeInfo2 != NULL)		// fastest way
		{
			UINT index;
			hr = spTypeInfo2->GetFuncIndexOfMemId(dispidMember, INVOKE_FUNC, &index);
			if (FAILED(hr))
				return hr;
			hr = pTypeInfo->GetFuncDesc(index, &funcdesc);
			if (FAILED(hr))
				return hr;
			*pSlot = funcdesc->oVft/4;
			pTypeInfo->ReleaseFuncDesc(funcdesc);
			return S_OK;
		}

		//---- search for funcdesc ----
		TYPEATTR* pAttr;
		hr = pTypeInfo->GetTypeAttr(&pAttr);
		if (FAILED(hr))
			return hr;

		BOOL gotit = FALSE;

		for (int i=0;i<pAttr->cFuncs;i++)
		{
			hr = pTypeInfo->GetFuncDesc(i, &funcdesc);
			if (SUCCEEDED(hr))
			{
				if (funcdesc->memid == dispidMember)
				{
					*pSlot = funcdesc->oVft/4;
					gotit = TRUE;
				}
				pTypeInfo->ReleaseFuncDesc(funcdesc);
			}
			if (gotit)
				break;
		}

		pTypeInfo->ReleaseTypeAttr(pAttr);
		if (! gotit)
			return E_FAIL;
		
		return S_OK;
	}

	static ITypeLib *GetTypeLib(IUnknown *punkObj) 
	{
		ATLASSERT(punkObj != NULL);

		HRESULT hr = E_FAIL;
		CComPtr<IDispatch> spDispatch;
		CComPtr<ITypeInfo> spTypeInfo;

		if (! punkObj)
			goto exit;

		hr = punkObj->QueryInterface(IID_IDispatch, (void**)&spDispatch);
		if (FAILED(hr))
			goto exit;
		
		hr = spDispatch->GetTypeInfo(0, 0, &spTypeInfo);
		if (FAILED(hr))
			goto exit;
		
		ITypeLib *pTypeLib;
		hr = spTypeInfo->GetContainingTypeLib(&pTypeLib, 0);
		if (SUCCEEDED(hr))
			return pTypeLib;

	exit:
		return NULL;
	}

	static BOOL GetDefaultSourceGuid(IUnknown *punkObj, ITypeLib *pTypeLib, GUID *pGuid)
	{
		ATLASSERT(punkObj != NULL);
		ATLASSERT(pTypeLib != NULL);

		HRESULT hr;
		int i;

		// First see if the object is willing to tell us about the
		// default source interface via IProvideClassInfo2
		CComPtr<IProvideClassInfo2> spInfo;
		hr = punkObj->QueryInterface(IID_IProvideClassInfo2, (void**)&spInfo);
		if ((SUCCEEDED(hr)) && (spInfo != NULL))
		{
			hr = spInfo->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID, pGuid);
			return SUCCEEDED(hr);
		}

		// No, we have to go hunt for it
		CComPtr<ITypeInfo> spInfoCoClass;
		CComPtr<IPersist> spPersist;
		TYPEATTR* pAttr=NULL;

		// Try to locate the clsid from IPersist
		CLSID clsid;
		hr = punkObj->QueryInterface(IID_IPersist, (void**)&spPersist);
		if (FAILED(hr))
			goto exit;
		
		hr = spPersist->GetClassID(&clsid);
		if (FAILED(hr))
			goto exit;
		
		hr = pTypeLib->GetTypeInfoOfGuid(clsid, &spInfoCoClass);
		if (FAILED(hr))
			goto exit;
		
		hr = spInfoCoClass->GetTypeAttr(&pAttr);
		if (! pAttr)
			hr = E_FAIL;
		if (FAILED(hr))
			goto exit;
		
 		HREFTYPE hRef;
		for (i = 0; i < pAttr->cImplTypes; i++)
		{
			int nType;
			hr = spInfoCoClass->GetImplTypeFlags(i, &nType);
			if (FAILED(hr))
				continue;

			if (nType == (IMPLTYPEFLAG_FDEFAULT | IMPLTYPEFLAG_FSOURCE))
			{
				// we found it
				hr = spInfoCoClass->GetRefTypeOfImplType(i, &hRef);
				if (SUCCEEDED(hr))
				{
					CComPtr<ITypeInfo> spInfo;
					hr = spInfoCoClass->GetRefTypeInfo(hRef, &spInfo);
					if (SUCCEEDED(hr))
					{
						TYPEATTR* pAttrIF;
						spInfo->GetTypeAttr(&pAttrIF);
						if (pAttrIF != NULL)
						{
							memcpy(pGuid, &pAttrIF->guid, sizeof(GUID));
						}
						spInfo->ReleaseTypeAttr(pAttrIF);
					}
				}
				break;
			}
		}

		spInfoCoClass->ReleaseTypeAttr(pAttr);

	exit:
		return SUCCEEDED(hr);
	}

	static BOOL GetNamedSourceInterface(IUnknown *punkObj, ITypeLib *pTypeLib, 
		LPCOLESTR ifacename, GUID *pGuid) 
	{
		ATLASSERT(punkObj != NULL);
		ATLASSERT(pTypeLib != NULL);
		ATLASSERT(ifacename != NULL);

		HRESULT hr;
		CString strIfaceName = ifacename;
		CComPtr<ITypeInfo> spInfoCoClass;
		TYPEATTR* pAttr=NULL;
		CComPtr<IPersist> spPersist;
		CLSID clsid;
		int i;

		//---- try to locate the typeinfo for the clsid ----
		CComPtr<IProvideClassInfo> spProvideClassInfo;
		hr = punkObj->QueryInterface(IID_IProvideClassInfo, (void **)&spProvideClassInfo);
		if (SUCCEEDED(hr))
			hr = spProvideClassInfo->GetClassInfo(&spInfoCoClass);

		if (FAILED(hr))
		{
			// Try to locate the clsid from IPersist
			hr = punkObj->QueryInterface(IID_IPersist, (void**)&spPersist);
			if (FAILED(hr))
				goto exit;
			
			hr = spPersist->GetClassID(&clsid);
			if (FAILED(hr))
				goto exit;
			
			hr = pTypeLib->GetTypeInfoOfGuid(clsid, &spInfoCoClass);
			if (FAILED(hr))
				goto exit;
		}
		
		hr = spInfoCoClass->GetTypeAttr(&pAttr);
		if (! pAttr)
			hr = E_FAIL;
		if (FAILED(hr))
			goto exit;
		
 		HREFTYPE hRef;

		for (i = 0; i < pAttr->cImplTypes; i++)
		{
			int nType;
			hr = spInfoCoClass->GetImplTypeFlags(i, &nType);
			if (FAILED(hr))
				continue;

			if (nType | IMPLTYPEFLAG_FSOURCE)		// a source interface
			{
				hr = spInfoCoClass->GetRefTypeOfImplType(i, &hRef);
				if (SUCCEEDED(hr))
				{
					CComPtr<ITypeInfo> spInfo;
					hr = spInfoCoClass->GetRefTypeInfo(hRef, &spInfo);
					if (SUCCEEDED(hr))
					{
						BSTR bstrName;
						BOOL gotit = FALSE;
						hr = spInfo->GetDocumentation(-1, &bstrName, NULL, NULL, NULL);	// name of interface
						if (SUCCEEDED(hr))
						{
							if (strIfaceName == bstrName)		// got it!
							{
								TYPEATTR* pAttrIF;
								spInfo->GetTypeAttr(&pAttrIF);
								if (pAttrIF != NULL)
								{
									memcpy(pGuid, &pAttrIF->guid, sizeof(GUID));
									gotit = TRUE;
								}
								spInfo->ReleaseTypeAttr(pAttrIF);
							}
							
							SysFreeString(bstrName);
							if (gotit)
								break;
						}
					}
				}
			}
		}

		spInfoCoClass->ReleaseTypeAttr(pAttr);

	exit:
		return SUCCEEDED(hr);
	}
};
//-------------------------------------------------------------------------
