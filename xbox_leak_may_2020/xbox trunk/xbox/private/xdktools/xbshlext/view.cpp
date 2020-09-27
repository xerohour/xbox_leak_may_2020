/*++

Copyright (c) Microsoft Corporation

Module Name:

    view.cpp

Abstract:

    Implementation of CXboxViewCB

Environment:

    Windows 2000 and Later 
    User Mode
    ATL
    Depends on Private Shell APIs.

Revision History:
    
    03-28-2001 : created (mitchd)

--*/

#include "stdafx.h"


//------------------------------------------------------------
//  Useful Definitions
//------------------------------------------------------------
#define XBOX_SHCNE_EVENTS \
            (SHCNE_DISKEVENTS   |\
             SHCNE_ASSOCCHANGED |\
             SHCNE_RMDIR        |\
             SHCNE_DELETE       |\
             SHCNE_MKDIR        |\
             SHCNE_CREATE       |\
             SHCNE_RENAMEFOLDER |\
             SHCNE_RENAMEITEM   |\
             SHCNE_UPDATEITEM   |\
             SHCNE_ATTRIBUTES)

#define XBOX_STATUS_PART_WIDTH  165

//------------------------------------------------------------
//  Creating a ShellView
//------------------------------------------------------------
HRESULT CXboxViewCB::CreateShellView(CXboxFolder *pXboxFolder, HWND hWnd, REFIID riid, void **ppv)
/*++
  Routine Description:
    Creates a CDefView with an instance of CXboxViewCB as the callback to represent
    the view of pXboxFolder.

    This method mostly could be implemented 
    A factory method for getting an instance of an IShellView, IShellView2 or
    an other method that CDefView supports.
--*/
{
   HRESULT                  hr;
   CComObject<CXboxViewCB> *pViewCB = NULL;
   IShellFolderViewCB      *pShellFolderViewCB = NULL;
   IShellFolder            *pShellFolder = NULL;
   IShellView              *pShellView = NULL;
   
   *ppv = NULL;

   hr = CComObject<CXboxViewCB>::CreateInstance(&pViewCB);
   if(SUCCEEDED(hr))
   {
      //
      // Introduce the view to its parent\creator, this object.
      //
      pXboxFolder->AddRef();
      pViewCB->m_pXboxFolder = pXboxFolder;
      pViewCB->m_hWnd = hWnd;

      //
      // QI the view for its IShellFolderViewCB interface.
      // (This cannot fail.  We know it has the interface.)
      //

      hr = pViewCB->QueryInterface(IID_PPV_ARG(IShellFolderViewCB, &pShellFolderViewCB));
      _ASSERTE(SUCCEEDED(hr));
      
      //
      // QI the CXboxFolder for IShellFolder.
      // (This cannot fail.  We know it has the interface.)
      //

      hr = pXboxFolder->QueryInterface(IID_PPV_ARG(IShellFolder,&pShellFolder));
      _ASSERTE(SUCCEEDED(hr));
      
      //
      //  UNDOCUMENTED:
      //      Calls SHCreateShellFolderView, which is a private undocumented
      //      shell function to create a shell default view.
      //

      SFV_CREATE sfvCreate;
      sfvCreate.cbSize = sizeof(SFV_CREATE);
      sfvCreate.pshf = pShellFolder;
      sfvCreate.psvOuter = NULL;  // Add an outer, if we want to add an interface to the web view.
      sfvCreate.psfvcb = pShellFolderViewCB; // Add a callback later, as the features start taking shape.
      hr = SHCreateShellFolderView(&sfvCreate, &pShellView);
      if(SUCCEEDED(hr))
      {
        *ppv = pShellView;
      }
      
      //
      // We are done with these interfaces.  The shell view should have AddRef'd them
      // during the call to SHCreateShellFolderView.
      //

      pShellFolder->Release();
      pShellFolderViewCB->Release();

   }
   return hr;
}

HRESULT
CXboxViewCB::MessageSFVCB(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
/*++
  Routine Description:
  
    This is a message dispatch routine for the callback.  For the most part we
    crack messages and call routines that handle each message.  In a few cases,
    the message is handled inline.

--*/
{
    HRESULT hr = E_FAIL;

    switch (uMsg)
    {
      
      //
      // Messages cracked and dispatched
      //
      case DVM_DEFITEMCOUNT:
        hr = OnDefItemCount((LPINT)lParam);
        break;
      case DVM_GETHELPTEXT:
        hr = OnGetHelpText((int)LOWORD(wParam), (LPWSTR)lParam, (int)HIWORD(wParam));
        break;
      case SFVM_GETPANE:
        hr = OnGetPane((DWORD) wParam, (DWORD *)lParam);
        break;
      case DVM_REFRESH:
        hr = OnRefresh((BOOL) wParam);
        break;
      case DVM_DIDDRAGDROP:
        hr = OnDidDragDrop(wParam, (IDataObject *)lParam);
        break;
      case DVM_GETDETAILSOF:
        hr = OnGetDetailsOf((UINT)wParam, (PDETAILSINFO)lParam);
        break;
      case DVM_COLUMNCLICK:
        hr = OnColumnClick((UINT)wParam);
        break;
      case DVM_MERGEMENU:
        hr = OnMergeMenu((LPQCMINFO)lParam);
        break;
      case DVM_UNMERGEMENU:
        hr = OnUnMergeMenu((HMENU)lParam);
        break;
      case DVM_INVOKECOMMAND:
        hr = OnInvokeCommand((UINT)wParam);
        break;
      case SFVM_GETNOTIFY:
        hr = OnGetNotify((LPITEMIDLIST *) wParam, (LONG *) lParam);
        break;
      case SFVM_FSNOTIFY:
        hr = OnFSNotify((LPITEMIDLIST) wParam, (LONG ) lParam);
        break;
      case SFVM_SIZE:
        hr = OnSize((LONG) wParam, (LONG) lParam);
        break;
      case DVM_UPDATESTATUSBAR:
        hr = OnUpdateStatusBar();
        break;
      case SFVM_THISIDLIST:
        hr = OnThisIDList((LPITEMIDLIST *) lParam);
        break;
      case SFVM_ADDPROPERTYPAGES:
        hr = OnAddPropertyPages((SFVM_PROPPAGE_DATA *)lParam);
        break;
      //
      // Messages handled in place
      //
      case SFVM_BACKGROUNDENUMDONE:
        hr = S_OK;
        break;
      case DVM_INITMENUPOPUP:
        hr =S_OK;
        break;
      case DVM_RELEASE:
      {
        IShellFolderViewCB *pfv = (IShellFolderViewCB*)lParam;
        if (pfv)
            hr = pfv->Release();
      }
        break;
      case DVM_BACKGROUNDENUM:
        //  WARNING!  If we return S_OK from DVM_BACKGROUNDENUM, we also
        //  are promising that we support free threading on our IEnumIDList
        //  interface!  This allows the shell to do enumeration on our
        //  IEnumIDList on a separate background thread.
        hr = S_OK; // Don't enum in background
        break;
      case SFVM_DONTCUSTOMIZE:
        if (lParam)
            *((BOOL *) lParam) = FALSE;  // Yes, we are customizable.
        hr = S_OK;
        break;
      case SFVM_GETZONE:
        //Consider us the local machine (for now anyway).
        *((DWORD *)lParam) = URLZONE_LOCAL_MACHINE;
        hr = S_OK;
        break;
      case DVM_WINDOWCREATED:
        hr = S_OK;
        break;

    // Others that aren't currently handled.
#if 0
    NOTHANDLED(SFVM_SETISFV);           //We handle SetSite
    NOTHANDLED(DVM_GETTOOLTIPTEXT);
    NOTHANDLED(SFVM_QUERYFSNOTIFY);
    
    NOTHANDLED(SFVM_GETHELPTOPIC);
    NOTHANDLED(DVM_GETBUTTONINFO);
    NOTHANDLED(DVM_GETBUTTONS);
    NOTHANDLED(DVM_SELCHANGE);
    NOTHANDLED(DVM_DRAWITEM);
    NOTHANDLED(DVM_MEASUREITEM);
    NOTHANDLED(DVM_EXITMENULOOP);
    NOTHANDLED(DVM_GETCCHMAX);
    NOTHANDLED(DVM_WINDOWDESTROY);
    NOTHANDLED(DVM_SETFOCUS);
    NOTHANDLED(DVM_KILLFOCUS);
    NOTHANDLED(DVM_QUERYCOPYHOOK);
    NOTHANDLED(DVM_NOTIFYCOPYHOOK);
    NOTHANDLED(DVM_DEFVIEWMODE);
    NOTHANDLED(DVM_INSERTITEM);         // Too verbose
    NOTHANDLED(DVM_DELETEITEM);         // Too verbose
    NOTHANDLED(DVM_GETWORKINGDIR);
    NOTHANDLED(DVM_GETCOLSAVESTREAM);
    NOTHANDLED(DVM_SELECTALL);
    NOTHANDLED(DVM_SUPPORTSIDENTIFY);
    NOTHANDLED(DVM_FOLDERISPARENT);
#endif
    default:
        hr = E_NOTIMPL;
        break;
    }

    return hr;
}


HRESULT CXboxViewCB::OnDefItemCount(LPINT pi)
/*++
  Routine Description:
    Requests a preliminary guess of how many items we may have.

  Comments:
    Ask our Xbox Folder how many children we have.  In many
    circumstances it knows.  Just in case it doesn't we
    set the fGuesstimate parameter as TRUE.  This tells the
    folder not to do something time consuming like go over the
    wire, if it doesn't already know.  Instead it makes a better
    guess than we can.
    
  Arguments:
    pi - pointer to receive count.
--*/
{
    *pi = m_pXboxFolder->GetChildCount(TRUE);
    return S_OK;
}



HRESULT CXboxViewCB::OnGetHelpText(int /*ici*/, LPWSTR pszHelpText, int cChar)
/*++
  Routine Description:  
    Gets a help string for each UI element.
  Arguments:
    ici - column index
    pszHelpText - help string
    cChar - count in characters of pszHelpText buffer.
--*/
{
    wcsncpy(pszHelpText, L"", cChar);
    return S_OK;
}

HRESULT CXboxViewCB::OnGetPane(DWORD dwPaneID, DWORD *pdwPane)
/*++
  Routine Description:  
    Not 100% clear on the concept, yet, but I am pretty sure:
    
    Divides the status bar into multiple panes.  You may assign an ordinal
    to each pane type.  The bar is divided into enough panes for your highest
    oridinal.  If some numbers are skipped you just have blank panes.

  Arguments:
    dwPaneID - pane type
    pdwPane - pointer to oridinal for pane

  Remarks:
    *pdwPane is NO_PANE (-1) on entry.

--*/
{
    return S_OK;
}


HRESULT CXboxViewCB::OnRefresh(BOOL fReload)
/*++
  Routine Description:  
    Reenumerate everything.  The default view does most of the work.  However,
    we need to tell our CXboxFolder to invalidate the information

  Arguments:
    fReload - TRUE if we have to this
    
--*/
{
    //We can deal later
    if(fReload)
    {
        //
        //  The folder should forget about its children
        //  and go and reenumerate them.
        //
        m_pXboxFolder->InvalidateChildList();
        
    }
    return S_OK;
}



HRESULT CXboxViewCB::OnDidDragDrop(DWORD dwDropEffect, IDataObject * pdo)
/*++
  Routine Description:  
    Notifies us that a drag and drop operation completed.  If it was a move,
    return FALSE.  This basically delegates the delete portion to our
    parent.  It should work by calling delete on our context menu.
--*/
{
    HRESULT hr = S_OK;
    if (DROPEFFECT_MOVE == dwDropEffect)
    {
        IAsyncOperation *pao;
        hr = pdo->QueryInterface(IID_IAsyncOperation, (void **) &pao);
        if (SUCCEEDED(hr))
        {
            BOOL fInAsyncOp = TRUE;
            hr = S_OK;  // Don't have caller do the delete.
            if(SUCCEEDED(pao->InOperation(&fInAsyncOp)))
            {
                if (FALSE == fInAsyncOp)
                {
                    hr = S_FALSE;   // Have parent do the delete.
                }
            }
            pao->Release();
        }
    }
    return hr;
}

typedef struct _SHELLDETAILSINFO
{
    LPCITEMIDLIST pidl;
    SHELLDETAILS  shellDetails;
    int iImage;
} SHELLDETAILSINFO;


HRESULT CXboxViewCB::OnGetDetailsOf(UINT iColumn, PDETAILSINFO pdi)
/*++
  Routine Description:
    Get information about the columns (either for an item, or the column header).
    Actually, our folder implements not us, so like most everything else, we 
    delegate.
--*/
{
  SHELLDETAILSINFO *pShellDetailsInfo = (SHELLDETAILSINFO *)pdi;
  if(pdi->pidl)
  {
    return m_pXboxFolder->GetDetails(pdi->pidl, iColumn, &pShellDetailsInfo->shellDetails);
  } else
  {
    return m_pXboxFolder->GetColumnHeaderDetails(iColumn, &pShellDetailsInfo->shellDetails);
  }
}

HRESULT CXboxViewCB::OnInvokeCommand(UINT idc)
/*++
  Routine Description:
    Some of the "shared" menu commands end up here, rather than in our context
    menu.  Go figure.  So we need to support them here.

  NYI
--*/
{
    HRESULT hr = S_OK;
    /*
    switch (idc)
    {
        case IDM_SORTBYNAME:
        case IDM_SORTBYSIZE:
        case IDM_SORTBYTYPE:
        case IDM_SORTBYDATE:
          hr = CXboxViewCB::OnColumnClick(MESSAGE_TO_ICI(idc));
          break;
        case IDC_ITEM_NEWFOLDER:
          OnInvokeNewFolder(m_hWnd);
        break;

     default:
        ASSERT(0);
        hr = E_NOTIMPL;
        break;
    }
    */
    return hr;
}



HRESULT CXboxViewCB::OnGetNotify(LPITEMIDLIST *ppidl, LONG * lEvents)
/*++

  Routine Description:
    The browser is asking which notifications we want, and what PIDL
     to use as a context.

  Arguments:
   ppidl - put the pidl here
   lEvents - fill out any event flags you want.
     
--*/
{
    HRESULT hr;
    *lEvents = XBOX_SHCNE_EVENTS;
    *ppidl = m_pXboxFolder->GetPidl(CPidlUtils::PidlTypeAbsolute);
    if(NULL == *ppidl)     
    {
       return E_OUTOFMEMORY;
    }
    return S_OK;
}

HRESULT CXboxViewCB::OnFSNotify(LPITEMIDLIST  pidl, LONG lEvent)
{
    m_pXboxFolder->InvalidateChildList();
    return S_OK;
}

/*++
OnThisIDList
    Routine Description:
     Get the pidl relative to our root.

    Arguments:
     ppidl - put the pidl here
--*/
HRESULT CXboxViewCB::OnThisIDList(LPITEMIDLIST * ppidl)
{
    *ppidl = m_pXboxFolder->GetPidl(CPidlUtils::PidlTypeRoot);
    if(NULL == *ppidl)     
    {
       return E_OUTOFMEMORY;
    }
    return S_OK;
}

HRESULT CXboxViewCB::OnColumnClick(UINT ici)
{
    HRESULT hr;
    IShellFolderView *pShellFolderView;
    hr = GetSite(IID_PPV_ARG(IShellFolderView, &pShellFolderView));
    if(SUCCEEDED(hr))
    {
        hr = pShellFolderView->Rearrange(ici);
        pShellFolderView->Release();
    }
    return hr;
}

HRESULT CXboxViewCB::OnUpdateStatusBar(void)
{
    HRESULT hr;
    LRESULT lr;
    UINT uObjCount = 0;
    UINT uSelectCount = 0;
    LPCITEMIDLIST *pidls = NULL;
    
    //
    //  Get the ShellFolderView, we can get information about the number objects,
    //  how many and which objects are selected.
    //
    IShellFolderView *pShellFolderView;
    hr = GetSite(IID_PPV_ARG(IShellFolderView, &pShellFolderView));
    if(SUCCEEDED(hr))
    {
        //
        //  Get the count of total objects in the view
        //
        hr = pShellFolderView->GetObjectCount(&uObjCount);
        _ASSERT(SUCCEEDED(hr));

        //
        //  Get the selected objects
        //
        hr = pShellFolderView->GetSelectedObjects(&pidls, &uSelectCount);
        _ASSERT(SUCCEEDED(hr));

        //
        //  Get an interface to a shell browser
        //
        IShellBrowser *pShellBrowser;
        hr = GetService(SID_SShellBrowser, IID_PPV_ARG(IShellBrowser, &pShellBrowser));
        if(SUCCEEDED(hr))
        {
            /*
            **  Main of the operations may fail.  Oh well, the status bar won't look right.  Don't bother
            **  saving or looking at the error codes, since there is nothing to be done about it.
            */
            
            //
            //  Update the Icon in the zone area
            //
            HICON hXboxIcon = (HICON)LoadImage(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 16, 16, LR_LOADMAP3DCOLORS);
            pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETICON, (WPARAM)1, (LPARAM)hXboxIcon, &lr);

            //
            //  Update the Text in the zone area
            //
            char szConsoleName[60];
            m_pXboxFolder->GetConsoleName(szConsoleName);
            if(*szConsoleName)
            {
                pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXTA, (WPARAM)1, (LPARAM)szConsoleName, &lr);
            } else
            {
                pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXTA, (WPARAM)1, (LPARAM)WindowUtils::GetPreloadedString(IDS_PRELOAD_XBOX_ZONE_NAME), &lr);
            }
            
            //
            //  If there are no objects selected, show the object count in the 0th part
            //
            if(0==uSelectCount)
            {
                char szBuffer[30];
                WindowUtils::rsprintf(szBuffer, IDS_STATUS_OBJECT_COUNT_FORMAT, uObjCount);
                pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXTA, (WPARAM)0, (LPARAM)szBuffer, &lr);
            }
            //
            //  If there is more than one object selected, show the selected object count in the 0th part
            //
            else if(1<uSelectCount)
            {
                char szBuffer[30];
                WindowUtils::rsprintf(szBuffer, IDS_STATUS_SELOBJECT_COUNT_FORMAT, uSelectCount);
                pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXTA, (WPARAM)0, (LPARAM)szBuffer, &lr);
            }
            //
            //  Othewise, let the folder set the text for the 0th part
            //
            else
            {
                m_pXboxFolder->SetStatusBarText(pShellBrowser, pidls[0]);
            }
            pShellBrowser->Release();
        }

        //
        //  Done with the ShellFolderView
        //
        pShellFolderView->Release();
    }
    
    // Whether or not we managed to update the status bar, we probably don't want to alert the
    // shell or the user, so just say S_OK.
    return S_OK;
}


HRESULT CXboxViewCB::SetStatusParts()
{
    HRESULT hr;
    LRESULT lr;
    UINT i;
    HWND hStatusWnd;
    RECT statusRect;
    UINT uRightCoordinateArray[2];

    // Get the pShellBrowser, without it we cannot do anything.
    IShellBrowser *pShellBrowser;
    hr = GetService(SID_SShellBrowser, IID_PPV_ARG(IShellBrowser, &pShellBrowser));
    if(FAILED(hr))
        return hr;

    // Get the status window
    hr = pShellBrowser->GetControlWindow(FCW_STATUS, &hStatusWnd);
    if(SUCCEEDED(hr))
    {
        // Get the client rect of the status window
        if(GetClientRect(hStatusWnd, &statusRect))
        {
            // Calculate where the divider goes between the two pains.
            if(statusRect.right < 2*XBOX_STATUS_PART_WIDTH)
            {
                uRightCoordinateArray[0] = statusRect.right/2;
            } else
            {
                uRightCoordinateArray[0] = statusRect.right - XBOX_STATUS_PART_WIDTH;
            }
            uRightCoordinateArray[1] = -1;
            
            // Update the SB_SETPARTS
            hr = pShellBrowser->SendControlMsg(
                    FCW_STATUS, SB_SETPARTS, (WPARAM)2, (LPARAM)uRightCoordinateArray, &lr);
        }else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    //Done with the shell browser
    pShellBrowser->Release();

    return hr;
}
