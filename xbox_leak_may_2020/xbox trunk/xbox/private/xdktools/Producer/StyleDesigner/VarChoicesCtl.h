#if !defined(VARCHOICESCTL_H__3BD2BA23_46E7_11D0_89AC_00A0C9054129__INCLUDED_)
#define VARCHOICESCTL_H__3BD2BA23_46E7_11D0_89AC_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// VarChoicesCtl.h : Declaration of the CVarChoicesCtrl ActiveX Control class.

#include "VarChoicesDlg.h"
#include "DMusProd.h"

/////////////////////////////////////////////////////////////////////////////
// CVarChoicesCtrl : See VarChoicesCtl.cpp for implementation.

class CVarChoicesCtrl : public COleControl
{
friend class CVarChoices;
friend class CVarChoicesDlg;

	DECLARE_DYNCREATE(CVarChoicesCtrl)

// Constructor
public:
	CVarChoicesCtrl();

// Attributes
public:
	CVarChoicesDlg*			m_pVarChoicesDlg;

private:
	HWND					m_hWndContainer;
	HMENU					m_hMenuInPlace;
	CVarChoices*			m_pVarChoices;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarChoicesCtrl)
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
	~CVarChoicesCtrl();

	DECLARE_OLECREATE_EX(CVarChoicesCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CVarChoicesCtrl)      // GetTypeInfo
	DECLARE_OLECTLTYPE(CVarChoicesCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CVarChoicesCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void AboutBox();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnHelpFinder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CVarChoicesCtrl)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CVarChoicesCtrl)
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
	//{{AFX_DISP_ID(CVarChoicesCtrl)
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(VARCHOICESCTL_H__3BD2BA23_46E7_11D0_89AC_00A0C9054129__INCLUDED_)
