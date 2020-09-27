// NewPartDlg.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "NewPartDlg.h"
#include <PChannelName.h>
#include <SegmentDesigner.h>
#include "Timeline.h"
#include <dmusici.h>
#include "TrackMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewPartDlg dialog


CNewPartDlg::CNewPartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewPartDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_lPChannel = 0;
	m_pTrackMgr = NULL;
}

// Set the track number
void CNewPartDlg::SetTrack( int nTrack )
{
	m_lPChannel = nTrack;
}

// Set m_pTrackMgr
void CNewPartDlg::SetTrackMgr( CTrackMgr *pTrackMgr )
{
	ASSERT( pTrackMgr != NULL );
	if ( pTrackMgr == NULL )
	{
		return;
	}
	m_pTrackMgr = pTrackMgr;
}

void CNewPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewPartDlg)
	DDX_Control(pDX, IDC_PCHANNEL_STATUS, m_staticStatus);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_NEWPART_EDIT_PCHANNEL, m_editPChannel);
	DDX_Control(pDX, IDC_NEWPART_SPIN_PCHANNEL, m_spinPChannel);
	DDX_Control(pDX, IDC_EDIT_PCHNAME, m_editPChannelName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewPartDlg, CDialog)
	//{{AFX_MSG_MAP(CNewPartDlg)
	ON_EN_CHANGE(IDC_NEWPART_EDIT_PCHANNEL, OnChangeNewpartEditPChannel)
	ON_EN_KILLFOCUS(IDC_NEWPART_EDIT_PCHANNEL, OnKillfocusNewpartEditPChannel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NEWPART_SPIN_PCHANNEL, OnDeltaposNewpartSpinPchannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPartDlg message handlers

BOOL CNewPartDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CDialog::OnInitDialog();

	// Make sure the TrackMgr pointer is valid
	ASSERT( m_pTrackMgr );
	if( m_pTrackMgr == NULL )
	{
		return TRUE;
	}

	m_lPChannel = m_pTrackMgr->GetGreatestUsedPChannel() + 1;

	m_spinPChannel.SetRange(1, MAX_PCHANNEL);
	m_spinPChannel.SetPos( m_lPChannel + 1 );

	m_editPChannel.LimitText(4);	// Length of 'Perf'

	m_strEmptyPChannel.LoadString( IDS_EMPTY_PCHANNEL );
	m_strUsedPChannel.LoadString( IDS_USED_PCHANNEL );
	m_strInvalidPChannel.LoadString( IDS_INVALID_PCHANNEL );

	m_strBroadcastSeg.LoadString( IDS_BROADCAST_SEG );
	m_strBroadcastPerf.LoadString( IDS_BROADCAST_PERF );

	UpdatePChannelName();

	m_btnOK.EnableWindow( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewPartDlg::OnChangeNewpartEditPChannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_spinPChannel.GetSafeHwnd() != NULL)
	{
		CString cstrTmp;
		m_editPChannel.GetWindowText( cstrTmp );

		// Strip leading and trailing spaces
		cstrTmp.TrimRight();
		cstrTmp.TrimLeft();

		// Exit if empty
		if ( cstrTmp.IsEmpty() )
		{
			m_staticStatus.SetWindowText(m_strInvalidPChannel);
			m_editPChannelName.SetWindowText("--------");
			m_btnOK.EnableWindow( FALSE );
			return;
		}

		// Check if this is broadcast PChannel
		if( cstrTmp.CompareNoCase( m_strBroadcastPerf ) == 0 )
		{
			m_lPChannel = -1;
			UpdatePChannelName();
		}
		else if( cstrTmp.CompareNoCase( m_strBroadcastSeg ) == 0 )
		{
			m_lPChannel = -2;
			UpdatePChannelName();
		}
		else
		{
			BOOL fTransSucceeded;
			int nPChannel = GetDlgItemInt( IDC_NEWPART_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
			if ( !fTransSucceeded || (nPChannel < 1) || (nPChannel > MAX_PCHANNEL) )
			{
				m_staticStatus.SetWindowText(m_strInvalidPChannel);
				m_editPChannelName.SetWindowText("--------");
				m_btnOK.EnableWindow( FALSE );
			}
			else
			{
				m_lPChannel = nPChannel - 1;

				UpdatePChannelName();
				m_btnOK.EnableWindow( TRUE );
			}
		}
	}
}

void CNewPartDlg::OnKillfocusNewpartEditPChannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		CString cstrTmp;
		m_editPChannel.GetWindowText( cstrTmp );

		// Strip leading and trailing spaces
		cstrTmp.TrimRight();
		cstrTmp.TrimLeft();

		if ( cstrTmp.IsEmpty() )
		{
			if( m_lPChannel >= 0 )
			{
				m_spinPChannel.SetPos( m_lPChannel + 1 );
			}
			else if( m_lPChannel == -1 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
			}
			else if( m_lPChannel == -2 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
			}

			UpdatePChannelName();
			m_btnOK.EnableWindow( TRUE );
			return;
		}

		if( (cstrTmp.CompareNoCase( m_strBroadcastPerf ) != 0)
		&&	(cstrTmp.CompareNoCase( m_strBroadcastSeg ) != 0) )
		{
			BOOL fTransSucceeded;
			int nPChannel = GetDlgItemInt( IDC_NEWPART_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
			if ( !fTransSucceeded || (nPChannel < 1) )
			{
				m_lPChannel = 0;
				m_spinPChannel.SetPos( 1 );
			}
			else if ( nPChannel > MAX_PCHANNEL )
			{
				m_lPChannel = MAX_PCHANNEL - 1;
				m_spinPChannel.SetPos( MAX_PCHANNEL );
			}
			else
			{
				// m_lPChannel should have been set by OnChangeNewpartEditPchannel()
				ASSERT(m_lPChannel == nPChannel - 1 );
			}
		}

		UpdatePChannelName();

		m_btnOK.EnableWindow( TRUE );
	}
}

void CNewPartDlg::UpdatePChannelName( void )
{
	ASSERT( m_pTrackMgr != NULL );

	if( m_pTrackMgr->PChannelExists( m_lPChannel ) )
	{
		m_staticStatus.SetWindowText( m_strUsedPChannel );
	}
	else
	{
		m_staticStatus.SetWindowText( m_strEmptyPChannel );
	}

	// Fill the PChannel name edit box with the PChannel's name
	if( m_pTrackMgr )
	{
		CString strPChannelName;

		if( SUCCEEDED( m_pTrackMgr->GetPChannelName( m_lPChannel, strPChannelName ) ) )
		{
			m_editPChannelName.SetWindowText( strPChannelName );
		}
	}
}

void CNewPartDlg::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return;
	}

	// Convert the m_lPChannel to a DWORD
	DWORD dwPChannel;
	if( m_lPChannel >= 0 )
	{
		dwPChannel = m_lPChannel;
	}
	else if( m_lPChannel == -1 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
	}
	else if( m_lPChannel == -2 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
	}

	// Create the Part
	if( SUCCEEDED ( m_pTrackMgr->AddPart( dwPChannel ) ) )
	{
		// Update the UI to the next PChannel
		if( m_lPChannel >= 0 )
		{
			m_lPChannel = min( (m_lPChannel + 1), (MAX_PCHANNEL - 1) );
		}
		else
		{
			m_lPChannel = 0;
		}

		m_spinPChannel.SetPos( m_lPChannel + 1 );
		UpdatePChannelName();
	}
}

void CNewPartDlg::OnDeltaposNewpartSpinPchannel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	*pResult = 0;

	if( pNMUpDown && (m_lPChannel <= 0) )
	{
		if( pNMUpDown->iDelta > 0 )
		{
			if( m_lPChannel == -2 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
				m_lPChannel = -1;
				*pResult = 1;
			}
			else if( m_lPChannel == -1 )
			{
				m_spinPChannel.SetPos( 1 );
				m_lPChannel = 0;
				*pResult = 1;
			}
		}
		else // ( pNMUpDown->iDelta <= 0 )
		{
			if( m_lPChannel == -1 )
			{
				m_lPChannel = -2;
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
			}
			else if( m_lPChannel == 0 )
			{
				m_lPChannel = -1;
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
			}
			*pResult = 1;
		}
	}
}


// Custom edit control that only accepts numbers, editing keys, 's' and 'p'
IMPLEMENT_DYNCREATE( CNewPartEdit, CEdit )

CNewPartEdit::CNewPartEdit() : CEdit()
{
}

LRESULT CNewPartEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CHAR:
		switch( wParam )
		{
		case 8: // Backspace
		case 's':
		case 'S':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		case 'p':
		case 'P':
		case 'r':
		case 'R':
		case 'f':
		case 'F':
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
			break;
		default:
			return TRUE;
		break;
		}
	}
	return CEdit::WindowProc( message, wParam, lParam );
}
