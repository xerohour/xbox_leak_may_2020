// Implementation of CNotifySink

#include "stdafx.h"
#include "CConduct.h"
#include "Toolbar.h"
#include "SecondaryToolbar.h"
#include "OutputTool.h"
//#include <fstream.h>
#include "StatusToolbar.h"
#include "WaveRecordToolbar.h"
#include "..\WaveSaveDmo\testdmo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMIDIInputContainer* g_pMIDIInputContainer;
extern void EchoBuffer( IDirectMusicBuffer *pDMBuffer );

/*
struct _V_GENERIC_INTERFACE
{
    FARPROC *(__vptr[1]);
};
*/

//extern ofstream ofsMessageOutput;

//#define VERBOSE_MIDI_INPUT_SPEW

/*
HRESULT SendMIDIOutMessage( IDirectMusicPerformance *pPerformance, DWORD dwPChannel, REFERENCE_TIME rtNow, BYTE bStatus, BYTE bByte1, BYTE bByte2 )
{
	if( g_pMIDIInputContainer && g_pMIDIInputContainer->m_fThruingMIDI )
	{
		ASSERT( pPerformance );

		HRESULT hr;
		DMUS_PMSG *pPipelineEvent = NULL;
		DMUS_MIDI_PMSG *pDMMidiEvent = NULL;

		// Allocate MIDI message
		hr = pPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pDMMidiEvent );
		if ( FAILED(hr) )
		{
			return hr;
		}
		memset( pDMMidiEvent, 0, sizeof(DMUS_MIDI_PMSG) );

		pDMMidiEvent->bStatus = bStatus;
		pDMMidiEvent->bByte1 = bByte1;
		pDMMidiEvent->bByte2 = bByte2;
		pDMMidiEvent->rtTime = rtNow;
		pDMMidiEvent->dwFlags = DMUS_PMSGF_TOOL_QUEUE | DMUS_PMSGF_REFTIME;
		pDMMidiEvent->dwPChannel = dwPChannel;
		//pDMMidiEvent->dwVirtualTrackID = 0;
		pDMMidiEvent->dwType = DMUS_PMSGT_MIDI;
		pDMMidiEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
		pPipelineEvent = (DMUS_PMSG*) pDMMidiEvent;

		//char cLine[200];
		//sprintf( cLine, "PMSG: %x %x %x %I64d\n", bStatus, bByte1, bByte2, rtNow);
		//ofsMessageOutput<<cLine;
		return pPerformance->SendPMsg( pPipelineEvent );
	}
	return S_OK;
}
*/


bool IsAudiopathPerformanceDefault( const IDirectMusicAudioPath *pIDirectMusicAudioPath )
{
	IDirectMusicAudioPath *pPerfIDirectMusicAudioPath;
	if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetDefaultAudioPath( &pPerfIDirectMusicAudioPath ) ) )
	{
		if( pPerfIDirectMusicAudioPath )
		{
			pPerfIDirectMusicAudioPath->Release();
		}

		if( pIDirectMusicAudioPath == pPerfIDirectMusicAudioPath )
		{
			return true;
		}
	}

	return false;
}

bool AudiopathHasWaveSaveDMO( IDirectMusicAudioPath *pIDirectMusicAudioPath )
{
	if( pIDirectMusicAudioPath == NULL )
	{
		return false;
	}

	if( IsAudiopathPerformanceDefault( pIDirectMusicAudioPath ) )
	{
		return false;
	}

	bool fResult = false;
	IDirectSoundBuffer *pIDirectSoundBuffer;
	DWORD dwBufferIndex = 0;
	while( !fResult
		&&	(S_OK == pIDirectMusicAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, dwBufferIndex,
					GUID_All_Objects, 0, IID_IDirectSoundBuffer, (void**) &pIDirectSoundBuffer )) )
	{
		IDump* pIDump = NULL;
		DWORD dwDMOIndex = 0;
		while( !fResult
			&&	(S_OK == pIDirectMusicAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER_DMO, dwBufferIndex,
						GUID_DSFX_STANDARD_DUMP, dwDMOIndex, IID_IDump, (void**) &pIDump )) )
		{
			fResult = true;
			dwDMOIndex++;
			pIDump->Release();
		}

		dwBufferIndex++;
		pIDirectSoundBuffer->Release();
	}
	dwBufferIndex = 0;
	while( !fResult
		&&	(S_OK == pIDirectMusicAudioPath->GetObjectInPath( 0, DMUS_PATH_MIXIN_BUFFER, dwBufferIndex,
					GUID_All_Objects, 0, IID_IDirectSoundBuffer, (void**) &pIDirectSoundBuffer )) )
	{
		IDump* pIDump = NULL;
		DWORD dwDMOIndex = 0;
		while( !fResult
			&&	(S_OK == pIDirectMusicAudioPath->GetObjectInPath( 0, DMUS_PATH_MIXIN_BUFFER_DMO, dwBufferIndex,
						GUID_DSFX_STANDARD_DUMP, dwDMOIndex, IID_IDump, (void**) &pIDump )) )
		{
			fResult = true;
			dwDMOIndex++;
			pIDump->Release();
		}

		dwBufferIndex++;
		pIDirectSoundBuffer->Release();
	}

	return fResult;
}

void AddToolsAndSetupWaveSaveForSegState( IUnknown *punk )
{
	IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
	if( punk
	&&	SUCCEEDED( punk->QueryInterface( IID_IDirectMusicSegmentState8, (void**)&pIDirectMusicSegmentState8 ) ) )
	{
		IDirectMusicAudioPath *pIDirectMusicAudioPath;
		if( SUCCEEDED( pIDirectMusicSegmentState8->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0, GUID_All_Objects, 0, IID_IDirectMusicAudioPath, (void **)&pIDirectMusicAudioPath ) ) )
		{
			if( !IsAudiopathPerformanceDefault( pIDirectMusicAudioPath ) )
			{
				if( AudiopathHasWaveSaveDMO( pIDirectMusicAudioPath ) )
				{
					SegStateAudioPath *pSegStateAudioPath = new SegStateAudioPath( pIDirectMusicAudioPath, pIDirectMusicSegmentState8 );
					if( pSegStateAudioPath )
					{
						::EnterCriticalSection( &g_pconductor->m_csAudiopathList );
						POSITION pos = g_pconductor->m_lstSegStateAudioPaths.GetHeadPosition();
						while( pos )
						{
							if( g_pconductor->m_lstSegStateAudioPaths.GetNext( pos )->pSegmentState == pIDirectMusicSegmentState8 )
							{
								delete pSegStateAudioPath;
								pSegStateAudioPath = NULL;
								break;
							}
						}

						if( pSegStateAudioPath )
						{
							g_pconductor->SetupWaveSaveDMO( pIDirectMusicAudioPath );
							g_pconductor->m_lstSegStateAudioPaths.AddTail( pSegStateAudioPath );
						}
						::LeaveCriticalSection( &g_pconductor->m_csAudiopathList );
					}
				}
				g_pconductor->AddToolsToSegState( pIDirectMusicSegmentState8, g_pconductor->m_pOutputTool, g_pconductor->m_pMIDISaveTool );
			}
			pIDirectMusicAudioPath->Release();
		}
		pIDirectMusicSegmentState8->Release();
	}
}

long g_lMaxNotifyThreads = 10;

UINT AFX_CDECL NotifyThreadProc( LPVOID pParam )
{
	UNREFERENCED_PARAMETER(pParam);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( g_pconductor == NULL )
	{
		ASSERT(FALSE);
		return UINT_MAX;    // illegal parameter
	}

	BOOL fInCritSeq = FALSE;

	ASSERT( g_pconductor->m_hNotifyEvent != NULL );
	ASSERT( g_pconductor->m_pDMPerformance != NULL );
	try {
	while (1)
	{
		DWORD dwRes;
		dwRes = WaitForSingleObject( g_pconductor->m_hNotifyEvent, INFINITE );
		if ( !g_pconductor || !g_pconductor->m_hNotifyEvent ||
			 !g_pconductor->m_pDMPerformance || g_pconductor->m_fShutdown )
		{
			//TRACE("NotifyThreadProc: pointer NULL!.\n");
			break;
		}
		if ( (dwRes == WAIT_OBJECT_0) || (dwRes == WAIT_TIMEOUT) )
		{
			DMUS_NOTIFICATION_PMSG *pNotificationMsg = NULL;
			HRESULT hr;
			do
			{
				// Assuming that IDirectMusicPerformance::GetNotificationEvent is thread safe.
				hr = g_pconductor->m_pDMPerformance->GetNotificationPMsg( &pNotificationMsg );
				if ( hr == S_OK )
				{
					ASSERT( pNotificationMsg != NULL );
					if ( pNotificationMsg->dwType == DMUS_PMSGT_NOTIFICATION)
					{
						// Check if this is a Segment notification, we're not in a transition, and 
						// the music stopped.
						if ( g_pconductor && g_pconductor->m_pToolbarHandler &&
							InlineIsEqualGUID( pNotificationMsg->guidNotificationType, GUID_NOTIFICATION_SEGMENT ) )
						{
							/*
							MUSIC_TIME mtNow;
							REFERENCE_TIME rtNow;
							g_pconductor->m_pDMPerformance->GetTime( &rtNow, &mtNow );
							char szDebug[512];
							sprintf( szDebug, "Segment Notification %d for %x received at %ld | %I64d for time %ld | %I64d\n",
								pNotificationMsg->dwNotificationOption, pNotificationMsg->punkUser,
								mtNow, rtNow, pNotificationMsg->mtTime, pNotificationMsg->rtTime );
							OutputDebugString(szDebug);
							*/
							if( (pNotificationMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGEND) ||
								(pNotificationMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGABORT) )
							{
								IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
								if( pNotificationMsg->punkUser
								&&	SUCCEEDED( pNotificationMsg->punkUser->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&pIDirectMusicSegmentState8 ) ) )
								{
									::EnterCriticalSection( &g_pconductor->m_csAudiopathList );
									POSITION pos = g_pconductor->m_lstSegStateAudioPaths.GetHeadPosition();
									while( pos )
									{
										const POSITION posToDelete = pos;
										SegStateAudioPath *pSegStateAudioPath = g_pconductor->m_lstSegStateAudioPaths.GetNext( pos );
										if( pSegStateAudioPath->pSegmentState == pIDirectMusicSegmentState8 )
										{
											delete pSegStateAudioPath;
											pSegStateAudioPath = NULL;
											g_pconductor->m_lstSegStateAudioPaths.RemoveAt( posToDelete );
										}
									}
									::LeaveCriticalSection( &g_pconductor->m_csAudiopathList );
									pIDirectMusicSegmentState8->Release();
								}

								POSITION pos = g_pconductor->m_lstSecondaryToolbars.GetHeadPosition();
								while( pos )
								{
									CSecondaryToolbarHandler *pToolbarHandler = g_pconductor->m_lstSecondaryToolbars.GetNext( pos );
									pToolbarHandler->OnSegEndNotify( pNotificationMsg->punkUser );
									::PostMessage( pToolbarHandler->m_hWndToolbar, WM_COMMAND, (WPARAM) MAKELONG( 0, ID_SET_STATE_AUTO ), (LPARAM) g_pconductor->m_pToolbarHandler->m_hWndToolbar );
								}
								::PostMessage( g_pconductor->m_pToolbarHandler->m_hWndToolbar, WM_COMMAND, (WPARAM) MAKELONG( 0, ID_SET_STATE_AUTO ), (LPARAM) g_pconductor->m_pToolbarHandler->m_hWndToolbar );

								// If it exists, redraw the status toolbar
								if( g_pconductor->m_pStatusToolbarHandler )
								{
									g_pconductor->m_pStatusToolbarHandler->RefreshAllButtons();
								}

								if( g_pconductor->m_pWaveRecordToolbar )
								{
									g_pconductor->m_pWaveRecordToolbar->PostMessage( WM_USER + 3, NULL, NULL );
								}
							}
							else if( pNotificationMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGSTART )
							{
								POSITION pos = g_pconductor->m_lstSecondaryToolbars.GetHeadPosition();
								while( pos )
								{
									CSecondaryToolbarHandler *pToolbarHandler = g_pconductor->m_lstSecondaryToolbars.GetNext( pos );
									pToolbarHandler->OnSegStartNotify( pNotificationMsg->punkUser );
									::PostMessage( pToolbarHandler->m_hWndToolbar, WM_COMMAND, (WPARAM) MAKELONG( 0, ID_SET_STATE_AUTO ), (LPARAM) g_pconductor->m_pToolbarHandler->m_hWndToolbar );
								}
								::PostMessage( g_pconductor->m_pToolbarHandler->m_hWndToolbar, WM_COMMAND, (WPARAM) MAKELONG( 0, ID_SET_STATE_AUTO ), (LPARAM) g_pconductor->m_pToolbarHandler->m_hWndToolbar );
							}
							else
							{
								::PostMessage( g_pconductor->m_pToolbarHandler->m_hWndToolbar, WM_COMMAND, (WPARAM) MAKELONG( 0, ID_SET_STATE_AUTO ), (LPARAM) g_pconductor->m_pToolbarHandler->m_hWndToolbar );
							}
						}
						// Removed code that automatically resets Play/Stop when we receive a
						// MusicStopped notification.  It was too unreliable.  Transports are now
						// responsible for calling IDMUSProdConductor::TransportStopped(..) when they stop
						// playing by a means other than hitting the Stop or Transition buttons.

						// do the multicast
						::EnterCriticalSection( &g_pconductor->m_csNotifyEntry );
						fInCritSeq = TRUE;

						CNotifyEntry* pNotifyEntry = NULL;
						BOOL fFound = FALSE;
						POSITION pos;
						pos = g_pconductor->m_lstNotifyEntry.GetHeadPosition();
						while ( pos != NULL && !fFound )
						{
							pNotifyEntry = g_pconductor->m_lstNotifyEntry.GetNext( pos );
							if ( InlineIsEqualGUID( pNotifyEntry->m_guid, pNotificationMsg->guidNotificationType ) )
							{
								fFound = TRUE;
								ConductorNotifyEvent cnEvent;
								cnEvent.m_cbSize = sizeof( pNotificationMsg );
								cnEvent.m_dwType = pNotificationMsg->dwType;
								cnEvent.m_pbData = (BYTE *)pNotificationMsg;

								POSITION pos2;
								IDMUSProdNotifyCPt *pINotifyCPt;
								pos2 = pNotifyEntry->m_lstNotifyCPt.GetHeadPosition();
								while ( pos2 != NULL )
								{
									pINotifyCPt = pNotifyEntry->m_lstNotifyCPt.GetAt( pos2 );
									if ( pINotifyCPt != NULL )
									/*
									if ( !IsBadReadPtr(pINotifyCPt, sizeof(_V_GENERIC_INTERFACE))
									&&	 !IsBadReadPtr(*reinterpret_cast<_V_GENERIC_INTERFACE*>(pINotifyCPt)->__vptr, sizeof(FARPROC))
									&&	 !IsBadCodePtr(*(reinterpret_cast<_V_GENERIC_INTERFACE*>(pINotifyCPt)->__vptr)[0]) )
									*/
									{
										pINotifyCPt->OnNotify( &cnEvent );
									}
									else
									{
										ASSERT( FALSE );
									}
									pNotifyEntry->m_lstNotifyCPt.GetNext( pos2 );
								}
							}
						}

						::LeaveCriticalSection( &g_pconductor->m_csNotifyEntry );
						fInCritSeq = FALSE;
					}
					g_pconductor->m_pDMPerformance->FreePMsg( (DMUS_PMSG *)pNotificationMsg );
					pNotificationMsg = NULL;
				}
			} while ( hr == S_OK );
			if ( FAILED(hr) )
			{
				TRACE("NotifyThreadProc: GetNotifyEvent failed with %x\n", hr);
				break;
			}
		}
		else
		{
			TRACE("NotifyThreadProc: break out early.\n");
			break;
		}
		// Go back and WaitForSingleObject again.
	}
	}
	catch( ... )
	{
		TRACE("Caught exception in NotifyThreadProc. Exiting.\n");
	}

	//TRACE("NotifyThreadProc setting event.\n");

	if( fInCritSeq )
	{
		::LeaveCriticalSection( &g_pconductor->m_csNotifyEntry );
	}

	if( g_pconductor && g_pconductor->m_hNotifyExitEvent )
	{
		::SetEvent( g_pconductor->m_hNotifyExitEvent );
	}

	//TRACE("NotifyThreadProc exiting.\n");
	return 0;
}
