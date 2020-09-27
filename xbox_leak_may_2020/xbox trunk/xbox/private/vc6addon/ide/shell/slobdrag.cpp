#include "stdafx.h"
#include "slob.h"
#include "mainfrm.h"
#include "bardockx.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


HCURSOR hcurNoDrop = NULL;
HCURSOR hcurArrow = NULL;


/////////////////////////////////////////////////////////////////////////////
// Drag and Drop Manager API

void BeginDragDrop(CPoint downPoint, CSlob* dragSlob)
{
	ASSERT(!IsDragDropInProgress());
	BeginDragDrop(new CDragNDrop(downPoint, dragSlob));
}

void BeginDragDrop(CDragNDrop* pInfo)
{
    VERIFY(((CMainFrame*)theApp.m_pMainWnd)->m_dragDrop == pInfo);

	if (hcurNoDrop == NULL)
//		VERIFY((hcurNoDrop = LoadCursor(GetResourceHandle(), MAKEINTRESOURCE(IDC_DDNODROP))) != NULL);
		VERIFY((hcurNoDrop = theApp.LoadStandardCursor(IDC_NO)) != NULL);
	if (hcurArrow == NULL)
		VERIFY((hcurArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW)) != NULL);

    theApp.m_pMainWnd->SetCapture();
}

void CancelDragDrop()
{
	ASSERT(IsDragDropInProgress());
	((CMainFrame*)theApp.m_pMainWnd)->m_dragDrop->AbortDrop();
	delete ((CMainFrame*)theApp.m_pMainWnd)->m_dragDrop;
	((CMainFrame*)theApp.m_pMainWnd)->m_dragDrop = NULL;

	ReleaseCapture();
}

BOOL IsDragDropInProgress()
{
	return ((CMainFrame*)theApp.m_pMainWnd)->m_dragDrop != NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Drag/Drop Handling in the main window

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_dragDrop == NULL || GetCapture() != this)
		CMDIFrameWnd::OnMouseMove(nFlags, point);
	else
    {
        ClientToScreen(&point);
        m_dragDrop->OnMouseMove(nFlags, point);
    }
}

void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (CASBar::s_pCustomizer != NULL)
		::MessageBeep(UINT(-1));
	else if (GetCapture() != this)
        CMDIFrameWnd::OnLButtonDown(nFlags, point);
    else
    {
        if (m_dragDrop != NULL)
            m_dragDrop->AbortDrop();

        EndDragDrop();
    }
}

void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
    if (GetCapture() != this)
        CMDIFrameWnd::OnRButtonDown(nFlags, point);
    else
    {
        if (m_dragDrop != NULL)
            m_dragDrop->AbortDrop();

        EndDragDrop();
    }
}

void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (GetCapture() != this)
        CMDIFrameWnd::OnLButtonUp(nFlags, point);
    else
    {
        if (m_dragDrop != NULL)
        {
            ClientToScreen(&point);

            m_dragDrop->OnLButtonUp(nFlags, point);
        }

        EndDragDrop();
    }
}

void CMainFrame::OnRButtonUp(UINT nFlags, CPoint point)
{
    if (GetCapture() != this)
        CMDIFrameWnd::OnRButtonUp(nFlags, point);
    else
    {
        if (m_dragDrop != NULL)
        {
            ClientToScreen(&point);

			m_dragDrop->m_flags = nFlags;
			m_dragDrop->m_point = point;

            m_dragDrop->OnRButtonUp(nFlags, point);
        }

		// The menu menu probably took capture from us anyway, but this
		// will force us to abort the drag in idle if the user cancels
		// the menu.

		ReleaseCapture();
    }
}

void CMainFrame::OnDragMove()
{
	ASSERT (m_dragDrop != NULL);
	m_dragDrop->m_MoveCopy = CDragNDrop::dragMove;
	m_dragDrop->Drop(m_dragDrop->m_flags, m_dragDrop->m_point);
	EndDragDrop();
}

void CMainFrame::OnDragCopy()
{
	ASSERT (m_dragDrop != NULL);
	m_dragDrop->m_MoveCopy = CDragNDrop::dragCopy;
	m_dragDrop->Drop(m_dragDrop->m_flags, m_dragDrop->m_point);
	EndDragDrop();
}

void CMainFrame::OnDragCancel()
{
	ASSERT (m_dragDrop != NULL);

    m_dragDrop->AbortDrop();
	EndDragDrop();
}

void CMainFrame::EndDragDrop()
{
	if (m_dragDrop != NULL)
	{
		delete m_dragDrop;
		m_dragDrop = NULL;
	}

	ReleaseCapture ();
}

////////////////////////////////////////////////////////////////////////////
// CDragNDrop

CDragNDrop::CDragNDrop(CPoint downPoint, CSlob *dragSlob)
{
    ASSERT(dragSlob != NULL);
    
    ASSERT(((CMainFrame*)theApp.m_pMainWnd)->m_dragDrop == NULL);
    ((CMainFrame*)theApp.m_pMainWnd)->m_dragDrop = this;

    m_dragSlob = dragSlob;
    m_dragInfo = NULL;

	CObList* pList = m_dragSlob->GetContentList();
	BOOL bMulti (pList != NULL && pList->GetCount() > 1);

    m_wLastKeyState = 0;

    // HACK to make sure that we go through the mousemove code at
    // least once
    m_target = (CSlob*)(-1);

	// we may get here before we get to the initialization in BeginDragDrop(),
	// so make sure we have a valid no-drop cursor before calling OnMouseMove
	if (hcurNoDrop == NULL)
//		VERIFY((hcurNoDrop = LoadCursor(GetResourceHandle(), MAKEINTRESOURCE(IDC_DDNODROP))) != NULL);
		VERIFY((hcurNoDrop = theApp.LoadStandardCursor(IDC_NO)) != NULL);

    m_lastPoint.x = -1;
    m_lastPoint.y = -1;
    m_downPoint = downPoint;

	m_MoveCopy = dragNotSet;

    OnMouseMove(0, m_downPoint);
}

CDragNDrop::~CDragNDrop()
{
    if (m_dragInfo)
        delete m_dragInfo;

    if ( m_dragSlob && m_dragSlob->GetContentList() )
        delete m_dragSlob;
}

BOOL CDragNDrop::IsCopy()
{
    // default behavior is to move unless the control key is down.
	// User's choice in dragdrop menu overrides.
    return (m_dragSlob->CanAct(act_copy)
		&&	(m_MoveCopy == dragCopy
			||	(m_MoveCopy == dragNotSet && (m_wLastKeyState & DD_CTRL) != 0)));
}

void CDragNDrop::OnLButtonUp(UINT nFlags, CPoint point)
{
	Drop(nFlags, point);
}

BEGIN_POPUP_MENU(DragDrop)
	MENU_ITEM_(ID_DRAG_MOVE, IDS_DRAG_MOVE)
	MENU_ITEM_(ID_DRAG_COPY, IDS_DRAG_COPY)
	MENU_SEPARATOR()
	MENU_ITEM_(ID_DRAG_CANCEL, IDS_DRAG_CANCEL)
END_POPUP_MENU()

void CDragNDrop::OnRButtonUp(UINT nFlags, CPoint point)
{
    if (m_dragInfo == NULL)
    {
        AbortDrop();
        return;
    }

#ifndef NO_CMDBARS
    TRACE2("CBShow: DragDrop (%s, %d)\r\n", __FILE__, __LINE__);
	::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(DragDrop), point); //DER
#else
	::ShowContextPopupMenu(MENU_CONTEXT_POPUP(DragDrop), point);
#endif
}

void CDragNDrop::Drop(UINT nFlags, CPoint point)
{
    CWaitCursor waitCursor;

    // Make sure the point where going to use has been passed to 
    // OnMouseMove...
    if (point != m_lastPoint)
	{
        OnMouseMove(nFlags, point);
	}
    
    if (m_dragInfo == NULL || !m_target->PreAct(act_drop))
    {
        AbortDrop();
        return;
    }

    // need one last update to make sure that the keystate flag is
    // correct.
    IdleUpdate();

    // tell the last drop target to go ahead and make the drop
    CSlobWnd* pWnd = (CSlobWnd*)CWnd::WindowFromPoint(point);
    while (pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CSlobWnd)))
        pWnd = (CSlobWnd*)pWnd->GetParent();
    ASSERT(pWnd != NULL);
    ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CSlobWnd)));

    theApp.m_pMainWnd->SetActiveWindow();

    m_dragInfo->Hide();
	
	
	CSlobWnd* pSourceWnd = m_dragSlob->GetSlobWnd();
	CSlobWnd* pDestWnd = m_target->GetSlobWnd();
	
	ASSERT(pDestWnd != NULL);
	ASSERT(pDestWnd == pWnd);
	// NOTE: pSourceWnd is NULL when dragging from control palette
	
    CMultiSlob* pSelection = NULL;
    if (pSourceWnd != NULL)
        pSelection = (CMultiSlob*)pSourceWnd->GetSelection();
    if (pSelection != NULL &&
        !pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
    {
        // no selection slob in the wnd
        pSelection = NULL;
    }

    CMultiSlob* pDestSelection = (CMultiSlob*)pDestWnd->GetSelection();
    if (pDestSelection != NULL &&
        !pDestSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
    {
        pDestSelection = NULL;
    }

	m_target->PrepareDrop(this);

    theUndoSlob.FlushLast(); // Don't coalesce D&D operations!

	CSlob* pDeleteThisSlob = NULL; // used for cross-window moves

    if (!m_dragSlob->GetContentList()->IsEmpty())
    {
		theApp.NotifyPackages(PN_BEGIN_MULTIOP);
		pDestWnd->BeginUndo((UINT) (IsCopy() ? IDS_UNDO_COPY : IDS_UNDO_MOVE));
		pDestWnd->EnableUpdates(FALSE);
        
        // Clear the selection (if there is one) so DoDrop 
        // can just Add everything
        if (pDestSelection != NULL)
            pDestSelection->Clear();
    
        ASSERT(m_target != NULL);
		
		if (pSourceWnd != pDestWnd && pSourceWnd != NULL)
		{
			// Clone the thing no matter what.  Drop the clone on the
			// destination and delete the original.  That way the undo
			// stacks will get set up correctly...
			
			CSlob* pClone = m_dragSlob->Clone();
			if (pClone != NULL)
			{
				m_target->DoDrop(pClone, m_dragInfo);

				if (pClone->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
					delete pClone;

				if (!IsCopy())
					pDeleteThisSlob = m_dragSlob;
			}
		}
		else // dropping in source window
		{
			if (!IsCopy())
			{
				m_target->DoDrop(m_dragSlob, m_dragInfo);
			}
			else
			{
				// Keep a pointer, so if the clone is really just a list
				// we can delete it, and avoid leaks.
				//
				CSlob* pClone = m_dragSlob->Clone();
				if (pClone != NULL)
				{
					m_target->DoDrop(pClone, m_dragInfo);

					if (pClone->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
						delete pClone;
				}
			}
		}
		theApp.NotifyPackages(PN_END_MULTIOP);
	}	

	FixupSelections(pSelection, pDestSelection);

    if (theUndoSlob.IsRecording())
    {
		pDestWnd->EnableUpdates(TRUE);
        pDestWnd->EndUndo();
        theUndoSlob.FlushLast();
    }
	
	// We did a cross-window move, so we need to delete the original
	// slob and record that in the source window's undo stack.
	if (pDeleteThisSlob != NULL)
	{
		ASSERT(pSourceWnd != NULL);
		theUndoSlob.Link();
		pSourceWnd->BeginUndo(IDS_UNDO_MOVE);
		pSourceWnd->EnableUpdates(FALSE);
		pDeleteThisSlob->MoveInto(NULL);
		pSourceWnd->EnableUpdates(TRUE);
		pSourceWnd->EndUndo();
	}
	
	ASSERT(!theUndoSlob.IsRecording());

    // Activate after doing the drop, so if we are doing dragging between
    // windows, the old window's toolbar gets updated to the new selection.
    //
	ActivateSlobWnd(pDestWnd);
	   
    m_target = NULL;
}

void CDragNDrop::FixupSelections(CMultiSlob* pSource, CMultiSlob* pDest)
{
    if (pDest != NULL)
        pDest->EnsureValidSelection();
    
    if (pSource != NULL && pSource != pDest)
        pSource->EnsureValidSelection();
}


void CDragNDrop::OnMouseMove(UINT nFlags, CPoint point)
{
    // Nasty hack here because Windows sends a continuous stream of
    // WM_MOUSEMOVE messages when a button is held down...
    if (point == m_lastPoint)
        return;
    m_lastPoint = point;

	BOOL bChange = FALSE;

    // get the drop target from the screen point and see if
    // it will accept the slob being dragged
    CSlob* newTarget = CSlob::DropTargetFromPoint (point);

	// walk up parent chain looking for a slob that will accept the drop.
	while (newTarget != NULL && newTarget != m_target &&
			!newTarget->CanAdd(m_dragSlob))
		newTarget = newTarget->GetContainer();

    // if the target has changed, delete the old dragInfo
    // and make a new one
    if (newTarget != m_target)
    {
        m_target = newTarget;
        if (m_dragInfo != NULL)
        {
            delete m_dragInfo;
            m_dragInfo = NULL;
        }

        if (m_target != NULL)
            m_dragInfo = m_target->CreateDragger(m_dragSlob, point);

		bChange = TRUE;
    }
    
	if (m_target != NULL && (nFlags & MK_RBUTTON) == 0)
    {
		WORD wKeyState  = 0;
		if (nFlags & MK_CONTROL)
			wKeyState |= DD_CTRL;
		if (nFlags & MK_SHIFT)
			wKeyState |= DD_SHIFT;

		if (wKeyState != m_wLastKeyState)
		{
			m_wLastKeyState = wKeyState;
			bChange = TRUE;
		}
    }

	if (bChange)	
		ChangeCursor();

    // tell the target about the drag
    if (m_target && m_dragInfo)
        m_target->DoDrag(m_dragSlob, m_dragInfo, point);
}

void CDragNDrop::ChangeCursor()
{
	CObList* pList = m_dragSlob->GetContentList();
	BOOL bMulti (pList != NULL && pList->GetCount() > 1);
	
	SetCursor(m_target == NULL ? hcurNoDrop : 
		(m_dragSlob->GetCursor(IsCopy() ? (bMulti ? DDC_COPY_MULTI : DDC_COPY_SINGLE) : 
			(bMulti ? DDC_MULTI : DDC_SINGLE))));
}

void CDragNDrop::AbortDrop()
{
    TRACE( "ABORTING Drag-Drop!\n" );

    // the following code restores the trackers on the source.
        
    if (m_dragInfo != NULL)
        m_dragInfo->Hide();

    CSlobWnd* pSrcWnd = m_dragSlob->GetSlobWnd();
	if (pSrcWnd == NULL)
		pSrcWnd = GetActiveSlobWnd();

    if (pSrcWnd != NULL)
    {
        pSrcWnd->OnSelChange();
		pSrcWnd->ShowSprites();
    }

    m_target= NULL;

    ::SetCursor(hcurArrow);
}

void CDragNDrop::IdleUpdate()
{
    // don't do anything if we don't have a drag slob or if
    // we haven't processed a mouse move yet, or this is not a left
	// mouse drag.

    if ((m_dragSlob != NULL) && GetKeyState(VK_LBUTTON) < 0 &&
		(m_target != (CSlob*)(-1)) && (m_target != NULL))
    {
        WORD wKeyState = 0;
		if (GetKeyState(VK_CONTROL) < 0)
			wKeyState |= DD_CTRL;
		if (GetKeyState(VK_SHIFT) < 0)
			wKeyState |= DD_SHIFT;

        if (wKeyState != m_wLastKeyState)
        {
            m_wLastKeyState = wKeyState;
			ChangeCursor();
        }
    }
}


