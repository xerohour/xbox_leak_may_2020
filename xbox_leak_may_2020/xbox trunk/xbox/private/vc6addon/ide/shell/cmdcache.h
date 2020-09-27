/////////////////////////////////////////////////////////////////////////////
//	CMDCACHE.H

#ifndef __CMDCACHE_H__
#define __CMDCACHE_H__

class CPack;
class CASBar;
class CDockManager;
class CMainGroup;
class CBMenuPopup;
class CCmdGroup;

struct POPDESC;

/////////////////////////////////////////////////////////////////////////////
//	class CToolGroup

class CToolGroup
{
// Construction
public:
	CToolGroup();

// Operations
public:
	void AddGroup(int nGroup);
	void Fill(POPDESC* ppop,UINT nId=0);

	LPCTSTR GetCommandName(UINT nCmdID);

	CASBar* CreateCASBar(CWnd* pParent, CDockManager* pManager);
	// when bForButtons is true, this will omit any command
	// which cannot be bound to a toolbar button
	void FillCommandList(CListBox *pList, BOOL bForButtons=FALSE);

// Attributes
public:
	CPtrArray m_aCmds;
	CString m_strGroup;
	// ID of the menu bar menu which caused this toolgroup to exist
	UINT m_nId;
	// Number of commands in the group, currently unused.
	int m_nCmds;
};

/////////////////////////////////////////////////////////////////////////////
//	class CAppToolGroups

class CAppToolGroups
{
public:
	static CAppToolGroups* s_pAppToolGroups;

	static CAppToolGroups* GetAppToolGroups(BOOL bShowWaitCursor = FALSE);
	static void ReleaseAppToolGroups();

// Construction
public:
	CAppToolGroups();
	~CAppToolGroups();

// Operations
public:
	//REVIEW: Support for a dirty command cache
	BOOL ScanCommandCache();

	void GetCommandName(UINT nCmdID, CString& str);

	void FillGroupList(CComboBox *pList);
	void FillCommandList(UINT nGroup, CListBox *pList);
	void CreateCustomizeToolbars(CWnd* pParent, CDockManager* pManager, CObArray* pToolbars, CStringArray *pTitles, CWordArray *aIds);
	CToolGroup* GroupFromCommandName(LPCTSTR szCmdName);

// Attributes
public:
	//REVIEW: Support for a dirty command cache
	WORD m_wSyncID;

	UINT m_nUsage;
	int m_nGroups;

	// total number of commands in all groups
	int m_nCmds; 
	CToolGroup* m_rgGroups;
};

/////////////////////////////////////////////////////////////////////////////
//	class CCmdCache

struct CTE
{
	WORD id;
	WORD group;
	WORD flags;
	WORD glyph;
	LPCTSTR szCommand;

	BOOL IsActive(void);

	// Access the pack stored in the CTE. For internal use only
	CPack *GetPack(void) { return pPack; };
	void SetPack(CPack *thePack) { pPack=thePack; };

	// Gets the pack, adjusted for which pack is active, and resolving sharing issues
	CPack *GetActivePack(void);

	// returns true if the specified pack is in the list
	BOOL FindPack(CPack *);

	// returns true if the specified pack is in the list
	BOOL FindPackage(CPackage *);

	// Gets any real pack, resolving sharing issues
	CPack *GetFirstPack(void);

private:
	CPack* pPack;
};

inline LPCTSTR ShellGetNextString(LPCTSTR szCommand, UINT iString)
{	while (iString--) szCommand += lstrlen(szCommand) + 1;
	return szCommand; }

class CCmdCache
{
public:
	CCmdCache();
	~CCmdCache();

	BOOL AddCommandResource(CPack* pPack, LPCTSTR lpCmdTableID, BOOL bSearch=FALSE);
	BOOL AddCommand(WORD id, WORD group, WORD flags, WORD glyph,
					CPack* pPack, LPCTSTR szCommand, int nCmdLength);
	// Packages should use this to remove commands that are being permanently removed from the system.
	// UI elements related to the command will be irretrievably removed from the UI. If pPack is not
	// the owner of the command, then the command will not be removed. This stops most commands being removed
	// erroneously by confused packages. 
	BOOL RemoveCommand(WORD id, CPack* pPack);

	CTE* GetCommandEntry(UINT nID);
	void ReplaceCommandString(UINT nID, UINT iString, LPCTSTR szNewSubString);
	BOOL GetCommandString(UINT nID, UINT iString, LPCTSTR* pszString, WORD *pflags = NULL, CTE* pCTE = NULL);
	BOOL GetCommandID(LPCTSTR szCommand, UINT* pnID);

	// retrieves the CBMenu structure for a given menu command id
	CBMenuPopup *GetMenu(UINT nIDMenu);
	// sets or replaces the existing version of a menu (not for general use
	void SetMenu(UINT nIDMenu, CBMenuPopup *pMenu);
	// iteration through menus
	POSITION GetFirstMenuPosition() { return m_pMenus->GetStartPosition(); };
	void GetNextMenu(POSITION &rNextPosition, UINT &nIDMenu, CBMenuPopup *&rpMenu) { m_pMenus->GetNextAssoc(rNextPosition, nIDMenu, rpMenu); };

	HMENU GetOleMenu();

	void ProcessCommands(CMainGroup* pMainGroup, BOOL bRestrict=TRUE, BOOL bOLE=FALSE, BOOL bMenuOnly=TRUE);
	void AddPopToGroup(POPDESC* ppop, CCmdGroup* pGroup);
	void FillMenuList(CListBox *pList);
	void FillNewMenuList(CListBox *pList);
	void FillAllCommandsList(CListBox *pList, BOOL bKeyboard=FALSE);
	void FillDeletedCommandsList(CListBox *pList);
	int GetNextFreeMenu(void);
	void FlushMenuSizes(void);

	// These two are for shared commands only.
	// Gets the pack, adjusted for which pack is active, and resolving sharing issues
	CPack *GetActivePack(UINT id);
	// Gets any real pack, resolving sharing issues
	CPack *GetFirstPack(UINT id);
	// Finds if a pack is in the list
	BOOL FindPack(UINT id, CPack *pFindPack);
	// Finds if a pack belonging to the package is in the list
	BOOL FindPackage(UINT id, CPackage *pFindPackage);

	// call at shutdown to save all of the menus
	void CCmdCache::SaveMenus();

#ifdef _DEBUG
	void PrintCommandTable();
#endif

	//REVIEW: Support for a dirty command cache
	WORD m_wSyncID;

protected:
	const BYTE* ParseCommand(const BYTE* pb, int nLen,
		CPack* pPack, CTE& cte, BOOL *bSuppress);

protected:
	enum {
		SHELL_COMMAND_BASE = 0x8000,

		SHELL_COMMAND_BLOCKSIZE = 0x1000,
	};

	int m_cBlocks;
	int m_cCommands;
	CTE* m_rgCommands;
	CWordArray m_rgMenuCommands;	// this is an array of indexes into m_rgCommands, and contains all menu commands.
	HGLOBAL m_prgKnownPackages;	// the array of known packages, in registry form, preceded by size word. Each package
								// id is in a DWORD

	HANDLE m_hStringHeap;

	CMap<UINT, UINT, CBMenuPopup *, CBMenuPopup *&> *m_pMenus;
	
	// To support shared commands, we need to map from a command id to a CPtrArray of CPacks
	CMap<UINT, UINT, CTypedPtrArray<CObArray, CPack *> *, CTypedPtrArray<CObArray, CPack*> *&> *m_pSharedCommands;

	friend class CTheApp;
	friend class CMainFrame;
	friend class CToolGroup;

};

extern CCmdCache theCmdCache;

#endif	// __CMDCACHE_H__
