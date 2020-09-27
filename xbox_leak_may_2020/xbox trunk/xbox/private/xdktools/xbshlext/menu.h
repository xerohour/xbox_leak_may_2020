/*++

Copyright (c) Microsoft Corporation


Module Name:

    menu.h

Abstract:
    
    Declars the CXboxContextMenu class

Environment:

    Windows 2000
    Uses ATL

Notes:

Revision History:

    April 2001 - created by Mitchell Dernis (mitchd)
    July  2001 - major overhaul (mitchd)

--*/
#ifndef __XBOXMENU_H__
#define __XBOXMENU_H__

//
//  Funciton Prototype Definition for a Menu Verb
//
class CXboxMenu;
typedef HRESULT (CXboxMenu::*PMENU_VERB)(LPCMINVOKECOMMANDINFO pici);

// CXboxMenu
class ATL_NO_VTABLE CXboxMenu : 
	public CComObjectRootEx<CComMultiThreadModel>,
    public CShellObjectWithSite,
    public IContextMenu
{
  public:
    
    /*
    **  c'tor and d'tor
    */
      CXboxMenu() : m_uItemCount(FALSE), m_pSelection(NULL){}
    ~CXboxMenu(){if(m_pSelection) m_pSelection->Release();}
    static HRESULT Create(UINT cidl, LPCITEMIDLIST * apidl, CXboxFolder *pParent, IContextMenu **ppContextMenu);

    /*
    ** ATL COM INTERFACE MAP
    */

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    BEGIN_COM_MAP(CXboxMenu)
    COM_INTERFACE_ENTRY_IID(IID_IObjectWithSite, IObjectWithSite)
    COM_INTERFACE_ENTRY_IID(IID_IContextMenu, IContextMenu)
    END_COM_MAP()
  
  public:

    /*
    **  IContextMenu methods
    */
    STDMETHOD(GetCommandString)(UINT idCmd, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO pici);
    STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);

    typedef struct _MENU_ITEM_ENTRY
    {
        UINT        uResourceId;       // Resource ID from which to get display string
        LPCSTR      szLangIndepName;   // Language independent name of item
        PMENU_VERB  pmfnVerb;          // Pointer to member function that execture verb 
    }MENU_ITEM_ENTRY, *PMENU_ITEM_ENTRY;

  protected:


    UINT GetCommandIndex(LPCMINVOKECOMMANDINFO pici);

    /*
    **  Implementation of the various supported verbs.
    */
    HRESULT Launch(LPCMINVOKECOMMANDINFO pici);
    HRESULT Open(LPCMINVOKECOMMANDINFO pici);
    HRESULT Explore(LPCMINVOKECOMMANDINFO pici);
    HRESULT RebootWarm(LPCMINVOKECOMMANDINFO pici);
    HRESULT RebootSameTitle(LPCMINVOKECOMMANDINFO pici);
    HRESULT RebootCold(LPCMINVOKECOMMANDINFO pici);
    HRESULT Capture(LPCMINVOKECOMMANDINFO pici);
    HRESULT SetDefault(LPCMINVOKECOMMANDINFO pici);
    HRESULT Security(LPCMINVOKECOMMANDINFO pici);
    HRESULT Cut(LPCMINVOKECOMMANDINFO pici);
    HRESULT Copy(LPCMINVOKECOMMANDINFO pici);
    HRESULT Paste(LPCMINVOKECOMMANDINFO pici);
    HRESULT Delete(LPCMINVOKECOMMANDINFO pici);
    HRESULT Rename(LPCMINVOKECOMMANDINFO pici);
    HRESULT Properties(LPCMINVOKECOMMANDINFO pici);
    HRESULT NewFolder(LPCMINVOKECOMMANDINFO pici);
    HRESULT NewConsole(LPCMINVOKECOMMANDINFO pici);
    HRESULT CutCopy(bool fCut);

    // Also supported, but not in the menu
    HRESULT CreateShortcut(LPCMINVOKECOMMANDINFO pici);
    
    void ExploreOpen(LPCMINVOKECOMMANDINFO pici, BOOL fOpen);
    

    /*
    **  A CXboxFolder selection clone that contains the context.
    */

    DWORD        m_dwVerbs;
    UINT         m_uItemCount;
    CXboxFolder *m_pSelection;

    static MENU_ITEM_ENTRY sm_MenuItems[];
    UINT m_uIdOffset;
    
};

#endif //__XBOXMENU_H__