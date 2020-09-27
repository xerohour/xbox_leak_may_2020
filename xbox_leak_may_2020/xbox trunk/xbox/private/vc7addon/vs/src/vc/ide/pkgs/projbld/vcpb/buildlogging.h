#pragma once

// #include <ErrContext.h>
#include "VCProjectEngine.h"

class CBldCommandLineEC :
	public IDispatchImpl<IVCBuildErrorContext, &IID_IVCBuildErrorContext, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot
{
public:
	CBldCommandLineEC()
	{
		s_cnt++;
		m_nContext = s_cnt;
		m_nErrors = 0;
		m_nWarnings = 0;
		m_bShowOutput = VARIANT_TRUE;
	}
	virtual ~CBldCommandLineEC() {}
	static HRESULT CreateInstance(IVCBuildErrorContext** ppContext, CBldCommandLineEC** ppContextObj, 
		IVCBuildEngine* pBuildEngine);
	static void CloseOutputWindow();

BEGIN_COM_MAP(CBldCommandLineEC)
	COM_INTERFACE_ENTRY(IVCBuildErrorContext)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CBldCommandLineEC) 

// IVCBuildErrorContext
public:
	STDMETHOD(AddError)(BSTR bstrMessage, BSTR bstrHelp ,BSTR bstrFile, long nLine, BSTR bstrFullMsg );
	STDMETHOD(AddWarning)(BSTR bstrMessage, BSTR bstrHelp, BSTR bstrFile, long nLine, BSTR bstrFullMsg );
	STDMETHOD(AddInfo)(BSTR bstrMessage);
	STDMETHOD(AddLine)(BSTR bstrMessage);
	STDMETHOD(WriteLog)(BSTR bstrMessage);
	STDMETHOD(ActivateWindow)(VARIANT_BOOL bForce) { return S_OK; }
	STDMETHOD(ActivateTaskList)(VARIANT_BOOL bForce) { return S_OK; }
	STDMETHOD(Close)() { return S_OK; }
	STDMETHOD(ClearWindow)();
	STDMETHOD(get_BaseDirectory)(BSTR* pbstrBaseDir);
	STDMETHOD(put_BaseDirectory)(BSTR bstrBaseDir);
	STDMETHOD(get_ProjectName)(BSTR* pbstrName);
	STDMETHOD(put_ProjectName)(BSTR bstrName);
	STDMETHOD(get_Warnings)( long *pnWrn );
	STDMETHOD(get_Errors)( long *pnErr );
	STDMETHOD(get_AssociatedBuildEngine)(IDispatch** ppBldEngine);
	STDMETHOD(put_AssociatedBuildEngine)(IDispatch* pBldEngine);
	STDMETHOD(get_ShowOutput)(VARIANT_BOOL* pbShow);
	STDMETHOD(put_ShowOutput)(VARIANT_BOOL bShow);

protected:
	int m_nContext;
	static int s_cnt;
	long m_nErrors;
	long m_nWarnings;
	VARIANT_BOOL m_bShowOutput;
	CComBSTR m_bstrDir;
	CComBSTR m_bstrName;
	CComQIPtr<IVCBuildEngine> m_spBuildEngine;
};

class CBldLogEnabler 
{ 
public:
	CBldLogEnabler(VCConfiguration *pProjCfg, IVCBuildEngine* pBuildEngine);
	~CBldLogEnabler();
protected:
	CComQIPtr<IVCBuildEngineImpl> m_spBuildEngineImpl;
};

