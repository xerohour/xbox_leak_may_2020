#if !defined(AFX_SONGDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_)
#define AFX_SONGDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SongDlg.h : header file
//

#include "VirtualSegmentPropPageObject.h"
#include "TrackPropPageObject.h"
#include "TransitionPropPageObject.h"


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox window

class CSegmentListBox : public CListBox
{
// Construction
public:
	CSegmentListBox();
	virtual ~CSegmentListBox();

// Attributes
public:
	CSongDlg*	m_pSongDlg;
	POINT		m_ptLastXYPos;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectVirtualSegment( CVirtualSegment* pVirtualSegment );
	void SelectVirtualSegmentList( CTypedPtrList<CPtrList, CVirtualSegment*>& list );
	HRESULT CreateDataObject( IDataObject** ppIDataObject );

	// Generated message map functions
protected:
	//{{AFX_MSG(CSegmentListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDblClk();
	afx_msg int VKeyToItem(UINT nKey, UINT nIndex);
	afx_msg void OnSelChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CTrackListBox window

class CTrackListBox : public CListBox
{
// Construction
public:
	CTrackListBox();
	virtual ~CTrackListBox();

// Attributes
public:
	CSongDlg*	m_pSongDlg;
	POINT		m_ptLastXYPos;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrackListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectTrack( CTrack* pTrack, bool fSetFocus );
	void SelectTrackList( CTypedPtrList<CPtrList, CTrack*>& list );
	HRESULT CreateDataObject( IDataObject** ppIDataObject );

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrackListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDblClk();
	afx_msg int VKeyToItem(UINT nKey, UINT nIndex);
	afx_msg void OnSelChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CTransitionListBox window

class CTransitionListBox : public CListBox
{
// Construction
public:
	CTransitionListBox();
	virtual ~CTransitionListBox();

// Attributes
public:
	CSongDlg*	m_pSongDlg;
	POINT		m_ptLastXYPos;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransitionListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectTransition( CTransition* pTransition, bool fSetFocus );
	void SelectTransitionList( CTypedPtrList<CPtrList, CTransition*>& list );
	HRESULT CreateDataObject( IDataObject** ppIDataObject );

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransitionListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDblClk();
	afx_msg int VKeyToItem(UINT nKey, UINT nIndex);
	afx_msg void OnSelChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSongDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSongDlg : public CFormView, public IDropSource, public IDropTarget
{
friend class CSegmentListBox;
friend class CTrackListBox;
friend class CTransitionListBox;

public:
	CSongDlg();           // protected constructor used by dynamic creation
	virtual ~CSongDlg();

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// IDropSource methods
	HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );

	// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave();
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	// Additional methods
protected:
	HRESULT CanPasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt, bool* fMoveIsOK );
	HRESULT PasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	HRESULT CF_SEGMENT_PasteVirtualSegment( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	HRESULT CF_VIRTUAL_SEGMENT_LIST_PasteVirtualSegment( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	HRESULT CF_TRANSITION_LIST_PasteTransition( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	HRESULT CF_TRACK_LIST_PasteTrack( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	
	void FillSourceSegmentComboBox();
	void FillVirtualSegmentComboBox();
	void FillToolGraphComboBox();
	void FillVirtualSegmentListBox();
	void FillTrackListBox( CVirtualSegment* pVirtualSegment );
	void FillTransitionListBox( CVirtualSegment* pVirtualSegment );

	void SelectSourceSegmentComboBox( CSourceSegment* pSourceSegment );
	void SelectVirtualSegmentComboBox( CVirtualSegment* pVirtualSegment );
	void SelectToolGraphComboBox( IDMUSProdNode* pIToolGraphNode );

	void RefreshVirtualSegmentControls();
	void EnableVirtualSegmentControls( BOOL fEnable );
	void OnInsertVirtualSegment();
	void OnDeleteVirtualSegments();
	void MarkSelectedVirtualSegments( DWORD dwFlags );
	void UnMarkVirtualSegments( DWORD dwFlags );
	void DeleteMarkedVirtualSegments( DWORD dwFlags );
	
	void OnInsertTrack();
	void OnDeleteTracks();
	void MarkSelectedTracks( CVirtualSegment* pVirtualSegment, DWORD dwFlags );
	void UnMarkTracks( CVirtualSegment* pVirtualSegment, DWORD dwFlags );
	void DeleteMarkedTracks( CVirtualSegment* pVirtualSegment, DWORD dwFlags );

	void OnInsertTransition();
	void OnDeleteTransitions();
	void MarkSelectedTransitions( CVirtualSegment* pVirtualSegment, DWORD dwFlags );
	void UnMarkTransitions( CVirtualSegment* pVirtualSegment, DWORD dwFlags );
	void DeleteMarkedTransitions( CVirtualSegment* pVirtualSegment, DWORD dwFlags );

	bool IsDataObjectFromSameSong( IDataObject* pIDataObject );

public:
	CVirtualSegment* GetVirtualSegmentToEdit();
	CTrack* GetTrackToEdit();
	CTransition* GetTransitionToEdit();
	int GetVirtualSegmentSelCount();
	int GetTrackSelCount();
	int GetTransitionSelCount();

	void DoDrag( UINT nID, DWORD dwStartDragButton ); 
	void RefreshProperties( UINT nControlID );
	void SyncSongEditor( DWORD dwFlags );
	void OnUpdateEditCut( CCmdUI* pCmdUI );
	void OnEditCut();
	void OnUpdateEditCopy( CCmdUI* pCmdUI );
	void OnEditCopy();
	void OnUpdateEditPaste( CCmdUI* pCmdUI );
	void OnEditPaste();
	void OnUpdateEditInsert( CCmdUI* pCmdUI );
	void OnEditInsert();
	void OnUpdateEditDelete( CCmdUI* pCmdUI );
	void OnEditDelete();
	void OnEditSelectAll();
	BOOL OnViewProperties();
	CFont* CreateFont();
	void SelectVirtualSegment( CVirtualSegment* pVirtualSegment );
	CVirtualSegment* GetFirstSelectedVirtualSegment();

public:
	CSegmentListBox		m_lstbxSegments;			// Subclasses IDC_LIST_VIRTUAL_SEGMENTS
	CTrackListBox		m_lstbxTracks;				// Subclasses IDC_LIST_TRACKS
	CTransitionListBox	m_lstbxTransitions;			// Subclasses IDC_LIST_TRANSITIONS

private:
	CVirtualSegmentPropPageObject*	m_pVirtualSegmentPropPageObject;
	CTrackPropPageObject*			m_pTrackPropPageObject;
	CTransitionPropPageObject*		m_pTransitionPropPageObject;
	bool							m_fOpenProperties;

	int					m_nEditMenuCtrlID;			// Last control to have the focus

	IDataObject*		m_pIDataObject;				// Object being dragged	
	UINT				m_nStartDragControlID;		// Control that initiated drag operation
	DWORD				m_dwStartDragButton;		// Mouse button that initiated drag operation
	DWORD				m_dwOverDragButton;			// Mouse button stored in IDropTarget::DragOver
	DWORD				m_dwOverDragEffect;			// Drag effects stored in IDropTarget::DragOver
	CImageList*			m_pDragImage;				// Image used for drag operation feedback
	DWORD				m_dwDragRMenuEffect;		// Result from drag context menu
	CDirectMusicSong*	m_pDragSong;				// Items being dragged from this Song

	IDMUSProdNode*		m_pINodeRightMenu;			// Recipient of right menu command id
	POINT				m_pointRightMenu;			// x,y coords of right click

	DWORD				m_dwMouseDownButton;		// MK_RBUTTON or MK_LBUTTON
	CRect				m_rectMouseDown;			// If user moves out of this rect start drag-drop
	POINT				m_pointMouseDown;			// Origin of drag-drop operation

	DWORD				m_dwScrollTick;

	static long			sm_lBitmapRefCount;
	static CBitmap		sm_bmpTransition;

public:
	CSongCtrl*			m_pSongCtrl;
	CDirectMusicSong*	m_pSong;

// Form Data
public:
	//{{AFX_DATA(CSongDlg)
	enum { IDD = IDD_DLG_SONG };
	CButton	m_btnTransition;
	CComboBox	m_comboToolGraph;
	CEdit	m_editName;
	CComboBox	m_comboSegment;
	CComboBox	m_comboNextSegment;
	CButton	m_checkStartPlayback;
	CButton	m_checkQueueNext;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSongDlg)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSongDlg)
	afx_msg void OnDragRMenuMove();
	afx_msg void OnDragRMenuCopy();
	afx_msg void OnDragRMenuCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelChangeNextVirtualSegment();
	afx_msg void OnSelChangeSourceSegment();
	afx_msg void OnSelChangeToolGraph();
	afx_msg void OnKillFocusEditName();
	afx_msg void OnTransition();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONGDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_)
