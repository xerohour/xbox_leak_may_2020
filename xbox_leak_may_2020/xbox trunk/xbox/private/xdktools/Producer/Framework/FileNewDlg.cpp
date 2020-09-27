// FileNewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "FileNewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg dialog

CFileNewDlg::CFileNewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileNewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileNewDlg)
	//}}AFX_DATA_INIT

	m_nNbrSelItems = 0;
	m_apTemplate = NULL;
}

CFileNewDlg::~CFileNewDlg()
{
	if( m_apTemplate )
	{
		delete [] m_apTemplate;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg::FillTypeListBox

void CFileNewDlg::FillTypeListBox( void ) 
{
	CString strText;
	int nPos;

	// Empty list box
	m_lstbxTypes.ResetContent();

	// Add DocTypes
	if( theApp.m_pDocManager )
	{
		POSITION pos = theApp.m_pDocManager->GetFirstDocTemplatePosition();
		while( pos )
		{
			CDocTemplate* pTemplate = theApp.m_pDocManager->GetNextDocTemplate( pos );

			// Get text for listbox
			pTemplate->GetDocString( strText, CDocTemplate::fileNewName );

			// Add string to listbox
			if( strText.IsEmpty() == FALSE )
			{
				nPos = m_lstbxTypes.AddString( strText );
				m_lstbxTypes.SetItemDataPtr( nPos, pTemplate );
			}
		}
	}
}


void CFileNewDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileNewDlg)
	DDX_Control(pDX, IDC_CHECK_USE_DEFAULT_NAMES, m_checkUseDefaultNames);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_LSTBX_TYPES, m_lstbxTypes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileNewDlg, CDialog)
	//{{AFX_MSG_MAP(CFileNewDlg)
	ON_BN_CLICKED(IDC_CHECK_USE_DEFAULT_NAMES, OnCheckUseDefaultNames)
	ON_BN_DOUBLECLICKED(IDC_CHECK_USE_DEFAULT_NAMES, OnDoubleClickedCheckUseDefaultNames)
	ON_LBN_DBLCLK(IDC_LSTBX_TYPES, OnDblClkLstbxTypes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg::OnInitDialog

BOOL CFileNewDlg::OnInitDialog( void ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CDialog::OnInitDialog();
	
	// "File Types" list box
	FillTypeListBox();
	m_lstbxTypes.SetSel( 0 );
	m_lstbxTypes.SetFocus();

	// "Name" edit control
	m_editName.EnableWindow( FALSE );
	m_checkUseDefaultNames.SetCheck( 1 );
	
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg::OnCheckUseDefaultNames

void CFileNewDlg::OnCheckUseDefaultNames( void ) 
{
	if( m_checkUseDefaultNames.GetCheck() )
	{
		m_editName.GetWindowText( m_strTempName );
		m_editName.EnableWindow( FALSE );
		m_editName.SetWindowText( NULL );
	}
	else
	{
		m_editName.SetWindowText( m_strTempName );
		m_editName.EnableWindow( TRUE );
		m_editName.SetFocus();
		m_editName.SetSel( 0, -1 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg::OnDoubleClickedCheckUseDefaultNames

void CFileNewDlg::OnDoubleClickedCheckUseDefaultNames( void ) 
{
	OnCheckUseDefaultNames() ;
}


/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg::OnDblClkLstbxTypes

void CFileNewDlg::OnDblClkLstbxTypes( void ) 
{
	OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg::OnOK

void CFileNewDlg::OnOK( void ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	m_strDefaultName.Empty();

	if( m_checkUseDefaultNames.GetCheck() == 0 )
	{
		m_editName.GetWindowText( m_strTempName );

		if( m_strTempName.IsEmpty() )
		{
			AfxMessageBox( IDS_ERR_NAME_EMPTY );
			return;
		}

		int iBad = m_strTempName.FindOneOf( _T("\\/:*?\"<>;|%") );
		if( iBad != -1 )
		{
			AfxMessageBox( IDS_ERR_FILE_CHAR );
			return;
		}

		m_strDefaultName = m_strTempName;
	}

	m_nNbrSelItems = m_lstbxTypes.GetSelCount();
	if( m_nNbrSelItems > 0 )
	{
		int* pnSelItems = new int[m_nNbrSelItems];
		if( pnSelItems )
		{
			m_apTemplate = new CDocTemplate*[m_nNbrSelItems];
			if( m_apTemplate )
			{
				m_lstbxTypes.GetSelItems( m_nNbrSelItems, pnSelItems );

				for( int i = 0;  i < m_nNbrSelItems ;  i++ )
				{
					m_apTemplate[i] = (CDocTemplate *)m_lstbxTypes.GetItemDataPtr( pnSelItems[i] );
				}
			}

			delete [] pnSelItems;
		}
	}
	
	CDialog::OnOK();
}
