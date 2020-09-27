// CBpopup.cpp : CBmenu replacement for Sushi popup menu support
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// popup menus

CBContextPopupMenu::CBContextPopupMenu()
{
//der 30 may 96 - TODO Remove next line
	m_pListSubMenus = NULL;
	m_idgPrev = 0;
	m_bPopup = FALSE;
	m_nPopupPos = 0;
}

CBContextPopupMenu::~CBContextPopupMenu()
{
	/* der 30 may 96 -	
	if (m_pListSubMenus != NULL)
	{
		POSITION	pos = m_pListSubMenus->GetHeadPosition();

		while (pos != NULL)
			delete m_pListSubMenus->GetNext(pos);

		delete m_pListSubMenus;
	}
	*/

	//DER Not Needed...DestroyMenu();
}

void CBContextPopupMenu::Create(POPDESC* ppop /*=NULL*/)
{
   //DER - Removed because it tries to call ::CreatePopupMenu
   // CreatePopupMenu();

	if (ppop == NULL)
		return;

	UINT nID, nIDLast = POP_SEPARATOR;
	for (int i = 0; (nID = ppop->rgmtm[i].id) != POP_NIL; i++)
	{
		BOOL bAdded=FALSE;
		if (nID == POP_SEPARATOR)
		{
			// No double separators, or separators at end of popup.
			if (nIDLast != POP_SEPARATOR && ppop->rgmtm[i + 1].id != POP_NIL)
			{
				bAdded=AppendMenu(MF_SEPARATOR);
			}
		}
		else
		{
			CString str;
			if (ppop->rgmtm[i].idString != POP_IDS_NIL)
				VERIFY(str.LoadString(ppop->rgmtm[i].idString));

			bAdded=AddItem(nID, str.IsEmpty() ? NULL : (LPCTSTR) str);
		}

		if(bAdded)
		{
			nIDLast = nID;
		}
	}
	m_bPopup = FALSE;
}

CBContextPopupMenu* CBContextPopupMenu::CreateNewSubPopup(CBContextPopupMenu* pParent)
{
	ASSERT(pParent != NULL);

	CBContextPopupMenu* pSubMenu = new CBContextPopupMenu;

	//pSubMenu->Create();
	// No longer needed pSubMenu->CreatePopupMenu(pParent) ;

	/* Der - 30 May As far as I can tell, this isn't used.
	if (pParent->m_pListSubMenus == NULL)
		pParent->m_pListSubMenus = new CObList;

	pParent->m_pListSubMenus->AddTail(pSubMenu);
	*/

	return pSubMenu;
}

BOOL CBContextPopupMenu::AddItem(UINT id, LPCTSTR lpszText)
{
	if (lpszText == NULL)
	{
		CTE *pCTE = theCmdCache.GetCommandEntry(id);

		if (NULL == pCTE)
			return FALSE;

		if ((pCTE->flags & CT_NOUI) != 0)
			return FALSE;	// Hidden menuitem.

		theCmdCache.GetCommandString(id, STRING_MENUTEXT, &lpszText, NULL, pCTE);
	}

   if ((lpszText == NULL) || ( _tcslen(lpszText) == 0))
   {
//#ifdef _DEBUG
         static TCHAR badCmdTable[] = "-PlaceHolder-" ;
         lpszText = badCmdTable ;
//#else
//         return ;
//#endif
      }

	LPTSTR buf = new TCHAR [_tcslen(lpszText) + 1];

	// strip out any trailing tab and keyboard shortcut.

	LPTSTR pchDest = buf; 
	while (*lpszText != _T('\0') && *lpszText != _T('\t'))
	{
		if (*lpszText)
		{
			_tccpy(pchDest, lpszText);
			pchDest = _tcsinc(pchDest);
			lpszText = _tcsinc(lpszText);
		}
	}
	*pchDest = _T('\0');

	BOOL bAdded=AppendMenu(MF_ENABLED, id, buf);

	delete [] buf;

	return bAdded;
}

void CBContextPopupMenu::AddSubMenu(CBContextPopupMenu* pSubMenu, LPCTSTR lpszText)
{
	//DER AppendMenu(MF_POPUP, (UINT)pSubMenu->m_hMenu, lpszText);
   //Note CBMenumPopup uses 4th parm for menu pointer...
   AppendMenu(MF_POPUP, (UINT)0, lpszText, pSubMenu);
}

UINT CBContextPopupMenu::TrackPopup(UINT flags, int x, int y, CWnd* pOwner)
{
	// if using popups from menu resource,
	if (m_bPopup)
	{
      ASSERT(0) ; // CMenuPopup can't do this yet.
      TRACE0("CBContextPoupMenu:TrackPopup -- GetSubMenu NA\r\n") ;
      return FALSE ;
		//CMenu * pmenu = GetSubMenu(m_nPopupPos);
		//ASSERT(pmenu != NULL);
		//return pmenu->TrackPopupMenu(flags, x, y, pOwner);
	}
	else
		return TrackPopupMenu(flags, x, y, pOwner);
}

UINT CBContextPopupMenu::TrackPopup(UINT flags, int x, int y, CWnd* pOwner, CRect rectAvoid)
{
	// if using popups from menu resource,
	if (m_bPopup)
	{
      ASSERT(0) ; // CMenuPopup can't do this yet.
      TRACE0("CBContextPoupMenu:TrackPopup -- GetSubMenu NA\r\n") ;
      return FALSE ;
		//CMenu * pmenu = GetSubMenu(m_nPopupPos);
		//ASSERT(pmenu != NULL);
		//return pmenu->TrackPopupMenu(flags, x, y, pOwner);
	}
	else
		return TrackPopupMenuEx(flags, x, y, pOwner, rectAvoid);
}

#if 0
BOOL CBContextPopupMenu::LoadMenu(UINT nID)
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

void CBContextPopupMenu::SetPos( int nPopupPos )
{
	ASSERT(m_bPopup == TRUE);
	ASSERT(GetSubMenu(nPopupPos)!=NULL);
	m_nPopupPos = nPopupPos;
}

#endif

void CBShowContextPopupMenu(POPDESC* ppop, CPoint pt, CWnd* pWnd /*= NULL*/)
{
   CWnd* pWndCommand = pWnd ;
   if (pWndCommand == NULL)
   {
      //CMainFrame* pWndCommand = (CMainFrame*) AfxGetMainWnd();
      pWndCommand = (CWnd*) AfxGetMainWnd();
      ASSERT_VALID(pWndCommand);
   }

	CBContextPopupMenu menuPopup;
	menuPopup.Create(ppop);

	menuPopup.TrackPopup(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, pWndCommand);
}
