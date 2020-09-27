// XboxAddin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "XboxAddinDlg.h"
#include "XboxAddinComponent.h"
#include "TargetDirectoryDlg.h"
#include "TargetXboxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256

// command prefix
static const TCHAR g_szCmd[] = _T(CMD_PREFIX);
static const TCHAR g_szNotificationDelimiters[] = _T(" \n");

// command structure
struct NotificationParseStruct {
	const TCHAR *pszString;
	NOTIFICATION_TYPE notificationType;
};

NotificationParseStruct g_csNotifications[] = 
{	{_T("Segment"), NOTIFICATION_SEGMENT },
	{_T("LoadResult"), NOTIFICATION_LOAD_RESULT },
	{_T("UnloadResult"), NOTIFICATION_UNLOAD_RESULT },
	{_T("PlayResult"), NOTIFICATION_PLAY_RESULT },
	{_T("ClearCacheResult"), NOTIFICATION_CLEARCACHE_RESULT },
	{_T("PanicResult"), NOTIFICATION_PANIC_RESULT },
	{_T("CreateResult"), NOTIFICATION_CREATE_RESULT },
	{_T("SetDefaultResult"), NOTIFICATION_SETDEFAULT_RESULT },
	{_T("ReleaseResult"), NOTIFICATION_RELEASE_RESULT },
};


//-----------------------------------------------------------------------------
// Name: ExtNotifyFunc
// Desc: Notifier function registered via DmRegisterNotificationProcessor 
//          below.  This is called to return output from the remote ECP
//-----------------------------------------------------------------------------
DWORD __stdcall ExtNotifyFunc(LPCSTR szNotification)
{
    // skip over prefix
    TCHAR szCmd[MAX_PATH];
    _tcsncpy( szCmd, szNotification + _tcslen(g_szCmd) + 1, MAX_PATH );

    // Parse up to ten tokens
    TCHAR *astrTokens[10];

    // Get the command from szCmd
    TCHAR *strToken = _tcstok( szCmd, g_szNotificationDelimiters );
	int nTokens=0;
    while( strToken && nTokens < 10 )
    {
        astrTokens[nTokens] = strToken;

		nTokens++;

        strToken = _tcstok( NULL, g_szNotificationDelimiters );
    }

	NOTIFICATION_TYPE notificationType = NOTIFICATION_UNKNOWN;

	for( int i=0; i<sizeof(g_csNotifications) / sizeof(g_csNotifications[0]); i++ )
	{
		if(0 == _tcsncmp(g_csNotifications[i].pszString, astrTokens[0], _tcslen(g_csNotifications[i].pszString)))
		{
			notificationType = g_csNotifications[i].notificationType;
			break;
		}
	}

    if( notificationType == NOTIFICATION_UNKNOWN )
    {
        TRACE("Unknown Xbox notification: %s\n", szNotification);
    }
	else
	{
		// Handle the notification
		theApp.HandleNotification( notificationType, astrTokens, nTokens );
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: HandleDebugString
// Desc: Prints debug output to the output window, if desired
//-----------------------------------------------------------------------------
DWORD __stdcall HandleDebugString( ULONG dwNotification, DWORD dwParam )
{
    if( (DM_DEBUGSTR == dwNotification)
    &&  theApp.m_fDebugMonitor )
    {
        PDMN_DEBUGSTR p = (PDMN_DEBUGSTR)dwParam;

		// Copy the string in order to NULL-terminate it
		char *strText = new char[p->Length+1];
		if( strText )
		{
			memcpy( strText, p->String, sizeof(char) * p->Length );

			// NULL-terminate it
			strText[ p->Length ] = 0;

			// Output the debug string
	        TRACE("Xbox Dbg: %s\n", p->String );

			// Delete the temporary array
			delete []strText;
		}
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXboxAddinApp object

CXboxAddinApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp construction

CXboxAddinApp::CXboxAddinApp()
{
	// Place all significant initialization in InitInstance
	m_pXboxAddinComponent = NULL;
	m_fConnected = false;
	m_fECPConnected = false;
	m_fDebugMonitor = true;

	m_pdmnSession = NULL;
	m_pdmConnection = NULL;

	InitializeCriticalSection( &m_csNotificationList );
}

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp initialization

BOOL CXboxAddinApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		AfxEnableControlContainer();
	}

	TCHAR tcstrXboxPath[MAX_PATH];
	ZeroMemory( tcstrXboxPath, sizeof(TCHAR) * MAX_PATH );
	DWORD dwCbData = sizeof(TCHAR) * MAX_PATH;
	if( GetUserRegString( _T("XboxPath"), tcstrXboxPath, &dwCbData ) )
	{
		// Copy the path to m_strXboxDestination
		m_strXboxDestination = tcstrXboxPath;
	}

	if( m_strXboxDestination.IsEmpty() )
	{
		// Use a default path
		m_strXboxDestination = _T("T:\\Temp");
	}
	else
	{
		// Ensure the path starts with T:\ 
		if( m_strXboxDestination.Left(3).CompareNoCase( _T("T:\\") ) != 0 )
		{
			// Strip off everything before the first \ and prepend T:
			int nBackSlashIndex = m_strXboxDestination.Find('\\');
			if( nBackSlashIndex!= -1 )
			{
				// Found a backslash, just prepend T:
				m_strXboxDestination = _T("T:") + m_strXboxDestination.Right(m_strXboxDestination.GetLength() - nBackSlashIndex );
			}
			else
			{
				// No backslash found, prepend T:\ 
				m_strXboxDestination = _T("T:\\") + m_strXboxDestination;
			}
		}
	}

	// Update the string in the registry
	SetUserRegString( _T("XboxPath"), m_strXboxDestination );

	ZeroMemory( tcstrXboxPath, sizeof(TCHAR) * MAX_PATH );
	dwCbData = sizeof(TCHAR) * MAX_PATH;
	if( GetUserRegString( _T("XboxName"), tcstrXboxPath, &dwCbData ) )
	{
		// Copy the Xbox name to m_strXboxName
		m_strXboxName = tcstrXboxPath;
	}

	if( m_strXboxName.IsEmpty() )
	{
		// Get the default destination
		char cstrXboxName[MAX_PATH];
		ZeroMemory( tcstrXboxPath, sizeof(char) * MAX_PATH );
		dwCbData = sizeof(char) * MAX_PATH;
		if( SUCCEEDED( DmGetXboxName( cstrXboxName, &dwCbData ) ) )
		{
			m_strXboxName = cstrXboxName;
		}
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp::ExitInstance - DLL termination

int CXboxAddinApp::ExitInstance()
{
	DisconnectFromXbox();

	DeleteCriticalSection( &m_csNotificationList );

	while( !m_lstNotifications.IsEmpty() )
	{
		delete m_lstNotifications.RemoveHead();
	}

	return COleControlModule::ExitInstance();
}


////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp::ConnectToXbox

void CXboxAddinApp::ConnectToXbox()
{
	HRESULT hr = S_OK;
	CString strErrorString;

	// Set the xbox machine name to connect to, if specified
	if( m_strXboxName.IsEmpty() )
	{
		// Must set Xbox machine name to connect to
		CTargetXboxDlg targetXboxDlg;
		if( IDOK != targetXboxDlg.DoModal() )
		{
			// If use canceled, abort connection
			hr = E_FAIL;
		}
	}

	// Try and set the Xbox name
	if( SUCCEEDED( hr ) )
	{
		hr = DmSetXboxNameNoRegister( m_strXboxName );

		if( FAILED( hr ) )
		{
			strErrorString.FormatMessage( IDS_ERR_CANT_SET_NAME, m_strXboxName );
		}
	}

	// Open our connection
	if( SUCCEEDED( hr ) )
	{
		hr = DmOpenConnection(&m_pdmConnection);
		if( FAILED( hr ) )
		{
			strErrorString.FormatMessage( IDS_ERR_CANT_CONNECT, m_strXboxName );
		}
	}

	if( SUCCEEDED( hr ) )
	{
		m_fConnected = true;

		// Make sure we'll be able to receive notifications
		hr = DmOpenNotificationSession( DM_PERSISTENT, &m_pdmnSession );
		if( FAILED( hr ) )
		{
			strErrorString.FormatMessage( IDS_ERR_CANT_CONNECT, m_strXboxName );
		}
	}

	if( SUCCEEDED( hr ) )
	{
		hr = DmNotify(m_pdmnSession, DM_DEBUGSTR, HandleDebugString);
		if( FAILED( hr ) )
		{
			strErrorString.FormatMessage( IDS_ERR_CANT_CONNECT, m_strXboxName );
		}
	}

	if( SUCCEEDED( hr ) )
	{
		hr = DmRegisterNotificationProcessor(m_pdmnSession, g_szCmd, ExtNotifyFunc);
		if( FAILED( hr ) )
		{
			strErrorString.FormatMessage( IDS_ERR_CANT_CONNECT, m_strXboxName );
		}
	}

	// send initial connect command to ECP so it knows we're here
	if( SUCCEEDED( hr ) )
	{
		DWORD cchResp = MAX_PATH;
		char szResp[MAX_PATH];

		hr = DmSendCommand(m_pdmConnection, TEXT(CMD_PREFIX "!__connect__"), szResp, &cchResp);

		if(FAILED(hr))
		{
			strErrorString.FormatMessage( IDS_ERR_CANT_CONNECT_DMCONSOLE, m_strXboxName );
		}
		/*
		else if( strcmp(szResp, "200- Version 0.1 Connected.") )
		{
			// Wrong version
			hr = E_FAIL;
			strErrorString.FormatMessage( IDS_ERR_AUDCONSOLE_VERSION, m_strXboxName );
		}
		*/
		else
		{
			/*
			strErrorString.Format( _T("Connected with response:\n%s"), szResp );
			XboxAddinMessageBox( NULL, strErrorString, MB_ICONINFORMATION | MB_OK );
			strErrorString.Empty();
			*/

			// Initialize the AudConsole app
			DmSendCommand(m_pdmConnection, TEXT(CMD_PREFIX "!initialize"), szResp, &cchResp);

			m_fECPConnected = true;

			// Verify destination directory
			bool fContinue = true;
			DM_FILE_ATTRIBUTES dmFileAttributes;
			HRESULT hr = DmGetFileAttributes( theApp.m_strXboxDestination, &dmFileAttributes );
			if( FAILED(hr) )
			{
				// Directory does not exist
				CString strXboxDirectory = CString(_T("X")) + theApp.m_strXboxDestination;
				CString strMessage;
				strMessage.FormatMessage( IDS_ERR_PATH_NOEXIST, strXboxDirectory );
				if( IDOK == XboxAddinMessageBox( NULL, strMessage, MB_ICONERROR | MB_OKCANCEL ) )
				{
					if( FAILED( DmMkdir( theApp.m_strXboxDestination ) ) )
					{
						fContinue = false;
					}
				}
				else
				{
					fContinue = false;
				}
			}
			else if( !(dmFileAttributes.Attributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// File with same name exists
				CString strXboxDirectory = CString(_T("X")) + theApp.m_strXboxDestination;
				CString strMessage;
				strMessage.FormatMessage( IDS_ERR_PATH_FILEEXIST, strXboxDirectory );
				XboxAddinMessageBox( NULL, strMessage, MB_ICONERROR | MB_OK );
				fContinue = false;
			}

			// If verification failed, prompt user to set the destination directory
			if( !fContinue )
			{
				CTargetDirectoryDlg targetDirectoryDlg;
				int nResult = targetDirectoryDlg.DoModal();
				if( nResult != IDC_SYNCHRONIZE
				&&	nResult != IDOK )
				{
					hr = E_FAIL;
				}
			}
		}
	}

	// If successfull, set our search directory
	if( SUCCEEDED( hr ) )
	{
		UpdateTargetDirectory();
	}

	if( FAILED( hr ) )
	{
		if( !strErrorString.IsEmpty() )
		{
			XboxAddinMessageBox( NULL, strErrorString, MB_ICONERROR | MB_OK );
		}

		// Disconnect
		DisconnectFromXbox();
	}
}


////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp::DisconnectFromXbox

void CXboxAddinApp::DisconnectFromXbox()
{
	if(m_fConnected)
	{
		//RtfPrintf(CLR_INVALID, "Closing connection\n");

		// Uninitialize the AudConsole app
		DWORD cchResp = MAX_PATH;
		char szResp[MAX_PATH];
		DmSendCommand(m_pdmConnection, TEXT(CMD_PREFIX "!uninitialize"), szResp, &cchResp);

		DmNotify(m_pdmnSession, DM_NONE, NULL);
		DmCloseNotificationSession( m_pdmnSession );
		DmCloseConnection(m_pdmConnection);
		m_fConnected = m_fECPConnected = false;
	}
	//return true;
}


////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp::UpdateTargetDirectory

void CXboxAddinApp::UpdateTargetDirectory()
{
	if(m_fConnected)
	{
		CString strRemoteCmd;
		char    szResp[MAX_PATH];
		DWORD   cchResp = MAX_PATH;
		HRESULT hr;

		// Clear the cache
		hr = DmSendCommand(theApp.m_pdmConnection, TEXT(CMD_PREFIX "!clearCache"), szResp, &cchResp);

		// Set the search directory
		strRemoteCmd.Format( TEXT(CMD_PREFIX "!searchDirectory \"%s\""), theApp.m_strXboxDestination );
		hr = DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);
	}
}


////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp::HandleNotification

void CXboxAddinApp::HandleNotification( NOTIFICATION_TYPE notificationType, TCHAR *astrTokens[10], int nParams )
{
	if( nParams < 2 )
	{
		TRACE("Too few arguments to HandleNotification\n");
		return;
	}

	DWORD dwSegmentID = _ttoi( astrTokens[1] );

	DWORD dwData1 = 0;
	if( nParams > 2 )
	{
		dwData1 = _ttoi( astrTokens[2] );
	}

	if( m_pXboxAddinComponent
	&&	m_pXboxAddinComponent->m_pXboxAddinDlg )
	{
		Notification_Struct *pNotification_Struct = new Notification_Struct;
		if( pNotification_Struct )
		{
			pNotification_Struct->notificationType = notificationType;
			pNotification_Struct->dwSegmentID = dwSegmentID;
			pNotification_Struct->dwData1 = dwData1;

			EnterCriticalSection( &m_csNotificationList );
			m_lstNotifications.AddTail( pNotification_Struct );
			LeaveCriticalSection( &m_csNotificationList );

			// Signal the dialog if this is a segment notification
			if( notificationType == NOTIFICATION_SEGMENT )
			{
				::PostMessage( m_pXboxAddinComponent->m_pXboxAddinDlg->GetSafeHwnd(), WM_APP, 0, 0 );
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp::GetNotification

Notification_Struct *CXboxAddinApp::GetNotification( NOTIFICATION_TYPE notificationType )
{
	Notification_Struct *pNotification_Struct = NULL;

	// Find a segment notification
	EnterCriticalSection( &theApp.m_csNotificationList );
	POSITION pos = theApp.m_lstNotifications.GetHeadPosition();
	while( pos )
	{
		POSITION posHere = pos;
		Notification_Struct *pTempNotification = theApp.m_lstNotifications.GetNext( pos );
		if( pTempNotification->notificationType == notificationType )
		{
			theApp.m_lstNotifications.RemoveAt( posHere );
			pNotification_Struct = pTempNotification;
			break;
		}
	}
	LeaveCriticalSection( &theApp.m_csNotificationList );

	return pNotification_Struct;
}


////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp::WaitForNotification

void CXboxAddinApp::WaitForNotification( NOTIFICATION_TYPE notificationType, Notification_Struct **ppNotification_Struct )
{
	// Wait for the command to be processed
	DWORD dwTimeOut = 50;
	while( true )
	{
		// Find a notification
		Notification_Struct *pNotification_Struct = GetNotification( notificationType );

		// If we found a notification
		if( pNotification_Struct )
		{
			// Return or delete it
			if( ppNotification_Struct )
			{
				*ppNotification_Struct = pNotification_Struct;
			}
			else
			{
				delete pNotification_Struct;
			}
			break;
		}

		// Make sure we time out after one second
		if( dwTimeOut == 0 )
		{
			break;
		}
		dwTimeOut--;

		// Wait
		Sleep(20);
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetRegString - Gets a string from the system registry 

BOOL GetRegString( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPTSTR lpszString, LPDWORD lpdwSize )
{
	HKEY  hKeyOpen;
	LONG  lResult;
	BOOL  fSuccess = FALSE;
	DWORD dwType;

	lResult = ::RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hKeyOpen );
	if( lResult == ERROR_SUCCESS )
	{
		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, 0, &dwType, (BYTE *)lpszString, lpdwSize );

		if( (dwType == REG_SZ) && (lResult == ERROR_SUCCESS) )
		{
			fSuccess = TRUE;
		}

		::RegCloseKey( hKeyOpen );
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// GetRegDWORD - Gets a DWORD from the system registry 

BOOL GetRegDWORD( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPDWORD lpDWORD )
{
	HKEY  hKeyOpen;
	LONG  lResult;
	BOOL  fSuccess = FALSE;
	DWORD dwType;

	lResult = ::RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hKeyOpen );
	if( lResult == ERROR_SUCCESS )
	{
        DWORD dwDataSize = sizeof(DWORD);
		lResult = ::RegQueryValueEx( hKeyOpen, lpValueName, 0, &dwType, (BYTE *)lpDWORD, &dwDataSize );

		if( (dwType == REG_DWORD) && (lResult == ERROR_SUCCESS) && (dwDataSize == sizeof(DWORD)) )
		{
			fSuccess = TRUE;
		}

		::RegCloseKey( hKeyOpen );
	}

	return fSuccess;
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

BOOL SetRegDWORD( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPDWORD lpDWORD, BOOL fOverWrite )
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
		||	(dwType != REG_DWORD)
		||	(fOverWrite == TRUE) )
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
// RegisterComponents - Adds registry entries for DMUS Producer components

static BOOL RegisterComponents( void )
{
	LPOLESTR psz1;
	TCHAR	 szRegPath[MAX_BUFFER];
	TCHAR	 szCLSID[SMALL_BUFFER];
	TCHAR	 szInProcServer32[SMALL_BUFFER];
	TCHAR	 szThreadingModel[SMALL_BUFFER];
	TCHAR	 szApartment[SMALL_BUFFER];
	TCHAR	 szOCXPath[MAX_BUFFER];
	TCHAR	 szOCXLongPath[MAX_BUFFER];
	TCHAR	 szGuid1[MID_BUFFER];
	CString  strName;
	TCHAR	 szComponentPath[MAX_BUFFER];
	TCHAR	 szSkip[SMALL_BUFFER];
	DWORD	 dwSkip = 0;
	
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

// Register Xbox addin Component
	if( SUCCEEDED( StringFromIID(CLSID_XboxAddinComponent, &psz1) ) )
	{
		WideCharToMultiByte( CP_ACP, 0, psz1, -1, szGuid1, sizeof(szGuid1), NULL, NULL );
		CoTaskMemFree( psz1 );
		strName.LoadString( IDS_XBOXADDIN_COMPONENT_NAME );

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
		||	!(SetRegString(HKEY_CLASSES_ROOT, szRegPath, szThreadingModel, szApartment)) )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, szComponentPath );
		_tcscat( szRegPath, szGuid1 );
		if( !(SetRegString(HKEY_LOCAL_MACHINE, szRegPath, _T(""), strName))
		||	!(SetRegDWORD(HKEY_LOCAL_MACHINE, szRegPath, szSkip, &dwSkip, FALSE)) )
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

	if( !RegisterComponents() )
	{
		return ResultFromScode(SELFREG_E_FIRST+2);
	}

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// UnregisterComponents - Removes registry entries for DMUS Producer components

static BOOL UnregisterComponents( void )
{
	LPOLESTR psz;
	TCHAR	 szRegPath[MAX_BUFFER];
	TCHAR	 szCLSID[SMALL_BUFFER];
	TCHAR	 szGuid[MID_BUFFER];
	TCHAR	 szComponentPath[MAX_BUFFER];
	TCHAR	 szContainerPath[MAX_BUFFER];
	TCHAR	 szRefNode[SMALL_BUFFER];
	
	_tcscpy( szCLSID, _T("CLSID") );
	_tcscpy( szComponentPath, _T("Software\\Microsoft\\DMUSProducer\\Components\\") );
	_tcscpy( szContainerPath, _T("Software\\Microsoft\\DMUSProducer\\Container Objects\\") );
	_tcscpy( szRefNode, _T("RefNode") );
	
// Unregister Xbox Addin Component
	if( SUCCEEDED( StringFromIID(CLSID_XboxAddinComponent, &psz) ) )
	{
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		_tcscpy( szRegPath, szCLSID );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, _T("InProcServer32") );
		// First delete the subkey
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, szCLSID );
		_tcscat( szRegPath, _T("\\") );
		_tcscat( szRegPath, szGuid );
		// Then the main key
		if( RegDeleteKey(HKEY_CLASSES_ROOT, szRegPath) != ERROR_SUCCESS )
		{
			return FALSE;
		}

		_tcscpy( szRegPath, szComponentPath );
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
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
	ULONG	m_dwRef;		 // interface reference count
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
	CreateInstanceFunc *m_pfunc;	// function that creates an object instance
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

CREATE_INSTANCE( CXboxAddinComponent )


/////////////////////////////////////////////////////////////////////////////
// DllGetClassObject

STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*ppv = NULL;

	// this DLL can only create class factory objects that support
	// IUnknown and IClassFactory
	if( !IsEqualIID(riid, IID_IUnknown)
	&&	!IsEqualIID(riid, IID_IClassFactory) )
	{
		return E_NOINTERFACE;
	}

// point <pfunc> to a function that can create a new object instance
	CreateInstanceFunc *pfunc;

	if( IsEqualCLSID(rclsid, CLSID_XboxAddinComponent) )
	{
		pfunc = CXboxAddinComponent_CreateInstance;
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

int XboxAddinMessageBox( CWnd *pWnd, LPCTSTR lpszText, UINT nType, LPCTSTR lpszCaption )
{
	UNREFERENCED_PARAMETER(pWnd);
	CString strCaption;
	if( lpszCaption)
	{
		strCaption = lpszCaption;
	}
	else
	{
		strCaption.LoadString( IDS_XBOXADDIN_COMPONENT_NAME );
	}

	return MessageBox( NULL, lpszText, strCaption, nType | MB_TASKMODAL );
}
