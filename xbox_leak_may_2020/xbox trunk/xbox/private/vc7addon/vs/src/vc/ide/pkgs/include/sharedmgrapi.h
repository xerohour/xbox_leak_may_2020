#ifndef _SHAREDMGRAPI_H_
#define _SHAREDMGRAPI_H_

// This file contains the shared API information between the 'old' Wizard Model code manager
// and the 'new' Code Manager model intended to be DO support.

#include <cppsvc.h>
#include <bldtypes.h>

interface IDOStrings;		// handles lists of strings returned from various queries

// these interfaces help during the execution of a transaction
interface IDOExpandTemplates;	// used to expand templates into the project
interface IDOTemplateDictionary;	// dictionary used to help render template

interface IBscPersistentItem;	// PRIVATE in wizard model, used for thinly wrapping IINST info for DOs

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
#define dxWizImport				0x0010	// it's an import
#define dxWizImportlib			0x0020	// it's an importlib
#define dxWizInclude			0x0040	// it's an include
#define dxWizProjectInclude		0x0100	// typically, stdafx.h
#define dxWizGlobalItem			0x0200	// use this to get the #include file for a global func or var 
#define dxWizFileInclude		0x0400	// use this to put an #include in a particular file
#define dxWizAllFileTypes		0x0FFF

// these flags are for determining scope for IDL/ODL import/importlib inclusion
// we need to be able to OR them with dxWizImport and dxWizImportlib
#define dxWizFileScope			0x1000	// it's at file scope (matters only for IDL/ODL)
#define dxWizLibraryScope		0x2000	// it's at library scope (matters only for IDL/ODL)
#define dxWizObjectScope		0x4000	// it's at the scope of whatever object we resolved the filename for

// file existence types
#define dxWizFileExist				0x00000001	// the file already exists
#define dxWizFileInProject			0x00000002	// the file is already in the project
#define dxWizFileInsertIface		0x00010000	// insert an interface appropriately into IDL/ODL
#define dxWizFileInsertCoclass		0x00020000	// insert a coclass appropriately into IDL/ODL
#define dxWizFileInsertDispIface	0x00040000	// insert a dispinterface appropriately into IDL/ODL

// location & hint types
#define dxWizHintNone			0x00000000	// no hint
#define dxWizFilePathDontCare	0x00000000	// yes, we want this to be default...
#define dxWizStart				0x00000001	// start of whatever it is
#define dxWizEnd				0x00000002	// end of whatever it is (end of prototype for comment)
#define dxWizHintCOM			0x00000004	// it's a COM thingie (like STDMETHOD)
#define dxWizHintLineNumber		0x00000008	// hint is a line number
#define dxWizFilePathAbsolute	0x00000100	// absolute path
#define dxWizFilePathRelative	0x00000200	// relative to project directory
#define dxWizFilePathSimple		0x00000400	// just the file name, no directories
#define dxWizName				0x00000800	// want the name of whatever it is
#define dxWizHintGlobalScope	0x00001000	// want the thing at global scope
#define dxWizHintCodeStart		0x00002000	// want start of file, below any #includes, etc.
#define dxWizDoIndents			0x00004000	// want the code we're inserting to be indented like auto-indent would do
#define dxWizClassWiz			0x00008000	// want whatever it is to be compatible with ClassWizard comments
#define dxWizHintFileName		0x00010000	// hint is the file name to use
#define dxWizHintIndex          0x00020000	// hint is an index number
#define dxWizHintUseSameLoc     0x00040000	// use the same location (for comment) as was used before
#define dxWizDuplicateOK        0x00080000  // allow something to duplicate something already present
#define dxWizIgnoreScope        0x00100000  // This is used to handle adding a function in interface::method format

// some basic types
#define dxWizNormal				0x00000000	// can be anything
#define dxWizVariable			0x00000100	// variable
#define dxWizFunction			0x00000200	// function
#define dxWizCoclass			0x00000400	// coclass
#define dxWizInterface			0x00000800	// interface
#define dxWizClass				0x00001000	// class
#define	dxWizTemplate			0x00002000	// it is ok for this thing to be a template
#define dxWizDispinterface		0x00004000	// dispinterface
#define dxWizLibrary			0x00008000	// library
#define dxWizEnum				0x00000001	// enum	-- never OR'ed with other type flags
#define dxWizTypedef			0x00000002	// typedef -- never OR'ed with other type flags
#define dxWizMap				0x00000003	// map -- never OR'ed with other type flags
#define dxWizMacro				0x00000004	// macro -- never OR'ed with other type flags
#define dxWizEnumItem           0x00000005  // enum item -- never OR'ed with other type flags
#define dxWizStruct             0x00000006  // struct -- never OR'ed with other type flags
#define dxWizUnion              0x00000007 // union -- never OR'ed with other type flags
#define dxWizConstant           0x00000008  // constant -- never OR'ed with other type flags
#define dxWizAttribute          0x00000009  // attribute -- never OR'ed with other type flags
#define dxWizBaseClass          0x0000000A  // base class -- never OR'ed with other type flags
#define dxWizParameter          0x0000000B  // parameter -- never OR'ed with other type flags
#define dxWizMapItem            0x0000000C  // map item -- never OR'ed with other type flags
#define dxWizNameSpace			0x0000000D	// name space -- never OR'ed with other type flags

// ActiveX association types
#define dxWizNothing			0x00000000	// current item has no association
	// need to be able to OR these types with dxWizFunction and dxWizVariable
#define dxWizProperty			0x00000001	// current item, variable or function, is actually a property
#define dxWizPropertyNotify		0x00000002	// current item is a property change notification function
#define dxWizMethod				0x00000004	// current item is a function

// generic association major types
#define dxWizActiveXMatch		0x00000010	// ActiveX assoc present
#define dxWizActiveXMissing		0x00000020	// would be present for ActiveX assoc if wasn't missing
#define dxWizActiveXNone		0x00000040	// not related to ActiveX assoc

// these are expected to be just barely smaller than the allowable range
#define dxWizModEndLine			-1					// last line of the module
#define dxWizDefEndLine			dxWizModEndLine-1	// last line of the definition
#define dxWizEndOfLine			-1					// last column of the line
#define dxWizDefBeginLine		dxWizDefEndLine-1	// first line of the definition (for func, mostly)

// file access types
#define dxWizModeRead			0	// read access
#define dxWizModeWrite			1	// read/write access

// symbol types
enum SymbolConflictType 
{ 
	eSymbolConflictNone,		// no conflicts whatsoever for this name
	eSymbolConflictInProject,	// this symbol exists in this project at this scope
	eSymbolConflictIntrinsic,	// this symbol is an intrinsic keyword (like 'int')
	eSymbolConflictMFCName		// this global symbol is the same as an MFC class name
};

// automation style enforced on members of the specified class
enum memberAutomationStyles 
{ 
	memAutoNone,				// not an automation class at all
	memAutoDispinterface,		// dispinterface
	memAutoInterfaceDual,		// dual interface
	memAutoInterfaceAutomation,	// interface with the automation attribute
	memAutoInterfaceObject,		// interface with the object attribute and not the local attribute
	memAutoInterfaceLocal		// interface with both object and local attributes
};

// Open flags for working with the rc file
#define dxWizOpenAlwaysRC   (0)
#define dxWizCreateNewRC    (1)
#define dxWizOpenExistingRC (2)

// short hand ways of specifying default projects/targets
#define dxWizAllProjects		(HPROJECT)-1
#define dxWizSelectedProject	(HPROJECT)NULL

// these exactly match the build system's Prop_UseOfMFC values
enum UseOfMFC { eMFCNone = 0, eMFCStaticLib = 1, eMFCDLL = 2 };		

/////////////////////////////////////////////////////////////////////////////
// IDOTemplateDictionary
//   This interface handles template creation including the generation of templates that
//	 use that dictionary.

#undef  INTERFACE
#define INTERFACE IDOTemplateDictionary
DECLARE_INTERFACE_(IDOTemplateDictionary, IDispatch)
{
	// value interaction
	STDMETHOD(GetValue)(BSTR bstrKey, BSTR* pbstrValue) PURE;
	STDMETHOD(SetValue)(BSTR bstrKey, BSTR bstrValue) PURE;	// can set new or reset existing
	STDMETHOD(RemoveValue)(BSTR bstrKey) PURE;
	STDMETHOD(ClearValues)() PURE;

	// template creation
	STDMETHOD(CreateTemplateFromFile)(BSTR bstrTemplateFile, IDOExpandTemplates** ppvObj) PURE;
	STDMETHOD(CreateTemplateFromString)(BSTR bstrTemplateString, IDOExpandTemplates** ppvObj) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IDOExpandTemplates
//   This interface is used to interact with templates.
//	NOTE: you can create one of these without a transaction, but you must have a transaction
//		in place to render one.

#undef  INTERFACE
#define INTERFACE IDOExpandTemplates
DECLARE_INTERFACE_(IDOExpandTemplates, IDispatch)
{
	STDMETHOD(RenderIntoFile)(BSTR bstrFileName, long nHint, DWORD iLine, DWORD iColumn, BSTR bstrTab) PURE;
	STDMETHOD(RenderIntoString)(BSTR bstrTab, BSTR* pbstrRenderedTemplate) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IDOStrings
//   This interface provides access to generic lists of Wizard or DO string data.

#undef  INTERFACE
#define INTERFACE IDOStrings
DECLARE_INTERFACE_(IDOStrings, IDispatch)
{
	STDMETHOD(_NewEnum)(IEnumVARIANT **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IBscPersistentItem
//	This interface is the extremely thin wrapper we put on IINST objects
//	to pass them from the code manager to the DOs.
//
//	It is also an interface that all items in a CBscCollection must support.
//
//	For 'old' style Wizard Model wizards, this can be considered a PRIVATE interface
//	inside the code manager.

#undef  INTERFACE
#define INTERFACE IBscPersistentItem
DECLARE_INTERFACE_(IBscPersistentItem, IDispatch)
{
	STDMETHOD(GetIinst)(DWORD *piinst) PURE;
	STDMETHOD(SetIinst)(DWORD iinst) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// Result codes
// WARNING: make sure you don't conflict numerically with errors in ogapi.h
// that mean something different than what we have here.
// Stay out of the 50 - 299 range unless the error really is the same as the
// one in ogapi.h.  (No note there about overlaps since ogapi.h can ship to ISVs.)
#define WIZ_FAC (463)
#define WIZ_S(code) MAKE_HRESULT(SEVERITY_SUCCESS,WIZ_FAC,code)
#define WIZ_E(code) MAKE_HRESULT(SEVERITY_ERROR,WIZ_FAC,code)

#define WIZ_S_CLASSWIZ_UNRECOGNIZED_AUTOMATION_TYPE  WIZ_S(3)
#define WIZ_S_AUTOMATION_FIXED_PARAM_LIST            WIZ_S(4)
#define WIZ_S_ACCESS_TYPE_NEEDED                     WIZ_S(5)	// insert location found, but desired access specifier needs to be added

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
#define WIZ_E_PARSE_PARAM_COUNT_MISMATCH WIZ_E(69)

#define WIZ_E_NO_FILE				 WIZ_E(100)
#define WIZ_E_FILE_READONLY			 WIZ_E(101)

#define WIZ_E_NO_TRANSACTION         WIZ_E(300)
#define WIZ_E_BODY_EXPECTED          WIZ_E(301)
#define WIZ_E_CLASS_NOT_FOUND        WIZ_E(302)
#define WIZ_E_NO_PARSE_DATA          WIZ_E(303)
#define WIZ_E_INVALID_PARSE_DATA     WIZ_E(304)
#define WIZ_E_ILLEGAL_OPERATION      WIZ_E(305)
#define WIZ_E_FUNCTION_EXPECTED      WIZ_E(306)
#define WIZ_E_NOT_FOUND              WIZ_E(307)
#define WIZ_E_PARENT_NOT_FOUND       WIZ_E(308)
#define WIZ_E_OUT_OF_RANGE           WIZ_E(309)
#define WIZ_E_INTERNAL_ERROR         WIZ_E(310)
#define WIZ_E_USER_CANCEL            WIZ_E(311)
#define WIZ_E_IMPL_FILE              WIZ_E(312)
#define WIZ_E_LEXER_NOT_INIT         WIZ_E(313)
#define WIZ_E_WIZARD_CANT_RUN        WIZ_E(314)
#define WIZ_E_NO_TARGET              WIZ_E(315)
#define WIZ_E_AMBIGUOUS_FLAGS        WIZ_E(316)
#define WIZ_E_CONFLICTS              WIZ_E(317)
#define WIZ_E_RETURN_TYPE_FIXED      WIZ_E(318)
#define WIZ_E_READ_ONLY_PROPERTY     WIZ_E(319)
#define WIZ_E_AMBIGUOUS              WIZ_E(320)
#define WIZ_E_TEMPLATE_ELEMENT_NOT_FOUND WIZ_E(350)
#define WIZ_E_TEMPLATE_INVALID_VALUE     WIZ_E(351)
#define WIZ_E_TEMPLATE_MISMATCHED_TOKENS WIZ_E(352)
#define WIZ_E_TEMPLATE_MISMATCHED_IF     WIZ_E(353)
#define WIZ_E_TEMPLATE_SYNTAX_ERR        WIZ_E(354)
#define WIZ_E_TEMPLATE_EMPTY_CONDITIONAL WIZ_E(355)
#define WIZ_E_TEMPLATE_LOOP_ERR          WIZ_E(356)
#define WIZ_E_TEMPLATE_EXTRA_END         WIZ_E(357)
#define WIZ_E_FUNC_NAME_FIXED        WIZ_E(400)
#define WIZ_E_FUNC_TYPE_FIXED        WIZ_E(401)
#define WIZ_E_PARAM_TYPES_FIXED      WIZ_E(402)
#define WIZ_E_NOT_AUTOMATION_TYPE    WIZ_E(403)
#define WIZ_E_INVALID_IDENTIFIER_NAME    WIZ_E(404)

#endif	// _SHAREDMGRAPI_H_