// Implementation of COutputTool and CMIDIInputContainer

#include "stdafx.h"
#include "OutputTool.h"
#include "cconduct.h"
#include "StatusToolbar.h"
#include "Toolbar.h"

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0

#define LATENCY_CUTOFF_MS 60

extern UINT AFX_CDECL EchoMIDIInThreadProc( LPVOID pParam );
IDirectMusicPort *g_rpLastSampledPort;
static REFERENCE_TIME s_rtLastSampledLatency;

// Defined in notify.cpp
extern void AddToolsAndSetupWaveSaveForSegState( IUnknown *punk );

// Constructor/Desctructor
COutputTool::COutputTool()
{
	m_cRef = 0;
	AddRef();
	
	// Initially allocate for 32 channels
	m_afActivePChannels.SetSize( 32 );
	m_afMute.SetSize( 32 );
	m_alPChannelStates.SetSize( 32 );
	m_alPChannelNoteOnStates.SetSize( 32 );
	m_abPChannelNoteOn.SetSize( 32 );
//	ofsOutput.open("OutputFile.txt");
}

COutputTool::~COutputTool()
{
//	ofsOutput.close();
}

// IUnknown
HRESULT STDMETHODCALLTYPE COutputTool::QueryInterface( const IID &iid, void **ppv )
{
    if( ::IsEqualIID( iid, IID_IDirectMusicTool )  ||
		::IsEqualIID( iid, IID_IUnknown ) )
	{
		*ppv = static_cast<IDirectMusicTool *>(this);
	}
	else  {
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IUnknown *>(*ppv)->AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE COutputTool::AddRef()
{
	return InterlockedIncrement( &m_cRef );
}

ULONG STDMETHODCALLTYPE COutputTool::Release()
{
	if( InterlockedDecrement( &m_cRef ) == 0 )
	{
		delete this;
		return 0;
	}
	return m_cRef;
}

// IDirectMusicTool methods
HRESULT STDMETHODCALLTYPE COutputTool::Init(
	IDirectMusicGraph* pGraph) 
{
	UNREFERENCED_PARAMETER(pGraph);
	// This tool has no need to do any type of initialization.
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE COutputTool::GetMsgDeliveryType(
	DWORD* pdwDeliveryType )
{
	// This tool wants messages just before their time stamp occurs.
	*pdwDeliveryType = DMUS_PMSGF_TOOL_QUEUE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE COutputTool::GetMediaTypeArraySize(
	DWORD* pdwNumElements )
{
	// This tool only wants note messages, MIDI messages, user messages, 
	// wave messages, and Notification messages; for manbugs 45192, we'll also
    // have the tool take curves. So, set *pdwNumElements to 6.
	*pdwNumElements = 6;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE COutputTool::GetMediaTypes(
	DWORD** padwMediaTypes, 
	DWORD dwNumElements)
{
	// Fill in the array padwMediaTypes with the type of
	// messages this tool wants to process. In this case,
	// dwNumElements will be 6, since that is what this
	// tool returns from GetMediaTypeArraySize().

	if( dwNumElements == 6 )
	{
		(*padwMediaTypes)[0] = DMUS_PMSGT_NOTE;
		(*padwMediaTypes)[1] = DMUS_PMSGT_MIDI;
		(*padwMediaTypes)[2] = DMUS_PMSGT_USER;
		(*padwMediaTypes)[3] = DMUS_PMSGT_NOTIFICATION;
		(*padwMediaTypes)[4] = DMUS_PMSGT_WAVE;
        (*padwMediaTypes)[5] = DMUS_PMSGT_CURVE;
		return S_OK;
	}
	else
	{
		// this should never happen
		return E_FAIL;
	}
}

HRESULT STDMETHODCALLTYPE COutputTool::ProcessPMsg(
	IDirectMusicPerformance* pPerf, 
	DMUS_PMSG* pPMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// The Tool is set up to only receive messages of types
	// DMUS_PMSGT_NOTE, DMUS_PMSGT_MIDI, DMUS_PMSGT_WAVE, DMUS_PMSGT_NOTIFICATION, and DMUS_PMSGT_USER

	// Check if this is a "user" message
	if( pPMsg->dwType == DMUS_PMSGT_USER )
	{
		// Ensure this is our user message
		if( pPMsg->punkUser == this )
		{
			DMUS_MIDI_PMSG* pMidi = (DMUS_MIDI_PMSG*) pPMsg;
			if( pMidi->bStatus == MIDI_NOTEON )
			{
				//ofsOutput<<"On,  "<<pPMsg->dwPChannel<<","<<pPMsg->mtTime<<"\n";
				m_alPChannelStates[pPMsg->dwPChannel] += pMidi->bByte2;
				m_alPChannelNoteOnStates[pPMsg->dwPChannel] += pMidi->bByte2;
				m_abPChannelNoteOn[pPMsg->dwPChannel] = 1;
			}
			else if( pMidi->bStatus == MIDI_NOTEOFF )
			{
				//ofsOutput<<"Off, "<<pPMsg->dwPChannel<<","<<pPMsg->mtTime<<"\n";
				m_alPChannelStates[pPMsg->dwPChannel] -= pMidi->bByte2;
			}
			else
			{
				ASSERT(0);
			}
			return DMUS_S_FREE;
		}
		else
		{
			// returning S_FREE frees the message. If StampPMsg()
			// fails, there is no destination for this message so
			// free it.
			if(( NULL == pPMsg->pGraph ) ||
				FAILED(pPMsg->pGraph->StampPMsg(pPMsg)))
			{
				TRACE("OutputTool freeing user message - StampPMsg failed.\n");
				return DMUS_S_FREE;
			}
			return DMUS_S_REQUEUE;
		}
	}

	// Need to save the original PChannel, since StampPMsg will change it
	const DWORD dwOrigPChannel = pPMsg->dwPChannel;

	// returning S_FREE frees the message. If StampPMsg()
	// fails, there is no destination for this message so
	// free it.
	if( (NULL == pPMsg->pGraph)
	||	FAILED(pPMsg->pGraph->StampPMsg(pPMsg)))
	{
		//TRACE("OutputTool freeing message - StampPMSG failed. Type %d.\n", pPMsg->dwType);
		return DMUS_S_FREE;
	}

	// Ignore broadcast messages
	if( dwOrigPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS )
	{
		// Send the message on to the next tool
		return DMUS_S_REQUEUE;
	}

	// Handle metronome events
	if( pPMsg->dwType == DMUS_PMSGT_NOTIFICATION )
	{
		DMUS_NOTIFICATION_PMSG* pNotification = (DMUS_NOTIFICATION_PMSG*) pPMsg;
		if(  g_pconductor->m_fMetronomeEnabled &&
			( pNotification->guidNotificationType == GUID_NOTIFICATION_MEASUREANDBEAT) && 
			( pNotification->dwNotificationOption == DMUS_NOTIFICATION_MEASUREBEAT ) &&
			( pPMsg->mtTime >= g_pconductor->m_pToolbarHandler->m_mtPlayTime ) )
		{
			//TRACE("Received MeasureBeat notification %d,%d at %d. punkUser=%x.\n", pNotification->dwField1, pNotification->dwField2, pPMsg->mtTime, pPMsg->punkUser );

			IDirectMusicSegmentState *pSegmentState = NULL;
			if( FAILED( g_pconductor->m_pDMPerformance->GetSegmentState( &pSegmentState, pPMsg->mtTime ) ) )
			{
				// Send the message on to the next tool
				return DMUS_S_REQUEUE;
			}
			
			RELEASE( pSegmentState );

			DMUS_MIDI_PMSG* pOnMidiMsg, *pOffMidiMsg;
			if( SUCCEEDED( pPerf->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pOnMidiMsg )) &&
				SUCCEEDED( pPerf->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pOffMidiMsg )) )
			{
				memset( pOnMidiMsg, 0, sizeof(DMUS_MIDI_PMSG) );
				memset( pOffMidiMsg, 0, sizeof(DMUS_MIDI_PMSG) );
				// MIDI_NOTEOFF = 0x80, MIDI_NOTEON = 0x90
				pOnMidiMsg->dwSize = sizeof(DMUS_MIDI_PMSG);
				pOnMidiMsg->bStatus = 0x90;
				pOnMidiMsg->mtTime = pNotification->mtTime;
				pOnMidiMsg->dwFlags = DMUS_PMSGF_MUSICTIME;
				pOnMidiMsg->dwPChannel = g_pconductor->m_dwMetronomePChannel;
				pOnMidiMsg->dwType = DMUS_PMSGT_MIDI;

				pOffMidiMsg->dwSize = sizeof(DMUS_MIDI_PMSG);
				pOffMidiMsg->bStatus = 0x80;
				pOffMidiMsg->mtTime = pNotification->mtTime + (DMUS_PPQ / 4);
				pOffMidiMsg->dwFlags = DMUS_PMSGF_MUSICTIME;
				pOffMidiMsg->dwPChannel = g_pconductor->m_dwMetronomePChannel;
				pOffMidiMsg->dwType = DMUS_PMSGT_MIDI;

				// Determine which PChannel the metronome should play on
				IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
				if( pNotification->punkUser
				&&	SUCCEEDED( pNotification->punkUser->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&pIDirectMusicSegmentState8 ) ) )
				{
					IDirectMusicAudioPath *pIDirectMusicAudioPath;
					if( SUCCEEDED( pIDirectMusicSegmentState8->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0, GUID_All_Objects, 0, IID_IDirectMusicAudioPath, (void **)&pIDirectMusicAudioPath ) ) )
					{
						pIDirectMusicAudioPath->ConvertPChannel( pOnMidiMsg->dwPChannel, &pOnMidiMsg->dwPChannel );
						pIDirectMusicAudioPath->ConvertPChannel( pOffMidiMsg->dwPChannel, &pOffMidiMsg->dwPChannel );
						pIDirectMusicAudioPath->Release();
					}
					pIDirectMusicSegmentState8->Release();
				}

				if( pNotification->dwField1 == 0 )
				{
					pOnMidiMsg->bByte1 = g_pconductor->m_bMetronomeNoteOne;
					pOnMidiMsg->bByte2 = g_pconductor->m_bMetronomeVelocityOne;
					pOffMidiMsg->bByte1 = g_pconductor->m_bMetronomeNoteOne;
					pOffMidiMsg->bByte2 = g_pconductor->m_bMetronomeVelocityOne;
				}
				else
				{
					pOnMidiMsg->bByte1 = g_pconductor->m_bMetronomeNoteOther;
					pOnMidiMsg->bByte2 = g_pconductor->m_bMetronomeVelocityOther;
					pOffMidiMsg->bByte1 = g_pconductor->m_bMetronomeNoteOther;
					pOffMidiMsg->bByte2 = g_pconductor->m_bMetronomeVelocityOther;
				}

				pPerf->SendPMsg( (DMUS_PMSG*)pOnMidiMsg );
				pPerf->SendPMsg( (DMUS_PMSG*)pOffMidiMsg );
			}
		}

		// Send the message on to the next tool
		return DMUS_S_REQUEUE;
	}

	// Add the buttons, if necessary
	if( ((pPMsg->dwType == DMUS_PMSGT_MIDI) && ((((DMUS_MIDI_PMSG*)pPMsg)->bStatus & 0xF0) == MIDI_NOTEON))
	||	((pPMsg->dwType == DMUS_PMSGT_NOTE) && (((DMUS_NOTE_PMSG*)pPMsg)->bFlags & DMUS_NOTEF_NOTEON))
	||	((pPMsg->dwType == DMUS_PMSGT_WAVE) && !(((DMUS_NOTE_PMSG*)pPMsg)->bFlags & DMUS_WAVEF_OFF)) )
	{
		if( (dwOrigPChannel >= (unsigned)m_afMute.GetSize())
		||	!m_afActivePChannels[ dwOrigPChannel ] )
		{
			AddNew( dwOrigPChannel );
		}
	}

	// If this channel should be muted
	if( (dwOrigPChannel < (unsigned)m_afMute.GetSize())
	&&	m_afMute[ dwOrigPChannel ] )
	{
		//Fix 19688: Eat Note on events, but let all other events flow through.
		if( pPMsg->dwType == DMUS_PMSGT_MIDI )
		{
			DMUS_MIDI_PMSG* pMidi = (DMUS_MIDI_PMSG*) pPMsg;
			if( (pMidi->bStatus & 0xF0) == MIDI_NOTEON )
			{
				return DMUS_S_FREE;
			}
		}
		else if( pPMsg->dwType == DMUS_PMSGT_NOTE )
		{
			DMUS_NOTE_PMSG* pNote = (DMUS_NOTE_PMSG*) pPMsg;
			if( pNote->bFlags & DMUS_NOTEF_NOTEON )
			{
				return DMUS_S_FREE;
			}
		}
		else if( pPMsg->dwType == DMUS_PMSGT_WAVE )
		{
			DMUS_WAVE_PMSG* pWave = (DMUS_WAVE_PMSG*) pPMsg;
			if( !(pWave->bFlags & DMUS_WAVEF_OFF) )
			{
				return DMUS_S_FREE;
			}
		}
	}

	if( pPMsg->dwType == DMUS_PMSGT_MIDI )
	{
		DMUS_MIDI_PMSG* pMidi = (DMUS_MIDI_PMSG*) pPMsg;
		if( (pMidi->bStatus & 0xF0) == MIDI_NOTEON )
		{
			ASSERT( dwOrigPChannel < (unsigned)m_afMute.GetSize() );
			ASSERT( m_afActivePChannels[ dwOrigPChannel ] );

			m_alPChannelStates[dwOrigPChannel] += pMidi->bByte2;
			m_alPChannelNoteOnStates[dwOrigPChannel] += pMidi->bByte2;
			m_abPChannelNoteOn[dwOrigPChannel] = 1;
		}
		else if( ((pMidi->bStatus & 0xF0) == MIDI_NOTEOFF)
			 &&  (dwOrigPChannel < (unsigned)m_alPChannelStates.GetSize()) )
		{
			m_alPChannelStates[dwOrigPChannel] -= pMidi->bByte2;
		}
		else
		{
			// Let all other MIDI messages flow through
			//TRACE("MIDI - \t - \t - %2.2x %2.2x %2.2x\n", dwOrigPChannel, pMidi->bStatus, pMidi->bByte1);
		}
	}

	if( pPMsg->dwType == DMUS_PMSGT_WAVE )
	{
		DMUS_WAVE_PMSG* pWave = (DMUS_WAVE_PMSG*) pPMsg;
		if( !(pWave->bFlags & DMUS_WAVEF_OFF) )
		{
			ASSERT( dwOrigPChannel < (unsigned)m_afMute.GetSize() );
			ASSERT( m_afActivePChannels[ dwOrigPChannel ] );

			DMUS_MIDI_PMSG* pOnMidiMsg, *pOffMidiMsg;
			if( SUCCEEDED( pPerf->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
				(DMUS_PMSG**)&pOnMidiMsg )) &&
				SUCCEEDED( pPerf->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
				(DMUS_PMSG**)&pOffMidiMsg )))
			{
				// copy the original note into this message
				memcpy( pOnMidiMsg, pPMsg, sizeof(DMUS_PMSG) );
				memcpy( pOffMidiMsg, pPMsg, sizeof(DMUS_PMSG) );
				pOnMidiMsg->dwPChannel = dwOrigPChannel;
				pOffMidiMsg->dwPChannel = dwOrigPChannel;
				pOnMidiMsg->pTool	 = (IDirectMusicTool*)this;
				pOffMidiMsg->pTool	 = (IDirectMusicTool*)this;
				pOnMidiMsg->pTool->AddRef();
				pOffMidiMsg->pTool->AddRef();
				if( pOnMidiMsg->pGraph ) pOnMidiMsg->pGraph->AddRef();
				if( pOffMidiMsg->pGraph ) pOffMidiMsg->pGraph->AddRef();
				pOnMidiMsg->punkUser = NULL;
				pOffMidiMsg->punkUser= NULL;
				pOnMidiMsg->dwSize	 = sizeof(DMUS_MIDI_PMSG);
				pOffMidiMsg->dwSize  = sizeof(DMUS_MIDI_PMSG);
				pOnMidiMsg->dwType	 = DMUS_PMSGT_USER;
				pOffMidiMsg->dwType  = DMUS_PMSGT_USER;
				pOnMidiMsg->bStatus	 = MIDI_NOTEON;
				pOffMidiMsg->bStatus = MIDI_NOTEOFF;
				pOnMidiMsg->bByte2	 = 100;
				pOffMidiMsg->bByte2  = 100;
				pOnMidiMsg->dwFlags	 = DMUS_PMSGF_TOOL_ATTIME | DMUS_PMSGF_REFTIME | DMUS_PMSGF_MUSICTIME;
				if( pWave->dwFlags & DMUS_PMSGF_LOCKTOREFTIME )
				{
					pOffMidiMsg->rtTime += pWave->rtDuration;
					pOffMidiMsg->dwFlags = DMUS_PMSGF_TOOL_ATTIME | DMUS_PMSGF_REFTIME;
				}
				else
				{
					pOffMidiMsg->mtTime += long(min( LONG_MAX, pWave->rtDuration ));
					pPerf->MusicToReferenceTime( pOffMidiMsg->mtTime, &pOffMidiMsg->rtTime );
					pOffMidiMsg->dwFlags = DMUS_PMSGF_TOOL_ATTIME | DMUS_PMSGF_REFTIME | DMUS_PMSGF_MUSICTIME;
				}

				pOnMidiMsg->punkUser = this;
				AddRef();
				pOffMidiMsg->punkUser = this;
				AddRef();

				pPerf->SendPMsg( (DMUS_PMSG*)pOnMidiMsg );
				pPerf->SendPMsg( (DMUS_PMSG*)pOffMidiMsg );
			}
		}
	}

	if( pPMsg->dwType == DMUS_PMSGT_NOTE )
	{
		DMUS_NOTE_PMSG* pNote = (DMUS_NOTE_PMSG*) pPMsg;
		if( pNote->bFlags & DMUS_NOTEF_NOTEON )
		{
			ASSERT( dwOrigPChannel < (unsigned)m_afMute.GetSize() );
			ASSERT( m_afActivePChannels[ dwOrigPChannel ] );

			DMUS_MIDI_PMSG* pOnMidiMsg, *pOffMidiMsg;
			if( SUCCEEDED( pPerf->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
				(DMUS_PMSG**)&pOnMidiMsg )) &&
				SUCCEEDED( pPerf->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
				(DMUS_PMSG**)&pOffMidiMsg )))
			{
				// copy the original note into this message
				memcpy( pOnMidiMsg, pPMsg, sizeof(DMUS_PMSG) );
				memcpy( pOffMidiMsg, pPMsg, sizeof(DMUS_PMSG) );
				pOnMidiMsg->dwPChannel = dwOrigPChannel;
				pOffMidiMsg->dwPChannel = dwOrigPChannel;
				pOnMidiMsg->pTool	 = (IDirectMusicTool*)this;
				pOffMidiMsg->pTool	 = (IDirectMusicTool*)this;
				pOnMidiMsg->pTool->AddRef();
				pOffMidiMsg->pTool->AddRef();
				if( pOnMidiMsg->pGraph ) pOnMidiMsg->pGraph->AddRef();
				if( pOffMidiMsg->pGraph ) pOffMidiMsg->pGraph->AddRef();
				pOnMidiMsg->punkUser = NULL;
				pOffMidiMsg->punkUser= NULL;
				pOnMidiMsg->dwSize	 = sizeof(DMUS_MIDI_PMSG);
				pOffMidiMsg->dwSize  = sizeof(DMUS_MIDI_PMSG);
				pOnMidiMsg->dwType	 = DMUS_PMSGT_USER;
				pOffMidiMsg->dwType  = DMUS_PMSGT_USER;
				pOnMidiMsg->bStatus	 = MIDI_NOTEON;
				pOffMidiMsg->bStatus = MIDI_NOTEOFF;
				pOnMidiMsg->bByte2	 = pNote->bVelocity;
				pOffMidiMsg->bByte2  = pNote->bVelocity;
				pOnMidiMsg->dwFlags	 = DMUS_PMSGF_TOOL_ATTIME | DMUS_PMSGF_MUSICTIME | DMUS_PMSGF_REFTIME;
				pOffMidiMsg->mtTime += pNote->mtDuration;
				pPerf->MusicToReferenceTime( pOffMidiMsg->mtTime, &pOffMidiMsg->rtTime );
				pOffMidiMsg->dwFlags = DMUS_PMSGF_TOOL_ATTIME | DMUS_PMSGF_MUSICTIME | DMUS_PMSGF_REFTIME;

				pOnMidiMsg->punkUser = this;
				AddRef();
				pOffMidiMsg->punkUser = this;
				AddRef();

				pPerf->SendPMsg( (DMUS_PMSG*)pOnMidiMsg );
				pPerf->SendPMsg( (DMUS_PMSG*)pOffMidiMsg );
			}
		}
	}

	// return DMUS_S_REQUEUE so the original message is requeued
	return DMUS_S_REQUEUE;
}

HRESULT STDMETHODCALLTYPE COutputTool::Flush(
	IDirectMusicPerformance* pPerf, 
	DMUS_PMSG* pPMsg,
	REFERENCE_TIME rt)
{
	UNREFERENCED_PARAMETER(rt);
	UNREFERENCED_PARAMETER(pPerf);

	// Check if this is a "user" message
	if( pPMsg->dwType == DMUS_PMSGT_USER )
	{
		DMUS_MIDI_PMSG* pMidi = (DMUS_MIDI_PMSG*) pPMsg;
		if( pMidi->bStatus == MIDI_NOTEON )
		{
			//ofsOutput<<"FOn, "<<pPMsg->dwPChannel<<","<<pPMsg->mtTime<<"\n";
			m_alPChannelStates[pPMsg->dwPChannel] += pMidi->bByte2;
			m_alPChannelNoteOnStates[pPMsg->dwPChannel] += pMidi->bByte2;
			m_abPChannelNoteOn[pPMsg->dwPChannel] = 1;
		}
		else if( pMidi->bStatus == MIDI_NOTEOFF )
		{
			//ofsOutput<<"FOff, "<<pPMsg->dwPChannel<<","<<pPMsg->mtTime<<"\n";
			m_alPChannelStates[pPMsg->dwPChannel] -= pMidi->bByte2;
		}
		else
		{
			ASSERT(0);
		}
		return DMUS_S_FREE;
	}

	// returning S_FREE frees the message. If StampPMsg()
	// fails, there is no destination for this message so
	// free it.
	if(( NULL == pPMsg->pGraph ) ||
		FAILED(pPMsg->pGraph->StampPMsg(pPMsg)))
	{
		return DMUS_S_FREE;
	}
	return DMUS_S_REQUEUE;
}

void COutputTool::SetPChannelMute( DWORD dwPChannel, BOOL fMute )
{
	ASSERT( !m_afMute.GetSize() ? FALSE : dwPChannel < (unsigned)m_afMute.GetSize() );
	if( m_afMute.GetSize()
	&& (dwPChannel < (unsigned)m_afMute.GetSize()) )
	{
		m_afMute[ dwPChannel ] = fMute;
	}
}

BOOL COutputTool::GetPChannelMute( DWORD dwPChannel )
{
	ASSERT( !m_afMute.GetSize() ? FALSE : dwPChannel < (unsigned)m_afMute.GetSize() );
	if( m_afMute.GetSize()
	&& (dwPChannel < (unsigned)m_afMute.GetSize()) )
	{
		return m_afMute[ dwPChannel ];
	}
	return FALSE;
}

void COutputTool::AddNew( DWORD dwPChannel )
{
	const DWORD dwOldSize = (unsigned) m_afActivePChannels.GetSize();

	// Check if we're already active
	if( dwPChannel < dwOldSize
	&&	m_afActivePChannels[dwPChannel] == TRUE )
	{
		return;
	}

	BOOL fMute = TRUE;
	int nNumUnMuted = 0;
	int nNumActive = 0;
	for( DWORD dwIndex = 0; dwIndex < dwOldSize; dwIndex ++ )
	{
		if( m_afActivePChannels[dwIndex] )
		{
			nNumActive++;
			if( !m_afMute[dwIndex] )
			{
				nNumUnMuted++;
				if( nNumUnMuted > 1 )
				{
					fMute = FALSE;
					break;
				}
			}
		}
	}

	if( fMute && (nNumActive <= 1) && (nNumUnMuted <= 1) )
	{
		// Only one PChannel active, and it's unmuted
		fMute = FALSE;
	}

	if( dwPChannel >= dwOldSize )
	{
		dwPChannel++;
		m_afMute.SetSize( dwPChannel );
		m_afActivePChannels.SetSize( dwPChannel );
		m_alPChannelStates.SetSize( dwPChannel );
		m_alPChannelNoteOnStates.SetSize( dwPChannel );
		m_abPChannelNoteOn.SetSize( dwPChannel );
		/* Not necessary - SetSize calls ConstructElements, which sets all bits
		   of the new values to 0.
		for( dwIndex = dwOldSize; dwIndex < dwPChannel; dwIndex++ )
		{
			m_afMute[dwIndex] = FALSE;
			m_afActivePChannels[dwIndex] = FALSE;
			m_alPChannelStates[dwIndex] = 0;
			m_alPChannelNoteOnStates[dwIndex] = 0;
			m_abPChannelNoteOn[dwIndex] = 0;
		}
		*/
		dwPChannel--;
	}

	m_afActivePChannels[dwPChannel] = TRUE;
	m_afMute[dwPChannel] = fMute;

	if( g_pconductor->m_pStatusToolbarHandler
	&&	g_pconductor->m_pStatusToolbarHandler->m_hWndToolbar )
	{
		::PostMessage( g_pconductor->m_pStatusToolbarHandler->m_hWndToolbar,
					   WM_COMMAND, MAKELONG( dwPChannel, ID_ADD_BUTTON ),
					   (LPARAM) g_pconductor->m_pStatusToolbarHandler->m_hWndToolbar );
	}
}

void COutputTool::MarkAllChannelsUnused( void )
{
	for( int i=0; i < m_afActivePChannels.GetSize(); i++ )
	{
		m_afMute[i] = FALSE;
		m_afActivePChannels[i] = FALSE;
	}
}

/*
class CAbortTracker : public IUnknown
{
public:
	CAbortTracker()
	{
		m_cRef = 0;
		AddRef();
	}
	~CAbortTracker()
	{
	}

	// IUnknown methods
	virtual STDMETHODIMP QueryInterface( const IID &iid, void **ppv )
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	virtual STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement( &m_cRef );
	}
	virtual STDMETHODIMP_(ULONG) Release()
	{
		AfxDumpStack(AFX_STACK_DUMP_TARGET_TRACE);
		if( InterlockedDecrement( &m_cRef ) == 0 )
		{
			delete this;
			return 0;
		}
		return m_cRef;
	}

protected:
	LONG	m_cRef;
};
*/

// Constructor/Desctructor
CNotifyTool::CNotifyTool()
{
	m_cRef = 0;
	AddRef();
}

CNotifyTool::~CNotifyTool()
{
}

// IUnknown
HRESULT STDMETHODCALLTYPE CNotifyTool::QueryInterface( const IID &iid, void **ppv )
{
    if( ::IsEqualIID( iid, IID_IDirectMusicTool )  ||
		::IsEqualIID( iid, IID_IUnknown ) )
	{
		*ppv = static_cast<IDirectMusicTool *>(this);
	}
	else  {
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IUnknown *>(*ppv)->AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CNotifyTool::AddRef()
{
	return InterlockedIncrement( &m_cRef );
}

ULONG STDMETHODCALLTYPE CNotifyTool::Release()
{
	if( InterlockedDecrement( &m_cRef ) == 0 )
	{
		delete this;
		return 0;
	}
	return m_cRef;
}

// IDirectMusicTool methods
HRESULT STDMETHODCALLTYPE CNotifyTool::Init(
	IDirectMusicGraph* pGraph) 
{
	UNREFERENCED_PARAMETER(pGraph);
	// This tool has no need to do any type of initialization.
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CNotifyTool::GetMsgDeliveryType(
	DWORD* pdwDeliveryType )
{
	// This tool wants messages as soon as possible
	*pdwDeliveryType = DMUS_PMSGF_TOOL_IMMEDIATE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CNotifyTool::GetMediaTypeArraySize(
	DWORD* pdwNumElements )
{
	// This tool only wants Notification messages, so set *pdwNumElements to 1.
	*pdwNumElements = 1;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CNotifyTool::GetMediaTypes(
	DWORD** padwMediaTypes, 
	DWORD dwNumElements)
{
	// Fill in the array padwMediaTypes with the type of
	// messages this tool wants to process. In this case,
	// dwNumElements will be 5, since that is what this
	// tool returns from GetMediaTypeArraySize().

	if( dwNumElements == 1 )
	{
		(*padwMediaTypes)[0] = DMUS_PMSGT_NOTIFICATION;
		return S_OK;
	}
	else
	{
		// this should never happen
		return E_FAIL;
	}
}

HRESULT STDMETHODCALLTYPE CNotifyTool::ProcessPMsg(
	IDirectMusicPerformance* pPerf, 
	DMUS_PMSG* pPMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(pPerf);

	// The Tool is set up to only receive messages of type
	// DMUS_PMSGT_NOTIFICATION

	// returning S_FREE frees the message. If StampPMsg()
	// fails, there is no destination for this message so
	// free it.
	if( (NULL == pPMsg->pGraph)
	||	FAILED(pPMsg->pGraph->StampPMsg(pPMsg)))
	{
		//TRACE("OutputTool freeing message - StampPMSG failed. Type %d.\n", pPMsg->dwType);
		return DMUS_S_FREE;
	}

	// Handle metronome events
	if( pPMsg->dwType == DMUS_PMSGT_NOTIFICATION )
	{
		DMUS_NOTIFICATION_PMSG* pNotification = (DMUS_NOTIFICATION_PMSG*) pPMsg;
		if( pNotification->guidNotificationType == GUID_NOTIFICATION_SEGMENT )
		{
			/*
			MUSIC_TIME mtNow;
			REFERENCE_TIME rtNow;
			pPerf->GetTime( &rtNow, &mtNow );
			TRACE("Segment Notification message %d for %x received at %ld | %I64d for time %ld | %I64d\n",
				pNotification->dwNotificationOption, pNotification->punkUser,
				mtNow, rtNow, pNotification->mtTime, pNotification->rtTime );
			*/
			if( pNotification->dwNotificationOption == DMUS_NOTIFICATION_SEGSTART )
			{
				AddToolsAndSetupWaveSaveForSegState( pNotification->punkUser );
			}
			/*
			else if( pNotification->dwNotificationOption == DMUS_NOTIFICATION_SEGABORT )
			{
				CAbortTracker *pAbortTracker = new CAbortTracker;
				if( pNotification->punkUser )
				{
					pNotification->punkUser->Release();
				}
				pNotification->punkUser = pAbortTracker;
			}
			*/
		}

		// Send the message on to the next tool
		return DMUS_S_REQUEUE;
	}

	// return DMUS_S_REQUEUE so the original message is requeued
	return DMUS_S_REQUEUE;
}

HRESULT STDMETHODCALLTYPE CNotifyTool::Flush(
	IDirectMusicPerformance* pPerf, 
	DMUS_PMSG* pPMsg,
	REFERENCE_TIME rt)
{
	UNREFERENCED_PARAMETER(rt);
	UNREFERENCED_PARAMETER(pPerf);

	// returning S_FREE frees the message. If StampPMsg()
	// fails, there is no destination for this message so
	// free it.
	if(( NULL == pPMsg->pGraph ) ||
		FAILED(pPMsg->pGraph->StampPMsg(pPMsg)))
	{
		return DMUS_S_FREE;
	}
	return DMUS_S_REQUEUE;
}


// Constructor/Desctructor
CMIDIInputContainer::CMIDIInputContainer(IDirectMusic* pDMusic)
{
	m_pDMusic = pDMusic;

	m_fWantToEcho = FALSE;
	m_fMIDIInputRunning = FALSE;

	ZeroMemory( m_aMIDIInputContainer, sizeof(MIDIInputContainer) * MIDI_IN_PORTS );

	ZeroMemory(	m_afThruingMIDI, sizeof(BOOL) * 16 );
	ZeroMemory( m_adwThruingMIDI, sizeof(DWORD) * 16 );

	ZeroMemory(	m_ahMIDIInputHandles, sizeof(HANDLE) * MIDI_IN_PORTS );
	m_dwValidInputHandles = 0;

	::InitializeCriticalSection( &m_csEchoMIDI );
	m_hEchoMIDIInputExitEvent = NULL;
	m_hEchoMIDIInputThread = NULL;

	for( int i=1; i < MIDI_IN_PORTS; i++ )
	{
		m_aMIDIInputContainer[i].m_fEchoMIDI = true;
	}
}

CMIDIInputContainer::~CMIDIInputContainer()
{
	// Cleans up thread
	StopMIDIInputThreadAndRemovePorts();

	// Clean up the list of MIDI input ports
	while( !m_lstMIDIInPorts.IsEmpty() )
	{
		PortContainer *pPortContainer = m_lstMIDIInPorts.RemoveHead();
		if( pPortContainer->m_pPort )
		{
			pPortContainer->m_pPort->Release();
		}
		if( pPortContainer->m_hPortEvent )
		{
			::CloseHandle( pPortContainer->m_hPortEvent );
		}

		delete pPortContainer;
	}

	// Clear the array of handles
	ZeroMemory( m_ahMIDIInputHandles, sizeof(HANDLE) * MIDI_IN_PORTS );
	m_dwValidInputHandles = 0;

	// Clean up the critical section
	::DeleteCriticalSection( &m_csEchoMIDI );
}

void CMIDIInputContainer::InitializeFromRegistry( void )
{
	TCHAR szValue[64];
	TCHAR szRegPath[MAX_PATH];
	// Look for a DirectMusic MIDI In port
	_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\PortConfiguration\\") );

	// Iterate through the list of echo MIDI input ports and initialize them
	for( int i=1; i < MIDI_IN_PORTS; i++ )
	{
		if( i==1 )
		{
			_tcscpy( szValue, _T("DMEchoMidiInPort") );
		}
		else
		{
			_stprintf( szValue, _T("DMEchoMidiInPort%d"), i );
		}

		DWORD dwCbData = sizeof(TCHAR) * 100;
		TCHAR szGuid[100];
		GUID guidPortGUID = GUID_AllZeros;
		if( GetRegString( HKEY_CURRENT_USER, szRegPath, szValue, szGuid, &dwCbData ) )
		{
			// Parse port GUID
			LPOLESTR psz = new OLECHAR[100];
			MultiByteToWideChar( CP_ACP, 0, szGuid, -1, psz, 100);

			CLSIDFromString(psz, &guidPortGUID);
			delete psz;
		}

		// Look up 'Echo MIDI In' Output PChannel group
		if( i==1 )
		{
			_tcscpy( szValue, _T("EchoMidiInPChannelGroup") );
		}
		else
		{
			_stprintf( szValue, _T("EchoMidiInPChannelGroup%d"), i );
		}

		DWORD dwPChannelBase = 0;
		GetRegDWORD( HKEY_CURRENT_USER, szRegPath, szValue, &dwPChannelBase );

		SetDMusicEchoMIDIIn( i, dwPChannelBase, guidPortGUID );
	}

	// Check if echo MIDI input is supposed to be enabled or disabled
	_tcscpy( szValue, _T("EnableEchoMidiInPort") );
	DWORD dwEnable = 0;
	if( GetRegDWORD( HKEY_CURRENT_USER, szRegPath, szValue, &dwEnable ) )
	{
		if( dwEnable != 0 )
		{
			m_fWantToEcho = TRUE;
		}
	}

	// Look for a normal MIDI input port
	DWORD dwCbData = sizeof(TCHAR) * 100;
	TCHAR szGuid[100];
	bool fGotDMusicPort = false;

	// Look in the new registry location
	if( GetNewRegString( _T("DMMidiInPort"), szGuid, &dwCbData ) )
	{
		fGotDMusicPort = true;
	}
	else
	{
		// Look in the old registry location

		// Convert CLSID_CConductor to a string
		LPOLESTR psz;
		if( SUCCEEDED( StringFromIID(CLSID_CConductor, &psz) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
			CoTaskMemFree( psz );

			// Build the registry key path
			_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\Components\\") );
			_tcscat( szRegPath, szGuid );

			// Try and read the value
			if( GetRegString( HKEY_LOCAL_MACHINE, szRegPath, _T("DMMidiInPort"), szGuid, &dwCbData ) )
			{
				// Found a GUID
				fGotDMusicPort = true;

				// Write the GUID to the new key
				SetNewRegString( _T("DMMidiInPort"), szGuid );
			}
		}
	}

	// Did we find a MIDI input port
	if( fGotDMusicPort )
	{
		// Yes - store its GUID

		// Parse port GUID
		LPOLESTR psz = new OLECHAR[100];
		MultiByteToWideChar( CP_ACP, 0, szGuid, -1, psz, 100);

		GUID guidPortGUID = GUID_AllZeros;
		CLSIDFromString(psz, &guidPortGUID);
		delete psz;

		// Store the GUID
		SetDMusicEchoMIDIIn( 0, 0, guidPortGUID );
	}
	else
	{
		// No MIDI input GUID found

		// Enumerate through all ports
		HRESULT hr;
		for(DWORD index = 0; ; index++)
		{
			// Initialize dmpc
			DMUS_PORTCAPS dmpc;
			ZeroMemory(&dmpc, sizeof(dmpc));
			dmpc.dwSize = sizeof(DMUS_PORTCAPS);

			// Get the port's capabilities
			hr = m_pDMusic->EnumPort(index, &dmpc);
			if(SUCCEEDED(hr) && hr != S_FALSE)
			{
				if ( !fGotDMusicPort && (dmpc.dwClass == DMUS_PC_INPUTCLASS) )
				{
					// Found an input port

					// Store the GUID
					SetDMusicEchoMIDIIn( 0, 0, dmpc.guidPort );

					fGotDMusicPort = true;
				}
			}
			else
			{
				break;
			}
		}
	}

	// Now, try and start the input thread (the method will decide whether it needs to or not)
	StartMIDIInputThreadAndCreatePorts();
}

HRESULT CMIDIInputContainer::OnOutputEnabled( void )
{
	return StartMIDIInputThreadAndCreatePorts();
}

HRESULT CMIDIInputContainer::StartMIDIInputThreadAndCreatePorts( void )
{
	if( !g_pconductor->m_fOutputEnabled || !AnyMIDIInputValid() )
	{
		// Output is 'disabled' or there are no valid MIDI input devices
		return S_FALSE;
	}

	// Stop and destroy the existing MIDI input thread and ports
	StopMIDIInputThreadAndRemovePorts();

	// Reset the latency offsets
	ResetLatencyOffset();

	// Not really necessary, since the thread shouldn't be doing anything, but it can't hurt
	::EnterCriticalSection( &m_csEchoMIDI );

	HRESULT hr = S_OK;

	// Iterate through all the MIDI input port settings
	int i=0;
	while( (i < MIDI_IN_PORTS) && SUCCEEDED(hr) )
	{
		// If we don't want Echo Midi Input, then only do the first port
		if( !m_fWantToEcho
		&&	i >= 1 )
		{
			break;
		}

		// We should not yet have a port container assigned
		ASSERT( !m_aMIDIInputContainer[i].m_pPortContainer );

		// Try and find an existing port
		PortContainer *pPortContainer = FindPort( m_aMIDIInputContainer[i].m_guidPort );

		// If no existing port and we have a valid GUID
		if( !pPortContainer
		&&	(m_aMIDIInputContainer[i].m_guidPort != GUID_AllZeros) )
		{
			// Create a new port container
			pPortContainer = new PortContainer;
			if( !pPortContainer )
			{
				hr = E_OUTOFMEMORY;
			}
			else
			{
				// Initialize the port container
				pPortContainer->m_pPort = NULL;
				pPortContainer->m_hPortEvent = NULL;
				pPortContainer->m_lRef = 0;

				// Try and create the port
				DMUS_PORTPARAMS PortParams;
				PortParams.dwSize = sizeof( DMUS_PORTPARAMS );
				PortParams.dwValidParams = 0;
				hr = m_pDMusic->CreatePort( m_aMIDIInputContainer[i].m_guidPort, &PortParams, &pPortContainer->m_pPort, NULL );
				if( SUCCEEDED( hr ) )
				{
					// Try and create an event for the port
					pPortContainer->m_hPortEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
					if( pPortContainer->m_hPortEvent )
					{
						// Tell the port to signal the event when MIDI data is available
						hr = pPortContainer->m_pPort->SetReadNotificationHandle( pPortContainer->m_hPortEvent );
					}
				}

				// If we failed, clean up
				if( FAILED( hr ) )
				{
					if( pPortContainer->m_pPort )
					{
						pPortContainer->m_pPort->Release();
					}
					if( pPortContainer->m_hPortEvent )
					{
						::CloseHandle( pPortContainer->m_hPortEvent );
					}

					delete pPortContainer;
					pPortContainer = NULL;
				}
				else
				{
					// Add to list of ports
					m_lstMIDIInPorts.AddHead( pPortContainer );

					// Add to array of notification handles
					m_ahMIDIInputHandles[m_dwValidInputHandles] = pPortContainer->m_hPortEvent;
					m_dwValidInputHandles++;
				}
			}
		}

		// If we have a port container
		if( pPortContainer )
		{
			// Save it
			m_aMIDIInputContainer[i].m_pPortContainer = pPortContainer;

			// Clear the latency offset
			m_aMIDIInputContainer[i].m_rtLatencyOffset = 0;

			// Increment the reference count
			InterlockedIncrement( &pPortContainer->m_lRef );
		}

		// Go to the next MIDI input port setting
		i++;
	}

	// Only start the thread if we were successful and we have at least one valid
	// handle to wait on
	if( SUCCEEDED(hr)
	&&	m_dwValidInputHandles )
	{
		// Start MIDI input thread
		CWinThread *pThread = ::AfxBeginThread( EchoMIDIInThreadProc, this );
		if( pThread )
		{
			// Save a pointer to the input thread
			m_hEchoMIDIInputThread = pThread->m_hThread;
			pThread->m_bAutoDelete = TRUE;

			// Activate the input ports
			POSITION pos = m_lstMIDIInPorts.GetHeadPosition();
			while( pos )
			{
				m_lstMIDIInPorts.GetNext(pos)->m_pPort->Activate( TRUE );
			}

			// Set the flag that we're doing MIDI input
			m_fMIDIInputRunning = TRUE;
		}
		else
		{
			TRACE("CMIDIInputContainer: Failed to start Echo MIDI input thread.\n");
			ASSERT( FALSE );

			// Clean up the MIDI input ports
			StopMIDIInputThreadAndRemovePorts();

			hr = E_FAIL;
		}
	}

	::LeaveCriticalSection( &m_csEchoMIDI );

	return hr;
}

HRESULT CMIDIInputContainer::StopMIDIInputThreadAndRemovePorts( void )
{
	// Deactivate the input ports
	POSITION pos = m_lstMIDIInPorts.GetHeadPosition();
	while( pos )
	{
		PortContainer *pPortContainer = m_lstMIDIInPorts.GetNext(pos);
		pPortContainer->m_pPort->Activate( FALSE );
		pPortContainer->m_pPort->SetReadNotificationHandle( NULL );
	}

	// Exit the thread
	if( m_hEchoMIDIInputThread )
	{
		DWORD dwExitCode;
		if( GetExitCodeThread( m_hEchoMIDIInputThread, &dwExitCode ) )
		{
			if( dwExitCode == STILL_ACTIVE )
			{
				// Create an event so we can know when the thread exits
				ASSERT( m_hEchoMIDIInputExitEvent == NULL );
				if( !m_hEchoMIDIInputExitEvent )
				{
					m_hEchoMIDIInputExitEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
				}

				// Signal the MIDI input handler thread so it can exit
				::SetEvent( m_ahMIDIInputHandles[0] );

				// Wait for the input handler thread to exit
				::WaitForSingleObject( m_hEchoMIDIInputExitEvent, 2000 );
			}
		}
		m_hEchoMIDIInputThread = NULL;
	}

	if( m_hEchoMIDIInputExitEvent )
	{
		::CloseHandle( m_hEchoMIDIInputExitEvent );
		m_hEchoMIDIInputExitEvent = NULL;
	}

	::EnterCriticalSection( &m_csEchoMIDI );

	// Remove the input ports
	while( !m_lstMIDIInPorts.IsEmpty() )
	{
		PortContainer *pPortContainer = m_lstMIDIInPorts.RemoveHead();
		pPortContainer->m_pPort->Release();
		if( pPortContainer->m_hPortEvent )
		{
			::CloseHandle( pPortContainer->m_hPortEvent );
		}

		delete pPortContainer;
	}

	// Clear the array of handles
	ZeroMemory( m_ahMIDIInputHandles, sizeof(HANDLE) * MIDI_IN_PORTS );
	m_dwValidInputHandles = 0;

	// Clear the MIDI input pointers to the ports
	for( int i=0; i < MIDI_IN_PORTS; i++ )
	{
		m_aMIDIInputContainer[i].m_pPortContainer = NULL;
	}

	::LeaveCriticalSection( &m_csEchoMIDI );

	m_fMIDIInputRunning = FALSE;
	m_hEchoMIDIInputThread = NULL;

	return S_OK;
}

HRESULT CMIDIInputContainer::OnOutputDisabled( void )
{
	return StopMIDIInputThreadAndRemovePorts();
}

HRESULT CMIDIInputContainer::SetDMusicEchoMIDIIn( DWORD dwEchoID, DWORD dwNewOutputBlock, REFGUID guidEchoInputPort, bool fUpdateNow )
{
	ASSERT( dwEchoID < MIDI_IN_PORTS );
	if( dwEchoID >= MIDI_IN_PORTS )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	m_aMIDIInputContainer[dwEchoID].m_dwPChannelBase = dwNewOutputBlock * 16;

	if( m_aMIDIInputContainer[dwEchoID].m_guidPort != guidEchoInputPort )
	{
		// Port change, so stop and re-start existing MIDI Input thread, if requested
		if( fUpdateNow )
		{
			hr = StopMIDIInputThreadAndRemovePorts();
		}

		if( SUCCEEDED(hr) )
		{
			// Save the original GUID
			GUID guidOrig = m_aMIDIInputContainer[dwEchoID].m_guidPort;

			// Copy the GUID
			m_aMIDIInputContainer[dwEchoID].m_guidPort = guidEchoInputPort;

			// Restart MIDI input, if requested
			if( fUpdateNow )
			{
				hr = StartMIDIInputThreadAndCreatePorts();
			}

			// If we failed
			if( FAILED(hr) )
			{
				// Can only get here if we were requested to update the port
				ASSERT( fUpdateNow );

				// Restore the original GUID
				m_aMIDIInputContainer[dwEchoID].m_guidPort = guidOrig;

				// Try to start MIDI input again
				StartMIDIInputThreadAndCreatePorts();
			}
		}
	}

	return hr;
}

void CMIDIInputContainer::UpdateMIDIThruChannels( IDirectMusicAudioPath *pDMAudiopath )
{
	if( !m_aMIDIInputContainer[0].m_pPortContainer
	||	!m_aMIDIInputContainer[0].m_pPortContainer->m_pPort
	||	!pDMAudiopath )
	{
		return;
	}

	IDirectMusicThru *pIDMThru;
	if( SUCCEEDED( m_aMIDIInputContainer[0].m_pPortContainer->m_pPort->QueryInterface( IID_IDirectMusicThru, (void**) &pIDMThru ) ) )
	{
		for( DWORD dwPChannel = 0; dwPChannel < 16; dwPChannel++ )
		{
			if( m_afThruingMIDI[dwPChannel] )
			{
				DWORD dwRealPChannel;
				if( SUCCEEDED( pDMAudiopath->ConvertPChannel( m_adwThruingMIDI[dwPChannel], &dwRealPChannel ) ) )
				{
					IDirectMusicPort *pIDMOutputPort;
					DWORD dwGroup, dwMChannel;

					if( SUCCEEDED( g_pconductor->m_pDMPerformance->PChannelInfo( dwRealPChannel, &pIDMOutputPort, &dwGroup, &dwMChannel  ) ) )
					{
						pIDMThru->ThruChannel( 1, dwPChannel, dwGroup, dwMChannel, pIDMOutputPort );
						pIDMOutputPort->Release();
					}
				}
			}
		}
		pIDMThru->Release();
	}
}	

void CMIDIInputContainer::ClearThruConnections( void )
{
	if( !m_aMIDIInputContainer[0].m_pPortContainer
	||	!m_aMIDIInputContainer[0].m_pPortContainer->m_pPort )
	{
		return;
	}

	IDirectMusicThru *pIDMThru;
	if( SUCCEEDED( m_aMIDIInputContainer[0].m_pPortContainer->m_pPort->QueryInterface( IID_IDirectMusicThru, (void**) &pIDMThru ) ) )
	{
		for( DWORD dwPChannel = 0; dwPChannel < 16; dwPChannel++ )
		{
			if( m_afThruingMIDI[dwPChannel] )
			{
				pIDMThru->ThruChannel( 1, dwPChannel, 0, 0, NULL );
			}
		}
		pIDMThru->Release();
	}
}

void CMIDIInputContainer::UpdateRegistry( void )
{

	// Save the input GUIDs
	LPOLESTR psz;
	TCHAR szGuid[100];
	TCHAR szValue[100];
	for( int i=0; i < MIDI_IN_PORTS; i++ )
	{
		if( i==0 )
		{
			_tcscpy( szValue, _T("DMMidiInPort") );
		}
		else if( i==1 )
		{
			_tcscpy( szValue, _T("DMEchoMidiInPort") );
		}
		else
		{
			_stprintf( szValue, _T("DMEchoMidiInPort%d"), i );
		}

		// Set DirectMusic input GUID
		if( SUCCEEDED( StringFromIID(m_aMIDIInputContainer[i].m_guidPort, &psz) ) )
		{
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
			CoTaskMemFree( psz );
			SetNewRegString( szValue, szGuid );
		}

		if( i > 1 )
		{
			if( i==1 )
			{
				_tcscpy( szValue, _T("EchoMidiInPChannelGroup") );
			}
			else
			{
				_stprintf( szValue, _T("EchoMidiInPChannelGroup%d"), i );
			}

			SetNewRegDWORD( szValue, m_aMIDIInputContainer[i].m_dwPChannelBase / 16, TRUE );
		}
	}

	// Set 'Echo MIDI In' Enable/Disable state
	SetNewRegDWORD(_T("EnableEchoMidiInPort"), m_fWantToEcho, TRUE);
}

void CMIDIInputContainer::RestartMIDIInThreadIfNecessary( void )
{
	// Exit the thread
	if( m_hEchoMIDIInputThread )
	{
		DWORD dwExitCode;
		if( GetExitCodeThread( m_hEchoMIDIInputThread, &dwExitCode ) )
		{
			if( dwExitCode != STILL_ACTIVE )
			{
				// If the MIDI input thread died, restart it
				CWinThread *pThread = ::AfxBeginThread( EchoMIDIInThreadProc, this );
				if( pThread )
				{
					TRACE("CMIDIInputContainer::RestartMIDIInThreadIfNecessary: Successfully restarted MIDI input thread.\n");

					// Save a pointer to the input thread
					m_hEchoMIDIInputThread = pThread->m_hThread;
					pThread->m_bAutoDelete = TRUE;
				}
				else
				{
					TRACE("CMIDIInputContainer::RestartMIDIInThreadIfNecessary: Failed to restart MIDI input thread.\n");
				}
			}
		}
	}
}

REFERENCE_TIME SampleLatency( IDirectMusicPort *pIDMOutputPort, IDirectMusicPerformance *pDMPerformance )
{
	if( g_rpLastSampledPort == pIDMOutputPort )
	{
		return s_rtLastSampledLatency;
	}

	// Default to 1 second latency
	REFERENCE_TIME rtResult = 1000 * 100000;

	IReferenceClock *pIReferenceClock;
	if( pIDMOutputPort && SUCCEEDED( pIDMOutputPort->GetLatencyClock( &pIReferenceClock ) ) )
	{
		rtResult = 0;

		REFERENCE_TIME rtNow, rtLatency;
		for( int i=0; i<40; i++ )
		{
			if( SUCCEEDED( pDMPerformance->GetTime( &rtNow, NULL ) ) )
			{
				if( SUCCEEDED( pIReferenceClock->GetTime( &rtLatency ) ) )
				{
					rtResult += rtLatency - rtNow;
				}
			}

			Sleep(3);
		}

		rtResult /= 40;

		pIReferenceClock->Release();
	}

	g_rpLastSampledPort = pIDMOutputPort;
	s_rtLastSampledLatency = rtResult;

	return rtResult;
}

HRESULT CMIDIInputContainer::SetPChannelThru( DWORD dwInputChannel, DWORD dwPChannel, IDirectMusicAudioPath *pDMAudiopath )
{
	// Ensure the audiopath pointer is valid, and that we have a MIDI input port
	if( !pDMAudiopath
	||	!m_aMIDIInputContainer[0].m_pPortContainer
	||	!m_aMIDIInputContainer[0].m_pPortContainer->m_pPort )
	{
		return E_FAIL;
	}

	// If we're already thruing, and we're just going to thru to the same channel, return S_FALSE
	if( m_afThruingMIDI[dwInputChannel] && (m_adwThruingMIDI[dwInputChannel] == dwPChannel) )
	{
		return S_FALSE;
	}

	// Convert to a PChannel that the Performance engine knows about
	DWORD dwRealPChannel;
	if( FAILED( pDMAudiopath->ConvertPChannel( dwPChannel, &dwRealPChannel ) ) )
	{
		return E_FAIL;
	}

	// Find what port, channel group, and channel the PChannel maps to
	IDirectMusicPort *pIDMOutputPort = NULL;
	DWORD dwGroup, dwMChannel;
	HRESULT hr = E_FAIL;
	if( SUCCEEDED( g_pconductor->m_pDMPerformance->PChannelInfo( dwRealPChannel, &pIDMOutputPort, &dwGroup, &dwMChannel  ) ) )
	{
		// Check to make sure we found a port
		if( pIDMOutputPort )
		{
			// Determine the latency of this output port
			REFERENCE_TIME rtLatency = SampleLatency( pIDMOutputPort, g_pconductor->m_pDMPerformance );
			TRACE("SetPChannelThru: Port latency: %I64d\n", rtLatency );

			// If the latency is low enough
			if( rtLatency < LATENCY_CUTOFF_MS * 10000 )
			{
				// Query for the Thru interface
				IDirectMusicThru *pIDMThru;
				if( SUCCEEDED( m_aMIDIInputContainer[0].m_pPortContainer->m_pPort->QueryInterface( IID_IDirectMusicThru, (void**) &pIDMThru ) ) )
				{
					// If we're already thruing this channel
					if( m_afThruingMIDI[dwInputChannel] )
					{
						// Clear the Thru channel
						if( FAILED( pIDMThru->ThruChannel( 1, dwInputChannel, 0, 0, NULL ) ) )
						{
							// Failed to clear the channel
							// TODO: Display error dialog?
							ASSERT(FALSE);
						}
						else
						{
							m_afThruingMIDI[dwInputChannel] = false;
						}
					}

					// Thru this channel to the given MIDI group, output channel, and output port
					if( SUCCEEDED( pIDMThru->ThruChannel( 1, dwInputChannel, dwGroup, dwMChannel, pIDMOutputPort ) ) )
					{

						// We succeeded - save the output PChannel and set the flag that we're thruing
						hr = S_OK;
						m_adwThruingMIDI[dwInputChannel] = dwPChannel;
						m_afThruingMIDI[dwInputChannel] = true;
					}
					pIDMThru->Release();
				}
			}
			pIDMOutputPort->Release();
		}
	}

	return hr;
}

HRESULT CMIDIInputContainer::CancelPChannelThru( DWORD dwInputChannel )
{
	// Ensure the audiopath pointer is valid, and that we have a MIDI input port
	if( !m_aMIDIInputContainer[0].m_pPortContainer
	||	!m_aMIDIInputContainer[0].m_pPortContainer->m_pPort )
	{
		return E_FAIL;
	}

	if( !m_afThruingMIDI[dwInputChannel] )
	{
		return S_FALSE;
	}

	HRESULT hr = E_FAIL;
	IDirectMusicThru *pIDMThru;
	if( SUCCEEDED( m_aMIDIInputContainer[0].m_pPortContainer->m_pPort->QueryInterface( IID_IDirectMusicThru, (void**) &pIDMThru ) ) )
	{
		hr = pIDMThru->ThruChannel( 1, dwInputChannel, 0, 0, NULL );
		pIDMThru->Release();
	}

	m_afThruingMIDI[dwInputChannel] = false;

	return hr;
}

void CMIDIInputContainer::ResetLatencyOffset( void )
{
	for( int i=1; i<MIDI_IN_PORTS; i++ )
	{
		m_aMIDIInputContainer[i].m_rtLatencyOffset = 0;
	}
}

HRESULT CMIDIInputContainer::EnableEchoMIDI( BOOL fEnable )
{
	if( m_fWantToEcho != fEnable )
	{
		m_fWantToEcho = fEnable;
		if( fEnable )
		{
			return StartMIDIInputThreadAndCreatePorts();
		}
		else
		{
			return StopMIDIInputThreadAndRemovePorts();
		}
	}

	return S_FALSE;
}

PortContainer *CMIDIInputContainer::FindPort( const REFGUID guidPort )
{
	for( int i=0; i<MIDI_IN_PORTS; i++ )
	{
		if( (guidPort == m_aMIDIInputContainer[i].m_guidPort)
		&&	m_aMIDIInputContainer[i].m_pPortContainer )
		{
			return m_aMIDIInputContainer[i].m_pPortContainer;
		}
	}

	return NULL;
}

bool CMIDIInputContainer::AnyMIDIInputValid( void )
{
	for( int i=0; i<MIDI_IN_PORTS; i++ )
	{
		if( GUID_AllZeros != m_aMIDIInputContainer[i].m_guidPort )
		{
			return true;
		}
	}

	return false;
}
