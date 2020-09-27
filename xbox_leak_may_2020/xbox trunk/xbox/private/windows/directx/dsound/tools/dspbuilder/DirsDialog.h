/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	DirsDialog.h

Abstract:

	Dialog for setting the directories needed by dspbuilder

Author:

	Robert Heitkamp (robheit) 20-Jul-2001

Revision History:

	20-Jul-2001 robheit
		Initial Version

--*/
#if !defined(AFX_DIRSDIALOG_H__5671EA07_280C_4BCF_B7DA_1388F011A4B6__INCLUDED_)
#define AFX_DIRSDIALOG_H__5671EA07_280C_4BCF_B7DA_1388F011A4B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	CDirsDialog
//------------------------------------------------------------------------------
class CDirsDialog : public CDialog
{
public:

	CDirsDialog(CWnd* pParent = NULL);   // standard constructor

	LPCTSTR GetIniPath(void) const { return (LPCTSTR)m_iniPath; };
	LPCTSTR GetDspCodePath(void) const { return (LPCTSTR)m_dspCodePath; };
	void SetIniPath(LPCTSTR pPath) { m_iniPath = pPath; };
	void SetDspCodePath(LPCTSTR pPath) { m_dspCodePath = pPath; };

// Dialog Data
	//{{AFX_DATA(CDirsDialog)
	enum { IDD = IDD_DIRS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirsDialog)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDirsDialog)
	afx_msg void OnIniFilePathBrowse();
	afx_msg void OnDspCodePathBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CString	m_iniPath;
	CString	m_dspCodePath;
	CEdit*	m_pIniEdit;
	CEdit*	m_pDspCodeEdit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRSDIALOG_H__5671EA07_280C_4BCF_B7DA_1388F011A4B6__INCLUDED_)
