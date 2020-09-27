// CBuildEngine
//
// Build engine.
//
// [matthewt]
//
				   
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "slob.h"		// CSlob
#include "bldapi.h"		// Interfaces
#include "bldguid.h"		// Interfaces

// definition of the CActionSlob class
class CProjItem;
class CBuildTool;
class CFileRegistry;
class CIncludeEntryList;

// Action Enumerator
class CEnumActions : public CCmdTarget {
protected:
	BEGIN_INTERFACE_PART(EnumActions, IEnumBSActions)
		INIT_INTERFACE_PART(CEnumActions, EnumActions)
		// IEnum style methods
		STDMETHOD(Next)(THIS_ ULONG celt, IBSAction **rgelt, ULONG *pceltFetched);
		STDMETHOD(Skip)(THIS_ ULONG celt);
		STDMETHOD(Reset)(THIS);
		STDMETHOD(Clone)(THIS_ IEnumBSActions **ppenum);
	END_INTERFACE_PART(EnumActions)

	DECLARE_INTERFACE_MAP()

public:
	void SetList( CActionSlobList *list ){ pList = list; }

	// convenience function. to get the interface ptr.
	LPENUMBSACTIONS GetInterface(){
		LPENUMBSACTIONS pIEnum;
		m_xEnumActions.QueryInterface( IID_IEnumBSActions, (void **)&pIEnum );
		return pIEnum;
	}

private:
	POSITION pos;
	CActionSlobList *pList;
};

// definition of the CActionSlobList class
class BLD_IFACE CActionSlobList : public CObList
{
	friend class CEnumActions;
	DECLARE_DYNAMIC(CActionSlobList)
	CActionSlobList():CObList( 3 ){
		enumActions = NULL;
		pIEnum = NULL;
	};

public:
	// find a tool and return the postion of the action
	POSITION Find(CBuildTool * pTool);

	// Get Interface and release interface are used to 
	// control the add ons ability to access the action list
	void ReleaseInterface( void ){
		if( pIEnum ) {
			enumActions->SetList( NULL );
			pIEnum->Release();
			pIEnum = NULL;
		}
	};

	LPENUMBSACTIONS GetInterface(void){
		if( pIEnum ) {
			pIEnum->AddRef();
		} else {
			enumActions = new CEnumActions;
			pIEnum = enumActions->GetInterface();
			enumActions->SetList(this);
		}
		return pIEnum;
	}

#ifdef _DEBUG
	virtual void Dump(CDumpContext & dc) const;
#endif
	// this should be co create instanced
	CEnumActions 	*enumActions;
	LPENUMBSACTIONS  pIEnum;
};

#pragma hdrstop
#include "depgraph.h"	// dep. graph

// creating and deleting a list of temporary files
class BLD_IFACE CTempMaker
{
public:
	// FUTURE: pick better size? (prime)
	CTempMaker() {m_Files.InitHashTable(17);}
	~CTempMaker();	

	BOOL CreateTempFile(CStdioFile * & pFile, CPath & rPath, CErrorContext & EC = g_DummyEC, BOOL fBatFile = FALSE );
	BOOL CreateTempFileFromString(CString & str, CPath & rPath, CErrorContext & EC=g_DummyEC, BOOL fBatFile = FALSE );

	// Remove all the files from disk:
	void NukeFiles(CErrorContext& EC = g_DummyEC);

private:
	CMapPathToOb m_Files;
	CDir m_DirTempPath;
};

// state of an action 
typedef enum ActionState
{
	_SrcMissing,		// source missing for action
	_SrcOutOfDate,		// output out of date wrt sources
	_DepsOutOfDate,		// source out of date wrt deps
	_Current,			// output is current wrt to output and deps
	_Unknown			// unknown action state
};

// Scanned source dependency list state
#define DEPSTATE	CIncludeEntryList *
#define DEP_Empty	((DEPSTATE)NULL)	// empty deps
#define DEP_None	((DEPSTATE)-1)		// forced to have no deps

// type and stage of the action to be performed

// action type
#define TOB_Build	0x0				// action must build output
#define TOB_ReBuild	0x1				// action must re-build output
#define TOB_Clean	0x2				// action must clean output

// action stage
#define TOB_Pre		0x0				// perform pre-action processing
#define TOB_Stage	0x1				// perform the action type
#define TOB_Post	0x2				// perform post-action processing

// attributes of a build
#define AOB_Default			0x0		// default build
#define AOB_IgnoreErrors	0x1		// 'ignore errors' build, eg. batched builds
#define AOB_Forced			0x2		// 'forced' build, eg. compile a single item

// action return status
#define ACT_Error		0x0			// error during action
#define ACT_Canceled	0x1			// canceled action
#define ACT_Complete	0x2			// completed action

// generation of output files
#define AOGO_Primary	0x1							// only those considered primary
#define AOGO_Other		0x2							// other output files
#define AOGO_All		AOGO_Primary | AOGO_Other	// all output files
#define AOGO_Default	AOGO_All					// default is top generate all output files



class CBSAction : public CCmdTarget {
protected:
	BEGIN_INTERFACE_PART(Action, IBSAction)
		INIT_INTERFACE_PART(CBSAction, Action)
		STDMETHOD(GetInputs)(LPBUILDFILESET *);
		STDMETHOD(GetOutputs)(LPBUILDFILESET *);
		STDMETHOD(GetTool)(LPBUILDTOOL *);
		STDMETHOD(GetFile)(LPBUILDFILE *);
		STDMETHOD(GetConfiguration)(HCONFIGURATION *);
		STDMETHOD(AddOutput)(const CPath *pPath);
		STDMETHOD(GetCommandOptions)(CString *);
	END_INTERFACE_PART(Action)

	DECLARE_INTERFACE_MAP()
public:
	void SetAction( CActionSlob *pAct ){ pAction = pAct; }

	LPBSACTION GetInterface( void ){
		LPBSACTION pIAction;
		m_xAction.QueryInterface( IID_IBSAction, (void **)&pIAction );
		return pIAction;
	}
private:
	CActionSlob *pAction;
};


class BLD_IFACE CActionSlob : public CSlob, public CFileRegNotifyRx
{
	DECLARE_DYNAMIC(CActionSlob)

	
public:
	// Associate this action to a particular project item. This project item's
	// configuration reocrd will be accessed to retrieve configuration specific
	// action information.
	// Associate this action with a particular tool. It will access the CBuildTool's
	// methods in order to modify the action's build information.
	CActionSlob() {};
	CActionSlob(CProjItem *, CBuildTool *, BOOL fBuilder = FALSE, ConfigurationRecord * pcr = (ConfigurationRecord *)NULL);
	~CActionSlob();

	LPBSACTION GetInterface(void){
		if( pCBSAction ) {
			pIAction->AddRef();
		} else {
			pCBSAction = new CBSAction;
			pIAction = pCBSAction->GetInterface();
			pCBSAction->SetAction(this);
		}
		return pIAction;
	}
	void ReleaseInterface(void) {
		if( pCBSAction ) {
			pCBSAction->SetAction(NULL);
			pIAction->Release();
		}
	}

	// Binding and un-binding to tools
	void Bind(CBuildTool * pTool);
	void UnBind();
	//
	// Assigning/unassigning of actions to project items (default is active config.)
	//
	static void AssignActions
	(
		CProjItem * pItem,
		CPtrList * plstSelectTools = (CPtrList *)NULL,
		ConfigurationRecord * pcr = (ConfigurationRecord *)NULL,
		BOOL fAssignContained = TRUE,
		BOOL fAssignContainedOnly = FALSE
	);
	static void UnAssignActions
	(
		CProjItem * pItem,
		CPtrList * plstSelectTools = (CPtrList *)NULL,
		ConfigurationRecord * pcr = (ConfigurationRecord *)NULL,
		BOOL fUnassignContained = TRUE
	);

	//
	// Adding/removing of actions to/from build instances (default is active config.)
	//
	static void BindActions
	(
		CProjItem * pItem,
		CPtrList * plstSelectTools = (CPtrList *)NULL,
		ConfigurationRecord * pcr = (ConfigurationRecord *)NULL,
		BOOL fBindContained = TRUE
	);
	static void UnBindActions
	(
		CProjItem * pItem,
		CPtrList * plstSelectTools = (CPtrList *)NULL,
		ConfigurationRecord * pcr = (ConfigurationRecord *)NULL,
		BOOL fUnbindContained = TRUE
	);

	//
	// Loading/storing from an archive
	//
	// FUTURE: 
	// Currently this will only load/save to an archive the
	// action command-lines and possible change flag.
	// If we ever 'export' .MAKs we can use this code to
	// serialize/deserialize the 'whole' builder (multiple projects)
	static BOOL LoadFromArchive(CArchive & ar);

	// old format compatible with 4.x and before
	static BOOL LoadFromArchiveVCP(CArchive & ar);
#if 0
	static BOOL SaveToArchive(CArchive & ar);
#endif
	static void IgnoreCachedOptions(BOOL bIgnore)  { s_bIgnoreCachedOptions = bIgnore; }
	static BOOL s_bIgnoreCachedOptions;

	// serialize/deserialize the action slob
	virtual void Serialize(CArchive & ar);

	// Assoc. target level action (iff we are not one ourselves)
	CActionSlob * GetTargetAction();

	//
	// Informing of all project actions for the specified config. (default is active config.)
	//
	static void InformActions
	(
		CProjItem * pItem,
		UINT idChange,
		ConfigurationRecord * pcr = (ConfigurationRecord *)NULL,
		BOOL fInformContained = TRUE
	);

	// 
	// Options at last perform etc.
	//

	
	__inline void DirtyCommand() {m_fDirtyCommand = TRUE;}

	// Retrieve and refresh the command-line options
	__inline BOOL GetCommandOptions(CString & strOptions) {strOptions = m_strOptions; return TRUE;}
	BOOL RefreshCommandOptions(BOOL & fChanged, BOOL fNoSet = FALSE);

	static void DirtyAllCommands
	(
		CProjItem * pItem,
		BOOL fPossibleChange = FALSE,
		ConfigurationRecord * pcr = (ConfigurationRecord *)NULL
	);

	//
	// Generate outputs
	//

	// Use when just a simple file reg. handle
	__inline BOOL AddOutput(FileRegHandle frh, BOOL fWithAction = TRUE)
		{frh->AddFRHRef(); return AddOutputI(frh, fWithAction);}

	static const TCHAR szDefVCFile[];

	// Used when path is already known
	BOOL AddOutput(const CPath * pPath, BOOL fWithAction = TRUE, BOOL fAllowFilter = TRUE);

	// Used for figuring out filenames controlled by option flags, e.g. /MAP:foo.map 
	// 'nPathId' is the path prop, may or may not return fullpath, might be dir. only
	// 'pcDefExtension' is the new extension to use
	// 'pBaseDir' is the directory to re-base the filename in
	// 'pcDefFilename' is the new filename (if none supplied from nPathId) (default is use filebase)
	// 'nEnablePropId' is the enabling prop., if FALSE then no output added
	// 'fWithAction' if TRUE will associate the output with an action in the graph
	BOOL AddOutput
	(
		UINT nPathId,
		const TCHAR *pcDefExtension,
		const CDir *pBaseDir,
		const TCHAR *pcDefFilename = (TCHAR *)NULL,
		UINT nEnablePropId = (UINT)-1,
		BOOL fWithAction = TRUE
	);

	// retrieve our 'actual' input(s)
	CFileRegSet * GetInput();
	__inline void DirtyInput(BOOL fDirty = TRUE) {m_fDirtyInput = fDirty;}
	void ClearInput();

	// retrieve our list of outputs (list of file registry handles)
	CFileRegSet * GetOutput();
	__inline CWordArray * GetOutputAttrib() {return m_fInGenOutput ? m_paryOldOutputAttrib : &m_aryOutputAttrib;}
	__inline void DirtyOutput(BOOL fDirty = TRUE) {m_fDirtyOutput = fDirty;}
	void ClearOutput();

	__inline CBuildTool * BuildTool()	{return m_pTool;}
	__inline CProjItem * Item()			{return m_pItem;}
	__inline BOOL FBuilder()			{return m_fBuilder;}

	//
	// Scanned dependencies
	//
	__inline CIncludeEntryList * GetIncludeDep() {return m_pIncludeList;}

	CFileRegSet * GetScannedDep();
	__inline void ClearScannedDep() {ClearDepI(DEP_Scanned);}
	void SubScannedDep(FileRegHandle frhDep);

	// add scanned dependency (unresolved)
	BOOL AddScannedDep(int type, CString & strFile, int nLine = 0, BOOL fScan = TRUE);

	// add/remove scanned dependency (resolved)
	__inline BOOL AddScannedDep(CString & strFile) {return AddDepI(DEP_Scanned, strFile);}
	__inline BOOL AddScannedDep(FileRegHandle frhDep) {return AddDepI(DEP_Scanned, frhDep);}
	__inline BOOL RemoveScannedDep(FileRegHandle frhDep) {return RemoveDepI(DEP_Scanned, frhDep);}

	//
	// Missing (scanned) dependencies
	//
	CFileRegSet * GetMissingDep(BOOL bAlloc = TRUE);
	__inline void ClearMissingDep() {ClearDepI(DEP_Missing);}
 	__inline BOOL AddMissingDep(CString & strFile) {return AddDepI(DEP_Missing, strFile);}
	__inline BOOL AddMissingDep(FileRegHandle frhDep) {return AddDepI(DEP_Missing, frhDep);}
	__inline BOOL RemoveMissingDep(FileRegHandle frhDep) {return RemoveDepI(DEP_Missing, frhDep);}

	//
	// Source dependencies
	//
	CFileRegSet * GetSourceDep();
	__inline void ClearSourceDep() {ClearDepI(DEP_Source);}

	// add/remove source dependency
 	__inline BOOL AddSourceDep(CString & strFile) {return AddDepI(DEP_Source, strFile);}
	__inline BOOL AddSourceDep(FileRegHandle frhDep) {return AddDepI(DEP_Source, frhDep);}
	__inline BOOL RemoveSourceDep(FileRegHandle frhDep) {return RemoveDepI(DEP_Source, frhDep);}

	__inline void SetDepState(DEPSTATE stateDep)
		{m_pIncludeList = stateDep;}

	//
	// Notification of input changing
	//
 
	// Rx for file change events
	virtual void OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint);

	// Batched notification?
	__inline void BatchNotify(BOOL fBatch) {m_fBatchNotify = fBatch;}

	//
	// Notification of output changing
	//

	// Rx for property change events, eg. we're interested in P_ToolOutput
 	virtual void OnInform(CSlob * pChangedSlob, UINT idChange, DWORD dwHint);

	//
	// User Interface helpers
	//

	// Fill a listbox with available tool names for the project item
	// Note, listbox ptr data is a CActionSlob * for each tool
	static void FillToolLBox(CListBox * pLBox, CProjItem * pItem);

	// Get the action for the currently selected tool
	static CActionSlob * GetToolLBoxAction(CListBox * pLBox);

	// Fill a grid with input or output for an action
	static void FillFileGrid(CActionSlob * pAction, CListBox * pLBox, UINT depType = DEP_Input);

	// Get the currently selected grid file registry entry
	static CFileRegEntry * GetFileGridEntry(CListBox * pLBox);
#ifdef _DEBUG
	virtual void Dump(CDumpContext & dc) const;
#endif

	// FUTURE: make private (currently used to Refresh() etc.)
	// Internal helper to set tool
	void SetToolI(CBuildTool *);

	// Helpers for add ref/release ref'ing file items when
	// doing an Assign/UnAssign action or Bind/Unbind action
	static void AddRefFileItem(CProjItem * pItem, CPtrList *frhList=NULL);
	static void ReleaseRefFileItem(CProjItem * pItem);

	// Current action mark to use
	static WORD m_wActionMarker;

	// Associated project item and tool (+ tool user-data)
	CProjItem * m_pItem;
	CBuildTool * m_pTool;
	CBuildTool * m_pOldTool;
	DWORD m_dwData;

	// Event data
	DWORD m_dwEvent;

	// The configuration this action is concerning.
	ConfigurationRecord * m_pcrOur;

	// Build process information
	CFileRegistry * m_pregistry;	// registry our build files are in
	CFileDepGraph * m_pdepgraph;	// build process graph

	//
	// Scanned source deps
	//
	ActionState m_state;	// state of this action

	// Used as mark during dep. graph analysis and build process
	WORD m_wMark;

	// Used during command-options refresh
	WORD m_wMarkRefCmdOpt;


	//  All bitfields are intentionally placed here to get best packing

	BOOL m_fPossibleOptionChange:1;
	// Used during command-options refresh
	BOOL m_fCmdOptChanged:1;

private:

	// Builder action? If not it'll be used just to store options at last build
	// for the target-level options.
	BOOL m_fBuilder:1;

	// Dirty input?
	BOOL m_fDirtyInput:1;

	// Dirty output?
	BOOL m_fDirtyOutput:1;

	// Dirty command-line?
	BOOL m_fDirtyCommand:1;

	// Batched input/output notify? .. ignore if TRUE
	BOOL m_fBatchNotify:1;

	// Updating output?
	BOOL m_fInGenOutput:1;

	// Assoc. target level action (iff we are not one ourselves)
	CActionSlob * m_pTrgAction;

	CString m_strOptions;

	// Input file or files (filter) 'possibly' consumed by this action
	FileRegHandle m_frhInput;

	// Temp. 'actual' input file set
	CFileRegSet m_frsInput;

	// Output files generated by this action
	CFileRegSet m_frsOutput;

#ifdef REFCOUNT_WORK
	// Output files that can be used as input to schmooze tools.
	CFileRegSet m_frsSchmoozableOutput;
#endif
	CFileRegSet *m_pfrsOldOutput;

	// Attributes of the output
	// Currently non-zero == 'has action', zero == 'no action'
	CWordArray m_aryOutputAttrib;
	CWordArray *m_paryOldOutputAttrib;

	// Scanned dependencies
	CIncludeEntryList * m_pIncludeList;		// unresolved
	CFileRegSet m_frsScannedDep;			// resolved (in the graph...)

	// Missing dependencies
	CFileRegSet m_frsMissingDep;

	// Source dependencies
	CFileRegSet m_frsSourceDep;			// resolved (in the graph...)

	//
	// Time of last dependency update on this CActionSlob.
	// Zero if uninitialized.
	//
	FILETIME m_LastDepUpdate;

public:
	//
	// Update dependency stuff.
	//
	BOOL IsDepInfoInitialized();
	BOOL IsNewDepInfoAvailable();

	//
	// Update dependency information if new information is available.
	// Set bForceUpdate = TRUE to force update if updating has been turned off
	// minimal rebuild info is not available.
	BOOL UpdateDepInfo(BOOL bForceUpdate = FALSE);

	void UpdateDepInfoTimeStamp();
private:

	
	// Internal helper for UI
	static int FillFileGridI(CActionSlob * pAction, CFileRegEntry * pEntry, CListBox * pLBox, int istart);

	// Internal dependency
	void ClearDepI(UINT depType);
	BOOL AddDepI(UINT depType, CString & strFile);
	BOOL AddDepI(UINT depType, FileRegHandle frh);
	BOOL RemoveDepI(UINT depType, FileRegHandle frh);

	// Internal input/output refreshers
	void RefreshInputI();
	void RefreshOutputI();

	// Internal add output (assumes ref. count is correct)
	BOOL AddOutputI(FileRegHandle frh, BOOL fWithAction = TRUE);

	// Add and remove the input (helpers)
	void AddInput();
	void RemoveInput();
	
	// Valid input file? ie. no in the output
	BOOL ValidInput(FileRegHandle frh);

	// COM support.
	CBSAction *pCBSAction;
	LPBSACTION pIAction;
};

// definition of the pages property used to view CActionSlob information
class CFileGridPage : public CProjPage
{
	DECLARE_MESSAGE_MAP()

public:
	CFileGridPage(UINT depType = DEP_Input);
	virtual ~CFileGridPage() {};

	virtual BOOL OnInitDialog();
	virtual void InitPage();
	virtual BOOL OnPropChange(UINT);

	// respond to selection changes in the tools listbox
	void OnToolsSelChange();

	// respond to selection changes in the file grid
	void OnFileGridSelChange();

private:
	// viewing what files?
	UINT m_depType;
};

class CInputsPage : public CFileGridPage
{	
	DECLARE_IDE_CONTROL_MAP();

public:
	CInputsPage() : CFileGridPage(DEP_Input) {}
};

class COutputsPage : public CFileGridPage
{
	DECLARE_IDE_CONTROL_MAP();

public:
	COutputsPage() : CFileGridPage(DEP_Output) {}
};

class CDepsPage : public CFileGridPage
{
	DECLARE_IDE_CONTROL_MAP();

public:
	CDepsPage() : CFileGridPage(DEP_Dependency) {}
};

extern CInputsPage g_InputsPage;
extern COutputsPage g_OutputsPage;
extern CDepsPage g_DepsPage;

// FUTURE - clean this CMD_ and ACT_ stuff up so the build engine is clearer.
// Make these enums (the type will then be int, which is what it should be anyway
// since that's what DoSpawn returns) and then update the return type on some of
// the build funcs (like ExecuteCmdLines)
// The same should be done to a lot of the #defines in this file.

// state of a command line execution
#define CMD				UINT
#define CMD_Canceled	(UINT)-2
#define CMD_Complete	(UINT)0	
#define CMD_Error		(UINT)1

// build engine errors

// can't get the command-line text for a command-line based tool
// -> context is a CBuildTool *
#define BldError_CantGetCmdLines	IDS_GET_COMMAND_LINE_FAILED
#define BldError_ScanError			IDS_SCAN_ERROR

// CBuildEngine debugging bits
#define DBG_BS_OFF		0x0		// (no debugging)
#define DBG_BS_GRP		0x1		// show graph ctor/dtor info

#define ENABLE_BS_GRP	0x10	// enable graph ctor/dtor 

// definition of the CBuildEngine class
class CFileDepGraph;
class COutputWinEC;

class BLD_IFACE CBuildEngine : public CObject
{
	DECLARE_DYNAMIC(CBuildEngine)

public:
	CBuildEngine();
 	virtual ~CBuildEngine();

	// \/ Command Tool Interface functions
	// Execute some command-lines
	CMD ExecuteCmdLines(CPtrList &, CDir &, BOOL fCheckForComSpec, BOOL fIgnoreErrors, CErrorContext & EC = g_DummyEC);

	// Batching command-lines
	// 1) FALSE->TRUE == start batching
	// 2) TRUE->FALSE == flush currently batched command-lines
	// Returns ExecuteCmdLines() error value if flushing command-lines in 2)
	CMD BatchCmdLines(BOOL fBatch = TRUE);
	CMD FlushCmdLines();
    void ClearCmdLines();

	// /\ Command Tool Interface functions 	
	
	// \/ Error Utilituy functions 	
	void LogBuildError(UINT idError, void * pvContext, CErrorContext & EC = g_DummyEC);
	// /\ Error Utility functions 	

	// \/ File Utility functions
	__inline BOOL CreateTempFile(CString str, CPath & path, CErrorContext & EC = g_DummyEC)
		{return TmpMaker.CreateTempFileFromString(str, path, EC);}

	__inline void NukeTempFiles(CErrorContext & EC)
		{TmpMaker.NukeFiles(EC);}

	// Return a character pointer to a memory mapped file (also close the file)
	BOOL OpenMemFile(FileRegHandle frh,
					 HANDLE & hMap,
					 const TCHAR * & pcFileMap, DWORD & dwFileSize,
					 CErrorContext & EC = g_DummyEC);

 	BOOL CloseMemFile(HANDLE & hMap, CErrorContext & EC = g_DummyEC);

	// Delete a file from the disk
	BOOL DeleteFile(FileRegHandle, CErrorContext & EC = g_DummyEC);

	// Form a command-line, may use a response file if too long
	BOOL FormCmdLine(CString & strToolExeName, CString & strCmdLine, CErrorContext & EC, BOOL fRspFileOk = TRUE);
	BOOL FormBatchFile(CString & strCmdLine, CErrorContext & EC);

	// /\ File Utility functions 

	// \/ Build instance (graph + registry) manipulation for items
	// Note, must have active config. set.
	CFileRegistry * GetRegistry(ConfigurationRecord *);
	CFileDepGraph * GetDepGraph(ConfigurationRecord *);

	// Set the graph mode for all dep graphs of this builder
	void SetGraphMode(UINT mode, CProject * pBuilder);

	// a build instance
	typedef struct
	{
		CFileRegistry * s_preg;		// registry
		CFileDepGraph * s_pgraph;	// dependency graph
	} SBldInst;

	SBldInst * AddBuildInst(ConfigurationRecord *);
	void RemoveBuildInst(ConfigurationRecord *);

	// /\ Build instance manipulation items

	// \/ Building of project
	//
	// 'pcrBuild' is the configuration of the build
	//
	// 'EC' is the destination of error messages during build
	//
	// 'frhStart' is the file to build (default is all target files in graph)
	//
	// 'aob' is the attributes of the build
	// o AOB_Default		- default
	// o AOB_IgnoreErrors	- ignore errors
	//
	// 'type' is the type of build
	// o TOB_Build			- build
	// o TOB_ReBuild		- rebuild
	//
	// 'statefilter' is the state to check for in BuildState()
	// 'lstFrh' is the list of files that match 'statefilter' approp.
	//
	// 'fVerbose' will cause the build to prompt, eg. 'settings changed' etc.
	BOOL HasBuildState
	(
		ConfigurationRecord * pcrBuild,	
		CPtrList & lstFrh,															  
		UINT statefilter = DS_OutOfDate,
		FileRegHandle frhStart = (FileRegHandle)NULL
	);

	CMD DoBuild
	(																  
		ConfigurationRecord * pcrBuild,
 		CErrorContext & EC,
		FileRegHandle frhStart = (FileRegHandle)NULL,
		DWORD aob = AOB_Default,
		UINT type = TOB_Build,
		BOOL fRecurse = TRUE,
		BOOL fVerbose = TRUE
	);
	// /\ Building of project

	__inline CErrorContext * OutputWinEC() {return (CErrorContext *)m_pECOutputWin;}

	// debugging options
	DWORD m_dwDebugging;

private:
	// internal
	CMD ExecuteCmdLinesI(CPtrList &, CDir &, BOOL fCheckForComSpec, BOOL fIgnoreErrors, CErrorContext & EC = g_DummyEC);

	// command-line interface batching
	// batched cmdline. lists
	CStringList		m_slCmds;
	CStringList		m_slDesc;

	CPtrList		m_plCmds;

	CDir			m_dirLast;
	BOOL			m_fLastCheckForComSpec;
	BOOL			m_fLastIgnoreErrors;
	BOOL			m_fBatchCmdLines;
	CErrorContext *	m_pECLast;

	// Build instance map
	CMapPtrToPtr m_mapBldInst;

	// an output window error context
	COutputWinEC * m_pECOutputWin;

	CTempMaker	TmpMaker;
	CPtrList	m_MemFiles;
};

// our *single* instance of the build engine
extern BLD_IFACE CBuildEngine g_buildengine;

extern BLD_IFACE CPtrList g_lsttargref;

typedef struct tagTARGREFINFO {
	CProjItem *	pTargRef;
	ConfigurationRecord *	pcr;
} TARGREFINFO;
 
#endif // _ENGINE_H_
