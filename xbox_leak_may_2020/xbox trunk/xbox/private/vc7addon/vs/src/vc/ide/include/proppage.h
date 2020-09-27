/////////////////////////////////////////////////////////////////////////////
//  PROPPAGE.H
//      Defines classes which can be used to display a property page
//      to view a CSlob's property map

#ifndef __PROPPAGE_H__
#define __PROPPAGE_H__

#include "dlgbase.h"

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA

class CSlob;
class CSheetWnd;

// CControlMap is a structure that maps control ids to property ids and
// control types.  An array of these is defined for each property dialog
// by using the macros below (DECLARE_PROP_CONTROL_MAP, BEGIN_PROP_CONTROL_MAP,
// END_CONTROL_MAP, etc.)

class CEnum;

struct CControlMap
{
    UINT m_nCtlID;      // dialog id for m_nCtlType == page
    UINT m_nLastCtlID;  // name string id for m_nCtlType == page

    UINT m_nProp;       // optional page help ID for m_nCtlType == page

    enum CTL_TYPE
    {
        null,
        check,
        list,
        checkList,
        comboList,
        comboText,
        autoComboList,
        edit,
        autoEdit,
        radio,
        page,
        editInt,        // FUTURE: some of these types could be removed,
        editNum,        // and achieved through flags.  But no hurry.
        thinText,
        thickText,
        icon,
        editStrCap,
        bitmap,
        listText,
        symbolCombo,
        pathText,
    } m_nCtlType;

    DWORD m_dwFlags;

    UINT_PTR m_nExtra;      // extra UINT, eg. used for offset in RADIO

//  union {
//      CEnum* m_pEnum; // for lists and combos
//  } u;
};

// flags for CControlMap::m_dwFlags:
enum
{
    CM_EXTRA_ENUM   = 0x001,    // m_nExtra is a CEnum*
    CM_EXTRA_LIMIT  = 0x002,    // m_nExtra is a limit value (UINT)
    CM_EXTRA_OFFSET = 0x003,    // m_nExtra is an offset
    CM_EXTRA_HELPID = 0x004,    // m_nExtra is a HelpID (of a page)
    CM_EXTRA_MASK   = 0x0ff,    // for (m_dwFlags & CM_EXTRA_MASK) == CM_EXTRA_*
    CM_NOAMBIGUOUS  = 0x100,    // disable control if value is ambiguous
    CM_NOMULTISEL   = 0x200,    // disable control for any multiple selection
};


// Include this inside a CSlobPage-derived class declaration.
#define DECLARE_PROP_CONTROL_MAP() \
    virtual CControlMap* GetControlMap(); \
    static CControlMap m_controlMap [];

// Include these with the implementation of a CSlobPage-derived class.
#define BEGIN_PROP_CONTROL_MAP(theClass, nDlgID, nNameID) \
    CControlMap* theClass::GetControlMap() \
        { return &m_controlMap[0]; } \
    CControlMap theClass::m_controlMap [] = { \
        { nDlgID, nNameID, 0, CControlMap::page, 0x0, NULL },

#define BEGIN_PROP_CONTROL_MAP_H(theClass, nDlgID, nNameID, nHelpID) \
    CControlMap* theClass::GetControlMap() \
        { return &m_controlMap[0]; } \
    CControlMap theClass::m_controlMap [] = { \
        { nDlgID, nNameID, 0, CControlMap::page, CM_EXTRA_HELPID, nHelpID },

#define END_PROP_CONTROL_MAP() { 0, 0, 0, CControlMap::null, 0x0, NULL } };


// These macros define CControlMap structures for inclusion between
// the BEGIN_PROP_CONTROL_MAP and END_CONTROL_MAP macros.
#define MAP_CHECK(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::check,      0x0,  NULL },
#define MAP_LIST(nCtlID, nProp, pEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::list,       CM_EXTRA_ENUM, (UINT_PTR)&pEnum },
#define MAP_LIST_TEXT(nCtlID, nProp, pEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::listText,   CM_EXTRA_ENUM, (UINT_PTR)&pEnum },
#define MAP_CHECK_LIST(nCtlID, pEnum) \
    { nCtlID,  nCtlID,  0,      CControlMap::checkList,  CM_EXTRA_ENUM, (UINT_PTR)&pEnum },
#define MAP_COMBO_LIST(nCtlID, nProp, pEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::comboList,  CM_EXTRA_ENUM, (UINT_PTR)&pEnum },
#define MAP_COMBO_LIST_EX(nCtlID, nProp, theEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::comboList,  CM_EXTRA_ENUM, (UINT_PTR)&(C##theEnum::GetObject()) },
#define MAP_COMBO_TEXT(nCtlID, nProp, pEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::comboText,  CM_EXTRA_ENUM, (UINT_PTR)&pEnum },
#define MAP_AUTO_COMBO_LIST(nCtlID, nProp, pEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::autoComboList, CM_EXTRA_ENUM, (UINT_PTR)&pEnum },
#define MAP_COMBO_TEXT_EX(nCtlID, nProp, theEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::comboText,  CM_EXTRA_ENUM, (UINT_PTR)&(C##theEnum::GetObject()) },
#define MAP_AUTO_COMBO_LIST_EX(nCtlID, nProp, theEnum) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::autoComboList,  CM_EXTRA_ENUM, (UINT_PTR)&(C##theEnum::GetObject()) },
#define MAP_EDIT(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::edit,       0x0,  NULL },
#define MAP_EDIT_LIMIT(nCtlID, nProp, nLimit) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::edit,       CM_EXTRA_LIMIT, nLimit },
#define MAP_EDIT_INT(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::editInt,    0x0,  NULL },
#define MAP_EDIT_NUM(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::editNum,    0x0,  NULL },
#define MAP_AUTO_EDIT(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::autoEdit,   0x0,  NULL },
#define MAP_AUTO_EDIT_LIMIT(nCtlID, nProp, nLimit) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::autoEdit,   CM_EXTRA_LIMIT, nLimit },
#define MAP_RADIO(nFirstCtlID, nLastCtlID, nOffset, nProp) \
    { nFirstCtlID, nLastCtlID, nProp, CControlMap::radio,  CM_EXTRA_OFFSET, nOffset },
#define MAP_THIN_TEXT(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::thinText,   0x0,  NULL },
#define MAP_THICK_TEXT(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::thickText,  0x0,  NULL },
#define MAP_ICON(nCtlID) \
    { nCtlID,  nCtlID,  0,      CControlMap::icon,       0x0,  NULL },
#define MAP_EDIT_STRCAP(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::editStrCap, 0x0,  NULL },
#define MAP_BITMAP(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::bitmap,     CM_NOAMBIGUOUS,  NULL },
#define MAP_SYMBOL_COMBO(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::symbolCombo, 0x0, NULL },
#define MAP_PATH_TEXT(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::pathText,   0x0,  NULL },
#define MAP_EDIT_NOAMBIG(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::edit,       CM_NOAMBIGUOUS,  NULL },
#define MAP_EDIT_NOMULTISEL(nCtlID, nProp) \
    { nCtlID,  nCtlID,  nProp,  CControlMap::edit,       CM_NOMULTISEL,  NULL },

class C3dPropertyPage : public C3dDialog
{
    DECLARE_DYNAMIC(C3dPropertyPage)

public:
    C3dPropertyPage();

    virtual BOOL Create(UINT nIDSheet, CWnd* pWndOwner) = 0;

    virtual BOOL SetupPage(CSheetWnd* pSheetWnd, CSlob* pSlob);
    virtual void Activate(UINT nState, CSlob* pCurSlob);
    
    virtual void InitializePage();
    virtual void TermPage();
    virtual BOOL ShowPage(int nCmdShow);
    virtual void MovePage(const CRect& rect);

    virtual BOOL Validate() = 0;
    virtual BOOL UndoPendingValidate() = 0;

    virtual void OnActivate();
    virtual void OnDeactivate();
    virtual LRESULT OnPageHelp(WPARAM wParam, LPARAM lParam);

    virtual void GetPageName(CString& strName);
    virtual CSize GetPageSize();
    virtual BOOL IsPageActive() { return m_hWnd != NULL; }
    virtual BOOL IsPageDisabled() { return FALSE; }

    CSlob* GetSlob() { return m_pSlob; }

protected:
    CSlob* m_pSlob;
    CSheetWnd* m_pSheetWnd;

    friend class CSheetWnd;
    friend class CMultiSlob;
};

/////////////////////////////////////////////////////////////////////////////
//  COlePage property page

class COlePage : public C3dPropertyPage
{
    DECLARE_DYNAMIC(COlePage)

public:
    // Page caching.
    static CObList s_listPages;
    static LPUNKNOWN *s_pObjectCurrent;
    static LPUNKNOWN s_pSingleObject;
    static ULONG s_nObjectCurrent;
    static UINT s_nPagesCurrent;
    static LPCLSID s_lpClsID;
    static BOOL s_bPossibleUnusedServers;


//$UNDONE HACK HACK HACK get rid of this when the resource package changes its
// data bound control handling
    static BOOL s_fShowAllPage;
    static BOOL GetShowAllPage();
    static void SetShowAllPage(BOOL fSet);

    static UINT LoadPages(ULONG nUnkCnt, LPUNKNOWN *pprgUnk);
    static UINT LoadAllPageOnly(ULONG nUnkCnt, LPUNKNOWN *pprgUnk);
    static BOOL InPageCache(ULONG nUnkCnt, LPUNKNOWN *pprgUnk);
    static UINT AddAllPageToList(UINT nPagesCurrent, CAUUID *pcaGUID);
    static HRESULT DoPageIntersection(ISpecifyPropertyPages *pSPP, CAUUID *pcaGUID);
    static C3dPropertyPage* GetPropPage(UINT iPage);
    static C3dPropertyPage* GetPropPage(REFCLSID clsid);
    static void Cleanup();
    static void SetUnusedServers(BOOL fUnused = TRUE);

protected:
    COlePage(REFCLSID clsid);   // Use GetPropPage()
    ~COlePage();

public:
    BOOL IsUsable()
        { return m_lpPropPage != NULL; }

    virtual BOOL Create(UINT nIDSheet, CWnd* hWndOwner);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    virtual void InitializePage();
    virtual void TermPage();
    virtual BOOL ShowPage(int nCmdShow);
    virtual void MovePage(const CRect& rect);
    virtual BOOL DestroyWindow();

    virtual BOOL Validate();
    virtual BOOL UndoPendingValidate();

    virtual LRESULT OnPageHelp(WPARAM wParam, LPARAM lParam);

    virtual void GetPageName(CString& strName);
    virtual CSize GetPageSize();
    virtual BOOL IsPageActive() { return m_bActive; }

    void EditProperty(DISPID dispid);


// Attributes
protected:
    BOOL m_bActive:1;
    BOOL m_bVisible:1;
    BOOL m_bTranslatingAccel:1;

    CLSID m_clsid;
    CString m_strName;
    CSize m_size;

    LPPROPERTYPAGE m_lpPropPage;

// Interface Maps   
protected:
    // IPropertyPageSite
    BEGIN_INTERFACE_PART(PropertyPageSite, IPropertyPageSite)
        INIT_INTERFACE_PART(COlePage, PropertyPageSite)
        STDMETHOD(OnStatusChange)(DWORD);
        STDMETHOD(GetLocaleID)(LCID FAR*);
        STDMETHOD(GetPageContainer)(LPUNKNOWN FAR*);
        STDMETHOD(TranslateAccelerator)(LPMSG);
    END_INTERFACE_PART(PropertyPageSite)

    DECLARE_INTERFACE_MAP()
};

// CSlobPages are CDialogs that use the CSlob property mechanism to
// automaticly handle the dialog.

class CSlobPage : public C3dPropertyPage
{
public:
    DECLARE_PROP_CONTROL_MAP()
    DECLARE_DYNAMIC(CSlobPage)

public:
    CSlobPage();
    ~CSlobPage();

    virtual BOOL Create(UINT nIDSheet, CWnd* pWndOwner);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    virtual void InitializePage();
    virtual void InitPage();

    BOOL EnablePage(BOOL bEnable = TRUE);
    virtual BOOL IsPageDisabled() { return !m_bEnabled; }

    virtual BOOL Validate();
    virtual BOOL UndoPendingValidate();

    virtual LRESULT OnPageHelp(WPARAM wParam, LPARAM lParam)
            {
                return C3dPropertyPage::OnPageHelp(wParam, lParam);
            }

    virtual BOOL OnPropChange(UINT nProp);
    virtual void GetPageName(CString& strName);

    CControlMap* FindControl(UINT nCtlID);
    CControlMap* FindProp(UINT nProp);

    void GetGuid (GUID & guid) 
    {
        guid = m_guid;
    }
protected:
    GUID m_guid;

private:
    BOOL m_bDirty;
    UINT m_idDlg;
    UINT m_idCaption;
    CString m_strPageName;
    SIZE m_sizePage;
    CString m_strDocString;
    CString m_strHelpFile;
    DWORD m_dwHelpContext;
    LPPROPERTYPAGESITE m_pPageSite;

    LPDISPATCH* m_ppDisp;       // Array of IDispatch pointers, used to
                                // access the properties of each control

    LPDWORD m_pAdvisors;        // Array of connection tokens used by
                                // IConnecitonPoint::Advise/UnAdvise.

    BOOL m_bPropsChanged;       // IPropertyNotifySink::OnChanged has been
                                // called, but not acted upon yet.

    ULONG m_nObjects;           // Objects in m_ppDisp, m_ppDataObj, m_pAdvisors

    BOOL m_bInitializing;       // TRUE if the contents of the fields of
                                // the dialog box are being initialized

    int m_nControls;            // Number of fields on this property page

//  AFX_PPFIELDSTATUS* m_pStatus;   // Array containing information on
                                    // which fields are dirty

    CDWordArray m_IDArray;      // Array containing information on which
                                // controls to ignore when deciding if
                                // the apply button is to be enabled

    HGLOBAL m_hDialog;          // Handle of the dialog resource

#ifdef _DEBUG
protected:
    BOOL m_bNonStandardSize;
#endif

// LIST OF INTERFACE MAPS
public:
    // IPropertyPage2
    BEGIN_INTERFACE_PART(PropertyPage, IPropertyPage2)
        INIT_INTERFACE_PART(CSlobPage, PropertyPage)
        STDMETHOD(SetPageSite)(LPPROPERTYPAGESITE);
        STDMETHOD(Activate)(HWND, LPCRECT, BOOL);
        STDMETHOD(Deactivate)();
        STDMETHOD(GetPageInfo)(LPPROPPAGEINFO);
        STDMETHOD(SetObjects)(ULONG, LPUNKNOWN*);
        STDMETHOD(Show)(UINT);
        STDMETHOD(Move)(LPCRECT);
        STDMETHOD(IsPageDirty)();
        STDMETHOD(Apply)();
        STDMETHOD(Help)(LPCOLESTR);
        STDMETHOD(TranslateAccelerator)(LPMSG);
        STDMETHOD(EditProperty)(DISPID);
    END_INTERFACE_PART(PropertyPage)

    // IPropertyNotifySink
    BEGIN_INTERFACE_PART(PropNotifySink, IPropertyNotifySink)
        INIT_INTERFACE_PART(CSlobPage, PropNotifySink)
        STDMETHOD(OnRequestEdit)(DISPID);
        STDMETHOD(OnChanged)(DISPID);
    END_INTERFACE_PART(PropNotifySink)

    DECLARE_INTERFACE_MAP()

protected:
    // m_nValidateID is set to the ID of a control that will need to be validated
    // by Validate() as soon as that control has been changed.  It will
    // be NULL if nothing needs to be validated.  This will mainly be
    // used by edits and combos that are normally validated when they
    // loose the focus.  Needs to be in here for the Escape accelerator.

    int m_nValidateID;

    BOOL m_bIgnoreChange:1;
    BOOL m_bEnabled:1;

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    void LoadDlgTemplate (UINT nIDSheet);
    DECLARE_MESSAGE_MAP()
};

void StringEditorEditToProp(char* szBuf, int *len);
void StringEditorPropToEdit(CString& str);

/////////////////////////////////////////////////////////////////////////////
// List and Combo Box Enumerations...

// One entry in an enumeration
struct CEnumerator
{
    const char* szId;
    int val;
};

class CPropCheckList;

// An enumeration
class CEnum
{
public:
    virtual void FillListBox(CListBox* pWnd, BOOL bClear = TRUE, CSlob* = NULL);
    virtual void FillCheckList(CPropCheckList* pWnd, BOOL bClear = TRUE, CSlob* = NULL);
    virtual void FillComboBox(CComboBox* pWnd, BOOL bClear = TRUE, CSlob* = NULL);

    virtual BOOL ContainsVal(int val);
    virtual POSITION GetHeadPosition();
    virtual CEnumerator* GetNext(POSITION& pos);

    virtual CEnumerator* GetList()
        { VSASSERT(FALSE, "override of GetList required"); return(NULL); };

    virtual int GetCount ();
    virtual BOOL FindStringID (int val, CString & strID); 
    virtual HRESULT GetPredefinedStrings (CALPOLESTR* pCaStringsOut,CADWORD* pCaCookiesOut);
};


// this is a enum designed to store its strings in the string table

// One entry in a localized enumeration
struct CLocalizedEnumerator
{
    CLocalizedEnumerator( UINT aId, int aVal );
    ~CLocalizedEnumerator();
    CString szId;
    UINT id;
    int val;
};

// A localized enumeration
class CLocalizedEnum : public CEnum
{
public:
    CLocalizedEnum (){};

    virtual void FillListBox(CListBox* pWnd, BOOL bClear = TRUE, CSlob* = NULL);
    virtual void FillCheckList(CPropCheckList* pWnd, BOOL bClear = TRUE, CSlob* = NULL);
    virtual void FillComboBox(CComboBox* pWnd, BOOL bClear = TRUE, CSlob* = NULL);

    virtual BOOL ContainsVal(int val);
    virtual POSITION GetHeadPosition();
    virtual CLocalizedEnumerator* GetNextL(POSITION& pos);
    virtual CEnumerator* GetNext(POSITION& pos)
        { VSASSERT( FALSE, "You must use GetNextL()" ); return (NULL); };
    virtual CLocalizedEnumerator *GetListL()
        { VSASSERT( FALSE, "You must override GetListL" ); return (NULL); };
    virtual int GetCount ();
    virtual BOOL FindStringID (int val, CString & strID); 
    virtual HRESULT GetPredefinedStrings (CALPOLESTR* pCaStringsOut,CADWORD* pCaCookiesOut);
    virtual void GetOSDependantLangNeutral(CString & strLangNeutral);
};

// A sorted localized enumeration
class CLocalizedEnumEx : public CLocalizedEnum
{
public:
    CLocalizedEnumEx () : bUseVerLanguageName(TRUE), bIsSorted(FALSE) {};

    virtual void InitList();
    static int CompareEnumerator(const CLocalizedEnumerator * lhs, const CLocalizedEnumerator * rhs);

    //HACK - for language enumerations, we need to call VerLanguageName instead of using the contents of szId.
    unsigned bUseVerLanguageName    : 1;
    unsigned bIsSorted              : 1; 
};


// Helper macros for making enumerations
#define DECLARE_ENUM_LIST() \
    public: \
    virtual CEnumerator* GetList() \
                { return &c_list[0]; } \
    static CEnumerator c_list [];

// Define the enumerator identifiers and values in this table
#define BEGIN_ENUM_LIST(theClass) \
    CEnumerator theClass::c_list [] = {

#define END_ENUM_LIST() \
    { NULL, 0 } };

// Define a simple CEnum derived class
#define DEFINE_ENUM(theEnum) \
    class C##theEnum : public CEnum { DECLARE_ENUM_LIST() }; \
    C##theEnum NEAR theEnum; \
    BEGIN_ENUM_LIST(C##theEnum)

#define DECLARE_ENUM(theEnum) \
    class C##theEnum : public CEnum { DECLARE_ENUM_LIST() }; \
    extern C##theEnum NEAR theEnum;

// Helper macros for making localized enumerations
#define DECLARE_LOCALIZED_ENUM_LIST() \
    public: \
    virtual CLocalizedEnumerator* GetListL() \
                { return &c_list[0]; } \
    static CLocalizedEnumerator c_list [];

// Define the enumerator identifiers and values in this table
#define BEGIN_LOCALIZED_ENUM_LIST(theClass) \
    CLocalizedEnumerator theClass::c_list [] = {

// A localized enumeration entry
#define LOCALIZED_ENUM_ENTRY(id,val) \
    CLocalizedEnumerator( id, val ),

#define END_LOCALIZED_ENUM_LIST() \
    CLocalizedEnumerator( 0, 0 ) };

// Define a simple CLocalizedEnum derived class
#define DEFINE_LOCALIZED_ENUM(theEnum) \
    class C##theEnum : public CLocalizedEnum { DECLARE_LOCALIZED_ENUM_LIST() }; \
    C##theEnum NEAR theEnum; \
    BEGIN_LOCALIZED_ENUM_LIST(C##theEnum)

#define DECLARE_LOCALIZED_ENUM(theEnum) \
    class C##theEnum : public CLocalizedEnum { DECLARE_LOCALIZED_ENUM_LIST() }; \
    extern C##theEnum NEAR theEnum;

#define DEFINE_LOCALIZED_ENUM_EX(theEnum) \
    class C##theEnum : public CLocalizedEnum \
{ \
public: \
    static C##theEnum & GetObject() \
    { \
    static C##theEnum s_enum; \
    return s_enum; \
    }; \
    virtual CLocalizedEnumerator * GetListL() \
    { \
        static CLocalizedEnumerator c_list [] = { 
        

#define END_LOCALIZED_ENUM_EX() \
    CLocalizedEnumerator( 0, 0 ) }; \
    CLocalizedEnumerator* pEnum = c_list; \
    while (pEnum->id != 0) \
    { \
        BOOL bOK = pEnum->szId.LoadString(pEnum->id); \
        VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?"); \
        pEnum++; \
    } \
    return &c_list[0];  \
    } ; \
};

#define DEFINE_LOCALIZED_ENUM_EX2(theEnum) \
    class C##theEnum : public CLocalizedEnumEx \
{ \
public: \
    static C##theEnum & GetObject() \
    { \
    static C##theEnum s_enum; \
    return s_enum; \
    }; \
    virtual CLocalizedEnumerator * GetListL() \
    { \
        static CLocalizedEnumerator c_list [] = { 
        
#define END_LOCALIZED_ENUM_EX2() \
    CLocalizedEnumerator( 0, 0 ) }; \
    char sz [1024]; \
    CLocalizedEnumerator* pEnum = c_list; \
    if (bIsSorted) \
        return c_list; \
    bIsSorted = TRUE; \
    CString strLangNeutral; \
    GetOSDependantLangNeutral(strLangNeutral); \
    while (pEnum->id != 0) \
    { \
        VerLanguageName(pEnum->val, sz, 100); \
        if (strLangNeutral.Compare(sz)) \
        { \
            pEnum->szId.Empty(); \
            pEnum->szId = sz; \
        } \
        else \
        { \
            if (0 != pEnum->val && 2048 != pEnum->val) \
            { \
                CString str; \
                str.Format(IDS_LANG_ID, pEnum->szId); \
                pEnum->szId.Empty(); \
                pEnum->szId = str; \
            } \
            else \
            { \
                BOOL bOK = pEnum->szId.LoadString(pEnum->id); \
                VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?"); \
            } \
        } \
        pEnum++; \
    } \
    pEnum = c_list; \
    qsort(pEnum + 3, GetCount() - 3, sizeof(CLocalizedEnumerator), reinterpret_cast<int (__cdecl *)(const void *, const void *)>(CompareEnumerator)); \
    return &c_list[0];  \
    } ; \
};

/////////////////////////////////////////////////////////////////////////////
// Property Browser API

#define WM_USER_VALIDATEREQ (WM_USER + 3)

void SetPropertyBrowserVisible(BOOL bVisible); // show/hide and set visible state
void ShowPropertyBrowser(BOOL bShow = TRUE);   // show/hide (temporary)
BOOL IsPropertyBrowserVisible();               // visibility test
void PinPropertyBrowser(BOOL bPin = TRUE);     // toggles the pushpin when visible
BOOL IsPropertyBrowserPinned();                // pinned test
void ResetPropertyBrowserSelectionSlob(CSlob* pSlob);

void InvalidatePropertyBrowser();              // repaint property browser
void UpdatePropertyBrowser();                  // force page to reflect selection
BOOL ValidatePropertyBrowser();                // validate pending changes
void ActivatePropertyBrowser();                // activate and show
void DeactivatePropertyBrowser();              // return focus to the app
void CancelPropertyBrowser();                  // cancel changes
void ClosePropertyBrowser();                   // really just hides it and sets popping off
BOOL BeginPropertyBrowserEdit(UINT nProp,      // edit a specific property
    UINT nChar = 0, UINT nRepeat = 0, UINT nFlags = 0);
BOOL BeginPropertyGridEdit(UINT dispid,         // edit a specific property
    UINT nChar = 0, UINT nRepeat = 0, UINT nFlags = 0);

CWnd* GetPropertyBrowserControl(UINT nID);     // get a control on for the current object
C3dPropertyPage* GetNullPropertyPage();        // get null page (has ID controls)
C3dPropertyPage* GetCurrentPropertyPage();     // current propert page
void SetPropertyBrowserDefPage();              // make the currect page the default
BOOL IsPropertyBrowserInCancel();              // did the user just cancel
BOOL IsPropertyBrowserValidating();            // is the page being validated
BOOL FlipToPage (CString str);                 // flipping to another page

// Methods used during CSlob::SetupPropertyPages()
int AddPropertyPage(C3dPropertyPage* pPage, CSlob* pSlob);
void SetPropertyCaption(LPCTSTR sz);
BOOL AppendExtraPropertyPages(void);
BOOL InhibitExtraPropertyPages(BOOL bInhibit = TRUE);
int StartNewPropertyPageSet(void);
int MergePropertyPageSets(void);

//#undef AFX_DATA
//#define AFX_DATA NEAR

#endif // __PROPPAGE_H__
