// EnCEncBldThrd.pp: implementation of the CEnCEncBldThrd class.
//
//		Supports background rebuild for Edit and Continue
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VCProjectEngine.h"
#include "buildengine.h"
#include <fcntl.h>
#include "msgboxes2.h"
#include "profile.h"

#include "encbldthrd.h"
#include "path2.h"
#include "EnvDaemon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncBldThrd::CEncBldThrd()
{
	m_hThread = NULL;
	m_dwThreadId = 0;
	m_lErrors = 0;
	m_lWarnings = 0;
	m_hResult = 0;

	m_pStreamENCBuildSpawnerCallback = NULL;
}

CEncBldThrd::~CEncBldThrd()
{
	if (m_hThread)
	{ 
		::CloseHandle(m_hThread);
	}
}


BOOL CEncBldThrd::Init(LPCOLESTR szCmdLine, LPCOLESTR szCurrentDir, IVCBuildErrorContext* pIVCBuildErrorContext,
	IVCBuildEngine* pBuildEngine, IENCBuildSpawnerCallback *pENCBuildSpawnerCallback, BOOL fUseConsoleCP)
{
	if (m_hThread || pIVCBuildErrorContext == NULL || pBuildEngine == NULL)
	{
		return FALSE; // can't have more than one thread going....
	}

	m_spBuildEngineImpl = pBuildEngine;
	if (m_spBuildEngineImpl == NULL)
		return FALSE;	// bad build engine

	m_szCurrentDir = szCurrentDir;
	m_fUseConsoleCP = fUseConsoleCP;

	CDirW dir;
	if (!dir.CreateFromString(szCurrentDir))
		return FALSE;

	m_szCmdLine = szCmdLine;

	m_lErrors = 0;
	m_lWarnings = 0;
	m_hResult = 0;

	VSASSERT(NULL == m_pStreamENCBuildSpawnerCallback, "ENCBuildSpawnerCallback already initialized before it should have been.");

	if (pENCBuildSpawnerCallback)
	{
		HRESULT hr = CoMarshalInterThreadInterfaceInStream(
			__uuidof (IENCBuildSpawnerCallback),
			(IUnknown *)pENCBuildSpawnerCallback,
			&m_pStreamENCBuildSpawnerCallback
			);

		VSASSERT(SUCCEEDED(hr), "Failed to marshal ENCBuildSpawnerCallback");
	}
	
	m_spBuildErrorContext = pIVCBuildErrorContext;
	m_spBuildEngineImpl->put_ErrorContext(pIVCBuildErrorContext);
	pIVCBuildErrorContext->put_AssociatedBuildEngine(pBuildEngine);		// make sure we're talking to the same build engine...

	CComBSTR bstrCurrentDir = szCurrentDir;
	pIVCBuildErrorContext->put_BaseDirectory(bstrCurrentDir);

	return TRUE;
}

void CEncBldThrd::Wait()
{
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
	}	
}


/* static */ DWORD WINAPI CEncBldThrd::EncBuildThread(LPVOID *pvBldThrd)
{
	CEncBldThrd *pBldThrd = (CEncBldThrd *)pvBldThrd;  // this is a static function; replace use of "this" with pBldThrd.

	// By default you had an error
	pBldThrd->m_hResult	 = E_FAIL;
	pBldThrd->m_lErrors = 1;

	CoInitialize(NULL); // make sure we can do ole in this thread
	VSASSERT(pBldThrd, "Trying to start ENCBldThrd before creating it");

	// Get the callback	pointer
	CComPtr<IENCBuildSpawnerCallback> pCallback;

	if (pBldThrd->m_pStreamENCBuildSpawnerCallback)	{
	
		HRESULT	hr = CoGetInterfaceAndReleaseStream(
			pBldThrd->m_pStreamENCBuildSpawnerCallback,
			__uuidof (IENCBuildSpawnerCallback),
			(void**)&pCallback
			);

		VSASSERT (SUCCEEDED	(hr), "Failed to unmarshal ENCBuildSpawnerCallback");
		
		pBldThrd->m_pStreamENCBuildSpawnerCallback = NULL;
	}

	// The following will be deleted by CSpawner::DoSpawn
	CComPtr<IVCCommandLine> pCmdLine;
	HRESULT hr = CVCCommandLine::CreateInstance(&pCmdLine);

	VSASSERT(SUCCEEDED(hr) && pCmdLine != NULL, "Failed to create the command line for the ENC build spawner");

	hr = pBldThrd->m_spBuildEngineImpl->InitializeSpawner(FALSE);
	VSASSERT(SUCCEEDED(hr), "Failed to initialize the error context for the ENC build spawner");

	if (SUCCEEDED(hr))
	{
		// Create CCmdLine object 
		CComPtr<IVCCommandLineList> plCmds;
		hr = CVCCommandLineList::CreateInstance(&plCmds);
		VSASSERT(SUCCEEDED(hr) && plCmds != NULL, "Failed to create the command line list for the ENC build spawner");

		pCmdLine->put_CommandLineContents(pBldThrd->m_szCmdLine);
		pCmdLine->put_Description(OLESTR(""));

		plCmds->Add(pCmdLine, FALSE);
		if ( pBldThrd->m_fUseConsoleCP )
			plCmds->put_UseConsoleCodePageForSpawner( VARIANT_TRUE );
		else
			plCmds->put_UseConsoleCodePageForSpawner( VARIANT_FALSE );

		VSASSERT(g_pProjectEngine != NULL, "Project engine not initialized.  Deep trouble.");

		CComPtr<IDispatch>		 pDispPlatformList;
		CComQIPtr<IVCCollection> pPlatformList;
		CComQIPtr<VCPlatform>	 pPlatform; 

		hr = g_pProjectEngine->get_Platforms(&pDispPlatformList);
		pPlatformList = pDispPlatformList;

		if (SUCCEEDED(hr) && pPlatformList)
		{
			CComPtr<IDispatch> pDispPlatform;
			pPlatformList->Item(CComVariant(L"Win32"), &pDispPlatform);
			pPlatform = pDispPlatform;
		}

		if (pPlatform != NULL)
		{
			CEnvironmentKeeper envKeeper(pPlatform);

			long cmd = CMD_Complete;
			pBldThrd->m_spBuildEngineImpl->SpawnBuild(plCmds, pBldThrd->m_szCurrentDir, FALSE, FALSE, 
				pBldThrd->m_spBuildErrorContext, TRUE, &cmd);
			CComQIPtr<IVCBuildEngine> spBuildEngine = pBldThrd->m_spBuildEngineImpl;
			if (pBldThrd->m_spBuildErrorContext)
			{
				pBldThrd->m_spBuildErrorContext->get_Errors(&(pBldThrd->m_lErrors));
				pBldThrd->m_spBuildErrorContext->get_Warnings(&(pBldThrd->m_lWarnings));
			}

			switch (cmd) {
			case CMD_Complete:
				pBldThrd->m_hResult = NOERROR;
				break;
			case CMD_Canceled:
				pBldThrd->m_hResult = E_ABORT;
				break;
			default:
				//VSASSERT(FALSE, "compiler/linker returned errorcode");
				pBldThrd->m_hResult = S_FALSE;
				// fixup case where an error occured but the spawner couldn't figure that out.
				// REVIEW: the whole counting of errors scheme should be reworked so we don't have to do this.
				// [vs98 24842]
				if (0 == pBldThrd->m_lErrors)
					pBldThrd->m_lErrors ++;
			}

			if (cmd == CMD_Canceled) {
				CComBSTR msg;
				msg.LoadString(IDS_USERCANCELED);
				pBldThrd->m_spBuildErrorContext->AddLine(msg);
			}

			pBldThrd->m_spBuildEngineImpl->TerminateSpawner();
		}
	}
	 
	if (pBldThrd->m_spBuildErrorContext) {

		// If we had errors or warnings, activate task list
		pBldThrd->m_spBuildErrorContext->ActivateTaskList(VARIANT_FALSE);
		pBldThrd->m_spBuildErrorContext->put_AssociatedBuildEngine(NULL);
	}

	if (pBldThrd->m_spBuildEngineImpl)
		pBldThrd->m_spBuildEngineImpl->put_ErrorContext(NULL);

	pBldThrd->m_spBuildErrorContext.Release();
	pBldThrd->m_spBuildEngineImpl.Release();

	HRESULT hrFromBuild = pBldThrd->m_hResult;
	DWORD	dwErrors = (DWORD)pBldThrd->m_lErrors;
	DWORD	dwWarnings = (DWORD)pBldThrd->m_lWarnings;

	pBldThrd->m_hThread = NULL; // allow a new thread to start up....

	VSASSERT( pCallback != NULL, "Can't call back to ecbuild!!!" );

	if ( pCallback ) {
		hr = pCallback->Done( hrFromBuild, dwErrors, dwWarnings);
		VSASSERT( hr == S_OK, "error calling ecbuild call back!!!" );
	}

	CoUninitialize();
	return (S_OK);
}



BOOL CEncBldThrd::FStart()
{
	if (m_hThread != NULL)
		return FALSE; // can't have more than one thread going....
	
	VARIANT_BOOL bSpawnerActive = VARIANT_TRUE;
	CComQIPtr<IVCBuildEngine> spBuildEngine = m_spBuildEngineImpl;
	if (spBuildEngine == NULL)
		return FALSE;	// bad input parameter

	spBuildEngine->get_SpawnerActive(&bSpawnerActive);
	if (bSpawnerActive == VARIANT_TRUE)
	{
		return FALSE; // can't have more than one thread going....
	}

	return CreateThread();
}

BOOL CEncBldThrd::CreateThread()
{
	m_hThread = ::CreateThread(NULL,		// default security attributes
								0,			// default stack size
								(LPTHREAD_START_ROUTINE) &EncBuildThread,
								(LPVOID)this,
								0,			// create and execute thread
								&m_dwThreadId);

	return (m_hThread != NULL);
}

