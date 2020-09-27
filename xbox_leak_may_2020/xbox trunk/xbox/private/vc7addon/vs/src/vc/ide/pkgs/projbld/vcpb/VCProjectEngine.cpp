// VCProjectEngine.cpp : Implementation of CVCProjectEngine
#include "stdafx.h"
#include "VCProjectEngine.h"
#include "collection.h"

#include "project.h"
#include "regscan.h"
#include "scriptexecutor.h"
#include "buildengine.h"
#include "msgboxes2.h"
#include "profile.h"
#include "StyleSheet.h"
#include "register.h"
#include "VCNodeFactory.h"
#include "xmlfile.h"
#include "ConfigRecords.h"
#include "vsappid.h"
#include "MRDependencies.h"
#include "platform.h"

#include <fcntl.h>
#include <iads.h>

// Shell interface
interface CommandBars;
#include "dte.h"
#include <uilocale.h>
#include "helpsys.h"

// REVIEW: REMOVE THESE WHEN WE GET BETTER LINKAGE BETWEEN DLL'S
const IID 	IID_IVCProjConvert = {0x7D8AE8C4,0x3FD3,0x11D3,{0x8D,0x52,0x00,0xC0,0x4F,0x8E,0xCD,0xB6}};
const IID 	LIBID_VCPROJCNVTLib = {0xD43DAED2,0x3FCF,0x11D3,{0x8D,0x52,0x00,0xC0,0x4F,0x8E,0xCD,0xB6}};
const CLSID CLSID_VCProjConvert = {0x7D8AE8C5,0x3FD3,0x11D3,{0x8D,0x52,0x00,0xC0,0x4F,0x8E,0xCD,0xB6}};

#define LOCALHOST       L"LocalHost"  
#define LOCALHOST_CCH   9     // count of chars in LOCALHOST

// this symbol only for BBT, don't remove.
int ENDBOOT_i = 0;
extern "C" void ENDBOOT_vcpb(void) { ENDBOOT_i++; return; }

class CInLoadStyle
{
public:
	CInLoadStyle() { m_bOldInLoad = g_bInStyleLoad; g_bInStyleLoad = TRUE; }
	~CInLoadStyle() { g_bInStyleLoad = m_bOldInLoad; }
	BOOL m_bOldInLoad;
};

/////////////////////////////////////////////////////////////////////////////
// CVCProjectEngine

VARIANT_BOOL 			 CVCProjectEngine::s_bBuildLogging = VARIANT_TRUE;	// Do we log builds to a file?
IVCBuildPackageInternal* CVCProjectEngine::s_pBuildPackage = NULL;
CComPtr<IServiceProvider> CVCProjectEngine::s_pServiceProvider = NULL;
HANDLE 					 CVCProjectEngine::s_hLogFile = NULL;
CComBSTR                 CVCProjectEngine::s_bstrAlternateRegistryRoot = L"";
BOOL                     CVCProjectEngine::s_fInitializedCom = FALSE;
CComPtr<IMultiLanguage2> CVCProjectEngine::s_spMultiLanguage2 = NULL;
CComPtr<IMultiLanguage> CVCProjectEngine::s_spMultiLanguage = NULL;
CComBSTR				CVCProjectEngine::s_bstrIISRoot = L"";
CComPtr<IMSAdminBase>	CVCProjectEngine::s_spAdminBase = NULL;
CAdsiWrapper*			CVCProjectEngine::s_pAdsiWrapper = NULL;

/////////////////////////////////////////////////////////////////////////////
// CVCProjectEngine
HRESULT CVCProjectEngine::CreateInstance(CVCProjectEngine **ppDispInfo)
{
	HRESULT hr;
	CVCProjectEngine *pVar;
	CComObject<CVCProjectEngine> *pObj;
	hr = CComObject<CVCProjectEngine>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppDispInfo = pVar;
		hr = pVar->Initialize(NULL, NULL, NULL, NULL);
	}
	return hr;
}


CVCProjectEngine::CVCProjectEngine()
{
	m_bInitialized = false;
	m_bSoftInitialized = false;
	VSASSERT(g_pProjectEngine == NULL, "Why did we create the project engine twice?");
	g_pProjectEngine = this;
	m_nNextVendorID = VCVENDORTOOL_MIN_DISPID;
	m_dwCookieComposite = 0;
	m_bPerformanceLogging = VARIANT_FALSE;
	m_bSuppressUI = VARIANT_FALSE;
	m_nSKU = -1;
	s_fInitializedCom = FALSE;
	CVCProjectEngineEvents::CreateInstance(&m_pProjectEngineEvents);
}

/*
  Initialize
  Does any and all initialization
   This inludes:
		Registration into the Running Object Table (ROT)
		Start up the property manager and initialize it
		Create global property bags such as environment, include, librarypaths, executablepaths, metaplatform
*/
STDMETHODIMP CVCProjectEngine::Initialize( LONG_PTR hInstResources, IUnknown *pUnk, IVSMDPropertyBrowser *pBrowser, 
	IVCBuildPackageInternal* pBldPkg)
{
	if (m_bInitialized)
		return S_OK;

	m_bInitialized = true;
	return DoInitialize(hInstResources, pUnk, pBrowser, pBldPkg, TRUE);
}

HRESULT CVCProjectEngine::DoInitialize(LONG_PTR hInstResources, IUnknown *pUnk, IVSMDPropertyBrowser *pBrowser, 
	IVCBuildPackageInternal* pBldPkg, BOOL bForceInit)
{
	if (!bForceInit && m_bSoftInitialized)
		return S_OK;

	CComQIPtr<IServiceProvider> pSP = pUnk;
	s_pServiceProvider = pSP;

	bool bInitOnce = m_bSoftInitialized;
	m_bSoftInitialized = true;

	// if we aren't given a module hinst for resources, use the default
	if( !hInstResources )
	{
		// get the system default locale id
		LCID lcid = GetSystemDefaultLCID();
		// create a directory name from it
		wchar_t szDirName[8] = {0};
		_itow( lcid, szDirName, 10 );
		// look in that directory for VCProjectUI.dll
		CStringW strDllName;
		if (UtilGetModuleFileNameW(_Module.GetModuleInstance(), strDllName))
		{
			int index = strDllName.ReverseFind( L'\\' );
			strDllName = strDllName.Left( index+1 );
			strDllName += szDirName;
			strDllName += L"\\VCProjectUI.dll" ;
			HINSTANCE hInst = LoadLibraryW( strDllName );
			if (hInst)
				_Module.SetResourceInstance( hInst );
		}
	}
	else
		_Module.SetResourceInstance( (HINSTANCE)hInstResources );

	s_pBuildPackage = pBldPkg;

	if (pBldPkg == NULL)	// no UI if no back door to here...
	{
		put_SuppressUI(VARIANT_TRUE);
		InitializeForCommandLineUsage();
	}

	if( pSP )
	{
		CComPtr<IVsShell> pShell;
		pSP->QueryService(SID_SVsShell, __uuidof(IVsShell), (void**)&pShell);
		if(pShell)
		{
			HRESULT hr;
			CComVariant var;
			hr = pShell->GetProperty(VSSPROPID_VirtualRegistryRoot, &var);
			if(SUCCEEDED(hr) && var.vt == VT_BSTR)
			{
				s_bstrAlternateRegistryRoot = var.bstrVal;
				CStringA strAltKey = var.bstrVal;		// YES, ansi
				SetAlternateKey(strAltKey);
				VsLoaderSetRegistryRootW( s_bstrAlternateRegistryRoot );
			}
		}
	}
	else
	{
		s_bstrAlternateRegistryRoot = L"Software\\Microsoft\\VisualStudio\\7.0";
	}

	HRESULT hr;

	// create the platforms from the registry
	// (platforms create their own tools)
	hr = GetPlatforms();
	VSASSERT(SUCCEEDED(hr), "No Supported platforms found in the registry.  Rerun vsreg.");

	// read tools->options.project.vc build data from registry

	// build logging
	CStringW strKey = s_bstrAlternateRegistryRoot + L"\\VC";

	VARIANT_BOOL bVal = VARIANT_TRUE;
	hr = GetRegBoolW( strKey, L"Build Logging", &bVal);
	SetBoolProperty(PROJENGID_BuildLogging, bVal);

	// build timing
	bVal = VARIANT_FALSE;
	hr = GetRegBoolW( strKey, L"Build Timing", &bVal);
 	SetBoolProperty(PROJENGID_BuildTiming, bVal);

	// performance logging
	bVal = VARIANT_FALSE;
	hr = GetRegBoolW( strKey, L"Performance Logging", &bVal);
 	m_bPerformanceLogging =  bVal;

	// this call only for BBT, don't remove.
	ENDBOOT_vcpb();
	return S_OK;
}

/* static */
void CVCProjectEngine::GetBasePlatformInfoKey( BSTR *pbstrKey )
{
	CComBSTR bstrKey;
	bstrKey = s_bstrAlternateRegistryRoot;
	bstrKey.Append(szVCPlatformRegSubKey);
	*pbstrKey = bstrKey.Detach();
	return;
}

/* static */
void CVCProjectEngine::GetPlatformInfoKey(BSTR bstrPlatform, BSTR bstrMainKey, BSTR *pRetVal )
{
	CComBSTR bstrSubKey;
	bstrSubKey = bstrMainKey;
	bstrSubKey.Append(L"\\");
	bstrSubKey.Append(bstrPlatform);
	*pRetVal = bstrSubKey.Detach();
}

HRESULT CVCProjectEngine::GetPlatforms()
{
    	HRESULT hr;
	// just in case we've been here before...
	ClearPlatforms();

	if( UsingRegistry() )
	{
	    /*
	    Registry looks something like this:
	    HKEY_LOCAL_MACHINE\ ... \VC_OBJECTS_PLATFORM_INFO
		    Win32 (with default prop = guid)
			    directories
				    <inc, exe, lib directories>
			    vendor tool short name (with default prop = guid)
				    <any excluded configs>
	    */

	    CRegistryScanner RegScanner;
	    CComBSTR bstrMainKey;
	    CComBSTR bstrPlatformName;
	    GetBasePlatformInfoKey(&bstrMainKey);
	    if( !RegScanner.Open(HKEY_LOCAL_MACHINE, bstrMainKey) )
		    return S_OK;	// no key, nothing to do
    
	    hr = VCPROJ_E_NOT_FOUND;
	    while( RegScanner.EnumKey(&bstrPlatformName) )
	    {
			// for each item, create a new platform INSTANCE
			if (bstrPlatformName.Length() == 0)
				continue;	// nothing to do if platform name is empty
			// read the default value for the platform key as a guid,
			// and CoCreate that guid to get the platform
			CComBSTR bstrVal;
			if( !RegScanner.GetValue( L"\0", &bstrVal ) )
				continue;

			// turn the string into a guid
			CLSID clsidPlatform;
			hr = CLSIDFromString( bstrVal, &clsidPlatform );
			VSASSERT( SUCCEEDED( hr ), "Failed to get CLSID of platform. Make sure platforms are registered correctly" );
			if( FAILED( hr ) )
				continue;
	
			// CoCreate it
	// 		hr = pPlatform.CoCreateInstance( clsidPlatform );
			CComPtr<VCPlatform> pPlatform;
			hr = VsLoaderCoCreateInstance<VCPlatform>( clsidPlatform, &pPlatform );
			VSASSERT( SUCCEEDED( hr ), "Failed to CoCreate platform. Make sure platforms are registered correctly" );
			if( FAILED( hr ) )
				continue;
	
			CComQIPtr<IVCPlatformImpl> pPlatformImpl = pPlatform;
			VSASSERT(SUCCEEDED(hr) && pPlatformImpl && pPlatform, "Failed to create platform object for platform name.");
			if (!pPlatform || !pPlatformImpl)
				continue;	// nothing to do if no platform
			// initialize the platform
			pPlatformImpl->Initialize();
			m_rgPlatforms.Add(pPlatform);
		}
	}
	else
	{
		CComPtr<VCPlatform> pPlatform;
		hr = CPlatformWin32::CreateInstance( &pPlatform );
		if( FAILED( hr ) )
			return hr;
		CComQIPtr<IVCPlatformImpl> pPlatformImpl = pPlatform;
		if( pPlatformImpl )
		{
			hr = pPlatformImpl->Initialize();
			m_rgPlatforms.Add(pPlatform);
		}
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_NO_PLATFORMS);

	return hr;
}


CVCProjectEngine::~CVCProjectEngine()
{
	// need to Close() for standalone (non-VS shell) clients
	// this should be ok, even if we've already been closed
	Close();
	if (s_pAdsiWrapper != NULL)
		delete s_pAdsiWrapper;

	g_pProjectEngine = NULL; // don't kill the fat lady till she doesn't sing anymore
}

STDMETHODIMP CVCProjectEngine::Close()
{
	long l, lcItems;

	lcItems = m_rgProjects.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<IVCProjectImpl> pProjectImpl = m_rgProjects.GetAt(l);
		if (pProjectImpl)
			pProjectImpl->Close(); // close down entirely; release all
	}

	lcItems = m_rgAllStyleSheets.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<IVCStyleSheetImpl> pStyleSheetImpl = m_rgAllStyleSheets.GetAt(l);
		if (pStyleSheetImpl)
			pStyleSheetImpl->Close(); // close down entirely; release all
	}

	ClearPlatforms();

	lcItems = m_rgVendorTools.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<IVCVendorToolFactory> pToolFactory = m_rgVendorTools.GetAt(l);
		if (pToolFactory)
			pToolFactory->CloseSession();
	}
	m_rgVendorTools.RemoveAll();

	m_pPropBrowser = NULL;
	s_pBuildPackage = NULL;
	m_rgProjects.RemoveAll(); // collection of projects. 
	m_rgStyleSheets.RemoveAll(); // collection of visible style sheets. 
	m_rgAllStyleSheets.RemoveAll();	// collection of *all* style sheets

	g_StaticBuildEngine.Close();
	s_pServiceProvider.Release();

	m_pProjConvert.Release();

	CBldMreDependenciesContainer::UnloadMSPDB();

	if (s_spMultiLanguage2)
		s_spMultiLanguage2.Release();
	if (s_spMultiLanguage)
		s_spMultiLanguage.Release();

	if (s_fInitializedCom)
		CoUninitialize();

	FinishClose();

	return S_OK;
}

void CVCProjectEngine::ClearPlatforms()
{
	long lcItems = m_rgPlatforms.GetSize();
	for (long l = 0; l < lcItems; l++)
	{
		CComQIPtr<IVCPlatformImpl> pPlatformImpl = m_rgPlatforms.GetAt(l);
		if (pPlatformImpl)
			pPlatformImpl->Close(); // close down entirely; release all
	}
	m_rgPlatforms.RemoveAll();
}

STDMETHODIMP CVCProjectEngine::get_BuildEngine(IDispatch **ppBuildEngine)
{
	CHECK_POINTER_NULL(ppBuildEngine);

	CComPtr<IVCBuildEngine> spBuildEngine;
	HRESULT hr = CDynamicBuildEngine::CreateInstance(&spBuildEngine);
	RETURN_ON_FAIL(hr);

	CComQIPtr<IDispatch> spDispBuildEngine = spBuildEngine;
	*ppBuildEngine = spDispBuildEngine.Detach();
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireItemAdded(IDispatch *pDispObj, IDispatch *pDispParent)
{
	Fire_OnItemAdded(pDispObj, pDispParent);
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireItemAdded(pDispObj, pDispParent);
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireItemRemoved(IDispatch *pDispObj, IDispatch *pDispParent)
{
	Fire_OnItemRemoved(pDispObj, pDispParent);
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireItemRemoved(pDispObj, pDispParent);
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireItemRenamed(IDispatch *pDispObj, IDispatch *pDispParent, BSTR bstrOldName)
{
	Fire_OnItemRenamed(pDispObj, pDispParent, bstrOldName);
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireItemRenamed(pDispObj, pDispParent, bstrOldName);
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireItemMoved(IDispatch* pItem, IDispatch* pNewParent, IDispatch* pOldParent)
{
	Fire_OnItemMoved(pItem, pNewParent, pOldParent);
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireItemMoved(pItem, pNewParent, pOldParent);
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireItemPropertyChange(IDispatch *pDispObj, long lDispid)
{
	Fire_OnItemPropertyChange(pDispObj, lDispid);
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireItemPropertyChange(pDispObj, lDispid);
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireSccEvent( IDispatch* pItem, enumSccEvent event, VARIANT_BOOL *pRet )
{
	Fire_OnSccEvent( pItem, event, pRet );
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireSccEvent( pItem, event, pRet );
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireReportError( BSTR bstrErrMsg, HRESULT hrReport, BSTR bstrHelpKeyword )
{
	Fire_OnReportError( bstrErrMsg, hrReport, bstrHelpKeyword );
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireReportError( bstrErrMsg, hrReport, bstrHelpKeyword );
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireProjectBuildStarted(IDispatch *pCfg)
{
	Fire_OnProjectBuildStarted( pCfg );
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireProjectBuildStarted( pCfg );
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::DoFireProjectBuildFinished(IDispatch *pCfg, long warnings, long errors, VARIANT_BOOL bCancelled)
{
	Fire_OnProjectBuildFinished( pCfg, warnings, errors, bCancelled );
	CComQIPtr<IVCProjectEngineEventsImpl> pFire = m_pProjectEngineEvents;
	RETURN_ON_NULL2(pFire, S_OK);
	pFire->DoFireProjectBuildFinished( pCfg, warnings, errors, bCancelled );
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::get_BuildLogging(VARIANT_BOOL * pbLog)
{
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
	return GetBoolProperty(PROJENGID_BuildLogging, pbLog);
}

STDMETHODIMP CVCProjectEngine::put_BuildLogging(VARIANT_BOOL bLog)
{
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
	return SetBuildLogging(bLog, true);
}

HRESULT CVCProjectEngine::SetBuildLogging(VARIANT_BOOL bLog, bool bPersist)
{
	HRESULT hr = SetBoolProperty(PROJENGID_BuildLogging, bLog);
	if (bPersist && SUCCEEDED(hr))
	{
		CStringW strRoot  = CVCProjectEngine::s_bstrAlternateRegistryRoot + "\\VC";
		WriteRegBoolW( strRoot, L"Build Logging", bLog );
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::get_SuppressUI(VARIANT_BOOL* pbSuppressUI)
{
	CHECK_POINTER_NULL(pbSuppressUI);
	*pbSuppressUI = m_bSuppressUI;			
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::put_SuppressUI(VARIANT_BOOL bSuppressUI)
{
	m_bSuppressUI = bSuppressUI;
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::get_BuildTiming(VARIANT_BOOL * pbTime)
{
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
	return GetBoolProperty(PROJENGID_BuildTiming, pbTime);
}

STDMETHODIMP CVCProjectEngine::put_BuildTiming(VARIANT_BOOL bTime)
{ 
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
	return SetBuildTiming(bTime, true);
}

HRESULT CVCProjectEngine::SetBuildTiming(VARIANT_BOOL bTime, bool bPersist)
{ 
	HRESULT hr = SetBoolProperty(PROJENGID_BuildTiming, bTime);
	if (bPersist && SUCCEEDED(hr))
	{
		CStringW strRoot  = CVCProjectEngine::s_bstrAlternateRegistryRoot + "\\VC";
		WriteRegBoolW( strRoot, L"Build Timing", bTime );
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::get_PerformanceLogging(VARIANT_BOOL *pbLog)
{
	CHECK_POINTER_VALID(pbLog);
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
	*pbLog = m_bPerformanceLogging;
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::put_PerformanceLogging(VARIANT_BOOL bLog)
{
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
	return SetPerformanceLogging(bLog, true);
}

HRESULT CVCProjectEngine::SetPerformanceLogging(VARIANT_BOOL bLog, bool bPersist)
{
	m_bPerformanceLogging = bLog;
	if (bPersist)
	{
		CStringW strRoot  = CVCProjectEngine::s_bstrAlternateRegistryRoot + "\\VC";
		WriteRegBoolW( strRoot, L"Performance Logging", m_bPerformanceLogging );
	}
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::get_Events(IDispatch * * ppEvents)
{
	CHECK_POINTER_VALID(ppEvents);
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
	RETURN_ON_NULL(m_pProjectEngineEvents);
	return m_pProjectEngineEvents->QueryInterface(__uuidof(IDispatch), (void**)ppEvents);
}

STDMETHODIMP CVCProjectEngine::get_Platforms(IDispatch * * ppPlatforms)
{
	CHECK_POINTER_VALID(ppPlatforms);
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCPlatform>::CreateInstance(&pCollection, &m_rgPlatforms);
	if (SUCCEEDED(hr))
	{
		*ppPlatforms = pCollection.Detach();
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::get_Projects(IDispatch * * ppProjects)
{
	CHECK_POINTER_VALID(ppProjects);
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCProject>::CreateInstance(&pCollection, &m_rgProjects);
	if (SUCCEEDED(hr))
	{
		*ppProjects = pCollection.Detach();
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::get_StyleSheets(IDispatch * * ppStyleSheets)
{
	CHECK_POINTER_VALID(ppStyleSheets);
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCStyleSheet>::CreateInstance(&pCollection, &m_rgStyleSheets);
	if (SUCCEEDED(hr))
	{
		*ppStyleSheets = pCollection.Detach();
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::get_AllStyleSheets(IDispatch * * ppStyleSheets)
{
	CHECK_POINTER_NULL(ppStyleSheets);
	DoInitialize(NULL, NULL, NULL, NULL, FALSE);	// just to make sure we have SOMETHING
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCStyleSheet>::CreateInstance(&pCollection, &m_rgAllStyleSheets);
	if (SUCCEEDED(hr))
	{
		*ppStyleSheets = pCollection.Detach();
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::CreateProject(BSTR szProject, IDispatch * * ppProject)
{
	CHECK_POINTER_VALID(ppProject);
	if (*szProject == L'\0')
		return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_EMPTY_PROJ_NAME);

	*ppProject = NULL; // just in case we fail

	Initialize(NULL, NULL, NULL, NULL); // just to be sure....

	CComPtr<VCProject> pProject;
	HRESULT hr = CProject::CreateInstance(&pProject);
	if (SUCCEEDED(hr) && pProject)
	{
		// firstly, we set the name
		hr = pProject->put_Name(szProject);
		RETURN_ON_FAIL(hr);

		// and get the dispatch to return
		CComQIPtr<IDispatch> pDispProject = pProject;
		pDispProject.CopyTo(ppProject);

		// record the project...
		m_rgProjects.Add(pProject);

		// lastly, tell the world about a new project existing
		DoFireItemAdded(pDispProject, NULL);
		hr = S_OK;
	}
	return hr;
}

HRESULT CVCProjectEngine::LoadXML( BSTR bstrFile, IDispatch **ppDispProject )
{
	HRESULT hr;

	// XML file format reader
	CComPtr<IXMLParser> spParser;
	CComPtr<IVCNodeFactory> spVCNodeFactory;
	
	// co-create one
	hr = spParser.CoCreateInstance( CLSID_XMLParser );
	RETURN_ON_FAIL_OR_NULL(hr, spParser);

	// co-create the node factory
	CComObject< CVCNodeFactory >* pVCNodeFactory = NULL;
	hr = CComObject< CVCNodeFactory >::CreateInstance( &pVCNodeFactory );
	RETURN_ON_FAIL(hr);
	hr = pVCNodeFactory->QueryInterface( __uuidof(IVCNodeFactory), reinterpret_cast<void**>( &spVCNodeFactory ) );
	RETURN_ON_FAIL(hr);
	RETURN_ON_NULL2( spVCNodeFactory, E_NOINTERFACE )
 	CComQIPtr<IXMLNodeFactory> spNodeFactory = spVCNodeFactory;
	RETURN_ON_NULL2(spNodeFactory, E_NOINTERFACE) 

	// set the node factory for the parser to use
	hr = spParser->SetFactory( spNodeFactory );
	RETURN_ON_FAIL(hr);

	hr = spParser->SetURL( NULL, bstrFile, 1 );
	RETURN_ON_FAIL(hr);

	// set the project/style sheet name for the node factory
	hr = spVCNodeFactory->put_Name( bstrFile );
	RETURN_ON_FAIL(hr);

	// run
	hr = spParser->Run( -1 );
	if( FAILED( hr ) )
	{
		// handle error conditions passed back from Run()
		return hr;
	}

	// get the root we just created (either project or stylesheet)
	spVCNodeFactory->get_Root( ppDispProject );

	CComBSTR bstrEncoding;
	spVCNodeFactory->get_FileEncoding(&bstrEncoding);
	if (bstrEncoding.Length() == 0)
		CXMLFile::DetermineDefaultFileEncoding(bstrEncoding);
	if (bstrEncoding.Length() > 0)
	{
		// get the VCProject from it so we can set the project file encoding we just loaded
		CComQIPtr<VCProject> pProject = *ppDispProject;
		if( pProject )	// if not project, probably style sheet
			pProject->put_FileEncoding( bstrEncoding );
	}

	return S_OK;
}

STDMETHODIMP CVCProjectEngine::LoadProject(BSTR szProject, IDispatch * * ppDispProject)
{
	if( s_pBuildPackage )	// only allowed to call this directly if not under the shell!
		return E_FAIL;

	CHECK_POINTER_VALID(ppDispProject);
	return LoadProjectUnderShell(szProject, ppDispProject);
}

STDMETHODIMP CVCProjectEngine::LoadProjectUnderShell(LPCOLESTR szProject, IDispatch * * ppDispProject)
{
	CHECK_POINTER_NULL(ppDispProject);
	Initialize(NULL, NULL, NULL, NULL); // just to be sure....

#ifdef AARDVARK
	{
		CStringW strStyleName = szProject;
		int nLen = strStyleName.GetLength();
		if (nLen > 8)
		{
			strStyleName = strStyleName.Right(8);
			strStyleName.MakeLower();
			if (strStyleName == L".vcstyle")
			{
				CComBSTR bstrStyleSheet = szProject;
				return LoadStyleSheet(bstrStyleSheet, ppDispProject);
			}
		}
	}
#endif
	bool bProjectConverted = false;
	CComPtr<IDispatch> pDispProject;
	CComPtr<IDispatch> pDispProjects;
	CComPtr<IVCCollection> pProjects;
	HRESULT hr = get_Projects(&pDispProjects);
	pProjects = pDispProjects;
	VSASSERT(pProjects, "No Projects collection?  Probably out of memory.");
	RETURN_ON_NULL2(pProjects, E_NOINTERFACE);

	// just in case
	pDispProject = NULL;

	CInLoadProject loadProj;
	
	// see if it is a conversion project...
	BOOL bConvertIt = FALSE;
	CComBSTR bstrProjectName = szProject;
	CComBSTR szDefaultProjExt = L".vcproj";
	if( bstrProjectName.Length() > szDefaultProjExt.Length() )
	{
	    LPCOLESTR pStr = &bstrProjectName[bstrProjectName.Length() - szDefaultProjExt.Length()];
	    if( wcsicmp( pStr, szDefaultProjExt ) )
	    {
			bConvertIt = TRUE;
	    }
	}
	if (bConvertIt)
	{
		CStringW strPath;
		if (!UtilGetModuleFileNameW(NULL, strPath))
			return E_FAIL;
		CCurDirW cwd(strPath, TRUE);	// Find the directory containing MFC before doing this CoCreate

		// only create a new proj converter if we need one
		hr = S_OK;
		if( !m_pProjConvert )
			hr = VsLoaderCoCreateInstance<IVCProjConvert>(CLSID_VCProjConvert, &m_pProjConvert);

		if (SUCCEEDED(hr) && m_pProjConvert)
		{
			CStringW strSourceFilesString;
			strSourceFilesString.LoadString(IDS_CONVERT_SOURCE_FILES);
			CComBSTR bstrSourceFilesString = strSourceFilesString;
			m_pProjConvert->Initialize(bstrSourceFilesString);

			hr = m_pProjConvert->OpenProject(bstrProjectName, static_cast<VCProjectEngine *>(this), s_pServiceProvider );

			// if we failed to convert, put up an error message
			if( FAILED( hr ) )
			{
				// if we got back "VCPROJ_E_UNSUPPORTED_PROJ_FILE" this is an
				// old project file
				const wchar_t *pStr = wcsrchr(bstrProjectName,L'.');
				long i = -1;
				if( pStr )
				{
					i = (long)(pStr - bstrProjectName);
				}

				if( hr == VCPROJ_E_UNSUPPORTED_PROJ_FILE )
				{
					CComBSTR bstrConversionErr;
					bstrConversionErr.LoadString( IDS_UNSUPPORTED_PROJ_FILE );
					DoFireReportError( bstrConversionErr, hr, CComBSTR(L"VC.ProjectConversionProblems") );
				}
				else if (hr == VCPROJ_E_BAD_PROJ_FILE)
				{
					CComBSTR bstrConversionErr;
					bstrConversionErr.LoadString( IDS_ERR_BAD_PROJ_FILE );
					DoFireReportError( bstrConversionErr, hr, CComBSTR(L"VC.ProjectConversionProblems") );
				}
				// check the extension to see if this was an .mak file
				else if( i != -1 && wcsicmp( L".mak", &bstrProjectName[i] ) == 0 )
				{
					CComBSTR bstrConversionErr;
					bstrConversionErr.LoadString( IDS_MAKCONVERSION_ERROR );
					DoFireReportError( bstrConversionErr, hr, CComBSTR(L"VC.ProjectConversionProblems") );
				}
				else
				{
					CComBSTR bstrConversionErr;
					bstrConversionErr.LoadString( IDS_CONVERSION_ERROR );
					DoFireReportError( bstrConversionErr, hr, CComBSTR(L"VC.ProjectConversionProblems") );
				}
				return hr;
			}

			wchar_t *pStr = wcsrchr(bstrProjectName,L'.');
			if( pStr )
			    pStr[0] = L'\0'; 			// temporarily terminate string
			CComBSTR bstrTempProjName = (wchar_t*)bstrProjectName; // Get the length of the string corrected, so that we can append without causing problems
			bstrTempProjName.Append(L".vcproj"); 	// Now append to the string 
			bstrProjectName = bstrTempProjName; 	// should have converted it to this...
			bProjectConverted = true;

			// get the project
			m_pProjConvert->GetProject( &pDispProject );
		}
	}
	else
	{
		hr = LoadXML( bstrProjectName, &pDispProject );
	}
	RETURN_ON_FAIL_OR_NULL(hr, pDispProject);

	CComQIPtr<VCProject> pProj = pDispProject;
	if( pProj )
	{
		// set the project file
		pProj->put_ProjectFile( bstrProjectName );

		// is this project under SCC?
		CComBSTR bstrSccProjectName;
		pProj->get_SccProjectName( &bstrSccProjectName );
		if( bstrSccProjectName.Length() )
		{
			// notify archy
			VARIANT_BOOL bRetVal;
			Fire_OnSccEvent( pDispProject, eProjectInScc, &bRetVal );
		}
	}
	CComQIPtr<IVCBuildableItem> spBuildableItem;
	spBuildableItem = pDispProject;
	VSASSERT(spBuildableItem, "How did we get a project object that doesn't support IVCBuildableItem?");
	if (spBuildableItem != NULL)
		spBuildableItem->AssignActions(VARIANT_TRUE);

	g_bInProjLoad = FALSE;

	// if we're converting, dirty the project file so it gets written
	CComQIPtr<IVCProjectImpl> pProjImpl = pDispProject;
 	if (pProjImpl && bProjectConverted)
 		pProjImpl->put_IsDirty(VARIANT_TRUE);

	// detach the IDispatch project object for return
	*ppDispProject = pDispProject.Detach();
	return hr;
}

STDMETHODIMP CVCProjectEngine::RemoveProject(IDispatch * pDispProject)
{
	if( s_pBuildPackage )	// only allowed to call this directly if not under the shell!
		return E_FAIL;

	return RemoveProjectUnderShell(pDispProject);
}

STDMETHODIMP CVCProjectEngine::RemoveProjectUnderShell(IDispatch * pDispProject)
{
	HRESULT hr = VCPROJ_E_NOT_FOUND;
	long l, lcItems;

	lcItems = m_rgProjects.GetSize();
	for (l = 0; l < lcItems && hr == VCPROJ_E_NOT_FOUND; l++)
	{
		CComPtr<VCProject> pProject;
		CComQIPtr<IDispatch> pDisp;

		pProject = m_rgProjects.GetAt(l);
		pDisp = pProject;
		if (pDisp && pDispProject == pDisp)
		{
			DoFireItemRemoved( pDispProject, NULL );
			CComQIPtr<IVCProjectImpl> pProjectImpl = pProject;
			pProjectImpl->Close(); // close down entirely; release all
			m_rgProjects.RemoveAtIndex(l);
			hr = S_OK;
		}
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_PROJ_NOT_FOUND_FOR_REMOVE);

	return hr;
}

STDMETHODIMP CVCProjectEngine::LoadHiddenStyleSheet(LPCOLESTR szStyleSheet, IDispatch **ppDispStyleSheet)
{
	CComPtr<VCStyleSheet> spStyleSheet;
	return DoLoadHiddenStyleSheet(szStyleSheet, &spStyleSheet, ppDispStyleSheet);
}

HRESULT CVCProjectEngine::DoLoadHiddenStyleSheet(LPCOLESTR szStyleSheet, VCStyleSheet** ppStyleSheet, IDispatch** ppDispStyleSheet)
{
	CHECK_POINTER_NULL(ppDispStyleSheet);
	Initialize(NULL, NULL, NULL, NULL); // just to be sure....

	CComPtr<IDispatch> pDispStyleSheets;
	HRESULT hr = get_AllStyleSheets(&pDispStyleSheets);
	CComQIPtr<IVCCollection> pStyleSheets = pDispStyleSheets;
	VSASSERT(pStyleSheets, "No StyleSheets collection?  Probably out of memory.");
	RETURN_ON_NULL2(pStyleSheets, E_NOINTERFACE);

	if (SUCCEEDED(hr))
	{
		hr = pStyleSheets->Item(CComVariant(szStyleSheet), ppDispStyleSheet);
		if (SUCCEEDED(hr) && *ppDispStyleSheet)
		{
			CComQIPtr<IVCStyleSheetImpl> spStyleSheetImpl = *ppDispStyleSheet;
			if (spStyleSheetImpl)
			{
				long nLoadCount = 0;
				spStyleSheetImpl->get_LoadCount(&nLoadCount);
				nLoadCount++;
				spStyleSheetImpl->put_LoadCount(nLoadCount);
			}
			CComQIPtr<VCStyleSheet> spStyle = spStyleSheetImpl;
			*ppStyleSheet = spStyle.Detach();
			return S_FALSE; // indicate that we had one already....
		}
	}

	CInLoadStyle loadStyle;

	hr = LoadXML( CComBSTR(szStyleSheet), ppDispStyleSheet );
	RETURN_ON_FAIL(hr);

	// DIANEME_TODO: we need to figure out how to handle style sheets under SCC


	// lastly, tell the world about a new style sheet existing
	DoFireItemAdded(*ppDispStyleSheet, NULL);

	// initial load count set in CreateHiddenStyleSheet
	CComQIPtr<VCStyleSheet> spStyle = *ppDispStyleSheet;
	*ppStyleSheet = spStyle.Detach();

	return hr;
}

STDMETHODIMP CVCProjectEngine::LoadStyleSheet(BSTR szStyleSheet, IDispatch **ppDispStyleSheet)
{
	CComPtr<VCStyleSheet> spStyleSheet;
	HRESULT hr = DoLoadHiddenStyleSheet(szStyleSheet, &spStyleSheet, ppDispStyleSheet);
	RETURN_ON_FAIL(hr);

	CComPtr<IDispatch> pDispStyleSheets;
	hr = get_StyleSheets(&pDispStyleSheets);
	CComQIPtr<IVCCollection> pStyleSheets = pDispStyleSheets;
	VSASSERT(pStyleSheets, "No StyleSheets collection?  Probably out of memory.");
	RETURN_ON_FAIL_OR_NULL2(hr, pStyleSheets, E_NOINTERFACE);

	CComPtr<IDispatch> pDispStyleSheet;
	hr = pStyleSheets->Item(CComVariant(szStyleSheet), &pDispStyleSheet);
	if (SUCCEEDED(hr) && pDispStyleSheet)
		return S_FALSE;		// already here in the 'visible' collection

	m_rgStyleSheets.Add(spStyleSheet);
	return S_OK;
}

HRESULT CVCProjectEngine::DoCreateHiddenStyleSheet(LPCOLESTR szStyleSheet, VCStyleSheet** ppStyleSheet, IDispatch** ppDispStyle)
{
	CHECK_POINTER_NULL(ppDispStyle);
	if (*szStyleSheet == L'\0')
		RETURN_INVALID();

	*ppDispStyle = NULL; // just in case we fail

	Initialize(NULL, NULL, NULL, NULL); // just to be sure....

	HRESULT hr = CStyleSheet::CreateInstance(ppStyleSheet);
	if (SUCCEEDED(hr) && *ppStyleSheet)
	{
		// firstly, we set the name
		hr = (*ppStyleSheet)->put_Name(CComBSTR(szStyleSheet));
		RETURN_ON_FAIL(hr);

		CComQIPtr<IVCStyleSheetImpl> spStyleSheetImpl = *ppStyleSheet;
		if (spStyleSheetImpl)
			spStyleSheetImpl->put_LoadCount(1);

		// and get the dispatch to return
		CComQIPtr<IDispatch> pDispStyleSheet = *ppStyleSheet;
		pDispStyleSheet.CopyTo(ppDispStyle);

		// record the StyleSheet...
		m_rgAllStyleSheets.Add(*ppStyleSheet);

		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::CreateHiddenStyleSheet(LPCOLESTR szStyleSheet, IDispatch **ppDispStyle)
{
	CComPtr<VCStyleSheet> spStyleSheet;
	return DoCreateHiddenStyleSheet(szStyleSheet, &spStyleSheet, ppDispStyle);
}

STDMETHODIMP CVCProjectEngine::CreateStyleSheet(BSTR szStyleSheet, IDispatch **ppDispStyle)
{
	CComPtr<VCStyleSheet> pStyleSheet;
	HRESULT hr = DoCreateHiddenStyleSheet(szStyleSheet, &pStyleSheet, ppDispStyle);
	RETURN_ON_FAIL(hr);

	// record the StyleSheet...
	m_rgStyleSheets.Add(pStyleSheet);

	// lastly, tell the world about a new style sheet existing
	DoFireItemAdded(*ppDispStyle, NULL);

	return hr;
}

STDMETHODIMP CVCProjectEngine::RemoveStyleSheet(IDispatch* pDispStyle)
{
	CHECK_READ_POINTER_VALID(pDispStyle);
	return DoRemoveStyleSheet(pDispStyle, FALSE /* don't remove from 'hidden' list */);
}

HRESULT CVCProjectEngine::DoRemoveStyleSheet(IDispatch* pDispStyle, BOOL bRemoveFromHiddenList)
{
	CHECK_READ_POINTER_NULL(pDispStyle);
	HRESULT hr = VCPROJ_E_NOT_FOUND;
	long l, lcItems, nLoadCount = 0;

	lcItems = m_rgStyleSheets.GetSize();
	for (l = 0; l < lcItems && hr == VCPROJ_E_NOT_FOUND; l++)
	{
		CComPtr<VCStyleSheet> spStyleSheet = m_rgStyleSheets.GetAt(l);
		CComQIPtr<IDispatch> spDisp = spStyleSheet;
		if (!spDisp)
			continue;
		if (pDispStyle != spDisp)
			continue;

		m_rgStyleSheets.RemoveAtIndex(l);
		CComQIPtr<IVCStyleSheetImpl> spStyleSheetImpl = spStyleSheet;
		if (spStyleSheetImpl)
		{
			spStyleSheetImpl->get_LoadCount(&nLoadCount);
			if (nLoadCount > 0)
			{
				nLoadCount--;
				spStyleSheetImpl->put_LoadCount(nLoadCount);
			}
		}
		hr = S_OK;
	}

	if ((hr == VCPROJ_E_NOT_FOUND && bRemoveFromHiddenList) || (nLoadCount < 1))
	{
		BOOL bCountAlreadyRemoved = SUCCEEDED(hr);
		lcItems = m_rgAllStyleSheets.GetSize();
		hr = VCPROJ_E_NOT_FOUND;	// if it's on the other list, it BETTER be on this one...
		for (l = 0; l < lcItems && hr == VCPROJ_E_NOT_FOUND; l++)
		{
			CComPtr<VCStyleSheet> spStyleSheet = m_rgAllStyleSheets.GetAt(l);
			CComQIPtr<IDispatch> spDisp = spStyleSheet;
			if (!spDisp)
				continue;
			if (pDispStyle != spDisp)
				continue;

			m_rgAllStyleSheets.RemoveAtIndex(l);
			CComQIPtr<IVCStyleSheetImpl> spStyleSheetImpl = spStyleSheet;
			if (spStyleSheetImpl)
			{
				if (!bCountAlreadyRemoved)
				{
					spStyleSheetImpl->get_LoadCount(&nLoadCount);
					if (nLoadCount > 0)
					{
						nLoadCount--;
						spStyleSheetImpl->put_LoadCount(nLoadCount);
					}
					VSASSERT(nLoadCount <= 0, "Hey, trying to remove a style sheet too early!");
				}
				spStyleSheetImpl->Close(); // close down entirely; release all
				hr = S_OK;
			}
		}
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_STYLE_NOT_FOUND_FOR_REMOVE);

	return hr;
}

STDMETHODIMP CVCProjectEngine::UnloadStyleSheet(IDispatch* pDispStyleSheet)
{
	CComQIPtr<IVCStyleSheetImpl> spStyleSheet = pDispStyleSheet;
	RETURN_ON_NULL2(spStyleSheet, E_UNEXPECTED);

	long nLoadCount = 0;
	spStyleSheet->get_LoadCount(&nLoadCount);
	if (nLoadCount > 0)
		nLoadCount--;
	spStyleSheet->put_LoadCount(nLoadCount);
	if (nLoadCount < 1)
		return DoRemoveStyleSheet(pDispStyleSheet, TRUE /* remove from 'hidden' list */);

	return S_OK;
}

STDMETHODIMP CVCProjectEngine::CreatePropertyGrid(IVSMDPropertyGrid **ppGrid)
{
	if( !m_pPropBrowser )
		if( s_pServiceProvider )
			s_pServiceProvider->QueryService( SID_SVSMDPropertyBrowser, __uuidof(IVSMDPropertyBrowser), (LPVOID *)&m_pPropBrowser );
	if( m_pPropBrowser )
		return m_pPropBrowser->CreatePropertyGrid(ppGrid);

	return E_FAIL;
}

STDMETHODIMP CVCProjectEngine::GetHelp(Help **ppHelp)
{
	HRESULT hr = E_FAIL;
	if( s_pServiceProvider )
		hr = s_pServiceProvider->QueryService(SID_SVsHelp, __uuidof(Help), (void **)ppHelp);
	return hr;
}

STDMETHODIMP CVCProjectEngine::IsSystemInclude(BSTR bstrFileName, VARIANT_BOOL *pbInclude)
{
	CHECK_POINTER_VALID(pbInclude);
	*pbInclude = VARIANT_FALSE;

    CStringW strFileName(bstrFileName);
	INT_PTR nLen = strFileName.GetLength();
	if (nLen == 0)
		return S_OK;

	// let's see if we were given a full path.  If so, get the actual OS case so that IsSysInclude can recognize it
	if (nLen > 3)
	{
		bool bNormalizePath = false;
		if (strFileName[1] == L':' && (strFileName[2] == L'/' || strFileName[2] == L'\\'))
			bNormalizePath = true;
		else if ((strFileName[0] == L'/' || strFileName[0] == L'\\') && (strFileName[1] == L'/' || strFileName[1] == L'\\'))
			bNormalizePath = true;

		if (bNormalizePath)
		{
			CPathW path;
			if (path.Create(strFileName))
			{
				path.GetActualCase(TRUE);
				strFileName = path;
			}
		}
	}
	if (g_SysInclReg.IsSysInclude(strFileName))
		*pbInclude = VARIANT_TRUE;
		
	return S_OK;
}

void CVCProjectEngine::InitializeForCommandLineUsage()
{
	// make sure we have console output
	HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);

	if ((hStdOut != INVALID_HANDLE_VALUE) && (hStdOut != NULL))
		s_hLogFile = hStdOut;

	// should be okay
	if (!s_hLogFile)
		s_hLogFile = hStdOut;
}

STDMETHODIMP CVCProjectEngine::WriteLog(LPCOLESTR lpszOut, BOOL bAppendCRLF)
{
	if (DoWriteLog(lpszOut, bAppendCRLF))
		return S_OK;

	return S_FALSE;
}

BOOL CVCProjectEngine::DoWriteLog(LPCOLESTR szOut, BOOL bAppendCRLF /* = FALSE */)
{
	RETURN_ON_NULL2(s_hLogFile, FALSE);

	// NULL means close stdout
	if (szOut == NULL || *szOut == L'\0')
	{
		::CloseHandle(s_hLogFile);
		s_hLogFile = NULL;
		return TRUE;
	}

	CStringA strOut = szOut;		// yes, ANSI -- due to the fact that we write an ANSI log
	strOut.AnsiToOem();

	int len;
	DWORD nBytes;
	const char* szOutA = strOut;
	const char* pszC = szOutA;
	while ((pszC = (const char *) _mbschr((const unsigned char*)szOutA, '\n')) != NULL)
	{
		len = (int)(pszC - szOutA);
		if (len > 0)
		{
			nBytes = (DWORD)(len*sizeof(char));
			WriteFile(s_hLogFile, szOutA, (DWORD)(len*sizeof(char)), &nBytes, NULL);
		}

		WriteFile(s_hLogFile, "\r\n", 2*sizeof(char), &nBytes, NULL);
		szOutA = ++pszC;
	}

	// check for any remainder
	len = lstrlenA(szOutA);
	if (len > 0)
	{
		nBytes = (DWORD)(len*sizeof(char));
		WriteFile(s_hLogFile, szOutA, (DWORD)(len*sizeof(char)), &nBytes, NULL);
	}

	if (bAppendCRLF)
	{
		WriteFile(s_hLogFile, "\r\n", 2*sizeof(char), &nBytes, NULL);
	}
	return TRUE;
}

STDMETHODIMP CVCProjectEngine::get_DialogOwner( HWND *pHwnd )
{
	CHECK_POINTER_NULL( pHwnd );

	CComPtr<IVsUIShell> pUIShell;
	HRESULT hr = E_FAIL;
	if( s_pServiceProvider )
	    hr = s_pServiceProvider->QueryService( SID_SVsUIShell, __uuidof(IVsUIShell), reinterpret_cast<void **>(&pUIShell) );
	RETURN_ON_FAIL_OR_NULL(hr, pUIShell);
	return pUIShell->GetDialogOwnerHwnd( pHwnd );
}

STDMETHODIMP CVCProjectEngine::EnableModeless( BOOL bEnable )
{
	CComPtr<IVsUIShell> pUIShell;
	HRESULT hr = E_FAIL;
	if( s_pServiceProvider )
		hr = s_pServiceProvider->QueryService( SID_SVsUIShell, __uuidof(IVsUIShell), reinterpret_cast<void **>(&pUIShell) );
	RETURN_ON_FAIL_OR_NULL(hr, pUIShell);

	return pUIShell->EnableModeless( bEnable );
}

STDMETHODIMP CVCProjectEngine::GetIVCProjConvert( IUnknown ** ppUnkProjConvert )
{
	// if we have a valid project conversion pointer, return it
	if( m_pProjConvert )
	{
		CComPtr<IUnknown> pUnk;
	   	m_pProjConvert.QueryInterface( &pUnk );
		if( pUnk )
			*ppUnkProjConvert = pUnk.Detach();
	}
	else
		*ppUnkProjConvert = NULL;
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::GetSKU( long *pSKU )
{
	HRESULT hr = E_FAIL;

    	if( m_nSKU == -1 )
	{
		CComPtr<IVsAppId> pAppID;
		CComVariant var;
		if( s_pServiceProvider )
		{
			hr = s_pServiceProvider->QueryService( SID_SVsAppId, __uuidof(IVsAppId), (void **)&pAppID );
			RETURN_ON_FAIL_OR_NULL(hr, pAppID);
			hr = pAppID->GetProperty(VSAPROPID_SKUEdition, &var );
			if( SUCCEEDED(hr) )
			{
			    m_nSKU = var.lVal;
			}
		}
	}
	else
	{
	    hr = S_OK;
	}

	*pSKU = m_nSKU;
	return hr;
}

STDMETHODIMP CVCProjectEngine::GetDialogFont( BOOL bBold, HFONT *pHFont )
{
	UIDLGLOGFONT logFont;
	CComPtr<IUIHostLocale> pLocale;

	*pHFont = 0;
	
	HRESULT hr = E_FAIL;
	if( s_pServiceProvider )
	{
		hr = s_pServiceProvider->QueryService( SID_SUIHostLocale, __uuidof(IUIHostLocale), (void**)&pLocale );
		RETURN_ON_FAIL_OR_NULL(hr, pLocale);
		pLocale->GetDialogFont( &logFont );
		if( bBold )
			logFont.lfWeight = 700;
	}
		
	*pHFont = CreateFontIndirectW( (LOGFONTW*)&logFont );
	
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::get_VendorTools(IDispatch** ppVendorTools)
{
	CHECK_POINTER_NULL(ppVendorTools);
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<IVCToolImpl>::CreateInstance(&pCollection, &m_rgVendorTools);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVendorTools);
	}
	return hr;
}

STDMETHODIMP CVCProjectEngine::AddVendorTool(LPCOLESTR szToolName, LPCOLESTR szToolGUID, IDispatch** ppVendorTool)
{
	CHECK_POINTER_NULL(ppVendorTool);
	CStringW strToolName = szToolName;
	strToolName.TrimLeft();
	strToolName.TrimRight();
	if (strToolName.IsEmpty())
		RETURN_INVALID();

	CComPtr<IDispatch> spDispTool;
	HRESULT hr = E_FAIL;
	if (szToolGUID)
	{
		CStringW strToolGUID = szToolGUID;
		strToolGUID.TrimLeft();
		strToolGUID.TrimRight();
		if (!strToolGUID.IsEmpty())
			hr = spDispTool.CoCreateInstance(LPCOLESTR(strToolGUID.GetBuffer(strToolGUID.GetLength())));
	}
	if (FAILED(hr))	// if we couldn't create it from the tool GUID, then try the tool name proper
		spDispTool.CoCreateInstance(LPCOLESTR(strToolName.GetBuffer(strToolName.GetLength())));
	RETURN_ON_FAIL(hr);

	CComQIPtr<IVCToolImpl> spToolImpl = spDispTool;
	CComQIPtr<IVCVendorToolFactory> spVendorToolFactory = spToolImpl;
	RETURN_ON_NULL2(spVendorToolFactory, E_UNEXPECTED);

	CComPtr<IDispatch> spDispProjEngine = g_pProjectEngine;
	spVendorToolFactory->SetProjectEngine(spDispProjEngine);

	// DIANEME_TODO: need to do validation on whether the number of IDs requested will
	// fit into whatever is left of the vendor tool ID range.  If it doesn't, then we
	// need to figure out how to get other chunks to stick stuff into.
	long cIDs = 0;
	spVendorToolFactory->get_RequiredPropertyRange(&cIDs);
	spVendorToolFactory->SetStartID(m_nNextVendorID);
	m_nNextVendorID += cIDs;

	// DIANEME_TODO: call GetBuildOnlyPropertyRange and do something with the results
	// DIANEME_TODO: call SupportsPlatform with all supported platforms and do something with the results
	*ppVendorTool = spDispTool.Detach();

	m_rgVendorTools.AddTail(spToolImpl);
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::GetStringForPropertyID(long nID, BSTR* pbstrString)
{
	CHECK_POINTER_NULL(pbstrString);
	if (nID <= 0)
		RETURN_INVALID();

	CComBSTR bstr;
	bstr.LoadString(nID);
	*pbstrString = bstr.Detach();
	return S_OK;
}

STDMETHODIMP CVCProjectEngine::GetMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);

	// no multi-props as yet
	return GetStrProperty(id, pbstrValue);
}

HRESULT CVCProjectEngine::DoGetProp(long id, VARIANT* pVarValue)
{
	CComVariant varVal;

	if (m_PropMap.Lookup(id, varVal))
	{
		if (pVarValue)
			varVal.Detach(pVarValue);
		return S_OK;
	}

	if (pVarValue)
		pVarValue = NULL;

	// We didn't find it
	return S_FALSE;
}

HRESULT CVCProjectEngine::DoSetProp(long id, BOOL bCheckSpecial, VARIANT varValue, long nOverrideID /* = -1 */)
{
	if (bCheckSpecial)
	{
		switch (id)
		{
		case PROJENGID_BuildLogging:
			return DoSetBoolProperty(PROJENGID_BuildLogging, TRUE, varValue.boolVal);
			break;
		default:
			// fall through
			break;
		}
	}

	m_PropMap.SetAt(id, varValue);
	return S_OK;
}

HRESULT CVCProjectEngine::DoSetStrProperty(long idProp, BOOL bCheckSpecial, BSTR bstrValue, long nOverrideID /* = -1 */)
{
	return CPropertyContainerImpl::DoSetStrProperty(idProp, bCheckSpecial, bstrValue, nOverrideID);
}

HRESULT CVCProjectEngine::DoSetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL bValue, long nOverrideID /* = -1 */)
{
	CHECK_VARIANTBOOL(bValue);

	if (bCheckSpecial && idProp == PROJENGID_BuildLogging)
	{
		s_bBuildLogging = bValue;
		return DoSetBoolProperty(PROJENGID_BuildLogging, FALSE, bValue);
	}

	CComVariant var(bValue);
	return DoSetProp(idProp, bCheckSpecial, var);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, 
	IVCPropertyContainer* pPropContainer)
{
	if (pEC == NULL)
		return;

	CComBSTR bstrProj;
	if (pPropContainer == NULL || FAILED(pPropContainer->GetStrProperty(VCPROJID_Name, &bstrProj)) || 
		bstrProj.Length() == 0)
		bstrProj.LoadString(IDS_PROJECT_WORD);
	pEC->AddError(bstrErr, bstrPrjErrCode, bstrProj, 0, NULL);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, IVCBuildActionList* pActionList)
{
	if (pActionList)
	{
		CComPtr<IVCBuildAction> spAction;
		pActionList->get_LastAction(&spAction);
		AddProjectError(pEC, bstrErr, bstrPrjErrCode, spAction);
		return;
	}

	AddProjectError(pEC, bstrErr, bstrPrjErrCode, (IVCPropertyContainer*)NULL);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, IVCBuildAction* pAction)
{
	CComPtr<IVCPropertyContainer> spPropContainer;
	if (pAction)
		pAction->get_PropertyContainer(&spPropContainer);

	AddProjectError(pEC, bstrErr, bstrPrjErrCode, spPropContainer);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, BSTR bstrErr, BSTR bstrPrjErrCode, CBldCfgRecord* pCfgRecord)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer;
	if (pCfgRecord)
	{
		VCConfiguration* pConfig = pCfgRecord->GetVCConfiguration();	// not ref-counted
		spPropContainer = pConfig;
	}

	AddProjectError(pEC, bstrErr, bstrPrjErrCode, spPropContainer);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, CStringW& strErr, BSTR bstrPrjErrCode,
	IVCPropertyContainer* pPropContainer)
{
	CComBSTR bstrErr = strErr;
	AddProjectError(pEC, bstrErr, bstrPrjErrCode, pPropContainer);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, CStringW& rstrErr, BSTR bstrPrjErrCode, CBldCfgRecord* pCfgRecord)
{
	CComBSTR bstrErr = rstrErr;
	AddProjectError(pEC, bstrErr, bstrPrjErrCode, pCfgRecord);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, CStringW& strErr, BSTR bstrPrjErrCode,
	IVCBuildActionList* pActionList)
{
	CComBSTR bstrErr = strErr;
	AddProjectError(pEC, bstrErr, bstrPrjErrCode, pActionList);
}

void CVCProjectEngine::AddProjectError(IVCBuildErrorContext* pEC, CStringW& strErr, BSTR bstrPrjErrCode, IVCBuildAction* pAction)
{
	CComBSTR bstrErr = strErr;
	AddProjectError(pEC, bstrErr, bstrPrjErrCode, pAction);
}

void CVCProjectEngine::AddProjectWarning(IVCBuildErrorContext* pEC, BSTR bstrWarn, BSTR bstrPrjWarnCode, 
	IVCPropertyContainer* pPropContainer)
{
	if (pEC == NULL)
		return;

	CComBSTR bstrProj;
	if (pPropContainer == NULL || FAILED(pPropContainer->GetStrProperty(VCPROJID_Name, &bstrProj)) || 
		bstrProj.Length() == 0)
		bstrProj.LoadString(IDS_PROJECT_WORD);
	pEC->AddWarning(bstrWarn, bstrPrjWarnCode, bstrProj, 0, NULL);
}

void CVCProjectEngine::AddProjectWarning(IVCBuildErrorContext* pEC, CStringW& strWarn, BSTR bstrPrjWarnCode,
	IVCPropertyContainer* pPropContainer)
{
	CComBSTR bstrWarn = strWarn;
	AddProjectWarning(pEC, bstrWarn, bstrPrjWarnCode, pPropContainer);
}

void CVCProjectEngine::AddProjectWarning(IVCBuildErrorContext* pEC, CStringW& strWarn, BSTR bstrPrjWarnCode, CBldCfgRecord* pCfgRecord)
{
	CComBSTR bstrWarn = strWarn;
	CComQIPtr<IVCPropertyContainer> spPropContainer;
	if (pCfgRecord)
	{
		VCConfiguration* pConfig = pCfgRecord->GetVCConfiguration();	// not ref-counted
		spPropContainer = pConfig;
	}

	AddProjectWarning(pEC, bstrWarn, bstrPrjWarnCode, spPropContainer);
}

void CVCProjectEngine::DoUpdateAfterApply()
{
	if (!s_pServiceProvider)
		return;

	CComPtr<IVsPropertyPageFrame> pPPFrame;
	HRESULT hr = s_pServiceProvider->QueryService(SID_SVsPropertyPageFrame, __uuidof(IVsPropertyPageFrame), (void**)&pPPFrame);
	if (SUCCEEDED(hr) && pPPFrame)
		pPPFrame->UpdateAfterApply();
}

/* static */
void CVCProjectEngine::ResolvePropertyContainer(IVCPropertyContainer* pPropContainer, IVCPropertyContainer** ppActualContainer)
{
	*ppActualContainer = NULL;
	if (pPropContainer == NULL)
		return;		// not a lot we can do

	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = pPropContainer;
	if (spStagedContainer)
		spStagedContainer->get_Parent(ppActualContainer);
	else
	{
		CComPtr<IVCPropertyContainer> spContainer = pPropContainer;
		*ppActualContainer = spContainer.Detach();
	}
}

/* static */
BOOL CVCProjectEngine::FLoadMlang()
{
	HRESULT hr = S_OK;

	if (!s_fInitializedCom)
	{
		hr = CoInitialize(NULL);
		RETURN_ON_FAIL2(hr, FALSE);

		s_fInitializedCom = TRUE;
	}

	if (!s_spMultiLanguage2 && !s_spMultiLanguage)
	{
		hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, __uuidof(IMultiLanguage2)
			, (void **)&s_spMultiLanguage2);
		if (FAILED(hr) || s_spMultiLanguage2 == NULL)
			hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, __uuidof(IMultiLanguage), 
			(void **)&s_spMultiLanguage);
		if (FAILED(hr) || (s_spMultiLanguage2 == NULL && s_spMultiLanguage == NULL))
			return FALSE;
	}

	return(SUCCEEDED(hr));
}

/* static */
UINT CVCProjectEngine::ConvertFromUnicodeMlang(UINT cp, BOOL fNoBestFit, BOOL fWriteEntities, LPCWSTR rgchUtf16, UINT cchUtf16, 
	LPSTR rgchMbcs, UINT cchMbcs, BOOL *pfDefCharUsed)
{
	if (!FLoadMlang())
		return(0);

	DWORD dwMode = 0;
	UINT cchSrc = cchUtf16;
	UINT cchDst = cchMbcs;
	DWORD dwFlags = fWriteEntities ? MLCONVCHARF_NCR_ENTITIZE : MLCONVCHARF_USEDEFCHAR;

	if (fNoBestFit)
		dwFlags |= MLCONVCHARF_NOBESTFITCHARS;

	HRESULT hr = S_OK;
	if (s_spMultiLanguage2)
		hr = s_spMultiLanguage2->ConvertStringFromUnicodeEx(&dwMode, cp, (WCHAR *) rgchUtf16, &cchSrc, rgchMbcs, &cchDst, 
			dwFlags, NULL);
	else
		hr = s_spMultiLanguage->ConvertStringFromUnicode(&dwMode, cp, (WCHAR *) rgchUtf16, &cchSrc, rgchMbcs, &cchDst);
	RETURN_ON_FAIL2(hr, 0);

	if (pfDefCharUsed != NULL)
		*pfDefCharUsed = (hr == S_FALSE);

	return(cchDst);
}

/* static */
BOOL CVCProjectEngine::GetCodePageName(int iCodePage, CComBSTR& bstrEncoding)
{
	bstrEncoding.Empty();

	if (!FLoadMlang())
		return FALSE;

	MIMECPINFO mimeInfo;
	HRESULT hr = S_OK;
	if (s_spMultiLanguage2)
		hr = s_spMultiLanguage2->GetCodePageInfo(iCodePage, 0, &mimeInfo);
	else
		hr = s_spMultiLanguage->GetCodePageInfo(iCodePage, &mimeInfo);
	RETURN_ON_FAIL2(hr, FALSE);
	bstrEncoding = mimeInfo.wszWebCharset;

	return TRUE;
}

/* static */
int CVCProjectEngine::GetCodePageFromCharSet(CComBSTR& bstrEncoding)
{
	if (!FLoadMlang())
		return 0;

	MIMECSETINFO mimeInfo;
	HRESULT hr = S_OK;
	if (s_spMultiLanguage2)
		hr = s_spMultiLanguage2->GetCharsetInfo(bstrEncoding, &mimeInfo);
	else
		hr = s_spMultiLanguage->GetCharsetInfo(bstrEncoding, &mimeInfo);
	RETURN_ON_FAIL2(hr, 0);

	return mimeInfo.uiCodePage;
}

// DIANEME_TODO: update CVCWebDeploymentTool to use this IISGetLocalPathForVirtualRoot instead of GetPathWWWRoot 
// once we lift the restriction on localhost.
// This version gets the FIRST vroot that we can find...  Won't work for anything other than localhost.
// Debug & use IISGetLocalPathForVirtualRoot to work with active directory & non-localhost 
/* static */
HRESULT CVCProjectEngine::GetPathWWWRoot(CComBSTR& bstrRoot)
{
	bstrRoot.Empty();
	HRESULT hr = S_OK;
	if (!s_fInitializedCom)
	{
		hr = CoInitialize(NULL);
		RETURN_ON_FAIL(hr);

		s_fInitializedCom = TRUE;
	}

	if (s_bstrIISRoot.Length() == 0)
	{
		CComPtr<IMSAdminBase> spAdmBase;
		hr = spAdmBase.CoCreateInstance(CLSID_MSAdminBase);
		RETURN_ON_FAIL_OR_NULL(hr, spAdmBase);

		METADATA_HANDLE hRootHandle = NULL;
		CStringW strKey(L"/LM/W3SVC/1");

		hr = spAdmBase->OpenKey(METADATA_MASTER_ROOT_HANDLE, strKey, METADATA_PERMISSION_READ, 20, &hRootHandle);
		RETURN_ON_FAIL(hr);

		METADATA_RECORD mr;
		ZeroMemory(&mr, sizeof(mr));
		DWORD dwLen = 0;
		int n = 0;
		CTempBuffer<wchar_t, MAX_PATH> buff;
		
		mr.pbMDData = (unsigned char*)(wchar_t*)buff.Allocate(MAX_PATH);
		mr.dwMDDataLen = MAX_PATH * sizeof(wchar_t); //size in bytes
		mr.dwMDDataType = ALL_METADATA;
		mr.dwMDUserType = IIS_MD_UT_WAM;
		mr.dwMDAttributes = METADATA_INHERIT;
		mr.dwMDIdentifier = MD_VR_PATH;
		hr = spAdmBase->GetData(hRootHandle, L"/ROOT", &mr, &dwLen);
		if (SUCCEEDED(hr))
		{
			CPathW pathRoot;
			if (pathRoot.Create(buff) && pathRoot.ExistsOnDisk())
				s_bstrIISRoot = buff;
		}
		else if (hr == E_ACCESSDENIED)
			s_bstrIISRoot = L"!admin";

		spAdmBase->CloseKey(hRootHandle);
	}

	bstrRoot = s_bstrIISRoot;
	return (bstrRoot.Length() > 0 ? S_OK : E_FAIL);
}

static void BuildPath(CStringW& strDir, LPCOLESTR pszFilename);
static void UrlSlashToPathSlash(LPOLESTR pBuff);
static void SplitUrl2(LPCOLESTR pszUrl, CStringW& strProtocol, CStringW& strServer, CStringW& strPort, CStringW& strWebPath);
static void SplitUrl(LPCOLESTR pszUrl, CStringW& strServer, CStringW& strWeb);
static void ReplaceLocalHost(CStringW& strServerName);
static BOOL FIsIPAddr(CStringW& strDNSName);
static HRESULT GetServerDescriptors(LPCWSTR pszWebUrl,     // IN: full web url
                          BOOL *pbIsRootDir,         // OUT: set to true if this project is at a root
                          CStringW &strServerDNSName,// OUT: server name
                          CStringW &strAdsWebDirPath,// OUT: /Root/webdir - ADS path to web directory
                          CStringW &strPortNum,      // OUT: port server is on
                          CStringW &strIPAddr);       // OUT server ip addr
static void CreateIISBindings(LPCWSTR pszPortNum,  // IN: port for server
                      LPCWSTR pszIPAddr,   // IN: server ipaddr
                      CStringW &strIISBindingsGenericMatch,   // OUT: generic k2 binding string
                      CStringW &strIISBindingsBestMatch);      // OUT: best binding
static void CreateADSRootPath(LPCWSTR pszServerDNSName,   // IN: server name
                       CString &strADSRootPath); // OUT: path to root

/* static */
HRESULT CVCProjectEngine::InitAdminBase()
{
	if (s_spAdminBase != NULL)
		return S_OK;

	HRESULT hr = S_OK;
	if (!s_fInitializedCom)
	{
		hr = CoInitialize(NULL);
		RETURN_ON_FAIL(hr);

		s_fInitializedCom = TRUE;
	}

    // Create the IIS admin base object
    return ::CoCreateInstance(CLSID_MSAdminBase, NULL, CLSCTX_SERVER, __uuidof(IMSAdminBase), (void**)&s_spAdminBase);
}

//---------------------------------------------------------------------------
// It uses the IIS metabase to lookup the local path for the given virtual root.
//---------------------------------------------------------------------------
// DIANEME_TODO: update CVCWebDeploymentTool to use this method once we lift the restriction on localhost
// This version finds the actual path for the given vroot.  Adapted from VSProject code of the same name(s).
// Currently using GetPathWWWRoot since we're only after localhost.
/* static */	
HRESULT CVCProjectEngine::IISGetLocalPathForVirtualRoot(LPCWSTR pszWebUrl, CStringW& strLocalPath)
{
	// Init return value
	strLocalPath.Empty();

	CStringW strWebNum;
	HRESULT hr = IISGetWebNumberForUrl(pszWebUrl, strWebNum);

	if(FAILED(hr))
		strWebNum = L"1";

	// Create the IIS admin base object
	hr = InitAdminBase();
	RETURN_ON_FAIL(hr);

	VSASSERT(s_spAdminBase, "Hey, how did we get this far with no IMSAdminBase service?");
	RETURN_ON_NULL(s_spAdminBase);

	// We are going to walk forward starting at the root until we get the best match for the vroot. As we walk forward,
	// if we get the same path in subsequent calls we stop and append the remaining vroot path. We need to
	// do this because IIS doesn't purge metadata information and when you ask for the path of a folder which no longer
	// exists it may return the iis root path. 

	CStringW strVRootPath;
	WCHAR szPath [MAX_PATH + 1];
	METADATA_RECORD mdrNewVrootData;
	ZeroMemory(&mdrNewVrootData, sizeof(mdrNewVrootData));

	mdrNewVrootData.dwMDIdentifier = MD_VR_PATH;
	mdrNewVrootData.dwMDAttributes = METADATA_INHERIT;
	mdrNewVrootData.dwMDUserType = IIS_MD_UT_FILE;
	mdrNewVrootData.dwMDDataType = STRING_METADATA;
	mdrNewVrootData.pbMDData = (PBYTE)szPath;
	mdrNewVrootData.dwMDDataLen = (_MAX_PATH) * sizeof (WCHAR);
	mdrNewVrootData.dwMDDataTag = 0;  // datatag is a reserved field.
	strVRootPath = L"/LM/W3SVC/";
	strVRootPath += strWebNum + L"/root";
	DWORD dwReqLen;

    // Get the trailing vroot part
    CStringW strVRoot;
    CStringW strServer;
	SplitUrl(pszWebUrl, strServer, strVRoot);

	// Algorithm below requires a leading slash
	if(strVRoot[0] != L'/')
		strVRoot.Insert(0, L'/');
        
	// Setup our string pointers. We use them in the loop to walk forward slash by slash. pszCurrent points to
	// the current one we are attempting, pszNext points to the next token
	WCHAR* pszCurrent = strVRoot.GetBuffer(MAX_PATH+1);
	WCHAR* pszNext = pszCurrent + 1;
	while(TRUE)
	{
		METADATA_HANDLE hVRootHandle = 0;
		hr = s_spAdminBase->OpenKey(METADATA_MASTER_ROOT_HANDLE, strVRootPath.GetBuffer(MAX_PATH+1), METADATA_PERMISSION_READ, 1000, &hVRootHandle);
		strVRootPath.ReleaseBuffer();
		if (FAILED(hr)) // Break outta here on any failure
			break;

		// VRoot found. Get the local path (note that we must reset the datalength each time through)
		mdrNewVrootData.dwMDDataLen = (_MAX_PATH) * sizeof (WCHAR);
		hr = s_spAdminBase->GetData(hVRootHandle, L"/", &mdrNewVrootData, &dwReqLen);
		// Close the key we opened
		s_spAdminBase->CloseKey(hVRootHandle);
		if (FAILED(hr)) // Break outta here on any failure
			break;

		// If path matches the last path, we are done and we will append the remaining vroot onto it.
		if (strLocalPath.CompareNoCase(szPath) == 0)
		{
			break;
		}
		else
		{   // Paths differ. Update to the new path and we will try again
			strLocalPath = szPath;

			// Find the next slash, or end.
			pszCurrent = pszNext;
			if(pszCurrent == NULL || *pszCurrent == 0)
				break;
			pszNext = wcschr(pszNext, L'/');
			if(pszNext)
			{
				*pszNext = 0;
				strVRootPath += L'/';
				strVRootPath += pszCurrent;
				*pszNext = L'/';
				pszNext++;
			}
			else
			{   // No more slashes, just add in the last one.
				strVRootPath += L'/';
				strVRootPath += pszCurrent;
			}
		}
	}

	// Just append the remaining part (pszCurrent) to get the full path. 
	if (!strLocalPath.IsEmpty())
	{
		if (pszCurrent && *pszCurrent != 0)
		{
			UrlSlashToPathSlash(pszCurrent);
			BuildPath(strLocalPath, pszCurrent);
		}
		hr = S_OK;
	}

	return hr;
}

//---------------------------------------------------------------------------
// Returns the IIS web number for the given url
//---------------------------------------------------------------------------
/* static */
HRESULT CVCProjectEngine::IISGetWebNumberForUrl(LPCWSTR pszWebUrl, CStringW& strWebNum)
{
	CStringW strServerDNSName;
	CStringW strAdsWebDirPath;
	CStringW strPortNum;
	CStringW strIPAddr;
	CStringW strIISBindingsGenericMatch;
	CStringW strIISBindingsBestMatch;
	CStringW strADSRootPath;
	BOOL    bIsRootDir;

	// First get some information about the target server
	HRESULT hr = GetServerDescriptors(pszWebUrl, &bIsRootDir, strServerDNSName, strAdsWebDirPath, strPortNum, strIPAddr);
	RETURN_ON_FAIL(hr);

	// Create IIS bindings. Used to get server number
	CreateIISBindings(strPortNum, strIPAddr, strIISBindingsGenericMatch, strIISBindingsBestMatch);
	CreateADSRootPath(strServerDNSName, strADSRootPath);

	// Go get the web number
	return GetWebNumberForServer(strADSRootPath, strIISBindingsGenericMatch, strIISBindingsBestMatch, strWebNum);
}   

//---------------------------------------------------------------------------
// Returns the web number of the IIS server which best matches the bindings 
// passed in. It's ugly, real ugly
//---------------------------------------------------------------------------
/* static */
HRESULT CVCProjectEngine::GetWebNumberForServer(LPCWSTR pszADSRootPath,           // IN: path to ads root
                              const CStringW& strIISBindingsGenericMatch, // IN: generic IIS bindings
                              const CStringW& strIISBindingsBestMatch,   // IN: best IIS bindings
                              CStringW &strWebNum)               // OUT: web number
{
	CComPtr<IADsContainer> srpContainer;
	IEnumVARIANT* pEnum = NULL; // Not a smartptr since we need to free it with ADsFreeEnumerator().
	CComPtr<IADs> srpChild;
	CComPtr<IADs> srpMatch;
	CComPtr<IADs> srpNonStartedMatch;
	CComPtr<IADs> srpADsRoot;
	int idHelp = 0;
	CComVariant varTemp;
	BOOL bFound = FALSE;
	HRESULT hr;

	// Get the ActiveDs wrapper object. If ADSI is not installed we return a web number of 1 adn S_FALSE
	if (s_pAdsiWrapper == NULL)
	{
		s_pAdsiWrapper = new CAdsiWrapper();
        // Init the wrapper. If it fails we clean up the object
        if(s_pAdsiWrapper && s_pAdsiWrapper->Init() == FALSE)
        {
            delete s_pAdsiWrapper;
            s_pAdsiWrapper = NULL;
        }
	}
	if(s_pAdsiWrapper == NULL)
	{
		DoSetErrorInfo(E_FAIL, IDS_E_ADSI_NOTINSTALLED);
		return E_FAIL;
	}

	hr = s_pAdsiWrapper->ADsGetObject(pszADSRootPath, __uuidof(IADs), reinterpret_cast<void **>(&srpADsRoot));
	if(FAILED(hr)) 
	{	// IIS changed their schema between ver4 and ver5. This means we can't set the approot on 
		// an iis4 machine from an iis5 machine.
		if(hr == MD_ERROR_IISAO_INVALID_SCHEMA)
		{
			DoSetErrorInfo(MD_ERROR_IISAO_INVALID_SCHEMA, IDS_E_CANTCFG_APPROOT_IIS4_FROM_IIS5);
		}
		goto done;
	}        

	// Find the correct server number for this server.
	hr = srpADsRoot->QueryInterface(__uuidof(IADsContainer), (void **)&srpContainer);
	if(SUCCEEDED(hr))
		hr = s_pAdsiWrapper->ADsBuildEnumerator(srpContainer, &pEnum);
	if(FAILED(hr)) 
		goto done;

	while (!bFound && (S_OK == (hr = s_pAdsiWrapper->ADsEnumerateNext(pEnum, 1, &varTemp, NULL))))
	{
		CComBSTR bstrTemp1;
		// This should be a dispatch interface...
		if (varTemp.vt != VT_DISPATCH) 
		{
			VSASSERT(FALSE, "Wrong kind of item in ADs enumerator!");
			varTemp.Clear();
			continue;
		}
		if (varTemp.pdispVal == NULL)
		{
			VSASSERT(FALSE, "Null pointer in ADs enumerator!");
			varTemp.Clear();
			continue;
		}

		// The class of the ADSI object needs to be IIS_CLASS_WEB_SERVER
		srpChild = NULL;
		hr = varTemp.pdispVal->QueryInterface(__uuidof(IADs), (void **)&srpChild);
		if(SUCCEEDED(hr) && srpChild != NULL)
			hr = srpChild->get_Class(&bstrTemp1);

		// Free IDispatch
		varTemp.Clear();
		if(FAILED(hr) || !bstrTemp1 || wcscmp(IIS_CLASS_WEB_SERVER_W, bstrTemp1)) 
		{   
			continue;
		}

		// We have a web server. Is this THE ONE?
		hr = srpChild->Get(L"ServerBindings", &varTemp);
		if(FAILED(hr) || V_VT(&varTemp) != (VT_ARRAY|VT_VARIANT)) 
		{
			VSASSERT(FALSE, "Bad server binding!");
			varTemp.Clear();
			continue;
		}
		SAFEARRAY *psaBindings = V_ARRAY(&varTemp);
		if(::SafeArrayGetDim(psaBindings) != 1) 
		{
			varTemp.Clear();
			VSASSERT(FALSE, "Bad server binding!");
			continue;
		}
		long lLBound, lUBound;
		hr = ::SafeArrayGetLBound(psaBindings, 1, &lLBound);
		if SUCCEEDED(hr)
			hr = ::SafeArrayGetUBound(psaBindings, 1, &lUBound);
		if (FAILED(hr)) 
		{
			varTemp.Clear();
			VSASSERT(FALSE, "Bad server binding!");
			continue;
		}
		// Iterate over the bindings list looking for a match
		for (long l = lLBound; l <= lUBound && SUCCEEDED(hr); l++) 
		{
			CComVariant varItem;
			hr = ::SafeArrayGetElement(psaBindings, &l, &varItem);
			if (FAILED(hr) || V_VT(&varItem) != (VT_BSTR) || varItem.bstrVal == NULL) 
			{
				VSASSERT(FALSE, "Bad server binding!");
				continue;
			}
			if(!_wcsnicmp(strIISBindingsBestMatch, V_BSTR(&varItem), strIISBindingsBestMatch.GetLength()))
			{	// exact match. We're done
				srpMatch = srpChild;
				bFound = TRUE;
				break;
			} 
			else if(!_wcsnicmp(strIISBindingsGenericMatch, V_BSTR(&varItem),  strIISBindingsGenericMatch.GetLength()))
			{	// So far this is the best match. See if this web server is started. If so, set it as a match.
				// We have a web server. Is this THE ONE?
				CComVariant varState;
				hr = srpChild->Get(L"ServerState", &varState);
				if(SUCCEEDED(hr) && varState.vt == (VT_I4) && varState.lVal == MD_SERVER_STATE_STARTED) 
				{
					srpMatch = srpChild;
					continue;
				}
				else if(srpNonStartedMatch == NULL)
				{	// Save it as a nonstarted server. We will use this in the end if all else fails and there are
					// no started servers.
					srpNonStartedMatch = srpChild;
				}
			}
		}
		// Free the safeArray
		varTemp.Clear();
	}

	// If all we got were non started servers we use it
	if(srpMatch == NULL && srpNonStartedMatch)
		srpMatch = srpNonStartedMatch;

	// Get the web number. It is the name of the match object. 
	if(srpMatch) 
	{
		CComBSTR bstrWebNum;
		hr = srpMatch->get_Name(&bstrWebNum);
		if(SUCCEEDED(hr)) 
			strWebNum = bstrWebNum;
	} 
	else 
	{	// Did not find anything
		hr = E_FAIL;
	}

done:
	if(pEnum)
	{   // Free ourr enumerator
		VSASSERT(s_pAdsiWrapper, "How did we get an enumerator without an active directory wrapper?");
		s_pAdsiWrapper->ADsFreeEnumerator(pEnum);
	}
	return hr;
}

HRESULT CVCProjectEngine::GetOpenFileNameViaDlg( VSOPENFILENAMEW* pOpenFileName )
{
	HRESULT hr = S_FALSE;
	CComPtr<IVsUIShell> pShell;
	if (s_pServiceProvider)
		hr = s_pServiceProvider->QueryService( SID_SVsUIShell, &pShell );
	if (pShell)
		hr = pShell->GetOpenFileNameViaDlg( pOpenFileName );
	return hr;
}

HRESULT CVCProjectEngine::GetDirViaDlg( VSBROWSEINFOW* pDir )
{
	HRESULT hr = S_FALSE;
	CComPtr<IVsUIShell> pShell;
	if (s_pServiceProvider)
		hr = s_pServiceProvider->QueryService( SID_SVsUIShell, &pShell );
	if (pShell)
		hr = pShell->GetDirectoryViaBrowseDlg(pDir);
	return hr;
}

/* static */
HRESULT CVCProjectEngine::DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bReportError /* = TRUE */, 
	BSTR bstrHelpTopic /* = NULL */, BOOL bErrorInfoOnly /* = TRUE */)
{
	VCASSERTME(hrErr);

	if (bReportError)
	{
		CComBSTR bstrErr;
		bstrErr.LoadString(idErr);
		HandleSetErrorInfo(hrErr, bstrErr, bstrHelpTopic, bErrorInfoOnly);
	}

	return hrErr;
}

/* static */
HRESULT CVCProjectEngine::DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bReportError /* = TRUE */, 
	BSTR bstrHelpTopic /* = NULL */, BOOL bErrorInfoOnly /* = TRUE */)
{
	if (bReportError)
	{
		CStringW strErr;
		strErr.Format(idErr, szParam);
		CComBSTR bstrErr = strErr;
		HandleSetErrorInfo(hrErr, bstrErr, bstrHelpTopic, bErrorInfoOnly);
	}

	return hrErr;
}

/* static */
HRESULT CVCProjectEngine::DoSetErrorInfo3(HRESULT hrErr, long idErr, LPCOLESTR szParam1, LPCOLESTR szParam2, 
	BOOL bReportError /*= TRUE */, BSTR bstrHelpTopic /* = NULL */, BOOL bErrorInfoOnly /* = TRUE */)
{
	if (bReportError)
	{
		CStringW strErr;
		strErr.Format(idErr, szParam1, szParam2);
		CComBSTR bstrErr = strErr;
		HandleSetErrorInfo(hrErr, bstrErr, bstrHelpTopic, bErrorInfoOnly);
	}

	return hrErr;
}

/* static */
HRESULT CVCProjectEngine::HandleSetErrorInfo(HRESULT hrError, LPCOLESTR pszDescription, BSTR bstrHelpTopic, 
	BOOL bErrorInfoOnly)
{
	if (hrError == S_OK)
	{
		if (pszDescription == NULL)
		return (::SetErrorInfo(0, NULL));
	}

	CComPtr<ICreateErrorInfo> spCreateErrInfo;
	CComPtr<IErrorInfo> spErrInfo;
	HRESULT hr = ::CreateErrorInfo(&spCreateErrInfo);
	RETURN_ON_FAIL(hr);
	hr = spCreateErrInfo->SetDescription((LPOLESTR)pszDescription);
	RETURN_ON_FAIL(hr);
	hr = spCreateErrInfo->SetGUID(GUID_NULL);
	RETURN_ON_FAIL(hr);
	hr = spCreateErrInfo->SetHelpContext(0);
	RETURN_ON_FAIL(hr);
	hr = spCreateErrInfo->QueryInterface(__uuidof(IErrorInfo), (void **)&spErrInfo);
	RETURN_ON_FAIL(hr);
	::SetErrorInfo(0, spErrInfo);
	if (bErrorInfoOnly)
		return hrError;

	CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
	if (spProjEngineImpl)
	{
		CComBSTR bstrErr = pszDescription;
		spProjEngineImpl->DoFireReportError(bstrErr, hrError, bstrHelpTopic);
	}

	return hrError;
}

HRESULT ExternalQueryService(REFGUID rsid, REFIID iid, void **ppvObj)
{
	CHECK_POINTER_NULL(ppvObj);
	*ppvObj = NULL;
	RETURN_ON_NULL2(CVCProjectEngine::s_pServiceProvider, S_FALSE);
	return CVCProjectEngine::s_pServiceProvider->QueryService(rsid, iid, ppvObj);
}

HRESULT DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bReport /* = TRUE */)
{
	return CVCProjectEngine::DoSetErrorInfo(hrErr, idErr, bReport);
}

HRESULT DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bReport /* = TRUE */)
{
	return CVCProjectEngine::DoSetErrorInfo2(hrErr, idErr, szParam, bReport);
}

HRESULT DoSetErrorInfo3(HRESULT hrErr, long idErr, LPCOLESTR szParam1, LPCOLESTR szParam2, BOOL bReport /* = TRUE */)
{
	return CVCProjectEngine::DoSetErrorInfo3(hrErr, idErr, szParam1, szParam2, bReport);
}

//---------------------------------------------------------------------------
// This class does a loadlibrary on the active directory services dll, and 
// exposes methods to call its functions.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constructor.
//---------------------------------------------------------------------------
CAdsiWrapper::CAdsiWrapper() :
	m_hAdsiLib(NULL),
	m_pADsGetObject(NULL),
	m_pADsBuildEnumerator(NULL),
	m_pADsEnumerateNext(NULL),
	m_pADsFreeEnumerator(NULL),
	m_pADsGetLastError(NULL)
{
}

//---------------------------------------------------------------------------
// Free up our library if need be
//---------------------------------------------------------------------------
CAdsiWrapper::~CAdsiWrapper()
{
	if(m_hAdsiLib)
		::FreeLibrary(m_hAdsiLib);
}

//---------------------------------------------------------------------------
// Used to intialize the class. Will call LoadLibaryEx() to try and load
// the active directory services dll. If it fails, it returns FALSE;
//---------------------------------------------------------------------------
BOOL CAdsiWrapper::Init()
{
	BOOL bRet = FALSE;
	VSASSERT(m_hAdsiLib == NULL, "Already initialized?");
	if (m_hAdsiLib == NULL)
		m_hAdsiLib = ::LoadLibraryExW(L"activeds.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (m_hAdsiLib >= (HINSTANCE)HINSTANCE_ERROR) 
	{
		bRet = TRUE;
	}
	else
	{
		m_hAdsiLib = 0;
	}
	return bRet;
}

//---------------------------------------------------------------------------
// Calls the ADsGetObject() function
//---------------------------------------------------------------------------
HRESULT CAdsiWrapper::ADsGetObject(LPCWSTR pszADSRootPath, REFIID riid, void** ppADsRoot)
{
	HRESULT hr = E_UNEXPECTED;
	VSASSERT(m_hAdsiLib, "Trying to call ADsGetObject without initializing wrapper first!");
	if(m_hAdsiLib)
	{	// Get a pointer to the function if we haven't done so already.
		if (m_pADsGetObject == NULL)
			m_pADsGetObject = (fnpADsGetObject)GetProcAddress(m_hAdsiLib, "ADsGetObject");

		// Call ADsGetObject()
		if (m_pADsGetObject)
			hr = m_pADsGetObject(pszADSRootPath, riid, ppADsRoot);
	}
	return hr;
}
//---------------------------------------------------------------------------
// Calls the ADsBuildEnumerator function
//---------------------------------------------------------------------------
HRESULT CAdsiWrapper::ADsBuildEnumerator(IADsContainer *pADsContainer, IEnumVARIANT **ppEnumVariant)
{
	HRESULT hr = E_UNEXPECTED;
	VSASSERT(m_hAdsiLib, "Trying to call ADsGetObject without initializing wrapper first!");
	if (m_hAdsiLib)
	{	// Get a pointer to the function if we haven't done so already.
		if (m_pADsBuildEnumerator == NULL)
			m_pADsBuildEnumerator = (fnpADsBuildEnumerator)GetProcAddress(m_hAdsiLib, "ADsBuildEnumerator");

		// Call ADsGetObject()
		if (m_pADsBuildEnumerator)
			hr = m_pADsBuildEnumerator(pADsContainer, ppEnumVariant);
	}
	return hr;
}
//---------------------------------------------------------------------------
// Calls the ADsEnumerateNext function
//---------------------------------------------------------------------------
HRESULT CAdsiWrapper::ADsEnumerateNext(IEnumVARIANT *pEnumVariant, ULONG cElements, VARIANT* pvar, ULONG* pcElementsFetched)
{
	HRESULT hr = E_UNEXPECTED;
	VSASSERT(m_hAdsiLib, "Trying to call ADsGetObject without initializing wrapper first!");
	if(m_hAdsiLib)
	{	// Get a pointer to the function if we haven't done so already.
		if(m_pADsEnumerateNext == NULL)
			m_pADsEnumerateNext = (fnpADsEnumerateNext)GetProcAddress(m_hAdsiLib, "ADsEnumerateNext");

		// Call ADsEnumberateNext()
		if(m_pADsEnumerateNext)
			hr = m_pADsEnumerateNext(pEnumVariant, cElements, pvar, pcElementsFetched);
	}
	return hr;
}

//---------------------------------------------------------------------------
// Calls the ADsFreeEnumerator function
//---------------------------------------------------------------------------
HRESULT CAdsiWrapper::ADsFreeEnumerator(IEnumVARIANT *pEnumVariant)
{
	HRESULT hr = E_UNEXPECTED;
	VSASSERT(m_hAdsiLib, "Trying to call ADsGetObject without initializing wrapper first!");
	if(m_hAdsiLib)
	{	// Get a pointer to the function if we haven't done so already.
		if(m_pADsFreeEnumerator == NULL)
			m_pADsFreeEnumerator = (fnpADsFreeEnumerator)GetProcAddress(m_hAdsiLib, "ADsFreeEnumerator");

		// Call ADsGetObject()
		if(m_pADsFreeEnumerator)
			hr = m_pADsFreeEnumerator(pEnumVariant);
	}
	return hr;
}
//---------------------------------------------------------------------------
// Calls the ADsGetLastError function
//---------------------------------------------------------------------------
HRESULT CAdsiWrapper::ADsGetLastError(LPDWORD lpError, LPWSTR lpErrorBuf, DWORD dwErrorBufLen, LPWSTR lpNameBuf, DWORD dwNameBufLen)
{
	HRESULT hr = E_UNEXPECTED;
	VSASSERT(m_hAdsiLib, "Trying to call ADsGetObject without initializing wrapper first!");
	if (m_hAdsiLib)
	{	// Get a pointer to the function if we haven't done so already.
		if (m_pADsGetLastError == NULL)
			m_pADsGetLastError = (fnpADsGetLastError)GetProcAddress(m_hAdsiLib, "ADsGetLastError");

		// Call ADsGetObject()
		if (m_pADsGetLastError)
			hr = m_pADsGetLastError(lpError, lpErrorBuf, dwErrorBufLen, lpNameBuf, dwNameBufLen);
	}
	return hr;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireItemAdded(IDispatch *pDispObj, IDispatch *pDispParent)
{
	Fire_OnItemAdded(pDispObj, pDispParent);
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireItemRemoved(IDispatch *pDispObj, IDispatch *pDispParent)
{
	Fire_OnItemRemoved(pDispObj, pDispParent);
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireItemRenamed(IDispatch *pDispObj, IDispatch *pDispParent, BSTR bstrOldName)
{
	Fire_OnItemRenamed(pDispObj, pDispParent, bstrOldName);
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireItemMoved(IDispatch* pItem, IDispatch* pNewParent, IDispatch* pOldParent)
{
	Fire_OnItemMoved(pItem, pNewParent, pOldParent);
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireItemPropertyChange(IDispatch *pDispObj, long lDispid)
{
	Fire_OnItemPropertyChange(pDispObj, lDispid);
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireSccEvent( IDispatch* pItem, enumSccEvent event, VARIANT_BOOL *pRet )
{
	Fire_OnSccEvent( pItem, event, pRet );
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireReportError( BSTR bstrErrMsg, HRESULT hrReport, BSTR bstrHelpKeyword )
{
	Fire_OnReportError( bstrErrMsg, hrReport, bstrHelpKeyword );
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireProjectBuildStarted(IDispatch *pCfg)
{
	Fire_OnProjectBuildStarted( pCfg );
	return S_OK;
}

STDMETHODIMP CVCProjectEngineEvents::DoFireProjectBuildFinished(IDispatch *pCfg, long warnings, long errors, VARIANT_BOOL bCancelled)
{
	Fire_OnProjectBuildFinished( pCfg, warnings, errors, bCancelled );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Given protocol://server/web, splits the url into protocol://server, web
//-----------------------------------------------------------------------------
static void SplitUrl(LPCOLESTR pszUrl, CStringW& strServer, CStringW& strWeb)
{
	if (!pszUrl)
		return;

	wchar_t szTemp[_MAX_PATH+1];
	wcscpy(szTemp, pszUrl);

	// Find protocol ":"
	wchar_t* p1 = wcschr(szTemp, L':');
	if (p1)
	{   // Skip 2 slashes
		p1++;
		if (*p1 == L'/')
		{
			p1++;
			if(*p1 == L'/')
				p1++;
		}
		// Now find next slash. This denotes the end of the server part. For file urls
		// this should be file://
		p1 = _tcschr(p1, L'/');
		if (p1)
		{   // Terminate to set the server
			*p1 = 0;
			strServer = szTemp;
			// Increment to the web and assign.
			p1++;
			strWeb = p1;
		}
		else
		{   // No webname
			strServer = pszUrl;
			strWeb.Empty();
		}
	}
}

//-----------------------------------------------------------------------------
// Given a full url, http://server:80/folder1/folder2, splits the url into:
// strProtocol  - "http:"
// strPort      - "80" - empty string if no port specified
// strServer    - "server"
// strWebPath   - "folder1/folder2"
//-----------------------------------------------------------------------------
static void SplitUrl2(LPCOLESTR pszUrl, CStringW& strProtocol, CStringW& strServer, CStringW& strPort, CStringW& strWebPath)
{
	if (!pszUrl)
		return;

	wchar_t szTemp[_MAX_PATH+1];
	wcscpy(szTemp, pszUrl);

	// Find protocol ":"
	wchar_t* p1 = wcschr(szTemp, L':');
	if (p1)
	{   // Skip colon
		p1 = _wcsinc(p1);
		wchar_t temp = *p1;
		*p1 = 0;
		// Save the protocol
		strProtocol = szTemp;
		*p1 = temp;
		// Now skip leading slashes
		while(*p1 == L'/')
		{
			p1 = _wcsinc(p1);
		}
		// p1 now points to the server part
		wchar_t* pszTemp = p1;
		// Search for a port colon
		p1 = wcschr(p1, L':');
		if(p1)
		{
			*p1 = 0;
			strServer = pszTemp;
			// Search for next slash - end of port
			p1 =_tcsinc(p1);
			pszTemp = p1;
			p1 = wcschr(p1, L'/');
			if(p1)
			{
				*p1 = 0;
				strPort = pszTemp;
			}
		}
		else
		{   // No port. Find next slash and terminate. If no trailing slash
			// we are done.
			p1 = wcschr(pszTemp, L'/');
			if(p1)
				*p1 = 0;
			strServer = pszTemp;
		}
		if(p1)
		{   // Skip the slash, remainder is the webpath
			p1 =_wcsinc(p1);
			strWebPath = p1;
		}
	}
}

//-----------------------------------------------------------------------------
// Converts all forward slashes to backslashes in pBuff.
//-----------------------------------------------------------------------------
static void UrlSlashToPathSlash(LPOLESTR pBuff)
{
	if (!pBuff)
		return;

	while(*pBuff != 0)
	{
		if(*pBuff == L'/')
			*pBuff = L'\\';
		pBuff++;	// no MBCS to worry about
	}
}

//-----------------------------------------------------------------------------
// Given a path and a filename, concatanates the two correctly (handles trailing
// nontrailing slash in folder name).  If strDir is empty, this function just
// returns whatever is in pszfilename
//-----------------------------------------------------------------------------
static void BuildPath(CStringW& strDir, LPCOLESTR pszFilename)
{
	if(strDir.IsEmpty())
	{
		strDir = pszFilename;
	}
	else if(strDir[strDir.GetLength()-1] != L'\\' && pszFilename[0] != L'\\')
	{   // Folder does not have a trailing slash and filename does not have a leading slash
		strDir += L'\\';
		strDir += pszFilename;
	}
	else if(strDir[strDir.GetLength()-1] == L'\\' && pszFilename[0] == L'\\')
	{   // Folder has trailing slash and filename has a leading slash  we need to remove 
		// one of them
		strDir += &pszFilename[1];
	}
	else
	{   // Either Folder has trailing slash or filename has leading slash but not both
		strDir += pszFilename;
	}
}

//-----------------------------------------------------------------------------
// Replaces "LocalHost:port//rest of url" with "computername:port//rest of url". Note that
// url cannot have a leading protocol. It can have a trailing url, however
//-----------------------------------------------------------------------------
static void ReplaceLocalHost(CStringW& strServerName)
{
	// Replace "LocalHost" with the actual machine name if localhost is specified
	if(_wcsnicmp(LOCALHOST, strServerName, LOCALHOST_CCH) == 0)
	{	// The next char needs to be a ':' or a '\0'  or a '/' for a match
		// CString asserts if you try to index the null terminator - I think it's a bug....
		// So to get around that, we get the buffer
		LPCWSTR pszServerName = strServerName;
		if(pszServerName[LOCALHOST_CCH] == L'/' || pszServerName[LOCALHOST_CCH] == L':' || pszServerName[LOCALHOST_CCH] == L'\0') 
		{   // Get our computer name. If it fails for some reason, we stick to localhost
			CHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
			DWORD dwSize = sizeof(szComputerName);
			if(::GetComputerNameA(szComputerName, &dwSize))
			{
				CStringW strTemp = szComputerName;
				strTemp += strServerName.Right(strServerName.GetLength() - LOCALHOST_CCH);
				strServerName = strTemp;
			}
		}
	}
}

//---------------------------------------------------------------------------
// Validates an IP address.
//---------------------------------------------------------------------------
static BOOL FIsIPAddr(CStringW& strDNSName)
{
    // string must be only numbers and digits
    CStringW strSubstring;
    strSubstring = strDNSName.SpanIncluding(L"0123456789.");
    if (strSubstring != strDNSName)
		return FALSE;

    // now we need to see exactly 3 periods
    int cPeriods = 0;   // number of .'s seen
    int cchDNSName = strDNSName.GetLength();
    for (int i = 0; i < cchDNSName; i++)
    {
        if ( strDNSName.GetAt(i) == L'.')
            cPeriods++;
    }

    return (cPeriods == 3);
}

//---------------------------------------------------------------------------
// Helper function to find the server's name, port and ipaddr for a project
//---------------------------------------------------------------------------
static HRESULT GetServerDescriptors(LPCWSTR pszWebUrl,     // IN: full web url
                          BOOL *pbIsRootDir,         // OUT: set to true if this project is at a root
                          CStringW &strServerDNSName,// OUT: server name
                          CStringW &strAdsWebDirPath,// OUT: /Root/webdir - ADS path to web directory
                          CStringW &strPortNum,      // OUT: port server is on
                          CStringW &strIPAddr)       // OUT server ip addr
{
	USES_CONVERSION;
	HRESULT hr = E_FAIL;
	CStringW strTemp;
	CStringW strWebPath, strProtocol;
	CStringW strWork;

	// Parse ServerName string into server, port number and folder
	SplitUrl2(pszWebUrl, strProtocol, strServerDNSName, strPortNum, strWebPath);
	*pbIsRootDir = strWebPath.IsEmpty();
	strAdsWebDirPath = L"/Root";
	if (!*pbIsRootDir) 
	{
		strAdsWebDirPath += L"/";
		strAdsWebDirPath += strWebPath;
	}   

	// If no port specified, set default
	if(strPortNum.IsEmpty())
	{
		if(_wcsicmp(strProtocol, L"http:") == 0) 
			strPortNum = L"80";
		else
		{
			VSASSERT(_wcsicmp(strProtocol, L"https:") == 0, "Bad protocol!");
			strPortNum = L"443";
		}
	}
	// replace local host with machine name
	ReplaceLocalHost(strServerDNSName);

	// Determine the server machine's IP address
	if (FIsIPAddr(strServerDNSName))
	{
		strIPAddr = strServerDNSName;
		hr = S_OK;
	}
	else
	{   // We use windows sockets to get the ip address
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1,1);
		if (::WSAStartup(wVersionRequested, &wsaData) == 0) 
		{
			HOSTENT *pHostEnt;
			pHostEnt = ::gethostbyname(W2A(strServerDNSName));
			if(pHostEnt)
			{
				struct in_addr addr;
				memcpy (&addr, pHostEnt->h_addr_list[0], sizeof(addr));
				strIPAddr = A2W(::inet_ntoa(addr));
				hr = S_OK;
			}
			// Must cleanup the sockets
			WSACleanup();
		}
	}
	return hr;
}

//---------------------------------------------------------------------------
// Creates binding strings for IIS
//---------------------------------------------------------------------------
static void CreateIISBindings(LPCWSTR pszPortNum,  // IN: port for server
                      LPCWSTR pszIPAddr,   // IN: server ipaddr
                      CStringW &strIISBindingsGenericMatch,   // OUT: generic k2 binding string
                      CStringW &strIISBindingsBestMatch)      // OUT: best binding
{
	strIISBindingsGenericMatch = L":";
	strIISBindingsGenericMatch += pszPortNum;
	strIISBindingsGenericMatch += L":";
	strIISBindingsBestMatch = pszIPAddr + strIISBindingsGenericMatch;
}

//---------------------------------------------------------------------------
// Create path of the ads root
//---------------------------------------------------------------------------
static void CreateADSRootPath(LPCWSTR pszServerDNSName,   // IN: server name
                       CString &strADSRootPath) // OUT: path to root
{
	// Open an ADSI DCOM connection to the server.
	strADSRootPath = L"IIS://";
	strADSRootPath += pszServerDNSName;
	strADSRootPath += "/w3svc";
}

