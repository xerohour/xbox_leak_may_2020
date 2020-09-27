/////////////////////////////////////////////////////////////////////////////
//
//	CFileRegEntry, CFileRegSet, CFileRegFilter, CFileRegFile, CProjFileRegistry
//
//	File registry database classes
//
//	History:
//	Date						Who						What
//	07/20/94					colint					Cleaned up existing
//														file registry and
//														added file filters.
/////////////////////////////////////////////////////////////////////////////

#ifndef __PFILEREG_H__
#define __PFILEREG_H__

// New reference count work.
#define REFCOUNT_WORK

#include <bldapi.h>
#include <gmap.h>

#if defined(_DEBUG) && defined(new)
#define _DO_DEBUG_NEW
#undef new
#endif

//
// inline replacement for ::CompareFileTime (takes references instead of pointers).
//
__inline int CompareFileTimes( const struct _FILETIME& ft1, const struct _FILETIME& ft2 )
{
	if ( ft1.dwHighDateTime == ft2.dwHighDateTime )
	{
		if ( ft1.dwLowDateTime == ft2.dwLowDateTime )
		{
			return 0;
		}
		else if (ft1.dwLowDateTime > ft2.dwLowDateTime)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else if (ft1.dwHighDateTime > ft2.dwHighDateTime)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

__inline int FuzzyCompareFileTimes( const struct _FILETIME& ft1, const struct _FILETIME& ft2 )
{
	if ( ft1.dwHighDateTime == ft2.dwHighDateTime )
	{
		unsigned int fuzzy = 0;
		if( theApp.m_bWin95 ) fuzzy = 20000000;
		if ( ft1.dwLowDateTime > (ft2.dwLowDateTime+fuzzy))
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else if (ft1.dwHighDateTime > ft2.dwHighDateTime)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Local heap

#ifdef USE_LOCAL_BUILD_HEAP
class CLclHeap
{
public:
	// constructor + destruct
	// 'cPages' is # of pages (4k) to initially alloc.
	// 'fCreate' is whether to create the heap at ctor time
	CLclHeap(USHORT cPages = 0, BOOL fCreate = FALSE);
	virtual ~CLclHeap();

	// create + destroy the heap
	BOOL Create();
	BOOL Destroy(); 

	// alloc. + dealloc.
	__inline void * alloc(size_t size, BOOL fZero = FALSE)
	{
#ifdef _DEBUG
		m_lAlloc += (ULONG)size;
#endif // _DEBUG 
		return ::HeapAlloc(m_hHeap, HEAP_NO_SERIALIZE, size);
	}

	enum FILL {zeroed = 0};
	__inline void * alloc(size_t size, FILL)
	{
#ifdef m_lAlloc
		lAlloc += (ULONG)size;
#endif // _DEBUG 
		return ::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, size);
	}

	__inline void dealloc(void * pv)
	{
		ASSERT(pv != (void *)NULL);	// don't allow de-alloc. NULL ptrs
#ifdef _DEBUG
		m_lAlloc -= (ULONG)::HeapSize(m_hHeap, HEAP_NO_SERIALIZE, pv);
#endif // _DEBUG
		VERIFY(::HeapFree(m_hHeap, HEAP_NO_SERIALIZE, pv));
	}

#ifdef _DEBUG
	// dump heap statistics
	virtual void Dump(CDumpContext & dc);
#endif // _DEBUG

private:
	// our heap
	HANDLE m_hHeap;

	// default pages in our heap
	USHORT m_cPages;

#ifdef _DEBUG
	// allocated data size?
	ULONG m_lAlloc;
#endif // _DEBUG
};
#endif // USE_LOCAL_BUILD_HEAP

class CFileRegEntry;
class CFileItem;
class ConfigurationRecord;
class FCItem;
class CFileRegistry;
class CTargetItem;

/////////////////////////////////////////////////////////////////////////////
// FileRegHandle definition. All access to file registry entries is done
// using a handle. Currently this handle is just a pointer to the entry
// but this may change and so EVERYONE should use handles to refer to
// file registry entries. The file registry has a function to map from
// a FileRegHandle to a real file registry entry.
typedef CFileRegEntry * FileRegHandle;

// Typed array of CFileRefFile pointers.
typedef CTypedPtrArray<CPtrArray,class CFileRegEntry *> CFRFPtrArray;

/////////////////////////////////////////////////////////////////////////////
// The file change notification function. This function is called whenever
// properties of a file registered with the file registry change on disk.
void BldSrcFileChangeCallBack(const FCItem* pfc, DWORD dwUser);

/////////////////////////////////////////////////////////////////////////////
// The asynchronous file-added funciton. This function is called when
// the file change thread has successfully added the file to it's file
// change notification list (asynchronously)
//
// #define _BLD_SRC_FC_ASYNC to enable this
//
// #define _BLD_SRC_FC_ASYNC
void BldSrcFileAddedCallBack(const FCItem* pfc);

/////////////////////////////////////////////////////////////////////////////
// The build system can cope without file change notifications and if this
// is the case then the build system will go to disk whenever it needs a
// property of a file, e.g. it stats the file. To turn off file change
// notifications just define _BLD_NO_FILE_CHNG
//
// #define _BLD_NO_FILE_CHNG to disable file change notifications

/////////////////////////////////////////////////////////////////////////////
// This enables file registry object history debugging. This is useful
// for tracking down problems where we have done one too many ReleaseRegRef's
// or one too many AddRegRef's.
//
// This is enabled by default in the debug build
//
// #define _BLD_REG_HISTORY to enable this

#ifdef _DEBUG
// This is VERY TIME INTENSIVE, so we don't do it for now!!!
// #define _BLD_REG_HISTORY     
#endif

#ifdef _BLD_REG_HISTORY

class CFRHHistory : public CObject
{
public:
	CFRHHistory(TCHAR * szFile, int nLine, BOOL bAddRef, CFRHHistory * pNext) 
		{ m_szFile = szFile; m_nLine = nLine; m_bAddRef = bAddRef; m_pNext = pNext; }
	~CFRHHistory() { if (m_pNext) delete m_pNext; }

#ifdef _DEBUG
	void Dump(CDumpContext &DC) const;
#endif

private:
	TCHAR *	        m_szFile;
	int		        m_nLine;
	CFRHHistory *	m_pNext;
	BOOL			m_bAddRef;
};

#endif

/////////////////////////////////////////////////////////////////////////////
// Queue and dequeue our file registry entry changes.

typedef struct 
{
	FileRegHandle	_frh;		// file registry handle
	BOOL			_fOk;		// ok?
	FILETIME		_filetime;	// last write time
	BOOL			_fExists;	// exists?
	BOOL			_fInSccOp;	// SCC op in progress at the time?
} S_ATTRIB;

extern void FileChangeQ(FileRegHandle frh, BOOL fInSccOp = FALSE);
extern int FileChangeDeQ();

/////////////////////////////////////////////////////////////////////////////
// Setting this flag to TRUE will prevent any further requests for file
// change notification, eg. it is used during builder creation
extern BOOL g_FileRegDisableFC;

/////////////////////////////////////////////////////////////////////////////
//
// The file registry uses its own notification system. Originally some
// thought was given as to whether the CSlob class could be used as the
// base class. However it was decided that the difference between the
// functionality of the file registry classes and the CSlob classes was
// large enough to warrant not using the CSlob class as the base class.
// 
// The notification mechanism is sufficient to provide notifications within
// the file registry. However if other classes outside of the file registry
// wish to be notified of changes in the "properties" of the file registry
// items then this will have to be added by hand.

/////////////////////////////////////////////////////////////////////////////
// Standard file registry notifications

#ifdef REFCOUNT_WORK

#define		FRN_ALL			0x2
#define		FRI_DESTROY		0x3
#define		FRI_ADD			0x4

#else

#define		FRN_DESTROY		0x0
#define		FRN_ADD			0x1
#define		FRN_ALL			0x2
#define		FRN_LCL_DESTROY	0x4
#define		FRN_LAST		FRN_LCL_DESTROY

/////////////////////////////////////////////////////////////////////////////
// Standard file registry informs

#define		INF_MASK		0x10

#define		FRI_DESTROY		(FRN_DESTROY | INF_MASK)
#define		FRI_ADD			(FRN_ADD | INF_MASK)
#define		FRI_ALL			(FRN_ALL | INF_MASK)
#define		FRI_LCL_DESTROY	(FRN_LCL_DESTROY | INF_MASK)

#endif

/////////////////////////////////////////////////////////////////////////////
// File registry property identifiers

#define		P_NewTStamp		11
#define		P_ExistsOnDisk	12

/////////////////////////////////////////////////////////////////////////////
//
//	CFileRegNotifyRx. This class provides support for file registry
//	notification receiving. Basically any class that is interested in
//	being able to receive notifications about changes in the file
//	registry should derive from this. NOTE: Multiple inheritance is
//	possible, for an example take a look at the CFileRegEntry class
//	below. 

class CFileRegNotifyRx
{
protected:
	DWORD	dwNodeType;

public:
	virtual void	OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint) = 0;
		// OnNotify is a pure virtual function, this should be overridden in
		// a derived class to handle notification from the file registry
		// about any changes

	// in order to know what type of node we have, we keep track locally so we don't
	// have to use IsKindOf which takes a bloody lot of time!
	enum
	{
		nodetypeRegNotifyRx = 0x1,
		nodetypeRegEntry = 0x2,
		nodetypeRegSet = 0x4,
		nodetypeRegFilter = 0x8,
		nodetypeRegFile = 0x10,
		nodetypeRegRegistry = 0x20,
#ifndef REFCOUNT_WORK
		nodetypeRegMax = 0x40
#else
		nodetypeRegFilterSet = 0x40,
		nodetypeRegMax = 0x80
#endif
	};
	CFileRegNotifyRx()
	{
		dwNodeType = nodetypeRegNotifyRx;
	}
	DWORD	NodeType() const { return dwNodeType; }
	BOOL	IsNodeType(DWORD dw)
	{
		ASSERT(dwNodeType &&
			dwNodeType > nodetypeRegNotifyRx &&
			dwNodeType < nodetypeRegMax);
		return (dw & NodeType()) != 0;
	}
};

// General-purpose class for storing a "small" set of pointers.
// This is used (& tuned) for keeping track of the dependants of a CFileRegEntry.
// For simplicity & tune-ability, it supports only the operations which are
// actually used by the dependency handling code.
//
class CSmallPtrSet : public CPtrArray
{
	// Implementation detail: POSITION is a 1-based index.
public:
	CSmallPtrSet()
		{}
	~CSmallPtrSet()
		{}
	POSITION AddTail(void* newElement)
	{
		// To prevent heap fragmentation, make the array double its size
		// whenever it needs to grow.
		if (m_nSize > 4)
		{
			m_nGrowBy = m_nSize;
		}
		return POSITION(Add(newElement) + 1);
	}
	POSITION GetHeadPosition()
	{
		return m_nSize == 0 ? POSITION(0) : POSITION(1);
	}
	POSITION Find(void const * const pItem);
	void* GetNext(POSITION& rPosition) const;
	void RemoveItem(void *pItem);
};

/////////////////////////////////////////////////////////////////////////////
//
//	CFileRegEntry. This class contains all information that the build
//	system will ever need to know about a specific file. The file registry
//	contains references to items of this class.

class BLD_IFACE CFileRegEntry : public CObject, public CFileRegNotifyRx
{
	DECLARE_DYNAMIC(CFileRegEntry);

public:
 
// Constructors/Destructors
  	CFileRegEntry();
	virtual ~CFileRegEntry();

#ifdef REFCOUNT_WORK
	//
	// Override SafeDelete if asynchronous access to map is possible.
	//
	virtual void SafeDelete(){ delete this; }
#endif

	// handle our own allocations
	// we'll use our own private Win32 heap
	// and place de-alloc'ed blocks into a 
	// global list
// can't override operator new if the "new" is already #defined to be
// DEBUG_NEW
#ifdef USE_LOCAL_BUILD_HEAP
#if defined(_DEBUG) && defined (new)
#define REDEF_NEW
#undef new
#endif
	__inline void * operator new(size_t size) {return g_heapEntries.alloc(size);}
	__inline void * operator new(size_t size, LPCSTR, int) {return g_heapEntries.alloc(size);;}
#ifdef REDEF_NEW
#undef REDEF_NEW
#define new DEBUG_NEW
#endif
	__inline void operator delete(void * p) {g_heapEntries.dealloc(p);}
#endif

// Update property functions 
	// return TRUE if actually performed refresh (ie. props were not current)
	virtual BOOL	RefreshAttributes() = 0;

// Notification support functions
 	virtual void	OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint);
 	virtual void	InformNotifyRxs(UINT idChange, DWORD dwHint = 0);
	void			AddNotifyRx(CFileRegNotifyRx* pfre);
	void			RemoveNotifyRx(CFileRegNotifyRx* pfre);

// Property access functions
  	virtual BOOL	GetTimeProp(UINT idProp, FILETIME& ftime) { return FALSE; }
	// FUTURE(karlsi): this doesn't need to be virtual
	virtual BOOL	GetIntProp(UINT idProp, int& nVal);
 	__inline BOOL 	ExistsOnDisk() { (void)UpdateAttributes(); return m_nExistsOnDisk; }
#if 0
	__inline BOOL	IsProjectFile() { return (m_nProjRefCount > 0); }
#endif
#ifndef REFCOUNT_WORK
	__inline BOOL	IsRelative() { return m_bRelative; }
#endif
	__inline BOOL	IsMatchCase() { return m_bMatchCase; }
	// FUTURE (colint): This will be needed when m_nProjRefCount becomes a simple bool
	//virtual void	SetProjectFile(BOOL bProjectFile) { m_bProjectFile = bProjectFile; }
#ifndef REFCOUNT_WORK
	virtual void	SetRelative(BOOL bRelative) { m_bRelative = bRelative; }
#else
	// Do nothing!
	virtual void	SetRelative(BOOL bRelative) {}
#endif
	virtual void	SetMatchCase(BOOL bMatchCase, BOOL bCaseApproved = FALSE) { m_bMatchCase = bMatchCase; }
 	virtual const CPath *	GetFilePath() { return NULL; }
 	virtual BOOL	GetFileTime(FILETIME& ft) { return FALSE; }
 	virtual BOOL	GetFileTime(CString& str) { return FALSE; }

    // Bring time stamp uptodate
    virtual BOOL    Touch() { return FALSE; }

// Reference counting functions
#ifndef REFCOUNT_WORK
 	__inline void 	AddFRHRef() { m_nRefCount++; }
	__inline void 	ReleaseFRHRef() { m_nRefCount--; if (m_nRefCount == 0) {InformNotifyRxs(FRN_DESTROY); delete this;} }
#else
#ifdef _DEBUG
	// Define in pfilereg.cpp instead of inline for easier debugging.
	void AddFRHRef();
	void ReleaseFRHRef();
#else
 	__inline void 	AddFRHRef() { m_nRefCount++; }
	__inline void 	ReleaseFRHRef() { m_nRefCount--; if (m_nRefCount == 0) {SafeDelete();} }
#endif
#endif
	__inline int	GetRefCount() { return m_nRefCount; }
 
// Helper function for comparing times
	__inline static long		MyCompareFileTime(FILETIME* pTime1, FILETIME* pTime2)
	{
		return CompareFileTimes(*pTime1, *pTime2);
	}

	__inline static long		MyFuzzyCompareFileTime(FILETIME* pTime1, FILETIME* pTime2)
	{
		return FuzzyCompareFileTimes(*pTime1, *pTime2);
	}
													 
// Helper function for registering this file reg entry with the file change
// notification system
	virtual BOOL	WatchFile() { return FALSE; }

//#ifndef REFCOUNT_WORK
// Support for mapping from file reg handles to CFileItems.
// FUTURE (colint): This will go away sometime in the future.
	void 			AssociateFileItem(CFileItem * pItem);
	BOOL			DisassociateFileItem(CFileItem * pItem);
//#endif // REFCOUNT_WORK

#if 0
	//
	// Note(tomse): This project reference count does not work with the new
	// multiple project model.  This functionality is now provided by the
	// CProject.
	//
// Support for project reference counting - this is only used for making
// sure that IsProjectFile gives the correct answer.
	__inline void	AddProjRef() { m_nProjRefCount++; }
	__inline void	ReleaseProjRef() { m_nProjRefCount--; }
	__inline int	GetProjRefCount() { return m_nProjRefCount; }
#endif


	// NOTE: plstFileItems should eventually be removed from CFileRegEntry. 
	CObList *		plstFileItems;		// == (CObList *)NULL if *not used* (empty)

// Helper function - updates the attributes if necessary
	// return TRUE if actually changed (ie. props were updated)
	virtual BOOL		UpdateAttributes();

// Should we refresh attributes? (default is only if we are dirty)
	virtual inline BOOL DoRefresh()	{ return m_bDirty && m_bUpdateAttribs; }
	__inline void MakeDirty()	{ if (!m_bDirty) { m_bDirty = TRUE; InformNotifyRxs(FRN_ALL);} }

// include file dep related funcitons
	__inline void SetFileFoundInIncPath() { m_bFoundInPath = TRUE;}
	__inline BOOL IsFileFoundInIncPath() { return m_bFoundInPath;}
	__inline void SetFileNameRelativeToProj() { m_bRelativeToProjName = TRUE;}
	__inline BOOL IsFileNameRelativeToProj() { return m_bRelativeToProjName;}
	__inline void SetIndexNameRelative(int ich) { m_ich = ich;}
	__inline int  GetIndexNameRelative() { return m_ich;}
	__inline void SetOrgName(const CString& strName) { if (m_pOrgName) {free(m_pOrgName);} m_pOrgName = _tcsdup((TCHAR *)(LPCTSTR)strName);}
	__inline TCHAR * GetOrgName() { return m_pOrgName;}

protected:
// Access methods for read-only properties. These methods can only be
// called from derived classes and friends
	virtual BOOL	SetTimeProp(UINT idProp, FILETIME ftime) { return FALSE; }
	// FUTURE(karlsi): this doesn't need to be virtual
	virtual BOOL	SetIntProp(UINT idProp, int nVal);

// Dependants list
	CSmallPtrSet	m_Dependants;

// Properties - NO dependency notifications occur on these props
 	unsigned	m_nRefCount;
#if 0
	unsigned	m_nProjRefCount;	
#endif
#ifndef REFCOUNT_WORK
	BOOL		m_bRelative:2;
#endif
	BOOL		m_bMatchCase:2;

// Are we currently refreshing the entry's properties. If we are we have
// to turn off the automatic update mechanism to avoid getting into
// an infinite loop
	BOOL		m_bUpdateAttribs:2;

// Do we need to refresh our attributes
	BOOL		m_bDirty:2;

// Used for include file dependency list
	BOOL		m_bFoundInPath:2;		// this file is on in -I/INCLUDE paths
	BOOL		m_bRelativeToProjName:2;// this file is on in -I/INCLUDE paths
	BOOL		m_bUseIntDir:2;
	BOOL		m_bUseOutDir:2;
	BOOL		m_bWantActualCase:2;

// Properties - Dependency notifications occur on these props
	BOOL		m_nExistsOnDisk:2;
	 	
	unsigned	m_ich;					// the include file name relative to path(just name without ..\)
	TCHAR *		m_pOrgName;				// keep the original include name if it contain ..\(relative path)

public:
// Diagnostics
#ifdef _DEBUG
	virtual void	Dump(CDumpContext& dc) const;
#endif
    virtual void    DeleteFromDisk() { return; }

#ifdef USE_LOCAL_BUILD_HEAP
	// our local file reg. entry heap
	static CLclHeap g_heapEntries;
#endif
};

#if 0 // REFCOUNT_WORK ??
class CFileRegFile;
class BLD_IFACE CFileRegRefSet : public CFileRegEntry
{
public:
	virtual BOOL AddRegRef(FileRegHandle hndFileReg);
	virtual BOOL ReleaseRegRef(FileRegHandle hndFileReg);

	BOOL HasReference(FileRegHandle hndFileReg);

// Search functions
	FileRegHandle	LookupFile(const CPath* pPath); 
	FileRegHandle	LookupFile(LPCTSTR lpszFile);

	FileRegHandle RegisterFileFilter(const CString* pstrFilter);

// Filter the file handle (only use if RegisterFile() with fNoFilter == TRUE)
	void FilterFile(FileRegHandle hndFileReg);

	// Stuff added just to get this running.
	__inline CFileRegEntry *	GetRegEntry(FileRegHandle hndFileReg) { return hndFileReg; }

protected:
	FileRegHandle	LookupFileFilter(const CString* pstrFilter);

// Maps to go quickly from a string to a FileRegHandle
	CMapStringToPtr	m_FileMap;
	CMapStringToPtr	m_FilterMap;
};

class BLD_IFACE CFileRegProjectSet : public CFileRegRefSet
{
public:
	virtual BOOL AddRegRef(FileRegHandle hndFileReg);
	virtual BOOL ReleaseRegRef(FileRegHandle hndFileReg);
	virtual BOOL	RefreshAttributes();

	__inline BOOL IsProjectFile(FileRegHandle hndFileReg){ return HasReference(hndFileReg); };
	BOOL RemoveFromProject(CProjItem * pItem, BOOL fRemoveContained = TRUE);
	BOOL RestoreToProject(CProjItem * pItem, BOOL fRestoreContained = TRUE);

 	BOOL GetFileItemList(const CPtrList* pFileList, CObList& pFileItemList, BOOL fOnlyBuildable = TRUE, const ConfigurationRecord* pcr = NULL);
	UINT GetFileCount(const CPtrList * pFileList, BOOL fOnlyBuildable = TRUE, const ConfigurationRecord * pcr = NULL);
  	BOOL GetFileList(const CPath* pPath, const CPtrList*& pFileList);

	__inline const CPtrList* GetODLFileList() const 	{ return (const CPtrList*) &m_filelistODL; }
 	__inline const CPtrList* GetRCFileList() const { return (const CPtrList*) &m_filelistRC; }
	__inline const CPtrList* GetDEFFileList() const { return (const CPtrList*) &m_filelistDEF; }

private:
	CPtrList m_filelistRC;
	CPtrList m_filelistDEF;
	CPtrList m_filelistODL;
};

#endif // REFCOUNT_WORK

/////////////////////////////////////////////////////////////////////////////
//
// CFileRegSet

class BLD_IFACE CFileRegSet : public CFileRegEntry
{
	friend class CFileRegEntry;

	DECLARE_DYNAMIC(CFileRegSet)

public:

// Constructors/Destructors
	CFileRegSet(int nHashSize = 37);
	virtual ~CFileRegSet();

// Containment (by reference) support functions
	virtual BOOL	AddRegHandle(FileRegHandle hndFileReg, BOOL bAddRef = TRUE, BOOL bFront = FALSE);
	virtual BOOL	RemoveRegHandle(FileRegHandle hndFileReg, BOOL bReleaseRef = TRUE);
	
// Exist?
#ifdef REGISTRY_FILEMAP
	BOOL	RegHandleExists(FileRegHandle hndFileReg) const;
#else
	BOOL CFileRegSet::RegHandleExists(FileRegHandle hndFileReg) const
	{
		WORD w;
		return m_pmapContents != NULL
			   ? m_pmapContents->Lookup((void *)hndFileReg, w)
			   : (BOOL)m_Contents.Find((void *)hndFileReg);
	}
#endif

// Return the contents list
#ifdef REFCOUNT_WORK
	// GetContents is no longer exposed.
	__inline BOOL IsEmpty() { return m_Contents.IsEmpty(); }
	__inline int GetCount() { return m_Contents.GetCount(); }
	__inline FileRegHandle GetFirstFrh()
	{
		FileRegHandle frh = NULL;
		if (!m_Contents.IsEmpty())
		{
			frh = (FileRegHandle)m_Contents.GetHead();
			ASSERT (NULL!=frh);
			frh->AddFRHRef();
		}
		return frh;
	}
	virtual void				EmptyContent(BOOL fReleaseRef = TRUE);

	// Friend access needed for m_Contents.
	friend BOOL QuickNDirtyRegSetCompare(CFileRegSet * pregSet, CFileRegSet * pregSet2);
	friend class CFileRegistry;

	// Reluctantly added since 
	friend class CFileDepGraph;
#else
	__inline const CPtrList *	GetContent() { return &m_Contents; }
	virtual void				EmptyContent(BOOL fReleaseRef = TRUE);
	virtual void				ReplaceContent(CPtrList *, BOOL fAddRef = TRUE);
#endif

// Update property functions 
	// return TRUE if actually performed refresh (ie. props were not current)
	virtual BOOL	RefreshAttributes();

// Should we refresh attributes? (ask each of our content in turn...)
	virtual BOOL	DoRefresh();

// Helper function - updates the attributes if necessary
	// return TRUE if actually changed (ie. props were updated)
	virtual BOOL	UpdateAttributes();

// Incr. update of properties?
	BOOL	CalcAttributes(BOOL fOn = TRUE);

// Notification support function
	virtual void	OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint);

// Property Access functions
   	virtual BOOL	GetTimeProp(UINT idProp, FILETIME& ftime);

// Touch contents of file set
    virtual BOOL    Touch();
	 
// Recursive enumerate containment and return non-sets
	void			InitFrhEnum();
	FileRegHandle 	NextFrh();

#ifndef REFCOUNT_WORK
// Set the file registry associated with this set
	__inline void	SetFileRegistry(CFileRegistry * pFileRegistry)
		{ m_pFileRegistry = pFileRegistry; }
#else
	// Works differently for new reference count model.
	void SetFileRegistry(CFileRegistry * pFileRegistry);
#endif

// Map providing a quick lookup into our content (also used by CFileRegistry objects
// to maintain ref. count).  This pointer is NULL if no map is being maintained.
	CMapPtrToWord	*m_pmapContents;
	enum { cMapThreshold = 8 };			// no map until there are at least # items

protected:

// Access methods for read-only properties. These methods can only be
// called from derived classes and friends
	virtual BOOL	SetTimeProp(UINT idProp, FILETIME ftime);

// Properties - Dependency notifications occur on these props
	FILETIME		m_NewTime;

// Internal helper function for finding the new/old times of the files
// in this set.  Also combines functionally of FindExists().
	// This function will ensure that all props. are updated
	// for each of our contents
	void 			FindTimeAndExists(UINT idProp, BOOL bFindExists = FALSE);

// Internal helper function for finding the number of files that exist
	// This function will ensure that all props. are updated
	// for each of our contents
	void			FindExists();

	enum { nContentsBlockSize = 10 };
 	CPtrList		m_Contents;			// Containment list
	unsigned short	m_nHashSizeMapContents;
	unsigned short	m_nNotExists;		// Numb. of contained items that do not exist on disk
	CFileRegistry *	m_pFileRegistry;	// The file registry associated with us.	

public:
// Diagnostics
#ifdef _DEBUG
	virtual void	Dump(CDumpContext& dc) const;
#endif

// Internal (no checks == faster)  internal equivalents
// RemoveRegHandleI() used by ::EmptyContent()
	virtual void RemoveRegHandleI(FileRegHandle hndFileReg, BOOL bReleaseRef = TRUE);

// Remove contents from disk
    virtual void    DeleteFromDisk();

// Calculate attributes?
	BOOL m_fCalcAttrib:2;

private:
	BOOL m_fInitSet:2;

	// Used by the enumerator
	POSITION m_posEnum;
};

/////////////////////////////////////////////////////////////////////////////
//
// CFileRegFilter

class BLD_IFACE CFileRegFilter : public CFileRegSet
{
public:

// Constructors/Destructors
	CFileRegFilter(CString strFilter);
	virtual ~CFileRegFilter();
 
// Property access functions
	__inline const CString *	GetFilter() { return &m_strFilter; }

// Containment (by reference) support functions
	virtual BOOL	AddRegHandle(FileRegHandle hndFileReg, BOOL bAddRef = TRUE);
	virtual BOOL	RemoveRegHandle(FileRegHandle hndFileReg, BOOL bReleaseRef = TRUE);

#ifdef REFCOUNT_WORK
	BOOL AddFilterHandle(FileRegHandle hndFileReg);
	BOOL RemoveFilterHandle(FileRegHandle hndFileReg);
#endif

protected:
// Internal pattern matching function
	BOOL			Filter(LPCTSTR pszText);

// Properties - NO dependency notifications occur on these props
	CString		m_strFilter;
	CString		m_strFilterLC;

public:
// Diagnostics
#ifdef _DEBUG
	virtual void	Dump(CDumpContext& dc) const;
#endif

	DECLARE_DYNAMIC(CFileRegFilter)
};

#ifdef REFCOUNT_WORK

/////////////////////////////////////////////////////////////////////////////
//
// CFileRegFilterSet
//		This class represents a compound filter that contains the union of 2 or more
//		CFileRegFilter's
//

class BLD_IFACE CFileRegFilterSet : CFileRegSet
{
public:
	CFileRegFilterSet();
	virtual ~CFileRegFilterSet();
	virtual void OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint);
	BOOL AddFilter( CFileRegFilter* pFilter );

private:
	// Keep track of contained CFileRegFilter's.
	CPtrList m_FilterList;

	DECLARE_DYNAMIC(CFileRegFilterSet)
};
#endif

/////////////////////////////////////////////////////////////////////////////
//
// CFileRegFile

class BLD_IFACE CFileRegFile : public CFileRegEntry
{
public:

// Constructors/Destructors
#ifdef REFCOUNT_WORK
private:
#endif
	CFileRegFile(const CPath* pPath, BOOL bMatchCase = FALSE, BOOL bFiltered = TRUE);
#ifdef REFCOUNT_WORK
public:
#endif
	virtual ~CFileRegFile();

#ifdef REFCOUNT_WORK
	//
	// Override SafeDelete if asynchronous access to map is possible.
	//
	virtual void SafeDelete();

	enum { MaxKeyLen = _MAX_PATH };
	static FileRegHandle GetFileHandle(LPCTSTR szFileName, LPTSTR szKeyname = NULL);
	static FileRegHandle LookupFileHandleByName(LPCTSTR szFileName, LPTSTR szKeyName = NULL);
	static FileRegHandle LookupFileHandleByKey(LPCTSTR szKeyName);
	class CInitMap {
	public:
		CInitMap(int nHashSize=17);
		~CInitMap();
	};
	friend class CInitMap;
#endif

// Retrieve and set the file attributes
	void			GetAttributes(S_ATTRIB *);
	void			SetAttributes(S_ATTRIB *);

// Property Access Functions
	const CPath*	GetFilePath()
	{
		ASSERT(0<_tcslen(m_Path.GetFullPath()));
		return &m_Path;
	}

	// only use this if you are 100% sure you know what you are doing
	void SetFilePath(const CPath * pPath)
	{
		ASSERT(pPath);
		ASSERT(m_Path.IsInit());

		m_Path = *pPath;
	}

 	virtual BOOL	GetFileTime(FILETIME& ft);
  	virtual BOOL	GetFileTime(CString& str);
 	virtual BOOL	GetTimeProp(UINT idProp, FILETIME& ftime);
	virtual void	SetMatchCase(BOOL bMatchCase, BOOL bCaseApproved = FALSE);
	BOOL IsActualCase() { return (m_bMatchCase || (!m_bWantActualCase)); }

	inline void SetOutDirType(UINT type) { ASSERT(type <= 2); m_bUseOutDir = (type==1); m_bUseIntDir = (type==2); }
	inline UINT GetOutDirType() { return m_bUseIntDir ? 2 : (m_bUseOutDir ? 1 : 0); }

    virtual BOOL    Touch();

// Update property functions 
	// return TRUE if actually performed refresh (ie. props were not current)
	virtual BOOL	RefreshAttributes();

// Helper function - updates the attributes if necessary
	// return TRUE if actually changed (ie. props were updated)
	virtual BOOL	UpdateAttributes();

// File change notification registration
	// fRefreshed is TRUE if actually performed refresh (ie. props were not current)
	BOOL			WatchFile(BOOL & fRefreshed);

// Are we watching this file or not
	BOOL			m_bFileWatch:2;

// Have we added this to file change yet?
// Pls. note, m_bFileWatch may be FALSE if asynchronous addition.
	BOOL			m_bFileAddedToFC:2;

#ifndef REFCOUNT_WORK
// Has this file been filtered?
	BOOL			m_bFiltered:2;
#endif

// Management of files for which AddWatch failed (usually because the directory is nonexistent).
// We want to re-try the watch periodically, but not too often (or our idle processing will be too slow).
//
// FCanWatch() tests whether AddWatch has never failed, or failed but not within the last "N" ticks.
// "ticks" are not realtime but are incremented by AllowRetryFailedWatches(), once per idle and at some
//		other times also (e.g. at filechange notifications).  AllowRetryFailedWatches() can also be used
//		to force reevaluation of all failed watches.

	ULONG			m_tickFailedWatch;	// non-zero if AddWatch has failed
	static ULONG	s_tickCurrent;		// incremented by AllowRetryFailedWatches

	enum { ctickRetryInterval = 32 };

	inline BOOL			FCanWatch()
		{ return m_tickFailedWatch == 0 ||
				 (s_tickCurrent - m_tickFailedWatch) >= ctickRetryInterval; }

	static void		AllowRetryFailedWatches(BOOL fForceAll);
#ifdef _DEBUG
	static ULONG	s_cFailedWatches;
#endif

protected:
// Access methods for read-only properties. These methods can only be
// called from derived classes and friends
	virtual BOOL	SetTimeProp(UINT idProp, FILETIME ftime);

// Properties - Dependency notifications occur on these props

	// REVIEW(karlsi): m_nSccStatus is no longer used
	// int		m_nSccStatus;
	FILETIME	m_FileTime;

// Properties - NO dependency notifications occur on these props
	CPath		m_Path;

public:
	virtual void	DeleteFromDisk();
// Diagnostics
#ifdef _DEBUG
	virtual void	Dump(CDumpContext& dc) const;
#endif

	DECLARE_DYNAMIC(CFileRegFile)

#ifdef REFCOUNT_WORK
private:
	static CGrowingMapStringToPtr* s_pFileNameMap;
	static CTestableSection g_sectionFileMap;
#endif
};

/////////////////////////////////////////////////////////////////////////////
//
// CFileRegEnum. 

#ifdef _BLD_REG_HISTORY
#define AddRegRef(frh)			AddRegRefI(frh, __FILE__, __LINE__)
#define ReleaseRegRef(frh)		ReleaseRegRefI(frh, __FILE__, __LINE__)
#endif

/////////////////////////////////////////////////////////////////////////////
//
//	CFileRegistry. A file registry provides an indexed way to access
//	CFileRegEntry items.

class BLD_IFACE CFileRegistry : public CFileRegSet
{
public:
// Constructors/Destructors
	CFileRegistry(int nHashSize = 911);
	virtual ~CFileRegistry();
 
// Registration functions
	FileRegHandle	RegisterFile(const CPath* pPath, BOOL bMatchCase = FALSE, BOOL bNoFilter = TRUE);
	FileRegHandle	RegisterFileFilter(const CString* pstrFilter);

// Filter the file handle (only use if RegisterFile() with fNoFilter == TRUE)
	void 			FilterFile(FileRegHandle hndFileReg);
#ifdef REFCOUNT_WORK
	void 			UnFilterFile(FileRegHandle hndFileReg);
#endif

// Removal functions
 	BOOL			RemoveFileFilter(const CString* pstrFilter);
#ifndef REFCOUNT_WORK
	BOOL			RemoveFile(const CPath* pPath);
#endif

// Reference counting functions
#ifdef _BLD_REG_HISTORY
	void			AddRegRefI(FileRegHandle hndFileReg, TCHAR * szFile, int nLine);
	void			ReleaseRegRefI(FileRegHandle, TCHAR * szFile, int nLine);
#else
	void			AddRegRef(FileRegHandle hndFileReg);
	void			ReleaseRegRef(FileRegHandle hndFileReg);
#endif	

// Search functions
	FileRegHandle	LookupFile(const CPath* pPath); 
	FileRegHandle	LookupFile(LPCTSTR lpszFile);
	FileRegHandle	LookupFileFilter(const CString* pstrFilter);

// Map a FileRegHandle to a CFileRegEntry pointer
	__inline CFileRegEntry *	GetRegEntry(FileRegHandle hndFileReg) { return hndFileReg; }

// Notification support function
	virtual void	OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint);

#ifdef REFCOUNT_WORK
	virtual BOOL	AddRegHandle(FileRegHandle hndFileReg, BOOL bAddRef = TRUE, BOOL bFront = FALSE){ ASSERT(FALSE); return FALSE; }
	virtual BOOL	RemoveRegHandle(FileRegHandle hndFileReg, BOOL bReleaseRef = TRUE){ ASSERT(FALSE); return FALSE; }
#endif

#ifndef REFCOUNT_WORK
	void			AllowAccess(CFileRegistry* pfreg);
	BOOL			DisallowAccess(CFileRegistry* pfreg);
#endif

// Property Access functions
	void			RemoveFromProject(CProjItem * pItem, BOOL fRemoveContained = TRUE);
	void			RestoreToProject(CProjItem * pItem, BOOL fRestoreContained = TRUE);
#if 0
	//
	// Note(tomse): This functionality is now provide by CProject.
	//
	BOOL			IsProjectFile(FileRegHandle hndFileReg)	{return hndFileReg->IsProjectFile();}
#endif

// Deleting file function
 	BOOL			DeleteFile(FileRegHandle hndFileReg);


 	BOOL			GetFileItemList(const CTargetItem *pTargetFilter, const CPtrList* pFileList, CObList& pFileItemList, BOOL fOnlyBuildable = TRUE, const ConfigurationRecord* pcr = NULL);
	UINT			GetFileCount(const CTargetItem * pTargetFilter, const CPtrList * pFileList, BOOL fOnlyBuildable = TRUE, const ConfigurationRecord * pcr = NULL);
  	BOOL			GetFileList(const CPath* pPath, const CPtrList*& pFileList);

	__inline const CPtrList *	GetODLFileList() const 	{ return (const CPtrList*) &m_filelistODL; }
 	__inline const CPtrList *	GetRCFileList() const { return (const CPtrList*) &m_filelistRC; }
	__inline const CPtrList *	GetDEFFileList() const { return (const CPtrList*) &m_filelistDEF; }

// Critical section to ensure sequential access to the file maps
// between the main build sys. thread and the file change thread
#ifndef REFCOUNT_WORK
	static CTestableSection		g_sectionFileMap;
#endif

private:
// Maps to go quickly from a string to a FileRegHandle
#ifdef REGISTRY_FILEMAP
	CMapStringToPtr	m_FileMap;
#endif
	CMapStringToPtr	m_FilterMap;

// Internal File filter support functions
	void 			GetBasicFilters(const CString* pstrFilter, CStringList& strList);
	FileRegHandle	FindBasicFilter(const CString* pstrBasicFilter);

#ifdef REGISTRY_FILEMAP
// Other associated file registrys that we look in when searching
	CPtrList			m_LocalFileRegs;
#endif

//	Our 'remembered' files that we are particularly interested in during
//	project processing, eg. the resource files when we generate resource
//	compiler command-lines
	CPtrList m_filelistRC;
	CPtrList m_filelistDEF;
	CPtrList m_filelistODL;

	BOOL m_fCareAboutRCFiles;
	BOOL m_fCareAboutDefFiles;

#ifdef _BLD_REG_HISTORY
	// A map from frh pointers to pointers to CPtrLists containing the
	// reference counting history of that frh object.
	CMapPtrToPtr	m_mapFRHHistory;
#endif

public:
	// olympus 16 (briancr)
	// Provide mechanism for clearing file registry. Theoretically, this
	// shouldn't be needed, since each FRH is deleted when its ref count
	// gets to zero.
#ifndef REFCOUNT_WORK
	void DeleteAll(void);
#endif

public:
// Diagnostics
#ifdef _DEBUG
	virtual void	Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#ifndef REFCOUNT_WORK
	BOOL IsLocalRegistryOf( CFileRegistry* pParent) const;
#endif
private:
	BOOL m_bAssertValidCalled;
#endif
 	
	DECLARE_DYNAMIC(CFileRegistry)

#ifdef REFCOUNT_WORK
private:
	BOOL AddRegistryHandle(FileRegHandle hndFileReg);
	BOOL RemoveRegistryHandle(FileRegHandle hndFileReg);
#endif
};

//This is used for queuing up the update dep lists
class BLD_IFACE CDepUpdateRec : public CObject
{
public:
	CFileItem * m_pFileItem;  //file item
	CPtrList * m_pdepList;    //changed dep list
	FileRegHandle m_frh;     //to cache changed rc file
	UINT m_hTarg;         // from parser
	CDepUpdateRec()
	{
		m_pdepList = NULL;
		m_pFileItem = NULL;
		m_hTarg = NULL;
	}
	~CDepUpdateRec()
	{
		if (m_pdepList)
			delete m_pdepList;
	}
};

void FileItemDepUpdateQ(CFileItem *pItem, CPtrList * depList, FileRegHandle frh, CObList &updateList, BOOL fRemove = FALSE, HBLDTARGET hTarg = NO_TARGET);
int FileItemDepUpdateDeQ();
void RemoveFromForceUpdateQueue(CDepUpdateRec *pdepRec);
void UpdateBeforeWriteOutMakeFile();
void RemoveItemsFromUpdateQueue(UINT hTarget);
void FileItemDepUpdateDeQAll();
BLD_IFACE void RemoveFileItemFromUpdateQ(CFileItem *pFileItem);
extern BLD_IFACE CObList g_FileDepUpdateListQ;
extern BLD_IFACE CObList g_FileForceUpdateListQ;

/////////////////////////////////////////////////////////////////////////////
//	The File global file registry. All files are registered in here. 

extern BLD_IFACE CFileRegistry	g_FileRegistry;


void BLD_IFACE ClearMQSCache();

void BLD_IFACE MakeQuotedString
( 
	CFileRegSet * pregset,
	CString & strOut,
	const CDir * pBaseDir = NULL, 
	BOOL bQuote = TRUE,
	BOOL (*pfn) (DWORD, FileRegHandle) = NULL,
	DWORD dwFuncCookie = NULL,
	const TCHAR * pSeperator = NULL,
	BOOL bOneOnly = FALSE,
	const CObList *plItems = NULL,	// if single item, then used for *all* handles
	BOOL bSort = FALSE
);

//
// Overload MakeQuotedString that takes a CFRFPtrArray * instead of
// a CFileRegSet *
//
void BLD_IFACE MakeQuotedString
( 
	CFRFPtrArray * pregarray,
	CString & strOut,
	const CDir * pBaseDir = NULL, 
	BOOL bQuote = TRUE,
	BOOL (*pfn) (DWORD, FileRegHandle) = NULL,
	DWORD dwFuncCookie = NULL,
	const TCHAR * pSeperator = NULL,
	BOOL bOneOnly = FALSE,
	const CObList *plItems = NULL,	// if single item, then used for *all* handles
	BOOL bSort = FALSE
);


//
// Global comparision operators for FILETIME
//
__inline BOOL operator == (const struct _FILETIME& ft1, const struct _FILETIME& ft2)
{
	return (0 == CompareFileTimes(ft1,ft2));
}
__inline BOOL operator < (const struct _FILETIME& ft1, const struct _FILETIME& ft2)
{
	return (-1 == CompareFileTimes(ft1,ft2));
}
__inline BOOL operator > (const struct _FILETIME& ft1, const struct _FILETIME& ft2)
{
	return (1 == CompareFileTimes(ft1,ft2));
}
__inline BOOL operator <= (const struct _FILETIME& ft1, const struct _FILETIME& ft2)
{
	return (1 != CompareFileTimes(ft1,ft2));
}
__inline BOOL operator >= (const struct _FILETIME& ft1, const struct _FILETIME& ft2)
{
	return (-1 != CompareFileTimes(ft1,ft2));
}

#ifdef _DO_DEBUG_NEW
#define new DEBUG_NEW
#endif

#endif // __PFILEREG_H__
