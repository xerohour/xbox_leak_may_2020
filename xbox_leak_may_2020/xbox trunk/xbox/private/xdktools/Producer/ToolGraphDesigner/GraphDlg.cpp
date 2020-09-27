// GraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Graph.h"
#include "GraphCtl.h"
#include "GraphDlg.h"
#include <PChannelName.h>
#include "DlgAddPChannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPChannelHeaderBtn

CPChannelHeaderBtn::CPChannelHeaderBtn()
{
	m_pGraphDlg = NULL;
}

CPChannelHeaderBtn::~CPChannelHeaderBtn()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelHeaderBtn message handlers

BEGIN_MESSAGE_MAP(CPChannelHeaderBtn, CButton)
	//{{AFX_MSG_MAP(CPChannelHeaderBtn)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPChannelHeaderBtn::OnSetFocus

void CPChannelHeaderBtn::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CButton::OnSetFocus( pOldWnd );
	
	m_pGraphDlg->m_nEditMenuCtrlID = IDC_PCHANNEL_HEADER;
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelHeaderBtn::OnEraseBkgnd

BOOL CPChannelHeaderBtn::OnEraseBkgnd( CDC* pDC ) 
{
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelHeaderBtn::DrawItem

void CPChannelHeaderBtn::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CRect rect( lpDrawItemStruct->rcItem );

	CString strPChannel;
	strPChannel.LoadString( IDS_PCHANNEL_GROUP_TEXT );

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC )
	{
		CBrush brush;
		if( brush.CreateSolidBrush( GetSysColor(COLOR_BTNFACE) ) )
		{
			CBrush* pOldBrush = pDC->SelectObject( &brush );
	        int nBkModeOld = pDC->SetBkMode( TRANSPARENT );

			// PChannel column
			pDC->Draw3dRect( &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW) );
			rect.InflateRect( -1, -1 );
			pDC->FillRect( &rect, &brush );
			rect.InflateRect( 1, 1 );
			rect.left += 2;
			rect.right -= 3;
			pDC->DrawText( strPChannel, -1, &rect, (DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX) );
			
			pDC->SelectObject( pOldBrush );
	        pDC->SetBkMode( nBkModeOld );
		}		
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox

CPChannelListBox::CPChannelListBox()
{
	m_pGraphDlg = NULL;
}

CPChannelListBox::~CPChannelListBox()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox message handlers

BEGIN_MESSAGE_MAP(CPChannelListBox, CListBox)
	//{{AFX_MSG_MAP(CPChannelListBox)
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClk)
	ON_WM_VKEYTOITEM_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox::OnSetFocus

void CPChannelListBox::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnSetFocus( pOldWnd );
	
	m_pGraphDlg->m_nEditMenuCtrlID = IDC_PCHANNEL_LIST;
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox::OnKillFocus

void CPChannelListBox::OnKillFocus( CWnd* pNewWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnKillFocus( pNewWnd );
	
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox::DrawItem

void CPChannelListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( lpDrawItemStruct->itemID == -1 )
	{
		return;
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
		CPChannelGroup* pPChannelGroup = (CPChannelGroup *)lpDrawItemStruct->itemData; 
		ASSERT( pPChannelGroup != NULL );

		// Determine text
		CString strText;
		pPChannelGroup->FormatPChannelText( strText );

	    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
		int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

		CRect rect( 0, 0, nWidth, nHeight );

		CDC dc;
		CBitmap bmp;

		if( dc.CreateCompatibleDC( pDC )
		&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
		{
			CFont* pFontOld = NULL;
			CFont* pFont = m_pGraphDlg->CreateFont();
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
			if( sizeText.cx > GetHorizontalExtent() )
			{
				SetHorizontalExtent( sizeText.cx );
			}

			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				if( ::GetFocus() == GetSafeHwnd() )
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
					dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}
				else
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_INACTIVECAPTION) );
					dc.SetTextColor( ::GetSysColor(COLOR_INACTIVECAPTIONTEXT) );
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

		if( ::GetFocus() == GetSafeHwnd() )
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
// CPChannelListBox::MeasureItem

void CPChannelListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDC* pDC = GetDC();
	if( pDC )
	{
		CFont* pFontOld = NULL;
		CFont* pFont = m_pGraphDlg->CreateFont();
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
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox::OnRButtonDown

void CPChannelListBox::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get nearest item
	BOOL fOutside;
	int nPos = ItemFromPoint( point, fOutside );

	if( fOutside == FALSE
	&&  nPos != LB_ERR )
	{
		CRect rect;
		GetItemRect( nPos, &rect );
		if( rect.PtInRect( point ) )
		{
			if( GetSel( nPos ) == 0 )
			{
				// Cursor is in the nearest item
				SetSel( -1, FALSE ); 
				SetSel( nPos, TRUE ); 
			}
			SetCaretIndex( nPos, 0 ); 
		}
	}

	CListBox::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox::OnDblClk

void CPChannelListBox::OnDblClk() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPoint pt;
	GetCursorPos( &pt );

	m_pGraphDlg->OnChangePChannelGroup( pt, false );
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox::VKeyToItem

int CPChannelListBox::VKeyToItem( UINT nKey, UINT nIndex ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nKey )
	{
		case VK_INSERT :
		{
			POINT pt = {0, 0};
			m_pGraphDlg->OnChangePChannelGroup( pt, true );
			return -2;
		}

		case VK_RETURN :
		{
			RECT rect;
			if( GetItemRect(nIndex, &rect) != LB_ERR )
			{
				ClientToScreen( &rect );
				POINT pt = {rect.left, rect.top};
				m_pGraphDlg->OnChangePChannelGroup( pt, false );
			}
			return -2;
		}

		case VK_DELETE :
			m_pGraphDlg->OnDeletePChannelGroups();
			return -2;
	}
	
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn

CGraphHeaderBtn::CGraphHeaderBtn()
{
	m_pGraphDlg = NULL;
	m_nLastXPos = INT_MAX;
}

CGraphHeaderBtn::~CGraphHeaderBtn()
{
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::CreateDataObject

HRESULT CGraphHeaderBtn::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraphDlg->m_pGraph != NULL );
	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}
	*ppIDataObject = NULL;

	HRESULT hr = E_FAIL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Save selected tools in Graph
	IStream* pIStream;
	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_SaveSelectedTools, &pIStream) ) )
	{
		if( SUCCEEDED ( m_pGraphDlg->m_pGraph->Save( pIStream, FALSE ) ) )
		{
			// Place CF_GRAPH into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pGraphComponent->m_cfGraph, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->SaveClipFormat( theApp.m_pGraphComponent->m_cfProducerFile, m_pGraphDlg->m_pGraph, &pIStream ) ) )
		{
			// Graph nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pGraphComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			RELEASE( pIStream );
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn message handlers

BEGIN_MESSAGE_MAP(CGraphHeaderBtn, CButton)
	//{{AFX_MSG_MAP(CGraphHeaderBtn)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnSetFocus

void CGraphHeaderBtn::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CButton::OnSetFocus( pOldWnd );
	
	m_pGraphDlg->m_nEditMenuCtrlID = IDC_GRAPH_HEADER;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnEraseBkgnd

BOOL CGraphHeaderBtn::OnEraseBkgnd( CDC* pDC ) 
{
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::DrawItem

void CGraphHeaderBtn::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CRect rect( lpDrawItemStruct->rcItem );

	// Account for horizontal scrolling
	rect.left = -m_pGraphDlg->m_lstbxGraph.m_nHScrollPos;

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC )
	{
		CString strName;
		CBrush brush;
		if( brush.CreateSolidBrush( GetSysColor(COLOR_BTNFACE) ) )
		{
			CBrush* pOldBrush = pDC->SelectObject( &brush );
	        int nBkModeOld = pDC->SetBkMode( TRANSPARENT );

			CTool* pTool;
			for( int i = 0 ;  pTool = m_pGraphDlg->m_pGraph->MyEnumTools(i) ;  i++ )
			{
				if( pTool == NULL )
				{
					break;
				}

				rect.right = min( (rect.left + pTool->GetColumnWidth()), lpDrawItemStruct->rcItem.right );
				if( rect.right >= 0 )
				{
					if( pTool->m_fSelected )
					{
						pDC->Draw3dRect( &rect, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT) );
					}
					else
					{
						pDC->Draw3dRect( &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW) );
					}
					rect.InflateRect( -1, -1 );
					pDC->FillRect( &rect, &brush );
					rect.InflateRect( 1, 1 );
					rect.left += 2;
					rect.right -= 3;

					if( pTool->m_fSelected )
					{
						rect.OffsetRect( 1, 1 );
					}
					pTool->GetName( strName );
					pDC->DrawText( strName, -1, &rect, (DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX) );
					if( pTool->m_fSelected )
					{
						rect.OffsetRect( -1, -1 );
					}
					rect.right += 3;
				}

				rect.left = rect.right;
				rect.right = lpDrawItemStruct->rcItem.right;
				if( rect.left >= rect.right )
				{
					break;
				}
			}

			if( rect.left < rect.right )
			{
				pDC->Draw3dRect( &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW) );
				rect.InflateRect( -1, -1 );
				pDC->FillRect( &rect, &brush );
				rect.InflateRect( 1, 1 );

				if( m_pGraphDlg->m_pGraph->MyEnumTools(0) == NULL )
				{
					CString strPrompt;
					strPrompt.LoadString( IDS_NO_TOOLS_PROMPT );
					rect.left += 2;
					pDC->DrawText( strPrompt, -1, &rect, (DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX) );
					rect.left -= 2;
				}
			}
			
			pDC->SelectObject( pOldBrush );
	        pDC->SetBkMode( nBkModeOld );
		}		
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnCaptureChanged

void CGraphHeaderBtn::OnCaptureChanged( CWnd *pWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pWnd == this )
	{
		return;
	}

	m_pGraphDlg->m_dwMouseDownButton = 0;
	m_pGraphDlg->m_rectMouseDown.SetRectEmpty();

	if( m_pGraphDlg->m_pToolToToggle )
	{
		// Nothing to do
		m_pGraphDlg->m_pToolToToggle = NULL;
	}
	
	CButton::OnCaptureChanged( pWnd );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnLButtonDblClk

void CGraphHeaderBtn::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	OnLButtonDown( nFlags, point );

	// Get nearest tool
	CTool* pTool = m_pGraphDlg->GetToolFromXPos( point.x );
	if( pTool )
	{
		// Show the Tool properties
		pTool->OnShowProperties();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnLButtonDown

void CGraphHeaderBtn::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraphDlg != NULL );

	SetFocus();

	m_pGraphDlg->m_pToolToToggle = NULL;
	m_nLastXPos = point.x;

	if( m_pGraphDlg->m_dwMouseDownButton == 0 )
	{
		// Get nearest tool
		CTool* pTool = m_pGraphDlg->GetToolFromXPos( point.x );
		if( pTool )
		{
			// SHIFT
			if( nFlags & MK_SHIFT )
			{
				// Figure out range
				int nStartIndex;
				int nEndIndex;
				if(m_pGraphDlg-> m_pToolForShiftClick == NULL )
				{
					nStartIndex = 0;
				}
				else
				{
					nStartIndex = m_pGraphDlg->m_pGraph->ToolToIndex( m_pGraphDlg->m_pToolForShiftClick );
				}
				nEndIndex = m_pGraphDlg->m_pGraph->ToolToIndex( pTool );
				if( nStartIndex > nEndIndex )
				{
					int nTempIndex = nStartIndex;
					nStartIndex = nEndIndex;
					nEndIndex = nTempIndex;
				}

				// Select Tools in range
				m_pGraphDlg->m_pGraph->SelectToolsInRange( nStartIndex, nEndIndex );
			}
			else
			{
				// CTRL
				if( nFlags & MK_CONTROL )
				{
					if( pTool->m_fSelected )
					{
						m_pGraphDlg->m_pToolToToggle = pTool;
					}
					else
					{
						pTool->m_fSelected = TRUE;
					}
				}
				// No CTRL or SHIFT
				else
				{
					m_pGraphDlg->m_pGraph->UnselectAllTools();
					pTool->m_fSelected = TRUE;
				}

				// Set anchor for future shift-select operations
				m_pGraphDlg->m_pToolForShiftClick = pTool;
			}
			ASSERT( pTool->m_fSelected );

			// First set capture
			SetCapture();

			// Now store fields used when drag drop is initiated
			m_pGraphDlg->m_dwMouseDownButton = MK_LBUTTON;

			m_pGraphDlg->m_pointMouseDown.x = point.x;
			m_pGraphDlg->m_pointMouseDown.y = point.y;

			m_pGraphDlg->m_rectMouseDown.left = point.x - 5;
			m_pGraphDlg->m_rectMouseDown.top = point.y - 5;
			m_pGraphDlg->m_rectMouseDown.right = point.x + 5;
			m_pGraphDlg->m_rectMouseDown.bottom = point.y + 5;

			// Redraw header control
			Invalidate();
		}

		// Refresh properties
		m_pGraphDlg->OnViewProperties();
	}
	
	CButton::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnLButtonUp

void CGraphHeaderBtn::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraphDlg->m_dwMouseDownButton == MK_LBUTTON )
	{
		// Do this before we release capture
		if( m_pGraphDlg->m_pToolToToggle )
		{
			m_pGraphDlg->m_pToolToToggle->m_fSelected = !m_pGraphDlg->m_pToolToToggle->m_fSelected;
			m_pGraphDlg->m_pToolToToggle = NULL;
			Invalidate();
		}

		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}

		// Refresh properties
		m_pGraphDlg->OnViewProperties();
	}
	
	CButton::OnLButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnRButtonDown

void CGraphHeaderBtn::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraphDlg != NULL );

	m_pGraphDlg->m_pToolForShiftClick = NULL;
	m_nLastXPos = point.x;

	SetFocus();

	if( m_pGraphDlg->m_dwMouseDownButton == 0 )
	{
		// Get nearest tool
		CTool* pTool = m_pGraphDlg->GetToolFromXPos( point.x );
		if( pTool )
		{
			// Select tool
			if( !(nFlags & MK_CONTROL) )
			{
				if( pTool->m_fSelected == FALSE )
				{
					m_pGraphDlg->m_pGraph->UnselectAllTools();
					pTool->m_fSelected = TRUE;
				}
				ASSERT( pTool->m_fSelected );

				// Set anchor for future shift-select operations
				m_pGraphDlg->m_pToolForShiftClick = pTool;
			}

			if( pTool->m_fSelected )
			{
				// First set capture
				SetCapture();

				// Now store fields used when drag drop is initiated
				m_pGraphDlg->m_dwMouseDownButton = MK_RBUTTON;

				m_pGraphDlg->m_pointMouseDown.x = point.x;
				m_pGraphDlg->m_pointMouseDown.y = point.y;

				m_pGraphDlg->m_rectMouseDown.left = point.x - 5;
				m_pGraphDlg->m_rectMouseDown.top = point.y - 5;
				m_pGraphDlg->m_rectMouseDown.right = point.x + 5;
				m_pGraphDlg->m_rectMouseDown.bottom = point.y + 5;
			}

			// Redraw header control
			Invalidate();
		}

		// Refresh properties
		m_pGraphDlg->OnViewProperties();
	}
	
	CButton::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnRButtonUp

void CGraphHeaderBtn::OnRButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraphDlg->m_dwMouseDownButton == MK_RBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}

		// Refresh properties
		m_pGraphDlg->OnViewProperties();
	}
	
	CButton::OnRButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnMouseMove

void CGraphHeaderBtn::OnMouseMove( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER( nFlags );

	ASSERT( m_pGraphDlg != NULL );
	
	if( CWnd::GetCapture() == this )
	{
		if( m_pGraphDlg->m_dwMouseDownButton != 0 )
		{
			// See if user is moving the mouse
			if( m_pGraphDlg->m_rectMouseDown.PtInRect( point ) == FALSE )
			{
				// Start drag drop
				m_pGraphDlg->DoDrag( IDC_GRAPH_HEADER, m_pGraphDlg->m_dwMouseDownButton ); 
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn::OnKeyDown

void CGraphHeaderBtn::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( nRepCnt == 1 )
	{
		switch( nChar )
		{
			case VK_DELETE :
				m_pGraphDlg->OnDeleteTools();
				break;
		}
	}
	
	CButton::OnKeyDown( nChar, nRepCnt, nFlags );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CGraphListBox

CGraphListBox::CGraphListBox()
{
	m_pGraphDlg = NULL;

	m_nHScrollPos = 0;
	m_nVScrollPos = 0;

	m_nSetPChannelState = -1;
}

CGraphListBox::~CGraphListBox()
{
}


BEGIN_MESSAGE_MAP(CGraphListBox, CListBox)
	//{{AFX_MSG_MAP(CGraphListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphListBox::OnSetFocus

void CGraphListBox::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnSetFocus( pOldWnd );
	
	m_pGraphDlg->m_nEditMenuCtrlID = IDC_GRAPH_LIST;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphListBox::DrawItem

void CGraphListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( lpDrawItemStruct->itemID == -1 )
	{
		return;
	}

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC == NULL )
	{
		return;
	}

	// Handle HScroll
	CPoint pt = pDC->GetWindowOrg();
	if( pt.x != m_nHScrollPos )
	{
		m_nHScrollPos = pt.x;
		m_pGraphDlg->m_btnGraphHeader.Invalidate();	
	}

	// Handle VScroll
	int nTopIndex = GetTopIndex();
	if( nTopIndex != m_nVScrollPos )
	{
		m_nVScrollPos = nTopIndex;
		m_pGraphDlg->m_lstbxPChannels.SetTopIndex( nTopIndex );
	}

	CPen pen;
	if( pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) == FALSE )
	{
		return;
	}

    if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
    ||  lpDrawItemStruct->itemAction & ODA_SELECT 
    ||  lpDrawItemStruct->itemAction & ODA_FOCUS )
	{
		CPChannelGroup* pPChannelGroup = (CPChannelGroup *)lpDrawItemStruct->itemData; 
		ASSERT( pPChannelGroup != NULL );

		CRect rect( lpDrawItemStruct->rcItem );

		// Draw the item
		{
			CPen* pPenOld = pDC->SelectObject( &pen );

			pDC->FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );

			rect.bottom--;

			CTool* pTool;
			int nXPos = 0;
			for( int i = 0 ;  pTool = m_pGraphDlg->m_pGraph->MyEnumTools(i) ;  i++ )
			{
				if( pTool == NULL )
				{
					break;
				}

				// Determine rectangle
				rect.left = nXPos;
				nXPos += pTool->GetColumnWidth();
				rect.right = nXPos;

				// Draw horizontal line (bottom)
				pDC->MoveTo( rect.left, rect.bottom );
				pDC->LineTo( rect.right, rect.bottom );

				// Draw vertical lines (separate tools)
				pDC->MoveTo( rect.right-1, rect.top );
				pDC->LineTo( rect.right-1, rect.bottom );

				if( pTool->ContainsPChannelGroup(pPChannelGroup) )
				{
					InflateRect( &rect, -1, -1 );
					rect.right--;
					pDC->FillSolidRect( &rect, pDC->GetNearestColor(RGB(10,10,255)) );
					rect.right++;
					InflateRect( &rect, 1, 1 );
				}
			}

			if( pPenOld )
			{
				pDC->SelectObject( pPenOld );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphListBox::MeasureItem

void CGraphListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDC* pDC = GetDC();
	if( pDC )
	{
		CFont* pFontOld = NULL;
		CFont* pFont = m_pGraphDlg->CreateFont();
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
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphListBox::OnMouseMove

void CGraphListBox::OnMouseMove( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER( nFlags );

	ASSERT( m_pGraphDlg != NULL );
	
	if( CWnd::GetCapture() == this )
	{
		if( m_nSetPChannelState != -1 )
		{
			// Get nearest item
			BOOL fOutside;
			int nPos = ItemFromPoint( point, fOutside );
			if( fOutside == FALSE
			&&  nPos != LB_ERR )
			{
				// Get PChannelGroup
				CPChannelGroup* pPChannelGroup = (CPChannelGroup *)m_pGraphDlg->m_lstbxPChannels.GetItemDataPtr( nPos );
				ASSERT( pPChannelGroup != NULL );
				ASSERT( pPChannelGroup != (CPChannelGroup *)0xFFFFFFFF );

				// Highlight corresponding item in PChannel listbox
				m_pGraphDlg->m_lstbxPChannels.SetSel( -1, FALSE ); 
				m_pGraphDlg->m_lstbxPChannels.SetSel( nPos, TRUE ); 
				m_pGraphDlg->m_lstbxPChannels.SetCaretIndex( nPos, 0 ); 

				// Get nearest tool
				CTool* pTool = m_pGraphDlg->GetToolFromXPos( point.x );
				if( pTool )
				{
					// Set m_nSetPChannelState
					if( pTool->ContainsPChannelGroup( pPChannelGroup ) )
					{
						if( m_nSetPChannelState == FALSE )
						{
							pTool->RemovePChannelGroup( pPChannelGroup );

							// Redraw graph
							RECT rect;
							GetItemRect( nPos, &rect );
							InvalidateRect( &rect, FALSE );
						}
					}
					else
					{
						if( m_nSetPChannelState == TRUE )
						{
							pTool->InsertPChannelGroup( pPChannelGroup );

							// Redraw graph
							RECT rect;
							GetItemRect( nPos, &rect );
							InvalidateRect( &rect, FALSE );
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphListBox::OnLButtonDown

void CGraphListBox::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraphDlg != NULL );

	// Get nearest item
	BOOL fOutside;
	int nPos = ItemFromPoint( point, fOutside );
	if( fOutside == FALSE
	&&  nPos != LB_ERR )
	{
		// Get PChannelGroup
		CPChannelGroup* pPChannelGroup = (CPChannelGroup *)m_pGraphDlg->m_lstbxPChannels.GetItemDataPtr( nPos );
		ASSERT( pPChannelGroup != NULL );
		ASSERT( pPChannelGroup != (CPChannelGroup *)0xFFFFFFFF );

		// Highlight corresponding item in PChannel listbox
		m_pGraphDlg->m_lstbxPChannels.SetSel( -1, FALSE ); 
		m_pGraphDlg->m_lstbxPChannels.SetSel( nPos, TRUE ); 
		m_pGraphDlg->m_lstbxPChannels.SetCaretIndex( nPos, 0 ); 

		// Get nearest tool
		CTool* pTool = m_pGraphDlg->GetToolFromXPos( point.x );
		if( pTool )
		{
			// First set capture
			SetCapture();

			// Save undo state
			m_pGraphDlg->m_pGraph->m_pUndoMgr->SaveState( m_pGraphDlg->m_pGraph, theApp.m_hInstance, IDS_UNDO_CHANGE_PCHANNELS );

			// Set m_nSetPChannelState
			if( pTool->ContainsPChannelGroup( pPChannelGroup ) )
			{
				m_nSetPChannelState = FALSE;
				pTool->RemovePChannelGroup( pPChannelGroup );
			}
			else
			{
				m_nSetPChannelState = TRUE;
				pTool->InsertPChannelGroup( pPChannelGroup );
			}

			// Redraw graph
			RECT rect;
			GetItemRect( nPos, &rect );
			InvalidateRect( &rect, FALSE );
		}
	}

	CListBox::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphListBox::OnLButtonDblClk

void CGraphListBox::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphListBox::OnLButtonUp

void CGraphListBox::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( CWnd::GetCapture() == this )
	{
		::ReleaseCapture();

		m_pGraphDlg->m_pGraph->SetModified( TRUE );
		m_pGraphDlg->m_pGraph->SyncGraphWithDirectMusic();
	}
	
	m_nSetPChannelState = -1;

	CListBox::OnLButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CToolHeaderBtn

CToolHeaderBtn::CToolHeaderBtn()
{
	m_pGraphDlg = NULL;
}

CToolHeaderBtn::~CToolHeaderBtn()
{
}


/////////////////////////////////////////////////////////////////////////////
// CToolHeaderBtn message handlers

BEGIN_MESSAGE_MAP(CToolHeaderBtn, CButton)
	//{{AFX_MSG_MAP(CToolHeaderBtn)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolHeaderBtn::OnSetFocus

void CToolHeaderBtn::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CButton::OnSetFocus( pOldWnd );
	
	m_pGraphDlg->m_nEditMenuCtrlID = IDC_TOOL_HEADER;
}


/////////////////////////////////////////////////////////////////////////////
// CToolHeaderBtn::OnEraseBkgnd

BOOL CToolHeaderBtn::OnEraseBkgnd( CDC* pDC ) 
{
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CToolHeaderBtn::DrawItem

void CToolHeaderBtn::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC )
	{
		CRect rect( lpDrawItemStruct->rcItem );
		CBrush brush;

		pDC->Draw3dRect( &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW) );

		if( brush.CreateSolidBrush( GetSysColor(COLOR_BTNFACE) ) )
		{
			CBrush* pOldBrush = pDC->SelectObject( &brush );
			rect.InflateRect( -1, -1 );
			pDC->FillRect( &rect, &brush );
			pDC->SelectObject( pOldBrush );
		}		

		CString strTools;
		strTools.LoadString( IDS_TOOL_PALETTE_TEXT );
		rect.left += 2;
        int nBkModeOld = pDC->SetBkMode( TRANSPARENT );
        pDC->DrawText( strTools, -1, &rect, (DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX) );
        pDC->SetBkMode( nBkModeOld );
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CToolListBox

CToolListBox::CToolListBox()
{
	m_pGraphDlg = NULL;
}

CToolListBox::~CToolListBox()
{
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::CreateDataObject

HRESULT CToolListBox::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIGraphDocType8 != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}
	*ppIDataObject = NULL;

	HRESULT hr = E_FAIL;

	CDirectMusicGraph* pTempGraph = NULL;
	IDMUSProdNode* pITempGraphNode = NULL;

	// Create temporary graph and insert selected tool
	int nPos = GetCurSel();
	if( nPos != LB_ERR )
	{
		RegisteredTool* pRegisteredTool = (RegisteredTool *)GetItemDataPtr( nPos );
		
		if( pRegisteredTool != NULL 
		&&  pRegisteredTool != (RegisteredTool *)0xFFFFFFFF )
		{
			IDirectMusicTool* pIDMTool;
			if( SUCCEEDED ( ::CoCreateInstance( pRegisteredTool->clsidTool, NULL, CLSCTX_INPROC_SERVER,
												IID_IDirectMusicTool, (void**)&pIDMTool ) ) )
			{
				if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIGraphDocType8->AllocNode( GUID_ToolGraphNode, &pITempGraphNode ) ) )
				{
					pTempGraph = (CDirectMusicGraph *)pITempGraphNode;

					// Set root and parent node of ALL children
					theApp.SetNodePointers( pITempGraphNode, pITempGraphNode, NULL );

					CTool* pTool = new CTool( pRegisteredTool->clsidTool, pIDMTool );
					if( pTool )
					{
						pTool->RefreshStreamOfData();
						pTempGraph->InsertTool( pTool, -1 );
						RELEASE( pTool );
					}
					else
					{
						pTempGraph = NULL;
					}
				}

				RELEASE( pIDMTool );
			}
		}
	}
	if( pTempGraph == NULL )
	{
		RELEASE( pITempGraphNode );
		return E_FAIL;
	}

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		RELEASE( pITempGraphNode );
		return E_OUTOFMEMORY;
	}

	// Save temporary Graph
	IStream* pIStream;
	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		// Make sure we know this tool is from the palette 
		pTempGraph->m_GraphUI.dwFlagsUI |= GRAPHUI_FROM_TOOL_PALETTE;
		
		if( SUCCEEDED ( pTempGraph->Save( pIStream, FALSE ) ) )
		{
			// Place CF_GRAPH into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pGraphComponent->m_cfGraph, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->SaveClipFormat( theApp.m_pGraphComponent->m_cfProducerFile, pITempGraphNode, &pIStream ) ) )
		{
			// Graph nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pGraphComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			RELEASE( pIStream );
		}
	}

	// Delete temporary graph and selected tool
	RELEASE( pITempGraphNode );

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox message handlers

BEGIN_MESSAGE_MAP(CToolListBox, CListBox)
	//{{AFX_MSG_MAP(CToolListBox)
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::OnSetFocus

void CToolListBox::OnSetFocus( CWnd* pOldWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CListBox::OnSetFocus( pOldWnd );
	
	m_pGraphDlg->m_nEditMenuCtrlID = IDC_TOOL_LIST;
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::DrawItem

void CToolListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( lpDrawItemStruct->itemID == -1 )
	{
		return;
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
		RegisteredTool* pRegisteredTool = (RegisteredTool *)lpDrawItemStruct->itemData; 
		ASSERT( pRegisteredTool != NULL );

	    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
		int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

		CRect rect( 0, 0, nWidth, nHeight );

		CDC dc;
		CBitmap bmp;

		if( dc.CreateCompatibleDC( pDC )
		&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
		{
			CFont* pFontOld = NULL;
			CFont* pFont = m_pGraphDlg->CreateFont();
			if( pFont )
			{
				pFontOld = dc.SelectObject( pFont );
			}
			CBitmap* pBitmapOld = dc.SelectObject( &bmp );
			int nBkModeOld = dc.SetBkMode( TRANSPARENT );

			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				if( ::GetFocus() == GetSafeHwnd() )
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_HIGHLIGHT) );
					dc.SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}
				else
				{
					dc.FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );
					dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
				}
			}
			else
			{
				dc.FillSolidRect( &rect, ::GetSysColor(COLOR_WINDOW) );
				dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
			}

			// Draw the item
			rect.left  += 3;
			dc.DrawText( pRegisteredTool->strName, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );
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

		if( ::GetFocus() == GetSafeHwnd() )
		{
			if( (lpDrawItemStruct->itemAction & ODA_FOCUS)
			&&  (lpDrawItemStruct->itemState & ODS_SELECTED) )
			{
				InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
				InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::MeasureItem

void CToolListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDC* pDC = GetDC();
	if( pDC )
	{
		CFont* pFontOld = NULL;
		CFont* pFont = m_pGraphDlg->CreateFont();
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
	}
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::OnMouseMove

void CToolListBox::OnMouseMove( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER( nFlags );

	ASSERT( m_pGraphDlg != NULL );
	
	if( CWnd::GetCapture() == this )
	{
		if( m_pGraphDlg->m_dwMouseDownButton != 0 )
		{
			// See if user is moving the mouse
			if( m_pGraphDlg->m_rectMouseDown.PtInRect( point ) == FALSE )
			{
				// Start drag drop
				m_pGraphDlg->DoDrag( IDC_TOOL_LIST, m_pGraphDlg->m_dwMouseDownButton ); 
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::OnCaptureChanged

void CToolListBox::OnCaptureChanged( CWnd *pWnd ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pWnd == this )
	{
		return;
	}

	m_pGraphDlg->m_dwMouseDownButton = 0;
	m_pGraphDlg->m_rectMouseDown.SetRectEmpty();

	
	CListBox::OnCaptureChanged( pWnd );
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::OnLButtonDown

void CToolListBox::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraphDlg != NULL );

	if( m_pGraphDlg->m_dwMouseDownButton == 0 )
	{
		BOOL fOutside;

		// Get nearest item
		int nPos = ItemFromPoint( point, fOutside );
		if( fOutside == FALSE
		&&  nPos != LB_ERR )
		{ 
			CRect rect;

			GetItemRect( nPos, &rect );
			if( rect.PtInRect( point ) )
			{
				// Cursor is in the nearest item
				SetCurSel( nPos );

				// First set capture
				SetCapture();

				// Now store fields used when drag drop is initiated
				m_pGraphDlg->m_dwMouseDownButton = MK_LBUTTON;

				m_pGraphDlg->m_pointMouseDown.x = point.x;
				m_pGraphDlg->m_pointMouseDown.y = point.y;

				m_pGraphDlg->m_rectMouseDown.left = point.x - 5;
				m_pGraphDlg->m_rectMouseDown.top = point.y - 5;
				m_pGraphDlg->m_rectMouseDown.right = point.x + 5;
				m_pGraphDlg->m_rectMouseDown.bottom = point.y + 5;
			}
		}
	}

	CListBox::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::OnLButtonUp

void CToolListBox::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraphDlg->m_dwMouseDownButton == MK_LBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}
	
	CListBox::OnLButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::OnRButtonDown

void CToolListBox::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraphDlg != NULL );

	if( m_pGraphDlg->m_dwMouseDownButton == 0 )
	{
		BOOL fOutside;

		// Get nearest item
		int nPos = ItemFromPoint( point, fOutside );
		if( fOutside == FALSE
		&&  nPos != LB_ERR )
		{
			CRect rect;

			GetItemRect( nPos, &rect );
			if( rect.PtInRect( point ) )
			{
				// Cursor is in the nearest item
				SetCurSel( nPos ); 

				// First set capture
				SetCapture();

				// Now store fields used when drag drop is initiated
				m_pGraphDlg->m_dwMouseDownButton = MK_RBUTTON;

				m_pGraphDlg->m_pointMouseDown.x = point.x;
				m_pGraphDlg->m_pointMouseDown.y = point.y;

				m_pGraphDlg->m_rectMouseDown.left = point.x - 5;
				m_pGraphDlg->m_rectMouseDown.top = point.y - 5;
				m_pGraphDlg->m_rectMouseDown.right = point.x + 5;
				m_pGraphDlg->m_rectMouseDown.bottom = point.y + 5;
			}
		}
	}

	CListBox::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CToolListBox::OnRButtonUp

void CToolListBox::OnRButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraphDlg->m_dwMouseDownButton == MK_RBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}
	
	CListBox::OnRButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg

//IMPLEMENT_DYNCREATE(CGraphDlg, CFormView)

CGraphDlg::CGraphDlg()
	: CFormView(CGraphDlg::IDD)
{
	//{{AFX_DATA_INIT(CGraphDlg)
	//}}AFX_DATA_INIT

	m_pGraphCtrl = NULL;
	m_pGraph = NULL;

	m_nPChannelSplitterXPos = 0;
	m_nToolSplitterXPos = 0;

	m_nEditMenuCtrlID = 0;

	m_pIDataObject = NULL;
	m_nStartDragControlID = 0;
	m_dwStartDragButton = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_pDragImage = NULL;
	m_pDragGraph = NULL;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_fDragToSameGraph = false;
	m_pointMouseDown.x = 0;
	m_pointMouseDown.y = 0;

	m_pINodeRightMenu = NULL;
	m_pointRightMenu.x = -1;
	m_pointRightMenu.y = -1;

	m_dwMouseDownButton = 0;
	m_pToolForShiftClick = NULL;
	m_pToolToToggle = NULL;
	m_dwScrollTick = 0;
}

CGraphDlg::~CGraphDlg()
{
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::RefreshControls

void CGraphDlg::RefreshControls( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	FillPChannelListBox();
	FillGraphListBox();
	m_btnGraphHeader.Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::ResetContent

void CGraphDlg::ResetContent( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_lstbxPChannels.ResetContent();
	m_lstbxGraph.ResetContent();
	m_btnGraphHeader.Invalidate();
}


void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphDlg, CFormView)
	//{{AFX_MSG_MAP(CGraphDlg)
	ON_COMMAND(IDM_DRAG_MOVE, OnDragRMenuMove)
	ON_COMMAND(IDM_DRAG_COPY, OnDragRMenuCopy)
	ON_COMMAND(IDM_DRAG_CANCEL, OnDragRMenuCancel)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg diagnostics

#ifdef _DEBUG
void CGraphDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CGraphDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// IUknown CGraphDlg::QueryInterface

HRESULT CGraphDlg::QueryInterface( REFIID riid, LPVOID *ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDropSource)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDropSource *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDropTarget) )
    {
        AddRef();
        *ppvObj = (IDropTarget *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown CGraphDlg::AddRef

ULONG CGraphDlg::AddRef( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown CGraphDlg::Release

ULONG CGraphDlg::Release( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    ASSERT( m_dwRef != 0 );	// m_dwRef should never get to zero.

//  if( m_dwRef == 0 )		   CGraphDlg should be deleted when		
//  {						   control is destroyed. 						
//		delete this;		
//		return 0;
//  }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg IDropSource implementation

/////////////////////////////////////////////////////////////////////////////
// IDropSource CGraphDlg::QueryContinueDrag

HRESULT CGraphDlg::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( fEscapePressed )
	{
        return DRAGDROP_S_CANCEL;
	}

	if( m_dwStartDragButton == MK_LBUTTON )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}

		if( !(grfKeyState & MK_LBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	if( m_dwStartDragButton == MK_RBUTTON )
	{
		if( grfKeyState & MK_LBUTTON )
		{
			return DRAGDROP_S_CANCEL;
		}
		
		if( !(grfKeyState & MK_RBUTTON) )
		{
			return DRAGDROP_S_DROP;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropSource CGraphDlg::GiveFeedback

HRESULT CGraphDlg::GiveFeedback( DWORD dwEffect )
{
	UNREFERENCED_PARAMETER( dwEffect );

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pDragImage )
	{
		CPoint pt;

		GetCursorPos( &pt );

		// Move the drag image
		m_pDragImage->DragMove( pt );
	}

	return DRAGDROP_S_USEDEFAULTCURSORS;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg IDropTarget implementation

/////////////////////////////////////////////////////////////////////////////
// IDropTarget CGraphDlg::DragEnter

HRESULT CGraphDlg::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDataObject != NULL );
	ASSERT( m_pIDataObject == NULL );

	m_dwScrollTick = 0;

	// Store IDataObject associated with current drag-drop operation
	m_pIDataObject = pIDataObject;
	m_pIDataObject->AddRef();

	if( m_pDragImage )
	{
		CPoint point( pt.x, pt.y );

		// Show the feedback image
		m_pDragImage->DragEnter( GetDesktopWindow (), point );
	}

	// Make sure editor is on top
	if( m_pGraph->m_hWndEditor )
	{
		::BringWindowToTop( m_pGraph->m_hWndEditor );
	}

	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CGraphDlg::DragOver

HRESULT CGraphDlg::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDataObject != NULL );

	POINT point;
	point.x = pt.x;
	point.y = pt.y;

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	if( CanPasteFromData( m_pIDataObject, true, point ) == S_OK )
	{
		if( grfKeyState & MK_RBUTTON )
		{
			dwEffect = *pdwEffect;
		}
		else
		{
			if( grfKeyState & MK_CONTROL )
			{
				dwEffect = DROPEFFECT_COPY;
			}
			else
			{
				if( *pdwEffect & DROPEFFECT_COPY
				&&  *pdwEffect & DROPEFFECT_MOVE )
				{
					dwEffect = DROPEFFECT_MOVE;
				}
				else
				{
					dwEffect = *pdwEffect;
				}
			}
		}
	}

	// Scroll?
	{
		RECT rect;
		m_btnGraphHeader.ScreenToClient( &point );
		m_btnGraphHeader.GetClientRect( &rect );

		if( point.x < 0
		||  point.x > rect.right )
		{
			if( m_dwScrollTick == 0 )
			{
				m_dwScrollTick = GetTickCount() + 350;
			}
			else if( m_dwScrollTick < GetTickCount() )
			{
				if( point.x < 0 )
				{
					m_lstbxGraph.SendMessage( WM_HSCROLL,
											  MAKEWPARAM( SB_THUMBPOSITION, max(0, m_lstbxGraph.m_nHScrollPos - 30) ),
											  NULL );
				}
				else
				{
					int nExtent = m_lstbxGraph.GetHorizontalExtent();
					m_lstbxGraph.SendMessage( WM_HSCROLL,
											  MAKEWPARAM( SB_THUMBPOSITION, min(nExtent, m_lstbxGraph.m_nHScrollPos + 30) ),
											  NULL );
				}

				m_dwScrollTick += 100;
			}
			
			dwEffect |= DROPEFFECT_SCROLL;
		}
		else
		{
			m_dwScrollTick = 0;
		}

		m_btnGraphHeader.ClientToScreen( &point );
	}

	if( m_pDragImage )
	{
		// Show the feedback image
		m_pDragImage->DragShowNolock( TRUE );
	}

	// Set temp drag over fields
	if( grfKeyState & (MK_RBUTTON | MK_LBUTTON) )
	{
		m_dwOverDragButton = grfKeyState & (MK_RBUTTON | MK_LBUTTON);
		m_dwOverDragEffect = dwEffect;
	}

	*pdwEffect = dwEffect;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CGraphDlg::DragLeave

HRESULT CGraphDlg::DragLeave( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_dwScrollTick = 0;

	// Release IDataObject
	RELEASE( m_pIDataObject );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	//Reset temp drag over fields
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CGraphDlg::Drop

HRESULT CGraphDlg::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	UNREFERENCED_PARAMETER( grfKeyState );

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDataObject != NULL );
	ASSERT( m_pIDataObject == pIDataObject );
	ASSERT( m_pGraph != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	// Set default values
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

	POINT point;
	point.x = pt.x;
	point.y = pt.y;
	if( m_pDragGraph == m_pGraph )
	{
		if( m_nStartDragControlID == IDC_GRAPH_HEADER )
		{
			CTool* pToolStart = GetToolFromXPos( m_pointMouseDown.x );
			if( pToolStart )
			{
				m_btnGraphHeader.ScreenToClient( &point );
				CTool* pToolEnd = GetToolFromXPos( point.x );
				m_btnGraphHeader.ClientToScreen( &point );

				if( pToolStart == pToolEnd )
				{
					// Nothing to do
					DragLeave();
					return hr;
				}
			}
		}
	}

	// Choose effect when right mouse drag - Move, Copy, or Cancel?
	if( m_dwOverDragButton & MK_RBUTTON )
	{
		HMENU hMenu;
		HMENU hMenuPopup;
		
		// Display arrow cursor
		::LoadCursor( theApp.m_hInstance, IDC_ARROW );
	
		// Prepare context menu
		hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_DRAG_RMENU) );
		if( hMenu )
		{
			m_dwDragRMenuEffect = DROPEFFECT_NONE;

			// Track right context menu for drag-drop via TrackPopupMenu
			hMenuPopup = ::GetSubMenu( hMenu, 0 );

			// Init state of menu items
			if( !(m_dwOverDragEffect & DROPEFFECT_MOVE) )
			{
				::EnableMenuItem( hMenuPopup, IDM_DRAG_MOVE, (MF_GRAYED | MF_BYCOMMAND) );
			}

			// Display and track menu
			CPoint point( pt.x, pt.y );
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			DestroyMenu( hMenu );

			// Need to process WM_COMMAND from TrackPopupMenu
			MSG msg;
			while( ::PeekMessage( &msg, GetSafeHwnd(), NULL, NULL, PM_REMOVE) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			// WM_COMMAND from TrackPopupMenu will have set m_dwDragRMenuEffect
			m_dwOverDragEffect = m_dwDragRMenuEffect;
			m_dwDragRMenuEffect = DROPEFFECT_NONE;
		}
	}

	if( m_dwOverDragEffect != DROPEFFECT_NONE )
	{
		// Paste data
		if( m_pDragGraph == m_pGraph )
		{
			m_fDragToSameGraph = true;
		}
		if( m_dwOverDragEffect == DROPEFFECT_MOVE 
		&&  m_fDragToSameGraph )
		{
			m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_MOVE_TOOLS );
			m_pGraph->DeleteMarkedTools( UD_DRAGSELECT );
		}
		else
		{
			m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_INSERT_TOOLS );
		}
		hr = PasteFromData( pIDataObject, true, point );
		if( SUCCEEDED ( hr ) )
		{
			*pdwEffect = m_dwOverDragEffect;
		}

		m_pGraph->SetModified( TRUE );
		if( m_fDragToSameGraph == false )
		{
			m_pGraph->Refresh();
		}

		// Refresh properties
		OnViewProperties();
	}

	// Cleanup
	DragLeave();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::Create

BOOL CGraphDlg::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !CFormView::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext ) )
	{
		return FALSE;
	}

	// Create the splitter bars
	m_wndPChannelSplitter.Create( this, 3, IDC_PCHANNEL_SPLITTER );
	m_wndToolSplitter.Create( this, 6, IDC_TOOL_SPLITTER );
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnInitialUpdate

void CGraphDlg::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	CFormView::OnInitialUpdate();
	
	HRESULT hr = ::RegisterDragDrop( GetSafeHwnd(), (IDropTarget *)this );
	ASSERT(SUCCEEDED(hr));

	// Subclass controls
	m_btnPChannelHeader.SubclassDlgItem( IDC_PCHANNEL_HEADER, this );
	m_btnPChannelHeader.m_pGraphDlg = this;

	m_lstbxPChannels.SubclassDlgItem( IDC_PCHANNEL_LIST, this );
	m_lstbxPChannels.m_pGraphDlg = this;

	m_btnGraphHeader.SubclassDlgItem( IDC_GRAPH_HEADER, this );
	m_btnGraphHeader.m_pGraphDlg = this;

	m_lstbxGraph.SubclassDlgItem( IDC_GRAPH_LIST, this );
	m_lstbxGraph.m_pGraphDlg = this;

	m_btnToolHeader.SubclassDlgItem( IDC_TOOL_HEADER, this );
	m_btnToolHeader.m_pGraphDlg = this;

	m_lstbxTools.SubclassDlgItem( IDC_TOOL_LIST, this );
	m_lstbxTools.m_pGraphDlg = this;

	// Populate controls
	FillToolsListBox();
	FillPChannelListBox();
	FillGraphListBox();

	// If the graph has no PChannels, open up the Add/Remove PChannels dialog
	if( m_pGraph
	&&  m_pGraph->m_lstGraphPChannelGroups.IsEmpty() )
	{
		::PostMessage( GetSafeHwnd(), WM_COMMAND, IDM_INSERT_PCHANNEL_GROUP, 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnDestroy

void CGraphDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = ::RevokeDragDrop( GetSafeHwnd() );
	ASSERT( SUCCEEDED( hr ) );

	CFormView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnDragRMenuMove

void CGraphDlg::OnDragRMenuMove() 
{
	m_dwDragRMenuEffect = DROPEFFECT_MOVE;	
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnDragRMenuCopy

void CGraphDlg::OnDragRMenuCopy() 
{
	m_dwDragRMenuEffect = DROPEFFECT_COPY;	
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnDragRMenuCancel

void CGraphDlg::OnDragRMenuCancel() 
{
	m_dwDragRMenuEffect = DROPEFFECT_NONE;	
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnSize

void CGraphDlg::OnSize( UINT nType, int cx, int cy ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow(m_wndPChannelSplitter) )
	{
		return;
	}

	ASSERT( m_pGraph != NULL );

	CRect rect;
	GetClientRect( &rect );

	// Determine tool splitter bar position
	m_nToolSplitterXPos = cx - m_pGraph->m_GraphUI.nRegisteredToolPaneWidth;
	m_nToolSplitterXPos = max( m_wndPChannelSplitter.GetWidth(), m_nToolSplitterXPos ); 
	m_nToolSplitterXPos = min( m_nToolSplitterXPos, (cx - m_wndToolSplitter.GetWidth()) );
	
	// Move the tool splitter bar
    m_wndToolSplitter.MoveWindow( m_nToolSplitterXPos, 0, m_wndToolSplitter.GetWidth(), cy, TRUE );

	// Recalc registered tools pane
	int nX  = m_nToolSplitterXPos + m_wndToolSplitter.GetWidth();
	int nCX = max( 0, (cx - nX) );
	int nCY = max( 0, (cy - GRAPH_HEADER_HEIGHT) );
    m_btnToolHeader.MoveWindow( nX, 0, (nCX + 1), GRAPH_HEADER_HEIGHT, TRUE );
    m_lstbxTools.MoveWindow( nX, GRAPH_HEADER_HEIGHT, nCX, nCY, TRUE );

	// Determine pchannel splitter bar position
	m_nPChannelSplitterXPos = m_pGraph->m_GraphUI.nPChannelColumnWidth;
	m_nPChannelSplitterXPos = max( 0, m_nPChannelSplitterXPos ); 
	m_nPChannelSplitterXPos = min( m_nPChannelSplitterXPos, (m_nToolSplitterXPos - m_wndPChannelSplitter.GetWidth()) );
	
	// Move the pchannel splitter bar
    m_wndPChannelSplitter.MoveWindow( m_nPChannelSplitterXPos, 0, m_wndPChannelSplitter.GetWidth(), cy, TRUE );
	
	// Recalc graph pane
	nCX = min( m_pGraph->m_GraphUI.nPChannelColumnWidth, m_nPChannelSplitterXPos );
    m_btnPChannelHeader.MoveWindow( 0, 0, nCX, GRAPH_HEADER_HEIGHT, TRUE );
    m_lstbxPChannels.MoveWindow( 0, GRAPH_HEADER_HEIGHT, nCX, nCY, TRUE );

	nX = m_nPChannelSplitterXPos + m_wndPChannelSplitter.GetWidth()+ 1;
	nCX = max( 0, (m_nToolSplitterXPos - nX) );
    m_btnGraphHeader.MoveWindow( nX, 0, nCX, GRAPH_HEADER_HEIGHT, TRUE );
    m_lstbxGraph.MoveWindow( nX, GRAPH_HEADER_HEIGHT, nCX, nCY, TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::DoDrag

void CGraphDlg::DoDrag( UINT nID, DWORD dwStartDragButton )
{
	// If already dragging, just return
	if( m_pDragImage )
	{
		return;
	}

	IDataObject* pIDataObject = NULL;
	switch( nID )
	{
		case IDC_TOOL_LIST:
			m_lstbxTools.CreateDataObject( &pIDataObject );
			break;

		case IDC_GRAPH_HEADER:
			m_pGraph->MarkSelectedTools( UD_DRAGSELECT );
			m_btnGraphHeader.CreateDataObject( &pIDataObject );
			break;

		default:
			ASSERT( 0 );
			break;
	}

	if( pIDataObject )
	{
		DWORD dwEffect;
		HRESULT hr;

		// Create image used for drag-drop feedback
		if( m_pDragImage )
		{
			m_pDragImage->BeginDrag( 0, CPoint(8, 12) );
		}

		// Start drag-drop operation
		DWORD dwOKDragEffects = DROPEFFECT_COPY;
		if( nID == IDC_GRAPH_HEADER )
		{
			dwOKDragEffects |= DROPEFFECT_MOVE;
		}

		m_pDragGraph = m_pGraph;
		m_nStartDragControlID = nID;
		m_dwStartDragButton = dwStartDragButton;
		m_fDragToSameGraph = false;
		hr = ::DoDragDrop( pIDataObject, (IDropSource *)this, dwOKDragEffects, &dwEffect );
		m_pDragGraph = NULL;
		m_nStartDragControlID = 0;
		m_dwStartDragButton = 0;
		m_pointMouseDown.x = 0;
		m_pointMouseDown.y = 0;

		// Delete image used for drag-drop feedback
		if( m_pDragImage )
		{
			m_pDragImage->EndDrag();

			delete m_pDragImage;
			m_pDragImage = NULL;
		}

		switch( hr )
		{
			case DRAGDROP_S_DROP:
				if( dwEffect & DROPEFFECT_MOVE )
				{
					ASSERT( nID == IDC_GRAPH_HEADER );
					if( m_fDragToSameGraph == false )
					{
						m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_DELETE_TOOLS );
						m_pGraph->DeleteMarkedTools( UD_DRAGSELECT );
						m_pGraph->SetModified( TRUE );
					}
					m_pGraph->Refresh();
				}
				else if( dwEffect & DROPEFFECT_COPY )
				{
					if( m_fDragToSameGraph == false )
					{
						m_pGraph->UnselectAllTools();
					}
					m_pGraph->Refresh();
				}
				break;
		}

		m_fDragToSameGraph = false;
		m_pGraph->UnMarkTools( UD_DRAGSELECT ); 
			
		RELEASE( pIDataObject );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::CanPasteFromData

HRESULT CGraphDlg::CanPasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( fInDragDrop )
	{
		CRect rect;

		bool fCursorOverGraph = false;

		// Make sure cursor is over Graph
		if( fCursorOverGraph == false )
		{
			m_btnGraphHeader.ScreenToClient( &pt );
			m_btnGraphHeader.GetClientRect( &rect );
			if( rect.PtInRect( pt ) )
			{
				fCursorOverGraph = true;
			}
			m_btnGraphHeader.ClientToScreen( &pt );
		}
		if( fCursorOverGraph == false )
		{
			m_lstbxGraph.ScreenToClient( &pt );
			m_lstbxGraph.GetClientRect( &rect );
			if( rect.PtInRect( pt ) )
			{
				fCursorOverGraph = true;
			}
			m_lstbxGraph.ClientToScreen( &pt );
		}
		if( fCursorOverGraph == false )
		{
			return S_FALSE;
		}
	}

	// Create a new CDllJazzDataObject and see if it contains a Graph
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pGraphComponent->m_cfGraph );

	RELEASE( pDataObject );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::PasteFromData

HRESULT CGraphDlg::PasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( CanPasteFromData( pIDataObject, fInDragDrop, pt ) != S_OK )
	{
		return E_UNEXPECTED;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pGraphComponent->m_cfGraph ) ) )
	{
		// Handle CF_GRAPH format
		hr = PasteCF_GRAPH( pDataObject, pIDataObject, fInDragDrop, pt );
	}

	RELEASE( pDataObject );
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::PasteCF_GRAPH

HRESULT CGraphDlg::PasteCF_GRAPH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	// Unselect all Tools
	m_pGraph->UnselectAllTools();

	// Determine paste index
	int nPasteIndex = -1;
	if( fInDragDrop )
	{
		// Drop
		m_btnGraphHeader.ScreenToClient( &pt );
		CTool* pTool = GetToolFromXPos( pt.x );
		if( pTool )
		{
			nPasteIndex = m_pGraph->ToolToIndex( pTool );
		}
	}
	else
	{
		// Paste
		CTool* pTool = GetToolFromXPos( m_btnGraphHeader.m_nLastXPos );
		if( pTool )
		{
			nPasteIndex = m_pGraph->ToolToIndex( pTool );
		}
	}

	// Determine whether or not source and target graphs are the same
	bool fToolsFromSameGraph = false;
	if( fInDragDrop )
	{
		// Drop
		if( m_pDragGraph == m_pGraph )
		{
			fToolsFromSameGraph = true;
		}
	}
	else
	{
		// Paste
		if( theApp.IsInClipboard( m_pGraph ) )
		{
			fToolsFromSameGraph = true;
		}
	}

	// Get the stream containing the Graph
	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pGraphComponent->m_cfGraph, &pIStream  ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create a temporary Graph and load the stream
		CDirectMusicGraph* pTempGraph = new CDirectMusicGraph;
		if( pTempGraph )
		{
			hr = pTempGraph->Load( pIStream );

			// Transfer tools from temporary Graph to m_pGraph
			if( SUCCEEDED ( hr ) )
			{
				if( pTempGraph->m_lstGraphTools.IsEmpty() == FALSE )
				{
					// Paste tools
					POSITION pos = pTempGraph->m_lstGraphTools.GetHeadPosition();
					while( pos )
					{
						CTool* pTool = pTempGraph->m_lstGraphTools.GetNext( pos );

						pTool->AddRef();
						{
							pTempGraph->RemoveTool( pTool );

							// If pasting from Tool Palette or a different Graph.....
							if( (pTempGraph->m_GraphUI.dwFlagsUI & GRAPHUI_FROM_TOOL_PALETTE)
							||  (fToolsFromSameGraph == false) )
							{
								// Replace tool's PChannelGroups with all PChannelGroups in this Graph
								m_pGraph->UseGraphPChannelGroups( pTool );
							}
							pTool->CombineAllPChannelGroups();

							pTool->m_fSelected = TRUE;
							m_pGraph->InsertTool( pTool, nPasteIndex );
							if( nPasteIndex >= 0 )
							{
								nPasteIndex++;
							}
						}
						RELEASE( pTool );
					}
				}
			}

			RELEASE( pTempGraph );
		}

		RELEASE( pIStream );
	}

	// Refresh properties
	if( fInDragDrop == FALSE
	||  fToolsFromSameGraph == false )
	{
		OnViewProperties();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::EndTrack

void CGraphDlg::EndTrack( int nID, int nXPos )
{
	ASSERT( m_pGraph != NULL );

	CRect rect;
	GetClientRect( &rect );

	switch( nID )
	{
		case IDC_PCHANNEL_SPLITTER:
			m_pGraph->m_GraphUI.nPChannelColumnWidth = nXPos;
			m_lstbxGraph.InvalidateRect( NULL );
			break;

		case IDC_TOOL_SPLITTER:
			m_pGraph->m_GraphUI.nRegisteredToolPaneWidth = rect.right - nXPos;
			m_lstbxTools.InvalidateRect( NULL );
			break;
	}

	m_pGraph->SetModified( TRUE );
	OnSize( SIZE_RESTORED, rect.Width(), rect.Height() );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::GetToolFromXPos

CTool* CGraphDlg::GetToolFromXPos( int nXPos )
{
	if( nXPos < 0 )
	{
		return NULL;
	}

	// Account for horizontal scrolling
	nXPos += m_lstbxGraph.m_nHScrollPos;

	int nX = 0;

	CTool* pTool;
	for( int i = 0 ;  pTool = m_pGraph->MyEnumTools(i) ;  i++ )
	{
		if( pTool == NULL )
		{
			break;
		}

		nX += pTool->GetColumnWidth();

		if( nXPos <= nX )
		{
			return pTool;
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::FillToolsListBox

void CGraphDlg::FillToolsListBox( void )
{
	ASSERT( theApp.m_pGraphComponent != NULL );

	CDC* pDC = m_lstbxTools.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics( &tm );

		RegisteredTool* pRegisteredTool;
		CSize sizeText;
		int nPos;
		int nCurExtent = m_lstbxTools.GetHorizontalExtent();

		for( int i = 0 ;  pRegisteredTool = theApp.m_pGraphComponent->EnumRegisteredTools(i) ;  i++ )
		{
			if( pRegisteredTool == NULL )
			{
				break;
			}

			nPos = m_lstbxTools.AddString( pRegisteredTool->strName );
			m_lstbxTools.SetItemDataPtr( nPos, pRegisteredTool );

			sizeText = pDC->GetTextExtent( pRegisteredTool->strName );
			if( sizeText.cx > nCurExtent )
			{
				nCurExtent = sizeText.cx;
				m_lstbxTools.SetHorizontalExtent( nCurExtent );
			}
		}

		m_lstbxTools.ReleaseDC( pDC );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::FillPChannelListBox

void CGraphDlg::FillPChannelListBox( void )
{
	m_lstbxPChannels.ResetContent();
	m_lstbxPChannels.SetHorizontalExtent( 0 );

	POSITION pos = m_pGraph->m_lstGraphPChannelGroups.GetHeadPosition();
	while( pos )
	{ 
		CPChannelGroup* pPChannelGroup = m_pGraph->m_lstGraphPChannelGroups.GetNext( pos );

		int nPos = m_lstbxPChannels.AddString( _T("") );
		m_lstbxPChannels.SetItemDataPtr( nPos, pPChannelGroup );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::FillGraphListBox

void CGraphDlg::FillGraphListBox( void )
{
	ASSERT( theApp.m_pGraphComponent != NULL );

	m_lstbxGraph.SetRedraw( FALSE );
	
	// Fill listbox
	m_lstbxGraph.ResetContent();
	POSITION pos = m_pGraph->m_lstGraphPChannelGroups.GetHeadPosition();
	while( pos )
	{ 
		CPChannelGroup* pPChannelGroup = m_pGraph->m_lstGraphPChannelGroups.GetNext( pos );

		int nPos = m_lstbxGraph.AddString( _T("") );
		m_lstbxGraph.SetItemDataPtr( nPos, pPChannelGroup );
	}

	// Set horizontal extent
	CTool* pTool;
	int nExtent = 0;
	for( int i = 0 ;  pTool = m_pGraph->MyEnumTools(i) ;  i++ )
	{
		if( pTool == NULL )
		{
			break;
		}

		nExtent += pTool->GetColumnWidth();
	}
	m_lstbxGraph.SetHorizontalExtent( nExtent );
	m_lstbxGraph.SendMessage( WM_HSCROLL,
							  MAKEWPARAM( SB_THUMBPOSITION, min(m_lstbxGraph.m_nHScrollPos, nExtent) ),
							  NULL );

	m_lstbxGraph.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::CreateFont

CFont* CGraphDlg::CreateFont( void )
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


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnUpdateEditCut

void CGraphDlg::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	BOOL fEnable = FALSE;

	switch( m_nEditMenuCtrlID )
	{
		case IDC_PCHANNEL_LIST:
			fEnable = FALSE;
			break;

		default:
			if( m_pGraph->GetFirstSelectedTool() != NULL )
			{
				fEnable = TRUE;
			}
			break;
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnEditCut

void CGraphDlg::OnEditCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	switch( m_nEditMenuCtrlID )
	{
		case IDC_PCHANNEL_LIST:
			// Nothing to do
			break;

		default:
			if( m_pGraph->GetFirstSelectedTool() != NULL )
			{
				OnEditCopy();
				OnEditDelete();
			}
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnUpdateEditCopy

void CGraphDlg::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );
	
	BOOL fEnable = FALSE;

	switch( m_nEditMenuCtrlID )
	{
		case IDC_PCHANNEL_LIST:
			fEnable = FALSE;
			break;

		default:
			if( m_pGraph->GetFirstSelectedTool() != NULL )
			{
				fEnable = TRUE;
			}
			break;
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnEditCopy

void CGraphDlg::OnEditCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	switch( m_nEditMenuCtrlID )
	{
		case IDC_PCHANNEL_LIST:
			// Nothing to do
			break;

		default:
			if( m_pGraph->GetFirstSelectedTool() != NULL )
			{
				IDataObject* pIDataObject;
				if( SUCCEEDED ( m_btnGraphHeader.CreateDataObject( &pIDataObject ) ) )
				{
					theApp.PutDataInClipboard( pIDataObject, m_pGraph );

					RELEASE( pIDataObject );
				}
			}
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnUpdateEditPaste

void CGraphDlg::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL fEnable = FALSE;

	// Get the IDataObject
	IDataObject* pIDataObject;
	if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
	{
		POINT pt = {0, 0};
		HRESULT hr = CanPasteFromData( pIDataObject, false, pt );
		
		RELEASE( pIDataObject );
		
		if( hr == S_OK )
		{
			fEnable = TRUE;
		}
	}
	
	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnEditPaste

void CGraphDlg::OnEditPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the IDataObject
	IDataObject* pIDataObject;
	if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
	{
		m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_INSERT_TOOLS );

		POINT pt = {0, 0};
		PasteFromData( pIDataObject, false, pt );

		m_pGraph->SetModified( TRUE );
		m_pGraph->Refresh();
		
		RELEASE( pIDataObject );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnUpdateEditInsert

void CGraphDlg::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	pCmdUI->Enable( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnEditInsert

void CGraphDlg::OnEditInsert() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	POINT pt = {0, 0};
	OnChangePChannelGroup( pt, true );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnUpdateEditDelete

void CGraphDlg::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );
	
	BOOL fEnable = FALSE;
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_PCHANNEL_LIST:
			if( m_lstbxPChannels.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			break;

		default:
			if( m_pGraph->GetFirstSelectedTool() != NULL )
			{
				fEnable = TRUE;
			}
			break;
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnEditDelete

void CGraphDlg::OnEditDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_PCHANNEL_LIST:
			OnDeletePChannelGroups();
			break;

		default:
			OnDeleteTools();
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnEditSelectAll

void CGraphDlg::OnEditSelectAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );
	
	switch( m_nEditMenuCtrlID )
	{
		case IDC_PCHANNEL_LIST:
			m_lstbxPChannels.SetSel( -1, TRUE );
			m_lstbxPChannels.SetCaretIndex( m_lstbxPChannels.GetTopIndex(), 0 ); 
			break;

		default:
			m_pGraph->SelectAllTools();
			m_btnGraphHeader.Invalidate();
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnViewProperties

BOOL CGraphDlg::OnViewProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdPropSheet* pIPropSheet;

	ASSERT( m_pGraph != NULL );
	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	if( FAILED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return FALSE;
	}

	if( pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return TRUE;
	}

	BOOL fSuccess = FALSE;
	CTool* pTool = m_pGraph->GetFirstSelectedTool();
	if( pTool )
	{
		// Tool properties
		if( SUCCEEDED ( pTool->OnShowProperties() ) )
		{
			fSuccess = TRUE;
		}
	}
	else
	{
		// Graph properties
		if( SUCCEEDED ( m_pGraph->OnShowProperties() ) )
		{
			fSuccess = TRUE;
		}
	}

	RELEASE( pIPropSheet );

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnChangePChannelGroup

void CGraphDlg::OnChangePChannelGroup( POINT pointChange, bool fInsertNewGroup )
{
	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	CPChannelGroup* pPChannelGroupToEdit = NULL;

	if( fInsertNewGroup == false )
	{
		// Convert from screen to client coordinates
		m_lstbxPChannels.ScreenToClient( &pointChange );

		// Get the item at pointChange
		BOOL fOutside;
		UINT nPos = m_lstbxPChannels.ItemFromPoint( pointChange, fOutside );
		if( nPos != LB_ERR
		&&  fOutside == FALSE )
		{
			pPChannelGroupToEdit = (CPChannelGroup *)m_lstbxPChannels.GetItemDataPtr( nPos );
			ASSERT( pPChannelGroupToEdit != NULL );
			ASSERT( pPChannelGroupToEdit != (CPChannelGroup *)0xFFFFFFFF );
		}
		else
		{
			if( m_lstbxPChannels.GetSelCount() == 1 )
			{
				int nPos;
				m_lstbxPChannels.GetSelItems( 1, &nPos );
				pPChannelGroupToEdit = (CPChannelGroup *)m_lstbxPChannels.GetItemDataPtr( nPos );
				ASSERT( pPChannelGroupToEdit != NULL );
				ASSERT( pPChannelGroupToEdit != (CPChannelGroup *)0xFFFFFFFF );
			}
		}
	}

	// Initialize the add PChannel dialog
	CDlgAddPChannel dlgAddPChannel;

	// Set dialog title
	if( pPChannelGroupToEdit )
	{
		AfxFormatString1( dlgAddPChannel.m_strTitle, IDS_TITLE_CHANGE_PCHANNEL_GROUP, m_pGraph->m_strName );
	}
	else
	{
		AfxFormatString1( dlgAddPChannel.m_strTitle, IDS_TITLE_INSERT_PCHANNEL_GROUP, m_pGraph->m_strName );
	}

	// Set the PChannelName pointer
	IDMUSProdProject* pIProject;
	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->FindProject( m_pGraph, &pIProject ) ) )
	{
		pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&dlgAddPChannel.m_pIPChannelName );
		RELEASE( pIProject );
	}

	// Set up the array of existing PChannels
	POSITION pos = m_pGraph->m_lstGraphPChannelGroups.GetHeadPosition();
	while( pos )
	{
		CPChannelGroup* pPChannelGroup = m_pGraph->m_lstGraphPChannelGroups.GetNext( pos );

		// Skip the item we're editing
		if( pPChannelGroup != pPChannelGroupToEdit )
		{
			// Iterate through all PChannels
			for( DWORD i = 0;  i < pPChannelGroup->m_dwNbrPChannels;  i++ )
			{
				// Add each PChannel to the array (order is irrelevant)
				dlgAddPChannel.m_adwExistingPChannels.Add( pPChannelGroup->m_pdwPChannel[i] );
			}
		}
	}

	// Set up the array of PChannels for the item we're adding
	if( pPChannelGroupToEdit )
	{
		// Itrate through all PChannels
		for( DWORD i=0; i < pPChannelGroupToEdit->m_dwNbrPChannels; i++ )
		{
			// Add each PChannel to the array (order is irrelevant)
			dlgAddPChannel.m_adwPChannels.Add( pPChannelGroupToEdit->m_pdwPChannel[i] );
		}
	}

	if( dlgAddPChannel.DoModal() == IDOK )
	{
		bool fChanged = false;
		bool fNewPChannelGroup = false;
		if( pPChannelGroupToEdit == NULL )
		{
			// Create a new PChannel group
			pPChannelGroupToEdit = new CPChannelGroup();
			fNewPChannelGroup = true;
		}

		if( pPChannelGroupToEdit )
		{
			// Check if anything changed
			if( (pPChannelGroupToEdit->m_dwNbrPChannels != (DWORD)dlgAddPChannel.m_adwPChannels.GetSize())
			||	(memcmp( pPChannelGroupToEdit->m_pdwPChannel, dlgAddPChannel.m_adwPChannels.GetData(),
						 sizeof(DWORD) * pPChannelGroupToEdit->m_dwNbrPChannels )) )
			{
				// Create a new array to store the PChannels in
				DWORD *pdwNewPChannels = new DWORD[dlgAddPChannel.m_adwPChannels.GetSize()];
				if( pdwNewPChannels )
				{
					// Save undo state
					if( pPChannelGroupToEdit->m_dwNbrPChannels == 0 )
					{
						m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_INSERT_PCHANNELGROUP );
					}
					else
					{
						m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_CHANGE_PCHANNELGROUP );
					}

					// Copy the PChannels into the new array
					memcpy( pdwNewPChannels, dlgAddPChannel.m_adwPChannels.GetData(), sizeof(DWORD) * dlgAddPChannel.m_adwPChannels.GetSize() );

					// Delete the old array
					delete [] pPChannelGroupToEdit->m_pdwPChannel;

					// Set the size of, and point to the new array
					pPChannelGroupToEdit->m_pdwPChannel = pdwNewPChannels;
					pPChannelGroupToEdit->m_dwNbrPChannels = dlgAddPChannel.m_adwPChannels.GetSize();

					// Reposition in m_lstPChannelGroups
					m_pGraph->RepositionPChannelGroup( pPChannelGroupToEdit );

					// Add new PChannelGroup to all Tools
					if( fNewPChannelGroup )
					{
						POSITION pos = m_pGraph->m_lstGraphTools.GetHeadPosition();
						while( pos )
						{
							CTool* pTool = m_pGraph->m_lstGraphTools.GetNext( pos );

							pTool->InsertPChannelGroup( pPChannelGroupToEdit );
						}
					}

					// Sync list box controls
					m_pGraph->SetModified( TRUE );
					m_pGraph->Refresh();
				}
			}
		}
	}

	m_lstbxPChannels.SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnDeletePChannelGroups

void CGraphDlg::OnDeletePChannelGroups( void )
{
	int nNbrSelItems = m_lstbxPChannels.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_DELETE_PCHANNELGROUP );

		m_lstbxPChannels.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			CPChannelGroup* pPChannelGroup = (CPChannelGroup *)m_lstbxPChannels.GetItemDataPtr( pnSelItems[i] );
		
			if( pPChannelGroup
			&&  pPChannelGroup != (CPChannelGroup *)0xFFFFFFFF )
			{
				// Remove pPChannelGroup from m_lstPChannelGroups
				m_pGraph->RemovePChannelGroup( pPChannelGroup );
			}
		}

		// Sync list box controls
		m_pGraph->SetModified( TRUE );
		m_pGraph->Refresh();

		delete [] pnSelItems;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnDeleteTools

void CGraphDlg::OnDeleteTools( void )
{
	CTool* pTool = m_pGraph->GetFirstSelectedTool();
	if( pTool == NULL )
	{
		// Nothing to do
		return;
	}

	m_pGraph->m_pUndoMgr->SaveState( m_pGraph, theApp.m_hInstance, IDS_UNDO_DELETE_TOOLS );
	m_pGraph->DeleteSelectedTools();

	m_btnGraphHeader.m_nLastXPos = INT_MAX;
	m_pGraph->SetModified( TRUE );
	m_pGraph->Refresh();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnContextMenu

void CGraphDlg::OnContextMenu( CWnd* pWnd, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pWnd != NULL );
	ASSERT( m_pGraph != NULL );

	switch( pWnd->GetDlgCtrlID() )
	{
		case IDC_PCHANNEL_HEADER:
		case IDC_PCHANNEL_LIST:
		{
			CPChannelGroup* pPChannelGroupToEdit = NULL;

			m_lstbxPChannels.SetFocus(); 

			// Get the item under the mouse
			BOOL fOutside;
			POINT ptClient = point;
			m_lstbxPChannels.ScreenToClient( &ptClient );
			UINT nPos = m_lstbxPChannels.ItemFromPoint( ptClient, fOutside );
			if( nPos != LB_ERR
			&&  fOutside == FALSE )
			{
				pPChannelGroupToEdit = (CPChannelGroup *)m_lstbxPChannels.GetItemDataPtr( nPos );
				ASSERT( pPChannelGroupToEdit != NULL );
				ASSERT( pPChannelGroupToEdit != (CPChannelGroup *)0xFFFFFFFF );
			}

			// Load menu
			HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_GRAPH_PCHANNELGROUPS) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			// Initialize menu
			::EnableMenuItem( hMenuPopup, IDM_CHANGE_PCHANNEL_GROUP, ( (pPChannelGroupToEdit != NULL) || (m_lstbxPChannels.GetSelCount() == 1) )
								? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_DELETE_PCHANNEL_GROUP, ( m_lstbxPChannels.GetSelCount() > 0 )
								? MF_ENABLED : MF_GRAYED );

			m_pointRightMenu = point;
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			::DestroyMenu( hMenu );
			break;
		}
			 
		case IDC_GRAPH_HEADER:
		case IDC_GRAPH_LIST:
		{
			// Get the tool under the mouse
			POINT ptClient = point;
			m_btnGraphHeader.ScreenToClient( &ptClient );
			CTool* pTool = GetToolFromXPos( ptClient.x );

			// Load menu
			HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_GRAPH_TOOLS) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			// Initialize menu
			CTool* pFirstSelectedTool = m_pGraph->GetFirstSelectedTool();
			::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, pFirstSelectedTool ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, pFirstSelectedTool ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_SELECT_ALL, m_pGraph->MyEnumTools(0) ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, pFirstSelectedTool ? MF_ENABLED : MF_GRAYED );
			::EnableMenuItem( hMenuPopup, IDM_TOOL_PROPERTIES, pFirstSelectedTool ? MF_ENABLED : MF_GRAYED );
			
			BOOL fEnable = FALSE;
			if( ::GetFocus() == m_lstbxPChannels.GetSafeHwnd()
			&&  m_lstbxPChannels.GetSelCount() > 0 )
			{
				fEnable = TRUE;
			}
			::EnableMenuItem( hMenuPopup, IDM_DELETE_PCHANNEL_GROUP, fEnable ? MF_ENABLED : MF_GRAYED );

			fEnable = FALSE;
			IDataObject* pIDataObject;
			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				POINT pt = {0, 0};
				HRESULT hr = CanPasteFromData( pIDataObject, false, pt );
				
				RELEASE( pIDataObject );
				
				if( hr == S_OK )
				{
					fEnable = TRUE;
				}
			}
			::EnableMenuItem( hMenuPopup, ID_EDIT_PASTE, fEnable ? MF_ENABLED : MF_GRAYED );

			m_pointRightMenu = point;
			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			::DestroyMenu( hMenu );
			break;
		}
			 
		case IDC_TOOL_HEADER:
		case IDC_TOOL_LIST:
			// Nothing to do
			break;

		default:
		{
			HINSTANCE hInstance;
			UINT nResourceId;
			if( SUCCEEDED ( m_pGraph->GetRightClickMenuId(&hInstance, &nResourceId) ) )
			{
				HMENU hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );
				if( hMenu )
				{
					HMENU hMenuPopup = ::GetSubMenu( hMenu, 0 );
					m_pGraph->OnRightClickMenuInit( hMenuPopup );

					m_pINodeRightMenu = m_pGraph;
					m_pointRightMenu = point;
					::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
									  point.x, point.y, 0, GetSafeHwnd(), NULL );

					DestroyMenu( hMenu );
				}
			}
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg::OnCommand

BOOL CGraphDlg::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( HIWORD(wParam) == 0 )	// menu command
	{
		if( m_pINodeRightMenu )	// can receive menu commands when m_pINodeRightMenu is NULL
		{
			m_pINodeRightMenu->OnRightClickMenuSelect( LOWORD(wParam) );
			m_pINodeRightMenu = NULL ;
			return TRUE;
		}

		switch( LOWORD(wParam) )
		{
			case IDM_CHANGE_PCHANNEL_GROUP:
				OnChangePChannelGroup( m_pointRightMenu, false );
				return TRUE;
			
			case IDM_INSERT_PCHANNEL_GROUP:
				OnChangePChannelGroup( m_pointRightMenu, true );
				return TRUE;

			case IDM_DELETE_PCHANNEL_GROUP:
				OnDeletePChannelGroups();
				return TRUE;

			case ID_EDIT_CUT:
				OnEditCut();
				return TRUE;

			case ID_EDIT_COPY:
				OnEditCopy();
				return TRUE;

			case ID_EDIT_PASTE:
				OnEditPaste();
				return TRUE;

			case IDM_SELECT_ALL:
				OnEditSelectAll();
				return TRUE;

			case ID_EDIT_INSERT:
				OnEditInsert();
				return TRUE;

			case ID_EDIT_DELETE:
				OnEditDelete();
				return TRUE;

			case IDM_TOOL_PROPERTIES:
			{
				CTool* pTool = m_pGraph->GetFirstSelectedTool();
				if( pTool )
				{
					pTool->OnShowProperties();
				}
				return TRUE;
			}
		}
	}
	
	return CFormView::OnCommand( wParam, lParam );
}
