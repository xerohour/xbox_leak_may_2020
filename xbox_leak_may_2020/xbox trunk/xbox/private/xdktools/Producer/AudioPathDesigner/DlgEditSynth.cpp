// DlgEditSynth.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEditSynth.h"
#include "dmusicc.h"
#include "ItemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditSynth dialog


CDlgEditSynth::CDlgEditSynth(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditSynth::IDD, pParent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CDlgEditSynth)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dwEffects = 0;
	m_dwSupportedEffects = 0xFFFFFFFF;
	m_dwSampleRate = 22050;
	m_dwVoices = DEFAULT_NUM_VOICES;
	m_dwMaxVoices = DEFAULT_NUM_VOICES;
}


void CDlgEditSynth::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditSynth)
	DDX_Control(pDX, IDC_SPIN_VOICES, m_spinVoices);
	DDX_Control(pDX, IDC_EDIT_VOICES, m_editVoices);
	DDX_Control(pDX, IDC_EDIT_SYNTH_NAME, m_editSynthName);
	DDX_Control(pDX, IDC_CHECK_REVERB, m_checkReverb);
	DDX_Control(pDX, IDC_CHECK_CHORUS, m_checkChorus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditSynth, CDialog)
	//{{AFX_MSG_MAP(CDlgEditSynth)
	ON_EN_KILLFOCUS(IDC_EDIT_VOICES, OnKillfocusEditVoices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditSynth message handlers

void CDlgEditSynth::OnKillfocusEditVoices() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fSucceeded;
	int nNumVoices = GetDlgItemInt( IDC_EDIT_VOICES, &fSucceeded, FALSE );
	if( !fSucceeded
	||	nNumVoices < 1 )
	{
		// Set a minimum of 1
		nNumVoices = 1;

		// Reset the number of channels
		SetDlgItemInt( IDC_EDIT_VOICES, nNumVoices, FALSE );
		m_spinVoices.SetPos( nNumVoices );
	}
	else if( (unsigned) nNumVoices > m_dwMaxVoices )
	{
		// Set a maximum of m_dwMaxVoices
		nNumVoices = m_dwMaxVoices;

		// Reset the number of channels
		SetDlgItemInt( IDC_EDIT_VOICES, nNumVoices, FALSE );
		m_spinVoices.SetPos( nNumVoices );
	}
}

void CDlgEditSynth::OnOK() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_dwVoices = m_spinVoices.GetPos();

	m_dwEffects = m_checkReverb.GetCheck() ? DMUS_EFFECT_REVERB : 0;
	m_dwEffects |= m_checkChorus.GetCheck() ? DMUS_EFFECT_CHORUS : 0;

	switch( GetCheckedRadioButton( IDC_11, IDC_48 ) )
	{
	case IDC_11:
		m_dwSampleRate = 11025;
		break;
	case IDC_44:
		m_dwSampleRate = 44100;
		break;
	case IDC_48:
		m_dwSampleRate = 48000;
		break;
	case IDC_22:
	default:
		m_dwSampleRate = 22050;
		break;
	}

	CDialog::OnOK();
}

BOOL CDlgEditSynth::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	if( m_dwMaxVoices == 0xFFFFFFFF )
	{
		m_spinVoices.EnableWindow( FALSE );
		m_editVoices.EnableWindow( FALSE );
		m_editVoices.SetWindowText( _T("---") );
	}
	else
	{
		TCHAR tcstrTmp[12];
		_itot( m_dwMaxVoices, tcstrTmp, 10 );
		m_editVoices.SetLimitText( _tcslen( tcstrTmp ) );
		m_spinVoices.SetRange( 1, m_dwMaxVoices );
		m_spinVoices.SetPos( m_dwVoices );
	}

	// Ensure that only supported effects are checked
	m_dwEffects &= m_dwSupportedEffects;

	m_checkReverb.EnableWindow( (m_dwSupportedEffects & DMUS_EFFECT_REVERB) ? TRUE : FALSE );
	m_checkReverb.SetCheck( (m_dwEffects & DMUS_EFFECT_REVERB) ? 1 : 0 );
	m_checkChorus.EnableWindow( (m_dwSupportedEffects & DMUS_EFFECT_CHORUS) ? TRUE : FALSE );
	m_checkChorus.SetCheck( (m_dwEffects & DMUS_EFFECT_CHORUS) ? 1 : 0 );

	switch( m_dwSampleRate )
	{
	case 11025:
		CheckRadioButton( IDC_11, IDC_48, IDC_11 );
		break;
	case 44100:
		CheckRadioButton( IDC_11, IDC_48, IDC_44 );
		break;
	case 48000:
		CheckRadioButton( IDC_11, IDC_48, IDC_48 );
		break;
	case 22050:
	default:
		CheckRadioButton( IDC_11, IDC_48, IDC_22 );
		break;
	}
	
	m_editSynthName.SetWindowText( m_strName );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
