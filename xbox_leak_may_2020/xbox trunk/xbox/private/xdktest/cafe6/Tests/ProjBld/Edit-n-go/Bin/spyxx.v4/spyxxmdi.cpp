// spyxxmdi.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "spyxxmdi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpyMDIChildWnd

IMPLEMENT_DYNCREATE(CSpyMDIChildWnd, CMDIChildWnd)

CSpyMDIChildWnd::CSpyMDIChildWnd()
{
}

CSpyMDIChildWnd::~CSpyMDIChildWnd()
{
}

BEGIN_MESSAGE_MAP(CSpyMDIChildWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSpyMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL fIgnoreSizing = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CSpyMDIChildWnd message handlers

int CSpyMDIChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	fIgnoreSizing = TRUE;

	return 0;
}

void CSpyMDIChildWnd::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	if (!fIgnoreSizing)
	{
		if (nType == SIZE_MAXIMIZED)
			theApp.m_bIsMDIMaximized = TRUE;
		else
			theApp.m_bIsMDIMaximized = FALSE;
	}
	else
		fIgnoreSizing = FALSE;
}
