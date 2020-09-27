#ifndef __CODEMGRSUPPORTAPI__
#define __CODEMGRSUPPORTAPI__

// These interfaces are intended for internal use inside the Code Manager or for 
// ClassView's direct benefit (there are MFC parameters on some methods, fer instance).

interface ICodeManagerMenuBuilder;	// menu builder for ClassView

interface ICodeManagerSupport_Internal;	// private interface used by the code manager to talk to the 
										// language packages

class VCDesignElementI;

// nExtra parameter constants to methods within ICodeManagerMenuBuilder
// NOTE: keep these in sync with the elements of the same names in WizRsrv.h
#ifndef COM_MAP_NONE
#define COM_MAP_NONE         0
#define COM_MAP_IMPL         1
#define COM_MAP_IMPL_PROP    2
#endif

/////////////////////////////////////////////////////////////////////////////
// ICodeManagerMenuBuilder
//   This interface is used to build menus for the DO-based ClassView
//   It is non-standard COM to fit with the MSDEV architecture for context menus.  The interface 
//   will need to change dramatically (or go away?) when we move to the new shell.

#undef  INTERFACE
#define INTERFACE ICodeManagerMenuBuilder
DECLARE_INTERFACE_(ICodeManagerMenuBuilder, IUnknown)
{
	STDMETHOD(GetCmdIDs)(VCDesignElementI* pDO, CWordArray& aVerbs, CWordArray& aCmds, int nExtra) PURE;
	STDMETHOD(OnCmdMsg)(VCDesignElementI* pDO, UINT nID, int nCode, void* pExtra, LPBOOL lpRetVal, 
		CWnd* slobWnd, int nExtra) PURE;
};

/////////////////////////////////////////////////////////////////////////////
// ICodeManagerSupport_Internal
//   This interface is private to within the code manager and shouldn't be used by
//   code manager clients.  
//   Pay no attention to the man behind the curtain.  :-)

#undef  INTERFACE
#define INTERFACE ICodeManagerSupport_Internal
DECLARE_INTERFACE_(ICodeManagerSupport_Internal, IUnknown)
{
	// basic communication between the parts of the Code Manager
	STDMETHOD(put_MainCodeManager)(ICodeManager* pvCodeManager) PURE;
	STDMETHOD(put_AlternativeCodeManagerSupport)(ICodeManagerSupport_Internal* pvAlternative) PURE;

	// general services
	STDMETHOD(get_Lexer)(LPCTSTR lpszFilename, ICodeMgrLexer** ppvLexer, BOOL* pbHandled) PURE;
	STDMETHOD(get_LexerForLanguage)(short nParseLanguage, ICodeMgrLexer** ppvLexer, BOOL* pbHandled) PURE;
	STDMETHOD(put_SelectedProject)(HPROJECT hProject) PURE;

	// services for specific objects
	STDMETHOD(GotoDialogEditorLocation)(HPROJECT hProject, IINST iinstClass, LPCTSTR lpszDialogID, 
		BOOL* pbHandled) PURE;
	STDMETHOD(get_FileName)(IINST iinst, long nFileType, short nPathType, BSTR* pbstrFileName, 
		BOOL* pbHandled) PURE;
	STDMETHOD(get_AllFileNames)(IINST iinst, long nFileType, short nPathType, IDOStrings** ppvFiles,
		BOOL* pbHandled) PURE;
	STDMETHOD(GetDerivedClasses)(short nParseLanguage, HPROJECT hProject, LPCTSTR lpszBaseClass, 
		BOOL bImmediateOnly, ICodeMgrBscItems** ppvClasses, BOOL* pbHandled) PURE;
	STDMETHOD(GetInclude)(HPROJECT hProject, long nFileType, LPCTSTR lpszClassName, BOOL bMakeRelative,  
		LPCTSTR lpszRelativeWhere, BSTR* pbstrIncludeFile, BOOL* pbHandled) PURE;
	STDMETHOD(BreakUpParameter)(IINST iinstParent, LPCTSTR lpszFullParam, BOOL bDoErrorChecks,
		BSTR* pbstrParamType, BSTR* pbstParamName, BSTR* pbstrParamDefault, BSTR* pbstrParamComments, 
		BSTR* pbstrParamAttribs, BOOL* pbHandled) PURE;

	// ActiveX associations
	STDMETHOD(HasAssociations)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, BOOL* bHasAssoc, BOOL* pbHandled) PURE;
	STDMETHOD(GetAssociations)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, ICodeMgrBscItems** ppvBscItems, BOOL* pbHandled) PURE;
	STDMETHOD(GetAssociation)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, LPCTSTR lpszNameToAssocTo, IBscPersistentItem** ppvBscItem, 
		long* pnType, BSTR* pbstrAttributes, BOOL* pbHandled) PURE;
	STDMETHOD(GetAssociationName)(HPROJECT hProject, IINST iinstToAssoc, long nAssociationMajorType, 
		long nAssociationMinorType, long nPresentHow, long nAssociatedHow, BSTR* pbstrName, 
		BOOL* pbHandled) PURE;
	STDMETHOD(GetAssociatedFunctions)(IINST iinstClass, IINST iinstIFace, long nAssociationMajorType,
		ICodeMgrBscItems** ppvFunctions, BOOL* pbHandled) PURE;
	STDMETHOD(IsAssociatedFunction)(IINST iinstFunction, IINST iinstIFace, long nAssociationMajorType, BOOL* pbHandled) PURE;

	// basic transaction notifications
	STDMETHOD(NotifyStartTransaction)() PURE;
	STDMETHOD(NotifyBeforeCommitTransaction)() PURE;
	STDMETHOD(NotifyAfterCommitTransaction)() PURE;
	STDMETHOD(NotifyBeforeAbortTransaction)() PURE;
	STDMETHOD(NotifyAfterAbortTransaction)() PURE;

	// transaction support services (don't require transaction to use)
	STDMETHOD(GetSingleNamedMember)(IINST iinstParent, short nParseLanguage, HPROJECT hProject, long nMemberKind, 
		LPCTSTR lpszMemberName, IINST* piiMember, BOOL* pbHandled) PURE;
	STDMETHOD(GetMultipleNamedMembers)(IINST iinstParent, short nParseLanguage, HPROJECT hProject, long nMemberKind, 
		LPCTSTR lpszMemberName, ICodeMgrBscItems** ppvItems, BOOL* pbHandled) PURE;
	STDMETHOD(GetMemberConflicts)(IINST iinstClass, long nMemberKind, LPCTSTR lpszMembere, IINST* piinstMember, 
		BOOL* pbHandled) PURE;
	STDMETHOD(CreateFunctionStubBody)(IINST iinstClass, VARTYPE vt, BSTR* pbstrReturn, BOOL* pbHandled) PURE;
	STDMETHOD(GetAutomationStyle)(IINST iinst, memberAutomationStyles* pmemAutoStyle, BOOL* pbHandled) PURE;
	STDMETHOD(ParseName)(short nParseLanguage, long nParseType, LPCTSTR lpszParseString, BSTR* pbstrResponse,
		BOOL* pbHandled) PURE;
	STDMETHOD(FindGlobalInsertLocation)(long nHint, ICodeMgrTextEditor* pEditor, LPCSTR pszFileName, 
		ULONG* puActualLine, ULONG* puActualColumn, BSTR* pbstrTab, BOOL* pbHandled) PURE;
	STDMETHOD(FindScopedInsertLocation)(IINST iinst, long nMemberKind, short nAccessType, BOOL bInParentDefn, 
		long nHintType, LPCTSTR lpszHintName, BOOL bMemberAddAfter, BSTR* pbstrFilename, ULONG* pnLine, 
		ULONG* pnColumn, BSTR* pbstrTab, BOOL* pbHandled) PURE;
	STDMETHOD(IsValidSymbol)(HPROJECT hProject, short nParseLanguage, long nObjectType, LPCTSTR lpszName, 
		BOOL* pbIsValidSymbol, BOOL* pbHandled) PURE;
	STDMETHOD(IsReservedSymbol)(HPROJECT hProject, short nParseLanguage, long nObjectType, LPCTSTR lpszName, 
		SymbolConflictType* pnSymbolType, BOOL* pbHandled) PURE;
	STDMETHOD(FindAnsiToken)(ICodeMgrTextEditor* pEditor, LPCTSTR lpszFilename, ULONG uLineStart, 
		LPCTSTR lpszToken, BOOL bFuzzyMatch, ULONG* puLineHit, ULONG* puColumnHit, 
		BOOL* pbHandled) PURE;

	// transaction support services for files
	STDMETHOD(AddFile)(HPROJECT hProject, LPCTSTR lpszFilename, BOOL* pbHandled) PURE;

	// generic project modification services
	STDMETHOD(AddInclude)(short nParseLanguage, HPROJECT hProject, long nAddWhere, LPCTSTR lpszClassName, 
		LPCTSTR lpszIncludeName, LPCTSTR lpszAttribs, LPCTSTR lpszConditions, LPCTSTR lpszComments, long nHintType, 
		LPCTSTR lpszHintName, BOOL bAddAfter, BOOL* pbHandled) PURE;

	// generic IINST-based modification services
	STDMETHOD(put_Name)(IINST iinst, LPCTSTR lpszName, BOOL* pbHandled) PURE;
	STDMETHOD(put_Contents)(IINST iinst, LPCTSTR lpszContents, BOOL* pbHandled) PURE;
	STDMETHOD(put_AllAttributes)(IINST iinstParent, LPCTSTR lpszAttribs, BOOL* pbHandled) PURE;
	STDMETHOD(AddAttribute)(IINST iinst, LPCTSTR lpszAttributeName, LPCTSTR lpszAttributeValue, 
		LPCTSTR lpszAttributeComment, long nHintType, LPCTSTR lpszHintName, BOOL bAddAfter, 
		BOOL* pbHandled) PURE;
	STDMETHOD(put_AccessType)(IINST iinst, short nAccessType, BOOL* pbHandled) PURE;
	STDMETHOD(GetOneLine)(IINST iinst, long nFileType, long nLineNumber, BSTR* bstrLineContents, 
		BOOL* pbHandled) PURE;
	STDMETHOD(SetOneLine)(IINST iinst, long nFileType, long nLineNumber, LPCTSTR lpszText, 
		BOOL* pbHandled) PURE;
	STDMETHOD(InsertText)(IINST iinst, long nFileType, LPCTSTR lpszText, long nLineNumber, long nColumn, 
		BOOL bFullLine, BOOL* pbHandled) PURE;
	STDMETHOD(Delete)(IINST iinst, short nWhichPart, short nCommentOrDeletePart, BOOL* pbHandled) PURE;
	STDMETHOD(put_Type)(IINST iinst, LPCTSTR lpszType, BOOL* pbHandled) PURE;
	STDMETHOD(put_Comment)(IINST iinst, LPCTSTR lpszComment, long nHintType, LPCTSTR lpszHintName,
		BOOL* pbHandled) PURE;

	// IINST-based modification services that require that the IINST be something that has members, 
	// like a class
	STDMETHOD(AddMember)(IINST iinstClass, short nParseLanguage, long nMemberKind, LPCTSTR lpszMemberName, 
		LPCTSTR lpszMemberType, short nAccessType, LPCTSTR lpszAttributes, LPCTSTR lpszComments, 
		LPCTSTR lpszConditions, LPCTSTR lpszBody, long nHintType, LPCTSTR lpszHintName, BOOL bMemberAddAfter, 
		BOOL* pbHandled) PURE;
	STDMETHOD(AddMemberBody)(IINST iinstClass, short nParseLanguage, long nMemberKind, LPCTSTR lpszMemberName, 
		LPCTSTR lpszMemberType, LPCTSTR lpszComments, LPCTSTR lpszMemberBody, LPCTSTR lpszConditions, 
		long nHintType, LPCTSTR lpszHintName, BOOL bMemberAddAfter, BOOL* pbHandled) PURE;
	STDMETHOD(AddBaseClass)(IINST iinstClass, LPCTSTR lpszBaseClass, short nAccessType, long nHintType, 
		LPCTSTR lpszHintName, BOOL* pbHandled) PURE;

	// IINST-based modification services that require that the IINST be something that takes parameters, 
	// like a function
	STDMETHOD(SetParameters)(IINST iinstObject, short nWhichPart, LPCTSTR lpszParameters, 
		BOOL* pbHandled) PURE;
	STDMETHOD(AddParameter)(IINST iinstObject, short nParamNum, BOOL bAddAfter, LPCTSTR lpszNewParam,
		LPCTSTR lpszNewParamAttributes, LPCTSTR lpszNewParamComments, BOOL* pbHandled) PURE;
	STDMETHOD(put_ParameterName)(IINST iinstObject, short nParamNum, LPCTSTR lpszName, BOOL* pbHandled) PURE;
	STDMETHOD(put_ParameterValue)(IINST iinstObject, short nParamNum, LPCTSTR lpszValue, BOOL* pbHandled) PURE;
	STDMETHOD(DeleteParameter)(IINST iinstObject, short nParamNum, BOOL bHardDelete, BOOL* pbHandled) PURE;
	STDMETHOD(put_ParameterType)(IINST iinstObject, short nParamNum, LPCTSTR lpszType, BOOL* pbHandled) PURE;
	STDMETHOD(put_ParameterComment)(IINST iinstObject, short nParamNum, LPCTSTR lpszComment, long nHintType, 
		LPCTSTR lpszHintName, BOOL* pbHandled) PURE;
	STDMETHOD(put_ParameterAttributes)(IINST iinstObject, short nParamNum, LPCTSTR lpszAttribs, BOOL* pbHandled) PURE;
	STDMETHOD(AddParameterAttribute)(IINST iinstObject, short nParamNum, LPCTSTR lpszAttributeName, 
		LPCTSTR lpszAttributeValue, LPCTSTR lpszAttributeComment, long nHintType, LPCTSTR lpszHintName, 
		BOOL bAddAfter, BOOL* pbHandled) PURE;
	STDMETHOD(DeleteParameterAttribute)(IINST iinstObject, short nParamNum, LPCTSTR lpszAttributeName,
		BOOL bHardDelete, BOOL* pbHandled) PURE;
	STDMETHOD(put_ParameterAttribute)(IINST iinstObject, short nParamNum, LPCTSTR lpszOldAttributeName,
		LPCTSTR lpszAttributeName, LPCTSTR lpszAttributeValue, LPCTSTR lpszAttributeComment, BOOL* pbHandled) PURE;

	// IINST-based modification services for adding elements to a map or enum.  bstrValue only has 
	// meaning for adding an enum item
	STDMETHOD(AddItem)(IINST iinstObject, LPCTSTR lpszItem, LPCTSTR lpszValue, LPCTSTR lpszComments, 
		long nHintType, LPCTSTR lpszHintName, BOOL bAddAfter, BOOL* pbHandled) PURE;

	// menu support services for ICodeManagerMenuBuilder.  These are non-standard COM and will need
	// to be changed (removed?) when we change shell architecture
	STDMETHOD(GetCmdIDs)(VCDesignElementI* pDO, CWordArray& aVerbs, 
		CWordArray& aCmds, int nExtra) PURE;
	STDMETHOD(OnCmdMsg)(VCDesignElementI* pDO, UINT nID, int nCode, void* pExtra, LPBOOL lpRetVal, 
		CWnd* slobWnd, int nExtra) PURE;
};

#endif	// __CODEMGRSUPPORTAPI__

