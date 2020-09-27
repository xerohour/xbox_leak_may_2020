// AppBasePropPageManager.cpp: implementation of CAppBasePropPageManager
//
//////////////////////////////////////////////////////////////////////

#include <afxole.h>         // MFC OLE classes

#include "AppBasePropPageManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppBasePropPageManager::CAppBasePropPageManager() : CBasePropPageManager()
{
}

CAppBasePropPageManager::~CAppBasePropPageManager()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager IUnknown implementation

HRESULT CAppBasePropPageManager::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBasePropPageManager::QueryInterface(riid, ppvObj);
}

ULONG CAppBasePropPageManager::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBasePropPageManager::AddRef();
}

ULONG CAppBasePropPageManager::Release()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBasePropPageManager::Release();
}

/////////////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CAppBasePropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBasePropPageManager::OnRemoveFromPropertySheet();
}


/////////////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CAppBasePropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBasePropPageManager::SetObject( pINewPropPageObject );
}


/////////////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CAppBasePropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBasePropPageManager::RemoveObject( pIPropPageObject );
}


/////////////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CAppBasePropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBasePropPageManager::IsEqualObject( pIPropPageObject );
}

/////////////////////////////////////////////////////////////////////////////
// CAppBasePropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT FAR EXPORT CAppBasePropPageManager::IsEqualPageManagerGUID(REFGUID rguidPageManager)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBasePropPageManager::IsEqualPageManagerGUID( rguidPageManager );
}

