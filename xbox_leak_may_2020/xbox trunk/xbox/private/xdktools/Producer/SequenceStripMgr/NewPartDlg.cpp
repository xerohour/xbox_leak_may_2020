// NewPartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewPartDlg.h"
#include <PChannelName.h>
#include <SegmentDesigner.h>
#include "Timeline.h"
#include "SequenceStripMgr.h"
#include <dmusici.h>
#include "SequenceMgr.h"

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
	m_pSequenceMgr = NULL;
}

// Set the track number
void CNewPartDlg::SetTrack( int nTrack )
{
	m_lPChannel = nTrack;
}

// Set m_pSequenceMgr
void CNewPartDlg::SetSequenceMgr( CSequenceMgr *pSequenceMgr )
{
	ASSERT( pSequenceMgr != NULL );
	if ( pSequenceMgr == NULL )
	{
		return;
	}
	m_pSequenceMgr = pSequenceMgr;
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

	// Make sure the PChannel Name pointer is valid
	ASSERT( m_pSequenceMgr && m_pSequenceMgr->m_pIPChannelName );
	if ( !m_pSequenceMgr || !m_pSequenceMgr->m_pIPChannelName )
	{
		return TRUE;
	}

	m_lPChannel = GetGreatestUsedPChannel() + 1;

	m_spinPChannel.SetRange(1, MAX_PCHANNEL);
	m_spinPChannel.SetPos( m_lPChannel + 1 );

	m_editPChannel.SetLimitText(5); // Length of 'APath'

	m_strEmptyPChannel.LoadString(IDS_EMPTY_PCHANNEL);
	m_strUsedPChannel.LoadString(IDS_USED_PCHANNEL);
	m_strInvalidPChannel.LoadString(IDS_INVALID_PCHANNEL);

	m_strBroadcastSeg.LoadString(IDS_BROADCAST_SEG);
	m_strBroadcastPerf.LoadString(IDS_BROADCAST_PERF);
	m_strBroadcastAPath.LoadString(IDS_BROADCAST_APATH);
	m_strBroadcastGroup.LoadString(IDS_BROADCAST_GRP);

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
		if( _tcsnicmp( cstrTmp, m_strBroadcastPerf, cstrTmp.GetLength() ) == 0 )
		{
			m_lPChannel = -4;
			UpdatePChannelName();
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastSeg, cstrTmp.GetLength() ) == 0 )
		{
			m_lPChannel = -1;
			UpdatePChannelName();
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastAPath, cstrTmp.GetLength() ) == 0 )
		{
			m_lPChannel = -2;
			UpdatePChannelName();
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastGroup, cstrTmp.GetLength() ) == 0 )
		{
			m_lPChannel = -3;
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
			else
			{
				if( m_lPChannel == -4 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastPerf );
				}
				else if( m_lPChannel == -1 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastSeg );
				}
				else if( m_lPChannel == -2 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastAPath );
				}
				else if( m_lPChannel == -3 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastGroup );
				}
			}

			UpdatePChannelName();
			m_btnOK.EnableWindow( TRUE );
			return;
		}

		if( _tcsnicmp( cstrTmp, m_strBroadcastPerf, cstrTmp.GetLength() ) == 0 )
		{
			m_editPChannel.SetWindowText( m_strBroadcastPerf );
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastSeg, cstrTmp.GetLength() ) == 0 )
		{
			m_editPChannel.SetWindowText( m_strBroadcastSeg );
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastAPath, cstrTmp.GetLength() ) == 0 )
		{
			m_editPChannel.SetWindowText( m_strBroadcastAPath );
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastGroup, cstrTmp.GetLength() ) == 0 )
		{
			m_editPChannel.SetWindowText( m_strBroadcastGroup );
		}
		else
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
	long lPChannel = m_lPChannel;
	if( lPChannel < 0 )
	{
		switch( lPChannel )
		{
		case -1:
			lPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
			break;
		case -2:
			lPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
			break;
		case -3:
			lPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
			break;
		case -4:
			lPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
			break;
		}
	}

	if( TrackExists( lPChannel ) > 0 )
	{
		m_staticStatus.SetWindowText(m_strUsedPChannel);
	}
	else
	{
		m_staticStatus.SetWindowText(m_strEmptyPChannel);
	}

	// Fill the PChannel name edit box with the PChannel's name
	if( m_pSequenceMgr && m_pSequenceMgr->m_pIPChannelName )
	{
		WCHAR wstrName[MAX_PATH];
		if( SUCCEEDED( m_pSequenceMgr->m_pIPChannelName->GetPChannelName( lPChannel, wstrName) ) )
		{
			m_editPChannelName.SetWindowText( CString( wstrName ) );
		}
	}
}

void CNewPartDlg::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to the segment node
	VARIANT var;
	if( FAILED( m_pSequenceMgr->m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var ) ) )
	{
		ASSERT(FALSE);
		return;
	}

	// Ask for a SegmentEdit inteface
	IDMUSProdSegmentEdit *pSegmentEdit;
	if( FAILED( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdSegmentEdit, (void**)&pSegmentEdit ) ) )
	{
		V_UNKNOWN(&var)->Release();
		ASSERT(FALSE);
		return;
	}

	// Release the reference on the segment node
	V_UNKNOWN(&var)->Release();

	// Assume the strip creation succeeds
	// BUGBUG: We will add an extra undo stip of AddStrip fails.  Oh well.
	m_pSequenceMgr->m_pSequenceStrip->m_nLastEdit = IDS_UNDO_ADD_PART;
	m_pSequenceMgr->m_pTimeline->OnDataChanged( (ISequenceMgr*)m_pSequenceMgr );

	// Try and create a sequence strip
	IUnknown *punkStripMgr;
	if( FAILED( pSegmentEdit->AddStrip( CLSID_DirectMusicSeqTrack, m_pSequenceMgr->m_dwGroupBits, &punkStripMgr ) ) )
	{
		pSegmentEdit->Release();
		ASSERT(FALSE);
		return;
	}

	// Release the reference on the SegmentEdit interface
	pSegmentEdit->Release();

	// Ask for a ISequenceMgr interface
	ISequenceMgr *pSequenceMgr;
	if( FAILED( punkStripMgr->QueryInterface( IID_ISequenceMgr, (void **)&pSequenceMgr ) ) )
	{
		punkStripMgr->Release();
		ASSERT(FALSE);
		return;
	}

	// Release the reference on the sequence strip mgr
	punkStripMgr->Release();

	// Set the PChannel #
	if( m_lPChannel >= 0 )
	{
		pSequenceMgr->SetPChannel( m_lPChannel );
	}
	else if( m_lPChannel == -1 )
	{
		pSequenceMgr->SetPChannel( DMUS_PCHANNEL_BROADCAST_SEGMENT );
	}
	else if( m_lPChannel == -2 )
	{
		pSequenceMgr->SetPChannel( DMUS_PCHANNEL_BROADCAST_AUDIOPATH );
	}
	else if( m_lPChannel == -3 )
	{
		pSequenceMgr->SetPChannel( DMUS_PCHANNEL_BROADCAST_GROUPS );
	}
	else if( m_lPChannel == -4 )
	{
		pSequenceMgr->SetPChannel( DMUS_PCHANNEL_BROADCAST_PERFORMANCE );
	}

	// Release the reference on the ISequenceMgr interface
	pSequenceMgr->Release();

	// Change to the next PChannel
	if( m_lPChannel >= 0 )
	{
		m_lPChannel = min( m_lPChannel + 1, MAX_PCHANNEL - 1);
	}
	else
	{
		m_lPChannel = 0;
	}

	m_spinPChannel.SetPos( m_lPChannel + 1 );
	// If we incremented, this updated the PChannel number display.
	// In all cases, this updates the PChannel used/unused display.
	UpdatePChannelName();
}

// Returns TRUE if the PChannel already is used in this segment
BOOL CNewPartDlg::TrackExists( long lPChannel )
{
	BOOL fFound = FALSE;
	if( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline )
	{
		VARIANT varStripMgr, varPChannel;
		IDMUSProdStrip *pStrip;
		DWORD dwEnum = 0;
		// Iterate through all the strips
		while( !fFound && SUCCEEDED( m_pSequenceMgr->m_pTimeline->EnumStrip( dwEnum, &pStrip ) ) )
		{
			// Get their strip manager
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) ) )
			{
				// Check if they belong to a Sequence strip manager
				IPersist *pIPersist;
				if( SUCCEEDED( V_UNKNOWN( &varStripMgr )->QueryInterface( IID_IPersist, (void**)&pIPersist ) ) )
				{
					CLSID clsid;
					if( SUCCEEDED( pIPersist->GetClassID( &clsid ) )
					&&	(clsid == CLSID_SequenceMgr) )
					{
						// Since they're a sequence strip, check to see which PChannel they're displaying
						if( SUCCEEDED( pStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_PCHANNEL, &varPChannel ) )
						&&	(V_I4(&varPChannel) == lPChannel) )
						{
							fFound = TRUE;
						}
					}
					pIPersist->Release();
				}
				V_UNKNOWN( &varStripMgr )->Release();
			}
			pStrip->Release();
			dwEnum++;
		}
	}

	return fFound;
}

// Returns the greatest PChannel already in use in this segment
long CNewPartDlg::GetGreatestUsedPChannel( void )
{
	long lRetVal = -1;
	if( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline )
	{
		VARIANT varStripMgr, varPChannel;
		IDMUSProdStrip *pStrip;
		DWORD dwEnum = 0;
		// Iterate through all the strips
		while( SUCCEEDED( m_pSequenceMgr->m_pTimeline->EnumStrip( dwEnum, &pStrip ) ) )
		{
			// Get their strip manager
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) ) )
			{
				// Check if they belong to a Sequence strip manager
				IPersist *pIPersist;
				if( SUCCEEDED( V_UNKNOWN( &varStripMgr )->QueryInterface( IID_IPersist, (void**)&pIPersist ) ) )
				{
					CLSID clsid;
					if( SUCCEEDED( pIPersist->GetClassID( &clsid ) )
					&&	(clsid == CLSID_SequenceMgr) )
					{
						// Since they're a sequence strip, check to see which PChannel they're displaying
						if( SUCCEEDED( pStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_PCHANNEL, &varPChannel ) ) )
						{
							lRetVal = max( lRetVal, V_I4(&varPChannel) );
						}
					}
					pIPersist->Release();
				}
				V_UNKNOWN( &varStripMgr )->Release();
			}
			pStrip->Release();
			dwEnum++;
		}
	}

	return lRetVal;
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
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				m_lPChannel = -1;
				*pResult = 1;
			}
			else if( m_lPChannel == -3 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
				m_lPChannel = -2;
				*pResult = 1;
			}
			else if( m_lPChannel == -4 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGroup );
				m_lPChannel = -3;
				*pResult = 1;
			}
			else if( m_lPChannel == -1 )
			{
				m_spinPChannel.SetPos( 1 );
				m_lPChannel = 0;
				*pResult = 1;
			}
			/*
			else if( m_lPCHannel == 0 )
			{
				// Do nothing
			}
			*/
		}
		else // ( pNMUpDown->iDelta <= 0 )
		{
			if( m_lPChannel == -1 )
			{
				m_lPChannel = -2;
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
			}
			else if( m_lPChannel == -2 )
			{
				m_lPChannel = -3;
				m_editPChannel.SetWindowText( m_strBroadcastGroup );
			}
			else if( m_lPChannel == -3 )
			{
				m_lPChannel = -4;
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
			}
			else if( m_lPChannel == 0 )
			{
				m_lPChannel = -1;
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
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
		case 'a':
		case 'A':
		case 't':
		case 'T':
		case 'h':
		case 'H':
		case 'o':
		case 'O':
		case 'u':
		case 'U':
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
