#if !defined(AFX_GUIDDLG_H__F2B9A029_63AF_11D2_8918_00C04FBF8D15__INCLUDED_)
#define AFX_GUIDDLG_H__F2B9A029_63AF_11D2_8918_00C04FBF8D15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// guiddlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGuidDlg dialog

class CGuidDlg : public CDialog
{
// Construction
public:
	CGuidDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGuidDlg)
	enum { IDD = IDD_DLG_GUID };
	CButton	m_btnOK;
	CEdit	m_editGuid;
	//}}AFX_DATA

	GUID m_guid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGuidDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	GUID m_guidOriginal;

	// Generated message map functions
	//{{AFX_MSG(CGuidDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNewGuid();
	afx_msg void OnKillfocusEditGuid();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnUpdateEditGuid();
	afx_msg void OnGmGuid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUIDDLG_H__F2B9A029_63AF_11D2_8918_00C04FBF8D15__INCLUDED_)
