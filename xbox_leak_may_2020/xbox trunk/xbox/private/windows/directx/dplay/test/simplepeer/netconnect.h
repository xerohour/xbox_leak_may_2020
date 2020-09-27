//-----------------------------------------------------------------------------
// File: NetConnect.h
//
// Desc:
//
//@@BEGIN_MSINTERNAL
//
// Hist: See source file for detailed history
//
//@@END_MSINTERNAL
// Copyright (C) 2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef NETCONNECT_H
#define NETCONNECT_H


#include <windows.h>
#include <dplay8.h>
#include <dpaddr.h>
#include <tchar.h>


//-----------------------------------------------------------------------------
// Defines, structures, and error codes
//-----------------------------------------------------------------------------
#define DISPLAY_REFRESH_RATE        250
#define TIMERID_DISPLAY_HOSTS       1
#define TIMERID_CONNECT_COMPLETE    2

#define NCW_S_FORWARD      0x01000001  // Dialog success, so go forward
#define NCW_S_BACKUP       0x01000002  // Dialog canceled, show previous dialog
#define NCW_S_QUIT         0x01000003  // Dialog quit, close app


class CNetConnectWizard
{
public:
	CNetConnectWizard( HINSTANCE hInst, TCHAR* strAppName, GUID* pGuidApp );
	virtual ~CNetConnectWizard();

    HRESULT WINAPI MessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );

    VOID    Init( IDirectPlay8Peer* pDP );
    HRESULT DoConnectWizard( BOOL bBackTrack );

    void   SetMaxPlayers( DWORD dwMaxPlayers )                 { m_dwMaxPlayers = dwMaxPlayers; }
    void   SetPlayerName( TCHAR* strPlayerName )               { _tcscpy( m_strLocalPlayerName, strPlayerName ); }
    void   SetSessionName( TCHAR* strSessionName )             { _tcscpy( m_strSessionName, strSessionName ); }
    void   SetPreferredProvider( TCHAR* strPreferredProvider ) { _tcscpy( m_strPreferredProvider, strPreferredProvider ); }

    TCHAR* GetPlayerName()                                     { return m_strLocalPlayerName; }
    TCHAR* GetSessionName()                                    { return m_strSessionName; }
    TCHAR* GetPreferredProvider()                              { return m_strPreferredProvider; }
    BOOL   IsHostPlayer()                                      { return m_bHostPlayer; }
    BOOL   IsMigrateHost()                                     { return m_bMigrateHost; }

protected:
    struct DPHostEnumInfo
    {
        DWORD                 dwRef;
        DPN_APPLICATION_DESC* pAppDesc;
        IDirectPlay8Address* pHostAddr;
        IDirectPlay8Address* pDeviceAddr;
		TCHAR                szSession[MAX_PATH];
        DWORD                dwLastPollTime;
        BOOL                 bValid;
        DPHostEnumInfo*      pNext;
    };

    static INT_PTR CALLBACK StaticConnectionsDlgProc( HWND, UINT, WPARAM, LPARAM );
    static INT_PTR CALLBACK StaticSessionsDlgProc( HWND, UINT, WPARAM, LPARAM );
    static INT_PTR CALLBACK StaticCreateSessionDlgProc( HWND, UINT, WPARAM, LPARAM );
    INT_PTR CALLBACK ConnectionsDlgProc( HWND, UINT, WPARAM, LPARAM );
    INT_PTR CALLBACK SessionsDlgProc( HWND, UINT, WPARAM, LPARAM );
    INT_PTR CALLBACK CreateSessionDlgProc( HWND, UINT, WPARAM, LPARAM );

    HRESULT ConnectionsDlgFillListBox( HWND hDlg );
    HRESULT ConnectionsDlgOnOK( HWND hDlg );
    VOID    ConnectionsDlgCleanup( HWND hDlg );

    VOID    SessionsDlgInitListbox( HWND hDlg );
    HRESULT SessionsDlgEnumHosts( HWND hDlg );
    HRESULT SessionsDlgNoteEnumResponse( PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponse );
    VOID    SessionsDlgExpireOldHostEnums();
    HRESULT SessionsDlgDisplayEnumList( HWND hDlg );
    HRESULT SessionsDlgJoinGame( HWND hDlg );
    HRESULT SessionsDlgCreateGame( HWND hDlg );
    VOID    SessionsDlgEnumListCleanup();

    IDirectPlay8Peer*       m_pDP;
    CRITICAL_SECTION        m_csHostEnum;
    GUID                    m_guidApp;
    HRESULT                 m_hrDialog;
    HWND                    m_hDlg;
    HINSTANCE               m_hInst;
    DWORD                   m_dwMaxPlayers;
    TCHAR                   m_strAppName[MAX_PATH];
    TCHAR                   m_strPreferredProvider[MAX_PATH];
    TCHAR                   m_strSessionName[MAX_PATH];
    TCHAR                   m_strLocalPlayerName[MAX_PATH];
    BOOL                    m_bSearchingForSessions;
    BOOL                    m_bMigrateHost;
    IDirectPlay8Address*    m_pDeviceAddress;
    IDirectPlay8Address*    m_pHostAddress;
    DPHostEnumInfo          m_DPHostEnumHead;
    BOOL                    m_bEnumListChanged;
    DPNHANDLE               m_hEnumAsyncOp;
    BOOL                    m_bHostPlayer;
    DWORD                   m_dwEnumHostExpireInterval;
    BOOL                    m_bConnecting;
    DPNHANDLE               m_hConnectAsyncOp;
    HANDLE                  m_hConnectCompleteEvent;
    HANDLE                  m_hLobbyConnectionEvent;
    HRESULT                 m_hrConnectComplete;
};


#endif // NETCONNECT_H

