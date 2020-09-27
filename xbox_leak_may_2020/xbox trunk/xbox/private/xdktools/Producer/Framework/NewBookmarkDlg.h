#if !defined(AFX_NEWBOOKMARKDLG_H__3A09B082_F393_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_NEWBOOKMARKDLG_H__3A09B082_F393_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NewBookmarkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewBookmarkDlg dialog

class CNewBookmarkDlg : public CDialog
{
// Construction
public:
	CNewBookmarkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewBookmarkDlg)
	enum { IDD = IDD_NEW_BOOKMARK };
	CButton	m_btnOK;
	CEdit	m_editBookmarkName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewBookmarkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Memver variables
public:
	CString m_strName;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewBookmarkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeBookmarkName();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWBOOKMARKDLG_H__3A09B082_F393_11D0_89AE_00A0C9054129__INCLUDED_)
