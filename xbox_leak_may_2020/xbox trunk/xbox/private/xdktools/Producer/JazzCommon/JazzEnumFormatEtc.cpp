// JazzEnumFormatEtc.cpp : Implementation of CJazzEnumFormatEtc

#include <afxole.h>         // MFC OLE classes

#include "JazzEnumFormatEtc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc Constructors/Destructor

CJazzEnumFormatEtc::CJazzEnumFormatEtc()
{
	m_pFormatList		= NULL;
	m_pCurrentFormat	= NULL;
	m_dwRef				= 0;
	AddRef();
}

CJazzEnumFormatEtc::~CJazzEnumFormatEtc()
{
	RemoveAllFormats();
}

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc::Initialize
//
//	This method is called from Clone to initialize the CJazzEnumFormatEtc from
//	another CJazzEnumFormatEtc.  If pJEF is NULL, this object is initialized to
//	an empty state.
//
//	Return values:
//		S_OK:				Initialization was successful.
//		E_UNEXPECTED:		An unexpected error occurred.
//		E_OUTOFMEMORY:		Out of memory.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::Initialize(CJazzEnumFormatEtc *pJEF)
{
	FormatList *pFLThis;
	FormatList *pFLOther;
	FormatList *pFLReverse;

	// Set our current format pointer to NULL
	m_pCurrentFormat = NULL;

	// Delete our current format list.
	RemoveAllFormats();
	ASSERT(m_pFormatList == NULL);
	if(m_pFormatList != NULL)
	{
		return E_UNEXPECTED;
	}

	// If pJEF is NULL, make the CJazzEnumFormatEtc empty and return
	if(pJEF == NULL)
	{
		return S_OK;
	}

	// Add the formats in pJEF's list to our list.
	pFLOther = pJEF->m_pFormatList;
	while(pFLOther != NULL)
	{
		// Allocate a new FormatList entry and FORMATETC
		pFLThis = new FormatList;
		if(pFLThis == NULL)
		{
			return E_OUTOFMEMORY;
		}

		pFLThis->pFormat = new FORMATETC;
		if(pFLThis->pFormat == NULL)
		{
			delete pFLThis;
			return E_OUTOFMEMORY;
		}

		// Copy the FORMATETC from pJEF to the new entry.
		memcpy(pFLThis->pFormat, pFLOther->pFormat, sizeof(FORMATETC));

		// Add this entry to the head of the list.
		pFLThis->pNext = m_pFormatList;
		m_pFormatList = pFLThis;

		// If this is the current format in pJEF, make it the current format in our list.
		if(pJEF->m_pCurrentFormat == pFLOther)
		{
			m_pCurrentFormat = pFLThis;
		}
		pFLOther = pFLOther->pNext;
	}

	// Reverse the list
	pFLThis = m_pFormatList;
	m_pFormatList = NULL;
	while(pFLThis != NULL)
	{
		pFLReverse = pFLThis;
		pFLThis = pFLThis->pNext;
		pFLReverse->pNext = m_pFormatList;
		m_pFormatList = pFLReverse;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc IUnknown implementation

HRESULT CJazzEnumFormatEtc::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
    if( ::IsEqualIID(riid, IID_IEnumFORMATETC)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IEnumFORMATETC *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CJazzEnumFormatEtc::AddRef()
{
    return ++m_dwRef;
}

ULONG CJazzEnumFormatEtc::Release()
{
    ASSERT(m_dwRef != 0);

    --m_dwRef;

    if(m_dwRef == 0)
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc IEnumFORMATETC implementation

/////////////////////////////////////////////////////////////////////////////
// IEnumFORMATETC::Next
//
//	Get the next celt elements from the format list.
//
//	Return values:
//		S_OK:			The next celt elements were returned.
//		S_FALSE:		Fewer than celt elements were returned.
//		E_INVALIDARG:	rgelt was NULL and at least one element was requested.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::Next(ULONG celt, FORMATETC __RPC_FAR *rgelt, ULONG __RPC_FAR *pceltFetched)
{
	ULONG ulFetched = 0;

	if(rgelt == NULL && celt > 0)
	{
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	// As long as we haven't fetched celt elements, continue to fetch them.
	while(ulFetched < celt)
	{
		if(m_pCurrentFormat == NULL)
		{
			// No more formats to fetch.
			break;
		}

		// Copy the current format to the correct position in rgelt
		memcpy(&(rgelt[ulFetched]), m_pCurrentFormat->pFormat, sizeof(FORMATETC));
		m_pCurrentFormat = m_pCurrentFormat->pNext;
		ulFetched++;
	}

	// Fill in pceltFetched if it is provided.
	if(pceltFetched)
	{
		*pceltFetched = ulFetched;
	}

	if(celt == ulFetched)
	{
		// The requested number of items were fetched.
		return S_OK;
	}

	// Fewer than celt items remained in the list.
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IEnumFORMATETC::Skip
//
//	Skip the specified number of elements.
//
//	Return values:
//		S_OK:		celt elements were skipped.
//		S_FALSE:	Fewer than the requested number of elements were skipped.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::Skip(ULONG celt)
{
	ULONG ulSkipped = 0;

	// Continue skipping elements until either celt have been skipped or 
	// there are no more to skip.
	while(ulSkipped < celt && m_pCurrentFormat != NULL)
	{
		m_pCurrentFormat = m_pCurrentFormat->pNext;
		ulSkipped++;
	}

	if(celt == ulSkipped)
	{
		// The requested number of elements were skipped.
		return S_OK;
	}
	// Fewer than celt items remained in the list.
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IEnumFORMATETC::Reset
//
//	Set the current format to the first format.
//
//	Return values: S_OK
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::Reset(void)
{
	m_pCurrentFormat = m_pFormatList;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IEnumFORMATETC::Clone
//
//	This method returns a duplicate of this CJazzEnumFormatEtc with the
//	current format pointer also at the same location as the original.
//
//	Return values: standard error codes and the following
//		S_OK:	The object was successfully cloned.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::Clone(IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenum)
{
	CJazzEnumFormatEtc *pJEFNew;

	// Make sure ppenum != NULL
	ASSERT(ppenum != NULL);
	if(ppenum == NULL)
	{
		return E_POINTER;
	}

	*ppenum = NULL;

	// Create a new CJazzEnumFormatEtc
	pJEFNew = new CJazzEnumFormatEtc();
	if(pJEFNew == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Copy all our format data into pJEFNew
	if(FAILED(pJEFNew->Initialize(this)))
	{
		delete pJEFNew;
		return E_OUTOFMEMORY;
	}

	// Get an IEnumFORMATETC pointer to pJEFNew
	if(FAILED(pJEFNew->QueryInterface(IID_IEnumFORMATETC, (void **) ppenum)))
	{
		delete pJEFNew;
		return E_FAIL;
	}

	// Release the reference from the constructor
	pJEFNew->Release();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Additional functions

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc::AddFormat
//
//	Adds a format to the head of the format list
//
//	Return values:
//		E_OUTOFMEMORY:	Couldn't allocate a new format.
//		S_OK:			The format was successfully added.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::AddFormat(FORMATETC *pFormat)
{
	FormatList *pFL;

	// Create the new FormatList entry and its FORMATETC
	pFL = new FormatList;
	if(pFL == NULL)
	{
		return E_OUTOFMEMORY;
	}
	pFL->pFormat = new FORMATETC;
	if(pFL->pFormat == NULL)
	{
		delete pFL;
		return E_OUTOFMEMORY;
	}

	// Copy the new format in.
	memcpy(pFL->pFormat, pFormat, sizeof(FORMATETC));

	// Add it to the head of the list.
	pFL->pNext = m_pFormatList;
	m_pFormatList = pFL;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc::RemoveFormat
//
//	Removes the specified format from the format list.
//
//	Return values:
//		S_OK:			The format was removed.
//		S_FALSE:		The format could not be found.
//		E_INVALIDARG:	The format was NULL.
//		E_UNEXPECTED:	An entry was found in the list without a format
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::RemoveFormat(FORMATETC *pFormat)
{
	FormatList *pFLThis;
	FormatList *pFLFollow;

	pFLThis		= m_pFormatList;
	pFLFollow	= NULL;

	// Make sure pFormat is valid.
	ASSERT(pFormat != NULL);
	if(pFormat == NULL)
	{
		return E_INVALIDARG;
	}

	// Iterate through the list
	while(pFLThis != NULL)
	{
		// Make sure this list entry has a format
		ASSERT(pFLThis->pFormat != NULL);
		if(pFLThis->pFormat == NULL)
		{
			return E_UNEXPECTED;
		}

		if(memcmp(pFLThis->pFormat, pFormat, sizeof(FORMATETC)) == 0)
		{
			// We found this format!
			if(pFLFollow == NULL)
			{
				// We are at the head of the list.
				pFLThis = pFLThis->pNext;
				delete m_pFormatList->pFormat;
				delete m_pFormatList;
				m_pFormatList = pFLThis;
			}
			else
			{
				// We aren't at the head of the list
				pFLFollow->pNext = pFLThis->pNext;
				delete pFLThis->pFormat;
				delete pFLThis;
				pFLThis = pFLFollow->pNext;
			}
			return S_OK;
		}
		pFLFollow = pFLThis;
		pFLThis = pFLThis->pNext;
	}

	// The format was not found.
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc::RemoveAllFormats
//
//	Removes all the formats from the list.
//
//	Return values:
//		S_OK:			Success.
//		E_UNEXPECTED:	A list entry was found without a format.  Note that
//						the list will still be deleted.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::RemoveAllFormats()
{
	HRESULT		hr;
	FormatList	*pFL;

	hr = S_OK;

	pFL = m_pFormatList;
	while(pFL != NULL)
	{
		m_pFormatList = m_pFormatList->pNext;
		ASSERT(pFL->pFormat != NULL);
		if(pFL->pFormat == NULL)
		{
			hr = E_UNEXPECTED;
		}
		else
		{
			delete pFL->pFormat;
		}
		delete pFL;
		pFL = m_pFormatList;
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzEnumFormatEtc::FormatInList
//
//	Determines whether a format is in the format list.
//
//	Return values:
//		S_OK:			The format was in the list.
//		S_FALSE:		The format was not in the list.
//		E_UNEXPECTED:	A list entry was found without a format.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzEnumFormatEtc::FormatInList(UINT cfFormat)
{
	HRESULT		hr;
	FormatList	*pFL;

	hr = S_FALSE;
	pFL = m_pFormatList;
	while(pFL != NULL)
	{
		ASSERT(pFL->pFormat != NULL);
		if(pFL->pFormat == NULL)
		{
			return E_UNEXPECTED;
		}
		if(pFL->pFormat->cfFormat == cfFormat)
		{
			hr = S_OK;
			break;
		}
		pFL = pFL->pNext;
	}
	return hr;
}