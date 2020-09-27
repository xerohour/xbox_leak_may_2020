// SettingsPage.h : Declaration of the CSettingsPage

#ifndef __SETTINGSPAGE_H_
#define __SETTINGSPAGE_H_

#include "vsshell.h"	   // IVsPropertyPage
#include "vsmanaged.h"		// IVSMDPropertyGrid
#include "vcprojectengine.h"		// g_pProjectEngine;
#include "DTE.h"
#include "projbld2.h"
#include "vcmap.h"
#include "vshelp.h"
#include "ocdesign.h"
#include "profile.h"
#include "DirControl.h"

class COptionHandlerBase;

#include <vcdlg.h>
	
EXTERN_C const CLSID CLSID_SettingsPage;
#define INHERIT_PROP_VALUE -2
#define EDIT_PROP_VALUE -3

// static count of edit dialogs to skip
static int s_cDlgs = 0;
// static to indicate that we need to set the prop directly and not bother
// with edit dlgs...
static bool s_bSetPropNow = false;

static CComPtr<IVSMDPropertyGrid> s_pGrid;
static int s_cGrids = 0;
static void* s_pLastActive = NULL;

HRESULT OpenTopic(IVCProjectEngineImpl *pProjEngineImpl, BSTR bstrTopic);

/////////////////////////////////////////////////////////////////////////////
// CSettingsPageBase

#define CSettingsPageBaseTemplate template <class T, const CLSID * Tpclsid, DWORD TTitleID, DWORD TGroupID>
#define CSettingsPageBaseType CSettingsPageBase<T, Tpclsid, TTitleID, TGroupID>

template <class T, const CLSID * Tpclsid = &CLSID_NULL, DWORD TTitleID = 0 , DWORD TGroupID = 0>
class ATL_NO_VTABLE CSettingsPageBase :
	public IPropertyPageImpl<T>,
	public IVsPropertyPage,
	public IVCSettingsPage,
	public IVsPropertyPageNotify,
	public CDialogImpl<T>,
	public CComObjectRoot,
	public CComCoClass<T, Tpclsid>
{
public:
	BOOL		m_bActive;
	UINT		m_nLastObjects;
	IUnknown ** m_ppUnkArray;

	CSettingsPageBase();
	~CSettingsPageBase();

	enum {IDD = IDDP_SETTINGSPAGE};

DECLARE_NO_REGISTRY()
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(T) 
	COM_INTERFACE_ENTRY(IPropertyPage)
	COM_INTERFACE_ENTRY(IVsPropertyPage)
	COM_INTERFACE_ENTRY(IVsPropertyPageNotify)
	COM_INTERFACE_ENTRY(IVCSettingsPage)
END_COM_MAP()

BEGIN_MSG_MAP(T)
	CHAIN_MSG_MAP(IPropertyPageImpl<T>)
END_MSG_MAP()

	IVSMDPropertyGrid* GetGrid();
		
// Handler prototypes:
//	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//	LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IVSPropertyPageNotify
	STDMETHOD(OnShowPage)(BOOL fActivated);

// IVSPropertyPage
	STDMETHOD(get_CategoryTitle)( UINT iLevel, BSTR *pbstrCategory );

// IPropertyPage
	STDMETHOD(Apply)(void);
	STDMETHOD(Help)(LPCOLESTR pszHelpDir);
	STDMETHOD(Deactivate)(void);
	STDMETHOD(Activate)(HWND hWndParent, LPCRECT pRect, BOOL /* bModal */);
	STDMETHOD(GetPageInfo)(PROPPAGEINFO* pPageInfo);
	STDMETHOD(SetObjects)(ULONG nObjects, IUnknown **ppUnk);
	STDMETHOD(TranslateAccelerator)(MSG *pMsg);

// IVCSettingsPage
	STDMETHOD(Dirty)( void );
	STDMETHOD(get_NumObjects)( int* pNumObjects );
	STDMETHOD(GetObject)( int index, IUnknown **ppUnkObject );
	STDMETHOD(Refresh)( void );
	STDMETHOD(UpdateObjects)( void );

protected:
	virtual void DoCreatePageObject(UINT i, IDispatch* pDisp, IVCPropertyContainer* pLiveContainer, IVCSettingsPage* pPage);
};

// use this class if you don't have anything you need to override in CSettingsPageBase
template <const CLSID * Tpclsid = &CLSID_NULL, DWORD TTitleID = 0 , DWORD TGroupID = 0>
class ATL_NO_VTABLE CSettingsPage :
	public CSettingsPageBase<CSettingsPage, Tpclsid, TTitleID, TGroupID>
{
};

#define CPageObjectImplTemplate template<class T, long nMin, long nMax>
#define CPageObjectImplType CPageObjectImpl<T, nMin, nMax>

template<class T, long nMin, long nMax>
class CPageObjectImpl :
	public IVCPropertyContainer,
	public IVCPropertyPageObject,
	public IVsPerPropertyBrowsing,
	public IProvidePropertyBuilder,
	public IPerPropertyBrowsing
{
public:
	typedef CPageObjectImpl<T, nMin , nMax> CBase;
	CComPtr<IVCPropertyContainer> m_pContainer;
	CComPtr<IVCSettingsPage> m_pPage;
	CVCMap<long, long, CComVariant, VARIANT&> m_PropMap;

	~CPageObjectImpl(){}

	static HRESULT CreateInstance(IUnknown **pI, IVCPropertyContainer *pContainer, IVCSettingsPage *pPage);

// IVCPropertyPageObject
public:
	STDMETHOD(Close)();		// turn this object into a ZOMBIE
	
// ISpecifyPropertyPages (base for IVCPropertyContainer)
public:
	STDMETHOD(GetPages)(CAUUID*) { return E_NOTIMPL; }

// IVCPropertyContainer
public:
	STDMETHOD(Commit)();
	STDMETHOD(DirtyProp)(long id) { return S_FALSE; }
	STDMETHOD(ClearAll)();
	STDMETHOD(GetLocalProp)(long id, /*[out]*/ VARIANT *pvarValue);
	STDMETHOD(GetParentProp)(long id, VARIANT_BOOL bAllowInherit, VARIANT *pvarValue);
	STDMETHOD(Clear)(long id);
	STDMETHOD(SetProp)(long id, VARIANT varValue);
	STDMETHOD(GetProp)(long id, /*[in]*/ VARIANT *pVarValue);
	STDMETHOD(GetMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue);
	STDMETHOD(GetEvaluatedMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bCollapseMultiples, 
		VARIANT_BOOL bCaseSensitive, BSTR* pbstrValue);
	STDMETHOD(IsMultiProp)(long id, VARIANT_BOOL* pbIsMultiProp);
	STDMETHOD(GetMultiPropSeparator)(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);
	STDMETHOD(GetStrProperty)(long idProp, BSTR* pbstrValue);
	STDMETHOD(GetEvaluatedStrProperty)(long idProp, BSTR* pbstrValue);
	STDMETHOD(SetStrProperty)(long idProp, BSTR bstrValue);
	STDMETHOD(GetIntProperty)(long idProp, long* pnValue);
	STDMETHOD(SetIntProperty)(long idProp, long nValue);
	STDMETHOD(GetBoolProperty)(long idProp, VARIANT_BOOL* pbValue);
	STDMETHOD(SetBoolProperty)(long idProp, VARIANT_BOOL bValue);
	STDMETHOD(Evaluate)(BSTR bstrIn, BSTR *bstrOut) { return E_NOTIMPL; }
	STDMETHOD(EvaluateWithValidation)(BSTR bstrSource, long idProp, BSTR* pbstrExpanded)
		{ return E_NOTIMPL; }	// TODO: use this to validate whether the input string is biting its own tail...
	STDMETHOD(HasLocalStorage)(VARIANT_BOOL /* bForSave */, VARIANT_BOOL* pbHasLocalStorage);
	STDMETHOD(GetStagedPropertyContainer)(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppStagedContainer);
	STDMETHOD(ReleaseStagedPropertyContainer)();		// intended for use ONLY by the staged property container itself

	// this helper is for anybody who manipulates settings outside its own ID range (like the config general page does)
	HRESULT CommitSingleProp(long id);

	HRESULT GetEnumBoolProperty(long idProp, enumBOOL* pbValue);
	HRESULT GetEnumBoolProperty2(long idProp, long* pbValue, COptionHandlerBase* pOptHandler = NULL);
	HRESULT ToolGetIntProperty(long idProp, COptionHandlerBase* pOptHandler, long* pVal);
	HRESULT ToolGetStrProperty(long idProp, COptionHandlerBase* pOptHandler, BSTR* pVal, bool bLocalOnly = false);
	HRESULT ToolGetBoolProperty(long idProp, COptionHandlerBase* pOptHandler, VARIANT_BOOL* pVal);

// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)( DISPID dispid, BOOL *pfHide);
	STDMETHOD(DisplayChildProperties)( DISPID dispid, BOOL *pfDisplay);
	STDMETHOD(GetLocalizedPropertyInfo)( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc);
	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault);
	STDMETHOD(IsPropertyReadOnly)( DISPID dispid, BOOL *fReadOnly);
	STDMETHOD(GetClassName)(BSTR* ) {return E_NOTIMPL;} 
	STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* ) {return E_NOTIMPL;}	
	STDMETHOD(ResetPropertyValue)(DISPID ) {return E_NOTIMPL;}

// IPerPropertyBrowsing methods
public:
	STDMETHOD(MapPropertyToPage)( DISPID dispID, CLSID *pClsid ){ return E_NOTIMPL; }
	STDMETHOD(GetDisplayString)( DISPID dispID, BSTR *pBstr );
	STDMETHOD(GetPredefinedStrings)( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut );
	STDMETHOD(GetPredefinedValue)( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut );
	
// IProvidePropertyBuilder
public:
	STDMETHOD(MapPropertyToBuilder)( LONG dispid, LONG *pdwCtlBldType, BSTR *pbstrGuidBldr, VARIANT_BOOL *pfRetVal );
	STDMETHOD(ExecuteBuilder)( LONG dispid, BSTR bstrGuidBldr, IDispatch *pdispApp, LONG_PTR hwndBldrOwner, 
		VARIANT *pvarValue, VARIANT_BOOL *pfRetVal );

// helper functions
public:
	virtual void ResetParentIfNeeded() {}
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual long KludgeLocID(long id) { return id; }
	virtual BOOL UseDirectoryPickerDialog(long id) { return FALSE; }
	virtual BOOL UseCommandsDialog(long id) { return FALSE; }
	virtual BOOL UseSinglePropDirectoryPickerDialog(long id) { return FALSE; }
	virtual BOOL UseMultiLineNoInheritDialog(long id) { return FALSE; }
	virtual BOOL IsDir(long id) { return FALSE; }
	virtual BOOL IsFile(long id) { return FALSE; }
	virtual BOOL SupportsMultiLine(long id) { return FALSE; }
	virtual void GetPreferredSeparator(long id, CStringW& strSeparator) { strSeparator = L";"; }
	virtual void GetLocalizedName(DISPID dispID, MEMBERID memid, ITypeInfo2* pTypeInfo2Enum, long lVal, CComBSTR& bstrDoc);
	BOOL GetTruncatedStringForProperty(DISPID dispid, CStringW & strOut);
	void RefreshPropGrid(void);
};

/////////////////////////////////////////////////////////////////////////////
// CAdditionalOptionsPage

#define CAdditionalOptionsPageTemplate template<class I, const CLSID * Tpclsid, DWORD TGroupID, long nID >
#define CAdditionalOptionsPageType CAdditionalOptionsPage<I, Tpclsid, TGroupID, nID>

template <class I, const CLSID * Tpclsid = &CLSID_NULL, DWORD TGroupID = 0, long nID >
class ATL_NO_VTABLE CAdditionalOptionsPage :
	public IPropertyPageImpl<CAdditionalOptionsPage>,
	public IVsPropertyPage,
	public IVCSettingsPage,
	public CVCAxDialogImpl<CAdditionalOptionsPage>,
	public CComObjectRoot,
	public CComCoClass<CAdditionalOptionsPage, Tpclsid>
{
public:
	BOOL m_bActive;
	HFONT m_hFont;

	CAdditionalOptionsPage();
	~CAdditionalOptionsPage() {}

	enum {IDD = IDDP_ADDITIONALPAGE};

DECLARE_NO_REGISTRY()
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAdditionalOptionsPage) 
	COM_INTERFACE_ENTRY(IPropertyPage)
	COM_INTERFACE_ENTRY(IVsPropertyPage)
	COM_INTERFACE_ENTRY(IVCSettingsPage)
END_COM_MAP()

BEGIN_MSG_MAP(CAdditionalOptionsPage)
	COMMAND_HANDLER(IDC_ADDOPTIONS, EN_CHANGE, OnChange)
	MESSAGE_HANDLER(WM_PAINT, OnFocus)
	CHAIN_MSG_MAP(IPropertyPageImpl<CAdditionalOptionsPage>)
END_MSG_MAP()

// Handler prototypes:
//	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//	LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	STDMETHOD(Dirty)( void );
	STDMETHOD(get_NumObjects)( int* pNumObjects );
	STDMETHOD(GetObject)( int index, IUnknown **ppUnkObject );
	STDMETHOD(Refresh)( void );
	STDMETHOD(get_CategoryTitle)( UINT iLevel, BSTR *pbstrCategory );
	STDMETHOD(Apply)(void);
	STDMETHOD(Help)(LPCOLESTR pszHelpDir);
	STDMETHOD(Deactivate)(void);
	STDMETHOD(Activate)(HWND hWndParent, LPCRECT pRect, BOOL /* bModal */);
	STDMETHOD(GetPageInfo)(PROPPAGEINFO* pPageInfo);
	STDMETHOD(SetObjects)(ULONG nObjects, IUnknown **ppUnk);
	STDMETHOD(TranslateAccelerator)(MSG *pMsg);

private:
	HRESULT UpdateObjects( bool bUpdateAllOnly = false );
};


#define EDITSTRINGS_WIDTH						44.0
#define EDITSTRINGS_BORDER_WIDTH				 2.0
#define EDITSTRINGS_BUTTON_HEIGHT				 2.0
#define EDITSTRINGS_COLLAPSED_HEIGHT			21.6
#define EDITSTRINGS_ROW1_TOP					15.0
#define EDITSTRINGS_ROW2_TOP					17.5
#define EDITSTRINGS_BUTTON_TOP1					 2.5					
#define EDITSTRINGS_COLLAPSED_BOTTOM_EDGE		22.0
#define EDITSTRINGS_RIGHT						10.0
#define EDITSTRINGS_MACROPICKER_LEFT			EDITSTRINGS_WIDTH - (EDITSTRINGS_BORDER_WIDTH + 9.0)
#define EDITSTRINGS_INSERTMACRO_LEFT			EDITSTRINGS_WIDTH - (EDITSTRINGS_BORDER_WIDTH + 40.5)
#define EDITSTRINGS_CANCEL_LEFT					EDITSTRINGS_WIDTH - (EDITSTRINGS_BORDER_WIDTH + 19.5)
#define EDITSTRINGS_OK_LEFT						EDITSTRINGS_WIDTH - (EDITSTRINGS_BORDER_WIDTH + 30.0)
#define EDITSTRINGS_RESIZE_EDGE					 0.5
#define EDITSTRINGS_RESIZE_TOP					10.0
#define EDITSTRINGS_BUTTON_WIDTH				10.0

#define EDITSTRINGS_KEY							L"VC\\EditSettingsStrings"
#define EDITSTRINGS_SAVED_HEIGHT				L"Height"
#define EDITSTRINGS_SAVED_WIDTH					L"Width"
#define EDITSTRINGS_SINGLE_SAVED_HEIGHT			L"SingleLineHeight"
#define EDITSTRINGS_SINGLE_SAVED_WIDTH			L"SingleLineWidth"

/////////////////////////////////////////////////////////////////////////////
// CBaseEditStringDlg

#define CBaseEditStringDlgTemplate template<class T>
#define CBaseEditStringDlgType CBaseEditStringDlg<T>

template <class T>
class CBaseEditStringDlg : 
	public CVCAxDialogImpl<T>
{
public:
	CBaseEditStringDlg(IVCPropertyContainer* pPropCnt, wchar_t* wszName, wchar_t* wszProp);

	// caller must free the return value
	CComBSTR GetPropertyString( void ) 
		{ return m_bstrProperty; }

	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT DoOnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT DoOnPaint();
	LRESULT DoOnSetFocus();
	LRESULT DoOnMacros();
	LRESULT DoOnInsertMacro();
	LRESULT DoOnOk();
	LRESULT DoOnCancel();
	LRESULT DoOnHelp();
	virtual LRESULT DoOnMinMaxInfo( LPMINMAXINFO lpMinMax );

	void DoSetWindowFont(int idControl);
	void SizeButton(int idControl, double left, double top, double right, double bottom);
	void SizeSubWindow(int idControl, double left, double top, double right, double bottom);
	void SetExpansionHeight();
	double GetExpansionExtent();
	void SetupScreenUnits();
	void RetrieveDynamicInfo(long id, CComBSTR& bstrBuffer);
	void DoCleanup();

protected:
	virtual void Reset() {}
	virtual void RetrieveInfo() {}
	virtual long GetMacroPickerParent() PURE;
	virtual long GetMacroPickerTop() { return m_nExpansionHeight + m_yBorder; }
	virtual void DoChildOnSize() {}
	virtual void DoChildInit() {}
	virtual BOOL MacroPickerSupported() { return TRUE; }
	virtual HWND GetMacroPickerParentEditWindow() { return GetDlgItem(GetMacroPickerParent()); }
	virtual LPCOLESTR GetSavedWidthString() { return EDITSTRINGS_SAVED_WIDTH; }
	virtual LPCOLESTR GetSavedHeightString() { return EDITSTRINGS_SAVED_HEIGHT; }
	virtual void ResetParentBottom(RECT& rect) { rect.bottom -= m_nExpansionHeight; }
	void ReleaseMacroPicker();
	void CleanUpString( CComBSTR & bstrVal, CStringW& strPreferredSeparator, CStringW& strAllSeparators );

protected:
	CComPtr<IVCPropertyContainer> m_pPropContainer;
	CComPtr<IVCMacroPicker> m_pMacroPicker;
	CAxWindow m_wndMacroPicker;
	CComBSTR m_bstrName;
	CComBSTR m_bstrProperty;
	SIZE m_unit;
	HFONT m_hFont;
	HPEN m_hpenBtnHilite;
	HPEN m_hpenBtnShadow;
	long m_nExpansionHeight;
	WORD m_wOriginalHeight;
	WORD m_wHeight;
	WORD m_wWidth;
	int m_xBorder;
	int m_yBorder;
	int m_yCaptionSize;
	int m_cxResizeBox;
	int m_buttonWidth;
	bool m_bExpanded;
	bool m_bInitialSizeDone;
	bool m_bInPaint;
};

/////////////////////////////////////////////////////////////////////////////
// CMultiLineDlgBase

#define CMultiLineDlgBaseTemplate template<class T, WORD wID>
#define CMultiLineDlgBaseType CMultiLineDlgBase<T, wID>

template <class T, WORD wID>
class CMultiLineDlgBase : 
	public CBaseEditStringDlg<T>
{
public:
	CMultiLineDlgBase( IVCPropertyContainer *pPropCnt, long propid, wchar_t *wszName, wchar_t *wszProp, wchar_t *wszInherit );
	~CMultiLineDlgBase() {}

BEGIN_MSG_MAP( T )
	MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
	MESSAGE_HANDLER( WM_SIZE, OnSize )
	MESSAGE_HANDLER( WM_PAINT, OnPaint )
	MESSAGE_HANDLER( WM_GETMINMAXINFO, OnMinMaxInfo )
	COMMAND_HANDLER( wID, EN_SETFOCUS, OnSetFocus )
	COMMAND_ID_HANDLER( IDOK, OnOk )	
	COMMAND_ID_HANDLER( IDCANCEL, OnCancel )	
	COMMAND_ID_HANDLER( IDC_SETTINGSEDIT_HELP, OnHelp )
	COMMAND_ID_HANDLER( IDC_MACROPICKER, OnMacros )
	COMMAND_ID_HANDLER( IDC_INSERTMACRO, OnInsertMacro )	
END_MSG_MAP()

	// handlers:
	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return CBaseEditStringDlg<T>::OnInitDialog(uMsg, wParam, lParam, bHandled); }
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return CBaseEditStringDlg<T>::DoOnSize(uMsg, wParam, lParam, bHandled); }
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return CBaseEditStringDlg<T>::DoOnPaint(); }
	LRESULT OnMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return CBaseEditStringDlg<T>::DoOnMinMaxInfo((LPMINMAXINFO)lParam); }
	LRESULT OnSetFocus( WORD, UINT, HWND, BOOL& )
		{ return CBaseEditStringDlg<T>::DoOnSetFocus(); }
	LRESULT OnOk( WORD, UINT, HWND, BOOL& )
		{ return CBaseEditStringDlg<T>::DoOnOk(); }
	LRESULT OnCancel( WORD, UINT, HWND, BOOL& )
		{ return CBaseEditStringDlg<T>::DoOnCancel(); }
	LRESULT OnHelp( WORD, UINT, HWND, BOOL& )
		{ return CBaseEditStringDlg<T>::DoOnHelp(); }
	LRESULT OnMacros( WORD, UINT, HWND, BOOL& )
		{ return CBaseEditStringDlg<T>::DoOnMacros(); }
	LRESULT OnInsertMacro( WORD, UINT, HWND, BOOL& )
		{ return CBaseEditStringDlg<T>::DoOnInsertMacro(); }

protected:
	virtual void RetrieveSpecificInfo(CComBSTR& bstrFixedData) PURE;

	virtual long GetMacroPickerTop() { return (long)(m_nExpansionHeight + m_yBorder + 0.5*m_unit.cx); }
	virtual long GetMacroPickerParent() { return wID; }
	virtual void RetrieveInfo();
	void CheckForInheritOnOpen( CComBSTR & bstrVal );
	bool HasInheritMacros( BSTR bstrVal );
	void DoSharedChildOnSize(double& dTotalUnits, double& dWindowUnits);
	void DoSharedChildInit();
	void DoSharedReset(CComBSTR& bstrFormatted);
	static int FindNoCaseW( const CStringW & str, const wchar_t* const substr, int pos = 0 );

protected:
	long m_lPropId;
	CComBSTR m_bstrInherit;
	bool m_bUserSetInherit;
	CStringW m_strPreferredSeparator;
	CStringW m_strAllSeparators;
};	

/////////////////////////////////////////////////////////////////////////////
// CMultiLineDlg
class CMultiLineDlg: 
	public CMultiLineDlgBase<CMultiLineDlg, IDC_MULTILINELIST>
{
public:
	CMultiLineDlg( IVCPropertyContainer *pPropCnt, long propid, wchar_t *wszName, wchar_t *wszProp, wchar_t *wszInherit ) 
		: CMultiLineDlgBase<CMultiLineDlg, IDC_MULTILINELIST>( pPropCnt, propid, wszName, wszProp, wszInherit )
	{}

	enum { IDD = IDDP_MULTI_LINE };

protected:
	virtual void DoChildOnSize();
	virtual void DoChildInit();
	virtual void Reset();

	virtual void RetrieveSpecificInfo(CComBSTR& bstrFixedData);
};

/////////////////////////////////////////////////////////////////////////////
// CMultiDirectoryPickerEditorDlg
class CMultiDirectoryPickerEditorDlg :
	public CMultiLineDlgBase<CMultiDirectoryPickerEditorDlg, IDC_DIRCONTROL>
{
public:
	CMultiDirectoryPickerEditorDlg( IVCPropertyContainer *pPropCnt, long propid, wchar_t *wszName, wchar_t *wszProp, wchar_t *wszInherit ) 
		: CMultiLineDlgBase<CMultiDirectoryPickerEditorDlg, IDC_DIRCONTROL>( pPropCnt, propid, wszName, wszProp, wszInherit )
	{}
	enum { IDD = IDDP_MULTI_DIRECTORY_PICKER_EDITOR };

protected:
	virtual void DoChildOnSize();
	virtual void DoChildInit();
	virtual void Reset();

	virtual void RetrieveSpecificInfo(CComBSTR& bstrFixedData);
	virtual BOOL MacroPickerSupported() { return FALSE; }
	virtual HWND GetMacroPickerParentEditWindow() { return NULL; }

protected:
	CAxWindow m_wndDirList;
	CComPtr<IDirList> m_pList;
};

/////////////////////////////////////////////////////////////////////////////
// CSingleLineDlgBase
template <class T, WORD wID>
class CSingleLineDlgBase : 
	public CBaseEditStringDlg<T>
{
public:
	CSingleLineDlgBase( IVCPropertyContainer *pPropCnt, wchar_t *wszName, wchar_t *wszProp ) :
		CBaseEditStringDlg<T>(pPropCnt, wszName, wszProp)
	{}

BEGIN_MSG_MAP( T )
	MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
	MESSAGE_HANDLER( WM_SIZE, OnSize )
	MESSAGE_HANDLER( WM_PAINT, OnPaint )
	MESSAGE_HANDLER( WM_GETMINMAXINFO, OnMinMaxInfo )
	COMMAND_HANDLER( wID, EN_SETFOCUS, OnSetFocus )
	COMMAND_ID_HANDLER( IDOK, OnOk )	
	COMMAND_ID_HANDLER( IDCANCEL, OnCancel )	
	COMMAND_ID_HANDLER( IDC_SETTINGSEDIT_HELP, OnHelp )
	COMMAND_ID_HANDLER( IDC_MACROPICKER, OnMacros ) 
	COMMAND_ID_HANDLER( IDC_INSERTMACRO, OnInsertMacro )	
END_MSG_MAP()

	// handlers:
	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return CBaseEditStringDlg<T>::OnInitDialog(uMsg, wParam, lParam, bHandled); }
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return DoOnSize(uMsg, wParam, lParam, bHandled); }
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return DoOnPaint(); }
	LRESULT OnMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
		{ return DoOnMinMaxInfo((LPMINMAXINFO)lParam); }
	LRESULT OnSetFocus( WORD, UINT, HWND, BOOL& )
		{ return DoOnSetFocus(); }
	LRESULT OnOk( WORD, UINT, HWND, BOOL& )
		{ return DoOnOk(); }
	LRESULT OnCancel( WORD, UINT, HWND, BOOL& )
		{ return DoOnCancel(); }
	LRESULT OnHelp( WORD, UINT, HWND, BOOL& )
		{ return DoOnHelp(); }
	LRESULT OnMacros( WORD, UINT, HWND, BOOL& )
		{ return DoOnMacros(); }
	LRESULT OnInsertMacro( WORD, UINT, HWND, BOOL& )
		{ return DoOnInsertMacro(); }

protected:
	virtual long GetMacroPickerParent() { return wID; }
};

/////////////////////////////////////////////////////////////////////////////
// CSingleLineDlg
template <class T>
class CSingleLineDlg : 
	public CSingleLineDlgBase<T, IDC_STRING_EDIT>
{
public:
	CSingleLineDlg( IVCPropertyContainer *pPropCnt, wchar_t *wszName, wchar_t *wszProp ) :
		CSingleLineDlgBase<T, IDC_STRING_EDIT>(pPropCnt, wszName, wszProp)
	{}
	~CSingleLineDlg() 
	{}

protected:
	virtual void DoChildInit()
	{
		if (m_hFont)
			DoSetWindowFont(IDC_STRING_EDIT);
	}
};

/////////////////////////////////////////////////////////////////////////////
// CSingleLineNoReturnsDlg
class CSingleLineNoReturnsDlg : 
	public CSingleLineDlg<CSingleLineNoReturnsDlg>
{
public:
	CSingleLineNoReturnsDlg( IVCPropertyContainer *pPropCnt, wchar_t *wszName, wchar_t *wszProp ) :
		CSingleLineDlg<CSingleLineNoReturnsDlg>(pPropCnt, wszName, wszProp)
	{}
	~CSingleLineNoReturnsDlg()
	{}

	enum { IDD = IDD_STRING_EDIT };

	virtual LRESULT DoOnMinMaxInfo( LPMINMAXINFO lpMinMax );

protected:
	virtual void Reset();
	virtual void RetrieveInfo();
	virtual void DoChildOnSize();
	virtual LPCOLESTR GetSavedWidthString() { return EDITSTRINGS_SINGLE_SAVED_WIDTH; }
	virtual LPCOLESTR GetSavedHeightString() { return EDITSTRINGS_SINGLE_SAVED_HEIGHT; }
	virtual long GetMacroPickerTop() { return (long)(3.8*m_unit.cy) + m_yBorder; }
	virtual void ResetParentBottom(RECT& rect);
};

/////////////////////////////////////////////////////////////////////////////
// CSingleLineWithReturnsDlg
class CSingleLineWithReturnsDlg : 
	public CSingleLineDlg<CSingleLineWithReturnsDlg>
{
public:
	CSingleLineWithReturnsDlg( IVCPropertyContainer *pPropCnt, wchar_t *wszName, wchar_t *wszProp, const wchar_t* wszSeparator ) :
		CSingleLineDlg<CSingleLineWithReturnsDlg>(pPropCnt, wszName, wszProp),
		m_strSeparator(wszSeparator)
	{}

	~CSingleLineWithReturnsDlg()
	{}

	enum { IDD = IDD_STRING_EDIT_WITH_RETURNS };

protected:
	virtual void Reset();
	virtual void RetrieveInfo();
	virtual void DoChildOnSize();
	
protected:
	CStringW m_strSeparator;
};

/////////////////////////////////////////////////////////////////////////////
// CSingleDirectoryPickerEditorDlg
class CSingleDirectoryPickerEditorDlg: 
	public CSingleLineDlgBase<CSingleDirectoryPickerEditorDlg, IDC_DIRCONTROL>
{
public:
	CSingleDirectoryPickerEditorDlg( IVCPropertyContainer *pPropCnt, wchar_t *wszName, wchar_t *wszProp ) :
		CSingleLineDlgBase<CSingleDirectoryPickerEditorDlg, IDC_DIRCONTROL>(pPropCnt, wszName, wszProp)
	{}

	enum { IDD = IDDP_SINGLE_DIRECTORY_PICKER_EDITOR };

protected:
	virtual void Reset();
	virtual void RetrieveInfo();
	virtual void DoChildOnSize();
	virtual void DoChildInit();
	virtual BOOL MacroPickerSupported() { return FALSE; }
	virtual HWND GetMacroPickerParentEditWindow() { return NULL; }

protected:
	CAxWindow m_wndDirList;
	CComPtr<IDirList> m_pList;
};

/////////////////////////////////////////////////////////////////////////////
// CCommandsLineDlg
class CCommandsDlg: 
	public CBaseEditStringDlg<CCommandsDlg>
{
public:
	CCommandsDlg( IVCPropertyContainer *pPropCnt, wchar_t *wszName, wchar_t *wszProp ) :
		CBaseEditStringDlg<CCommandsDlg>(pPropCnt, wszName, wszProp)
	{
	}

	~CCommandsDlg()
	{
	}

	enum { IDD = IDD_COMMANDS_EDIT };

BEGIN_MSG_MAP( CCommandsDlg )
	MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
	MESSAGE_HANDLER( WM_SIZE, OnSize )
	MESSAGE_HANDLER( WM_PAINT, OnPaint )
	MESSAGE_HANDLER( WM_GETMINMAXINFO, OnMinMaxInfo )
	COMMAND_HANDLER( IDC_COMMANDS_EDIT, EN_SETFOCUS, OnSetFocus )
	COMMAND_ID_HANDLER( IDOK, OnOk )	
	COMMAND_ID_HANDLER( IDCANCEL, OnCancel )	
	COMMAND_ID_HANDLER( IDC_SETTINGSEDIT_HELP, OnHelp )
	COMMAND_ID_HANDLER( IDC_MACROPICKER, OnMacros ) 
	COMMAND_ID_HANDLER( IDC_INSERTMACRO, OnInsertMacro )	
END_MSG_MAP()

	// handlers:
	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnSetFocus( WORD, UINT, HWND, BOOL& ); 
	LRESULT OnOk( WORD, UINT, HWND, BOOL& );
	LRESULT OnCancel( WORD, UINT, HWND, BOOL& );
	LRESULT OnHelp( WORD, UINT, HWND, BOOL& );
	LRESULT OnMacros( WORD, UINT, HWND, BOOL& );
	LRESULT OnInsertMacro( WORD, UINT, HWND, BOOL& );
	
protected:
	virtual void Reset();
	virtual void RetrieveInfo();
	virtual long GetMacroPickerParent() { return IDC_COMMANDS_EDIT; }
	virtual void DoChildOnSize();
	virtual void DoChildInit();
};

/////////////////////////////////////////////////////////////////////////////
// CMultiLineNoInheritDlg
class CMultiLineNoInheritDlg: 
	public CBaseEditStringDlg<CMultiLineNoInheritDlg>
{
public:
	CMultiLineNoInheritDlg( IVCPropertyContainer *pPropCnt, wchar_t *wszName, wchar_t *wszProp ) :
		CBaseEditStringDlg<CMultiLineNoInheritDlg>(pPropCnt, wszName, wszProp)
	{
		m_strPreferredSeparator = L";";
		m_strAllSeparators = L";,";
	}

	~CMultiLineNoInheritDlg()
	{
	}

	enum { IDD = IDDP_MULTI_LINE_NOINHERIT };

BEGIN_MSG_MAP( CMultiLineNoInheritDlg )
	MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
	MESSAGE_HANDLER( WM_SIZE, OnSize )
	MESSAGE_HANDLER( WM_PAINT, OnPaint )
	MESSAGE_HANDLER( WM_GETMINMAXINFO, OnMinMaxInfo )
	COMMAND_HANDLER( IDC_MULTILINELIST, EN_SETFOCUS, OnSetFocus )
	COMMAND_ID_HANDLER( IDOK, OnOk )	
	COMMAND_ID_HANDLER( IDCANCEL, OnCancel )	
	COMMAND_ID_HANDLER( IDC_SETTINGSEDIT_HELP, OnHelp )
	COMMAND_ID_HANDLER( IDC_MACROPICKER, OnMacros ) 
	COMMAND_ID_HANDLER( IDC_INSERTMACRO, OnInsertMacro )	
END_MSG_MAP()

	// handlers:
	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnSetFocus( WORD, UINT, HWND, BOOL& ); 
	LRESULT OnOk( WORD, UINT, HWND, BOOL& );
	LRESULT OnCancel( WORD, UINT, HWND, BOOL& );
	LRESULT OnHelp( WORD, UINT, HWND, BOOL& );
	LRESULT OnMacros( WORD, UINT, HWND, BOOL& );
	LRESULT OnInsertMacro( WORD, UINT, HWND, BOOL& );
	
protected:
	virtual void Reset();
	virtual void RetrieveInfo();
	virtual long GetMacroPickerParent() { return IDC_MULTILINELIST; }
	virtual void DoChildOnSize();
	virtual void DoChildInit();

protected:
	CStringW m_strPreferredSeparator;
	CStringW m_strAllSeparators;
};

#include "SettingsPage.inl"

#endif //__SETTINGSPAGE_H_
