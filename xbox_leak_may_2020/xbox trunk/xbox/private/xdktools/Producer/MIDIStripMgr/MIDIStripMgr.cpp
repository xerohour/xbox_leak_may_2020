// MIDIStripMgr.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f MIDIStripMgrps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "SequenceStripMgr.h"
#include "initguid.h"
#include "MIDIStripMgr.h"
#include <PChannelName.h>
#include <SegmentGUIDs.h>
#include "StyleDesigner.h"
#include "SegmentDesigner.h"

#include "MIDIMgr.h"
#include "AFXCTL.H"
#include "dmusicp.h"


const UINT g_nGridsPerBeatBitmaps[MAX_GRIDS_PER_BEAT_ENTRIES] =
	{ 
	  IDB_GPB1,     IDB_GPB2,     IDB_GPB3,     IDB_GPB4,     IDB_GPB5,		// Beat = quarter note
	  IDB_GPB6,     IDB_GPB7,     IDB_GPB8,     IDB_GPB9,     IDB_GPBa10,
	  IDB_GPBa11,   IDB_GPBa12,   IDB_GPBa13,   IDB_GPBa14,   IDB_GPBa15,
	  IDB_GPBa16,   IDB_GPBa17,   IDB_GPBa18,   IDB_GPBa19,   IDB_GPBb20,
	  IDB_GPBb21,   IDB_GPBb22,   IDB_GPBb23,   IDB_GPBb24,
	  IDB_GPB1_ALT, IDB_GPB2_ALT, IDB_GPB3_ALT, IDB_GPB4_ALT, IDB_GPB5,		// Beat != quarter note
	  IDB_GPB6_ALT, IDB_GPB7,     IDB_GPB8_ALT, IDB_GPB9,     IDB_GPBa10,
	  IDB_GPBa11,   IDB_GPBa12,   IDB_GPBa13,   IDB_GPBa14,   IDB_GPBa15,
	  IDB_GPBa16,   IDB_GPBa17,   IDB_GPBa18,   IDB_GPBa19,   IDB_GPBb20,
	  IDB_GPBb21,   IDB_GPBb22,   IDB_GPBb23,   IDB_GPBb24
	};


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_MIDIMgr, CMIDIMgr)
END_OBJECT_MAP()


CMIDIStripMgrApp theApp;


BOOL CMIDIStripMgrApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);
	
	m_pIPageManager = NULL;

	m_cfCurve = ::RegisterClipboardFormat( CF_CURVE );
	m_pCurveStripFont = NULL;
	m_cfNoteFormat = RegisterClipboardFormat( CF_MUSICNOTE );
	m_cfAllEventFormat = RegisterClipboardFormat(CF_MUSICNOTEANDCURVE);
	m_cfSeqTrack = ::RegisterClipboardFormat(CF_SEQUENCELIST);
	m_cfSeqCurves = ::RegisterClipboardFormat(CF_CURVELIST);
	m_cfMIDIFile = ::RegisterClipboardFormat( CF_MIDIFILE );
	m_cfStyleMarker = ::RegisterClipboardFormat( CF_STYLEMARKER );

	return CWinApp::InitInstance();
}

int CMIDIStripMgrApp::ExitInstance()
{
	if( m_pCurveStripFont )
	{
		m_pCurveStripFont->DeleteObject();
		delete m_pCurveStripFont;
		m_pCurveStripFont = NULL;
	}

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
	
    if( SUCCEEDED( StringFromIID(CLSID_MIDIMgr, &psz) ) )
    {
        WideCharToMultiByte( CP_ACP, 0, psz, -1, szEditorGuid, sizeof(szEditorGuid), NULL, NULL );
        CoTaskMemFree( psz );
		if( SUCCEEDED( StringFromIID(CLSID_DirectMusicPatternTrack, &psz) ) )
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
	
	if( SUCCEEDED( StringFromIID(CLSID_DirectMusicPatternTrack, &psz) ) )
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
	if ( !AfxOleUnregisterTypeLib( LIBID_MIDISTRIPMGRLib ) )
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


