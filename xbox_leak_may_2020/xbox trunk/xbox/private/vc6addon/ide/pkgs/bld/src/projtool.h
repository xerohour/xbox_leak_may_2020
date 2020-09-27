//
//	PROJTOOL.H
//
//		Definition of classes for tracking buidld tools.

#ifndef _PROJTOOL_H_
#define _PROJTOOL_H_

#ifndef _SUSHI_PROJECT
// If you are encluding this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

#include "oletool.h"

// Note: NMAKE /A currently doesn't properly handle multiple targets on the
// LHS of a dependency/build rule (eg. A B C : deps).  Remove this if fixed.
#define _NMAKE_LHS_BUG 1

#include "project.h"
#include "projcomp.h"
#include "projitem.h"
#include "prjoptn.h"
#include "makread.h"
#include "scanner.h"
#include "engine.h"

#include <utilbld_.h>

class CCmdLine {
public:
	CCmdLine()
	{
		bDelay = FALSE;
		nProcessor = 1;
	}
	CStringList 	slCommandLines;
	CStringList 	slDescriptions;
	int				nProcessor;
	BOOL			bDelay;
};

class CBuildTool;

class CIncludeEntry;
class CIncludeEntryList;

class CNameMunger;

// Helper functions used by tools (in PROJTOOL.CPP):
BOOL BLD_IFACE FileNameMatchesExtension(const CPath *pPath, const TCHAR *pExtensions);
BOOL ReplaceEnvVars(CString &string);

BOOL PtrListsAreEqual
( 
	const CPtrList &l1, 
	const CPtrList &l2,
	BOOL (*pfn) (DWORD, FileRegHandle) = NULL,
	DWORD dwFuncCookie = NULL
);

// Used for figuring out filenames controlled by option flags, e.g. /MAP:foo.map 
BOOL BLD_IFACE GetItemOutputFile
( 
	CProjItem *pItem,
	UINT nFileNameID, 
	FileRegHandle &frh,
	const TCHAR *pcDefExtension,
	const CDir *pBaseDir,
	UINT nEnablePropId = -1 
);	  

// Function used by DoBuild to handle prelink/postbuild actions.
extern UINT DoSpecialCommandLines
( 
	CProjItem* pItem,
	BOOL bPreLink,
	DWORD attrib,
	CErrorContext & EC
);

///////////////////////////////////////////////////////////////////////////////
//
//	Build tool classes:
//
//	We divide build tools into two types: "Source Tools" which take a single 
//	input and produce intermeddiate files as output (e.g. C compiler)  and 
//  "Schmooze tools" which take several intermediate files and produce a 
//  the final targets.
enum WritingAreas
{
	NoAreas		= 0x0,
	PerConfig	= 0x1,
	BuildRule	= 0x2,
	Header		= 0x4,
	ToolUsed	= 0x8,
	PerConfigInt	= 0x10,		// internal proj only
	PerConfigExt	= 0x20,		// makefile only
};

// CBuildTool debugging bits
#define DBG_BT_OFF	0x0				// (no debugging)
#define DBG_BT_V3	0x1				// uses v3 tool API
#define DBG_BT_ACT	0x2				// show assoc. action dbg. info
#define DBG_BT_NOT	0x4				// show action notifications

// CBuildTool deferred enumeration?
enum DAB				// Deferred Action Behavior
{
	dabNotDeferred,		// Normal behavior -- build if out-of-date
	dabDeferred,		// Defer -- build only if target doesn't exist
	dabNeverBuild		// Never build -- regardless of existence or timestamps
};

// action events
#define ACTEVT_Create		0x0	// action is being created
#define ACTEVT_Destroy		0x1	// action is being destroyed
#define ACTEVT_PreGenDep	0x2	// action input file dep. generate is about to start
#define ACTEVT_PostGenDep	0x3	// action input file dep. generate is finished
#define ACTEVT_QueryBrowse	0x4	// ask action if it is browsing
#define ACTEVT_SetBrowse	0x5	// ask action to set it browsing
#define ACTEVT_ClearData	0x6	// ask action to clear m_dwData in the action, fortran use only

class BLD_IFACE CBuildTool : public CBldSysCmp, public COptHdlrMapper
{
	DECLARE_DYNCREATE(CBuildTool)
	DECLARE_SLOBPROP_MAP()
	DECLARE_REGISTRY_MAP()

public:
	// Default CTOR
	// N.B. dwDebug is for debugging purposes,
	// bits can be set to enable/disable debugging features.
	CBuildTool(DWORD dwDebug = DBG_BT_OFF);
	virtual ~CBuildTool();

	// Init.
	void FInit();

	virtual LPBUILDTOOL GetToolInterface( void ){
		return( NULL );
	}

	// default tool options for the tool for the target type component, 'bsc_id', or
	// for any target type with attributes
	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption);

	//
	// \/ Build system engine interface API 
	//

	// Interaction with per-action 'tool-supplied' data and build system
	// events
	virtual void OnActionEvent(UINT idEvent, CActionSlob * pAction, DWORD dwExtra = 0)
		{if (idEvent == ACTEVT_Create) pAction->m_dwData = 0;}

	// Generate dependencies for 'frhFile' and use CAction::AddScannedDep() or CAction::AddSourceDep()
	// to modify the action 'pAction'.
	// (errrors should be pushed into the error context EC)
	virtual BOOL GenerateDependencies(CActionSlob * pAction, FileRegHandle frhFile, CErrorContext & EC);

 	// Scan a file item and fill out its depenency list.
	// Note this is not virtual, the tool specific behaviour
	// is implemented in the 'GenerateDependencies()' method.
	BOOL ScanDependencies(CProjItem * pItem, BOOL bWriteOutput = TRUE);

	BOOL UpdateDependencies(CProjItem * pItem, CPtrList *pdepList);

 	// Helpers called by 'ScanFile()' etc.
	virtual void GetIncludePathFromOptions(CProjItem *pItem, CString& strResult) {}
	virtual void GetIncludePaths(CProjItem *pItem, CString& str) {}

	// Generate the corresponding outputs for each of the input file sets in each
	// of the actions
	// (errors should be pushed into the error context EC)
	// The 'type' is an AOGO_.
	virtual BOOL GenerateOutput(UINT type, CActionSlobList & actions, CErrorContext & EC);

	// Does this option property affect the name of the output files?
	virtual BOOL AffectsOutput(UINT idProp) {return FALSE; /* no */}

	// Return the action state given an action that specifies the input and output
	// file sets
	// (errors should be pushed into the error context EC)
	virtual ActionState FigureActionSate(CActionSlob action, CErrorContext & EC);

	// UINT type can be TOB_Build, or TOB_ReBuild
	// UINT stage can be TOB_Pre, TOB_Stage, TOB_Post
	virtual UINT PerformBuildActions(UINT type, UINT stage, CActionSlobList & lstActions,
									 DWORD attrib, CErrorContext & EC);

	// Does this tool operate on this file?
	virtual BOOL AttachToFile(FileRegHandle frh = (FileRegHandle)NULL, CProjItem * pItem = (CProjItem *)NULL);

	// Get command lines for this tool
	// o performs default for both non-target and target tools
	virtual BOOL GetCommandLines
	( 
		CActionSlobList & lstActions,
		CPtrList & plCommandLines, 
		DWORD attrib,
		CErrorContext & EC
	);

	// Get command line options for this tool
	virtual BOOL GetCommandOptions
	(
		CActionSlob * pAction,
		CString & strOptions
	);

	// Used as mark during build file read/write
	WORD m_wBuildFileMark;

  	CString	m_strToolExeName;	// our executable name
	CString m_strToolPrefix;	// our nmake macro prefix
	CString m_strToolInput;		// our input spec.
	CString m_strToolOutput;	// our output spec.
	CString m_strToolMenuText;	// our menu text
	CString m_strDescription;	// our description
	BOOL m_bToolPerProject;		// per-project tool?
	BOOL m_bToolInternal;		// an internal tool?
	BOOL m_fPerformIfAble;		// perform tool actions if able?
	BOOL m_fDeferredTool;		// is this a deferred tool?

	// GetMenuID when showing deferred action:
	UINT GetMenuID() {return m_MenuID;}
	void SetMenuID(UINT MenuID) {m_MenuID = MenuID;}
	
	// Set/retrieve deferred tool attributes
	// Return either dabNotDeferred indicating normal timestamp-checking
	// behavior, dabDeferred indicating deferred action, or dabNeverBuild indicating
	// the target should never be built, regardless of existence or timestamps.
	// The string is set up with the menu name ONLY if the string pointer passed in is non-NULL.
	virtual DAB DeferredAction(CActionSlob * pAction, CString * pstrAction = (CString *)NULL) {return dabNotDeferred;}
	virtual BOOL SetDeferredAction(CActionSlob * pAction, DAB dab) {return FALSE;}

	// Is this target a oriented tool?
	BOOL m_fTargetTool;
	UINT GetOrder(){ return m_nOrder; };

	//
	// /\ Build system engine interface API 
	//

	// Has primary output for a target?
	virtual BOOL HasPrimaryOutput() {return FALSE;}

	//  Does this tool to dependency scanning:
	virtual BOOL IsScannable() {return FALSE;}
		
	virtual BOOL ProcessAddSubtractString
	( 
		CProjItem * pItem,
		const TCHAR * pstr,
		BOOL fSubtract,
		BOOL fBaseBag = FALSE
	);

	// Ensure that the tool will build an the project item (debug or release config. mode)
	// Likely to entail doing such things as munging tool options, eg. the libs for linker
	// FUTURE: Use wizards to do this in v4.0?
	virtual BOOL PerformSettingsWizard(CProjItem *, BOOL fDebug, int iUseMFC) {return FALSE;}

	// How many time is  this tool used in the configurations of this project:
	int GetToolUsedCount(CProject * pProject);

	// Is this tool used in the project type for the item;s active configuartion:
	BOOL PresentInActiveConfig(CProjItem * pItem);

	// builder file reading
	//
	// function called to read a tool builder macro line
	// ie. one of the form '<tool prefix>_FOOBAR = '
	// return FALSE if an error occurred, this will abort builder reading
	virtual BOOL ReadToolMacro(CMakMacro * pMacro, CProjItem * pItem) {return TRUE; /* ok */}

	// Functions called at the start end end of builder file writing.
	// This is allow the tool to allocate special buffers and so on for building options strings etc.
	virtual void BeginProjectWrite(CProject * pProject, CDir & dirBase) {}
	virtual void EndProjectWrite(CProject * pProject, CDir & dirBase) {EndWritingSession ();}

	// Filter function to pick interesting files this item might make:
	virtual BOOL IsProductFile(const CPath * pPath) { return TRUE; }

	// Functions for writing to make file.  There are three possible
	// areas a tool can write into:
	//
	//	1. Configuration ifdef area, e.g. #if "$(CFG)"=="DEBUG"  This is where
	//	   the tool puts its per confiuration rules for each item.
	//	2. Item build rule, where the tool can put its acutally nmake 
	//	   description.
	//	3. Project level "header," called only once for the top level project.
	//	   This is where the item puts inference rule and any other special
	//	   once off stuff (like precompiled headers)
	//
	// To allow us to "fold" configuartion data together and to allow the tools
	// to cache information, each item writing session beging with	BeginWritingSession.
	// The tool then tells what areas it will need to write into, and can at this
	// time prepare strings to write. EndWritingSession marks the end of an item
	// and any special data associated with it can be tossed.
	// In all of a tools macros should be prefaced with its prefix string
	// so that it we always find the associated tool.  Also, macro names should
 	// not embed configuartion names if possible. 

	virtual int BeginWritingSession
	(
		CProjItem * pItem,
		CActionSlob * pAction,
		CMakFileWriter * ppw,
		CMakFileWriter * pmw,
		const CDir * pBaseDir,
		CNameMunger * pnm
	);

	// May get called twice in a row, or even if BeginWritingSession returns NoAreas.
	// Should do safe cleanup.
	virtual void EndWritingSession();

	// Write out the imems flags for internal consumptions:
	virtual void WriteAddSubtractLine(CProjItem * pItem);

	// Write data for the Item's active configuration.  This will have been 
	// temporarily set with ForcePropBagActive:
	virtual BOOL WritePerConfigData(CProjItem * pItem, CActionSlob * pAction);
	virtual BOOL WriteHeaderSection(CActionSlob * pAction);

	virtual BOOL WriteBuildRule(CActionSlob * pAction);
	virtual BOOL DoWriteBuildRule(CActionSlob * pAction);

	virtual void WriteOutputDirMacro(CActionSlob * pAction) {}
	virtual void WriteInferenceRules(CActionSlob * pAction) {}
	virtual void WriteInferenceRuleMacro(CActionSlob * pAction) {};

	//	Check if this is a prepass tool. By default it is NOT a prepass tool
	virtual BOOL IsPrePassTool()		{return FALSE;}

	const static int nMaxCommandLineLength;

	// UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE
	// These methods exist to replace 'IsKindOf(Tool_Class)' functionality
	// they should be removed with a more extensive re-work of the build system.
	// [matthewt]
	virtual BOOL StoreOptionsOnTarget() {return FALSE;}
	virtual BOOL IsMecrTool() {return FALSE;}	// HACK ha ha
	// UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE UNDONE

	// Support for dependencies from tool instead of from scanner.  Derived tool must 
	// override these if it can generate its own dependencies for a source
	// file, e.g., the compiler can use minimal rebuild information to
	// get dependencies.
	virtual BOOL HasDependencies( ConfigurationRecord* pcr) { return FALSE; }
	virtual BOOL DontWriteDependencies( ConfigurationRecord* pcr) { return HasDependencies( pcr ); }
	virtual BOOL GetDependencies( ConfigurationRecord* pcr, CStringArray& strArray, BOOL* bUpToDate = NULL) { return FALSE; }

	// debugging options
	DWORD m_dwDebugging;

protected:

	// Helper, write out multi-inputs,
	// eg. 
	// LINK_OBJS=/
	// "$(INTDIR)\foobar.obj"/
	// "$(INTDIR)\bloggs.obj"/
	//
	BOOL WriteMultiInputs(CActionSlob * pAction, BOOL bSort = FALSE);
	void ReplaceEnvVars(CString &string);

	BOOL bParentFlags, bOtherTools, bWeAreUsed;
	BOOL bPerConfigFlags, bPerConfigTargs, bUsedInAllConfigs, bPerConfigDeps;
	BOOL bPerConfigInt, bPerConfigExt;
	BOOL bShouldWriteBuildRulePerConfig;
	BOOL bForceNoInferenceRule;
	BOOL bProjPerConfigFlags;
	BOOL bWeHaveFlags;

	// Cached information for writing:
	CProjItem		*m_pItemBogus;	// FUTURE: remove this
	CMakFileWriter 	*m_pPrjWriter;
	CMakFileWriter 	*m_pMakWriter;
	CNameMunger 	*m_pNameMunger;
	const CDir 		*m_pBaseDir;

	enum OutputDirOptions { NoOutputDirs, OneOutputDir, PerConfigOutputDirs };
	virtual  OutputDirOptions HasPerConfigOutputDirs (CProjItem * pItem) {return NoOutputDirs;}
	OutputDirOptions m_OutputDirOptions;

	UINT m_nIDName;				// resource ID of tool name, for deferred init
	UINT m_nIDUIName;			// resource ID of tool UI name, for deferred init
	UINT m_nOrder;
   
private:
	BOOL m_MenuID;				// deferred action menu id
	static int m_nMyMarker;
};

#pragma message( "Using temporary code to enable/disable new update dependency code")
	//
	// NOTE: g_bNewUpdateDepModel is a temporary workaround for enabling/disabling this
	// new feature.
	//
extern const BOOL g_bNewUpdateDepModel;

// move these declarations to non-addon #include'd header
#ifdef _BLD_INTERNAL

/*
R.I.P. for v4.0 with VBA?
// 
// Custom tools (derived from the generic CBuildTool)
//
class CCustomTool : public CBuildTool
{
	DECLARE_DYNAMIC(CCustomTool)

ublic:
	CCustomTool() {};
	CCustomTool(const TCHAR * pszName);
	virtual ~CCustomTool();

	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption) { return TRUE; } // REVIEW

	// -> from CBuildTool
	BOOL GenerateOutput(UINT type, CActionSlobList & actions, CErrorContext & EC);
	// <- from CBuildTool

	//
	// Custom tool macros
	//

	// form the output path from the input path and output specification
	BOOL FormOutputI
	(
		CPath & pathOutput,
		const CPath * pPathInput,
		const TCHAR * pchOutSpec,
		CProjItem * pItem
	);

	// map a macro id (IDMACRO_) to the macro name
	static BOOL MapMacroIdToName(UINT idMacro, const TCHAR * & pchName);

	// map a macro name to a macro id (IDMACRO_)
	static BOOL MapMacroNameToId(const TCHAR * pchName, int & cchName, UINT & idMacro);

	void Refresh();
};
*/

extern UINT g_nIDToolUnknown;

// 
// Custom build tool (derived from the generic CBuildTool)
//

extern const TCHAR g_pcBeginCustomBuild[];
extern const TCHAR g_pcEndCustomBuild[];

typedef struct tagMACROINFO {
	CString strName;
	CString strValue;
} MACROINFO;

class CCustomBuildTool : public CBuildTool
{
	DECLARE_DYNAMIC(CCustomBuildTool)

public:
	CCustomBuildTool();
	virtual ~CCustomBuildTool();

	// -> from CBuildTool
	BOOL AttachToFile(FileRegHandle frh, CProjItem * pItem);
	BOOL GenerateOutput(UINT type, CActionSlobList & actions, CErrorContext & EC);
	BOOL HasPrimaryOutput() {return TRUE;}

	virtual void WriteAddSubtractLine(CProjItem * pItem) { } // do nothing
	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption) { return TRUE; } // nothing interesting to do for custom build

	virtual BOOL HasDependencies( ConfigurationRecord* pcr) { return FALSE; }
	virtual BOOL GetDependencies( ConfigurationRecord* pcr, CStringArray& strArray, BOOL* bUpToDate = NULL) { return FALSE; }

	BOOL GetCommandLines
	( 
		CActionSlobList & lstActions,
		CPtrList & plCommandLines,
		DWORD attrib,
		CErrorContext & EC
	);

	int BeginWritingSession
	(
		CProjItem * pItem,
		CActionSlob * pAction,
		CMakFileWriter * ppw,
		CMakFileWriter * pmw,
		const CDir * pBaseDir,
		CNameMunger * pnm
	);

	// write our custom build step to the builder file
	BOOL DoWriteBuildRule(CActionSlob * pAction);
	// <- from CBuildTool

	// read our custom build step from the builder file
	static BOOL DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude);

	// get and set custom build properties
	static BOOL SetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & str);
	static BOOL GetCustomBuildStrProp(CProjItem * pItem, UINT idProp, CString & str, BOOL fDefault = TRUE);

	// write macros
	BOOL WriteMacros(CMakFileWriter& mw, BYTE aMacros[], CActionSlob* pAction);

protected:
	// Macro names and values - used when we are reading/writing a custom build
	// tool for an unknown platform
	CPtrList	m_lstMacros;
};

// 
// Unknown tools (derived from the generic CBuildTool)
//
class CUnknownTool : public CBuildTool
{
	DECLARE_DYNAMIC(CUnknownTool)

public:

	CUnknownTool() {};
	CUnknownTool(const TCHAR * lpszPlatform, const TCHAR * lpszPrefix, const TCHAR * lpszExeName = NULL);
	virtual ~CUnknownTool();

	// from -> CBldSysCmp
	BOOL IsSupported() const { return FALSE; }
	// <- CBldSysCmp

	// -> from CBuildTool
	BOOL GenerateDependencies(CActionSlob * pAction, FileRegHandle frhFile, CErrorContext & EC) {return TRUE; /* none */}
 	BOOL AttachToFile(FileRegHandle frh = (FileRegHandle)NULL, CProjItem * pItem = (CProjItem *)NULL);
	// <- from CBuildTool

	__inline UINT GetUnknownPropId() const {return m_nIDUnkStr;}

	// builder file reading
	//
	// function called to read a tool builder macro line
	// ie. one of the form '<tool prefix>_FOOBAR = '
	// for the unknown tool we'll preserve these...
	BOOL ReadToolMacro(CMakMacro * pMacro, CProjItem * pItem);
	
	// builder file writing
	//
	int BeginWritingSession(CProjItem * pItem, CActionSlob * pAction, CMakFileWriter * ppw, CMakFileWriter * pmw, const CDir * pBaseDir, CNameMunger * pnm); 
	BOOL WritePerConfigData(CProjItem * pItem, CActionSlob * pAction);
	void WriteInferenceRuleMacro(CActionSlob * pAction);
	void WriteInferenceRules(CActionSlob * pAction);
	void WriteOutputDirMacro(CActionSlob * pAction);

private:
	CString m_strBuffer;
	UINT m_nIDUnkStr;
};



// COLEBuildTool - the proxy for an OLE Add On Build tool
class COLEBuildTool : public CBuildTool {
	DECLARE_DYNAMIC(COLEBuildTool)

public:

	COLEBuildTool( LPUNKNOWN pUnk, DWORD dwDebug = DBG_BT_OFF );

	virtual ~COLEBuildTool();

	virtual LPBUILDTOOL GetToolInterface( void ){
		m_pBuildTool->AddRef();
		return( m_pBuildTool );
	}


	//*******************************
	// IBUILDTOOL
	//*******************************

	// this tool claims this file if Filter returns true.
	BOOL AttachToFile(FileRegHandle frh, CProjItem * pItem);

	// UINT type can be TOB_Build, or TOB_ReBuild
	// UINT stage can be TOB_Pre, TOB_Stage, TOB_Post
	UINT PerformBuildActions(UINT type, UINT stage, CActionSlobList & lstActions,
										 DWORD attrib, CErrorContext & EC);
	// Get command lines for this tool
	BOOL GetCommandLines
		( 
			CActionSlobList & lstActions,
			CStringList & slCommandLines, CStringList & slDescriptions,
			DWORD attrib,
			CErrorContext & EC
		) ;

	BOOL GenerateOutput(UINT type, CActionSlobList & lstActions, CErrorContext & EC);

	BOOL GetDependencies( ConfigurationRecord* pcr, CStringArray& strArray, BOOL* bUpToDate );


	//*******************************
	// IDEPSCANNER 
	//*******************************

	// Generate dependencies for 'frhFile' and use CAction::AddScannedDep() or CAction::AddSourceDep()
	// to modify the action 'pAction'.
	// (errrors should be pushed into the error context EC)
	BOOL GenerateDependencies(CActionSlob * pAction, FileRegHandle frh, CErrorContext & EC);

 	// Scan a file item and fill out its depenency list.
	// Note this is not virtual, the tool specific behaviour
	// is implemented in the 'GenerateDependencies()' method.
	// BOOL ScanDependencies(CProjItem * pItem);
	// BOOL UpdateDependencies(CProjItem * pItem, CPtrList *pdepList);

	//*******************************
	// IDEPMANAGER - NYI
	//*******************************

	// BOOL IsOutOfDate()

private:
	LPBUILDTOOL	m_pBuildTool;

	CScanner 	*m_pDepScanner;
	CDeps		*m_pDepManager;
};


// 
// Special build tool (derived from the generic CBuildTool)
//

extern const TCHAR g_pcBeginSpecialBuildTool[];
extern const TCHAR g_pcEndSpecialBuildTool[];

class CSpecialBuildTool : public CBuildTool
{
	DECLARE_DYNAMIC(CSpecialBuildTool)

public:
	CSpecialBuildTool();
	virtual ~CSpecialBuildTool();

	// Similar to custom build tool.
	BOOL AttachToFile(FileRegHandle frh, CProjItem * pItem);

	virtual void WriteAddSubtractLine(CProjItem * pItem) { } // do nothing
	virtual BOOL GetDefaultToolOptions(DWORD bsc_id, DWORD attrib, DWORD mode, CString & strOption) { return TRUE; } // nothing interesting to do for special build

	virtual UINT PerformBuildActions(UINT type, UINT stage, CActionSlobList & lstActions,
		DWORD attrib, CErrorContext & EC) { return ACT_Complete; }

	// Similar to custom build tool.
	int BeginWritingSession
	(
		CProjItem * pItem,
		CActionSlob * pAction,
		CMakFileWriter * ppw,
		CMakFileWriter * pmw,
		const CDir * pBaseDir,
		CNameMunger * pnm
	);

	// write our custom build step to the builder file
	BOOL DoWriteBuildRule(CActionSlob * pAction);
	// <- from CBuildTool

	// read our custom build step from the builder file
	static BOOL DoReadBuildRule(CProjItem * pItem, TCHAR * pchCustomPrelude, BOOL bAllConfigs = FALSE);

	// write macros
	BOOL WriteMacros(CMakFileWriter& mw, BYTE aMacros[], CActionSlob* pAction);

};

#endif // _BLD_INTERNAL

///////////////////////////////////////////////////////////////////////////////
#endif // _PROJTOOL_H_
