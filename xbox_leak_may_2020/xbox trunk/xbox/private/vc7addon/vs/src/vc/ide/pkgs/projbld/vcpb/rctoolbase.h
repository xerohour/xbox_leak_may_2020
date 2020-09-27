// VCResourceCompilerBaseTool.h: Definition of the CVCResourceCompilerBaseTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

/////////////////////////////////////////////////////////////////////////////
// CVCResourceCompilerPage

class ATL_NO_VTABLE CVCResourceCompilerPage :
	public IDispatchImpl<IVCResourceCompilerPage, &IID_IVCResourceCompilerPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCResourceCompilerPage,VCRCTOOL_MIN_DISPID,VCRCTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CVCResourceCompilerPage)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCResourceCompilerPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCResourceCompilerPage, &IID_IVCResourceCompilerPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IPerPropertyBrowsing methods
public:
	STDMETHOD(GetDisplayString)( DISPID dispID, BSTR *pBstr );

// IVCResourceCompilerPage
public:
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_PreprocessorDefinitions)(BSTR* pbstrDefines);
	STDMETHOD(put_PreprocessorDefinitions)(BSTR bstrDefines);
	STDMETHOD(get_Culture)(enumResourceLangID* pnLangID);
	STDMETHOD(put_Culture)(enumResourceLangID nLangID);
	STDMETHOD(get_AdditionalIncludeDirectories)(BSTR* pbstrIncludePath);
	STDMETHOD(put_AdditionalIncludeDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_IgnoreStandardIncludePath)(enumIgnoreStandardIncludePathBOOL* pbIgnoreInclPath);
	STDMETHOD(put_IgnoreStandardIncludePath)(enumIgnoreStandardIncludePathBOOL bIgnoreInclPath);
	STDMETHOD(get_ShowProgress)(enumShowProgressBOOL* pbShowProgress);
	STDMETHOD(put_ShowProgress)(enumShowProgressBOOL bShowProgress);
	STDMETHOD(get_ResourceOutputFileName)(BSTR* pbstrResFile);
	STDMETHOD(put_ResourceOutputFileName)(BSTR bstrResFile);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual void GetLocalizedName(DISPID dispID, MEMBERID memid, ITypeInfo2* pTypeInfo2Enum, long lVal, CComBSTR& bstrDoc);
	virtual BOOL UseDirectoryPickerDialog(long id) { return (id == VCRCID_AdditionalIncludeDirectories); }
};
