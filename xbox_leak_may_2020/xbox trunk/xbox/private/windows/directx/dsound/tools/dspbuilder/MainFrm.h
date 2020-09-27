/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	MainFrm.h

Abstract:

	Interface of the CMainFrame class

Author:

	Robert Heitkamp (robheit) 16-Jul-2001

Revision History:

	16-Jul-2001 robheit
		Initial Version

--*/
#if !defined(AFX_MAINFRM_H__16B9FE59_8061_4B61_B67C_F25B1395FB45__INCLUDED_)
#define AFX_MAINFRM_H__16B9FE59_8061_4B61_B67C_F25B1395FB45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "ChildView.h"

//------------------------------------------------------------------------------
//	CMainFrame 
//------------------------------------------------------------------------------
class CMainFrame : public CFrameWnd
{
public:

	CMainFrame();
	virtual ~CMainFrame();

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

	void OnUpdateStatusBar(CCmdUI* pCmdUI);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected: 

	DECLARE_DYNAMIC(CMainFrame)

protected:

	CStatusBar	m_wndStatusBar;
	CToolBar	m_wndToolBar;
	CChildView	m_wndView;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnAppExit(void);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__16B9FE59_8061_4B61_B67C_F25B1395FB45__INCLUDED_)
