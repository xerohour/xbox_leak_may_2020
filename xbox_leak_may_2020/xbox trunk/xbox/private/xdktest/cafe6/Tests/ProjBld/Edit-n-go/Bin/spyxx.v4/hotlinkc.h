// hotlinkc.h : header file
//


//
// Sent to the parent of a hot link control window.  The
// lParam is the link type (OT_* define) and the wParam
// is the handle of the object that should be linked to,
// such as the hwnd or the process id.
//
#define WM_HOTLINK	(WM_USER+150)



/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrl window

class CHotLinkCtrl : public CWnd
{
// Construction
public:
	CHotLinkCtrl();

// Attributes
public:

// Operations
public:
	BOOL IsJumpable()
	{
		return m_fJumpable;
	}

// Implementation
public:
	virtual ~CHotLinkCtrl();
	virtual int GetObjectType() = 0;
	virtual void SetJumpObject(DWORD dwObject);
	virtual void MarkInvalid();

protected:
	DWORD m_dwJumpObject;
	CString m_strCaption;
	BOOL m_fJumpable;

public:
	static void InitStaticData();

protected:
	static HCURSOR m_hcurHand;
	static CFont m_FontJump;
	static COLORREF m_clrJump;

	// Generated message map functions
protected:
	//{{AFX_MSG(CHotLinkCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrlWnd window

class CHotLinkCtrlWnd : public CHotLinkCtrl
{
// Construction
public:
	CHotLinkCtrlWnd();

// Implementation
public:
	virtual ~CHotLinkCtrlWnd();
	virtual int GetObjectType()
	{
		return OT_WINDOW;
	}
	virtual void SetJumpObject(DWORD dwObject);

	// Generated message map functions
protected:
	//{{AFX_MSG(CHotLinkCtrlWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrlPrc window

class CHotLinkCtrlPrc : public CHotLinkCtrl
{
// Construction
public:
	CHotLinkCtrlPrc();

// Implementation
public:
	virtual ~CHotLinkCtrlPrc();
	virtual int GetObjectType()
	{
		return OT_PROCESS;
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CHotLinkCtrlPrc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CHotLinkCtrlThd window

class CHotLinkCtrlThd : public CHotLinkCtrl
{
// Construction
public:
	CHotLinkCtrlThd();

// Implementation
public:
	virtual ~CHotLinkCtrlThd();
	virtual int GetObjectType()
	{
		return OT_THREAD;
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CHotLinkCtrlThd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
