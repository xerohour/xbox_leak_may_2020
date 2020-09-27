#if !defined(AFX_DLGSENDPROPERTIES_H__6754EBBC_BC07_4879_BCBF_E9998362F885__INCLUDED_)
#define AFX_DLGSENDPROPERTIES_H__6754EBBC_BC07_4879_BCBF_E9998362F885__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSendProperties.h : header file
//

#include "resource.h"
#include "Dlg3DParam.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSendProperties dialog

class CDlgSendProperties : public CDialog
{
// Construction
public:
	CDlgSendProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSendProperties)
	enum { IDD = IDD_DLG_SEND_PROPERTIES };
	C3DEdit	m_editVolume;
	CTabCtrl	m_tab;
	//}}AFX_DATA

	long	m_lVolume;
	CString m_strTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSendProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSendProperties)
	afx_msg void OnKillfocusEditVolume();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSENDPROPERTIES_H__6754EBBC_BC07_4879_BCBF_E9998362F885__INCLUDED_)
