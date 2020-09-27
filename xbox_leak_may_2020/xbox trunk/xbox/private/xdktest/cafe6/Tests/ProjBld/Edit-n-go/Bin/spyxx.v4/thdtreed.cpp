// thdtreed.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "spytreed.h"
#include "thdtreed.h"
#include "treectl.h"
#include "spytreec.h"
#include "thdnode.h"
#include "findtool.h"
#include "spytreev.h"
#include "srchdlgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThdTreeDoc

IMPLEMENT_DYNCREATE(CThdTreeDoc, CSpyTreeDoc);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CThdTreeDoc::CThdTreeDoc()
{
	m_dwThread = (DWORD)-1;
	m_strModule.Empty();
	m_fSearchUp = FALSE;
}



CThdTreeDoc::~CThdTreeDoc()
{
}



void CThdTreeDoc::InitializeData()
{
	CMapPtrToPtr ThdMap;

	if (!theApp.IsChicago())
	{
		// Win32
//		enum CPROCDB::PROCDBERR error;
		WORD wQueryType = PROCDBQ_THREAD;
		CPROCDB ProcessDatabase(wQueryType/*, &error*/);
		CPROCDB::PRFCNTRTHRD PrfCntrThrd;
		int iCount = 0, iLoop = ProcessDatabase.GetNumThreads();

		for (; iCount < iLoop; iCount++)
		{
			memset((void *)&PrfCntrThrd, 0, sizeof(CPROCDB::PRFCNTRTHRD));
			ProcessDatabase.GetPrfCntrThrd(iCount, &PrfCntrThrd);

			CThdTreeNode* pThdTreeNode = new CThdTreeNode;
			pThdTreeNode->Create(&ProcessDatabase, iCount);
			m_TopLevelNodeList.AddTail(pThdTreeNode);
			ThdMap.SetAt((void*)pThdTreeNode->GetObjectHandle(), pThdTreeNode);
		}
	}
	else
	{
		// Chicago
		HANDLE hThreadList;
		THREADENTRY32 te32;

		te32.dwSize = sizeof(THREADENTRY32);

		if ((hThreadList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPTHREAD, 0)) != NULL)
		{
			HANDLE hProcessList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0);
			PROCESSENTRY32 pe32;
			char *szExePath;

			if ((*theApp.pfnThread32First)(hThreadList, &te32))
			{
				CThdTreeNode* pThdTreeNode = new CThdTreeNode;

				pe32.dwSize = sizeof(PROCESSENTRY32);
				if (((*theApp.pfnProcess32First)(hProcessList, &pe32)) && (pe32.dwSize >= sizeof(PROCESSENTRY32)))
				{
					if (pe32.th32ProcessID == te32.th32OwnerProcessID)
					{
						szExePath = pe32.szExeFile;
					}
					else
					{
						while ((*theApp.pfnProcess32Next)(hProcessList, &pe32))
						{
							if (pe32.th32ProcessID == te32.th32OwnerProcessID)
							{
								szExePath = pe32.szExeFile;
								break;
							}
						}
					}
				}
				else
					szExePath = "";

				pThdTreeNode->Create(hThreadList, te32.th32ThreadID, szExePath);
				m_TopLevelNodeList.AddTail(pThdTreeNode);
				ThdMap.SetAt((void*)pThdTreeNode->GetObjectHandle(), pThdTreeNode);

				while ((*theApp.pfnThread32Next)(hThreadList, &te32))
				{
					CThdTreeNode* pThdTreeNode = new CThdTreeNode;

					if (((*theApp.pfnProcess32First)(hProcessList, &pe32)) && (pe32.dwSize >= sizeof(PROCESSENTRY32)))
					{
						if (pe32.th32ProcessID == te32.th32OwnerProcessID)
						{
							szExePath = pe32.szExeFile;
						}
						else
						{
							while ((*theApp.pfnProcess32Next)(hProcessList, &pe32))
							{
								if (pe32.th32ProcessID == te32.th32OwnerProcessID)
								{
									szExePath = pe32.szExeFile;
									break;
								}
							}
						}
					}
					else
						szExePath = "";

					pThdTreeNode->Create(hThreadList, te32.th32ThreadID, szExePath);
					m_TopLevelNodeList.AddTail(pThdTreeNode);
					ThdMap.SetAt((void*)pThdTreeNode->GetObjectHandle(), pThdTreeNode);
				}
			}

			CloseHandle(hThreadList);
		}
	}

	CThdTreeNode::AddChildren(&ThdMap);
}

void CThdTreeDoc::SetSearchMenuText(CCmdUI *pCmdUI)
{
	pCmdUI->SetText(ids(IDS_MENU_FINDTHREAD));
}

BOOL CThdTreeDoc::DoFind()
{
#ifdef USE_TABBED_SEARCH_DIALOGS
	CSearchTabbedDialog dlgTest(IDS_SEARCHCAPTION, NULL, (UINT)-1);

	dlgTest.AddTab(new CSearchThreadDlgTab(IDD_SEARCHTHREADTAB, IDS_SEARCHTHREADTAB));
	dlgTest.AddTab(new CSearchWindowDlgTab(IDD_SEARCHWINDOWTAB, IDS_SEARCHWINDOWTAB));

	dlgTest.DoModal();
#else	// USE_TABBED_SEARCH_DIALOGS
	CSearchThreadDlg dlg;
	CSpyTreeView *pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
	CThdTreeNode *pcttn = (CThdTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(pcstvActive->m_SpyTreeCtl.GetCurSel())->m_dwData;

	m_dwThread = pcttn->GetObjectHandle();
	m_strModule = pcttn->GetModule();

	dlg.SetSelectedThread(m_dwThread);
	dlg.SetModule(m_strModule);
	dlg.SetDirection(m_fSearchUp);

	if (dlg.DoModal() == TRUE)
	{
		m_wSearchFlags	= dlg.GetSearchFlags();
		m_dwThread		= dlg.GetSelectedThread();
		m_strModule		= dlg.GetModule();
		m_fSearchUp		= dlg.GetDirection();

		if (m_fSearchUp)
		{
			if (!FindPreviousMatchingNode() && !FindLastMatchingNode())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_THD);
			}
		}
		else
		{
			if (!FindNextMatchingNode() && !FindFirstMatchingNode())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_THD);
			}
		}
	}
#endif	// USE_TABBED_SEARCH_DIALOGS

	return(TRUE);
}

BOOL CThdTreeDoc::DoFindNext()
{
	if (!FindNextMatchingNode() && !FindFirstMatchingNode())
	{
		// notify user that no next matching node found
		SpyMessageBox(IDS_NO_NEXT_MATCH_THD);
	}

	return(TRUE);
}

BOOL CThdTreeDoc::DoFindPrev()
{
	if (!FindPreviousMatchingNode() && !FindLastMatchingNode())
	{
		// notify user that no previous matching node found
		SpyMessageBox(IDS_NO_PREV_MATCH_THD);
	}

	return(TRUE);
}

BOOL CThdTreeDoc::HasValidFind()
{
	if (m_wSearchFlags & SRCHFLAG_THREAD_USETHREAD)
		return(FALSE);
	else if (m_wSearchFlags & SRCHFLAG_THREAD_USEMODULE)
		return(!m_strModule.IsEmpty());
	else
		return(FALSE);
}

BOOL CThdTreeDoc::FindFirstMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CThdTreeNode *pcttn;

	for (int i = 0; i < pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount(); i++)
	{
		pcttn = (CThdTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcttn->IsKindOf(RUNTIME_CLASS(CThdTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_THREAD_USETHREAD:

				if (pcttn->GetObjectHandle() == m_dwThread)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_THREAD_USEMODULE:

				if (pcttn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BOOL CThdTreeDoc::FindNextMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CThdTreeNode *pcttn;

	for (int i = pcstvActive->m_SpyTreeCtl.GetCurSel() + 1; i < pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount(); i++)
	{
		pcttn = (CThdTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcttn->IsKindOf(RUNTIME_CLASS(CThdTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_THREAD_USETHREAD:

				if (pcttn->GetObjectHandle() == m_dwThread)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_THREAD_USEMODULE:

				if (pcttn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BOOL CThdTreeDoc::FindPreviousMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CThdTreeNode *pcttn;

	for (int i = pcstvActive->m_SpyTreeCtl.GetCurSel() - 1; i >= 0; i--)
	{
		pcttn = (CThdTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcttn->IsKindOf(RUNTIME_CLASS(CThdTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_THREAD_USETHREAD:

				if (pcttn->GetObjectHandle() == m_dwThread)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_THREAD_USEMODULE:

				if (pcttn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BOOL CThdTreeDoc::FindLastMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CThdTreeNode *pcttn;

	for (int i = pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount() - 1; i >= 0; i--)
	{
		pcttn = (CThdTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcttn->IsKindOf(RUNTIME_CLASS(CThdTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_THREAD_USETHREAD:

				if (pcttn->GetObjectHandle() == m_dwThread)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_THREAD_USEMODULE:

				if (pcttn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BEGIN_MESSAGE_MAP(CThdTreeDoc, CSpyTreeDoc)
	//{{AFX_MSG_MAP(CThdTreeDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

