//
// <Scanner Classes>
//
// Scanning of source files.
//
// [???]
//				   

#include "stdafx.h"
#pragma hdrstop

#include "scanner.h"	// local header
#include "malloc.h"		// for _alloca
#include "mrdepend.h"

IMPLEMENT_DYNAMIC (CIncludeEntryList, CObList)
IMPLEMENT_DYNAMIC (CIncludeEntry, CObject)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static TCHAR * rgSubDir [] = { 
								"SYS\\",
								"RES\\",
								"MACOS\\",
								"MRC\\"
							 };
#define MAX_SUBDIR 4
///////////////////////////////////////////////////////////////////////////////
//
// -------------------------------- CScannerCache -----------------------------
//
///////////////////////////////////////////////////////////////////////////////
void CScannerCache::Add (
			FileRegHandle frh,
			CIncludeEntryList 	*pAddedList
			)
{
	#ifdef _DEBUG
	void *vp;
	ASSERT ( !m_EntryMap.Lookup ( (void *) frh, vp ));
	#endif

#ifndef REFCOUNT_WORK
	g_FileRegistry.AddRegRef(frh);
#endif
	m_EntryMap.SetAt ( (void *) frh, (void *) pAddedList );

}
///////////////////////////////////////////////////////////////////////////////
CIncludeEntryList *CScannerCache::LookupFile (FileRegHandle frh)
{
	CIncludeEntryList *pList;
	if (m_EntryMap.Lookup ( (void *) frh, (void*&) pList))
			return pList;
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
//#ifdef _DEBUG
CScannerCache::~CScannerCache ()
{
	ASSERT(m_EntryMap.IsEmpty());
	Clear();

};
//#endif


int CScannerCache::BeginCache( )
{
	// Currently the only CScannerCache object we have is the global one.
	// It is a BAD idea to manipulate this global cache, from anything 
	// but the main thread. If you want to use the cache in other threads,
	// store the identity of the thread in the object and do something like
	// ASSERT(AfxGetThread( ) == m_pMyThread);
 
	ASSERT(this == &g_ScannerCache);
	ASSERT(AfxGetThread() == AfxGetApp());
	
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
		ASSERT(m_EntryMap.GetStartPosition() == NULL);
		Clear();
	}
	return ++m_nUniqueNo;
}

void CScannerCache::EndCache( )
{
	// See comments in BeginCache.
	ASSERT(this == &g_ScannerCache);
	ASSERT(AfxGetThread() == AfxGetApp());

	if (!--m_nBeginCount)
		Clear();
}

#ifdef REFCOUNT_WORK
static void ClearMapHelper( CMapStringToPtr* pmap )
{
	CString strTemp;
	FileRegHandle frh = NULL;
	ASSERT(NULL!=pmap);
	POSITION pos = pmap->GetStartPosition();

	while (pos!=NULL)
	{
		pmap->GetNextAssoc(pos, strTemp, (void*&)frh );
		ASSERT(NULL!=frh);
		frh->ReleaseFRHRef();
	}
	pmap->RemoveAll();
}
#endif

void CScannerCache::Clear ()
{
	POSITION pos;
	CIncludeEntryList *pList;
	FileRegHandle frh;

	PSCAN_TRACE ("CScannerCache clearing with %i entries.\n",
							m_EntryMap.GetCount());
	
	for (pos = m_EntryMap.GetStartPosition (); pos != NULL; )
	{
		m_EntryMap.GetNextAssoc ( pos, (void *&) frh, (void *&) pList );

		if (pList == (CIncludeEntryList *)-1 ) continue;	// A null entry

		ASSERT (pList->IsKindOf ( RUNTIME_CLASS (CIncludeEntryList )));

		delete (pList);       
#ifndef REFCOUNT_WORK
		g_FileRegistry.ReleaseRegRef(frh);
#endif
	}

	m_EntryMap.RemoveAll ();		

#ifndef REFCOUNT_WORK
	m_AbsCache.RemoveAll();
	m_OrigCache.RemoveAll();
	m_NotOrigCache.RemoveAll();
	m_ParentCache.RemoveAll();
	m_NotParentCache.RemoveAll();
	m_IncludeCache.RemoveAll();
#else
	ClearMapHelper( &m_AbsCache );
	ClearMapHelper( &m_OrigCache );
	m_NotOrigCache.RemoveAll();		// No references
	ClearMapHelper( &m_ParentCache );
	m_NotParentCache.RemoveAll();	// No references
	ClearMapHelper( &m_IncludeCache );
#endif
}

BOOL CScannerCache::c_bScanning;
BOOL CScannerCache::UpdateDependencies( CSlob *pItem, BOOL bUseAnyMethod /* = FALSE */, 
	BOOL bWriteOutput /* = TRUE */ )
{
	// Should not reach this code when using new model.
	// ASSERT(!g_bNewUpdateDepModel);
	// Except during conversion of old makefiles!

	CWaitCursor waitCur;
	UINT nTotalFiles, nDone;
	BOOL rc = TRUE;

	//
	// Cache IMreDependencies* if pItem is a CProjContainer
	//
	IMreDependencies* pMreDepend = NULL;
	if (pItem->IsKindOf(RUNTIME_CLASS(CProjContainer)))
	{
		CProject* pProject = ((CProjItem*)pItem)->GetProject();

		ASSERT(NULL!=pProject);

		// Get IMreDependencies* so that it is cached.
		CPath pathIdb = pProject->GetMrePath(pProject->GetActiveConfig());
		if (!pathIdb.IsEmpty())
		{
			// Minimal rebuild dependency interface.
			pMreDepend = IMreDependencies::GetMreDependenciesIFace((LPCTSTR)pathIdb);
		}
	}


	ASSERT( !c_bScanning );
	c_bScanning = TRUE;
	g_ScannerCache.BeginCache();

	// NOTE: This seemingly useless line saves the day!  If this isn't called, and
	// you've used <Esc> to cancel a dialog, then GetFileCount() will abort immediately.
	GetAsyncKeyState(VK_ESCAPE);

	nTotalFiles = GetFileCount( pItem );
	if (nTotalFiles)
	{
		CString strStatus;
		VERIFY( strStatus.LoadString( IDS_UPDATING_DEPENDENCIES ) );
		StatusBeginPercentDone( strStatus );

		BOOL bQueryUser;
		if (g_Spawner.IsSaveBeforeRunningTools(&bQueryUser))
		{
			if (!theApp.SaveAll(bQueryUser, FILTER_DEBUG | FILTER_PROJECT | FILTER_NEW_FILE))
				rc = FALSE;
		}

		if (rc)
		{
			if ((bWriteOutput) && (!theApp.m_bInvokedCommandLine))
			{
				g_VPROJIdeInterface.GetOutputWindow()->ClearOutputWindow();
			}

			nDone = 0;
			rc = ScanFiles( pItem, nTotalFiles, nDone, bUseAnyMethod, bWriteOutput );

			if (rc)
				rc = RefreshDependencyContainers();
		}

		StatusEndPercentDone();

		VERIFY( strStatus.LoadString( IDS_UPDATING_DEPENDENCIES_OWIN ) );

		if (bWriteOutput)
		{
			if (theApp.m_bInvokedCommandLine)
			{
				theApp.WriteLog(strStatus, TRUE);
			}
			else
			{
				g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow( strStatus, FALSE, TRUE );
			}
		}
	}
	else
	{
		rc = FALSE;
	}

	g_ScannerCache.EndCache();
	m_DirtyGroups.RemoveAll();
	
	c_bScanning = FALSE;

	if (NULL!=pMreDepend)
		pMreDepend->Release();

	return rc;
}

UINT CScannerCache::GetFileCount( CSlob *pItem )
{
	UINT nTotalFiles = 0;

	if (GetAsyncKeyState(VK_ESCAPE))
		return 0;

	ASSERT_VALID( pItem );
	if (pItem->IsKindOf( RUNTIME_CLASS(CFileItem) ))
	{
		nTotalFiles++;
	}

	CObList *pSubList = pItem->GetContentList();
	if (pSubList)
	{
		if (!pItem->IsKindOf( RUNTIME_CLASS(CDependencyContainer) ))
		{
			POSITION pos = pSubList->GetHeadPosition();
			while (pos)
			{
				CProjItem *pTempItem = (CProjItem *) pSubList->GetNext( pos );
				ASSERT( pTempItem->IsKindOf( RUNTIME_CLASS(CProjItem) ) );
				ASSERT_VALID( pTempItem );

				nTotalFiles += GetFileCount( pTempItem );
			}
		}
	}

	return nTotalFiles;
}

BOOL CScannerCache::ScanFiles( CSlob *pItem, UINT nTotalFiles, UINT &nDone, BOOL bUseAnyMethod /* = FALSE */,
	BOOL bWriteOutput /* = TRUE */ )
{
	if (bWriteOutput && GetAsyncKeyState(VK_ESCAPE))
		return FALSE;

	ASSERT_VALID( pItem );
	if (pItem->IsKindOf( RUNTIME_CLASS(CFileItem) ))
	{
		nDone++;
		((CFileItem *)pItem)->ScanDependencies(bUseAnyMethod,bWriteOutput);

		CProjContainer *pContainer = (CProjContainer *)pItem->GetContainer();
		ASSERT( pContainer->IsKindOf( RUNTIME_CLASS(CTargetItem)) ||
			pContainer->IsKindOf( RUNTIME_CLASS(CProjGroup)) );
		m_DirtyGroups.SetAt( pContainer, NULL );
	}

	CObList *pSubList = pItem->GetContentList();
	if (pSubList)
	{
		if (!pItem->IsKindOf( RUNTIME_CLASS(CDependencyContainer) ))
		{
			POSITION pos = pSubList->GetHeadPosition();
			while (pos)
			{
				CProjItem *pTempItem = (CProjItem *) pSubList->GetNext( pos );
				ASSERT( pTempItem->IsKindOf( RUNTIME_CLASS(CProjItem) ) );
				ASSERT_VALID( pTempItem );

				if (!ScanFiles( pTempItem, nTotalFiles, nDone, bUseAnyMethod, bWriteOutput ))
					return FALSE;
			}
		}
	}

	StatusPercentDone( nDone * 100 / nTotalFiles );
	return TRUE;
}

BOOL CScannerCache::RefreshDependencyContainers()
{
	// This calls CProjContainer::RefreshDependencyContainer() for
	// each group we have modified
	POSITION pos = m_DirtyGroups.GetStartPosition();
	while (pos)
	{
		void *pKey, *pVal;
		m_DirtyGroups.GetNextAssoc( pos, pKey, pVal );

		CProjContainer *pItem = (CProjContainer *)pKey;
		ASSERT_VALID( pItem );

		pItem->RefreshDependencyContainer();
	}

	return TRUE;
}

CScannerCache g_ScannerCache;
///////////////////////////////////////////////////////////////////////////////
//
// -------------------------------- CSysInclReg -------------------------------
//
///////////////////////////////////////////////////////////////////////////////

BOOL CSysInclReg::IsSysInclude(CString & strFile, CProject* pProject /* = NULL */)
{
	if (m_nLoadState < 1)
	{
		if (m_nLoadState == 0)
		{
			if (!DoLoad())
				TRACE ("Loading sys include registry failed!\n");
		}
		else
			return FALSE;	// had previous load failure
	}

	// [bug 175 Oct 24 v-danwh]
	// Apparently, we igonore all files, not just marked as system includes.
	// We're still going to keep file with path characters in front of them:
	// if (pEntry->m_EntryType != IncTypeCheckIncludePath) return FALSE;
	CDir dirBase; CPath path;
	if (NULL!=pProject)
	{
		dirBase = pProject->GetWorkspaceDir();
	}
	else if (g_pActiveProject != (CProject *)NULL)
	{
		dirBase = g_pActiveProject->GetWorkspaceDir();
	}
	else
	{
		CString strCWD;
		GetCurrentDirectory(MAX_PATH, strCWD.GetBuffer(MAX_PATH));
		strCWD.ReleaseBuffer();
		dirBase.CreateFromString(strCWD);
	}

	if (path.CreateFromDirAndFilename(dirBase, strFile))
	{
		if (GetFileAttributes((TCHAR *)(const TCHAR *) path) != 0xffffffff)
		{
			// found the file in local directory
			return FALSE;
		}
	}

	TCHAR buf[MAX_PATH];
	strcpy(buf, (const TCHAR *)strFile);
	_tcsupr(buf);

	void *vp;
	if (m_EntryMap.Lookup(buf, vp))
	{
		return TRUE;
	}
	else
	{
		if (strFile.Find('\\') == -1)
		{
			// for files without dirs, append redefined sys include sub dir 
			// to see if it is sys include
			for (int i=0; i<MAX_SUBDIR; i++)
			{
				CString strTmp = rgSubDir[i];
				strTmp += buf;
				if (m_EntryMap.Lookup((LPCTSTR)strTmp, vp))
				{
					return TRUE;
				}
			}
		}		
	}
	return FALSE;
}
	
///////////////////////////////////////////////////////////////////////////////
BOOL CSysInclReg::DoLoad ()
{
	CPath pt;
	CDir  dr;
	CString strSysIncl;
	_TCHAR buf [ MAX_PATH ], *pb;
	_TINT i;
	BOOL bLeadByte;
	FILE *pFile;
	
	ASSERT (m_nLoadState == 0);
	m_nLoadState = -1;		// Assume failure.
	INT FileNo, FileID;
	for (FileNo = 0; FileNo < 2; FileNo++)
	{

		if (FileNo==0)
		{										// Look for Sys version in .EXE directory
			FileID = IDS_SysExcludeHeaderFile;
			// get the Exe directory
			(void) GetModuleFileName(theApp.m_hInstance, buf, sizeof(buf));
			if (!pt.Create(buf) || !dr.CreateFromPath((const CPath)pt))	continue;
		}
		else
		{	  									// Look for user version in Windows directory
			FileID = IDS_ExcludeHeaderFile;
			// get the Windows directory:
			// CONSIDER: should use %HOMEPATH% under NT!
			::GetWindowsDirectory(buf, sizeof(buf));
		 	if (!dr.CreateFromString(buf)) continue;
		}
		strSysIncl.LoadString(FileID);				// Load filename

		if (!pt.CreateFromDirAndFilename(dr, strSysIncl))
			continue;	// Dir + filename pattern (wild cards)

		WIN32_FIND_DATA wfd;
		HANDLE hFile = ::FindFirstFile((LPCTSTR)pt, &wfd); 
		BOOL bNotDone = (hFile != INVALID_HANDLE_VALUE);
		for ( ; bNotDone; bNotDone = ::FindNextFile(hFile, &wfd))
		{
			if (wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM))
				continue;	// ignore directories, OS files

			if (!pt.CreateFromDirAndFilename(dr, wfd.cFileName))
				continue;	// Dir + actual filename

			// FUTURE: Unicode: fopen doesn't support Unicode filenames!
			if ((pFile = fopen((const _TCHAR *)pt, _T("rt"))) == NULL )
				continue;
	
			bLeadByte = FALSE;
			while (1)
			{
				pb = buf;

				//	Read in a line up to, but not including the return:
				for (;;)
				{
					i = _fgettc ( pFile );	
					if (i==EOF
						|| pb >= buf + MAX_PATH - 1
						|| (i == _T('\n') && !bLeadByte )
						) break;
					*pb++ = (_TCHAR) i;
					bLeadByte = bLeadByte ? FALSE : IsDBCSLeadByte (i);
				}
				*pb = _T('\0');

				if (feof (pFile) || ferror (pFile) ) break;
				_tcsupr (buf);

		        m_EntryMap.SetAt ( buf, this );	// Value of ptr is unimportant.
			}

			if (!(ferror (pFile) | fclose (pFile)) ) m_nLoadState = 1; //tricky tricky!
		}
		if (hFile != INVALID_HANDLE_VALUE)
			::FindClose(hFile);

	} // function succeeds if we could read any file
	return (m_nLoadState > 0 ? TRUE : FALSE);
}	

CSysInclReg AFX_DATA_EXPORT g_SysInclReg;

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------- CIncludeEntry ----------------------------
//
///////////////////////////////////////////////////////////////////////////////

BOOL CIncludeEntry::CheckAbsolute ()
{
//	See if this is a fully qualified path name.  We use the compler's method
//	here.

	const TCHAR *pchr = m_FileReference;
	if (*pchr == _T('/') || *pchr == _T('\\') || *pchr == _T(':') ||
		( !IsDBCSLeadByte (*pchr) && *(pchr + 1) == _T(':') ))
	{
		ASSERT(*pchr != _T(':'));  // should never happen!!!
		m_EntryType = IncTypeCheckAbsolute;
		return TRUE;
	}

	return FALSE;
}	
///////////////////////////////////////////////////////////////////////////////
BOOL CIncludeEntry::FindFile
(	
	const TCHAR *		lpszIncludePath,
	const CDir& 		cdBaseDirectory,
	const TCHAR *		pchProjDirectory,
	FileRegHandle &		frhResult,
	CProjItem *			pItem /* = NULL */
)
{
//
//	Try to find our file based on m_Entry type.  Note that
//	order is important here.
//
//	See CScannerCache in PROJTOOL.H for a description of how the
//	caches work.

	ASSERT(m_EntryType & (IncTypeCheckAbsolute | IncTypeCheckParentDir | IncTypeCheckIncludePath));

	void * pvCache;
	CPath Path;
	char buf [MAX_PATH];
	TCHAR *pch;
	int ich, iFound;
	CString strOptionPath, strRelative;

	// ignore per config information if pItem == NULL
	BOOL bIgnoreConfig = pItem ? FALSE : TRUE;

	// Nullify the return param.
	frhResult = (FileRegHandle)NULL;

	// Absolute means file name is right or else
	if (m_EntryType & IncTypeCheckAbsolute)
	{
		if (g_ScannerCache.m_AbsCache.Lookup((const TCHAR *)m_FileReference, pvCache))
		{
			frhResult = (FileRegHandle)pvCache;
#ifndef REFCOUNT_WORK
			g_FileRegistry.AddRegRef(frhResult);
#else
				// frhResult reference
			frhResult->AddFRHRef();
#endif
			return TRUE;
		}
		else
		{
			if (GetFileAttributes((TCHAR *)(const TCHAR *)m_FileReference) != 0xffffffff)
			{
				if (!Path.Create(m_FileReference))
					return FALSE;

#ifndef REFCOUNT_WORK
				if (frhResult = g_FileRegistry.LookupFile(&Path))
					frhResult->AddFRHRef();
				else 
					frhResult = g_FileRegistry.RegisterFile(&Path, TRUE);
#else
				// frhResult reference
				frhResult = CFileRegFile::GetFileHandle(Path);
#endif

				g_ScannerCache.m_AbsCache.SetAt((const TCHAR *)m_FileReference, (void *)frhResult);
#ifdef REFCOUNT_WORK
				// references in m_AbsCache.
				frhResult->AddFRHRef();
#endif
				return TRUE;
			}
		}
	}

	if (m_EntryType & IncTypeCheckOriginalDir)
	{
		TCHAR *	pchLookup;

		pchLookup = new TCHAR[_tcslen((const TCHAR *)m_OriginalDir) + m_FileReference.GetLength() + 2];
		_tcscpy(pchLookup, (const TCHAR *)m_OriginalDir);
		_tcscat(pchLookup, _T("#"));
		_tcscat(pchLookup, (const TCHAR *)m_FileReference);

		if (g_ScannerCache.m_OrigCache.Lookup(pchLookup, pvCache))
		{
			frhResult = (FileRegHandle)pvCache;
#ifndef REFCOUNT_WORK
			g_FileRegistry.AddRegRef(frhResult);
#else
			frhResult->AddFRHRef();
#endif
			delete [] pchLookup;
			return TRUE;
		}
		else if (!g_ScannerCache.m_NotOrigCache.Lookup(pchLookup, pvCache))
		{
			if (!Path.CreateFromDirAndFilename(m_OriginalDir, m_FileReference))
			{
				delete [] pchLookup;
				return FALSE;
			}

			if (GetFileAttributes((TCHAR *)(const TCHAR *) Path) != 0xffffffff)
			{
#ifndef REFCOUNT_WORK
				if (frhResult = g_FileRegistry.LookupFile(&Path))
					frhResult->AddFRHRef();
				else 
					frhResult = g_FileRegistry.RegisterFile(&Path);
#else
				frhResult = CFileRegFile::GetFileHandle(Path);
#endif

				g_ScannerCache.m_OrigCache.SetAt(pchLookup, (void *)frhResult);
#ifdef REFCOUNT_WORK
				// references in m_OrigCache.
				frhResult->AddFRHRef();
#endif
				Path.SetAlwaysRelative();     //like this to be relative to proj dir
				//we don't want to resolve relative path(with ..\) unless we have to
				//first, we should make sure to make everything under proj dir is proj dir relative 
				//if orignial dir is sub dir of the proj dir, convert the include name relative to 
				//proj dir, which means adding the sub dir for the include name
				//1) if the path is relative to proj dir, keep it that way
				//2) if the path is relative to include path, keep it that way
				//3) if we can resolve it on the include path, resolve it and keep the relative part
				//4) hey, it is illegal code if user wants a sharable makefile
				pch = (TCHAR *)Path.GetFullPath();
				if (m_FileReference.Find(_TEXT("..\\")) != -1)
				{
					//make sub trees relative to proj dir.
					if ((_tcslen((TCHAR *)(const TCHAR *)m_OriginalDir) > _tcslen(pchProjDirectory)) &&
					    _tcsnicmp((TCHAR *)(const TCHAR *)m_OriginalDir, pchProjDirectory, _tcslen(pchProjDirectory))==0)
					{
						CString strTmp;
						TCHAR * pchTmp = (TCHAR *)(const TCHAR *)m_OriginalDir;

						pchTmp += _tcslen(pchProjDirectory)+1;
						strTmp = pchTmp;
						strTmp = strTmp + _TEXT("\\") + m_FileReference;

						if (pchProjDirectory && (GetFileFullPath((TCHAR *)pchProjDirectory, (TCHAR *)(LPCTSTR)strTmp, buf, strOptionPath, bIgnoreConfig)!= NOTFOUND))
						{
							//yes, make the original name relative to proj
							g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strTmp);
							g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
							g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
						}
					}
					else if (pchProjDirectory && (GetFileFullPath((TCHAR *)pchProjDirectory, (TCHAR *)(LPCTSTR)m_FileReference, buf, strOptionPath, bIgnoreConfig) != NOTFOUND))
					{
						//first, can we make this relative to the project?
						//yes, let's keep the original name
						g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(m_FileReference);
						g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
						g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
					}
					else
					{
						//we can't make it relative to proj, any luck with the path?
						if ((iFound = GetFileFullPath((TCHAR *)lpszIncludePath, (TCHAR *)(LPCTSTR)m_FileReference, buf, strOptionPath, bIgnoreConfig))!=NOTFOUND)
						{
							//set it relative to the path, the ..\ won't be resolved
							g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(m_FileReference);
							g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);

							if (iFound == ONPATH)
							{
								g_FileRegistry.GetRegEntry(frhResult)->SetFileFoundInIncPath();
							}
							else
							{
								// found on option path, should remember the option path
								// reset the m_OrgName to append this option path
								g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strOptionPath);
								g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
								g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
								const TCHAR *pchr = strOptionPath;
								if (*pchr == _T('/') || *pchr == _T('\\') || *pchr == _T(':') ||
									( !IsDBCSLeadByte (*pchr) && *(pchr + 1) == _T(':') ))
								{
   									ASSERT(*pchr != _T(':'));  // should never happen!!!
									strRelative = GetRelativeName(strOptionPath, (TCHAR *)pchProjDirectory);
									g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strRelative);
								}
							}
						}
						else
						{
							//otherwise, see if resoved full path will be relative to the include path
							if ((iFound = SetFileRelativeFromPath((TCHAR *)lpszIncludePath, pch, frhResult, &ich, strOptionPath))!=NOTFOUND)
							{
								if (iFound == ONPATH)
								{
									//the index should be setup here
									g_FileRegistry.GetRegEntry(frhResult)->SetFileFoundInIncPath();
									g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(ich);
								}
								else
								{
									g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strOptionPath);
									g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
									g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
									const TCHAR *pchr = strOptionPath;
									if (*pchr == _T('/') || *pchr == _T('\\') || *pchr == _T(':') ||
										( !IsDBCSLeadByte (*pchr) && *(pchr + 1) == _T(':') ))
									{
										ASSERT(*pchr != _T(':'));  // should never happen!!!
										strRelative = GetRelativeName(strOptionPath, (TCHAR *)pchProjDirectory);
										g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strRelative);
									}
								}
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
					//always trying to see if we can be relative to the include path
					if (_tcsicmp((TCHAR *)(const TCHAR *)cdBaseDirectory, (TCHAR *)(const TCHAR *)m_OriginalDir) != 0 &&
					    ((iFound = SetFileRelativeFromPath((TCHAR *)lpszIncludePath, pch, frhResult, &ich, strOptionPath)) != NOTFOUND))
					{
						if (iFound == ONPATH)
						{
							//the index should be setup here
							g_FileRegistry.GetRegEntry(frhResult)->SetFileFoundInIncPath();
							g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(ich);
						}
						else
						{
							g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strOptionPath);
							g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
							g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
							const TCHAR *pchr = strOptionPath;
							if (*pchr == _T('/') || *pchr == _T('\\') || *pchr == _T(':') ||
								( !IsDBCSLeadByte (*pchr) && *(pchr + 1) == _T(':') ))
							{
								ASSERT(*pchr != _T(':'));  // should never happen!!!
								strRelative = GetRelativeName(strOptionPath, (TCHAR *)pchProjDirectory);
								g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strRelative);
							}
						}
					}
				}
				delete [] pchLookup;
				return TRUE;
			}
			else
			{
				g_ScannerCache.m_NotOrigCache.SetAt(pchLookup, 0);
			}
		}

		delete [] pchLookup;
	}	

	if ( m_EntryType & IncTypeCheckParentDir )
	{
		TCHAR *	pchLookup;

		pchLookup = new TCHAR[_tcslen((const TCHAR *)cdBaseDirectory) + m_FileReference.GetLength() + 2];
		_tcscpy(pchLookup, (const TCHAR *)cdBaseDirectory);
		_tcscat(pchLookup, _T("#"));
		_tcscat(pchLookup, (const TCHAR *)m_FileReference);

		if (g_ScannerCache.m_ParentCache.Lookup(pchLookup, pvCache))
		{
			frhResult = (FileRegHandle)pvCache;
#ifndef REFCOUNT_WORK
			g_FileRegistry.AddRegRef(frhResult);
#else
			frhResult->AddFRHRef();
#endif
			delete [] pchLookup;
			return TRUE;
		}
		else if (!g_ScannerCache.m_NotParentCache.Lookup(pchLookup, pvCache))
		{
			if (!Path.CreateFromDirAndFilename(cdBaseDirectory, m_FileReference))
			{
				delete [] pchLookup;
				return FALSE;
			}

			if (GetFileAttributes((TCHAR *)(const TCHAR *) Path) != 0xffffffff)
			{
#ifndef REFCOUNT_WORK
				frhResult = g_FileRegistry.LookupFile(&Path);
				if (frhResult != NULL)
					frhResult->AddFRHRef();
				else 
					frhResult = g_FileRegistry.RegisterFile(&Path);
#else
				frhResult = CFileRegFile::GetFileHandle(Path);
#endif

				g_ScannerCache.m_ParentCache.SetAt(pchLookup, (void *)frhResult);
#ifdef REFCOUNT_WORK
				// references in m_ParentCache.
				frhResult->AddFRHRef();
#endif
				Path.SetAlwaysRelative();   //like this to be relative to proj dir
				pch = (TCHAR *)Path.GetFullPath();
				if (m_FileReference.Find(_TEXT("..\\")) != -1)
				{
					//first, can we make this relative to the project?
					if (pchProjDirectory && (GetFileFullPath((TCHAR *)pchProjDirectory, (TCHAR *)(LPCTSTR)m_FileReference, buf, strOptionPath, bIgnoreConfig)!=NOTFOUND))
					{
						//yes, let's keep the original name
						g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(m_FileReference);
						g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
						g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
					}
					else
					{
						//we can't make it relative to proj, any luck with the path?
						if ((iFound = GetFileFullPath((TCHAR *)lpszIncludePath, (TCHAR *)(LPCTSTR)m_FileReference, buf, strOptionPath, bIgnoreConfig))!=NOTFOUND)
						{
							//set it relative to the path, the ..\ won't be resolved
							g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(m_FileReference);
							g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);

							if (iFound == ONPATH)
								g_FileRegistry.GetRegEntry(frhResult)->SetFileFoundInIncPath();
							else
							{
								// found on option path, should remember the option path
								// reset the m_OrgName to append this option path
								g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strOptionPath);
								g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
								g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
								const TCHAR *pchr = strOptionPath;
								if (*pchr == _T('/') || *pchr == _T('\\') || *pchr == _T(':') ||
									( !IsDBCSLeadByte (*pchr) && *(pchr + 1) == _T(':') ))
								{
									ASSERT(*pchr != _T(':'));  // should never happen!!!
									strRelative = GetRelativeName(strOptionPath, (TCHAR *)pchProjDirectory);
									g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strRelative);
								}
							}
						}
						else
						{
							//otherwise, see if resoved full path will be relative to the include path
							if ((iFound = SetFileRelativeFromPath((TCHAR *)lpszIncludePath, pch, frhResult, &ich, strOptionPath)) != NOTFOUND)
							{
								if (iFound == ONPATH)
								{
									//the index should be setup here
									g_FileRegistry.GetRegEntry(frhResult)->SetFileFoundInIncPath();
									g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(ich);
								}
								else
								{
									g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strOptionPath);
									g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
									g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
									const TCHAR *pchr = strOptionPath;
									if (*pchr == _T('/') || *pchr == _T('\\') || *pchr == _T(':') ||
										( !IsDBCSLeadByte (*pchr) && *(pchr + 1) == _T(':') ))
									{
										ASSERT(*pchr != _T(':'));  // should never happen!!!
										strRelative = GetRelativeName(strOptionPath, (TCHAR *)pchProjDirectory);
										g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strRelative);
									}							
								}
							}
							else
							{
								//stuck with the full path, live with it
							}
						}
					}
				}
				delete [] pchLookup;
				return TRUE;
			}
			else
			{
				g_ScannerCache.m_NotParentCache.SetAt(pchLookup, 0);
			}
		}

		delete [] pchLookup;
	}	

	if ( m_EntryType & IncTypeCheckIncludePath )
	{
		TCHAR *	pchLookup;

		pchLookup = new TCHAR[_tcslen(lpszIncludePath) + m_FileReference.GetLength() + 2];
		_tcscpy(pchLookup, lpszIncludePath);
		_tcscat(pchLookup, _T("#"));
		_tcscat(pchLookup, (const TCHAR *)m_FileReference);

		if (g_ScannerCache.m_IncludeCache.Lookup(pchLookup, pvCache))
		{
			frhResult = (FileRegHandle)pvCache;
#ifndef REFCOUNT_WORK
			g_FileRegistry.AddRegRef(frhResult);
#else
			frhResult->AddFRHRef();
#endif
			delete [] pchLookup;
			return TRUE;
		}
		else
		{
			if ((iFound=GetFileFullPath((TCHAR *)lpszIncludePath, (TCHAR *)(LPCTSTR)m_FileReference, buf, strOptionPath, bIgnoreConfig))!=NOTFOUND)
			{
				BOOL fRet = FALSE;
				if (Path.Create(buf))
				{
#ifndef REFCOUNT_WORK
					if (frhResult = g_FileRegistry.LookupFile(&Path))
						frhResult->AddFRHRef();
					else 
						frhResult = g_FileRegistry.RegisterFile(&Path);
#else
					frhResult = CFileRegFile::GetFileHandle(Path);
#endif

					g_ScannerCache.m_IncludeCache.SetAt(pchLookup, (void *)frhResult);
#ifdef REFCOUNT_WORK
					// references in m_IncludeCache.
					frhResult->AddFRHRef();
#endif
					if (iFound == ONPATH)
					{
						g_FileRegistry.GetRegEntry(frhResult)->SetFileFoundInIncPath();
						if (m_FileReference.Find(_TEXT("..\\")) != -1)
						{
							g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(m_FileReference);
							g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
						}
						else
						{
							g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(m_FileReference.GetLength());
						}
					}
					else if( !g_FileRegistry.GetRegEntry(frhResult)->IsFileFoundInIncPath() )
					{
						// found on option path, should remember the option path
						// reset the m_OrgName to append this option path
						g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strOptionPath);
						g_FileRegistry.GetRegEntry(frhResult)->SetIndexNameRelative(0);
						g_FileRegistry.GetRegEntry(frhResult)->SetFileNameRelativeToProj();
						const TCHAR *pchr = strOptionPath;
						if (*pchr == _T('/') || *pchr == _T('\\') || *pchr == _T(':') ||
							( !IsDBCSLeadByte (*pchr) && *(pchr + 1) == _T(':') ))
						{
							ASSERT(*pchr != _T(':'));  // should never happen!!!
							CDir dr;
							dr.CreateFromString( pchProjDirectory);
							Path.GetRelativeName( dr, strRelative );

							if (strRelative.GetLength() < (strOptionPath.GetLength()-2))
							{
								// get relative path
								TCHAR *pch = (TCHAR *)(LPCTSTR)strRelative;
								if (pch && *pch != _T('.'))
								{
									CString strTmp;
									if (*pch == _T('\\'))
									{
										strTmp = _TEXT(".") + strRelative;
									}
									else if (*pch != _T(':'))
									{
										strTmp = _TEXT(".\\") + strRelative;
									}
									else
									{
										ASSERT(0);	// Speed bug #3838
									}
									strRelative = strTmp;
								}
								g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strRelative);
							}
							g_FileRegistry.GetRegEntry(frhResult)->SetOrgName(strRelative);
						}
					}
					fRet = TRUE;
				}

				delete [] pchLookup;
				return fRet;
			}
		}

		delete [] pchLookup;
	}

	// couldn't find the file
	// so return our best guess
	BOOL fGotUnknown = FALSE, fMatchCase = FALSE;
	if (pItem != (CProjItem *)NULL &&
		(m_EntryType & IncTypeCheckIntDir) || (m_EntryType & IncTypeCheckOutDir))
	{
		// get the output directory
		CString strdirOut;
		if (pItem->GetStrProp((m_EntryType & IncTypeCheckIntDir) ?
							  P_OutDirs_Intermediate : P_OutDirs_Target, strdirOut) == valid)
		{
			// is this empty or a directory?
			if (!strdirOut.IsEmpty())
			{
				TCHAR * pchStart = strdirOut.GetBuffer(1);
				TCHAR * pchT;

				pchT = pchStart + _tcslen(pchStart);	// point to nul terminator
				pchT = _tcsdec(pchStart, pchT);			// back up one char, DBCS safe

				if (*pchT != _T('\\') && *pchT != _T('/'))
				{
					strdirOut.ReleaseBuffer();
					strdirOut += _T('/');
				}

				CDir dirOut;
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
#ifndef REFCOUNT_WORK
		if (frhResult = g_FileRegistry.LookupFile(&Path))
			frhResult->AddFRHRef();
		else 
			frhResult = g_FileRegistry.RegisterFile(&Path, fMatchCase);
#else
		frhResult = CFileRegFile::GetFileHandle(Path);
#endif
	}

	// shouldn't be scanned!
	m_bShouldBeScanned = FALSE;
	return FALSE;
}   		


///////////////////////////////////////////////////////////////////////////////
//
// ----------------------------- CIncludeEntryList ----------------------------
//
///////////////////////////////////////////////////////////////////////////////

void CIncludeEntryList::DeleteContents ()
{
	POSITION pos;
	for (pos = GetHeadPosition (); pos != NULL; )
	{
		delete ( GetNext (pos ) );
	}
	RemoveAll ();
}


// Helper function for finding include file's full path
// replace SearchPath
int GetFileFullPath(TCHAR * pstrInclude, TCHAR *pBase, TCHAR *buf, CString &strOptionPath, BOOL bIgnoreConfig /* = FALSE */)
{
	ASSERT(buf != (TCHAR *)NULL);
	CDir dirBase;
	CFileRegistry * pregistry = NULL;

	if (g_pActiveProject != (CProject *)NULL)
	{
		if (!bIgnoreConfig)
		{
			// the configuration of our file search
			ConfigurationRecord * pcr = g_pActiveProject->GetActiveConfig();

			// get the build registry for this file search
			pregistry = g_buildengine.GetRegistry(pcr);
			ASSERT(pregistry != (CFileRegistry *)NULL);
		}
		dirBase = g_pActiveProject->GetWorkspaceDir();
	}
	else
	{
		CString strCWD;
		GetCurrentDirectory(MAX_PATH, strCWD.GetBuffer(MAX_PATH));
		strCWD.ReleaseBuffer();
		dirBase.CreateFromString(strCWD);
	}

	TCHAR * szIncludeTmp = (TCHAR *)_alloca((_tcslen(pstrInclude) + 1) * sizeof(TCHAR));
	_tcscpy(szIncludeTmp, pstrInclude);

	// path of a possible resolved file
	CPath path;
	CString str1;

	GetDirMgr()->GetDirListString(str1, DIRLIST_INC);

	// get the include path from env setting of $(INCLUDE)
	TCHAR *pch, *pIncStart = _tcsstr(szIncludeTmp, (LPCTSTR)str1);

	// separate the include path list
	pch = _tcstok(szIncludeTmp, ";,");

	while (pch != NULL)
	{
		// remove leading spaces
		while (pch && *pch == _T(' '))
		{
			pch++;
		}

		int nLen = _tcslen(pch);

		// copy our include path
		_tcscpy(buf, pch);

		// remove ending spaces
		while (*_tcsdec(buf, buf + nLen) == _T(' '))
		{
			nLen--;
		}

		buf[nLen]='\0';
		// ensure that is is '\' terminated
		TCHAR chTerm = *_tcsdec(buf, buf + nLen);
		if (chTerm != _T('\\') && chTerm != _T('/'))
		{
			buf[nLen] = _T('\\'); nLen += sizeof(TCHAR);
			buf[nLen]='\0';
		}

		ASSERT((nLen + _tcslen(pBase)) <= MAX_PATH);
		//_tcscpy(buf + nLen, pBase);

		dirBase.CreateFromString(buf);

		if (path.CreateFromDirAndFilename(dirBase, pBase))
		{
	
			// exist on disk as is?
			if (path.ExistsOnDisk())
			{
				if (pch < pIncStart)
				{
					strOptionPath = buf;
					strOptionPath += pBase;
				}
				_tcscpy(buf, path.GetFullPath() );
				return ((pch>=pIncStart) ? ONPATH : ONOPTION);
			}

			// exist as a file (not yet created perhaps)
			// in our build registry (i.e. build graph)?
#ifndef REFCOUNT_WORK
			if ((g_pActiveProject != (CProject *)NULL) && (pregistry != NULL) && 
				(pregistry->LookupFile((LPCTSTR)path) != (FileRegHandle)NULL))
			{
#else
			FileRegHandle frhTmp = NULL; 
			if ((g_pActiveProject != (CProject *)NULL) && (pregistry != NULL) && 
				((frhTmp=pregistry->LookupFile((LPCTSTR)path)) != (FileRegHandle)NULL))
			{
				frhTmp->ReleaseFRHRef();
#endif
				if (pch < pIncStart)
				{
					strOptionPath = path.GetFullPath();
				}
				_tcscpy(buf, path.GetFullPath() );
				return ((pch>=pIncStart) ? ONPATH : ONOPTION);
			}
		}

		// next include path
		pch = _tcstok(NULL, ";,");
	}

	return NOTFOUND;
}

// Help routine to find out if the file is on the search path
int SetFileRelativeFromPath(TCHAR * pstrInclude, TCHAR * pPath, FileRegHandle frh, int *pich, CString &strOptionPath)
{
	TCHAR * szIncludeTmp = (TCHAR *)_alloca((_tcslen(pstrInclude) + 1) * sizeof(TCHAR));
	_tcscpy(szIncludeTmp, pstrInclude);

	TCHAR szPath[MAX_PATH];

	_tcscpy(szPath, pPath);
	_tcsupr(szPath);
	const int nPathLen = _tcslen(szPath);
	int nPchLen;
	BOOL bHasSlash;

	CString str1;

	GetDirMgr()->GetDirListString(str1, DIRLIST_INC);

	TCHAR *pch, *pIncStart = _tcsstr(szIncludeTmp, (LPCTSTR)str1);

	//separate the include path
	pch = _tcstok(szIncludeTmp, ";,");
	for ( ; pch != NULL; pch = _tcstok(NULL, ";,"))
	{
		if (pch[0] != _T('\0'))
		{
			// do we have a match?

			// upper case the include path
			_tcsupr(pch);
			nPchLen = _tcslen(pch);

			// no possible match if path is shorter than dir
			if ((nPathLen < nPchLen) || (_tcsncmp(szPath, pch, nPchLen)!=0))
				continue;

			bHasSlash = (*_tcsdec(pch, pch + nPchLen)==_T('\\'));

			if ((bHasSlash) || (szPath[nPchLen]==_T('\\')))
			{
				// yes, it is on the search path,
				if (pch >= pIncStart)
				{
					// get the relative name and length
					CString str = GetRelativeName(pPath, pch);

					*pich = str.GetLength();

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
