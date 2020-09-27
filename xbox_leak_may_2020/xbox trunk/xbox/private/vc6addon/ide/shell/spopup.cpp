// spopup.cpp : Sushi popup menu support
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// popup menus

CContextPopupMenu::CContextPopupMenu()
{
	m_pListSubMenus = NULL;
	m_idgPrev = 0;
	m_bPopup = FALSE;
	m_nPopupPos = 0;
}

CContextPopupMenu::~CContextPopupMenu()
{
	if (m_pListSubMenus != NULL)
	{
		POSITION	pos = m_pListSubMenus->GetHeadPosition();

		while (pos != NULL)
			delete m_pListSubMenus->GetNext(pos);

		delete m_pListSubMenus;
	}

	DestroyMenu();
}

void CContextPopupMenu::Create(POPDESC* ppop /*=NULL*/)
{
	CreatePopupMenu();

	if (ppop == NULL)
		return;

	UINT nID, nIDLast = POP_SEPARATOR;
	for (int i = 0; (nID = ppop->rgmtm[i].id) != POP_NIL; i++)
	{
		if (nID == POP_SEPARATOR)
		{
			// No double separators, or separators at end of popup.
			if (nIDLast != POP_SEPARATOR && ppop->rgmtm[i + 1].id != POP_NIL)
				AppendMenu(MF_SEPARATOR);
		}
		else
		{
			CString str;
			if (ppop->rgmtm[i].idString != POP_IDS_NIL)
				VERIFY(str.LoadString(ppop->rgmtm[i].idString));

			AddItem(nID, str.IsEmpty() ? NULL : (LPCTSTR) str);
		}

		nIDLast = nID;
	}
	m_bPopup = FALSE;
}

CContextPopupMenu* CContextPopupMenu::CreateNewSubPopup(CContextPopupMenu* pParent)
{
	ASSERT(pParent != NULL);

	CContextPopupMenu* pSubMenu = new CContextPopupMenu;
	pSubMenu->Create();

	if (pParent->m_pListSubMenus == NULL)
		pParent->m_pListSubMenus = new CObList;

	pParent->m_pListSubMenus->AddTail(pSubMenu);

	return pSubMenu;
}

void CContextPopupMenu::AddItem(UINT id, LPCTSTR lpszText)
{
	if (lpszText == NULL)
	{
		CTE *pCTE = theCmdCache.GetCommandEntry(id);

		// Must be found
		ASSERT( pCTE != NULL );

		if ((pCTE->flags & CT_NOUI) != 0)
			return;	// Hidden menuitem.

		theCmdCache.GetCommandString(id, STRING_MENUTEXT, &lpszText, NULL, pCTE);
	}

	LPTSTR buf = new TCHAR [_tcslen(lpszText) + 1];

	// strip out ampersands (&) since popups don't have mnemonics,
	// get any context string, and strip off any trailing tab and
	// keyboard shortcut.

	LPTSTR pchDest = buf; 
	while (*lpszText != _T('\0') && *lpszText != _T('\t'))
	{
		// check for string "(&X)" which is supported by the Japanese's system
		// and get rid of them too
		if ( (*lpszText == _T('(')) && (*(_tcsinc(lpszText)) == _T('&')) )
		{
			while (*lpszText && *lpszText != _T(')'))
				lpszText = _tcsinc(lpszText);
 			ASSERT(*lpszText != '\0');
			lpszText = _tcsinc(lpszText);
		}

		// [olympus 16870 - chauv]
		// make sure it's not NULL here. Otherwise, lpszText will be over incremented pass the NULL terminating
		if (*lpszText)
		{
			if (*lpszText != _T('&'))
			{
				_tccpy(pchDest, lpszText);
				pchDest = _tcsinc(pchDest);
			}

			lpszText = _tcsinc(lpszText);
		}
	}
	*pchDest = _T('\0');

	AppendMenu(MF_ENABLED, id, buf);

	delete [] buf;
}

void CContextPopupMenu::AddSubMenu(CContextPopupMenu* pSubMenu, LPCTSTR lpszText)
{
	AppendMenu(MF_POPUP, (UINT)pSubMenu->m_hMenu, lpszText);
}

BOOL CContextPopupMenu::TrackPopup(UINT flags, int x, int y, CWnd* pOwner)
{
	// if using popups from menu resource,
	if (m_bPopup)
	{
		CMenu * pmenu = GetSubMenu(m_nPopupPos);
		ASSERT(pmenu != NULL);
		return pmenu->TrackPopupMenu(flags, x, y, pOwner);
	}
	else
		return TrackPopupMenu(flags, x, y, pOwner);
}

BOOL CContextPopupMenu::LoadMenu(UINT nID)
{
	if (CMenu::LoadMenu(nID))
	{
		m_bPopup = TRUE;
		SetPos(0);
	}
	else 
	{
		m_bPopup = FALSE;
		m_nPopupPos = 0;
	}
	return m_bPopup;
}

void CContextPopupMenu::SetPos( int nPopupPos )
{
	ASSERT(m_bPopup == TRUE);
	ASSERT(GetSubMenu(nPopupPos)!=NULL);
	m_nPopupPos = nPopupPos;
}

void ShowContextPopupMenu(POPDESC* ppop, CPoint pt)
{
	CMainFrame* pWndCommand = (CMainFrame*) AfxGetMainWnd();
	ASSERT_VALID(pWndCommand);

	CContextPopupMenu menuPopup;
	menuPopup.Create(ppop);

	menuPopup.TrackPopup(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, pWndCommand);
}
