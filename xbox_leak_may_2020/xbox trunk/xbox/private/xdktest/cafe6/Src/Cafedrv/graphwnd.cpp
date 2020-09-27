///////////////////////////////////////////////////////////////////////////////
//	graphwnd.cpp
//
//	Created by :			Date :
//		BrianCr				07/03/95
//
//	Description :
//		Implementation of the CGraphWnd class
//

#include "stdafx.h"
#include "graphwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphWnd

const CGraphWnd::cVertexWidth = 75;
const CGraphWnd::cVertexHeight = 50;
const CGraphWnd::cXSeparator = 30;
const CGraphWnd::cYSeparator = 0;
const CGraphWnd::cXBorder = 20;
const CGraphWnd::cYBorder = 35;

CGraphWnd::CGraphWnd(CGraph* pGraph, CListBox* plb)
	: m_pGraph(pGraph),
	m_plbTests(plb),
	m_ptOrigin(0, 0),
	m_nSelVertex(-1)
{
	FillTestList();
}

CGraphWnd::~CGraphWnd()
{
	// iterate through array of vertex info and delete each one
	for (int i = 0; i < m_aVertices.GetSize(); i++) {
		delete m_aVertices[i];
	}
	m_aVertices.RemoveAll();
}


BEGIN_MESSAGE_MAP(CGraphWnd, CWnd)
	//{{AFX_MSG_MAP(CGraphWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphWnd message handlers

void CGraphWnd::OnPaint() 
{
//	CPaintDC dc(this); // device context for painting
	CDC* pDC = GetDC();

	// set the viewport origin
	pDC->SetViewportOrg(-m_ptOrigin.x, -m_ptOrigin.y);

	// draw background over invalid rectangle
	CRect rectInvalid;
	GetUpdateRect(&rectInvalid);
	rectInvalid.OffsetRect(m_ptOrigin);
	CBrush brushBackground;
	brushBackground.CreateSolidBrush(RGB(128, 128, 128));
	pDC->FillRect(&rectInvalid, &brushBackground);

	// iterate through the vertices and draw each one
	for (int index = 0; index < m_aVertices.GetSize(); index++) {
		DrawVertex(pDC, index);
	}
	
	CWnd::OnPaint();
}

BOOL CGraphWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	// store each vertex in the array of vertex information
	for (POSITION pos = m_pGraph->GetFirstVertexPosition(); pos != NULL; ) {
		// get the vertex
		CVertex* pSrcVertex = m_pGraph->GetNextVertex(pos);
		// store vertex information
		StoreVertexInfo(pSrcVertex);
	}
	
	return CWnd::PreCreateWindow(cs);
}

BOOL CGraphWnd::Create(const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	return CWnd::Create(NULL, "GraphWnd", WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS, rect, pParentWnd, nID);
}

void CGraphWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// invalidate the rect that was previously selected
	if (m_nSelVertex >= 0) {
		InvalidateRect(m_aVertices[m_nSelVertex]->m_rect);
	}

	// offset the clicked point based on the graph origin
	point.Offset(m_ptOrigin);

	// figure out which vertex the user clicked in
	for (int i = 0; i < m_aVertices.GetSize(); i++) {
		// is the point clicked in the vertex rect?
		if (m_aVertices[i]->m_rect.PtInRect(point)) {
			m_nSelVertex = i;
			InvalidateRect(m_aVertices[i]->m_rect);
			break;
		}
	}

	if (i >= m_aVertices.GetSize()) {
		// no vertex selected
		m_nSelVertex = -1;
	}

	// update the test list
	FillTestList();

	// call the base class
	CWnd::OnLButtonDown(nFlags, point);
}

void CGraphWnd::StoreVertexInfo(CVertex* pVertex)
{
	int x1, y1, x2, y2;

	// figure out the index of this vertex
	int nIndex = m_aVertices.GetSize();

	// create space to store the vertex information
	VertexInfo* pVertexInfo = new VertexInfo(pVertex);
	// compute the rectangle for this vertex
	x1 = cXBorder + nIndex*cXSeparator + nIndex*cVertexWidth;
	y1 = cYBorder;
	x2 = x1 + cVertexWidth;
	y2 = cYBorder + cVertexHeight;

	pVertexInfo->m_rect = CRect(x1, y1, x2, y2);

	// add the vertex info to the array
	m_aVertices.Add(pVertexInfo);
}

void CGraphWnd::DrawVertex(CDC* pDC, int index)
{
	// select the pen
	pDC->SelectStockObject(BLACK_PEN);

	// select the brush
	// select the black brush, if this is the selected vertex
	pDC->SelectStockObject((index == m_nSelVertex) ? BLACK_BRUSH : WHITE_BRUSH);

	// draw the vertex
	pDC->Rectangle(m_aVertices[index]->m_rect);

	// select the font
	CFont font;
	if (!font.CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS | DEFAULT_PITCH, "Arial")) {
		TRACE("Can't create the font");
	}
	CFont* poldfont = pDC->SelectObject(&font);

	// set the text color
	// set the text color to white, if this is the selected vertex
	if (index == m_nSelVertex) {
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(0, 0, 0));
	}
	else {
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkColor(RGB(255, 255, 255));
	}

	// draw the text
	pDC->DrawText(m_aVertices[index]->m_pVertex->GetName(), -1, m_aVertices[index]->m_rect, DT_CENTER | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);

	// draw the edges leaving from this vertex

	// iterate through all edges from this vertex
	CVertex* pSrcVertex = m_aVertices[index]->m_pVertex;
	CVertex* pDestVertex;
	for (POSITION pos = m_pGraph->GetFirstEdgePosition(pSrcVertex); pos != NULL; ) {
		// get the edge
		CEdge* pEdge = m_pGraph->GetNextEdge(pSrcVertex, pos, &pDestVertex);
		// figure out the index of the destination vertex
		for (int nDestIndex = 0; nDestIndex < m_aVertices.GetSize(); nDestIndex++) {
			if (*(m_aVertices[nDestIndex]->m_pVertex) == *pDestVertex) {
				// draw an edge from the source vertex to the destination vertex
				DrawEdge(pDC, index, nDestIndex);
			}
		}
	}

	pDC->SelectObject(poldfont);
}

void CGraphWnd::DrawEdge(CDC* pDC, int srcindex, int destindex)
{
	int x1, y1, x2, y2, x3, y3, x4, y4;
	int dist;
	CRect srcRect, destRect;

	// get the rects for each vertex
	srcRect = m_aVertices[srcindex]->m_rect;
	destRect = m_aVertices[destindex]->m_rect;

	// select the pen
	pDC->SelectStockObject(BLACK_PEN);

	// draw the edge
	// is the edge going left to right?
	if (srcindex < destindex) {
		dist = destindex - srcindex;
		// is the edge to an adjacent vertex?
		if (dist == 1) {
			x1 = srcRect.right;
			y1 = srcRect.top + cVertexHeight/3;
			x2 = destRect.left;
			y2 = y1;

			// draw the edge
			pDC->MoveTo(x1, y1);
			pDC->LineTo(x2, y2);
			// draw the arrow head
			pDC->LineTo(x2-5, y2-5);
			pDC->MoveTo(x2, y2);
			pDC->LineTo(x2-5, y2+5);
		}
		// not an adjacent vertex
		else {
			x1 = srcRect.right;
			y1 = srcRect.top + cVertexHeight/4;
			x2 = x1 + cXSeparator/2;
			y2 = cYBorder - 2*dist;
			x3 = destRect.left - cXSeparator/2;
			y3 = y2;
			x4 = destRect.left;
			y4 = y1;

			// draw the edge
			pDC->MoveTo(x1, y1);
			pDC->LineTo(x2, y2);
			pDC->LineTo(x3, y3);
			pDC->LineTo(x4, y4);
			// draw the arrow head
			pDC->LineTo(x4, y4-5);
			pDC->MoveTo(x4, y4);
			pDC->LineTo(x4-5, y4);
		}
	}
	// is the edge going right to left?
	else if (srcindex > destindex) {
		dist = srcindex - destindex;
		// is the edge to an adjacent vertex?
		if (dist == 1) {
			x1 = srcRect.left;
			y1 = srcRect.top + (cVertexHeight/3)*2;
			x2 = destRect.right;
			y2 = y1;

			// draw the edge
			pDC->MoveTo(x1, y1);
			pDC->LineTo(x2, y2);
			// draw the arrow head
			pDC->LineTo(x2+5, y2-5);
			pDC->MoveTo(x2, y2);
			pDC->LineTo(x2+5, y2+5);
		}
		// not an adjacent vertex
		else {
			x1 = srcRect.left;
			y1 = srcRect.top + (cVertexHeight/4)*3;
			x2 = x1 - cXSeparator/2;
			y2 = cYBorder + cVertexHeight + 2*dist;
			x3 = destRect.right + cXSeparator/2;
			y3 = y2;
			x4 = destRect.right;
			y4 = y1;

			// draw the edge
			pDC->MoveTo(x1, y1);
			pDC->LineTo(x2, y2);
			pDC->LineTo(x3, y3);
			pDC->LineTo(x4, y4);
			// draw the arrow head
			pDC->LineTo(x4, y4+5);
			pDC->MoveTo(x4, y4);
			pDC->LineTo(x4+5, y4);
		}
	}
	// the edge goes to the same vertex
	else {
		x1 = srcRect.left + srcRect.Width() / 3;
		y1 = srcRect.top;
		x2 = x1;
		y2 = y1 - 10;
		x3 = x1 + srcRect.Width() / 3;
		y3 = y2;
		x4 = x3;
		y4 = y1;
		
		// draw the edge
		pDC->MoveTo(x1, y1);
		pDC->LineTo(x2, y2);
		pDC->LineTo(x3, y3);
		pDC->LineTo(x4, y4);
		// draw the arrow head
		pDC->LineTo(x4+5, y4-5);
		pDC->MoveTo(x4, y4);
		pDC->LineTo(x4-5, y4-5);
	}
}

void CGraphWnd::FillTestList(void)
{
	// empty the test list
	m_plbTests->ResetContent();

	// fill with the tests from the currently selected vertex
	if (m_nSelVertex >= 0) {
		CVertex* pSrcVertex;
		CVertex* pDestVertex;
		CEdge* pEdge;

		// get the source vertex
		pSrcVertex = m_aVertices[m_nSelVertex]->m_pVertex;

		// iterate through all edges from the source vertex
		for (POSITION pos = m_pGraph->GetFirstEdgePosition(pSrcVertex); pos != NULL; ) {
			// get the edge
			pEdge = m_pGraph->GetNextEdge(pSrcVertex, pos, &pDestVertex);
			// build the list entry
			CString str = pEdge->GetName() + "\t--> " + pDestVertex->GetName();
			// add the edge's name to the list
			m_plbTests->AddString(str);
		}
	}
	else {
		m_plbTests->AddString("No vertex selected");
	}
}

CSize CGraphWnd::GetSize(void)
{
	CSize size(0, 0);

	// iterate through each vertex and store the max X and Y position
	for (int i = 0; i < m_aVertices.GetSize(); i++) {
		size.cx = max(size.cx, m_aVertices[i]->m_rect.right);
		size.cy = max(size.cy, m_aVertices[i]->m_rect.bottom);
	}
	// the size must include a border on the right and bottom
	size.cx += cXBorder;
	size.cy += cYBorder;

	return size;
}

void CGraphWnd::SetOrigin(int x, int y)
{
	// ensure within our size range!
	CSize sizeGraph = GetSize();
	ASSERT(0 <= x && x <= sizeGraph.cx);
	ASSERT(0 <= y && y <= sizeGraph.cy);

	// calculate our invalid screen areas

	// get our client coord.
	RECT rectInvalid;
	GetClientRect(&rectInvalid);

	int xdiff = x - m_ptOrigin.x;
	int ydiff = y - m_ptOrigin.y;

	// scroll the window
	if (xdiff || ydiff)
		ScrollWindow(-xdiff, -ydiff);

	// horiz. invalid part
	if (ydiff)
	{
		if (ydiff < 0)
			rectInvalid.bottom = rectInvalid.top - ydiff;
		else
			rectInvalid.top = rectInvalid.bottom - ydiff;

		InvalidateRect(&rectInvalid, TRUE);
	}

	// vert. invalid part
	if (xdiff)
	{
		if (xdiff < 0)
			rectInvalid.right = rectInvalid.left - xdiff;
		else
			rectInvalid.left = rectInvalid.right - xdiff;

		InvalidateRect(&rectInvalid, TRUE);
	}

	// set our origin
	m_ptOrigin = CPoint(x, y);
}
