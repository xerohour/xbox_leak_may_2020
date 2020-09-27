/////////////////////////////////////////////////////////////////////////////
//	PKGAPI.H
//		Sample package interface declarations.

#ifndef __PKGAPI_H__
#define __PKGAPI_H__

interface IExample;

typedef IExample* LPEXAMPLE;

/////////////////////////////////////////////////////////////////////////////
// IExample

#undef  INTERFACE
#define INTERFACE IExample
DECLARE_INTERFACE_(IExample, IUnknown)
{
	// IUnknown methods

	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IExample methods

	STDMETHOD(IsAvailable)(VOID) PURE;
	STDMETHOD(SetAvailable)(BOOL bAvailable) PURE;
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __PKGAPI_H__
