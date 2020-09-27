// ipcmpvw.cpp : implementation of the CIPCompView class
//

#include "stdafx.h"
#pragma hdrstop

#include <afxodlgs.h>

#include "shldocs_.h"
#include "ipcmpctr.h"
#include "ipcmpvw.h"

IMPLEMENT_DYNCREATE(CIPCompView, CDocObjectView)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// These are defined in undoctl.cpp.
extern UINT WM_GETUNDOSTRING;
extern UINT WM_GETREDOSTRING;

/////////////////////////////////////////////////////////////////////////////
// CIPCompView


BEGIN_MESSAGE_MAP(CIPCompView, CDocObjectView)
	//{{AFX_MSG_MAP(CIPCompView)

    ON_WM_WINDOWPOSCHANGED()

	ON_MESSAGE(WM_HELP, OnWmHelp)
	ON_REGISTERED_MESSAGE(WM_GETUNDOSTRING, OnGetUndoString)
	ON_REGISTERED_MESSAGE(WM_GETREDOSTRING, OnGetRedoString)
	ON_REGISTERED_MESSAGE(WM_RESETCOMMANDTARGET, OnResetCommandTarget)

	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCompView construction

CIPCompView::CIPCompView()
{
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CIPCompDoc * CIPCompView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIPCompDoc )));
	return((CIPCompDoc *)m_pDocument);
}
#endif

BOOL CIPCompView::PreTranslateMessage(MSG* pMsg)
{
	// prevent CSlobWnd from grabbing shift-F10 away from the component.
	//
	if (((pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) && // If we hit a key and
		  (pMsg->wParam == VK_F10) &&		// it's F10
		  (GetKeyState(VK_SHIFT) & ~1) &&   //   and Shift is down
		  !(GetKeyState(VK_CONTROL) & ~1) ) //   but not Ctrl (DevStudio96 #7532)
		  ||                                // OR
		 (pMsg->message == WM_CONTEXTMENU)  //   Natural keyboard key
		)
	{
		return FALSE;
	}

	return CDocObjectView::PreTranslateMessage(pMsg);

}


void CIPCompView::OnInitialUpdate()
{
	CPartView::OnInitialUpdate();
}

HRESULT CIPCompView::OnActivateDocObjectView()
{
	// The document should contain exactly 1 OLE container item (otherwise the
	// doc shouldn't have been created successfully).  Get a pointer to the item.
	CIPCompDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	POSITION position = pDoc->GetStartPosition();
	CIPCompContainerItem *pItem = (CIPCompContainerItem *)pDoc->GetNextClientItem(position);
	ASSERT(pItem != NULL && pItem->IsKindOf(RUNTIME_CLASS(CIPCompContainerItem)) &&
		   pDoc->GetNextClientItem(position) == NULL);	// should be 1 and only 1 client item

	HRESULT hr = E_FAIL;
	if (pItem->GetOleDocument())
		hr = pItem->ActivateDocObjectView(this);

	if (SUCCEEDED(hr))
		pDoc->SetDocObjViewActivate(TRUE);
		
	return hr;
}


BOOL CIPCompView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	GUID * pguid;
	DWORD cmdid, rgf;
	BOOL fHandled = FALSE, fRoutedToShell = FALSE;;

	CIPCompDoc *pDoc = (CIPCompDoc *)GetDocument();

	if (pHandlerInfo == NULL &&
		pDoc &&
		pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)) &&
		pDoc->m_pItem != NULL &&
		pDoc->OleCmdFromId(nID, &pguid, &cmdid, &rgf))

	{
		if (rgf & OLECMDROUTEFLAG_SHELLFIRST)
		{
			fHandled = CDocObjectView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
		}

		if (!fHandled)
		{
			fHandled = pDoc->m_pItem->OnCompCmdMsg(pguid, cmdid, rgf,
				nCode, pExtra);
		}
	}

	if (!fHandled && !fRoutedToShell)
		fHandled = CDocObjectView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	return fHandled;
}

CSlob* CIPCompView::GetSelection()
{
	CIPCompDoc* pDoc = (CIPCompDoc*)GetDocument();
	if(NULL != pDoc)
		return pDoc->GetSlob();
	return NULL;
}

LRESULT
CIPCompView::OnGetUndoString(WPARAM nLevel, LPARAM lpstr)
{
	CIPCompDoc* pDoc = (CIPCompDoc*)GetDocument();

	if (NULL != pDoc)
		return pDoc->GetUndoRedoString(nLevel, lpstr, 256, TRUE);
	else
		return FALSE;
}

LRESULT CIPCompView::OnWmHelp(WPARAM, LPARAM)
{
	// If the in-place component didn't handle the WM_HELP message,
	// there's not much the shell can do about it, so we intercept
	// this message here.  (The default window proc passes the WM_HELP
	// on to the parent window.)
	//
	return TRUE;
}

LRESULT
CIPCompView::OnGetRedoString(WPARAM nLevel, LPARAM lpstr)
{
	CIPCompDoc* pDoc = (CIPCompDoc*)GetDocument();

	if (NULL != pDoc)
		return pDoc->GetUndoRedoString(nLevel, lpstr, 256, FALSE);
	else
		return FALSE;
}


afx_msg void CIPCompView::OnHelp()
{
	// This function is just here to save us from having to change
	// the shell's exports & relink the world.
}

void CIPCompView::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CIPCompDoc* pDoc = (CIPCompDoc*)GetDocument();
	if (pDoc != NULL)
	{
		ASSERT_KINDOF(CIPCompDoc, pDoc);
		pDoc->OnWindowPosChanged(lpwndpos);
	}
	
	CDocObjectView::OnWindowPosChanged(lpwndpos);
}

void CIPCompView::ShowContextPopupMenu(CPoint pt)
{
	CIPCompDoc* pDoc = (CIPCompDoc*)GetDocument();
	if (pDoc != NULL)
	{
		ASSERT_KINDOF(CIPCompDoc, pDoc);
		pDoc->ShowContextPopupMenu(pt);
	}
}

//
//
//	LRESULT CIPCompView::OnResetCommandTarget
//
//	Description:
//		Used to reset command target after Context Menu command has been
//		handled.  Calls CIPCFrameHook::ResetCommandTarget to do the reset.
//
//	Arguments:
//		WPARAM nUnUsed:
//
//		LPARAM lUnUsed:
//
//	Return (LRESULT): TRUE
//
LRESULT CIPCompView::OnResetCommandTarget(WPARAM nUnUsed, LPARAM lUnUsed)
{
	CIPCompDoc* pDoc = (CIPCompDoc*)GetDocument();
	ASSERT_KINDOF(CIPCompDoc, pDoc);
	if (NULL != pDoc && NULL != pDoc->m_pItem)
	{
		CIPCompFrameHook*	pFrameHook;
		pFrameHook = (CIPCompFrameHook*)(pDoc->m_pItem->m_pInPlaceFrame);
		if(NULL != pFrameHook)
		{
			pFrameHook->ResetCommandTarget();
		}
	}
	return TRUE;
}
