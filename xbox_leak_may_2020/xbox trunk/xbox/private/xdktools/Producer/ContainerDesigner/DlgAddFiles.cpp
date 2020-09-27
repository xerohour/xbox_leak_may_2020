// DlgAddFiles.cpp : implementation file
//

#include "stdafx.h"
#include "ContainerDesignerDll.h"
#include "Container.h"
#include "DlgAddFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles dialog

CDlgAddFiles::CDlgAddFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddFiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddFiles)
	//}}AFX_DATA_INIT

	m_nSortType = SORT_NAME;
	m_fDisableReferenced = false;
}


void CDlgAddFiles::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddFiles)
	DDX_Control(pDX, IDC_REFERENCE_PROMPT1, m_staticReferencePrompt1);
	DDX_Control(pDX, IDC_REFERENCE_PROMPT2, m_staticReferencePrompt2);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LIST_REFERENCE, m_lstbxReference);
	DDX_Control(pDX, IDC_LIST_AVAILABLE, m_lstbxAvailable);
	DDX_Control(pDX, IDC_LIST_EMBED, m_lstbxEmbed);
	DDX_Control(pDX, IDC_BTN_RESET, m_btnReset);
	DDX_Control(pDX, IDC_BTN_REFERENCE_REMOVE, m_btnRemoveReference);
	DDX_Control(pDX, IDC_BTN_REFERENCE_ADD, m_btnAddReference);
	DDX_Control(pDX, IDC_BTN_EMBED_REMOVE, m_btnRemoveEmbed);
	DDX_Control(pDX, IDC_BTN_EMBED_ADD, m_btnAddEmbed);
	DDX_Control(pDX, IDC_RADIO_SORT_NAME, m_radioSortName);
	DDX_Control(pDX, IDC_RADIO_SORT_TYPE, m_radioSortType);
	DDX_Control(pDX, IDC_RADIO_SORT_SIZE, m_radioSortSize);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::FillAvailableList

void CDlgAddFiles::FillAvailableList( void )
{
	m_lstbxAvailable.SetRedraw( FALSE );

	// Initialize the list box
	m_lstbxAvailable.ResetContent();
	m_lstbxAvailable.SetHorizontalExtent( 0 );

	// Fill the list box
    POSITION pos = m_lstAvailable.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pINode = m_lstAvailable.GetNext( pos );

		m_lstbxAvailable.AddString( (LPCTSTR)pINode );
	}

	m_lstbxAvailable.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::FillEmbedList

void CDlgAddFiles::FillEmbedList( void )
{
	m_lstbxEmbed.SetRedraw( FALSE );

	// Initialize the list box
	m_lstbxEmbed.ResetContent();
	m_lstbxEmbed.SetHorizontalExtent( 0 );

	// Fill the list box
    POSITION pos = m_lstEmbedded.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pINode = m_lstEmbedded.GetNext( pos );

		m_lstbxEmbed.AddString( (LPCTSTR)pINode );
	}

	m_lstbxEmbed.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::FillReferenceList

void CDlgAddFiles::FillReferenceList( void )
{
	m_lstbxReference.SetRedraw( FALSE );

	// Initialize the list box
	m_lstbxReference.ResetContent();
	m_lstbxReference.SetHorizontalExtent( 0 );

	// Fill the list box
    POSITION pos = m_lstReferenced.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pINode = m_lstReferenced.GetNext( pos );

		m_lstbxReference.AddString( (LPCTSTR)pINode );
	}

	m_lstbxReference.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::SortAvailableList

void CDlgAddFiles::SortAvailableList( void )
{
	m_lstbxAvailable.SetRedraw( FALSE );

	// Store current list
	CTypedPtrList<CPtrList, IDMUSProdNode*> listAvailable;
	int nCount = m_lstbxAvailable.GetCount();
	for( int nPos = 0 ;  nPos < nCount ;  nPos++ )
	{
		IDMUSProdNode* pINode = (IDMUSProdNode *)m_lstbxAvailable.GetItemData( nPos ); 

		if( pINode )
		{
			listAvailable.AddTail( pINode );
		}
	}

	// Initialize the list box
	m_lstbxAvailable.ResetContent();
	m_lstbxAvailable.SetHorizontalExtent( 0 );

	// Fill the list box
    POSITION pos = listAvailable.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pINode = listAvailable.GetNext( pos );

		m_lstbxAvailable.AddString( (LPCTSTR)pINode );
	}

	m_lstbxAvailable.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::SortEmbedList

void CDlgAddFiles::SortEmbedList( void )
{
	m_lstbxEmbed.SetRedraw( FALSE );

	// Store current list
	CTypedPtrList<CPtrList, IDMUSProdNode*> listEmbedded;
	int nCount = m_lstbxEmbed.GetCount();
	for( int nPos = 0 ;  nPos < nCount ;  nPos++ )
	{
		IDMUSProdNode* pINode = (IDMUSProdNode *)m_lstbxEmbed.GetItemData( nPos ); 

		if( pINode )
		{
			listEmbedded.AddTail( pINode );
		}
	}

	// Initialize the list box
	m_lstbxEmbed.ResetContent();
	m_lstbxEmbed.SetHorizontalExtent( 0 );

	// Fill the list box
    POSITION pos = listEmbedded.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pINode = listEmbedded.GetNext( pos );

		m_lstbxEmbed.AddString( (LPCTSTR)pINode );
	}

	m_lstbxEmbed.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::SortReferenceList

void CDlgAddFiles::SortReferenceList( void )
{
	m_lstbxReference.SetRedraw( FALSE );

	// Store current list
	CTypedPtrList<CPtrList, IDMUSProdNode*> listReferenced;
	int nCount = m_lstbxReference.GetCount();
	for( int nPos = 0 ;  nPos < nCount ;  nPos++ )
	{
		IDMUSProdNode* pINode = (IDMUSProdNode *)m_lstbxReference.GetItemData( nPos ); 

		if( pINode )
		{
			listReferenced.AddTail( pINode );
		}
	}

	// Initialize the list box
	m_lstbxReference.ResetContent();
	m_lstbxReference.SetHorizontalExtent( 0 );

	// Fill the list box
    POSITION pos = listReferenced.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pINode = listReferenced.GetNext( pos );

		m_lstbxReference.AddString( (LPCTSTR)pINode );
	}

	m_lstbxReference.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::FormatTextUI

void CDlgAddFiles::FormatTextUI( IDMUSProdNode* pINode, CString& strText )
{
	strText.Empty();

	CString strProjectDir;
	
	BSTR bstrFileName;
	IDMUSProdProject* pIProject;
	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->FindProject( pINode, &pIProject ) ) )
	{
		IDMUSProdNode* pIProjectNode;
		if( SUCCEEDED ( pIProject->QueryInterface( IID_IDMUSProdNode, (void**)&pIProjectNode ) ) )
		{
			if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileName( pIProjectNode, &bstrFileName ) ) )
			{
				strProjectDir = bstrFileName;
				::SysFreeString( bstrFileName );

				int nFindPos = strProjectDir.ReverseFind( (TCHAR)'\\' );
				if( nFindPos != -1 )
				{
					strProjectDir = strProjectDir.Left( nFindPos + 1 );
				}
			}

			RELEASE( pIProjectNode );
		}

		RELEASE( pIProject );
	}

	if( SUCCEEDED ( theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileName( pINode, &bstrFileName ) ) )
	{
		strText = bstrFileName;
		::SysFreeString( bstrFileName );

		// Strip Project directory from path
		strText = strText.Right( strText.GetLength() - strProjectDir.GetLength() );

		int nFindPos = strText.Find( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strText = _T( "..\\" ) + strText;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::CreateFont

CFont* CDlgAddFiles::CreateFont( void )
{
	CFont* pFont = new CFont;

	if( pFont 
	&&  pFont->CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 						   DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
						   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) )
	{
		return pFont;
	}

	if( pFont )
	{
		delete pFont;
	}
	return NULL;
}


BEGIN_MESSAGE_MAP(CDlgAddFiles, CDialog)
	//{{AFX_MSG_MAP(CDlgAddFiles)
	ON_WM_COMPAREITEM()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_RADIO_SORT_NAME, OnRadioSortName)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SORT_NAME, OnDoubleClickedRadioSortName)
	ON_BN_CLICKED(IDC_RADIO_SORT_TYPE, OnRadioSortType)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SORT_TYPE, OnDoubleClickedRadioSortType)
	ON_BN_CLICKED(IDC_RADIO_SORT_SIZE, OnRadioSortSize)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SORT_SIZE, OnDoubleClickedRadioSortSize)
	ON_BN_CLICKED(IDC_BTN_EMBED_ADD, OnBtnEmbedAdd)
	ON_BN_CLICKED(IDC_BTN_EMBED_REMOVE, OnBtnEmbedRemove)
	ON_BN_CLICKED(IDC_BTN_REFERENCE_ADD, OnBtnReferenceAdd)
	ON_BN_CLICKED(IDC_BTN_REFERENCE_REMOVE, OnBtnReferenceRemove)
	ON_BN_CLICKED(IDC_BTN_RESET, OnBtnReset)
	ON_LBN_DBLCLK(IDC_LIST_AVAILABLE, OnDblClkListAvailable)
	ON_LBN_DBLCLK(IDC_LIST_EMBED, OnDblClkListEmbed)
	ON_LBN_DBLCLK(IDC_LIST_REFERENCE, OnDblClkListReference)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles message handlers

/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnInitDialog

BOOL CDlgAddFiles::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	// Set dialog title
	if( m_strTitle.IsEmpty() == FALSE )
	{
		SetWindowText( m_strTitle );
	}

	// Initialize list boxes
	FillAvailableList();
	FillEmbedList();
	FillReferenceList();

	// Initialize sort order radio buttons
	m_radioSortName.SetCheck( (m_nSortType == SORT_NAME) ? 1 : 0 );
	m_radioSortType.SetCheck( (m_nSortType == SORT_TYPE) ? 1 : 0 );
	m_radioSortSize.SetCheck( (m_nSortType == SORT_SIZE) ? 1 : 0 );

	// Disable "reference" controls
	if( m_fDisableReferenced )
	{
		m_lstbxReference.EnableWindow( FALSE );
		m_btnAddReference.EnableWindow( FALSE );
		m_btnRemoveReference.EnableWindow( FALSE );
		m_staticReferencePrompt1.EnableWindow( FALSE );
		m_staticReferencePrompt2.EnableWindow( FALSE );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnCompareItem

int CDlgAddFiles::OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nIDCtl )
	{
		case IDC_LIST_AVAILABLE:
		case IDC_LIST_EMBED:
		case IDC_LIST_REFERENCE:
		{
			IDMUSProdNode* pINode1 = (IDMUSProdNode *)lpCompareItemStruct->itemData1;
			IDMUSProdNode* pINode2 = (IDMUSProdNode *)lpCompareItemStruct->itemData2;
			
			CString strName1;
			FormatTextUI( pINode1, strName1 );
			
			CString strName2;
			FormatTextUI( pINode2, strName2 );

			switch( m_nSortType )
			{
				// Sort by file type
				case SORT_TYPE:
				{
					CString strExt1;
					int nFindPos = strName1.ReverseFind( (TCHAR)'.' );
					if( nFindPos != -1 )
					{
						strExt1 = strName1.Right( strName1.GetLength() - nFindPos - 1 );
					}

					CString strExt2;
					nFindPos = strName2.ReverseFind( (TCHAR)'.' );
					if( nFindPos != -1 )
					{
						strExt2 = strName2.Right( strName2.GetLength() - nFindPos - 1 );
					}

					if( strExt1.CompareNoCase( strExt2 ) > 0 )
					{
						return 1;
					}
					if( strExt1.CompareNoCase( strExt2 ) < 0 )
					{
						return -1;
					}
					return strName1.CompareNoCase( strName2 );
				}

				// Sort by file size
				case SORT_SIZE:
				{
					BSTR bstrFileName1;
					theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileName( pINode1, &bstrFileName1 );
					CString strFileName1 = bstrFileName1;
					::SysFreeString( bstrFileName1 );

					BSTR bstrFileName2;
					theApp.m_pContainerComponent->m_pIFramework8->GetNodeFileName( pINode2, &bstrFileName2 );
					CString strFileName2 = bstrFileName2;
					::SysFreeString( bstrFileName2 );

					DWORD dwFileSize1 = 0;
					HANDLE hFile = ::CreateFile( strFileName1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 											 FILE_ATTRIBUTE_NORMAL, NULL );
					if( hFile != INVALID_HANDLE_VALUE )
					{
						dwFileSize1 = GetFileSize( hFile, NULL );
						CloseHandle( hFile );
					}

					DWORD dwFileSize2 = 0;
					hFile = ::CreateFile( strFileName2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 									  FILE_ATTRIBUTE_NORMAL, NULL );
					if( hFile != INVALID_HANDLE_VALUE )
					{
						dwFileSize2 = GetFileSize( hFile, NULL );
						CloseHandle( hFile );
					}

					if( dwFileSize1 < dwFileSize2 )
					{
						return 1;
					}
					if( dwFileSize1 > dwFileSize2 )
					{
						return -1;
					}
					return strName1.CompareNoCase( strName2 );
				}

				// Sort by file name
				default:
					return strName1.CompareNoCase( strName2 );
			}
		}
	}
	
	return CDialog::OnCompareItem( nIDCtl, lpCompareItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnDrawItem

void CDlgAddFiles::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( lpDrawItemStruct->itemID == -1 )
	{
		// Just deal with focus rect
		InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
		DrawFocusRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem );
		InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
		return;
	}

	if( nIDCtl != IDC_LIST_AVAILABLE
	&&  nIDCtl != IDC_LIST_EMBED
	&&  nIDCtl != IDC_LIST_REFERENCE )
	{
		ASSERT( 0 );
		CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
	}

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC == NULL )
	{
		return;
	}

    if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
    ||  lpDrawItemStruct->itemAction & ODA_SELECT 
    ||  lpDrawItemStruct->itemAction & ODA_FOCUS )
	{
		IDMUSProdNode* pINode = (IDMUSProdNode *)lpDrawItemStruct->itemData; 
		ASSERT( pINode != NULL );

		// Determine text
		CString strText;
		FormatTextUI( pINode, strText );

	    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
		int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

		CRect rect( 0, 0, nWidth, nHeight );

		CDC dc;
		CBitmap bmp;

		if( dc.CreateCompatibleDC( pDC )
		&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
		{
			CFont* pFontOld = NULL;
			CFont* pFont = CreateFont();
			if( pFont )
			{
				pFontOld = dc.SelectObject( pFont );
			}
			CBitmap* pBitmapOld = dc.SelectObject( &bmp );
			int nBkModeOld = dc.SetBkMode( TRANSPARENT );

			// Set horizontal extent
			TEXTMETRIC tm;
			dc.GetTextMetrics( &tm );
			CSize sizeText = dc.GetTextExtent( strText );
			sizeText.cx += tm.tmMaxCharWidth;
			if( sizeText.cx > ::SendMessage(lpDrawItemStruct->hwndItem, LB_GETHORIZONTALEXTENT, 0, 0) )
			{
				::SendMessage( lpDrawItemStruct->hwndItem, LB_SETHORIZONTALEXTENT, sizeText.cx, 0 );
			}

			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				if( ::GetFocus() == lpDrawItemStruct->hwndItem )
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
					dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}
				else
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
					dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}
			}
			else
			{
				dc.FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );
				dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
			}

			// Draw the item
			rect.left  += 3;
			dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );
			pDC->BitBlt( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nWidth, nHeight, 
						 &dc, 0, 0, SRCCOPY );

			// Clean up
			dc.SetBkMode( nBkModeOld );
			if( pFontOld )
			{
				dc.SelectObject( pFontOld );
				pFont->DeleteObject();
				delete pFont;
			}
			if( pBitmapOld )
			{
				dc.SelectObject( pBitmapOld );
			}
		}

		if( ::GetFocus() == lpDrawItemStruct->hwndItem )
		{
			if( (lpDrawItemStruct->itemAction & ODA_FOCUS)
			&&  (lpDrawItemStruct->itemState & ODS_FOCUS) )
			{
				InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
				InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnMeasureItem

void CDlgAddFiles::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nIDCtl )
	{
		case IDC_LIST_AVAILABLE:
		case IDC_LIST_EMBED:
		case IDC_LIST_REFERENCE:
		{
			CDC* pDC = GetDC();
			if( pDC )
			{
				CFont* pFontOld = NULL;
				CFont* pFont = CreateFont();
				if( pFont )
				{
					pFontOld = pDC->SelectObject( pFont );
				}

				TEXTMETRIC tm;
				pDC->GetTextMetrics( &tm );
				lpMeasureItemStruct->itemHeight = tm.tmHeight + 1;
				
				if( pFontOld )
				{
					pDC->SelectObject( pFontOld );
					pFont->DeleteObject();
					delete pFont;
				}

				ReleaseDC( pDC );
				return;
			}
		}
	}
	
	CDialog::OnMeasureItem( nIDCtl, lpMeasureItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnRadioSortName

void CDlgAddFiles::OnRadioSortName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;

	m_radioSortName.SetCheck( 1 );

	if( m_nSortType != SORT_NAME )
	{
		m_nSortType = SORT_NAME;

		// Sort list boxes
		SortAvailableList();
		SortEmbedList();
		SortReferenceList();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnDoubleClickedRadioSortName

void CDlgAddFiles::OnDoubleClickedRadioSortName() 
{
	OnRadioSortName();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnRadioSortType

void CDlgAddFiles::OnRadioSortType() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;

	m_radioSortType.SetCheck( 1 );

	if( m_nSortType != SORT_TYPE )
	{
		m_nSortType = SORT_TYPE;

		// Sort list boxes
		SortAvailableList();
		SortEmbedList();
		SortReferenceList();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnDoubleClickedRadioSortType

void CDlgAddFiles::OnDoubleClickedRadioSortType() 
{
	OnRadioSortType();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnRadioSortSize

void CDlgAddFiles::OnRadioSortSize() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;

	m_radioSortSize.SetCheck( 1 );

	if( m_nSortType != SORT_SIZE )
	{
		m_nSortType = SORT_SIZE;

		// Sort list boxes
		SortAvailableList();
		SortEmbedList();
		SortReferenceList();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnDoubleClickedRadioSortSize

void CDlgAddFiles::OnDoubleClickedRadioSortSize() 
{
	OnRadioSortSize();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnBtnEmbedAdd

void CDlgAddFiles::OnBtnEmbedAdd() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nNbrSelItems = m_lstbxAvailable.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	m_lstbxAvailable.SetRedraw( FALSE );
	m_lstbxEmbed.SetRedraw( FALSE );

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxAvailable.GetSelItems( nNbrSelItems, pnSelItems );

		m_lstbxAvailable.SetSel( -1, FALSE ); 
		m_lstbxEmbed.SetSel( -1, FALSE ); 

		for( int nPos = (nNbrSelItems - 1);  nPos >= 0 ;  nPos-- )
		{
			IDMUSProdNode* pINode = (IDMUSProdNode *)m_lstbxAvailable.GetItemDataPtr( pnSelItems[nPos] );
		
			if( pINode
			&&  pINode != (IDMUSProdNode *)0xFFFFFFFF )
			{
				// Delete selected item from "Available" list
				m_lstbxAvailable.DeleteString( pnSelItems[nPos] );

				// Place selected item in "Embed" list
				int nThePos = m_lstbxEmbed.AddString( (LPCTSTR)pINode );
				m_lstbxEmbed.SetSel( nThePos, TRUE ); 
				m_lstbxEmbed.SetCaretIndex( nThePos, 0 ); 
			}
		}

		delete [] pnSelItems;
	}

	m_lstbxAvailable.SetRedraw( TRUE );
	m_lstbxEmbed.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnBtnEmbedRemove

void CDlgAddFiles::OnBtnEmbedRemove() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nNbrSelItems = m_lstbxEmbed.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	m_lstbxAvailable.SetRedraw( FALSE );
	m_lstbxEmbed.SetRedraw( FALSE );

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxEmbed.GetSelItems( nNbrSelItems, pnSelItems );

		m_lstbxAvailable.SetSel( -1, FALSE ); 
		m_lstbxEmbed.SetSel( -1, FALSE ); 

		for( int nPos = (nNbrSelItems - 1);  nPos >= 0 ;  nPos-- )
		{
			IDMUSProdNode* pINode = (IDMUSProdNode *)m_lstbxEmbed.GetItemDataPtr( pnSelItems[nPos] );
		
			if( pINode
			&&  pINode != (IDMUSProdNode *)0xFFFFFFFF )
			{
				// Delete selected item from "Embed" list
				m_lstbxEmbed.DeleteString( pnSelItems[nPos] );

				// Place selected item in "Available" list
				int nThePos = m_lstbxAvailable.AddString( (LPCTSTR)pINode );
				m_lstbxAvailable.SetSel( nThePos, TRUE ); 
				m_lstbxAvailable.SetCaretIndex( nThePos, 0 ); 
			}
		}

		delete [] pnSelItems;
	}

	m_lstbxAvailable.SetRedraw( TRUE );
	m_lstbxEmbed.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnBtnReferenceAdd

void CDlgAddFiles::OnBtnReferenceAdd() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nNbrSelItems = m_lstbxAvailable.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	m_lstbxAvailable.SetRedraw( FALSE );
	m_lstbxReference.SetRedraw( FALSE );

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxAvailable.GetSelItems( nNbrSelItems, pnSelItems );

		m_lstbxAvailable.SetSel( -1, FALSE ); 
		m_lstbxReference.SetSel( -1, FALSE ); 

		for( int nPos = (nNbrSelItems - 1);  nPos >= 0 ;  nPos-- )
		{
			IDMUSProdNode* pINode = (IDMUSProdNode *)m_lstbxAvailable.GetItemDataPtr( pnSelItems[nPos] );
		
			if( pINode
			&&  pINode != (IDMUSProdNode *)0xFFFFFFFF )
			{
				// Delete selected item from "Available" list
				m_lstbxAvailable.DeleteString( pnSelItems[nPos] );

				// Place selected item in "Reference" list
				int nThePos = m_lstbxReference.AddString( (LPCTSTR)pINode );
				m_lstbxReference.SetSel( nThePos, TRUE ); 
				m_lstbxReference.SetCaretIndex( nThePos, 0 ); 
			}
		}

		delete [] pnSelItems;
	}

	m_lstbxAvailable.SetRedraw( TRUE );
	m_lstbxReference.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnBtnReferenceRemove

void CDlgAddFiles::OnBtnReferenceRemove() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nNbrSelItems = m_lstbxReference.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	m_lstbxAvailable.SetRedraw( FALSE );
	m_lstbxReference.SetRedraw( FALSE );

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxReference.GetSelItems( nNbrSelItems, pnSelItems );

		m_lstbxAvailable.SetSel( -1, FALSE ); 
		m_lstbxReference.SetSel( -1, FALSE ); 

		for( int nPos = (nNbrSelItems - 1);  nPos >= 0 ;  nPos-- )
		{
			IDMUSProdNode* pINode = (IDMUSProdNode *)m_lstbxReference.GetItemDataPtr( pnSelItems[nPos] );
		
			if( pINode
			&&  pINode != (IDMUSProdNode *)0xFFFFFFFF )
			{
				// Delete selected item from "Reference" list
				m_lstbxReference.DeleteString( pnSelItems[nPos] );

				// Place selected item in "Available" list
				int nThePos = m_lstbxAvailable.AddString( (LPCTSTR)pINode );
				m_lstbxAvailable.SetSel( nThePos, TRUE ); 
				m_lstbxAvailable.SetCaretIndex( nThePos, 0 ); 
			}
		}

		delete [] pnSelItems;
	}

	m_lstbxAvailable.SetRedraw( TRUE );
	m_lstbxReference.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnBtnReset

void CDlgAddFiles::OnBtnReset() 
{
	// Reset content of list boxes
	FillAvailableList();
	FillEmbedList();
	FillReferenceList();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnDblClkListAvailable

void CDlgAddFiles::OnDblClkListAvailable() 
{
	OnBtnEmbedAdd();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnDblClkListEmbed

void CDlgAddFiles::OnDblClkListEmbed() 
{
	OnBtnEmbedRemove();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnDblClkListReference

void CDlgAddFiles::OnDblClkListReference() 
{
	OnBtnReferenceRemove();
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAddFiles::OnOK

void CDlgAddFiles::OnOK() 
{
	IDMUSProdNode* pINode;
	int nPos;

	// Recreate "Available" list
	m_lstAvailable.RemoveAll();
	int nNbrItems = m_lstbxAvailable.GetCount();
	for( nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		pINode = (IDMUSProdNode *)m_lstbxAvailable.GetItemDataPtr( nPos );
	
		if( pINode
		&&  pINode != (IDMUSProdNode *)0xFFFFFFFF )
		{
			// Place item in "Available" list
			m_lstAvailable.AddTail( pINode );
		}
	}
	
	// Recreate "Embedded" list
	m_lstEmbedded.RemoveAll();
	nNbrItems = m_lstbxEmbed.GetCount();
	for( nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		pINode = (IDMUSProdNode *)m_lstbxEmbed.GetItemDataPtr( nPos );
	
		if( pINode
		&&  pINode != (IDMUSProdNode *)0xFFFFFFFF )
		{
			// Place item in "Embedded" list
			m_lstEmbedded.AddTail( pINode );
		}
	}
	
	// Recreate "Referenced" list
	m_lstReferenced.RemoveAll();
	nNbrItems = m_lstbxReference.GetCount();
	for( nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		pINode = (IDMUSProdNode *)m_lstbxReference.GetItemDataPtr( nPos );
	
		if( pINode
		&&  pINode != (IDMUSProdNode *)0xFFFFFFFF )
		{
			// Place item in "Referenced" list
			m_lstReferenced.AddTail( pINode );
		}
	}
	
	CDialog::OnOK();
}
