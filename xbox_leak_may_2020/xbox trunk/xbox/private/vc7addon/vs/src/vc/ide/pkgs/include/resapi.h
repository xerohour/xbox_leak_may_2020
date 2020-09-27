/////////////////////////////////////////////////////////////////////////////
//  RESAPI.H
//      Resource package interface declarations.

#ifndef __RESAPI_H__
#define __RESAPI_H__

#include "bldtypes.h"

#include "resservice.h"

interface IResourceIntl;
interface IControlList;
interface IResObject;
interface IExternalResPkg;
interface IExternalResDoc;
interface IExternalResView;
interface IResourceBrowserState;
interface IResourceEditorState;
interface IItemEditorInfo;
interface IBinaryResEditor;

typedef IResourceIntl* LPRESOURCEINTL;
typedef IControlList* LPCONTROLLIST;
typedef IResObject* LPRESOBJECT;
typedef IExternalResDoc* LPEXTERNALRESDOC;
typedef IExternalResView* LPEXTERNALRESVIEW;
typedef IResourceBrowserState* LPRESOURCEBROWSERSTATE;
typedef IResourceEditorState* LPRESOURCEEDITORSTATE;
typedef IItemEditorInfo* LPITEMEDITORINFO;
typedef IBinaryResEditor* LPBINARYRESEDITOR;

#define P_FrameTitle                128 // For resource editor frames

#ifdef LOCTOOLS
/////////////////////////////////////////////////////////////////////////////
//  IResourceIntl interface
//      Espresso access to resource package functionality.

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
    STDMETHOD(IntlEditResource)(THIS_ CVCFile* pFile, CObArray * pSlobs,
        CSlob** ppSelection, CPartDoc** ppDocument) PURE;   // Non-standard COM.
};
#endif

/////////////////////////////////////////////////////////////////////////////
//  IControlList interface
//      Feature gallery access to resource package OLE control list.

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
//  SResHeader(): m_clsidEditor(NULL){};
    HCOMPOSITION m_hComp;       // handle for parent res file
    HRESOURCE    m_hRes;        // handle of resource object
    REFCLSID  m_clsidEditor;    // clsid of CPartTemplate to use
};
typedef SResHeader RESHEADER, * LPRESHEADER;
*/

enum    // flags for IResObject::OnUpdateCommandUI
{
    OUCUI_ENABLE = 0x1,
    OUCUI_CHECK = 0x2,
    OUCUI_RADIO = 0x3,
};

/////////////////////////////////////////////////////////////////////////////
//  IResPackage interface
//      Used by external resource editors to initiate communication with RES
//      package.  When RES package wants to initiate, use IExternalRes.

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
//  IResObject interface
//      Used by external resource editors to initiate communication with RES
//      package.  When RES package wants to initiate, use IExternalRes.

#undef  INTERFACE
#define INTERFACE IResObject

DECLARE_INTERFACE_(IResObject, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IMasterRes methods
    STDMETHOD(UpdateData)(THIS_ CVCFile* pFile) PURE;   // REVIEW: non-standard COM
    STDMETHOD(EditorClosed)(THIS) PURE;
    STDMETHOD(SetDirty)(THIS) PURE;

    STDMETHOD_(GPT, GetIntProp)(THIS_ int idProp, int& rn) PURE;    // REVIEW: non-standard COM
    STDMETHOD_(GPT, GetStrProp)(THIS_ int idProp, CString& rstr) PURE;  // REVIEW: non-standard COM
    STDMETHOD_(BOOL, SetIntProp)(THIS_ int idProp, int n) PURE;
    STDMETHOD_(BOOL, SetStrProp)(THIS_ int idProp, const CString& rstr) PURE;   // REVIEW: non-standard COM
    STDMETHOD(SetupPropertyGrid)(THIS_ ITrackSelection *pTrackSelection) PURE;
    STDMETHOD(SetupPropertyPages)(THIS_ BOOL bSetCaption) PURE;

    STDMETHOD(OnCommand)(THIS_ UINT idCmd) PURE;
    STDMETHOD(OnUpdateCommandUI)(THIS_ UINT idCmd, DWORD* pdwFlags) PURE;
    /*
    STDMETHOD(AddDependant)(THIS_ CSlob * pSlob) PURE;
    STDMETHOD(RemoveDependant)(THIS_ CSlob * pSlob) PURE;
    */
};

/////////////////////////////////////////////////////////////////////////////
//  IExternalResPkg interface
//      Used by CResObjects to create communication with external res
//      packages.

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
        CVCFile* pFile, LPRESOBJECT pResObject) PURE;   // REVIEW: non-standard COM
    STDMETHOD_(LPEXTERNALRESDOC, NewResource)(THIS_ REFCLSID clsidTemplate, 
        LPRESOBJECT pResObject) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//  IExternalResDoc interface
//      Contained by CExternalResDoc in external resource editors.
//      A CResObject keeps a pointer to it when it is opened externally.

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
    STDMETHOD(UpdateStream)(THIS_ CVCFile* pFile) PURE; // REVIEW: non-standard COM
    STDMETHOD(NotifyPropChange)(THIS_ UINT idProp) PURE;
    STDMETHOD_(HWND,GetEditorView)(THIS) PURE;  // TODO: remove
    STDMETHOD(ActivateEditor)(THIS) PURE;
    STDMETHOD (Reload) (THIS_ CVCFile * pFile) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//  IExternalResView interface
//      Used to recognize external resource editors among the various MDI
//      children in the IDE.

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
//  IResourceBrowserState interface
//      Used to determine the identity, selection state and other 
//      information about a resource browser.

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
//  IResourceEditorState interface
//      Used to determine the identity, selection state and other 
//      information about a resource editor.

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
//  IItemEditorInfo interface
//      Used to retreive information about a specific editor in any of a
//      variety of resource editors.
//      This interface is used for dialog controls (and the dialog itself),
//      menu items and popups, strings table strings, accelerators, 
//      toolbars buttons, resources in the resource browser tree, etc.

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
//  IBinaryResEditor interface
//      Used by CResObjects to create communication with the binary editor.

#undef  INTERFACE
#define INTERFACE IBinaryResEditor

DECLARE_INTERFACE_(IBinaryResEditor, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IBinaryResEditor methods
    STDMETHOD(SetStream)(THIS_ CVCFile* pFile, LPRESOBJECT pResObject) PURE;
    STDMETHOD_(LPEXTERNALRESDOC, GetExternalResDoc)(THIS) PURE;
};

#endif  // __RESAPI_H__
