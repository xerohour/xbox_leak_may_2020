// apiwidlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CApiwizDlg Tabbed Dialog

class CApiwizDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CApiwizDlg)

// Construction
public:
	CApiwizDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CApiwizDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApiwizDlg)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CApiwizDlg();
protected:
	HICON m_hIcon;	// Review: (Delete)
	

	// Generated message map functions
	//{{AFX_MSG(CApiwizDlg)
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
