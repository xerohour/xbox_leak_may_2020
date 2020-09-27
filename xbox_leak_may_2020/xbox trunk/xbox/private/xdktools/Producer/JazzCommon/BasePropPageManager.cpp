// BasePropPageManager.cpp: implementation of CBasePropPageManager
//
//////////////////////////////////////////////////////////////////////
#include <afxole.h>         // MFC OLE classes

#include "BasePropPageManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CBasePropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBasePropPageManager::CBasePropPageManager()
{
	m_dwRef				= 0;
	AddRef();
	m_pIPropPageObject	= NULL;
	m_pIPropSheet		= NULL;
}

CBasePropPageManager::~CBasePropPageManager()
{
	if( m_pIPropSheet )
	{
		m_pIPropSheet->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager IUnknown implementation

HRESULT CBasePropPageManager::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	if(::IsEqualIID(riid, IID_IDMUSProdPropPageManager)
	|| ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageManager *)this;
        return S_OK;
    }

    *ppvObj = NULL; 
    return E_NOINTERFACE; 
}

ULONG CBasePropPageManager::AddRef()
{
    return ++m_dwRef;
}

ULONG CBasePropPageManager::Release()
{
    ASSERT(m_dwRef != 0);

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager::RemoveCurrentObject

void CBasePropPageManager::RemoveCurrentObject()
{
	if(m_pIPropPageObject == NULL)
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CBasePropPageManager::OnRemoveFromPropertySheet()
{
	RemoveCurrentObject();

	if(m_pIPropSheet)
	{
		m_pIPropSheet->Release();
		m_pIPropSheet = NULL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CBasePropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( S_OK == IsEqualObject(pINewPropPageObject) )
	{
//	    RefreshData();

	    if( m_pIPropSheet )
	    {
		    m_pIPropSheet->RefreshTitle();
		    m_pIPropSheet->RefreshActivePage();
	    }
		return S_OK;
	}

	RemoveCurrentObject();

	m_pIPropPageObject = pINewPropPageObject;
//	m_pIPropPageObject->AddRef();		intentionally missing

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CBasePropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	if( (pIPropPageObject == NULL)
	||  (pIPropPageObject != m_pIPropPageObject) )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentObject();
	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CBasePropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIPropPageObject == m_pIPropPageObject )
	{
		return S_OK;
	}

	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBasePropPageManager IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT FAR EXPORT CBasePropPageManager::IsEqualPageManagerGUID(REFGUID rguidPageManager)
{
	if(::IsEqualGUID(rguidPageManager, m_GUIDManager))
	{
		return S_OK;
	}

	return S_FALSE;
}