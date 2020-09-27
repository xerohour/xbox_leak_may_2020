#if !defined(AFX_ADDTRACKDLG_H__BF3F08BC_C8D4_11D1_88B6_00C04FBF8D15__INCLUDED_)
#define AFX_ADDTRACKDLG_H__BF3F08BC_C8D4_11D1_88B6_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"

// AddTrackDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddTrackDlg dialog

class CAddTrackDlg : public CDialog
{
// Construction
public:
	CAddTrackDlg(CWnd* pParent = NULL);   // standard constructor
	~CAddTrackDlg();

// Dialog Data
	//{{AFX_DATA(CAddTrackDlg)
	enum { IDD = IDD_ADD_TRACK };
	CButton	m_btnCancel;
	CButton	m_btnOK;
	CListBox	m_listTrack;
	//}}AFX_DATA

	GUID	*m_pGuid;
	DWORD	m_dwCount;
	CString	m_strSegmentName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddTrackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddTrackDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnDblclkListTrack();
	afx_msg void OnSelChangeListTrack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDTRACKDLG_H__BF3F08BC_C8D4_11D1_88B6_00C04FBF8D15__INCLUDED_)
