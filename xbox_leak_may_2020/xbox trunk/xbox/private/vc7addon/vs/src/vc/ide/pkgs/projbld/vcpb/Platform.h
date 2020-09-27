// Platform.h: Definition of the CBasePlatform, CPlatformWin32, and CPlatformWin64 classes
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vccolls.h"
#include "comlist.h"
#include "collection.h"

/////////////////////////////////////////////////////////////////////////////
// CBasePlatform
template <class TClass, const CLSID* TPClsid>
class CBasePlatform : 
	public IDispatchImpl<VCPlatform, &__uuidof(VCPlatform), &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCPlatformImpl,
	public CComObjectRoot,
	public CComCoClass<TClass, TPClsid>
{
public:
	CBasePlatform() {}
	~CBasePlatform() {}

BEGIN_COM_MAP(TClass)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCPlatform)
	COM_INTERFACE_ENTRY(IVCPlatformImpl)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(TClass) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

public:
	CComBSTR m_bstrExecPath;
	CComBSTR m_bstrIncPath;
	CComBSTR m_bstrRefPath;
	CComBSTR m_bstrLibPath;
	CComBSTR m_bstrSrcPath;

//data
protected:
	CComDynamicListTyped<IVCToolImpl> m_rgTools; // collection of Tools. 

// VCPlatform
public:
//	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Tools)(/*[out, retval]*/  IDispatch ** ppVal)
	{
		CHECK_POINTER_VALID(ppVal);
		CComPtr<IVCCollection> pCollection;

		HRESULT hr = CCollection<IVCToolImpl>::CreateInstance(&pCollection, &m_rgTools);
		if (SUCCEEDED(hr))
			hr = pCollection.QueryInterface(ppVal);
		return hr;
	}
//	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);

	STDMETHOD(get_ExecutableDirectories)(/*[out, retval]*/ BSTR *pVal)
	{
		return m_bstrExecPath.CopyTo(pVal);
	}
	STDMETHOD(put_ExecutableDirectories)(/*[in]*/ BSTR newVal)
	{
		StripQuotesAndSetValue(newVal, m_bstrExecPath);
		return S_OK;
	}
	STDMETHOD(get_IncludeDirectories)(/*[out, retval]*/ BSTR *pVal)
	{
		return m_bstrIncPath.CopyTo(pVal);
	}
	STDMETHOD(put_IncludeDirectories)(/*[in]*/ BSTR newVal)
	{
		StripQuotesAndSetValue(newVal, m_bstrIncPath);
		return S_OK;
	}
	STDMETHOD(get_ReferenceDirectories)(/*[out, retval]*/ BSTR *pVal)
	{
		return m_bstrRefPath.CopyTo(pVal);
	}
	STDMETHOD(put_ReferenceDirectories)(/*[in]*/ BSTR newVal)
	{
		StripQuotesAndSetValue(newVal, m_bstrRefPath);
		return S_OK;
	}
	STDMETHOD(get_LibraryDirectories)(/*[out, retval]*/ BSTR *pVal)
	{
		return m_bstrLibPath.CopyTo(pVal);
	}
	STDMETHOD(put_LibraryDirectories)(/*[in]*/ BSTR newVal)
	{
		StripQuotesAndSetValue(newVal, m_bstrLibPath);
		return S_OK;
	}
	STDMETHOD(get_SourceDirectories)(/*[out, retval]*/ BSTR *pVal)
	{
		return m_bstrSrcPath.CopyTo(pVal);
	}
	STDMETHOD(put_SourceDirectories)(/*[in]*/ BSTR newVal)
	{
		StripQuotesAndSetValue(newVal, m_bstrSrcPath);
		return S_OK;
	}
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)
	{ 
		CHECK_POINTER_VALID(projEngine);
		RETURN_ON_NULL(g_pProjectEngine);
		return g_pProjectEngine->QueryInterface(IID_IDispatch, (void **)projEngine);
	}
	STDMETHOD(Evaluate)(BSTR In, BSTR* Out)
	{
		RETURN_ON_NULL(g_pProjectEngine);
		return g_pProjectEngine->Evaluate(In, Out);
	}

// IVCPlatformImpl
	STDMETHOD(Close)()
	{
		long l, lcItems;

		lcItems = m_rgTools.GetSize();
		for (l = 0; l < lcItems; l++)
		{
			CComQIPtr<IVCToolImpl> pToolImpl;
			pToolImpl = m_rgTools.GetAt(l);
			if (pToolImpl)
				pToolImpl->Close(); // close down entirely; release all
		}

		m_rgTools.RemoveAll(); // collection of Tools. 
		return S_OK;
	}
//	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
//	STDMETHOD(Initialize)( void ); 	
//	STDMETHOD(IsToolInToolset)(toolSetType listStyle, IVCToolImpl* pTool, VARIANT_BOOL* pbInToolset);

protected:
	void StripQuotesAndSetValue(BSTR newVal, CComBSTR& bstrVal)
	{
		// strip quotations
		CStringW temp( newVal );
		temp.Remove(L'\"');
		CComBSTR out(temp);
		bstrVal = out;
	}

	void DoSetToolExtensions(IVCToolImpl* pTool, LPCOLESTR strKey, LPOLESTR szToolShortName)
	{
		CComBSTR bstrVal;
		HRESULT hr = GetRegStringW(strKey, szToolShortName, &bstrVal);
		if( SUCCEEDED(hr) && bstrVal.Length()>0 )
		{
			pTool->put_DefaultExtensions(bstrVal);
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// CPlatformWin32

class CPlatformWin32 : 
	public CBasePlatform<CPlatformWin32, &CLSID_VCPlatformWin32>
{
public:
DECLARE_NO_REGISTRY()

// VCPlatform
public:
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);

// IVCPlatformImpl
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(Initialize)( void ); 	
	STDMETHOD(IsToolInToolset)(toolSetType listStyle, IVCToolImpl* pTool, VARIANT_BOOL* pbInToolset);

protected:
	static CVCStringWList		s_strToolsExcludeList[5];		// tools excluded for the tool type
};

/////////////////////////////////////////////////////////////////////////////
// CPlatformWin64

class CPlatformWin64 : 
	public CBasePlatform<CPlatformWin64, &CLSID_VCPlatformWin64>
{
public:
#ifdef _WIN64
DECLARE_VS_REGISTRY_RESOURCEID(IDR_PLATFORM_WIN64)
#else	// _WIN64
DECLARE_NO_REGISTRY()
#endif	// _WIN64

// VCPlatform
public:
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);

// IVCPlatformImpl
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(Initialize)( void ); 	
	STDMETHOD(IsToolInToolset)(toolSetType listStyle, IVCToolImpl* pTool, VARIANT_BOOL* pbInToolset);

protected:
	static CVCStringWList		s_strToolsExcludeList[5];		// tools excluded for the tool type
};
