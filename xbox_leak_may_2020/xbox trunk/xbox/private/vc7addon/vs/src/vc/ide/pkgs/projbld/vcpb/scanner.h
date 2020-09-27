#pragma once

///////////////////////////////////////////////////////////////////////////////
//	Map which holds list of all files which have been scanned.  Keeps track
//	of file names using g_FileRegistry and FileRegHandles.

class CBldIncludeEntry;
class CBldIncludeEntryList;
class CBldScannerCache;

#include "FileRegistry.h"

class CBldScannerCache
{
	// This class needs to be a friend because of the "find file" caches described below.
	friend class CBldIncludeEntry;

public:
	CBldScannerCache() : m_EntryMap (29) 
		{
			m_nBeginCount = 0;
			m_nUniqueNo = 0;
			// FUTURE: might need to retune these hash table sizes!
			m_EntryMap.InitHashTable(523);
			m_DirtyGroups.InitHashTable(17);
			m_AbsCache.InitHashTable(29);
			m_OrigCache.InitHashTable(97);
			m_NotOrigCache.InitHashTable(29);
			m_ParentCache.InitHashTable(17);
			m_NotParentCache.InitHashTable(29);
			m_IncludeCache.InitHashTable(29);
		};
	~CBldScannerCache();

	void Add(BldFileRegHandle frh, CBldIncludeEntryList* pAddedList);

	CBldIncludeEntryList* LookupFile(BldFileRegHandle frh);

	int  BeginCache(); 
	void EndCache();   

	//	Deletes all entries:
	void Clear();

private:
	int m_nBeginCount;
	int m_nUniqueNo;
	CVCMapPtrToPtr	m_EntryMap;
	CVCMapPtrToPtr m_DirtyGroups;

	// These caches significantly speed up the process of finding #include
	// files.  All of these map *from* strings *to* FileRegHandles:
	//
	// m_AbsCache			Files found with IncTypeCheckAbsolute
	//						Maps from filename to BldFileRegHandle
	// m_OrigCache			Files found with IncTypeCheckOriginalDir
	//						Maps from filename to BldFileRegHandle
	// m_NotOrigCache		Files *not* found with IncTypeCheckOriginalDir
	//						Maps from filename to BldFileRegHandle
	// m_ParentCache		Files found with IncTypeCheckParentDir
	//						Maps from "<parent_dir>#<filename>" to BldFileRegHandle
	// m_NotParentCache		Files *not* found with IncTypeCheckParentDir
	//						Maps from "<parent_dir>#<filename>" to BldFileRegHandle
	// m_IncludeCache		Files found with IncTypeCheckIncludePath
	//						Maps from "<include_path>#<filename>" to BldFileRegHandle
	//
	// All of these caches are used by CBldIncludeEntry::FindFile().
	//
	// The "Not" caches need some explanation.  CBldIncludeEntry::FindFile() must
	// make each search for the file as indicated by the CBldIncludeEntry's "IncType",
	// and these checks must be in a particular order to emulate the C compiler's
	// behavior.  If a file was previously found as IncTypeCheckIncludePath, that
	// cache (m_IncludeCache) will again locate the file, but not until m_AbsCache,
	// m_OrigCache and m_ParentCache have been potentially checked (assuming those
	// "IncType"s were set as well).  The "Not" caches (m_NotOrigCache and m_Not-
	// ParentCache) keep track of files *not* found for those "IncTypes", so when
	// looking for a file, if it's in the "Not" cache, we know we didn't find it
	// last time and we don't need to look again.
	//
	// In other words, without the "Not" caches, the logic would be something like
	// this.  Assume we have a file which will eventually be found on the INCLUDE
	// path.
	//
	// if (m_EntryType & IncTypeCheckOriginalDir)
	// {
	//		// look in cache -- if not there, must check on disk (expensive)
	//		// won't find hypothetical file here -- will find on INCLUDE path
	//		// we could check on disk multiple times for the same file!
	// }
	//
	// With the "Not" caches, we can avoid the check on disk:
	//
	// if (m_EntryType & IncTypeCheckOriginalDir)
	// {
	//		if (FileInNotCache())
	//		{
	//			// didn't find it last time! -- no reason to look again
	//		}
	//		else
	//		{
	//			// now we have to look in cache and if not there, check on disk
	//		}
	// }
	CVCMapStringWToPtr	m_AbsCache;
	CVCMapStringWToPtr	m_OrigCache;
	CVCMapStringWToPtr	m_NotOrigCache;
	CVCMapStringWToPtr	m_ParentCache;
	CVCMapStringWToPtr	m_NotParentCache;
	CVCMapStringWToPtr	m_IncludeCache;
}; 
///////////////////////////////////////////////////////////////////////////////
//	Map which holds a registry of the system inculdes files and reads 
//	itself in from disk.  Scanning routines can call this to see if the've
//  hit a system include file:

class CBldSysInclReg
{
public:
	CBldSysInclReg()
	{
		m_nLoadState = 0;
	};

	BOOL IsSysInclude(CStringW& strFile, BOOL bUseSlowCompare=TRUE);

private:
	int m_nLoadState;  // 0 => Not initilized, >0 => Loaded, <0 => Load error.
	CVCStringWList  	m_dirList;
	
	BOOL DoLoad();	
};

extern CBldSysInclReg g_SysInclReg;

///////////////////////////////////////////////////////////////////////////////
//	Class for holding an including reference. Each #include "stuff" generates
//	one of these:
//
enum BldIncludeEntryTypes		// Note: these are in order of precendence
{
	IncTypeCheckAbsolute		=  1,
	IncTypeCheckParentDir		=  2,
	IncTypeCheckIncludePath		=  4,
	IncTypeCheckLibPath			=  8,
	IncTypeCheckOriginalDir		= 16,
	IncTypeCheckIntDir			= 32,
	IncTypeCheckOutDir			= 64,
};

class CBldIncludeEntry
{
public:
	//	Finds the file referenced by this entry in cpResultPath  Returns
	//	false if file not found.  Use GetLastError to found out what's
	//	wrong.
	BOOL FindFile(const wchar_t* lpszIncludePath, const CDirW& cdBaseDirectory, const wchar_t* pchProjDirectory,
		BldFileRegHandle& frhResult, IVCBuildableItem* pItem, CBldScannerCache* pScannerCache, 
		const wchar_t* szPlatDirs = NULL);

	//	Examine m_FileReference to find out if this is an absolute
	//	file name, and set EntryType if it is.
	BOOL CheckAbsolute();

	int  		m_EntryType;
	BOOL		m_bShouldBeScanned;	// should be scanned?

	CStringW  	m_FileReference;
	CDirW		m_OriginalDir;		// directory of originally scanned file

	CStringW    m_strReferenceIn;	// the source file that #included this
	int			m_nLineNumber;		// line # in this source file

protected:
	void SetFilePathInfo(CBldFileRegFile* pFile1, CBldFileRegFile* pFile2, int idx, CStringW& strOptionPath, 
		const wchar_t* pchProjDirectory, int iFound, bool bSetOrgNameOnPath);
	BOOL ResolveToCache(const CDirW& baseDir, CVCMapStringWToPtr* pFoundCache, CVCMapStringWToPtr* pNotFoundCache, 
		const wchar_t* pchProjDirectory, BOOL fIsInclude, IVCBuildableItem* pItem, wchar_t* buf, BldFileRegHandle& frhResult, 
		const wchar_t* lpszIncludePath, const wchar_t* szPlatDirs, bool bRelativeToParent);
};

///////////////////////////////////////////////////////////////////////////////
//	Expanded CVCPtrList to hold "touched" mark:
//
class CBldIncludeEntryList : public CVCPtrList
{
public:
	CBldIncludeEntryList(int nBlockSize = 10) : CVCPtrList(nBlockSize)
	{ 
		m_LastTouch = 0;
	};
	virtual ~CBldIncludeEntryList () { DeleteContents (); }
	CBldIncludeEntry *GetNextEntry(VCPOSITION& pos)
	{
		return(CBldIncludeEntry *)CVCPtrList::GetNext(pos);
	};

	CBldIncludeEntry *GetEntryAt(VCPOSITION& pos)  
	{
		return(CBldIncludeEntry *)CVCPtrList::GetAt(pos);
	};
	int m_LastTouch;		// Scratch integer for spotting cycles.

	void DeleteContents(); 
};

extern int SetFileRelativeFromPath(wchar_t* pstrInclude, wchar_t* pPath, BldFileRegHandle frh, IVCBuildableItem* pItem, 
	int *pich, CStringW &strOptionPath, const wchar_t* szPlatDirs, BOOL bIsIncludeNotLib);

enum { ONOPTION, ONPATH, NOTFOUND };

