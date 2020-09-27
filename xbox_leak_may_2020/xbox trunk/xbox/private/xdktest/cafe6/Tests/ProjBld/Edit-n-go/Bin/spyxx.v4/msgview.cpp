// msgview.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "fontdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgView

IMPLEMENT_DYNCREATE(CMsgView, CView)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMsgView::CMsgView()
{
}

CMsgView::~CMsgView()
{
}


BEGIN_MESSAGE_MAP(CMsgView, CView)
	//{{AFX_MSG_MAP(CMsgView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_FONT, OnViewFont)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewProperties)
	ON_COMMAND(ID_VIEW_HELPONMESSAGE, OnViewHelpOnMessage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HELPONMESSAGE, OnUpdateViewHelpOnMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CMsgView::AddLine(LPSTR psz, PMSGSTREAMDATA pmsd)
{
	m_MsgLog.AddLine(psz, pmsd);

	CMsgDoc *pMsgDoc = GetDocument();

	ASSERT(pMsgDoc != NULL);

	if (pMsgDoc->m_fLogToFile && pMsgDoc->m_hLoggingFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwLen;
		WriteFile(pMsgDoc->m_hLoggingFile, psz, (DWORD)_tcslen(psz), (LPDWORD)&dwLen, NULL);
		ASSERT(dwLen == (DWORD)_tcslen(psz));
		WriteFile(pMsgDoc->m_hLoggingFile, "\r\n", (DWORD)2, (LPDWORD)&dwLen, NULL);
	}
}



/////////////////////////////////////////////////////////////////////////////
// CMsgView drawing

void CMsgView::OnPaint()
{
	// do not call CView::OnPaint since it will call OnDraw
	CWnd::OnPaint();
}

void CMsgView::OnDraw(CDC*)
{
	// do nothing here
}



/////////////////////////////////////////////////////////////////////////////
// CMsgView message handlers

void CMsgView::OnViewFont()
{
	HFONT hfontNew;

	if (hfontNew = SpyFontDialog(m_hfont))
	{
		HFONT hfontOld = m_hfont;

		SetFont(CFont::FromHandle(hfontNew));
		::DeleteObject(hfontOld);
	}
}

int CMsgView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (m_MsgLog.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 0) == -1)
	{
		return -1;
	}

	CFont Font;
	Font.CreateFontIndirect(&theApp.m_DefLogFont);
	SetFont(&Font, FALSE);
	Font.Detach();

	return 0;
}



void CMsgView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_MsgLog.MoveWindow(CRect(0, 0, cx, cy));
}



void CMsgView::OnSetFocus(CWnd* pOldWnd)
{
	m_MsgLog.SetFocus();
}

void CMsgView::OnViewProperties()
{
	if (m_MsgLog.IsEmpty())
		m_MsgLog.ShowViewedItemProperties(this);
	else
		m_MsgLog.ShowSelItemProperties();
}

void CMsgView::OnUpdateViewProperties(CCmdUI* pCmdUI)
{
	//
	// Enable only if there is a selected item.
	//
	pCmdUI->Enable((m_MsgLog.IsEmpty()) || (m_MsgLog.GetCurSel() != -1));
}



void CMsgView::OnViewHelpOnMessage()
{
	//
	// Get a pointer to the message description struct for the
	// last message that was clicked on to bring up the context
	// popup menu.
	//
	PMSGDESC pmd = m_MsgLog.GetPopupMsgDesc();

	if (pmd)
	{
		theApp.InvokeWinHelp((DWORD)pmd->pszMsg, HELPTYPE_MSGHELP);
	}
}

void CMsgView::OnUpdateViewHelpOnMessage(CCmdUI* pCmdUI)
{
	//
	// Enable only if a known message was clicked on.  This
	// must be done because we can only show help on messages
	// that are known (we need a valid string to show help
	// on, of course!)
	//
//	pCmdUI->Enable(m_MsgLog.GetPopupMsgDesc() != NULL);
	pCmdUI->Enable(FALSE);
}
