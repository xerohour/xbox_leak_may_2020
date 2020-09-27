// autocol.cpp : implementation file
//

#include "stdafx.h"
#include <utilauto.h>
#include "autoguid.h"
//#include <dualimpl.h>
//#include <autocol.h>
//#include <shlaguid.h>

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// 4 Apr 96 - Changed to iterate a list of IDispatch*'s instead of CAutoObj's pointers
//

/////////////////////////////////////////////////////////////////////////////
// This is provided as an example collection, and can also be used directly
//  if no changes need to be made to the members.
// Please read the usage directions at the top of ide\include\autocol.h
//  before using.

/////////////////////////////////////////////////////////////////////////////
// CEnumVariantObjs

IMPLEMENT_DYNAMIC(CEnumVariantObjs, CCmdTarget)

CEnumVariantObjs::CEnumVariantObjs(CDispatchList* pPtrlData)
{
	ASSERT( pPtrlData != NULL) ;
	//@sync - m_pPtrlData = pPtrlData;
	// In order to support live collections, we need to actually
	// copy the collection here and delete it below in the constructor.

	// Create a new array.
	m_pPtrlData = new CDispatchList;

	// Copy data from external array to internal array.
	POSITION pos = pPtrlData->GetHeadPosition();
	while (pos != NULL)
	{
		IDispatch* pDispatch = pPtrlData->GetNext(pos);
		ASSERT (pDispatch != NULL);
		if (pDispatch != NULL) //Defensive
		{
			// AddRef pointer
			pDispatch->AddRef() ;
			// Put into array.
			m_pPtrlData->AddTail(pDispatch) ;
		}
	}

	m_posCurrent = m_pPtrlData->GetHeadPosition();
	AfxOleLockApp();
}

CEnumVariantObjs::~CEnumVariantObjs()
{
	ASSERT(m_pPtrlData != NULL);

	POSITION pos = m_pPtrlData->GetHeadPosition();
	while (pos != NULL)
	{
		IDispatch* pDispatch = m_pPtrlData->GetNext(pos);
		ASSERT (pDispatch != NULL);
		pDispatch->Release() ;
	}
	// The collection is in charge of cleaning this up.
	delete m_pPtrlData ;
	
	AfxOleUnlockApp();
}

void CEnumVariantObjs::OnFinalRelease()
{
	// When the last reference for an automation object is released
	//	OnFinalRelease is called.  This implementation deletes the 
	//	object.  Add additional cleanup required for your object before
	//	deleting it from memory.
	CCmdTarget::OnFinalRelease();
}

STDMETHODIMP_(ULONG) CEnumVariantObjs::XEnumVariant::AddRef()
{	
	METHOD_PROLOGUE(CEnumVariantObjs, EnumVariant)
	return pThis->ExternalAddRef() ;
}	

STDMETHODIMP_(ULONG) CEnumVariantObjs::XEnumVariant::Release()
{	
	METHOD_PROLOGUE(CEnumVariantObjs, EnumVariant)
	return pThis->ExternalRelease() ;
}	

STDMETHODIMP CEnumVariantObjs::XEnumVariant::QueryInterface
	(REFIID iid, void FAR* FAR* ppvObj)
{	
	METHOD_PROLOGUE(CEnumVariantObjs, EnumVariant)
	return (HRESULT)pThis->ExternalQueryInterface( (void FAR*)&iid, ppvObj) ;
}	

// IEnumVariant::Next
// 
STDMETHODIMP CEnumVariantObjs::XEnumVariant::Next
	(ULONG celt, VARIANT FAR* rgvar, ULONG FAR* pceltFetched)
	// pceltFetched may be NULL!!  We must take cases on whether it's
	//  NULL.
{
	METHOD_PROLOGUE(CEnumVariantObjs, EnumVariant)

	HRESULT hr = S_OK;
	ULONG	l ;

	// pceltFetched can legally be NULL, and if it's not,
	//  *pceltFetched can still be 0!  We must be kind to our caller
	if (pceltFetched != NULL)
		*pceltFetched = 0;

	for (l=0; l < celt; l++)
		VariantInit( &rgvar[l] ) ;

	// Retrieve the next celt elements.
	for (l = 0; pThis->m_posCurrent != NULL && celt != 0; l++, celt--)
	{	
		IDispatch* pObj = (IDispatch*) pThis->m_pPtrlData->GetNext( pThis->m_posCurrent ) ;
		/*DER 12-Apr-96 changed to store IDispatch pointers 
		CAutoObj* pObj = (CAutoObj*) pThis->m_pPtrlData->GetNext( pThis->m_posCurrent ) ;
		ASSERT_KINDOF(CAutoObj, pObj);
		*/
		pObj->AddRef() ;

		rgvar[l].vt = VT_DISPATCH;
		rgvar[l].pdispVal = pObj ; //DER 12-Apr96 - Changed for IDispatch pointers pObj->GetIDispatch(TRUE);
		if (pceltFetched != NULL)
		   (*pceltFetched)++ ;
	}
	
	if (celt != 0)
		hr = S_FALSE ;

	return hr ;
}

// IEnumVariant::Skip
//
STDMETHODIMP CEnumVariantObjs::XEnumVariant::Skip(unsigned long celt) 
{
	METHOD_PROLOGUE(CEnumVariantObjs, EnumVariant)

	while (pThis->m_posCurrent != NULL && celt--)
		pThis->m_pPtrlData->GetNext( pThis->m_posCurrent ) ;
	
	return (celt == 0 ? S_OK : S_FALSE) ;
}

STDMETHODIMP CEnumVariantObjs::XEnumVariant::Reset()
{
	METHOD_PROLOGUE(CEnumVariantObjs, EnumVariant)

	pThis->m_posCurrent = pThis->m_pPtrlData->GetHeadPosition();
	
	return S_OK;
}

STDMETHODIMP CEnumVariantObjs::XEnumVariant::Clone(IEnumVARIANT** ppenum) 
{
	METHOD_PROLOGUE(CEnumVariantObjs, EnumVariant)	 

	CEnumVariantObjs* pEnumVariant = new CEnumVariantObjs(pThis->m_pPtrlData) ;
	if (pEnumVariant)
	{
		pEnumVariant->m_posCurrent = pThis->m_posCurrent ;
		return S_OK;
	}
	else
		return E_OUTOFMEMORY;
}


BEGIN_MESSAGE_MAP(CEnumVariantObjs, CCmdTarget)
	//{{AFX_MSG_MAP(CEnumVariantObjs)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(CEnumVariantObjs, CCmdTarget)
	INTERFACE_PART(CEnumVariantObjs, IID_IEnumVARIANT, EnumVariant)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Implementation of CAutoCollection's dual interface

STDMETHODIMP CAutoCollection::XDualObjs::get_Count(long* Count)
{
	METHOD_PROLOGUE(CAutoCollection, Dispatch)
	
	TRY_DUAL(IID_IDualObjs)
	{
		*Count = pThis->GetCount();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoCollection::XDualObjs::get__NewEnum(IUnknown** _NewEnum)
{
	METHOD_PROLOGUE(CAutoCollection, Dispatch)
	
	TRY_DUAL(IID_IDualObjs)
	{
		*_NewEnum = pThis->_NewEnum();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CAutoCollection::XDualObjs::Item(VARIANT index, IDispatch** Item)
{
	METHOD_PROLOGUE(CAutoCollection, Dispatch)
	
	TRY_DUAL(IID_IDualObjs)
	{
		*Item = pThis->Item(index);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

/////////////////////////////////////////////////////////////////////////////
// CAutoCollection


IMPLEMENT_DYNAMIC(CAutoCollection, CAutoObj)

CAutoCollection::CAutoCollection(CDispatchList* pPtrlData)
{
	EnableDualAutomation();
	AfxOleLockApp();
	m_pPtrlData = pPtrlData;
}

CAutoCollection::~CAutoCollection()
{
	// DER 12 Apr 96 - Release the pointers in the collection. (?)
	POSITION pos = m_pPtrlData->GetHeadPosition();
	while (pos != NULL)
	{
		IDispatch* pDispatch = m_pPtrlData->GetNext(pos);
		ASSERT (pDispatch != NULL);
		pDispatch->Release() ;
	}
	// delete the collection itself
	delete m_pPtrlData ;
		
	AfxOleUnlockApp();
}

void CAutoCollection::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.	Add additional cleanup required for your
	// object before calling the base class.

	CAutoObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAutoCollection, CAutoObj)
	//{{AFX_MSG_MAP(CAutoCollection)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoCollection, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoCollection)
	DISP_PROPERTY_EX(CAutoCollection, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_FUNCTION(CAutoCollection, "Item", Item, VT_DISPATCH, VTS_VARIANT)
	DISP_DEFVALUE(CAutoCollection, "Item")
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoCollection, "_NewEnum", DISPID_NEWENUM, _NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CAutoCollection, CAutoObj)
	// UNCOMMENT IF COPYING
	// See note at the top of ide\include\autocol.h for info on how
	//  to use this collection class as an example
	/* INTERFACE_PART(CAutoCollection, IID_IDualObjs, Dispatch)
	INTERFACE_PART(CAutoCollection, IID_IObjs, Dispatch)
    DUAL_ERRORINFO_PART(CAutoCollection) */
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
DS_IMPLEMENT_ENABLE_DUAL(CAutoCollection, DualObjs)
DS_DELEGATE_DUAL_INTERFACE(CAutoCollection, DualObjs)
// UNCOMMENT IF COPYING
// See note at the top of ide\include\autocol.h for info on how
//  to use this collection class as an example
// IMPLEMENT_DUAL_ERRORINFO(CAutoCollection, IID_IObjs)


/////////////////////////////////////////////////////////////////////////////
// CAutoCollection message handlers

long CAutoCollection::GetCount() 
{
	return m_pPtrlData->GetCount();
}

LPUNKNOWN CAutoCollection::_NewEnum() 
{
	CEnumVariantObjs* pEnumVariant = new CEnumVariantObjs(m_pPtrlData);
	
	if (pEnumVariant == NULL)
		DsThrowCannedOleDispatchException(E_OUTOFMEMORY);

	// We don't QueryInterface, since we don't want the IEnumVARIANT
	//  AddRef'd.  The "new CEnumVariantObjs" above already AddRefs it for
	//  us.
	return &pEnumVariant->m_xEnumVariant;
}

LPDISPATCH CAutoCollection::Item(const VARIANT FAR& index) 
{
    // coerce index to VT_I4
	COleVariant coercedIndex(index);
	coercedIndex.ChangeType(VT_I4);
	//DER 12 Apr 96 - store IDispatch*'s --- CAutoObj* pObj = NULL;
	IDispatch* pObj = NULL ;
    POSITION pos = m_pPtrlData->FindIndex(coercedIndex.lVal);
    if (pos != NULL)
	{
		/* DER 12 Apr 96 - Use IDispatch instead of CAutoObj
        pObj = (CAutoObj*)m_pPtrlData->GetAt(pos);
		ASSERT (pObj != NULL);
		ASSERT_KINDOF(CAutoObj, pObj);
		*/
        pObj = m_pPtrlData->GetAt(pos);
		ASSERT (pObj != NULL);
	}
	else
	{
		DsThrowCannedOleDispatchException(E_INVALIDARG);
	}
    
	ASSERT (pObj != NULL);
	/* DER Use IDispatch instead of CAutoObj 
	return pObj->GetIDispatch(TRUE) ;        // AddRef
	*/
	pObj->AddRef();
	return pObj ;
}
