#if !defined(BANDCTL_H__3BD2BA32_46E7_11D0_89AC_00A0C9054129__INCLUDED_)
#define BANDCTL_H__3BD2BA32_46E7_11D0_89AC_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// BandCtl.h : Declaration of the CBandCtrl ActiveX Control class.

class CBandDlg;

/////////////////////////////////////////////////////////////////////////////
// CBandCtrl : See BandCtl.cpp for implementation.

class CBandCtrl : public COleControl
{
friend class CBandDlg;

	DECLARE_DYNCREATE(CBandCtrl)

// Constructor
public:
	CBandCtrl();

// Attributes
public:
	CBandDlg*	m_pBandDlg;
	bool		m_fFrameActive;
	bool		m_fDocWindowActive;

private:
	HWND				m_hWndContainer;
	HMENU				m_hMenuInPlace;
	HANDLE				m_hKeyStatusBar;
	CToolBar*			m_pToolBar;
	CBand*				m_pBand;
	CBandComponent*		m_pComponent;
	HWND				m_hLastFocusWnd;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBandCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual HMENU OnGetInPlaceMenu();
	virtual void OnHideToolBars();
	virtual void OnShowToolBars();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CBandCtrl();

	DECLARE_OLECREATE_EX(CBandCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CBandCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CBandCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CBandCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CBandCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void AboutBox();
	afx_msg void OnEditProperties();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnHelpFinder();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CBandCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CBandCtrl)
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

	// IOleInPlaceObject
	BEGIN_INTERFACE_PART(MyOleInPlaceObject, IOleInPlaceObject)
		INIT_INTERFACE_PART(COleControl, MyOleInPlaceObject)
		STDMETHOD(GetWindow)(HWND*);
		STDMETHOD(ContextSensitiveHelp)(BOOL);
		STDMETHOD(InPlaceDeactivate)();
		STDMETHOD(UIDeactivate)();
		STDMETHOD(SetObjectRects)(LPCRECT, LPCRECT);
		STDMETHOD(ReactivateAndUndo)();
	END_INTERFACE_PART(MyOleInPlaceObject)

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
	//{{AFX_DISP_ID(CBandCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(BANDCTL_H__3BD2BA32_46E7_11D0_89AC_00A0C9054129__INCLUDED)
