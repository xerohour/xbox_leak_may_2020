// filtrdlg.h : header file
//


class CFiltersTabbedDialog;
class CFiltersWindowDlgTab;
class CFiltersMessageDlgTab;
class CFiltersOutputDlgTab;

/////////////////////////////////////////////////////////////////////////////
// CFiltersTabbedDialog window

class CFiltersTabbedDialog : public CTabbedDialog
{
// Construction
public:
	CFiltersTabbedDialog(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectTab = -1)
		: CTabbedDialog(nIDCaption, pParentWnd, iSelectTab) {}

// Implementation
public:
	virtual ~CFiltersTabbedDialog();
	void SetDocPointer(CMsgDoc *pDoc)
	{
		m_pDoc = pDoc;
	}

protected:
	CMsgDoc *m_pDoc;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFiltersTabbedDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CFiltersWindowDlgTab;
	friend class CFiltersMessageDlgTab;
	friend class CFiltersOutputDlgTab;
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFiltersWindowDlgTab window

class CFiltersWindowDlgTab : public CDlgTab
{
// Construction
public:
	CFiltersWindowDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CFiltersWindowDlgTab)
	enum { IDD = IDD_FILTERSWINDOWTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CFiltersWindowDlgTab();

protected:
	virtual BOOL ValidateTab();
	virtual void CommitTab();
	virtual void CancelTab();

	void BeginFindWindowTracking();
	void UpdateFindWindowTracking(HWND hwndFound);
	void EndFindWindowTracking();

	void UpdateSOBFields();
	void ShowSOBLabels(int nObjectType);
	void SetSelectedWindow(HWND hwndSelected);
	void SetSelectedThread(DWORD tid);
	void SetSelectedProcess(DWORD pid);
	void EnableCheckboxes(BOOL fEnable);
	void GraySOB(BOOL fGray);

	CMsgDoc *m_pDoc;
	HWND m_hwndSelected;
	HWND m_hwndFound;
	int m_nObjectType;
	CFindToolIcon m_FindToolIcon;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFiltersWindowDlgTab)
	afx_msg void OnClickedWinFiltersAllWindows();
	afx_msg void OnClickedHide();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CFindToolIcon;
	friend class CFiltersTabbedDialog;
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFiltersMessageDlgTab window

class CFiltersMessageDlgTab : public CDlgTab
{
// Construction
public:
	CFiltersMessageDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption)
	{
		m_paiSelected = NULL;
		m_paiSelected2 = NULL;
	}

// Dialog Data
	//{{AFX_DATA(CFiltersMessageDlgTab)
	enum { IDD = IDD_FILTERSMESSAGETAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CFiltersMessageDlgTab();

protected:
	virtual void CommitTab();

	void UpdateCheckBoxes();

	CMsgDoc * m_pDoc;
	int * m_paiSelected;
	int * m_paiSelected2;
	int m_cItemsSave;

private:
	static BOOL m_fInitializedStatic;		   // TRUE if static data init'd
	static MSGGROUP m_aMsgGroup[];
	static INT m_cMsgGroups;

	static void InitStaticData();

	void OnClickedGroup(int iGroup);

	// Generated message map functions
	//{{AFX_MSG(CFiltersMessageDlgTab)
	afx_msg void OnClickedMsgFiltersAll();
	afx_msg void OnClickedMsgFiltersNone();
	afx_msg void OnClickedMsgFiltersBM();
	afx_msg void OnClickedMsgFiltersCB();
	afx_msg void OnClickedMsgFiltersClip();
	afx_msg void OnClickedMsgFiltersDDE();
	afx_msg void OnClickedMsgFiltersEM();
	afx_msg void OnClickedMsgFiltersKeybd();
	afx_msg void OnClickedMsgFiltersLB();
	afx_msg void OnClickedMsgFiltersMouse();
	afx_msg void OnClickedMsgFiltersNC();
	afx_msg void OnClickedMsgFiltersSTM();
	afx_msg void OnClickedMsgFiltersMDI();
	afx_msg void OnClickedMsgFiltersSBM();
	afx_msg void OnClickedMsgFiltersIME();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMsgFiltersList();
	afx_msg void OnClickedMsgFiltersDlg();
	afx_msg void OnClickedMsgFiltersAni();
	afx_msg void OnClickedMsgFiltersHdr();
	afx_msg void OnClickedMsgFiltersHK();
	afx_msg void OnClickedMsgFiltersLV();
	afx_msg void OnClickedMsgFiltersProg();
	afx_msg void OnClickedMsgFiltersStat();
	afx_msg void OnClickedMsgFiltersTab();
	afx_msg void OnClickedMsgFiltersTB();
	afx_msg void OnClickedMsgFiltersTrk();
	afx_msg void OnClickedMsgFiltersTT();
	afx_msg void OnClickedMsgFiltersTV();
	afx_msg void OnClickedMsgFiltersUD();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CFiltersTabbedDialog;
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFiltersOutputDlgTab window

class CFiltersOutputDlgTab : public CDlgTab
{
// Construction
public:
	CFiltersOutputDlgTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CFiltersOutputDlgTab)
	enum { IDD = IDD_FILTERSOUTPUTTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CFiltersOutputDlgTab();

protected:
	virtual BOOL ValidateTab();
	virtual void CommitTab();

	CMsgDoc *m_pDoc;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFiltersOutputDlgTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedLogToFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CFiltersTabbedDialog;
};

/////////////////////////////////////////////////////////////////////////////
