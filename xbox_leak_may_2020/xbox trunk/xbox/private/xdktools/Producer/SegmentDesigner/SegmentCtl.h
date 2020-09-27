#if !defined(AFX_SEGMENTCTL_H__8F8F5C74_A6AB_11D0_8C10_00A0C92E1CAC__INCLUDED_)
#define AFX_SEGMENTCTL_H__8F8F5C74_A6AB_11D0_8C10_00A0C92E1CAC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifndef __SEGMENTCTL_H__
#define __SEGMENTCTL_H__
#include "SegmentDlg.h"
#include "Segment.h"

class CSegmentDlg;

// SegmentCtl.h : Declaration of the CSegmentCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CSegmentCtrl : See SegmentCtl.cpp for implementation.

class CSegmentCtrl : public COleControl
{
friend class CSegmentDlg;

	DECLARE_DYNCREATE(CSegmentCtrl)

// Constructor
public:
	CSegmentCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentCtrl)
	public:
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnShowToolBars();
	virtual void OnHideToolBars();
	virtual HMENU OnGetInPlaceMenu();
	//}}AFX_VIRTUAL

public:
	CSegmentDlg	*m_pSegmentDlg;

private:
	HWND			m_hWndContainer;
	HMENU			m_hMenuInPlace;
	HACCEL			m_hAcceleratorTable;
	HANDLE			m_hKeyStatusBar;
	CSegment		*m_pSegment;
	BOOL			m_fDocWindowActive;

// Implementation
protected:
	~CSegmentCtrl();

	DECLARE_OLECREATE_EX(CSegmentCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CSegmentCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CSegmentCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CSegmentCtrl)		// Type name and misc status

// Message maps

	afx_msg void OnWndActivate(WPARAM, LPARAM);

	//{{AFX_MSG(CSegmentCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnAppAbout();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditRedo();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteMerge();
	afx_msg void OnEditPasteOverwrite();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectall();
	afx_msg void OnEditInsert();
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSaveAsSection();
	afx_msg void OnUpdateSaveAsSection(CCmdUI* pCmdUI);
	afx_msg void OnSegmentDeleteTrack();
	afx_msg void OnUpdateSegmentDeleteTrack(CCmdUI* pCmdUI);
	afx_msg void OnSegmentNewtrack();
	afx_msg void OnHelpFinder();
	afx_msg void OnUpdateEditPasteMerge(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPasteOverwrite(CCmdUI* pCmdUI);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Additional Functions
protected:
	void SendEditToTimeline(WPARAM wId);
	BOOL TimelineEditAvailable(WPARAM wId);

// Dispatch maps
	//{{AFX_DISPATCH(CSegmentCtrl)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CSegmentCtrl)
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
	//{{AFX_DISP_ID(CSegmentCtrl)
	//}}AFX_DISP_ID
	};
};

#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTCTL_H__8F8F5C74_A6AB_11D0_8C10_00A0C92E1CAC__INCLUDED)
