#ifndef __SPLITTER_H__
#define __SPLITTER_H__

// Splitter.h : header file
//

#define SPLITTER_HEIGHT			3
#define STYLE_DLG_BORDER		2

/////////////////////////////////////////////////////////////////////////////
// CSplitter class

class CSplitter : public CWnd
{
protected:
   CRect	m_rcTrack;
   BOOL		m_fTracking;
   BOOL		m_fVisible;
   int		m_nTopBorder;
   CWnd		*m_pParent;

public:
   CSplitter();
   BOOL Create(CWnd *pParent, RECT *rect = NULL);
   void SetTopBorder( int nTopBorder );

protected:
   void OnEndCapture();
   void InvertTracker();

   //{{AFX_VIRTUAL(CSplitter)
   //}}AFX_VIRTUAL

public:
   virtual ~CSplitter();

protected:
   //{{AFX_MSG(CSplitter)
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnCancelMode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
   afx_msg void OnCaptureChanged(CWnd *pWnd);
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SPLITTER_H__
