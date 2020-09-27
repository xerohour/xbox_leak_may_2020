//-----------------------------------------------------------------------------
// File: XbNet.cpp
//
// Desc: Shortcut macros and helper functions for the Xbox networking samples
//       Requires linking with XNET[D][S].LIB or XONLINE[D][S].LIB
//
// Hist: 09.06.01 - New for November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XBNet.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Check network link status twice/sec
const FLOAT CHECK_LINK_STATUS = 0.5f;




//-----------------------------------------------------------------------------
// Name: XBNet_Init()
// Desc: Performs all required initialization for network play
//-----------------------------------------------------------------------------
HRESULT XBNet_Init( BYTE cfgFlags )
{
    // Initialize the network stack. For default initialization, call
    // XNetStartup( NULL );
    XNetStartupParams xnsp;
    ZeroMemory( &xnsp, sizeof(xnsp) );
    xnsp.cfgSizeOfStruct = sizeof(xnsp);
    xnsp.cfgFlags = cfgFlags;
    INT iResult = XNetStartup( &xnsp );
    if( iResult != NO_ERROR )
        return E_FAIL;

    // Standard WinSock startup. The Xbox allows all versions of Winsock
    // up through 2.2 (i.e. 1.0, 1.1, 2.0, 2.1, and 2.2), although it 
    // technically supports only and exactly what is specified in the 
    // Xbox network documentation, not necessarily the full Winsock 
    // functional specification.
    WSADATA WsaData;
    iResult = WSAStartup( MAKEWORD(2,2), &WsaData );
    if( iResult != NO_ERROR )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: XBNet_OnlineInit()
// Desc: Performs all required initialization for XOnline and network APIs
//-----------------------------------------------------------------------------
HRESULT XBNet_OnlineInit( BYTE cfgFlags, PXONLINE_STARTUP_PARAMS pxosp)
{
	XONLINE_STARTUP_PARAMS xosp = { 0 };

	if( pxosp == NULL )
	{
		pxosp = &xosp;
	}

    HRESULT hr = XBNet_Init( cfgFlags );
    if( SUCCEEDED( hr ) )
    {
        hr = XOnlineStartup( pxosp );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: XBNet_Cleanup()
// Desc: Performs all required cleanup for network play
//-----------------------------------------------------------------------------
HRESULT XBNet_Cleanup()
{
    // Shutdown sockets
    INT iResult = WSACleanup();
    BOOL bSuccess = ( iResult == NO_ERROR );

    // Shutdown network stack
    // Always call XNetCleanup even if WSACleanup failed
    iResult = XNetCleanup();
    bSuccess &= ( iResult == NO_ERROR );

    return( bSuccess ? S_OK : E_FAIL );
}




//-----------------------------------------------------------------------------
// Name: XBNet_OnlineCleanup()
// Desc: Performs all required cleanup for online APIs
//-----------------------------------------------------------------------------
HRESULT XBNet_OnlineCleanup()
{

  HRESULT hr = XOnlineCleanup();

  if( SUCCEEDED( hr ) )
  {
      hr = XBNet_Cleanup();
  }

  return hr;
}




//-----------------------------------------------------------------------------
// Name: CXBNetLink()
// Desc: Create network link checker
//-----------------------------------------------------------------------------
CXBNetLink::CXBNetLink()
:
    m_LinkStatusTimer( FALSE ) // don't start timer yet
{
}




//-----------------------------------------------------------------------------
// Name: IsActive()
// Desc: Returns TRUE if network link is active; FALSE if not
//-----------------------------------------------------------------------------
BOOL CXBNetLink::IsActive() const
{
    BOOL bIsActive = TRUE;

    // For speed, only check every half second
    if( !m_LinkStatusTimer.IsRunning() ||
         m_LinkStatusTimer.GetElapsedSeconds() > CHECK_LINK_STATUS )
    {
        // Reset timer
        m_LinkStatusTimer.StartZero();

        // Poll status
        DWORD dwStatus = XNetGetEthernetLinkStatus();
        bIsActive = ( dwStatus & XNET_ETHERNET_LINK_ACTIVE ) != 0;
    }

    return bIsActive;
}
