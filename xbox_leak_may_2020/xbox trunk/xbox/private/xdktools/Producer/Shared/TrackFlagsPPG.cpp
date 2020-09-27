/************************************************************************
*                                                                       *
*   Copyright (c) 2000-2001 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// TrackFlagsPPG.cpp : implementation file
//

#include "stdafx.h"
#include "TrackFlagsPPG.h"
#include "SegmentIO.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include "DMUSProd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short *CTrackFlagsPPG::sm_pnActiveTab = NULL;

#define DMUS_TRACKCONFIG_TRANS_ALL (DMUS_TRACKCONFIG_TRANS1_TOSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT)

/////////////////////////////////////////////////////////////////////////////
// CTrackFlagsPPG property page

IMPLEMENT_DYNCREATE(CTrackFlagsPPG, CPropertyPage)

CTrackFlagsPPG::CTrackFlagsPPG() : CPropertyPage(CTrackFlagsPPG::IDD)
{
	//{{AFX_DATA_INIT(CTrackFlagsPPG)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_pIPropSheet = NULL;
	m_fNeedToDetach = FALSE;
	m_PPGTrackFlagsParams.dwPageIndex = TRACKFLAGSPPG_INDEX;
	m_PPGTrackFlagsParams.dwTrackExtrasFlags = 0;
	m_PPGTrackFlagsParams.dwTrackExtrasMask = 0;
	m_PPGTrackFlagsParams.dwProducerOnlyFlags = 0;
	m_PPGTrackFlagsParams.dwProducerOnlyMask = 0;
}

CTrackFlagsPPG::~CTrackFlagsPPG()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
		m_pPPO = NULL;
	}
}

void CTrackFlagsPPG::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrackFlagsPPG)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrackFlagsPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CTrackFlagsPPG)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_AUDITION_ONLY, OnCheckAuditionOnly)
	ON_BN_CLICKED(IDC_CHECK_TRANSITION, OnCheckTransition)
	ON_BN_DOUBLECLICKED(IDC_CHECK_TRANSITION, OnDoubleclickedCheckTransition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CTrackFlagsPPG::SetObject( IDMUSProdPropPageObject* pPPO )
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
	m_pPPO = pPPO;
	if( m_pPPO )
	{
		m_pPPO->AddRef();
	}
}

void CTrackFlagsPPG::EnableControls( BOOL fEnable ) 
{
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if( !fEnable )
	{
		for( int i = IDC_CHECK_OVERRIDE_PLAY ;  i <= IDC_RADIO_TRANS_TOSEGSTART ;  i++ )
		{
			EnableItem( i, FALSE );
		}
	}
	else
	{
		EnableItem( IDC_CHECK_OVERRIDE_ALL, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_OVERRIDE_ALL) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_OVERRIDE_PRIMARY, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_OVERRIDE_PRIMARY) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_FALLBACK, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_FALLBACK) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_OVERRIDE_PLAY, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_CONTROL_PLAY) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_OVERRIDE_NOTIFICATION, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_CONTROL_NOTIFICATION) ? TRUE : FALSE );

		EnableItem( IDC_CHECK_ENABLE_PLAY, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_PLAY_ENABLED) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_ENABLE_CONTROL, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_CONTROL_ENABLED) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_SEND_NOTIFICATIONS, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_NOTIFICATION_ENABLED) ? TRUE : FALSE );

		EnableItem( IDC_CHECK_USE_CLOCKTIME, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_PLAY_CLOCKTIME) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_RECOMPOSE_ON_PLAY, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_PLAY_COMPOSE) ? TRUE : FALSE );
		EnableItem( IDC_CHECK_RECOMPOSE_ON_LOOP, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_LOOP_COMPOSE) ? TRUE : FALSE );
		//EnableItem( IDC_CHECK_COMPOSING, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_COMPOSING) ? TRUE : FALSE );

		// Enable checkbox if any of the transition flags are valid
		EnableItem( IDC_CHECK_TRANSITION, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_TRANS_ALL) ? TRUE : FALSE );

		// Only enable radio buttons if the checkbox will be checked (if at least one transition flag is checked)
		if( m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS_ALL )
		{
			EnableItem( IDC_RADIO_TRANS_TOSEGSTART, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_TRANS1_TOSEGSTART) ? TRUE : FALSE );
			EnableItem( IDC_RADIO_TRANS_FROMSEGSTART, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART) ? TRUE : FALSE );
			EnableItem( IDC_RADIO_TRANS_FROMSEGCURRENT, (m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT) ? TRUE : FALSE );
		}
		else
		{
			EnableItem( IDC_RADIO_TRANS_TOSEGSTART, FALSE );
			EnableItem( IDC_RADIO_TRANS_FROMSEGSTART, FALSE );
			EnableItem( IDC_RADIO_TRANS_FROMSEGCURRENT, FALSE );
		}

		EnableItem( IDC_CHECK_AUDITION_ONLY, (m_PPGTrackFlagsParams.dwProducerOnlyMask & SEG_PRODUCERONLY_AUDITIONONLY) ? TRUE : FALSE );
	}

}

/////////////////////////////////////////////////////////////////////////////
// CTrackFlagsPPG message handlers

int CTrackFlagsPPG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if( !FromHandlePermanent( m_hWnd ) )
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );
		m_fNeedToDetach = TRUE;
	}

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CTrackFlagsPPG::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
}

void CTrackFlagsPPG::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	PPGTrackFlagsParams *pPPGTrackFlagsParams = &m_PPGTrackFlagsParams;
	if( FAILED( m_pPPO->GetData( (void**)&pPPGTrackFlagsParams ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	// Prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	// Control Flags
	CheckDlgButton( IDC_CHECK_OVERRIDE_ALL, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_OVERRIDE_ALL ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_OVERRIDE_PRIMARY, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_OVERRIDE_PRIMARY ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_FALLBACK, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_FALLBACK ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_OVERRIDE_PLAY, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_CONTROL_PLAY ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_OVERRIDE_NOTIFICATION, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_CONTROL_NOTIFICATION ? BST_CHECKED : BST_UNCHECKED );

	// General Flags
	CheckDlgButton( IDC_CHECK_ENABLE_PLAY, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_PLAY_ENABLED ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_ENABLE_CONTROL, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_CONTROL_ENABLED ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_SEND_NOTIFICATIONS, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_NOTIFICATION_ENABLED ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_USE_CLOCKTIME, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_PLAY_CLOCKTIME ? BST_CHECKED : BST_UNCHECKED );

	// Ensure only one transition flag is set
	if( m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT )
	{
		m_PPGTrackFlagsParams.dwTrackExtrasFlags &= ~(DMUS_TRACKCONFIG_TRANS1_TOSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART);
	}
	else if( m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART )
	{
		m_PPGTrackFlagsParams.dwTrackExtrasFlags &= ~(DMUS_TRACKCONFIG_TRANS1_TOSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT);
	}
	else if( m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS1_TOSEGSTART )
	{
		m_PPGTrackFlagsParams.dwTrackExtrasFlags &= ~(DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT);
	}

	// Transition flags
	CheckDlgButton( IDC_CHECK_TRANSITION, (m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS_ALL) ? TRUE : FALSE );
	CheckDlgButton( IDC_RADIO_TRANS_TOSEGSTART, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS1_TOSEGSTART ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_RADIO_TRANS_FROMSEGSTART, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_RADIO_TRANS_FROMSEGCURRENT, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT ? BST_CHECKED : BST_UNCHECKED );

	// Composing flags
	CheckDlgButton( IDC_CHECK_RECOMPOSE_ON_PLAY, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_PLAY_COMPOSE ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( IDC_CHECK_RECOMPOSE_ON_LOOP, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_LOOP_COMPOSE ? BST_CHECKED : BST_UNCHECKED );
	//CheckDlgButton( IDC_CHECK_COMPOSING, m_PPGTrackFlagsParams.dwTrackExtrasFlags & DMUS_TRACKCONFIG_COMPOSING ? BST_CHECKED : BST_UNCHECKED );

	// Producer-only Flags
	CheckDlgButton( IDC_CHECK_AUDITION_ONLY, m_PPGTrackFlagsParams.dwProducerOnlyFlags & SEG_PRODUCERONLY_AUDITIONONLY ? BST_CHECKED : BST_UNCHECKED );

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;

}

BOOL CTrackFlagsPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	if( sm_pnActiveTab && m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( sm_pnActiveTab );
	}

	return CPropertyPage::OnSetActive();
}

BOOL CTrackFlagsPPG::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nControlID = LOWORD( wParam );
	int nCommandID = HIWORD( wParam );

	if( nCommandID == BN_CLICKED
	||  nCommandID == BN_DOUBLECLICKED )
	{
		switch( nControlID )
		{
		case IDC_CHECK_OVERRIDE_ALL:
			OnCheckHelper( IDC_CHECK_OVERRIDE_ALL, DMUS_TRACKCONFIG_OVERRIDE_ALL );
			return TRUE;
		case IDC_CHECK_OVERRIDE_PRIMARY:
			OnCheckHelper( IDC_CHECK_OVERRIDE_PRIMARY, DMUS_TRACKCONFIG_OVERRIDE_PRIMARY );
			return TRUE;
		case IDC_CHECK_FALLBACK:
			OnCheckHelper( IDC_CHECK_FALLBACK, DMUS_TRACKCONFIG_FALLBACK );
			return TRUE;
		case IDC_CHECK_OVERRIDE_PLAY:
			OnCheckHelper( IDC_CHECK_OVERRIDE_PLAY, DMUS_TRACKCONFIG_CONTROL_PLAY );
			return TRUE;
		case IDC_CHECK_OVERRIDE_NOTIFICATION:
			OnCheckHelper( IDC_CHECK_OVERRIDE_NOTIFICATION, DMUS_TRACKCONFIG_CONTROL_NOTIFICATION );
			return TRUE;

		case IDC_CHECK_ENABLE_PLAY:
			OnCheckHelper( IDC_CHECK_ENABLE_PLAY, DMUS_TRACKCONFIG_PLAY_ENABLED );
			return TRUE;
		case IDC_CHECK_ENABLE_CONTROL:
			OnCheckHelper( IDC_CHECK_ENABLE_CONTROL, DMUS_TRACKCONFIG_CONTROL_ENABLED );
			return TRUE;
		case IDC_CHECK_SEND_NOTIFICATIONS:
			OnCheckHelper( IDC_CHECK_SEND_NOTIFICATIONS, DMUS_TRACKCONFIG_NOTIFICATION_ENABLED );
			return TRUE;

		case IDC_RADIO_TRANS_TOSEGSTART:
			OnTransHelper( IDC_RADIO_TRANS_TOSEGSTART, DMUS_TRACKCONFIG_TRANS1_TOSEGSTART );
			return TRUE;
		case IDC_RADIO_TRANS_FROMSEGSTART:
			OnTransHelper( IDC_RADIO_TRANS_FROMSEGSTART, DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART );
			return TRUE;
		case IDC_RADIO_TRANS_FROMSEGCURRENT:
			OnTransHelper( IDC_RADIO_TRANS_FROMSEGCURRENT, DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT );
			return TRUE;

		case IDC_CHECK_USE_CLOCKTIME:
			OnCheckHelper( IDC_CHECK_USE_CLOCKTIME, DMUS_TRACKCONFIG_PLAY_CLOCKTIME );
			return TRUE;
		case IDC_CHECK_RECOMPOSE_ON_PLAY:
			OnCheckHelper( IDC_CHECK_RECOMPOSE_ON_PLAY, DMUS_TRACKCONFIG_PLAY_COMPOSE );
			return TRUE;
		case IDC_CHECK_RECOMPOSE_ON_LOOP:
			OnCheckHelper( IDC_CHECK_RECOMPOSE_ON_LOOP, DMUS_TRACKCONFIG_LOOP_COMPOSE );
			return TRUE;
//		case IDC_CHECK_COMPOSING:
//			OnCheckHelper( IDC_CHECK_COMPOSING, DMUS_TRACKCONFIG_COMPOSING );
//			return TRUE;
		}
	}

	return CPropertyPage::OnCommand( wParam, lParam );
}

void CTrackFlagsPPG::UpdatePPO()
{
	if( m_pPPO )
	{
		m_pPPO->SetData( (void *)&m_PPGTrackFlagsParams );
	}
}

void CTrackFlagsPPG::OnTransHelper( int nButtonID, DWORD dwFlag )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Mask out the transition flags
	DWORD dwNewTrackFlags = m_PPGTrackFlagsParams.dwTrackExtrasFlags & ~DMUS_TRACKCONFIG_TRANS_ALL;

	// Set the transition flag
	dwNewTrackFlags |= dwFlag;

	if( m_PPGTrackFlagsParams.dwTrackExtrasFlags != dwNewTrackFlags )
	{
		m_PPGTrackFlagsParams.dwTrackExtrasFlags = dwNewTrackFlags;
		UpdatePPO();
	}
}

void CTrackFlagsPPG::OnCheckHelper( int nButtonID, DWORD dwFlag )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD dwNewTrackFlags = m_PPGTrackFlagsParams.dwTrackExtrasFlags;

	// If the button is checked
	if( IsDlgButtonChecked( nButtonID ) )
	{
		dwNewTrackFlags |= dwFlag;
	}
	else // The button is unchecked
	{
		dwNewTrackFlags &= ~dwFlag;
	}

	if( m_PPGTrackFlagsParams.dwTrackExtrasFlags != dwNewTrackFlags )
	{
		m_PPGTrackFlagsParams.dwTrackExtrasFlags = dwNewTrackFlags;
		UpdatePPO();
	}
}

void CTrackFlagsPPG::OnCheckAuditionOnly() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD dwNewProducerOnlyFlags = m_PPGTrackFlagsParams.dwProducerOnlyFlags;

	if( IsDlgButtonChecked( IDC_CHECK_AUDITION_ONLY ) )
	{
		dwNewProducerOnlyFlags |= SEG_PRODUCERONLY_AUDITIONONLY;
	}
	else
	{
		dwNewProducerOnlyFlags &= ~SEG_PRODUCERONLY_AUDITIONONLY;
	}

	if( m_PPGTrackFlagsParams.dwProducerOnlyFlags != dwNewProducerOnlyFlags )
	{
		m_PPGTrackFlagsParams.dwProducerOnlyFlags = dwNewProducerOnlyFlags;
		UpdatePPO();
	}
}

void CTrackFlagsPPG::EnableItem(int nItem, BOOL fEnable)
{
	CWnd* pWnd;
	pWnd = GetDlgItem(nItem);
	if (pWnd)
	{
		pWnd->EnableWindow(fEnable);
	}
}

void CTrackFlagsPPG::OnCheckTransition() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( IsDlgButtonChecked( IDC_CHECK_TRANSITION ) )
	{
		if( m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT )
		{
			CheckDlgButton( IDC_RADIO_TRANS_FROMSEGCURRENT, BST_CHECKED );
			OnTransHelper( IDC_RADIO_TRANS_FROMSEGCURRENT, DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT );
		}
		else if( m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART )
		{
			CheckDlgButton( IDC_RADIO_TRANS_FROMSEGSTART,  BST_CHECKED );
			OnTransHelper( IDC_RADIO_TRANS_FROMSEGSTART, DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART );
		}
		else if( m_PPGTrackFlagsParams.dwTrackExtrasMask & DMUS_TRACKCONFIG_TRANS1_TOSEGSTART )
		{
			CheckDlgButton( IDC_RADIO_TRANS_TOSEGSTART, BST_CHECKED );
			OnTransHelper( IDC_RADIO_TRANS_TOSEGSTART, DMUS_TRACKCONFIG_TRANS1_TOSEGSTART );
		}
	}
	else
	{
		// Mask out the transition flags
		DWORD dwNewTrackFlags = m_PPGTrackFlagsParams.dwTrackExtrasFlags & ~DMUS_TRACKCONFIG_TRANS_ALL;

		// Uncheck the transition buttons
		CheckDlgButton( IDC_RADIO_TRANS_FROMSEGCURRENT, BST_UNCHECKED );
		CheckDlgButton( IDC_RADIO_TRANS_FROMSEGSTART, BST_UNCHECKED );
		CheckDlgButton( IDC_RADIO_TRANS_TOSEGSTART, BST_UNCHECKED );

		if( m_PPGTrackFlagsParams.dwTrackExtrasFlags != dwNewTrackFlags )
		{
			m_PPGTrackFlagsParams.dwTrackExtrasFlags = dwNewTrackFlags;
			UpdatePPO();
		}
	}

	EnableControls( TRUE );
}

void CTrackFlagsPPG::OnDoubleclickedCheckTransition() 
{
	OnCheckTransition();
}
