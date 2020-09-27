/////////////////////////////////////////////////////////////////////////////
//	BLDAPIP.H
//		Bld package private interfaces

#ifndef __BLDAPIP_H__
#define __BLDAPIP_H__

/////////////////////////////////////////////////////////////////////////////
//	IAutoBld interface

// This guy is a helper interface called by the shell to help it implement
//  the build-specific properties/methods of the Application automation
//  object.

// CAUTION!  These methods are meant only to be called in response to
//  a dispatch or dual call from the outside.  They will throw
//  OLE Dispatch exceptions (DsThrowOleDispatchException), so the
//  caller better be one of those MFC dispatch handlers or be
//  prepared to catch the error.
// Because these methods throw exceptions on errors, when they return they
//  return S_OK.

#undef  INTERFACE
#define INTERFACE IAutoBld

DECLARE_INTERFACE_(IAutoBld, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IAutoBld methods
	STDMETHOD(Build)(THIS_ const VARIANT FAR& configuration, BOOL bRebuildAll) PURE;
	STDMETHOD(GetDefaultConfiguration)(THIS_ LPDISPATCH* DefaultConfiguration) PURE;
	STDMETHOD(SetDefaultConfiguration)(THIS_ LPDISPATCH DefaultConfiguration) PURE;
	STDMETHOD(Clean)(THIS_ const VARIANT FAR& configuration) PURE;
	STDMETHOD(Errors)(THIS_ long &nErrors) PURE;
	STDMETHOD(Warnings)(THIS_ long &nWarnings ) PURE;
	STDMETHOD(AddProject)(THIS_ LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, VARIANT_BOOL bAddDefsultFolders) PURE;
};


#endif //__BLDAPIP_H__
