// findwdlg.h : header file
//

class CFindWindowDlg;
class CSearchWindowDlg;
class CSearchProcessDlg;
class CSearchThreadDlg;
class CSearchMessageDlg;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFindWindowDlg window

class CFindWindowDlg : public CDialog
{
// Construction
public:
	CFindWindowDlg(CWnd *pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CFindWindowDlg)
	enum { IDD = IDD_FINDWINDOW };
	BOOL	m_ShowProperties;
	BOOL	m_ShowMessages;
	//}}AFX_DATA

// Implementation
public:
	virtual ~CFindWindowDlg();
	HWND GetSelectedHwnd()
	{
		return(m_hwndSelected);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void BeginFindWindowTracking();
	void UpdateFindWindowTracking(HWND hwndFound);
	void EndFindWindowTracking();

	void SetSelectedWindow(HWND hwndSelected);

	HWND m_hwndSelected;
	HWND m_hwndFound;
	CFindToolIcon m_FindToolIcon;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindWindowDlg)
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
