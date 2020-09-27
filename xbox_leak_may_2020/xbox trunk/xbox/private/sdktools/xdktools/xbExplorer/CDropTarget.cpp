// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     CDropTarget.cpp
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::CDropTarget
// Purpose:   CDropTarget constructor. Sole purpose is to preset the ref count to 1.
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CDropTarget::CDropTarget() : m_cRef(1)
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::QueryInterface
// Purpose:   Obtain a particular interface to this object.
// Arguments: riid          -- The interface to this object being queried for.
//            ppvObject     -- Buffer to fill with obtained interface.
// Return:    S_OK if interface obtained; E_NOINTERFACE otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropTarget::QueryInterface(REFIID riid, void **ppvObject)
{ 
    *ppvObject = NULL;

    if (riid == IID_IUnknown) 
        *ppvObject = this;
    else if (riid == IID_IDropTarget) 
        *ppvObject = static_cast<IDropTarget*>(this);
    else
        return E_NOINTERFACE;

    if (*ppvObject)
    {
        static_cast<LPUNKNOWN>(*ppvObject)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}   

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::AddRef
// Purpose:   Add a reference to this object
// Arguments: None
// Return:    New reference count
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CDropTarget::AddRef()
{
    return (ULONG)InterlockedIncrement(&m_cRef);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::Release
// Purpose:   Subtract a reference to this object.  If the new number of references is zero, then
//            delete the object.
// Arguments: None
// Return:    New reference count.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CDropTarget::Release()
{
    if (0 == InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return (ULONG)m_cRef;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::DragEnter
// Purpose:   Called when an object is dragged into us.  Return what sort of operation we allow
//            given the current key state (ie move, copy, none).
// Arguments: pidataobject      -- The object containing information on all of the files
//            dwKeyState        -- Provides state of the keys and mouse buttons.
//            pt                -- The position of the mouse cursor.
//            pdwEffect         -- On exit, will contain allowed operation.
// Return:    NOERROR to represent valid pdwEffect, E_FAIL otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropTarget::DragEnter(LPDATAOBJECT pidataobject, DWORD dwKeyState, POINTL pt,
                                    LPDWORD pdwEffect)
{
    HRESULT hr;
    bool fSourceXbox = false;

    // We need to track the 'current' data object since CDropTarget::DragOver isn't passed the
    // actual data object.  We NULL it out here in case we determine farther down that no drop
    // is allowed.
    m_pidataobject = NULL;

    // Check to see if the object being dropped is a file (or collection of files).  We only
    // support files passed through CF_HDROP or our own custom format (See CFileContainer for an
    // explanation of why we have our own custom format).
    FORMATETC fe = {(USHORT)g_suXboxFiles, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    hr = pidataobject->QueryGetData(&fe);
    if (hr != S_OK)
    {
        // The data object didn't recognize our custom format, so check if it's a standard CF_HDROP
        // format object.
        fe.cfFormat = CF_HDROP;
        hr = pidataobject->QueryGetData(&fe);
    }
    else
        fSourceXbox = true;
    
    if (hr != S_OK)
    {
        // We don't know what it is...
        *pdwEffect=DROPEFFECT_NONE;
        return NOERROR;
    }
  
    // Return effect flags based on keys.
    if (fSourceXbox)
    {
        // From an xbox, so use same-drive convention
        *pdwEffect=DROPEFFECT_MOVE;    
        if (dwKeyState & MK_CONTROL)     
            *pdwEffect=DROPEFFECT_COPY;
    }
    else
    {
        // From PC, so use cross-drive convention
        *pdwEffect=DROPEFFECT_COPY;    
        if (dwKeyState & MK_SHIFT)     
            *pdwEffect=DROPEFFECT_MOVE;
    }
    
    // Keep a pointer to the data being dragged for CDropTarget::DragOver
    m_pidataobject = pidataobject;

    // Track our reference to the data object.  Must be released when DragLeave occurs
    m_pidataobject->AddRef();

    // Bring our app's window to the top
//    BringWindowToTop(g_hwnd);
//    UpdateWindow(g_hwnd);

    return NOERROR;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::DragOver
// Purpose:   Called whenever the mouse moves over the drag target while dragging.
// Arguments: dwKeyState        -- Provides state of the keys and mouse buttons.
//            pt                -- The position of the mouse cursor.
//            pdwEffect         -- On exit, will contain allowed operation.
// Return:    NOERROR to represent valid pdwEffect, E_FAIL otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropTarget::DragOver(DWORD dwKeyState, POINTL ptl, LPDWORD pdwEffect)    
{
    bool fDropAllowed;
    sSourceInfo si;
    GetDropSourceInfo(m_pidataobject, &si);

    // Check if the particular data was determined to be allowed in the CDropTarget::DragEnter
    // function.  If not, then we don't allow the drop
    if (m_pidataobject == NULL)
        fDropAllowed = false;
    else
    {
        POINT pt;
        pt.x = ptl.x;
        pt.y = ptl.y;

        // Tell the listview to set the temporary drag/drop highlight.  It will handle highlighting
        // of directories and NOT files, and will remove highlight when appropriate.
        bool fValidDropItem = listview.SetDragHighlight(true, &pt);

        // If the cursor is over a 'valid drop item' (ie a folder) then we can drop regardless of source
        if (fValidDropItem)
            fDropAllowed = true;
        else
        {
            // Not over a valid drop item; if *we* are the source of the drag, then no drop is
            // allowed.  If something external is the source, then the drop is allowed
            if (si.fXbox && g_hwnd == si.hwnd)
            {
                // We are dragging files within our own window and are not currently over a folder;
                // therefore no drop is allowed
                fDropAllowed = false;
            }
            else
            {
                // We are dragging files from another window; therefore a drop is allowed
                // (regardless of whether the source is an Xbox or PC).
                fDropAllowed = true;
            }
        }
    }

    if (fDropAllowed)
    {
        // Return effect flags based on keys.
        if (si.fXbox)
        {
            // From an xbox, so use same-drive convention
            *pdwEffect=DROPEFFECT_MOVE;    
            if (dwKeyState & MK_CONTROL)     
                *pdwEffect=DROPEFFECT_COPY;
        }
        else
        {
            // From PC, so use cross-drive convention
            *pdwEffect=DROPEFFECT_COPY;    
            if (dwKeyState & MK_SHIFT)     
                *pdwEffect=DROPEFFECT_MOVE;
        }
    }
    else
        *pdwEffect=DROPEFFECT_NONE;

    return NOERROR;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::DragLeave
// Purpose:   Called whenever the mouse cursor leaves the drag target while dragging.
// Arguments: None.
// Return:    NOERROR.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropTarget::DragLeave()  
{
    // If we had a valid data object dragged in, then release our reference to it now.
    if (m_pidataobject != NULL)       
    { 
        m_pidataobject->Release();

        // Tell the listview to unhighlight the temporary drag highlight.
        listview.SetDragHighlight(false, NULL);
    } 
    return NOERROR;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropTarget::Drop
// Purpose:   Called when the user drops data onto us.
// Arguments: pidataobject      -- The object containing information about all of the files
//            dwKeyState        -- Provides state of the keys and mouse buttons.
//            pt                -- The position of the mouse cursor.
//            pdwEffect         -- On exit, will contain performed operation.
// Return:    NOERROR to represent valid pdwEffect, E_FAIL otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropTarget::Drop(LPDATAOBJECT pidataobject, DWORD dwKeyState, POINTL pt,
                               LPDWORD pdwEffect)  
{
    HRESULT   hr = S_OK;
    STGMEDIUM stm;
    char      *szNextSourcePathFile;
    char szBuf[1024], szBuf2[1024];
    bool fContinue;

    // Clear out our performed effect in case things fail below
    *pdwEffect=DROPEFFECT_NONE;

    // If we determined earlier that the data was not valid for us, then return failure now.
    if (pidataobject == NULL)
        return ResultFromScode(E_FAIL);

    g_progress.StartThread();

    sSourceInfo sourceinfo;
    GetDropSourceInfo(pidataobject, &sourceinfo);

    // Get the file names from the dropobject and do the copy.  There are two possible sources of
    // the data; us (xbExplorer), and not us (Shell32, other drag sources).  In the first case,
    // we go through our own format since our CFileContainer does not support CF_HDROP (See that
    // object definition for the reason why).  In the second case, we go through CF_HDROP.  In
    // either case, the format of the data matches CF_HDROP's definition (See MSDN), so we can
    // continue on in the same code path...

    // First, check if the source is one of our custom jobbies (this could be the user dragging
    // from one place to another in the same xbExplorer, or dragging files from xbExplorer A
    // to xbExplorer B).
    FORMATETC fe = {(USHORT)g_suXboxFiles, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    hr = pidataobject->GetData(&fe, &stm);
    if (hr != S_OK)
    {
        // The data object didn't recognize our custom format, so it must be a standard CF_HDROP
        // data object.
        fe.cfFormat = CF_HDROP;
        hr = pidataobject->GetData(&fe, &stm);
    }
    if (hr == S_OK && stm.hGlobal != NULL)
    {
        DROPFILES *pdf = (DROPFILES*)GlobalLock(stm.hGlobal);
        bool fSourceXbox = false;
        
        // Copy each of the files over
	    char szDestFilePath[MAX_PATH];

        // If we are the source and target then it's an xbox->xbox copy.
        // Else it's a PC->xbox copy.
        //     XBox->PC copies are handled in the CFileContainer object
        //     We handle xbox->xbox and PC->xbox here...

//        if (sourceinfo.fXbox && MyStrcmp(sourceinfo.szName, g_TargetXbox))
        if (sourceinfo.fXbox && g_hwnd != sourceinfo.hwnd)
        {
            // We don't allow copying from one xbox to another yet
            LoadString(hInst, IDS_XTOX_COPY_NOT_ALLOWED, szBuf, MAX_PATH);
            LoadString(hInst, IDS_UNSUPPORTED_FEATURE, szBuf2, MAX_PATH);

            MessageBox(g_hwnd, szBuf, szBuf2, MB_ICONWARNING | MB_OK);
            goto done;
        }
        // Check to see if the files were dropped onto a directory
        char szFolderName[MAX_PATH];
        bool fDroppedOnDir = listview.GetDragItemName(szFolderName);

        // First check to see if the user simply dragged a collection of objects around and ended
        // up dropped them back onto one of the folders in that collection.  In that case, do
        // nothing.  We can tell if this happened by looking at the list of dropped objects and
        // checking to see if the destination folder contains any of them.
        // This only happens if the source was an xbox, and the files were dropped onto a specific
        // directory.
        if (fDroppedOnDir && sourceinfo.fXbox)
        {
            szNextSourcePathFile = (char*)(pdf) + pdf->pFiles;
            while (*szNextSourcePathFile)
            {
                char *pszSource;

                // UNDONE: Note - current assumption is that Xbox HD requires ASCII chars...
                if (pdf->fWide)
                {
                    // Convert from wide to single byte
                    pszSource = WideToSingleByte((WCHAR*)szNextSourcePathFile);
                }
                else
                    pszSource = szNextSourcePathFile;

                wsprintf(szDestFilePath, "%s%s", g_CurrentDir, szFolderName);
                if (!MyStricmp(szDestFilePath, pszSource))
                {
                    // The user clicked on a file/folder on the Xbox, dragged it around, and finally
                    // dropped it *back* onto the same file/folder.  Ignore everything!
                    hr = E_UNEXPECTED;
                    goto done;
                }
                if (pdf->fWide)
                    szNextSourcePathFile += (MyStrlen(pszSource)+1)*2;
                else
                    szNextSourcePathFile += MyStrlen(pszSource)+1;
            }
        }

        // If here, then we're good to copy/move...
        bool fQuery = true;
        szNextSourcePathFile = (char*)(pdf) + pdf->pFiles;
        while (*szNextSourcePathFile)
        {
            char *pszSource;

            if (pdf->fWide)
            {
                // Convert from wide to single byte
                pszSource = WideToSingleByte((WCHAR*)szNextSourcePathFile);
            }
            else
                pszSource = szNextSourcePathFile;
            
            if (fDroppedOnDir)
            {
                // Dropped onto a directory! Combine its name into the destination path
                if (sourceinfo.fXbox)
                    wsprintf(szDestFilePath, "%s%s\\%s", g_CurrentDir, szFolderName,
                             pszSource + MyStrlen(g_szCopyDir));
                else
                    wsprintf(szDestFilePath, "%s%s\\%s", g_CurrentDir, szFolderName,
                             strrchr(pszSource,'\\') + 1);
            }
            else
            {
                // Dropped directly into us
                if (sourceinfo.fXbox)
                    wsprintf(szDestFilePath, "%s%s", g_CurrentDir, pszSource + MyStrlen(g_szCopyDir));
                else
                    wsprintf(szDestFilePath, "%s%s", g_CurrentDir, strrchr(pszSource,'\\') + 1);
            }

            // Perform the actual copy.  The 'Copy' command will take care of directories auto-
            // matically (ie creating and recursively copying them).
            // NOTE: This code is only called in the Xbox->Xbox or PC->Xbox cases.
            bool fDoMove = true;
            if ((sourceinfo.fXbox  && (dwKeyState & MK_CONTROL)) ||
                (!sourceinfo.fXbox && !(dwKeyState & MK_SHIFT)))
                fDoMove = false;

            fContinue = (xbfu.CopyMoveToXbox(pszSource, szDestFilePath, &fQuery, true, sourceinfo.fXbox, true, fDoMove) == S_OK);
            if (!fContinue)
                break;
            
            if (pdf->fWide)
                szNextSourcePathFile += (MyStrlen(pszSource)+1)*2;
            else
                szNextSourcePathFile += MyStrlen(pszSource)+1;
        }

        GlobalUnlock(stm.hGlobal);
    }
    else
        hr = E_UNEXPECTED;
done:
    
    ReleaseStgMedium(&stm);
    
    // Return effect flags based on keys.
    if (hr != S_OK)
        *pdwEffect = DROPEFFECT_NONE;
    else if (sourceinfo.fXbox)
    {
        // From an xbox, so use same-drive convention
        *pdwEffect=DROPEFFECT_MOVE;    
        if (dwKeyState & MK_CONTROL)     
            *pdwEffect=DROPEFFECT_COPY;
    }
    else
    {
        // From PC, so use cross-drive convention
        *pdwEffect=DROPEFFECT_COPY;    
        if (dwKeyState & MK_SHIFT)     
            *pdwEffect=DROPEFFECT_MOVE;
    }

    g_progress.EndThread();

    // Refresh the listview with the new files
	listview.SortListView(-1, FALSE);

    DragLeave();

    /* Make sure we have focus if the drop succeeded */
    if(SUCCEEDED(hr))
        SetForegroundWindow(g_hwnd);

    return hr;
}


void GetDropSourceInfo(IDataObject *pidataobject, sSourceInfo *psourceinfo)
{
    STGMEDIUM   stm;

    // Check to see if the data object is originating from an xbFileMgr (us or other)
    FORMATETC fe = {(USHORT)g_suXboxObject, 0, DVASPECT_CONTENT, (USHORT)-1,TYMED_HGLOBAL};

    if (pidataobject->GetData(&fe, &stm) == S_OK)
    {
        sSourceInfo *psi = (sSourceInfo *)GlobalLock(stm.hGlobal);
        
        // Copy the info
        *psourceinfo = *psi;

        GlobalUnlock(stm.hGlobal);
    }
    else
    {
        // the object didn't recognize our 'g_suXboxObject' request, so it's therefore not an
        // xbox!
        psourceinfo->fXbox = false;
    }

    // Release the storage memory allocated by the GetData call above.
    ReleaseStgMedium(&stm);
}
