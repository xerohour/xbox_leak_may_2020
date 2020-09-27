// DeleteFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "DeleteFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteFileDlg dialog


CDeleteFileDlg::CDeleteFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteFileDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDeleteFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteFileDlg)
	DDX_Control(pDX, IDC_PROMPT, m_staticPrompt);
	DDX_Control(pDX, IDC_LISTBOX_FILES, m_listFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteFileDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteFileDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteFileDlg message handlers

BOOL CDeleteFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT( m_pFileNode != NULL );
	
	CString strText;

	AfxFormatString1( strText, IDS_PROMPT_DELETE_FILE, m_pFileNode->m_strName );
	m_staticPrompt.SetWindowText( strText );

	CJzNotifyNode* pJzNotifyNode;
	CFileNode* pFileNode;
	CDC* pDC;
	CSize sizeText;
	int nCurExtent;

	// Add files to listbox
	POSITION pos = m_pFileNode->m_lstNotifyNodes.GetHeadPosition();
	while( pos )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( m_pFileNode->m_lstNotifyNodes.GetNext(pos) );
		
		pFileNode = theApp.GetFileByGUID( pJzNotifyNode->m_guidFile );
		if( pFileNode )
		{
			pFileNode->ConstructFileName( strText );
			m_listFiles.AddString( strText );
			pFileNode->Release();
			pFileNode = NULL;

			// Set horizontal extent
			nCurExtent = m_listFiles.GetHorizontalExtent();

			pDC = m_listFiles.GetDC();
			if( pDC )
			{
				sizeText = pDC->GetTextExtent( strText );

				if( sizeText.cx > nCurExtent )
				{
					m_listFiles.SetHorizontalExtent( sizeText.cx );
				}

				m_listFiles.ReleaseDC( pDC );
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
