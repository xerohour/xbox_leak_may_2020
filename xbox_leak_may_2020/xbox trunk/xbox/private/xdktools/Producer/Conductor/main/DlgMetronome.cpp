// DlgMetronome.cpp : implementation file
//

#include "stdafx.h"
#include "cconduct.h"
#include "DlgMetronome.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char *gacMidiValueToName[12] = {
"C ",
"C#",
"D ",
"D#",
"E ",
"F ",
"F#",
"G ",
"G#",
"A ",
"A#",
"B " };

const static char *gacDrums[61] = {
"High Q",
"Slap ",
"Scratch Push",
"Scratch Pull",
"Sticks",
"Square Click",
"Click Metronome",
"Bell Metronome",
"Kick Drum 2",
"Kick Drum 1",
"Side Stick",
"Snare Drum 1",
"Hand Clap",
"Snare Drum 2",
"Low Tom 2",
"Closed Hi-Hat",
"Low Tom 1",
"Pedal Hi-Hat",
"Mid Tom 2",
"Open Hi-Hat",
"Mid Tom 1",
"Hi Tom 2 ",
"Crash Cymbal 1",
"Hi Tom 1",
"Ride Cymbal 1",
"Chinese Cymbal ",
"Ride Bell",
"Tambourine",
"Splash Cymbal",
"Cowbell",
"Crash Cymbal 2",
"Vibra-slap",
"Ride Cymbal 2",
"Hi Bongo",
"Low Bongo",
"Mute Hi Conga",
"Open Hi Conga",
"Low Conga",
"Hi Timbale",
"Low Timbale",
"Hi Agogo",
"Low Agogo",
"Cabasa",
"Maracas",
"Short Whistle",
"Long Whistle",
"Short Guiro",
"Long Guiro",
"Claves",
"Hi Woodblock",
"Low Woodblock",
"Mute Cuica",
"Open Cuica",
"Mute Triangle",
"Open Triangle",
"Shaker",
"Jingle Bell",
"Belltree",
"Castanets",
"Mute Surdo",
"Open Surdo" };

/////////////////////////////////////////////////////////////////////////////
// DlgMetronome dialog


DlgMetronome::DlgMetronome(CWnd* pParent /*=NULL*/)
	: CDialog(DlgMetronome::IDD, pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//{{AFX_DATA_INIT(DlgMetronome)
	//}}AFX_DATA_INIT
}


void DlgMetronome::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgMetronome)
	DDX_Control(pDX, IDC_SPIN_OTHER_VELOCITY, m_spinOtherVelocity);
	DDX_Control(pDX, IDC_EDIT_OTHER_VELOCITY, m_editOtherVelocity);
	DDX_Control(pDX, IDC_COMBO_OTHER_VALUE, m_comboOtherValue);
	DDX_Control(pDX, IDC_SPIN_COUNTIN, m_spinCountIn);
	DDX_Control(pDX, IDC_EDIT_COUNTIN, m_editCountIn);
	DDX_Control(pDX, IDC_SPIN_PCHANNEL, m_spinPChannel);
	DDX_Control(pDX, IDC_EDIT_PCHANNEL, m_editPChannel);
	DDX_Control(pDX, IDC_EDIT_ONE_VELOCITY, m_editOneVelocity);
	DDX_Control(pDX, IDC_COMBO_ONE_VALUE, m_comboOneValue);
	DDX_Control(pDX, IDC_SPIN_ONE_VELOCITY, m_spinOneVelocity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgMetronome, CDialog)
	//{{AFX_MSG_MAP(DlgMetronome)
	ON_EN_KILLFOCUS(IDC_EDIT_ONE_VELOCITY, OnKillfocusEditOneVelocity)
	ON_EN_KILLFOCUS(IDC_EDIT_PCHANNEL, OnKillfocusEditPchannel)
	ON_EN_KILLFOCUS(IDC_EDIT_OTHER_VELOCITY, OnKillfocusEditOtherVelocity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgMetronome message handlers

BOOL DlgMetronome::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::OnInitDialog();
	
	m_bOneVelocity = g_pconductor->m_bMetronomeVelocityOne;
	m_bOneValue = g_pconductor->m_bMetronomeNoteOne;
	m_bOtherVelocity = g_pconductor->m_bMetronomeVelocityOther;
	m_bOtherValue = g_pconductor->m_bMetronomeNoteOther;
	m_dwPChannel = g_pconductor->m_dwMetronomePChannel;

	if (m_editOneVelocity.GetSafeHwnd() != NULL)
	{
		m_editOneVelocity.EnableWindow(TRUE);
		m_editOneVelocity.SetLimitText( 3 );
	}
	if (m_spinOneVelocity.GetSafeHwnd() != NULL)
	{
		m_spinOneVelocity.EnableWindow(TRUE);
		m_spinOneVelocity.SetRange( 0, 127 );
		m_spinOneVelocity.SetPos( m_bOneVelocity );
	}

	if (m_editOtherVelocity.GetSafeHwnd() != NULL)
	{
		m_editOtherVelocity.EnableWindow(TRUE);
		m_editOtherVelocity.SetLimitText( 3 );
	}
	if (m_spinOtherVelocity.GetSafeHwnd() != NULL)
	{
		m_spinOtherVelocity.EnableWindow(TRUE);
		m_spinOtherVelocity.SetRange( 0, 127 );
		m_spinOtherVelocity.SetPos( m_bOtherVelocity );
	}

	if( (m_comboOneValue.GetSafeHwnd() != NULL)
	&&	(m_comboOtherValue.GetSafeHwnd() != NULL) )
	{
		m_comboOneValue.EnableWindow(TRUE);
		m_comboOtherValue.EnableWindow(TRUE);
		m_comboOneValue.ResetContent();
		m_comboOtherValue.ResetContent();

		int nIndex;
		CString cstrTxt;
		for( nIndex = 127; nIndex > 87; nIndex-- )
		{
			cstrTxt.Format( "%s %d", gacMidiValueToName[nIndex % 12] , nIndex/12 );
			m_comboOneValue.AddString( cstrTxt );
			m_comboOtherValue.AddString( cstrTxt );
		}

		for( nIndex = 87; nIndex > 26; nIndex-- )
		{
			cstrTxt.Format( "%s %d - %s", gacMidiValueToName[nIndex % 12] , nIndex/12, gacDrums[nIndex - 27] );
			m_comboOneValue.AddString( cstrTxt );
			m_comboOtherValue.AddString( cstrTxt );
		}

		for( nIndex = 26; nIndex >= 0; nIndex-- )
		{
			cstrTxt.Format( "%s %d", gacMidiValueToName[nIndex % 12] , nIndex/12 );
			m_comboOneValue.AddString( cstrTxt );
			m_comboOtherValue.AddString( cstrTxt );
		}

		m_comboOneValue.SetCurSel( 127 - m_bOneValue );
		m_comboOtherValue.SetCurSel( 127 - m_bOtherValue );
	}

	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		m_editPChannel.EnableWindow(TRUE);
		m_editPChannel.SetLimitText( 3 );
	}
	if (m_spinPChannel.GetSafeHwnd() != NULL)
	{
		m_spinPChannel.EnableWindow(TRUE);
		m_spinPChannel.SetRange( 1, 999 );
		m_spinPChannel.SetPos( m_dwPChannel + 1 );
	}

	if (m_editCountIn.GetSafeHwnd() != NULL)
	{
		m_editCountIn.EnableWindow(TRUE);
		m_editCountIn.SetLimitText( 2 );
	}
	if (m_spinCountIn.GetSafeHwnd() != NULL)
	{
		m_spinCountIn.EnableWindow(TRUE);
		m_spinCountIn.SetRange( 0, 99 );
		m_spinCountIn.SetPos( m_lCountInBars );
	}

	CheckRadioButton( IDC_RADIO_RECORD, IDC_RADIO_ALL_PLAYBACK,
					  m_fCountOnlyOnRecord ? IDC_RADIO_RECORD : IDC_RADIO_ALL_PLAYBACK );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgMetronome::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( m_comboOneValue.GetCurSel() != CB_ERR )
	{
		m_bOneValue = BYTE(127 - m_comboOneValue.GetCurSel());
	}

	if( m_comboOtherValue.GetCurSel() != CB_ERR )
	{
		m_bOtherValue = BYTE(127 - m_comboOtherValue.GetCurSel());
	}
	
	m_bOneVelocity = BYTE(m_spinOneVelocity.GetPos());
	m_bOtherVelocity = BYTE(m_spinOtherVelocity.GetPos());

	m_dwPChannel = m_spinPChannel.GetPos() - 1;

	m_lCountInBars = m_spinCountIn.GetPos();

	m_fCountOnlyOnRecord = (GetCheckedRadioButton( IDC_RADIO_RECORD, IDC_RADIO_ALL_PLAYBACK ) == IDC_RADIO_RECORD);

	CDialog::OnOK();
}

void DlgMetronome::OnKillfocusEditOneVelocity() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strNewVelocity;

	m_editOneVelocity.GetWindowText( strNewVelocity );

	// Strip leading and trailing spaces
	strNewVelocity.TrimRight();
	strNewVelocity.TrimLeft();

	if( strNewVelocity.IsEmpty() )
	{
		m_spinOneVelocity.SetPos( m_bOneVelocity );
	}
	else
	{
		int iNewVelocity = _ttoi( strNewVelocity );
		if( iNewVelocity > 127 )
		{
			iNewVelocity = 127;
			m_spinOneVelocity.SetPos( iNewVelocity );
		}
		// Only update m_bVelocity in OnOK()
		/*
		if( (BYTE)iNewVelocity != m_bVelocity )
		{
			m_bVelocity = (BYTE)iNewVelocity;
		}
		*/
	}
}

void DlgMetronome::OnKillfocusEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strNewPChannel;

	m_editPChannel.GetWindowText( strNewPChannel );

	// Strip leading and trailing spaces
	strNewPChannel.TrimRight();
	strNewPChannel.TrimLeft();

	if( strNewPChannel.IsEmpty() )
	{
		m_spinPChannel.SetPos( m_dwPChannel + 1 );
	}
	else
	{
		// Can never be greater than 999, since the user can only enter 3 digits
		// But, it can be less than 1:
		int iNewPChannel = _ttoi( strNewPChannel );
		if( iNewPChannel < 1 )
		{
			iNewPChannel = 1;
			m_spinPChannel.SetPos( iNewPChannel );
		}
		// Only update m_dwPChannel in OnOK()
		/*
		if( (iNewPChannel - 1) != m_dwPChannel )
		{
			m_dwPChannel = iNewPChannel - 1;
		}
		*/
	}
}

BOOL DlgMetronome::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && !(pMsg->lParam & 0x40000000) )
	{
		switch( (int) pMsg->wParam )
		{
		case VK_RETURN:
		{
			CWnd* pWnd = GetFocus();
			if( pWnd )
			{
				int id = pWnd->GetDlgCtrlID();
				if( (id != IDOK) && (id != IDCANCEL) )
				{
					CWnd* pWndNext = GetNextDlgTabItem( pWnd );
					if( pWndNext )
					{
						pWndNext->SetFocus();
						return TRUE;
					}
				}
			}
		}
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


void DlgMetronome::OnKillfocusEditOtherVelocity() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strNewVelocity;

	m_editOtherVelocity.GetWindowText( strNewVelocity );

	// Strip leading and trailing spaces
	strNewVelocity.TrimRight();
	strNewVelocity.TrimLeft();

	if( strNewVelocity.IsEmpty() )
	{
		m_spinOtherVelocity.SetPos( m_bOtherVelocity );
	}
	else
	{
		int iNewVelocity = _ttoi( strNewVelocity );
		if( iNewVelocity > 127 )
		{
			iNewVelocity = 127;
			m_spinOtherVelocity.SetPos( iNewVelocity );
		}
		// Only update m_bVelocity in OnOK()
		/*
		if( (BYTE)iNewVelocity != m_bVelocity )
		{
			m_bVelocity = (BYTE)iNewVelocity;
		}
		*/
	}
}
