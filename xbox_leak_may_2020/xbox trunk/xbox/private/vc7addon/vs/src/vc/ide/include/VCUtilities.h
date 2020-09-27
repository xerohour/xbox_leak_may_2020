#pragma once

struct IVsSolutionBuildManager;
struct IVsStatusbar;
struct VCProjectEngine;
struct IVCTextEdit;
struct ILocalRegistry;
struct IUIHostLocale;
struct IVsInvisibleEditorManager;
struct IVsRegisterProjectTypes;
struct IVsRegisterEditors;
struct IAttributeGrammar;
struct IVsObjectManager;
struct IVsMonitorSelection;
struct IVsFileChangeEx;

class VCCodeModelService;

namespace VxDTE
{
	struct _DTE;
	struct ObjectExtenders;
}

using namespace VxDTE;

class CVCUtilityLibrary
{
private :

	static CComPtr<IServiceProvider> sm_spServiceProvider;

public :
 
	static void SafeExitProcess(void);

	static HRESULT SetSite(IServiceProvider * pIServiceProvider);

	// Non-cached QS
	//

	template<typename IService>	static HRESULT QueryService(const GUID & rguidService, IService ** ppIService)
	{
		FAILED_ASSERT_RETURN(E_UNEXPECTED, sm_spServiceProvider);
		return sm_spServiceProvider->QueryService(rguidService, __uuidof(IService), reinterpret_cast<void **>(ppIService));
	}

	template<typename IService>	static HRESULT QueryService(IService ** ppIService)
	{
		FAILED_ASSERT_RETURN(E_UNEXPECTED, sm_spServiceProvider);
		return sm_spServiceProvider->QueryService(GUID_NULL, __uuidof(IService), reinterpret_cast<void **>(ppIService));
	}

	// Cached QS
	//

	template<typename IService>	static HRESULT CachedQueryService(

		const GUID &		rsidService, 
		const IID *			piidService,
		CComPtr<IService> &	spIServiceCache, 
		IService **			ppIService)

	{
		HRESULT hr(S_OK);

		if (!spIServiceCache)
		{
			FAILED_ASSERT_RETURN(E_UNEXPECTED, sm_spServiceProvider);
			FAILED_ASSERT_RETURN_HR(sm_spServiceProvider->QueryService(
				rsidService, *piidService, reinterpret_cast<void**>(&spIServiceCache)));
		}

		return spIServiceCache.CopyTo(ppIService);
	}

	static HRESULT QueryService(VCProjectEngine ** ppVCProjectEngine);
	static HRESULT QueryService(IVsUIShellOpenDocument ** ppIVsUIShellOpenDocument);
	static HRESULT QueryService(IVsSolution ** ppIVsSolution);
	static HRESULT QueryService(IVsRunningDocumentTable ** ppIVsRunningDocumentTable);
	static HRESULT QueryService(IVsLinkedUndoTransactionManager ** ppIVsLinkedUndoTransactionManager);
	static HRESULT QueryService(ILocalRegistry ** ppILocalRegistry);
	static HRESULT QueryService(IVsShell ** ppIVsShell);
	static HRESULT QueryService(IVsUIShell ** ppIVsUIShell);
	static HRESULT QueryService(_DTE ** pp_DTE);
	static HRESULT QueryService(IUIHostLocale ** ppIUIHostLocale);
	static HRESULT QueryService(IVsTextManager ** ppIVsTextManager);
	static HRESULT QueryService(IVCTextEdit ** ppIVCTextEdit);
	static HRESULT QueryService(IVsSolutionBuildManager ** ppIVsSolutionBuildManager);
	static HRESULT QueryService(IVsInvisibleEditorManager ** ppIVsInvisibleEditorManager);
	static HRESULT QueryService(ObjectExtenders ** ppObjectExtenders);
	static HRESULT QueryService(VCCodeModelService ** ppVCCodeModelService);
	static HRESULT QueryService(IAttributeGrammar ** ppIAttributeGrammar);
	static HRESULT QueryService(IVsStatusbar ** ppIVsStatusbar);
	static HRESULT QueryService(IVsRegisterProjectTypes ** ppIVsRegisterProjectTypes);
	static HRESULT QueryService(IVsRegisterEditors ** ppIVsRegisterEditors);
	static HRESULT QueryService(IVsObjectManager ** ppIVsObjectManager);
	static HRESULT QueryService(IVsMonitorSelection ** ppIVsMonitorSelection);
	static HRESULT QueryService(IVsFileChangeEx ** ppIVsFileChangeEx);
};
