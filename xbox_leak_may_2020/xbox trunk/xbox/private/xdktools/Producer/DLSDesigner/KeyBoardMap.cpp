//////////////////////////////////////////////////////////////////////
// KeyBoardMap.cpp : implementation file
//

#include "StdAfx.h"
#include "RegionNote.h"
#include "KeyBoardMap.h"

HRESULT KeyBoardMap::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG KeyBoardMap::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG KeyBoardMap::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}