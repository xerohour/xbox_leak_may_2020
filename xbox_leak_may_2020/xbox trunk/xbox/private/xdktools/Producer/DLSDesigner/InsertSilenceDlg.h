#if !defined(AFX_INSERTSILENCEDLG_H__C2999478_1FBB_4E79_8624_B3B7A390A499__INCLUDED_)
#define AFX_INSERTSILENCEDLG_H__C2999478_1FBB_4E79_8624_B3B7A390A499__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InsertSilenceDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CInsertSilenceDlg dialog

class CInsertSilenceDlg : public CDialog
{
// Construction
public:
	CInsertSilenceDlg(DWORD dwSampleRate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInsertSilenceDlg)
	enum { IDD = IDD_INSERT_SILENCE };
	CEdit	m_editSeconds;
	CEdit	m_editSamples;
	CEdit	m_editMilliseconds;
	//}}AFX_DATA

	DWORD GetSampleLength() { return m_dwSampleLength; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertSilenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInsertSilenceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEditSamples();
	afx_msg void OnKillfocusEditMilliseconds();
	afx_msg void OnKillfocusEditSeconds();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DWORD	m_dwSampleRate;
	DWORD	m_dwSampleLength;
	DWORD	m_dwSecondLength;
	DWORD	m_dwMillisecondLength;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTSILENCEDLG_H__C2999478_1FBB_4E79_8624_B3B7A390A499__INCLUDED_)
