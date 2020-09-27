// AwusbIODlg.h : header file
//

#if !defined(AFX_AWUSBIODLG_H__7802B6AA_0C7F_11D3_AC25_00104B306BEE__INCLUDED_)
#define AFX_AWUSBIODLG_H__7802B6AA_0C7F_11D3_AC25_00104B306BEE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CAwusbIODlg dialog

class CAwusbIODlg : public CDialog
{
// Construction
public:
	CAwusbIODlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAwusbIODlg)
	enum { IDD = IDD_AWUSBIO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwusbIODlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	WORD	Dir;
	WORD	Data;

	void Update();
	void DoDir(int num);
	void DoValue(int num);
	void Debug (char *fmt, ...);
	void Error (char *fmt, ...);

	// Generated message map functions
	//{{AFX_MSG(CAwusbIODlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnDir0();
	afx_msg void OnDir1();
	afx_msg void OnDir2();
	afx_msg void OnDir3();
	afx_msg void OnDir4();
	afx_msg void OnDir5();
	afx_msg void OnDir6();
	afx_msg void OnDir7();
	afx_msg void OnDir8();
	afx_msg void OnDir9();
	afx_msg void OnDir10();
	afx_msg void OnDir11();
	afx_msg void OnDir12();
	afx_msg void OnDir13();
	afx_msg void OnDir14();
	afx_msg void OnDir15();
	afx_msg void OnValue0();
	afx_msg void OnValue1();
	afx_msg void OnValue2();
	afx_msg void OnValue3();
	afx_msg void OnValue4();
	afx_msg void OnValue5();
	afx_msg void OnValue6();
	afx_msg void OnValue7();
	afx_msg void OnValue8();
	afx_msg void OnValue9();
	afx_msg void OnValue10();
	afx_msg void OnValue11();
	afx_msg void OnValue12();
	afx_msg void OnValue13();
	afx_msg void OnValue14();
	afx_msg void OnValue15();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWUSBIODLG_H__7802B6AA_0C7F_11D3_AC25_00104B306BEE__INCLUDED_)
