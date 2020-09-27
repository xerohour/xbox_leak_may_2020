///////////////////////////////////////////////////////////////////////////////
//	graphdlg.cpp
//
//	Created by :			Date :
//		BrianCr				06/30/95
//
//	Description :
//		Implementation of the CGraphDlg class
//

#include "stdafx.h"
#include "graphdlg.h"
#include "graphwnd.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg dialog

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/, CSuiteDoc* pSuiteDoc /*= NULL*/)
	: CDialog(CGraphDlg::IDD, pParent),
	  m_pSuiteDoc(pSuiteDoc),
	  m_pwndGraph(NULL)
{
	// the doc must be valid
	ASSERT(m_pSuiteDoc);

	//{{AFX_DATA_INIT(CGraphDlg)
	//}}AFX_DATA_INIT
}

CGraphDlg::~CGraphDlg()
{
	delete m_pwndGraph;
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialog)
	//{{AFX_MSG_MAP(CGraphDlg)
	ON_CBN_SELCHANGE(IDC_GraphList, OnSelChangeGraphList)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg message handlers

BOOL CGraphDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// fill the graph list
	FillGraphList();

	// select the first graph
	// get the graph combo box
	CComboBox* pcbGraphs = (CComboBox*)GetDlgItem(IDC_GraphList);
	pcbGraphs->SetCurSel(0);
	OnSelChangeGraphList();

	// set the tab stops in the test list box
	CListBox* plbTests = (CListBox*)GetDlgItem(IDL_GraphTests);
	// get the size of the list box
	CRect rect;
	plbTests->GetClientRect(&rect);
	// get the horizontal dialog base units
	int nXBaseUnit = LOWORD(GetDialogBaseUnits());
	// set the tab stops at half the width of the list box
	// (figure the number of dialog units horizontally in the list box)
	int nTabStop = ((rect.Width() * 4) / nXBaseUnit) * 9 / 10;
	// set the tab stops
	plbTests->SetTabStops(nTabStop);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGraphDlg::OnSelChangeGraphList()
{
	// get the graph combo box
	CComboBox* pcbGraphs = (CComboBox*)GetDlgItem(IDC_GraphList);

	// get a pointer to the graph
	CGraph*	pGraph = (CGraph*)pcbGraphs->GetItemDataPtr(pcbGraphs->GetCurSel());
	ASSERT(pGraph->IsKindOf(RUNTIME_CLASS(CGraph)));

	// get the test list box
	CListBox* plbTests = (CListBox*)GetDlgItem(IDL_GraphTests);

	// create the graph window
	delete m_pwndGraph;
	m_pwndGraph = new CGraphWnd(pGraph, plbTests);
	m_pwndGraph->Create(CRect(0,0,0,0), this, 10001);

	// show and resize the graph window
	CRect rect;
	GetDlgItem(IDP_GraphView)->GetWindowRect(rect);
	ScreenToClient(&rect);
	m_pwndGraph->SetWindowPos(NULL,
							  rect.left, rect.top,
							  rect.Width(), rect.Height(),
							  SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);

	// compute the size of the scroll bars
	CalcScrollMetrics();
}

void CGraphDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT(pScrollBar == GetDlgItem(IDS_GraphViewHoriz));
	OnScroll(MAKEWORD(nSBCode, -1), nPos);
}

void CGraphDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT(pScrollBar == GetDlgItem(IDS_GraphViewVert));
	OnScroll(MAKEWORD(-1, nSBCode), nPos);
}

void CGraphDlg::OnScroll(UINT nScrollCode, UINT nPos)
{
	// get current (x,y) scroll bar position
	int x = ((CScrollBar *)GetDlgItem(IDS_GraphViewHoriz))->GetScrollPos();
	int y = ((CScrollBar *)GetDlgItem(IDS_GraphViewVert))->GetScrollPos();

	// figure new horizontal scroll bar position
	switch (LOBYTE(nScrollCode))
	{
		case SB_TOP: x = 0; break;
		case SB_BOTTOM: x = m_sizeScroll.cx; break;
		case SB_THUMBTRACK: x = nPos; break;
		case SB_LINEUP: x -= m_sizeLine.cx; break;
		case SB_LINEDOWN: x += m_sizeLine.cx; break;
		case SB_PAGEUP: x -= m_sizePage.cx; break;
		case SB_PAGEDOWN: x += m_sizePage.cx; break;
	}

	// figure new vertical scroll bar position
	switch (HIBYTE(nScrollCode))
	{
		case SB_TOP: y = 0; break;
		case SB_BOTTOM: y = m_sizeScroll.cy; break;
		case SB_THUMBTRACK: y = nPos; break;
		case SB_LINEUP: y -= m_sizeLine.cy; break;
		case SB_LINEDOWN: y += m_sizeLine.cy; break;
		case SB_PAGEUP: y -= m_sizePage.cy; break;
		case SB_PAGEDOWN: y += m_sizePage.cy; break;
	}

	// ensure within bounds
	if (x < 0) {
		x = 0;
	}
	else if (x > m_sizeScroll.cx) {
		x = m_sizeScroll.cx;
	}

	if (y < 0) {
		y = 0;
	}
	else if (y > m_sizeScroll.cy) {
		y = m_sizeScroll.cy;
	}

	// perform scroll
	m_pwndGraph->SetOrigin(x, y);

	// set new (x,y) pos
	((CScrollBar *)GetDlgItem(IDS_GraphViewHoriz))->SetScrollPos(x);
	((CScrollBar *)GetDlgItem(IDS_GraphViewVert))->SetScrollPos(y);
}

void CGraphDlg::FillGraphList(void)
{
	int nElement;
	POSITION pos;

	// get the graph combo box
	CComboBox* pcbGraphs = (CComboBox*)GetDlgItem(IDC_GraphList);

	// empty the list
	pcbGraphs->ResetContent();

	// iterate through the list of graphs in the forest
	CGraphForest* pForest = m_pSuiteDoc->GetGraphForest();
	for (pos = pForest->GetFirstGraphPosition(); pos != NULL; ) {
		// add each graph to the list
		CGraph* pGraph = pForest->GetNextGraph(pos);
		// only add non-empty graphs to the list
		if (!pGraph->IsEmpty()) {
			// add the graph name to the list
			nElement = pcbGraphs->AddString(pGraph->GetName());
			// store a pointer to the graph along with this item
			pcbGraphs->SetItemDataPtr(nElement, (void*)pGraph);
		}
	}
}

void CGraphDlg::CalcScrollMetrics(void)
{
	// set the ranges for our scroll bars
	m_sizeScroll = m_pwndGraph->GetSize();

	// get the size of the visible frame
	CRect rect;
	GetDlgItem(IDP_GraphView)->GetWindowRect(rect);

	// set the line and page sizes for scrolling
	m_sizeLine.cx = 16;
	m_sizeLine.cy = 16;
	m_sizePage.cx = rect.Width() - m_sizeLine.cx;
	m_sizePage.cy = rect.Height() - m_sizeLine.cy;
	
	((CScrollBar *)GetDlgItem(IDS_GraphViewHoriz))->SetScrollRange(0, m_sizeScroll.cx, FALSE);
	((CScrollBar *)GetDlgItem(IDS_GraphViewVert))->SetScrollRange(0, m_sizeScroll.cy, FALSE);
}

