/////////////////////////////////////////////////////////////////////////////
//	SLOBWND.CPP

#include "stdafx.h"

#include "slob.h"

// used for wizard bar invalidation in selection
#include <clvwapi.h>
#include <clvwguid.h>
#include "toolexpt.h"


IMPLEMENT_DYNAMIC(CSlobWnd, CView)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern UINT NEAR WM_GETUNDOSTRING;
extern UINT NEAR WM_GETREDOSTRING;

CObList NEAR CSlobWnd::c_slobWnds;

POSITION CSlobWnd::GetHeadSlobWndPosition()
{
	return c_slobWnds.GetHeadPosition();
}

CSlobWnd* CSlobWnd::GetNextSlobWnd(POSITION& pos)
{
	return (CSlobWnd*)c_slobWnds.GetNext(pos);
}

CSlobWnd::CSlobWnd()
{
	m_pUndoStack = new CUndoStack;

    c_slobWnds.AddHead(this);
}

CSlobWnd::~CSlobWnd()
{
	delete m_pUndoStack;

    POSITION pos = c_slobWnds.Find(this);
    ASSERT(pos != NULL);
    c_slobWnds.RemoveAt(pos);
}

BEGIN_MESSAGE_MAP(CSlobWnd, CView)
	ON_WM_DESTROY()
	ON_WM_RBUTTONUP()
	ON_COMMAND(IDM_WINDOW_SHOWPROPERTIES, CmdShowProp)
	ON_COMMAND(IDMX_IDPROP, CmdIDProp)
	ON_COMMAND(IDMX_PROPGENERAL, CmdGeneralPage)
	ON_COMMAND(IDMX_PROPSTYLES, CmdStylesPage)
	ON_REGISTERED_MESSAGE(WM_GETUNDOSTRING, OnGetUndoString)
	ON_REGISTERED_MESSAGE(WM_GETREDOSTRING, OnGetRedoString)
END_MESSAGE_MAP()

BOOL CSlobWnd::PreTranslateMessage(MSG* pMsg)
{
	// Shift + F10 show popup menu.  (Like Word and Excel).
	if (((pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) && // If we hit a key and
		  (pMsg->wParam == VK_F10) &&		// it's F10
		  (GetKeyState(VK_SHIFT) & ~1) &&   //   and Shift is down
		  !(GetKeyState(VK_CONTROL) & ~1) ) //   but not Ctrl (DevStudio96 #7532)
		  ||                                // OR
		 (pMsg->message == WM_CONTEXTMENU)  //   Natural keyboard key
		)
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		CPoint point = rect.TopLeft();
		point.Offset(5, 5);
		ShowContextPopupMenu(point);

		return TRUE;
	}

	return CView::PreTranslateMessage(pMsg);
}

void CSlobWnd::OnDestroy()
{
	// So we don't GP Fault on deleted action slobs.
	FlushUndo();

	// HACK: NT is supposed to do this for us!
	CWnd* pParent = GetParent();
	if (pParent != NULL)
		pParent->SendMessage(WM_PARENTNOTIFY, WM_DESTROY, (LPARAM)(LPVOID)m_hWnd);
	
	CView::OnDestroy();
}

void CSlobWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (GetActiveSlobWnd() != this)
		return;

	ClientToScreen(&point);
	ShowContextPopupMenu(point);
}

void CSlobWnd::PostNcDestroy()
{
	delete this;
}

void CSlobWnd::ObscureSprites(BOOL bObscure)
{
}

void CSlobWnd::ShowSprites(BOOL bShow)
{
}

void CSlobWnd::ShowContextPopupMenu(CPoint pt)
{
}

/////////////////////////////////////////////////////////////////////////////
// Property sheet Support

void CSlobWnd::CmdGeneralPage()
{
	if (GetCapture() || IsIconic())
		return; // ignore if in drag op

	GetPropertyBrowser()->FlipToPage(IDS_GENERAL);
}

void CSlobWnd::CmdStylesPage()
{
	if (GetCapture() || IsIconic())
		return; // ignore if in drag op

	GetPropertyBrowser()->FlipToPage(IDS_STYLES);
}
 
void CSlobWnd::CmdIDProp()
{
	// Command to turn the property browser on and select the ID field
	
	if (GetCapture() || IsIconic())
		return; // ignore if in drag op
	
	if (BeginPropertyBrowserEdit(P_ID))
		ActivatePropertyBrowser();
}

void CSlobWnd::CmdShowProp()
{
	// Command to turn the property browser on
	
	if (GetCapture())
		return; // ignore if in drag op
	
	ActivatePropertyBrowser();
}

/////////////////////////////////////////////////////////////////////////////
// Undo/Redo Support

BOOL CSlobWnd::EnableUndo()
{
	// Override this implementation only if you want to
	// *DISABLE* undo/redo handling in your CSlobWnd.
	return TRUE;
}

void CSlobWnd::BeginUndo(const char* szAction)
{
	theUndoSlob.BeginUndo(szAction, this);
}

void CSlobWnd::BeginUndo(UINT nID)
{
	theUndoSlob.BeginUndo(nID, this);
}

void CSlobWnd::EndUndo(BOOL bAbort /*=FALSE*/)
{
	theUndoSlob.EndUndo(bAbort);
}

void CSlobWnd::FlushUndo()
{
	EndUndo(TRUE);	// Abort any recording.
	m_pUndoStack->Flush();
}

BOOL CSlobWnd::CanUndo()
{
	return m_pUndoStack->m_nRedoSeqs < m_pUndoStack->m_seqs.GetCount();
}

BOOL CSlobWnd::CanRedo()
{
	return m_pUndoStack->m_nRedoSeqs > 0;
}

void CSlobWnd::DoUndo()
{
	if (!CanUndo())
		return;

	CSheetWnd * pPropSheet = GetPropertyBrowser();
    if (pPropSheet != NULL && pPropSheet->UndoPendingValidate())
        return;

    // NOTE:  We need to obscure any sprites as well because we may
	// be about to change the selection and/or show objects.

    ObscureSprites(TRUE);

	theUndoSlob.DoUndo(this);
}

void CSlobWnd::DoRedo()
{
	if (!CanRedo())
		return;
	
    // NOTE:  We need to obscure any sprites as well because we may
	// be about to change the selection and/or show objects.
	
    ObscureSprites(TRUE);

	theUndoSlob.DoRedo(this);
}

LRESULT CSlobWnd::OnGetUndoString(WPARAM nLevel, LPARAM lpstr)
{
	CString str;
	theUndoSlob.GetUndoString(str, nLevel);
	lstrcpy((LPSTR)lpstr, str);
	
	return !str.IsEmpty();
}

LRESULT CSlobWnd::OnGetRedoString(WPARAM nLevel, LPARAM lpstr)
{
	CString str;
	theUndoSlob.GetRedoString(str, nLevel);
	lstrcpy((LPSTR)lpstr, str);
	
	return !str.IsEmpty();
}


/////////////////////////////////////////////////////////////////////////////
// Selection Management

void CSlobWnd::Select(CSlob* pSlob, BOOL bAdd /* = FALSE */ )
{
	CMultiSlob* pSelection = (CMultiSlob*)GetSelection();
	ASSERT(pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)));

	if (pSlob == NULL)
		pSelection->Clear();	
	else if (bAdd)
		pSelection->Add(pSlob);
	else
		pSelection->Single(pSlob);

	// Invalidate Wizard Bar only if it is tracking in current view

	LPWIZARDBAR lpwbIFace =	g_IdeInterface.GetWizBarIFace();
	if (lpwbIFace != NULL)
	{
		HRESULT hr;
		ULONG wbFlags;
		hr = lpwbIFace->GetWizBarFlags(&wbFlags, FALSE /* fCreateIfNone */);

		if (hr == S_OK && (wbFlags & mskWBTrackedByView))
		{
			ASSERT((wbFlags & mskWBExists));
			lpwbIFace->WizBarInvalidate();
		}
	}

}

void CSlobWnd::Deselect(CSlob* pSlob)
{
	CMultiSlob* pSelection = (CMultiSlob*)GetSelection();
	ASSERT(pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)));
	
	pSelection->Remove(pSlob);
}

BOOL CSlobWnd::IsSelected(CSlob* pSlob)
{
	CMultiSlob* pSelection = (CMultiSlob*)GetSelection();
	ASSERT(pSelection != NULL);
	
	if (!pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		return pSlob == pSelection;
	
	return pSelection->IsSelected(pSlob);
}

BOOL CSlobWnd::IsOnlySelected(CSlob* pSlob)
{
	CMultiSlob* pSelection = (CMultiSlob*)GetSelection();
	ASSERT(pSelection != NULL);
	
	if (!pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		return pSlob == pSelection;
	
	return pSelection->IsOnlySelected(pSlob);
}

BOOL CSlobWnd::IsDominantSelected(CSlob* pSlob)
{
	CMultiSlob* pSelection = (CMultiSlob*)GetSelection();
	ASSERT(pSelection != NULL);
	
	if (!pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		return pSlob == pSelection;
	
	return pSelection->IsDominantSelected(pSlob);
}


CSlob* CSlobWnd::DropTargetFromPoint(const POINT& point)
{
	return GetEditSlob();
}

void CSlobWnd::OnSelChange()
{
}

void CSlobWnd::OnDraw(CDC* pDC)
{
}

void CSlobWnd::OnActivateView(BOOL bActivate, CView* pActivateView,
	CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if (bActivate)
	{
		CMDIChildWnd* pFrame = (CMDIChildWnd*) GetParentFrame();
		if (pFrame != NULL &&
				pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) &&
				pFrame->IsIconic())
			theApp.m_theAppSlob.SetSlobProp(P_CurrentSlob, NULL);
		else
			theApp.m_theAppSlob.SetSlobProp(P_CurrentSlob, GetSelection());
	}
}

/////////////////////////////////////////////////////////////////////////////

CSlobWnd* GetActiveSlobWnd()
{
	HWND hWnd = theApp.GetActiveView();

	if (hWnd == NULL)
		return NULL;
	
	CSlobWnd* pWnd = (CSlobWnd*)CWnd::FromHandle(hWnd);
	ASSERT(pWnd != NULL);
	
	if (!pWnd->IsKindOf(RUNTIME_CLASS(CSlobWnd)))
		return NULL;
	
	return pWnd;
}

void ActivateSlobWnd(CSlobWnd* pWnd)
{
	ASSERT(pWnd->GetSafeHwnd() != NULL);
	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CSlobWnd)));
	
	theApp.SetActiveView(pWnd->m_hWnd);
}


/////////////////////////////////////////////////////////////////////////////
