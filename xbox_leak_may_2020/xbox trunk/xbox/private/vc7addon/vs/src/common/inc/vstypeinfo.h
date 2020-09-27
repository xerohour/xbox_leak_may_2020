//--------------------------------------------------------------------------
// Microsoft Visual Basic
//
// Copyright (c) 2000 Microsoft Corporation 
// All rights reserved
//
// VSTypeInfo.h
//
// Modified version of CComTypeInfoHolder that loads the typelib from the 
// HMODULE instead of from the registry. This allows the typelib of the interface 
// implemented by the class using this class, to not be registered.
// The index of the type library resource to load is passed
// in by the classes that use this class instead of the major ver they would 
// have otherwise passed to IDispatchImpl. ( Major ver is no longer needed 
// since the type lib is not loaded from the registry.)
//---------------------------------------------------------------------------
#ifndef __VSTYPEINFO_H__
#define __VSTYPEINFO_H__

#include "vsmodule.h"

class CVsTypeInfoHolder
{
// Should be 'protected' but can cause compiler to generate fat code.
public:
	const GUID* m_pguid;
	const GUID* m_plibid;
	WORD m_wTypeLibRes;
	WORD m_wMinor;

	ITypeInfo* m_pInfo;
	long m_dwRef;
	struct stringdispid
	{
		CComBSTR bstr;
		int nLen;
		DISPID id;
	};
	stringdispid* m_pMap;
	int m_nCount;

public:
	HRESULT GetTI(LCID lcid, ITypeInfo** ppInfo)
	{
		HRESULT hr = S_OK;
		if (m_pInfo == NULL)
			hr = GetTI(lcid);
		*ppInfo = m_pInfo;
		if (m_pInfo != NULL)
		{
			m_pInfo->AddRef();
			hr = S_OK;
		}
		return hr;
	}
	HRESULT GetTI(LCID lcid);
	HRESULT EnsureTI(LCID lcid)
	{
		HRESULT hr = S_OK;
		if (m_pInfo == NULL)
			hr = GetTI(lcid);
		return hr;
	}

	// This function is called by the module on exit
	// It is registered through _Module.AddTermFunc()
	static void __stdcall Cleanup(DWORD_PTR dw)
	{
		CVsTypeInfoHolder* p = (CVsTypeInfoHolder*) dw;
		if (p->m_pInfo != NULL)
			p->m_pInfo->Release();
		p->m_pInfo = NULL;
		delete [] p->m_pMap;
		p->m_pMap = NULL;
	}

	HRESULT GetTypeInfo(UINT /* itinfo */, LCID lcid, ITypeInfo** pptinfo)
	{
		HRESULT hRes = E_POINTER;
		if (pptinfo != NULL)
			hRes = GetTI(lcid, pptinfo);
		return hRes;
	}
	HRESULT GetIDsOfNames(REFIID /* riid */, LPOLESTR* rgszNames, UINT cNames,
		LCID lcid, DISPID* rgdispid)
	{
		HRESULT hRes = EnsureTI(lcid);
		if (m_pInfo != NULL)
		{
			for (int i=0; i<(int)cNames; i++)
			{
				int n = (int)ocslen(rgszNames[i]);
				for (int j=m_nCount-1; j>=0; j--)
				{
					if ((n == m_pMap[j].nLen) &&
						(memcmp(m_pMap[j].bstr, rgszNames[i], m_pMap[j].nLen * sizeof(OLECHAR)) == 0))
					{
						rgdispid[i] = m_pMap[j].id;
						break;
					}
				}
				if (j < 0)
				{
					hRes = m_pInfo->GetIDsOfNames(rgszNames + i, 1, &rgdispid[i]);
					if (FAILED(hRes))
						break;
				}
			}
		}
		return hRes;
	}

	HRESULT Invoke(IDispatch* p, DISPID dispidMember, REFIID /* riid */,
		LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
		EXCEPINFO* pexcepinfo, UINT* puArgErr)
	{
		HRESULT hRes = EnsureTI(lcid);
		if (m_pInfo != NULL)
			hRes = m_pInfo->Invoke(p, dispidMember, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
		return hRes;
	}
	HRESULT LoadNameCache(ITypeInfo* pTypeInfo)
	{
		TYPEATTR* pta;
		HRESULT hr = pTypeInfo->GetTypeAttr(&pta);
		if (SUCCEEDED(hr))
		{
			m_nCount = pta->cFuncs;
			m_pMap = m_nCount == 0 ? 0 : new stringdispid[m_nCount];
			for (int i=0; i<m_nCount; i++)
			{
				FUNCDESC* pfd;
				if (SUCCEEDED(pTypeInfo->GetFuncDesc(i, &pfd)))
				{
					CComBSTR bstrName;
					if (SUCCEEDED(pTypeInfo->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL)))
					{
						m_pMap[i].bstr.Attach(bstrName.Detach());
						m_pMap[i].nLen = SysStringLen(m_pMap[i].bstr);
						m_pMap[i].id = pfd->memid;
					}
					pTypeInfo->ReleaseFuncDesc(pfd);
				}
			}
			pTypeInfo->ReleaseTypeAttr(pta);
		}
		return S_OK;
	}
};


inline HRESULT CVsTypeInfoHolder::GetTI(LCID lcid)
{
	//If this assert occurs then most likely didn't initialize properly
	ATLASSERT(m_plibid != NULL && m_pguid != NULL);
	ATLASSERT(!InlineIsEqualGUID(*m_plibid, GUID_NULL) && "Did you forget to pass the LIBID to CComModule::Init?");

	if (m_pInfo != NULL)
		return S_OK;
	HRESULT hRes = E_FAIL;
	EnterCriticalSection(&_Module.m_csTypeInfoHolder);
	if (m_pInfo == NULL)
	{
		ITypeLib* pTypeLib;

        // Changed only this block of code from the original CComTypeInfoHolder to load from HMODULE 
        // instead of the registry.
        TCHAR szTypeLibRes[21];
        TCHAR szModuleFileName[_MAX_PATH + 1];

        HINSTANCE hInst = _Module.GetModuleInstance();
        DWORD dwSize = GetModuleFileName(hInst, szModuleFileName, _MAX_PATH);
        CString strTypeLibRes = szModuleFileName;
        // don't bother appending \1, as that's the default and just causes the 
        // system to have to look for two different files unnecessarily
        if( m_wTypeLibRes != 1 )
        {
	        strTypeLibRes += _T("\\");
	        _itot(m_wTypeLibRes, szTypeLibRes, 10);
	        strTypeLibRes += szTypeLibRes;
        }
        hRes = LoadTypeLib(strTypeLibRes, &pTypeLib);

        
		if (SUCCEEDED(hRes))
		{
			CComPtr<ITypeInfo> spTypeInfo;
			hRes = pTypeLib->GetTypeInfoOfGuid(*m_pguid, &spTypeInfo);
			if (SUCCEEDED(hRes))
			{
				CComPtr<ITypeInfo> spInfo(spTypeInfo);
				CComPtr<ITypeInfo2> spTypeInfo2;
				if (SUCCEEDED(spTypeInfo->QueryInterface(&spTypeInfo2)))
					spInfo = spTypeInfo2;

				LoadNameCache(spInfo);
				m_pInfo = spInfo.Detach();
			}
			pTypeLib->Release();
		}
        else
            ASSERT(FALSE);

	}
	LeaveCriticalSection(&_Module.m_csTypeInfoHolder);
	_Module.AddTermFunc(Cleanup, (DWORD_PTR)this);
	return hRes;
}


#endif