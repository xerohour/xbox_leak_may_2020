// wndtreed.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "imgwell.h"
#include "findtool.h"
#include "spytreed.h"
#include "spytreec.h"
#include "wndnode.h"
#include "wndtreed.h"
#include "spytreev.h"
#include "srchdlgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndTreeDoc

IMPLEMENT_DYNCREATE(CWndTreeDoc, CSpyTreeDoc);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWndTreeDoc::CWndTreeDoc()
{
	m_hwndWnd = NULL;
	m_strCaption.Empty();
	m_strClass.Empty();
	m_fSearchUp = FALSE;
}



CWndTreeDoc::~CWndTreeDoc()
{
}



void CWndTreeDoc::InitializeData()
{
	//
	// Create the one and only top level node based on the windows
	// desktop window.  The create of this node will automatically
	// populate all the children entries, creating nodes for them
	// as well.
	//
	CWndTreeNode* pWndTreeNode = new CWndTreeNode;
	pWndTreeNode->Create(::GetDesktopWindow(), TRUE);
	m_TopLevelNodeList.AddTail(pWndTreeNode);
}

void CWndTreeDoc::SetSearchMenuText(CCmdUI *pCmdUI)
{
	pCmdUI->SetText(ids(IDS_MENU_FINDWINDOW));
}

BOOL CWndTreeDoc::DoFind()
{
#ifdef USE_TABBED_SEARCH_DIALOGS
	CSearchTabbedDialog dlgTest(IDS_SEARCHCAPTION, NULL, (UINT)-1);

	dlgTest.AddTab(new CSearchWindowDlgTab(IDD_SEARCHWINDOWTAB, IDS_SEARCHWINDOWTAB));

	dlgTest.DoModal();
#else	// USE_TABBED_SEARCH_DIALOGS
	CSearchWindowDlg dlg;
	CSpyTreeView *pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
	CWndTreeNode *pcwtn = (CWndTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(pcstvActive->m_SpyTreeCtl.GetCurSel())->m_dwData;

	m_hwndWnd = (HWND)pcwtn->GetObjectHandle();
	m_strCaption = pcwtn->GetCaption();
	m_strClass = pcwtn->GetClass();

	dlg.SetWindow(m_hwndWnd);
	dlg.SetCaption(m_strCaption);     
	dlg.SetClass(m_strClass);       
	dlg.SetDirection(m_fSearchUp);

	if (dlg.DoModal() == TRUE)
	{
		m_wSearchFlags	= dlg.GetSearchFlags();
		m_hwndWnd		= dlg.GetSelectedHwnd();
		m_strCaption	= dlg.GetCaption();
		m_strClass		= dlg.GetClass();
		m_fSearchUp		= dlg.GetDirection();

		if (m_fSearchUp)
		{
			if (!FindPreviousMatchingNode() && !FindLastMatchingNode())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_WND);
			}
		}
		else
		{
			if (!FindNextMatchingNode() && !FindFirstMatchingNode())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_WND);
			}
		}
	}
#endif	// USE_TABBED_SEARCH_DIALOGS

	return(TRUE);
}

BOOL CWndTreeDoc::DoFindNext()
{
	if (!FindNextMatchingNode() && !FindFirstMatchingNode())
	{
		// notify user that no next matching node found
		SpyMessageBox(IDS_NO_NEXT_MATCH_WND);
	}

	return(TRUE);
}

BOOL CWndTreeDoc::DoFindPrev()
{
	if (!FindPreviousMatchingNode() && !FindLastMatchingNode())
	{
		// notify user that no previous matching node found
		SpyMessageBox(IDS_NO_PREV_MATCH_WND);
	}

	return(TRUE);
}

BOOL CWndTreeDoc::HasValidFind()
{
	if (m_wSearchFlags & SRCHFLAG_WINDOW_USEHANDLE)
		return(FALSE);
	else if (m_wSearchFlags & SRCHFLAG_WINDOW_USECAPTION)
		return(!m_strCaption.IsEmpty());
	else if (m_wSearchFlags & SRCHFLAG_WINDOW_USECLASS)
		return(!m_strClass.IsEmpty());
	else
		return(FALSE);
}

BOOL CWndTreeDoc::CompareNode(CWndTreeNode *pcwtn)
{
	// do comparison of node contents with search criteria
	switch (m_wSearchFlags)
	{
		case SRCHFLAG_WINDOW_USEHANDLE:

			if (pcwtn->GetObjectHandle() == (DWORD)m_hwndWnd)
				return(TRUE);
			break;

		case SRCHFLAG_WINDOW_USECAPTION:

			if (pcwtn->GetCaption() == m_strCaption)
				return(TRUE);
			break;

		case SRCHFLAG_WINDOW_USECLASS:

			if (pcwtn->GetClass() == m_strClass)
				return(TRUE);
			break;

		case (SRCHFLAG_WINDOW_USECAPTION | SRCHFLAG_WINDOW_USECLASS):

			if ((pcwtn->GetCaption() == m_strCaption) && (pcwtn->GetClass() == m_strClass))
				return(TRUE);
			break;
	}

	return(FALSE);
}

BOOL CWndTreeDoc::FindNodeRecurse(CWndTreeNode *pcwtn, CObList &listExpand, BOOL fSearchingUp)
{
	// do comparison
	if (CompareNode(pcwtn))
	{
		if (fSearchingUp)
			listExpand.RemoveAll();
		listExpand.AddTail(pcwtn);
		if (!fSearchingUp)
			return(TRUE);
	}

	POSITION pos = pcwtn->GetFirstChildPosition();
	while (pos)
	{
		CWndTreeNode *pcwtnLoop = (CWndTreeNode*)pcwtn->GetNextChild(pos);

		if (FindNodeRecurse(pcwtnLoop, listExpand, fSearchingUp))
		{
			listExpand.AddTail(pcwtn);
			if (!fSearchingUp)
				return(TRUE);
		}
	}

	if (fSearchingUp)
		return(!listExpand.IsEmpty());

	return(FALSE);
	
}

BOOL CWndTreeDoc::FindFirstMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CWndTreeNode *pcwtn;
	CObList listExpand;

	for (int i = 0; i < pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount(); i++)
	{
		pcwtn = (CWndTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (FindNodeRecurse(pcwtn, listExpand))
		{
			CWndTreeNode *pcwtnLoop;
			int iLoop = 0;
			BOOL fHeld = FALSE;

			POSITION pos = listExpand.GetTailPosition();
			while (pos)
			{
				iLoop++;
				pcwtnLoop = (CWndTreeNode*)listExpand.GetPrev(pos);

				if (iLoop != listExpand.GetCount())
				{
					if (!(pcstvActive->m_SpyTreeCtl.IsExpanded(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop))))
					{
						if (!fHeld)
						{
							pcstvActive->m_SpyTreeCtl.HoldUpdates();
							fHeld = TRUE;
						}
						pcstvActive->m_SpyTreeCtl.Expand(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
					}
				}
			}
			listExpand.RemoveAll();

			if (fHeld)
				pcstvActive->m_SpyTreeCtl.EnableUpdates();

			pcstvActive->m_SpyTreeCtl.SetCurSel(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
			return(TRUE);
		}
	}

	return(FALSE);
}

BOOL CWndTreeDoc::FindNextMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CWndTreeNode *pcwtn;
	CObList listExpand;

	for (int i = pcstvActive->m_SpyTreeCtl.GetCurSel() + 1; i < pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount(); i++)
	{
		pcwtn = (CWndTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (FindNodeRecurse(pcwtn, listExpand))
		{
			CWndTreeNode *pcwtnLoop;
			int iLoop = 0;
			BOOL fHeld = FALSE;

			POSITION pos = listExpand.GetTailPosition();
			while (pos)
			{
				iLoop++;
				pcwtnLoop = (CWndTreeNode*)listExpand.GetPrev(pos);

				if (iLoop != listExpand.GetCount())
				{
					if (!(pcstvActive->m_SpyTreeCtl.IsExpanded(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop))))
					{
						if (!fHeld)
						{
							pcstvActive->m_SpyTreeCtl.HoldUpdates();
							fHeld = TRUE;
						}
						pcstvActive->m_SpyTreeCtl.Expand(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
					}
				}
			}
			listExpand.RemoveAll();

			if (fHeld)
				pcstvActive->m_SpyTreeCtl.EnableUpdates();

			pcstvActive->m_SpyTreeCtl.SetCurSel(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
			return(TRUE);
		}
	}

	return(FALSE);
}

BOOL CWndTreeDoc::FindPreviousMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CWndTreeNode *pcwtn;
	CObList listExpand;

	for (int i = pcstvActive->m_SpyTreeCtl.GetCurSel() - 1; i >= 0; i--)
	{
		pcwtn = (CWndTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (pcstvActive->m_SpyTreeCtl.GetNode(i) == pcstvActive->m_SpyTreeCtl.GetNode(i + 1)->m_pParent)
		{
			if (CompareNode(pcwtn))
			{
				pcstvActive->m_SpyTreeCtl.SetCurSel(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtn));
				return(TRUE);
			}
		}
		else
		{
			if (FindNodeRecurse(pcwtn, listExpand, TRUE))
			{
				CWndTreeNode *pcwtnLoop;
				int iLoop = 0;
				BOOL fHeld = FALSE;

				POSITION pos = listExpand.GetTailPosition();
				while (pos)
				{
					iLoop++;
					pcwtnLoop = (CWndTreeNode*)listExpand.GetPrev(pos);

					if (iLoop != listExpand.GetCount())
					{
						if (!(pcstvActive->m_SpyTreeCtl.IsExpanded(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop))))
						{
							if (!fHeld)
							{
								pcstvActive->m_SpyTreeCtl.HoldUpdates();
								fHeld = TRUE;
							}
							pcstvActive->m_SpyTreeCtl.Expand(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
						}
					}
				}
				listExpand.RemoveAll();

				if (fHeld)
					pcstvActive->m_SpyTreeCtl.EnableUpdates();

				pcstvActive->m_SpyTreeCtl.SetCurSel(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
				return(TRUE);
			}
		}
	}

	return(FALSE);
}

BOOL CWndTreeDoc::FindLastMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CWndTreeNode *pcwtn;
	CObList listExpand;

	for (int i = pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount() - 1; i >= 0; i--)
	{
		pcwtn = (CWndTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if ((i < pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount() - 1) &&
			(pcstvActive->m_SpyTreeCtl.GetNode(i) == pcstvActive->m_SpyTreeCtl.GetNode(i + 1)->m_pParent))
		{
			if (CompareNode(pcwtn))
			{
				pcstvActive->m_SpyTreeCtl.SetCurSel(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtn));
				return(TRUE);
			}
		}
		else
		{
			if (FindNodeRecurse(pcwtn, listExpand, TRUE))
			{
				CWndTreeNode *pcwtnLoop;
				int iLoop = 0;
				BOOL fHeld = FALSE;

				POSITION pos = listExpand.GetTailPosition();
				while (pos)
				{
					iLoop++;
					pcwtnLoop = (CWndTreeNode*)listExpand.GetPrev(pos);

					if (iLoop != listExpand.GetCount())
					{
						if (!(pcstvActive->m_SpyTreeCtl.IsExpanded(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop))))
						{
							if (!fHeld)
							{
								pcstvActive->m_SpyTreeCtl.HoldUpdates();
								fHeld = TRUE;
							}
							pcstvActive->m_SpyTreeCtl.Expand(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
						}
					}
				}
				listExpand.RemoveAll();

				if (fHeld)
					pcstvActive->m_SpyTreeCtl.EnableUpdates();

				pcstvActive->m_SpyTreeCtl.SetCurSel(pcstvActive->m_SpyTreeCtl.FindNode((DWORD)pcwtnLoop));
				return(TRUE);
			}
		}
	}

	return(FALSE);
}

BEGIN_MESSAGE_MAP(CWndTreeDoc, CSpyTreeDoc)
	//{{AFX_MSG_MAP(CWndTreeDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
