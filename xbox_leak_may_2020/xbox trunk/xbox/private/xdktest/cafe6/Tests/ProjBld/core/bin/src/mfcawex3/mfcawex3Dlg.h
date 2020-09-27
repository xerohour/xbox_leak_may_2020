// mfcawex3Dlg.h : header file
//

#if !defined(AFX_MFCAWEX3DLG_H__3EDC9527_885B_11D1_87EF_00AA003451E1__INCLUDED_)
#define AFX_MFCAWEX3DLG_H__3EDC9527_885B_11D1_87EF_00AA003451E1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMfcawex3Dlg dialog

class CMfcawex3Dlg : public CDialog
{
// Construction
public:
	CMfcawex3Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMfcawex3Dlg)
	enum { IDD = IDD_MFCAWEX3_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawex3Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMfcawex3Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEX3DLG_H__3EDC9527_885B_11D1_87EF_00AA003451E1__INCLUDED_)
