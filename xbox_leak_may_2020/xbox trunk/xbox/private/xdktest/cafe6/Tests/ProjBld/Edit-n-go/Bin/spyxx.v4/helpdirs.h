// helpdirs.h


/////////////////////////////////////////////////////////////////////////////
// In Sushi, C3dDialog is the base class for all dialogs.  But other
// projects which share this file (books online & wizards) will probably
// want to use the standard MFC CDialog class.
#ifdef _SUSHI
#define CBaseDialog C3dDialog
#else
#define CBaseDialog CDialog
#endif


/////////////////////////////////////////////////////////////////////////////
// Functions
//
BOOL LocateHelpFile(const CString& strFName, CString& strHelpFile, BOOL bQueryUser);
BOOL IsFileThere(const CString& strDir, const CString& strFName, CString& strHelpFile);
BOOL HelpDirectoriesDialog(const CString& strFile);
void GenerateDefaultHelpDirectories();
CString AppendIntegerToString(int i, const TCHAR* sz);


/////////////////////////////////////////////////////////////////////////////
// CHelpDirDlg dialog
//
class CHelpDirDlg : public CBaseDialog
{
// Construction
public:
	CHelpDirDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHelpDirDlg)
	enum { IDD = IDD_NOHELPFILE };
	CString	m_strPath1;
	CString	m_strPath2;
	CString	m_strMessage;
	CStatic m_icon;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CHelpDirDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};
