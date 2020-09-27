// wndptabs.h : header file
//


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CWindowGeneralPropTab window

class CWindowGeneralPropTab : public CPropertyTab
{
// Construction
public:
	CWindowGeneralPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CWindowGeneralPropTab)
	enum { IDD = IDD_WINDOWGENERALTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CWindowGeneralPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowGeneralPropTab)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CWindowStylesPropTab window

class CWindowStylesPropTab : public CPropertyTab
{
// Construction
public:
	CWindowStylesPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CWindowStylesPropTab)
	enum { IDD = IDD_WINDOWSTYLESTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CWindowStylesPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowStylesPropTab)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CWindowWindowsPropTab window

class CWindowWindowsPropTab : public CPropertyTab
{
// Construction
public:
	CWindowWindowsPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CWindowWindowsPropTab)
	enum { IDD = IDD_WINDOWWINDOWSTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CWindowWindowsPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	CHotLinkCtrlWnd m_HotLinkNextWindow;
	CHotLinkCtrlWnd m_HotLinkPrevWindow;
	CHotLinkCtrlWnd m_HotLinkParentWindow;
	CHotLinkCtrlWnd m_HotLinkFirstChild;
	CHotLinkCtrlWnd m_HotLinkOwnerWindow;

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowWindowsPropTab)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CWindowClassPropTab window

class CWindowClassPropTab : public CPropertyTab
{
// Construction
public:
	CWindowClassPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CWindowClassPropTab)
	enum { IDD = IDD_WINDOWCLASSTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CWindowClassPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowClassPropTab)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CWindowProcessPropTab window

class CWindowProcessPropTab : public CPropertyTab
{
// Construction
public:
	CWindowProcessPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CWindowProcessPropTab)
	enum { IDD = IDD_WINDOWPROCESSTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CWindowProcessPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	CHotLinkCtrlPrc m_HotLinkProcessID;
	CHotLinkCtrlThd m_HotLinkThreadID;

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowProcessPropTab)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
