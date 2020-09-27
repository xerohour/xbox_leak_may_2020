// spytreed.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "spytreed.h"
#include "spytreec.h"
#include "imgwell.h"
#include "spytreev.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpyTreeDoc

IMPLEMENT_DYNCREATE(CSpyTreeDoc, CDocument);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSpyTreeDoc::CSpyTreeDoc()
{
}



CSpyTreeDoc::~CSpyTreeDoc()
{
}



BOOL CSpyTreeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	InitializeData();

	return TRUE;
}



void CSpyTreeDoc::DeleteContents()
{
	POSITION pos;
	CSpyTreeNode* pSpyTreeNode;

	pos = m_TopLevelNodeList.GetHeadPosition();
	while (pos)
	{
		pSpyTreeNode = (CSpyTreeNode*)m_TopLevelNodeList.GetNext(pos);
		delete pSpyTreeNode;
	}

	m_TopLevelNodeList.RemoveAll();
}



BEGIN_MESSAGE_MAP(CSpyTreeDoc, CDocument)
	//{{AFX_MSG_MAP(CSpyTreeDoc)
	ON_COMMAND(ID_WINDOW_REFRESH, OnWindowRefresh)
	ON_COMMAND(ID_TREE_EXPAND_ONE_LEVEL, OnTreeExpandOneLevel)
	ON_UPDATE_COMMAND_UI(ID_TREE_EXPAND_ONE_LEVEL, OnUpdateTreeExpandOneLevel)
	ON_COMMAND(ID_TREE_EXPAND_BRANCH, OnTreeExpandBranch)
	ON_UPDATE_COMMAND_UI(ID_TREE_EXPAND_BRANCH, OnUpdateTreeExpandBranch)
	ON_COMMAND(ID_TREE_EXPAND_ALL, OnTreeExpandAll)
	ON_UPDATE_COMMAND_UI(ID_TREE_EXPAND_ALL, OnUpdateTreeExpandAll)
	ON_COMMAND(ID_TREE_COLLAPSE, OnTreeCollapse)
	ON_UPDATE_COMMAND_UI(ID_TREE_COLLAPSE, OnUpdateTreeCollapse)
	ON_COMMAND(ID_SEARCH_FIND, OnSearchFind)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FIND, OnUpdateSearchFind)
	ON_COMMAND(ID_SEARCH_FINDNEXT, OnSearchFindNext)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FINDNEXT, OnUpdateSearchFindNext)
	ON_COMMAND(ID_SEARCH_FINDPREV, OnSearchFindPrev)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FINDPREV, OnUpdateSearchFindPrev)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSpyTreeDoc commands

void CSpyTreeDoc::OnWindowRefresh()
{
	DeleteContents();
	InitializeData();
	UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnTreeExpandOneLevel()
{
	POSITION pos;
	CSpyTreeView *pcstvCur, *pcstvActive;

	BeginWaitCursor();
	pos = GetFirstViewPosition();
	while (pos)
	{
		pcstvCur = (CSpyTreeView *)GetNextView(pos);
		pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
		if (pcstvCur == pcstvActive)
		{
			pcstvCur->m_SpyTreeCtl.Expand(pcstvCur->m_SpyTreeCtl.GetCurSel(), FALSE);
			break;
		}
	}
	EndWaitCursor();
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnUpdateTreeExpandOneLevel(CCmdUI* pCmdUI)
{
	POSITION pos;
	CSpyTreeView *pcstvCur, *pcstvActive;
	
	pos = GetFirstViewPosition();
	while (pos)
	{
		pcstvCur = (CSpyTreeView *)GetNextView(pos);
		pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
		if (pcstvCur == pcstvActive)
		{
			BOOL bEnable;

			bEnable = pcstvCur->m_SpyTreeCtl.NodeIsExpandable(pcstvCur->m_SpyTreeCtl.GetNode(pcstvCur->m_SpyTreeCtl.GetCurSel()));
			bEnable &= !(pcstvCur->m_SpyTreeCtl.IsExpanded(pcstvCur->m_SpyTreeCtl.GetCurSel()));
			pCmdUI->Enable(bEnable);
			break;
		}
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnTreeExpandBranch()
{
	POSITION pos;
	CSpyTreeView *pcstvCur, *pcstvActive;
	
	BeginWaitCursor();
	pos = GetFirstViewPosition();
	while (pos)
	{
		pcstvCur = (CSpyTreeView *)GetNextView(pos);
		pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
		if (pcstvCur == pcstvActive)
		{
			pcstvCur->m_SpyTreeCtl.Expand(pcstvCur->m_SpyTreeCtl.GetCurSel(), TRUE);
			break;
		}
	}
	EndWaitCursor();
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnUpdateTreeExpandBranch(CCmdUI* pCmdUI)
{
	POSITION pos;
	CSpyTreeView *pcstvCur, *pcstvActive;
	
	pos = GetFirstViewPosition();
	while (pos)
	{
		pcstvCur = (CSpyTreeView *)GetNextView(pos);
		pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
		if (pcstvCur == pcstvActive)
		{
			pCmdUI->Enable(pcstvCur->m_SpyTreeCtl.NodeIsExpandable(pcstvCur->m_SpyTreeCtl.GetNode(pcstvCur->m_SpyTreeCtl.GetCurSel())));
			break;
		}
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnTreeExpandAll()
{
	POSITION pos;
	CSpyTreeView *pcstvCur, *pcstvActive;
	
	BeginWaitCursor();
	pos = GetFirstViewPosition();
	while (pos)
	{
		pcstvCur = (CSpyTreeView *)GetNextView(pos);
		pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
		if (pcstvCur == pcstvActive)
		{
			pcstvCur->m_SpyTreeCtl.ExpandAll();
			break;
		}
	}
	EndWaitCursor();
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnUpdateTreeExpandAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnTreeCollapse()
{
	POSITION pos;
	CSpyTreeView *pcstvCur, *pcstvActive;
	
	BeginWaitCursor();
	pos = GetFirstViewPosition();
	while (pos)
	{
		pcstvCur = (CSpyTreeView *)GetNextView(pos);
		pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
		if (pcstvCur == pcstvActive)
		{
			pcstvCur->m_SpyTreeCtl.Collapse(pcstvCur->m_SpyTreeCtl.GetCurSel());
			break;
		}
	}
	EndWaitCursor();
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnUpdateTreeCollapse(CCmdUI* pCmdUI)
{
	POSITION pos;
	CSpyTreeView *pcstvCur, *pcstvActive;
	
	pos = GetFirstViewPosition();
	while (pos)
	{
		pcstvCur = (CSpyTreeView *)GetNextView(pos);
		pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
		if (pcstvCur == pcstvActive)
		{
			pCmdUI->Enable(pcstvCur->m_SpyTreeCtl.IsExpanded(pcstvCur->m_SpyTreeCtl.GetCurSel()));
			break;
		}
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnSearchFind()
{
	DoFind();
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnUpdateSearchFind(CCmdUI* pCmdUI)
{
	SetSearchMenuText(pCmdUI);	// virtual--sets text to Find Window, Find Process, etc.
	pCmdUI->Enable(TRUE);
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnSearchFindNext()
{
	DoFindNext();
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnUpdateSearchFindNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(HasValidFind());
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnSearchFindPrev()
{
	DoFindPrev();
	return;
}

/////////////////////////////////////////////////////////////////////////////

void CSpyTreeDoc::OnUpdateSearchFindPrev(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(HasValidFind());
	return;
}

/////////////////////////////////////////////////////////////////////////////
