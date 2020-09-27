// ToolbarHandler.cpp : implementation file
//

#include "stdafx.h"
#include "CConduct.h"
#include <math.h>
#include "Toolbar.h"
#include "OutputTool.h"
#include "DlgTransition.h"
#include "DlgAltTempo.h"
#include "TREntry.h"
#include <dmusicf.h>
#include "OptionsToolbar.h"
#include "SecondaryToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void QueueSegmentState( IDirectMusicPerformance *pPerformance, MUSIC_TIME mtNow );

CToolbarHandler::CToolbarHandler()
{
	m_fInOnBarButtonClicked = FALSE;
	m_fPlayFromStart = FALSE;
	m_fStopImmediate = FALSE;
	m_fDontUpdateRewPlay = FALSE;
	m_fInTransition = FALSE;
	m_fDisplayingPlay = FALSE;
	m_mtPlayTime = 0;
	m_pCountInSegmentState = NULL;
}

CToolbarHandler::~CToolbarHandler()
{
	m_font.DeleteObject();
	if( m_pCountInSegmentState )
	{
		m_pCountInSegmentState->Release();
		m_pCountInSegmentState = NULL;
	}
}

BOOL CToolbarHandler::SetBtnState( int nID, UINT nState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ::IsWindow( m_hWndToolbar ) )
	{
#ifdef _DEBUG
		if( (nID == ID_TRANSP_PLAY) && (nState == TBSTATE_ENABLED) )
		{
			ASSERT(FALSE);
		}
		else if( (nID == ID_TRANSP_STOP) && (nState == TBSTATE_ENABLED) )
		{
			ASSERT(FALSE);
		}
		else
#endif
		{
			return ::SendMessage( m_hWndToolbar, TB_SETSTATE, nID, MAKELONG(nState, 0) );
		}
	}

	return FALSE;
}

BOOL CToolbarHandler::SetState( DWORD dwFlags )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ::IsWindow( m_hWndToolbar ) )
	{
		if( m_fDisplayingPlay )
		{
			if( (dwFlags & B_STOP_ENABLED) && !(dwFlags & B_STOP_CHECKED) )
			{
				// Change Play to Stop button
				TBBUTTONINFO buttonInfo;
				buttonInfo.cbSize = sizeof( TBBUTTONINFO );
				buttonInfo.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE;
				buttonInfo.idCommand = ID_TRANSP_STOP;
				buttonInfo.iImage = 9;
				buttonInfo.fsState = TBSTATE_ENABLED;
				buttonInfo.fsStyle = TBSTYLE_BUTTON;
				buttonInfo.cx = 0;
				buttonInfo.lParam = 0;
				buttonInfo.pszText = 0;
				buttonInfo.cchText = 0;
				//TRACE("Change play->stop.\n");
				::SendMessage( m_hWndToolbar, TB_SETBUTTONINFO, ID_TRANSP_PLAY, LPARAM(&buttonInfo) );

				buttonInfo.cbSize = sizeof( TBBUTTONINFO );
				buttonInfo.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE;
				buttonInfo.idCommand = ID_TRANS_STOPIMMEDIATE;
				buttonInfo.iImage = 1;
				buttonInfo.fsState = TBSTATE_ENABLED;
				buttonInfo.fsStyle = TBSTYLE_BUTTON;
				buttonInfo.cx = 0;
				buttonInfo.lParam = 0;
				buttonInfo.pszText = 0;
				buttonInfo.cchText = 0;
				//TRACE("Change play->stop.\n");
				::SendMessage( m_hWndToolbar, TB_SETBUTTONINFO, ID_TRANS_REWPLAY, LPARAM(&buttonInfo) );

				//TRACE("Update m_fDisplayingPlay\n");
				m_fDisplayingPlay = FALSE;
			}
			else
			{
				// Play button
				if (dwFlags & B_PLAY_ENABLED)
				{
					::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_PLAY, MAKELONG(TRUE, 0) );
					::SendMessage( m_hWndToolbar, TB_INDETERMINATE, ID_TRANSP_PLAY, MAKELONG(FALSE, 0) );
					if ( !m_fDontUpdateRewPlay )
					{
						::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANS_REWPLAY, MAKELONG(TRUE, 0) );
						::SendMessage( m_hWndToolbar, TB_INDETERMINATE, ID_TRANS_REWPLAY, MAKELONG(FALSE, 0) );
					}
				}
				else
				{
					::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_PLAY, MAKELONG(FALSE, 0) );
					if ( !m_fDontUpdateRewPlay )
					{
						::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANS_REWPLAY, MAKELONG(FALSE, 0) );
					}
				}
				if (dwFlags & B_PLAY_CHECKED)
				{
					::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_PLAY, MAKELONG(TRUE, 0) );
				}
				else
				{
					::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_PLAY, MAKELONG(FALSE, 0) );
				}
			}
		}
		else
		{
			if( (dwFlags & B_PLAY_ENABLED) && !(dwFlags & B_PLAY_CHECKED) )
			{
				// Change Stop to Play button
				TBBUTTONINFO buttonInfo;
				buttonInfo.cbSize = sizeof( TBBUTTONINFO );
				buttonInfo.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE;
				buttonInfo.idCommand = ID_TRANSP_PLAY;
				buttonInfo.iImage = 0;
				buttonInfo.fsState = TBSTATE_ENABLED;
				buttonInfo.fsStyle = TBSTYLE_BUTTON;
				buttonInfo.cx = 0;
				buttonInfo.lParam = 0;
				buttonInfo.pszText = 0;
				buttonInfo.cchText = 0;
				//TRACE("Change stop->play.\n");
				::SendMessage( m_hWndToolbar, TB_SETBUTTONINFO, ID_TRANSP_STOP, LPARAM(&buttonInfo) );

				buttonInfo.cbSize = sizeof( TBBUTTONINFO );
				buttonInfo.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE;
				buttonInfo.idCommand = ID_TRANS_REWPLAY;
				buttonInfo.iImage = 8;
				buttonInfo.fsState = TBSTATE_ENABLED;
				buttonInfo.fsStyle = TBSTYLE_BUTTON;
				buttonInfo.cx = 0;
				buttonInfo.lParam = 0;
				buttonInfo.pszText = 0;
				buttonInfo.cchText = 0;
				//TRACE("Change stop->play.\n");
				::SendMessage( m_hWndToolbar, TB_SETBUTTONINFO, ID_TRANS_STOPIMMEDIATE, LPARAM(&buttonInfo) );

				//TRACE("Update m_fDisplayingPlay\n");
				m_fDisplayingPlay = TRUE;
			}
			else
			{
				// Stop button
				if (dwFlags & B_STOP_ENABLED)
				{
					::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_STOP, MAKELONG(TRUE, 0) );
					::SendMessage( m_hWndToolbar, TB_INDETERMINATE, ID_TRANSP_STOP, MAKELONG(FALSE, 0) );
					if ( !m_fDontUpdateRewPlay )
					{
						::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANS_STOPIMMEDIATE, MAKELONG(TRUE, 0) );
						::SendMessage( m_hWndToolbar, TB_INDETERMINATE, ID_TRANS_STOPIMMEDIATE, MAKELONG(FALSE, 0) );
					}
				}
				else
				{
					::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_STOP, MAKELONG(FALSE, 0) );
					if ( !m_fDontUpdateRewPlay )
					{
						::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANS_STOPIMMEDIATE, MAKELONG(FALSE, 0) );
					}
				}
				if (dwFlags & B_STOP_CHECKED)
				{
					::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_STOP, MAKELONG(TRUE, 0) );
				}
				else
				{
					::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_STOP, MAKELONG(FALSE, 0) );
				}
			}
		}

		// Play from start button
		/*
		if( !m_fDontUpdateRewPlay )
		{
			if ((dwFlags & B_PLAY_ENABLED) || (dwFlags & B_STOP_ENABLED))
			{
				::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANS_STOPIMMEDIATE, MAKELONG(FALSE, 0) );
				m_buttonPlayFromStart.EnableWindow( TRUE );
			}
			else
			{
				::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANS_STOPIMMEDIATE, MAKELONG(FALSE, 0) );
				m_buttonPlayFromStart.EnableWindow( FALSE );
			}
			if ((dwFlags & B_PLAY_CHECKED) || (dwFlags & B_STOP_CHECKED))
			{
				if ( m_fDisplayingPlay && m_fPlayFromStart && (dwFlags & B_PLAY_CHECKED) )
				{
					m_buttonPlayFromStart.SetState( ODS_CHECKED | ODS_SELECTED | ODS_FOCUS );
				}
				else if ( !m_fDisplayingPlay && m_fStopImmediate && (dwFlags & B_STOP_CHECKED) )
				{
					m_buttonPlayFromStart.SetState( ODS_CHECKED | ODS_SELECTED | ODS_FOCUS );
				}
				else
				{
					m_buttonPlayFromStart.SetState( 0 );
				}
			}
			else
			{
				m_buttonPlayFromStart.SetState( 0 );
			}
		}
		*/

		// Transition button
		if (dwFlags & B_TRANS_ENABLED)
		{
			::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_TRANSITION, MAKELONG(TRUE, 0) );
			::SendMessage( m_hWndToolbar, TB_INDETERMINATE, ID_TRANSP_TRANSITION, MAKELONG(FALSE, 0) );
		}
		else
		{
			::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_TRANSITION, MAKELONG(FALSE, 0) );
		}
		if (dwFlags & B_TRANS_CHECKED)
		{
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_TRANSITION, MAKELONG(TRUE, 0) );
		}
		else
		{
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_TRANSITION, MAKELONG(FALSE, 0) );
		}

		// Record button
		if (dwFlags & B_REC_ENABLED)
		{
			::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_RECORD, MAKELONG(TRUE, 0) );
			::SendMessage( m_hWndToolbar, TB_INDETERMINATE, ID_TRANSP_RECORD, MAKELONG(FALSE, 0) );
		}
		else
		{
			::SendMessage( m_hWndToolbar, TB_ENABLEBUTTON, ID_TRANSP_RECORD, MAKELONG(FALSE, 0) );
		}
		if (dwFlags & B_REC_CHECKED)
		{
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_RECORD, MAKELONG(TRUE, 0) );
		}
		else
		{
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_RECORD, MAKELONG(FALSE, 0) );
		}
		return TRUE;
	}

	return FALSE;
}

// Set the state of the play, stop and transition buttons based on the current
// state of DMUSProd.

void CToolbarHandler::SetStateFromEngine( DWORD *pdwState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( pdwState != NULL );

	if (pdwState == NULL)
	{
		return;
	}

	#define PACK_BUTTON_STATES(play_enabled,play_checked,stop_enabled,transition_enabled) \
	((play_enabled) + ((play_checked) << 1) + ((stop_enabled) << 2) + ((transition_enabled) << 3))
	#define UNPACK_PLAY_ENABLED(c) ((c) & 1 ? B_PLAY_ENABLED:0)
	#define UNPACK_PLAY_CHECKED(c) ((c) & (1 << 1) ? B_PLAY_CHECKED:0)
	#define UNPACK_STOP_ENABLED(c) ((c) & (1 << 2) ? B_STOP_ENABLED:0)
	#define UNPACK_TRANSITION_ENABLED(c) ((c) & (1 << 3) ? B_TRANS_ENABLED:0)

	static char truthtable[] =
	{
		// +---m_rpActive != NULL (i.e. There is a valid active transport to receive play.)
		// |+---IsPlaying() == TRUE
		// ||
		/* 00 */ PACK_BUTTON_STATES(0,0,0,0),
		/* 01 */ PACK_BUTTON_STATES(0,1,1,0),
		/* 10 */ PACK_BUTTON_STATES(1,0,0,0),
		/* 11 */ PACK_BUTTON_STATES(0,1,1,1),
	};

	int select = 0;
	select = select + (g_pconductor->m_rpActive != NULL ? 1:0);
	select = select << 1;
	select = select + (g_pconductor->IsEnginePlaying() ? 1:0);
	
	char t = truthtable[select];
	
	*pdwState &= ~(B_PLAY_ENABLED | B_PLAY_CHECKED | B_STOP_ENABLED | B_TRANS_ENABLED);
	*pdwState |= UNPACK_PLAY_ENABLED(t);
	*pdwState |= UNPACK_PLAY_CHECKED(t);
	*pdwState |= UNPACK_STOP_ENABLED(t);

	if( g_pconductor->IsEnginePlaying()
	&&	g_pconductor->m_TransitionOptions.dwFlags & TRANS_TO_NOTHING )
	{
		// Always enable Transition if TRANS_TO_NOTHING is set and the engine is playing
		*pdwState |= B_TRANS_ENABLED;
	}
	else
	{
		*pdwState |= UNPACK_TRANSITION_ENABLED(t);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CToolbarHandler IDMUSProdToolBar::GetInfo

HRESULT CToolbarHandler::GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle )
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
	strTitle.LoadString(IDS_TRANSPORT_TITLE);
	*pbstrTitle = strTitle.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolbarHandler IDMUSProdToolBar::GetMenuText

HRESULT CToolbarHandler::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CString strText;
	strText.LoadString(IDS_TRANSPORT_MENUTEXT);
	*pbstrText = strText.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolbarHandler IDMUSProdToolBar::GetMenuHelpText

HRESULT CToolbarHandler::GetMenuHelpText( BSTR* pbstrMenuHelpText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrMenuHelpText == NULL )
	{
		return E_POINTER;
	}

	CString strMenuHelpText;
	strMenuHelpText.LoadString(IDS_TRANSPORT_HELP);
	*pbstrMenuHelpText = strMenuHelpText.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolbarHandler IDMUSProdToolBar::Initialize

HRESULT CToolbarHandler::Initialize( HWND hWndToolBar )
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
	int cxChar = tm.tmAveCharWidth;
	int cyChar = tm.tmHeight + tm.tmExternalLeading;

	pDC->SelectObject( pOldFont );
	delete pDC;

	TBBUTTON button;
	button.dwData = 0;
	button.iString = NULL;

	// 9 - Wide separator for relative tempo
	button.iBitmap = cxChar * 12;
	button.idCommand = IDC_RELATIVE_TEMPO;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton( 0, &button );

	// 8 - Wide separator for tempo
	button.iBitmap = cxChar * 13;
	button.idCommand = IDC_TEMPO;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton( 0, &button );

	// 7 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton( 0, &button );

	// 6 - Transition
	button.iBitmap = 2;
	button.idCommand = ID_TRANSP_TRANSITION;
	button.fsState = TBSTATE_INDETERMINATE;
	button.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 0, &button );

	// 5 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton( 0, &button );

	// 4 - Record
	button.iBitmap = 5;
	button.idCommand = ID_TRANSP_RECORD;
	button.fsState = TBSTATE_INDETERMINATE;
	button.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 0, &button );

	// 4 - Stop
	/*
	button.iBitmap = 1;
	button.idCommand = ID_TRANSP_STOP;
	button.fsState = TBSTATE_INDETERMINATE;
	button.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 0, &button );
	*/

	// 3 - Play
	button.iBitmap = 0;
	button.idCommand = ID_TRANSP_PLAY;
	button.fsState = TBSTATE_INDETERMINATE;
	button.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 0, &button );
	m_fDisplayingPlay = TRUE;

	// 2 - Play from beggining
	button.iBitmap = 8;
	button.idCommand = ID_TRANS_REWPLAY;
	button.fsState = TBSTATE_INDETERMINATE;
	button.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 0, &button );

	// 1 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton( 0, &button );

	// 0 - Wide separator for active transport indicator
	button.iBitmap = cxChar * 30;
	button.idCommand = IDC_TRANSP_COMBO;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	pToolBarCtrl->InsertButton( 0, &button );

	// Attach the synth button
	CRect rect;

	// Attach the relative tempo window
	pToolBarCtrl->GetItemRect( 9, &rect );
	CString str;
	if (g_pconductor)
	{
		str.Format("x%3d%%",g_pconductor->m_nRatio);
	}
	if( !m_buttonRatio.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
		rect, pToolBarCtrl, IDC_RELATIVE_TEMPO) )
	{
		return -1;
	}
	m_buttonRatio.SetFont( &m_font );
	m_buttonRatio.EnableWindow(TRUE);
	DWORD dwData;
	if( GetNewRegDWORD( _T("TempoRatioEnabled"), &dwData ) && (dwData != 0) )
	{
		m_buttonRatio.SetCheck( 1 );
		// CConductor::InitializeDirectMusic will set the state of the performance
		// engine to reflect the button's initial state
	}

	// Attach the tempo window
	pToolBarCtrl->GetItemRect( 8, &rect );
	// Don't add ES_NUMBER, or the user won't be able to enter decimal points
	if( !m_editTempo.Create(WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN,
		rect, pToolBarCtrl, IDC_TEMPO) )
	{
		return -1;
	}
	m_editTempo.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );
	m_editTempo.SetFont( &m_font );
	m_editTempo.EnableWindow(FALSE);
	m_editTempo.SetLimitText(7);// 1000.00

	// Attach the tempo spin control
	if( !m_spinTempo.Create(WS_CHILD | WS_VISIBLE |
		UDS_ARROWKEYS | UDS_ALIGNRIGHT | UDS_WRAP | UDS_SETBUDDYINT | UDS_NOTHOUSANDS,
		rect, pToolBarCtrl, IDC_SPIN_TEMPO) )
	{
		return -1;
	}
	m_spinTempo.SetBuddy( &m_editTempo );
	m_spinTempo.SetRange( DMUS_TEMPO_MIN, DMUS_TEMPO_MAX );
	m_spinTempo.SetPos( 120 );
	m_spinTempo.EnableWindow(TRUE);

	// Attach the combo box window
	pToolBarCtrl->GetItemRect( 0, &rect );
	rect.bottom = rect.top + (cyChar * 21);
	if( !m_comboActive.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_SORT,
		rect, pToolBarCtrl, IDC_TRANSP_COMBO) )
	{
		return -1;
	}
	m_comboActive.SetFont( &m_font );
	m_comboActive.EnableWindow(FALSE);

	if( pToolBarCtrl )
	{
		pToolBarCtrl->Detach();
		delete pToolBarCtrl;
	}

	m_fInTransition = FALSE;
	m_fInOnBarButtonClicked = FALSE;
	m_fPlayFromStart = FALSE;
	m_fDontUpdateRewPlay = FALSE;
	m_fStopImmediate = FALSE;

	return S_OK;
}

LRESULT CToolbarHandler::OnPlayClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);

	//TRACE("OnPlayClicked enter\n");
	//ASSERT( m_fDisplayingPlay );
	if( !m_fDisplayingPlay )
	{
		//TRACE("OnPlayClicked exit - stop button enabled\n");
		m_fInOnBarButtonClicked = FALSE;
		return 0;
	}

	if ( m_fInOnBarButtonClicked )
	{
		m_fInOnBarButtonClicked = FALSE;
		m_fPlayFromStart = TRUE;
	}
	else
	{
		m_fPlayFromStart = FALSE;
	}

	// Force KillFocus to give controls a chance to sync changes handled in KillFocus code
	CWnd* pWndHadFocus = CWnd::GetFocus();
	::SetFocus( NULL );
	if( pWndHadFocus )
	{
		pWndHadFocus->SetFocus();
	}

	/*
	// If something is playing, make it stop before playing.
	if( IsValid( g_pconductor->m_rpPlaying ) )
	{
		IDMUSProdTransport*  pITransport;
		g_pconductor->m_rpPlaying->GetTransport( &pITransport );
		if (FAILED(pITransport->Stop()))
		{
			ASSERT(FALSE);
			return FALSE;
		}
		else
		{
			// If we've stopped the engine, wait for the engine to really stop
			if ( g_pconductor->IsPlayingUsingEngine() )
			{
				short n = 0;
				while( n++ < 20 )
				{
					if( !g_pconductor->IsEnginePlaying() )
					{
						break;
					}
					Sleep( 100 );
				}
				if (n>20)
				{
					ASSERT(FALSE);
					TRACE("OnPlayClicked Timed out while stopping playing transport\n");
				}
				UpdatePlayingFromEngine();
			}
			else
			{
				// Transport is responsible for updating its flags
			}
			g_pconductor->m_rpPlaying = NULL;
			SetStateAuto();
		}
	}
	*/
	// Dispatch the play message to the active transport.
	if( IsValid( g_pconductor->m_rpActive ) )
	{
		IDMUSProdTransport*  pITransport;
		g_pconductor->m_rpActive->GetTransport( &pITransport );

		// Ensure the transport correctly tracks/doesn't track the cursor
		pITransport->TrackCursor( g_pconductor->m_pOptionsToolbarHandler ? g_pconductor->m_pOptionsToolbarHandler->m_fCursorEnabled : TRUE );

		ITransportRegEntry *pTmpPlaying = g_pconductor->m_rpPlaying;
		g_pconductor->m_rpPlaying = g_pconductor->m_rpActive;

		g_pconductor->m_pDMPerformance->GetTime( NULL, &m_mtPlayTime );

		PlayCountInSegment( pITransport );

		if (FAILED(pITransport->Play( m_fPlayFromStart )))
		{
			// Leave the buttons in their current state.
			g_pconductor->m_rpPlaying = pTmpPlaying;
			//TRACE("OnPlayClicked exit - play failed\n");
			return FALSE;
		}
		
		//EnableTimer(TRUE);

		if ( g_pconductor->IsPlayingUsingEngine() )
		{
			// Wait and see if the engine really started playing.
			short n = 0;
			while( n++ < 20 )
			{
				if( g_pconductor->IsEnginePlaying() )
				{
					UpdateActiveFromEngine();
					SetStateAuto();
					g_pconductor->SetMIDIExportFields( (IUnknown *)pITransport );
					//TRACE("OnPlayClicked exit - play music succeeded\n");
					return TRUE;
				}
				Sleep( 100 );
			}

			TRACE("OnPlayClicked Timed out while starting new transport\n");

			UpdatePlayingFromEngine();
			if (g_pconductor->m_rpPlaying != g_pconductor->m_rpActive)
			{
				UpdateActiveFromEngine();
			}
			g_pconductor->m_rpPlaying = NULL;
			SetStateAuto();
			//TRACE("OnPlayClicked exit\n");
			return FALSE;
		}
		else
		{
			// Not using the engine, so we can't double-check that the
			// transport actually started playing
			// The transport is responsible for setting the button states.
			// Switch to displaying the playing buttons, rather than the active buttons
			SetStateAuto();
			g_pconductor->SetMIDIExportFields( (IUnknown *)pITransport );
			//TRACE("OnPlayClicked exit - play succeeded\n");
			return TRUE;
		}
	}
	else
	{
		// Leave the buttons in their current state.
	}
	//TRACE("OnPlayClicked exit - last\n");
	return FALSE;
}

LRESULT CToolbarHandler::OnStopClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//TRACE("OnStopClicked enter\n");
	if( m_fDisplayingPlay )
	{
		//TRACE("OnStopClicked exiting early - play button enabled.\n");
		m_fInOnBarButtonClicked = FALSE;
		return FALSE;
	}

	if ( m_fInOnBarButtonClicked )
	{
		m_fInOnBarButtonClicked = FALSE;
		m_fStopImmediate = TRUE;
	}
	else
	{
		m_fStopImmediate = FALSE;
	}

	// Capture the state of the control key
	BOOL fCtrlKeyDown;
	fCtrlKeyDown = ((GetKeyState( VK_CONTROL ) & 0x8000) != 0);

	if( m_pCountInSegmentState )
	{
		// Stop the Count-In segment state
		// Don't pay attention to the m_fStopImmediate flag.
		g_pconductor->m_pDMPerformance->Stop( NULL, m_pCountInSegmentState, 0, 0 );

		// Release the Count-In segment state
		m_pCountInSegmentState->Release();
		m_pCountInSegmentState = NULL;
	}

	BOOL fSendAllNotesOff = TRUE;

	if( !fCtrlKeyDown )
	{
		// Control key up - stop all secondary segments.
		// Stop all Secondary Segments
		POSITION pos = g_pconductor->m_lstSecondaryToolbars.GetHeadPosition();
		while( pos )
		{
			CSecondaryToolbarHandler *pToolbarHandler = g_pconductor->m_lstSecondaryToolbars.GetNext( pos );
			BOOL bHandled;
			for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
			{
				pToolbarHandler->OnButtonClicked( 0, (WORD)(ID_TRANSP_STOP1 + i), 0, bHandled );
			}
		}
	}
	else
	{
		// Check if any secondary segments are currently playing
		POSITION pos = g_pconductor->m_lstSecondaryToolbars.GetHeadPosition();
		while( pos && fSendAllNotesOff )
		{
			CSecondaryToolbarHandler *pToolbarHandler = g_pconductor->m_lstSecondaryToolbars.GetNext( pos );
			for( DWORD i=0; i < pToolbarHandler->m_dwNumButtons; i++ )
			{
				if( pToolbarHandler->m_arrayButtons[i]->pISegmentState )
				{
					// Something's playing - don't send AllNotesOff
					fSendAllNotesOff = FALSE;
					break;
				}
			}
		}
	}

	//ASSERT( g_pconductor->m_rpPlaying != NULL );
	if( IsValid( g_pconductor->m_rpPlaying ) )
	{
		IDMUSProdTransport*  pITransport;
		ITransportRegEntry *pPlaying = g_pconductor->m_rpPlaying;
		g_pconductor->m_rpPlaying->GetTransport( &pITransport );
		if (pITransport)
		{
			if (FAILED(pITransport->Stop( m_fStopImmediate )))
			{
				// Try and stop the music
				MUSIC_TIME mtNow;
				if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
				{
					// If stop at boundary chosen, update mtNow to reflect the boundary of the current segment state
					if( !m_fStopImmediate )
					{
						IDirectMusicSegmentState *pSegState;
						if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetSegmentState( &pSegState, mtNow ) ) )
						{
							DWORD dwResolution = 0;
							IDirectMusicSegment *pSegment;
							if( SUCCEEDED( pSegState->GetSegment( &pSegment ) ) )
							{
								pSegment->GetDefaultResolution( &dwResolution );
								pSegment->Release();
							}

							REFERENCE_TIME rtNow;
							g_pconductor->m_pDMPerformance->GetQueueTime(&rtNow);
							g_pconductor->m_pDMPerformance->GetResolvedTime(rtNow, &rtNow, dwResolution);
							g_pconductor->m_pDMPerformance->ReferenceToMusicTime(rtNow, &mtNow);
							pSegState->Release();
						}
					}

					if( !fCtrlKeyDown )
					{
						// Control key up - stop all segments.
						g_pconductor->m_pDMPerformance->Stop( NULL, NULL, mtNow, 0 );
					}
					else
					{
						// Stop just primary segment
						QueueSegmentState( g_pconductor->m_pDMPerformance, mtNow );
					}
				}
				//TRACE("OnStopClicked exit - Stop failed\n");
				//return FALSE;
			}
			//EnableTimer(FALSE);

			// Always tell the transport we released the record button
			pITransport->Record( FALSE );

			// Pop up the Record button if it is pressed
			DWORD dwFlags;
			pPlaying->GetFlags( &dwFlags );
			if( (dwFlags & B_REC_CHECKED) != 0 )
			{
				dwFlags &= ~B_REC_CHECKED;
				pPlaying->SetFlags( dwFlags );
				::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_RECORD, MAKELONG(FALSE, 0) );
			}

			if ( g_pconductor->IsPlayingUsingEngine() )
			{
				// Only wait for the music to stop if an immediate stop was requested
				if( m_fStopImmediate )
				{
					short n = 0;
					while( n++ < 20 )
					{
						if( !g_pconductor->IsEnginePlaying() )
						{
							g_pconductor->m_rpPlaying = NULL;

							UpdateActiveFromEngine();
							m_fPlayFromStart = FALSE;
							m_fStopImmediate = FALSE;
							SetStateAuto();
							// Send all notes/controllers/sound off commands
							if( fSendAllNotesOff )
							{
								g_pconductor->SendAllNotesOff( 250, FALSE );
							}

							//TRACE("OnStopClicked exit - Stop succeeded\n");
							return TRUE;
						}
						Sleep( 100 );
					}

					TRACE("OnStopClicked Timed out\n");

					// Try and stop the music
					MUSIC_TIME mtNow;
					if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
					{
						// If stop at boundary chosen, update mtNow to reflect the boundary of the current segment state
						if( !m_fStopImmediate )
						{
							IDirectMusicSegmentState *pSegState;
							if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetSegmentState( &pSegState, mtNow ) ) )
							{
								DWORD dwResolution = 0;
								IDirectMusicSegment *pSegment;
								if( SUCCEEDED( pSegState->GetSegment( &pSegment ) ) )
								{
									pSegment->GetDefaultResolution( &dwResolution );
									pSegment->Release();
								}

								REFERENCE_TIME rtNow;
								g_pconductor->m_pDMPerformance->GetQueueTime(&rtNow);
								g_pconductor->m_pDMPerformance->GetResolvedTime(rtNow, &rtNow, dwResolution);
								g_pconductor->m_pDMPerformance->ReferenceToMusicTime(rtNow, &mtNow);
								pSegState->Release();
							}
						}

						if( !fCtrlKeyDown )
						{
							// Control key up - stop all segments.
							g_pconductor->m_pDMPerformance->Stop( NULL, NULL, mtNow, 0 );
						}
						else
						{
							// Stop just primary segment
							QueueSegmentState( g_pconductor->m_pDMPerformance, mtNow );
						}
					}

					// Something messed up.  Refresh the playing buttons
					UpdatePlayingFromEngine();
					m_fPlayFromStart = FALSE;
					m_fStopImmediate = FALSE;
					SetStateAuto();
					// Send all notes/controllers/sound off commands
					if( fSendAllNotesOff )
					{
						g_pconductor->SendAllNotesOff( 250, FALSE );
					}
					//TRACE("OnStopClicked exit - Stop failed\n");
				}
				return FALSE;
			}
			else
			{
				// Not using the engine, so we can't double-check that the
				// transport actually stopped
				g_pconductor->m_rpPlaying = NULL;

				// Update the active flags, if they're using the engine
				UpdateActiveFromEngine();

				// Switch to displaying the active buttons
				m_fPlayFromStart = FALSE;
				SetStateAuto();
				// Send all notes/controllers/sound off commands
				if( fSendAllNotesOff )
				{
					g_pconductor->SendAllNotesOff( 250, FALSE );
				}
				//TRACE("OnStopClicked exit - Stop no music succeeded\n");
				return TRUE;
			}
		}
	}
	else
	{
		TRACE("OnStopClicked: playing index is invalid!\n");
		if( IsValid( g_pconductor->m_rpActive ) )
		{
			ITransportRegEntry *pPlaying = g_pconductor->m_rpActive;
			IDMUSProdTransport*  pITransport;
			g_pconductor->m_rpActive->GetTransport( &pITransport );
			if (pITransport)
			{
				if (FAILED(pITransport->Stop( m_fStopImmediate )))
				{
					//TRACE("OnStopClicked exit - Stop failed\n");
				}
				//EnableTimer(FALSE);

				// Always tell the transport we released the record button
				pITransport->Record( FALSE );

				// Pop up the Record button if it is pressed
				DWORD dwFlags;
				pPlaying->GetFlags( &dwFlags );
				if( (dwFlags & B_REC_CHECKED) != 0 )
				{
					dwFlags &= ~B_REC_CHECKED;
					pPlaying->SetFlags( dwFlags );
					::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_RECORD, MAKELONG(FALSE, 0) );
				}

				// Try and stop the music
				MUSIC_TIME mtNow;
				if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
				{
					// If stop at boundary chosen, update mtNow to reflect the boundary of the current segment state
					if( !m_fStopImmediate )
					{
						IDirectMusicSegmentState *pSegState;
						if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetSegmentState( &pSegState, mtNow ) ) )
						{
							DWORD dwResolution = 0;
							IDirectMusicSegment *pSegment;
							if( SUCCEEDED( pSegState->GetSegment( &pSegment ) ) )
							{
								pSegment->GetDefaultResolution( &dwResolution );
								pSegment->Release();
							}

							REFERENCE_TIME rtNow;
							g_pconductor->m_pDMPerformance->GetQueueTime(&rtNow);
							g_pconductor->m_pDMPerformance->GetResolvedTime(rtNow, &rtNow, dwResolution);
							g_pconductor->m_pDMPerformance->ReferenceToMusicTime(rtNow, &mtNow);
							pSegState->Release();
						}
					}

					if( !fCtrlKeyDown )
					{
						// Control key up - stop all segments.
						g_pconductor->m_pDMPerformance->Stop( NULL, NULL, mtNow, 0 );
					}
					else
					{
						// Stop just primary segment
						QueueSegmentState( g_pconductor->m_pDMPerformance, mtNow );
					}
				}

				UpdateActiveFromEngine();
				m_fPlayFromStart = FALSE;
				m_fStopImmediate = FALSE;
				SetStateAuto();
				// Send all notes/controllers/sound off commands
				if( fSendAllNotesOff )
				{
					g_pconductor->SendAllNotesOff( 250, FALSE );
				}

				//TRACE("OnStopClicked exit - Stop succeeded\n");
				return TRUE;
			}
		}
	}

	//TRACE("OnStopClicked exit\n");
	return FALSE;
}

LRESULT CToolbarHandler::OnTransitionClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);

	m_fInTransition = TRUE;

	if( g_pconductor->m_TransitionOptions.dwFlags & TRANS_TO_NOTHING )
	{
		// Transition to nothing
		TransitionToNothing();
		m_fInTransition = FALSE;
		return TRUE;
	}

	ASSERT( g_pconductor->m_rpActive != NULL );
	// We should never even get here if m_rpActive is NULL, but just in case.
	if( IsValid( g_pconductor->m_rpActive ) )
	{
		// Get a pointer to the active transport
		IDMUSProdTransport*  pITransportActive;
		g_pconductor->m_rpActive->GetTransport( &pITransportActive );

		// Try and get pointers to the currently playing transport
		IDMUSProdTransport*  pITransportPlaying = NULL;
		if( IsValid( g_pconductor->m_rpPlaying ) )
		{
			g_pconductor->m_rpPlaying->GetTransport( &pITransportPlaying );
		}

		// Save the original settings, in case we have to change them
		DWORD dwOrigFlags = g_pconductor->m_TransitionOptions.dwFlags;
		DWORD dwOrigBoundaryFlags = g_pconductor->m_TransitionOptions.dwBoundaryFlags;

		// If the current segment repeats endlessly, change the transition option from
		// 'End of segment' to 'next bar'
		if( (g_pconductor->m_TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT)
		||	( (g_pconductor->m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND)
			 && !(g_pconductor->m_TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN) ) )
		{
			MUSIC_TIME mtNow;
			if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetTime( NULL, &mtNow ) ) )
			{
				IDirectMusicSegmentState* pSegState;
				if( SUCCEEDED( g_pconductor->m_pDMPerformance->GetSegmentState( &pSegState, mtNow ) ) )
				{
					IDirectMusicSegment* pSegment;
					if( SUCCEEDED( pSegState->GetSegment( &pSegment ) ) )
					{
						// Check if this segment is a Pattern audition segment
						IDirectMusicObject *pIDirectMusicObject = NULL;
						if( SUCCEEDED( pSegment->QueryInterface( IID_IDirectMusicObject, (void **)&pIDirectMusicObject ) ) )
						{
							DMUS_OBJECTDESC objDesc;
							objDesc.dwSize = sizeof( DMUS_OBJECTDESC );
							if( SUCCEEDED( pIDirectMusicObject->GetDescriptor( &objDesc ) )
							&&	(objDesc.dwValidData & DMUS_OBJ_OBJECT)
							&&	(objDesc.guidObject == GUID_PatternAuditionSegment) )
							{
								// This is a pattern audition segment - change the boundary to 'measure'
								g_pconductor->m_TransitionOptions.dwFlags &= ~TRANS_END_OF_SEGMENT;
								g_pconductor->m_TransitionOptions.dwBoundaryFlags = DMUS_COMPOSEF_MEASURE;
							}
							pIDirectMusicObject->Release();
						}
						pSegment->Release();
					}
					pSegState->Release();
				}
			}
		}

		// Ensure the transport correctly tracks/doesn't track the cursor
		pITransportActive->TrackCursor( g_pconductor->m_pOptionsToolbarHandler ? g_pconductor->m_pOptionsToolbarHandler->m_fCursorEnabled : TRUE );

		// Try to transition to the active transport
		HRESULT hr;
		hr = pITransportActive->Transition();
		if( hr == E_NOTIMPL )
		{
			// Reset the transition options
			g_pconductor->m_TransitionOptions.dwFlags = dwOrigFlags;
			g_pconductor->m_TransitionOptions.dwBoundaryFlags = dwOrigBoundaryFlags ;

			// Since anything may have happened during the above call to Transition()
			// (even it is supposedly not implemented) reset pITransportActive and
			// pITransportPlaying
			if ( IsValid( g_pconductor->m_rpPlaying ) )
			{
				g_pconductor->m_rpPlaying->GetTransport( &pITransportPlaying );
			}
			else
			{
				pITransportPlaying = NULL;
			}
			if ( IsValid( g_pconductor->m_rpActive ) )
			{
				// Transition from what is currently playing to the active transport
				g_pconductor->m_rpActive->GetTransport( &pITransportActive );

				ASSERT( pITransportActive );

				if ( pITransportPlaying )
				{
					if (FAILED(pITransportPlaying->Stop( TRUE )))
					{
						ASSERT(FALSE);
						// If Stop() failed, update the toolbar state and return.
						UpdatePlayingFromEngine();
						// If the playing transport isn't using the engine, it should update
						// its buttons itself
						m_fInTransition = FALSE;
						SetStateAuto();
						return FALSE;
					}
					if ( g_pconductor->IsPlayingUsingEngine() )
					{
						// Wait until the engine actually stops
						short n = 0;
						while( n++ < 20 )
						{
							if( !g_pconductor->IsEnginePlaying() )
							{
								g_pconductor->m_rpPlaying = NULL;
								break;
							}
							Sleep( 100 );
						}

						TRACE("OnTransitionClicked Timed out while stopping transport\n");
					}
					else
					{
						// Not using the engine, clear the playing index
						g_pconductor->m_rpPlaying = NULL;
					}
				}

				// no longer necessarily playing from start
				m_fPlayFromStart = FALSE;
				// update the toolbar state 
				SetStateAuto();

				if (FAILED(pITransportActive->Play( TRUE )))
				{
					// Update the active flags, if they're using the engine
					UpdateActiveFromEngine();
					// update the toolbar state and return
					m_fInTransition = FALSE;
					SetStateAuto();
					return FALSE;
				}

				//EnableTimer(TRUE);

				// Assume that it really started..
				// Set the playing index
				g_pconductor->m_rpPlaying = g_pconductor->m_rpActive;
				if ( g_pconductor->IsActiveUsingEngine() )
				{
					short n = 0;
					while( n++ < 20 )
					{
						if( g_pconductor->IsEnginePlaying() )
						{
							// Update the playing and active buttons.  This one call does both
							// because m_rpPlaying == m_rpActive
							UpdateActiveFromEngine();
							// Update the buttons on the toolbar
							m_fInTransition = FALSE;
							SetStateAuto();
							return TRUE;
						}
						Sleep( 100 );
					}

					TRACE("OnTransitionClicked Timed out while starting transport\n");

					g_pconductor->m_rpPlaying = NULL;
					// Update the active buttons, if it's using the engine
					UpdateActiveFromEngine();
					// Update the buttons on the toolbar
					m_fInTransition = FALSE;
					SetStateAuto();
					return FALSE;
				}
				SetStateAuto();
			}
		}
		else if( SUCCEEDED( hr ) )
		{
			// Transition succeeded
			// Reset the transition options
			g_pconductor->m_TransitionOptions.dwFlags = dwOrigFlags;
			g_pconductor->m_TransitionOptions.dwBoundaryFlags = dwOrigBoundaryFlags ;

			g_pconductor->m_rpPlaying = g_pconductor->m_rpActive;
			SetStateAuto();
		}
		else
		{
			// Transition failed, but didn't return E_NOTIMPL

			// Leave the buttons in their current state.
			//TRACE("OnTransitionClicked exit - transition failed\n");
			return FALSE;
		}
	}
	m_fInTransition = FALSE;
	return TRUE;
}

LRESULT CToolbarHandler::OnRecordClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();

	if ( !IsValid( pDisplayedRegEntry ) )
	{
		return TRUE;
	}

	IDMUSProdTransport*  pITransport = NULL;
	pDisplayedRegEntry->GetTransport( &pITransport );
	if( !pITransport )
	{
		return FALSE;
	}
	DWORD dwFlags;
	pDisplayedRegEntry->GetFlags( &dwFlags );

	if( !(dwFlags & B_REC_ENABLED) )
	{
		return FALSE;
	}

	if( (dwFlags & B_REC_CHECKED) == 0 )
	{
		if( SUCCEEDED( pITransport->Record( TRUE ) ) )
		{
			dwFlags |= B_REC_CHECKED;
			pDisplayedRegEntry->SetFlags( dwFlags );
			SetStateAuto();
		}
	}
	else
	{
		if( SUCCEEDED( pITransport->Record( FALSE ) ) )
		{
			dwFlags &= ~B_REC_CHECKED;
			pDisplayedRegEntry->SetFlags( dwFlags );
			SetStateAuto();
		}
	}

	return TRUE;
}

LRESULT CToolbarHandler::OnSelchangeComboTransport( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_comboActive.GetCurSel() != CB_ERR )
	{
		g_pconductor->m_rpActive = static_cast<ITransportRegEntry *>(m_comboActive.GetItemDataPtr( m_comboActive.GetCurSel() ));
		SetStateAuto();
	}
	return TRUE;
}

void CToolbarHandler::SetStateAuto( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Verify our window handle is still valid
	if( !::IsWindow( m_hWndToolbar ) )
	{
		return;
	}

	// Get the index of the currently displayed transport
	ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();

	// Make sure the index is within the size of the registry
	if ( !IsValid( pDisplayedRegEntry ) )
	{
		SetState( 0 );
		return;
	}

	// Get the flags to display from the displayed transport
	DWORD dwFlags;
	pDisplayedRegEntry->GetFlags( &dwFlags );

	// If the displayed transport uses the engine, update its flags from the engine's state
	if (!(dwFlags & B_NOT_USING_ENGINE))
	{
		SetStateFromEngine(&dwFlags);
	}

	if (pDisplayedRegEntry != g_pconductor->m_rpActive)
	{
		// If we didn't get the flags from the active transport, get
		// the transition button state from the active transport flags.
		dwFlags &= ~(B_TRANS_ENABLED | B_TRANS_CHECKED);
		if( IsValid( g_pconductor->m_rpActive ) )
		{
			// If there is a valid active transport, get the transition flags from it.
			DWORD dwTmpFlags;
			g_pconductor->m_rpActive->GetFlags( &dwTmpFlags );
			
			// If the active transport uses the engine, update its flags from the engine's state
			if( !(dwTmpFlags & B_NOT_USING_ENGINE) )
			{
				SetStateFromEngine(&dwTmpFlags);
			}

			// Finally, set the transition flags from the active transport
			dwFlags |= dwTmpFlags & (B_TRANS_ENABLED | B_TRANS_CHECKED);
		}
	}

	// Set the button states to what the flags describe.
	SetState( dwFlags );

	// Make sure the index is within the size of the registry
	if ( !IsValid( pDisplayedRegEntry ) )
	{
		SetState( 0 );

		// Disable the tempo edit box
		if( ::IsWindow( m_editTempo.GetSafeHwnd() ) )
		{
			m_editTempo.EnableWindow(FALSE);
			m_spinTempo.EnableWindow(FALSE);
		}
		return;
	}

	BOOL fTempoUpdated = FALSE;
	IDMUSProdTransport* pITransport;
	pDisplayedRegEntry->GetTransport( &pITransport );
	if (pITransport)
	{
		IDMUSProdConductorTempo* pIConductorTempo;
		if( SUCCEEDED( pITransport->QueryInterface(IID_IDMUSProdConductorTempo, (void **)&pIConductorTempo) ) )
		{
			// We've found an IConductorTempo interface - the transport probably supports the tempo controls
			ASSERT( pIConductorTempo );

			// Ask the transport what it wants the tempo modifier to display
			/*
			double dModifier;
			if ( ::IsWindow( m_buttonRatio.GetSafeHwnd() ) &&
				 SUCCEEDED( pIConductorTempo->GetTempoModifier( &dModifier )))
			{
				// Limit the ratio to 0.25 - 2.0
				if ( dModifier < DMUS_MASTERTEMPO_MIN )
				{
					dModifier = DMUS_MASTERTEMPO_MIN;
				}
				else if ( dModifier > DMUS_MASTERTEMPO_MAX )
				{
					dModifier = DMUS_MASTERTEMPO_MAX;
				}

				// Update the button, if anything has changed.
				if( g_pconductor->m_nRatio != int (dModifier * 100.0) )
				{
					g_pconductor->m_nRatio = int (dModifier * 100.0);
					UpdateButtonRatio();
				}
			}
			*/

			// Ask the transport what it wants the tempo to display
			double dTempo;
			if ( ::IsWindow( m_editTempo.GetSafeHwnd() ) )
			{
				HRESULT hr = pIConductorTempo->GetTempo( &dTempo );
				// Update the edit box, if anything has changed.
				if( SUCCEEDED(hr) && (g_pconductor->m_dblTempo != dTempo) )
				{
					g_pconductor->m_dblTempo = dTempo;
					m_spinTempo.SetPos( int(dTempo) );
					if (m_editTempo.GetSafeHwnd())
					{
						CString str;
						str.Format("%.2f",dTempo);
						m_editTempo.SetWindowText(str);
					}
				}

				if( FAILED(hr) )
				{
					// Clear the control, since the transport doesn't provide a tempo
					m_editTempo.SetWindowText(NULL);
					g_pconductor->m_dblTempo = 0.0;
				}

				if( hr == S_OK )
				{
					// Enable the control, since the transport supports it
					m_editTempo.EnableWindow(TRUE);
					m_spinTempo.EnableWindow(TRUE);
				}
				else
				{
					// Disable the control, since the transport doesn't support changing tempos
					m_editTempo.EnableWindow(FALSE);
					m_spinTempo.EnableWindow(FALSE);
				}
			}
			pIConductorTempo->Release();

			// Note that we've updated the tempo control
			fTempoUpdated = TRUE;
		}
	}

	if( !fTempoUpdated )
	{
		// Disable the tempo edit box
		if( ::IsWindow( m_editTempo.GetSafeHwnd() ) )
		{
			m_editTempo.EnableWindow(FALSE);
			m_spinTempo.EnableWindow(FALSE);
		}
	}
}

void CToolbarHandler::UpdateActiveFromEngine( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Make sure m_rpActive is valid
	if( IsValid( g_pconductor->m_rpActive ) )
	{
		// Get the flags for the active transport
		DWORD dwFlags;
		g_pconductor->m_rpActive->GetFlags( &dwFlags );

		// If the active transport uses the engine, update its flags from the engine's state
		if (!(dwFlags & B_NOT_USING_ENGINE))
		{
			SetStateFromEngine(&dwFlags);
			g_pconductor->m_rpActive->SetFlags( dwFlags );
		}
	}
}

void CToolbarHandler::UpdatePlayingFromEngine( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Make sure m_rpActive is valid
	if( IsValid( g_pconductor->m_rpPlaying ) )
	{
		// Get the flags for the playing transport
		DWORD dwFlags;
		g_pconductor->m_rpPlaying->GetFlags( &dwFlags );

		// If the playing transport uses the engine, update its flags from the engine's state
		if (!(dwFlags & B_NOT_USING_ENGINE))
		{
			SetStateFromEngine(&dwFlags);
			g_pconductor->m_rpPlaying->SetFlags( dwFlags );
		}
	}
}

LRESULT CToolbarHandler::OnRelTempoClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	/*
	ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();

	if ( !IsValid( pDisplayedRegEntry ) )
	{
		return TRUE;
	}

	IDMUSProdTransport* pITransport;
	pDisplayedRegEntry->GetTransport( &pITransport );

	if (!pITransport)
	{
		return TRUE;
	}

	IDMUSProdConductorTempo* pIConductorTempo = NULL;
	pITransport->QueryInterface(IID_IDMUSProdConductorTempo, (void **)&pIConductorTempo);
	*/

	BOOL fDone = FALSE;
	if (m_buttonRatio.GetCheck() & 1)
	{
		// Checked
		/*
		if ( pIConductorTempo && SUCCEEDED(pIConductorTempo->TempoModified(TRUE)) )
		{
			fDone = TRUE;
		}
		*/
		if ( !fDone )
		{
			if ( g_pconductor->m_pDMPerformance )
			{
				float fModifier;
				fModifier = float(g_pconductor->m_nRatio) / 100.0f;
				g_pconductor->m_pDMPerformance->SetGlobalParam( GUID_PerfMasterTempo, &fModifier, sizeof(float) );
			}
		}
	}
	else
	{
		// Unchecked
		/*
		if ( pIConductorTempo && SUCCEEDED(pIConductorTempo->TempoModified(FALSE)) )
		{
			fDone = TRUE;
		}
		*/
		if ( !fDone )
		{
			if ( g_pconductor->m_pDMPerformance )
			{
				float fModifier;
				fModifier = 1.0;
				g_pconductor->m_pDMPerformance->SetGlobalParam( GUID_PerfMasterTempo, &fModifier, sizeof(float) );
			}
		}
	}
	/*
	if ( pIConductorTempo )
	{
		pIConductorTempo->Release();
	}
	*/

	// Save the state in the registry
	SetNewRegDWORD( _T("TempoRatioEnabled"), m_buttonRatio.GetCheck() & 1, TRUE );

	return TRUE;
}

void CToolbarHandler::UpdateButtonRatio( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString str;
	if (g_pconductor)
	{
		str.Format( "x%3d%%", g_pconductor->m_nRatio );
		m_buttonRatio.SetWindowText(str);

		// Update the associated transport
		BOOL fDone = FALSE;

		/*
		ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();
		if ( IsValid( pDisplayedRegEntry ) )
		{
			IDMUSProdTransport* pITransport;
			pDisplayedRegEntry->GetTransport( &pITransport );

			if (pITransport)
			{
				IDMUSProdConductorTempo* pIConductorTempo;
				if( SUCCEEDED( pITransport->QueryInterface(IID_IDMUSProdConductorTempo, (void **)&pIConductorTempo) ) )
				{
					ASSERT( pIConductorTempo );

					if( SUCCEEDED( pIConductorTempo->SetTempoModifier( double(g_pconductor->m_nRatio) / 100.0) ) )
					{
						fDone = TRUE;
					}
					pIConductorTempo->Release();
				}
			}
		}
		*/

		// If not set abive, and the ratio button is checked, update GUID_PerfMasterTempo
		if ( (m_buttonRatio.GetState() & 1) && !fDone )
		{
			if ( g_pconductor->m_pDMPerformance )
			{
				float fModifier;
				fModifier = float(g_pconductor->m_nRatio) / 100.0f;
				g_pconductor->m_pDMPerformance->SetGlobalParam( GUID_PerfMasterTempo, &fModifier, sizeof(float) );
			}
		}

	}
}

LRESULT CToolbarHandler::OnKillFocusTempo( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (g_pconductor && (m_editTempo.GetSafeHwnd() != NULL))
	{
		// Update the associated transport
		ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();

		if ( !IsValid( pDisplayedRegEntry ) )
		{
			return TRUE;
		}

		IDMUSProdTransport* pITransport;
		pDisplayedRegEntry->GetTransport( &pITransport );

		if (!pITransport)
		{
			return TRUE;
		}

		IDMUSProdConductorTempo* pIConductorTempo;
		if( FAILED( pITransport->QueryInterface(IID_IDMUSProdConductorTempo, (void **)&pIConductorTempo) ) )
		{
			return TRUE;
		}

		ASSERT( pIConductorTempo );

		CString strNewTempo;
		m_editTempo.GetWindowText( strNewTempo );
		strNewTempo.TrimRight();
		strNewTempo.TrimLeft();

		if( strNewTempo.IsEmpty() )
		{
			m_spinTempo.SetPos( int(g_pconductor->m_dblTempo) );
			if( floor(g_pconductor->m_dblTempo) != g_pconductor->m_dblTempo )
			{
				strNewTempo.Format("%.2f",g_pconductor->m_dblTempo);
				m_editTempo.SetWindowText( strNewTempo );
			}
		}
		else
		{
			double dblTempo;
			dblTempo = _tcstod(strNewTempo, NULL);
			if( dblTempo > DMUS_TEMPO_MAX )
			{
				dblTempo = DMUS_TEMPO_MAX;
				m_spinTempo.SetPos( int(dblTempo) );
			}
			else if( dblTempo < DMUS_TEMPO_MIN )
			{
				dblTempo = DMUS_TEMPO_MIN;
				m_spinTempo.SetPos( int(dblTempo) );
			}
			if (dblTempo != g_pconductor->m_dblTempo )
			{
				pIConductorTempo->SetTempo( dblTempo );
				g_pconductor->m_dblTempo = dblTempo;
				if( floor(dblTempo) != dblTempo )
				{
					m_spinTempo.SetPos( int(dblTempo) );
					strNewTempo.Format("%.2f",dblTempo);
					m_editTempo.SetWindowText( strNewTempo );
				}
			}
		}
		pIConductorTempo->Release();
	}
	return TRUE;
}

LRESULT CToolbarHandler::OnDeltaPosTempo( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (g_pconductor && (m_spinTempo.GetSafeHwnd() != NULL))
	{
		// Update the associated transport
		ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();

		if ( !IsValid( pDisplayedRegEntry ) )
		{
			return TRUE;
		}

		IDMUSProdTransport* pITransport;
		pDisplayedRegEntry->GetTransport( &pITransport );

		if (!pITransport)
		{
			return TRUE;
		}

		IDMUSProdConductorTempo* pIConductorTempo;
		if( FAILED( pITransport->QueryInterface(IID_IDMUSProdConductorTempo, (void **)&pIConductorTempo) ) )
		{
			return TRUE;
		}

		ASSERT( pIConductorTempo );

		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		CString strNewTempo;
		m_editTempo.GetWindowText( strNewTempo );
		strNewTempo.TrimRight();
		strNewTempo.TrimLeft();

		double dblTempo;
		dblTempo = _tcstod(strNewTempo, NULL);

		int nNewValue = int(floor(dblTempo)) + pNMUpDown->iDelta;
		if( nNewValue < DMUS_TEMPO_MIN )
		{
			nNewValue = DMUS_TEMPO_MIN;
		}
		else if( nNewValue > DMUS_TEMPO_MAX )
		{
			nNewValue = DMUS_TEMPO_MAX;
		}

		if( nNewValue != pNMUpDown->iPos )
		{
			m_spinTempo.SetPos( nNewValue );
		}
		if( nNewValue != g_pconductor->m_dblTempo )
		{
			g_pconductor->m_dblTempo = nNewValue;

			// Update the display
			CString str;
			str.Format("%.2f",g_pconductor->m_dblTempo);
			m_editTempo.SetWindowText(str);

			pIConductorTempo->SetTempo( nNewValue );
		}
		pIConductorTempo->Release();
	}
	return TRUE;
}

LRESULT CToolbarHandler::OnRewPlayClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_fInOnBarButtonClicked = TRUE;
	return OnPlayClicked( wNotifyCode, wID, hWndCtl, bHandled);
}

LRESULT CToolbarHandler::OnStopImmediateClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_fInOnBarButtonClicked = TRUE;
	return OnStopClicked( wNotifyCode, wID, hWndCtl, bHandled);
}

LRESULT CToolbarHandler::OnSetStateAuto( WORD /*wCode*/, WORD /*wLoWord*/, HWND /*hWnd*/, BOOL& /*bHandled*/ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_fDontUpdateRewPlay = TRUE;
	SetStateAuto();
	m_fDontUpdateRewPlay = FALSE;
	return TRUE;
}

LRESULT CToolbarHandler::OnMusicStopped( WORD /*wCode*/, WORD /*wLoWord*/, HWND /*hWnd*/, BOOL& /*bHandled*/ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( IsValid( g_pconductor->m_rpPlaying ) )
	{
		IDMUSProdTransport*  pITransport;
		g_pconductor->m_rpPlaying->GetTransport( &pITransport );
		if (pITransport)
		{
			// Always tell the transport we released the record button
			pITransport->Record( FALSE );
		}

		// Pop up the Record button if it is pressed
		DWORD dwFlags;
		g_pconductor->m_rpPlaying->GetFlags( &dwFlags );
		if( (dwFlags & B_REC_CHECKED) != 0 )
		{
			dwFlags &= ~B_REC_CHECKED;
			g_pconductor->m_rpPlaying->SetFlags( dwFlags );
			::SendMessage( m_hWndToolbar, TB_CHECKBUTTON, ID_TRANSP_RECORD, MAKELONG(FALSE, 0) );
		}
	}

	g_pconductor->m_rpPlaying = NULL;
	SetStateAuto();
	return TRUE;
}

LRESULT CToolbarHandler::OnPressPlayButton( WORD /*wCode*/, WORD /*wLoWord*/, HWND /*hWnd*/, BOOL& /*bHandled*/ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SetBtnState( ID_TRANSP_PLAY, TBSTATE_ENABLED | TBSTATE_CHECKED );
	return TRUE;
}

LRESULT CToolbarHandler::OnPressStopButton( WORD /*wCode*/, WORD /*wLoWord*/, HWND /*hWnd*/, BOOL& /*bHandled*/ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SetBtnState( ID_TRANSP_STOP, TBSTATE_ENABLED | TBSTATE_CHECKED );
	return TRUE;
}

CWndClassInfo& CToolbarHandler::GetWndClassInfo()
{
	static CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, StartWindowProc,
		  0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW+1), 0, NULL, 0 },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}


LRESULT CToolbarHandler::OnRightClick(WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled)
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
		// Transition is 6th item.
		::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 6, (LPARAM) &rect );

		if( rect.PtInRect( ptClient ) )
		{
			DlgTransition dlg;
			if( IDOK == dlg.DoModal() )
			{
				g_pconductor->m_TransitionOptions.dwBoundaryFlags = dlg.m_TransitionOptions.dwBoundaryFlags;
				g_pconductor->m_TransitionOptions.wPatternType = dlg.m_TransitionOptions.wPatternType;
				g_pconductor->m_TransitionOptions.dwFlags = dlg.m_TransitionOptions.dwFlags;
				g_pconductor->m_TransitionOptions.pDMUSProdNodeSegmentTransition = dlg.m_TransitionOptions.pDMUSProdNodeSegmentTransition;

				SetNewRegDWORD( _T("TransitionBoundary"), g_pconductor->m_TransitionOptions.dwBoundaryFlags, TRUE );
				SetNewRegDWORD( _T("TransitionPattern"), g_pconductor->m_TransitionOptions.wPatternType, TRUE );
				SetNewRegDWORD( _T("TransitionFlags"), g_pconductor->m_TransitionOptions.dwFlags, TRUE );
				// TODO: Save transition segment?
			}
		}
		else
		{
			// Relative tempo is handled by CRatioButton
		}
	}

	return TRUE;
}


IMPLEMENT_DYNCREATE( CMyEdit, CEdit )

CMyEdit::CMyEdit() : CEdit()
{
}

LRESULT CMyEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CHAR:
		switch( wParam )
		{
		case 8:  // Backspace
		case '.':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 71: // Home
		case 75: // Left
		case 77: // Right
		case 79: // End
		case 82: // Ins
		case 83: // Del
			break;
		default:
			return TRUE;
		break;
		}
	}
	return CEdit::WindowProc( message, wParam, lParam );
}

void CToolbarHandler::PlayCountInSegment( IDMUSProdTransport* pITransport )
{
	ASSERT( pITransport );
	if( !pITransport )
	{
		return;
	}

	if( m_pCountInSegmentState )
	{
		m_pCountInSegmentState->Release();
		m_pCountInSegmentState = NULL;
	}

	if( !g_pconductor->m_fMetronomeEnabled || !g_pconductor->m_pOptionsToolbarHandler )
	{
		return;
	}

	if( g_pconductor->m_pOptionsToolbarHandler->m_lCountInBars <= 0 )
	{
		return;
	}

	if( g_pconductor->m_pOptionsToolbarHandler->m_fCountOnlyOnRecord )
	{
		// This must be called just after m_rpPlaying was updated
		ASSERT( g_pconductor->m_rpPlaying == g_pconductor->m_rpActive );
		ASSERT( IsValid( g_pconductor->m_rpPlaying ) );
		if( !IsValid( g_pconductor->m_rpPlaying ) )
		{
			return;
		}

		// Check if the Record button is pressed
		DWORD dwFlags;
		g_pconductor->m_rpPlaying->GetFlags( &dwFlags );
		if( (dwFlags & B_REC_CHECKED) == 0 )
		{
			// It's not pressed - exit
			return;
		}
	}		

	IDMUSProdNode *pTransportNode = NULL;
	if( SUCCEEDED( pITransport->QueryInterface( IID_IDMUSProdNode, (void**)&pTransportNode ) ) )
	{
		// mtTime == 0 -> Get TimeSig/Tempo from from Start
		// mtTime != 0 -> Get TimeSig/Tempo from cursor
		DMUS_TIMESIGNATURE TimeSig;
		TimeSig.mtTime = m_fPlayFromStart ? 0 : 1;
		DMUS_TEMPO_PARAM Tempo;
		Tempo.mtTime = m_fPlayFromStart ? 0 : 1;
		if( SUCCEEDED( pTransportNode->GetObject( GUID_TimeSignature, GUID_TimeSignature, (void**)&TimeSig ) ) )
		{
			bool fGotTempo = false;
			if( SUCCEEDED( pTransportNode->GetObject( GUID_TempoParam, GUID_TempoParam, (void**)&Tempo ) ) )
			{
				fGotTempo = true;
			}
			// If not playing from start, get a 'fudge' factor to make the beat remain constant
			MUSIC_TIME mtOffset = 0;
			if( !m_fPlayFromStart )
			{
				pTransportNode->GetObject( GUID_ConductorCountInBeatOffset, GUID_ConductorCountInBeatOffset, (void**)&mtOffset );
			}

			IDirectMusicSegment *pSegment = NULL;
			if( SUCCEEDED( ::CoCreateInstance( CLSID_DirectMusicSegment, NULL, CLSCTX_INPROC, 
								 			   IID_IDirectMusicSegment, (void**)&pSegment ) ) )
			{
				IDirectMusicTrack *pTimeSigTrack = NULL;
				if( SUCCEEDED(  ::CoCreateInstance( CLSID_DirectMusicTimeSigTrack, NULL, CLSCTX_INPROC, 
								 					IID_IDirectMusicTrack, (void**)&pTimeSigTrack ) ) )
				{
					IDirectMusicTrack *pTempoTrack = NULL;
					if( !fGotTempo
					||	SUCCEEDED(  ::CoCreateInstance( CLSID_DirectMusicTempoTrack, NULL, CLSCTX_INPROC, 
								 						IID_IDirectMusicTrack, (void**)&pTempoTrack ) ) )
					{
						// Set the times of the Tempo and TimeSig events to 0
						TimeSig.mtTime = 0;
						Tempo.mtTime = 0;

						//TRACE("TimeSig: %d/%d\n", TimeSig.bBeatsPerMeasure, TimeSig.bBeat );
						//TRACE("Tempo: %lf\n", Tempo.dblTempo );

						// Insert the TimeSig event
						// This is unsupported in DX 6.1 and DX 7.
						// pTimeSigTrack->SetParam( GUID_TimeSignature, 0, &TimeSig );
						{
							IPersistStream *pIPersistStream;
							if( SUCCEEDED( pTimeSigTrack->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
							{
								IStream *pIStream;
								if( SUCCEEDED( ::CreateStreamOnHGlobal( NULL, TRUE, &pIStream ) ) )
								{
									// Write the FOURCC id
									FOURCC ckid = DMUS_FOURCC_TIMESIGNATURE_TRACK; // chunk ID to write
									pIStream->Write( &ckid, sizeof(FOURCC), NULL );

									// Write out the size of the chunk
									DWORD dwSize = sizeof( DMUS_IO_TIMESIGNATURE_ITEM ) + sizeof(DWORD);
									pIStream->Write( &dwSize, sizeof(DWORD), NULL );

									// Write out the size of the TimeSig item
									dwSize -= 4;
									pIStream->Write( &dwSize, sizeof(DWORD), NULL );

									// Write out the TimeSig item
									DMUS_IO_TIMESIGNATURE_ITEM oTimeSig;
									ZeroMemory( &oTimeSig, sizeof( DMUS_IO_TIMESIGNATURE_ITEM ) );
									oTimeSig.lTime = 0;
									oTimeSig.bBeatsPerMeasure = TimeSig.bBeatsPerMeasure;
									oTimeSig.bBeat = TimeSig.bBeat;
									oTimeSig.wGridsPerBeat = TimeSig.wGridsPerBeat;
									pIStream->Write( &oTimeSig, sizeof(DMUS_IO_TIMESIGNATURE_ITEM), NULL );

									// Seek back to the start
									LARGE_INTEGER li;
									li.HighPart = 0;
									li.LowPart = 0;
									pIStream->Seek( li, SEEK_SET, NULL );

									// Load in the TimeSig stream
									pIPersistStream->Load( pIStream );

									pIStream->Release();
								}
								pIPersistStream->Release();
							}
						}

						// Inset the TimeSig track
						pSegment->InsertTrack( pTimeSigTrack, 0xFFFFFFFF );

						// If necessary, insert the Tempo event and track
						if( pTempoTrack )
						{
							pTempoTrack->SetParam( GUID_TempoParam, 0, &Tempo );
							pSegment->InsertTrack( pTempoTrack, 0xFFFFFFFF );
						}

						// Set the length
						pSegment->SetLength( mtOffset + ((DMUS_PPQ * 4 / TimeSig.bBeat) * TimeSig.bBeatsPerMeasure * g_pconductor->m_pOptionsToolbarHandler->m_lCountInBars) );

						// Play the segment
						g_pconductor->m_pDMPerformance->PlaySegment( pSegment, 0, 0, &m_pCountInSegmentState );

						if( pTempoTrack )
						{
							pTempoTrack->Release();
						}
					}
					pTimeSigTrack->Release();
				}
				pSegment->Release();
			}
		}
		pTransportNode->Release();
	}
}

IMPLEMENT_DYNCREATE( CRatioButton, CButton )

CRatioButton::CRatioButton() : CButton()
{
}

LRESULT CRatioButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	switch( message )
	{
	case WM_RBUTTONUP:
		{
			DlgAltTempo dlg;
			if( IDOK == dlg.DoModal() )
			{
				g_pconductor->m_nRatio = dlg.m_nRatio;
				g_pconductor->m_pToolbarHandler->UpdateButtonRatio();

				SetNewRegDWORD( _T("TempoRatio"), g_pconductor->m_nRatio, TRUE );
			}
			break;
		}
	}
	return CButton::WindowProc( message, wParam, lParam );
}

void QueueSegmentState( IDirectMusicPerformance *pPerformance, MUSIC_TIME mtNow )
{
	// Validate parameter
	if( pPerformance )
	{
		IDirectMusicSegment *pSegment;
		if( SUCCEEDED( ::CoCreateInstance( CLSID_DirectMusicSegment, NULL, CLSCTX_INPROC, 
										   IID_IDirectMusicSegment, (void**)&pSegment ) ) )
		{
			pSegment->SetLength( 5 );
			int nCount = 20;
			while( (nCount > 0) && (pPerformance->PlaySegment( pSegment, 0, mtNow, NULL ) == DMUS_E_TIME_PAST) )
			{
				nCount--;
				mtNow += DMUS_PPQ / 4;
			}
			ASSERT( nCount > 0 );
			pSegment->Release();
		}
	}
}
/*
void DeQueueSegmentState( IDirectMusicPerformance *pPerformance, IDirectMusicSegmentState *pSegState, DWORD dwStopFlags )
{
	// Validate parameters
	if( pPerformance && pSegState )
	{
		// Get the segment this segment state refers to 
		IDirectMusicSegment *pSegment;
		if( SUCCEEDED( pSegState->GetSegment( &pSegment ) ) )
		{
			// Get the starting time, point, and duration of this segment
			MUSIC_TIME mtStartPoint, mtStartTime, mtDuration;
			VERIFY( SUCCEEDED( pSegState->GetStartPoint( &mtStartPoint ) ) );
			VERIFY( SUCCEEDED( pSegState->GetStartTime( &mtStartTime ) ) );
			VERIFY( SUCCEEDED( pSegment->GetLength( &mtDuration ) ) );

			// Get the number of loops in this segment
			DWORD dwLoops;
			VERIFY( SUCCEEDED( pSegment->GetRepeats( &dwLoops ) ) );

			if( dwLoops )
			{
				MUSIC_TIME dwLoopStart, dwLoopEnd;
				VERIFY( SUCCEEDED( pSegment->GetLoopPoints( &dwLoopStart, &dwLoopEnd ) ) );
				if( mtStartPoint < dwLoopEnd )
				{
					mtStartTime += (dwLoopEnd - dwLoopStart) * dwLoops;
				}
			}
			// Look for a segment state after this one
			IDirectMusicSegmentState *pNewSegState = NULL;
			mtStartPoint = mtStartTime - mtStartPoint + mtDuration;
			for( mtStartTime = mtStartPoint; mtStartTime < mtStartPoint + DMUS_PPQ * 4; mtStartTime += 100 )
			{
				if( SUCCEEDED( pPerformance->GetSegmentState( &pNewSegState, mtStartTime ) ) )
				{
					if( pNewSegState != pSegState )
					{
						break;
					}
					else
					{
						pNewSegState->Release();
						pNewSegState = NULL;
					}
				}
			}

			// Found another segment state, stop it first
			if( pNewSegState )
			{
				DeQueueSegmentState( pPerformance, pNewSegState, dwStopFlags );
				pNewSegState->Release();
			}

			// Stop this segment
			pPerformance->Stop( pSegment, pSegState, 0, dwStopFlags );

			//TRACE("Stoping SegState %x that ends at time %d with duration %d\n", pSegState, mtStartPoint, mtDuration);

			pSegment->Release();
		}
	}
}
*/

void CToolbarHandler::TransitionToNothing( void )
{
	IDirectMusicComposer8 *pIDMComposer;
	if( FAILED( ::CoCreateInstance( CLSID_DirectMusicComposer, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicComposer8, (void**)&pIDMComposer ) )
	||	!pIDMComposer )
	{
		return;
	}

	// Set the transition pattern (embellishment)
	WORD wCommand = 0;
	if( !(g_pconductor->m_TransitionOptions.dwFlags & TRANS_REGULAR_PATTERN) )
	{
		// if we have an embellishment, set it.
		wCommand = g_pconductor->m_TransitionOptions.wPatternType;
	}

	// Set the boundary flags
	DWORD dwFlags = g_pconductor->m_TransitionOptions.dwBoundaryFlags;

	// Set the end of segment boundary flag
	if( g_pconductor->m_TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT )
	{
		// Can't transition at COMPOSEF_QUEUE, so just use COMPOSEF_SEGMENTEND
		dwFlags |= DMUS_COMPOSEF_SEGMENTEND;
	}

	IDirectMusicSegment *pITransitionSegment = NULL;
	pIDMComposer->AutoTransition(g_pconductor->m_pDMPerformance,
											   NULL,
											   wCommand,
											   dwFlags,
											   NULL,
											   &pITransitionSegment,
											   NULL,
											   NULL);

	if( pITransitionSegment )
	{
		pITransitionSegment->Release();
		pITransitionSegment = NULL;
	}
}

void CToolbarHandler::AddTransport( ITransportRegEntry *pITransportRegEntry )
{
	// Ensure the pointer is valid
	if( pITransportRegEntry == NULL )
	{
		ASSERT(FALSE);
		return;
	}

	// Ensure the transport is valid
	IDMUSProdTransport* pTransport = NULL;
	pITransportRegEntry->GetTransport( &pTransport );
	if( pTransport == NULL )
	{
		ASSERT(FALSE);
		return;
	}

	BSTR  bstrName;
	CString strName;
	// If the transport doens't have a name, use a default name
	if ( SUCCEEDED(	pTransport->GetName( &bstrName ) ) )
	{
		strName = bstrName;
		SysFreeString( bstrName );
	}
	else
	{
		strName.LoadString( IDS_DEFAULT_TRANSPORT_NAME );
	}
	int nIndex = m_comboActive.AddString( strName );
	m_comboActive.SetItemDataPtr( nIndex, pITransportRegEntry );

	if(m_comboActive.GetCount() >= 1)
	{
		m_comboActive.EnableWindow( TRUE );
	}
}

void CToolbarHandler::RemoveTransport( ITransportRegEntry *pITransportRegEntry )
{
	// Ensure the pointer is valid
	if( pITransportRegEntry == NULL )
	{
		ASSERT(FALSE);
		return;
	}

	for( int nIndex = m_comboActive.GetCount() - 1; nIndex >= 0; nIndex-- )
	{
		if( m_comboActive.GetItemDataPtr( nIndex ) == pITransportRegEntry )
		{
			m_comboActive.DeleteString( nIndex );

			if(m_comboActive.GetCount() == 0)
			{
				m_comboActive.EnableWindow( FALSE );
			}
			else
			{
				m_comboActive.SetCurSel( min( m_comboActive.GetCount() - 1, nIndex) );
			}
			return;
		}
	}

	ASSERT(FALSE);
}

void CToolbarHandler::SetActiveTransport( ITransportRegEntry *pITransportRegEntry )
{
	// Ensure the pointer is valid
	if( pITransportRegEntry == NULL )
	{
		ASSERT(FALSE);
		return;
	}

	for( int nIndex = m_comboActive.GetCount() - 1; nIndex >= 0; nIndex-- )
	{
		if( m_comboActive.GetItemDataPtr( nIndex ) == pITransportRegEntry )
		{
			m_comboActive.SetCurSel( nIndex );
			return;
		}
	}

	ASSERT(FALSE);
}

LRESULT CToolbarHandler::OnSetActiveFromPointer( WORD /*wCode*/, WORD /*wLoWord*/, HWND /*hWnd*/, BOOL& /*bHandled*/ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SetActiveTransport( g_pconductor->m_rpActive );
	return TRUE;
}
