#ifndef _CODEMGRAPI_
#define _CODEMGRAPI_

// This file contains the interfaces that make up the Code Manager.  The Code Manager is
// intended to act as a support layer for code manipulation for DOs.  Those elements that 
// are shared between the Wizard Model and the DO support style of code manager are in
// mgrsharedapi.h

#include <sharedmgrapi.h>

// Conventions:
//    Passing NULL in the HPROJECT parameter will result in the selected project being used.  
//       The selected project is the one seen by the code manager -- this is the active project 
//       unless otherwise set by someone calling into the code manager.
//    If you see a method with the same name in two interfaces, it takes the same 
//       parameters and performs the same action.  (Some methods in ICodeMgrSupport_Internal 
//       take slightly different parameters, but that interface isn't intended for clients 
//       of the code manager to use.)
//  Parameter explanations:
//    iinst - the IINST for the thing being changed
//    instParent - the IINST for the object's parent (ex: class IINST when modifying an 
//       attribute)
//    iinstClass - the IINST for a class or class-like object (ex: class IINST when 
//       modifying a base class)
//    iinstObject - the IINST for the function/enum/whatever-makes-sense-here when 
//       modifying a parameter
//    nWhichPart - which part to modify.  typically, dxWizDefinition, dxWizDeclaration, or 
//       both (could also eventually include dxWizMemberDefinitions for classes later).
//    nCommentOrDeletePart - OR in the flag(s) you want hard deletes for (instead of 
//       getting it commented out).  Pass 0 if you want all affected parts commented out.
//    nFileType - the type of file desired.  Supported flags: dxWizDefinition, 
//       dxWizDeclaration, dxWizMemberDefinitions, dxWizMemberDeclarations.  Can be OR'ed 
//       together when the returned value is a collection.
//    nHintType - placement hints for various methods.  Flags can be OR'ed together.  
//       Frequently (but not always) accompanied by nHintName that can provide further info 
//       for one of the specified hint types.  Legal values here vary by the method taking
//       this parameter.
//    nHintName - placement hint help for various methods.  Provides extra information 
//       needed by some hint types
//    nPathType - type of path. Legal values: dxWizFilePathDontCare, dxWizFilePathAbsolute,
//       dxWizFilePathRelative, dxWizFilePathSimple
//    nMemberKind - type of object.  Examples: dxWizClass, dxWizFunction.  Not to be 
//       confused with the object's type (like "int")
//    nParseLanguage - language to use.  Legal values: dxWizIDL (also covers ODL) and 
//       dxWizCPP
//    nParseType - type of parsing to do.  dxWizName causes keywords (like 'afx_msg' or 
//       'virtual') to be stripped out.  Default is to strip out both keywords and default 
//       parameter values.

interface ICodeManager;		// entry point to the code manager support layer for DOs
interface ICodeMgrBscItems;	// collection of IBscPersistentItem objects.  Used when a
							// collection of IINSTs needs to be returned from the code 
							// manager to a DO

// Transaction management
interface IDOTransaction;			// basic transaction
interface IDOTransaction_Internal;	// transaction with all the goodies to do code modification
interface ITransMgrResourceHelper;	// 'temporary' stand-in for transacted resource DOs
interface ITransMgrProjHelper;		// allows transacted addition of files to project
interface IDOTransactionParticipant;// 'smart' participant in transaction (like the text editor, etc.)

// Text editor services
interface ICodeMgrTextEditor;		// the text editor the code manager uses
interface ICodeMgrTextEditorCommand; // an individual command for the text editor
interface ICodeMgrLexer;				// the lexer that goes with a particular text file/language

// parser language to use (values for nParseLanguage)
#define dxWizCPP                0x01		// C/C++
#define dxWizIDL                0x02        // IDL/ODL
#define dxWizAllLanguages       0xFF		// all languages

/////////////////////////////////////////////////////////////////////////////
// ICodeManager
//   This is the primary interface on the transaction manager backend.
//

#undef  INTERFACE
#define INTERFACE ICodeManager
DECLARE_INTERFACE_(ICodeManager, IUnknown)
{
	// general services
	STDMETHOD(GetHelper)(REFIID riidInterface, IUnknown** ppvInterface) PURE;
	STDMETHOD(GetCodeManagerLocation)(BSTR* pbstrCodeMgrLoc) PURE;	// location of code templates
	STDMETHOD(GotoTextEditorLocation)(BSTR bstrFilename, ULONG uLineNumber, ULONG uColumnNumber) PURE;
	STDMETHOD(GotoAnsiTextEditorLocation)(LPCTSTR lpszFilename, ULONG uLineNumber, ULONG uColumnNumber) PURE;
		// Goto(Ansi)TextEditorLocation opens the specified file to the specified location in the text editor
		// uColumnNumber = -1 means highlight the entire line
	STDMETHOD(GotoFileClassMember)(BSTR bstrFilename, BSTR bstrClassName, BSTR bstrMemberName) PURE;
	STDMETHOD(GotoAnsiFileClassMember)(LPCTSTR lpszFilename, LPCTSTR lpszClassName, LPCTSTR lpszMemberName) PURE;
		// GetTextEditor()'s bForceOpen needs to be TRUE in order to get global undo to work on the file,
		// so you are best off setting it to the same value as bWriteable unless you specifically KNOW
		// you don't want undo to work on the file (such as the .clw file).  bForceOpen is pretty much
		// ignored for files that you are NOT opening for write.
	STDMETHOD(GetTextEditor)(BOOL bWriteable, BOOL bForceOpen, BSTR bstrFilename, 
		ICodeMgrTextEditor** ppvTextEditor) PURE;
	STDMETHOD(get_LexerForLanguage)(short nParseLanguage, ICodeMgrLexer** ppvLexer) PURE;

	// project services
	STDMETHOD(get_ProjectName)(HPROJECT hProject, BSTR* pbstrProjName) PURE;
	STDMETHOD(get_SelectedProject)(HPROJECT* phProject) PURE;
	STDMETHOD(put_SelectedProject)(HPROJECT hProject) PURE;
	STDMETHOD(IsObjectInProject)(HPROJECT hProject, long nObjectType, short nParseLanguage, BSTR bstrObjectName, 
		long* pnActualType) PURE;
	STDMETHOD(get_ProjectUseOfMFC)(HPROJECT hProject, UseOfMFC* pUseOfMFC) PURE;
	STDMETHOD(get_ProjectIncludePath)(HPROJECT hProject, BSTR bstrTool, IDOStrings** ppvIncludePath) PURE;

	// services for specific objects
		// GotoDialogEditorLocation allows you to open the dialog editor given either the dialog ID
		// or the dialog class IINST
	STDMETHOD(GotoDialogEditorLocation)(HPROJECT hProject, IINST iinstClass, BSTR bstrDialogID) PURE;
	STDMETHOD(get_FileName)(IINST iinst, long nFileType, short nPathType, BSTR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(IINST iinst, long nFileType, short nPathType, IDOStrings** ppvFiles) PURE;
	STDMETHOD(GetDerivedClasses)(short nParseLanguage, HPROJECT hProject, BSTR bstrBaseClass, 
		BOOL bImmediateOnly, ICodeMgrBscItems** ppvClasses) PURE;
		// GetInclude: nFileType of dxWizProjectInclude gives you back the project include  
		// file (bstrClassName ignored).  nFileType of dxWizFileInclude is currently 
		// unimplemented.  All other values of nFileType will result in an attempt to find 
		// the header file where bstrClassName is defined.  If it is defined in an 
		// implementation file, we don't return any filename.
	STDMETHOD(GetInclude)(HPROJECT hProject, long nFileType, BSTR bstrClassName, BOOL bMakeRelative,  
		BSTR bstrRelativeWhere, BSTR* pbstrIncludeFile) PURE;
	STDMETHOD(BreakUpParameter)(IINST iinstParent, BSTR bstrFullParam, BOOL bDoErrorChecks,
		BSTR* pbstrParamType, BSTR* pbstParamName, BSTR* pbstrParamDefault, 
		BSTR* pbstrParamComments, BSTR* pbstrParamAttribs) PURE;

	// ActiveX associations
	//		Usage: 
	//			iinstToAssoc = the IINST you're trying to get the matching elements for
	//			nAssociationMajorType = the basic type of association you are trying to do
	//				dxActiveXMatch - want the elements that match via ActiveX association
	//				dxActiveXNone - (use only with GetAssociatedFunctions) want the elements
	//					that do NOT match any ActiveX association
	//			nAssociationMinorType = the type of object you want back for this association
	//				dxWizClass (use with IDL/ODL IINST) to get a collection of classes
	//				dxWizInterface (use with C++ IINST) to get a collection of interfaces/dispinterfaces
	//				dxWizCoclass (use with C++ IINST) to get a collection of coclasses
	//				dxWizLibrary (IINST optional) to get the library name or file
	//			nPresentHow = how you want the information presented (GetAssociationName only)
	//				dxWizName - want the name of the associated element
	//				dxWizFilePath* - want the name of the file the associated element is in
	//	For example, to determine what classes implement a particular interface, you would call
	//		GetAssociations(hProject, iinstIface, dxWizActiveXMatch, dxWizClass, &ppvAssoc)
	//		where hProject is the project your IINST is in (pass dxWizSelectedProject for 
	//		selected project), iinstIFace is the IINST of the interface, and ppvAssoc is the 
	//		ICodeMgrBscItems* object you want back.
	//	To determine what interfaces a particular class implements, you would call
	//		GetAssociations(hProject, iinstClass, dxWizActiveXMatch, dxWizInterface, &ppvAssoc)
	//		where the parameters mean the same thing as in the first example except that iinstClass
	//		is the IINST for the C++ class.
	STDMETHOD(HasAssociations)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, BOOL* pbHasAssoc) PURE;
	STDMETHOD(GetAssociations)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, ICodeMgrBscItems** ppvBscItems) PURE;
	STDMETHOD(GetAssociation)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, BSTR bstrNameToAssocTo, IBscPersistentItem** ppvBscItem, 
		long* pnType, BSTR* pbstrAttributes) PURE;
	STDMETHOD(GetAssociationName)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, long nPresentHow, long nAssociatedHow, BSTR* pbstrName) PURE;
	STDMETHOD(GetAssociatedFunctions)(IINST iinstClass, IINST iinstIFace, long nAssociationMajorType,
		ICodeMgrBscItems** ppvFunctions) PURE;
	STDMETHOD(IsAssociatedFunction)(IINST iinstFunction, IINST iinstIFace, long nAssociationMajorType) PURE;

	// transaction support services (don't require transaction to use)
	// for GetSingleNamedMember and GetMultipleNamedMembers, nParseLanguage and hProject are only an issue
	// if iinstParent is IINST_GLOBALS, otherwise they are ignored.
	STDMETHOD(GetSingleNamedMember)(IINST iinstParent, short nParseLanguage, HPROJECT hProject, long nMemberKind, 
		BSTR bstrMemberName, IINST* piiMember) PURE;
	STDMETHOD(GetMultipleNamedMembers)(IINST iinstParent, short nParseLanguage, HPROJECT hProject, long nMemberKind, 
		BSTR bstrMemberName, ICodeMgrBscItems** ppvItems) PURE;
	STDMETHOD(GetMemberConflicts)(IINST iinstClass, long nMemberKind, BSTR bstrMember, IINST* piinstMember) PURE;
	STDMETHOD(CreateFunctionStubBody)(IINST iinstClass, VARTYPE vt, BSTR* pbstrReturn) PURE;
	STDMETHOD(GetAutomationStyle)(IINST iinst, memberAutomationStyles* pmemAutoStyle) PURE;
	STDMETHOD(CreateTemplateDictionary)(IDOTemplateDictionary** ppvDictionary) PURE;
		// ParseName currently accepts two different types of items for parsing:
		//	dxWizName - pull out any keywords (like virtual, afx_msg, etc.)
		//	default - comment out any default settings
	STDMETHOD(ParseName)(short nParseLanguage, long nParseType, BSTR bstrParseString, BSTR* pbstrResponse) PURE;
	STDMETHOD(MakeRelative)(HPROJECT hProject, BSTR bstrOrigPath, BSTR bstrRelativeWhat, 
		BSTR* pbstrRelativePath) PURE;
		// FindGlobalInsertLocation is intended for finding nice, big insert locations like 
		// for some arbitrary text you want to place somewhere or for inserting something 
		// like an interface into an IDL file.  Hints for FindGlobalInsertLocation: 
		//	- dxWizFileInsertIface (just below the 'lowest' existing interface or just above the 'library' 
		//		statement)
		//	- dxWizFileInsertDispIface (just below the 'lowest' existing dispinterface or just below the
		//		'library' statement)
		//	- dxWizFileInsertCoclass (just below the 'lowest' existing coclass or just above the end of 
		//		the library block)
		//	- dxWizHintCodeStart (just below any leading #include directives), 
		//	- dxWizStart (just below any header wrapper)
		//	- default (at 'end' which is inside header wrappers if applicable)
		// the passed in default line and column will be used iff we can't figure out anything better
		//		based on the hint.  A line number of (ULONG)-1 means end of file.
		// pbstrTab is an optional parameter and is used to obtain the tab level at the line you
		//		want to insert at
	STDMETHOD(FindGlobalInsertLocation)(BSTR bstrFilename, long nHintType, ULONG nDefaultLine, 
		ULONG nDefaultColumn, ULONG* pnActualLine, ULONG* pnActualColumn, BSTR* pbstrTab) PURE;
		// FindScopedInsertLocation is intended for finding a spot to stick a member item, like 
		// a function into a class.  These same flags (by definition) are in effect for 
		// bInParentDefn = TRUE means we're looking for insertion point in parent definition,
		//	otherwise, we're looking for insertion point in parent implementation
		// C++ Hints:	
		//	nHintType			nHintName
		//	dxWizHintFileName	name of file to insert in
		//	dxWizClassWiz		name of ClassWiz block
		//	default				NULL		picks up access correct access block to use
		//	dxWizEnd	- OR with other flags to get the END of whatever block it is the element is 
		//					going to be placed at
		// WARNING: using FindScopedInsertLocation will NOT result in the appropriate access specifier
		//	being guaranteed to be in place if you then use a straight InsertText at that line to add
		//	your element.
		// IDL/ODL Hints:
		//	nHintType			nHintName
		//	dxWizHintLineNumber	line number to insert at
		//	dxWizFunction		add before or after the specified function
		//	dxWizVariable		add before or after the specified variable
		//	default				attribute name	- place the new element before or after the element with 
		//											the specified attribute (lets us put things in id order)
		//	dxWizEnd	- OR with other flags to specify you want to add after that spot (absence means add before)
		// pbstrTab is an optional parameter and is used to obtain the tab level at the line you
		//		want to insert at
	STDMETHOD(FindScopedInsertLocation)(IINST iinst, long nMemberKind, short nAccessType, BOOL bInParentDefn, 
		long nHintType, BSTR bstrHintName, BOOL bMemberAddAfter, BSTR* pbstrFilename, ULONG* pnLine, 
		ULONG* pnColumn, BSTR* pbstrTab) PURE;
	STDMETHOD(IsValidSymbol)(HPROJECT hProject, short nParseLanguage, long nObjectType, BSTR bstrName, 
		BOOL* pbIsValidSymbol) PURE;
	STDMETHOD(IsReservedSymbol)(HPROJECT hProject, short nParseLanguage, long nObjectType, BSTR bstrName, 
		SymbolConflictType* pnSymbolType) PURE;

	// Transaction Manager
	STDMETHOD(StartTransaction)(IDOTransaction_Internal** ppvTransaction) PURE;
	STDMETHOD(get_Transaction)(IDOTransaction** ppvTransaction) PURE;	// can be NULL
	STDMETHOD(get_InternalTransaction)(IDOTransaction_Internal** ppvTransaction) PURE;	// can be NULL
	STDMETHOD(AddTransactionParticipant)(IDOTransactionParticipant* pParticipant) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IDOTransaction
//   This interface handles transaction management.
//	 'begin transaction' is implicit in creating one of these.
//
// See IDOTransactionParticipant for details on transaction chronology

#undef  INTERFACE
#define INTERFACE IDOTransaction
DECLARE_INTERFACE_(IDOTransaction, IDispatch)
{
	// generic methods
	STDMETHOD(CommitTransaction)(BOOL bSave) PURE;
	STDMETHOD(AbortTransaction)() PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IDOTransaction_Internal
//   This interface is used to modify any generic IINST-based object.  In addition to 
//   acting just like an IDOTransaction, it allows access to the code modification 
//   infrastructure.  See ICodeManager for more details on methods that are also available
//	 there.
//
// See IDOTransactionParticipant for details on transaction chronology

#undef  INTERFACE
#define INTERFACE IDOTransaction_Internal
DECLARE_INTERFACE_(IDOTransaction_Internal, IUnknown)
{
	// standard transaction methods
	STDMETHOD(CommitTransaction)(BOOL bSave) PURE;
	STDMETHOD(AbortTransaction)() PURE;

	// general services
	STDMETHOD(GetHelper)(REFIID riidInterface, IUnknown** ppvInterface) PURE;
	STDMETHOD(GetCodeManagerLocation)(BSTR* pbstrCodeMgrLoc) PURE;
	STDMETHOD(GetTextEditor)(BOOL bWriteable, BOOL bForceOpen, BSTR bstrFilename, 
		ICodeMgrTextEditor** ppvTextEditor) PURE;
	STDMETHOD(get_LexerForLanguage)(short nParseLanguage, ICodeMgrLexer** ppvLexer) PURE;

	// project services
	STDMETHOD(get_ProjectName)(HPROJECT hProject, BSTR* pbstrProjName) PURE;
	STDMETHOD(get_SelectedProject)(HPROJECT* phProject) PURE;
	STDMETHOD(put_SelectedProject)(HPROJECT hProject) PURE;
	STDMETHOD(IsObjectInProject)(HPROJECT hProject, long nObjectType, short nParseLanguage, 
		BSTR bstrObjectName, long* pnActualType) PURE;

	// Transaction Manager
	STDMETHOD(AddTransactionParticipant)(IDOTransactionParticipant* pParticipant) PURE;
	STDMETHOD(get_TempResourceDO)(HPROJECT hProject, ITransMgrResourceHelper** ppvHelper) PURE;
	STDMETHOD(get_TempProjSystemDO)(HPROJECT hProject, ITransMgrProjHelper** ppvHelper) PURE;

	// read-only services for specific objects
	STDMETHOD(get_FileName)(IINST iinst, long nFileType, short nPathType, BSTR* pbstrFileName) PURE;
	STDMETHOD(get_AllFileNames)(IINST iinst, long nFileType, short nPathType, IDOStrings** ppvFiles) PURE;
	STDMETHOD(GetDerivedClasses)(short nParseLanguage, HPROJECT hProject, BSTR bstrBaseClass, 
		BOOL bImmediateOnly, ICodeMgrBscItems** ppvClasses) PURE;
	STDMETHOD(GetInclude)(HPROJECT hProject, long nFileType, BSTR bstrClassName, BOOL bMakeRelative,  
		BSTR bstrRelativeWhere, BSTR* pbstrIncludeFile) PURE;
	STDMETHOD(BreakUpParameter)(IINST iinstParent, BSTR bstrFullParam, BOOL bDoErrorChecks,
		BSTR* pbstrParamType, BSTR* pbstParamName, BSTR* pbstrParamDefault, 
		BSTR* pbstrParamComments, BSTR* pbstrParamAttribs) PURE;

	// transaction support services (don't require transaction to use)
	STDMETHOD(GetSingleNamedMember)(IINST iinstParent, short nParseLanguage, HPROJECT hProject, long nMemberKind, 
		BSTR bstrMemberName, IINST* piiMember) PURE;
	STDMETHOD(GetMultipleNamedMembers)(IINST iinstParent, short nParseLanguage, HPROJECT hProject, long nMemberKind, 
		BSTR bstrMemberName, ICodeMgrBscItems** ppvItems) PURE;
	STDMETHOD(GetMemberConflicts)(IINST iinstClass, long nMemberKind, BSTR bstrMember, IINST* piinstMember) PURE;
	STDMETHOD(CreateFunctionStubBody)(IINST iinstClass, VARTYPE vt, BSTR* pbstrReturn) PURE;
	STDMETHOD(GetAutomationStyle)(IINST iinst, memberAutomationStyles* pmemAutoStyle) PURE;
	STDMETHOD(CreateTemplateDictionary)(IDOTemplateDictionary** ppvDictionary) PURE;
	STDMETHOD(ParseName)(short nParseLanguage, long nParseType, BSTR bstrParseString, BSTR* pbstrResponse) PURE;
	STDMETHOD(MakeRelative)(HPROJECT hProject, BSTR bstrOrigPath, BSTR bstrRelativeWhat, 
		BSTR* pbstrRelativePath) PURE;
	STDMETHOD(FindGlobalInsertLocation)(BSTR bstrFilename, long nHintType, ULONG nDefaultLine, 
		ULONG nDefaultColumn, ULONG* pnActualLine, ULONG* pnActualColumn, BSTR* pbstrTab) PURE;
	STDMETHOD(FindScopedInsertLocation)(IINST iinst, long nMemberKind, short nAccessType, BOOL bInParentDefn, 
		long nHintType, BSTR bstrHintName, BOOL bMemberAddAfter, BSTR* pbstrFilename, ULONG* pnLine, 
		ULONG* pnColumn, BSTR* pbstrTab) PURE;
	STDMETHOD(IsValidSymbol)(HPROJECT hProject, short nParseLanguage, long nObjectType, BSTR bstrName, 
		BOOL* pbIsValidSymbol) PURE;
	STDMETHOD(IsReservedSymbol)(HPROJECT hProject, short nParseLanguage, long nObjectType, BSTR bstrName, 
		SymbolConflictType* pnSymbolType) PURE;

	// generic project modification services
		// AddInclude: 
		//	- nAddWhere lets you set the basic location for where the #include is going to be added 
		//		dxWizProjectInclude - add to the project include file (C++)
		//		dxWizFileInclude - add to the file specified in bstrClassName (C++)
		//		dxWizFileScope - add near top of file (IDL/ODL)
		//		dxWizLibraryScope - add near top of library (IDL/ODL)
		//		dxWizObjectScope - add inside object specified by bstrClassName
		//		dxWizImport - adding an import 
		//		dxWizInclude - adding an include (default for C++)
		//		dxWizImportlib - adding an importlib (IDL/ODL only)
		//		Default is to add to the file indicated by nHintType 
		//	- nHintType 
		//		dxWizDefinition, dxWizDeclaration, dxWizMemberDefinitions, dxWizMemberDeclarations
		//			used in conjunction with default nAddWhere and given the class name in 
		//			bstrClassName
		//		dxWizFilePathRelative - make the path in bstrIncludeName relative to the file
		//			we're adding the directive to
	STDMETHOD(AddInclude)(short nParseLanguage, HPROJECT hProject, long nAddWhere, BSTR bstrClassName, 
		BSTR bstrIncludeName, BSTR bstrAttribs, BSTR bstrConditions, BSTR bstrComments, long nHintType, 
		BSTR bstrHintName, BOOL bAddAfter) PURE;

	// generic IINST-based modification services. 
	STDMETHOD(put_Name)(IINST iinst, BSTR bstrName) PURE;
	STDMETHOD(put_Contents)(IINST iinst, BSTR bstrContents) PURE;
	// shortcut for deleting all attributes on an object is to call put_AllAttributes with bstrAttribs = NULL
	STDMETHOD(put_AllAttributes)(IINST iinstParent, BSTR bstrAttribs) PURE;
	// Hints for AddAttribute:
	//	dxWizDuplicateOK - don't check if this attribute is already present
	//	dxWizEnd - add it to the end of the existing attribute list, if any
	//	dxWizHintIndex - add it at the index specified in bstrHintName (use bAddAfter to designate
	//		before or after); zero-based count
	STDMETHOD(AddAttribute)(IINST iinst, BSTR bstrAttributeName, BSTR bstrAttributeValue, 
		BSTR bstrAttributeComment, long nHintType, BSTR bstrHintName, BOOL bAddAfter) PURE;
	STDMETHOD(put_AccessType)(IINST iinst, short nAccessType) PURE;
	STDMETHOD(GetOneLine)(IINST iinst, long nFileType, long nLineNumber, BSTR* bstrLineContents) PURE;
	STDMETHOD(SetOneLine)(IINST iinst, long nFileType, long nLineNumber, BSTR bstrText) PURE;
	// Usage of elements of InsertText: 
	//	iinst - elemement you're trying to modify
	//	nFileType - dxWizDefinition, dxWizDeclaration, dxWizMemberDefinitions, dxWizMemberDeclarations
	//		specifies which file associated with the IINST to use
	//	nLineNumber
	//		dxWizDefEndLine - last line of the definition of the iinst element
	//		dxWizDefBeginLine - first line of the definition of the iinst element
	//		dxWizModEndLine - last line of the file specified using iinst and nFileType
	//		default - actual line to use
	//	nColumn
	//		dxWizEndOfLine - after last column on specified line
	//		default - actual column location
	STDMETHOD(InsertText)(IINST iinst, long nFileType, BSTR bstrText, long nLineNumber, long nColumn, 
		BOOL bFullLine) PURE;
	// nWhichPart for Delete: (all can be OR'ed together)
	//	dxWizDefinition - object definition
	//	dxWizDeclaration - object declaration 
	//	dxWizMemberDefinitions - member definitions (class only)
	//	dxWizMemberDeclarations - same as dxWizDefinition for a class
	// You'll need to OR together whatever parts you want hard deleted in nCommentOrDeletePart
	//	or it will just be commented out
	STDMETHOD(Delete)(IINST iinst, short nWhichPart, short nCommentOrDeletePart) PURE;
	STDMETHOD(put_Type)(IINST iinst, BSTR bstrType) PURE;
	// Hints for put_Comment
	//	dxWizDeclaration - put comment in declaration file
	//	dxWizDefinition - put comment in definition file
	//	dxWizEnd - make the comment an end-of-line comment
	//	dxWizSameLoc - use the same location previously used if we're replacing an existing comment
	STDMETHOD(put_Comment)(IINST iinst, BSTR bstrComment, long nHintType, BSTR bstrHintName) PURE;

	// IINST-based modification services that require that the IINST be something that has members, 
	// like a class
	// NOTE: AddMember and AddMemberBody only pay attention to nParseLanguage if iinstClass is for
	// the global class.  Otherwise, that parameter will be ignored
		// Hints shared by AddMember, AddMemberBody, AddBaseClass, and AddItem
		//	dxWizDoIndents - put the element at the same tabbing level as what's present near its
		//		insertion point (one tab further in if at beginning or end or just after an access
		//		specifier) with all \r\n pairs in the body supplemented with the new tab level
		//		and follow the whole thing with an extra \r\n pair
		//	dxWizHintCOM - the name is a COM thing (STDMETHOD, STDMETHOD_) and needs to be munged
		//		appropriately (AddMember, AddMemberBody only)
		//	See also FindScopedInsertLocation for further hints available for AddMember and 
		//		FindGlobalInsertLocation for further hints available for AddMemberBody
	STDMETHOD(AddMember)(IINST iinstClass, short nParseLanguage, long nMemberKind, BSTR bstrMemberName, 
		BSTR bstrMemberType, short nAccessType, BSTR bstrAttributes, BSTR bstrComments, BSTR bstrConditions, 
		BSTR bstrBody, long nHintType, BSTR bstrHintName, BOOL bMemberAddAfter) PURE;
		// AddMember: pass a NULL for bstrBody if you want a declaration, not an in-line, for a function
	STDMETHOD(AddMemberBody)(IINST iinstClass, short nParseLanguage, long nMemberKind, BSTR bstrMemberName, 
		BSTR bstrMemberType, BSTR bstrComments, BSTR bstrMemberBody, BSTR bstrConditions, long nHintType, 
		BSTR bstrHintName, BOOL bMemberAddAfter) PURE;
	STDMETHOD(AddBaseClass)(IINST iinstClass, BSTR bstrBaseClass, short nAccessType, long nHintType, 
		BSTR bstrHintName) PURE;

	// IINST-based modification services that require that the IINST be something that 
	// takes parameters, like a function
		// SetParameters: nWhichPart allows you to set the parameters separately for the 
		// definition and declaration if you choose
	STDMETHOD(SetParameters)(IINST iinstObject, short nWhichPart, BSTR bstrParameters) PURE;
	STDMETHOD(AddParameter)(IINST iinstObject, short nParamNum, BOOL bAddAfter, BSTR bstrNewParam,
		BSTR bstrNewParamAttributes, BSTR bstrNewParamComments) PURE;
	STDMETHOD(put_ParameterName)(IINST iinstObject, short nParamNum, BSTR bstrName) PURE;
	STDMETHOD(put_ParameterValue)(IINST iinstObject, short nParamNum, BSTR bstrValue) PURE;
	STDMETHOD(DeleteParameter)(IINST iinstObject, short nParamNum, BOOL bHardDelete) PURE;
	STDMETHOD(put_ParameterType)(IINST iinstObject, short nParamNum, BSTR bstrType) PURE;
	// put_ParameterComment - see put_Comments for list of options for hints.
	STDMETHOD(put_ParameterComment)(IINST iinstObject, short nParamNum, BSTR bstrComment, long nHintType, 
		BSTR bstrHintName) PURE;
	STDMETHOD(put_ParameterAttributes)(IINST iinstObject, short nParamNum, BSTR bstrAttribs) PURE;
	// AddParameterAttribute: see AddParameter for list of options for hints.  Note also that
	//	this is only available for IDL/ODL
	STDMETHOD(AddParameterAttribute)(IINST iinstObject, short nParamNum, BSTR bstrAttributeName, 
		BSTR bstrAttributeValue, BSTR bstrAttributeComment, long nHintType, BSTR bstrHintName, 
		BOOL bAddAfter) PURE;
	STDMETHOD(DeleteParameterAttribute)(IINST iinstObject, short nParamNum, BSTR bstrAttributeName,
		BOOL bHardDelete) PURE;
	STDMETHOD(put_ParameterAttribute)(IINST iinstObject, short nParamNum, BSTR bstrOldAttributeName,
		BSTR bstrAttributeName, BSTR bstrAttributeValue, BSTR bstrAttributeComment) PURE;

	// IINST-based modification services for adding elements to a map or enum.  
	// bstrValue only has meaning for adding an enum item
	//	Hints available for AddItem:
	//		dxWizClassWiz - place inside the ClassWiz block designated by bstrHintName (ex: MSG_MAP)
	//		dxWizEnd - place at end of whatever (end of ClassWiz block if that is also designated)
	//		dxWizHintIndex - place at the index designated in bstrHintName (use bAddAfter to designate
	//			before or after that index); zero-based count
	STDMETHOD(AddItem)(IINST iinstObject, BSTR bstrItem, BSTR bstrValue, BSTR bstrComments, long nHintType, 
		BSTR bstrHintName, BOOL bAddAfter) PURE;

	STDMETHOD(GetLastFileName)(BSTR *pbstrFileName) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ITransMgrResourceHelper
//   This interface is a temporary stand-in for resource DOs.
//   For 6X, this provides us with a limited ability to do transacted resource manipulation.

#undef  INTERFACE
#define INTERFACE ITransMgrResourceHelper
DECLARE_INTERFACE_(ITransMgrResourceHelper, IUnknown)
{	
	STDMETHOD(CreateResourceTable)(UINT nCreateFlags) PURE;
	STDMETHOD(GetResourceInclude)(BSTR* pbstrFilename) PURE;
	STDMETHOD(AttachResourceStream)(LPSTREAM lpStream) PURE;
	STDMETHOD(CreateResourceSymbol)(BSTR bstrID, BSTR* pbstrActualID, int* pnIDValue) PURE;
	STDMETHOD(CreateResource)(BSTR bstrType, BSTR bstrID, int nLangID, BSTR bstrCondition, 
		BSTR bstrNewID) PURE;
	STDMETHOD(ImportResource)(BSTR bstrType, BSTR bstrID, BSTR bstrFileName, int nLangID, 
		BSTR bstrCondition) PURE;
	STDMETHOD(SetResourceID)(BSTR bstrType, BSTR bstrOldID, int nLangID, BSTR bstrCondition, 
		BSTR bstrNewID) PURE;
	STDMETHOD(CreateResourceString)(BSTR bstrID, int nLangID, BSTR bstrCondition, BSTR bstrString) PURE;
	STDMETHOD(ResourceAccess)(UINT nMode) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ITransMgrProjHelper
//   This interface is a temporary stand-in for Proj/Build DOs
//   For 6X, this provides us with the ability to transact at least the addition
//   of new files to a project.

#undef  INTERFACE
#define INTERFACE ITransMgrProjHelper
DECLARE_INTERFACE_(ITransMgrProjHelper, IUnknown)
{
	STDMETHOD(AddFile)(BSTR bstrFilename) PURE;
	STDMETHOD(CalculateFilename)(BSTR bstrFilename, BSTR* pbstrNewFilename, long* pnHint) PURE;
	STDMETHOD(IsFileOnAddList)(BSTR bstrFilename, BOOL* pbIsOnList) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// IDOTransactionParticipant
//   This interface handles the various elements that make up a transaction.
//   For 6X, these elements will consist of:
//   - the text editor (ICodeMgrTextEditor, IDOExpandTemplates),
//   - the resource editor (ITransMgrResourceHelper)
//   - the Proj/Build system (ITransMgrProjHelper)
//   It is possible to add additional elements (including specific DOs) to the
//   list by adding one of these elements via 
//   ICodeManager::AddTransactionParticipant
//
//   Chronology of a transaction:
//   - Wizard starts a transaction
//   - Wizard does something to modify a project, ex: add a file, add a member, delete 
//       something, add a string to resources, etc.
//   - As each kind of project modifier comes on line (text edits, resource edits, etc.), 
//       it adds itself as a transaction participant
//   - Wizard commits the transaction
//   - The Code Manager calls CanCommit for each IDOTransactionParticipant object it has.  
//       A failure return from any participant results in aborting the transaction
//   - The Code Manager calls Commit on each participant three times, specifying which round 
//       is involved.  Any given participant should only do work at this point once.  This 
//       is done so that we don't have order dependence between the participants.  
//       Participants should maintain any required state to be aborted throughout this 
//       process.
//       - Round 1: Anybody who uses other transaction services but doesn't provide any 
//           (ex: IClassDO)
//       - Round 2: Anybody who both uses and provides transaction services 
//           (ex, sort of: IDOExpandTemplates)
//       - Round 3: Anybody who only provides transaction services 
//           (ex: ICodeMgrTextEditor)
//   - The Code Manager calls AfterCommit on each participant.  It is now safe to throw 
//       away any maintained state information.
//
//   For a nested transaction, repeat the same scenario above, EXCEPT use the 
//   appropriate methods that apply to nested transactions AND only state information 
//   related to the intermediate state of the transaction can be thrown away.  Abort can 
//   be called on the full transaction at any time except after AfterCommit is called on 
//   the participants.

#undef  INTERFACE
#define INTERFACE IDOTransactionParticipant
DECLARE_INTERFACE_(IDOTransactionParticipant, IUnknown)
{
	// IDOTransactionParticipant methods
	STDMETHOD(InitialTransactionLevel)(int nStartingLevel) PURE;	
		// error out on InitialTransactionLevel() to abort adding yourself to list

	// these methods apply to nested levels within a transaction
	STDMETHOD(AddTransactionLevel)() PURE;
	STDMETHOD(CommitTransactionLevel)(int nRoundNumber, BOOL bSave) PURE;
	STDMETHOD(CanCommitTransactionLevel)() PURE;
	STDMETHOD(AfterCommitTransactionLevel)() PURE;
	STDMETHOD(AbortTransactionLevel)() PURE;	// return value ignored
	STDMETHOD(AfterAbortTransactionLevel)() PURE;	// return value ignored

	// these methods are called on final commit/abort of a transaction
	STDMETHOD(CommitTransaction)(int nRoundNumber, BOOL bSave) PURE;
	STDMETHOD(CanCommitTransaction)() PURE;
	STDMETHOD(AfterCommitTransaction)() PURE;
	STDMETHOD(AbortTransaction)() PURE;	// return value ignored
	STDMETHOD(AfterAbortTransaction)() PURE;	// return value ignored
};

/////////////////////////////////////////////////////////////////////////////
// ICodeMgrBscItems
//   This interface provides access to generic lists of code manager IINST data.
//   The IEnumVARIANT values here are IBscPersistentItem objects.

#undef  INTERFACE
#define INTERFACE ICodeMgrBscItems
DECLARE_INTERFACE_(ICodeMgrBscItems, IDispatch)
{
	// required collection method
	STDMETHOD(_NewEnum)(IEnumVARIANT **ppenum) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ICodeMgrTextEditor
//	This interface provides access to the code manager's transacted text editor.
//	It is NOT intended for use by the 'old' style Wizard Model wizards.

#undef  INTERFACE
#define INTERFACE ICodeMgrTextEditor
DECLARE_INTERFACE_(ICodeMgrTextEditor, IUnknown)
{	
	// GetLineText: used with lexer, etc.; not intended for wide scale use.  More than one line can be
	//	returned here.  Call ICodeManager::GetLineText if all you really want is the single line of text
	STDMETHOD(GetLineText)(ULONG iLine, BSTR* pbstr, ULONG* pcb, BOOL bStripEOL) PURE;
	STDMETHOD(GetLineCount)(ULONG* piLines) PURE;
	STDMETHOD(GetFileName)(BSTR* pbstrFileName) PURE;
	STDMETHOD(GetTextSection)(ULONG iStartLine, ULONG iStartColumn, ULONG iEndLine, ULONG iEndColumn,
		BSTR* pbstrText) PURE;
	STDMETHOD(FindToken)(ULONG uLineNumber, BSTR bstrToken, BOOL bFuzzyMatch, ULONG* puLineHit, 
		ULONG* puColumnHit) PURE;
	STDMETHOD(FindAnsiToken)(ULONG uLineNumber, LPCTSTR lpszToken, BOOL bFuzzyMatch, ULONG* puLineHit, 
		ULONG* puColumnHit) PURE;
	STDMETHOD(InsertText)(BSTR bstrText, ULONG uline, ULONG uColumn) PURE;
	STDMETHOD(InsertAnsiText)(LPCTSTR pszText, ULONG uline, ULONG uColumn) PURE;
	STDMETHOD(DeleteText)(ULONG uStartLine, ULONG uStartCol, ULONG uEndLine, ULONG uEndCol) PURE;
	STDMETHOD(ChangeText)(ULONG uStartLine, ULONG uStartCol, ULONG uEndLine, ULONG uEndCol, 
		BSTR bstrText) PURE;
	STDMETHOD(ChangeAnsiText)(ULONG uStartLine, ULONG uStartCol, ULONG uEndLine, ULONG uEndCol, 
		LPCTSTR pszText) PURE;
	// [bstr/psz]FileContent is the name of the file we're getting the text from and
	// uLine/uColumn specifies where to put it in the file described by this particular
	// ICodeMgrTextEditor object
	STDMETHOD(InsertFileContent)(BSTR bstrFileContent, ULONG uLine, ULONG uColumn) PURE;
	STDMETHOD(InsertAnsiFileContent)(LPCTSTR pszFileContent, ULONG uLine, ULONG uColumn) PURE;

	STDMETHOD(get_Lexer)(ICodeMgrLexer** ppvLexer) PURE;
	// the command list is only useful if you care about what is already being done to the
	// same file you're working with, so this may actually only be useful inside the Code Manager
	// and not useful at all to DOs.
	STDMETHOD(get_CommandList)(IEnumVARIANT** ppvCommands) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ICodeMgrLexer
//   This interface is used for private communication with the wiz model text lexer
//   Getting one of these from ICodeMgrTextEditor ensures that you've got the right lexer
//   for a particular file.  If you have a text buffer you want to check out, pass
//   the right language symbol (dxWizCPP or dxWizIDL) to the version of get_LexerForLanguage

#undef  INTERFACE
#define INTERFACE ICodeMgrLexer
DECLARE_INTERFACE_(ICodeMgrLexer, IUnknown)
{
	STDMETHOD(InitializeLexer)() PURE;
	STDMETHOD(LexLine)(BSTR bstrLine, UINT cbLine) PURE;
	STDMETHOD(GetTokens)(long* plCount, TXTB** prgTokens) PURE;
	STDMETHOD(ClearLexer)() PURE;
	STDMETHOD(RemoveClassName)(BSTR bstrOriginalDecl, BSTR* pbstrNewDecl, BSTR* pbstrClass) PURE;
	STDMETHOD(AddClassName)(BSTR bstrOriginalDecl, BSTR bstrClass, BSTR* pbstrNewDecl) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ICodeMgrTextEditorCommand
//   This interface is used to describe an individual command for the text editor 

#define dxActionAdd		0x1		// adding text
#define dxActionDelete	0x2		// deleting text
#define dxActionReplace	0x4		// replacing text

#undef  INTERFACE
#define INTERFACE ICodeMgrTextEditorCommand
DECLARE_INTERFACE_(ICodeMgrTextEditorCommand, IUnknown)
{
	STDMETHOD(get_Action)(long* pnActionType) PURE;	// the type of action
	STDMETHOD(get_Text)(BSTR* pbstrText) PURE;	// the text we're putting in, if any
	STDMETHOD(put_Text)(BSTR bstrText) PURE;			// replacement for the intended text
	STDMETHOD(put_AnsiText)(LPCTSTR pszText) PURE;			// replacement for the intended text
	STDMETHOD(get_StartLine)(long* pnStartLine) PURE;
	STDMETHOD(put_StartLine)(long nStartLine) PURE;	// use CAREFULLY
	STDMETHOD(get_StartColumn)(long* pnStartColumn) PURE;
	STDMETHOD(put_StartColumn)(long nStartColumn) PURE;	// use CAREFULLY
	STDMETHOD(get_EndLine)(long* pnEndLine) PURE;		// not valid for dxActionAdd
	STDMETHOD(put_EndLine)(long nEndLine) PURE;	// use CAREFULLY to extend selection for delete/replace
	STDMETHOD(get_EndColumn)(long* pnEndColumn) PURE;	// not valid for dxActionAdd
	STDMETHOD(put_EndColumn)(long nEndColumn) PURE;	// use CAREFULLY to extend selection for delete/replace
};

#endif	// _CODEMGRAPI_