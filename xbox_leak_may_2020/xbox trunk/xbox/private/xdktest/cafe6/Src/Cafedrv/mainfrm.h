///////////////////////////////////////////////////////////////////////////////
//	MAINFRM.H
//
//	Created by :			Date :
//		DavidGa					8/13/93
//
//	Description :
//		Declaration of the CMainFrame class
//

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

#ifndef __AFXWIN_H__
	#error include 'cafe.h' before including this file for PCH
#endif

#include "settings.h"
#include "suitebar.h"
#include "cafebar.h"
#include "vwprtbar.h"

///////////////////////////////////////////////////////////////////////////////
// CMainFrame class

class CMainFrame : public CMDIFrameWnd
{
public:
	CMainFrame();

	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	CSettings* GetSettings(void)			{ ASSERT(m_psettingsWnd); return m_psettingsWnd; }

private:
	BOOL CreateStatusBar(void);
	BOOL InitializeSettings(void);
	BOOL UpdateSettings(void);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif

// data
protected:
	CCAFEBar* m_ptbCAFEBar;
	CSubSuiteBar* m_ptbSubSuiteBar;
	CViewportBar* m_ptbViewportBar;
	CStatusBar m_wndStatusBar;
	CSettings* m_psettingsWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnOptionsToolbars();
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnToolset(void);
	afx_msg void OnBuildType(void);
	afx_msg void OnLanguage(void);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif //__MAINFRM_H__
