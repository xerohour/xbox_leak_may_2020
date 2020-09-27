// BuildOptions.h : Declaration of the CBuildOptions

#ifndef __BUILDOPTIONS_H_
#define __BUILDOPTIONS_H_

#include "rsrc.h"       // main symbols
#undef WM_OCC_LOADFROMSTREAM
#undef WM_OCC_LOADFROMSTORAGE         
#undef WM_OCC_INITNEW                 
#undef WM_OCC_LOADFROMSTREAM_EX       
#undef WM_OCC_LOADFROMSTORAGE_EX      

#include <atlhost.h>
#include "bldpkg.h"
#include "profile.h"

/////////////////////////////////////////////////////////////////////////////
// CVCBuildOptionsPage
class ATL_NO_VTABLE CVCBuildOptionsPage :
	public IDispatchImpl<IVCBuildOptionsPage, &IID_IVCBuildOptionsPage, &LIBID_VCProjectLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public IVsPerPropertyBrowsing,
	public IPerPropertyBrowsing,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CVCBuildOptionsPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCBuildOptionsPage)
END_COM_MAP()

	static HRESULT CreateInstance( IVCBuildOptionsPage **ppI )
	{
		HRESULT hr;
		CComObject<CVCBuildOptionsPage> *pObj;
		hr = CComObject<CVCBuildOptionsPage>::CreateInstance(&pObj);
		if (SUCCEEDED(hr))
		{
			pObj->QueryInterface(__uuidof(IVCBuildOptionsPage), (void**)ppI);
		}

		VARIANT_BOOL buildLogging;
		VARIANT_BOOL buildTiming;
		CComBSTR bstrExt;

		g_pBuildPackage->m_pProjectEngine->get_BuildLogging(&buildLogging);
		g_pBuildPackage->m_pProjectEngine->get_BuildTiming(&buildTiming);
		
		CStringW strRoot = CBuildPackage::s_bstrAltKey;
		strRoot += L"\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\ToolDefaultExtensionLists";
		hr = GetRegStringW(strRoot, L"VCCLCompilerTool", &bstrExt);

		(*ppI)->put_BuildTiming(buildTiming);
		(*ppI)->put_BuildLogging(buildLogging);
		(*ppI)->put_CPPExtensions(bstrExt);

		return hr;
	}
	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCBuildOptionsPage, &IID_IVCBuildOptionsPage, &LIBID_VCProjectLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCBuildOptionsPage
public:
	STDMETHOD(get_BuildLogging)(VARIANT_BOOL *pbLog);
	STDMETHOD(put_BuildLogging)(VARIANT_BOOL bLog);
	STDMETHOD(get_BuildTiming)(VARIANT_BOOL *pbTime);
	STDMETHOD(put_BuildTiming)(VARIANT_BOOL bTime);
	STDMETHOD(get_CPPExtensions)(BSTR *pbstrExt);
	STDMETHOD(put_CPPExtensions)(BSTR bstrExt);

// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)( DISPID dispid, BOOL *pfHide)
	{
		CHECK_POINTER_NULL(pfHide);
		*pfHide = FALSE;
		return E_NOTIMPL;
	};

	STDMETHOD(DisplayChildProperties)( DISPID dispid, BOOL *pfDisplay)
	{
		CHECK_POINTER_NULL(pfDisplay);
		*pfDisplay = FALSE;
		return S_OK;
	}

	STDMETHOD(GetLocalizedPropertyInfo)( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc)
	{

		CComQIPtr<IDispatch> pdisp = static_cast<IDispatch*>(this);
		RETURN_ON_NULL(pdisp);

		CComPtr<ITypeInfo> pTypeInfo;
		HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
		CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
		RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo2);

		CComBSTR bstrDoc;
		hr = pTypeInfo2->GetDocumentation2(dispid, localeID, &bstrDoc, NULL, NULL);
		RETURN_ON_FAIL_OR_NULL(hr, bstrDoc.m_str);
		
		LPOLESTR pDesc = wcsstr( bstrDoc, L": " );
		RETURN_ON_NULL(pDesc);

		int nSize = (int)(pDesc - bstrDoc);
		pDesc+=2;

		CComBSTR bstrName( nSize-1, bstrDoc );
		if (pbstrName != NULL)
			*pbstrName = bstrName.Detach();

		CComBSTR bstrDesc( pDesc );
		if (pbstrDesc != NULL)
			*pbstrDesc = bstrDesc.Detach();
		
		return S_OK;
	}

	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault)
	{
		*pfDefault = TRUE;
		return S_OK;
	}
	
	STDMETHOD(IsPropertyReadOnly)( DISPID dispid, BOOL *fReadOnly)
	{
		CHECK_POINTER_NULL(fReadOnly);
		*fReadOnly = FALSE;
		return S_OK;
	}

	STDMETHOD(GetClassName)(BSTR* ) {return E_NOTIMPL;}	
    STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* ) {return E_NOTIMPL;}	
    STDMETHOD(ResetPropertyValue)(DISPID ) {return E_NOTIMPL;}


public:
	// IPerPropertyBrowsing methods
	STDMETHOD(MapPropertyToPage)( DISPID dispID, CLSID *pClsid )
		{ return E_NOTIMPL; }
	
	STDMETHOD(GetDisplayString)( DISPID dispID, BSTR *pBstr )
	{
		CHECK_POINTER_NULL(pBstr)
		*pBstr = NULL;
				
		HRESULT hr;

		CComQIPtr<IDispatch> pdisp = static_cast<IDispatch*>(this);
		
		// what type is this?
		CComVariant var;
		hr = CComDispatchDriver::GetProperty( pdisp, dispID, &var );
		RETURN_ON_FAIL2(hr, S_FALSE);

		switch( var.vt )
		{
			case VT_BSTR:
			{
				// if it was a bstr, return it and let the default behavior happen
				CComBSTR bstrTemp = var.bstrVal;
				*pBstr = bstrTemp.Detach();
 				return S_FALSE;
			}
			case VT_BOOL:
			{
				if( var.boolVal == VARIANT_TRUE )
				{
					CComBSTR bstrTrue;
					bstrTrue.LoadString(VCTDENUM_Yes);
					*pBstr = bstrTrue.Detach();
				}
				else 
				{
					CComBSTR bstrFalse;
					bstrFalse.LoadString(VCTDENUM_No);
					*pBstr = bstrFalse.Detach();
				}
				return S_OK;
			}
			default:
			{
				// unhandled type, do default
				return S_FALSE;
			}
		}
	}
	
	STDMETHOD(GetPredefinedStrings)( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut )
	{
		CHECK_POINTER_NULL(pCaStringsOut)
		CHECK_POINTER_NULL(pCaCookiesOut)
		
		HRESULT hr;

		CComQIPtr<IDispatch> pdisp = static_cast<IDispatch*>(this);
		
		// what type is this?
		CComVariant var;
		hr = CComDispatchDriver::GetProperty( pdisp, dispID, &var );
		RETURN_ON_FAIL2(hr, S_FALSE);

		pCaCookiesOut->cElems = 0;
		pCaCookiesOut->pElems = NULL;
		pCaStringsOut->cElems = 0;
		pCaStringsOut->pElems = NULL;

		switch( var.vt )
		{
			case VT_BOOL:
			{
				CComBSTR bstrTrue;
				bstrTrue.LoadString(VCTDENUM_Yes);
				CComBSTR bstrFalse;
				bstrFalse.LoadString(VCTDENUM_No);
				int nElems = 2;

				pCaCookiesOut->cElems = nElems;
				pCaCookiesOut->pElems = (DWORD*)CoTaskMemAlloc( nElems * sizeof(DWORD) );
				pCaCookiesOut->pElems[0] = 0;
				pCaCookiesOut->pElems[1] = 1;
				
				pCaStringsOut->cElems = nElems;
				pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( nElems * sizeof(LPOLESTR) );
				pCaStringsOut->pElems[0] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrFalse ) + 1) * sizeof(OLECHAR) );
				wcscpy( pCaStringsOut->pElems[0], bstrFalse );
				pCaStringsOut->pElems[1] = (LPOLESTR)CoTaskMemAlloc( (SysStringLen( bstrTrue ) + 1) * sizeof(OLECHAR) );
				wcscpy( pCaStringsOut->pElems[1], bstrTrue );

				return S_OK;
			}
			default:
			{
				// unhandled type, do default
				return S_FALSE;
			}
		}
		return E_FAIL;
	}

	STDMETHOD(GetPredefinedValue)( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut )
	{
		CHECK_POINTER_NULL(pVarOut)
		
		HRESULT hr;
		CComQIPtr<IDispatch> pdisp = static_cast<IDispatch*>(this);
		
		// what type is this?
		CComVariant var;
		hr = CComDispatchDriver::GetProperty( pdisp, dispID, &var );
		RETURN_ON_FAIL2(hr, S_FALSE);
		
		CComVariant varOut;
		
		switch( var.vt )
		{
		// boolean
		case VT_BOOL:
		{
			if( dwCookie == 0 )
			{
				// varOut.boolVal = VARIANT_FALSE;
				CComBSTR bstrFalse(L"False");
				varOut = bstrFalse;
			}
			else if( dwCookie == 1 )
			{
				CComBSTR bstrTrue(L"True");
				varOut = bstrTrue;
				// varOut.boolVal = VARIANT_TRUE;
				// varOut = L"True";
			}
			break;
		}
		default:
			// unhandled type, do default
			return S_FALSE;
		}
		varOut.Detach(pVarOut);
		return S_OK;
	}

	VARIANT_BOOL m_buildLogging;
	VARIANT_BOOL m_buildTiming;
	CComBSTR m_bstrExt;
};

/////////////////////////////////////////////////////////////////////////////
// CBuildOptions
class CBuildOptions : 
	public CAxDialogImpl<CBuildOptions>
{
public:
	CBuildOptions()
	{
		CVCBuildOptionsPage::CreateInstance( &m_spOptions );
	}

	~CBuildOptions()
	{
	}

	enum { IDD = IDDP_OPTIONS_BUILD };

BEGIN_MSG_MAP(CBuildOptions)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroyDialog)
  	NOTIFY_CODE_HANDLER(PSN_APPLY,OnApply)	
  	NOTIFY_CODE_HANDLER(PSN_HELP,OnHelp)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroyDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnApply(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnHelp(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

protected:
	CComPtr<IVSMDPropertyGrid> m_pGrid;
	CComQIPtr<IVCBuildOptionsPage> m_spOptions;
};


/////////////////////////////////////////////////////////////////////////////
// CVCBuildOptionsObject - use this for automation, but otherwise matches CVCBuildOptionsPage class above
class ATL_NO_VTABLE CVCBuildOptionsObject :
	public IDispatchImpl<IVCBuildOptionsPage, &IID_IVCBuildOptionsPage, &LIBID_VCProjectLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CVCBuildOptionsObject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCBuildOptionsPage)
END_COM_MAP()
        static HRESULT CreateInstance( IDispatch **ppDisp )
        {
                HRESULT hr;
                CComObject<CVCBuildOptionsObject> *pObj;
                hr = CComObject<CVCBuildOptionsObject>::CreateInstance(&pObj);
                if (SUCCEEDED(hr))
                {
                        pObj->QueryInterface(__uuidof(IDispatch), (void**)ppDisp);
                }
                return hr;
        }

// IVCBuildOptionsPage
public:
	STDMETHOD(get_BuildLogging)(VARIANT_BOOL *pbLog);
	STDMETHOD(put_BuildLogging)(VARIANT_BOOL bLog);
	STDMETHOD(get_BuildTiming)(VARIANT_BOOL *pbTime);
	STDMETHOD(put_BuildTiming)(VARIANT_BOOL bTime);
	STDMETHOD(get_CPPExtensions)(BSTR *pbstrExt);
	STDMETHOD(put_CPPExtensions)(BSTR bstrExt);
};

#endif //__BUILDOPTIONS_H_
