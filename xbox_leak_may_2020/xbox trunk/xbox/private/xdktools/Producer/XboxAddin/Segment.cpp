// Segment.cpp : implementation file
//

#include "stdafx.h"
#include "Segment.h"
#include "XboxAddin.h"
#include "XboxAddinComponent.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <Xbox-dmusici.h>
#pragma warning ( pop )
#include "Audiopath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CSegment::m_sdwNextIndex = 0;

CSegment::CSegment( IDMUSProdNode *pDMUSProdNode ) : CFileItem(pDMUSProdNode)
{
	m_fPlaying = false;
	m_fPressed = false;
	m_dwPlayFlags = DMUS_SEGF_DEFAULT;
	m_pAudiopath = NULL;
	m_dwStandardAudiopath = 0;

	m_dwIndex = ++m_sdwNextIndex;
}

void CSegment::ButtonClicked( void )
{
	CString strRemoteCmd;
    char    szResp[MAX_PATH];
    DWORD   cchResp = MAX_PATH;
    HRESULT hr;

	if( m_fPlaying )
	{
		strRemoteCmd.Format( TEXT(CMD_PREFIX "!stop %d %d"), m_dwIndex, m_dwPlayFlags );
	}
	else
	{
		DWORD dwAudiopathIndex = 0;
		if( m_pAudiopath )
		{
			dwAudiopathIndex = m_pAudiopath->m_dwIndex;
		}
		strRemoteCmd.Format( TEXT(CMD_PREFIX "!play %d %d %d %d"), m_dwIndex, m_dwPlayFlags, dwAudiopathIndex, m_dwStandardAudiopath );
	}


	// Send the command to the Xbox
	hr = DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);

	// If we're not playing
	if( !m_fPlaying )
	{
		// Wait for the command to be processed
		Notification_Struct *pNotification_Struct = NULL;
		theApp.WaitForNotification( NOTIFICATION_PLAY_RESULT, &pNotification_Struct );
		if( !pNotification_Struct )
		{
			CString strPlayFailed;
			strPlayFailed.FormatMessage( IDS_ERR_PLAY_TIMEOUT, GetName() );
			XboxAddinMessageBox( NULL, strPlayFailed, MB_ICONERROR | MB_OK );
		}
		else
		{
			if( FAILED( pNotification_Struct->dwData1 ) )
			{
				CString strPlayFailed;
				strPlayFailed.FormatMessage( IDS_ERR_PLAY, GetName() );
				XboxAddinMessageBox( NULL, strPlayFailed, MB_ICONERROR | MB_OK );
			}
			else
			{
				m_fPlaying = true;
			}
			delete pNotification_Struct;
		}
	}

	// OnSegEnd sets m_fPlaying to false when the segment stops
}

HRESULT CSegment::CopyToXbox( void )
{
	HRESULT hr = CFileItem::CopyToXbox();

	if( FAILED(hr) )
	{
		return hr;
	}

	// Handle to the framework's progress bar
	HANDLE hProgressBar = NULL;

	// Initialize the progress bar
	CString strProgressText;
	strProgressText.FormatMessage( IDS_PROGRESS_DOWNLOAD, m_strDisplayName );
	theApp.m_pXboxAddinComponent->m_pIFramework->StartProgressBar( 0, 1, strProgressText.AllocSysString(), &hProgressBar );

	CString strTargetFile = theApp.m_strXboxDestination + TEXT("\\") + m_strFileName;
	CString strRemoteCmd;
	char    szResp[MAX_PATH];
	DWORD   cchResp = MAX_PATH;

	strRemoteCmd.Format( TEXT(CMD_PREFIX "!load %d \"%s\""), m_dwIndex, strTargetFile );

	// Send the command to the Xbox
	DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);

	// Wait for the command to be processed
	Notification_Struct *pNotification_Struct = NULL;
	theApp.WaitForNotification( NOTIFICATION_LOAD_RESULT, &pNotification_Struct );
	if( !pNotification_Struct )
	{
		CString strLoadFailed;
    	strLoadFailed.FormatMessage( IDS_ERR_LOAD_TIMEOUT, GetName() );

    	while( IDRETRY == XboxAddinMessageBox( NULL, strLoadFailed, MB_ICONERROR | MB_RETRYCANCEL ) )
		{
			theApp.WaitForNotification( NOTIFICATION_LOAD_RESULT, &pNotification_Struct );
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
    		strLoadFailed.FormatMessage( IDS_ERR_LOAD, GetName() );

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

void CSegment::OnSegEnd( void )
{
	ASSERT( m_fPlaying );
	m_fPlaying = false;
}

HRESULT CSegment::Unload( void )
{
	// Stop the segment by faking a click on the Stop button
	if( m_fPlaying )
	{
		ButtonClicked();
	}

	// Unload the segment
	CString strRemoteCmd;
	char    szResp[MAX_PATH];
	DWORD   cchResp = MAX_PATH;

	strRemoteCmd.Format( TEXT(CMD_PREFIX "!unload %d "), m_dwIndex );

	// Send the command to the Xbox
	DmSendCommand(theApp.m_pdmConnection, strRemoteCmd, szResp, &cchResp);

	// Wait for the command to be processed
	Notification_Struct *pNotification_Struct = NULL;
	theApp.WaitForNotification( NOTIFICATION_UNLOAD_RESULT, &pNotification_Struct );

	HRESULT hr = E_FAIL;

	if( pNotification_Struct )
	{
		hr = pNotification_Struct->dwData1;
		delete pNotification_Struct;
	}

	return hr;
}
