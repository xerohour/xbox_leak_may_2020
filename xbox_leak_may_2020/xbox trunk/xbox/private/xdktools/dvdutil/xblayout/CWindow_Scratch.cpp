// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CScratch.cpp
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CWindow_Scratch::CWindow_Scratch(DWORD dwWindowCreationFlags, CWindow *pwindowParent) : CWindow("Scratch", dwWindowCreationFlags, pwindowParent->m_hinst)
{
    m_pwindowParent = pwindowParent;
}

bool CWindow_Scratch::Init()
{
    // Set our menu name (necessary for base class initialization)
    strcpy(m_szMenuName, "");

    // Do base class initialization
    if(!CWindow::Init(m_pwindowParent))
        return false;

    // Create our listview
    m_plistview = new CListView();
    if (!m_plistview)
        FatalError(E_OUTOFMEMORY);
    if (!m_plistview->Init(this, false, false))
        return false;

    // Set the window's initial size and positioned if it was previously known
    if (m_dwWindowCreationFlags & FLAG_WINDOW_POS_LASTKNOWN)
        SetLastKnownPos();
   
    SetCaption("Unplaced files");

    m_fVisible = false; 

    return true;
}

CWindow_Scratch::~CWindow_Scratch()
{
    DestroyWindow(m_hwnd);
}

void CWindow_Scratch::Uninit()
{
    delete m_plistview;
}

// save out contents to the specified file
bool CWindow_Scratch::PersistTo(CFile *pfile)
{
    CObjList olTemp;
    CObj_Group *pogPrev = NULL;
    
    // Copy our contents to the temporary object list
    int iItem = -1;
    while ((iItem = ListView_GetNextItem(m_plistview->m_hwnd, iItem, 0)) != -1)
    {
        CObject *poCur = m_plistview->GetObjectFromIndex(iItem);
        poCur->RemoveFromList();

        if (poCur->m_pog && poCur->m_pog != pogPrev)
        {
            poCur->m_pog->RemoveFromList();
            olTemp.AddToTail(poCur->m_pog);
            pogPrev = poCur->m_pog;
        }

        olTemp.AddToTail(poCur);
    }

    // Persist the temporary object list
    olTemp.PersistTo(pfile);

    olTemp.Clear();
	return true;
}

CObject *CWindow_Scratch::FindObjectByFullFileName(char *szName)
{
    int iItem = -1;
    while ((iItem = ListView_GetNextItem(m_plistview->m_hwnd, iItem, 0)) != -1)
    {
        CObject *poCur = m_plistview->GetObjectFromIndex(iItem);

        if (!lstrcmpi(szName, poCur->m_szFullFileName))
            return poCur;
    }

	return NULL;
}


bool CWindow_Scratch::CreateFrom(CFile *pfile)
{
    // Create and populate a temporary list with the objects from the file
    CObjList olTemp;
    olTemp.CreateFrom(pfile);

    // Populate our list with the objects in the temporary list
    CObject *poCur;
    while ((poCur = olTemp.GetOutside()) != NULL)
    {
        poCur->RemoveFromList();
        m_plistview->AddObject(poCur);
    }

	return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Scratch::DerivedWndProc
// Purpose:   Handle messages that are specific to this derived window.  Most common messages are
//            already automatically sent to the appropriate "OnFOO" functions.
// Arguments: uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CWindow_Scratch::DerivedWndProc(UINT uMsg, WPARAM wparam, LPARAM lparam) 
{
    switch (uMsg) 
    {
    case WM_DRAWITEM:
        return m_plistview->HandleDrawItem((LPDRAWITEMSTRUCT)lparam);

    case WM_CLOSE:
        // User clicked on the 'close' button on the scratch window.  Tell the
        // main window to hide us
        SendMessage(m_pwindowParent->m_hwnd, WM_COMMAND, ID_VIEW_SCRATCHAREA, 0);

        // Return zero so that we tell the OS "we handled the message; DON'T
        // close the window"
        return 0;
    }

    // Let windows handle the rest
    return DefWindowProc(m_hwnd, uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Scratch::OnSize
// Purpose:   Called when the user resizes the window.  We resize the controls to fill the new
//            window size
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Scratch::OnSize()
{
    // Get the new dimensions of the window
    GetClientRect(m_hwnd, &m_rc);

    // Resize the list control to fit the window
    m_plistview->MoveTo(0, 0, (m_rc.right - m_rc.left), m_rc.bottom - m_rc.top);
}

void CWindow_Scratch::ToggleView()
{
    m_fVisible = !m_fVisible;
    ShowWindow(m_hwnd, m_fVisible ? SW_SHOW : SW_HIDE);
}

void CWindow_Scratch::AddObject(CObject *pobj)
{
    m_plistview->AddObject(pobj);
}

void CWindow_Scratch::FinishedDrag(CListView *plvSource, POINT pt)
{
    // let the main window handle this
    MapWindowPoints(m_hwnd, m_pwindowParent->m_hwnd, &pt, 1);
    m_pwindowParent->FinishedDrag(plvSource, pt);
}

void CWindow_Scratch::DragMove(POINT pt)
{
    // Let the main window handle this.  'pt' is in terms of our coordinates,
    // so first convert to the main window's coordinates
    MapWindowPoints(m_hwnd, m_pwindowParent->m_hwnd, &pt, 1);
    m_pwindowParent->DragMove(pt);
}

void CWindow_Scratch::EraseAllSelectionBars()
{
    m_pwindowParent->EraseAllSelectionBars();
}

bool CWindow_Scratch::HandleFileRemoval(CObjList *pol)
{
    // for each file moved off of the scratch window, the file's folder is also
    // moved.  This is done without warning the user.
    CObject *poCur = pol->GetInside();
    while (poCur)
    {
        // Ensure the current object's parent folder is not in the listview.
        // If it is, then remove it from the listview and add it to the list of
        // objects to move (immediately after poCur, so that the folder is then
        // subsequently checked, gauranteeing recursiveness).
        int iFolder = m_plistview->GetIndexFromObject(poCur->m_pof);
        if (iFolder != -1)
        {
            // Folder is in the listview.  remove it.
            poCur->m_pof->RemoveFromList();
            m_plistview->RemoveObject(poCur->m_pof);
            pol->InsertAfter(poCur->m_pof, poCur);
        }
        poCur = poCur->m_poOuter;
    }
    return true;
}  
void CWindow_Scratch::SetGroupable(bool fGroupable)
{
    // We don't allow grouping or ungrouping in the scratch window
    m_pwindowParent->SetGroupable(fGroupable);
}

void CWindow_Scratch::SetUngroupable(bool fUngroupable)
{
    // We don't allow grouping or ungrouping in the scratch window
    m_pwindowParent->SetUngroupable(fUngroupable);
}


bool CWindow_Scratch::InsertObjectList(CObjList *polToInsert)
{
    int iObject = m_plistview->m_iDropPoint;
    CObject *poToInsert = polToInsert->GetInside();
    while (poToInsert)
    {
        CObject *poNext = polToInsert->GetNextOuter();
        polToInsert->Remove(poToInsert);
        if (poToInsert->GetType() != OBJ_GROUP)
        {
            m_plistview->AddObjectAtIndex(poToInsert, iObject);
            iObject++;
        }
        poToInsert = poNext;
    }
    return true;
}

bool CWindow_Scratch::OnNotify(WPARAM wparam, LPARAM lparam)
{
    return m_pwindowParent->OnNotify(wparam, lparam);
}
