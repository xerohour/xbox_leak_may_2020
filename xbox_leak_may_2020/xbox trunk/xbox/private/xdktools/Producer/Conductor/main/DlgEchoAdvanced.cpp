// DlgEchoAdvanced.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEchoAdvanced.h"
#include "OutputTool.h"
#include "cconduct.h"
#include "DlgMIDI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMIDIInputContainer* g_pMIDIInputContainer;

/////////////////////////////////////////////////////////////////////////////
// DlgEchoAdvanced dialog


DlgEchoAdvanced::DlgEchoAdvanced(CWnd* pParent /*=NULL*/)
	: CDialog(DlgEchoAdvanced::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgEchoAdvanced)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	ZeroMemory( m_anCurrentBlock, sizeof(int) * ECHO_ADVANCED_PORTS );
	m_pDlgMIDI = NULL;
}


void DlgEchoAdvanced::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEchoAdvanced)
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_1, m_acomboEchoInput[0]);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_2, m_acomboEchoInput[1]);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_3, m_acomboEchoInput[2]);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_4, m_acomboEchoInput[3]);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_5, m_acomboEchoInput[4]);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_6, m_acomboEchoInput[5]);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_7, m_acomboEchoInput[6]);
	DDX_Control(pDX, IDC_COMBO_ECHO_INPUT_8, m_acomboEchoInput[7]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_1, m_aspinEchoPChannels[0]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_2, m_aspinEchoPChannels[1]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_3, m_aspinEchoPChannels[2]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_4, m_aspinEchoPChannels[3]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_5, m_aspinEchoPChannels[4]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_6, m_aspinEchoPChannels[5]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_7, m_aspinEchoPChannels[6]);
	DDX_Control(pDX, IDC_SPIN_ECHO_PCHANNELS_8, m_aspinEchoPChannels[7]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_1, m_aeditEchoPChannels[0]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_2, m_aeditEchoPChannels[1]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_3, m_aeditEchoPChannels[2]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_4, m_aeditEchoPChannels[3]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_5, m_aeditEchoPChannels[4]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_6, m_aeditEchoPChannels[5]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_7, m_aeditEchoPChannels[6]);
	DDX_Control(pDX, IDC_EDIT_ECHO_PCHANNELS_8, m_aeditEchoPChannels[7]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgEchoAdvanced, CDialog)
	//{{AFX_MSG_MAP(DlgEchoAdvanced)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_1, OnDeltaposSpinEchoPchannels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_2, OnDeltaposSpinEchoPchannels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_3, OnDeltaposSpinEchoPchannels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_4, OnDeltaposSpinEchoPchannels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_5, OnDeltaposSpinEchoPchannels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_6, OnDeltaposSpinEchoPchannels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_7, OnDeltaposSpinEchoPchannels)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ECHO_PCHANNELS_8, OnDeltaposSpinEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_1, OnKillfocusEditEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_2, OnKillfocusEditEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_3, OnKillfocusEditEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_4, OnKillfocusEditEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_5, OnKillfocusEditEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_6, OnKillfocusEditEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_7, OnKillfocusEditEchoPchannels)
	ON_EN_KILLFOCUS(IDC_EDIT_ECHO_PCHANNELS_8, OnKillfocusEditEchoPchannels)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgEchoAdvanced message handlers

BOOL DlgEchoAdvanced::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Ensure we have a valid window handle
	if( (m_acomboEchoInput[0].GetSafeHwnd() == NULL)
	||	(NULL == m_pDlgMIDI) )
	{
		return FALSE;
	}

	// Loop through all available DMusic ports, adding them to the combo boxes and
	// looking for existing 'Echo MIDI' input ports
	bool afFoundEchoInput[MIDI_IN_PORTS] = {false};
	HRESULT	hr;
	DMUS_PORTCAPS dmpc;
	for( DWORD dwDMIndex = 0; ; dwDMIndex++ )
	{
		// Initialize the PORTCAPS structure
		ZeroMemory(&dmpc, sizeof(dmpc));
		dmpc.dwSize = sizeof(DMUS_PORTCAPS);

		// Enumerate the next port
		hr = g_pconductor->m_pDMusic->EnumPort( dwDMIndex, &dmpc );
		if(SUCCEEDED(hr) && hr != S_FALSE)
		{
			if( dmpc.dwClass == DMUS_PC_INPUTCLASS )
			{
				// Found an input port - add it to the combo box

				// Create a GUID to save this port's GUID
				GUID *pGuid;
				pGuid = new GUID;
				ASSERT( pGuid );
				CopyMemory( pGuid, &dmpc.guidPort, sizeof(GUID) );

				//TRACE("Adding %S %x\n", dmpc.wszDescription, dmpc.guidPort.Data1 );

				// Point the combo box item's data ptr at the port's GUID
				CString strDesc = dmpc.wszDescription;
				for( int i=0; i<ECHO_ADVANCED_PORTS; i++ )
				{
					// Add the item to the Echo input combo box
					DWORD dwComboIndex = m_acomboEchoInput[i].AddString( strDesc );
					m_acomboEchoInput[i].SetItemDataPtr( dwComboIndex, (void *)pGuid );

					// If we found the currently selected port, select it
					// In the MIDIInputContainer, the Echo ports start at 1, but we're
					// iterating from zero, so add one.
					if ( ::IsEqualGUID( m_pDlgMIDI->m_aguidEchoMIDIInPort[i], dmpc.guidPort ) )
					{
						afFoundEchoInput[i] = true;
						m_acomboEchoInput[i].SetCurSel( dwComboIndex );
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	// Add a 'none' option - it has the data UINT_MAX
	CString strNoMIDI;
	strNoMIDI.LoadString( IDS_NO_MIDI_INPUT );

	// Add the 'none' option to the Echo MIDI input box
	for( int i=0; i<ECHO_ADVANCED_PORTS; i++ )
	{
		DWORD dwComboIndex = m_acomboEchoInput[i].InsertString( 0, strNoMIDI );
		m_acomboEchoInput[i].SetItemData( dwComboIndex, UINT_MAX );
		m_acomboEchoInput[i].SetItemDataPtr( dwComboIndex, (void *)UINT_MAX );

		// By default, select 'None'
		if( !afFoundEchoInput[i] )
		{
			m_acomboEchoInput[i].SetCurSel(dwComboIndex);
		}

		m_aspinEchoPChannels[i].SetRange( 0, 250 );
		UpdatePChannelEdit( i, m_pDlgMIDI->m_anCurrentBlock[i] );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgEchoAdvanced::OnOK() 
{
	for( int i=0; i<ECHO_ADVANCED_PORTS; i++ )
	{
		GUID guidEchoInputPort = GUID_AllZeros;

		// Get the index of the currently selected item
		int nIndex = m_acomboEchoInput[i].GetCurSel();
		if (nIndex == CB_ERR)
		{
			// Error getting index of selected item - perhaps nothing's selected?
			ASSERT( FALSE );
		}
		else
		{
			// Get the selected port GUID
			if( m_acomboEchoInput[i].GetItemDataPtr( nIndex ) != (void *)UINT_MAX )
			{
				GUID *pGuid = (GUID *) m_acomboEchoInput[i].GetItemDataPtr( nIndex );
				memcpy( &guidEchoInputPort, pGuid, sizeof(GUID) );
			}
		}

		// Update the main MIDI option dialog
		m_pDlgMIDI->m_aguidEchoMIDIInPort[i] = guidEchoInputPort;
		m_pDlgMIDI->m_anCurrentBlock[i] = m_anCurrentBlock[i];
	}
	
	CDialog::OnOK();
}

void DlgEchoAdvanced::UpdatePChannelEdit( DWORD dwIndex, int nBlock )
{
	// Validate the block number
	if( nBlock < 0 )
	{
		return;
	}

	// Validate the index
	if( dwIndex >= ECHO_ADVANCED_PORTS )
	{
		return;
	}

	// Copy it to our local variable (since the spin control doesn't store it)
	m_anCurrentBlock[dwIndex] = nBlock;

	// Compute the text to display, and then display it
	CString strText, strFormat;
	strFormat.LoadString( IDS_PCHANNEL_GROUP_FORAMT );
	strText.Format( strFormat, nBlock * 16 + 1, nBlock * 16 + 16 );
	m_aeditEchoPChannels[dwIndex].SetWindowText( strText );
}

void DlgEchoAdvanced::OnDeltaposSpinEchoPchannels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	DWORD dwIndex = pNMUpDown->hdr.idFrom - IDC_SPIN_ECHO_PCHANNELS_1;

	ASSERT( dwIndex < ECHO_ADVANCED_PORTS );

	if( dwIndex >= ECHO_ADVANCED_PORTS )
	{
		return;
	}

	// Calculate what the block # will change to
	int nNewBlock = m_anCurrentBlock[dwIndex] + pNMUpDown->iDelta;

	// If the block number is invalid (negative), return
	if( nNewBlock < 0 )
	{
		*pResult = 1;
		return;
	}

	// Update the text in the PChannel edit box
	UpdatePChannelEdit( dwIndex, nNewBlock );

	*pResult = 1;
}

void DlgEchoAdvanced::OnKillfocusEditEchoPchannels() 
{
	for( int i=0; i<ECHO_ADVANCED_PORTS; i++ )
	{
		CString strText;
		m_aeditEchoPChannels[i].GetWindowText( strText );
		
		// Strip leading and trailing spaces
		strText.TrimRight();
		strText.TrimLeft();

		// If empty set value to previous one
		if( strText.IsEmpty() )
		{
			UpdatePChannelEdit( i, m_anCurrentBlock[i] );
		}
		else
		{
			// Convert from text to int
			int nNewBlock;
			TCHAR *tcstrTmp;
			tcstrTmp = new TCHAR[strText.GetLength() + 1];
			tcstrTmp[0] = 0;
			if( _stscanf( strText, "%d%s", &nNewBlock, tcstrTmp ) )
			{
				// Check bounds
				if( nNewBlock < 0 )
				{
					nNewBlock = 0;
				}
				else if( nNewBlock > 999 )
				{
					nNewBlock = 999;
				}

				nNewBlock /= 16;
				UpdatePChannelEdit( i, nNewBlock );
			}
			else
			{
				// Invalid data - reset control with previous value
				UpdatePChannelEdit( i, m_anCurrentBlock[i] );
			}

			delete tcstrTmp;
		}
	}
}

void DlgEchoAdvanced::DisplayPortCreationError( HRESULT hr, CComboBox &comboBox )
{
	// Failed to set Conductor's MIDI input
	CString strError, strFormat;
	strFormat.LoadString( IDS_ERR_UNKNOWN_CREATE_PORT );

	CString strName;
	comboBox.GetLBText( comboBox.GetCurSel(), strName );
	strError.Format( strFormat, hr, strName );

	AfxMessageBox( strError, MB_ICONEXCLAMATION | MB_OK );
}
