// StyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "TimeSignatureDlg.h"
#include "StyleCtl.h"
#include "StyleDlg.h"
#include "Splitter.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox

CStyleComboBox::CStyleComboBox()
{
	m_pStyleDlg = NULL;
}

CStyleComboBox::~CStyleComboBox()
{
}


BEGIN_MESSAGE_MAP(CStyleComboBox, CComboBox)
	//{{AFX_MSG_MAP(CStyleComboBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox message handlers

/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox::OnMouseMove

void CStyleComboBox::OnMouseMove( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pStyleDlg != NULL );

	CComboBox::OnMouseMove( nFlags, point );
	
	if( CWnd::GetCapture() == this )
	{
		if( m_pStyleDlg->m_dwMouseDownButton != 0 )
		{
			// See if user is moving the mouse
			if( m_pStyleDlg->m_rectMouseDown.PtInRect( point ) == FALSE )
			{
				// Start drag drop
				m_pStyleDlg->DoDrag( m_pStyleDlg->m_pIMouseDownNode, m_pStyleDlg->m_dwMouseDownButton ); 
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox::OnCaptureChanged

void CStyleComboBox::OnCaptureChanged( CWnd *pWnd ) 
{
	if( pWnd == this )
	{
		return;
	}

	m_pStyleDlg->m_dwMouseDownButton = 0;
	m_pStyleDlg->m_pIMouseDownNode = NULL;
	m_pStyleDlg->m_rectMouseDown.SetRectEmpty();
	
	CComboBox::OnCaptureChanged( pWnd );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox::OnLButtonDown

void CStyleComboBox::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pStyleDlg != NULL );

	CRect rect;

	GetClientRect( &rect );
	rect.right -= 18;

	if( rect.PtInRect( point ) == TRUE )
	{
		// Cursor in edit box
		SetFocus();
		m_pStyleDlg->OnSelChangeBandCombo();

		if( m_pStyleDlg->m_dwMouseDownButton == 0 )
		{
			// Get the Band
			int nPos = GetCurSel();
			if( nPos != CB_ERR )
			{
				IDMUSProdNode* pINode = (IDMUSProdNode *)GetItemDataPtr( nPos ); 
				if( pINode
				&&  pINode != (IDMUSProdNode *)-1 )
				{
					// First set capture
					SetCapture();

					// Now store fields used when drag drop is initiated
					m_pStyleDlg->m_dwMouseDownButton = MK_LBUTTON;
					m_pStyleDlg->m_pIMouseDownNode = pINode;

					m_pStyleDlg->m_rectMouseDown.left = point.x - 5;
					m_pStyleDlg->m_rectMouseDown.top = point.y - 5;
					m_pStyleDlg->m_rectMouseDown.right = point.x + 5;
					m_pStyleDlg->m_rectMouseDown.bottom = point.y + 5;
				}
			}
		}
		return;
	}

	CComboBox::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox::OnLButtonUp

void CStyleComboBox::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	if( m_pStyleDlg->m_dwMouseDownButton == MK_LBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}
	
	CComboBox::OnLButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox::OnLButtonDblClk

void CStyleComboBox::OnLButtonDblClk( UINT nFlags, CPoint point ) 
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CRect rect;

	GetClientRect( &rect );
	rect.right -= 18;

	if( rect.PtInRect( point ) == TRUE )
	{
		// Cursor in edit box
		IDMUSProdNode* pINode = NULL;

		// Get the Band
		int nPos = GetCurSel();
		if( nPos != CB_ERR )
		{
			pINode = (IDMUSProdNode *)GetItemDataPtr( nPos ); 
		}

		if( pINode )
		{
			theApp.m_pStyleComponent->m_pIFramework->OpenEditor( pINode );
		}

		return;
	}

	CComboBox::OnLButtonDblClk(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox::OnRButtonDown

void CStyleComboBox::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pStyleDlg != NULL );

	SetFocus();
	m_pStyleDlg->OnSelChangeBandCombo();

	if( m_pStyleDlg->m_dwMouseDownButton == 0 )
	{
		// Get the Band
		int nPos = GetCurSel();
		if( nPos != CB_ERR )
		{
			IDMUSProdNode* pINode = (IDMUSProdNode *)GetItemDataPtr( nPos ); 
			if( pINode
			&&  pINode != (IDMUSProdNode *)-1 )
			{
				// First set capture
				SetCapture();

				// Now store fields used when drag drop is initiated
				m_pStyleDlg->m_dwMouseDownButton = MK_RBUTTON;
				m_pStyleDlg->m_pIMouseDownNode = pINode;

				m_pStyleDlg->m_rectMouseDown.left = point.x - 5;
				m_pStyleDlg->m_rectMouseDown.top = point.y - 5;
				m_pStyleDlg->m_rectMouseDown.right = point.x + 5;
				m_pStyleDlg->m_rectMouseDown.bottom = point.y + 5;
			}
		}
	}

	CComboBox::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox::OnRButtonUp

void CStyleComboBox::OnRButtonUp( UINT nFlags, CPoint point ) 
{
	if( m_pStyleDlg->m_dwMouseDownButton == MK_RBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}
	
	CComboBox::OnRButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox

CStyleListBox::CStyleListBox()
{
	m_pStyleDlg = NULL;
}

CStyleListBox::~CStyleListBox()
{
}


BEGIN_MESSAGE_MAP(CStyleListBox, CListBox)
	//{{AFX_MSG_MAP(CStyleListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStyleListBox message handlers

/////////////////////////////////////////////////////////////////////////////
// CStyleListBox::OnMouseMove

void CStyleListBox::OnMouseMove( UINT nFlags, CPoint point ) 
{
	UNREFERENCED_PARAMETER( nFlags );

	ASSERT( m_pStyleDlg != NULL );
	
	if( CWnd::GetCapture() == this )
	{
		if( m_pStyleDlg->m_dwMouseDownButton != 0 )
		{
			// See if user is moving the mouse
			if( m_pStyleDlg->m_rectMouseDown.PtInRect( point ) == FALSE )
			{
				// Start drag drop
				m_pStyleDlg->DoDrag( m_pStyleDlg->m_pIMouseDownNode, m_pStyleDlg->m_dwMouseDownButton ); 
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox::OnCaptureChanged

void CStyleListBox::OnCaptureChanged( CWnd *pWnd ) 
{
	if( pWnd == this )
	{
		return;
	}

	m_pStyleDlg->m_dwMouseDownButton = 0;
	m_pStyleDlg->m_pIMouseDownNode = NULL;
	m_pStyleDlg->m_rectMouseDown.SetRectEmpty();
	
	CListBox::OnCaptureChanged( pWnd );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox::OnLButtonDown

void CStyleListBox::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pStyleDlg != NULL );

	if( m_pStyleDlg->m_dwMouseDownButton == 0 )
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
				
				switch( GetDlgCtrlID() )
				{
					case IDC_PATTERN_LIST:
						m_pStyleDlg->OnSelChangePatternList();
						break;

					case IDC_MOTIF_LIST:
						m_pStyleDlg->OnSelChangeMotifList();
						m_pStyleDlg->PlayMotif();
						break;
				}

				CDirectMusicPattern* pPattern = (CDirectMusicPattern *)GetItemData( nPos );

				if( pPattern
				&&  pPattern != (CDirectMusicPattern *)LB_ERR )
				{
					// First set capture
					SetCapture();

					// Now store fields used when drag drop is initiated
					m_pStyleDlg->m_dwMouseDownButton = MK_LBUTTON;
					m_pStyleDlg->m_pIMouseDownNode = pPattern;

					m_pStyleDlg->m_rectMouseDown.left = point.x - 5;
					m_pStyleDlg->m_rectMouseDown.top = point.y - 5;
					m_pStyleDlg->m_rectMouseDown.right = point.x + 5;
					m_pStyleDlg->m_rectMouseDown.bottom = point.y + 5;
				}
			}
		}
	}

	CListBox::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox::OnLButtonUp

void CStyleListBox::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	if( m_pStyleDlg->m_dwMouseDownButton == MK_LBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}
	
	CListBox::OnLButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox::OnRButtonDown

void CStyleListBox::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pStyleDlg != NULL );

	if( m_pStyleDlg->m_dwMouseDownButton == 0 )
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
				
				switch( GetDlgCtrlID() )
				{
					case IDC_PATTERN_LIST:
						m_pStyleDlg->OnSelChangePatternList();
						break;

					case IDC_MOTIF_LIST:
						m_pStyleDlg->OnSelChangeMotifList();
						break;
				}

				CDirectMusicPattern* pPattern = (CDirectMusicPattern *)GetItemData( nPos );

				if( pPattern
				&&  pPattern != (CDirectMusicPattern *)LB_ERR )
				{
					// First set capture
					SetCapture();

					// Now store fields used when drag drop is initiated
					m_pStyleDlg->m_dwMouseDownButton = MK_RBUTTON;
					m_pStyleDlg->m_pIMouseDownNode = pPattern;

					m_pStyleDlg->m_rectMouseDown.left = point.x - 5;
					m_pStyleDlg->m_rectMouseDown.top = point.y - 5;
					m_pStyleDlg->m_rectMouseDown.right = point.x + 5;
					m_pStyleDlg->m_rectMouseDown.bottom = point.y + 5;
				}
			}
		}
	}

	CListBox::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox::OnRButtonUp

void CStyleListBox::OnRButtonUp( UINT nFlags, CPoint point ) 
{
	if( m_pStyleDlg->m_dwMouseDownButton == MK_RBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}
	
	CListBox::OnRButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox::OnHScroll

void CStyleListBox::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	CListBox::OnHScroll( nSBCode, nPos, pScrollBar );

	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg

IMPLEMENT_DYNCREATE(CStyleDlg, CFormView)

CStyleDlg::CStyleDlg()
	: CFormView(CStyleDlg::IDD)
{
	//{{AFX_DATA_INIT(CStyleDlg)
	//}}AFX_DATA_INIT

	m_dwRef = 0;
	AddRef();
	
	m_pStyleCtrl = NULL;
	m_pStyle = NULL;

	m_pIDataObject = NULL;
	m_dwStartDragButton = 0;
	m_dwOverDragButton = 0;
	m_dwOverDragEffect = 0;
	m_pDragImage = NULL;
	m_dwDragRMenuEffect = DROPEFFECT_NONE;
	m_pIDragNode = NULL;

	m_dwMouseDownButton = 0;
	m_pIMouseDownNode = NULL;

	m_pINodeRightMenu = NULL;
	m_nEditMenuCtrlID = 0;
	m_pFont = NULL;
}

CStyleDlg::~CStyleDlg()
{
}

void CStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStyleDlg)
	DDX_Control(pDX, IDC_SEPARATOR_LINE, m_staticSeparator);
	DDX_Control(pDX, IDC_TIME_SIGNATURE, m_btnTimeSignature);
	DDX_Control(pDX, IDC_TEMPO_SPIN, m_spinTempo);
	DDX_Control(pDX, IDC_TEMPO, m_editTempo);
	DDX_Control(pDX, IDC_BAND_DEFAULT, m_checkBandDefault);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::SetTempoControlText

void CStyleDlg::SetTempoControlText( void ) 
{
	if( m_pStyle )
	{
		CString strTempo;

		strTempo.Format( "%.2f", m_pStyle->m_dblTempo );
		m_editTempo.SetWindowText( strTempo );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::DoDrag

void CStyleDlg::DoDrag( IDMUSProdNode* pINode, DWORD dwStartDragButton )
{
	ASSERT( pINode != NULL );

	// If already dragging, just return
	if( m_pDragImage )
	{
		return;
	}

	if( pINode )
	{
		IDataObject* pIDataObject;
		DWORD dwEffect;
		HRESULT hr;

		if( SUCCEEDED ( pINode->CreateDataObject( &pIDataObject ) ) )
		{
			// Create image used for drag-drop feedback
			if( m_pDragImage )
			{
				m_pDragImage->BeginDrag( 0, CPoint(8, 12) );
			}

			// Start drag-drop operation
			DWORD dwOKDragEffects = DROPEFFECT_COPY;
			if( pINode->CanCut() == S_OK )
			{
				dwOKDragEffects |= DROPEFFECT_MOVE;
			}

			m_pIDragNode = pINode;
			m_dwStartDragButton = dwStartDragButton;
			
			hr = ::DoDragDrop( pIDataObject, (IDropSource *)this, dwOKDragEffects, &dwEffect );

			m_pIDragNode = NULL;
			m_dwStartDragButton = 0;

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
						pINode->DeleteNode( FALSE );
					}
					break;
			}
			
			RELEASE( pIDataObject );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::EndTrack

void CStyleDlg::EndTrack( int nHeight )
{
	ASSERT( m_pStyle != NULL );

	m_pStyle->m_nSplitterYPos = nHeight;
	m_pStyle->SetModified( TRUE );

	CRect rect;
	GetClientRect( &rect );

    m_lstbxMotif.InvalidateRect( NULL );
	OnSize( SIZE_RESTORED, rect.Width(), rect.Height() );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::RefreshControls

void CStyleDlg::RefreshControls( DWORD dwFlags )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( dwFlags & SSE_STYLE )
	{
		// Update bitmap on time signature button
		SetTimeSignatureBitmap();

		// Set tempo control
		SetTempoControlText();
	}

	if( dwFlags & SSE_BANDS )
	{
		FillBandComboBox();
	}

	if( dwFlags & SSE_PATTERNS )
	{
		FillPatternListBox();
	}

	if( dwFlags & SSE_MOTIFS )
	{
		FillMotifListBox();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::SelectPattern

void CStyleDlg::SelectPattern( CDirectMusicPattern* pPattern )
{
	ASSERT( pPattern != NULL );

	int nPos = m_lstbxPattern.FindStringExact( -1, (LPCTSTR)pPattern );

	if( nPos != LB_ERR )
	{
		m_lstbxPattern.SetCurSel( nPos );
		m_lstbxPattern.SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::SelectMotif

void CStyleDlg::SelectMotif( CDirectMusicPattern* pMotif )
{
	ASSERT( pMotif != NULL );

	int nPos = m_lstbxMotif.FindStringExact( -1, (LPCTSTR)pMotif );

	if( nPos != LB_ERR )
	{
		m_lstbxMotif.SetCurSel( nPos );
		m_lstbxMotif.SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::SelectBand

void CStyleDlg::SelectBand( IDMUSProdNode* pINode )
{
	ASSERT( pINode != NULL );
	ASSERT( m_pStyle != NULL );

	// Get name of Band
	CString strName;
	BSTR bstrName;

	if( SUCCEEDED ( pINode->GetNodeName ( &bstrName ) ) )
	{
		strName = bstrName;
		::SysFreeString( bstrName );
	}

	// Search combo box for Band
	IDMUSProdNode* pINodeList;

	int nPos = m_cmbxBand.FindStringExact( 0, strName );
	while( nPos != CB_ERR )
	{
		pINodeList = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
		if( pINodeList == pINode )
		{
			break;
		}

		nPos = m_cmbxBand.FindStringExact( nPos, strName );
	}

	if( nPos != CB_ERR )
	{
		m_cmbxBand.SetCurSel( nPos );
		OnSelChangeBandCombo();

		// Set the Style's active Band
		m_pStyle->SetActiveBand( pINode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::SetTimeSignatureBitmap

void CStyleDlg::SetTimeSignatureBitmap( void )
{
	HBITMAP hNewBits = NULL;

	ASSERT( m_pStyle != NULL );

	RECT rect;
	m_btnTimeSignature.GetClientRect( &rect );

	// Create a DC for the new bitmap
	// a DC for the 'Grids Per Beat' bitmap
	// a Bitmap for the new bits
	CDC cdcDest;
	CDC cdcGridsPerBeat;
	CBitmap bmpNewBits;
	CBitmap bmpGridsPerBeat;

	CDC* pDC = m_btnTimeSignature.GetDC();
	if( pDC )
	{

		if( cdcDest.CreateCompatibleDC( pDC ) == FALSE
		||  cdcGridsPerBeat.CreateCompatibleDC( pDC ) == FALSE
		||  bmpNewBits.CreateCompatibleBitmap( pDC, rect.right, rect.bottom ) == FALSE )
		{
			m_btnTimeSignature.ReleaseDC( pDC );
			return;
		}

		m_btnTimeSignature.ReleaseDC( pDC );
	}

	// Create the new bitmap
	CBitmap* pbmpOldMem = cdcDest.SelectObject( &bmpNewBits );

	// Fill Rect with button color
	cdcDest.SetBkColor( ::GetSysColor(COLOR_BTNFACE) );
	cdcDest.ExtTextOut( 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

	// Write text
	CString strTimeSignature;

	CFont font;
	CFont* pfontOld = NULL;

	if( font.CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 						DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) )
	{
		pfontOld = cdcDest.SelectObject( &font );
	}

	strTimeSignature.Format( "%d/%d",
							 m_pStyle->m_TimeSignature.m_bBeatsPerMeasure,
							 m_pStyle->m_TimeSignature.m_bBeat );
	rect.left += 6;
	cdcDest.SetTextColor( COLOR_BTNTEXT );
	cdcDest.DrawText( strTimeSignature, -1, &rect, (DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX) );
	rect.left -= 6;

	if( pfontOld )
	{
		cdcDest.SelectObject( pfontOld );
		font.DeleteObject();
	}

	// Set x coord for 'Grids Per Beat' image
	CSize sizeText = cdcDest.GetTextExtent( strTimeSignature );
	int nX = max( 48, (sizeText.cx + 8) );

	// Draw "splitter"
	{
		CPen pen1;
		CPen pen2;
		CPen* ppenOld;

		int nPlace = nX - 6;
		int nModeOld = cdcDest.SetROP2( R2_COPYPEN );

		// Highlight
		if( pen1.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW) ) )
		{
			ppenOld = cdcDest.SelectObject( &pen1 );
			cdcDest.MoveTo( nPlace, (rect.top + 3) );
			cdcDest.LineTo( nPlace, (rect.bottom - 3) );
			cdcDest.SelectObject( ppenOld );
		}

		// Shadow
		if( pen2.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT) ) )
		{
			ppenOld = cdcDest.SelectObject( &pen2 );
			cdcDest.MoveTo( ++nPlace, (rect.top + 3) );
			cdcDest.LineTo( nPlace, (rect.bottom - 3) );
			cdcDest.SelectObject( ppenOld );
		}

		if( nModeOld )
		{
			cdcDest.SetROP2( nModeOld );
		}
	}

	// Add 'Grids Per Beat' bitmap
	{
		int nResourceID = m_pStyle->m_TimeSignature.m_wGridsPerBeat - 1;
		if( m_pStyle->m_TimeSignature.m_bBeat != 4 )		// 4 = quarter note gets the beat
		{
			nResourceID += MAX_GRIDS_PER_BEAT;
		}
		ASSERT( (nResourceID >= 0) && (nResourceID <= MAX_GRIDS_PER_BEAT_ENTRIES) );
		
		if( bmpGridsPerBeat.LoadBitmap( g_nGridsPerBeatBitmaps[nResourceID] ) )
		{
			BITMAP bm;

			bmpGridsPerBeat.GetBitmap( &bm );

			int nY = ((rect.bottom - rect.top) - bm.bmHeight) >> 1;

			CBitmap* pbmpOld = cdcGridsPerBeat.SelectObject( &bmpGridsPerBeat );

			{
				CDC cdcMono;
				CBitmap bmpMono;

				if( cdcMono.CreateCompatibleDC( &cdcDest )
				&&  bmpMono.CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL ) )
				{
					CBitmap* pbmpOldMono = cdcMono.SelectObject( &bmpMono );
					
					cdcGridsPerBeat.SetBkColor( RGB(255,255,255) );
					cdcDest.SetBkColor( RGB(255,255,255) );

					cdcMono.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCCOPY);
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCINVERT );
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcMono, 0, 0, SRCAND );
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCINVERT );

					cdcMono.SelectObject( pbmpOldMono );
				}
			}

			cdcGridsPerBeat.SelectObject( pbmpOld );
		}
	}

	cdcDest.SelectObject( pbmpOldMem );

	// Set the new bitmap
	hNewBits = (HBITMAP)bmpNewBits.Detach();
	if( hNewBits )
	{
		HBITMAP hBitmapOld = m_btnTimeSignature.SetBitmap( hNewBits );
		if( hBitmapOld )
		{
			::DeleteObject( hBitmapOld );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::FillPatternListBox

void CStyleDlg::FillPatternListBox()
{
	CDirectMusicPattern * pCurPattern;

	ASSERT( m_pStyle != NULL );

	// Store the current Pattern selection
	int nCurPos = m_lstbxPattern.GetCurSel();
	if( nCurPos != LB_ERR )
	{
		pCurPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nCurPos ); 
	}

	// Initialize the list box
	m_lstbxPattern.ResetContent();
	m_lstbxPattern.SetHorizontalExtent( 0 );

	// Fill the list box
	CDirectMusicPattern * pPattern;
	CDC* pDC;
	TEXTMETRIC tm;
	int nTextWidth;
	int nChordRhythmWidth;
	int nMaxBPM;
	int nCurExtent;

    POSITION pos = m_pStyle->m_StylePatterns.m_lstPatterns.GetHeadPosition();
    while( pos )
    {
        pPattern = m_pStyle->m_StylePatterns.m_lstPatterns.GetNext( pos );

		m_lstbxPattern.AddString( (LPCTSTR)pPattern  );

		// Set horizontal extent
		nCurExtent = m_lstbxPattern.GetHorizontalExtent();

		pDC = m_lstbxPattern.GetDC();
		if( pDC )
		{
			// Calc width of chord rhythm
			nMaxBPM = min( 32, pPattern->m_TimeSignature.m_bBeatsPerMeasure ); 
			nChordRhythmWidth  = (nMaxBPM * pPattern->m_wNbrMeasures) * 3;
			nChordRhythmWidth += pPattern->m_wNbrMeasures * 3; 

			// Calc width of left-hand text + chord rhythm
			pDC->GetTextMetrics( &tm );
			nTextWidth = (tm.tmAveCharWidth * 52) + nChordRhythmWidth;
			
//			if( nTextWidth > nCurExtent )
//			{
//				m_lstbxPattern.SetHorizontalExtent( nTextWidth );
//			}

			m_lstbxPattern.ReleaseDC( pDC );
		}
	}

	// Set the current selection
	if( nCurPos == LB_ERR )
	{
		m_lstbxPattern.SetCurSel( -1 );
	}
	else
	{
		int nPos = LB_ERR;

		pos = m_pStyle->m_StylePatterns.m_lstPatterns.Find( pCurPattern );
		if( pos )
		{
			nPos = m_lstbxPattern.FindStringExact( -1, (LPCTSTR)pCurPattern );
		}

		if( nPos == LB_ERR )
		{
			nPos = nCurPos;

			int nCount = m_lstbxPattern.GetCount();
			if( nCount > 0 )
			{
				if( nPos > (nCount - 1) )
				{
					nPos = 0;
				}
			}
		}

		m_lstbxPattern.SetCurSel( nPos );
	}

	m_lstbxPattern.UpdateWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::FillMotifListBox

void CStyleDlg::FillMotifListBox()
{
	CDirectMusicPattern * pCurMotif;

	ASSERT( m_pStyle != NULL );

	// Store the current Motif selection
	int nCurPos = m_lstbxMotif.GetCurSel();
	if( nCurPos != LB_ERR )
	{
		pCurMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nCurPos ); 
	}

	// Initialize the list box
	m_lstbxMotif.ResetContent();

	// Fill the list box
	CDirectMusicPattern * pMotif;

    POSITION pos = m_pStyle->m_StyleMotifs.m_lstMotifs.GetHeadPosition();
    while( pos )
    {
        pMotif = m_pStyle->m_StyleMotifs.m_lstMotifs.GetNext( pos );

		m_lstbxMotif.AddString( (LPCTSTR)pMotif  );
	}

	// Set the current selection
	if( nCurPos == LB_ERR )
	{
		m_lstbxMotif.SetCurSel( -1 );
	}
	else
	{
		int nPos = LB_ERR;

		pos = m_pStyle->m_StyleMotifs.m_lstMotifs.Find( pCurMotif );
		if( pos )
		{
			nPos = m_lstbxMotif.FindStringExact( -1, (LPCTSTR)pCurMotif );
		}

		if( nPos == LB_ERR )
		{
			nPos = nCurPos;

			int nCount = m_lstbxMotif.GetCount();
			if( nCount > 0 )
			{
				if( nPos > (nCount - 1) )
				{
					nPos = 0;
				}
			}
		}

		m_lstbxMotif.SetCurSel( nPos );
	}

	m_lstbxMotif.UpdateWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::FillBandComboBox

void CStyleDlg::FillBandComboBox()
{
	IDMUSProdNode* pINodeCurSel;
	BSTR bstrNameCurSel;
	CString	strNameCurSel;

	ASSERT( m_pStyle != NULL );

	// Store the current Band selection
	int nCurPos = m_cmbxBand.GetCurSel();
	if( nCurPos != CB_ERR )
	{
		pINodeCurSel = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nCurPos ); 
		if( SUCCEEDED ( pINodeCurSel->GetNodeName ( &bstrNameCurSel ) ) )
		{
			strNameCurSel = bstrNameCurSel;
			::SysFreeString( bstrNameCurSel );
		}
	}

	// Initialize the list box
	m_cmbxBand.ResetContent();

	// Fill the combo box
	IDMUSProdNode* pINode;
	CString strName;
	BSTR bstrName;
	int nPos;

    POSITION pos = m_pStyle->m_StyleBands.m_lstBands.GetHeadPosition();
    while( pos )
    {
        pINode = m_pStyle->m_StyleBands.m_lstBands.GetNext( pos );

		if( SUCCEEDED ( pINode->GetNodeName ( &bstrName ) ) )
		{
			strName = bstrName;
			::SysFreeString( bstrName );
		}

		nPos = m_cmbxBand.AddString( strName );
		if( nPos != CB_ERR )
		{
			m_cmbxBand.SetItemDataPtr( nPos, pINode );
		}
	}

	// Set the current selection
	nPos = 0;

	if( nCurPos != CB_ERR )
	{
		int nStartPos = m_cmbxBand.FindStringExact( 0, strNameCurSel );

		nPos = nStartPos;
		while( nPos != CB_ERR )
		{
			pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
			if( pINode == pINodeCurSel )
			{
				break;
			}

			nPos = m_cmbxBand.FindStringExact( nPos, strNameCurSel );
			if( nPos <= nStartPos )
			{
				// Already searched through entire list
				// Force error
				nPos = CB_ERR;
			}
		}
		if( nPos == CB_ERR )
		{
			nPos = nCurPos;
		}
	}
	int nCount = m_cmbxBand.GetCount();
	if( nCount > 0 )
	{
		if( nPos > (nCount - 1) )
		{
			nPos = 0;
		}
	}
	m_cmbxBand.SetCurSel( nPos );
	OnSelChangeBandCombo();
}


#define NBR_EMBELLISHMENT_FLAGS	4
#define NBR_CHORD_CHANGE_FLAGS	3

static LONG lEmbFlags[NBR_EMBELLISHMENT_FLAGS] = { EMB_INTRO, EMB_FILL, EMB_BREAK, EMB_END };
static char chEmbText[NBR_EMBELLISHMENT_FLAGS]  = { 'I', 'F', 'B', 'E' } ; 

static char chChordChangeText[NBR_CHORD_CHANGE_FLAGS]  = { 'W', 'H', 'Q' } ; 

/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::DrawPatternItem

void CStyleDlg::DrawPatternItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	// Get the Pattern
	CDirectMusicPattern* pPattern = (CDirectMusicPattern *)lpDrawItemStruct->itemData; 
	ASSERT( pPattern != NULL );

	// Get the DC
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC == NULL )
	{
		return;
	}

    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
    int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

    CRect rect( 0, 0, nWidth, nHeight );

	CDC dc;
	CBitmap bmp;
	CString strText;
	int i;

	if( dc.CreateCompatibleDC( pDC )
	&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
	{
		CFont* pFontOld = dc.SelectObject( m_pFont );
		CBitmap* pBitmapOld = dc.SelectObject( &bmp );
        int nBkModeOld = dc.SetBkMode( TRANSPARENT );

		TEXTMETRIC tm;
		dc.GetTextMetrics( &tm );
		if( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			if( ::GetFocus() == m_lstbxPattern.GetSafeHwnd() )
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

		// Draw Pattern name
        rect.left  += 3;
        rect.right  = tm.tmAveCharWidth * 17;
        dc.DrawText( pPattern->m_strName, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Pattern length
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 7);
        strText.Format( "%d    ", pPattern->m_wNbrMeasures );
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Pattern time signature
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 11);
		strText.Format( "%d/%d", pPattern->m_TimeSignature.m_bBeatsPerMeasure, pPattern->m_TimeSignature.m_bBeat );
		if( !(pPattern->m_TimeSignature.m_wGridsPerBeat % 3) )
		{
			strText += _T("  3");
		}
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Pattern groove bottom
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 5);
        strText.Format( "%u    ", pPattern->m_bGrooveBottom );
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Pattern groove top
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 10);
        strText.Format( "%u    ", pPattern->m_bGrooveTop );
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Pattern dest groove bottom
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 5);
        strText.Format( "%u    ", pPattern->m_bDestGrooveBottom );
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Pattern dest groove top
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 8);
        strText.Format( "%u    ", pPattern->m_bDestGrooveTop );
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Pattern embellishment flags
        for( i = 0 ;  i < NBR_EMBELLISHMENT_FLAGS ;  i++ )
        {
	        rect.left  = rect.right;
            rect.right = rect.left + tm.tmMaxCharWidth + 1;
            if( pPattern->m_wEmbellishment & lEmbFlags[i] )
            {
                strText = chEmbText[i];
                dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_CENTER | DT_NOPREFIX) );
            }
        }

		// Draw Pattern user-defined embellishment number
		{
			strText.Empty();
			rect.left  = ++rect.right;
			rect.right = rect.left + tm.tmMaxCharWidth * 3;
			if( HIBYTE(pPattern->m_wEmbellishment) )
			{
		        strText.Format( "%d    ", HIBYTE(pPattern->m_wEmbellishment) );
			}
			dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );
		}
		rect.right += tm.tmMaxCharWidth >> 1; 

		// Draw Pattern chord rhythm
		int j;
		int nMaxRight = nWidth - 4;
		rect.InflateRect( 0, -3 );
		int nTickHeight = (rect.Height() >> 1) - 1;
		for( i = 0 ;  i < pPattern->m_wNbrMeasures ; i++ )
		{
			for( j = 0 ;  j < 32 ;  j++ )
			{
				if( j >= pPattern->m_TimeSignature.m_bBeatsPerMeasure )
				{
					break;
				}

				rect.left  = rect.right + 2;
			    rect.right = rect.left + 1;
				if( rect.left >= nMaxRight )
				{
					break;
				}

				if( pPattern->m_pRhythmMap[i] & (1 << j) )
				{
					dc.FillSolidRect( &rect, RGB(0,0,0) );
				}
				else
				{
					rect.InflateRect( 0, -nTickHeight );
					dc.FillSolidRect( &rect, RGB(0,0,0) );
					rect.InflateRect( 0, nTickHeight );
				}
			}
			
			rect.left += 3;
			rect.right += 3;
			if( rect.left >= nMaxRight )
			{
				break;
			}
		}
		rect.InflateRect( 0, 3 );

		// Draw the item
        pDC->BitBlt( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nWidth, nHeight, 
                     &dc, 0, 0, SRCCOPY );

		// Clean up
		dc.SetBkMode( nBkModeOld );
		if( pFontOld )
		{
			dc.SelectObject( pFontOld );
		}
		if( pBitmapOld )
		{
			dc.SelectObject( pBitmapOld );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::DrawMotifItem

void CStyleDlg::DrawMotifItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	// Get the Motif
	CDirectMusicPattern* pMotif = (CDirectMusicPattern *)lpDrawItemStruct->itemData; 
	ASSERT( pMotif != NULL );

	// Get the DC
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC == NULL )
	{
		return;
	}

    int nWidth  = lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left;
    int nHeight = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

    CRect rect( 0, 0, nWidth, nHeight );

	CDC dc;
	CBitmap bmp;
	CString strText;

	if( dc.CreateCompatibleDC( pDC )
	&&  bmp.CreateCompatibleBitmap( pDC, nWidth, nHeight ) )
	{
		CFont* pFontOld = dc.SelectObject( m_pFont );
		CBitmap* pBitmapOld = dc.SelectObject( &bmp );
        int nBkModeOld = dc.SetBkMode( TRANSPARENT );

		TEXTMETRIC tm;
		dc.GetTextMetrics( &tm );
		if( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			if( ::GetFocus() == m_lstbxMotif.GetSafeHwnd() )
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

		// Draw Motif name
        rect.left  += 3;
        rect.right  = tm.tmAveCharWidth * 17;
        dc.DrawText( pMotif->m_strName, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Motif length
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 7);
        strText.Format( "%d    ", pMotif->m_wNbrMeasures );
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw Motif time signature
        rect.left  = ++rect.right;
        rect.right = rect.left + (tm.tmAveCharWidth * 11);
		strText.Format( "%d/%d", pMotif->m_TimeSignature.m_bBeatsPerMeasure, pMotif->m_TimeSignature.m_bBeat );
		if( !(pMotif->m_TimeSignature.m_wGridsPerBeat % 3) )
		{
			strText += _T("  3");
		}
        dc.DrawText( strText, -1, &rect, (DT_SINGLELINE | DT_TOP | DT_LEFT | DT_NOPREFIX) );

		// Draw the item
        pDC->BitBlt( lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nWidth, nHeight, 
                     &dc, 0, 0, SRCCOPY );

		// Clean up
		dc.SetBkMode( nBkModeOld );
		if( pFontOld )
		{
			dc.SelectObject( pFontOld );
		}
		if( pBitmapOld )
		{
			dc.SelectObject( pBitmapOld );
		}
	}
}


BEGIN_MESSAGE_MAP(CStyleDlg, CFormView)
	//{{AFX_MSG_MAP(CStyleDlg)
	ON_COMMAND(IDM_DRAG_MOVE, OnDragRMenuMove)
	ON_COMMAND(IDM_DRAG_COPY, OnDragRMenuCopy)
	ON_COMMAND(IDM_DRAG_CANCEL, OnDragRMenuCancel)
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_EN_KILLFOCUS(IDC_TEMPO, OnKillFocusTempo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TEMPO_SPIN, OnDeltaPosTempoSpin)
	ON_BN_CLICKED(IDC_TIME_SIGNATURE, OnTimeSignature)
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_COMPAREITEM()
	ON_LBN_DBLCLK(IDC_PATTERN_LIST, OnDblClkPatternList)
	ON_LBN_DBLCLK(IDC_MOTIF_LIST, OnDblClkMotifList)
	ON_LBN_SELCHANGE(IDC_MOTIF_LIST, OnSelChangeMotifList)
	ON_LBN_SELCHANGE(IDC_PATTERN_LIST, OnSelChangePatternList)
	ON_CBN_SELCHANGE(IDC_BAND_COMBO, OnSelChangeBandCombo)
	ON_CBN_SETFOCUS(IDC_BAND_COMBO, OnSetFocusBandCombo)
	ON_CBN_KILLFOCUS(IDC_BAND_COMBO, OnKillFocusBandCombo)
	ON_LBN_SETFOCUS(IDC_PATTERN_LIST, OnSetFocusPatternList)
	ON_LBN_KILLFOCUS(IDC_PATTERN_LIST, OnKillFocusPatternList)
	ON_LBN_SETFOCUS(IDC_MOTIF_LIST, OnSetFocusMotifList)
	ON_LBN_KILLFOCUS(IDC_MOTIF_LIST, OnKillFocusMotifList)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_BN_CLICKED(IDC_BAND_DEFAULT, OnBandDefault)
	ON_BN_DOUBLECLICKED(IDC_BAND_DEFAULT, OnDblClkBandDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg diagnostics

#ifdef _DEBUG
void CStyleDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CStyleDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG




/////////////////////////////////////////////////////////////////////////////
// CStyleDlg IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// IUknown CStyleDlg::QueryInterface

HRESULT CStyleDlg::QueryInterface( REFIID riid, LPVOID *ppvObj )
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
// IUnknown CStyleDlg::AddRef

ULONG CStyleDlg::AddRef( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// IUnknown CStyleDlg::Release

ULONG CStyleDlg::Release( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    ASSERT( m_dwRef != 0 );	// m_dwRef should never get to zero.

//  if( m_dwRef == 0 )		   CStyleDlg should be deleted when		
//  {						   control is destroyed. 						
//		delete this;		
//		return 0;
//  }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg IDropSource implementation

/////////////////////////////////////////////////////////////////////////////
// IDropSource CStyleDlg::QueryContinueDrag

HRESULT CStyleDlg::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
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
// IDropSource CStyleDlg::GiveFeedback

HRESULT CStyleDlg::GiveFeedback( DWORD dwEffect )
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
// CStyleDlg IDropTarget implementation

/////////////////////////////////////////////////////////////////////////////
// IDropTarget CStyleDlg::DragEnter

HRESULT CStyleDlg::DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDataObject != NULL );
	ASSERT( m_pIDataObject == NULL );

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
	if( m_pStyle->m_hWndEditor )
	{
		::BringWindowToTop( m_pStyle->m_hWndEditor );
	}

	// Determine effect of drop
	return DragOver( grfKeyState, pt, pdwEffect );
}


/////////////////////////////////////////////////////////////////////////////
// IDropTarget CStyleDlg::DragOver

HRESULT CStyleDlg::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	UNREFERENCED_PARAMETER( pt );

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDataObject != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragShowNolock( FALSE );
	}

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;
	BOOL fWillSetReference;

	if( m_pStyle->CanPasteFromData( m_pIDataObject, &fWillSetReference ) == S_OK )
	{
		if( fWillSetReference )
		{
			dwEffect = DROPEFFECT_COPY;
		}
		else if( grfKeyState & MK_RBUTTON )
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
// IDropTarget CStyleDlg::DragLeave

HRESULT CStyleDlg::DragLeave( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// IDropTarget CStyleDlg::Drop

HRESULT CStyleDlg::Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	UNREFERENCED_PARAMETER( grfKeyState );

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIDataObject != NULL );
	ASSERT( m_pIDataObject == pIDataObject );
	ASSERT( m_pStyle != NULL );

	if( m_pDragImage )
	{
		// Hide the feedback image
		m_pDragImage->DragLeave( GetDesktopWindow () );
	}

	// Set default values
	HRESULT hr = S_OK;
	*pdwEffect = DROPEFFECT_NONE;

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

	if( m_dwOverDragEffect & DROPEFFECT_MOVE 
	&&  m_pIDragNode )
	{
		// No point in moving node to same Style
		hr = E_FAIL;
	}
	else
	{
		if( m_dwOverDragEffect != DROPEFFECT_NONE )
		{
			// Paste data
			hr = m_pStyle->PasteFromData( pIDataObject );
			if( SUCCEEDED ( hr ) )
			{
				*pdwEffect = m_dwOverDragEffect;
			}
		}
	}

	// Cleanup
	DragLeave();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg message handlers


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::Create

BOOL CStyleDlg::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
{
	// Create Font
	m_pFont = new CFont;
	if( m_pFont == NULL )
	{
		return FALSE;
	}
	if( m_pFont->CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 							 DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
							 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) == FALSE )
	{
		return FALSE;
	}

	if( !CFormView::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext ) )
	{
		return FALSE;
	}

	// Create the splitter bar
	m_wndSplitter.Create( this );
	m_wndSplitter.SetTopBorder( GetTopBorder() );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnInitialUpdate

void CStyleDlg::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	CFormView::OnInitialUpdate();
	
	HRESULT hr = ::RegisterDragDrop( GetSafeHwnd(), (IDropTarget *)this );
	ASSERT(SUCCEEDED(hr));

	// Subclass controls
	m_cmbxBand.SubclassDlgItem( IDC_BAND_COMBO, this );
	m_cmbxBand.m_pStyleDlg = this;
	m_lstbxPattern.SubclassDlgItem( IDC_PATTERN_LIST, this );
	m_lstbxPattern.m_pStyleDlg = this;
	m_lstbxMotif.SubclassDlgItem( IDC_MOTIF_LIST, this );
	m_lstbxMotif.m_pStyleDlg = this;

	m_spinTempo.SetRange( DMUS_TEMPO_MIN, DMUS_TEMPO_MAX );
	m_editTempo.LimitText( 6 ); // 350.00

	IDMUSProdNode* pINode = m_pStyle->GetActiveBand();
	if( pINode == NULL )
	{
		pINode = m_pStyle->GetTheDefaultBand();
	}

	RefreshControls( SSE_ALL );

	if( pINode )
	{
		SelectBand( pINode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDestroy

void CStyleDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Delete the time signature button's bitmap
	HBITMAP hBitmap = m_btnTimeSignature.GetBitmap();
	if( hBitmap )
	{
		::DeleteObject( hBitmap );
	}

	// Delete the font
	if( m_pFont )
	{
		m_pFont->DeleteObject();
		delete m_pFont;
		m_pFont = NULL;
	}

	HRESULT hr = ::RevokeDragDrop( GetSafeHwnd() );
	ASSERT( SUCCEEDED( hr ) );
	
	CFormView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDragRMenuMove

void CStyleDlg::OnDragRMenuMove() 
{
	m_dwDragRMenuEffect = DROPEFFECT_MOVE;	
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDragRMenuCopy

void CStyleDlg::OnDragRMenuCopy() 
{
	m_dwDragRMenuEffect = DROPEFFECT_COPY;	
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDragRMenuCancel

void CStyleDlg::OnDragRMenuCancel() 
{
	m_dwDragRMenuEffect = DROPEFFECT_NONE;	
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::GetTopBorder

int CStyleDlg::GetTopBorder()
{
	int nTopBorder = 54;

	CWnd* pWnd = GetDlgItem( IDC_NAME_PROMPT );
	if( pWnd )
	{
		RECT rect;

		pWnd->GetClientRect( &rect );
		pWnd->ClientToScreen( &rect );
		ScreenToClient( &rect );
		nTopBorder = rect.bottom + STYLE_DLG_BORDER;
	}

	return nTopBorder;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnSize

void CStyleDlg::OnSize( UINT nType, int cx, int cy ) 
{
    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow(m_wndSplitter) )
	{
		return;
	}

	ASSERT( m_pStyle != NULL );

	// Determine top border
	int nTopBorder = GetTopBorder();

	CRect rect;

	// Set default splitter position
	if( m_pStyle->m_nSplitterYPos == 0 )
	{
		GetClientRect( &rect );
		m_pStyle->m_nSplitterYPos = rect.Height() -
								    ((rect.Height() - nTopBorder) >> 2) +
									nTopBorder;
		m_pStyle->m_nSplitterYPos = max( m_pStyle->m_nSplitterYPos, nTopBorder + SPLITTER_HEIGHT + 1 ); 
	}

	// Resize the separator line
	m_staticSeparator.GetClientRect( &rect );
	m_staticSeparator.ClientToScreen( &rect );
	ScreenToClient( &rect );
	m_staticSeparator.MoveWindow( 0, rect.top - 1, cx, 2, TRUE );

	// Determine splitter bar position
	int nSplitterYPos = min( m_pStyle->m_nSplitterYPos, cy - (SPLITTER_HEIGHT + STYLE_DLG_BORDER + 4) );
	if( nSplitterYPos != m_pStyle->m_nSplitterYPos )
	{
		int nMinSplitterYPos = max( nSplitterYPos, nTopBorder + SPLITTER_HEIGHT + 1 ); 
		if( nMinSplitterYPos > nSplitterYPos )
		{
			nSplitterYPos = m_pStyle->m_nSplitterYPos;
		}
	}
	
	// Move the splitter bar
    m_wndSplitter.MoveWindow( STYLE_DLG_BORDER, nSplitterYPos,
							  cx - (STYLE_DLG_BORDER << 1), SPLITTER_HEIGHT, TRUE );

	int nX  = STYLE_DLG_BORDER;
	int nCX = cx - (STYLE_DLG_BORDER << 1);

	// Recalc size of Pattern listbox
	int nY  = nTopBorder;
	int nCY = nSplitterYPos - nTopBorder;

    m_lstbxPattern.MoveWindow( nX, nY, nCX, nCY, TRUE );

	// Recalc size of Motif listbox
	nY  = nSplitterYPos + SPLITTER_HEIGHT;
	nCY = cy - ((nSplitterYPos + SPLITTER_HEIGHT) + STYLE_DLG_BORDER);

    m_lstbxMotif.MoveWindow( nX, nY, nCX, nCY, TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnKillFocusTempo

void CStyleDlg::OnKillFocusTempo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	CString strNewTempo;

	m_editTempo.GetWindowText( strNewTempo );

	// Strip leading and trailing spaces
	strNewTempo.TrimRight();
	strNewTempo.TrimLeft();

	if( strNewTempo.IsEmpty() )
	{
		SetTempoControlText();
	}
	else
	{
		double dblNewTempo;

		TCHAR* pszTempo;
		pszTempo = new TCHAR[strNewTempo.GetLength() + 1];

		if( pszTempo )
		{
			pszTempo[0] = 0;

			if( _stscanf( strNewTempo, "%lf%s", &dblNewTempo, pszTempo ) )
			{
				// Check bounds
				if( dblNewTempo > DMUS_TEMPO_MAX )
				{
					dblNewTempo = DMUS_TEMPO_MAX;
				}
				else if( dblNewTempo < DMUS_TEMPO_MIN )
				{
					dblNewTempo = DMUS_TEMPO_MIN;
				}

				m_pStyle->SetTempo( dblNewTempo, FALSE );
			}

			SetTempoControlText();	// Make sure edit control
									// reflects the tempo of m_pStyle
			delete pszTempo;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDeltaPosTempoSpin

void CStyleDlg::OnDeltaPosTempoSpin( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusTempo();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	double dblNewTempo = floor( m_pStyle->m_dblTempo + pNMUpDown->iDelta );

	// If too small, wrap to DMUS_TEMPO_MAX
	if( dblNewTempo < DMUS_TEMPO_MIN )
	{
		dblNewTempo = DMUS_TEMPO_MAX;
	}
	// If too large, wrap to DMUS_TEMPO_MIN
	else if( dblNewTempo > DMUS_TEMPO_MAX )
	{
		dblNewTempo = DMUS_TEMPO_MIN;
	}

	// Sync edit control so OnKillFocus doesn't change it back
	CString strTempo;
	strTempo.Format( "%.2f", dblNewTempo );
	m_editTempo.SetWindowText( strTempo );

	m_pStyle->SetTempo( dblNewTempo, FALSE );
	SetTempoControlText();	// Make sure edit control
							// reflects results of SetTempo()
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnTimeSignature

void CStyleDlg::OnTimeSignature() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	CTimeSignatureDlg tsDlg;

	tsDlg.m_TimeSignature = m_pStyle->m_TimeSignature;
	tsDlg.m_nContext = IDS_STYLE_TEXT;

	if( tsDlg.DoModal() == IDOK )
	{
		// Update time signature
		m_pStyle->SetTimeSignature( tsDlg.m_TimeSignature, FALSE );

		// Update bitmap on time signature button
		SetTimeSignatureBitmap();
	}

	m_btnTimeSignature.SetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnSelChangePatternList

void CStyleDlg::OnSelChangePatternList() 
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDirectMusicPattern* pPattern = NULL;

	// Get the Pattern
	int nPos = m_lstbxPattern.GetCurSel();
	if( nPos != LB_ERR )
	{
		pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos ); 
	}

	if( pPattern )
	{
		// Sync property sheet
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				pPattern->OnShowProperties();
			}

			RELEASE( pIPropSheet );
		}

		pPattern->OnNodeSelChanged( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDblClkPatternList

void CStyleDlg::OnDblClkPatternList() 
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDirectMusicPattern* pPattern = NULL;

	// Get the Pattern
	int nPos = m_lstbxPattern.GetCurSel();
	if( nPos != LB_ERR )
	{
		pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos ); 
	}

	if( pPattern )
	{
		theApp.m_pStyleComponent->m_pIFramework->OpenEditor( pPattern );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::PlayMotif

void CStyleDlg::PlayMotif( void ) 
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDirectMusicPattern* pMotif = NULL;

	// Get the Motif
	int nPos = m_lstbxMotif.GetCurSel();
	if( nPos != LB_ERR )
	{
		pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos ); 
	}

	if( pMotif )
	{
		// Trigger Motif to play
		ASSERT( pMotif->m_pStyle != NULL );
		ASSERT( pMotif->m_pStyle->m_pIDMStyle != NULL );

		IDirectMusicSegmentState* pIDMSegmentState;
		MUSIC_TIME mtTimeNow;

		theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtTimeNow );
		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIDMPerformance->GetSegmentState( &pIDMSegmentState, mtTimeNow ) ) )
		{
			IDirectMusicSegment* pIDMSegmentMotif;

			BSTR bstrName = pMotif->m_strName.AllocSysString();

			if( m_pStyle->m_pIDMStyle->GetMotif( bstrName, &pIDMSegmentMotif ) == S_OK )
			{
				theApp.m_pStyleComponent->m_pIDMPerformance->PlaySegment( pIDMSegmentMotif,
																		 (DMUS_SEGF_SECONDARY | pMotif->m_dwResolution),
																		  0, NULL );
				RELEASE( pIDMSegmentMotif );
			}

			SysFreeString( bstrName );
			RELEASE( pIDMSegmentState );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnSelChangeMotifList

void CStyleDlg::OnSelChangeMotifList() 
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDirectMusicPattern* pMotif = NULL;

	// Get the Motif
	int nPos = m_lstbxMotif.GetCurSel();
	if( nPos != LB_ERR )
	{
		pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos ); 
	}

	if( pMotif )
	{
		// Sync property sheet
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				pMotif->OnShowProperties();
			}

			RELEASE( pIPropSheet );
		}

		pMotif->OnNodeSelChanged( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDblClkMotifList

void CStyleDlg::OnDblClkMotifList() 
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDirectMusicPattern* pMotif = NULL;

	// Get the Motif
	int nPos = m_lstbxMotif.GetCurSel();
	if( nPos != LB_ERR )
	{
		pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos ); 
	}

	if( pMotif )
	{
		theApp.m_pStyleComponent->m_pIFramework->OpenEditor( pMotif );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnSelChangeBandCombo

void CStyleDlg::OnSelChangeBandCombo() 
{
	ASSERT( m_pStyle != NULL );

	IDMUSProdNode* pINode = NULL;

	// Get the Band
	int nPos = m_cmbxBand.GetCurSel();
	if( nPos != CB_ERR )
	{
		pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
	}

	if( pINode )
	{
		// Set the Band "Default" control
		IDMUSProdBandEdit* pIBandEdit;
		BOOL fDefaultFlag = FALSE;

		if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
		{
			if( FAILED ( pIBandEdit->GetDefaultFlag( &fDefaultFlag ) ) )
			{
				fDefaultFlag = FALSE;
			}

			RELEASE( pIBandEdit );
		}
		m_checkBandDefault.SetCheck( fDefaultFlag );

		// Sync property sheet
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				IDMUSProdPropPageObject* pIPageObject;

				if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
				{
					pIPageObject->OnShowProperties();
					RELEASE( pIPageObject );
				}
			}

			RELEASE( pIPropSheet );
		}

		// Set the Style's active Band
		m_pStyle->SetActiveBand( pINode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnMeasureItem

void CStyleDlg::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_PATTERN_LIST:
		case IDC_MOTIF_LIST:
		{
			ASSERT( m_pFont != NULL );

			TEXTMETRIC tm;
			CDC* pDC;

			pDC = GetDC();
			if( pDC )
			{
				CFont* pFontOld = pDC->SelectObject( m_pFont );
				pDC->GetTextMetrics( &tm );
				lpMeasureItemStruct->itemHeight = tm.tmHeight + 1;
				if( pFontOld )
				{
					pDC->SelectObject( pFontOld );
				}
				ReleaseDC( pDC );
				return;
			}
		}
	}
	
	CFormView::OnMeasureItem( nIDCtl, lpMeasureItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnCompareItem

int CStyleDlg::OnCompareItem( int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern1 = (CDirectMusicPattern *)lpCompareItemStruct->itemData1;
			CDirectMusicPattern* pPattern2 = (CDirectMusicPattern *)lpCompareItemStruct->itemData2;

			// Place embellishments at the bottom of the list
			if( pPattern1->m_wEmbellishment == EMB_NORMAL
			&&  pPattern2->m_wEmbellishment != EMB_NORMAL )
			{
				return -1;
			}
			if( pPattern1->m_wEmbellishment != EMB_NORMAL
			&&  pPattern2->m_wEmbellishment == EMB_NORMAL )
			{
				return 1;
			}

			return pPattern1->m_strName.CompareNoCase( pPattern2->m_strName );
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif1 = (CDirectMusicPattern *)lpCompareItemStruct->itemData1;
			CDirectMusicPattern* pMotif2 = (CDirectMusicPattern *)lpCompareItemStruct->itemData2;

			return pMotif1->m_strName.CompareNoCase( pMotif2->m_strName );
		}
	}
	
	return CFormView::OnCompareItem( nIDCtl, lpCompareItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDrawItem

void CStyleDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	switch( nIDCtl )
	{
		case IDC_PATTERN_LIST:
		case IDC_MOTIF_LIST:
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

            if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE
            ||  lpDrawItemStruct->itemAction & ODA_SELECT 
            ||  lpDrawItemStruct->itemAction & ODA_FOCUS )
			{
				switch( nIDCtl )
				{
					case IDC_PATTERN_LIST:
					{
						DrawPatternItem( lpDrawItemStruct );
						if( ::GetFocus() == m_lstbxPattern.GetSafeHwnd() )
						{
							if( lpDrawItemStruct->itemAction & ODA_FOCUS )
							{
								InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
								pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
								InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
							}
						}
						break;
					}

					case IDC_MOTIF_LIST:
					{
						DrawMotifItem( lpDrawItemStruct );
						if( ::GetFocus() == m_lstbxMotif.GetSafeHwnd() )
						{
							if( lpDrawItemStruct->itemAction & ODA_FOCUS )
							{
								InflateRect( &lpDrawItemStruct->rcItem, -1, -1 );
								pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
								InflateRect( &lpDrawItemStruct->rcItem, 1, 1 );
							}
						}
						break;
					}
				}
			}

			return;
		}
	}
	
	CFormView::OnDrawItem( nIDCtl, lpDrawItemStruct );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnUpdateEditCut

void CStyleDlg::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pStyle != NULL );

	// Initialize menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		int nPos;

		case IDC_BAND_COMBO:
		{
			IDMUSProdNode* pINode;

			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					if( pINode->CanCut() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}

		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern;

			nPos = m_lstbxPattern.GetCurSel();
			if( nPos != LB_ERR )
			{
				pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
				if( pPattern )
				{
					if( pPattern->CanCut() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif;

			nPos = m_lstbxMotif.GetCurSel();
			if( nPos != LB_ERR )
			{
				pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos );
				if( pMotif )
				{
					if( pMotif->CanCut() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnEditCut

void CStyleDlg::OnEditCut() 
{
	ASSERT( m_pStyle != NULL );

	// Handle menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		IDataObject* pIDataObject;
		int nPos;

		case IDC_BAND_COMBO:
		{
			IDMUSProdNode* pINode;

			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					if( pINode->CanCut() == S_OK )
					{
						if( SUCCEEDED ( pINode->CreateDataObject( &pIDataObject ) ) )
						{
							if( theApp.PutDataInClipboard( pIDataObject, pINode ) )
							{
								pINode->DeleteNode( FALSE );
								m_cmbxBand.SetFocus();
							}

							RELEASE( pIDataObject );
						}
					}
				}
			}
			break;
		}

		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern;

			nPos = m_lstbxPattern.GetCurSel();
			if( nPos != LB_ERR )
			{
				pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
				if( pPattern )
				{
					if( pPattern->CanCut() == S_OK )
					{
						if( SUCCEEDED ( pPattern->CreateDataObject( &pIDataObject ) ) )
						{
							if( theApp.PutDataInClipboard( pIDataObject, pPattern ) )
							{
								pPattern->DeleteNode( FALSE );
								m_lstbxPattern.SetFocus();
							}

							RELEASE( pIDataObject );
						}
					}
				}
			}
			break;
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif;

			nPos = m_lstbxMotif.GetCurSel();
			if( nPos != LB_ERR )
			{
				pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos );
				if( pMotif )
				{
					if( pMotif->CanCut() == S_OK )
					{
						if( SUCCEEDED ( pMotif->CreateDataObject( &pIDataObject ) ) )
						{
							if( theApp.PutDataInClipboard( pIDataObject, pMotif ) )
							{
								pMotif->DeleteNode( FALSE );
								m_lstbxMotif.SetFocus();
							}

							RELEASE( pIDataObject );
						}
					}
				}
			}
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnUpdateEditCopy

void CStyleDlg::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pStyle != NULL );

	// Initialize menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		int nPos;

		case IDC_BAND_COMBO:
		{
			IDMUSProdNode* pINode;

			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					if( pINode->CanCopy() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}

		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern;

			nPos = m_lstbxPattern.GetCurSel();
			if( nPos != LB_ERR )
			{
				pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
				if( pPattern )
				{
					if( pPattern->CanCopy() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif;

			nPos = m_lstbxMotif.GetCurSel();
			if( nPos != LB_ERR )
			{
				pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos );
				if( pMotif )
				{
					if( pMotif->CanCopy() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnEditCopy

void CStyleDlg::OnEditCopy() 
{
	ASSERT( m_pStyle != NULL );

	// Handle menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		IDataObject* pIDataObject;
		int nPos;

		case IDC_BAND_COMBO:
		{
			IDMUSProdNode* pINode;

			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					if( pINode->CanCopy() == S_OK )
					{
						if( SUCCEEDED ( pINode->CreateDataObject( &pIDataObject ) ) )
						{
							theApp.PutDataInClipboard( pIDataObject, pINode );

							RELEASE( pIDataObject );
						}
					}
				}
			}
			break;
		}

		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern;

			nPos = m_lstbxPattern.GetCurSel();
			if( nPos != LB_ERR )
			{
				pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
				if( pPattern )
				{
					if( pPattern->CanCopy() == S_OK )
					{
						if( SUCCEEDED ( pPattern->CreateDataObject( &pIDataObject ) ) )
						{
							theApp.PutDataInClipboard( pIDataObject, pPattern );

							RELEASE( pIDataObject );
						}
					}
				}
			}
			break;
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif;

			nPos = m_lstbxMotif.GetCurSel();
			if( nPos != LB_ERR )
			{
				pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos );
				if( pMotif )
				{
					if( pMotif->CanCopy() == S_OK )
					{
						if( SUCCEEDED ( pMotif->CreateDataObject( &pIDataObject ) ) )
						{
							theApp.PutDataInClipboard( pIDataObject, pMotif );

							RELEASE( pIDataObject );
						}
					}
				}
			}
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnUpdateEditPaste

void CStyleDlg::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pStyle != NULL );

	IDataObject* pIDataObject;

	// Get the IDataObject
	if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
	{
		// Determine if Style can paste this object
		BOOL fWillSetReference;
		HRESULT hr = m_pStyle->CanPasteFromData( pIDataObject, &fWillSetReference );
		
		RELEASE( pIDataObject );
		
		if( hr == S_OK )
		{
			pCmdUI->Enable( TRUE );
			return;
		}
	}
	
	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnEditPaste

void CStyleDlg::OnEditPaste() 
{
	ASSERT( m_pStyle != NULL );

	IDataObject* pIDataObject;

	// Get the IDataObject
	if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
	{
		// Determine if Style can paste this object
		m_pStyle->PasteFromData( pIDataObject );
		
		RELEASE( pIDataObject );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnUpdateEditInsert

void CStyleDlg::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pStyle != NULL );

	// Initialize menu item based on "Edit Menu" control
	if( m_nEditMenuCtrlID )
	{
		pCmdUI->Enable( TRUE );
		return;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnEditInsert

void CStyleDlg::OnEditInsert() 
{
	ASSERT( m_pStyle != NULL );

	// Handle menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		case IDC_BAND_COMBO:
			m_pStyle->m_StyleBands.InsertChildNode( NULL );
			m_cmbxBand.SetFocus();
			break;

		case IDC_PATTERN_LIST:
			m_pStyle->m_StylePatterns.InsertChildNode( NULL );
			m_lstbxPattern.SetFocus();
			break;

		case IDC_MOTIF_LIST:
			m_pStyle->m_StyleMotifs.InsertChildNode( NULL );
			m_lstbxMotif.SetFocus();
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnUpdateEditDelete

void CStyleDlg::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	ASSERT( m_pStyle != NULL );

	// Initialize menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		int nPos;

		case IDC_BAND_COMBO:
		{
			IDMUSProdNode* pINode;

			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					if( pINode->CanDelete() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}

		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern;

			nPos = m_lstbxPattern.GetCurSel();
			if( nPos != LB_ERR )
			{
				pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
				if( pPattern )
				{
					if( pPattern->CanDelete() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif;

			nPos = m_lstbxMotif.GetCurSel();
			if( nPos != LB_ERR )
			{
				pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos );
				if( pMotif )
				{
					if( pMotif->CanDelete() == S_OK )
					{
						pCmdUI->Enable( TRUE );
						return;
					}
				}
			}
			break;
		}
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnEditDelete

void CStyleDlg::OnEditDelete() 
{
	ASSERT( m_pStyle != NULL );

	// Handle menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		int nPos;

		case IDC_BAND_COMBO:
		{
			IDMUSProdNode* pINode;

			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					if( pINode->CanDelete() == S_OK )
					{
						pINode->DeleteNode( TRUE );
						m_cmbxBand.SetFocus();
					}
				}
			}
			break;
		}

		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern;

			nPos = m_lstbxPattern.GetCurSel();
			if( nPos != LB_ERR )
			{
				pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
				if( pPattern )
				{
					if( pPattern->CanDelete() == S_OK )
					{
						pPattern->DeleteNode( TRUE );
						m_lstbxPattern.SetFocus();
					}
				}
			}
			break;
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif;

			nPos = m_lstbxMotif.GetCurSel();
			if( nPos != LB_ERR )
			{
				pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos );
				if( pMotif )
				{
					if( pMotif->CanDelete() == S_OK )
					{
						pMotif->DeleteNode( TRUE );
						m_lstbxMotif.SetFocus();
					}
				}
			}
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnViewProperties

BOOL CStyleDlg::OnViewProperties( void )
{
	IDMUSProdPropSheet* pIPropSheet;

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( FAILED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return FALSE;
	}

	if( pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return TRUE;
	}

	BOOL fSuccess = FALSE;

	// Handle menu item based on "Edit Menu" control
	switch( m_nEditMenuCtrlID )
	{
		int nPos;

		case IDC_BAND_COMBO:
		{
			IDMUSProdNode* pINode;

			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					IDMUSProdPropPageObject* pIPageObject;

					if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
					{
						if( SUCCEEDED ( pIPageObject->OnShowProperties() ) )
						{
							fSuccess = TRUE;
						}
						RELEASE( pIPageObject );
					}
				}
			}
			break;
		}

		case IDC_PATTERN_LIST:
		{
			CDirectMusicPattern* pPattern;

			nPos = m_lstbxPattern.GetCurSel();
			if( nPos != LB_ERR )
			{
				pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos );
				if( pPattern )
				{
					if( SUCCEEDED ( pPattern->OnShowProperties() ) )
					{
						fSuccess = TRUE;
					}
				}
			}
			break;
		}

		case IDC_MOTIF_LIST:
		{
			CDirectMusicPattern* pMotif;

			nPos = m_lstbxMotif.GetCurSel();
			if( nPos != LB_ERR )
			{
				pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos );
				if( pMotif )
				{
					if( SUCCEEDED ( pMotif->OnShowProperties() ) )
					{
						fSuccess = TRUE;
					}
				}
			}
			break;
		}

		default:
			if( SUCCEEDED ( m_pStyle->OnShowProperties() ) )
			{
				fSuccess = TRUE;
			}
			break;
	}

	RELEASE( pIPropSheet );

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnBandDefault

void CStyleDlg::OnBandDefault() 
{
	ASSERT( m_pStyle != NULL );
	
	BOOL fDefaultFlag = m_checkBandDefault.GetCheck();
	if( fDefaultFlag == 0 )
	{
		// Cannot uncheck the default Band!!!
		m_checkBandDefault.SetCheck( 1 );
		return;
	}

	// Get the Band
	int nPos = m_cmbxBand.GetCurSel();
	if( nPos != CB_ERR )
	{
		IDMUSProdNode* pIBandNode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
		if( pIBandNode )
		{
			// Set the Style's default Band
			m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_STYLE_DEFAULT_BAND );
			m_pStyle->SetDefaultBand( pIBandNode, FALSE );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnDblClkBandDefault

void CStyleDlg::OnDblClkBandDefault() 
{
	// Throw it away
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnSetFocusBandCombo

void CStyleDlg::OnSetFocusBandCombo() 
{
	m_nEditMenuCtrlID = IDC_BAND_COMBO;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnKillFocusBandCombo

void CStyleDlg::OnKillFocusBandCombo() 
{
	if( m_nEditMenuCtrlID == IDC_BAND_COMBO )
	{
		// Get the control that stole the focus
		CWnd* pWnd = CWnd::GetFocus();

		if( pWnd == NULL
		||  pWnd->GetParent() == NULL )
		{
			return;
		}

		m_nEditMenuCtrlID = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnSetFocusPatternList

void CStyleDlg::OnSetFocusPatternList() 
{
	m_nEditMenuCtrlID = IDC_PATTERN_LIST;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnKillFocusPatternList

void CStyleDlg::OnKillFocusPatternList() 
{
	if( m_nEditMenuCtrlID == IDC_PATTERN_LIST )
	{
		// Get the control that stole the focus
		CWnd* pWnd = CWnd::GetFocus();

		if( pWnd == NULL
		||  pWnd->GetParent() == NULL )
		{
			return;
		}

		m_nEditMenuCtrlID = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnSetFocusMotifList

void CStyleDlg::OnSetFocusMotifList() 
{
	m_nEditMenuCtrlID = IDC_MOTIF_LIST;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnKillFocusMotifList

void CStyleDlg::OnKillFocusMotifList() 
{
	if( m_nEditMenuCtrlID == IDC_MOTIF_LIST )
	{
		// Get the control that stole the focus
		CWnd* pWnd = CWnd::GetFocus();

		if( pWnd == NULL
		||  pWnd->GetParent() == NULL )
		{
			return;
		}

		m_nEditMenuCtrlID = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnContextMenu

void CStyleDlg::OnContextMenu( CWnd* pWnd, CPoint point ) 
{
	ASSERT( pWnd != NULL );
	ASSERT( m_pStyle != NULL );

	switch( pWnd->GetDlgCtrlID() )
	{
		HINSTANCE hInstance;
		UINT nResourceId;
		HMENU hMenu;
		HMENU hMenuPopup;
		BOOL fOutside;
		int nPos;

		case IDC_BAND_COMBO:
			nPos = m_cmbxBand.GetCurSel();
			if( nPos != CB_ERR )
			{
				m_cmbxBand.SetFocus(); 

				IDMUSProdNode* pINode = (IDMUSProdNode *)m_cmbxBand.GetItemDataPtr( nPos ); 
				if( pINode )
				{
					if( SUCCEEDED ( pINode->GetRightClickMenuId(&hInstance, &nResourceId) ) )
					{
						hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );
						if( hMenu )
						{
							hMenuPopup = ::GetSubMenu( hMenu, 0 );
							pINode->OnRightClickMenuInit( hMenuPopup );

							m_pINodeRightMenu = pINode;
							::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
											  point.x, point.y, 0, GetSafeHwnd(), NULL );

							DestroyMenu( hMenu );
						}
					}
				}
			}
			break;
			 
		case IDC_PATTERN_LIST:
		{
			CPoint ptLstBx( point );

			m_lstbxPattern.ScreenToClient( &ptLstBx );
			nPos = m_lstbxPattern.ItemFromPoint( ptLstBx, fOutside );
			if( fOutside == FALSE
			&&  nPos != LB_ERR )
			{
				m_lstbxPattern.SetCurSel( nPos ); 
				m_lstbxPattern.SetFocus(); 

				CDirectMusicPattern* pPattern = (CDirectMusicPattern *)m_lstbxPattern.GetItemData( nPos ); 
				if( pPattern )
				{
					if( SUCCEEDED ( pPattern->GetRightClickMenuId(&hInstance, &nResourceId) ) )
					{
						hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );
						if( hMenu )
						{
							hMenuPopup = ::GetSubMenu( hMenu, 0 );
							pPattern->OnRightClickMenuInit( hMenuPopup );

							m_pINodeRightMenu = pPattern;
							::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
											  point.x, point.y, 0, GetSafeHwnd(), NULL );

							DestroyMenu( hMenu );
						}
					}
				}
			}
			break;
		}
			 
		case IDC_MOTIF_LIST:
		{
			CPoint ptLstBx( point );

			m_lstbxMotif.ScreenToClient( &ptLstBx );
			nPos = m_lstbxMotif.ItemFromPoint( ptLstBx, fOutside );
			if( fOutside == FALSE
			&&  nPos != LB_ERR )
			{
				m_lstbxMotif.SetCurSel( nPos ); 
				m_lstbxMotif.SetFocus(); 

				CDirectMusicPattern* pMotif = (CDirectMusicPattern *)m_lstbxMotif.GetItemData( nPos ); 
				if( pMotif )
				{
					if( SUCCEEDED ( pMotif->GetRightClickMenuId(&hInstance, &nResourceId) ) )
					{
						hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );
						if( hMenu )
						{
							hMenuPopup = ::GetSubMenu( hMenu, 0 );
							pMotif->OnRightClickMenuInit( hMenuPopup );

							m_pINodeRightMenu = pMotif;
							::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
											  point.x, point.y, 0, GetSafeHwnd(), NULL );

							DestroyMenu( hMenu );
						}
					}
				}
			}
			break;
		}

		default:
			if( SUCCEEDED ( m_pStyle->GetRightClickMenuId(&hInstance, &nResourceId) ) )
			{
				hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE(nResourceId) );
				if( hMenu )
				{
					hMenuPopup = ::GetSubMenu( hMenu, 0 );
					m_pStyle->OnRightClickMenuInit( hMenuPopup );

					m_pINodeRightMenu = m_pStyle;
					::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
									  point.x, point.y, 0, GetSafeHwnd(), NULL );

					DestroyMenu( hMenu );
				}
			}
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnCommand

BOOL CStyleDlg::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	if( HIWORD(wParam) == 0 )	// menu command
	{
		if( m_pINodeRightMenu )	// can receive menu commands when m_pINodeRightMenu is NULL
		{
			m_pINodeRightMenu->OnRightClickMenuSelect( LOWORD(wParam) );
			m_pINodeRightMenu = NULL ;
			return TRUE;
		}
	}
	
	return CFormView::OnCommand( wParam, lParam );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnMouseMove

void CStyleDlg::OnMouseMove( UINT nFlags, CPoint point ) 
{
	CFormView::OnMouseMove( nFlags, point );
	
	if( CWnd::GetCapture() == this )
	{
		if( m_dwMouseDownButton != 0 )
		{
			// See if user is moving the mouse
			if( m_rectMouseDown.PtInRect( point ) == FALSE )
			{
				// Start drag drop
				DoDrag( m_pIMouseDownNode, m_dwMouseDownButton ); 
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnCaptureChanged

void CStyleDlg::OnCaptureChanged( CWnd *pWnd ) 
{
	if( pWnd == this )
	{
		return;
	}
	
	m_dwMouseDownButton = 0;
	m_pIMouseDownNode = NULL;
	m_rectMouseDown.SetRectEmpty();

	CFormView::OnCaptureChanged( pWnd );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnLButtonDown

void CStyleDlg::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pStyle != NULL );

	if( m_dwMouseDownButton == 0 )
	{
		// Sync property sheet
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				m_pStyle->OnShowProperties();
			}

			RELEASE( pIPropSheet );
		}

		// First set capture
		SetCapture();

		// Now store fields used when drag drop is initiated
		m_dwMouseDownButton = MK_LBUTTON;
		m_pIMouseDownNode = m_pStyle;

		m_rectMouseDown.left = point.x - 5;
		m_rectMouseDown.top = point.y - 5;
		m_rectMouseDown.right = point.x + 5;
		m_rectMouseDown.bottom = point.y + 5;
	}

	CFormView::OnLButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnLButtonUp

void CStyleDlg::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	if( m_dwMouseDownButton == MK_LBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}
	
	CFormView::OnLButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnRButtonDown

void CStyleDlg::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pStyle != NULL );

	if( m_dwMouseDownButton == 0 )
	{
		// Sync property sheet
		IDMUSProdPropSheet* pIPropSheet;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			if( pIPropSheet->IsShowing() == S_OK )
			{
				m_pStyle->OnShowProperties();
			}

			RELEASE( pIPropSheet );
		}

		// First set capture
		SetCapture();

		// Now store fields used when drag drop is initiated
		m_dwMouseDownButton = MK_RBUTTON;
		m_pIMouseDownNode = m_pStyle;

		m_rectMouseDown.left = point.x - 5;
		m_rectMouseDown.top = point.y - 5;
		m_rectMouseDown.right = point.x + 5;
		m_rectMouseDown.bottom = point.y + 5;
	}

	CFormView::OnRButtonDown( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::OnRButtonUp

void CStyleDlg::OnRButtonUp( UINT nFlags, CPoint point ) 
{
	if( m_dwMouseDownButton == MK_RBUTTON )
	{
		if( CWnd::GetCapture() == this )
		{
			::ReleaseCapture();
		}
	}

	CFormView::OnRButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg::HandleKeyDown

BOOL CStyleDlg::HandleKeyDown( MSG* pMsg ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case VK_DELETE:
					if( m_nEditMenuCtrlID )
					{
						OnEditDelete();
						return TRUE;
					}
					break;

				case VK_INSERT:
					if( m_nEditMenuCtrlID )
					{
						OnEditInsert();
						return TRUE;
					}
					break;

				case 0x43:  // VK_C		(Copy)
					if( m_nEditMenuCtrlID )
					{
						if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
						{
							OnEditCopy();
							return TRUE;
						}
					}
					break;

				case 0x56:	// VK_V		(Paste)
					if( m_nEditMenuCtrlID )
					{
						if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
						{
							OnEditPaste();
							return TRUE;
						}
					}
					break;

				case 0x58:	// VK_X		(Cut)
					if( m_nEditMenuCtrlID )
					{
						if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
						{
							OnEditCut();
							return TRUE;
						}
					}
					break;

				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_TEMPO: 
								SetTempoControlText();
								return TRUE;
						}
					}
					break;
				}

				case VK_RETURN:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_TEMPO:
								::SetFocus( NULL );
								pWnd->SetFocus();
								return TRUE;

							case IDC_TIME_SIGNATURE:
								if( m_btnTimeSignature.GetButtonStyle() & BS_DEFPUSHBUTTON )
								{
									OnTimeSignature();
									return TRUE;
								}
								break;

							case IDC_PATTERN_LIST:
								OnDblClkPatternList();
								return TRUE;

							case IDC_MOTIF_LIST:
								OnDblClkMotifList();
								return TRUE;
						}
					}
					break;
				}
			}
			break;
	}
	
	return FALSE;
}
