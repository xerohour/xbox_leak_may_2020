#if !defined(AFX_GRAPHDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_)
#define AFX_GRAPHDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// GraphDlg.h : header file
//

#include "Splitter.h"


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGraphHeaderBtn window

class CGraphHeaderBtn : public CButton
{
friend class CGraphDlg;

// Construction
public:
	CGraphHeaderBtn();
	virtual ~CGraphHeaderBtn();

// Attributes
public:
	CGraphDlg*	m_pGraphDlg;
	int			m_nLastXPos;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphHeaderBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	HRESULT CreateDataObject( IDataObject** ppIDataObject );

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphHeaderBtn)
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGraphListBox window

class CGraphListBox : public CListBox
{
// Construction
public:
	CGraphListBox();
	virtual ~CGraphListBox();

// Attributes
public:
	CGraphDlg*	m_pGraphDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_VIRTUAL

protected:
	short	m_nSetPChannelState;

// Implementation
public:
	int		m_nHScrollPos;
	int		m_nVScrollPos;

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPChannelHeaderBtn window

class CPChannelHeaderBtn : public CButton
{
friend class CGraphDlg;

// Construction
public:
	CPChannelHeaderBtn();

// Attributes
public:
	CGraphDlg*	m_pGraphDlg;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPChannelHeaderBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPChannelHeaderBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPChannelHeaderBtn)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPChannelListBox window

class CPChannelListBox : public CListBox
{
// Construction
public:
	CPChannelListBox();

// Attributes
public:
	CGraphDlg*	m_pGraphDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPChannelListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPChannelListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPChannelListBox)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDblClk();
	afx_msg int VKeyToItem(UINT nKey, UINT nIndex);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CToolHeaderBtn window

class CToolHeaderBtn : public CButton
{
friend class CGraphDlg;

// Construction
public:
	CToolHeaderBtn();

// Attributes
public:
	CGraphDlg*	m_pGraphDlg;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolHeaderBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolHeaderBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolHeaderBtn)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CToolListBox window

class CToolListBox : public CListBox
{
// Construction
public:
	CToolListBox();
	virtual ~CToolListBox();

// Attributes
public:
	CGraphDlg*	m_pGraphDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	HRESULT CreateDataObject( IDataObject** ppIDataObject );

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolListBox)
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGraphDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CGraphDlg : public CFormView, public IDropSource, public IDropTarget
{
friend class CPChannelHeaderBtn;
friend class CPChannelListBox;
friend class CGraphHeaderBtn;
friend class CGraphListBox;
friend class CToolHeaderBtn;
friend class CToolListBox;

public:
	CGraphDlg();           // protected constructor used by dynamic creation
	virtual ~CGraphDlg();

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
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	// Additional methods
protected:
	HRESULT CanPasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	HRESULT PasteFromData( IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	HRESULT PasteCF_GRAPH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, bool fInDragDrop, POINT pt );
	void FillToolsListBox();
	void FillPChannelListBox();
	void FillGraphListBox();
	
	void OnChangePChannelGroup( POINT point, bool fInsertNewGroup );
	void OnDeletePChannelGroups();

	CTool* GetToolFromXPos( int nXPos );
	void OnDeleteTools();

public:
	void DoDrag( UINT nID, DWORD dwStartDragButton ); 
	void RefreshControls();
	void ResetContent();
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
	void EndTrack( int nID, int nXPos );
	CFont* CreateFont();

private:
	CPChannelHeaderBtn	m_btnPChannelHeader;		// Subclasses IDC_PCHANNEL_HEADER
	CPChannelListBox	m_lstbxPChannels;			// Subclasses IDC_PCHANNEL_LIST
	CGraphHeaderBtn		m_btnGraphHeader;			// Subclasses IDC_GRAPH_HEADER
	CGraphListBox		m_lstbxGraph;				// Subclasses IDC_GRAPH_LIST
	CToolHeaderBtn		m_btnToolHeader;			// Subclasses IDC_TOOL_HEADER
	CToolListBox		m_lstbxTools;				// Subclasses IDC_TOOL_LIST

	CSplitter			m_wndPChannelSplitter;
	CSplitter			m_wndToolSplitter;
	int					m_nPChannelSplitterXPos;
	int					m_nToolSplitterXPos;

	int					m_nEditMenuCtrlID;			// Last control to have the focus

	IDataObject*		m_pIDataObject;				// Object being dragged	
	UINT				m_nStartDragControlID;		// Control that initiated drag operation
	DWORD				m_dwStartDragButton;		// Mouse button that initiated drag operation
	DWORD				m_dwOverDragButton;			// Mouse button stored in IDropTarget::DragOver
	DWORD				m_dwOverDragEffect;			// Drag effects stored in IDropTarget::DragOver
	CImageList*			m_pDragImage;				// Image used for drag operation feedback
	DWORD				m_dwDragRMenuEffect;		// Result from drag context menu
	CDirectMusicGraph*	m_pDragGraph;				// Tools being dragged from this Graph
	bool				m_fDragToSameGraph;			// Tools were dropped in same Graph

	IDMUSProdNode*		m_pINodeRightMenu;			// Recipient of right menu command id
	POINT				m_pointRightMenu;			// x,y coords of right click

	DWORD				m_dwMouseDownButton;		// MK_RBUTTON or MK_LBUTTON
	CRect				m_rectMouseDown;			// If user moves out of this rect start drag-drop
	POINT				m_pointMouseDown;			// Origin of drag-drop operation

	CTool*				m_pToolForShiftClick;
	CTool*				m_pToolToToggle;
	DWORD				m_dwScrollTick;

public:
	CGraphCtrl*			m_pGraphCtrl;
	CDirectMusicGraph*	m_pGraph;

// Form Data
public:
	//{{AFX_DATA(CGraphDlg)
	enum { IDD = IDD_DLG_GRAPH };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphDlg)
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
	//{{AFX_MSG(CGraphDlg)
	afx_msg void OnDragRMenuMove();
	afx_msg void OnDragRMenuCopy();
	afx_msg void OnDragRMenuCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_)
