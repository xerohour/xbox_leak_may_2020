// cafemdi.h : header file
//

#ifndef __CAFEMDI_H__
#define __CAFEMDI_H__
#include "portview.h"

/////////////////////////////////////////////////////////////////////////////
// CCafeDrvMDIChild frame

class CCafeDrvMDIChild : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCafeDrvMDIChild)
protected:
	CSplitterWnd m_wndSplitter;
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	CCafeDrvMDIChild();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCafeDrvMDIChild)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCafeDrvMDIChild();

	// Generated message map functions
	//{{AFX_MSG(CCafeDrvMDIChild)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__CAFEMDI_H__ 

/////////////////////////////////////////////////////////////////////////////
