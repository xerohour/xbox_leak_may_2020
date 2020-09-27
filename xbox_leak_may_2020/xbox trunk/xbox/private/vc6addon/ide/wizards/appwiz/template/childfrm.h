// $$child_frame_hfile$$.h : interface of the $$CHILD_FRAME_CLASS$$ class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

$$IF(PROJTYPE_EXPLORER)
class $$VIEW_CLASS$$;
$$ENDIF
$$IF(NODOCVIEW)
#include "$$wndview_hfile$$.h"
$$ENDIF

class $$CHILD_FRAME_CLASS$$ : public $$CHILD_FRAME_BASE_CLASS$$
{
	DECLARE_DYNCREATE($$CHILD_FRAME_CLASS$$)
public:
	$$CHILD_FRAME_CLASS$$();

// Attributes
$$IF(SPLITTER_MDI || PROJTYPE_EXPLORER)
protected:
	CSplitterWnd m_wndSplitter;
$$ENDIF //SPLITTER_MDI || EXPLORER
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL($$CHILD_FRAME_CLASS$$)
$$IF(SPLITTER_MDI || CHILD_FRAME_STYLES || PROJTYPE_EXPLORER || NODOCVIEW)
	public:
$$ENDIF
$$IF(SPLITTER_MDI || PROJTYPE_EXPLORER )
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
$$ENDIF //SPLITTER_MDI || PROJTYPE_EXPLORER
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
$$IF(CHILD_FRAME_MAXIMIZED || CHILD_FRAME_MINIMIZED)
	virtual void ActivateFrame(int nCmdShow);
$$ENDIF
$$IF(NODOCVIEW)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
$$ENDIF
	//}}AFX_VIRTUAL

// Implementation
public:
$$IF(NODOCVIEW)
	// view for the client area of the frame.
	$$WNDVIEW_CLASS$$ m_wndView;
$$ENDIF
	virtual ~$$CHILD_FRAME_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
protected:
	$$VIEW_CLASS$$* GetRightPane();
$$ENDIF	// CListView
$$ENDIF // PROJTYPE_EXPLORER
// Generated message map functions
protected:
$$IF( REBAR )
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	CToolBar m_wndToolBar;
$$ENDIF // (ANY SERVER)
$$ENDIF // REBAR
	//{{AFX_MSG($$CHILD_FRAME_CLASS$$)
$$IF(VERBOSE)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
$$ENDIF
$$IF(NODOCVIEW)
	afx_msg void OnFileClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
$$ENDIF
	//}}AFX_MSG
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
$$ENDIF
$$ENDIF
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
