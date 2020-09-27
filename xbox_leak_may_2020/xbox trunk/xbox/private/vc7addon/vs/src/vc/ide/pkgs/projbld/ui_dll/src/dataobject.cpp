// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "DataObject.h"
#include "PrjNode.h"
#include "vcarchy.h"
#include "bldpkg.h"

//---------------------------------------------------------------------------
// Clipboard stuff
//---------------------------------------------------------------------------

#define PROJECTCLIPDESCRIPTOR L"VX Clipboard Descriptor Format"
#define HTMLFORMAT            L"HTML Format"

// Register clip formats
CLIPFORMAT CVCProjDataSource::s_cfProjectDescriptor = RegisterClipboardFormatW(PROJECTCLIPDESCRIPTOR);
CLIPFORMAT CVCProjDataSource::s_cfStgProjItems = CF_VSSTGPROJECTITEMS;
CLIPFORMAT CVCProjDataSource::s_cfRefProjItems = CF_VSREFPROJECTITEMS;

/////////////////////////////////////////////////////////////////////////////
// CVCOleDataSource implementation

struct VC_DATACACHE_ENTRY
{
	FORMATETC m_formatEtc;
	STGMEDIUM m_stgMedium;
	DATADIR m_nDataDir;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class CVCCopyFormatEtc
{
public:
	static void init(FORMATETC *p)
	{
		p->cfFormat	= 0;
		p->ptd		= NULL;
		p->dwAspect	= 0;
		p->lindex	= 0;
		p->tymed	= 0;
	}
	static HRESULT copy(FORMATETC *pTo, FORMATETC *pFrom)
	{
		pTo->cfFormat	= pFrom->cfFormat;
		pTo->ptd		= _VCOleCopyTargetDevice(pFrom->ptd);
		pTo->dwAspect	= pFrom->dwAspect;
		pTo->lindex		= pFrom->lindex;
		pTo->tymed		= pFrom->tymed;
		return S_OK;
	}
	static void destroy(FORMATETC *p)
	{
		ASSERT(NULL != p);
		CoTaskMemFree(p->ptd);
	}
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class CVCEnumFormatEtc :
	 public CComEnum<IEnumFORMATETC, &IID_IEnumFORMATETC, FORMATETC, CVCCopyFormatEtc, CComSingleThreadModel>
{
protected:
	virtual ~CVCEnumFormatEtc(void) { };

public:
	CVCEnumFormatEtc(void);

	void AddFormat(const FORMATETC* lpFormatEtc);

protected:
	DWORD m_nSize, m_nMaxSize;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CVCEnumFormatEtc::CVCEnumFormatEtc(void) : m_nSize(0), m_nMaxSize(0)
{
	m_dwFlags |= BitOwn;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CVCEnumFormatEtc::AddFormat(const FORMATETC* lpFormatEtc)
{
	ASSERT(m_nSize <= m_nMaxSize);

	if (m_nSize == m_nMaxSize)
	{
		// not enough space for new item -- allocate more
		FORMATETC* pListNew = new FORMATETC[m_nSize+10];
		m_nMaxSize += 10;
		memcpy(pListNew, m_begin, m_nSize*sizeof(FORMATETC));
		m_iter = (m_iter - m_begin) + pListNew;
		delete m_begin;
		m_begin = pListNew;
		m_end = m_begin + m_nSize;
	}

	// add this item to the list
	ASSERT(m_nSize < m_nMaxSize);
	FORMATETC* pFormat = &m_begin[m_nSize];
	pFormat->cfFormat = lpFormatEtc->cfFormat;
	pFormat->ptd = lpFormatEtc->ptd;

	// Note: ownership of lpFormatEtc->ptd is transfered with this call.
	pFormat->dwAspect = lpFormatEtc->dwAspect;
	pFormat->lindex = lpFormatEtc->lindex;
	pFormat->tymed = lpFormatEtc->tymed;
	++m_nSize;
	++m_end;
}

/////////////////////////////////////////////////////////////////////////////
// CVCOleDataObject constructors

CVCOleDataObject::CVCOleDataObject()
{
	m_lpEnumerator = NULL;
	m_lpDataObject = NULL;
	m_bAutoRelease = TRUE;
	m_bClipboard = FALSE;
}

CVCOleDataObject::~CVCOleDataObject()
{
	Release();
}

void CVCOleDataObject::Attach(LPDATAOBJECT lpDataObject, BOOL bAutoRelease)
{
	ASSERT(lpDataObject != NULL);

	Release();  // detach previous
	m_lpDataObject = lpDataObject;
	m_bAutoRelease = bAutoRelease;
}

void CVCOleDataObject::Release()
{
	if (m_lpEnumerator != NULL)
	{
		m_lpEnumerator->Release();
		m_lpEnumerator = NULL;
	}

	if (m_lpDataObject != NULL)
	{
		if (m_bAutoRelease)
			m_lpDataObject->Release();
		m_lpDataObject = NULL;
	}
	m_bClipboard = FALSE;
}

LPDATAOBJECT CVCOleDataObject::Detach()
{
	EnsureClipboardObject();

	LPDATAOBJECT lpDataObject = m_lpDataObject;
	m_lpDataObject = NULL;  // detach without Release
	m_bClipboard = FALSE;

	return lpDataObject;
}

LPDATAOBJECT CVCOleDataObject::GetIDataObject(BOOL bAddRef)
{
	EnsureClipboardObject();

	LPDATAOBJECT lpDataObject = m_lpDataObject;
	if (bAddRef && lpDataObject != NULL)
		lpDataObject->AddRef();

	return lpDataObject;
}

/////////////////////////////////////////////////////////////////////////////
// CVCOleDataObject attributes

void CVCOleDataObject::BeginEnumFormats()
{
	EnsureClipboardObject();
	ASSERT(m_bClipboard || m_lpDataObject != NULL);

	// release old enumerator
	if (m_lpEnumerator)
	{
		m_lpEnumerator->Release();
		m_lpEnumerator = NULL;
	}

	if (m_lpDataObject == NULL)
		return;

	// get the new enumerator
	SCODE sc = m_lpDataObject->EnumFormatEtc(DATADIR_GET, &m_lpEnumerator);
	ASSERT(sc != S_OK || m_lpEnumerator != NULL);
	sc;
}

BOOL CVCOleDataObject::GetNextFormat(LPFORMATETC lpFormatEtc)
{
	ASSERT(m_bClipboard || m_lpDataObject != NULL);
	ASSERT(VCIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));

	// return FALSE if enumerator is already NULL
	RETURN_ON_NULL2(m_lpEnumerator, FALSE);

	// attempt to retrieve the next format with the enumerator
	SCODE sc = m_lpEnumerator->Next(1, lpFormatEtc, NULL);

	// if enumerator fails, stop the enumeration
	if (sc != S_OK)
	{
		if (m_lpEnumerator)
		{
			m_lpEnumerator->Release();
			m_lpEnumerator = NULL;
		}
		return FALSE;   // enumeration has ended
	}
	// otherwise, continue
	return TRUE;
}

CVCFile* CVCOleDataObject::GetFileData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
{
	EnsureClipboardObject();
	ASSERT(m_bClipboard || m_lpDataObject != NULL);
	RETURN_ON_NULL2(m_lpDataObject, NULL);

	ASSERT(lpFormatEtc == NULL ||
		VCIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));

	// fill in FORMATETC struct
	FORMATETC formatEtc;
	lpFormatEtc = _VCFillFormatEtc(lpFormatEtc, cfFormat, &formatEtc);
	formatEtc.tymed = TYMED_FILE|TYMED_MFPICT|TYMED_HGLOBAL|TYMED_ISTREAM;

	// attempt to get the data
	STGMEDIUM stgMedium;
	SCODE sc = m_lpDataObject->GetData(lpFormatEtc, &stgMedium);
	if (FAILED(sc))
		return NULL;

	// STGMEDIUMs with pUnkForRelease need to be copied first
	if (stgMedium.pUnkForRelease != NULL)
	{
		STGMEDIUM stgMediumDest;
		stgMediumDest.tymed = TYMED_NULL;
		stgMediumDest.pUnkForRelease = NULL;
		if (!_VCCopyStgMedium(lpFormatEtc->cfFormat, &stgMediumDest, &stgMedium))
		{
			::ReleaseStgMedium(&stgMedium);
			return NULL;
		}
		// release original and replace with new
		::ReleaseStgMedium(&stgMedium);
		stgMedium = stgMediumDest;
	}

	// convert it to a file, depending on data
	CStringA strFileName;
	CVCFile* pFile = NULL;
	{
		switch (stgMedium.tymed)
		{
		case TYMED_FILE:
			strFileName = stgMedium.lpszFileName;
			pFile = new CVCFile;
			if (!pFile->Open(strFileName,
				CVCFile::modeReadWrite|CVCFile::shareExclusive))
			{
				delete pFile;
				pFile = NULL;
				break;
			}
			// caller is responsible for deleting the actual file,
			//  but we free the file name.
			CoTaskMemFree(stgMedium.lpszFileName);
			break;

		case TYMED_MFPICT:
		case TYMED_HGLOBAL:
			ASSERT(!_T("port CSharedFile"));
			pFile = new CVCSharedFile;
			((CVCSharedFile*)pFile)->SetHandle(stgMedium.hGlobal);
			break;

		case TYMED_ISTREAM:
			ASSERT(!_T("port COleStreamFile"));
#if 0
			pFile = new COleStreamFile(stgMedium.pstm);
#endif
			break;

		default:
			// type not supported, so return error
			::ReleaseStgMedium(&stgMedium);
			break;
		}
	}
	// store newly created CVCFile* and return
	return pFile;
}

HGLOBAL CVCOleDataObject::GetGlobalData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
{
	EnsureClipboardObject();
	ASSERT(m_bClipboard || m_lpDataObject != NULL);
	RETURN_ON_NULL2(m_lpDataObject, NULL);

	ASSERT(lpFormatEtc == NULL || VCIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));

	// fill in FORMATETC struct
	FORMATETC formatEtc;
	BOOL bFillFormatEtc = (lpFormatEtc == NULL);
	lpFormatEtc = _VCFillFormatEtc(lpFormatEtc, cfFormat, &formatEtc);
	if (bFillFormatEtc)
		lpFormatEtc->tymed = TYMED_HGLOBAL|TYMED_MFPICT;
	ASSERT((lpFormatEtc->tymed & (TYMED_HGLOBAL|TYMED_MFPICT)) != 0);

	// attempt to get the data
	STGMEDIUM stgMedium;
	SCODE sc = m_lpDataObject->GetData(lpFormatEtc, &stgMedium);
	if (FAILED(sc))
		return FALSE;

	// handle just hGlobal types
	switch (stgMedium.tymed)
	{
	case TYMED_MFPICT:
	case TYMED_HGLOBAL:
		RETURN_ON_NULL2(stgMedium.pUnkForRelease, stgMedium.hGlobal);
		STGMEDIUM stgMediumDest;
		stgMediumDest.tymed = TYMED_NULL;
		stgMediumDest.pUnkForRelease = NULL;
		if (!_VCCopyStgMedium(lpFormatEtc->cfFormat, &stgMediumDest, &stgMedium))
		{
			::ReleaseStgMedium(&stgMedium);
			return NULL;
		}
		::ReleaseStgMedium(&stgMedium);
		return stgMediumDest.hGlobal;

	// default -- falls through to error condition...
	}

	::ReleaseStgMedium(&stgMedium);
	return NULL;
}

BOOL CVCOleDataObject::GetData(CLIPFORMAT cfFormat, LPSTGMEDIUM lpStgMedium,
	LPFORMATETC lpFormatEtc)
{
	EnsureClipboardObject();
	ASSERT(m_bClipboard || m_lpDataObject != NULL);
	RETURN_ON_NULL2(m_lpDataObject, FALSE);
	ASSERT(VCIsValidAddress(lpStgMedium, sizeof(STGMEDIUM), FALSE));

	// fill in FORMATETC struct
	FORMATETC formatEtc;
	lpFormatEtc = _VCFillFormatEtc(lpFormatEtc, cfFormat, &formatEtc);

	// attempt to get the data
	SCODE sc = m_lpDataObject->GetData(lpFormatEtc, lpStgMedium);
	if (FAILED(sc))
		return FALSE;

	return TRUE;
}

BOOL CVCOleDataObject::IsDataAvailable(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
{
	if (m_bClipboard)
	{
		// it is faster and more reliable to ask the real Win32 clipboard
		//  instead of the OLE clipboard.
		return ::IsClipboardFormatAvailable(cfFormat);
	}
	else
	{
		ASSERT(m_lpDataObject != NULL);
		ASSERT(lpFormatEtc == NULL || VCIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));

		// fill in FORMATETC struct
		FORMATETC formatEtc;
		lpFormatEtc = _VCFillFormatEtc(lpFormatEtc, cfFormat, &formatEtc);

		// attempt to get the data
		return m_lpDataObject->QueryGetData(lpFormatEtc) == S_OK;
	}
}

/////////////////////////////////////////////////////////////////////////////
// clipboard API wrappers

BOOL CVCOleDataObject::AttachClipboard()
{
	ASSERT(VCIsValidAddress(this, sizeof(CVCOleDataObject)));
	ASSERT(m_lpDataObject == NULL); // need to call release?
	ASSERT(!m_bClipboard); // already attached to clipboard?

	// set special "clipboard" flag for optimizations
	m_bClipboard = TRUE;
	return TRUE;
}

void CVCOleDataObject::EnsureClipboardObject()
{
	ASSERT(VCIsValidAddress(this, sizeof(CVCOleDataObject)));

	if (m_bClipboard && m_lpDataObject == NULL)
	{
		// get clipboard using OLE API
		LPDATAOBJECT lpDataObject;
		SCODE sc = ::OleGetClipboard(&lpDataObject);

		// attach CVCOleDataObject wrapper to IDataObject from clipboard
		if (sc == S_OK)
			Attach(lpDataObject, TRUE);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	// attempt to find match in the cache
	VC_DATACACHE_ENTRY* pCache = this->Lookup(pformatetcIn, DATADIR_GET);
	RETURN_ON_NULL2(pCache, DATA_E_FORMATETC);

	// use cache if entry is not delay render
	memset(pmedium, 0, sizeof(STGMEDIUM));
	if (pCache->m_stgMedium.tymed != TYMED_NULL)
	{
		// Copy the cached medium into the lpStgMedium provided by caller.
		if (!_VCCopyStgMedium(pformatetcIn->cfFormat, pmedium, &pCache->m_stgMedium))
			return DATA_E_FORMATETC;

		// format was supported for copying
		return S_OK;
	}

	SCODE sc = DATA_E_FORMATETC;

	// attempt LPSTGMEDIUM based delay render
	if (OnRenderData(pformatetcIn, pmedium))
		sc = S_OK;
	return sc;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
	// these two must be the same
	ASSERT(pformatetc->tymed == pmedium->tymed);
	pformatetc->tymed = pmedium->tymed;    // but just in case...

	// attempt to find match in the cache
	VC_DATACACHE_ENTRY* pCache = this->Lookup(pformatetc, DATADIR_GET);
	RETURN_ON_NULL2(pCache, DATA_E_FORMATETC);

	// handle cached medium and copy
	if (pCache->m_stgMedium.tymed != TYMED_NULL)
	{
		// found a cached format -- copy it to dest medium
		ASSERT(pCache->m_stgMedium.tymed == pmedium->tymed);
		if (!_VCCopyStgMedium(pformatetc->cfFormat, pmedium, &pCache->m_stgMedium))
			return DATA_E_FORMATETC;

		// format was supported for copying
		return S_OK;
	}

	SCODE sc = DATA_E_FORMATETC;
	// attempt pmedium based delay render
	if (OnRenderData(pformatetc, pmedium))
		sc = S_OK;
	return sc;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	*ppenumFormatEtc = NULL;

	// generate a format list from the cache
	CComObject<CVCEnumFormatEtc> *pFormatList = NULL;
	HRESULT hr = CComObject<CVCEnumFormatEtc>::CreateInstance(&pFormatList);
	if (SUCCEEDED(hr))
	{
		for (UINT nIndex = 0; nIndex < m_nSize; nIndex++)
		{
			VC_DATACACHE_ENTRY* pCache = &m_pDataCache[nIndex];
			if ((DWORD)pCache->m_nDataDir & dwDirection)
			{
				// entry should be enumerated -- add it to the list
				FORMATETC formatEtc;
				_VCOleCopyFormatEtc(&formatEtc, &pCache->m_formatEtc);
				pFormatList->AddFormat(&formatEtc);
			}
		}
	}
	(*ppenumFormatEtc) = pFormatList;
	(*ppenumFormatEtc)->AddRef();
	return hr;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
	ATLTRACE(_T("IDataObjectImpl::EnumDAdvise\n"));
	HRESULT hr = E_FAIL;
	if (m_spDataAdviseHolder != NULL)
		hr = m_spDataAdviseHolder->EnumAdvise(ppenumAdvise);
	return hr;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::DUnadvise(DWORD dwConnection)
{
	ATLTRACE(_T("IDataObjectImpl::DUnadvise\n"));
	RETURN_ON_NULL2(m_spDataAdviseHolder, OLE_E_NOCONNECTION);
	return m_spDataAdviseHolder->Unadvise(dwConnection);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	ATLTRACE(_T("IDataObjectImpl::DAdvise\n"));
	HRESULT hr = S_OK;
	if (this->m_spDataAdviseHolder == NULL)
		hr = CreateDataAdviseHolder(&m_spDataAdviseHolder);

	if (hr == S_OK)
		hr = m_spDataAdviseHolder->Advise((IDataObject*)this, pformatetc, advf, pAdvSink, pdwConnection);

	return hr;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	ASSERT(pformatetc->tymed == pmedium->tymed);

	// attempt to find match in the cache
	VC_DATACACHE_ENTRY* pCache = Lookup(pformatetc, DATADIR_SET);
	RETURN_ON_NULL2(pCache, DATA_E_FORMATETC);
	ASSERT(pCache->m_stgMedium.tymed == TYMED_NULL);

	SCODE sc = E_UNEXPECTED;

	// attempt pmedium based SetData
	if (OnSetData(pformatetc, pmedium, fRelease))
		sc = S_OK;
	return sc;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
STDMETHODIMP CVCOleDataSource::QueryGetData(FORMATETC *pformatetc)
{
	// attempt to find match in the cache
	VC_DATACACHE_ENTRY* pCache = Lookup(pformatetc, DATADIR_GET);
	RETURN_ON_NULL2(pCache, DATA_E_FORMATETC);

	// it was found in the cache or can be rendered -- success
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CVCOleDataSource cache implementation

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
VC_DATACACHE_ENTRY* CVCOleDataSource::Lookup(LPFORMATETC lpFormatEtc, DATADIR nDataDir) const
{
	VC_DATACACHE_ENTRY* pLast = NULL;
	// look for suitable match to lpFormatEtc in cache
	for (UINT nIndex = 0; nIndex < m_nSize; nIndex++)
	{
		// get entry from cache at nIndex
		VC_DATACACHE_ENTRY* pCache = &m_pDataCache[nIndex];
		FORMATETC *pCacheFormat = &pCache->m_formatEtc;

		// check for match
		if (pCacheFormat->cfFormat == lpFormatEtc->cfFormat && (pCacheFormat->tymed & lpFormatEtc->tymed) != 0 &&
			pCacheFormat->lindex == lpFormatEtc->lindex && pCacheFormat->dwAspect == lpFormatEtc->dwAspect &&
			pCache->m_nDataDir == nDataDir)
		{
			// for backward compatibility we match even if we never
			// find an exact match for the DVTARGETDEVICE
			DVTARGETDEVICE* ptd1 = pCacheFormat->ptd;
			DVTARGETDEVICE* ptd2 = lpFormatEtc->ptd;
			pLast = pCache;
			if ( ((ptd1 == NULL) && (ptd2 == NULL)) || ((ptd1 != NULL) && (ptd2 != NULL) && (ptd1->tdSize == ptd2->tdSize) &&
				(memcmp(ptd1, ptd2, ptd1->tdSize)==0)))
			{
				// exact match, so break now and return it
				break;
			}
			// continue looking for better match
		}
	}

	return pLast;    // not found
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CVCOleDataSource::OnRenderData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium)
{
	// attempt TYMED_HGLOBAL as prefered format
	if (lpFormatEtc->tymed & TYMED_HGLOBAL)
	{
		// attempt HGLOBAL delay render hook
		HGLOBAL hGlobal = lpStgMedium->hGlobal;
		if (OnRenderGlobalData(lpFormatEtc, &hGlobal))
		{
			ASSERT(lpStgMedium->tymed != TYMED_HGLOBAL || (lpStgMedium->hGlobal == hGlobal));
			ASSERT(hGlobal != NULL);
			lpStgMedium->tymed = TYMED_HGLOBAL;
			lpStgMedium->hGlobal = hGlobal;
			return TRUE;
		}

		// attempt CVsFile* based delay render hook
		CVCSharedFile file;
		if (lpStgMedium->tymed == TYMED_HGLOBAL)
		{
			ASSERT(lpStgMedium->hGlobal != NULL);
			file.SetHandle(lpStgMedium->hGlobal, FALSE);
		}
		if (OnRenderFileData(lpFormatEtc, &file))
		{
			lpStgMedium->tymed = TYMED_HGLOBAL;
			lpStgMedium->hGlobal = file.Detach();
			ASSERT(lpStgMedium->hGlobal != NULL);
			return TRUE;
		}
		if (lpStgMedium->tymed == TYMED_HGLOBAL)
			file.Detach();
	}

	// attempt TYMED_ISTREAM format
	if (lpFormatEtc->tymed & TYMED_ISTREAM)
	{
		ASSERT(!_T("port COleStreamFile"));
#if 0
		COleStreamFile file;
		if (lpStgMedium->tymed == TYMED_ISTREAM)
		{
			ASSERT(lpStgMedium->pstm != NULL);
			file.Attach(lpStgMedium->pstm);
		}
		else
		{
			if (!file.CreateMemoryStream())
				return FALSE;
		}

		// get data into the stream
		if (OnRenderFileData(lpFormatEtc, &file))
		{
			lpStgMedium->tymed = TYMED_ISTREAM;
			lpStgMedium->pstm = file.Detach();
			return TRUE;
		}
		if (lpStgMedium->tymed == TYMED_ISTREAM)
			file.Detach();
#endif //0
	}

	return FALSE;   // default does nothing
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CVCOleDataSource::CVCOleDataSource(void)
{
	m_pDataCache = NULL;
	m_nMaxSize = 0;
	m_nSize = 0;
	m_nGrowBy = 10;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CVCOleDataSource::~CVCOleDataSource(void)
{
	// clear clipboard source if this object was on the clipboard
#if 0
	_AFX_OLE_STATE* pOleState = _afxOleState;
	if (this == pOleState->m_pClipboardSource)
		pOleState->m_pClipboardSource = NULL;
#endif
	// free the clipboard data cache
	Empty();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CVCOleDataSource::Empty()
{
	if (m_pDataCache != NULL)
	{
		ASSERT(m_nMaxSize != 0);
		ASSERT(m_nSize != 0);

		// release all of the STGMEDIUMs and FORMATETCs
		for (UINT nIndex = 0; nIndex < m_nSize; nIndex++)
		{
			::CoTaskMemFree(m_pDataCache[nIndex].m_formatEtc.ptd);
			::ReleaseStgMedium(&m_pDataCache[nIndex].m_stgMedium);
		}

		// delete the cache
		delete[] m_pDataCache;
		m_pDataCache = NULL;
		m_nMaxSize = 0;
		m_nSize = 0;
	}
	ASSERT(m_pDataCache == NULL);
	ASSERT(m_nMaxSize == 0);
	ASSERT(m_nSize == 0);
}

/////////////////////////////////////////////////////////////////////////////
// CVCOleDataSource clipboard API wrappers

void CVCOleDataSource::SetClipboard(void)
{
	// attempt OLE set clipboard operation
	SCODE sc = ::OleSetClipboard(this);
	ASSERT(S_OK == sc);

	// success - set as current clipboard source
	ASSERT(::OleIsCurrentClipboard(this) == S_OK);
}

// for LPSTGMEDIUM or HGLOBAL based delayed render
void CVCOleDataSource::DelayRenderData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
{
	ASSERT(lpFormatEtc == NULL || VCIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));

	// fill in FORMATETC struct
	FORMATETC formatEtc;
	if (lpFormatEtc == NULL)
	{
		lpFormatEtc = _VCFillFormatEtc(lpFormatEtc, cfFormat, &formatEtc);
		lpFormatEtc->tymed = TYMED_HGLOBAL;
	}
	// ensure that cfFormat member is set
	if (cfFormat != 0)
		lpFormatEtc->cfFormat = cfFormat;

	// add it to the cache
	VC_DATACACHE_ENTRY* pEntry = GetCacheEntry(lpFormatEtc, DATADIR_GET);
	memset(&pEntry->m_stgMedium, 0, sizeof pEntry->m_stgMedium);
}

/////////////////////////////////////////////////////////////////////////////
// CVCOleDataSource cache allocation

VC_DATACACHE_ENTRY* CVCOleDataSource::GetCacheEntry(LPFORMATETC lpFormatEtc, DATADIR nDataDir)
{
	VC_DATACACHE_ENTRY* pEntry = Lookup(lpFormatEtc, nDataDir);
	if (pEntry != NULL)
	{
		// cleanup current entry and return it
		::CoTaskMemFree(pEntry->m_formatEtc.ptd);
		::ReleaseStgMedium(&pEntry->m_stgMedium);
	}
	else
	{
		// allocate space for item at m_nSize (at least room for 1 item)
		if (m_pDataCache == NULL || m_nSize == m_nMaxSize)
		{
			ASSERT(m_nGrowBy != 0);
			VC_DATACACHE_ENTRY* pCache = new VC_DATACACHE_ENTRY[m_nMaxSize+m_nGrowBy];
			m_nMaxSize += m_nGrowBy;
			if (m_pDataCache != NULL)
			{
				memcpy(pCache, m_pDataCache, m_nSize * sizeof(VC_DATACACHE_ENTRY));
				delete[] m_pDataCache;
			}
			m_pDataCache = pCache;
		}
		ASSERT(m_pDataCache != NULL);
		ASSERT(m_nMaxSize != 0);

		pEntry = &m_pDataCache[m_nSize++];
	}

	// fill the cache entry with the format and data direction and return it
	pEntry->m_nDataDir = nDataDir;
	pEntry->m_formatEtc = *lpFormatEtc;
	return pEntry;
}

// for HGLOBAL based cached render
void CVCOleDataSource::CacheGlobalData(CLIPFORMAT cfFormat, HGLOBAL hGlobal, LPFORMATETC lpFormatEtc)
{
	ASSERT(hGlobal != NULL);
	ASSERT(lpFormatEtc == NULL || VCIsValidAddress(lpFormatEtc, sizeof(FORMATETC), FALSE));

	// fill in FORMATETC struct
	FORMATETC formatEtc;
	lpFormatEtc = _VCFillFormatEtc(lpFormatEtc, cfFormat, &formatEtc);
	lpFormatEtc->tymed = TYMED_HGLOBAL;

	// add it to the cache
	VC_DATACACHE_ENTRY* pEntry = GetCacheEntry(lpFormatEtc, DATADIR_GET);
	pEntry->m_stgMedium.tymed = TYMED_HGLOBAL;
	pEntry->m_stgMedium.hGlobal = hGlobal;
	pEntry->m_stgMedium.pUnkForRelease = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CVCOleDataSource diagnostics

#ifdef _DEBUG
void CVCOleDataSource::AssertValid() const
{
//	CCmdTarget::AssertValid();
	ASSERT(m_nSize <= m_nMaxSize);
	ASSERT(m_nMaxSize != 0 || m_pDataCache == NULL);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Free our hold on the nodes
//---------------------------------------------------------------------------
CVCProjDataSource::~CVCProjDataSource() 
{ 
    CleanupNodes();
}

//---------------------------------------------------------------------------
// We copy the nodes in rgNodeList to our list and addref them so they don't go
// away.
//---------------------------------------------------------------------------
void CVCProjDataSource::SetNodeList(CVCArchy* pHier, CVCNodeArray& rgNodeList)
{   // Addref all the nodes
	ASSERT(m_pVCProjHier == NULL);
	m_pVCProjHier = pHier;
	CleanupNodes();
	m_rgNodeList.SetSize(rgNodeList.GetSize());
	for (int i = 0; i < rgNodeList.GetSize(); i++)
	{
		m_rgNodeList[i] = rgNodeList[i];
		m_rgNodeList[i]->AddRef();
	}
}

//---------------------------------------------------------------------------
// Free our hold on the nodes
//---------------------------------------------------------------------------
void CVCProjDataSource::CleanupNodes()
{
    for (int i = 0; i < m_rgNodeList.GetSize(); i++)
        m_rgNodeList[i]->Release();

    m_rgNodeList.RemoveAll();
}

//---------------------------------------------------------------------------
// Called to render our dragdrop format. By delaying the rendering it allows
// us to get the files local for VC projects other than the one that sourced it.
//---------------------------------------------------------------------------
BOOL CVCProjDataSource::OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal)
{
    BOOL bRet = FALSE;
    ASSERT(lpFormatEtc && phGlobal);
    if(lpFormatEtc && phGlobal && lpFormatEtc->cfFormat == s_cfRefProjItems && (lpFormatEtc->tymed & TYMED_HGLOBAL))
    {   // All right, it's our format. Walk the node list building up the projref 
        // format for each item.
        CComPtr<IVsSolution> spSolution;
		GetBuildPkg()->GetIVsSolution(&spSolution);
        ASSERT(spSolution);
        CVCSharedFile dataFile2(GMEM_DDESHARE|GMEM_MOVEABLE, 512);
        DROPFILES dropFiles = {sizeof(DROPFILES), {0, 0}, FALSE, TRUE};
        BOOL bSuccess = dataFile2.Write(&dropFiles, sizeof(DROPFILES));
        ASSERT(bSuccess);
        for (int i = 0; i < m_rgNodeList.GetSize(); i++)
        {   
            BOOL bAddedItem = FALSE;
            CVCNode* pNode = m_rgNodeList[i];
            // Make sure node hasn't been zombied in the mean time.
            if(!pNode->IsZombie())
            {
                CComBSTR bstrProjref;
                spSolution->GetProjrefOfItem(m_pVCProjHier->GetIVsHierarchy(), pNode->GetVsItemID(), &bstrProjref);
                if(bstrProjref)
                {   // This is a good one
                    bSuccess = dataFile2.Write(bstrProjref, sizeof(WCHAR) * (lstrlenW(bstrProjref)+1));
                    ASSERT(bSuccess);
                    bAddedItem = TRUE;
                }
            }
            if (!bAddedItem)
            {   // Remove this node from our list of dragged items
                m_rgNodeList.RemoveAt(i);
                --i;
            }
        }
        // Only cache the format if we actually added at least one file (otherwise we simply let
        // the sharedFile release the HGlobal.
        if(m_rgNodeList.GetSize())
        {   // Write trailer
            WCHAR szTerm[2] = {0,0};
            dataFile2.Write(szTerm, 2*sizeof(WCHAR));

            // Return the hGlobal. 
            *phGlobal = dataFile2.Detach();
            bRet = TRUE;
        }
    }
    return bRet;
}

//---------------------------------------------------------------------------
// Helper for creating default FORMATETC from cfFormat
//---------------------------------------------------------------------------
LPFORMATETC _VCFillFormatEtc(LPFORMATETC lpFormatEtc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtcFill)
{
	ASSERT(lpFormatEtcFill != NULL);
	if (lpFormatEtc == NULL && cfFormat != 0)
	{
		lpFormatEtc = lpFormatEtcFill;
		lpFormatEtc->cfFormat = cfFormat;
		lpFormatEtc->ptd = NULL;
		lpFormatEtc->dwAspect = DVASPECT_CONTENT;
		lpFormatEtc->lindex = -1;
		lpFormatEtc->tymed = (DWORD) -1;
	}
	return lpFormatEtc;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static HGLOBAL CopyGlobalMemory(HGLOBAL hDest, HGLOBAL hSource)
{
	ASSERT(hSource != NULL);

	// make sure we have suitable hDest
	ASSERT(::GlobalSize(hSource) < INT_MAX);
	DWORD nSize = (LONG32) (::GlobalSize(hSource));
	if (hDest == NULL)
	{
		hDest = ::GlobalAlloc(GMEM_SHARE|GMEM_MOVEABLE, nSize);
		RETURN_ON_NULL2(hDest, NULL);
	}
	else if (nSize > ::GlobalSize(hDest))
	{
		// hDest is not large enough
		return NULL;
	}

	// copy the bits
	LPVOID lpSource = ::GlobalLock(hSource);
	LPVOID lpDest = ::GlobalLock(hDest);
	ASSERT(lpDest != NULL);
	ASSERT(lpSource != NULL);
	memcpy(lpDest, lpSource, nSize);
	::GlobalUnlock(hDest);
	::GlobalUnlock(hSource);

	// success -- return hDest
	return hDest;
}

//---------------------------------------------------------------------------
BOOL _VCCopyStgMedium(CLIPFORMAT cfFormat, LPSTGMEDIUM lpDest, LPSTGMEDIUM lpSource)
{
	if (lpDest->tymed == TYMED_NULL)
	{
		ASSERT(lpSource->tymed != TYMED_NULL);
		switch (lpSource->tymed)
		{
		case TYMED_ENHMF:
		case TYMED_HGLOBAL:
			ASSERT(sizeof(HGLOBAL) == sizeof(HENHMETAFILE));
			lpDest->tymed = lpSource->tymed;
			lpDest->hGlobal = NULL;
			break;  // fall through to CopyGlobalMemory case

		case TYMED_ISTREAM:
			lpDest->pstm = lpSource->pstm;
			lpDest->pstm->AddRef();
			lpDest->tymed = TYMED_ISTREAM;
			return TRUE;

		case TYMED_ISTORAGE:
			lpDest->pstg = lpSource->pstg;
			lpDest->pstg->AddRef();
			lpDest->tymed = TYMED_ISTORAGE;
			return TRUE;

		case TYMED_MFPICT:
			{
				// copy LPMETAFILEPICT struct + embedded HMETAFILE
				HGLOBAL hDest = ::CopyGlobalMemory(NULL, lpSource->hGlobal);
				RETURN_ON_NULL2(hDest, FALSE);
				LPMETAFILEPICT lpPict = (LPMETAFILEPICT)::GlobalLock(hDest);
				ASSERT(lpPict != NULL);
				lpPict->hMF = ::CopyMetaFile(lpPict->hMF, NULL);
				if (lpPict->hMF == NULL)
				{
					::GlobalUnlock(hDest);
					::GlobalFree(hDest);
					return FALSE;
				}
				::GlobalUnlock(hDest);

				// fill STGMEDIUM struct
				lpDest->hGlobal = hDest;
				lpDest->tymed = TYMED_MFPICT;
			}
			return TRUE;

		case TYMED_GDI:
			lpDest->tymed = TYMED_GDI;
			lpDest->hGlobal = NULL;
			break;

		case TYMED_FILE:
			{
				USES_CONVERSION;
				lpDest->tymed = TYMED_FILE;
				ASSERT(lpSource->lpszFileName != NULL);
				UINT cbSrc = (int)ocslen(lpSource->lpszFileName);
				LPOLESTR szFileName = (LPOLESTR)CoTaskMemAlloc(cbSrc*sizeof(OLECHAR));
				lpDest->lpszFileName = szFileName;
				RETURN_ON_NULL2(szFileName, FALSE);
				memcpy(szFileName, lpSource->lpszFileName,  (cbSrc+1)*sizeof(OLECHAR));
				return TRUE;
			}

		// unable to create + copy other TYMEDs
		default:
			return FALSE;
		}
	}
	ASSERT(lpDest->tymed == lpSource->tymed);

	switch (lpSource->tymed)
	{
	case TYMED_HGLOBAL:
		{
			HGLOBAL hDest = ::CopyGlobalMemory(lpDest->hGlobal,
			lpSource->hGlobal);
			RETURN_ON_NULL2(hDest, FALSE);
			lpDest->hGlobal = hDest;
		}
		return TRUE;

	case TYMED_ISTREAM:
		{
			ASSERT(lpDest->pstm != NULL);
			ASSERT(lpSource->pstm != NULL);

			// get the size of the source stream
			STATSTG stat;
			if (lpSource->pstm->Stat(&stat, STATFLAG_NONAME) != S_OK)
			{
				// unable to get size of source stream
				return FALSE;
			}
			ASSERT(stat.pwcsName == NULL);

			// always seek to zero before copy
			LARGE_INTEGER zero = { 0, 0 };
			lpDest->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
			lpSource->pstm->Seek(zero, STREAM_SEEK_SET, NULL);

			// copy source to destination
			if ( lpSource->pstm->CopyTo(lpDest->pstm, stat.cbSize, NULL, NULL) != S_OK )
			{
				// copy from source to dest failed
				return FALSE;
			}

			// always seek to zero after copy
			lpDest->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
			lpSource->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
		}
		return TRUE;

	case TYMED_ISTORAGE:
		{
			ASSERT(lpDest->pstg != NULL);
			ASSERT(lpSource->pstg != NULL);

			// just copy source to destination
			if (lpSource->pstg->CopyTo(0, NULL, NULL, lpDest->pstg) != S_OK)
				return FALSE;
		}
	return TRUE;

	case TYMED_FILE:
		{
			USES_CONVERSION;
			ASSERT(lpSource->lpszFileName != NULL);
			ASSERT(lpDest->lpszFileName != NULL);
			return CopyFile(OLE2T(lpSource->lpszFileName), OLE2T(lpDest->lpszFileName), FALSE);
		}


	case TYMED_ENHMF:
	case TYMED_GDI:
		{
			ASSERT(sizeof(HGLOBAL) == sizeof(HENHMETAFILE));

			// with TYMED_GDI cannot copy into existing HANDLE
			if (lpDest->hGlobal != NULL)
				return FALSE;

			// otherwise, use OleDuplicateData for the copy
			lpDest->hGlobal = OleDuplicateData(lpSource->hGlobal, cfFormat, 0);
			RETURN_ON_NULL2(lpDest->hGlobal, FALSE);
		}
		return TRUE;

	// other TYMEDs cannot be copied
	default:
		return FALSE;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DVTARGETDEVICE* _VCOleCopyTargetDevice(DVTARGETDEVICE* ptdSrc)
{
	RETURN_ON_NULL2(ptdSrc, NULL);

	DVTARGETDEVICE* ptdDest = (DVTARGETDEVICE*)CoTaskMemAlloc(ptdSrc->tdSize);
	RETURN_ON_NULL2(ptdDest, NULL);

	memcpy(ptdDest, ptdSrc, (size_t)ptdSrc->tdSize);
	return ptdDest;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _VCOleCopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc)
{
	ASSERT(petcDest != NULL);
	ASSERT(petcSrc != NULL);

	petcDest->cfFormat = petcSrc->cfFormat;
	petcDest->ptd = _VCOleCopyTargetDevice(petcSrc->ptd);
	petcDest->dwAspect = petcSrc->dwAspect;
	petcDest->lindex = petcSrc->lindex;
	petcDest->tymed = petcSrc->tymed;
}

