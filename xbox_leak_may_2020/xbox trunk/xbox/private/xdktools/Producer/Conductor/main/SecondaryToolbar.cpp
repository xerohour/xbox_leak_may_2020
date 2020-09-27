// ToolbarHandler.cpp : implementation file
//

#include "stdafx.h"
#include <dmusici.h>
#include "CConduct.h"
#include "SecondaryToolbar.h"
#include "DlgSecondaryStart.h"
#include "TREntry.h"
#include <RiffStrm.h>
#include <NodeRefChunk.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Defined in cconduct.cpp
extern int GetIndexByDataPtr( CComboBox &combobox, void *pData );

// Defined in notify.cpp
extern void AddToolsAndSetupWaveSaveForSegState( IUnknown *punk );

CSecondaryToolbarHandler::CSecondaryToolbarHandler()
{
	m_dwNumButtons = 0;

//	m_dwSecondaryToolbarID = 0;
	m_hWndToolbar = NULL;

	// Just fill in some non-zero values - these will be overwritten in Initialize()
	m_cxChar = 4;
	m_cyChar = 4;

	for( int i=0; i< MAX_BUTTONS; i++ )
	{
		m_apOldButtonInfo[i]=NULL;
	}
}

CSecondaryToolbarHandler::~CSecondaryToolbarHandler()
{
	m_font.DeleteObject();

	while( m_arrayButtons.GetSize() )
	{
		delete m_arrayButtons[m_arrayButtons.GetUpperBound()];
		m_arrayButtons.RemoveAt(m_arrayButtons.GetUpperBound());
	}
	for( int i=0; i< MAX_BUTTONS; i++ )
	{
		if( m_apOldButtonInfo[i] )
		{
			delete m_apOldButtonInfo[i];
			m_apOldButtonInfo[i] = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSecondaryToolbarHandler IDMUSProdToolBar::GetInfo

HRESULT CSecondaryToolbarHandler::GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle )
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
	strTitle.LoadString(IDS_SECONDARY_TOOLBAR_TITLE);
	*pbstrTitle = strTitle.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSecondaryToolbarHandler IDMUSProdToolBar::GetMenuText

HRESULT CSecondaryToolbarHandler::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CString strText;
	strText.LoadString(IDS_SECONDARY_TOOLBAR_MENUTEXT);
	*pbstrText = strText.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSecondaryToolbarHandler IDMUSProdToolBar::GetMenuHelpText

HRESULT CSecondaryToolbarHandler::GetMenuHelpText( BSTR* pbstrMenuHelpText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrMenuHelpText == NULL )
	{
		return E_POINTER;
	}

	CString strMenuHelpText;
	strMenuHelpText.LoadString(IDS_SECONDARY_TOOLBAR_HELP);
	*pbstrMenuHelpText = strMenuHelpText.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSecondaryToolbarHandler IDMUSProdToolBar::Initialize

HRESULT CSecondaryToolbarHandler::Initialize( HWND hWndToolBar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// We had better have a valid window handle
	if( hWndToolBar == NULL )
	{
		return E_INVALIDARG;
	}

	// Save the window handle for use later
	m_hWndToolbar = hWndToolBar;
 
	// Create a CToolBarCtrl to modify the toolbar with
	CToolBarCtrl* pToolBarCtrl = new CToolBarCtrl;
	if( pToolBarCtrl == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Attach it to the window handle
	pToolBarCtrl->Attach( hWndToolBar );
	pToolBarCtrl->AddBitmap( 10, IDB_Transport );

	// Create font for toolbar combo box
	CClientDC* pDC = new CClientDC( pToolBarCtrl );

	int nHeight = -( (pDC->GetDeviceCaps(LOGPIXELSY) * 8) / 72 );

	CString strFontName;
	if( strFontName.LoadString( IDS_TOOLBAR_FONT ) == 0)
	{
		strFontName = CString("MS Sans Serif");
	}
	m_font.CreateFont( nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName );
	
	CFont* pOldFont = pDC->SelectObject( &m_font );
	
	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	m_cxChar = 30 * (tm.tmAveCharWidth);
	m_cyChar = 20 * (tm.tmHeight + tm.tmExternalLeading);

	pDC->SelectObject( pOldFont );
	delete pDC;

	// Clean up m_arrayButtons
	while( m_arrayButtons.GetSize() )
	{
		delete m_arrayButtons[m_arrayButtons.GetUpperBound()];
		m_arrayButtons.RemoveAt(m_arrayButtons.GetUpperBound());
	}

	// Initialize TBBUTTON strcture
	TBBUTTON button;
	button.dwData = 0;
	button.iString = NULL;


//	if( m_dwSecondaryToolbarID == 0 )
	{
		/*
		// 1 - Separator
		button.iBitmap = 0;
		button.idCommand = 0;
		button.fsState = TBSTATE_ENABLED;
		button.fsStyle = TBSTYLE_SEP;
		pToolBarCtrl->InsertButton( 0, &button );
		*/

		// 0 - Wide separator for edit/spin control
		button.iBitmap = m_cxChar / 4;
		button.idCommand = IDC_EDIT_SECONDARY;
		button.fsState = TBSTATE_ENABLED;
		button.fsStyle = TBSTYLE_SEP;
		pToolBarCtrl->InsertButton( 0, &button );
	}

	// Now add all the buttons
	SetNumButtons(4);

//	if( m_dwSecondaryToolbarID == 0 )
	{
		// Attach the secondary segment toolbar control window
		RECT rect;
		pToolBarCtrl->GetItemRect( 0, &rect );
		if( !m_editControl.Create(WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_LEFT | ES_WANTRETURN,
			rect, pToolBarCtrl, IDC_EDIT_SECONDARY) )
		{
			return -1;
		}
		m_editControl.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );
		m_editControl.SetFont( &m_font );
		m_editControl.EnableWindow(TRUE);
		m_editControl.SetLimitText(2);

		// Attach the spin control
		if( !m_spinControl.Create(WS_CHILD | WS_VISIBLE |
			UDS_ARROWKEYS | UDS_ALIGNRIGHT | UDS_WRAP | UDS_SETBUDDYINT | UDS_NOTHOUSANDS,
			rect, pToolBarCtrl, IDC_SPIN_SECONDARY) )
		{
			return -1;
		}
		m_spinControl.SetBuddy( &m_editControl );
		m_spinControl.SetRange( 1, MAX_BUTTONS );
		m_spinControl.SetPos( 4 );
		m_spinControl.EnableWindow(TRUE);
	}

	pToolBarCtrl->Detach();
	delete pToolBarCtrl;

	return S_OK;
}

void CSecondaryToolbarHandler::UpdatePlayStopBtns( ButtonInfo *pButtonInfo )
{
	ASSERT( pButtonInfo );
	if( !pButtonInfo )
	{
		return;
	}

	if( !::IsWindow( pButtonInfo->comboActive.GetSafeHwnd() ) )
	{
		return;
	}

	BOOL fHasEntries = FALSE;
	if( !g_pconductor->m_lstISecondaryTransportRegEntry.IsEmpty() )
	{
		fHasEntries = TRUE;
	}

	if( pButtonInfo->pActiveRegEntry == NULL )
	{
		SetBtnState( ID_TRANSP_PLAY1 + pButtonInfo->dwIndex, TBSTATE_INDETERMINATE );
		SetBtnState( ID_TRANSP_STOP1 + pButtonInfo->dwIndex, TBSTATE_INDETERMINATE );
		pButtonInfo->comboActive.EnableWindow( fHasEntries );
	}
	else if( pButtonInfo->fDisplayingPlay )
	{
		if( pButtonInfo->fWaitForStart || (pButtonInfo->pISegmentState && (g_pconductor->m_pDMPerformance->IsPlaying( NULL, pButtonInfo->pISegmentState ) == S_OK)) )
		{
			if( pButtonInfo->fDisplayingPlay )
			{
				//SetBtnState( ID_TRANSP_PLAY1 + pButtonInfo->dwIndex, TBSTATE_CHECKED );
				//SetBtnState( ID_TRANSP_STOP1 + pButtonInfo->dwIndex, TBSTATE_ENABLED );

				// Change Play to Stop button
				TBBUTTONINFO tbButtonInfo;
				tbButtonInfo.cbSize = sizeof( TBBUTTONINFO );
				tbButtonInfo.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE;
				tbButtonInfo.idCommand = ID_TRANSP_STOP1 + pButtonInfo->dwIndex;
				tbButtonInfo.iImage = 9;
				tbButtonInfo.fsState = TBSTATE_ENABLED;
				tbButtonInfo.fsStyle = TBSTYLE_BUTTON;
				//TRACE("Change play1->stop1.\n");
				::SendMessage( m_hWndToolbar, TB_SETBUTTONINFO, ID_TRANSP_PLAY1 + pButtonInfo->dwIndex, LPARAM(&tbButtonInfo) );
				pButtonInfo->comboActive.EnableWindow( FALSE );

				//TRACE("Update m_fDisplayingPlay1\n");
				pButtonInfo->fDisplayingPlay = FALSE;
			}
		}
		else
		{
			SetBtnState( ID_TRANSP_PLAY1 + pButtonInfo->dwIndex, TBSTATE_ENABLED );
		}
	}
	else // !pButtonInfo->fDisplayingPlay
	{
		if( !pButtonInfo->fWaitForStart && (!pButtonInfo->pISegmentState || (g_pconductor->m_pDMPerformance->IsPlaying( NULL, pButtonInfo->pISegmentState ) != S_OK)) )
		{
			if( !pButtonInfo->fDisplayingPlay )
			{
				//SetBtnState( ID_TRANSP_PLAY1 + pButtonInfo->dwIndex, TBSTATE_ENABLED );
				//SetBtnState( ID_TRANSP_STOP1 + pButtonInfo->dwIndex, TBSTATE_INDETERMINATE );

				// Change Stop to Play button
				TBBUTTONINFO tbButtonInfo;
				tbButtonInfo.cbSize = sizeof( TBBUTTONINFO );
				tbButtonInfo.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE;
				tbButtonInfo.idCommand = ID_TRANSP_PLAY1 + pButtonInfo->dwIndex;
				tbButtonInfo.iImage = 8;
				tbButtonInfo.fsState = TBSTATE_ENABLED;
				tbButtonInfo.fsStyle = TBSTYLE_BUTTON;
				//TRACE("Change stop1->play1.\n");
				::SendMessage( m_hWndToolbar, TB_SETBUTTONINFO, ID_TRANSP_STOP1 + pButtonInfo->dwIndex, LPARAM(&tbButtonInfo) );
				pButtonInfo->comboActive.EnableWindow( fHasEntries );

				//TRACE("Update m_fDisplayingPlay1\n");
				pButtonInfo->fDisplayingPlay = TRUE;
			}
		}
		else
		{
			SetBtnState( ID_TRANSP_STOP1 + pButtonInfo->dwIndex, TBSTATE_ENABLED );
		}
	}
}

void CSecondaryToolbarHandler::SetStateAuto( void )
{
	// Verify our window handle is still valid
	if( !::IsWindow( m_hWndToolbar ) )
	{
		return;
	}

	for( DWORD i=0; i<m_dwNumButtons; i++ )
	{
		UpdatePlayStopBtns( m_arrayButtons[i] );
	}
}

BOOL CSecondaryToolbarHandler::SetBtnState( int nID, UINT nState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ::IsWindow( m_hWndToolbar ) )
	{
		return ::SendMessage( m_hWndToolbar, TB_SETSTATE, nID, MAKELONG(nState, 0) );
	}

	return FALSE;
}

CWndClassInfo& CSecondaryToolbarHandler::GetWndClassInfo()
{
	static CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, StartWindowProc,
		  0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW+1), 0, NULL, 0 },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}

LRESULT CSecondaryToolbarHandler::OnRightClick(WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled)
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(wNotifyCode);
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

		for( DWORD i=0; i< m_dwNumButtons; i++ )
		{
			// Get rect defining boundary of Play/Stop button
//			if( m_dwSecondaryToolbarID == 0 )
			{
				::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 4 + i * 4, (LPARAM) &rect );
			}
			/*
			else
			{
				::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 3 + i * 4, (LPARAM) &rect );
			}
			*/

			if( rect.PtInRect( ptClient ) )
			{
				DlgSecondaryStart dlg;
				dlg.m_dwSegmentFlags = m_arrayButtons[i]->dwSegFlags;;

				if( IDOK == dlg.DoModal() )
				{
					m_arrayButtons[i]->dwSegFlags = dlg.m_dwSegmentFlags;
				}
				break;
			}
		}
	}

	return TRUE;
}


void CSecondaryToolbarHandler::OnSegEndNotify( IUnknown *punkSegment )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//TRACE("OnSegEndNotify %x\n", punkSegment);

	IDirectMusicSegmentState *pSegmentState = NULL;
	if( punkSegment )
	{
		punkSegment->QueryInterface( IID_IDirectMusicSegmentState, (void **)&pSegmentState );
	}

	for( DWORD i=0; i < m_dwNumButtons; i++ )
	{
		if( pSegmentState && m_arrayButtons[i]->pISegmentState &&
			(pSegmentState == m_arrayButtons[i]->pISegmentState) )
		{
			RELEASE( m_arrayButtons[i]->pISegmentState );
			m_arrayButtons[i]->fWaitForStart = FALSE;
		}
		if( !m_arrayButtons[i]->fWaitForStart && m_arrayButtons[i]->pISegmentState &&
			(g_pconductor->m_pDMPerformance->IsPlaying( NULL, m_arrayButtons[i]->pISegmentState ) != S_OK) )
		{
			RELEASE( m_arrayButtons[i]->pISegmentState );
		}
	}

	if( pSegmentState )
	{
		pSegmentState->Release();
	}
}

void CSecondaryToolbarHandler::OnSegStartNotify( IUnknown *punkSegment )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//TRACE("OnSegStartNotify %x\n", punkSegment);

	IDirectMusicSegmentState *pSegmentState = NULL;
	if( punkSegment )
	{
		punkSegment->QueryInterface( IID_IDirectMusicSegmentState, (void **)&pSegmentState );
	}

	for( DWORD i=0; i < m_dwNumButtons; i++ )
	{
		if( m_arrayButtons[i]->fWaitForStart )
		{
#ifdef _DEBUG
			if( !m_arrayButtons[i]->pISegmentState )
			{
				TRACE("Conductor: Secondary Segment %d doesn't exist when receiving notification.\n", i);
			}
#endif
			if( pSegmentState && (pSegmentState == m_arrayButtons[i]->pISegmentState) )
			{
				m_arrayButtons[i]->fWaitForStart = FALSE;
			}
			else if( !m_arrayButtons[i]->pISegmentState ||
				(g_pconductor->m_pDMPerformance->IsPlaying( NULL, m_arrayButtons[i]->pISegmentState ) == S_OK) )
			{
				m_arrayButtons[i]->fWaitForStart = FALSE;
			}
		}
	}

	if( pSegmentState )
	{
		pSegmentState->Release();
	}
}

LRESULT CSecondaryToolbarHandler::OnSetStateAuto( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(wCode);
	UNREFERENCED_PARAMETER(wLoWord);
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(bHandled);

	SetStateAuto();
	return TRUE;
}

HRESULT CSecondaryToolbarHandler::Save( IDMUSProdRIFFStream* pIRiffStream )
{
	ASSERT( pIRiffStream );
	if( !pIRiffStream )
	{
		return E_POINTER;
	}

	IStream *pIStream = pIRiffStream->GetStream();

	for( DWORD i=0; i < m_dwNumButtons; i++ )
	{
		// Create the toolbar state chunk
		MMCKINFO ckToolBar;
		ckToolBar.fccType = FOURCC_2NDARY_TOOLBAR_CHUNK;
		pIRiffStream->CreateChunk( &ckToolBar, MMIO_CREATELIST );

		// Write out the button's flags
		MMCKINFO ckFlags;
		ckFlags.ckid = FOURCC_2NDARY_TOOLBAR_FLAGS;
		pIRiffStream->CreateChunk( &ckFlags, 0 );
		pIStream->Write( &m_arrayButtons[i]->dwSegFlags, sizeof(DWORD), NULL );
		pIRiffStream->Ascend( &ckFlags, 0 );

		if( m_arrayButtons[i]->pActiveRegEntry )
		{
			IDMUSProdSecondaryTransport*  pISecondaryTransport;
			m_arrayButtons[i]->pActiveRegEntry->GetSecondaryTransport( &pISecondaryTransport );

			IDMUSProdNode *pNode;
			if( SUCCEEDED( pISecondaryTransport->QueryInterface( IID_IDMUSProdNode, (void **)&pNode ) ) )
			{

				CNodeRefChunk NodeRef( g_pconductor->m_pFrameWork, pNode );
				if( FAILED( NodeRef.Save( pIStream ) ) )
				{
					TRACE("CSecondaryToolbarHandler::Save unable to save node ref chunk.\n");
				}

				pNode->Release();
			}
		}
		else
		{
			// Create an empty chunk if nothing is selected
		}
		pIRiffStream->Ascend( &ckToolBar, 0 );
	}

	pIStream->Release();
	return S_OK;
}


HRESULT CSecondaryToolbarHandler::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO *pckMain )
{
	ASSERT( pckMain );
	ASSERT( pIRiffStream );
	if( !pckMain || !pIRiffStream )
	{
		return E_POINTER;
	}

	// Get a pointer to the stream
	IStream *pIStream = pIRiffStream->GetStream();

	HRESULT hr;
	DWORD cbRead;

	for( DWORD i=0; i < m_dwNumButtons; i++ )
	{
		// Descend into the toolbar chunk
		MMCKINFO ckSubChunk;
		if( pIRiffStream->Descend( &ckSubChunk, pckMain, 0 ) != 0 )
		{
			pIStream->Release();
			return E_FAIL;
		}

		// Load combo box
		if( (ckSubChunk.ckid == FOURCC_LIST) && (ckSubChunk.fccType == FOURCC_2NDARY_TOOLBAR_CHUNK) )
		{
			if( !m_arrayButtons[i]->fWaitForStart &&
			   (!m_arrayButtons[i]->pISegmentState || (g_pconductor->m_pDMPerformance->IsPlaying( NULL, m_arrayButtons[i]->pISegmentState ) != S_OK)) )
			{
				// Read in the SegFlags
				MMCKINFO ckFlags;
				if( pIRiffStream->Descend( &ckFlags, &ckSubChunk, 0 ) != 0 )
				{
					pIStream->Release();
					return E_FAIL;
				}
				if( ckFlags.ckid == FOURCC_2NDARY_TOOLBAR_FLAGS )
				{
					hr = pIStream->Read( &m_arrayButtons[i]->dwSegFlags, sizeof(DWORD), &cbRead );
					if( FAILED(hr) || (cbRead != sizeof(DWORD)) )
					{
						pIStream->Release();
						return E_FAIL;
					}
					pIRiffStream->Ascend( &ckFlags, 0 );

					// If empty, clear the transport combo box
					if( ckSubChunk.cksize == 3 * sizeof(DWORD) + ckFlags.cksize)
					{
						SetActiveTransport( m_arrayButtons[i]->comboActive, NULL, m_arrayButtons[i]->pActiveRegEntry );
					}
					else
					{
						// Initialize the CNodeRef
						CNodeRefChunk NodeRef( g_pconductor->m_pFrameWork, NULL );

						// Load the ref chunk
						if( SUCCEEDED( NodeRef.Load( pIStream ) ) && NodeRef.m_pINode )
						{
							IDMUSProdSecondaryTransport*  pISecondaryTransport;
							if( SUCCEEDED( NodeRef.m_pINode->QueryInterface( IID_IDMUSProdSecondaryTransport, (void**)&pISecondaryTransport ) ) )
							{
								// Set active to pISecondaryTransport
								SetActiveTransport( m_arrayButtons[i]->comboActive, pISecondaryTransport, m_arrayButtons[i]->pActiveRegEntry );

								pISecondaryTransport->Release();
							}
						}
					}
				}
			}
		}

		pIRiffStream->Ascend( &ckSubChunk, 0 );
	}

	// Release the stream pointer
	pIStream->Release();
	return S_OK;
}

void CSecondaryToolbarHandler::SetActiveTransport( CComboBox &comboActive, IDMUSProdSecondaryTransport* pISecondaryTransport, ISecondaryTransportRegEntry *& pActiveRegEntry )
{
	if( pISecondaryTransport == NULL )
	{
		comboActive.SetCurSel( -1 );
		pActiveRegEntry = NULL;

		SetStateAuto();
		return;
	}

	for( int i=0; i < comboActive.GetCount(); i++ )
	{
		ISecondaryTransportRegEntry *pThisRegEntry = static_cast<ISecondaryTransportRegEntry *> (comboActive.GetItemDataPtr( i ));

		ASSERT( pThisRegEntry );
		if( pThisRegEntry )
		{
			IDMUSProdSecondaryTransport*  pITmpSecondaryTransport;
			pThisRegEntry->GetSecondaryTransport( &pITmpSecondaryTransport );

			if( pITmpSecondaryTransport == pISecondaryTransport )
			{
				comboActive.SetCurSel( i );
				pActiveRegEntry = pThisRegEntry;

				SetStateAuto();
				return;
			}
		}
	}

	return;
}

LRESULT CSecondaryToolbarHandler::OnPlayClicked( WORD wID )
{
	// This should have already been checked by the calling method
	ASSERT( wID < m_dwNumButtons );

	// Make sure the play button is currently displayed
	if( !m_arrayButtons[wID]->fDisplayingPlay )
	{
		return 0;
	}

	// Dispatch the play message to the active transport.
	if( m_arrayButtons[wID]->pActiveRegEntry )
	{
		// Force KillFocus to give controls a chance to sync changes handled in KillFocus code
		CWnd* pWndHadFocus = CWnd::GetFocus();
		::SetFocus( NULL );
		if( pWndHadFocus )
		{
			pWndHadFocus->SetFocus();
		}

		IDMUSProdSecondaryTransport*  pISecondaryTransport;
		m_arrayButtons[wID]->pActiveRegEntry->GetSecondaryTransport( &pISecondaryTransport );

		IUnknown *pIUnknown = NULL;
		if( FAILED(pISecondaryTransport->GetSecondaryTransportSegment( &pIUnknown )))
		{
			// Leave the buttons in their current state.
			return FALSE;
		}

		IDirectMusicSegment* pIDMSegment;
		if( FAILED( pIUnknown->QueryInterface( IID_IDirectMusicSegment, (void**)&pIDMSegment ) ) )
		{
			// This must be a song
			pIDMSegment = NULL;
		}

		// Fix for 22616: Secondary segment starts playing from 
		// the primary segment's start time...
		// Get the start point for the segment
		MUSIC_TIME mtPrimarySegmentStartTime = 0;
		if( pIDMSegment )
		{
			pIDMSegment->GetStartPoint(&mtPrimarySegmentStartTime);
			pIDMSegment->SetStartPoint(0);
		}

		// CONSIDER: Add flags for Queue time and Prepare time (or leave them to be picked up by segf_default?)
		DWORD dwFlags = m_arrayButtons[wID]->dwSegFlags | DMUS_SEGF_SECONDARY;
		if( FAILED( g_pconductor->m_pDMPerformance->PlaySegmentEx( pIUnknown,
																   NULL,
																   NULL,
																   dwFlags,
																   0,
																   &m_arrayButtons[wID]->pISegmentState,
																   NULL,
																   NULL ) ) )
		{
			// Leave the buttons in their current state.
			RELEASE( pIDMSegment );
			RELEASE( pIUnknown );
			return FALSE;
		}
		
		// Set the original start point back for the primary segment
		if( pIDMSegment )
		{
			pIDMSegment->SetStartPoint(mtPrimarySegmentStartTime);
		}

		//TRACE("OnPlayClicked %x\n", m_arrayButtons[wID]->pISegmentState);

		RELEASE( pIDMSegment );
		RELEASE( pIUnknown );

		// This just calls AddToolsAndSetupWaveSaveForSegState
		//g_pconductor->SetupMIDIAndWaveSave( m_arrayButtons[wID]->pISegmentState );

		AddToolsAndSetupWaveSaveForSegState( m_arrayButtons[wID]->pISegmentState );

		m_arrayButtons[wID]->fWaitForStart = TRUE;
		SetStateAuto();
		g_pconductor->SetMIDIExportFields( (IUnknown *)pISecondaryTransport );
		return TRUE;
	}
	else
	{
		// Leave the buttons in their current state.
	}

	return FALSE;
}

LRESULT CSecondaryToolbarHandler::OnStopClicked( WORD wID )
{
	// This should have already been checked by the calling method
	ASSERT( wID < m_dwNumButtons );

	// Make sure the play button is not currently displayed
	if( m_arrayButtons[wID]->fDisplayingPlay )
	{
		return 0;
	}

	if( m_arrayButtons[wID]->pISegmentState )
	{
		// Stop pISegmentState after prepare time
		MUSIC_TIME mtStopTime = 0;
		if( m_arrayButtons[wID]->dwSegFlags & DMUS_SEGF_CONTROL )
		{
			// Stop after prepare time to ensure no invalidations occur
			REFERENCE_TIME rtStopTime;
			g_pconductor->m_pDMPerformance->GetTime( &rtStopTime, &mtStopTime );
			//TRACE("Stop: Now = %I64d %d ", rtStopTime, mtStopTime );
			g_pconductor->m_pDMPerformance->GetResolvedTime( 0, &rtStopTime, DMUS_TIME_RESOLVE_AFTERPREPARETIME );
			rtStopTime += 100000;
			g_pconductor->m_pDMPerformance->ReferenceToMusicTime( rtStopTime, &mtStopTime );
			//TRACE(" Stop = %I64d ", rtStopTime );
		}

		//TRACE("Stopping %x at %d with %x\n", m_arrayButtons[wID]->pISegmentState, mtStopTime, m_arrayButtons[wID]->dwSegFlags );
		if( SUCCEEDED( g_pconductor->m_pDMPerformance->Stop( NULL, m_arrayButtons[wID]->pISegmentState, mtStopTime, m_arrayButtons[wID]->dwSegFlags ) ) )
		{
			RELEASE( m_arrayButtons[wID]->pISegmentState );
			m_arrayButtons[wID]->fWaitForStart = FALSE;
			SetStateAuto();
			return TRUE;
		}
		else
		{
			TRACE("Unable to stop secondary segment %hd\n", wID);
			ASSERT(FALSE);
		}
	}

	SetStateAuto();
	return FALSE;
}

LRESULT CSecondaryToolbarHandler::OnButtonClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wNotifyCode);
	if( (wID >= ID_TRANSP_PLAY1) && (wID < ID_TRANSP_STOP1) )
	{
		wID -= ID_TRANSP_PLAY1;
		//TRACE("Play %d\n", wID);

		ASSERT( wID < m_dwNumButtons );
		if( wID < m_dwNumButtons )
		{
			return OnPlayClicked( wID );
		}
	}
	else if( (wID >= ID_TRANSP_STOP1) && (wID < IDC_TRANSP_COMBO1) )
	{
		wID -= ID_TRANSP_STOP1;
		//TRACE("Stop %d\n", wID);

		ASSERT( wID < m_dwNumButtons );
		if( wID < m_dwNumButtons )
		{
			return OnStopClicked( wID );
		}
	}
	else
	{
		/* No need - handled by OnChangeSecondaryEdit
		// If user pressed 'Enter' in the Secondary segment # edit box, treat it as a killfocus
		if( (wNotifyCode == 0) && (wID == 1) )
		{
			return OnKillFocusSecondaryEdit( wNotifyCode, wID, hWndCtl, bHandled );
		}
		*/
		//ASSERT(FALSE);
	}
	return 0;
}

LRESULT CSecondaryToolbarHandler::OnSelchangeCombo( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wNotifyCode);
	if( (wID >= IDC_TRANSP_COMBO1) && (wID < IDC_TRANSP_COMBO1 + 100) )
	{
		wID -= IDC_TRANSP_COMBO1;
		//TRACE("Combo %d\n", wID);

		ASSERT( wID < m_dwNumButtons );
		if( wID < m_dwNumButtons )
		{
			if ( m_arrayButtons[wID]->comboActive.GetCurSel() != CB_ERR )
			{
				int nIndex = m_arrayButtons[wID]->comboActive.GetCurSel();
				m_arrayButtons[wID]->pActiveRegEntry = static_cast<ISecondaryTransportRegEntry *> (m_arrayButtons[wID]->comboActive.GetItemDataPtr(nIndex));
				SetStateAuto();
			}
		}
	}
	else
	{
		//ASSERT(FALSE);
	}
	return 0;
}

void CSecondaryToolbarHandler::AddButton( DWORD dwIndex )
{
	// Initialize TBBUTTON strcture
	TBBUTTON button;
	button.dwData = 0;
	button.iString = NULL;

	int nPositionToInsert;
//	if( m_dwSecondaryToolbarID == 0 )
	{
		nPositionToInsert = 1 + dwIndex * 4;
	}
	/*
	else
	{
		nPositionToInsert = dwIndex * 4;
	}
	*/

	// nPositionToInsert + 3 - Play/Stop
	button.iBitmap = 8;
	button.idCommand = ID_TRANSP_PLAY1 + dwIndex;
	button.fsState = TBSTATE_INDETERMINATE;
	button.fsStyle = TBSTYLE_BUTTON;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, nPositionToInsert, (LPARAM)&button );

	// nPositionToInsert + 2 - Wide separator for active transport indicator
	button.iBitmap = m_cxChar;
	button.idCommand = IDC_TRANSP_COMBO1 + dwIndex;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, nPositionToInsert, (LPARAM)&button );

	// nPositionToInsert + 1 - Identifier
	button.iBitmap = (2 * m_cxChar)/15;
	button.idCommand = ID_TRANSP_IDEN1 + dwIndex;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, nPositionToInsert, (LPARAM)&button );

	// nPositionToInsert - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = 0;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, nPositionToInsert, (LPARAM)&button );

	// Attach the combo box window
	RECT rect;
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, nPositionToInsert + 2, (LPARAM)&rect );
	rect.bottom = rect.top + m_cyChar;
	if( !m_arrayButtons[dwIndex]->comboActive.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_SORT,
		rect, CWnd::FromHandle( m_hWndToolbar ), IDC_TRANSP_COMBO1 + dwIndex) )
	{
		ASSERT(FALSE);
		return;
	}
	m_arrayButtons[dwIndex]->comboActive.SetFont( &m_font );

	// Attach the static text window
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, nPositionToInsert + 1, (LPARAM)&rect );

	TCHAR tcstrName[10];
	if( dwIndex < 9 )
	{
		tcstrName[0] = '&';
		tcstrName[1] = '1' + dwIndex;
		tcstrName[2] = 0;
	}
	else if( dwIndex == 9 )
	{
		tcstrName[0] = '1';
		tcstrName[1] = '&';
		tcstrName[2] = '0';
		tcstrName[3] = 0;
	}
	else if( dwIndex < 36 )
	{
		tcstrName[0] = '&';
		tcstrName[1] = 'A' + dwIndex - 10;
		tcstrName[2] = 0;
	}
	else
	{
		_itot( dwIndex + 1, tcstrName, 10 );
	}
	if( !m_arrayButtons[dwIndex]->staticIdent.Create(tcstrName, WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER,
		rect, CWnd::FromHandle( m_hWndToolbar ), ID_TRANSP_IDEN1 + dwIndex) )
	{
		ASSERT(FALSE);
		return;
	}
	m_arrayButtons[dwIndex]->staticIdent.SetFont( &m_font );

	// Populate the combobox if we're not creating the first button
	if( dwIndex != 0 )
	{
		CString strText;
		for( int i=0; i < m_arrayButtons[0]->comboActive.GetCount(); i++ )
		{
			m_arrayButtons[0]->comboActive.GetLBText( i, strText );
			int nIndex = m_arrayButtons[dwIndex]->comboActive.InsertString( i, strText );
			ASSERT(nIndex == i);
			void *pData = m_arrayButtons[0]->comboActive.GetItemDataPtr( i );
			m_arrayButtons[dwIndex]->comboActive.SetItemDataPtr( nIndex, pData );
		}
	}

	// Set the combobox to the correct item
	m_arrayButtons[dwIndex]->comboActive.SetCurSel( GetIndexByDataPtr( m_arrayButtons[dwIndex]->comboActive, m_arrayButtons[dwIndex]->pActiveRegEntry ) );

	m_arrayButtons[dwIndex]->comboActive.EnableWindow(
		!g_pconductor->m_lstISecondaryTransportRegEntry.IsEmpty() );
}

void CSecondaryToolbarHandler::RemoveButton( DWORD dwIndex )
{
	ASSERT( dwIndex < (unsigned)m_arrayButtons.GetSize() );

	int nPositionToDelete;
//	if( m_dwSecondaryToolbarID == 0 )
	{
		nPositionToDelete = 1 + dwIndex * 4;
	}
	/*
	else
	{
		nPositionToDelete = dwIndex * 4;
	}
	*/

	m_arrayButtons[dwIndex]->comboActive.DestroyWindow();
	m_arrayButtons[dwIndex]->staticIdent.DestroyWindow();

	// Remove the Separator
	::SendMessage( m_hWndToolbar, TB_DELETEBUTTON, nPositionToDelete, 0 );
	// Remove the static text Separator
	::SendMessage( m_hWndToolbar, TB_DELETEBUTTON, nPositionToDelete, 0 );
	// Remove the combo box Separator
	::SendMessage( m_hWndToolbar, TB_DELETEBUTTON, nPositionToDelete, 0 );
	// Remove the play/stop button
	::SendMessage( m_hWndToolbar, TB_DELETEBUTTON, nPositionToDelete, 0 );
}

void CSecondaryToolbarHandler::SetNumButtons( DWORD dwNumButtons )
{
	if( dwNumButtons > m_dwNumButtons )
	{
		for( DWORD i=m_dwNumButtons; i < dwNumButtons; i++ )
		{
			// Create the button
			ButtonInfo *pButtonInfo = new ButtonInfo();
			pButtonInfo->dwIndex = i;

			// Copy saved information, if there is any
			if( m_apOldButtonInfo[i] )
			{
				memcpy( (ButtonState*) pButtonInfo, m_apOldButtonInfo[i], sizeof(ButtonState) );
			}

			// Add the new button to the array
			m_arrayButtons.SetAtGrow( i, pButtonInfo );

			// Add the button to the toolbar
			AddButton( i );
		}
		m_dwNumButtons = dwNumButtons;
	}
	else if( dwNumButtons < m_dwNumButtons )
	{
		for( DWORD i=m_dwNumButtons - 1; i >= dwNumButtons; i-- )
		{
			// Remove the button from the display
			RemoveButton(i);

			// Create a ButttonState to save information, if necessary
			if( !m_apOldButtonInfo[i] )
			{
				m_apOldButtonInfo[i] = new ButtonInfo();
			}

			// Save state information in case we recreate this button
			memcpy( m_apOldButtonInfo[i], (ButtonState *)m_arrayButtons[i], sizeof(ButtonState) );

			// Fake a click on the Stop button so we stop this transport if it's currently playing
			BOOL bHandled;
			OnButtonClicked( 0, ID_TRANSP_STOP1 + i, 0, bHandled );
			if( m_arrayButtons[i]->pISegmentState )
			{
				TRACE("SetNumButtons: Transport #%d failed to Stop() when removed\n", i);
				RELEASE( m_arrayButtons[i]->pISegmentState );
			}

			// Delete the button
			delete m_arrayButtons[i];
			m_arrayButtons.RemoveAt(i);
			m_dwNumButtons--;
		}
		ASSERT( m_dwNumButtons == dwNumButtons );
	}

	::SendMessage( m_hWndToolbar, TB_AUTOSIZE, 0, 0 );
	// Send a WM_SIZE message to the Frame window (Producer's main window) to
	// cause it to recalculate the toolbar layout.
	RECT rect;
	::GetWindowRect( ::GetParent(::GetParent(m_hWndToolbar)), &rect );
	::SendMessage( ::GetParent(::GetParent(m_hWndToolbar)), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rect.right - rect.left, rect.bottom-rect.top ) );

	SetStateAuto();
}

/* No need - handled by OnChangeSecondaryEdit
LRESULT CSecondaryToolbarHandler::OnKillFocusSecondaryEdit( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_editControl.GetSafeHwnd() != NULL)
	{
		CString strNewNumButtons;
		m_editControl.GetWindowText( strNewNumButtons );
		strNewNumButtons.TrimRight();
		strNewNumButtons.TrimLeft();

		if( strNewNumButtons.IsEmpty() )
		{
			m_spinControl.SetPos( m_dwNumButtons );
		}
		else
		{
			DWORD dwNumButtons;
			dwNumButtons = _tcstoul(strNewNumButtons, NULL, 10);
			if( dwNumButtons > MAX_BUTTONS )
			{
				dwNumButtons = MAX_BUTTONS;
				m_spinControl.SetPos( dwNumButtons );
			}
			else if( dwNumButtons < 1 )
			{
				dwNumButtons = 1;
				m_spinControl.SetPos( dwNumButtons );
			}
			if (dwNumButtons != m_dwNumButtons )
			{
				SetNumButtons( dwNumButtons );
			}
		}
	}
	return TRUE;
}
*/

LRESULT CSecondaryToolbarHandler::OnChangeSecondaryEdit( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_editControl.GetSafeHwnd() != NULL)
	{
		CString strNewNumButtons;
		m_editControl.GetWindowText( strNewNumButtons );
		strNewNumButtons.TrimRight();
		strNewNumButtons.TrimLeft();

		if( !strNewNumButtons.IsEmpty() )
		{
			DWORD dwNumButtons;
			dwNumButtons = _tcstoul(strNewNumButtons, NULL, 10);
			if( dwNumButtons > MAX_BUTTONS )
			{
				dwNumButtons = MAX_BUTTONS;
				m_spinControl.SetPos( dwNumButtons );
			}
			else if( dwNumButtons < 1 )
			{
				dwNumButtons = 1;
				m_spinControl.SetPos( dwNumButtons );
			}
			if (dwNumButtons != m_dwNumButtons )
			{
				SetNumButtons( dwNumButtons );
			}
		}
		// We don't do anything if it is empty
	}
	return TRUE;
}

LRESULT CSecondaryToolbarHandler::OnDeltaPosSecondary( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_spinControl.GetSafeHwnd() != NULL)
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		CString strNewNumButtons;
		m_editControl.GetWindowText( strNewNumButtons );
		strNewNumButtons.TrimRight();
		strNewNumButtons.TrimLeft();

		long lNumButtons;
		lNumButtons = _tcstol(strNewNumButtons, NULL, 10);

		lNumButtons += pNMUpDown->iDelta;
		if( lNumButtons < 1 )
		{
			lNumButtons = 1;
		}
		else if( lNumButtons > MAX_BUTTONS )
		{
			lNumButtons = MAX_BUTTONS;
		}

		if( lNumButtons != pNMUpDown->iPos )
		{
			m_spinControl.SetPos( lNumButtons );
		}
		if ((unsigned)lNumButtons != m_dwNumButtons )
		{
			SetNumButtons( lNumButtons );
		}
	}
	return TRUE;
}

void CSecondaryToolbarHandler::Activate( BOOL fActivate )
{
	for( DWORD i=0; i < m_dwNumButtons; i++ )
	{
		if( m_arrayButtons[i]->pISegmentState )
		{
			IDirectMusicSegmentState8 *pIDirectMusicSegmentState8;
			if( SUCCEEDED( m_arrayButtons[i]->pISegmentState->QueryInterface( IID_IDirectMusicSegmentState8, (void **)&pIDirectMusicSegmentState8 ) ) )
			{
				IDirectMusicAudioPath *pIDirectMusicAudioPath;
				if( SUCCEEDED( pIDirectMusicSegmentState8->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0, GUID_All_Objects, 0, IID_IDirectMusicAudioPath, (void **)&pIDirectMusicAudioPath ) ) )
				{
					pIDirectMusicAudioPath->Activate( fActivate );
					pIDirectMusicAudioPath->Release();
				}
				pIDirectMusicSegmentState8->Release();
			}
		}
	}
}
