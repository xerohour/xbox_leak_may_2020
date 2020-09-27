// HackAttackDlg.h : header file
//

#if !defined(AFX_HACKATTACKDLG_H__920B2895_8B2D_4520_BA76_956AF5AC17E0__INCLUDED_)
#define AFX_HACKATTACKDLG_H__920B2895_8B2D_4520_BA76_956AF5AC17E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




/////////////////////////////////////////////////////////////////////////////
// CHackAttackDlg dialog

class CHackAttackDlg : public CDialog
{
friend DWORD WINAPI SpartaThread(LPVOID lpParameter);

// Construction
public:
	CHackAttackDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHackAttackDlg)
	enum { IDD = IDD_HACKATTACK_DIALOG };
	CButton	m_checkESP;
	CButton	m_checkUDP;
	CButton	m_checkTCP;
	CButton	m_checkIP;
	CButton	m_buttonStart;
	BOOL	m_fTCP;
	BOOL	m_fIP;
	BOOL	m_fUDP;
	CString	m_stringResults;
	CString	m_stringMacAddress;
	BOOL	m_fESP;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHackAttackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	TCHAR m_szMacAddress[13];
	BOOL m_fAttackRunning;
	CInterface * m_pInterface;
	CInterfaceList * m_pInterfaceList;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHackAttackDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HACKATTACKDLG_H__920B2895_8B2D_4520_BA76_956AF5AC17E0__INCLUDED_)
