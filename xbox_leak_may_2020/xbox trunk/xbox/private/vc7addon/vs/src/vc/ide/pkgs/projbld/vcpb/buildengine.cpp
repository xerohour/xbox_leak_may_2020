#include "stdafx.h"
#pragma hdrstop
#include "BuildEngine.h"
#include "msgboxes2.h"
#include "scanner.h"
#include "BldThread.h"
#include <mbctype.h>
#include "EnvDaemon.h"
#include "BldEventTool.h"
#include "MRDependencies.h"
#include "util2.h"
#include "register.h"
#include "xmlfile.h"

BOOL g_bInProjClose = FALSE;		// project close in progress
BOOL g_bInProjLoad = FALSE;			// project load in progres
BOOL g_bInStyleClose = FALSE;		// style sheet close in progress
BOOL g_bInStyleLoad = FALSE;		// style sheet load in progress
BOOL g_bInFileChangeMode = FALSE;	// file is being changed (added, removed, etc.)

CTestableSection g_sectionMemFiles;
CTestableSection g_sectionCfgRecords;

#ifdef	DIANEME_DEP_CHECK_DIAGNOSTICS
static int s_nCurrentBuild = 0;
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

typedef struct tagMEMFILE
{
	HANDLE	        hFile;
	HANDLE	        hMap;
	const char *	pcFileMap;
} MEMFILE;			  

// our instance of the static build engine
CStaticBuildEngine g_StaticBuildEngine;		// build engine housekeeping stuff

CBldHoldRegRecalcs::CBldHoldRegRecalcs(IVCBuildAction* pAction, enumRegSet eSet)
{
	m_pRegSet = NULL;

	switch (eSet)
	{
	case eScannedRegSet:
		pAction->get_ScannedDependencies((void **)&m_pRegSet);
		break;
	case eSourceRegSet:
		pAction->get_SourceDependencies((void **)&m_pRegSet);
		break;
	case eNullRegSet:	// want ability to have this object do no work
		m_pRegSet = NULL;
		break;
	default:
		VSASSERT(FALSE, "No default state for which regset to hold recalculations for...");
		return;
	}

	if (m_pRegSet)
		m_fOldOn = m_pRegSet->CalcAttributes(FALSE);
}

CBldHoldRegRecalcs::~CBldHoldRegRecalcs()
{
	if (m_pRegSet)
		m_pRegSet->CalcAttributes(m_fOldOn);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CStaticBuildEngine
//		This class handles all the solution-wide, more or less static bookkeeping stuff
//		for building.
//
//		There is only one instance of this class: g_StaticBuildEngine
/////////////////////////////////////////////////////////////////////////////////////////////
CStaticBuildEngine::CStaticBuildEngine()
{
#ifdef DIANEME_TODO	// CStaticBuildEngine::CStaticBuildEngine; debugging reg key
#ifdef _DEBUG
	DWORD dwDbg = 0;

	// construct our registry key
	CStringW strKeyName(::GetRegistryKeyNameW());


	strKeyName += L"\\";
	strKeyName += szRegKey;

	HKEY hKey;
	// load the '\\Debugging' key
	if (RegOpenKeyExW(HKEY_CURRENT_USER, strKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwValueSize = sizeof(DWORD);
		if (RegQueryValueExW(hKey, L"Debugging", NULL, &dwType, (LPBYTE)&dwDbg, &dwValueSize) == ERROR_SUCCESS)
		{
		  	// make sure we get the value we expect
			VSASSERT(dwType == REG_DWORD, "Unexpected registry value type for Debugging key");
			VSASSERT(dwValueSize == sizeof(DWORD), "Unexpected registry value type for Debugging key");
		}

		RegCloseKey(hKey); // close the key we just opened
	} 

#endif
#endif	// DIANEME_TODO -- CStaticBuildEngine::CStaticBuildEngine

#ifdef _DEBUG
#ifdef DIANEME_TODO // CStaticBuildEngine::CStaticBuildEngine; debugging reg key
	m_dwDebugging = dwDbg;
#else	// DIANEME_TODO -- CStaticBuildEngine::CStaticBuildEngine
	m_dwDebugging = 0;
#endif	// DIANEME_TODO -- CStaticBuildEngine::CStaticBuildEngine
#else // _DEBUG
	m_dwDebugging = 0;	// off
#endif // !_DEBUG
}

CStaticBuildEngine::~CStaticBuildEngine()
{
	VSASSERT(m_MemFiles.IsEmpty(), "Failed to empty out the collection of mem files before closing down build system");
}

void CStaticBuildEngine::Close()
{
	// Prevent access to cfg record list by other threads.
	CritSectionT cs(g_sectionCfgRecords);

	VCPOSITION pos;
	pos = m_mapBldInst.GetStartPosition();
	while (pos)
	{
		SBldInst* pbldinst;
		CBldCfgRecord* pCfgRecord;
		void *pItem;

		m_mapBldInst.GetNextAssoc(pos, (void*&)pCfgRecord, pItem);
		pbldinst = (SBldInst *)pItem;
		if (pbldinst)
		{
			delete pbldinst->s_pgraph;
			pbldinst->s_preg->EmptyContent();
			delete pbldinst->s_preg;
			delete pbldinst;
		}
		m_mapBldInst.RemoveKey((void *)pCfgRecord);
	}
	m_SolutionRecord.Close();
}

CStaticBuildEngine::SBldInst* CStaticBuildEngine::AddBuildInst(CBldCfgRecord* pCfgRecord)
{
	// Prevent access to cfg record list by other threads.
	CritSectionT cs(g_sectionCfgRecords);

	SBldInst* pbldinst;

	if (m_mapBldInst.Lookup(pCfgRecord, (void *&)pbldinst))
		return pbldinst;	// already got one!

	// add a build instance for the currently active config.

	// allocate
	pbldinst = new SBldInst;
	pbldinst->s_preg = new CBldFileRegistry(pCfgRecord);
	pbldinst->s_pgraph = new CBldFileDepGraph(pbldinst->s_preg);

	// set in our lookup map
	m_mapBldInst.SetAt((void *)pCfgRecord, (void *)pbldinst);

	return pbldinst;
}

void CStaticBuildEngine::RemoveBuildInst(CBldCfgRecord* pCfgRecord)
{
	// Prevent access to cfg record list by other threads.
	CritSectionT cs(g_sectionCfgRecords);

	// remove a build instance for the currently active config.
	SBldInst* pbldinst = NULL;
	if (m_mapBldInst.Lookup((void *)pCfgRecord, (void *&)pbldinst) && pbldinst)
	{
		// de-allocate
		delete pbldinst->s_pgraph;
		delete pbldinst->s_preg;
		delete pbldinst;
	}

	// remove from our lookup map
	m_mapBldInst.RemoveKey((void *)pCfgRecord);
}

CBldFileRegistry* CStaticBuildEngine::GetRegistry(CBldCfgRecord* pCfgRecord)
{
	// get build instance for specified project/config/toolset combination
	// if one doesn't exist, create one on demand
	SBldInst* pbldinst = AddBuildInst(pCfgRecord);

	return pbldinst->s_preg;
}

CBldFileDepGraph* CStaticBuildEngine::GetDepGraph(CBldCfgRecord* pCfgRecord)
{
	// get build instance for specified project/config/toolset combination
	// if one doesn't exist, create one on demand
	SBldInst* pbldinst = AddBuildInst(pCfgRecord);

	return pbldinst->s_pgraph;
}

CBldFileDepGraph* CStaticBuildEngine::GetDepGraph(VCProject* pProject, VCConfiguration* pCfg)
{
	CBldCfgRecord* pCfgRecord = GetCfgRecord(pProject, pCfg);
	if (pCfgRecord == NULL)
	{
		VSASSERT(FALSE, "Failed to get a matching registry for a CfgRecord");
		return NULL;
	}

	return GetDepGraph(pCfgRecord);
}

CBldFileRegistry* CStaticBuildEngine::GetRegistry(VCProject* pProject, VCConfiguration* pCfg)
{
	CBldCfgRecord* pCfgRecord = GetCfgRecord(pProject, pCfg);
	if (pCfgRecord == NULL)
	{
		VSASSERT(FALSE, "Failed to get a matching registry for a CfgRecord");
		return NULL;
	}

	return GetRegistry(pCfgRecord);
}

CBldCfgRecord* CStaticBuildEngine::GetCfgRecord(IDispatch* pProject, IDispatch* pCfg)
{
	// Prevent access to cfg record list by other threads.
	CritSectionT cs(g_sectionCfgRecords);

	return m_SolutionRecord.GetCfgRecord(pProject, pCfg);
}

void CStaticBuildEngine::RemoveCfgRecord(IDispatch* pProject, IDispatch* pCfg)
{
	// Prevent access to cfg record list by other threads.
	CritSectionT cs(g_sectionCfgRecords);

	CBldCfgRecord* pCfgRecord = m_SolutionRecord.GetCfgRecord(pProject, pCfg, FALSE);

	if (pCfgRecord == NULL)
		return;

	RemoveBuildInst(pCfgRecord);
	m_SolutionRecord.RemoveCfgRecord(pProject, pCfg);
}

CBldProjectRecord* CStaticBuildEngine::GetProjectRecord(IDispatch* pProject, BOOL bCreateIfNeeded /* = TRUE */)
{
	// Prevent access to cfg record list by other threads.
	CritSectionT cs(g_sectionCfgRecords);

	return m_SolutionRecord.GetProjectRecord(pProject, bCreateIfNeeded);
}

void CStaticBuildEngine::RemoveProjectRecord(IDispatch* pProject)
{
	// Prevent access to cfg record list by other threads.
	CritSectionT cs(g_sectionCfgRecords);

	CBldProjectRecord* pProjectRecord = m_SolutionRecord.GetProjectRecord(pProject, FALSE);

	if (pProjectRecord == NULL)
		return;

	VCPOSITION pos = pProjectRecord->GetPtrRecordMap()->GetStartPosition();
	while (pos != (VCPOSITION)NULL)
	{
		CBldCfgRecord* pCfgRecord;
		void* pDummy;
		pProjectRecord->GetPtrRecordMap()->GetNextAssoc(pos, (void*&)pCfgRecord, pDummy);
		RemoveBuildInst(pCfgRecord);
	}
	m_SolutionRecord.RemoveProjectRecord(pProject);
}

void CStaticBuildEngine::SetGraphMode(UINT mode, IDispatch* pProject)
{
	if (pProject == NULL)
		return;
	CBldProjectRecord* pProjRecord = GetProjectRecord(pProject);
	if (pProjRecord == NULL)
		return;
	CVCMapPtrToPtr* pCfgRecordMap = pProjRecord->GetPtrRecordMap();

	VCPOSITION pos = pCfgRecordMap->GetStartPosition();
	while (pos != (VCPOSITION)NULL)
	{
		CBldCfgRecord* pCfgRecord = NULL;
		void* pDummy;
		pCfgRecordMap->GetNextAssoc(pos, (void *&)pCfgRecord, pDummy);
		GetDepGraph(pCfgRecord)->SetGraphMode(mode);
	}
}

BOOL CStaticBuildEngine::OpenMemFile(BldFileRegHandle frh, HANDLE& hMap, const char *& pcFileMap, DWORD& dwFileSize,
	IVCBuildErrorContext* pEC, IVCBuildAction* pAction)
{
    	// NOTE: Because we assume this function is being used to open source code which is MBCS, the output paramater is MBCS
	// Its explicitly NOT TCHAR. When dealing with the contents of the file we should use mbs  and char * NOT _tcs and TCHAR
	// ALL other aspects of this function should be compeltely UNICODE.
    	// yes, ANSI
	
	BOOL fRet = FALSE;	// success?
	HANDLE hFile = INVALID_HANDLE_VALUE;

	// pointer to memory mapped file (== (const TCHAR *)NULL if not available)
	pcFileMap = (const char *)NULL;	// default

	const CPathW* pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();

	for	(;;)
	{
		hMap = NULL;
		pcFileMap = NULL;
		CStringW strPathW;
		*pPath->GetFullPath(strPathW);
		hFile = CreateFileW(strPathW, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			if (pEC)
			{
			    	// REVIEW(KiP) : this is a weird test for error conditions.
				CStringA strPathA = strPathW;
				CStringW strPath2 = strPathA;
				if (strPath2 != strPathW)
				{
					CStringW strMsg;
					strMsg.Format(IDS_ERR_UNICODE_PATH_PRJ0024, strPathW);
					CVCProjectEngine::AddProjectError(pEC, strMsg, L"PRJ0024", pAction);
				}
			}
			break;	// failure
		}

		DWORD dwFileSize2;
		dwFileSize = GetFileSize(hFile, &dwFileSize2);			

		if (dwFileSize == 0xffffffff)
			break;	// failure

		if (dwFileSize == 0)
		{
			CloseHandle(hFile);
			return TRUE;	// ignore
		}

		hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);	// yes, looking for ANSI

		if (hMap == NULL)
			break;	// failure

		pcFileMap = (const char *)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

		break;			// break-out
	}

	if (pcFileMap == (const char *)NULL)
	{
	  	wchar_t errstring [4096];
		DWORD dw = GetLastError();

		if (hMap != NULL)
			CloseHandle(hMap);

		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
				
		//	Try to get a text message for the error.  This fails if NT doesn't have its error resource:
		errstring[0] = 0;
	 	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0 /* ignored */, dw,
			LOCALE_USER_DEFAULT, errstring, 4096, 0 /* ignored */);

		VCRemoveNewlines(errstring);
	}
	else
	{
		// Prevent access to memory mapped files list by other threads.
		CritSectionT cs(g_sectionMemFiles);

		fRet = TRUE;	// success
		MEMFILE * pMemFile = new MEMFILE;
		pMemFile->hFile = hFile;
		pMemFile->hMap = hMap;
		pMemFile->pcFileMap = pcFileMap;

		m_MemFiles.AddTail(pMemFile);
	}

	return fRet;
}

BOOL CStaticBuildEngine::CloseMemFile(HANDLE& hMap, IVCBuildErrorContext* pEC)
{
	// Prevent access to memory mapped files list by other threads.
	CritSectionT cs(g_sectionMemFiles);

	// Look for a memory mapped file entry in our list which has the same mapping object handle
    MEMFILE * pMemFile; 
	VCPOSITION posCurr;
	VCPOSITION pos = m_MemFiles.GetHeadPosition();
	while (pos != NULL)
	{
        posCurr = pos;
		pMemFile = (MEMFILE *) m_MemFiles.GetNext(pos);
		if (pMemFile->hMap == hMap)
		{
			// close this memory mapped file
			UnmapViewOfFile((LPVOID) pMemFile->pcFileMap);
			CloseHandle(pMemFile->hMap);
			CloseHandle(pMemFile->hFile);

            // Remove the MEMFILE entry from our list
            delete pMemFile;
            m_MemFiles.RemoveAt(posCurr);

			return TRUE;
		}		
	}

	// We failed to close the requested file	
	return FALSE;
}

BOOL CStaticBuildEngine::DeleteFile(BldFileRegHandle frh, IVCBuildErrorContext* pEC, VCConfiguration* pConfig,
	IVCBuildActionList* pActionList)
{
	BOOL fRet = TRUE;	// default is success

	if (!g_FileRegistry.DeleteFile(frh))
	{
		DWORD dw = GetLastError();
		if (dw !=  ERROR_FILE_NOT_FOUND && dw != ERROR_PATH_NOT_FOUND)
		{
			wchar_t buf[4096]; buf[0] = L'\0';

			// we get NO_ERROR sometimes!
			if (dw != NO_ERROR)
			{
				// place a ': ' between 'cannot delete' and system messages
				buf[0] = L':'; buf[1] = L' ';

				FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0 /* ignored */,
					dw, LOCALE_USER_DEFAULT, buf + 2, 4094, 0 /* ignored */);
				VCRemoveNewlines(buf);
			}

			CStringW strError;
			::VCMsgTextW(strError, IDS_PROJ_COULD_NOT_DELETE_FILE_PRJ008,
				(const wchar_t *) *g_FileRegistry.GetRegEntry(frh)->GetFilePath(), buf);

			if (pEC != NULL)
			{
				CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
				if (pConfig)
					CVCProjectEngine::AddProjectError(pEC, strError, L"PRJ0008", spPropContainer);
				else
					CVCProjectEngine::AddProjectError(pEC, strError, L"PRJ0008", pActionList);
			}

			fRet = FALSE;	// failure
		}
	}

	return fRet;
}

int CStaticBuildEngine::FindFirstSeparator(CStringW& strList, CStringW& strSeparatorList, int nStartIdx)
{
	int cSeparators = strSeparatorList.GetLength();
	if (cSeparators == 0)	// no separators means give back first char of list
		return 0;
	else if (cSeparators == 1)
		return strList.Find(strSeparatorList, nStartIdx);

	// got here; means we have the potential for multiple separator possibilities
	int nMinSep = -1, nThisSep = -1;
	for (int idx = 0; idx < cSeparators; idx++)
	{
		nThisSep = strList.Find(strSeparatorList.GetAt(idx), nStartIdx);
		if (nThisSep >= 0 && ((nThisSep < nMinSep) || (nMinSep < 0)))
			nMinSep = nThisSep;
	}

	return nMinSep;
}

int CStaticBuildEngine::GetNextItem(CStringW& strList, int nStartIdx, int nMaxIdx, CStringW& strSeparator, CStringW& strItem)
{
	strItem.Empty();
	if (nStartIdx < 0)
		return nStartIdx;
	else if (nStartIdx >= nMaxIdx)
		return -1;

	int nSep = FindFirstSeparator(strList, strSeparator, nStartIdx);
	if (nSep >= 0)
	{
		int nQuote = strList.Find(L'"', nStartIdx);
		if (nQuote >= nStartIdx && nQuote < nSep)	// need to get outside the quoted string
			return GetNextQuotedItem(strList, nStartIdx, nMaxIdx, nSep, nQuote, TRUE, strSeparator, strItem);
		strItem = strList.Mid(nStartIdx, nSep-nStartIdx);
		nSep++;
		return nSep;
	}

	if (nStartIdx < nMaxIdx)
		strItem = strList.Right(nMaxIdx-nStartIdx);

	return -1;
}

int CStaticBuildEngine::GetNextQuotedItem(CStringW& strList, int nStartIdx, int nMaxIdx, int nSep, int nLastQuote,
	BOOL bOddQuote, CStringW& strSeparator, CStringW& strItem)
{
	if (bOddQuote)
	{
		int nQuote = strList.Find(L'"', nLastQuote+1);
		if (nQuote < nSep)	// doesn't matter that we found a quote
		{
			strItem = strList.Mid(nStartIdx, nSep-nStartIdx);
			nSep++;
			return nSep;
		}
		else
			return GetNextQuotedItem(strList, nStartIdx, nMaxIdx, nSep, nQuote, FALSE, strSeparator, strItem);
	}
	else	// even quote
	{
		nSep = FindFirstSeparator(strList, strSeparator, nLastQuote+1);
		if (nSep >= 0)
		{
			int nQuote = strList.Find(L'"', nLastQuote+1);
			if (nQuote > nLastQuote && nQuote < nSep)	// still need to get outside the quoted string
				return GetNextQuotedItem(strList, nStartIdx, nMaxIdx, nSep, nQuote, TRUE, strSeparator, strItem);
			strItem = strList.Mid(nStartIdx, nSep-nStartIdx);
			nSep++;
			return nSep;
		}

		if (nStartIdx < nMaxIdx)
			strItem = strList.Right(nMaxIdx-nStartIdx);
		
		return -1;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDynamicBuildEngine
//		This class handles all the stuff for actually doing a build.  It owns the spawner, log file, etc.
//		Ideally, you would spin up one of these every time you do a build.  If you don't, be sure to
//		reinitialize things before doing a build (InitializeForBuild).
////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT CDynamicBuildEngine::s_iConsoleCodePage = 0;
UINT CDynamicBuildEngine::s_iANSICodePage = 0;

CDynamicBuildEngine::CDynamicBuildEngine()
{
	InitializeForBuild();
}

STDMETHODIMP CDynamicBuildEngine::StopBuild()
{
	m_bUserCanceled = TRUE;
	if (m_Spawner.SpawnActive())
		m_Spawner.DoSpawnCancel();
	return S_OK;
}

CMD CDynamicBuildEngine::FlushCmdLines()
{
	CMD cmd = CMD_Complete;

	// execute our currently batched commands
	cmd = ExecuteCmdLinesI(m_spCmds, m_dirLast, m_fLastCheckForComSpec, m_fLastIgnoreErrors, m_pECLast);

	// flush
	if (m_spCmds != NULL)
		m_spCmds->RemoveAll();

	return cmd;
}

void CDynamicBuildEngine::ClearCmdLines()
{
    // Clear out any batched cmds - important if we stop a build in the middle.
	if (m_spCmds != NULL)
		m_spCmds->RemoveAll();
}

CMD CDynamicBuildEngine::BatchCmdLines(BOOL fBatch /* = TRUE */)
{
	if (m_bUserCanceled)
	{
		InformUserCancel();
		return CMD_Canceled;
	}

	CMD cmd = CMD_Complete;

	if (m_fBatchCmdLines != fBatch)
	{
		m_fBatchCmdLines = fBatch;	// set-it

		if (!fBatch)	// turning off batching?
			cmd = FlushCmdLines();
	}

	return cmd;
}

STDMETHODIMP CDynamicBuildEngine::ExecuteCommandLines(IVCCommandLineList* pCmds, BSTR bstrDir, VARIANT_BOOL fCheckForComSpec, 
	VARIANT_BOOL fIgnoreErrors, IVCBuildErrorContext* pEC, CMD* pCmdRet)
{
	CHECK_POINTER_NULL(pCmdRet);
	CStringW strDir = bstrDir;
	if (m_bUserCanceled)
	{
		*pCmdRet = CMD_Canceled;
		InformUserCancel();
	}
	else
		*pCmdRet = ExecuteCmdLines(pCmds, strDir, (fCheckForComSpec == VARIANT_TRUE), (fIgnoreErrors == VARIANT_TRUE), pEC);
	switch (*pCmdRet)
	{
	case CMD_Complete:
		return S_OK;
	case CMD_Canceled:
		return S_FALSE;
	default:
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BUILD_FAILED, IDS_ERR_BUILD_FAILED);
	}
}

CMD CDynamicBuildEngine::ExecuteCmdLines(IVCCommandLineList* pCmds, CStringW& rstrDir, BOOL fCheckForComSpec, 
	BOOL fIgnoreErrors, IVCBuildErrorContext* pEC)
{
	CMD cmd = CMD_Complete;
	CDirW dir;
	dir.CreateFromKnown(rstrDir);

	if (m_fBatchCmdLines)	// batching?
	{
		BOOL fRememberContext = FALSE;

		// need to flush current if context changed?
		long cCmds;
		pCmds->get_Count(&cCmds);
		if (cCmds > 0)
		{
			if (dir != m_dirLast || fCheckForComSpec != m_fLastCheckForComSpec ||
				fIgnoreErrors != m_fLastIgnoreErrors || pEC != m_pECLast)
			{
				cmd = FlushCmdLines();		// flush our currently batched commands
				fRememberContext = TRUE;	// remember this new context
			}
		}
		else
			fRememberContext = TRUE;	// initialise the context

		// remember the last context?
		if (fRememberContext)
		{
			m_dirLast = dir;
			m_fLastCheckForComSpec = fCheckForComSpec;
			m_fLastIgnoreErrors = fIgnoreErrors;
			m_pECLast = pEC;
		}

		// perform batch
		m_spCmds->AddList(pCmds, FALSE);

		// done
	}
	else
	{
		// perform build commands
		cmd = ExecuteCmdLinesI(pCmds, dir, fCheckForComSpec, fIgnoreErrors, pEC);
	}

	// Make sure cmd is set to a valid value.
	VSASSERT(cmd == CMD_Canceled || cmd == CMD_Complete || cmd == CMD_Error, "ExecuteCmdLines setting cmd to invalid value");

	return cmd;
}

CMD CDynamicBuildEngine::ExecuteCmdLinesI(IVCCommandLineList* pCmds, CDirW& dir, BOOL fCheckForComSpec, BOOL fIgnoreErrors,
	IVCBuildErrorContext* pEC)
{
	// perform the command
	int ret = m_Spawner.DoSpawn(pCmds, dir, this, m_pCfgRecord, fCheckForComSpec, fIgnoreErrors, pEC);

	// The return value we've got now is  actually an int error code returned 
	// by NTSPAWN.  Translate it to one of our CMD_ values.
	if (ret != 0) 
	{
		if (CMD_Canceled == (UINT)ret)
			return CMD_Canceled;
		else
			return CMD_Error;
	}

	return CMD_Complete;
}

STDMETHODIMP CDynamicBuildEngine::InitializeForBuild()
{
	// logging defaults
	m_pLogFile = NULL;
	m_cReuseLogFile = 0;
	m_bFirstUsage = TRUE;
	m_logCurrentSection = eLogNoSection;

	m_actualErrs = 0;
	m_warns = 0;
	m_bDoIdleAndPump = FALSE;
	m_bUserCanceled = FALSE;
	m_bUserInformedAboutCancel = FALSE;
	m_pCfgRecord = NULL;

	// batching is off by default
	m_fBatchCmdLines = FALSE;
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::get_LogFile( BSTR *bstrLogFile )
{
	CComBSTR bstrRet( m_strLogFile );
	*bstrLogFile = bstrRet.Detach();
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::AddMissingEnvironmentVariable(BSTR bstrMissingVar)
{
	CStringW strMissingVar = bstrMissingVar;
	if (strMissingVar.IsEmpty())
		return S_FALSE;

	CStringW strMissingVarNoCase = strMissingVar;
	strMissingVarNoCase.MakeLower();
	void* pDummy;
	if (m_mapMissingEnvVarsNoCase.Lookup(strMissingVarNoCase, pDummy))
		return S_OK;
	
	// add a warning count for each unique missing env var
	m_warns++;

	m_mapMissingEnvVars.SetAt(strMissingVar, NULL);
	m_mapMissingEnvVarsNoCase.SetAt(strMissingVarNoCase, NULL);
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::ClearPendingEnvironmentVariableWarnings()
{
	CComPtr<IVCBuildErrorContext> spEC;
	get_ErrorContext(&spEC);

	if (spEC)
	{
		VCPOSITION pos = m_mapMissingEnvVars.GetStartPosition();
		if (pos)
		{
			CStringW strVar, strVar2;
			void *dummy;
			strVar.LoadString(IDS_MISSING_ENV_VAR_PRJ0018);
			while (pos)
			{
				m_mapMissingEnvVars.GetNextAssoc(pos, strVar2, dummy);
				strVar2 = L"\n$(" + strVar2 + L")";
				strVar += strVar2;
			}

			// dump the missing environment variables to the output window and task list
			CVCProjectEngine::AddProjectWarning(spEC, strVar, L"PRJ0018", m_pCfgRecord);
		}
	}

	m_mapMissingEnvVars.RemoveAll();
	m_mapMissingEnvVarsNoCase.RemoveAll();
	m_mapOutputDirs.RemoveAll();
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::AddToOutputDirectoryList(BSTR bstrDir)
{
	CStringW strDir = bstrDir;
	if (strDir.IsEmpty())
		return E_UNEXPECTED;

	void *pDummy = NULL;
	if (m_mapOutputDirs.Lookup(strDir, pDummy))
		return S_FALSE;		// already present

	m_mapOutputDirs.SetAt(strDir, pDummy);
	return S_OK;
}

#define MAX_CMD_LEN 2
// REVIEW: #define MAX_CMD_LEN 127

BOOL CDynamicBuildEngine::FormCmdLine(CStringW& strToolExeName, CStringW& strCmdLine, IVCBuildErrorContext* pEC, 
	BOOL fRspFileOk, BOOL fLogCommand)
{
	// is this line too long?
	if (strCmdLine.GetLength() + strToolExeName.GetLength() + 2 > MAX_CMD_LEN - 1)
	{
		CPathW pathRsp;
		if (!fRspFileOk)
			return FALSE;

		// need to pull /nologo out and stick it *outside* the response file.  This is 'cause MIDL is stupid and puts
		// out its banner *before* reading the response file and then won't live with us also specifying /nologo inside
		// the response file as well as on the command line.  Ugh.
		int nLoc = strCmdLine.Find(L"/nologo");
		if (nLoc >= 0)
		{
			CStringW strLeft = strCmdLine.Left(nLoc);
			int nLen = strCmdLine.GetLength();
			CStringW strRight = strCmdLine.Right(nLen-nLoc-7);
			strLeft.TrimRight();
			strRight.TrimLeft();
			if (strLeft.IsEmpty())
				strCmdLine = strRight;
			else if (strRight.IsEmpty())
				strCmdLine = strLeft;
			else
			{
				strCmdLine = strLeft + L" ";
				strCmdLine += strRight;
			}
		}
		if (!TmpMaker.CreateTempFileFromString(strCmdLine, pathRsp, this, pEC, m_pCfgRecord)) 
			return FALSE;

		// quote response file path if necessary
		CStringW strRspPath = pathRsp;
		if (pathRsp.ContainsSpecialCharacters())
			strRspPath = L'\"' + strRspPath + L'\"';

		// use response file
		strCmdLine = (strToolExeName + L" @") + strRspPath;
		if (nLoc >= 0)
			strCmdLine += L" /nologo";
	}
	else
	{
		// don't use response file

		// replace '\r\n\t' with spaces
		int cchLen = strCmdLine.GetLength();
		wchar_t * pch = (wchar_t *)(const wchar_t *)strCmdLine;
		for (int ich = 0; ich < cchLen;)
		{
			wchar_t ch = pch[ich];
			if (ch == L'\r' || ch == L'\n' || ch == L'\t')
				pch[ich] = L' ';

			ich += lstrlenW(pch + ich);
		}

		strCmdLine = (strToolExeName + L' ') + strCmdLine;
	}

	if (fLogCommand)
	{
		CStringW strCommand;
		strCommand.Format(IDS_CREATING_COMMANDLINE, strCmdLine);
		LogTrace(eLogCommand, (const wchar_t *)strCommand);
	}

	return TRUE;	// success
}

BOOL CDynamicBuildEngine::FormBatchFile(CStringW& strCmdLine, IVCBuildErrorContext* pEC)
{
	// start out by stripping any /r characters as Win9X doesn't like these
	CStringW strLine = strCmdLine;
	strCmdLine.Empty();
	int nLen = strLine.GetLength();
	for (int idx = 0; idx < nLen; idx++)
	{
		if (strLine[idx] != L'\r')
			strCmdLine += strLine[idx];
	}

	CPathW pathRsp;
	if (!TmpMaker.CreateTempFileFromString(strCmdLine, pathRsp, this, pEC, m_pCfgRecord, FILE_BATCH))
		return FALSE;

	// use response file
	strCmdLine = pathRsp;

	// quote path if necessary
	if (pathRsp.ContainsSpecialCharacters())
		strCmdLine = L'\"' + strCmdLine + L'\"';

	CStringW strCommand;
	strCommand.Format(IDS_CREATING_COMMANDLINE, strCmdLine); 
	LogTrace(eLogCommand, strCommand);

	return TRUE;	// success
}

BOOL CDynamicBuildEngine::FormTempFile(CStringW& strCmdLine, CStringW& strCommentLine)
{
	CPathW pathRsp;
	if (!TmpMaker.CreateTempFileFromString(strCmdLine, pathRsp, this, m_spBuildErrorContext, m_pCfgRecord, FILE_TEMP))
		return FALSE;

	// use response file
	strCmdLine = pathRsp;

	// quote path if necessary
	if (pathRsp.ContainsSpecialCharacters())
		strCmdLine = L'\"' + strCmdLine + L'\"';

	CStringW strCommand;
	if (strCommentLine.IsEmpty())
		strCommand.Format(IDS_CREATING_COMMANDLINE, strCmdLine); 
	else
	{
		if (strCommentLine.Find(L"%s") >= 0)
			strCommand.Format(strCommentLine, strCmdLine);
		else
		{
			strCommand = strCommentLine;
			strCommand += strCmdLine;
		}
		if (strCommand[strCommand.GetLength()-1] != L'\n')
			strCommand += L"\n";
	}
	LogTrace(eLogCommand, strCommand);

	return TRUE;	// success
}

BOOL CDynamicBuildEngine::FormXMLCommand(CStringW& strCmdLine, CStringW& strExe, CStringW& strExtraArgs)
{
	CPathW pathRsp;
	if (!TmpMaker.CreateTempFileFromString(strCmdLine, pathRsp, this, m_spBuildErrorContext, m_pCfgRecord, FILE_XML))
		return FALSE;

	// use response file
	CStringW strRsp = pathRsp;

	// quote path if necessary
	if (pathRsp.ContainsSpecialCharacters())
		strRsp = L'\"' + strRsp + L'\"';

	strCmdLine = strExe + L" ";
	if (!strExtraArgs.IsEmpty())
		strCmdLine += strExtraArgs + L" ";
	strCmdLine += strRsp;
	
	CStringW strCommand;
	strCommand.Format(IDS_CREATING_COMMANDLINE, strCmdLine); 
	LogTrace(eLogCommand, strCommand);

	return TRUE;	// success
}


STDMETHODIMP CDynamicBuildEngine::HasBuildState(void* pcrBuild, IVCBuildErrorContext* pEC, VARIANT_BOOL* pbUpToDate)
{
	CHECK_POINTER_NULL(pbUpToDate);
	*pbUpToDate = VARIANT_TRUE;

	CBldCfgRecord* pcrCheck = (CBldCfgRecord*)pcrBuild;
	// get our build dependency graph for this config.
	CBldFileDepGraph* pdepgraph = g_StaticBuildEngine.GetDepGraph(pcrCheck);
	RETURN_ON_NULL(pdepgraph);

	// use a different action marker than the last time we did a build
	CBldAction::m_wActionMarker++;

	// equivalent to 'CEnvironmentVariableList' of yore
	VCConfiguration* pConfig = pcrCheck->GetVCConfiguration();

    CComPtr<IDispatch>			spDisp;
	pConfig->get_Platform(&spDisp);
   	CComQIPtr<VCPlatform>	spPlatform = spDisp;
	CEnvironmentKeeper envMangler(spPlatform);

	// retrieve our actions
	CBldActionListHolder lstActions;
	UINT aor = AOR_Recurse | AOR_ChkOpts | AOR_PreChk | AOR_ChkInp;
	CVCPtrList lstFrh;
#ifdef	DIANEME_DEP_CHECK_DIAGNOSTICS
	ATLTRACE("Starting post-build RetrieveBuildActions for build #%d\n", s_nCurrentBuild);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
	CMD cmdRetRA = pdepgraph->RetrieveBuildActions(pcrCheck, lstActions.m_pActions, lstFrh, pEC, this, NULL, DS_OutOfDate, aor, 
		TRUE /* not clean */);
#ifdef	DIANEME_DEP_CHECK_DIAGNOSTICS
	ATLTRACE("Finishing post-build RetrieveBuildActions for build #%d\n", s_nCurrentBuild);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
	if (cmdRetRA != CMD_Complete)
	{
		// cannot retrieve actions because of error or user cancel
		lstActions.m_pActions->Clear();
		*pbUpToDate = VARIANT_FALSE;
	}
	else
		*pbUpToDate = lstActions.m_pActions->NotEmpty() ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}


STDMETHODIMP CDynamicBuildEngine::DoBuild(void* pcrBuild, BOOL bIsProjectBuild, IVCBuildErrorContext* pEC, 
	void* frhStart, bldAttributes aob, bldActionTypes type, BOOL fRecurse, IVCBuildAction* pAction, BuildResults* pbrResults)
{
	CHECK_POINTER_NULL(pbrResults);
	*pbrResults = DoBuild((CBldCfgRecord*)pcrBuild, bIsProjectBuild, pEC, (BldFileRegHandle)frhStart, aob, type, fRecurse, pAction);

	switch (*pbrResults)
	{
	case BuildComplete:
	case NoBuild:
		return S_OK;
	case BuildCanceled:
		return S_FALSE;
//	case BuildError:
	default:
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BUILD_FAILED, IDS_ERR_BUILD_FAILED);
	}
}

BuildResults CDynamicBuildEngine::DoBuild(CBldCfgRecord* pcrBuild, BOOL bIsProjectBuild, IVCBuildErrorContext* pEC,
	BldFileRegHandle frhStart /* = (BldFileRegHandle)NULL*/, bldAttributes aob /* = AOB_Default */,
	bldActionTypes type /* = TOB_Build */, BOOL fRecurse /* = TRUE */, IVCBuildAction* pEventAction /* = NULL */)
{
	if (m_bUserCanceled)
	{
		InformUserCancel();
		return BuildCanceled;
	}

	m_pCfgRecord = pcrBuild;	// cache this off in case of error handling later

	// get our build dependency graph and registry for this config.
	CBldFileDepGraph * pdepgraph = g_StaticBuildEngine.GetDepGraph(pcrBuild);

	// these are the pleasant formatting strings for the HTML page
	CStringW strPreHeader;
    strPreHeader.LoadString(IDS_HTML_PRE_HEADER);
	CStringW strPostHeader;
    strPostHeader.LoadString(IDS_HTML_POST_HEADER);

	// equivalent to 'CEnvironmentVariableList' of yore
	VCConfiguration* pConfig = pcrBuild->GetVCConfiguration();

    CComPtr<IDispatch>			spDisp;
	pConfig->get_Platform(&spDisp);
   	CComQIPtr<VCPlatform>	spPlatform = spDisp;

	CEnvironmentKeeper envMangler(spPlatform);

	// what dependencies do we want to build?
	UINT stateFilter = DS_OutOfDate | DS_ForcedOutOfDate;

	/// if we're re-building or forcing a build then build 'everything'
	if (type == TOB_Clean || type == TOB_ReBuild || (aob & AOB_Forced))
		stateFilter |= (DS_Current | DS_DepMissing);

	// return value of the build
	// default is 'complete', assuming finish build passes without 'cancel' or 'error'
 	BuildResults bldResults = BuildComplete;
	CMD cmdRet = CMD_Complete;

	// start the passes
	pdepgraph->StartPasses();

//
// perform the build pass
//
	// let the build log know what the environment space looks like
	CStringW strEnvTemp;
	strEnvTemp.LoadString(IDS_HTML_ENVIRONMENT);
	CStringW strEnvLines = strPreHeader + strEnvTemp + strPostHeader;
	LogTrace((eLogSectionTypes)(eLogEnvironment|eLogStartSection), strEnvLines);
	SetCurrentLogSection(eLogEnvironment);	// this forces the line we just added above to be BEFORE anything else we've been saving up for the section
		
	/* Block */ {
		wchar_t *pcStart = (wchar_t*)GetEnvironmentStringsW();
		wchar_t * pc = pcStart;
		while (*pc != 0) 
		{
			if (*pc && *pc != L'=')
			{
				CStringW strTemp = pc;
				strEnvTemp = L"    " + strTemp;
				strEnvTemp += L"\n";
				LogTrace(eLogEnvironment, strEnvTemp);
			}
			pc += wcslen( pc );
			pc++;	// gets past the NULL?
		}
		FreeEnvironmentStringsW(pcStart);
	}

	// Actions attempted in TOB_Stage.
	CBldActionListHolder lstStagedActions;

	CStringW strCommandLinesTemp;
	strCommandLinesTemp.LoadString(IDS_HTML_COMMANDLINE);
	CStringW strCommandLines = strPreHeader + strCommandLinesTemp + strPostHeader;
	    
	LogTrace((eLogSectionTypes)(eLogCommand|eLogStartSection), strCommandLines);
	SetCurrentLogSection(eLogCommand);	// this forces the line we just added above to be BEFORE anything else we've been saving up for the section

	if (type == TOB_Clean || type == TOB_ReBuild)
		DoWildCardCleanup(pConfig, type, pEC);	// wildcard cleanup if we're here; do before any other build action retrieval

	if (m_bUserCanceled)
	{
		InformUserCancel();
		return BuildCanceled;
	}

	// state filter should get everything on a rebuild all.
	// so there is no need to lookup file changes or Deps.

	//
	// retrieve our actions (prepend with our retry actions)
	//
	CBldActionListHolder lstActions;
	UINT aor = fRecurse ? AOR_Recurse : AOR_None;

	// check option changes and input changes on first pass
	aor |= (AOR_ChkOpts | AOR_ChkInp);

	CVCPtrList lstFrh;
	UINT cNonDeferred = 0;
	
//	StartCAP();
	CMD cmdRetRA = CMD_Complete;
	if (IsBuildEvent(type))	// single element build event build
	{
		if (pEventAction != NULL)
			lstActions.m_pActions->Add(pEventAction);
		cmdRetRA = CMD_Complete;
	}
	else if (type == TOB_Link)
	{
		if (pEventAction != NULL)
			lstActions.m_pActions->Add(pEventAction);
		cmdRetRA = CMD_Complete;
	}
	else	// normal build
	{
#ifdef	DIANEME_DEP_CHECK_DIAGNOSTICS
		s_nCurrentBuild++;
		ATLTRACE("Just before RetrieveBuildActions for build #%d\n", s_nCurrentBuild);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
		cmdRetRA = pdepgraph->RetrieveBuildActions(pcrBuild, lstActions.m_pActions, lstFrh, pEC, this, frhStart, stateFilter, aor, 
			type != TOB_Clean);
#ifdef	DIANEME_DEP_CHECK_DIAGNOSTICS
		ATLTRACE("Just after RetrieveBuildActions for build #%d\n", s_nCurrentBuild);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
	}
//	SuspendCAP();

	if (m_bUserCanceled)
	{
		cmdRet = CMD_Canceled;
		InformUserCancel();
		goto EndBuildPass;
	}

	ConfigurationTypes cfgType;
	pConfig->get_ConfigurationType( &cfgType );
	if (((cmdRetRA != CMD_Complete) || !lstActions.m_pActions->NotEmpty()) && cfgType != typeGeneric && cfgType != typeUnknown )
	{
		// cannot retrieve actions because of error, lack of things to to do or user cancel
		if (cmdRetRA == CMD_Complete)
			bldResults = NoBuild;
		else
			bldResults = ConvertFromCMDToBuildResults(cmdRetRA);
		goto EndBuildPass;
	}

	UINT iCodePage = GetCodePage(); // We are going to build: Init the Console codepage
	
	// time to start hunting for any build events
	// don't care about them if we're here on a build of an individual item
	if (fRecurse && !IsBuildEvent(type) && bIsProjectBuild)
	{
		VCConfiguration* pProjCfg = pcrBuild->GetVCConfiguration();
		CComQIPtr<IVCBuildableItem> spBuildableItem = pProjCfg;
		CComPtr<IVCBuildActionList> spBuildActions;
		if (spBuildableItem && SUCCEEDED(spBuildableItem->get_ActionList(&spBuildActions)) && spBuildActions)
		{
			AddBuildEventTool(spBuildActions, szPreBuildEventToolShortName, lstActions);
			AddBuildEventTool(spBuildActions, szPostBuildEventToolShortName, lstActions);

			CComPtr<IVCBuildAction> spLinkAction;
			if (lstActions.m_pActions->FindByName(szLinkerToolShortName, &spLinkAction) == S_OK ||
				lstActions.m_pActions->FindByName(szLibrarianToolShortName, &spLinkAction) == S_OK)
			{
				AddBuildEventTool(spBuildActions, szPreLinkEventToolShortName, lstActions);
				AddBuildEventTool(spBuildActions, szDeployToolShortName, lstActions);
				AddBuildEventTool(spBuildActions, szXboxImageToolShortName, lstActions);
			}
			// Run Xbox deploy tool whenever Xbox image tool is run
			CComPtr<IVCBuildAction> spXboxImageAction;
			if (lstActions.m_pActions->FindByName(szXboxImageToolShortName, &spXboxImageAction) == S_OK)
			{
				AddBuildEventTool(spBuildActions, szXboxDeployToolShortName, lstActions);
			}
		}
	}

	if( cfgType == typeGeneric || cfgType == typeUnknown )
	{
		VCConfiguration* pProjCfg = pcrBuild->GetVCConfiguration();
		CComQIPtr<IVCBuildableItem> spBuildableItem = pProjCfg;
		CComPtr<IVCBuildActionList> spBuildActions;
		if (spBuildableItem && SUCCEEDED(spBuildableItem->get_ActionList(&spBuildActions)) && spBuildActions)
		{
			AddNMakeTool( spBuildActions, lstActions);
		}

		if (!lstActions.m_pActions->NotEmpty())
		{
			bldResults = NoBuild;
			goto EndBuildPass;
		}
	}

	if (m_bUserCanceled)
	{
		cmdRet = CMD_Canceled;
		InformUserCancel();
		goto EndBuildPass;
	}
	//
	// post-process our actions
	//
	{
		// post-process stage 1.

		// put all actions in their positional buckets
		#define MAX_TOOL_BUCKET BUCKET_MAXIMUM + 1
		CVCPtrList bucket[MAX_TOOL_BUCKET];
		VCPOSITION pos = lstActions.m_pActions->GetStart();
		while (pos != (VCPOSITION)NULL)
		{
			CBldAction* pAction = lstActions.m_pActions->Next(pos);
			CBldToolWrapper* pToolWrapper = pAction->BuildToolWrapper();
			if (pToolWrapper)
			{
				int nOrder = pToolWrapper->GetOrder();
				if (nOrder == BUCKET_CUSTOMBUILD) 
				{
					if (pAction->IsProject())	// config level, not file level
						bucket[BUCKET_POSTBUILD].AddTail(pAction);
					else
						bucket[BUCKET_CUSTOMBUILD].AddTail(pAction);
				}
				else
					bucket[nOrder].AddTail(pAction);
			}
		}

		// now clear the original list and join all the separate lists together.
		lstActions.m_pActions->Clear(FALSE);
		for (int i = (MAX_TOOL_BUCKET-1); i >= 0; i--)
			lstActions.m_pActions->AddToEnd(&(bucket[i]), FALSE);
	}
	
	//
	// perform the actions for each of our stages
	//

	// if we're logging performance stats
	DWORD endTime;
	VARIANT_BOOL bLog;
   	bLog = VARIANT_FALSE;
	g_pProjectEngine->get_PerformanceLogging( &bLog );
	if( bLog == VARIANT_TRUE )
	{
		// end time
		endTime = GetTickCount();
		fprintf( stdout, "Project Build end: %d\n", endTime );
	}
	// batch?
	// don't batch the defered ones
	if (cNonDeferred != 0)
	{
		CMD cmd = BatchCmdLines();
		VSASSERT(cmd == CMD_Complete || cmd == CMD_Canceled, "Failure during BatchCmdLines!");
		if (cmd == CMD_Canceled)
		{
			cmdRet = CMD_Canceled;
			goto EndBuildPass;
		}
	}

	{
		CBldActionListHolder lstToolActions;
		VCPOSITION pos = lstActions.m_pActions->GetStart();
		while (pos != (VCPOSITION)NULL)
		{
			// collect together our actions for the same tool
			CBldToolWrapper* pToolWrapper = (CBldToolWrapper *)NULL;
			while (pos != (VCPOSITION)NULL)
			{
				VCPOSITION posOld = pos;
				CBldAction* pAction = lstActions.m_pActions->Next(pos);

				// o can we do this action now if errors encountered?
				if (cmdRet == CMD_Error && !pAction->BuildToolWrapper()->PerformIfAble())
					// no, so ignore
					continue;

				if (pToolWrapper == (CBldToolWrapper *)NULL || pAction->BuildToolWrapper() == pToolWrapper)
				{
					if (pToolWrapper == (CBldToolWrapper *)NULL)
					{
						// init. our state
						lstToolActions.m_pActions->Clear();
						pToolWrapper = pAction->BuildToolWrapper();
					}

					// add this to the tool's list of actions
					lstToolActions.m_pActions->AddToEnd(pAction);
				}
				else
				{
					// backtrack and breakout
					pos = posOld;
					break;
				}
			}

			// any tool to perform for this stage?
			if (pToolWrapper == (CBldToolWrapper *)NULL)
				continue;

			// perform the stage of this build
			actReturnStatus actRet;
			if (m_bUserCanceled)
			{
				actRet = ACT_Canceled;
				InformUserCancel();
			}
			else
				actRet = pToolWrapper->PerformBuildActions(type, lstToolActions.m_pActions, aob, pEC, this);

			// Add lstToolActions to lstStagedActions.
			if (type != TOB_Clean)
				lstStagedActions.m_pActions->AddToEnd(lstToolActions.m_pActions);

			switch (actRet)
			{
				case ACT_Canceled:
					cmdRet = CMD_Canceled;
					// cancel whole build
					goto EndBuildPass;
					break;

				case ACT_Error:
					cmdRet = CMD_Error;
					// continue with the build passes in case any tools
					// can still 'Perform If Able' regardless of errors
					break;

				case ACT_Complete:
					// cmdRet = CMD_Complete;	// Don't set if already had an error
					break;

				default:
					VSASSERT(FALSE, "DoBuild returing invalid cmdRet value");
					break;
			}
		}
	}

	// flush batch?
	{
		CStringW strOutputWindowTemp;
		strOutputWindowTemp.LoadString(IDS_HTML_OUTPUTWINDOW);
		CStringW strOutputWindow = strPreHeader + strOutputWindowTemp + strPostHeader;

		LogTrace((eLogSectionTypes)(eLogOutput|eLogStartSection), strOutputWindow);
		SetCurrentLogSection(eLogOutput);
	}

	if (cmdRet == CMD_Complete)
		cmdRet = BatchCmdLines(FALSE);
	else
		ClearCmdLines();	// clear any still batched but not flushed cmd lines now

	if (cmdRet == CMD_Canceled)
		goto EndBuildPass;	// cancel whole build

//
// End the build passes
//
EndBuildPass:

	if (cmdRet != CMD_Complete)	// if this is CMD_Complete, bldResults should be either BuildComplete or NoBuild, set appropriately above
		bldResults = ConvertFromCMDToBuildResults(cmdRet);

	m_bUserCanceled = FALSE;				// clear out this flag if it was ever set
	m_bUserInformedAboutCancel = FALSE;		// clear out this flag if it was ever set

	// Flush anything we haven't written yet into the log file
	FlushLogSection(eLogAll);
	SetCurrentLogSection(eLogOutput);

	// Update dependencies
	if (lstStagedActions.m_pActions->NotEmpty())
	{
		// Update files.
		CBldFileRegFile::UpdateTimeStampTick();		// this makes it so we don't look up the time stamp on something multiple times
		VCPOSITION posCurrentAction = lstStagedActions.m_pActions->GetStart();
		CBldAction* pFirstAction = (CBldAction*)lstStagedActions.m_pActions->Next(posCurrentAction);
		CComPtr<VCConfiguration> spProjCfg;
		pFirstAction->get_ProjectConfiguration(&spProjCfg);
		CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spProjCfg;
		VSASSERT(spProjCfgImpl != NULL, "No configuration found to go with an action!");
		CStringW strIdb;
		if (spProjCfgImpl != NULL)
		{
			CComBSTR bstrIdb;
			HRESULT hr = spProjCfgImpl->get_MrePath(&bstrIdb);
			VSASSERT(SUCCEEDED(hr), "Failed on spProjCfgImpl->get_MrePath call!");
			strIdb = bstrIdb;
		}

		// Minimal rebuild dependency interface.
		CBldMreDependenciesContainer mreDepend(strIdb);

		// Update dependencies.
		// Set up a new cache for this target			
//		ResumeCAP();
		m_ScannerCache.BeginCache();

		posCurrentAction = lstStagedActions.m_pActions->GetStart();

		while (NULL != posCurrentAction)
		{
			CBldAction* pCurrentAction = (CBldAction*)lstStagedActions.m_pActions->Next(posCurrentAction);

			VSASSERT(NULL != pCurrentAction, "NULL pointer value in staged actions action list");

			pCurrentAction->UpdateDepInfo(pEC);
		}

		m_ScannerCache.EndCache();
//		StopCAP();
	}

	// clear any still batched but not flushed cmd lines now
	ClearCmdLines();

	// nuke any temporary files created as a part of this build
	NukeTempFiles(pEC);

	// let go of any pretense of knowing about the cfg record anymore
	m_pCfgRecord = NULL;

	// env is restored in EnvDaemon destructor

	return bldResults;
}

BuildResults CDynamicBuildEngine::ConvertFromCMDToBuildResults(CMD cmd)
{
	switch(cmd)
	{
	case CMD_Canceled:
		return BuildCanceled;
	case CMD_Complete:
		return BuildComplete;
	// case CMD_Error:
	default:
		return BuildError;
	}
}

// wildcard cleanup if we're here; do before any other build action retrieval
HRESULT CDynamicBuildEngine::DoWildCardCleanup(VCConfiguration* pConfig, bldActionTypes type, IVCBuildErrorContext* pEC)
{
	CHECK_READ_POINTER_NULL(pConfig);

	CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
	RETURN_ON_NULL2(spPropContainer, S_FALSE);	// no config stuff, we're toast

	CComBSTR bstrDelExt;
	pConfig->get_DeleteExtensionsOnClean(&bstrDelExt);
	HRESULT hr = spPropContainer->Evaluate(bstrDelExt, &bstrDelExt);
	RETURN_ON_FAIL(hr);
	if (bstrDelExt.Length() == 0)	// no wild cards, so go away
		return S_FALSE;
	CStringW strDelExt = bstrDelExt;
	strDelExt.TrimLeft();
	strDelExt.TrimRight();
	if (strDelExt.IsEmpty())	// no wild cards, so go away
		return S_FALSE;

	// pick up the absolute path to the intermediate directory
	CComBSTR bstrProjDir;
	spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
	CStringW strProjDir = bstrProjDir;
	strProjDir.TrimLeft();
	if (strProjDir.IsEmpty())
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CDirW dirProj;
	if (!dirProj.CreateFromKnown(strProjDir))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CComBSTR bstrIntDir;
	pConfig->get_IntermediateDirectory(&bstrIntDir);
	CStringW strIntDir = bstrIntDir;
	CPathW pathIntDir;
	if (!pathIntDir.CreateFromDirAndFilename(dirProj, strIntDir))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	CDirW dirIntDir;
	if (!dirIntDir.CreateFromPath(pathIntDir, TRUE))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	// now cycle through the wild card sets within the intermediate directory
	int nNextIdx = 0;
	int nMaxIdx = strDelExt.GetLength();
	CStringW strSemi = L";";
	while (nNextIdx >= 0)
	{
		CStringW strWild;
		nNextIdx = g_StaticBuildEngine.GetNextItem(strDelExt, nNextIdx, nMaxIdx, strSemi, strWild);
		strWild.TrimLeft();
		if (strWild.IsEmpty())
			continue;
		CPathW pathWild;
		if (!pathWild.CreateFromDirAndFilename(dirIntDir, strWild))
			continue;
		WIN32_FIND_DATAW findWildData;
		HANDLE hFindWild = ::FindFirstFileW((LPCOLESTR)pathWild, &findWildData);
		if (hFindWild == INVALID_HANDLE_VALUE)
			continue;	// problem with FindFirstFileW

		DeleteOneFile(findWildData, strWild, dirIntDir, pEC, pConfig);	// delete the first file in the list

		WIN32_FIND_DATAW findWild;
		while (::FindNextFileW(hFindWild, &findWild))
			DeleteOneFile(findWild, strWild, dirIntDir, pEC, pConfig);
		::FindClose(hFindWild);

		if (m_bUserCanceled)
		{
			InformUserCancel();
			break;
		}
	}

	return S_OK;
}

void CDynamicBuildEngine::DeleteOneFile(WIN32_FIND_DATAW& findWild, CStringW& strWild, CDirW& dirIntDir, IVCBuildErrorContext* pEC,
	VCConfiguration* pConfig)
{
	if (findWild.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return;	// not deleting sub-directories

	CStringW strFile = findWild.cFileName;
	if (_wcsicmp(strFile, L"BuildLog.htm") == 0 && _wcsicmp(strWild, strFile) != 0)	// leave the build log there unless explicitly removing it...
		return;

	CPathW pathFile;
	if (!pathFile.CreateFromDirAndFilename(dirIntDir, strFile))
		return;
	BldFileRegHandle frh = CBldFileRegFile::GetFileHandle(pathFile, TRUE);
	if (frh == NULL)
		return;

	g_StaticBuildEngine.DeleteFile(frh, pEC, pConfig, NULL);	// don't care about return code...
	frh->ReleaseFRHRef();
}

void CDynamicBuildEngine::AddBuildEventTool(IVCBuildActionList* pBuildActions, LPOLESTR szBuildEventToolShortName, 
	CBldActionListHolder& lstActions)
{
	CComPtr<IVCBuildAction> spEventAction;
	if (pBuildActions->FindByName(szBuildEventToolShortName, &spEventAction) == S_OK)
	{
		CComPtr<IVCToolImpl> spToolImpl;
		if (SUCCEEDED(spEventAction->get_VCToolImpl(&spToolImpl)) && spToolImpl != NULL)
		{
			CComQIPtr<IVCBuildEventToolInternal> spEventTool = spToolImpl;
			if (spEventTool != NULL)
			{
				VARIANT_BOOL bExcluded = VARIANT_FALSE;
				if (FAILED(spEventTool->get_ExcludedFromBuildInternal(&bExcluded)) || bExcluded == VARIANT_FALSE)
				{
					CComBSTR bstrCommand;
					if (spEventTool->get_CommandLineInternal(&bstrCommand) == S_OK && bstrCommand.Length() > 0)
					{
						CBldAction* pAction = NULL;
						spEventAction->get_ThisPointer((void **)&pAction);
						lstActions.m_pActions->AddToEnd(pAction);
					}
				}
			}
		}
	}
}

void CDynamicBuildEngine::AddNMakeTool(IVCBuildActionList* pBuildActions, CBldActionListHolder& lstActions)
{
	CComPtr<IVCBuildAction> spEventAction;
	if (pBuildActions->FindByName(szNMakeToolShortName, &spEventAction) == S_OK)
	{
		CComPtr<IVCToolImpl> spToolImpl;
		if (SUCCEEDED(spEventAction->get_VCToolImpl(&spToolImpl)) && spToolImpl != NULL)
		{
			CBldAction* pAction = NULL;
			spEventAction->get_ThisPointer((void **)&pAction);
			lstActions.m_pActions->Add(pAction);
		}
	}
}

STDMETHODIMP CDynamicBuildEngine::get_ScannerCache(void** ppScannerCache)
{
	CHECK_POINTER_NULL(ppScannerCache);
	*ppScannerCache = (void *)&m_ScannerCache;
	return S_OK;
}
STDMETHODIMP CDynamicBuildEngine::get_ReuseLogFile(VARIANT_BOOL* pbReuse)
{
	CHECK_POINTER_NULL(pbReuse);
	if (m_bReuseLogFile)
		*pbReuse = VARIANT_TRUE;
	else
		*pbReuse = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::put_ReuseLogFile(VARIANT_BOOL bReuse)
{
	if (bReuse == VARIANT_TRUE)
		m_cReuseLogFile++;
	else if (m_cReuseLogFile > 0)
		m_cReuseLogFile--;
	if (m_cReuseLogFile == 0)	// all usages are the first one if we're not doing reuse...
		m_bFirstUsage = TRUE;
	return S_OK;
}

HRESULT CDynamicBuildEngine::CreateInstance(IVCBuildEngine** ppEngine)
{
	CHECK_POINTER_NULL(ppEngine);
	*ppEngine = NULL;

	CComObject<CDynamicBuildEngine> *pObj;
	HRESULT hr = CComObject<CDynamicBuildEngine>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		CDynamicBuildEngine *pVar = pObj;
		pVar->AddRef();
		*ppEngine = pVar;
	}
	return hr;
}

STDMETHODIMP CDynamicBuildEngine::get_BuildThread(IVCBuildThread** ppThread)
{
	CHECK_POINTER_NULL(ppThread);
	return CVCBuildThread::CreateInstance(ppThread);
}

STDMETHODIMP CDynamicBuildEngine::get_ErrorContext(IVCBuildErrorContext** ppErrContext)
{
	CHECK_POINTER_NULL(ppErrContext);
	*ppErrContext = NULL;
	if (m_spBuildErrorContext)
		return m_spBuildErrorContext.CopyTo(ppErrContext);
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::put_ErrorContext(IVCBuildErrorContext* pErrContext)
{
	m_spBuildErrorContext = pErrContext;
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::get_DoIdleAndPump(BOOL* pbDoIdleAndPump)
{ 
	CHECK_POINTER_NULL(pbDoIdleAndPump);
	*pbDoIdleAndPump = m_bDoIdleAndPump; 
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::put_DoIdleAndPump(BOOL bIdleAndPump)
{ 
	m_bDoIdleAndPump = bIdleAndPump; 
	return S_OK;
}
	
STDMETHODIMP CDynamicBuildEngine::get_SpawnerActive(VARIANT_BOOL* pbSpawnerActive)
{
	CHECK_POINTER_NULL(pbSpawnerActive);
	*pbSpawnerActive = m_Spawner.SpawnActive() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::get_CanSpawnBuild(VARIANT_BOOL* pbCanSpawn)
{
	CHECK_POINTER_NULL(pbCanSpawn);
	*pbCanSpawn = m_Spawner.CanSpawn() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::InitializeSpawner(BOOL bHidden)
{
	return m_Spawner.InitSpawn(bHidden);
}

STDMETHODIMP CDynamicBuildEngine::TerminateSpawner()
{
	m_Spawner.TermSpawn();
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::SpawnBuild(IVCCommandLineList* pCmds, LPCOLESTR szDir, BOOL fCheckForComspec, BOOL fIgnoreErrors, 
	IVCBuildErrorContext* pEC, BOOL fAsyncSpawn, long* nRetVal)
{
	CHECK_POINTER_NULL(nRetVal);
	CDirW dirStart;
	if (!dirStart.CreateFromKnown(szDir))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	if (m_bUserCanceled)
	{
		*nRetVal = -2;
		InformUserCancel();
	}
	else
		*nRetVal = m_Spawner.DoSpawn(pCmds, dirStart, this, NULL, fCheckForComspec, fIgnoreErrors, pEC, fAsyncSpawn);
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::get_UserStoppedBuild(VARIANT_BOOL *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = m_Spawner.UserStoppedBuild() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::get_HaveLogging(VARIANT_BOOL *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = HaveLogging() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::LogTrace(int fFlags, BSTR bstrMessage)
{
	if (!HaveLogging())
		return S_OK;	// nothing to do if not logging

	CStringW strMessage = bstrMessage;
	LogTrace((eLogSectionTypes)fFlags, strMessage);
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::get_CurrentLogSection(eLogSectionTypes* peSection)
{
	CHECK_POINTER_NULL(peSection);
	*peSection = GetCurrentLogSection();
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::put_CurrentLogSection(eLogSectionTypes eSection)
{
	SetCurrentLogSection(eSection);
	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::FormCommandLine(BSTR bstrToolExeName, BSTR bstrCmdLineIn, IVCBuildErrorContext* pEC,
	BOOL fRspFileOK, BOOL fLogCommand, BSTR* pbstrCmdLineOut)
{
	CHECK_POINTER_NULL(pbstrCmdLineOut);
	*pbstrCmdLineOut = NULL;

	CStringW strToolExeName = bstrToolExeName;
	CStringW strCmdLine = bstrCmdLineIn;
	BOOL fOK = FormCmdLine(strToolExeName, strCmdLine, pEC, fRspFileOK, fLogCommand);
	*pbstrCmdLineOut = strCmdLine.AllocSysString();

	if (fOK)
		return S_OK;

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
}

STDMETHODIMP CDynamicBuildEngine::FormBatchFile(BSTR bstrCmdLineIn, IVCBuildErrorContext* pEC, BSTR* pbstrCmdLineOut)
{
	CHECK_POINTER_NULL(pbstrCmdLineOut);
	*pbstrCmdLineOut = NULL;

	CStringW strCmdLine = bstrCmdLineIn;
	BOOL fOK = FormBatchFile(strCmdLine, pEC);
	*pbstrCmdLineOut = strCmdLine.AllocSysString();

	if (fOK)
		return S_OK;

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
}

STDMETHODIMP CDynamicBuildEngine::FormTempFile(BSTR bstrCmdLineIn, BSTR bstrComment, BSTR* pbstrCmdLineOut)
{
	CHECK_POINTER_NULL(pbstrCmdLineOut);
	*pbstrCmdLineOut = NULL;

	CStringW strCmdLine = bstrCmdLineIn;
	CStringW strComment = bstrComment;
	BOOL fOK = FormTempFile(strCmdLine, strComment);
	*pbstrCmdLineOut = strCmdLine.AllocSysString();

	if (fOK)
		return S_OK;

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
}

STDMETHODIMP CDynamicBuildEngine::FormXMLCommand(BSTR bstrCmdLineIn, BSTR bstrExe, BSTR bstrExtraArgs, BSTR* pbstrCmdLineOut)
{
	CHECK_POINTER_NULL(pbstrCmdLineOut);
	*pbstrCmdLineOut = NULL;

	CStringW strCmdLine = bstrCmdLineIn;
	CStringW strExe = bstrExe;
	CStringW strExtraArgs = bstrExtraArgs;
	BOOL fOK = FormXMLCommand(strCmdLine, strExe, strExtraArgs);
	*pbstrCmdLineOut = strCmdLine.AllocSysString();

	if (fOK)
		return S_OK;

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
}

STDMETHODIMP CDynamicBuildEngine::GetDeploymentDependencies(void* pcrBuild, IVCBuildStringCollection** ppDeployStrings)
{
	CHECK_POINTER_NULL(ppDeployStrings);
	RETURN_ON_NULL(pcrBuild);

	GetDeploymentDependencies((CBldCfgRecord*)pcrBuild, ppDeployStrings);
	return S_OK;
}

void CDynamicBuildEngine::GetDeploymentDependencies(CBldCfgRecord* pcrBuild, IVCBuildStringCollection** ppDeployStrings)
{
	m_pCfgRecord = pcrBuild;	// cache this off in case of error handling later

	// get our build dependency graph and registry for this config.
	CBldFileDepGraph * pdepgraph = g_StaticBuildEngine.GetDepGraph(pcrBuild);
	if (pdepgraph == NULL)
		return;

	// equivalent to 'CEnvironmentVariableList' of yore
	VCConfiguration* pConfig = pcrBuild->GetVCConfiguration();

    CComPtr<IDispatch>			spDisp;
	pConfig->get_Platform(&spDisp);
   	CComQIPtr<VCPlatform>	spPlatform = spDisp;

	CEnvironmentKeeper envMangler(spPlatform);

	// start the passes
	pdepgraph->StartPasses();

	pdepgraph->RetrieveDeploymentDependencies(pcrBuild, ppDeployStrings);

	// let go of any pretense of knowing about the cfg record anymore
	m_pCfgRecord = NULL;

	// env is restored in EnvDaemon destructor
}

UINT CDynamicBuildEngine::GetCodePage(BOOL bWantConsoleCP /* = TRUE */)
{
	if (s_iConsoleCodePage == 0)
	{
		BOOL bFoundVCSpawn = FALSE;
		CStringW strPath;
		CStringW strVCSpawnPath;
		DWORD dwLen;

		// first, try the system PATH
		dwLen = SearchPathW(NULL, L"vcspawn.exe", NULL, 0, NULL, NULL);
		if (dwLen)
		{
			SearchPathW(NULL, L"vcspawn.exe", NULL, dwLen, strVCSpawnPath.GetBuffer(dwLen+1), NULL);
			strVCSpawnPath.ReleaseBuffer();
			bFoundVCSpawn = TRUE;
		}

		if (!bFoundVCSpawn)	// hmmm.  Try Common7/Tools
		{
			UtilGetCommon7Folder(strVCSpawnPath);
			strVCSpawnPath += L"Tools\\vcspawn.exe";
			CPathW pathSpawn;
			if (pathSpawn.Create(strVCSpawnPath) && pathSpawn.ExistsOnDisk())
				bFoundVCSpawn = TRUE;
		}

		if (!bFoundVCSpawn)	// last ditch effort: try the first platform we can find
		{
			if (g_pProjectEngine)
			{
				CComPtr<IDispatch> spDispPlatforms;
				g_pProjectEngine->get_Platforms(&spDispPlatforms);
				CComQIPtr<IVCCollection> spPlatforms = spDispPlatforms;
				if (spPlatforms)
				{
					CComPtr<IDispatch> spDispPlatform;
					spPlatforms->Item(CComVariant((LONG)1), &spDispPlatform);
					CComQIPtr<VCPlatform> spPlatform = spDispPlatform;
					if (spPlatform)
					{
						CComBSTR bstrPath1, bstrPath2;
						spPlatform->get_ExecutableDirectories(&bstrPath1);
						spPlatform->Evaluate(bstrPath1, &bstrPath2);
						strPath = bstrPath2;
						if (!strPath.IsEmpty())
						{
							dwLen = SearchPathW(strPath, L"vcspawn.exe", NULL, 0, NULL, NULL);
							if (dwLen)
							{
								SearchPathW(strPath, L"vcspawn.exe", NULL, dwLen, strVCSpawnPath.GetBuffer(dwLen+1), NULL);
								strVCSpawnPath.ReleaseBuffer();
								bFoundVCSpawn = TRUE;
							}
						}
					}
				}
			}
		}

		if (!bFoundVCSpawn)		// still no luck.  pray
			strVCSpawnPath = L"vcspawn.exe";

		// Create the spawn string with the vcspawn prepended
		CStringW strSpawn;
		strSpawn.Format(L"\"%s\" -c", strVCSpawnPath);
	
		// check to see if we're allowed to run vcspawn
		
		BOOL fCreateSuccess;	// create process succeeded?
		if( !CanCreateProcess( L"vcspawn.exe" ) )
		{
			fCreateSuccess = FALSE;
		}
		else
		{
			PROCESS_INFORMATION     procInfo;
			STARTUPINFOW            startInfo;
			memset((void *)&procInfo, 0, sizeof(PROCESS_INFORMATION));
			memset((void *)&startInfo, 0, sizeof(STARTUPINFOW));
			startInfo.dwFlags = STARTF_USESHOWWINDOW;
			startInfo.wShowWindow = SW_HIDE;
			CDirW dir;
			dir.CreateFromCurrent();
			CStringW strDir = dir;
			fCreateSuccess = CreateProcessW(NULL, strSpawn.GetBuffer(strSpawn.GetLength()), NULL, NULL, TRUE,
				CREATE_NEW_CONSOLE, NULL, strDir, &startInfo, &procInfo);
			strSpawn.ReleaseBuffer();
	
			DWORD dwError = GetLastError();
			// GetLastError() returns ERROR_ACCESS_DENIED on NT German when path contains extended chars!
			if (!fCreateSuccess && (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_ACCESS_DENIED))
			{
				// Unable to run vcspawn with explicit path, try without...
				strSpawn = L"vcspawn.exe -c";
				fCreateSuccess = CreateProcessW(NULL, strSpawn.GetBuffer(strSpawn.GetLength()), NULL, NULL, TRUE,
					CREATE_NEW_CONSOLE, NULL, strDir, &startInfo, &procInfo);
				strSpawn.ReleaseBuffer();
			}
			if (procInfo.hThread)
				CloseHandle ( procInfo.hThread );
			if( fCreateSuccess )
			{
				WaitForSingleObject ( procInfo.hProcess, INFINITE );
				DWORD dwExit = 0;
				GetExitCodeProcess ( procInfo.hProcess, &dwExit);
				s_iConsoleCodePage = dwExit;
			}
			else
			{
			    // Yikes !
			    // GetThe current system codepage I guess.
			}
			if (procInfo.hProcess)
				CloseHandle ( procInfo.hProcess );
		}
		s_iANSICodePage = CXMLFile::DetermineDefaultCodePage();
		if (s_iConsoleCodePage <= 0)
			s_iConsoleCodePage = s_iANSICodePage;
	}
	if (bWantConsoleCP)
		return s_iConsoleCodePage;
	else
		return s_iANSICodePage;
}

/////////////////////////////////////////////////////////////////////////////
// IENCBuildSpawner Methods

STDMETHODIMP CDynamicBuildEngine::StartSpawn(LPCOLESTR szCurrentDir, LPCOLESTR szCmdLine, 
	IVCBuildErrorContext *pIVCBuildErrorContext, IENCBuildSpawnerCallback *pCallback, BOOL fUseConsoleCP)
{
	BOOL fOK = m_encBldThread.Init(szCmdLine, szCurrentDir, pIVCBuildErrorContext, this, pCallback, fUseConsoleCP);

	if (fOK)
	{
		InitializeForBuild();
		fOK = m_encBldThread.FStart();
	}

	HRESULT hr = (fOK ? S_OK : E_FAIL);
	return hr;
}


STDMETHODIMP CDynamicBuildEngine::StopSpawn()
{
	return StopBuild();
}


STDMETHODIMP CDynamicBuildEngine::IsSysHeader(LPCOLESTR bstrFileName)
{
    CStringW szFileName(bstrFileName);

	if( g_SysInclReg.IsSysInclude(szFileName) )
        return S_OK;
    else
        return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_NOT_SYS_HEADER, bstrFileName, FALSE);
}

void CDynamicBuildEngine::InformUserCancel()
{
	if (m_bUserInformedAboutCancel)
		return;

	m_bUserInformedAboutCancel = TRUE;
	CComBSTR bstrErr;
	bstrErr.LoadString(IDS_TERMINATE_USER_REQ);
	m_spBuildErrorContext->AddInfo(bstrErr);
}
