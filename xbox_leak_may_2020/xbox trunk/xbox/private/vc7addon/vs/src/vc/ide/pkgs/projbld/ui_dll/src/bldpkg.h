// BldPkg.h : Declaration of the CBuildPackage

#ifndef __BUILDPACKAGE_H_
#define __BUILDPACKAGE_H_

#include "rsrc.h"       // main symbols
#include "servprov.h"

#include "vc.h"      // in vc\ide\include
#include <commctrl.h> // for HIMAGELIST
#include "vcguid.h"
#include <objext.h>		// IProfferService
#include <BldMarshal.h>	// for CBldMarshaller
#include "mssplash.h"
#include "utils.h"

#include "prjnode.h"
#include "vssolutn.h"
#include "vstrkdoc.h"
#include <IVsTrackProjectDocuments2.h>
 
typedef CVCPtrList CProjNodeList;

const int MAX_PLATFORMS=16;
#define W_VISUAL_CPP L"VisualC++"

/////////////////////////////////////////////////////////////////////////////
// forward declarations
class CBuildPackage;
class CVCFileNode;	// defined in vcfile.h
class CVCFileGroup;	// defined in vcfgrp.h
class CVCArchy;
class CBuildOptions;
class CDirectoriesDlg;
class CModule;

// Our one and only build Package object
// This is defined in vproj.cpp
extern CBuildPackage * g_pBuildPackage;
CBuildPackage* GetBuildPkg(void);

// global "in project load" flag. DO NOT EDIT DIRECTLY!!!
extern bool g_bInProjectLoad;

// global "root level project" string for VC 4.x conversion ONLY
extern wchar_t* g_wszRootProjectName;

IServiceProvider* GetServiceProvider(void);
HRESULT ExternalQueryService(REFGUID rsid, REFIID iid, void **ppvObj);
void ReportError(BSTR bstrError, HRESULT hrReport = E_FAIL);
void HandleSetErrorInfo(HRESULT hrReport, BSTR bstrError);
// are we under the IDE or not?
BOOL FIsIDEInteractive();

//-----------------------------------------------------------------------------
// provides:
//		interface: IVsProjectFactory
//-----------------------------------------------------------------------------
class CVCProjectFactory :
	public IVsProjectFactory,
	public IVsOwnedProjectFactory,
	public CComObjectRoot
{
public:
			 CVCProjectFactory(void);
	virtual ~CVCProjectFactory(void);

BEGIN_COM_MAP(CVCProjectFactory)
	COM_INTERFACE_ENTRY(IVsProjectFactory)
	COM_INTERFACE_ENTRY(IVsOwnedProjectFactory)
END_COM_MAP()

// IVsProjectFactory
public:
	STDMETHOD(CanCreateProject)( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ DWORD grfCreateProj,
            /* [out] */ BOOL *pfCanCreate);
        
	STDMETHOD(CreateProject)( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled);

	STDMETHOD(SetSite)(
			/* [in] */ IServiceProvider *pSP);

	STDMETHOD(Close)(void);

// IVsOwnedProjectFactory
public:
    STDMETHOD(PreCreateForOwner)( 
        /*[in]*/  IUnknown* pOwner,
        /*[out]*/ IUnknown**            ppInner,
        /*[out]*/ VSOWNEDPROJECTOBJECT* pCookie );

    STDMETHOD(InitializeForOwner)( 
        /*[in]*/ LPCOLESTR pszFilename,
        /*[in]*/ LPCOLESTR                 pszLocation,
        /*[in]*/ LPCOLESTR                 pszName,
        /*[in]*/ VSCREATEPROJFLAGS         grfCreateFlags,
        /*[in]*/ REFIID                    iidProject,
        /*[in]*/ VSOWNEDPROJECTOBJECT      cookie, // additional parameter
        /*[out, iid_is(iidProject)]*/void  **ppvProject,
        /*[out]*/ BOOL                     *pfCanceled);

public:
	static VSOWNEDPROJECTOBJECT s_EFPolicyCookie;
protected:
	CComPtr<IServiceProvider> m_srpIServiceProvider;

};

class CExeProjectFactory :
	public IVsProjectFactory,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CExeProjectFactory)
	COM_INTERFACE_ENTRY(IVsProjectFactory)
END_COM_MAP()

// IVsProjectFactory
public:
	STDMETHOD(CanCreateProject)( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ DWORD grfCreateProj,
            /* [out] */ BOOL *pfCanCreate);
        
	STDMETHOD(CreateProject)( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled);

	STDMETHOD(SetSite)(
			/* [in] */ IServiceProvider *pSP);

	STDMETHOD(Close)(void);

};

class CCrashDumpProjectFactory : public CExeProjectFactory
{
// IVsProjectFactory
public:
	STDMETHOD(CreateProject)( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled);
};

#ifdef AARDVARK
class CStyleSheetProjectFactory :
	public IVsProjectFactory,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CStyleSheetProjectFactory)
	COM_INTERFACE_ENTRY(IVsProjectFactory)
END_COM_MAP()

// IVsProjectFactory
public:
	STDMETHOD(CanCreateProject)( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ DWORD grfCreateProj,
            /* [out] */ BOOL *pfCanCreate);
        
	STDMETHOD(CreateProject)( 
            /* [in] */ LPCOLESTR pszFilename,
            /* [in] */ LPCOLESTR pszLocation,
            /* [in] */ LPCOLESTR pszName,
            /* [in] */ DWORD grfCreateProj,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppUnk,
            /* [out] */ BOOL *pfCanceled);

	STDMETHOD(SetSite)(
			/* [in] */ IServiceProvider *pSP);

	STDMETHOD(Close)(void);

};
#endif	// AARDVARK

class CVUPProjectFactory :
	public IVsProjectFactory,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVUPProjectFactory)
	COM_INTERFACE_ENTRY(IVsProjectFactory)
END_COM_MAP()

// IVsProjectFactory
public:
	STDMETHOD(CanCreateProject)( LPCOLESTR pszFilename, DWORD grfCreateProj, BOOL *pfCanCreate)
	{
		// if extension is .vup pretend that this is going to work.
		if( pfCanCreate )
		{
			*pfCanCreate = TRUE;
		}
		return S_OK;
	}
        
	STDMETHOD(CreateProject)( LPCOLESTR pszFilename, LPCOLESTR pszLocation, LPCOLESTR pszName, DWORD grfCreateProj, REFIID riid, void **ppUnk, BOOL *pfCanceled)
	{
		// Put up an error message that say Sorry Nope !
		CComBSTR bstrErr;
		bstrErr.LoadString(IDS_E_NO_MORE_VUP);
	    	UtilSetErrorInfo( E_FAIL, bstrErr );
		return E_FAIL;
	}

	STDMETHOD(SetSite)(IServiceProvider *pSP)
	{
		m_srpIServiceProvider = pSP;
		return S_OK;
	}

	STDMETHOD(Close)(void)
	{
		m_srpIServiceProvider = NULL;
		return S_OK;
	};

	CComPtr<IServiceProvider> m_srpIServiceProvider;

};

extern _ATL_REGMAP_ENTRY g_rm[];

/////////////////////////////////////////////////////////////////////////////
// CBuildPackage
class CBuildPackage : 
	public IVsPackage,
	public IOleCommandTarget,
	public IVCBuildPackageInternal,
	public IVsPersistSolutionOpts,
	public IServiceProvider,
	public IVsUpdateSolutionEvents,
	public IVsMicrosoftInstalledProduct,
	public IVsSolutionEvents,
	public CComObjectRoot,
	public CComCoClass<CBuildPackage, &CLSID_BuildPackage>
{
public:
	CBuildPackage();
	~CBuildPackage();
	STDMETHOD(Initialize)();

	static HRESULT WINAPI UpdateRegistry(BOOL bRegister) {
		return _Module.UpdateRegistryFromResource( IDR_BUILDPACKAGE, bRegister, g_rm );
	}

//DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CBuildPackage)
	COM_INTERFACE_ENTRY(IVsPackage)
	COM_INTERFACE_ENTRY(IServiceProvider)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY(IVsPersistSolutionOpts)
	COM_INTERFACE_ENTRY(IVCBuildPackageInternal)
	COM_INTERFACE_ENTRY(IVsUpdateSolutionEvents)
	COM_INTERFACE_ENTRY(IVsSolutionEvents)
	COM_INTERFACE_ENTRY(IVsMicrosoftInstalledProduct)
	COM_INTERFACE_ENTRY(IVsInstalledProduct)
END_COM_MAP()

// IVsPackage
public:
   	STDMETHOD(SetSite)(IServiceProvider *pSP);
	STDMETHOD(QueryClose)(BOOL *pCanClose);
	STDMETHOD(Close)();
	STDMETHOD(GetAutomationObject)(LPCOLESTR pszPropName, IDispatch **ppDisp);
	STDMETHOD(GetPropertyPage)(REFGUID rguidPage, VSPROPSHEETPAGE *ppage);
	STDMETHOD(ResetDefaults)(PKGRESETFLAGS dwFlags);
	STDMETHOD(CreateTool)(REFGUID rguidPersistenceSlot);

// IVsPersistSolutionOpts
public:
    STDMETHOD(SaveUserOptions)(IVsSolutionPersistence *pPersistence);
    STDMETHOD(LoadUserOptions)(IVsSolutionPersistence *pPersistence, VSLOADUSEROPTS grfLoadOpts);
    STDMETHOD(WriteUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);
    STDMETHOD(ReadUserOptions)(IStream *pOptionsStream, LPCOLESTR pszKey);

// IOleCommandTarget
public:
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);

// IServiceProvider
public:
	STDMETHOD(QueryService)(THIS_ REFGUID rsid, REFIID iid, void ** ppvObj);

//IVsMicrosoftInstalledProduct
    STDMETHOD(get_IdBmpSplash)(UINT *pIdBmp) { *pIdBmp = IDB_SPLASHSCREEN; return NOERROR; }
    STDMETHOD(get_OfficialName)(BSTR *pbstrName);
    STDMETHOD(get_ProductID)(BSTR *pbstrPID) {return E_NOTIMPL;} //pulled from registry
    STDMETHOD(get_ProductDetails)(BSTR *pbstrProductDetails);
    STDMETHOD(get_IdIcoLogoForAboutbox)(UINT *pIdIco) {*pIdIco = IDI_AboutBox; return S_OK;}
    STDMETHOD(get_ProductRegistryName)(BSTR *pbstrRegName){*pbstrRegName = SysAllocString(W_VISUAL_CPP); return S_OK;}
  
// IVsUpdateSolutionEvents
public:
    STDMETHOD(UpdateSolution_Begin)(BOOL* pbCancelUpdate) { return S_OK; }
    STDMETHOD(UpdateSolution_Done)(BOOL bSucceeded, BOOL bModified, BOOL bCancelCommand) { return S_OK; }
    STDMETHOD(UpdateSolution_StartUpdate)(BOOL* pbCancelUpdate) { return S_OK; }
    STDMETHOD(UpdateSolution_Cancel)() { return S_OK; }
    STDMETHOD(OnActiveProjectCfgChange)(IVsHierarchy* pIVsHierarchy);

// IVsSolutionEvents
public:
	STDMETHOD(OnAfterOpenProject)( IVsHierarchy *pHierarchy, BOOL fAdded);
	STDMETHOD(OnQueryCloseProject)( IVsHierarchy *pHierarchy, BOOL fRemoving, BOOL __RPC_FAR *pfCancel);
	STDMETHOD(OnBeforeCloseProject)( IVsHierarchy *pHierarchy, BOOL fRemoved);
	STDMETHOD(OnAfterLoadProject)( IVsHierarchy *pStubHierarchy, IVsHierarchy __RPC_FAR *pRealHierarchy);
	STDMETHOD(OnQueryUnloadProject)( IVsHierarchy *pRealHierarchy, BOOL __RPC_FAR *pfCancel);
	STDMETHOD(OnBeforeUnloadProject)( IVsHierarchy *pRealHierarchy, IVsHierarchy __RPC_FAR *pStubHierarchy);
	STDMETHOD(OnAfterOpenSolution)( IUnknown *pUnkReserved, BOOL fNewSolution);
	STDMETHOD(OnQueryCloseSolution)( IUnknown *pUnkReserved, BOOL __RPC_FAR *pfCancel);
	STDMETHOD(OnBeforeCloseSolution)( IUnknown *pUnkReserved);
	STDMETHOD(OnAfterCloseSolution)( IUnknown *pUnkReserved);

// IVCBuildPackageInternal
public:
	STDMETHOD(GetConfigurationDependencies)(VCConfiguration* pProjConfig, IVCBuildOutputItems** ppBuildOutputs);
	STDMETHOD(HasConfigurationDependencies)(VCConfiguration* pProjConfig, VARIANT_BOOL* pbHasDeps);
	STDMETHOD(GetUIConfigurationForVCConfiguration)(VCConfiguration* pProjCfg, IVCGenericConfiguration** ppGenCfg);
	STDMETHOD(get_SolutionFile)(BSTR* pbstrSolnFile);
	STDMETHOD(SaveProject)(VCProject* pProject, LPCOLESTR szFileName);
	STDMETHOD(AddConfigurationToProject)(VCProject* pProject, BSTR bstrConfigName);
	STDMETHOD(AddPlatformToProject)(VCProject* pProject, LPCOLESTR szPlatformName);
	STDMETHOD(RemovePlatformFromProject)(VCProject* pProject, IDispatch* pDispPlatform);
	STDMETHOD(ErrorCloseOfProject)(VCProject* pProject);	// call this BEFORE removing a 'dead' project from the project collection; ERROR CLOSE OF PROJECT ONLY
	STDMETHOD(AllowedToAddFile)(VCProject* pProject, LPCOLESTR szFile, VARIANT_BOOL bSetErrorInfo);
	STDMETHOD(AllowedToRemoveFile)(VCProject* pProject, LPCOLESTR szFile);
	STDMETHOD(InformFileAdded)(VCProject* pProject, LPCOLESTR szFile);
	STDMETHOD(InformFileRemoved)(VCProject* pProject, LPCOLESTR szFile);

// helper functions
public:
	HRESULT GetBuildErrorContext(IVCBuildErrorContext** ppIVCBuildErrorContext);
	HRESULT GetDebugCommandLines(VCDebugSettings* pDbgSettings, VCConfiguration* pConfig);
	HRESULT GetVsShell(IVsShell** ppShell);
	HRESULT GetDTE(DTE** lppaReturn);
	HRESULT DoFallbackAddConfiguration(VCProject* pProject, BSTR bstrConfigName);
	HRESULT DoSetErrorInfo(HRESULT hrErr, long idErr, BOOL bSetInfo = TRUE);
	HRESULT DoSetErrorInfo2(HRESULT hrErr, long idErr, LPCOLESTR szParam, BOOL bSetInfo = TRUE);

protected:
	BOOL GetVsProjectAndTracker(VCProject* pProject, CComPtr<IVsProject>& rspVsProj, CComPtr<IVsTrackProjectDocuments>& rspTrackDocs);
	BOOL GetVsProjectAndTracker(VCProject* pProject, CComPtr<IVsProject>& rspVsProj, CComPtr<IVsTrackProjectDocuments2>& rspTrackDocs);
	void GetVsProject(VCProject* pProject, CComPtr<IVsProject>& rspVsProj);
	int	 FindFirstSeparator(CStringW& strList, CStringW& strSeparatorList, int nStartIdx);
	int	 GetNextItem(CStringW& strList, int nStartIdx, int nMaxIdx, CStringW& strSeparator, CStringW& strItem);
	int	 GetNextQuotedItem(CStringW& strList, int nStartIdx, int nMaxIdx, int nSep, int nLastQuote,
		BOOL bOddQuote, CStringW& strSeparator, CStringW& strItem);

////////////////////////////////////////////////////////////
// non-COM methods for manipulating build package items
////////////////////////////////////////////////////////////
public:
	static	HRESULT	CreateInstance(CBuildPackage **);

public:
	HRESULT GetBuildEngine(IVCBuildEngine** ppBuildEngine);
	VCProjectEngine* 	GetProjectEngine();			// not ref-counted
	CVCProjectNode* 	GetProjectNode(VCProject* pProject);
	HRESULT GetArchyForProjConfig(VCConfiguration* pProjCfg, CVCArchy** ppArchy);
	HRESULT GetProjectNodeForProjConfig(VCConfiguration* pProjCfg, CVCProjectNode** ppProjNode);
	HRESULT GetHierarchyForProjConfig(VCConfiguration* pProjCfg, IVsHierarchy** ppHier, BOOL bInMainThread = FALSE);
	HRESULT GetSolutionBuildManager(IVsSolutionBuildManager** ppSolnBldMgr, BOOL bInMainThread = FALSE);
	HRESULT GetVsUIShell(IVsUIShell** ppShell, BOOL bInMainThread = FALSE);
	HRESULT GetTaskList(IVsTaskList** ppTaskList, BOOL bInMainThread = FALSE);
	HRESULT GetStatusbar(IVsStatusbar** ppStatusBar, BOOL bInMainThread = FALSE);
	HRESULT UpdateSolutionFile();

	void				AddSolutionRef();
	void				ReleaseSolutionRef();
	HRESULT GetIVsSolution(IVsSolution** ppSolution);	// intended for main thread use only
	void				IncrementSolutionBuildCount() { m_cSolutionBuilds++; }
	void				DecrementSolutionBuildCount() { if (m_cSolutionBuilds) m_cSolutionBuilds--; }
	BOOL				NoBuildsInProgress()			{ return (m_cSolutionBuilds == 0); }

	virtual BOOL OnInit();
	virtual void OnExit();
	// OLE Automation
	IDispatch* GetAutoProjects();
	// UIArchy related functions
	void InitializeArchy(BSTR bstrFilename, CVCProjectNode* pProj, CVCArchy* pArchy);
    void DoRefreshTaskList();
	void DoUpdateTaskList();
	IVCNotificationSource* GetNotificationSource();
	BOOL UseGlobalEditorForResourceFiles() { return m_bUseGlobalEditorForRC; }
	void SetUseGlobalEditorForResourceFiles(VARIANT_BOOL bUseGlobal);

	BOOL GetSccManager(IVsSccManager2** ppManager);

public:
	static CComBSTR s_bstrAltKey;
    static CVCExecutionCtx s_executionCtx;    // Global Execution Context object

public:  // REVIEW: all of these could be private with a little effort
	CComPtr<IServiceProvider>		m_pServiceProvider;
	CComPtr<IVCNotificationSource>	m_pVCNotificationSource;
 	CComPtr<VCProjectEngine> 		m_pProjectEngine;
	CComPtr<IVCProjectEngineEvents> m_pProjBuildEvents;	
	HIMAGELIST						m_hImageList;
	int								m_fAddNewItem;

private:
	CComQIPtr<IDispatch> m_spAutoProjects;

	CBuildOptions 	*m_pBuildOptions;
	CDirectoriesDlg *m_pBuildDirectories;
	CComPtr<IVsSolutionBuildManager> m_spSolutionBuildManager;
	CBldMarshaller<IVsSolutionBuildManager> m_marshalSolutionBuildManager;
	CBldMarshaller<IVsUIShell> m_marshalVsUIShell;
	CBldMarshaller<IVsTaskList> m_marshalTaskList;
	CBldMarshaller<IVsSolution> m_marshalVsSolution;
	CComPtr<IVsSccManager2> m_spVsSccManager2;
	long			m_nSolutionRefCount;
	VARIANT_BOOL	m_bUseGlobalEditorForRC;

	VSCOOKIE		m_pVCProjectCookie;
	VSCOOKIE		m_pExeProjectCookie;
	VSCOOKIE		m_pCrashDumpProjectCookie;
	VSCOOKIE		m_pVUPProjectCookie;
#ifdef AARDVARK
	VSCOOKIE		m_pStyleSheetProjectCookie;
#endif	// AARDVARK
	VSCOOKIE		m_dwTaskCookie;
	VSCOOKIE		m_vsSolutionEventsCookie;	// cookie for UpdateSolutionEvents
	VSCOOKIE		m_dwSlnEventsCookie;		// cookie for SolutionEvents

	DWORD           m_dwProjectEngineEventsCookie;  
	DWORD			m_dwBuildProjectEngineCookie;	// REVIEW: this should be a "VSCOOKIE"
	DWORD			m_dwENCBuildSpawnerCookie;		// REVIEW: this should be a "VSCOOKIE"
	DWORD			m_dwBuildErrorContextCookie;	// REVIEW: this should be a "VSCOOKIE"

    CComPtr<IVsTaskList> m_pTaskList;
	CComPtr<IVsUIShell>	m_spVsUIShell;
	CComPtr<IVCBuildOutput> m_spModelessOutputWindow;

	GUID			m_guidPersistenceSlot;
	CVCPtrList 		m_exePrjList;
	long			m_cSolutionBuilds;

	CComBSTR		m_bstrSolutionFile;
};

// Image offsets for our image list.
// NOTE!!! Do NOT change the order! Always add the "grey" version of an image before the
//		   nongrey version. The code relies on this.
enum
{
// 	BMP_LINKOVERLAY = 1,		// "link" overlay (1 based offset)
// 	BMP_NOTINSCCOVERLAY = 2,			// "not in scc" overlay
// 	BMP_CHECKEDOUTOVERLAY = 3,	// "checked out" overlay
// 	BMP_GREYHTML= 3,
// 	BMP_HTML,
	BMP_LINKOVERLAY,		// "link" overlay (1 based offset)
	BMP_NOTINSCCOVERLAY,			// "not in scc" overlay
	BMP_CHECKEDOUTOVERLAY,	// "checked out" overlay
	BMP_BUILDABLEITEM,
	BMP_NONBUILDABLEITEM,
	BMP_GREYCSS,
	BMP_CSSDEAD,
	BMP_GREYDINALI,
	BMP_DINALI,
	BMP_GREYGLOBAL,
	BMP_GLOBAL,
	BMP_GREYTEXT,
	BMP_TEXT,
	BMP_GREYIMAGEMAP,
	BMP_IMAGEMAP,
	BMP_GREYDOCUMENT,
	BMP_DOCUMENT,
	BMP_GREYAPPLICATION,
	BMP_APPLICATION,
	BMP_GREYACTIVELAYOUT,
	BMP_ACTIVELAYOUT,
	BMP_GREYIMAGE,
	BMP_IMAGE,
	BMP_GREYAUDIO,
	BMP_AUDIO,
	BMP_GREYVIDEO,
	BMP_VIDEO,
	BMP_GREYXWORLD,
	BMP_XWORLD,
	BMP_FOLDER,
	BMP_OPENFOLDER,
	BMP_PROJNODE,
	BMP_DISABLEDPROJNODE,
	BMP_GREYUNKNOWN,
	BMP_UNKNOWN,
	BMP_DBCONNECTION,
	BMP_NODBCONNECTION,
	BMP_COMPILABLEFILE,
    BMP_SOLUTION,
	// AARDVARK bitmaps start here:
	BMP_REFERENCEFOLDER,
	BMP_OPENREFERENCEFOLDER,
	BMP_REFERENCE,
	BMP_WINFORM,
	BMP_CONTROL,
	BMP_CLASS,
	BMP_VBFILE,
	BMP_CSFILE,
	BMP_XSD,
	BMP_XML,
	BMP_WEBFORM,
	BMP_GLOBALASAX,
	BMP_WEBSERVICE,
	BMP_WEBCONTROL,
	BMP_WEBCUSTOMCONTROL,
	BMP_ASPPAGE,
	BMP_WEBCONFIG,
	BMP_HTML,
	BMP_CSS,
	BMP_BMP,
	BMP_BADREFERENCE,
	BMP_ICO,
	BMP_RESX,
	BMP_SCRIPT,
	BMP_CSPROJNODE,
	BMP_VBPROJNODE,
	BMP_OUTPUTS,
	BMP_SATELLITE,
	BMP_OUTPUTSFOLDER,
	BMP_OPENOUTPUTSFOLDER,
	BMP_SATELLITEFOLDER,
	BMP_OPENSATELLITEFOLDER
};
#define BITMAPIMAGE_X	16
#define IMAGELISTMASKCOLOR RGB(255,0,255)


void GetFromRegistry();

#endif //__BUILDPACKAGE_H_
