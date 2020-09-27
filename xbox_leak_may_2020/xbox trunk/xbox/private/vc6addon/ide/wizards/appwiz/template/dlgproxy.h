// $$dlgautoproxy_hfile$$.h : header file
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class $$DLG_CLASS$$;

/////////////////////////////////////////////////////////////////////////////
// $$DLGAUTOPROXY_CLASS$$ command target

class $$DLGAUTOPROXY_CLASS$$ : public $$DLGAUTOPROXY_BASE_CLASS$$
{
	DECLARE_DYNCREATE($$DLGAUTOPROXY_CLASS$$)

	$$DLGAUTOPROXY_CLASS$$();           // protected constructor used by dynamic creation

// Attributes
public:
	$$DLG_CLASS$$* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL($$DLGAUTOPROXY_CLASS$$)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~$$DLGAUTOPROXY_CLASS$$();

	// Generated message map functions
	//{{AFX_MSG($$DLGAUTOPROXY_CLASS$$)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE($$DLGAUTOPROXY_CLASS$$)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH($$DLGAUTOPROXY_CLASS$$)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
