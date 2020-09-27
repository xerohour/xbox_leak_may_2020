#ifndef __SPLITTER_H__
#define __SPLITTER_H__

// Splitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitter class

class CSplitter : public CWnd
{
protected:
   CRect	m_rcFirstPane;
   CRect	m_rcSecondPane;

   CWnd		*m_pFirstPane;
   CWnd		*m_pSecondPane;

   CRect	m_rcTrack;
   BOOL		m_fTracking;
   BOOL		m_fVisible;
   CWnd		*m_pParent;
   int		m_nWidth;

public:
   CSplitter();
   BOOL Create(CWnd *pParent, RECT *rect = NULL);
   RECT	GetFirstPaneRect();
   RECT	GetSecondPaneRect();
   void SetFirstPane( CWnd *pPane );
   void SetSecondPane( CWnd *pPane );
   void SetTracker(RECT& rect)
   {
	   m_rcTrack = rect;
   }

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
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
   afx_msg void OnCaptureChanged(CWnd *pWnd);
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SPLITTER_H__
