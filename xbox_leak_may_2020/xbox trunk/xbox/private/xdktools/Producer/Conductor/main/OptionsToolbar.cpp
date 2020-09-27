// OptionsToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsToolbar.h"
#include "OutputTool.h"
#include "CConduct.h"
#include "DlgMIDI.h"
#include "DlgMetronome.h"
#include "Toolbar.h"
#include "TREntry.h"
#include <initguid.h>
#include "AudioPathDesigner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CMIDIInputContainer* g_pMIDIInputContainer;
extern TCHAR g_szPhoneyDSoundFilterName[MAX_PATH];

COptionsToolbarHandler::COptionsToolbarHandler()
{
	m_fCursorEnabled = TRUE;
	m_lCountInBars = 0;
	m_fCountOnlyOnRecord = TRUE;

	// Read the cursor state from the registry
	DWORD dwID;
	if( GetNewRegDWORD( _T("TimeCursorEnabled"), &dwID ) )
	{
		if( dwID == 0 )
		{
			m_fCursorEnabled = FALSE;
		}
	}

	// Read the number of Count-In bars from the registry
	if( GetNewRegDWORD( _T("CountInBars"), &dwID ) )
	{
		m_lCountInBars = min( dwID, 99 );
	}

	// Read whether we should only Count-In on Record from the registry
	if( GetNewRegDWORD( _T("CountInOnlyOnRecord"), &dwID ) )
	{
		if( dwID == 0 )
		{
			m_fCountOnlyOnRecord = FALSE;
		}
	}
}

COptionsToolbarHandler::~COptionsToolbarHandler()
{
	m_font.DeleteObject();
}

BOOL COptionsToolbarHandler::SetBtnState( int nID, UINT nState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ::IsWindow( m_hWndToolbar ) )
	{
		return ::SendMessage( m_hWndToolbar, TB_SETSTATE, nID, MAKELONG(nState, 0) );
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// COptionsToolbarHandler IDMUSProdToolBar::GetInfo

HRESULT COptionsToolbarHandler::GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( (phWndOwner == NULL)
	||	(phInstance == NULL)
	||	(pnResourceId == NULL)
	||	(pbstrTitle == NULL) )
	{
		return E_POINTER;
	}

	*phWndOwner = m_hWnd;
	*phInstance = _Module.GetResourceInstance();
	*pnResourceId = (UINT)-1;  // we are going to build the toolbar from scratch

	CString strTitle;
	strTitle.LoadString(IDS_OPTIONSTOOLBAR_TITLE);
	*pbstrTitle = strTitle.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// COptionsToolbarHandler IDMUSProdToolBar::GetMenuText

HRESULT COptionsToolbarHandler::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CString strText;
	strText.LoadString(IDS_OPTIONSTOOLBAR_MENUTEXT);
	*pbstrText = strText.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// COptionsToolbarHandler IDMUSProdToolBar::GetMenuHelpText

HRESULT COptionsToolbarHandler::GetMenuHelpText( BSTR* pbstrMenuHelpText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrMenuHelpText == NULL )
	{
		return E_POINTER;
	}

	CString strMenuHelpText;
	strMenuHelpText.LoadString(IDS_OPTIONSTOOLBAR_HELP);
	*pbstrMenuHelpText = strMenuHelpText.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// COptionsToolbarHandler IDMUSProdToolBar::Initialize

HRESULT COptionsToolbarHandler::Initialize( HWND hWndToolBar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( hWndToolBar == NULL )
	{
		return E_INVALIDARG;
	}
	m_hWndToolbar = hWndToolBar;
 
	CToolBarCtrl* pToolBarCtrl = new CToolBarCtrl;
	if( pToolBarCtrl == NULL )
	{
		return E_OUTOFMEMORY;
	}

	pToolBarCtrl->Attach( hWndToolBar );
	pToolBarCtrl->AddBitmap( 10, IDB_Transport );

	// Create font for Audiopath combo box
	CClientDC* pDC = new CClientDC( pToolBarCtrl );

	int nHeight = -( (pDC->GetDeviceCaps(LOGPIXELSY) * 8) / 72 );

	CString strFontName;
	if( strFontName.LoadString( IDS_TOOLBAR_FONT ) == 0)
	{
		strFontName = _T("MS Sans Serif");
	}
	m_font.CreateFont( nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName );
	
	CFont* pOldFont = pDC->SelectObject( &m_font );
	
	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	int cxChar = tm.tmAveCharWidth;
	int cyChar = tm.tmHeight + tm.tmExternalLeading;

	pDC->SelectObject( pOldFont );
	delete pDC;

	TBBUTTON button;
	button.dwData = 0;
	button.iString = NULL;

	// 4 - Options
	button.iBitmap = 4;
	button.idCommand = ID_TRANSP_OPT;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 0, &button );

	// 3 - MIDI
	button.iBitmap = 3;
	button.idCommand = ID_TRANSP_MIDI;
	button.fsState = TBSTATE_INDETERMINATE;
	button.fsStyle = TBSTYLE_CHECK;
	pToolBarCtrl->InsertButton( 0, &button );

	// 2 - Cursor
	button.iBitmap = 7;
	button.idCommand = ID_TRANSP_CURSOR;
	button.fsState = BYTE(m_fCursorEnabled ? TBSTATE_ENABLED | TBSTATE_CHECKED : TBSTATE_ENABLED);
	button.fsStyle = TBSTYLE_CHECK;
	pToolBarCtrl->InsertButton( 0, &button );

	// 1 - Metronome
	button.iBitmap = 6;
	button.idCommand = ID_TRANSP_METRONOME;
	button.fsState = BYTE(g_pconductor->m_fMetronomeEnabled ? TBSTATE_ENABLED | TBSTATE_CHECKED : TBSTATE_ENABLED);
	button.fsStyle = TBSTYLE_CHECK;
	pToolBarCtrl->InsertButton( 0, &button );

	// 0 - Separator for Audiopath combobox
	button.iBitmap = cxChar * 30;
	button.idCommand = ID_TRANSP_AUDIOPATH;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton( 0, &button );

	// Attach the Audiopath combobox
	CRect rect;
	pToolBarCtrl->GetItemRect( 0, &rect );
	rect.bottom = rect.top + (cyChar * 21);
	if( !m_comboAudiopath.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_SORT,
		rect, pToolBarCtrl, ID_TRANSP_AUDIOPATH) )
	{
		return -1;
	}
	m_comboAudiopath.SetFont( &m_font );
	m_comboAudiopath.EnableWindow(TRUE);

	UpdateComboBoxFromConductor();

	if( pToolBarCtrl )
	{
		pToolBarCtrl->Detach();
		delete pToolBarCtrl;
	}

	return S_OK;
}

void COptionsToolbarHandler::UpdateComboBoxFromConductor( void )
{
	while( m_comboAudiopath.GetCount() > 0 )
	{
		m_comboAudiopath.DeleteString( 0 );
	}

	if( g_pconductor->m_pAudiopathListItem
	&&	!g_pconductor->m_pAudiopathListItem->strName.IsEmpty() )
	{
		// Add the text to the combo box
		int nStringIndex = m_comboAudiopath.AddString( g_pconductor->m_pAudiopathListItem->strName );

		// Set the combobox item to point to nothing
		m_comboAudiopath.SetItemDataPtr( nStringIndex, NULL );

		// Set the current selection to this item
		m_comboAudiopath.SetCurSel( nStringIndex );
	}
	else
	{
		// Load the 'No default audiopath' string
		CString strNone;
		if( strNone.LoadString( IDS_NO_AUDIOPATH ) )
		{
			// Add the text to the combo box
			int nStringIndex = m_comboAudiopath.AddString( strNone );

			// Set the combobox item to point to nothing
			m_comboAudiopath.SetItemDataPtr( nStringIndex, NULL );

			// Set the current selection to this item
			m_comboAudiopath.SetCurSel( nStringIndex );
		}
	}
}

LRESULT COptionsToolbarHandler::OnMidiClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( g_pconductor->m_fOutputEnabled )
	{
		g_pconductor->SuspendOutput();
		SetBtnState( ID_TRANSP_MIDI, TBSTATE_ENABLED );
	}
	else {
		g_pconductor->ResumeOutput();
		SetBtnState( ID_TRANSP_MIDI, TBSTATE_ENABLED | TBSTATE_CHECKED );
	}

	if( g_pconductor->m_pToolbarHandler )
	{
		g_pconductor->m_pToolbarHandler->SetStateAuto();
	}

	return TRUE;
}

LRESULT COptionsToolbarHandler::OnOptionsClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DlgMIDI dlg;
	dlg.m_dwLatency = g_pconductor->m_dwLatency;
	dlg.m_fLatencyAppliesToAllAudioPaths = g_pconductor->m_fLatencyAppliesToAllAudiopaths;

	dlg.m_dwSampleRate = g_pconductor->m_dwSampleRate;
	dlg.m_dwVoices = g_pconductor->m_dwVoices;
	dlg.m_clsidDefaultSynth = g_pconductor->m_clsidDefaultSynth;

	dlg.m_fDownloadGM = g_pconductor->m_fDownloadGM;
	dlg.m_fDownloadDLS = g_pconductor->m_fDownloadDLS;

	dlg.m_fUsePhoneyDSound = g_pconductor->m_fUsePhoneyDSound;
	strcpy( dlg.m_strPhoneyFilterName, g_szPhoneyDSoundFilterName );

	if( IDCANCEL == dlg.DoModal() )
	{
		return TRUE;
	}


	if( g_pMIDIInputContainer )
	{
		g_pMIDIInputContainer->UpdateRegistry();

		g_pMIDIInputContainer->RestartMIDIInThreadIfNecessary();
	}

	// Set 'Echo MIDI In' Output PChannel group
	if( dlg.m_dwLatency != g_pconductor->m_dwLatency
	||	dlg.m_fLatencyAppliesToAllAudioPaths != g_pconductor->m_fLatencyAppliesToAllAudiopaths )
	{
		// Set 'Latency' value
		g_pconductor->m_dwLatency = dlg.m_dwLatency;
		SetNewRegDWORD(_T("Latency"), g_pconductor->m_dwLatency, TRUE);

		// Set 'Update Latency' state
		g_pconductor->m_fLatencyAppliesToAllAudiopaths = dlg.m_fLatencyAppliesToAllAudioPaths;
		SetNewRegDWORD(_T("ApplyLatencyToAllAudioPaths"), g_pconductor->m_fLatencyAppliesToAllAudiopaths, TRUE);

		g_pconductor->UpdateLatency();
	}

	if( g_pconductor->m_fDownloadGM != dlg.m_fDownloadGM )
	{
		// Set download GM flag
		g_pconductor->m_fDownloadGM = dlg.m_fDownloadGM;
		SetNewRegDWORD(_T("DownloadGM"), g_pconductor->m_fDownloadGM, TRUE);

		g_pconductor->DownOrUnLoadGM();
	}


	if( g_pconductor->m_fDownloadDLS != dlg.m_fDownloadDLS )
	{
		// Set download DLS flag
		g_pconductor->m_fDownloadDLS = dlg.m_fDownloadDLS;
		SetNewRegDWORD(_T("DownloadDLS"), g_pconductor->m_fDownloadDLS, TRUE);

		g_pconductor->DownOrUnLoadDLS();
	}

	if( dlg.m_dwSampleRate != g_pconductor->m_dwSampleRate
	||	dlg.m_clsidDefaultSynth != g_pconductor->m_clsidDefaultSynth
	||	dlg.m_dwVoices != g_pconductor->m_dwVoices
	||	dlg.m_fUsePhoneyDSound != g_pconductor->m_fUsePhoneyDSound
	||	strcmp( dlg.m_strPhoneyFilterName, g_szPhoneyDSoundFilterName) != 0 )
	{
		// Set 'Sample Rate' value
		g_pconductor->m_dwSampleRate = dlg.m_dwSampleRate;
		SetNewRegDWORD(_T("SampleRate"), g_pconductor->m_dwSampleRate, TRUE);

		// Set 'Voices' value
		g_pconductor->m_dwVoices = dlg.m_dwVoices;
		SetNewRegDWORD(_T("Voices"), g_pconductor->m_dwVoices, TRUE);

		// Set 'Default Synth' value
		LPOLESTR psz;
		g_pconductor->m_clsidDefaultSynth = dlg.m_clsidDefaultSynth;
		if( SUCCEEDED( StringFromIID(g_pconductor->m_clsidDefaultSynth, &psz) ) )
		{
			TCHAR szGuid[MAX_PATH];
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
			CoTaskMemFree( psz );
			SetNewRegString( _T("DMDefaultPort"), szGuid );
		}

		// Set 'Use Phoney DSound' value
		bool fChangedPhoneyDSound = (g_pconductor->m_fUsePhoneyDSound != dlg.m_fUsePhoneyDSound);
		g_pconductor->UsePhoneyDSound( dlg.m_fUsePhoneyDSound );
		SetNewRegDWORD(_T("UsePhoneyDSound"), g_pconductor->m_fUsePhoneyDSound ? 1 : 0, TRUE);

		// Set 'Default Synth' value
		strcpy( g_szPhoneyDSoundFilterName, dlg.m_strPhoneyFilterName );
		SetNewRegString( _T("PhoneyDSoundDefaultPort"), g_szPhoneyDSoundFilterName );

		// If we changed to use Phoney DSound, ensure that the latency value the user
		// set is actually used
		bool fOrigLatencyAppliesToAllAudiopaths = g_pconductor->m_fLatencyAppliesToAllAudiopaths;
		if( fChangedPhoneyDSound
		&&	dlg.m_fUsePhoneyDSound )
		{
			g_pconductor->m_fLatencyAppliesToAllAudiopaths = true;
		}

		// Now, re-initialize the performance
		g_pconductor->UpdateAudioSettings();

		// Reset m_fLatencyAppliesToAllAudiopaths to its initial value
		if( fChangedPhoneyDSound
		&&	dlg.m_fUsePhoneyDSound )
		{
			g_pconductor->m_fLatencyAppliesToAllAudiopaths = fOrigLatencyAppliesToAllAudiopaths;
		}
	}

	return TRUE;
}

CWndClassInfo& COptionsToolbarHandler::GetWndClassInfo()
{
	static CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, StartWindowProc,
		  0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW+1), 0, NULL, 0 },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}


LRESULT COptionsToolbarHandler::OnRightClick(WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//TRACE("%x %x %x %x\n",wNotifyCode, pNMHDR->hwndFrom,pNMHDR->idFrom,pNMHDR->code);

	// Get the cursor position (To put the menu there)
	POINT ptScreen, ptClient;
	BOOL  bResult;
	bResult = ::GetCursorPos( &ptScreen );
	ASSERT( bResult );
	if( !bResult )
	{
		return FALSE;
	}

	ptClient = ptScreen;
	bResult = ::ScreenToClient( pNMHDR->hwndFrom, &ptClient );
	ASSERT( bResult );
	if( !bResult )
	{
		return FALSE;
	}

	if( ::IsWindow( m_hWndToolbar ) )
	{
		CRect rect;
		// Metronome is 1st item.
		::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 1, (LPARAM) &rect );

		if( rect.PtInRect( ptClient ) )
		{
			DlgMetronome dlg;
			dlg.m_lCountInBars = m_lCountInBars;
			dlg.m_fCountOnlyOnRecord = m_fCountOnlyOnRecord;
			if( IDOK == dlg.DoModal() )
			{
				g_pconductor->m_bMetronomeVelocityOne = dlg.m_bOneVelocity;
				g_pconductor->m_bMetronomeNoteOne = dlg.m_bOneValue;
				g_pconductor->m_bMetronomeVelocityOther = dlg.m_bOtherVelocity;
				g_pconductor->m_bMetronomeNoteOther = dlg.m_bOtherValue;
				g_pconductor->m_dwMetronomePChannel = dlg.m_dwPChannel;
				
				SetNewRegDWORD( _T("MetronomeVelocity"), g_pconductor->m_bMetronomeVelocityOther, TRUE );
				SetNewRegDWORD( _T("MetronomeNote"), g_pconductor->m_bMetronomeNoteOther, TRUE );
				SetNewRegDWORD( _T("MetronomeOneVelocity"), g_pconductor->m_bMetronomeVelocityOne, TRUE );
				SetNewRegDWORD( _T("MetronomeOneNote"), g_pconductor->m_bMetronomeNoteOne, TRUE );
				SetNewRegDWORD( _T("MetronomePChannel"), g_pconductor->m_dwMetronomePChannel, TRUE );

				m_lCountInBars = dlg.m_lCountInBars;
				m_fCountOnlyOnRecord = dlg.m_fCountOnlyOnRecord;

				SetNewRegDWORD( _T("CountInBars"), m_lCountInBars, TRUE );
				SetNewRegDWORD( _T("CountInOnlyOnRecord"), m_fCountOnlyOnRecord, TRUE );
			}
		}
		else
		{
			// MIDI Options is 4th Item
			::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 4, (LPARAM) &rect );

			if( rect.PtInRect( ptClient ) )
			{
				OnOptionsClicked( wNotifyCode, ID_TRANSP_OPT, pNMHDR->hwndFrom, bHandled );
			}
		}
	}

	return TRUE;
}

LRESULT COptionsToolbarHandler::OnMetronomeClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( g_pconductor->m_fMetronomeEnabled )
	{
		g_pconductor->m_fMetronomeEnabled = FALSE;
		SetBtnState( ID_TRANSP_METRONOME, TBSTATE_ENABLED );
	}
	else {
		g_pconductor->m_fMetronomeEnabled = TRUE;
		SetBtnState( ID_TRANSP_METRONOME, TBSTATE_ENABLED | TBSTATE_CHECKED );
	}

	// Save the state in the registry
	SetNewRegDWORD( _T("MetronomeEnabled"), g_pconductor->m_fMetronomeEnabled, TRUE );

	return TRUE;
}

LRESULT COptionsToolbarHandler::OnCursorClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();

	if( g_pconductor->m_pToolbarHandler == NULL )
	{
		return FALSE;
	}

	if ( !g_pconductor->m_pToolbarHandler->IsValid( pDisplayedRegEntry ) )
	{
		return FALSE;
	}

	IDMUSProdTransport*  pITransport = NULL;
	pDisplayedRegEntry->GetTransport( &pITransport );
	if( !pITransport )
	{
		return FALSE;
	}

	if( m_fCursorEnabled )
	{
		m_fCursorEnabled = FALSE;
		SetBtnState( ID_TRANSP_CURSOR, TBSTATE_ENABLED );
	}
	else
	{
		m_fCursorEnabled = TRUE;
		SetBtnState( ID_TRANSP_CURSOR, TBSTATE_ENABLED | TBSTATE_CHECKED );
	}

	pITransport->TrackCursor( m_fCursorEnabled );

	// Write the state to the registry
	SetNewRegDWORD(_T("TimeCursorEnabled"), m_fCursorEnabled, TRUE);

	return TRUE;
}

// Structure for validating an interface pointer. 
//
struct _V_GENERIC_INTERFACE
{
    FARPROC *(__vptr[1]);
};

LRESULT COptionsToolbarHandler::OnCloseUpComboAudiopath( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nIndex = m_comboAudiopath.GetCurSel();
	if( nIndex >= 0 )
	{
		CAudiopathListItem *pAudiopathListItem = static_cast<CAudiopathListItem *>(m_comboAudiopath.GetItemDataPtr( nIndex ));
		if( pAudiopathListItem != g_pconductor->m_pAudiopathListItem)
		{
			// Stop everything to get rid of stuck notes
			g_pconductor->StopAllNotesAndSegments();
			Sleep( 100 );	// Necessary so unload of waves will not fail

			// Display an hourglass cursor
			CWaitCursor waitCursor;

			// Save a pointer to the old Audiopath
			CAudiopathListItem *pOldAudiopathListItem = g_pconductor->m_pAudiopathListItem;

			// Broadcast the WaveUnload notification
			if( g_pconductor->m_pDMAudiopath )
			{
				g_pconductor->BroadCastWaveNotification( GUID_ConductorUnloadWaves );

				// Send the port removal notification
				g_pconductor->BroadCastPortRemovalNotification();

				// If we were previously using an audiopath from a node, set the old audio path
				// to be inactive
		 		if( g_pconductor->m_pAudiopathListItem
				&&	g_pconductor->m_pAudiopathListItem->pNode )
				{
					// Try and get the IDMUSProdAudioPathInUse interface for the node
					IDMUSProdAudioPathInUse *pIDMUSProdAudiopathInUse;
					if( SUCCEEDED( g_pconductor->m_pAudiopathListItem->pNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
					{
						// Notify the Audiopath that it is no longer in use
						pIDMUSProdAudiopathInUse->UsingAudioPath( g_pconductor->m_pDMAudiopath, FALSE );
						pIDMUSProdAudiopathInUse->Release();
					}

					// Remove from the audiopath's notification list
					g_pconductor->m_pFrameWork->RemoveFromNotifyList( g_pconductor->m_pAudiopathListItem->pNode, g_pconductor );
				}
			}

			// Remove the existing audiopath - this ensures that the 'shared'
			// buffers are created properly
			g_pconductor->ActivateAudiopath( NULL );

			// Initialize the ListItem pointer to NULL
			g_pconductor->m_pAudiopathListItem = NULL;

			IDirectMusicAudioPath *pAudiopath;
			HRESULT hr = g_pconductor->CreateAudiopath( pAudiopathListItem, &pAudiopath );
			if( SUCCEEDED( hr )
			&&	pAudiopath )
			{
				// Activate the Audio path
				hr = g_pconductor->ActivateAudiopath( pAudiopath );

				// If we succeeded
				if( SUCCEEDED( hr ) )
				{
#ifdef DMP_XBOX
                    g_pconductor->ActivateXboxPath( pAudiopathListItem->dwXboxID );
#endif
					// If we created an Audiopath from the node, set the new audiopath as active
					if( pAudiopathListItem
					&&	pAudiopathListItem->pNode )
					{
						// Try and get the IDMUSProdAudioPathInUse interface for the node
						IDMUSProdAudioPathInUse *pIDMUSProdAudiopathInUse;
						if( SUCCEEDED( pAudiopathListItem->pNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
						{
							// Notify the Audiopath that it now in use
							pIDMUSProdAudiopathInUse->UsingAudioPath( pAudiopath, TRUE );
							pIDMUSProdAudiopathInUse->Release();
						}

						// Add us to the audiopath's notification list
						g_pconductor->m_pFrameWork->AddToNotifyList( pAudiopathListItem->pNode, g_pconductor );
					}

					// Update the Concductor with information about the new audio path
					g_pconductor->m_pAudiopathListItem = pAudiopathListItem;

					// Broadcast the WaveDownload notification
					g_pconductor->BroadCastWaveNotification( GUID_ConductorDownloadWaves );

					// Notify all other components that output has been changed
					// Need to do this even if activation failed, so that the other components
					// can reset their port lists
					g_pconductor->BroadCastPortChangeNotification();
				}

				// Release our pointer to the new audio path
				if( pAudiopath )
				{
					pAudiopath->Release();
				}
			}

			if( FAILED( hr ) )
			{
				// Try and create the old audiopath
				hr = g_pconductor->CreateAudiopath( pOldAudiopathListItem, &pAudiopath );
				if( SUCCEEDED( hr ) )
				{
					// Activate the Audio path
					hr = g_pconductor->ActivateAudiopath( pAudiopath );

					// If we succeeded
					if( SUCCEEDED( hr ) )
					{
						// If we created an Audiopath from the node, set the new audiopath as active
						if( pOldAudiopathListItem
						&&	pOldAudiopathListItem->pNode )
						{
							// Try and get the IDMUSProdAudioPathInUse interface for the node
							IDMUSProdAudioPathInUse *pIDMUSProdAudiopathInUse;
							if( SUCCEEDED( pOldAudiopathListItem->pNode->QueryInterface( IID_IDMUSProdAudioPathInUse, (void **)&pIDMUSProdAudiopathInUse ) ) )
							{
								// Notify the Audiopath that it now in use
								pIDMUSProdAudiopathInUse->UsingAudioPath( pAudiopath, TRUE );
								pIDMUSProdAudiopathInUse->Release();
							}

							// Add us to the audiopath's notification list
							g_pconductor->m_pFrameWork->AddToNotifyList( pOldAudiopathListItem->pNode, g_pconductor );
						}

						// Update the Concductor with information about the new audio path
						g_pconductor->m_pAudiopathListItem = pOldAudiopathListItem;

						// Broadcast the WaveDownload notification
						g_pconductor->BroadCastWaveNotification( GUID_ConductorDownloadWaves );

						// Notify all other components that output has been changed
						// Need to do this even if activation failed, so that the other components
						// can reset their port lists
						g_pconductor->BroadCastPortChangeNotification();
					}

					// Release our pointer to the new audio path
					if( pAudiopath )
					{
						pAudiopath->Release();
					}
				}
			}

			// Update the transport toolbar
			if( g_pconductor->m_pToolbarHandler )
			{
				g_pconductor->m_pToolbarHandler->UpdateActiveFromEngine();
				g_pconductor->m_pToolbarHandler->SetStateAuto();
			}
		}
	}

	// Remove all items from the combo box, then add and select the Audiopath used by the conductor
	UpdateComboBoxFromConductor();

	return TRUE;
}

LRESULT COptionsToolbarHandler::OnDropDownComboAudiopath( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	while( m_comboAudiopath.GetCount() > 0 )
	{
		m_comboAudiopath.DeleteString( 0 );
	}

	// Iterate through all registered audiopaths
	POSITION pos = g_pconductor->m_lstAudiopaths.GetHeadPosition();
	while( pos )
	{
		CAudiopathListItem *pAPListItem = g_pconductor->m_lstAudiopaths.GetNext( pos );

		// Add the text to the combo box
		int nStringIndex = m_comboAudiopath.AddString( pAPListItem->strName );

		// Create a new AudiopathListItem to store information about this Audiopath in
		// Set the combobox item to point to this AudiopathListItem
		m_comboAudiopath.SetItemDataPtr( nStringIndex, pAPListItem );

		// Check if this item is the current Audiopath
		if( pAPListItem == g_pconductor->m_pAudiopathListItem )
		{
			m_comboAudiopath.SetCurSel( nStringIndex );
		}
	}

	// Load the 'No default audiopath' string
	CString strNone;
	if( strNone.LoadString( IDS_NO_AUDIOPATH ) )
	{
		// Add the text to the combo box
		int nStringIndex = m_comboAudiopath.AddString( strNone );

		// Set the combobox item to point to nothing
		m_comboAudiopath.SetItemDataPtr( nStringIndex, NULL );

		// Check if this item is the current Audiopath
		if( NULL == g_pconductor->m_pAudiopathListItem )
		{
			m_comboAudiopath.SetCurSel( nStringIndex );
		}
	}

	return TRUE;
}

LRESULT COptionsToolbarHandler::OnDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& lResult)
{
	UNREFERENCED_PARAMETER(nMsg);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	while( m_comboAudiopath.GetCount() > 0 )
	{
		m_comboAudiopath.DeleteString( 0 );
	}

	lResult = FALSE;
	return TRUE;
}
