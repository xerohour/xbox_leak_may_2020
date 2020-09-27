// GuidDlg.cpp : implementation file
//

#include "stdafx.h"
#include "guiddlg.h"
#include "dmusici.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuidDlg dialog


CGuidDlg::CGuidDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGuidDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGuidDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGuidDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGuidDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_EDIT_GUID, m_editGuid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGuidDlg, CDialog)
	//{{AFX_MSG_MAP(CGuidDlg)
	ON_BN_CLICKED(IDC_NEW_GUID, OnNewGuid)
	ON_EN_KILLFOCUS(IDC_EDIT_GUID, OnKillfocusEditGuid)
	ON_EN_UPDATE(IDC_EDIT_GUID, OnUpdateEditGuid)
	ON_BN_CLICKED(IDC_GM_GUID, OnGmGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGuidDlg message handlers

BOOL CGuidDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LPOLESTR psz;
	if( SUCCEEDED( ::StringFromIID(m_guid, &psz) ) )
    {
		TCHAR szGuid[100];
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		m_editGuid.SetWindowText( szGuid );
	}
	
	memcpy( &m_guidOriginal, &m_guid, sizeof( GUID ) );

	m_btnOK.EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGuidDlg::OnNewGuid() 
{
	if( SUCCEEDED( ::CoCreateGuid( &m_guid ) ) )
	{
		LPOLESTR psz;
		if( SUCCEEDED( ::StringFromIID(m_guid, &psz) ) )
		{
			TCHAR szGuid[100];
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
			CoTaskMemFree( psz );

			m_editGuid.SetWindowText( szGuid );

			m_btnOK.EnableWindow( TRUE );
		}
	}
}

void CGuidDlg::OnKillfocusEditGuid() 
{
}

void CGuidDlg::OnCancel() 
{
	memcpy( &m_guid, &m_guidOriginal, sizeof( GUID ) );
	
	CDialog::OnCancel();
}

void CGuidDlg::OnOK() 
{
	CString strText;
	m_editGuid.GetWindowText( strText );

	WCHAR szWChar[100];
	ZeroMemory( szWChar, 100 * sizeof( WCHAR ) );
	::MultiByteToWideChar( CP_ACP, 0, strText, -1, szWChar, 100 );
	if( SUCCEEDED( ::IIDFromString( szWChar, &m_guid ) ) )
	{
		CDialog::OnOK();
	}
#ifdef _DEBUG
	else
	{
		TRACE("CGuidDlg::OnOK: Edit box doesn't contain a valid GUID.\n");
		ASSERT( FALSE );
	}
#endif
}

void CGuidDlg::OnUpdateEditGuid() 
{
	CString strText;
	m_editGuid.GetWindowText( strText );

	GUID guid;

	WCHAR szWChar[100];
	HRESULT hr;
	hr = ::MultiByteToWideChar( CP_ACP, 0, strText, -1, szWChar, 100 );
	hr = ::IIDFromString( szWChar, &guid );
	if( SUCCEEDED( hr ) )
	{
		if( memcmp( &guid, &m_guidOriginal, sizeof(GUID) ) != 0 )
		{
			m_btnOK.EnableWindow( TRUE );
		}
		else
		{
			m_btnOK.EnableWindow( FALSE );
		}
	}
	else
	{
		m_btnOK.EnableWindow( FALSE );
	}
}

void CGuidDlg::OnGmGuid() 
{
	LPOLESTR psz;
	if( SUCCEEDED( ::StringFromIID(GUID_DefaultGMCollection, &psz) ) )
	{
		TCHAR szGuid[100];
		WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
		CoTaskMemFree( psz );

		m_editGuid.SetWindowText( szGuid );

		m_btnOK.EnableWindow( TRUE );
	}
}
