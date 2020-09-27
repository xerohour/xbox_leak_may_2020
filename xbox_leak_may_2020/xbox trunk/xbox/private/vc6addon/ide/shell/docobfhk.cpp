// docobfhk.cpp : implementation of the CDocObjectFrameHook class
//

#include "stdafx.h"
#pragma hdrstop

#include "docobfhk.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocObjectFrameHook implementation

BEGIN_INTERFACE_MAP(CDocObjectFrameHook, COleFrameHook)
	INTERFACE_PART(CDocObjectFrameHook, IID_IOleCommandTarget, OleCommandTarget)
END_INTERFACE_MAP()

CDocObjectFrameHook::CDocObjectFrameHook(CFrameWnd* pFrameWnd, COleClientItem* pItem)
	:COleFrameHook(pFrameWnd, pItem)
{
}

CDocObjectFrameHook::~CDocObjectFrameHook()
{
}

BOOL CDocObjectFrameHook::OnDocActivate(BOOL bActive)
{
	ASSERT_VALID(this);

	if (m_lpActiveObject == NULL)
		return TRUE;

	// allow server to do document activation related actions
	VERIFY(SUCCEEDED(m_lpActiveObject->OnDocWindowActivate(bActive)));

	// make sure window caption gets updated later
	COleFrameHook* pNotifyHook = m_pActiveItem->m_pInPlaceFrame;
	pNotifyHook->m_pFrameWnd->DelayUpdateFrameTitle();

	if (!bActive)
	{
		if (m_pActiveItem->m_pInPlaceDoc != NULL)
			m_pActiveItem->m_pInPlaceDoc->m_xOleInPlaceFrame.SetBorderSpace(NULL);

		// remove the menu hook when the doc is not active
		pNotifyHook->m_xOleInPlaceFrame.SetMenu(NULL, NULL, NULL);
		// clear border space
		pNotifyHook->m_xOleInPlaceFrame.SetBorderSpace(NULL);

		// unhook top-level frame if not needed
		if (pNotifyHook != this)
		{
			// shouldn't be removing some other hook
			ASSERT(pNotifyHook->m_pFrameWnd->m_pNotifyHook == pNotifyHook);
			pNotifyHook->m_pFrameWnd->m_pNotifyHook = NULL;
		}
	}
	else
	{
		// rehook top-level frame if necessary (no effect if top-level == doc-level)
		pNotifyHook->m_pFrameWnd->m_pNotifyHook = pNotifyHook;
	}

	// don't do default if activating
	return bActive;

}

/////////////////////////////////////////////////////////////////////////////
// Implementation of IOleCommandTarget

IMPLEMENT_ADDREF		(CDocObjectFrameHook, OleCommandTarget)
IMPLEMENT_RELEASE		(CDocObjectFrameHook, OleCommandTarget)
IMPLEMENT_QUERYINTERFACE(CDocObjectFrameHook, OleCommandTarget)

// This interface is only implemented so that PowerPoint finds us acceptable.  We
// do not have to return anything meaningful from the methods in the interface.

STDMETHODIMP CDocObjectFrameHook::XOleCommandTarget::QueryStatus(
			const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[],
			OLECMDTEXT *pCmdText)
{
	METHOD_PROLOGUE(CDocObjectFrameHook, OleCommandTarget);
	for(int i=0; i<cCmds; i++)
	{
		DWORD dwFlags = 0;
		switch(prgCmds[i].cmdID)
		{
		case OLECMDID_OPEN:
		case OLECMDID_NEW:
		case OLECMDID_SAVE:
		case OLECMDID_PRINT:
		case OLECMDID_PAGESETUP:
			dwFlags = OLECMDF_SUPPORTED | OLECMDF_ENABLED;
			break;
		}
		prgCmds[i].cmdf = dwFlags;
	}
	return S_OK;
}
        
STDMETHODIMP CDocObjectFrameHook::XOleCommandTarget::Exec(
			const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANTARG *pvaIn, VARIANTARG *pvaOut)
{
	METHOD_PROLOGUE(CDocObjectFrameHook, OleCommandTarget);
	switch(nCmdID)
	{
		case OLECMDID_OPEN:
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_OPEN);
			break;
		case OLECMDID_NEW:
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW_SOURCE);
			break;
		case OLECMDID_SAVE:
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_SAVE);
			break;
		case OLECMDID_PRINT:
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_PRINT);
			break;
		case OLECMDID_PAGESETUP:
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_PAGE_SETUP);
			break;
		default:
			return OLECMDERR_E_NOTSUPPORTED;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
