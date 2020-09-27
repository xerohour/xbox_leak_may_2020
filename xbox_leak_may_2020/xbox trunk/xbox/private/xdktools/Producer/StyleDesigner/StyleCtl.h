#if !defined(STYLECTL_H__3BD2BA23_46E7_11D0_89AC_00A0C9054129__INCLUDED_)
#define STYLECTL_H__3BD2BA23_46E7_11D0_89AC_00A0C9054129__INCLUDED_

// StyleCtl.h : Declaration of the CStyleCtrl ActiveX Control class.

#include "StyleDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CStyleCtrl : See StyleCtl.cpp for implementation.

class CStyleCtrl : public COleControl
{
	DECLARE_DYNCREATE(CStyleCtrl)

// Constructor
public:
	CStyleCtrl();

// Attributes
public:
	CStyleDlg*			m_pStyleDlg;

private:
	HWND				m_hWndContainer;
	HMENU				m_hMenuInPlace;
	CToolBar*			m_pToolBar;
	CDirectMusicStyle*	m_pStyle;

private:
	LRESULT OnSyncStyleEditor( WPARAM wParam, LPARAM lParam );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual HMENU OnGetInPlaceMenu();
	virtual void OnHideToolBars();
	virtual void OnShowToolBars();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CStyleCtrl();

	DECLARE_OLECREATE_EX(CStyleCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CStyleCtrl)      // GetTypeInfo
	DECLARE_OLECTLTYPE(CStyleCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CStyleCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void AboutBox();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnEditInsert();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnHelpFinder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CStyleCtrl)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CStyleCtrl)
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
	//{{AFX_DISP_ID(CStyleCtrl)
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(STYLECTL_H__3BD2BA23_46E7_11D0_89AC_00A0C9054129__INCLUDED)
