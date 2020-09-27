//
// <Scanner Classes>
//
// Scanning of source files.
//
// [???]
//
				   
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "pfilereg.h"

///////////////////////////////////////////////////////////////////////////////
//	Map which holds list of all files which have been scanned.  Keeps track
//	of file names using g_FileRegistry and FileRegHanldes.

class CIncludeEntry;
class CIncludeEntryList;
class CScannerCache
{
	// This class needs to be a friend because of the "find file" caches
	// described below.
	friend class CIncludeEntry;

public:
	CScannerCache () : m_EntryMap (29) 
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
//	#ifdef _DEBUG
	~CScannerCache ();
//	#endif

	void Add (FileRegHandle frh, CIncludeEntryList *pAddedList);

	CIncludeEntryList *LookupFile (FileRegHandle frh);

	int  BeginCache (); 
	void EndCache ();   

	// dependency updating members
	static BOOL c_bScanning;
	BOOL UpdateDependencies( CSlob *pItem, BOOL bUseAnyMethod = FALSE, BOOL bWriteOutput = TRUE );
	UINT GetFileCount( CSlob *pItem );
	BOOL ScanFiles( CSlob *pItem, UINT nTotalFiles, UINT &nDone, BOOL bUseAnyMethod = FALSE,
		BOOL bWriteOutput = TRUE );
	BOOL RefreshDependencyContainers();

	//	Deletes all entries:
	void Clear ();

private:
	
	int m_nBeginCount;
	int m_nUniqueNo;
	CMapPtrToPtr	m_EntryMap;

	CMapPtrToPtr m_DirtyGroups;

	// These caches significantly speed up the process of finding #include
	// files.  All of these map *from* strings *to* FileRegHandles:
	//
	// m_AbsCache			Files found with IncTypeCheckAbsolute
	//						Maps from filename to FileRegHandle
	// m_OrigCache			Files found with IncTypeCheckOriginalDir
	//						Maps from filename to FileRegHandle
	// m_NotOrigCache		Files *not* found with IncTypeCheckOriginalDir
	//						Maps from filename to FileRegHandle
	// m_ParentCache		Files found with IncTypeCheckParentDir
	//						Maps from "<parent_dir>#<filename>" to FileRegHandle
	// m_NotParentCache		Files *not* found with IncTypeCheckParentDir
	//						Maps from "<parent_dir>#<filename>" to FileRegHandle
	// m_IncludeCache		Files found with IncTypeCheckIncludePath
	//						Maps from "<include_path>#<filename>" to FileRegHandle
	//
	// All of these caches are used by CIncludeEntry::FindFile().
	//
	// The "Not" caches need some explanation.  CIncludeEntry::FindFile() must
	// make each search for the file as indicated by the CIncludeEntry's "IncType",
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
	CMapStringToPtr	m_AbsCache;
	CMapStringToPtr	m_OrigCache;
	CMapStringToPtr	m_NotOrigCache;
	CMapStringToPtr	m_ParentCache;
	CMapStringToPtr	m_NotParentCache;
	CMapStringToPtr	m_IncludeCache;
}; 
extern CScannerCache g_ScannerCache;
///////////////////////////////////////////////////////////////////////////////
//	Map which holds a registry of the system inculdes files and reads 
//	itself in from disk.  Scanning routines can call this to see if the've
//  hit a system include file:

class BLD_IFACE CSysInclReg
{
public:
	CSysInclReg () : m_EntryMap (97)
	{
		m_nLoadState = 0;
		m_EntryMap.InitHashTable(751);	// FUTURE: pick best size (prime)
	};

	BOOL IsSysInclude (CString & strFile, CProject* pProject = NULL);

private:
	int m_nLoadState;  // 0 => Not initilized, >0 => Loaded, <0 => Load error.
	CMapStringToPtr  m_EntryMap;
	
	BOOL DoLoad ();	
};
extern BLD_IFACE CSysInclReg g_SysInclReg;

///////////////////////////////////////////////////////////////////////////////
//	Class for holding an including refence. Each #include "stuff" generates
//	one of these:
//
enum IncludeEntryTypes			// Note: these are in order of precendence
{
	IncTypeCheckAbsolute		=  1,
	IncTypeCheckParentDir		=  2,
	IncTypeCheckIncludePath		=  4,
	IncTypeCheckOriginalDir		=  8,
	IncTypeCheckIntDir			= 16,
	IncTypeCheckOutDir			= 32,
};

class BLD_IFACE CIncludeEntry : public CObject 
{
	DECLARE_DYNAMIC (CIncludeEntry)

public:
	//	Finds the file refeneced by this entry in cpResultPath  Returns
	//	false if file not found.  Use GetLastError to found out what's
	//	wrong.
	BOOL FindFile
	(
		const TCHAR * lpszIncludePath,
		const CDir & cdBaseDirectory,
		const TCHAR * pchProjDirectory,
		FileRegHandle & frhResult,
		CProjItem * pItem = (CProjItem *)NULL
	);

	//	Examine m_FileReference to fiind out if this is an absolute
	//	file name, and set EntryType if it is.
	BOOL CheckAbsolute();

	int  		m_EntryType;
	BOOL		m_bShouldBeScanned;	// should be scanned?

	CString    	m_FileReference;
	CDir		m_OriginalDir;		// directory of originally scanned file

	CString     m_strReferenceIn;	// the source file that #included this
	int			m_nLineNumber;		// line # in this source file
};

///////////////////////////////////////////////////////////////////////////////
//	Expanded CObList to hold "touched" mark:
//
class BLD_IFACE CIncludeEntryList : public CObList
{
	DECLARE_DYNAMIC (CIncludeEntryList)

public:
	CIncludeEntryList (int nBlockSize = 10) : CObList (nBlockSize)
	{ 
		m_LastTouch = 0;
	};
	virtual ~CIncludeEntryList () { DeleteContents (); }
	CIncludeEntry *GetNextEntry(POSITION& pos)
	{
		return(CIncludeEntry *)CObList::GetNext(pos);
	};

	CIncludeEntry *GetEntryAt(POSITION& pos)  
	{
		return(CIncludeEntry *)CObList::GetAt(pos);
	};
	int m_LastTouch;		// Scatch integer for spotting cycles.

	void DeleteContents (); 
};

BLD_IFACE int GetFileFullPath(TCHAR* pstrInclude, TCHAR *pBase, TCHAR *buf, CString &strOptionPath, BOOL bIgnoreConfig = FALSE);
BLD_IFACE int SetFileRelativeFromPath(TCHAR * pstrInclude, TCHAR * pPath, FileRegHandle frh, int *pich, CString &strOPtionPath);

enum {ONOPTION, ONPATH, NOTFOUND};

#endif // _SCANNER_H_
