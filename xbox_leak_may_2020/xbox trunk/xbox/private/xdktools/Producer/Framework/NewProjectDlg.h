#if !defined(AFX_NEWPROJECT_H__DCE49D26_CAAC_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_NEWPROJECT_H__DCE49D26_CAAC_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NewProjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewProjectDlg dialog

class CNewProjectDlg : public CDialog
{
// Construction
public:
	CNewProjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
public:
	CString m_strName;
	CString m_strProjectDir;
	CString m_strTitle;		// Title of dialog

	//{{AFX_DATA(CNewProjectDlg)
	enum { IDD = IDD_NEW_PROJECT };
	CButton	m_btnOK;
	CEdit	m_editProjectDir;
	CEdit	m_editProjectName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL IsValidProjectName();
	BOOL IsValidProjectDir();
	void GetProjectName( CString& strName );
	void GetProjectDir( CString& strProjectDir );

	// Generated message map functions
	//{{AFX_MSG(CNewProjectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnProjectDirBrowse();
	afx_msg void OnChangeProjectName();
	virtual void OnOK();
	afx_msg void OnChangeProjectDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPROJECT_H__DCE49D26_CAAC_11D0_89AE_00A0C9054129__INCLUDED_)
