// AppJazzDataObject.cpp : Implementation of CAppJazzDataObject

#include <afxole.h>         // MFC OLE classes

#include "AppJazzDataObject.h"

/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject Constructor/Destructor

CAppJazzDataObject::CAppJazzDataObject() : CBaseJazzDataObject()
{
}

CAppJazzDataObject::~CAppJazzDataObject()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject IUnknown implementation

HRESULT CAppJazzDataObject::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return CBaseJazzDataObject::QueryInterface( riid, ppvObj );
}

ULONG CAppJazzDataObject::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::AddRef();
}

ULONG CAppJazzDataObject::Release()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::Release();
}

/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject IDataObject implementation

/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::GetData

HRESULT CAppJazzDataObject::GetData( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::GetData( pformatetcIn, pmedium );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::GetDataHere

HRESULT CAppJazzDataObject::GetDataHere( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::GetDataHere( pformatetcIn, pmedium );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::QueryGetData

HRESULT CAppJazzDataObject::QueryGetData( FORMATETC *pformatetc )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::QueryGetData( pformatetc );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::GetCanonicalFormatEtc

HRESULT CAppJazzDataObject::GetCanonicalFormatEtc( FORMATETC *pformatetcIn, FORMATETC *pformatetcOut )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::GetCanonicalFormatEtc( pformatetcIn, pformatetcOut );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::SetData

HRESULT CAppJazzDataObject::SetData( FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::SetData( pformatetc, pmedium, fRelease );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::EnumFormatEtc

HRESULT CAppJazzDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::EnumFormatEtc( dwDirection, ppenumFormatEtc );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::DAdvise

HRESULT CAppJazzDataObject::DAdvise( FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
									 DWORD *pdwConnection )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::DAdvise( pformatetc, advf, pAdvSink, pdwConnection );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::DUnadvise

HRESULT CAppJazzDataObject::DUnadvise( DWORD dwConnection )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::DUnadvise( dwConnection );
}


/////////////////////////////////////////////////////////////////////////////
// CAppJazzDataObject::EnumDAdvise

HRESULT CAppJazzDataObject::EnumDAdvise( IEnumSTATDATA **ppenumAdvise )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return CBaseJazzDataObject::EnumDAdvise( ppenumAdvise );
}
