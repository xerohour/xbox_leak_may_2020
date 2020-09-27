// DllJazzDataObject.cpp : Implementation of CDllJazzDataObject

#include <afxctl.h>
#include <afxole.h>         // MFC OLE classes

#include "DllJazzDataObject.h"

/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject Constructor/Destructor

CDllJazzDataObject::CDllJazzDataObject() : CBaseJazzDataObject()
{
}

CDllJazzDataObject::~CDllJazzDataObject()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject IUnknown implementation

HRESULT CDllJazzDataObject::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

	return CBaseJazzDataObject::QueryInterface( riid, ppvObj );
}

ULONG CDllJazzDataObject::AddRef()
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::AddRef();
}

ULONG CDllJazzDataObject::Release()
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::Release();
}

/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject IDataObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::GetData

HRESULT CDllJazzDataObject::GetData( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::GetData( pformatetcIn, pmedium );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::GetDataHere

HRESULT CDllJazzDataObject::GetDataHere( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::GetDataHere( pformatetcIn, pmedium );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::QueryGetData

HRESULT CDllJazzDataObject::QueryGetData( FORMATETC *pformatetc )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::QueryGetData( pformatetc );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::GetCanonicalFormatEtc

HRESULT CDllJazzDataObject::GetCanonicalFormatEtc( FORMATETC *pformatetcIn, FORMATETC *pformatetcOut )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::GetCanonicalFormatEtc( pformatetcIn, pformatetcOut );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::SetData

HRESULT CDllJazzDataObject::SetData( FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::SetData( pformatetc, pmedium, fRelease );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::EnumFormatEtc

HRESULT CDllJazzDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::EnumFormatEtc( dwDirection, ppenumFormatEtc );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::DAdvise

HRESULT CDllJazzDataObject::DAdvise( FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
									 DWORD *pdwConnection )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::DAdvise( pformatetc, advf, pAdvSink, pdwConnection );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::DUnadvise

HRESULT CDllJazzDataObject::DUnadvise( DWORD dwConnection )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::DUnadvise( dwConnection );
}


/////////////////////////////////////////////////////////////////////////////
// CDllJazzDataObject::EnumDAdvise

HRESULT CDllJazzDataObject::EnumDAdvise( IEnumSTATDATA **ppenumAdvise )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

    return CBaseJazzDataObject::EnumDAdvise( ppenumAdvise );
}
