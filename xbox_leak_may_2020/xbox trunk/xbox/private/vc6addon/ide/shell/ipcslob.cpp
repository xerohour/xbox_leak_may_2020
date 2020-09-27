#include "stdafx.h"

#include "shldocs_.h"
#include "ipcslob.h"
#include "proppage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COleTrackSelection, CCmdTarget)

BEGIN_INTERFACE_MAP(COleTrackSelection, CCmdTarget)
	INTERFACE_PART(COleTrackSelection, IID_ITrackSelection, TrackSelection)
END_INTERFACE_MAP()

// This GUID is defined in fm20uuid.lib, which we don't want to use because there
// do not exist RISC builds and the IDE needs to be built for some RISC platforms.
// Also see guids.cpp
DEFINE_GUID(IID_IElement,	0x3050f1ff, 0x98b5, 0x11cf, 0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b);

///////////////////////////////////////////////////////////////////////////////
// COleTrackSelection
//
COleTrackSelection::COleTrackSelection(CIPCompSlob *pIpCompSlob)
{
	ASSERT(NULL != pIpCompSlob);
	ASSERT(pIpCompSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)));
	m_pIpCompSlob = pIpCompSlob;
}

COleTrackSelection::~COleTrackSelection()
{
	m_pIpCompSlob = NULL;
}

HRESULT COleTrackSelection::OnSelectionContainerChange(ISelectionContainer *pSelectionCntr)
{
	if(NULL != m_pIpCompSlob)
		return m_pIpCompSlob->OnSelectionContainerChange(pSelectionCntr);
	else
		return E_UNEXPECTED;
}

STDMETHODIMP COleTrackSelection::XTrackSelection::OnSelectChange(
	ISelectionContainer * pSelectionCntr)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(COleTrackSelection, TrackSelection)
	ASSERT_VALID(pThis);

	pThis->OnSelectionContainerChange(pSelectionCntr);
	return NOERROR;
}

STDMETHODIMP_(ULONG) COleTrackSelection::XTrackSelection::AddRef()
{
	// Delegate to our exported AddRef.

	METHOD_PROLOGUE_EX(COleTrackSelection, TrackSelection)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) COleTrackSelection::XTrackSelection::Release()
{
	// Delegate to our exported Release.

	METHOD_PROLOGUE_EX(COleTrackSelection, TrackSelection)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP COleTrackSelection::XTrackSelection::QueryInterface(
	REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.

	METHOD_PROLOGUE_EX(COleTrackSelection, TrackSelection)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
//	CInternalTrackSelection

IMPLEMENT_DYNAMIC(CInternalTrackSelection, CCmdTarget)

BEGIN_INTERFACE_MAP(CInternalTrackSelection, CCmdTarget)
	INTERFACE_PART(CInternalTrackSelection, IID_IInternalTrackSelection, InternalTrackSelection)
END_INTERFACE_MAP()

CInternalTrackSelection::CInternalTrackSelection(
	CIPCompSlob*	pIpCompSlob)
{
	ASSERT(NULL != pIpCompSlob);
	ASSERT(pIpCompSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)));
	m_pIpCompSlob = pIpCompSlob;
}

CInternalTrackSelection::~CInternalTrackSelection()
{
	m_pIpCompSlob = NULL;
}

STDMETHODIMP_(ULONG) CInternalTrackSelection::XInternalTrackSelection::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CInternalTrackSelection, InternalTrackSelection)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CInternalTrackSelection::XInternalTrackSelection::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CInternalTrackSelection, InternalTrackSelection)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CInternalTrackSelection::XInternalTrackSelection::QueryInterface(
	REFIID riid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CInternalTrackSelection, InternalTrackSelection)
	return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
}

STDMETHODIMP CInternalTrackSelection::XInternalTrackSelection::IsExternalSelectChange(
	VARIANT_BOOL* pbReturn)
{
	METHOD_PROLOGUE_EX(CInternalTrackSelection, InternalTrackSelection)
	ASSERT(NULL != pbReturn);
	if (NULL == pbReturn)
		return E_INVALIDARG;

	return pThis->IsExternalSelectChange(pbReturn);
}

STDMETHODIMP CInternalTrackSelection::XInternalTrackSelection::OnInternalSelectChange(
	DWORD dwCookie)
{
	METHOD_PROLOGUE_EX(CInternalTrackSelection, InternalTrackSelection)
	ASSERT(dwCookie);
	if (0 == dwCookie)
		return E_INVALIDARG;

	return pThis->OnInternalSelectChange(dwCookie);
}

STDMETHODIMP CInternalTrackSelection::XInternalTrackSelection::GetInternalSelectList(
	CALPOLESTR*	pcaStringsOut,
	CADWORD*	pcaCookiesOut)
{
	METHOD_PROLOGUE_EX(CInternalTrackSelection, InternalTrackSelection)

	ASSERT(pcaStringsOut);
	ASSERT(pcaCookiesOut);
	if (NULL != pcaStringsOut)
	{
		pcaStringsOut->cElems = 0;
		pcaStringsOut->pElems = NULL;
	}

	if (NULL != pcaCookiesOut)
	{
		pcaCookiesOut->cElems = 0;
		pcaCookiesOut->pElems = NULL;
	}

	if (NULL == pcaStringsOut || NULL == pcaCookiesOut)
		return E_INVALIDARG;

	return pThis->GetInternalSelectList(pcaStringsOut, pcaCookiesOut);
}

HRESULT CInternalTrackSelection::IsExternalSelectChange(
	VARIANT_BOOL* pbReturn)
{
	ASSERT(NULL != m_pIpCompSlob);
	if (NULL == m_pIpCompSlob)
		return E_UNEXPECTED;

	ASSERT(NULL != pbReturn);
	if (NULL == pbReturn)
		return E_INVALIDARG;

	if (m_pIpCompSlob->GetSelectChangeFlag())
		*pbReturn = VARIANT_TRUE;
	else
		*pbReturn = VARIANT_FALSE;

	return NOERROR;
}

HRESULT CInternalTrackSelection::OnInternalSelectChange(
	DWORD dwCookie)
{
	ASSERT(NULL != m_pIpCompSlob);
	if (NULL == m_pIpCompSlob)
		return E_UNEXPECTED;

	ASSERT(dwCookie);
	if (0 == dwCookie)
		return E_INVALIDARG;

	// do the other things to make the CIPCompSlob push itself using
	// SetPropertyBrowserSelection so that during IdleUpdate, the proper
	// thing happens (for loading appropriate pages)
	m_pIpCompSlob->OnInternalSelectChange((IUnknown*)dwCookie);

	return NOERROR;
}

HRESULT CInternalTrackSelection::GetInternalSelectList(
	CALPOLESTR*	pcaStringsOut,
	CADWORD*	pcaCookiesOut)

{
	ASSERT(NULL != m_pIpCompSlob);
	ASSERT(pcaStringsOut);
	ASSERT(pcaCookiesOut);
	// be a good citizen, initialize out params
	if (NULL != pcaStringsOut)
	{
		pcaStringsOut->cElems = 0;
		pcaStringsOut->pElems = NULL;
	}

	if (NULL != pcaCookiesOut)
	{
		pcaCookiesOut->cElems = 0;
		pcaCookiesOut->pElems = NULL;
	}

	if (NULL == pcaStringsOut || NULL == pcaCookiesOut)
		return E_INVALIDARG;

	if (NULL == m_pIpCompSlob)
		return E_UNEXPECTED;

	pcaCookiesOut->cElems = 0;
	pcaCookiesOut->pElems = NULL;

	// delegate the call to CIPCompSlob
	return m_pIpCompSlob->GetInternalSelectList(pcaStringsOut, pcaCookiesOut);
}

///////////////////////////////////////////////////////////////////////////////
// CIPCompSlob
//
IMPLEMENT_DYNAMIC(CIPCompSlob, CObject)

CIPCompSlob::CIPCompSlob(CIPCompDoc *pDoc)
{
	ASSERT(NULL != pDoc);
	m_pOleTrackSelection	= NULL;
	m_pDoc					= pDoc;
	m_pSingleObject			= NULL;
	m_rgpCachedObjects		= NULL;
	m_nCachedObjectCnt		= 0;
	m_pInternalTrackSelection = NULL;
	m_bSelectChange			= TRUE;
	m_bShowLevelCombo		= FALSE;
}

CIPCompSlob::~CIPCompSlob()
{
	if(NULL != m_pOleTrackSelection)
	{
		m_pOleTrackSelection->Zombie();
		m_pOleTrackSelection = NULL;
	}

	if(NULL != m_pInternalTrackSelection)
	{
		m_pInternalTrackSelection->Zombie();
		m_pInternalTrackSelection = NULL;
	}

	// clean up the cache by release selection
	// objects and freeing up heap if necessary
	ReleaseCachedObjects();
}

BOOL CIPCompSlob::Create()
{
	m_pOleTrackSelection = new COleTrackSelection(this);

	if(FAILED(m_pOleTrackSelection->ExternalQueryInterface(&IID_ITrackSelection, 
				(void**)&m_srpTrackSelection)))
	{
		delete m_pOleTrackSelection;
		m_pOleTrackSelection = NULL;
		return FALSE;
	}
	else
	{
		// mfc starts ref counting at 1, so let's compensate
		m_srpTrackSelection->Release();
		return TRUE;
	}
}

BOOL CIPCompSlob::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	if(m_srpSelectionCntr == NULL)
		return TRUE;

	ReleaseCachedObjects();

	ULONG nUnkCnt = 0;
	if(FAILED(m_srpSelectionCntr->CountObjects(GETOBJS_SELECTED, &nUnkCnt)) || 0 == nUnkCnt)
		return TRUE;

	if(!COlePage::GetShowAllPage())
		COlePage::SetShowAllPage(TRUE);

	UINT cPages = 0;
	HRESULT		hr;
	IUnknown*	pUnkTemp;
	IElement*	pElement;
	IElement*	pParentElement;
	int			iSize;
	ULONG		i;
	ULONG		j;
	ULONG		nCntElements;

	if(1 == nUnkCnt) 
	{
		IUnknown *pUnk = NULL;
		if (GetSelectChangeFlag())
		{
			// external select change
			if(FAILED(m_srpSelectionCntr->GetObjects(GETOBJS_SELECTED, nUnkCnt, &pUnk)))
				return NULL;

			m_pSingleObject = pUnk;
			m_rgpCachedObjects = &m_pSingleObject;
			m_nCachedObjectCnt = 1;
			cPages = COlePage::LoadPages(nUnkCnt, &pUnk);
			// we don't release the objects returned from GetObjects here
			// they get released in ITrackSelection::OnSelectChanged or when
			// the we get destroyed (our destructor)
			//		pUnk->Release();

			// find out how many levels exist in the ancestry including the current one
			// and collect the strings and cookies
			BOOL	fRet;

			fRet = GetElementsIntoArray(pUnk, &m_ElementsArray);
			// if we have a hierarchy, show the level combo box
			m_bShowLevelCombo = (m_ElementsArray.GetSize() > 0);
		}
		else
		{
			// internal select change
			ASSERT(m_pUnkInternalSelectChange != NULL);

			// addref this since in ReleaseCachedObjects, we are going to
			// release this pUnk
			m_pUnkInternalSelectChange->AddRef();

			m_pSingleObject = m_pUnkInternalSelectChange;
			m_rgpCachedObjects = &m_pSingleObject;
			m_nCachedObjectCnt = 1;
			cPages = COlePage::LoadPages(nUnkCnt, &m_pUnkInternalSelectChange);
		}
	}			    
	else
	{
		if (GetSelectChangeFlag())
		{
			// the COlePage object is responsible for freeing up this heap
			// if the array if larger than one, then it knows it is 
			// repsonsible for the memory
			IUnknown **ppUnk = (IUnknown **)AfxAllocTaskMem(sizeof(IUnknown *) * nUnkCnt);
			ASSERT(ppUnk);
			if(NULL == ppUnk)
				return NULL;
			if(FAILED(m_srpSelectionCntr->GetObjects(GETOBJS_SELECTED, nUnkCnt, ppUnk)))
			{
				AfxFreeTaskMem(ppUnk);
				return NULL;
			}
			m_pSingleObject = NULL;
			m_rgpCachedObjects = ppUnk;
			m_nCachedObjectCnt = nUnkCnt;
			cPages = COlePage::LoadPages(nUnkCnt, ppUnk);
			// we don't release the objects returned from GetObjects here
			// they get released in ITrackSelection::OnSelectChanged or when
			// we get destroyed (our destructor)
			//		for(ULONG nCnt = 0; nCnt < nUnkCnt; ++nCnt)
			//			ppUnk[nCnt]->Release();

			// find out how many levels exist in the ancestry including the current one
			// and store the element interfaces

			for (i = 0; i < nUnkCnt; ++i)
			{
				if (!IntersectElements(m_rgpCachedObjects[i]) ||
					m_ElementsArray.GetSize() <= 0)
					i = nUnkCnt;	// break out of loop
			}

			// if we have a hierarchy, show the level combo box
			m_bShowLevelCombo = (m_ElementsArray.GetSize() > 0);
		}
		else
		{
			// internal select change
			ASSERT(m_pUnkInternalSelectChange != NULL);

			// addref this since in ReleaseCachedObjects, we are going to
			// release this pUnk
			if ((IUnknown*)MULTI_SELECT_COOKIE == m_pUnkInternalSelectChange)
			{
				// since we released all the pUnks of selected items in 
				// ReleaseCachedObjects, get them again
				IUnknown **ppUnk = (IUnknown **)AfxAllocTaskMem(sizeof(IUnknown *) * nUnkCnt);
				ASSERT(ppUnk);
				if(NULL == ppUnk)
					return NULL;
				if(FAILED(m_srpSelectionCntr->GetObjects(GETOBJS_SELECTED, nUnkCnt, ppUnk)))
				{
					AfxFreeTaskMem(ppUnk);
					return NULL;
				}

				m_pSingleObject = NULL;
				m_rgpCachedObjects = ppUnk;
				m_nCachedObjectCnt = nUnkCnt;

				cPages = COlePage::LoadPages(nUnkCnt, ppUnk);
			}
			else
			{
				m_pUnkInternalSelectChange->AddRef();
				m_pSingleObject = m_pUnkInternalSelectChange;
				m_rgpCachedObjects = &m_pSingleObject;
				m_nCachedObjectCnt = 1;

				cPages = COlePage::LoadPages(m_nCachedObjectCnt, &m_pUnkInternalSelectChange);
			}
		}
	}

	for( UINT nPage = 0; nPage < cPages; nPage++ )
	{
		ASSERT_VALID(COlePage::GetPropPage(nPage));
		AddPropertyPage(COlePage::GetPropPage(nPage), this);
	}

	CSlob::SetupPropertyPages(pNewSel, bSetCaption);

	// no caption

	return TRUE;
}

BOOL CIPCompSlob::IntersectElements(
	IUnknown*		pUnk)
{
	ASSERT(NULL != pUnk);
	if (NULL == pUnk)
		return FALSE;

	BOOL	fRet;

	// if this is the first element, use the m_ElementsArray
	// to get all the elements into this.
	// else, get the elements into a temporary array and
	// intersect the common elements and keep them and delete
	// the uncommon ones from m_ElementsArray
	if (m_ElementsArray.GetSize() > 0)
	{
		CPtrArray	ElementsArrayLocal;

		fRet = GetElementsIntoArray(pUnk, &ElementsArrayLocal);

		int		iSize = m_ElementsArray.GetSize();
		int		iSize2 = ElementsArrayLocal.GetSize();
		BOOL	bFound;
		int		j;
		HRESULT	hr;

		IUnknown*	pCurr;
		IUnknown*	pUnk;
		
		// make sure that all the elements in pdwa also exists in
		// the new array -- otherwise, remove all the elements that do not
		// exist in the new array
		for (int i = iSize - 1; i >= 0; --i)
		{
			bFound = FALSE;
			pCurr = (IUnknown*)m_ElementsArray.GetAt(i);
			for (j = iSize2 - 1; j >= 0; --j)
			{
				if (pCurr == (IUnknown*)ElementsArrayLocal.GetAt(j))
				{
					bFound = TRUE;
					break;
				}
				else
				{
					if ((pCurr != (IUnknown*)MULTI_SELECT_COOKIE) &&
						(ElementsArrayLocal.GetAt(j) != (IUnknown*)MULTI_SELECT_COOKIE))
					{
						COleRef<IUnknown>	srpUnk1;
						COleRef<IUnknown>	srpUnk2;
						
						// get the IUnknown of both and compare them
						pUnk = pCurr;
						hr = pUnk->QueryInterface(IID_IUnknown, (void**)&srpUnk1);
						if (S_OK == hr)
						{
							pUnk = (IUnknown*)ElementsArrayLocal.GetAt(j);
							hr = pUnk->QueryInterface(IID_IUnknown,
								(void**)&srpUnk2);
							if ((S_OK == hr) && (srpUnk1 == srpUnk2))
							{
								bFound = TRUE;
								break;
							}
						}
					}
				}
			}

			if (!bFound)
				m_ElementsArray.RemoveAt(i);
		}
	}
	else
	{
		fRet = GetElementsIntoArray(pUnk, &m_ElementsArray);
	}

	return fRet;
}

BOOL CIPCompSlob::GetElementsIntoArray(
	IUnknown*	pUnk,
	CPtrArray*	pElementsArray)
{
	ASSERT(pUnk);
	ASSERT(pElementsArray);

	HRESULT			hr;
	IElement*		pElement;
	IElement*		pParentElement;

	// the pElement and all its parents are freed by ReleaseCachedObjects
	pElement = NULL;
#if defined(_M_ALPHA) || defined(_M_PPC)
#pragma message(__FILE__ " commented out to avoid dependancy of IID_IElement on fm30uuid.lib - need to fix later!!! v-ibmmb")
	hr = FALSE;
#else
	hr = pUnk->QueryInterface(IID_IElement, (void**)&pElement);
#endif
	if (!((S_OK == hr) && (NULL != pElement)))
		return FALSE;

	// add the control itself
	if (m_nCachedObjectCnt == 1)
	{
		// single select
		pUnk->AddRef();
		pElementsArray->Add(pUnk);
	}
	else
	{
		// special case:
		// add a special number, the first time to separate this element
		// as a multi-selection where you can find the interfaces
		// for the selected controls in m_rgpCachedObjects.
		pElementsArray->Add((IUnknown*)MULTI_SELECT_COOKIE);
	}

	// traverse the parent chain until you get no parent
	// while you are at it, store the pUnks and the tag names
	while (pElement)
	{
		pParentElement = NULL;
		hr = pElement->get_parentElement(&pParentElement);
		if (NOERROR == hr)
			pElementsArray->Add(pParentElement);

		pElement->Release();
		pElement = NULL;

		if (pParentElement && NOERROR == hr)
#if defined(_M_ALPHA) || defined(_M_PPC)
#pragma message(__FILE__ " commented out to avoid dependancy of IID_IElement on fm30uuid.lib - need to fix later!!! v-ibmmb")
			hr = FALSE;
#else
			hr = pParentElement->QueryInterface(IID_IElement, (void**)&pElement);
#endif
	}

	return TRUE;
}

HRESULT CIPCompSlob::OnSelectionContainerChange(ISelectionContainer *pSelectionCntr)
{
	SetSelectChangeFlag(TRUE);	// set flag to indicate that an external change happened

	// release all cached objects and free up
	// heap if necessary
	ReleaseCachedObjects();

	if(m_srpSelectionCntr != NULL)
	{
		m_srpSelectionCntr.SRelease();
	}

	if(pSelectionCntr != NULL)
	{
		m_srpSelectionCntr = pSelectionCntr;
		m_srpSelectionCntr->AddRef();
		SetPropertyBrowserSelection(this);
	}
	else 
		SetPropertyBrowserSelection(NULL);

	return NOERROR;
}

void CIPCompSlob::OnInternalSelectChange(IUnknown* pUnkInternalSelectChange)
{
	SetSelectChangeFlag(FALSE);	// set flag to indicate that an internal change happened

	ReleaseCachedObjects();

	ASSERT(pUnkInternalSelectChange != NULL);
	m_pUnkInternalSelectChange = pUnkInternalSelectChange;

	SetPropertyBrowserSelection(this);
}

IUnknown *CIPCompSlob::GetTrackSelection()
{
	ASSERT(m_srpTrackSelection != NULL);
	if(m_srpTrackSelection == NULL)
		return NULL;
	else
		return (IUnknown *)m_srpTrackSelection;
}

HRESULT	CIPCompSlob::GetInternalTrackSelection(
	IInternalTrackSelection**	ppITS)
{
	ASSERT(ppITS != NULL);

	HRESULT	hr = E_NOINTERFACE;
	
	*ppITS = NULL;
	if (m_srpInternalTrackSelection == NULL)
	{
		DWORD	grfSupport = 0;

		// ask the m_pDoc if it supports internal track selection
		ASSERT(m_pDoc);
		m_pDoc->DoesSupportInternalSelect(&grfSupport);
		if((grfSupport & ITS_STANDARD_SUPPORT) ||
			(grfSupport & ITS_CUSTOM_SUPPORT))
		{
			m_pInternalTrackSelection = new CInternalTrackSelection(this);

			hr = m_pInternalTrackSelection->ExternalQueryInterface(
				&IID_IInternalTrackSelection, (void**)&m_srpInternalTrackSelection);
			if (FAILED(hr))
			{
				// its an ole interface, so delete it
				// this way, not by using delete
				m_pInternalTrackSelection->ExternalRelease();
				m_pInternalTrackSelection =	NULL;
				return hr;
			}

			if (m_srpInternalTrackSelection)
			{
				// set the return ptr and return
				// we don't addref this because mfc objects
				// start out with a ref count of 1
				*ppITS = m_srpInternalTrackSelection;

 				hr = NOERROR;
			}
		}
	}
	else
	{
		// since we have already created this before, just return it
		*ppITS = m_srpInternalTrackSelection;

		// since we are passing it to the caller, just addref this
		m_srpInternalTrackSelection->AddRef();
		hr = NOERROR;
	}

	return hr;
}

HRESULT CIPCompSlob::GetInternalSelectList(
	CALPOLESTR*	pcaStringsOut,
	CADWORD*	pcaCookiesOut)
{
	ASSERT(pcaStringsOut);
	ASSERT(pcaCookiesOut);
	// be a good citizen, initialize out params
	if (NULL != pcaStringsOut)
	{
		pcaStringsOut->cElems = 0;
		pcaStringsOut->pElems = NULL;
	}

	if (NULL != pcaCookiesOut)
	{
		pcaCookiesOut->cElems = 0;
		pcaCookiesOut->pElems = NULL;
	}

	if (NULL == pcaStringsOut || NULL == pcaCookiesOut)
		return E_INVALIDARG;

	if(m_srpSelectionCntr == NULL)
		return E_UNEXPECTED;

	DWORD	grfSupport = 0;
	// ask the m_pDoc if it supports internal track selection
	ASSERT(m_pDoc);
	m_pDoc->DoesSupportInternalSelect(&grfSupport);
	if(grfSupport & ITS_STANDARD_SUPPORT)
	{
		// QI for IElement interface and then get the tag name
		// for HTML Level objects.
		// special case:
		// if the interfacepointer we get is a special cookie
		// (the MULTI_SELECT_COOKIE), use _T("Multi-Select")
		// string for the time being and later change this to
		// an appropriate string suggested by UE
		// $TODO
		ULONG	ulElems = m_ElementsArray.GetSize();
		if (ulElems > 0)
		{
			HRESULT			hr;
			IElement*		pElement;
			BSTR			bstrTagName;
			ULONG			i;
			LPOLESTR		pszPtr;

			USES_CONVERSION;

			pcaStringsOut->pElems = (LPOLESTR*)AfxAllocTaskMem(sizeof(LPOLESTR)* ulElems);
			if (NULL == pcaStringsOut->pElems)
				return E_OUTOFMEMORY;

			pcaCookiesOut->pElems = (DWORD*)AfxAllocTaskMem(sizeof(DWORD* )* ulElems);
			if (NULL == pcaCookiesOut->pElems)
			{
				::AfxFreeTaskMem(pcaStringsOut->pElems);
				return E_OUTOFMEMORY;
			}

			pcaStringsOut->cElems = ulElems;
			pcaCookiesOut->cElems = ulElems;

			int		iSize;
			CString	strOut;

			for (i = 0; i < ulElems; ++i)
			{
				pElement = NULL;

				if ((IUnknown*)MULTI_SELECT_COOKIE ==
					(IUnknown*)m_ElementsArray.GetAt(i))
				{
					// for multiple selection, make up a tag name,
					// since we have a "Cookie" instead of a interface ptr
					// in the first element
					// $TODO put this in the string table
					strOut = _T("Multi-Select");
				}
				else
				{
#if defined(_M_ALPHA) || defined(_M_PPC)
#pragma message(__FILE__ " commented out to avoid dependancy of IID_IElement on fm30uuid.lib - need to fix later!!! v-ibmmb")
					hr = FALSE;
#else
					hr = ((IUnknown*)m_ElementsArray.GetAt(i))->QueryInterface(
						IID_IElement, (void**)&pElement);
#endif
					if (NOERROR == hr)
					{
						hr = pElement->get_tagName(&bstrTagName);
						if (NOERROR == hr)
						{
							strOut = OLE2T(bstrTagName);
							::SysFreeString(bstrTagName);
						}
						else
						{
							hr = E_FAIL;
							goto errorexit;
						}

						pElement->Release();
					}
					else
					{
						hr = E_FAIL;
						goto errorexit;
					}
				}

				iSize = _tcslen(LPCTSTR(strOut)) + 1;
				pszPtr = NULL;

				if ((pszPtr = (LPOLESTR)AfxAllocTaskMem(iSize * sizeof(OLECHAR))) != NULL)
				{
#ifdef _UNICODE
					wcscpy(pszPtr, LPCTSTR(strOut));
#elif !defined(OLE2ANSI)
					MultiByteToWideChar(CP_ACP, 0, LPCTSTR(strOut), -1, pszPtr, iSize);
#else
					lstrcpy(pszPtr, LPCTSTR(strOut));
#endif
				}

				if (NULL == pszPtr)
				{
					hr = E_OUTOFMEMORY;
errorexit:
					// clean up everything allocated so far
					while (--i)
						::AfxFreeTaskMem(pcaStringsOut->pElems[i]);

					::AfxFreeTaskMem(pcaStringsOut->pElems);
					::AfxFreeTaskMem(pcaCookiesOut->pElems);

					return hr;
				}

				pcaStringsOut->pElems[i] = pszPtr;
				pcaCookiesOut->pElems[i] = (DWORD)(IUnknown*)m_ElementsArray.GetAt(i);
			}

			return NOERROR;
		}
		else
		{
			// no items to send
			return E_FAIL;
		}
	}
	else if (grfSupport & ITS_CUSTOM_SUPPORT)
	{
		if (m_srpSelectionCntr == NULL)
			return E_UNEXPECTED;

		return m_pDoc->GetInternalSelectList(m_srpSelectionCntr,
			pcaStringsOut, pcaCookiesOut);
	}

	return E_UNEXPECTED;
}

void CIPCompSlob::BeginUndo(UINT nID)
{
	CString str;

	ASSERT(nID);
	ASSERT(m_pDoc);
	if(NULL == m_pDoc || !str.LoadString(nID))
		return;

	m_pDoc->BeginUndo(str);
}

BOOL CIPCompSlob::SetUndoStringMgr(IPropertyPageUndoString *pPPUS)
{
	ASSERT(m_pDoc);
	ASSERT(NULL != pPPUS);
	if(NULL == m_pDoc || NULL == pPPUS)
		return FALSE;

	return m_pDoc->SetUndoStringMgr(pPPUS);
}

void CIPCompSlob::EndUndo(BOOL bAbort /*= FALSE*/)
{
	ASSERT(m_pDoc);
	if(NULL == m_pDoc)
		return;

	m_pDoc->EndUndo(bAbort);
}

void CIPCompSlob::ReleaseCachedObjects()
{
	for(ULONG nCnt = 0; nCnt < m_nCachedObjectCnt; ++nCnt)
	{
		if(NULL != m_rgpCachedObjects[nCnt])
			::OleRelease(m_rgpCachedObjects[nCnt]);
	}
	// if we have more than one object we would have
	// allocated heap, so free it up.
	if(m_nCachedObjectCnt > 1)
		AfxFreeTaskMem(m_rgpCachedObjects);

	m_pSingleObject		= NULL;
	m_rgpCachedObjects	= NULL;
	m_nCachedObjectCnt	= 0;

	// if we have an Elements array and the select change is external
	// then release the elements and free the array
	ULONG	ulSize = m_ElementsArray.GetSize();
	if (GetSelectChangeFlag() && (ulSize > 0))
	{
		IUnknown*	pUnk;
		for (nCnt = 0; nCnt < ulSize; ++nCnt)
		{
			pUnk = (IUnknown*)m_ElementsArray.GetAt(nCnt);
			ASSERT(pUnk);
			if ((IUnknown*)MULTI_SELECT_COOKIE != pUnk)
				::OleRelease(pUnk);
		}

		m_ElementsArray.RemoveAll();
	}
}
