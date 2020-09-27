#ifndef __STYLEDLG_H__
#define __STYLEDLG_H__

// StyleDlg.h : header file
//

#include "Splitter.h"

/////////////////////////////////////////////////////////////////////////////
// CStyleDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define SSE_STYLE		1
#define SSE_BANDS		2
#define SSE_PATTERNS	4
#define SSE_MOTIFS		8
#define SSE_ALL			0xFFFFFFFF

class CStyleCtrl;


/////////////////////////////////////////////////////////////////////////////
// CStyleComboBox window

class CStyleComboBox : public CComboBox
{
// Construction
public:
	CStyleComboBox();

// Attributes
public:
	CStyleDlg*	m_pStyleDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStyleComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStyleComboBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CStyleListBox window

class CStyleListBox : public CListBox
{
// Construction
public:
	CStyleListBox();

// Attributes
public:
	CStyleDlg*	m_pStyleDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStyleListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStyleListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CStyleDlg window

class CStyleDlg : public CFormView, public IDropSource, public IDropTarget
{
friend class CStyleListBox;
friend class CStyleComboBox;

public:
	CStyleDlg();
	virtual ~CStyleDlg();

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
	void SetTempoControlText();
	void SetTimeSignatureBitmap();
	void FillPatternListBox();
	void FillMotifListBox();
	void FillBandComboBox();
	void DrawPatternItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	void DrawMotifItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	void PlayMotif();
	int GetTopBorder();

public:
	void DoDrag( IDMUSProdNode* pINode, DWORD dwStartDragButton ); 
	BOOL HandleKeyDown( MSG* pMsg );
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
	BOOL OnViewProperties();
	void RefreshControls( DWORD dwFlags );
	void EnableTimer( BOOL fEnable );
	void EndTrack( int nHeight );
	void SelectPattern( CDirectMusicPattern* pPattern );
	void SelectMotif( CDirectMusicPattern* pMotif );
	void SelectBand( IDMUSProdNode* pINode );

protected:
	DECLARE_DYNCREATE(CStyleDlg)

// Form Data
public:
	//{{AFX_DATA(CStyleDlg)
	enum { IDD = IDD_DLG_STYLE };
	CStatic	m_staticSeparator;
	CButton	m_btnTimeSignature;
	CSpinButtonCtrl	m_spinTempo;
	CEdit	m_editTempo;
	CButton	m_checkBandDefault;
	CListBox	m_PatternList;
	//}}AFX_DATA

// Member variables
private:
	DWORD			m_dwRef;
	IDataObject*	m_pIDataObject;			// Object being dragged	
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton;		// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect;		// Drag effects stored in IDropTarget::DragOver
	CImageList*		m_pDragImage;			// Image used for drag operation feedback
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	IDMUSProdNode*	m_pIDragNode;			// Node being dragged
	IDMUSProdNode*	m_pINodeRightMenu;		// Recipient of right menu command id
	int				m_nEditMenuCtrlID;		// Last "list" control to have the focus
	short			m_nTimerID;				// Used to display transport cursor

	DWORD			m_dwMouseDownButton;	// MK_RBUTTON or MK_LBUTTON
	IDMUSProdNode*		m_pIMouseDownNode;		// Object under cursor when mouse button pressed
	CRect			m_rectMouseDown;		// If user moves out of this rect start drag-drop

	CStyleComboBox	m_cmbxBand;				// Subclasses Band combo box
	CStyleListBox	m_lstbxPattern;			// Subclasses Pattern list box
	CStyleListBox	m_lstbxMotif;			// Subclasses Motif list box

	CSplitter		m_wndSplitter;
	CFont*			m_pFont;

public:
	CStyleCtrl*			m_pStyleCtrl;
	CDirectMusicStyle*	m_pStyle;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleDlg)
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
	//{{AFX_MSG(CStyleDlg)
	afx_msg void OnDragRMenuMove();
	afx_msg void OnDragRMenuCopy();
	afx_msg void OnDragRMenuCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocusTempo();
	afx_msg void OnDeltaPosTempoSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimeSignature();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg int OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	afx_msg void OnDblClkPatternList();
	afx_msg void OnDblClkMotifList();
	afx_msg void OnSelChangeMotifList();
	afx_msg void OnSelChangePatternList();
	afx_msg void OnSelChangeBandCombo();
	afx_msg void OnSetFocusBandCombo();
	afx_msg void OnKillFocusBandCombo();
	afx_msg void OnSetFocusPatternList();
	afx_msg void OnKillFocusPatternList();
	afx_msg void OnSetFocusMotifList();
	afx_msg void OnKillFocusMotifList();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnBandDefault();
	afx_msg void OnDblClkBandDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __STYLEDLG_H__
