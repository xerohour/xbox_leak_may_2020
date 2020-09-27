#if !defined(AFX_CLOSEPROJECTDLG_H__7F1702C3_28E7_11D3_B44B_00105A2796DE__INCLUDED_)
#define AFX_CLOSEPROJECTDLG_H__7F1702C3_28E7_11D3_B44B_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CloseProjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCloseProjectDlg dialog

class CCloseProjectDlg : public CDialog
{
// Construction
public:
	CCloseProjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCloseProjectDlg)
	enum { IDD = IDD_CLOSE_PROJECT };
	CStatic	m_staticPrompt2;
	CStatic	m_staticPrompt1;
	CListBox	m_lstbxFiles;
	//}}AFX_DATA

public:
	CProject*	m_pProject;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCloseProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCloseProjectDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOSEPROJECTDLG_H__7F1702C3_28E7_11D3_B44B_00105A2796DE__INCLUDED_)
