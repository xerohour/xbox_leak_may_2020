// wizbase.h : header file
//
#ifndef _WIZBASE_
#define _WIZBASE_

// classes in this header
class CWizBarEditorCollection;
class CWizBarClntEditor;
class CWizBarClntLang;
class CWizBarObject;
class CWizBarItem;

#include <wizbar.h>
///#include <clvwguid.h>
#include "..\include\langlib\olecoll.h"
#include <prjapi.h>
//#include <utilapi.h>
//#include <utilguid.h>
//#include <srcapi3.h>
//#include <srcguid.h>
//#include "ncparex.h"


// ***********  generic classes for WizBar 
// Need these classes in every language package that supports wizard bar.

 
class CWizBarEditorCollection : public COleCollection
{
public:
	CWizBarEditorCollection();
	~CWizBarEditorCollection();

 	virtual BOOL Do_Init(LPWIZBARCLNTLANG pWBClntLang);

  	virtual BOOL Init() PURE;  // must be implemented by derived classes. Tells what is available in the collection

protected:
	virtual IID const* PiidMember()	// interface all items must support, if any
		{ return NULL; }

	LPWIZBARCLNTLANG	m_pWBClntLang;

	DECLARE_DYNAMIC(CWizBarEditorCollection)

	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(WizBarClntEditors, IWizBarClntEditors)
		STDMETHOD(_NewEnum)(IEnumVARIANT **ppenum);
	END_INTERFACE_PART(WizBarClntEditors)
};
 


class CWizBarClntLang : public CCmdTarget
	// object that supports the IWizBarClntLang interface. All users derive.
{
	DECLARE_DYNAMIC(CWizBarClntLang)

public:
	CWizBarClntLang(CTheApp *app, LPCTSTR szLanguageName, ILanguageService* pvLangService);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizBarClntLang)
	//}}AFX_VIRTUAL

		// cache the Project Workspace interface
	LPPROJECTWORKSPACE GetWorkspace()
	{
		if (m_pWorkspace == NULL)
			m_pTheApp->FindInterface(IID_IProjectWorkspace, (LPVOID*)&m_pWorkspace);
		return (m_pWorkspace);
	}

	void ReleaseWorkspace()
	{
		if (m_pWorkspace != NULL)
		{
			m_pWorkspace->Release();
			m_pWorkspace = NULL;
		}
	}

protected:
	CTheApp		*m_pTheApp;
	ILanguageService* m_pvLangService;
	CString m_strLanguageName;
	LPPROJECTWORKSPACE m_pWorkspace;


protected:
	~CWizBarClntLang();

	// virtuals that implement the interface (implement in derived classes)

		virtual HRESULT Do_GetLanguageService(ILanguageService** ppvLangService)
		{
			*ppvLangService = m_pvLangService;  // associated ILanguageService, if any
			if (m_pvLangService != NULL)
				(*ppvLangService)->AddRef();
			return S_OK;
		}

		virtual HRESULT Do_GetLanguageName(BSTR* pbstrLang)
		{
			BSTR bstr = m_strLanguageName.AllocSysString();
			*pbstrLang = bstr;
			return S_OK;
		}

	// Dynamic command strings are resource strings in this format:
	// "STRING_COMMAND\nSTRING_MENUTEXT\nSTRING_PROMPT\nSTRING_TIP". 
	// Omit unused portions with adjacent \n's. NULL will be returned for these
	// which will cause called to extract string from the command table
	// the total string must be <= 1024 bytes
	// Command strings are in the order described by the defined indexes used as iString:
	// #define STRING_COMMAND	0
	// #define STRING_MENUTEXT 1
	// #define STRING_PROMPT   2
	// #define STRING_TIP		3

		virtual HRESULT Do_GetDynamicCmdString(UINT nID, UINT iString, BSTR* pbstrCmd)
		{
			*pbstrCmd = NULL;
			return NOERROR;
		}

  		virtual HRESULT	Do_GetCmdDefault(WBContext *pwbcontext, UINT *pnIDDefault, BOOL *pfHandled)
		{
			*pfHandled = FALSE;
			*pnIDDefault = (UINT)(-1);
			return NOERROR;
		}

	   	virtual HRESULT Do_FSupportsWBActivate(UINT *pbfSupportsWB) PURE;
		virtual HRESULT Do_InitWBLangData(UINT bfSupportsWB) PURE;
		virtual HRESULT Do_ClearWBLangData() PURE;

		virtual HRESULT Do_CompleteWBContext(WBContext * pwbcontext, WB_LIST_TYPE wbltFirstEmpty) PURE;
		virtual HRESULT Do_FillWizBarList (WBContext * pwbcontext, WB_LIST_TYPE wblType, BOOL *pfSortedByFiller) PURE;
		virtual HRESULT Do_CreateWBMenuAction(WBContext * pwbcontext, int *piMenuDefault) PURE; 
		virtual HRESULT Do_DoWBAction(WBContext * pwbcontext, UINT idAction, BOOL *pfHandledAction) PURE;
		virtual HRESULT Do_GetCurWBCFromTrackData(WBContextTrack * pwbctrack, WBContext * pwbcontext) PURE;
		virtual HRESULT Do_GetAllClntEditors(LPWIZBARCLNTLANG pWBClntLang, LPWIZBARCLNTEDITORS* ppvClntEditors);  // associated IWizBarClntEditors interface

		virtual HRESULT Do_GetWBItemText (void * pwbItem, WB_LIST_TYPE wblType, BSTR* pbstrText, BOOL fTextForEdit) PURE;
		virtual HRESULT Do_DrawWBItemGlyph (void * pwbItem, WB_LIST_TYPE wblType, HDC hDC, RECT *lpRect, UINT *puDrawStringFlags) PURE;
		virtual HRESULT Do_CompareWBItems (const void * pwbItem1, const void * pwbItem2, WB_LIST_TYPE wblType, int *pRetCompare) PURE;
		virtual HRESULT Do_AddRefWBItem (void * pwbItem, WB_LIST_TYPE wblType) PURE;
		virtual HRESULT Do_ReleaseWBItem (void * pwbItem, WB_LIST_TYPE wblType) PURE;


		virtual	CWizBarEditorCollection *GetNewEditorCollection() PURE;

		// get a project type id from the build system
		HRESULT GetProjectID (LPPROJECTWORKSPACE pWWInterface, CLSID *pClsidProj);

	DECLARE_INTERFACE_MAP()

	// Wizard Bar Client Language interface
	BEGIN_INTERFACE_PART(WizBarClntLang, IWizBarClntLang)
		INIT_INTERFACE_PART(CWizBarClntLang, WizBarClntLang)

   		STDMETHOD(FSupportsWBActivate)(UINT *pbfSupportsWB);
		STDMETHOD(InitWBLangData)(UINT bfSupportsWB);
		STDMETHOD(ClearWBLangData)();

		STDMETHOD(CompleteWBContext)(WBContext * pwbcontext, WB_LIST_TYPE wbltFirstEmpty);
		STDMETHOD(FillWizBarList) (WBContext * pwbcontext, WB_LIST_TYPE wblType, BOOL *pfSortedByFiller);
		STDMETHOD(CreateWBMenuAction)(WBContext * pwbcontext, int *piMenuDefault); 
		STDMETHOD(DoWBAction)(WBContext * pwbcontext, UINT idAction, BOOL *pfHandledAction);
		STDMETHOD(GetCurWBCFromTrackData)(WBContextTrack * pwbctrack, WBContext * pwbcontext);

		STDMETHOD(GetLanguageService)(ILanguageService** ppvLangService);  // associated ILanguageService
		STDMETHOD(GetLanguageName)(BSTR* pbstrLang);  // associated language name

		STDMETHOD(GetAllClntEditors)(LPWIZBARCLNTEDITORS* ppvClntEditors);  // associated IWizBarClntEditors interface

		STDMETHOD(GetWBItemText) (void * pwbItem, WB_LIST_TYPE wblType, BSTR* pbstrText, BOOL fTextForEdit);
		STDMETHOD(DrawWBItemGlyph) (void * pwbItem, WB_LIST_TYPE wblType, HDC hDC, RECT *lpRect, UINT *puDrawStringFlags);
		STDMETHOD(CompareWBItems) (const void * pwbItem1, const void * pwbItem2, WB_LIST_TYPE wblType, int *pRetCompare);
		STDMETHOD(AddRefWBItem) (void * pwbItem, WB_LIST_TYPE wblType);
		STDMETHOD(ReleaseWBItem) (void * pwbItem, WB_LIST_TYPE wblType);

		STDMETHOD(GetDynamicCmdString)(UINT nID, UINT iString, BSTR* pbstrCmd);
		STDMETHOD(GetCmdDefault)(WBContext *pwbcontext, UINT *pnIDDefault, BOOL *pfHandled);

	END_INTERFACE_PART(WizBarClntLang)

};


class CWizBarClntEditor : public CCmdTarget
	// object that supports the IWizBarClntEditor interface. All users derive.
{
public:
	CWizBarClntEditor(CTheApp* app, const GUID &rguidEditor, LPCTSTR lpszLang, LPWIZBARCLNTLANG pWBClntLang);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizBarClntEditor)
	//}}AFX_VIRTUAL


protected:
	CTheApp				*m_pTheApp;
	LPWIZBARCLNTLANG	m_pWBClntLang;
	const GUID			&m_rguidEditor;
	CString				m_strLang;

protected:
	~CWizBarClntEditor();

	// virtuals that implement the interface (implement in derived classes)
   		virtual HRESULT Do_FSupportsWBActivate(UINT *pbfSupportsWB) PURE;
		virtual HRESULT Do_InitWBEditorData(UINT bfSupportsWB) PURE;
		virtual HRESULT Do_ClearWBEditorData() PURE;
		virtual HRESULT Do_GetCurWBContext(WBContext * pwbcontext, BOOL *pfUnchanged) PURE;
		virtual HRESULT Do_AddWBMenuAction(WBContext * pwbcontext) PURE;
		virtual HRESULT Do_DoWBAction(WBContext * pwbcontext, UINT idAction, BOOL *pfHandledAction) PURE;

		virtual HRESULT Do_GetLangAndEditor(BSTR* pbstrLang, GUID* pguidEditor) 
		{
			BSTR bstr = m_strLang.AllocSysString();
			*pbstrLang = bstr;
			*pguidEditor = m_rguidEditor;
			return NOERROR;
		}

		virtual HRESULT Do_GetIClntLang(LPWIZBARCLNTLANG *ppWBClntLang)
		{
			m_pWBClntLang->AddRef();
			*ppWBClntLang =  m_pWBClntLang;
			return NOERROR;
		}

		// Dynamic command strings are resource strings in this format:
		// "STRING_COMMAND\nSTRING_MENUTEXT\nSTRING_PROMPT\nSTRING_TIP". 
		// Omit unused portions with adjacent \n's. NULL will be returned for these
		// which will cause called to extract string from the command table
		// the total string must be <= 1024 bytes
		// Command strings are in the order described by the defined indexes used as iString:
		// #define STRING_COMMAND	0
		// #define STRING_MENUTEXT 1
		// #define STRING_PROMPT   2
		// #define STRING_TIP		3

		virtual HRESULT Do_GetDynamicCmdString(UINT nID, UINT iString, BSTR* pbstrCmd)
		{
			*pbstrCmd = NULL;
			return NOERROR;
		}


		 virtual HRESULT Do_GetCmdDefault(WBContext *pwbcontext, UINT *pnIDDefault, BOOL *pfHandled)
		{
			*pfHandled = FALSE;
			*pnIDDefault = (UINT)(-1);
			return NOERROR;
		}

	DECLARE_INTERFACE_MAP()

		// Wizard Bar Client Editor interface 
	BEGIN_INTERFACE_PART(WizBarClntEditor, IWizBarClntEditor)
		INIT_INTERFACE_PART(CWizBarClntEditor, WizBarClntEditor)

   		STDMETHOD(FSupportsWBActivate)(UINT *pbfSupportsWB);
		STDMETHOD(InitWBEditorData)(UINT bfSupportsWB); 	
		STDMETHOD(ClearWBEditorData)();

		STDMETHOD(GetCurWBContext)(WBContext * pwbcontext, BOOL *pfUnchanged);
		STDMETHOD(AddWBMenuAction)(WBContext * pwbcontext);
		STDMETHOD(DoWBAction)(WBContext * pwbcontext, UINT idAction, BOOL *pfHandledAction);
		STDMETHOD(GetLangAndEditor)(BSTR* pbstrLang, GUID* pguidEditor);
		STDMETHOD(GetIClntLang)(LPWIZBARCLNTLANG *ppWBClntLang);  // get associated IWizBarClntLang interface

		STDMETHOD(GetDynamicCmdString)(UINT nID, UINT iString, BSTR* pbstrCmd);
		STDMETHOD(GetCmdDefault)(WBContext *pwbcontext, UINT *pnIDDefault, BOOL *pfHandled);

	END_INTERFACE_PART(WizBarClntEditor)

};


// addreffable objects usedin wizard bar

// Classes such as CWizBarItem derive from this. It is a fairly cheap objec that supports addref and release

class CWizBarObject : public CObject
{
public:
	DECLARE_DYNAMIC(CWizBarObject)

	CWizBarObject()
	{
		m_dwRef = 1;
	}
	virtual ~CWizBarObject() 
	{
		ASSERT(m_dwRef <= 1);
	}

	virtual void AddRef()
	{
		m_dwRef++;
	}

	virtual void Release()
	{
		m_dwRef--;
		ASSERT(m_dwRef >= 0);
		if (m_dwRef < 1)
			delete (this);
	}

protected:
	long m_dwRef;	// used for addref and release
};


// objects stored in wizard bar lists

// Clients derive from this class to put entries into the wizard bar lists
// the derived classes destructor must take care of deleting the contents
// of these objects

class CWizBarItem : public CWizBarObject
{
public:
	DECLARE_DYNAMIC(CWizBarItem)

	CWizBarItem()
	{
	}
	virtual ~CWizBarItem()
	{
	}

	// Temporary in nature - Don't hold this pointer and don't write to it.
	virtual LPCTSTR GetText(BOOL fTextForEdit) PURE;
	virtual int Compare(CWizBarItem *pcwbitem) PURE;

};


#endif	// _WIZBASE_
