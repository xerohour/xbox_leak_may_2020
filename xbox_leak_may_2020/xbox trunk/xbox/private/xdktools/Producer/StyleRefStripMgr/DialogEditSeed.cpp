// DialogEditSeed.cpp : implementation file
//

#include "stdafx.h"
#include "DialogEditSeed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogEditSeed dialog


CDialogEditSeed::CDialogEditSeed(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogEditSeed::IDD, pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//{{AFX_DATA_INIT(CDialogEditSeed)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dwVariationSeed = 1;
}


void CDialogEditSeed::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEditSeed)
	DDX_Control(pDX, IDC_EDIT_VARIATION_SEED, m_editSeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogEditSeed, CDialog)
	//{{AFX_MSG_MAP(CDialogEditSeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEditSeed message handlers

void CDialogEditSeed::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	TCHAR tcstrText[20];
	ZeroMemory( tcstrText, sizeof(TCHAR) * 20 );
	m_editSeed.GetWindowText( tcstrText, 19 );
	if( _tcslen( tcstrText ) == 0 )
	{
		m_dwVariationSeed = 0;
	}
	else
	{
		_stscanf( tcstrText, "%u", &m_dwVariationSeed );
	}

	if( m_dwVariationSeed == 0 )
	{
		CString strText;
		if( strText.LoadString( IDS_ERR_SEED_ZERO ) )
		{
			MessageBox( strText, NULL, MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
			return;
		}
	}

	CDialog::OnOK();
}

BOOL CDialogEditSeed::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::OnInitDialog();
	
	TCHAR tcstrText[20];
	_stprintf( tcstrText, "%u", m_dwVariationSeed );
	m_editSeed.SetWindowText( tcstrText );

	_stprintf( tcstrText, "%u", UINT_MAX );
	m_editSeed.SetLimitText( _tcslen( tcstrText ) );
	m_editSeed.SetFocus();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
