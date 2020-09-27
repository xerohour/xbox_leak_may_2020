// PropPageSeqTrack.cpp : implementation file
//
// This encompases setting the PChannel, name, and color settings.
//

#include "stdafx.h"
#include "PropPageSeqTrack.h"
#include <PChannelName.h>
#include "Timeline.h"
#include "DMusProd.h"
#include <dmusici.h>
#include "DlgExtraLength.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short* CPropPageSeqTrack::sm_pnActiveTab = NULL;

/////////////////////////////////////////////////////////////////////////////
// CPropPageSeqTrack property page

IMPLEMENT_DYNCREATE(CPropPageSeqTrack, CPropertyPage)

CPropPageSeqTrack::CPropPageSeqTrack() : CPropertyPage(CPropPageSeqTrack::IDD)
{
	//{{AFX_DATA_INIT(CPropPageSeqTrack)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_dwPChannel = 0;
	m_fNeedToDetach = FALSE;
	m_crSelectedNoteColor = 0;
	m_crUnselectedNoteColor = 0;
	m_crOverlappingNoteColor = 0;
	m_crAccidentalColor = 0;
	ZeroMemory( m_pcrCustomColors, sizeof(COLORREF) * 16);
	m_pIPChannelName = NULL;
	m_dwExtraBars = 0;
	m_fPickupBar = FALSE;
}

CPropPageSeqTrack::~CPropPageSeqTrack()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
	if( m_pIPChannelName )
	{
		m_pIPChannelName->Release();
	}
}

void CPropPageSeqTrack::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageSeqTrack)
	DDX_Control(pDX, IDC_STATIC_PICKUP, m_staticPickup);
	DDX_Control(pDX, IDC_STATIC_EXTENSION, m_staticExtension);
	DDX_Control(pDX, IDC_EDIT_PCHANNELNAME, m_editPChannelName);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_SPIN_PCHANNEL, m_spinPChannel);
	DDX_Control(pDX, IDC_EDIT_PCHANNEL, m_editPChannel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageSeqTrack, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageSeqTrack)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_PCHANNEL, OnChangeEditPchannel)
	ON_EN_KILLFOCUS(IDC_EDIT_PCHANNEL, OnKillfocusEditPchannel)
	ON_BN_CLICKED(IDC_BUTTON_NOTECOLOR, OnButtonNotecolor)
	ON_BN_CLICKED(IDC_BUTTON_SELECTIONCOLOR, OnButtonSelectioncolor)
	ON_BN_CLICKED(IDC_BUTTON_ACCIDENTALCOLOR, OnButtonAccidentalcolor)
	ON_BN_CLICKED(IDC_BUTTON_OVERLAPCOLOR, OnButtonOverlapcolor)
	ON_EN_CHANGE(IDC_EDIT_PCHANNELNAME, OnChangeEditPChannelName)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PCHANNEL, OnDeltaposSpinPchannel)
	ON_BN_CLICKED(IDC_BUTTON_EXT, OnButtonExt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CPropPageSeqTrack::SetObject( IDMUSProdPropPageObject* pPPO )
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

void CPropPageSeqTrack::EnableItem( int nID, BOOL fEnable )
{
	CWnd *pWnd = GetDlgItem( nID );
	if( pWnd )
	{
		pWnd->EnableWindow(fEnable);
	}
}

void CPropPageSeqTrack::EnableControls( BOOL fEnable )
{
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if( ::IsWindow(m_editPChannel.m_hWnd))
	{
		m_editPChannel.EnableWindow(fEnable);
		m_spinPChannel.EnableWindow( fEnable );
	}

	if( ::IsWindow(m_editName.m_hWnd))
	{
		m_editName.EnableWindow(fEnable);
	}

	if( ::IsWindow(m_editPChannelName.m_hWnd))
	{
		m_editPChannelName.EnableWindow(fEnable);
	}

	EnableItem( IDC_BUTTON_NOTECOLOR, fEnable );
	EnableItem( IDC_BUTTON_SELECTIONCOLOR, fEnable );
	EnableItem( IDC_BUTTON_OVERLAPCOLOR, fEnable );
	EnableItem( IDC_BUTTON_ACCIDENTALCOLOR, fEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageSeqTrack message handlers

int CPropPageSeqTrack::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CPropPageSeqTrack::OnDestroy() 
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

	// Needed to fix memory leak (circular reference PPG->Segment->StripMgr->PPG
	if( m_pIPChannelName )
	{
		m_pIPChannelName->Release();
		m_pIPChannelName = NULL;
	}

	CPropertyPage::OnDestroy();
}

BOOL CPropPageSeqTrack::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	m_editPChannel.SetLimitText( 5 ); // 'APath'
	m_spinPChannel.SetRange( MIN_PCHANNEL, MAX_PCHANNEL );

	// Limit PChannel names to DMUS_MAX_NAME - 1 characters
	m_editPChannelName.SetLimitText( DMUS_MAX_NAME - 1 );

	// Limit track names to DMUS_MAX_NAME - 1 characters
	m_editName.SetLimitText( DMUS_MAX_NAME - 1 );

	m_strBroadcastSeg.LoadString(IDS_BROADCAST_SEG);
	m_strBroadcastPerf.LoadString(IDS_BROADCAST_PERF);
	m_strBroadcastAPath.LoadString(IDS_BROADCAST_APATH);
	m_strBroadcastGrp.LoadString(IDS_BROADCAST_GRP);

	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPageSeqTrack::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioSeqTrackPPG iSeqTrackPPG, *pioSeqTrackPPG;
	pioSeqTrackPPG = &iSeqTrackPPG;
	iSeqTrackPPG.dwPageIndex = 2;
	if( FAILED( m_pPPO->GetData( (void**)&pioSeqTrackPPG ) ) )
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

	m_dwPChannel = iSeqTrackPPG.dwPChannel;

	// If a broadcast PChannel, disable setting the PChannel name, and display text
	// instead of a number
	if( m_dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS )
	{
		m_editPChannelName.EnableWindow( FALSE );
		if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
		{
			m_editPChannel.SetWindowText( m_strBroadcastSeg );
		}
		else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
		{
			m_editPChannel.SetWindowText( m_strBroadcastPerf );
		}
		else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
		{
			m_editPChannel.SetWindowText( m_strBroadcastGrp );
		}
		else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
		{
			m_editPChannel.SetWindowText( m_strBroadcastAPath );
		}
	}
	else
	{
		m_spinPChannel.SetPos( m_dwPChannel + 1 );
	}

	m_crSelectedNoteColor = iSeqTrackPPG.crSelectedNoteColor;
	m_crUnselectedNoteColor = iSeqTrackPPG.crUnselectedNoteColor;
	m_crOverlappingNoteColor = iSeqTrackPPG.crOverlappingNoteColor;
	m_crAccidentalColor = iSeqTrackPPG.crAccidentalColor;

	m_strName = iSeqTrackPPG.strName;
	m_editName.SetWindowText( m_strName );

	m_strPChannelName = iSeqTrackPPG.strPChannelName;
	m_editPChannelName.SetWindowText( m_strPChannelName );

	m_dwExtraBars = iSeqTrackPPG.dwExtraBars;
	m_fPickupBar = iSeqTrackPPG.fPickupBar;

	SetExtPickupText();

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}

BOOL CPropPageSeqTrack::OnSetActive() 
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

void CPropPageSeqTrack::UpdatePPO()
{
	if( m_pPPO )
	{
		ioSeqTrackPPG oSeqTrackPPG;
		oSeqTrackPPG.dwPageIndex = 2;
		oSeqTrackPPG.dwPChannel = m_dwPChannel;
		oSeqTrackPPG.crSelectedNoteColor = m_crSelectedNoteColor;
		oSeqTrackPPG.crUnselectedNoteColor = m_crUnselectedNoteColor;
		oSeqTrackPPG.crOverlappingNoteColor = m_crOverlappingNoteColor;
		oSeqTrackPPG.crAccidentalColor = m_crAccidentalColor;
		oSeqTrackPPG.strName = m_strName;
		oSeqTrackPPG.strPChannelName = m_strPChannelName;
		oSeqTrackPPG.dwExtraBars = m_dwExtraBars;
		oSeqTrackPPG.fPickupBar = m_fPickupBar;
		m_pPPO->SetData( &oSeqTrackPPG );
	}
}

void CPropPageSeqTrack::OnChangeEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		CString strNewPChannel;
		m_editPChannel.GetWindowText( strNewPChannel );

		// Strip leading and trailing spaces
		strNewPChannel.TrimRight();
		strNewPChannel.TrimLeft();

		if ( !strNewPChannel.IsEmpty() )
		{
			// Check if this is broadcast PChannel
			if( _tcsnicmp( strNewPChannel, m_strBroadcastPerf, strNewPChannel.GetLength() ) == 0 )
			{
				if( m_dwPChannel != DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
				{
					m_dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
						m_editPChannelName.EnableWindow( FALSE );
					}
				}
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastSeg, strNewPChannel.GetLength() ) == 0 )
			{
				if( m_dwPChannel != DMUS_PCHANNEL_BROADCAST_SEGMENT )
				{
					m_dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
						m_editPChannelName.EnableWindow( FALSE );
					}
				}
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastAPath, strNewPChannel.GetLength() ) == 0 )
			{
				if( m_dwPChannel != DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
				{
					m_dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
						m_editPChannelName.EnableWindow( FALSE );
					}
				}
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastGrp, strNewPChannel.GetLength() ) == 0 )
			{
				if( m_dwPChannel != DMUS_PCHANNEL_BROADCAST_GROUPS )
				{
					m_dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
						m_editPChannelName.EnableWindow( FALSE );
					}
				}
			}
			else
			{
				BOOL fTransSucceeded;
				int nPChannel = GetDlgItemInt( IDC_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
				if ( !fTransSucceeded || (nPChannel < MIN_PCHANNEL) )
				{
					nPChannel = MIN_PCHANNEL;
					m_spinPChannel.SetPos( nPChannel );
				}
				else if( nPChannel > MAX_PCHANNEL )
				{
					nPChannel = MAX_PCHANNEL;
					m_spinPChannel.SetPos( nPChannel );
				}

				m_editPChannelName.EnableWindow( TRUE );

				if( (unsigned)nPChannel != (m_dwPChannel + 1) )
				{
					m_dwPChannel = nPChannel - 1;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
					}
				}
			}
		}
	}
}

void CPropPageSeqTrack::OnKillfocusEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		CString strNewPChannel;
		m_editPChannel.GetWindowText( strNewPChannel );

		// Strip leading and trailing spaces
		strNewPChannel.TrimRight();
		strNewPChannel.TrimLeft();

		if ( strNewPChannel.IsEmpty() )
		{
			if( m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				m_spinPChannel.SetPos( m_dwPChannel + 1 );
				m_editPChannelName.EnableWindow( TRUE );
			}
			else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				m_editPChannelName.EnableWindow( FALSE );
			}
			else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
				m_editPChannelName.EnableWindow( FALSE );
			}
			else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
				m_editPChannelName.EnableWindow( FALSE );
			}
			else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGrp );
				m_editPChannelName.EnableWindow( FALSE );
			}
		}
		else
		{
			if( _tcsnicmp( strNewPChannel, m_strBroadcastPerf, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastSeg, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastAPath, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastGrp, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGrp );
			}
			else
			{
				// Convert from text to dword;
				BOOL fTransSucceeded;
				int nPChannel = GetDlgItemInt( IDC_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
				ASSERT( fTransSucceeded );
				/*
				if ( !fTransSucceeded || (nPChannel < 1) )
				{
					m_dwPChannel = 0;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
					}
				}
				else */ if( unsigned(nPChannel - 1) != m_dwPChannel )
				{
					m_dwPChannel = nPChannel - 1;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
					}
				}
				m_editPChannelName.EnableWindow( TRUE );
			}
		}
	}
}

void CPropPageSeqTrack::OnChangeEditPChannelName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	CString cstrNewName;

	m_editPChannelName.GetWindowText( cstrNewName );

	// Strip leading and trailing spaces
	cstrNewName.TrimRight();
	cstrNewName.TrimLeft();

	if( cstrNewName.Compare( m_strPChannelName ) != 0 )
	{
		m_strPChannelName = cstrNewName;
		UpdatePPO();
	}
}

void CPropPageSeqTrack::OnChangeEditName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	CString cstrNewName;

	m_editName.GetWindowText( cstrNewName );

	// Strip leading and trailing spaces
	cstrNewName.TrimRight();
	cstrNewName.TrimLeft();

	if( cstrNewName.Compare( m_strName ) != 0 )
	{
		m_strName = cstrNewName;
		UpdatePPO();
	}
}

BOOL CPropPageSeqTrack::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_EDIT_PCHANNEL:
								// If a broadcast PChannel, disable setting the PChannel name, and display text
								// instead of a number
								if( m_dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS )
								{
									m_editPChannelName.EnableWindow( FALSE );
									if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
									{
										m_editPChannel.SetWindowText( m_strBroadcastSeg );
									}
									else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
									{
										m_editPChannel.SetWindowText( m_strBroadcastPerf );
									}
									else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
									{
										m_editPChannel.SetWindowText( m_strBroadcastAPath );
									}
									else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
									{
										m_editPChannel.SetWindowText( m_strBroadcastGrp );
									}
								}
								else
								{
									m_editPChannelName.EnableWindow( TRUE );
									m_spinPChannel.SetPos( m_dwPChannel + 1 );
								}
								break;
						}
					}
					return TRUE;
				}

				case VK_RETURN:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						CWnd* pWndNext = GetNextDlgTabItem( pWnd );
						if( pWndNext )
						{
							pWndNext->SetFocus();
						}
					}
					return TRUE;
				}
			}
			break;
	}
	
	return CPropertyPage::PreTranslateMessage( pMsg );
}

void CPropPageSeqTrack::OnButtonNotecolor() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CHOOSECOLOR cc;

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->GetSafeHwnd();
	cc.hInstance = NULL;
	cc.rgbResult = m_crUnselectedNoteColor; // initial color
	cc.lpCustColors = m_pcrCustomColors;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	HWND hwndFocus = ::GetFocus();
	if (ChooseColor( &cc ))
	{
		// User chose a color
		m_crUnselectedNoteColor = cc.rgbResult;
		UpdatePPO();
	}

	if( ::GetFocus() != hwndFocus )
	{
		::SetFocus( hwndFocus );
	}
}

void CPropPageSeqTrack::OnButtonSelectioncolor() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CHOOSECOLOR cc;

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->GetSafeHwnd();
	cc.hInstance = NULL;
	cc.rgbResult = m_crSelectedNoteColor; // initial color
	cc.lpCustColors = m_pcrCustomColors;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	HWND hwndFocus = ::GetFocus();
	if (ChooseColor( &cc ))
	{
		// User chose a color
		m_crSelectedNoteColor = cc.rgbResult;
		UpdatePPO();
	}

	if( ::GetFocus() != hwndFocus )
	{
		::SetFocus( hwndFocus );
	}
}

void CPropPageSeqTrack::OnButtonAccidentalcolor() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CHOOSECOLOR cc;

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->GetSafeHwnd();
	cc.hInstance = NULL;
	cc.rgbResult = m_crAccidentalColor; // initial color
	cc.lpCustColors = m_pcrCustomColors;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	HWND hwndFocus = ::GetFocus();
	if (ChooseColor( &cc ))
	{
		// User chose a color
		m_crAccidentalColor = cc.rgbResult;
		UpdatePPO();
	}

	if( ::GetFocus() != hwndFocus )
	{
		::SetFocus( hwndFocus );
	}
}

void CPropPageSeqTrack::OnButtonOverlapcolor() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CHOOSECOLOR cc;

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->GetSafeHwnd();
	cc.hInstance = NULL;
	cc.rgbResult = m_crOverlappingNoteColor; // initial color
	cc.lpCustColors = m_pcrCustomColors;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	HWND hwndFocus = ::GetFocus();
	if (ChooseColor( &cc ))
	{
		// User chose a color
		m_crOverlappingNoteColor = cc.rgbResult;
		UpdatePPO();
	}

	if( ::GetFocus() != hwndFocus )
	{
		::SetFocus( hwndFocus );
	}
}

CString CPropPageSeqTrack::GetPChannelName( DWORD dwPChannel )
{
	if( !m_pIPChannelName )
	{
		IDMUSProdStrip *pIStrip;
		if( SUCCEEDED( m_pPPO->QueryInterface( IID_IDMUSProdStrip, (void**) &pIStrip ) ) )
		{
			VARIANT varStripMgr;
			if( SUCCEEDED( pIStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
			&& (V_UNKNOWN(&varStripMgr) != NULL) )
			{
				IDMUSProdStripMgr *pIStripMgr;
				if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void **)&pIStripMgr ) ) )
				{
					VARIANT varTimeline;
					if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline ) ) )
					{
						IDMUSProdTimeline *pITimelineCtl;
						if( SUCCEEDED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void **)&pITimelineCtl ) ) )
						{
							VARIANT varFramework;
							if (SUCCEEDED(pITimelineCtl->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &varFramework )))
							{
								IDMUSProdFramework *pIFramework;
								if( SUCCEEDED( V_UNKNOWN(&varFramework)->QueryInterface( IID_IDMUSProdFramework, (void**)&pIFramework ) ) )
								{
									VARIANT varCallback;
									if (SUCCEEDED(pITimelineCtl->GetTimelineProperty( TP_TIMELINECALLBACK, &varCallback )))
									{
										IDMUSProdNode* pINode;
										if( SUCCEEDED( V_UNKNOWN(&varCallback)->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
										{
											IDMUSProdProject* pIProject;
											if( SUCCEEDED ( pIFramework->FindProject( pINode, &pIProject ) ) )
											{
												pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&m_pIPChannelName );
												pIProject->Release();
											}
											pINode->Release();
										}
										V_UNKNOWN(&varCallback)->Release();
									}
									pIFramework->Release();
								}
								V_UNKNOWN(&varFramework)->Release();
							}
							pITimelineCtl->Release();
						}
						V_UNKNOWN( &varTimeline )->Release();
					}
					pIStripMgr->Release();
				}
				V_UNKNOWN(&varStripMgr)->Release();
			}
			pIStrip->Release();
		}
	}

	ASSERT( m_pIPChannelName );
	
	WCHAR wstrName[MAX_PATH];

	if( m_pIPChannelName && SUCCEEDED( m_pIPChannelName->GetPChannelName( dwPChannel, wstrName ) ) )
	{
		// Convert PChannel name
		CString cstrText;
		cstrText = wstrName;

		return cstrText;
	}

	return CString("");
}

void CPropPageSeqTrack::OnDeltaposSpinPchannel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	*pResult = 0;

	if( pNMUpDown )
	{
		if( pNMUpDown->iDelta > 0 )
		{
			if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				m_spinPChannel.SetPos( 1 );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = 0;
				*pResult = 1;
			}
			else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
				*pResult = 1;
			}
			else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
				*pResult = 1;
			}
			else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGrp );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
				*pResult = 1;
			}
		}
		else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
		{
			*pResult = 1;
		}
		else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
		{
			m_editPChannel.SetWindowText( m_strBroadcastGrp );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
			*pResult = 1;
		}
		else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
		{
			m_editPChannel.SetWindowText( m_strBroadcastAPath );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
			*pResult = 1;
		}
		else if( m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
		{
			m_editPChannel.SetWindowText( m_strBroadcastPerf );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
			*pResult = 1;
		}
		else if( m_dwPChannel == 0 )
		{
			m_editPChannel.SetWindowText( m_strBroadcastSeg );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
			*pResult = 1;
		}
	}
}

void CPropPageSeqTrack::OnButtonExt() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDlgExtraLength	dlg;
	dlg.m_dwNbrExtraBars = m_dwExtraBars;
	dlg.m_fPickupBar = m_fPickupBar;

	if( dlg.DoModal() == IDOK )
	{
		bool fChange = false;

		if(	dlg.m_fPickupBar != m_fPickupBar )
		{
			m_fPickupBar = dlg.m_fPickupBar;

			fChange = true;
		}

		if( dlg.m_dwNbrExtraBars != m_dwExtraBars )
		{
			m_dwExtraBars = dlg.m_dwNbrExtraBars;

			fChange = true;
		}

		if( fChange )
		{
			SetExtPickupText();
			UpdatePPO();
		}
	}
}

void CPropPageSeqTrack::SetExtPickupText( void )
{
	// Set extension bar text
	CString strTemp;
	if( m_dwExtraBars < 2 )
	{
		int nID;
		if( m_dwExtraBars == 0 )
		{
			nID  = IDS_PPG_0_EXT_TEXT;
		}
		else
		{
			nID = IDS_PPG_1_EXT_TEXT;
		}
		strTemp.LoadString( nID );
	}
	else
	{
		TCHAR tcstrNum[20];
		_itot( m_dwExtraBars, tcstrNum, 10 );
		AfxFormatString1( strTemp, IDS_PPG_MORE_EXT_TEXT, tcstrNum );
	}
	m_staticExtension.SetWindowText( strTemp );

	// Set pickup bar text
	strTemp.LoadString( m_fPickupBar ? IDS_PPG_1_PICKUP : IDS_PPG_NO_PICKUP );
	m_staticPickup.SetWindowText( strTemp );
}
