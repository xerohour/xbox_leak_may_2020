// $$ipframe_hfile$$.h : interface of the $$IPFRAME_CLASS$$ class
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class $$IPFRAME_CLASS$$ : public $$IPFRAME_BASE_CLASS$$
{
	DECLARE_DYNCREATE($$IPFRAME_CLASS$$)
public:
	$$IPFRAME_CLASS$$();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL($$IPFRAME_CLASS$$)
$$IF(TOOLBAR)
	public:
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
$$ENDIF
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~$$IPFRAME_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
$$IF(TOOLBAR)
	CToolBar    m_wndToolBar;
$$IF(REBAR)
	CDialogBar m_wndDlgBar;
	CReBar m_wndReBar;
$$ENDIF
$$ENDIF
	COleDropTarget	m_dropTarget;
	COleResizeBar   m_wndResizeBar;

// Generated message map functions
protected:
	//{{AFX_MSG($$IPFRAME_CLASS$$)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
$$IF(VERBOSE)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
$$ENDIF
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
