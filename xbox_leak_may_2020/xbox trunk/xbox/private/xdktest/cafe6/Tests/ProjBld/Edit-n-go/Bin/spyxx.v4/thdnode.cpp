// thdtreec.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "spytreec.h"
#include "thdnode.h"
#include "wndnode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK EnumChildWndProc(HWND hwnd, LPARAM lParam);



/////////////////////////////////////////////////////////////////////////////
// CThdTreeNode

IMPLEMENT_DYNCREATE(CThdTreeNode, CSpyTreeNode);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CThdTreeNode::Create(CPROCDB *pProcessDatabase, int iThdIndex)
{
	LPWSTR pszExe;
	LPWSTR pszModule;
	CPROCDB::PRFCNTRTHRD PrfCntrThrd;

	memset((void *)&PrfCntrThrd, 0, sizeof(CPROCDB::PRFCNTRTHRD));
	pProcessDatabase->GetPrfCntrThrd(iThdIndex, &PrfCntrThrd);

	m_tid = (DWORD)PrfCntrThrd.dwThreadId;

	pszExe = (LPWSTR)pProcessDatabase->GetThreadName(iThdIndex);

	if (pszModule = wcsrchr(pszExe, '\\'))
	{
		pszModule++;
	}
	else
	{
		pszModule = pszExe;
	}

	m_strModule = wcsupr(pszModule);

	return TRUE;
}



BOOL CThdTreeNode::Create(HANDLE hThreadList, DWORD dwThreadID, char *szExePath)
{
	m_tid = dwThreadID;

	char szFileName[_MAX_FNAME];
	_splitpath(szExePath, NULL, NULL, szFileName, NULL);

	m_strModule = szFileName;
//	m_strModule += " (";
//	m_strModule += szExePath;
//	m_strModule += ")";

	return TRUE;
}



void CThdTreeNode::GetDescription(CString& str)
{
	LPSTR psz = str.GetBuffer(260);

	_stprintf(psz, ids(IDS_THREAD_PARAM), m_tid, m_strModule.GetBuffer(0));

	str.ReleaseBuffer();
	m_strModule.ReleaseBuffer();
}



void CThdTreeNode::AddChildren(CMapPtrToPtr* pMap)
{
	EnumWindows(EnumWndProc, (LPARAM)pMap);
}



BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam)
{
	CMapPtrToPtr* pMap = (CMapPtrToPtr*)lParam;
	void * pThdTreeNode;
	DWORD tid = GetWindowThreadProcessId(hwnd, NULL);

	if (pMap->Lookup((void*)tid, pThdTreeNode))
	{
		CWndTreeNode* pWndTreeNode = new CWndTreeNode;
		pWndTreeNode->Create(hwnd, FALSE);

		((CThdTreeNode*)pThdTreeNode)->AddChild(pWndTreeNode);

		EnumChildWindows(hwnd, EnumChildWndProc, lParam);
	}

	return TRUE;
}



BOOL CALLBACK EnumChildWndProc(HWND hwnd, LPARAM lParam)
{
	CMapPtrToPtr* pMap = (CMapPtrToPtr*)lParam;
	void * pThdTreeNode;
	DWORD tid = GetWindowThreadProcessId(hwnd, NULL);

	if (pMap->Lookup((void*)tid, pThdTreeNode))
	{
		CWndTreeNode* pWndTreeNode = new CWndTreeNode;
		pWndTreeNode->Create(hwnd, FALSE);

		((CThdTreeNode*)pThdTreeNode)->AddChild(pWndTreeNode);
	}

	return TRUE;
}



void CThdTreeNode::ShowProperties()
{
	CPropertyInspector::ShowObjectProperties((DWORD)m_tid, OT_THREAD);
}


