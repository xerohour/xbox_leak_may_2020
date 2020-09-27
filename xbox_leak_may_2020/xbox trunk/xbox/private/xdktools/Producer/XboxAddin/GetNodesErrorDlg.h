#if !defined(AFX_GETNODESERRORDLG_H__80154D9B_4DD7_4CFE_9BE0_75C6FB18F2E4__INCLUDED_)
#define AFX_GETNODESERRORDLG_H__80154D9B_4DD7_4CFE_9BE0_75C6FB18F2E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetNodesErrorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetNodesErrorDlg dialog

class CGetNodesErrorDlg : public CDialog
{
// Construction
public:
	CGetNodesErrorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetNodesErrorDlg)
	enum { IDD = IDD_GETNODES_ERROR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_strErrorText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetNodesErrorDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetNodesErrorDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETNODESERRORDLG_H__80154D9B_4DD7_4CFE_9BE0_75C6FB18F2E4__INCLUDED_)
