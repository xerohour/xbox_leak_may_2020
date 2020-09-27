#ifndef _WIZAPI_
#define _WIZAPI_

interface IBuildProject;		// Defined in pkgs\include\auto\bldauto.h, used in
						// IBuildWizard::GetAutoProject.

// Wizard interface declarations for language-independent pieces

// these are all real basic interfaces

interface IWizardNode;		// base for all Wizard nodes
interface IWizardTransaction;	// used for tracking transactions to allow commit & abort
interface IWizardItem;		// base for all Wizard instances
interface IWizardListNode;	// services for Wizard lists
interface IWizardStrings;	// handles lists of strings returned from various queries
interface IWizardIntegers;	// handles lists of integers returned from various queries
interface IWizardEnum;		// handles lists of COM objects returned from various queries
interface IWizardParser;	// all language specific things deal with this

// these interfaces help during the execution of a transaction
interface IWizManageValues;	// used to manage template macros
interface IWizAddFiles;		// used to add files to the project
interface IWizAddResources;	// used to add resources to the project
interface IWizExpandTemplates;	// used to expand templates into the project

// these interfaces are used to communicate between the wizard model and external
// wizards that use this model
interface IVCSimpleWizard;	// external wizards need to expose this interface
interface IVCWizardContext;	// this interface gives external wizards the context from which they're run

// these interfaces are used to handle AppObjects

#ifdef NOT_YET
interface IAppObjectNode;	// use for determining AppObjects in project
interface IAppObject;		// basis for all AppObjects
interface IAppObjectPart;	// all AppObjects consist of a series of these
#endif // NOT_YET

// Language specific interfaces

interface ILanguageNode;
interface ILanguageNodes;
interface ILanguageItem;
interface ILanguageModule;
interface ILanguageClassNode;
interface ILanguageClass;
interface ILanguageFunction;
interface ILanguageVariable;
interface ILanguageEnum;
interface ILanguageTypedef;
interface ILanguageMap;
interface ILanguageMapItem;
interface ILanguageMacro;
interface ILanguageItems;

// use to create a new class

#ifdef NOT_YET
interface ICreateClasses;
#endif // NOT_YET

// interface pointer types (some are left out on purpose)

typedef IWizardNode* LPWIZARDNODE;
typedef IWizardTransaction* LPWIZARDTRANSACTION;
typedef IWizardEnum* LPWIZARDENUM;
typedef IEnumVARIANT* LPENUMVARIANT;
typedef IWizardStrings* LPWIZARDSTRINGS;
typedef LPWIZARDSTRINGS LPFILES;
typedef LPWIZARDSTRINGS LPCLASSNAMES;
typedef IWizardIntegers* LPWIZARDINTEGERS;
typedef ILanguageNode* LPLANGUAGENODE;
typedef ILanguageNodes* LPLANGUAGENODES;
typedef ILanguageItem* LPLANGUAGEITEM;
typedef ILanguageModule* LPLANGUAGEMODULE;
typedef ILanguageClassNode* LPLANGUAGECLASSNODE;
typedef ILanguageClass* LPLANGUAGECLASS;
typedef ILanguageFunction* LPLANGUAGEFUNCTION;
typedef ILanguageVariable* LPLANGUAGEVARIABLE;
typedef ILanguageEnum* LPLANGUAGEENUM;
typedef ILanguageTypedef* LPLANGUAGETYPEDEF;
typedef ILanguageMap* LPLANGUAGEMAP;
typedef ILanguageMapItem* LPLANGUAGEMAPITEM;
typedef ILanguageMacro* LPLANGUAGEMACRO;
typedef ILanguageItems* LPLANGUAGEITEMS;
typedef ILanguageItems ILanguageClasses;
typedef LPLANGUAGEITEMS LPLANGUAGECLASSES;
typedef ILanguageItems ILanguageFunctions;
typedef LPLANGUAGEITEMS LPLANGUAGEFUNCTIONS;
typedef ILanguageItems ILanguageVariables;
typedef LPLANGUAGEITEMS LPLANGUAGEVARIABLES;
typedef LPLANGUAGEITEMS LPLANGUAGEENUMS;
typedef LPLANGUAGEITEMS LPLANGUAGETYPEDEFS;
typedef LPLANGUAGEITEMS LPLANGUAGEMAPS;
typedef ILanguageItems ILanguageMapItems;
typedef LPLANGUAGEITEMS LPLANGUAGEMAPITEMS;
typedef LPLANGUAGEITEMS LPLANGUAGEMACROS;
#ifdef NOT_YET
typedef IAppObjectNode* LPAPPOBJECTNODE;
typedef IAppObject* LPAPPOBJECT;
typedef IAppObjectPart* LPAPPOBJECTPART;
typedef ICreateClasses* LPCREATECLASSES;
#endif // NOT_YET

// name types
#define dxWizFullName			0x0001		// w/ parameters if a function
#define dxWizScoped				0x0002		// name with scoping info
#define dxWizExtendProp			0x0004		// name extends propget or proput

// used for backward compatibility
#define dxWizScopedFullName		(dxWizScoped|dxWizFullName)
#define dxWizScopedShortName	(dxWizScoped)
#define dxWizNoScopeFullName	(dxWizFullName)
#define dxWizNoScopeShortName	0x0000

// access flags
// NOTE: we are expecting the first four elements to be 0 - 4 as these values are being used
// for array indices.  DO NOT ADD THINGS IN THE MIDDLE WITHOUT CHECKING WHAT YOU HAVE AFFECTED.
#define dxWizAccessDontCare         (0)
#define dxWizAccessPublic           (1)
#define dxWizAccessProtected        (2)
#define dxWizAccessPrivate          (3)
#define dxWizAccessJavaProtected    (4)         // protected means something different in Java, we need a different glyph
#define dxWizAccessDefault          (5)

// file types and 'include' variations
#define dxWizDefinition			0x0001
#define dxWizDeclaration		0x0002
#define dxWizMemberDefinitions	0x0004	// file the members of a class are defined in (the .cpp for a class)
#define dxWizMemberDeclarations	0x0008	// file the members of a class are declared in (the .h for a class)
#define dxWizProjectInclude		0x0010	// typically, stdafx.h
#define dxWizGlobalItem			0x0020	// use this to get the #include file for a global func or var 
#define dxWizFileInclude		0x0040	// use this to put an #include in a particular file
#define dxWizInclude			0x0080	// it's an include
#define dxWizImport				0x0100	// it's an import
#define dxWizImportlib			0x0200	// it's an importlib
#define dxWizAllFileTypes		0x0FFF

// these flags are for determining scope for IDL/ODL import/importlib inclusion
// we need to be able to OR them with dxWizImport and dxWizImportlib
#define dxWizFileScope			0x1000	// it's at file scope (matters only for IDL/ODL)
#define dxWizLibraryScope		0x2000	// it's at library scope (matters only for IDL/ODL)
#define dxWizObjectScope		0x4000	// it's at the scope of whatever object we resolved the filename for

// file existence types
#define dxWizFileExist			0x00000001	// the file already exists
#define dxWizFileInProject		0x00000002	// the file is already in the project
#define dxWizFileInsertIface	0x00010000	// insert an interface appropriately into IDL/ODL
#define dxWizFileInsertCoclass	0x00020000	// insert a coclass appropriately into IDL/ODL

// parsing status
#define dxWizNoParseInfo		1	// no parsing info available
#define dxWizParsing			2	// in the middle of parsing
#define dxWizParseDone			3	// finished parsing

// location & hint types
#define dxWizHintNone			0x0000	// no hint
#define dxWizFilePathDontCare	0x0000	// yes, we want this to be default...
#define dxWizStart				0x0001	// start of whatever it is
#define dxWizEnd				0x0002	// end of whatever it is
#define dxWizHintCOM			0x0004	// it's a COM thingie (like STDMETHOD)
#define dxWizHintLineNumber		0x0008	// hint is a line number
#define dxWizFilePathAbsolute	0x0100	// absolute path
#define dxWizFilePathRelative	0x0200	// relative to project directory
#define dxWizFilePathSimple		0x0400	// just the file name, no directories
#define dxWizName				0x0800	// want the name of whatever it is
#define dxWizHintGlobalScope	0x1000	// want the thing at global scope
#define dxWizHintCodeStart		0x2000	// want start of file, below any #includes, etc.
#define dxWizDoIndents			0x4000	// want the code we're inserting to be indented like auto-indent would do
#define dxWizClassWiz			0x8000	// want whatever it is to be compatible with ClassWizard comments

// ActiveX association types
#define dxWizNothing			0x00000	// current item has no association
	// need to be able to OR these types with dxWizFunction and dxWizVariable
#define dxWizProperty			0x00001	// current item, variable or function, is actually a property
#define dxWizPropertyNotify		0x00002	// current item is a property change notification function
#define dxWizMethod				0x00004	// current item is a function

// generic association major types
#define dxWizActiveXMatch		0x00010	// ActiveX assoc present
#define dxWizActiveXMissing		0x00020	// would be present for ActiveX assoc if wasn't missing
#define dxWizActiveXNone		0x00040	// not related to ActiveX assoc

// some basic types
#define dxWizNormal				0x00000	// can be anything
#define dxWizVariable			0x00100	// variable
#define dxWizFunction			0x00200	// function
#define dxWizCoclass			0x00400	// coclass
#define dxWizInterface			0x00800	// interface
#define dxWizClass				0x01000	// class
#define	dxWizTemplate			0x02000	// it is ok for this thing to be a template
#define dxWizDispinterface		0x04000	// dispinterface
#define dxWizLibrary			0x08000	// library
#define dxWizEnum				0x00001	// enum	-- never OR'ed with other type flags
#define dxWizTypedef			0x00002	// typedef -- never OR'ed with other type flags
#define dxWizMap				0x00003	// map -- never OR'ed with other type flags
#define dxWizMacro				0x00004	// macro -- never OR'ed with other type flags

// these are expected to be just barely smaller than the allowable range
#define dxWizDefEndLine			-1					// last line of the definition
#define dxWizModEndLine			dxWizDefEndLine-1	// last line of the module
#define dxWizEndOfLine			-1					// last column of the line
#define dxWizDefBeginLine		dxWizModEndLine-1	// first line of the definition (for func, mostly)

// file access types
#define dxWizModeRead			0	// read access
#define dxWizModeWrite			1	// read/write access

// symbol types
#define dxWizIntrinsic			1	// this is an intrinsic word in the language ('class', 'int', etc.)
#define dxWizMFCName			2	// belongs to MFC (class name, etc.)

// Open flags for working with the rc file
#define dxWizOpenAlwaysRC   (0)
#define dxWizCreateNewRC    (1)
#define dxWizOpenExistingRC (2)

/////////////////////////////////////////////////////////////////////////////
// IWizardNode
//   This is the primary interface on a base Wizard node.

#undef  INTERFACE
#define INTERFACE IWizardNode
DECLARE_INTERFACE_(IWizardNode, IDispatch)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IWizardNode methods
	STDMETHOD(GetVersion)(THIS_ long FAR* pnMajor, long FAR* pnMinor) PURE;
	STDMETHOD(CheckVersion)(THIS_ long nMajor, long nMinor) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(FindInterfacesOfType)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterfaceCollection) PURE;
	STDMETHOD(StartTransaction)(THIS_ LPWIZARDTRANSACTION FAR* ppvTransaction) PURE;
	STDMETHOD(WaitOnParser)(THIS) PURE;
	STDMETHOD(put_DefaultConfiguration)(THIS_ LPCTSTR lpszConfig) PURE;
	STDMETHOD(get_ProjectName)(THIS_ BSTR FAR* pbstrProjName) PURE;
	STDMETHOD(IsObjectInProject)(THIS_ LPCTSTR lpszObjName, BOOL FAR* pbInProj) PURE;
	STDMETHOD(MakeRelative)(THIS_ LPCTSTR lpszOrigPath, LPCTSTR lpszRelativeWhat, BSTR FAR* pbstrRelativePath) PURE;
	STDMETHOD(GetSelectedProject)(THIS_ HTARGET FAR* phTarget) PURE;
	STDMETHOD(GetBuildProject)(THIS_ HTARGET hTarget, IBuildProject FAR* FAR* pBuildProject) PURE;
	STDMETHOD(GetProjectType)(THIS_ HTARGET hTarget, int FAR* pnProjType) PURE;
	STDMETHOD(GetProjectDirectory)(THIS_ HTARGET hTarget, BSTR FAR* pbstrProjDir) PURE;
	STDMETHOD(GetCodeManagerLocation)(THIS_ BSTR FAR* pbstrCodeMgrLoc) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IVCSimpleWizard
//   This interface is exposed by external wizards that use the wizard model.

#undef  INTERFACE
#define INTERFACE IVCSimpleWizard
DECLARE_INTERFACE_(IVCSimpleWizard, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IVCSimpleWizard methods.
	STDMETHOD(CanRunWizard)(THIS_ IWizardNode* lpWizardNode, IVCWizardContext* lpWizardContext, UINT nMsgID,
		IWizardTransaction* pTransaction, BOOL FAR* pbCanRun) PURE;	// called wizard MUST release both lpWizardNode and lpWizardContext
	STDMETHOD(RunWizard)(THIS_ IWizardNode* lpWizardNode, IVCWizardContext* lpWizardContext, UINT nMsgID,
		IWizardTransaction* pTransaction) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IVCWizardContext
//   This interface provides the means for external wizards that use the wizard model to determine
//	 what their context is.

#undef  INTERFACE
#define INTERFACE IVCWizardContext
DECLARE_INTERFACE_(IVCWizardContext, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IVCWizardContext methods.
	// assumes context is for a single object
	STDMETHOD(GetContextType)(THIS_ long FAR* pnContextType) PURE;
	STDMETHOD(GetContextObject)(THIS_ LPDISPATCH FAR* pvContextObject) PURE;
	STDMETHOD(GetContextProjectName)(THIS_ BSTR FAR* pbstrProjectName) PURE;
	STDMETHOD(GetContextProjectCookie)(THIS_ HTARGET FAR* phTarget) PURE;
	STDMETHOD(GetContextLanguage)(THIS_ BSTR FAR* pbstrLanguage) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IWizardStrings
//   This interface provides access to generic lists of Wizard string data.

#undef  INTERFACE
#define INTERFACE IWizardStrings
DECLARE_INTERFACE_(IWizardStrings, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
 	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IWizardStrings methods
	STDMETHOD(GetListType)(THIS_ long FAR* pnListType) PURE;
	STDMETHOD(Find)(THIS_ LPCTSTR lpszElement, BOOL bPartialMatch, BSTR FAR* pbstrActualElement) PURE;
	STDMETHOD(_NewEnum)(IEnumVARIANT **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IWizardIntegers
//   This interface provides access to generic lists of Wizard string data.

#undef  INTERFACE
#define INTERFACE IWizardIntegers
DECLARE_INTERFACE_(IWizardIntegers, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IWizardIntegers methods
	STDMETHOD(GetListType)(THIS_ long FAR* pnListType) PURE;
	STDMETHOD(Next)(THIS_ long FAR* pnElement) PURE;
	STDMETHOD(Skip)(THIS_ long nSkipNumber) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Find)(THIS_ long nElement, BOOL bPartialMatch, long FAR* pnActualElement) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IWizardEnum
//   This interface provides access to generic lists of Wizard objects.

#undef  INTERFACE
#define INTERFACE IWizardEnum
DECLARE_INTERFACE_(IWizardEnum, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IWizardEnum methods
	STDMETHOD(Next)(THIS_ LPVOID FAR* ppvObj) PURE;
	STDMETHOD(Skip)(THIS_ long nSkipNumber) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Find)(THIS_ LPCTSTR lpszElementName, BOOL bPartialMatch, LPVOID FAR* ppvObj) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IWizardTransaction
//   This interface handles transaction management for other Wizard APIs (i.e., bail out capability).
//	 'begin transaction' is implicit in creating one of these

#undef  INTERFACE
#define INTERFACE IWizardTransaction
DECLARE_INTERFACE_(IWizardTransaction, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IWizardTransaction methods
	STDMETHOD(CommitTransaction)(THIS_ BOOL bRemember) PURE;
	STDMETHOD(AbortTransaction)(THIS) PURE;
	STDMETHOD(CreateTemplate)(THIS_ LPSTREAM lpStm, IWizExpandTemplates FAR** ppvObj) PURE;	// similar to Gallery's CreateTemplateFromText
#ifdef NOT_YET
	// very probably V6 work:
	STDMETHOD(SetCheckPoint)(THIS) PURE;
	STDMETHOD(get_CheckPointNumber)(THIS_ long FAR* nCheckPointNumber);
	STDMETHOD(Rollback)(THIS_ long nCheckPointNumber);
#endif	// NOT_YET
};

/////////////////////////////////////////////////////////////////////////////
// IWizManageValues
//   This interface allows interaction with the value table.

#undef  INTERFACE
#define INTERFACE IWizManageValues
DECLARE_INTERFACE_(IWizManageValues, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Value interaction
	STDMETHOD(AddValue)(THIS_ LPCSTR pszKey, LPCSTR pszValue) PURE;
	STDMETHOD(GetValue)(THIS_ LPCSTR pszKey, BSTR FAR* pbstrValue) PURE;
	STDMETHOD(SetValue)(THIS_ LPCSTR pszKey, LPCSTR pszValue) PURE;
	STDMETHOD(RemoveValue)(THIS_ LPCSTR pszKey) PURE;
	STDMETHOD(ClearValues)(THIS) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IWizAddFiles
//   This interface on a transaction object is used to interact with the
//   project.  i.e. add/enumerate files, etc.

#undef  INTERFACE
#define INTERFACE IWizAddFiles
DECLARE_INTERFACE_(IWizAddFiles, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Filename methods.
	STDMETHOD(CalculateFilename)(THIS_ LPCSTR pszFilename, BSTR FAR* pbstrNewFilename) PURE;
	STDMETHOD(SaveFilename)(THIS_ LPCSTR pszDesiredFilename, LPCSTR pszActualFilename) PURE;
	STDMETHOD(AddFile)(THIS_ LPCSTR pszFilename) PURE;
	STDMETHOD(CalculateFilenameEx)(THIS_ LPCSTR pszFilename, BSTR FAR* pbstrNewFilename, 
		long* pnHint) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IWizAddResources
//   This interface on a transaction object is used to interact with
//   the project's resource script.

#undef  INTERFACE
#define INTERFACE IWizAddResources
DECLARE_INTERFACE_(IWizAddResources, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Resource methods.
	STDMETHOD(Create)(THIS_ UINT nCreateFlags) PURE;
	STDMETHOD(GetResourceInclude)(THIS_ BSTR FAR* pbstrFilename) PURE;

	STDMETHOD(AttachResourceStream)(THIS_ LPSTREAM lpStream) PURE;
	STDMETHOD(CreateSymbol)(THIS_ LPCSTR pszID, BSTR FAR* pbstrActualID, int FAR* pnIDValue) PURE;
	STDMETHOD(CreateResource)(THIS_ LPCSTR pszType, LPCSTR pszID, int nLangID, LPCSTR szCondition, 
		LPCSTR pszNewID) PURE;
	STDMETHOD(ImportResource)(THIS_ LPCSTR pszType, LPCSTR pszID, LPCSTR pszFileName, int nLangID, 
		LPCSTR szCondition) PURE;
	STDMETHOD(SetResourceID)(THIS_ LPCSTR pszType, LPCSTR pszOldID, int nLangID, LPCSTR szCondition, 
		LPCSTR pszNewID) PURE;
	STDMETHOD(CreateString)(THIS_ LPCSTR pszID, int nLangID, LPCSTR szCondition, LPCSTR pszString) PURE;
	STDMETHOD(Access)(THIS_ UINT nMode) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IWizExpandTemplates
//   This interface is used to interact with templates.

#undef  INTERFACE
#define INTERFACE IWizExpandTemplates
DECLARE_INTERFACE_(IWizExpandTemplates, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	STDMETHOD(Render)(THIS_ LPSTREAM lpStm, IWizardTransaction FAR* lpWT) PURE;
	STDMETHOD(RenderIntoFile)(THIS_ LPCTSTR szFileName, long nHint, IWizardTransaction FAR * lpWT) PURE;
};


#ifdef NOT_YET
/////////////////////////////////////////////////////////////////////////////
// IAppObjectNode
//   This interface exposes the application objects present in the current workspace.

#undef  INTERFACE
#define INTERFACE IAppObjectNode
DECLARE_INTERFACE_(IAppObjectNode, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IAppObjectNode methods
	STDMETHOD(GetOneAppObject)(THIS_ LPCTSTR lpszKind, LPCTSTR lpszAppObjectName, 
		LPAPPOBJECT FAR* ppvAppObject) PURE;
	STDMETHOD(GetAllAppObjectsOfType)(THIS_ LPCTSTR lpszKind, LPWIZARDENUM FAR* ppvWizardEnum) PURE;
	STDMETHOD(GetAllAppObjects)(THIS_ LPCTSTR lpszKind, LPWIZARDENUM FAR* ppvWizardEnum) PURE;
	STDMETHOD(Add)(THIS_ LPCTSTR lpszContext, LPCTSTR lpszKind, LPCTSTR lpszName) PURE;
	STDMETHOD(Delete)(THIS_ LPAPPOBJECT ppvAppObject) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IAppObject
//   This is an interface to an application object present in the current workspace.

#undef  INTERFACE
#define INTERFACE IAppObject
DECLARE_INTERFACE_(IAppObject, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IAppObject methods
	STDMETHOD(Next)(THIS_ LPVOID FAR* ppvObj) PURE;
	STDMETHOD(Skip)(THIS_ long nSkipNumber) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(FindByName)(THIS_ LPCTSTR lpszAppObjectName, LPAPPOBJECTPART FAR* ppvAppObjectPart) PURE;
	STDMETHOD(FindByType)(THIS_ LPCTSTR lpszAppObjectType, LPAPPOBJECTPART FAR* ppvAppObjectPart) PURE;
	STDMETHOD(FindByKind)(THIS_ LPCTSTR lpszAppObjectKind, LPAPPOBJECTPART FAR* ppvAppObjectPart) PURE;
	STDMETHOD(get_Kind)(THIS_ BSTR FAR* pbstrKind) PURE;
	STDMETHOD(get_Context)(THIS_ BSTR FAR* pbstrContext) PURE;
	STDMETHOD(put_Context)(THIS_ LPCTSTR lpszContext) PURE;
	STDMETHOD(Add)(THIS_ LPCTSTR lpszPartType, LPCTSTR lpszPartName, LPCTSTR lpszPartKind) PURE;
	STDMETHOD(Delete)(THIS_ LPAPPOBJECTPART ppvAppObjectPart) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IAppObjectPart
//   This is an interface to get to a piece of an application object.

#undef  INTERFACE
#define INTERFACE IAppObjectPart
DECLARE_INTERFACE_(IAppObjectPart, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// IAppObjectPart methods
	STDMETHOD(get_Type)(THIS_ BSTR FAR* pbstrAppObjectType) PURE;
	STDMETHOD(put_Type)(THIS_ LPCTSTR lpszAppObjectType) PURE;
	STDMETHOD(get_Name)(THIS_ BSTR FAR* pbstrAppObjectName) PURE;
	STDMETHOD(put_Name)(THIS_ LPCTSTR lpszAppObjectName) PURE;
	STDMETHOD(get_Kind)(THIS_ BSTR FAR* pbstrAppObjectKind) PURE;
	STDMETHOD(put_Kind)(THIS_ LPCTSTR lpszAppObjectKind) PURE;
};
#endif	// NOT_YET

/////////////////////////////////////////////////////////////////////////////
// ILanguageNodes
//   This is an interface to get to the set of ILanguageNode interfaces in the workspace

#undef  INTERFACE
#define INTERFACE ILanguageNodes
DECLARE_INTERFACE_(ILanguageNodes, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageNodes methods
	STDMETHOD(get_ParseStatus)(THIS_ short FAR* pnParseStatus) PURE;
	STDMETHOD(get_ParseFilesLeft)(THIS_ int FAR* pnParseFiles) PURE;
	STDMETHOD(put_DefaultConfiguration)(THIS_ LPCTSTR lpszConfig) PURE;
 	STDMETHOD(GetLanguageNode)(THIS_ LPCTSTR lpszName, LPLANGUAGENODE FAR* ppvLangNode) PURE;
 	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(_NewEnum)(IEnumVARIANT **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ILanguageNode
//   This is an interface to get to language specific stuff.  Current focus: C++

#undef  INTERFACE
#define INTERFACE ILanguageNode
DECLARE_INTERFACE_(ILanguageNode, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageNode methods
	STDMETHOD(get_LanguageName)(THIS_ BSTR FAR* bstrLanguageName) PURE;
	STDMETHOD(GetVersion)(THIS_ long FAR* pnMajor, long FAR* pnMinor) PURE;
	STDMETHOD(CheckVersion)(THIS_ long nMajor, long nMinor) PURE;
	STDMETHOD(EnumAllWizardTypes)(THIS_ long nScopeType, LPWIZARDINTEGERS FAR* ppvWizardIntegers) PURE;
	STDMETHOD(GetAllNamesOfType)(THIS_ long nScopeType, LPCTSTR lpszScopeName, long nType, 
		LPWIZARDSTRINGS FAR* ppvWizardStrings) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(get_ParseStatus)(THIS_ short FAR* pnParseStatus) PURE;
	STDMETHOD(get_ParseFilesLeft)(THIS_ int FAR* pnParseFiles) PURE;
	STDMETHOD(GetSelectedProject)(THIS_ HTARGET FAR* phTarget) PURE;
	STDMETHOD(ParseName)(THIS_ long nParseType, LPCTSTR lpszParseString, BSTR FAR* pbstrResponse) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ILanguageItem
//   This is a generic base interface to get to language specific items.  Current focus: C++

#undef  INTERFACE
#define INTERFACE ILanguageItem
DECLARE_INTERFACE_(ILanguageItem, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageItem methods
	STDMETHOD(EnumAllWizardTypes)(THIS_ long nScopeType, LPCTSTR lpszScopeName, 
		LPWIZARDINTEGERS FAR* ppvWizardIntegers) PURE;
	STDMETHOD(GetAllNamesOfType)(THIS_ long nScopeType, LPCTSTR lpszScopeName, 
		long nWizardType, LPWIZARDSTRINGS FAR* ppvWizardStrings) PURE;
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* bstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_AllAttributes)(THIS_ BSTR FAR* bstrName) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(GetAllObjectsOfType)(THIS_ long nWizardMajorType, long nWizardMinorType, LPCTSTR lpszExtra,
		LPDISPATCH FAR* ppvWizardEnum) PURE;
	STDMETHOD(GetObjectOfType)(THIS_ long nWizardMajorType, long nWizardMinorType, LPCTSTR lpszExtra,
		LPCTSTR lpszObjectName, LPDISPATCH FAR* ppvLangItem) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(get_AccessType)(THIS_ short FAR* pnAccessType) PURE;
	STDMETHOD(put_AccessType)(THIS_ short nAccessType) PURE;
	STDMETHOD(get_StartLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(GetOneLine)(THIS_ long nFileType, long nLineNumber, BSTR FAR* pbstrLineContents) PURE;
	STDMETHOD(SetOneLine)(THIS_ long nFileType, long nLineNumber , LPCTSTR lpszText) PURE;
	STDMETHOD(InsertText)(THIS_ long nFileType, LPCTSTR lpszText, long nLineNumber, long nColumn, 
		BOOL bFullLine) PURE;
#ifdef NOT_YET
	STDMETHOD(GetConditions)(THIS_ short nWhichFile, BSTR FAR* pbstrConditions) PURE;
	STDMETHOD(GetContents)(THIS_ BSTR FAR* pbstrContents) PURE;
	STDMETHOD(GetTemplate)(THIS_ long nWizardType, LPCTSTR lpszName, BSTR FAR* pbstrTemplate, 
		BSTR FAR* pbstrTemplateVariables) PURE;
	STDMETHOD(RenderFunctionTemplate)(THIS_ LPCTSTR lpszFunctionName, LPCTSTR lpszTemplateVariables, 
		long nDeclHintType, LPCTSTR lpszHintName, BOOL bDeclAddAfter, long nImplHintType, 
		LPCTSTR lpszImplHintName, BOOL bImplAddAfter) PURE;
	STDMETHOD(RenderMyTemplate)(THIS_ LPCTSTR lpszTemplate, LPCTSTR lpszTemplateVariables, 
		BSTR FAR* pbstrRenderedTemplate) PURE;
	STDMETHOD(FindExactText)(THIS_ long nLineNumberStart, LPCTSTR lpszText, BOOL bCheckNext, 
		BOOL bCheckComments, BSTR FAR* pbstrFoundFile, BSTR FAR* pbstrFoundLocation, 
		long FAR* pnLineNumberFound) PURE;
	STDMETHOD(FindVariable)(THIS_ LPCTSTR lpszVariableType, long nLineNumberStart, BOOL bCheckNext, 
		BSTR FAR* pbstrVariableName, BSTR FAR* pbstrFoundFile, BSTR FAR* pbstrFoundLocation, 
		long FAR* pnLineNumberFound) PURE;
	STDMETHOD(WhatIsThis)(THIS_ LPCTSTR lpszText, LPCTSTR lpszFileFound, long nLineNumber, long FAR* pnTokenType) PURE;
	STDMETHOD(DeleteOneLine)(THIS_ long nFileType, long nLineNumber) PURE;
#endif // NOT_YET
}; 

/////////////////////////////////////////////////////////////////////////////
// ILanguageModuleNode
//   This is an interface for obtaining language-specific modules. Current focus: C++

#undef  INTERFACE
#define INTERFACE ILanguageModuleNode
DECLARE_INTERFACE_(ILanguageModuleNode, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageModuleNode methods
	STDMETHOD(GetAllObjectsOfType)(THIS_ HTARGET hTarget, long nWizardMajorType, long nWizardMinorType, 
		LPCTSTR lpszExtra, LPDISPATCH FAR* ppvWizardEnum) PURE;
	STDMETHOD(GetObjectOfType)(THIS_ HTARGET hTarget, long nWizardMajorType, long nWizardMinorType, 
		LPCTSTR lpszExtra, LPCTSTR lpszObjectName, LPDISPATCH FAR* ppvLangItem) PURE;
	STDMETHOD(AddInclude)(THIS_ long nAddWhere, LPCTSTR lpszClassName, LPCTSTR lpszIncludeName, 
		LPCTSTR lpszAttribs, LPCTSTR lpszConditions, LPCTSTR lpszComments, long nHintType, 
		LPCTSTR lpszHintName, BOOL bAddAfter) PURE;
	STDMETHOD(GetSelectedProject)(THIS_ HTARGET FAR* phTarget) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
}; 

/////////////////////////////////////////////////////////////////////////////
// ILanguageModule
//   This is an interface for manipulating language-specific modules. Current focus: C++

#undef  INTERFACE
#define INTERFACE ILanguageModule
DECLARE_INTERFACE_(ILanguageModule, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
 
    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageModule methods
	STDMETHOD(EnumResidentWizardTypes)(THIS_ long nScopeType, LPCTSTR lpszScopeName, 
		LPWIZARDINTEGERS FAR* ppvWizardIntegers) PURE;
	STDMETHOD(GetResidentFunctionNames)(THIS_ long nScopeType, LPCTSTR lpszScopeName, 
		long nWizardType, LPWIZARDSTRINGS FAR* ppvWizardStrings) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(GetAllNamesOfType)(THIS_ long nScopeType, LPCTSTR lpszScopeName, long nWizardType, 
		LPWIZARDSTRINGS FAR* pvWizardStrings) PURE;
	STDMETHOD(GetAllObjectsOfType)(THIS_ long nWizardMajorType, long nWizardMinorType, LPCTSTR lpszExtra, 
		LPDISPATCH FAR* pvWizardEnum) PURE;
	STDMETHOD(GetFileAccess)(THIS_ short nMode, short nFindType, long nHintType, LPCTSTR lpszHintName, 
		BOOL FAR* pbHaveAccess) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
 	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(GetOneLine)(THIS_ long nFileType, long nLineNumber, BSTR FAR* bstrLineContents) PURE;
	STDMETHOD(SetOneLine)(THIS_ long nFileType, long nLineNumber , LPCTSTR lpszText) PURE;
	STDMETHOD(InsertText)(THIS_ long nFileType, LPCTSTR lpszText, long nLineNumber, long nColumn, 
		BOOL bFullLine) PURE;
	STDMETHOD(AddInclude)(THIS_ long nAddWhere, LPCTSTR lpszExtra, LPCTSTR lpszIncludeName, 
		LPCTSTR lpszAttribs, LPCTSTR lpszConditions, LPCTSTR lpszComments, long nHintType, 
		LPCTSTR lpszHintName, BOOL bAddAfter) PURE;
#ifdef NOT_YET
	STDMETHOD(GetIncludeFile)(THIS_ LPCTSTR lpszInclude, long nStartLine, BOOL FAR* pbFoundIt, 
		long FAR* pnFoundLine, BSTR FAR* pbstrActual, BSTR FAR* pbstrConditions, BSTR FAR* pbstrComments) PURE;
	STDMETHOD(GetContext)(THIS_ long FAR* pnLineNumber, BSTR FAR* pbstrContextInfo) PURE;
	STDMETHOD(GetLineNumber)(THIS_ long FAR* pnLineNumber) PURE;
	STDMETHOD(GetStartLineOfContext)(THIS_ LPCTSTR lpszContext, long FAR* pnStartLine) PURE;
	STDMETHOD(GetDefaultCounterpartName)(THIS_ BSTR FAR* pbstrCounterpart) PURE;
	STDMETHOD(ResolveContext)(THIS_ LPCTSTR lpszFullContext, LPCTSTR lpszContextPart, BSTR FAR* pbstrType, 
		BSTR FAR* pbstrName, BSTR FAR* pbstrRemaining) PURE;
	STDMETHOD(AddMember)(THIS_ long nMemberKind, LPCTSTR lpszMemberName, LPCTSTR lpszMemberType, 
		LPCTSTR lpszMemberBody, LPCTSTR lpszConditions, long nHintType, LPCTSTR lpszHintName, 
		BOOL bAddAfter) PURE;
	STDMETHOD(DeleteMember)(THIS_ long nMemberKind, LPCTSTR lpszMemberName) PURE;
	STDMETHOD(DeleteUnnamedMember)(THIS_ long nMemberKind, LPCTSTR lpszMemberContents) PURE;
#endif	// NOT_YET
};
 
/////////////////////////////////////////////////////////////////////////////
// ILanguageClassNode
//   This is an interface for obtaining classes.

#undef  INTERFACE
#define INTERFACE ILanguageClassNode
DECLARE_INTERFACE_(ILanguageClassNode, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageClassNode methods
	STDMETHOD(GetScopeObject)(LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GetAllObjectsOfType)(THIS_ HTARGET hTarget, long nWizardMajorType, long nWizardMinorType, 
		LPCTSTR lpszExtra, LPDISPATCH FAR* ppvWizardEnum) PURE;
	STDMETHOD(GetObjectOfType)(THIS_ HTARGET hTarget, long nWizardMajorType, long nWizardMinorType, 
		LPCTSTR lpszExtra, LPCTSTR lpszObjectName, LPDISPATCH FAR* ppvLangItem) PURE;
	STDMETHOD(GetDerivedClasses)(THIS_ LPCTSTR lpszBaseClass, BOOL bImmediateOnly, 
		LPLANGUAGECLASSES FAR* ppvClasses) PURE;
	STDMETHOD(HasMembers)(THIS_ HTARGET hTarget, BOOL FAR* bHasMembers) PURE;
	STDMETHOD(HasAssociations)(THIS_ HTARGET hTarget, long nAssociationMajorType, long nAssociationMinorType, 
		LPCTSTR lpszClass, BOOL FAR* bHasAssoc) PURE;
	STDMETHOD(GetAssociations)(THIS_ HTARGET hTarget, long nAssociationMajorType, long nAssociationMinorType,
		LPCTSTR lpszClass, LPDISPATCH FAR* pvWizardEnum) PURE;
	STDMETHOD(GetAssociationName)(THIS_ HTARGET hTarget, long nAssociationMajorType, long nAssociationMinorType, 
		long nPresentHow, long nAssociatedHow, LPCTSTR lpszAssociatedName, BSTR FAR* pbstrName) PURE;
	STDMETHOD(AddInclude)(THIS_ long nAddWhere, LPCTSTR lpszClassName, LPCTSTR lpszIncludeName, 
		LPCTSTR lpszAttribs, LPCTSTR lpszConditions, LPCTSTR lpszComments, long nHintType, 
		LPCTSTR lpszHintName, BOOL bAddAfter) PURE;
	STDMETHOD(GetInclude)(THIS_ long nFileType, LPCTSTR lpszClassName, BOOL bMakeRelative, LPCTSTR lpszRelativeWhere, BSTR FAR* pbstrIncludeFile) PURE;
	STDMETHOD(IsValidSymbol)(THIS_ long nObjectType, LPCTSTR lspzName, BOOL FAR* pbIsValidSymbol) PURE;
	STDMETHOD(IsReservedSymbol)(THIS_ long nObjectType, LPCTSTR lpszName, long FAR* pnSymbolType) PURE;
	STDMETHOD(CreateWrapper)(THIS_ long nHintType, LPCTSTR lpszHintName, LPCTSTR lpszSeed, 
		BSTR FAR* pbstrWrapper) PURE;
	STDMETHOD(GetSelectedProject)(THIS_ HTARGET FAR* phTarget) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
#ifdef NOT_YET
	STDMETHOD(GetOverrideList)(THIS_ LPCTSTR lpszClassName, LPWIZARDLIST FAR* ppvWizardList) PURE;
	STDMETHOD(GetOverrideItem)(THIS_ LPCTSTR lpszClassName, LPCTSTR lpszFunctionName, BSTR FAR* pbstrFullName) PURE;
	STDMETHOD(GetClassTemplate)(THIS_ long nTemplateType, LPCTSTR lpszClassName, BSTR FAR* pbstrTemplate, 
		BSTR FAR* pbstrTemplateVars) PURE;
	STDMETHOD(DeleteClass)(THIS_ LPCTSTR lpszType, LPCTSTR lpszClassName) PURE;
#endif	// NOT_YET
}; 

/////////////////////////////////////////////////////////////////////////////
// IAltLanguageClassNode
//   This is an interface for doing extra stuff.  It probably should be a service from IWizardNode,
//   but it is fastest to put it off the CPP package for V6, so that's where it is going.

#undef  INTERFACE
#define INTERFACE IAltLanguageClassNode
DECLARE_INTERFACE_(IAltLanguageClassNode, IUnknown)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
 
	STDMETHOD(GetSelectedProject)(THIS_ HTARGET FAR* phTarget) PURE;
	STDMETHOD(GotoDialogEditorLocation)(THIS_ HTARGET hTarget, LPCTSTR lpszClassName, LPCTSTR lpszDialogID) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ILanguageClass
//   This is an interface for manipulating classes.  Current focus: C++ classes

#undef  INTERFACE
#define INTERFACE ILanguageClass
DECLARE_INTERFACE_(ILanguageClass, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
 
    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageClass methods
	STDMETHOD(get_BaseClass)(THIS_ BSTR FAR* pbstrBaseClass) PURE;
	STDMETHOD(get_BaseClassList)(THIS_ LPCLASSNAMES FAR* ppvBaseClassNames) PURE;
	STDMETHOD(HasBaseClass)(THIS_ LPCTSTR lpszBaseClass, BOOL bAsImmediate, BOOL FAR* pbHasBaseClass) PURE;
	STDMETHOD(HasMembers)(THIS_ BOOL FAR* pbHasMembers) PURE;
	STDMETHOD(HasAssociations)(THIS_ long nAssociationMajorType, long nAssociationMinorType, BOOL FAR* pbHasAssoc) PURE;
	STDMETHOD(GetName)(THIS_ short nWizType, BSTR FAR* pbClassName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_Type)(THIS_ BSTR FAR* pbClassType) PURE;
	STDMETHOD(get_AccessType)(THIS_ short FAR* pnAccessType) PURE;
	STDMETHOD(put_AccessType)(THIS_ short nAccessType) PURE;
	STDMETHOD(get_Contents)(THIS_ BSTR FAR* pbstrClassContents) PURE;
	STDMETHOD(put_Contents)(THIS_ LPCTSTR lpszContents) PURE;
	STDMETHOD(get_AllAttributes)(THIS_ BSTR FAR* pbstrAllAttributes) PURE;
	STDMETHOD(put_AllAttributes)(THIS_ LPCTSTR lpszAllAttributes) PURE;
	STDMETHOD(get_NumberOfAttributes)(THIS_ short FAR* pnNumAttrib) PURE;
	STDMETHOD(GetAssociations)(THIS_ long nAssociationMajorType, long nAssociationMinorType, 
		LPDISPATCH FAR* pvAssociations) PURE;
	STDMETHOD(GetScope)(THIS_ long FAR* nScopeType, BSTR FAR* bstrScopeName) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Conditions)(THIS_ BSTR FAR* bstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ LPCTSTR lpszConditions) PURE;
	STDMETHOD(GetNamedAttribute)(THIS_ LPCTSTR lpszName, BSTR FAR* bstrValue, BOOL FAR* bIsDefined) PURE;
	STDMETHOD(SetNamedAttribute)(THIS_ LPCTSTR lpszName, LPCTSTR lpszValue, BOOL bIsDefined) PURE;
	STDMETHOD(GetIndexedAttribute)(THIS_ short nIndex, BSTR FAR* bstrName, BSTR FAR* bstrValue) PURE;
	STDMETHOD(EnumAllWizardTypes)(THIS_ long nScopeType, LPCTSTR lpszScopeName, 
		LPWIZARDINTEGERS FAR* pvWizardIntegers) PURE;
	STDMETHOD(GetAllNamesOfType)(THIS_ long nScopeType, LPCTSTR lpszScopeName, long nWizardType, 
		LPWIZARDSTRINGS FAR* pvWizardStrings) PURE;
	STDMETHOD(GetAllObjectsOfType)(THIS_ long nWizardMajorType, long nWizardMinorType, LPCTSTR lpszExtra, 
		LPDISPATCH FAR* pvWizardEnum) PURE;
	STDMETHOD(GetObjectOfType)(THIS_ long nWizardMajorType, long nWizardMinorType, LPCTSTR lpszExtra,
		LPCTSTR lpszObjectName, LPDISPATCH FAR* pvLangItem) PURE;
	STDMETHOD(GetFileAccess)(THIS_ short nMode, short nFindType, long nHintType, LPCTSTR lpszHintName, 
		BOOL FAR* pbHaveAccess) PURE;
	STDMETHOD(GetAssociation)(THIS_ long nAssociationMajorType, long nAssociationMinorType, LPCTSTR lpszName, 
		LPDISPATCH FAR* pvLangClass, long FAR* pnType, BSTR FAR* pbstrAttributes) PURE;
	STDMETHOD(GetAssociationName)(THIS_ long nAssociationMajorType, long nAssociationMinorType, long nPresentHow,
		BSTR FAR* pbstrName) PURE;
	STDMETHOD(GetTemplate)(THIS_ long nWizardType, LPCTSTR lpszName, BSTR FAR* pbstrTemplate, 
		BSTR FAR* pbstrTemplateVariables) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(AddMember)(THIS_ long nMemberKind, LPCTSTR lpszMemberName, LPCTSTR lpszMemberType, 
		short nAccessType, LPCTSTR lpszAttributes, LPCTSTR lpszConditions, LPCTSTR lpszBody, 
		long nHintType, LPCTSTR lpszHintName, BOOL bMemberAddAfter) PURE;
	STDMETHOD(AddMemberBody)(THIS_ long nMemberKind, LPCTSTR lpszMemberName, LPCTSTR lpszMemberType, 
		LPCTSTR lpszAttributes, LPCTSTR lpszMemberBody, LPCTSTR lpszConditions, long nHintType, 
		LPCTSTR lpszHintName, BOOL bMemberAddAfter) PURE;
	STDMETHOD(CreateFunctionStubBody)(THIS_  VARTYPE vt, BSTR FAR* pbstrReturn) PURE;
	STDMETHOD(AddBaseClass)(THIS_ LPCTSTR pszBaseClass, short nAccessType, long nHintType, LPCTSTR lpszHintName) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
 	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(GetClassStyle) (THIS_ BSTR FAR* pbstrStyle) PURE;
	STDMETHOD(GetMinorClassStyle)(THIS_ LPCTSTR lpszMajorStyle, BSTR FAR * pbstrStyle) PURE;
	STDMETHOD(GetSpecialIncludes)(THIS_ LPCTSTR lpszParams, BSTR FAR * pbstrIncludes) PURE;
	STDMETHOD(GetOneLine)(THIS_ long nFileType, long nLineNumber, BSTR FAR* bstrLineContents) PURE;
	STDMETHOD(SetOneLine)(THIS_ long nFileType, long nLineNumber , LPCTSTR lpszText) PURE;
	STDMETHOD(InsertText)(THIS_ long nFileType, LPCTSTR lpszText, long nLineNumber, long nColumn, 
		BOOL bFullLine) PURE;
	STDMETHOD(Refresh)(THIS) PURE;
	STDMETHOD(Delete)(THIS_ short nPieceToDelete, short nCommentOrDeletePiece) PURE;
#ifdef NOT_YET
	STDMETHOD(AddActiveXAssociation)(THIS_  LPCTSTR lpszInterfaceName, long nType, LPCTSTR lpszAttributes) PURE;
	STDMETHOD(get_RelatedAppObjects)(THIS_ LPWIZARDENUM FAR* ppvWizardEnum) PURE;
	STDMETHOD(get_OverrideList)(THIS_ LPWIZARDLIST FAR* ppvWizardList) PURE;
	STDMETHOD(GetOverrideItem)(THIS_ LPCTSTR lpszFunctionName, BSTR FAR* pbstrFullName) PURE;
	STDMETHOD(DeleteClass)(THIS_ LPCTSTR lpszClassName, LPCTSTR lpszAttribs) PURE;
	STDMETHOD(DeleteMember)(THIS_ long nMemberKind, LPCTSTR lpszMemberName, LPCTSTR lpszAttribs) PURE;
	STDMETHOD(DeleteActiveXAssociation)(THIS_ LPCTSTR lpszInterfaceName, LPCTSTR lpszAttribs) PURE;
	STDMETHOD(RenderFunctionTemplate)(THIS_ LPCTSTR lpszFunctionName, LPCTSTR lpszTemplateVariables, 
		long nDeclHintType, LPCTSTR lpszDeclHintName, BOOL bDeclAddAfter, long nImplHintType, 
		LPCTSTR lpszImplHintName, BOOL bImplAddAfter) PURE;
	STDMETHOD(RenderMyTemplate)(THIS_ LPCTSTR lpszTemplate, LPCTSTR lpszTemplateVariables, 
		BSTR FAR* bstrRenderedTemplate) PURE;
	STDMETHOD(FindExactText)(THIS_ long nLineNumberStart, LPCTSTR lpszText, BOOL bCheckNext, BOOL bCheckComments, 
		BSTR FAR* pbstrFoundFile, BSTR FAR* pbstrFoundLocation, long FAR* pnLineNumberFound) PURE;
	STDMETHOD(FindVariable)(THIS_ LPCTSTR lpszVariableType, long nLineNumberStart, BOOL bCheckNext, 
		BSTR FAR* pbstrVariableName, BSTR FAR* pbstrFoundFile, BSTR FAR* pbstrFoundLocation, 
		long FAR* nLineNumberFound) PURE;
	STDMETHOD(WhatIsThis)(THIS_ LPCTSTR lpszText, LPCTSTR lpszFileFound, long nLineNumber, long FAR* nTokenType) PURE;
	STDMETHOD(DeleteOneLine)(THIS_ long nFileType, long nLineNumber) PURE;
#endif	// NOT_YET
}; 

/////////////////////////////////////////////////////////////////////////////
// ILanguageItems
//   Collections of lang items ... classes, functions etc.

#undef  INTERFACE
#define INTERFACE ILanguageItems
DECLARE_INTERFACE_(ILanguageItems, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageItems methods
	STDMETHOD(_NewEnum)(THIS_ IEnumVARIANT **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ICollectionEvents
// Event source interface for the global classes collection.

#undef  INTERFACE
#define INTERFACE ICollectionEvents
DECLARE_INTERFACE_(ICollectionEvents, IDispatch)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ICollectionEvents methods
	STDMETHOD(AfterAdd)(THIS_ LPDISPATCH pItem) PURE;
	STDMETHOD(BeforeDelete)(THIS_ LPDISPATCH pItem) PURE;
	STDMETHOD(AfterChange)(THIS_ LPDISPATCH pItem) PURE;
	STDMETHOD(BeginAtomic)(THIS) PURE;
	STDMETHOD(EndAtomic)(THIS) PURE;
	STDMETHOD(MemberOp)(LPDISPATCH pItem, DWORD op, DWORD oldIinst, DWORD newIinst) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ILanguageFunction
//   This is an interface for manipulating functions.

#undef  INTERFACE
#define INTERFACE ILanguageFunction
DECLARE_INTERFACE_(ILanguageFunction, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageFunction methods
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* bstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_AccessType)(THIS_ short FAR* pnAccessType) PURE;
	STDMETHOD(put_AccessType)(THIS_ short nAccessType) PURE;
	STDMETHOD(get_Contents)(THIS_ BSTR FAR* pbstrContents) PURE;
	STDMETHOD(put_Contents)(THIS_ LPCTSTR lpszContents) PURE;
	STDMETHOD(get_AllAttributes)(THIS_ BSTR FAR* pbstrAllAttributes) PURE;
	STDMETHOD(put_AllAttributes)(THIS_ LPCTSTR lpszAllAttributes) PURE;
	STDMETHOD(get_NumberOfAttributes)(THIS_ short FAR* pnNumAttrib) PURE;
	STDMETHOD(get_Conditions)(THIS_ short nWhichFile, BSTR FAR* pbstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ short nWhichFile, LPCTSTR lpszConditions) PURE;
	STDMETHOD(GetScope)(THIS_ long FAR* nScopeType, BSTR FAR* bstrScopeName) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GetReturnType)(THIS_ short nWhichOne, BSTR FAR* pbstrReturnType) PURE;
	STDMETHOD(SetReturnType)(THIS_ short nWhichOne, LPCTSTR lpszReturnType) PURE;
	STDMETHOD(GetNamedAttribute)(THIS_ LPCTSTR lpszName, BSTR FAR* bstrValue, BOOL FAR* bIsDefined) PURE;
	STDMETHOD(SetNamedAttribute)(THIS_ LPCTSTR lpszName, LPCTSTR lpszValue, BOOL bIsDefined) PURE;
	STDMETHOD(GetIndexedAttribute)(THIS_ short nAttribNum, BSTR FAR* bstrName, BSTR FAR* bstrValue) PURE;
	STDMETHOD(GetParameters)(THIS_ BOOL bWithAttributes, BSTR FAR* pbstrParameters) PURE;
	STDMETHOD(SetParameters)(THIS_ LPCTSTR lpszParameters) PURE;
	STDMETHOD(get_NumberOfParameters)(THIS_ short FAR* pnNumParams) PURE;
 	STDMETHOD(GetOneParameter)(THIS_ short nParamNum, BSTR FAR* pbstrParam, BSTR FAR* bstrAttribs) PURE;
	STDMETHOD(SetOneParameter)(THIS_ short nParamNum, LPCTSTR lpszParam, LPCTSTR lpszAttribs) PURE;
	STDMETHOD(GetOneParameterAttributes)(THIS_ short nParamNum, BSTR FAR* bstrAttribs) PURE;
	STDMETHOD(GetOneParameterAttributeCount)(THIS_ short nParamNum, short FAR* nAttribCount) PURE;
	STDMETHOD(GetOneParameterNamedAttribute)(THIS_ short nParamNum, LPCTSTR lpszName, BSTR FAR* bstrValue, 
		BOOL FAR* bIsDefined) PURE;
	STDMETHOD(SetOneParameterNamedAttribute)(THIS_ short nParamNum, LPCTSTR lpszName, LPCTSTR lpszValue, 
		BOOL bIsDefined) PURE;
	STDMETHOD(GetOneParameterIndexedAttribute)(THIS_ short nParamNum, short nAttribNum, BSTR FAR* bstrName, 
		BSTR FAR* bstrValue) PURE;
	STDMETHOD(GetAssociationType)(THIS_ long nAssociationType, LPCTSTR lpszInterfaceName, BSTR FAR* bstrInterfaceName, 
		long FAR* nType) PURE;
	STDMETHOD(GetAssociation)(THIS_ long nAssociationMajorType, long nAssociationMinorType, LPCTSTR lpszName, 
		LPDISPATCH FAR* pvLangFunc, long FAR* pnType, BSTR FAR* pbstrName) PURE;
	STDMETHOD(GetAssociationName)(THIS_ long nAssociationMajorType, long nAssociationMinorType, long nPresentHow,
		BSTR FAR* pbstrName) PURE;
	STDMETHOD(IsVirtual)(THIS_ BOOL FAR* pbIsVirtual) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
	STDMETHOD(Delete)(THIS_ short nPieceToDelete, short nCommentOrDeletePiece) PURE;
	STDMETHOD(GetOneLine)(THIS_ long nFileType, long nLineNumber, BSTR FAR* bstrLineContents) PURE;
	STDMETHOD(SetOneLine)(THIS_ long nFileType, long nLineNumber , LPCTSTR lpszText) PURE;
	STDMETHOD(InsertText)(THIS_ long nFileType, LPCTSTR lpszText, long nLineNumber, long nColumn, 
		BOOL bFullLine) PURE;
#ifdef NOT_YET
	STDMETHOD(GetActiveXMethod)(THIS_ BSTR FAR* bstrInterfaceName, BSTR FAR* bstrMethodName) PURE;
	STDMETHOD(GetActiveXProperty)(THIS_ BSTR FAR* bstrInterfaceName, long FAR* nPart, BSTR FAR* bstrImplGet, 
		BSTR FAR* bstrImplPut, BSTR FAR* bstrPropGet, BSTR FAR* bstrPropPut) PURE;
	STDMETHOD(AddToFunction)(THIS_ long nOffset, int nByNumLines, LPCTSTR lpszText) PURE;
	STDMETHOD(FindExactText)(THIS_ long nLineNumberStart, LPCTSTR lpszText, BOOL bCheckNext, BOOL bCheckComments, 
		BSTR FAR* pbstrFoundFile, BSTR FAR* pbstrFoundLocation, long FAR* nLineNumberFound) PURE;
	STDMETHOD(FindVariable)(THIS_ LPCTSTR lpszVariableType, long nLineNumberStart, BOOL bCheckNext, 
		BSTR FAR* pbstrVariableName, BSTR FAR* pbstrFoundFile, BSTR FAR* pbstrFoundLocation, 
		long FAR* nLineNumberFound) PURE;
	STDMETHOD(WhatIsThis)(THIS_ LPCTSTR lpszText, LPCTSTR lpszFileFound, long nLineNumber, long FAR* nTokenType) PURE;
	STDMETHOD(DeleteOneLine)(THIS_ long nFileType, long nLineNumber) PURE;
#endif	// NOT_YET
}; 

/////////////////////////////////////////////////////////////////////////////
// ILanguageVariable
//   This is an interface for manipulating variables.

#undef  INTERFACE
#define INTERFACE ILanguageVariable
DECLARE_INTERFACE_(ILanguageVariable, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageVariable methods
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* bstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_AccessType)(THIS_ short FAR* pnAccessType) PURE;
	STDMETHOD(put_AccessType)(THIS_ short nAccessType) PURE;
	STDMETHOD(get_Type)(THIS_ BSTR FAR* pbstrType) PURE;
	STDMETHOD(put_Type)(THIS_ LPCTSTR lpszType) PURE;
	STDMETHOD(get_NumberOfAttributes)(THIS_ short FAR* numAttribs) PURE;
	STDMETHOD(get_AllAttributes)(THIS_ BSTR FAR* bstrAttribs) PURE;
	STDMETHOD(put_AllAttributes)(THIS_ LPCTSTR lpszAttribs) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Conditions)(THIS_ BSTR FAR* bstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ LPCTSTR lpszConditions) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(GetScope)(THIS_ long FAR* nScopeType, BSTR FAR* bstrScopeName) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GetNamedAttribute)(THIS_ LPCTSTR lpszName, BSTR FAR* bstrValue, BOOL FAR* bIsDefined) PURE;
	STDMETHOD(SetNamedAttribute)(THIS_ LPCTSTR lpszName, LPCTSTR lpszValue, BOOL bIsDefined) PURE;
	STDMETHOD(GetIndexedAttribute)(THIS_ short nAttribNum, BSTR FAR* bstrName, BSTR FAR* bstrValue) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
	STDMETHOD(Delete)(THIS_ short nPieceToDelete, short nCommentOrDeletePiece) PURE;
#ifdef NOT_YET
	STDMETHOD(GetActiveXProperty)(THIS_ BSTR FAR* bstrInterfaceName, long FAR* nPart, BSTR FAR* bstrOne, 
		BSTR FAR* bstrImplNotify, BSTR FAR* bstrProp, BSTR FAR* bstrTwo) PURE;
#endif	// NOT_YET

}; 

/////////////////////////////////////////////////////////////////////////////
// ILanguageEnum
//   This is an interface for manipulating C++ enums.

#undef  INTERFACE
#define INTERFACE ILanguageEnum
DECLARE_INTERFACE_(ILanguageEnum, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
 	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageEnum methods
#ifdef NOT_YET
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* bstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_AccessType)(THIS_ short FAR* pnAccessType) PURE;
	STDMETHOD(put_AccessType)(THIS_ short nAccessType) PURE;
	STDMETHOD(get_Contents)(THIS_ BSTR FAR* pbstrContents) PURE;
	STDMETHOD(put_Contents)(THIS_ LPCTSTR lpszContents) PURE;
	STDMETHOD(get_Conditions)(THIS_ BSTR FAR* bstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ LPCTSTR lpszConditions) PURE;
	STDMETHOD(get_NumberOfAttributes)(THIS_ short FAR* numAttribs) PURE;
	STDMETHOD(get_AllAttributes)(THIS_ BSTR FAR* bstrAttribs) PURE;
	STDMETHOD(put_AllAttributes)(THIS_ LPCTSTR lpszAttribs) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(GetScope)(THIS_ long FAR* nScopeType, BSTR FAR* bstrScopeName) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GetNamedAttribute)(THIS_ LPCTSTR lpszName, BSTR FAR* bstrValue, BOOL FAR* bIsDefined) PURE;
	STDMETHOD(SetNamedAttribute)(THIS_ LPCTSTR lpszName, LPCTSTR lpszValue, BOOL bIsDefined) PURE;
	STDMETHOD(GetIndexedAttribute)(THIS_ short nAttribNum, BSTR FAR* bstrName, BSTR FAR* bstrValue) PURE;
	STDMETHOD(AppendToContents)(THIS_ LPCTSTR lpszContents) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
#endif	// NOT_YET
}; 

/////////////////////////////////////////////////////////////////////////////
// ILanguageTypedef
//   This is an interface for manipulating C++ typedefs.

#undef  INTERFACE
#define INTERFACE ILanguageTypedef
DECLARE_INTERFACE_(ILanguageTypedef, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
 	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageTypedef methods
#ifdef NOT_YET
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* bstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_Contents)(THIS_ BSTR FAR* pbstrContents) PURE;
	STDMETHOD(put_Contents)(THIS_ LPCTSTR lpszContents) PURE;
	STDMETHOD(get_Conditions)(THIS_ BSTR FAR* bstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ LPCTSTR lpszConditions) PURE;
	STDMETHOD(get_NumberOfAttributes)(THIS_ short FAR* numAttribs) PURE;
	STDMETHOD(get_AllAttributes)(THIS_ BSTR FAR* bstrAttribs) PURE;
	STDMETHOD(put_AllAttributes)(THIS_ LPCTSTR lpszAttribs) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GetNamedAttribute)(THIS_ LPCTSTR lpszName, BSTR FAR* bstrValue, BOOL FAR* bIsDefined) PURE;
	STDMETHOD(SetNamedAttribute)(THIS_ LPCTSTR lpszName, LPCTSTR lpszValue, BOOL bIsDefined) PURE;
	STDMETHOD(GetIndexedAttribute)(THIS_ short nAttribNum, BSTR FAR* bstrName, BSTR FAR* bstrValue) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
#endif	// NOT_YET
}; 

/////////////////////////////////////////////////////////////////////////////
// ILanguageMap
//   This is an interface for manipulating any particular map (as defined by Microsoft)

#undef  INTERFACE
#define INTERFACE ILanguageMap
DECLARE_INTERFACE_(ILanguageMap, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageMap methods
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* pbstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_FileName)(THIS_ long nFileType, short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long nFileType, long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Conditions)(THIS_ long nFileType, BSTR FAR* bstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ long nFileType, LPCTSTR lpszConditions) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GetContents)(THIS_ LPLANGUAGEMAPITEMS FAR* ppvMFCMapItems) PURE;
	STDMETHOD(get_MapParameters)(THIS_ short nWhichPart, BSTR FAR* bstrMapParameters) PURE;	
	STDMETHOD(put_MapParameters)(THIS_ short nWhichPart, LPCTSTR lpszMapParameters) PURE;
	STDMETHOD(GetMapItemSet)(THIS_ short nByParam, LPCTSTR lpszName, LPLANGUAGEMAPITEMS FAR* ppvMapItems) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
	STDMETHOD(AddMapItem)(THIS_ LPCTSTR lpszMapItem, BOOL bInsertAtTop) PURE;
	STDMETHOD(InsertMapItem)(THIS_ long nOffsetLocation, LPCTSTR lpszMapItem) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ILanguageMapItem
//   This is an interface for manipulating a generic map item

#undef  INTERFACE
#define INTERFACE ILanguageMapItem
DECLARE_INTERFACE_(ILanguageMapItem, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ILanguageMapItem methods
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* pbstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_NumberOfParameters)(THIS_ short FAR* numParameters) PURE;
	STDMETHOD(get_FileName)(THIS_ short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Conditions)(THIS_ BSTR FAR* bstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ LPCTSTR lpszConditions) PURE;
	STDMETHOD(get_Contents)(THIS_ BSTR FAR* pbstrContents) PURE;
	STDMETHOD(put_Contents)(THIS_ LPCTSTR lpszContents) PURE;
	STDMETHOD(get_Parameter)(THIS_ short nParamNum, BSTR FAR* pbstrParam) PURE;
	STDMETHOD(put_Parameter)(THIS_ short nParamNum, LPCTSTR lpszParameter) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
	STDMETHOD(Delete)(THIS_ BOOL bCommentOut) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ILanguageMacro
//   This is an interface for manipulating macros understood by the Wizard model

#undef  INTERFACE
#define INTERFACE ILanguageMacro
DECLARE_INTERFACE_(ILanguageMacro, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

#ifdef NOT_YET
	// ILanguageMacro methods
	STDMETHOD(GetName)(THIS_ short nNameType, BSTR FAR* pbstrName) PURE;
	STDMETHOD(SetName)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_NumberOfParameters)(THIS_ short FAR* numParameters) PURE;
	STDMETHOD(get_Parameter)(THIS_ short nParamNum, BSTR FAR* pbstrParam) PURE;
	STDMETHOD(put_Parameter)(THIS_ short nParamNum, LPCTSTR lpszParameters) PURE;
	STDMETHOD(get_Contents)(THIS_ BSTR FAR* pbstrContents) PURE;
	STDMETHOD(put_Contents)(THIS_ LPCTSTR lpszContents) PURE;
	STDMETHOD(get_FileName)(THIS_ short nPathType, BSTR FAR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(THIS_ long nFileType, short nPathType, LPFILES FAR* ppvFiles) PURE;
	STDMETHOD(get_StartLine)(THIS_ long FAR* pnLineNumber) PURE;
	STDMETHOD(get_EndLine)(THIS_ long FAR* pnLineNumber) PURE;
	STDMETHOD(get_Conditions)(THIS_ BSTR FAR* bstrConditions) PURE;
	STDMETHOD(put_Conditions)(THIS_ LPCTSTR lpszConditions) PURE;
	STDMETHOD(get_Configuration)(THIS_ BSTR FAR* pbstrConfig) PURE;
	STDMETHOD(GetScopeObject)(THIS_ LPCTSTR lpszScopeName, LPDISPATCH FAR* pvScopeObject) PURE;
	STDMETHOD(GotoEditorLocation)(THIS_ long nFileType, short nLocationType) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
#endif	// NOT_YET
};

#ifdef NOT_YET
/////////////////////////////////////////////////////////////////////////////
// ICreateClasses
//   This is an interface for creating classes.  It is currently set up to deal well
//   with C++ classes, but there is nothing that says it cannot be used for classes
//   of other languages.

#undef  INTERFACE
#define INTERFACE ICreateClasses
DECLARE_INTERFACE_(ICreateClasses, IDispatch)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
 
    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(THIS_ UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(THIS_ REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(THIS_ DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

	// ICreateClasses methods
	STDMETHOD(get_WizardType)(THIS_ long FAR* pnWizardType) PURE;
	STDMETHOD(put_WizardType)(THIS_ long nWizardType) PURE;
	STDMETHOD(get_Name)(THIS_ BSTR FAR* pbstrName) PURE;
	STDMETHOD(put_Name)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(get_BaseClass)(THIS_ BSTR FAR* pbstrName) PURE;
	STDMETHOD(put_BaseClass)(THIS_ LPCTSTR lpszName) PURE;
	STDMETHOD(GetFiles)(THIS_ BSTR FAR* pbstrHeader, BSTR FAR* pbstrImplementation) PURE;
	STDMETHOD(SetFiles)(THIS_ LPCTSTR lpszHeader, LPCTSTR lpszImplementation) PURE;
	STDMETHOD(SetScope)(THIS_ LPCTSTR lpszScopeType, LPCTSTR lpszScopeName) PURE;
	STDMETHOD(GetOLEOptions)(THIS_ long FAR* pnFlags, BSTR FAR* pbstrName) PURE;
	STDMETHOD(SetOLEOptions)(THIS_ long nFlags, LPCTSTR lpszName) PURE;
	STDMETHOD(get_DialogTemplate)(THIS_ BSTR FAR* pbstrDialogID) PURE;
	STDMETHOD(put_DialogTemplate)(THIS_ LPCTSTR lpszDialogID) PURE;
	STDMETHOD(put_NameFlags)(THIS_ long nFlags) PURE;
	STDMETHOD(put_BaseClassNameList)(THIS_ LPCTSTR lpszBaseClassList) PURE;
	STDMETHOD(put_DialogTemplateFlags)(THIS_ long nFlags) PURE;
	STDMETHOD(put_DialogTitle)(THIS_ LPCTSTR lpszTitle) PURE;
	STDMETHOD(DoModal)(THIS) PURE;
	STDMETHOD(CreateClass)(THIS) PURE;
	STDMETHOD(FindInterface)(THIS_ LPCTSTR lpszInterfaceName, LPVOID FAR* ppvInterface) PURE;
};
#endif	// NOT_YET

/////////////////////////////////////////////////////////////////////////////
// Result codes
// WARNING: make sure you don't conflict numerically with errors in ogapi.h
// that mean something different than what we have here.
// Stay out of the 50 - 299 range unless the error really is the same as the
// one in ogapi.h.  (No note there about overlaps since ogapi.h can ship to ISVs.)
#define WIZ_FAC (463)
#define WIZ_S(code) MAKE_HRESULT(SEVERITY_SUCCESS,WIZ_FAC,code)
#define WIZ_E(code) MAKE_HRESULT(SEVERITY_ERROR,WIZ_FAC,code)

#define WIZ_S_PARSE_PARAM_COUNT_MISMATCH WIZ_S(1)

#define WIZ_E_MEMBER_EXISTS      WIZ_E(50)
#define WIZ_E_MEMBER_CONFLICTS   WIZ_E(51)
#define WIZ_E_MEMBER_NOT_FOUND   WIZ_E(52)

#define WIZ_E_PARSE_MISMATCH_PARENT  WIZ_E(53)
#define WIZ_E_PARSE_NO_TEMPLATE_DEFN WIZ_E(54)
#define WIZ_E_PARSE_NO_DEFN          WIZ_E(55)
#define WIZ_E_PARSE_MULTI_LINE       WIZ_E(56)
#define WIZ_E_PARSE_INVALID_TOKEN    WIZ_E(57)
#define WIZ_E_PARSE_NO_LEXER         WIZ_E(58)
#define WIZ_E_PARSE_EMPTY_STREAM     WIZ_E(59)
#define WIZ_E_PARSE_MISMATCH_BRACKET WIZ_E(60)
#define WIZ_E_PARSE_UNEXPECTED_EOL   WIZ_E(61)
#define WIZ_E_PARSE_EXTRA_TOKEN      WIZ_E(62)
#define WIZ_E_PARSE_CTOR_RET_TYPE    WIZ_E(63)
#define WIZ_E_PARSE_DUP_ACCESS_SPEC  WIZ_E(64)
#define WIZ_E_PARSE_INVALID_MODIFIER WIZ_E(65)
#define WIZ_E_PARSE_MACROERROR       WIZ_E(66)

#define WIZ_E_PARAMETER_CONFLICTS    WIZ_E(67)
#define WIZ_E_DUP_BASE_CLASS		 WIZ_E(68)

#define WIZ_E_NO_FILE				 WIZ_E(100)

#define WIZ_E_NO_TRANSACTION         WIZ_E(300)

#endif	// _WIZAPI_

