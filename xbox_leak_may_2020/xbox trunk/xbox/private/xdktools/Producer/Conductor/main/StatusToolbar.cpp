#include "stdafx.h"
#include "StatusToolbar.h"
#include "cconduct.h"
#include "OutputTool.h"
#include "Toolbar.h"
#include "SecondaryToolbar.h"
#include "TREntry.h"
#include <math.h>
#include <PChannelName.h>
#include "DlgEditPChannel.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CStatusToolbarHandler *g_pStatusToolbarHandler = NULL;

#define MIDI_CCHANGE    0xB0
#define MIDI_NOTEOFF    0x80

#ifdef _DEBUG
void DumpAudiopath( IDirectMusicAudioPath *pIDirectMusicAudiopath );
#endif

CStatusToolbarHandler::CStatusToolbarHandler()
{
	m_nTimerID = 0;
	g_pStatusToolbarHandler = this;
	m_hWndToolbar = NULL;
	m_nAfterWindowPosChanged = 0;
}

CStatusToolbarHandler::~CStatusToolbarHandler()
{
	if( m_nTimerID )
	{
		// No need to kill the timer - it was stopped when the window was destroyed.
	//	KillTimer( m_nTimerID );
		m_nTimerID = 0;
	}
	g_pStatusToolbarHandler = NULL;
	if( !m_mpButtonStatus.IsEmpty() )
	{
		CMyButton* pMyButton;
		WORD wPChannel;
		POSITION pos = m_mpButtonStatus.GetStartPosition();
		while( pos )
		{
			m_mpButtonStatus.GetNextAssoc( pos, wPChannel, pMyButton );
			m_mpButtonStatus.RemoveKey( wPChannel );
			delete pMyButton;
		}
	}
}

void CStatusToolbarHandler::RemoveAllStatusButtons( void )
{
	// Delete all buttons from the toolbar
	while( ::SendMessage( m_hWndToolbar, TB_BUTTONCOUNT, 0, 0 ) > 1 )
	{
		::SendMessage( m_hWndToolbar, TB_DELETEBUTTON, 1, 0 );
	}

	// Delete all button classes
	if( !m_mpButtonStatus.IsEmpty() )
	{
		CMyButton* pMyButton;
		WORD wPChannel;
		POSITION pos = m_mpButtonStatus.GetStartPosition();
		while( pos )
		{
			m_mpButtonStatus.GetNextAssoc( pos, wPChannel, pMyButton );
			m_mpButtonStatus.RemoveKey( wPChannel );
			pMyButton->DestroyWindow();
			delete pMyButton;
		}
	}

	// Mark all channels as unused
	if( g_pconductor
	&&	g_pconductor->m_pOutputTool )
	{
		g_pconductor->m_pOutputTool->MarkAllChannelsUnused();
	}
	// Send a WM_SIZE message to the Frame window (Producer's main window) to cause it to recalculate the
	// toolbar layout.
	RECT rect;
	::GetWindowRect( ::GetParent(::GetParent(m_hWndToolbar)), &rect );
	::PostMessage( ::GetParent(::GetParent(m_hWndToolbar)), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rect.right - rect.left, rect.bottom-rect.top ) );
}

void CStatusToolbarHandler::EnableTimer( BOOL fEnable )
{
	if( fEnable )
	{
		if( m_nTimerID == 0 )
		{
			//m_nTimerID = SetTimer( 1, 100, TimerProc );
			m_nTimerID = ::SetTimer( NULL, 0, 100, TimerProc );
		}
	}
	else
	{
		if( m_nTimerID )
		{
			KillTimer( m_nTimerID );
			m_nTimerID = 0;
		}
	}
}

void CALLBACK EXPORT TimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,  // timer identification
   DWORD dwTime    // system time
)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(nMsg);
	UNREFERENCED_PARAMETER(nIDEvent);
	UNREFERENCED_PARAMETER(dwTime);

	if( !g_pStatusToolbarHandler || !g_pconductor )
	{
		return;
	}

	POSITION pos;
	pos = g_pStatusToolbarHandler->m_mpButtonStatus.GetStartPosition();
	if( pos && g_pconductor->m_pOutputTool )
	{
		CMyButton* pMyButton;
		WORD wPChannel;
		BYTE bNewLevel;
		while( pos )
		{
			g_pStatusToolbarHandler->m_mpButtonStatus.GetNextAssoc( pos, wPChannel, pMyButton );
			if( pMyButton )
			{
				if( ::IsWindowVisible( pMyButton->GetSafeHwnd() ) )
				{
					//m_alPChannelNoteOnStates
					if( g_pconductor->m_pOutputTool->m_abPChannelNoteOn[wPChannel] == 1 )
					{
						bNewLevel = BYTE( max( g_pconductor->m_pOutputTool->m_alPChannelStates[wPChannel],
											   g_pconductor->m_pOutputTool->m_alPChannelNoteOnStates[wPChannel] ) / 8 );
						if( bNewLevel > 15 ) bNewLevel = 15;
						g_pconductor->m_pOutputTool->m_alPChannelNoteOnStates[wPChannel] = 0;
					}
					else if( g_pconductor->m_pOutputTool->m_alPChannelStates[wPChannel] > 0 )
					{
						bNewLevel = BYTE(g_pconductor->m_pOutputTool->m_alPChannelStates[wPChannel] / 8);
						if( bNewLevel > 15 ) bNewLevel = 15;
					}
					else
					{
						bNewLevel = 0;
					}
					if( (pMyButton->m_bLevel != bNewLevel)
					||	g_pconductor->m_pOutputTool->m_abPChannelNoteOn[wPChannel] )
					{
						pMyButton->m_bLevel = bNewLevel;
						if( g_pconductor->m_pOutputTool->m_abPChannelNoteOn[wPChannel] == 1 )
						{
							pMyButton->m_fNoteOn = TRUE;
							g_pconductor->m_pOutputTool->m_abPChannelNoteOn[wPChannel] = 2;
						}
						else
						{
							g_pconductor->m_pOutputTool->m_abPChannelNoteOn[wPChannel] = 0;
						}
						//pMyButton->Invalidate( FALSE );
						static const RECT rectInvalidate = { 4, 3, 20, 18 };
						pMyButton->InvalidateRect( &rectInvalidate, FALSE );
					}
				}
			}
		}
	}
}

LRESULT CStatusToolbarHandler::OnSize( WORD , NMHDR* , BOOL& )
{
	if( !m_mpButtonStatus.IsEmpty() )
	{
		CMyButton* pMyButton;
		WORD wPChannel;
		RECT rectNew, rectOrig;
		POSITION pos = m_mpButtonStatus.GetStartPosition();
		while( pos )
		{
			m_mpButtonStatus.GetNextAssoc( pos, wPChannel, pMyButton );
			if( pMyButton )
			{
				::SendMessage( m_hWndToolbar, TB_GETRECT, IDC_MUTE1 + wPChannel, (LPARAM)&rectNew );

				POINT ptNew = {rectNew.left, rectNew.top};
				::ClientToScreen( m_hWndToolbar, &ptNew );

				HWND hWndButton = pMyButton->GetSafeHwnd();
				::GetWindowRect( hWndButton, &rectOrig );

				if( (rectOrig.left != ptNew.x)
				||	(rectOrig.top != ptNew.y) )
				{
					long lWidth = rectOrig.right - rectOrig.left;
					long lHeight = rectOrig.bottom - rectOrig.top;

					::MoveWindow( hWndButton, rectNew.left, rectNew.top, lWidth, lHeight, TRUE );
				}
			}
		}
	}
	return 0;
}

CWndClassInfo& CStatusToolbarHandler::GetWndClassInfo()
{
	static CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, StartWindowProc,
		  0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW+1), 0, NULL, 0 },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}

/////////////////////////////////////////////////////////////////////////////
// CStatusToolbarHandler IDMUSProdToolBar::GetInfo

HRESULT CStatusToolbarHandler::GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle )
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
	strTitle.LoadString(IDS_STATUS_TITLE);
	*pbstrTitle = strTitle.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStatusToolbarHandler IDMUSProdToolBar::GetMenuText

HRESULT CStatusToolbarHandler::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CString strText;
	strText.LoadString(IDS_STATUS_MENUTEXT);
	*pbstrText = strText.AllocSysString();
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStatusToolbarHandler IDMUSProdToolBar::GetMenuHelpText

HRESULT CStatusToolbarHandler::GetMenuHelpText( BSTR* pbstrMenuHelpText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrMenuHelpText == NULL )
	{
		return E_POINTER;
	}

	CString strMenuHelpText;
	strMenuHelpText.LoadString(IDS_STATUS_HELP);
	*pbstrMenuHelpText = strMenuHelpText.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStatusToolbarHandler IDMUSProdToolBar::Initialize

HRESULT CStatusToolbarHandler::Initialize( HWND hWndToolBar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// We had better have a valid window handle
	ASSERT( hWndToolBar );
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
	pToolBarCtrl->SetButtonSize( CSize( 50, 22 ) );
	pToolBarCtrl->AddBitmap( 1, IDB_Status );
	pToolBarCtrl->SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(50, 22));

	// Initialize the TBBUTTON structure
	TBBUTTON button;
	button.dwData = 0;
	button.iString = NULL;

	// Force resisize to wrap controls correctly (TBSTYLE_BUTTON)
	button.iBitmap = 0;
	button.idCommand = ID_TRANSP_PANIC;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_BUTTON;
	pToolBarCtrl->InsertButton( 0, &button );

	pToolBarCtrl->Detach();
	delete pToolBarCtrl;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStatusToolbarHandler IDMUSProdToolBar8::ShowToolBar

HRESULT CStatusToolbarHandler::ShowToolBar( BOOL bShowToolBar )
{
	UNREFERENCED_PARAMETER(bShowToolBar);
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

LRESULT CStatusToolbarHandler::OnClicked( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(wCode);
	// Check if the command is for a button we created
	int nChannel = wLoWord - IDC_MUTE1;

	CMyButton* pMyButton;
	if( m_mpButtonStatus.Lookup( (WORD)nChannel, pMyButton ) )
	{
		// Flip its state
		pMyButton->m_fDown = !pMyButton->m_fDown;

		// If we have an output tool (we'd better), update it with the new mute state
		if( g_pconductor && g_pconductor->m_pOutputTool )
		{
			g_pconductor->m_pOutputTool->SetPChannelMute( nChannel, pMyButton->m_fDown );

			// If mute was just enabled, send an All-notes off message
			if( pMyButton->m_fDown )
			{
				// Send Reset All Controllers (121)
				g_pconductor->SendMIDIMessage( pMyButton->m_dwChannel, 0, MIDI_CCHANGE, 121 );

				// Send All Notes Off (123)
				g_pconductor->SendMIDIMessage( pMyButton->m_dwChannel, 0, MIDI_CCHANGE, 123 );
			}
		}

		// Redraw the button since its state changed
		pMyButton->Invalidate( FALSE );

		bHandled = TRUE;
		return 0;
	}

	// Not for us
	bHandled = FALSE;
	return -1;
}

LRESULT CStatusToolbarHandler::OnDrawItem(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& fHandled)
{
	UNREFERENCED_PARAMETER(nMsg);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// If the message is for one of the mute/activity buttons we created, pass it on
	int nChannel = LOWORD(wParam) - IDC_MUTE1;
	{
		CMyButton* pMyButton;
		if( m_mpButtonStatus.Lookup( (WORD)nChannel, pMyButton ) )
		{
			pMyButton->SendMessage( WM_DRAWITEM, wParam, lParam );

			fHandled = TRUE;
			return 0;
		}
	}

	// Not for us
	fHandled = FALSE;
	return -1;
}

LRESULT CStatusToolbarHandler::OnWindowPosChanged(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& fHandled)
{
	UNREFERENCED_PARAMETER(nMsg);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(wParam);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_nAfterWindowPosChanged = 2;

	fHandled = FALSE;
	return -1;
}

LRESULT CStatusToolbarHandler::OnAddButton( WORD /*wCode*/, WORD wPChannel, HWND /*hWnd*/, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMyButton *pMyButton;

	ASSERT( !m_mpButtonStatus.Lookup( wPChannel, pMyButton ) );
	if( !m_mpButtonStatus.Lookup( wPChannel, pMyButton ) )
	{
		Sleep(10);

		// Calculate where the button should be inserted
		int nIndex = 1;
		WORD wTmpPChannel = 0;
		while( wPChannel > wTmpPChannel )
		{
			if( m_mpButtonStatus.Lookup( wTmpPChannel, pMyButton ) )
			{
				nIndex++;
			}
			wTmpPChannel++;
		}

		nIndex = 1 + (nIndex - 1) * 2;

		// Initialize the TBBUTTON structure
		TBBUTTON button;
		button.dwData = 0;
		button.iString = NULL;

		// Insert separator
		button.iBitmap = 2;
		button.idCommand = 0;
		button.fsState = TBSTATE_ENABLED;
		button.fsStyle = TBSTYLE_SEP;
		::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, nIndex, (LPARAM)&button );

		// Add the button just after the the separator
		nIndex++;

		// Ensure the button size is correct
		::SendMessage(m_hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(50, 22));

		// Insert placeholder for button
		button.iBitmap = -1;
		button.idCommand = IDC_MUTE1 + wPChannel;
		button.fsState = TBSTATE_ENABLED;
		button.fsStyle = TBSTYLE_BUTTON;
		::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, nIndex, (LPARAM)&button );

		// Create and attach the button;
		TCHAR tcsText[10];
		RECT rect;

		pMyButton = new CMyButton( wPChannel );
		::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rect );
		_itot( wPChannel + 1, tcsText, 10 );

		if( !pMyButton->Create(tcsText, WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_OWNERDRAW,
			rect, CWnd::FromHandle( m_hWndToolbar ), IDC_MUTE1 + wPChannel) )
		{
			delete pMyButton;
			return -1;
		}

		// Initialize the output button with the mute state
		pMyButton->m_fDown = g_pconductor->m_pOutputTool->GetPChannelMute( wPChannel );

		pMyButton->EnableWindow(TRUE);

		m_mpButtonStatus.SetAt( wPChannel, pMyButton );

		// Update position of other buttons
		// Not needed - handled when we send a WM_SIZE message to the Frame window
		//OnSize( 0, 0, bHandled );

		// Not needed - handled when we send a WM_SIZE message to the Frame window
		//::PostMessage( m_hWndToolbar, TB_AUTOSIZE, 0, 0 );

		MSG msg;
		if( 0 == PeekMessage( &msg, m_hWndToolbar, WM_COMMAND, WM_COMMAND, PM_NOREMOVE ) )
		{
			// Send a WM_SIZE message to the Frame window (Producer's main window) to cause it to recalculate the
			// toolbar layout.
			Sleep(100);
			::GetWindowRect( ::GetParent(::GetParent(m_hWndToolbar)), &rect );
			::PostMessage( ::GetParent(::GetParent(m_hWndToolbar)), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rect.right - rect.left, rect.bottom-rect.top ) );
		}
	}
	return 0;
}

LRESULT CStatusToolbarHandler::OnDblClicked( WORD wCode, WORD wLoWord, HWND hWnd, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(wCode);
	// Ensure that we have an output tool
	ASSERT( g_pconductor && g_pconductor->m_pOutputTool );

	// Check if the command is for a button we created
	int nChannel = wLoWord - IDC_MUTE1;

	CMyButton* pMyButton;
	if( g_pconductor && g_pconductor->m_pOutputTool && m_mpButtonStatus.Lookup( (WORD)nChannel, pMyButton ) )
	{
		BOOL fOtherButtonState;
		if( pMyButton->m_fDown )
		{
			// Set its state to active
			pMyButton->m_fDown = FALSE;

			// Update the output tool with the new mute state
			g_pconductor->m_pOutputTool->SetPChannelMute( nChannel, FALSE );

			// Redraw the button since its state changed
			pMyButton->Invalidate( FALSE );

			if( m_mpButtonStatus.GetCount() < 2 )
			{
				bHandled = TRUE;
				return 0;
			}

			// Find out if all other buttons have the same state (and what it is)
			BOOL fFirstButtonStateSet = FALSE;
			BOOL fFirstButtonState = FALSE;
			BOOL fAllButtonsSame = TRUE;
			POSITION pos = m_mpButtonStatus.GetStartPosition();
			CMyButton *pTmpButton;
			WORD wChannel;
			while( pos && fAllButtonsSame )
			{
				m_mpButtonStatus.GetNextAssoc( pos, wChannel, pTmpButton );
				if( pTmpButton != pMyButton )
				{
					if( !fFirstButtonStateSet )
					{
						fFirstButtonStateSet = TRUE;
						fFirstButtonState = pTmpButton->m_fDown;
					}
					else
					{
						if( fFirstButtonState != pTmpButton->m_fDown )
						{
							fAllButtonsSame = FALSE;
						}
					}
				}
			}

			if( fAllButtonsSame && fFirstButtonState )
			{
				// All other buttons are muted - unmute all other buttons
				fOtherButtonState = FALSE;
			}
			else
			{
				// Mute all other buttons
				fOtherButtonState = TRUE;
			}
		}
		else
		{
			// Mute all other buttons
			fOtherButtonState = TRUE;
		}

		// Set all other buttons to fOtherButtonState
		CMyButton *pTmpButton;
		POSITION pos = m_mpButtonStatus.GetStartPosition();
		WORD wChannel;
		while( pos )
		{
			m_mpButtonStatus.GetNextAssoc( pos, wChannel, pTmpButton );
			if( (pTmpButton != pMyButton) && (pTmpButton->m_fDown != fOtherButtonState) )
			{
				// Set its state to muted (TRUE) or unmuted (FALSE)
				pTmpButton->m_fDown = fOtherButtonState;

				// Update the output tool with the new mute state
				g_pconductor->m_pOutputTool->SetPChannelMute( pTmpButton->m_dwChannel, fOtherButtonState );

				// Redraw the button since its state changed
				pTmpButton->Invalidate( FALSE );
			}
		}

		bHandled = TRUE;
		return 0;
	}

	// Not for us
	bHandled = FALSE;
	return -1;
}

LRESULT CStatusToolbarHandler::OnPanicClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_pconductor->StopAllNotesAndSegments();
	//EnableTimer(FALSE);

	return 0;
}

LRESULT CStatusToolbarHandler::OnRightClick( WORD wNotifyCode, NMHDR* pNMHDR, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Get the cursor position (To find out which button we've hit
	POINT ptScreen;
	BOOL  bResult = ::GetCursorPos( &ptScreen );
	ASSERT( bResult );
	if( !bResult )
	{
		return FALSE;
	}

	POINT ptClient = ptScreen;
	bResult = ::ScreenToClient( pNMHDR->hwndFrom, &ptClient );
	ASSERT( bResult );
	if( !bResult )
	{
		return FALSE;
	}

	if( ::IsWindow( m_hWndToolbar ) )
	{
		// Get the rect for the panic button
		RECT rect;
		::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 0, (LPARAM) &rect );

		// Check if the panic button was clicked
		if( ::PtInRect( &rect, ptClient ) )
		{
			// Yes - remove all buttons
			RemoveAllStatusButtons();

#ifdef _DEBUG
			DumpAudiopath( g_pconductor->m_pDMAudiopath );
#endif
		}
	}

	return TRUE;
}

LRESULT CStatusToolbarHandler::OnPanicDblClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( g_pconductor->m_pDMPerformance )
	{
		for( int i=0; i<32; i++)
		{
			for( BYTE bNote = 0; bNote < 127; bNote ++)
			{
				// Send Note Off
				g_pconductor->SendMIDIMessage( i, 0, MIDI_NOTEOFF, bNote );
			}
		}
		/*
		if( g_pconductor->m_pDMMIDIOutPort )
		{
			DWORD dwNumGroups;
			if( SUCCEEDED( g_pconductor->m_pDMMIDIOutPort->GetNumChannelGroups( &dwNumGroups ) ) )
			{
			}
		}
		if( g_pconductor->m_pDMSynthPort )
		{
			DWORD dwNumGroups;
			if( SUCCEEDED( g_pconductor->m_pDMSynthPort->GetNumChannelGroups( &dwNumGroups ) ) )
			{
			}
		}
		*/
	}

	return 0;
}

void CStatusToolbarHandler::RefreshAllButtons()
{
	POSITION pos = m_mpButtonStatus.GetStartPosition();
	CMyButton *pTmpButton;
	WORD wChannel;
	while( pos )
	{
		m_mpButtonStatus.GetNextAssoc( pos, wChannel, pTmpButton );
		pTmpButton->Invalidate( FALSE );
	}
}


BEGIN_MESSAGE_MAP(CMyButton, CBitmapButton)
	//{{AFX_MSG_MAP(CMyButton)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

long CMyButton::m_lFontRefCount = 0;
HFONT CMyButton::m_hFont = NULL;

HFONT CreateMyButtonFont( void )
{
	LOGFONT lf;
	ZeroMemory( &lf, sizeof(LOGFONT));
	lf.lfHeight = 18; // Max height, in pixels
	/*
	if( m_dwChannel > 98 )
	{
	*/
		lf.lfWidth = 5; // Max width, in pixels
	/*
	}
	else
	{
		lf.lfWidth = 7; // Max width, in pixels
	}
	*/
	//lf.lfEscapement = 0;
	//lf.lfOrientation = 0;
	lf.lfWeight = 400; // Weight, 0-1000, 400 = Normal, 700 = Bold
	//lf.lfItalic = FALSE;
	//lf.lfUnderline = FALSE;
	//lf.lfStrikeOut = FALSE;
	//lf.lfCharSet = ANSI_CHARSET;
	//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	//lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH  | FF_ROMAN;
	//memcpy( lf.lfFaceName, _T("Arial"), 31 );
	
	// Create the font
	return ::CreateFontIndirect( &lf );
}

CMyButton::CMyButton( DWORD dwPChannel ) : CBitmapButton()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Initialize to up (active), no activity, PChannel 0
	m_fDown = FALSE;
	m_bLevel = 0;
	m_dwChannel = dwPChannel;
	m_fNoteOn = FALSE;
	
	if( InterlockedIncrement( &m_lFontRefCount ) == 1 )
	{
		if( m_hFont == NULL )
		{
			m_hFont = CreateMyButtonFont();
		}
	}
}

CMyButton::CMyButton() : CBitmapButton()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Initialize to up (active), no activity, PChannel 0
	m_fDown = FALSE;
	m_bLevel = 0;
	m_dwChannel = 0;
	m_fNoteOn = FALSE;
	
	if( InterlockedIncrement( &m_lFontRefCount ) == 1 )
	{
		if( m_hFont == NULL )
		{
			m_hFont = CreateMyButtonFont();
		}
	}
}

CMyButton::~CMyButton()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( InterlockedDecrement( &m_lFontRefCount ) == 0 )
	{
		if( m_hFont )
		{
			::DeleteObject( m_hFont );
			m_hFont = 0;
		}
	}
}

bool GetPChannelName( DWORD dwPChannel, CString &strName )
{
	bool fResult = false;

	if( g_pconductor && g_pconductor->m_pFrameWork )
	{
		ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();
		if( pDisplayedRegEntry != NULL )
		{
			IDMUSProdTransport *pTransport;
			if( SUCCEEDED( pDisplayedRegEntry->GetTransport( &pTransport ) ) )
			{
				IDMUSProdNode *pNode;
				if( SUCCEEDED( pTransport->QueryInterface( IID_IDMUSProdNode, (void**)&pNode ) ) )
				{
					IDMUSProdProject *pProject;
					if( SUCCEEDED( g_pconductor->m_pFrameWork->FindProject( pNode, &pProject ) ) )
					{
						IDMUSProdPChannelName *pPChannelName;
						if( SUCCEEDED( pProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pPChannelName ) ) )
						{
							// Get the PChannel name
							WCHAR wszName[MAX_PATH];
							pPChannelName->GetPChannelName( dwPChannel, wszName );

							strName = wszName;

							fResult = true;

							pPChannelName->Release();
						}
						pProject->Release();
					}
					pNode->Release();
				}
				// GetTransport doesn't AddRef, do don't Release
				//pTransport->Release();
			}
		}
	}

	return fResult;
}

void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(lpDrawItemStruct != NULL);

	if( lpDrawItemStruct->itemAction != ODA_DRAWENTIRE )
	{
		return;
	}

	// Attach to the drawing context handle
	if( lpDrawItemStruct->hDC )
	{
		// Create a copy of the button's rectangle
		RECT rect = lpDrawItemStruct->rcItem;

		// Draw the outside rect (typically White and Black)
		if( m_fDown )
		{
			::DrawFrameControl( lpDrawItemStruct->hDC, &rect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED );
		}
		else
		{
			::DrawFrameControl( lpDrawItemStruct->hDC, &rect, DFC_BUTTON, DFCS_BUTTONPUSH );
		}

		// Draw the inside rect (typically light grey and dark grey)
		// Add a two pixel gap at the top and bottom of the button
		rect.left++;
		rect.right--;
		rect.top += 3;
		rect.bottom -= 3;

		{
			// Set up our drawing rect, should be (4,4) - (20,17) (x,y)
			RECT rectMeter = rect;
			rectMeter.left += 2;
			rectMeter.right = 20;

			/*
			// Fill the background Black (active), White (inactive, down)
			if( m_fDown )
			{
				::SetBkColor(lpDrawItemStruct->hDC, RGB(255, 255, 255));
				::ExtTextOut(lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &rectMeter, NULL, 0, NULL);
			}
			else
			{
				::SetBkColor(lpDrawItemStruct->hDC, RGB(0, 0, 0));
				::ExtTextOut(lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &rectMeter, NULL, 0, NULL);
			}
			*/

			// If we're not 0
			if( m_bLevel )
			{
				// Leave a one pixel border around the volume bars
				rectMeter.left++;
				rectMeter.right--;
				rectMeter.bottom--;

				// Draw the volume bars (COLOR_BTNFACE - inactive, Yellow - active)
				if( m_fDown )
				{
					rectMeter.top = rectMeter.bottom - m_bLevel;
					::FillRect( lpDrawItemStruct->hDC, &rectMeter, (HBRUSH) (COLOR_BTNFACE + 1) );
					/*
					for( int nIndex = m_bLevel; nIndex > 0; nIndex-- )
					{
						rectMeter.top = rectMeter.bottom - 2;
						::FillRect( lpDrawItemStruct->hDC, &rectMeter, (HBRUSH) (COLOR_BTNFACE + 1) );
						rectMeter.bottom = rectMeter.top - 1;
					}
					*/
				}
				else
				{
					if( m_fNoteOn )
					{
						::SetBkColor(lpDrawItemStruct->hDC, RGB(255, 255, 0));
						m_fNoteOn = FALSE;
					}
					else
					{
						::SetBkColor(lpDrawItemStruct->hDC, RGB(235, 235, 0));
					}

					rectMeter.top = rectMeter.bottom - m_bLevel;
					::ExtTextOut(lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &rectMeter, NULL, 0, NULL);
					/*
					for( int nIndex = m_bLevel; nIndex > 0; nIndex-- )
					{
						rectMeter.top = rectMeter.bottom - 2;
						::ExtTextOut(lpDrawItemStruct->hDC, 0, 0, ETO_OPAQUE, &rectMeter, NULL, 0, NULL);
						rectMeter.bottom = rectMeter.top - 1;
					}
					*/
				}
			}
		}

		// Draw the PChannel number for this button
		{
			if( m_hFont )
			{
				HFONT oldFont;
				oldFont = (HFONT)::SelectObject( lpDrawItemStruct->hDC, m_hFont );

				// Set up our drawing rect, should be (4,4) - (20,17) (x,y)
				RECT tempRect = rect;
				tempRect.left += 2;
				tempRect.right = 20;

				// We want to draw transparently
				int nOldBkMode;
				nOldBkMode = ::SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );

				// Convert from a number to a string
				char cstrName[10];
				_itoa( m_dwChannel + 1, cstrName, 10 );

				// Black if active (up), White if muted (down)
				COLORREF oldColor;
				if( m_fDown )
				{
					oldColor = ::SetTextColor( lpDrawItemStruct->hDC, RGB(255, 255, 255) );
				}
				else
				{
					oldColor = ::SetTextColor( lpDrawItemStruct->hDC, RGB(0, 0, 0) );
				}

				// Actually draw the text
				::DrawText( lpDrawItemStruct->hDC, cstrName, strlen(cstrName), &tempRect,
					DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOPREFIX );

				// Reset everything back to how it was before
				::SetTextColor( lpDrawItemStruct->hDC, oldColor );
				::SetBkMode( lpDrawItemStruct->hDC, nOldBkMode );
				::SelectObject( lpDrawItemStruct->hDC, oldFont );
			}

			// Set up our drawing rect, should be (22,4) - (20,17) (x,y)
			RECT tempRect = rect;
			tempRect.left = 22;
			tempRect.right -= 2;

			// Get a copy of the rect that was invalidated
			RECT rectClip;
			::GetClipBox( lpDrawItemStruct->hDC, &rectClip );

			// Check if we need to draw the PChannel name
			CRect rectInter;
			if( rectInter.IntersectRect( &tempRect, &rectClip ) )
			{
			// Yep - need to draw the PChannel name
			if( m_hFont )
			{
				HFONT oldFont;
				oldFont = (HFONT)::SelectObject( lpDrawItemStruct->hDC, m_hFont );

				// We want to draw transparently
				int nOldBkMode;
				nOldBkMode = ::SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );

				// Black if active (up), White if muted (down)
				COLORREF oldColor;
				if( m_fDown )
				{
					oldColor = ::SetTextColor( lpDrawItemStruct->hDC, RGB(255, 255, 255) );
				}
				else
				{
					oldColor = ::SetTextColor( lpDrawItemStruct->hDC, RGB(0, 0, 0) );
				}

				CString strPChannelName;
				if( GetPChannelName( m_dwChannel, strPChannelName ) )
				{
					// Actually draw the text
					::DrawText( lpDrawItemStruct->hDC, strPChannelName, strPChannelName.GetLength(), &tempRect,
						DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX );
				}

				// Reset everything back to how it was before
				::SetTextColor( lpDrawItemStruct->hDC, oldColor );
				::SetBkMode( lpDrawItemStruct->hDC, nOldBkMode );
				::SelectObject( lpDrawItemStruct->hDC, oldFont );
			}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMyButton message handlers

void CMyButton::OnRButtonUp(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CBitmapButton::OnRButtonUp(nFlags, point);

	if( g_pconductor && g_pconductor->m_pFrameWork )
	{
		ITransportRegEntry *pDisplayedRegEntry = g_pconductor->GetDisplayedTransport();
		if( pDisplayedRegEntry != NULL )
		{
			IDMUSProdTransport *pTransport;
			if( SUCCEEDED( pDisplayedRegEntry->GetTransport( &pTransport ) ) )
			{
				IDMUSProdNode *pNode;
				if( SUCCEEDED( pTransport->QueryInterface( IID_IDMUSProdNode, (void**)&pNode ) ) )
				{
					IDMUSProdProject *pProject;
					if( SUCCEEDED( g_pconductor->m_pFrameWork->FindProject( pNode, &pProject ) ) )
					{
						IDMUSProdPChannelName *pPChannelName;
						if( SUCCEEDED( pProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pPChannelName ) ) )
						{
							DlgEditPChannel dlgEditPChannel;
							dlgEditPChannel.m_dwPChannel = m_dwChannel;

							// Get the PChannel name
							WCHAR wszName[MAX_PATH];
							pPChannelName->GetPChannelName( m_dwChannel, wszName );
							CString strOrigName = wszName;
							dlgEditPChannel.m_strName = strOrigName;

							if( (dlgEditPChannel.DoModal() == IDOK)
							&&	(dlgEditPChannel.m_strName.Compare( strOrigName ) != 0) )
							{
								// Update PChannel name
								MultiByteToWideChar( CP_ACP, 0, dlgEditPChannel.m_strName, -1, wszName, MAX_PATH );
								pPChannelName->SetPChannelName( m_dwChannel, wszName );
							}

							pPChannelName->Release();
						}
						pProject->Release();
					}
					pNode->Release();
				}
				// GetTransport doesn't AddRef, do don't Release
				//pTransport->Release();
			}
		}
	}
}


#ifdef _DEBUG
void OutputAudiopathText( TCHAR *tcstrText )
{
	TRACE("%s", tcstrText);
}

void DumpAudiopathSegment( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectMusicSegment* pIDirectMusicSegment = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( 0, DMUS_PATH_SEGMENT, 0,
		GUID_All_Objects, dwIndex, IID_IDirectMusicSegment, (void**) &pIDirectMusicSegment ) )
	{
		_stprintf( tcstrText, _T("Found segment %x.\n"), pIDirectMusicSegment );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectMusicSegment->Release();
	}

	if( dwIndex == 0 )
	{
		OutputAudiopathText( _T("No segments in audio path.\n") );
	}
}

void DumpAudiopathSegmentTrack( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectMusicTrack* pIDirectMusicTrack = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( 0, DMUS_PATH_SEGMENT_TRACK, 0,
		GUID_All_Objects, dwIndex, IID_IDirectMusicTrack, (void**) &pIDirectMusicTrack ) )
	{
		_stprintf( tcstrText, _T("Found track %x.\n"), pIDirectMusicTrack );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectMusicTrack->Release();
	}

	if( dwIndex == 0 )
	{
		OutputAudiopathText( _T("No tracks in audio path.\n") );
	}
}

void DumpAudiopathAudiopath( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectMusicAudioPath* pIDirectMusicAudiopathTmp = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( 0, DMUS_PATH_AUDIOPATH, 0,
		GUID_All_Objects, dwIndex, IID_IDirectMusicAudioPath, (void**) &pIDirectMusicAudiopathTmp ) )
	{
		_stprintf( tcstrText, _T("Found audio path %x.\n"), pIDirectMusicAudiopathTmp );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectMusicAudiopathTmp->Release();
	}

	if( dwIndex == 0 )
	{
		OutputAudiopathText( _T("No audio paths in audio path.\n") );
	}
}

void DumpAudiopathPerformance( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectMusicPerformance* pIDirectMusicPerformance = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( 0, DMUS_PATH_PERFORMANCE, 0,
		GUID_All_Objects, dwIndex, IID_IDirectMusicPerformance, (void**) &pIDirectMusicPerformance ) )
	{
		_stprintf( tcstrText, _T("Found performance %x.\n"), pIDirectMusicPerformance );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectMusicPerformance->Release();
	}

	if( dwIndex == 0 )
	{
		OutputAudiopathText( _T("No performances in audio path.\n") );
	}
}

void DumpAudiopathPort( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR tcstrText[256];
	DMUS_PORTCAPS dmPortCaps;
	DWORD dwIndex = 0;
	IDirectMusicPort* pIDirectMusicPort = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0,
		GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void**) &pIDirectMusicPort ) )
	{
		ZeroMemory( &dmPortCaps, sizeof(DMUS_PORTCAPS) );
		dmPortCaps.dwSize = sizeof(DMUS_PORTCAPS);
		pIDirectMusicPort->GetCaps( &dmPortCaps );
		_stprintf( tcstrText, _T("Found port %x %S.\n"), pIDirectMusicPort, dmPortCaps.wszDescription );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectMusicPort->Release();
	}

	if( dwIndex == 0 )
	{
		OutputAudiopathText( _T("No ports in audio path.\n") );
	}
}

/*
void DumpAudiopathSink( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IUnknown* pIDirectSoundSink = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_SINK, 0,
		GUID_All_Objects, dwIndex, IID_IUnknown, (void**) &pIDirectSoundSink ) )
	{
		_stprintf( tcstrText, _T("Found sink %x.\n"), pIDirectSoundSink );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectSoundSink->Release();
	}

	if( dwIndex == 0 )
	{
		OutputAudiopathText( _T("No sinks in audio path.\n") );
	}
}
*/

void DumpAudiopathGraph( DWORD dwStage, IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR *tcstrType = NULL;

	switch( dwStage )
	{
	case DMUS_PATH_SEGMENT_GRAPH:
		tcstrType = _T("segment");
		break;

	case DMUS_PATH_AUDIOPATH_GRAPH:
		tcstrType = _T("audiopath");
		break;

	case DMUS_PATH_PERFORMANCE_GRAPH:
		tcstrType = _T("performance");
		break;

	default:
		tcstrType = _T("unknown");
		break;
	}

	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectMusicGraph* pIDirectMusicGraph = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( 0, dwStage, 0,
		GUID_All_Objects, dwIndex, IID_IDirectMusicGraph, (void**) &pIDirectMusicGraph ) )
	{
		_stprintf( tcstrText, _T("Found %s graph %x.\n"), tcstrType, pIDirectMusicGraph );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectMusicGraph->Release();
	}

	if( dwIndex == 0 )
	{
		_stprintf( tcstrText, _T("No %s graphs in audio path.\n"), tcstrType );
		OutputAudiopathText( tcstrText );
	}
}

void DumpAudiopathTool( DWORD dwStage, IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR *tcstrType = NULL;

	switch( dwStage )
	{
	case DMUS_PATH_SEGMENT_TOOL:
		tcstrType = _T("segment");
		break;

	case DMUS_PATH_AUDIOPATH_TOOL:
		tcstrType = _T("audio path");
		break;

	case DMUS_PATH_PERFORMANCE_TOOL:
		tcstrType = _T("performance");
		break;

	default:
		ASSERT(FALSE);
		return;
	}

	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectMusicTool* pIDirectMusicTool = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, dwStage, 0,
		GUID_All_Objects, dwIndex, IID_IDirectMusicTool, (void**) &pIDirectMusicTool ) )
	{
		_stprintf( tcstrText, _T("Found %s tool %x.\n"), tcstrType, pIDirectMusicTool );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIDirectMusicTool->Release();
	}

	if( dwIndex == 0 )
	{
		_stprintf( tcstrText, _T("No %s tools in audio path.\n"), tcstrType );
		OutputAudiopathText( tcstrText );
	}
}

void DumpAudiopathBufferDMO( DWORD dwStage, DWORD dwBufferIndex, IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR *tcstrType = NULL;

	DWORD dwPChannel;
	switch( dwStage )
	{
	case DMUS_PATH_BUFFER_DMO:
		tcstrType = _T("standard");
		dwPChannel = DMUS_PCHANNEL_ALL;
		break;

	case DMUS_PATH_MIXIN_BUFFER_DMO:
		tcstrType = _T("mixin");
		dwPChannel = 0;
		break;
	default:
		ASSERT(FALSE);
		return;
	}

	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IUnknown* pIUnknown = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( dwPChannel, dwStage, dwBufferIndex,
		GUID_All_Objects, dwIndex, IID_IUnknown, (void**) &pIUnknown ) )
	{
		_stprintf( tcstrText, _T("Found %s buffer dmo %x.\n"), tcstrType, pIUnknown );
		OutputAudiopathText( tcstrText );
		dwIndex++;
		pIUnknown->Release();
	}

	if( dwIndex == 0 )
	{
		_stprintf( tcstrText, _T("No %s buffer dmos in audio path.\n"), tcstrType );
		OutputAudiopathText( tcstrText );
	}
}

void DumpAudiopathBuffer( DWORD dwStage, IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR *tcstrType = NULL;

	DWORD dwPChannel;
	switch( dwStage )
	{
	case DMUS_PATH_BUFFER:
		tcstrType = _T("standard");
		dwPChannel = DMUS_PCHANNEL_ALL;
		break;

	case DMUS_PATH_MIXIN_BUFFER:
		tcstrType = _T("mixin");
		dwPChannel = 0;
		break;

	case DMUS_PATH_PRIMARY_BUFFER:
		tcstrType = _T("primary");
		dwPChannel = 0;
		break;

	default:
		ASSERT(FALSE);
		return;
	}

	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectSoundBuffer* pIDirectSoundBuffer = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( dwPChannel, dwStage, dwIndex,
		GUID_All_Objects, 0, IID_IDirectSoundBuffer, (void**) &pIDirectSoundBuffer ) )
	{
		_stprintf( tcstrText, _T("Found %s buffer %x.\n"), tcstrType, pIDirectSoundBuffer );
		OutputAudiopathText( tcstrText );

		if( dwStage == DMUS_PATH_BUFFER )
		{
			DumpAudiopathBufferDMO( DMUS_PATH_BUFFER_DMO, dwIndex, pIDirectMusicAudiopath );
		}
		else if( dwStage == DMUS_PATH_MIXIN_BUFFER )
		{
			DumpAudiopathBufferDMO( DMUS_PATH_MIXIN_BUFFER_DMO, dwIndex, pIDirectMusicAudiopath );
		}

		dwIndex++;
		pIDirectSoundBuffer->Release();
	}

	if( dwIndex == 0 )
	{
		_stprintf( tcstrText, _T("No %s buffers in audio path.\n"), tcstrType );
		OutputAudiopathText( tcstrText );
	}
}

void DumpAudiopathPrimaryBuffer( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	TCHAR tcstrText[256];
	DWORD dwIndex = 0;
	IDirectSoundBuffer* pIDirectSoundBuffer = NULL;
	while( S_OK == pIDirectMusicAudiopath->GetObjectInPath( 0, DMUS_PATH_PRIMARY_BUFFER, 0,
		GUID_All_Objects, dwIndex, IID_IDirectSoundBuffer, (void**) &pIDirectSoundBuffer ) )
	{
		_stprintf( tcstrText, _T("Found primary buffer %x.\n"), pIDirectSoundBuffer );
		OutputAudiopathText( tcstrText );

		dwIndex++;
		pIDirectSoundBuffer->Release();
	}

	if( dwIndex == 0 )
	{
		OutputAudiopathText( _T("No primary buffers in audio path.\n") );
	}
}

void DumpAudiopath( IDirectMusicAudioPath *pIDirectMusicAudiopath )
{
	ASSERT( pIDirectMusicAudiopath );
	if( !pIDirectMusicAudiopath )
	{
		OutputAudiopathText( _T("NULL pointer passed for audio path.\n") );
		return;
	}

	DumpAudiopathSegment( pIDirectMusicAudiopath );
	DumpAudiopathSegmentTrack( pIDirectMusicAudiopath );
	DumpAudiopathGraph( DMUS_PATH_SEGMENT_GRAPH, pIDirectMusicAudiopath );
	DumpAudiopathTool( DMUS_PATH_SEGMENT_TOOL, pIDirectMusicAudiopath );
	DumpAudiopathAudiopath( pIDirectMusicAudiopath );
	DumpAudiopathGraph( DMUS_PATH_AUDIOPATH_GRAPH, pIDirectMusicAudiopath );
	DumpAudiopathTool( DMUS_PATH_AUDIOPATH_TOOL, pIDirectMusicAudiopath );
	DumpAudiopathPerformance( pIDirectMusicAudiopath );
	DumpAudiopathGraph( DMUS_PATH_PERFORMANCE_GRAPH, pIDirectMusicAudiopath );
	DumpAudiopathTool( DMUS_PATH_PERFORMANCE_TOOL, pIDirectMusicAudiopath );
	DumpAudiopathPort( pIDirectMusicAudiopath );
	//DumpAudiopathSink( pIDirectMusicAudiopath );
	DumpAudiopathBuffer( DMUS_PATH_BUFFER, pIDirectMusicAudiopath ); // Also dumps DMOs
	DumpAudiopathBuffer( DMUS_PATH_MIXIN_BUFFER, pIDirectMusicAudiopath ); // Also dumps DMOs
	DumpAudiopathPrimaryBuffer( pIDirectMusicAudiopath );
}
#endif