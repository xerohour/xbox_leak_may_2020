//
// <Scanner Classes>
// SYSINCL.dat processing and manual #include scanner
//				   

#include "stdafx.h"
#pragma hdrstop

#include <shlobj.h>
#include <profile.h>	// registry info
#include <register.h>
#include "util2.h"

#include "scanner.h"	// local header
#include "mrdependencies.h"
#include "buildengine.h"

#define	OPENFAST 	2
#define	OPENSLOW 	1
#define	NOTOPEN 	0

void StripQuotes(LPCOLESTR szFilePath, LPOLESTR szBuffer, long nMaxLen);
int GetFileFullPath(wchar_t* pstrInclude, wchar_t *pBase, IVCBuildableItem* pItem, wchar_t *buf, 
	CStringW& strOptionPath, const wchar_t* szPlatDirs, BOOL bIsIncludeNotLib, const CDirW& dirProj);

///////////////////////////////////////////////////////////////////////////////
//
// -------------------------------- CBldScannerCache --------------------------
//
///////////////////////////////////////////////////////////////////////////////

void CBldScannerCache::Add(BldFileRegHandle frh, CBldIncludeEntryList* pAddedList)
{
#ifdef _DEBUG
	void *vp;
	VSASSERT(!m_EntryMap.Lookup((void *) frh, vp), "Trying to add something to the cache that is already there");
#endif

	m_EntryMap.SetAt((void *)frh, (void *)pAddedList);

}

///////////////////////////////////////////////////////////////////////////////
CBldIncludeEntryList* CBldScannerCache::LookupFile(BldFileRegHandle frh)
{
	CBldIncludeEntryList* pList;
	if (m_EntryMap.Lookup((void *) frh, (void*&) pList))
			return pList;

	return NULL;
}

CBldScannerCache::~CBldScannerCache ()
{
	VSASSERT(m_EntryMap.IsEmpty(), "Failed to empty scanner cache before destruction.  Anything left will leak.");
	Clear();

};

int CBldScannerCache::BeginCache()
{
	if (!m_nBeginCount++)
	{
		// olympus 1277 (briancr)
		// Now clear the caches when m_nBeginCount is zero.
		// One bug that resulted from the caches *not* being cleared is this:
		// After updating dependencies and getting a missing dep error
		// (no .h file), creating the .h file, and updating deps again,
		// the error was still reported to the user.
		// After updating deps the first time (and clearing the caches in
		// EndCache), the parser reparses all files and, as a side effect,
		// fills the caches again. After creating the .h file (the parser
		// doesn't reparse), we assume that since the parser tells us (via
		// the caches) the file doesn't exist that it doesn't and report this
		// to the user. Note, that we later find that the file does exist and
		// update the Dep folder correctly (as well as subsequent dep scans).
		VSASSERT(m_EntryMap.GetStartPosition() == NULL, "Scanner cache should be empty at this point");
		Clear();
	}
	return ++m_nUniqueNo;
}

void CBldScannerCache::EndCache()
{
	// See comments in BeginCache.
	if (!--m_nBeginCount)
		Clear();
}

static void ClearMapHelper(CVCMapStringWToPtr* pmap)
{
	CStringW strTemp;
	BldFileRegHandle frh = NULL;
	VSASSERT(NULL != pmap, "Passed in NULL pointer to get the scanner cache map.  Bad programmer, bad programmer.");
	VCPOSITION pos = pmap->GetStartPosition();

	while (pos != NULL)
	{
		pmap->GetNextAssoc(pos, strTemp, (void*&)frh);
		VSASSERT(NULL != frh, "Bad file name/reg handle pair in the scanner cache");
		frh->ReleaseFRHRef();
	}
	pmap->RemoveAll();
}

void CBldScannerCache::Clear()
{
	CBldIncludeEntryList* pList;
	BldFileRegHandle frh;

	for (VCPOSITION pos = m_EntryMap.GetStartPosition (); pos != NULL; )
	{
		m_EntryMap.GetNextAssoc(pos, (void *&)frh, (void *&)pList);

		if (pList == (CBldIncludeEntryList *)-1 ) 
			continue;	// A null entry

		delete (pList);       
	}

	m_EntryMap.RemoveAll();		

	ClearMapHelper(&m_AbsCache);
	ClearMapHelper(&m_OrigCache);
	m_NotOrigCache.RemoveAll();		// No references
	ClearMapHelper(&m_ParentCache);
	m_NotParentCache.RemoveAll();	// No references
	ClearMapHelper(&m_IncludeCache);
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------- CBldSysInclReg -----------------------------
//
///////////////////////////////////////////////////////////////////////////////

BOOL CBldSysInclReg::IsSysInclude(CStringW& strFile, BOOL bUseSlowerCompare )
{
	static CStringW strSingleDir;
	static int nSingleLen = 0;

	VCPOSITION pos;

	if (m_nLoadState == NOTOPEN )
	{
		if (!DoLoad())
		{
			ATLTRACE("Loading sys include registry failed!\n");
		}

		if( m_nLoadState == OPENFAST )
		{
			// There should be only 1 entry on the list. Use the speedy algorithm.
			pos = m_dirList.GetHeadPosition();
			strSingleDir = m_dirList.GetNext(pos);
			nSingleLen = strSingleDir.GetLength();
		}
	}

	if( m_nLoadState == OPENFAST )
	{
		if( bUseSlowerCompare )
		{
			if( !wcsnicmp(strFile,strSingleDir, nSingleLen) )
				return TRUE;
		}
		else
		{
			if( !wcsncmp(strFile,strSingleDir, nSingleLen) )
				return TRUE;
		}
	}
	else // m_nLoadState == OPENSLOW
	{
		pos = m_dirList.GetHeadPosition();
		while (pos)
		{
			CStringW strDir = m_dirList.GetNext(pos);
			int nLen = strDir.GetLength();
			if (nLen == 0)
				continue;
			if( bUseSlowerCompare )
			{
				if( !wcsnicmp(strFile,strDir, nLen) )
					return TRUE;
			}
			else
			{
				if( !wcsncmp(strFile,strDir, nLen) )
					return TRUE;
			}
		}
	}

	return FALSE;
}
	
///////////////////////////////////////////////////////////////////////////////
BOOL CBldSysInclReg::DoLoad()
{
	m_dirList.RemoveAll();
	m_nLoadState = OPENFAST; // 2 - use speedy compare algorithm.
							 // 1 - use slow compare algorithm.
							 // 0 - not not yet attempted.

	// First get the default system include directory
	CStringW strVCDir;
	UtilGetVCInstallFolder(strVCDir);

	strVCDir.MakeLower();
	m_dirList.AddHead(strVCDir);

	// Now try to Load the SysIncl.dat file; this is all ANSI since a bunch of the stuff below is ANSI-only
	CPathW pt;

	// Alt reg root is usually software\microsoft\visualstudio\7.0 .... and we need to remove the "software\\" part
	CStringW strAlt = CVCProjectEngine::s_bstrAlternateRegistryRoot;
	CStringW strSoftware = strAlt.Left(8);
	if( strSoftware.CompareNoCase(L"Software") == 0 )
	{
		strAlt = strAlt.Right(strAlt.GetLength()-8);
	}

	CComBSTR bstrAppData;
	HRESULT hr = GetAppDataDirW(&bstrAppData);
	CStringW strAppDataLocal = bstrAppData;
	strAppDataLocal += strAlt;
	strAppDataLocal += L"\\SysIncl.dat";
	
	if ( !pt.Create(strAppDataLocal) )
	{
		return FALSE;
	}

	// Find the file on disk and open it.
	FILE *pFile;
	if ((pFile = fopenW((const wchar_t *)pt, L"rt")) == NULL )
	{
		// Failed probably because dir didn't exist.
		CDirW  dr;
		dr.CreateFromPath(pt);
		dr.CreateOnDisk();

		if ((pFile = fopenW((const wchar_t *)pt, L"w")) != NULL)	// create it for next time
				fclose(pFile);

		return FALSE;  
	}

	// We've got the file open, now read it line by line
	BOOL bLeadByte = FALSE;
	char buf[MAX_PATH], *pb;
	while (1)
	{
    	// yes, ANSI -- this should be re-done
		// Things to worry about are the codepage of the file
		// and we should do the whole conversion at once, and then parse the UNICODE
		pb = buf;
		//	Read in a line up to, but not including the return:
		while (1)
		{
			int i;
			i = fgetc(pFile);	
			if (i == EOF || pb >= buf + MAX_PATH - 1 || (i == '\n' && !bLeadByte)) 
				break;
			*pb++ = (char)i;
			bLeadByte = bLeadByte ? FALSE : IsDBCSLeadByte((unsigned char)i);
		}
		*pb = '\0';
		if (ferror(pFile))
			break;
		// end of yes, ANSI section

		// clean up the string
		CStringW strName = buf;
		strName.MakeUpper();
		strName.TrimLeft();
		if (strName.IsEmpty())
		{
			if(feof(pFile))
				break;
			else
				continue;
		}

		// skip the comments  "#"
		if (strName[0] != L'#')
		{
			strName.TrimRight();
			CStringW strBuf = strName;
			strName.Empty();
			StripQuotes(strBuf, strName.GetBuffer(4096), 4096);
			strName.ReleaseBuffer();
			strName.MakeLower();

			// add it to our list of paths to return.
			// TODO: perf, if it is a sub dir of something already on the list, remove it
			m_dirList.AddHead((LPCOLESTR)strName);
			m_nLoadState = OPENSLOW; // use slower compare algorithm.
		}
	}

	int nVal1 = ferror(pFile);
	int nVal2 = fclose(pFile);
	if (nVal1 || nVal2)
		return FALSE;

	return TRUE;
}	

CBldSysInclReg g_SysInclReg;

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------- CBldIncludeEntry ----------------------------
//
///////////////////////////////////////////////////////////////////////////////

BOOL CBldIncludeEntry::CheckAbsolute()
{
//	See if this is a fully qualified path name.  We use the compiler's method
//	here.

	const wchar_t *pchr = m_FileReference;
	if (*pchr == L'/' || *pchr == L'\\' || *pchr == L':' || *(pchr + 1) == L':')
	{
		VSASSERT(*pchr != L':', "We managed to get a two character drive letter...");  // should never happen!!!
		m_EntryType = IncTypeCheckAbsolute;
		return TRUE;
	}

	return FALSE;
}	
///////////////////////////////////////////////////////////////////////////////
BOOL CBldIncludeEntry::FindFile(const wchar_t* lpszIncludePath, const CDirW& cdBaseDirectory, 
	const wchar_t* pchProjDirectory, BldFileRegHandle& frhResult, IVCBuildableItem* pItem, CBldScannerCache* pScannerCache,
	const wchar_t* szPlatDirs /* = NULL */)
{
//
//	Try to find our file based on m_Entry type.  Note that order is important here.
//
//	See CBldScannerCache in PROJTOOL.H for a description of how the caches work.

	VSASSERT(m_EntryType & (IncTypeCheckAbsolute | IncTypeCheckParentDir | IncTypeCheckIncludePath | IncTypeCheckLibPath), "Invalid entry type");

	BOOL fIsInclude = !(m_EntryType & IncTypeCheckLibPath);
	void* pvCache;
	CPathW Path;
	wchar_t buf[MAX_PATH];
	int iFound;
	CStringW strOptionPath, strRelative;
	CComPtr<VCConfiguration> spProjCfg;

	// Nullify the return param.
	frhResult = (BldFileRegHandle)NULL;

	CDirW dirProj;
	if (pchProjDirectory)
		dirProj.CreateFromKnown(pchProjDirectory);
	else
		dirProj.CreateFromCurrent();

	// Absolute means file name is right or else
	if (m_EntryType & IncTypeCheckAbsolute)
	{
		if (pScannerCache && pScannerCache->m_AbsCache.Lookup((const wchar_t *)m_FileReference, pvCache))
		{
			frhResult = (BldFileRegHandle)pvCache;
				// frhResult reference
			frhResult->AddFRHRef();
			return TRUE;
		}
		else
		{
			if (GetFileAttributesW((wchar_t *)(const wchar_t *)m_FileReference) != 0xffffffff)
			{
				if (!Path.Create(m_FileReference))
					return FALSE;

				// frhResult reference
				frhResult = CBldFileRegFile::GetFileHandle(Path, TRUE);

				if (pScannerCache)
					pScannerCache->m_AbsCache.SetAt((const wchar_t *)m_FileReference, (void *)frhResult);
				// references in m_AbsCache.
				frhResult->AddFRHRef();
				return TRUE;
			}
		}
	}

	BOOL bGotIt;
	if (m_EntryType & IncTypeCheckOriginalDir)
	{
		bGotIt = ResolveToCache(m_OriginalDir, pScannerCache ? &(pScannerCache->m_OrigCache) : NULL, 
			pScannerCache ? &(pScannerCache->m_NotOrigCache) : NULL, pchProjDirectory, fIsInclude, pItem, buf, frhResult, 
			lpszIncludePath, szPlatDirs, false);
		if (bGotIt)
			return TRUE;
	}	

	if (m_EntryType & IncTypeCheckParentDir)
	{
		bGotIt = ResolveToCache(cdBaseDirectory, pScannerCache ? &(pScannerCache->m_ParentCache) : NULL, 
			pScannerCache ? &(pScannerCache->m_NotParentCache) : NULL, pchProjDirectory, fIsInclude, pItem, buf, frhResult, 
			lpszIncludePath, szPlatDirs, true);
		if (bGotIt)
			return TRUE;
	}	

	if (m_EntryType & IncTypeCheckIncludePath || m_EntryType & IncTypeCheckLibPath)
	{
		CStringW	strLookup;
		wchar_t*	pchLookup = strLookup.GetBuffer(lstrlenW(lpszIncludePath) + m_FileReference.GetLength() + 2);

		wcscpy(pchLookup, lpszIncludePath);
		wcscat(pchLookup, L"#");
		wcscat(pchLookup, (const wchar_t *)m_FileReference);

		if (pScannerCache && pScannerCache->m_IncludeCache.Lookup(pchLookup, pvCache))
		{
			frhResult = (BldFileRegHandle)pvCache;
			frhResult->AddFRHRef();
			return TRUE;
		}
		else
		{
			if ((iFound=GetFileFullPath((wchar_t *)lpszIncludePath, (wchar_t *)(LPCOLESTR)m_FileReference, 
				pItem, buf, strOptionPath, szPlatDirs, fIsInclude, dirProj)) != NOTFOUND)
			{
				BOOL fRet = FALSE;
				if (Path.Create(buf))
				{
					frhResult = CBldFileRegFile::GetFileHandle(Path, TRUE);
					CBldFileRegFile *pFile = (CBldFileRegFile*)g_FileRegistry.GetRegEntry(frhResult);

					if (pScannerCache)
						pScannerCache->m_IncludeCache.SetAt(pchLookup, (void *)frhResult);
					// references in m_IncludeCache.
					frhResult->AddFRHRef();
					if (iFound == ONPATH)
					{
						pFile->SetFileFoundInIncPath();
						if (m_FileReference.Find(L"..\\") != -1)
						{
							pFile->SetOrgName(m_FileReference);
							pFile->SetIndexNameRelative(0);
						}
						else
							pFile->SetIndexNameRelative((int)m_FileReference.GetLength());
					}
					else if (!pFile->IsFileFoundInIncPath())
					{
						// found on option path, should remember the option path
						// reset the m_OrgName to append this option path
						pFile->SetOrgName(strOptionPath);
						pFile->SetIndexNameRelative(0);
						pFile->SetFileNameRelativeToProj();
						const wchar_t *pchr = strOptionPath;
						if (*pchr == L'/' || *pchr == L'\\' || *pchr == L':' || *(pchr + 1) == L':')
						{
							VSASSERT(*pchr != L':', "Managed to get a two char drive letter...");  // should never happen!!!
							CDirW dr;
							dr.CreateFromKnown(pchProjDirectory);
							Path.GetRelativeName(dr, strRelative);

							if (strRelative.GetLength() < (strOptionPath.GetLength()-2))
							{
								// get relative path
								wchar_t *pch = (wchar_t *)(LPCOLESTR)strRelative;
								if (pch && *pch != L'.')
								{
									CStringW strTmp;
									if (*pch == L'\\')
										strTmp = L"." + strRelative;
									else if (*pch != L':')
										strTmp = L".\\" + strRelative;
									else
										VSASSERT(0, "Invalid case in building relative path name");	// Speed bug #3838
									strRelative = strTmp;
								}
								pFile->SetOrgName(strRelative);
							}
							pFile->SetOrgName(strRelative);
						}
					}
					fRet = TRUE;
				}
				return fRet;
			}
		}
	}

	// couldn't find the file so return our best guess
	BOOL fGotUnknown = FALSE, fMatchCase = FALSE;
	if (pItem != (IVCBuildableItem *)NULL &&
		(m_EntryType & IncTypeCheckIntDir) || (m_EntryType & IncTypeCheckOutDir))
	{
		CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
		if (spPropContainer != NULL)
		{
			// get the output directory
			CComBSTR bstrDirOut;
			HRESULT hr = S_OK;
			if (m_EntryType & IncTypeCheckIntDir)
				hr = spPropContainer->GetEvaluatedStrProperty(VCCFGID_IntermediateDirectory, &bstrDirOut);
			else
				hr = spPropContainer->GetEvaluatedStrProperty(VCCFGID_OutputDirectory, &bstrDirOut);
			if (SUCCEEDED(hr))
			{
				CCurDirW cwd(pchProjDirectory);	// just in case the output dir is relative...
				CStringW strdirOut;
				strdirOut = bstrDirOut;
				// is this empty or a directory?
				if (!strdirOut.IsEmpty())
				{
					const wchar_t* pchStart = strdirOut;
					const wchar_t* pchT;

					pchT = pchStart + wcslen(pchStart);	// point to nul terminator
					pchT = _wcsdec(pchStart, pchT);		// back up one char

					if (*pchT != L'\\' && *pchT != L'/')
					{
						strdirOut.ReleaseBuffer();
						strdirOut += L'/';
					}

					CDirW dirOut;
					if (dirOut.CreateFromString(strdirOut))
					{
						if (Path.CreateFromDirAndFilename(dirOut, m_FileReference))
						{
							fMatchCase = TRUE;
							fGotUnknown = TRUE;
						}
					}
				}
			}
		}	
	}

	if (!fGotUnknown && (m_EntryType & IncTypeCheckAbsolute))
	{
		if (Path.Create(m_FileReference))
		{
			fMatchCase = TRUE;
			fGotUnknown = TRUE;
		}
	}

	if (!fGotUnknown && (m_EntryType & IncTypeCheckOriginalDir))
	{
		if (Path.CreateFromDirAndFilename(m_OriginalDir, m_FileReference))
		{
			fMatchCase = TRUE;
			fGotUnknown = TRUE;
		}
	}	
	
	if (!fGotUnknown && (m_EntryType & IncTypeCheckParentDir))
	{
		if (Path.CreateFromDirAndFilename(cdBaseDirectory, m_FileReference))
		{
			fMatchCase = FALSE;
			fGotUnknown = TRUE;
		}
	}	

	if (fGotUnknown)
	{
		frhResult = CBldFileRegFile::GetFileHandle(Path, TRUE);
	}

	// shouldn't be scanned!
	m_bShouldBeScanned = FALSE;
	return FALSE;
}  

BOOL CBldIncludeEntry::ResolveToCache(const CDirW& baseDir, CVCMapStringWToPtr* pFoundCache, CVCMapStringWToPtr* pNotFoundCache, 
	const wchar_t* pchProjDirectory, BOOL fIsInclude, IVCBuildableItem* pItem, wchar_t* buf, BldFileRegHandle& frhResult, 
	const wchar_t* lpszIncludePath, const wchar_t* szPlatDirs, bool bRelativeToParent)
{
	CStringW	strLookup;
	wchar_t*	pchLookup = strLookup.GetBuffer(lstrlenW((const wchar_t *)baseDir) + m_FileReference.GetLength() + 2);
	wchar_t *pch;
	int ich, iFound;
	void* pvCache;
	CPathW Path;
	CStringW strOptionPath;

	wcscpy(pchLookup, (const wchar_t *)baseDir);
	wcscat(pchLookup, L"#");
	wcscat(pchLookup, (const wchar_t *)m_FileReference);

	if (pFoundCache && pFoundCache->Lookup(pchLookup, pvCache))
	{
		frhResult = (BldFileRegHandle)pvCache;
		frhResult->AddFRHRef();
		return TRUE;
	}
	else if (!pNotFoundCache || !pNotFoundCache->Lookup(pchLookup, pvCache))
	{
		if (!Path.CreateFromDirAndFilename(baseDir, m_FileReference))
			return FALSE;

		if (GetFileAttributesW((wchar_t *)(const wchar_t *) Path) != 0xffffffff)
		{
			frhResult = CBldFileRegFile::GetFileHandle(Path, TRUE);
			CBldFileRegFile *pFile = (CBldFileRegFile*)g_FileRegistry.GetRegEntry(frhResult);
			if (pFoundCache)
				pFoundCache->SetAt(pchLookup, (void *)frhResult);
			// references in foundCache.
			frhResult->AddFRHRef();
			Path.SetAlwaysRelative();     //like this to be relative to proj dir
			//we don't want to resolve relative path(with ..\) unless we have to
			//first, we should make sure to make everything under proj dir is proj dir relative 
			//if orignial dir is sub dir of the proj dir, convert the include name relative to 
			//proj dir, which means adding the sub dir for the include name
			//1) if the path is relative to proj dir, keep it that way
			//2) if the path is relative to include path, keep it that way
			//3) if we can resolve it on the include path, resolve it and keep the relative part
			//4) hey, it is illegal code if user wants a sharable makefile
			pch = (wchar_t *)Path.GetFullPath();
			if (m_FileReference.Find(L"..\\") != -1)
			{	
				//make sub trees relative to proj dir.
				CDirW dirProj;
				if (pchProjDirectory)
					dirProj.CreateFromKnown(pchProjDirectory);
				else
					dirProj.CreateFromCurrent();

				if (!bRelativeToParent && 
					((wcslen((wchar_t *)(const wchar_t *)baseDir) > wcslen(pchProjDirectory)) &&
					wcsnicmp((wchar_t *)(const wchar_t *)baseDir, pchProjDirectory, wcslen(pchProjDirectory))
					== 0))
				{
					CStringW strTmp;
					wchar_t* pchTmp = (wchar_t *)(const wchar_t *)baseDir;

					pchTmp += wcslen(pchProjDirectory)+1;
					strTmp = pchTmp;
					strTmp = strTmp + L"\\" + m_FileReference;

					if (pchProjDirectory && (GetFileFullPath((wchar_t *)pchProjDirectory, (wchar_t *)(LPCOLESTR)strTmp, pItem, buf, 
						strOptionPath, szPlatDirs, fIsInclude, dirProj) != NOTFOUND))
					{
						//yes, make the original name relative to proj
						pFile->SetOrgName(strTmp);
						pFile->SetFileNameRelativeToProj();
						pFile->SetIndexNameRelative(0);
					}
				}
				else if (pchProjDirectory && (GetFileFullPath((wchar_t *)pchProjDirectory, (wchar_t *)(LPCOLESTR)m_FileReference, 
					pItem, buf, strOptionPath, szPlatDirs, fIsInclude, dirProj) != NOTFOUND))
				{
					//first, can we make this relative to the project?
					//yes, let's keep the original name
					pFile->SetOrgName(m_FileReference);
					pFile->SetFileNameRelativeToProj();
					pFile->SetIndexNameRelative(0);
				}
				else
				{
					//we can't make it relative to proj, any luck with the path?
					if ((iFound = GetFileFullPath((wchar_t *)lpszIncludePath, (wchar_t *)(LPCOLESTR)m_FileReference, 
						pItem, buf, strOptionPath, szPlatDirs, fIsInclude, dirProj)) != NOTFOUND)
					{
						CBldFileRegFile* pFile2 = (bRelativeToParent) ? 
							(CBldFileRegFile*)(g_FileRegistry.GetRegEntry(frhResult)) : pFile;
						SetFilePathInfo(pFile, pFile2, 0, strOptionPath, pchProjDirectory, iFound, true);
					}
					else
					{
						//otherwise, see if resoved full path will be relative to the include path
						if ((iFound = SetFileRelativeFromPath((wchar_t *)lpszIncludePath, pch, frhResult, pItem, 
							&ich, strOptionPath, szPlatDirs, fIsInclude)) != NOTFOUND)
						{
							SetFilePathInfo(pFile, pFile, ich, strOptionPath, pchProjDirectory, iFound, false);
						}
						else
						{
							//stuck with the full path, live with it
						}
					}
				}
			}
			else
			{
				if (!bRelativeToParent)
				{
					//always trying to see if we can be relative to the include path
					if (_wcsicmp((wchar_t *)(const wchar_t *)baseDir, (wchar_t *)(const wchar_t *)m_OriginalDir) != 0 
						&& ((iFound = SetFileRelativeFromPath((wchar_t *)lpszIncludePath, pch, frhResult, pItem, &ich, 
						strOptionPath, szPlatDirs, fIsInclude)) != NOTFOUND))
					{
						SetFilePathInfo(pFile, pFile, ich, strOptionPath, pchProjDirectory, iFound, false);
					}
				}
			}
			return TRUE;
		}
		else if (pNotFoundCache)
			pNotFoundCache->SetAt(pchLookup, 0);
	}

	return FALSE;
}

void CBldIncludeEntry::SetFilePathInfo(CBldFileRegFile* pFile1, CBldFileRegFile* pFile2, int idx, CStringW& strOptionPath, 
	const wchar_t* pchProjDirectory, int iFound, bool bSetOrgNameOnPath)
{
	CStringW strRelative;
	if (iFound == ONPATH)
	{
		//set it relative to the path, the ..\ won't be resolved
		if (bSetOrgNameOnPath)
			pFile1->SetOrgName(m_FileReference);
		pFile1->SetIndexNameRelative(idx);
		pFile2->SetFileFoundInIncPath();
	}
	else
	{
		// found on option path, should remember the option path
		// reset the m_OrgName to append this option path
		pFile1->SetOrgName(strOptionPath);
		pFile1->SetIndexNameRelative(0);
		pFile1->SetFileNameRelativeToProj();
		const wchar_t *pchr = strOptionPath;
		if (*pchr == L'/' || *pchr == L'\\' || *pchr == L':' || *(pchr + 1) == L':')
		{
   			VSASSERT(*pchr != L':', "Managed to get a two char drive letter...");  // should never happen!!!
			strRelative = GetRelativeName(strOptionPath, (wchar_t *)pchProjDirectory);
			pFile1->SetOrgName(strRelative);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//
// ----------------------------- CBldIncludeEntryList --------------------------
//
///////////////////////////////////////////////////////////////////////////////

void CBldIncludeEntryList::DeleteContents()
{
	VCPOSITION pos;
	for (pos = GetHeadPosition (); pos != NULL; )
		delete(GetNext(pos));
	RemoveAll();
}

static int GetFileFullPathI(wchar_t* pszIncludePathLocal, wchar_t *pBase, const wchar_t* pszIncludePathSys, wchar_t *buf, 
	CStringW &strOptionPath, CBldFileRegistry* pregistry, CDirW& dirBase, const CDirW& dirProj);

// Helper function for finding include file's full path
// replace SearchPath
int GetFileFullPath(wchar_t* pstrInclude, wchar_t *pBase, IVCBuildableItem* pItem, wchar_t *buf, CStringW &strOptionPath, 
	const wchar_t* szPlatDirs, BOOL bIsInclude, const CDirW& dirProj)
{
	VSASSERT(buf != (wchar_t *)NULL, "Trying to use a NULL buffer to return data in.  Bad programmer, bad programmer.");
	CDirW dirBase;
	CBldFileRegistry* pregistry = NULL;

	if (pItem != NULL)
	{
		// the configuration of our file search
		pItem->get_Registry((void **)&pregistry);
		VSASSERT(pregistry != (CBldFileRegistry *)NULL, "No registry associated with item.  Bad initialization.");
	}

	CStringW strIncludeTmp;
	wchar_t* szIncludeTmp = strIncludeTmp.GetBuffer(lstrlenW(pstrInclude) + 1);
	wcscpy(szIncludeTmp, pstrInclude);

	// path of a possible resolved file
	CStringW str1 = szPlatDirs;

	if (pItem != NULL && szPlatDirs == NULL)
	{
		CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
		if (spPropContainer != NULL)
		{
			CComBSTR bstr1;
			spPropContainer->GetEvaluatedStrProperty(bIsInclude ? VCPLATID_IncludeDirectories : VCPLATID_LibraryDirectories, &bstr1);
			str1 = bstr1;
		}
	}

	// get the include path from env setting of $(INCLUDE)
	const wchar_t* pIncStart = wcsstr(szIncludeTmp, (LPCOLESTR)str1);
	int nFound = GetFileFullPathI(szIncludeTmp, pBase, pIncStart, buf, strOptionPath, pregistry, dirBase,
		dirProj);
	if (nFound == NOTFOUND)
		nFound = GetFileFullPathI(str1.GetBuffer(str1.GetLength()), pBase, str1, buf, strOptionPath, pregistry, 
			dirBase, dirProj);
	return nFound;
}

static int GetFileFullPathI(wchar_t* pszIncludePathLocal, wchar_t *pBase, const wchar_t* pszIncludePathSys, wchar_t *buf, 
	CStringW &strOptionPath, CBldFileRegistry* pregistry, CDirW& dirBase, const CDirW& dirProj)
{
	// separate the include path list
	const wchar_t *pch = wcstok(pszIncludePathLocal, L";,");
	CPathW path;

	while (pch != NULL)
	{
		// remove leading spaces
		while (pch && *pch == L' ')
			pch++;
		if (pch && *pch == L'"')
			pch++;

		INT_PTR nLen = wcslen(pch);
		if (nLen <= 0)
			break;

		// copy our include path
		wcscpy(buf, pch);

		// remove ending spaces
		while (*_wcsdec(buf, buf + nLen) == L' ')
		{
			nLen--;
			if (nLen <= 0)
				break;
		}

		wchar_t chTerm = *_wcsdec(buf, buf + nLen);
		if (chTerm == L'"')	// quoted string, get rid of quote
		{
			nLen--;
			if (nLen <= 0)
				break;
			chTerm = *_wcsdec(buf, buf + nLen);
			// remove ending spaces
			while (*_wcsdec(buf, buf + nLen) == L' ')
			{
				nLen--;
				if (nLen <= 0)
					break;
			}
		}
		while (nLen > 0 && (chTerm == L'/' || chTerm == L'\\'))		// don't want any trailing slashes
		{
			nLen--;
			chTerm = *_wcsdec(buf, buf + nLen);
		}
		if (nLen <= 0)
			break;

		buf[nLen] = L'\0';
		VSASSERT((nLen + wcslen(pBase)) <= MAX_PATH, "Directory path longer than MAX_PATH");
		//wcscpy(buf + nLen, pBase);

		CPathW pathBase;
		if (pathBase.CreateFromDirAndRelative(dirProj, buf))
			dirBase.CreateFromPath(pathBase, TRUE /* known directory */);
		else
			return NOTFOUND;

		if (path.CreateFromDirAndFilename(dirBase, pBase))
		{
			// exist on disk as is?
			if (path.ExistsOnDisk())
			{
				if (pch < pszIncludePathSys)
				{
					strOptionPath = buf;
					strOptionPath += pBase;
				}
				wcscpy(buf, path.GetFullPath());
				return ((pch >= pszIncludePathSys) ? ONPATH : ONOPTION);
			}

			// exist as a file (not yet created perhaps)
			// in our build registry (i.e. build graph)?
			BldFileRegHandle frhTmp = NULL; 
			if ((pregistry != NULL) && ((frhTmp = pregistry->LookupFile((LPCOLESTR)path)) != (BldFileRegHandle)NULL))
			{
				frhTmp->ReleaseFRHRef();
				if (pch < pszIncludePathSys)
					strOptionPath = path.GetFullPath();
				wcscpy(buf, path.GetFullPath() );
				return ((pch >= pszIncludePathSys) ? ONPATH : ONOPTION);
			}
		}

		// next include path
		pch = wcstok(NULL, L";,");
	}

	return NOTFOUND;
}

void StripQuotes(LPCOLESTR szFilePath, LPOLESTR szBuffer, long nMax)
{
	if (szFilePath == NULL || szBuffer == NULL)
	{
		VSASSERT(FALSE, "Cannot strip quotes off a NULL path!");
		return;		// nothing we can do
	}

	const wchar_t * pchLookup = szFilePath;
	wchar_t *pchSet = szBuffer;
	long nIdx = 0;

	while (*pchLookup != L'\0')
	{
		if (*pchLookup != L'"')
 		{
			wcscpy(pchSet, pchLookup);
			pchSet++;
			nIdx++;
			if (nIdx == nMax-1)
			{
				VSASSERT(FALSE, "Buffer too small!");
				break;	// overflow...
			}
 		}
		pchLookup++;
	}

    *pchSet = L'\0';
}

// Help routine to find out if the file is on the search path
int SetFileRelativeFromPath(wchar_t* pstrInclude, wchar_t* pPath, BldFileRegHandle frh, IVCBuildableItem* pItem, 
	int *pich, CStringW &strOptionPath, const wchar_t* szPlatDirs, BOOL bIsInclude)
{
	CStringW strIncludeTmp = pstrInclude;
	strIncludeTmp.MakeUpper();

	CStringW strPath = pPath;
	strPath.MakeUpper();

	const int nPathLen = strPath.GetLength();
	int nPchLen;
	BOOL bHasSlash;

	CStringW str1 = szPlatDirs;

	if (pItem != NULL && szPlatDirs == NULL)
	{
		CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
		if (spPropContainer != NULL)
		{
			CComBSTR bstr;
			spPropContainer->GetEvaluatedStrProperty(bIsInclude ? VCPLATID_IncludeDirectories : VCPLATID_LibraryDirectories, &bstr);
			str1 = bstr;
		}
	}

	wchar_t *pch, *pIncStart = wcsstr(strIncludeTmp, (LPCOLESTR)str1);

	//separate the include path
	pch = wcstok((wchar_t*)(const wchar_t*)strIncludeTmp, L";,");
	for ( ; pch != NULL; pch = wcstok(NULL, L";,"))
	{
		if (pch[0] != L'\0')
		{
			// do we have a match?
			nPchLen = lstrlenW(pch);

			// no possible match if path is shorter than dir
			if ((nPathLen < nPchLen) || (wcsncmp(strPath, pch, nPchLen)!=0))
				continue;

			bHasSlash = (*_wcsdec(pch, pch + nPchLen)==L'\\');

			if ((bHasSlash) || (strPath[nPchLen]==L'\\'))
			{
				// yes, it is on the search path,
				if (pch >= pIncStart)
				{
					// get the relative name and length
					CStringW str = GetRelativeName(pPath, pch);

					*pich = (int) str.GetLength();

					return ONPATH;
				}
				else
				{
					strOptionPath = pPath;
					return ONOPTION;
				}
			}
		}

	}

	// not found, zero-length
	*pich = 0;
	return NOTFOUND;

}
