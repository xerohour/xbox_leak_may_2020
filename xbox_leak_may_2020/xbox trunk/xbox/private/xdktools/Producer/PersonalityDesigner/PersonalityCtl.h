#if !defined(AFX_PERSONALITYCTL_H__D433F96C_B588_11D0_9EDC_00AA00A21BA9__INCLUDED_)
#define AFX_PERSONALITYCTL_H__D433F96C_B588_11D0_9EDC_00AA00A21BA9__INCLUDED_

#ifndef	__PERSONALITYCTL_H__
#define	__PERSONALITYCTL_H__
//#include "Personality.h"
#include "Splitter.h"
#include "ChordDialog.h"

#include "SignPostDialog.h"
class CSignPostDialog;

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PersonalityCtl.h : Declaration of the CPersonalityCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CPersonalityCtrl : See PersonalityCtl.cpp for implementation.

class CPersonalityCtrl : public COleControl
{
	friend CSignPostDialog;	// Declare CSignPostDialog as friend so static member (m_SignPostDialog) can resolve private IUnknown methods.

	DECLARE_DYNCREATE(CPersonalityCtrl)

private:
	CSplitter	m_wndSplitter;
	HWND		m_hWndContainer;
	HMENU		m_hMenuInPlace;
	CToolBar*	m_pToolBar;
	HACCEL					m_hAcceleratorTable;

	int			m_OnSizeCount;	// incremented first two times goes through OnSize, determines when
								// real size of window is determined. TODO: replace this with something more robust

// Constructor
public:
	CPersonalityCtrl();
	enum {DefaultSplitterWidth=6, MinSplitterXPos = 300, ChordMap, SignPost };
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPersonalityCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual HMENU OnGetInPlaceMenu();
	virtual void OnHideToolBars();
	virtual void OnShowToolBars();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

public:
//	CPersonalityDlg *m_pPersonalityDlg;
//	PersonalityFrameWnd *m_pFrameWindow;
	void EndTrack( int nWidth );

	void SetLastFocus(int obj)
	{
		if(obj == ChordMap)
		{
			m_pdlgLastFocus = &m_ChordDialog;
		}
		else if(obj == SignPost)
		{
			m_pdlgLastFocus = &m_SignPostDialog;
			m_ChordDialog.ClearSelections();
		}
		else
		{
			m_pdlgLastFocus = 0;
		}
	}

private:
	CPersonality	*m_pPersonality;
	CSignPostDialog	m_SignPostDialog;
	CChordDialog	m_ChordDialog;
	CDialog*		m_pdlgLastFocus;

// Implementation
protected:
	~CPersonalityCtrl();

	BOOL EditAvailable(WPARAM wId);
	void SendEdit(WPARAM wId);
	void SendEditToTimeline(WPARAM wId);
	BOOL TimelineEditAvailable(WPARAM wId);
	void SendEditToSignPost(WPARAM wId);
	BOOL SignPostEditAvailable(WPARAM wId);

	DECLARE_OLECREATE_EX(CPersonalityCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CPersonalityCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPersonalityCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CPersonalityCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CPersonalityCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void AboutBox();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnCut();
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnInsert();
	afx_msg void OnUpdateInsert(CCmdUI* pCmdUI);
	afx_msg void OnSelectall();
	afx_msg void OnUpdateSelectall(CCmdUI* pCmdUI);
	afx_msg void OnProperties();
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDiatonic();
	afx_msg void OnUpdateDiatonic(CCmdUI* pCmdUI);
	afx_msg void OnDom7ths();
	afx_msg void OnUpdateDom7ths(CCmdUI* pCmdUI);
	afx_msg void OnMajor7ths();
	afx_msg void OnUpdateMajor7ths(CCmdUI* pCmdUI);
	afx_msg void OnMinor7ths();
	afx_msg void OnUpdateMinor7ths(CCmdUI* pCmdUI);
	afx_msg void OnAllmajor();
	afx_msg void OnUpdateAllmajor(CCmdUI* pCmdUI);
	afx_msg void OnAllminor();
	afx_msg void OnUpdateAllminor(CCmdUI* pCmdUI);
	afx_msg void OnDiatonictriad();
	afx_msg void OnUpdateDiatonictriad(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnEditCut();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUndo();
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnRedo();
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpFinder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CPersonalityCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CPersonalityCtrl)
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
	//{{AFX_DISP_ID(CPersonalityCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};



#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERSONALITYCTL_H__D433F96C_B588_11D0_9EDC_00AA00A21BA9__INCLUDED)
