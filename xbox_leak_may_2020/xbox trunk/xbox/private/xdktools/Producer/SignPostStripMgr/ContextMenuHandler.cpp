// ContextMenuHandler.cpp : implementation file
//

#include "stdafx.h"
#include "SignPostStripMgr.h"
#include "ContextMenuHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler

CContextMenuHandler::CContextMenuHandler()
{
	m_pStrip = NULL;
}

CContextMenuHandler::~CContextMenuHandler()
{
}


BEGIN_MESSAGE_MAP(CContextMenuHandler, CWnd)
	//{{AFX_MSG_MAP(CContextMenuHandler)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CContextMenuHandler message handlers

BOOL CContextMenuHandler::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HRESULT hr = S_FALSE;

	if(m_pStrip != NULL)
	{
		hr = m_pStrip->OnWMMessage(WM_COMMAND, wParam, lParam);
	}
	
	if(hr == S_OK)
	{
		return TRUE;
	}

	return CWnd::OnCommand(wParam, lParam);
}
