// BuildOptions.cpp : Implementation of CBuildOptions
#include "stdafx.h"
#include "OptionsDlg.h"
#include "bldpkg.h"
#include "profile.h"

/////////////////////////////////////////////////////////////////////////////
// CBuildOptions

LRESULT CBuildOptions::OnDestroyDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_pGrid )
	{
	    m_pGrid->SetSelectedObjects( 0, NULL );
	    m_pGrid = NULL;
	}
	return 1;  // Let the system set the focus
}

LRESULT CBuildOptions::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	CComPtr<IVSMDPropertyBrowser> pBrowser;
	HRESULT hr = ::ExternalQueryService(SID_SVSMDPropertyBrowser, __uuidof(IVSMDPropertyBrowser), (LPVOID *)&pBrowser);
	RETURN_ON_FAIL2(hr, 1);

	pBrowser->CreatePropertyGrid( &m_pGrid );

	if( hr == S_OK && m_pGrid )
	{

		HWND hwndGrid;
		HWND hwndGridPos;
		RECT rectGridPos;
		RECT rectGrid;
		hwndGridPos = ::GetDlgItem(m_hWnd, IDC_TOOLSOPTSGRIDPOS );
		::GetWindowRect(hwndGridPos, &rectGridPos);

		LONG res = ::GetWindowLong( hwndGridPos, GWL_STYLE );
		::SetWindowLong( hwndGridPos, GWL_STYLE, res ^ WS_VISIBLE );
		
		rectGrid.top = 0;
		rectGrid.left = 0;
		rectGrid.bottom = rectGridPos.bottom - rectGridPos.top;
		rectGrid.right = rectGridPos.right - rectGridPos.left;
		
		m_pGrid->get_Handle(&hwndGrid);
		CWindow wndGrid;
		wndGrid.Attach(hwndGrid);
		wndGrid.SetParent(m_hWnd);
        
	        VARIANT vt;
        	VariantInit(&vt);
	        V_VT(&vt) = VT_BOOL;
        	V_BOOL(&vt) = VARIANT_FALSE;

	        m_pGrid->SetOption(PGOPT_TOOLBAR, vt);
        	m_pGrid->put_GridSort(PGSORT_ALPHABETICAL);
		wndGrid.SetWindowPos( HWND_TOP, &rectGrid, SWP_FRAMECHANGED );
		
		IUnknown* pTemp = m_spOptions;
		m_pGrid->SetSelectedObjects( 1, &pTemp );
	}
	
	return 1;  // Let the system set the focus
}

LRESULT CBuildOptions::OnHelp(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	CComPtr<Help> pHlp;
	CComBSTR bstrTopic;
	bstrTopic = L"vc.proj.properties";

	HRESULT hr = ExternalQueryService(SID_SVsHelp, __uuidof(Help), (void **)&pHlp);
	if( pHlp )
		hr = pHlp->DisplayTopicFromF1Keyword(bstrTopic);
	return hr;
}

LRESULT CBuildOptions::OnApply(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	// let the project engine know about the changes
	VSASSERT(m_spOptions, "Don't have a property container to feed build option data back to!");

	CStringW strRoot  = CBuildPackage::s_bstrAltKey + "\\VC";
	VCProjectEngine *pProjEng = GetBuildPkg()->GetProjectEngine();

	VARIANT_BOOL buildLogging;
	m_spOptions->get_BuildLogging(&buildLogging);
	pProjEng->put_BuildLogging(buildLogging);
	WriteRegBoolW( strRoot, L"Build Logging", buildLogging );

	VARIANT_BOOL buildTiming;
	m_spOptions->get_BuildTiming(&buildTiming);
	pProjEng->put_BuildTiming(buildTiming);
	WriteRegBoolW( strRoot, L"Build Timing", buildTiming );

	// performance logging
	VARIANT_BOOL performanceLogging;
 	pProjEng->get_PerformanceLogging( &performanceLogging );	
	WriteRegBoolW( strRoot, L"Performance Logging", performanceLogging );

	CComBSTR bstrExt;
	m_spOptions->get_CPPExtensions(&bstrExt);
	// REVIEW: Multi platform broken !
	strRoot += L"\\VC_OBJECTS_PLATFORM_INFO\\Win32\\ToolDefaultExtensionLists\\";
	WriteRegStringW( strRoot, L"VCCLCompilerTool", bstrExt );
	return 0;
}

STDMETHODIMP CVCBuildOptionsPage::get_BuildLogging(VARIANT_BOOL *pbLog)
{
	CHECK_POINTER_NULL(pbLog);
	*pbLog = m_buildLogging;
	return S_OK;
}

STDMETHODIMP CVCBuildOptionsPage::put_BuildLogging(VARIANT_BOOL bLog)
{
	m_buildLogging = bLog;
	return S_OK;
}

STDMETHODIMP CVCBuildOptionsPage::get_BuildTiming(VARIANT_BOOL *pbTime)
{
	CHECK_POINTER_NULL(pbTime);
	*pbTime = m_buildTiming;
	return S_OK;
}

STDMETHODIMP CVCBuildOptionsPage::put_BuildTiming(VARIANT_BOOL bTime)
{
	m_buildTiming = bTime;
	return S_OK;
}

STDMETHODIMP CVCBuildOptionsPage::get_CPPExtensions(BSTR *pbstrExt)
{
	CHECK_POINTER_NULL(pbstrExt);
	return m_bstrExt.CopyTo(pbstrExt);
}

STDMETHODIMP CVCBuildOptionsPage::put_CPPExtensions(BSTR bstrExt)
{
    m_bstrExt = bstrExt;
	return S_OK;
}


STDMETHODIMP CVCBuildOptionsObject::get_BuildLogging(VARIANT_BOOL *pbLog)
{
	return g_pBuildPackage->m_pProjectEngine->get_BuildLogging(pbLog);
}

STDMETHODIMP CVCBuildOptionsObject::put_BuildLogging(VARIANT_BOOL bLog)
{
	return g_pBuildPackage->m_pProjectEngine->put_BuildLogging(bLog);
}

STDMETHODIMP CVCBuildOptionsObject::get_BuildTiming(VARIANT_BOOL *pbTime)
{
	return g_pBuildPackage->m_pProjectEngine->get_BuildTiming(pbTime);
}

STDMETHODIMP CVCBuildOptionsObject::put_BuildTiming(VARIANT_BOOL bTime)
{
	return g_pBuildPackage->m_pProjectEngine->put_BuildTiming(bTime);
}

STDMETHODIMP CVCBuildOptionsObject::get_CPPExtensions(BSTR *pbstrExt)
{
	CHECK_POINTER_VALID(pbstrExt);
	// REVIEW (CVCBuildOptionsObject::get_CPPExtensions): Multi platform broken!
	CStringW strRoot = CBuildPackage::s_bstrAltKey;
	strRoot += L"\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\ToolDefaultExtensionLists";
	return GetRegStringW(strRoot, L"VCCLCompilerTool", pbstrExt);
}

STDMETHODIMP CVCBuildOptionsObject::put_CPPExtensions(BSTR bstrExt)
{
	// REVIEW (CVCBuildOptionsObject::put_CPPExtensions): Multi platform broken!
	CStringW strRoot = CBuildPackage::s_bstrAltKey;
	strRoot += L"\\VC_OBJECTS_PLATFORM_INFO\\Win32\\ToolDefaultExtensionLists\\";
	WriteRegStringW( strRoot, L"VCCLCompilerTool", bstrExt );
	return S_OK;
}

