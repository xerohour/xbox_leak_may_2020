#if !defined(AFX_DLGEDITPCHANNEL_H__710E795E_94BF_4A5F_A5FE_E3C870C24674__INCLUDED_)
#define AFX_DLGEDITPCHANNEL_H__710E795E_94BF_4A5F_A5FE_E3C870C24674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditPChannel.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// DlgEditPChannel dialog

class DlgEditPChannel : public CDialog
{
// Construction
public:
	DlgEditPChannel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgEditPChannel)
	enum { IDD = IDD_DIALOG_EDIT_PCHANNEL };
	CStatic	m_staticPChannelHelp;
	CEdit	m_editPChannelname;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgEditPChannel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	DWORD	m_dwPChannel;
	CString	m_strName;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgEditPChannel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITPCHANNEL_H__710E795E_94BF_4A5F_A5FE_E3C870C24674__INCLUDED_)
