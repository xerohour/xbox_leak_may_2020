#if !defined(AFX_CONTEXTMENUHANDLER_H__B49AF6C3_EE6A_11D0_BAD6_00805F493F43__INCLUDED_)
#define AFX_CONTEXTMENUHANDLER_H__B49AF6C3_EE6A_11D0_BAD6_00805F493F43__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ContextMenuHandler.h : header file
//

#include "timeline.h"

/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler window

class CContextMenuHandler : public CWnd
{
// Construction
public:
	CContextMenuHandler();

// Attributes
public:

	IDMUSProdStrip *m_pStrip;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextMenuHandler)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CContextMenuHandler();

	// Generated message map functions
protected:
	//{{AFX_MSG(CContextMenuHandler)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTMENUHANDLER_H__B49AF6C3_EE6A_11D0_BAD6_00805F493F43__INCLUDED_)
