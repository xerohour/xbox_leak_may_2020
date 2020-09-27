// XboxAddin.h : main header file for the XBOXADDIN application
//

#if !defined(AFX_XBOXADDIN_H__9B6CC530_E352_4D98_9CBA_CAA4A82C67B7__INCLUDED_)
#define AFX_XBOXADDIN_H__9B6CC530_E352_4D98_9CBA_CAA4A82C67B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <xboxdbg.h>


#define RELEASE(x) if( (x) ) {(x)->Release(); (x) = 0;};
#define CMD_PREFIX      "XAUD"

class CXboxAddinComponent;

BOOL GetRegString( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPTSTR lpszString, LPDWORD lpdwSize );
BOOL SetRegString( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpszString );
BOOL SetRegDWORD( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPDWORD lpDWORD, BOOL fOverWrite );
BOOL GetRegDWORD( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPDWORD lpDWORD );

int XboxAddinMessageBox( CWnd *pWnd, LPCTSTR lpszText, UINT nType, LPCTSTR lpszCaption = NULL ); 

typedef enum
{
	NOTIFICATION_UNKNOWN,
    NOTIFICATION_SEGMENT,
    NOTIFICATION_LOAD_RESULT,
    NOTIFICATION_UNLOAD_RESULT,
    NOTIFICATION_PLAY_RESULT,
    NOTIFICATION_CLEARCACHE_RESULT,
	NOTIFICATION_PANIC_RESULT,
	NOTIFICATION_CREATE_RESULT,
	NOTIFICATION_SETDEFAULT_RESULT,
	NOTIFICATION_RELEASE_RESULT,
} NOTIFICATION_TYPE;

typedef struct
{
	NOTIFICATION_TYPE notificationType;
	DWORD			  dwSegmentID;
	DWORD			  dwData1;
} Notification_Struct;

/////////////////////////////////////////////////////////////////////////////
// GetUserRegString - Gets a string from the user's registry key

inline BOOL GetUserRegString( LPCTSTR lpValueName, LPTSTR lpszString, LPDWORD lpdwSize )
{
	return GetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"),
		lpValueName, lpszString, lpdwSize );
}

/////////////////////////////////////////////////////////////////////////////
// SetUserRegString - Sets a string in the new PortConfig registry key

inline BOOL SetUserRegString( LPCTSTR lpValueName, LPCTSTR lpszString )
{
	return SetRegString( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"),
		lpValueName, lpszString );
}

/////////////////////////////////////////////////////////////////////////////
// GetUserRegDWORD - Gets a string from the user's registry key

inline BOOL GetUserRegDWORD( LPCTSTR lpValueName, LPDWORD lpdwValue )
{
	return GetRegDWORD( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"),
		lpValueName, lpdwValue );
}

/////////////////////////////////////////////////////////////////////////////
// SetUserRegDWORD - Sets a string in the new PortConfig registry key

inline BOOL SetUserRegDWORD( LPCTSTR lpValueName, LPDWORD lpdwValue )
{
	return SetRegDWORD( HKEY_CURRENT_USER, _T("Software\\Microsoft\\DMUSProducer\\XboxAddin\\"),
		lpValueName, lpdwValue, TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinApp:
// See XboxAddin.cpp for the implementation of this class
//

class CXboxAddinApp : public COleControlModule
{
public:
	CXboxAddinApp();

public:
	BOOL InitInstance();
	int ExitInstance();
	void ConnectToXbox();
	void DisconnectFromXbox();
    void UpdateTargetDirectory();
    void HandleNotification( NOTIFICATION_TYPE notificationType, TCHAR *astrTokens[10], int nParams );
	Notification_Struct *GetNotification( NOTIFICATION_TYPE notificationType );
	void WaitForNotification( NOTIFICATION_TYPE notificationType, Notification_Struct **ppNotification_Struct = NULL );

public:
	CXboxAddinComponent*			m_pXboxAddinComponent;
	CString 						m_strXboxDestination;
	CString							m_strXboxName;

	bool							m_fConnected;	// Connected to Xbox?
	bool							m_fECPConnected; // Connected to ECP in application?
	bool							m_fDebugMonitor; // Display debug output?

	PDMN_SESSION					m_pdmnSession; 	// Debug Monitor Session
	PDM_CONNECTION					m_pdmConnection;	// Debug Monitor Connection

	// Notification list
	CRITICAL_SECTION				m_csNotificationList;
	CTypedPtrList< CPtrList, Notification_Struct *> m_lstNotifications;
};

extern CXboxAddinApp theApp;


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBOXADDIN_H__9B6CC530_E352_4D98_9CBA_CAA4A82C67B7__INCLUDED_)
