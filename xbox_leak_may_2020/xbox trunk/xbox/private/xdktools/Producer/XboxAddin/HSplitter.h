#ifndef __HSPLITTER_H__
#define __HSPLITTER_H__

// HSplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHSplitter class

#define DEFAULTSPLITTERHEIGHT (6)
#define MINSPLITTERYPOS (30)

class CEndTrack : public CDialog
{
public:
	CEndTrack(UINT nIDTemplate,CWnd* pParentWnd ) : CDialog( nIDTemplate, pParentWnd ) {}
    virtual void EndTrack( long lNewPos ) = 0;
};


class CHSplitter : public CWnd
{
protected:
   CRect	m_rcFirstPane;
   CRect	m_rcSecondPane;

   CWnd		*m_pFirstPane;
   CWnd		*m_pSecondPane;

   CRect	m_rcTrack;
   BOOL		m_fTracking;
   BOOL		m_fVisible;
   CEndTrack		*m_pParent;
   int		m_nHeight;
   long     m_lHeightOffset;

public:
   CHSplitter();
   BOOL Create(CEndTrack *pParent, RECT *rect = NULL);
   RECT	GetFirstPaneRect();
   RECT	GetSecondPaneRect();
   void SetFirstPane( CWnd *pPane );
   void SetSecondPane( CWnd *pPane );
   void SetTracker(RECT& rect)
   {
	   m_rcTrack = rect;
   }
   void SetHeightOffset(long lOffset)
   {
       m_lHeightOffset = lOffset;
       m_rcFirstPane.top = lOffset;
   }

protected:
   void OnEndCapture();
   void InvertTracker();

   //{{AFX_VIRTUAL(CHSplitter)
   //}}AFX_VIRTUAL

public:
   virtual ~CHSplitter();

protected:
   //{{AFX_MSG(CHSplitter)
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

#endif // __HSPLITTER_H__
