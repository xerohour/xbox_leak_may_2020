#if !defined(AFX_TARGETDIRECTORYDLG_H__37BC6E2C_A711_4D78_BD08_2E7B086AE2EB__INCLUDED_)
#define AFX_TARGETDIRECTORYDLG_H__37BC6E2C_A711_4D78_BD08_2E7B086AE2EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TargetDirectoryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargetDirectoryDlg dialog

class CTargetDirectoryDlg : public CDialog
{
// Construction
public:
	CTargetDirectoryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTargetDirectoryDlg)
	enum { IDD = IDD_TARGET_DIRECTORY };
	CButton	m_btnSynchronize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTargetDirectoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTargetDirectoryDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSynchronize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGETDIRECTORYDLG_H__37BC6E2C_A711_4D78_BD08_2E7B086AE2EB__INCLUDED_)
