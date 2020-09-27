#ifndef _WIZRESERVED_
#define _WIZRESERVED_

// NOTE: you will need to define HTARGET, HFILESET, and uniq_platform to include this file
// These are all traditionally defined in bldapi.h.

// these interfaces are reserved for use within the implementation of the Wizard model
// Users of the Wizard model don't need to know about them.

interface IPrivateLangToLang;	// for private communication between ILanguageServices
interface IPrivateDynamicUI;	// for private communication between class view and lang packages
interface IPrivateDynamicClsFldUI;	// for private communication between class folder and lang packages
interface IPrivateLangToWiz;	// for private communication between lang package and IWizardService

typedef IPrivateLangToLang* LPPRIVATELANGTOLANG;
typedef IPrivateDynamicUI* LPPRIVATEDYNAMICUI;
typedef IPrivateDynamicClsFldUI* LPPRIVATEDYNAMICCLSFLDUI;
typedef IPrivateLangToWiz* LPPRIVATELANGTOWIZ;

// nExtra parameter constants to methods within IPrivateDynamicUI
#define COM_MAP_NONE         0
#define COM_MAP_IMPL         1
#define COM_MAP_IMPL_PROP    2

/////////////////////////////////////////////////////////////////////////////
// IPrivateLangToLang
//   This interface is used for private communication between ILanguageService and ILanguageService

#undef  INTERFACE
#define INTERFACE IPrivateLangToLang
DECLARE_INTERFACE_(IPrivateLangToLang, IUnknown)
{
	STDMETHOD(CreateClassCollection)(short nClassType, CStringList &rstrList, HTARGET hTarget, 
		ILanguageClasses** ppBscColl) PURE;
	STDMETHOD(GetFunctionSet)(LPCSTR lpszInterface, HTARGET hTarget, ILanguageFunctions** ppFuncColl) PURE;
	STDMETHOD(GetClassSet)(short nClassType, LPCSTR lpszInterface, HTARGET hTarget, 
		ILanguageClasses** ppClassColl) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IPrivateDynamicUI
//   This interface is used for private communication between class view and lang packages

#undef  INTERFACE
#define INTERFACE IPrivateDynamicUI
DECLARE_INTERFACE_(IPrivateDynamicUI, IUnknown)
{
	STDMETHOD(GetCmdIDs)(CWordArray& aVerbs, CWordArray& aCmds, int nExtra) PURE;
	STDMETHOD(OnCmdMsg)(UINT nID, int nCode, void* pExtra, LPBOOL lpRetVal, CWnd* slobWnd, int nExtra) PURE;
	STDMETHOD(ExcludeCommonUI)(LPWORD lpwExcludeMask, int nExtra) PURE;
	STDMETHOD(SetPropertyCaption)(BOOL *pSetCaption, CString& strCaption) PURE;
	STDMETHOD(GetBaseClassText)(BOOL *pSetText, CString& strText) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IPrivateLanguageItems
//		Private interface for collections

#undef INTERFACE
#define INTERFACE IPrivateLanguageItems
DECLARE_INTERFACE_(IPrivateLanguageItems, IUnknown)
{
	STDMETHOD(Intersect)(ILanguageItems *pLangItems, CRuntimeClass * const pClass, ILanguageItems **ppLangItemsOut) PURE;
	STDMETHOD(Subtract)(ILanguageItems *pLangItems, CRuntimeClass * const pClass, ILanguageItems **ppLangItemsOut) PURE;
	STDMETHOD(Union)(ILanguageItems *pLangItems) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IPrivateLanguageItem
//		Private interface for language item (class, function, var)

#undef INTERFACE
#define INTERFACE IPrivateLanguageItem
DECLARE_INTERFACE_(IPrivateLanguageItem, IUnknown)
{
	STDMETHOD(GetIinst)(IINST * pIinst) PURE;
	STDMETHOD(CompareParams)(IINST iinst, BOOL * pbResult) PURE;
	STDMETHOD(GetNameString)(short nNameType, CString & rstrName) PURE;
	STDMETHOD(GetBrowserInfo)(SZ* psz, TYP* ptyp, ATR32* patr, BOOL* pbSuccess) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IPrivateDynamicUI
//   This interface is used for private communication between class view folder and lang packages

#undef  INTERFACE
#define INTERFACE IPrivateDynamicClsFldUI
DECLARE_INTERFACE_(IPrivateDynamicClsFldUI, IUnknown)
{
	STDMETHOD(SupportsPlatform)(uniq_platform platform, BOOL* fSupport) PURE;
	STDMETHOD(GetCmdIDs)(HTARGET hTarget, HFILESET hFileSet, CWordArray& aVerbs, CWordArray& aCmds) PURE;
	STDMETHOD(OnCmdMsg)(HTARGET hTarget, HFILESET hFileSet, UINT nID, int nCode, void* pExtra, LPBOOL lpRetVal, CWnd* slobWnd) PURE;
	STDMETHOD(OnUpdateCreateNewClass)(CCmdUI* pCmdUI, BOOL fEmpty) PURE;
	STDMETHOD(OnUpdateInvokeATLComp)(CCmdUI* pCmdUI, HTARGET hTarget) PURE;
	STDMETHOD(OnCreateNewClass)(HTARGET hTarget, CWnd* slobWnd) PURE;
	STDMETHOD(OnInvokeATLComp)(HTARGET hTarget, CWnd* slobWnd) PURE;
	STDMETHOD(OnCreateNewForm)(HTARGET hTarget, CWnd* slobWnd) PURE;
	STDMETHOD(OnUpdateCreateNewForm)(CCmdUI* pCmdUI, HTARGET hTarget, BOOL fEmpty) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IPrivateLangToWiz
//   This interface is used for private communication between lang packages and IWizardService

#undef  INTERFACE
#define INTERFACE IPrivateLangToWiz
DECLARE_INTERFACE_(IPrivateLangToWiz, IUnknown)
{
	STDMETHOD(CreateFolderContext)(IVCWizardContext** pvContext) PURE;
};

#endif	// _WIZRESERVED_


