#include "stdafx.h"

#include <ocdesign.h>
#include <shldocs_.h>

#include "bwizmgr.h"
#include "bwizdlg.h"
#include "utilauto.h"		// for app's automation object
#include "shell.h"			// szKeyRoot && szCompanyName strings(initexit.cpp)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBuilderWizardManager, CCmdTarget)

BEGIN_INTERFACE_MAP(CBuilderWizardManager, CCmdTarget)
	INTERFACE_PART(CBuilderWizardManager, IID_IBuilderWizardManager, BuilderWizardManager)
END_INTERFACE_MAP()

// static strings
TCHAR *CBuilderWizardManager::szSysCompCatKey = 
		_T("Component Categories");

static LPCTSTR s_szSlash		= _T("\\");


///////////////////////////////////////////////////////////////////////////////
// CBuilderWizardManager
//
CBuilderWizardManager::CBuilderWizardManager()
{
}

CBuilderWizardManager::~CBuilderWizardManager()
{
}

BOOL CBuilderWizardManager::DoesDevCategoryExist(
	LPCTSTR szCategory, HKEY *pKeyOut /* = NULL*/)
{
	ASSERT(NULL != szCategory);
	if(NULL == szCategory)
		return FALSE;

	if(NULL != pKeyOut)
		*pKeyOut = NULL;
	
	HKEY hKey = NULL;
	///////////////////////////////////
	// must build this key ourselves.  we can not use theApp.GetRegistryKeyName()
	// as this builds a key that takes into account the /i switch.  since these
	// settings are in HKLM and /i is an HKCU setting, we would end up with 
	// a bogus HKLM path.
	CString strKey;
	strKey =  g_szKeyRoot; 
	strKey += s_szSlash;
	strKey += g_szCompanyName;
	strKey += s_szSlash;
	strKey += theApp.GetExeString(DefaultRegKeyName);
	strKey += s_szSlash;
	strKey += szSysCompCatKey;
	strKey += s_szSlash;
	strKey += szCategory;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		strKey, 0, KEY_READ, &hKey))
	{
		TRACE("DoesDevCatExist failed- Could not open reg key\n");
		return FALSE;
	}

	ASSERT(NULL != hKey);
	if(NULL != pKeyOut)
		*pKeyOut = hKey;
	else
		::CloseHandle(hKey);
	return TRUE;
}

#if _DEBUG
// enusure that the system compcat registry entries
// have been properly setup
BOOL CBuilderWizardManager::DoesSysCategoryExist(LPCTSTR szCategory)
{
	ASSERT(NULL != szCategory);
	if(NULL == szCategory)
		return FALSE;

	HKEY hKey = NULL;
	CString strKey;

	strKey = szSysCompCatKey;
	strKey += s_szSlash;
	strKey += szCategory;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CLASSES_ROOT, 
		strKey, 0, KEY_READ, &hKey))
	{
		TRACE("DoesSysCatExist failed- Could not open reg key\n");
		return FALSE;
	}

	ASSERT(NULL != hKey);
	::CloseHandle(hKey);
	return TRUE;
}
#endif

BOOL CBuilderWizardManager::GetDefaultCompCatCLSID(
	HKEY hCATIDKey, 
	CLSID *pclsidBuilder)
{
	USES_CONVERSION;

	ASSERT(NULL != hCATIDKey);
	ASSERT(NULL != pclsidBuilder);
	if(NULL == hCATIDKey)
		return FALSE;

	HKEY hSubKey;
	CString strKey;
	//strKey.LoadString(
	strKey = _T("Default Component\\CLSID");
	if(ERROR_SUCCESS != ::RegOpenKeyEx(hCATIDKey, strKey, 0, KEY_READ, &hSubKey))
		return FALSE;

	DWORD nType;
	DWORD cbData = 0;
	// get the size of the string
	if(ERROR_SUCCESS != ::RegQueryValueEx(hSubKey, 
		NULL, NULL, &nType, NULL, &cbData))
	{
		TRACE("GetDefaultCompCatCLSID failed to get size of the RegValue\n");
		::CloseHandle(hSubKey);	
		return FALSE;
	}
	ASSERT(REG_SZ == nType);

	// grab some heap and get the value
	cbData += sizeof(TCHAR);
	TCHAR *pValue = new TCHAR[cbData];
	if(ERROR_SUCCESS != ::RegQueryValueEx(hSubKey, NULL, NULL, 
		&nType, (LPBYTE)pValue, &cbData))
	{
		TRACE("GetDefaultCompCatCLSID failed to get Registry Value\n");
		delete [] pValue;
		::CloseHandle(hSubKey);	
		return FALSE;
	}
	// convert the string to a clsid
	HRESULT hr = ::CLSIDFromString(T2OLE(pValue), pclsidBuilder);
	delete [] pValue;
	pValue = NULL;
	::CloseHandle(hSubKey);	
	hSubKey = NULL;
	return SUCCEEDED(hr);
}

BOOL CBuilderWizardManager::GetCompCatCLSID(
	HKEY hCATIDKey, 
	DWORD nKeyNumber, 
	CLSID *pclsidBuilder)
{
	ASSERT(NULL != hCATIDKey);
	ASSERT(NULL != pclsidBuilder);
	if(NULL == hCATIDKey)
		return FALSE;

	USES_CONVERSION;

	DWORD cSubKeys = 0;
	DWORD cbMaxSubKeyLen = 0;
	HRESULT hr;
	if(ERROR_SUCCESS == ::RegQueryInfoKey(hCATIDKey, NULL, NULL, NULL, 
			&cSubKeys, &cbMaxSubKeyLen,	NULL, NULL, NULL, NULL, NULL, NULL))
	{
		HKEY hSubKey;
		CBWizEnumItem *pItem = NULL;
		TCHAR *pszSubKeyName = new TCHAR[++cbMaxSubKeyLen];

		// if we can't enum the key, or we got back the 
		// default component key, we're in error
		if(ERROR_SUCCESS != ::RegEnumKeyEx(hCATIDKey, nKeyNumber, 
			pszSubKeyName, &cbMaxSubKeyLen, NULL, NULL, NULL, NULL) ||
			!lstrcmp(pszSubKeyName, _T("Default Component")))
		{
#if defined _DEBUG
			CString strDebug;
			strDebug.Format(_T("GetCompCatCLSID Error(%s!= Default Component\r\n"), pszSubKeyName);
			::AfxMessageBox(strDebug);
#endif
			hr = S_FALSE;
		}
		else
			hr = ::CLSIDFromString(T2OLE(pszSubKeyName), pclsidBuilder);
		delete [] pszSubKeyName;
	}	
	return (hr == S_OK ? TRUE : FALSE);
}

STDMETHODIMP CBuilderWizardManager::XBuilderWizardManager::DoesBuilderExist(
	REFGUID rguidBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	ASSERT_VALID(pThis);

	USES_CONVERSION;

	// validate parameters
	ASSERT(CLSID_NULL != rguidBuilder);
	if(CLSID_NULL == rguidBuilder)
		return E_INVALIDARG;

	// give the package a shot a providing the builder
	if(S_OK == theApp.DoesIntrinsicBuilderExist(rguidBuilder))
		return S_OK;

	// convert the object's clsid to a string
	LPOLESTR szGUID = NULL;
	::StringFromCLSID(rguidBuilder, &szGUID);
	CString strGUID = OLE2T(szGUID);
	AfxFreeTaskMem(szGUID);
	szGUID = NULL;

	if(pThis->DoesDevCategoryExist(strGUID))
		return S_OK;

	// build the key string
	CString strCLSIDKey = _T("CLSID\\");
	strCLSIDKey += strGUID;
	strCLSIDKey += _T("\\Implemented Categories");
	// look for this under classes root and see
	// if it implements any compcats.  get the
	// compcats is implements and see if they
	// exist under our dev key

	HKEY hCLSIDKey;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CLASSES_ROOT, 
		strCLSIDKey, 0, KEY_READ, &hCLSIDKey))
		return S_FALSE;

	// figure out how may categories this key implements
	DWORD cSubKeys = 0;
	DWORD cbMaxSubKeyLen = 0;
	if(ERROR_SUCCESS != ::RegQueryInfoKey(hCLSIDKey, NULL, 
		NULL, NULL, &cSubKeys, &cbMaxSubKeyLen,	NULL, 
		NULL, NULL, NULL, NULL, NULL))
	{
		TRACE("DoesBuilderExist: key does not implement any cateogories\n");
		::CloseHandle(hCLSIDKey);
		return S_FALSE;
	}

	// walk each key and look to see if its supported 
	// under out dev key
	DWORD nUnused;
	TCHAR *pszSubKeyName = new TCHAR[++cbMaxSubKeyLen];
	HRESULT hr = S_FALSE;
	for(DWORD nCnt = 0; nCnt < cSubKeys; ++nCnt)
	{
		nUnused = cbMaxSubKeyLen;
		// if we can't enum the key, don't do anything
		if(ERROR_SUCCESS != ::RegEnumKeyEx(hCLSIDKey, nCnt, pszSubKeyName, 
			&nUnused, NULL, NULL, NULL, NULL))
		{
			ASSERT(!_T("Could not Enum Key"));
			continue;
		}

		if(pThis->DoesDevCategoryExist(pszSubKeyName))
		{
			hr = S_OK;
			break;
		}
	}
	delete [] pszSubKeyName;
	::CloseHandle(hCLSIDKey);	
	return hr;
}

STDMETHODIMP CBuilderWizardManager::XBuilderWizardManager::MapObjectToBuilderCLSID(
	REFCLSID rclsidObject, 
	DWORD dwPromptOpt,
	HWND hwndOwner, 
	CLSID *pclsidBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	ASSERT_VALID(pThis);

	USES_CONVERSION;

	// validate parameters
	ASSERT(CLSID_NULL != rclsidObject);
	ASSERT(NULL != pclsidBuilder);
	ASSERT(BLDPROMPTOPT_PROMPTIFMULTIPLE == dwPromptOpt ||
			BLDPROMPTOPT_PROMPTALWAYS == dwPromptOpt ||
			BLDPROMPTOPT_PROMPTNEVER == dwPromptOpt);

	if(CLSID_NULL == rclsidObject || 
		NULL == pclsidBuilder || 
		(BLDPROMPTOPT_PROMPTIFMULTIPLE != dwPromptOpt &&
		BLDPROMPTOPT_PROMPTALWAYS != dwPromptOpt &&
		BLDPROMPTOPT_PROMPTNEVER != dwPromptOpt))
		return E_INVALIDARG;

	// if there's a possibility of prompting
	// make sure the hwndOwner is valid
	if(BLDPROMPTOPT_PROMPTNEVER != dwPromptOpt 
		&& !::IsWindow(hwndOwner))
	{
		hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	}

	// convert the object's clsid to a string
	LPOLESTR szCLSID = NULL;
	::StringFromCLSID(rclsidObject, &szCLSID);
	CString strCLSID = OLE2T(szCLSID);
	AfxFreeTaskMem(szCLSID);
	szCLSID = NULL;

	// build the full clsid key
	HKEY hCLSIDKey = NULL;
	CString strCLSIDKey = _T("CLSID\\");;
	strCLSIDKey += strCLSID;
	strCLSIDKey += _T("\\ObjectBuilderWizard");

	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CLASSES_ROOT, 
		strCLSIDKey, 0, KEY_READ, &hCLSIDKey))
	{
		TRACE("Failed to Open HKEY_CLASSES_ROOT\\{clsid}\\ObjectBuilderWizard\n");
		return REGDB_E_KEYMISSING;
	}

	DWORD nType;
	DWORD cbData = 0;
	// get the size of the string
	if(ERROR_SUCCESS != ::RegQueryValueEx(hCLSIDKey, 
		NULL, NULL, &nType, NULL, &cbData))
	{
		TRACE("RegQueryValueEx for size of Reg Data failed\n");
		::CloseHandle(hCLSIDKey);	
		return REGDB_E_READREGDB;
	}
	ASSERT(REG_SZ == nType);

	// grab some heap and get the value
	cbData += sizeof(TCHAR);
	TCHAR *pValue = new TCHAR[cbData];
	if(ERROR_SUCCESS != ::RegQueryValueEx(hCLSIDKey, 
		NULL, NULL, &nType, (LPBYTE)pValue, &cbData))
	{
		TRACE("RegQueryValueEx for Reg Data failed\n");
		delete [] pValue;
		::CloseHandle(hCLSIDKey);	
		return REGDB_E_READREGDB;
	}
	// convert the string to a clsid
	GUID guidBuilder;
	HRESULT hr = ::CLSIDFromString(T2OLE(pValue), &guidBuilder);

	if(SUCCEEDED(hr))
	{
		// OK to reuse the CString again
		strCLSIDKey = _T("CLSID\\");
		strCLSIDKey += T2OLE(pValue);

		// optimize for the case when the control has a specific
		// CLSID as its builder and is not using a CATID
		HKEY hNotUsed = NULL;
		if(ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CLASSES_ROOT, 
								strCLSIDKey, 0, KEY_READ, &hNotUsed))
		{
			// the control has a CLSID registered as its builder
			// no need to treat it as a CATID, lets return it
			// and get out of here
			*pclsidBuilder = guidBuilder;
			::CloseHandle(hNotUsed);
			hr = S_OK;
		}
		else
			hr = MapBuilderCATIDToCLSID(guidBuilder, dwPromptOpt, 
										hwndOwner, pclsidBuilder);
	}

	::CloseHandle(hCLSIDKey);
	delete [] pValue;
	pValue = NULL;
	return hr;
}

STDMETHODIMP CBuilderWizardManager::XBuilderWizardManager::MapBuilderCATIDToCLSID(
	REFGUID rguidBuilder, 
	DWORD dwPromptOpt,
	HWND hwndOwner, 
	CLSID *pclsidBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	ASSERT_VALID(pThis);

	USES_CONVERSION;

	// validate parameters
	ASSERT(CLSID_NULL != rguidBuilder);
	ASSERT(NULL != pclsidBuilder);
	ASSERT(BLDPROMPTOPT_PROMPTIFMULTIPLE == dwPromptOpt ||
			BLDPROMPTOPT_PROMPTALWAYS == dwPromptOpt ||
			BLDPROMPTOPT_PROMPTNEVER == dwPromptOpt);

	*pclsidBuilder = CLSID_NULL;
	if(CLSID_NULL == rguidBuilder || 
		NULL == pclsidBuilder || 
		(BLDPROMPTOPT_PROMPTIFMULTIPLE != dwPromptOpt &&
		BLDPROMPTOPT_PROMPTALWAYS != dwPromptOpt &&
		BLDPROMPTOPT_PROMPTNEVER != dwPromptOpt))
		return E_INVALIDARG;

	// if there's a possibility of prompting
	// make sure the hwndOwner is valid
	if(BLDPROMPTOPT_PROMPTNEVER != dwPromptOpt && !::IsWindow(hwndOwner))
		hwndOwner = AfxGetMainWnd()->GetSafeHwnd();

	// convert the catid to a string
	HKEY hDevRootCompCatKey = NULL;
	LPOLESTR szCATID = NULL;
	::StringFromCLSID(rguidBuilder, &szCATID);
	CString strCATID = OLE2T(szCATID);
	AfxFreeTaskMem(szCATID);
	szCATID = NULL;
	HKEY hCATIDKey = NULL;

	///////////////////////////////////
	// must build this key ourselves.  we can not use theApp.GetRegistryKeyName()
	// as this builds a key that takes into account the /i switch.  since these
	// settings are in HKLM and /i is an HKCU setting, we would end up with 
	// a bogus HKLM path.
	CString strKey;
	strKey =  g_szKeyRoot;
	strKey += s_szSlash;
	strKey += g_szCompanyName;
	strKey += s_szSlash;
	strKey += theApp.GetExeString(DefaultRegKeyName);
	strKey += s_szSlash;
	strKey += szSysCompCatKey;

	// see if our compcat key exists
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
							strKey, 0, KEY_READ, &hDevRootCompCatKey) ||
#if _DEBUG
// enusure that the system compcat registry entries
// have been properly setup
		!pThis->DoesSysCategoryExist(strCATID) ||
#endif
		// ensure that CATID exists in developer hive
		!pThis->DoesDevCategoryExist(strCATID, &hCATIDKey))
	{
		TRACE0("DoesDevCategoryExist Failed\n");
		if(hDevRootCompCatKey)
			::CloseHandle(hDevRootCompCatKey);
		return REGDB_E_KEYMISSING;
	}
	::CloseHandle(hDevRootCompCatKey);	
	
	// More Parameter validation: on Meta CATID
	BOOL fMetaCATID;
	if((fMetaCATID = IsMetaCATID(hCATIDKey)) && 
		(BLDPROMPTOPT_PROMPTNEVER == dwPromptOpt))
	{
		//Need PROMPTING on a Meta CATID
		::CloseHandle(hCATIDKey);
		return E_INVALIDARG;
	}
	
	// figure out how may sub keys this catid has
	// skipped if MetaCATID
	DWORD cSubKeys = 0;
	if(!fMetaCATID && 
		ERROR_SUCCESS != ::RegQueryInfoKey(hCATIDKey, NULL, 
									NULL, NULL, &cSubKeys, NULL, NULL, NULL, 
									NULL, NULL, NULL, NULL))
	{
		TRACE("RegQueryInfoKey for # of sub keys failed\n");
		::CloseHandle(hCATIDKey);
		return REGDB_E_READREGDB;
	}

	HRESULT hr = S_OK;
	// never prompt, grab the default
	// skipped if METACATID
	if(!fMetaCATID && (BLDPROMPTOPT_PROMPTNEVER == dwPromptOpt || 
		(1 == cSubKeys && BLDPROMPTOPT_PROMPTALWAYS != dwPromptOpt)))
	{
		// if we have more than one subkey, see if we
		// have a default
		if(cSubKeys > 1)
		{
			pThis->GetDefaultCompCatCLSID(hCATIDKey, pclsidBuilder);
		}

		// either we only have 1 subkey or have multiple but no
		// default has been defined
		if(CLSID_NULL == *pclsidBuilder &&
			!pThis->GetCompCatCLSID(hCATIDKey, 0, pclsidBuilder))
		{
			// we don't have a clsid
			*pclsidBuilder = CLSID_NULL;	
			hr = REGDB_E_KEYMISSING;
		}
	}
	else 
	{
		// if we have a prompt if multipe case, see if we have a default
		// key set, if so , we take this and give no dlg ui
		// skipped if MetaCATID
		if(!fMetaCATID && BLDPROMPTOPT_PROMPTIFMULTIPLE == dwPromptOpt)
		{
			pThis->GetDefaultCompCatCLSID(hCATIDKey, pclsidBuilder);
		}

		// either promp always or we didn't have a default
		// or we have a MetaCATID 
		if(fMetaCATID || CLSID_NULL == *pclsidBuilder)
		{
			CWnd wnd;		// for modal dialog happiness
			CWnd* pWnd = CWnd::FromHandlePermanent(hwndOwner);
			if(NULL == pWnd)
			{
				wnd.Attach(hwndOwner);
				pWnd = &wnd;
			}
			// prompt with the enumerations dialog
			CBldWizSelectDlg dlg(pWnd);
			if(dlg.SetCATID(hCATIDKey) && IDOK == dlg.DoModal() &&
					CLSID_NULL != dlg.m_clsidSel)
			{
				//get select builder's clsid
				*pclsidBuilder = dlg.m_clsidSel;
			}
			else
				// User canceled the dialog so return S_FALSE
				hr = S_FALSE;
			// IStudio bug # 5808
			// force the window that parented the dialog to repaint itself now.
			pWnd->UpdateWindow();
				
			if(pWnd == &wnd)
				wnd.Detach();
		}
		else
			hr = S_OK;		// we got the default
	}
	::CloseHandle(hCATIDKey);
	return hr;
}

STDMETHODIMP CBuilderWizardManager::XBuilderWizardManager::GetBuilder(
	REFGUID rguidBuilder, 
	DWORD grfGetOpt, 
	HWND hwndPromptOwner, 
	IDispatch **ppdispApp,
	HWND *pwndBuilderOwner, 
	REFIID riidBuilder,
	IUnknown **ppunkBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	ASSERT_VALID(pThis);

	ASSERT(CLSID_NULL != rguidBuilder);
	ASSERT(NULL != ppunkBuilder);
	if(CLSID_NULL == rguidBuilder ||
		NULL == ppunkBuilder)
		return E_INVALIDARG;

	if(NULL != ppdispApp)
		*ppdispApp = NULL;
	if(NULL != pwndBuilderOwner)
		*pwndBuilderOwner = NULL;
	*ppunkBuilder = NULL;

	GUID guidPrivateBuilder;
	// we are being asked to map the GUID
	if((grfGetOpt & BLDGETOPT_FAUTOMAPGUID))
	{
		DWORD dwMapFlag = BLDPROMPTOPT_PROMPTNEVER;
		// we've been asked to perform the mapping
		if(grfGetOpt & BLDGETOPT_FAUTOMAPENABLEPROMPT)
		{
			// make sure we have a valid hwnd
			if(!::IsWindow(hwndPromptOwner))
				hwndPromptOwner = AfxGetMainWnd()->GetSafeHwnd();

			// determime if its a prompt always
			// or a prompt if multiple
			if(grfGetOpt & BLDGETOPT_FAUTOMAPPROMPTALWAYS)
				dwMapFlag = BLDPROMPTOPT_PROMPTALWAYS;
			else
				dwMapFlag = BLDPROMPTOPT_PROMPTIFMULTIPLE;
		}

		// Call MapBuilder to get the guid of the builder.
		// If MapBuilder returns S_FALSE, then user has canceled the operation.
		// If MapBuilder returns an error without Prompting then we will check 
		// for an instrinsic builder.  If MapBuilder fails with prompting then 
		// there is no reason to continue.
		HRESULT hr = MapBuilderCATIDToCLSID(rguidBuilder, dwMapFlag, 
										hwndPromptOwner, &guidPrivateBuilder);
		// Check for canceled 
		if(hr == S_FALSE)
			// User has cancelled GetBuilder, so return S_FALSE
			return S_FALSE;
			
		// Check for map failure
		if(FAILED(hr))
		{
			if(grfGetOpt & BLDGETOPT_FAUTOMAPENABLEPROMPT)
				// Map Failed, with the PROMPT flag, so dont continue
				return hr;
			else
				// Map Failed, without the PROMPT flag.
				// Check if rguidBuilder is a builder, or it is an
				// intrisinic builder.
				guidPrivateBuilder = rguidBuilder;
		}
	}
	else
		guidPrivateBuilder = rguidBuilder;
	HRESULT hr = ::CoCreateInstance(guidPrivateBuilder, NULL, 
					CLSCTX_INPROC_SERVER, riidBuilder, 
					(void**)ppunkBuilder);

	// if we've failed, give the packages a shot
	if(FAILED(hr) && !(grfGetOpt & BLDGETOPT_FNOINTRINSICS))
	{
#if defined _DEBUG
		HRESULT hrIntrinsic = theApp.GetIntrinsicBuilder(rguidBuilder, grfGetOpt, 
													hwndPromptOwner, ppdispApp,
													pwndBuilderOwner, riidBuilder, 
													ppunkBuilder);
		// Display both error messages
		if(FAILED(hrIntrinsic))
		{
			USES_CONVERSION;
			// convert the object's clsid to a string
			LPOLESTR	 pszBuilder;
			LPOLESTR	 pszPrivateBuilder;
			::StringFromCLSID(rguidBuilder, &pszBuilder);
			::StringFromCLSID(guidPrivateBuilder, &pszPrivateBuilder);
			CString strDebug;
			strDebug.Format(_T("GetBuilder Failed\r\n")\
								 _T("rguidBuilder(%s)\r\n")\
								 _T("rguidPrivateBuilder(%s)\r\n")\
								 _T("GetIntrinsicBuilder()=%08X\r\n")\
								 _T("CoCreateInstance()=%08X)"), 
								 OLE2T(pszBuilder), OLE2T(pszPrivateBuilder), 
								 hrIntrinsic, hr);
			::AfxFreeTaskMem(pszBuilder);
			::AfxFreeTaskMem(pszPrivateBuilder);
			::AfxMessageBox(strDebug);
		}
		return hrIntrinsic;
#else	
		// see if this is an intrinsic builder
		// offered up by one of the loaded packages
		return theApp.GetIntrinsicBuilder(rguidBuilder, grfGetOpt, 
								hwndPromptOwner, ppdispApp,
								pwndBuilderOwner, riidBuilder, 
								ppunkBuilder);
#endif								
	}
	else
	{
		if(NULL != pwndBuilderOwner)
		{
			// fix IStudio bug# 4782
			// can't assume that hwndPromptOwner is valid
			if(::IsWindow(hwndPromptOwner))
				*pwndBuilderOwner = hwndPromptOwner;
			else
				*pwndBuilderOwner = AfxGetMainWnd()->GetSafeHwnd();
			ASSERT(NULL != *pwndBuilderOwner && ::IsWindow(*pwndBuilderOwner));
		}


		// give them the app object if they want it
		if(NULL != ppdispApp)
		{
			COleRef<IDispatch> srpDisp;
			ASSERT(NULL != theApp.m_pAutoApp);
			HRESULT hr = theApp.m_pAutoApp->ExternalQueryInterface(
											&IID_IDispatch, (void**)&srpDisp);
			ASSERT(SUCCEEDED(hr));
			*ppdispApp = srpDisp.Disown();
		}
		return hr;
	}
}

STDMETHODIMP CBuilderWizardManager::XBuilderWizardManager::EnableModeless(
	BOOL fEnable)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	ASSERT_VALID(pThis);

	// Disabling the main frame window will manage modality with
	// the component manager.
	AfxGetMainWnd()->EnableWindow(fEnable);
	return NOERROR;	
}

STDMETHODIMP_(ULONG) CBuilderWizardManager::XBuilderWizardManager::AddRef()
{
	// Delegate to our exported AddRef.

	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBuilderWizardManager::XBuilderWizardManager::Release()
{
	// Delegate to our exported Release.

	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CBuilderWizardManager::XBuilderWizardManager::QueryInterface(
	REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.

	METHOD_PROLOGUE_EX(CBuilderWizardManager, BuilderWizardManager)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}



IMPLEMENT_DYNAMIC(CIPCompBuilderWizardManager, CCmdTarget)

BEGIN_INTERFACE_MAP(CIPCompBuilderWizardManager, CCmdTarget)
	INTERFACE_PART(CIPCompBuilderWizardManager, 
					IID_IBuilderWizardManager, IPCompBuilderWizardManager)
END_INTERFACE_MAP()

///////////////////////////////////////////////////////////////////////////////
// CIPCompBuilderWizardManager
//
CIPCompBuilderWizardManager::CIPCompBuilderWizardManager(
	CIPCompContainerItem *pItem)
{
	ASSERT(NULL != pItem);
	m_pContainerItem = pItem;
}

CIPCompBuilderWizardManager::~CIPCompBuilderWizardManager()
{
	ASSERT(0 == m_dwRef);
	m_pContainerItem = NULL;
}


STDMETHODIMP CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::DoesBuilderExist(
	REFGUID rguidBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
	ASSERT_VALID(pThis);

	// validate parameters
	ASSERT(CLSID_NULL != rguidBuilder);
	if(CLSID_NULL == rguidBuilder)
		return E_INVALIDARG;

	ASSERT(NULL != pThis->m_pContainerItem);
	CIPCompDoc *pDoc = pThis->m_pContainerItem->GetDocument();
	ASSERT(NULL != pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)));

	HRESULT hr = pDoc->DoesBuilderExist(rguidBuilder);
	if(FAILED(hr))
	{
		ASSERT(pThis->m_srpBuilderWizardManager != NULL);
		return pThis->m_srpBuilderWizardManager->DoesBuilderExist(rguidBuilder);
	}
	return hr;
}

STDMETHODIMP CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::MapObjectToBuilderCLSID(
	REFCLSID rclsidObject, 
	DWORD dwPromptOpt,
	HWND hwndOwner, 
	CLSID *pclsidBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
	ASSERT_VALID(pThis);

	ASSERT(CLSID_NULL != rclsidObject);
	if(CLSID_NULL == rclsidObject)
		return E_INVALIDARG;

	ASSERT(pThis->m_srpBuilderWizardManager != NULL);
	return pThis->m_srpBuilderWizardManager->MapObjectToBuilderCLSID(rclsidObject, 
							dwPromptOpt, hwndOwner,	pclsidBuilder);
}

STDMETHODIMP CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::MapBuilderCATIDToCLSID(
	REFGUID rguidBuilder, 
	DWORD dwPromptOpt,
	HWND hwndOwner, 
	CLSID *pclsidBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
	ASSERT_VALID(pThis);

	ASSERT(CLSID_NULL != rguidBuilder);
	if(CLSID_NULL == rguidBuilder)
		return E_INVALIDARG;

	ASSERT(pThis->m_srpBuilderWizardManager != NULL);
	return pThis->m_srpBuilderWizardManager->MapBuilderCATIDToCLSID(rguidBuilder, 
						dwPromptOpt, hwndOwner,	pclsidBuilder);
}

STDMETHODIMP CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::GetBuilder(
	REFGUID rguidBuilder, 
	DWORD grfGetOpt, 
	HWND hwndPromptOwner, 
	IDispatch **ppdispApp,
	HWND *pwndBuilderOwner, 
	REFIID riidBuilder,
	IUnknown **ppunkBuilder)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
	ASSERT_VALID(pThis);

	ASSERT(CLSID_NULL != rguidBuilder);
	ASSERT(NULL != ppunkBuilder);
	if(CLSID_NULL == rguidBuilder || NULL == ppunkBuilder)
		return E_INVALIDARG;

	// the app's implementation should never 
	// get called with this bit set
	ASSERT(!(grfGetOpt & BLDGETOPT_FOBJECTBUILDER));
	ASSERT(NULL != pThis->m_pContainerItem);
	CIPCompDoc *pDoc = pThis->m_pContainerItem->GetDocument();
	ASSERT(NULL != pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)));

	HRESULT hr = pDoc->GetBuilder(rguidBuilder, grfGetOpt, 
							hwndPromptOwner, ppdispApp, 
							pwndBuilderOwner, riidBuilder, 
							ppunkBuilder);

	if(FAILED(hr))
	{
		ASSERT(pThis->m_srpBuilderWizardManager != NULL);
		return pThis->m_srpBuilderWizardManager->GetBuilder(rguidBuilder, 
						grfGetOpt, hwndPromptOwner, ppdispApp, 
						pwndBuilderOwner, riidBuilder, ppunkBuilder);
	}
	return hr;
}

STDMETHODIMP CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::EnableModeless(
	BOOL fEnable)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
	ASSERT_VALID(pThis);

	ASSERT(pThis->m_srpBuilderWizardManager != NULL);
	pThis->m_srpBuilderWizardManager->EnableModeless(fEnable);
	return NOERROR;	
}



STDMETHODIMP_(ULONG) CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::AddRef()
{
	// Delegate to our exported AddRef.

	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::Release()
{
	// Delegate to our exported Release.

	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CIPCompBuilderWizardManager::XIPCompBuilderWizardManager::QueryInterface(
	REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.

	METHOD_PROLOGUE_EX(CIPCompBuilderWizardManager, IPCompBuilderWizardManager)

	if(IID_IBuilderWizardManager == iid && 
		pThis->m_srpBuilderWizardManager == NULL)
	{
		HRESULT hr = theApp.GetService(SID_SBuilderWizardManager, 
							IID_IBuilderWizardManager, 
							(void**)&(pThis->m_srpBuilderWizardManager));
		ASSERT(SUCCEEDED(hr));
	}
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

