#pragma once

#include "bldtypes.h"
#include "vcfileclasses.h"

//	GetDocLong enum
//		For use with resource template documents.

enum
{
	GDL_CMP_FIRST						= 1000,
	GDL_CMP_SAVE_AS_FILTER,
	GDL_CMP_SYMBOL_VALIDATION_CALLBACK,
	GDL_CMP_SUPPORT_EXTERNAL_FILES,
	GDL_CMP_USES_EXTERNAL_FILES,
	GDL_CMP_SUPPORT_MFC_MODE,
	GDL_CMP_USE_3D_CONTROLS,
};


#ifndef STRICT
typedef DWORD HCOMPOSITION;
typedef DWORD HCOMPIO;
typedef DWORD HRCIO;
typedef DWORD HRESIO;
typedef DWORD HRESOURCE;
#else	// STRICT
DECLARE_HANDLE(HCOMPOSITION);
DECLARE_HANDLE(HCOMPIO);
DECLARE_HANDLE(HRCIO);
DECLARE_HANDLE(HRESIO);
DECLARE_HANDLE(HRESOURCE);
#endif	// STRICT

interface IResourceIO;
interface IExternalResPkg;

typedef IExternalResPkg* LPEXTERNALRESPKG;

/////////////////////////////////////////////////////////////////////////////
//	IResourceIO interface
//		Feature gallery access to resource package I/O funtionality.

#undef  INTERFACE
#define INTERFACE IResourceIO

enum { NEWRESTYPE_RC, NEWRESTYPE_RCT, NEWRESTYPE_RES };
typedef BOOL (CALLBACK* SYMBOL_VALIDATION_CALLBACK)(LPCSTR szSymbol);

DECLARE_INTERFACE_(IResourceIO, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IResourceIO methods
	STDMETHOD(LoadCmpIO)(THIS_ CVCFile* pFile, HCOMPIO* phComp) PURE; 	// Non-standard COM.
	STDMETHOD(LoadRctFile)(THIS_ LPCSTR lpszPath, BOOL& bCloseIt, HRCIO* phComp) PURE;
	STDMETHOD(NewCmpIO)(THIS_ HCOMPIO* phComp) PURE;
	STDMETHOD(NewRctFile)(THIS_ HRCIO* phComp) PURE;
	STDMETHOD(SaveComp)(THIS_ HCOMPOSITION hComp, CVCFile* pFile) PURE; 	// Non-standard COM.
	STDMETHOD(SaveRctFile)(THIS_ HCOMPOSITION hComp, LPCSTR lpszPath) PURE;
	STDMETHOD(CloseComp)(THIS_ HCOMPOSITION hComp) PURE;
	STDMETHOD(NewUniqueSymbol)(THIS_ HCOMPOSITION hComp, LPCSTR szName,
		CString& rstrName, CString& rstrValue) PURE; 	// Non-standard COM.
	STDMETHOD(CloneIntoComp)(THIS_ HCOMPOSITION hSrc, HCOMPOSITION hDest,
		LPCSTR lpszType, LPCSTR lpszID, int nLangID, LPCSTR szCondition,
		LPCSTR szNewSymbol, LPCSTR szNewValue, int nNewLangID,
		LPCSTR szNewCondition) PURE;
	STDMETHOD(ImportIntoComp)(THIS_ HCOMPOSITION hDest, LPCTSTR szType, LPCTSTR szID, 
		LPCTSTR szValue, LPCTSTR szContents, int nLangID, LPCTSTR szCondition) PURE;
	STDMETHOD(DeleteItem)(THIS_ HCOMPOSITION hComp,
		LPCSTR lpszType, LPCSTR lpszID, int nLandID, LPCSTR lpszCondition) PURE;
	STDMETHOD(GetSymbolInclude)(THIS_ HRESIO hComp, CString& rStr) PURE; 	// Non-standard COM.
	STDMETHOD(NewString)(THIS_ HCOMPOSITION hComp, LPCSTR pszSymbol,
		LPCSTR szValue, int nLangID, LPCSTR pszString) PURE;
	STDMETHOD(ChangeItemSymbol)(THIS_ HCOMPOSITION hComp,
		LPCSTR lpszType, LPCSTR lpszID, int nLangID, LPCSTR lpszNewID, LPCSTR szValue) PURE;
	STDMETHOD(UpdateClassWizardInfo)(THIS_ LPCSTR lpszPath, BOOL bUpdateScope) PURE;
	STDMETHOD(RegisterExternalEditor)(THIS_ LPEXTERNALRESPKG pExtRes, 
		REFCLSID clsidEditor, LPCSTR szEditorName, LPCSTR szResType) PURE;
	STDMETHOD(OpenRcFile)(THIS_ LPCSTR lpszPath, HCOMPOSITION* phComp) PURE;
	STDMETHOD(OpenResource)(THIS_ HCOMPOSITION hComp, LPCTSTR szResType, 
		LPCTSTR szResName, long nLanguage, LPCTSTR szCondition) PURE;
};
