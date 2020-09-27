/////////////////////////////////////////////////////////////////////////////
//	GALAPI.H
//		Gallery package interface declarations.

#ifndef __GALAPI_H__
#define __GALAPI_H__

#ifndef STRICT
typedef DWORD HCUSTOMFEATUREREF;
#else	// STRICT
DECLARE_HANDLE(HCUSTOMFEATUREREF);
#endif	// STRICT

interface IObjectGallery;

typedef IObjectGallery* LPOBJECTGALLERY;

/////////////////////////////////////////////////////////////////////////////
// IObjectGallery
//   This interface is used by ClassWizard and custom oglets
//   to interact with the database, and create apply contexts.

#undef  INTERFACE
#define INTERFACE IObjectGallery
DECLARE_INTERFACE_(IObjectGallery, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IObjectGallery methods

	// Apply context creation methods
	STDMETHOD(CreateApplyContext)(THIS_ REFGUID rFeatureID, UINT nCreateFlags, LPVOID FAR* ppvObj) PURE;

	// Object Gallery Extension creation methods
	STDMETHOD(BeginNewExtension)(THIS_ LPCSTR pszName, HCUSTOMFEATUREREF* phExt) PURE;
	STDMETHOD(AddFile)(THIS_ HCUSTOMFEATUREREF hExt, LPCSTR pszFilename) PURE;
	STDMETHOD(AddResource)(THIS_ HCUSTOMFEATUREREF hExt, LPCSTR pszType, LPCSTR pszSymbol) PURE;
	STDMETHOD(EndExtension)(THIS_ HCUSTOMFEATUREREF hExt) PURE;
	STDMETHOD(ScanProject)(THIS_ LPCSTR pszConfig) PURE;
	STDMETHOD(ScanClass)(THIS_ LPCSTR pszClass) PURE;

	// Code writer methods.
	STDMETHOD(CreateMember)(THIS_ LPCSTR pszMember, UINT nAccess) PURE;
	STDMETHOD(CreateImplementation)(THIS_ LPCSTR pszMember, LPCSTR pszCode) PURE;

	// Misc.
	STDMETHOD(SetDefaultConfiguration)(THIS_ LPCSTR pszConfig) PURE;
	STDMETHOD(CommitEdits)(THIS) PURE;
	STDMETHOD(RevertEdits)(THIS) PURE;
	STDMETHOD(GetClassFiles)(THIS_ LPCSTR pszClass, CString& rstrHeader, CString& rstrImpl) PURE; 	// Non-standard COM.
	STDMETHOD(ClassAccess)(THIS_ LPCSTR pszClass, UINT nMode) PURE;
	STDMETHOD(FindInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
};
#define OPEN_ALWAYS_AC   (0)
#define CREATE_NEW_AC    (1)
#define OPEN_EXISTING_AC (2)

// Access mode flags
#ifndef MODE_READ
#define MODE_READ   (0)
#endif
#ifndef MODE_WRITE
#define MODE_WRITE  (1)
#endif

/////////////////////////////////////////////////////////////////////////////

#endif	// __GALAPI_H__
