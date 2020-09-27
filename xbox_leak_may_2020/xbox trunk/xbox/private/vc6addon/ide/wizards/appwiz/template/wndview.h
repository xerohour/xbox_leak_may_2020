// $$wndview_hfile$$.h : interface of the $$WNDVIEW_CLASS$$ class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// $$WNDVIEW_CLASS$$ window

class $$WNDVIEW_CLASS$$ : public $$WNDVIEW_BASE_CLASS$$
{
// Construction
public:
	$$WNDVIEW_CLASS$$();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL($$WNDVIEW_CLASS$$)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~$$WNDVIEW_CLASS$$();

	// Generated message map functions
protected:
	//{{AFX_MSG($$WNDVIEW_CLASS$$)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
