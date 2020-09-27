// DialogSelectPartRef.cpp : implementation file
//

#include "stdafx.h"
#include "midistripmgr.h"
#include "DialogSelectPartRef.h"
#include "Pattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectPartRef dialog


CDialogSelectPartRef::CDialogSelectPartRef(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSelectPartRef::IDD, pParent)
{
	m_pDMPartRef = NULL;
	m_dwPChannel = 0;

	//{{AFX_DATA_INIT(CDialogSelectPartRef)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogSelectPartRef::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSelectPartRef)
	DDX_Control(pDX, IDC_LIST_PARTREF, m_listPartRef);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogSelectPartRef, CDialog)
	//{{AFX_MSG_MAP(CDialogSelectPartRef)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectPartRef message handlers

BOOL CDialogSelectPartRef::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Template for strings to display ("%s : %s")
	CString strTemplate;
	strTemplate.LoadString( IDS_SELECT_PARTREF_TEMPL );

	// String to display if Part has no name
	CString strUnnamed;
	strUnnamed.LoadString( IDS_UNNAMED );

	// Temporary string to hold text to add to list box and PartRef name
	CString strTmp, strPartRef;

	// Index of item in the list box
	int nIndex;

	// Iterate through all PartRefs
	CDirectMusicPartRef *pPartRef;
    POSITION pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

		if( m_dwPChannel == pPartRef->m_dwPChannel )
		{
			// Get the PartRef name
			//strPartRef = pPartRef->m_strName.IsEmpty() ? strUnnamed : pPartRef->m_strName;
			strTmp = pPartRef->m_strName.IsEmpty() ? strUnnamed : pPartRef->m_strName;

			/*
			// Format the name to display
			if( pPartRef->m_pDMPart->m_strName.IsEmpty() )
			{
				strTmp.Format( strTemplate, strPartRef, strUnnamed );
			}
			else
			{
				strTmp.Format( strTemplate, strPartRef, pPartRef->m_pDMPart->m_strName );
			}
			*/

			// Add the PartRef to the list box
			nIndex = m_listPartRef.AddString( strTmp );
			if( nIndex >= 0 )
			{
				m_listPartRef.SetItemDataPtr( nIndex, pPartRef );
			}
		}
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogSelectPartRef::OnOK() 
{
	// Index of the selected item in the list box
	int nIndex;
	nIndex = m_listPartRef.GetCurSel();

	// If index is valid, set m_pDMPartRef to the associated PartRef
	if( nIndex >= 0 )
	{
		m_pDMPartRef = (CDirectMusicPartRef *)m_listPartRef.GetItemDataPtr( nIndex );
	}
	
	CDialog::OnOK();
}
