// RuntimeDupeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "RuntimeDupeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDupeDlg dialog


CRuntimeDupeDlg::CRuntimeDupeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRuntimeDupeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRuntimeDupeDlg)
	//}}AFX_DATA_INIT

	m_pFileNode = NULL;
	m_pProject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CRuntimeDupeDlg::AddToList

void CRuntimeDupeDlg::AddToList( CFileNode* pFileNode ) 
{
	ASSERT( pFileNode != NULL );

	CString strText;
	CString strRelativeFileName;

	// Get relative filename for "Design" file
	if( pFileNode->ConstructRelativePath( strRelativeFileName ) )
	{
		if( !strRelativeFileName.IsEmpty() )
		{
			strRelativeFileName += _T("\\");
		}
		strRelativeFileName += pFileNode->m_strName;
	}

	CString strFileName;
	CString strFileModified;
	CString strFileSize;

	pFileNode->ConstructFileName( strFileName );
	HANDLE hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 							 FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		FILETIME ftModified;
		DWORD dwFileSize;

		if( ::GetFileTime(hFile, NULL, NULL, &ftModified) )
		{
			CTime timeFile;
			
			timeFile = ftModified;
			strFileModified = timeFile.Format( "%A, %B %d, %Y  %I:%M:%S %p" );
		}

		dwFileSize = GetFileSize( hFile, NULL );

		CString strSize;
		CString strBytes;
		strSize.Format( "%u", dwFileSize );

		int i, j;
		int nLength = strSize.GetLength();

		for( i=0, j=nLength;  i < nLength ; i++ )
		{
			strBytes += strSize[i];
			j--;
			if( (j > 0)
			&& !(j % 3) )
			{
				strBytes += ',';
			}
		}
		
		if( dwFileSize < 1024 )
		{
			AfxFormatString1( strFileSize, IDS_SIZE_BYTES, strBytes );
		}
		else
		{
			CString strKB;
			
			double dblKB = dwFileSize / 1024.0;
			strKB.Format( "%.2f", dblKB );

			AfxFormatString2( strFileSize, IDS_SIZE_KB, strKB, strBytes );
		}

		CloseHandle( hFile );
	}

	strText = strRelativeFileName + _T("     ") + strFileModified + _T("     ") + strFileSize;

	int nPos = m_listbxFiles.AddString( strText );
	if( nPos >= 0 )
	{
		m_listbxFiles.SetItemDataPtr( nPos, pFileNode );

		// Set horizontal extent
		int nCurExtent = m_listbxFiles.GetHorizontalExtent();

		CDC* pDC = m_listbxFiles.GetDC();
		if( pDC )
		{
			CSize sizeText = pDC->GetTextExtent( strText );

			if( sizeText.cx > nCurExtent )
			{
				m_listbxFiles.SetHorizontalExtent( sizeText.cx );
			}

			m_listbxFiles.ReleaseDC( pDC );
		}
	}
}


void CRuntimeDupeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuntimeDupeDlg)
	DDX_Control(pDX, IDC_PROMPT, m_staticPrompt);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LIST, m_listbxFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRuntimeDupeDlg, CDialog)
	//{{AFX_MSG_MAP(CRuntimeDupeDlg)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelChangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDupeDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDupeDlg::OnInitDialog

BOOL CRuntimeDupeDlg::OnInitDialog() 
{
	ASSERT( m_pFileNode != NULL );

	CDialog::OnInitDialog();

	// Find the Project
	IDMUSProdProject* pIProject;
	if( SUCCEEDED ( theApp.m_pFramework->FindProject( m_pFileNode, &pIProject ) ) )
	{
		m_pProject = (CProject *)pIProject;
		pIProject->Release();
	}

	ASSERT( m_pProject != NULL );

	CString strPrompt;
	CString strRuntimeFileName;

	// Get "Runtime" filename
	m_pFileNode->ConstructRuntimePath( strRuntimeFileName );
	strRuntimeFileName += m_pFileNode->m_strRuntimeFileName;
	
	// Set Prompt
	AfxFormatString2( strPrompt, IDS_RUNTIME_DUPE_PROMPT, m_pProject->m_strName, strRuntimeFileName );
	m_staticPrompt.SetWindowText( strPrompt );

	// Fill the list box with filenames
	CFileNode* pFileNode;
	CString strFileName;

	POSITION pos = m_pProject->m_lstFiles.GetHeadPosition();
    while( pos )
    {
        pFileNode = m_pProject->m_lstFiles.GetNext( pos );

		pFileNode->ConstructRuntimePath( strFileName );
		strFileName += pFileNode->m_strRuntimeFileName;

		if( strFileName.CompareNoCase( strRuntimeFileName ) == 0 )
		{
			AddToList( pFileNode );
		}
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CRuntimeDupeDlg::OnSelChangeList

void CRuntimeDupeDlg::OnSelChangeList() 
{
	m_btnOK.EnableWindow( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CRuntimeDupeDlg::OnOK

void CRuntimeDupeDlg::OnOK() 
{
	CFileNode* pFileNode;
	int nNbrFiles;
	int nCurSel;
	int i;

	nCurSel = m_listbxFiles.GetCurSel();
	nNbrFiles = m_listbxFiles.GetCount();

	for( i = 0 ;  i < nNbrFiles ;  i++ )
	{
		pFileNode = (CFileNode *)m_listbxFiles.GetItemDataPtr( i );
		if( pFileNode > 0 )
		{
			if( i == nCurSel ) 
			{
				pFileNode->m_nRuntimeSaveAction = RSA_SAVE;
			}
			else
			{
				pFileNode->m_nRuntimeSaveAction = RSA_SKIP;
			}
		}
	}
	
	CDialog::OnOK();
}

