/////////////////////////////////////////////////////////////////////////////
// treectl.h
//
// email	date		change
// briancr	11/01/94	created
//
// copyright 1994 Microsoft

// Interfaces of the CTreeCtl, CTreeListBox, CNode, and CImageWell classes
// Much of this code was copied from the IDE

#ifndef __TREECTL_H__
#define __TREECTL_H__

#define LIST_SEARCH_DELAY (2)

// forward declarations
class CNode;
class CImageWell;

/////////////////////////////////////////////////////////////////////////////
// CTreeListBox

class CTreeListBox : public CListBox
{
	DECLARE_DYNCREATE(CTreeListBox)

	CTreeListBox();

private:
	int ItemFromPoint(const CPoint& point);

	class CTreeCtl* m_pTree;
	BOOL m_bGotMouseDown : 1;
	BOOL m_bDeferedDeselection : 1;
	BOOL m_bStdMouse : 1;
	int m_nDeferedItem;
	int m_nLastItem;

	static int iSlobPixels;
	static CPoint mouseDownPt;

	friend class CTreeCtl;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListBox)
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnLButtonDblClk(UINT, CPoint);
	afx_msg void OnRButtonUp(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnChar(UINT nChar, UINT nRepCount, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl window

class CTreeCtl : public CWnd
{
	DECLARE_DYNCREATE(CTreeCtl)
	DECLARE_MESSAGE_MAP()

// ctor/dtor
public:
	CTreeCtl(BOOL bIsMultiSelection = FALSE, BOOL bStickySelection = FALSE, BOOL bKeepSelectionRect = FALSE);
	virtual ~CTreeCtl();
	
// list box operations
public:
	BOOL PreCreateWindow(CREATESTRUCT& cs);

	// get/set the current selection
	int GetCurSel()									{ return m_pListBox->GetCurSel(); }
	void SetCurSel(int nIndex)						{ m_pListBox->SetCurSel(nIndex); }
	void KeySetSel(int nIndex, BOOL bClearSearchBuffer = TRUE);

	// get/set the current selection if multi-select is on
	int GetSelCount()								{ return m_pListBox->GetSelCount(); }
	int GetSelItems(int nMaxItems, LPINT rgItems)	{ return m_pListBox->GetSelItems(nMaxItems, rgItems); }
	int GetSel(int nIndex)							{ return m_pListBox->GetSel(nIndex); }
	int SetSel(int nIndex, BOOL bSelect = TRUE);
	void SelectAll(void);
	void DeselectAll(void);
	void ToggleAll(void);

// Node operations
public:
	CNode* InsertNode(CNode *pParent, CString strText, DWORD dwData = 0, int nIndex = -1, UINT idGlyphClose = IDB_GlyphFolderClose, UINT idGlyphOpen = IDB_GlyphFolderOpen, BOOL bSelected = TRUE);
	CNode* InsertNode(CNode *pParent, CNode *pNode, int nIndex = -1);

	BOOL RemoveNode(CNode *pParent, int nIndex = -1);

	CNode *GetNode(CNode *pParent, int nIndex = -1);

	CNode *FindNode(CString strText, BOOL bMatchCase = FALSE);
	CNode *FindNode(DWORD dwData);

// Tree operations
public:
	void Expand(CNode *pNode, BOOL bAllDescendants = FALSE);
	void Collapse(CNode *pNode);
	void ExpandAll();

// data types (internal)
protected:
	enum HIT_ZONE { left, expand, glyph, text, right };

// Window operations (internal)
protected:
	void RecalcHorizontalExtent ();
	void ClearSearchBuffer();
	int ItemFromPoint(const CPoint& point)		{ return m_pListBox->ItemFromPoint(point); }
	int GetItemRect(int nItem, LPRECT lpRect)	{ return m_pListBox->GetItemRect(nItem, lpRect); }
	virtual void CreateListBox()				{ m_pListBox = new CTreeListBox; }
	virtual CListBox* GetListBox()				{ return m_pListBox; }
	HIT_ZONE HitTest(int nItem, int x);
	virtual void OnOpen(int nIndex);				// called on Enter key or double-click
	virtual void OnSelect(int nIndex);				// called on selection change

	virtual BOOL OnKeyDown(UINT nChar);				// called on any Keydown msg
	virtual BOOL OnChar(UINT nChar);					// called on any Char msg
	virtual void OnRightButtonUp(UINT, CPoint);		// called on right mouse button up
	virtual void OnRightButtonDown(UINT, CPoint);	// called on right mouse button down
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    void ResetSizes(CDC* pDC);
    void InvalSelection();
    void InvalItem(int nItem);
    
	int  GetMaxItemExent () { return m_cxExtent; };

// Tree operations (internal)
protected:
	enum TraversalOrder { TO_PREORDER, TO_POSTORDER };

	CNode* ForEach(CNode* pNode, BOOL (CTreeCtl::*pfn)(CNode *pNode), TraversalOrder order = TO_PREORDER);

	BOOL ExpandOne(CNode* pNode);
	BOOL SetSelOne(CNode* pNode);
	BOOL SetDeselOne(CNode* pNode);
	BOOL ToggleSelOne(CNode* pNode);

	CNode* FindNode(CNode *pNode, CString strText, BOOL bMatchCase = FALSE);
	CNode* FindNode(CNode *pNode, DWORD dwData);

	CFont* GetTreeFont(void);

	BOOL Refresh(CNode *pNode);
	void RefreshAll(void);
	void FillListBox(CNode *pNode);

	void DirtyNode(CNode *pNode, WORD);

	void FreeAllTreeData();

	// specific key handlers
	void OnPrimary();
	void OnExpand();
	void OnExpandBranch();
	void OnExpandAll();
	void OnCollapse();

	// For save/loading expanded state:

	void HoldUpdates ();
	void EnableUpdates ();

	virtual void BeginDragDrop();

// Node operations (internal)
protected:
	CNode* GetLBNode(int nIndex = -1);
	virtual int CompareData(const CNode* pNode1, const CNode* pNode2);
	int CompareNodes(const CNode* pNode1, const CNode* pNode2);
	virtual BOOL DeleteNode(CNode* pNode);

// data
protected:
	BOOL m_bIsMultiSelection : 1;	// Allow multiple selections;
	BOOL m_bStickySelection : 1;	// allows for mulitple sticky selections (LBS_MULTIPLESEL)
	BOOL m_bKeepSelectionRect : 1;	// keep the selecton rect when not focus
	BOOL m_bEnableDragDrop : 1;
	BOOL m_bShowContextPopup : 1;
	BOOL m_bShowProperties : 1;

	CTreeListBox *m_pListBox;	// our only child

	CSize m_sizeM;				// size of an "M" in the current font
								// used to determine horiz/vert tree bar dimensions

	int m_cyNode;				// height of one node in the list
	int m_cxExtent;				// approximate width of the whole list
	
	int m_EnableUpdates;		// Reference count of update holding;

	CString m_strSearch;		// typomatic search string
	time_t	m_timeLastKey;		// time since last typomatic key stroke

	CNode *m_pTree;

	static CImageWell m_Glyphs;
	static int c_cyBorder;		// SM_CYBORDER
	static int c_cyBorderX2;	// SM_CYBORDER * 2
	
	// Generated message map functions

protected:
	//{{AFX_MSG(CTreeCtl)
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnLbDblClk();
	afx_msg void OnLbSelChange();
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	
	friend class CTreeListBox;
};

#define IDCW_TREELISTBOX	1 // child id for listbox

/////////////////////////////////////////////////////////////////////////////
// CImageWell
// Container for bitmap images

class CImageWell
{
// ctor/dtor
public:
	CImageWell(void) {}
	~CImageWell();

// operations
public:
	BOOL Add(UINT idImage);
	BOOL Remove(UINT idImage);
	BOOL Draw(UINT idImage, CDC *pDC, CPoint pt, DWORD rop = 0);

// attributes
public:
	CSize GetBitmapExtent(UINT idImage);

// operations (internal)
protected:
	BOOL CalculateMask(CBitmap *pbitmap, CBitmap *pmask);

// data types
protected:
	struct ImageType {
		CBitmap* m_pbitmap;
		CBitmap* m_pmask;
		int m_count;
	};

// data
protected:
	CMap<UINT, UINT, ImageType, ImageType&> m_mapImages;
};

/////////////////////////////////////////////////////////////////////////////
// CNode
// Each entry in a CTreeCtl is a CNode

// CNode flags

#define TF_LASTLEVELENTRY		0x0001
#define TF_EXPANDED				0x0004
#define TF_DISABLED				0x0008
#define TF_DIRTY_CONTENT		0x0080
#define TF_DIRTY_ITEM			0x0010
#define TF_DROPTARGET           0x0100
#define TF_DRAGSOURCE           0x0200
#define TF_HIDDEN				0x0400
#define TF_SELECTED				0x0800

class CNode
{
// data types
public:
	typedef CTypedPtrList<CPtrList, CNode *> CNodeList;
	enum GlyphType { GT_Close = 0, GT_Open };

// ctor/dtor
public:
	CNode(CString strText = "", DWORD dwData = 0, CNode *pParent = NULL, UINT idGlyphClose = IDB_GlyphFolderClose, UINT idGlyphOpen = IDB_GlyphFolderOpen);
	virtual ~CNode();

// operations
	virtual BOOL SetText(CString strText)							{ ASSERT(!strText.IsEmpty()); m_strText = strText; return TRUE; }
	virtual BOOL SetData(DWORD dwData)								{ m_dwData = dwData; return TRUE; }
	virtual BOOL SetParent(CNode *pParent)							{ ASSERT(pParent); m_pParent = pParent; return TRUE; }
	virtual BOOL SetFlags(int nFlags)								{ m_nFlags |= nFlags; return TRUE; }
	virtual BOOL ClearFlags(int nFlags)								{ m_nFlags &= ~nFlags; return TRUE; }
	virtual BOOL DrawGlyph(GlyphType glyph, CDC *pDC, CPoint pt)	{ return m_Glyphs.Draw(glyph == GT_Close ? m_idGlyphClose : m_idGlyphOpen, pDC, pt); }

// attributes
	virtual CString GetText(void)					{ ASSERT(!m_strText.IsEmpty()); return m_strText; }
	virtual DWORD GetData(void) const				{ return m_dwData; }
	virtual CNode *GetParent(void)					{ return m_pParent; }
	virtual CNodeList *GetChildren(void)			{ return &m_listChildren; }
	virtual int GetFlags(void)						{ return m_nFlags; }
	virtual int GetLevel(void)						{ if (m_pParent == NULL) return 0; return 1 + m_pParent->GetLevel(); }
	virtual UINT GetGlyphId(GlyphType glyph)		{ return glyph == GT_Close ? m_idGlyphClose : m_idGlyphOpen; }

// REVIEW(CFlaat)

	virtual void SetGlyphIds(UINT idGlyph)
  	{ m_idGlyphClose = m_idGlyphOpen = idGlyph; }


	virtual CSize GetGlyphExtent(GlyphType glyph)	{ return m_Glyphs.GetBitmapExtent(glyph == GT_Close ? m_idGlyphClose : m_idGlyphOpen); }

// data
protected:
	CString m_strText;
	DWORD m_dwData;
	CNode *m_pParent;
	CTypedPtrList<CPtrList, CNode *> m_listChildren;
	int m_nFlags;
	UINT m_idGlyphClose;
	UINT m_idGlyphOpen;

	static CImageWell m_Glyphs;
};

#endif // __TREECTL_H__
