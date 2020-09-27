// DialogLinkExisting.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "DialogLinkExisting.h"
#include "Style.h"
#include "Pattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogLinkExisting dialog


CDialogLinkExisting::CDialogLinkExisting(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLinkExisting::IDD, pParent)
{
	m_pDMPartRef = NULL;
	m_pDMStyle = NULL;

	//{{AFX_DATA_INIT(CDialogLinkExisting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogLinkExisting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLinkExisting)
	DDX_Control(pDX, IDC_LIST_PART, m_listPart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogLinkExisting, CDialog)
	//{{AFX_MSG_MAP(CDialogLinkExisting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogLinkExisting message handlers

BOOL CDialogLinkExisting::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Template for strings to display ("%s : %s(%d)")
	CString strTemplate;
	strTemplate.LoadString( IDS_SELECT_PARTREF_TEMPL );

	// Template for broadcast strings to display ("%s : %s(%s)")
	CString strTemplateBCast;
	strTemplateBCast.LoadString( IDS_SELECT_BCAST_PARTREF_TEMPLATE );

	// String to display if PartRef has no name
	CString strUnnamed;
	strUnnamed.LoadString( IDS_UNNAMED_PARTREF );

	// Temporary string to hold text to add to list box and PartRef name
	CString strTmp, strPartRef;

	// Index of item in the list box
	int nIndex;

	// Iterate through all Patterns
	CDirectMusicPattern *pPattern;
	POSITION posPattern = m_pDMStyle->m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( posPattern )
	{
        pPattern = m_pDMStyle->m_StylePatterns.m_lstPatterns.GetNext( posPattern );

		// Iterate through all PartRefs
		CDirectMusicPartRef *pPartRef;
		POSITION posPartRef = pPattern->m_lstPartRefs.GetHeadPosition();
		while( posPartRef )
		{
			pPartRef = pPattern->m_lstPartRefs.GetNext( posPartRef );

			// Get the PartRef name
			strPartRef = pPartRef->m_strName.IsEmpty() ? strUnnamed : pPartRef->m_strName;
			//strTmp = pPartRef->m_strName.IsEmpty() ? strUnnamed : pPartRef->m_strName;

			// Format the name to display
			if( pPartRef->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				strTmp.Format( strTemplate, pPattern->m_strName, strPartRef, pPartRef->m_dwPChannel + 1 );
			}
			else
			{
				CString strPChannelName;
				if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
				{
					strPChannelName.LoadString( IDS_BROADCAST_SEG );
				}
				else if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
				{
					strPChannelName.LoadString( IDS_BROADCAST_PERF );
				}
				else if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
				{
					strPChannelName.LoadString( IDS_BROADCAST_APATH );
				}
				else if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
				{
					strPChannelName.LoadString( IDS_BROADCAST_GRP );
				}

				strTmp.Format( strTemplateBCast, pPattern->m_strName, strPartRef, strPChannelName );
			}

			// Add the PartRef's name to the list box
			nIndex = m_listPart.AddString( strTmp );
			if( nIndex >= 0 )
			{
				m_listPart.SetItemDataPtr( nIndex, pPartRef );
			}
		}
	}
	
	posPattern = m_pDMStyle->m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( posPattern )
	{
        pPattern = m_pDMStyle->m_StyleMotifs.m_lstMotifs.GetNext( posPattern );

		// Iterate through all PartRefs
		CDirectMusicPartRef *pPartRef;
		POSITION posPartRef = pPattern->m_lstPartRefs.GetHeadPosition();
		while( posPartRef )
		{
			pPartRef = pPattern->m_lstPartRefs.GetNext( posPartRef );

			// Get the PartRef name
			strPartRef = pPartRef->m_strName.IsEmpty() ? strUnnamed : pPartRef->m_strName;
			//strTmp = pPartRef->m_strName.IsEmpty() ? strUnnamed : pPartRef->m_strName;

			// Format the name to display
			if( pPartRef->m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				strTmp.Format( strTemplate, pPattern->m_strName, strPartRef, pPartRef->m_dwPChannel + 1 );
			}
			else
			{
				CString strPChannelName;
				if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
				{
					strPChannelName.LoadString( IDS_BROADCAST_SEG );
				}
				else if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
				{
					strPChannelName.LoadString( IDS_BROADCAST_PERF );
				}
				else if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
				{
					strPChannelName.LoadString( IDS_BROADCAST_APATH );
				}
				else if( pPartRef->m_dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
				{
					strPChannelName.LoadString( IDS_BROADCAST_GRP );
				}

				strTmp.Format( strTemplateBCast, pPattern->m_strName, strPartRef, strPChannelName );
			}

			// Add the PartRef's name to the list box
			nIndex = m_listPart.AddString( strTmp );
			if( nIndex >= 0 )
			{
				m_listPart.SetItemDataPtr( nIndex, pPartRef );
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogLinkExisting::OnOK() 
{
	// Index of the selected item in the list box
	int nIndex;
	nIndex = m_listPart.GetCurSel();

	// If index is valid, set m_pDMPartRef to the associated PartRef
	if( nIndex >= 0 )
	{
		m_pDMPartRef = (CDirectMusicPartRef *)m_listPart.GetItemDataPtr( nIndex );
	}
	
	CDialog::OnOK();
}
