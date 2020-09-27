// spytreev.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "spytreec.h"
#include "spytreev.h"
#include "spytreed.h"
#include "fontdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpyTreeView

IMPLEMENT_DYNCREATE(CSpyTreeView, CView)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSpyTreeView::CSpyTreeView()
{
}

CSpyTreeView::~CSpyTreeView()
{
}


BEGIN_MESSAGE_MAP(CSpyTreeView, CView)
	//{{AFX_MSG_MAP(CSpyTreeView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_FONT, OnViewFont)
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewProperties)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpyTreeView drawing

void CSpyTreeView::OnPaint()
{
	// do not call CView::OnPaint since it will call OnDraw
	CWnd::OnPaint();
}

void CSpyTreeView::OnDraw(CDC*)
{
	// do nothing here
}

/////////////////////////////////////////////////////////////////////////////
// CSpyTreeView message handlers

void CSpyTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CSpyTreeNode* pSpyTreeNode;
	CString str;
	POSITION pos;
	CSpyTreeDoc* pDoc;

	BeginWaitCursor();

	m_SpyTreeCtl.FreeAllTreeData();

	pDoc = (CSpyTreeDoc*)GetDocument();

	m_SpyTreeCtl.SetSorting(pDoc->SortLines());

	pos = pDoc->m_TopLevelNodeList.GetHeadPosition();
	while (pos)
	{
		pSpyTreeNode = (CSpyTreeNode*)pDoc->m_TopLevelNodeList.GetNext(pos);
		pSpyTreeNode->GetDescription(str);
		m_SpyTreeCtl.InsertNode(-1, str, (DWORD)pSpyTreeNode);
	}

	m_SpyTreeCtl.SetCurSel(0);

	if (pDoc->ExpandFirstLine())
	{
		m_SpyTreeCtl.Expand();
	}

	m_SpyTreeCtl.RecalcHorizontalExtent();

	EndWaitCursor();
}

int CSpyTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	LONG gwl = GetWindowLong(this->m_hWnd, GWL_STYLE);
	gwl |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	SetWindowLong(this->m_hWnd, GWL_STYLE, gwl);

	if (m_SpyTreeCtl.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CRect(0, 0, 0, 0), this, 0) == -1)
	{
		return -1;
	}

	CFont Font;
	Font.CreateFontIndirect(&theApp.m_DefLogFont);
	SetFont(&Font, FALSE);
	Font.Detach();

	return 0;
}

void CSpyTreeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_SpyTreeCtl.MoveWindow(CRect(0, 0, cx, cy));
}

void CSpyTreeView::OnViewFont()
{
	HFONT hfontNew;

	if (hfontNew = SpyFontDialog(m_hfont))
	{
		HFONT hfontOld = m_hfont;

		SetFont(CFont::FromHandle(hfontNew));
		::DeleteObject(hfontOld);
	}
}

void CSpyTreeView::OnViewProperties()
{
	m_SpyTreeCtl.ShowSelItemProperties();
}

void CSpyTreeView::OnUpdateViewProperties(CCmdUI* pCmdUI)
{
	//
	// Enable only if there is a selected item in the tree.
	//
	pCmdUI->Enable(m_SpyTreeCtl.GetCurSel() != LB_ERR);
}

void CSpyTreeView::OnSetFocus(CWnd* pOldWnd)
{
	m_SpyTreeCtl.SetFocus();
}
