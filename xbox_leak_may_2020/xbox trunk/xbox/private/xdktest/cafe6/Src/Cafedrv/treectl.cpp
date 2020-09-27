/////////////////////////////////////////////////////////////////////////////
// treectl.cpp
//
// email	date		change
// briancr	11/01/94	created
//
// copyright 1994 Microsoft

// Implementation of the CTreeCtl, CTreeListBox, CNode, and CImageWell classes
// Much of this code was copied from the IDE

#include "stdafx.h"
#include "treectl.h"
#include "caferes.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl

int CTreeCtl::c_cyBorder = GetSystemMetrics(SM_CYBORDER);
int CTreeCtl::c_cyBorderX2 = c_cyBorder * 2;

CImageWell CTreeCtl::m_Glyphs;

// NOTE: We invert the standard MFC GetHalftoneBrush since this code was
// written to line up dots based on our old GetHalftoneBrush which was
// one off from MFC's...
#define NOTPATCOPY 0x000F0001	// Pn

IMPLEMENT_DYNAMIC(CTreeListBox, CWnd)

BEGIN_MESSAGE_MAP(CTreeListBox, CWnd)
	//{{AFX_MSG_MAP(CTreeListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CTreeCtl, CWnd)

BEGIN_MESSAGE_MAP(CTreeCtl, CWnd)
	//{{AFX_MSG_MAP(CTreeCtl)
	ON_WM_CHARTOITEM()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_VKEYTOITEM()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_LBN_DBLCLK(IDCW_TREELISTBOX, OnLbDblClk)
	ON_LBN_SELCHANGE(IDCW_TREELISTBOX, OnLbSelChange)
	ON_LBN_SETFOCUS(IDCW_TREELISTBOX, InvalSelection)
	ON_LBN_KILLFOCUS(IDCW_TREELISTBOX, InvalSelection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl ctor/dtor

CTreeCtl::CTreeCtl(BOOL bIsMultiSelection /*= FALSE*/, BOOL bStickySelection /*= FALSE*/, BOOL bKeepSelectionRect /*=FALSE*/)
{
	m_Glyphs.Add(IDB_GlyphExpanded);
	m_Glyphs.Add(IDB_GlyphCollapsed);
	m_Glyphs.Add(IDB_GlyphTest);
	m_Glyphs.Add(IDB_GlyphBadTest);

	m_bIsMultiSelection = bIsMultiSelection;
	m_bKeepSelectionRect= bKeepSelectionRect;
	// sticky selection overrides multiselection
	m_bStickySelection = bStickySelection;
	if (m_bStickySelection) {
		m_bIsMultiSelection = !m_bStickySelection;
	};
	m_EnableUpdates = 0;
	m_bEnableDragDrop = FALSE;
	m_bShowContextPopup = FALSE;
	m_bShowProperties = FALSE;
	m_timeLastKey = 0;
	m_pTree = new CNode("Tree root");
	m_pTree->SetFlags(TF_HIDDEN);
}

CTreeCtl::~CTreeCtl()
{
	m_Glyphs.Remove(IDB_GlyphExpanded);
	m_Glyphs.Remove(IDB_GlyphCollapsed);
	m_Glyphs.Remove(IDB_GlyphTest);
	m_Glyphs.Remove(IDB_GlyphBadTest);

	if (m_hWnd != NULL)
		DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl Window operations

BOOL CTreeCtl::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(0);
			
	return TRUE;
}

void CTreeCtl::KeySetSel(int nIndex, BOOL bClearSearchBuffer)
{
	// called when user selects a specific item with the keyboard
	if (bClearSearchBuffer)
		ClearSearchBuffer();

	if (m_bIsMultiSelection)
		SetSel(-1, FALSE);

	SetSel(nIndex, TRUE);
	OnLbSelChange();
}

int CTreeCtl::SetSel(int nIndex, BOOL bSelect /*= TRUE*/)
{
	CNode* pNode;

	// this function only works with multiple selection list boxes
	ASSERT((m_pListBox->GetStyle() & LBS_EXTENDEDSEL) || (m_pListBox->GetStyle() & LBS_MULTIPLESEL));

	// if we're selecting a single item, mark its node as selected, too
	if (nIndex >= 0) {
		pNode = GetLBNode(nIndex);
		if (bSelect) {
			pNode->SetFlags(TF_SELECTED);
		}
		else {
			pNode->ClearFlags(TF_SELECTED);
		}
	}

	return m_pListBox->SetSel(nIndex, bSelect);
}

void CTreeCtl::SelectAll(void)
{
	// this function only works with multiple selection list boxes
	ASSERT((m_pListBox->GetStyle() & LBS_EXTENDEDSEL) || (m_pListBox->GetStyle() & LBS_MULTIPLESEL));

	// mark each node as selected, starting with the root
	ForEach(m_pTree, &CTreeCtl::SetSelOne);

	// select all items in the list
	SetSel(-1, TRUE);
}

void CTreeCtl::DeselectAll(void)
{
	// this function only works with multiple selection list boxes
	ASSERT((m_pListBox->GetStyle() & LBS_EXTENDEDSEL) || (m_pListBox->GetStyle() & LBS_MULTIPLESEL));

	// mark each node as deselected, starting with the root
	ForEach(m_pTree, &CTreeCtl::SetDeselOne);

	// deselect all items in the list
	SetSel(-1, FALSE);
}

void CTreeCtl::ToggleAll(void)
{
	// this function only works with multiple selection list boxes
	ASSERT((m_pListBox->GetStyle() & LBS_EXTENDEDSEL) || (m_pListBox->GetStyle() & LBS_MULTIPLESEL));

	// toggle each node, starting with the root
	ForEach(m_pTree, &CTreeCtl::ToggleSelOne);

	// invalidate the window to repaint everything
	Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// CTreeCtl Node operations

CNode* CTreeCtl::InsertNode(CNode *pParent, CString strText, DWORD dwData /*= 0*/, int nIndex /*= -1*/, UINT idGlyphClose /*= IDB_GlyphFolderClose*/, UINT idGlyphOpen /*= IDB_GlyphFolderOpen*/, BOOL bSelected /*= TRUE*/)
{
	CNode *pNode;
	CNode *pRetNode;

	// the text must not be empty
	ASSERT(!strText.IsEmpty());

	// create a new node
	pNode = new CNode(strText, dwData, pParent, idGlyphClose, idGlyphOpen);

	// set selected flag
	pNode->SetFlags(bSelected ? TF_SELECTED : 0);

	// insert this node
	pRetNode = InsertNode(pParent, pNode, nIndex);
	if (!pRetNode) {
		// delete the node
		delete pNode;
	}
	return pRetNode;
}

CNode* CTreeCtl::InsertNode(CNode *pParent, CNode *pNode, int nIndex /*= -1*/)
{
	CNode::CNodeList *plistChildren;
	POSITION pos;

	// if the parent node is NULL, insert the new node at the root
	if (!pParent) {
		pParent = m_pTree;
	}

	// get the list of children for this node
	plistChildren = pParent->GetChildren();

	// if there are any children, clear the LASTLEVELENTRY flag from the last child
	if (!plistChildren->IsEmpty()) {
		plistChildren->GetTail()->ClearFlags(TF_LASTLEVELENTRY);
	}

	// if index < 0 or >= number of children, insert the new node at the end of the child list
	if (nIndex < 0 || nIndex >= plistChildren->GetCount()) {
		plistChildren->AddTail(pNode);
	}
	// otherwise, insert at the given index
	else {
		pos = plistChildren->FindIndex(nIndex);
		// this should never be NULL
		ASSERT(pos);
		plistChildren->InsertAfter(pos, pNode);
	}

	// set the LASTLEVELENTRY flag for the last child
	plistChildren->GetTail()->SetFlags(TF_LASTLEVELENTRY);

	// expand the parent when a new child is added
	Expand(pParent, FALSE);

	return pNode;
}

BOOL CTreeCtl::RemoveNode(CNode *pParent, int nIndex /*= -1*/)
{
	CNode::CNodeList *plistChildren;
	CNode *pNode;
	POSITION pos;

	// if the parent node is NULL, remove the node from the root
	if (!pParent) {
		pParent = m_pTree;
	}

	// get the list of children for this node
	plistChildren = pParent->GetChildren();

	// if there are no children, don't remove any
	if (plistChildren->IsEmpty()) {
		return FALSE;
	}

	// clear the LASTLEVELENTRY flag from the last child
	plistChildren->GetTail()->ClearFlags(TF_LASTLEVELENTRY);

	// if index < 0 or >= number of children, remove the last node in the list
	if (nIndex < 0 || nIndex >= plistChildren->GetCount()) {
		pNode = plistChildren->RemoveTail();
	}
	// otherwise, remove the node at the given index
	else {
		pos = plistChildren->FindIndex(nIndex);
		// this should never be NULL
		ASSERT(pos);
		pNode = plistChildren->GetAt(pos);
		plistChildren->RemoveAt(pos);
	}
	
	// as long as there are more children, set the LASTLEVELENTRY flag for the last child
	if (!plistChildren->IsEmpty()) {
		plistChildren->GetTail()->SetFlags(TF_LASTLEVELENTRY);
	}

	// delete the node
	delete pNode;

	return TRUE;
}

CNode *CTreeCtl::GetNode(CNode *pParent, int nIndex /*= -1*/)
{
	CNode::CNodeList *plistChildren;
	CNode *pNode;
	POSITION pos;

	// if the parent node is NULL, get the node from the root
	if (!pParent) {
		pParent = m_pTree;
	}

	// get the list of children for this node
	plistChildren = pParent->GetChildren();

	// if there are no children, don't get any
	if (plistChildren->IsEmpty()) {
		return NULL;
	}

	// if index < 0 or >= number of children, get the last node in the list
	if (nIndex < 0 || nIndex >= plistChildren->GetCount()) {
		pNode = plistChildren->GetTail();
	}
	// otherwise, remove the node at the given index
	else {
		pos = plistChildren->FindIndex(nIndex);
		// this should never be NULL
		ASSERT(pos);
		pNode = plistChildren->GetAt(pos);
	}
	
	return pNode;
}

CNode *CTreeCtl::FindNode(CString strText, BOOL bMatchCase /*= FALSE*/)
{
	// perform a preorder traversal, looking for a node with strText
	return FindNode(m_pTree, strText, bMatchCase);
}

CNode *CTreeCtl::FindNode(DWORD dwData)
{
	// perform a preorder traversal, looking for a node with dwData
	return FindNode(m_pTree, dwData);
}

CNode *CTreeCtl::FindNode(CNode *pNode, CString strText, BOOL bMatchCase /*= FALSE*/)
{
	ASSERT(0);
	// TODO(briancr): preorder traversal of node, looking for strText
	return NULL;

	DBG_UNREFERENCED_PARAMETER(pNode);
	DBG_UNREFERENCED_PARAMETER(strText);
	DBG_UNREFERENCED_PARAMETER(bMatchCase);
}

CNode* CTreeCtl::FindNode(CNode* pNode, DWORD dwData)
{
	// is this the node?
	if (pNode->GetData() == dwData) {
		return pNode;
	}

	// search children

	// get the list of children
	CNode::CNodeList* plistChildren = pNode->GetChildren();

	// call FindNode for each child
	for (POSITION pos = plistChildren->GetHeadPosition(); pos != NULL; ) {
		CNode* pChild = plistChildren->GetNext(pos);
		CNode* pRetNode = FindNode(pChild, dwData);
		if (pRetNode != NULL) {
			return pRetNode;
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl tree operations

void CTreeCtl::Expand(CNode *pNode, BOOL bAllDescendants /*= FALSE*/)
{
	// mark this node as expanded
	pNode->SetFlags(TF_EXPANDED);

	// do we need to expand all descendants?
	if (bAllDescendants) {
		// expand all descendants
		ForEach(pNode, &CTreeCtl::ExpandOne);
	}
}

void CTreeCtl::Collapse(CNode *pNode)
{
	// mark this node as not expanded
	pNode->ClearFlags(TF_EXPANDED);
}

void CTreeCtl::ExpandAll()
{
	// mark all descendants of m_pTree as expanded
	ForEach(m_pTree, &CTreeCtl::ExpandOne);
}


/////////////////////////////////////////////////////////////////////////////
// CTreeCtl Window operations (internal)


void CTreeCtl::ResetSizes(CDC* pDC)
{
	m_sizeM = pDC->GetTextExtent("M", 1);
	// Make sure this is even so our brush lines up when we scroll:
	m_cyNode = (max(m_sizeM.cy, 0 /*cyFolder*/) + 2) & ~1;
		
	m_cxExtent = 0; // not calculated yet...
}

void CTreeCtl::RecalcHorizontalExtent ()
{
	CClientDC dc(this);
	CFont* pOldFont;
	int nIndex;
	CNode *pNode;
	int nExtent;

	m_cxExtent = 0;

	// select the tree font in this DC
	pOldFont = dc.SelectObject(GetTreeFont());
	// find the longest string currently displayed in the list box
	for (nIndex = m_pListBox->GetCount() - 1; nIndex >= 0; nIndex--) {
		pNode = GetLBNode(nIndex);
		// get the extent of this string, plus the glyph, plus the level of indentation
		nExtent = dc.GetTextExtent(pNode->GetText(), pNode->GetText().GetLength()).cx + 
				(2 * (pNode->GetLevel() + 1)) * m_sizeM.cx +
				pNode->GetGlyphExtent(pNode->GetFlags() & TF_EXPANDED ? CNode::GT_Open : CNode::GT_Close).cx +
				c_cyBorderX2 * 5;
		if (nExtent > m_cxExtent) {
			m_cxExtent = nExtent;
		}
	}
	dc.SelectObject(pOldFont);

	// If the listbox fits, set the scroll position to 0 to make sure
	// the scroll bar goes away:
	CRect rcWindow;
	m_pListBox->GetClientRect(rcWindow);
	if (m_cxExtent <= rcWindow.right && m_pListBox->GetScrollPos(SB_HORZ))
		m_pListBox->SendMessage(WM_HSCROLL, SB_TOP);

	m_pListBox->SetHorizontalExtent(m_cxExtent);
}
	
void CTreeCtl::InvalItem(int nItem)
{
	CRect rect;
	m_pListBox->GetItemRect(nItem, rect);
	m_pListBox->InvalidateRect(rect);
}

void CTreeCtl::InvalSelection()
{
	if (m_bIsMultiSelection)
	{
		CRect client;
		m_pListBox->GetClientRect(client);
		for (int nItem = m_pListBox->GetTopIndex(); nItem != -1; nItem += 1)
		{
			CRect rect;
			if (m_pListBox->GetItemRect(nItem, &rect) == LB_ERR || rect.top >= client.bottom)
				break;
			
			if (m_pListBox->GetSel(nItem))
				m_pListBox->InvalidateRect(rect);
		}
	}
	else
	{
		InvalItem(GetCurSel());
	}
}

BOOL CTreeCtl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message != WM_KEYDOWN || GetParentFrame()->IsIconic())
		return FALSE;
		
	switch (pMsg->wParam)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_BACK:
	case VK_RETURN:
	case VK_ADD:
	case VK_SUBTRACT:
	case VK_MULTIPLY:
		// Make sure we get the keys we handle in OnKeyDown!
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}
	
	return FALSE;
}

CTreeCtl::HIT_ZONE CTreeCtl::HitTest(int nItem, int x)
{
	CNode* pNode = GetLBNode(nItem);
	ASSERT(pNode != NULL);
	
	CString strText;
	CRect client;
	GetClientRect(client);
	
	if (m_cxExtent > client.right)
		x += m_pListBox->GetScrollPos(SB_HORZ) * (m_cxExtent - client.right) / 100;
	
	int xLeft = c_cyBorderX2 + pNode->GetLevel() * m_sizeM.cx * 2;
	// if this node has children
	if (!(pNode->GetChildren()->IsEmpty()))
	{
		if (x < xLeft)
			return left;
		if (x < xLeft + 2 * m_sizeM.cx)
			return expand;
	}
	
	xLeft += 2 * m_sizeM.cx;
	if (x < xLeft)
		return left;
	
	xLeft += pNode->GetGlyphExtent(pNode->GetFlags() & TF_EXPANDED ? CNode::GT_Open : CNode::GT_Close).cx + c_cyBorderX2;
	if (x < xLeft)
		return glyph;
	
	CClientDC dc(this);
	dc.SelectObject(GetTreeFont());

	// Extra area between primary glyph and text is in the text zone...
//	xLeft += cxFolder - cxFolder;
//	xLeft += pNode->GetGlyphWidth() - cxFolder;
	
	strText = pNode->GetText();
	xLeft += dc.GetTextExtent(strText, strText.GetLength()).cx + 4 * c_cyBorderX2;
	if (x < xLeft)
		return text;
	
	return right;
}




/////////////////////////////////////////////////////////////////////////////
// CTreeCtl tree operations (internal)

BOOL CTreeCtl::ExpandOne(CNode* pNode)
{
	pNode->SetFlags(TF_EXPANDED);

	return TRUE;
}

BOOL CTreeCtl::SetSelOne(CNode* pNode)
{
	// mark the node as selected
	pNode->SetFlags(TF_SELECTED);

	return TRUE;
}

BOOL CTreeCtl::SetDeselOne(CNode* pNode)
{
	// mark the node as deselected
	pNode->ClearFlags(TF_SELECTED);

	return TRUE;
}

BOOL CTreeCtl::ToggleSelOne(CNode* pNode)
{
	// if it's selected, clear the selected flag
	if (pNode->GetFlags() & TF_SELECTED) {
		pNode->ClearFlags(TF_SELECTED);
	}
	else {
		pNode->SetFlags(TF_SELECTED);
	}
	return TRUE;
}

CNode* CTreeCtl::ForEach(CNode* pNode, BOOL (CTreeCtl::*pfn)(CNode *pNode), CTreeCtl::TraversalOrder order /*=TO_PREORDER*/)
{
	// perform a preorder traversal of the given node,
	// calling pfn for each node until pNode is NULL or pfn returns FALSE
	// return a pointer to the node where pfn returned FALSE

	CNode::CNodeList* plistChildren;
	CNode* pChild;
	CNode* pRetNode;
	POSITION pos;

	// is pNode NULL?
	if (pNode == NULL) {
		return NULL;
	}

	// if this is a preorder traversal, call pfn on the current node before child nodes
	if (order == TO_PREORDER) {
		if (!(this->*pfn)(pNode)) {
			return pNode;
		}
	}

	// get the list of children
	plistChildren = pNode->GetChildren();

	// call ForEach for each child
	for (pos = plistChildren->GetHeadPosition(); pos != NULL; ) {
		pChild = plistChildren->GetNext(pos);
		pRetNode = ForEach(pChild, pfn, order);
		if (pRetNode != NULL) {
			return pRetNode;
		}
	}

	// if this is a postorder traversal, call pfn on the current node after the child nodes
	if (order == TO_POSTORDER) {
		if (!(this->*pfn)(pNode)) {
			return pNode;
		}
	}

	return NULL;
}

BOOL CTreeCtl::Refresh(CNode *pNode)
{
	// is this node dirty?
	if (pNode->GetFlags() & TF_DIRTY_CONTENT)
	{
		// reset this node's dirty bit
		pNode->ClearFlags(TF_DIRTY_CONTENT);

		// is it expanded? then collapse it
		if (pNode->GetFlags() & TF_EXPANDED)
			Collapse(pNode);

		// is this node potentially expandable?
		if (!pNode->GetChildren()->IsEmpty()) {
			Expand(pNode, FALSE);
		}
	}

	if (pNode->GetFlags() & TF_DIRTY_ITEM) {
		// reset this node's dirty bit
		pNode->ClearFlags(TF_DIRTY_ITEM);
	}

	return TRUE;
}

void CTreeCtl::FreeAllTreeData()
{
	// delete all the nodes in the tree
	ForEach(m_pTree, &CTreeCtl::DeleteNode, TO_POSTORDER);

	m_pListBox->ResetContent();
}

void CTreeCtl::RefreshAll(void)
{
	// refresh each node in the tree
	ForEach(m_pTree, &CTreeCtl::Refresh);

	// clear and fill the list box
	m_pListBox->ResetContent();

	FillListBox(m_pTree);
}

void CTreeCtl::FillListBox(CNode* pNode)
{
	CNode::CNodeList* plistChildren;
	int nIndex;
	POSITION pos;

	if (!pNode) {
		return;
	}

	// is this node hidden?
	if (!(pNode->GetFlags() & TF_HIDDEN)) {
		// add this node's string to the list box
		nIndex = m_pListBox->AddString((LPSTR)pNode);
		// set its selection
		m_pListBox->SetSel(nIndex, pNode->GetFlags() & TF_SELECTED? TRUE: FALSE);
	}

	// is this node expanded
	if (pNode->GetFlags() & TF_EXPANDED) {
		// get the list of children
		plistChildren = pNode->GetChildren();

		ASSERT(!plistChildren->IsEmpty());

		// fill the list box for each child
		for (pos = plistChildren->GetHeadPosition(); pos != NULL; ) {
			FillListBox(plistChildren->GetNext(pos));
		}
	}
}

CFont* CTreeCtl::GetTreeFont(void)
{
	static CFont font;
	static LOGFONT lf;
	
	if (font.m_hObject == NULL) {
		lf.lfHeight = 14;
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfCharSet = ANSI_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FW_DONTCARE;
		lstrcpy(lf.lfFaceName, "Arial");

		font.CreateFontIndirect(&lf);
	}

	return &font;
}

void  CTreeCtl::HoldUpdates ()
{
	if (m_EnableUpdates == 0) m_pListBox->SetRedraw (FALSE);
	m_EnableUpdates++;
}

void  CTreeCtl::EnableUpdates ()
{
	m_EnableUpdates--;
	if (m_EnableUpdates == 0)
	{
		m_pListBox->SetRedraw (TRUE);
		m_pListBox->Invalidate();
	}

	ASSERT (m_EnableUpdates >= 0);
}

void CTreeCtl::BeginDragDrop()
{
	TRACE("Must override CTreeCtl::BeginDragDrop for drag/drop functionality!\n");
}

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl Node operations (internal)

int CTreeCtl::CompareData(const CNode* pNode1, const CNode* pNode2)
{
	if (pNode1 == pNode2)
		return 0;
	
	if (pNode1->GetData() < pNode2->GetData()) {
		return -1;
	}
	else {
		if (pNode1->GetData() > pNode2->GetData()) {
			return 1;
		}
		else {
			return 0;
		}
	}
}

int CTreeCtl::CompareNodes(const CNode* pNode1, const CNode* pNode2)
{
	if (pNode1 == pNode2) {
		return 0;
	}
	else {
		return 1;
	}
}

void CTreeCtl::DirtyNode(CNode *pNode, WORD tf)
{
	ASSERT(pNode != NULL);
	pNode->SetFlags(tf);

	Invalidate();
}

CNode* CTreeCtl::GetLBNode(int nIndex)
{
	// get the currently selected node if the given index is -1
	if (nIndex == -1 && (nIndex = GetCurSel()) == LB_ERR)
		return NULL;
	
	// we store a pointer to the node in the listbox
	CNode* pNode = (CNode*)m_pListBox->GetItemData(nIndex);
	if (pNode == (CNode*)LB_ERR)
		return NULL;
	
	return pNode;
}

BOOL CTreeCtl::DeleteNode(CNode* pNode)
{
	delete pNode;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl message handlers

int CTreeCtl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CreateListBox();

	CClientDC dc(this);
	ResetSizes(&dc);
	
	m_pListBox->m_pTree = this;
	if (!m_pListBox->Create(WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER |
		LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT |
		LBS_WANTKEYBOARDINPUT | ( (m_bIsMultiSelection) ? LBS_EXTENDEDSEL : 0) |
		(m_bStickySelection ? LBS_MULTIPLESEL : 0),
		CRect(0, 0, 0, 0), this, IDCW_TREELISTBOX))
	{
		return -1;
	}
	
	SetFont(GetTreeFont());
	
	return 0;

	UNREFERENCED_PARAMETER(lpCreateStruct);
}

void CTreeCtl::OnDestroy()
{
	FreeAllTreeData();

	delete m_pListBox;
}

void CTreeCtl::OnPaint()
{
	// we want to search through the tree control
	// for any dirty nodes, if we find one of these
	// we want to update its contents
	RefreshAll();		// refresh from our roots down

	RecalcHorizontalExtent();

	CWnd::OnPaint();	// pass on to our base instance
}

LPARAM CTreeCtl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	CFont* pFont = CFont::FromHandle((HFONT)wParam);

	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(pFont);
	ResetSizes(&dc);
	dc.SelectObject(pOldFont);

	m_pListBox->SetFont(pFont);
	m_pListBox->SetItemHeight(0, m_cyNode);
	
	return 0;

	UNREFERENCED_PARAMETER(lParam);
}

LPARAM CTreeCtl::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return m_pListBox->SendMessage(WM_GETFONT, wParam, lParam);
}

void CTreeCtl::OnSetFocus(CWnd* pOldWnd)
{
	m_pListBox->SetFocus();

	UNREFERENCED_PARAMETER(pOldWnd);
}

void CTreeCtl::OnSize(UINT nType, int cx, int cy)
{
	if (GetParent()->IsIconic())
		return;

	// Put the client area of the listbox in the client area of the tree.
	CRect rectNew, rectCurrent;
	GetClientRect(rectNew);
	::AdjustWindowRect(rectNew, m_pListBox->GetStyle(), FALSE);

	m_pListBox->GetWindowRect(rectCurrent);
	ScreenToClient(rectCurrent);

	if (rectNew == rectCurrent)
		return;
		
	m_pListBox->MoveWindow(rectNew.left, rectNew.top,
		rectNew.Width(), rectNew.Height());
		
	int iMax = GetCurSel();
	if (iMax >= 0)
	{
		int top, bottom;
			
		m_pListBox->GetClientRect(rectCurrent);
		top = m_pListBox->GetTopIndex();
		bottom = top + rectCurrent.bottom / m_cyNode;
		if (iMax < top || iMax > bottom)
			m_pListBox->SetTopIndex(iMax - (bottom - top) / 2);
	}
	RecalcHorizontalExtent();

	UNREFERENCED_PARAMETER(nType);
	UNREFERENCED_PARAMETER(cx);
	UNREFERENCED_PARAMETER(cy);
}

int CTreeCtl::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex)
{
	// On 6/15/94, I deleted a bunch of code in this function which appears
	// to be obsolete.  If the assertion below ever fires, please send an
	// exact repro case to me (ALANE) and I'll put it back.
	//
	ASSERT((GetCurSel() < 0) || (nChar <= ' '));
	return -2;

	UNREFERENCED_PARAMETER(pListBox);
	UNREFERENCED_PARAMETER(nIndex);
}

// OnKeyDown and OnChar
//
// These two functions are called as the listbox receives keyboard input,
// and give you a chance to add specific keyboard processing for your
// tree.
//
// Note that you will receive both an OnKeyDown AND an OnChar for character
// input.
//
// Return TRUE if you handle this message, FALSE if you want default listbox
// handling to occur.
//
BOOL CTreeCtl::OnKeyDown(UINT nChar)
{
	int i = GetCurSel();
	if (i < 0)
		return TRUE;

	int j = 1;
	CNode* pNode = GetLBNode(i);
	CNode* pNodeT;

	switch (nChar)
	{
	case VK_LEFT:
		while ((pNodeT = GetLBNode(--i)) != NULL)
		{
			if (pNodeT == pNode->GetParent())
			{
				KeySetSel(i);
				return TRUE;
			}
		}
		goto LSameSelection;
		
	case VK_RIGHT:
		if ((pNodeT = GetLBNode(i + 1)) == NULL || pNodeT->GetParent() != pNode)
		{
			goto LSameSelection;
		}
		KeySetSel(i + 1);
		return TRUE;
		
	case VK_UP:
		j = -1;
		// FALL THROUGH
		
	case VK_DOWN:
		if (GetKeyState(VK_CONTROL) >= 0)
		{
			ClearSearchBuffer();
			return FALSE;
		}
		
		while ((i += j) >= 0 && (pNodeT = GetLBNode(i)) != NULL)
		{
			if (pNodeT->GetParent() == pNode->GetParent())
			{
				KeySetSel(i);
				return TRUE;
			}
		}
LSameSelection:
		MessageBeep(0);
		return TRUE;
		
	case VK_BACK:
		{
			if (i <= 0)
				return TRUE;
			
			// go to the parent
			/*
			int nStartLevel = pNode->m_nLevels;
			do
			{
				pNodeT = GetNode(--i);
			}
			while (i > 0 && pNodeT->m_nLevels >= nStartLevel);
			*/
			
			KeySetSel(i);
			return TRUE;
		}
		
	case VK_RETURN:
		OnPrimary();
		return TRUE;

	case VK_ADD:
		OnExpand();
		return TRUE;
		
	case VK_SUBTRACT:
		OnCollapse();
		return TRUE;
		
	case VK_MULTIPLY:
		if (GetKeyState(VK_CONTROL) < 0)
			OnExpandAll();
		else
			OnExpandBranch();
		return TRUE;
	}

	return FALSE;
}

BOOL CTreeCtl::OnChar(UINT nChar)
{
	switch (nChar)
	{
	case '+':
		OnExpand();
		return TRUE;
		
	case '-':
		OnCollapse();
		return TRUE;
		
	case '*':
		OnExpandBranch();
		return TRUE;
	}
	
	// if enough time has elapsed, or the user hits a control key (e.g.
	// backspace) zero the string.
	time_t timeNow = time(NULL);
	
	// need a place to store the lead byte of a DBC
	static TCHAR cLead;

	if (iscntrl(nChar) || (timeNow - m_timeLastKey > LIST_SEARCH_DELAY))
		ClearSearchBuffer();

	// process the key
	if ((m_pListBox->GetCount() == 0) || iscntrl(nChar))
		return FALSE;

	if (cLead)
	{
		TCHAR szTmp[3];
		szTmp[0] = cLead;
		szTmp[1] = (TCHAR) nChar;
		szTmp[2] = _T('\0');
		
		cLead = 0;
		m_strSearch += CString( szTmp );
	}
	else
	{
		if (IsDBCSLeadByte( (char) nChar ))
		{
			cLead = (TCHAR) nChar;
			return TRUE;
		}

		m_strSearch += CString((char)nChar);
	}
	
	m_timeLastKey = timeNow;

	// look for a match
	CString str;
	int index, indexStart;
	index = indexStart = max(GetCurSel(), 0);

	do
	{
		str = GetLBNode(index)->GetText();
		if (m_strSearch.CompareNoCase(str.Left(m_strSearch.GetLength())) == 0)
		{
			// found a match
			KeySetSel(index, FALSE);
			return TRUE;
		}

		if (++index >= m_pListBox->GetCount())
			index = 0;
	} while (index != indexStart);

	// no match found, but we did handle the key
	return TRUE;
}

void CTreeCtl::ClearSearchBuffer()
{
	m_strSearch.Empty();
}



////////////////////////////////////////////////////////////////////////////
// Outline "command" handlers...

void CTreeCtl::OnPrimary()
{
	if (m_pListBox->GetCaretIndex() != -1)
		OnOpen(m_pListBox->GetCaretIndex());
	ClearSearchBuffer();
}

void CTreeCtl::OnExpand()
{
	// get the node that's currently selected in the list box
	CNode* pNode = GetLBNode();
	// if there's a node currently selected, it has children, and it's not expanded, expand it
	if (pNode != NULL && (!(pNode->GetChildren()->IsEmpty())) && !(pNode->GetFlags() & TF_EXPANDED))
		Expand(pNode);
	ClearSearchBuffer();
}

void CTreeCtl::OnCollapse()
{
	// get the node that's currently selected in the list box
	CNode* pNode = GetLBNode();
	int nItem = m_pListBox->GetCurSel();
	
	// if the node doesn't have children
	if (!(pNode->GetChildren()->IsEmpty()))
	{
		if (nItem <= 0)
			return;

		// collapse the parent node
		pNode = pNode->GetParent();
	}
	
	// if the node is valid and expanded, collapse it
	if (pNode != NULL && (pNode->GetFlags() & TF_EXPANDED)) {
		Collapse(pNode);
	}
		
	ClearSearchBuffer();
}

void CTreeCtl::OnExpandBranch()
{
	// get the currently selected node
	CNode *pNode = GetLBNode();

	// if the node is valid, expand it and all its children
	if (pNode) {
		Expand(pNode, TRUE);
	}
	ClearSearchBuffer();
}

void CTreeCtl::OnExpandAll()
{
	// UNDONE: these cases could be consolidated
//	SetCurSel(0);	// work-around for #2909
	ExpandAll();
	ClearSearchBuffer();
}

void CTreeCtl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_cyNode;

	UNREFERENCED_PARAMETER(nIDCtl);
}

void CTreeCtl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	UINT idGlyph;
	CSize sizeGlyph;
	CNode::GlyphType glyphNode;
	CString strText;
	
	// [patbr] tree control faults with empty list box
	if (lpDrawItemStruct->itemData == NULL || lpDrawItemStruct->itemData == (UINT)-1)
	{
		// list box is empty, so indicate that list box has focus.
		// DrawFocusRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem));
		return;								 
	}

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CNode* pNode = (CNode*)lpDrawItemStruct->itemData;
	CFont* pOldFont = pDC->SelectObject(GetTreeFont());

	strText = pNode->GetText();
	int cchText = strText.GetLength();
	
	CSize sizeText = pDC->GetTextExtent(strText, cchText);

	// our drawing starting point for this node from left and top
	CPoint	pt(c_cyBorderX2 + pNode->GetLevel() * m_sizeM.cx * 2, lpDrawItemStruct->rcItem.top);

	// our focus/highlight rectangle for the text
	CRect rect;
	rect.top = lpDrawItemStruct->rcItem.top;
	rect.left = pt.x + 2 * m_sizeM.cx + pNode->GetGlyphExtent(pNode->GetFlags() & TF_EXPANDED ? CNode::GT_Open : CNode::GT_Close).cx + c_cyBorderX2;
	rect.bottom = lpDrawItemStruct->rcItem.bottom;
	rect.right = rect.left + sizeText.cx + 4 * c_cyBorderX2;
	
	if (lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
	{
		int x = pt.x + m_sizeM.cx;	// initial drawing starting position
		x &= ~1;	// Make even;
		int y = pt.y + m_cyNode / 2; 
		int yp = y | 1; 

		BOOL bDrawSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0 || (pNode->GetFlags() & TF_DROPTARGET) != 0;
		BOOL bHasFocus = ::GetFocus() == lpDrawItemStruct->hwndItem || (pNode->GetFlags() & TF_DROPTARGET) != 0;
		COLORREF rgbText, rgbBackground;

		// Draw the dotted lines...
		
		CBrush* pOldBrush = pDC->SelectObject(CDC::GetHalftoneBrush());

		// if this node has a parent and children,
		if (pNode->GetParent() != NULL || (!pNode->GetChildren()->IsEmpty()) != 0)
		{
			// Draw the horizontal line over the (possible) folder
			// Draw on an odd lines so our hatch brush looks nice.
			pDC->PatBlt (x, yp, m_sizeM.cx, 1, NOTPATCOPY);
		}
		
		if (pNode->GetParent() != NULL)
		{
			// Draw the top part of the vertical line
			pDC->PatBlt (x, rect.top, 1, m_cyNode / 2, NOTPATCOPY); 
			
			// If not the end of a node, draw the bottom part
			if ((pNode->GetFlags() & TF_LASTLEVELENTRY) == 0) {
				pDC->PatBlt (x, y + c_cyBorder, 1, m_cyNode / 2, NOTPATCOPY); 
			}

			// Draw the verticals on the left connecting other nodes
			int nLevel = pNode->GetLevel();
			for (CNode* pNodeT = pNode->GetParent(); pNodeT != NULL; pNodeT = pNodeT->GetParent())
			{
				nLevel -= 1;

				if (!(pNodeT->GetFlags() & TF_LASTLEVELENTRY) && (pNodeT->GetParent() != NULL))
				{
					int	x = (nLevel + 1) * m_sizeM.cx * 2 + c_cyBorderX2 - m_sizeM.cx;
					x &= ~1;	// Make even;
					pDC->PatBlt(x, rect.top, 1, m_cyNode, NOTPATCOPY); 
				}
			}
		}
		
		pDC->SelectObject(pOldBrush);
		
		
		// Draw the boxed plus/minus sign if appropriate...

		// if this node has childeren,
		if (!(pNode->GetChildren()->IsEmpty()))
		{
			// calculate the id of the glyph
			idGlyph = pNode->GetFlags() & TF_EXPANDED ? IDB_GlyphExpanded : IDB_GlyphCollapsed;

			// get the size of the glyph
			sizeGlyph = m_Glyphs.GetBitmapExtent(idGlyph);

			// Draw the box	[+] or [-]
			m_Glyphs.Draw(idGlyph, pDC, CPoint(x - sizeGlyph.cx / 2, yp - sizeGlyph.cy / 2));
		}

		// Draw the glyph...
		
		glyphNode = pNode->GetFlags() & TF_EXPANDED ? CNode::GT_Open : CNode::GT_Close;
		pNode->DrawGlyph(glyphNode, pDC, CPoint(x + m_sizeM.cx + c_cyBorder, y - (pNode->GetGlyphExtent(glyphNode).cy / 2)));
		
		// Draw the text...
		
		if ((bHasFocus || m_bKeepSelectionRect) && bDrawSelected)
		{
			rgbText = pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			rgbBackground = pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}
		
		pDC->ExtTextOut(rect.right - sizeText.cx - c_cyBorderX2 * 2, y - (m_sizeM.cy / 2), ETO_OPAQUE, &rect, strText, cchText, NULL);
		
		// draw the selection...
		if (bDrawSelected)
		{
			if (bHasFocus || m_bKeepSelectionRect)
			{
				pDC->SetTextColor(rgbText);
				pDC->SetBkColor(rgbBackground);
			}
			else
			{
				// Highlight is a frame when we don't have the focus...
				CBrush brush;
				if (brush.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)))
					pDC->FrameRect(&rect, &brush);
			}
		}
	}
	
	if (lpDrawItemStruct->itemAction == ODA_FOCUS)
		pDC->DrawFocusRect(&rect);
	
	pDC->SelectObject(pOldFont);

	UNREFERENCED_PARAMETER(nIDCtl);
}

void CTreeCtl::OnRightButtonUp(UINT, CPoint)
{
	// do nothing .. derived class implementation is more useful
}

void CTreeCtl::OnRightButtonDown(UINT, CPoint)
{
	// do nothing .. derived class implementation is more useful
}

void CTreeCtl::OnLbDblClk()
{	
/*
	// CHICAGO: Move property activation to rbutton.
	if (m_bShowProperties && (GetKeyState(VK_MENU) & ~1) != 0)
	{
		ActivatePropertyBrowser();
		return;
	}

	OnPrimary();
*/
}

void CTreeCtl::OnLbSelChange()
{
	// a line in the list box has been clicked
	OnSelect(GetCurSel());
}

void CTreeCtl::OnOpen(int nIndex)
{
	CNode* pNode = GetLBNode(nIndex);
	if (pNode != NULL) {
		if (pNode->GetFlags() & TF_EXPANDED) {
			Collapse(pNode);
		}
		else {
			if (!(pNode->GetChildren()->IsEmpty())) {
				Expand(pNode);
			}
		}
		Invalidate();
	}
}

void CTreeCtl::OnSelect(int nIndex)
{
	CNode* pNode;

	// mark the node's selection flag appropriately

	// get the node
	pNode = GetLBNode(nIndex);

	// REVIEW(briancr): this doesn't unselect the previous
	// selection in a nonsticky selection list box

	// is the node selected
	if (GetSel(nIndex)) {
		pNode->SetFlags(TF_SELECTED);
	}
	else {
		pNode->ClearFlags(TF_SELECTED);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListBox -- the actual list box inside a tree control

int CTreeListBox::iSlobPixels = 5;
CPoint CTreeListBox::mouseDownPt;

CTreeListBox::CTreeListBox()
{
	m_pTree = NULL;
    m_bGotMouseDown = FALSE;
    m_bDeferedDeselection = FALSE;
	m_bStdMouse = TRUE;
}

int CTreeListBox::ItemFromPoint(const CPoint& point)
{
    CRect client;
    GetClientRect(&client);

    if (!client.PtInRect(point))
        return LB_ERR;

    CRect itemRect;
    GetItemRect(0, &itemRect);

	int nItem = (GetTopIndex() + (point.y / itemRect.Height()));
	return nItem < GetCount() ? nItem : LB_ERR;
}

void CTreeListBox::OnLButtonDown(UINT nFlags, CPoint pt)
{
	BOOL bPassOn = TRUE;

	SetFocus();
	SetCapture();

	m_bStdMouse = TRUE;
	
	int nItem = ItemFromPoint(pt);
	if (nItem != LB_ERR)
	{
		int nCurSel = GetCurSel();
		int nCount = GetCount();

		CTreeCtl::HIT_ZONE hitZone = m_pTree->HitTest(nItem, pt.x);
		
		if (hitZone == CTreeCtl::expand)
		{
			// Inside the expand/collapse box...
		
			m_pTree->OnOpen(nItem);

			if (nItem < nCurSel)
			{
				if (GetCount() > nCount)
				{
					// did expand, change selection accordingly
					SetCurSel(nCurSel + GetCount() - nCount);
				}
				else if (GetCount() < nCount)
				{
					// did collapse, change selection accordingly
					if (nCurSel > (nItem + (nCount - GetCount())))
					{
						// previous selection not in collapsed area, reset selection
						SetCurSel(nCurSel + GetCount() - nCount);
					}
					else
					{
						// previous selection in collapsed area, set selection to collapsed node
						SetCurSel(nItem);
					}
				}
			}

			// don't change selection unless we just cleared in
			// by collapsing the tree...
			if (GetSelCount() != 0)
				return;
		}
	
		if (m_pTree->m_bEnableDragDrop && 
			(hitZone == CTreeCtl::glyph || hitZone == CTreeCtl::text))
		{
			// Inside glyph or text and D&D is enabled...
		
			m_bStdMouse = FALSE;
		    m_bGotMouseDown = TRUE;
		    mouseDownPt = pt;

		    if ((nFlags & MK_SHIFT) == 0 && GetSel(nItem))
			{
		        // don't deselect yet, wait for either a mouse up or a drag and drop
		        m_bDeferedDeselection = TRUE;
		        m_nDeferedItem = nItem;
				return;
			}
			else
			{
				if ((nFlags & MK_SHIFT) != 0)
				{
					// Shift held down - so select a range, but do not pass on.
					int nCurrentSel;

					SetSel(-1, FALSE);
					nCurrentSel = GetCaretIndex();
					if (nCurrentSel > nItem)
						SelItemRange(TRUE, nItem, nCurrentSel);
					else
						SelItemRange(TRUE, nCurrentSel, nItem);
 				}
				else
				{
 					// Only clear selection if CONTROL key not pressed
					if ((nFlags & MK_CONTROL) == 0)
						SetSel(-1, FALSE);
			    	SetSel(nItem, TRUE);
				}
				bPassOn = FALSE;
			}
		}
	}

	if (bPassOn)
		CListBox::OnLButtonDown(nFlags, pt);	// pass on

	if (m_pTree->m_bEnableDragDrop)
	{
#ifdef _WIN32
	    GetParent()->SendMessage(WM_COMMAND, 
			MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
	    GetParent()->SendMessage(WM_COMMAND, (WPARAM) GetDlgCtrlID(),
	        MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
	}
}

void CTreeListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nItem = ItemFromPoint(point);
	if (nItem != LB_ERR)
	{
		switch (m_pTree->HitTest(nItem, point.x))
		{
		case CTreeCtl::expand:
			// Treat as single click...
			m_pTree->OnOpen(nItem);
			return;
			
		case CTreeCtl::left:
		case CTreeCtl::right:
			// Ignore the double click...
			return;
		
		case CTreeCtl::glyph:
		case CTreeCtl::text:
			break;
		}
	}

	CListBox::OnLButtonDblClk(nFlags, point);
}

void CTreeListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bStdMouse || !m_pTree->m_bEnableDragDrop)
	{
		CListBox::OnMouseMove(nFlags, point);
		return;
	}

    // Nasty hack here because Windows sends a continuous stream of
    // WM_MOUSEMOVE messages when a button is held down...
    static CPoint NEAR lastPoint;
    if ((point == lastPoint) || (GetCount() == 0))
        return;

    lastPoint = point;

    CPoint offsetPoint (abs(mouseDownPt.x - point.x),
                        abs(mouseDownPt.y - point.y));

    if (m_bGotMouseDown && offsetPoint.x + offsetPoint.y > iSlobPixels && GetSelCount() > 0)
    {
		// Begin a drag...
		m_pTree->BeginDragDrop();

        m_bGotMouseDown = FALSE;
        m_bDeferedDeselection = FALSE;
    }
}

void CTreeListBox::OnLButtonUp(UINT nFlags, CPoint pt)
{
	if (!m_bStdMouse && m_pTree->m_bEnableDragDrop)
	{
		m_bGotMouseDown = FALSE;

		if (m_bDeferedDeselection)
		{
		    if (nFlags & MK_CONTROL)
		    {
		        SetSel(m_nDeferedItem, FALSE);
		    }
		    else
		    {
		        SetSel(-1, FALSE);
		        SetSel(m_nDeferedItem, TRUE);
		    }
		    m_bDeferedDeselection = FALSE;
#ifdef _WIN32
	        GetParent()->SendMessage(WM_COMMAND, 
				MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
	        GetParent()->SendMessage(WM_COMMAND, (WPARAM) GetDlgCtrlID(),
	            MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
		}
    }

    // need to call Windows so notifications get sent
    CListBox::OnLButtonUp(nFlags, pt);
	ReleaseCapture();
}

void CTreeListBox::OnRButtonUp(UINT nFlags, CPoint pt)
{
	CListBox::OnRButtonUp(nFlags, pt);	// pass on

	CWnd *	pwnd = GetParent();

	ASSERT(pwnd->IsKindOf(RUNTIME_CLASS(CTreeCtl)));

	// inform the tree control of this
	((CTreeCtl *)pwnd)->OnRightButtonUp(nFlags, pt);
}

void CTreeListBox::OnRButtonDown(UINT nFlags, CPoint pt)
{
/*
	SetFocus();

	CSlobWnd* pWnd = GetActiveSlobWnd();
	if (!(m_pTree->m_bShowContextPopup && pWnd->IsChild(this)))
		return;
	
    int nItem = ItemFromPoint(pt);
	if (nItem != LB_ERR)
	{
		if (!GetSel(nItem))
		{
	        if (SetSel(-1, FALSE) == LB_ERR || SetSel(nItem, TRUE) == LB_ERR)
				SetCurSel(nItem);
		}
		else
		{
			SetCaretIndex(nItem);
		}

	    GetParent()->SendMessage(WM_COMMAND, 
			MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());

		UpdateWindow();	// So we don't blow save bits.
	}

	ClientToScreen(&pt);
	pWnd->ShowContextPopupMenu(pt);
*/

	DBG_UNREFERENCED_PARAMETER(nFlags);
	DBG_UNREFERENCED_PARAMETER(pt);
}

void CTreeListBox::OnKeyDown(UINT nChar, UINT nRepCount, UINT nFlags)
{
	if (m_pTree->OnKeyDown(nChar))
		return;

	CListBox::OnKeyDown(nChar, nRepCount, nFlags);
}

void CTreeListBox::OnChar(UINT nChar, UINT nRepCount, UINT nFlags)
{
	if (m_pTree->OnChar(nChar))
		return;

	CListBox::OnChar(nChar, nRepCount, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CNode

CImageWell CNode::m_Glyphs;

CNode::CNode(CString strText /*= ""*/, DWORD dwData /*= 0*/, CNode *pParent /*= NULL*/, UINT idGlyphClose /*= IDB_GlyphFolderClose*/, UINT idGlyphOpen /*= IDB_GlyphFolderOpen*/)
: m_strText(strText),
  m_dwData(dwData),
  m_pParent(pParent),
  m_nFlags(0),
  m_idGlyphClose(idGlyphClose),
  m_idGlyphOpen(idGlyphOpen)
{
	ASSERT(m_listChildren.IsEmpty());

	// add the glyphs
	m_Glyphs.Add(m_idGlyphClose);
	m_Glyphs.Add(m_idGlyphOpen);

  // this is to ensure that both test glyps are available

	m_Glyphs.Add(IDB_GlyphTest);
	m_Glyphs.Add(IDB_GlyphBadTest);
}

CNode::~CNode()
{
	// REVIEW(briancr): remove all items from the children list
	// I think FreeAllTreeData() does this
	m_Glyphs.Remove(m_idGlyphClose);
	m_Glyphs.Remove(m_idGlyphOpen);
	m_Glyphs.Remove(IDB_GlyphTest);
	m_Glyphs.Remove(IDB_GlyphBadTest);
}

/////////////////////////////////////////////////////////////////////////////
// CImageWell

#define DSx		0x00660046L
#define DSna	0x00220326L

CImageWell::~CImageWell()
{
	// delete all entries in the map
}

BOOL CImageWell::Add(UINT idImage)
{
	ImageType image;
	CBitmap* pbitmap;
	CBitmap* pmask;
	HINSTANCE hInst;
	HINSTANCE hOldInst;

	// if it's already in the map, bump the reference count
	if (m_mapImages.Lookup(idImage, image)) {
		image.m_count++;
		m_mapImages.SetAt(idImage, image);
		return TRUE;
	}
	// otherwise, load the bitmap, calculate the mask, and store it
	else {
		pbitmap = new CBitmap;
		pmask = new CBitmap;

		// set the instance handle to the current instance
		hInst = AfxFindResourceHandle(MAKEINTRESOURCE(idImage), RT_BITMAP); 
		hOldInst = AfxGetResourceHandle();

		AfxSetResourceHandle(hInst);

		// load the bitmap
		if (!pbitmap->LoadBitmap(idImage)) {
			TRACE("CImageWell::Add: could not load image (id=%d)\n", idImage);
			return FALSE;
		}
		
		// reset the instance handle
		AfxSetResourceHandle(hOldInst);
	
		// calculate the mask
		CalculateMask(pbitmap, pmask);

		// insert the bitmap and mask into the map
		image.m_pbitmap = pbitmap;
		image.m_pmask = pmask;
		image.m_count = 1;
		m_mapImages.SetAt(idImage, image);

		return TRUE;
	}
}

BOOL CImageWell::Remove(UINT idImage)
{
	ImageType image;

	// the image should be in the map
	ASSERT(m_mapImages.Lookup(idImage, image));

	if (m_mapImages.Lookup(idImage, image)) {
		// decrement the reference count
		image.m_count--;
		m_mapImages.SetAt(idImage, image);
		// if the reference count < 1 then delete the bitmaps
		if (image.m_count <= 0) {
			m_mapImages.RemoveKey(idImage);
			delete image.m_pbitmap;
			delete image.m_pmask;
		}
		return TRUE;
	}
	else {
		TRACE("CImageWell::Remove: no image found (id=%d)\n", idImage);
		return FALSE;
	}
}

CSize CImageWell::GetBitmapExtent(UINT idImage)
{
	ImageType image;
	BITMAP bm;
	CSize size;

	// the image should be in the map
	ASSERT(m_mapImages.Lookup(idImage, image));

	if (m_mapImages.Lookup(idImage, image)) {
		// get information on this bitmap
		image.m_pbitmap->GetObject(sizeof(bm), &bm);
		// fill in the size
		size.cx = bm.bmWidth;
		size.cy = bm.bmHeight;
	}
	else {
		TRACE("CImageWell::GetBitmapExtent: no image found (id=%d)\n", idImage);
	}
	return size;
}

BOOL CImageWell::Draw(UINT idImage, CDC *pDC, CPoint pt, DWORD rop /*= 0*/)
{
	ImageType image;
	BITMAP bmp;
	CDC dc;
	CDC maskdc;
	COLORREF oldBkColor;
	COLORREF oldTextColor;

	// the image should be in the map
	ASSERT(m_mapImages.Lookup(idImage, image));

	if (m_mapImages.Lookup(idImage, image)) {

		// get the size of the glyph
		image.m_pbitmap->GetObject(sizeof(bmp), &bmp);

		// create DCs for the bitmap and mask
		dc.CreateCompatibleDC(pDC);
		maskdc.CreateCompatibleDC(pDC);

		// set the bitmap and mask in their respective DCs
		dc.SelectObject(image.m_pbitmap);
		maskdc.SelectObject(image.m_pmask);

		// is there a user-supplied raster operation (rop)?
		if (rop == 0) {		
			// set colors	
			oldBkColor = pDC->SetBkColor(RGB(255, 255, 255));
			oldTextColor = pDC->SetTextColor(RGB(0, 0, 0));

			// draw the glyph
			pDC->BitBlt(pt.x, pt.y, bmp.bmWidth, bmp.bmHeight, &dc, 0, 0, DSx);
			pDC->BitBlt(pt.x, pt.y, bmp.bmWidth, bmp.bmHeight, &maskdc, 0, 0, DSna);
			pDC->BitBlt(pt.x, pt.y, bmp.bmWidth, bmp.bmHeight, &dc, 0, 0, DSx);

			// reset colors
			pDC->SetBkColor(oldBkColor);
			pDC->SetTextColor(oldTextColor);

			return TRUE;
		}
		else {
			pDC->BitBlt(pt.x, pt.y, bmp.bmWidth, bmp.bmHeight, &dc, 0, 0, rop);
			return TRUE;
		}
	}
	else {
		TRACE("CImageWell::Draw: no image found (id=%d)\n", idImage);
		return FALSE;
	}
}

BOOL CImageWell::CalculateMask(CBitmap* pbitmap, CBitmap* pmask)
{
	CDC dc;
	CDC maskdc;
	BITMAP bmp;

	// get the size of the bitmap
	pbitmap->GetObject(sizeof(bmp), &bmp);
	
	// create DCs for the bitmap and mask
	dc.CreateCompatibleDC(NULL);
	maskdc.CreateCompatibleDC(NULL);

	// create the mask's bitmap
	if (!pmask->CreateBitmap(bmp.bmWidth, bmp.bmHeight, 1, 1, NULL)) {
		TRACE("CImageWell::CalculateMask: unable to create mask\n");
		return FALSE;
	}

	// select the bitmaps in the DCs
	VERIFY(dc.SelectObject(pbitmap) != NULL);
	VERIFY(maskdc.SelectObject(pmask) != NULL);

	dc.SetBkColor(dc.GetPixel(0, 0));
	maskdc.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dc, 0, 0, NOTSRCCOPY);

	return TRUE;
}

