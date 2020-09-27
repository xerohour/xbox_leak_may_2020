// prctreed.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "spytreed.h"
#include "prctreed.h"
#include "treectl.h"
#include "imgwell.h"
#include "spytreec.h"
#include "prcnode.h"
#include "thdnode.h"
#include "findtool.h"
#include "spytreev.h"
#include "srchdlgs.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrcTreeDoc

IMPLEMENT_DYNCREATE(CPrcTreeDoc, CSpyTreeDoc);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPrcTreeDoc::CPrcTreeDoc()
{
	m_dwProcess = (DWORD)-1;
	m_strModule.Empty();
	m_fSearchUp = FALSE;
}



CPrcTreeDoc::~CPrcTreeDoc()
{
}



void CPrcTreeDoc::InitializeData()
{
	CMapPtrToPtr ThdMap;

	if (!theApp.IsChicago())
	{
		// Win32
//		enum CPROCDB::PROCDBERR error;
		WORD wQueryType = (PROCDBQ_PROCESS | PROCDBQ_THREAD);
		CPROCDB ProcessDatabase(wQueryType/*, &error*/);
		int iCount = 0, iLoop = ProcessDatabase.GetNumProcesses();

		for (; iCount < iLoop; iCount++)
		{
			CPrcTreeNode* pPrcTreeNode = new CPrcTreeNode;
			pPrcTreeNode->Create(&ProcessDatabase, iCount, &ThdMap);
			m_TopLevelNodeList.AddTail(pPrcTreeNode);
		}
	}
	else
	{
		// Chicago
		HANDLE hProcessList;
		PROCESSENTRY32 pe32;

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if ((hProcessList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0)) != NULL)
		{
			if ((*theApp.pfnProcess32First)(hProcessList, &pe32))
			{
				CPrcTreeNode* pPrcTreeNode = new CPrcTreeNode;
				if (pe32.dwSize >= sizeof(PROCESSENTRY32))
					pPrcTreeNode->Create(hProcessList, pe32.th32ProcessID, pe32.szExeFile, &ThdMap);
				else
					pPrcTreeNode->Create(hProcessList, pe32.th32ProcessID, "", &ThdMap);
				m_TopLevelNodeList.AddTail(pPrcTreeNode);

				while ((*theApp.pfnProcess32Next)(hProcessList, &pe32))
				{
					CPrcTreeNode* pPrcTreeNode = new CPrcTreeNode;
					if (pe32.dwSize >= sizeof(PROCESSENTRY32))
						pPrcTreeNode->Create(hProcessList, pe32.th32ProcessID, pe32.szExeFile, &ThdMap);
					else
						pPrcTreeNode->Create(hProcessList, pe32.th32ProcessID, "", &ThdMap);
					m_TopLevelNodeList.AddTail(pPrcTreeNode);
				}
			}

			CloseHandle(hProcessList);
		}
	}

	CThdTreeNode::AddChildren(&ThdMap);
}

void CPrcTreeDoc::SetSearchMenuText(CCmdUI *pCmdUI)
{
	pCmdUI->SetText(ids(IDS_MENU_FINDPROCESS));
}

BOOL CPrcTreeDoc::DoFind()
{
#ifdef USE_TABBED_SEARCH_DIALOGS
	CSearchTabbedDialog dlgTest(IDS_SEARCHCAPTION, NULL, (UINT)-1);

	dlgTest.AddTab(new CSearchProcessDlgTab(IDD_SEARCHPROCESSTAB, IDS_SEARCHPROCESSTAB));
	dlgTest.AddTab(new CSearchThreadDlgTab(IDD_SEARCHTHREADTAB, IDS_SEARCHTHREADTAB));
	dlgTest.AddTab(new CSearchWindowDlgTab(IDD_SEARCHWINDOWTAB, IDS_SEARCHWINDOWTAB));

	dlgTest.DoModal();
#else	// USE_TABBED_SEARCH_DIALOGS
	CSearchProcessDlg dlg;
	CSpyTreeView *pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
	CPrcTreeNode *pcptn = (CPrcTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(pcstvActive->m_SpyTreeCtl.GetCurSel())->m_dwData;

	m_dwProcess = pcptn->GetObjectHandle();
	m_strModule = pcptn->GetModule();

	dlg.SetSelectedProcess(m_dwProcess);
	dlg.SetModule(m_strModule);
	dlg.SetDirection(m_fSearchUp);

	if (dlg.DoModal() == TRUE)
	{
		m_wSearchFlags	= dlg.GetSearchFlags();
		m_dwProcess		= dlg.GetSelectedProcess();
		m_strModule		= dlg.GetModule();
		m_fSearchUp		= dlg.GetDirection();

		if (m_fSearchUp)
		{
			if (!FindPreviousMatchingNode() && !FindLastMatchingNode())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_PRC);
			}
		}
		else
		{
			if (!FindNextMatchingNode() && !FindFirstMatchingNode())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_PRC);
			}
		}
	}
#endif	// USE_TABBED_SEARCH_DIALOGS

	return(TRUE);
}

BOOL CPrcTreeDoc::DoFindNext()
{
	if (!FindNextMatchingNode() && !FindFirstMatchingNode())
	{
		// notify user that no next matching node found
		SpyMessageBox(IDS_NO_NEXT_MATCH_PRC);
	}

	return(TRUE);
}

BOOL CPrcTreeDoc::DoFindPrev()
{
	if (!FindPreviousMatchingNode() && !FindLastMatchingNode())
	{
		// notify user that no previous matching node found
		SpyMessageBox(IDS_NO_PREV_MATCH_PRC);
	}

	return(TRUE);
}

BOOL CPrcTreeDoc::HasValidFind()
{
	if (m_wSearchFlags & SRCHFLAG_PROCESS_USEPROCESS)
		return(FALSE);
	else if (m_wSearchFlags & SRCHFLAG_PROCESS_USEMODULE)
		return(!m_strModule.IsEmpty());
	else
		return(FALSE);
}

BOOL CPrcTreeDoc::FindFirstMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CPrcTreeNode *pcptn;

	for (int i = 0; i < pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount(); i++)
	{
		pcptn = (CPrcTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcptn->IsKindOf(RUNTIME_CLASS(CPrcTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_PROCESS_USEPROCESS:

				if (pcptn->GetObjectHandle() == m_dwProcess)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_PROCESS_USEMODULE:

				if (pcptn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BOOL CPrcTreeDoc::FindNextMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CPrcTreeNode *pcptn;

	for (int i = pcstvActive->m_SpyTreeCtl.GetCurSel() + 1; i < pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount(); i++)
	{
		pcptn = (CPrcTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcptn->IsKindOf(RUNTIME_CLASS(CPrcTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_PROCESS_USEPROCESS:

				if (pcptn->GetObjectHandle() == m_dwProcess)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_PROCESS_USEMODULE:

				if (pcptn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BOOL CPrcTreeDoc::FindPreviousMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CPrcTreeNode *pcptn;

	for (int i = pcstvActive->m_SpyTreeCtl.GetCurSel() - 1; i >= 0; i--)
	{
		pcptn = (CPrcTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcptn->IsKindOf(RUNTIME_CLASS(CPrcTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_PROCESS_USEPROCESS:

				if (pcptn->GetObjectHandle() == m_dwProcess)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_PROCESS_USEMODULE:

				if (pcptn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BOOL CPrcTreeDoc::FindLastMatchingNode()
{
	CSpyTreeView *pcstvActive;
	
	pcstvActive = (CSpyTreeView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	CPrcTreeNode *pcptn;

	for (int i = pcstvActive->m_SpyTreeCtl.GetListBox()->GetCount() - 1; i >= 0; i--)
	{
		pcptn = (CPrcTreeNode *)pcstvActive->m_SpyTreeCtl.GetNode(i)->m_dwData;

		if (!(pcptn->IsKindOf(RUNTIME_CLASS(CPrcTreeNode))))
			continue;

		// do comparison of node contents with search criteria
		switch (m_wSearchFlags)
		{
			case SRCHFLAG_PROCESS_USEPROCESS:

				if (pcptn->GetObjectHandle() == m_dwProcess)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;

			case SRCHFLAG_PROCESS_USEMODULE:

				if (pcptn->GetModule() == m_strModule)
				{
					pcstvActive->m_SpyTreeCtl.SetCurSel(i);
					return(TRUE);
				}
				break;
		}
	}

	return(FALSE);
}

BEGIN_MESSAGE_MAP(CPrcTreeDoc, CSpyTreeDoc)
	//{{AFX_MSG_MAP(CPrcTreeDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

