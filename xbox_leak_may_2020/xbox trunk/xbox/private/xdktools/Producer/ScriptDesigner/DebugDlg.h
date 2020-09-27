#if !defined(AFX_DEBUGDLG_H__5FD7B909_08C4_49C5_9F4A_CA45857C3D44__INCLUDED_)
#define AFX_DEBUGDLG_H__5FD7B909_08C4_49C5_9F4A_CA45857C3D44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DebugDlg.h : header file
//

class CDebugDlg;

/////////////////////////////////////////////////////////////////////////////
// CDebugListBox window

class CDebugListBox : public CListBox
{
// Construction
public:
	CDebugListBox();
	virtual ~CDebugListBox();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugListBox)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CDebugListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg window

class CDebugDlg : public CWnd
{
// Construction
public:
	CDebugDlg();
	virtual ~CDebugDlg();

// Attributes
protected:
	CDebugListBox	m_lstbxDebugSpew;
	int				m_nNextLineNbr;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugDlg)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetNextLineNbr();

	// Generated message map functions
protected:
	LRESULT OnDisplayMessageText( WPARAM wParam, LPARAM lParam );

	//{{AFX_MSG(CDebugDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGDLG_H__5FD7B909_08C4_49C5_9F4A_CA45857C3D44__INCLUDED_)
