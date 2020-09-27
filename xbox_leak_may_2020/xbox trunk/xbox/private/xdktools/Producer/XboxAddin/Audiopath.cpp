// Audiopath.cpp : implementation file
//

#include "stdafx.h"
#include "Audiopath.h"
#include "XboxAddin.h"
#include "XboxAddinComponent.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <Xbox-dmusici.h>
#pragma warning ( pop )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CAudiopath::m_sdwNextIndex = 0;

CAudiopath::CAudiopath( IDMUSProdNode *pDMUSProdNode, DWORD dwType ) : CFileItem(pDMUSProdNode)
{
	m_dwIndex = ++m_sdwNextIndex; // Must be one or greater
	m_fDefault = false;
	m_dwStandardType = dwType;
	m_dwStandardPChannels = 128;

	switch( dwType )
	{
	case DMUS_APATH_SHARED_STEREOPLUSREVERB:
		m_strDisplayName.LoadString( IDS_APATH_SHARED_STEREOPLUSREVERB );
		break;
	case DMUS_APATH_DYNAMIC_3D:
		m_strDisplayName.LoadString( IDS_APATH_DYNAMIC_3D );
		break;
	case DMUS_APATH_DYNAMIC_MONO:
		m_strDisplayName.LoadString( IDS_APATH_DYNAMIC_MONO );
		break;
	case DMUS_APATH_SHARED_STEREO:
		m_strDisplayName.LoadString( IDS_APATH_SHARED_STEREO );
		break;
	case DMUS_APATH_MIXBIN_QUAD:
		m_strDisplayName.LoadString( IDS_APATH_MIXBIN_QUAD );
		break;
	case DMUS_APATH_MIXBIN_QUAD_ENV:
		m_strDisplayName.LoadString( IDS_APATH_MIXBIN_QUAD_ENV );
		break;
	case DMUS_APATH_MIXBIN_QUAD_MUSIC:
		m_strDisplayName.LoadString( IDS_APATH_MIXBIN_QUAD_MUSIC );
		break;
	case DMUS_APATH_MIXBIN_5DOT1:
		m_strDisplayName.LoadString( IDS_APATH_MIXBIN_5DOT1 );
		break;
	case DMUS_APATH_MIXBIN_5DOT1_ENV:
		m_strDisplayName.LoadString( IDS_APATH_MIXBIN_5DOT1_ENV );
		break;
	case DMUS_APATH_MIXBIN_5DOT1_MUSIC:
		m_strDisplayName.LoadString( IDS_APATH_MIXBIN_5DOT1_MUSIC );
		break;
	case DMUS_APATH_MIXBIN_STEREO_EFFECTS:
		m_strDisplayName.LoadString( IDS_APATH_MIXBIN_STEREO_EFFECTS );
		break;
	}

	if( dwType )
	{
		m_strFileName = m_strDisplayName;
	}
}

HRESULT CAudiopath::CopyToXbox( void )
{
	HRESULT hr = S_OK;

	// Handle to the framework's progress bar
	HANDLE hProgressBar = NULL;

	// Check if this is a standard audiopath
	if( m_dwStandardType ) 
	{
		CString strRemoteCmd;
		char    szResp[MAX_PATH];
		DWORD   cchResp = MAX_PATH;

		strRemoteCmd.Format( TEXT(CMD_PREFIX "!createStandardAudiopath %d %d %d"), m_dwIndex, m_dwStandardType, m_dwStandardPChannels );

		// Send the command to the Xbox
		DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);
	}
	else
	{
		hr = CFileItem::CopyToXbox();

		if( FAILED(hr) )
		{
			return hr;
		}

		// Initialize the progress bar
		CString strProgressText;
		strProgressText.FormatMessage( IDS_PROGRESS_CREATE, m_strDisplayName );
		theApp.m_pXboxAddinComponent->m_pIFramework->StartProgressBar( 0, 1, strProgressText.AllocSysString(), &hProgressBar );

		CString strTargetFile = theApp.m_strXboxDestination + TEXT("\\") + m_strFileName;
		CString strRemoteCmd;
		char    szResp[MAX_PATH];
		DWORD   cchResp = MAX_PATH;

		strRemoteCmd.Format( TEXT(CMD_PREFIX "!createAudiopath %d \"%s\""), m_dwIndex, strTargetFile );

		// Send the command to the Xbox
		DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);
	}

	// Wait for the command to be processed
	Notification_Struct *pNotification_Struct = NULL;
	theApp.WaitForNotification( NOTIFICATION_CREATE_RESULT, &pNotification_Struct );
	if( !pNotification_Struct )
	{
		CString strLoadFailed;
    	strLoadFailed.FormatMessage( IDS_ERR_CREATE_TIMEOUT, GetName() );

    	while( IDRETRY == XboxAddinMessageBox( NULL, strLoadFailed, MB_ICONERROR | MB_RETRYCANCEL ) )
		{
			theApp.WaitForNotification( NOTIFICATION_CREATE_RESULT, &pNotification_Struct );
			if( pNotification_Struct )
			{
				break;
			}
		}

		if( !pNotification_Struct )
		{
			hr = E_FAIL;
		}
	}

	if( pNotification_Struct )
	{
		hr = pNotification_Struct->dwData1;
		if( FAILED( hr ) )
		{
			CString strLoadFailed;
    		strLoadFailed.FormatMessage( IDS_ERR_CREATE, GetName() );

    		XboxAddinMessageBox( NULL, strLoadFailed, MB_ICONERROR | MB_OK );
		}
		delete pNotification_Struct;
	}

	// Remove the progress bar
	if( hProgressBar )
	{
		theApp.m_pXboxAddinComponent->m_pIFramework->EndProgressBar( hProgressBar );
	}

	return hr;
}

HRESULT CAudiopath::Release( void )
{
	// Release the audiopath
	CString strRemoteCmd;
	char    szResp[MAX_PATH];
	DWORD   cchResp = MAX_PATH;

	strRemoteCmd.Format( TEXT(CMD_PREFIX "!releaseAudiopath %d "), m_dwIndex );

	// Send the command to the Xbox
	DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);

	// Wait for the command to be processed
	Notification_Struct *pNotification_Struct = NULL;
	theApp.WaitForNotification( NOTIFICATION_RELEASE_RESULT, &pNotification_Struct );

	HRESULT hr = E_FAIL;

	if( pNotification_Struct )
	{
		hr = pNotification_Struct->dwData1;
		delete pNotification_Struct;
	}

	return hr;
}

HRESULT CAudiopath::SetDefault( bool fDefault )
{
	HRESULT hr = E_FAIL;

	// Update m_strDisplayName
	SetAppendValue( m_nAppendValue );

	if( fDefault )
	{
		// Send the command to the Xbox
		CString strRemoteCmd;
		char    szResp[MAX_PATH];
		DWORD   cchResp = MAX_PATH;
		strRemoteCmd.Format( TEXT(CMD_PREFIX "!setDefaultAudiopath %d "), m_dwIndex );
		DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);

		// Wait for the command to be processed
		Notification_Struct *pNotification_Struct = NULL;
		theApp.WaitForNotification( NOTIFICATION_SETDEFAULT_RESULT, &pNotification_Struct );

		if( pNotification_Struct )
		{
			hr = pNotification_Struct->dwData1;
			if( FAILED( hr ) )
			{
				CString strLoadFailed;
    			strLoadFailed.FormatMessage( IDS_ERR_SETDEFAULT, GetName() );

    			XboxAddinMessageBox( NULL, strLoadFailed, MB_ICONERROR | MB_OK );
			}
			delete pNotification_Struct;
		}

		if( SUCCEEDED(hr) )
		{
			m_fDefault = true;

			// Update the display name
			CString strCurrentName = m_strDisplayName;
			m_strDisplayName.FormatMessage( IDS_DEFAULT_AUDIOPATH, strCurrentName );
		}
	}
	else if( m_fDefault )
	{
		m_fDefault = false;
		// Leave m_strDisplayName alone
		hr = S_OK;
	}

	return hr;
}

HRESULT CAudiopath::RemoveFromXbox( void )
{
	// If audiopath is based on a file, remove it
	if( m_pFileNode )
	{
		return CFileItem::RemoveFromXbox();
	}

	// Otherwise, return S_OK
	return S_OK;
}
