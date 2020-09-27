// CGraphPainter, CGraphWnd, CGraphDialog
//
// Graph viewer -- given basic information about the graph it causes
// the graph to be layed out and then renders it
//
// [ricom] (modified by [matthewt])

#ifndef _GRVIEW_H
#define _GRVIEW_H

#ifndef _INCLUDE_DEPGRAPH_H
#error Expected depgraph.h to be included before grview.h
#endif

#ifndef DEPGRAPH_VIEW
#error grview.h should only be used if DEPGRAPH_VIEW is defined.
#endif

#ifdef DEPGRAPH_VIEW

#include "layout.h"

////////////////////////////////////////
// CGraphLegend - graph legend window //
////////////////////////////////////////

class CGraphLegend : public CWnd
{
	DECLARE_DYNAMIC(CGraphLegend)
	DECLARE_MESSAGE_MAP()

public:
	CGraphLegend() {}
	virtual ~CGraphLegend() {}

	// create the graph legend
	virtual BOOL Create(UINT nID, CRect rect, CWnd * pParentWnd = (CWnd *)NULL);

protected:
	// message map for window
	//{{AFX_MSG(CGraphLegend)
	virtual	void	OnPaint();
	//}}AFX_MSG
};

/////////////////////////////////////////////////////////////////////////////////
// CGraphPainter - generic protocol for an object that paints nodes for a view //
/////////////////////////////////////////////////////////////////////////////////

class CGraphPainter
{
public:
	CGraphPainter() {}
	virtual ~CGraphPainter() {}

	virtual	BOOL FInitGraphCDC(CDC *, BOOL fErase);

	virtual void GetNidSize(NID, CSize &, CDC *) {}
	virtual void PaintAt(NID, int, int, CDC *) {}
	virtual void PaintAnchor(NID, int, int, int, int, CDC *);
	virtual void PaintSelect(NID, int, int, int, int, CDC *);
	virtual void PaintDeAnchor(NID, int, int, int, int, CDC *);
	virtual void PaintDeSelect(NID, int, int, int, int, CDC *);

	virtual void GetOffsets(CDC *,
							NID nidFrom,
						    NID nidTo,
							ET  et,
							int & cyFromOffset,
						    int & cyToOffset
							);

	virtual void GetSpacing(CDC *,
							int & cxMinTag,		// size of stub arrows go on
							int & cxEdgeSpace,  // space for edges
							int &  xLeftAbs,	// x origin of graph
							int &  yTopAbs,		// y origin of graph
							int & cyFakeHeight  // height of fake nodes
						   );
};

////////////////////////
// CDepGraphPainter   //
////////////////////////

class CDepGraphPainter : public CGraphPainter
{

public:
	CDepGraphPainter() {m_dirCur = g_pActiveProject->GetProjDir();}
	virtual ~CDepGraphPainter() {}

	virtual FInitGraphCDC(CDC *, BOOL);

	virtual void GetNidSize(NID, CSize &, CDC *);
	virtual void PaintAt(NID, int, int, CDC *);

private:
	void GetNidText(NID nid, CString & strText);

	// current directory
	CDir m_dirCur;
};

/////////////////
// CGraphWnd   //
/////////////////

__inline USHORT _abs(int a) {return (a>=0) ? a : -a;}

#define MapCol(col)	(m_fReversed ? (m_icolLim - (col)) : (col))

// return the center point of the given row
#define YcoCenter(row) ((m_rgYTop[(row)] + m_rgYTop[(row)+1])/2)
#define CyLine(irow) ((int)(m_rgYTop[irow+1] - m_rgYTop[(irow)]))

class CGraphWnd : public CWnd
{
	DECLARE_DYNAMIC(CGraphWnd)
	DECLARE_MESSAGE_MAP()

public:
	CGraphWnd(CGraphPainter *, CNidGraphDoc *, BOOL = FALSE, USHORT = 0);
	virtual ~CGraphWnd();

	// create the graph
	virtual BOOL Create(UINT nID, CRect rect, CWnd * pParentWnd = (CWnd *)NULL);

	// get the graph size
	void GetGraphSize(CSize & size);

	// set the origin of the graph
	void SetGraphOrigin(int x, int y);

	// refresh the graph
	BOOL RefreshGraph();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// message map for window
	//{{AFX_MSG(CGraphWnd)
	virtual	void	OnPaint();
	//}}AFX_MSG

	USHORT			m_usDepth;
	CLayout * 		m_play;
	CGraphPainter *	m_ppainter;
	CNidGraphDoc *	m_pdoc;
	USHORT			m_irowVisMin;
	USHORT			m_icolVisMin;
	USHORT			m_icolMac;
	USHORT			m_icolLim;
	USHORT			m_irowMac;
	USHORT			m_crowsVis;
	BOOL			m_fPositioned;
	BOOL			m_fReversed;
	int				m_xLeftAbs;
	int				m_yTopAbs;
	int				m_cxMinTag;
	int				m_cxEdgeSpace;
	int				m_cyFakeHeight;
	int	*			m_rgXLeft;
	int	*			m_rgXRight;
	long *			m_rgYTop;

	virtual BOOL	PreCreateWindow(CREATESTRUCT & cs);

protected:
	virtual void		PositionNodes();
	virtual USHORT		IColVisLim();
	virtual CGrNode *	PnodeSearchBest(USHORT, USHORT, USHORT);
	virtual CGrNode *	PnodeSearchCorner(USHORT, USHORT, USHORT, USHORT);

private:
	void UpdateVisibleRows();

	// the graph origin
	CPoint m_ptOrig;
};

/////////////////////////////////////////////////////////////////////////////
// CGraphDialog

class CGraphDialog : public CDialog
{
	DECLARE_DYNAMIC(CGraphDialog)
	DECLARE_MESSAGE_MAP()

public:
	CGraphDialog(CGraph * pgraph, CWnd * pParentWnd = NULL);
	virtual ~CGraphDialog();

	virtual BOOL OnInitDialog();

protected:
	//{{AFX_MSG(CGraphDialog)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT, UINT, CScrollBar *);
	afx_msg void OnVScroll(UINT, UINT, CScrollBar *);
	afx_msg void OnShowDeps();
	//}}AFX_MSG

	void OnScroll(UINT, UINT);

	// calculate the scroll metrics
	void CalcScrollMetrics();

private:
	// pointer to graph
	CGraph * m_pgraph;

	// window in which to paint graph
	CGraphWnd * m_pgraphwnd;

	// window in which to paint graph legend
	CGraphLegend * m_pgraphlgnd;

	// graph painter
	CGraphPainter * m_ppainter;

	// scroll range
	CSize m_sizeScroll;

	// page size
	CSize m_page;

	// line size
	CSize m_line;
};

#endif // DEPGRAPH_VIEW
#endif // _GRVIEW_H 
