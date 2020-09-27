// page1dlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CPage1Dlg dialog

class CPage1Dlg : public CAppWizStepDlg
{
// Construction
public:
	CPage1Dlg();   // standard constructor

 virtual BOOL OnDismiss();
 BOOL InitDrawingObjects() ;
// Dialog Data
	//{{AFX_DATA(CPage1Dlg)
	enum { IDD = IDD_PAGE1 };
	CButton	m_EditScriptButton;
	CEdit	m_DriverCtrl;
	CEdit	m_TestSetCtrl;
	CString	m_DriverClass;
	CString	m_DriverH;
	CString	m_DriverCpp;
	CString	m_TestOwner;
	CString	m_TestSetClass;
	CString	m_TestSetCpp;
	CString	m_TestSetH;
	CString	m_TestTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPage1Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPage1Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditscript();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
