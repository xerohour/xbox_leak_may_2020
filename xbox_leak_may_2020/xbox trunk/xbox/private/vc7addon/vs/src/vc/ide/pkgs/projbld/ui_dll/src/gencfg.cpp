// GenCfg.cpp : Implementation of CDevbldApp and DLL registration.

#include "stdafx.h"
#include "vssln.h"
#include "bldpkg.h"
#include "GenCfg.h"
#include "vcarchy.h"
#include "path2.h"
#include "msdbg.h"
#include "projouts.h"
#include "output.h"
#include "outsenum.h"
#include "outgroup.h"
#include "ErrContext2.h"
#include "utils.h"
#include "vsappid.h"
#include <dpkg.h>

/////////////////////////////////////////////////////////////////////////////
//
CGenCfg::CGenCfg()
{
	m_pBuildHelper = NULL;
}

CGenCfgBuildHelper::CGenCfgBuildHelper()
{
	m_ppFiles = NULL;
	m_celtFiles = 0;
}

CGenCfg::~CGenCfg()
{
	ClearOutputs();
	if (m_pBuildHelper)
		delete m_pBuildHelper;
}

CGenCfgBuildHelper::~CGenCfgBuildHelper() 
{
	for (int i = 0; i < NUMBER_OF(m_rgsrpIVsBuildStatusCallback); i++)
	{
		if (m_rgsrpIVsBuildStatusStreamCallback[i])
		{
			HRESULT hr = AtlFreeMarshalStream(m_rgsrpIVsBuildStatusStreamCallback[i]);
			VSASSERT(SUCCEEDED(hr), "Failed to free marshal a build status callback");
		}
	}
	FreeFiles(FALSE);
}

void CGenCfg::ClearOutputs()
{
	VCPOSITION pos = m_ListOutputs.GetHeadPosition();
	while (pos)
	{
		IVsOutput *pOut = (IVsOutput *)m_ListOutputs.GetNext(pos);
		if (pOut != NULL)
			pOut->Release();
	}
	m_ListOutputs.RemoveAll();
}

HRESULT CGenCfg::CreateInstance(IVsCfg * * ppGenCfg,CVCArchy *pArchy, VCConfiguration *pCfg)
{
	CComObject<CGenCfg> *pGenCfgObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CGenCfg>::CreateInstance(&pGenCfgObj);
	if (SUCCEEDED(hr))
	{
		pGenCfgObj->Initialize(pArchy, pCfg);
		hr = pGenCfgObj->QueryInterface(__uuidof(IVsCfg), (void**)ppGenCfg );
	}
	return hr;

}

void CGenCfg::Initialize(CVCArchy *pArchy, VCConfiguration *pCfg)
{
	m_pConfig = pCfg;
	if (pArchy)
		m_marshalVsProjectCfgProvider.Init((IVsProjectCfgProvider *)pArchy);
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg2
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_CfgType( /* [in] */ REFIID iidCfg, /* [out] */ void **ppCfg )
{
    return QueryInterface(iidCfg, ppCfg); 
}
	
STDMETHODIMP CGenCfg::get_OutputGroups( /* [in] */ ULONG celt, /* [in, out] */ IVsOutputGroup *rgpcfg[],
			/* [out] */ ULONG *pcActual)
{
	if( celt == 0 )
	{
		CHECK_POINTER_NULL(pcActual);
		*pcActual = 4;
		return S_OK;
	}

	if (celt != 1)
		CHECK_POINTER_NULL(pcActual);

	// return as many as they want.
	CComQIPtr<IVsProjectCfg2> pCfg = this;
	HRESULT hr = COutputGroup::CreateInstance(&rgpcfg[PRIMARY_OUTPUT], pCfg, PRIMARY_OUTPUT );
	RETURN_ON_FAIL(hr);
	if (pcActual)
		*pcActual = 1;

	if( celt > 1 )
	{
		hr = COutputGroup::CreateInstance(&rgpcfg[SECONDARY_OUTPUT], pCfg, SECONDARY_OUTPUT );
		RETURN_ON_FAIL(hr);
		*pcActual = 2;
	}
	
	if( celt > 2 )
	{
		COutputGroup::CreateInstance(&rgpcfg[CONTENT_OUTPUT], pCfg, CONTENT_OUTPUT );
		RETURN_ON_FAIL(hr);
		*pcActual = 3;
	}

	if( celt > 3 )
	{
		COutputGroup::CreateInstance(&rgpcfg[SOURCE_OUTPUT], pCfg, SOURCE_OUTPUT );
		RETURN_ON_FAIL(hr);
		*pcActual = 4;
	}

	return S_OK;
}

STDMETHODIMP CGenCfg::OpenOutputGroup(
		/* [in] */ LPCOLESTR szCanonicalName,
		/* [out] */ IVsOutputGroup **ppIVsOutputGroup)
{
	CHECK_POINTER_NULL(ppIVsOutputGroup);

	HRESULT hr = E_NOTIMPL;	// CGenCfg::OpenOutputGroup, anything other than what's specifically supported
	CStringW strCanonicalName = szCanonicalName;
	if (strCanonicalName == L"Built")
	{
		hr = COutputGroup::CreateInstance(ppIVsOutputGroup, this, PRIMARY_OUTPUT);
	}
	else if (strCanonicalName == L"ContentFiles")
	{
		hr = COutputGroup::CreateInstance(ppIVsOutputGroup, this, CONTENT_OUTPUT);
	}
	else if (strCanonicalName == L"Symbols")
	{
		hr = COutputGroup::CreateInstance(ppIVsOutputGroup, this, SECONDARY_OUTPUT);
	}
	else if (strCanonicalName == L"SourceFiles")
	{
		hr = COutputGroup::CreateInstance(ppIVsOutputGroup, this, SOURCE_OUTPUT);
	}

	return hr;
}

STDMETHODIMP CGenCfg::OutputsRequireAppRoot( /* [out] */ BOOL *pfRequiresAppRoot )
{
	CHECK_POINTER_NULL(pfRequiresAppRoot);
	*pfRequiresAppRoot = FALSE;
	return S_OK;
}
		
STDMETHODIMP CGenCfg::get_VirtualRoot( /* [out] */ BSTR *pbstrVRoot )
{
	CHECK_POINTER_NULL(pbstrVRoot);
	return E_NOTIMPL;	// CGenCfg::get_VirtualRoot
}

STDMETHODIMP CGenCfg::get_IsPrivate( /* [out] */ BOOL *pfPrivate )
{
	if( pfPrivate )
		*pfPrivate = FALSE;
	return S_FALSE;
}


//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_ProjectCfgProvider(/* [out] */ IVsProjectCfgProvider **ppIVsProjectCfgProvider)
{
	CHECK_POINTER_NULL(ppIVsProjectCfgProvider);
	return m_marshalVsProjectCfgProvider.UnmarshalPtr(ppIVsProjectCfgProvider);
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_CanonicalName(/* [out] */ BSTR *pbstrCanonicalName)
{
	CHECK_POINTER_NULL(pbstrCanonicalName);
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);
	return m_pConfig->get_Name(pbstrCanonicalName);
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_DisplayName(/* [out] */ BSTR *pbstrDisplayName)
{
	CHECK_POINTER_NULL(pbstrDisplayName);
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);
	return m_pConfig->get_Name(pbstrDisplayName);
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_IsDebugOnly(/* [out] */ BOOL *pfIsDebugOnly)
{
	CHECK_POINTER_NULL(pfIsDebugOnly)
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

    CComBSTR bstrName;
	HRESULT hr = m_pConfig->get_Name(&bstrName);
	RETURN_ON_FAIL(hr);
	if ( !wcsstr(bstrName,L"Debug") )
		*pfIsDebugOnly = TRUE;
	else
		*pfIsDebugOnly = FALSE;
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_IsReleaseOnly(/* [out] */ BOOL *pfIsRetailOnly)
{
	CHECK_POINTER_NULL(pfIsRetailOnly)
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

    CComBSTR bstrName;
	HRESULT hr = m_pConfig->get_Name(&bstrName);
	RETURN_ON_FAIL(hr);

	if ( !wcsstr(bstrName,L"Release") )
		*pfIsRetailOnly = TRUE;
	else
		*pfIsRetailOnly = FALSE;
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_IsRetailOnly(/* [out] */ BOOL *pfIsRetailOnly)
{
	return get_IsReleaseOnly(pfIsRetailOnly);
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_Platform(/* [out] */ GUID *pguidPlatform)
{
	CHECK_POINTER_NULL(pguidPlatform)

	*pguidPlatform = GUID_VS_PLATFORM_WIN32_X86;   // TODO: DRA Could be others too...
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::EnumOutputs(/* [out] */ IVsEnumOutputs **ppIVsEnumOutputs)
{
	CHECK_POINTER_NULL(ppIVsEnumOutputs);
	*ppIVsEnumOutputs = NULL;

	HRESULT hr = InitEnumOuts();
	if (SUCCEEDED(hr))
		hr = COutsEnum::CreateInstance(ppIVsEnumOutputs, m_pConfig, &m_ListOutputs);

	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::OpenOutput(
            /* [in] */ LPCOLESTR szOutputCanonicalName,
            /* [out] */ IVsOutput **ppIVsOutput)
{
	CHECK_POINTER_NULL(ppIVsOutput);
	ATLTRACENOTIMPL(L"CGenCfg::OpenOutput");
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_BuildableProjectCfg(/* [out] */ IVsBuildableProjectCfg **ppIVsBuildableProjectCfg)
{
	CHECK_POINTER_NULL(ppIVsBuildableProjectCfg);

	*ppIVsBuildableProjectCfg = this;
	((IVsBuildableProjectCfg *)this)->AddRef();

	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_IsPackaged(/* [out] */ BOOL *pfIsPackaged)
{
	CHECK_POINTER_NULL(pfIsPackaged);

	*pfIsPackaged = FALSE;
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_IsSpecifyingOutputSupported(/* [out] */ BOOL *pfIsSpecifyingOutputSupported)
{
	CHECK_POINTER_NULL(pfIsSpecifyingOutputSupported);

	*pfIsSpecifyingOutputSupported = FALSE;
	return S_OK;
}

//---------------------------------------------------------------------------
// Unicode is officially code page 1200.  For strict unicode vs. mbcs matching of
// project configurations, you can use TargetCodePage == 1200 as equivalent to
// the project configuration being for Unicode.  Project configurations which
// are generically not Unicode should use CP_ACP.
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_TargetCodePage(/* [out] */ UINT *puiTargetCodePage)
{
	CHECK_POINTER_NULL(puiTargetCodePage);

	*puiTargetCodePage = CP_ACP;
	return S_OK;
}

//---------------------------------------------------------------------------
// The UpdateSequenceNumber property is used to determine how often to reprompt
// the user about whether they would like to rebuild the solution.  The primary
// scenario for this property is stepping in the debugger.  On each step operation
// in the debugger, the solution build manager will check the active project
// configurations, to see if any have updated their USNs.
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_UpdateSequenceNumber(/* [out] */ ULARGE_INTEGER *puliUSN)
{
	CHECK_POINTER_NULL(puliUSN);

	puliUSN->LowPart = 0;
	puliUSN->HighPart = 0;
	return S_OK;
}


//---------------------------------------------------------------------------
// The RootURL property is the root URL for the project outputs.  Essentially,
// if an output's project-relative path is required, it's computed by removing
// the RootURL from its DeploySourceURL.  As is the case for DeploySourceURL
// values, the convention is that paths that are in the local filesystem
// begin with the eight characters: "file:///".
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_RootURL(/* [out] */ BSTR *pbstrRootURL)
{
	CHECK_POINTER_NULL(pbstrRootURL);
	*pbstrRootURL = NULL;

	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	CComPtr<IDispatch> spDispProject;
	m_pConfig->get_Project(&spDispProject);
	CComQIPtr<VCProject> spProject = spDispProject;
	CHECK_ZOMBIE(spProject, IDS_ERR_PROJ_ZOMBIE);

	return spProject->get_ProjectDirectory(pbstrRootURL);
}

//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_ProjectCfg(/* [out] */ IVsProjectCfg __RPC_FAR *__RPC_FAR *ppIVsProjectCfg)
{
	CHECK_POINTER_NULL(ppIVsProjectCfg);

	*ppIVsProjectCfg = static_cast<IVsProjectCfg2*>(this);
	(*ppIVsProjectCfg)->AddRef();
	return S_OK;

}

STDMETHODIMP CGenCfg::AdviseBuildStatusCallback( 
            /* [in] */ IVsBuildStatusCallback __RPC_FAR *pIVsBuildStatusCallback,
            /* [out] */ VSCOOKIE __RPC_FAR *pdwCookie)
{ // stolen from solution\sprojbld.cpp (CVsSimpleProjectBuilder)
	CHECK_POINTER_NULL(pdwCookie);
	RETURN_INVALID_ON_NULL(pIVsBuildStatusCallback);

	if (m_pBuildHelper == NULL)
		m_pBuildHelper = new CGenCfgBuildHelper;
	RETURN_ON_NULL2(m_pBuildHelper, E_OUTOFMEMORY);

	return m_pBuildHelper->AdviseBuildStatusCallback(pIVsBuildStatusCallback, pdwCookie);
}

HRESULT CGenCfgBuildHelper::AdviseBuildStatusCallback( 
            /* [in] */ IVsBuildStatusCallback __RPC_FAR *pIVsBuildStatusCallback,
            /* [out] */ VSCOOKIE __RPC_FAR *pdwCookie)
{ // stolen from solution\sprojbld.cpp (CVsSimpleProjectBuilder)
	ULONG i;

	for (i=0; i<NUMBER_OF(m_rgsrpIVsBuildStatusCallback); i++)
	{
		if (m_rgsrpIVsBuildStatusCallback[i] == NULL)
			break;
	}

	if (i < NUMBER_OF(m_rgsrpIVsBuildStatusCallback))
	{
		HRESULT hr;
		CComPtr<IStream> pIStream;

		*pdwCookie = (i + 1);
		//m_rgsrpIVsBuildStatusCallback[i].p = pIVsBuildStatusCallback; // just set the pointer. Don't addref
		m_rgsrpIVsBuildStatusCallback[i] = pIVsBuildStatusCallback; 
		hr = AtlMarshalPtrInProc((IUnknown *)pIVsBuildStatusCallback, __uuidof (IVsBuildStatusCallback),
			&pIStream);
		VSASSERT(SUCCEEDED(hr), "Failed to marshal IVsBuildStatusCallback pointer!");
		if (SUCCEEDED(hr))
		{
			m_rgsrpIVsBuildStatusStreamCallback[i] = pIStream;

			// We'll tick right now so we can just keep track of whether we want
			// to issue further ticks.
			hr = pIVsBuildStatusCallback->Tick(NULL);
			VSASSERT(SUCCEEDED(hr), "Tick failed!");

			m_rgfTicking[i] = (hr != S_FALSE);
		}

		return NOERROR;
	}

	// We're out of room.
	return CONNECT_E_ADVISELIMIT;
}
        
//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::UnadviseBuildStatusCallback(/* [in] */ VSCOOKIE dwCookie)
{
	RETURN_ON_NULL2(m_pBuildHelper, S_OK);
	return m_pBuildHelper->UnadviseBuildStatusCallback(dwCookie);
}

HRESULT CGenCfgBuildHelper::UnadviseBuildStatusCallback(/* [in] */ VSCOOKIE dwCookie)
{
	HRESULT hr;

	VSASSERT((dwCookie > 0) && (dwCookie <= NUMBER_OF(m_rgsrpIVsBuildStatusCallback)), "Callback cookie out of range");

	if ((dwCookie <= 0) || (dwCookie > NUMBER_OF(m_rgsrpIVsBuildStatusCallback)))
		RETURN_INVALID();

	dwCookie--;

	VSASSERT(m_rgsrpIVsBuildStatusCallback[dwCookie] != NULL, "Bad callback pointer at cookie!");

	if (m_rgsrpIVsBuildStatusStreamCallback[dwCookie])
	{
		IStream* pStream = m_rgsrpIVsBuildStatusStreamCallback[dwCookie];
		pStream->AddRef();	// this lets us release via AtlFreeMarshalStream AND by setting the pointer to NULL later...
		hr = AtlFreeMarshalStream(m_rgsrpIVsBuildStatusStreamCallback[dwCookie]);
		VSASSERT(SUCCEEDED(hr), "Failed to free marshal a callback pointer");
		if (SUCCEEDED(hr))
		{
			m_rgsrpIVsBuildStatusStreamCallback[dwCookie] = NULL;
		}
	}
	if (m_rgsrpIVsBuildStatusCallback[dwCookie])
		m_rgsrpIVsBuildStatusCallback[dwCookie] = NULL;

	return NOERROR;
}
   
//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::StartBuild(IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, DWORD dwOptions)
{
	bldActionTypes bldType = TOB_Build;
	BOOL bLocalStorage = FALSE;
	if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_LOCAL)
		bLocalStorage = TRUE;
	if (dwOptions & VS_BUILDABLEPROJECTCFGOPTS_REBUILD)
		bldType = TOB_ReBuild;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_COMPILE)
		bldType = TOB_Compile;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_LINK)
		bldType = TOB_Link;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_PREBLD)
		bldType = TOB_PreBuildEvent;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_PRELINK)
		bldType = TOB_PreLinkEvent;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_POSTBLD)
		bldType = TOB_PostBuildEvent;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_DEPLOY)
		bldType = TOB_Deploy;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_XBOXIMAGE)
		bldType = TOB_XboxImage;
	else if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_XBOXDEPLOY)
		bldType = TOB_XboxDeploy;

	return DoStartBuild(pIVsOutputWindowPane, bldType, bLocalStorage);
}
        
//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::StartClean( 
				/* [in] */ IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane,
				/* [in] */ DWORD dwOptions)
{
	BOOL bLocalStorage = FALSE;
	if (dwOptions & VC_BUILDABLEPROJECTCFGOPTS_LOCAL)
		bLocalStorage = TRUE;

	return DoStartBuild(pIVsOutputWindowPane, TOB_Clean, bLocalStorage);
}
        
//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::StartUpToDateCheck( 
				/* [in] */ IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane,
				/* [in] */ DWORD dwOptions)
{
	RETURN_ON_NULL2(m_pConfig, S_FALSE);	// nothing we can do in this case...

	VARIANT_BOOL bUpToDate = VARIANT_FALSE;
	m_pConfig->get_UpToDate(&bUpToDate);
	return (bUpToDate == VARIANT_TRUE) ? S_OK : E_FAIL;
}
        
//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::QueryStatus(/* [out] */ BOOL __RPC_FAR *pfBuildDone)
{
	BOOL bDone = !SpawnerActive();
	if (pfBuildDone)
		*pfBuildDone = bDone;

	return (bDone ? S_OK : E_FAIL);
}
        
//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::Stop(/* [in] */ BOOL fSync)
{
	RETURN_ON_NULL2(m_pBuildHelper, S_OK);
	return m_pBuildHelper->Stop(fSync);
}

HRESULT CGenCfgBuildHelper::Stop(BOOL fSync)
{
	CComPtr<IVCBuildEngine> spBldEngine = m_spBuildEngine;	// QI just to make sure it doesn't go away on us in another thread...
	if (spBldEngine != NULL)
		spBldEngine->StopBuild();
	FreeFiles(FALSE);

	return S_OK;
}
        
//---------------------------------------------------------------------------
// interface: IVsBuildableProjectCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::Wait( 
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL fTickWhenMessageQNotEmpty)
{
	VSASSERT(dwMilliseconds == INFINITE, "Don't know how to do non-infinite wait");
	RETURN_ON_NULL2(m_pBuildHelper, S_OK);	// not doing a build

	return m_pBuildHelper->Wait(dwMilliseconds, fTickWhenMessageQNotEmpty);
}

HRESULT CGenCfgBuildHelper::Wait( 
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL fTickWhenMessageQNotEmpty)
{
	CComPtr<IVCBuildEngine> spBldEngine = m_spBuildEngine;	// QI just to make sure it doesn't go away on us in another thread...

	RETURN_ON_NULL2(spBldEngine, S_OK);	// not doing a build if this is NULL

	MSG msg;
	VARIANT_BOOL bSpawnerActive = VARIANT_FALSE;
	spBldEngine->get_SpawnerActive(&bSpawnerActive);
	while((bSpawnerActive == VARIANT_TRUE) && ::GetMessage(&msg, NULL, 0, 0))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		spBldEngine->get_SpawnerActive(&bSpawnerActive);
    }
	return S_OK;
}

STDMETHODIMP CGenCfg::QueryStartBuild( 
    /* [in] */ DWORD dwOptions,
    /* [optional][out] */ BOOL __RPC_FAR *pfSupported,
    /* [optional][out] */ BOOL __RPC_FAR *pfReady)
{
	return QueryCanBuild(pfSupported, pfReady);
}

HRESULT CGenCfg::QueryCanBuild(BOOL __RPC_FAR *pfSupported, BOOL __RPC_FAR *pfReady)
{
	HRESULT hr = SpawnerActive() ? E_FAIL : S_OK;

	if (pfSupported != NULL && !IsBadWritePtr(pfSupported, sizeof(*pfSupported)))
		*pfSupported = TRUE;

	if (pfReady != NULL && !IsBadWritePtr(pfReady, sizeof(*pfReady)))
		*pfReady = SUCCEEDED(hr);

	return hr;
}

BOOL CGenCfg::SpawnerActive()
{
	RETURN_ON_NULL2(m_pBuildHelper, FALSE);
	return m_pBuildHelper->SpawnerActive();
}

BOOL CGenCfgBuildHelper::SpawnerActive()
{
	return (m_spBuildEngine != NULL);
}

STDMETHODIMP CGenCfg::QueryStartClean( 
    /* [in] */ DWORD dwOptions,
    /* [optional][out] */ BOOL __RPC_FAR *pfSupported,
    /* [optional][out] */ BOOL __RPC_FAR *pfReady)
{
	return QueryCanBuild(pfSupported, pfReady);
}



STDMETHODIMP CGenCfg::QueryStartUpToDateCheck( 
    /* [in] */ DWORD dwOptions,
    /* [optional][out] */ BOOL __RPC_FAR *pfSupported,
    /* [optional][out] */ BOOL __RPC_FAR *pfReady)
{
	if (pfSupported && !IsBadWritePtr(pfSupported, sizeof(*pfSupported)))
		*pfSupported = (m_pConfig != NULL); 
	if (pfReady && !IsBadWritePtr(pfReady, sizeof(*pfReady)))
		*pfReady = (m_pConfig != NULL);

	return (m_pConfig != NULL) ? S_OK : E_FAIL;
}



// protected
HRESULT CGenCfg::InitEnumOuts()
{
	VSASSERT(m_pConfig != NULL, "Config is NULL!  Is this GenCfg badly initialized or is it zombie?");
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	ClearOutputs();	// a little inefficient in some ways...

	CComBSTR bstrPrimaryOutput;
	HRESULT hr = m_pConfig->get_PrimaryOutput(&bstrPrimaryOutput);
	RETURN_ON_FAIL(hr);
	if (bstrPrimaryOutput.Length() == 0)
		CHECK_ZOMBIE(NULL, IDS_ERR_CFG_ZOMBIE);
	
	CStringW strPrimaryOutput = bstrPrimaryOutput;
	CPathW path;
	if (!path.CreateFromKnown(strPrimaryOutput))
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CComPtr<IVsOutput> spOut;
	hr = CProjectOutput::CreateInstance(&spOut, m_pConfig, path);
	VSASSERT(SUCCEEDED(hr) && spOut != NULL, "Out of memory");
	if (spOut)
	{
		IVsOutput* pProjOut = spOut.Detach();
		m_ListOutputs.AddTail((LPVOID)pProjOut);
	}

	CComBSTR bstrImportLib;
	HRESULT hr2 = m_pConfig->get_ImportLibrary(&bstrImportLib);
	if (hr2 == S_OK)
	{
		CStringW strImportLib = bstrImportLib;
		CPathW pathLib;
		if (!pathLib.Create(strImportLib))
			return VCPROJ_E_BAD_PATH;

		CComPtr<IVsOutput> spOut2;
		hr = CProjectOutput::CreateInstance(&spOut2, m_pConfig, pathLib);
		VSASSERT(SUCCEEDED(hr) && spOut2 != NULL, "Out of memory");
		if (spOut2)
		{
			IVsOutput* pProjOut = spOut2.Detach();
			m_ListOutputs.AddTail((LPVOID)pProjOut);
		}
	}
	return hr;
}

void CGenCfg::SetOutputPath(BSTR bstrOut, CPathW& rpathPrimary, CPathW& rpathPdb)
{
	CStringW strOut = bstrOut;
	CPathW pathOut;
	BOOL bOK = pathOut.Create(strOut);
	VSASSERT(bOK, "Failed to create output path!");
	CStringW strExt = pathOut.GetExtension();
	strExt.MakeLower();
	if (strExt == L".pdb")
		rpathPdb = pathOut;
	else
		rpathPrimary = pathOut;
}

STDMETHODIMP CGenCfg::InformBuildEnd(BOOL fOK)
{
	if (m_pBuildHelper)
		m_pBuildHelper->InformBuildEnd(fOK);

	return S_OK;	// return code not checked
}

void CGenCfgBuildHelper::InformBuildEnd(BOOL fOK)
{
	// handle EVERYTHING we can before we tell the solution we're done
	m_spBuildEngine.Release();
	if (m_spBuildThread)
	{
		m_spBuildThread->ExitThread();
		m_spBuildThread.Release();
	}

	GetBuildPkg()->DecrementSolutionBuildCount();

	// Inform the solution builder that the build is done
	// we're in the main thread now, so no need to marshal anything
	for (int i = 0; m_rgsrpIVsBuildStatusCallback[i]; i++)
	{
		m_rgsrpIVsBuildStatusCallback[i]->BuildEnd(fOK);
	}

	FreeFiles(FALSE);
}

STDMETHODIMP CGenCfg::InformBuildBegin()
{
	if (m_pBuildHelper)
		return m_pBuildHelper->InformBuildBegin();

	return S_OK;
}

HRESULT CGenCfgBuildHelper::InformBuildBegin()
{
	BOOL fContinue = TRUE;
	HRESULT hr;

	// Inform the solution builder that the build is starting
	for (int i = 0; m_rgsrpIVsBuildStatusCallback[i]; i++)
	{
		//m_rgsrpIVsBuildStatusCallback[i]->BuildBegin(&fContinue);
		if (m_rgsrpIVsBuildStatusStreamCallback[i])
		{
			IVsBuildStatusCallback *pIVsBuildStatusCallback;
			hr = AtlUnmarshalPtr(m_rgsrpIVsBuildStatusStreamCallback[i],
				__uuidof (IVsBuildStatusCallback), (IUnknown **)&pIVsBuildStatusCallback);
			VSASSERT(SUCCEEDED(hr), "Failed to unmarshal build status callback pointer");
			if (SUCCEEDED(hr) && pIVsBuildStatusCallback)
			{
				pIVsBuildStatusCallback->BuildBegin(&fContinue);
			}
		}
	}

	if (fContinue)
		GetBuildPkg()->IncrementSolutionBuildCount();
	return (fContinue ? S_OK : E_FAIL);
}

HRESULT CGenCfg::GetOutputWindowPane(IVsOutputWindowPane** ppOutputWindowPane)
{
	CHECK_POINTER_NULL(ppOutputWindowPane);

	CComPtr<IVsOutputWindow> spOutputWindow;
	HRESULT hr = ExternalQueryService(SID_SVsOutputWindow, __uuidof(IVsOutputWindow), (void **)&spOutputWindow);
	RETURN_ON_FAIL_OR_NULL(hr, spOutputWindow);

	hr = spOutputWindow->GetPane(GUID_BuildOutputWindowPane, ppOutputWindowPane);
	RETURN_ON_FAIL_OR_NULL(hr, *ppOutputWindowPane);

	return S_OK;
}

STDMETHODIMP CGenCfg::StartAutomationBuild(bldActionTypes bldType)
{
	HRESULT hr = QueryCanBuild(NULL, NULL);	// returns S_OK if no build currently in progress
	RETURN_ON_FAIL(hr);

	DWORD dwFlags = VC_BUILDABLEPROJECTCFGOPTS_LOCAL;
	VSSOLNBUILDUPDATEFLAGS solnFlags = SBF_OPERATION_BUILD;

	switch (bldType)
	{
	case TOB_Build:
		break;	// everything set up correctly already
	case TOB_ReBuild:
		dwFlags |= VS_BUILDABLEPROJECTCFGOPTS_REBUILD;
		break;
	case TOB_Clean:
		solnFlags = SBF_OPERATION_CLEAN;
		break;
	case TOB_Link:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_LINK;
		break;
	case TOB_PreBuildEvent:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_PREBLD;
		break;
	case TOB_PreLinkEvent:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_PRELINK;
		break;
	case TOB_PostBuildEvent:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_POSTBLD;
		break;
	case TOB_Deploy:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_DEPLOY;
		break;
	case TOB_XboxImage:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_XBOXIMAGE;
		break;
	case TOB_XboxDeploy:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_XBOXDEPLOY;
		break;
	case TOB_Compile:
		dwFlags |= VC_BUILDABLEPROJECTCFGOPTS_COMPILE;
		break;
	default:
		RETURN_INVALID();
	}

	return StartSolutionBuild(dwFlags, solnFlags);
}

STDMETHODIMP CGenCfg::StartFileBuild(ULONG celtFiles, VCFile* pFiles[])
{
	HRESULT hr = QueryCanBuild(NULL, NULL);	// returns S_OK if no build currently in progress
	RETURN_ON_FAIL(hr);

	if (m_pBuildHelper == NULL)
		m_pBuildHelper = new CGenCfgBuildHelper;
	RETURN_ON_NULL2(m_pBuildHelper, E_OUTOFMEMORY);

	hr = m_pBuildHelper->SetLocalStorage(celtFiles, pFiles);
	RETURN_ON_FAIL(hr);

	hr = StartSolutionBuild(VC_BUILDABLEPROJECTCFGOPTS_LOCAL | VC_BUILDABLEPROJECTCFGOPTS_COMPILE, SBF_OPERATION_BUILD);
	if (FAILED(hr) && m_pBuildHelper)
		m_pBuildHelper->FreeFiles(FALSE);
	return hr;
}

HRESULT CGenCfgBuildHelper::SetLocalStorage(ULONG celtFiles, VCFile* pFiles[])
{
	FreeFiles(TRUE);
	if (celtFiles == 0)
		return S_FALSE;

	m_ppFiles = (VCFile**) ::CoTaskMemAlloc(celtFiles * sizeof(VCFile *));
	memset(m_ppFiles, 0, sizeof(VCFile*)*celtFiles);
	RETURN_ON_NULL2(m_ppFiles, E_OUTOFMEMORY);

	m_celtFiles = celtFiles;
	for (ULONG idx = 0; idx < m_celtFiles; idx++)
	{
		VCFile* pFile = pFiles[idx];
		if (pFile)
			pFile->AddRef();
		m_ppFiles[idx] = pFile;
	}
	return S_OK;
}

void CGenCfgBuildHelper::FreeFiles(BOOL bExpectFresh)
{
	if (bExpectFresh)
		VSASSERT(m_ppFiles == NULL, "Hey, trying to reuse the files list before it's ready!");

	if (m_ppFiles == NULL && m_celtFiles == 0)
		return;

	for (ULONG idx = 0; idx < m_celtFiles; idx++)
	{
		VCFile* pFile = m_ppFiles[idx];
		if (pFile)
			pFile->Release();
	}
	::CoTaskMemFree(m_ppFiles);
	m_ppFiles = NULL;
	m_celtFiles = 0;
}
 
HRESULT CGenCfg::StartSolutionBuild(DWORD dwFlags, VSSOLNBUILDUPDATEFLAGS solnFlags)
{
	CComPtr<IVsProjectCfgProvider> spCfgProvider;
	HRESULT hr = m_marshalVsProjectCfgProvider.GetPtr(&spCfgProvider);
	CComQIPtr<IVsHierarchy> spHier = spCfgProvider;
	RETURN_ON_FAIL_OR_NULL2(hr, spHier, E_UNEXPECTED);

	CComPtr<IVsSolutionBuildManager> spBldMgr;
	hr = GetBuildPkg()->GetSolutionBuildManager(&spBldMgr, TRUE);
	CComQIPtr<IVsSolutionBuildManager2> spBldMgr2 = spBldMgr;
	VSASSERT(spBldMgr2, "Hey, why no IVsSolutionBuildManager2 for IVsSolutionBuildManager?!?");
	RETURN_ON_FAIL_OR_NULL2(hr, spBldMgr2, E_UNEXPECTED);

	IVsHierarchy* pHierTmp = spHier;	// yes, no refcount
	IVsCfg* pVsCfg = (IVsCfg*)(IVsProjectCfg2*)this;		// yes, no refcount
	return spBldMgr2->StartUpdateSpecificProjectConfigurations(1, &pHierTmp, &pVsCfg, 
		solnFlags & SBF_OPERATION_CLEAN ? &dwFlags : NULL, solnFlags & SBF_OPERATION_CLEAN ? NULL : &dwFlags,
		NULL /* deploy flags */, solnFlags, !(FIsIDEInteractive()));
}

STDMETHODIMP CGenCfg::SupportsBuildType(bldActionTypes buildType, VARIANT_BOOL* pbSupports, VARIANT_BOOL* pbAvailable)
{
	CHECK_POINTER_NULL(pbSupports);
	CHECK_POINTER_NULL(pbAvailable);
	*pbSupports = VARIANT_FALSE;
	*pbAvailable = VARIANT_FALSE;

	CComQIPtr<IVCConfigurationImpl> spCfgImpl = m_pConfig;
	CHECK_ZOMBIE(spCfgImpl, IDS_ERR_CFG_ZOMBIE);

	return spCfgImpl->SupportsBuildType(buildType, pbSupports, pbAvailable);
}

STDMETHODIMP CGenCfg::get_NoBuildIsInProgress(BOOL* pbNoneInProgress)
{
	return QueryCanBuild(NULL, pbNoneInProgress);
}

// IVsDebuggableProjectCfg
STDMETHODIMP CGenCfg::DebugLaunch(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch)
{
	CComPtr<IVsDebugger> pVsDebugger;
	HRESULT hr = ExternalQueryService(IID_IVsDebugger, IID_IVsDebugger, (void **)&pVsDebugger);
	RETURN_ON_FAIL2(hr, S_OK);
	RETURN_ON_NULL2(pVsDebugger, S_OK);

	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);
	CComPtr<IDispatch> pDispDbgSettings;
	m_pConfig->get_DebugSettings(&pDispDbgSettings);

	CComQIPtr<IVCDebugSettingsImpl> pDbgSettings = pDispDbgSettings;
	VSASSERT(pDbgSettings, "Failed to get debug settings object");
	RETURN_ON_NULL(pDbgSettings);

	VsDebugTargetInfo  dbgi[2];
	DWORD dwNumTargets = 2;
	ZeroMemory( dbgi, 2 * sizeof(VsDebugTargetInfo) );
	hr = pDbgSettings->CanGetDebugTargetInfo(NULL);
	if (hr != S_OK)
	{
		CComQIPtr<VCDebugSettings> pDbgSettingsPublic = pDbgSettings;
		hr = GetBuildPkg()->GetDebugCommandLines(pDbgSettingsPublic, m_pConfig);
	}
	if (hr == S_OK)
		hr = pDbgSettings->GetDebugTargetInfo( grfLaunch, dbgi, &dwNumTargets );
	if (hr == S_OK)
	{
		UpdateDebuggingInfoForDeployment(dbgi);
		
		hr = pVsDebugger->LaunchDebugTargets(dwNumTargets, dbgi);

		// free up the structures
		for (DWORD i = 0; i < dwNumTargets; i++)
		{
			SysFreeString(dbgi[i].bstrRemoteMachine);
			SysFreeString(dbgi[i].bstrMdmRegisteredName);
			SysFreeString(dbgi[i].bstrExe);
			SysFreeString(dbgi[i].bstrArg);
			SysFreeString(dbgi[i].bstrCurDir);
			SysFreeString(dbgi[i].bstrEnv);
			SysFreeString(dbgi[i].bstrOptions);
			CoTaskMemFree(dbgi[i].pClsidList);
		}
		if( FAILED(hr) )
		{
			CComBSTR bstrError;
			switch( hr )
			{
				case E_HTTP_PERMISSION_DENIED :
					bstrError.LoadString(IDS_E_DEBUG_ASPXPERMISSIONDENIED);
					UtilSetErrorInfo(E_FAIL, bstrError);
					break;
				case E_HTTP_FORBIDDEN :
					bstrError.LoadString(IDS_E_DEBUG_ASPXFORBIDDEN);
					UtilSetErrorInfo(E_FAIL, bstrError);
					break;
				case E_HTTP_SENDREQUEST_FAILED :
					bstrError.LoadString(IDS_E_DEBUG_ASPXSENDREQUESTFAILED);
					UtilSetErrorInfo(E_FAIL, bstrError);
					break;
				case E_HTTP_NOT_SUPPORTED :
					bstrError.LoadString(IDS_E_HTTP_NOT_SUPPORTED);
					UtilSetErrorInfo(E_FAIL, bstrError);
					break;
				case E_ATTACH_DEBUGGER_ALREADY_ATTACHED:
					bstrError.LoadString(IDS_E_DEBUG_ALREADY_ATTACHED);
					UtilSetErrorInfo(E_FAIL, bstrError);
					break;
				default :
					break;
			}
		}
	}

	return hr;
}

HRESULT CGenCfg::UpdateDebuggingInfoForDeployment(VsDebugTargetInfo* dbgi)
{
	CComPtr<IVsShell> spShell;
	HRESULT hr = ExternalQueryService(SID_SVsShell, IID_IVsShell, (void **)&spShell);
	if (hr != S_OK || spShell == NULL)
		return S_OK;

	// figure out if we've even got deployment involved
	CLSID clsidDeploy;
	hr = CLSIDFromString(VS_GUID_DEPLOYMENT_PACKAGE, &clsidDeploy);
	CComPtr<IVsPackage> spPackage;
	hr = spShell->IsPackageLoaded(clsidDeploy, &spPackage);
	if (hr != S_OK || spPackage == NULL)
		return S_OK;

	// check with deployment to see if this target has been deployed
	CComPtr<IVsSolutionDebuggingAssistant2> pDebugAss;
	hr = ExternalQueryService( SID_SVsSolutionDebuggingAssistant2, IID_IVsSolutionDebuggingAssistant2, (void**)&pDebugAss );
	if ( FAILED( hr ) || pDebugAss == NULL)
		return S_OK;

	CComQIPtr<IVsProjectCfg> pProjectCfg = (IVCGenericConfiguration*)this;
	CComBSTR bstrMachine, bstrURL;
	unsigned long ulNumTargets = 0;
	hr = pDebugAss->MapOutputToDeployedURLs( 0, pProjectCfg, dbgi[0].bstrExe, 1, &bstrMachine, &bstrURL, &ulNumTargets );
	if( FAILED( hr ) || ulNumTargets <= 0 )
		return S_OK;

	// reset the exe and remote machine
	SysFreeString(dbgi[0].bstrExe);
	SysFreeString(dbgi[0].bstrRemoteMachine);

	// Change the file URL to be a "Path"
	CStringW strExePath;
	if( bstrURL && _wcsnicmp(bstrURL,L"file:///", 8) == 0 )
	{
		CComBSTR bstrPath = bstrURL + 8;
		strExePath = bstrPath;
		dbgi[0].bstrExe = bstrPath.Detach();
	} 
	else 
	{
		strExePath = bstrURL;
		dbgi[0].bstrExe = bstrURL.Detach();
	}

	bstrMachine.CopyTo(&(dbgi[0].bstrRemoteMachine));

	if (m_pConfig == NULL || !(dbgi[0].bstrCurDir) || *(dbgi[0].bstrCurDir) == L'\0' || bstrMachine.Length() <= 0)
		return S_OK;

	CComBSTR bstrComputerName;
	if (::IsUnicodeSystem())
	{
		wchar_t wszComputerName[MAX_COMPUTERNAME_LENGTH+1];
		DWORD dwLen = MAX_COMPUTERNAME_LENGTH;
		if (::GetComputerNameW(wszComputerName, &dwLen))
			bstrComputerName = wszComputerName;
	}
	else	// must be Win9X
	{
		char szComputerName[MAX_COMPUTERNAME_LENGTH+1];
		DWORD dwLen = MAX_COMPUTERNAME_LENGTH;
		if (::GetComputerNameA(szComputerName, &dwLen))
			bstrComputerName = szComputerName;
	}
	if (bstrComputerName.Length() == 0)
		return S_OK;

	CStringW strDeployMachine = bstrMachine;
	CStringW strLocalMachine = bstrComputerName;
	strDeployMachine.MakeLower();
	strLocalMachine.MakeLower();
	if (strDeployMachine == strLocalMachine)
		return S_OK;

	CComBSTR bstrWorkingDir = dbgi[0].bstrCurDir;
	::SysFreeString(dbgi[0].bstrCurDir);
	dbgi[0].bstrCurDir = NULL;

	CDirW dirExe;
	if (!dirExe.CreateFromPath(strExePath))
		return S_OK;

	CComPtr<IDispatch> spDispProj;
	m_pConfig->get_Project(&spDispProj);
	CComQIPtr<VCProject> spProject = spDispProj;
	if (spProject == NULL)
		return S_OK;

	CComBSTR bstrProjDir;
	spProject->get_ProjectDirectory(&bstrProjDir);
	if (bstrProjDir.Length() == 0)
		return S_OK;

	CDirW dirProj;
	dirProj.CreateFromKnown(bstrProjDir);
	CStringW strWorkingDir = bstrWorkingDir;
	CPathW pathDir;
	if (!pathDir.Create(strWorkingDir))
		return S_OK;

	CStringW strRelativeName;
	if (!pathDir.GetRelativeName(dirProj, strRelativeName))
		strRelativeName = pathDir.GetFileName();

	CPathW pathNewDir;
	if (pathNewDir.CreateFromDirAndFilename(dirExe, strRelativeName))
	{
		CComBSTR bstrNewName = pathNewDir.GetFullPath();
		dbgi[0].bstrCurDir = bstrNewName.Detach();
	}

	return S_OK;
}

// IVsDebuggableProjectCfg
STDMETHODIMP CGenCfg::QueryDebugLaunch(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch, /* [out] */ BOOL *pfCanLaunch)
{
	CHECK_POINTER_NULL(pfCanLaunch);

	HRESULT hr = S_OK;
	if (m_pConfig == NULL)
		*pfCanLaunch = FALSE;
	else
	{
		CComPtr<IDispatch> pDispDbgSettings;
		hr = m_pConfig->get_DebugSettings(&pDispDbgSettings);
		CComQIPtr<VCDebugSettings> pDbgSettings = pDispDbgSettings;
		if( pDbgSettings )
			*pfCanLaunch = TRUE;
		else
			*pfCanLaunch = FALSE;
	}
	return hr;
}

//---------------------------------------------------------------------------
// interface: ISpecifyPropertyPages
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::GetPages(/* [out] */ CAUUID *pPages)
{
	CHECK_POINTER_NULL(pPages)
	
	CComQIPtr<ISpecifyPropertyPages> pSPP = GetVCConfiguration();
	RETURN_ON_NULL2(pSPP, E_NOINTERFACE);
	return pSPP->GetPages( pPages );
}

HRESULT CGenCfg::DoStartBuild(IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, bldActionTypes bldType, BOOL bLocalStorage)
{
	if (m_pBuildHelper == NULL)
		m_pBuildHelper = new CGenCfgBuildHelper;
	RETURN_ON_NULL2(m_pBuildHelper, E_OUTOFMEMORY);

	return m_pBuildHelper->DoStartBuild(this, m_pConfig, pIVsOutputWindowPane, bldType, bLocalStorage);
}

HRESULT CGenCfgBuildHelper::DoStartBuild(IVCGenericConfiguration* pGenCfg, VCConfiguration* pConfig, 
	IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, bldActionTypes bldType, BOOL bLocalStorage)
{
	VSASSERT(g_pBuildPackage->m_pProjectEngine != NULL, "No project engine!  How'd we get this far?");
	if (g_pBuildPackage->m_pProjectEngine == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	GetBuildPkg()->GetBuildEngine(&m_spBuildEngine);

	if (m_spBuildEngine == NULL)
	{
		VSASSERT(FALSE, "No build engine!  How'd we get this far?");
		return E_OUTOFMEMORY;
	}

	CHECK_ZOMBIE(pConfig, IDS_ERR_CFG_ZOMBIE);

	VARIANT_BOOL bBuildable = VARIANT_FALSE;
	VARIANT_BOOL bValid = VARIANT_FALSE;
	CComQIPtr <IVCConfigurationImpl> pConfigImpl = pConfig;
	HRESULT hr = pConfigImpl->get_IsBuildable(&bBuildable);
	RETURN_ON_FAIL(hr);
	 if (bBuildable == VARIANT_FALSE)
		 return DoSetErrorInfo(VCPROJ_E_NOT_BUILDABLE, IDS_ERR_PROJ_NOT_BUILDABLE);
	hr = pConfigImpl->get_IsValidConfiguration(&bValid);
	RETURN_ON_FAIL(hr);
	if (bValid == VARIANT_FALSE)
		 return DoSetErrorInfo(VCPROJ_E_NOT_BUILDABLE, IDS_ERR_PROJ_NOT_BUILDABLE);

	// doing this get of the solution build manager just to initialize it in the main thread
	CComPtr<IVsSolutionBuildManager> spSolnBldMgr;
	hr = GetBuildPkg()->GetSolutionBuildManager(&spSolnBldMgr, TRUE);
	RETURN_ON_FAIL_OR_NULL(hr, spSolnBldMgr);

	if (m_spBuildThread == NULL)
	{
		CComQIPtr <IVCBuildEngineImpl> spBuildEngineImpl = m_spBuildEngine;
		if (spBuildEngineImpl == NULL)
			return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
		else
			hr = spBuildEngineImpl->get_BuildThread(&m_spBuildThread);
		RETURN_ON_FAIL(hr);
		if (m_spBuildThread == NULL)
			return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	}

	CComPtr<IVCBuildOutput> spModelessOutputWindow;
	hr = CVCBuildOutput::CreateInstance(&spModelessOutputWindow, pIVsOutputWindowPane);
	RETURN_ON_FAIL_OR_NULL(hr, spModelessOutputWindow);

	CComPtr<IVCBuildErrorContext> spErrorContext;

	CComPtr<IDispatch> spDispProject;
	pConfig->get_Project(&spDispProject);
	CComQIPtr<VCProject> spProject = spDispProject;
	RETURN_ON_NULL2(spProject, E_UNEXPECTED);
	CComBSTR bstrDir;
	spProject->get_ProjectDirectory(&bstrDir);
	CComBSTR bstrName;
	spProject->get_Name(&bstrName);

	CBldOutputWinEC::CreateInstance(&spErrorContext, NULL, m_spBuildEngine, bstrDir, spModelessOutputWindow);
	RETURN_ON_NULL2(spErrorContext, E_OUTOFMEMORY);
	spErrorContext->put_ProjectName(bstrName);

	hr = m_spBuildThread->InitializeForBuild(pGenCfg, pConfig, m_celtFiles, m_ppFiles, bldType, (m_ppFiles == NULL), 
		FALSE, m_spBuildEngine, spErrorContext);

	if (SUCCEEDED(hr))
		hr = m_spBuildThread->StartThread();
	
	return hr;
}
        
//---------------------------------------------------------------------------
// interface: IVCCfg
//---------------------------------------------------------------------------
STDMETHODIMP CGenCfg::get_Tool(/* [in] */ VARIANT toolIndex, /* [out] */ IUnknown **ppConfig)
{
	CHECK_POINTER_NULL(ppConfig);
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);
	
	HRESULT hr;

	CComQIPtr<IVCCollection> spCollection;	//Collection SP
	CComPtr<IDispatch> pDisp;

	hr = m_pConfig->get_Tools(&pDisp);	//Get the tools collection
	spCollection = pDisp;
	
	if (SUCCEEDED(hr) && spCollection)
	{
		CComPtr<IDispatch> spDisp;
		hr = spCollection->Item(toolIndex, &spDisp);	//Get the tool item
		RETURN_ON_FAIL_OR_NULL(hr, spDisp);
		hr = spDisp->QueryInterface(IID_IUnknown, (void **) ppConfig);	//Get the tool unk
	}
	
	return hr;
}

STDMETHODIMP CGenCfg::get_Object( IDispatch **ppConfig)
{
	CHECK_POINTER_NULL(ppConfig);
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);
	return m_pConfig->QueryInterface(IID_IDispatch, (void **) ppConfig);
}

