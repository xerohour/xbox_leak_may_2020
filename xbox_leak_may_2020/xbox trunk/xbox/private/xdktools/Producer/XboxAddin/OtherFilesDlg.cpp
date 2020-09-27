// OtherFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "OtherFilesDlg.h"
#include "OtherFile.h"
#include "XboxAddinComponent.h"
#include "XboxAddinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherFilesDlg dialog


COtherFilesDlg::COtherFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COtherFilesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COtherFilesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_plstOtherFiles = &theApp.m_pXboxAddinComponent->m_lstOtherFiles;
}

COtherFilesDlg::~COtherFilesDlg()
{
}

void COtherFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherFilesDlg)
	DDX_Control(pDX, IDC_STATIC_OTHER_FILES, m_staticOtherFiles);
	DDX_Control(pDX, IDC_LIST_OTHER_FILES, m_listFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtherFilesDlg, CDialog)
	//{{AFX_MSG_MAP(COtherFilesDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherFilesDlg message handlers

void COtherFilesDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow( m_listFiles.GetSafeHwnd() ) )
	{
		return;
	}

	RECT rectWindow;
	GetClientRect( &rectWindow );

    RECT rect;
    m_staticOtherFiles.GetClientRect( &rect );
    rect.right = rectWindow.right;
    rect.bottom += 2 * ::GetSystemMetrics(SM_CYBORDER);
    m_staticOtherFiles.MoveWindow( &rect );

    rect.top = rect.bottom;
    rect.bottom = rectWindow.bottom;
    m_listFiles.MoveWindow( &rect );
	m_listFiles.Invalidate( TRUE );
}

BOOL COtherFilesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT( m_plstOtherFiles );
    POSITION pos = m_plstOtherFiles->GetHeadPosition();
    while( pos )
    {
        COtherFile *pOtherFile = m_plstOtherFiles->GetNext( pos );
        m_listFiles.AddString( (LPCTSTR) pOtherFile );
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COtherFilesDlg::OnConnectionStateChanged( void )
{
	m_listFiles.EnableWindow( theApp.m_fConnected );
	m_staticOtherFiles.EnableWindow( theApp.m_fConnected );
}

bool COtherFilesDlg::IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode )
{
	ASSERT( m_plstOtherFiles );
	POSITION pos = m_plstOtherFiles->GetHeadPosition();
	while( pos )
	{
		COtherFile *pOtherFile = m_plstOtherFiles->GetNext( pos );
		if( pOtherFile->ContainsNode( pIDMUSProdNode ) )
		{
			return true;
		}
	}
	return false;
}

HRESULT COtherFilesDlg::AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode )
{
	// Add it to our internal list
	return AddFileToList( new COtherFile(pIDMUSProdNode) );
}

HRESULT COtherFilesDlg::AddFileToList( COtherFile *pOtherFile )
{
	if( pOtherFile == NULL )
	{
		return E_POINTER;
	}

	ASSERT( m_plstOtherFiles );

	// Add to our list
	m_plstOtherFiles->AddHead( pOtherFile );

	// Copy to Xbox
	HRESULT hr = pOtherFile->CopyToXbox( );

	if( SUCCEEDED(hr) )
	{
		// Add it to the display
		if( m_listFiles.GetSafeHwnd() )
		{
			m_listFiles.AddString( (LPCTSTR) pOtherFile );
		}
	}
	else
	{
		// Remove it from our list
		POSITION pos = m_plstOtherFiles->Find( pOtherFile );
		if( pos )
		{
			m_plstOtherFiles->RemoveAt( pos );
		}

		// Delete it
		delete pOtherFile;

		// Now, clean up the display
		theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->CleanUpDisplay();
	}

	return hr;
}

void COtherFilesDlg::CleanUpDisplay( void )
{
	bool fChanged = true;

	ASSERT( m_plstOtherFiles );

	while( fChanged )
	{
		fChanged = false;

		POSITION pos = m_plstOtherFiles->GetHeadPosition();
		while( pos )
		{
			CFileItem *pFileItem= m_plstOtherFiles->GetNext( pos );

			if( !theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->IsFileInUse( pFileItem ) )
			{
				if( RemoveFile( pFileItem ) )
				{
					fChanged = true;
				}
			}
		}
	}
}

bool COtherFilesDlg::RemoveFile( CFileItem *pFileItem )
{
	// Remove it from the Xbox
	if( FAILED( pFileItem->RemoveFromXbox() ) )
	{
		CString strErrorText;
		strErrorText.FormatMessage( IDS_ERR_REMOVE, pFileItem->GetName() );
		if( IDRETRY == XboxAddinMessageBox( this, strErrorText, MB_ICONERROR | MB_RETRYCANCEL ) )
		{
			return true;
		}
		return false;
	}

	// Remove it from the display
	int nIndex = m_listFiles.IndexFromFile( pFileItem );
	if( nIndex != LB_ERR )
	{
		m_listFiles.DeleteString( nIndex );
	}

	// Remove it from the list
	ASSERT( m_plstOtherFiles );
	POSITION posFile = m_plstOtherFiles->Find( pFileItem );
	if( posFile )
	{
		m_plstOtherFiles->RemoveAt( posFile );
	}

	// Finally, delete it
	delete pFileItem;

	// Don't do this, since we can only be called from CleanUpDisplay
	// Now, clean up the display
	//theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->CleanUpDisplay();

	return true;
}

bool COtherFilesDlg::IsFileInUse( CFileItem *pFileItem )
{
	ASSERT( m_plstOtherFiles );
	POSITION pos = m_plstOtherFiles->GetHeadPosition();
	while( pos )
	{
		CFileItem *pTemp = m_plstOtherFiles->GetNext( pos );
		if( pTemp != pFileItem )
		{
			if( pTemp->UsesFile( pFileItem ) )
			{
				return true;
			}
		}
	}

	return false;
}

void COtherFilesDlg::DeleteAll( void )
{
	// This should be empty
	ASSERT( m_plstOtherFiles );
	ASSERT( m_plstOtherFiles->IsEmpty() );

	// But in case it's not...
	POSITION pos = m_plstOtherFiles->GetHeadPosition();
	while( pos )
	{
		CFileItem *pTemp = m_plstOtherFiles->GetNext( pos );
		pTemp->RemoveFromXbox();
	}
}
