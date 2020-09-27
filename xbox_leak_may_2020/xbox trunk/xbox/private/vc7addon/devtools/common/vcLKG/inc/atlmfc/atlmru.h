//NONSHIP

// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLMRU_H__
#define __ATLMRU_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error atlmru.h requires atlbase.h to be included first
#endif

#ifndef __ATLRES_H__
	#error atlmru.h requires atlres.h to be included first
#endif

namespace ATL
{
/////////////////////////////////////////////////////////////////////////////
// CRecentDocumentList - MRU List Support

class CRecentDocumentList
{
public:
	struct _DocEntry
	{
		TCHAR szDocName[MAX_PATH];
		bool operator==(const _DocEntry& de) const
		{ return (lstrcmpi(szDocName, de.szDocName) == 0); }
	};

	CSimpleArray<_DocEntry> m_arrDocs;
	int m_nMaxEntries;	// default is 4
	HMENU m_hMenu;

	TCHAR m_szNoEntries[MAX_PATH];

// Constructor
	CRecentDocumentList() : m_hMenu(NULL), m_nMaxEntries(4)
	{ }

// Attributes
	HMENU GetMenuHandle()
	{
		return m_hMenu;
	}
	void SetMenuHandle(HMENU hMenu)
	{
		ATLASSERT(hMenu == NULL || ::IsMenu(hMenu));
		m_hMenu = hMenu;
		if(m_hMenu == NULL || (::GetMenuString(m_hMenu, ID_FILE_MRU_FIRST, m_szNoEntries, MAX_PATH, MF_BYCOMMAND) == 0))
			lstrcpy(m_szNoEntries, _T("(empty)"));
	}
	int GetMaxEntries()
	{
		return m_nMaxEntries;
	}
	void SetMaxEntries(int nMaxEntries)
	{
		ATLASSERT(nMaxEntries > 0 && nMaxEntries < (ID_FILE_MRU_LAST - ID_FILE_MRU_FIRST + 1));
		m_nMaxEntries = nMaxEntries;
	}

// Operations
	BOOL AddToList(LPCTSTR lpstrDocName)
	{
		_DocEntry de;
		if(lstrcpy(de.szDocName, lpstrDocName) == NULL)
			return FALSE;

		for(int i = 0; i < m_arrDocs.GetSize(); i++)
		{
			if(lstrcmpi(m_arrDocs[i].szDocName, lpstrDocName) == 0)
			{
				m_arrDocs.RemoveAt(i);
				break;
			}
		}

		if(m_arrDocs.GetSize() == m_nMaxEntries)
			m_arrDocs.RemoveAt(0);

		BOOL bRet = m_arrDocs.Add(de);
		if(bRet)
			bRet = UpdateMenu();
		return bRet;
	}
	BOOL GetFromList(int nItemID, LPTSTR lpstrDocName)
	{
		int nIndex = m_arrDocs.GetSize() - (nItemID - ID_FILE_MRU_FIRST) - 1;
		if(nIndex < 0 || nIndex >= m_arrDocs.GetSize())
			return FALSE;
		return (lstrcpy(lpstrDocName, m_arrDocs[nIndex].szDocName) != NULL);
	}
	BOOL RemoveFromList(int nItemID)
	{
		int nIndex = m_arrDocs.GetSize() - (nItemID - ID_FILE_MRU_FIRST) - 1;
		BOOL bRet = m_arrDocs.RemoveAt(nIndex);
		if(bRet)
			bRet = UpdateMenu();
		return bRet;
	}
	BOOL MoveToTop(int nItemID)
	{
		int nIndex = m_arrDocs.GetSize() - 1 - (nItemID - ID_FILE_MRU_FIRST);
		if(nIndex < 0 || nIndex >= m_arrDocs.GetSize())
			return FALSE;
		_DocEntry de;
		de = m_arrDocs[nIndex];
		m_arrDocs.RemoveAt(nIndex);
		BOOL bRet = m_arrDocs.Add(de);
		if(bRet)
			bRet = UpdateMenu();
		return bRet;
	}

	BOOL ReadFromRegistry(LPCTSTR lpstrRegKey)
	{
		CRegKey rkParent;
		CRegKey rk;
		LONG lRet = 0;

		lRet = rkParent.Open(HKEY_CURRENT_USER, lpstrRegKey);
		if(lRet != 0)
			return FALSE;
		lRet = rk.Open(rkParent, _T("Recent Document List"));
		if(lRet != 0)
			return FALSE;

		DWORD dwRet;
		lRet = rk.QueryValue(dwRet, _T("DocumentCount"));
		if(lRet == 0 || dwRet > 0 && dwRet < (ID_FILE_MRU_LAST - ID_FILE_MRU_FIRST + 1))
			m_nMaxEntries = dwRet;

		m_arrDocs.RemoveAll();

		TCHAR szRetString[MAX_PATH];
		_DocEntry de;

		for(int nItem = m_nMaxEntries; nItem > 0; nItem--)
		{
			TCHAR szBuff[11];
			wsprintf(szBuff, _T("Document%i"), nItem);
			DWORD dwCount = MAX_PATH;
			lRet = rk.QueryValue(szRetString, szBuff, &dwCount);
			if(lRet == 0 && (lstrcpy(de.szDocName, szRetString) != NULL))
				m_arrDocs.Add(de);
		}

		rk.Close();
		rkParent.Close();

		return UpdateMenu();
	}
	BOOL WriteToRegistry(LPCTSTR lpstrRegKey)
	{
		CRegKey rkParent;
		CRegKey rk;
		LONG lRet = 0;

		lRet = rkParent.Create(HKEY_CURRENT_USER, lpstrRegKey);
		if(lRet != 0)
			return FALSE;
		lRet = rk.Create(rkParent, _T("Recent Document List"));
		if(lRet != 0)
			return FALSE;

		lRet = rk.SetValue(m_nMaxEntries, _T("DocumentCount"));

		// set new values
		int nItem;
		for(nItem = m_arrDocs.GetSize(); nItem > 0; nItem--)
		{
			TCHAR szBuff[11];
			wsprintf(szBuff, _T("Document%i"), nItem);
			TCHAR szDocName[MAX_PATH];
			GetFromList(ID_FILE_MRU_FIRST + nItem - 1, szDocName);
			lRet = rk.SetValue(szDocName, szBuff);
		}

		// delete unused keys
		for(nItem = m_arrDocs.GetSize() + 1; nItem < (ID_FILE_MRU_LAST - ID_FILE_MRU_FIRST + 1); nItem++)
		{
			TCHAR szBuff[11];
			wsprintf(szBuff, _T("Document%i"), nItem);
			lRet = rk.DeleteValue(szBuff);
		}

		rk.Close();
		rkParent.Close();

		return TRUE;
	}

// Implementation
	BOOL UpdateMenu()
	{
		if(m_hMenu == NULL)
			return FALSE;
		ATLASSERT(::IsMenu(m_hMenu));

		int nItems = ::GetMenuItemCount(m_hMenu);
		int nInsertPoint;
		for(nInsertPoint=0; nInsertPoint<nItems; nInsertPoint++)
		{
			MENUITEMINFO mi;
			mi.cbSize = sizeof(MENUITEMINFO);
			mi.fMask = MIIM_ID;
			::GetMenuItemInfo(m_hMenu, nInsertPoint, TRUE, &mi);
			if (mi.wID == ID_FILE_MRU_FIRST)
				break;
		}
		ATLASSERT(nInsertPoint < nItems && "You need a menu item with an ID = ID_FILE_MRU_FIRST");

		int nItem;
		for(nItem = ID_FILE_MRU_FIRST; nItem < ID_FILE_MRU_FIRST + m_nMaxEntries; nItem++)
		{
			// keep the first one as an insertion point
			if (nItem != ID_FILE_MRU_FIRST)
				::DeleteMenu(m_hMenu, nItem, MF_BYCOMMAND);
		}

		TCHAR szItemText[MAX_PATH + 6];		// add space for &, 2 digits, and a space
		int nSize = m_arrDocs.GetSize();
		nItem = 0;
		if(nSize > 0)
		{
			for(nItem = 0; nItem < nSize; nItem++)
			{
				wsprintf(szItemText, _T("&%i %s"), nItem + 1, m_arrDocs[nSize - 1 - nItem].szDocName);
				::InsertMenu(m_hMenu, nInsertPoint + nItem, MF_BYPOSITION | MF_STRING, ID_FILE_MRU_FIRST + nItem, szItemText);
			}
		}
		else	// empty
		{
			::InsertMenu(m_hMenu, nInsertPoint, MF_BYPOSITION | MF_STRING, ID_FILE_MRU_FIRST, m_szNoEntries);
			::EnableMenuItem(m_hMenu, ID_FILE_MRU_FIRST, MF_GRAYED);
		}
		::DeleteMenu(m_hMenu, nInsertPoint + nItem, MF_BYPOSITION);

		return TRUE;
	}
};

}; //namespace ATL

#endif // __ATLMRU_H__
