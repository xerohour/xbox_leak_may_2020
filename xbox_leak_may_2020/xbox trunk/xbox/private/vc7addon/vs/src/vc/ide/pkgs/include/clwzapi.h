/////////////////////////////////////////////////////////////////////////////
//	CLWZAPI.H
//		Class Wizard package interface declarations.

#ifndef __CLWZAPI_H__
#define __CLWZAPI_H__

#ifdef _WIZ_INTERNAL
#define WIZ_EXPORT _declspec(dllexport)
#else
#define WIZ_EXPORT _declspec(dllimport)
#endif

__interface IClassWizard;
__interface IChangeClasses;
__interface ICreateClasses2;


typedef IClassWizard* LPCLASSWIZARD;


/////////////////////////////////////////////////////////////////////////////
// IClassWizard
//   This interface is used to access ClassWizard features.  The interface
//   is implemented in the CPP package which implibs to the classwizard DLL.
//

#undef INTERFACE
#define INTERFACE IClassWizard

class CITextDoc;
class CClsWizClass;
#include "ogapi.h"	// need IApplyContext
#include "atlobj.h"  // for OleDB COM object.

#define MAX_LINECOUNTS	4

// Defines for Verb Flags
#define CWV_VIRTFUNCT	0x2
#define CWV_PROTFUNCT	0x4
#define CWV_CONSTFUNCT	0x8
#define CWV_ADVFUNCT	0x10
#define CWV_NOTIFY		0x20	/* verb is a type of WM_NOTIFY */
#define CWV_REFLECTED	0x40	/* verb represents a Windows control message which has been
								   reflected to the message map in the control class.
								 */
#define CWV_WM			0x80	/* verb is a window message (currently this flag is
								 * used only with CWV_REFLECTED).
								 */
#define CWV_NIL			0

// CClassWizInterface::Run return values
#define CODEWIZ_FAILED	0
#define CODEWIZ_OK	1
#define CODEWIZ_UPDATE_CLW	2

// flags for nRequest parameter of IClassWizard::Run
#define	CODEWIZ_CLASS_NAME			0			// lpszWholeName = class name
#define	CODEWIZ_DIALOG_RESOURCE		1			// lpszWholeName = resource
#define	CODEWIZ_MENU_RESOURCE		2			// lpszWholeName = resource
#define CODEWIZ_ACCELERATOR_RESOURCE 3			// lpszWholeName = resource
#define CODEWIZ_LAST_CLASS			4			// lpszWholeName = NULL
#define CODEWIZ_FLUSH_PROJECT		5			// lpszWholeName = NULL
#define CODEWIZ_TOOLBAR_RESOURCE	6			// lpszWholeName = resource

// Flags for determining OLE info that ClassWizard knows about
// these need to stay in same order and have the same meanings as the NAMETYPE enum in oleutil.h
enum oleNameType { ontCName, ontOleName, ontPrettyName, ontOdlName };	

// Non-standard COM
class CClsWizVerb : public CObject
{
public:
	CClsWizVerb() {	m_strVerb.Empty(); m_strData.Empty(); m_nVerbFlags = 0; }
	virtual LPCSTR GetDisplayString() { return m_strVerb; }
	virtual UINT GetFlags() { return m_nVerbFlags; }
	virtual void SetFlags(UINT nFlags) { m_nVerbFlags = nFlags; }
	virtual UINT GetEvents() { return m_indexEvents; }
	virtual void SetEvents(UINT nEvents) { m_indexEvents = nEvents; }
	virtual CString &GetVerb() { return m_strVerb; }
	virtual void SetVerb(CString strVerb) { m_strVerb = strVerb; }
	virtual CString &GetCast() { return m_strCast; }
	virtual void SetCast(CString strCast) { m_strCast = strCast; }
	virtual CString &GetData() { return m_strData; }
	virtual void SetData(CString strData) { m_strData = strData; }
	virtual void GetMFCName(CString& strBuf)
	{
		int index = m_strData.Find('|');
		if (index != -1)
			strBuf = m_strData.Left(index);
		else
			strBuf.Empty();
	}
	virtual void GetHelpText(CString& strBuf)
	{
		int index = m_strData.Find('|');
		if (index != -1)
			strBuf = m_strData.Right(m_strData.GetLength()-index-1);
		else
			strBuf.Empty();
	}

#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const
	{
		CObject::Dump(dc);
		dc << " named " << m_strVerb;
		dc << " data: " << m_strData;
		dc << "\n";
	}
#endif //_DEBUG

private:
	CString m_strVerb;
	CString m_strData;
	UINT m_nVerbFlags;
	UINT m_indexEvents;
	CString m_strCast;
};

// Non-standard COM.
class CClsWizNoun : public CObject
{
public:
	CClsWizNoun(LPCSTR lpszName) { m_strNoun = lpszName; m_ptinfoEvents = NULL; }
	virtual ~CClsWizNoun() { if (m_ptinfoEvents != NULL) m_ptinfoEvents->Release(); }

	// Temporary in nature - Don't hold this pointer and don't write to it.
	virtual LPCSTR GetDisplayString() { return m_strNoun; }
	virtual void SetNoun(CString strNoun) { m_strNoun = strNoun; }
	virtual UINT GetFlags() { return m_nNounFlags; }
	virtual void SetFlags(UINT nFlags) { m_nNounFlags = nFlags; }
	virtual ITypeInfo *GetEvents() { return m_ptinfoEvents; }
	virtual void SetEvents(ITypeInfo *ptInfo) { m_ptinfoEvents = ptInfo; }
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const
	{
		CObject::Dump(dc);
		dc << " named " << m_strNoun;
		dc << "\n";
	}
#endif //_DEBUG

private:
	CString m_strNoun;
	UINT m_nNounFlags;
	ITypeInfo *m_ptinfoEvents;
};

// There are no exports for the CString object embedded below.
// so turn off Warning 4251 "Don't Ask and don't tell."
#pragma warning(disable: 4251 4275)

typedef CTypedPtrList<CObList, CClsWizVerb*> CClsWizVerbListBase;

class WIZ_EXPORT CClsWizVerbList : public CClsWizVerbListBase
{
// Implementation use only
public:
	virtual BOOL AddVirtualFunctionsToList(const CString& strFilter);
	virtual BOOL AddToListFromResource(LPCSTR lpszResourceName, LPCSTR lpszNoun, char chFilter = '\0',
							   WORD wKey = CWV_NIL, BOOL fFormDlgFilter = FALSE);
	virtual BOOL AddToListFromMemory(LPCSTR lpch, LPCSTR lpszFilter, WORD wKey = 0, BOOL fFormDlgFilter = FALSE);
	virtual BOOL AddToListFromTypeinfo(ITypeInfo *ptinfo);
};

typedef CTypedPtrList<CObList, CClsWizNoun*> CClsWizNounList;


// Non-standard COM.

typedef struct _CWNounEnum
{
	CClsWizNounList *pNounList;
	POSITION posNounEnum;
} CWNounEnum; // Class Wizard noun enumeration state

// Non-standard COM.

typedef struct _CWVerbEnum
{
	CClsWizVerbList *pVerbList;
	POSITION posVerbEnum;
} CWVerbEnum; // Class Wizard verb enumeration state


DECLARE_INTERFACE_(IClassWizard, IUnknown)
{
	STDMETHOD(IsDBAvailable)() PURE;
	STDMETHOD(PickDataSource)(BOOL& bDAO, BOOL& bBindAll, BOOL& bAutoDetect, BOOL bEnableDAO,
									CString& strSQL, CString& strConnect, CString& strTableType,
									CStringList& columns, CStringList& defVarNames,
									CString& strParamVars, CString& strParamBindings, CString& strParamMap,
									CString& strOpen,
									CWnd* pParent = NULL,							// Non-standard COM.
									IOLEDBConsCode *pOleDBConsDlg = NULL) PURE; 	// Non-standard COM.
	STDMETHOD(GenerateControlWrapper)(const GUID& guid, IApplyContext *pAC, LPCSTR szOCXFile) PURE;
	STDMETHOD(RegisterOLEControl)(LPCSTR pszPath) PURE;

	STDMETHOD(Run)(int *i, UINT nRequest, LPCSTR lpszProjPath, HWND hWndOwner,
			LPCSTR lpszWholeName, LPCSTR lpszPartName, LPCSTR lpszAutoContext) PURE;
	STDMETHOD(Update)(UINT nRequest, LPCSTR lpszProjPath, LPCSTR lpszResourceName, LPSTR lpResClwInfo) PURE;
	STDMETHOD(Update)(UINT nRequest, LPCSTR lpszProjPath, LPCSTR lpszOldName,
			LPCSTR lpszNewName, LPSTR lpResClwInfo, BOOL bRebuilding) PURE;
	STDMETHOD(UpdateResourceRebuildStatus)(LPCSTR lpszProjPath, BOOL bForceRebuild) PURE;

	STDMETHOD(DatabaseValid)() PURE;
	STDMETHOD(UpdateCmdUIDatabaseValid)() PURE;
	STDMETHOD(ClassFromFileName)(LPCSTR lpszPathName, CClsWizClass* pClass) PURE; 	// Non-standard COM.
	STDMETHOD(GetClwFileFromFileset)(UINT hFileSet, CString& rstrClwFile) PURE; 	// Non-standard COM.
	STDMETHOD(ImportClass)(UINT hFileSet, LPCSTR lpszClassName, 
							LPCSTR lpszHFileName, LPCSTR lpszCppFileName) PURE;
	STDMETHOD(CreateClassObject)(CClsWizClass** ppClass) PURE; 	// Non-standard COM.
	STDMETHOD(DeleteClassObject)(CClsWizClass* pClass) PURE; 	// Non-standard COM.
	STDMETHOD(GetCreateClasses)(ICreateClasses** ppCreateClasses) PURE;
	STDMETHOD(IsValidClass)(LPCSTR lpszClassName, LPCSTR lpszFileName) PURE;
	STDMETHOD(IsClassWizardClass)(LPCSTR lpszClassName) PURE;
	STDMETHOD(IsValidFile)(LPCSTR lpszFile, CString* pstrClass) PURE; 	// Non-standard COM.
	STDMETHOD(GetAssocFile)(LPCSTR lpszFile, CString& strClass, CString* pstrAssocFile) PURE; 	// Non-standard COM.
	STDMETHOD(SetClass)(CClsWizClass* pClass, LPCSTR lpszFileName, LPCSTR lpszClass) PURE; 	// Non-standard COM.
	STDMETHOD(GetClassWizardClass)(LPCSTR lpszClassName, CClsWizClass** pClass) PURE;       // Non-standard COM.
	STDMETHOD(InitNounEnum)(CClsWizClass *pClass, CWNounEnum *pCWNounEnum) PURE; 	// Non-standard COM.
	STDMETHOD(GetNextNoun)(CClsWizClass *pClass, CWNounEnum *pCWNounEnum, CClsWizNoun **ppNoun) PURE; 	// Non-standard COM.
	STDMETHOD(ReleaseNounEnum)(CClsWizClass *pClass, CWNounEnum *pCWNounEnum, CClsWizNoun *pNounNoDelete) PURE; 	// Non-standard COM.
	STDMETHOD(InitVerbEnum)(CClsWizClass *pClass, CClsWizNoun *pNoun, CWVerbEnum *pCWVerbEnum) PURE;
	STDMETHOD(InitVerbEnumFromFilter)(char chFilterType, CWVerbEnum *pCWVerbEnum) PURE;
	STDMETHOD(GetNextVerb)(CClsWizClass *pClass, CClsWizNoun *pNoun, CWVerbEnum *pCWVerbEnum, CClsWizVerb **ppVerb) PURE; 	// Non-standard COM.
	STDMETHOD(ReleaseVerbEnum)(CClsWizClass *pClass, CClsWizNoun *pNoun, CWVerbEnum *pCWVerbEnum, CClsWizVerb *pVerbNoDelete) PURE; 	// Non-standard COM.
	STDMETHOD(SetNoun)(CClsWizClass *pClass, CClsWizNoun *pNoun) PURE; 	// Non-standard COM.
	STDMETHOD(GetClassDisplayString)(CClsWizClass *pClass, CString &strDisplay) PURE; 	// Non-standard COM.
	STDMETHOD(GetClassFunctionName)(CClsWizClass *pClass, CClsWizVerb *pVerb, CString &strName) PURE; 	// Non-standard COM.
	STDMETHOD(IsClassVerbHandled)(CClsWizClass *pClass, CClsWizVerb *pVerb) PURE; 	// Non-standard COM.
	STDMETHOD(AddClassVerbHandler)(CClsWizClass *pClass, CClsWizVerb *pVerb, BOOL bVerbose) PURE; 	// Non-standard COM.
	STDMETHOD(RemoveClassVerbHandler)(CClsWizClass *pClass, CClsWizVerb *pVerb) PURE; 	// Non-standard COM.
	STDMETHOD(FinishClassChanges)(CClsWizClass *pClass, BOOL bCommit = TRUE) PURE; 	// Non-standard COM.
	STDMETHOD(ClassNameFromResource)(LPCSTR lpszPathName, LPCSTR lpszResourceName, LPCSTR lpszResourceType, CString& rstrClass) PURE;  // Non-standard COM.
	STDMETHOD(GetChangeClasses)(IChangeClasses** ppChangeClasses) PURE;
	STDMETHOD(GetCreateClassesInnerDlg)(ICreateClasses2** ppCreateClasses2) PURE;
	STDMETHOD(GetClassFilterType)(CClsWizClass* pClass, char *pchFilterType) PURE; 	// Non-standard COM.
	STDMETHOD(SetClassFilterType)(CClsWizClass* pClass, char chFilterType, BOOL fWriteToCLW = TRUE) PURE; 	// Non-standard COM.
	STDMETHOD(SetDefaultConfiguration)(LPCTSTR lpszConfig) PURE;
	STDMETHOD(GetCtlDefaultEvent)(LPCTSTR lpszIDCtl, LPCTSTR lpszTypeCtl, CString &rstrEventDef) PURE;
	STDMETHOD(UpdateClassWizardInfo)() PURE;
	STDMETHOD(UpdateClassWizardInfoForRC)(LPCSTR lpszRCFile) PURE;
	STDMETHOD(GetResourceRebuildStatus)(BOOL* pbDoRebuild) PURE;
	STDMETHOD(SetResourceRebuildStatus)(BOOL bDoRebuild) PURE;
	STDMETHOD(RemoveFromClassWizard)(LPCSTR lpszClassName) PURE;
	STDMETHOD(RenameClass)(LPCSTR lpszOldClassName, LPCSTR lpszNewClassName) PURE;
	STDMETHOD(GetSourceFileName)(CClsWizClass* pClass, CString& rstrSourceFile) PURE;	// Non-standard COM.
	STDMETHOD(GetHeaderFileName)(CClsWizClass* pClass, CString& rstrHeaderFile) PURE; // Non-standard COM.

	// These two methods work together to help you access what ClassWizard knows about automation.
	// If *pwOleIndex is (WORD)-1, then ClassWizard doesn't know anything about the type in question.
	// There is a distinction between a type as used for a return type and as used as a parameter type,
	// so be sure to specify bIsReturnType accordingly.
	STDMETHOD(GetOleIndex)(LPCTSTR szName, oleNameType oleWhichName, BOOL bIsReturnType, WORD* pwOleIndex) PURE;
	STDMETHOD(GetOleType)(WORD wOleIndex, oleNameType oleWhichName, BOOL bIsReturnType, CString& rstrName) PURE;	// Non-standard COM
};

/////////////////////////////////////////////////////////////////////////
// IChangeClasses Interface
// This interface allows you to add properties and methods to existing 
// classes.
/////////////////////////////////////////////////////////////////////////
#undef  INTERFACE
#define INTERFACE IChangeClasses
DECLARE_INTERFACE_(IChangeClasses, IUnknown)
{
	STDMETHOD(AddProperty)(THIS_ LPCSTR pszClass) PURE;
	STDMETHOD(AddMethod)(THIS_ LPCSTR pszClass) PURE;
	STDMETHOD (AddEvent) (THIS_ LPCSTR pszClass) PURE;
}; 

/////////////////////////////////////////////////////////////////////////
// ICreateClasses2 Interface
// This interface allows you to manipulate ClassWizard's 'Create New Class'
// dialog as a child dialog instead of as a stand-alone one.
/////////////////////////////////////////////////////////////////////////
#undef  INTERFACE
#define INTERFACE ICreateClasses2
DECLARE_INTERFACE_(ICreateClasses2, IUnknown)
{
	// INewClass methods
	STDMETHOD(SetName)(THIS_ LPCSTR pszName) PURE;
	STDMETHOD(SetBaseClass)(THIS_ LPCSTR pszName) PURE;
	STDMETHOD(SetFiles)(THIS_ LPCSTR pszHeader, LPCSTR pszImplementation) PURE;

	STDMETHOD(SetOLEOptions)(THIS_ UINT nFlags, LPCSTR pszName) PURE;

	// Only for classes that require a dialog template.
	STDMETHOD(SetDialogTemplate)(THIS_ LPCSTR pszDialogID) PURE;

	// These methods are used to allow the user to change class information.
	STDMETHOD(SetNameFlags)(THIS_ UINT nFlags) PURE;
	STDMETHOD(SetBaseClassFlags)(THIS_ UINT nFlags) PURE;
	STDMETHOD(SetFilesFlags)(THIS_ UINT nFlags) PURE;
	STDMETHOD(SetOLEOptionsFlags)(THIS_ UINT nFlags) PURE;
	STDMETHOD(SetDialogTemplateFlags)(THIS_ UINT nFlags) PURE;

	// These methods are used to manipulate the window
	STDMETHOD(DoCreate)(THIS_ CWnd* pParentWnd) PURE;
	STDMETHOD(DoDestroyWindow)(THIS) PURE;
	STDMETHOD(OnInitDialog)(THIS) PURE;
    STDMETHOD(DoSetWindowPos)(THIS_ const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) PURE;
    STDMETHOD(DoShowWindow)(THIS_ int nCmdShow) PURE;
    STDMETHOD(DoEnableWindow)(THIS_ BOOL bEnable) PURE;
	STDMETHOD(DoSetFocus)(THIS_ BOOL bFirst) PURE;
	STDMETHOD(DoUpdateData)(THIS_ BOOL bSaveAndValidate) PURE;
	STDMETHOD(ProcessOnOK)(THIS) PURE;
	STDMETHOD(GetAutoTrackFiles)(THIS_ BOOL* pbAutoTrack) PURE;
	STDMETHOD(SetAutoTrackFiles)(THIS_ BOOL bAutoTrack) PURE;

	// This method actually creates the class.
	STDMETHOD(CreateClass)(THIS) PURE;

	// Error handling for during CreateClass.  We may need to call these
	// if we only called into ICreateClass programatically.  (UI path
	// includes checking with the user in these situations and handling
	// problems that way)
	STDMETHOD(SetGuidFlags)(THIS_ UINT nFlags) PURE;
	STDMETHOD(SetResFlags)(THIS_ UINT nFlags) PURE;

	// Information retrieval.
	STDMETHOD(GetName)(THIS_ LPSTR* ppszName) PURE;
	STDMETHOD(GetBaseClass)(THIS_ LPSTR* ppszName) PURE;
	STDMETHOD(GetFiles)(THIS_ LPSTR* ppszHeader, LPSTR* ppszImplementation) PURE;
	STDMETHOD(GetOLEOptions)(THIS_ UINT* pnFlags, LPSTR* ppszName) PURE;
	STDMETHOD(GetDialogTemplate)(THIS_ LPSTR* ppszDialogID) PURE;

	STDMETHOD(GetFormStrings)(THIS_ UINT nTypeForm, LPFORMSTRINGS *ppFormStrings) PURE;
}; 


#endif	// __CLWZAPI_H__
