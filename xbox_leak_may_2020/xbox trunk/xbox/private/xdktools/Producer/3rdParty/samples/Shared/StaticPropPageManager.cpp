/************************************************************************
*                                                                       *
*   Copyright (c) 1997-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// StaticPropPageManager.cpp: implementation of CStaticPropPageManager
//
// Identical to AppBasePropPageManager except that this calls
// AfxGetStaticModuleState instead of AfxGetAppModuleState.
// This is used by objects such as the Timeline Control, which is
// an independant ATL object embedded in a Jazz Component.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxole.h>         // MFC OLE classes

#include "StaticPropPageManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CStaticPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticPropPageManager::CStaticPropPageManager() : CBasePropPageManager()
{
}

CStaticPropPageManager::~CStaticPropPageManager()
{
}

/////////////////////////////////////////////////////////////////////////////
// CStaticPropPageManager IUnknown implementation

HRESULT CStaticPropPageManager::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CBasePropPageManager::QueryInterface(riid, ppvObj);
}

ULONG CStaticPropPageManager::AddRef()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

    return CBasePropPageManager::AddRef();
}

ULONG CStaticPropPageManager::Release()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CBasePropPageManager::Release();
}

/////////////////////////////////////////////////////////////////////////////
// CStaticPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CStaticPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CStaticPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CBasePropPageManager::OnRemoveFromPropertySheet();
}


/////////////////////////////////////////////////////////////////////////////
// CStaticPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CStaticPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CBasePropPageManager::SetObject( pINewPropPageObject );
}


/////////////////////////////////////////////////////////////////////////////
// CStaticPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CStaticPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CBasePropPageManager::RemoveObject( pIPropPageObject );
}


/////////////////////////////////////////////////////////////////////////////
// CStaticPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CStaticPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CBasePropPageManager::IsEqualObject( pIPropPageObject );
}

/////////////////////////////////////////////////////////////////////////////
// CStaticPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT FAR EXPORT CStaticPropPageManager::IsEqualPageManagerGUID(REFGUID rguidPageManager)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CBasePropPageManager::IsEqualPageManagerGUID( rguidPageManager );
}

