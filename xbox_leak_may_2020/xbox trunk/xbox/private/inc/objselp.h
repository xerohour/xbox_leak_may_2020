//+--------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1994 - 1998.
//
//  File:       objselp.h
//
//  Contents:   Object Picker Dialog private header
//
//---------------------------------------------------------------------------

#include <objbase.h>

DEFINE_GUID(IID_IObjectPickerScope,
0xe8f3a4c4, 0x9e62, 0x11d1, 0x8b, 0x22, 0x00, 0xc0, 0x4f, 0xd8, 0xdb, 0xf7);

DEFINE_GUID(IID_IDsObjectPickerScope,
0xd8150286, 0x9e64, 0x11d1, 0x8b, 0x22, 0x00, 0xc0, 0x4f, 0xd8, 0xdb, 0xf7);

DEFINE_GUID(IID_IBindHelper,
0x29b3711c, 0xa6fd, 0x11d1, 0x8b, 0x23, 0x00, 0xc0, 0x4f, 0xd8, 0xdb, 0xf7);

DEFINE_GUID(IID_ICustomizeDsBrowser,
0x58dc8834, 0x9d9f, 0x11d1, 0x8b, 0x22, 0x00, 0xc0, 0x4f, 0xd8, 0xdb, 0xf7);

DEFINE_GUID(IID_IObjectBrowser,
0x35920279, 0xee67, 0x11d0, 0x80, 0x93, 0x00, 0xc0, 0x4f, 0xd8, 0xe1, 0xda);

DEFINE_GUID(IID_IDsObjectPickerEx,
0xa4e86007, 0x62e4, 0x11d2, 0xae, 0x51, 0x00, 0xc0, 0x4f, 0x79, 0xdb, 0x19);

#ifndef __OBJSELP_H_
#define __OBJSELP_H_

#ifndef __iads_h__
#include <iads.h>
#endif // __iads_h__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef struct tagDSQUERYINFO
{
    ULONG               cbSize;
    LPCWSTR             pwzLdapQuery;
    ULONG               cFilters;
    LPCWSTR            *apwzFilter;
    LPCWSTR             pwzCaption;
} DSQUERYINFO, *PDSQUERYINFO;



//
// Private Object Picker Interfaces
//

#undef INTERFACE
#define INTERFACE IObjectPickerScope

DECLARE_INTERFACE_(IObjectPickerScope, IUnknown)
{
    // *** IUnknown methods ***

    STDMETHOD(QueryInterface)(THIS_
                              REFIID riid,
                              PVOID *ppv) PURE;

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;

    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IObjectPickerScope methods ***

    STDMETHOD_(HWND, GetHwnd)(THIS) PURE;

    STDMETHOD(SetHwnd)(THIS_
                HWND hwndScopeDialog) PURE;
};

#undef INTERFACE
#define INTERFACE IDsObjectPickerScope


DECLARE_INTERFACE_(IDsObjectPickerScope, IObjectPickerScope)
{
    // *** IUnknown methods ***

    STDMETHOD(QueryInterface)(THIS_
                REFIID   riid,
                LPVOID * ppv) PURE;

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;

    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IObjectPickerScope methods ***

    STDMETHOD_(HWND, GetHwnd)(THIS) PURE;

    STDMETHOD(SetHwnd)(THIS_
                HWND hwndScopeDialog) PURE;

    // *** IDsObjectPickerScope methods ***

    STDMETHOD_(ULONG,GetType)(THIS) PURE;

    STDMETHOD(GetQueryInfo)(THIS_
                PDSQUERYINFO *ppqi) PURE;

    STDMETHOD_(BOOL, IsUplevel)(THIS) PURE;

    STDMETHOD_(BOOL, IsDownlevel)(THIS) PURE;

    STDMETHOD_(BOOL, IsExternalDomain)(THIS) PURE;

    STDMETHOD(GetADsPath)(THIS_
                PWSTR *ppwzADsPath) PURE;
};


#undef INTERFACE
#define INTERFACE IBindHelper

DECLARE_INTERFACE_(IBindHelper, IUnknown)
{
    // *** IUnknown methods ***

    STDMETHOD(QueryInterface)(THIS_
                REFIID   riid,
                LPVOID * ppv) PURE;

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;

    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IBindHelper methods ***

    STDMETHOD(BindToObject)(THIS_
                HWND    hwnd,
                PCWSTR pwzADsPath,
                REFIID riid,
                LPVOID *ppv,
                ULONG  flags) PURE;

    STDMETHOD(GetNameTranslate)(THIS_
                HWND                hwnd,
                PCWSTR              pwzADsPath,
                IADsNameTranslate **ppNameTranslate) PURE;

    STDMETHOD(GetDomainRootDSE)(THIS_
                HWND    hwnd,
                PCWSTR pwzDomain,
                IADs **ppADsRootDSE) PURE;
};



#undef INTERFACE
#define INTERFACE ICustomizeDsBrowser

DECLARE_INTERFACE_(ICustomizeDsBrowser, IUnknown)
{
    // *** IUnknown methods ***

    STDMETHOD(QueryInterface)(THIS_
                REFIID   riid,
                LPVOID * ppv) PURE;

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;

    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** ICustomizeDsBrowser methods ***

    STDMETHOD(Initialize)(THIS_
                          HWND         hwnd,
                          PCDSOP_INIT_INFO pInitInfo,
                          IBindHelper *pBindHelper) PURE;

    STDMETHOD(GetQueryInfoByScope)(THIS_
                IDsObjectPickerScope *pDsScope,
                PDSQUERYINFO *ppdsqi) PURE;

    STDMETHOD(AddObjects)(THIS_
                IDsObjectPickerScope *pDsScope,
                IDataObject **ppdo) PURE;

    STDMETHOD(ApproveObjects)(THIS_
                IDsObjectPickerScope *pDsScope,
                IDataObject *pdo,
                PBOOL afApproved) PURE;

    STDMETHOD(PrefixSearch)(THIS_
                IDsObjectPickerScope *pDsScope,
                PCWSTR pwzSearchFor,
                IDataObject **pdo) PURE;

    STDMETHOD_(PSID, LookupDownlevelName)(THIS_
        PCWSTR pwzAccountName) PURE;
};


#undef INTERFACE
#define INTERFACE IDsObjectPickerEx

DECLARE_INTERFACE_(IDsObjectPickerEx, IDsObjectPicker)
{
    // *** IUnknown methods ***

    STDMETHOD(QueryInterface)(THIS_
                REFIID   riid,
                LPVOID * ppv) PURE;

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;

    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IDsObjectPicker methods ***

    // Sets scope, filter, etc. for use with next invocation of dialog
    STDMETHOD(Initialize)(
        THIS_
        PDSOP_INIT_INFO pInitInfo) PURE;

    // Creates the modal DS Object Picker dialog.
    STDMETHOD(InvokeDialog)(
         THIS_
         HWND               hwndParent,
         IDataObject      **ppdoSelections) PURE;

    // *** IDsObjectPickerEx methods ***

    // Creates the modal DS Object Picker dialog.
    STDMETHOD(InvokeDialogEx)(
         THIS_
         HWND                   hwndParent,
         ICustomizeDsBrowser   *pCustomizeDsBrowser,
         IDataObject          **ppdoSelections) PURE;
};


//
// Private scope types
//

#define DSOP_SCOPE_TYPE_INVALID                        0x00000000
#define DSOP_SCOPE_TYPE_SEPARATOR                      0x00100000

//
// Private clipboard formats supported by CDataObject
//

#define CFSTR_DSOP_DS_OBJECT_LIST     TEXT("CFSTR_DSOP_DS_OBJECT_LIST")

//
// Object Picker Structures
//

typedef struct tagSELECTIONITEM
{
    ULONG   cbNameOffset;       // Offset to the item name
    ULONG   cbData;             // Item-specific data size
    ULONG   cbDataOffset;       // Offset to item-specific data
} SELECTIONITEM, *PSELECTIONITEM;

typedef const struct tagSELECTIONITEM * PCSELECTIONITEM;

typedef struct tagSELECTIONITEMLIST
{
    ULONG        cbSize;                // size in bytes, including cbSize
    ULONG        cItems;                // Selection count
    SELECTIONITEM rgItems[ANYSIZE_ARRAY];
} SELECTIONITEMLIST, * PSELECTIONITEMLIST;

typedef const struct tagSELECTIONITEMLIST * PCSELECTIONITEMLIST;

typedef struct tagOBJECTPICKERINFO *POBJECTPICKERINFO;

typedef struct tagOBJECTBROWSEINIT
{
    HWND hwndFrame;
    HWND hwndComboBoxEx;
    POBJECTPICKERINFO popi;
} OBJECTBROWSEINIT, *POBJECTBROWSEINIT;


//
// Private Object Picker Interfaces
//


#undef INTERFACE
#define INTERFACE IObjectBrowser

DECLARE_INTERFACE_(IObjectBrowser, IUnknown)
{
    // *** IUnknown methods ***

    STDMETHOD(QueryInterface)(THIS_
                REFIID   riid,
                LPVOID * ppv) PURE;

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;

    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // *** IObjectBrowser methods ***

    STDMETHOD(Initialize)(THIS_
                POBJECTBROWSEINIT pInit) PURE;

    STDMETHOD(CreateView)(THIS_
                HWND hwndParent,
                IObjectPickerScope *pScope) PURE;

    STDMETHOD(DeleteScope)(THIS_
                IObjectPickerScope *pScope) PURE;
};




typedef struct tagOBJECTPICKERINFO
{
    ULONG               cbSize;             // Struct size
    HWND                hwndParent;         // Parent window
    ULONG               flObjectPicker;     // OP_* flags
    LPCTSTR             pwzCaption;         // Dialog caption
    IObjectBrowser     *pBrowser;           // browse view manager
    LPVOID              pvSelection;        // selection view template/name
    DLGPROC             pSelectDlgProc;     // selection dialog procedure
    LPARAM              lParamSelectInit;   // for CreateDialogParam
    HINSTANCE           hInstance;          // location for resources
    IDataObject        *pDataObject;        // holds selection on return
} OBJECTPICKERINFO;


//
// Private Object Picker Messages
//


#define OPM_BASE                   (WM_USER + 400)
#define OPM_SHOW                   (OPM_BASE + 0)  // show or hide browse
#define OPM_GET_SELECTION          (OPM_BASE + 1)  // create data object
#define OPM_GET_CUR_SCOPE          (OPM_BASE + 2)  // return cur IObjectPickerScope
#define OPM_BROWSE_DOUBLE_CLICK    (OPM_BASE + 3)  // user dbl clicked in browser
#define OPM_QUERY_CLOSE            (OPM_BASE + 4)  // is it ok to close?
#define OPM_GET_ICON               (OPM_BASE + 5)  // get icon for item
#define OPM_REFRESH_SCOPE          (OPM_BASE + 6)  // sent after setcursel
#define OPM_BROWSER_HAS_SELECTION  (OPM_BASE + 7) // notification fromo browser, t/f
#define OPM_GET_SCOPE_HWND         (OPM_BASE + 8) // return hwnd of scope combo
#define OPM_ENABLE_OK              (OPM_BASE + 9) // enable/disable OK button
#define OPM_CLOSING                (OPM_BASE + 10) // notification of dialog closing
#define OPM_DELETE_SCOPE           (OPM_BASE + 11) // delete lookin combo item

//
// Private DS Object Picker Messages
//

#define DSOPM_BASE                      (WM_USER + 500)
#define DSOPM_NEWBLOCK                  (DSOPM_BASE + 0)
#define DSOPM_QUERY_DONE                (DSOPM_BASE + 1)
#define DSOPM_PROMPT_FOR_CREDS          (DSOPM_BASE + 2)
#define DSOPM_GET_BROWSE_MGR            (DSOPM_BASE + 3)

//
// Private API
//

STDAPI
GetObjectSelection(
    POBJECTPICKERINFO popi);


//
// Private Bit flags for flObjectPicker
//

#define OP_MULTISELECT                          0x00000001  // show selection bucket dialog
#define OP_SELECTION_TEMPLATE                   0x00000002  // use CreateDialogIndirect
#define OP_SELECTION_TEMPLATE_NAME              0x00000004  // use CreateDialog
#define OP_SELECTION_TEMPLATE_DEFAULT           0x00000008  // use built-in selection template

//
// Private typedefs and ordinals for use with LoadLibrary
//

#define GETOBJECTSELECTION_ORDINAL      1

typedef HRESULT (STDAPICALLTYPE *PFN_GetObjectSelection)(POBJECTPICKERINFO);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OBJSELP_H_


