///////////////////////////////////////////////////////////////////////////////
//	CMDTABLE.CPP
//

#include "stdafx.h"
#include "bardockx.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void RemoveAccel(CString& strMenu);

/////////////////////////////////////////////////////////////////////////////
//	CToolGroup class

CToolGroup::CToolGroup()
{
	m_nCmds = 0;
	m_nId=0;
	// OPTIMIZE: Initial size and grow values
	m_aCmds.SetSize(250, 50);
}

void CToolGroup::AddGroup(int nGroup)
{
	// first do commands, so everything in one group is together
	for (int i = 0; i < theCmdCache.m_cCommands; i++)
	{
		CTE* pCTE = &theCmdCache.m_rgCommands[i];

		if ((pCTE->flags & CT_NOUI) == 0 && pCTE->group == nGroup)
		{
			if((pCTE->flags & CT_MENU)==0)
			{
				// add for non-menus
				m_aCmds.SetAtGrow(m_nCmds++, pCTE);
			}
		}
	}

	// then do groups contained on submenus
	for (i = 0; i < theCmdCache.m_cCommands; i++)
	{
		CTE* pCTE = &theCmdCache.m_rgCommands[i];

		if ((pCTE->flags & CT_NOUI) == 0 && pCTE->group == nGroup)
		{
			if(pCTE->flags & CT_MENU)
			{
				// menus can't be shared
				ASSERT(pCTE->GetPack()->PacketID()!=PACKET_SHARED);

				// recurse for submenus
				if(	pCTE->GetPack() &&
					pCTE->GetPack()->GetPackage()) 
				{
					POPDESC *ppop=pCTE->GetPack()->GetPackage()->GetMenuDescriptor(pCTE->id);
					if(ppop)
					{
						Fill(ppop);
					}
				}

			}
		}
	}
}

void CToolGroup::Fill(POPDESC* ppop, UINT nId)
{
	// if this is a top level call (and not a recursion), set up the group's id
	if(nId!=0)
	{
		m_nId=nId;
	}

	if (m_strGroup.IsEmpty())
	{
		LPCTSTR pszCmd;
		VERIFY(theCmdCache.GetCommandString(ppop->cmdID, STRING_MENUTEXT, &pszCmd));
		m_strGroup=pszCmd;
		RemoveAccel(m_strGroup);
	}

	MTM* pmtm = &ppop->rgmtm[0];
	UINT idLast = 0;

	while (pmtm->id != POP_NIL)
	{
		// Add commands from this group.
		if (pmtm->id != idLast && pmtm->idString == POP_IDS_NIL)
		{
			AddGroup((int) pmtm->id);
			idLast = pmtm->id;
		}

		pmtm++;
	}
}

LPCTSTR CToolGroup::GetCommandName( UINT nCmdID )
{
	for (int i = 0; i < m_nCmds; i++)
	{
		if ((UINT) ((CTE *)m_aCmds[i])->id == nCmdID)
			return ((CTE *)m_aCmds[i])->szCommand;
	}

	return NULL;
}

// Arbitrary number, but you wouldn't be able to see this many buttons
// in the customize dialog anyway.
#define MAX_BUTTONS 256

CASBar* CToolGroup::CreateCASBar(CWnd* pParent, CDockManager* pManager)
{
	TOOLBARINIT tbi;
	UINT aToolIDs[MAX_BUTTONS];
	CTE* pCTE;

	tbi.nIDWnd = MAKEDOCKID(PACKAGE_SUSHI,0);
	tbi.nIDCount = 0;

	int nOldGroup = ((CTE *) m_aCmds[0])->group;
	for (int i = 0; i < m_nCmds && tbi.nIDCount < MAX_BUTTONS - 1; i++)
	{
		pCTE = (CTE*) m_aCmds[i];
		if (theApp.HasCommandBitmap(pCTE->id))
		{
			if (pCTE->group != nOldGroup &&
				(nOldGroup & 1)==0) // lo bit set means no separator
			{
				nOldGroup = pCTE->group;
				aToolIDs[tbi.nIDCount++] = ID_SEPARATOR;
			}

			aToolIDs[tbi.nIDCount++] = (UINT) pCTE->id;
		}
	}

	// No empty toolbars.
	if (tbi.nIDCount == 0)
		return NULL;

	CASBar *pBar = new CASBar;
	if (pBar->Create(pParent, pManager, WS_VSCROLL , &tbi, aToolIDs, NULL))
	{
		pBar->m_dwStyle &= ~(CBRS_TOOLTIPS | CBRS_FLYBY);
		pBar->EnableToolTips(FALSE);
	}

	return pBar;
}	

void CToolGroup::FillCommandList(CListBox *pList, BOOL bForButtons)
{
	LPCTSTR lpszCmdName;

	for (int i = 0; i < m_nCmds; i++)
	{
		lpszCmdName = ((CTE*) m_aCmds[i])->szCommand;
		if (lpszCmdName && lstrlen(lpszCmdName) &&
			!(((CTE*) m_aCmds[i])->flags & CT_NOKEY))
		{
			if(!bForButtons ||
			   (((CTE*) m_aCmds[i])->flags & CT_NOBUTTON)==0)
			{
				int index = pList->AddString(lpszCmdName);
				pList->SetItemData(index, (DWORD) ((CTE*) m_aCmds[i])->id);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//	CAppToolGroups class

// We  only want one  of these at a time.
CAppToolGroups* CAppToolGroups::s_pAppToolGroups = NULL;

CAppToolGroups* CAppToolGroups::GetAppToolGroups(BOOL bShowWaitCursor /* = FALSE */)
{
	if (s_pAppToolGroups == NULL)
	{
		if (bShowWaitCursor)
			AfxGetApp()->BeginWaitCursor();

		s_pAppToolGroups = new CAppToolGroups;

		if (bShowWaitCursor)
			AfxGetApp()->EndWaitCursor();
	}

	s_pAppToolGroups->m_nUsage++;

	return s_pAppToolGroups;
}

void CAppToolGroups::ReleaseAppToolGroups()
{
	if (s_pAppToolGroups != NULL && --s_pAppToolGroups->m_nUsage == 0)
	{
		delete s_pAppToolGroups;
		s_pAppToolGroups = NULL;
	}
}

CAppToolGroups::CAppToolGroups()
{
	m_nUsage = 0;
	m_nCmds=0;

	//REVIEW: Support for a dirty command cache
	m_wSyncID = 0;
	m_rgGroups = NULL;
	ScanCommandCache();
}

CAppToolGroups::~CAppToolGroups()
{
	delete [] m_rgGroups;
}

void CAppToolGroups::GetCommandName(UINT nCmdID, CString& strName)
{
	LPCTSTR lpszName;

	if(theCmdCache.GetCommandString(nCmdID, STRING_COMMAND, &lpszName))
	{
		strName = lpszName;
		if(strName.GetLength()>0)
		{
			return;
		}
	}

	VERIFY(strName.LoadString(IDS_UNNAMED));
}

void CAppToolGroups::FillGroupList(CComboBox *pList)
{
	for (int i = 0; i < m_nGroups; i++)
	{
		if (m_rgGroups[i].m_nCmds == 0)
			continue;

		int index = pList->AddString(m_rgGroups[i].m_strGroup);
		pList->SetItemData(index, i);
	}
}

void CAppToolGroups::FillCommandList(UINT nGroup, CListBox *pList )
{
	m_rgGroups[nGroup].FillCommandList(pList);
}

// Fills an array with one toolbar for each category that has more than 0 commands.
// If the category indicates it is textual, then NULL is added to the array
void CAppToolGroups::CreateCustomizeToolbars(	CWnd* pParent,	
												CDockManager* pManager, 
												CObArray* pToolbars,	// The bars, or null for text categories
												CStringArray *pTitles,	// The titles
												CWordArray *paIds)		// the ids of the groups
{
	ASSERT_VALID(pToolbars);
	ASSERT_VALID(pParent);
	ASSERT(pToolbars->GetSize() == 0);
	
	CASBar *pBar;
	for (int i = 0; i < m_nGroups; i++)
	{
		if (m_rgGroups[i].m_nCmds == 0)
			continue;

		if(m_rgGroups[i].m_nId!=0)
		{
			CTE *pCTE=theCmdCache.GetCommandEntry(m_rgGroups[i].m_nId);
			if(	pCTE &&
				pCTE->GetPack())
			{
				CPackage *pPackage=pCTE->GetPack()->GetPackage();
				if(pPackage)
				{
					if(pPackage->IsCategoryTextual(m_rgGroups[i].m_nId))
					{
						pToolbars->Add(NULL);
						pTitles->Add(m_rgGroups[i].m_strGroup);
						paIds->Add((WORD)m_rgGroups[i].m_nId);
						continue;
					}
				}
			}
		}

		if ((pBar = m_rgGroups[i].CreateCASBar(pParent, pManager)) != NULL)
		{
			pToolbars->Add(pBar);
			pTitles->Add(m_rgGroups[i].m_strGroup);
			paIds->Add((WORD)m_rgGroups[i].m_nId);
		}
	}
}


// This is called by the commands and keyboard tabs of the customize dialog
//  when they're displayed by Tools.Macro.  The dialogs need to prime themselves
//  with the command name passed to them from Tools.Macro, and need to
//  find the group containing that command (currently it'll always
//  be the Macros category).
CToolGroup* CAppToolGroups::GroupFromCommandName(LPCTSTR szCmdName)
{
	// Search through all the groups until we can find this command
	UINT nIDCmd;
	if (!theCmdCache.GetCommandID(szCmdName, &nIDCmd))
		return NULL;

	CTE* pCTE = theCmdCache.GetCommandEntry(nIDCmd);
	for (int i=0; i < m_nGroups; i++)
	{
		CToolGroup* pToolGroup = &(m_rgGroups[i]);
		LPCTSTR szCurrCmdName = pToolGroup->GetCommandName(nIDCmd);
		if (szCurrCmdName == NULL)
			continue;

		if (_tcscmp(szCmdName, szCurrCmdName))
		{
			// Found a name for this command, but it didn't
			//  match szCmdName.  That's really weird.
			return NULL;
		}

		return pToolGroup;
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//	Utility functions

void RemoveAccel(CString& strMenu)
{
	int i = strMenu.Find('&');
	if (i == -1)
		return;
	
	if (i == 0)
		strMenu = strMenu.Right(strMenu.GetLength() - i - 1);
	else if (i == strMenu.GetLength() - 1)
		strMenu = strMenu.Left(i);
	else
	{
		// International code handles both "R&eplace..." and "<text>(&E)..."
		LPCTSTR lpchSrc;
		LPTSTR lpchDest = strMenu.GetBuffer(strMenu.GetLength() + 1) + i;
		lpchDest = _tcsdec(strMenu, lpchDest);
		if (*lpchDest == _T('('))
		{
			lpchSrc = lpchDest;
			while (*lpchSrc != _T(')') && *lpchSrc != _T('\0'))
				lpchSrc = _tcsinc(lpchSrc);
			if (*lpchSrc != _T('\0'))
				lpchSrc = _tcsinc(lpchSrc);
		}
		else
		{
			lpchDest = _tcsinc(lpchDest);
			lpchSrc = lpchDest + 1;
		}
		_tcscpy(lpchDest, lpchSrc);
		strMenu.ReleaseBuffer();
	}
}

