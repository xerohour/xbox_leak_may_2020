// NewPatternDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "NewPatternDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg dialog


CNewPatternDlg::CNewPatternDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPatternDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewPatternDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_wEmbellishment = EMB_NORMAL;
	m_pPattern = NULL;
	m_pStyle = NULL;
}


void CNewPatternDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewPatternDlg)
	DDX_Control(pDX, IDC_PATTERN_LIST, m_lstbxPattern);
	DDX_Control(pDX, IDC_NEW_PATTERN, m_radioNewPattern);
	DDX_Control(pDX, IDC_LINK_PATTERN, m_radioLinkPattern);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewPatternDlg, CDialog)
	//{{AFX_MSG_MAP(CNewPatternDlg)
	ON_BN_CLICKED(IDC_NEW_PATTERN, OnNewPattern)
	ON_BN_DOUBLECLICKED(IDC_NEW_PATTERN, OnDoubleClickedNewPattern)
	ON_BN_CLICKED(IDC_LINK_PATTERN, OnLinkPattern)
	ON_BN_DOUBLECLICKED(IDC_LINK_PATTERN, OnDoubleClickedLinkPattern)
	ON_LBN_SELCHANGE(IDC_PATTERN_LIST, OnSelChangePatternList)
	ON_WM_COMPAREITEM()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnInitDialog

BOOL CNewPatternDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT( m_pStyle != NULL );	// Should have been set after CNewPatternDlg was constructed

	// Get text for dialog title and controls
	CString strTitle;
	CString strTextNew;
	CString strTextLink;

	if( m_wEmbellishment & EMB_MOTIF )
	{
		strTitle.LoadString( IDS_NEW_MOTIF_TEXT );
		strTextNew.LoadString( IDS_CREATE_NEW_MOTIF_TEXT );
		strTextLink.LoadString( IDS_CREATE_LINK_MOTIF_TEXT );
	}
	else
	{
		strTitle.LoadString( IDS_NEW_PATTERN_TEXT );
		strTextNew.LoadString( IDS_CREATE_NEW_PATTERN_TEXT );
		strTextLink.LoadString( IDS_CREATE_LINK_PATTERN_TEXT );
	}

	// Initialize title of dialog
	SetWindowText( strTitle );
	
	// Initialize radio buttons
	m_radioNewPattern.SetCheck( 1 );
	m_radioNewPattern.SetWindowText( strTextNew );
	m_radioLinkPattern.SetCheck( 0 );
	m_radioLinkPattern.SetWindowText( strTextLink );

	// Fill Pattern list box
	CDirectMusicPattern* pPattern;

    //		Add Patterns
	POSITION pos = m_pStyle->m_StylePatterns.m_lstPatterns.GetHeadPosition();
    while( pos )
    {
        pPattern = m_pStyle->m_StylePatterns.m_lstPatterns.GetNext( pos );

		m_lstbxPattern.AddString( (LPCTSTR)pPattern  );
	}

    //		Add Motifs
    pos = m_pStyle->m_StyleMotifs.m_lstMotifs.GetHeadPosition();
    while( pos )
    {
        pPattern = m_pStyle->m_StyleMotifs.m_lstMotifs.GetNext( pos );

		m_lstbxPattern.AddString( (LPCTSTR)pPattern  );
	}

	// Disable Pattern list box
	m_lstbxPattern.EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnNewPattern

void CNewPatternDlg::OnNewPattern() 
{
	if( m_radioNewPattern.GetCheck() )
	{
		m_lstbxPattern.EnableWindow( FALSE );
	}
	else
	{
		m_lstbxPattern.EnableWindow( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnDoubleClickedNewPattern

void CNewPatternDlg::OnDoubleClickedNewPattern() 
{
	OnNewPattern();
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnLinkPattern

void CNewPatternDlg::OnLinkPattern() 
{
	if( m_radioLinkPattern.GetCheck() )
	{
		m_lstbxPattern.EnableWindow( TRUE );
	}
	else
	{
		m_lstbxPattern.EnableWindow( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnDoubleClickedLinkPattern

void CNewPatternDlg::OnDoubleClickedLinkPattern() 
{
	OnLinkPattern();
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnSelChangePatternList

void CNewPatternDlg::OnSelChangePatternList() 
{
	m_pPattern = NULL;

	// Get the current Pattern
	int nPos = m_lstbxPattern.GetCurSel();
	if( nPos == LB_ERR )
	{
		return;
	}

	// Get a pointer to the Pattern
	CDirectMusicPattern* pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
	if( pPattern 
	&&  pPattern != (CDirectMusicPattern *)-1 )
	{
		m_pPattern = pPattern;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnOK

void CNewPatternDlg::OnOK() 
{
	if( m_radioNewPattern.GetCheck() )
	{
		EndDialog( IDC_NEW_PATTERN );
	}
	else if( m_radioLinkPattern.GetCheck() )
	{
		if( m_pPattern == NULL )
		{
			AfxMessageBox( IDS_MISSING_PATTERN );
			m_lstbxPattern.SetFocus();
		}
		else
		{
			EndDialog( IDC_LINK_PATTERN );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnMeasureItem

void CNewPatternDlg::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_PATTERN_LIST:
		{
			TEXTMETRIC tm;
			
			CDC* pDC = GetDC();
			if( pDC )
			{
				pDC->GetTextMetrics( &tm );
				lpMeasureItemStruct->itemHeight = tm.tmHeight + 1;

				ReleaseDC( pDC );
				return;
			}
		}
	}
	
	CDialog::OnMeasureItem( nIDCtl, lpMeasureItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnCompareItem

int CNewPatternDlg::OnCompareItem( int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern1 = (CDirectMusicPattern *)lpCompareItemStruct->itemData1;
			CDirectMusicPattern* pPattern2 = (CDirectMusicPattern *)lpCompareItemStruct->itemData2;

			// Place Motifs at the bottom of the list
			if( !(pPattern1->m_wEmbellishment & EMB_MOTIF)
			&&   (pPattern2->m_wEmbellishment & EMB_MOTIF) )
			{
				return -1;
			}
			if(  (pPattern1->m_wEmbellishment & EMB_MOTIF)
			&&  !(pPattern2->m_wEmbellishment & EMB_MOTIF) )
			{
				return 1;
			}

			return pPattern1->m_strName.CompareNoCase( pPattern2->m_strName );
		}
	}
	
	return CDialog::OnCompareItem( nIDCtl, lpCompareItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg::OnDrawItem

void CNewPatternDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_PATTERN_LIST:
		{
			if( lpDrawItemStruct->itemID == -1 )
			{
				return;
			}

			CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
			if( pDC == NULL )
			{
				return;
			}

            if( lpDrawItemStruct->itemAction & ODA_FOCUS )
			{
		        InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
                pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
		        InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
				return;
			}

            if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
            ||  lpDrawItemStruct->itemAction & ODA_SELECT )
			{
				// Get the Pattern
				CDirectMusicPattern* pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( lpDrawItemStruct->itemID );
				ASSERT( pPattern != NULL ); 

				// Format the string
				CString strText;
				if( pPattern->m_wEmbellishment & EMB_MOTIF )
				{
					CString strMotifText;
					strMotifText.LoadString( IDS_MOTIF_TEXT );
					strText = strMotifText + _T(":  ") + pPattern->m_strName;
				}
				else
				{
					CString strPatternText;
					strPatternText.LoadString( IDS_PATTERN_TEXT );
					strText = strPatternText + _T(":  ") + pPattern->m_strName;
				}
				
				int nBkModeOld = pDC->SetBkMode( TRANSPARENT );
				if( m_lstbxPattern.IsWindowEnabled() )
				{
					pDC->SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
				}
				else
				{
					pDC->SetTextColor( ::GetSysColor(COLOR_GRAYTEXT) );
				}

				// Draw the string
				pDC->FillSolidRect( &lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_WINDOW) );
		        InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
		        pDC->DrawText( strText, -1, &lpDrawItemStruct->rcItem, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );
		        InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
				
				// Cleanup
				pDC->SetBkMode( nBkModeOld );

				// Invert if selected
                if( lpDrawItemStruct->itemState & ODS_SELECTED )
				{
					pDC->InvertRect( &lpDrawItemStruct->rcItem );
				}
			}

			return;
		}
	}
	
	CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
}
