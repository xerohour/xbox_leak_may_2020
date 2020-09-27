// treectl.h : header file
//

#ifndef __TREECTL_H__
#define __TREECTL_H__

#ifndef __IMGWELL_H__
#include "imgwell.h"
#endif

//[patbr] we are not a DLL
//#undef AFXAPP_DATA
//#define AFXAPP_DATA AFXEXT_DATA

//[patbr] get rid of this external dependency
//class CPartFrame;
/////////////////////////////////////////////////////////////////////////////
// Values for CNode::m_nFlags
// CNode -- one entry in a CTreeCtl

// Specify packing becuase this gets used accross DLL's:
#pragma pack(4)
class CNode
{
public:
	CNode(DWORD dwData);
	virtual ~CNode();

	virtual CFont* GetFont() const;
	virtual int GetGlyphWidth();

	CNode* m_pParent;
	int m_nLevels;
	DWORD m_dwData;
	WORD m_nFlags;
	TCHAR * m_szText;
	int  m_nWidth;
};
#pragma pack()

#define TF_LASTLEVELENTRY		0x0001
#define TF_HASCHILDREN			0x0002
#define TF_EXPANDED				0x0004
#define TF_DISABLED				0x0008
#define TF_EXPANDABLE			0x0020
#define TF_NOT_EXPANDABLE		0x0040
#define TF_DIRTY_CONTENT		0x0080
#define TF_DIRTY_ITEM			0x0010
#define TF_DROPTARGET           0x0100
#define TF_DRAGSOURCE           0x0200

#define LIST_SEARCH_DELAY (2)

/////////////////////////////////////////////////////////////////////////////
// CTreeListBox item

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

// Construction
public:
	CTreeCtl(BOOL bIsMultiSelection = FALSE, BOOL bKeepSelectionRect = FALSE);
	
	BOOL PreCreateWindow(CREATESTRUCT& cs);

// Attributes
public:

protected:
	BOOL m_bIsMultiSelection : 1;	// Allow multiple selections;
	BOOL m_bKeepSelectionRect : 1;	// keep the selecton rect when not focus
	BOOL m_bEnableDragDrop : 1;
	BOOL m_bShowContextPopup : 1;
	BOOL m_bShowProperties : 1;

	CTreeListBox m_listBox;		// our only child

	CSize m_sizeM;				// size of an "M" in the current font
								// used to determine horiz/vert tree bar dimensions

	int m_cyNode;				// height of one node in the list
	int m_cxExtent;				// approximate width of the whole list
	
	int m_EnableUpdates;		// Reference count of update holding;

	CString m_strSearch;		// typomatic search string
	time_t	m_timeLastKey;		// time since last typomatic key stroke

	//[byrond] Support for unsorted bitmaps (added using a cache)
	BOOL m_bSorted;				// TRUE if the levels should be sorted
	CNode* m_pParentNodeCache;	// Last parent node that had a child
								// added.  This is used for the cache.
	int m_iNextCache;			// Cached next index for the above parent node

// Operations
public:
	// expansion/contraction

	// override this method so that containment can be determined
	virtual BOOL NodeIsExpandable(CNode *);
	
	// *must* override this method so that the text of a node can be
	// determined through means other than the passed 'sz' arg to InsertNode
	// N.B. generally this is for the a node that was set dirty
	virtual void GetNodeTitle(CNode *, CString &);

	// expand/collapse a node (referenced by index)
	void Expand(int nIndex = -1, BOOL bWholeBranch = FALSE);
	void Collapse(int nIndex = -1);
	void Refresh(int nIndex = -1);
	void RefreshAllRoots();

	// expand all nodes (full-expansion)
	//void ExpandAll() 
	//	{Expand(0, TRUE);}
	void ExpandAll();

	// is a particular node expanded?
	BOOL IsExpanded(int nIndex = -1);
	
	// does a particular node have children?
	BOOL HasChildren(int nIndex = -1);

	// get/set the node dwData (referenced by index)
	DWORD GetData(int nIndex = -1);
	void SetData(int nIndex, DWORD dwData);

	// get/set the current selection
	int GetCurSel() { return m_listBox.GetCurSel(); }
	void SetCurSel(int nIndex) { m_listBox.SetCurSel(nIndex); }
	void KeySetSel(int nIndex, BOOL bClearSearchBuffer = TRUE);

	// get/set the current selection if multi-select is on
	int GetSelCount() { return m_listBox.GetSelCount(); }
	int GetSelItems(int nMaxItems, LPINT rgItems) { return m_listBox.GetSelItems(nMaxItems, rgItems); }
	int SetSel(int nIndex, BOOL bSelect = TRUE) { return m_listBox.SetSel(nIndex, bSelect); }
	void SelectAll()
		{ SetSel(-1, TRUE); }

	// our tree-control node modification functions
	int FindNode(DWORD dwData);
	int FindNode(const TCHAR * szText, DWORD & dwData, BOOL fMatchCase = FALSE);
	int InsertNode(int iParentNode, const char* szName, DWORD dwData);
	void DirtyNode(int iNode, WORD);	// where the 2nd arg. is TF_DIRTY_???
	void RemoveNode(int iNode, BOOL bUpdateSel = TRUE);

	void RecalcHorizontalExtent ();
	void ClearSearchBuffer();
	
	int ItemFromPoint(const CPoint& point)
		{ return m_listBox.ItemFromPoint(point); }

	int GetItemRect(int nItem, LPRECT lpRect)
		{ return m_listBox.GetItemRect(nItem, lpRect); }

	CListBox* GetListBox()
		{ return &m_listBox; }
		
	enum HIT_ZONE { left, expand, glyph, text, right };
	
	HIT_ZONE HitTest(int nItem, int x);
	
	// override these methods to support your own actions for the user input

	virtual void OnOpen(int nIndex);				// called on Enter key or double-click
	virtual void OnSelect(int nIndex);				// called on selection change

	virtual BOOL OnKeyDown(UINT nChar);				// called on any Keydown msg
	virtual BOOL OnChar(UINT nChar);					// called on any Char msg

	// specific key handlers
	void OnPrimary();
	void OnExpand();
	void OnExpandBranch();
	void OnExpandAll();
	void OnCollapse();
	
	virtual void OnRightButtonUp(UINT, CPoint);		// called on right mouse button up
	virtual void OnRightButtonDown(UINT, CPoint);	// called on right mouse button down
	
// Implementation
public:
	virtual ~CTreeCtl();
	void FreeAllTreeData();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL InsertLevel(CNode* pNode, int nIndex, BOOL bExpandAll = FALSE);
	virtual void DrawNodeGlyph(CDC* pDC, CNode* pNode, BOOL bHighlight, CPoint pt);
	
	BOOL InsertNode(CNode* pParentNode, int iParentNode, const char* szName,
					DWORD dwData, CNode*& rpNodeRet, int& riNodeRet);
	void Collapse(CNode* pNode, int nIndex);
	void Refresh(CNode* pNode, int nIndex);
    CNode* GetNode(int nIndex = -1);
    void ResetSizes(CDC* pDC);
    void InvalSelection();
    void InvalItem(int nItem);
    
	int  GetMaxItemExent () { return m_cxExtent; };

#if 0	//[patbr] usused code
	// For save/loading expanded state:
	BYTE* GetInitData();
	void ReadInitData ( CPartFrame *pFrame );
	void SizeViewToTree ( CPartFrame *pFrame );
#endif	//[patbr] end of unused code...

	void HoldUpdates ();
	void EnableUpdates ();

	virtual int CompareData(const CNode* pNode1, const CNode* pNode2);
	int CompareNodes(const CNode* pNode1, const CNode* pNode2);

	virtual CNode* CreateNode(DWORD dwData);
	virtual void DeleteNode(CNode* pNode);

	virtual void BeginDragDrop();

	//[byrond] Support for the unsorted tree cache
	void FlushTheCache() { m_pParentNodeCache = NULL; m_iNextCache = 0; }

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
	
	static CImageWell c_imageWell;
	static int c_cyBorder;		// SM_CYBORDER
	static int c_cyBorderX2;	// SM_CYBORDER * 2
	
	friend class CTreeListBox;
};

#define IDCW_TREELISTBOX	1 // child id for listbox



/////////////////////////////////////////////////////////////////////////////

//[patbr] we are not a DLL
//#undef AFXAPP_DATA
//#define AFXAPP_DATA NEAR

#endif // __TREECTL_H__
