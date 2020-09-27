#if !defined(AFX_ARTICULATIONCTL_H__BC964EB3_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_ARTICULATIONCTL_H__BC964EB3_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "dlsdesigner.h"
#include "ArticulationEditor.h"

class CArticulation;

// ArticulationCtl.h : Declaration of the CArticulationCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CArticulationCtrl : See ArticulationCtl.cpp for implementation.

class CArticulationCtrl : public COleControl
{
	DECLARE_DYNCREATE(CArticulationCtrl)

// Constructor
public:
	CArticulationCtrl();
	~CArticulationCtrl();

	CArticulation * GetArticulation() {return(m_pArticulation);}

// Attributes
public:
	CArticulation*		m_pArticulation;

private:
	WORD            m_nMIDINoteOns[128];	// J3 do we need this
	HWND				m_hWndContainer;
	HMENU				m_hMenuInPlace;
	CToolBar*			m_pToolBar;
	CArticulationEditor* m_pArticulationEditor; 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArticulationCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:


	DECLARE_OLECREATE_EX(CArticulationCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CArticulationCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CArticulationCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CArticulationCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CArticulationCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CArticulationCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CArticulationCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
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

    // IJazzEditor functions
	BEGIN_INTERFACE_PART(Editor, IJazzEditor)
		STDMETHOD(AttachObjects)(IJazzNode*, IOleUndoManager*);
		STDMETHOD(OnInitMenuFilePrint)(HMENU, UINT);
		STDMETHOD(OnFilePrint)();
		STDMETHOD(OnInitMenuFilePrintPreview)(HMENU, UINT);
		STDMETHOD(OnFilePrintPreview)();
	END_INTERFACE_PART(Editor)

	DECLARE_INTERFACE_MAP()

	enum {
	//{{AFX_DISP_ID(CArticulationCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARTICULATIONCTL_H__BC964EB3_96F7_11D0_89AA_00A0C9054129__INCLUDED)
