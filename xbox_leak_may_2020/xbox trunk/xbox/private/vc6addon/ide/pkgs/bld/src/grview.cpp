//
// CGraphPainter, CGraphWnd, CGraphView, CGraphDialog
//
// definitions for CGraphView -- the graph viewer
//
// The graph viewer makes use of the CLayout object to position
// the nodes of a graph into rows and columns that nicely show
// the heirarchy.
// 
// The CLayout object in turn uses the CNidGraph protocol to determine
// the relations in the graph.  Based on this information it
// assigns logical rows and columns to each CGrNode.
//
// Responsiblity for painting the nodes is on the CGraphPainter object
// which can give the extent of any node and can paint it in any
// given position.
//
// The graph view makes use of the CGraphPainter to find out the size
// of each node from which it computes the physical size of each row
// and column.  With this information it can compute the physical
// position of every node.  It uses these positions to draw all of
// the rows which interconnect the nodes and to further instruct
// the CGraphPainter to paint each node at that position.
//
// [ricom] (modified by [matthewt])

#include "stdafx.h"
#pragma hdrstop

#ifdef DEPGRAPH_VIEW

#include "resource.h"	// resources header
#include "grview.h"		// local header

IMPLEMENT_DYNAMIC(CGraphLegend, CWnd)
IMPLEMENT_DYNAMIC(CGraphWnd, CWnd)
IMPLEMENT_DYNAMIC(CGraphDialog, CDialog)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CX_LEFT_MARGIN 10

/////////////////////////////////////////////////////////////////////////////
// CGraphLegend

// RGBs to be used
#define RGB_BKG_OUTOFDATE		RGB_RED
#define RGB_BKG_DEPMISSING		RGB_BLUE
#define RGB_BKG_CURRENT			RGB_GREEN
#define RGB_BKG_FORCEDOUTOFDATE RGB_YELLOW

#define RGB_TXT_FILEEXISTS		RGB_BLACK
#define RGB_TXT_FILEMISSING		RGB_WHITE

#define RGB_TXT_DEPENDENCY		RGB_DARKMAGENTA

typedef struct _tagLgndEntry
{
	COLORREF clr;			// color	
/*
	UINT idType;			// UI usage
	UINT idText;			// meaning
*/
	const TCHAR * pchType;	// UI usage
	const TCHAR * pchText;	// meaning
} LgndEntry;

// legend to use
static const LgndEntry rgLgnds[] =
{
	RGB_BKG_CURRENT, "(Bkgnd)", "Current",
	RGB_BKG_OUTOFDATE, "(Bkgnd)", "Out of date",
    RGB_BKG_FORCEDOUTOFDATE, "(Bkgnd)", "Forced out of date",
	RGB_BKG_DEPMISSING, "(Bkgnd)", "Dependency missing",
	RGB_TXT_FILEEXISTS, "(Text)", "File exists",
	RGB_TXT_FILEMISSING, "(Text)", "File missing",
	RGB_TXT_DEPENDENCY, "(Text)", "Dependency File",
};

BEGIN_MESSAGE_MAP(CGraphLegend, CWnd)
	//{{AFX_MSG_MAP(CGraphLegend)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGraphLegend::Create(UINT nID, CRect rect, CWnd * pParentWnd /* = NULL */)
{
	return CWnd::Create(NULL,						// class
						"GraphLgndWnd",				// window name
						WS_CHILD | WS_CLIPSIBLINGS,	// style
						(const RECT)rect,			// rect
						pParentWnd,					// parent window
						nID							// id
					   );
}

void CGraphLegend::OnPaint()
// paint the graph legend
//
{
	// get the window DC?
	CDC * pdc = GetDC();

	// use a small font
	pdc->SelectObject(GetStdFont(font_Italic));
	
	// legend (x,y)
	int x = 0, y = 0;

	TEXTMETRIC textmetric;
	VERIFY(pdc->GetTextMetrics(&textmetric));

	// cycle through legend
	for (int i = 0; i < sizeof(rgLgnds) / sizeof(LgndEntry); i++)
	{
		// paint legend color blob
		RECT rectBox = {x, y, x + textmetric.tmHeight, y + textmetric.tmHeight};
		// draw box
		pdc->MoveTo(rectBox.left, rectBox.top);
		pdc->LineTo(rectBox.right - 1, rectBox.top);
		pdc->LineTo(rectBox.right - 1, rectBox.bottom - 1);
		pdc->LineTo(rectBox.left, rectBox.bottom - 1);
		pdc->LineTo(rectBox.left, rectBox.top);

		// paint color 'usage' text
		pdc->SetBkColor(::GetSysColor(COLOR_BTNFACE));
		pdc->ExtTextOut(x + rectBox.right + 4, y, 0, NULL, rgLgnds[i].pchType, _tcslen(rgLgnds[i].pchType), NULL);

		rectBox.left++; rectBox.right--; rectBox.top++; rectBox.bottom--;
		pdc->SetBkColor(rgLgnds[i].clr);
		pdc->ExtTextOut(x + 1, y + 1, ETO_OPAQUE, &rectBox, NULL, 0, NULL);

		// move down a line of text + spacing
		y += (textmetric.tmHeight + 2);

		// paint color 'meaning' text
		pdc->SetBkColor(::GetSysColor(COLOR_BTNFACE));
		pdc->ExtTextOut(x, y, 0, NULL, rgLgnds[i].pchText, _tcslen(rgLgnds[i].pchText), NULL);

		// move down a line of text + spacing
		y += (textmetric.tmHeight + 4);
	}

	// call the base-class
	CWnd::OnPaint();
}

/////////////////////////////////////////////////////////////////////////////
// CGraphPainter

BOOL CGraphPainter::FInitGraphCDC(CDC * pdc, BOOL fErase)
{
	if (fErase)
		ASSERT(FALSE);	// FUTURE

	// make sure that this is MM_TEXT mapping mode
	pdc->SetMapMode(MM_TEXT);

	return TRUE;
}

void CGraphPainter::GetOffsets(CDC *, NID, NID, ET, int & cyOut, int & cyIn)
// return the offset from center for a lines going from the first NID arg
// to the second NID arg in cyOut and cyIn
//
{
	cyOut = 0;
	cyIn  = 0;
}

void CGraphPainter::GetSpacing
(
	CDC *,                
	int & cxMinTag,   
	int & cxEdgeSpace,
	int &  xLeftAbs,   
	int &  yTopAbs,    
	int & cyFakeHeight
)                     
// return the default spacing for a graph
//
{
	cxMinTag	 = 5;		// minimum length of straight line after label
	cxEdgeSpace  = 30;		// space allocated between nodes for drawing edges
	xLeftAbs	 = 10;		// x origin of the graph in standard position
	yTopAbs      = 2;		// y origin of the graph in standard position
	cyFakeHeight = 10;		// height allocated for fake nodes
}

void CGraphPainter::PaintAnchor(NID, int x, int y, int cx, int cy, CDC * pdc)
// default anchor is a box around the whole node
//
{
	pdc->InvertRect(CRect(x+1, y, cx-2, 1));		// Top
	pdc->InvertRect(CRect(x+1, y+cy-1, cx-2, 1));	// Bottom
	pdc->InvertRect(CRect(x, y, 1, cy));			// Left
	pdc->InvertRect(CRect(x+cx-1, y, 1, cy));		// Right
}

void CGraphPainter::PaintSelect(NID, int x, int y, int cx, int cy, CDC * pdc)
// invert entire xtnt by default
//
{
	pdc->InvertRect(CRect(x, y, cx, cy));
}			  

void CGraphPainter::PaintDeAnchor(NID, int x, int y, int cx, int cy, CDC * pdc)
// default anchor is a box around the whole node
//
{
	pdc->InvertRect(CRect(x+1, y, cx-2, 1));		// Top
	pdc->InvertRect(CRect(x+1, y+cy-1, cx-2, 1));	// Bottom
	pdc->InvertRect(CRect(x, y, 1, cy));			// Left
	pdc->InvertRect(CRect(x+cx-1, y, 1, cy));		// Right
}

void CGraphPainter::PaintDeSelect(NID, int x, int y, int cx, int cy, CDC * pdc)
// invert entire xtnt by default
//
{
	pdc->InvertRect(CRect(x, y, cx, cy));
}

/////////////////////////////////////////////////////////////////////////////
// CDepGraphPainter

BOOL CDepGraphPainter::FInitGraphCDC(CDC * pdc, BOOL fErase)
{
	// call base-class
	if (!CGraphPainter::FInitGraphCDC(pdc, fErase))
		return FALSE;

	pdc->SelectObject(GetStdFont(font_Small));

	return TRUE;	// ok
}

void CDepGraphPainter::GetNidSize(NID nid, CSize & size, CDC * pDC)
{
	size.cx = size.cy = 0;
	
	// get the node text
	CString strText;
	GetNidText(nid, strText);

	// calculate the size of the text
	const TCHAR * pchText = (const TCHAR *)strText;
	const TCHAR * pch = pchText;

	CSize sizeLine;
	while (*pch != _T('\0'))
	{
		while (*pch != _T('\0') && *pch != ('\n'))
			pch = _tcsinc(pch);

		// what is the size of text?
		sizeLine = pDC->GetTextExtent((LPCTSTR)pchText, pch - pchText);

		if (sizeLine.cx > size.cx)
			size.cx = sizeLine.cx;
		size.cy += (sizeLine.cy + 2);	// padding

		if (*pch == _T('\n'))
			pchText = ++pch;
	}

    size.cy += sizeLine.cy + 2; // Depstate as text

	size.cx += 4;
	size.cy += 4;
}

void CDepGraphPainter::PaintAt(NID nid, int x, int y, CDC * pdc)
{
	CSize size(0,0);

	// what is the size of text?
	GetNidSize(nid, size, pdc);

	// draw box
	pdc->MoveTo(x, y);
	pdc->LineTo(x + size.cx - 1, y);
	pdc->LineTo(x + size.cx - 1, y + size.cy - 1);
	pdc->LineTo(x, y + size.cy - 1);
	pdc->LineTo(x, y);

	CRect rect(x + 1, y + 1, x + size.cx - 1, y + size.cy - 1);

	// text fore + back colors?
	CFileRegEntry * preg = g_FileRegistry.GetRegEntry(((CDepGrNode *)nid)->Frh());
	COLORREF frClr = preg->ExistsOnDisk() ? RGB_TXT_FILEEXISTS : RGB_TXT_FILEMISSING , bkClr = ::GetSysColor(COLOR_BTNFACE);

	// what is the state of this node?
	UINT state = ((CDepGrNode *)nid)->Depstate();

    CString strDepState;
	
	// dependency missing
	if (state & DS_DepMissing)
	{
		bkClr = RGB_BKG_DEPMISSING;
	}
	// out of date
	else if (state & DS_OutOfDate)
	{
		bkClr = RGB_BKG_OUTOFDATE;
	}
    else if (state & DS_ForcedOutOfDate)
    {
        bkClr = RGB_BKG_FORCEDOUTOFDATE;
    }
	// current
	else if (state & DS_Current)
	{
		bkClr = RGB_BKG_CURRENT;
	}

    if (state & DS_DepMissing)
        strDepState += _T("M");
    if (state & DS_OutOfDate)
        strDepState += _T("O");
    if (state & DS_ForcedOutOfDate)
        strDepState += _T("F");
    if (state & DS_Current)
        strDepState += _T("C");

	// dependency set?
	if (preg->IsKindOf(RUNTIME_CLASS(CFileRegSet)))
	{
		frClr = RGB_TXT_DEPENDENCY;
	}

	pdc->SetTextColor(frClr);
	pdc->SetBkColor(bkClr);

	// paint the text

	// get the node text
	CString strText;
	GetNidText(nid, strText);

	const TCHAR * pchText = (const TCHAR *)strText;
	const TCHAR * pch = pchText;

	CSize sizeLine;
	while (*pch != _T('\0'))
	{
		while (*pch != _T('\0') && *pch != ('\n'))
			pch = _tcsinc(pch);

		// paint the text
		pdc->ExtTextOut(x + 2, y + 2, ETO_OPAQUE, rect, pchText, pch - pchText, NULL);

		// what is the size of text?
		sizeLine = pdc->GetTextExtent((LPCTSTR)pchText, pch - pchText);

		// move down a line
		rect.bottom += sizeLine.cy + 2;
		rect.top = rect.bottom;
		y += (sizeLine.cy + 2);	// padding

		if (*pch == _T('\n'))
			pchText = ++pch;
	}

	pchText = (const TCHAR *)strDepState;
    pdc->ExtTextOut(x + 2, y + 2, ETO_OPAQUE, rect, pchText, strDepState.GetLength(), NULL);
}

void CDepGraphPainter::GetNidText(NID nid, CString & strText)
{
	CDepGrNode * pgrn = (CDepGrNode *)nid;
	CFileRegEntry * preg = g_FileRegistry.GetRegEntry(pgrn->Frh());

	CString strFile;

	// get the node text
	if (preg->IsKindOf(RUNTIME_CLASS(CFileRegSet)))
	{
		// dependency set text
		CFileRegSet * pregset = (CFileRegSet *)preg;
		pregset->InitFrhEnum();

		// get an action 'edge'?
		if (pgrn->CParents() > 0)
		{
			CActionSlob * pAction = (CActionSlob *)pgrn->Parent(0)->Edge(0);

			// what kind of set?
			if (preg == pAction->GetScannedDep())
				strText = _T("<Scanned>\n");

			else if (preg == pAction->GetMissingDep())
				strText = _T("<Missing>\n");

			else if (preg == pAction->GetSourceDep())
				strText = _T("<Source>\n");
		}

		FileRegHandle frh;

		// show the names of each dep.
		while ((frh = pregset->NextFrh()) != (FileRegHandle)NULL)
		{
			preg = g_FileRegistry.GetRegEntry(frh);

			const CPath * pFile = preg->GetFilePath();
#ifdef REFCOUNT_WORK
			frh->ReleaseFRHRef();
#endif
			
			// display it relative to current directory
			if (!pFile->GetRelativeName(m_dirCur, strFile))
		 		strFile = (const TCHAR *)*pFile;	// failed, just use name

			// sep. lines
			if (!strText.IsEmpty())
				strText += _T('\n');

			strText += strFile;

		}
	}
	else
	{
		// file text is
		// 'name\ntimestamp' or 'name\n<not found>'
		const CPath * pFile = preg->GetFilePath();
		
		// display it relative to current directory
		if (!pFile->GetRelativeName(m_dirCur, strFile))
		 	strFile = (const TCHAR *)*pFile;	// failed, just use name

		strText += strFile;

		// prepare for next line
		strText += _T('\n');

		// append timestamp or '<not found>'
		CString strTime;
		if (preg->ExistsOnDisk() && preg->GetFileTime(strTime))
		{
			strText += strTime;
		}
		else
		{
			strText += "<not found>";
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGraphWnd

BEGIN_MESSAGE_MAP(CGraphWnd, CWnd)
	//{{AFX_MSG_MAP(CGraphWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGraphWnd::CGraphWnd(CGraphPainter * ppainter, CNidGraphDoc * pdoc, BOOL fRev, USHORT usDepth)
{
	m_ppainter      = ppainter;
	m_pdoc			= pdoc;
	m_usDepth		= usDepth;
	m_irowVisMin 	= 0;
	m_icolVisMin  	= 0;
    m_icolMac  		= 0;
	m_crowsVis   	= 0;
    m_fReversed		= fRev;
    m_fPositioned	= FALSE;
	m_rgXRight		= NULL;
	m_rgXLeft		= NULL;
	m_rgYTop		= NULL;
	m_play			= NULL;
	m_ptOrig		= CPoint(0,0);
}

CGraphWnd::~CGraphWnd()
// free up all of the secondary allocations that we made on our way out
//
{
	if (m_play)		delete m_play;
	if (m_rgXRight)	delete [] m_rgXRight;
	if (m_rgXLeft)	delete [] m_rgXLeft;
	if (m_rgYTop)	delete [] m_rgYTop;
}

BOOL CGraphWnd::PreCreateWindow(CREATESTRUCT & cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	return CLayout__FCreate(m_play, m_pdoc, m_usDepth);
}

BOOL CGraphWnd::Create(UINT nID, CRect rect, CWnd * pParentWnd /* = NULL */)
{
	return CWnd::Create(NULL,						// class
						"GraphWnd",					// window name
						WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,		// style
						(const RECT)rect,			// rect
						pParentWnd,					// parent window
						nID							// id
					   );
}

BOOL CGraphWnd::RefreshGraph()
{
	ASSERT(m_play);
	delete m_play;

	// re-create layout and position nodes
	m_fPositioned = FALSE;
	if (!CLayout__FCreate(m_play, m_pdoc, m_usDepth))
		return FALSE;

	// re-paint
	Invalidate(FALSE);

	return TRUE;
}

void CGraphWnd::GetGraphSize(CSize & size)
{
	// need to position nodes?
	if (!m_fPositioned)
		PositionNodes();

	// size of the graph
	size = CSize(m_icolMac == 0 ? 0 : m_rgXRight[m_icolMac-1], m_irowMac == 0 ? 0 : m_rgYTop[m_irowMac]);
}

void CGraphWnd::SetGraphOrigin(int x, int y)
{
	// ensure within our size range!
	CSize sizeGraph;
	GetGraphSize(sizeGraph);
	
	ASSERT(0 <= x && x <= sizeGraph.cx);
	ASSERT(0 <= y && y <= sizeGraph.cy);

	// calculate our invalid screen areas

	// get our client coord.
	RECT rectInvalid;
	GetClientRect(&rectInvalid);

	int xdiff = x - m_ptOrig.x;
	int ydiff = y - m_ptOrig.y;

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

		InvalidateRect(&rectInvalid, FALSE);
	}

	// vert. invalid part
	if (xdiff)
	{
		if (xdiff < 0)
			rectInvalid.right = rectInvalid.left - xdiff;
		else
			rectInvalid.left = rectInvalid.right - xdiff;

		InvalidateRect(&rectInvalid, FALSE);
	}

	// set our origin
	m_ptOrig = CPoint(x, y);
}


void CGraphWnd::OnPaint()
// repaint the dirty portion of the graph
//
{
	// get the window DC?
	CDC * pdc = GetDC();

	// initialise this DC?
	if (pdc == (CDC * )NULL ||
		!m_ppainter->FInitGraphCDC(pdc, FALSE)
	   )
		return;	// failed to initialise

	// position the graph, reset our position based on this new layout
	if (!m_fPositioned)
		PositionNodes();

	// anything to paint?
	if (m_play->CNodesMac() == 0)
		return;	// no

	// set the viewport origin
	pdc->SetViewportOrg(-m_ptOrig.x, -m_ptOrig.y);

	USHORT i, iMac, j, jMac, k, kMac;

	CRect rectClient, rectInvalid;
	GetClientRect(rectClient);
	GetUpdateRect(&rectInvalid);
	rectInvalid.OffsetRect(m_ptOrig);

	pdc->FillRect(rectInvalid, GetSysBrush(COLOR_BTNFACE));

	// compute the number of visible rows
	
	iMac = m_irowMac;

	for (i=0; i<iMac; i++)
		if (m_rgYTop[i] >= rectClient.Height()) break;

	m_crowsVis = i - m_irowVisMin;

	// compute the range of rows which needs to be redrawn
							   
	for (i=m_irowVisMin; i<iMac; i++)
		if (m_rgYTop[i+1] >= rectInvalid.top) break;

	USHORT irowClipMin = i;

	for (; i<iMac; i++)
		if (m_rgYTop[i] > rectInvalid.bottom) break;

	USHORT irowClipMac = i;

	iMac = m_play->IColMac();

	for (i=0; i < iMac-1; i++)
		if (m_rgXLeft[i+1] > 0) break;

	m_icolVisMin = i;

	for (; i < iMac-1; i++)
		if (m_rgXLeft[i+1] >= rectInvalid.left) break;

	USHORT icolClipMin = i;

	for (; i<iMac; i++)
		if (m_rgXLeft[i] > rectInvalid.right) break;

	USHORT icolClipMac = i;

	// redraw all of the columns which are in the clipping rectangle

	for (i = icolClipMin; i < icolClipMac; i++)
	{
		jMac = m_play->CNodesCol(MapCol(i));
		for (j=0; j < jMac; j++)
		{
			CGrNode *pnode = m_play->PnodeAt(MapCol(i),j);
			USHORT row = pnode->IRow();
			NID nid = pnode->Nid();

			if (row < irowClipMin) continue;

			if (row >= irowClipMac) break;

			if (!pnode->FFake())
			{
				int y = (int)(YcoCenter(row) - (pnode->GetSize().cy / 2));
			    int x = m_rgXLeft[i];

				m_ppainter->PaintAt(nid, x, y, pdc);
			}
		}


		// no children for the last column
		if (i >= iMac-1)
			break;

		USHORT colParent = m_fReversed ? i+1 : i;

		jMac = m_play->CNodesCol(MapCol(colParent));

		for (j=0; j < jMac; j++)
		{
			CGrNode *pnode = m_play->PnodeAt(MapCol(colParent),j);
			USHORT row  = pnode->IRow();
			NID nid     = pnode->Nid();

			BOOL fLow  = pnode->IRow() <  irowClipMin;
			BOOL fHigh = pnode->IRow() >= irowClipMac;

			int	cxLen  = 0;

			kMac = pnode->CChildren();

			if (kMac == 0) continue;

			if (!pnode->FFake())
				cxLen = pnode->GetSize().cx;

			// this tests for overflow into the sign bit
			// otherwise our drawing code will try to draw large positive
			// y coordinates in the negative space
			
			// there is some slack here because the y coordinate might be
			// further offset by the painter

			long  lT = YcoCenter(row);
			int   cyBase;

			if (lT >= 0x7f00)
				cyBase = 0x7f00;
			else if (lT <= -0x7f00)
				cyBase = -0x7f00;
			else
				cyBase = (int)lT;

			for (k=0; k < kMac; k++)
			{
				CGrNode *pChild = pnode->Child(k);
				ET	  et	 = pnode->Edge(k);
				row = pChild->IRow();

				if (fLow  && row <  irowClipMin) continue;
				if (fHigh && row >= irowClipMac) continue;

				int cyIn  = 0;
				int cyOut = 0;

				m_ppainter->GetOffsets(pdc, nid, pChild->Nid(), et, cyOut, cyIn);

				int cyParent = cyBase + cyOut;

				// this tests for overflow into the sign bit
				// otherwise our drawing code will try to draw large positive
				// y coordinates in the negative space
				
				// there is some slack here because the y coordinate might be
				// further offset by the painter

				lT = YcoCenter(row);
				int cyChild;

				if (lT >= 0x7f00)
					cyChild = 0x7f00;
				else if (lT <= -0x7f00)
					cyChild = -0x7f00;
				else
					cyChild = (int)lT;

				cyChild += cyIn;

				int x1, x2, x3;
				int y1, y2;

				if (!m_fReversed)
				{
					x1 = m_rgXLeft[colParent] + cxLen;
					x2 = m_rgXRight[colParent];
					x3 = m_rgXLeft[colParent+1];
					y1 = cyParent;
					y2 = cyChild;
				}
				else
				{
					if (!pChild->FFake())
						cxLen = pChild->GetSize().cx;
					else
						cxLen = 0;

					x1 = m_rgXLeft[colParent-1] + cxLen;
					x2 = m_rgXRight[colParent-1];
					x3 = m_rgXLeft[colParent];
					y1 = cyChild;
					y2 = cyParent;
				}

				if (k + pnode->CReversed() >= kMac)
				{
					y1 += 4;
					y2 += 4;
					pdc->MoveTo(x1    , y1    );	
					pdc->LineTo(x1 + 2, y1 + 2);		// draw arrowhead	
					pdc->LineTo(x1 + 2, y1 - 2);
					pdc->LineTo(x1    , y1    );	
				}

				pdc->MoveTo(x1    , y1);
				pdc->LineTo(x2    , y1);
				pdc->LineTo(x3 - 1, y2);
				pdc->LineTo(x3    , y2);
			}
		}
	}

	CWnd::OnPaint();	// pass on to our base instance
}

void CGraphWnd::PositionNodes() 
// each CGrNode has now been placed in a column and assigned a position
// within that column.  Here we determine the widths of our columns
// and rows
//
{
	// got a painter?
	ASSERT(m_ppainter != (CGraphPainter *)NULL);

	// get the graph DC
	CDC * pdc = GetDC();
	ASSERT(pdc != (CDC * )NULL);

	if (!m_ppainter->FInitGraphCDC(pdc, FALSE))
		return;

	// origin point to position relative to
	CPoint pt(m_ptOrig);

	m_ppainter->GetSpacing(pdc, m_cxMinTag, m_cxEdgeSpace,
						  m_xLeftAbs, m_yTopAbs, m_cyFakeHeight);

	m_irowMac  = m_play->IRowMac();
    m_icolMac  = m_play->IColMac();

	pt.x = pt.y = 0;

	if (m_icolMac == 0)
		return;

	m_icolLim  = m_icolMac - 1;

	// FUTURE: place the construction of a wait cursor here

	if ((m_rgXRight = new int[m_icolMac]) == (int *)NULL)
		return;

	if ((m_rgXLeft = new int[m_icolMac]) == (int *)NULL)
		return;

	if ((m_rgYTop = new long[m_irowMac+1]) == (long *)NULL)
		return;

	// initialize out the YTop array
	USHORT i, iMac;
	iMac = m_irowMac+1;
	for (i=0; i<iMac; i++) m_rgYTop[i] = 0;

	USHORT xCur  = m_xLeftAbs;	
	int xNext    = m_xLeftAbs;	

	for (UINT iCol = 0; iCol < m_icolMac; iCol++)
	{
		m_rgXLeft[iCol] = xCur;

		iMac = m_play->CNodesCol(MapCol(iCol));

		for (i = 0; i < iMac; i++)
		{
			CGrNode  *pnode  = m_play->PnodeAt(MapCol(iCol),i);
	
			int xEnd = xCur;

			if (!pnode->FFake())
			{
				CSize size;

				m_ppainter->GetNidSize(pnode->Nid(), size, pdc);

				pnode->SetSize(size);	// cache node size

				if (size.cy > m_rgYTop[pnode->IRow()+1])
					m_rgYTop[pnode->IRow()+1] = size.cy;

				xEnd += size.cx;
			}
			else
			{
				// use metric for height of fake nodes

				if (m_cyFakeHeight > m_rgYTop[pnode->IRow()+1])
					m_rgYTop[pnode->IRow()+1] = m_cyFakeHeight;
			}

			if (xEnd > xNext) xNext = xEnd;
		}
	
		m_rgXRight[iCol] = xNext + m_cxMinTag;  // space after label
		xCur = xNext + m_cxEdgeSpace;		  // space between columns
	}

	m_rgYTop[0] = m_yTopAbs;

	for (USHORT irow = 1; irow <= m_irowMac; irow++)
		m_rgYTop[irow] += m_rgYTop[irow-1];

	// position view so that the correct nodes are initially visible

	CRect rect; GetClientRect(rect);
	int cxSize = rect.Width();

	if (m_fReversed && m_rgXRight[m_icolMac-1] > cxSize)
		pt.x = m_rgXRight[m_icolMac-1] - cxSize + 5;	// 5 is arb buffering

	// if there are any zero height rows at the end of the document, don't both visualizing them...
	// that will just make our scrolling look stupid

	while (m_irowMac > 1 && CyLine(m_irowMac-1) == 0)
		m_irowMac--;

	m_fPositioned = TRUE;
}

CGrNode * CGraphWnd::PnodeSearchBest(USHORT irow, USHORT icol, USHORT icolEnd)
// search for the node closest to the given irow starting from the given
// column and proceeding up to and including the End column
//
// returns only non fake nodes, return NULL if no appropriate node
// could be found
//
{
	CGrNode * pnodeBest = (CGrNode *)NULL;
	USHORT costBest  = 0xffff;

	int dcol = icolEnd > icol ? 1 : -1;

	while (pnodeBest == NULL)
	{
		USHORT inodeMac = m_play->CNodesCol(MapCol(icol));

		for (USHORT inode = 0; inode < inodeMac ; inode++)
		{
			CGrNode *pnodeT = m_play->PnodeAt(MapCol(icol), inode);

			if (!pnodeT->FFake())
			{
				USHORT cost = _abs(pnodeT->IRow() - irow);

				if (cost < costBest)
				{
					costBest  = cost;
					pnodeBest = pnodeT;
				}
			}
		}

		if (icol == icolEnd) break;

		icol += dcol;
	}

	return pnodeBest;
}

CGrNode * CGraphWnd::PnodeSearchCorner
(
	USHORT icolBeg,		// starting column	(cost zero if in this column)
	USHORT irowBeg,		// starting row		(cost zero if in this row)
	USHORT icolEnd,		// ending column
	USHORT irowEnd		// ending row
)
//
// search the graph for the node which is closest to icolBeg, irowBeg
// but the column doesn't exceed icolEnd and the row doesn't exceed irowEnd
// note that icolEnd might be less than icolBeg as can irowEnd
//
{
	// first convert the given beginning and end rows which may not be
	// in the correct order to irowStart and irowStop values which will be
	// sorted.  This allows easy comparison...

	USHORT irowStart, irowStop;

	if (irowBeg < irowEnd)
		{ irowStart = irowBeg; irowStop  = irowEnd; }
	else
		{ irowStart = irowEnd; irowStop  = irowBeg; }

	USHORT	icol = icolBeg;
	int    	dcol = icolBeg < icolEnd ? 1 : -1;
	CGrNode *pnodeBest = (CGrNode *)NULL;
	USHORT 	costBest  = 0xffff;

	for (;;)
	{
		if (_abs(icol - icolBeg) > costBest)
			break;

		USHORT inode;
		USHORT inodeMac = m_play->CNodesCol(MapCol(icol));

		for (inode = 0; inode < inodeMac; inode++)
		{
			CGrNode *pnodeT = m_play->PnodeAt(MapCol(icol), inode);

			if (pnodeT->FFake()) continue;

			USHORT irow = pnodeT->IRow();

			if (irow < irowStart) continue;

			if (irow > irowStop) break;

			USHORT costT = _abs((int)(m_rgYTop[irow] - m_rgYTop[irowBeg])) +
						   _abs(m_rgXLeft[icol] - m_rgXLeft[icolBeg]);

			if (costT < costBest)
			{
				pnodeBest = pnodeT;
				costBest = costT;
			}
		}

		if (icol == icolEnd) break;

		icol += dcol;
	}

	return pnodeBest;
}

USHORT CGraphWnd::IColVisLim()
// return the last visible column
//
{
	CRect rect; GetClientRect(rect);
	CPoint pt(0,0);	// = GetScrollPosition();	// FUTURE:

	USHORT iMac = m_icolMac;

	for (USHORT i = 0; i < iMac; i++)
		if (m_rgXLeft[i] > pt.x + rect.Width()) break;

	ASSERT(i > 0);
							
	return i-1;
}

void CGraphWnd::UpdateVisibleRows()
// compute the number of visible rows
{
	CPoint pt(0,0);	// = GetScrollPosition();	// FUTURE:

	USHORT i, iMac = m_irowMac;

	for (i=0; i<iMac; i++)
		if (m_rgYTop[i] >= pt.y) break;

	// check for bogus origin (pt.y)
	if (i >= iMac)
		return;

	m_irowVisMin = i;	// number of visible rows
}

/////////////////////////////////////////////////////////////////////////////
// CGraphWnd diagnostics

#ifdef _DEBUG
void CGraphWnd::AssertValid() const
{
	CWnd::AssertValid();
}

void CGraphWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphDialog

BEGIN_MESSAGE_MAP(CGraphDialog, CDialog)
	//{{AFX_MSG_MAP(CGraphDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()	
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_GRAPH_SHOWDEPS, OnShowDeps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGraphDialog::CGraphDialog(CGraph * pgraph, CWnd * pParentWnd /* = NULL */) : CDialog(IDD_GRAPH_DIALOG, pParentWnd)
{
	// our graph document
	m_pgraph = pgraph;

	// disable FC
	g_FileRegDisableFC = TRUE;
}

CGraphDialog::~CGraphDialog()
{
	// re=enable FC
	g_FileRegDisableFC = FALSE;
}

BOOL CGraphDialog::OnInitDialog()
{
	if (!CDialog::OnInitDialog())
		return FALSE;	// failure

	// show and resize the graph legend window
	CRect rectPicture;
	GetDlgItem(IDC_GRAPH_LEGEND)->GetWindowRect(rectPicture);
	ScreenToClient(&rectPicture);

	m_pgraphlgnd->SetWindowPos(NULL,
							  rectPicture.left, rectPicture.top,
							  rectPicture.Width(), rectPicture.Height(),
							  SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);

	// show and resize the graph window
	GetDlgItem(IDC_GRAPH_FRAME)->GetWindowRect(rectPicture);
	ScreenToClient(&rectPicture);
	m_pgraphwnd->SetWindowPos(NULL,
							  rectPicture.left, rectPicture.top,
							  rectPicture.Width(), rectPicture.Height(),
							  SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);

	// calc. the scroll metrics
	CalcScrollMetrics();

	// hide show deps. check or init. it's state?
	if (m_pgraph->IsKindOf(RUNTIME_CLASS(CFileDepGraph)))
		((CButton *)GetDlgItem(IDC_GRAPH_SHOWDEPS))->SetCheck(((CFileDepGraph *)m_pgraph)->m_fShowDepSets);
	else
		GetDlgItem(IDC_GRAPH_SHOWDEPS)->ShowWindow(SW_HIDE);

	return TRUE;	// ok
}

int CGraphDialog::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CDialog::OnCreate(lpcs) != 0)
		return -1;

	// get our painter
	m_ppainter = new CDepGraphPainter;

	// get our graph window
	m_pgraphwnd = new CGraphWnd(m_ppainter, m_pgraph, TRUE);
	if (!m_pgraphwnd->Create(16, CRect(0,0,0,0), this))
	{
		delete m_ppainter;
		return -1;
	}

	// get our graph legend
	m_pgraphlgnd = new CGraphLegend;
	if (!m_pgraphlgnd->Create(17, CRect(0,0,0,0), this))
	{
		delete m_ppainter;
		delete m_pgraphwnd;
		return -1;
	}

	return 0;	// ok
}

void CGraphDialog::OnDestroy()
{
	delete m_ppainter;
	delete m_pgraphwnd;
	delete m_pgraphlgnd;
}

void CGraphDialog::CalcScrollMetrics()
{
	// set the ranges for our scroll bars
	// and the line and page sizes
	m_pgraphwnd->GetGraphSize(m_sizeScroll);

	CRect rectPicture;
	GetDlgItem(IDC_GRAPH_FRAME)->GetWindowRect(rectPicture);

	m_line.cx = 16;
	m_line.cy = 32;
	m_page.cx = rectPicture.Width()- m_line.cx;
	m_page.cy = rectPicture.Height() - m_line.cy;
	
	((CScrollBar *)GetDlgItem(IDC_GRAPH_HSCRL))->SetScrollRange(0, m_sizeScroll.cx, FALSE);
	((CScrollBar *)GetDlgItem(IDC_GRAPH_VSCRL))->SetScrollRange(0, m_sizeScroll.cy, FALSE);
}

void CGraphDialog::OnShowDeps()
{
	// global/per-builder check state?
	ASSERT(m_pgraph->IsKindOf(RUNTIME_CLASS(CFileDepGraph)));
	((CFileDepGraph *)m_pgraph)->m_fShowDepSets = ((CButton *)GetDlgItem(IDC_GRAPH_SHOWDEPS))->GetCheck() != 0;

	// refresh the graph
	VERIFY(m_pgraphwnd->RefreshGraph());

	// re-calc. the scroll metrics
	CalcScrollMetrics();
}

void CGraphDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar * pscrlbar)
{
	ASSERT(pscrlbar == GetDlgItem(IDC_GRAPH_HSCRL));
	OnScroll(MAKEWORD(nSBCode, -1), nPos);
}

void CGraphDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pscrlbar)
{
	ASSERT(pscrlbar == GetDlgItem(IDC_GRAPH_VSCRL));
	OnScroll(MAKEWORD(-1, nSBCode), nPos);
}

void CGraphDialog::OnScroll(UINT nScrollCode, UINT nPos)
{
	// get current (x,y) position
	int x = ((CScrollBar *)GetDlgItem(IDC_GRAPH_HSCRL))->GetScrollPos();
	int y = ((CScrollBar *)GetDlgItem(IDC_GRAPH_VSCRL))->GetScrollPos();

	switch (LOBYTE(nScrollCode))
	{
		case SB_TOP: x = 0; break;
		case SB_BOTTOM: x = m_sizeScroll.cx; break;
		case SB_THUMBTRACK: x = nPos; break;
		case SB_LINEUP: x -= m_line.cx; break;
		case SB_LINEDOWN: x += m_line.cx; break;
		case SB_PAGEUP: x -= m_page.cx; break;
		case SB_PAGEDOWN: x += m_page.cx; break;
	}

	switch (HIBYTE(nScrollCode))
	{
		case SB_TOP: y = 0; break;
		case SB_BOTTOM: y = m_sizeScroll.cy; break;
		case SB_THUMBTRACK: y = nPos; break;
		case SB_LINEUP: y -= m_line.cy; break;
		case SB_LINEDOWN: y += m_line.cy; break;
		case SB_PAGEUP: y -= m_page.cy; break;
		case SB_PAGEDOWN: y += m_page.cy; break;
	}

	// ensure within bounds
	if (x < 0)
		x = 0;
	else if (x > m_sizeScroll.cx)
		x = m_sizeScroll.cx;

	if (y < 0)
		y = 0;
	else if (y > m_sizeScroll.cy)
		y = m_sizeScroll.cy;

	// perform scroll
	m_pgraphwnd->SetGraphOrigin(x, y);

	// set new (x,y) pos
	((CScrollBar *)GetDlgItem(IDC_GRAPH_HSCRL))->SetScrollPos(x);
	((CScrollBar *)GetDlgItem(IDC_GRAPH_VSCRL))->SetScrollPos(y);
}


#endif // DEPGRAPH_VIEW
