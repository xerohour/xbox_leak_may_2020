// BldThrd.cpp: implementation of the CVCBuildThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bldthread.h"
#include "FileRegistry.h"
#include "BldHelpers.h"
#include "BuildEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVCBuildThread::CVCBuildThread()
{
	m_hThread = NULL;
	m_hThreadExitEvent = NULL;
	m_fThreadDie = TRUE;
	m_dwThreadId = NULL;
	m_ppFiles = NULL;
	m_cFiles = 0;
}

CVCBuildThread::~CVCBuildThread()
{
	StopThread();
}

HRESULT CVCBuildThread::CreateInstance(IVCBuildThread** ppThread)
{
	CHECK_POINTER_NULL(ppThread);
	*ppThread = NULL;

	CComObject<CVCBuildThread> *pObj;
	HRESULT hr = CComObject<CVCBuildThread>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		CVCBuildThread *pVar = pObj;
		pVar->AddRef();
		*ppThread = pVar;
	}
	return hr;
}

STDMETHODIMP CVCBuildThread::StartThread()
{
	HRESULT hr = S_OK;

	VSASSERT(m_spBuildErrorContext != NULL, "NULL ErrorContext!!!");

	if (m_hThread)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BLD_ALREADY_INIT, IDS_ERR_BLD_THREAD_ALREADY_INIT);
	RETURN_INVALID_ON_NULL(m_spBuildErrorContext);

	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = m_spProjCfg;
	RETURN_INVALID_ON_NULL(spProjCfgImpl);

	// setup thread
	m_hThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_fThreadDie = FALSE;

	RETURN_ON_NULL(m_hThreadExitEvent);

	// need to fire the build start event *before* we start up the thread
	CComQIPtr<IDispatch> spDispCfg = m_spProjCfg;
	CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
	if (spProjEngineImpl)
		spProjEngineImpl->DoFireProjectBuildStarted(spDispCfg);

	// run thread
	m_hThread = CreateThread(NULL, 0, CVCBuildThread::BuildThread, this, 0, &m_dwThreadId);
	RETURN_ON_NULL(m_hThread);

	return hr;	
}

void CVCBuildThread::CleanupThread()
{
	// WARNING: MUST RELEASE COM POINTERS NOW (before ExitThread)!!!
	m_spProjCfg.Release();
	if (m_ppFiles != NULL)
	{
		for (ULONG idx = 0; idx < m_cFiles; idx++)
		{
			VCFile* pVCFile = m_ppFiles[idx];
			if (pVCFile != NULL)
				pVCFile->Release();
		}
		::CoTaskMemFree(m_ppFiles);
		m_ppFiles = NULL;
	}
	if (m_spBuildErrorContext != NULL)
	{
		m_spBuildErrorContext->Close();
		m_spBuildErrorContext.Release();
		m_spBuildErrorContext = NULL;
	}

	m_spBuildEngine.Release();
	m_spGenCfg.Release();
	m_marshalledEventHandler.Clear();
	m_spEventHandler.Release();
}

STDMETHODIMP CVCBuildThread::ExitThread()
{
	CoUninitialize();

	// exit thread
	if (m_hThread)
		::CloseHandle(m_hThread);
	m_hThread = NULL;
	SetEvent(m_hThreadExitEvent);	

	return S_OK;
}

void CVCBuildThread::StopThread()
{	
    if (m_hThread)
    {
		::ExitThread(0);
	    m_fThreadDie = TRUE;
	    WaitForSingleObject(m_hThreadExitEvent, INFINITE);
		::CloseHandle(m_hThread);
	    m_hThread = NULL;
    }

    if (m_hThreadExitEvent)
    {
	    CloseHandle(m_hThreadExitEvent);
	    m_hThreadExitEvent = NULL;
    }
}

STDMETHODIMP CVCBuildThread::InitializeForBuild(IVCGenericConfiguration* pGenCfg, VCConfiguration* pProjCfg, 
	ULONG celtFiles, VCFile* pFiles[], bldActionTypes bldType, BOOL bRecurse, BOOL bProvideBanner, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pErrorContext)
{
	if (m_hThread || pProjCfg == NULL || pBuildEngine == NULL || pGenCfg == NULL)
		RETURN_INVALID();	// bad input parameter...

	CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = pBuildEngine;
	RETURN_INVALID_ON_NULL(spBuildEngineImpl); // bad input parameter...
	spBuildEngineImpl->InitializeForBuild();

	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = pProjCfg;
	RETURN_INVALID_ON_NULL(spProjCfgImpl);		// bad config

	VARIANT_BOOL bCanStartBuild = VARIANT_FALSE;
	spProjCfgImpl->get_CanStartBuild(&bCanStartBuild);
	if (bCanStartBuild == VARIANT_FALSE)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BLD_IN_PROG, IDS_ERR_BLD_THREAD_ALREADY_INIT); // can't have more than one thread going....

	m_spGenCfg = pGenCfg;
	m_bldType = bldType;
	m_spProjCfg = pProjCfg;
	m_cFiles = celtFiles;

	if (pFiles != NULL)
	{
		m_ppFiles = (VCFile**) ::CoTaskMemAlloc(celtFiles * sizeof(VCFile *));
		if( m_ppFiles )
		{
			for (ULONG idx = 0; idx < celtFiles; idx++)
			{
				VCFile* pVCFile = pFiles[idx];
				if (pVCFile != NULL)
					pVCFile->AddRef();
				m_ppFiles[idx] = pVCFile;
			}
		}
	}

	m_bRecurse = bRecurse;
	m_bProvideBanner = bProvideBanner;
	m_spBuildErrorContext = pErrorContext;
	m_spBuildEngine = pBuildEngine;

	if (SUCCEEDED(CVCMarshallableEventHandler::CreateInstance(&m_spEventHandler)))
		m_marshalledEventHandler.Init(m_spEventHandler);

	return S_OK;
}

STDMETHODIMP CVCBuildThread::Wait()
{
	if (m_hThread)
		WaitForSingleObject(m_hThread, 0);

	return S_OK;	// return value not checked
}

class CThreadHelper
{
public:
	CThreadHelper(CVCBuildThread* pThread) : m_pBldThrd(pThread), m_fOK(FALSE), m_warnings(-1), m_errors(-1), m_bCancelled(FALSE) 
	{
		CoInitialize(NULL); // make sure we can do ole in this thread
	}
	~CThreadHelper()
	{
		if (m_pBldThrd->m_spBuildErrorContext)
			m_pBldThrd->m_spBuildErrorContext->put_AssociatedBuildEngine(NULL);
		if (m_spBuildEngineImpl)
			m_spBuildEngineImpl->put_ErrorContext(NULL);
		m_pBldThrd->m_spBuildEngine.Release();

		// send our own project build event
		if (m_spEventHandler)
			m_spEventHandler->FireProjectBuildFinished( m_pDispCfg, m_warnings, m_errors, (m_bCancelled ? VARIANT_TRUE : VARIANT_FALSE ) );
		else	// must not be registered, so fire for those who can handle cross-thread
		{
			CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
			if (spProjEngineImpl)
				spProjEngineImpl->DoFireProjectBuildFinished( m_pDispCfg, m_warnings, m_errors, (m_bCancelled ? VARIANT_TRUE : VARIANT_FALSE ) );
		}
		m_spEventHandler.Release();

		// tell the build thread to clean itself up
		CComPtr<IVCGenericConfiguration> spGenCfg = m_pBldThrd->m_spGenCfg;	// need this 'cause the thread's going to release its refcount
		m_pBldThrd->CleanupThread();

		// increase our priority so that we'll definitely get a chance to finish up before the primary thread
		// can possibly go away
		SetThreadPriority(m_pBldThrd->m_hThread, THREAD_PRIORITY_HIGHEST);

		// inform the shell the build has completed
		spGenCfg->InformBuildEnd(m_fOK);	// NOTE: this releases the thread

		// DO ABSOLUTELY **NOTHING** BEYOND THIS POINT.  We can get into threading issues if we do ANYTHING after
		// we tell the shell we're done with a build...

		// (The spGenCfg above had BETTER have exited our thread for us or we're toast, though...)
	}

public:
	CComQIPtr<IVCMarshallableEventHandler> m_spEventHandler;
	CComQIPtr<IDispatch> m_pDispCfg;
	CComQIPtr<IVCBuildEngineImpl> m_spBuildEngineImpl;
	CVCBuildThread* m_pBldThrd;
	BOOL m_fOK;
	long m_warnings;
	long m_errors;
	BOOL m_bCancelled;
};

/* static */ DWORD WINAPI CVCBuildThread::BuildThread(LPVOID pvBldThread)
{
	CVCBuildThread *pBldThrd = (CVCBuildThread *)pvBldThread;  // this is a static function; replace use of "this" with pBldThrd.
	VSASSERT(pBldThrd, "Starting the build thread without creating it first!");

	CThreadHelper threadHelper(pBldThrd);

	IVCGenericConfiguration *pGenCfg = pBldThrd->m_spGenCfg;	
	VSASSERT(pGenCfg != NULL, "Hey, how did we get this far with a NULL gencfg?!?");
	VSASSERT(pBldThrd->m_spProjCfg != NULL, "Must supply project config to build thread for things to work");

	// inform the shell the build has begun
	threadHelper.m_fOK = SUCCEEDED(pGenCfg->InformBuildBegin());
	if (!threadHelper.m_fOK)	// continue?
		return FALSE;

	// send our own project build event
	threadHelper.m_pDispCfg = pBldThrd->m_spProjCfg;
	pBldThrd->m_marshalledEventHandler.UnmarshalPtr(&threadHelper.m_spEventHandler);
	threadHelper.m_spBuildEngineImpl = pBldThrd->m_spBuildEngine;
	if (threadHelper.m_spBuildEngineImpl)
		threadHelper.m_spBuildEngineImpl->put_ErrorContext(pBldThrd->m_spBuildErrorContext);

	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = pBldThrd->m_spProjCfg;
	if (spProjCfgImpl == NULL)
	{
		VSASSERT(FALSE, "Project config should support IVCConfigurationImpl...");
		return FALSE;
	}

	HRESULT hr = S_OK;
	if (pBldThrd->m_bldType == TOB_Compile)
	{
		hr = spProjCfgImpl->TopLevelBuild(TOB_Compile, pBldThrd->m_cFiles, pBldThrd->m_ppFiles, 
			pBldThrd->m_spBuildEngine, pBldThrd->m_spBuildErrorContext, pBldThrd->m_bRecurse, FALSE /* no idle*/, 
			TRUE /* do cleanup */, pBldThrd->m_bProvideBanner, &threadHelper.m_warnings, &threadHelper.m_errors, 
			&threadHelper.m_bCancelled);
	}
	else if (pBldThrd->m_bldType == TOB_ReBuild)
	{
		// don't do a clean-before-build for makefile config types!
		ConfigurationTypes cfgType;
		pBldThrd->m_spProjCfg->get_ConfigurationType( &cfgType );
		if( cfgType != typeUnknown )
		{
			hr = spProjCfgImpl->TopLevelBuild(TOB_Clean, pBldThrd->m_cFiles, pBldThrd->m_ppFiles, 
				pBldThrd->m_spBuildEngine, pBldThrd->m_spBuildErrorContext, TRUE /* recurse */, TRUE /* do idle */, 
				TRUE /* no cleanup */, pBldThrd->m_bProvideBanner, &threadHelper.m_warnings, &threadHelper.m_errors,
				&threadHelper.m_bCancelled);
		}
	}

	if (pBldThrd->m_bldType != TOB_Compile && !threadHelper.m_bCancelled)
		hr = spProjCfgImpl->TopLevelBuild(pBldThrd->m_bldType, pBldThrd->m_cFiles, pBldThrd->m_ppFiles, 
			pBldThrd->m_spBuildEngine, pBldThrd->m_spBuildErrorContext, pBldThrd->m_bRecurse, FALSE /* no idle */, 
			TRUE /* do cleanup */, pBldThrd->m_bProvideBanner, &threadHelper.m_warnings, &threadHelper.m_errors, 
			&threadHelper.m_bCancelled);

	if (pBldThrd->m_spBuildErrorContext)
		pBldThrd->m_spBuildErrorContext->Close();
	threadHelper.m_fOK = (hr == S_OK) ? TRUE: FALSE;

	return threadHelper.m_fOK;
}

HRESULT CVCMarshallableEventHandler::CreateInstance(IVCMarshallableEventHandler** ppEventHandler)
{
	CHECK_POINTER_NULL(ppEventHandler);
	*ppEventHandler = NULL;

	CComObject<CVCMarshallableEventHandler> *pObj;
	HRESULT hr = CComObject<CVCMarshallableEventHandler>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		CVCMarshallableEventHandler *pVar = pObj;
		pVar->AddRef();
		*ppEventHandler = pVar;
	}
	return hr;
}

STDMETHODIMP CVCMarshallableEventHandler::FireProjectBuildFinished(IDispatch *pCfg, long errors, long warnings, 
	VARIANT_BOOL bCancelled)
{
	CComQIPtr<IVCProjectEngineImpl> spProjEngImpl = g_pProjectEngine;
	RETURN_ON_NULL2(spProjEngImpl, E_UNEXPECTED);
	return spProjEngImpl->DoFireProjectBuildFinished(pCfg, errors, warnings, bCancelled);
}
