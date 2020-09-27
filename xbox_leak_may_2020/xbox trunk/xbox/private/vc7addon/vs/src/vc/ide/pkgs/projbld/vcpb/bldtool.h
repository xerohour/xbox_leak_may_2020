#pragma once

#include "vccolls.h"
#include "CmdLines.h"

// classes in this header
class CBldToolWrapper;

// classes needed by this header
class CBldAction;
class CBldActionList;

// action events
#define ACTEVT_Create		0x0	// action is being created
#define ACTEVT_Destroy		0x1	// action is being destroyed
#define ACTEVT_PreGenDep	0x2	// action input file dep. generate is about to start
#define ACTEVT_PostGenDep	0x3	// action input file dep. generate is finished
#define ACTEVT_QueryBrowse	0x4	// ask action if it is browsing
#define ACTEVT_SetBrowse	0x5	// ask action to set it browsing

class CBldToolWrapper
{
public:
	CBldToolWrapper(IVCToolImpl* pTool);
	~CBldToolWrapper();
	IVCToolImpl* GetVCToolImpl() { return m_spTool; }
	BOOL IsTargetTool(IVCBuildAction* pAction);
	BOOL SameToolType(IVCToolImpl* pCheckTool);
	BOOL SameToolType(CStringW& rstrToolName);
	BOOL CustomToolType();
	BOOL GetDefaultExtensions(CStringW& rstrExtensions);
	void GetToolName(IVCToolImpl* pTool, CStringW& rstrToolName);
	long GetOrder();
	BOOL PerformIfAble();
	actReturnStatus PerformBuildActions(bldActionTypes type, CBldActionList* plstActions, bldAttributes aob, 
		IVCBuildErrorContext* pEC, IVCBuildEngine* pBuildEngine);
	BOOL HasDependencies(IVCBuildAction* pAction);
	BOOL GenerateOutput(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC);
	BOOL AffectsOutput(long nPropID);
	BOOL GetCommandLineOptions(IUnknown* pItem, IVCBuildAction* pAction, CStringW& rstrOptions);
	BOOL CanScanForDependencies();
	BOOL ScanDependencies(IVCBuildAction* pBldAction, IVCBuildErrorContext* pEC, BOOL bWriteOutput);
	BOOL GetDependencies(IVCBuildAction* pBldAction, CVCStringWArray& rstrArray, BOOL* pbUpToDate = NULL);
	BOOL HasPrimaryOutput();
	BOOL IsSpecialConsumable(LPCOLESTR szPath);
	BOOL AffectsTool(long nLowPropID, long nHighPropID);
	BOOL CommandLineOptionsAreDirty(IVCBuildableItem* pItem);
	BOOL OutputsAreDirty(IVCBuildableItem* pItem);
	BOOL ClearDirtyOutputs(IVCBuildableItem* pItem);
	BOOL GetDeploymentDependencies(IVCBuildAction* pBldAction, IVCBuildStringCollection** ppDeployDeps);

protected:
	void InitToolName();

protected:
	BOOL m_fCustomTool:1;		// is this tool a kind of custom tool?
	BOOL m_fCustomToolInit:1;	// m_fCustomTool initialized?
	BOOL m_fToolNameInit:1;		// m_strToolName initialized?
	BOOL m_fCustomBuildToolNameInit:1;	// m_strCustomBuildToolName initialized?
	BOOL m_fExtensionsInit:1;	// m_strExtensions initialized?
	BOOL m_fExtensionsValid:1;	// m_strExtensions valid?
	BOOL m_fOrderInit:1;		// m_nOrder initialized?
	BOOL m_fPerformIfAbleInit:1;	// m_fPerformIfAble initialized?
	BOOL m_fPerformIfAble:1;	// perform actions even if previous errors
	BOOL m_fPrimaryOutputInit:1;	// m_fPrimaryOutput initialized?
	BOOL m_fPrimaryOutput:1;		// tool generates primary output

	CComPtr<IVCToolImpl> m_spTool;	// the underlying, config/platform/toolset tool
	CStringW m_strToolName;	// name of this tool (ex: C/C++ Compiler Tool)
	CStringW m_strExtensions;	// default extension list for this tool
	CStringW m_strCustomBuildToolName;	// name of the custom build tool
	long m_nOrder;
};

