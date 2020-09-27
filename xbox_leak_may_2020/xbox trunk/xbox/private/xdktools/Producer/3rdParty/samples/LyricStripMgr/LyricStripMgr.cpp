/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// LyricStripMgr.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "LyricStripMgr.h"
#include <AFXCTL.H>
#include "LyricMgr.h"

#include <initguid.h>
#include "LyricStripMgr_i.c"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ATL Stuff follows
CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_LyricMgr, CLyricMgr)
END_OBJECT_MAP()

class CLyricStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

CLyricStripMgrApp theApp;

BOOL CLyricStripMgrApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);
	AfxEnableControlContainer();
	return CWinApp::InitInstance();
}

int CLyricStripMgrApp::ExitInstance()
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
// RegisterComponents - Adds entries to the system registry for DirectMusicProducer strip managers

static BOOL RegisterComponents( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Convert the Lyric Strip Manager's CLSID into an OLE string
	LPOLESTR psz;
	if( SUCCEEDED( StringFromIID(CLSID_LyricMgr, &psz) ) )
	{
		// Convert from an OLE string to a multi-byte string
		char szEditorCLSID[100];
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szEditorCLSID, sizeof(szEditorCLSID), NULL, NULL );

		// Free the memory used by the OLE string
		CoTaskMemFree( psz );

		// Convert the Lyric Track's CLSID into an OLE string
		if( SUCCEEDED( StringFromIID(CLSID_LyricTrack, &psz) ) )
		{
			// Convert from an OLE string to a multi-byte string
			char szTrackCLSID[100];
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szTrackCLSID, sizeof(szTrackCLSID), NULL, NULL );

			// Free the memory used by the OLE string
			CoTaskMemFree( psz );

			// Load the name of the track from the resource file
			CString strTrackName;
			strTrackName.LoadString( IDS_TRACK_NAME );

			// Build the path to the item in the registry to set
			TCHAR szRegPath[256];
			_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\StripEditors\\") );
			_tcscat( szRegPath, szTrackCLSID );

			// Set the default value to the track's name
			if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T(""), strTrackName)) )
			{
				return FALSE;
			}

			// Set the CLSID of the Strip Manager editor to use to edit this track
			if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T("StripManager"), szEditorCLSID)) )
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

/////////////////////////////////////////////////////////////////////////////
// UnregisterComponents - Removes entries from the system registry for DirectMusicProducer strip managers

static BOOL UnregisterComponents( void )
{
	// Convert the Lyric Track's CLSID into an OLE string
	LPOLESTR psz;
	if( SUCCEEDED( StringFromIID(CLSID_LyricTrack, &psz) ) )
	{
		// Convert the OLE string into a character string
		char szTrackCLSID[100];
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szTrackCLSID, sizeof(szTrackCLSID), NULL, NULL );
		CoTaskMemFree( psz );

		// Build the registry key to delete
		char szRegPath[255];
		strcpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\StripEditors\\") );
		strcat( szRegPath, szTrackCLSID );

		// Delete the registry key
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
	if ( _Module.RegisterServer(TRUE) )
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}

	// Creates the DirectMusic Producer-specific registry entries
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

	if ( !AfxOleUnregisterTypeLib( LIBID_LYRICSTRIPMGRLib ) )
	{
		return ResultFromScode(SELFREG_E_TYPELIB);
	}

	// unregisters object, typelib and all interfaces in typelib
	if ( _Module.UnregisterServer() )
	{
		return ResultFromScode(SELFREG_E_CLASS);
	}

	// Removes the DirectMusic Producer-specific registry entries
	if( !UnregisterComponents() )
	{
		return ResultFromScode(SELFREG_E_FIRST+2);
	}

	return NOERROR;
}


