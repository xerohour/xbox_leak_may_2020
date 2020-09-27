// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CWindow_Main_Drag.cpp
// Contents:  
// Revisions: 12-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

bool g_fDontWarnFolderMoveToScratch = false;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  VerifyMoveFolderProc
// Purpose:   Message handler for 'Verify folder move' dialog. This function is
//            automatically called by Windows whenever a message needs to be
//            sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK VerifyMoveFolderProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return true;

	case WM_COMMAND:
        g_fDontWarnFolderMoveToScratch = (wParam == IDC_YESSTOPASKING);
		EndDialog(hDlg, wParam);
		return false;
	}
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::FinishedDrag
// Purpose:   The user finished a drag operation.  Move the selected objects.
// Arguments: plvSource     -- The list view that the drag operation began on.
//            pt            -- The point at which the mouse button was released
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::FinishedDrag(CListView *plvSource, POINT pt)
{
    int nSrcListView;
    bool fInsertionSucceeded;
    CListView *plvDest;
    CObjList olToMove;
    CObject *poInsertAt;
    bool fWarned = false;

    // Determine which listview is the destination (could also be the scratch listview!)
    int nDestListView = WhichListViewUnderPt(pt);
    if (nDestListView == -1)
        return; // invalid drop spot; ignore

    switch(nDestListView)
    {
    case 0: plvDest = m_rgplv[LV_LAYER0];  break;
    case 1: plvDest = m_rgplv[LV_LAYER1];  break;
    case 2: plvDest = m_rgplv[LV_SCRATCH]; break;
    }

    if (plvSource == m_rgplv[LV_LAYER0])
        nSrcListView = 0;
    else if (plvSource == m_rgplv[LV_LAYER1])
        nSrcListView = 1;
    else
        nSrcListView = 2;

    // If drag/dropped to different lists, then clear out any previous selection
    // state in the destination list.
    if (plvDest != plvSource)
        plvDest->ClearSelected();

    // Did the user try to drop the selected files back onto themselves?  If so,
    // then don't do anything.
    if ((plvDest == plvSource) && plvDest->CheckDroppedOnSelf())
    {
        plvDest->ClearSelected();
        plvDest->ErasePreviousSelectionBar();
        return;
    }

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

    // Disable updating in the lists until all the movement has been completed.
    SendMessage(plvDest->GetHwnd(),   WM_SETREDRAW, 0, 0);
    SendMessage(plvSource->GetHwnd(), WM_SETREDRAW, 0, 0);

    // Get the list of selected objects in the listview.  This is so that we
    // can be sure the objects remain selected after removal/re-insertion.
    m_rgplv[LV_LAYER0]->StoreSelectedObjList();
    m_rgplv[LV_LAYER1]->StoreSelectedObjList();
    m_rgplv[LV_SCRATCH]->StoreSelectedObjList();

    // Tell the destination listview that an object was dropped on it.  This will
    // let the listview determine what object the selection was dropped on.
    plvDest->ObjectDropped();

    // Take a snapshot of the workspace in case we abort part-way through the insert
    m_pws->QueueSnapspot();

    // Get the list of selected objects.  This will remove the objects from
    // the list and layer (if not in scratch window).  The objects in polToMove
    // will be in top-to-bottom order based on visibility in the listview. This
    // is so that insertion can be independent of the order of the source list.
    plvSource->GetListOfSelectedObjects(&olToMove);

    // If the destination is the scratch window, and the source isn't, then we
    // need to check if any folders are getting moved.  If they are, then we
    // need to warn the user that all of the folder's files will also be moved
    // (recursively).  Note that the files could be on a different layer than
    // the folder...
    if (nDestListView == 2 && nSrcListView != 2)
    {
        CObject *poCur = olToMove.GetInside();
        while (poCur)
        {
            if (poCur->GetType() == OBJ_FOLDER)
            {
                // Has the user already checked the "don't warn me again" checkbox?
                if (!g_fDontWarnFolderMoveToScratch && !fWarned)
                {
                    // Pop up the messagebox
                    if (DialogBox(m_hinst, (LPCTSTR)IDD_VERIFYMOVEFOLDER, m_hwnd, (DLGPROC)VerifyMoveFolderProc) == IDC_NO1)
                    {
                        // User cancelled out of the removal.
                        fInsertionSucceeded = false;
                        goto warned;
                    }
                    fWarned = true;
                }

                // Remove all of the objects which belong to this folder, and add
                // them to the list of objects to move.
                m_pws->RemoveChildObjects((CObj_Folder*)poCur, &olToMove);
            }

            // Note that the function above may have added folders to the list
            // of objects to remove.  The function was not recursive because
            // we will subsequently step over the folders as we continue
            // enumerating the list (which now includes the folders to search).
            poCur = poCur->m_poOuter;
        }
        m_pws->CompactLayer(0);
        m_pws->CompactLayer(1);
    }

    // If the source is the scratch window, and the destination isn't, then we
    // need to be sure that, for each file moved off of the scratch window, the
    // file's folder is also moved.
    if (nSrcListView == 2 && nDestListView != 2)
    {
        m_pscratch->HandleFileRemoval(&olToMove);
    }

    // If the insertion point is a security placeholder, then it's possible that
    // the upcoming compactlayer call will move an object PAST the placeholder,
    // thus screwing up our 'poInsertAt' pointer.  So, if it's a placeholder,
    // just step out to the next object.
    poInsertAt = plvDest->m_pobjDropAt;
    if (poInsertAt != (CObject*)INSERT_INSIDEEDGE && poInsertAt != (CObject*)INSERT_OUTSIDEEDGE)
        while (poInsertAt->GetType() == OBJ_SEC)
            poInsertAt = poInsertAt->m_poOuter;
            
    // After removing the selected objects from their source layer (if not scratch
    // window), we need to compact the layer towards the outside of the disk
    if (nSrcListView != 2)
        m_pws->CompactLayer(nSrcListView);
    
    if (nDestListView == 2)
    {
        // Add objects to scratch window
        fInsertionSucceeded = m_pscratch->InsertObjectList(&olToMove);
    }
    else
    {
        // Add the objects to the dvd on layer 'nDestListView' at the specified
        // location.
        fInsertionSucceeded = m_pws->AddObjectsToLayer(&olToMove, nDestListView, poInsertAt);
        if (fInsertionSucceeded)
            fInsertionSucceeded = m_pws->CompactLayer(nDestListView);
    }

    if (fInsertionSucceeded)
    {
        // The insertion succeeded, so update the window caption bar appropriately.
        m_pws->SetModified();
        m_pws->UpdateWindowCaption();
    }
    else
    {
        // Warn the user that the move couldn't be performed
        MessageBoxResource(m_hwnd, IDS_ERR_MOVE, IDS_ERR_MOVE_CAPTION, MB_ICONWARNING | MB_APPLMODAL | MB_OK);
    }

warned:

    // Restore the snapshot if we failed the insertion
    if (!fInsertionSucceeded)
        m_pws->DequeueSnapspot();

    // Now that we've moved the objects, update the views onto the layers.
    m_pws->UpdateViews();

    SendMessage(plvDest->GetHwnd(),   WM_SETREDRAW, 1, 0);
    SendMessage(plvSource->GetHwnd(), WM_SETREDRAW, 1, 0);

    // Clear out the selected object list information we stored previously.
    // This function also updates the scroll position to its previous location.
    m_rgplv[LV_LAYER0]->ClearSelectedObjList();
    m_rgplv[LV_LAYER1]->ClearSelectedObjList();
    m_rgplv[LV_SCRATCH]->ClearSelectedObjList();

    SetFocus(plvDest->GetHwnd());

    // Tell the listviews that they should go ahead and refresh themselves now.
 //   SendMessage(plvDest->GetHwnd(),   WM_SETREDRAW, 1, 0);
   // SendMessage(plvSource->GetHwnd(), WM_SETREDRAW, 1, 0);

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  Window_Main::WhichListViewUnderPt
// Purpose:   Returns which listview (if any) is under the specified point.
// Arguments: pt            -- The point at which the cursor is currently
//                             located.  Specified in our coordinate system.
// Return: 0 = listview0, 1 = listview1, 2 = scratch window's listview, -1 = no listview   
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CWindow_Main::WhichListViewUnderPt(POINT pt)
{
    RECT rc;

    // Check if the specified point is over the scratch window's listview control
    if (m_pscratch->m_fVisible)
    {
        // Convert point from our coordinates to the scratch window's
        POINT ptScratch = pt;
        MapWindowPoints(m_hwnd, m_pscratch->m_hwnd, &ptScratch, 1);
        m_pscratch->GetChildControlRect(m_rgplv[LV_SCRATCH]->m_hwnd, &rc);
        if (PtInRect(&rc, ptScratch))
            return 2;
    }

    if (m_nCurLayerView != 1)
    {
        GetChildControlRect(m_rgplv[LV_LAYER0]->m_hwnd, &rc);
        if (PtInRect(&rc, pt))
            return 0;
    }

    if (m_nCurLayerView != 0)
    {
        GetChildControlRect(m_rgplv[LV_LAYER1]->m_hwnd, &rc);
        if (PtInRect(&rc, pt))
            return 1;
    }
    return -1;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::DragMove
// Purpose:   Called in the middle of a drag operation.  Updates the selection
//            bar in the appropriate list control(s).
// Arguments: pt            -- The point at which the cursor is currently
//                             located.  Specified in our coordinate system.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::DragMove(POINT pt)
{
    POINT ptScratch;

    // Check if the specified point is over one of our listview controls
    switch(WhichListViewUnderPt(pt))
    {
    case 0:
        // Dragging over listview 0
        m_rgplv[LV_LAYER0]->DrawInsertionBar(pt);
        m_rgplv[LV_LAYER1]->ErasePreviousSelectionBar();
        m_rgplv[LV_SCRATCH]->ErasePreviousSelectionBar();

        SetCursor(LoadCursor(m_hinst, MAKEINTRESOURCE(IDC_MOVECURSOR)));
        break;

    case 1:
        // Dragging over listview 1
        m_rgplv[LV_LAYER1]->DrawInsertionBar(pt);
        m_rgplv[LV_LAYER0]->ErasePreviousSelectionBar();
        m_rgplv[LV_SCRATCH]->ErasePreviousSelectionBar();
        SetCursor(LoadCursor(m_hinst, MAKEINTRESOURCE(IDC_MOVECURSOR)));
        break;

    case 2:
        // Dragging over scratch window's list view
        m_rgplv[LV_LAYER0]->ErasePreviousSelectionBar();
        m_rgplv[LV_LAYER1]->ErasePreviousSelectionBar();

        // Convert point from our coordinates to the scratch window's
        ptScratch = pt;
        MapWindowPoints(m_hwnd, m_pscratch->m_hwnd, &ptScratch, 1);
        m_rgplv[LV_SCRATCH]->DrawInsertionBar(ptScratch);
        SetCursor(LoadCursor(m_hinst, MAKEINTRESOURCE(IDC_MOVECURSOR)));
        break;

    case -1:
        // Otherwise, set the cursor to 'nodrop'
        m_rgplv[LV_LAYER0]->ErasePreviousSelectionBar();
        m_rgplv[LV_LAYER1]->ErasePreviousSelectionBar();
        m_rgplv[LV_SCRATCH]->ErasePreviousSelectionBar();
        SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_NO)));
        break;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::EraseAllSelectionBars
// Purpose:   Erases the selection bars in all of the listview controls.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::EraseAllSelectionBars()
{
    m_rgplv[LV_LAYER0]->ErasePreviousSelectionBar();
    m_rgplv[LV_LAYER1]->ErasePreviousSelectionBar();
    m_rgplv[LV_SCRATCH]->ErasePreviousSelectionBar();
}
