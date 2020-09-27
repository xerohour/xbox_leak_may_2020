/*++

Copyright (c) Microsoft Corporation

Module Name:

    xbicon.cpp

Abstract:

    Implementation of CXboxMenu

Environment:

    Windows 2000 and Later 
    User Mode
    ATL

Notes:

    There are five different menus for which IContextMenu is used:

    1) File Menu (menu bar): with no selection.
    2) File Menu (menu bar) with a selection (one or more items).
    3) Popup Context Menu: with no selection.
    4) Popup Context Menu: with a selection (one or more items).
    5) Short-Cut menu: (popup context on a short-but)

    Unfortunately, we have to do different things in all of these cases,
    and IContextMenu is never explictly told which one it is.  Fortunately,
    a combination of documented and undocumented devices can tell us.  Firstly,
    we are created from IShellFolder::GetUIObjectOf which passes a cidl and apidl
    which are then passed to our create.  If if cidl is 0, there is no select:
    1), 3) or 5) above.  If cidl is > 0, then it is 2) or 4).  1) and 2) always have
    CMF_DVFILE set. 

Revision History:
    
    04-03-2001 : created (mitchd)

--*/

#include "stdafx.h"
static const char *szLangIndepLaunch          = "launch";
static const char *szLangIndepOpen            = "open";
static const char *szLangIndepExplore         = "explore";
static const char *szLangIndepReboot          = "reboot";
static const char *szLangIndepRebootSameTitle = "reboot_same_title";
static const char *szLangIndepRebootCold      = "reboot_cold";
static const char *szLangIndepCapture         = "capture";
static const char *szLangIndepSecurity        = "security";
static const char *szLangIndepSetDefault      = "setdefault";
static const char *szLangIndepCut             = "cut";
static const char *szLangIndepCopy            = "copy";
static const char *szLangIndepPaste           = "paste";
static const char *szLangIndepDelete          = "delete";
static const char *szLangIndepRename          = "rename";
static const char *szLangIndepNewFolder       = "newfolder";
static const char *szLangIndepNewConsole      = "newconsole";
static const char *szLangIndepProps           = "properties";


#define INVALID_MENU_INDEX ((UINT)-1)

CXboxMenu::MENU_ITEM_ENTRY CXboxMenu::sm_MenuItems[] = 
{
    {IDS_CM_LAUNCH,            szLangIndepOpen,            &CXboxMenu::Launch},
    {IDS_CM_OPEN,              szLangIndepOpen,            &CXboxMenu::Open},
    {IDS_CM_EXPLORE,           szLangIndepExplore,         &CXboxMenu::Explore},
    {IDS_CM_REBOOT_WARM,       szLangIndepReboot,          &CXboxMenu::RebootWarm},
    {IDS_CM_REBOOT_SAME_TITLE, szLangIndepRebootSameTitle, &CXboxMenu::RebootSameTitle},
    {IDS_CM_REBOOT_COLD,       szLangIndepRebootCold,      &CXboxMenu::RebootCold},
    {IDS_CM_CAPTURE,           szLangIndepCapture,         &CXboxMenu::Capture},
    {IDS_CM_SECURITY,          szLangIndepSecurity,        &CXboxMenu::Security},
    {IDS_CM_SETDEFAULT,        szLangIndepSetDefault,      &CXboxMenu::SetDefault},
    {IDS_CM_CUT,               szLangIndepCut,             &CXboxMenu::Cut},
    {IDS_CM_COPY,              szLangIndepCopy,            &CXboxMenu::Copy},
    {IDS_CM_PASTE,             szLangIndepPaste,           &CXboxMenu::Paste},
    {IDS_CM_DELETE,            szLangIndepDelete,          &CXboxMenu::Delete},
    {IDS_CM_RENAME,            szLangIndepRename,          &CXboxMenu::Rename},
    {IDS_CM_NEW_FOLDER,        szLangIndepNewFolder,       &CXboxMenu::NewFolder},
    {IDS_CM_NEW_CONSOLE,       szLangIndepNewConsole,      &CXboxMenu::NewConsole},
    {IDS_CM_PROPERTIES,        szLangIndepProps,           &CXboxMenu::Properties}
    
};

//Share Verbs are handled by CDefaultView on the file menu, and on the right click menu in the view.  However, they are not
//handled by CDefaultView on the right click menu for a selection of items in the view.
#define SHARED_VERBS (I2BIT(VERB_OPEN)|I2BIT(VERB_EXPLORE)|I2BIT(VERB_CUT)|I2BIT(VERB_COPY)|I2BIT(VERB_PASTE)\
                      |I2BIT(VERB_DELETE)|I2BIT(VERB_RENAME)|I2BIT(VERB_PROPERTIES))
//SEPARATED_VERBS always have a separator inserted into the menu before them.
#define SEPARATED_VERBS (I2BIT(VERB_REBOOT_WARM)|I2BIT(VERB_CUT)|I2BIT(VERB_DELETE)|I2BIT(VERB_NEWFOLDER)|I2BIT(VERB_NEWCONSOLE)| I2BIT(VERB_PROPERTIES))
// SINGLE_SELECTION_VERBS can only be performed on a single selection: renaming, pasting, launching.
#define SINGLE_SELECTION_VERBS (I2BIT(VERB_PASTE)|I2BIT(VERB_RENAME)|I2BIT(VERB_SETDEFAULT)|I2BIT(VERB_SECURITY)|I2BIT(VERB_LAUNCH))
// Default Verbs - can be a default verb
#define DEFAULT_VERBS (I2BIT(VERB_LAUNCH)|I2BIT(VERB_OPEN))
// SHORTCUT_VERBS - verbs that belong on the context menu of a short-cut
#define SHORTCUT_VERBS (I2BIT(VERB_OPEN)|I2BIT(VERB_EXPLORE)|I2BIT(VERB_CAPTURE)|VERB_REBOOT_BITS|I2BIT(VERB_SECURITY))


#define MENU_ITEM_COUNT   ((sizeof(sm_MenuItems)/sizeof(MENU_ITEM_ENTRY)))

HRESULT CXboxMenu::Create(UINT cidl, LPCITEMIDLIST * apidl, CXboxFolder *pParent, IContextMenu **ppContextMenu)
{
    HRESULT hr;
    CComObject<CXboxMenu> *pNewMenu = new CComObject<CXboxMenu>;
    *ppContextMenu = NULL;
    if(pNewMenu)
    {

        pNewMenu->m_uItemCount = cidl;
        hr = pParent->CloneSelection(cidl, apidl, &pNewMenu->m_pSelection);
        if(SUCCEEDED(hr))
        {
            pNewMenu->m_dwVerbs = pNewMenu->m_pSelection->GetVerbsForSelection();
            hr = pNewMenu->QueryInterface(IID_PPV_ARG(IContextMenu, ppContextMenu));
            _ASSERTE(SUCCEEDED(hr));
        } else
        {
            delete pNewMenu;
        }
    } else
    {
        hr = E_OUTOFMEMORY;
    }
    return hr;
}

HRESULT 
CXboxMenu::GetCommandString(
    UINT idCmd,
    UINT uFlags,
    UINT *pwReserved,
    LPSTR pszName,
    UINT cchMax
    )
{
    HRESULT hr = S_OK;
    int iChars = 0;
    
    // Bug 10854
    if(MENU_ITEM_COUNT <= idCmd)
    {
        return E_INVALIDARG;
    }

    switch(uFlags)
    {
        case GCS_HELPTEXTA:
             iChars = LoadStringA(_Module.GetModuleInstance(), HELP_ID_FROM_COMMAND_ID(sm_MenuItems[idCmd].uResourceId), pszName, cchMax);
             if(!iChars) hr = HRESULT_FROM_WIN32(GetLastError());
             break;
        case GCS_HELPTEXTW:
             iChars = LoadStringW(_Module.GetModuleInstance(), HELP_ID_FROM_COMMAND_ID(sm_MenuItems[idCmd].uResourceId), (LPOLESTR)pszName, cchMax);
             if(!iChars) hr = HRESULT_FROM_WIN32(GetLastError());
             break;
        case GCS_VERBA:
             strncpy(pszName, sm_MenuItems[idCmd].szLangIndepName, cchMax);
             break;
        case GCS_VERBW:
             wsprintfW((WCHAR *)pszName, L"%hs", sm_MenuItems[idCmd].szLangIndepName);
             break;
        default:
            _ASSERTE(FALSE);
            hr = E_INVALIDARG;
    };
    
    return hr;
}

HRESULT
CXboxMenu::InvokeCommand(
    LPCMINVOKECOMMANDINFO pici
    )
/*++

--*/
{
    UINT    uCmd;
    
    //
    //  Look up the command index
    //
    uCmd = GetCommandIndex(pici);

    //
    //  If we got a command index
    //  call the verb method.
    //
    if(INVALID_MENU_INDEX != uCmd)
    {
        // This really strange syntax is calling a member of
        // this through a pointer to a member function from 
        // the menu table.
        return (this->*sm_MenuItems[uCmd].pmfnVerb)(pici);
    }

    //
    //  The command was not part of our menu system.  Still it may be some standard
    //  string command that the shell can send.  See if we can handle it.
    if(HIWORD(pici->lpVerb))
    {
        // The shell sends "link" in respone to "create short", we handle it by offering
        // to create a short-cut on the desktop.
        if(0==_stricmp(pici->lpVerb, "link"))
        {
            CreateShortcut(pici);
        }
    }

    //MessageBoxA(pici->hwnd, "Some verb from the CDefView not in our menu", "NYI", MB_OK);
  
    //
    //  Pretend we did something
    //
    return S_FALSE;
}

class CSetDefault : public IXboxVisitor
{
  public:
    CSetDefault() : m_fSet(FALSE), m_fIsDefault(FALSE){}
    virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitConsole      (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags);
    virtual void VisitVolume       (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitDirectoryPost(IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){_ASSERT(FALSE);}
    inline void SetDefault(CXboxFolder *pSelection);
    inline BOOL IsDefault(CXboxFolder *pSelection);
  private:
    BOOL m_fSet;
    BOOL m_fIsDefault;
};


HRESULT
CXboxMenu::QueryContextMenu(
    HMENU hmenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags
    )
/*++
    Arguments:
        hmenu      - destination menu
        indexMenu  - location at which menu items should be inserted
        idCmdFirst - first available menu identifier
        idCmdLast  - first unavailable menu identifier    
   
    Return Value:
    
--*/

{
    UINT   uIndex;
    DWORD  dwItemMask; 
    UINT   uMenuId;
    UINT   uDefaultItem;
    char   szMenuItemName[80];
    char   szNewMenuName[80];
    char   szRebootMenuName[80];
    int    iMenuItemCount;
    
    m_uIdOffset = idCmdFirst;

    DWORD dwVerbs = m_dwVerbs;

    /*
    char szDebugMessage[512];
    sprintf(szDebugMessage, "uFlags = 0x%0.8x, m_uItemCount = 0x%0.8x, m_dwVerbs = 0x%0.8x", uFlags, m_uItemCount, m_dwVerbs);
    MessageBoxA(NULL, szDebugMessage, "QueryContextMenu", MB_OK);
    */
    
    //
    //  if CMF_CANRENAME is not set disable it.
    //

    if(!(uFlags&CMF_CANRENAME))
        dwVerbs &= ~I2BIT(VERB_RENAME);

    //
    //  If the default view is calling us to populate the
    //  file menu, don't add the shared items.
    //
    if((m_uItemCount == 0)||(uFlags&CMF_DVFILE))
    {
        dwVerbs &= ~SHARED_VERBS;
    } 
    
    if(m_uItemCount)
    // Do not insert the new folder item on a selection.
    {
        dwVerbs &= ~I2BIT(VERB_NEWFOLDER);
    }
    
    //If CMF_VERBSONLY this is a short-cut menu, so severly restrict, what is allowed on it.
    if(uFlags&CMF_VERBSONLY)
        dwVerbs &= SHORTCUT_VERBS;

    //
    //  If this is a multiple selection, remove the verbs that only apply to a single item.
    //
    if(m_uItemCount > 1)
    {
        dwVerbs &= ~SINGLE_SELECTION_VERBS;
    }

    //If it is  and the console is already the default, skip this verb
    if(dwVerbs&I2BIT(VERB_SETDEFAULT))
    {
        CSetDefault setDefault;
        if(setDefault.IsDefault(m_pSelection))
        {
            dwVerbs &= ~I2BIT(VERB_SETDEFAULT);
        }
    }

    //
    //  There seems to be some anomoly with sub-menus, currently new and reboot.
    //  These don't get removed automatically, so we need to clean them up ourselves.
    //  We were adding them only if they were not already present.  However, that doesn't
    //  work as nicely as I would like, so we will just always remove them, and readd
    //  them.  I doubt the performance hit will be too bad.
    //


    // Load the name of the "new" and "reboot" submenus.
    LoadStringA(_Module.GetModuleInstance(), IDS_CM_NEW, szNewMenuName, 80);        
    LoadStringA(_Module.GetModuleInstance(), IDS_CM_REBOOT, szRebootMenuName, 80);
    
    //  Search

    //  Search the menu to see if has a "New" or "Reboot" drop down.
    iMenuItemCount = GetMenuItemCount(hmenu);
    for(int iPos = 0; iPos < iMenuItemCount; iPos++)
    {
        char szExistingMenuName[80];
        GetMenuStringA(hmenu, iPos, szExistingMenuName, 80, MF_BYPOSITION);
        //See if it is the new menu
        if( (0==strcmp(szNewMenuName, szExistingMenuName)) || 
            (0==strcmp(szRebootMenuName, szExistingMenuName)) ||
            (0==strcmp("&View", szExistingMenuName))//BUG 10101 - The shell faults while handling this,
                                                    //since this problem seems internal to the shell, and I don't
                                                    //have time our resources to track down exactly why, and likely
                                                    //won't be able to fix it without serious hacking, we will just nuke it
                                                    //from our menu.  It wasn't there on Win2K, so big deal!
        )
        {
            DeleteMenu(hmenu, iPos, MF_BYPOSITION); 
        }
    }

    //
    //  Loop through all the items and insert those which have corresponding
    //  bit set in ulVerbs.
    //
    uDefaultItem = INVALID_MENU_INDEX;
    for(uIndex = 0, dwItemMask = 1; uIndex < MENU_ITEM_COUNT; uIndex++, dwItemMask <<= 1)
    {
        // If the item mask is not in our bitmap of verbs,
        // just skip it.
        if(!(dwItemMask&dwVerbs)) continue;

        //
        //  Get the string for the item
        //
        LoadStringA(_Module.GetModuleInstance(), sm_MenuItems[uIndex].uResourceId, szMenuItemName, 80);
        
        // If this is a SEPARATED_VERB, add the separator
        if(SEPARATED_VERBS&dwItemMask)
        {
            InsertMenuA(hmenu, indexMenu, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);
            if(indexMenu != 0xFFFFFFFF) indexMenu++;
        }
        uMenuId = m_uIdOffset+uIndex;
            
        //  If paste, decide whether not to Gray it out.
        UINT uMenuFlags = MF_STRING|MF_BYPOSITION;
        if(VERB_PASTE == uIndex)
        {
            IDataObject *pDataObject;
            HRESULT hr = OleGetClipboard(&pDataObject);
            uMenuFlags |= MF_GRAYED;
            if(SUCCEEDED(hr))
            {
                if(CDropOperation::GetDropFormat(pDataObject))
                {
                    uMenuFlags &= ~MF_GRAYED;
                }
                pDataObject->Release();
            }
        }

        //  If this is the new folder item, then we special case the insertion
        if( (VERB_NEWFOLDER == uIndex) || (VERB_NEWCONSOLE == uIndex) )
        {
            HMENU hNewMenu = CreateMenu();
            InsertMenuA(hNewMenu, -1, MF_STRING|MF_BYPOSITION, uMenuId, szMenuItemName);
            InsertMenuA(hmenu, indexMenu, MF_POPUP|MF_BYPOSITION, (UINT_PTR)hNewMenu, szNewMenuName);
            if(indexMenu != 0xFFFFFFFF) indexMenu++;
        } else if (VERB_REBOOT_WARM == uIndex)
        {
            HMENU hRebootMenu = CreateMenu();
            InsertMenuA(hRebootMenu, -1, MF_STRING|MF_BYPOSITION, uMenuId, szMenuItemName);
            LoadStringA(_Module.GetModuleInstance(), sm_MenuItems[++uIndex].uResourceId, szMenuItemName, 80);
            InsertMenuA(hRebootMenu, -1, MF_STRING|MF_BYPOSITION, ++uMenuId, szMenuItemName);
            LoadStringA(_Module.GetModuleInstance(), sm_MenuItems[++uIndex].uResourceId, szMenuItemName, 80);
            InsertMenuA(hRebootMenu, -1, MF_STRING|MF_BYPOSITION, ++uMenuId, szMenuItemName);
            dwItemMask <<= 2;
            InsertMenuA(hmenu, indexMenu, MF_POPUP|MF_BYPOSITION, (UINT_PTR)hRebootMenu, szRebootMenuName);
            if(indexMenu != 0xFFFFFFFF) indexMenu++;
        } else //  Not the reboot menu or the new folder menu, so just use the standard code.
        {   
            if(dwItemMask&DEFAULT_VERBS)
                        uDefaultItem = uMenuId;
            // Insert the menu item
            InsertMenuA(hmenu, indexMenu, uMenuFlags, uMenuId, szMenuItemName);
            if(indexMenu != 0xFFFFFFFF) indexMenu++;
        }
    }

    // The first item that we inserted is always the default.
    if(INVALID_MENU_INDEX != uDefaultItem)
        SetMenuDefaultItem(hmenu, uDefaultItem, FALSE);
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, uIndex+m_uIdOffset); 
}

UINT 
CXboxMenu::GetCommandIndex(
    LPCMINVOKECOMMANDINFO pici
    )
{
    //
    //  If the HIWORD is set it is a pointer
    //  to a command string.
    //
    if(HIWORD(pici->lpVerb))
    {
        // This is a pointer to a string, so do a case insenstive comparison against our table.
        for(UINT uId = 0; uId < MENU_ITEM_COUNT; uId++)
        {
            if( 0 == _stricmp(sm_MenuItems[uId].szLangIndepName, pici->lpVerb))
            {
                return uId;
            }
        }
    } else
    //
    //  Otherwise, it is just a command id from our menu, but subtract
    //  the offset we added, when building the menu
    //
    {
        
        UINT uId = LOWORD(pici->lpVerb);
        // Only return uId if it is in range for our ID table.
        if(MENU_ITEM_COUNT > uId)
                        return uId;
    }
    return INVALID_MENU_INDEX;
}

HRESULT CXboxMenu::Cut(LPCMINVOKECOMMANDINFO pici)
{
    HRESULT hr = CutCopy(true);
    return hr;
}

HRESULT CXboxMenu::Copy(LPCMINVOKECOMMANDINFO pici)
{
    HRESULT hr = CutCopy(false);
    return hr;
}


HRESULT CXboxMenu::CutCopy(bool fCut)
{
 
    // Get a DataObject
    IDataObject *pDataObject;
    HRESULT hr;
    hr = CXboxDataObject::Create(m_uItemCount, m_pSelection, &pDataObject);
    if(SUCCEEDED(hr))
    {
        IShellFolderView *pShellFolderView = NULL;

        //
        //  Mark this a copy.
        //
        DataObjUtil::SetPreferredDropEffect(pDataObject, fCut ? DROPEFFECT_MOVE : DROPEFFECT_COPY);
        
        //
        //  Get a shell folder view and set the points, for the cut.
        //
        if(SUCCEEDED(GetSite(IID_PPV_ARG(IShellFolderView, &pShellFolderView))))
          pShellFolderView->SetPoints(pDataObject);
        else
          pShellFolderView = NULL;

        //
        //  Put this up on the clipboard
        //  
        hr = OleSetClipboard(pDataObject);

        //
        //  Set the clipboard and release the pShellFolderView
        //
        if(pShellFolderView)
        {
            pShellFolderView->SetClipboard(fCut);
            pShellFolderView->Release();
        }

        //
        //  The clipboard will have referenced this, and if it hasn't
        //  we will be returning an error.  Either, we are not keeping
        //  a copy of the pointer, so release it.
        //
        pDataObject->Release();
    }
    return hr;
}

HRESULT CXboxMenu::Paste(LPCMINVOKECOMMANDINFO pici)
{
    HRESULT hr; 
    IDataObject *pDataObject;

    hr = OleGetClipboard(&pDataObject);
    if(FAILED(hr) || ! CDropOperation::GetDropFormat(pDataObject))
    {
        WindowUtils::MessageBoxResource(pici->hwnd, IDS_NOTHING_TO_PASTE, IDS_ERROR_PASTE_CAPTION, MB_OK|MB_ICONERROR);
        return E_FAIL;
    }

    //
    //  Get the preferred drop effect
    //
    DWORD dwEffect;
    hr = DataObjUtil::GetPreferredDropEffect(pDataObject, &dwEffect);
    if(FAILED(hr))
    {
        dwEffect = DROPEFFECT_COPY;
    }

    CDropOperation *pDropOperation = new CDropOperation(m_pSelection, pDataObject, dwEffect, pici->hwnd);
    if(pDropOperation)
    {
        pDropOperation->StartTransfer();
    } else
    {
        WindowUtils::MessageBoxResource(pici->hwnd, IDS_ERROR_LOW_MEMORY, IDS_ERROR_PASTE_CAPTION, MB_OK|MB_ICONERROR);
    }
    pDataObject->Release();
    return S_OK;
}

HRESULT CXboxMenu::Delete(LPCMINVOKECOMMANDINFO pici)
/*++
  Routine Description:
    Called to delete items.
--*/
{
    CWaitCursor waitCursor;

    if(0==m_uItemCount) return E_FAIL;

    CXboxDelete deleteItems(pici->hwnd, m_uItemCount, (pici->fMask&CMIC_MASK_FLAG_NO_UI) ? true : false);
    m_pSelection->VisitEach(&deleteItems, IXboxVisitor::FlagContinue|IXboxVisitor::FlagRecurse);

    return S_OK;
}

HRESULT CXboxMenu::Rename(LPCMINVOKECOMMANDINFO pici)
{
    return S_OK;
}

HRESULT CXboxMenu::Properties(LPCMINVOKECOMMANDINFO pici)
{
    HRESULT hr;
    CWaitCursor waitCursor;
    
    CXboxPropertySheet *pPropSheet = new CXboxPropertySheet;
    hr = pPropSheet->Initialize(m_uItemCount, m_pSelection);
    if(SUCCEEDED(hr))
    {
        //
        //  pPropSheet deletes itself when the property sheet closes.
        //
        pPropSheet->DoProperties(0, NULL);
    } else
    {
        delete pPropSheet;
    }

    return S_OK;
}

HRESULT CXboxMenu::Security(LPCMINVOKECOMMANDINFO pici)
{
    HRESULT hr;
    CWaitCursor waitCursor;
    
    CXboxPropertySheet *pPropSheet = new CXboxPropertySheet;
    hr = pPropSheet->Initialize(m_uItemCount, m_pSelection);
    if(SUCCEEDED(hr))
    {
        //
        //  pPropSheet deletes itself when the property sheet closes.
        //
        pPropSheet->DoProperties(1, NULL);
    } else
    {
        delete pPropSheet;
    }

    return hr;
}

HRESULT CXboxMenu::NewFolder(LPCMINVOKECOMMANDINFO pici)
/*++
  Routine Description:
    Creates a new folder in the given folder.  Basically,
    using the selection as the "drop target."


--*/
{
    char szTargetWireName[MAX_PATH];
    char szNewFolder[40];
    IXboxConnection *pConnection = m_pSelection->GetXboxConnection();
    HRESULT hr;

    //
    //  Create a new folder.
    //
    UINT uFolderNum = 1;
    LoadStringA(_Module.GetModuleInstance(), IDS_NEW_FOLDER, szNewFolder, sizeof(szNewFolder));
    do
    {
        m_pSelection->GetTargetWireName(szTargetWireName, szNewFolder);
        hr = pConnection->HrMkdir(szTargetWireName);
        if(XBDM_ALREADYEXISTS == hr)
        {
            WindowUtils::rsprintf(szNewFolder, IDS_NEW_FOLDER_TEMPLATE, ++uFolderNum);
        }
    } while(XBDM_ALREADYEXISTS == hr);
    if(FAILED(hr))
    {
        char szError[60];
        FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
        WindowUtils::MessageBoxResource(pici->hwnd, IDS_ERROR_CREATE_FOLDER, IDS_ERROR_CREATE_FOLDER_CAPTION, MB_OK|MB_ICONSTOP, szError);
        return hr;
    }

    //
    //  Make a pidl
    //
    LPITEMIDLIST pidl = m_pSelection->GetTargetPidl(szNewFolder);
    if(!pidl)
    {
        return E_OUTOFMEMORY;
    }

    //
    //  Send out notifications.
    //
    SHChangeNotify(SHCNE_MKDIR, SHCNF_FLUSH|SHCNF_IDLIST, pidl, NULL);
    
    LPITEMIDLIST pidlSimple = CPidlUtils::LastItem(pidl);


    //
    //  Position and select the folder for edit
    //
    
    IShellView2         *pShellView2;
    IShellFolderView    *pShellFolderView;
    hr = GetSite(IID_PPV_ARG(IShellFolderView, &pShellFolderView));
    if(SUCCEEDED(hr))
    {
        POINT pt;
        hr = pShellFolderView->GetDropPoint(&pt);
        if(SUCCEEDED(hr))
        {
            hr = GetSite(IID_PPV_ARG(IShellView2, &pShellView2));
            if(SUCCEEDED(hr))
            {
                hr = pShellView2->SelectAndPositionItem(pidlSimple,SVSI_SELECT|SVSI_EDIT|SVSI_DESELECTOTHERS|SVSI_ENSUREVISIBLE,&pt);
                pShellView2->Release();
            }
        }
        pShellFolderView->Release();
    }

    CPidlUtils::Free(pidl);
    return hr;
}

HRESULT CXboxMenu::NewConsole(LPCMINVOKECOMMANDINFO pici)
/*++
  Routine Description:
    Launch the Add Console Wizard
--*/
{
    CWaitCursor waitCursor;
    ExecuteAddConsoleWizard();
    return S_OK;
}


class CExplore : public IXboxVisitor
{
  public:
    CExplore(LPCMINVOKECOMMANDINFO pici, IShellBrowser *pShellBrowser, BOOL fOpen=TRUE) :
        m_pici(pici), m_pShellBrowser(pShellBrowser), m_fOpen(fOpen), m_fFirstOne(TRUE){}
    virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags);
    virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags);
    virtual void VisitConsole      (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags);   
    virtual void VisitVolume       (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags);
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags);
    virtual void VisitDirectoryPost(IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){}
  protected:
    void Explore(IXboxVisit *pItem);
  private:
    BOOL m_fOpen;
    BOOL m_fFirstOne;
    LPCMINVOKECOMMANDINFO m_pici;
    IShellBrowser *m_pShellBrowser;
};

void CExplore::VisitRoot(IXboxVisit *pRoot, DWORD *pdwFlags)
{
    _ASSERTE(FALSE);
}
void CExplore::VisitAddConsole(IXboxVisit *pAddConsole, DWORD *pdwFlags)
{
    ExecuteAddConsoleWizard();
}
void CExplore::VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags)
{
    Explore(pConsole);
}
void CExplore::VisitVolume(IXboxVolumeVisit *pVolume, DWORD *pdwFlags)
{
    Explore(pVolume);
}
void CExplore::VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir, DWORD *pdwFlags)
{
    Explore(pFileOrDir);
}

#define SEE_MASK_SHARED (SEE_MASK_FLAG_NO_UI | SEE_MASK_HOTKEY | SEE_MASK_NO_CONSOLE)
void CExplore::Explore(IXboxVisit *pItem)
{ 
    LPITEMIDLIST pidl = NULL;
    UINT uFlags;
    if(m_fOpen)
    {   
        uFlags = m_fFirstOne ? (SBSP_DEFBROWSER|SBSP_OPENMODE|SBSP_RELATIVE) : (SBSP_NEWBROWSER|SBSP_OPENMODE|SBSP_RELATIVE);
    } else
    {
        uFlags = SBSP_NEWBROWSER|SBSP_EXPLOREMODE|SBSP_RELATIVE;
    }

    // Make sure box is live.
    char szConsoleName[80];
    pItem->GetConsoleName(szConsoleName);
    if(!Utils::VerifyXboxAlive(szConsoleName))
    {
        char szError[60];
        char szName[MAX_PATH];
        pItem->GetName(szName);
        FormatUtils::XboxErrorString(XBDM_CANNOTCONNECT, szError, sizeof(szError));
        WindowUtils::MessageBoxResource(m_pici->hwnd, IDS_ERROR_OPENNING_FOLDER, IDS_ERROR_OPENNING_FOLDER_CAPTION, MB_OK|MB_ICONSTOP, szName, szError);
        return;
    }
    
    if(pItem->GetShellAttributes()&SFGAO_FOLDER)
    {
        if(m_pShellBrowser)
        {
            pidl = pItem->GetPidl(CPidlUtils::PidlTypeSimple);
            if(pidl)
            {
                HRESULT hr = m_pShellBrowser->BrowseObject(pidl, uFlags);
                if(FAILED(hr))
                {
                    char szError[60];
                    char szName[MAX_PATH];
                    pItem->GetName(szName);
                    if(HRESULT_FACILITY(hr)==FACILITY_XBDM)
                    {
                        FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
                    } else
                    {       
                        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, szError, sizeof(szError), NULL);
                    }
                    WindowUtils::MessageBoxResource(m_pici->hwnd, IDS_ERROR_OPENNING_FOLDER, IDS_ERROR_OPENNING_FOLDER_CAPTION, MB_OK|MB_ICONSTOP, szName, szError);
                }
                CPidlUtils::Free(pidl);
            }
        } else
        {
            pidl = pItem->GetPidl(CPidlUtils::PidlTypeAbsolute);
            if(pidl)
            {
                SHELLEXECUTEINFOA sei;
                ZeroMemory(&sei, sizeof(SHELLEXECUTEINFOA));
                sei.cbSize = sizeof(SHELLEXECUTEINFOA);
                sei.hwnd = m_fOpen ? m_pici->hwnd : NULL;
                sei.fMask = SEE_MASK_IDLIST | (m_pici->fMask & SEE_MASK_SHARED);
                sei.nShow = m_pici->nShow;
                sei.dwHotKey = m_pici->dwHotKey;
                sei.hIcon = m_pici->hIcon;
                sei.lpParameters = m_pici->lpParameters;
                sei.lpDirectory = m_pici->lpDirectory;
                sei.lpVerb = m_fOpen ? "open" : "explore";
                sei.lpIDList = pItem->GetPidl(CPidlUtils::PidlTypeAbsolute);
                ShellExecuteExA(&sei);
                CPidlUtils::Free(pidl);
            }
        }
    }
    m_fFirstOne = FALSE;
}

HRESULT CXboxMenu::Explore(LPCMINVOKECOMMANDINFO pici)
{
    CWaitCursor waitCursor;
    ExploreOpen(pici, FALSE);
    return S_OK;
}

HRESULT CXboxMenu::Open(LPCMINVOKECOMMANDINFO pici)
{
    CWaitCursor waitCursor;
    ExploreOpen(pici, TRUE);
    return S_OK;
}

void CXboxMenu::ExploreOpen(LPCMINVOKECOMMANDINFO pici, BOOL fOpen)
{
    IShellBrowser *pShellBrowser = NULL;
    if(m_uItemCount)
    {
        if(m_pUnknownSite)
        {
            HRESULT hr = GetService(SID_SShellBrowser, IID_PPV_ARG(IShellBrowser, &pShellBrowser));
            if(FAILED(hr))
            {
                pShellBrowser = NULL;
            }
        }
        CExplore explore(pici, pShellBrowser, fOpen);
        m_pSelection->VisitEach(&explore, IXboxVisitor::FlagContinue);
        
    } else
    {
        _ASSERTE(FALSE);
    }
}

class CRebootLaunch : public IXboxVisitor
{
  public:
    CRebootLaunch(BOOL fCold = FALSE, BOOL fSameTitle = FALSE) : m_fCold(fCold), m_fSameTitle(fSameTitle){}
    virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitConsole      (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags);
    virtual void VisitVolume       (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags);
    virtual void VisitDirectoryPost(IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){_ASSERT(FALSE);}
  protected:
    void Reboot(IXboxVisit *pVisit, LPCSTR pszWireName);
    BOOL m_fCold;
    BOOL m_fSameTitle;
};

void CRebootLaunch::VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags)
{
    Reboot(pConsole, NULL);
}
void CRebootLaunch::VisitFileOrDir(IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags)
{
    char szWireName[MAX_PATH];
    pFileOrDir->GetWireName(szWireName);
    if(CXboxFolder::IsXbeFile(szWireName))
    {
        Reboot(pFileOrDir, szWireName);
    }
}

void CRebootLaunch::Reboot(IXboxVisit *pVisit, LPCSTR pszWireName)
{
    char szConsoleName[60];
    DM_XBE dmXbe;
    pVisit->GetConsoleName(szConsoleName);
    IXboxConnection *pConnection;
    if(SUCCEEDED(Utils::GetXboxConnection(szConsoleName, &pConnection)))
    {
        if(!pszWireName && m_fSameTitle)
        {
            if(SUCCEEDED(pConnection->HrGetXbeInfo(NULL, &dmXbe)))
            {
                pszWireName = dmXbe.LaunchPath;
            }
        }
        pConnection->HrReboot(m_fCold ? 0 : DMBOOT_WARM, pszWireName);
        pConnection->Release();
    }
}

HRESULT CXboxMenu::RebootWarm(LPCMINVOKECOMMANDINFO pici)
{
    CWaitCursor waitCursor;
    CRebootLaunch rebootLaunch(FALSE, FALSE);
    m_pSelection->VisitEach(&rebootLaunch, IXboxVisitor::FlagContinue);
    return S_OK;
}

HRESULT CXboxMenu::RebootSameTitle(LPCMINVOKECOMMANDINFO pici)
{
    CWaitCursor waitCursor;
    CRebootLaunch rebootLaunch(FALSE, TRUE);
    m_pSelection->VisitEach(&rebootLaunch, IXboxVisitor::FlagContinue);
    return S_OK;
}

HRESULT CXboxMenu::RebootCold(LPCMINVOKECOMMANDINFO pici)
{
    CWaitCursor waitCursor;
    CRebootLaunch rebootLaunch(TRUE, FALSE);
    m_pSelection->VisitEach(&rebootLaunch, IXboxVisitor::FlagContinue);
    return S_OK;
}

HRESULT CXboxMenu::Launch(LPCMINVOKECOMMANDINFO pici)
{
    CWaitCursor waitCursor;
    CRebootLaunch rebootLaunch;
    m_pSelection->VisitEach(&rebootLaunch, IXboxVisitor::FlagContinue);
    return S_OK;
}

class CCreateDesktopShortcuts : public IXboxVisitor
{
    public:
       CCreateDesktopShortcuts();
        virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags){Visit(pRoot, pdwFlags);}
        virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags){Visit(pAddConsole, pdwFlags);}
        virtual void VisitConsole      (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags){Visit(pConsole, pdwFlags);}
        virtual void VisitVolume       (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags){Visit(pVolume, pdwFlags);}
        virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){Visit(pFileOrDir, pdwFlags);}
        virtual void VisitDirectoryPost(IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){_ASSERT(FALSE);}
    private:
        void Visit(IXboxVisit *pItem, DWORD *pdwFlags);
        char m_szDesktopPath[MAX_PATH];
};

CCreateDesktopShortcuts::CCreateDesktopShortcuts()
{
    if(FAILED(SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, m_szDesktopPath)))
    {
        *m_szDesktopPath = '\0';
    }
}

    
void CCreateDesktopShortcuts::Visit(IXboxVisit *pItem, DWORD *pdwFlags)
{
    HRESULT hr;
    ULONG ulAttributes = pItem->GetShellAttributes();
    if(!(SFGAO_FOLDER&ulAttributes))
    {
        return;
    }

    IShellLinkA *pShellLink;
    IPersistFile *pPersistFile;

    // Create an IShellLink object and get a pointer to the IShellLink
	// interface (returned from CoCreateInstance).
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARG(IShellLinkA, &pShellLink));
    if(SUCCEEDED(hr))
    {
       hr = pShellLink->QueryInterface(IID_PPV_ARG(IPersistFile, &pPersistFile));
	   if (SUCCEEDED(hr))
	   {
           LPITEMIDLIST pidl = pItem->GetPidl(CPidlUtils::PidlTypeAbsolute);
           if(pidl)
           {
                hr = pShellLink->SetIDList(pidl);
                CPidlUtils::Free(pidl);
                if(SUCCEEDED(hr))
                {
                    char szConsoleName[80];
                    char szItemName[MAX_PATH];
                    char szShortCutName[MAX_PATH];
                    pItem->GetConsoleName(szConsoleName);
                    pItem->GetName(szItemName);
                    WindowUtils::rsprintf(szShortCutName, IDS_NORMAL_NAME_FORMAT, szItemName, szConsoleName);
                    // Set the description of the shortcut.
                  	hr = pShellLink->SetDescription(szShortCutName);

                    if(SUCCEEDED(hr))
                    {   
                        WCHAR szPathName[MAX_PATH];
                        wsprintf(szPathName, L"%hs\\%hs.lnk", m_szDesktopPath, szShortCutName);
                        hr = pPersistFile->Save(szPathName, TRUE);
                    }
                }
           }
           pPersistFile->Release();
       }
       pShellLink->Release();
    }
}


HRESULT CXboxMenu::CreateShortcut(LPCMINVOKECOMMANDINFO pici)
{
    //Prompt user
    if(IDYES==WindowUtils::MessageBoxResource(pici->hwnd, IDS_CONFIRM_DESKTOP_SHORTCUT, IDS_CONFIRM_DESKTOP_SHORTCUT_CAPTION, MB_YESNO|MB_ICONQUESTION))
    {
        CCreateDesktopShortcuts createDesktopShortCuts;
        m_pSelection->VisitEach(&createDesktopShortCuts, IXboxVisitor::FlagContinue); 
    }
    return S_OK;
}


class CScreenCapture : public IXboxVisitor
{
  public:
    CScreenCapture(HWND hWnd):m_hWnd(hWnd){if(!m_hWnd) m_hWnd = GetDesktopWindow();}
    virtual void VisitRoot         (IXboxVisit *pRoot,                DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitAddConsole   (IXboxVisit *pAddConsole,          DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitConsole      (IXboxConsoleVisit   *pConsole,    DWORD *pdwFlags);
    virtual void VisitVolume       (IXboxVolumeVisit    *pVolume,     DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitFileOrDir    (IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){_ASSERT(FALSE);}
    virtual void VisitDirectoryPost(IXboxFileOrDirVisit *pFileOrDir,  DWORD *pdwFlags){_ASSERT(FALSE);}
  private:
    HWND m_hWnd;
};

void CScreenCapture::VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags)
{
    char szConsoleName[60];
    pConsole->GetConsoleName(szConsoleName);
    Utils::ScreenCapture(m_hWnd, szConsoleName);
}

HRESULT CXboxMenu::Capture(LPCMINVOKECOMMANDINFO pici)
{
    CScreenCapture screenCapture(pici->hwnd);
    m_pSelection->VisitEach(&screenCapture, IXboxVisitor::FlagContinue);
    return S_OK;
}


void CSetDefault::VisitConsole(IXboxConsoleVisit *pConsole, DWORD *pdwFlags)
{
    if(m_fSet)
    {
        pConsole->SetDefault(TRUE);
        *pdwFlags = 0;
    } else
    {
        char szConsoleName[60];
        pConsole->GetConsoleName(szConsoleName);
        CManageConsoles manageConsole;
        m_fIsDefault = manageConsole.IsDefault(szConsoleName);
        *pdwFlags = 0;
    }
}

void CSetDefault::SetDefault(CXboxFolder *pSelection)
{
    m_fSet = TRUE;
    pSelection->VisitEach(this, IXboxVisitor::FlagContinue);
}

BOOL CSetDefault::IsDefault(CXboxFolder  *pSelection)
{
    m_fSet = FALSE;
    pSelection->VisitEach(this, IXboxVisitor::FlagContinue);
    return m_fIsDefault;
}

HRESULT CXboxMenu::SetDefault(LPCMINVOKECOMMANDINFO pici)
{
    CWaitCursor waitCursor;
    CSetDefault setDefault;
    setDefault.SetDefault(m_pSelection);
    return S_OK;
}
