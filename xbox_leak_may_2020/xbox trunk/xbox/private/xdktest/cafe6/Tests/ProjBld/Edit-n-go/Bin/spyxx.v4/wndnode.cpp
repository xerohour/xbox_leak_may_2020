// wndtreec.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "treectl.h"
#include "imgwell.h"
#include "spytreec.h"
#include "wndnode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CWndTreeNode

IMPLEMENT_DYNCREATE(CWndTreeNode, CSpyTreeNode);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWndTreeNode::Create(HWND hwnd, BOOL fEnumChildren)
{
	HWND hwndChild;
	CWndTreeNode* pWndTreeNode;
	CHAR szTemp[256];
	LPSTR lpszClass;

	m_hwnd = hwnd;

	if (GetWindowText(hwnd, szTemp, 256))
	{
		m_strText = szTemp;
	}

	if (GetClassName(hwnd, szTemp, 256))
	{
		m_strClass = szTemp;
		if ((lpszClass = GetExpandedClassName((LPSTR)(const char *)m_strClass)) != NULL)
			m_strClass += lpszClass;
	}

	if (fEnumChildren)
	{
		hwndChild = ::GetWindow(hwnd, GW_CHILD);
		while (hwndChild)
		{
			pWndTreeNode = new CWndTreeNode;
			pWndTreeNode->Create(hwndChild, TRUE);
			this->AddChild(pWndTreeNode);

			hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
		}
	}

	return TRUE;
}



void CWndTreeNode::GetDescription(CString& str)
{
	LPSTR psz = str.GetBuffer(600);

	sprintf(psz, "Window %8.8X \"%s\" %s", (DWORD)m_hwnd, m_strText.GetBuffer(0), m_strClass.GetBuffer(0));

	str.ReleaseBuffer();
	m_strText.ReleaseBuffer();
	m_strClass.ReleaseBuffer();
}



void CWndTreeNode::ShowProperties()
{
	CPropertyInspector::ShowObjectProperties((DWORD)m_hwnd, OT_WINDOW);
}
