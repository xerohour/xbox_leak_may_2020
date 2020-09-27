// DlgSendProperties.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSendProperties.h"
#include "DSound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSendProperties dialog


CDlgSendProperties::CDlgSendProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSendProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSendProperties)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_lVolume = 0;
}


void CDlgSendProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSendProperties)
	DDX_Control(pDX, IDC_EDIT_VOLUME, m_editVolume);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSendProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgSendProperties)
	ON_EN_KILLFOCUS(IDC_EDIT_VOLUME, OnKillfocusEditVolume)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSendProperties message handlers

void CDlgSendProperties::OnKillfocusEditVolume() 
{
	BOOL bTransSuccessful = FALSE;
	long lNewValue = GetDlgItemInt( IDC_EDIT_VOLUME, &bTransSuccessful, TRUE );
	if( !bTransSuccessful )
	{
		SetDlgItemInt( IDC_EDIT_VOLUME, m_lVolume, TRUE );
		return;
	}
	else
	{
		if( lNewValue < DSBVOLUME_MIN )
		{
			lNewValue = DSBVOLUME_MIN;
		}
		else if( lNewValue > DSBVOLUME_MAX )
		{
			lNewValue = DSBVOLUME_MAX;
		}
		m_lVolume = lNewValue;
		SetDlgItemInt( IDC_EDIT_VOLUME, lNewValue, TRUE );
	}
}

void CDlgSendProperties::OnOK() 
{
	OnKillfocusEditVolume();

	CDialog::OnOK();
}

BOOL CDlgSendProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Initialize the title
	SetWindowText( m_strTitle );

	// Initialize tab control
	CString strText;
	strText.LoadString( IDS_SEND_TEXT );
	m_tab.InsertItem( 0, strText );

	// Set volume
	m_editVolume.m_fDotValid = false;
	SetDlgItemInt( IDC_EDIT_VOLUME, m_lVolume, TRUE );
	strText.Format( _T("%d"), DSBVOLUME_MIN );
	m_editVolume.SetLimitText( strText.GetLength() );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
