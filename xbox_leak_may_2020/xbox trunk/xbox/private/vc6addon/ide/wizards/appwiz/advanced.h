// advanced.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDlg dialog

class CAdvancedDlg : public CTabbedDialog
{
public:
	CAdvancedDlg(CWnd* pParent);
};

/////////////////////////////////////////////////////////////////////////////
// CFrameStylesDlg dialog

class CFrameStylesDlg : public CDlgTab
{
// Construction
public:
	CFrameStylesDlg();

// Dialog Data
	//{{AFX_DATA(CFrameStylesDlg)
	enum { IDD = IDD_FRAME_STYLES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL ValidateTab();
	virtual void CommitTab();

	// Generated message map functions
	//{{AFX_MSG(CFrameStylesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMaximize();
	afx_msg void OnMinimize();
	afx_msg void OnChildMaximize();
	afx_msg void OnChildMinimize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDocStringsDlg dialog

class CDocStringsDlg : public CDlgTab
{
// Construction
public:
	CDocStringsDlg();

// Dialog Data
	//{{AFX_DATA(CDocStringsDlg)
	enum { IDD = IDD_DOCSTRINGS };
	CString	m_strDocTag;
	CString	m_strDocFileExt;
	CString	m_strDocFileNew;
	CString	m_strDocFilter;
	CString	m_strDocRegID;
	CString	m_strDocRegName;
	//}}AFX_DATA
	CString m_strTitle;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void UpdateDocFields();	
	virtual BOOL ValidateTab();
	virtual void CommitTab();
	BOOL ValidateCurrLang();
	void DisplayCurrLang();

	// Generated message map functions
	//{{AFX_MSG(CDocStringsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeDocType();
	afx_msg void OnChangeDocExt();
	afx_msg void OnChangeFileNew();
	afx_msg void OnChangeFilter();
	afx_msg void OnChangeRegId();
	afx_msg void OnChangeRegName();
	//}}AFX_MSG
	// FUTURE: Multiple langs:
	//afx_msg void OnSelchangeLangCombo();
	DECLARE_MESSAGE_MAP()

	int m_nCurrLang;
};

/////////////////////////////////////////////////////////////////////////////
// CMacDlg dialog

class CMacDlg : public CDlgTab
{
// Construction
public:
	CMacDlg();

// Dialog Data
	//{{AFX_DATA(CMacDlg)
	enum { IDD = IDD_MAC };
	//}}AFX_DATA
	CString	m_strDocFileCreator;
	CString	m_strDocFileType;
	CString	m_strDocFilter;

// Implementation
protected:
	BOOL m_bTrackFilter;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL ValidateTab();
	virtual void CommitTab();

	// Generated message map functions
	//{{AFX_MSG(CMacDlg)
	afx_msg void OnChangeFileType();
	afx_msg void OnChangeFilter();
	afx_msg void OnChangeDocCreator();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
