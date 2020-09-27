#ifndef __TREEBAR_H__
#define __TREEBAR_H__

// TreeBar.h : header file
//

#include "EditLabel.h"

#define NS_NONE			0
#define NS_SELECT		1
#define NS_SELECTDROP	2

#define DTN_PROMPT_DELETE		0x0001
#define DTN_PROMPT_REFERENCES	0x0002
#define DTN_PROMPT_USER			0x0003

#define TREE_SORTBY_NAME	1
#define TREE_SORTBY_TYPE	2
#define TREE_SORTBY_SIZE	3

/////////////////////////////////////////////////////////////////////////////
// CSplitter class

class CSplitter : public CWnd
{
protected:
   CRect m_rcTrack;
   BOOL  m_fTracking;
   BOOL  m_fVisible;

public:
   CSplitter();
   BOOL Create(CWnd *pParent);

protected:
   void OnEndCapture();
   void InvertTracker();

   //{{AFX_VIRTUAL(CSplitter)
   //}}AFX_VIRTUAL

public:
   virtual ~CSplitter();

protected:
   //{{AFX_MSG(CSplitter)
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnCancelMode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
   afx_msg void OnCaptureChanged(CWnd *pWnd);
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CTreeBar control bar

class CTreeBar : public CDialogBar, public IDropSource, public IDropTarget
{
public:
	CTreeBar();
	virtual ~CTreeBar();
	BOOL Create( CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID );
	BOOL Create( CWnd* pParentWnd, LPCSTR szTemplateName, UINT nStyle, UINT nID );

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
private:
	HTREEITEM GetItemFromPoint( CPoint pt );
	void DoDrag( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, DWORD dwStartDragButton, LRESULT* pResult ); 
	BOOL CopyToClipboard( IDMUSProdNode* pINode );

public:
	BOOL DeleteTreeNode( IDMUSProdNode* pINode, WORD wFlags );
	void CheckEditControl();
	void OnUpdateEditCut( CCmdUI* pCmdUI );
	void OnEditCut();
	void OnUpdateEditCopy( CCmdUI* pCmdUI );
	void OnEditCopy();
	void OnUpdateEditPaste( CCmdUI* pCmdUI );
	void OnEditPaste();
	void OnUpdateEditDelete( CCmdUI* pCmdUI );
	void OnEditDelete();
	void OnBeginLabelEdit( TV_DISPINFO FAR* pTVDispInfo, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void OnEndLabelEdit( TV_DISPINFO FAR* pTVDispInfo, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void OnItemExpanded( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void OnDeleteItem( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void OnSelChanged( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void OnBeginDrag( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void OnBeginRDrag( NM_TREEVIEW* pNMTreeView, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void OnDoubleClick( NMHDR* pNMHdr, CTreeCtrl* pTreeCtrl, BOOL fEnterKey, LRESULT* pResult ); 
	void OnRightClick( NMHDR* pNMHdr, CTreeCtrl* pTreeCtrl, CPoint pt, LRESULT* pResult ); 
	void OnSetFocus( NMHDR* pNMHdr, CTreeCtrl* pTreeCtrl, LRESULT* pResult ); 
	void RightClickMenu( IDMUSProdNode* pINode, CPoint pt ); 
	void SortChildren( HTREEITEM hParentItem );
	void DisplayNodeProperties();
	int GetSortType();
	void SetSortType( int nNewSortType );

// Attributes
protected:
	DWORD			m_dwRef;

	IDataObject*	m_pIDataObject;			// Object being dragged	
	IDMUSProdNode*	m_pIDragNode;			// Node being dragged
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton;		// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect;		// Drag effects stored in IDropTarget::DragOver
	CImageList*		m_pDragImage;			// Image used for drag operation feedback
	HTREEITEM		m_hDropItem;			// Current target node
	IDMUSProdNode*	m_pINodeRightMenu;		// Node tracking popup menu
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	DWORD			m_dwDragScrollTick;		// Used to scroll Project Tree during drag
	DWORD			m_dwDragExpandTick;		// Used to expand Project Tree node during drag
	HACCEL			m_hAcceleratorTable;

	IDataObject*	m_pIClipboardDataObject;// Object copied into clipboard
	IDMUSProdNode*	m_pIClipboardNode;		// Node responsible for data copied into clipboard

	CEditLabel*		m_pEditCtrl;
	CSplitter		m_wndSplitter;
	int				m_nWidth;
	int				m_nSortType;
	
public:
	CSize			m_sizeFloating;
	CSize			m_sizeDocked;

//Operations
public:

// Overrides
public:
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
	//{{AFX_MSG(CTreeBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnDragRMenuMove();
	afx_msg void OnDragRMenuCopy();
	afx_msg void OnDragRMenuCancel();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __TREEBAR_H__
