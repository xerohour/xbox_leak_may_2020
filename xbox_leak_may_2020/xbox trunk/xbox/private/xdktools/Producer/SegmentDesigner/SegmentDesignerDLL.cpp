// SegmentDesignerDLL.cpp : Implementation of CSegmentDesignerApp and DLL registration.

#include <winver.h>
#include "stdafx.h"
#include "SegmentDesigner.h"
#include "UnknownStripMgr.h"
#include "resource.h"
#include <initguid.h>
#include "SegmentComponent.h"
#include "Segment.h"
#include <dmusici.h>
#include "AudioPathDesigner.h"
#include "ToolGraphDesigner.h"
#include "ContainerDesigner.h"
#include "dmusicp.h"
#include <WaveTimelineDraw.h>
#include <DLSDesigner.h>
#include <dsoundp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSegmentDesignerApp NEAR theApp;

// {DFCE8608-A6FA-11d1-8881-00C04FBF8D15}
const GUID CDECL BASED_CODE _tlid = 
		{ 0xdfce8608, 0xa6fa, 0x11d1, { 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CSegmentDesignerApp::InitInstance - DLL initialization

BOOL CSegmentDesignerApp::InitInstance()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		AfxEnableControlContainer();
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CSegmentDesignerApp::ExitInstance - DLL termination

int CSegmentDesignerApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentDesignerApp::GetHelpFileName

BOOL CSegmentDesignerApp::GetHelpFileName( CString& strHelpFileName )
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
// CSegmentDesignerApp::SetNodePointers  (used by all components)

void CSegmentDesignerApp::SetNodePointers( IDMUSProdNode* pINode, IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pIParentNode )
{
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
		RELEASE( pIChild );
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
// RegisterComponents - Adds entries to the system registry for Jazz components

static BOOL RegisterComponents( void )
{
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

// Register Segment Editor Component
	if( SUCCEEDED( StringFromIID(CLSID_SegmentDesigner, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_SEGMENT_COMPONENT_NAME );

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

	if( SUCCEEDED( StringFromIID(CLSID_SegmentComponent, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_SEGMENT_COMPONENT_NAME );

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

		if( SUCCEEDED( StringFromIID(GUID_SegmentNode, &psz1) ) 
		&&  SUCCEEDED( StringFromIID(GUID_SegmentRefNode, &psz2) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_SegmentComponent, &psz3) ) 
		&&  SUCCEEDED( StringFromIID(CLSID_DirectMusicSegment, &psz4) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
			CoTaskMemFree( psz1 );
			WideCharToMultiByte( CP_ACP, 0, psz2, -1, szGuid2, sizeof(szGuid2), NULL, NULL );
			CoTaskMemFree( psz2 );
			WideCharToMultiByte( CP_ACP, 0, psz3, -1, szGuid3, sizeof(szGuid3), NULL, NULL );
			CoTaskMemFree( psz3 );
			WideCharToMultiByte( CP_ACP, 0, psz4, -1, szGuid4, sizeof(szGuid4), NULL, NULL );
			CoTaskMemFree( psz4 );
			strName.LoadString( IDS_SEGMENT_OBJECT_TEXT );

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

// Register UnknownStripMgr
	if( SUCCEEDED( StringFromIID(CLSID_UnknownStripMgr, &psz1) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
        CoTaskMemFree( psz1 );
		strName.LoadString( IDS_UNKNOWN_STRIPMGR );

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
	else
	{
		return FALSE;
	}

	return TRUE;
}

static BOOL UnregisterComponents( void )
{
	LPOLESTR psz;
	TCHAR    szRegPath[MAX_BUFFER];
	TCHAR    szGuid[MID_BUFFER];
	
	if( SUCCEEDED( StringFromIID(CLSID_SegmentDesigner, &psz) ) )
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

	if( SUCCEEDED( StringFromIID(CLSID_SegmentComponent, &psz) ) )
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

		if( SUCCEEDED( StringFromIID(GUID_SegmentNode, &psz) ) )
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

	if( SUCCEEDED( StringFromIID(CLSID_UnknownStripMgr, &psz) ) )
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
        return ++m_dwRef;
    }
    STDMETHODIMP_(ULONG) Release()
    {
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
	pthis->Release(); \
    return hrReturn; \
}

CREATE_INSTANCE( CSegmentComponent )
CREATE_INSTANCE( CUnknownStripMgr )


/////////////////////////////////////////////////////////////////////////////
// DllGetClassObject

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

    if( IsEqualCLSID(rclsid, CLSID_SegmentComponent) )
	{
        pfunc = CSegmentComponent_CreateInstance;
		if( ( *ppv = static_cast<void*>( static_cast<IClassFactory *>( new CClassFactory( pfunc ) ) ) ) == NULL )
		{
			return E_OUTOFMEMORY;
		}
	    static_cast<IUnknown*>( *ppv )->AddRef();
	}
    else if( IsEqualCLSID(rclsid, CLSID_SegmentDesigner)
		/* ||  IsEqualCLSID(rclsid, CLSID_SegmentPropPage) */)
	{
		AfxDllGetClassObject( rclsid, riid, ppv );
		if( *ppv == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}
	else if( IsEqualCLSID(rclsid, CLSID_UnknownStripMgr) )
	{
        pfunc = CUnknownStripMgr_CreateInstance;
		if( ( *ppv = static_cast<void*>( static_cast<IClassFactory *>( new CClassFactory( pfunc ) ) ) ) == NULL )
		{
			return E_OUTOFMEMORY;
		}
	    static_cast<IUnknown*>( *ppv )->AddRef();
	}
    else
	{
        return E_FAIL;
	}

    return NOERROR;
}

////////////////////////////////////////////////////////////////////////////
// CSegmentDesignerApp::GetFileVersion

BOOL CSegmentDesignerApp::GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize )
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
