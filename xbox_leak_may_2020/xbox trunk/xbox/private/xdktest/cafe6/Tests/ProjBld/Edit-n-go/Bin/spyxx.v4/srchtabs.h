// srchtabs.h : header file
//


class CSearchTabbedDialog;
class CSearchGeneralDlgTab;
class CSearchWindowDlgTab;
class CSearchProcessDlgTab;
class CSearchThreadDlgTab;
class CSearchMessageDlgTab;

/////////////////////////////////////////////////////////////////////////////
// CSearchTabbedDialog window

class CSearchTabbedDialog : public CTabbedDialog
{
// Construction
public:
	CSearchTabbedDialog(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectTab = -1)
		: CTabbedDialog(nIDCaption, pParentWnd, iSelectTab) {}

// Implementation
public:
	virtual ~CSearchTabbedDialog();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchTabbedDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CSearchGeneralDlgTab;
	friend class CSearchWindowDlgTab;
	friend class CSearchProcessDlgTab;
	friend class CSearchThreadDlgTab;
	friend class CSearchMessageDlgTab;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchObjectDlgTab window

class CSearchObjectDlgTab : public CDlgTab
{
// Construction
public:
	CSearchObjectDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) {}

// Implementation
public:
	virtual ~CSearchObjectDlgTab();
	DWORD GetSelectedObject()
	{
		return(m_dwObjSelected);
	}

	void SetSelectedObject(DWORD dwObjSelected)
	{
		m_dwObjSelected = dwObjSelected;
	}

protected:
	virtual BOOL ValidateTab();
	virtual void CommitTab();

	DWORD m_dwObjSelected;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchObjectDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CSearchTabbedDialog;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchWindowDlgTab window

class CSearchWindowDlgTab : public CDlgTab
{
// Construction
public:
	CSearchWindowDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) {}

// Implementation
public:
	virtual ~CSearchWindowDlgTab();
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

protected:
	virtual BOOL ValidateTab();
	virtual void CommitTab();

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

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchWindowDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedHide();
	afx_msg void VerifyEnteredHwnd();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CFindToolIcon;
	friend class CSearchTabbedDialog;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchProcessDlgTab window

class CSearchProcessDlgTab : public CDlgTab
{
// Construction
public:
	CSearchProcessDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) {}

// Implementation
public:
	virtual ~CSearchProcessDlgTab();
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

	void SetSelectedProcess(DWORD dwPrcSelected)
	{
		m_dwPrcSelected = dwPrcSelected;
	}
	void SetModule(CString strSet)
	{
		m_strModule = strSet;
	}

protected:
	virtual BOOL ValidateTab();
	virtual void CommitTab();

	DWORD m_dwPrcSelected;

	// data variables use in searching
	WORD 	m_wSearchFlags;
	CString	m_strModule;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchProcessDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CSearchTabbedDialog;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchThreadDlgTab window

class CSearchThreadDlgTab : public CDlgTab
{
// Construction
public:
	CSearchThreadDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) {}

// Implementation
public:
	virtual ~CSearchThreadDlgTab();
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

	void SetSelectedThread(DWORD dwThdSelected)
	{
		m_dwThdSelected = dwThdSelected;
	}
	void SetModule(CString strSet)
	{
		m_strModule = strSet;
	}

protected:
	virtual BOOL ValidateTab();
	virtual void CommitTab();

	DWORD m_dwThdSelected;

	// data variables use in searching
	WORD	m_wSearchFlags;
	CString	m_strModule;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchThreadDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CSearchTabbedDialog;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSearchMessageDlgTab window

class CSearchMessageDlgTab : public CDlgTab
{
// Construction
public:
	CSearchMessageDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) {}

// Implementation
public:
	virtual ~CSearchMessageDlgTab();
	HWND GetWindow()
	{
		return(m_hwndSelected);
	}
	WORD GetMessage()
	{
		return(m_wMsgSelected);
	}
	WORD GetSearchFlags()
	{
		return(m_wSearchFlags);
	}

	void SetWindow(HWND hwndSelected)
	{
		m_hwndSelected = hwndSelected;
	}
	void SetMessage(WORD wMsgSelected)
	{
		m_wMsgSelected = wMsgSelected;
	}

protected:
	virtual BOOL ValidateTab();
	virtual void CommitTab();

	void SetSelectedWindow(HWND hwndSelected);

	void BeginFindWindowTracking();
	void UpdateFindWindowTracking(HWND hwndFound);
	void EndFindWindowTracking();

	CFindToolIcon m_FindToolIcon;
	HWND m_hwndTrack;

	HWND m_hwndSelected;
	WORD m_wMsgSelected;

	// data variables use in searching
	WORD	m_wSearchFlags;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchMessageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedHide();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CFindToolIcon;
	friend class CSearchTabbedDialog;
};

/////////////////////////////////////////////////////////////////////////////
