// RightPaneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xboxaddin.h"
#include "RightPaneDlg.h"
#include "OtherFilesDlg.h"
//#include "ScriptsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRightPaneDlg dialog


CRightPaneDlg::CRightPaneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRightPaneDlg::IDD, pParent) //: CEndTrack(CRightPaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRightPaneDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    //m_wSplitterPos = 0;
    m_pOtherFilesDlg = NULL;
    //m_pScriptsDlg = NULL;
}

CRightPaneDlg::~CRightPaneDlg()
{
	if( m_pOtherFilesDlg )
	{
		delete m_pOtherFilesDlg;
		m_pOtherFilesDlg = NULL;
	}
	/*
	if( m_pScriptsDlg )
	{
		delete m_pScriptsDlg;
		m_pScriptsDlg = NULL;
	}
	*/
}


void CRightPaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRightPaneDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRightPaneDlg, CDialog)
	//{{AFX_MSG_MAP(CRightPaneDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRightPaneDlg message handlers

BOOL CRightPaneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // Resize the panes
    /*
    RECT rectClient;
    GetClientRect( &rectClient );
    OnSize( SIZE_RESTORED, rectClient.right, rectClient.bottom );
    */

    UpdateListBoxPositions();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CRightPaneDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	/*
	// Create the Splitter and position it in the Editor
	// (temporary values added until coords are saved to design-time file)
	RECT rect;
	rect.left = lpCreateStruct->x;
    rect.right = lpCreateStruct->cx;
    rect.top = lpCreateStruct->y;
	rect.bottom = rect.top + DEFAULTSPLITTERHEIGHT;

	m_wndHSplitter.Create( this, &rect );
	*/

	// Create the OtherFiles dialog and position it in the First Splitter Pane
	m_pOtherFilesDlg = new COtherFilesDlg;
	if( m_pOtherFilesDlg == NULL )
	{
		return -1;
	}

	// Point the Secondary segment dialog back to us and at the AudioPath object
	m_pOtherFilesDlg->Create( IDD_OTHER_FILES, this );
	m_pOtherFilesDlg->ShowWindow( SW_SHOW );
	//m_wndHSplitter.SetFirstPane( m_pOtherFilesDlg );

	// Create the scripts dialog and position it in the Second Splitter Pane
	/*
	m_pScriptsDlg = new CScriptsDlg;
	if( m_pScriptsDlg == NULL )
	{
		return -1;
	}

	// Point the EffectList dialog back to us and at the AudioPath object
	m_pScriptsDlg->Create( IDD_SCRIPTS, this );
	m_pScriptsDlg->ShowWindow( SW_SHOW );
	m_wndHSplitter.SetSecondPane( m_pScriptsDlg );
	*/
	
	return 0;
}

void CRightPaneDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    UpdateListBoxPositions();
}

void CRightPaneDlg::UpdateListBoxPositions( void )
{
    if( GetSafeHwnd() == NULL )
    {
        return;
    }

    RECT rectClient;
    GetClientRect( &rectClient );

	/*
    // get splitter position
	WORD pos = m_wSplitterPos;

	// calculate where splitter should be
	if(pos == 0)
	{
		// put it right in the middle
		pos = WORD(min( USHRT_MAX, (rectClient.bottom - DEFAULTSPLITTERHEIGHT)/2 ));

		// save it
		m_wSplitterPos = pos;
	}

    // Update splitter position, if it's non-zero
	if( pos != 0 )
	{
		RECT rect = rectClient;
		rect.bottom = max( DEFAULTSPLITTERHEIGHT, pos );
		rect.top = rect.bottom - DEFAULTSPLITTERHEIGHT;
		m_wndHSplitter.SetTracker(rect);
		m_wndHSplitter.MoveWindow( &rect, TRUE );
		m_pOtherFilesDlg->MoveWindow(0, 0, rect.right, rect.top);
		m_pScriptsDlg->MoveWindow(0, rect.bottom, rect.right, rectClient.bottom - rect.bottom);
	}
	*/

	// Make the OtherFiles window fill the dialog
	m_pOtherFilesDlg->MoveWindow(0, 0, rectClient.right, rectClient.bottom);
}

/*
void CRightPaneDlg::EndTrack( long lNewPos )
{
    m_wSplitterPos = (WORD)lNewPos;
}
*/

void CRightPaneDlg::OnConnectionStateChanged( void )
{
	m_pOtherFilesDlg->OnConnectionStateChanged();
	//m_pScriptsDlg->OnConnectionStateChanged();
}

bool CRightPaneDlg::IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode )
{
	return m_pOtherFilesDlg->IsNodeDisplayed( pIDMUSProdNode );
	//|| m_pScriptsDlg->IsNodeDisplayed( pIDMUSProdNode );
}

HRESULT CRightPaneDlg::AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode )
{
	// TODO: Add scrips to the scripts dialog
	return m_pOtherFilesDlg->AddNodeToDisplay( pIDMUSProdNode );
}

void CRightPaneDlg::CleanUpDisplay( void )
{
	m_pOtherFilesDlg->CleanUpDisplay();
	// No need to clean up scripts dialog
}

bool CRightPaneDlg::IsFileInUse( CFileItem *pFileItem )
{
	return m_pOtherFilesDlg->IsFileInUse( pFileItem );
	//|| m_pScriptsDlg->IsFileInUse( pFileItem );
}

void CRightPaneDlg::DeleteAll( void )
{
	m_pOtherFilesDlg->DeleteAll();
	//m_pScriptsDlg->DeleteAll();
}

/*
void CRightPaneDlg::ReCopyAll( void )
{
	// Only copy scripts
	m_pScriptsDlg->ReCopyAll();
}
*/
