#if !defined(PATTERNCTL_H__3BD2BA28_46E7_11D0_89AC_00A0C9054129__INCLUDED_)
#define PATTERNCTL_H__3BD2BA28_46E7_11D0_89AC_00A0C9054129__INCLUDED_

// PatternCtl.h : Declaration of the CPatternCtrl ActiveX Control class.

#include "PatternDlg.h"


class CDirectMusicPattern;


/////////////////////////////////////////////////////////////////////////////
// CPatternCtrl : See PatternCtl.cpp for implementation.

class CPatternCtrl : public COleControl
{
friend class CPatternDlg;
friend class CDirectMusicStyle;

	DECLARE_DYNCREATE(CPatternCtrl)

// Constructor
public:
	CPatternCtrl();

// Attributes
public:
	CPatternDlg*			m_pPatternDlg;

private:
	HWND					m_hWndContainer;
	HMENU					m_hMenuInPlace;
	HACCEL					m_hAcceleratorTable;
	HANDLE					m_hKeyStatusBar;
	CToolBar*				m_pToolBar;
	CDirectMusicPattern*	m_pPattern;
	BOOL					m_fDocWindowActive;

private:
	BOOL TimelineEditAvailable(WPARAM wId);
	void SendEditToTimeline(WPARAM wId);
	LRESULT OnOpenVarChoicesEditor( WPARAM wParam, LPARAM lParam );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual HMENU OnGetInPlaceMenu();
	virtual void OnHideToolBars();
	virtual void OnShowToolBars();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CPatternCtrl();

	DECLARE_OLECREATE_EX(CPatternCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CPatternCtrl)      // GetTypeInfo
	DECLARE_OLECTLTYPE(CPatternCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CPatternCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void AboutBox();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPatternNewstrip();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditRedo();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnEditPasteInsert();
	afx_msg void OnEditPasteMerge();
	afx_msg void OnEditPasteOverwrite();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectall();
	afx_msg void OnEditInsert();
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnEditMergeVariations();
	afx_msg void OnUpdateEditMergeVariations(CCmdUI* pCmdUI);
	afx_msg void OnHelpFinder();
	afx_msg void OnEditDeletePart();
	afx_msg void OnUpdateEditDeletePart(CCmdUI* pCmdUI);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnEditQuantize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CPatternCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CPatternCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Interface Maps
public:
	// IOleInPlaceActiveObject
	BEGIN_INTERFACE_PART(MyOleInPlaceActiveObject, IOleInPlaceActiveObject)
		INIT_INTERFACE_PART(COleControl, MyOleInPlaceActiveObject)
		STDMETHOD(GetWindow)(HWND*);
		STDMETHOD(ContextSensitiveHelp)(BOOL);
		STDMETHOD(TranslateAccelerator)(LPMSG);
		STDMETHOD(OnFrameWindowActivate)(BOOL);
		STDMETHOD(OnDocWindowActivate)(BOOL);
		STDMETHOD(ResizeBorder)(LPCRECT, LPOLEINPLACEUIWINDOW, BOOL);
		STDMETHOD(EnableModeless)(BOOL);
	END_INTERFACE_PART(MyOleInPlaceActiveObject)

    // IDMUSProdEditor functions
	BEGIN_INTERFACE_PART(Editor, IDMUSProdEditor)
		STDMETHOD(AttachObjects)(IDMUSProdNode*);
		STDMETHOD(OnInitMenuFilePrint)(HMENU, UINT);
		STDMETHOD(OnFilePrint)();
		STDMETHOD(OnInitMenuFilePrintPreview)(HMENU, UINT);
		STDMETHOD(OnFilePrintPreview)();
		STDMETHOD(OnViewProperties)();
		STDMETHOD(OnF1Help)();
	END_INTERFACE_PART(Editor)

	DECLARE_INTERFACE_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CPatternCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(PATTERNCTL_H__3BD2BA28_46E7_11D0_89AC_00A0C9054129__INCLUDED)
