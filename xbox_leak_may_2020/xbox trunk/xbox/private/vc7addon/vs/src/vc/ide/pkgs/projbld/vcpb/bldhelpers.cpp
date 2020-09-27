#include "stdafx.h"
#pragma hdrstop
#include <vccolls.h>
#include "BuildEngine.h"
#include "msgboxes2.h"
#include "util2.h"
#include "vcprojectengine.h"
#include "BldThread.h"

// #define SHOW_SPAWNER_DIAGNOSTICS		// uncomment this if you want the ATLTRACE calls related to spawning in this file

#define MAX_PROCESS_SIZE 32768 - _MAX_PATH		// this is all the more characters the OS can handle in a single CreateProcess

CTestableSection g_sectionSpawner;

///////////////////////////////////////////////////////////////////////////////
CBldSpawner::CBldSpawner()
{
	m_pSpawner = NULL;
	m_InitCount = 0;
	m_bBuildTerminated = FALSE;

	m_bHidden = FALSE;
	m_fUserStoppedBuild = FALSE;
}

BOOL CBldSpawner::UserStoppedBuild()
{
	return m_fUserStoppedBuild;
}

BOOL CBldSpawner::CanSpawn()
{
	// Prevent access to spawner by other threads.
	CritSectionT cs(g_sectionSpawner);

	return(m_pSpawner == NULL || 
		// return TRUE if we have a hidden active spawn.
		// A hidden spawn will be implicitly terminated 
		// when a regular spawn request is made
		m_bHidden
		);
}

HRESULT CBldSpawner::InitSpawn(BOOL bHidden /* = FALSE */)
{
	if (bHidden) 
		VSASSERT(m_InitCount == 0, "can only init a hidden spawn once");

	m_bBuildTerminated = FALSE;

	if (!m_InitCount)
	{
		// our COMSPEC environment variable
		m_strComspec = L"command.com";

		CComBSTR bstrComspec;
		HRESULT hr = ::VCGetEnvironmentVariableW(L"COMSPEC", &bstrComspec);
		if( SUCCEEDED(hr) && bstrComspec.Length() > 0 )
		{
			// use this and normalise
			// REVIEW: What happens if there are quotes in the comspec already ?
			m_strComspec = L"\"";
			m_strComspec += bstrComspec;
			m_strComspec += L"\"";
			m_strComspec.MakeLower();
		}
		m_InitCount++;
	}

	return S_OK;
}

// implemented somewhere below
extern void GetFirstFile(CStringW&, CStringW&, CStringW&);

#define szVCSpawnEchoCmd	L"~vcecho!"
#ifdef NO	// don't bother showing spawn time anymore
#define szVCSpawnTimeCmd	L"~vctime!"
#endif	// NO

// Note the the integer error codes returned by this function are
// actually	 exit codes of NTSPAWN.	 Look there if you want to know
// what they mean.
int CBldSpawner::DoSpawn(IVCCommandLineList* pCmds, CDirW &dir, CDynamicBuildEngine* pBldEngine, CBldCfgRecord* pCfgRecord,
	BOOL fCheckForComspec, BOOL fIgnoreErrors, IVCBuildErrorContext* pEC, BOOL fAsyncSpawn)
{
	// create the spawner object here--InitSpawn() not called for every DoSpawn()
	VSASSERT(m_pSpawner == NULL, "Can only have one spawn in progress at a time.");
	VSASSERT(!fIgnoreErrors, "Spawner does not support ability to ignore errors."); // not supported

	if (!OkToSpawn(pCmds))
		return 0;

	m_pEC = pEC;
	BOOL fUseConsoleCP;
	{	// block just so we don't use the VARIANT_BOOL version anywhere else but here...
		VARIANT_BOOL bUseConsoleCP;
		pCmds->get_UseConsoleCodePageForSpawner(&bUseConsoleCP);
		fUseConsoleCP = (bUseConsoleCP == VARIANT_TRUE);
	}

	// Just join all the lists together for now.
	// NOTE this must be changed to support parrallel stuff.
	CVCStringWList slCmds;
	CVCStringWList slDesc;
	BuildCommandLineList(pCmds, slCmds, slDesc);

	if (pBldEngine->m_bUserCanceled)
		return HandleUserCancel(pBldEngine);

	// two words
	// - loword == return value
	// - hiword == command that returned the value
	int nRetval = 0;
	int iCmd = 1;

	int nMaxCmdLen = MAX_PROCESS_SIZE;
	CVCStringWList slSpawnCmds;

	BOOL bUserCancel = FALSE;
	BOOL bSpawnFailed = FALSE;
	BOOL bFinished = FALSE;
	BOOL bFirst = TRUE;
	m_fUserStoppedBuild = FALSE;

	VCPOSITION posCmd = slCmds.GetHeadPosition();
	VCPOSITION posDesc = slDesc.GetHeadPosition();
	VCPOSITION posCmdLast = posCmd;
	VCPOSITION posDescLast = posDesc;
	while (!bFinished && !nRetval && !bUserCancel && !bSpawnFailed)
	{
		CreateSpawner(dir); 

		slSpawnCmds.RemoveAll();
		nRetval = 0;
		iCmd = 1;
		int nCurrentLen = 0;

		BOOL bContinue = TRUE;
		while (posCmd != (VCPOSITION)NULL && bContinue)
		{
			posCmdLast = posCmd;
			CStringW& strCmdLine = slCmds.GetNext(posCmd);

			// check to see if we need to prefix with a comspec?
			if (fCheckForComspec)
				HandleCheckForComspec(pCfgRecord, strCmdLine, nRetval, iCmd);

			// add the description
			// as '~vcecho<desc>'
			CStringW strDescLine = szVCSpawnEchoCmd;
			if (fUseConsoleCP)
				strDescLine += L"c";	// tell vcspawn we want the description back in the console CP
			else
				strDescLine += L"s";	// tell vcspawn we want the description back in the system CP
			posDescLast = posDesc;
			strDescLine += slDesc.GetNext(posDesc);

			int nCmdLen = strCmdLine.GetLength();
			int nDescLen = strDescLine.GetLength();

			bContinue = (nCurrentLen + nCmdLen + nDescLen + 2 /* for sep chars */ < nMaxCmdLen);
			if (bContinue)
			{
				slSpawnCmds.AddTail(strDescLine);	// first, add the description
				slSpawnCmds.AddTail(strCmdLine);	// now, add the command
				nCurrentLen += nCmdLen + nDescLen + 2;
			}
			else if (iCmd == 1) // hmmm, didn't get at least one command on the list
				slSpawnCmds.AddTail(strCmdLine);	// give 'em at least the command line we want run...
			else
			{
				posCmd = posCmdLast;
				posDesc = posDescLast;
			}

			// index of the next command
			iCmd += 2;
		}
		bFinished = (posCmd == (VCPOSITION)NULL);

		// already failed the execute in pre-check above?
		if (nRetval)
			ClearSpawner();
		else
			DoActualSpawn(slSpawnCmds, pBldEngine, fUseConsoleCP, nRetval, iCmd, bUserCancel, bSpawnFailed);
	}

	return DetermineSpawnRetval(slSpawnCmds, pBldEngine, pCfgRecord, nRetval, iCmd, bUserCancel, bSpawnFailed);
}

void CBldSpawner::BuildCommandLineList(IVCCommandLineList* pCmds, CVCStringWList& slCmds, CVCStringWList& slDesc)
{
	pCmds->Reset();
	HRESULT hr = S_OK;
	while (hr == S_OK)
	{
		CComPtr<IVCCommandLine> pCmd;
		hr = pCmds->Next(&pCmd);
		BREAK_ON_DONE(hr);
		CComBSTR bstrCmdLine;
		HRESULT hr1 = pCmd->get_CommandLineContents(&bstrCmdLine);
		CONTINUE_ON_FAIL(hr1);
		CComBSTR bstrDescription;
		hr1 = pCmd->get_Description(&bstrDescription);
		CONTINUE_ON_FAIL(hr1);
		CStringW strCmdLine = bstrCmdLine;
		CStringW strDescription = bstrDescription;
		slCmds.AddTail(strCmdLine);
		slDesc.AddTail(strDescription);
	}
	pCmds->RemoveAll();

	VSASSERT(slCmds.GetCount() == slDesc.GetCount(), "Need to make sure number of commands and number of descriptions match.");
}

BOOL CBldSpawner::OkToSpawn(IVCCommandLineList* pCmds)
{
	RETURN_ON_NULL2(pCmds, FALSE);

	long cCmds = 0;
	pCmds->get_Count(&cCmds);
	return (cCmds > 0);
}

int CBldSpawner::HandleUserCancel(CDynamicBuildEngine* pBldEngine)
{
	pBldEngine->InformUserCancel();
	TermSpawn();
	return -2;
}

void CBldSpawner::CreateSpawner(CDirW &dir)
{
	// Prevent access to spawner by other threads.
	CritSectionT cs(g_sectionSpawner);

	m_pSpawner = new CBldConsoleSpawner(&m_pSpawner);

	BOOL bOK = m_pSpawner->InitializeSpawn(dir, FALSE, FALSE, m_pEC);
	VSASSERT(bOK, "Failed to initialize spawner!");
}

void CBldSpawner::ClearSpawner()
{
	// Prevent access to spawner by other threads.
	CritSectionT cs(g_sectionSpawner);

	if (m_pSpawner == NULL)
		return;

	delete m_pSpawner;
	m_pSpawner = NULL;
}

void CBldSpawner::HandleCheckForComspec(CBldCfgRecord* pCfgRecord, CStringW& strCmdLine, int& nRetval, int& iCmd)
{
	// split into command and arguments
	CStringW strCaller, strArgs;
	GetFirstFile(strCmdLine, strCaller, strArgs);

	// found this command?
	wchar_t * psz, szCmdPath[_MAX_PATH + 1];
	BOOL fGotPath = SearchPathW(NULL, strCaller, L".exe", _MAX_PATH, szCmdPath, &psz);
	if (!fGotPath)
	{
		wcscpy(szCmdPath, strCaller);	// we failed, so just use the name
		if (m_pEC)
		{
			CStringW strCaller2 = strCaller;
			if (strCaller != strCaller2)	// uh, oh, conversion problem...
			{
				CStringW strMsg;
				strMsg.Format(IDS_ERR_UNICODE_PATH_PRJ0024, strCaller);
				CVCProjectEngine::AddProjectError(m_pEC, strMsg, L"PRJ0024", pCfgRecord);
			}
		}
	}

	// is this a GUI command? (assume no if not found)
	BOOL fIsGUI = FALSE;
	BOOL fUseComSpec = TRUE;
	if (fGotPath)
	{
		WORD wSubSystem;
		WORD wExeType = (WORD)GetExeType(szCmdPath, &wSubSystem);

		fIsGUI = wExeType == EXE_WIN || ((wExeType == EXE_NT) && (wSubSystem == IMAGE_SUBSYSTEM_WINDOWS_GUI));
		fUseComSpec = wExeType == EXE_NONE || wExeType == EXE_FAIL;
	}

	// make sure not executing command shell
	BOOL fIsShell = FALSE;
	if (!fIsGUI)
	{
		CPathW pathComspec;
		if( pathComspec.Create(m_strComspec) )	
		{
			CStringW strCmdPathTmp = szCmdPath;		
			strCmdPathTmp.MakeLower();
			fIsShell = wcsstr(strCmdPathTmp, pathComspec) != NULL;
			fUseComSpec = fUseComSpec && !fIsShell;
		}
		else if (m_pEC)
		{
			// hmmmm, Unicode problem, maybe?
			// REVIEW: this is a weird test. Do we need it ?
			CStringA strTmpA = m_strComspec;
			CStringW strTmpW = strTmpA;
			if (strTmpW != m_strComspec)
			{
				CStringW strMsg;
				strMsg.Format(IDS_ERR_UNICODE_PATH_PRJ0024, m_strComspec);
				CVCProjectEngine::AddProjectError(m_pEC, strMsg, L"PRJ0024", pCfgRecord);
			}
		}
	}

	// use the command-spec if we have re-direction chars.
	fUseComSpec = fUseComSpec || (strCmdLine.FindOneOf(L"<>|&^") != -1);

	// don't allow spawning just the shell!
	if (fIsShell && strArgs.IsEmpty())
	{
		// fail the execute
		// make a note + index of command for multi-commands
		nRetval = 1 | (iCmd << 16);
	}

	// need to prefix with comspec?
	else if (fUseComSpec)
	{
		strCmdLine = szCmdPath;
		strCmdLine += L" ";
		strCmdLine += strArgs;
	}
}

void CBldSpawner::DoActualSpawn(CVCStringWList& slSpawnCmds, CDynamicBuildEngine* pBldEngine, BOOL fUseConsoleCP, 
	int& nRetval, int& iCmd, BOOL& bUserCancel, BOOL& bSpawnFailed)
{
	{
		// Prevent access to spawner by other threads.
		CritSectionT cs(g_sectionSpawner);

		if (m_pSpawner)
			m_pSpawner->m_hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	if (!m_pSpawner || !m_pSpawner->PerformSpawnMultiple(&slSpawnCmds, pBldEngine, fUseConsoleCP))
	{
		bSpawnFailed = TRUE;
	}
	else
	{
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("DoActualSpawn: About to wait for hNotifyEvent\n");
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		BOOL bDoIdleAndPump = FALSE;
		if (m_pEC)
		{
			CComPtr<IDispatch> spDispBuildEngine;
			m_pEC->get_AssociatedBuildEngine(&spDispBuildEngine);
			CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = spDispBuildEngine;
			if (spBuildEngineImpl)
				spBuildEngineImpl->get_DoIdleAndPump(&bDoIdleAndPump);
		}
		if (m_pSpawner && m_pSpawner->m_hNotifyEvent)
			IdleAndPump(m_pSpawner->m_hNotifyEvent, bDoIdleAndPump);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("DoActualSpawn: Finished waiting for hNotifyEvent\n");
#endif	// SHOW_SPAWNER_DIAGNOSTICS

		{
			// Prevent access to spawner by other threads.
			CritSectionT cs(g_sectionSpawner);

			if (m_pSpawner)
				nRetval = m_pSpawner->m_dwTermStatus;
		}
					
		// check to see if user cancelled the build
		if (WaitForSingleObject(m_pSpawner->m_hCancelEvent, 0) == WAIT_OBJECT_0)
			bUserCancel = TRUE;
		
		HANDLE hProcess = GetCurrentProcess();
		HANDLE hThread = NULL;
		BOOL bOK;
		{
			// Prevent access to spawner by other threads.
			CritSectionT cs(g_sectionSpawner);

			bOK = DuplicateHandle(hProcess, m_pSpawner->m_hWaitThread, hProcess, &hThread, DUPLICATE_SAME_ACCESS, TRUE, 
				DUPLICATE_SAME_ACCESS);
			VSASSERT(bOK, "Failed to duplicate handle for spawner wait thread!");
#ifdef SHOW_SPAWNER_DIAGNOSTICS
			ATLTRACE("DoActualSpawn: Set hCleanEvent\n");
#endif	// SHOW_SPAWNER_DIAGNOSTICS
			SetEvent(m_pSpawner->m_hCleanEvent);
		}
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("DoActualSpawn: About to wait for hWaitThread\n");
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		WaitForSingleObject(hThread, INFINITE);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("DoActualSpawn: Finished waiting for  hWaitThread\n");
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		{
			// Prevent access to spawner by other threads.
			CritSectionT cs(g_sectionSpawner);
			bOK = CloseHandle(hThread);
			VSASSERT(bOK, "Failed to close handle for spawner wait thread!");
		}
		if( m_fUserStoppedBuild )
			m_InitCount=0;
	}
}

int CBldSpawner::DetermineSpawnRetval(CVCStringWList& slSpawnCmds, CDynamicBuildEngine* pBldEngine, CBldCfgRecord* pCfgRecord, 
	int& nRetval, int& iCmd, BOOL& bUserCancel, BOOL& bSpawnFailed)
{
	// unpack return value and index of command that failed
	// for multiple commands
	iCmd = (int)((nRetval >> 16) & 0xffff);
	nRetval = (int)(short)(nRetval & 0xffff);

	// why did we fail the execute?
	VCPOSITION pos = slSpawnCmds.FindIndex(iCmd);
	if (nRetval && pos != (VCPOSITION)NULL)
	{
		CStringW& strCmdLine = slSpawnCmds.GetAt(pos);
		// load the error string
		UINT nIDError;
		CComBSTR bstrPrjErrCode;
		if (nRetval == -1)
		{
			nIDError = IDS_FAIL_SPAWN_PRJ0003;
			bstrPrjErrCode = L"PRJ0003";
		}
		else if (bUserCancel)
		{
			pBldEngine->InformUserCancel();
			nRetval = -2;
			m_fUserStoppedBuild = FALSE;
			return nRetval;
		}
		else
		{
			long nError = 0;
			if (m_pEC)
				m_pEC->get_Errors( &nError );
			if( nError )
			{
				// we have already printed this error probably so bail now
				m_fUserStoppedBuild = FALSE;
				return nRetval;
			}
			nIDError = IDS_FAIL_EXECUTE_PRJ0002;
			bstrPrjErrCode = L"PRJ0002";
		}

		CStringW strErr;
		FormatError(nIDError, strCmdLine, strErr);

		m_fUserStoppedBuild = FALSE;
		CVCProjectEngine::AddProjectError(m_pEC, strErr, bstrPrjErrCode, pCfgRecord);
	}
	else if (bSpawnFailed)
	{
		nRetval = -1;	// since it obviously didn't get set some other way...
	}

	if (bUserCancel)
		nRetval = -2;

	m_fUserStoppedBuild = FALSE;
	return nRetval;
}

void CBldSpawner::FormatError(int nIDError, CStringW& strCmdLine, CStringW& rstrError)
{
	// Get end of exe name:
	const wchar_t * pchCmdLine = (const wchar_t *)strCmdLine;
	unsigned short nExeNameLen = 0;
	const wchar_t * pchEndQuote = 0;
	if (pchCmdLine[0] == L'"' && GetQuotedString(pchCmdLine, pchEndQuote)) {
		// ENC: handle quoted commands that may contain spaces
		// 12/22/97 georgiop [vs98 23180]
		nExeNameLen = (unsigned short)(pchEndQuote - pchCmdLine);
	}
	else while (pchCmdLine[nExeNameLen] != L'\0' &&
		   !iswspace(pchCmdLine[nExeNameLen])
		  )
	{
		nExeNameLen++;
	}

	CStringW strErr;
	BOOL bOK = strErr.LoadString(nIDError);
	VSASSERT(bOK, "Failed to load a string; are the resources initialized properly?");
		
	// format the error message
	rstrError.Format(strErr, nExeNameLen, nExeNameLen, pchCmdLine);
}

void CBldSpawner::TermSpawn()
{
//	Called at end of spawning session.
	if (--m_InitCount > 0) 
		return;

	m_InitCount = 0;
}

void CBldSpawner::DoSpawnCancel()
{
	VSASSERT (!m_bHidden, "DoSpawnCancel called from hidden spawn.");
	m_bBuildTerminated = TRUE;

//	VSASSERT(m_pSpawner != NULL, "Cannot cancel spawner if we did not start it to begin with...");
	m_fUserStoppedBuild = TRUE;
	{
		// Prevent access to spawner by other threads.
		CritSectionT cs(g_sectionSpawner);

		if (m_pSpawner)
			m_pSpawner->CancelSpawn();
		else
			TermSpawn();
	}
}

void CBldSpawner::ReInitSpawn()
{
//	Called after DoSpawnCancel to reinitialise the
//	cancelled spawner
	VSASSERT(!m_bHidden, "ReInitSpawn called from hidden spawn.");
	m_bBuildTerminated = FALSE;
	m_InitCount = 1;
}

// REVIEW: is this really needed anymore?  Apparently in the old shell it was (VS98 9177), but is that
// still relevant in VS7?  Maybe WaitForSingleObject is sufficient...
void CBldSpawner::IdleAndPump(HANDLE hEventEarlyOut, BOOL fIdleAndPump)
{
	// This is the essence of Idle and pump (without the idle part)
	if (fIdleAndPump)
	{
		while (::MsgWaitForMultipleObjects(1, &hEventEarlyOut, FALSE, INFINITE, QS_ALLEVENTS) == WAIT_OBJECT_0 + 1) 
		{
			// we have a message
			MSG msg;
			if (!::GetMessage(&msg, NULL, NULL, NULL))
			{
				continue;
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	else
	{
		WaitForSingleObject(hEventEarlyOut, INFINITE);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------- CTempMaker ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////

unsigned long CTempMaker::s_ulSeed = 0;

CTempMaker::~CTempMaker()
{
	if (!m_Files.IsEmpty())
		NukeFiles();
}

BOOL CTempMaker::CreateTempFile(CVCStdioFile *&pFile, CPathW& rPath, IVCBuildErrorContext* pEC, 
	CBldCfgRecord* pCfgRecord, int fFileType /* =FILE_RESPONSE */)
{
	// Get the temp path if we don't alread have it:
	if (m_Files.IsEmpty())
	{
		BOOL bContinue = FALSE;
		if (pCfgRecord)
		{
			VCConfiguration* pProjCfg = pCfgRecord->GetVCConfiguration();	// not ref-counted
			CComQIPtr<IVCPropertyContainer> spPropContainer = pProjCfg;
			if (spPropContainer)
			{
				ConfigurationTypes configType = typeUnknown;
				spPropContainer->GetIntProperty(VCCFGID_ConfigurationType, (long *)&configType);
				switch (configType)
				{
				case typeApplication:
				case typeDynamicLibrary:
				case typeStaticLibrary:
					{
						CComBSTR bstrProjDir, bstrIntDir;
						spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);	// never contains macros
						spPropContainer->GetEvaluatedStrProperty(VCCFGID_IntermediateDirectory, &bstrIntDir);
						if (bstrIntDir.Length() > 2)	// need AT LEAST three characters to be valid...
						{
							if (bstrIntDir[0] != L'/' && bstrIntDir[0] != L'\\' && bstrIntDir[1] != L'/' && bstrIntDir[1] != L'\\')
							{	// don't allow UNC paths for temp files...
								CDirW dirProj;
								CPathW pathIntDir;
								if (dirProj.CreateFromKnown(bstrProjDir)
									&& pathIntDir.CreateFromDirAndFilename(dirProj, bstrIntDir)
									&& m_DirTempPath.CreateFromPath(pathIntDir, TRUE))
									bContinue = TRUE;
							}
						}
					}
					break;
//				case typeUnknown:
//				case typeGeneric:
				default:
					bContinue = FALSE;	// gonna need to use TEMP
					break;
				}
			}
		}
		if (!bContinue)	// fall back and try to use TEMP
		{
			bContinue = m_DirTempPath.CreateTemporaryName();
			m_bMustBeNew = true;
		}
		if (!bContinue)
		{
			if (pEC)
			{
				CComBSTR bstrError;
				bstrError.LoadString(IDS_COULD_NOT_GET_TEMP_PATH);
				pEC->AddInfo(bstrError);
			}
			// OS call failed, use CWD:
			if (!m_DirTempPath.CreateFromCurrent())
			{
				if (pEC)
				{
					CComBSTR bstrError;
					bstrError.LoadString(IDS_COULD_NOT_GET_CWD);
					pEC->AddInfo(bstrError);
				}
				return FALSE;
			}
		}
	}

	CStringW strPre, strExt, strName;
	switch(fFileType)
	{
	case FILE_RESPONSE:
		strPre = L"RSP";
		strExt = L".rsp";
		break;
	case FILE_BATCH:
		strPre = L"BAT";
		strExt = L".bat";
		break;
	case FILE_XML:
		strPre = L"XML";
		strExt = L".xml";
		break;
//	case FILE_TEMP:
	default:
		strPre = L"TMP";
		strExt = L".tmp";
	}

	s_ulSeed++;
	strName.Format(L"%s%.6X%s", strPre, s_ulSeed, strExt);
	if (!rPath.CreateFromDirAndFilename(m_DirTempPath, strName))
	{
		if (pEC)
		{
			CStringW strErr;
			strErr.Format(IDS_COULD_NOT_GET_TEMP_NAME_PRJ0005, (const wchar_t*)m_DirTempPath);
			CComBSTR bstrError = strErr;
			CVCProjectEngine::AddProjectError(pEC, bstrError, L"PRJ0005", pCfgRecord);
		}
		return FALSE;
	}
	if (m_bMustBeNew && rPath.ExistsOnDisk())
	{
		while (rPath.ExistsOnDisk())
		{
			s_ulSeed++;
			strName.Format(L"%s%.6X%s", strPre, s_ulSeed, strExt);
			if (!rPath.CreateFromDirAndFilename(m_DirTempPath, strName))
			{
				if (pEC)
				{
					CStringW strErr;
					strErr.Format(IDS_COULD_NOT_GET_TEMP_NAME_PRJ0005, (const wchar_t*)m_DirTempPath);
					CComBSTR bstrError = strErr;
					CVCProjectEngine::AddProjectError(pEC, bstrError, L"PRJ0005", pCfgRecord);
				}
				return FALSE;
			}
		}
	}

	CStringW strPathW;
	rPath.GetFullPath(strPathW);
	CStringA strPathA = strPathW;	// yes, ANSI -- 'cause CVCFile wants ANSI
	DWORD fOpen = CVCFile::modeWrite | CVCFile::modeCreate | CVCFile::typeText;
	pFile = new CVCStdioFile;	

	if (!pFile->Open(strPathA, fOpen, FILE_ATTRIBUTE_TEMPORARY))
	{
		if (pEC)
		{
			CStringW str;
			CComBSTR bstrErr;
			CStringW strPath2 = strPathA;
			if (strPath2 != strPathW)
			{
				str.Format(IDS_ERR_UNICODE_PATH_PRJ0024, strPathW);
				bstrErr = L"PRJ0024";
			}
			else
			{
				::VCMsgTextW(str, IDS_COULD_NOT_OPEN_TEMP_FILE_PRJ0006, rPath.GetFullPath());
				bstrErr = L"PRJ0006";
			}
			CVCProjectEngine::AddProjectError(pEC, str, bstrErr, pCfgRecord);
		}
		delete (pFile);
		return FALSE;
	}

	m_Files.SetAt(rPath, pFile);
	return TRUE;
}	

// Remove all the files from disk:
void CTempMaker::NukeFiles(IVCBuildErrorContext* pEC/*=NULL*/)
{
	CPathW cp;
	VCPOSITION pos;
	CVCStdioFile *pFile;

	for (pos = m_Files.GetStartPosition(); pos != NULL; )
	{
		m_Files.GetNextAssoc(pos, cp, (void *&)pFile);

		// Check that the file has already been closed:
		VSASSERT(pFile->m_hFile == CVCFile::hFileNull, "File isn't closed before nuking!");
		delete (pFile);

		if (!::DeleteFileW((const wchar_t *) cp))
		{
			CStringW str;
			::VCMsgTextW(str, cp, IDS_COULD_NOT_DELETE_TEMP_FILE);
			CComBSTR bstrInfo = str;
			pEC->AddInfo(bstrInfo);
		}
	}
	m_Files.RemoveAll();
}

BOOL CTempMaker::CreateTempFileFromString(CStringW& str, CPathW &rPath, CDynamicBuildEngine* pBuildEngine, 
	IVCBuildErrorContext* pEC, CBldCfgRecord* pCfgRecord, int fFileType /* = FILE_RESPONSE */)
{
	CVCStdioFile *pFile;
	if (!CreateTempFile(pFile, rPath, pEC, pCfgRecord, fFileType))
		return FALSE;
	BOOL bRetval;
	CStringW strCreate;
	strCreate.LoadString(IDS_CREATING_TEMPFILE);

	VARIANT_BOOL bHaveLogging = VARIANT_FALSE;
	if (pBuildEngine)
		pBuildEngine->get_HaveLogging(&bHaveLogging);
	if (bHaveLogging == VARIANT_TRUE)
	{
		CStringW strTranslatedContents;
		// have to escape any '<' characters we see or they won't show in the build log properly
		int nLen = str.GetLength();
		int nLastBracket = -1;
		int nNextBracket = str.Find(L'<');
		if (nNextBracket < 0)	// nothing to escape
			strTranslatedContents = str;
		else
		{
			if (nNextBracket == 0)	// started with one
			{
				strTranslatedContents = L"&lt;";
				nLastBracket = 0;
				nNextBracket = str.Find(L'<', 1);
			}
			while (nNextBracket > 0)
			{
				CStringW strMid = str.Mid(nLastBracket+1, nNextBracket-nLastBracket-1);
				strTranslatedContents += strMid;
				strTranslatedContents += L"&lt;";
				nLastBracket = nNextBracket;
				nNextBracket = str.Find(L'<', nLastBracket+1);
			}
			if (nLastBracket < nLen-1)
				strTranslatedContents += str.Right(nLen-nLastBracket-1);
		}

		CStringW strContents;
		strContents.Format((const wchar_t *)strCreate, (const wchar_t *)rPath, (const wchar_t *)strTranslatedContents);
		CComBSTR bstrContents = strContents;
		pBuildEngine->LogTrace((int)eLogCommand, (BSTR)bstrContents);
	}

	CStringA strA;	// yes, ANSI -- 'cause we're writing an ANSI file
	BOOL bUsedDefault = FALSE;
	UINT iCodePage = pBuildEngine->GetCodePage(fFileType == FILE_BATCH);
	if (iCodePage)
	{
		int nLen = str.GetLength();
		wchar_t* szOriginal = str.GetBuffer(nLen+1);
		unsigned int buf_size = CVCProjectEngine::ConvertFromUnicodeMlang(iCodePage, TRUE, TRUE, szOriginal, nLen, NULL, 0, NULL);
		if (buf_size)
		{
			char* buffer = new char[buf_size+1];
			RETURN_ON_NULL2(buffer, FALSE);
			buffer[buf_size] = 0;

			// convert
			CVCProjectEngine::ConvertFromUnicodeMlang(iCodePage, TRUE, TRUE, szOriginal, nLen, buffer, buf_size, &bUsedDefault);
			strA = buffer;
			delete[] buffer;
		}
		str.ReleaseBuffer();
	}
	else
	{
		strA = str;
		CStringW strW = strA;	// checking for whether it translated back the same
		bUsedDefault = (str != strW);
	}
	if (bUsedDefault && pEC)
	{
		CStringW strMsg;
		CComBSTR bstrErr;
		switch (fFileType)
		{
		case FILE_BATCH:
			strMsg.Format(IDS_ERR_UNICODE_BAT_PRJ0025, (const wchar_t*)rPath, str);
			bstrErr = L"PRJ0025";
			break;
		case FILE_TEMP:
			strMsg.Format(IDS_ERR_UNICODE_TEMP_PRJ0028, (const wchar_t*)rPath, str);
			bstrErr = L"PRJ0028";
			break;
		case FILE_XML:
			strMsg.Format(IDS_ERR_UNICODE_XML_PRJ0035, (const wchar_t*)rPath, str);
			bstrErr = L"PRJ0035";
			break;
//		case FILE_RESPONSE:
		default:
			strMsg.Format(IDS_ERR_UNICODE_RSP_PRJ0026, (const wchar_t*)rPath, str);
			bstrErr = L"PRJ0026";
			break;
		}
		CVCProjectEngine::AddProjectError(pEC, strMsg, bstrErr, pCfgRecord);
	}

	bRetval = pFile->WriteString(strA);
	if (bRetval)
		pFile->Close();
	else
	{
		if (bHaveLogging)
		{
			CComBSTR bstrCmd;
			bstrCmd.LoadString(IDS_ERR_CREATE_TEMP_FILE);
			pBuildEngine->LogTrace((int)eLogCommand, (BSTR)bstrCmd);
		}
		pFile->Abort();
	}
	return bRetval;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT CVCBuildStringCollection::CreateInstance(IVCBuildStringCollection** ppStrings, CVCBuildStringCollection** ppStringColl)
{
	CHECK_POINTER_NULL(ppStrings);
	*ppStrings = NULL;
	if (ppStringColl)
		*ppStringColl = NULL;

	CComObject<CVCBuildStringCollection> *pObj = NULL;
	HRESULT hr = CComObject<CVCBuildStringCollection>::CreateInstance(&pObj);
	if (SUCCEEDED(hr) && pObj != NULL)
	{
		CVCBuildStringCollection *pVar = pObj;
		if (pVar)
			pVar->AddRef();
		*ppStrings = pVar;
		if (ppStringColl)
			*ppStringColl = pVar;
	}
	return(hr);
}

STDMETHODIMP CVCBuildStringCollection::Reset() 
{ 
	m_nIndex = 0;
	return S_OK; 
}

STDMETHODIMP CVCBuildStringCollection::get_Count(long *pVal)
{ 
	CHECK_POINTER_NULL(pVal);
	*pVal = (long) m_strStrings.GetSize();
	return S_OK;
}

STDMETHODIMP CVCBuildStringCollection::Next(BSTR* pbstrElem)
{
	CHECK_POINTER_NULL(pbstrElem);
	*pbstrElem = NULL;
	if (m_nIndex == m_strStrings.GetSize())
		return S_FALSE;
	CStringW strElem = m_strStrings.GetAt(m_nIndex);
	m_nIndex++;
	*pbstrElem = strElem.AllocSysString();
	return S_OK;
}

STDMETHODIMP CVCBuildStringCollection::Add(BSTR bstrVal)
{
	m_strStrings.Add(bstrVal);
	return S_OK;
}

STDMETHODIMP CVCBuildStringCollection::AddAtLocation(long nLocation, BSTR bstrVal)
{	// add the element at the specified index; 1-based
	if (nLocation < 1)
		RETURN_INVALID();
	m_strStrings.InsertAt(nLocation-1, bstrVal);
	return S_OK;
}

STDMETHODIMP CVCBuildStringCollection::Remove(long nLocation)
{	// remove the element at the specified index; 1-based
	if (nLocation < 1)
		RETURN_INVALID();
	m_strStrings.RemoveAt(nLocation-1);
	return S_OK;
}

STDMETHODIMP CVCBuildStringCollection::Clear()	// remove all elements
{
	m_strStrings.RemoveAll();
	return S_OK;
}

STDMETHODIMP CVCBuildStringCollection::RemoveDuplicates(VARIANT_BOOL bIsPaths, VARIANT_BOOL bIsCaseSensitive)
{
	// weed out any dupes we may have in the list
	CVCMapStringWToStringW mapStrings;
	CStringW strString, strStringLower;
	INT_PTR cStrings = m_strStrings.GetSize();
	for (INT_PTR idx = 0; idx < cStrings; idx++)
	{
		strString = m_strStrings.GetAt(idx);
		if (strString.IsEmpty())
			continue;

		strStringLower = strString;
		if (!bIsCaseSensitive)
			strStringLower.MakeLower();
		mapStrings.SetAt(strStringLower, strString);
	}
	m_strStrings.RemoveAll();

	VCPOSITION pos = mapStrings.GetStartPosition();
	while (pos != (VCPOSITION)NULL)
	{
		mapStrings.GetNextAssoc(pos, strStringLower, strString);
		m_strStrings.Add(strString);
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////

CBldTurnRegRecalcOff::CBldTurnRegRecalcOff(CBldFileRegSet* pRegSet)
{ 
	m_pRegSet = pRegSet; 
	if (pRegSet != NULL)
		m_fOldOn = m_pRegSet->CalcAttributes(FALSE);
	else
		m_fOldOn = FALSE;
}

CBldTurnRegRecalcOff::~CBldTurnRegRecalcOff()
{ 
	if (m_pRegSet != NULL)
		m_pRegSet->CalcAttributes(m_fOldOn); 
}

//////////////////////////////////////////////////////////////////////////////
// implementation of the CBldOptionList class
#pragma intrinsic(memcpy)

void CBldOptionList::SetString(const wchar_t* pch)
{
	VSASSERT(pch != (const wchar_t *)NULL, "Trying to set the option list to a NULL string.");

	// empty the old one
	Empty();

	const wchar_t* rgchJoin = (const wchar_t *)m_strJoin;

	//
	// Use rgchJoinAndSpace to eliminate spaces surrounding separators.	 This is a quick
	// fix to prevent the usability problem of leading and trailing spaces in tokens. (Speed:49).
	//
	// NOTE: This fix will prevent the obscure but legal use of path names with leading spaces.
	//

	// allocate enough to copy grchJoin and append space.
	wchar_t* rgchJoinAndSpace = new wchar_t[wcslen(rgchJoin)+2];
	wcscpy(rgchJoinAndSpace,rgchJoin);
	wcscat(rgchJoinAndSpace,L" ");

	// skip leading join chars and spaces.
	while (*pch != L'\0' && wcschr(rgchJoinAndSpace, (unsigned int)(int)*pch) != (wchar_t *)NULL)	
		pch++;	// join chars assumed to be SBC

	// anything to do?
	if (*pch == L'\0')
	{
		delete [] rgchJoinAndSpace;
		return; // do nothing
	}

	wchar_t chVal, chJoin = m_strJoin[0];
	VSASSERT(chJoin != L'\0', "Trying to use \0 as separator character in build option list");

	BOOL fQuoted = FALSE;
	const wchar_t * pchWord = pch;
	size_t cch, cchWord = 0;
	do
	{
		chVal = *pch;

		// are we quoted?
		if (chVal == L'"')
			fQuoted = !fQuoted;

		// look for a non-quoted join char, or terminator
		if (chVal == L'\0' || (wcschr(rgchJoin, chVal) != NULL && (chVal == L'\t' || !fQuoted)))
		{
			// skip any extra join chars (and spaces) with this one, ie. ' , '
			if (chVal != L'\0')
			{
				chVal = *(++pch);	// join chars assumed to be SBC
				while (chVal != L'\0' && wcschr(rgchJoinAndSpace, chVal) != NULL)
				{												
					pch++; chVal = *pch;	// join chars assumed to be SBC
				}
			}

			// Remove trailing blanks from pchWord.
			while (0 != cchWord && 0 == _wccmp(_wcsdec(pchWord, pchWord+cchWord), L" "))
				--cchWord;

			// do we have any word characters?
			if (cchWord != 0)	
				Append(pchWord, (int) cchWord);

			cchWord = 0;	// ready for start of next word
			pchWord = pch;
		}
		else
		{
			// skip non-join char
			cch = _wclen(pch);
			cchWord += cch; pch += cch;
		}
	}
	while (chVal != L'\0');

	delete [] rgchJoinAndSpace;
	return;
}

void CBldOptionList::GetString(CStringW& str)
{
	wchar_t chJoin = m_strJoin[0];
	VSASSERT(chJoin != L'\0', "Trying to use \0 as separator character in build option list.");

	// allocate our buffer
	wchar_t* pch;
	if (!m_cchStr ||	// empty string?
		(pch = new wchar_t[m_cchStr]) == (wchar_t *)NULL)
	{
		str = L""; // in case of error
		return;
	}

	wchar_t* pchWord = pch;
	VCPOSITION pos = m_lstStr.GetHeadPosition();
	VSASSERT(pos != (VCPOSITION)NULL, "Option list is empty.");
	for (;;)	// we'll break (more efficient!)
	{
		BldOptEl* poptel = (BldOptEl *)m_lstStr.GetNext(pos);

		int cch = poptel->cch;
		if( cch > 0 )
		{
			memcpy(pchWord, poptel->pch, cch*sizeof(wchar_t));
	
			if (pos != (VCPOSITION)NULL)
				pchWord[cch-1] = chJoin;	// join *not* terminate
			else
				break;	// terminate!
	
			pchWord += cch;
		}
	}

	// return string and then delete our local buffer
	str = pch;
	delete [] pch;
}

void CBldOptionList::Empty()
{
	// free-up our string memory
	VCPOSITION pos = m_lstStr.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		BldOptEl* poptel = (BldOptEl *)m_lstStr.GetNext(pos);
		poptel->cref--; // decr. the ref. count
		if (poptel->cref == 0)	// if zero, then free-up the element
			delete [] (BYTE *)poptel;
	}
	m_lstStr.RemoveAll();
	m_mapStrToPtr.RemoveAll();
}

void CBldOptionList::Add(BldOptEl* poptel)
{
	// put this into our list
	poptel->cref++; // bump. ref. count.
	VCPOSITION posOurWord = m_lstStr.AddTail((void *)poptel);
	m_cchStr += poptel->cch;

	// remember this for quick 'does string exist in list?' query
	// (in the case of dupes, we might nuke the old duplicate but that
	//	doesn't matter, it'll still solve its purpose)
	m_mapStrToPtr.SetAt(poptel->pch, (void *)posOurWord);
}

void CBldOptionList::Append(const wchar_t* pch, int cch)
{
	BldOptEl* poptel = (BldOptEl *)new BYTE[sizeof(BldOptEl) + cch*sizeof(wchar_t)];
	if (poptel == (BldOptEl *)NULL) return;

	// initialise with a ref. count of 1
	memcpy(poptel->pch, pch, cch*sizeof(wchar_t));
	poptel->pch[cch] = L'\0';
	poptel->cch = cch + 1;
	poptel->cref = 0;

	// can we add, ie. 
	// do we want to check for duplicates?
	if (!Exists(poptel->pch))
		Add(poptel);	// put this into our list
	else
		delete [] (BYTE *)poptel;	// duplicate
}

void CBldOptionList::Append(CBldOptionList & optlst)
{
	VCPOSITION pos = optlst.m_lstStr.GetHeadPosition();
	while (pos != NULL)
	{
		BldOptEl* poptel = (BldOptEl *)optlst.m_lstStr.GetNext(pos);
		if (!Exists(poptel->pch))
			Add(poptel);
	}
}

/////////////////////////////////////////////////////////////////////////////////

BOOL BldFileNameMatchesExtension(LPCOLESTR szPath, const wchar_t* pExtensions)
{
	CStringW strPath = szPath;
	if (strPath.IsEmpty())
		return FALSE;

	CPathW path;
	path.CreateFromKnown(strPath);	// don't really care what the case is...

	return BldFileNameMatchesExtension(&path, pExtensions);
}

BOOL BldFileNameMatchesExtension(const CPathW* pPath, const wchar_t* pExtensions)
{
	RETURN_ON_NULL2(pPath, FALSE);

	const wchar_t * pExtNext = pExtensions;
	const wchar_t * pFileExt = pPath->GetExtension();
	if (*pFileExt) pFileExt++;
		// GetExtension() returns pointer to '.'

	INT_PTR n = wcslen (pFileExt);

	if (n)
	{
		while (*pExtNext != L'\0')

		{
			pExtNext = pExtensions;
			while (*pExtNext != L';' && *pExtNext != L',' && *pExtNext != L'\0') 
				pExtNext++;

			// skip over the optional '*.'
			if (pExtensions[0] == L'*' && pExtensions[1] == L'.')
				pExtensions += 2;
			else if (pExtensions[0] == L'.')
				pExtensions += 1;

			// compare extensions in case-insensitive fashion
			if ((n == (pExtNext - pExtensions)) &&			// check for ext. length equivalence
				_wcsnicmp (pFileExt, pExtensions, n) == 0)	// check for ext. string equivalence
				return TRUE;

			pExtensions = pExtNext + 1; // skip over the sep.
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
void SkipInitialWSpace(CStringW& strSrc, CStringW& strDest)
{
	// strip initial whitespace
	size_t cchWhiteSpace = 0;
	wchar_t * pch = (wchar_t *)(const wchar_t *)strSrc; wchar_t ch = *pch;
	while (ch != L'\0')
	{
		// is this whitespace?
		if (ch != L' ' && ch != L'\t' && ch != L'\r' && ch != L'\n') 
			break;

		cchWhiteSpace += _wclen(pch);
		ch = *(pch + cchWhiteSpace);
	}

	strDest = (wchar_t *)(const wchar_t *)strSrc + cchWhiteSpace;
}

/////////////////////////////////////////////////////////////////////////////////
void GetFirstFile(CStringW& strSrc, CStringW& strFirstFile, CStringW& strRemainder)
{
	// get rid of white space first
	CStringW strTemp;
	SkipInitialWSpace(strSrc, strTemp);
	if (!strTemp.IsEmpty() && strTemp[0]==L'\"') // For NTFS command args, need to handled quoted names
	{
		strFirstFile = L'\"' + strTemp.Mid(1).SpanExcluding(L"\"") + L'\"';
		BOOL bBatchFile = (strFirstFile.Find(L".bat") != -1) || (strFirstFile.Find(L".cmd") != -1);

		if (strFirstFile.CompareNoCase(L"\"\"")==0)
			bBatchFile = (strTemp.Find(L".bat") != -1) || (strTemp.Find(L".cmd") != -1);

		if (bBatchFile && (strTemp.GetAt(strTemp.GetLength()-1) == L'\"'))
			strFirstFile = strTemp; // we are looking at batch file, we need to take the whole command
	}
	else
		strFirstFile = strTemp.SpanExcluding (L" \r\t\n") ;
	strRemainder = strTemp.Mid(strFirstFile.GetLength());
}

BOOL GetQuotedString(const wchar_t*& pBase, const wchar_t*& pEnd)
{				
	VSASSERT(pBase, "Cannot quote a NULL string.");

	while (*pBase && *pBase != L'"')
		 pBase++;

	if (*pBase != L'"')
		return FALSE;

	pEnd = ++pBase;		// Go past quote.

	while (*pEnd && *pEnd != L'"')
		pEnd++;

	if (pBase == pEnd || *pEnd != L'"') 
		return FALSE;

	return TRUE;
} 

///////////////////////////////////////////////////////////////////////////////
// Get output file name for a projitem based on options pairs like /Fm and
// map file name, accounting for output directories.
//
// Basic plan is to check the generate flag (if present) and then get the name
// from the item.  If the returned name ends in a back slash (or forward slash)
// then its acutally an output directory, relative the the _project_ directory,
// since this is what the compiler & linker do.	 If the output directory starts
// in a backslash or drive letter combo then its an absolute name.

// To get the filename (if needed), we take the item's file name and change
// extension to the supplied extension.
BOOL GetItemOutputFile(IVCBuildAction* pBldAction, IUnknown* pItem, long nFileNameID, 
	BldFileRegHandle& frh, const wchar_t * pcDefExtension, BSTR bstrDir, BOOL bIsDefinitelyDir /* = FALSE */)
{
	CStringW strDir = bstrDir;
	CDirW dir;
	dir.CreateFromKnown(strDir);
	return GetItemOutputFile(pBldAction, pItem, nFileNameID, frh, pcDefExtension, &dir, bIsDefinitelyDir);
}

BOOL GetItemOutputFile(IVCBuildAction* pBldAction, IUnknown* pItem, long nFileNameID, 
	BldFileRegHandle& frh, const wchar_t * pcDefExtension, const CDirW* pBaseDir, BOOL bIsDefinitelyDir /* = FALSE */)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	if (spPropContainer == NULL)
	{
		VSASSERT(FALSE, "GetItemOutputFile requires a non-NULL property container to work correctly.");
		return FALSE;
	}
	VSASSERT(pBaseDir, "GetItemOutputFile requires a non-NULL base directory to work correctly.");

	CStringW strFileName;
	CPathW tp;
	frh = NULL;
	const wchar_t *pc;
	int len;
	BOOL bChangeEx = FALSE;

	CComVariant var;
	BOOL bHaveIt = spPropContainer->GetProp(nFileNameID, &var) == S_OK;
	if (bHaveIt && var.vt == VT_BSTR)
	{
		strFileName = var.bstrVal;
		bHaveIt = !strFileName.IsEmpty();
		if (bHaveIt)
		{
			CComBSTR bstrName = var.bstrVal;
			CComBSTR bstrOut;
			HRESULT hr = spPropContainer->Evaluate(bstrName, &bstrOut);
			VSASSERT(SUCCEEDED(hr), "Failed to evaluate string; is there something wrong with the way it is formed?");
			strFileName = bstrOut;
			bHaveIt = !strFileName.IsEmpty();
		}
	}
	else
		bHaveIt = FALSE;

	// Output filename? 
	if (bHaveIt)
	{
		// Okay, the item has an apparently non empty-name entry.  See if it's a directory by checking last character.  If it is,
		len = strFileName.GetLength();
		pc = (const wchar_t *) strFileName;
		pc = _wcsdec(pc, (wchar_t *)(pc+len));
		BOOL bIsDir = FALSE;
		if( pc )
			bIsDir = (*pc == L'/' || *pc == L'\\');

		if (!bIsDefinitelyDir)
			bIsDefinitelyDir = bIsDir;
	
		if ( bIsDefinitelyDir )
		{
			CComBSTR bstrFileName;
			pBldAction->get_ItemFilePath(&bstrFileName);
			CStringW strTmp = bstrFileName;
			if (!bIsDir)	// don't have the slash we need...
				strFileName += L'/';
			// just append and change extension later
			strFileName += strTmp;
			bChangeEx = TRUE;
		}
		if (tp.CreateFromDirAndFilename(*pBaseDir, strFileName))
		{
			if (bChangeEx)
				tp.ChangeExtension(pcDefExtension);
			if ((frh = CBldFileRegFile::GetFileHandle(tp, !tp.IsActualCase())) != NULL)
				return TRUE;
		}

		return FALSE;
	}

	// No filename or is empty filename
	CComBSTR bstrFileName;
	pBldAction->get_ItemFilePath(&bstrFileName);
	strFileName = bstrFileName;
	if (tp.CreateFromDirAndFilename(*pBaseDir, strFileName))
	{
		tp.ChangeExtension (pcDefExtension);
		if ((frh = CBldFileRegFile::GetFileHandle(tp, !tp.IsActualCase())) != NULL)
			return TRUE;
	}

	return FALSE;
}

