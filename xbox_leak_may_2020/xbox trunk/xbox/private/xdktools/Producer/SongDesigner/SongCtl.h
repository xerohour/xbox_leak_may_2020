#if !defined(AFX_SONGCTL_H__D135DB66_66ED_11D3_B45D_00105A2796DE__INCLUDED_)
#define AFX_SONGCTL_H__D135DB66_66ED_11D3_B45D_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SongCtl.h : Declaration of the CSongCtrl ActiveX Control class.

#include "SongDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSongCtrl : See SongCtl.cpp for implementation.

class CSongCtrl : public COleControl
{
	DECLARE_DYNCREATE(CSongCtrl)

// Constructor
public:
	CSongCtrl();

// Attributes
public:
	CSongDlg*			m_pSongDlg;

private:
	HWND				m_hWndContainer;
	HMENU				m_hMenuInPlace;
	HACCEL				m_hAcceleratorTable;
	CDirectMusicSong*	m_pSong;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSongCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void OnHideToolBars();
	virtual void OnShowToolBars();
	virtual HMENU OnGetInPlaceMenu();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CSongCtrl();

	DECLARE_OLECREATE_EX(CSongCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CSongCtrl)      // GetTypeInfo
	DECLARE_OLECTLTYPE(CSongCtrl)	   // Type name and misc status

// Message maps
	//{{AFX_MSG(CSongCtrl)
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
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnHelpFinder();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CSongCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CSongCtrl)
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
	//{{AFX_DISP_ID(CSongCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONGCTL_H__D135DB66_66ED_11D3_B45D_00105A2796DE__INCLUDED)
