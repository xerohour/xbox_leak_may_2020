#ifndef __cmnquery_h
#define __cmnquery_h

DEFINE_GUID(IID_IQueryFormW, 0x8cfcee30, 0x39bd, 0x11d0, 0xb8, 0xd1, 0x0, 0xa0, 0x24, 0xab, 0x2d, 0xbb);
DEFINE_GUID(IID_IQueryFormA, 0x66c95d82, 0x104e, 0x11d1, 0xb1, 0x31, 0x0, 0xa0, 0xc9, 0x06, 0xaf, 0x45);
DEFINE_GUID(IID_IPersistQueryW, 0x1a3114b8, 0xa62e, 0x11d0, 0xa6, 0xc5, 0x0, 0xa0, 0xc9, 0x06, 0xaf, 0x45);
DEFINE_GUID(IID_IPersistQueryA, 0x66c95d82, 0x104e, 0x11d1, 0xb1, 0x31, 0x0, 0xa0, 0xc9, 0x06, 0xaf, 0x45);

DEFINE_GUID(CLSID_CommonQuery,  0x83bc5ec0, 0x6f2a, 0x11d0, 0xa1, 0xc4, 0x0, 0xaa, 0x00, 0xc1, 0x6e, 0x65);
DEFINE_GUID(IID_ICommonQueryW, 0xab50dec0, 0x6f1d, 0x11d0, 0xa1, 0xc4, 0x0, 0xaa, 0x00, 0xc1, 0x6e, 0x65);
DEFINE_GUID(IID_ICommonQueryA, 0x3399fb0b, 0x18eb, 0x11d1, 0xb1, 0x3e, 0x0, 0xa0, 0xc9, 0x06, 0xaf, 0x45);


#ifndef GUID_DEFS_ONLY

//-----------------------------------------------------------------------------
// IQueryForm
//-----------------------------------------------------------------------------

//
// A query form object is registered under the query handlers CLSID,
// a list is stored in the registry:
//
//  HKCR\CLSID\{CLSID query handler}\Forms
//
// For each form object there are server values which can be defined:
//
//  Flags           = flags for the form object:
//                      QUERYFORM_CHANGESFORMLIST
//                      QUERYFORM_CHANGESOPTFORMLIST
//
//  CLSID           = string containing the CLSID of the InProc server to invoke
//                    to get the IQueryFormObject.
//
//  Forms           = a sub key containing the CLSIDs for the forms registered
//                    by IQueryForm::AddForms (or modified by ::AddPages), if
//                    the flags are 0, then we scan this list looking for a match
//                    for the default form specified.
//

#define QUERYFORM_CHANGESFORMLIST       0x000000001
#define QUERYFORM_CHANGESOPTFORMLIST    0x000000002

//
// Query Forms
// ===========
//  Query forms are registered and have query pages added to them, a form without
//  pages is not displayed.  Each form has a unique CLSID to allow it to be
//  selected by invoking the query dialog.
//

#define CQFF_NOGLOBALPAGES  0x0000001       // = 1 => doesn't have global pages added
#define CQFF_ISOPTIONAL     0x0000002       // = 1 => form is hidden, unless optional forms requested

struct _cqform_W;
typedef struct _cqform_W CQFORM_W, * LPCQFORM_W;
typedef HRESULT (CALLBACK *LPCQADDFORMSPROC_W)(LPARAM lParam, LPCQFORM_W pForm);

struct _cqform_A;
typedef struct _cqform_A CQFORM_A, * LPCQFORM_A;
typedef HRESULT (CALLBACK *LPCQADDFORMSPROC_A)(LPARAM lParam, LPCQFORM_A pForm);

struct _cqform_W
{
    DWORD   cbStruct;
    DWORD   dwFlags;
    CLSID   clsid;
    HICON   hIcon;
    LPCWSTR pszTitle;
};

struct _cqform_A
{
    DWORD   cbStruct;
    DWORD   dwFlags;
    CLSID   clsid;
    HICON   hIcon;
    LPCSTR  pszTitle;
};

#ifdef UNICODE
#define CQFORM           CQFORM_W
#define LPCQFORM         LPCQFORM_W
#define LPCQADDFORMSPROC LPCQADDFORMSPROC_W
#else
#define CQFORM           CQFORM_A
#define LPCQFORM         LPCQFORM_A
#define LPCQADDFORMSPROC LPCQADDFORMSPROC_A
#endif

//
// Query Form Pages
// ================
//  When a query form has been registered the caller can then add pages to it,
//  any form can have pages appended.
//


struct _cqpage_W;
typedef struct _cqpage_W CQPAGE_W, * LPCQPAGE_W;
typedef HRESULT (CALLBACK *LPCQADDPAGESPROC_W)(LPARAM lParam, REFCLSID clsidForm, LPCQPAGE_W pPage);
typedef HRESULT (CALLBACK *LPCQPAGEPROC_W)(LPCQPAGE_W pPage, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct _cqpage_A;
typedef struct _cqpage_A CQPAGE_A, * LPCQPAGE_A;
typedef HRESULT (CALLBACK *LPCQADDPAGESPROC_A)(LPARAM lParam, REFCLSID clsidForm, LPCQPAGE_A pPage);
typedef HRESULT (CALLBACK *LPCQPAGEPROC_A)(LPCQPAGE_A pPage, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct _cqpage_W
{
    DWORD          cbStruct;
    DWORD          dwFlags;
    LPCQPAGEPROC_W pPageProc;
    HINSTANCE      hInstance;
    INT            idPageName;
    INT            idPageTemplate;
    DLGPROC        pDlgProc;
    LPARAM         lParam;
};

struct _cqpage_A
{
    DWORD          cbStruct;
    DWORD          dwFlags;
    LPCQPAGEPROC_A pPageProc;
    HINSTANCE      hInstance;
    INT            idPageName;
    INT            idPageTemplate;
    DLGPROC        pDlgProc;
    LPARAM         lParam;
};

#ifdef UNICODE
#define CQPAGE           CQPAGE_W
#define LPCQPAGE         LPCQPAGE_W
#define LPCQADDPAGESPROC LPCQADDPAGESPROC_W
#define LPCQPAGEPROC     LPCQPAGEPROC_W
#else
#define CQPAGE           CQPAGE_A
#define LPCQPAGE         LPCQPAGE_A
#define LPCQADDPAGESPROC LPCQADDPAGESPROC_A
#define LPCQPAGEPROC     LPCQPAGEPROC_A
#endif

//
// IQueryForm interfaces
//

#undef  INTERFACE
#define INTERFACE IQueryFormW

DECLARE_INTERFACE_(IQueryFormW, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IQueryForm methods
    STDMETHOD(Initialize)(THIS_ HKEY hkForm) PURE;
    STDMETHOD(AddForms)(THIS_ LPCQADDFORMSPROC_W pAddFormsProc, LPARAM lParam) PURE;
    STDMETHOD(AddPages)(THIS_ LPCQADDPAGESPROC_W pAddPagesProc, LPARAM lParam) PURE;
};

#undef  INTERFACE
#define INTERFACE IQueryFormA

DECLARE_INTERFACE_(IQueryFormA, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IQueryForm methods
    STDMETHOD(Initialize)(THIS_ HKEY hkForm) PURE;
    STDMETHOD(AddForms)(THIS_ LPCQADDFORMSPROC_A pAddFormsProc, LPARAM lParam) PURE;
    STDMETHOD(AddPages)(THIS_ LPCQADDPAGESPROC_A pAddPagesProc, LPARAM lParam) PURE;
};

#ifdef UNICODE
#define IQueryForm     IQueryFormW
#define IID_IQueryForm IID_IQueryFormW
#else
#define IQueryForm     IQueryFormA
#define IID_IQueryForm IID_IQueryFormA
#endif

//
// Messages for pages
//

#define CQPM_INITIALIZE             0x00000001
#define CQPM_RELEASE                0x00000002
#define CQPM_ENABLE                 0x00000003 // wParam = TRUE/FALSE (enable, disable), lParam = 0
#define CQPM_GETPARAMETERS          0x00000005 // wParam = 0, lParam = -> receives the LocalAlloc
#define CQPM_CLEARFORM              0x00000006 // wParam, lParam = 0
#define CQPM_PERSIST                0x00000007 // wParam = fRead, lParam -> IPersistQuery
#define CQPM_HELP                   0x00000008 // wParam = 0, lParam -> LPHELPINFO
#define CQPM_SETDEFAULTPARAMETERS   0x00000009 // wParam = 0, lParam -> OPENQUERYWINDOW

#define CQPM_HANDLERSPECIFIC        0x10000000

//-----------------------------------------------------------------------------
// IPersistQuery
//-----------------------------------------------------------------------------

// IPersistQuery interface

#undef  INTERFACE
#define INTERFACE IPersistQueryW

DECLARE_INTERFACE_(IPersistQueryW, IPersist)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IPersist
    STDMETHOD(GetClassID)(THIS_ CLSID* pClassID) PURE;

    // IPersistQuery
    STDMETHOD(WriteString)(THIS_ LPCWSTR pSection, LPCWSTR pValueName, LPCWSTR pValue) PURE;
    STDMETHOD(ReadString)(THIS_ LPCWSTR pSection, LPCWSTR pValueName, LPWSTR pBuffer, INT cchBuffer) PURE;
    STDMETHOD(WriteInt)(THIS_ LPCWSTR pSection, LPCWSTR pValueName, INT value) PURE;
    STDMETHOD(ReadInt)(THIS_ LPCWSTR pSection, LPCWSTR pValueName, LPINT pValue) PURE;
    STDMETHOD(WriteStruct)(THIS_ LPCWSTR pSection, LPCWSTR pValueName, LPVOID pStruct, DWORD cbStruct) PURE;
    STDMETHOD(ReadStruct)(THIS_ LPCWSTR pSection, LPCWSTR pValueName, LPVOID pStruct, DWORD cbStruct) PURE;
    STDMETHOD(Clear)(THIS) PURE;
};

#undef  INTERFACE
#define INTERFACE IPersistQueryA

DECLARE_INTERFACE_(IPersistQueryA, IPersist)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IPersist
    STDMETHOD(GetClassID)(THIS_ CLSID* pClassID) PURE;

    // IPersistQuery
    STDMETHOD(WriteString)(THIS_ LPCSTR pSection, LPCSTR pValueName, LPCSTR pValue) PURE;
    STDMETHOD(ReadString)(THIS_ LPCSTR pSection, LPCSTR pValueName, LPSTR pBuffer, INT cchBuffer) PURE;
    STDMETHOD(WriteInt)(THIS_ LPCSTR pSection, LPCSTR pValueName, INT value) PURE;
    STDMETHOD(ReadInt)(THIS_ LPCSTR pSection, LPCSTR pValueName, LPINT pValue) PURE;
    STDMETHOD(WriteStruct)(THIS_ LPCSTR pSection, LPCSTR pValueName, LPVOID pStruct, DWORD cbStruct) PURE;
    STDMETHOD(ReadStruct)(THIS_ LPCSTR pSection, LPCSTR pValueName, LPVOID pStruct, DWORD cbStruct) PURE;
    STDMETHOD(Clear)(THIS) PURE;
};

#ifdef UNICODE
#define IPersistQuery     IPersistQueryW
#define IID_IPersistQuery IID_IPersistQueryW
#else
#define IPersistQuery     IPersistQueryA
#define IID_IPersistQuery IID_IPersistQueryA
#endif


//-----------------------------------------------------------------------------
// ICommonQuery
//-----------------------------------------------------------------------------

#define OQWF_OKCANCEL               0x00000001 // = 1 => Provide OK/Cancel buttons
#define OQWF_DEFAULTFORM            0x00000002 // = 1 => clsidDefaultQueryForm is valid
#define OQWF_SINGLESELECT           0x00000004 // = 1 => view to have single selection (depends on viewer)
#define OQWF_LOADQUERY              0x00000008 // = 1 => use the IPersistQuery to load the given query
#define OQWF_REMOVESCOPES           0x00000010 // = 1 => remove scope picker from dialog
#define OQWF_REMOVEFORMS            0x00000020 // = 1 => remove form picker from dialog
#define OQWF_ISSUEONOPEN            0x00000040 // = 1 => issue query on opening the dialog
#define OQWF_SHOWOPTIONAL           0x00000080 // = 1 => list optional forms by default
#define OQWF_SAVEQUERYONOK          0x00000200 // = 1 => use the IPersistQuery to write the query on close
#define OQWF_HIDEMENUS              0x00000400 // = 1 => no menu bar displayed
#define OQWF_HIDESEARCHUI           0x00000800 // = 1 => dialog is filter, therefore start, stop, new search etc

#define OQWF_PARAMISPROPERTYBAG     0x80000000 // = 1 => the form parameters ptr is an IPropertyBag (ppbFormParameters)

typedef struct
{
    DWORD           cbStruct;                   // structure size
    DWORD           dwFlags;                    // flags (OQFW_*)
    CLSID           clsidHandler;               // clsid of handler we are using
    LPVOID          pHandlerParameters;         // handler specific structure for initialization
    CLSID           clsidDefaultForm;           // default form to be selected (if OQF_DEFAULTFORM == 1 )
    IPersistQueryW* pPersistQuery;              // IPersistQuery used for loading queries
    union
    {
        void*         pFormParameters;
        IPropertyBag* ppbFormParameters;
    };
} OPENQUERYWINDOW_W, * LPOPENQUERYWINDOW_W;

typedef struct
{
    DWORD           cbStruct;                   // structure size
    DWORD           dwFlags;                    // flags (OQFW_*)
    CLSID           clsidHandler;               // clsid of handler we are using
    LPVOID          pHandlerParameters;         // handler specific structure for initialization
    CLSID           clsidDefaultForm;           // default form to be selected (if OQF_DEFAULTFORM == 1 )
    IPersistQueryA* pPersistQuery;              // IPersistQuery used for loading queries
    union
    {
        void*         pFormParameters;
        IPropertyBag* ppbFormParameters;
    };
} OPENQUERYWINDOW_A, * LPOPENQUERYWINDOW_A;

#ifdef UNICODE
#define OPENQUERYWINDOW OPENQUERYWINDOW_W
#define LPOPENQUERYWINDOW LPOPENQUERYWINDOW_W
#else
#define OPENQUERYWINDOW OPENQUERYWINDOW_A
#define LPOPENQUERYWINDOW LPOPENQUERYWINDOW_A
#endif


// ICommonQuery

#undef  INTERFACE
#define INTERFACE ICommonQueryW

DECLARE_INTERFACE_(ICommonQueryW, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // ICommonQuery methods
    STDMETHOD(OpenQueryWindow)(THIS_ HWND hwndParent, LPOPENQUERYWINDOW_W pQueryWnd, IDataObject** ppDataObject) PURE;
};

#undef  INTERFACE
#define INTERFACE ICommonQueryA

DECLARE_INTERFACE_(ICommonQueryA, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS)  PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // ICommonQuery methods
    STDMETHOD(OpenQueryWindow)(THIS_ HWND hwndParent, LPOPENQUERYWINDOW_A pQueryWnd, IDataObject** ppDataObject) PURE;
};

#ifdef UNICODE
#define ICommonQuery     ICommonQueryW
#define IID_ICommonQuery IID_ICommonQueryW
#else
#define ICommonQuery     ICommonQueryA
#define IID_ICommonQuery IID_ICommonQueryA
#endif



#endif  // GUID_DEFS_ONLY
#endif
