// spytreec.c : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "imgwell.h"
#include "spytreec.h"
#include "imgwell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CImageWell CSpyTreeCtl::c_imageSpyWell;

/////////////////////////////////////////////////////////////////////////////
// CSpyTreeCtl

CSpyTreeCtl::CSpyTreeCtl()
{
	if (!c_imageSpyWell.IsOpen())
	{
		VERIFY(c_imageSpyWell.Load(IDB_FOLDERS, CSize(16, 16)));
		VERIFY(c_imageSpyWell.Open());
		VERIFY(c_imageSpyWell.CalculateMask());
	}
}

CSpyTreeCtl::~CSpyTreeCtl()
{
}


BEGIN_MESSAGE_MAP(CSpyTreeCtl, CTreeCtl)
	//{{AFX_MSG_MAP(CSpyTreeCtl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDCW_TREELISTBOX, OnLbDblClk)
	ON_LBN_SETFOCUS(IDCW_TREELISTBOX, OnLbSetFocus)
	ON_LBN_KILLFOCUS(IDCW_TREELISTBOX, OnLbKillFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSpyTreeCtl message handlers


int CSpyTreeCtl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// load in our glyph bitmaps and inform the tree control of these
	if (!m_bmFolders.LoadBitmap(IDB_FOLDERS))
		return -1;  // failed to create our bitmap

	if (!m_bmExpandNodes.LoadBitmap(IDB_EXPNODES))
	{
		m_bmFolders.DeleteObject(); // failed to create our bitmap
		return -1;
	}

	if (CTreeCtl::OnCreate(lpCreateStruct) == -1)
	{
		m_bmFolders.DeleteObject();
		m_bmExpandNodes.DeleteObject();
		return -1;  // failed to create our tree control
	}

	return 0;
}

void CSpyTreeCtl::OnDestroy()
{
	CTreeCtl::OnDestroy();

	// destroy our glyph bitmaps
	m_bmFolders.DeleteObject();
	m_bmExpandNodes.DeleteObject();
}



BOOL
CSpyTreeCtl::InsertLevel(CNode* pParentNode, int nParentIndex, BOOL bExpandAll)
{
	POSITION pos;
	CSpyTreeNode* pSpyTreeNodeParent;
	CSpyTreeNode* pSpyTreeNodeChild;
	CString str;

	pSpyTreeNodeParent = (CSpyTreeNode*)pParentNode->m_dwData;

	pos = pSpyTreeNodeParent->GetFirstChildPosition();
	while (pos)
	{
		CNode* pNode;
		int iNode;

		pSpyTreeNodeChild = (CSpyTreeNode*)pSpyTreeNodeParent->GetNextChild(pos);
		pSpyTreeNodeChild->GetDescription(str);
		if (!InsertNode(pParentNode, nParentIndex, str, (DWORD)pSpyTreeNodeChild, pNode, iNode))
		{
			TRACE("CSpyTreeCtl::InsertLevel: call to InsertNode failed!\n");
			return FALSE;
		}

		if (bExpandAll && !InsertLevel(pNode, iNode, TRUE))
		{
			return FALSE;
		}
	}

	return TRUE;
}



void CSpyTreeCtl::DrawNodeGlyph(CDC* pDC, CNode* pNode, BOOL /*bHighlight*/, CPoint pt)
{
	CSpyTreeNode* pSpyTreeNode = (CSpyTreeNode*)pNode->m_dwData;
//	m_iwFolders.DrawImage(pDC, pt, pSpyTreeNode->GetBitmapIndex(), 0);
	c_imageSpyWell.DrawImage(pDC, pt, pSpyTreeNode->GetBitmapIndex(), 0);
}



BOOL CSpyTreeCtl::NodeIsExpandable(CNode* pNode)
{
	CSpyTreeNode* pSpyTreeNode;

	if (pNode)
	{
		pSpyTreeNode = (CSpyTreeNode*)pNode->m_dwData;

		return pSpyTreeNode->GetFirstChildPosition() ? TRUE : FALSE;
	}
	return FALSE;
}



//void CSpyTreeCtl::GetNodeTitle(CNode*, CString &)
//{
//	ASSERT(FALSE); //Never should get here if I don't dirty something.
//}



void CSpyTreeCtl::ShowSelItemProperties()
{
	//
	// Get the node for the currently selected item.
	//
	CSpyTreeNode* pSpyTreeNode = (CSpyTreeNode*)GetData();

	if (pSpyTreeNode)
	{
		pSpyTreeNode->ShowProperties();
	}
}



void CSpyTreeCtl::OnLbDblClk()
{
	ShowSelItemProperties();
}



void CSpyTreeCtl::OnLbSetFocus()
{
	//
	// Get the node for the currently selected item.
	//
	CSpyTreeNode* pSpyTreeNode = (CSpyTreeNode*)GetData();

	if (pSpyTreeNode)
	{
		SetLastSelectedObject(pSpyTreeNode->GetObjectHandle(), pSpyTreeNode->GetObjectType());
	}

	CTreeCtl::InvalSelection();
}



void CSpyTreeCtl::OnLbKillFocus()
{
	SetLastSelectedObject(0, OT_NONE);

	CTreeCtl::InvalSelection();
}



void CSpyTreeCtl::OnSelect(int nIndex)
{
	//
	// Get the node for the currently selected item.
	//
	CSpyTreeNode* pSpyTreeNode = (CSpyTreeNode*)GetData();

	if (pSpyTreeNode)
	{
		SetLastSelectedObject(pSpyTreeNode->GetObjectHandle(), pSpyTreeNode->GetObjectType());
	}
}



void CSpyTreeCtl::OnRightButtonDown(UINT nFlags, CPoint pt)
{
	CMDIChildWnd *pParent = (CMDIChildWnd *)(GetParent()->GetParent());
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

	((CMDIFrameWnd *)(theApp.m_pMainWnd))->MDIActivate(pParent);
	
	SetActiveWindow();
	SetFocus();

	int index = ItemFromPoint(pt);

	if (index != -1)
	{
		SetCurSel(index);
		OnSelect(index);

		CString str;
		CMenu MenuPopup;
		if (!MenuPopup.CreatePopupMenu())
		{
			return;
		}

		if (GetLastSelectedObjectType() != OT_WINDOW || IsSpyableWindow((HWND)GetLastSelectedObject()))
		{
			str.LoadString(IDS_MESSAGESMENU);
			MenuPopup.AppendMenu(MF_STRING, ID_SPY_MESSAGESDIRECT, str);
		}

		if (GetLastSelectedObjectType() == OT_WINDOW && ::IsWindowVisible((HWND)GetLastSelectedObject()))
		{
			str.LoadString(IDS_HIGHLIGHTWINDOW);
			MenuPopup.AppendMenu(MF_STRING, ID_SPY_HIGHLIGHTWINDOW, str);
		}

		str.LoadString(IDS_PROPERTIES);
		MenuPopup.AppendMenu(MF_STRING, ID_VIEW_PROPERTIES, str);

		ClientToScreen(&pt);
		MenuPopup.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, theApp.m_pMainWnd);
	}
}



/////////////////////////////////////////////////////////////////////////////
// CSpyTreeNode

IMPLEMENT_DYNCREATE(CSpyTreeNode, CObject);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSpyTreeNode::~CSpyTreeNode()
{
	POSITION pos;
	CSpyTreeNode* pTreeNode;

	pos = GetFirstChildPosition();
	while (pos)
	{
		pTreeNode = GetNextChild(pos);
		delete pTreeNode;
	}
}



//
// CSpyTreeNode static data and functions
//

int CSpyTreeNode::m_cUsage = 0;
