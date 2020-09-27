// Copyright (c) 1998-1999 Microsoft Corporation
// Loader.h : Declaration of CLoader
//
// @doc EXTERNAL
//

#ifndef __CDMLOADER_H_
#define __CDMLOADER_H_

#ifdef XBOX
#include <xtl.h>
#else // XBOX
#include <windows.h>
#endif // XBOX

#define COM_NO_WINDOWS_H
#include <objbase.h>
#include "dmusicip.h"
#include "Alist.h"
#include "riff.h"
#include "ima.h"
#include <mmreg.h>
#include <stdio.h>
#include "smartref.h"
#include "..\shared\dmusicp.h"

class CLoader;
class CClass;

#define FOURCC_RIFF_CACHE		mmioFOURCC('D','M','L','C')
#define FOURCC_LIST_CLASSLIST	mmioFOURCC('c','l','s','l')
#define FOURCC_CLASSHEADER		mmioFOURCC('c','l','s','h')
#define FOURCC_LIST_OBJLIST		mmioFOURCC('o','b','j','l')
#define FOURCC_OBJHEADER		mmioFOURCC('o','b','j','h')

// We need an older sized object descriptor for parameter validation. 

typedef struct _DMUS_OLDOBJECTDESC
{
    DWORD          dwSize;                 /* Size of this structure. */
    DWORD          dwValidData;            /* Flags indicating which fields below are valid. */
    GUID           guidObject;             /* Unique ID for this object. */
    GUID           guidClass;              /* GUID for the class of object. */
    FILETIME       ftDate;                 /* Last edited date of object. */
    DMUS_VERSION   vVersion;               /* Version. */
    WCHAR          wszName[DMUS_MAX_NAME]; /* Name of object. */
    WCHAR          wszCategory[DMUS_MAX_CATEGORY]; /* Category for object (optional). */
    WCHAR          wszFileName[DMUS_MAX_FILENAME]; /* File path. */
    LONGLONG       llMemLength;            /* Size of Memory data. */
    LPBYTE         pbMemData;              /* Memory pointer for data. */
} DMUS_OLDOBJECTDESC;


class CDescriptor
{
public:
					CDescriptor();
					~CDescriptor();

	void			Get(LPDMUS_OBJECTDESC pDesc);
	void			Set(LPDMUS_OBJECTDESC pDesc);
	void			Merge(CDescriptor * pDesc);
	void			ClearName();
	void			SetName(WCHAR *pwzName);
	void			ClearCategory();
	void			SetCategory(WCHAR *pwzCategory);
	void			ClearFileName();
	HRESULT			SetFileName(WCHAR *pwzFileName);
	void     		SetIStream(IStream *pIStream);
    void            ClearIStream();
	void			Copy(CDescriptor *pDesc);
	BOOL			IsExtension(WCHAR *pwzExtension);
    BOOL            IsEmpty();
public:
	LONGLONG		m_llMemLength;			// Memory size.
	DWORD			m_dwValidData;			// Flags indicating which of above is valid.
	GUID			m_guidObject;			// Unique ID for this object.
	GUID			m_guidClass;			// GUID for the class of object.
	FILETIME		m_ftDate;				// File date of object.
	DMUS_VERSION	m_vVersion;				// Version, as set by authoring tool.
	WCHAR *			m_pwzName;				// Name of object.	
	WCHAR *			m_pwzCategory;			// Category for object (optional).
	WCHAR *			m_pwzFileName;			// File path.
	DWORD			m_dwFileSize;			// Size of file.
	BYTE *			m_pbMemData;			// Pointer to memory.
    IStream *       m_pIStream;             // Pointer to IStream object.
    LARGE_INTEGER   m_liStartPosition;      // Position in stream to start at.   
    CRITICAL_SECTION	m_CriticalSection;	// Critical section for the descriptor's stream.
    BOOL                m_fCSInitialized;
};

#define     SCAN_CACHE		1			// Was found in cache file.
#define     SCAN_PRIOR		2			// Was in list prior to scanning.
#define     SCAN_PARSED		4			// Was parsed in the directory.
#define     SCAN_SEARCH		8			// Type of object we are searching for.
const DWORD SCAN_GC =		1 << 12;	// Object was cached while garbage collection was enabled.
const DWORD SCAN_GC_MARK =	1 << 13;	// Bit set during marking phase of CollectGarbage.  Objects that remain unmarked are garbage.
										// Also used to detect cycles.


class CObject : public AListItem
{
public:
									CObject(CClass *pClass);
									CObject(CClass *pClass, CDescriptor *pDesc);
									~CObject();
    CObject *						GetNext() {return(CObject *)AListItem::GetNext();};
	HRESULT							Load();
	HRESULT							Parse();
	HRESULT							ParseFromFile();
	HRESULT							ParseFromMemory();
	HRESULT							ParseFromStream();
	HRESULT							GC_Collectable();
	HRESULT							GC_AddReference(CObject *pObject);
	HRESULT							GC_RemoveReference(CObject *pObject);
	HRESULT							GC_RemoveAndDuplicateInParentList();

	CDescriptor						m_ObjectDesc;
	IDirectMusicObject *			m_pIDMObject;
	DWORD							m_dwScanBits;
	SmartRef::Vector<CObject*> *	m_pvecReferences;
	CClass *				m_pClass;
};

class CObjectList : public AList
{
public:
    CObject *	GetHead() {return (CObject *)AList::GetHead();};
    CObject *	RemoveHead() {return (CObject *)AList::RemoveHead();};
	void		GC_Sweep(BOOL bOnlyScripts = FALSE);
};


class CClass : public AListItem
{
public:
						CClass( CLoader *pLoader );
						CClass( CLoader *pLoader, CDescriptor *pDesc );
						~CClass();
	void				ClearObjects(BOOL fKeepCache,WCHAR *pwzExtension);
	void				RemoveObject(CObject* pRemoveObject);
    CClass *			GetNext() {return(CClass *)AListItem::GetNext();};
	HRESULT				GetPath(WCHAR *pwzPath);	// Finds path.
	HRESULT             FindObject(CDescriptor *pDesc,CObject **ppObject, CObject *NotThis = NULL, IDirectMusicObject *pIDMObject = NULL);
	HRESULT				EnumerateObjects(
							DWORD dwIndex, 
							CDescriptor *pDesc) ;
	HRESULT				SetSearchDirectory(const WCHAR *pwzPath,BOOL fClear);
	HRESULT				SearchDirectory(WCHAR *pwzFileExtension);
	HRESULT				EnableCache(BOOL fEnable);
	HRESULT				ClearCache(bool fClearStreams); // fClearStreams also clears the IStream members of all descriptors. It is used when shutting down the loader in Release.
	HRESULT				SaveToCache(IRIFFStream *pRiff);
	HRESULT				GetObject(CDescriptor *pDesc, CObject ** ppObject);
	void				PreScan();
	void				GC_Replace(CObject *pObject, CObject *pObjectReplacement);
	void				GC_Sweep() { m_ObjectList.GC_Sweep(); }

	CDescriptor			m_ClassDesc;
	CLoader *			m_pLoader;
	BOOL				m_fKeepObjects;		// Hang onto objects after loading them.

	friend void GC_Report(CLoader *);
private:
	CObjectList			m_ObjectList;
	DWORD				m_dwLastIndex;		// For tracking enumeration.
	CObject *			m_pLastObject;		// Last object enumerated.
	BOOL				m_fDirSearched;		// Directory has been searched for files.
};

class CClassList : public AList
{
public:
    CClass *	GetHead() {return (CClass *)AList::GetHead();};
    CClass *	RemoveHead() {return (CClass *)AList::RemoveHead();};
};


class CLoader : public IDirectMusicLoader8, public CMemTrack,
#ifdef DXAPI
    public IDirectMusicIMA,
#endif
    public IDirectMusicLoader8P
{
public:
    // IUnknown
    //
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

	// IDirectMusicLoader
	STDMETHODIMP GetObject(LPDMUS_OBJECTDESC pDesc, REFIID, LPVOID FAR *) ;
	STDMETHODIMP SetObject(LPDMUS_OBJECTDESC pDesc) ;
#ifdef XBOX
    STDMETHODIMP SetSearchDirectory(REFGUID rguidClass, const char *pszPath, BOOL fClear) ;
#else
    STDMETHODIMP SetSearchDirectory(REFGUID rguidClass, WCHAR *pwzPath, BOOL fClear) ;
#endif
	STDMETHODIMP ScanDirectory(REFGUID rguidClass, WCHAR *pwzFileExtension, WCHAR *pwzScanFileName) ;
	STDMETHODIMP CacheObject(IDirectMusicObject * pObject) ;
	STDMETHODIMP ReleaseObject(IDirectMusicObject * pObject) ;
	STDMETHODIMP ClearCache(REFGUID rguidClass) ;
	STDMETHODIMP EnableCache(REFGUID rguidClass, BOOL fEnable) ;
	STDMETHODIMP EnumObject(REFGUID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDesc) ;

	// IDirectMusicLoader8
	STDMETHODIMP_(void) CollectGarbage() ;
	STDMETHODIMP ReleaseObjectByUnknown(IUnknown *pObject) ;
	STDMETHODIMP GetDynamicallyReferencedObject(IDirectMusicObject *pSourceObject,
														LPDMUS_OBJECTDESC pDesc,
														REFIID riid,
														LPVOID FAR *ppv) ;
	STDMETHODIMP ReportDynamicallyReferencedObject(IDirectMusicObject *pSourceObject,
														   IUnknown *pReferencedObject);
#ifdef XBOX
    STDMETHODIMP LoadObjectFromFile(REFGUID rguidClassID, 
                                            REFIID iidInterfaceID, 
                                            const char *pszFilePath, 
                                            void ** ppObject) ;
#else
    STDMETHODIMP LoadObjectFromFile(REFGUID rguidClassID, 
                                            REFIID iidInterfaceID, 
                                            WCHAR *pwzFilePath, 
                                            void ** ppObject) ;
#endif
    virtual ULONG STDMETHODCALLTYPE AddRefP();			// Private AddRef, for streams and scripts.
	virtual ULONG STDMETHODCALLTYPE ReleaseP();			// Private Release, for streams and scripts.

	// IDirectMusicIMA
	STDMETHODIMP LegacyCaching( BOOL fEnable) ;

	CLoader();
	~CLoader();
	HRESULT				Init();
	HRESULT				GetPath(WCHAR *pwzPath);	// Finds path.
	void				GC_UpdateForReleasedObject(CObject *pObject); // Used by ReleaseObject and CClass::ClearCache in removing objects from the cache.
	BOOL				m_fIMA;				// Support IMA parsing of ini files, etc.
	BOOL				m_fKeepObjects;		// Hang on to objects after loading them.

	friend void GC_Report(CLoader *);

private:
	HRESULT				LoadCacheFile(WCHAR *pwzCacheFileName);
	HRESULT				SaveCacheFile(WCHAR *pwzCacheFileName);
	HRESULT				GetClass(CDescriptor *pDesc, CClass **ppClass, BOOL fCreate);
	HRESULT 			ClearCacheInternal(REFGUID rguidClass, bool fClearStreams); // fClearStreams also clears the IStream members of all descriptors. It is used when shutting down the loader in Release.
	HRESULT             FindObject(CDescriptor *pDesc, CClass **ppClass, CObject ** ppObject,IDirectMusicObject *pIDMObject);
	HRESULT				FindObject(IDirectMusicObject *pIDMObject, CObject ** ppObject);

	void				GC_Mark(CObject *pObject);
	bool				GC_HasCycle(CObject *pObject);
	// Recursive function used to implement GC_Mark and GC_HasCycle
	bool				GC_TraverseHelper(CObject *pObject, CObject *pObjectToFind, bool fMark);

	CClassList			m_ClassList;		// Each type has its own list.
	WCHAR				m_wzPath[DMUS_MAX_FILENAME];	// Search directory.
	BOOL				m_fPathValid;		// Search dir is valid.
	long				m_cRef;
	long				m_cPRef;			// Private reference count.
    CRITICAL_SECTION	m_CriticalSection;	// Critical section to manage lists.
    BOOL                m_fCSInitialized;

	CObject *			m_pApplicationObject; // Object used to track references to objects in use by the application.
	CObject *			m_pLoadedObjectContext; // Used to determine which object called GetObject.  Initially m_pApplicationContext, but set and restored as a cascade of GetObject calls occurs between components.
	CObjectList			m_ReleasedObjectList; // Holds objects that are released, but may still be referenced by other objects.

	// Debug-only functions used by GetObject to report load failures.
	// Frequently some file will fail to load because it depends on some other file
	// that is missing and this will print out trace statements helping the user
	// understand what's missing.
#ifndef XBOX
#ifdef DBG
	void DebugTraceLoadFailure(CObject *pObject, HRESULT hrLoad);
	static void DebugTraceObject(DMUS_OBJECTDESC *pDesc);
	SmartRef::Vector<DMUS_OBJECTDESC> m_vecdescDebugTraceLoadFailure; // accumulates the failed sub-items
#endif
#endif // XBOX
    BOOL                m_fFirstLoadTry;    // For LoadObjectFromFile(). Disables warning on first try.
};

class CFileStream : public IStream, public IDirectMusicGetLoader, public CMemTrack
{
public:
    // IUnknown
    //
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

    /* IStream methods */
    virtual STDMETHODIMP Read( void* pv, ULONG cb, ULONG* pcbRead );
    virtual STDMETHODIMP Write( const void* pv, ULONG cb, ULONG* pcbWritten );
	virtual STDMETHODIMP Seek( LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition );
    virtual STDMETHODIMP SetSize( ULARGE_INTEGER /*libNewSize*/ );
    virtual STDMETHODIMP CopyTo( IStream* /*pstm */, ULARGE_INTEGER /*cb*/,
                         ULARGE_INTEGER* /*pcbRead*/,
                         ULARGE_INTEGER* /*pcbWritten*/ );
    virtual STDMETHODIMP Commit( DWORD /*grfCommitFlags*/ );
    virtual STDMETHODIMP Revert();
    virtual STDMETHODIMP LockRegion( ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
                             DWORD /*dwLockType*/ );
    virtual STDMETHODIMP UnlockRegion( ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
                               DWORD /*dwLockType*/);
    virtual STDMETHODIMP Stat( STATSTG* /*pstatstg*/, DWORD /*grfStatFlag*/ );
    virtual STDMETHODIMP Clone( IStream** /*ppstm*/ );

	/* IDirectMusicGetLoader */
	virtual STDMETHODIMP GetLoader(IDirectMusicLoader ** ppLoader);

						CFileStream();
						~CFileStream();
	HRESULT Open( WCHAR *lpFileName, DWORD dwDesiredAccess );
    void    Init( CLoader *pLoader );
	HRESULT	Close();

private:
    LONG            m_cRef;         // object reference count
    WCHAR           m_wszFileName[DMUS_MAX_FILENAME]; // Save name for cloning.
#if defined(UNDER_CE) || defined(XBOX)
    HANDLE          m_hFile;
#else
	FILE*			m_pFile;		// file pointer
#endif
	CLoader *		m_pLoader;
};

class CMemStream : public IStream, public IDirectMusicGetLoader, public CMemTrack
{
public:
    // IUnknown
    //
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

    /* IStream methods */
    virtual STDMETHODIMP Read( void* pv, ULONG cb, ULONG* pcbRead );
    virtual STDMETHODIMP Write( const void* pv, ULONG cb, ULONG* pcbWritten );
	virtual STDMETHODIMP Seek( LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition );
    virtual STDMETHODIMP SetSize( ULARGE_INTEGER /*libNewSize*/ );
    virtual STDMETHODIMP CopyTo( IStream* /*pstm */, ULARGE_INTEGER /*cb*/,
                         ULARGE_INTEGER* /*pcbRead*/,
                         ULARGE_INTEGER* /*pcbWritten*/ );
    virtual STDMETHODIMP Commit( DWORD /*grfCommitFlags*/ );
    virtual STDMETHODIMP Revert();
    virtual STDMETHODIMP LockRegion( ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
                             DWORD /*dwLockType*/ );
    virtual STDMETHODIMP UnlockRegion( ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
                               DWORD /*dwLockType*/);
    virtual STDMETHODIMP Stat( STATSTG* /*pstatstg*/, DWORD /*grfStatFlag*/ );
    virtual STDMETHODIMP Clone( IStream** /*ppstm*/ );

	/* IDirectMusicGetLoader */
	virtual STDMETHODIMP GetLoader(IDirectMusicLoader ** ppLoader);

						CMemStream( );
						~CMemStream();
    void                Init( CLoader *pLoader) ;
	HRESULT				Open( BYTE *pbData, LONGLONG llLength );
	HRESULT				Close();

private:
    LONG            m_cRef;         // object reference count
	BYTE*			m_pbData;		// memory pointer
	LONGLONG		m_llLength;
	LONGLONG		m_llPosition;	// Current file position.
	CLoader *		m_pLoader;
};


class CStream : public IStream, public IDirectMusicGetLoader, public CMemTrack
{
public:
    // IUnknown
    //
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

    /* IStream methods */
    virtual STDMETHODIMP Read( void* pv, ULONG cb, ULONG* pcbRead );
    virtual STDMETHODIMP Write( const void* pv, ULONG cb, ULONG* pcbWritten );
	virtual STDMETHODIMP Seek( LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition );
    virtual STDMETHODIMP SetSize( ULARGE_INTEGER /*libNewSize*/ );
    virtual STDMETHODIMP CopyTo( IStream* /*pstm */, ULARGE_INTEGER /*cb*/,
                         ULARGE_INTEGER* /*pcbRead*/,
                         ULARGE_INTEGER* /*pcbWritten*/ );
    virtual STDMETHODIMP Commit( DWORD /*grfCommitFlags*/ );
    virtual STDMETHODIMP Revert();
    virtual STDMETHODIMP LockRegion( ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
                             DWORD /*dwLockType*/ );
    virtual STDMETHODIMP UnlockRegion( ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
                               DWORD /*dwLockType*/);
    virtual STDMETHODIMP Stat( STATSTG* /*pstatstg*/, DWORD /*grfStatFlag*/ );
    virtual STDMETHODIMP Clone( IStream** /*ppstm*/ );

	/* IDirectMusicGetLoader */
	virtual STDMETHODIMP GetLoader(IDirectMusicLoader ** ppLoader);

						CStream();
						~CStream();
    void                Init( CLoader *pLoader );
	HRESULT				Open(IStream *pIStream,LARGE_INTEGER liStartPosition);
	HRESULT				Close();

private:
    LONG            m_cRef;         // object reference count
    IStream *       m_pIStream;
	CLoader *		m_pLoader;
};

// Class factory
// 
class CLoaderFactory : public IClassFactory
{
public:
	// IUnknown
    //
	virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();

	// Interface IClassFactory
    //
	virtual STDMETHODIMP CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv);
	virtual STDMETHODIMP LockServer(BOOL bLock); 

	// Constructor
    //
	CLoaderFactory();

	// Destructor
	~CLoaderFactory(); 

private:
	long m_cRef;
};

class CContainerFactory : public IClassFactory
{
public:
	// IUnknown
    //
	virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();

	// Interface IClassFactory
    //
	virtual STDMETHODIMP CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv);
	virtual STDMETHODIMP LockServer(BOOL bLock); 

	// Constructor
    //
	CContainerFactory();

	// Destructor
	~CContainerFactory(); 

private:
	long m_cRef;
};

#endif //__CDMLOADER_H_


