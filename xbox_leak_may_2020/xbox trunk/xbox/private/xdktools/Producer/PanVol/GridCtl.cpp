// GridCtl.cpp : Implementation of the CGridCtrl ActiveX Control class.

#include "stdafx.h"
#include "Grid.h"
#include "GridCtl.h"
#include "GridPpg.h"
#include "Channel.h"
#include <afxext.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma optimize("", off)

IMPLEMENT_DYNCREATE(CGridCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CGridCtrl, COleControl)
	//{{AFX_MSG_MAP(CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CGridCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CGridCtrl)
	DISP_FUNCTION(CGridCtrl, "BringToFront", BringToFront, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CGridCtrl, "MoveButton", MoveButton, VT_EMPTY, VTS_I4 VTS_I2 VTS_I2)
	DISP_FUNCTION(CGridCtrl, "RemoveButton", RemoveButton, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CGridCtrl, "RemoveAllButtons", RemoveAllButtons, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGridCtrl, "SelectButton", SelectButton, VT_EMPTY, VTS_I4 VTS_BOOL)
	DISP_FUNCTION(CGridCtrl, "SetEnabled", SetEnabled, VT_EMPTY, VTS_I4 VTS_BOOL)
	DISP_FUNCTION(CGridCtrl, "AddButton", AddButton, VT_BOOL, VTS_UNKNOWN)
	DISP_FUNCTION(CGridCtrl, "UpdateButton", UpdateButton, VT_EMPTY, VTS_I4)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CGridCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CGridCtrl, COleControl)
	//{{AFX_EVENT_MAP(CGridCtrl)
	EVENT_CUSTOM("CursorOver", FireCursorOver, VTS_I4)
	EVENT_CUSTOM("Move", FireMove, VTS_I4  VTS_I2  VTS_I2)
	EVENT_CUSTOM("Update", FireUpdate, VTS_UNKNOWN  VTS_BOOL  VTS_BOOL)
	EVENT_CUSTOM("SelectPChannel", FireSelectPChannel, VTS_I4  VTS_BOOL  VTS_BOOL)
	EVENT_CUSTOM("SaveUndoState", FireSaveUndoState, VTS_NONE)
	EVENT_CUSTOM("DisplayStatus", FireDisplayStatus, VTS_I2)
	EVENT_CUSTOM("RightClick", FireRightClick, VTS_I2  VTS_I2)
	EVENT_CUSTOM("DeleteChannel", FireDeleteChannel, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

BEGIN_PROPPAGEIDS(CGridCtrl, 1)
	PROPPAGEID(CGridPropPage::guid)
END_PROPPAGEIDS(CGridCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CGridCtrl, "GRID.GridCtrl.1",
	0xeab0cd46, 0x9459, 0x11d0, 0x8c, 0x10, 0, 0xa0, 0xc9, 0x2e, 0x1c, 0xac)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CGridCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DGrid =
		{ 0xeab0cd44, 0x9459, 0x11d0, { 0x8c, 0x10, 0, 0xa0, 0xc9, 0x2e, 0x1c, 0xac } };
const IID BASED_CODE IID_DGridEvents =
		{ 0xeab0cd45, 0x9459, 0x11d0, { 0x8c, 0x10, 0, 0xa0, 0xc9, 0x2e, 0x1c, 0xac } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwGridOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE;/* |
	OLEMISC_RECOMPOSEONRESIZE;*/

IMPLEMENT_OLECTLTYPE(CGridCtrl, IDS_GRID, _dwGridOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl::CGridCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CGridCtrl

BOOL CGridCtrl::CGridCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_GRID,
			IDB_GRID,
			afxRegApartmentThreading,
			_dwGridOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl::CGridCtrl - Constructor

CGridCtrl::CGridCtrl() : m_EBW( 2 ), m_bSized(FALSE), m_bSendUpdate(true) 
{
	InitializeIIDs(&IID_DGrid, &IID_DGridEvents);

	// initialize member variables
	m_pBtnLst = NULL;
	m_pDrag = NULL;
	m_pOver = NULL;
	m_bmGridTile.LoadBitmap( IDB_GRIDTILE );

	// load the up/down button bitmaps
	m_bmBtnUp.LoadBitmap(IDB_BTNUP);
	m_bmBtnDn.LoadBitmap(IDB_BTNDN);
	m_bmDrums.LoadBitmap(IDB_DRUMS);
	m_bmMask.LoadBitmap(IDB_MASK);

	m_fMoved = FALSE;

	CRect rect(0, 0, 8, 8);
	m_pTracker = new CRectTracker(&rect, CRectTracker::dottedLine);
}


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl::~CGridCtrl - Destructor

CGridCtrl::~CGridCtrl()
{
	CChannel  *pScan = m_pBtnLst;
	CChannel  *pDelete;

	while( pScan )
	{
		pDelete = pScan;
		pScan = pScan->m_pNext;
		delete pDelete;
	}
	m_bmGridTile.DeleteObject();
		
	// Destroy the button bitmaps
	m_bmBtnUp.DeleteObject();
	m_bmBtnDn.DeleteObject();
	m_bmDrums.DeleteObject();	
	m_bmMask.DeleteObject();

	if(m_pTracker)
	{
		delete m_pTracker;
		m_pTracker = NULL;
	}
}


//
//  EmbossRect()       raised = 0: sunken, width = 2
//                              1: raised, width = 2
//                              2: sunken, width = 1
//                              3: raised, width = 1
//
void CGridCtrl::EmbossRect( HDC hdc, const CRect *pRect, char raised )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int     nModeOld  = 0 ;
	HPEN    hPenOld   = 0 ;
	HPEN    hPenWhite = 0, hPenGray = 0 ;
	int     pen_width, offset ;
	CRect   Rect = *pRect;

    if( raised > 1 ) {
        pen_width = 1 ;
        offset    = 0 ;
        raised   -= 2 ;
    } else {
        pen_width = 2 ;
        offset    = 1 ;
    }

    if( (hPenWhite = CreatePen(PS_SOLID, pen_width, RGB(255,255,255)))
    &&  (hPenGray  = CreatePen(PS_SOLID, pen_width, RGB(128,128,128))) ) {
        nModeOld = SetROP2( hdc, R2_COPYPEN ) ;

        if( pen_width == 2 ) {
            Rect.InflateRect( -1, -1 ) ;
        } else {
            Rect.right-- ;
            Rect.bottom-- ;
        }

        if( raised )
            hPenOld = (HPEN) SelectObject( hdc, hPenWhite ) ;
        else
            hPenOld = (HPEN) SelectObject( hdc, hPenGray ) ;
        MoveToEx( hdc, Rect.left, Rect.bottom, NULL ) ;
        LineTo( hdc, Rect.left, Rect.top ) ;
        LineTo( hdc, Rect.right, Rect.top ) ;

        if( raised )
            SelectObject( hdc, hPenGray ) ;
        else
            SelectObject( hdc, hPenWhite ) ;
        MoveToEx( hdc, Rect.right, Rect.top+offset, NULL ) ;
        LineTo( hdc, Rect.right, Rect.bottom ) ;
        LineTo( hdc, Rect.left+offset, Rect.bottom ) ;

        if( pen_width == 2 ) 
            Rect.InflateRect( 1, 1 ) ;

        if( raised ) {
            SetPixel( hdc, Rect.right-offset, Rect.top, RGB(128,128,128) ) ;
            SetPixel( hdc, Rect.left, Rect.bottom-offset, RGB(128,128,128) ) ;
        } else {
            SetPixel( hdc, Rect.right-offset, Rect.top, RGB(255,255,255) ) ;
            SetPixel( hdc, Rect.left, Rect.bottom-offset, RGB(255,255,255) ) ;
        }

        if( pen_width == 1 ) { 
            Rect.right++ ;
            Rect.bottom++ ;
        }
    }

    if( nModeOld )
        SetROP2( hdc, nModeOld ) ;
    if( hPenOld )
        SelectObject( hdc, hPenOld ) ;
    if( hPenWhite )
        DeleteObject( hPenWhite ) ;
    if( hPenGray )
        DeleteObject( hPenGray ) ;
}


CRect *CGridCtrl::GetAdjustedArea( CChannel *pc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_rcAdjusted = m_rcExtent;
	m_rcAdjusted.left = 1;//m_EBW;
	m_rcAdjusted.top = m_EBW + 1;
	m_rcAdjusted.right -= 2 * m_EBW + pc->m_cxSize;
	m_rcAdjusted.bottom -= 2 * m_EBW + pc->m_cySize;
	return &m_rcAdjusted;
}


void CGridCtrl::_BringToFront( CChannel *pc )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pc == NULL )  
		return;

	if( pc->m_pNext == NULL )  
		return;

	if( pc->m_pNext != m_pDrag )  
	{
		_BringToFront( pc->m_pNext );
		return;
	}

	if( pc->m_pNext->m_pNext == NULL )  
		return;

	pc->m_pNext = SwapAdjacentNodes( pc->m_pNext, pc->m_pNext->m_pNext );
	_BringToFront( pc->m_pNext );
}


void CGridCtrl::SendToBack()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel* pScan = m_pBtnLst;
	m_pDrag->m_pNext = pScan;
	m_pBtnLst = m_pDrag;

	// then splice it before the drag button
	while( pScan )
	{
		if( pScan->m_pNext == m_pDrag )  {
			pScan->m_pNext = NULL;
		}
		pScan = pScan->m_pNext;
}	}


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl::OnDraw - Drawing function

void CGridCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPen     *pPenOld, *pPen;
	CBitmap  *pbmOld;
	CBitmap  *pbmFb;  // ptr to frame buffer bitmap
	CDC      *pfbdc;
	CBrush   *pbrush, *pbrushOld;
	short     nHSCntr, nVSCntr;  // horizontal/vertical snap to grid center

	// create frame buffer bitmap
	pbmFb = new CBitmap;
	m_rcExtent.left = 0;
	m_rcExtent.top = 0;

	if(m_bSized == FALSE)
	{
		m_rcExtent.right = rcBounds.Width();
		m_rcExtent.bottom = rcBounds.Height();
	}
	pbmFb->CreateCompatibleBitmap( pdc, m_rcExtent.right, m_rcExtent.bottom );

	// create memory device context for the frame buffer
	pfbdc = new CDC;
	if(pfbdc->CreateCompatibleDC( pdc ) == 0 ||pfbdc->m_hDC == NULL)
	{
		delete pfbdc;
		delete pbmFb;
		return;
	}

	pbmOld = pfbdc->SelectObject( pbmFb );

	// paint the background with a grid pattern
	pbrush = new CBrush;
	pbrush->CreatePatternBrush( &m_bmGridTile );
	pbrushOld = pfbdc->SelectObject( pbrush );
	pfbdc->PatBlt( rcInvalid.left, rcInvalid.top,
		rcInvalid.right - rcInvalid.left, rcInvalid.bottom - rcInvalid.top, PATCOPY );
	pfbdc->SelectObject( pbrushOld );
	pbrush->DeleteObject();
	delete pbrush;

	// draw an embossed border
	EmbossRect( pfbdc->m_hDC, &m_rcExtent, (char) 0 );

	// draw the cross hairs
	pPen = new CPen;
	pPen->CreatePen( PS_SOLID, 0, RGB(128, 128, 0) ) ;
    pPenOld = pfbdc->SelectObject( pPen ) ;
	nHSCntr = (m_rcExtent.Width() / 2) - ((m_rcExtent.Width() / 2) % 8);
	nVSCntr = (m_rcExtent.Height() / 2) - ((m_rcExtent.Height() / 2) % 8);
    MoveToEx( pfbdc->m_hDC, nHSCntr, 2, NULL );
    LineTo( pfbdc->m_hDC, nHSCntr, m_rcExtent.Height() - 2 );
    MoveToEx( pfbdc->m_hDC, 2, nVSCntr, NULL );
    LineTo( pfbdc->m_hDC, m_rcExtent.Width() - 2, nVSCntr );
    pfbdc->SelectObject( pPenOld );
	pPen->DeleteObject();
    delete pPen;

	// draw the buttons
	if( m_pBtnLst )  
	{
		m_pBtnLst->Draw( pfbdc, rcInvalid );
	}

	// blt the frame buffer onto the screen
	pdc->BitBlt( rcBounds.left, rcBounds.top, rcBounds.right, rcBounds.bottom,
		pfbdc, 0, 0, SRCCOPY );

	// cleanup and destroy the frame buffer
	pfbdc->SelectObject( pbmOld );
	pbmFb->DeleteObject();
	delete pbmFb;
	delete pfbdc;
}


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl::DoPropExchange - Persistence support

void CGridCtrl::DoPropExchange(CPropExchange* pPX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

}


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl::OnResetState - Reset control to default state

void CGridCtrl::OnResetState()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel* pScan = m_pBtnLst;
	CChannel* pDelete;

	while( pScan )
	{
		pDelete = pScan;
		pScan = pScan->m_pNext;
		delete pDelete;
	}
	m_pBtnLst = NULL;
	m_pDrag = NULL;
	m_pOver = NULL;
	InvalidateControl( &m_rcExtent );

	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl::AboutBox - Display an "About" box to the user

void CGridCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog dlgAbout(IDD_ABOUTBOX_GRID);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers
BOOL CGridCtrl::AddButton(LPUNKNOWN pInstrumentItem) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(pInstrumentItem);

	CChannel  *pScan, *pAppend;
	CRect      rect;

	IDMUSProdBandPChannel* pInstrument = NULL;
	if(FAILED(pInstrumentItem->QueryInterface(IID_IDMUSProdBandPChannel, (void**)&pInstrument)))
		return FALSE;

	int nID = -1;
	pInstrument->GetPChannelNumber(&nID);
	
	if(nID  < 0)  
		return FALSE;

	int nPan = 0;
	pInstrument->GetPan(&nPan);
	nPan = max( 0, min( 127, nPan ) );

	int nVolume = 0;
	pInstrument->GetVolume(&nVolume);
	nVolume = max( 0, min( 127, nVolume ) );

	if( m_pBtnLst == NULL )
	{
		// first button on the control
		m_pBtnLst = new CChannel(this);
		// We're out of memory??
		if(m_pBtnLst == NULL)
			return FALSE;
		m_pBtnLst->Create(pInstrument);
		pAppend = m_pBtnLst;
	}
	else  
	{
		// append a new button to the end of the list
		pScan = m_pBtnLst;
		while( pScan )
		{
			pAppend = pScan;
			pScan = pScan->m_pNext;
		}
		pAppend->m_pNext = new CChannel(this);
		// We're out of memory??
		if(pAppend->m_pNext == NULL)
			return FALSE;

		pAppend->m_pNext->Create(pInstrument);
		pAppend = pAppend->m_pNext;
		pAppend->Move(GetAdjustedArea(pAppend));
	}

	// position it and draw it
	if( pAppend )  
	{
		pAppend->Move( GetAdjustedArea( pAppend ) );
		pAppend->GetButtonArea( &rect );
		InvalidateControl( &rect );
		return TRUE;
	}

	return FALSE;
}

void CGridCtrl::RemoveAllButtons()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel* pScan = m_pBtnLst;

	while(pScan)
	{
		m_pBtnLst = pScan->m_pNext;
		delete pScan;
		pScan = m_pBtnLst;
	}

	InvalidateControl(NULL);
}



void  CGridCtrl::RemoveButton(long nId)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel* pScan = m_pBtnLst;
	CChannel* pPrev = NULL;

	while(pScan)
	{
		if(pScan->GetId() == nId)
		{
			if(pPrev != NULL)
				pPrev->m_pNext = pScan->m_pNext; // Removed the button from the list
			else
				m_pBtnLst = pScan->m_pNext;		 // The first node matches

			delete pScan;
			pScan = NULL;
			break;
		}
		pPrev = pScan;
		pScan = pScan->m_pNext;
	}

	m_pDrag = NULL;

	InvalidateControl(NULL);
	
}

void CGridCtrl::SelectAllButtons()
{
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel*	pScan = m_pBtnLst;

	while(pScan)
	{
		SelectButton(pScan->GetId(), true);
		FireUpdate( pScan->m_pInstrument, true, true);
		pScan = pScan->m_pNext;
	}
}

void CGridCtrl::UnselectAllButtons()
{
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel*	pScan = m_pBtnLst;

	while(pScan)
	{
		pScan->SetSelected(false);
		pScan->m_fTop = FALSE;
		pScan = pScan->m_pNext;
	}
	
	FireUpdate(NULL, false, false);
}


void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel*	pScan = m_pBtnLst;
	CRect		rect;

	m_bSendUpdate = true;
	bool bTrack = true;
	bool bAnySelected = false;

	while(pScan)
	{
		if(pScan->HitTest(point) && pScan->IsSelected())
		{
			bAnySelected = true;
			bTrack = false;
			MarkButtonForMove(pScan, point);
			m_bMultiSelected = true;
			break;
		}

		pScan = pScan->m_pNext;
	}


	pScan = m_pBtnLst;
	while(pScan && !bAnySelected)
	{
		if(pScan->HitTest(point))
		{
			bTrack = false;
			bAnySelected = true;
			
			if(!(nFlags & MK_CONTROL) && pScan->IsSelected() == false)
			{
				UnselectAllButtons();
				MarkButtonForMove(pScan, point);
				m_bMultiSelected = false;
				break;
			}
			else if(pScan->IsSelected() == true)// User must have started dragging the button
			{
				MarkButtonForMove(pScan, point);
				m_bMultiSelected = true;
				break;
			}
			else if((nFlags & MK_CONTROL))
			{
				MarkButtonForMove(pScan, point);
				pScan->m_bDragButton = false;
				pScan->SetSelected(true);
				m_bMultiSelected = true;
				break;
			}
		}

		pScan = pScan->m_pNext;
	}

	// No click on any button
	if(bAnySelected == false)
	{
		ReleaseCapture();
		UnselectAllButtons();
		m_bMultiSelected = false;
	}

	if(bTrack)
	{
		m_pTracker->TrackRubberBand(this, point);
		// Test every channel button for a mouse hit
		pScan = m_pBtnLst;
		m_pDrag = NULL;
		CChannel* pFirstButton = NULL;
		while(pScan)
		{
			pScan->m_bDragButton = false;
			
			if(m_pTracker->HitTest(pScan->GetMiddle()) != CRectTracker::hitNothing)
			{
				if(pFirstButton == NULL)
					pFirstButton = pScan;

				m_bMultiSelected = true;
				pScan->SetSelected(true);
				FireUpdate( pScan->m_pInstrument, true, false);
			}
			else
				pScan->SetSelected(false);

			pScan = pScan->m_pNext;
		}
		if(pFirstButton)
			FireUpdate( pFirstButton->m_pInstrument, true, true);
	} 

	// Set the drag Distances for all selected buttons
	pScan = m_pBtnLst;
	while(m_pDrag && pScan)
	{
		bool bBroughtToFront = false;
		if(pScan->IsSelected())
		{
			pScan->GetButtonArea(&rect);
			CRect draggedButtonRect;
			m_pDrag->GetButtonArea(&draggedButtonRect);
			pScan->m_nDraggedPan = m_pDrag->GetPan() - pScan->GetPan();
			pScan->m_nDraggedVol = m_pDrag->GetVol() - pScan->GetVol();

			if(!pScan->m_fTop)
			{
				BringToFront(pScan->GetId());
				bBroughtToFront = true;
				pScan = m_pBtnLst;
			}
		}

		if(!bBroughtToFront)
			pScan= pScan->m_pNext;
	}

	// repaint the button
	InvalidateControl();

	COleControl::OnLButtonDown(nFlags, point);
}

void CGridCtrl::MarkButtonForMove(CChannel* pButton, CPoint point)
{
	pButton->SetSelected(true);
	BringToFront(pButton->GetId());
		
	CRect rect;
	pButton->GetButtonArea( &rect );

	// remember where on the button the mouse landed
	m_ptTouch = point;
	m_ptTouch.x -= rect.left;
	m_ptTouch.y -= rect.top;
	pButton->m_bDragButton = true;
	m_pDrag = pButton;
	m_fMoved = FALSE;
}

void CGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// No PChannels?
	if(m_pBtnLst == NULL)
	{
		return;
	}
	
	CRect  rect;
	ReleaseCapture();

	// Do a hit test for proper selection
	CChannel* pScan = m_pBtnLst;
	CChannel* pOver = NULL;

	if(!(nFlags & MK_CONTROL) && m_bMultiSelected == false)
	{
		if(pScan->HitTest(point) && pScan->IsSelected() == false)
		{
			m_pDrag = NULL;
			UnselectAllButtons();
		}
	}

	pScan = m_pBtnLst;
	while( pScan )  
	{
		if(pScan->IsSelected())
		{
			if(m_pDrag && ((nFlags & MK_CONTROL) || m_bMultiSelected == true))
			{
				FireUpdate( pScan->m_pInstrument, true, false);
			}
			else
			{
				FireUpdate( pScan->m_pInstrument, (nFlags & MK_CONTROL), true);
			}
		}
		
		pScan = pScan->m_pNext;
	}

	if(m_pDrag)
	{
		FireUpdate( m_pDrag->m_pInstrument, true, true);
		m_pDrag->m_pInstrument->SyncChanges(true);
	}

	COleControl::OnLButtonUp(nFlags, point);
}

void CGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	static CPoint oldPoint = CPoint(0, 0);
	if(point == oldPoint)
		return;

	oldPoint = point;

	CPoint     pt;
	CRect      rect;
	CChannel*  pScan = m_pBtnLst;
	
	static bool bSyncChannels = true;

	if(nFlags & MK_LBUTTON)
	{
		SetCapture();
		pScan = m_pBtnLst;

		// Saving the undo state should happen only once during the mouse move
		if(m_pDrag && m_bSendUpdate)
		{
			if(FAILED(m_pDrag->m_pInstrument->SyncChanges(true)))
				bSyncChannels = false;
			else
				bSyncChannels = true;

			FireSaveUndoState();
		}

		while(pScan)
		{
			if(pScan->IsSelected() == true && pScan == m_pDrag)
			{
				int nOldDragPan = pScan->GetPan();
				int nOldDragVol = pScan->GetVol();

				pt = point - m_ptTouch;
				pt.x = min( max( 0, pt.x ), m_rcExtent.right - 2 * m_EBW - pScan->m_cxSize );
				pt.y = min( max( 0, pt.y ), m_rcExtent.bottom - 2 * m_EBW - pScan->m_cySize );
				
				pScan->Move( GetAdjustedArea( pScan ), &pt );
				pScan->SetPanVol(pScan->GetPan(), pScan->GetVol());

				if(nOldDragPan != m_pDrag->GetPan() || nOldDragVol != m_pDrag->GetVol())
				{
					m_bSendUpdate = true;
					FireDisplayStatus(pScan->GetId());
				}
				else
					m_bSendUpdate = false;

				if(bSyncChannels)
					m_pDrag->m_pInstrument->SyncChanges(false);
						
				// Move all the other pchannels
				CChannel* pChannel = m_pBtnLst;
				while(pChannel)
				{
					if(pChannel->IsSelected())
					{
						int nNewPan = m_pDrag->GetPan() - pChannel->m_nDraggedPan;
						int nNewVol = m_pDrag->GetVol() - pChannel->m_nDraggedVol;

						nNewPan = max( 0, min( 127, nNewPan ) );
						nNewVol = max( 0, min( 127, nNewVol ) );

						CRect rect;

						// Move the pchannel only if it has really moved
						// This will save us some unnecessary updates
						if(nNewPan != pChannel->GetPan() || nNewVol != pChannel->GetVol())
						{
							pChannel->SetPanVol(nNewPan, nNewVol);
							pChannel->Move(GetAdjustedArea(pChannel));
						
							if(bSyncChannels)
								pChannel->m_pInstrument->SyncChanges(false);
						}
					}

					pChannel = pChannel->m_pNext;
				}
				 
				// remember that the button has been moved
				m_fMoved = TRUE;
				break;

			}
			pScan = pScan->m_pNext;
		}

		if(m_pDrag)
		{
			m_bSendUpdate = false;
		}

		InvalidateControl();
	}
}

void CGridCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_bSized == TRUE)
	{
		return;
	}
	
	m_bSized = TRUE;

	// the ClassWizard says call your parent first
	COleControl::OnSize(nType, cx, cy);

	// now save the new window size
	m_rcExtent.left = 0;
	m_rcExtent.top = 0;
	m_rcExtent.right = cx;
	m_rcExtent.bottom = cy;

	// recalculate all button positions
	CChannel* pEnum = m_pBtnLst;
	while( pEnum )
	{
		pEnum->Move( GetAdjustedArea( pEnum ) );
		pEnum = pEnum->m_pNext;
	}
	InvalidateControl( &m_rcExtent );
}


void CGridCtrl::BringToFront(long nID) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel* pScan = GetChannelByID(nID);

	if( pScan )  
	{
		pScan->m_fTop = TRUE;

		// special case for head of list
		if( m_pBtnLst == pScan  &&  m_pBtnLst->m_pNext )
		{
			m_pBtnLst = SwapAdjacentNodes( m_pBtnLst, m_pBtnLst->m_pNext );
		}

		// temporarily assign the button to be the drag button and then call
		// the private method to bring it to the front
		m_pOver = pScan;
		CChannel* pTemp = m_pDrag;
		m_pDrag = pScan;
		_BringToFront( m_pBtnLst );
		m_pDrag = pTemp;

		// repaint
		InvalidateControl( NULL );
	}	

}


void CGridCtrl::MoveButton(long nID, short nPan, short nVolume) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CChannel* pScan = GetChannelByID(nID);
	
	if(pScan)
	{
		CRect rect;
		// invalidate current area under the button
		pScan->GetButtonArea( &rect );
		InvalidateControl( &rect );

		// position it and redraw it
		//pScan->SetPanVol( nPan, nVolume );
		pScan->Move( GetAdjustedArea( pScan ) );
		pScan->GetButtonArea( &rect );
		InvalidateControl( &rect );
	}
}



BOOL CGridCtrl::OnEraseBkgnd(CDC* pDC) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return FALSE;
}




void CGridCtrl::SelectButton(long nID, BOOL bSelection) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CChannel* pScan = GetChannelByID(nID);
	
	if(pScan)
	{
		CRect  rect;
		pScan->SetSelected(bSelection ? true : false);
		pScan->GetButtonArea( &rect );
		InvalidateControl( &rect );
	}
}


CChannel* CGridCtrl::GetChannelByID(long nID)
{
	CChannel  *pScan = m_pBtnLst;
	while( pScan )
	{
		if( nID == pScan->GetId() )  
			return pScan;
		
		pScan = pScan->m_pNext;
	}

	return NULL;
}

void CGridCtrl::SetEnabled(long nID, BOOL bEnable) 
{
	CChannel* pChannel = GetChannelByID(nID);
	
	if(pChannel)
	{
		CRect  rect;
		pChannel->SetEnabled(bEnable ? true : false);
		pChannel->GetButtonArea( &rect );
		InvalidateControl( &rect );
	}
}

void CGridCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pBtnLst == NULL)
	{
		return;
	}

	CChannel*	pScan = m_pBtnLst;
	CRect		rect;

	bool bAnySelected = false;

	while(pScan)
	{
		if(pScan->HitTest(point) && pScan->IsSelected())
		{
			bAnySelected = true;
			MarkButtonForMove(pScan, point);
			m_bMultiSelected = true;
			break;
		}

		pScan = pScan->m_pNext;
	}


	pScan = m_pBtnLst;
	while(pScan)
	{
		if(pScan->HitTest(point))
		{
			bAnySelected = true;
			
			if(!(nFlags & MK_CONTROL) && pScan->IsSelected() == false)
			{
				UnselectAllButtons();
				MarkButtonForMove(pScan, point);
				m_bMultiSelected = false;
				break;
			}
			else if(pScan->IsSelected() == true)// User must have started dragging the button
			{
				MarkButtonForMove(pScan, point);
				m_bMultiSelected = true;
				break;
			}
			else if((nFlags & MK_CONTROL) && pScan->m_fTop == TRUE)
			{
				MarkButtonForMove(pScan, point);
				pScan->m_bDragButton = false;
				pScan->SetSelected(true);
				m_bMultiSelected = true;
				break;
			}
		}

		pScan = pScan->m_pNext;
	}

	// No click on any button
	if(bAnySelected == false)
	{
		UnselectAllButtons();
		m_bMultiSelected = false;
	}

	// special case for head of list
	if(m_pBtnLst->IsSelected() == true && m_pBtnLst == m_pDrag &&  m_pBtnLst->m_pNext )  
	{
		m_pBtnLst = SwapAdjacentNodes( m_pBtnLst, m_pBtnLst->m_pNext );
	}

	_BringToFront( m_pBtnLst );	

	InvalidateControl();
	
	COleControl::OnRButtonDown(nFlags, point);
}

void CGridCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	OnLButtonUp(nFlags, point);
}

void CGridCtrl::UpdateButton(long nPChannel) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CChannel* pScan = GetChannelByID(nPChannel);
	if(pScan)
	{
		pScan->Refresh();
		MoveButton(nPChannel, pScan->GetPan(),pScan->GetVol());
	}
}

CChannel* CGridCtrl::SwapAdjacentNodes( CChannel *pParent, CChannel *pChild )
{
	pParent->m_pNext = pChild->m_pNext;
	pChild->m_pNext = pParent;
	return pChild;
}

#pragma optimize("", on)

void CGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Adjust the volume and pan values
	switch(nChar)
	{
		case VK_UP: 
		{
			if(nRepCnt > 1)
			{
				break;
			}

			StepVolume(true);
			InvalidateControl();
			break;
		}

		case VK_DOWN:
		{
			if(nRepCnt > 1)
			{
				break;
			}

			StepVolume(false);
			InvalidateControl();
			break;
		}
		case VK_LEFT:
		{
			if(nRepCnt > 1)
			{
				break;
			}

			StepPan(false);
			InvalidateControl();
			break;
		}
		case VK_RIGHT:
		{
			if(nRepCnt > 1)
			{
				break;
			}

			StepPan(true);
			InvalidateControl();
			break;
		}

		case 'a':
		case 'A':
		{
			if(nFlags & VK_CONTROL) 
			{
				SelectAllButtons();
			}
			break;
		}

		case VK_DELETE:
		{
			FireDeleteChannel();
			break;
		}

		default:
		{
			break;
		}
	}
	
	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CGridCtrl::StepVolume(bool bStepUp)
{
	// Check if any buttons are selectd
	if(AreAnyChannelsSelected() == false)
	{
		return;
	}

	// Save the undo state
	FireSaveUndoState();

	int nIncrement = bStepUp ? 1 : -1;

	CChannel* pScan = m_pBtnLst;
	while(pScan)  
	{
		if(pScan->IsSelected())
		{
			int nOldVol = pScan->GetVol();
			int nPan = pScan->GetPan();
			int nNewVol = nOldVol + nIncrement;

			if(nNewVol >=0 && nNewVol <= 127)
			{
				pScan->SetEnabled(true);
				pScan->SetPanVol(nPan, nNewVol);
				long lId = pScan->GetId();
				MoveButton(lId, pScan->GetPan(),pScan->GetVol());
				
				CRect rect;				
				pScan->GetButtonArea( &rect );
				InvalidateControl( &rect );

				FireMove(lId, nPan, nNewVol);
				FireUpdate( pScan->m_pInstrument, true, false);
			}
		}
		
		pScan = pScan->m_pNext;
	}
}

void CGridCtrl::StepPan(bool bStepRight)
{
	// Check if any buttons are selectd
	if(AreAnyChannelsSelected() == false)
	{
		return;
	}

	// Save the undo state
	FireSaveUndoState();

	int nIncrement = bStepRight ? 1 : -1;

	CChannel* pScan = m_pBtnLst;
	while(pScan)  
	{
		if(pScan->IsSelected())
		{
			int nVol = pScan->GetVol();
			int nOldPan = pScan->GetPan();
			int nNewPan = nOldPan + nIncrement;

			if(nNewPan >=0 && nNewPan <= 127)
			{
				pScan->SetEnabled(true);
				pScan->SetPanVol(nNewPan, nVol);
				long lId = pScan->GetId();
				MoveButton(lId, pScan->GetPan(),pScan->GetVol());
				CRect rect;				
				pScan->GetButtonArea( &rect );
				InvalidateControl( &rect );
				FireMove(lId, nNewPan, nVol);
				FireUpdate( pScan->m_pInstrument, true, false);
			}
		}
		
		pScan = pScan->m_pNext;
	}
}


bool CGridCtrl::AreAnyChannelsSelected()
{
	CChannel* pScan = m_pBtnLst;
	while(pScan)  
	{
		if(pScan->IsSelected())
		{
			return true;
		}

		pScan = pScan->m_pNext;
	}

	return false;
}


void CGridCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Adjust the volume and pan values
	switch(nChar)
	{
		case VK_UP: 
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
		{
			if(nRepCnt > 1)
			{
				break;
			}
			CChannel* pScan = m_pBtnLst;
			while(pScan)  
			{
				if(pScan->IsSelected())
				{
					FireUpdate(pScan->m_pInstrument, true, true);
					pScan->m_pInstrument->SyncChanges(true);
				}

				pScan = pScan->m_pNext;
			}

			break;
		}

	}
	
	
	COleControl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGridCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	FireRightClick(point.x, point.y);

}
