// newgrp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewGroupDlg dialog

class CNewGroupDlg : public C3dDialog
{
// Construction
public:
	CNewGroupDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNewGroupDlg)
	CString m_strGroup;
	CString m_strGroupFilter;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewGroupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeGroup();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
