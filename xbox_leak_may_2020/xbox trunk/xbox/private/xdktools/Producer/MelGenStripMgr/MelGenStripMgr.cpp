// MelGenStripMgr.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f MelGenStripMgrps.mak in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "MelGenStripMgr.h"
#include <AFXCTL.H>

#include <initguid.h>
#include "MelGenMgr.h"
#include <dmusici.h>

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_MelGenMgr, CMelGenMgr)
END_OBJECT_MAP()

class CMelGenStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

CMelGenStripMgrApp theApp;

BOOL CMelGenStripMgrApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);
	AfxEnableControlContainer();
	return CWinApp::InitInstance();
}

int CMelGenStripMgrApp::ExitInstance()
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
// RegisterComponents - Adds entries to the system registry for DMUSProducer strip managers

static BOOL RegisterComponents( void )
{
    LPOLESTR psz;
	TCHAR    szRegPath[256];
	TCHAR	 szStripManager[32];
	TCHAR	 szUserName[32];
    TCHAR    szEditorGuid[100];
	TCHAR	 szTrackGuid[100];
    CString  strTrackName, strEditorName;
    TCHAR    szComponentPath[256];
    
	_tcscpy( szStripManager, _T("StripManager") );
	_tcscpy( szUserName, _T("UserName") );

	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\StripEditors\\") );
	
    if( SUCCEEDED( StringFromIID(CLSID_MelGenMgr, &psz) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz, -1, szEditorGuid, sizeof(szEditorGuid), NULL, NULL );
        CoTaskMemFree( psz );
		if( SUCCEEDED( StringFromIID(CLSID_DirectMusicMelodyFormulationTrack, &psz) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szTrackGuid, sizeof(szTrackGuid), NULL, NULL );
			CoTaskMemFree( psz );

			strTrackName.LoadString( IDS_TRACK_NAME );
			_tcscpy( szRegPath, szComponentPath );
			_tcscat( szRegPath, szTrackGuid );
			if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T(""), strTrackName)) )
			{
				return FALSE;
			}
			if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, szStripManager, szEditorGuid)) )
			{
				return FALSE;
			}
			strEditorName.LoadString( IDS_EDITOR_NAME );
			if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, szUserName, strEditorName)) )
			{
				return FALSE;
			}
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
	
	if( SUCCEEDED( StringFromIID(CLSID_DirectMusicMelodyFormulationTrack, &psz) ) )
   	{
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\StripEditors\\") );
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
	// registers object
	if ( _Module.RegisterServer(FALSE) )
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	/*
	if ( !AfxOleUnregisterTypeLib( LIBID_MELGENSTRIPMGRLib ) )
	{
		return ResultFromScode(SELFREG_E_TYPELIB);
	}
	*/

	if ( _Module.UnregisterServer(FALSE) )
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}

	if( !UnregisterComponents() )
	{
		return ResultFromScode(SELFREG_E_FIRST+2);
	}
	return NOERROR;
}


