// TabObject.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TabObject.h"
#include "GroupBitsPPG.h"
#include <PChannelName.h>
#include "Timeline.h"
#include "DMusProd.h"
#include <dmusicf.h>
#include <dmusici.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTabObject property page

CTabObject::CTabObject() : CPropertyPage(CTabObject::IDD)
{
	//{{AFX_DATA_INIT(CTabObject)
	//}}AFX_DATA_INIT

	m_pPPO = NULL;
	m_pPropPageMgr = NULL;
	m_pIPropSheet = NULL;

	m_PPGObjectParams.dwPageIndex = 2;
	m_PPGObjectParams.dwPChannel = 0;
	m_PPGObjectParams.dwParamFlagsDM = 0;

	m_fNeedToDetach = FALSE;
}

CTabObject::~CTabObject()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
}

void CTabObject::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabObject)
	DDX_Control(pDX, IDC_STAGE_STATIC, m_StageStatic);
	DDX_Control(pDX, IDC_OBJECT_STATIC, m_ObjectStatic);
	DDX_Control(pDX, IDC_EDIT_PCHANNEL, m_editPChannel);
	DDX_Control(pDX, IDC_EDIT_PCHANNELNAME, m_editPChannelName);
	DDX_Control(pDX, IDC_SPIN_PCHANNEL, m_spinPChannel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabObject, CPropertyPage)
	//{{AFX_MSG_MAP(CTabObject)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_PCHANNEL, OnChangeEditPchannel)
	ON_EN_KILLFOCUS(IDC_EDIT_PCHANNEL, OnKillfocusEditPchannel)
	ON_EN_CHANGE(IDC_EDIT_PCHANNELNAME, OnChangeEditPChannelName)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PCHANNEL, OnDeltaposSpinPchannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CTabObject::SetObject( IDMUSProdPropPageObject* pPPO )
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

void CTabObject::EnableControls( BOOL fEnable )
{
	// Make sure controls have been created
	if( ::IsWindow(m_editPChannel.m_hWnd) == FALSE )
	{
		return;
	}

	m_editPChannel.EnableWindow(fEnable);
	m_spinPChannel.EnableWindow( fEnable );
	//m_editName.EnableWindow(fEnable);
	m_editPChannelName.EnableWindow(fEnable);
}

/////////////////////////////////////////////////////////////////////////////
// CTabObject message handlers

int CTabObject::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CTabObject::OnDestroy() 
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

BOOL CTabObject::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	m_editPChannel.SetLimitText( 4 ); // 'perf'
	m_spinPChannel.SetRange( MIN_PCHANNEL, MAX_PCHANNEL );

	// Limit PChannel names to DMUS_MAX_NAME - 1 characters
	m_editPChannelName.LimitText( DMUS_MAX_NAME - 1 );

	// Limit track names to DMUS_MAX_NAME - 1 characters
	//m_editName.LimitText( DMUS_MAX_NAME - 1 );

	m_strBroadcastSeg.LoadString(IDS_BROADCAST_SEG);
	m_strBroadcastPerf.LoadString(IDS_BROADCAST_PERF);

	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabObject::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioObjectPPG* pioObjectPPG = &m_PPGObjectParams;
	if( FAILED( m_pPPO->GetData( (void**)&pioObjectPPG ) ) )
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

	// If a broadcast PChannel, disable setting the PChannel name, and display text
	// instead of a number
	/*if( m_PPGObjectParams.dwPChannel >= DMUS_PCHANNEL_BROADCAST_SEGMENT )
	{
		m_editPChannelName.EnableWindow( FALSE );
		if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
		{
			m_editPChannel.SetWindowText( m_strBroadcastSeg );
		}
		else if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
		{
			m_editPChannel.SetWindowText( m_strBroadcastPerf );
		}
	}
	else
	{
		m_spinPChannel.SetPos( m_PPGObjectParams.dwPChannel + 1 );
	}*/

	// Set strip name
	//m_editName.SetWindowText( m_PPGObjectParams.strStripName );

	m_ObjectStatic.SetWindowText(m_PPGObjectParams.strObjectName);
	//m_StageStatic.SetWindowText(m_PPGObjectParams.strStageName);

	// Set PChannel name
	//m_editPChannelName.SetWindowText( m_PPGObjectParams.strPChannelName );
	m_editPChannelName.SetWindowText(m_PPGObjectParams.strPChannelText);

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}

BOOL CTabObject::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	m_pIPropSheet->GetActivePage( &CGroupBitsPropPageMgr::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}

void CTabObject::UpdatePPO()
{
	if( m_pPPO )
	{
		VERIFY( SUCCEEDED( m_pPPO->SetData(&m_PPGObjectParams) ) );
	}
}

void CTabObject::OnChangeEditPchannel() 
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
				if( m_PPGObjectParams.dwPChannel != DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
				{
					m_PPGObjectParams.dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
					GetPChannelName( m_PPGObjectParams.dwPChannel, m_PPGObjectParams.strPChannelName );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_PPGObjectParams.strPChannelName );
						m_editPChannelName.EnableWindow( FALSE );
					}
				}
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastSeg, strNewPChannel.GetLength() ) == 0 )
			{
				if( m_PPGObjectParams.dwPChannel != DMUS_PCHANNEL_BROADCAST_SEGMENT )
				{
					m_PPGObjectParams.dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
					GetPChannelName( m_PPGObjectParams.dwPChannel, m_PPGObjectParams.strPChannelName );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_PPGObjectParams.strPChannelName );
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

				if( (unsigned)nPChannel != (m_PPGObjectParams.dwPChannel + 1) )
				{
					m_PPGObjectParams.dwPChannel = nPChannel - 1;
					GetPChannelName( m_PPGObjectParams.dwPChannel, m_PPGObjectParams.strPChannelName );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_PPGObjectParams.strPChannelName );
					}
				}
			}
		}
	}
}

void CTabObject::OnKillfocusEditPchannel() 
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
			if( m_PPGObjectParams.dwPChannel < DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				m_spinPChannel.SetPos( m_PPGObjectParams.dwPChannel + 1 );
				m_editPChannelName.EnableWindow( TRUE );
			}
			else if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				m_editPChannelName.EnableWindow( FALSE );
			}
			else if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
				m_editPChannelName.EnableWindow( FALSE );
			}
		}
		else
		{
			if( (_tcsnicmp( strNewPChannel, m_strBroadcastPerf, strNewPChannel.GetLength() ) != 0)
			&&	(_tcsnicmp( strNewPChannel, m_strBroadcastSeg, strNewPChannel.GetLength() ) != 0) )
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
				else */ if( unsigned(nPChannel - 1) != m_PPGObjectParams.dwPChannel )
				{
					m_PPGObjectParams.dwPChannel = nPChannel - 1;
					GetPChannelName( m_PPGObjectParams.dwPChannel, m_PPGObjectParams.strPChannelName );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_PPGObjectParams.strPChannelName );
					}
				}
				m_editPChannelName.EnableWindow( TRUE );
			}
		}
	}
}

void CTabObject::OnChangeEditPChannelName() 
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

	if( cstrNewName.Compare( m_PPGObjectParams.strPChannelName ) != 0 )
	{
		m_PPGObjectParams.strPChannelName = cstrNewName;
		UpdatePPO();
	}
}

/*void CTabObject::OnChangeEditName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	CString cstrNewStripName;

	m_editName.GetWindowText( cstrNewStripName );

	// Strip leading and trailing spaces
	cstrNewStripName.TrimRight();
	cstrNewStripName.TrimLeft();

	if( cstrNewStripName.Compare( m_PPGObjectParams.strStripName ) != 0 )
	{
		m_PPGObjectParams.strStripName = cstrNewStripName;
		UpdatePPO();
	}
}*/

BOOL CTabObject::PreTranslateMessage(MSG* pMsg) 
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
								if( m_PPGObjectParams.dwPChannel >= DMUS_PCHANNEL_BROADCAST_SEGMENT )
								{
									m_editPChannelName.EnableWindow( FALSE );
									if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
									{
										m_editPChannel.SetWindowText( m_strBroadcastSeg );
									}
									else if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
									{
										m_editPChannel.SetWindowText( m_strBroadcastPerf );
									}
								}
								else
								{
									m_editPChannelName.EnableWindow( TRUE );
									m_spinPChannel.SetPos( m_PPGObjectParams.dwPChannel + 1 );
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

HRESULT CTabObject::GetPChannelName( DWORD dwPChannel, CString& strPChannelName )
{
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr )
	{
		return m_pPropPageMgr->m_pTrackMgr->GetPChannelName( dwPChannel, strPChannelName ); 
	}

	// Should not happen!
	ASSERT( 0 );
	strPChannelName.Empty();
	return E_FAIL;
}

void CTabObject::OnDeltaposSpinPchannel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	*pResult = 0;

	if( pNMUpDown )
	{
		if( pNMUpDown->iDelta > 0 )
		{
			if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
				*pResult = 1;
			}
			else if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				m_spinPChannel.SetPos( 1 );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = 0;
				*pResult = 1;
			}
		}
		else if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
		{
			*pResult = 1;
		}
		else if( m_PPGObjectParams.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
		{
			m_editPChannel.SetWindowText( m_strBroadcastSeg );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
			*pResult = 1;
		}
		else if( m_PPGObjectParams.dwPChannel == 0 )
		{
			m_editPChannel.SetWindowText( m_strBroadcastPerf );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
			*pResult = 1;
		}
	}
}
