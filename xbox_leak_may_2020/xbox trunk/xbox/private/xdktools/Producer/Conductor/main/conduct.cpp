// Conductor.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f Conductorps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
#include <mmsystem.h>
#include "dsoundp.h"
#include "audiosink.h"
#include "Conductor.h"
#include "CConduct.h"
#include <dmksctrl.h>
#include <PrivateTransport.h>
#include <afxctl.h>
#include <PChannelName.h>
#ifdef DMP_XBOX 
#include "..\shared\xguids.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const GUID CDECL BASED_CODE _tlid =
		{ 0x36F6DDE2, 0x46CE, 0x11D0, { 0xB9, 0xDB, 0, 0xAA, 0, 0xC0, 0x81, 0x46 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

EXTERN_C const IID IID_IMediaObjectInPlace = 
		{ 0x651b9ad0, 0x0fc7, 0x4aa9, { 0x95, 0x38, 0xd8, 0x99, 0x31, 0x01, 0x07, 0x41 } };
EXTERN_C const IID IID_IMediaObject = 
		{ 0xd8ad0f58, 0x5494, 0x4102, { 0x97, 0xc5, 0xec, 0x79, 0x8e, 0x59, 0xbc, 0xf4 } };

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_CConductor, CConductor)
END_OBJECT_MAP()

class CConductorApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

CConductorApp theApp;

BOOL CConductorApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);
	return CWinApp::InitInstance();
}

int CConductorApp::ExitInstance()
{
	_Module.Term();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// SetRegString - Writes a string to system registry 

BOOL SetRegString( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpszString )
{
	HKEY  hKeyOpen;
	DWORD dwCbData;
	LONG  lResult;
	DWORD dwDisposition;
	BOOL  fSuccess = FALSE;

	lResult = RegCreateKeyEx( hKey, lpSubKey, 0, 0, REG_OPTION_NON_VOLATILE,
							  KEY_SET_VALUE, NULL, &hKeyOpen, &dwDisposition );
	if( lResult == ERROR_SUCCESS )
	{
		dwCbData = _tcslen(lpszString) + sizeof( TCHAR );

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

BOOL SetRegDWORD( HKEY hKey, LPCTSTR lpSubKey,
						 LPCTSTR lpValueName, LPDWORD lpDWORD, BOOL fOverWrite )
{
	HKEY  hKeyOpen;
	DWORD dwCbData;
	LONG  lResult;
	DWORD dwDisposition;
	DWORD dwType;
	BOOL  fSuccess = FALSE;

	lResult = ::RegCreateKeyEx( hKey, lpSubKey, 0, 0, REG_OPTION_NON_VOLATILE,
							  KEY_SET_VALUE, NULL, &hKeyOpen, &dwDisposition );
	if( lResult == ERROR_SUCCESS )
	{

		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, NULL, &dwType, NULL, NULL );

		if( (lResult != ERROR_SUCCESS)
		||  (dwType != REG_DWORD)
		||  (fOverWrite == TRUE) )
		{
			dwCbData = sizeof( DWORD );
			lResult = ::RegSetValueEx( hKeyOpen, lpValueName, 0, REG_DWORD, (LPBYTE)lpDWORD, dwCbData);

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
// RegisterComponents - Adds entries to the system registry for DMUSProducer components

static BOOL RegisterComponents( void )
{
    LPOLESTR psz;
	TCHAR    szRegPath[256];
	TCHAR	 szCLSID[32];
	TCHAR	 szInProcServer32[32];
	TCHAR	 szThreadingModel[32];
	TCHAR	 szApartment[32];
    TCHAR    szOCXPath[256];
    TCHAR    szOCXLongPath[256];
    TCHAR    szGuid[100];
    CString  strName;
    TCHAR    szComponentPath[256];
	TCHAR	 szSkip[32];
	DWORD    dwSkip = 0;
    
	GetModuleFileName( theApp.m_hInstance, szOCXLongPath, 256 ); 
	// This needs to be the short name, since the AfxRegister... functions
	// use the short name. (Except for AfxOleRegisterTypeLib()).
	// The type library is still using the long filename, hopefully that's ok.
	GetShortPathName( szOCXLongPath, szOCXPath, 256);

	_tcscpy( szCLSID, _T("CLSID") );
	_tcscpy( szInProcServer32, _T("InProcServer32") );
	_tcscpy( szThreadingModel, _T("ThreadingModel") );
	_tcscpy( szApartment, _T("Apartment") );

	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\Components\\") );
	_tcscpy( szSkip, _T("Skip") );
	
// Register Style Editor Component
    if( SUCCEEDED( StringFromIID(CLSID_CConductor, &psz) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
        CoTaskMemFree( psz );
		strName.LoadString( IDS_CONDUCTOR_COMPONENT_NAME );

		_tcscpy( szRegPath, szCLSID );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid );
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
		_tcscat( szRegPath, szGuid );
		if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T(""), strName))
		||  !(SetRegDWORD(HKEY_LOCAL_MACHINE, szRegPath, szSkip, &dwSkip, FALSE)) )
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
	TCHAR    szRegPath[255];
	TCHAR    szGuid[100];
	
	if( SUCCEEDED( StringFromIID(CLSID_CConductor, &psz) ) )
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
	// registers object, typelib and all interfaces in typelib
	//return _Module.RegisterServer(TRUE);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//	if( !AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid) )
//	{
//		return ResultFromScode(SELFREG_E_TYPELIB);
//	}

	if( !COleObjectFactoryEx::UpdateRegistryAll(TRUE) )
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}

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
	//_Module.UnregisterServer();
	//return S_OK;

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(LIBID_CONDUCTORLib))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	if( !UnregisterComponents() )
	{
		return ResultFromScode(SELFREG_E_FIRST+2);
	}

	return NOERROR;
}


