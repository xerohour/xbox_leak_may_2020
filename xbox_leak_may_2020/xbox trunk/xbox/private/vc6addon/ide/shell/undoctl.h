// undoctl.h : undo/redo controls
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __UNDOCTL_H__
#define __UNDOCTL_H__

class CDropControl;

/////////////////////////////////////////////////////////////////////////////
// CGhostListBox control

class CGhostListBox : public CListBox
{
// Construction
public:
	CGhostListBox();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CGhostListBox();

	BOOL	Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CDropControl* pDropControl);
	
	CDropControl*	m_pDropControl;	// drop control to send all mouse msgs
	int 	GetBottomIndex();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGhostListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CGhostListBox)
};


/////////////////////////////////////////////////////////////////////////////
// CUndoList window

class CUndoList : public CWnd
{
public:
	enum {
		BORDERSIZE = 3,
		STATUSHEIGHT = 16
	};
	
// Construction
public:
	CUndoList();

// Attributes
public:

// Operations
public:
	void	ShowList(BOOL bShow, CRect rc, CDropControl* pParent);
	CRect	CalcStatusRect(CRect rc);
	void	SetSel(int index);
	int 	GetSel();
	int 	CalcSel(CPoint point);
	void	ScrollDown();
	void	ScrollUp();
	void	PageDown();
	void	PageUp();

// Implementation
public:
	virtual ~CUndoList();
	
	CGhostListBox	m_listBox;

	// Generated message map functions
protected:
	//{{AFX_MSG(CUndoList)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnDestroySelf(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CDropControl : public CWnd
{
public:
	DECLARE_DYNAMIC(CDropControl)
	
	enum {
		BS_DEFAULT = 0,
		BS_LCLICK = 1,
		BS_RCLICK = 2,
		BS_DROP = 3,
		BS_EXTRUDED=4,
		smallWidth = 37,
		smallHeight = 22,
		smallLeftWidth = 22,
		smallImageWidth=16,
		smallImageHeight=15,
		largeWidth = 53,
		largeHeight = 38,
		largeLeftWidth = 38,
		largeImageWidth=32,
		largeImageHeight=32,
		ID_TIMER = 89,
		MSEC_TIMER = 90
	};

			CDropControl();
	virtual	~CDropControl();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
	                    UINT nID, CCreateContext* pContext = NULL);
	void	CancelModes();

// Implementation
public:
	virtual void DrawArrow(CDC& dc, CDC& dcTemp, int xdest, int ydest, BOOL bEnabled);
	virtual void GetStatusString(int i, CString& str) = 0;
	virtual void FillUndoList(CListBox&) = 0;

	static int GetExpectedWidth(void);
	static int GetExpectedHeight(void);
		// use these functions to determine what size the control would be, if it were 
		// created now

	BOOL	PointOnControl(CPoint);
	BOOL	PointOnLeft(CPoint);
	BOOL	PointOnListBox(CPoint);
	BOOL	PointOnScrollDown(CPoint);
	BOOL	PointOnScrollUp(CPoint);
	void	UndoEvents(int n);
	void	SetState(int bsState);
	void	ShowList(BOOL bShow = TRUE);
	int 	CalcSel(CPoint point);
	void	InvalidateLeft();
	void	InvalidateRight();
	void	OnListBoxLButtonDown(UINT nFlags, CPoint point);
	void	OnListBoxLButtonUp(UINT nFlags, CPoint point);

	BOOL	m_bLarge;				// TRUE if currently using the large bitmaps
	int		m_width;				// current width of the control
	int		m_height;				// current height of the control
	int		m_leftWidth;			// current width of the left half of the control
	int 	m_buttonState;
	BOOL	m_bShowList;		// is the drop list currently shown?
	CPoint	m_mousePos;
	CWnd*	m_pMainWnd; 		// window to dispatch undo commands
	CUndoList	m_undoList;		// valid only when list is shown

// Generated message map functions
protected:
	void	SetButtonSize(BOOL bLarge);
		// internal call used to set up sizes of the window areas. Does not perform
		// any redraw
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	//{{AFX_MSG(CDropControl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg LRESULT OnBarSizeChanging(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	UINT m_idSmallEnabledButton;
	UINT m_idSmallDisabledButton;
	UINT m_idLargeEnabledButton;
	UINT m_idLargeDisabledButton;
};

class CUndoControl : public CDropControl
{
public:
	CUndoControl();
	virtual ~CUndoControl();

// Implementation
public:
	virtual	void GetStatusString(int i, CString& str);
	virtual void FillUndoList(CListBox&);

// Generated message map functions
protected:
	//{{AFX_MSG(CUndoControl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CRedoControl : public CDropControl
{
public:
	CRedoControl();
	virtual ~CRedoControl();

// Implementation
public:
	virtual	void GetStatusString(int i, CString& str);
	virtual void FillUndoList(CListBox&);

// Generated message map functions
protected:
	//{{AFX_MSG(CRedoControl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __UNDOCTL_H__
