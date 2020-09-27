// GenCfg.h: Definition of the CGenCfg class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENCFG_H__9A9BFE7A_0D1A_11D1_B59E_00A0C91BC942__INCLUDED_)
#define AFX_GENCFG_H__9A9BFE7A_0D1A_11D1_B59E_00A0C91BC942__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "rsrc.h"       // main symbols
#include <path2.h>
#include "bldpkg.h"

#if !defined(NUMBER_OF)
#define NUMBER_OF(x) (sizeof(x) / sizeof((x)[0]))
#endif // !defined(NUMBER_OF)

const int MAX_ADVISES = 5;

#define PRIMARY_OUTPUT		0
#define SECONDARY_OUTPUT	1
#define CONTENT_OUTPUT		2
#define SOURCE_OUTPUT		3

/////////////////////////////////////////////////////////////////////////////
// CGenCfg

class CGenCfg;
class CVCArchy;
class COutputGroup;
class CGenCfgBuildHelper;

/////////////////////////////////////////////////////////////////////////////
// CGenCfg

class CGenCfg : 
	public IVsBuildableProjectCfg,
 	public ISpecifyPropertyPages,
	public IVsDebuggableProjectCfg,
	public IVsProjectCfg2,
	public CComObjectRoot,
	public IVCGenericConfiguration,
	public IVCCfg
{
public:
	CGenCfg();
	~CGenCfg();
	void Initialize(CVCArchy *pArchy, VCConfiguration *pCfg);
	static HRESULT CreateInstance(IVsCfg **ppGenCfg, CVCArchy *pArchy, VCConfiguration*);

BEGIN_COM_MAP(CGenCfg)
	COM_INTERFACE_ENTRY(IVCCfg)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IVsBuildableProjectCfg)
	COM_INTERFACE_ENTRY(IVsProjectCfg2)
	COM_INTERFACE_ENTRY(IVsDebuggableProjectCfg)
	COM_INTERFACE_ENTRY2(IVsProjectCfg, IVsDebuggableProjectCfg)
	COM_INTERFACE_ENTRY2(IVsCfg, IVsProjectCfg2)	// parent to IVsProjectCfg
	// IVsCfg
	COM_INTERFACE_ENTRY(IVCGenericConfiguration)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CGenCfg) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()


// IVsProjectCfg
public:
	STDMETHOD(get_ProjectCfgProvider)( IVsProjectCfgProvider **ppIVsProjectCfgProvider);
	STDMETHOD(get_CanonicalName)( BSTR *pbstrCanonicalName);
	STDMETHOD(get_DisplayName)( BSTR *pbstrDisplayName);
	STDMETHOD(get_IsDebugOnly)( BOOL *pfIsDebugOnly);
	STDMETHOD(get_IsReleaseOnly)( BOOL *pfIsRetailOnly);
	STDMETHOD(get_IsRetailOnly)( BOOL *pfIsRetailOnly);
	STDMETHOD(get_Platform)( GUID *pguidPlatform);
	STDMETHOD(EnumOutputs)( IVsEnumOutputs **ppIVsEnumOutputs);
	STDMETHOD(OpenOutput)(LPCOLESTR szOutputCanonicalName, IVsOutput **ppIVsOutput);
	STDMETHOD(get_BuildableProjectCfg)(IVsBuildableProjectCfg **ppIVsBuildableProjectCfg);
	STDMETHOD(get_IsPackaged)( BOOL *pfIsPackaged);
	STDMETHOD(get_IsSpecifyingOutputSupported)( BOOL *pfIsSpecifyingOutputSupported);

	// Unicode is officially code page 1200.  For strict unicode vs. mbcs matching of
	// project configurations, you can use TargetCodePage == 1200 as equivalent to
	// the project configuration being for Unicode.  Project configurations which
	// are generically not Unicode should use CP_ACP.
	STDMETHOD(get_TargetCodePage)( /* [out] */ UINT *puiTargetCodePage);

	// The UpdateSequenceNumber property is used to determine how often to reprompt
	// the user about whether they would like to rebuild the solution.  The primary
	// scenario for this property is stepping in the debugger.  On each step operation
	// in the debugger, the solution build manager will check the active project
	// configurations, to see if any have updated their USNs.
	STDMETHOD (get_UpdateSequenceNumber)(/* [out] */ ULARGE_INTEGER *puliUSN);

	// The RootURL property is the root URL for the project outputs.  Essentially,
	// if an output's project-relative path is required, it's computed by removing
	// the RootURL from its DeploySourceURL.  As is the case for DeploySourceURL
	// values, the convention is that paths that are in the local filesystem
	// begin with the eight characters: "file:///".
	STDMETHOD (get_RootURL)(/* [out] */ BSTR *pbstrRootURL);

// IVsProjectCfg2
public:
    STDMETHOD(get_CfgType)(REFIID iidCfg, void **ppCfg);
    STDMETHOD(get_OutputGroups)(ULONG celt, IVsOutputGroup *rgpcfg[], ULONG *pcActual);
    STDMETHOD(OpenOutputGroup)(LPCOLESTR szCanonicalName, IVsOutputGroup **ppIVsOutputGroup);
	STDMETHOD(OutputsRequireAppRoot)(BOOL *pfRequiresAppRoot);	// MattGe 8/12/99:  New method.
    STDMETHOD(get_VirtualRoot)(BSTR *pbstrVRoot);
    STDMETHOD(get_IsPrivate)(BOOL *pfPrivate);

// IVsBuildableProjectCfg        
public:
    STDMETHOD(get_ProjectCfg)(IVsProjectCfg __RPC_FAR *__RPC_FAR *ppIVsProjectCfg);
    STDMETHOD(AdviseBuildStatusCallback)(IVsBuildStatusCallback __RPC_FAR *pIVsBuildStatusCallback, VSCOOKIE __RPC_FAR *pdwCookie);
    STDMETHOD(UnadviseBuildStatusCallback)(VSCOOKIE dwCookie);
    STDMETHOD(StartBuild)(IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, DWORD dwOptions);
    STDMETHOD(StartClean)(IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, DWORD dwOptions);
    STDMETHOD(StartUpToDateCheck)(IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, DWORD dwOptions);
    STDMETHOD(QueryStatus)(BOOL __RPC_FAR *pfBuildDone);
    STDMETHOD(Stop)(BOOL fSync);
    STDMETHOD(Wait)(DWORD dwMilliseconds, BOOL fTickWhenMessageQNotEmpty);
    STDMETHOD(QueryStartBuild)(DWORD dwOptions, BOOL __RPC_FAR *pfSupported, BOOL __RPC_FAR *pfReady);
    STDMETHOD(QueryStartClean)(DWORD dwOptions, BOOL __RPC_FAR *pfSupported, BOOL __RPC_FAR *pfReady);
    STDMETHOD(QueryStartUpToDateCheck)(DWORD dwOptions, BOOL __RPC_FAR *pfSupported, BOOL __RPC_FAR *pfReady);
			
// IVsDebuggableProjectCfg
public:
    STDMETHOD(DebugLaunch)(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch);
    STDMETHOD(QueryDebugLaunch)(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch, /* [out] */ BOOL *pfCanLaunch);

// ISpecifyPropertyPages
public:
    STDMETHOD(GetPages)( /* [out] */ CAUUID *pPages );
       
// IVCGenericConfiguration
public:
	STDMETHOD(InformBuildBegin)();
	STDMETHOD(InformBuildEnd)(BOOL fOK);
	STDMETHOD(StartAutomationBuild)(bldActionTypes bldType);
	STDMETHOD(StartFileBuild)(ULONG celt, VCFile* pFile[]);
	STDMETHOD(SupportsBuildType)(bldActionTypes buildType, VARIANT_BOOL* pbSupports, VARIANT_BOOL* pbAvailable);
	STDMETHOD(get_NoBuildIsInProgress)(BOOL* pbNoneInProgress);	// no build currently in progress

// IVCCfg
public:
	STDMETHOD(get_Tool)(/* [in] */ VARIANT toolIndex, /* [out] */ IUnknown **ppConfig);
	STDMETHOD(get_Object)(/* [out] */ IDispatch **ppConfig);

// CGenCfg
public:
	VCConfiguration* GetVCConfiguration()	{ return m_pConfig; }	// not ref-counted
	HRESULT StartSolutionBuild(DWORD dwFlags, VSSOLNBUILDUPDATEFLAGS solnFlags);

protected:
	HRESULT InitEnumOuts();
	void ClearOutputs();
	void SetOutputPath(BSTR bstrOut, CPathW& rpathPrimary, CPathW& rpathPdb);
	HRESULT DoStartBuild(IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, bldActionTypes bldType, BOOL bLocalStorage);
	HRESULT QueryCanBuild(BOOL __RPC_FAR *pfSupported, BOOL __RPC_FAR *pfReady);
	BOOL SpawnerActive();

// variables
protected:
	CComPtr<VCConfiguration> m_pConfig;
	CVCPtrList 	m_ListOutputs;	// list of CProjectOutput for IVsOutput enumeration. See COutsEnum
	CBldMarshaller<IVsProjectCfgProvider> m_marshalVsProjectCfgProvider;
	CGenCfgBuildHelper* m_pBuildHelper;

// some nice helper functions
public:
	static HRESULT GetOutputWindowPane(IVsOutputWindowPane** ppOutputWindowPane);

protected:
	HRESULT UpdateDebuggingInfoForDeployment(VsDebugTargetInfo* dbgi);
};

class CGenCfgBuildHelper
{
public:
	CGenCfgBuildHelper();
	~CGenCfgBuildHelper();
	HRESULT InformBuildBegin();
	void InformBuildEnd(BOOL fOK);
	HRESULT DoStartBuild(IVCGenericConfiguration* pGenCfg, VCConfiguration* pConfig, 
		IVsOutputWindowPane __RPC_FAR *pIVsOutputWindowPane, bldActionTypes bldType, BOOL bLocalStorage);
	HRESULT AdviseBuildStatusCallback(/* [in] */ IVsBuildStatusCallback __RPC_FAR *pIVsBuildStatusCallback,
		/* [out] */ VSCOOKIE __RPC_FAR *pdwCookie);
	HRESULT UnadviseBuildStatusCallback(/* [in] */ VSCOOKIE dwCookie);
	HRESULT Stop(BOOL fSync);
	HRESULT Wait(/* [in] */ DWORD dwMilliseconds, /* [in] */ BOOL fTickWhenMessageQNotEmpty);
	BOOL SpawnerActive();
	HRESULT SetLocalStorage(ULONG celtFiles, VCFile* pFiles[]);
	void FreeFiles(BOOL bExpectFresh);

public:
	// thread for building
	CComPtr<IVCBuildThread> m_spBuildThread;
	CComPtr<IVCBuildEngine> m_spBuildEngine;

	// for AdviseBuildStatusCallBack, etc.	
	CComPtr<IVsBuildStatusCallback> m_rgsrpIVsBuildStatusCallback[MAX_ADVISES];
	CComPtr<IStream> m_rgsrpIVsBuildStatusStreamCallback[MAX_ADVISES]; 
	bool m_rgfTicking[MAX_ADVISES];
	bool m_rgfStarted[MAX_ADVISES];
	VCFile** m_ppFiles;
	ULONG m_celtFiles;
};

#endif // !defined(AFX_GENCFG_H__9A9BFE7A_0D1A_11D1_B59E_00A0C91BC942__INCLUDED_)
