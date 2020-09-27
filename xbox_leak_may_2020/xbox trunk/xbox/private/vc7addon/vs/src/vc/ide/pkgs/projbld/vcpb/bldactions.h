#pragma once

#include "FileRegistry.h"
#include "bldhelpers.h"
#include "ConfigRecords.h"
#include "BldTool.h"
#include "bldspawner.h"
#include "BldActions.h"

// classes in this header
class CBldAction;
class CBldActionSets;
class CBldActionList;
class CBldActionListHolder;

// other classes we need
class CBldIncludeEntryList;

// definition of the CBldActionList class
class CBldActionList : 
	public IVCBuildActionList,
	public CComObjectRoot,
	protected CVCPtrList
{
public:
	CBldActionList() : CVCPtrList(1) {}
	~CBldActionList() 
	{ 
		Clear(); 
	}
BEGIN_COM_MAP(CBldActionList)
	COM_INTERFACE_ENTRY(IVCBuildActionList)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CBldActionList) 

	static HRESULT CreateInstance(CBldActionList** ppActionList);
	static HRESULT CreateInstance(IVCBuildActionList** ppActionList);
	static HRESULT CreateInstance(CBldActionList** ppActionList, IVCBuildActionList** ppVCActionList);

// IVCBuildActionList
public:
	STDMETHOD(Find)(IVCToolImpl* pTool, IVCBuildAction** ppAction);
	STDMETHOD(FindByName)(BSTR bstrToolName, IVCBuildAction** ppAction);
	STDMETHOD(get_Count)(long * pceltFetched);
	STDMETHOD(Clone)(IEnumVARIANT FAR* FAR* ppenum);
	STDMETHOD(Add)(IVCBuildAction* pAction);
	STDMETHOD(Next)(IVCBuildAction** ppAction, void** ppPos);
	STDMETHOD(Reset)(void** ppPos);
	STDMETHOD(RemoveAtPosition)(void* pPos);
	STDMETHOD(FreeOutputFiles)();
	STDMETHOD(get_Project)(VCProject** ppProject);
	STDMETHOD(get_LastAction)(IVCBuildAction** ppAction);
	STDMETHOD(get_ProjectConfigurationDependencies)(IVCBuildStringCollection** ppDependencies);	// dependencies of this action; i.e., libs, etc.
	STDMETHOD(get_ProjectConfigurationBuildOutputs)(IVCBuildStringCollection** ppOutputs);		// outputs of this action; i.e., foo.exe, foo.pdb, etc.
	STDMETHOD(RefreshOutputs)(long nPropID, IVCBuildErrorContext* pEC);
	STDMETHOD(HandlePossibleCommandLineOptionsChange)(IVCBuildErrorContext* pEC);

protected:	// IVCBuildActionList helpers
	VCPOSITION m_posEnumerator;

public:
	// find a tool and return the postion of the action
	VCPOSITION Find(CBldToolWrapper* pTool);
	void Clear(BOOL bDoRelease = TRUE);
	void AddToEnd(CBldAction* pAction, BOOL bDoAddRef = TRUE);
	void AddToEnd(CVCPtrList* pActionList, BOOL bDoAddRef = TRUE);
	void AddToEnd(CBldActionList* pActionList, BOOL bDoAddRef = TRUE);
	long Count() { return (long) GetCount(); }
	BOOL NotEmpty() { return GetCount() != 0; }
	VCPOSITION GetStart() { return GetHeadPosition(); }
	CBldAction* Next(VCPOSITION& pos) { return (CBldAction*)GetNext(pos); }
};

class CBldActionListHolder
{
public:
	CBldActionListHolder()
	{
		CBldActionList::CreateInstance(&m_pActions);
	}
	~CBldActionListHolder()
	{
		if (m_pActions)
		{
			IVCBuildActionList* pVCActions = (IVCBuildActionList*)m_pActions;
			pVCActions->Release();
		}
	}
	CBldActionList* m_pActions;
};

#include "DependencyGraph.h"

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
#define DEPSTATE	CBldIncludeEntryList *
#define DEP_Empty	((DEPSTATE)NULL)	// empty deps
#define DEP_None	((DEPSTATE)-1)		// forced to have no deps

// type and stage of the action to be performed

// action stage
#define TOB_Pre		0x0				// perform pre-action processing
#define TOB_Stage	0x1				// perform the action type
#define TOB_Post	0x2				// perform post-action processing

class CBldAction : 
	public CBldFileRegNotifyRx,
	public IVCBuildAction,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CBldAction)
	COM_INTERFACE_ENTRY(IVCBuildAction)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CBldAction) 

	// Associate this action to a particular project item. This project item's
	// configuration reocrd will be accessed to retrieve configuration specific
	// action information.
	// Associate this action with a particular tool. It will access the CBuildToolWrapper's
	// methods in order to modify the action's build information.
	CBldAction();
	~CBldAction();
	static HRESULT CreateInstance(VCFileConfiguration* pFileCfg, CBldToolWrapper* pTool, 
		CBldCfgRecord* pcr, BOOL bIsProject, BOOL bNoFilter, IVCBuildAction** ppBldAction);

protected:
	void Initialize(VCFileConfiguration* pFileCfg, CBldToolWrapper* pTool, CBldCfgRecord* pcr, BOOL bNoFilter, BOOL bIsProject);

// IVCBuildAction
public:
	STDMETHOD(MatchesOldTool)(IVCToolImpl* pTool, BOOL* pbMatches);
	STDMETHOD(get_VCToolImpl)(IVCToolImpl** pVal);
	STDMETHOD(RefreshOutputs)(long nPropID, IVCBuildErrorContext* pEC);
	STDMETHOD(HandlePossibleCommandLineOptionsChange)(IVCBuildErrorContext* pEC);
	STDMETHOD(get_Project)(VCProject** ppProject);
	STDMETHOD(get_FileConfiguration)(VCFileConfiguration** ppFileCfg);
	STDMETHOD(get_ProjectConfiguration)(VCConfiguration** ppProjCfg);
	STDMETHOD(GetOutput)(IVCBuildErrorContext* pEC, void** ppFileRegSet);
	STDMETHOD(get_Registry)(void** ppFileRegistry);
	STDMETHOD(get_ItemFilePath)(BSTR* pbstrFilePath);
	STDMETHOD(get_Item)(IVCBuildableItem** ppItem);
	STDMETHOD(get_PropertyContainer)(IVCPropertyContainer** ppPropContainer);
	STDMETHOD(get_ThisPointer)(void** ppThis);
	STDMETHOD(get_IsTargetAction)(VARIANT_BOOL* pbIsTargetAction);
	STDMETHOD(InitSets)();	// new up the dependency set structures
	STDMETHOD(LateBind)(IVCBuildErrorContext* pEC);
	// Used for figuring out filenames controlled by option flags, e.g. /MAP:foo.map 
	// 'nPathId' is the path prop, may or may not return fullpath, might be dir. only
	// 'fIsDirectoryLocation' says whether to force the path prop as directory
	// 'bstrDefExtension' is the new extension to use
	// 'pDirBase' is the directory to re-base the filename in
	// 'bstrDefFilename' is the new filename (if none supplied from nPathId) (default is use filebase)
	// 'nEnablePropId' is the enabling prop., if FALSE then no output added
	// 'fWithAction' if TRUE will associate the output with an action in the graph
	STDMETHOD(AddOutputFromID)(long nPathId, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, BSTR bstrDirBase, 
		BSTR bstrDefFilename, long nEnablePropId, VARIANT_BOOL fWithAction, IVCToolImpl* pTool);
	STDMETHOD(AddOutputInSubDirectory)(BSTR bstrSubDirectory, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, BSTR bstrDirBase, 
		BSTR bstrDefFilename, long nEnablePropId, VARIANT_BOOL fWithAction, long nErrPropID, IVCToolImpl* pTool);
    STDMETHOD(AddOutputFromDirectory)(BSTR bstrDirBase, IVCBuildErrorContext* pEC, long nEnablePropId, BSTR bstrExt, 
		long nErrPropID, IVCToolImpl* pTool);
	STDMETHOD(AddOutputFromPath)(BSTR bstrPath, IVCBuildErrorContext* pEC, VARIANT_BOOL fWithAction, VARIANT_BOOL fAllowFilter, 
		long nPropID, IVCToolImpl* pTool);
	STDMETHOD(AddOutputFromFileName)(BSTR bstrFileName, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, BSTR bstrDirBase, 
		VARIANT_BOOL fWithAction, long nErrPropID, IVCToolImpl* pTool);
	STDMETHOD(get_SourceDependencies)(void** ppFileRegSet);
	STDMETHOD(get_ScannedDependencies)(void** ppFileRegSet);
	STDMETHOD(AddSourceDependencyFromString)(BSTR bstrDep, IVCBuildErrorContext* pEC);
	STDMETHOD(AddSourceDependencyFromFrh)(void* pFileRegHandle, IVCBuildErrorContext* pEC);
	STDMETHOD(RemoveSourceDependencyForFrh)(void* pFileRegHandle, IVCBuildErrorContext* pEC);
	STDMETHOD(ClearSourceDependencies)(IVCBuildErrorContext* pEC);
	STDMETHOD(AddScannedDependencyFromString)(BSTR bstrDep, IVCBuildErrorContext* pEC);
	STDMETHOD(AddScannedDependencyFromFrh)(void* pFileRegHandle, IVCBuildErrorContext* pEC);
	STDMETHOD(AddScannedDependencyOfType)(int type, BSTR bstrFile, int line, BOOL fScan, IVCBuildErrorContext* pEC);
	STDMETHOD(ClearScannedDependencies)(IVCBuildErrorContext* pEC);
	STDMETHOD(SetDependencyState)(enumDependencyState depState);
	STDMETHOD(MarkMissingDependencies)(BOOL bHaveMissing);
	STDMETHOD(GetRelativePathsToInputs)(BOOL bFilterOutSpecial, BOOL bUseMultiLine, IVCBuildErrorContext* pEC, BSTR *pVal);
	STDMETHOD(UpdateDependencyInformationTimeStamp)();
	STDMETHOD(get_Dependencies)(IVCBuildStringCollection** ppDependencies);	// dependencies of this action; i.e., libs, headers, etc.
	STDMETHOD(ScanToolDependencies)(IVCToolImpl* pTool, IVCBuildErrorContext* pEC, VARIANT_BOOL bWriteOutput);
	STDMETHOD(get_BuildOutputs)(IVCBuildStringCollection** ppOutputs);		// outputs of this action; i.e., foo.exe, foo.pdb, foo.obj, etc.
	STDMETHOD(GetFileNameForFileRegHandle)(void* pRegHandle, BSTR* pbstrFileName);
	STDMETHOD(GetFileRegHandleForFileName)(BSTR bstrFileName, void** ppRegHandle);
	STDMETHOD(ReleaseRegHandle)(void* pRegHandle);
	STDMETHOD(GetDeploymentDependencies)(IVCBuildStringCollection** ppDeployDeps);

public:
	// Binding and un-binding to tools
	void Bind(CBldToolWrapper * pTool, IVCBuildErrorContext* pEC = NULL, BOOL bNoFilter = FALSE);	// UnBind and LateBind are above under the COM stuff
	BOOL IsInitialized(){ return m_pdepgraph->IsInitialized(); };

	//
	// Assigning/unassigning of actions to project items (default is active config.)
	//
	static void AssignActions(CBldCfgRecord* pcr, VCFileConfiguration* pFileCfg, BOOL fAssignContained = TRUE, 
		BOOL fAssignContainedOnly = FALSE);
	static void UnAssignActions(CBldCfgRecord* pcr, VCFileConfiguration* pFileCfg, BOOL fUnassignContained = TRUE);

	static void IgnoreCachedOptions(BOOL bIgnore)  { s_bIgnoreCachedOptions = bIgnore; }
	static BOOL s_bIgnoreCachedOptions;

	// Assoc. target level action (iff we are not one ourselves)
	CBldAction* GetTargetAction();

	// 
	// Options at last perform etc.
	//
	__inline void DirtyCommand() {m_fDirtyCommand = TRUE;}

	// Retrieve and refresh the command-line options
	__inline BOOL GetCommandOptions(CStringW & strOptions) {strOptions = m_strOptions; return TRUE;}
	BOOL RefreshCommandOptions(BOOL & fChanged, BOOL fNoSet = FALSE);
	BOOL HaveMissingDeps() { return m_fDepMissing; }

	void MakeFilesDirty();
	void CalcFileAttributes(BOOL fOn = TRUE);

	//
	// Generate outputs
	//

	// Use when just a simple file reg. handle
	__inline HRESULT AddOutput(BldFileRegHandle frh, IVCBuildErrorContext* pEC, BOOL fWithAction = TRUE)
		{frh->AddFRHRef(); return AddOutputI(frh, pEC, fWithAction);}
	HRESULT FinishAddOutputFromID(CStringW& rstrFileName, IVCBuildErrorContext* pEC, BOOL fIsDirectoryLocation, 
		BSTR bstrDefExtension, CDirW& dirBase, BSTR bstrDefFilename, BOOL fWithAction, long nErrPropID, IVCToolImpl* pTool);

	static CComBSTR s_bstrDefVCFile;
	static LPCOLESTR s_szDefVCFile;

	// Used when path is already known
	HRESULT AddOutput(const CPathW* pPath, IVCBuildErrorContext* pEC, BOOL fWithAction = TRUE, BOOL fAllowFilter = TRUE, 
		long nPropID = -1, IVCToolImpl* pTool = NULL);
	HRESULT AddOutputFromFileName(CStringW& rstrFileName, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, CDirW* pDirBase, 
		BOOL fWithAction, BOOL fMakeItRelativeToProject, long nErrPropID, IVCToolImpl* pTool);


	// retrieve our 'actual' input(s)
	CBldFileRegSet * GetInput(IVCBuildErrorContext* pEC);
	__inline void DirtyInput(BOOL fDirty = TRUE) {m_fDirtyInput = fDirty;}
	void ClearInput();

	// retrieve our list of outputs (list of file registry handles)
	CBldFileRegSet * GetOutput(IVCBuildErrorContext* pEC);
	CVCWordArray * GetOutputAttrib();
	__inline void DirtyOutput(BOOL fDirty = TRUE) {m_fDirtyOutput = fDirty;}
	void ClearOutput(IVCBuildErrorContext* pEC);

	__inline CBldToolWrapper* BuildToolWrapper()	{ return m_pTool; }

protected:
	void FindDeletions(CBldFileRegSet& Deletions, IVCBuildErrorContext* pEC);
	void HandleDeletions(CBldFileRegSet& Deletions, IVCBuildErrorContext* pEC);

	//
	// Scanned dependencies
	//
public:
	__inline CBldIncludeEntryList * GetIncludeDep() {return m_pIncludeList;}
	BOOL ScanDependencies(IVCBuildErrorContext* pEC, BOOL bUseAnyMethod = FALSE, BOOL bWriteOutput = TRUE);

	CBldFileRegSet * GetScannedDep();
	__inline void ClearScannedDep(IVCBuildErrorContext* pEC = NULL) {ClearDepI(DEP_Scanned, pEC);}

	// add scanned dependency (unresolved)
	BOOL AddScannedDep(int type, IVCBuildErrorContext* pEC, CStringW & strFile, int nLine = 0, BOOL fScan = TRUE);

	// add/remove scanned dependency (resolved)
	__inline BOOL AddScannedDep(CStringW & strFile, IVCBuildErrorContext* pEC) {return AddDepI(DEP_Scanned, strFile, pEC);}
	__inline BOOL AddScannedDep(BldFileRegHandle frhDep, IVCBuildErrorContext* pEC) {return AddDepI(DEP_Scanned, frhDep, pEC);}

	//
	// Source dependencies
	//
	CBldFileRegSet * GetSourceDep();
	__inline void ClearSourceDep(IVCBuildErrorContext* pEC = NULL) {ClearDepI(DEP_Source, pEC);}

	// add/remove source dependency
 	__inline BOOL AddSourceDep(CStringW & strFile, IVCBuildErrorContext* pEC) {return AddDepI(DEP_Source, strFile, pEC);}
	__inline BOOL AddSourceDep(BldFileRegHandle frhDep, IVCBuildErrorContext* pEC) {return AddDepI(DEP_Source, frhDep, pEC);}
	__inline BOOL RemoveSourceDep(BldFileRegHandle frhDep, IVCBuildErrorContext* pEC) {return RemoveDepI(DEP_Source, frhDep, pEC);}

	__inline void SetDepState(DEPSTATE stateDep)
		{m_pIncludeList = stateDep;}

	//
	// Notification of input changing
	//
 
	// Rx for file change events
	virtual void OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint);

	// Batched notification?
	__inline void BatchNotify(BOOL fBatch) {m_fBatchNotify = fBatch;}

	// FUTURE: make private (currently used to Refresh() etc.)
	// Internal helper to set tool
	void SetToolI(CBldToolWrapper* pTool, BOOL bNoFilter = FALSE);

	BOOL IsProject() { return (m_fIsProject != 0); }

protected:
	void LoadDepArray(CBldFileRegSet* pDeps, CVCStringWArray& rstrDepsArray);
	HRESULT GetBuildEngineImplFromEC(IVCBuildErrorContext* pEC, IVCBuildEngineImpl** ppBldEngineImpl);
	void UnBind();

// some nice helpers
public:
	static HRESULT GetFileCollection(VCConfiguration* pProjCfg, IEnumVARIANT** ppFiles);
	static BOOL GetFileCfgForProjCfg(CComVariant& var, VCConfiguration* pProjCfg, VCFileConfiguration** ppFileCfg);
	static BOOL GetFileCfgActions(VCFileConfiguration* pFileCfg, IVCBuildActionList** ppActions);
	static void InformUserAboutBadFileName(IVCBuildErrorContext* pEC, IVCPropertyContainer* pPropContainer, IVCToolImpl* pTool, 
		long nErrPropID, CStringW& strFile);

private:
	static void BindSingleAction(CBldConfigurationHolder& cfgHolder);
	static void UnBindSingleAction(CBldConfigurationHolder& cfgHolder);
	static void AssignSingleAction(CBldConfigurationHolder& cfgHolder);
	static void UnAssignSingleAction(CBldConfigurationHolder& cfgHolder);

public:
	// Current action mark to use
	static WORD m_wActionMarker;

	// Associated project cfg(s) and tool (+ tool user-data)
	CBldCfgRecord* m_pcrOur;
	IVCBuildableItem* m_pItem;	// intentionally not ref-conted; should have same or lesser lifetime than associated item
	CBldToolWrapper* m_pTool;
	CBldToolWrapper* m_pOldTool;

	// Build process information
	CBldFileRegistry* m_pregistry;	// registry our build files are in
	CBldFileDepGraph* m_pdepgraph;	// build process graph

	// Used as mark during dep. graph analysis and build process
	WORD m_wMark;

	// Used during command-options refresh
	WORD m_wMarkRefCmdOpt;

	// Input file or files (filter) 'possibly' consumed by this action
	BldFileRegHandle m_frhInput;

	//  All bitfields are intentionally placed here to get best packing

	BOOL m_fPossibleOptionChange:1;
	// Used during command-options refresh
	BOOL m_fCmdOptChanged:1;
	// Used during input dep generation
	BOOL m_fDepMissing:1;

private:
	BOOL m_fDirtyInput:1;	// Dirty input?
	BOOL m_fDirtyOutput:1;	// Dirty output?
	BOOL m_fDirtyCommand:1;	// Dirty command-line?
	BOOL m_fBatchNotify:1;	// Batched input/output notify? .. ignore if TRUE
	BOOL m_fInGenOutput:1;	// Updating output?
	BOOL m_fIsProject:1;	// is this a project?

	CBldAction* m_pTrgAction;	// Assoc. target level action (iff we are not one ourselves)

	CStringW m_strOptions;

	CBldActionSets* m_pSets;

//REVIEW : good candidates for the CAction Sets class
	// Attributes of the output
	// Currently non-zero == 'has action', zero == 'no action'
	CVCWordArray* m_paryOldOutputAttrib;

	// Scanned dependencies
	CBldIncludeEntryList* m_pIncludeList;		// unresolved

	//
	// Time of last dependency update on this CBldAction.
	// Zero if uninitialized.
	//
	FILETIME m_LastDepUpdate;

private:
	static int s_nMyMarker;

public:
	//
	// Update dependency stuff.
	//
	BOOL IsDepInfoInitialized();
	BOOL IsNewDepInfoAvailable(IVCBuildErrorContext* pEC);

	//
	// Update dependency information if new information is available.
	// Set bForceUpdate = TRUE to force update if updating has been turned off
	// minimal rebuild info is not available.
	BOOL UpdateDepInfo(IVCBuildErrorContext* pEC, BOOL bForceUpdate = FALSE);

	void UpdateDepInfoTimeStamp();
private:
	static BOOL IsBuildableConfiguration(IVCConfigurationImpl* pProjCfgImpl);
	static BOOL IsValidConfiguration(IVCConfigurationImpl* pProjCfgImpl);
	BOOL GetStringProperty(long nPropID, CStringW& rstrValue, BOOL bEvaluate = FALSE);
	void GetItemFileName(CStringW& rstrName);

	// Internal dependency
	void ClearDepI(UINT depType, IVCBuildErrorContext* pEC);
	BOOL AddDepI(UINT depType, CStringW & strFile, IVCBuildErrorContext* pEC);
	BOOL AddDepI(UINT depType, BldFileRegHandle frh, IVCBuildErrorContext* pEC);
	BOOL RemoveDepI(UINT depType, BldFileRegHandle frh, IVCBuildErrorContext* pEC);

	// Internal input/output refreshers
	void RefreshOutputI(IVCBuildErrorContext* pEC);

	// Internal add output (assumes ref. count is correct)
	HRESULT AddOutputI(BldFileRegHandle frh, IVCBuildErrorContext* pEC, BOOL fWithAction = TRUE);
	BOOL EnabledOutput(long nEnablePropId);

	// Add and remove the input (helpers)
	void AddInput(BOOL bNoFilter = FALSE);
	void RemoveInput();
	
	// Valid input file? ie. no in the output
	BOOL ValidInput(BldFileRegHandle frh);
};


class CBldActionSets
{
public:
	CBldActionSets(CBldAction *pAction): m_frsInput(37), m_frsOutput(7), m_frsSchmoozableOutput(7)
	{
		// set up our input and output to use this target registry
		m_frsSchmoozableOutput.SetFileRegistry(pAction->m_pregistry);
		// FUTURE: For now we don't care about the incr. update
		// of input and output properties, eg. timestamp, exist on disk etc.

		// This was the review comment but I don't really understand these sets
		// completely and there is no way in hell I am changing anything about this
		// two days before building the release candidate. I am changing this to FUTURE
		// and we should understand this review comment and fix if necessary for 4.1
		m_frsInput.CalcAttributes(FALSE);
		m_frsOutput.CalcAttributes(FALSE);
		m_frsSchmoozableOutput.CalcAttributes(FALSE);
	}

	void MakeFilesDirty(CBldFileRegistry* pRegistry)
	{
		m_frsInput.MakeContentsDirty(pRegistry);
		m_frsOutput.MakeContentsDirty(pRegistry);
		m_frsSchmoozableOutput.MakeContentsDirty(pRegistry);
		m_frsScannedDep.MakeContentsDirty(pRegistry);
		m_frsSourceDep.MakeContentsDirty(pRegistry);
	}

	void CalcFileAttributes(BOOL fOn = TRUE)
	{
		m_frsInput.CalcAttributes(fOn);
		m_frsOutput.CalcAttributes(fOn);
		m_frsSchmoozableOutput.CalcAttributes(fOn);
		m_frsScannedDep.CalcAttributes(fOn);
		m_frsSourceDep.CalcAttributes(fOn);
	}

	// one bit per output (IsPrimary ?) 
	CVCWordArray m_aryOutputAttrib;
	
	// Temp. 'actual' input file set
	CBldFileRegSet m_frsInput;

	// Output files generated by this action
	CBldFileRegSet m_frsOutput;

	// Output files that can be used as input to schmooze tools.
	CBldFileRegSet m_frsSchmoozableOutput;

	CBldFileRegSet *m_pfrsOldOutput;

	CBldFileRegSet m_frsScannedDep;			// resolved (in the graph...)

	// Source dependencies
	CBldFileRegSet m_frsSourceDep;			// resolved (in the graph...)
};

