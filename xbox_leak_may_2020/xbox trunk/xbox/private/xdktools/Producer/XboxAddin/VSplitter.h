#ifndef __VSPLITTER_H__
#define __VSPLITTER_H__

// VSplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVSplitter class

#define DEFAULTSPLITTERWIDTH (6)
#define MINSPLITTERXPOS (30)

class CVSplitter : public CWnd
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
   CVSplitter();
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

   //{{AFX_VIRTUAL(CVSplitter)
   //}}AFX_VIRTUAL

public:
   virtual ~CVSplitter();

protected:
   //{{AFX_MSG(CVSplitter)
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnCancelMode();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
   afx_msg void OnCaptureChanged(CWnd *pWnd);
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __VSPLITTER_H__
