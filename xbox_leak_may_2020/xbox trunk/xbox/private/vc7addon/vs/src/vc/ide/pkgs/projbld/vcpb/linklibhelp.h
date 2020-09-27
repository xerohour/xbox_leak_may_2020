
#ifndef LINKLIBHELP_H
#define LINKLIBHELP_H

class CLinkerLibrarianHelper
{
public:
	HRESULT DoGenerateOutput(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC, BOOL bIsLinker,
		IVCToolImpl* pTool);
	HRESULT DoGetPrimaryOutputFromTool(IUnknown* pItem, CPathW& rpath, BOOL bIsLinker);
	HRESULT DoHasDependencies(IVCBuildAction* pAction, BOOL bIsLinker, VARIANT_BOOL* pbHasDependencies);
	HRESULT DoGetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, VARIANT_BOOL* pbUpToDate,
		BOOL bIsLinker, BOOL bAllowDirty = TRUE);

protected:
	virtual BOOL CreateOutputName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal) PURE;
	virtual HRESULT DoHasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput) PURE;
	int GetNextDep(int nStartDep, CStringW& strDepList, int nDepLen, CStringW& strDep);
	HRESULT DoGetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, BOOL bForSave, 
		BSTR* pbstrInputs, COptionHandlerBase* poptHandler, UINT idUserDeps, UINT idProjDeps, LPCOLESTR szExtras = L"");

public:
	static void GetProjectCfgForItem(IUnknown* pItem, VCConfiguration** pProjCfg);
};

#endif // end include fence
