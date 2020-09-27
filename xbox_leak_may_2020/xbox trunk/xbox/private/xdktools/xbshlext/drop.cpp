/*++

Copyright (c) Microsoft Corporation

Module Name:

    drop.cpp

Abstract:

    Implementation of CXboxDropTarget

Environment:

    Windows 2000 and Later 
    User Mode
    ATL

Revision History:
    
    04-03-2001 : created (mitchd)

--*/

#include "stdafx.h"

//--------------------------------------------------------------------
//  Implemenation of CXboxDropTarget
//--------------------------------------------------------------------

HRESULT 
CXboxDropTarget::Create(
    HWND hWndParent,
    UINT cidl,
    LPCITEMIDLIST * apidl,
    CXboxFolder *pParent,
    IDropTarget **ppDropTarget
    )
/*++
 Routine Description:
  Creates an IDropTarget for THE selected volume or directory.

 Arguments:

  cidl         - number of ItemIDs selected (better be zero -meaning the parent- or one.
  apidl        - array of selected ItemIDs.
  pParent      - parent of the selected items (unless cidl is zero).
  ppDropTarget - [OUT] get the drop target on success

 Return Value:

  S_OK on success.
  E_NOINTERFACE if the object type is not supported.
  E_OUTOFMEMORY is always possible.
--*/
{
    HRESULT      hr;
    ULONG        ulShellAttributes;
    CXboxFolder *pSelection = NULL;

    //
    //  Assume failure until success
    //
    hr = E_NOINTERFACE;
    *ppDropTarget = NULL;
    
    //
    //  Only proceed if only one item is in the selection.
    //
    if(cidl < 2)
    {
        //
        //  Ask the pParent if the selected items are valid drop targets.
        //  Since the parent is part of this code module we can trust it.
        //
        ulShellAttributes = SFGAO_DROPTARGET;
        hr = pParent->GetAttributesOf(cidl, apidl, &ulShellAttributes);
    }


    //
    //  Make a clone
    //

    if(SUCCEEDED(hr) && ulShellAttributes)
    {
        hr = pParent->CloneSelection(cidl, apidl,&pSelection);
    }

    //
    //  Create the object
    //

    if(SUCCEEDED(hr))
    {
        CComObject<CXboxDropTarget> *pDropTarget;
        hr = CComObject<CXboxDropTarget>::CreateInstance(&pDropTarget);
        if(SUCCEEDED(hr))
        {
            pDropTarget->m_pSelection = pSelection;
            pDropTarget->m_hWnd = hWndParent;
            hr = pDropTarget->QueryInterface(IID_PPV_ARG(IDropTarget, ppDropTarget));
            _ASSERT(SUCCEEDED(hr));
        } else
        {
            pSelection->Release();
        }
    }
    return hr;
}

HRESULT
CXboxDropTarget::DragEnter(
    IDataObject * pDataObject,
    DWORD grfKeyState,
    POINTL pt,
    DWORD * pdwEffect
    )
/*++
  Routine Description:
    pDataObject    
--*/
{
    HRESULT hr;
    hr = S_OK;
    
    //
    //  Assume we cannot drop until we learn otherwise.
    //
    *pdwEffect = DROPEFFECT_NONE;

    if(SetDefaultEffect(pDataObject))
    {
        //
        //  Cache this as Drop will need it, and
        //  gets a bogus value.  This may be updated
        //  in DragOver.
        //
        m_grfKeyState = grfKeyState;
            
        //
        // Clear the right mouse button, so GetEffect doesn't go
        // asking the user.
        //
        grfKeyState &= ~MK_RBUTTON;

        //
        //  Get the effect.
        //
        *pdwEffect = GetEffect(grfKeyState, pt);
    }
    return S_OK;
}

HRESULT
CXboxDropTarget::DragOver(
    DWORD grfKeyState,
    POINTL pt,
    DWORD *pdwEffect
    )
{
    //
    //  Update our cached value of grfKeyState
    //
    m_grfKeyState = grfKeyState;
    
    //
    //  Filter out the right mouse button to prevent
    //  GetEffect from querying the user.
    //
    grfKeyState &= ~MK_RBUTTON;

    //
    //  Get the effect 
    //
    *pdwEffect = GetEffect(grfKeyState, pt);
    return S_OK;
}

HRESULT
CXboxDropTarget::DragLeave()
{
    m_dwDefaultEffect = DROPEFFECT_NONE;
    return S_OK;
}

HRESULT
CXboxDropTarget::Drop(
    IDataObject * pDataObject,
    DWORD grfKeyState,
    POINTL pt,
    DWORD * pdwEffect
    )
/*++

  Routine Description:
    Called to start the actual file transfer associated with a drag\drop operation.

    The steps are:
        1) Figure out the drop effect.
        2) Call the XboxItem's DoFileTransfer.

    Note:
        We ignore the *pdwEffect on input.  Instead we call our GetEffect member
        function.  It looks at grfKeyState and decides the effect or prompts the user
        if appropriate.
--*/
{
    HRESULT hr = S_OK;
    *pdwEffect = DROPEFFECT_NONE;

    if(!m_pSelection)
    {
        return S_OK;
    }

    //
    //  Get the effect.
    //
    SetDefaultEffect(pDataObject);  //This shouldn't be necessary, since it was set on DragEnter, but just to be safe.
    *pdwEffect = GetEffect(m_grfKeyState, pt);

    //
    //  If we need to do a transfer, then do it.
    //
    if(DROPEFFECT_NONE != *pdwEffect)
    {
        CDropOperation *pDropOperation = new CDropOperation(m_pSelection, pDataObject, *pdwEffect, m_hWnd);
        *pdwEffect = pDropOperation->StartTransfer();
    }
   
    return hr;
}

BOOL CXboxDropTarget::SetDefaultEffect(IDataObject *pDataObject)
/*++
  Routine Description:
    SetDefaultEffect sets the effect based only the source and target, not considering
    which mouse button was clicked or which keyboard keys are being held down.

    This routine is intended to set m_dwDefaultEffect.

    This is called on DragEnter.  It is also called on Drop simply as a sanity check, really
    it shouldn't be necessary.  On DragOver we can assume that the source has not changed since DragEnter.

    Basically, we ask CDropOperation what clipformat it would use.  If there is none, then the
    data is not dropable on our target.  If it is CF_XBOXFILEDESCRIPTOR, then the source is an Xbox.
    If the drop source is an Xbox, we need to check for two additional conditions:

    1) The target directory and source directory are the same.  These transfers
    are dissallowed, as that would mean copying files over themselves.  Windows Explorer
    renames such files to Copy Of, but we think that is just silly, the user probably
    didn't mean to do it.  We return DROPEFFECT_NONE to disallow these.

    2) If the target and source directory are different, but on the same machine, we return
    DROPEFFECT_MOVE to indicate that a move is the default drop effect.

    Otherwise, we return DROPEFFECT_COPY as the default drop effect.

  Arguments:
    pDataObject - the data object provided by the source.

  Return Value:
    TRUE   - if m_dwDefaultEffect was set to anything but DROPEFFECT_NONE, the data is droppable.
    FALSE  - m_dwDefault was set to DROPEFFECT_NONE, the data is not droppable.
--*/
{
    // If there is no target yet, there is no drop effect.
    if(!m_pSelection)
    {
        m_dwDefaultEffect = DROPEFFECT_NONE;
        return FALSE;
    }

    CLIPFORMAT clipFormat = CDropOperation::GetDropFormat(pDataObject);
    if(!clipFormat)
    {
        m_dwDefaultEffect = DROPEFFECT_NONE;
        return FALSE;
    }
    
    // It is a copy until we find out otherwise.
    m_dwDefaultEffect = DROPEFFECT_COPY;

    // If the source is an Xbox there is more work
    if(CF_XBOXFILEDESCRIPTOR==clipFormat)
    {
        FORMATETC formatEtc;
        STGMEDIUM stgMedium;

        // Get the source directory.  The most effiecient CF for the job is CF_SHELLIDLIST.  
        // CF_XBOXFILEDESCRIPTOR is specific to this shell extension, and we know that we
        // also implemented CF_SHELLIDLIST, so it must have it.

        formatEtc.cfFormat = CF_SHELLIDLIST;
        formatEtc.ptd = NULL;
        formatEtc.dwAspect = DVASPECT_CONTENT;
        formatEtc.lindex = -1;
        formatEtc.tymed = TYMED_HGLOBAL;
        memset(&stgMedium, 0, sizeof(stgMedium));

        // Still check for error, as we could run out of memory, or something bizare
        // in the error case, dissallow transfers, if this fails something else will
        // later too.
        if(SUCCEEDED(pDataObject->GetData(&formatEtc, &stgMedium)))
        {
           BOOL fSameMachine;
           CIDA *pShellIdList = (CIDA  *)GlobalLock(stgMedium.hGlobal);
           LPITEMIDLIST pidlSourceFolder =  (LPITEMIDLIST)( (BYTE *)pShellIdList + pShellIdList->aoffset[0]);
           LPITEMIDLIST pidlTargetFolder = m_pSelection->GetTargetPidl(NULL);
           // Check the two conditions
           if(CXboxFolder::AreItemsIDsIdentical(pidlSourceFolder, pidlTargetFolder, &fSameMachine))
           {
                m_dwDefaultEffect = DROPEFFECT_NONE;
           } else if(fSameMachine)
           {
                m_dwDefaultEffect = DROPEFFECT_MOVE;
           }
           CPidlUtils::Free(pidlTargetFolder);
           ReleaseStgMedium(&stgMedium);
        } else
        {
            m_dwDefaultEffect = DROPEFFECT_NONE;
        }
    }
    return (DROPEFFECT_NONE==m_dwDefaultEffect) ? FALSE : TRUE;
}

DWORD CXboxDropTarget::GetEffect(DWORD grfKeyState, POINTL pt)
/*++
    Routine Description:
      Consider at the grfKeyState to determine the appropriate drop effect.  Possibly
      query the user if the right mouse button is down.

      m_dwDefaultDrop has previously been set.  It is the default effect for the source
      and target assuming that no keys are down, and the left mouse button was used.
      If it DROPEFFECT_NONE, then the source and target are not compatible, we should
      just leave.
      
      It is based off of CTRL, ALT, and SHFT.  None of these down means m_dwDefaultDrop.
      SHFT alone means DROPEFFECT_MOVE. CTRL alone is DROPEFFECT_COPY.  Any other combination
      of CTRL, ALT or SHFT would mean "create shortcut", but we don't support it, so we just
      leave the default.

      If MK_RBUTTON is pressed then we query the user at (pt) with a pop context menu.  The default
      effect on the menu, is the effect that was yielded from the test in the preceding paragraph.
    
    Parameters:
      grfKeyState - bitmap of keys pressed at time of call to Drop or DragEnter.
      pt          - point were drop is taking place.
   
    Return Value:
      The drop effect that should be performed.

    Caveat:
      DragEnter always clears MK_RBUTTON to avoid prompting the user.
    
--*/
{
    // Start with the default effect.
    DWORD dwEffect = m_dwDefaultEffect;

    // If the default effect was none, then there can be no drop, return none.
    if(DROPEFFECT_NONE == m_dwDefaultEffect) return DROPEFFECT_NONE;

    // Alt being down would be short-cut, which we are doing as default, so only
    // check further if ALT is not down.
    if(!(MK_ALT&grfKeyState))
    {
        //Check if shift button is down
        if(MK_SHIFT&grfKeyState)
        {
            //shift and control together is short-cut which we are doing as default.
            if(!(MK_CONTROL&grfKeyState))
                dwEffect = DROPEFFECT_MOVE;
        } else
        {
            // control alone (without SHFT or ALT) means copy
            if(MK_CONTROL&grfKeyState)
                dwEffect = DROPEFFECT_COPY;
        }
    }
    
    //
    //  If the right mouse button is pressed display the drop
    //  context menu (copy\move\cancel) default item in the
    //  menu is the result of the above logic.
    //
    if(MK_RBUTTON&grfKeyState)
    {
        dwEffect = DisplayDropPopUp(dwEffect, pt);
    }
    return dwEffect;
}

DWORD CXboxDropTarget::DisplayDropPopUp(DWORD dwEffect, POINTL pt)
/*++
  Routine Description:
    Display the "drop effect" context menu that queries copy/move/cancel.
--*/
{
    DWORD dwReturnEffect = 0;
    HMENU hMainMenu = LoadMenu(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDM_CONTEXT_MENU_DROP));
    if(hMainMenu)
    {
        HMENU hMenu = GetSubMenu(hMainMenu, 0);
        if(hMenu)
        {
            if(SetMenuDefaultItem(hMenu, dwEffect, FALSE)) //The menu item ID is DROPEFFECT_COPY or DROPEFFECT_MOVE
            {
                SetForegroundWindow(m_hWnd);
                dwReturnEffect = TrackPopupMenuEx(hMenu, TPM_NONOTIFY|TPM_RETURNCMD|TPM_LEFTALIGN|TPM_TOPALIGN, pt.x, pt.y, m_hWnd, NULL);
            }
        }
        DestroyMenu(hMainMenu);
    }
    return dwReturnEffect;
}



//---------------------------------------------------------------------------------------------------------
//  Implementation of CDropOperation
//---------------------------------------------------------------------------------------------------------
const FORMATETC CDropOperation::sm_SupportedFormats[] = 
{
    {CF_XBOXFILEDESCRIPTOR, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},
    {CF_HDROP,              NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},
    {CF_FILEDESCRIPTORA,    NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},
    {CF_FILEDESCRIPTORW,    NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
};
const UINT CDropOperation::sm_uSupportedFormatCount = sizeof(CDropOperation::sm_SupportedFormats)/sizeof(FORMATETC);

HRESULT CDropOperation::GetDropData(IDataObject *pDataObject, FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium)
/*++
  Routine Description:
    This static member queries a data object for a supported file transfer format.  It can query only or
    it can retrieve the data.
  Arguments:
   pDataObject - the data object to query.
   pFormatEtc  - [OUT] on output contains the format retrieved.
   pStgMedium  - [IN\OUT] If NULL on input, the QueryGetData is used.  If non-NULL, then it contains the data
                 on output.
 Return Value:
   On success - S_OK.
   Otherwise  - Any error returned from IDataObject::GetQueryData or IDataObject::GetData.
--*/
{
    HRESULT hr;
    UINT uFormatIndex;
    for(uFormatIndex=0; uFormatIndex < sm_uSupportedFormatCount; uFormatIndex++)
    {
        //
        //  Copy the format to the output buffer
        //
        *pFormatEtc = sm_SupportedFormats[uFormatIndex];

        if(pStgMedium)
        {
            hr = pDataObject->GetData(pFormatEtc, pStgMedium);
        } else
        {
            hr = pDataObject->QueryGetData(pFormatEtc);
        }
        if(SUCCEEDED(hr)) break;
    }
    return hr;
}

CLIPFORMAT CDropOperation::GetDropFormat(IDataObject *pDataObject)
/*++
  Routine Description:
    Wraps CDropOperation::GetDropData in the case that we just want to Query.

  Arguments:
    pDataObject - the data object to query.

  Return Value:
    On Success - cfFormat member of FORMATETC for the optimal format.
    Of Failure - 0, no further error information available.

--*/
{
   FORMATETC formatEtc;
   if(SUCCEEDED(CDropOperation::GetDropData(pDataObject, &formatEtc, NULL)))
   {
        return formatEtc.cfFormat;
   }
   return 0;
}


CDropOperation::~CDropOperation()
{
    if(m_pDataObject) m_pDataObject->Release();
    if(m_pSelection) m_pSelection->Release();
}
    
    
DWORD
CDropOperation::StartTransfer()
/*++
  Routine Description:
    Does the Transfer.

  Return Value:
    The effect actually performed.
--*/
{
    DWORD   dwReturn;
    
    HRESULT hr;
    IAsyncOperation *pAsyncOperation;
    IDataObject *pDataObject;
    
    hr = m_pDataObject->QueryInterface(IID_PPV_ARG(IAsyncOperation, &pAsyncOperation));
    if(SUCCEEDED(hr))
    {
        hr = pAsyncOperation->GetAsyncMode(&m_fAsync);
        if(SUCCEEDED(hr)&&m_fAsync)
        {
            hr = pAsyncOperation->StartOperation(NULL);
            pAsyncOperation->Release();
            if(SUCCEEDED(hr))
            {
                //
                //  Pack the IDataObject in a marshalling stream
                //
                hr = CoMarshalInterThreadInterfaceInStream(IID_IDataObject, m_pDataObject, &m_pMarshallingStream);
                if(SUCCEEDED(hr))
                {
                    pDataObject = m_pDataObject;
                    m_pDataObject = NULL;
                    if(SHCreateThread(CDropOperation::ThreadProc, (PVOID)this, CTF_COINIT, NULL))
                    {
                        pDataObject->Release();
                        return DROPEFFECT_NONE;
                    }
                    m_pDataObject = pDataObject;
                    m_pMarshallingStream->Release();
                    m_pMarshallingStream = NULL;
                    hr = HRESULT_FROM_WIN32(GetLastError());
                }
            }
        }
    }

    //
    //  If we are here, either we shouldn't or couldn't do it asynchronously, so trying
    //  doing it synchronously.
    //
    dwReturn = DoTransfer();
    delete this;
    return dwReturn;
}


DWORD CDropOperation::TransferThread()
/*++
  Routine Description:
    The thread routine for doing a background transfer.
    1) Unmarshals the data object across threads.
    2) Calls DoTransfer() to perform the transfer, just as
       if done synchronously.
    3) Get IAsyncOperation interface and calls EndOperation.
    4) Cleans up by deleting this object.
--*/
{
    DWORD dwResult;
    HRESULT hr;
    IAsyncOperation *pAsyncOperation;
    hr = CoGetInterfaceAndReleaseStream(m_pMarshallingStream, IID_PPV_ARG(IDataObject, &m_pDataObject));
    m_pMarshallingStream = NULL;
    if(SUCCEEDED(hr))
    {
        dwResult = DoTransfer();
        hr = m_pDataObject->QueryInterface(IID_PPV_ARG(IAsyncOperation, &pAsyncOperation));
        if(SUCCEEDED(hr))
        {
            pAsyncOperation->EndOperation(S_OK, NULL, dwResult);
            pAsyncOperation->Release();
        }
    }
    delete this;
    return 0;
}

DWORD CDropOperation::DoTransfer()
/*++
  Routine Description:
    The main workhorse routine for performing transfers.  This may be
    called from the main thread to do a synchronous transfer, or may
    be called from CDropOperation::TransferThread.

    1) Get the main clipboard format for the drop: CF_XBOXFILEDESCRIPTOR, CF_HDROP,
       CF_FILEDESCRIPTORA, or CF_FILEDESCRIPTORW.
    2) Put up the ProgressDialog with the message "Calculating the time to [move\copy] the files."
    3) Branch to the correct transfer routine, based on clipboard format.
    4) Stop the Progress Dialog.
    5) Set the performed effect, and the logical effect, etc.
--*/
{
   HRESULT hr;
   FORMATETC formatEtc;

   //
   //  Get the drop format data.
   //
   hr = GetDropData(m_pDataObject, &formatEtc, &m_stgMedium);
   if(FAILED(hr)) return DROPEFFECT_NONE;

   StartProgressDialog();
   
   m_pTargetConnection = m_pSelection->GetXboxConnection();
   m_pTargetConnection->HrUseSharedConnection(TRUE);
   _ASSERTE(m_pTargetConnection);

   //
   //   Call the correct transfer function, cannot use switch\case as
   //   the CF_ constants are runtime constants, not compiler constants.
   //
   if(CF_XBOXFILEDESCRIPTOR==formatEtc.cfFormat)   DoXboxFileGroupDescriptorTransfer();
   else if(CF_HDROP==formatEtc.cfFormat)           DoHDropTransfer();
   else if(CF_FILEDESCRIPTORA==formatEtc.cfFormat) DoFileGroupDescriptorATransfer();
   else if(CF_FILEDESCRIPTORW==formatEtc.cfFormat) DoFileGroupDescriptorWTransfer();
   else _ASSERT(FALSE);

   m_pTargetConnection->HrUseSharedConnection(FALSE);
   m_pTargetConnection->Release();
   m_pTargetConnection = NULL;
   
   //
   //  Release the main transfer data.
   //

   ReleaseStgMedium(&m_stgMedium);

   StopProgressDialog();

   DataObjUtil::SetPerformedDropEffect(m_pDataObject, m_dwEffectPerformed);
   DataObjUtil::SetPasteSucceeded(m_pDataObject, m_dwEffectPerformed);
    
   return m_dwEffectPerformed;
}


void CDropOperation::StartProgressDialog()
/*++
  Routine Description:
    Instantiates and starts the progress dialog.  The initial message is always
    a preparing message appropriate to move versus copy.
--*/
{
   
   HRESULT   hr;
   WCHAR     wszBuffer[128];
   WCHAR     wszProgressText[128];
   UINT      uTitleResource;
   UINT      uPrepareResource;
   HINSTANCE hInstance = _Module.GetModuleInstance(); 

   //
   //  Instantiate a progress dialog
   //
   hr = CoCreateInstance(
            CLSID_ProgressDialog,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARG(IProgressDialog, &m_pProgressDialog)
            );
    if(FAILED(hr))
    {
        m_pProgressDialog = NULL;
        return;
    }
    
    //
    //  Choose resources based on copy versus move.
    //
    if(DROPEFFECT_MOVE == m_dwDesiredEffect)
    {
        uTitleResource = IDS_FILE_MOVE_PROGRESS_TITLE;
        uPrepareResource = IDS_FILE_MOVE_PROGRESS_PREPARING;
    } else
    {
        uTitleResource = IDS_FILE_COPY_PROGRESS_TITLE;
        uPrepareResource = IDS_FILE_COPY_PROGRESS_PREPARING;
    }

    //
    //  Set the title
    //
    LoadStringW(hInstance, uTitleResource, wszBuffer, 128);
    m_pProgressDialog->SetTitle(wszBuffer);

    //
    //  Set the animation
    //
    m_pProgressDialog->SetAnimation(hInstance, IDA_COPY);

    //
    //  Start the dialog
    //
    m_pProgressDialog->StartProgressDialog(m_hWnd, NULL, PROGDLG_AUTOTIME, NULL);

    //
    //  Set the cancel message
    //
    LoadStringW(hInstance, IDS_FILE_PROGRESS_CANCEL, wszBuffer, 128);
    m_pProgressDialog->SetCancelMsg(wszBuffer, NULL);
    
    //
    //  See if we can get the progress dialog's window to use as the parent for message box's.
    //  Note that QueryWinodw will preserve m_hParentWnd if it fails, before the call it
    //  was to the same as m_hWnd in the c'tor.  Ideally, if we get the hWnd of the progress dialog
    //  it will solve some focus problems.
    //
    ComUtils::QueryWindow(&m_hParentWnd, m_pProgressDialog);

    //
    //  Put up the prepare message.
    //
    LoadStringW(hInstance, uPrepareResource, wszBuffer, 128);
    m_pProgressDialog->SetLine(1, wszBuffer,FALSE, NULL);


}

void CDropOperation::SetProgressTarget()
{
    if(!m_pProgressDialog) return;

    union
    {
        WCHAR wszTargetText[MAX_PATH];
        char  szTargetPath[MAX_PATH];
    };
    char   szConsoleName[60];
    char   szTargetText[MAX_PATH];
    m_pSelection->GetConsoleName(szConsoleName);
    m_pSelection->GetTargetWireName(szTargetPath, NULL);
    WindowUtils::rsprintf(szTargetText, IDS_FILE_PROGRESS_TARGET, szTargetPath, szConsoleName);
    Utils::CopyAtoW(wszTargetText, szTargetText);
    m_pProgressDialog->Timer(PDTIMER_RESET,NULL);
    m_pProgressDialog->SetLine(2, wszTargetText, TRUE, NULL);
}

void CDropOperation::UpdateCopyProgress(LPSTR pszFileName)
{
    if(!m_pProgressDialog) return;
    WCHAR wszFileName[MAX_PATH];
    Utils::CopyAtoW(wszFileName, pszFileName);
    m_pProgressDialog->SetLine(1, wszFileName, TRUE, NULL);
    m_pProgressDialog->SetProgress64(m_ullBytesCompleted, m_ullBytesTotal);
}

void CDropOperation::StopProgressDialog()
{
    if(!m_pProgressDialog) return;
    m_hParentWnd = m_hWnd;
    m_pProgressDialog->StopProgressDialog();
    m_pProgressDialog->Release();
    m_pProgressDialog = NULL;
}

BOOL CDropOperation::QueryCancel()
{
   if(!m_fCancelled)
   {
       if(!m_pProgressDialog) return FALSE;
       m_fCancelled = m_pProgressDialog->HasUserCancelled();
   }
   return m_fCancelled;
}

inline BOOL IsDirectory(FILEDESCRIPTORA *pFileDescriptor)
{
    return pFileDescriptor->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
}

inline BOOL IsReadOnly(FILEDESCRIPTORA *pFileDescriptor)
{
    return pFileDescriptor->dwFileAttributes&FILE_ATTRIBUTE_READONLY;
}

bool CDropOperation::ProcessConfirmResponse(UINT uDialogResponse, CDropOperation::CONFIRM_FLAGS eConfirmFlag)
{
    switch(uDialogResponse)
    {
        case IDC_XB_YESTOALL:
         m_uConfirmedYesToAll |= eConfirmFlag;
        case IDC_XB_YES:
         return true;
        case IDC_XB_CANCEL:
         m_fCancelled = TRUE;
        case IDC_XB_NOTOALL:
         m_uConfirmedNoToAll |= eConfirmFlag;
        case IDC_XB_NO:
         return false;
    }
    _ASSERT(FALSE);
    return false;
}

bool CDropOperation::ConfirmFolderMove(LPCSTR pszFolderName)
{
    //
    //  The user has previously answered yestoall or notoall
    //  just return yes or no.
    //
    if(m_uConfirmedNoToAll&CDropOperation::ConfirmFlagFolderMove) return false;
    if(m_uConfirmedYesToAll&CDropOperation::ConfirmFlagFolderMove) return true;

    //Ask the USER
    UINT uDlgResult = Dialog::ConfirmReadOnlyMove(m_hParentWnd, pszFolderName, true);
    
    //
    //  Process the user's response.
    //
    return ProcessConfirmResponse(uDlgResult, CDropOperation::ConfirmFlagFolderMove);
}
bool CDropOperation::ConfirmFileMove(LPCSTR pszFileName)
{
    //
    //  The user has previously answered yestoall or notoall
    //  just return yes or no.
    //
    if(m_uConfirmedNoToAll&CDropOperation::ConfirmFlagFileMove) return false;
    if(m_uConfirmedYesToAll&CDropOperation::ConfirmFlagFileMove) return true;

    //Ask the USER
    UINT uDlgResult = Dialog::ConfirmReadOnlyMove(m_hParentWnd, pszFileName,false);

    //
    //  Process the user's response.
    //
    return ProcessConfirmResponse(uDlgResult, CDropOperation::ConfirmFlagFileMove);
}

bool CDropOperation::ConfirmFileReplace(LPCSTR pszTargetWireName, LPSTR pszFileName, WIN32_FILE_ATTRIBUTE_DATA *pSourceFileAttributes, bool *pfOverWrite)
/*++

  Routine Description:
    Called when a file copy fails because the target already exists.  This routine figures out
    when we should blast over of the file.  It queries the user and keeps track of previous queries.
    Finally, if the answer is yes, it blows away the existing target.

--*/
{
    //
    //  Assume true.
    //  
    bool fResult = true;

    // Get the target attributes.
        
    DM_FILE_ATTRIBUTES dmFileAttributes;
    HRESULT hr = m_pTargetConnection->HrGetFileAttributes(pszTargetWireName, &dmFileAttributes);
    if(FAILED(hr))
    {
        *pfOverWrite = false;
        return true;
    }

    //
    //  If the target is a directory, then notify the user that this just isn't going to work.
    //
    if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY)
    {
        LPSTR pszSimpleName = strrchr(pszFileName, '\\');
        if(!pszSimpleName) pszSimpleName = pszFileName;
        if(m_dwFileCount>=2)
        {
            if(IDNO==WindowUtils::MessageBoxResource(m_hParentWnd, IDS_COULDNT_REPLACE_DIR_W_FILE_MULTI, IDS_TRANSFER_FAILED_CAPTION, MB_YESNO|MB_ICONERROR, pszSimpleName))
            {
                m_fCancelled = TRUE;
            }
        } else
        {
            // Kill Progress dialog, since the only file won't transfer anyway.
            StopProgressDialog();
            WindowUtils::MessageBoxResource(m_hParentWnd, IDS_COULDNT_REPLACE_DIR_W_FILE, IDS_TRANSFER_FAILED_CAPTION, MB_OK|MB_ICONERROR, pszSimpleName);
        }
        return false;
    }

    //
    //  If it is top level, i.e. doesn't have '\\' in its path,
    //  we should ask the user.
    //
    if(NULL==strchr(pszFileName, '\\'))
    {
        //
        //  If the user previously said notoall, we can leave now.
        //
        if(m_uConfirmedNoToAll&CDropOperation::ConfirmFlagFolderReplace) return false;

        //
        // If the user has not previously said yestoall, then ask the user.
        //
        if(!(m_uConfirmedYesToAll&CDropOperation::ConfirmFlagFolderReplace))
        {
            UINT uDlgResult;

            //
            //  Fill out the WIN32_FILE_ATTRIBUTE_DATA used by the replace dialog.
            //
            FILETIME ft = {0,0};
            WIN32_FILE_ATTRIBUTE_DATA TargetFileAttributes;
            TargetFileAttributes.dwFileAttributes = dmFileAttributes.Attributes;
            TargetFileAttributes.ftCreationTime = dmFileAttributes.CreationTime;
            TargetFileAttributes.ftLastAccessTime = ft;
            TargetFileAttributes.ftLastWriteTime = dmFileAttributes.ChangeTime;
            TargetFileAttributes.nFileSizeHigh = dmFileAttributes.SizeHigh;
            TargetFileAttributes.nFileSizeLow = dmFileAttributes.SizeLow;
            uDlgResult = Dialog::ConfirmFileReplace(m_hParentWnd, pszFileName, &TargetFileAttributes, pSourceFileAttributes);

            //
            //  Process the user's response.
            //
            fResult = ProcessConfirmResponse(uDlgResult, CDropOperation::ConfirmFlagFolderReplace);
        }
    }

    //
    //  If the answer is in the affirmative, blow away the existing file.
    //
    if(fResult)
    {
        //  Don't worry about failures blowing this stuff away.
        //  Soon enough the actual copy will fail.
        if(dmFileAttributes.Attributes&FILE_ATTRIBUTE_READONLY)
        {
            dmFileAttributes.Attributes &= ~FILE_ATTRIBUTE_READONLY;
            if(!dmFileAttributes.Attributes) dmFileAttributes.Attributes = FILE_ATTRIBUTE_NORMAL;
            hr = m_pTargetConnection->HrSetFileAttributes(pszTargetWireName, &dmFileAttributes);
        }
        hr = m_pTargetConnection->HrDeleteFile(pszTargetWireName, FALSE);
        if(FAILED(hr))
        {
            LPSTR pszSimpleName = strrchr(pszFileName, '\\');
            if(!pszSimpleName) pszSimpleName = pszFileName;
            HandleTransferFailed(IDS_COULDNT_REPLACE_FILE, hr, pszSimpleName);
        }
        *pfOverWrite = true;
    }
    
    return fResult;
}

bool CDropOperation::ConfirmFileReplace(LPCSTR szTargetWireName, FILEDESCRIPTORA *pFileDescriptor, bool *pfOverWrite)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    fileAttributes.dwFileAttributes = pFileDescriptor->dwFileAttributes;
    fileAttributes.ftCreationTime = pFileDescriptor->ftCreationTime;
    fileAttributes.ftLastAccessTime = pFileDescriptor->ftLastAccessTime;
    fileAttributes.ftLastWriteTime = pFileDescriptor->ftLastWriteTime;
    fileAttributes.nFileSizeHigh = pFileDescriptor->nFileSizeHigh;
    fileAttributes.nFileSizeLow = pFileDescriptor->nFileSizeLow;
    return ConfirmFileReplace(szTargetWireName, pFileDescriptor->cFileName, &fileAttributes, pfOverWrite);
}

bool CDropOperation::ConfirmFolderReplace(LPCSTR pszFilenName)
{
    //
    //  The user has previously answered yestoall or notoall
    //  just return yes or no.
    //
    if(m_uConfirmedNoToAll&CDropOperation::ConfirmFlagFolderReplace) return false;
    if(m_uConfirmedYesToAll&CDropOperation::ConfirmFlagFolderReplace) return true;

    //Ask the USER
    UINT uDlgResult = Dialog::ConfirmFolderReplace(m_hParentWnd, pszFilenName);

    //
    //  Process the user's response.
    //
    return ProcessConfirmResponse(uDlgResult, CDropOperation::ConfirmFlagFolderReplace);
}

bool CDropOperation::MakeTargetDirectory(LPCSTR pszTargetWireName, FILEDESCRIPTORA *pFileDescriptor)
/*++
  Routine Description:
    Called to make a directory on the target system.  The directory should have the attributes
    of the source
--*/
{
    DM_FILE_ATTRIBUTES dmFileAttributes;
    dmFileAttributes.Attributes = pFileDescriptor->dwFileAttributes;
    dmFileAttributes.ChangeTime = pFileDescriptor->ftLastWriteTime;
    dmFileAttributes.CreationTime = pFileDescriptor->ftCreationTime;
    dmFileAttributes.SizeHigh = pFileDescriptor->nFileSizeHigh;
    dmFileAttributes.SizeLow = pFileDescriptor->nFileSizeLow;
    strcpy(dmFileAttributes.Name, pFileDescriptor->cFileName);
    return MakeTargetDirectory(pszTargetWireName, &dmFileAttributes);
}

bool CDropOperation::MakeTargetDirectory(LPCSTR pszTargetWireName, DM_FILE_ATTRIBUTES *pSourceFileAttributes)
/*++
  Routine Description:
    Called to make a directory on the target system.  
    
    The new directory should have the attributes of the source directory.
    
    If the directory already exists and it is a top-level directory of the drop target
    (it does not a '\\' character in the pSourceFileAttributes->cFilename) then the
    user should be asked for confirmation.

    If the directory exists and confirmation has been given or wasn't needed, then this
    routine sets the attributes of the existing directory to match those of the source
    file.
--*/
{
    
    HRESULT hr;
    bool fOverWrite = false;
    hr = m_pTargetConnection->HrMkdir(pszTargetWireName);
    
    if(XBDM_ALREADYEXISTS == hr)
    {
        if(0==strchr(pSourceFileAttributes->Name, '\\'))
        {
            if(!ConfirmFolderReplace(pSourceFileAttributes->Name))
            {
                return false;
            }
        }
        fOverWrite = true;
    } else if(FAILED(hr))
    {
        LPCSTR pszSimpleName = strrchr(pszTargetWireName, '\\');
        if(!pszSimpleName) pszSimpleName = pszTargetWireName;
        HandleTransferFailed(IDS_COULDNT_CREATE_TARGET_DIR, hr, pszSimpleName);
        return false;
    }
    
    //  Don't bother with errors, there is nothing we are going to do about it, and 
    //  we won't abort or notify the user if we could not change the folder attributes.
    m_pTargetConnection->HrSetFileAttributes(pszTargetWireName, pSourceFileAttributes);
    // Send out notification that directory was created (or updated if this was an fOverWrite
    LPITEMIDLIST pidl = m_pSelection->GetTargetPidl(pSourceFileAttributes->Name);
    SHChangeNotify(fOverWrite ? SHCNE_ATTRIBUTES : SHCNE_MKDIR, SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
    CPidlUtils::Free(pidl);

    return true;
}


void CDropOperation::DoXboxFileGroupDescriptorTransfer()
{
    HRESULT hr;

    _ASSERTE(TYMED_HGLOBAL == m_stgMedium.tymed);
    PXBOXFILEGROUPDESCRIPTOR pXboxFileGroupDescriptor = (PXBOXFILEGROUPDESCRIPTOR)GlobalLock(m_stgMedium.hGlobal);

    IXboxConnection *pSourceConnection;
    char szTargetWireName[MAX_PATH];
    char szSourceWireName[MAX_PATH];
    char szTempFile[MAX_PATH];
        
    LPSTR pszParse;
    LPSTR pszConsoleName;

    bool fMove = (DROPEFFECT_MOVE == m_dwDesiredEffect);
    bool fOptimizedMove = fMove;  //Assume it is optimized, until we found that it doesn't work.
    bool fOverWrite; //Keeps track of overwrite in for notification purposes

    ULARGE_INTEGER li;
    
    //
    //  Generate a temporary filename, we will recycle throughout.
    //
    if(INVALID_HANDLE_VALUE==WindowUtils::CreateTempFile(szTempFile, false))
    {
        //This is an unexpected and somewhat critical error, the use shouldn't ever see it though, unless their machine
        //is hosed for some other reason.  Nothing sucks more than a cycle of errors when the system is hosed, which is likely
        //what will happen if we continue, so let's kill the progress dialog, and cancel the rest of the transfer too.
        StopProgressDialog();
        m_fCancelled = TRUE;
        WindowUtils::MessageBoxResource(m_hParentWnd, IDS_CREATE_TEMP_FILE_FAILED, IDS_TRANSFER_FAILED_CAPTION, MB_OK|MB_ICONERROR);
        GlobalUnlock(m_stgMedium.hGlobal);

        return;
    }

    //
    //  Figure out how many bytes need to be transfered.
    //
    m_dwFileCount = pXboxFileGroupDescriptor->cItems;
    m_ullBytesCompleted = 0;
    m_ullBytesTotal = 0;    
    UINT index;
    for(index=0; index < pXboxFileGroupDescriptor->cItems; index++)
    {
        m_ullBytesTotal += FILE_PROGRESS_FUDGE_BYTES;  //Add fudge bytes for files and directories
        if(!IsDirectory(pXboxFileGroupDescriptor->fgd+index))
        {
            li.LowPart = pXboxFileGroupDescriptor->fgd[index].nFileSizeLow;
            li.HighPart = pXboxFileGroupDescriptor->fgd[index].nFileSizeHigh;
            m_ullBytesTotal += li.QuadPart;
        }
    }

    //
    //  Check the console names and see if it is the same box
    //

    pszParse = pXboxFileGroupDescriptor->szFolderPath;
    pszConsoleName = szSourceWireName; //USe SoureWireName buffer for the console name.
    while(*pszParse!='\\')
    {
        *pszConsoleName++ = *pszParse++;
    }
    *pszConsoleName = '\0';
    m_pSelection->GetConsoleName(szTargetWireName);
    
    //
    //  If source and target are the same box, then we 
    //  can use the same connection.
    //
    if(0==_stricmp(szTargetWireName, szSourceWireName))
    {
        pSourceConnection = m_pTargetConnection;
        pSourceConnection->AddRef();
    } else
    //
    //  Otherwise, we must go out and get our connection
    //
    {
        hr = Utils::GetXboxConnection(szSourceWireName, &pSourceConnection);
        if(FAILED(hr))
        {
            HandleTransferFailed(IDS_COULDNT_CONNECT_TO_XBOX, hr, szSourceWireName);
            GlobalUnlock(m_stgMedium.hGlobal);
            return;
        }
        pSourceConnection->HrUseSharedConnection(TRUE);
        //
        //  And we certainly cannot do optimized moves.
        //
        fOptimizedMove = false;
    }
    
    //
    //  Change the message we are going to start copying.
    //
    SetProgressTarget();

    //
    //  Now loop over everything and do the copy
    //

    for(index=0; index < pXboxFileGroupDescriptor->cItems; index++)
    {
        FILEDESCRIPTORA *fileDescriptor = pXboxFileGroupDescriptor->fgd + index;
        
        //
        //  Each time through check for a cancel.
        //
        if(QueryCancel()) break;

        //
        //  Get information about the item
        //
        bool fIsTopLevel = strchr(fileDescriptor->cFileName, '\\') ? false : true;
        
        //
        //  Update Progress Dialog
        //
        UpdateCopyProgress(fileDescriptor->cFileName);

        //For each file we touch (and directory) add back fudge bytes
        m_ullBytesCompleted += FILE_PROGRESS_FUDGE_BYTES;

        //
        //  Get the target wire name.
        //
        m_pSelection->GetTargetWireName(szTargetWireName, fileDescriptor->cFileName);

        //
        //  For directories, we just create the new directories in first pass
        //  even for move operations
        if(IsDirectory(fileDescriptor))
        {
            //
            //  We have to confirm moves.
            //
            if(fMove && fIsTopLevel && IsReadOnly(fileDescriptor))
            {
                if(ConfirmFolderMove(fileDescriptor->cFileName))
                {
                    if(MakeTargetDirectory(szTargetWireName, fileDescriptor))
                    {
                        continue;
                    }
                }
            } else
            {
                if(MakeTargetDirectory(szTargetWireName, fileDescriptor))
                {
                        continue;
                }
            }

            //
            //  If we fell throughm the directory could not be created
            //  for some reason (file operation failed, user didn't confirm,
            //  etc.).  So skip everything under this directory.
            //

            LPSTR pszDirectory = fileDescriptor->cFileName;
            UINT  uDirectoryNameLen = strlen(pszDirectory);
            do
            {
                fileDescriptor->dwFlags = (UINT)-1;
                index++;
                fileDescriptor = pXboxFileGroupDescriptor->fgd + index;
            }while(0==memcmp(pszDirectory, fileDescriptor->cFileName, uDirectoryNameLen));
            index--; //backup one, since it gets incremented at the start of the loop.
            continue;
        }

        //
        //  If we fell through than it is file.
        //
        
        // By the next time we call UpdateCopyProgress we will be done with this file, either copied
        // cancel or failed.  Anyway you slice it, we need to account for those bytes, and here we get the all at once.
        li.LowPart = fileDescriptor->nFileSizeLow;
        li.HighPart = fileDescriptor->nFileSizeHigh;
        m_ullBytesCompleted += li.QuadPart;
        
        //
        //  The user may need to confirm moves.
        //
        if(fMove && fIsTopLevel && IsReadOnly(fileDescriptor))
        {
            if(!ConfirmFileMove(fileDescriptor->cFileName))
            {
                continue;
            }
        }
        //
        //  We will need a source name
        //
        CXboxFolder::GetWireName(szSourceWireName, pXboxFileGroupDescriptor->szFolderPath, fileDescriptor->cFileName);
        
        //
        //  We will need the source attributes.
        //
        DM_FILE_ATTRIBUTES dmFileAttributes;
        dmFileAttributes.Attributes = fileDescriptor->dwFileAttributes;
        dmFileAttributes.ChangeTime = fileDescriptor->ftLastWriteTime;
        dmFileAttributes.CreationTime = fileDescriptor->ftCreationTime;
        dmFileAttributes.SizeHigh = fileDescriptor->nFileSizeHigh;
        dmFileAttributes.SizeLow = fileDescriptor->nFileSizeLow;

        //
        //  If optimized move is selected,
        //  give it a try.
        //
        if(fOptimizedMove)
        {
            
            hr = S_OK;
            if(IsReadOnly(fileDescriptor))
            {
                dmFileAttributes.Attributes &= ~FILE_ATTRIBUTE_READONLY;
                if(!dmFileAttributes.Attributes) dmFileAttributes.Attributes = FILE_ATTRIBUTE_NORMAL;               
                hr = m_pTargetConnection->HrSetFileAttributes(szSourceWireName, &dmFileAttributes);
                dmFileAttributes.Attributes = fileDescriptor->dwFileAttributes;            
            }
            if(SUCCEEDED(hr))
            {
                if(ConfirmFileReplace(szTargetWireName, fileDescriptor, &fOverWrite))
                {
                    hr = m_pTargetConnection->HrRenameFile(szSourceWireName, szTargetWireName);
                } else
                {
                    hr = E_ABORT;
                }
                //
                //  Restore attributes if it was read-only
                //
                if(IsReadOnly(fileDescriptor))
                {
                    m_pTargetConnection->HrSetFileAttributes(szTargetWireName, &dmFileAttributes);
                }
            }
            //
            //  XBDM_MUSTCOPY means that we cannot do an optimized move on
            //  this file.  This means we shouldn't even try on the rest
            //  of the files.
            if(XBDM_MUSTCOPY==hr)
            {
                fOptimizedMove = false;
            } else if(FAILED(hr))
            {
               LPSTR pszSimpleName = strrchr(szTargetWireName, '\\');
               if(!pszSimpleName) pszSimpleName = szTargetWireName;
               HandleTransferFailed(IDS_COULDNT_MOVE_FILE, hr, pszSimpleName);
               continue;
            } else
            {
                // Send out notification that the source was deleted.
                LPITEMIDLIST pidl = m_pSelection->GetSourcePidl(
                                        pXboxFileGroupDescriptor->szFolderPath,
                                        fileDescriptor->cFileName
                                        );
                SHChangeNotify(SHCNE_DELETE,SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
                CPidlUtils::Free(pidl);
                // Send out notification that target was created (or updated if this was an fOverWrite
                pidl = m_pSelection->GetTargetPidl(fileDescriptor->cFileName);
                SHChangeNotify(fOverWrite ? SHCNE_UPDATEITEM : SHCNE_CREATE,SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
                CPidlUtils::Free(pidl);
                continue;
            }
        } //end of optimized move

        //
        //  Copy the source to a local file.
        //
        hr = pSourceConnection->HrReceiveFile(szTempFile, szSourceWireName);
        if(FAILED(hr))
        {
            LPSTR pszSimpleName = strrchr(szSourceWireName, '\\');
            if(!pszSimpleName) pszSimpleName = szSourceWireName;
            HandleTransferFailed(IDS_COULDNT_READ_SOURCE_FILE, hr, pszSimpleName);
            continue;
        }

        //
        //  Copy the temporary file to the target.
        //

        if(ConfirmFileReplace(szTargetWireName, fileDescriptor, &fOverWrite))
        {
           hr = m_pTargetConnection->HrSendFile(szTempFile, szTargetWireName);
           //
           //  Handle success or failure.
           //
           if(SUCCEEDED(hr))
           {
              //The target attributes should match the source attributes.
              m_pTargetConnection->HrSetFileAttributes(szTargetWireName, &dmFileAttributes);
              // Send out notification that target was created (or updated if this was an fOverWrite
              LPITEMIDLIST pidl = m_pSelection->GetTargetPidl(fileDescriptor->cFileName);
              SHChangeNotify(fOverWrite ? SHCNE_UPDATEITEM : SHCNE_CREATE,SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
              CPidlUtils::Free(pidl);
  
              if(fMove)
              {
                HRESULT hrDelete;
                //If read-only, clear the read-only attribute before trying to delete.
                if(IsReadOnly(fileDescriptor))
                {
                    dmFileAttributes.Attributes &= ~FILE_ATTRIBUTE_READONLY;
                    if(!dmFileAttributes.Attributes) dmFileAttributes.Attributes = FILE_ATTRIBUTE_NORMAL;
                    pSourceConnection->HrSetFileAttributes(szSourceWireName, &dmFileAttributes);
                }
                hrDelete = pSourceConnection->HrDeleteFile(szSourceWireName, IsDirectory(fileDescriptor));
                if(SUCCEEDED(hrDelete))
                {
                    // Send out notification that the source was deleted.
                    pidl = m_pSelection->GetSourcePidl(
                                pXboxFileGroupDescriptor->szFolderPath,
                                fileDescriptor->cFileName
                                );
                    SHChangeNotify(SHCNE_DELETE,SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
                    CPidlUtils::Free(pidl);
                }
              }
            } else
            {
                HandleTransferFailed(IDS_TRANSFER_FAILED, hr, fileDescriptor->cFileName);
            }
        }
        //
        //  Delete the temporary file.
        //
        DeleteFileA(szTempFile);

    } //Loop over all items

    //
    //  The last step is blowing away all the directories on a move.
    //
    if(fMove)
    {
        //
        //  Run the list in the reverse order to delete.
        //
        index=pXboxFileGroupDescriptor->cItems;
        while(index--)
        {
            FILEDESCRIPTORA *fileDescriptor = pXboxFileGroupDescriptor->fgd + index;
            if(IsDirectory(fileDescriptor) &&(fileDescriptor->dwFlags != (UINT)-1))
            {
                CXboxFolder::GetWireName(szSourceWireName, pXboxFileGroupDescriptor->szFolderPath, fileDescriptor->cFileName);
                if(IsReadOnly(fileDescriptor))
                {
                    DM_FILE_ATTRIBUTES dmFileAttributes;
                    dmFileAttributes.Attributes = fileDescriptor->dwFileAttributes;
                    dmFileAttributes.Attributes &= ~FILE_ATTRIBUTE_READONLY;
                    dmFileAttributes.ChangeTime = fileDescriptor->ftLastWriteTime;
                    dmFileAttributes.CreationTime = fileDescriptor->ftCreationTime;
                    dmFileAttributes.SizeHigh = fileDescriptor->nFileSizeHigh;
                    dmFileAttributes.SizeLow = fileDescriptor->nFileSizeLow;
                    pSourceConnection->HrSetFileAttributes(szSourceWireName, &dmFileAttributes);
                }
                hr = pSourceConnection->HrDeleteFile(szSourceWireName, TRUE);
                if(SUCCEEDED(hr))
                {   
                    // Send out notification that the source was deleted.
                    LPITEMIDLIST pidl = m_pSelection->GetSourcePidl(
                                        pXboxFileGroupDescriptor->szFolderPath,
                                        fileDescriptor->cFileName
                                        );
                    SHChangeNotify(SHCNE_RMDIR,SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
                    CPidlUtils::Free(pidl);
                }
            }
        }
    }

    //
    //  Release the source connection, and unlock the HGLOBAL
    //
    pSourceConnection->HrUseSharedConnection(FALSE);
    pSourceConnection->Release();
    GlobalUnlock(m_stgMedium.hGlobal);
    return;
}

void CDropOperation::InitNameMapping(IDataObject *pDataObject, PNAMEMAPPING pNameMapping)
{
    HRESULT hr;
    FORMATETC fetcFileNameMap;
    fetcFileNameMap.cfFormat = CF_FILENAMEMAPA;
    fetcFileNameMap.dwAspect = DVASPECT_CONTENT;
    fetcFileNameMap.lindex   = -1;
    fetcFileNameMap.ptd      = NULL;
    fetcFileNameMap.tymed    = TYMED_HGLOBAL;
    pNameMapping->pszNames   = NULL;
    pNameMapping->pwszNames  = NULL;
    hr = m_pDataObject->GetData(&fetcFileNameMap, &pNameMapping->stgMedium);
    if(SUCCEEDED(hr))
    {
        pNameMapping->pszNames = (LPSTR)GlobalLock(pNameMapping->stgMedium.hGlobal);
        
    } else
    {
        fetcFileNameMap.cfFormat = CF_FILENAMEMAPW;
        hr = m_pDataObject->GetData(&fetcFileNameMap, &pNameMapping->stgMedium);
        if(SUCCEEDED(hr))
        {
            pNameMapping->pwszNames = (LPWSTR)GlobalLock(pNameMapping->stgMedium.hGlobal);
        }
    }
}

bool CDropOperation::GetDestination(PNAMEMAPPING pNameMapping, PHDROPFILE pDropFile)
/*++
  Routine Description:

    Reads the next name mapping and assigns it as a destination.  This is hard to
    do because there are three cases (ANSI name mapping, UNICODE name mapping, and
    none.

    Since the destination is converted to ANSI here, this is also a good place to check
    the legaility of the name on Xbox and possibly alert the user.

--*/
{
    bool fRet = true;
    pDropFile->pszRelativeFileName = NULL;
    //
    //  See if we have an ANSI name mapping
    //
    if(pNameMapping->pszNames)
    {
        UINT uByteCount   = strlen(pNameMapping->pszNames)+1;  //including NULL
        UINT uMBCharCount = _mbstrlen(pNameMapping->pszNames)+1;
        // If this has any multibyte characters we need to 
        // to refuse this.
        if(uByteCount != uMBCharCount)
        {
            UINT uFlags;
            if(m_dwFileCount < 2) 
            {
                uFlags = MB_OK|MB_ICONSTOP;
                StopProgressDialog();  //Kill progress dialog if this was the only file
            }
            else{
                uFlags = MB_OKCANCEL|MB_ICONSTOP;
            }
            if(IDOK!=WindowUtils::MessageBoxResource(m_hParentWnd, IDS_TRANSFER_ILLEGAL_FILENAME, IDS_TRANSFER_FAILED_CAPTION, uFlags, pNameMapping->pszNames))
            {
                m_fCancelled = TRUE;
            }
            fRet = false;
        } else
        {
            pDropFile->pszRelativeFileName = new char[uByteCount];
            if(pDropFile->pszRelativeFileName)
            {
                memcpy(pDropFile->pszRelativeFileName, pNameMapping->pszNames, uByteCount);
                pNameMapping->pszNames += uByteCount;
            } else
            {
                UINT uFlags;
                if(m_dwFileCount < 2) 
                {
                    uFlags = MB_OK|MB_ICONSTOP;
                    StopProgressDialog();  //Kill progress dialog if this was the only file
                }
                else{
                    uFlags = MB_OKCANCEL|MB_ICONSTOP;
                }
                if(IDOK!=WindowUtils::MessageBoxResource(m_hParentWnd, IDS_ERROR_LOW_MEMORY, IDS_TRANSFER_FAILED_CAPTION, uFlags))
                {
                    m_fCancelled = TRUE;
                }
                fRet = false;
            }
        }
    } else if(pNameMapping->pwszNames)
    //
    //  See if we have a UNICODE name mapping
    //
    {
        UINT uCharCount = wcslen(pNameMapping->pwszNames)+1;
        pDropFile->pszRelativeFileName = new char[uCharCount];
        if(pDropFile->pszRelativeFileName)
        {
            if(Utils::CopyWtoA(pDropFile->pszRelativeFileName, pNameMapping->pwszNames))
            {
                UINT uFlags;
                if(m_dwFileCount < 2) 
                {
                    uFlags = MB_OK|MB_ICONSTOP;
                    StopProgressDialog();  //Kill progress dialog if this was the only file
                }
                else{
                    uFlags = MB_OKCANCEL|MB_ICONSTOP;
                }
                
                WCHAR wszCaption[80];
                WCHAR wszError[MAX_PATH+80];
                LoadString(_Module.GetModuleInstance(), IDS_TRANSFER_ILLEGAL_FILENAME_W, wszCaption, ARRAYSIZE(wszCaption));
                wsprintf(wszError, wszCaption, pNameMapping->pwszNames);
                LoadString(_Module.GetModuleInstance(), IDS_TRANSFER_FAILED_CAPTION, wszCaption, ARRAYSIZE(wszCaption));
                MessageBox(m_hParentWnd, wszError, wszCaption, MB_OK);
                if(IDOK!=MessageBox(m_hParentWnd, wszError, wszCaption, uFlags))
                {
                    m_fCancelled = TRUE;
                }
                fRet = false;
                delete [] pDropFile->pszRelativeFileName;
                pDropFile->pszRelativeFileName = NULL;
            }
        } else
        {
            UINT uFlags;
            if(m_dwFileCount < 2) 
            {
                uFlags = MB_OK|MB_ICONSTOP;
                StopProgressDialog();  //Kill progress dialog if this was the only file
            }
            else{
                uFlags = MB_OKCANCEL|MB_ICONSTOP;
            }
            if(IDOK!=WindowUtils::MessageBoxResource(m_hParentWnd, IDS_ERROR_LOW_MEMORY, IDS_TRANSFER_FAILED_CAPTION, uFlags))
            {
                m_fCancelled = TRUE;
            }
            fRet = false;
        }
    } else
    //
    //  There is no mapping, so just take the last path element
    //  and consider that the mapping.
    //
    {
        LPSTR pszRelativeName = strrchr(pDropFile->pszFile, '\\');
        if(pszRelativeName)
        {
            pszRelativeName++;
        } else
        {
            pszRelativeName = pDropFile->pszFile;
        }
        UINT uByteCount   = strlen(pszRelativeName)+1;  //including NULL
        UINT uMBCharCount = _mbstrlen(pszRelativeName)+1;
        if(uByteCount != uMBCharCount)
        {
            UINT uFlags;
            if(m_dwFileCount < 2) 
            {
                uFlags = MB_OK|MB_ICONSTOP;
                StopProgressDialog();  //Kill progress dialog if this was the only file
            }
            else{
                uFlags = MB_OKCANCEL|MB_ICONSTOP;
            }
            if(IDOK!=WindowUtils::MessageBoxResource(m_hParentWnd, IDS_TRANSFER_ILLEGAL_FILENAME, IDS_TRANSFER_FAILED_CAPTION, uFlags, pszRelativeName))
            {
                m_fCancelled = TRUE;
            }
            fRet = false;
        } else
        {
            pDropFile->pszRelativeFileName = new char[uByteCount];
            if(pDropFile->pszRelativeFileName)
            {
                memcpy(pDropFile->pszRelativeFileName, pszRelativeName, uByteCount);
            } else
            {
                UINT uFlags;
                if(m_dwFileCount < 2) 
                {
                    uFlags = MB_OK|MB_ICONSTOP;
                    StopProgressDialog();  //Kill progress dialog if this was the only file
                }
                else{
                    uFlags = MB_OKCANCEL|MB_ICONSTOP;
                }
                if(IDOK!=WindowUtils::MessageBoxResource(m_hParentWnd, IDS_ERROR_LOW_MEMORY, IDS_TRANSFER_FAILED_CAPTION, uFlags))
                {
                    m_fCancelled = TRUE;
                }
                fRet = false;
            }
        }
    }
    return true;
}
void CDropOperation::CleanupNameMapping(PNAMEMAPPING pNameMapping)
{
        
    if(pNameMapping->pwszNames || pNameMapping->pszNames)
    {
        GlobalUnlock(pNameMapping->stgMedium.hGlobal);
        ReleaseStgMedium(&pNameMapping->stgMedium);
    }
}

void CDropOperation::DoHDropTransfer()
{
    
    HRESULT     hr;
    PHDROPFILE  pHDropFiles=NULL;

    _ASSERTE(TYMED_HGLOBAL == m_stgMedium.tymed);
    DROPFILES *pDropFiles;
    pDropFiles = (DROPFILES *)GlobalLock(m_stgMedium.hGlobal);
    bool    fMove = (DROPEFFECT_MOVE == m_dwDesiredEffect);
    bool    fOverWrite; //Keeps track of overwrite in for notification purposes
    
    NAMEMAPPING nameMapping;
    InitNameMapping(m_pDataObject, &nameMapping);
    
    //
    //  Create the list of files to transfer
    //
    if(pDropFiles->fWide)
    {
        PHDROPFILE pHDropFilesTail=NULL;
        LPWSTR pwszNextFile = (LPWSTR)AdvancePtr(pDropFiles,pDropFiles->pFiles);
        while(*pwszNextFile)
        {
            UINT uCharCount = wcslen(pwszNextFile)+1;
            PHDROPFILE pNewDropFile = new HDROPFILE;
            if(pNewDropFile)
            {
                pNewDropFile->pszFile = new char[uCharCount*2];
                if(pNewDropFile->pszFile)
                {
                    if(Utils::CopyWtoA(pNewDropFile->pszFile, pwszNextFile))
                    {
                        WCHAR wszCaption[128];
                        WCHAR wszError[MAX_PATH+128];
                        LoadString(_Module.GetModuleInstance(), IDS_TRANSFER_ILLEGAL_FILENAME_W, wszCaption, ARRAYSIZE(wszCaption));
                        wsprintf(wszError, wszCaption, pwszNextFile);
                        LoadString(_Module.GetModuleInstance(), IDS_TRANSFER_FAILED_CAPTION, wszCaption, ARRAYSIZE(wszCaption));
                        MessageBox(m_hParentWnd, wszError, wszCaption, MB_OK);
                        delete [] pNewDropFile->pszFile;
                        delete pNewDropFile;
                    } else
                    {
                        if(GetDestination(&nameMapping, pNewDropFile))
                        {
                            pNewDropFile->pNext = NULL;
                            if(pHDropFilesTail)  pHDropFilesTail->pNext = pNewDropFile;
                            else                 pHDropFiles = pNewDropFile;
                            pHDropFilesTail = pNewDropFile;
                        }else
                        {
                            delete [] pNewDropFile->pszFile;
                            delete pNewDropFile;
                        }
                    }
                } else
                {
                    delete pNewDropFile;
                }
            }
            // Next string.
            while(*pwszNextFile++);
        }
    } else
    {
        PHDROPFILE pHDropFilesTail=NULL;
        LPSTR pszNextFile = (LPSTR)AdvancePtr(pDropFiles,pDropFiles->pFiles);;
        while(*pszNextFile)
        {
            PHDROPFILE pNewDropFile = new HDROPFILE;
            if(pNewDropFile)
            {
                pNewDropFile->pszFile = pszNextFile;
                if(GetDestination(&nameMapping, pNewDropFile))
                {
                    pNewDropFile->pszRelativeFileName = NULL; //This is fixed up a little later.
                    pNewDropFile->pNext = NULL;
                    if(pHDropFilesTail)  pHDropFilesTail->pNext = pNewDropFile;
                    else                 pHDropFiles = pNewDropFile;
                    pHDropFilesTail = pNewDropFile;
                } else
                {
                        delete pNewDropFile;
                }
            } else
            {
                delete pNewDropFile;
            }
            // Next string.
            while(*pszNextFile++);
        }
    }

    //
    //  We are done with the namemapping
    //
    CleanupNameMapping(&nameMapping);

    //
    //  We now have a linked list of top-level files to copy.
    //  Do a little recursive search on them to get the total number
    //  of a files and the total number of bytes.
    //

    CalculateHDropWork(pHDropFiles);
    
    //
    //  Change message.
    //

    SetProgressTarget();
    char szTargetWireName[MAX_PATH];
    PHDROPFILE pCurrentFile = pHDropFiles;
    for(pCurrentFile = pHDropFiles; pCurrentFile; pCurrentFile = pCurrentFile->pNext)
    {
        //
        //  Query for a cancel
        //
        if(QueryCancel()) break;

        //
        //  Update the progress dialog
        //
        UpdateCopyProgress(pCurrentFile->pszRelativeFileName);

        //Add in fudge bytes for each file or directory
        m_ullBytesCompleted += FILE_PROGRESS_FUDGE_BYTES;

        //
        //  Get the wire name
        //
        m_pSelection->GetTargetWireName(szTargetWireName, pCurrentFile->pszRelativeFileName);
        
        //
        //  Get the attributes of the source file
        //
        WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
        if(GetFileAttributesExA(pCurrentFile->pszFile, GetFileExInfoStandard, &fileAttributes))
        {
            //
            //  Converts the file attributes to DM_FILE_ATTRIBUTES
            //
            DM_FILE_ATTRIBUTES dmFileAttributes;
            dmFileAttributes.Attributes = fileAttributes.dwFileAttributes;
            dmFileAttributes.ChangeTime = fileAttributes.ftLastWriteTime;
            dmFileAttributes.CreationTime = fileAttributes.ftCreationTime;
            dmFileAttributes.SizeHigh = fileAttributes.nFileSizeHigh;
            dmFileAttributes.SizeLow = fileAttributes.nFileSizeLow;
            strcpy(dmFileAttributes.Name, pCurrentFile->pszRelativeFileName);

            // Updates bytes competed, either we will complete them or cancel them
            ULARGE_INTEGER li;
            li.LowPart = fileAttributes.nFileSizeLow;
            li.HighPart = fileAttributes.nFileSizeHigh;
            m_ullBytesCompleted += li.QuadPart;
            if(m_ullBytesCompleted > m_ullBytesTotal) m_ullBytesTotal = m_ullBytesCompleted;

            if(fileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(fMove && (fileAttributes.dwFileAttributes&FILE_ATTRIBUTE_READONLY))
                {
                    if(!ConfirmFolderMove(pCurrentFile->pszRelativeFileName))
                    {
                        continue;
                    }
                }
                char szFullSource[MAX_PATH];
                strcpy(szFullSource, pCurrentFile->pszFile);
                HDropRecurse(szTargetWireName, szFullSource, &dmFileAttributes, fMove);
            } else
            {
                

                fOverWrite = false;
                if(fMove && (fileAttributes.dwFileAttributes&FILE_ATTRIBUTE_READONLY))
                {
                    if(!ConfirmFileMove(pCurrentFile->pszRelativeFileName))
                    {
                        continue;
                    }
                    
                }
                if(ConfirmFileReplace(szTargetWireName, pCurrentFile->pszRelativeFileName, &fileAttributes, &fOverWrite))
                {
                    hr = m_pTargetConnection->HrSendFile(pCurrentFile->pszFile, szTargetWireName);
                    if(SUCCEEDED(hr))
                    {
                        //
                        // Update 
                        //
                        m_pTargetConnection->HrSetFileAttributes(szTargetWireName, &dmFileAttributes);
                        //
                        //  Notify shell of new file.
                        //
                        LPITEMIDLIST pidl = m_pSelection->GetTargetPidl(pCurrentFile->pszRelativeFileName);
                        if(pidl)
                        {
                            SHChangeNotify(fOverWrite ? SHCNE_ATTRIBUTES : SHCNE_CREATE, SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
                            CPidlUtils::Free(pidl);
                        }
                        if(fMove)
                        {
                            if(fileAttributes.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
                            {
                                SetFileAttributesA(pCurrentFile->pszFile, FILE_ATTRIBUTE_NORMAL);
                            }
                            DeleteFileA(pCurrentFile->pszFile);

                    
                        }
                    } else
                    {
                        HandleTransferFailed(IDS_TRANSFER_FAILED, hr, pCurrentFile->pszRelativeFileName);
                    }
                }
            }
        } else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            HandleTransferFailed(IDS_COULDNT_READ_SOURCE_FILE, hr, pCurrentFile->pszFile);
        }
    }

    //
    //  Now cleanup the linked list of HDROPFILEs
    //
    while(pHDropFiles)
    {
        pCurrentFile = pHDropFiles;
        pHDropFiles = pCurrentFile->pNext;
        if(pDropFiles->fWide)
        {
            delete [] pCurrentFile->pszFile;
        }
        delete [] pCurrentFile->pszRelativeFileName;
        delete pCurrentFile;
    }
}

void CDropOperation::HDropRecurse(LPSTR pszTargetWireName, LPSTR szFullSource, DM_FILE_ATTRIBUTES *pdmFileAttributes, bool fMove)
{
    HRESULT hr;
    UINT uTargetLen = strlen(pszTargetWireName);
    UINT uszFullSourceLen = strlen(szFullSource);
    UINT uszRelativeLen = strlen(pdmFileAttributes->Name);
    bool fOverWrite;

    if(MakeTargetDirectory(pszTargetWireName, pdmFileAttributes))
    {
        szFullSource[uszFullSourceLen] = '\\';
        szFullSource[uszFullSourceLen+1] = '*';
        szFullSource[uszFullSourceLen+2] = '\0';
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(szFullSource, &findData);
        while(INVALID_HANDLE_VALUE != hFind)
        {
            if(QueryCancel())
            {
                FindClose(hFind);
                break;
            }
            if(findData.cFileName[0]!='.')
            {
                //Add in fudge bytes for each file or directory
                m_ullBytesCompleted += FILE_PROGRESS_FUDGE_BYTES;

                //
                //  Verify that name is legal
                //
                UINT uCharCount = strlen(findData.cFileName)+1;
                UINT uMbCharCount = _mbstrlen(findData.cFileName)+1;
                if(uCharCount != uMbCharCount)
                {
                    UINT uFlags;
                    if(m_dwFileCount < 2) 
                    {
                        uFlags = MB_OK|MB_ICONSTOP;
                        StopProgressDialog();  //Kill progress dialog if this was the only file
                    }
                    else{
                        uFlags = MB_OKCANCEL|MB_ICONSTOP;
                    }
                    if(IDOK!=WindowUtils::MessageBoxResource(m_hParentWnd, IDS_TRANSFER_ILLEGAL_FILENAME, IDS_TRANSFER_FAILED_CAPTION, uFlags, findData.cFileName))
                    {
                        m_fCancelled = TRUE;
                    }
                } else
                {
                    //
                    //  Tack on name to name of parent directory
                    //
                    LPSTR pszParse; 
                    pszParse = pszTargetWireName + uTargetLen;
                    *pszParse++ = '\\';
                    memcpy(pszParse, findData.cFileName, uCharCount);
                    pszParse = szFullSource + uszFullSourceLen;
                    *pszParse++ = '\\';
                    memcpy(pszParse, findData.cFileName, uCharCount);
                
                    //
                    //  Converts the file attributes to DM_FILE_ATTRIBUTES
                    //
                    DM_FILE_ATTRIBUTES dmFileAttributes;
                    dmFileAttributes.Attributes = findData.dwFileAttributes;
                    dmFileAttributes.ChangeTime = findData.ftLastWriteTime;
                    dmFileAttributes.CreationTime = findData.ftCreationTime;
                    dmFileAttributes.SizeHigh = findData.nFileSizeHigh;
                    dmFileAttributes.SizeLow = findData.nFileSizeLow;
                    memcpy(dmFileAttributes.Name, pdmFileAttributes->Name, uszRelativeLen);
                    pszParse = dmFileAttributes.Name + uszRelativeLen;
                    *pszParse++ = '\\';
                    memcpy(pszParse, findData.cFileName, uCharCount);
                    
                    //
                    //  Update the progress dialog
                    //
                    UpdateCopyProgress(dmFileAttributes.Name);
                    
                    if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        HDropRecurse(pszTargetWireName, szFullSource, &dmFileAttributes, fMove);
                    }
                    else
                    {
                        // Updates bytes competed, either we will complete them or cancel them
                        ULARGE_INTEGER li;
                        li.LowPart = findData.nFileSizeLow;
                        li.HighPart = findData.nFileSizeHigh;
                        m_ullBytesCompleted += li.QuadPart;
                        if(m_ullBytesCompleted > m_ullBytesTotal) m_ullBytesTotal = m_ullBytesCompleted;

                        if(ConfirmFileReplace(pszTargetWireName, dmFileAttributes.Name, (WIN32_FILE_ATTRIBUTE_DATA *)&findData, &fOverWrite))
                        {
                            
                            hr = m_pTargetConnection->HrSendFile(szFullSource, pszTargetWireName);
                            if(SUCCEEDED(hr))
                            {
                                m_pTargetConnection->HrSetFileAttributes(pszTargetWireName, &dmFileAttributes);
                                //
                                //  Notify shell of new file.
                                //
                                LPITEMIDLIST pidl = m_pSelection->GetTargetPidl(dmFileAttributes.Name);
                                if(pidl)
                                {
                                    SHChangeNotify(fOverWrite ? SHCNE_ATTRIBUTES : SHCNE_CREATE, SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
                                    CPidlUtils::Free(pidl);
                                }
                                if(fMove)
                                {
                                    if(findData.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
                                    {
                                        SetFileAttributesA(szFullSource, FILE_ATTRIBUTE_NORMAL);
                                    }
                                    DeleteFileA(szFullSource);
                                }
                            } else
                            {
                                HandleTransferFailed(IDS_TRANSFER_FAILED, hr, dmFileAttributes.Name);
                            }
                        }
                    }
                }
            }
            //
            //  Setup next loop
            //
            if(!FindNextFileA(hFind, &findData))
            {
                FindClose(hFind);
                hFind = INVALID_HANDLE_VALUE;
            }
        }

        //
        //  Reterminate the paths lopping off anything added here.
        //
        pszTargetWireName[uTargetLen]='\0';
        szFullSource[uszFullSourceLen]='\0';
        if(fMove)
        {
            if(pdmFileAttributes->Attributes&FILE_ATTRIBUTE_READONLY)
            {
                SetFileAttributesA(szFullSource, FILE_ATTRIBUTE_DIRECTORY);
            }
            RemoveDirectoryA(szFullSource);
        }
    }
}


void CDropOperation::CountDirContents(LPSTR pszDir, WIN32_FIND_DATAA *pFindData)
{
    HANDLE hFind;
    hFind = FindFirstFileA(pszDir, pFindData);
    if(INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            m_dwFileCount++;
            //Add in fudge bytes for each file or directory
            m_ullBytesTotal += FILE_PROGRESS_FUDGE_BYTES;
            if(pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                strcpy(pszDir, pFindData->cFileName);
                CountDirContents(pszDir, pFindData);
            } else
            {
                ULARGE_INTEGER li;
                li.LowPart = pFindData->nFileSizeLow;
                li.HighPart = pFindData->nFileSizeHigh;
                m_ullBytesTotal += li.QuadPart;
            }
        }while(FindNextFileA(hFind, pFindData));
        FindClose(hFind);
    }
}

void CDropOperation::CalculateHDropWork(PHDROPFILE pHDropFiles)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    m_dwFileCount = 0;
    m_ullBytesCompleted = 0;
    m_ullBytesTotal = 0;
    PHDROPFILE pCurrentFile;
    ULARGE_INTEGER li;
    for(pCurrentFile = pHDropFiles; pCurrentFile; pCurrentFile = pCurrentFile->pNext)
    {
        if(GetFileAttributesExA(pCurrentFile->pszFile, GetFileExInfoStandard, &fileAttributes))
        {
            m_dwFileCount++;
            //Add in fudge bytes for each file or directory
            m_ullBytesTotal += FILE_PROGRESS_FUDGE_BYTES;
            if(fileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                WIN32_FIND_DATAA findData; //scratch buffer for CountDirContents
                char szPath[MAX_PATH] = "";
                PathCombineA(szPath, pCurrentFile->pszFile, "*");
                
                CountDirContents(szPath, &findData);
            } else
            {
                li.LowPart = fileAttributes.nFileSizeLow;
                li.HighPart = fileAttributes.nFileSizeHigh;
                m_ullBytesTotal += li.QuadPart;
            }
        }
    }
}

void CDropOperation::DoFileGroupDescriptorWTransfer()
{
    _ASSERTE(FALSE && "FILEGROUPDESCW not yet implemented");

}

void CDropOperation::DoFileGroupDescriptorATransfer()
{
    _ASSERTE(FALSE && "FILEGROUPDESCA not yet implemented");
}

void CDropOperation::HandleTransferFailed(UINT uResourceId, HRESULT hr, LPCSTR pszFilename)
{
    char szError[512];
    UINT uFlags = MB_ICONERROR;
    if(m_dwFileCount>1)
    {
        uFlags |= MB_OKCANCEL;
    } else
    {
        uFlags |= MB_OK;   
        StopProgressDialog();
    }
    
    //  It has been requested that we special case the disk full error, to include the identity of the
    //  full volume, which is the whole reason that the volume is passed in.
    if(hr==XBDM_DEVICEFULL)
    {
        //Assume that it is the target that is full.  Why would it fail if the source is full?
        char szDriveLetter[MAX_PATH];
        char szConsoleName[60];
        m_pSelection->GetConsoleName(szConsoleName);
        m_pSelection->GetTargetWireName(szDriveLetter, NULL);
        szDriveLetter[1] = '\0';
        WindowUtils::rsprintf(szError, IDS_ERROR_VOLUME_FULL, szDriveLetter, szConsoleName);
    } else
    {
        FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
        if(!FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, szError, sizeof(szError), NULL))
        {
            LoadStringA(_Module.GetModuleInstance(), IDC_E_UNEXPECTED, szError, sizeof(szError));
        }
    }
    if(IDOK!=WindowUtils::MessageBoxResource(m_hParentWnd, uResourceId, IDS_TRANSFER_FAILED_CAPTION, uFlags, pszFilename, szError))
    {
        m_fCancelled = TRUE;
    }
}