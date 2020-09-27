// prctreec.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "spytreec.h"
#include "prcnode.h"
#include "thdnode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CPrcTreeNode

IMPLEMENT_DYNCREATE(CPrcTreeNode, CSpyTreeNode);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CPrcTreeNode::Create(CPROCDB *pProcessDatabase, int iPrcIndex, CMapPtrToPtr* pThdMap)
{
	LPWSTR pszExe;
	CPROCDB::PRFCNTRPROC PrfCntrProc;
	CPROCDB::PRFCNTRTHRD PrfCntrThrd;
	int iCount = 0, iLoop = pProcessDatabase->GetNumThreads(), iNumThreads = 0;

	memset((void *)&PrfCntrProc, 0, sizeof(CPROCDB::PRFCNTRPROC));
	pProcessDatabase->GetPrfCntrProc(iPrcIndex, &PrfCntrProc);

	m_pid = (DWORD)PrfCntrProc.dwProcessId;

	pszExe = (LPWSTR)pProcessDatabase->GetProcessName(iPrcIndex);
	m_strModule = wcsupr(pszExe);

	for (; iCount < iLoop; iCount++)
	{
		memset((void *)&PrfCntrThrd, 0, sizeof(CPROCDB::PRFCNTRTHRD));
		pProcessDatabase->GetPrfCntrThrd(iCount, &PrfCntrThrd);

		if (PrfCntrThrd.dwProcessId == PrfCntrProc.dwProcessId)
		{
			CThdTreeNode* pThdTreeNode = new CThdTreeNode;
			pThdTreeNode->Create(pProcessDatabase, iCount);
			this->AddChild(pThdTreeNode);
			pThdMap->SetAt((void*)pThdTreeNode->GetObjectHandle(), pThdTreeNode);
			iNumThreads++;
		}
	}

	int iSupposedNumThreads = pProcessDatabase->GetNumProcessThreads(iPrcIndex);
	ASSERT(iNumThreads == iSupposedNumThreads);

	return TRUE;
}



BOOL CPrcTreeNode::Create(HANDLE hProcessList, DWORD dwProcessID, char *szExePath, CMapPtrToPtr* pThdMap)
{
	m_pid = dwProcessID;

	char szFileName[_MAX_FNAME];
	_splitpath(szExePath, NULL, NULL, szFileName, NULL);

	m_strModule = szFileName;
//	m_strModule += " (";
//	m_strModule += szExePath;
//	m_strModule += ")";

	HANDLE hThreadList;
	THREADENTRY32 te32;

	te32.dwSize = sizeof(THREADENTRY32);

	if ((hThreadList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPTHREAD, 0)) != NULL)
	{
		if ((*theApp.pfnThread32First)(hThreadList, &te32))
		{
			if (te32.th32OwnerProcessID == dwProcessID)
			{
				CThdTreeNode* pThdTreeNode = new CThdTreeNode;
				pThdTreeNode->Create(hThreadList, te32.th32ThreadID, szExePath);
				this->AddChild(pThdTreeNode);
				pThdMap->SetAt((void*)pThdTreeNode->GetObjectHandle(), pThdTreeNode);
			}

			while ((*theApp.pfnThread32Next)(hThreadList, &te32))
			{
				if (te32.th32OwnerProcessID == dwProcessID)
				{
					CThdTreeNode* pThdTreeNode = new CThdTreeNode;
					pThdTreeNode->Create(hThreadList, te32.th32ThreadID, szExePath);
					this->AddChild(pThdTreeNode);
					pThdMap->SetAt((void*)pThdTreeNode->GetObjectHandle(), pThdTreeNode);
				}
			}
		}

		CloseHandle(hThreadList);
	}

	return TRUE;
}



void CPrcTreeNode::GetDescription(CString& str)
{
	LPSTR psz = str.GetBuffer(260);

	_stprintf(psz, ids(IDS_PROCESS_PARAM), m_pid, m_strModule.GetBuffer(0));

	str.ReleaseBuffer();
	m_strModule.ReleaseBuffer();
}



void CPrcTreeNode::ShowProperties()
{
	CPropertyInspector::ShowObjectProperties((DWORD)m_pid, OT_PROCESS);
}
