/*++

Copyright (c) Microsoft Corporation

Module Name:

    delete.h

Abstract:

    Implementation of CXboxDelete.

    Pass an instance to VisitEach or VisitThese to visit death
    and deletion upon such items.

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    07-18-2001 : created (mitchd)

--*/

#include "stdafx.h"

CXboxDelete::CXboxDelete(HWND hWnd, UINT uItemCount, bool fSilent) : 
    m_hWnd(hWnd), m_uItemCount(uItemCount),
    m_fSilent(fSilent), m_fFirstItem(true),
    m_fYesToAllReadOnly(false), m_fNoToAllReadOnly(false),
    m_uRecurseDepth(0), m_pProgressDialog(NULL)
{
}

CXboxDelete::~CXboxDelete()
{
    if(m_pProgressDialog)
    {
        m_pProgressDialog->StopProgressDialog();
        m_pProgressDialog->Release();
    }
}

void CXboxDelete::StartProgressDialog()
{
    HRESULT hr;
    WCHAR   wszBuffer[128];
    
    //
    //  Show Dialog That we are preparing the copy.
    //
    hr = CoCreateInstance(
            CLSID_ProgressDialog,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IProgressDialog,
            (LPVOID *)&m_pProgressDialog
            );
    if(FAILED(hr))
    {
        m_pProgressDialog = NULL;
        return;
    }

    //
    //  Set the title of the progress dialog
    //
    
    LoadStringW(_Module.GetModuleInstance(), IDS_FILE_DELETE_PROGRESS_TITLE, wszBuffer, 128);
    m_pProgressDialog->SetTitle(wszBuffer);
    
    //
    //  Set the animation for the progress dialog
    //
    
    m_pProgressDialog->SetAnimation(_Module.GetModuleInstance(), IDA_DELETE);
    
    //
    //  Set the cancel message for the progress dialog
    //

    LoadStringW(_Module.GetModuleInstance(), IDS_FILE_PROGRESS_CANCEL, wszBuffer, 128);
    m_pProgressDialog->SetCancelMsg(wszBuffer, NULL);

    //
    //  Set Line One Text
    //
    LoadStringW(_Module.GetModuleInstance(), IDS_FILE_DELETE_PROGRESS_STATUS, wszBuffer, 128);
    m_pProgressDialog->SetLine(1, wszBuffer, FALSE, NULL);

    //
    //  Show the progress dialog
    //

    m_pProgressDialog->StartProgressDialog(m_hWnd, NULL, PROGDLG_AUTOTIME, NULL);
}



void CXboxDelete::VisitRoot(IXboxVisit *pRoot, DWORD *pdwFlags)
/*++
  Routine Description:
   User is attempting to delete the root.  Given them some nice
   message telling them that they cannot.  If they like like though,
   they may uninstall.
--*/
{
    //
    //  Display Cannot delete message.
    //
    WindowUtils::MessageBoxResource(m_hWnd, IDS_CANNOT_DELETE_ROOT, IDS_GENERIC_CAPTION, MB_OK|MB_ICONEXCLAMATION);
    // Stop visits.
    *pdwFlags = 0;
}

void CXboxDelete::VisitAddConsole(IXboxVisit *pAddConsole, DWORD *pdwFlags)
{
    WindowUtils::MessageBoxResource(m_hWnd, IDS_CANNOT_DELETE_ADD_XBOX, IDS_GENERIC_CAPTION, MB_OK|MB_ICONEXCLAMATION);
}

void CXboxDelete::VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags)
/*++
  Routine Description:
    Delete a console.  A.K.A. remove a console from the list of console that
    we know about.  This will fail if it is the default console.
        
    NOT YET IMPLEMENTED.
    
    This Yes\No behavior on each console should be consistent with the way printers
    work in the shell.

--*/
{
    UINT uMsgResult; 
    char szConfirmText[512];
    char szConfirmCaption[60];
    char szConsoleName[60];

    pConsole->GetName(szConsoleName);

    //
    //  Get confirmation.
    //
    CManageConsoles consoleManager;
    BOOL fIsDefault = consoleManager.IsDefault(szConsoleName);
    LoadStringA(_Module.GetModuleInstance(), IDS_CONFIRM_REMOVE_CONSOLE_CAPTION, szConfirmCaption, ARRAYSIZE(szConfirmCaption));
    if(m_fFirstItem)
    {
        m_fFirstItem = false;

        //
        //  There may be one or more than one selected console
        //
        if( m_uItemCount > 1)
        {
            WindowUtils::rsprintf(szConfirmText, IDS_CONFIRM_REMOVE_MULTIPLE_CONSOLES, m_uItemCount);
        } else
        {
            WindowUtils::rsprintf(szConfirmText, IDS_CONFIRM_REMOVE_CONSOLE, szConsoleName);
        }
        // Show this Message if the first item is not the default or if multiple consoles to be deleted.
        // If there is just one console and it is the default, then the below dialog will suffice.
        if((m_uItemCount > 1) || !fIsDefault)
        {
            if(IDYES!=MessageBoxA(m_hWnd, szConfirmText, szConfirmCaption, MB_YESNO|MB_ICONQUESTION))
            {
                *pdwFlags = 0;
                return;
            }
        }
    }
    if(fIsDefault)
    {
        m_fFirstItem = false;
        WindowUtils::rsprintf(szConfirmText, IDS_CONFIRM_REMOVE_DEFAULT_CONSOLE, szConsoleName);
        if(IDYES!=MessageBoxA(m_hWnd, szConfirmText, szConfirmCaption, MB_YESNO|MB_ICONQUESTION))
        {
            return;
        }
        consoleManager.SetDefault(NULL);
    }

    
    
    //
    //  Get the pidl
    //
    LPITEMIDLIST pidl = pConsole->GetPidl(CPidlUtils::PidlTypeAbsolute);
    
    //
    //  Remove the console from the registry.
    //
    consoleManager.Remove(szConsoleName);
    
    //
    //  Notify everyone of the change.
    //
    if(pidl)
    {
        SHChangeNotify(SHCNE_DELETE, SHCNF_IDLIST|SHCNF_FLUSH, pidl, NULL);
        CPidlUtils::Free(pidl);
    }
}

void CXboxDelete::VisitVolume(IXboxVolumeVisit *pVolume, DWORD *pdwFlags)
{
    MessageBoxA(m_hWnd, "Cannot delete a volume.", "CXboxDelete", MB_OK|MB_ICONERROR);
}

void CXboxDelete::VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags)
/*++
  Routine Description:
    Called to confirm deletion of files and\or folders.  This is the full blown
    confirmation deal.
    
    1) Get the attributes and name.  Return any errors that occur while retrieving these.

    2) If this is the first item of a group of selected items to delete:
    
      a) confirm the whole shabang. 

      b) If the user said 'Yes', start the Progress Dialog.

      c) If the user did not say 'Yes' return E_ABORT.

    3) Check for cancel in the progress dialog.  return E_ABORT on a cancel.

    4) Update the progress dialog to indicate we are working on the current item.

    5) If (one of multiple top-level items AND read-only item AND m_fYesToAllReadOnly is false)
      
      a) Ask the user to confirm.

      b) return S_FALSE if the user said no.

      c) Set m_fYesToAllReadOnly if the user said yes to all.

    6) Increment the recursion depth for any children we may or may not have.

    7) return S_OK.
--*/
{
    HRESULT hr;
    DM_FILE_ATTRIBUTES dmFileAttributes;
    char szItemName[MAX_PATH];
    bool fReadOnly;
    bool fFolder;
    bool fDelete  = true;
    UINT uConfirm;
    
    
    //
    //  Get the attributes.
    //

    pFileOrDir->GetFileAttributes(&dmFileAttributes);
    fReadOnly = (dmFileAttributes.Attributes&FILE_ATTRIBUTE_READONLY) ? true : false;
    fFolder = dmFileAttributes.Attributes&FILE_ATTRIBUTE_DIRECTORY ? true : false;

    if(!m_fSilent)
    {
       
        //
        //  This will give the relative name if we are recursing.
        //
        pFileOrDir->GetName(szItemName);
    
        //
        //  Get User Confirmation
        //  (for this we need the number of items, the name and attributes of the first item)
        //

        if(m_fFirstItem)
        {
            m_fFirstItem = false;

            //
            // There are three different confirmation dialogs for 
            // ConfirmDeleteMultiple, ConfirmDeleteFolder, ConfirmDelete
            // Figure out which one and collect the proper information.
            //
        
            if( m_uItemCount > 1)
            {
                uConfirm = Dialog::ConfirmDeleteMultiple(m_hWnd, m_uItemCount);
            } else
            {
                uConfirm = Dialog::ConfirmDelete(m_hWnd, szItemName, fFolder, fReadOnly);
            }

            if(IDC_XB_YES==uConfirm)
            {
                StartProgressDialog();
            } else
            {
                *pdwFlags = 0;
                return;
            }
        }
    
        if(m_pProgressDialog)
        {
        
            WCHAR wszName[512];
            LoadStringW(_Module.GetModuleInstance(), IDS_FILE_DELETE_PROGRESS_TITLE, wszName, 128);
            m_pProgressDialog->SetTitle(wszName);

            if(m_pProgressDialog->HasUserCancelled())
            {
                *pdwFlags = 0;
                return;
            }

            wsprintfW(wszName, L"%hs", szItemName);
            m_pProgressDialog->SetLine(2, wszName, TRUE, NULL);
        }

        if(!m_uRecurseDepth && (m_uItemCount!=1) && fReadOnly)
        {
            if(m_fYesToAllReadOnly)
            {
                *pdwFlags |= IXboxVisitor::FlagCallPost;
            } else if(m_fNoToAllReadOnly)
            {
                *pdwFlags &= ~IXboxVisitor::FlagCallPost;
            } else
            {
                uConfirm = Dialog::ConfirmDelete(m_hWnd, szItemName, fFolder, fReadOnly);
                switch(uConfirm)
                {
                    case IDC_XB_YESTOALL:    
                        m_fYesToAllReadOnly = true;
                    case IDC_XB_YES:
                        break;
                    case IDC_XB_NOTOALL:
                        m_fNoToAllReadOnly = true;
                    case IDC_XB_NO:
                        fDelete = false;
                        break;
                    case IDC_XB_CANCEL:
                        *pdwFlags = 0;
                        return;
                }
            }
        }
    }
    //
    //  If we are going to delete this item,
    //  we may have work.
    if(fDelete)
    {
        //
        //  If it is not a folder, peform the delete
        //  here.
        //
        if(!fFolder)
        {   
            LPITEMIDLIST pidl = pFileOrDir->GetPidl(CPidlUtils::PidlTypeAbsolute);
            hr = pFileOrDir->Delete();
            if(SUCCEEDED(hr))
            {
               if(pidl)
               {
                   SHChangeNotify(SHCNE_DELETE, SHCNF_IDLIST|SHCNF_FLUSH, pidl, NULL);
               }

            } else if(!m_fSilent)
            {
                MessageBoxA(m_hWnd, szItemName, "Could not delete file.", MB_OK|MB_ICONERROR);
            }
            if(pidl)
            {
                CPidlUtils::Free(pidl);
            }
        } else
        //
        //  It is a folder
        //
        {
            *pdwFlags = IXboxVisitor::FlagRecurse | 
                        IXboxVisitor::FlagContinue |
                        IXboxVisitor::FlagCallPost;
            m_uRecurseDepth++;
        }
    }
    return;
}

void CXboxDelete::VisitDirectoryPost(IXboxFileOrDirVisit *pDirectory, DWORD *pdwFlags)
{
    HRESULT hr;

    LPITEMIDLIST pidl = pDirectory->GetPidl(CPidlUtils::PidlTypeAbsolute);
    m_uRecurseDepth--;
    hr = pDirectory->Delete();
    if(SUCCEEDED(hr))
    {
       if(pidl)
       {
           SHChangeNotify(SHCNE_RMDIR, SHCNF_IDLIST|SHCNF_FLUSH, pidl, NULL);
       }

    } else if(!m_fSilent)
    {
        MessageBoxA(m_hWnd, NULL, "Could not delete folder.", MB_OK|MB_ICONERROR);
    }
    if(pidl)
    {
        CPidlUtils::Free(pidl);
    }
}

    
