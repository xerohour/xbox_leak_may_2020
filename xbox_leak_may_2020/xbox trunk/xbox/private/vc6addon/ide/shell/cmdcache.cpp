/////////////////////////////////////////////////////////////////////////////
//	CMDCACHE.CPP

/* We now allow commands to be  shared between two packets. Only packet commands can be shared. Each packet has a command 
table entry for the commandand as the packet. When the packet command table is loaded, we check each command to see if it
is a duplicate of an existing command. If it is, we 'merge' the two command table entries by setting them to PACKET_SHARED
and adding them to a map of shared commands.

In the debug build, we ensure that the commands have identical flags, bitmaps, etc, in both packages; in release, we ignore
such problems.

Advantages of this solution:
Shell does not have to grow for commands to be shared
All packet commands can be shared, even after ship
No coupling between those sharing commands
Only one command table entry per command, still
No major design changes late in the dev cycle

Disadvantages of this solution:
Searching for duplicates at load time may be a speed issue. If so, we will need to add a CMap of commands, which isn't a bad
idea anyway.
Command tables in different packages must be kept in sync

Other solutions considered:
Client packages 'subscribe' to commands at init time. This would be faster, but creates a coupling between packages or requires 
us to move commands into the shell.
We could simply allow multiple command table entries to exist for a single command ID, and switch the order of packet command 
tables through use of sublists. This is actually a pretty strong idea, but would destabilise things too much for this stage,
in my opinion. Plus, we'd have to closely examine the lifetime of CTEs at all stages, and we'd be prone to bad packages with
aberrant shared command flags.

martynl, 15 Aug 96
*/

#include "stdafx.h"
#include "cmdcache.h"
#include "shlmenu.h"
#include "bardockx.h"
#include "btnctl.h"
#include "toolexpt.h"
#include "prjapi.h"
#include "resource.h"
#include "barglob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern HMENU GetCommandMenu(BOOL bOleIPMenu);

CString g_strCmdDynamic;  // global holder for dynamic command string (must not be held, but immediately copied)

// Implementation helper class - CSharedPack is the CPack of shared commands

class CSharedPacket : public CPacket
{
public:
	CSharedPacket() : CPacket(NULL, PACKET_SHARED) {};
};

static CSharedPacket s_sharedPack;

/////////////////////////////////////////////////////////////////////////////
//	class CCmdCache

CCmdCache theCmdCache;	// Single command cache global.

CCmdCache::CCmdCache()
{
	m_cBlocks = 0;
	m_cCommands = 0;
	m_rgCommands = NULL;

	m_hStringHeap = HeapCreate(0, 4096, 0);

	m_pMenus=NULL;
	m_pSharedCommands=NULL;
	m_prgKnownPackages=NULL;

	//REVIEW: Support for a dirty command cache
	m_wSyncID = 1;
}

CCmdCache::~CCmdCache()
{
	if (m_rgCommands != NULL)
		free(m_rgCommands);

	if(m_prgKnownPackages!=NULL)
	{
		GlobalFree(m_prgKnownPackages);
		m_prgKnownPackages=NULL;
	}

	VERIFY(HeapDestroy(m_hStringHeap));

	if(m_pMenus!=NULL)
	{
		// delete all of the menu structures
		CBMenuPopup *pMenu=NULL;
		UINT nIDMenu;
		POSITION pos=m_pMenus->GetStartPosition();

		// first delete the objects
		while(pos)
		{
			m_pMenus->GetNextAssoc(pos, nIDMenu,pMenu);
			if(pMenu)
			{
				pMenu->DestroyWindow();
				delete pMenu;
			}
		}

		// then unmap them
		m_pMenus->RemoveAll();

		// can now lose the map itself
		delete m_pMenus;
		m_pMenus=NULL;
	}

	if(m_pSharedCommands!=NULL)
	{
		// delete all of the CPtrArrays
		CTypedPtrArray<CObArray, CPack*> *pPackets;
		UINT nIDCommand;
		POSITION pos=m_pSharedCommands->GetStartPosition();

		// first delete the objects
		while(pos)
		{
			m_pSharedCommands->GetNextAssoc(pos, nIDCommand,pPackets);
			if(pPackets)
			{
				delete pPackets;
			}
		}

		// then unmap them
		m_pSharedCommands->RemoveAll();

		// can now lose the map itself
		delete m_pSharedCommands;
		m_pSharedCommands=NULL;
	}
}

CBMenuPopup *CCmdCache::GetMenu(UINT nIDMenu)
{
	if(m_pMenus!=NULL)
	{
		CBMenuPopup *pMenu=NULL;
		
		if(m_pMenus->Lookup(nIDMenu, pMenu))
		{
			return pMenu;
		}
	}
	return NULL;
}

	// sets or replaces the existing version of a menu (not for general use
void CCmdCache::SetMenu(UINT nIDMenu, CBMenuPopup *pMenu)
{
	if(m_pMenus==NULL)
	{
		// the magic number 5 is the grow by size for the table; low because this
		// will do all of its growing at one time.
		m_pMenus=new CMap<UINT, UINT, CBMenuPopup *, CBMenuPopup *&>(5);
		// 70 should provide enough room for growth - the current product has 25 menus,
		// and the number should be 20% higher than that. We should probably increase this
		// as we get more package partners
		m_pMenus->InitHashTable(70);
	}
	else
	{
		// consider deleting this element
		CBMenuPopup *pOldMenu=GetMenu(nIDMenu);
		if(pOldMenu!=NULL)
		{
			pOldMenu->DestroyWindow();
			delete pOldMenu;
			m_pMenus->RemoveKey(nIDMenu);
		}
	}

	// now add the new one
	m_pMenus->SetAt(nIDMenu, pMenu);
}

static const BYTE* ParseWord(const BYTE* pb, int nLen, WORD& w)
{
	if(nLen < sizeof(WORD))
		return NULL;
	w = *(WORD*)pb;
	return pb + sizeof(WORD);
}

const BYTE* CCmdCache::ParseCommand(const BYTE* pb, int nLen,
	CPack* pPack, CTE& cte,
	BOOL *pbSuppress				// TRUE if the command was suppressed, and should not be included in the command table
	)
{
	ASSERT(pbSuppress!=NULL);
	*pbSuppress=FALSE;
	BOOL bShared=FALSE;

	const BYTE* pbEnd = pb + nLen;
	WORD w;

	if( (pb = ParseWord(pb, pbEnd - pb, w)) == NULL )		// ID
		return NULL;
	cte.id = w;
	if( (pb = ParseWord(pb, pbEnd - pb, w)) == NULL )		// Group ID
		return NULL;
	cte.group = w;
	if( (pb = ParseWord(pb, pbEnd - pb, w)) == NULL )		// Flags
		return NULL;
	cte.flags = w;
	if( (pb = ParseWord(pb, pbEnd - pb, w)) == NULL )		// Glyph
		return NULL;
	cte.glyph = (int)(short)w;

	// At this point, we can discover whether the command is a duplicate of one provided by another
	// package
	CTE *pOtherCTE=GetCommandEntry(cte.id);
	if(pOtherCTE)
	{
		// This command has been loaded before
		if(	pOtherCTE->GetPack()!=NULL &&
			pPack!=NULL)
		{
			UINT nOldPacketID=pOtherCTE->GetPack()->PacketID();
			UINT nNewPacketID=pPack->PacketID();

			if(	nOldPacketID!=PACKET_NIL &&
				nNewPacketID!=PACKET_NIL)
			{
				// At this point, we are going to share the command. If this is a debug build, we're also going to
				// make sure that the command has the same properties as its co-sharer.

				// If either of these fail, it means that one of the sharers has a bad command table entry.
				ASSERT(cte.group==pOtherCTE->group);
				ASSERT(cte.flags==pOtherCTE->flags);
				// The two glyph indices need not be the same, but they must either both be -1 or neither
				ASSERT(	((cte.glyph==-1) ^ (pOtherCTE->glyph==-1))==0);
				bShared=TRUE;

				// Some flags are not permitted on shared commands, because these flags need to ask a package for information
				// even when there is no, or another, active packet
				if(cte.flags & 	(CT_HWND | CT_HWNDSIZE | CT_HWNDENABLE | CT_MENU | CT_DYNAMIC_CMD_STRING))
				{
					// can's share this, guv.
					ASSERT(FALSE);
				}

				// Two possibilities here. Either the command is already shared (PACKET_SHARED), or it need to be
				// made into a shared one.

				if(nOldPacketID!=PACKET_SHARED)
				{
					// allocate the command map, if we've not already done that
					if(m_pSharedCommands==NULL)
					{
						// the magic number 5 is the grow by size for the table; low because this
						// will do all of its growing at one time.
						m_pSharedCommands=new CMap<UINT, UINT, CTypedPtrArray<CObArray, CPack*> *, CTypedPtrArray<CObArray, CPack*> *&>(5);
						// 100 should provide enough room for growth - the current product has an unknown number of 
						// shared commands and the number should be 20% higher than that. We should probably modify 
						// this near ship time.
						m_pSharedCommands->InitHashTable(50);
					}

					// ensure that we don't already have an association for this command
					CTypedPtrArray<CObArray, CPack*> *pPackets=NULL;
					ASSERT(!m_pSharedCommands->Lookup(cte.id, pPackets));

					// create the array of packets
					pPackets=new CTypedPtrArray<CObArray, CPack*>;

					// initial size is 2 so that we can accomodate the existing and the new packet
					pPackets->SetSize(2);

					// fill array
					pPackets->SetAt(0, pOtherCTE->GetPack());
					pPackets->SetAt(1, pPack);

					// map array
					m_pSharedCommands->SetAt(cte.id, pPackets);

					pOtherCTE->SetPack(&s_sharedPack);
				}
				else
				{
					// ensure that we do already have an association for this command
					CTypedPtrArray<CObArray, CPack*> *pPackets=NULL;
					VERIFY(m_pSharedCommands->Lookup(cte.id, pPackets));

					// add this packet to the set
					pPackets->Add(pPack);
				}
				
				// suppress this one because it got merged with the other one
				*pbSuppress=TRUE;

				// Should always be shared by now.
				ASSERT(pOtherCTE->GetPack()->PacketID()==PACKET_SHARED);
			}
			else
			{
				// someone tried to share a global command. You can't do that, and there's no need anyway.
				ASSERT(FALSE);
				*pbSuppress=TRUE;
			}
		}
		else
		{
			// Someone has attempted to share a command where one of the two has no pPack. This is not allowed
			ASSERT(FALSE);
			*pbSuppress=TRUE;
		}
	}

	// always parse rest of command, even if we are suppressing
	if( (pb = ParseWord(pb, pbEnd - pb, w)) == NULL )		// String length
		return NULL;
	if (w > pbEnd - pb)
		return NULL;

	// only allocate space for strings if we are not suppressing
	if(!*pbSuppress)
	{
		cte.SetPack(pPack);
		LPTSTR szCommand = (LPTSTR) HeapAlloc(m_hStringHeap, 0, w);
		memcpy(szCommand, pb, w);
		cte.szCommand = szCommand;
	}

	if(!*pbSuppress)
	{
		// if the commands were shared, then in debug we want to ensure the strings were the same
		if(bShared)
		{
			ASSERT(memcmp(pOtherCTE->szCommand, pb, w)==0);

			TRACE2("CCmdCache::ParseCommand: Sharing command %s (%d)\n\r", cte.szCommand, cte.id);
		}
		else
		{
#ifdef _DEBUG

// Because the database can't generate good command table entries, we're removing some of the assertions
// until after 5.0 ships
#if 0
#define BCASSERT(x) ASSERT(x)
#else
#define BCASSERT(x)
#endif

			// This is the first time of loading for a command. So we're going to do some validation on what 
			// command strings it should have
			ASSERT(STRING_COMMAND+1==STRING_MENUTEXT);
			ASSERT(STRING_MENUTEXT+1==STRING_PROMPT);
			ASSERT(STRING_PROMPT+1==STRING_TIP);

			LPCTSTR lpszString=cte.szCommand;
			BOOL bHasCommand=strlen(lpszString)>0;
			lpszString=ShellGetNextString(lpszString, STRING_MENUTEXT-STRING_COMMAND);
			BOOL bHasMenuText=strlen(lpszString)>0;
			lpszString=ShellGetNextString(lpszString, STRING_PROMPT-STRING_MENUTEXT);
			BOOL bHasPrompt=strlen(lpszString)>0;
			lpszString=ShellGetNextString(lpszString, STRING_TIP-STRING_PROMPT);
			BOOL bHasTip=strlen(lpszString)>0;

			// No other command can have the same name
			if(bHasCommand)
			{
				UINT nID;
				BOOL bAlreadyExists=GetCommandID(cte.szCommand, &nID);
				if(bAlreadyExists)
				{
					TRACE2("CCmdCache::ParseCommand: Duplicate command name in command table entry %s (%x)\n\r", cte.szCommand, cte.id);
				}
				ASSERT(!bAlreadyExists);
			}

			// validate unwanted flag combinations
			if(cte.flags & CT_NOKEY)
			{
				if((cte.flags & (CT_NOMENU | CT_QUERYMENU | CT_DYNAMIC))==0)
				{
					TRACE2("CCmdCache::ParseCommand: Non DYNAMIC Menu command with NOKEY flag is nonsensical %s (%x)\n\r", cte.szCommand, cte.id);
				}
				ASSERT((cte.flags & (CT_NOMENU | CT_QUERYMENU | CT_DYNAMIC))!=0);
			}
			
			if(cte.flags & CT_MENU)
			{
				if(!bHasCommand || !bHasMenuText || !bHasPrompt || bHasTip)
				{
					TRACE2("CCmdCache::ParseCommand: Bad menu command table entry %s (%x)\n\r", cte.szCommand, cte.id);
				}
				// menus are a special case. They require a command, menu text, and prompt, and should not have a tip (waste of space)
				ASSERT(bHasCommand);
				ASSERT(bHasMenuText);
				ASSERT(bHasPrompt);
				BCASSERT(!bHasTip);
			}
			else if(cte.flags & CT_HWND)
			{
				if(!bHasPrompt || !bHasTip)
				{
					TRACE2("CCmdCache::ParseCommand: Bad window command table entry %s (%x)\n\r", cte.szCommand, cte.id);
				}
				// hwnds are a special case. All we know is that they have to have a tip and prompt
				ASSERT(bHasPrompt);
				ASSERT(bHasTip);
			}
			else
			{
				if(cte.flags & CT_NOKEY)
				{
					if(bHasTip)
					{
						TRACE2("CCmdCache::ParseCommand: Bad NOKEY command table entry %s (%x)\n\r", cte.szCommand, cte.id);
					}
					// NOKEY, NOBUTTON commands shouldn't have a tip (waste of space)
					BCASSERT(!bHasTip);

					if(bHasCommand)
					{
						TRACE2("CCmdCache::ParseCommand: Questionable NOKEY command table entry (probably shouldn't have a command name) %s (%x)\n\r", cte.szCommand, cte.id);
					}

					// can have other two. This is probably a context menu command
				}
				else
				{
					if(cte.flags & CT_NOBUTTON)
					{
						if(!bHasCommand || !bHasMenuText || !bHasPrompt)
						{
							TRACE2("CCmdCache::ParseCommand: Bad NOBUTTON command table entry %s (%x)\n\r", cte.szCommand, cte.id);
						}
						
						// must have everything; can have tip to allow substitution
						ASSERT(bHasCommand);
						ASSERT(bHasMenuText);
						ASSERT(bHasPrompt);
					}
					else
					{
						if(!bHasTip || !bHasCommand || !bHasMenuText || !bHasPrompt)
						{
							TRACE2("CCmdCache::ParseCommand: Bad normal command table entry %s (%x)\n\r", cte.szCommand, cte.id);
						}
						
						// must have everything, except the tip, which is unneeded
						ASSERT(bHasTip);
						ASSERT(bHasCommand);
						ASSERT(bHasMenuText);
						ASSERT(bHasPrompt);
					}
				}
			}
#endif
		}
	}

	return (pb + w);
}

BOOL CCmdCache::AddCommandResource(CPack* pPack, LPCTSTR lpCmdTableID, BOOL bSearch /*=FALSE*/)
{
	HINSTANCE hInstance;
	if(!bSearch)
	{
		hInstance = pPack->HInstance();
	}
	else
	{
		hInstance= AfxFindResourceHandle(lpCmdTableID, _T("COMMAND_TABLE"));
		ASSERT(hInstance!=NULL);
	}

	HRSRC hRsrc = FindResource(hInstance, lpCmdTableID, _T("COMMAND_TABLE"));
	if (hRsrc == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	HGLOBAL hGlobal = LoadResource(hInstance, hRsrc);
	ASSERT(hGlobal != NULL);
	const BYTE* pb = (const BYTE*)LockResource(hGlobal);
	ASSERT(pb != NULL);
	int nLen = (int)SizeofResource(hInstance, hRsrc);
	ASSERT(nLen > 0);

	const BYTE* pbEnd = pb + nLen;

	// verify that this is a known data format
	WORD wVersion;
	if( (pb = ParseWord(pb, pbEnd - pb, wVersion)) == NULL )
		return FALSE;
	ASSERT(wVersion == 0x1);		// this code only reads data format #1

	int nDataMax = m_cBlocks * SHELL_COMMAND_BLOCKSIZE;
	int nDataNext = (m_cCommands + 1) * sizeof(CTE);
	int cCommandsStart = m_cCommands;

	do
	{
		if (nDataNext > nDataMax)
		{
			m_cBlocks++;
			nDataMax += SHELL_COMMAND_BLOCKSIZE;
			if (m_rgCommands == NULL)
				m_rgCommands = (CTE*) malloc(nDataMax);
			else
				m_rgCommands = (CTE*) realloc(m_rgCommands, nDataMax);
		}

		BOOL bSuppress=FALSE;
			
		pb = ParseCommand(pb, pbEnd - pb, pPack, m_rgCommands[m_cCommands], &bSuppress);
		if (pb != NULL && !bSuppress)
		{
			if(m_rgCommands[m_cCommands].flags & CT_MENU)
			{
				// cache the indexes of all menu commands into a separate array, to enable
				// RebuildMenus to perform acceptably fast
				m_rgMenuCommands.SetAtGrow(m_rgMenuCommands.GetSize(), (WORD)m_cCommands);
			}
			m_cCommands++;
			nDataNext += sizeof(CTE);
		}
	}
	while (pb != NULL);

	return TRUE;
}

BOOL CCmdCache::AddCommand(WORD id, WORD group, WORD flags, WORD glyph,
							CPack* pPack, LPCTSTR szCommand, int nCmdLength)
{
	// can't have two commands with the same id.
	ASSERT(GetCommandEntry(id)==NULL);

	if (m_rgCommands == NULL)
		return FALSE;		// Currently this only works if commands already exist

	// Increase array size if necessary

	int nSize = m_cBlocks * SHELL_COMMAND_BLOCKSIZE;
	m_cCommands++;
	int nRequiredSize = m_cCommands * sizeof(CTE);

	if (nRequiredSize > nSize)
	{
		m_cBlocks++;
		m_rgCommands = (CTE*) realloc(m_rgCommands, nSize + SHELL_COMMAND_BLOCKSIZE);
	}

	// This is the CTE we'll fill in
	CTE* pCte = &(m_rgCommands[m_cCommands-1]);

	pCte->id = id;
	pCte->group = group;
	pCte->flags = flags;
	pCte->glyph = glyph;
	pCte->SetPack(pPack);

	LPTSTR szCopiedCommand = (LPTSTR) HeapAlloc(m_hStringHeap, 0, nCmdLength);
	memcpy(szCopiedCommand, szCommand, nCmdLength);
	pCte->szCommand = szCopiedCommand;

	m_wSyncID++;

	return TRUE;
}

// Packages should use this to remove commands that are being permanently removed from the system.
// UI elements related to the command will be irretrievably removed from the UI. If pPack is not
// the owner of the command, then the command will not be removed. This stops most commands being removed
// erroneously by confused packages. 
BOOL CCmdCache::RemoveCommand(WORD id, CPack* pPack)
{
	// validate whether this pack is allowed to delete this
	CTE *pCTE=GetCommandEntry(id);
	if(pCTE==NULL)
	{
		return FALSE;
	}
	if(pCTE->GetPack()!=pPack)
	{
		return FALSE;
	}

	// Remove the command from all bars
	CASBar::RemoveAllCommandReferences(id);

	// Remove the command from all menus
	if(Menu::IsInCmdBarMode())
	{
		CBMenuPopup *pMenu=NULL;
		UINT nIDMenu;
		POSITION pos=m_pMenus->GetStartPosition();

		// first delete the objects
		while(pos)
		{
			m_pMenus->GetNextAssoc(pos, nIDMenu,pMenu);
			if(pMenu)
			{
				pMenu->RemoveCommandReferences(id);
			}
		}

	}
	else
	{
		CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();
		pFrame->RebuildMenus();
	}

	// work out how much to shift down
	int nTableIndexAfter=(pCTE+1)-m_rgCommands;
	int nEntriesToMove=m_cCommands-nTableIndexAfter;
	size_t nBytesToMove=sizeof(CTE)*nEntriesToMove;

	// use memmove to copy down rest of command table
	memmove(pCTE, pCTE+1, nBytesToMove);

	--m_cCommands;

	m_wSyncID++;

	return TRUE;
}

CTE* CCmdCache::GetCommandEntry(UINT nID)
{
	for (int i = 0; i < m_cCommands; i++)
	{
		if (m_rgCommands[i].id == (WORD) nID)
			return &m_rgCommands[i];
	}

	return NULL;
}

void CCmdCache::ReplaceCommandString(UINT nID, UINT iString, LPCTSTR szNewString)
{
	// Doesn't support changing the name of the command itself
	ASSERT(iString != STRING_COMMAND);
	ASSERT(iString <= STRING_MAX_INDEX);

	CTE* pCTE = GetCommandEntry(nID);
	if (pCTE == NULL)
	{
		ASSERT(FALSE);		// command not found
		return;
	}

	// How big is the new command string?
	LPCTSTR szSubStringToReplace = ShellGetNextString(pCTE->szCommand, iString);
	LPCTSTR szNextSubString = ShellGetNextString(szSubStringToReplace, 1);
	LPCTSTR szEndString = ShellGetNextString(szNextSubString, STRING_MAX_INDEX-iString);
	int nPreLength = szSubStringToReplace - pCTE->szCommand;	// stuff before replaced substring
	int nNewSubLength = _tcslen(szNewString) + 1;				// new substring
	int nPostLength = szEndString - szNextSubString;			// stuff after replaced substring
	int nLength = nPreLength + nNewSubLength + nPostLength;

	// Fill in the new command string
	LPTSTR szNewCmdString = (LPTSTR) HeapAlloc(m_hStringHeap, 0, nLength);
	//		Copy stuff before new sub string
	memcpy(szNewCmdString, pCTE->szCommand, nPreLength);
	//		Copy new substring
	memcpy(szNewCmdString + nPreLength, szNewString, nNewSubLength);
	//		Copy stuff after new substring
	memcpy(szNewCmdString + nPreLength + nNewSubLength, szNextSubString, nPostLength);

	// Out w/ the old, in w/ the new	
	HeapFree(m_hStringHeap, 0, (void*) pCTE->szCommand);
	pCTE->szCommand = szNewCmdString;
	m_wSyncID++;
}

// IMPORTANT NOTE: with the advent of dynamic command strings, we can no longer
// rely on the cmd table to hold the string data. A dynamic command string will
// return a bstr, which we convert to a CString in the SINGLE global string g_strCmdDynamic.

// What this means is that the caller of this function cannot simply hold onto the string
// this function returns, but must copy it immediately. Since virtually all callers did
// this already, this is no additional burden. If we decide we need to be able to hold
// strings we can revisit this, and pass back a bstr and a flag, but currently (7/96)
// this is not needed bobz

BOOL CCmdCache::GetCommandString(UINT nID, UINT iString, LPCTSTR* pszString, WORD *pflags /* = NULL */, CTE* pCTE /* = NULL */)
{
	if (pCTE == NULL)
	{
		pCTE = GetCommandEntry(nID);
		if (pCTE == NULL)
			return FALSE;
	}

	if (pflags != NULL)
		*pflags = pCTE->flags;

	// Note: (bobz) since macros use command names, we disallow changing the
	// command name (STRING_COMMAND) itself; otherwise macros would not work consistently

	if (pCTE->flags & CT_DYNAMIC_CMD_STRING && (iString != STRING_COMMAND)) // special handling for dynamic strings
	{
		ASSERT(pCTE->GetPack() != NULL);
		CPackage *pPackage = pCTE->GetPack()->GetPackage();
		ASSERT(pPackage != NULL);
		BSTR bstr;
		bstr = pPackage->GetDynamicCmdString(nID, iString);
		if (bstr != NULL)
		{
			g_strCmdDynamic = bstr;	  // only 1 cstring. Caller must not hold
			::SysFreeString(bstr);
			*pszString = (LPCTSTR)g_strCmdDynamic;
			return TRUE; // else fall through
		}
	}

	*pszString = ShellGetNextString(pCTE->szCommand, iString);
	return TRUE;
}

BOOL CCmdCache::GetCommandID(LPCTSTR szCommand, UINT* pnID)
{
	for (int i = 0; i < m_cCommands; i++)
	{
		if (_tcscmp(m_rgCommands[i].szCommand, szCommand) == 0)
		{
			*pnID = m_rgCommands[i].id;
			return TRUE;
		}
	}

	return FALSE;
}

HMENU CCmdCache::GetOleMenu()
{
	return GetCommandMenu(TRUE);
}

// Fill a list box (pList) with each of the possible available menus, except for empty ones
void CCmdCache::FillMenuList(CListBox *pList)
{
	// search all the menus in the command table for any menus in the specified group, and add them
	for (int i = 0; i < m_rgMenuCommands.GetSize() ; i++)
	{
		ASSERT(m_rgMenuCommands[i]<theCmdCache.m_cCommands);

		CTE *pCTE = &m_rgCommands[theCmdCache.m_rgMenuCommands[i]];

		ASSERT(pCTE);
		ASSERT(pCTE->flags & CT_MENU);

		CBMenuPopup *pMenu=theCmdCache.GetMenu(pCTE->id);
		if(	pMenu &&
			pMenu->GetCount()>0)
		{
			LPCTSTR pszMenu;
			VERIFY(GetCommandString(pCTE->id, STRING_MENUTEXT, &pszMenu));

			// remove the ampersands
			CString menuName=pszMenu;
			int nMenuNameLen=menuName.GetLength();
			LPTSTR pszMenuName=menuName.GetBuffer(nMenuNameLen+1); // plus 1 because getlength doesn't include terminator
			GLOBAL_DATA::StripAmpersands(pszMenuName, nMenuNameLen);
			menuName.ReleaseBuffer(-1);

			// add it to the list
			int index=pList->AddString(menuName);
			ASSERT(index!=LB_ERR);
			pList->SetItemData(index, pCTE->id);
		}
	}
}

// Fill a list box (pList) with the first empty custom menu ones
void CCmdCache::FillNewMenuList(CListBox *pList)
{
	if(GetNextFreeMenu()>0)
	{
		CString newMenu;
		newMenu.LoadString(IDS_NEWMENU);
		int index=pList->AddString(newMenu);

		ASSERT(index!=LB_ERR);

		pList->SetItemData(index, CButtonDragListBox::cNextFreeMenu);
	}
}

int CCmdCache::GetNextFreeMenu(void)
{
	// search all the menus in the command table for any menus in the specified group, and add them
	for (int i = 0; i < m_rgMenuCommands.GetSize() ; i++)
	{
		ASSERT(m_rgMenuCommands[i]<theCmdCache.m_cCommands);

		CTE *pCTE = &m_rgCommands[theCmdCache.m_rgMenuCommands[i]];

		ASSERT(pCTE);
		ASSERT(pCTE->flags & CT_MENU);

		if(	pCTE->id>=IDM_CUSTOMMENU_BASE && 
			pCTE->id<=IDM_CUSTOMMENU_LAST)
		{
			CBMenuPopup *pMenu=theCmdCache.GetMenu(pCTE->id);
			if(	pMenu &&
				pMenu->GetCount()==0)
			{
				return pCTE->id;
			}
		}
	}

	return 0;
}

#ifdef _DEBUG
void CCmdCache::PrintCommandTable()
{
	CDC dc;
	CDC dcBitmap;
	
//	CWaitCursor waitCursor;
	
	char szDevice [256];
	GetProfileString("Windows", "device", "", szDevice, sizeof (szDevice));
	if (szDevice[0] == 0)
	{
		AfxMessageBox("No printer selected.", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	char szDesc [128];
	char szDriver [32];
	char szPort [32];
	if (sscanf(szDevice, "%[^,],%[^,],%[^,]", szDesc, szDriver, szPort) != 3)
	{
		AfxMessageBox(CString(szDevice) + "\n\nInvalid printer description.", 
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	if (!dc.CreateDC(szDriver, szDesc, szPort, NULL))
	{
		AfxMessageBox("Cannot open printer.", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	int nPageWidth = dc.GetDeviceCaps(HORZRES);
	int nPageHeight = dc.GetDeviceCaps(VERTRES);
	int nXPelsPerInchPrinter = dc.GetDeviceCaps(LOGPIXELSX);
	int nYPelsPerInchPrinter = dc.GetDeviceCaps(LOGPIXELSY);

	CFont font;
	font.CreateFont(MulDiv(8, nYPelsPerInchPrinter, 72), 0, 0, 0, 
		FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		"Arial");
	dc.SelectObject(&font);

	dc.StartDoc("Commands");

	////
	
	dcBitmap.CreateCompatibleDC(NULL);
	int nXPelsPerInchScreen = dcBitmap.GetDeviceCaps(LOGPIXELSX);
	int nYPelsPerInchScreen = dcBitmap.GetDeviceCaps(LOGPIXELSY);
	
	BOOL bInPage = FALSE;
	int y;

	for (int i = 0; i < m_cCommands; i += 1)
	{
		if (!bInPage)
		{
			dc.StartPage();
			bInPage = TRUE;

			y = nYPelsPerInchPrinter;
		}

		CTE* pCTE = &m_rgCommands[i];
		
		int x = nXPelsPerInchPrinter;
		
		HBITMAP hbmWell;
		int iGlyph;
		CSize sizeImage=CCustomBar::GetDefaultBitmapSize(theApp.m_bLargeToolBars);

		if(theApp.GetCommandBitmap(pCTE->id, &hbmWell, &iGlyph, theApp.m_bLargeToolBars, pCTE))
		{
			CBitmap* pToolbarBitmap = CBitmap::FromHandle(hbmWell);
			
			CBitmap* pOldBitmap = dcBitmap.SelectObject(pToolbarBitmap);
			
			dc.StretchBlt(x, y, 
				sizeImage.cx * nXPelsPerInchPrinter / nXPelsPerInchScreen, 
				sizeImage.cy * nYPelsPerInchPrinter / nYPelsPerInchScreen, 
				&dcBitmap, iGlyph * sizeImage.cx, 0, sizeImage.cx, sizeImage.cy, SRCCOPY);
			
			dcBitmap.SelectObject(pOldBitmap);
		}
		
		x += sizeImage.cx * nXPelsPerInchPrinter / nXPelsPerInchScreen;
		x += nXPelsPerInchPrinter / 8;
		
		CString str;
		str = pCTE->szCommand;
		dc.TextOut(x, y, str);
		int cyText = dc.GetTextExtent(str, str.GetLength()).cy;
		x += 2 * nXPelsPerInchPrinter;
		
		str = ShellGetNextString(pCTE->szCommand, STRING_TIP);
		int nTipIndex = str.Find('\n');
		if (nTipIndex != -1)
			str = str.Left(nTipIndex);
		dc.TextOut(x, y, str);
	
		y += max(sizeImage.cy * nYPelsPerInchPrinter / nYPelsPerInchScreen, cyText);

		if (y + max(sizeImage.cy * nYPelsPerInchPrinter / nYPelsPerInchScreen, cyText) >= nPageHeight - nYPelsPerInchPrinter)
		{
			dc.EndPage();
			bInPage = FALSE;
		}
	}
	
	if (bInPage)
		dc.EndPage();
	
	dc.EndDoc();
}
#endif

#define AVERAGE_COMMAND_NAME_LENGTH 20

void CCmdCache::FillAllCommandsList(CListBox* pList, BOOL bKeyboard /*=FALSE*/)
{
	// always too much space, but most commands make it into the list, so not too much. And this speeds
	// things up a lot.
	pList->InitStorage(theCmdCache.m_cCommands, AVERAGE_COMMAND_NAME_LENGTH*theCmdCache.m_cCommands);

	// iterate the whole command cache
	for (int i = 0; i < m_cCommands; i++)
	{
		CTE* pCTE = &m_rgCommands[i];

		if(!bKeyboard)
		{
			// if it's not for the keyboard dialog, suppress non-button commands
			if (pCTE->flags & CT_NOBUTTON)
			{
				continue;
			}
		}

		// Suppress general nogo commands
		if ((pCTE->flags & (CT_NOUI|CT_NOKEY)) == 0)
		{
			// don't add menus which currently have no items
			if(pCTE->flags & CT_MENU)
			{
				CBMenuPopup * pMenu=theCmdCache.GetMenu(pCTE->id);
				if(pMenu==NULL)
				{
					continue;
				}
				else
				{
					// don't show menu in list if it has no items. Do show it if no items are visible
					if(pMenu->GetMenuItemCount()==0)
					{
						// The menu is empty, but might be present in one of the toolbars

						// TRUE if we found it somewhere in the UI
						BOOL bFound=FALSE;
						int barIndex=0;
						// cycle thru visible toolbars
						while(barIndex<CASBar::s_aUsage.GetSize()) {
							CASBar *pBar=(CASBar *)(CASBar::s_aUsage[barIndex]);
						
							ASSERT(pBar!=NULL);

							if(pBar->FindButton(pCTE->id))
							{
								bFound=TRUE;
							}

							++barIndex;
						}

						// not in any bar implies don't add to list
						if(!bFound)
						{
							continue;
						}
					}
				}
			}

			LPCTSTR lpszCmdName = pCTE->szCommand;
			int index = pList->AddString(lpszCmdName);
			if(index!=LB_ERR)
			{
				pList->SetItemData(index, (DWORD) pCTE->id);
			}
		}
	}
}

// The deleted commands list contains all commands which are no longer present on the menu or toolbar they started
// out on. We do this by comparing the menus and bars with what would happen to them if we reset them.
void CCmdCache::FillDeletedCommandsList(CListBox* pList)
{
	// Iterate all menus, then all bars
	CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();

	pList->SetRedraw(FALSE);
	pFrame->FillDeletedMenuItemList(pList);

	// now fill up all the toolbars; easier to do this because we can ask about each toolbar in from the owning package

	// find all the toolbars
	CObArray aToolWorkers;
	pFrame->m_pManager->ArrayOfType(dtEdit, &aToolWorkers, TRUE, TRUE);

	CString str;
	int nBars = aToolWorkers.GetSize();
	CDockWorker* pDocker;
	
	for (int iBar = 0; iBar < nBars; iBar++)
	{
		pDocker = (CDockWorker*) aToolWorkers.GetAt(iBar);

		if (pDocker->IsAvailable())
		{
			// no default exists for these
			if (LOWORD(pDocker->m_nIDWnd) < IDTB_SHELL_BASE)
				continue;
			
			// only can find missing for bars which have default.
			if (pDocker->m_nIDPackage != PACKAGE_SUSHI ||
				LOWORD(pDocker->m_nIDWnd) < IDTB_CUSTOM_BASE)
			{
				// get the bar associated with the docker
				CASBar* pBar = (CASBar*) pDocker->m_pWnd;

				// If the bar is null it must still be default, as custombars are always loaded. So we only need to look for
				// deleted buttons in non-null bars
				if (pBar != NULL)
				{
					CPackage* pGrp = theApp.GetPackage(pDocker->m_nIDPackage);

					// Shouldn't ever happen.
					if (pGrp == NULL)
						continue;
					
					HGLOBAL hglob = pGrp->GetToolbarData(LOWORD(pDocker->m_nIDWnd));

					if (hglob != NULL)
					{
						TOOLBARINIT FAR* lptbi = (TOOLBARINIT*) ::GlobalLock(hglob);
						UINT FAR* lpIDArray = (UINT FAR*) (lptbi + 1);

						// lpIDarray are the buttons we should put into the bar, so we now compare the bar with those ids

						// This algorithm is substantially similar to the one for menus. 
						// Now we have to compare the bar with the default comamnd ids. We iterate over the default ids, searching 
						// for each command in the current bar. Since the current bar has probably not changed much, we start looking 
						// for the next command/ just after where we found the previous one. This will tend to mean that the whole 
						// comparison isn't too slow.

						// This is the item we start looking at in the current bar
						int iInitialCurrentItem=0;

						// count of items in both places
						int nCurrentItems=pBar->GetCount();
						int nDefaultItems=lptbi->nIDCount;

						// iterate over default menu
						for(int iDefaultItem=0; iDefaultItem<nDefaultItems; ++iDefaultItem)
						{
							UINT nDefaultId=lpIDArray[iDefaultItem];

							// if it's not a separator or in some other way dud.
							if(nDefaultId!=0)
							{
								// iterate over current menu
								int iCurrentItem=iInitialCurrentItem;

								BOOL bFound=FALSE;


								if(nCurrentItems > 0)
								{
									do
									{
										UINT nCurrentId=pBar->GetItemID(iCurrentItem);

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
								}
								// add the missing command to the list box
								if(!bFound)
								{
									// get the command table entry for the missing command from the menu, which caches it.
									CTE *pCTEMissing=GetCommandEntry(nDefaultId);
									
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

						::GlobalUnlock(hglob);
						::GlobalFree(hglob);
					}
				}
			}
		}
	}
	


	pList->SetRedraw(TRUE);
	pList->InvalidateRect(NULL);
}

// To determine whether the given command is active given the current packet situation
// NOTE: This funciton has many exit points.
BOOL CTE::IsActive(void)
{
	UINT nPacketID = pPack->PacketID();

	// If it's an unpacketed command, it's active
	if (nPacketID != PACKET_NIL)
	{
		// Otherwise, we need to check the command's packet against the active one.
		CPack *pActivePack = theApp.m_pActivePacket;

		// If there's no active packet, all packet commands are hidden
		if (pActivePack == NULL)
		{
			return FALSE;
		}

		if(nPacketID==PACKET_SHARED)
		{
			if(GetActivePack()!=NULL)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			// If the packet isn't shared, then it's a case of simple comparison
			if(pActivePack != pPack)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CTE::FindPackage(CPackage *pFindPackage)
{
	UINT nPacketID = pPack->PacketID();

	if(nPacketID!=PACKET_SHARED)
	{
		UINT nPackageID = pPack->GetPackage()->PackageID();

		return pFindPackage->PackageID()==nPackageID;
	}
	else
	{
		return theCmdCache.FindPackage(id, pFindPackage);
	}
}

// Finds if a pack is in the list
BOOL CCmdCache::FindPackage(UINT id, CPackage *pFindPackage)
{
	// For shared packets we must iterate over the whole packet array
	CTypedPtrArray<CObArray, CPack*> *pPackets=NULL;

	if(m_pSharedCommands)
	{
		// If this fails, the shared command isn't in the table, which probably implies failure during command table load
		VERIFY(m_pSharedCommands->Lookup(id, pPackets));

		// If this fails, we probably added something bad to the shared table.
		ASSERT(pPackets);

		UINT nFindID=pFindPackage->PackageID();

		// Search for the pack among the shared ones.
		for(int i=0;i<pPackets->GetSize(); ++i)
		{
			UINT nPackageID = (*pPackets)[i]->GetPackage()->PackageID();

			if(nPackageID==nFindID)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	else
	{
		// shouldn't be able to have shared commands but no map
		ASSERT(FALSE);
		return FALSE;
	}
}

BOOL CTE::FindPack(CPack *pFindPack)
{
	UINT nPacketID = pPack->PacketID();

	if(nPacketID!=PACKET_SHARED)
	{
		return pFindPack->PacketID()==nPacketID;
	}
	else
	{
		return theCmdCache.FindPack(id, pFindPack);
	}
}

// Finds if a pack is in the list
BOOL CCmdCache::FindPack(UINT id, CPack *pFindPack)
{
	// For shared packets we must iterate over the whole packet array
	CTypedPtrArray<CObArray, CPack*> *pPackets=NULL;

	if(m_pSharedCommands)
	{
		// If this fails, the shared command isn't in the table, which probably implies failure during command table load
		VERIFY(m_pSharedCommands->Lookup(id, pPackets));

		// If this fails, we probably added something bad to the shared table.
		ASSERT(pPackets);

		// Search for the pack among the shared ones.
		for(int i=0;i<pPackets->GetSize(); ++i)
		{
			if((*pPackets)[i]==pFindPack)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	else
	{
		// shouldn't be able to have shared commands but no map
		ASSERT(FALSE);
		return FALSE;
	}
}

// Gets the pack, adjusted for which pack is active, and resolving sharing issues
CPack *CTE::GetActivePack(void)
{
	UINT nPacketID = pPack->PacketID();

	if(nPacketID!=PACKET_SHARED)
	{
		return pPack;
	}
	else
	{
		return theCmdCache.GetActivePack(id);
	}
}

CPack *CCmdCache::GetActivePack(UINT id)
{
	if(FindPack(id,theApp.m_pActivePacket))
	{
		return theApp.m_pActivePacket;
	}
	else
	{
		return NULL;
	}
}

// Gets any real pack, resolving sharing issues
CPack *CTE::GetFirstPack(void)
{
	UINT nPacketID = pPack->PacketID();

	if(nPacketID!=PACKET_SHARED)
	{
		return pPack;
	}
	else
	{
		return theCmdCache.GetFirstPack(id);
	}
}

CPack *CCmdCache::GetFirstPack(UINT id)
{
	// For shared packets we must iterate over the whole packet array
	CTypedPtrArray<CObArray, CPack*> *pPackets=NULL;

	if(m_pSharedCommands)
	{
		// If this fails, the shared command isn't in the table, which probably implies failure during command table load
		VERIFY(m_pSharedCommands->Lookup(id, pPackets));

		// If this fails, we probably added something bad to the shared table.
		ASSERT(pPackets);

		// table should never exist with no entries.
		ASSERT(pPackets->GetSize()>0);
		return (*pPackets)[0];
	}
	else
	{
		// shouldn't be able to have shared commands but no map
		ASSERT(FALSE);
		return NULL;
	}
}

void CCmdCache::FlushMenuSizes(void)
{
	// removed cached sizes
	CBMenuItem::FlushSizeCache();
	// and recalculate them
	CBMenuItem::EnsureSizeCache();

	if(m_pMenus)
	{
		// Flush the size caches on all of the menus and items
		POSITION pos=GetFirstMenuPosition();
		CBMenuPopup *pMenu;
		UINT nIDMenu;

		while(pos!=NULL)
		{
			GetNextMenu(pos, nIDMenu, pMenu);

			if(pMenu)
			{
				pMenu->FlushSizeCache();
			}
		}
	}
}

