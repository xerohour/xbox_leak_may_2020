#ifndef __BUILDENGINE_H__
#define __BUILDENGINE_H__

#pragma once

#include "FileRegistry.h"
#include "bldhelpers.h"
#include "ConfigRecords.h"
#include "BldTool.h"
#include "bldspawner.h"
#include "BldActions.h"
#include "vcbuild.h"            // built by ..\idl\vcbuild.idl
#include "encbldthrd.h"
#include "scanner.h"

// classes in this header
class CStaticBuildEngine;
class CDynamicBuildEngine;
class CBldHoldRegRecalcs;

// FUTURE - clean this CMD_ and ACT_ stuff up so the build engine is clearer.
// Make these enums (the type will then be int, which is what it should be anyway
// since that's what DoSpawn returns) and then update the return type on some of
// the build funcs (like ExecuteCmdLines)
// The same should be done to a lot of the #defines in this file.

// CStaticBuildEngine debugging bits
#define DBG_BS_OFF		0x0		// (no debugging)
#define DBG_BS_GRP		0x1		// show graph ctor/dtor info

#define ENABLE_BS_GRP	0x10	// enable graph ctor/dtor 

class CStaticBuildEngine
{
public:
	CStaticBuildEngine();
	virtual ~CStaticBuildEngine();
	void Close();

	// a build instance
	typedef struct
	{
		CBldFileRegistry* s_preg;		// registry
		CBldFileDepGraph* s_pgraph;	// dependency graph
	} SBldInst;

	SBldInst* AddBuildInst(CBldCfgRecord* pCfgRecord);
	void RemoveBuildInst(CBldCfgRecord* pCfgRecord);

	// \/ Build instance (graph + registry) manipulation for items
	CBldFileRegistry* GetRegistry(VCProject* pProject, VCConfiguration* pCfg);
	CBldFileRegistry* GetRegistry(CBldCfgRecord* pCfgRecord);
	CBldFileDepGraph* GetDepGraph(VCProject* pProject, VCConfiguration* pCfg);
	CBldFileDepGraph* GetDepGraph(CBldCfgRecord* pCfgRecord);

	CBldCfgRecord* GetCfgRecord(IDispatch* pProject, IDispatch* pCfg);
	void RemoveCfgRecord(IDispatch* pProject, IDispatch* pCfg);
	CBldProjectRecord* GetProjectRecord(IDispatch* pProject, BOOL bCreateIfNeeded = TRUE);
	void RemoveProjectRecord(IDispatch* pProject);

	// Set the graph mode for all dep graphs of this project
	void SetGraphMode(UINT mode, IDispatch* pProject);

	// Return a character pointer to a memory mapped file (also close the file)

	// NOTE: That this function is explicitly MBCS, because source code is MBCS ( yes, ANSI )
	// 	  We may / will need to rewrite when we start supporting UTF8/Unicode source code.
	BOOL OpenMemFile(BldFileRegHandle frh, HANDLE& hMap, const char *& pcFileMap, DWORD& dwFileSize,
		IVCBuildErrorContext* pEC = NULL, IVCBuildAction* pAction = NULL);
 	BOOL CloseMemFile(HANDLE& hMap, IVCBuildErrorContext* pEC = NULL);

	// Delete a file from the disk
	BOOL DeleteFile(BldFileRegHandle, IVCBuildErrorContext* pEC = NULL, VCConfiguration* pConfig = NULL,
		IVCBuildActionList* pActionList = NULL);

private:
	// Build instance map
	CVCMapPtrToPtr m_mapBldInst;

	CBldSolutionRecord m_SolutionRecord;
	CVCPtrList	m_MemFiles;

public:
	// debugging options
	DWORD m_dwDebugging;

public:
	int GetNextItem(CStringW& strList, int nNextIdx, int nMaxIdx, CStringW& strSeparator, CStringW& strItem);	// helper for traversing strSeparator delimited lists
	int FindFirstSeparator(CStringW& strList, CStringW& strSeparatorList, int nStartIdx);

protected:
	int GetNextQuotedItem(CStringW& strList, int nStartIdx, int nMaxIdx, int nSep, int nLastQuote, BOOL bOddQuote, 
		CStringW& strSeparator, CStringW& strItem);	// helper for GetNextItem
};

class CDynamicBuildEngine : 
	public IDispatchImpl<IVCBuildEngine, &IID_IVCBuildEngine, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCBuildEngineImpl,
    public IENCBuildSpawner,
	public CComObjectRoot
{
public:
	CDynamicBuildEngine();
	~CDynamicBuildEngine() {}
	static HRESULT CreateInstance(IVCBuildEngine** ppEngine);

BEGIN_COM_MAP(CDynamicBuildEngine)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCBuildEngine)
	COM_INTERFACE_ENTRY(IVCBuildEngineImpl)
	COM_INTERFACE_ENTRY(IENCBuildSpawner)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CDynamicBuildEngine) 

// IVCBuildEngine
public:
	STDMETHOD(get_SpawnerActive)(VARIANT_BOOL* pbSpawnerActive);
	STDMETHOD(get_CanSpawnBuild)(VARIANT_BOOL* pbCanSpawn);
	STDMETHOD(get_UserStoppedBuild)(VARIANT_BOOL *pVal);
	STDMETHOD(get_HaveLogging)(VARIANT_BOOL *pVal);
	STDMETHOD(LogTrace)(int fFlags, BSTR bstrMessage);
	STDMETHOD(get_CurrentLogSection)(eLogSectionTypes* peSection);
	STDMETHOD(put_CurrentLogSection)(eLogSectionTypes eSection);
	STDMETHOD(FormCommandLine)(BSTR bstrToolExeName, BSTR bstrCmdLineIn, IVCBuildErrorContext* pEC, BOOL fRspFileOK, 
		BOOL fLogCommand, BSTR* pbstrCmdLineOut);
	STDMETHOD(FormBatchFile)(BSTR bstrCmdLineIn, IVCBuildErrorContext* pEC, BSTR* pbstrCmdLineOut);
	STDMETHOD(FormTempFile)(BSTR bstrCmdLineIn, BSTR bstrComment, BSTR* pbstrCmdLineOut);
	STDMETHOD(FormXMLCommand)(BSTR bstrCmdLineIn, BSTR bstrExe, BSTR bstrExtraArgs, BSTR* pbstrCmdLineOut);
	STDMETHOD(ExecuteCommandLines)(IVCCommandLineList* pCmds, BSTR bstrDir, VARIANT_BOOL fCheckForComSpec, 
		VARIANT_BOOL fIgnoreErrors, IVCBuildErrorContext* pEC, CMD* pCmdRet);
	STDMETHOD(AddMissingEnvironmentVariable)(BSTR bstrMissingVar);
	STDMETHOD(ClearPendingEnvironmentVariableWarnings)();
	STDMETHOD(AddToOutputDirectoryList)(BSTR bstrDir);
	STDMETHOD(StopBuild)();
	STDMETHOD(get_ReuseLogFile)(VARIANT_BOOL* pbReuse);
	STDMETHOD(put_ReuseLogFile)(VARIANT_BOOL bReuse);

public:
	BOOL m_bUserCanceled;
	BOOL m_bUserInformedAboutCancel;

// IVCBuildEngineImpl
public:
	STDMETHOD(get_BuildThread)(IVCBuildThread** ppThread);
	STDMETHOD(get_ErrorContext)(IVCBuildErrorContext** ppErrContext);
	STDMETHOD(put_ErrorContext)(IVCBuildErrorContext* pErrContext);
	STDMETHOD(get_DoIdleAndPump)(BOOL* pbDoIdleAndPump);
	STDMETHOD(put_DoIdleAndPump)(BOOL bIdleAndPump);
	STDMETHOD(OpenProjectConfigurationLog)(VCConfiguration *pProjCfg);
	STDMETHOD(CloseProjectConfigurationLog)();
	STDMETHOD(DoBuild)(void* pcrBuild, BOOL bIsProjectBuild, IVCBuildErrorContext* pEC, void* frhStart, bldAttributes aob, 
		bldActionTypes type, BOOL fRecurse, IVCBuildAction* pAction, BuildResults* pbrResults);
	STDMETHOD(HasBuildState)(void* pcrBuild, IVCBuildErrorContext* pEC, VARIANT_BOOL* pbUpToDate);
	STDMETHOD(get_ScannerCache)(void ** ppScannerCache);
	STDMETHOD(InitializeSpawner)(BOOL bHidden);
	STDMETHOD(TerminateSpawner)();
	STDMETHOD(SpawnBuild)(IVCCommandLineList* pCmds, LPCOLESTR szDir, BOOL fCheckForComspec, BOOL fIgnoreErrors, 
		IVCBuildErrorContext* pEC, BOOL fAsyncSpawn, long* pnRetVal);
	STDMETHOD(InitializeForBuild)();
	STDMETHOD(get_LogFile)( BSTR *bstrLogFile );
	STDMETHOD(GetDeploymentDependencies)(void* pcrBuild, IVCBuildStringCollection** ppDeployStrings);

// IENCBuildSpawner
public:
	STDMETHOD(StartSpawn)(LPCOLESTR szCurrentDir, LPCOLESTR szCmdLine, IVCBuildErrorContext *pIVCBuildErrorContext, IENCBuildSpawnerCallback *pCallback, BOOL fUseConsoleCP);
	STDMETHOD(StopSpawn)();
	STDMETHOD(IsSysHeader)(LPCOLESTR szIncFileName);

// helper for multi-threading build cancel
	void InformUserCancel();

// helpers for IVCBuildEngine
protected:
	BOOL HaveLogging();
	void AddBuildEventTool(IVCBuildActionList* pBuildActions, LPOLESTR szBuildEventToolShortName, 
		CBldActionListHolder& lstActions);
	void AddNMakeTool(IVCBuildActionList* pBuildActions, CBldActionListHolder& lstActions);
	HRESULT DoWildCardCleanup(VCConfiguration* pConfig, bldActionTypes type, IVCBuildErrorContext* pEC);

	// Form a command-line, may use a response file if too long
	BOOL FormCmdLine(CStringW& strToolExeName, CStringW& strCmdLine, IVCBuildErrorContext* pEC, BOOL fRspFileOk = TRUE, 
		BOOL fLogCommand = TRUE);
	BOOL FormBatchFile(CStringW& strCmdLine, IVCBuildErrorContext* pEC);
	BOOL FormTempFile(CStringW& strCmdLine, CStringW& strComment);
	BOOL FormXMLCommand(CStringW& strCmdLine, CStringW& strExtraArgs, CStringW& strExe);

	// Batching command-lines
	// 1) FALSE->TRUE == start batching
	// 2) TRUE->FALSE == flush currently batched command-lines
	// Returns ExecuteCmdLines() error value if flushing command-lines in 2)
	CMD BatchCmdLines(BOOL fBatch = TRUE);
	CMD FlushCmdLines();
    void ClearCmdLines();

	// /\ File Utility functions 

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
	BuildResults DoBuild(CBldCfgRecord* pcrBuild, BOOL bIsProjectBuild, IVCBuildErrorContext* pEC,
		BldFileRegHandle frhStart = (BldFileRegHandle)NULL, bldAttributes aob = AOB_Default, 
		bldActionTypes type = TOB_Build, BOOL fRecurse = TRUE, IVCBuildAction* pAction = NULL);
	// /\ Building of project

	void GetDeploymentDependencies(CBldCfgRecord* pcrBuild, IVCBuildStringCollection** ppDeployStrings);

// IVCBuildEngineImpl
protected:
	BOOL m_bDoIdleAndPump;
	BOOL m_bReuseLogFile;

	// stuff for build logging
	void CDECL LogTrace(eLogSectionTypes logSection, LPCOLESTR szBuffer);
	void FlushLogSection(eLogSectionTypes logSection);
	void SetCurrentLogSection(eLogSectionTypes logSection);
	eLogSectionTypes GetCurrentLogSection();

	// \/ Command Tool Interface functions
	// Execute some command-lines
	CMD ExecuteCmdLines(IVCCommandLineList* pCmds, CStringW& rstrDir, BOOL fCheckForComSpec, BOOL fIgnoreErrors, 
		IVCBuildErrorContext* pEC = NULL);

	// /\ Command Tool Interface functions 	
	
	__inline void NukeTempFiles(IVCBuildErrorContext* pEC)
		{ TmpMaker.NukeFiles(pEC); }

// some helpers
public:
	CBldCfgRecord* GetAssociatedCfgRecord() { return m_pCfgRecord; }
	static UINT GetCodePage(BOOL bWantConsoleCP = TRUE);

private:
	// internal
	CMD ExecuteCmdLinesI(IVCCommandLineList*, CDirW&, BOOL fCheckForComSpec, BOOL fIgnoreErrors, 
		IVCBuildErrorContext* pEC = NULL);
	BuildResults ConvertFromCMDToBuildResults(CMD cmd);
	void DeleteOneFile(WIN32_FIND_DATAW& findWild, CStringW& strWild, CDirW& dirIntDir, IVCBuildErrorContext* pEC,
		VCConfiguration* pConfig);

	void FlushSingleLogSection(CVCStringWList* pLogList);
	CVCStringWList* FindLogBuffer(eLogSectionTypes logSection, BOOL bAdvanceOne = FALSE, 
		eLogSectionTypes* pNewLogSection = NULL);

private:
	FILE *m_pLogFile;
	CStringW m_strLogFile;
	int m_cReuseLogFile;
	BOOL m_bFirstUsage;
	eLogSectionTypes m_logCurrentSection;
	CVCStringWList m_logHeaderLines;
	CVCStringWList m_logEnvironmentLines;
	CVCStringWList m_logCommandLines;
	CVCStringWList m_logOutputLines;
	CVCStringWList m_logResultsLines;
	CVCStringWList m_logFooterLines;
	CBldSpawner m_Spawner;	// our spawner

	CVCMapStringWToPtr m_mapMissingEnvVars;
	CVCMapStringWToPtr m_mapMissingEnvVarsNoCase;
	CVCMapStringWToPtr m_mapOutputDirs;
	int m_actualErrs;
	int m_warns;

	BOOL		m_fBatchCmdLines;
	CTempMaker	TmpMaker;

	// an output window error context
	CComPtr<IVCBuildErrorContext> m_spBuildErrorContext;
	CComPtr<IVCCommandLineList>	m_spCmds;
	CBldCfgRecord*	m_pCfgRecord;

	CDirW			m_dirLast;
	BOOL			m_fLastCheckForComSpec;
	BOOL			m_fLastIgnoreErrors;
	IVCBuildErrorContext*	m_pECLast;

	CBldScannerCache m_ScannerCache;
	static UINT s_iConsoleCodePage;
	static UINT s_iANSICodePage;

// IENCBuildSpawner
protected:
    CEncBldThrd m_encBldThread;
};

class CBldHoldRegRecalcs
{
public:
	enum enumRegSet { eNullRegSet, eScannedRegSet, eSourceRegSet };
	CBldHoldRegRecalcs(IVCBuildAction* pAction, enumRegSet eSet);
	~CBldHoldRegRecalcs();
protected:
	CBldFileRegSet* m_pRegSet;
	BOOL m_fOldOn;
};

extern CStaticBuildEngine g_StaticBuildEngine;
extern BOOL g_bInProjClose;			// project close in progress
extern BOOL g_bInStyleClose;		// style sheet close in progress
extern BOOL g_bInStyleLoad;			// style sheet load in progress
extern BOOL g_bInFileChangeMode;	// file is being changed (added, removed, etc.)

#endif	// __BUILDENGINE_H__
