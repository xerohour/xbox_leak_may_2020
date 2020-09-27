// ScriptsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "ScriptsDlg.h"
#include "OtherFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptsDlg dialog


CScriptsDlg::CScriptsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CScriptsDlg::~CScriptsDlg()
{
    while( !m_lstOtherFiles.IsEmpty() )
    {
        delete m_lstOtherFiles.RemoveHead();
    }
}


void CScriptsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptsDlg)
	DDX_Control(pDX, IDC_STATIC_SCRIPTS, m_staticScripts);
	DDX_Control(pDX, IDC_LIST_SCRIPTS, m_listScripts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptsDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptsDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptsDlg message handlers

void CScriptsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow( m_listScripts.GetSafeHwnd() ) )
	{
		return;
	}

	RECT rectWindow;
	GetClientRect( &rectWindow );

    RECT rect;
    m_staticScripts.GetClientRect( &rect );
    rect.right = rectWindow.right;
    rect.bottom += 2 * ::GetSystemMetrics(SM_CYBORDER);
    m_staticScripts.MoveWindow( &rect );

    rect.top = rect.bottom;
    rect.bottom = rectWindow.bottom;
    m_listScripts.MoveWindow( &rect );
}

BOOL CScriptsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    POSITION pos = m_lstOtherFiles.GetHeadPosition();
    while( pos )
    {
        COtherFile *pOtherFile = m_lstOtherFiles.GetNext( pos );
        int nIndex = m_listScripts.InsertString( 0, TEXT("") );
        m_listScripts.SetItemDataPtr( nIndex, pOtherFile );
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptsDlg::OnConnectionStateChanged( void )
{
	m_listScripts.EnableWindow( false );
	m_staticScripts.EnableWindow( false );
	/*
	m_listScripts.EnableWindow( theApp.m_fConnected );
	m_staticScripts.EnableWindow( theApp.m_fConnected );
	*/
}

bool CScriptsDlg::IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode )
{
	UNREFERENCED_PARAMETER(pIDMUSProdNode);
	return false;
	/* TODO:
	POSITION pos = m_lstSegments.GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = m_lstSegments.GetNext( pos );
		if( pIDMUSProdNode == pSegment->m_pFileNode )
		{
			return true;
		}
	}
	return false;
	*/
}

/* TODO:
void CScriptsDlg::AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode )
{
	// Add it to our internal list
	AddSegmentToList( new CSegment(pIDMUSProdNode) );
}
*/
