#if !defined(AFX_WHICHPROJECT_H__503E0982_56B4_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_WHICHPROJECT_H__503E0982_56B4_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WhichProjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWhichProjectDlg dialog

class CWhichProjectDlg : public CDialog
{
// Construction
public:
	CWhichProjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWhichProjectDlg)
	enum { IDD = IDD_WHICHPROJECT };
	CStatic	m_staticPrompt;
	CButton	m_btnOK;
	CButton	m_radioUseProject;
	CButton	m_radioNewProject;
	//}}AFX_DATA

public:
	CString m_strFileName;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhichProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWhichProjectDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WHICHPROJECT_H__503E0982_56B4_11D1_89AE_00A0C9054129__INCLUDED_)
