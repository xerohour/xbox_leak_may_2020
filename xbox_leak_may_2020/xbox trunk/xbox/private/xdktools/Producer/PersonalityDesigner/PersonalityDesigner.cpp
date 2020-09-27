// PersonalityDesigner.cpp : Implementation of CPersonalityDesignerApp and DLL registration.

#include "stdafx.h"
#pragma warning(disable:4201)
#include <initguid.h>
#include "PersonalityDesigner.h"
#include "PersonalityComponent.h"
#include "ChordMapStripMgr.h"
#include "winver.h"
#include <SegmentGuids.h>
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CPersonalityDesignerApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xd433f95b, 0xb588, 0x11d0, { 0x9e, 0xdc, 0, 0xaa, 0, 0xa2, 0x1b, 0xa9 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CPersonalityApp::SetNodePointers  (used by all components)
void CPersonalityDesignerApp::SetNodePointers( IDMUSProdNode* pINode, IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr;
    IDMUSProdNode* pIChild;
    IDMUSProdNode* pINextChild;

	ASSERT( pINode != NULL );
	ASSERT( pIDocRootNode != NULL );

	pINode->SetDocRootNode( pIDocRootNode );    
	pINode->SetParentNode( pIParentNode );    

	hr = pINode->GetFirstChild( &pINextChild );

	while( SUCCEEDED( hr )  &&  pINextChild )
	{
		pIChild = pINextChild;

		SetNodePointers( pIChild, pIDocRootNode, pINode );

		hr = pINode->GetNextChild( pIChild, &pINextChild );
		pIChild->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// SetRegString - Writes a string to system registry 

static BOOL SetRegString( HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName, LPCTSTR lpszString )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HKEY  hKeyOpen;
	DWORD dwCbData;
	LONG  lResult;
	DWORD dwDisposition;
	BOOL  fSuccess = FALSE;

	lResult = RegCreateKeyEx( hKey, lpSubKey, 0, 0, REG_OPTION_NON_VOLATILE,
							  KEY_ALL_ACCESS, NULL, &hKeyOpen, &dwDisposition );
	if( lResult == ERROR_SUCCESS )
	{
		dwCbData = _tcslen(lpszString)+1; // Assume ASCII  This is BAD!!!!!

		lResult = RegSetValueEx( hKeyOpen, lpValueName, 0, REG_SZ, (LPBYTE)lpszString, dwCbData);

		if( lResult == ERROR_SUCCESS )
		{
			fSuccess = TRUE;
		}

		RegCloseKey( hKeyOpen );
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// SetRegDWORD - Writes a DWORD to system registry 

static BOOL SetRegDWORD( HKEY hKey, LPCTSTR lpSubKey,
						 LPTSTR lpValueName, LPDWORD lpDWORD, BOOL fOverWrite )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HKEY  hKeyOpen;
	DWORD dwCbData;
	LONG  lResult;
	DWORD dwDisposition;
	DWORD dwType;
	BOOL  fSuccess = FALSE;

	lResult = RegCreateKeyEx( hKey, lpSubKey, 0, 0, REG_OPTION_NON_VOLATILE,
							  KEY_ALL_ACCESS, NULL, &hKeyOpen, &dwDisposition );
	if( lResult == ERROR_SUCCESS )
	{

		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, NULL, &dwType, NULL, NULL );

		if( (lResult != ERROR_SUCCESS)
		||  (dwType != REG_DWORD)
		||  (fOverWrite == TRUE) )
		{
			dwCbData = sizeof( DWORD );
			lResult = RegSetValueEx( hKeyOpen, lpValueName, 0, REG_DWORD, (LPBYTE)lpDWORD, dwCbData);

			if( lResult == ERROR_SUCCESS )
			{
				fSuccess = TRUE;
			}
		}
		else
		{
			fSuccess = TRUE;
		}

		RegCloseKey( hKeyOpen );
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// RegisterComponents - Adds entries to the system registry for Jazz components
 
static BOOL RegisterComponents( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    LPOLESTR psz1;
    LPOLESTR psz2;
    LPOLESTR psz3;
    LPOLESTR psz4;
	TCHAR    szRegPath[MAX_BUFFER];
    TCHAR    szOCXPath[MAX_BUFFER];
    TCHAR    szOCXLongPath[MAX_BUFFER];
    TCHAR    szGuid1[MID_BUFFER];
    TCHAR    szGuid2[MID_BUFFER];
    TCHAR    szGuid3[MID_BUFFER];
    TCHAR    szGuid4[MID_BUFFER];
    CString  strName;
	DWORD    dwSkip = 0;
    
	GetModuleFileName( theApp.m_hInstance, szOCXLongPath, MAX_BUFFER ); 
	// This needs to be the short name, since the AfxRegister... functions
	// use the short name. (Except for AfxOleRegisterTypeLib()).
	// The type library is still using the long filename, hopefully that's ok.
	GetShortPathName( szOCXLongPath, szOCXPath, MAX_BUFFER);

// Register Personality Editor Component
	if( SUCCEEDED( StringFromIID(CLSID_PersonalityEditor, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_PERSONALITY_COMPONENT_NAME );

		_tcscpy( szRegPath, _T("CLSID") );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), strName)) )
		{
			return FALSE;
		}

		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), szOCXPath))
		||  !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T("ThreadingModel"), _T("Apartment"))) )
		{
			return FALSE;
		}
	}
	if( SUCCEEDED( StringFromIID(CLSID_PersonalityComponent, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_PERSONALITY_COMPONENT_NAME );

		_tcscpy( szRegPath, _T("CLSID") );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), strName)) )
		{
			return FALSE;
		}

		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), szOCXPath))
		||  !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T("ThreadingModel"), _T("Apartment"))) )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\Components\\") );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T(""), strName))
		||  !(SetRegDWORD(HKEY_LOCAL_MACHINE, szRegPath, _T("Skip"), &dwSkip, FALSE)) )
		{
			return FALSE;
		}

		if( SUCCEEDED( StringFromIID(GUID_PersonalityNode, &psz1) ) 
		&&  SUCCEEDED( StringFromIID(GUID_PersonalityRefNode, &psz2) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_PersonalityComponent, &psz3) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_DirectMusicChordMap, &psz4) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
			CoTaskMemFree( psz1 );
			WideCharToMultiByte( CP_ACP, 0, psz2, -1, szGuid2, sizeof(szGuid2), NULL, NULL );
			CoTaskMemFree( psz2 );
			WideCharToMultiByte( CP_ACP, 0, psz3, -1, szGuid3, sizeof(szGuid3), NULL, NULL );
			CoTaskMemFree( psz3 );
			WideCharToMultiByte( CP_ACP, 0, psz4, -1, szGuid4, sizeof(szGuid4), NULL, NULL );
			CoTaskMemFree( psz4 );
			strName.LoadString( IDS_CHORDMAP_OBJECT_TEXT );

			_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\Container Objects\\") );
			_tcscat( szRegPath, szGuid1 );
			if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T(""), strName))
			||  !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T("RefNode"), szGuid2)) 
			||  !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T("Component"), szGuid3)) 
			||  !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T("DMObject"), szGuid4)) )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
    }
	else
	{
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// CPersonalityDesignerApp::InitInstance - DLL initialization

BOOL CPersonalityDesignerApp::InitInstance()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxEnableControlContainer();

	BOOL bInit = COleControlModule::InitInstance();

	if( bInit )
	{
		m_pIPageManager = NULL;
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CPersonalityDesignerApp::ExitInstance - DLL termination

int CPersonalityDesignerApp::ExitInstance()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDesignerApp::GetHelpFileName

BOOL CPersonalityDesignerApp::GetHelpFileName( CString& strHelpFileName )
{
	TCHAR achHelpFileName[FILENAME_MAX];
	TCHAR achDrive[_MAX_DRIVE];
	TCHAR achDir[_MAX_DIR];
	TCHAR achFName[_MAX_FNAME];
	TCHAR achExt[_MAX_EXT];

	if( GetModuleFileName(NULL, achHelpFileName, FILENAME_MAX) > 0 )
	{
		_tsplitpath( achHelpFileName, achDrive, achDir, achFName, NULL );
		::LoadString( theApp.m_hInstance, IDS_HELP_FILE_EXT, achExt, _MAX_EXT );
		_tmakepath( achHelpFileName, achDrive, achDir, achFName, achExt );

		strHelpFileName = achHelpFileName;
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*
	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);
	*/

	if ( !RegisterComponents() )
		return ResultFromScode(SELFREG_E_FIRST+2);

	return NOERROR;
}

static BOOL UnregisterComponents( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	LPOLESTR psz;
	TCHAR    szRegPath[MAX_BUFFER];
	TCHAR    szGuid[MID_BUFFER];
	
	if( SUCCEEDED( StringFromIID(CLSID_PersonalityEditor, &psz) ) )
	{
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}
	}
	if( SUCCEEDED( StringFromIID(CLSID_PersonalityComponent, &psz) ) )
	{
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\Components\\") );
		_tcscat( szRegPath, szGuid );
		if( RegDeleteKey(HKEY_LOCAL_MACHINE, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		if( SUCCEEDED( StringFromIID(GUID_PersonalityNode, &psz) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
			CoTaskMemFree( psz );

			_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\Container Objects\\") );
			_tcscat( szRegPath, szGuid );
			if( RegDeleteKey(HKEY_LOCAL_MACHINE, szRegPath) != ERROR_SUCCESS )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*
	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);
	*/

	if( !UnregisterComponents() )
	{
		return ResultFromScode(SELFREG_E_FIRST+2);
	}

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// DllCanUnloadNow
#pragma warning(disable:4518 4502)
extern "C" __declspec(dllexport)
STDAPI DllCanUnloadNow()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return AfxDllCanUnloadNow();
}
#pragma warning(default:4518 4502)
//////////////////////////////////////////////////////////////////////////////
// CClassFactory
//
// Class factory object for creating any object implemented by this DLL.
//

typedef  HRESULT (__stdcall CreateInstanceFunc) (IUnknown *punkOuter,
    REFIID riid, void **ppv);

class CClassFactory : public IClassFactory
{
///// IUnknown implementation
protected:
    ULONG	m_dwRef;         // interface reference count
public:
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv)
    {
		AFX_MANAGE_STATE(_afxModuleAddrThis);
        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, IID_IClassFactory))
        {
            *ppv = (LPVOID) this;
            AddRef();
            return NOERROR;
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
    }
    STDMETHODIMP_(ULONG) AddRef()
    {
		AFX_MANAGE_STATE(_afxModuleAddrThis);
        return ++m_dwRef;
    }
    STDMETHODIMP_(ULONG) Release()
    {
		AFX_MANAGE_STATE(_afxModuleAddrThis);
        if (--m_dwRef == 0L)
        {
            delete this;
            return 0;
        }
        else
            return m_dwRef;
    }

///// IClassFactory implementation
protected:
    CreateInstanceFunc *m_pfunc;    // function that creates an object instance
public:
    STDMETHODIMP CreateInstance(LPUNKNOWN punkOuter, REFIID riid, LPVOID *ppv)
    {
        return (*m_pfunc)(punkOuter, riid, ppv);
    }
    STDMETHODIMP LockServer(BOOL fLock)
    {
        if( fLock )
        {
			AfxOleLockApp();
        }
        else
        {
			AfxOleUnlockApp();
        }

        return NOERROR;
    }

///// Construction
    CClassFactory(CreateInstanceFunc *pfunc) : m_pfunc(pfunc)
	{
		m_dwRef = 0;
	}
};

//////////////////////////////////////////////////////////////////////////////
// CREATE_INSTANCE macro
//

#define CREATE_INSTANCE( cls ) STDAPI cls##_CreateInstance( LPUNKNOWN /*punkOuter*/, REFIID riid, LPVOID *ppv ) \
{ \
    HRESULT   hrReturn; \
    cls* pthis; \
    pthis = new cls; \
    if( pthis == NULL ) \
    { \
        return E_OUTOFMEMORY; \
    } \
    hrReturn = pthis->QueryInterface( riid, ppv ); \
    if( FAILED( hrReturn ) ) \
    { \
        delete pthis; \
        *ppv = NULL; \
    } \
    return hrReturn; \
}

CREATE_INSTANCE( CPersonalityComponent )


/////////////////////////////////////////////////////////////////////////////
// DllGetClassObject
#pragma warning(disable:4518 4502)
extern "C" __declspec(dllexport)
STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*ppv = NULL;

    // this DLL can only create class factory objects that support
    // IUnknown and IClassFactory
    if( !IsEqualIID(riid, IID_IUnknown)
    &&  !IsEqualIID(riid, IID_IClassFactory) )
	{
        return E_NOINTERFACE;
	}

// point <pfunc> to a function that can create a new object instance
    CreateInstanceFunc *pfunc;

    if( IsEqualCLSID(rclsid, CLSID_PersonalityComponent) )
	{
        pfunc = CPersonalityComponent_CreateInstance;
		if( ( *ppv = static_cast<void*>( static_cast<IClassFactory *>( new CClassFactory( pfunc ) ) ) ) == NULL )
		{
			return E_OUTOFMEMORY;
		}
	    static_cast<IUnknown*>( *ppv )->AddRef();
	}
    else if( IsEqualCLSID(rclsid, CLSID_PersonalityEditor)
		 ||  IsEqualCLSID(rclsid, CLSID_PersonalityPropPage) )
	{
		AFX_MANAGE_STATE( AfxGetStaticModuleState() );
		AfxDllGetClassObject( rclsid, riid, ppv );
		if( *ppv == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}
    else
	{
        return E_FAIL;
	}

    return NOERROR;
}
#pragma warning(default:4518 4502)
////////////////////////////////////////////////////////////////////////////
// CPersonalityDesignerApp::GetFileVersion

BOOL CPersonalityDesignerApp::GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	DWORD dwBufferSize;
	DWORD dwReserved;
	
	dwBufferSize = GetFileVersionInfoSize( szExeName, &dwReserved );
	if( dwBufferSize > 0 )
	{
		void* pBuffer;

		pBuffer = (void *)malloc( dwBufferSize );
		if( pBuffer )
		{
			CString strTheFileVersion;
			VS_FIXEDFILEINFO* pFixedInfo;
			UINT nInfoSize;

			GetFileVersionInfo( szExeName, dwReserved, dwBufferSize, pBuffer );
			VerQueryValue( pBuffer, _T("\\"), (void **)&pFixedInfo, &nInfoSize );

			WORD wVer1 = HIWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer2 = LOWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer3 = HIWORD( pFixedInfo->dwFileVersionLS );
			WORD wVer4 = LOWORD( pFixedInfo->dwFileVersionLS );

			strTheFileVersion.Format( _T("%u.%u.%u.%u"), wVer1, wVer2, wVer3, wVer4 );

			_tcsncpy( szFileVersion, strTheFileVersion, nFileVersionSize );

			free( pBuffer );
			return TRUE;
		}
	}

	return FALSE;
}
