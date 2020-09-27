///////////////////////////////////////////////////////////////////////////////
//	graphwnd.h
//
//	Created by :			Date :
//		BrianCr				07/03/95
//
//	Description :
//		Interface of the CGraphWnd class
//

#ifndef __GRAPHWND_H__
#define __GRAPHWND_H__

#include "graph.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphWnd window

class CGraphWnd : public CWnd
{
// Construction
public:
	CGraphWnd(CGraph* pGraph, CListBox* plb);

// Attributes
public:
	CSize GetSize(void);

// Operations
public:
	void SetOrigin(int x, int y);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphWnd)
	public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// data types
protected:
	// data type that will store information about each vertex displayed
	struct VertexInfo {
	public:
		VertexInfo(CVertex* pvertex) : m_pVertex(pvertex) { }
		CVertex* m_pVertex;
		CRect m_rect;
	};

	// a graph is made up of a set of vertices
	typedef CArray<VertexInfo*, VertexInfo*> VertexArray;

// operations
protected:
	void StoreVertexInfo(CVertex* pVertex);
	void DrawVertex(CDC* pDC, int index);
	void DrawEdge(CDC* pDC, int srcindex, int destindex);
	void FillTestList(void);

// data
protected:
	CGraph* m_pGraph;
	CListBox* m_plbTests;
	VertexArray m_aVertices;

	CPoint m_ptOrigin;

	int m_nSelVertex;

	static const cVertexWidth;
	static const cVertexHeight;
	static const cXSeparator;
	static const cYSeparator;
	static const cXBorder;
	static const cYBorder;

};

/////////////////////////////////////////////////////////////////////////////

#endif // __GRAPHWND_H__
