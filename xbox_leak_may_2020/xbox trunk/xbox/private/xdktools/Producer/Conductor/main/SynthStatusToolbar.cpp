// SynthStatusToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "cconduct.h"
#include "SynthStatusToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CSynthStatusToolbar *g_pSynthStatusToolbar = NULL;

static CString s_strCPUFormat;
static CString s_strVoicesFormat;
static CString s_strMemoryFormat;
static CString s_strLatencyFormat;

static DWORD s_adwLatency[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static DWORD s_dwLatencyIndex = 0;
static DWORD s_dwLatencyCount = 0;


void RefTimeToString( REFERENCE_TIME rtTime, CString &cstrTime )
{
	bool fNegative = false;
	if( rtTime < 0 )
	{
		fNegative = true;
		rtTime = -rtTime;
	}

	int iMillisecond, iSecond, iMinute, iHour;
	// Convert to milliseconds
	iMillisecond = int(rtTime / 10000);
	iSecond = iMillisecond / 1000;
	iMillisecond %= 1000;
	iMinute = iSecond / 60;
	iSecond %= 60;
	iHour = iMinute / 60;
	iMinute %= 60;

	cstrTime.Format("%02d:%02d:%02d.%03d", iHour, iMinute, iSecond, iMillisecond);

	if( fNegative )
	{
		cstrTime = CString("-") + cstrTime;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CResetButton

CResetButton::CResetButton()
{
}

CResetButton::~CResetButton()
{
}


BEGIN_MESSAGE_MAP(CResetButton, CButton)
	//{{AFX_MSG_MAP(CResetButton)
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResetButton message handlers

void CResetButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);

	UpdateStyle();
}

void CResetButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonUp(nFlags, point);

	UpdateStyle();
}

void CResetButton::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
	UpdateStyle();
}

void CResetButton::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CButton::OnActivate(nState, pWndOther, bMinimized);
	
	UpdateStyle();
}

void CResetButton::UpdateStyle( void )
{
	ModifyStyle( BS_DEFPUSHBUTTON, BS_PUSHBUTTON, 0 );
}


/////////////////////////////////////////////////////////////////////////////
// CSynthStatusToolbar

CSynthStatusToolbar::CSynthStatusToolbar()
{
	m_nTimerID = 0;
	m_hWndToolbar = NULL;
	g_pSynthStatusToolbar = this;
	m_dwPeakCPU = 0;
	m_dwPeakVoices = 0;
	m_rtPeakCPU = 0;
	m_rtPeakVoices = 0;
	if( s_strCPUFormat.IsEmpty() )
	{
		if( !s_strCPUFormat.LoadString( IDS_SYNTH_CPU_STATUS ) )
		{
			s_strCPUFormat = _T("CPU %02d.%1.1d%% Peak %02d.%1.1d%% at %s");
		}
	}
	if( s_strVoicesFormat.IsEmpty() )
	{
		if( !s_strVoicesFormat.LoadString( IDS_SYNTH_VOICES_STATUS ) )
		{
			s_strVoicesFormat = _T("Voices %d Peak %d at %s");
		}
	}
	if( s_strMemoryFormat.IsEmpty() )
	{
		if( !s_strMemoryFormat.LoadString( IDS_SYNTH_MEMORY_STATUS ) )
		{
			s_strMemoryFormat = _T("Memory Use %dK");
		}
	}
	if( s_strLatencyFormat.IsEmpty() )
	{
		if( !s_strLatencyFormat.LoadString( IDS_SYNTH_LATENCY_STATUS ) )
		{
			s_strLatencyFormat = _T("Latency %dms");
		}
	}
}

CSynthStatusToolbar::~CSynthStatusToolbar()
{
	g_pSynthStatusToolbar = NULL;
	if( m_nTimerID )
	{
		// No need to kill the timer - it was stopped when the window was destroyed.
	//	KillTimer( m_nTimerID );
		m_nTimerID = 0;
	}
	if( m_font.GetSafeHandle() )
	{
		m_font.DeleteObject();
	}
}

void CSynthStatusToolbar::EnableTimer( BOOL fEnable )
{
	if( fEnable )
	{
		if( m_nTimerID == 0 )
		{
			// Synth stats are only refreshed twice a second - we refresh twice as fast to
			// try and get a more accurate reference to the point in time where the highs occurred.
			m_nTimerID = ::SetTimer( NULL, 0, 500, SynthStatusTimerProc );
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


void CALLBACK EXPORT SynthStatusTimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,  // timer identification
   DWORD dwTime    // system time
)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(nMsg);
	UNREFERENCED_PARAMETER(nIDEvent);
	UNREFERENCED_PARAMETER(dwTime);

	// Verify our pointers to the toolbar and conductor
	if( !g_pSynthStatusToolbar || !g_pconductor )
	{
		return;
	}

	// Copy the Conductor's pointer to the audio path
	::EnterCriticalSection( &g_pconductor->m_csAudiopath );
	IDirectMusicAudioPath *pDMAudiopath = g_pconductor->m_pDMAudiopath;
	if( pDMAudiopath )
	{
		pDMAudiopath->AddRef();
	}
	::LeaveCriticalSection( &g_pconductor->m_csAudiopath );

	DWORD dwTotalCPU = 0;
	DWORD dwVoices = 0;
	DWORD dwMemUse = 0;
	DWORD dwLatency = 0;

	// Iterate through the default Audiopath's port
	DMUS_SYNTHSTATS8 synthStats;
	DWORD dwIndex = 0;
	IDirectMusicPort *pDMPort = NULL;
	if( pDMAudiopath )
	{
		while( S_OK == pDMAudiopath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, GUID_All_Objects, dwIndex, IID_IDirectMusicPort, (void**)&pDMPort ) )
		{
			// Initialize the DMUS_SYNTHSTATS8 structure
			ZeroMemory( &synthStats, sizeof( DMUS_SYNTHSTATS8 ) );
			synthStats.dwSize = sizeof( DMUS_SYNTHSTATS8 );

			// Try and get the stats for this port
			if( SUCCEEDED( pDMPort->GetRunningStats( (LPDMUS_SYNTHSTATS)&synthStats ) ) )
			{
				// Add this port's stats to the total
				if( synthStats.dwValidStats & DMUS_SYNTHSTATS_TOTAL_CPU )
				{
					dwTotalCPU += synthStats.dwTotalCPU;
				}
				if( synthStats.dwValidStats & DMUS_SYNTHSTATS_VOICES )
				{
					dwVoices += synthStats.dwVoices;
				}
				// BUGBUG: Replace with the real value when it's added to dmusicc.h
				//if( synthStats.dwValidStats & DMUS_SYNTHSTATS_FREE_MEMORY )
				{
					dwMemUse += synthStats.dwSynthMemUse;
				}
			}

			// Release the port
			pDMPort->Release();

			// Go on to the next port in the audio path
			dwIndex++;
		}
		pDMAudiopath->Release();
	}

	if( g_pSynthStatusToolbar->m_dwPeakCPU < dwTotalCPU )
	{
		g_pSynthStatusToolbar->m_dwPeakCPU = dwTotalCPU;

		IDirectMusicSegmentState *pSegState;
		REFERENCE_TIME rtTimeNow;
		MUSIC_TIME mtTimeNow;
		if( g_pconductor->m_pDMPerformance
		&&	SUCCEEDED( g_pconductor->m_pDMPerformance->GetTime( &rtTimeNow, &mtTimeNow ) )
		&&	SUCCEEDED( g_pconductor->m_pDMPerformance->GetSegmentState( &pSegState, mtTimeNow ) ) )
		{
			MUSIC_TIME mtStartTime;
			if( SUCCEEDED( pSegState->GetStartTime( &mtStartTime ) ) )
			{
				REFERENCE_TIME rtSegmentStart;
				VERIFY( SUCCEEDED( g_pconductor->m_pDMPerformance->MusicToReferenceTime( mtStartTime, &rtSegmentStart ) ) );
				g_pSynthStatusToolbar->m_rtPeakCPU = rtTimeNow - rtSegmentStart;
			}
			pSegState->Release();
		}
	}

	if( (dwVoices < 20000) && (g_pSynthStatusToolbar->m_dwPeakVoices < dwVoices) )
	{
		g_pSynthStatusToolbar->m_dwPeakVoices = dwVoices;

		IDirectMusicSegmentState *pSegState;
		REFERENCE_TIME rtTimeNow;
		MUSIC_TIME mtTimeNow;
		if( g_pconductor->m_pDMPerformance
		&&	SUCCEEDED( g_pconductor->m_pDMPerformance->GetTime( &rtTimeNow, &mtTimeNow ) )
		&&	SUCCEEDED( g_pconductor->m_pDMPerformance->GetSegmentState( &pSegState, mtTimeNow ) ) )
		{
			MUSIC_TIME mtStartTime;
			if( SUCCEEDED( pSegState->GetStartTime( &mtStartTime ) ) )
			{
				REFERENCE_TIME rtSegmentStart;
				VERIFY( SUCCEEDED( g_pconductor->m_pDMPerformance->MusicToReferenceTime( mtStartTime, &rtSegmentStart ) ) );
				g_pSynthStatusToolbar->m_rtPeakVoices = rtTimeNow - rtSegmentStart;
			}
			pSegState->Release();
		}
	}

	if( g_pconductor->m_pDMPerformance )
	{
		long lCount = 20;
		REFERENCE_TIME rtNow = 0, rtLatency = 0, rtNowNext = 20000;
		while( (lCount != 0) && (rtNowNext - rtNow > 10000) )
		{
			g_pconductor->m_pDMPerformance->GetTime( &rtNow, NULL );
			g_pconductor->m_pDMPerformance->GetLatencyTime( &rtLatency );
			g_pconductor->m_pDMPerformance->GetTime( &rtNowNext, NULL );
			lCount--;
		}

		if( lCount != 0 )
		{
			dwLatency = DWORD((rtLatency - rtNow + 5000) / 10000);

			if( s_dwLatencyCount < 20 )
			{
				s_adwLatency[s_dwLatencyCount] = dwLatency;
				s_dwLatencyCount++;
			}
			else
			{
				s_dwLatencyIndex++;
				if( s_dwLatencyIndex > 19 )
				{
					s_dwLatencyIndex = 0;
				}
				s_adwLatency[s_dwLatencyIndex] = dwLatency;
			}

			dwLatency = 0;
			for( lCount = s_dwLatencyCount - 1; lCount >= 0; lCount-- )
			{
				dwLatency += s_adwLatency[lCount];
			}
			dwLatency /= s_dwLatencyCount;
		}
	}

	CString strTime;
	RefTimeToString( g_pSynthStatusToolbar->m_rtPeakCPU, strTime );

	TCHAR tcstrText[100];
	_stprintf( tcstrText, s_strCPUFormat,
		dwTotalCPU / 100, (dwTotalCPU % 100) / 10,
		g_pSynthStatusToolbar->m_dwPeakCPU / 100, (g_pSynthStatusToolbar->m_dwPeakCPU % 100) / 10,
		strTime );
	::SendMessage( g_pSynthStatusToolbar->m_staticCPUText.GetSafeHwnd(), WM_SETTEXT, 0, (LPARAM)(LPCTSTR)tcstrText );


	RefTimeToString( g_pSynthStatusToolbar->m_rtPeakVoices, strTime );

	_stprintf( tcstrText, s_strVoicesFormat,
		dwVoices, g_pSynthStatusToolbar->m_dwPeakVoices, strTime );
	::SendMessage( g_pSynthStatusToolbar->m_staticVoicesText.GetSafeHwnd(), WM_SETTEXT, 0, (LPARAM)(LPCTSTR)tcstrText );

	_stprintf( tcstrText, s_strMemoryFormat, dwMemUse >> 10 ); // Divide by 1024
	::SendMessage( g_pSynthStatusToolbar->m_staticMemoryText.GetSafeHwnd(), WM_SETTEXT, 0, (LPARAM)(LPCTSTR)tcstrText );

	_stprintf( tcstrText, s_strLatencyFormat, dwLatency );
	::SendMessage( g_pSynthStatusToolbar->m_staticLatencyText.GetSafeHwnd(), WM_SETTEXT, 0, (LPARAM)(LPCTSTR)tcstrText );
}

/////////////////////////////////////////////////////////////////////////////
// CSynthStatusToolbar IDMUSProdToolBar::GetInfo

HRESULT CSynthStatusToolbar::GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle )
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

	CComBSTR bstrMenuHelpText;
	if( bstrMenuHelpText.LoadString( IDS_SYNTHSTATUS_TITLE ) )
	{
		*pbstrTitle = bstrMenuHelpText.Detach();
	}
	else
	{
		*pbstrTitle = NULL;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSynthStatusToolbar IDMUSProdToolBar::GetMenuText

HRESULT CSynthStatusToolbar::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CComBSTR bstrMenuHelpText;
	if( bstrMenuHelpText.LoadString( IDS_SYNTHSTATUS_MENUTEXT ) )
	{
		*pbstrText = bstrMenuHelpText.Detach();
		return S_OK;
	}
	else
	{
		*pbstrText = NULL;
		return E_FAIL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSynthStatusToolbar IDMUSProdToolBar::GetMenuHelpText

HRESULT CSynthStatusToolbar::GetMenuHelpText( BSTR* pbstrMenuHelpText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pbstrMenuHelpText == NULL )
	{
		return E_POINTER;
	}

	CComBSTR bstrMenuHelpText;
	if( bstrMenuHelpText.LoadString( IDS_SYNTHSTATUS_HELP ) )
	{
		*pbstrMenuHelpText = bstrMenuHelpText.Detach();
		return S_OK;
	}
	else
	{
		*pbstrMenuHelpText = NULL;
		return E_FAIL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSynthStatusToolbar IDMUSProdToolBar::Initialize

HRESULT CSynthStatusToolbar::Initialize( HWND hWndToolBar )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( hWndToolBar == NULL )
	{
		return E_INVALIDARG;
	}
	m_hWndToolbar = hWndToolBar;
 
	// Create font for toolbar text
	CClientDC* pDC = new CClientDC( CWnd::FromHandle( m_hWndToolbar) );

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
	int cxChar =  tm.tmAveCharWidth;
	//int cyChar = tm.tmHeight + tm.tmExternalLeading;
	pDC->SelectObject( pOldFont );
	delete pDC;

	::SendMessage( m_hWndToolbar, TB_SETMAXTEXTROWS, 1, 0 );
	::SendMessage( m_hWndToolbar, TB_SETDRAWTEXTFLAGS, DT_LEFT | DT_SINGLELINE | DT_TOP, DT_LEFT | DT_SINGLELINE | DT_TOP);

	TBBUTTON button;
	button.dwData = 0;
	button.iString = 1;

	// 8 - Wide separator for reset button
	button.iBitmap = cxChar * 8;
	button.idCommand = IDC_SYNTH_RESET;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 7 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 6 - Wide separator for latency
	button.iBitmap = cxChar * 20;
	button.idCommand = IDC_SYNTH_LATENCY;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 5 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 4 - Wide separator for memory use
	button.iBitmap = cxChar * 25;
	button.idCommand = IDC_SYNTH_MEMORY;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 3 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 2 - Wide separator for number of voices
	button.iBitmap = cxChar * 41;
	button.idCommand = IDC_SYNTH_VOICES;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 1 - Separator
	button.iBitmap = 0;
	button.idCommand = 0;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// 0 - Wide separator for CPU usage
	button.iBitmap = cxChar * 42;
	button.idCommand = IDC_SYNTH_CPU;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_SEP;
	::SendMessage( m_hWndToolbar, TB_INSERTBUTTON, 0, (LPARAM)&button );

	// Attach the reset button
	RECT rect;
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 8, (LPARAM)&rect );
	CString strButtonText;
	strButtonText.LoadString( IDS_SYNTHSTATUS_RESET_BUTTON );
	if( !m_buttonReset.Create(strButtonText, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rect, CWnd::FromHandle( m_hWndToolbar), IDC_SYNTH_RESET) )
	{
		return -1;
	}
	m_buttonReset.SetFont( &m_font );
	m_buttonReset.EnableWindow(TRUE);

	// Attach the Latency static text box
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 6, (LPARAM)&rect );
	if( !m_staticLatencyText.Create(NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER,
		rect, CWnd::FromHandle( m_hWndToolbar), IDC_SYNTH_LATENCY) )
	{
		return -1;
	}
	m_staticLatencyText.SetFont( &m_font );
	//m_staticLatencyText.EnableWindow(TRUE);

	// Attach the Memory static text box
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 4, (LPARAM)&rect );
	if( !m_staticMemoryText.Create(NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER,
		rect, CWnd::FromHandle( m_hWndToolbar), IDC_SYNTH_MEMORY) )
	{
		return -1;
	}
	m_staticMemoryText.SetFont( &m_font );
	//m_staticMemoryText.EnableWindow(TRUE);

	// Attach the Voices static text box
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 2, (LPARAM)&rect );
	if( !m_staticVoicesText.Create(NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER,
		rect, CWnd::FromHandle( m_hWndToolbar), IDC_SYNTH_VOICES) )
	{
		return -1;
	}
	m_staticVoicesText.SetFont( &m_font );
	//m_staticVoicesText.EnableWindow(TRUE);

	// Attach the CPU usage static text box
	::SendMessage( m_hWndToolbar, TB_GETITEMRECT, 0, (LPARAM)&rect );
	if( !m_staticCPUText.Create(NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER,
		rect, CWnd::FromHandle( m_hWndToolbar), IDC_SYNTH_CPU) )
	{
		ASSERT(FALSE);
		return -1;
	}
	m_staticCPUText.SetFont( &m_font );
	//m_staticCPUText.EnableWindow(TRUE);

	return S_OK;
}

LRESULT CSynthStatusToolbar::OnResetClicked( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
	UNREFERENCED_PARAMETER(bHandled);
	UNREFERENCED_PARAMETER(hWndCtl);
	UNREFERENCED_PARAMETER(wID);
	UNREFERENCED_PARAMETER(wNotifyCode);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_dwPeakCPU = 0;
	m_dwPeakVoices = 0;
	m_rtPeakCPU = 0;
	m_rtPeakVoices = 0;

	ZeroMemory( s_adwLatency, sizeof(DWORD) * 20 );
	s_dwLatencyIndex = 0;
	s_dwLatencyCount = 0;

	return FALSE; // Don't process further
}
