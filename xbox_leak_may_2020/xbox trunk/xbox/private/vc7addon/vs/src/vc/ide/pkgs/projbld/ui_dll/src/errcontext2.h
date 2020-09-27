#pragma once

// classes in this header
class CBldOutputWinEC;

#include <path2.h>
#include "bldpkg.h"

///////////////////////////////////////////////////////////////////////////////
//	Error context that writes to the output window in the IDE.  Note the 
//	the IDE must be in output window "mode" so that the output window is
//	visible, etc:
class CBldOutputWinEC :
	public IDispatchImpl<IVCBuildErrorContext, &IID_IVCBuildErrorContext, &LIBID_VCProjectEnginePrivateLibrary, 1,0, CVsTypeInfoHolder>, 
	public CComObjectRoot
{
public:
	CBldOutputWinEC();
	~CBldOutputWinEC();
	static HRESULT CreateInstance(IVCBuildErrorContext** ppContext, CBldOutputWinEC** ppContextObj, IVCBuildEngine* pBldEngine, 
		BSTR dir, IVCBuildOutput* pModelessOutputWindow);

BEGIN_COM_MAP(CBldOutputWinEC)
	COM_INTERFACE_ENTRY(IVCBuildErrorContext)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CBldOutputWinEC) 

// IVCBuildErrorContext
public:

	STDMETHOD(AddError)		(BSTR bstrMessage, BSTR bstrHelp, BSTR bstrFile, long nLine, BSTR bstrFull );
	STDMETHOD(AddWarning) 	(BSTR bstrMessage, BSTR bstrHelp, BSTR bstrFile, long nLine, BSTR bstrFull );
	STDMETHOD(AddInfo) 		(BSTR bstrMessage);
	STDMETHOD(AddLine)		(BSTR bstrMessage);

	STDMETHOD(WriteLog)		(BSTR bstrString);

	STDMETHOD(ActivateWindow)(VARIANT_BOOL bForce);
	STDMETHOD(ActivateTaskList)(VARIANT_BOOL bForce);
	STDMETHOD(ClearWindow)();
	STDMETHOD(Close)();

	STDMETHOD(get_Warnings)( long *pnErr );
	STDMETHOD(get_Errors)( long *pnWrn );
	STDMETHOD(get_BaseDirectory)(BSTR* pbstrBaseDir);
	STDMETHOD(put_BaseDirectory)(BSTR bstrBaseDir);
	STDMETHOD(get_ProjectName)(BSTR* pbstrBaseDir);
	STDMETHOD(put_ProjectName)(BSTR bstrBaseDir);

	STDMETHOD(get_AssociatedBuildEngine)(IDispatch** ppBldEngine);
	STDMETHOD(put_AssociatedBuildEngine)(IDispatch* pBldEngine);
	STDMETHOD(get_ShowOutput)(VARIANT_BOOL* pbShow);
	STDMETHOD(put_ShowOutput)(VARIANT_BOOL bShow);

	HRESULT DetermineWindowActivation(VARIANT_BOOL bForce, VSHPROPID propid, BOOL& bActivate);

	CDirW m_dir;
	CComBSTR m_bstrName;
	int m_nErrors;
	int m_nWarnings;
	VARIANT_BOOL m_bShowOutput;
	CComQIPtr<IVCBuildEngine> m_spBuildEngine;
	CComQIPtr<IVCBuildOutput> m_spModelessOutputWindow;
};
