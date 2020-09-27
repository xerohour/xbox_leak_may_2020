// DlgEditPChannel.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEditPChannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgEditPChannel dialog


DlgEditPChannel::DlgEditPChannel(CWnd* pParent /*=NULL*/)
	: CDialog(DlgEditPChannel::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgEditPChannel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void DlgEditPChannel::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditPChannel)
	DDX_Control(pDX, IDC_STATIC_PCHANNEL, m_staticPChannelHelp);
	DDX_Control(pDX, IDC_EDIT_PCHANNEL_NAME, m_editPChannelname);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgEditPChannel, CDialog)
	//{{AFX_MSG_MAP(DlgEditPChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgEditPChannel message handlers

BOOL DlgEditPChannel::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog::OnInitDialog();

	CString strFormat;
	if( strFormat.LoadString( IDS_PCHANNEL_HELP ) )
	{
		CString strText;
		strText.Format( strFormat, m_dwPChannel + 1 );
		m_staticPChannelHelp.SetWindowText( strText );
	}

	m_editPChannelname.SetWindowText( m_strName );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgEditPChannel::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strName;
	m_editPChannelname.GetWindowText( strName );
	if( !strName.IsEmpty() )
	{
		m_strName = strName;
	}

	CDialog::OnOK();
}
