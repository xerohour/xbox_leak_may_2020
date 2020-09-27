#if !defined(AFX_DELETEPROJECTDLG_H__9EA7A743_707F_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_DELETEPROJECTDLG_H__9EA7A743_707F_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif

// _MSC_VER >= 1000
// DeleteProjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteProjectDlg dialog

class CDeleteProjectDlg : public CDialog
{
// Construction
public:
	CDeleteProjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteProjectDlg)
	enum { IDD = IDD_DELETEPROJECT };
	CButton	m_checkRuntimePrompt;
	CButton	m_btnOK;
	CButton	m_radioDesignFiles;
	CButton	m_radioAllFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Member variables
public:
	CProject*	m_pProject;
	short		m_nWhichFiles;
	BOOL		m_fRuntimePrompt;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteProjectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAllFiles();
	afx_msg void OnDesignFiles();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEPROJECTDLG_H__9EA7A743_707F_11D1_89AE_00A0C9054129__INCLUDED_)
