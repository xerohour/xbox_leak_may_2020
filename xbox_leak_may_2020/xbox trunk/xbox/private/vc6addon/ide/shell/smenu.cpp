// menu.cpp : Management of menu bar and command hierarchy
//

#include "stdafx.h"

#include <ctype.h>

#include "package.h"
#include "shlmenu.h"
#include "mainfrm.h"
#include "bardockx.h"
#include "resource.h"
#include "barglob.h"
#include "cmduiena.h"
#include "toolexpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// This little collection of macros determines the size of string buffer required to hold an unsigned short, excluding
// string terminator. It determines this at compile time, which means that it doesn't impose any retail runtime overhead.
// There is a runtime overhead in the debug build, because the string won't get optimised away in that one. However, this
// is completely portable to other limits.h settings. martynl 25Jun96
// makes a literal into a string
#define STRINGIZE(x) (# x)
// expands a macro and then makes that into a string
#define STRINGIZE_MACRO(x) STRINGIZE(x)
// determines the buffersize (excluding terminator) required by a numeric macro
#define BUFFERSIZE_MACRO(x) (sizeof(STRINGIZE_MACRO(x))-1)
#define COMMAND_ID_BUFFERSIZE BUFFERSIZE_MACRO(USHRT_MAX)

// define this to 1 to show trace messages on suspicious empty groups (groups which contain no commands in a particular execution of process commands). Useful for rooting out litter from the menu structures
#define FIND_EMPTY_GROUPS 0

// define this to 1 to show trace messages on suspicious commands (commands that don't belong to any group in a menu structure, but which don't have the no menu flag present). Useful for rooting out missing flags, but also happens validly
#define FIND_BAD_GROUPS 0

static char BASED_CODE szMenuKey[] = "Menu%x";
static char BASED_CODE szPackagesKey[] = "Packages";

//CObject
	class CCmdObject;
		class CCmd;
			class CCmdMenu;
		class CCmdGroup;
			class CMainGroup;	// A complete menu bar


#define ANYCMD ((WORD) -1)
#define ANYENABLEDCMD ((WORD) -2)

typedef CTypedPtrArray<CPtrArray, CCmdGroup *> CCmdGroupArray;

class CCmdObject : public CObject
{
public:
	// Since a group can appear in more than one menu, this now supports returning all groups in an array, or just the first group (bFirst==TRUE)
	virtual BOOL		FindGroups(WORD, CCmdGroupArray &groups, BOOL bFirst=FALSE) { return NULL; };
	virtual CCmd*		FindCmd(WORD) = 0;
	virtual int 		RebuildMenus(CMenu* pMenu) = 0;
	virtual int 		RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive) = 0;
};

class CCmd : public CCmdObject
{
public:
	CCmd(WORD id, WORD flags, WORD group, LPCTSTR sz);

	virtual CCmd*	FindCmd(WORD);
	virtual int 	RebuildMenus(CMenu* pMenu);
	virtual int 	RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive);

	LPCTSTR 	m_szMenu;
	CCmdGroup*	m_pParent;
	WORD m_id;
	WORD m_flags;
	WORD m_group;
};

class CCmdMenu : public CCmd
{
public:
						CCmdMenu(WORD id, WORD flags, WORD group, const CString &menuName, CPackage *pPackage);
	virtual				~CCmdMenu();
	virtual	BOOL		Add(CCmdGroup* pCmdGroup);
	virtual BOOL		FindGroups(WORD, CCmdGroupArray &groups, BOOL bFirst=FALSE);
	virtual CCmd*		FindCmd(WORD);
	virtual int 		RebuildMenus(CMenu* pMenu);
	virtual int 		RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive);
	virtual int			AddMenu(CBMenuPopup* pMenu);
	virtual void		AddItems(CBMenuPopup *pMenu, BOOL bRecursive);
	

	CObList	m_children;
	CPackage *m_pPackage;
	CString m_menuName; // (m_szMenu points at this)
};

class CCmdGroup : public CCmdObject
{
public:
						CCmdGroup(WORD id = -1);
	virtual				~CCmdGroup();
	virtual BOOL		Add(CCmd* pCmd, BOOL bFirst = FALSE);
	       	BOOL		IsRoot()
		   	    			{ return (m_pParent == NULL); };
	virtual BOOL		FindGroups(WORD, CCmdGroupArray &groups, BOOL bFirst=FALSE);
	virtual CCmd*		FindCmd(WORD);
	virtual int 		RebuildMenus(CMenu* pMenu);
	virtual int 		RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive);

	WORD		m_id;
	CObList 	m_children;
	CCmdMenu*	m_pParent;
};

// used when a separator appears literally in a menu (used only by OLE at the moment)
class CCmdGroupSeparator : public CCmdGroup
{
public:
						CCmdGroupSeparator(WORD id = -1);
	virtual				~CCmdGroupSeparator();
	virtual BOOL		Add(CCmd* pCmd, BOOL bFirst = FALSE);
	       	BOOL		IsRoot()
		   	    			{ return (m_pParent == NULL); };
	virtual BOOL		FindGroups(WORD, CCmdGroupArray &groups, BOOL bFirst=FALSE);
	virtual CCmd*		FindCmd(WORD);
	virtual int 		RebuildMenus(CMenu* pMenu);
	virtual int 		RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive);
};

class CMainGroup : public CCmdGroup
{
public:
						CMainGroup();
						~CMainGroup();
	virtual BOOL		Add(CCmd* pCmd);
	
	void BuildMenu(CMenu* pMenu);
	void BuildMenu(CBMenuPopup* pMenu);
	void BuildSingleMenu(CBMenuPopup *pMenu, int nCmdID);
	void ResetMenu(UINT nID, BOOL bRecurse=TRUE);
};

static HMENU hMenuWindow = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCmd

CCmd::CCmd(WORD id, WORD flags, WORD group, LPCTSTR sz)
{
    ASSERT(id != -1);   // reserved value
    m_szMenu = sz;
    m_pParent = NULL;
    m_id = id;
    m_flags = flags;
    m_group = group;
}

int CCmd::RebuildMenus(CMenu* pMenu)
{
    ASSERT(m_pParent != NULL);
	ASSERT(m_szMenu != NULL);
    
	CString strItem = m_szMenu;
	CString strKey;
	if (GetCmdKeyString(m_id, strKey))
	{
		strItem += '\t';
		strItem += strKey;
	}
    pMenu->AppendMenu(MF_STRING | MF_GRAYED, m_id, strItem);
    return 1;
}

int CCmd::RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive)
{
    ASSERT(m_pParent != NULL);
	ASSERT(m_szMenu != NULL);
    
	CString strItem = m_szMenu;
	CString strKey;
	if (GetCmdKeyString(m_id, strKey))
	{
		strItem += '\t';
		strItem += strKey;
	}
    pMenu->AppendMenu(MF_STRING | MF_GRAYED, m_id, strItem);
    return 1;
}

CCmd* CCmd::FindCmd(WORD id)
{
    if ((id == m_id) || (id == (WORD) ANYCMD) || (id == (WORD) ANYENABLEDCMD))
        return this;
    
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdMenu

CCmdMenu::CCmdMenu(WORD id, WORD flags, WORD group, const CString &menuName, CPackage *pPackage)
    :	CCmd(id, flags, group, NULL)
{
	m_menuName=menuName;
	m_szMenu=(LPCTSTR)m_menuName;
	m_pPackage=pPackage;
}

CCmdMenu::~CCmdMenu()
{
    CCmdGroup* pGroup;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pGroup = (CCmdGroup*) m_children.GetNext(pos);
        delete pGroup;
    }
}

BOOL CCmdMenu::Add(CCmdGroup* pCmdGroup)
{
    ASSERT(pCmdGroup->m_pParent == NULL);
    
    pCmdGroup->m_pParent = this;
    m_children.AddTail(pCmdGroup);
    return TRUE;
}

BOOL CCmdMenu::FindGroups(WORD idGroup, CCmdGroupArray &groups, BOOL bFirst)
{
    CCmdGroup* pGroup;
    BOOL bFound=FALSE;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pGroup = (CCmdGroup*) m_children.GetNext(pos);
        bFound |= pGroup->FindGroups(idGroup, groups, bFirst);
		if(bFirst && bFound)
		{
			return TRUE;
		}
    }
    
    return bFound;
}

CCmd* CCmdMenu::FindCmd(WORD id)
{
	if(id==m_id)
	{
		return this;
	}

    CCmdGroup* pGroup;
    CCmd* pFound;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pGroup = (CCmdGroup*) m_children.GetNext(pos);
        if ((pFound = pGroup->FindCmd(id)) != NULL)
            return pFound;
    }
    
    return NULL;
}

int CCmdMenu::RebuildMenus(CMenu* pMenu)
{
	if(m_pPackage)
	{
		if(m_pPackage->IsMenuVisible(m_id)==FALSE)
		{
			return 0;
		}
	}

	ASSERT(m_pPackage!=NULL);
    ASSERT(m_pParent != NULL);
    
    CMenu menuPopup;
    menuPopup.CreateMenu();
    
    CCmdGroup* pGroup;
	int nPrevGroup=0;

    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pGroup = (CCmdGroup*) m_children.GetNext(pos);
        ASSERT(pGroup->m_pParent == this);

        if (pGroup->FindCmd(ANYCMD) != NULL)
        {
            // Add separator if needed
            if (nPrevGroup!=0 && ((nPrevGroup & 1) == 0))
                menuPopup.AppendMenu(MF_SEPARATOR);
            pGroup->RebuildMenus(&menuPopup);
            nPrevGroup = pGroup->m_id;
        }
		else
		{
#if FIND_EMPTY_GROUPS
			TRACE1("CCmdMenu::RebuildMenus: Empty group suspect %d found\n\r", pGroup->m_id);
#endif
		}
    }

    UINT flags = MF_POPUP;
    if (FindCmd(ANYENABLEDCMD) == NULL)
    {
        if (FindCmd(ANYCMD) == NULL)
            return 0;
        else
            flags = MF_POPUP | (m_pParent->IsRoot() ? 0 : MF_GRAYED);
    }
    
	CString strMenu;
	if (m_szMenu[0] == _T('\0'))
		strMenu = " ";
	else
		strMenu = m_szMenu;
    pMenu->AppendMenu(flags, (UINT)menuPopup.m_hMenu, strMenu);

	if (m_id==IDM_MENU_WINDOW)
		hMenuWindow = menuPopup.m_hMenu;

    menuPopup.Detach(); // So popup won't be destroyed on scope exit.

    return 1;
}

int CCmdMenu::AddMenu(CBMenuPopup* pMenu)
{
	CString strMenu;
	if (m_szMenu == NULL)
		strMenu = " ";
	else
		strMenu=m_szMenu;
	// append this as a deferred action popup
    pMenu->AppendMenu(MF_POPUP, m_id, strMenu);

	return 1;
}

void CCmdMenu::AddItems(CBMenuPopup* pMenu, BOOL bRecursive)
{
	ASSERT(m_pParent != NULL);

	CCmdGroup* pGroup;
	int nPrevGroup = 0;

	POSITION pos = m_children.GetHeadPosition();
	while (pos != NULL)
	{
		pGroup = (CCmdGroup*) m_children.GetNext(pos);
		ASSERT(pGroup->m_pParent == this);

		if (pGroup->FindCmd(ANYCMD) != NULL)
		{
			// Add separator if needed
            if (nPrevGroup!=0 && ((nPrevGroup & 1) == 0))
				pMenu->AppendMenu(MF_SEPARATOR);
			pGroup->RebuildMenus(pMenu, bRecursive);
			nPrevGroup = pGroup->m_id;
		}
		else
		{
#if FIND_EMPTY_GROUPS
			TRACE1("CCmdMenu::AddItems: Empty group suspect %d found\n\r", pGroup->m_id);
#endif
		}
	}
}

int CCmdMenu::RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive)
{
	// if this menu can be loaded, we should do that
	ASSERT(CDockWorker::s_lpszLayoutSection != NULL);

	CBMenuPopup *menuPopup=new CBMenuPopup;
	menuPopup->CreateMenu();
	menuPopup->SetAutoEnable(TRUE);

	AddItems(menuPopup, bRecursive);
    
	AddMenu(pMenu);
    
	theCmdCache.SetMenu(m_id, menuPopup);

    return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdGroup

CCmdGroup::CCmdGroup(WORD id)
{
    m_id = id;
    m_pParent = NULL;
}

CCmdGroup::~CCmdGroup()
{
    CCmd* pItem;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pItem = (CCmd*) m_children.GetNext(pos);
        delete pItem;
    }
}

BOOL CCmdGroup::Add(CCmd* pCmd, BOOL bFirst /*=FALSE*/)
{
    ASSERT(pCmd->m_pParent == NULL);

    pCmd->m_pParent = this;
    if (bFirst)
        m_children.AddHead(pCmd);
    else
        m_children.AddTail(pCmd);
    return TRUE;
}

BOOL CCmdGroup::FindGroups(WORD idGroup, CCmdGroupArray &groups, BOOL bFirst)
{
	// if this is one of them
    if (idGroup == m_id)
	{
		// add it to the array
		groups.Add(this);

		if(bFirst)
		{
			return TRUE;
		}
	}

    CCmd* pItem;
    BOOL bFound=FALSE;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pItem = (CCmd*) m_children.GetNext(pos);
        bFound|=pItem->FindGroups(idGroup, groups, bFirst);
		if(bFirst && bFound)
		{
			return TRUE;
		}
    }
    
    return bFound;
}

CCmd* CCmdGroup::FindCmd(WORD id)
{
    CCmd* pItem;
    CCmd* pFound;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pItem = (CCmd*) m_children.GetNext(pos);
        if ((pFound = pItem->FindCmd(id)) != NULL)
            return pFound;
    }
    
    return NULL;
}

int CCmdGroup::RebuildMenus(CMenu* pMenu)
{
    // Iterate through all child CmdItems and add them to the menu
    int count = 0;
    CCmd* pCmd;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pCmd = (CCmd*) m_children.GetNext(pos);
        ASSERT(pCmd->m_pParent == this);
        count += pCmd->RebuildMenus(pMenu);
    }

    // return total number of command items added
    return count;
}

int CCmdGroup::RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive)
{
    // Iterate through all child CmdItems and add them to the menu
    int count = 0;
    CCmd* pCmd;
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        pCmd = (CCmd *)(m_children.GetNext(pos));
        ASSERT(pCmd->m_pParent == this);
		if(pCmd->m_flags & CT_MENU)
		{
			// don't recurse down into submenus, just add this to menu
			count += ((CCmdMenu *)pCmd)->AddMenu(pMenu);
		}
		else
		{
			count += pCmd->RebuildMenus(pMenu, bRecursive);
		}
    }

    // return total number of command items added
    return count;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdGroupSeparator

CCmdGroupSeparator::CCmdGroupSeparator(WORD id) : CCmdGroup(id)
{
}

CCmdGroupSeparator::~CCmdGroupSeparator()
{
}

BOOL CCmdGroupSeparator::Add(CCmd* pCmd, BOOL bFirst /*=FALSE*/)
{
	// can't add to a separator
	ASSERT(FALSE);
    return TRUE;
}

BOOL CCmdGroupSeparator::FindGroups(WORD , CCmdGroupArray &, BOOL)
{
	return FALSE;
}

CCmd* CCmdGroupSeparator::FindCmd(WORD id)
{
    return NULL;
}

int CCmdGroupSeparator::RebuildMenus(CMenu* pMenu)
{
	pMenu->AppendMenu(MF_SEPARATOR);

    // return total number of command items added
    return 1;
}

int CCmdGroupSeparator::RebuildMenus(CBMenuPopup* pMenu, BOOL bRecursive)
{
	pMenu->AppendMenu(MF_SEPARATOR);
    return 1;
}

/////////////////////////////////////////////////////////////////////////////
//	Class CMainGroup

CMainGroup::CMainGroup() :
    CCmdGroup()
{
}

CMainGroup::~CMainGroup()
{
}

BOOL CMainGroup::Add(CCmd* pCmd)
{
    // At present, we only support pCmdMenu addition
    CCmdMenu* pCmdMenu = (CCmdMenu*) pCmd;

    ASSERT(pCmdMenu->m_pParent == NULL);
    pCmdMenu->m_pParent = this;
    m_children.AddTail(pCmdMenu);
    
    return TRUE;
}

void CMainGroup::BuildMenu(CMenu* pMenu)
{
    CCmdGroup* pCmdGroup;
    CCmdMenu* pMainMenu;
    
	// child of the main group is the main menu bar
    POSITION mainpos = m_children.GetHeadPosition();
	pMainMenu = (CCmdMenu*) m_children.GetNext(mainpos);

	POSITION pos =pMainMenu->m_children.GetHeadPosition();
	
    while (pos != NULL)
    {
        // At present, we only support pCmdMenu addition
        pCmdGroup = (CCmdGroup*) pMainMenu->m_children.GetNext(pos);
        ASSERT(pCmdGroup->m_pParent == pMainMenu);

        pCmdGroup->RebuildMenus(pMenu);
    }
}

void CMainGroup::BuildMenu(CBMenuPopup* pMenu)
{
    CCmdMenu* pCmdMenu;
    
    POSITION pos = m_children.GetHeadPosition();
    while (pos != NULL)
    {
        // At present, we only support pCmdMenu addition
        pCmdMenu = (CCmdMenu*) m_children.GetNext(pos);
        ASSERT(pCmdMenu->m_pParent == this);

        pCmdMenu->RebuildMenus(pMenu, TRUE);
    }
}

void CMainGroup::BuildSingleMenu(CBMenuPopup *pMenu, int nCmdID)
{
	CCmdMenu* pCmdMenu=(CCmdMenu *)(FindCmd((WORD)nCmdID));

	if(pCmdMenu)
	{
		ASSERT(pCmdMenu->m_flags & CT_MENU);

		pCmdMenu->AddItems(pMenu, FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame menu stuff

void CCmdCache::AddPopToGroup(POPDESC* ppop, CCmdGroup* pGroup)
{
	CTE* pCTE = GetCommandEntry(ppop->cmdID);

	LPCTSTR pszMenu;
	VERIFY(GetCommandString(ppop->cmdID, STRING_MENUTEXT, &pszMenu));
	CString menuName=pszMenu;
	CCmdMenu* pCmdMenu = new CCmdMenu(pCTE->id, pCTE->flags, pCTE->group, menuName, pCTE->GetPack()->GetPackage());
	pGroup->Add(pCmdMenu);
	
	MTM* pmtm = &ppop->rgmtm[0];
	UINT idLast = 0;
	CCmdGroup* pCmdGroupLast = NULL;
	while (pmtm->id != 0)
	{
		if(pmtm->id==POP_SEPARATOR)
		{
			pCmdGroupLast = new CCmdGroupSeparator((WORD)pmtm->id);
			pCmdMenu->Add(pCmdGroupLast);
			idLast = pmtm->id;
		}
		else
		{
			if (pmtm->id != idLast)
			{
				pCmdGroupLast = new CCmdGroup((WORD)pmtm->id);
				pCmdMenu->Add(pCmdGroupLast);
				idLast = pmtm->id;
			}

			if (pmtm->ppop != NULL)
			{
				ASSERT(pCmdGroupLast != NULL);
				ASSERT(pmtm->ppop != NULL); // duplicate group id in popup?
				AddPopToGroup(pmtm->ppop, pCmdGroupLast);
			}
			// search all the menus in the command table for any menus in the specified group, and add them
			for (int i = 0; i < m_rgMenuCommands.GetSize() ; i++)
			{
				ASSERT(m_rgMenuCommands[i]<theCmdCache.m_cCommands);

				pCTE = &m_rgCommands[theCmdCache.m_rgMenuCommands[i]];

				ASSERT(pCTE);
				ASSERT(pCTE->flags & CT_MENU);

				if(pCTE->group == pmtm->id)
				{
					ASSERT(pCTE->GetPack());
					ASSERT(pCTE->GetPack()->GetPackage());

					POPDESC *pPopDesc=pCTE->GetPack()->GetPackage()->GetMenuDescriptor(pCTE->id);

					ASSERT(pPopDesc);
					AddPopToGroup(pPopDesc, pCmdGroupLast);
				}
			}
		}
		pmtm++;
	}
}

void CCmdCache::ProcessCommands(CMainGroup* pMainGroup, BOOL bRestrict, BOOL bOLE, BOOL bMenuOnly)
{
	POPDESC* pBarPopDesc;
	if(bOLE)
	{
		pBarPopDesc=theApp.GetMenuDescriptor(IDM_MENU_OLEBAR);
	}
	else
	{
		pBarPopDesc=theApp.GetMenuDescriptor(IDM_MENU_BAR);
	}

	AddPopToGroup(pBarPopDesc, pMainGroup);

	// Add menuitems.
    CTE* pCTE;
    CCmdGroup* pGroup;
	CPacket* pPack;
	
	for (int i = 0; i < m_cCommands; i++)
	{
		pCTE = &m_rgCommands[i];

		if(bMenuOnly)
		{
			if (pCTE->flags & CT_NOMENU)
				continue;
		}

		if ((pCTE->flags & (CT_NOUI | CT_MENU)) != 0)
			continue;

		if(bRestrict)
		{
			// when building traditional menu, leave off querymenu menus
			if ((pCTE->flags & CT_QUERYMENU) != 0)
				continue;

			if(!pCTE->IsActive())
			{
				continue;
			}
		}

		// Find all groups associated with this command 
		CCmdGroupArray rgGroups;
		pMainGroup->FindGroups(pCTE->group, rgGroups);

		// now iterate over them
		int nGroups=rgGroups.GetSize();
		for(int iGroup=0; iGroup<nGroups; ++iGroup)
		{
			CCmdGroup *pGroup=rgGroups[iGroup];

			LPCTSTR lpszText = NULL;
			theCmdCache.GetCommandString(pCTE->id, STRING_MENUTEXT, &lpszText, NULL, pCTE);

			// add the command to the group
			pGroup->Add(new CCmd(pCTE->id, pCTE->flags, pCTE->group, lpszText),	pCTE->flags & CT_TOP);

		}

		if(nGroups==0 && !bOLE)
		{
			// This warning means somebody tried to use a command group which doesn't 
			// exist on any menu.  Perhaps they intended to use the CT_NOMENU flag?

			// Another cause of this can be a command that's on the menu in a different EXE, such as some infoviewer commands.
			// Known instances of this are special-cased here:
			if(	pCTE->group!=IDG_IV_TOOLS_SEARCH &&
				pCTE->group!=IDG_IV_HELP_CONTENTS &&
				pCTE->group!=IDG_IV_HELP_ABOUT)
			{
#if FIND_BAD_GROUPS
				TRACE1("CCmdCache::ProcessCommands: Unexpected group %d\n", pCTE->group);
#endif
			}
		}
    }
}

HMENU GetCommandMenu(BOOL bOleIPMenu)
{
	if (Menu::IsShowingCmdBars() && !bOleIPMenu)	//OLD_MENU
	{
		// We are currently in command bar mode, so we will just be polite
		// and return NULL.
		return NULL ;
	}

	CMainGroup mainGroup;
    theCmdCache.ProcessCommands(&mainGroup, TRUE, bOleIPMenu);
    
    CMenu menuBar;
    menuBar.CreateMenu();
    
    mainGroup.BuildMenu(&menuBar);

    return menuBar.Detach();
}

// Apply flags now fixes a deeply devious bug. Imagine that you have a menu M that contains a package specific menu P1. You
// customize P1 to contain menu O. Then you restart devstudio with P1 not loaded. You then customize O to contain M. Next
// time you restart devstudio, your menus are in a cycle. In this case, we break the cycle at an arbitrary point (the point furthest from the 
// top left of the menu bar, depth first.

// applys invisibility flags to a menu, recursing as appropriate. Fixes any broken recursive menus. 
// Never counts the same menu twice.

// In debug builds, this routine now checks for duplicate mnemonic letters in non-customized menus. Customized menus are omitted (because they
// could have been duplicated by users). Only visible commands are checked, which allows two different editors to use the same mnemonic, potentially

#ifdef _DEBUG
#define CHECK_MNEMONICS 1
#else
#define CHECK_MNEMONICS 0
#endif

static BOOL ApplyFlags(UINT nIDMenu, BOOL bAllEditors, BOOL bAllProjects, BOOL bEmptyMenus)
{
	CBMenuPopup *pPopup=theCmdCache.GetMenu(nIDMenu);
	CASBar *pMenuBar=CASBar::GetMenuBar();

#if CHECK_MNEMONICS
	BOOL abLetterUsed[26];					// One bool for each letter. TRUE if the letter is used
	for(int i=0; i<(sizeof(abLetterUsed)/sizeof(BOOL)); ++i)
	{
		abLetterUsed[i]=FALSE;
	}
#endif

	// should be unknown to start with
	ASSERT(pPopup->m_nVisibleItems==-1);

	// stop any infinite recursion
	pPopup->m_nVisibleItems=-2;

	// so now we need to decide which items are visible
	int nVisibleItems=0;

	BOOL bBarModified=FALSE;

	CTE *pCTE=theCmdCache.GetCommandEntry(nIDMenu);
	CPackage *pPackage=NULL;
	if(	pCTE &&													// All menus should have a CTE
		pCTE->GetPack() &&										// and a pack
		(pPackage=pCTE->GetPack()->GetPackage())!=NULL &&		// and a package, but let's be safe
		pPackage->IsMenuVisible(nIDMenu)==FALSE)				// check if it's currently hidden
	{
		nVisibleItems=0;
	}
	else
	{
		int nLastVisibleItem=-1;
		int nLastSeparator=-1;

		for(int iItem=0;iItem<pPopup->GetCount(); ++iItem)
		{
			CBMenuItem *pItem=pPopup->GetMenuItem(iItem);

			BOOL bVisible=TRUE;

			if(pItem)
			{
				if(	pItem->m_pCTE &&
					pItem->m_pCTE->flags & CT_MENU)
				{
					// a submenu
					CBMenuPopup *pMenu=theCmdCache.GetMenu(pItem->m_pCTE->id);
					if(pMenu)
					{
						switch(pMenu->m_nVisibleItems)
						{
							case -2:
								// circular menu structure
								// We remove the cycle by cutting out this item
								pMenu->DeleteMenu(MF_BYPOSITION, iItem);

								// then decrement i so that we don't skip an item
								--iItem;

								// and continue the for loop since pItem is now invalid
								continue;

							case -1:
								// uncalculated menu
								bBarModified|=ApplyFlags(pItem->m_pCTE->id, bAllEditors, bAllProjects, bEmptyMenus);
								// must have counted now
								ASSERT(pMenu->m_nVisibleItems>=0);
								if(pMenu->m_nVisibleItems==0)
								{
									if(bEmptyMenus && pMenu->GetMenuItemCount()==0 && (pMenu->IsDirty() || (pItem->m_pCTE->id>=IDM_CUSTOMMENU_BASE && pItem->m_pCTE->id<=IDM_CUSTOMMENU_LAST)))
									{
										// A menu is visible if we're showing empty menus and it's an empty custom menu
										bVisible=TRUE;
									}
									else
									{
										bVisible=FALSE;
									}
								}
								break;

							case 0:
								// we already know this is empty
								if(bEmptyMenus && pMenu->GetMenuItemCount()==0 && (pMenu->IsDirty() || (pItem->m_pCTE->id>=IDM_CUSTOMMENU_BASE && pItem->m_pCTE->id<=IDM_CUSTOMMENU_LAST)))
								{
									// A menu is visible if we're showing empty menus and it's an empty custom menu
									bVisible=TRUE;
								}
								else
								{
									bVisible=FALSE;
								}
								break;

							default:
								if(pMenu->m_nVisibleItems>0)
								{
									// we already know this is used and visible
								}
								else
								{
									ASSERT(FALSE);
								}
								break;
						}

					}
					else
					{
						bVisible=FALSE;
					}
				}
				else
				{
					if( pItem->GetItemType()==CBMenuItem::MIT_Separator)
					{
						if(nLastVisibleItem==nLastSeparator)
						{
							bVisible=FALSE;
						}
						else
						{
							nLastSeparator=iItem;
						}
					}
					else
					{
						if(	pItem->m_pCTE &&
							pItem->m_pCTE->GetPack())
						{
							// suppress QUERYMENU items
							if(pItem->m_pCTE->flags & CT_QUERYMENU)
							{
								if(	(pItem->m_pCTE->flags & CT_PROJECT_DEPENDENT)!=0 &&
									bAllProjects)
								{
									// If we're showing all projects, then ignore the query menu that invisible project dependant commands have
									bVisible=TRUE;
								}
								else
								{
									bVisible=FALSE;
								}
							}

							// otherwise, suppress for wrong packet
							if(	!bAllEditors && 
								!pItem->m_pCTE->IsActive())
							{
								bVisible=FALSE;
							}

							// if it's visible, update its accelerator
							if(bVisible)
							{
								if(bAllEditors)
								{
									// when we're showing menus from all editors, there's no definitive set of keys, so don't show them.
									pItem->SetAccelerator("", FALSE);
								}
								else
								{
									// find out short cut
									CString strKey;
									if (GetCmdKeyString(pItem->m_pCTE->id, strKey))
									{
										// update in item, saving recalc for later
										pItem->SetAccelerator(strKey, FALSE);
									}
									else
									{
										// update in item, saving recalc for later
										pItem->SetAccelerator(NULL, FALSE);
									}

								}
							}
						}
						else
						{
							// no CTE implies the menu item belongs to an absent package
							bVisible=FALSE;
						}
					}
				}
			}

			if(!bVisible)
			{
				pItem->m_bVisible=FALSE;
			}
			else
			{
				nLastVisibleItem=iItem;
				pItem->m_bVisible=TRUE;
				if( pItem->GetItemType()!=CBMenuItem::MIT_Separator)
				{
					++nVisibleItems;

					// If item has been modified since last rebuild, we need to update its text
					if(pItem->IsTextModified() && pItem->m_nCmdID != 0)
					{
						LPCTSTR lpszMenuText=NULL;

						// get the default menu text
						VERIFY(theCmdCache.GetCommandString(pItem->m_nCmdID, STRING_MENUTEXT, &lpszMenuText, NULL, pItem->m_pCTE));

						// ensure it wasn't empty
						ASSERT(lpszMenuText[0]!='\0');
	
						// reset the menu text
						pItem->SetText(lpszMenuText);
						pItem->SetTextModified(FALSE);
					}
#if CHECK_MNEMONICS
					// don't check in all editors mode (clashes are bound to happen), or when the menu has been customized
					if(!bAllEditors && !pPopup->IsDirty())
					{
						// ignore disabled commands
						CCmdUIEnable state;
						state.m_nID = pItem->m_pCTE->id;
						state.DoUpdate(AfxGetApp()->m_pMainWnd, TRUE);

						if(state.m_bEnabled)
						{
							// ensure that this item isn't a duplicate of any other
							char chAccel=GLOBAL_DATA::ExtractAccelerator(pItem->GetName());

							if(isalpha(chAccel))
							{
								char chLower=(char)tolower(chAccel);

								int nIndex=chLower-'a';

								// FAILURES imply some kind of logic error, or a strange locale is in force
								ASSERT(nIndex>=0);		
								ASSERT(nIndex<(sizeof(abLetterUsed)/sizeof(BOOL)));

								if(nIndex>=0 && nIndex<(sizeof(abLetterUsed)/sizeof(BOOL)))
								{
									// If this fails, then the menu being built contains the same accelerator twice. You should fix it in the command table
									// or ccmdui handler. We don't make any attempt to fix this up, since this is a debug-only check.
									ASSERT(abLetterUsed[nIndex]==FALSE);

									// this mnemonic has now been used.
									abLetterUsed[nIndex]=TRUE;
								}
							}
						}
					}
#endif
				}
			}
			pItem->m_bNeedToCalculateSizes=TRUE;
		}
		// remove bogus last separator
		if(nLastVisibleItem==nLastSeparator &&
			nLastVisibleItem!=-1)
		{
			CBMenuItem *pItem=pPopup->GetMenuItem(nLastVisibleItem);
			pItem->m_bVisible=FALSE;
		}
		pPopup->CalcMenuSize();
	}

	// A menu is visible if it has visible items, or if we're showing empty menus and it's an empty custom menu
	BOOL bVisible=(nVisibleItems>0) || (bEmptyMenus && pPopup->GetMenuItemCount()==0 && (pPopup->IsDirty() || (nIDMenu>=IDM_CUSTOMMENU_BASE && nIDMenu<=IDM_CUSTOMMENU_LAST)));

	pPopup->m_nVisibleItems=nVisibleItems;

	return bBarModified;
}

static HMENU s_hOleMenu = NULL;

void CMainGroup::ResetMenu(UINT nID, BOOL bRecurse /*=TRUE*/)
{
	// build the menu from scratch
	CBMenuPopup *pMenu=new CBMenuPopup;
	pMenu->CreateMenu();
	pMenu->SetAutoEnable(TRUE);
	BuildSingleMenu(pMenu, nID);

	// Don't need to delete the old one - cmdcache will do it for us
	theCmdCache.SetMenu(nID, pMenu);

	// prepare to recalculate visibility
	pMenu->m_nVisibleItems=-1;

	// only do submenus if requested
	if(bRecurse)
	{
		// now reset all of its children
		int nItems=pMenu->GetMenuItemCount();
		for(int iItem=0;iItem<nItems; ++iItem)
		{
			CBMenuItem *pItem=pMenu->GetMenuItem(iItem);

			if(	pItem &&
				pItem->m_pCTE &&
				pItem->m_pCTE->flags & CT_MENU)
			{
				ResetMenu(pItem->m_pCTE->id);
			}
		}
	}
}

// resets a single menu to its default state
void CMainFrame::ResetMenu(UINT nID)
{
	// In debug, check that this is only being called for a menu
#ifdef _DEBUG
	{
		CTE *pCTE=NULL;
		ASSERT((pCTE=theCmdCache.GetCommandEntry(nID))!=NULL);
		ASSERT(pCTE->flags & CT_MENU);
	}
#endif

	// reprocess all of the commands - time consuming!
	CMainGroup mainGroup;
	theCmdCache.ProcessCommands(&mainGroup, FALSE);

	mainGroup.ResetMenu(nID);

	// now apply invisibility flags to the menu, and all of its current submenus
	ApplyFlags(nID, FALSE, FALSE, FALSE);
}

// resets a single menu to its default state
void CMainFrame::ResetAllMenus()
{
	CASBar::CancelMenu();

	// reprocess all of the commands - time consuming!
	CMainGroup mainGroup;
	theCmdCache.ProcessCommands(&mainGroup, FALSE);

	// search all the menus in the command table for any menus
	for (int i = 0; i < theCmdCache.m_rgMenuCommands.GetSize() ; i++)
	{
		ASSERT(theCmdCache.m_rgMenuCommands[i]<theCmdCache.m_cCommands);

		CTE *pCTE = &theCmdCache.m_rgCommands[theCmdCache.m_rgMenuCommands[i]];

		ASSERT(pCTE);
		ASSERT(pCTE->flags & CT_MENU);

		mainGroup.ResetMenu(pCTE->id, FALSE);
	}

	// now apply invisibility flags to the menus
	RebuildMenus();
}

// fills a list box with all commands that are missing from the menus
void CMainFrame::FillDeletedMenuItemList(CListBox *pList)
{
	// don't reprocess all of the commands until we know we need them
	CMainGroup *pMainGroup=NULL;

	// iterate over all menus
	for (int i = 0; i < theCmdCache.m_rgMenuCommands.GetSize() ; i++)
	{
		ASSERT(theCmdCache.m_rgMenuCommands[i]<theCmdCache.m_cCommands);

		CTE *pCTE = &theCmdCache.m_rgCommands[theCmdCache.m_rgMenuCommands[i]];

		// Now work out what is missing from this menu
		CBMenuPopup *pMenuCurrent=theCmdCache.GetMenu(pCTE->id);
		ASSERT(pMenuCurrent);

		// don't bother to compare if the menu hasn't been customized
		if(pMenuCurrent->IsDirty())
		{
			// reprocess all the commands. Time consuming, but the only way to work out the defaults
			if(pMainGroup==NULL)
			{
				pMainGroup=new CMainGroup;
				theCmdCache.ProcessCommands(pMainGroup, FALSE);
			}

			// Create a 'reset' menu to compare with the current one
			CBMenuPopup *pMenuDefault=new CBMenuPopup;
			pMenuDefault->CreateMenu();
			pMainGroup->BuildSingleMenu(pMenuDefault, pCTE->id);

			if(	pMenuCurrent && // just to be safe
				pMenuDefault)
			{
				// Now we have to compare the two menus. We iterate over the default menu, searching for each command in the
				// current menu. Since the current menu has probably not changed much, we start looking for the next command
				// just after where we found the previous one. This will tend to mean that the whole comparison isn't too slow.

				// This is the item we start looking at in the current menu
				int iInitialCurrentItem=0;

				// count of items in both menus
				int nCurrentItems=pMenuCurrent->GetMenuItemCount();
				int nDefaultItems=pMenuDefault->GetMenuItemCount();

				// iterate over default menu
				for(int iDefaultItem=0; iDefaultItem<nDefaultItems; ++iDefaultItem)
				{
					UINT nDefaultId=pMenuDefault->GetMenuItemID(iDefaultItem);

					// if it's not a separator or in some other way dud.
					if(nDefaultId!=0)
					{
						// iterate over current menu
						int iCurrentItem=iInitialCurrentItem;

						BOOL bFound=FALSE;

						do
						{
							UINT nCurrentId=pMenuCurrent->GetMenuItemID(iCurrentItem);

							// if it's the same as the one in the default menu, we just found it.
							if(nCurrentId==nDefaultId)
							{
								bFound=TRUE;
							}

							// move to next item to check; wrap around, since we don't always start at 0.
							++iCurrentItem;
							if(iCurrentItem>=nCurrentItems)
							{
								iCurrentItem=0;
							}

							// if we just found it, then the current index value is the value to start at next time.
							if(bFound)
							{
								iInitialCurrentItem=iCurrentItem;
							}
						}
						while(!bFound && iCurrentItem!=iInitialCurrentItem);

						// add the missing command to the list box
						if(!bFound)
						{
							// get the command table entry for the missing command from the menu, which caches it.
							CBMenuItem *pMissingItem=pMenuDefault->GetMenuItem(iDefaultItem);

							CTE *pCTEMissing=pMissingItem->m_pCTE;
							
							// if the command is currently in one of the loaded packages
							if(pCTEMissing)
							{
								// Add the command to the list box, if it's not already in there
								LPCTSTR lpszCmdName = pCTEMissing->szCommand;
								int nOldIndex=pList->FindStringExact(0, lpszCmdName);
								if(nOldIndex==LB_ERR)
								{
									int index = pList->AddString(lpszCmdName);
									if(index!=LB_ERR)
									{
										pList->SetItemData(index, (DWORD) pCTEMissing->id);
									}
								}
							}
						}
					}
				}
			}

			delete pMenuDefault;
		}
	}	
}

// This function updates the command table top make project dependent commands
// visible or invisible depending on the set of projects currently loaded
// in the workspace.
void CMainFrame::UpdateProjectCommands(void)
{
	IProjectWorkspace *pPW = g_IdeInterface.GetProjectWorkspace();
	if(pPW == NULL)
		return;

	CADWORD Projects;
	pPW->GetLoadedProjects(&Projects);
	UINT cProj = Projects.cElems;
	
	// Let's build an array of unique GUID gathered from the current
	// list of projects.

	// We'll have at most cProj unique GUIDs
	GUID *pGuid = NULL;
	if(cProj>0)
		pGuid = new GUID[cProj];
	UINT cUniqGuid = 0;
	int i;
	for(i=0; i<cProj; i++)
	{
		GUID guid;
		((IPkgProject *)Projects.pElems[i])->GetClassID(&guid);
		((IPkgProject *)Projects.pElems[i])->Release();
		BOOL bGuidFound = FALSE;
		for(int j=0; j<cUniqGuid; j++)
		{
			if(guid == pGuid[j])
			{
				bGuidFound = TRUE;
				break;
			}
		}
		if(!bGuidFound)
			pGuid[cUniqGuid++] = guid;
	}
	CoTaskMemFree((void *)Projects.pElems);

	// Now we need to walk the list of commands and ask the package owner
	// whether or not the command should be visible given the list of
	// current GUIDs of projects loaded
	for(i = 0; i < theCmdCache.m_cCommands; i++)
	{
		CTE *pCTE = &theCmdCache.m_rgCommands[i];
		if(pCTE->flags & CT_PROJECT_DEPENDENT)
		{
			CPack *pPack = pCTE->GetPack();

			// Shared editor commands cannot be made project dependent!!!
			ASSERT(pPack != NULL && pPack->PacketID() != PACKET_SHARED);
			if(pPack)
			{
				CPackage *pPackage = pPack->GetPackage();
				ASSERT(pPackage != NULL);
				BOOL bVisible;
				bVisible = pPackage->IsVisibleProjectCommand(pCTE->id, pGuid, cUniqGuid);
				if(!bVisible)
					pCTE->flags |= CT_QUERYMENU;
				else
					pCTE->flags &= ~CT_QUERYMENU;
			}
		}

	}
	if(pGuid)
		delete [] pGuid;

	theApp.m_bProjectCommandsDirty=FALSE;
}
    
// If bAllEditors is TRUE, then all editor specific commands will be shown, independent of the current editor. Otherwise,
// only those for the current editor will be shown.
// If bAllProjects is TRUE, all project dependent commands will be shown.
// If bEmptyMenus is TRUE, then any menu which has no items (visible or invisible) will be shown
void CMainFrame::RebuildMenus(BOOL bAllEditors, BOOL bAllProjects, BOOL bEmptyMenus)
{
	// Cancel any dropped command bar
	CASBar::CancelMenu();

	// if project environment has changed, recalc project commands. Defer recalc if we're going to show them all anyway
	if(theApp.m_bProjectCommandsDirty && !bAllProjects)
	{
		UpdateProjectCommands();
	}

	hMenuWindow = NULL;
	if((m_pManager && m_pManager->IsInDocObjectMode()) && (s_hOleMenu == NULL) &&
		CDockWorker::s_pActiveDocker != NULL)
	{
		// Move the handle to a cached location so that we can
		// restore the Ole menu when the doc object gets reactivated
		s_hOleMenu = ::GetMenu(m_hWnd);
	}

	HMENU hMenuOld = m_hMenuDefault;
    m_hMenuDefault = GetCommandMenu(FALSE);

	if(m_hMenuDefault)
	{
		::SendMessage(m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM) m_hMenuDefault, (LPARAM) hMenuWindow);
		::SendMessage(m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
		DrawMenuBar();
	}

	if (hMenuOld != NULL)
		::DestroyMenu(hMenuOld);

	if(theCmdCache.GetMenu(IDM_MENU_BAR) == NULL)
	{
		// creating menus for very first time
		// unless something odd has happened, we've probably got at least one unsaved menu
		CMainGroup mainGroup;
	    theCmdCache.ProcessCommands(&mainGroup, FALSE);

		// search all the command table for any menus
		for (int i = 0; i < theCmdCache.m_rgMenuCommands.GetSize() ; i++)
		{
			ASSERT(theCmdCache.m_rgMenuCommands[i]<theCmdCache.m_cCommands);

			CTE *pCTE = &theCmdCache.m_rgCommands[theCmdCache.m_rgMenuCommands[i]];

			ASSERT(pCTE);
			ASSERT(pCTE->flags & CT_MENU);

			CString strKey;
			wsprintf(strKey.GetBuffer(sizeof(szMenuKey) + COMMAND_ID_BUFFERSIZE), szMenuKey, pCTE->id);
			strKey.ReleaseBuffer();

			HGLOBAL hglobInit = GetRegData(CDockWorker::s_lpszLayoutSection, strKey, NULL);
			BOOL bInit=FALSE;
			CBMenuPopup *pMenu=new CBMenuPopup;
			pMenu->CreateMenu();
			pMenu->SetAutoEnable(TRUE);
    
			if(hglobInit)
			{
				if(pMenu->SetData(hglobInit))
				{
					bInit=TRUE;
				}
			}

			if(!bInit)
			{
				mainGroup.BuildSingleMenu(pMenu, pCTE->id);
			}
			theCmdCache.SetMenu(pCTE->id, pMenu);
		}

		// Now determine which packages these menus knew about
		HGLOBAL hglobOld=GetRegData(CDockWorker::s_lpszLayoutSection, szPackagesKey, NULL);
		if(hglobOld)
		{
			// data was found, so do a merge
			// allow size for the worst case, where none of the current packages are the same as before. This is theoretically
			// impossible (shell should always be there), but other than that it could happen
			WORD wSizeNew=(WORD)GlobalSize(hglobOld)-sizeof(WORD)+theApp.m_packages.GetCount()*sizeof(DWORD);

			HGLOBAL hglobNew = ::GlobalAlloc(GMEM_SHARE, wSizeNew+sizeof(WORD));

			// Save details of which packages were loaded at this time, plus those we already knew about
			int nPackages=theApp.m_packages.GetCount();
			DWORD *prgOldKnownPackages=NULL;
			DWORD *prgNewKnownPackages=NULL;

			WORD * lpWordOld = (WORD FAR*) ::GlobalLock(hglobOld);
			WORD * lpWordNew = (WORD FAR*) ::GlobalLock(hglobNew);

			// get count of stuff from registry
			WORD wSizeOld=(*lpWordOld++);
			int nOldPackages=wSizeOld/sizeof(DWORD);
			// skip 
			WORD * lpWordNewBase=lpWordNew;
			*lpWordNew++=wSizeNew;

			// copy old stuff
			memcpy(lpWordNew, lpWordOld, wSizeOld);
			lpWordNew+=wSizeOld/sizeof(WORD);
			
			// prepare for merge
			prgOldKnownPackages=(DWORD *)lpWordOld;
			prgNewKnownPackages=(DWORD *)lpWordNew;

			// find active package ids
			CPackage* pPackageEnum;
			POSITION pos = theApp.m_packages.GetHeadPosition();

			CWordArray rgUnknownPackages;

			while (pos != NULL)
			{
				pPackageEnum = (CPackage*) theApp.m_packages.GetNext(pos);
				UINT nId=pPackageEnum->PackageID();
				BOOL bFound=FALSE;

				// have we already accounted for this package
				for(int i=0;i<nOldPackages && !bFound; ++i)
				{
					if(prgOldKnownPackages[i]==nId)
					{
						bFound=TRUE;
					}
				}

				if(!bFound)
				{
					rgUnknownPackages.Add((WORD)nId);
				}

				*lpWordNew++=(WORD)nId;
			}

			CTE *pCTE=NULL;

			// merge commands from this package into the menu structure
			for (int i = 0; i < theCmdCache.m_cCommands; i++)
			{
				pCTE = &theCmdCache.m_rgCommands[i];

				if (pCTE->flags & CT_NOMENU)
					continue;

				if ((pCTE->flags & (CT_NOUI)) != 0)
					continue;

				// This command will only be merged if it belongs exclusively to the packet in question or it is shared
				// by several packages, all of which are new
				if(pCTE->GetPack()->PacketID()==PACKET_SHARED)
				{
					// If any of the packages sharing the shared command are new, then the command should be merged
					BOOL bMerge=FALSE;

					// If this package isn't unknown, we give up
					for(int i=0; i<rgUnknownPackages.GetSize() && !bMerge; ++i)
					{
						if(pCTE->FindPackage(theApp.GetPackage(rgUnknownPackages[i])))
						{
							bMerge=TRUE;
						}
					}
							
					// not from an unknown package, so continue
					if(!bMerge)
					{
						continue;
					}
						
				}
				else
				{
					int nIdPackage=pCTE->GetPack()->GetPackage()->PackageID();
					BOOL bMerge=FALSE;

					// does this command belong to an unknown package
					for(int i=0; i<rgUnknownPackages.GetSize() && !bMerge; ++i)
					{
						if(rgUnknownPackages[i]==nIdPackage)
						{
							bMerge=TRUE;
						}
					}
							
					// not from an unknown package, so continue
					if(!bMerge)
					{
						continue;
					}
				}

				// Find all groups associated with this command 
				CCmdGroupArray rgGroups;
				mainGroup.FindGroups(pCTE->group, rgGroups);

				// now iterate over them
				int nGroups=rgGroups.GetSize();
				for(int iGroup=0; iGroup<nGroups; ++iGroup)
				{
					CCmdGroup *pGroup=rgGroups[iGroup];

					// now find out the menu where this belongs
					CCmdMenu *pCmdMenu=pGroup->m_pParent;

					if(pCmdMenu)
					{
						CBMenuPopup *pMenu=theCmdCache.GetMenu(pCmdMenu->m_id);

						if(	pMenu &&
							pMenu->IsDirty()) // if the menu is dirty at this point, it must have just been loaded
						{
							// merge the commnand into the menu

							BOOL bFoundGroupStart=FALSE;
							int nInsert=-1;
							BOOL bAlreadyPresent=FALSE;

							// If there are commands from the same group already on the menu, then put it after the
							// first contiguous group of them. Otherwise, put it at the end
							for(int nItem=0;nItem<pMenu->GetMenuItemCount() && !bAlreadyPresent; ++nItem)
							{
								CBMenuItem *pItem=pMenu->GetMenuItem(nItem);
								if(	pItem &&
									pItem->m_pCTE)
								{
									if(pItem->m_pCTE->id==pCTE->id)
									{
										bAlreadyPresent=TRUE;
									}

									// have we found an item belonging to the same group as the command we want to merge?
									if(pItem->m_pCTE->group==pGroup->m_id)
									{
										bFoundGroupStart=TRUE;
									}
									else
									{
										// if we've seen the start, then we've just passed the end, so we can insert the item
										if(bFoundGroupStart)
										{
											// this causes the loop to terminate too
											nInsert=nItem;
										}
									}
								}
							}

							if(!bAlreadyPresent)
							{
								// insert 
								CBMenuItem *pNewItem=new CBMenuItem;
								pNewItem->CreateDynamic(pMenu, pCTE->id, "", TRUE, NULL);

								// if we didn't find a place, it'll go at the end of the menu, which is probably fine unless
								// it ends up the file exit command, but that's pretty unlikely.
								pMenu->AddMenuItem(nInsert, pNewItem);
							}
						}
					}
				}
			}

			// finish work
			GlobalUnlock(hglobNew);
			GlobalUnlock(hglobOld);

			// lose old
			GlobalFree(hglobOld);

			// resize new to fit
			GlobalReAlloc(hglobNew, (lpWordNew-lpWordNewBase)*sizeof(WORD),0);

			theCmdCache.m_prgKnownPackages=hglobNew;
		}
	}

	POSITION pos=theCmdCache.GetFirstMenuPosition();

	CASBar *pMenuBar=CASBar::GetMenuBar();
	BOOL bBarChanged=FALSE;

	UINT nIDMenu;
	CBMenuPopup *pPopup;
	while(pos!=NULL)
	{
		theCmdCache.GetNextMenu(pos, nIDMenu, pPopup);

		// at start, we know nothing
		pPopup->m_nVisibleItems=-1;
	}

	// now apply invisibility flags to the menu tree.
	bBarChanged|=ApplyFlags(IDM_MENU_BAR, bAllEditors, bAllProjects, bEmptyMenus);

	// finally, apply them to any menus not currently in the tree, in case they get customized into existence
	pos=theCmdCache.GetFirstMenuPosition();

	while(pos!=NULL)
	{
		theCmdCache.GetNextMenu(pos, nIDMenu, pPopup);

		// special value used to block infinite recursion, should never get here
		ASSERT(pPopup->m_nVisibleItems!=-2);

		// at start, we know nothing
		if(pPopup->m_nVisibleItems==-1)
		{
			bBarChanged|=ApplyFlags(nIDMenu, bAllEditors, bAllProjects, bEmptyMenus);
		}
	}
	CASBar::UpdateMenuBar(this);

	if(bBarChanged)
	{
		pMenuBar->RecalcLayout(pMenuBar->GetExpansionConstraint(CRect(0,0,0,0), NULL));
		pMenuBar->Invalidate();
	}

	// Restore the merged menu if we go back to the DocObject frame
	// from a docking window
	if((m_pManager && m_pManager->IsInDocObjectMode()) && CDockWorker::s_pActiveDocker == NULL)
	{
		if(s_hOleMenu != NULL)
		{
			::SendMessage(m_hWndMDIClient, WM_MDISETMENU,
				(WPARAM) s_hOleMenu, (LPARAM) NULL);
			::SendMessage(m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
			DrawMenuBar();
		}
		s_hOleMenu = NULL;
	}

    theApp.m_bMenuDirty = FALSE;
	theApp.m_bMenuDead = FALSE;
}

void CCmdCache::SaveMenus()
{
	BOOL bSavePackages=FALSE;

	// search all the menus in the command table for any menus in the specified group, and add them
	for (int i = 0; i < m_rgMenuCommands.GetSize() ; i++)
	{
		ASSERT(m_rgMenuCommands[i]<theCmdCache.m_cCommands);

		CTE *pCTE = &m_rgCommands[theCmdCache.m_rgMenuCommands[i]];

		ASSERT(pCTE);
		ASSERT(pCTE->flags & CT_MENU);

		CBMenuPopup *pMenu=GetMenu(pCTE->id);

		CString strKey;
		wsprintf(strKey.GetBuffer(sizeof(szMenuKey) + COMMAND_ID_BUFFERSIZE), szMenuKey, pCTE->id);
		strKey.ReleaseBuffer();
		
		if(pMenu->IsDirty())
		{
			ASSERT(CDockWorker::s_lpszLayoutSection != NULL);

			HGLOBAL hglobInit = (HGLOBAL) pMenu->GetData();

			if (hglobInit != NULL)
			{
				WriteRegData(CDockWorker::s_lpszLayoutSection, strKey, hglobInit);

				bSavePackages=TRUE;
			}
			else
			{
				DeleteRegData(CDockWorker::s_lpszLayoutSection, strKey);
			}

			::GlobalFree(hglobInit);
		}
		else
		{
			DeleteRegData(CDockWorker::s_lpszLayoutSection, strKey);
		}
	}

	if(bSavePackages)
	{
		HGLOBAL hglob = NULL;

		if(m_prgKnownPackages)
		{
			hglob=m_prgKnownPackages;
		}
		else
		{
			// Save details of which packages were loaded at this time, plus those we already knew about
			int nPackages=theApp.m_packages.GetCount();
			DWORD *prgSaveKnownPackages=NULL;

			WORD wSize=sizeof(DWORD)*nPackages;

			// allocate memory
			hglob = ::GlobalAlloc(GMEM_SHARE, wSize + sizeof(WORD));
			WORD FAR* lpWord = (WORD FAR*) ::GlobalLock(hglob);
			
			// init size info
			*lpWord++ = wSize;

			prgSaveKnownPackages=(DWORD *)lpWord;

			// save package ids
			CPackage* pPackageEnum;
			POSITION pos = theApp.m_packages.GetHeadPosition();

			while (pos != NULL)
			{
				pPackageEnum = (CPackage*) theApp.m_packages.GetNext(pos);
				*prgSaveKnownPackages++=pPackageEnum->PackageID();
			}

			GlobalUnlock(hglob);
		}

		// store in registry
		WriteRegData(CDockWorker::s_lpszLayoutSection, szPackagesKey, hglob);

		// lose the data
		GlobalFree(hglob);
		if(m_prgKnownPackages)
		{
			m_prgKnownPackages=NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//  Menu text helper

void MenuFileText(CString& str)
{
	// Double up ampersands in file names.
	if (str.Find(_T('&')) == -1)
		return;
	
	CString strBuffer;
	LPTSTR lpBuffer = strBuffer.GetBuffer(str.GetLength() * 2);
	LPCTSTR lpsz = str;

	while (*lpsz != _T('\0'))
	{
		_tccpy(lpBuffer, lpsz);

		if (*lpsz == _T('&'))
			*(++lpBuffer) = _T('&');

		lpBuffer = _tcsinc(lpBuffer);
		lpsz = _tcsinc(lpsz);
	}
	*lpBuffer = _T('\0');

	strBuffer.ReleaseBuffer();
	str = strBuffer;
}


// This code has been completely revised to deal with the new situation where the menu bar
// is built from groups and all existing concrete menu bars could have been customised (even 
// the one returned from theCmdCache.GetMenu(IDM_MENU_BAR), though that's unlikely to happen
// often.

//REVIEW: Support for a dirty command cache
BOOL CAppToolGroups::ScanCommandCache()
{
	if (m_wSyncID == theCmdCache.m_wSyncID)
		return FALSE;

	if (m_rgGroups != NULL)
		delete [] m_rgGroups;

	// get the menu bar popdesc from the appropriate package
	CTE *pCTE=theCmdCache.GetCommandEntry(IDM_MENU_BAR);
	ASSERT(pCTE);
	ASSERT(pCTE->GetPack());
	ASSERT(pCTE->GetPack()->GetPackage());
	POPDESC *pMenuBar=pCTE->GetPack()->GetPackage()->GetMenuDescriptor(IDM_MENU_BAR);

	// calculate the group structure
	CMainGroup mainGroup;
	theCmdCache.ProcessCommands(&mainGroup, FALSE, FALSE, FALSE);
	// scan the command cache for commands in the relevant groups
	// First time around we just need to know how many commands are in one of these groups

	// Count the groups for allocation.
	int iGroup;
	m_nGroups = 0;

	for(iGroup=0; pMenuBar->rgmtm[iGroup].id !=POP_NIL ; ++iGroup)
	{
		CCmdGroupArray rgGroups;

		// find first group and then stop
		if(mainGroup.FindGroups((WORD)pMenuBar->rgmtm[iGroup].id,rgGroups, TRUE))
		{
			CCmdGroup *pGroup=rgGroups[0];

			POSITION pos=pGroup->m_children.GetHeadPosition();
			while (pos != NULL)
			{
				CCmd *pItem = (CCmd*) pGroup->m_children.GetNext(pos);
				if(	pItem &&
					pItem->m_flags & CT_MENU)
				{
					m_nGroups++;
				}
			}
		}
	}

	m_rgGroups = new CToolGroup[m_nGroups];

	// Fill the groups.
	int iDestGroup = 0;
	for(iGroup=0; pMenuBar->rgmtm[iGroup].id !=POP_NIL ; ++iGroup)
	{
		CCmdGroupArray rgGroups;

		// find first group and then stop
		if(mainGroup.FindGroups((WORD)pMenuBar->rgmtm[iGroup].id,rgGroups, TRUE))
		{
			CCmdGroup *pGroup=rgGroups[0];

			POSITION pos=pGroup->m_children.GetHeadPosition();
			while (pos != NULL)
			{
				CCmd *pItem = (CCmd*) pGroup->m_children.GetNext(pos);
				if(	pItem &&
					pItem->m_flags & CT_MENU)
				{
					CTE *pMenuCTE=theCmdCache.GetCommandEntry(pItem->m_id);
					ASSERT(pMenuCTE);
					ASSERT(pMenuCTE->GetPack());
					ASSERT(pMenuCTE->GetPack()->GetPackage());
					POPDESC *pMenuPop=pMenuCTE->GetPack()->GetPackage()->GetMenuDescriptor(pMenuCTE->id);

					m_rgGroups[iDestGroup].Fill(pMenuPop, pItem->m_id);
					m_nCmds+=m_rgGroups[iDestGroup].m_nCmds;
					iDestGroup++;
				}
			}
		}
	}

	m_wSyncID = theCmdCache.m_wSyncID;
	return TRUE;
}
