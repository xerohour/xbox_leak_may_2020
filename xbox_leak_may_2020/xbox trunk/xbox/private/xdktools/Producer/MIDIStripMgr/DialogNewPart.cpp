// DialogNewPart.cpp : implementation file
//

#include "stdafx.h"
#include "midistripmgr.h"
#include "DialogNewPart.h"
#include "Pattern.h"
#include "DialogSelectPartRef.h"
#include <PChannelName.h>
#include "MIDIMgr.h"
#include <riffstrm.h>
#include "StyleDesigner.h"
#include "DialogLinkExisting.h"
#include "SharedPianoRoll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogNewPart dialog


CDialogNewPart::CDialogNewPart(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogNewPart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogNewPart)
	//}}AFX_DATA_INIT
	m_nTrack = 0;
	m_pMIDIMgr = NULL;
	m_pDMPartRef = NULL;
	m_pDMPart = NULL;
}

// Set the track number
void CDialogNewPart::SetTrack( int nTrack )
{
	m_nTrack = nTrack;
}

// Set m_pMIDIMgr
void CDialogNewPart::SetMIDIMgr( CMIDIMgr *pMIDIMgr )
{
	ASSERT( pMIDIMgr != NULL );
	if ( pMIDIMgr == NULL )
	{
		return;
	}
	m_pMIDIMgr = pMIDIMgr;
}

void CDialogNewPart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogNewPart)
	DDX_Control(pDX, IDC_EDIT_PCHNAME, m_editPChName);
	DDX_Control(pDX, IDC_BUTTON_EXISTING, m_btnExisting);
	DDX_Control(pDX, IDC_PCHANNEL_STATUS, m_staticStatus);
	DDX_Control(pDX, IDC_NEWPART_EDIT_PCHANNEL, m_editPChannel);
	DDX_Control(pDX, IDC_SPIN_PCHANNEL, m_spinPChannel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogNewPart, CDialog)
	//{{AFX_MSG_MAP(CDialogNewPart)
	ON_EN_CHANGE(IDC_NEWPART_EDIT_PCHANNEL, OnChangeNewpartEditPchannel)
	ON_EN_KILLFOCUS(IDC_NEWPART_EDIT_PCHANNEL, OnKillfocusNewpartEditPchannel)
	ON_BN_CLICKED(IDC_BUTTON_EXISTING, OnButtonExisting)
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PCHANNEL, OnDeltaposSpinPchannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogNewPart message handlers

BOOL CDialogNewPart::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CDialog::OnInitDialog();

	// Make sure the MIDIMgr pointer is valid
	ASSERT( m_pMIDIMgr != NULL );
	if ( m_pMIDIMgr == NULL )
	{
		return TRUE;
	}

	// By default, don't enable the part link dialog
	::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_LINK ), FALSE );

	// Check if we should show the part link dialog
	VARIANT var;
	if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var )))
	{
		// Look for an IPatternNodePrivate interface
		IPatternNodePrivate *pPatternNodePrivate;
		if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IPatternNodePrivate, (void**)&pPatternNodePrivate ) ) )
		{
			if( pPatternNodePrivate->CanShowPartLinkDialog() == S_OK )
			{
				::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_LINK ) , TRUE );
			}
			pPatternNodePrivate->Release();
		}
		// Look for a DirectMusic Track
		else if( m_pMIDIMgr->m_pIDMTrack )
		{
			// Look for existing parts
			if( !m_pMIDIMgr->m_lstStyleParts.IsEmpty() )
			{
				::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON_LINK ) , TRUE );
			}
		}
		V_UNKNOWN(&var)->Release();
	}

	m_nTrack = GetFirstUsedPChannel();

	m_spinPChannel.SetRange(1, MAX_PCHANNEL);
	m_spinPChannel.SetPos( m_nTrack + 1 );

	m_editPChannel.LimitText(5); // Length of 'APath'

	m_strEmptyPChannel.LoadString(IDS_EMPTY_PCHANNEL);
	m_strSinglePChannel.LoadString(IDS_SINGLE_PARTREF);
	m_strMultiplePChannel.LoadString(IDS_MULTIPLE_PARTREF);
	m_strInvalidPChannel.LoadString(IDS_INVALID_PCHANNEL);

	m_strDisplayPartRef.LoadString(IDS_DISPLAY_PARTREF);
	m_strChoosePartRef.LoadString(IDS_CHOOSE_PARTREF);

	m_strBroadcastSeg.LoadString(IDS_BROADCAST_SEG);
	m_strBroadcastPerf.LoadString(IDS_BROADCAST_PERF);
	m_strBroadcastAPath.LoadString(IDS_BROADCAST_APATH);
	m_strBroadcastGroup.LoadString(IDS_BROADCAST_GRP);

	UpdatePChannelStatus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogNewPart::OnChangeNewpartEditPchannel() 
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
			m_editPChName.SetWindowText("--------");
			return;
		}

		if( _tcsnicmp( cstrTmp, m_strBroadcastPerf, cstrTmp.GetLength() ) == 0 )
		{
			m_nTrack = -4;
			UpdatePChannelStatus();
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastSeg, cstrTmp.GetLength() ) == 0 )
		{
			m_nTrack = -1;
			UpdatePChannelStatus();
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastAPath, cstrTmp.GetLength() ) == 0 )
		{
			m_nTrack = -2;
			UpdatePChannelStatus();
		}
		else if( _tcsnicmp( cstrTmp, m_strBroadcastGroup, cstrTmp.GetLength() ) == 0 )
		{
			m_nTrack = -3;
			UpdatePChannelStatus();
		}
		else
		{
			BOOL fTransSucceeded;
			int nPChannel = GetDlgItemInt( IDC_NEWPART_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
			if ( !fTransSucceeded || (nPChannel < 1) || (nPChannel > MAX_PCHANNEL) )
			{
				m_staticStatus.SetWindowText(m_strInvalidPChannel);
				m_editPChName.SetWindowText("--------");
			}
			else
			{
				m_nTrack = nPChannel - 1;

				UpdatePChannelStatus();
			}
		}
	}

}

void CDialogNewPart::OnKillfocusNewpartEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		CString cstrTmp;
		m_editPChannel.GetWindowText( cstrTmp );

		// Strip leading and trailing spaces
		cstrTmp.TrimRight();
		cstrTmp.TrimLeft();

		// Exit if empty
		if ( cstrTmp.IsEmpty() )
		{
			if( m_nTrack >= 0 )
			{
				m_spinPChannel.SetPos( m_nTrack + 1 );
			}
			else
			{
				if( m_nTrack == -4 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastPerf );
				}
				else if( m_nTrack == -1 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastSeg );
				}
				else if( m_nTrack == -2 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastAPath );
				}
				else if( m_nTrack == -3 )
				{
					m_editPChannel.SetWindowText( m_strBroadcastGroup );
				}
			}

			UpdatePChannelStatus();
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
				nPChannel = 1;
				m_nTrack = 0;
				m_spinPChannel.SetPos( nPChannel );
			}
			else if ( nPChannel > MAX_PCHANNEL )
			{
				nPChannel = MAX_PCHANNEL;
				m_nTrack = MAX_PCHANNEL - 1;
				m_spinPChannel.SetPos( nPChannel );
			}
			else
			{
				// m_nTrack should have been set by OnChangeNewpartEditPchannel()
				ASSERT(m_nTrack == nPChannel - 1 );
			}
		}

		UpdatePChannelStatus();
	}
}

void CDialogNewPart::OnButtonExisting() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If true, call CDialog::OnOK to close the dialog
	BOOL fDoOK = TRUE;

	DWORD dwPChannel = 0;
	if( m_nTrack >= 0 )
	{
		dwPChannel = m_nTrack;
	}
	else if( m_nTrack == -1 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
	}
	else if( m_nTrack == -2 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
	}
	else if( m_nTrack == -3 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
	}
	else if( m_nTrack == -4 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
	}

	// If multiple part refs available, display a dialog letting the user
	// chose which one to display.
	if( NumberOfPartRefs( dwPChannel ) > 1 )
	{
		// Create and initialize the 'Choose PartRef' dialog
		CDialogSelectPartRef dlg;
		dlg.m_dwPChannel = dwPChannel;
		dlg.m_pDMPattern = m_pMIDIMgr->m_pDMPattern;

		// Display the dialog
		if( (dlg.DoModal() != IDOK) || (dlg.m_pDMPartRef == NULL) )
		{
			// If user canceled, don't close the dialog
			fDoOK = FALSE;
		}
		else
		{
			// User didn't cancel, set our PartRef pointer with the one they chose
			m_pDMPartRef = dlg.m_pDMPartRef;
		}
	}

	// If the user didn't cancel, close this dialog
	if( fDoOK )
	{
		DoCreatePart( IDC_BUTTON_EXISTING );
	}
}

void CDialogNewPart::OnButtonLink() 
{
	// Display a dialog letting the user chose which part to link to
	VARIANT var;
	if (SUCCEEDED(m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var )))
	{
		BOOL fTryAndLinkPart = FALSE;
		GUID guidPart = {0};
		IStream *pIStream = NULL;

		// Look for an IPatternNodePrivate interface
		IPatternNodePrivate *pPatternNodePrivate;
		if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IPatternNodePrivate, (void**)&pPatternNodePrivate ) ) )
		{
			if( pPatternNodePrivate->DisplayPartLinkDialog( &guidPart, &pIStream ) == S_OK )
			{
				fTryAndLinkPart = TRUE;
			}
			pPatternNodePrivate->Release();
		}
		// Look for a DirectMusic Track
		else if( m_pMIDIMgr->m_pIDMTrack )
		{
			// Display our own Link part dialog.
	
			// Display a dialog letting the user chose which part to link to
			CDialogLinkExisting dlg;
			dlg.m_pDMPattern = m_pMIDIMgr->m_pDMPattern;

			if( dlg.DoModal() == IDOK )
			{
				fTryAndLinkPart = TRUE;
				memcpy( &guidPart, &(dlg.m_pDMPartRef->m_pDMPart->m_guidPartID), sizeof(GUID) );
				dlg.m_pDMPartRef->m_fHardLink = TRUE;
			}
		}

		if( fTryAndLinkPart )
		{
			// See if they selected a part that is in this pattern
			CDirectMusicPartRef *pPartRef;
			pPartRef = m_pMIDIMgr->m_pDMPattern->FindPartRefByGUID( guidPart );
				
			// Yep - just point ourself at the part
			if( pPartRef )
			{
				m_pDMPart = pPartRef->m_pDMPart;
				// If necessary, m_fHardLink flag was set by pPatternNodePrivate->DisplayPartLinkDialog()
				// and we were re-loaded already.
				//pPartRef->m_fHardLink = TRUE;
			}
			else
			{
				// Nope - we need to create a new part
				ASSERT( pIStream );

				// First, Create a RiffStream
				IDMUSProdRIFFStream* pIRiffStream;
				if( pIStream && SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
				{
					MMCKINFO ckMain;
					ckMain.fccType = DMUS_FOURCC_PART_LIST;
					if( pIRiffStream->Descend( &ckMain, 0, MMIO_FINDLIST ) == 0 )
					{
						// Now, create the part
						m_pDMPart = m_pMIDIMgr->m_pDMPattern->m_pMIDIMgr->AllocPart();

						// And load the part
						m_pDMPart->DM_LoadPart( pIRiffStream, &ckMain );
					}

					pIRiffStream->Release();
				}
			}

			// Release the pIStream pointer
			if( pIStream )
			{
				pIStream->Release();
				// No need - not used again
				//pIStream = NULL;
			}

			// Create the partref
			DoCreatePart( IDC_BUTTON_LINK );
		}
		V_UNKNOWN(&var)->Release();
	}
}

int CDialogNewPart::GetFirstUsedPChannel( void )
{
	int iPChannel = 0;

	// Iterate through all PartRefs
	CDirectMusicPartRef *pPartRef;
	POSITION pos = m_pMIDIMgr->m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		pPartRef = m_pMIDIMgr->m_pDMPattern->m_lstPartRefs.GetNext( pos );

		// If PartRef's pChannel is lesser thatn dwPChannel, update dwPChannel
		if( iPChannel < signed(pPartRef->m_dwPChannel) )
		{
			iPChannel = pPartRef->m_dwPChannel;
		}
	}

	// Return the greatest PChannel number, plus 1
	return iPChannel + 1;
}

// Returns 0, 1, or 2 (representing any # greater than 1)
int CDialogNewPart::NumberOfPartRefs( DWORD dwPChannel )
{
	// Initially, none found
	int nFound = 0;

	// Iterate through all PartRefs
	CDirectMusicPartRef *pPartRef;
    POSITION pos = m_pMIDIMgr->m_pDMPattern->m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_pMIDIMgr->m_pDMPattern->m_lstPartRefs.GetNext( pos );

		if( dwPChannel == pPartRef->m_dwPChannel )
		{
			// Found one, increment our counter
			nFound++;
			if( nFound > 1 )
			{
				// We only care about 0, 1, or greater than 1.
				break;
			}
		}
    }

	return nFound;
}

void CDialogNewPart::UpdatePChannelStatus( void )
{
	DWORD dwPChannel = 0;
	if( m_nTrack >= 0 )
	{
		dwPChannel = m_nTrack;
	}
	else if( m_nTrack == -1 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
	}
	else if( m_nTrack == -2 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
	}
	else if( m_nTrack == -3 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
	}
	else if( m_nTrack == -4 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
	}

	// Compute number of PartRefs playing on the given PChannel
	int nFound = NumberOfPartRefs( dwPChannel );

	if ( nFound == 0 )
	{
		// None found - disable 'Display Existing PartRef' button
		m_staticStatus.SetWindowText(m_strEmptyPChannel);
		m_btnExisting.EnableWindow(FALSE);
		m_btnExisting.SetWindowText(m_strDisplayPartRef);
	}
	else if( nFound == 1 )
	{
		// One found - enable 'Display Existing PartRef' button
		m_staticStatus.SetWindowText(m_strSinglePChannel);
		m_btnExisting.EnableWindow(TRUE);
		m_btnExisting.SetWindowText(m_strDisplayPartRef);
	}
	else
	{
		// More than one found - enable 'Display Existing PartRef...' button
		m_staticStatus.SetWindowText(m_strMultiplePChannel);
		m_btnExisting.EnableWindow(TRUE);
		m_btnExisting.SetWindowText(m_strChoosePartRef);
	}
	
	// Fill the PChannel name edit box with the PChannel's name
	if( m_pMIDIMgr->m_pIPChannelName )
	{
		WCHAR wstrName[MAX_PATH];
		if( SUCCEEDED( m_pMIDIMgr->m_pIPChannelName->GetPChannelName( dwPChannel, wstrName) ) )
		{
			m_editPChName.SetWindowText( CString( wstrName ) );
		}
	}
}

void CDialogNewPart::OnDeltaposSpinPchannel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	*pResult = 0;

	if( pNMUpDown && (m_nTrack <= 0) )
	{
		if( pNMUpDown->iDelta > 0 )
		{
			if( m_nTrack == -2 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				m_nTrack = -1;
				*pResult = 1;
			}
			else if( m_nTrack == -1 )
			{
				m_spinPChannel.SetPos( 1 );
				m_nTrack = 0;
				*pResult = 1;
			}
			else if( m_nTrack == -3 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
				m_nTrack = -2;
				*pResult = 1;
			}
			else if( m_nTrack == -4 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGroup );
				m_nTrack = -3;
				*pResult = 1;
			}
		}
		else // ( pNMUpDown->iDelta <= 0 )
		{
			if( m_nTrack == -1 )
			{
				m_nTrack = -2;
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
				*pResult = 1;
			}
			else if( m_nTrack == 0 )
			{
				m_nTrack = -1;
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				*pResult = 1;
			}
			else if( m_nTrack == -3 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
				m_nTrack = -4;
				*pResult = 1;
			}
			else if( m_nTrack == -2 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGroup );
				m_nTrack = -3;
				*pResult = 1;
			}
			else if( m_nTrack == -4 )
			{
				// Do nothing
				*pResult = 1;
			}
		}
	}
}

void CDialogNewPart::OnCancel() 
{
	CDialog::OnCancel();
}

void CDialogNewPart::OnOK() 
{
	// Create new part
	DoCreatePart( IDOK );
}

void CDialogNewPart::DoCreatePart( int nButtonClicked )
{
	// Initialize PartRef and Part pointers to NULL
	CDirectMusicPartRef *pDMPartRef = NULL;
	CDirectMusicPart* pDMPart = NULL;

	DWORD dwPChannel = 0;
	if( m_nTrack >= 0 )
	{
		dwPChannel = m_nTrack;
	}
	else if( m_nTrack == -1 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
	}
	else if( m_nTrack == -2 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
	}
	else if( m_nTrack == -3 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
	}
	else if( m_nTrack == -4 )
	{
		dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
	}

	switch( nButtonClicked )
	{
	case IDOK:
		// Create new part
		break;
	case IDC_BUTTON_LINK:
		// Link to existing part
		ASSERT( m_pDMPart );
		pDMPart = m_pDMPart;
		
		// Refresh gutters of all strips that use this part
		{
			POSITION position = m_pMIDIMgr->m_pPRSList.GetHeadPosition();
			while (position != NULL)
			{
				CPianoRollStrip* pPianoRollStrip = m_pMIDIMgr->m_pPRSList.GetNext(position);
				if ( (pPianoRollStrip->m_pPartRef != NULL) &&
					 (pPianoRollStrip->m_pPartRef->m_pDMPart == pDMPart) )
				{
					// Invalidate only the function bar, not the entire strip
					pPianoRollStrip->InvalidateFunctionBar();
				}
			}
		}
		break;
	case IDC_BUTTON_EXISTING:
		// Display existing part
		if( m_pDMPartRef )
		{
			// More than one partRef on this channel - use user's selection
			pDMPartRef = m_pDMPartRef;
		}
		else
		{
			// Only on PartRef on this channel - find it and use it
			pDMPartRef = m_pMIDIMgr->m_pDMPattern->FindPartRefByPChannel( dwPChannel );
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	if( pDMPartRef == NULL )
	{
		// The PartRef doesn't exist yet, so create a new one
		pDMPartRef = m_pMIDIMgr->m_pDMPattern->AllocPartRef();
		ASSERT( pDMPartRef != NULL );
		if( pDMPartRef == NULL )
		{
			// TODO: Error dialog of some sort?
			TRACE("CDialogNewPart::DoCreatePart out of memory");
			return;
		}

		m_pMIDIMgr->m_fPChannelChange = TRUE;

		if( pDMPart == NULL )
		{
			// The Part doesn't exist yet, so create a new one
			pDMPart = m_pMIDIMgr->AllocPart();

			// Initialize the part's TimeSig and length with the Pattern's
			pDMPart->m_TimeSignature = m_pMIDIMgr->m_pDMPattern->m_TimeSignature;
			pDMPart->m_mtClocksPerBeat = DM_PPQNx4 / (long)pDMPart->m_TimeSignature.m_bBeat;
			pDMPart->m_mtClocksPerGrid = pDMPart->m_mtClocksPerBeat / (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
			pDMPart->m_wNbrMeasures = m_pMIDIMgr->m_pDMPattern->m_wNbrMeasures;

			// If drum track, set playmode to DMUS_PLAYMODE_FIXED
			if( (m_nTrack > 0) && ((m_nTrack % 16) == 9) )
			{
				pDMPart->m_bPlayModeFlags = DMUS_PLAYMODE_FIXED;
			}
		}
		else
		{
			// The Part does exist - we're linking to it
			pDMPartRef->m_fHardLink = TRUE;
		}

		// This should never occur...
		ASSERT( pDMPart != NULL );
		if( pDMPart == NULL )
		{
			m_pMIDIMgr->m_pDMPattern->DeletePartRef( pDMPartRef );
			// TODO: Error dialog of some sort?
			TRACE("CDialogNewPart::DoCreatePart out of memory");
			return;
		}

		// Link the PartRef to the part and set its PChannel
		pDMPartRef->SetPart( pDMPart );
		pDMPartRef->m_dwPChannel = dwPChannel;

		// Calculate the MIDI Values (necessary if this is a new part)
		pDMPartRef->RecalcMIDIValues();

		// Create a default PartRef name

		// If in a Pattern track, name the PartRef "Pattern"
		if( m_pMIDIMgr->m_pIDMTrack )
		{
			pDMPartRef->m_strName.LoadString( IDS_PATTERN_TEXT );
		}
		// Otherwise, convert the PChannel # to a string
		else
		{
			if( m_nTrack >= 0 )
			{
				CString cstrTrack;
				cstrTrack.LoadString( IDS_TRACK );
				pDMPartRef->m_strName.Format( cstrTrack, m_nTrack + 1 );
			}
			else if( m_nTrack == -1 )
			{
				pDMPartRef->m_strName.LoadString( IDS_BROADCAST_SEGMENT );
			}
			else if( m_nTrack == -2 )
			{
				pDMPartRef->m_strName.LoadString( IDS_BROADCAST_AUDIOPATH );
			}
			else if( m_nTrack == -3 )
			{
				pDMPartRef->m_strName.LoadString( IDS_BROADCAST_GROUP );
			}
			else if( m_nTrack == -4 )
			{
				pDMPartRef->m_strName.LoadString( IDS_BROADCAST_PERFORMANCE );
			}
		}
	}

	CPianoRollStrip* pPianoRollStrip = m_pMIDIMgr->CreatePianoRollStrip( pDMPartRef );

	if( pPianoRollStrip )
	{
		// Make sure there is a curve strip for each type of Curve in this Part
		pDMPartRef->m_pDMPart->AddCurveTypesFromData();

		// 19279: Create curve strip states for all curves or empty curve strips in this part
		int i;
		for( i=0; i< 17; i++ )
		{
			int j;
			for( j=0; j < 8; j++)
			{
				if( pDMPartRef->m_pDMPart->m_bHasCurveTypes[i] & (1 << j) )
				{
					pPianoRollStrip->GetCurveStripState( BYTE(i * 8 + j), 0 );
				}
			}
		}

		// Add Piano Roll Strip to the Timeline
		if( m_pMIDIMgr->m_pIDMTrack )
		{
			m_pMIDIMgr->m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)pPianoRollStrip, CLSID_DirectMusicPatternTrack, m_pMIDIMgr->m_dwGroupBits, PChannelToStripIndex( pDMPartRef->m_dwPChannel ) );
		}
		else
		{
			DWORD dwPosition;
			dwPosition = m_pMIDIMgr->DeterminePositionForPianoRollStrip( pDMPartRef );
			m_pMIDIMgr->m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)pPianoRollStrip, dwPosition );
			//m_pTimeline->AddStrip( (IDMUSProdStrip *)pPianoRollStrip );
		}

		// Add Curve Strip to the Timeline
		pPianoRollStrip->m_CurveStripView = SV_MINIMIZED;
		pPianoRollStrip->AddCurveStrips( FALSE );

		// Get the strip's default design info
		ioPianoRollDesign ioPRD;
		pPianoRollStrip->GetDesignInfo( &ioPRD );

		// Get the style's notation setting if we're not a drum track
		if( (pDMPartRef->m_dwPChannel & 0xF) != 9 )
		{
			DWORD dwType = 0;
			IDMUSProdStyleInfo *pStyleInfo;
			if( m_pMIDIMgr->m_pIStyleNode
			&&	SUCCEEDED( m_pMIDIMgr->m_pIStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pStyleInfo ) ) )
			{
				if( SUCCEEDED( pStyleInfo->GetNotationType( &dwType ) ) )
				{
					if( (unsigned)ioPRD.m_fHybridNotation != dwType )
					{
						ioPRD.m_fHybridNotation = dwType;
						ioPRD.m_dblVerticalZoom *= (dwType == 1) ? 1.6 : (1.0 / 1.6) ;
						// This is approximately correct
						if( dwType )
						{
							ioPRD.m_lVerticalScroll = MulDiv( ioPRD.m_lVerticalScroll, 38, 128 );
						}
						else
						{
							ioPRD.m_lVerticalScroll = MulDiv( ioPRD.m_lVerticalScroll, 128, 38 );
						}
					}
				}
				pStyleInfo->Release();
			}
		}

		// We need to do this to make the scroll bar appear
		pPianoRollStrip->SetDesignInfo( &ioPRD );

		// Make the strip the active strip
		VARIANT var;
		var.vt = VT_UNKNOWN;
		if( SUCCEEDED( pPianoRollStrip->QueryInterface( IID_IUnknown, (void **)&(V_UNKNOWN(&var)) ) ) )
		{
			m_pMIDIMgr->m_pDMPattern->m_fInLoad = TRUE; // This keeps the strip from updating the Style Designer
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
			m_pMIDIMgr->m_pDMPattern->m_fInLoad = FALSE;
		}

		// Now, make the strip the top strip
		var.vt = VT_UNKNOWN;
		if( SUCCEEDED( pPianoRollStrip->QueryInterface( IID_IUnknown, (void **)&(V_UNKNOWN(&var)) ) ) )
		{
			m_pMIDIMgr->m_pTimeline->SetTimelineProperty( TP_TOP_STRIP, var );
		}

		// save ourself, since something's changed
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_ADD_PART_DISPLAY );

		// Change to the next PChannel
		if( m_nTrack >= 0 )
		{
			m_nTrack = min( m_nTrack + 1, MAX_PCHANNEL - 1);
		}
		else
		{
			m_nTrack = 0;
		}
		m_spinPChannel.SetPos( m_nTrack + 1 );
		UpdatePChannelStatus();
	}
	else
	{
		// TODO: Error dialog of some sort?
		TRACE("CDialogNewPart::DoCreatePart unable to create pianoroll strip");
		return;
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
