#pragma once

#include "PerPropBrowsing.h"
#include <vccolls.h>
#include <vcmap.h>
#include "settingspage.h"

/////////////////////////////////////////////////////////////////////////////
// CVCBuildOptionsPage
class ATL_NO_VTABLE CVCBuildOptionsPage :
	public IDispatchImpl<IVCBuildOptionsPage, &IID_IVCBuildOptionsPage, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCBuildOptionsPage, BUILDOPTIONS_MIN_DISPID, BUILDOPTIONS_MAX_DISPID>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CVCBuildOptionsPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCBuildOptionsPage)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCBuildOptionsPage, &IID_IVCBuildOptionsPage, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCBuildOptionsPage
public:
	STDMETHOD(get_BuildLogging)(VARIANT_BOOL *pbLog);
	STDMETHOD(put_BuildLogging)(VARIANT_BOOL bLog);
	STDMETHOD(get_BuildTiming)(VARIANT_BOOL *pbTime);
	STDMETHOD(put_BuildTiming)(VARIANT_BOOL bTime);
};

/////////////////////////////////////////////////////////////////////////////
// CVCBuildOptionsObject - use this for automation, but otherwise matches CVCBuildOptionsPage class above
class ATL_NO_VTABLE CVCBuildOptionsObject :
	public IDispatchImpl<IVCBuildOptionsPage, &IID_IVCBuildOptionsPage, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CVCBuildOptionsObject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCBuildOptionsPage)
END_COM_MAP()

// IVCBuildOptionsPage
public:
	STDMETHOD(get_BuildLogging)(VARIANT_BOOL *pbLog);
	STDMETHOD(put_BuildLogging)(VARIANT_BOOL bLog);
	STDMETHOD(get_BuildTiming)(VARIANT_BOOL *pbTime);
	STDMETHOD(put_BuildTiming)(VARIANT_BOOL bTime);
};

