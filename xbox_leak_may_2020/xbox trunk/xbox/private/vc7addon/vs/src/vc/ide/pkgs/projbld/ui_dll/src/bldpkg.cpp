// BldPkg.cpp : Implementation of CBuildPackage
#include "stdafx.h"
#include "path2.h"
#include "projbuildevents.h"
#include "BldPkg.h"
#include "vcfile.h"
#include "vcfgrp.h"
#include "errcontext2.h"

#include "vcarchy.h"
#include "exehierarchy.h"
#include "..\resdll\gpcmd.h"
#include <YVALS.H>
#include "localeinfo.h"
#include "output.h"
#include "bldguid.h"
#include <vcextgd.h>
#include "vcprojcnvt2.h"

#include "OptionsDlg.h"
#include "DirectoriesDlg.h"
#include "ConvertDlg.h"
#include "exefordbg.h"

#include <atlsafe.h>

#include "outgroup.h"
#include "vsappid.h"

#include "utils.h"
#include "profile.h"
#include "AutoProject.h"

#include "bldtst.h"
#include "VsCoCreate.h"

#ifdef AARDVARK
#include "StyleHierarchy.h"


// {A5B688D0-F9AA-4462-94FF-6BD2D72C52BA}
DEFINE_GUID(IID_IStyleSheetHierarchy, 
0xa5b688d0, 0xf9aa, 0x4462, 0x94, 0xff, 0x6b, 0xd2, 0xd7, 0x2c, 0x52, 0xba);
#endif	// AARDVARK

#define USE_GLOBAL_RC_EDITOR	L"UseGlobalRCEditor"

//CWnd g_hMainWnd; // for dialogs' parent
CBuildPackage *g_pBuildPackage = NULL;
BOOL CConvertDlg::s_bCancel = FALSE;
BOOL CConvertDlg::s_bConvertAll = FALSE;
BOOL CConvertDlg::s_bConvertNone = FALSE;
VSOWNEDPROJECTOBJECT CVCProjectFactory::s_EFPolicyCookie;

BOOL CCCallExeForDebug::s_bTestContainerInit = FALSE;
CStringW CCCallExeForDebug::s_strTestContainer = L"";

CLocaleInfo g_LocaleInfo;
HINSTANCE g_hInstLocale; // hinstance of localized resources
CComBSTR CBuildPackage::s_bstrAltKey = L"Software\\Microsoft\\VisualStudio\\7.0";

// global "in project load" flag. DO NOT EDIT DIRECTLY!!!
bool g_bInProjectLoad = false;

// global "root level project" string for VC 4.x conversion ONLY
wchar_t* g_wszRootProjectName = NULL;

CComBSTR bstrExeArguments;

extern "C" void	ENDBOOT_projbld(void){ return; }

// helper class for "in project load" tracking
class CInProjectLoadHelper
{
public:
	CInProjectLoadHelper(){ g_bInProjectLoad = true; }
	~CInProjectLoadHelper(){ g_bInProjectLoad = false; }
};

// Global execution context
CVCExecutionCtx CBuildPackage::s_executionCtx;

CBuildPackage* GetBuildPkg(void)
{
	VSASSERT(/*CBuildPackage::*/g_pBuildPackage != NULL, "Build package not initialized!");
	return /*CBuildPackage::*/g_pBuildPackage;
}

IServiceProvider* GetServiceProvider(void)
{
	VSASSERT(/*CBuildPackage::*/g_pBuildPackage != NULL, "Build package not initialized!");
	VSASSERT(/*CBuildPackage::*/g_pBuildPackage->m_pServiceProvider != NULL, "Shell service provider not initialized!");
	return /*CBuildPackage::*/g_pBuildPackage->m_pServiceProvider;
}

HRESULT ExternalQueryService(REFGUID rsid, REFIID iid, void **ppvObj)
{
	VSASSERT(/*CBuildPackage::*/g_pBuildPackage != NULL, "Build package not initialized!");
	VSASSERT(/*CBuildPackage::*/g_pBuildPackage->m_pServiceProvider != NULL, "Shell service provider not initialized!");
	return /*CBuildPackage::*/g_pBuildPackage->m_pServiceProvider->QueryService(rsid, iid, ppvObj);
}

HRESULT DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bSetErrorInfo /* = TRUE */)
{
	VSASSERT(/*CBuildPackage::*/g_pBuildPackage != NULL, "Build package not initialized!");
	return g_pBuildPackage->DoSetErrorInfo(hrErr, idErr, bSetErrorInfo);
}

HRESULT DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bSetErrorInfo /* = TRUE */)
{
	VSASSERT(/*CBuildPackage::*/g_pBuildPackage != NULL, "Build package not initialized!");
	return g_pBuildPackage->DoSetErrorInfo2(hrErr, idErr, szParam, bSetErrorInfo);
}

void HandleSetErrorInfo(HRESULT hrReport, BSTR bstrMsg)
{
	CComPtr<IVsUIShell> spVsUIShell;
	HRESULT hr = g_pBuildPackage->GetVsUIShell(&spVsUIShell, TRUE /* in main thread */);
	if( hr == S_OK )
	{
		if(spVsUIShell)
			spVsUIShell->SetErrorInfo(hrReport, bstrMsg, 0, NULL, NULL);
	}
}

void ReportError(BSTR bstrMsg, HRESULT hrReport /* = E_FAIL */)
{
	CComPtr<IVsUIShell> spVsUIShell;
	HRESULT hr = g_pBuildPackage->GetVsUIShell(&spVsUIShell, TRUE /* in main thread */);
	if( hr == S_OK )
	{
		if(spVsUIShell)
		{
			spVsUIShell->SetErrorInfo(hrReport, bstrMsg, 0, NULL, NULL);
			spVsUIShell->ReportErrorInfo(hrReport);
		}
	}
}

//t-samhar: use this function to determine whether or not
//to perform any UI-related functions.
BOOL FIsIDEInteractive()
{
	static BOOL fAlreadyQueried = FALSE;
	static BOOL fIsInteractive = TRUE;

	if (fAlreadyQueried==FALSE)
	{
		CComPtr<IVsShell> pVsShell;
		HRESULT hr = GetBuildPkg()->GetVsShell(&pVsShell);
		RETURN_ON_FAIL2(hr, FALSE);

		CComVariant var;
		pVsShell->GetProperty(VSSPROPID_IsInteractive, &var);  
		VSASSERT(SUCCEEDED(hr) && var.vt == VT_BOOL, "Failed to get IsInteractive property");

		fAlreadyQueried = TRUE;
		if (V_BOOL(&var))
			fIsInteractive = TRUE;
		else
			fIsInteractive = FALSE;
	}

	return fIsInteractive;
}

HRESULT CBuildPackage::GetVsShell(IVsShell** ppShell)
{
	CHECK_POINTER_NULL(ppShell);

	HRESULT hr = ExternalQueryService(SID_SVsShell, __uuidof(IVsShell), (void**)ppShell);
	VSASSERT(SUCCEEDED(hr), "Failed to get IVsShell interface");
	RETURN_ON_FAIL_OR_NULL(hr, *ppShell);

	return S_OK;
}

HRESULT CBuildPackage::GetDTE(DTE** lppaReturn)
{
	CHECK_POINTER_VALID(lppaReturn);	// GetDTE is used all over everywhere for automation, so heavy-duty check required
	return ExternalQueryService(SID_SDTE, IID__DTE, (void **)lppaReturn);
}

/////////////////////////////////////////////////////////////////////////////
// CBuildPackage

CBuildPackage::CBuildPackage()
{
   	VSASSERT(g_pBuildPackage == NULL, "Build package previously initialized!");
    g_pBuildPackage = this;

	m_dwBuildProjectEngineCookie = ULONG_MAX;
	m_dwENCBuildSpawnerCookie = ULONG_MAX;
	m_dwBuildErrorContextCookie = ULONG_MAX;
	m_dwTaskCookie = 0;
	m_nSolutionRefCount = 0;
	m_cSolutionBuilds = 0;

	m_pVCNotificationSource = NULL; 
	m_hImageList = NULL;
	m_dwProjectEngineEventsCookie = NULL;
	m_vsSolutionEventsCookie = NULL;

	m_guidPersistenceSlot = GUID_NULL;
	m_fAddNewItem = 0;
	m_pBuildOptions = NULL;
	m_pBuildDirectories = NULL;
	m_bUseGlobalEditorForRC = VARIANT_FALSE;
}

CBuildPackage::~CBuildPackage()
{
	VSASSERT(m_pServiceProvider == NULL, "IBuildPackage::Close() not called");

   	VSASSERT(g_pBuildPackage == this, "Can only have one build package!");

	if (m_spAutoProjects)
		m_spAutoProjects.Release();

    g_pBuildPackage = NULL;

  if (m_pBuildDirectories)
	  delete m_pBuildDirectories;

  if (m_pBuildOptions)
	  delete m_pBuildOptions;
}

HRESULT CBuildPackage::CreateInstance(CBuildPackage **ppBuildPackage)
{
	CComObject<CBuildPackage> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CBuildPackage>::CreateInstance(&pPrjObj);
	RETURN_ON_FAIL_OR_NULL2(hr, pPrjObj, E_OUTOFMEMORY);
	pPrjObj->AddRef();
	pPrjObj->Initialize();
	*ppBuildPackage = pPrjObj;
	return hr;
}

STDMETHODIMP CBuildPackage::SetSite(IServiceProvider *pSP)
{
/* SetSite:
	Store pSP
*/
	RETURN_ON_NULL2(pSP, S_OK);

	VSASSERT(m_pServiceProvider == NULL, "Cannot call SetSite twice in a row with non-NULL service provider");

	m_pServiceProvider = pSP;

	CComPtr<IVsShell> srpShell;
	HRESULT hr = pSP->QueryService(SID_SVsShell, __uuidof(IVsShell), (void**)&srpShell);
	VSASSERT(SUCCEEDED(hr), "QueryService for IVsShell failed!  (Is this still a valid service?  Are we in a secondary thread?)");
	if(srpShell)
	{
		CComVariant var;
		hr = srpShell->GetProperty(VSSPROPID_VirtualRegistryRoot, &var);
		VSASSERT(SUCCEEDED(hr) && var.vt == VT_BSTR, "Failed to get VirtualRegistryRoot property!");
		if(SUCCEEDED(hr) && var.vt == VT_BSTR)
		{
			CBuildPackage::s_bstrAltKey = var.bstrVal;
			CStringA strAltKey = var.bstrVal;		// YES, ansi
			SetAlternateKey(strAltKey);
			VsLoaderSetRegistryRootW(var.bstrVal);
		}
	}
	
	CStringW strRoot = s_bstrAltKey + "\\VC";
	DWORD dwValue;
	if (FAILED(GetRegIntW(strRoot, USE_GLOBAL_RC_EDITOR, &dwValue)))
		dwValue = VARIANT_FALSE;
	m_bUseGlobalEditorForRC = dwValue ? VARIANT_TRUE : VARIANT_FALSE;
	
	// enable the satellite dll. 
	g_LocaleInfo.SetMainInst(_Module.GetModuleInstance()); 
	g_hInstLocale = g_LocaleInfo.GetUIHostLib();
	{
		// This bails us out of any serious trouble....
		CStringW strTest;

		if (!strTest.LoadString(g_hInstLocale, IDS_SATELLITETEST))
		{
			CComBSTR bstrMessage = L"Can't find localized resources";
			ReportError( bstrMessage );
			return S_FALSE;
		}
	}
	_Module.SetResourceInstance(g_hInstLocale);

	// now create the Engine
	{
		// REVIEW: redo this section of code when the engine becomes a registered package
		hr = Initialize();
		RETURN_ON_FAIL(hr);

		CComPtr<IVCBuildPackageInternal> pPkgInternal;
		QueryInterface(__uuidof(IVCBuildPackageInternal), (void**)&pPkgInternal);

		CComQIPtr<IVCProjectEngineImpl> pPrjUnk;
		pPrjUnk = m_pProjectEngine;
		if(pPrjUnk == NULL)
			return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
		pPrjUnk->Initialize((LONG_PTR)g_hInstLocale, pSP, NULL, pPkgInternal);
	}


	CComPtr<IVsRegisterProjectTypes> srpRegProj;
	hr = ExternalQueryService(SID_SVsRegisterProjectTypes, __uuidof(IVsRegisterProjectTypes), (void **)&srpRegProj);
	if (SUCCEEDED(hr) && srpRegProj)
	{	// register project types

		CComObject<CVCProjectFactory> *pProjectFactory = new CComObject<CVCProjectFactory>;
		RETURN_ON_NULL2(pProjectFactory, E_OUTOFMEMORY);
		pProjectFactory->AddRef();
		hr = srpRegProj->RegisterProjectType(IID_IVCArchy, pProjectFactory, &m_pVCProjectCookie);
		pProjectFactory->Release();
		RETURN_ON_FAIL(hr);

		CComObject<CExeProjectFactory> *pExeProjectFactory = new CComObject<CExeProjectFactory>;
		RETURN_ON_NULL2(pExeProjectFactory, E_OUTOFMEMORY);
		pExeProjectFactory->AddRef();
		hr = srpRegProj->RegisterProjectType(IID_IExeHierarchy, pExeProjectFactory, &m_pExeProjectCookie);
		pExeProjectFactory->Release();
		RETURN_ON_FAIL(hr);

		CComObject<CCrashDumpProjectFactory> *pCrashDumpProjectFactory = new CComObject<CCrashDumpProjectFactory>;
		RETURN_ON_NULL2(pCrashDumpProjectFactory, E_OUTOFMEMORY);
		pCrashDumpProjectFactory->AddRef();
		hr = srpRegProj->RegisterProjectType(IID_ICrashDumpHierarchy, pCrashDumpProjectFactory, &m_pCrashDumpProjectCookie);
		pCrashDumpProjectFactory->Release();
		RETURN_ON_FAIL(hr);

/*
		CComObject<CVUPProjectFactory> *pVUPProjectFactory = new CComObject<CVUPProjectFactory>;
		RETURN_ON_NULL2(pVUPProjectFactory, E_OUTOFMEMORY);
		pVUPProjectFactory->AddRef();
		hr = srpRegProj->RegisterProjectType(IID_IVUPHierarchy, pVUPProjectFactory, &m_pVUPProjectCookie);
		pVUPProjectFactory->Release();
		RETURN_ON_FAIL(hr);
*/

#ifdef AARDVARK
		CComObject<CStyleSheetProjectFactory> *pStyleSheetProjectFactory = new CComObject<CStyleSheetProjectFactory>;
		RETURN_ON_NULL2(pStyleSheetProjectFactory, E_OUTOFMEMORY);
		pStyleSheetProjectFactory->AddRef();
		hr = srpRegProj->RegisterProjectType(IID_IStyleSheetHierarchy, pStyleSheetProjectFactory, &m_pStyleSheetProjectCookie);
		pStyleSheetProjectFactory->Release();
		RETURN_ON_FAIL(hr);
#endif	// AARDVARK
	}

	CComPtr<IProfferService> pProffer;
	if ((SUCCEEDED(ExternalQueryService(SID_SProfferService, __uuidof(IProfferService), (void **)&pProffer))) && (pProffer != NULL))
	{
		CComPtr<IServiceProvider> pBuildServiceProvider;
		QueryInterface(__uuidof(IServiceProvider),(void**)&pBuildServiceProvider);
        pProffer->ProfferService(SID_SProjectEngine, pBuildServiceProvider, &m_dwBuildProjectEngineCookie);
        pProffer->ProfferService(SID_SENCBuildSpawner, pBuildServiceProvider, &m_dwENCBuildSpawnerCookie);
        pProffer->ProfferService(SID_SBuildErrorContext, pBuildServiceProvider, &m_dwBuildErrorContextCookie);
	}

	// Add init code
    if(!GetBuildPkg()->OnInit())
        return S_FALSE;

    // Call GetTaskList here : needed for marshalling the tasklist for builds
    CComPtr<IVsTaskList> spTaskList;
    hr = GetTaskList(&spTaskList, TRUE );

	// Now that we are loaded, check if we were loaded as part of a /debug switch
	CComPtr<IVsAppCommandLine> pCmdLine;
	if ((SUCCEEDED(ExternalQueryService(SID_SVsAppCommandLine, __uuidof(IVsAppCommandLine), (void **)&pCmdLine))) && (pCmdLine != NULL))
	{
		BOOL bDebug = FALSE;

		pCmdLine->GetOption(L"DebugExe",&bDebug,&bstrExeArguments);
		if( bDebug )
		{
			// It was there, so take all remaining switches and make them
			// the switches to the exe to debugs, debug config
			if (bstrExeArguments.Length() > 0)	// need to figure out if there is something here we have to open
			{
				CStringW strExeArguments = bstrExeArguments;
				int nNextIdx = 0;
				strExeArguments.TrimLeft();
				strExeArguments.TrimRight();
				int nMaxIdx = strExeArguments.GetLength();
				CStringW strSeparatorList = L" ";
				CStringW strPossibleExe;
				nNextIdx = GetNextItem(strExeArguments, nNextIdx, nMaxIdx, strSeparatorList, strPossibleExe);
				if (nNextIdx > 0)
				{
					strExeArguments = strExeArguments.Right(nMaxIdx-nNextIdx);
					strExeArguments.TrimLeft();
					strPossibleExe.TrimRight();
				}
				else
					strExeArguments.Empty();
				if (!strPossibleExe.IsEmpty())	// need to strip off any quotes, unfortunately, since SearchPathW doesn't like them
				{
					nMaxIdx = strPossibleExe.GetLength();
					if (nMaxIdx > 0 && strPossibleExe[0] == L'"')
					{
						strPossibleExe = strPossibleExe.Right(nMaxIdx-1);
						nMaxIdx--;
					}
					if (nMaxIdx > 0 && strPossibleExe[nMaxIdx-1] == L'"')
						strPossibleExe = strPossibleExe.Left(nMaxIdx-1);
				}

				bstrExeArguments = strExeArguments;
				CStringW strExtExpected;
				DWORD dwLen = SearchPathW(NULL, strPossibleExe, NULL, 0, NULL, NULL);
				if (!dwLen)
				{
					strExtExpected = L".exe";
					dwLen = SearchPathW(NULL, strPossibleExe, strExtExpected, 0, NULL, NULL);
				}
				if (!dwLen)
				{
					strExtExpected = L".com";
					dwLen = SearchPathW(NULL, strPossibleExe, strExtExpected, 0, NULL, NULL);
				}
				if (dwLen)	// found something
				{
					CStringW strExe;
					LPWSTR szFilePath = strExe.GetBuffer(dwLen+2);
					LPWSTR szFilePart = szFilePath;
					if (strExtExpected.IsEmpty())
						dwLen = SearchPathW(NULL, strPossibleExe, NULL, dwLen+1, szFilePath, &szFilePart);
					else
						dwLen = SearchPathW(NULL, strPossibleExe, strExtExpected, dwLen+1, szFilePath, &szFilePart);
					strExe.ReleaseBuffer();
					strPossibleExe = strExe;
				}

				CComPtr<IVsSolution> pSln;
				hr = GetIVsSolution( &pSln );
				CComPtr<IUnknown> spExeProj;
				if (SUCCEEDED(hr) && pSln)
				{
				    	g_bCommandLineDBG = TRUE;
					hr = pSln->CreateProject( IID_IExeHierarchy, strPossibleExe, NULL, NULL,
						CPF_OPENFILE | CPF_SILENT, IID_IUnknown, (void **)&spExeProj);
				}
			}
		}
	}
	
	// need this so as to initialize the marshalled pointer to the shell
	CComPtr<IVsUIShell> spShell;
	hr = GetVsUIShell(&spShell, TRUE /* in main thread */);

	// Add TSHELL CMDs
	InitTShell();

	UpdateSolutionFile();	// don't care whether we succeed or fail on this...

	CComPtr<IVsSolutionBuildManager> spSolnBldMgr;
	hr = GetSolutionBuildManager(&spSolnBldMgr, TRUE);
	if (SUCCEEDED(hr) && spSolnBldMgr)
		spSolnBldMgr->AdviseUpdateSolutionEvents((IVsUpdateSolutionEvents *)this, &m_vsSolutionEventsCookie);

	CComPtr<IVsSolution> pSln;
	hr = GetIVsSolution( &pSln );
	RETURN_ON_FAIL_OR_NULL2(hr, pSln, E_UNEXPECTED);
	return pSln->AdviseSolutionEvents( (IVsSolutionEvents*)this, &m_dwSlnEventsCookie );
}

int CBuildPackage::FindFirstSeparator(CStringW& strList, CStringW& strSeparatorList, int nStartIdx)
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

int CBuildPackage::GetNextItem(CStringW& strList, int nStartIdx, int nMaxIdx, CStringW& strSeparator, CStringW& strItem)
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

int CBuildPackage::GetNextQuotedItem(CStringW& strList, int nStartIdx, int nMaxIdx, int nSep, int nLastQuote,
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


STDMETHODIMP CBuildPackage::QueryClose(BOOL *pCanClose)
{
	CHECK_POINTER_NULL(pCanClose);

	*pCanClose = TRUE;
	return S_OK;
}

STDMETHODIMP CBuildPackage::Close()
{
	VSASSERT(m_pServiceProvider != NULL, "Cannot call Close before SetSite!");

	if (m_spAutoProjects)
		m_spAutoProjects.Release();

	// Add exit code
	GetBuildPkg()->OnExit();
	RETURN_ON_NULL2(m_pServiceProvider, E_UNEXPECTED);


	{	// scope spProffer
		CComPtr<IProfferService> spProffer;
		if ((SUCCEEDED(ExternalQueryService(SID_SProfferService, __uuidof(IProfferService), (void **)&spProffer))) && (spProffer != NULL))
		{
			HRESULT hr;
			if (m_dwBuildProjectEngineCookie != ULONG_MAX)
			{
				hr = spProffer->RevokeService(m_dwBuildProjectEngineCookie);
				VSASSERT(SUCCEEDED(hr), "Failed to revoke project engine cookie!");
			}
			m_dwBuildProjectEngineCookie = ULONG_MAX;
			if (m_dwENCBuildSpawnerCookie != ULONG_MAX)
			{
				hr = spProffer->RevokeService(m_dwENCBuildSpawnerCookie);
				VSASSERT(SUCCEEDED(hr), "Failed to revoke ENC build spawner cookie!");
			}
			m_dwENCBuildSpawnerCookie = ULONG_MAX;
			if (m_dwBuildErrorContextCookie != ULONG_MAX)
			{
				hr = spProffer->RevokeService(m_dwBuildErrorContextCookie);
				VSASSERT(SUCCEEDED(hr), "Failed to revoke error context cookie!");
			}
			m_dwBuildErrorContextCookie = ULONG_MAX;
		}
	}

	// unadvise from SolutionEvents before we let the service provider go
	if( m_dwSlnEventsCookie )
	{
		CComPtr<IVsSolution> pSln;
		HRESULT hr = GetIVsSolution( &pSln );
		if( SUCCEEDED( hr ) && pSln )
			pSln->UnadviseSolutionEvents( m_dwSlnEventsCookie );
	}

    m_pServiceProvider.Release();
	m_pVCNotificationSource.Release();

	if (m_pTaskList)
	{
		if (m_dwTaskCookie != 0)
		{
			HRESULT hr = m_pTaskList->UnregisterTaskProvider(m_dwTaskCookie);
			VSASSERT(SUCCEEDED(hr), "Failed to unregister task provider");
		}
		m_marshalTaskList.Clear();
		m_pTaskList.Release();
	}

	// unadvise from UpdateSolutionEvents
	if (m_vsSolutionEventsCookie)
	{
		CComPtr<IVsSolutionBuildManager> spSolnBldMgr;
		HRESULT hr = GetSolutionBuildManager(&spSolnBldMgr, TRUE);
		if (SUCCEEDED(hr) && spSolnBldMgr)
		{
			spSolnBldMgr->UnadviseUpdateSolutionEvents(m_vsSolutionEventsCookie);
			m_vsSolutionEventsCookie = NULL;
		}
	}
	
	if (m_dwProjectEngineEventsCookie && m_pProjectEngine)
	{
		CComQIPtr<IConnectionPointContainer> pCPContainer;
		CComPtr<IConnectionPoint> pCP;
		pCPContainer = m_pProjectEngine;
		VSASSERT(pCPContainer, "Project engine isn't a connection point container!");
		if (pCPContainer)
		{
			HRESULT hr;
			hr = pCPContainer->FindConnectionPoint(__uuidof(IVCProjectEngineEvents), &pCP);
			VSASSERT(SUCCEEDED(hr) && pCP, "Failed to find connection point for IVCProjectEngineEvents!");
			if (pCP)
			{
				pCP->Unadvise(m_dwProjectEngineEventsCookie);
				m_dwProjectEngineEventsCookie = 0;
			}
		}
		CComQIPtr<IVCProjectEngineImpl> pPrjUnk;
		pPrjUnk = m_pProjectEngine;
		pPrjUnk->Close();
		m_pProjectEngine.Release();
	}
	
	m_marshalSolutionBuildManager.Clear();
	m_marshalVsUIShell.Clear();
	m_spVsUIShell.Release();
	m_marshalVsSolution.Clear();
	m_spSolutionBuildManager.Release();
	m_spVsSccManager2.Release();

	return S_OK;
}

//---------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

void CBuildPackage::DoRefreshTaskList()
{
    if (this && m_pTaskList)
        (void)m_pTaskList->RefreshTasks(m_dwTaskCookie);
}


void CBuildPackage::DoUpdateTaskList()
{
    if (this && m_pTaskList)
        (void)m_pTaskList->UpdateProviderInfo(m_dwTaskCookie);
}

//-----------------------------------------------------------------------------
//	Called to update the status for each command
//-----------------------------------------------------------------------------
STDMETHODIMP CBuildPackage::QueryStatus(
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[],
		OLECMDTEXT *pCmdText)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);

	// use this as an example. 	The open command will move from here.
	if (*pguidCmdGroup == guidVSStd97)
	{
		for (ULONG i=0; i <cCmds; i++)
		{
			BOOL bEnable = FALSE;
			switch(prgCmds[i].cmdID)
			{
				case cmdidOpen:
					bEnable = TRUE; //GetProjectNode()->IsSourceControlled() && !IsInSourceControl();
					break;
				default:
					bEnable = -2;
					break;
			}

			if(bEnable == -2)
				return OLECMDERR_E_NOTSUPPORTED;
			prgCmds[i].cmdf = OLECMDF_SUPPORTED|OLECMDF_ENABLED;
		}

		return NOERROR;
	}
	else if (*pguidCmdGroup == guidVSStd2K)
	{
		return OLECMDERR_E_NOTSUPPORTED;
	}

	return OLECMDERR_E_UNKNOWNGROUP;
}

//-----------------------------------------------------------------------------
// Called to execute our commands
//-----------------------------------------------------------------------------
STDMETHODIMP CBuildPackage::Exec(
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	return OLECMDERR_E_NOTSUPPORTED;
}



//-----------------------------------------------------------------------------
STDMETHODIMP CBuildPackage::GetAutomationObject(
			/* [in] */ LPCOLESTR pszPropName,
            /* [out] */ IDispatch **ppDisp)
{
	CHECK_POINTER_VALID(ppDisp);
	*ppDisp = NULL;

	if (_wcsicmp(pszPropName, L"VCProjects") == 0)
	{
		IDispatch* pAutoProjects = GetAutoProjects();	// not ref-counted
		RETURN_ON_NULL2(pAutoProjects, E_OUTOFMEMORY);
		pAutoProjects->AddRef();
		*ppDisp = pAutoProjects;
		return S_OK;
	}
	else if (_wcsicmp(pszPropName, L"VCBuild") == 0)
	{
		if (m_pProjectEngine)
		{
		    CVCBuildOptionsObject::CreateInstance( ppDisp );
		    return S_OK;
		}
		return E_FAIL;	// something wrong somewhere
	}
	else if (_wcsicmp(pszPropName, L"VCDirectories") == 0)
	{
		if (m_pProjectEngine)
			return m_pProjectEngine->QueryInterface(IID_IDispatch, (void**)ppDisp);
		return E_FAIL;	// something wrong somewhere
	}
	else if (_wcsicmp(pszPropName, L"VCProjectEngineEventsObject") == 0)
	{
		if (m_pProjectEngine)
			return m_pProjectEngine->get_Events(ppDisp);
		return E_FAIL;	// something wrong somewhere
	}

	return E_FAIL;	// unrecognized
}

STDMETHODIMP CBuildPackage::CreateTool(REFGUID rguidPersistenceSlot)
{
	m_guidPersistenceSlot = rguidPersistenceSlot;
	return (S_OK);
}

STDMETHODIMP CBuildPackage::ResetDefaults(PKGRESETFLAGS dwFlags)
{
	return(S_OK);
}

STDMETHODIMP CBuildPackage::GetPropertyPage(REFGUID rguidPage, VSPROPSHEETPAGE * ppage)
{
	CHECK_POINTER_NULL(ppage);

	int i = 0;
	HRESULT hr = E_FAIL;

	// Locate page in our list

	if (IsEqualGUID(GUID_ProjGeneralOptions, rguidPage))
	{
		ZeroMemory(ppage, sizeof(VSPROPSHEETPAGE));
		if (m_pBuildOptions)
			delete m_pBuildOptions;
		m_pBuildOptions = NULL;
		if (m_pProjectEngine)
		{
			CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = m_pProjectEngine;
			if (spProjEngineImpl)
			{
				{
					m_pBuildOptions = new CBuildOptions();
					RETURN_ON_NULL2(m_pBuildOptions, E_OUTOFMEMORY);
					_Module.AddCreateWndData(&(m_pBuildOptions->m_thunk.cd), m_pBuildOptions);

					// Set page specific info
					ppage->wTemplateId = IDDP_OPTIONS_BUILD;
					ppage->pfnDlgProc  = (DLGPROC)&CBuildOptions::StartDialogProc;
				    ppage->dwFlags = PSP_DEFAULT | PSP_USECALLBACK | PSP_HASHELP;
					hr = S_OK;
				}
			}
		}
	}
	else if (IsEqualGUID(GUID_ProjDirectories, rguidPage))
	{
		  ZeroMemory(ppage, sizeof(VSPROPSHEETPAGE));
		  if (m_pBuildDirectories)
			  delete m_pBuildDirectories;
		  m_pBuildDirectories = new CDirectoriesDlg;
		  RETURN_ON_NULL2(m_pBuildDirectories, E_OUTOFMEMORY);
		  _Module.AddCreateWndData(&(m_pBuildDirectories->m_thunk.cd), m_pBuildDirectories);
		  // Set page specific info
		  ppage->wTemplateId = IDDP_OPTIONS_DIRECTORIES;
		  ppage->pfnDlgProc  = (DLGPROC)&CDirectoriesDlg::StartDialogProc;
		  ppage->dwFlags = PSP_DEFAULT | PSP_USECALLBACK | PSP_HASHELP;

		  hr = S_OK;
	}

	if( hr == S_OK )
	{
		  // Set common info in struct
		  ppage->dwSize      = sizeof(VSPROPSHEETPAGE);
		  ppage->hInstance   = g_LocaleInfo.GetUIHostLib();
		  ppage->lParam      = 0;
		  ppage->pfnCallback = NULL;
		  ppage->pcRefParent = NULL;
	}

	// Was page in our list?
	return hr;
}

IVCNotificationSource* CBuildPackage::GetNotificationSource()
{
	if (m_pVCNotificationSource == NULL)
	{
		if (FAILED(ExternalQueryService(SID_SVCService, __uuidof(IVCNotificationSource), (void **)&m_pVCNotificationSource)))
			m_pVCNotificationSource = NULL;
	}
	return m_pVCNotificationSource;
}


STDMETHODIMP CBuildPackage::Initialize()
{
    HRESULT hr = CVCProjBuildEvents::CreateInstance(&m_pProjBuildEvents);
	if (SUCCEEDED(hr) && m_pProjBuildEvents)
	{
		// Do generic initialization here...
		if (m_dwProjectEngineEventsCookie == 0)
		{
			hr = VsLoaderCoCreateInstance<VCProjectEngine>(CLSID_VCProjectEngineObject, &m_pProjectEngine);
			VSASSERT(SUCCEEDED(hr) && m_pProjectEngine, "Failed to create project engine!  Not a whole lot we can do without *that*");
			if (SUCCEEDED(hr) && m_pProjectEngine)
			{
				CComQIPtr<IConnectionPointContainer> pCPContainer;
				CComPtr<IConnectionPoint> pCP;
				pCPContainer = m_pProjectEngine;
				VSASSERT(pCPContainer, "Project engine isn't a connection point container!");
				if (pCPContainer)
				{
					hr = pCPContainer->FindConnectionPoint(__uuidof(IVCProjectEngineEvents), &pCP);
					VSASSERT(SUCCEEDED(hr) && pCP, "Failed to find connection point for IVCProjectEngineEvents!");
					if (pCP)
					{
						hr = pCP->Advise((IVCProjectEngineEvents*)(m_pProjBuildEvents), &m_dwProjectEngineEventsCookie);
					}
				}
			}
		}
	}
	// Create our image list.All the images exist in the same bitmap
	if(m_hImageList == 0)
	{
		m_hImageList = ImageList_LoadBitmap(_Module.GetModuleInstance(), (LPCOLESTR)IDB_IMAGELIST, BITMAPIMAGE_X, 0, IMAGELISTMASKCOLOR);
		if(!m_hImageList)
		{
			ATLTRACE(_T("Image list creation failed!\n"));	
		}
		else
		{	// Set overlay images
			ImageList_SetOverlayImage(m_hImageList, BMP_LINKOVERLAY-1, 1);
			ImageList_SetOverlayImage(m_hImageList, BMP_NOTINSCCOVERLAY-1,2);
			ImageList_SetOverlayImage(m_hImageList, BMP_CHECKEDOUTOVERLAY-1,3);
		}
	}

	// Start our thread. It will do nothing if it is already started.
	//StartThreading();

	return hr;

}

STDMETHODIMP CBuildPackage::QueryService(THIS_ REFGUID rsid, REFIID iid, void ** ppvObj)
{
	CHECK_POINTER_NULL(ppvObj);
		
    HRESULT hr = E_NOINTERFACE;
	if (rsid == SID_SProjectEngine)
	{
		if (m_pProjectEngine)
			hr = m_pProjectEngine->QueryInterface(iid, ppvObj);
	}
	else if (rsid == SID_SENCBuildSpawner)
	{
		CComPtr<IVCBuildEngine> spBuildEngine;
		GetBuildEngine(&spBuildEngine);
		if (spBuildEngine)
			hr = spBuildEngine->QueryInterface(iid, ppvObj);
	}
	else if (rsid == SID_SBuildErrorContext)
	{
		CComPtr<IVCBuildErrorContext> spBuildErrorContext;
		hr = GetBuildErrorContext(&spBuildErrorContext);
		if (SUCCEEDED(hr))
			hr = spBuildErrorContext->QueryInterface(iid, ppvObj);
	}
	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsPersistSolutionOpts
//---------------------------------------------------------------------------

STDMETHODIMP CBuildPackage::SaveUserOptions(IVsSolutionPersistence *pPersistence)
{
	CHECK_READ_POINTER_NULL(pPersistence);

	CComPtr<IVsSolution> pVsSolution;
	HRESULT hr = GetIVsSolution(&pVsSolution);
	VSASSERT(SUCCEEDED(hr), "Failed to obtain service IVsSolution; make sure we're calling from the primary thread");
	RETURN_ON_FAIL_OR_NULL(hr, pVsSolution);

	// VC Projects
	CComPtr<IEnumHierarchies> pEnum;
	hr = pVsSolution->GetProjectEnum( EPF_ALLPROJECTS|EPF_MATCHTYPE, IID_IVCArchy, &pEnum );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	pEnum->Reset();
	while ( true )
	{
		CComQIPtr<IVsHierarchy> pHierarchy;

		hr = pEnum->Next(1, &pHierarchy, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE )
			break;
		else if (pHierarchy == NULL)
			continue;

		CComVariant var;
		hr = pHierarchy->GetProperty(VSITEMID_ROOT, VSHPROPID_ProjectName, &var);
		wchar_t strName[32];
		wcsncpy( strName, var.bstrVal, 32 );
		strName[31] = NULL;

		CComQIPtr<IVsPersistSolutionOpts> pOpts = pHierarchy;
		if (pOpts == NULL)
			continue;
		hr = pPersistence->SavePackageUserOpts( pOpts, strName );
	}
	pEnum = NULL;

	// Exe projects
	hr = pVsSolution->GetProjectEnum( EPF_ALLPROJECTS|EPF_MATCHTYPE, IID_IExeHierarchy, &pEnum );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	pEnum->Reset();
	while ( true )
	{
		CComQIPtr<IVsHierarchy> pHierarchy;

		hr = pEnum->Next(1, &pHierarchy, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE )
			break;
		else if (pHierarchy == NULL)
			continue;

		CComVariant var;
		hr = pHierarchy->GetProperty(VSITEMID_ROOT, VSHPROPID_ProjectName, &var);
		wchar_t strName[32];
		wcsncpy( strName, var.bstrVal, 32 );
		strName[31] = NULL;

		CComQIPtr<IVsPersistSolutionOpts> pOpts = pHierarchy;
		if (pOpts == NULL)
			continue;

		hr = pPersistence->SavePackageUserOpts( pOpts, strName);
	}
	pEnum = NULL;

	// crash dump projects
	hr = pVsSolution->GetProjectEnum( EPF_ALLPROJECTS|EPF_MATCHTYPE, IID_ICrashDumpHierarchy, &pEnum );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	pEnum->Reset();
	while ( true )
	{
		CComQIPtr<IVsHierarchy> pHierarchy;

		hr = pEnum->Next(1, &pHierarchy, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE )
			break;
		else if (pHierarchy == NULL)
			continue;

		CComVariant var;
		hr = pHierarchy->GetProperty(VSITEMID_ROOT, VSHPROPID_ProjectName, &var);
		wchar_t strName[32];
		wcsncpy( strName, var.bstrVal, 32 );
		strName[31] = NULL;

		CComQIPtr<IVsPersistSolutionOpts> pOpts = pHierarchy;
		if (pOpts == NULL)
			continue;
		hr = pPersistence->SavePackageUserOpts( pOpts, strName );
	}

	return S_OK;
}

STDMETHODIMP CBuildPackage::LoadUserOptions(IVsSolutionPersistence *pPersistence, VSLOADUSEROPTS grfLoadOpts)
{
	CHECK_READ_POINTER_NULL(pPersistence);

	CComPtr<IVsSolution> pVsSolution;
	HRESULT hr = GetIVsSolution(&pVsSolution);
	VSASSERT(SUCCEEDED(hr), "Failed to obtain service IVsSolution; make sure we're calling from the primary thread");
	RETURN_ON_FAIL2(hr, S_OK);

	// VC Projects
	CComPtr<IEnumHierarchies> pEnum;
	hr = pVsSolution->GetProjectEnum( EPF_ALLPROJECTS|EPF_MATCHTYPE, IID_IVCArchy, &pEnum );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	pEnum->Reset();
	while ( true )
	{
		CComQIPtr<IVsHierarchy> pHierarchy;

		hr = pEnum->Next(1, &pHierarchy, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE )
			break;
		else if (pHierarchy == NULL)
			continue;

		CComVariant var;
		hr = pHierarchy->GetProperty(VSITEMID_ROOT, VSHPROPID_ProjectName, &var);
		wchar_t strName[32];
		wcsncpy( strName, var.bstrVal, 32 );
		strName[31] = NULL;

		CComQIPtr<IVsPersistSolutionOpts> pOpts = pHierarchy;
		if (pOpts == NULL)
			continue;
		hr = pPersistence->LoadPackageUserOpts( pOpts, strName );
	}
	pEnum = NULL;

	// Exe projects
	hr = pVsSolution->GetProjectEnum( EPF_ALLPROJECTS|EPF_MATCHTYPE, IID_IExeHierarchy, &pEnum );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	pEnum->Reset();
	while ( true )
	{
		CComQIPtr<IVsHierarchy> pHierarchy;

		hr = pEnum->Next(1, &pHierarchy, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE )
			break;
		else if (pHierarchy == NULL)
			continue;

		CComVariant var;
		hr = pHierarchy->GetProperty(VSITEMID_ROOT, VSHPROPID_ProjectName, &var);

		wchar_t strName[32];
		wcsncpy( strName, var.bstrVal, 32 );
		strName[31] = NULL;

		CComQIPtr<IVsPersistSolutionOpts> pOpts = pHierarchy;
		if (pOpts == NULL)
			continue;
		hr = pPersistence->LoadPackageUserOpts( pOpts, strName );
	}
	pEnum = NULL;

	// crash dump projects
	hr = pVsSolution->GetProjectEnum( EPF_ALLPROJECTS|EPF_MATCHTYPE, IID_ICrashDumpHierarchy, &pEnum );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	pEnum->Reset();
	while ( true )
	{
		CComQIPtr<IVsHierarchy> pHierarchy;

		hr = pEnum->Next(1, &pHierarchy, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE )
			break;
		else if (pHierarchy == NULL)
			continue;

		CComVariant var;
		hr = pHierarchy->GetProperty(VSITEMID_ROOT, VSHPROPID_ProjectName, &var);

		wchar_t strName[32];
		wcsncpy( strName, var.bstrVal, 32 );
		strName[31] = NULL;

		CComQIPtr<IVsPersistSolutionOpts> pOpts = pHierarchy;
		if (pOpts == NULL)
			continue;
		hr = pPersistence->LoadPackageUserOpts( pOpts, strName);
	}

	return S_OK;
}

STDMETHODIMP CBuildPackage::WriteUserOptions(IStream *pOptionsStream, LPCOLESTR pszKey)
{
	return E_NOTIMPL;	// CBuildPackage::WriteUserOptions
}

STDMETHODIMP CBuildPackage::ReadUserOptions(IStream *pOptionsStream, LPCOLESTR pszKey)
{
	return E_NOTIMPL;	// CBuildPackage::ReadUserOptions
}


void CBuildPackage::InitializeArchy(BSTR bstrFilename, CVCProjectNode* pProj, CVCArchy* pVCArchy)
{
	CStringW strFilename = bstrFilename;
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	_wsplitpath( (const wchar_t *) strFilename, drive, dir, fname, ext);

	CComBSTR bstrFileName = fname;

	if (pVCArchy)
		pVCArchy->SetImageList(GetBuildPkg()->m_hImageList);
	if (pProj)
		pProj->SetArchy(pVCArchy);		// Archy per project
	if (pVCArchy)
		pVCArchy->SetRootNode(pProj);
	// don't set the caption until the archy is initialized
	if (pProj)
		pProj->SetCaption(bstrFilename);
}

VCProjectEngine* CBuildPackage::GetProjectEngine()	// not ref-counted
{
	VSASSERT(m_pProjectEngine, "Project engine not initialized!");
	return m_pProjectEngine;
}

HRESULT CBuildPackage::GetBuildEngine(IVCBuildEngine** ppBuildEngine)
{
	CHECK_POINTER_NULL(ppBuildEngine);
	*ppBuildEngine = NULL;
	if (m_pProjectEngine == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CComPtr<IDispatch> pDispEngine;
	CComQIPtr <IVCProjectEngineImpl> pProjectEngineImpl;
	pProjectEngineImpl = m_pProjectEngine;
	pProjectEngineImpl->get_BuildEngine(&pDispEngine);
	CComQIPtr<IVCBuildEngine> spBuildEngine =  pDispEngine;
	*ppBuildEngine = spBuildEngine.Detach();
	return (*ppBuildEngine == NULL ? S_FALSE : S_OK);
}

IDispatch* CBuildPackage::GetAutoProjects()
{
	if (m_spAutoProjects == NULL && m_pProjectEngine != NULL)
	{
		CComPtr<IDispatch> spProjects;
		m_pProjectEngine->get_Projects(&spProjects);
		VSASSERT(spProjects != NULL, "Failed to get projects collection from the project engine!");
		if (spProjects != NULL)
		{
			CAutoProjects* pAutoProjects = NULL;
			CAutoProjects::CreateInstance(&pAutoProjects, spProjects);
			m_spAutoProjects = pAutoProjects;
			if (pAutoProjects != NULL)
				pAutoProjects->Release();
		}
	}
	return m_spAutoProjects;
}

STDMETHODIMP CBuildPackage::get_OfficialName(BSTR *pbstrName) 
{
	CComBSTR bstrString;
	if (!bstrString.LoadString(IDS_PROD_NAME))
	{
		*pbstrName = NULL;
		return E_OUTOFMEMORY;
	}
	*pbstrName = bstrString.Detach();
	return S_OK;
}

STDMETHODIMP CBuildPackage::get_ProductDetails(BSTR *pbstrProductDetails)
{
	CComBSTR bstrString;
	if (!bstrString.LoadString(IDS_PROD_DESC))
	{
		*pbstrProductDetails = NULL;
		return E_OUTOFMEMORY;
	}

	DWORD nType = REG_SZ;
	DWORD nSize = MAX_PATH;
 	HKEY hSectionKey = NULL;

	// REVIEW: Should use Alternate registry root !
	LONG lRet = RegOpenKeyExW( HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\VisualStudio\\7.0\\Setup\\VC", 0, KEY_READ, &hSectionKey );
	if( hSectionKey )
	{
		wchar_t szVersion[MAX_PATH+1] = {0};
		lRet = RegQueryValueExW( hSectionKey, L"Version", NULL, &nType, (LPBYTE)szVersion, &nSize );
		bstrString.Append( szVersion );
		RegCloseKey( hSectionKey );
	}

	*pbstrProductDetails = bstrString.Detach();
	return S_OK;
} 


/////////////////////////////////////////////////////////////////////////////
// CVCProjectFactory

CVCProjectFactory::CVCProjectFactory(void)
{
}

CVCProjectFactory::~CVCProjectFactory(void)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
STDMETHODIMP CVCProjectFactory::CanCreateProject( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ DWORD grfCreateProj,
            /* [out] */ BOOL *pfCanCreate)
{
	CHECK_POINTER_NULL(pfCanCreate);

	*pfCanCreate = TRUE;

	// if we're running a command line build, we cannot convert projects!
	if( !FIsIDEInteractive() )
	{
		CStringW strExt;
		CStringW strFilename = pszFilename;
		int pos = strFilename.ReverseFind( L'.' );
		if( pos != -1 )
			strExt = strFilename.Right( strFilename.GetLength() - pos );
		if( strExt.IsEmpty() || _wcsicmp( strExt, L".vcproj" ) != 0 )
		{
			*pfCanCreate = FALSE;
			
			// REVIEW !!!! This must be localized, and the information should be routed through the output window !
			// FIX ME !!!!
			wprintf( L"\nError: The specified project must be converted before it can be built.\n" );

			// CComQIPtr<IVsOutputWindowPane> pPane;
			// IUnknown* pUnkPane = NULL;
			// pPane->OutputTaskItemStringEx(bstrFullMessage, TP_HIGH, CAT_BUILDCOMPILE, L"", BMP_COMPILE, bstrFile, nLine-1, bstrMessage, bstrHelpKeyword);
		}
	}
	
	return S_OK;
}
        
STDMETHODIMP CVCProjectFactory::CreateProject( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled)
{
	// helper object to track "in project load" status
	CInProjectLoadHelper inProjLoad;

	// load the project normally
	HRESULT hr = InitializeForOwner( pszFilename, pszLocation, pszName, grfCreateProj, riid, NULL, ppUnk, pfCanceled );
	if (FAILED(hr))
	{
		if (hr != E_FAIL)
			hr = E_ABORT;
		return hr;
	}

	// look for the VSEF OwnerKey property on the project
	// ppUnk now has the address of a ptr to the hierarchy (which we know to
	// be a VCArchy)
	CComQIPtr<IVsHierarchy> pHier = (IUnknown*)*ppUnk;
	RETURN_ON_NULL2(pHier, E_NOINTERFACE);
	CVCArchy *pArchy = (CVCArchy*)pHier.p;
	CComPtr<VCProject> pProj;
	pArchy->GetVCProject( NULL, &pProj );
	RETURN_ON_NULL(pProj);
	CComBSTR bstrOwnerKey;
	pProj->get_OwnerKey( &bstrOwnerKey );

	// if we have an OwnerKey, we have to dump the project and let it be
	// re-loaded by the owner
	if( bstrOwnerKey.Length() != 0 )
	{
		if (*ppUnk)
		{
			((IUnknown*)(*ppUnk))->Release();
			*ppUnk = NULL;
		}
		// delete & clean up the loaded project
		CComQIPtr<IDispatch> pDisp = pProj;
		VSASSERT( pDisp, "Failed to get IDispatch from VCProject! How did this happen?!" );
		CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pBuildPackage->GetProjectEngine();
		if (spProjEngineImpl)
			spProjEngineImpl->RemoveProjectUnderShell( pDisp );
		// get the owner key (guid string)
		CString strKey = bstrOwnerKey;
		CComPtr<IVsProjectFactory> pOwnerPF;

        CLSID guidOwnerProject;
		hr = CLSIDFromString( bstrOwnerKey, &guidOwnerProject );
		RETURN_ON_FAIL(hr);
		CComPtr<IVsSolution> spSolution;
		GetBuildPkg()->GetIVsSolution( &spSolution );
		hr = spSolution->GetProjectFactory( 0, &guidOwnerProject, pszFilename, &pOwnerPF );
		RETURN_ON_FAIL_OR_NULL(hr, pOwnerPF);
		return pOwnerPF->CreateProject( pszFilename, pszLocation, pszName, grfCreateProj, riid, ppUnk, pfCanceled );
	}
	else
		return hr;
}

//---------------------------------------------------------------------------
// interface: IVsOwnedProjectFactory
//---------------------------------------------------------------------------
STDMETHODIMP CVCProjectFactory::PreCreateForOwner( 
            /*[in]*/  IUnknown              *pUnkOwner,
            /*[out]*/ IUnknown              **pppUnkInner,
            /*[out]*/ VSOWNEDPROJECTOBJECT  *pCookie )
{
    //
    // create an Aggregated version of CVsProjHierarchy
    //
    CComPolyObject<CVCArchy> *pAggHier;
    
    HRESULT hr = CComPolyObject<CVCArchy>::CreateInstance( pUnkOwner, &pAggHier);
	RETURN_ON_FAIL_OR_NULL2(hr, pAggHier, E_OUTOFMEMORY);

	CVCArchy *pHier;  // created with Ref count 0

	// assign pWebHier to the inner object
	pHier = &(pAggHier->m_contained);
	RETURN_ON_NULL(pHier);
	pHier->AddRef(); // addref to keep alive - don't forget to release this ref in Initialize for owner

	// get the inner IUnknown
	if( SUCCEEDED( hr = pAggHier->QueryInterface( IID_IUnknown, (void**)pppUnkInner ) ) )
		*pCookie = (VSOWNEDPROJECTOBJECT)(pHier);
	else
		// destroy the pAggHier (balances AddRef above)
		pHier->Release();

    return hr;
}

STDMETHODIMP CVCProjectFactory::InitializeForOwner( 
            /*[in]*/ LPCOLESTR                  pszFilename,
            /*[in]*/ LPCOLESTR                  pszLocation,
            /*[in]*/ LPCOLESTR                  pszName,
            /*[in]*/ VSCREATEPROJFLAGS          grfCreateProj,
            /*[in]*/ REFIID                     riid,
            /*[in]*/ VSOWNEDPROJECTOBJECT       cookie, // additional parameter
            /*[out, iid_is(iidProject)]*/void  **ppUnk,
            /*[out]*/ BOOL                      *pfCanceled)
{
	HRESULT hr = E_FAIL;
	DWORD startTime, endTime;
	VARIANT_BOOL bLog = VARIANT_FALSE;
	CComPtr<IDispatch> pDispProject;
	bool bConverted = false;

	// Enterprise frameworks VCArchy could have been created in PreCreateForOwner.
	s_EFPolicyCookie = cookie;

	CDirW dirTo;
	CDirW dirFrom;

	if (ppUnk != NULL && pfCanceled != NULL)
	{
		*ppUnk = NULL;
		*pfCanceled = FALSE;
		CComBSTR bstrFileName;

		// WARNING ! Although this looks like it is doing nothing, it actually initializes the Wizards package so 
		// that someone SetSite's them.  It also needs to go BEFORE the first project gets loaded or our conversion
		// DLL can't get CoCreate'd.  Go figure.
		// DO NOT REMOVE without asking DaveWa
		if( FIsIDEInteractive() )
		{
			CComPtr<IVsPackage> pUnk;
			hr = ExternalQueryService(SID_SVCService, __uuidof(IVsPackage), (void**)&pUnk);
		}
		// END DO NOT REMOVE

		if(grfCreateProj & CPF_CLONEFILE) // New project
		{
			CPathW pathTo;
			CPathW pathFrom;

			CStringW strTo = pszLocation;
			BOOL bOK = dirTo.CreateFromString(strTo, TRUE);
			VSASSERT(bOK, "Failed create destination directory path!");

			CStringW strName = pszName;
			bOK = pathTo.CreateFromDirAndRelative(dirTo, strName);
			VSASSERT(bOK, "Failed to create destination project file path!");

			// REVIEW: LOGO BUG -- foo.bar should become foo.bar.vcproj instead of foo.vcproj...
			pathTo.ChangeExtension(L".vcproj");
			CComBSTR bstrTo = pathTo;

			CStringW strFrom = pszFilename;
			pathFrom.Create(strFrom);
			dirFrom.CreateFromPath(pathFrom);
			
			if( !dirTo.CreateOnDisk() || !CopyFileW( pathFrom, pathTo, FALSE) || !SetFileAttributesW( pathTo, FILE_ATTRIBUTE_NORMAL) )
				*pfCanceled = TRUE;
			if( *pfCanceled )
			{
				// report error
				LPVOID lpErrMsg;
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPTSTR)&lpErrMsg, 0, NULL );
				ReportError( CComBSTR( (LPTSTR)lpErrMsg ), E_ACCESSDENIED );
				LocalFree( lpErrMsg );
				return E_ACCESSDENIED;
			}
			
			bstrFileName = bstrTo;
		}
		else  // Open existing project
		{
			// see if it is a conversion project...
			CStringW strFile = pszFilename;
			CPathW path;
			path.Create(strFile);
			CComBSTR bstrShortFile = path.GetFileName();
			bstrFileName = pszFilename;

			CStringW strExt = strFile.Right(7);
			strExt.MakeLower();
			if( strExt != L".vcproj" )
			{
				bConverted = true;

				if ( CConvertDlg::s_bConvertNone == TRUE )
				{
					*pfCanceled = TRUE;
					return S_OK;
				}
				else if ( CConvertDlg::s_bConvertAll == FALSE )
				{
					CComPtr<IVsUIShell> pUIShell = NULL;
					HWND hwnd;
					HRESULT hr;
					hr = GetBuildPkg()->GetVsUIShell(&pUIShell, TRUE /* in main thread */);
					if (SUCCEEDED(hr) && pUIShell)
					{
						pUIShell->SetForegroundWindow();
						hr = pUIShell->GetDialogOwnerHwnd(&hwnd);
						hr = pUIShell->EnableModeless(FALSE);
						CConvertDlg cvtDlg(bstrShortFile);
						INT_PTR result = cvtDlg.DoModal(hwnd);
						hr = pUIShell->EnableModeless(TRUE);
						if( CConvertDlg::s_bCancel == TRUE )
						{
							*pfCanceled = TRUE;
							return S_OK;
						}
					}
				}
			}

			// if we're doing performance logging
			GetBuildPkg()->GetProjectEngine()->get_PerformanceLogging( &bLog );
			if( bLog == VARIANT_TRUE )
			{
				// start time
				startTime = GetTickCount();
				fprintf( stdout, "Project Load start: %d\n", startTime );
			}

		}

		CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = GetBuildPkg()->m_pProjectEngine;
		if (spProjEngineImpl == NULL)
		{
			*pfCanceled = TRUE;
			return E_FAIL;
		}
		hr = spProjEngineImpl->LoadProjectUnderShell(bstrFileName, &pDispProject);
		// if we can't load the project file, bail
		if( FAILED( hr ) )
		{
			CStringW strFileName = bstrFileName;
			CStringW strMessage;
			strMessage.Format(IDS_WARN_PRJOPEN_FAILED, strFileName);
			CComBSTR bstrMessage = strMessage;
			ReportError( bstrMessage, hr );

			*pfCanceled = TRUE;
			if (hr != E_FAIL)
				hr = E_ABORT;
			return hr;
		}
		// if we were given a name, we need to set the correct name of the project
		if( pszName )
		{
			CComQIPtr<VCProject> pProject = pDispProject;
			VSASSERT( pProject, "Failed to get VCProject object back from LoadProject!" );
			RETURN_ON_NULL(pProject);
			// strip the file extension off the name
			CStringW strName = pszName;
			int idx = strName.ReverseFind( L'.' );
			if( idx != -1 )
				strName = strName.Left( idx );
			pProject->put_Name( CComBSTR( strName ) );
		}
	
		if (SUCCEEDED(hr) && pDispProject)
		{
			// if we were converted, and we don't have a "root node" string yet,
			// get the name and set our global "root node" place holder string
			if( bConverted && !g_wszRootProjectName )
			{
				CComBSTR bstrName;
				CComQIPtr<VCProject> pProject = pDispProject;
				if( pProject )
				{
					pProject->get_Name( &bstrName );
					g_wszRootProjectName = new wchar_t[bstrName.Length() + 1];
					memset( g_wszRootProjectName, 0, (bstrName.Length() + 1) * sizeof(wchar_t) );
					wcscpy( g_wszRootProjectName, bstrName );
				}
			}

			// get the IDispatch of the project and add all elements of that project to the UI.
			CComQIPtr<IVCExternalCookie> pExCookie = pDispProject;
			if (pExCookie)
			{
				void* pCookie;
				hr = pExCookie->get_ExternalCookie(&pCookie);
				CVCProjectNode *pProjNode = (CVCProjectNode *)pCookie;
				CComPtr<IVsHierarchy> spHier;
				if (pProjNode != NULL)
					pProjNode->GetVsHierarchy(&spHier, TRUE);
				*ppUnk = NULL;
				RETURN_ON_NULL(spHier);
				hr = spHier->QueryInterface(riid, ppUnk);
				if (FAILED(hr))
				{
					*ppUnk = NULL;
					return hr;
				}
// 				IVsHierarchy* pHier = spHier;
// 				if (pHier && !cookie )
// 					pHier->Release(); // yep, we're now down one ref when we leave here.  that's OK and necessary.
				// if we're a nested hierarchy
				if( cookie ) 					
				{
					IUnknown *pUnk = NULL;
					if( SUCCEEDED( spHier->QueryInterface( IID_IUnknown, (void**)&pUnk ) ) && pUnk )
					{
						pUnk->Release();	// release the AddRef from the QI
						pUnk->Release();	// release the EXACT interface that was addref'd in PreCreateForOwner (the controlling Unknown)
					}
				}
				else
				{
					IVsHierarchy *pHier = spHier;
					if( pHier )
						pHier->Release(); // yep, we're now down one ref when we leave here.  that's OK and necessary.
				}
					
				hr = S_OK;
			}
		}
	}

	if( *ppUnk == NULL )
		hr = E_FAIL;
	if(grfCreateProj & CPF_CLONEFILE)
	{
		// Now do the stupid things of trying to copy template files.

		CComQIPtr<VCProject> pProj = pDispProject;
		CComPtr<IDispatch> pDispFiles;
		pProj->get_Files(&pDispFiles);
		CComQIPtr<IVCCollection> pCollection; 
		pCollection = pDispFiles;
		CComPtr<IEnumVARIANT>		pEnum;
		HRESULT hrT;
		hrT = pCollection->_NewEnum( (IUnknown **)( &pEnum ) );
		if (SUCCEEDED(hrT) && pEnum)
		{
			pEnum->Reset();
			while ( true )
			{
				CComVariant var;
				CComQIPtr<VCFile> pFile;
				
				hrT = pEnum->Next(1, &var, NULL);
				if( FAILED(hrT) ) break;
				else if( hrT == S_FALSE ) break;

				pFile = var.punkVal;
				if (!pFile)
					continue;

				// copy it from where it might have been to where it should be
				CPathW pathFrom;
				CPathW pathTo;

				CComBSTR bstrRelative;
				hrT = pFile->get_RelativePath( &bstrRelative );
				CStringW strRel = bstrRelative;
	
				pathFrom.CreateFromDirAndRelative(dirFrom, strRel);
				pathTo.CreateFromDirAndRelative(dirTo, strRel);
				CDirW dirTo;
				if (dirTo.CreateFromPath(pathTo))
					dirTo.CreateOnDisk();	// don't care about error conditions; CopyFileW will take care of that
	
				if( !CopyFileW( pathFrom, pathTo, FALSE) || !SetFileAttributesW( pathTo, FILE_ATTRIBUTE_NORMAL) )
					*pfCanceled = TRUE;
				if( *pfCanceled )
				{
					// report error
					LPVOID lpErrMsg;
					FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPTSTR)&lpErrMsg, 0, NULL );
					ReportError( CComBSTR( (LPTSTR)lpErrMsg ), E_ACCESSDENIED );
					LocalFree( lpErrMsg );
					return E_ACCESSDENIED;
				}
			}
		}
	}
	else
	{
		// if we're doing performance logging
		if( bLog == VARIANT_TRUE )
		{
			// start time
			endTime = GetTickCount();
			fprintf( stdout, "Project Load end: %d\n", endTime );
		}
	}

	
	
	if( FAILED(hr) )
	{
		CComBSTR bstrMessage;
		bstrMessage.LoadString(IDS_WARN_PRJOPEN_FAILED);
		ReportError( bstrMessage, hr );

		*pfCanceled = TRUE;
		if (hr != E_FAIL)
			hr = E_ABORT;
		return hr;
	}
	// BBT End
	ENDBOOT_projbld();

	return hr;
}

STDMETHODIMP CVCProjectFactory::SetSite( /* [in] */ IServiceProvider *pSP)
{
	m_srpIServiceProvider.Release();
	if (NULL != pSP)
	{
		pSP->AddRef();
		m_srpIServiceProvider = pSP;
	}
	return S_OK;
}

STDMETHODIMP CVCProjectFactory::Close(void)
{
	VSASSERT(m_srpIServiceProvider != NULL, "Service provider not set on CVCProjectFactory!");
	m_srpIServiceProvider.Release();
	return S_OK;
}

CVCProjectNode* CBuildPackage::GetProjectNode(VCProject* pProject)
{
	RETURN_ON_NULL2(pProject, NULL);
	
	CComQIPtr<IVCExternalCookie> spExtCookie = pProject;
	RETURN_ON_NULL2(spExtCookie, NULL);

	void* pCookie;
	spExtCookie->get_ExternalCookie(&pCookie);
	return (CVCProjectNode*)pCookie;
}

HRESULT CBuildPackage::GetProjectNodeForProjConfig(VCConfiguration* pProjCfg, CVCProjectNode** ppProjNode)
{
	CHECK_POINTER_NULL(ppProjNode);
	CHECK_READ_POINTER_NULL(pProjCfg);

	CComPtr<IDispatch> spDispProject;
	pProjCfg->get_Project(&spDispProject);
	CComQIPtr<VCProject> spProject = spDispProject;
	VSASSERT(spProject != NULL, "Project config doesn't have a project!  Is it zombie?");
	CHECK_ZOMBIE(spProject, IDS_ERR_PROJ_ZOMBIE);

	*ppProjNode = GetProjectNode(spProject);
	CHECK_ZOMBIE(*ppProjNode, IDS_ERR_PROJ_ZOMBIE);
	return S_OK;
}

HRESULT CBuildPackage::GetArchyForProjConfig(VCConfiguration* pProjCfg, CVCArchy** ppArchy)
{
	CHECK_POINTER_NULL(ppArchy);
	CHECK_READ_POINTER_NULL(pProjCfg);
	*ppArchy = NULL;

	CVCProjectNode* pProjNode = NULL;
	HRESULT hr = GetProjectNodeForProjConfig(pProjCfg, &pProjNode);
	RETURN_ON_FAIL(hr);
	CHECK_ZOMBIE(pProjNode, IDS_ERR_PROJ_ZOMBIE);

	*ppArchy = pProjNode->GetArchy();
	CHECK_ZOMBIE(*ppArchy, IDS_ERR_PROJ_ZOMBIE);

	return S_OK;
}

HRESULT CBuildPackage::GetHierarchyForProjConfig(VCConfiguration* pProjCfg, IVsHierarchy** ppHier, BOOL bInMainThread /* = FALSE */)
{
	CHECK_POINTER_NULL(ppHier);
	*ppHier = NULL;

	CVCProjectNode* pProjNode = NULL;
	HRESULT hr = GetProjectNodeForProjConfig(pProjCfg, &pProjNode);
	RETURN_ON_FAIL(hr);
	CHECK_ZOMBIE(pProjNode, IDS_ERR_PROJ_ZOMBIE);

	return pProjNode->GetVsHierarchy(ppHier, bInMainThread);
}

void CBuildPackage::AddSolutionRef()
{
	CComPtr<IVsSolution> spSolution;
	GetIVsSolution(&spSolution);
	m_nSolutionRefCount++;
}

HRESULT CBuildPackage::GetIVsSolution(IVsSolution** ppSolution)	// intended for main thread use only
{
	CHECK_POINTER_NULL(ppSolution);
	if (!m_marshalVsSolution.IsInit())
	{
		CComPtr<IVsSolution> pVsSolution;
		HRESULT hr = ExternalQueryService(__uuidof(IVsSolution), __uuidof(IVsSolution), (void **)&pVsSolution);
		VSASSERT(SUCCEEDED(hr), "Failed to obtain service IVsSolution; make sure we're calling from the primary thread");
		if (SUCCEEDED(hr))
			m_marshalVsSolution.Init((IVsSolution *)pVsSolution);
	}
	return m_marshalVsSolution.GetPtr(ppSolution);
}

void CBuildPackage::ReleaseSolutionRef()
{
	m_nSolutionRefCount--;
	if (m_nSolutionRefCount == 0)
		m_marshalVsSolution.Clear();
	VSASSERT(m_nSolutionRefCount >= 0, "Negative refcount spells trouble...");
}

HRESULT CBuildPackage::GetSolutionBuildManager(IVsSolutionBuildManager** ppSolnBldMgr, BOOL bInMainThread /* = FALSE */)
{
	CHECK_POINTER_NULL(ppSolnBldMgr);
	HRESULT hr = S_OK;
	if (m_spSolutionBuildManager == NULL && bInMainThread)
	{
		hr = ExternalQueryService(SID_SVsSolutionBuildManager, __uuidof(IVsSolutionBuildManager), 
			(void **)&m_spSolutionBuildManager);
		m_marshalSolutionBuildManager.Init(m_spSolutionBuildManager);
	}
	RETURN_ON_FAIL(hr);
	if (bInMainThread)
		return m_spSolutionBuildManager.CopyTo(ppSolnBldMgr);
	RETURN_ON_NULL(m_spSolutionBuildManager);
	
	hr = m_marshalSolutionBuildManager.UnmarshalPtr(ppSolnBldMgr); 
	RETURN_ON_FAIL_OR_NULL(hr, *ppSolnBldMgr);

	return hr;
}

HRESULT CBuildPackage::GetVsUIShell(IVsUIShell** ppShell, BOOL bInMainThread /* = FALSE */)
{
	CHECK_POINTER_NULL(ppShell);
	HRESULT hr = S_OK;
	if (m_spVsUIShell == NULL && bInMainThread)
	{
		hr = ExternalQueryService(SID_SVsUIShell, __uuidof(IVsUIShell), (void **)&m_spVsUIShell);
		m_marshalVsUIShell.Init(m_spVsUIShell);
	}
	RETURN_ON_FAIL(hr);
	if (bInMainThread)
		return m_spVsUIShell.CopyTo(ppShell);
	RETURN_ON_NULL(m_spVsUIShell);

	hr = m_marshalVsUIShell.UnmarshalPtr(ppShell); 
	RETURN_ON_FAIL_OR_NULL(hr, *ppShell);

	return hr;
}

HRESULT CBuildPackage::GetTaskList(IVsTaskList** ppTaskList, BOOL bInMainThread /* = FALSE */)
{
	CHECK_POINTER_NULL(ppTaskList);
	HRESULT hr = S_OK;
	if (m_pTaskList == NULL && bInMainThread)
	{
		hr = ExternalQueryService(SID_SVsTaskList, __uuidof(IVsTaskList), (void **)&m_pTaskList);
		m_marshalTaskList.Init(m_pTaskList);
	}
	RETURN_ON_FAIL(hr);
	if (bInMainThread)
		return m_pTaskList.CopyTo(ppTaskList);
	RETURN_ON_NULL(m_pTaskList);

	hr = m_marshalTaskList.UnmarshalPtr(ppTaskList); 
	RETURN_ON_FAIL_OR_NULL(hr, *ppTaskList);

	return hr;
}

STDMETHODIMP CBuildPackage::HasConfigurationDependencies(VCConfiguration* pProjCfg, VARIANT_BOOL* pbHasDeps)
{
	CHECK_POINTER_NULL(pbHasDeps);
	*pbHasDeps = VARIANT_FALSE;

	CComPtr<IVsHierarchy> spHier;
	HRESULT hr = GetHierarchyForProjConfig(pProjCfg, &spHier);
	RETURN_ON_FAIL_OR_NULL(hr, spHier);

	CComPtr<IVsSolutionBuildManager> spSolnBldMgr;
	hr = GetSolutionBuildManager(&spSolnBldMgr);
	RETURN_ON_FAIL(hr);
	if (spSolnBldMgr == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	ULONG ulActual;
	hr = spSolnBldMgr->GetProjectDependencies(spHier, 0, NULL, &ulActual);
	RETURN_ON_FAIL(hr);

	if (ulActual != 0)
		*pbHasDeps = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CBuildPackage::GetConfigurationDependencies(VCConfiguration* pProjCfg, 
	IVCBuildOutputItems** ppBuildOutputs)
{
	CHECK_POINTER_NULL(ppBuildOutputs);
	*ppBuildOutputs = NULL;
	CHECK_READ_POINTER_NULL(pProjCfg);

	CComPtr<IVsHierarchy> spHier;
	HRESULT hr = GetHierarchyForProjConfig(pProjCfg, &spHier);
	RETURN_ON_FAIL_OR_NULL(hr, spHier);

	CComPtr<IVsSolutionBuildManager> spSolnBldMgr;
	hr = GetSolutionBuildManager(&spSolnBldMgr);
	RETURN_ON_FAIL(hr);
	if (spSolnBldMgr == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	ULONG ulActual;
	hr = spSolnBldMgr->GetProjectDependencies(spHier, 0, NULL, &ulActual);
	RETURN_ON_FAIL(hr);

	if (ulActual == 0)
		return S_OK;

	IVsHierarchy** rgpIVsHierarchy = (IVsHierarchy**) ::CoTaskMemAlloc(ulActual * sizeof(IVsHierarchy *));
	memset(rgpIVsHierarchy, 0, sizeof(IVsHierarchy*)*ulActual);
	RETURN_ON_NULL2(rgpIVsHierarchy, E_OUTOFMEMORY);

	ULONG ulActual2;
	hr = spSolnBldMgr->GetProjectDependencies(spHier, ulActual, rgpIVsHierarchy, &ulActual2);
	VSASSERT(ulActual == ulActual2, "Mismatch in number of dependencies based on whether we're just looking for the number or trying to get them!");
	VSASSERT(SUCCEEDED(hr), "Got the number without getting any?!?");
	if (SUCCEEDED(hr))
	{
		CVCBuildOutputItems* pOutputItemsObj = NULL;
		CVCBuildOutputItems::CreateInstance(&pOutputItemsObj);
		if (pOutputItemsObj != NULL)
		{
			for (ULONG idx = 0; idx < ulActual2; idx++)
			{
				CComPtr<IVsHierarchy> pVsArchy = rgpIVsHierarchy[idx];
				if (pVsArchy == NULL)
					continue;

				CComPtr<IVsProjectCfg> spVsProjCfg;
				HRESULT hr2 = spSolnBldMgr->FindActiveProjectCfg(NULL, NULL, pVsArchy, &spVsProjCfg);
				if (FAILED(hr2) || spVsProjCfg == NULL)
					continue;

				CComPtr<IVsEnumOutputs> spEnumOutputs;
				hr2 = spVsProjCfg->EnumOutputs(&spEnumOutputs);
				if (FAILED(hr2) || spEnumOutputs == NULL)
					continue;
				spEnumOutputs->Reset();

				while (TRUE)
				{
					CComPtr<IVsOutput> spOutput;
					ULONG ulRet = 0;
					hr2 = spEnumOutputs->Next(1, &spOutput, &ulRet);
					if (hr2 != S_OK || spOutput == NULL)
						break;
					pOutputItemsObj->Add(spOutput);
				}
			}
			*ppBuildOutputs = pOutputItemsObj;	// gets the ref from CreateInstance above...
		}
	}
	
	::CoTaskMemFree(rgpIVsHierarchy);
	return S_OK;
}

STDMETHODIMP CBuildPackage::GetUIConfigurationForVCConfiguration(VCConfiguration* pProjCfg, 
	IVCGenericConfiguration** ppGenCfg)
{
	CHECK_POINTER_NULL(ppGenCfg);
	CHECK_READ_POINTER_NULL(pProjCfg);

	CVCArchy* pArchy;
	HRESULT hr = GetArchyForProjConfig(pProjCfg, &pArchy);
	RETURN_ON_FAIL_OR_NULL(hr, pArchy);

	return pArchy->CreateGenCfg(pProjCfg, ppGenCfg);
}

STDMETHODIMP CBuildPackage::get_SolutionFile(BSTR* pbstrSolnFile)
{
	CHECK_POINTER_NULL(pbstrSolnFile);
	return m_bstrSolutionFile.CopyTo(pbstrSolnFile);
}

STDMETHODIMP CBuildPackage::SaveProject(VCProject* pProject, LPCOLESTR szFileName)
{
	// any failure return from here will cause the project engine to go ahead and save its way, anyway
	CVCProjectNode* pProjNode = GetProjectNode(pProject);
	CHECK_ZOMBIE(pProjNode, IDS_ERR_PROJ_ZOMBIE);

	CComPtr<IVsHierarchy> spHier;
	HRESULT hr = pProjNode->GetVsHierarchy(&spHier, TRUE);
	RETURN_ON_FAIL_OR_NULL(hr, spHier);

	CComPtr<IVsRunningDocumentTable> spDocTable;
	hr = ExternalQueryService(SID_SVsRunningDocumentTable, __uuidof(IVsRunningDocumentTable), (void **)&spDocTable);
	RETURN_ON_FAIL_OR_NULL(hr, spDocTable);

	CComPtr<VCProject> spProject;
	hr = pProjNode->GetVCProject(&spProject);
	RETURN_ON_FAIL_OR_NULL(hr, spProject);

	// the whole point of this next section is to determine whether the new and old names are the same...
	CStringW strNewName = szFileName;
	if (!strNewName.IsEmpty())
	{
		CComBSTR bstrNewName = szFileName;
		VARIANT_BOOL bMatches = VARIANT_TRUE;
		hr = spProject->MatchName(bstrNewName, VARIANT_FALSE, &bMatches);
		RETURN_ON_FAIL(hr);

		if (bMatches == VARIANT_FALSE)
		{
			CPathW pathNew;
			if (pathNew.Create(strNewName))
			{
				pathNew.ChangeExtension(L".vcproj");
				strNewName = (const wchar_t*)pathNew;
			}

			CComBSTR bstrOldName;
			hr = spProject->get_ProjectFile(&bstrOldName);
			RETURN_ON_FAIL(hr);

			CStringW strOldName = bstrOldName;
			CPathW pathOld;
			if (pathOld.Create(strOldName))
				strOldName = (const wchar_t*)pathOld;

			if (!strOldName.IsEmpty() && !strNewName.IsEmpty() && _wcsicmp(strOldName, strNewName) != 0)
			{
				hr = spDocTable->RenameDocument(strOldName, strNewName, spHier, VSITEMID_ROOT);
				RETURN_ON_FAIL(hr);
			}
		}
	}

	return spDocTable->SaveDocuments(RDTSAVEOPT_ForceSave, spHier, VSITEMID_ROOT, NULL);
}

HRESULT CBuildPackage::DoFallbackAddConfiguration(VCProject* pProject, BSTR bstrConfigName)
{
	CComQIPtr<IVCProjectImpl> pProjImpl = pProject;
	RETURN_ON_NULL2(pProjImpl, E_UNEXPECTED);
	return pProjImpl->AddConfigurationInternal(bstrConfigName);
}

STDMETHODIMP CBuildPackage::AddConfigurationToProject(VCProject* pProject, BSTR bstrConfigName)
{
	CVCProjectNode* pProjNode = GetProjectNode(pProject);
	if (pProjNode == NULL)
		return DoFallbackAddConfiguration(pProject, bstrConfigName);

	CComPtr<IVsHierarchy> spHier;
	HRESULT hr = pProjNode->GetVsHierarchy(&spHier, TRUE);
	if (FAILED(hr) || spHier == NULL)
		return DoFallbackAddConfiguration(pProject, bstrConfigName);

	CComPtr<IVsExtensibility> pExtService;
	hr = ExternalQueryService(SID_SVsExtensibility, __uuidof(IVsExtensibility), (LPVOID*)&pExtService);
	if(FAILED(hr))
		return DoFallbackAddConfiguration(pProject, bstrConfigName);

	CComPtr<ConfigurationManager> spConfigurationManager;
	hr = pExtService->GetConfigMgr(spHier, VSITEMID_ROOT, &spConfigurationManager);
	if (FAILED(hr) || spConfigurationManager == NULL)
		return DoFallbackAddConfiguration(pProject, bstrConfigName);

	CComPtr<Configurations> spConfigurations;
	return spConfigurationManager->AddConfigurationRow(bstrConfigName, CComBSTR(L""), VARIANT_TRUE, &spConfigurations);
}

STDMETHODIMP CBuildPackage::AddPlatformToProject(VCProject* pProject, LPCOLESTR szPlatformName)
{
	// FUTURE: let the solution in on what we're up to
	CComQIPtr<IVCProjectImpl> spProjImpl = pProject;
	RETURN_ON_NULL2(spProjImpl, E_UNEXPECTED);
	return spProjImpl->AddPlatformInternal(szPlatformName);
}

STDMETHODIMP CBuildPackage::RemovePlatformFromProject(VCProject* pProject, IDispatch* pPlatform)
{
	// FUTURE: let the solution in on what we're up to
	CComQIPtr<IVCProjectImpl> spProjImpl = pProject;
	RETURN_ON_NULL2(spProjImpl, E_UNEXPECTED);
	return spProjImpl->RemovePlatformInternal(pPlatform);
}

STDMETHODIMP CBuildPackage::ErrorCloseOfProject(VCProject* pProject)
{	// call this BEFORE removing a 'dead' project from the project collection; ERROR CLOSE OF PROJECT ONLY
	CVCProjectNode* pProjNode = GetProjectNode(pProject);
	RETURN_ON_NULL2(pProjNode, S_OK);

	CVCArchy* pArchy = pProjNode->GetArchy();
	if (pArchy)
		pArchy->Release();	// this gets rid of the refcount that the solution would have had

	return S_OK;
}

void CBuildPackage::GetVsProject(VCProject* pProject, CComPtr<IVsProject>& rspVsProj)
{
	CVCArchy* pArchy = NULL;
	CVCProjectNode* pProjNode = GetProjectNode(pProject);
	if (pProjNode)
		pArchy = pProjNode->GetHierarchy();
	if (pArchy)
		rspVsProj = VCQI_cast<IVsProject>(pArchy);
	else
		rspVsProj = NULL;
}

BOOL CBuildPackage::GetVsProjectAndTracker(VCProject* pProject, CComPtr<IVsProject>& rspVsProj, 
	CComPtr<IVsTrackProjectDocuments>& rspTrackDocs)
{
	GetVsProject(pProject, rspVsProj);

	if (rspVsProj)	// don't care about this service if no project...
		ExternalQueryService(SID_SVsTrackProjectDocuments, IID_IVsTrackProjectDocuments, (void **)&rspTrackDocs);

	return (rspVsProj != NULL && rspTrackDocs != NULL);
}

BOOL CBuildPackage::GetVsProjectAndTracker(VCProject* pProject, CComPtr<IVsProject>& rspVsProj, 
	CComPtr<IVsTrackProjectDocuments2>& rspTrackDocs)
{
	GetVsProject(pProject, rspVsProj);

	if (rspVsProj)	// don't care about this service if no project...
		ExternalQueryService(SID_SVsTrackProjectDocuments, IID_IVsTrackProjectDocuments2, (void **)&rspTrackDocs);

	return (rspVsProj != NULL && rspTrackDocs != NULL);
}

STDMETHODIMP CBuildPackage::AllowedToAddFile(VCProject* pProject, LPCOLESTR szFile, VARIANT_BOOL bSetErrorInfo)
{
	if ( !FIsIDEInteractive() )
		return S_OK;

	CComPtr<IVsProject> spVsProj;
	CComPtr<IVsTrackProjectDocuments2> spTrackDocs;
	if (!GetVsProjectAndTracker(pProject, spVsProj, spTrackDocs))
		return S_OK;		// don't really care if we could do this or not...

	const int cFiles = 1;
	VSQUERYADDFILEFLAGS rgFlags[cFiles] = {VSQUERYADDFILEFLAGS_NoFlags};
	VSQUERYADDFILERESULTS qafSummary = VSQUERYADDFILERESULTS_AddOK;
	VSQUERYADDFILERESULTS rgQAFResults[cFiles];

	HRESULT hr = spTrackDocs->OnQueryAddFiles(spVsProj, 1, &szFile, rgFlags, &qafSummary, rgQAFResults);
	RETURN_ON_FAIL(hr);

	if (VSQUERYADDFILERESULTS_AddOK == qafSummary)
		return S_OK;
	
	return DoSetErrorInfo2(E_ACCESSDENIED, IDS_ERR_CANNOT_ADD_FILE, szFile, (bSetErrorInfo == VARIANT_TRUE));
}

STDMETHODIMP CBuildPackage::AllowedToRemoveFile(VCProject* pProject, LPCOLESTR szFile)
{
	if ( !FIsIDEInteractive() )
		return S_OK;

	CComPtr<IVsProject> spVsProj;
	CComPtr<IVsTrackProjectDocuments2> spTrackDocs;
	if (!GetVsProjectAndTracker(pProject, spVsProj, spTrackDocs))
		return S_OK;		// don't really care if we could do this or not...

	const int cFiles = 1;
	VSQUERYREMOVEFILEFLAGS rgFlags[cFiles] = {VSQUERYREMOVEFILEFLAGS_NoFlags};
	VSQUERYREMOVEFILERESULTS qrfSummary = VSQUERYREMOVEFILERESULTS_RemoveOK;
	VSQUERYREMOVEFILERESULTS rgQRFResults[cFiles];

	HRESULT hr = spTrackDocs->OnQueryRemoveFiles(spVsProj, cFiles, &szFile, rgFlags, &qrfSummary, rgQRFResults);
	RETURN_ON_FAIL(hr);

	return (VSQUERYREMOVEFILERESULTS_RemoveOK == qrfSummary) ? S_OK : E_ACCESSDENIED;
}

STDMETHODIMP CBuildPackage::InformFileAdded(VCProject* pProject, LPCOLESTR szFile)
{
	if ( !FIsIDEInteractive() )
		return S_OK;

	CComPtr<IVsProject> spVsProj;
	CComPtr<IVsTrackProjectDocuments> spTrackDocs;
	if (!GetVsProjectAndTracker(pProject, spVsProj, spTrackDocs))
		return S_OK;		// don't really care if we could do this or not...

	return spTrackDocs->OnAfterAddFiles(spVsProj, 1, &szFile);
}

STDMETHODIMP CBuildPackage::InformFileRemoved(VCProject* pProject, LPCOLESTR szFile)
{
	if ( !FIsIDEInteractive() )
		return S_OK;

	CComPtr<IVsProject> spVsProj;
	CComPtr<IVsTrackProjectDocuments> spTrackDocs;
	if (!GetVsProjectAndTracker(pProject, spVsProj, spTrackDocs))
		return S_OK;		// don't really care if we could do this or not...

	VSREMOVEFILEFLAGS rgFlags[1] = {VSREMOVEFILEFLAGS_NoFlags};

	return spTrackDocs->OnAfterRemoveFiles(spVsProj, 1, &szFile, rgFlags);
}

HRESULT CBuildPackage::GetDebugCommandLines(VCDebugSettings* pDbgSettings, VCConfiguration* pConfig)
{
	CHECK_READ_POINTER_NULL(pDbgSettings);

	// get the parent for the dlg
	HWND hwndShell;
	CComPtr<IVsUIShell> pUIShell;
	HRESULT hr = ExternalQueryService( SID_SVsUIShell, IID_IVsUIShell, reinterpret_cast<void **>(&pUIShell) );
	RETURN_ON_FAIL_OR_NULL2(hr, pUIShell, E_NOINTERFACE);
	hr = pUIShell->GetDialogOwnerHwnd( &hwndShell );
	RETURN_ON_FAIL(hr);

	CCCallExeForDebug dbgDlg( pConfig, hwndShell );
	CComBSTR bstrUrl, bstrExe;
	pDbgSettings->get_HttpUrl(&bstrUrl);
	pDbgSettings->get_Command(&bstrExe);
	dbgDlg.m_bstrUrl = bstrUrl;
	dbgDlg.m_bstrExe = bstrExe;

	hr = pUIShell->EnableModeless(false);
	RETURN_ON_FAIL(hr);

	if (dbgDlg.DoModalW() == IDOK)
	{
		CComBSTR bstrHttpUrl;
		CComBSTR bstrExeToDebug;
		bstrHttpUrl = dbgDlg.m_bstrUrl;

		bstrExeToDebug = dbgDlg.m_bstrExe;
		if (bstrHttpUrl.Length() > 0)
			pDbgSettings->put_HttpUrl(bstrHttpUrl);
		if (bstrExeToDebug.Length() > 0)
			pDbgSettings->put_Command(bstrExeToDebug);
		hr = S_OK;
	}
	else
	{
		hr = E_ABORT;	// user canceled
	}

	/*hr =*/ pUIShell->EnableModeless(true); //don't want to mask E_ABORT

    return hr;
}

HRESULT CBuildPackage::GetBuildErrorContext(IVCBuildErrorContext** ppIVCBuildErrorContext)
{
	CHECK_POINTER_NULL(ppIVCBuildErrorContext);

	CComPtr<IVsOutputWindowPane> spOutputWindowPane;
	if (SUCCEEDED(CGenCfg::GetOutputWindowPane(&spOutputWindowPane)) && spOutputWindowPane != NULL)
		spOutputWindowPane->Activate();

	CComPtr<IVCBuildOutput> spModelessOutputWindow;
	HRESULT hr = CVCBuildOutput::CreateInstance(&spModelessOutputWindow, spOutputWindowPane);
	RETURN_ON_FAIL_OR_NULL(hr, spModelessOutputWindow);

	CComPtr<IVCBuildEngine> spBldEngine;
	GetBuildEngine(&spBldEngine);
	CComPtr<IVCBuildErrorContext> spErrorContext;

	hr = CBldOutputWinEC::CreateInstance(&spErrorContext, NULL, spBldEngine, NULL, spModelessOutputWindow);
	VSASSERT(SUCCEEDED(hr), "Failed to create error context!");
	*ppIVCBuildErrorContext = spErrorContext.Detach();
	return hr;
}

////////////////////////////////////////////////////////////
// CBldPackage::OnExit

void CBuildPackage::OnExit()
{
}

BOOL CBuildPackage::OnInit()
{

	CComQIPtr<IVCCollection>	spCollection;
	CComQIPtr<VCPlatform>		spPlatform;
	CComPtr<IEnumVARIANT>		spEnum;
	CComPtr<IDispatch>			spDisp;
	HRESULT hr;

	BOOL bUseEnv = FALSE;
	// Now that we are loaded, check if we were loaded as part of a /debug switch
	CComPtr<IVsAppCommandLine> pCmdLine;
	if ((SUCCEEDED(ExternalQueryService(SID_SVsAppCommandLine, __uuidof(IVsAppCommandLine), (void **)&pCmdLine))) && (pCmdLine != NULL))
	{
		pCmdLine->GetOption(L"UseEnv",&bUseEnv,&bstrExeArguments);
	}

	BOOL bWrite = FALSE;

	// For each platform initalize the paths
	hr = GetBuildPkg()->m_pProjectEngine->get_Platforms(&spDisp);
	RETURN_ON_FAIL2(hr, FALSE);
	spCollection = spDisp;
	RETURN_ON_NULL2(spCollection, FALSE);
	hr = spCollection->_NewEnum(reinterpret_cast<IUnknown **>(&spEnum));
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spEnum, FALSE);
	spEnum->Reset();
	while ( TRUE )
	{
		CComVariant var;
		hr = spEnum->Next(1, &var, NULL);
		if (hr != S_OK)
			break;
		
		spPlatform = var.punkVal;
		if (!spPlatform)
			return FALSE;

		CStringW strEntry = L"VC\\VC_OBJECTS_PLATFORM_INFO\\";
		CComBSTR bstrName;
		hr = spPlatform->get_Name(&bstrName);
		strEntry += bstrName;
		strEntry += L"\\Directories";
		
		CStringW strPath;

		CComBSTR bstrPath;
		CComBSTR bstrInc;
		CComBSTR bstrAsm;
		CComBSTR bstrLib;
		CComBSTR bstrSrc;

		if( bUseEnv == FALSE )
			GetRegPath(strEntry, L"Path Dirs", strPath, L"");

		if (strPath == L"") 
		{	
			::VCGetEnvironmentVariableW(L"Path", &bstrPath);
			bWrite = TRUE;
		}
		else
		{
			bstrPath = strPath;
		}
		spPlatform->put_ExecutableDirectories(bstrPath);

		strPath = L"";
		if( bUseEnv == FALSE )
			GetRegPath(strEntry, L"Include Dirs", strPath, L"");
		if (strPath == L"") 
		{
			::VCGetEnvironmentVariableW(L"Include", &bstrInc);
			bWrite = TRUE;
		}
		else
		{
			bstrInc = strPath;
		}
		spPlatform->put_IncludeDirectories(bstrInc);
		
		strPath = L"";
		if( bUseEnv == FALSE )
			GetRegPath(strEntry, L"Reference Dirs", strPath, L"");
		if (strPath == "") 
		{
			::VCGetEnvironmentVariableW(L"LIBPATH", &bstrAsm);
			bWrite = TRUE;
		}	
		else
		{
			bstrAsm = strPath;
		}
		spPlatform->put_ReferenceDirectories(bstrAsm);

		strPath = L"";
		if( bUseEnv == FALSE )
			GetRegPath(strEntry, L"Library Dirs", strPath, L"");
		if (strPath == "") 
		{
			::VCGetEnvironmentVariableW(L"Lib", &bstrLib);
			bWrite = TRUE;
		}	
		else
		{
			bstrLib = strPath;
		}
		spPlatform->put_LibraryDirectories(bstrLib);

		strPath = L"";
		if( bUseEnv == FALSE )
			GetRegPath(strEntry, L"Source Dirs", strPath, L"");
		if (strPath == "") 
		{
			::VCGetEnvironmentVariableW(L"Source", &bstrSrc);
			bWrite = TRUE;
		}	
		else
		{
			bstrSrc = strPath;
		}
		spPlatform->put_SourceDirectories(bstrSrc);
	}
	// if bWrite, write the file
	if( bUseEnv == FALSE )
	{
		if( bWrite )
			CDirectoriesDlg::WritePathsToRegistry();
	}
		
	return TRUE;
}

// IVsUpdateSolutionEvents
STDMETHODIMP CBuildPackage::OnActiveProjectCfgChange(IVsHierarchy* pIVsHierarchy)
{
	RETURN_ON_NULL2(m_pProjectEngine, S_FALSE);
	RETURN_ON_NULL2(pIVsHierarchy, S_FALSE);	

	CComQIPtr<IVsUpdateSolutionEvents> spEvents = pIVsHierarchy;
	if( spEvents )
		spEvents->OnActiveProjectCfgChange(NULL);

	return S_OK;
}

// IVsSolutionEvents
STDMETHODIMP CBuildPackage::OnAfterOpenProject( IVsHierarchy *pHierarchy, BOOL fAdded)
{
	// check to see if we need to set project dependencies here...
	// (for VC 4.x projects with sub-projects)
	
	// if the root project name is null
	if( !g_wszRootProjectName )
		return S_OK;

	CHECK_READ_POINTER_NULL(pHierarchy);

	// get the name of the project from the hierarchy
	CComVariant varName;
	pHierarchy->GetProperty( VSITEMID_ROOT, VSHPROPID_Caption, &varName );
	if( varName.vt != VT_BSTR )
	{
		// free the root string and set it back to null
		delete[] g_wszRootProjectName;
		g_wszRootProjectName = NULL;
		return S_OK;
	}
	// is it the same as the root? 
	if( _wcsicmp( g_wszRootProjectName, varName.bstrVal ) == 0 )
	{
		// get the IVCProjConvert pointer
		CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = m_pProjectEngine;
		VSASSERT( pProjEngImpl, "Project Engine is in an invalid state: could not be QI'd for IVCProjectEngineImpl!" );
		CComQIPtr<IVCProjConvert> pProjConvert;
		CComPtr<IUnknown> pUnkProjConvert;
		pProjEngImpl->GetIVCProjConvert( &pUnkProjConvert );
		if( !pUnkProjConvert )
		{
			// free the root string and set it back to null
			delete[] g_wszRootProjectName;
			g_wszRootProjectName = NULL;
			return S_OK;
		}
		pProjConvert = pUnkProjConvert;
		RETURN_ON_NULL(pProjConvert);

		// if there are no dependencies, bail
		VARIANT_BOOL bHasDeps;
		pProjConvert->get_HasProjectDependencies( &bHasDeps );
		if( bHasDeps == VARIANT_FALSE )
		{
			// free the root string and set it back to null
			delete[] g_wszRootProjectName;
			g_wszRootProjectName = NULL;
			return S_OK;
		}

		// get the dte object
		CComQIPtr<_DTE> pDTE;
		HRESULT hr = ExternalQueryService(SID_SDTE, __uuidof(_DTE), (void **)&pDTE);
		VSASSERT( SUCCEEDED( hr ), "Unable to get DTE!" );
		RETURN_ON_FAIL_OR_NULL(hr, pDTE);
		// get the Solution object
		CComPtr<_Solution> pSln;
		hr = pDTE->get_Solution( (Solution**)&pSln );
		VSASSERT( SUCCEEDED( hr ) && pSln, "Unable to get the solution from the DTE!" );
		RETURN_ON_FAIL_OR_NULL(hr, pSln);

		// get the solution service
		CComPtr<IVsSolution> pIVsSln;
		hr = GetIVsSolution( &pIVsSln );
		VSASSERT( SUCCEEDED( hr ) && pIVsSln, "Unable to get the solution service!" );
		RETURN_ON_FAIL_OR_NULL(hr, pIVsSln);

		// set the dependencies:
		
		// iterate through the dependencies
		pProjConvert->ResetProjectDependencyEnum();
		long lNumDeps;
		pProjConvert->get_ProjectDependenciesCount( &lNumDeps );
		for( long i = 0; i < lNumDeps; i++ )
		{
			// get the project and target strings
			CComBSTR bstrProject, bstrTarget;
			pProjConvert->GetNextProjectDependency( &bstrProject, &bstrTarget );

			// find the shell projects that match bstrProject and bstrTarget

			// get the hierarchies enumeration
			CComPtr<IEnumHierarchies> pHierEnum;
			hr = pIVsSln->GetProjectEnum( EPF_ALL, IID_IVCArchy, &pHierEnum );
			if( FAILED( hr ) || !pHierEnum )
				break;
			// for each hierarchy
			CComBSTR bstrProjUniqueName, bstrTargUniqueName;
			pHierEnum->Reset();
			while ( true )
			{
				CComQIPtr<IVsHierarchy> pHierarchy;

				hr = pHierEnum->Next(1, &pHierarchy, NULL);
				if( hr != S_OK )
					break;

				if (pHierarchy == NULL)
					continue;

				// GetProp its VSHPROPID_ExtObject to get the shell project
				CComVariant var;
				CComQIPtr<Project> pProj;
				hr = pHierarchy->GetProperty( VSITEMID_ROOT, VSHPROPID_ExtObject, &var );
				pProj = var.pdispVal;
				if (pProj == NULL)
					continue;

				// get its name
				CComBSTR bstrName;
				hr = pProj->get_Name( &bstrName );
				// compare the name to bstrProject
				if( bstrName && _wcsicmp( bstrName, bstrProject ) == 0 )
				{
					pProj->get_UniqueName( &bstrProjUniqueName );
					continue;
				}
				// compare the name to bstrTarget
				if( bstrName && _wcsicmp( bstrName, bstrTarget ) == 0 )
				{
					pProj->get_UniqueName( &bstrTargUniqueName );
					continue;
				}
			}

			// get the SolutionBuild object
			CComPtr<SolutionBuild> pSlnBld;
			hr = pSln->get_SolutionBuild( &pSlnBld );
			if( FAILED( hr ) || !pSlnBld )
				break;
			// get the BuildDependencies object for strProject
			CComPtr<BuildDependencies> pBldDeps;
			hr = pSlnBld->get_BuildDependencies( &pBldDeps );
			if( FAILED( hr ) || !pBldDeps )
				break;
			// get this particular dependency
			CComPtr<BuildDependency> pBldDep;
			hr = pBldDeps->Item( CComVariant( bstrProjUniqueName ), &pBldDep );
			if( FAILED( hr ) || !pBldDep )
				break;
			// call AddProject on it
			hr = pBldDep->AddProject( bstrTargUniqueName );

		}

		// free the root string and set it back to null
		delete[] g_wszRootProjectName;
		g_wszRootProjectName = NULL;
	}
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnQueryCloseProject( IVsHierarchy *pHierarchy, BOOL fRemoving, BOOL *pfCancel)
{
	*pfCancel = FALSE;
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnBeforeCloseProject( IVsHierarchy *pHierarchy, BOOL fRemoved)
{
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnAfterLoadProject( IVsHierarchy *pStubHierarchy, IVsHierarchy *pRealHierarchy)
{
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnQueryUnloadProject( IVsHierarchy *pRealHierarchy, BOOL *pfCancel)
{
	*pfCancel = FALSE;
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnBeforeUnloadProject( IVsHierarchy *pRealHierarchy, IVsHierarchy *pStubHierarchy)
{
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnAfterOpenSolution( IUnknown *pUnkReserved, BOOL fNewSolution)
{
	UpdateSolutionFile();
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnQueryCloseSolution( IUnknown *pUnkReserved, BOOL *pfCancel)
{
	*pfCancel = FALSE;
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnBeforeCloseSolution( IUnknown *pUnkReserved)
{
	CConvertDlg::s_bConvertAll = CConvertDlg::s_bConvertNone = FALSE;
	return S_OK;
}

STDMETHODIMP CBuildPackage::OnAfterCloseSolution( IUnknown *pUnkReserved)
{
	m_bstrSolutionFile.Empty();
	return S_OK;
}

HRESULT CBuildPackage::DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bSetInfo)
{
	if (bSetInfo)
	{
		CComBSTR bstrErr;
		bstrErr.LoadString(idErr);
		HandleSetErrorInfo(hrErr, bstrErr);
	}
	return hrErr;
}

HRESULT CBuildPackage::DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bSetInfo)
{
	if (bSetInfo)
	{
		CStringW strErr;
		strErr.Format(idErr, szParam);
		CComBSTR bstrErr = strErr;
		HandleSetErrorInfo(hrErr, bstrErr);
	}
	return hrErr;
}

void CBuildPackage::SetUseGlobalEditorForResourceFiles(VARIANT_BOOL bUseGlobal)
{
	CStringW strRoot = s_bstrAltKey + "\\VC";
	m_bUseGlobalEditorForRC = bUseGlobal;
	DWORD dwValue = WriteRegIntW(strRoot, USE_GLOBAL_RC_EDITOR, DWORD(m_bUseGlobalEditorForRC));
}

BOOL CBuildPackage::GetSccManager(IVsSccManager2** ppManager)
{
	CHECK_POINTER_NULL(ppManager);
	*ppManager = NULL;
	if (m_spVsSccManager2 == NULL)
		ExternalQueryService(SID_SVsSccManager, IID_IVsSccManager2, (void **)&m_spVsSccManager2);
	m_spVsSccManager2.CopyTo(ppManager);
	return (m_spVsSccManager2 != NULL);
}

HRESULT CBuildPackage::UpdateSolutionFile()
{
	m_bstrSolutionFile.Empty();
	CComPtr<IVsSolution> spSoln;
	HRESULT hr = GetIVsSolution(&spSoln);
	RETURN_ON_FAIL_OR_NULL(hr, spSoln);
	CComBSTR bstrSolnDir, bstrUserOpts;
	hr = spSoln->GetSolutionInfo(&bstrSolnDir, &m_bstrSolutionFile, &bstrUserOpts);
	VSASSERT(SUCCEEDED(hr), "Failed to get solution info!");
	return hr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
HRESULT CExeProjectFactory::CanCreateProject( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ DWORD grfCreateProj,
            /* [out] */ BOOL *pfCanCreate)
{
	CHECK_POINTER_NULL(pfCanCreate);

	*pfCanCreate = TRUE;
	return S_OK;
}
        
HRESULT CExeProjectFactory::CreateProject( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled)
{
	CHECK_POINTER_NULL(ppUnk);
	CHECK_POINTER_NULL(pfCanceled);

	*ppUnk = NULL;
	*pfCanceled = FALSE;

	if(grfCreateProj & CPF_CLONEFILE) // New project
	{
		// We don't support this.
		return E_FAIL;
	}

	 // Open existing project
	CComObject<CExeHierarchy> *pExeHierarchy;	// created with Ref count 0
	HRESULT hr = CComObject<CExeHierarchy>::CreateInstance(&pExeHierarchy);
	if( pExeHierarchy )
	{
		pExeHierarchy->Initialize(pszFilename, bstrExeArguments);
		pExeHierarchy->QueryInterface(__uuidof(IVsHierarchy), ppUnk);
		if( bstrExeArguments )
		{
			bstrExeArguments = "";
		}
	}

	return hr;
}

STDMETHODIMP CExeProjectFactory::SetSite( /* [in] */ IServiceProvider *pSP)
{
	return S_OK;
}

STDMETHODIMP CExeProjectFactory::Close(void)
{
	return S_OK;
}

HRESULT CCrashDumpProjectFactory::CreateProject( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled)
{
	CHECK_POINTER_NULL(ppUnk);
	CHECK_POINTER_NULL(pfCanceled);

	*ppUnk = NULL;
	*pfCanceled = FALSE;

	if(grfCreateProj & CPF_CLONEFILE) // New project
	{
		// We don't support this.
		return E_FAIL;
	}

	 // Open existing project
	CComObject<CCrashDumpHierarchy> *pCrashDumpHierarchy;	// created with Ref count 0
	HRESULT hr = CComObject<CCrashDumpHierarchy>::CreateInstance(&pCrashDumpHierarchy);
	if( pCrashDumpHierarchy )
	{
		pCrashDumpHierarchy->Initialize(pszFilename, bstrExeArguments);
		pCrashDumpHierarchy->QueryInterface(__uuidof(IVsHierarchy), ppUnk);
		if( bstrExeArguments )
		{
			bstrExeArguments = "";
		}
	}

	return hr;
}

#ifdef AARDVARK
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
HRESULT CStyleSheetProjectFactory::CanCreateProject( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ DWORD grfCreateProj,
            /* [out] */ BOOL *pfCanCreate)
{
	CHECK_POINTER_NULL(pfCanCreate);

	*pfCanCreate = TRUE;
	return S_OK;
}
        
HRESULT CStyleSheetProjectFactory::CreateProject( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled)
{
	// helper object to track "in project load" status
	CInProjectLoadHelper inProjLoad;
	HRESULT hr = E_FAIL;

	CComPtr<IDispatch> pDispStyleSheet;

	CDirW dirTo;
	CDirW dirFrom;

	if (ppUnk != NULL && pfCanceled != NULL)
	{
		*ppUnk = NULL;
		*pfCanceled = FALSE;
		CComBSTR bstrFileName = pszFilename;

		// WARNING ! Although this looks like it is doing nothing, it actually initializes the Wizards package so 
		// that someone SetSite's them.  It also needs to go BEFORE the first project gets loaded or our conversion
		// DLL can't get CoCreate'd.  Go figure.
		// DO NOT REMOVE without asking DaveWa
		if( FIsIDEInteractive() )
		{
			CComPtr<IVsPackage> pUnk;
			hr = ExternalQueryService(SID_SVCService, __uuidof(IVsPackage), (void**)&pUnk);
		}
		// END DO NOT REMOVE

		CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = GetBuildPkg()->m_pProjectEngine;
		if (spProjEngineImpl == NULL)
		{
			*pfCanceled = TRUE;
			return E_FAIL;
		}

		hr = spProjEngineImpl->LoadProjectUnderShell(bstrFileName, &pDispStyleSheet);
		// if we can't load the project file, bail
		if( FAILED( hr ) )
		{
			CStringW strFileName = bstrFileName;
			CStringW strMessage;
			strMessage.Format(IDS_WARN_PRJOPEN_FAILED, strFileName);
			CComBSTR bstrMessage = strMessage;
			ReportError( bstrMessage, hr );

			*pfCanceled = TRUE;
			if (hr != E_FAIL)
				hr = E_ABORT;
			return hr;
		}
		// if we were given a name, we need to set the correct name of the project
		if( pszName )
		{
			CComQIPtr<VCStyleSheet> pStyleSheet = pDispStyleSheet;
			VSASSERT( pStyleSheet, "Failed to get VCStyleSheet object back from LoadStyleSheet!" );
			RETURN_ON_NULL(pStyleSheet);
			// strip the file extension off the name
			CStringW strName = pszName;
			int idx = strName.ReverseFind( L'.' );
			if( idx != -1 )
				strName = strName.Left( idx );
			pStyleSheet->put_Name( CComBSTR( strName ) );
		}
	
		if (SUCCEEDED(hr) && pDispStyleSheet)
		{
			// get the IDispatch of the StyleSheet and add all elements of that StyleSheet to the UI.
			CComQIPtr<IVCExternalCookie> pExCookie = pDispStyleSheet;
			if (pExCookie)
			{
				void* pCookie;
				hr = pExCookie->get_ExternalCookie(&pCookie);
				CStyleSheetHierarchy *pStyleHierarchy = (CStyleSheetHierarchy *)pCookie;
				CComPtr<IVsHierarchy> spHier = VCQI_cast<IVsHierarchy>(pStyleHierarchy);
				*ppUnk = spHier;	// yep, we're now down one ref when we leave here.  that's OK and necessary.
	
				hr = S_OK;
			}
		}
	}

	if( *ppUnk == NULL )
		hr = E_FAIL;
	
	if( FAILED(hr) )
	{
		CComBSTR bstrMessage;
		bstrMessage.LoadString(IDS_WARN_PRJOPEN_FAILED);
		ReportError( bstrMessage, hr );
		if (hr != E_FAIL)
			hr = E_ABORT;

		*pfCanceled = TRUE;
	}

	return hr;
}

STDMETHODIMP CStyleSheetProjectFactory::SetSite( /* [in] */ IServiceProvider *pSP)
{
	return S_OK;
}

STDMETHODIMP CStyleSheetProjectFactory::Close(void)
{
	return S_OK;
}

#include "stylehierarchy.cpp"
#endif	// AARDVARK
