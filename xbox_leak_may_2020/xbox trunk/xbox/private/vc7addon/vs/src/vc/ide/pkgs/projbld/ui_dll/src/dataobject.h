//---------------------------------------------------------------------------
// This was taken from the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
//---------------------------------------------------------------------------
#pragma once

#include <vcfileclasses.h>
#include "vcnode.h"

struct VC_DATACACHE_ENTRY;

extern LPFORMATETC _VCFillFormatEtc(
	LPFORMATETC lpFormatEtc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtcFill);

extern DVTARGETDEVICE* _VCOleCopyTargetDevice(DVTARGETDEVICE* ptdSrc);

extern void _VCOleCopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc);

extern BOOL _VCCopyStgMedium(
	CLIPFORMAT cfFormat, LPSTGMEDIUM lpDest, LPSTGMEDIUM lpSource);

//---------------------------------------------------------------------------
// CVCOleDataObject -- simple wrapper for IDataObject
//---------------------------------------------------------------------------
class CVCOleDataObject
{
// Constructors
public:
			 CVCOleDataObject();
	virtual	 ~CVCOleDataObject();

// Operations
	void Attach(LPDATAOBJECT lpDataObject, BOOL bAutoRelease = TRUE);
	LPDATAOBJECT Detach();  // detach and get ownership of m_lpDataObject
	void Release(); // detach and Release ownership of m_lpDataObject
	BOOL AttachClipboard(); // attach to current clipboard object

// Attributes
	void BeginEnumFormats();
	BOOL GetNextFormat(LPFORMATETC lpFormatEtc);
	CVCFile* GetFileData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);
	HGLOBAL GetGlobalData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);
	BOOL GetData(CLIPFORMAT cfFormat, LPSTGMEDIUM lpStgMedium,
		LPFORMATETC lpFormatEtc = NULL);
	BOOL IsDataAvailable(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);

// Implementation
public:
	LPDATAOBJECT m_lpDataObject;
	LPENUMFORMATETC m_lpEnumerator;

	// advanced use and implementation
	LPDATAOBJECT GetIDataObject(BOOL bAddRef);
	void EnsureClipboardObject();
	BOOL m_bClipboard;      // TRUE if represents the Win32 clipboard

protected:
	BOOL m_bAutoRelease;    // TRUE if destructor should call Release

private:
	// Disable the copy constructor and assignment by default so you will get
	//   compiler errors instead of unexpected behaviour if you pass objects
	//   by value or assign objects.
	CVCOleDataObject(const CVCOleDataObject&);  // no implementation
	void operator=(const CVCOleDataObject&);  // no implementation
};

//---------------------------------------------------------------------------
// CVCOleDataSource -- wrapper for implementing IDataObject
//  (works similar to how data is provided on the clipboard)
//---------------------------------------------------------------------------
class CVCOleDataSource :
 	 public CComObjectRoot
	,public IDataObject
{
protected:	// Destructors (protected to prevent accidental deletion
	virtual ~CVCOleDataSource();

public:	// Constructors
	CVCOleDataSource();

BEGIN_COM_MAP(CVCOleDataSource)
	COM_INTERFACE_ENTRY(IDataObject)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCOleDataSource)

// IDataObject
public:
	STDMETHOD(GetData)( 
            /* [unique][in] */ FORMATETC *pformatetcIn,
            /* [out] */ STGMEDIUM *pmedium);
        
	STDMETHOD(GetDataHere)( 
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [out][in] */ STGMEDIUM *pmedium);
        
	STDMETHOD(QueryGetData)( 
            /* [unique][in] */ FORMATETC *pformatetc);
        
	STDMETHOD(GetCanonicalFormatEtc)( 
            /* [unique][in] */ FORMATETC *pformatectIn,
            /* [out] */ FORMATETC *pformatetcOut) { return DATA_S_SAMEFORMATETC; }
        
	STDMETHOD(SetData)( 
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [unique][in] */ STGMEDIUM *pmedium,
            /* [in] */ BOOL fRelease);
        
	STDMETHOD(EnumFormatEtc)( 
            /* [in] */ DWORD dwDirection,
            /* [out] */ IEnumFORMATETC **ppenumFormatEtc);
        
	STDMETHOD(DAdvise)( 
            /* [in] */ FORMATETC *pformatetc,
            /* [in] */ DWORD advf,
            /* [unique][in] */ IAdviseSink *pAdvSink,
            /* [out] */ DWORD *pdwConnection);
        
	STDMETHOD(DUnadvise)( 
            /* [in] */ DWORD dwConnection);
        
	STDMETHOD(EnumDAdvise)( 
            /* [out] */ IEnumSTATDATA **ppenumAdvise);

public:	// IDataObjectImpl support
	CComPtr<IDataAdviseHolder> m_spDataAdviseHolder;

public:
// Operations
	void Empty();   // empty cache (similar to ::EmptyClipboard)

	// CacheData & DelayRenderData operations similar to ::SetClipboardData
	void CacheGlobalData(CLIPFORMAT cfFormat, HGLOBAL hGlobal,
		LPFORMATETC lpFormatEtc = NULL);    // for HGLOBAL based data
	void DelayRenderFileData(CLIPFORMAT cfFormat,
		LPFORMATETC lpFormatEtc = NULL);    // for CVsFile* based delayed render

	// Clipboard and Drag/Drop access
#if 0
	DROPEFFECT DoDragDrop(
		DWORD dwEffects = DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK,
		LPCRECT lpRectStartDrag = NULL,
		COleDropSource* pDropSource = NULL);
#endif
	void SetClipboard();
	static void PASCAL FlushClipboard();
//	static CVCOleDataSource* PASCAL GetClipboardOwner();

	// Advanced: STGMEDIUM based cached data
	void CacheData(CLIPFORMAT cfFormat, LPSTGMEDIUM lpStgMedium,
		LPFORMATETC lpFormatEtc = NULL);    // for LPSTGMEDIUM based data
	// Advanced: STGMEDIUM or HGLOBAL based delayed render
	void DelayRenderData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);

	// Advanced: support for SetData in COleServerItem
	//  (not generally useful for clipboard or drag/drop operations)
	void DelaySetData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);

// Overidables
	virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal) { return FALSE; }
	virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CVCFile* pFile) { return FALSE; }
	virtual BOOL OnRenderData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium);
		// OnRenderFileData and OnRenderGlobalData are called by
		//  the default implementation of OnRenderData.
	virtual BOOL OnSetData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium, BOOL bRelease) { return FALSE; }
		// used only in COleServerItem implementation

// Implementation
public:

#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	VC_DATACACHE_ENTRY* m_pDataCache;  // data cache itself
	UINT m_nMaxSize;    // current allocated size
	UINT m_nSize;       // current size of the cache
	UINT m_nGrowBy;     // number of cache elements to grow by for new allocs

	VC_DATACACHE_ENTRY* Lookup(
		LPFORMATETC lpFormatEtc, DATADIR nDataDir) const;
	VC_DATACACHE_ENTRY* GetCacheEntry(
		LPFORMATETC lpFormatEtc, DATADIR nDataDir);

};

typedef CVCTypedPtrArray<CVCPtrArray, CVCNode*> CVCNodeArray;

//---------------------------------------------------------------------------
// Our DataSource wrapper
//---------------------------------------------------------------------------
class CVCProjDataSource : public CVCOleDataSource
{
public:
                        CVCProjDataSource() : m_pVCProjHier(NULL) {}

    void                SetNodeList(CVCArchy* pHier, CVCNodeArray& rgNodeList);
    const CVCNodeArray& GetNodeList() {return m_rgNodeList;}

    // Called to render our global data
	virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);

    // Registered clip formats
    static CLIPFORMAT   s_cfProjectDescriptor;  // Identifies the project which put the data on the clipboard.
    static CLIPFORMAT   s_cfStgProjItems;       // vsshell storage project items 
	static CLIPFORMAT	s_cfRefProjItems;		// vsshell reference items (like all VC items...)

protected:
    virtual ~CVCProjDataSource();
    void    CleanupNodes();
    
    CVCNodeArray	m_rgNodeList;
    CVCArchy*		m_pVCProjHier;
};
