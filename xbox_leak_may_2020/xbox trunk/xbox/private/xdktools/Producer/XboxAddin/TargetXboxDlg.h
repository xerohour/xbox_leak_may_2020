#if !defined(AFX_TARGETXBOXDLG_H__65DE46FD_90D2_4BF8_B7C8_70E989AE1A7C__INCLUDED_)
#define AFX_TARGETXBOXDLG_H__65DE46FD_90D2_4BF8_B7C8_70E989AE1A7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TargetXboxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargetXboxDlg dialog

class CTargetXboxDlg : public CDialog
{
// Construction
public:
	CTargetXboxDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTargetXboxDlg)
	enum { IDD = IDD_TARGET_XBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTargetXboxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTargetXboxDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGETXBOXDLG_H__65DE46FD_90D2_4BF8_B7C8_70E989AE1A7C__INCLUDED_)
