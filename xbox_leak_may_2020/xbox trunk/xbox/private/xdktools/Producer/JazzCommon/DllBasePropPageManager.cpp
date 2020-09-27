// DllBasePropPageManager.cpp: implementation of CDllBasePropPageManager
//
//////////////////////////////////////////////////////////////////////

#include <afxctl.h>
#include <afxole.h>		// MFC OLE classes

#include "DllBasePropPageManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDllBasePropPageManager::CDllBasePropPageManager() : CBasePropPageManager()
{
}

CDllBasePropPageManager::~CDllBasePropPageManager()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager IUnknown implementation

HRESULT CDllBasePropPageManager::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CBasePropPageManager::QueryInterface(riid, ppvObj);
} 

ULONG CDllBasePropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return CBasePropPageManager::AddRef();
}

ULONG CDllBasePropPageManager::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CBasePropPageManager::Release();
}

/////////////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CDllBasePropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CBasePropPageManager::OnRemoveFromPropertySheet();
}


/////////////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CDllBasePropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CBasePropPageManager::SetObject( pINewPropPageObject );
}


/////////////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CDllBasePropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CBasePropPageManager::RemoveObject( pIPropPageObject );
}


/////////////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CDllBasePropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CBasePropPageManager::IsEqualObject( pIPropPageObject );
}

/////////////////////////////////////////////////////////////////////////////
// CDllBasePropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT FAR EXPORT CDllBasePropPageManager::IsEqualPageManagerGUID(REFGUID rguidPageManager)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CBasePropPageManager::IsEqualPageManagerGUID( rguidPageManager );
}

