// $$frame_hfile$$.h : interface of the $$FRAME_CLASS$$ class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
$$IF(PROJTYPE_SDI)
$$IF(PROJTYPE_EXPLORER)

class $$VIEW_CLASS$$;
$$ELIF(NODOCVIEW)

#include "$$wndview_hfile$$.h"
$$ENDIF
$$ENDIF 

$$IF(PROJTYPE_MDI)
class $$FRAME_CLASS$$ : public $$FRAME_BASE_CLASS$$
{
	DECLARE_DYNAMIC($$FRAME_CLASS$$)
public:
	$$FRAME_CLASS$$();
$$ELSE // SDI
class $$FRAME_CLASS$$ : public $$FRAME_BASE_CLASS$$
{
	
$$IF(NODOCVIEW)
public:
	$$FRAME_CLASS$$();
protected: 
	DECLARE_DYNAMIC($$FRAME_CLASS$$)
$$ELSE
protected: // create from serialization only
	$$FRAME_CLASS$$();
	DECLARE_DYNCREATE($$FRAME_CLASS$$)
$$ENDIF
$$ENDIF // PROJTYPEMDI

// Attributes
$$IF(SPLITTER_SDI || PROJTYPE_EXPLORER)
$$IF(PROJTYPE_SDI)
protected:
	CSplitterWnd m_wndSplitter;
$$ENDIF 
$$ENDIF //SPLITTER_SDI
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL($$FRAME_CLASS$$)
$$IF(SPLITTER_SDI || FRAME_STYLES || PROJTYPE_EXPLORER)
	public:
$$ENDIF
$$IF(SPLITTER_SDI || PROJTYPE_EXPLORER)
$$IF(PROJTYPE_SDI)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
$$ENDIF 
$$ENDIF
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
$$IF(NODOCVIEW)
$$IF(PROJTYPE_SDI)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
$$ENDIF 
$$ENDIF
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~$$FRAME_CLASS$$();
$$IF(PROJTYPE_SDI)
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	$$VIEW_CLASS$$* GetRightPane();
$$ENDIF	// CListView
$$ENDIF	// PROJTYPE_EXPLORER
$$ENDIF	// PROJTYPE_SDI
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
$$IF(TOOLBAR || STATUSBAR || REBAR)

protected:  // control bar embedded members
$$IF(STATUSBAR)	
	CStatusBar  m_wndStatusBar;
$$ENDIF //STATUSBAR
$$IF(TOOLBAR)
	CToolBar    m_wndToolBar;
$$ENDIF //TOOLBAR
$$IF(REBAR)
	CReBar      m_wndReBar;
	CDialogBar      m_wndDlgBar;
$$ENDIF //REBAR
$$ENDIF //TOOLBAR || STATUSBAR || REBAR
$$IF(NODOCVIEW)
$$IF(PROJTYPE_SDI)
	$$WNDVIEW_CLASS$$    m_wndView;
$$ENDIF 
$$ENDIF

// Generated message map functions
protected:
	//{{AFX_MSG($$FRAME_CLASS$$)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
$$IF(NODOCVIEW)
$$IF(PROJTYPE_SDI)
	afx_msg void OnSetFocus(CWnd *pOldWnd);
$$ENDIF 
$$ENDIF
$$IF(VERBOSE)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
$$ENDIF
	//}}AFX_MSG
$$IF(PROJTYPE_EXPLORER)
$$IF(PROJTYPE_SDI)
$$IF(CListView)
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
$$ENDIF
$$ENDIF
$$ENDIF
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
