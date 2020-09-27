#ifndef __FILEREGISTRY_H__
#define __FILEREGISTRY_H__

#pragma once

#include <vccolls.h>
#include "path2.h"

extern CTestableSection g_sectionRegistrySingle;

///////////////////////////////////
// classes in this header
///////////////////////////////////
class CBldFileRegNotifyRx;
	class CBldFileRegEntry;
		class CBldFileRegSet;
			class CBldFileRegFilter;
			class CBldFileRegFilterSet;
			class CBldFileRegistry;
		class CBldFileRegFile;

class CSmallPtrSet;

// classes needed by this header
class CBldCfgRecord;

//
// inline replacement for ::CompareFileTime (takes references instead of pointers).
//
__inline int CompareFileTimes( const struct _FILETIME& ft1, const struct _FILETIME& ft2 )
{
	if (ft1.dwHighDateTime == ft2.dwHighDateTime)
	{
		if (ft1.dwLowDateTime == ft2.dwLowDateTime)
			return 0;
		else if (ft1.dwLowDateTime > ft2.dwLowDateTime)
			return 1;
		else
			return -1;
	}
	else if (ft1.dwHighDateTime > ft2.dwHighDateTime)
		return 1;
	else
		return -1;
}

/////////////////////////////////////////////////////////////////////////////
// BldFileRegHandle definition. All access to file registry entries is done
// using a handle. Currently this handle is just a pointer to the entry
// but this may change and so EVERYONE should use handles to refer to
// file registry entries. The file registry has a function to map from
// a BldFileRegHandle to a real file registry entry.
typedef CBldFileRegEntry * BldFileRegHandle;

// Typed array of CFileRefFile pointers.
typedef CVCTypedPtrArray<CVCPtrArray, class CBldFileRegEntry *> CFRFPtrArray;

/////////////////////////////////////////////////////////////////////////////
// Queue and dequeue our file registry entry changes.

typedef struct 
{
	BldFileRegHandle	_frh;		// file registry handle
	BOOL			_fOk;		// ok?
	FILETIME		_filetime;	// last write time
	BOOL			_fExists;	// exists?
	BOOL			_fInSccOp;	// SCC op in progress at the time?
} S_ATTRIB;

/////////////////////////////////////////////////////////////////////////////
// Standard file registry notifications

#define		FRN_ALL			0x2
#define		FRI_DESTROY		0x3
#define		FRI_ADD			0x4

/////////////////////////////////////////////////////////////////////////////
// File registry property identifiers

#define		P_NewTStamp		11
#define		P_ExistsOnDisk	12

/////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegNotifyRx. This class provides support for file registry
//	notification receiving. Basically any class that is interested in
//	being able to receive notifications about changes in the file
//	registry should derive from this. NOTE: Multiple inheritance is
//	possible, for an example take a look at the CBldFileRegEntry class
//	below. 

class CBldFileRegNotifyRx
{
public:
	virtual void	OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint) = 0;
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
		nodetypeRegFilterSet = 0x40,
		nodetypeRegMax = 0x80
	};
	CBldFileRegNotifyRx() : m_dwNodeType(nodetypeRegNotifyRx)
	{
	}
	BOOL	IsNodeType(DWORD dw)
	{
		return (dw & m_dwNodeType) != 0;
	}
protected:
	DWORD m_dwNodeType;		// Yes, this is a tempting target to cut memory by getting rid of
							// since it logically doesn't need to be different for every object
							// from the same derived class. DON'T TOUCH IT.  Calling into a 
							// derived class at the wrong time to get this info will crash the IDE...
};

// General-purpose class for storing a "small" set of pointers.
// This is used (& tuned) for keeping track of the dependants of a CBldFileRegEntry.
// For simplicity & tune-ability, it supports only the operations which are
// actually used by the dependency handling code.
//
class CSmallPtrSet : public CVCPtrArray
{
	// Implementation detail: POSITION is a 1-based index.
public:
	CSmallPtrSet()
		{}
	~CSmallPtrSet()
		{}
	int AddTail(void* newElement)
	{
		// To prevent heap fragmentation, make the array double its size
		// whenever it needs to grow.
		if (m_nSize > 4)
		{
			m_nGrowBy = m_nSize;
		}
		return (int) (Add(newElement));
	}
	int Find(void const * const pItem);
	void RemoveItem(void *pItem);
};

/////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegEntry. This class contains all information that the build
//	system will ever need to know about a specific file. The file registry
//	contains references to items of this class.

class CBldFileRegEntry : public CBldFileRegNotifyRx
{
public:
// Constructors/Destructors
  	CBldFileRegEntry();
	virtual ~CBldFileRegEntry();

	//
	// Override SafeDelete if asynchronous access to map is possible.
	//
	virtual void SafeDelete(){ delete this; }

	// handle our own allocations
	// we'll use our own private Win32 heap
	// and place de-alloc'ed blocks into a 
	// global list

// Update property functions 
	// return TRUE if actually performed refresh (ie. props were not current)
	virtual BOOL	RefreshAttributes() = 0;

// Notification support functions
 	virtual void	OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint);
 	virtual void	InformNotifyRxs(UINT idChange, IVCBuildErrorContext* pEC, void* pHint = NULL);
	void			AddNotifyRx(CBldFileRegNotifyRx* pfre, BOOL bCheckExistence);
	void			RemoveNotifyRx(CBldFileRegNotifyRx* pfre);

// Property access functions
  	virtual BOOL	GetTimeProp(UINT idProp, FILETIME& ftime, BOOL bUpdate = TRUE) { return FALSE; }
	BOOL			GetIntProp(UINT idProp, int& nVal, BOOL bUpdate = TRUE);
 	__inline BOOL 	ExistsOnDisk(BOOL bUpdate = TRUE) { if (bUpdate) (void)UpdateAttributes(); return m_nExistsOnDisk; }
	__inline BOOL	IsMatchCase() { return m_bMatchCase; }
	virtual void	SetRelative(BOOL bRelative) {}
	virtual void	SetMatchCase(BOOL bMatchCase, BOOL bCaseApproved = FALSE) { m_bMatchCase = bMatchCase; }
 	virtual const CPathW *	GetFilePath() { return NULL; }
 	virtual BOOL	GetFileTime(FILETIME& ft) { return FALSE; }

    // Bring time stamp uptodate
    virtual BOOL    Touch() { return FALSE; }

// Reference counting functions
#ifdef _DEBUG
	// Define in fileregistry.cpp instead of inline for easier debugging.
	void AddFRHRef();
	void ReleaseFRHRef();
#else
 	__inline void 	AddFRHRef() { 	CritSectionT cs(g_sectionRegistrySingle); m_nRefCount++; }
	__inline void 	ReleaseFRHRef() { 	CritSectionT cs(g_sectionRegistrySingle); m_nRefCount--; if (m_nRefCount == 0) {SafeDelete();} }
#endif
	__inline int	GetRefCount() { return m_nRefCount; }
 
// Helper function for comparing times
	__inline static long		MyCompareFileTime(FILETIME* pTime1, FILETIME* pTime2)
	{
		return CompareFileTimes(*pTime1, *pTime2);
	}
													 
// Helper function - updates the attributes if necessary
	// return TRUE if actually changed (ie. props were updated)
	virtual BOOL		UpdateAttributes();

// Should we refresh attributes? (default is only if we are dirty)
	virtual inline BOOL DoRefresh()	{ return m_bDirty && m_bUpdateAttribs; }
	__inline void MakeDirty() { if (!m_bDirty) { m_bDirty = TRUE; InformNotifyRxs(FRN_ALL, NULL); } }

// include file dep related funcitons
	__inline void SetFileFoundInIncPath() { m_bFoundInPath = TRUE;}
	__inline BOOL IsFileFoundInIncPath() { return m_bFoundInPath;}
	__inline void SetFileNameRelativeToProj() { m_bRelativeToProjName = TRUE;}
	__inline BOOL IsFileNameRelativeToProj() { return m_bRelativeToProjName;}

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
	 	
public:
    virtual void    DeleteFromDisk() { return; }
};

/////////////////////////////////////////////////////////////////////////////
//
// CBldFileRegSet

class CBldFileRegSet : public CBldFileRegEntry
{
	friend class CBldFileRegEntry;

public:
// Constructors/Destructors
	CBldFileRegSet(int nHashSize = 37);
	virtual ~CBldFileRegSet();

// Containment (by reference) support functions
	virtual BOOL	AddRegHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC = NULL, BOOL bAddRef = TRUE, 
		BOOL bFront = FALSE);
	virtual BOOL	RemoveRegHandle(BldFileRegHandle hndFileReg, BOOL bReleaseRef = TRUE);
	
// Exist?
	BOOL			RegHandleExists(BldFileRegHandle hndFileReg) const
	{
		WORD w;
		if( m_pmapContents != NULL )
		{
			BOOL b;
			b = m_pmapContents->Lookup((void *)hndFileReg, w);
			return b;
		}
		else
		{
			BOOL b;
			b = ( m_Contents.Find((void *)hndFileReg) != NULL );
			return b;
		}
	}

// Return the contents list
	// GetContents is no longer exposed.
	__inline BOOL IsEmpty() { return m_Contents.IsEmpty(); }
	__inline int GetCount() { return (int) m_Contents.GetCount(); }
	__inline BldFileRegHandle GetFirstFrh()
	{
		BldFileRegHandle frh = NULL;
		if (!m_Contents.IsEmpty())
		{
			frh = (BldFileRegHandle)m_Contents.GetHead();
			VSASSERT (NULL!=frh, "First element of registry is NULL");
			frh->AddFRHRef();
		}
		return frh;
	}
	virtual void				EmptyContent(BOOL fReleaseRef = TRUE);
	void MakeContentsDirty(CBldFileRegistry* pRegistry);

	// Friend access needed for m_Contents.
	friend BOOL QuickNDirtyRegSetCompare(CBldFileRegSet * pregSet, CBldFileRegSet * pregSet2);
	friend class CBldFileRegistry;

	// Reluctantly added since 
	friend class CBldFileDepGraph;

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
	virtual void	OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint);

// Property Access functions
   	virtual BOOL	GetTimeProp(UINT idProp, FILETIME& ftime, BOOL bUpdate = TRUE);

// Touch contents of file set
    virtual BOOL    Touch();
	 
// Recursive enumerate containment and return non-sets
	void			InitFrhEnum();
	BldFileRegHandle 	NextFrh();

	// Works differently for new reference count model.
	void SetFileRegistry(CBldFileRegistry* pFileRegistry);

protected:

// Access methods for read-only properties. These methods can only be
// called from derived classes and friends
	virtual BOOL	SetTimeProp(UINT idProp, FILETIME ftime);

// Internal helper function for finding the new/old times of the files
// in this set.  Also combines functionally of FindExists().
	// This function will ensure that all props. are updated
	// for each of our contents
	void 			FindTimeAndExists(UINT idProp, BOOL bFindExists = FALSE);

// Internal helper function for finding the number of files that exist
	// This function will ensure that all props. are updated
	// for each of our contents
	void			FindExists();

public:
// Internal (no checks == faster)  internal equivalents
// RemoveRegHandleI() used by ::EmptyContent()
	virtual void RemoveRegHandleI(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC = NULL, BOOL bReleaseRef = TRUE);

// Remove contents from disk
    virtual void    DeleteFromDisk();

// Map providing a quick lookup into our content (also used by CBldFileRegistry objects
// to maintain ref. count).  This pointer is NULL if no map is being maintained.
	CVCMapPtrToWord	*m_pmapContents;
	enum { cMapThreshold = 8 };			// no map until there are at least # items

protected:
// Properties - Dependency notifications occur on these props
	FILETIME		m_NewTime;

	enum { nContentsBlockSize = 10 };
 	CVCPtrList		m_Contents;			// Containment list
	unsigned short	m_nHashSizeMapContents;
	unsigned short	m_nNotExists;		// Numb. of contained items that do not exist on disk
	CBldFileRegistry *	m_pFileRegistry;	// The file registry associated with us.

private:
	// Used by the enumerator
	VCPOSITION m_posEnum;

public:
// Calculate attributes?
	BOOL m_bSkipUpdate;
	BOOL m_fCalcAttrib:2;

private:
	BOOL m_fInitSet:2;
};

/////////////////////////////////////////////////////////////////////////////
//
// CBldFileRegFilter

class CBldFileRegFilter : public CBldFileRegSet
{
public:
// Constructors/Destructors
	CBldFileRegFilter(CStringW strFilter);
	virtual ~CBldFileRegFilter();
 
// Containment (by reference) support functions
	virtual BOOL	AddRegHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC = NULL, BOOL bAddRef = TRUE, 
		BOOL bFront = FALSE);
	virtual BOOL	RemoveRegHandle(BldFileRegHandle hndFileReg, BOOL bReleaseRef = TRUE);

	BOOL AddFilterHandle(BldFileRegHandle hndFileReg);
	BOOL RemoveFilterHandle(BldFileRegHandle hndFileReg);

protected:
// Internal pattern matching function
	BOOL			Filter(const wchar_t* pszText);

// Properties - NO dependency notifications occur on these props
	CStringW		m_strFilter;
	CStringW		m_strFilterLC;
};

/////////////////////////////////////////////////////////////////////////////
//
// CBldFileRegFilterSet
//		This class represents a compound filter that contains the union of 2 or more
//		CBldFileRegFilter's
//

class CBldFileRegFilterSet : public CBldFileRegSet
{
public:
	CBldFileRegFilterSet();
	virtual ~CBldFileRegFilterSet();

	virtual void OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint);
	BOOL AddFilter( CBldFileRegFilter* pFilter );

private:
	// Keep track of contained CBldFileRegFilter's.
	CVCPtrList m_FilterList;
};

/////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegistry. A file registry provides an indexed way to access CBldFileRegEntry items.

class CBldFileRegistry : public CBldFileRegSet
{
public:
// Constructors/Destructors
	CBldFileRegistry(CBldCfgRecord* pCfgRecord = NULL, int nHashSize = 911);
	virtual ~CBldFileRegistry();
 
// Registration functions
	BldFileRegHandle	RegisterFile(const CPathW* pPath, BOOL bMatchCase = TRUE, BOOL bNoFilter = TRUE);
	BldFileRegHandle	RegisterFileFilter(const CStringW* pstrFilter);

// Filter the file handle (only use if RegisterFile() with fNoFilter == TRUE)
	void 			FilterFile(BldFileRegHandle hndFileReg);
	void 			UnFilterFile(BldFileRegHandle hndFileReg);

// Removal functions
 	BOOL			RemoveFileFilter(const CStringW* pstrFilter);

// Reference counting functions
	void			AddRegRef(BldFileRegHandle hndFileReg, BOOL bNoFilter = FALSE);
	void			ReleaseRegRef(BldFileRegHandle hndFileReg, BOOL bNoFilter = FALSE);

// Search functions
	BldFileRegHandle	LookupFile(const CPathW* pPath, BOOL bVerifyCase = TRUE); 
	BldFileRegHandle	LookupFile(LPCOLESTR lpszFile, BOOL bVerifyCase = TRUE);
	BldFileRegHandle	LookupFileFilter(const CStringW* pstrFilter);

// Map a BldFileRegHandle to a CBldFileRegEntry pointer
	__inline CBldFileRegEntry*	GetRegEntry(BldFileRegHandle hndFileReg) { return hndFileReg; }

// Notification support function
	virtual void	OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint);

	virtual BOOL	AddRegHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC = NULL, BOOL bAddRef = TRUE, 
		BOOL bFront = FALSE)
					{ VSASSERT(FALSE, "AddRegHandle is a required override!"); return FALSE; }
	virtual BOOL	RemoveRegHandle(BldFileRegHandle hndFileReg, BOOL bReleaseRef = TRUE)
					{ VSASSERT(FALSE, "RemoveRegHandle is a required override!"); return FALSE; }

// Property Access functions
	void			RemoveFromProject(IVCBuildableItem* pItem, BOOL fRemoveContained = TRUE);
	void			RestoreToProject(IVCBuildableItem* pItem, BOOL fRestoreContained = TRUE);

// Deleting file function
 	BOOL			DeleteFile(BldFileRegHandle hndFileReg);

private:
// Map to go quickly from a string to a BldFileRegHandle
	CVCMapStringWToPtr	m_FilterMap;

// Internal File filter support functions
	void 			GetBasicFilters(const CStringW* pstrFilter, CVCStringWList& strList);
	BldFileRegHandle	FindBasicFilter(const CStringW* pstrBasicFilter);

private:
	BOOL AddRegistryHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC = NULL, BOOL bNoFilter = FALSE);
	BOOL RemoveRegistryHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC = NULL);
};

/////////////////////////////////////////////////////////////////////////////
//
// CBldFileRegFile

class CBldFileRegFile : public CBldFileRegEntry
{
// Constructors/Destructors
private:
	CBldFileRegFile(const CPathW* pPath, BOOL bMatchCase = FALSE, BOOL bFiltered = TRUE);
public:
	virtual ~CBldFileRegFile();

	//
	// Override SafeDelete if asynchronous access to map is possible.
	//
	virtual void SafeDelete();

	enum { MaxKeyLen = _MAX_PATH };
	static BldFileRegHandle GetFileHandle(LPCOLESTR szFileName, BOOL bVerifyCase = TRUE);
	static BldFileRegHandle LookupFileHandleByName(LPCOLESTR szFileName, BOOL bVerifyCase = TRUE);
	static BldFileRegHandle LookupFileHandleByKey(LPCOLESTR szKeyName);
	static void UpdateTimeStampTick();
	class CInitMap {
	public:
		CInitMap(int nHashSize=17);
		~CInitMap();
	};
	friend class CInitMap;

// Retrieve and set the file attributes
	void			GetAttributes(S_ATTRIB *);
	void			SetAttributes(S_ATTRIB *);

// Property Access Functions
	const CPathW*	GetFilePath()
	{
		VSASSERT(0 < wcslen(m_Path.GetFullPath()), "File reg entry set up without file path.  Bad programmer, bad programmer.");
		return &m_Path;
	}

	__inline void SetIndexNameRelative(int ich) { m_ich = ich;}
	__inline int  GetIndexNameRelative() { return m_ich;}
	__inline void SetOrgName(const CStringW& strName) { if (m_pOrgName) {free(m_pOrgName);} m_pOrgName = _wcsdup((wchar_t *)(LPCOLESTR)strName);}
	__inline const wchar_t * GetOrgName() { return m_pOrgName;}

 	virtual BOOL	GetFileTime(FILETIME& ft);
 	virtual BOOL	GetTimeProp(UINT idProp, FILETIME& ftime, BOOL bUpdate = TRUE);
	virtual void	SetMatchCase(BOOL bMatchCase, BOOL bCaseApproved = FALSE);
	BOOL IsActualCase() { return (m_bMatchCase || (!m_bWantActualCase)); }

	inline void SetOutDirType(UINT type) { VSASSERT(type <= 2, "OutDir type out of bounds!"); m_bUseOutDir = (type==1); m_bUseIntDir = (type==2); }
	inline UINT GetOutDirType() { return m_bUseIntDir ? 2 : (m_bUseOutDir ? 1 : 0); }

    virtual BOOL    Touch();

// Update property functions 
	// return TRUE if actually performed refresh (ie. props were not current)
	virtual BOOL	RefreshAttributes();

// Helper function - updates the attributes if necessary
	// return TRUE if actually changed (ie. props were updated)
	virtual BOOL	UpdateAttributes();

// Special file type for Custom Build Events.
// This does not exist on disk
	BOOL			m_bFileNotOnDisk:2;

	CVCPtrList*		plstFileItems;		// == (CVCPtrList *)NULL if *not used* (empty)

protected:
// Access methods for read-only properties. These methods can only be
// called from derived classes and friends
	virtual BOOL	SetTimeProp(UINT idProp, FILETIME ftime);

// Properties - Dependency notifications occur on these props
	FILETIME	m_FileTime;

// Properties - NO dependency notifications occur on these props
	CPathW		m_Path;

	unsigned	m_ich;					// the include file name relative to path(just name without ..\)
	wchar_t *	m_pOrgName;				// keep the original include name if it contain ..\(relative path)

	DWORD		m_dwUpdateTick;			// helps us keep track of whether we've done an update lately or not

public:
	virtual void	DeleteFromDisk();

private:
	static CVCMapStringWToPtr* s_pFileNameMap;
	static CTestableSection g_sectionFileMap;
};

extern CBldFileRegistry	g_FileRegistry;

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

#endif // __FILEREGISTRY_H__
