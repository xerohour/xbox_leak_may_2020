#if !defined(AFX_INSTRUMENTCTL_H__BC964EA9_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_INSTRUMENTCTL_H__BC964EA9_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <DMUSProd.h>
class CInstrumentFVEditor;
class CInstrument;

// InstrumentCtl.h : Declaration of the CInstrumentCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl : See InstrumentCtl.cpp for implementation.

class CInstrumentCtrl : public COleControl
{
friend class CInstrumentFVEditor;
friend class CArticulation;
friend class CRegion;
friend class CInstrumentRegions;
friend class CDLSEdit;
	DECLARE_DYNCREATE(CInstrumentCtrl)

// Constructor
public:
	CInstrumentCtrl();

// Attributes
public:
	CInstrument*	GetInstrument() {return(m_pInstrument);}
	void			TurnOffMidiNotes();
	void			SetTransportName();

private:
	HACCEL					m_hAcceleratorTable;
	int						m_nMIDINoteOns[128];
	static int				m_nInstrumentCtrlRefCount;
	static HMENU			m_hMenuInPlace;
	//CToolBar*				m_pToolBar;
	CInstrument*			m_pInstrument;
	IDMUSProdNode*			m_pAttachedNode;
protected:
	int						GetSpacebarNote();
	int						m_nLastSpacebarNote;


public:
	CInstrumentFVEditor*	m_pInstrumentEditor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentCtrl)
	public:
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual HMENU OnGetInPlaceMenu();
	virtual void OnShowToolBars();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CInstrumentCtrl();

	DECLARE_OLECREATE_EX(CInstrumentCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CInstrumentCtrl)      // GetTypeInfo
	//DECLARE_PROPPAGEIDS(CInstrumentCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CInstrumentCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CInstrumentCtrl)
	afx_msg void AboutBox();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChildActivate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHelpFinder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CInstrumentCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()


// Event maps
	//{{AFX_EVENT(CInstrumentCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Interface Maps
public:
	// IOleInPlaceActiveObject
	BEGIN_INTERFACE_PART(MyOleInPlaceActiveObject, IOleInPlaceActiveObject)
		INIT_INTERFACE_PART(CInstrumentCtl, MyOleInPlaceActiveObject)
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
		STDMETHOD(OnInitMenuFilePrintPreview)(HMENU, UINT);
		STDMETHOD(OnFilePrint)();
		STDMETHOD(OnFilePrintPreview)();
		STDMETHOD(OnViewProperties)();
		STDMETHOD(OnF1Help)();
	END_INTERFACE_PART(Editor)

	DECLARE_INTERFACE_MAP()

	enum {
	//{{AFX_DISP_ID(CInstrumentCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUMENTCTL_H__BC964EA9_96F7_11D0_89AA_00A0C9054129__INCLUDED)
