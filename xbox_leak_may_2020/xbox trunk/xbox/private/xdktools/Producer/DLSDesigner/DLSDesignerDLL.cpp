// DLSDesigner.cpp : Implementation of CDLSDesignerApp and DLL registration.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "DLSComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDLSDesignerApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xbc964e83, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CDLSDesignerApp::InitInstance - DLL initialization

BOOL CDLSDesignerApp::InitInstance()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxEnableControlContainer();
	BOOL bInit = COleControlModule::InitInstance();
	
	if (bInit)
	{
		m_pFramework = NULL;
	}

	m_pClipboardObject = NULL;
	m_pIClipboardDataObject = NULL;
	m_hKeyProgressBar = NULL;

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CDLSDesignerApp::ExitInstance - DLL termination

int CDLSDesignerApp::ExitInstance()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CDLSDesignerApp::GetHelpFileName

BOOL CDLSDesignerApp::GetHelpFileName( CString& strHelpFileName )
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


////////////////////////////////////////////////////////////////////////////
// CBandEditorApp::PutDataInClipboard

BOOL CDLSDesignerApp::PutDataInClipboard( IDataObject* pIDataObject, void* pObject )
{
	if( pIDataObject )
	{
		if( ::OleSetClipboard( pIDataObject ) == S_OK )
		{
			if( m_pIClipboardDataObject )
			{
				m_pIClipboardDataObject->Release();
				m_pIClipboardDataObject = NULL;
			}

			m_pIClipboardDataObject = pIDataObject;
			m_pIClipboardDataObject->AddRef();

			m_pClipboardObject = pObject;

			return TRUE;
		}
	}

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////
// CBandEditorApp::FlushClipboard

void CDLSDesignerApp::FlushClipboard( void* pClipboardObject )
{
	if( m_pClipboardObject == pClipboardObject )
	{
		ASSERT( m_pIClipboardDataObject != NULL );

		if( ::OleIsCurrentClipboard( m_pIClipboardDataObject ) == S_OK )
		{
			::OleFlushClipboard();
		}

		if( m_pIClipboardDataObject )
		{
			m_pIClipboardDataObject->Release();
			m_pIClipboardDataObject = NULL;
		}

		m_pClipboardObject = NULL;
	}
}



////////////////////////////////////////////////////////////////////////////
// CDLSDesignerApp::SetNodePointers  (used by all components)

void CDLSDesignerApp::SetNodePointers(IDMUSProdNode* pINode, IDMUSProdNode* pIRootNode, IDMUSProdNode* pIParentNode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdNode* pIChild;

	ASSERT( pINode != NULL );
	ASSERT( pIRootNode != NULL );

	pINode->SetDocRootNode( pIRootNode );    
	pINode->SetParentNode( pIParentNode );    

	HRESULT hr = pINode->GetFirstChild(&pIChild);

	while(SUCCEEDED(hr)  &&  pIChild)
	{
		IDMUSProdNode* pINextChild;
		
		SetNodePointers(pIChild, pIRootNode, pINode);

		hr = pINode->GetNextChild( pIChild, &pINextChild );
		
		pIChild->Release();
		pIChild = pINextChild;
	}
}

/////////////////////////////////////////////////////////////////////////////
// SetRegString - Writes a string to system registry 

static BOOL SetRegString( HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName, LPCTSTR lpszString )
{
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

void CDLSDesignerApp::SetFramework(IDMUSProdFramework * pFramework)

{
	m_pFramework = pFramework;
}


IDMUSProdFramework *CDLSDesignerApp::GetFramework()

{
	return m_pFramework;
}

/////////////////////////////////////////////////////////////////////////////
// SetRegDWORD - Writes a DWORD to system registry 

static BOOL SetRegDWORD( HKEY hKey, LPCTSTR lpSubKey,
						 LPTSTR lpValueName, LPDWORD lpDWORD, BOOL fOverWrite )
{
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
// RegisterComponents - Adds entries to the system registry for DirectMusic Producer components

static BOOL RegisterComponents( void )
{
    LPOLESTR psz1;
    LPOLESTR psz2;
    LPOLESTR psz3;
    LPOLESTR psz4;
	TCHAR    szRegPath[MAX_BUFFER];
	TCHAR	 szCLSID[SMALL_BUFFER];
	TCHAR	 szInProcServer32[SMALL_BUFFER];
	TCHAR	 szThreadingModel[SMALL_BUFFER];
	TCHAR	 szApartment[SMALL_BUFFER];
    TCHAR    szOCXPath[MAX_BUFFER];
	TCHAR	 szOCXLongPath[MAX_BUFFER];
    TCHAR    szGuid1[MID_BUFFER];
    TCHAR    szGuid2[MID_BUFFER];
    TCHAR    szGuid3[MID_BUFFER];
    TCHAR    szGuid4[MID_BUFFER];
    CString  strName;
    TCHAR    szComponentPath[MAX_BUFFER];
	TCHAR	 szSkip[SMALL_BUFFER];
	DWORD    dwSkip = 0;
    
	GetModuleFileName( theApp.m_hInstance, szOCXLongPath, MAX_BUFFER ); 
	// This needs to be the short name, since the AfxRegister... functions
	// use the short name. (Except for AfxOleRegisterTypeLib()).
	// The type library is still using the long filename, hopefully that's ok.
	GetShortPathName( szOCXLongPath, szOCXPath, MAX_BUFFER);

	_tcscpy( szCLSID, _T("CLSID") );
	_tcscpy( szInProcServer32, _T("InProcServer32") );
	_tcscpy( szThreadingModel, _T("ThreadingModel") );
	_tcscpy( szApartment, _T("Apartment") );

	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\Components\\") );
	_tcscpy( szSkip, _T("Skip") );
	
// Register DLS Designer Component
    if( SUCCEEDED( StringFromIID(CLSID_Wave, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_DLS_COMPONENT_NAME );

		_tcscpy( szRegPath, szCLSID );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), strName)) )
		{
			return FALSE;
		}

		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szInProcServer32 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), szOCXPath))
		||  !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, szThreadingModel, szApartment)) )
		{
			return FALSE;
		}
	}
    if( SUCCEEDED( StringFromIID(CLSID_Instrument, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_DLS_COMPONENT_NAME );

		_tcscpy( szRegPath, szCLSID );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), strName)) )
		{
			return FALSE;
		}

		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szInProcServer32 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), szOCXPath))
		||  !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, szThreadingModel, szApartment)) )
		{
			return FALSE;
		}
	}
    if( SUCCEEDED( StringFromIID(CLSID_DLSComponent, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_DLS_COMPONENT_NAME );

		_tcscpy( szRegPath, szCLSID );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), strName)) )
		{
			return FALSE;
		}

		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szInProcServer32 );
		if( !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, _T(""), szOCXPath))
		||  !(SetRegString(HKEY_CLASSES_ROOT, szRegPath, szThreadingModel, szApartment)) )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, szComponentPath );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T(""), strName))
		||  !(SetRegDWORD(HKEY_LOCAL_MACHINE, szRegPath, szSkip, &dwSkip, FALSE)) )
		{
			return FALSE;
		}

		if( SUCCEEDED( StringFromIID(GUID_CollectionNode, &psz1) ) 
		&&  SUCCEEDED( StringFromIID(GUID_CollectionRefNode, &psz2) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_DLSComponent, &psz3) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_DirectMusicCollection, &psz4) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
			CoTaskMemFree( psz1 );
			WideCharToMultiByte( CP_ACP, 0, psz2, -1, szGuid2, sizeof(szGuid2), NULL, NULL );
			CoTaskMemFree( psz2 );
			WideCharToMultiByte( CP_ACP, 0, psz3, -1, szGuid3, sizeof(szGuid3), NULL, NULL );
			CoTaskMemFree( psz3 );
			WideCharToMultiByte( CP_ACP, 0, psz4, -1, szGuid4, sizeof(szGuid4), NULL, NULL );
			CoTaskMemFree( psz4 );
			strName.LoadString( IDS_COLLECTION_OBJECT_TEXT );

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

		if( SUCCEEDED( StringFromIID(GUID_WaveNode, &psz1) ) 
		&&  SUCCEEDED( StringFromIID(GUID_WaveRefNode, &psz2) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_DLSComponent, &psz3) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_DirectSoundWave, &psz4) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
			CoTaskMemFree( psz1 );
			WideCharToMultiByte( CP_ACP, 0, psz2, -1, szGuid2, sizeof(szGuid2), NULL, NULL );
			CoTaskMemFree( psz2 );
			WideCharToMultiByte( CP_ACP, 0, psz3, -1, szGuid3, sizeof(szGuid3), NULL, NULL );
			CoTaskMemFree( psz3 );
			WideCharToMultiByte( CP_ACP, 0, psz4, -1, szGuid4, sizeof(szGuid4), NULL, NULL );
			CoTaskMemFree( psz4 );
			strName.LoadString( IDS_WAVE_OBJECT_TEXT );

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



/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/*
	if( !AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid) )
	{
		return ResultFromScode(SELFREG_E_TYPELIB);
	}

	if( !COleObjectFactoryEx::UpdateRegistryAll(TRUE) )
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}
	*/

	if( !RegisterComponents() )
	{
		return ResultFromScode(SELFREG_E_FIRST+2);
	}

	return NOERROR;
}

static BOOL UnregisterComponents( void )
{
	LPOLESTR psz;
	TCHAR    szRegPath[MAX_BUFFER];
	TCHAR    szGuid[MID_BUFFER];
	
	if( SUCCEEDED( StringFromIID(CLSID_Wave, &psz) ) )
    {
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		// First delete the subkey
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		// Then the main key
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}
	}
	if( SUCCEEDED( StringFromIID(CLSID_Instrument, &psz) ) )
    {
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		// First delete the subkey
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		// Then the main key
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}
	}
	if( SUCCEEDED( StringFromIID(CLSID_DLSComponent, &psz) ) )
    {
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		// First delete the subkey
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, _T("CLSID\\") );
		_tcscat( szRegPath, szGuid );
		// Then the main key
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

		if( SUCCEEDED( StringFromIID(GUID_CollectionNode, &psz) ) )
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

		if( SUCCEEDED( StringFromIID(GUID_WaveNode, &psz) ) )
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
	if( !AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor) )
	{
		return ResultFromScode(SELFREG_E_TYPELIB);
	}

	if( !COleObjectFactoryEx::UpdateRegistryAll(FALSE) )
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}
	*/

	if( !UnregisterComponents() )
	{
		return ResultFromScode(SELFREG_E_FIRST+2);
	}

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// DllCanUnloadNow

STDAPI DllCanUnloadNow()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return AfxDllCanUnloadNow();
}


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

CREATE_INSTANCE( CDLSComponent )


/////////////////////////////////////////////////////////////////////////////
// DllGetClassObject

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
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

    if( IsEqualCLSID(rclsid, CLSID_DLSComponent) )
	{
        pfunc = CDLSComponent_CreateInstance;
		if( ( *ppv = static_cast<void*>( static_cast<IClassFactory *>( new CClassFactory( pfunc ) ) ) ) == NULL )
		{
			return E_OUTOFMEMORY;
		}
	    static_cast<IUnknown*>( *ppv )->AddRef();
	}
    
    else if( IsEqualCLSID(rclsid, CLSID_Collection)
//		 ||  IsEqualCLSID(rclsid, CLSID_CollectionPropPage)
		 ||  IsEqualCLSID(rclsid, CLSID_Instrument)
//		 ||  IsEqualCLSID(rclsid, CLSID_InstrumentPropPage)
		 ||  IsEqualCLSID(rclsid, CLSID_Region)
//		 ||  IsEqualCLSID(rclsid, CLSID_RegionPropPage)
		 ||  IsEqualCLSID(rclsid, CLSID_Wave)
//		 ||  IsEqualCLSID(rclsid, CLSID_WavePropPage)
		 ||  IsEqualCLSID(rclsid, CLSID_Articulation)
//		 ||  IsEqualCLSID(rclsid, CLSID_ArticulationPropPage) 
		)
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
