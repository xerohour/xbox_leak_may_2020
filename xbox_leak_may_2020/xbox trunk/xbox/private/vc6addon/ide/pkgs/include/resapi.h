/////////////////////////////////////////////////////////////////////////////
//	RESAPI.H
//		Resource package interface declarations.

#ifndef __RESAPI_H__
#define __RESAPI_H__

#include "bldapi.h"

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
interface IResourceIntl;
interface IControlList;
interface IResObject;
interface IExternalResPkg;
interface IExternalResDoc;
interface IExternalResView;
interface IResourceBrowserState;
interface IResourceEditorState;
interface IItemEditorInfo;

typedef IResourceIO* LPRESOURCEIO;
typedef IResourceIntl* LPRESOURCEINTL;
typedef IControlList* LPCONTROLLIST;
typedef IResObject* LPRESOBJECT;
typedef IExternalResPkg* LPEXTERNALRESPKG;
typedef IExternalResDoc* LPEXTERNALRESDOC;
typedef IExternalResView* LPEXTERNALRESVIEW;
typedef IResourceBrowserState* LPRESOURCEBROWSERSTATE;
typedef IResourceEditorState* LPRESOURCEEDITORSTATE;
typedef IItemEditorInfo* LPITEMEDITORINFO;

#define P_FrameTitle		        128	// For resource editor frames

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
	STDMETHOD(LoadCmpIO)(THIS_ CFile* pFile, HCOMPIO* phComp) PURE; 	// Non-standard COM.
	STDMETHOD(LoadRctFile)(THIS_ HFILESET hFileSet, LPCSTR lpszPath, BOOL& bCloseIt, HRCIO* phComp) PURE;
	STDMETHOD(NewCmpIO)(THIS_ HCOMPIO* phComp) PURE;
	STDMETHOD(NewRctFile)(THIS_ HFILESET hFileSet, HRCIO* phComp) PURE;
	STDMETHOD(SaveComp)(THIS_ HCOMPOSITION hComp, CFile* pFile) PURE; 	// Non-standard COM.
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
	STDMETHOD(SetDefaultResTemplate)(THIS_ CDocTemplate* pTemplate) PURE;
	STDMETHOD(RegisterExternalEditor)(THIS_ LPEXTERNALRESPKG pExtRes, 
		REFCLSID clsidEditor, LPCSTR szEditorName, LPCSTR szResType) PURE;
	STDMETHOD(OpenRcFile)(THIS_ HFILESET hFileSet, LPCSTR lpszPath, HCOMPOSITION* phComp) PURE;
	STDMETHOD(OpenResource)(THIS_ HCOMPOSITION hComp, LPCTSTR szResType, 
		LPCTSTR szResName, long nLanguage, LPCTSTR szCondition) PURE;
};

#ifdef LOCTOOLS
/////////////////////////////////////////////////////////////////////////////
//	IResourceIntl interface
//		Espresso access to resource package functionality.

#undef  INTERFACE
#define INTERFACE IResourceIntl

typedef BOOL (__stdcall* CANCLOSE_CALLBACK)(CFrameWnd * pFrame);

DECLARE_INTERFACE_(IResourceIntl, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IResourceIntl methods
	STDMETHOD(IntlSetMode)(THIS_ BOOL bMode) PURE;
	STDMETHOD(IntlEditResource)(THIS_ CFile* pFile, CObArray * pSlobs,
		CSlob** ppSelection, CPartDoc** ppDocument) PURE; 	// Non-standard COM.
};
#endif

/////////////////////////////////////////////////////////////////////////////
//	IControlList interface
//		Feature gallery access to resource package OLE control list.

#undef  INTERFACE
#define INTERFACE IControlList

DECLARE_INTERFACE_(IControlList, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IControlList methods
	STDMETHOD(Clear)(THIS) PURE;
	STDMETHOD(AddControl)(THIS_ REFCLSID rClsID) PURE;
	STDMETHOD(RemoveControl)(THIS_ REFCLSID rClsID) PURE;
	STDMETHOD(GetControl)(THIS_ int iCtl, LPCLSID lpClsID) PURE;
};

/////////////////////////////////////////////////////////////////////////////
/*
struct SResHeader
{
//	SResHeader(): m_clsidEditor(NULL){};
	HCOMPOSITION m_hComp;		// handle for parent res file
	HRESOURCE    m_hRes;		// handle of resource object
	REFCLSID  m_clsidEditor;	// clsid of CPartTemplate to use
};
typedef SResHeader RESHEADER, * LPRESHEADER;
*/

enum	// flags for IResObject::OnUpdateCommandUI
{
	OUCUI_ENABLE = 0x1,
	OUCUI_CHECK = 0x2,
	OUCUI_RADIO = 0x3,
};

/////////////////////////////////////////////////////////////////////////////
//	IResPackage interface
//		Used by external resource editors to initiate communication with RES
//		package.  When RES package wants to initiate, use IExternalRes.

#undef  INTERFACE
#define INTERFACE IResPackage

DECLARE_INTERFACE_(IResPackage, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IMasterRes methods
};

/////////////////////////////////////////////////////////////////////////////
//	IResObject interface
//		Used by external resource editors to initiate communication with RES
//		package.  When RES package wants to initiate, use IExternalRes.

#undef  INTERFACE
#define INTERFACE IResObject

DECLARE_INTERFACE_(IResObject, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IMasterRes methods
	STDMETHOD(UpdateData)(THIS_ CFile* pFile) PURE;	// REVIEW: non-standard COM
	STDMETHOD(EditorClosed)(THIS) PURE;
	STDMETHOD(SetDirty)(THIS) PURE;

	STDMETHOD_(GPT, GetIntProp)(THIS_ int idProp, int& rn) PURE;	// REVIEW: non-standard COM
	STDMETHOD_(GPT, GetStrProp)(THIS_ int idProp, CString& rstr) PURE;	// REVIEW: non-standard COM
	STDMETHOD_(BOOL, SetIntProp)(THIS_ int idProp, int n) PURE;
	STDMETHOD_(BOOL, SetStrProp)(THIS_ int idProp, const CString& rstr) PURE;	// REVIEW: non-standard COM

	STDMETHOD(SetupPropertyPages)(THIS_ BOOL bSetCaption) PURE;

	STDMETHOD(OnCommand)(THIS_ UINT idCmd) PURE;
	STDMETHOD(OnUpdateCommandUI)(THIS_ UINT idCmd, DWORD* pdwFlags) PURE;
	/*
	STDMETHOD(AddDependant)(THIS_ CSlob * pSlob) PURE;
	STDMETHOD(RemoveDependant)(THIS_ CSlob * pSlob) PURE;
	*/
};

/////////////////////////////////////////////////////////////////////////////
//	IExternalResPkg interface
//		Used by CResObjects to create communication with external res
//		packages.

#undef  INTERFACE
#define INTERFACE IExternalResPkg

DECLARE_INTERFACE_(IExternalResPkg, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IExternalResPkg methods
	STDMETHOD_(LPEXTERNALRESDOC, OpenStream)(THIS_ REFCLSID clsidTemplate, 
		CFile* pFile, LPRESOBJECT pResObject) PURE;	// REVIEW: non-standard COM
	STDMETHOD_(LPEXTERNALRESDOC, NewResource)(THIS_ REFCLSID clsidTemplate, 
		LPRESOBJECT pResObject) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IExternalResDoc interface
//		Contained by CExternalResDoc in external resource editors.
//		A CResObject keeps a pointer to it when it is opened externally.

#undef  INTERFACE
#define INTERFACE IExternalResDoc

DECLARE_INTERFACE_(IExternalResDoc, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IExternalResDoc methods
	STDMETHOD(CloseEditor)(THIS_ BOOL bDoSave) PURE;
	STDMETHOD(SaveEditor)(THIS) PURE;
	STDMETHOD(UpdateStream)(THIS_ CFile* pFile) PURE;	// REVIEW: non-standard COM
	STDMETHOD(NotifyPropChange)(THIS_ UINT idProp) PURE;
	STDMETHOD_(HWND,GetEditorView)(THIS) PURE;	// TODO: remove
	STDMETHOD(ActivateEditor)(THIS) PURE;
	STDMETHOD (Reload) (THIS_ CFile * pFile) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IExternalResView interface
//		Used to recognize external resource editors among the various MDI
//		children in the IDE.

#undef  INTERFACE
#define INTERFACE IExternalResView

DECLARE_INTERFACE_(IExternalResView, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IExternalResView methods
	STDMETHOD_(LPEXTERNALRESDOC, GetDocument)(THIS) PURE;
	STDMETHOD_(LPRESOBJECT, GetResObject)(THIS) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IResourceBrowserState interface
//		Used to determine the identity, selection state and other 
//		information about a resource browser.

#undef  INTERFACE
#define INTERFACE IResourceBrowserState

DECLARE_INTERFACE_(IResourceBrowserState, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Resource identification methods
	STDMETHOD_(int, GetFileName)(THIS_ LPTSTR szBuffer, int cchBuffer) PURE;

	// Dialog State methods
	STDMETHOD_(int, GetSelectionCount)(THIS) PURE;
	STDMETHOD(GetSelectedItem)(THIS_ int nItem, LPITEMEDITORINFO* ppEditor) PURE;
};


/////////////////////////////////////////////////////////////////////////////
//	IResourceEditorState interface
//		Used to determine the identity, selection state and other 
//		information about a resource editor.

#undef  INTERFACE
#define INTERFACE IResourceEditorState

DECLARE_INTERFACE_(IResourceEditorState, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Resource identification methods
	STDMETHOD_(int, GetFileName)(THIS_ LPTSTR szBuffer, int cchBuffer) PURE;
	STDMETHOD_(int, GetResourceName)(THIS_ LPTSTR szBuffer, int cchBuffer) PURE;
	STDMETHOD(GetResourceValue)(THIS_ WORD* pValue) PURE;
	STDMETHOD(GetResourceLanguage)(THIS_ WORD* pLanguage) PURE;
	STDMETHOD_(int, GetResourceCondition)(THIS_ LPTSTR szBuffer, int cchBuffer) PURE;
	STDMETHOD_(int, GetResourceExternalFilename)(THIS_ 
		LPTSTR szBuffer, int cchBuffer) PURE;

	// Dialog State methods
	STDMETHOD_(int, GetSelectionCount)(THIS) PURE;
	STDMETHOD(GetSelectedItem)(THIS_ int nItem, LPITEMEDITORINFO* ppEditor) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IItemEditorInfo interface
//		Used to retreive information about a specific editor in any of a
//		variety of resource editors.
//		This interface is used for dialog controls (and the dialog itself),
//		menu items and popups, strings table strings, accelerators, 
//		toolbars buttons, resources in the resource browser tree, etc.

#undef  INTERFACE
#define INTERFACE IItemEditorInfo

DECLARE_INTERFACE_(IItemEditorInfo, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Resource identification methods
	STDMETHOD(GetResourceEditor)(THIS_ LPRESOURCEEDITORSTATE* ppResource) PURE;

	// Item info methods
	STDMETHOD_(int, GetClassName)(THIS_ LPTSTR szNameBuffer, int cchBuffer) PURE;
	STDMETHOD_(int, GetItemName)(THIS_ LPTSTR szBuffer, int cchBuffer) PURE;
	STDMETHOD(GetItemValue)(THIS_ WORD* pValue) PURE;
	STDMETHOD_(int, GetItemCaption)(THIS_ LPTSTR szBuffer, int cchBuffer) PURE;
	STDMETHOD_(int, GetClassType)(THIS_ LPTSTR szBuffer, int cchBuffer) PURE;
	// IDEA: GetStyleBits and GetExStyleBits might be useful
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __RESAPI_H__
