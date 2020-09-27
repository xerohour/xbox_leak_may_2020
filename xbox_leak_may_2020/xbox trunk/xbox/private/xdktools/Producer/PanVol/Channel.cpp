// Channel.cpp: implementation of the CChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Grid.h"
#include "GridCtl.h"
#include "Channel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma optimize("", off)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChannel::CChannel(CGridCtrl* pParent) : m_cxSize( 25 ), m_cySize( 22 ), m_bDrums(false), 
										 m_bSelected(false), m_bDragButton(false), 
										 m_nDraggedPan(0), m_nDraggedVol(0), m_bEnabled(false)
{
	ASSERT(pParent);
	m_pParent = pParent;
}

void CChannel::Create(IDMUSProdBandPChannel* pInstrument)
{
	ASSERT(pInstrument);

	m_pInstrument = pInstrument;

	// set button identification and initial values
	pInstrument->GetPChannelNumber(&m_nId);
	pInstrument->GetPan(&m_nPan);
	pInstrument->GetVolume(&m_nVol);

	// terminate list and set initial state to up
	m_pNext = NULL;
	m_fDepressed = FALSE;
	m_fTop = FALSE;
}

CChannel::~CChannel()
{
	if(m_pInstrument)
		m_pInstrument->Release();
}

//////////////////////////////////////////////////////////////////////
// Member function implementation
//////////////////////////////////////////////////////////////////////
void CChannel::Move( CRect *prcExtent, CPoint *pptNewLoc )
{
	if( pptNewLoc )
	{
		// calculate the new pan and volume values
		m_nPan = (short) ((pptNewLoc->x * 127) / prcExtent->right);
		m_nVol = 127 - (short) ((pptNewLoc->y * 127) / prcExtent->bottom);

		m_nPan = max( 0, min( 127, m_nPan ) );
		m_nVol = max( 0, min( 127, m_nVol ) );

	}

	// set the new screen area variable
	m_rcBtnArea.left = (m_nPan * prcExtent->right) / 127 + prcExtent->left;
	m_rcBtnArea.top = ((127 - m_nVol) * prcExtent->bottom) / 127 + prcExtent->top;
	m_rcBtnArea.right = m_rcBtnArea.left + m_cxSize;
	m_rcBtnArea.bottom = m_rcBtnArea.top + m_cySize;
}


void CChannel::Draw( CDC* pDC, const CRect& rcInvalid )
{
	ASSERT(m_pInstrument);
	m_pInstrument->GetPan(&m_nPan);
	m_pInstrument->GetVolume(&m_nVol);


	// select the down look bitmap if we are being dragged
	if( m_fDepressed )  
	{
		DrawButton(pDC, rcInvalid, &(m_pParent->m_bmBtnDn), true, m_bSelected);
	}
	else  
	{
		BOOL bSelected = FALSE;
		m_pInstrument->IsSelected(&bSelected);
		if(m_bSelected)
			DrawButton(pDC, rcInvalid, &(m_pParent->m_bmMask), false, m_bSelected);
		else
			DrawButton(pDC, rcInvalid, &(m_pParent->m_bmBtnUp), false, m_bSelected);
	}

	// call draw routine for the next button
	if( m_pNext )
		m_pNext->Draw(pDC, rcInvalid);
}

void CChannel::DrawButton(CDC* pDC, const CRect& rcInvalid, CBitmap* pBitmap, bool bDown, bool bSelected)
{
	ASSERT(pDC);

	CRect rcTemp;

	if(rcTemp.IntersectRect(&rcInvalid, &m_rcBtnArea) == 0)
		return;


	if(m_pNext != NULL)
	{
		rcTemp.IntersectRect(&m_rcBtnArea, &(m_pNext->m_rcBtnArea));
		if(rcTemp == m_pNext->m_rcBtnArea)
			return;
	}

	// Create a copy of the original bitmap...we don't want to touch the original
	CBitmap bmpCopy;
	CSize size = pBitmap->GetBitmapDimension();
	bmpCopy.CreateCompatibleBitmap(pDC, (m_rcBtnArea.right - m_rcBtnArea.left), (m_rcBtnArea.bottom - m_rcBtnArea.top));

	// Create a new compatible device context
	CDC* pMemDC = new CDC;
	if(pMemDC->CreateCompatibleDC( NULL ) == 0 || pMemDC->m_hDC == NULL)
	{
		delete pMemDC;
		return;
	}
	
	CBitmap* pbmOldFromMemDC = pMemDC->SelectObject(&bmpCopy);
	CBitmap* pbmOldFromScreenDC = pDC->SelectObject(pBitmap);

	pMemDC->BitBlt( 0, 0, m_cxSize, m_cySize, pDC, 0, 0, SRCCOPY);
	pDC->SelectObject(pbmOldFromScreenDC);

	/*if(m_bDrums)
	{
		DrawDrums(pMemDC, bDown, bSelected);
		DrawNumber(pMemDC, bDown);
	}
	else*/
		DrawNumber(pMemDC, bDown);

	pDC->BitBlt( m_rcBtnArea.left, m_rcBtnArea.top, m_rcBtnArea.right,
				 m_rcBtnArea.bottom, pMemDC, 0, 0, SRCCOPY );

	//Select he old bitmap into the DC and delete the DC
	pMemDC->SelectObject(pbmOldFromMemDC);
	delete pMemDC;
}


void CChannel::DrawDrums(CDC* pDC, bool bDown, bool bSelected)
{
	ASSERT(pDC);
	DWORD dwRop = SRCCOPY;
		
	// NOTE : The Rect shifts for the down state
	CRect* pRect = new CRect( 2 + bDown, 2+ bDown, m_cxSize - 4 + bDown, m_cySize - 4 + bDown);

	// Need a new device context for the source bitmap
	CDC* pSrcDC= new CDC;
	if(pSrcDC->CreateCompatibleDC( NULL ) == 0 || pSrcDC->m_hDC == NULL)
	{
		delete pSrcDC;
		return;
	}

	// Select the drums bitmap into the DC

	CBitmap* pbmoSrc = pSrcDC->SelectObject(&(m_pParent->m_bmDrums));
	
	// Use a different raster operation if the channel is selected
	if(bSelected)
		dwRop = DSTINVERT;

	pDC->BitBlt( pRect->left, pRect->top, pRect->right, pRect->bottom, pSrcDC,	0, 0, dwRop);

	// Select the old bitmap into the DC and delete the DC
	pSrcDC->SelectObject(pbmoSrc);
	delete pSrcDC;
	delete pRect;
}


void CChannel::DrawNumber(CDC* pDC, bool bDown)
{
	ASSERT(pDC);
	TCHAR buf[8];
	
	LOGFONT lfThin;
	ZeroMemory( &lfThin, sizeof(LOGFONT));

	lfThin.lfHeight = 16; 
	lfThin.lfWidth = 5; 
	lfThin.lfWeight = 700; 
	CString sFont;
	sFont.LoadString(IDS_FONT);
	strcpy(lfThin.lfFaceName, (LPCSTR)sFont);

	CFont thinFont;
	thinFont.CreateFontIndirect(&lfThin);

	// NOTE : The Rect shifts for the down state
	CRect* pRect = new CRect( 2 + bDown, 2 + bDown, m_cxSize - 1 + bDown, m_cySize - 1 + bDown);

	// draw the channel number on the button up bitmap
	COLORREF oldColor = NULL;
	if(!m_bEnabled && !m_bSelected)
		oldColor = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT)/*RGB(6, 103, 102)*/);
	else if(m_bEnabled && m_bSelected)
		oldColor = pDC->SetTextColor(RGB(245, 239, 141));
	else if(!m_bEnabled && m_bSelected)
		oldColor = pDC->SetTextColor(RGB(255, 128, 128));

	_itot( m_nId + 1, buf, 10 );
	pDC->SetBkMode( TRANSPARENT );
	CFont* pOldFont = pDC->SelectObject(&thinFont);
	pDC->DrawText( buf, lstrlen( buf ), pRect, DT_CENTER | DT_VCENTER |	DT_SINGLELINE );
	
	if(pOldFont)
		pDC->SelectObject(pOldFont);

	if(oldColor)
		pDC->SetTextColor(oldColor);

	delete pRect;
}

void CChannel::SetSelected(bool bSelection)
{
	m_pInstrument->SetSelected(bSelection);
	m_bSelected = bSelection;
}

bool CChannel::IsSelected()
{
	BOOL bSelected = false;
	m_pInstrument->IsSelected(&bSelected);
    return bSelected ? true : false;
}

CPoint CChannel::GetMiddle()
{
	CPoint point(m_rcBtnArea.right - (m_rcBtnArea.right - m_rcBtnArea.left)/2, m_rcBtnArea.bottom - (m_rcBtnArea.bottom - m_rcBtnArea.top)/2);
	return point;
}
	

void CChannel::SetEnabled(bool bEnable)
{
	m_bEnabled = bEnable;
}

bool CChannel::IsEnabled()
{
	return m_bEnabled;
}

void CChannel::SetPanVol( short nPan, short nVol )  
{
	ASSERT(m_pInstrument);

	m_pInstrument->SetPan(nPan);
	m_pInstrument->SetVolume(nVol);
	m_bEnabled = true;

	m_nPan = max( 0, min( 127, nPan ) );
	m_nVol = max( 0, min( 127, nVol ) );
}

void CChannel::Refresh()
{
	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
		return; 

	m_pInstrument->IsEnabled((BOOL*)&m_bEnabled);
	m_pInstrument->GetPan(&m_nPan);
	m_pInstrument->GetVolume(&m_nVol);
	m_pInstrument->IsSelected((BOOL*)&m_bSelected);
}

BOOL CChannel::HitTest( CPoint &point )
{
	return m_rcBtnArea.PtInRect(point);
}

void CChannel::GetButtonArea( CRect *prc )	
{ 
	*prc = m_rcBtnArea; 
}

long CChannel::GetId()  
{ 
	return m_nId; 
}

short CChannel::GetPan()  
{ 
	return m_nPan; 
}

short CChannel::GetVol()  
{ 
	return m_nVol;
}
#pragma optimize("", on)