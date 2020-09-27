// lang.cpp : implementation file
//

#include "stdafx.h"
#include "mfcappwz.h"
#include "lang.h"
#include "symbols.h"

#ifndef VS_PACKAGE
#include <slob.h>
#include <utilctrl.h>
#endif // VS_PACKAGE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef VS_PACKAGE
static const TCHAR* szModuleSubdir1 = "\\";
#else
	#ifdef _DEBUG
		static const TCHAR* szModuleSubdir1 = "\\ided\\";
	#else
		static const TCHAR* szModuleSubdir1 = "\\ide\\";
	#endif //_DEBUG
#endif

LangDlls langDlls;

/////////////////////////////////////////////////////////////////////////////
// Localizable text dll functions

// Return whether the DLL is the user's default language
BOOL GetLangFromDllName(const char* szFileName, CString& strLang, BOOL& bUS, DWORD& dwTranslation)
{
	LPVOID abData;
	DWORD handle;
	UINT dwSize;
	UINT dwDummySize;
	LPVOID lpBuffer;
	CString strUnknown;
	strUnknown.LoadString(IDS_UNKNOWN);
	strLang = strUnknown;

	CString strFileName = theDLL.m_strAppwizDir + szModuleSubdir1 + szFileName;

	dwSize = GetFileVersionInfoSize((TCHAR *)(const TCHAR *)strFileName, &handle);
	if (dwSize == 0)
		return FALSE;

	abData = new char[dwSize];
	if (!GetFileVersionInfo((TCHAR *)(const TCHAR *) strFileName, handle, dwSize, abData)
		|| !VerQueryValue(abData, "\\VarFileInfo\\Translation", &lpBuffer, &dwDummySize)
		|| dwSize == 0 || dwDummySize == 0)
	{
		delete [dwSize] abData;
		return FALSE;
	}

	// We've successfully read the versioninfo stuff.  Get the
	//  langid/charset pair
	LANGID langid = *((WORD*)lpBuffer);
	dwTranslation = langid;
	dwTranslation <<= 16;
	dwTranslation |= *((WORD*)lpBuffer+1);
	
	// Now translate the the langid into a localized string of the language name
	LCID lcid = MAKELCID(langid, SORT_DEFAULT);
	char szbufInfo[256];
	int nLength = GetLocaleInfo(lcid, LOCALE_SLANGUAGE | LOCALE_USE_CP_ACP, (LPTSTR) szbufInfo, 254);
	if (nLength != 0)
		strLang = szbufInfo;

	// If this didn't work (e.g., the language isn't yet in the user's version
	//  of NT (like Japanese on US NT3.1)), try the LanguageName string.
	if (strLang == strUnknown)
	{
		char szName[512];
		wsprintf(szName, "\\StringFileInfo\\%04x%04x\\LanguageName",
			*((WORD*)lpBuffer), *((WORD*)lpBuffer+1));
		if (VerQueryValue(abData, szName, &lpBuffer, &dwDummySize))
			strLang = (char*) lpBuffer;
	}

	// If strLang has parentheses in it, change them to brackets, since
	//  we put our own parentheses after the name around the DLL name, and
	//  assume no other parentheses exist.
	const TCHAR* pch = strLang;
	for (int i=0; *pch != '\0'; i += _tclen(pch), pch = _tcsinc(pch))
	{
		if (strLang[i] == _T('('))
			strLang.SetAt(i, _T('['));
		else if (strLang[i] == _T(')'))
			strLang.SetAt(i, _T(']'));
	}
	delete [dwSize] abData;

	// Set bUS
	if (MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) == langid)
		bUS = TRUE;
	else
		bUS = FALSE;

	if (PRIMARYLANGID(langid) == PRIMARYLANGID(GetUserDefaultLangID()))
		return TRUE;
	else
		return FALSE;
}

void ExtractDllName(const CString& strEntry, CString& strResult)
{
	int nStart = strEntry.Find('(');
	int nEnd = strEntry.ReverseFind(')');
	if (nStart >= nEnd)
	{
		// This should only happen if a custom apwz is being creative with the names
		//  of languages it's popping into the language checklist.
		ASSERT(FALSE);
		return;
	}
	CString strDllName = strEntry.Mid(nStart+1, nEnd - nStart - 1);

	// If the DLL has changed, we'll have to update the lang-dependent names.
	if (strResult != strDllName)
		projOptions.m_names.m_bUpdateNames = TRUE;

	strResult = strDllName;
}

void ExtractLangName(const CString& strEntry, CString& strResult)
{
	int nLength = strEntry.Find('(') - 1;
	ASSERT(nLength >= 0);
	strResult = strEntry.Left(nLength);
}

DWORD ExtractTranslation(LPCTSTR szEntry)
{
	szEntry = _tcsrchr(szEntry, ';') + 1;
	DWORD dwTranslation;
	if (sscanf(szEntry, "0x%lx", &dwTranslation) == 1)
		return dwTranslation;
	return 0xffffFFFF;
}

WORD CodepageFromTranslation(DWORD dwTranslation)
{
	// Ignore code page in the translation DWORD.  Instead, ask system what
	//  code page to use.  (If error, revert to CP in dwTranslation.)
	char szbufInfo[16];
	LANGID langid = (WORD) (dwTranslation >> 16);
	LCID lcid = MAKELCID(langid, SORT_DEFAULT);
	if (GetLocaleInfo(lcid, LOCALE_IDEFAULTANSICODEPAGE, (LPTSTR) szbufInfo, 14) == 0)
		return (WORD) (dwTranslation & 0x0000FFFF);
	return (WORD)atoi(szbufInfo);
}

/* void ExtractLangid(CCheckList* pList, const CString& strEntry, WORD& langid)
{
	int nEntry = pList->FindString(-1, strEntry);
	DWORD dwData = pList->GetItemData(nEntry);
	langid = (WORD) dwData;
}*/

HANDLE GetSearchHandle(WIN32_FIND_DATA* pffd)
{
	// NOTE: you MUST close the handle you get back from this method with ::FindClose().
	// Failure to do so will result in a memory leak.
	CString strMatch = theDLL.m_strAppwizDir + szModuleSubdir1 + "appwz*.dll";
	return ::FindFirstFile(strMatch, pffd);
}

BOOL ScanForLangDll()
{
	WIN32_FIND_DATA ffd;
	HANDLE hSearch;
	while ((hSearch = GetSearchHandle(&ffd)) == INVALID_HANDLE_VALUE)
	{
		if (AfxMessageBox(IDP_RESCAN_LANGDLL, MB_RETRYCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
			return FALSE;
	}
	::FindClose(hSearch);
	return TRUE;
}

BOOL AvailableLangDlls::ScanForAvailableLanguages()
{
	m_bLastFilledManually = FALSE;
	m_adwTranslations.RemoveAll();
	m_astrAvailableLangs.RemoveAll();
	CStringList strlFiles;
	WIN32_FIND_DATA ffd;
	HANDLE hSearch = GetSearchHandle(&ffd);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				DWORD dwTranslation;
				CString strEntry;
				BOOL bUS;
				BOOL bDefault = GetLangFromDllName(ffd.cFileName, strEntry, bUS, dwTranslation);
				int nEntry = m_astrAvailableLangs.Add
					( ( ( strEntry + " (" ) + ffd.cFileName ) + ")" );
				if (bDefault)
				{
					m_nDefaultEntry = nEntry;
					m_strDefaultDllName = ffd.cFileName;
					m_dwDefaultTranslation = dwTranslation;
				}
				if (bUS)
					m_nUSEntry = nEntry;
				VERIFY (m_adwTranslations.Add(dwTranslation) == nEntry);
			}
		}
		while (::FindNextFile(hSearch,  &ffd));
		::FindClose(hSearch);
	}

	int nCount = m_astrAvailableLangs.GetSize();
	ASSERT(nCount == m_adwTranslations.GetSize());
	if (nCount == 0)
	{
		// list is empty, so rescan for dlls
		if (!ScanForLangDll())
		{
			// User hit cancel, so get out of here
			return FALSE;
		}
		else
		{
			// We found a dll, so rerun
			return ScanForAvailableLanguages();
		}
	}
	return TRUE;
}

BOOL AvailableLangDlls::InitLangDllList(CComboBox* pCombo)
{
	pCombo->ResetContent();

	// If there are no languages scanned at all, we definitely
	//  want to scan for them now.
	if (!ScanForAvailableLanguagesIfNecessary())
		return FALSE;

	// Also, even if there are languages listed, we may want to
	//  force a rescan...
	if (!IsUserAWX() && m_bLastFilledManually)
	{
		// This is normal appwiz, but languages were last entered manually
		//  by a custom AppWizard.  Thus, rescan lang DLLs from disk
		if (!ScanForAvailableLanguages())
			return FALSE;
	}

	// Load checklist with languages
	int nSize = m_astrAvailableLangs.GetSize();
	ASSERT(nSize > 0);
	ASSERT(nSize == m_adwTranslations.GetSize());
	for (int i=0; i < nSize; i++)
		VERIFY (pCombo->AddString(m_astrAvailableLangs[i]) == i);

	// Check appropriate defaults
	if (!langDlls.CheckPreviouslyCheckedLangs(pCombo))
	{
		// The default or previously selected lang dll doesn't exist.  Select
		//  the user's language, or if unavailable, US English
		if (m_nDefaultEntry == -1)
			m_nDefaultEntry = m_nUSEntry;
		//pList->SetItemData(m_nDefaultEntry, 1);
		pCombo->SetCurSel(m_nDefaultEntry);
	}
	return TRUE;
}

BOOL AvailableLangDlls::GetAvailableLanguages(CStringList& strlLangs)
{
	strlLangs.RemoveAll();

	if (!ScanForAvailableLanguagesIfNecessary())
	{
		// This means that we hadn't yet scanned for languages, and on this scan,
		//  there weren't any.
		return FALSE;
	}

	// Load strlLangs with contents of m_astrAvailableLangs & m_adwTranslations
	int nSize = m_astrAvailableLangs.GetSize();
	ASSERT(nSize > 0);
	for (int i=0; i < nSize; i++)
	{
		CString strEntry;
		strEntry.Format("%s;%#x", (LPCTSTR) m_astrAvailableLangs[i], m_adwTranslations[i]);
		strlLangs.AddTail(strEntry);
	}

	return TRUE;
}

void GetNextLine(CString& strLangsToLoad, CString& rstrLine)
{
	int nEnd = strLangsToLoad.Find('\n');
	if (nEnd == -1)
		nEnd = strLangsToLoad.GetLength();
	ASSERT (strLangsToLoad.GetLength() >= nEnd);

	rstrLine = strLangsToLoad.Left(nEnd);
	if (nEnd + 1 >= strLangsToLoad.GetLength() - 1)
		strLangsToLoad.Empty();
	else
		strLangsToLoad = strLangsToLoad.Mid(nEnd + 1);
}

void AvailableLangDlls::SetSupportedLanguages(LPCTSTR szSupportedLangs, BOOL bParseTranslations /* = TRUE */)
{
	m_bLastFilledManually = TRUE;
	m_astrAvailableLangs.RemoveAll();
	m_adwTranslations.RemoveAll();
	m_nDefaultEntry = -1;
	m_nUSEntry = 0;
	
	CString strLangs = szSupportedLangs;
	CString strLine, strLang;
	DWORD dwTranslation;

	while (!strLangs.IsEmpty())
	{
		GetNextLine(strLangs, strLine);
		ASSERT(!strLine.IsEmpty());

		if (bParseTranslations)
		{
			int nSemicolon = strLine.Find(';');
			if (nSemicolon != -1)
			{
				CString strLang(strLine, nSemicolon);
				if ((dwTranslation = ExtractTranslation(strLine)) != 0xffffFFFF)
				{
					int nEntry = m_astrAvailableLangs.Add(strLang);
					m_adwTranslations.Add(dwTranslation);

					// See if this is English or the default language
					WORD langid = (WORD) (dwTranslation >> 16);
					if (MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) == langid)
						m_nUSEntry = nEntry;
					if (PRIMARYLANGID(langid) == PRIMARYLANGID(GetUserDefaultLangID()))
						m_nDefaultEntry = nEntry;
					continue;
				}
			}

			// If we've reached this point, strLine wasn't of the expected format.  So
			// forget trying to parse out the translation.
			SetSupportedLanguages(szSupportedLangs, FALSE);
			return;
		}

		// We've given up trying to parse out the translation, so just stick the whole
		//  line in m_astrAvailableLangs
		m_astrAvailableLangs.Add(strLine);
	}

	// If m_nDefaultEntry wasn't filled in yet, just make it be English
	if (m_nDefaultEntry == -1)
		m_nDefaultEntry = m_nUSEntry;
}


BOOL LangDlls::CheckPreviouslyCheckedLangs(CComboBox* pCombo)
{
	// LATER: Uncomment the source code lines when we allow multiple languages
	// First, uncheck all langs
	/*int nCount = pList->GetCount();
	for (int i = 0; i < nCount; i++)
		pList->SetItemData(i, 0);*/
	// Then, check the ones in this.
	int nCurrCount = GetSize();
	BOOL bAnyChecked = FALSE;
	for (int i = 0; i < nCurrCount && !m_astrNameLang[i].IsEmpty(); i++)
	{
		int nItem = (pCombo->FindString( -1,
			( ( m_astrNameLang[i] + " (" ) + m_astrNameDll[i] ) + ")" ));
			
		if (nItem != CB_ERR)
		{
			//pList->SetItemData(nItem, 1);
			pCombo->SetCurSel(nItem);
			ASSERT(bAnyChecked == FALSE); // Until we allow multiple languages, there can only be one
			bAnyChecked = TRUE;
		}
	}
	return bAnyChecked;
}


void LangDlls::WriteLangDlls(CComboBox* pCombo)
{
	if (pCombo->GetCount() == 0)
		// We're about to bounce to beginning, so just return
		return;
	
	int nNumChecked = 0;
	int nCount = pCombo->GetCount();
	/*for (int i = 0; i < nCount; i++)
	{
		// We only care about the languages selected
		if (pList->GetItemData(i) != 1)
			continue;
*/
		// LATER: remove this when we have multiple languages, and reinstate the for loop
		int i = pCombo->GetCurSel();
		
		CString strEntry;
		pCombo->GetLBText(i, strEntry);

		if (!strEntry.IsEmpty())
		{
			CString tmp;
			ExtractLangName(strEntry, tmp);
			m_astrNameLang.SetAtGrow(nNumChecked, tmp);
			ExtractDllName(strEntry, tmp);
			m_astrNameDll.SetAtGrow(nNumChecked, tmp);
			m_adwTranslation.SetAtGrow(nNumChecked, m_AvailableLangDlls.GetTranslationAt(i));
			nNumChecked++;
		}
//	}

	// Fill in rest of array with blanks
	int nArraySize = GetSize();
	for (i = nNumChecked; i < nArraySize; i++)
	{
		m_astrNameLang.SetAt(i, NULL);
		m_astrNameDll.SetAt(i, NULL);
	}
	// LATER: remove this when we have multiple languages
	ASSERT(nNumChecked == 1);
}

// General error-induced exit routine
void LangDlls::ComplainAndFreeAll(int nDll)
{
	ASSERT (nDll >= 0 && nDll < GetSize());
	CString strPrompt;
	AfxFormatString1(strPrompt, IDP_BAD_LANG_DLL, m_astrNameDll[nDll]);
	AfxMessageBox(strPrompt);
	FreeLibs();
}


BOOL LangDlls::LoadLangSpecificDefaults()
{
	int nSize = GetSize();
	for (int nDll = 0; nDll < nSize && IsValidEntry(nDll); nDll++)
	{
	
		// Pluck out language-dependent defaults from the langdll (e.g., from appwzenu.dll).
		//  These are stored as special strings in the DLL's stringtable (128 & 129).
		//  See the comment in appwzenu.rc for more information.

		CString strFiles;
		BOOL bFilesAfter;
		if (m_ahDll[nDll] == NULL)
		{
			// If this is an extension which supports a language we don't have, fake the
			//  translation of "Files" and its "after" attribute.
			ASSERT(IsUserAWX());
			strFiles.LoadString(IDS_FILES);
			bFilesAfter = TRUE;
		}
		else
		{
			// Get translation of "Files"
			int nRet = ::LoadString(m_ahDll[nDll], 128, strFiles.GetBuffer(256), 255);
			strFiles.ReleaseBuffer();
			if (nRet == 0 || nRet > 255)
			{
				ComplainAndFreeAll(nDll);
				return FALSE;
			}
		
			// Does "Files" come before or after the adjective?
			CString strAfter;
			nRet = ::LoadString(m_ahDll[nDll], 129, strAfter.GetBuffer(10), 9);
			strAfter.ReleaseBuffer();
			if (nRet == 0 || nRet > 9)
			{
				ComplainAndFreeAll(nDll);
				return FALSE;
			}

			if (strAfter == "0")
				bFilesAfter = FALSE;
			else if (strAfter == "1")
				bFilesAfter = TRUE;
			else
			{
				ComplainAndFreeAll(nDll);
				return FALSE;
			}
		}

		// Store Files & After in this
		m_astrFiles.SetAtGrow(nDll, strFiles);
		m_abFilesAfter.SetAtGrow(nDll, (USHORT)bFilesAfter);

		// TODO: Need to fix m_bUpdateNames
		// Only update these lang-specific names if necessary.  We don't want to
		//  trample over a user's edited name unless the project name changed
		//  or the language changed.
		/* if (!projOptions.m_names.m_bUpdateNames)
			return TRUE;*/
		CString strTag;
		GetTag(strTag);
		CString strDocTag = strTag;
	    if (strDocTag.GetLength() > MAX_TAG)
	        strDocTag = strDocTag.Left(MAX_TAG);

		m_astrDocTag.SetAtGrow(nDll, strDocTag);
		m_astrDocFilter.SetAtGrow(nDll, NULL);
		m_astrDocFileNew.SetAtGrow(nDll, strDocTag);
		m_astrDocRegName.SetAtGrow(nDll, strDocTag + " Document");
		m_astrTitle.SetAtGrow(nDll, projOptions.m_strProj);
		m_abUpdateFilter.SetAtGrow(nDll, TRUE);
		m_abUpdateFileNew.SetAtGrow(nDll, TRUE);
		m_abUpdateRegName.SetAtGrow(nDll, TRUE);

		// Update Win32 filter name
		if (!projOptions.m_names.strDocFileExt.IsEmpty())
		{
			if (bFilesAfter)
				m_astrDocFilter.SetAtGrow(nDll, m_astrDocTag[nDll] + " "
					+ strFiles);
			else
				m_astrDocFilter.SetAtGrow(nDll, strFiles + " "
					+ strDocTag);
			m_astrDocFilter[nDll] += " (*." + projOptions.m_names.strDocFileExt + ")";
		}

	}
	// Update Mac filter name
	// TODO: Fix this
	if (m_abFilesAfter[0])
		projOptions.m_names.strMacFilter = projOptions.m_names.strDocFileType + " "
			+ m_astrFiles[0];
	else
		projOptions.m_names.strMacFilter = m_astrFiles[0] + " "
			+ projOptions.m_names.strDocFileType;
	projOptions.m_names.m_bUpdateNames = FALSE;
	return TRUE;

}

BOOL LangDlls::LoadLibs()
{
	FreeLibs();
	int nArraySize = GetSize();
	for (int i = 0; i < nArraySize && !m_astrNameDll[i].IsEmpty(); i++)
	{
		HINSTANCE hinst;
		CString strFileName = theDLL.m_strAppwizDir + szModuleSubdir1 + m_astrNameDll[i];
	    while ((hinst = LoadLibrary((const TCHAR *)strFileName)) == NULL && !IsUserAWX())
		{
			CString strPrompt;
			AfxFormatString1(strPrompt, IDP_ERROR_LOAD_DLL, m_astrNameDll[0]);
			if (AfxMessageBox(strPrompt, MB_RETRYCANCEL) == IDCANCEL)
				return FALSE;
		}
		m_ahDll.SetAtGrow(i, hinst);
	}

	return TRUE;
}

BOOL LangDlls::LoadLibsByName(LPCTSTR szLibs /* = NULL */)
{
	FreeLibs();
	int nArrayCount = 0;
	if (!m_AvailableLangDlls.ScanForAvailableLanguagesIfNecessary())
		return FALSE;

	if (szLibs == NULL)
	{
		// No libs specified, so load them all.  One by one, fill in m_astrNameDll
		//  with the names of the installed languages.
		CStringList strlLangs;
		if (!m_AvailableLangDlls.GetAvailableLanguages(strlLangs))
			return FALSE;

		POSITION pos = strlLangs.GetHeadPosition();
		while (pos != NULL)
		{
			CString strLang = strlLangs.GetNext(pos);
			CString strDll;
			DWORD dwTranslation;
			ExtractDllName(strLang, strDll);
			if (strDll.IsEmpty() ||
				((dwTranslation = ExtractTranslation(strLang)) == 0xffffFFFF))
			{
				// Gracefully skip over bad DLL names.  We can get such things
				//  if a custom apwz is being creative about what it puts in
				//  the language checklist.
				continue;
			}
			m_astrNameDll.SetAtGrow(nArrayCount, strDll);
			ExtractLangName(strLang, strDll);
			m_astrNameLang.SetAtGrow(nArrayCount, strDll);
			m_adwTranslation.SetAtGrow(nArrayCount, dwTranslation);
			nArrayCount++;
		}
	}
	else
	{
		// Libs are specified, so only load the specified ones
		//  Its value looks like: appwzenu.dll\n0x040904E4\nappwzfra.dll\n0x444404E4
		CString strLangsToLoad = szLibs;
		ASSERT(!strLangsToLoad.IsEmpty());

		// One by one, fill in m_astrNameDll with each line from this string
		CString strLine;
		while (!strLangsToLoad.IsEmpty())
		{
			GetNextLine(strLangsToLoad, strLine);
			ASSERT(!strLine.IsEmpty());
			m_astrNameDll.SetAtGrow(nArrayCount, strLine);
			GetNextLine(strLangsToLoad, strLine);
			ASSERT(!strLine.IsEmpty());
			DWORD dwTranslation;
			sscanf((LPCTSTR) strLine, "0x%lx", &dwTranslation);
			m_adwTranslation.SetAtGrow(nArrayCount++, dwTranslation);
		}
	}

	// Finally, load the libraries
	return LoadLibs();
}

void LangDlls::FreeLibs()
{
	int nSize = m_ahDll.GetSize();
	for (int nDll = 0; nDll < nSize && IsValidEntry(nDll); nDll++)
	{
		if (m_ahDll[nDll] != NULL)
		{
			VERIFY(FreeLibrary(m_ahDll[nDll]));
			m_ahDll[nDll] = NULL;
		}
	}
	m_nDefaultDll = 0;
	m_strDefaultLang.Empty();

#ifdef _DEBUG
	// Make sure that m_ahDll is all NULL'd out
	for (nDll = 0; nDll < nSize; nDll++)
		ASSERT(m_ahDll[nDll] == NULL);
#endif //_DEBUG

}

int LangDlls::GetSize()
{
	int nSize = m_astrNameDll.GetSize();
	ASSERT (m_astrNameLang.GetSize() == nSize);
	return nSize;
}

LangDlls::LangDlls()
{
	Init();
}

BOOL LangDlls::SetDefaultLang(LPCTSTR szLangSuffix)
{
	CString strNameDll;
	strNameDll.Format("appwz%s.dll", szLangSuffix);
	m_strDefaultLang = szLangSuffix;

	// Search for loaded DLL with this name
	int nSize = m_ahDll.GetSize();
	for (int nDll = 0; nDll < nSize && IsValidEntry(nDll); nDll++)
	{
		if (!m_astrNameDll[nDll].CompareNoCase(strNameDll))
		{
			m_nDefaultDll = nDll;
			return TRUE;
		}
	}

	// Didn't find a match, so it's an error
	if (!IsUserAWX())
		m_strDefaultLang.Empty();
	return FALSE;
}

LPCTSTR LangDlls::GetDefaultLang()
{
	ASSERT(IsUserAWX());
	if (m_strDefaultLang.IsEmpty())
		return NULL;
	else
		return m_strDefaultLang;
}


#define INIT_LANG_ARRAY_SIZE 6
void LangDlls::Init()
{
	FreeLibs();

	m_astrNameLang.RemoveAll();
	m_astrNameDll.RemoveAll();
	m_adwTranslation.RemoveAll();
	m_ahDll.RemoveAll();
	m_astrNameLang.SetSize(INIT_LANG_ARRAY_SIZE);
	m_astrNameDll.SetSize(INIT_LANG_ARRAY_SIZE);
	m_adwTranslation.SetSize(INIT_LANG_ARRAY_SIZE);
	m_ahDll.SetSize(INIT_LANG_ARRAY_SIZE);

	m_astrFiles.RemoveAll();
	m_abFilesAfter.RemoveAll();
	m_astrFiles.SetSize(INIT_LANG_ARRAY_SIZE);
	m_abFilesAfter.SetSize(INIT_LANG_ARRAY_SIZE);

	m_astrTitle.RemoveAll();
	m_astrDocFileNew.RemoveAll();
	m_astrDocFilter.RemoveAll();
	m_astrDocTag.RemoveAll();
	m_astrDocRegName.RemoveAll();
	m_astrTitle.SetSize(INIT_LANG_ARRAY_SIZE);
	m_astrDocFileNew.SetSize(INIT_LANG_ARRAY_SIZE);
	m_astrDocFilter.SetSize(INIT_LANG_ARRAY_SIZE);
	m_astrDocTag.SetSize(INIT_LANG_ARRAY_SIZE);
	m_astrDocRegName.SetSize(INIT_LANG_ARRAY_SIZE);

	m_abUpdateFilter.RemoveAll();
	m_abUpdateFileNew.RemoveAll();
	m_abUpdateRegName.RemoveAll();
	m_abUpdateFilter.SetSize(INIT_LANG_ARRAY_SIZE);
	m_abUpdateFileNew.SetSize(INIT_LANG_ARRAY_SIZE);
	m_abUpdateRegName.SetSize(INIT_LANG_ARRAY_SIZE);

	for (int i=0; i < INIT_LANG_ARRAY_SIZE; i++)
	{
		m_astrNameLang.SetAt(i, NULL);
		m_astrNameDll.SetAt(i, NULL);
		m_ahDll.SetAt(i, NULL);

		m_astrFiles.SetAt(i, NULL);
		m_abFilesAfter.SetAt(i, TRUE);

		m_astrTitle.SetAt(i, NULL);
		m_astrDocFileNew.SetAt(i, NULL);
		m_astrDocFilter.SetAt(i, NULL);
		m_astrDocTag.SetAt(i, NULL);
		m_astrDocRegName.SetAt(i, NULL);

		m_abUpdateFilter.SetAt(i, TRUE);
		m_abUpdateFileNew.SetAt(i, TRUE);
		m_abUpdateRegName.SetAt(i, TRUE);
	}
	//projOptions.m_langid = 0;
}

// Defined in customaw.cpp.
// TODO: When the appwiz*.dlls are updated, GET RID OF THIS!!!
//HRSRC FindResourceAnyType(HINSTANCE hInstance, LPCTSTR lpszResource);

HRSRC LangDlls::FindLangResource(LPCTSTR lpszResource, HINSTANCE& hinst)
{
	ASSERT (0 <= m_nDefaultDll && m_nDefaultDll < m_ahDll.GetSize());
	ASSERT (m_ahDll[m_nDefaultDll] != NULL || IsUserAWX());

	HRSRC hrsrc = NULL;

	// First try the default DLL
	if ((hrsrc = ::FindResource(hinst = m_ahDll[m_nDefaultDll], lpszResource, "TEMPLATE")) != NULL)
		// Found it
		return hrsrc;
	if ((hrsrc = ::FindResource(hinst = m_ahDll[m_nDefaultDll], lpszResource, "TEMPLATE_TAG")) != NULL)
		// Found it
		return hrsrc;

	// Hmmm.  Not there.  Well, we gotta try them all now, though the chances we'll find
	//  it are pretty slim.
	
	int nSize = GetSize();
	for (int nDll = 0; nDll < nSize && IsValidEntry(nDll); nDll++)
	{
		hrsrc = ::FindResource(hinst = m_ahDll[nDll], lpszResource, "TEMPLATE");
		if (hrsrc != NULL)
			break;		// We've found the template, so stop looking
	}

/*	if (hrsrc == NULL)
	{
		// TODO: Errorbox: possible invalid appwiz dll.
		// Backward compatibility: a langdll created in the V2 timeframe will
		//  name its help file templates with an "rft" extension.  Our new newproj.inf
		//  uses "rtf".  So, to make sure *our* newproj.inf can find *their* template,
		//  allow "rtf" in newproj.inf map to "rft" in the resources, if necessary.
		// TODO: When the appwiz*.dlls are updated, GET RID OF THIS!!!

		CString strRFTResource = lpszResource;
		if (strRFTResource.Right(4) == ".rtf")
		{
			strRFTResource = strRFTResource.Left(strRFTResource.GetLength()-3) + "rft";
			for (nDll = 0; nDll < nSize && IsValidEntry(nDll); nDll++)
			{
				hrsrc = ::FindResource(hinst = m_ahDll[nDll], lpszResource, "TEMPLATE");
				if (hrsrc != NULL)
					break;		// We've found the template, so stop looking
			}
		}
	}

	if (langDlls.m_ahDll[0] == NULL)
	{
		ASSERT(IsUserAWX());
		ReportAndThrowExt(IDP_CANT_LOAD_LOC_TPL, lpszResource);
	}
	
	// We've determined whether to look in mfcapwz.dll or appwz*.dll, so look.
	// hrsrc better not be NULL at this point, or I'm telling Mommy.
	if (hrsrc == NULL)
	{
		if (IsUserAWX())
		{
			// The extension is to blame... let's tattle
			ReportAndThrowExt(IDS_CG_CANT_FIND_TPL, lpszResource);
		}
		else
		{
			// If we're not in an extension, it must be appwz*.dll's fault
			ASSERT(nLocalized != LOC_NO);	// ASSERT that we're right
			// TODO: Fix me
			ReportAndThrow(IDP_BAD_LANG_DLL, langDlls.m_astrNameDll[0]);
		}
	}*/
	//ASSERT(hrsrc != NULL);	// TODO: Remove this when we get in a good errorbox
	return hrsrc;
}
