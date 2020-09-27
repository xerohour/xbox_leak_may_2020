// This is a part of the Active Template Library.
// Copyright (C) 1996-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLACC_H__
#define __ATLACC_H__

#pragma once

#include <oleacc.h>
#include <winable.h>
#include <atliface.h>
#include <atlbase.h>
#include <atlcom.h>
#pragma comment(lib, "oleacc.lib")

namespace ATL
{
template <class T>
class IAccessibleProxyImpl : public IAccessible, public IAccessibleProxy
{
public :
	IAccessible* m_pAccessible;
	IAccessibleServer* m_pAccessibleServer;
	IAccessibleProxyImpl() : m_pAccessible(NULL), m_pAccessibleServer(NULL)
	{
	}

	HRESULT STDMETHODCALLTYPE get_accParent(IDispatch **ppdispParent)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (ppdispParent == NULL)
			return E_POINTER;
		return m_pAccessible->get_accParent(ppdispParent);
	}
    
    HRESULT STDMETHODCALLTYPE get_accChildCount(long *pcountChildren)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pcountChildren== NULL)
			return E_POINTER;
		return m_pAccessible->get_accChildCount(pcountChildren);
	}
    
    HRESULT STDMETHODCALLTYPE get_accChild(VARIANT varChild, IDispatch **ppdispChild)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (ppdispChild == NULL)
			return E_POINTER;
		return m_pAccessible->get_accChild(varChild, ppdispChild);
	}
    
    HRESULT STDMETHODCALLTYPE get_accName(VARIANT varChild, BSTR *pszName)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pszName == NULL)
			return E_POINTER;
		return m_pAccessible->get_accName(varChild, pszName);
	}
    
    HRESULT STDMETHODCALLTYPE get_accValue(VARIANT varChild, BSTR *pszValue)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pszValue == NULL)
			return E_POINTER;
		return m_pAccessible->get_accValue(varChild, pszValue);
	}
    
    HRESULT STDMETHODCALLTYPE get_accDescription(VARIANT varChild, BSTR *pszDescription)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pszDescription == NULL)
			return E_POINTER;
		return m_pAccessible->get_accDescription(varChild, pszDescription);
	}
    
    HRESULT STDMETHODCALLTYPE get_accRole(VARIANT varChild, VARIANT *pvarRole)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pvarRole == NULL)
			return E_POINTER;
		return m_pAccessible->get_accRole(varChild, pvarRole);
	}
    
    HRESULT STDMETHODCALLTYPE get_accState(VARIANT varChild, VARIANT *pvarState)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pvarState == NULL)
			return E_POINTER;
		return m_pAccessible->get_accState(varChild, pvarState);
	}

    
    HRESULT STDMETHODCALLTYPE get_accHelp(VARIANT varChild, BSTR *pszHelp)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pszHelp == NULL)
			return E_POINTER;
		return m_pAccessible->get_accHelp(varChild, pszHelp);
	}

    HRESULT STDMETHODCALLTYPE get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pszHelpFile == NULL)
			return E_POINTER;
		if (pidTopic == NULL)
			return E_POINTER;
		return m_pAccessible->get_accHelpTopic(pszHelpFile, varChild, pidTopic);
	}

    
    HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pszKeyboardShortcut == NULL)
			return E_POINTER;
		return m_pAccessible->get_accKeyboardShortcut(varChild, pszKeyboardShortcut);
	}

    
    HRESULT STDMETHODCALLTYPE get_accFocus(VARIANT *pvarChild)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pvarChild == NULL)
			return E_POINTER;
		return m_pAccessible->get_accFocus(pvarChild);
	}

    
    HRESULT STDMETHODCALLTYPE get_accSelection(VARIANT *pvarChildren)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pvarChildren == NULL)
			return E_POINTER;
		return m_pAccessible->get_accSelection(pvarChildren);
	}

    
    HRESULT STDMETHODCALLTYPE get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pszDefaultAction == NULL)
			return E_POINTER;
		return m_pAccessible->get_accDefaultAction(varChild, pszDefaultAction);
	}

    
    HRESULT STDMETHODCALLTYPE accSelect(long flagsSelect, VARIANT varChild)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return m_pAccessible->accSelect(flagsSelect, varChild);
	}

    
    HRESULT STDMETHODCALLTYPE accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pxLeft == NULL)
			return E_POINTER;
			
		if (pyTop == NULL)
			return E_POINTER;
			
		if (pcxWidth == NULL)
			return E_POINTER;
			
		if (pcyHeight == NULL)
			return E_POINTER;
		return m_pAccessible->accLocation(pxLeft, pyTop, pcxWidth, pcyHeight, varChild);
	}

    
    HRESULT STDMETHODCALLTYPE accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pvarEndUpAt == NULL)
			return E_POINTER;
		return m_pAccessible->accNavigate(navDir, varStart, pvarEndUpAt);
	}

    
    HRESULT STDMETHODCALLTYPE accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		if (pvarChild == NULL)
			return E_POINTER;
		return m_pAccessible->accHitTest(xLeft, yTop, pvarChild);
	}

    
    HRESULT STDMETHODCALLTYPE accDoDefaultAction(VARIANT varChild)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return m_pAccessible->accDoDefaultAction(varChild);
	}

    
    HRESULT STDMETHODCALLTYPE put_accName(VARIANT /*varChild*/, BSTR /*szName*/)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return E_NOTIMPL;
	}

    
    HRESULT STDMETHODCALLTYPE put_accValue(VARIANT /*varChild*/, BSTR /*szValue*/)
	{
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE SetServer(IAccessible *pAccessible, IAccessibleServer* pServer)
	{
		// hold a weak reference to the server
		m_pAccessible = pAccessible;
		m_pAccessibleServer = pServer;
		return S_OK;
	}
    virtual HRESULT STDMETHODCALLTYPE Invoke(
                DISPID dispIdMember,
                REFIID riid,
                LCID lcid,
                WORD wFlags,
                DISPPARAMS *pDispParams,
                VARIANT *pVarResult,
                EXCEPINFO *pExcepInfo,
                UINT *puArgErr) 
    {
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return m_pAccessible->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
    }
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
                REFIID riid,
                LPOLESTR *rgszNames,
                UINT cNames,
                LCID lcid,
                DISPID *rgDispId) 
    {
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return m_pAccessible->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
    }
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(unsigned int*  pctinfo) 
    {
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return m_pAccessible->GetTypeInfoCount(pctinfo);
    }
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo** ppTInfo) 
    {
		if (m_pAccessible == NULL)
			return RPC_E_DISCONNECTED;
		return m_pAccessible->GetTypeInfo(iTInfo, lcid, ppTInfo);
    }
};

class ATL_NO_VTABLE CAccessibleProxy : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IAccessibleProxyImpl<CAccessibleProxy>,
	public IOleWindow
{
public:
	CAccessibleProxy()
	{
	}
	~CAccessibleProxy()
	{
	}

	HRESULT STDMETHODCALLTYPE GetWindow(HWND* /*phwnd*/)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL /*fEnterMode*/)
	{
		return E_NOTIMPL;
	}

	HRESULT FinalRelease()
	{
		if (m_pAccessibleServer == NULL)
			return S_OK;
		return m_pAccessibleServer->SetProxy(NULL);
	}

BEGIN_COM_MAP(CAccessibleProxy)
	COM_INTERFACE_ENTRY(IAccessibleProxy)
	COM_INTERFACE_ENTRY(IAccessible)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IOleWindow)
END_COM_MAP()

public:
};



template <class T>
class IAccessibleImpl : public IAccessible, public IAccessibleServer
{
public :
	IAccessibleImpl() : m_pProxy(NULL)
	{
	}

	IAccessibleProxy* m_pProxy;
	CComPtr<IAccessible> m_spStdObject;
	HRESULT EnsureStdObj()
	{
		if (m_spStdObject == NULL)
		{
			T* pT = static_cast<T*>(this);
			HRESULT hr = CreateStdAccessibleObject(pT->m_hWnd, OBJID_CLIENT, __uuidof(IAccessible), (void**)&m_spStdObject);
			if (FAILED(hr))
				return hr;
		}
		return S_OK;
	}

	// Delegate to standard helper?
    HRESULT STDMETHODCALLTYPE get_accParent(IDispatch **ppdispParent)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accParent(ppdispParent);
	}
    
	// Delegate to standard helper?
    HRESULT STDMETHODCALLTYPE get_accChildCount(long *pcountChildren)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accChildCount(pcountChildren);
	}
    
	// Delegate to standard helper?
    HRESULT STDMETHODCALLTYPE get_accChild(VARIANT varChild, IDispatch **ppdispChild)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accChild(varChild, ppdispChild);
	}
    
	// Override in users code
    HRESULT STDMETHODCALLTYPE get_accName(VARIANT varChild, BSTR *pszName)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accName(varChild, pszName);
	}
    
	// Override in users code
	// Default inplementation will get window text and return it.
    HRESULT STDMETHODCALLTYPE get_accValue(VARIANT varChild, BSTR *pszValue)
	{
		return m_spStdObject->get_accValue(varChild, pszValue);
	}
    
	// Override in users code
    HRESULT STDMETHODCALLTYPE get_accDescription(VARIANT varChild, BSTR *pszDescription)
	{
		return m_spStdObject->get_accDescription(varChild, pszDescription);
	}
    
	// Investigate
	HRESULT STDMETHODCALLTYPE get_accRole(VARIANT varChild, VARIANT *pvarRole)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accRole(varChild, pvarRole);
	}
    
	// Investigate
    HRESULT STDMETHODCALLTYPE get_accState(VARIANT varChild, VARIANT *pvarState)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accState(varChild, pvarState);
	}

    // Override in User's code?
    HRESULT STDMETHODCALLTYPE get_accHelp(VARIANT varChild, BSTR *pszHelp)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accHelp(varChild, pszHelp);
	}

    // Override in user's code?
    HRESULT STDMETHODCALLTYPE get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accHelpTopic(pszHelpFile, varChild, pidTopic);
	}

    // Override in user's code?
    HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accKeyboardShortcut(varChild, pszKeyboardShortcut);
	}

    // Delegate to standard implementation?
    HRESULT STDMETHODCALLTYPE get_accFocus(VARIANT *pvarChild)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accFocus(pvarChild);
	}

    // Investigate
    HRESULT STDMETHODCALLTYPE get_accSelection(VARIANT *pvarChildren)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accSelection(pvarChildren);
	}

    // Override in user's code
    HRESULT STDMETHODCALLTYPE get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->get_accDefaultAction(varChild, pszDefaultAction);
	}

    // Investigate
    HRESULT STDMETHODCALLTYPE accSelect(long flagsSelect, VARIANT varChild)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->accSelect(flagsSelect, varChild);
	}

    // Delegate?
    HRESULT STDMETHODCALLTYPE accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->accLocation(pxLeft, pyTop, pcxWidth, pcyHeight, varChild);
	}

    // Delegate? May have to implement for COM children
    HRESULT STDMETHODCALLTYPE accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->accNavigate(navDir, varStart, pvarEndUpAt);
	}

    // Delegate?
    HRESULT STDMETHODCALLTYPE accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->accHitTest(xLeft, yTop, pvarChild);
	}

    // Override in user's code
    HRESULT STDMETHODCALLTYPE accDoDefaultAction(VARIANT varChild)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->accDoDefaultAction(varChild);
	}

	// Obsolete
    HRESULT STDMETHODCALLTYPE put_accName(VARIANT varChild, BSTR szName)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->put_accName(varChild, szName);
	}

    // Obsolete
    HRESULT STDMETHODCALLTYPE put_accValue(VARIANT varChild, BSTR szValue)
	{
		ATLASSERT(m_spStdObject != NULL);
		return m_spStdObject->put_accValue(varChild, szValue);
	}

    HRESULT STDMETHODCALLTYPE SetProxy(IAccessibleProxy *pUnknown)
	{
		// We keep a weak reference to the server
		m_pProxy = pUnknown;
		return S_OK;
	}
    
    HRESULT STDMETHODCALLTYPE GetHWND(HWND *phWnd)
	{
		if (phWnd == NULL)
			return E_POINTER;
		T* pT = static_cast<T*>(this);
		*phWnd = pT->m_hWnd;
		return S_OK;
	}

    HRESULT STDMETHODCALLTYPE GetEnumVariant(IEnumVARIANT **ppEnumVariant)
	{
		if (ppEnumVariant == NULL)
			return E_POINTER;
		*ppEnumVariant = NULL;
		return E_NOTIMPL;
	}
    virtual HRESULT STDMETHODCALLTYPE Invoke(
                DISPID dispIdMember,
                REFIID,
                LCID,
                WORD wFlags,
                DISPPARAMS *pDispParams,
                VARIANT *pVarResult,
                EXCEPINFO *,
                UINT *) 
    {
        if (pDispParams == 0) 
		{
            return DISP_E_BADVARTYPE;
        }
   
		HRESULT hr = DISP_E_MEMBERNOTFOUND;
		
        switch (dispIdMember) 
		{
        case DISPID_ACC_DODEFAULTACTION :		// -5018
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                hr = accDoDefaultAction(i1);
                break;
            }
        case DISPID_ACC_HITTEST :				// -5017
            {
                long i1 = V_I4(&pDispParams->rgvarg[1]);
                long i2 = V_I4(&pDispParams->rgvarg[0]);
                VARIANT* i3 = (VARIANT*) V_VARIANTREF(pVarResult);
                hr = accHitTest(i1, i2, i3);
                break;
            }
        case DISPID_ACC_NAVIGATE :				// -5016
            {
                long i1 = V_I4(&pDispParams->rgvarg[1]);
                VARIANT i2 = pDispParams->rgvarg[0];
                VARIANT* i3 = (VARIANT*) V_VARIANTREF(pVarResult);
                hr = accNavigate(i1, i2, i3);
                break;
            }
        case DISPID_ACC_LOCATION :				// -5015
            {
                long* i1 = (long*) V_I4REF(&pDispParams->rgvarg[4]);
                long* i2 = (long*) V_I4REF(&pDispParams->rgvarg[3]);
                long* i3 = (long*) V_I4REF(&pDispParams->rgvarg[2]);
                long* i4 = (long*) V_I4REF(&pDispParams->rgvarg[1]);
                VARIANT i5 = pDispParams->rgvarg[0];
                hr = accLocation(i1, i2, i3, i4, i5);
                break;
            }
        case DISPID_ACC_SELECT :				// -5014
            {
                long i1 = V_I4(&pDispParams->rgvarg[1]);
                VARIANT i2 = pDispParams->rgvarg[0];
                hr = accSelect(i1, i2);
                break;
            }
        case DISPID_ACC_DEFAULTACTION :			// -5013
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                BSTR* i2 = V_BSTRREF(pVarResult);
                hr = get_accDefaultAction(i1, i2);
                break;
            }
        case DISPID_ACC_SELECTION :				// -5012
            {
                VARIANT* i1 = (VARIANT*) V_VARIANTREF(pVarResult);
                hr = get_accSelection(i1);
                break;
            }
        case DISPID_ACC_FOCUS :					// -5011
            {
                VARIANT* i1 = (VARIANT*) V_VARIANTREF(pVarResult);
                hr = get_accFocus(i1);
                break;
            }
        case DISPID_ACC_KEYBOARDSHORTCUT :		// -5010
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                BSTR* i2 = V_BSTRREF(pVarResult);
                hr = get_accKeyboardShortcut(i1, i2);
                break;
            }
        case DISPID_ACC_HELPTOPIC :				// -5009
            {
                BSTR* i1 = V_BSTRREF(&pDispParams->rgvarg[1]);
                VARIANT i2 = pDispParams->rgvarg[0];
                long* i3 = (long*) V_I4REF(pVarResult);
                hr = get_accHelpTopic(i1, i2, i3);
                break;
            }
        case DISPID_ACC_HELP :					// -5008
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                BSTR* i2 = V_BSTRREF(pVarResult);
                hr = get_accHelp(i1, i2);
                break;
            }
        case DISPID_ACC_STATE :					// -5007
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                VARIANT* i2 = (VARIANT*) V_VARIANTREF(pVarResult);
                hr = get_accState(i1, i2);
                break;
            }
        case DISPID_ACC_ROLE :					// -5006
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                VARIANT* i2 = (VARIANT*) V_VARIANTREF(pVarResult);
                hr = get_accRole(i1, i2);
                break;
            }
        case DISPID_ACC_DESCRIPTION :			// -5005
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                BSTR* i2 = V_BSTRREF(pVarResult);
                hr = get_accDescription(i1, i2);
                break;
            }
        case DISPID_ACC_VALUE :					// -5004
            {
                if (wFlags & 2) 
				{
                    VARIANT i1 = pDispParams->rgvarg[0];
                    BSTR* i2 = V_BSTRREF(pVarResult);
                    hr = get_accValue(i1, i2);
                    break;
                }
                else if (wFlags & 4) 
				{
                    VARIANT i1 = pDispParams->rgvarg[1];
                    BSTR i2 = V_BSTR(&pDispParams->rgvarg[0]);
                    hr = put_accValue(i1, i2);
                    break;
                }
            }
        case DISPID_ACC_NAME :					// -5003
            {
                if (wFlags & 2) 
				{
                    VARIANT i1 = pDispParams->rgvarg[0];
                    BSTR* i2 = V_BSTRREF(pVarResult);
                    hr = get_accName(i1, i2);
                    break;
                }
                else if (wFlags & 4) 
				{
                    VARIANT i1 = pDispParams->rgvarg[1];
                    BSTR i2 = V_BSTR(&pDispParams->rgvarg[0]);
                    hr = put_accName(i1, i2);
                    break;
                }
            }
        case DISPID_ACC_CHILD :					// -5002
            {
                VARIANT i1 = pDispParams->rgvarg[0];
                IDispatch** i2 = V_DISPATCHREF(pVarResult);
                hr = get_accChild(i1, i2);
                break;
            }
        case DISPID_ACC_CHILDCOUNT :			// -5001
            {
                long* i1 = (long*) V_I4REF(pVarResult);
                hr = get_accChildCount(i1);
                break;
            }
        case DISPID_ACC_PARENT :				// -5000
            {
                IDispatch** i1 = V_DISPATCHREF(pVarResult);
                hr = get_accParent(i1);
                break;
            }
        default:
			break;
        }
        return hr;
    }
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
                REFIID,
                LPOLESTR *rgszNames,
                UINT cNames,
                LCID,
                DISPID *rgDispId) 
    {
	    static LPOLESTR names[] = 
		{ 
			L"accParent", 
			L"accChildCount", 
			L"accChild", 
			L"accName", 
			L"accValue", 
			L"accDescription", 
			L"accRole", 
			L"accState", 
			L"accHelp", 
			L"accHelpTopic", 
			L"accKeyboardShortcut", 
			L"accFocus", 
			L"accSelection", 
			L"accDefaultAction", 
			L"accSelect", 
			L"accLocation", 
			L"accNavigate", 
			L"accHitTest", 
			L"accDoDefaultAction"
		};
        static DISPID dids[] = 
		{ 
			DISPID_ACC_PARENT,				// -5000
			DISPID_ACC_CHILDCOUNT,			// -5001
			DISPID_ACC_CHILD,				// -5002
			DISPID_ACC_NAME,				// -5003
			DISPID_ACC_VALUE,				// -5004
			DISPID_ACC_DESCRIPTION,			// -5005
			DISPID_ACC_ROLE,				// -5006
			DISPID_ACC_STATE,				// -5007
			DISPID_ACC_HELP,				// -5008
			DISPID_ACC_HELPTOPIC,			// -5009
			DISPID_ACC_KEYBOARDSHORTCUT,	// -5010
			DISPID_ACC_FOCUS,				// -5011
			DISPID_ACC_SELECTION,			// -5012
			DISPID_ACC_DEFAULTACTION,		// -5013
			DISPID_ACC_SELECT,				// -5014
			DISPID_ACC_LOCATION,			// -5015
			DISPID_ACC_NAVIGATE,			// -5016
			DISPID_ACC_HITTEST,				// -5017
			DISPID_ACC_DODEFAULTACTION		// -5018
		};
        for (unsigned int i = 0; i < cNames; ++i) 
		{
            bool bFoundIt = false;
            for (unsigned int j = 0; j < sizeof(names)/sizeof(LPOLESTR); ++j) 
			{
                if (lstrcmpW(rgszNames[i], names[j]) == 0) 
				{
                    bFoundIt = true;
                    rgDispId[i] = dids[j];
                }
            }
            if (!bFoundIt) 
			{
                return DISP_E_UNKNOWNNAME;
            }
        }
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(unsigned int*  pctinfo) 
    {
        if (pctinfo == NULL) 
		{
            return E_POINTER;
        }
        *pctinfo = 1;
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(unsigned int /*iTInfo*/, LCID /*lcid*/, ITypeInfo** /*ppTInfo*/) 
    {
		return E_NOTIMPL;
    }
	long __stdcall QueryInterface(const struct _GUID &/*refIID*/, void **ppv )
	{
		if (ppv == NULL)
			return E_POINTER;
		*ppv = NULL;
/*		if (IsEqualGUID(refIID, __uuidof(IAccessibleServer)))
		{
			*ppv = static_cast<void*>(static_cast<IAccessibleServer*>(this));
			return S_OK;
		}
		if (IsEqualGUID(refIID, __uuidof(IAccessible)))
		{
			*ppv = static_cast<void*>(static_cast<IAccessible*>(this));
			return S_OK;
		}
		if (IsEqualGUID(refIID, __uuidof(IUnknown)))
		{
			*ppv = static_cast<void*>(static_cast<IUnknown*>(static_cast<IAccessible*>(this)));
			return S_OK;
		}
		if (IsEqualGUID(refIID, __uuidof(IDispatch)))
		{
			*ppv = static_cast<void*>(static_cast<IDispatch*>(static_cast<IAccessible*>(this)));
			return S_OK;
		}
*/
		return E_NOINTERFACE;
	}
	unsigned long __stdcall AddRef(void)
	{
		return 1;
	}
	unsigned long __stdcall Release(void)
	{
		return 1;
	}

	HRESULT CreateAccessibleProxy(WPARAM wParam, LPARAM lParam, LRESULT *plRet)
	{
		ATLASSERT(plRet != NULL);
		DWORD dwObjId = (DWORD) lParam;
		HRESULT hr = E_FAIL;

		if (dwObjId == OBJID_CLIENT)
		{
			hr = EnsureStdObj();
			if (SUCCEEDED(hr))
			{
				if (m_pProxy == NULL)
				{
					CComObject<CAccessibleProxy> *p;
					hr = CComObject<CAccessibleProxy>::CreateInstance(&p);
					if (SUCCEEDED(hr))
					{
						CComPtr<IAccessibleProxy> spProx;
						hr = p->QueryInterface(&spProx);
						if (SUCCEEDED(hr))
						{
							m_pProxy = spProx;
							spProx->SetServer(static_cast<IAccessible*>(this), static_cast<IAccessibleServer*>(this));
							*plRet = LresultFromObject (__uuidof(IAccessible), wParam, m_pProxy);
						}
						hr = S_OK;
					}
				}
				else
				{
					*plRet = LresultFromObject (__uuidof(IAccessible), wParam, m_pProxy);
					hr = S_OK;
				}
			}
		}
		return hr;
	}
};

} // namespace ATL

#endif