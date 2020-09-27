// srchdlgs.h : header file
//

class CFindWindowDlg;
class CSearchWindowDlg;
class CSearchProcessDlg;
class CSearchThreadDlg;
class CSearchMessageDlg;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchWindowDlg window

class CSearchWindowDlg : public CDialog
{
// Construction
public:
	CSearchWindowDlg(CWnd *pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CSearchWindowDlg)
	enum { IDD = IDD_SEARCH_FINDWINDOW };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CSearchWindowDlg();
	HWND GetSelectedHwnd()
	{
		return(m_hwndSelected);
	}
	WORD GetSearchFlags()
	{
		return(m_wSearchFlags);
	}
	CString GetCaption()
	{
		return(m_strCaption);
	}
	CString GetClass()
	{
		return(m_strClass);
	}
	BOOL GetDirection()
	{
		return(m_fSearchUp);
	}

	void SetWindow(HWND hwndSet)
	{
		m_hwndSelected = hwndSet;
	}
	void SetCaption(CString strSet)
	{
		m_strCaption = strSet;
	}
	void SetClass(CString strSet)
	{
		m_strClass = strSet;
	}
	void SetDirection(BOOL fSearchUp)
	{
		m_fSearchUp = fSearchUp;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void SetSelectedWindow(HWND hwndSelected);

	void BeginFindWindowTracking();
	void UpdateFindWindowTracking(HWND hwndFound);
	void EndFindWindowTracking();

	HWND m_hwndSelected;
	HWND m_hwndFound;
	CFindToolIcon m_FindToolIcon;

	// data variables use in searching
	WORD 	m_wSearchFlags;
	CString	m_strCaption;
	CString m_strClass;
	BOOL	m_fSearchUp;	// TRUE if searching up, FALSE if searching down (from dialog)

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchWindowDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickedHide();
	afx_msg void VerifyEnteredHwnd();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CFindToolIcon;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlg window

class CSearchProcessDlg : public CDialog
{
// Construction
public:
	CSearchProcessDlg(CWnd *pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CSearchProcessDlg)
	enum { IDD = IDD_SEARCH_FINDPROCESS };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CSearchProcessDlg();
	DWORD GetSelectedProcess()
	{
		return(m_dwPrcSelected);
	}
	WORD GetSearchFlags()
	{
		return(m_wSearchFlags);
	}
	CString GetModule()
	{
		return(m_strModule);
	}
	BOOL GetDirection()
	{
		return(m_fSearchUp);
	}

	void SetSelectedProcess(DWORD dwPrcSelected)
	{
		m_dwPrcSelected = dwPrcSelected;
	}
	void SetModule(CString strSet)
	{
		m_strModule = strSet;
	}
	void SetDirection(BOOL fSearchUp)
	{
		m_fSearchUp = fSearchUp;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DWORD m_dwPrcSelected;

	// data variables use in searching
	WORD 	m_wSearchFlags;
	CString	m_strModule;
	BOOL	m_fSearchUp;	// TRUE if searching up, FALSE if searching down (from dialog)

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchProcessDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchThreadDlg window

class CSearchThreadDlg : public CDialog
{
// Construction
public:
	CSearchThreadDlg(CWnd *pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CSearchThreadDlg)
	enum { IDD = IDD_SEARCH_FINDTHREAD };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CSearchThreadDlg();
	DWORD GetSelectedThread()
	{
		return(m_dwThdSelected);
	}
	WORD GetSearchFlags()
	{
		return(m_wSearchFlags);
	}
	CString GetModule()
	{
		return(m_strModule);
	}
	BOOL GetDirection()
	{
		return(m_fSearchUp);
	}

	void SetSelectedThread(DWORD dwThdSelected)
	{
		m_dwThdSelected = dwThdSelected;
	}
	void SetModule(CString strSet)
	{
		m_strModule = strSet;
	}
	void SetDirection(BOOL fSearchUp)
	{
		m_fSearchUp = fSearchUp;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DWORD m_dwThdSelected;

	// data variables use in searching
	WORD	m_wSearchFlags;
	CString	m_strModule;
	BOOL	m_fSearchUp;	// TRUE if searching up, FALSE if searching down (from dialog)

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchThreadDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchMessageDlg window

class CSearchMessageDlg : public CDialog
{
// Construction
public:
	CSearchMessageDlg(CWnd *pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CSearchMessageDlg)
	enum { IDD = IDD_SEARCH_FINDMESSAGE };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CSearchMessageDlg();
	HWND GetWindow()
	{
		return(m_hwndSelected);
	}
	WORD GetMessage()
	{
		return(m_wMsgSelected);
	}
	WORD GetType()
	{
		return(m_uTypeSelected);
	}
	WORD GetSearchFlags()
	{
		return(m_wSearchFlags);
	}
	BOOL GetDirection()
	{
		return(m_fSearchUp);
	}

	void SetWindow(HWND hwndSelected)
	{
		m_hwndSelected = hwndSelected;
	}
	void SetMessage(WORD wMsgSelected)
	{
		m_wMsgSelected = wMsgSelected;
	}
	void SetType(UINT uTypeSelected)
	{
		m_uTypeSelected = uTypeSelected;
	}
	void SetDirection(BOOL fSearchUp)
	{
		m_fSearchUp = fSearchUp;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void SetSelectedWindow(HWND hwndSelected);

	void BeginFindWindowTracking();
	void UpdateFindWindowTracking(HWND hwndFound);
	void EndFindWindowTracking();

	HWND m_hwndTrack;
	HWND m_hwndSelected;
	CFindToolIcon m_FindToolIcon;

	// data variables use in searching
	WORD	m_wSearchFlags;
	WORD	m_wMsgSelected;
	UINT	m_uTypeSelected;
	BOOL	m_fSearchUp;	// TRUE if searching up, FALSE if searching down (from dialog)

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchMessageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickedHide();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CFindToolIcon;
};

/////////////////////////////////////////////////////////////////////////////
