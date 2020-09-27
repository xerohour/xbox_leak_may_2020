#ifndef __OGAPI_H__
#define __OGAPI_H__

// Component Gallery interface declarations

// These are all interfaces on an apply context object
interface IApplyContext;
interface IManageValues;
interface IAddFiles;
interface IAddResources;
interface IFindClasses;
interface IWriteCode;

// This interface is used to create a new class.
interface ICreateClasses;

interface IFormStrings;	// handles lists of strings returned from various queries from ICreateClasses


// This is an interface provided by a client of the apply context, in order to
// provide information to the user while editing the value map.
interface IExplainValues;

// These interfaces are used as results from the parser
interface IEnumClasses;
interface IExplainClasses;

interface IEnumMembers;
interface IExplainMembers;

// This interface is used to interact with templates.
interface IExpandTemplates;

// These interfaces are supplied by external Components.
interface IComponentType;
interface IImportComponents;
interface IComponent;
interface IExplainComponents;

// This interface is used by external components to
// interact with the Gallery.
interface IComponentGallery;


// Interface pointer types.
typedef IApplyContext* LPAPPLYCONTEXT;
typedef IManageValues* LPMANAGEVALUES;
typedef IAddFiles* LPADDFILES;
typedef IAddResources* LPADDRESOURCES;
typedef IFindClasses* LPFINDCLASSES;
typedef IWriteCode* LPWRITECODE;
typedef ICreateClasses* LPCREATECLASSES;
typedef IExplainValues* LPEXPLAINVALUES;
typedef IEnumClasses* LPENUMCLASSES;
typedef IExplainClasses* LPEXPLAINCLASSES;
typedef IEnumMembers* LPENUMMEMBERS;
typedef IExplainMembers* LPEXPLAINMEMBERS;
typedef IExpandTemplates* LPEXPANDTEMPLATES;
typedef IComponentType* LPCOMPONENTTYPE;
typedef IImportComponents* LPIMPORTCOMPONENTS;
typedef IComponent* LPCOMPONENT;
typedef IExplainComponents* LPEXPLAINCOMPONENTS;
typedef IComponentGallery* LPCOMPONENTGALLERY;
typedef IFormStrings* LPFORMSTRINGS;


/////////////////////////////////////////////////////////////////////////////
// IApplyContext
//   This is the primary interface on an Apply Context object.

#undef  INTERFACE
#define INTERFACE IApplyContext
DECLARE_INTERFACE_(IApplyContext, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IApplyContext methods
	STDMETHOD(FinishApply)(THIS_ BOOL bRemember) PURE;
	STDMETHOD(AbortApply)(THIS) PURE;

	STDMETHOD(DoModal)(THIS_ IExplainValues FAR* pev) PURE;

	STDMETHOD(GetIComponentGallery)(THIS_ IComponentGallery ** pCG) PURE;

};


/////////////////////////////////////////////////////////////////////////////
// IManageValues
//   This interface allows interaction with the value table.

#undef  INTERFACE
#define INTERFACE IManageValues
DECLARE_INTERFACE_(IManageValues, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Value interaction
	STDMETHOD(AddValue)(THIS_ LPCSTR pszKey, LPCSTR pszValue) PURE;
	STDMETHOD(GetValue)(THIS_ LPCSTR pszKey, LPSTR FAR* ppszValue) PURE;
	STDMETHOD(SetValue)(THIS_ LPCSTR pszKey, LPCSTR pszValue) PURE;
	STDMETHOD(RemoveValue)(THIS_ LPCSTR pszKey) PURE;
	STDMETHOD(ClearValues)(THIS) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IAddFiles
//   This interface on an Apply Context object is used to interact with the
//   project.  i.e. add/enumerate files, etc.

#define DW_FILE_EXIST	0x00000001
#define DW_FILE_IN_PROJECT 0x00000002
#define DW_FILE_INSERT_IFACE 0x00010000
#define DW_FILE_INSERT_COCLASS 0x00020000

#undef  INTERFACE
#define INTERFACE IAddFiles
DECLARE_INTERFACE_(IAddFiles, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Filename methods.
	STDMETHOD(CalculateFilename)(THIS_ LPCSTR pszFilename, LPSTR FAR* ppszNewFilename) PURE;
	STDMETHOD(SaveFilename)(THIS_ LPCSTR pszDesiredFilename, LPCSTR pszActualFilename) PURE;
	STDMETHOD(AddFile)(THIS_ LPCSTR pszFilename) PURE;
	STDMETHOD (CalculateFilenameEx) (THIS_ LPCSTR pszFilename, LPSTR FAR * ppszNewFilename, DWORD * pdwHint) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IAddResources
//   This interface on an Apply Context object is used to interact with
//   the projects resource script.

#undef  INTERFACE
#define INTERFACE IAddResources
DECLARE_INTERFACE_(IAddResources, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Resource methods.
	STDMETHOD(Create)(THIS_ UINT nCreateFlags) PURE;
	STDMETHOD(GetResourceInclude)(THIS_ LPSTR FAR* ppszFilename) PURE;

	STDMETHOD(AttachResourceStream)(THIS_ LPSTREAM lpStream) PURE;
	STDMETHOD(CreateSymbol)(THIS_ LPCSTR pszID, LPSTR FAR* ppszActualID) PURE;
	STDMETHOD(CreateResource)(THIS_ LPCSTR pszType, LPCSTR pszID, int nLangID, LPCSTR szCondition, LPCSTR pszNewID) PURE;
	STDMETHOD(ImportResource)(THIS_ LPCSTR pszType, LPCSTR pszID, LPCSTR pszFileName, int nLangID, LPCSTR szCondition) PURE;
	STDMETHOD(SetResourceID)(THIS_ LPCSTR pszType, LPCSTR pszOldID, int nLangID, LPCSTR szCondition, LPCSTR pszNewID) PURE;
	STDMETHOD(CreateString)(THIS_ LPCSTR pszID, int nLangID, LPCSTR szCondition, LPCSTR pszString) PURE;
	STDMETHOD(Access)(THIS_ UINT nMode) PURE;
};
// Open flags
#define OPEN_ALWAYS_RC   (0)
#define CREATE_NEW_RC    (1)
#define OPEN_EXISTING_RC (2)

// Access mode flags
#ifndef MODE_READ
#define MODE_READ   (0)
#endif
#ifndef MODE_WRITE
#define MODE_WRITE  (1)
#endif

/////////////////////////////////////////////////////////////////////////////
// IFindClasses
//   This interface on an Apply Context object is used to query the parser
//   as to the location and existence of interesting things.

#undef  INTERFACE
#define INTERFACE IFindClasses
DECLARE_INTERFACE_(IFindClasses, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Class interaction methods.
	STDMETHOD(GetClassInfo)(THIS_ LPCSTR pszClass, IExplainClasses FAR** ppec) PURE;
	STDMETHOD(GetDerivedClasses)(THIS_ LPCSTR pszParent, BOOL bImmediateOnly, IEnumClasses FAR** ppec) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IWriteCode
//   This interface on an Apply Context object is used to make code changes
//   to the project.  It allows the addition of new members, and some simple
//   additions to existing members.

#undef  INTERFACE
#define INTERFACE IWriteCode
DECLARE_INTERFACE_(IWriteCode, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Code modification methods
	STDMETHOD(InsertMember)(THIS_ LPCSTR pszMember, UINT nAccess) PURE;
	STDMETHOD(CreateImplementation)(THIS_ LPCSTR pszMember, LPCSTR pszCode, UINT nPlacement, BOOL bCreate) PURE;
	STDMETHOD(InsertMapEntry)(THIS_ LPCSTR pszMap, LPCSTR pszClass, LPCSTR pszEntry) PURE;

	// Helper methods
	STDMETHOD(ValidateSymbol)(THIS_ LPCSTR pszSymbol) PURE;
	STDMETHOD(IsMFCClass)(THIS_ LPCSTR pszClass) PURE;
	STDMETHOD(CreateFilenameDefine)(THIS_ LPCSTR pszFilename, LPSTR FAR *ppszSymbol) PURE;

	// Include directive methods
	STDMETHOD(AddProjectInclude)(THIS_ LPCSTR pszIncludeFile) PURE;
	STDMETHOD(AddClassInclude)(THIS_ LPCSTR pszClass, LPCSTR pszIncludeFile, UINT nFlags) PURE;
	STDMETHOD(AddFileInclude)(THIS_ LPCSTR pszFilename, LPCSTR pszIncludeName) PURE;
	
	// Generic code addition.
	STDMETHOD(AddClassCode)(THIS_ LPCSTR pszClass, LPCSTR pszCode) PURE;
	STDMETHOD(AddFileCode)(THIS_ LPCSTR pszFile, LPCSTR pszCode) PURE;

	// Project query methods.
	STDMETHOD(GetProjectName)(THIS_ LPSTR FAR* ppszProjectName) PURE;

	// Insert file content to file
	STDMETHOD(InsertFileContent)(THIS_ LPCSTR pszFileContent, LPCSTR szLang, DWORD dwHint, LPCSTR pszFileName) PURE;

	//Get name of project include file (99% of time is stdafx.h)
	STDMETHOD(GetProjectIncludeFile)(THIS_ LPSTR FAR* ppszProjectInclude) PURE;

};
// Access flags
#define ACCESS_DONTCARE  (0)
#define ACCESS_PUBLIC    (1)
#define ACCESS_PROTECTED (2)
#define ACCESS_PRIVATE   (3)

// Placement flags
#define PLACEMENT_BEGIN         (1)
#define PLACEMENT_END           (2)
#define PLACEMENT_BEFORE_PARENT (3)
#define PLACEMENT_AFTER_PARENT  (4)

// Include flags
#define CLASS_DEFINITION	(0)
#define CLASS_IMPLEMENTATION	(1)


/////////////////////////////////////////////////////////////////////////////
// ICreateClasses
//   This interface is returned by the IOGCodeMunge interface, and is used to
//   manipulate the attributes of a new class.

#undef  INTERFACE
#define INTERFACE ICreateClasses
DECLARE_INTERFACE_(ICreateClasses, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

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
	STDMETHOD(SetDialogTitle)(THIS_ LPCSTR pszTitle) PURE;
	STDMETHOD(DoModal)(THIS) PURE;

	// This method actually creates the class.
	STDMETHOD(CreateClass)(THIS) PURE;

	// Error handling for during CreateClass.  We may need to call these
	// if we only called into ICreateClasses2 programatically.  (UI path
	// includes checking with the user in these situations and handling
	// problems that way)
	STDMETHOD(SetGuidFlags)(THIS_ UINT nFlags) PURE;
	STDMETHOD(SetResFlags)(THIS_ UINT nFlags) PURE;

	// Information retrieval.
	STDMETHOD(GetName)(THIS_ LPSTR FAR* ppszName) PURE;
	STDMETHOD(GetBaseClass)(THIS_ LPSTR FAR* ppszName) PURE;
	STDMETHOD(GetFiles)(THIS_ LPSTR FAR* ppszHeader, LPSTR FAR* ppszImplementation) PURE;
	STDMETHOD(GetOLEOptions)(THIS_ UINT FAR* pnFlags, LPSTR FAR* ppszName) PURE;
	STDMETHOD(GetDialogTemplate)(THIS_ LPSTR FAR* ppszDialogID) PURE;

	STDMETHOD(GetFormStrings)(THIS_ UINT nTypeForm, LPFORMSTRINGS *ppFormStrings) PURE;
};

// FORM String interface types (nTypeForm values in GetFormStrings)

#define FS_DIALOG_ID (1)
#define FS_BASE_CLASS (2)


// OLE Option flags
#define NC_OLE_NONE   (0)
#define NC_OLE_AUTO   (1)
#define NC_OLE_CREATE (2)
// UI flags
#define NC_ALLOW_EDIT    (0)
#define NC_DISALLOW_EDIT (1)
// Base class filters
#define NC_BASE_ALL   (0x0000)
#define NC_BASE_DOC   (0x0100)
#define NC_BASE_VIEW  (0x0200)
#define NC_BASE_FRAME (0x0400)
#define NC_BASE_DLG   (0x0800)
#define NC_BASE_CTRL  (0x1000)

// this one causes some classes to be added to the set which "ALL" doesn't include.
// ONLY SET THIS FLAG IF YOU ARE GOING TO BE USING ClassWizard UI WITH ICreateClasses.
// The database source won't get setup properly if you don't let ClassWizard handle
// the UI.
#define NC_BASE_ADD_DB_CLASSES	(0x2000)

// Error response flags
#define NC_USE_EXISTING_GUID   (1)
#define NC_CREATE_NEW_GUID     (2)
#define NC_USE_RES_ASIS        (3)
#define NC_USE_PLACEHOLDER_RES (4)

/////////////////////////////////////////////////////////////////////////////
// IExplainValues
//   This is an interface used by a client of the apply context, in order to
//   provide information to the user while editing the value map.

#undef  INTERFACE
#define INTERFACE IExplainValues
DECLARE_INTERFACE_(IExplainValues, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Validation methods
	STDMETHOD(ValidateValue)(THIS_ LPCSTR pszKey, LPCSTR pszNewValue) PURE;

	// Information methods.  The info returned by these methods
	// is used to populate the grid control in the value editor.
	STDMETHOD(GetValueInfo)(THIS_ LPCSTR pszKey, LPSTR FAR* ppszName, LPSTR FAR* ppszType, LPSTR FAR* ppszExtraInfo) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IEnumClasses
//   This is a simple interface to a list of classes.  It is used as a result
//   from some of the parser queries.

#undef  INTERFACE
#define INTERFACE IEnumClasses
DECLARE_INTERFACE_(IEnumClasses, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnum style methods
	STDMETHOD(Next)(THIS_ ULONG celt, IExplainClasses FAR** rgelt, ULONG* pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumClasses** ppenum) PURE;

	// Custom search method.
	STDMETHOD(Find)(THIS_ LPCSTR pszClass, IExplainClasses FAR** ppec) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IExplainClasses
//   Use this interface to get details on a particular class.

#undef  INTERFACE
#define INTERFACE IExplainClasses
DECLARE_INTERFACE_(IExplainClasses, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Class information methods.
	STDMETHOD(GetName)(THIS_ LPSTR FAR* ppszName) PURE;
	STDMETHOD(GetBaseClasses)(THIS_ LPSTR FAR* ppszParent) PURE;
	STDMETHOD(GetMembers)(THIS_ IEnumMembers FAR** ppem) PURE;
	STDMETHOD(Access)(THIS_ UINT nMode) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IEnumMembers
//   This is a simple interface on a list of members

#undef  INTERFACE
#define INTERFACE IEnumMembers
DECLARE_INTERFACE_(IEnumMembers, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnum style methods
	STDMETHOD(Next)(THIS_ ULONG celt, IExplainMembers FAR** rgelt, ULONG* pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumMembers** ppenum) PURE;

	// Custom search method.
	STDMETHOD(Find)(THIS_ LPCSTR pszMember, IExplainMembers FAR** ppem) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IExplainMembers
//   Use this interface to get details on a member

#undef  INTERFACE
#define INTERFACE IExplainMembers
DECLARE_INTERFACE_(IExplainMembers, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Member information methods.
	STDMETHOD(GetName)(THIS_ LPSTR FAR* ppszName) PURE;
	STDMETHOD(GetType)(THIS_ LPSTR FAR* ppszType) PURE;
	STDMETHOD(GetAccess)(THIS_ UINT FAR* pnAccess) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IExpandTemplates
//   This interface is used to interact with templates.

#undef  INTERFACE
#define INTERFACE IExpandTemplates
DECLARE_INTERFACE_(IExpandTemplates, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	STDMETHOD(Render)(THIS_ LPSTREAM lpStm, IApplyContext FAR* lpAC) PURE;
	STDMETHOD (RenderIntoFile) (THIS_ LPCTSTR szFileName, LPCTSTR szLang, DWORD dwHint, IApplyContext FAR * lpAC) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IComponentType
//   This interface is exposed by external components.

#undef  INTERFACE
#define INTERFACE IComponentType
DECLARE_INTERFACE_(IComponentType, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Component type methods.
	STDMETHOD(OnRegister)(THIS_ UINT nTypeID, LPCOMPONENTGALLERY lpcg) PURE;
	STDMETHOD(OnInitialRegister)(THIS) PURE;
	STDMETHOD(BltTypeGlyph)(THIS_ HDC hDC, int x, int y) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IImportComponents
//   This interface is exposed by external components.

#undef  INTERFACE
#define INTERFACE IImportComponents
DECLARE_INTERFACE_(IImportComponents, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Import methods.
	STDMETHOD(ImportFile)(THIS_ LPCSTR pszFilename, BOOL bCopyToGallery) PURE;
	STDMETHOD(GetFilterStrings)(THIS_ LPSTR FAR* ppszFilter) PURE;
	STDMETHOD(CanImportFile)(THIS_ LPCSTR pszFile) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IComponent
//   This interface is exposed by external components.

#undef  INTERFACE
#define INTERFACE IComponent
DECLARE_INTERFACE_(IComponent, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Component methods.
	STDMETHOD(GetTypeID)(THIS_ UINT FAR* pnTypeID) PURE;
	STDMETHOD(OnInsertComponent)(THIS_ LPAPPLYCONTEXT lpac) PURE;
	STDMETHOD(OnComponentHelp)(THIS) PURE;

	STDMETHOD(WriteState)(THIS_ LPSTREAM lpsr) PURE;
	STDMETHOD(ReadState)(THIS_ LPSTREAM lpsr) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IExplainComponents
//   This interface is exposed by external components.

#undef  INTERFACE
#define INTERFACE IExplainComponents
DECLARE_INTERFACE_(IExplainComponents, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// Component information methods.
	STDMETHOD(GetCLSID)(THIS_ CLSID FAR* pClsID) PURE;
	STDMETHOD(GetStrings)(THIS_ LPSTR FAR* ppvTitle, LPSTR FAR* ppvDescription) PURE;
	STDMETHOD(GetIcon)(THIS_ HICON FAR* phIcon) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IComponentGallery
//   This interface is used to interact with the Gallery itself.

#undef  INTERFACE
#define INTERFACE IComponentGallery
DECLARE_INTERFACE_(IComponentGallery, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IComponentGallery methods
	STDMETHOD(GetGalleryLocation)(THIS_ LPSTR FAR* ppszPath) PURE;
	STDMETHOD(AddComponent)(THIS_ IComponent FAR* lpc, UINT nTypeID, LPCSTR pszCategory) PURE;
	STDMETHOD(CreateTemplate)(THIS_ LPSTREAM lpStm, IExpandTemplates FAR** ppvObj) PURE;
	STDMETHOD(CreateNewClass)(THIS_ ICreateClasses FAR** ppvObj) PURE;
	STDMETHOD(CheckVersion)(THIS_ UINT nMajor, UINT nMinor) PURE;
	STDMETHOD(CreateTemplateFromText)(THIS_ LPSTREAM lpStm, IExpandTemplates FAR** ppvObj) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// Version number:
#define GALLERY_VER_MAJ (0x0002)
#define GALLERY_VER_MIN (0x0000)


/////////////////////////////////////////////////////////////////////////////
// Result codes
#define CG_FAC (463)
#define CG_S(code) MAKE_HRESULT(SEVERITY_SUCCESS,CG_FAC,code)
#define CG_E(code) MAKE_HRESULT(SEVERITY_ERROR,CG_FAC,code)

#define CG_E_MEMBER_EXISTS      CG_E(50)
#define CG_E_MEMBER_CONFLICTS   CG_E(51)
#define CG_E_MEMBER_NOT_FOUND   CG_E(52)

#define CG_E_PARSE_MISMATCH_PARENT  CG_E(53)
#define CG_E_PARSE_NO_TEMPLATE_DEFN CG_E(54)
#define CG_E_PARSE_NO_DEFN          CG_E(55)
#define CG_E_PARSE_MULTI_LINE       CG_E(56)
#define CG_E_PARSE_INVALID_TOKEN    CG_E(57)
#define CG_E_PARSE_NO_LEXER         CG_E(58)
#define CG_E_PARSE_EMPTY_STREAM     CG_E(59)
#define CG_E_PARSE_MISMATCH_BRACKET CG_E(60)
#define CG_E_PARSE_UNEXPECTED_EOL   CG_E(61)
#define CG_E_PARSE_EXTRA_TOKEN      CG_E(62)
#define CG_E_PARSE_CTOR_RET_TYPE    CG_E(63)
#define CG_E_PARSE_DUP_ACCESS_SPEC  CG_E(64)
#define CG_E_PARSE_INVALID_MODIFIER CG_E(65)
#define CG_E_PARSE_MACROERROR       CG_E(66)
#define CG_E_PARSE_PARAM_COUNT_MISMATCH CG_E(67)

#define CG_E_NO_FILE						 CG_E(100)

#define CG_E_CREATECLASS_NO_CLASSNAME        CG_E(200)
#define CG_E_CREATECLASS_RESERVED_CLASSNAME  CG_E(201)
#define CG_E_CREATECLASS_EXISTING_CLASSNAME  CG_E(202)
#define CG_E_CREATECLASS_NO_HEADERFILE       CG_E(203)
#define CG_E_CREATECLASS_CREATE_HEADERFILE   CG_E(204)
#define CG_E_CREATECLASS_NO_IMPLFILE         CG_E(205)      
#define CG_E_CREATECLASS_CREATE_IMPLFILE     CG_E(206)
#define CG_E_CREATECLASS_WRITE_FILES         CG_E(207)
#define CG_E_CREATECLASS_INVALID_FILES       CG_E(208)
#define CG_E_CREATECLASS_NO_BASECLASS        CG_E(209)
#define CG_E_CREATECLASS_UNKNOWN_BASECLASS   CG_E(210)
#define CG_E_CREATECLASS_GUID_EXISTS         CG_E(211)
#define CG_E_CREATECLASS_CLASS_EXISTS        CG_E(212)
#define CG_E_CREATECLASS_DUP_RES             CG_E(213)
#define CG_E_CREATECLASS_INVALID_RES         CG_E(214)
#define CG_E_CREATECLASS_BLANK_RES           CG_E(215)
#define CG_E_CREATECLASS_QUOTED_RES          CG_E(216)
#define CG_E_CREATECLASS_NO_CLASSWIZINFO     CG_E(217)
#define CG_E_CREATECLASS_COCREATEGUID_FAILED CG_E(218)
#define CG_E_CREATECLASS_NEED_RECSET         CG_E(219)


/////////////////////////////////////////////////////////////////////////////
// IFormStrings
//   This interface provides access to generic lists of New Form string data.

#undef  INTERFACE
#define INTERFACE IFormStrings
DECLARE_INTERFACE_(IFormStrings, IUnknown)
{
	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
 	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;

	// IFormStrings methods
	STDMETHOD(_NewEnum)(IEnumVARIANT **ppenum) PURE;
};


#endif // __OGAPI_H__
