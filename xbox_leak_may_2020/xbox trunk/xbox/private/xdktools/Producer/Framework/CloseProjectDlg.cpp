// CloseProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "CloseProjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCloseProjectDlg dialog


CCloseProjectDlg::CCloseProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCloseProjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCloseProjectDlg)
	//}}AFX_DATA_INIT

	m_pProject = NULL;
}


void CCloseProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCloseProjectDlg)
	DDX_Control(pDX, IDC_PROMPT2, m_staticPrompt2);
	DDX_Control(pDX, IDC_PROMPT1, m_staticPrompt1);
	DDX_Control(pDX, IDC_LISTBOX_FILES, m_lstbxFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCloseProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CCloseProjectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCloseProjectDlg message handlers

BOOL CCloseProjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT( m_pProject != NULL );	

	if( m_pProject )
	{
		// Set prompts
		CString strText;
		AfxFormatString1( strText, IDS_PROMPT1_CLOSE_PROJECT, m_pProject->m_strName );
		m_staticPrompt1.SetWindowText( strText );
		AfxFormatString1( strText, IDS_PROMPT2_CLOSE_PROJECT, m_pProject->m_strName );
		m_staticPrompt2.SetWindowText( strText );

		// Fill file listbox
		CFileNode* pFileNode;
		CFileNode* pTheFileNode;
		CJzNotifyNode* pJzNotifyNode;
		POSITION posNotifyList;
		CDC* pDC;
		CSize sizeText;
		int nCurExtent;
		BOOL fDisplayFile;

		POSITION pos = m_pProject->m_lstFiles.GetHeadPosition();
		while( pos )
		{
			pFileNode = m_pProject->m_lstFiles.GetNext( pos );

			fDisplayFile = FALSE;

			posNotifyList = pFileNode->m_lstNotifyNodes.GetHeadPosition();
			while( posNotifyList )
			{
				pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(posNotifyList) );
				
				pTheFileNode = theApp.GetFileByGUID( pJzNotifyNode->m_guidFile );
				if( pTheFileNode )
				{
					if( pTheFileNode->m_pProject != m_pProject )
					{
						fDisplayFile = TRUE;
					}

					pTheFileNode->Release();

					if( fDisplayFile )
					{
						pFileNode->ConstructFileName( strText );
						m_lstbxFiles.AddString( strText );

						// Set horizontal extent
						nCurExtent = m_lstbxFiles.GetHorizontalExtent();
						pDC = m_lstbxFiles.GetDC();
						if( pDC )
						{
							sizeText = pDC->GetTextExtent( strText );

							if( sizeText.cx > nCurExtent )
							{
								m_lstbxFiles.SetHorizontalExtent( sizeText.cx );
							}
							m_lstbxFiles.ReleaseDC( pDC );
						}
						break;
					}
				}
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
