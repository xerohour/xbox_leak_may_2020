#include "stdafx.h"
#pragma hdrstop
#include "BuildEngine.h"
#include "msgboxes2.h"
#include "scanner.h"

HRESULT CBldActionList::CreateInstance(CBldActionList **ppBldActionList)
{
	return CreateInstance(ppBldActionList, NULL);
}

HRESULT CBldActionList::CreateInstance(IVCBuildActionList **ppBldActionList)
{
	return CreateInstance(NULL, ppBldActionList);
}

HRESULT CBldActionList::CreateInstance(CBldActionList **ppBldActionList, IVCBuildActionList **ppVCBldActionList)
{
	if (ppBldActionList == NULL && ppVCBldActionList == NULL)
		RETURN_INVALID();
	
	if (ppBldActionList)
		*ppBldActionList = NULL;
	if (ppVCBldActionList)
		*ppVCBldActionList = NULL;

	HRESULT hr;
	CBldActionList *pVar;
	CComObject<CBldActionList> *pObj;
	hr = CComObject<CBldActionList>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		if (ppBldActionList)
			*ppBldActionList = pObj;
		if (ppVCBldActionList)
			*ppVCBldActionList = pVar;
	}
	return hr;
}

// gets the next celt elements; cookie is optional
STDMETHODIMP CBldActionList::Next(IVCBuildAction** ppAction, void** ppPos)
{
	long lMax = (long) GetCount();
	HRESULT hr = S_FALSE;

	if (lMax > 0 && m_posEnumerator != NULL)
	{
		VCPOSITION oldPos = m_posEnumerator;
		CBldAction* pAction = (CBldAction*)GetNext(m_posEnumerator);
		*ppAction = (IVCBuildAction *)(pAction);
		if (*ppAction)
			(*ppAction)->AddRef();
		if (ppPos != NULL)
			*ppPos = (void *)oldPos;
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CBldActionList::get_Count(long* pceltFetched)
{
	CHECK_POINTER_NULL(pceltFetched);
	*pceltFetched = (long) GetCount();
	return S_OK;
}

// reset the enumeration
STDMETHODIMP CBldActionList::Reset(void** ppPos)
{
	m_posEnumerator = GetHeadPosition();
	if (ppPos != NULL)
		*ppPos = (void *)m_posEnumerator;
	return S_OK;
}

// clone the enumerator
STDMETHODIMP CBldActionList::Clone(IEnumVARIANT FAR* FAR* ppenum)
{
	return E_NOTIMPL; // don't have capability for clone and don't expect to, either
}

// Try to find a tool in an action in our list
STDMETHODIMP CBldActionList::Find(IVCToolImpl *pTool, IVCBuildAction **ppBldAction)
{
	VCPOSITION pos = GetHeadPosition();
	CStringW strToolName;
	while (pos != (VCPOSITION)NULL)
	{
		VCPOSITION posCurr = pos;
		CBldAction* pAction = (CBldAction *)GetNext(pos);
		if (strToolName.IsEmpty() && pAction->BuildToolWrapper())
			pAction->BuildToolWrapper()->GetToolName(pTool, strToolName);
		if ((pAction->BuildToolWrapper() && pAction->BuildToolWrapper()->SameToolType(strToolName)) ||
			(pAction->BuildToolWrapper() == NULL && pTool == NULL))
		{
			if (ppBldAction)
			{
				IVCBuildAction* pVCAction = (IVCBuildAction*)pAction;
				pVCAction->AddRef();
				*ppBldAction = pVCAction;
			}
			return S_OK;
		}
	}

	return S_FALSE;
}

STDMETHODIMP CBldActionList::FindByName(BSTR bstrName, IVCBuildAction** ppBldAction)
{
	CStringW strToolName = bstrName;

	VCPOSITION pos = GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		VCPOSITION posCurr = pos;
		CBldAction* pAction = (CBldAction *)GetNext(pos);
		if ((pAction->BuildToolWrapper() && pAction->BuildToolWrapper()->SameToolType(strToolName)) ||
			(pAction->BuildToolWrapper() == NULL && strToolName.IsEmpty()))
		{
			if (ppBldAction)
			{
				IVCBuildAction* pVCAction = (IVCBuildAction*)pAction;
				pVCAction->AddRef();
				*ppBldAction = pVCAction;
			}
			return S_OK;
		}
	}

	return S_FALSE;
}

STDMETHODIMP CBldActionList::get_LastAction(IVCBuildAction** ppAction)
{
	CHECK_POINTER_NULL(ppAction);
	*ppAction = NULL;

	CBldAction* pBldAction = (CBldAction *)GetTail();
	if (pBldAction)
	{
		*ppAction = pBldAction;
		(*ppAction)->AddRef();
	}

	return (*ppAction != NULL) ? S_OK : S_FALSE;
}

STDMETHODIMP CBldActionList::get_ProjectConfigurationDependencies(IVCBuildStringCollection** ppDependencies)	// dependencies of this action i.e., libs, etc.
{
	CHECK_POINTER_NULL(ppDependencies);
	*ppDependencies = NULL;

	Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = Next(&spAction, NULL);
		if (hr != S_OK)
			break;
		CONTINUE_ON_NULL(spAction);
		VARIANT_BOOL bIsTargetAction = VARIANT_FALSE;
		hr = spAction->get_IsTargetAction(&bIsTargetAction);
		CONTINUE_ON_FAIL(hr);
		if (bIsTargetAction == VARIANT_TRUE)
		{
			CComPtr<IVCToolImpl> spToolImpl;
			spAction->get_VCToolImpl(&spToolImpl);
			if (spToolImpl != NULL)
			{
				VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
				spToolImpl->HasPrimaryOutputFromTool(NULL, VARIANT_TRUE, &bHasPrimaryOutput);
				if (bHasPrimaryOutput == VARIANT_TRUE)
					return spAction->get_Dependencies(ppDependencies);
			}
		}
	}

	return S_FALSE;
}

STDMETHODIMP CBldActionList::get_ProjectConfigurationBuildOutputs(IVCBuildStringCollection** ppOutputs)		// outputs of this action i.e., foo.exe, foo.pdb, etc.
{
	CHECK_POINTER_NULL(ppOutputs);
	*ppOutputs = NULL;

	Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = Next(&spAction, NULL);
		if (hr != S_OK)
			break;
		CONTINUE_ON_NULL(spAction);
		VARIANT_BOOL bIsTargetAction = VARIANT_FALSE;
		hr = spAction->get_IsTargetAction(&bIsTargetAction);
		CONTINUE_ON_FAIL(hr);
		if (bIsTargetAction == VARIANT_TRUE)
		{
			CComPtr<IVCToolImpl> spToolImpl;
			spAction->get_VCToolImpl(&spToolImpl);
			if (spToolImpl != NULL)
			{
				VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
				spToolImpl->HasPrimaryOutputFromTool(NULL, VARIANT_TRUE, &bHasPrimaryOutput);
				if (bHasPrimaryOutput == VARIANT_TRUE)
					return spAction->get_BuildOutputs(ppOutputs);
			}
		}
	}

	return S_FALSE;
}

STDMETHODIMP CBldActionList::RefreshOutputs(long nPropID, IVCBuildErrorContext* pEC)
{
	HRESULT hrFinal = S_FALSE;
	Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = Next(&spAction, NULL);
		if (hr != S_OK)
			break;
		CONTINUE_ON_NULL(spAction);
		if (spAction->RefreshOutputs(nPropID, pEC) == S_OK)
			hrFinal = S_OK;
	}

	return hrFinal;
}

STDMETHODIMP CBldActionList::HandlePossibleCommandLineOptionsChange(IVCBuildErrorContext* pEC)
{
	HRESULT hrFinal = S_FALSE;
	Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = Next(&spAction, NULL);
		if (hr != S_OK)
			break;
		CONTINUE_ON_NULL(spAction);
		if (spAction->HandlePossibleCommandLineOptionsChange(pEC) == S_OK)
			hrFinal = S_OK;
	}

	return hrFinal;
}

// Try to find a tool in an action in our list
VCPOSITION CBldActionList::Find(CBldToolWrapper* pTool)
{
	VCPOSITION pos = GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		VCPOSITION posCurr = pos;
		CBldAction* pAction = (CBldAction *)GetNext(pos);
		if (pTool == pAction->BuildToolWrapper())
			return posCurr;
	}

	return (VCPOSITION)NULL;
}

STDMETHODIMP CBldActionList::Add(IVCBuildAction *pVCAction)
{
	CHECK_POINTER_NULL(pVCAction);

	CBldAction* pAction = NULL;
	pVCAction->get_ThisPointer((void **)&pAction);
	VSASSERT(pAction != NULL, "Attempting to add NULL or bad object to action list.");
	RETURN_ON_NULL2(pAction, E_OUTOFMEMORY);

	pVCAction->AddRef();
	AddTail(pAction);
	return S_OK;
}

STDMETHODIMP CBldActionList::RemoveAtPosition(void* pPos)
{
	VCPOSITION pos = (VCPOSITION)pPos;
	RETURN_ON_NULL2(pos, S_FALSE);

	CBldAction* pAction = (CBldAction *)GetAt(pos);
	if (pAction != NULL)
	{
		IVCBuildAction* pVCAction = pAction;
		pVCAction->Release();
	}
	RemoveAt(pos);

	return S_OK;
}

STDMETHODIMP CBldActionList::FreeOutputFiles()
{
	return E_NOTIMPL;	// CBldActionList::FreeOutputFiles -- possible optimization point
}

STDMETHODIMP CBldActionList::get_Project(VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;
	if (IsEmpty())
		return S_FALSE;

	VCPOSITION pos = GetHeadPosition();
	CBldAction* pAction = (CBldAction *)GetNext(pos);
	if (pAction == NULL)
	{
		VSASSERT(FALSE, "Unexpected action in action list");
		return E_UNEXPECTED;
	}

	return pAction->get_Project(ppProject);
}

void CBldActionList::Clear(BOOL bDoRelease /* = TRUE */)
{
	while (!IsEmpty())
	{
		CBldAction* pAction = (CBldAction*)RemoveHead();
		if (pAction != NULL && bDoRelease)
		{
			IVCBuildAction* pVCAction = (IVCBuildAction*)pAction;
			pVCAction->Release();
		}
	}
}

void CBldActionList::AddToEnd(CBldAction* pAction, BOOL bDoAddRef /* = TRUE */)
{
	if (pAction == NULL)
		return;

	if (bDoAddRef)
	{
		IVCBuildAction* pVCAction = (IVCBuildAction *)pAction;
		pVCAction->AddRef();
	}
	AddTail(pAction);
}

void CBldActionList::AddToEnd(CVCPtrList* pActions, BOOL bDoAddRef /* = TRUE */)
{
	if (pActions == NULL)
		return;

	VCPOSITION pos = pActions->GetHeadPosition();
	while (pos != NULL)
	{
		CBldAction* pAction = (CBldAction *)pActions->GetNext(pos);
		AddToEnd(pAction, bDoAddRef);
	}
}

void CBldActionList::AddToEnd(CBldActionList* pActions, BOOL bDoAddRef /* = TRUE */)
{
	if (pActions == NULL)
		return;

	VCPOSITION pos = pActions->GetHeadPosition();
	while (pos != NULL)
	{
		CBldAction* pAction = (CBldAction *)pActions->GetNext(pos);
		AddToEnd(pAction, bDoAddRef);
	}
}

CComBSTR CBldAction::s_bstrDefVCFile = L"vc70";
LPCOLESTR CBldAction::s_szDefVCFile = L"vc70";

// our action marker
WORD CBldAction::m_wActionMarker = 0;
BOOL CBldAction::s_bIgnoreCachedOptions = TRUE;
int CBldAction::s_nMyMarker = 0;


CBldAction::CBldAction()
{
	// default
	m_pIncludeList = (CBldIncludeEntryList *)NULL;
	m_fBatchNotify = m_fInGenOutput = FALSE;
	m_wMark = 0; 
	m_pSets = NULL;

	m_pTool = m_pOldTool = (CBldToolWrapper *)NULL;
	m_pTrgAction = (CBldAction *)NULL;
	m_frhInput = (BldFileRegHandle)NULL;
	m_pcrOur = NULL;

	// assume that our settings are ok, but dirty
	m_fDirtyCommand = TRUE;
	m_fPossibleOptionChange = FALSE;
	m_fCmdOptChanged = FALSE;
	m_fDepMissing = FALSE;

	m_wMarkRefCmdOpt = 0;

	m_LastDepUpdate.dwLowDateTime = 0;
	m_LastDepUpdate.dwHighDateTime = 0;
}

void CBldAction::Initialize(VCFileConfiguration* pFileCfg, CBldToolWrapper* pTool, CBldCfgRecord* pcr, 
	BOOL fIsProject, BOOL bNoFilter)
{
	// associate
	m_pcrOur = pcr;
	VSASSERT(m_pcrOur != NULL, "Must specify CfgRecord for an action");
	m_fIsProject = fIsProject;

	CComQIPtr<IVCBuildableItem> spItem;
	if (pFileCfg)
		spItem = pFileCfg;
	else 
		spItem = pcr->GetVCConfiguration();
	m_pItem = spItem.Detach();	
	m_pItem->Release(); // yes, intentionally dropping ref-count to be zero change from when we came in

	// registry & dep.graph??
	m_pregistry = g_StaticBuildEngine.GetRegistry(m_pcrOur);
	m_pdepgraph = g_StaticBuildEngine.GetDepGraph(m_pcrOur);

	// bind to our tool?
	if (pTool != (CBldToolWrapper *)NULL)
		Bind(pTool, NULL, bNoFilter);
}

HRESULT CBldAction::CreateInstance(VCFileConfiguration* pFileCfg, CBldToolWrapper* pTool, 
	CBldCfgRecord* pcr, BOOL bIsProject, BOOL bNoFilter, IVCBuildAction** ppBldAction)
{
	CHECK_POINTER_NULL(ppBldAction);
	*ppBldAction = NULL;

	CComObject<CBldAction> *pObj;
	HRESULT hr = CComObject<CBldAction>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		CBldAction *pVar = pObj;
		pVar->AddRef();
		*ppBldAction = pVar;
		pObj->Initialize(pFileCfg, pTool, pcr, bIsProject, bNoFilter);
	}
	return hr;
}

CBldAction::~CBldAction()
{
	// clean up our source, missing and scanned deps.
	ClearSourceDep();
	ClearScannedDep();

	// unbind from the tool?
	if (m_pTool != (CBldToolWrapper *)NULL)
	{
		// remember if we had a tool so we can use this info. after UnBind()
		CBldToolWrapper* pTool = m_pTool;
		UnBind();
	}
	if (m_pSets)
		delete m_pSets;
}

STDMETHODIMP CBldAction::MatchesOldTool(IVCToolImpl *pTool, BOOL *pbMatches)
{
	CHECK_POINTER_NULL(pbMatches);
	if (pTool == NULL && m_pTool == NULL)
		*pbMatches = TRUE;
	else if (pTool == NULL || m_pTool == NULL)
		*pbMatches = FALSE;
	else
		*pbMatches = m_pTool->SameToolType(pTool);

	return S_OK;
}

STDMETHODIMP CBldAction::get_VCToolImpl(IVCToolImpl **pVal)
{
	CHECK_POINTER_NULL(pVal);

	if (m_pTool == NULL)
		*pVal = NULL;
	else
	{
		*pVal = m_pTool->GetVCToolImpl();
		(*pVal)->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CBldAction::get_Project(VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);

	VSASSERT(m_pcrOur != NULL, "Action was not initialized correctly; missing CfgRecord pointer");
	CHECK_ZOMBIE(m_pcrOur, IDS_ERR_CFG_ZOMBIE);

	VCConfiguration* pProjCfg = m_pcrOur->GetVCConfiguration();
	CHECK_ZOMBIE(pProjCfg, IDS_ERR_CFG_ZOMBIE);

	CComPtr<IDispatch> pDispProject;
	HRESULT hr = pProjCfg->get_Project(&pDispProject);
	CComQIPtr<VCProject> spProject = pDispProject;
	RETURN_ON_FAIL(hr);
	CHECK_ZOMBIE(spProject, IDS_ERR_CFG_ZOMBIE);

	spProject.CopyTo(ppProject);
	return S_OK;
}

STDMETHODIMP CBldAction::get_FileConfiguration(VCFileConfiguration** ppFileCfg)
{
	CHECK_POINTER_NULL(ppFileCfg);
	*ppFileCfg = NULL;

	m_pItem->QueryInterface(__uuidof(VCFileConfiguration), (LPVOID *)ppFileCfg);
	return ((*ppFileCfg) != NULL) ? S_OK : S_FALSE;
}

STDMETHODIMP CBldAction::get_ProjectConfiguration(VCConfiguration** ppProjCfg)
{
	CHECK_POINTER_NULL(ppProjCfg);

	VSASSERT(m_pcrOur != NULL, "Action was not initialized correctly; missing CfgRecord pointer");
	CHECK_ZOMBIE(m_pcrOur, IDS_ERR_CFG_ZOMBIE);

	*ppProjCfg = m_pcrOur->GetVCConfiguration();
	(*ppProjCfg)->AddRef();
	return S_OK;
}

STDMETHODIMP CBldAction::get_ItemFilePath(BSTR* pbstrFilePath)
{
	CHECK_POINTER_NULL(pbstrFilePath);
	return m_pItem->get_ItemFileName(pbstrFilePath);
}

STDMETHODIMP CBldAction::get_Item(IVCBuildableItem** ppItem)
{
	CHECK_POINTER_NULL(ppItem);
	*ppItem = m_pItem;
	if (m_pItem)
		(*ppItem)->AddRef();
	return S_OK;
}

STDMETHODIMP CBldAction::get_PropertyContainer(IVCPropertyContainer** ppPropContainer)
{
	CHECK_POINTER_NULL(ppPropContainer);
	CComQIPtr<IVCPropertyContainer> spPropContainer = m_pItem;
	return spPropContainer.CopyTo(ppPropContainer);
}

STDMETHODIMP CBldAction::get_ThisPointer(void** ppThis)
{
	CHECK_POINTER_NULL(ppThis);
	*ppThis = (void *)this;
	return S_OK;
}

STDMETHODIMP CBldAction::get_IsTargetAction(VARIANT_BOOL* pbIsTargetAction)
{
	CHECK_POINTER_NULL(pbIsTargetAction);

	if (m_pTool == NULL)
	{
		*pbIsTargetAction = VARIANT_FALSE;
		return S_FALSE;
	}

	*pbIsTargetAction = m_pTool->IsTargetTool(this) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBldAction::GetOutput(IVCBuildErrorContext* pEC, void** ppOutput)
{
	CHECK_POINTER_NULL(ppOutput);

	CBldFileRegSet* pRegSet = GetOutput(pEC);
	*ppOutput = (void *)pRegSet;
	return S_OK;
}

STDMETHODIMP CBldAction::get_Registry(void** ppFileRegistry)
{
	CHECK_POINTER_NULL(ppFileRegistry);
	*ppFileRegistry = (void *)m_pregistry;
	return S_OK;
}

STDMETHODIMP CBldAction::InitSets()
{
	m_pSets = new CBldActionSets(this);
	return S_OK;
}

STDMETHODIMP CBldAction::AddSourceDependencyFromString(BSTR bstrDep, IVCBuildErrorContext* pEC)
{
	CStringW strDep = bstrDep;
	return AddSourceDep(strDep, pEC) ? S_OK : S_FALSE;
}

STDMETHODIMP CBldAction::AddSourceDependencyFromFrh(void* pFileRegHandle, IVCBuildErrorContext* pEC)
{
	BldFileRegHandle frh = (BldFileRegHandle)pFileRegHandle;
	return AddSourceDep(frh, pEC) ? S_OK : S_FALSE;
}

STDMETHODIMP CBldAction::AddScannedDependencyFromString(BSTR bstrDep, IVCBuildErrorContext* pEC)
{
	CStringW strDep = bstrDep;
	return AddScannedDep(strDep, pEC) ? S_OK : S_FALSE;
}

STDMETHODIMP CBldAction::AddScannedDependencyFromFrh(void *pDep, IVCBuildErrorContext* pEC)
{
	BldFileRegHandle frh = (BldFileRegHandle)pDep;
	return AddScannedDep(frh, pEC) ? S_OK : S_FALSE;
}

STDMETHODIMP CBldAction::AddScannedDependencyOfType(int type, BSTR bstrFile, int line, BOOL fScan, IVCBuildErrorContext* pEC)
{
	CStringW strFile = bstrFile;
	return AddScannedDep(type, pEC, strFile, line, fScan) ? S_OK : S_FALSE;
}

STDMETHODIMP CBldAction::ClearScannedDependencies(IVCBuildErrorContext* pEC)
{
	ClearScannedDep(pEC);
	return S_OK;
}

STDMETHODIMP CBldAction::SetDependencyState(enumDependencyState depState)
{
	switch (depState)
	{
	case depNone:
		SetDepState(DEP_None);
		break;
	case depEmpty:
		SetDepState(DEP_Empty);
		break;
	default:
		RETURN_INVALID();
	}
	return S_OK;
}

STDMETHODIMP CBldAction::MarkMissingDependencies(BOOL bHaveMissing)
{
	m_fDepMissing = bHaveMissing;
	return S_OK;
}

STDMETHODIMP CBldAction::UpdateDependencyInformationTimeStamp()
{
	UpdateDepInfoTimeStamp();
	return S_OK;
}

STDMETHODIMP CBldAction::get_Dependencies(IVCBuildStringCollection** ppDependencies)	// dependencies of this action i.e., libs, headers, etc.
{
	CHECK_POINTER_NULL(ppDependencies);
	*ppDependencies = NULL;

	CBldFileRegSet* pScannedDeps = GetScannedDep();
	CBldFileRegSet* pSourceDep = GetSourceDep();
	if (pScannedDeps == NULL && pSourceDep == NULL)
		return S_FALSE;	// nothing to do

	CVCBuildStringCollection* pStringCol;
	HRESULT hr = CVCBuildStringCollection::CreateInstance(ppDependencies, &pStringCol);
	RETURN_ON_FAIL(hr);

	if (pScannedDeps != NULL)
		LoadDepArray(pScannedDeps, pStringCol->m_strStrings);
	if (pSourceDep != NULL)
		LoadDepArray(pSourceDep, pStringCol->m_strStrings);

	if (pStringCol->m_strStrings.GetSize() == 0)	// no deps, so release our collection...
	{
		(*ppDependencies)->Release();
		*ppDependencies = NULL;
		return S_FALSE;
	}

	return S_OK;
}

void CBldAction::LoadDepArray(CBldFileRegSet* pDeps, CVCStringWArray& rstrDepsArray)
{
	BldFileRegHandle frh;
	pDeps->InitFrhEnum();
	while ((frh = pDeps->NextFrh()) != (BldFileRegHandle)NULL)
	{
		if (frh->IsNodeType(nodetypeRegSet))
			LoadDepArray((CBldFileRegSet *)frh, rstrDepsArray);
		else if (frh->IsNodeType(nodetypeRegFile))
		{
			const CPathW* pPath = frh->GetFilePath();
			if (pPath != NULL)
			{
				CStringW strPath = *pPath;
				rstrDepsArray.Add(strPath);
			}
		}
		frh->ReleaseFRHRef();
	}
}

STDMETHODIMP CBldAction::GetDeploymentDependencies(IVCBuildStringCollection** ppDeployDeps)
{
	if (m_pTool)
		m_pTool->GetDeploymentDependencies(this, ppDeployDeps);
	return S_OK;
}

STDMETHODIMP CBldAction::get_BuildOutputs(IVCBuildStringCollection** ppOutputs)		// outputs of this action i.e., foo.exe, foo.pdb, foo.obj, etc.
{
	CHECK_POINTER_NULL(ppOutputs);
	*ppOutputs = NULL;

	CBldFileRegSet* pregset = &m_pSets->m_frsOutput;
	RETURN_ON_NULL2(pregset, S_FALSE);

	CVCBuildStringCollection* pStringCol;
	HRESULT hr = CVCBuildStringCollection::CreateInstance(ppOutputs, &pStringCol);
	RETURN_ON_FAIL(hr);

	LoadDepArray(pregset, pStringCol->m_strStrings);
	return S_OK;
}

STDMETHODIMP CBldAction::GetFileNameForFileRegHandle(void* pRegHandle, BSTR* pbstrFileName)
{
	CHECK_POINTER_NULL(pbstrFileName);
	*pbstrFileName = NULL;
	RETURN_ON_NULL2(pRegHandle, S_FALSE);

	BldFileRegHandle frh = (BldFileRegHandle)pRegHandle;
	const CPathW* pPath = frh->GetFilePath();
	RETURN_ON_NULL2(pPath, S_FALSE);

	CStringW strName;
	pPath->GetFullPath(strName);

	*pbstrFileName = strName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBldAction::GetFileRegHandleForFileName(BSTR bstrFileName, void** ppRegHandle)
{
	CHECK_POINTER_NULL(ppRegHandle);
	CStringW strFile = bstrFileName;
	if (strFile.IsEmpty())
		RETURN_INVALID();

	BldFileRegHandle frh = CBldFileRegFile::LookupFileHandleByName(strFile, TRUE);
	*ppRegHandle = (void *)frh;
	return S_OK;
}

STDMETHODIMP CBldAction::ReleaseRegHandle(void* pRegHandle)
{
	RETURN_ON_NULL2(pRegHandle, S_FALSE);

	BldFileRegHandle frh = (BldFileRegHandle)pRegHandle;
	frh->ReleaseFRHRef();
	return S_OK;
}

// Binding and un-binding to tools
void CBldAction::Bind(CBldToolWrapper* pTool, IVCBuildErrorContext* pEC /* = NULL */, BOOL bNoFilter /* = FALSE */)
{
	// Set our tool
	SetToolI(pTool, bNoFilter);

	// update our output
	if (m_pdepgraph && m_pdepgraph->IsInitialized())
		LateBind(pEC);
}

// Binding and un-binding to tools
HRESULT CBldAction::LateBind(IVCBuildErrorContext* pEC)
{
	// update our output
	if (m_pTool)
	{
		if (m_pSets == NULL)
			InitSets();
		RefreshOutputs(0, pEC);
	}

	return S_OK;
}

void CBldAction::UnBind()
{
	ClearSourceDep(NULL);	// un-bind our source deps
	ClearScannedDep(NULL);	// nuke our other deps

	// clean up our input & output file/set references
	ClearOutput(NULL);
	ClearInput();

	SetToolI((CBldToolWrapper *)NULL);	// set our tool					   
}

// Notification of input or output changing
// Rx for file change events
void CBldAction::OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint)
{
	// interested in this?
	if (!(idChange == FRI_ADD || idChange == FRI_DESTROY))
		return;	// no

	// ignore due to batching?
	if (m_fBatchNotify)
		return;	// yes
												
	// convert hint into a file registry handle
	BldFileRegHandle frhHint = (BldFileRegHandle)pHint;
									    
	// must be concerning our 'possible' input!
	VSASSERT(hndFileReg == m_frhInput, "receiving notification about something we do not care about!");

	// valid input?
	if (m_pTool && m_pTool->IsTargetTool(this) && !ValidInput(frhHint))
		return;

	// add/remove these input dependencies to/from the graph
	if (m_pdepgraph != NULL)
	{
		BOOL bOK = m_pdepgraph->ModifyDep(frhHint, this, DEP_Input, idChange == FRI_ADD /* add or remove? */, pEC);
		VSASSERT(bOK, "Failed to modify dependency graph!");
	}

	// dirty input
	m_fDirtyInput = TRUE;
}

// replaces OnInform for those few things that need it...
STDMETHODIMP CBldAction::RefreshOutputs(long nPropID, IVCBuildErrorContext* pEC)
{
	// don't bother with output changes 'til there's a dep graph.
	RETURN_ON_NULL2(m_pSets, S_OK);
		
	if (nPropID != 0)	// only refresh if we need to
	{
		if (m_pTool && !m_pTool->AffectsOutput(nPropID))
			return S_OK;
	}

	RefreshOutputI(pEC);

	HRESULT hrFinal = S_FALSE;
	if (nPropID == 0)	// on a force like this, always say to cascade changes through if needed
		hrFinal = S_OK;
	else if (m_pTool && !m_pTool->IsTargetTool(this))
		hrFinal = S_OK;

	return hrFinal;
}

STDMETHODIMP CBldAction::HandlePossibleCommandLineOptionsChange(IVCBuildErrorContext* pEC)
{
	RETURN_ON_NULL2(m_pTool, S_FALSE);	// nothing to bother with here

	if (m_pTool->CommandLineOptionsAreDirty(m_pItem))
	{
		m_fPossibleOptionChange = TRUE;
		if (m_pTool->OutputsAreDirty(m_pItem))
		{
			RefreshOutputs(0, pEC);
			m_pTool->ClearDirtyOutputs(m_pItem);
		}
		return S_OK;
	}

	return S_FALSE;		// not for us
}

void CBldAction::AddInput(BOOL bNoFilter /* = FALSE */)
{
	// no tool, no input to add!
	if (m_pTool == (CBldToolWrapper *)NULL)
		return;

	// create the input set or file
	if (m_pTool->IsTargetTool(this))
	{
		// 'possible' input is a file filter
		CStringW strFilter;
		if (m_pTool->GetDefaultExtensions(strFilter))
			m_frhInput = m_pregistry->RegisterFileFilter((const CStringW *)&strFilter);
	}
	else
	{
		CComBSTR bstrPath;
		// 'possible' input is the file itself
		if (m_fIsProject)
		{
			VSASSERT(m_pcrOur != NULL, "Action was not initialized correctly; missing CfgRecord pointer");
			VCConfiguration* pCfg = m_pcrOur->GetVCConfiguration();
			if (pCfg)
				pCfg->get_PrimaryOutput(&bstrPath);
		}
		else
		{
			CComPtr<IDispatch> pDispFile;
			VSASSERT(m_pItem != NULL, "Action was not initialized correctly; missing BuildableItem pointer");
			if (m_pItem != NULL)
			{
				CComQIPtr<VCFileConfiguration> spFileCfg = m_pItem;
				if (spFileCfg != NULL && SUCCEEDED(spFileCfg->get_File(&pDispFile)) && pDispFile != NULL)
				{
					CComQIPtr<VCFile> pFile = pDispFile;
					if (pFile != NULL)
						pFile->get_FullPath(&bstrPath);
				}
			}
		}

		CStringW strPath = bstrPath;
		if (!strPath.IsEmpty())
		{
			CPathW Path;
			// both get_PrimaryOutput and get_FullPath already verify that this path is valid and actual case
			if (Path.CreateFromKnown(strPath))
				m_frhInput = m_pregistry->RegisterFile(&Path, TRUE, bNoFilter);
		}
	}
}

void CBldAction::RemoveInput()
{
	// no tool or no input to remove?
	if (m_pTool == (CBldToolWrapper*)NULL || m_frhInput == (BldFileRegHandle)NULL)
		return;

	// clean up our 'possible' input file/set references
	if (m_frhInput->IsNodeType(nodetypeRegFile))
		m_pregistry->ReleaseRegRef(m_frhInput);
	else
		m_frhInput->ReleaseFRHRef();
	m_frhInput=NULL;
}

BOOL CBldAction::ValidInput(BldFileRegHandle frhIn)
{
	if (m_pSets)
		return !m_pSets->m_frsOutput.RegHandleExists(frhIn);	// ok?
	return FALSE;
}

// FUTURE: remove this
void CBldAction::SetToolI(CBldToolWrapper* pTool, BOOL bNoFilter /* = FALSE */)
{
	// DIANEME_TODO (CBldAction::SetToolI): need to figure out way to make ourselves dependent on the outputs of this tool...
	if (m_pTool != (CBldToolWrapper*)NULL)
	{
		// make sure we *don't* get input change events
		if (m_frhInput != (BldFileRegHandle)NULL)
			m_frhInput->RemoveNotifyRx(this);

		// remove the input for the old tool
 		RemoveInput();
	}
								   
	// add our input for this new tool?
	m_pOldTool = m_pTool;
	m_pTool = pTool;

	if (m_pTool != (CBldToolWrapper*)NULL)
	{
		AddInput(bNoFilter);

		// make sure we get 'possible' input and output change events
		if (m_frhInput != (BldFileRegHandle)NULL)
			m_frhInput->AddNotifyRx(this, TRUE);
	}

	// input and output is dirty
	m_fDirtyInput = TRUE;
	m_fDirtyOutput = TRUE;
}

BOOL CBldAction::IsBuildableConfiguration(IVCConfigurationImpl* pProjCfgImpl)
{
	RETURN_ON_NULL2(pProjCfgImpl, FALSE);
	VARIANT_BOOL bIsBuildable = VARIANT_FALSE;
	return (SUCCEEDED(pProjCfgImpl->get_IsBuildable(&bIsBuildable)) && (bIsBuildable == VARIANT_TRUE));
}

BOOL CBldAction::IsValidConfiguration(IVCConfigurationImpl* pProjCfgImpl)
{
	RETURN_ON_NULL2(pProjCfgImpl, FALSE);
	VARIANT_BOOL bIsValid = VARIANT_FALSE;
	return (SUCCEEDED(pProjCfgImpl->get_IsValidConfiguration(&bIsValid)) && 
		(bIsValid == VARIANT_TRUE));
}

void CBldAction::AssignActions(CBldCfgRecord* pcr, VCFileConfiguration* pFileCfg, 
	BOOL fAssignContained /* = TRUE */, BOOL fAssignContainedOnly /* = FALSE */)
{
	VSASSERT(pcr != NULL, "Cannot assign an action where we do not know the associated CfgRecord");
	if (pcr == NULL)
		return;

	VCConfiguration* pProjCfg = pcr->GetVCConfiguration();	// not ref-counted
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = pProjCfg;
	if (spProjCfgImpl == NULL)
		return;

	// valid?
	if (!IsValidConfiguration(spProjCfgImpl))
		return;

	// Buildable configuration?
	if (!IsBuildableConfiguration(spProjCfgImpl))
		return;

	if (!fAssignContainedOnly)
	{
		CBldConfigurationHolder cfgHolder(pcr, pFileCfg, pProjCfg);
		AssignSingleAction(cfgHolder);
	}

	BOOL bIsProject = (pFileCfg == NULL);
	if (!fAssignContained || !bIsProject)
		return;		// all done

	CComPtr<IEnumVARIANT> pFiles;
	HRESULT hr = GetFileCollection(pProjCfg, &pFiles);
	VOID_RETURN_ON_FAIL_OR_NULL(pFiles, hr);

	while (TRUE)
	{
		CComVariant var;
		hr = pFiles->Next(1, &var, NULL);
		BREAK_ON_DONE(hr);
		CComPtr<VCFileConfiguration> pFileCfg;
		if (!GetFileCfgForProjCfg(var, pProjCfg, &pFileCfg))
			continue;

		CBldConfigurationHolder cfgHolder(pcr, pFileCfg, pProjCfg);
		AssignSingleAction(cfgHolder);
	}
}

void CBldAction::AssignSingleAction(CBldConfigurationHolder& cfgHolder)
{
	BldFileRegHandle frh = cfgHolder.GetFileRegHandle();
	VARIANT_BOOL bExcluded = VARIANT_FALSE;

	if (!cfgHolder.IsProject() && SUCCEEDED(cfgHolder.GetFileCfg()->get_ExcludedFromBuild(&bExcluded)) && 
		(bExcluded == VARIANT_FALSE))
	{
		VSASSERT(NULL != cfgHolder.GetPCR(), "Trying to assign an action without a valid CfgRecord");
		// Add to dependency graph as possible input to schmooze tool.
		CBldFileRegistry *preg = g_StaticBuildEngine.GetRegistry(cfgHolder.GetPCR());
		VSASSERT(NULL != preg, "No file registry available for the CfgRecord.  Big trouble.");
		VSASSERT(NULL != frh, "No frh associated with this file.  Big trouble.");
		if( preg )
		    preg->AddRegRef(frh);
	}

	// Get the list of actions; put these actions on the configuration
	CComPtr<IVCBuildActionList> pActions;
	cfgHolder.GetActionList(&pActions);

	// Go through them and attach any to this item that want to be
	CBldToolWrapper * pTool = NULL;
	CVCPtrList* pToolList = cfgHolder.GetToolList();
	VCPOSITION pos = pToolList == NULL ? NULL : pToolList->GetHeadPosition();

	// 	No need to loop through tools.
	CComQIPtr<IVCToolImpl> pVCTool;
	CStringW strToolName;
	if (!cfgHolder.IsProject())
	{
		if (NULL == frh)
			pos = NULL;
		else
		{
			long cActions = 0;
			if (SUCCEEDED(pActions->get_Count(&cActions))  && (cActions > 0))
				pos = NULL;
		}
		if (cfgHolder.GetFileCfg())
		{
			CComPtr<IDispatch> spDispTool;
			cfgHolder.GetFileCfg()->get_Tool(&spDispTool);		// this will pick up default, if any, if nothing set
			pVCTool = spDispTool;
			if (pVCTool)
			{
				CComBSTR bstrToolName;
				pVCTool->get_ToolShortName(&bstrToolName);
				strToolName = bstrToolName;
			}
		}
	}

	// Check to see if forced to a different tool than the default.
	BOOL bFound = FALSE;

	if (pVCTool)
	{
		while (pos != (VCPOSITION)NULL)
		{
			pTool = (CBldToolWrapper *)pToolList->GetNext(pos);
			if (pTool->SameToolType(strToolName))
			{
				bFound = TRUE;
				break;
			}

		}
		if(bFound)
		{
			CComPtr<IVCBuildAction> pAction;
			CBldAction::CreateInstance(cfgHolder.GetFileCfg(), pTool, cfgHolder.GetPCR(), cfgHolder.IsProject(), 
				(bExcluded == VARIANT_TRUE), &pAction);

			// check for multiple assignment
			VSASSERT(pActions->Find(pTool->GetVCToolImpl(), NULL) == S_FALSE, "Cannot have multiple assignments of same tool to same object.");

			pActions->Add(pAction);
			pAction->RefreshOutputs(0, NULL);
		}
	}
	else if (cfgHolder.IsProject())
	{
		while (pos != (VCPOSITION)NULL)
		{
			pTool = (CBldToolWrapper *)pToolList->GetNext(pos);
			
			if (!pTool->IsTargetTool(NULL))
				continue;

			if (pActions->Find(pTool->GetVCToolImpl(), NULL) == S_OK)
				continue;	// already have one

			// allocate a 'building' action to list
			CComPtr<IVCBuildAction> pAction;
			CBldAction::CreateInstance(cfgHolder.GetFileCfg(), pTool, cfgHolder.GetPCR(), cfgHolder.IsProject(), 
				FALSE, &pAction);

			pActions->Add(pAction);
			pAction->RefreshOutputs(0, NULL);
		}
	}
}

void CBldAction::UnAssignActions(CBldCfgRecord* pcr, VCFileConfiguration* pFileCfg, 
	BOOL fUnassignContained /* = TRUE */)
{
	VSASSERT(pcr != NULL, "Trying to unassign action where CfgRecord is not known.  How could we manage to get here, even?");
	VCConfiguration* pProjCfg = pcr->GetVCConfiguration();	// not ref-counted
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = pProjCfg;
	if (spProjCfgImpl == NULL)
		return;

	// valid?
	if (!IsValidConfiguration(spProjCfgImpl))
		return;

	// Buildable configuration?
	if (!IsBuildableConfiguration(spProjCfgImpl))
		return;

	{	// unassign the item we came in with..
		CBldConfigurationHolder cfgHolder(pcr, pFileCfg, pProjCfg);
		UnAssignSingleAction(cfgHolder);
	}

	if (!fUnassignContained || (pFileCfg != NULL))
		return;		// either not a config, or we don't want to unassign the config contents

	CComPtr<IEnumVARIANT> pFiles;
	HRESULT hr = GetFileCollection(pProjCfg, &pFiles);
	VOID_RETURN_ON_FAIL_OR_NULL(pFiles, hr);
	while (TRUE)
	{
		CComVariant var;
		hr = pFiles->Next(1, &var, NULL);
		BREAK_ON_DONE(hr);
		CComPtr<VCFileConfiguration> pFileCfg;
		if (!GetFileCfgForProjCfg(var, pProjCfg, &pFileCfg))
			continue;

		CBldConfigurationHolder cfgHolder(pcr, pFileCfg, pProjCfg);
		UnAssignSingleAction(cfgHolder);
	}
}

void CBldAction::UnAssignSingleAction(CBldConfigurationHolder& cfgHolder)
{
	BldFileRegHandle frh = cfgHolder.GetFileRegHandle();

	VARIANT_BOOL bExcluded = VARIANT_FALSE;
	if (!cfgHolder.IsProject() && SUCCEEDED(cfgHolder.GetFileCfg()->get_ExcludedFromBuild(&bExcluded)) && 
		(bExcluded == VARIANT_FALSE))
	{
		VSASSERT(NULL != cfgHolder.GetPCR(), "Trying to unassign action without valid CfgRecord.  How could we get here, even?");
		// Add to dependency graph as possible input to schmooze tool.
		CBldFileRegistry *preg = g_StaticBuildEngine.GetRegistry(cfgHolder.GetPCR());
		VSASSERT(NULL != preg, "No file registry associated with the CfgRecord.  Big trouble.");
		if (frh && preg )
			preg->ReleaseRegRef(frh, TRUE /* pull out filtering */);
	}
			
	// Get the list of actions
	CComPtr<IVCBuildActionList> pActions;
	cfgHolder.GetActionList(&pActions);

	// If unassigning 'en masse' then clear up our output files
	if (cfgHolder.IsProject())
		pActions->FreeOutputFiles();

	// Go through them and detach them 
	pActions->Reset(NULL);
	HRESULT hr = S_OK;
	while (hr == S_OK)
	{
		void* pPos;
		CComPtr<IVCBuildAction> pAction;
		hr = pActions->Next(&pAction, (void **)&pPos);
		BREAK_ON_DONE(hr);
		VALID_CONTINUE_ON_NULL(pAction);

		// Remove from list and de-allocate?
		CComPtr<IVCToolImpl> pVCTool;
		pAction->get_VCToolImpl(&pVCTool);
		if (pVCTool)
		{
			CComPtr<IVCBuildActionList> spBldActionList;
			cfgHolder.GetActionList(&spBldActionList, !cfgHolder.IsProject());
			if (spBldActionList != NULL && spBldActionList->Find(pVCTool, NULL) == S_FALSE)
				continue;	// no, it's not in our list
		}
		pActions->RemoveAtPosition(pPos);
	}
}	    

CBldAction* CBldAction::GetTargetAction()
{
	// link it to the target-level action if we need to?
	if (m_fIsProject)
	{
		// Get the target-level actions
		CBldConfigurationHolder cfgHolder(m_pcrOur, NULL, m_pcrOur->GetVCConfiguration());
		CComPtr<IVCBuildActionList> spActionList;
		cfgHolder.GetActionList(&spActionList, FALSE);

		IVCToolImpl* pVCTool = NULL;
		if (m_pTool != NULL)
			pVCTool = m_pTool->GetVCToolImpl();

		CComPtr<IVCBuildAction> spAction;
		spActionList->Find(pVCTool, &spAction);
		if (spAction != NULL)
		{
			spAction->get_ThisPointer((void **)&m_pTrgAction);
			VSASSERT(m_pTrgAction != NULL, "Gotta have a target action...");
		}
	}

	return m_pTrgAction;
}

void CBldAction::RefreshOutputI(IVCBuildErrorContext* pEC)
{
	// is this item being excluded from the build?
	BOOL fExcluded = FALSE;
	VARIANT_BOOL bvExcluded = VARIANT_FALSE;
	CComQIPtr<VCFileConfiguration> spFileCfg = m_pItem;
	if (spFileCfg == NULL)
		fExcluded = FALSE;
	else if (FAILED(spFileCfg->get_ExcludedFromBuild(&bvExcluded)) || bvExcluded == VARIANT_TRUE)
		fExcluded = TRUE;	// assume yes

	if (fExcluded)
	{
		// clear our source dep. on the output
		ClearSourceDep(pEC);

		// clear the actual output
		ClearOutput(pEC);
	}
	else
	{
		// make our outputs dirty so that we update the output
		m_fDirtyOutput = TRUE;
		(void) GetOutput(pEC);
	}
}

void CBldAction::ClearOutput(IVCBuildErrorContext* pEC)
{
	// ignore FRN_LCL_DESTROY changes to input
	// that may occur because of removal
	// of 'input->output' deps. in graph
	m_fBatchNotify = TRUE;

	int i = 0;
	if (m_pSets != NULL)
	{
		CBldFileRegSet* pregset = &m_pSets->m_frsOutput;
		BldFileRegHandle frh = NULL;
		pregset->InitFrhEnum();
		frh = pregset->NextFrh();
		while (frh != NULL)
		{
			// re-gen'ing the output?
			if (m_fInGenOutput)
			{
				// remember the old outputs...
				m_pSets->m_pfrsOldOutput->AddRegHandle(frh, pEC);
				m_paryOldOutputAttrib->Add(m_pSets->m_aryOutputAttrib.GetAt(i++));
 			}
			else
			{
				// remove this output dependency from the graph
				if (m_pdepgraph != NULL)
				{
					BOOL bOK = m_pdepgraph->ModifyDep(frh, this, DEP_Output, FALSE /* remove */, pEC);
					VSASSERT(bOK, "Failed to modify dependency graph!");
				}
			}

			// remove the entry in the registry if we created it in ::AddOutput()
			VSASSERT(m_pSets->m_frsOutput.RegHandleExists(frh), "Trying to remove a reg handle we did not add.");
			m_pSets->m_frsOutput.RemoveRegHandle(frh);

			if (m_pSets->m_frsSchmoozableOutput.RegHandleExists(frh))
				m_pSets->m_frsSchmoozableOutput.RemoveRegHandle(frh);

			// Next.
			frh->ReleaseFRHRef();
			frh = pregset->NextFrh();
		}

		// empty our output contents and assoc. attributes ...
		VSASSERT(m_pSets->m_frsOutput.IsEmpty(), "Failed to remove all the reg handles in the output set.");
		VSASSERT(m_pSets->m_frsSchmoozableOutput.IsEmpty(), "Failed to remove all the reg handles in the schoomzable output set.");
		m_pSets->m_aryOutputAttrib.RemoveAll();
	}

	// take notice of all input changes....
	m_fBatchNotify = FALSE;
}

// retrieve our scanned dependencies
CBldFileRegSet* CBldAction::GetScannedDep()
{
	if (!m_pSets)
		return NULL;
	return &m_pSets->m_frsScannedDep;
}

// retrieve our source dependencies
CBldFileRegSet* CBldAction::GetSourceDep()
{
	if (!m_pSets)
		return NULL;
	return &m_pSets->m_frsSourceDep;
}

STDMETHODIMP CBldAction::get_SourceDependencies(void** ppFileRegSet)
{
	CHECK_POINTER_NULL(ppFileRegSet);

	CBldFileRegSet* pDeps = GetSourceDep();
	*ppFileRegSet = (void *)pDeps;
	return S_OK;
}

STDMETHODIMP CBldAction::get_ScannedDependencies(void** ppFileRegSet)
{
	CHECK_POINTER_NULL(ppFileRegSet);

	CBldFileRegSet* pDeps = GetScannedDep();
	*ppFileRegSet = (void *)pDeps;
	return S_OK;
}


STDMETHODIMP CBldAction::RemoveSourceDependencyForFrh(void* pFileRegHandle, IVCBuildErrorContext* pEC)
{
	BldFileRegHandle frh = (BldFileRegHandle)pFileRegHandle;
	if (RemoveSourceDep(frh, pEC))
		return S_OK;
	
	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
}

STDMETHODIMP CBldAction::ClearSourceDependencies(IVCBuildErrorContext* pEC)
{
	ClearSourceDep(pEC);
	return S_OK;
}

void CBldAction::ClearDepI(UINT depType, IVCBuildErrorContext* pEC)
{
	if (m_pSets == NULL)
		return;	// nothing to do!

	CBldFileRegSet * pfrsDep = NULL;
	if (depType == DEP_Scanned)
		pfrsDep = &m_pSets->m_frsScannedDep;

	else if (depType == DEP_Source)
		pfrsDep = &m_pSets->m_frsSourceDep;
	
	if (pfrsDep == (CBldFileRegSet *)NULL)
		return;	// nothing to do!

	pfrsDep->InitFrhEnum();
	BldFileRegHandle frh = pfrsDep->NextFrh();
	while (frh != NULL)
	{
		BOOL bOK = RemoveDepI(depType, frh, pEC);
		VSASSERT(bOK, "Failed to remove dependency!");
		frh->ReleaseFRHRef();
		frh = pfrsDep->NextFrh();
	}

	// empty our dependency contents...
	pfrsDep->EmptyContent();
}

BOOL CBldAction::AddDepI(UINT depType, CStringW & strFile, IVCBuildErrorContext* pEC)
{
	BldFileRegHandle frh;

	// if we're not a target tool try to form absolute relative to source
	CPathW pathFile;
	if (!m_pTool->IsTargetTool(this))
	{
		const CPathW* pPath = m_pregistry->GetRegEntry(m_frhInput)->GetFilePath();
		CDirW dirFile;
		if (dirFile.CreateFromPath(*pPath) && 
			pathFile.CreateFromDirAndFilename(dirFile, strFile))
			strFile = (const wchar_t *)pathFile;
		else if (!pathFile.Create(strFile))
			return FALSE;
	}
	else if (!pathFile.Create(strFile))
		return FALSE;

	// do we need to register this?
	frh = CBldFileRegFile::GetFileHandle(pathFile.GetFullPath(), !pathFile.IsActualCase());	// incr. ref

	if (frh == (BldFileRegHandle)NULL)
		return FALSE;

	BOOL bResult = AddDepI(depType,frh, pEC);
	frh->ReleaseFRHRef();
	return bResult;
}

BOOL CBldAction::RemoveDepI(UINT depType, BldFileRegHandle frh, IVCBuildErrorContext* pEC)
{
	CBldFileRegSet* pfrsDep = NULL;
	if (depType == DEP_Scanned)
		pfrsDep = &m_pSets->m_frsScannedDep;

	else if (depType == DEP_Source)
		pfrsDep = &m_pSets->m_frsSourceDep;

	if (pfrsDep == (CBldFileRegSet *)NULL)
		return TRUE;	// nothing to do!

	if (depType == DEP_Scanned || depType == DEP_Missing)
	{
		// added as a source dep.?
		if (GetSourceDep() && GetSourceDep()->RegHandleExists(frh))
			if (!RemoveSourceDep(frh, pEC))	// remove as a source dep. as well!
				return FALSE;
	}
	else if (depType == DEP_Source)
	{
		if (m_pdepgraph != NULL)
		{
			// remove this source dependency from the graph now...
			BOOL bOK = m_pdepgraph->ModifyDep(frh, this, DEP_Source, FALSE /* remove */, pEC);
			VSASSERT(bOK, "Failed to modify dependency graph!");
		}
	}

	if (pfrsDep->RegHandleExists(frh))
	{
		pfrsDep->RemoveRegHandleI(frh, pEC);
	}

	return TRUE;
}

BOOL CBldAction::AddDepI(UINT depType, BldFileRegHandle frh, IVCBuildErrorContext* pEC)
{
	// is this a scanned dep. that should really be a source dep.?
	CDepGrNode * pgrnDep;

	CBldFileRegSet * pfrsDep = NULL;
	if (depType == DEP_Scanned)
		pfrsDep = GetScannedDep();

	else if (depType == DEP_Source)
		pfrsDep = GetSourceDep();

	else
		VSASSERT(0, "Invalid dependency type");


	if (depType == DEP_Scanned)
	{
		// not found on disk?
		// so add as a missing dep.
	}

	if (depType == DEP_Scanned )
	{
		// found this dep.?
		if (m_pdepgraph != NULL )
		{
			if( !m_pdepgraph->IsInitialized() )
				return FALSE;
			// make it a source dep. as well?
			if (m_pdepgraph->FindDep(frh, pgrnDep) && !AddSourceDep(frh, pEC))
				return FALSE;
		}
	}
	// no dependency set by this point == unsupported platform config
	if (pfrsDep == (CBldFileRegSet *)NULL)
		return FALSE;

	// add this source dependencies to the graph now...
	BOOL fOldInGenOutput = m_fInGenOutput;
	m_fInGenOutput = FALSE;

	if (depType == DEP_Source)
	{
		if (m_pdepgraph != NULL)
		{
			BOOL bOK = m_pdepgraph->ModifyDep(frh, this, DEP_Source, TRUE /* add */, pEC);
			VSASSERT(bOK, "Failed modify dependency graph!");
		}
	}
	
	m_fInGenOutput = fOldInGenOutput;
	if (pfrsDep->RegHandleExists(frh))
	{
		// frh is already in pfrsDep.
		return TRUE;
	}
	else
	{
		// Assume that handle is *always* added!
		return pfrsDep->AddRegHandle(frh, pEC);
	}
}

// retrieve our input(s) (may be a file set)
CBldFileRegSet* CBldAction::GetInput(IVCBuildErrorContext* pEC)
{
	RETURN_ON_NULL2(m_pSets, NULL);

	// dirty?
	if (m_fDirtyInput)
	{
		if ((m_frhInput == NULL) && (m_pTool != NULL) && m_pTool->CustomToolType() && m_fIsProject)
			AddInput();	// HACK: try adding it now

		if (m_frhInput != NULL)
		{
			ClearInput();

			// initialize the input
			// just return our input?
			if (!m_pTool->IsTargetTool(this))
				m_pSets->m_frsInput.AddRegHandle(m_frhInput, pEC, FALSE);		// *don't* incr. ref.
			// else validate our inputs
			else
			{
				CBldFileRegSet* psetInput = (CBldFileRegSet *)m_pregistry->GetRegEntry(m_frhInput);

				BldFileRegHandle frh;
				psetInput->InitFrhEnum();
				while ((frh = psetInput->NextFrh()) != (BldFileRegHandle)NULL)
				{
					if (ValidInput(frh))
						m_pSets->m_frsInput.AddRegHandle(frh, pEC, FALSE);	// *don't* incr. ref.
					frh->ReleaseFRHRef();
				}
			}
			m_fDirtyInput = FALSE;
		}
	}

	return &m_pSets->m_frsInput;
}

// retrieve our input(s) (may be a file set)
HRESULT CBldAction::GetRelativePathsToInputs(BOOL bFilterOutSpecial, BOOL bUseMultiLine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CBldFileRegSet *pset = GetInput(pEC);
	BldFileRegHandle  frhInput;
	pset->InitFrhEnum();
	CStringW strInput;
	CStringW strObjs;

	CComBSTR bstrDir;
	CComPtr<VCProject> spProject;
	get_Project(&spProject);
	spProject->get_ProjectDirectory(&bstrDir);
	
	CStringW strDir = bstrDir;
	CDirW dir;
	dir.CreateFromKnown(strDir);
	bool bFirst = true;

	while ((frhInput = pset->NextFrh()) != (BldFileRegHandle)NULL)
	{
		const CPathW * pPath = m_pregistry->GetRegEntry(frhInput)->GetFilePath();
		if (pPath == NULL)
		{
			VSASSERT(FALSE, "Reg entry without an associated path?!?");
			continue;
		}

		if (bFilterOutSpecial)
		{
			CStringW strPath = *pPath;
			CComBSTR bstrPath = strPath;
			if (m_pTool && m_pTool->IsSpecialConsumable(bstrPath))
				continue;
		}

		// quote this input and make relative to project directory?
		pPath->GetRelativeName(dir, strInput, TRUE, FALSE);
		// If GetRelativeName failed, it's probably because the two
		// paths are on different drives, so we just can't make a 
		// relative path.  In this case, however, pPath must contain
		// at least a drive letter (and will probably be a full path)
		// Even if relativization fails, the string will still be quoted

		if (bFirst)
			bFirst = false;
		else if (bUseMultiLine)
			strObjs += L'\n';
		else
			strObjs += L' ';
		strObjs += strInput;
		frhInput->ReleaseFRHRef();
	}
	CComBSTR bstr = strObjs;
	bstr.CopyTo(pVal);
	return S_OK;
}

void CBldAction::ClearInput()
{
	// clear our input
	// (perhaps a file set containing file sets so just remove files...)
	if (m_pSets)
		m_pSets->m_frsInput.EmptyContent();
}

CVCWordArray* CBldAction::GetOutputAttrib()
{
	if (m_fInGenOutput)
		return m_paryOldOutputAttrib;
	if (m_pSets)
		return &(m_pSets->m_aryOutputAttrib);
	return NULL;
}

// retrieve our list of outputs (list of file registry handles)
CBldFileRegSet* CBldAction::GetOutput(IVCBuildErrorContext* pEC)
{
	// return old output if we're currently re-gen'ing new output...
	RETURN_ON_NULL2(m_pSets, NULL);

	if (m_fInGenOutput)
		return m_pSets->m_pfrsOldOutput;

	// got dirty output?
	if (m_fDirtyOutput)
	{
		// set up temporary regset and array until we are done generating outputs
		CBldFileRegSet	frsOldOutput;
		CVCWordArray	aryOldOutputAttrib;
		frsOldOutput.SetFileRegistry(m_pregistry);
	    frsOldOutput.CalcAttributes(FALSE);
		m_pSets->m_pfrsOldOutput = &frsOldOutput;
		m_paryOldOutputAttrib = &aryOldOutputAttrib;

		// the output is no longer dirty
		m_fDirtyOutput = FALSE;

		// we're currently re-gen'ing the output
		m_fInGenOutput = TRUE;
	
		// clear and remember the old output
		ClearOutput(pEC);

	 	// re-initialise the output

		CBldActionListHolder lstActions;
		lstActions.m_pActions->AddToEnd(this);
		{	// scope output regset attribute recalculation...
			CBldTurnRegRecalcOff recalcOff(&m_pSets->m_frsOutput);
			if (!m_pTool->GenerateOutput(AOGO_Default, lstActions.m_pActions, pEC))
			{
				ClearSourceDep(pEC);
				CBldFileRegSet Deletions;
				FindDeletions(Deletions, pEC);
				m_pSets->m_pfrsOldOutput->EmptyContent();
				if (m_pdepgraph != NULL)
					HandleDeletions(Deletions, pEC);
				m_paryOldOutputAttrib->RemoveAll();
				m_fInGenOutput = FALSE;
				return &m_pSets->m_frsOutput;
			}
		}

		if (m_fInGenOutput)
		{
			//
			// modify the graph with any differences we detect
			//

			BldFileRegHandle frh = NULL;
			int i = 0;

			// additions and same
			CBldFileRegSet* pregset = &m_pSets->m_frsOutput;

			CBldFileRegSet	Additions;
			CBldFileRegSet	Deletions;
			CVCWordArray	AdditionsAttrib;

			pregset->InitFrhEnum();
			frh = pregset->NextFrh();
			while (frh != NULL)
			{
				WORD attrib = m_pSets->m_aryOutputAttrib.GetAt(i++);
				BOOL fWithAction = !!attrib;

				// found in our old outputs?
				if (!m_pSets->m_pfrsOldOutput->RegHandleExists(frh))
				{
					// new dep.
					// not found, so add this output dependencies to the graph
					Additions.AddRegHandle(frh, pEC);
					AdditionsAttrib.Add((WORD)fWithAction);
				}
				else
					m_pSets->m_pfrsOldOutput->RemoveRegHandleI(frh, pEC);	// same dep.
				// Next.
				frh->ReleaseFRHRef();
				frh = pregset->NextFrh();
			}

			// deletions
			FindDeletions(Deletions, pEC);

			m_pSets->m_pfrsOldOutput->EmptyContent();

			if (m_pdepgraph != NULL)
			{
				pregset = &Additions;
				pregset->InitFrhEnum();
				frh = pregset->NextFrh();
				int iAdd = 0;
				while (frh != NULL)
				{
					BOOL bOK = m_pdepgraph->ModifyDep(frh, this, DEP_Output, TRUE /* add */, pEC,
						AdditionsAttrib.GetAt(iAdd));
					VSASSERT(bOK, "Failed to modify dependency!");

					// Next.
					frh->ReleaseFRHRef();
					frh = pregset->NextFrh();
					iAdd++;
				}

				HandleDeletions(Deletions, pEC);
			}

			m_paryOldOutputAttrib->RemoveAll();

			// no longer re-gen'ing the output
			m_fInGenOutput = FALSE;
		}
	}

	return &m_pSets->m_frsOutput;
}

void CBldAction::FindDeletions(CBldFileRegSet& Deletions, IVCBuildErrorContext* pEC)
{
	CBldFileRegSet* pregset = m_pSets->m_pfrsOldOutput;
	pregset->InitFrhEnum();
	BldFileRegHandle frh = pregset->NextFrh();
	while (frh != NULL)
	{
		// remove this output dependency from the graph
		Deletions.AddRegHandle(frh, pEC);

		// Next.
		frh->ReleaseFRHRef();
		frh = pregset->NextFrh();
	}
}

void CBldAction::HandleDeletions(CBldFileRegSet& Deletions, IVCBuildErrorContext* pEC)
{
	CBldFileRegSet* pregset = &Deletions;
	pregset->InitFrhEnum();
	BldFileRegHandle frh = pregset->NextFrh();
	while (frh != NULL)
	{
		BOOL bOK = m_pdepgraph->ModifyDep(frh, this, DEP_Output, FALSE /* remove */, pEC);
		VSASSERT(bOK, "Failed to modify dependency graph!");

		// Next.
		frh->ReleaseFRHRef();
		frh = pregset->NextFrh();
	}
}

HRESULT CBldAction::AddOutputI(BldFileRegHandle frh, IVCBuildErrorContext* pEC, BOOL fWithAction /* = TRUE */)
{
	m_fDirtyOutput = FALSE;

	if (!m_pSets->m_frsOutput.RegHandleExists(frh))
		m_pSets->m_frsOutput.AddRegHandle(frh, pEC);

	// re-gen'ing the output
	if (m_fInGenOutput)
		m_pSets->m_aryOutputAttrib.Add((WORD)fWithAction);
	else
	{
		// add this output dependencies to the graph
		if (m_pdepgraph != NULL)
		{
			BOOL bOK = m_pdepgraph->ModifyDep(frh, this, DEP_Output, TRUE /* add */, pEC, fWithAction);
			VSASSERT(bOK, "Failed to modify dependency graph!");
		}
	}

	return S_OK;
}

STDMETHODIMP CBldAction::AddOutputFromPath(BSTR bstrPath, IVCBuildErrorContext* pEC, VARIANT_BOOL fWithAction, 
	VARIANT_BOOL fAllowFilter, long nPropID, IVCToolImpl* pTool)
{
	CPathW path;
	if (!path.CreateFromKnown(bstrPath))
	{
		if (nPropID > 0)
		{
			CStringW strPath = bstrPath;
			CComQIPtr<IVCPropertyContainer> spPropContainer = m_pItem;
			InformUserAboutBadFileName(pEC, spPropContainer, pTool, nPropID, strPath);
		}
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	}

	return AddOutput(&path, pEC, fWithAction == VARIANT_TRUE, fAllowFilter == VARIANT_TRUE, nPropID, pTool);
}

HRESULT CBldAction::AddOutput(const CPathW* pPath, IVCBuildErrorContext* pEC, BOOL fWithAction /* = TRUE */, 
	BOOL fAllowFilter /* = TRUE */, long nPropID /* = -1 */, IVCToolImpl* pTool /* = NULL */)
{
	BldFileRegHandle frh;

	CBldFileRegFile * pFileReg;

	// do we need to register this?
	frh = CBldFileRegFile::GetFileHandle(*pPath, !pPath->IsActualCase());	// incr. ref, no filter

	if (NULL==frh)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	pFileReg = (CBldFileRegFile *)m_pregistry->GetRegEntry(frh);

	// set default output dir type
	pFileReg->SetOutDirType(m_pTool->IsTargetTool(this) ? 1 : 2);

	HRESULT hRet = AddOutputI(frh, pEC, fWithAction);
	frh->ReleaseFRHRef();

	// we delayed the filtering until after it was in our output
	// now filter the file it	
	if (SUCCEEDED(hRet) && fAllowFilter && !m_pSets->m_frsSchmoozableOutput.RegHandleExists(frh))
		m_pSets->m_frsSchmoozableOutput.AddRegHandle(frh, pEC);

	return hRet;
}
   
BOOL CBldAction::EnabledOutput(long nEnablePropId)
{
	if (nEnablePropId == -1)
		return TRUE;

	CComQIPtr<IVCPropertyContainer> spPropertyContainer = m_pItem;
	RETURN_ON_NULL2(spPropertyContainer, TRUE);

	CComVariant var;
	if (FAILED(spPropertyContainer->GetProp(nEnablePropId, &var)))
		return TRUE;

	if (var.vt == VT_EMPTY)
		return FALSE;
	else if (var.vt == VT_BOOL)
		return (var.boolVal == VARIANT_TRUE);
	else if (var.vt == VT_I4)
		return (var.lVal != 0);
	else
		return TRUE;
}

BOOL CBldAction::GetStringProperty(long nPropID, CStringW& rstrValue, BOOL bEvaluateProperty /* = FALSE */)
{
	rstrValue.Empty();
	CComQIPtr<IVCPropertyContainer> spPropCntr = m_pItem;
	RETURN_ON_NULL2(spPropCntr, FALSE);

	CComBSTR bstrVal;
	BOOL bRet = TRUE;
	{
		CComVariant var;
		bRet = (S_OK == spPropCntr->GetProp(nPropID, &var) );
		if (bRet && var.vt == VT_BSTR)
			bstrVal = var.bstrVal;
	}

	if (bRet && bEvaluateProperty)
	{
		CComBSTR bstrOut;
		bRet = SUCCEEDED(spPropCntr->Evaluate(bstrVal, &bstrOut));
		rstrValue = bstrOut;
	}
	else
		rstrValue = bstrVal;

	return bRet;
}

void CBldAction::GetItemFileName(CStringW& rstrName)
{
	rstrName.Empty();

	CComBSTR bstrName;
	if (m_pItem == NULL || FAILED(m_pItem->get_ItemFileName(&bstrName)))
		return;

	rstrName = bstrName;
}

HRESULT CBldAction::AddOutputFromFileName(CStringW& rstrFileName, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, 
	CDirW* pDirBase, BOOL fWithAction, BOOL fMakeItRelativeToProject, long nErrPropID, IVCToolImpl* pTool)
{
	// create a path for this filename and add it as an output
	CPathW tp;
	BOOL fContinue = TRUE;
	CStringW strErrFileName;
	fContinue = tp.CreateFromDirAndFilename(*pDirBase, rstrFileName);
	if (!fContinue && fMakeItRelativeToProject)
	{
		CDirW dirBaseProj;
		CComQIPtr<IVCPropertyContainer> spPropContainer = m_pItem;
		if (m_pItem == NULL)
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
		CComBSTR bstrProjDir;
		if (FAILED(spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir)))	// should never have macros in it
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
		CStringW strProjDir = bstrProjDir;
		fContinue = dirBaseProj.CreateFromKnown(strProjDir);
		if (fContinue)
			fContinue = tp.CreateFromDirAndFilename(dirBaseProj, rstrFileName);
		else if (nErrPropID > 0)
			strErrFileName = strProjDir;
	}

	if (fContinue)
	{
		CStringW strDefExtension = bstrDefExtension;
		tp.ChangeExtension (strDefExtension);
		tp.GetActualCase(TRUE);
		return AddOutput((const CPathW *)&tp, pEC, fWithAction, TRUE, nErrPropID, pTool);
	}
	else if (nErrPropID > 0 && strErrFileName.IsEmpty())
		strErrFileName = rstrFileName;

	CComQIPtr<IVCPropertyContainer> spPropContainer = m_pItem;
	InformUserAboutBadFileName(pEC, spPropContainer, pTool, nErrPropID, strErrFileName);
	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
}

STDMETHODIMP CBldAction::AddOutputFromFileName(BSTR bstrFileName, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, 
	BSTR bstrDirBase, VARIANT_BOOL fWithAction, long nErrPropID, IVCToolImpl* pTool)
{
	CDirW dirBase;
	if (!dirBase.CreateFromKnown(bstrDirBase))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	CStringW strFileName = bstrFileName;
	return FinishAddOutputFromID(strFileName, pEC, FALSE, bstrDefExtension, dirBase, NULL, fWithAction == VARIANT_TRUE, 
		nErrPropID, pTool);
}

STDMETHODIMP CBldAction::AddOutputInSubDirectory(BSTR bstrSubDirectory, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, 
	BSTR bstrDirBase, BSTR bstrDefFilename, long nEnablePropId, VARIANT_BOOL fWithAction, long nErrPropID, IVCToolImpl* pTool)
{
	// First check the enabling prop
	if (!EnabledOutput(nEnablePropId))
		return S_OK;

	CDirW dirBase;
	if (!dirBase.CreateFromKnown(bstrDirBase))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	CStringW strSubDirectory = bstrSubDirectory;
	return FinishAddOutputFromID(strSubDirectory, pEC, TRUE, bstrDefExtension, dirBase, bstrDefFilename,
		fWithAction == VARIANT_TRUE, nErrPropID, pTool);
}

STDMETHODIMP CBldAction::AddOutputFromID(long nFileNameId, IVCBuildErrorContext* pEC, BSTR bstrDefExtension, BSTR bstrDirBase, 
	BSTR bstrDefFilename, long nEnablePropID, VARIANT_BOOL fWithAction, IVCToolImpl* pTool)
{
	// First check the enabling prop
	if (!EnabledOutput(nEnablePropID))
		return S_OK;

	CDirW dirBase;
	if (!dirBase.CreateFromKnown(bstrDirBase))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	// Output filename? 
	CStringW strFileName;
	GetStringProperty(nFileNameId, strFileName, TRUE);
	return FinishAddOutputFromID(strFileName, pEC, FALSE, bstrDefExtension, dirBase, bstrDefFilename,
		fWithAction == VARIANT_TRUE, nFileNameId, pTool);
}

HRESULT CBldAction::FinishAddOutputFromID(CStringW& rstrFileName, IVCBuildErrorContext* pEC, BOOL fIsDirectoryLocation, 
	BSTR bstrDefExtension, CDirW& dirBase, BSTR bstrDefFilename, BOOL fWithAction, long nErrPropID, IVCToolImpl* pTool)
{
	BOOL fGotFileName = FALSE;
	BOOL fMakeItRelativeToProject = !rstrFileName.IsEmpty();

	// Output filename? 
	if (!rstrFileName.IsEmpty())
	{
		// Okay, the item has an apparently non empty-name entry.  See if it's a directory by checking 
		// last character.
		int len = rstrFileName.GetLength();
		const wchar_t * pc = (const wchar_t *) rstrFileName;
		pc = _wcsdec(pc, (wchar_t *)(pc+len));
	
		// need a filename?
		if (pc && (*pc != L'/' && *pc != L'\\') )
		{
			if (fIsDirectoryLocation)	// need to add that slash...
				rstrFileName += L"\\";
			else
				fGotFileName = TRUE;	// no ... it doesn't appear to be a dir.
		}
	}

	// got a filename?
	if (!fGotFileName)
	{
		// no, create a default..
		if (bstrDefFilename)
		{
			CStringW strDefFilename = bstrDefFilename;
			rstrFileName += strDefFilename;
		}
		else
		{
			CStringW strTmp;
			// just append and change extension later
			GetItemFileName(strTmp);
			rstrFileName += strTmp;
		}
	}

	return AddOutputFromFileName(rstrFileName, pEC, bstrDefExtension, &dirBase, fWithAction, fMakeItRelativeToProject, 
		nErrPropID, pTool);
}

// Used to add output files that don't use a property to determine their path -
// that is, output files which are co-resident with the input files generating
// them
STDMETHODIMP CBldAction::AddOutputFromDirectory(BSTR bstrDirBase, IVCBuildErrorContext* pEC, long nEnablePropId, BSTR bstrExt, 
	long nErrPropID, IVCToolImpl* pTool)
{
    CStringW strOutFile;
    CPathW   outFile;
    
    // Check whether the enabling property is present
    if (!EnabledOutput(nEnablePropId))
        return S_OK;

    // Formulate the path
	GetItemFileName(strOutFile);
	CDirW baseDir;
	if (!baseDir.CreateFromKnown(bstrDirBase))
	{
		CStringW strDirBase = bstrDirBase;
		CComQIPtr<IVCPropertyContainer> spPropContainer = m_pItem;
		InformUserAboutBadFileName(pEC, spPropContainer, pTool, nErrPropID, strDirBase);
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	}
    if (!outFile.CreateFromDirAndFilename(baseDir, strOutFile))
	{
		CComQIPtr<IVCPropertyContainer> spPropContainer = m_pItem;
		InformUserAboutBadFileName(pEC, spPropContainer, pTool, nErrPropID, strOutFile);
		return E_FAIL;
	}
	CStringW strExt = bstrExt;
    outFile.ChangeExtension(strExt);
    
    // Add the output file
    return AddOutput(&outFile, pEC, FALSE, TRUE, nErrPropID, pTool);
}

void CBldAction::InformUserAboutBadFileName(IVCBuildErrorContext* pEC, IVCPropertyContainer* pPropContainer, IVCToolImpl* pTool, 
	long nErrPropID, CStringW& strFile)
{
	if (nErrPropID < 0 || pEC == NULL || pTool == NULL)
		return;

	pTool->InformUserAboutBadFileName(pEC, pPropContainer, nErrPropID, strFile);
}

BOOL CBldAction::RefreshCommandOptions(BOOL& fChanged, BOOL fNoSet /* = FALSE */)
{
	BOOL fRet = TRUE;

	CBldAction* pTrgAction = GetTargetAction();

	// dirty command-line?
	if (!m_fDirtyCommand)
	{
		// if we've already done this return whether we changed
		if (m_wMarkRefCmdOpt == CBldAction::m_wActionMarker)
		{
			// what was the status of the last refresh?
			fChanged = m_fCmdOptChanged;
			return TRUE;
		}

		// don't if there's no possible change at our level
		if (!m_fPossibleOptionChange &&
			// and at the target-level if we are a file-level action
			(pTrgAction == (CBldAction *)NULL ||
			 (!pTrgAction->m_fPossibleOptionChange && !pTrgAction->m_fDirtyCommand))
		   )
		{
			// no change....
			fChanged = FALSE;
			return TRUE;
		}
	}

	BOOL fCheckTargetAction = FALSE;

	// get target actions from the tool
	CStringW strOldOptions(m_strOptions);

	// just make the check and not actually set the new command-line
	if (fNoSet)
	{
		// changed?

		// dirty command-line?
		if (m_fDirtyCommand)
			fChanged = m_fPossibleOptionChange;	// want to allow for forced option change *before* first build
		else
		{
			CStringW strDummyOptions; 
			fRet = m_pTool->GetCommandLineOptions(m_pItem, this, strDummyOptions);
			fChanged = strDummyOptions != strOldOptions;
		}
	}
	else
	{
		// changed?
		m_strOptions = L""; 
		fRet = m_pTool->GetCommandLineOptions(m_pItem, this, m_strOptions);

		// dirty command-line?
		if (m_fDirtyCommand)
		{
			fChanged = m_fPossibleOptionChange;
			m_fDirtyCommand = FALSE;	// we've got the command-line -> not dirty!
		}
		else
			fChanged = m_strOptions != strOldOptions;
	}

	if (!fNoSet && m_fPossibleOptionChange)
	{
		m_fPossibleOptionChange = FALSE;
		m_pItem->ClearDirtyCommandLineOptionsFlag();
	}

	// make sure we don't attempt to retrieve it again
	m_fCmdOptChanged = fChanged;
	m_wMarkRefCmdOpt = CBldAction::m_wActionMarker;

	return fRet;
}

BOOL CBldAction::AddScannedDep(int type, IVCBuildErrorContext* pEC, CStringW& strFile, int nLine, BOOL fScan)
{
	VSASSERT(m_pIncludeList != DEP_None, "About add a dependency to a non-dependency list...");

	// is this a system include? ignore?
	if( !g_SysInclReg.IsSysInclude(strFile) )
	{
		// need to allocate our include entry list?
		if (m_pIncludeList == DEP_Empty)
		{
			if ((m_pIncludeList = new CBldIncludeEntryList(5)) == (CBldIncludeEntryList *)NULL)
				return FALSE;	// failure
		}

		// construct the include entry
		CBldIncludeEntry * pEntry = new CBldIncludeEntry;
		pEntry->m_EntryType = type;	  	
		pEntry->m_nLineNumber = nLine;
		pEntry->m_bShouldBeScanned = fScan;
		pEntry->m_FileReference = strFile;

		pEntry->CheckAbsolute();	// check the absoluteness of scanned dep.

		// add this to ourselves
		m_pIncludeList->AddTail(pEntry);
	}

	return TRUE; // success
}

//
// Update dependency stuff for CBldAction
//
BOOL CBldAction::IsDepInfoInitialized()
{
	return 0 != m_LastDepUpdate.dwLowDateTime || 0 != m_LastDepUpdate.dwLowDateTime;
}


BOOL CBldAction::IsNewDepInfoAvailable(IVCBuildErrorContext* pEC)
{
	if (m_fIsProject)
	{
		RETURN_ON_NULL2(m_pTool, FALSE);
		return m_pTool->HasPrimaryOutput();	// rescan any project dependencies we might have for primary target tool
	}
	else if (NULL != m_pTool && m_pTool->HasDependencies(this))
	{
		//
		// Dependency information from minimal rebuild engine.
		// See if .obj date is newer than m_LastDepUpdate.
		//
		CBldFileRegSet* pRegSet = GetOutput(pEC);
		VSASSERT(NULL != pRegSet, "Failed to initialize output list (somewhere well before here)");
		if (NULL != pRegSet)
		{
			BldFileRegHandle frh = pRegSet->GetFirstFrh();
			if (NULL!=frh)
			{
				FILETIME ftime;

				VSASSERT(!g_bInProjClose, "Bad timestamp due to being in project close.");	// GetTimeProp will not return correct timestamp if closing project.
				frh->RefreshAttributes();

				BOOL bOK = frh->GetTimeProp(P_NewTStamp, ftime, FALSE);
				VSASSERT(bOK, "Failed to pick up time stamp property!");
				frh->ReleaseFRHRef();

				return ftime > m_LastDepUpdate;
			}
		}
	}
	else
	{
		//
		// Dependency information from scanner or NCB.
		//	See dependency file set date is later than m_LastDepUpdate.
		//

		VSASSERT(!g_bInProjClose, "Bad timestamp due to being in project close.");	// GetTimeProp will not return correct timestamp if closing project.

		// Check self
		// Note: GetFileRegHandle() doesn't ref count...yet.
		CComQIPtr<VCFileConfiguration> spFileCfg = m_pItem;
		CBldConfigurationHolder cfgHolder(m_pcrOur, spFileCfg, m_pcrOur->GetVCConfiguration());
		BldFileRegHandle frh = cfgHolder.GetFileRegHandle();
		VSASSERT(NULL!=frh, "Bad initialization way back when we created this action; frh cannot be NULL");
		FILETIME ftime;
		BOOL bOK = frh->GetTimeProp(P_NewTStamp, ftime);
		VSASSERT(bOK, "Failed to pick up time stamp property!");
		if (ftime > m_LastDepUpdate)
			return TRUE;

		// Check sources.
		bOK = GetSourceDep()->GetTimeProp(P_NewTStamp, ftime);
		VSASSERT(bOK, "Failed to pick up time stamp property!");
		if (ftime > m_LastDepUpdate)
			return TRUE;

		// Check dependencies.
		bOK = GetScannedDep()->GetTimeProp(P_NewTStamp, ftime);
		VSASSERT(bOK, "Failed to pick up time stamp property!");

		return ftime > m_LastDepUpdate;
	}
	return FALSE;
}

//
// Update dependency information if new information is available.
// Set bForceUpdate = TRUE to force update if updating has been turned off
// minimal rebuild info is not available.
BOOL CBldAction::UpdateDepInfo(IVCBuildErrorContext* pEC, BOOL bForceUpdate /* = FALSE */)
{
	// Not ready for bForceUpdate yet.
	VSASSERT(!bForceUpdate, "Not ready for bForceUpdate yet in UpdateDepInfo");

	// Don't update if new information is not available.
	if (!m_fDepMissing && !IsNewDepInfoAvailable(pEC))
		return FALSE;

	m_fDepMissing = FALSE;
	return ScanDependencies(pEC);
}

void CBldAction::UpdateDepInfoTimeStamp()
{
	GetSystemTimeAsFileTime(&m_LastDepUpdate);
}

HRESULT CBldAction::GetBuildEngineImplFromEC(IVCBuildErrorContext* pEC, IVCBuildEngineImpl** ppBldEngineImpl)
{
	CHECK_POINTER_NULL(ppBldEngineImpl);
	*ppBldEngineImpl = NULL;
	RETURN_ON_NULL2(pEC, S_FALSE);

	CComPtr<IDispatch> spDispBldEngine;
	pEC->get_AssociatedBuildEngine(&spDispBldEngine);
	CComQIPtr<IVCBuildEngineImpl> spBldEngineImpl = spDispBldEngine;
	*ppBldEngineImpl = spBldEngineImpl.Detach();
	return S_OK;
}

BOOL CBldAction::ScanDependencies(IVCBuildErrorContext* pEC, BOOL bUseAnyMethod /* = FALSE */, BOOL bWriteOutput /* = TRUE */ )
{
	BOOL bRetval = TRUE;

	CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl;
	if (!m_fIsProject)
	{
		if (NULL == m_pcrOur || NULL == m_pTool || !m_pTool->HasDependencies(this))
		{
			CBldConfigCacheEnabler EnableCaching;

			if (m_pTool != NULL && m_pTool->CanScanForDependencies())
			{
				GetBuildEngineImplFromEC(pEC, &spBuildEngineImpl);
				if (spBuildEngineImpl)
				{
					CBldScannerCache* pScannerCache = NULL;
					spBuildEngineImpl->get_ScannerCache((void **)&pScannerCache);
					if (pScannerCache)
					{
						pScannerCache->BeginCache();
						bRetval = m_pTool->ScanDependencies(this, pEC, bWriteOutput);
						pScannerCache->EndCache();
					}
				}
			}

			return TRUE;
		}
	}

	CComPtr<IVCBuildAction> spAction;
	CVCStringWArray strArray;

	if (m_pTool->GetDependencies(this, strArray))
	{
		// See if dependencies have changed and then update if necessary
		BOOL bDirty = FALSE;
		if (m_pItem != NULL)
		{
			CComPtr<IVCBuildActionList> spActionList;
			if (FAILED(m_pItem->get_ActionList(&spActionList)) || spActionList == NULL)
				return FALSE;

			spActionList->Reset(NULL);
			while (TRUE)
			{
				CComPtr<IVCBuildAction> spThisAction;
				HRESULT hr = spActionList->Next(&spThisAction, NULL);
				if (hr != S_OK)
					break;
				if (spThisAction == NULL)
					continue;

				IVCToolImpl* pToolImpl = (m_pTool == NULL) ? NULL : m_pTool->GetVCToolImpl();
				BOOL bMatchesOldTool = FALSE;
				if (FAILED(spThisAction->MatchesOldTool(pToolImpl, &bMatchesOldTool)))
					continue;
				if (bMatchesOldTool)
				{
					spAction = spThisAction;
					break;
				}
			}

			RETURN_ON_NULL2(spAction, FALSE);
		}

		//
		// Determine if dependencies have changed.
		//
		{
			// remember previously scanned deps.
			// and also any old ones (so we can compare diffs)
			CVCMapPtrToPtr depsPrevious;

			// construct the map of current deps
			BldFileRegHandle frh = NULL;
			CBldFileRegSet* pfrs = NULL;
			
			if (SUCCEEDED(spAction->get_ScannedDependencies((void **)&pfrs)) && pfrs != NULL)
			{
				pfrs->InitFrhEnum();
				while ((frh = pfrs->NextFrh()) != (BldFileRegHandle)NULL)
				{
					depsPrevious.SetAt(frh, (void*)DEP_Scanned);
					frh->ReleaseFRHRef();
				}
			}

			bDirty = (strArray.GetSize() != depsPrevious.GetCount());
			if (!bDirty)
			{
				for (int i=0; i < strArray.GetSize(); i++)
				{
					CPathW path;
					BOOL bOK = path.Create(strArray[i]);
					VSASSERT(bOK, "Failed to create path for a dependency!");	
					BldFileRegHandle frh = CBldFileRegFile::LookupFileHandleByName(path.GetFullPath(), TRUE);
					if (NULL != frh)
						frh->ReleaseFRHRef();
					if (NULL == frh)
					{
						bDirty = TRUE;
						break;
					}
					else if (!depsPrevious.RemoveKey((void *)frh))
					{
						bDirty = TRUE;
						break;
					}
				}
				bDirty = bDirty || !depsPrevious.IsEmpty();
			}
		}

		if (bDirty)
		{
			CBldHoldRegRecalcs holdRecalcs(spAction, CBldHoldRegRecalcs::eScannedRegSet);	// don't do regset updates 'til we're done here
			// clear the action's dependencies list
			spAction->ClearScannedDependencies(pEC);

			// add new ones
			for (int i = 0; i < strArray.GetSize(); i++)
			{
				CPathW path;
				BOOL bOK = path.Create(strArray[i]);
				if (!bOK)	// not a real path
					continue;
				VSASSERT(bOK, "Failed to create path for a dependency!");	
				BldFileRegHandle frh = CBldFileRegFile::GetFileHandle(path, TRUE);
				if (NULL != frh)
				{
					spAction->AddScannedDependencyFromFrh((void *)frh, pEC);
					frh->ReleaseFRHRef();
				}
			}
		}

		spAction->UpdateDependencyInformationTimeStamp();

		bRetval = TRUE;
	}
	else if (bUseAnyMethod)
	{
		//
		// Try to use the scanner.  
		// review(tomse): May try to use NCB information if available.
		//
		CBldConfigCacheEnabler EnableCaching;

		if (m_pTool != NULL && m_pTool->CanScanForDependencies())
		{
			if (spBuildEngineImpl == NULL)
				GetBuildEngineImplFromEC(pEC, &spBuildEngineImpl);
			if (spBuildEngineImpl)
			{
				CBldScannerCache* pScannerCache = NULL;
				spBuildEngineImpl->get_ScannerCache((void **)&pScannerCache);
				if (pScannerCache)
				{
					pScannerCache->BeginCache();
					bRetval = m_pTool->ScanDependencies(this, pEC, bWriteOutput);
					pScannerCache->EndCache();
				}
			}
		}
	}
	else
	{
		bRetval = FALSE;
	}

	return bRetval;
}

STDMETHODIMP CBldAction::ScanToolDependencies(IVCToolImpl* pToolImpl, IVCBuildErrorContext* pEC, VARIANT_BOOL bWriteOutput)
{
	RETURN_INVALID_ON_NULL(pToolImpl);
	if (m_pItem == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	
	CComPtr<IVCBuildActionList> spActionList;
	if (FAILED(m_pItem->get_ActionList(&spActionList)) || spActionList == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

#ifdef _DEBUG
	{
		long cCount = 0;
		spActionList->get_Count(&cCount);
		// FUTURE: handle more than one tool on this source file
		VSASSERT(cCount == 1, "Cannot handle more than one tool on a source file at a time.");
	}
#endif	// _DEBUG

	// remember previously scanned deps. and also any old ones (so we can compare diffs)
	CVCMapPtrToPtr depsPrevious;

	// construct the map of current deps
	CComPtr<IVCBuildAction> spAction;
	spActionList->get_LastAction(&spAction);
	if (spAction == NULL)
	{
		VSASSERT(FALSE, "Failed to get last action!");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	}

	BldFileRegHandle frh;
	CBldFileRegSet* pfrs = GetScannedDep();
	if (pfrs != NULL)
	{
		pfrs->InitFrhEnum();
		while ((frh = pfrs->NextFrh()) != (BldFileRegHandle)NULL)
		{
			depsPrevious.SetAt(frh, (void*)DEP_Scanned);
			frh->ReleaseFRHRef();
		}
	}

	// This marker is incremented once every call to this function. 
	// It is used to prevent processing of an include file that is seen
	// multiple times when scanning a particular compiland (.cpp). 
	s_nMyMarker++;

	// get the include path for this item
	CComBSTR bstrIncludePath;
	CComQIPtr<IVCPropertyContainer> spPropContainer = m_pItem;
	pToolImpl->GetAdditionalIncludeDirectoriesInternal(spPropContainer, &bstrIncludePath);

	// should temporarily change to the project directory so we
	// could scan appropriately (since relative SEARCH path should be from the project level)
	CComBSTR bstrProjPath;
	if (spPropContainer != NULL)
		spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjPath);	// should never have macros in it

	CStringW strProjPath = bstrProjPath;
	CCurDirW currentDir(strProjPath);

	// file's registry handle  
	BldFileRegHandle hndItemFile = NULL;
	m_pItem->get_FileRegHandle((void **)&hndItemFile);
	VSASSERT(hndItemFile != NULL, "All files must have reg handles! Is this file properly in the project and project config?");

	// directory of this file to be scanned
	CDirW dirFile; 
	dirFile.CreateFromPath(*g_FileRegistry.GetRegEntry(hndItemFile)->GetFilePath());
	
	// add our file name to the scan list to start the process:
	VARIANT_BOOL fDontScanItem = VARIANT_FALSE;
	CComQIPtr<VCFileConfiguration> spFileCfg = m_pItem;
	if (spFileCfg != NULL)
		spFileCfg->get_ExcludedFromBuild(&fDontScanItem);
	else
		fDontScanItem = VARIANT_FALSE;

	// list of FileRegHandles for files to be examined. Entries in this
	// list have their own references:
	CVCPtrList lToBeScanned; 	
	BldFileRegHandle hndFileReg = NULL;
	BldFileRegHandle hndIncludeEntryItem = NULL;

	if (fDontScanItem == VARIANT_FALSE)
		lToBeScanned.AddTail((void *) hndItemFile);

	CVCPtrList lstDeps;
	BOOL bDirty = FALSE;
	
	CBldScannerCache* pScannerCache = NULL;
	if (!lToBeScanned.IsEmpty() && pEC)
	{
		CComPtr<IDispatch> spDispBuildEngine;
		pEC->get_AssociatedBuildEngine(&spDispBuildEngine);
		CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = spDispBuildEngine;
		if (spBuildEngineImpl)
			spBuildEngineImpl->get_ScannerCache((void **)&pScannerCache);
	}

	while (!lToBeScanned.IsEmpty())	
	{
		hndFileReg = (BldFileRegHandle) lToBeScanned.GetHead();
		
		CBldIncludeEntryList * pEntryList = NULL;
		if (pScannerCache)
			pEntryList = pScannerCache->LookupFile(hndFileReg);
		if (!pEntryList)
		{
   			lToBeScanned.RemoveHead();	// scanned this file

			SetDepState(DEP_Empty);	// FUTURE: remove hack init.

			if (FAILED(pToolImpl->GenerateDependencies(spAction, (void *)hndFileReg, NULL)))
			{
				if (hndFileReg == hndItemFile)
					break;	// failure
				else
					continue;
			}

			pEntryList = GetIncludeDep();

			// FUTURE: remove hack init.
			// do we need an 'dummy' list?
			if (pEntryList == DEP_Empty)
				pEntryList = new CBldIncludeEntryList(5);

			if (pScannerCache)
  				pScannerCache->Add(hndFileReg, pEntryList);

			if (pEntryList == DEP_None)
			{
				continue;
			}

			// Go through an mark all the include entries with the path of the
			// file we scanned, so we can use it to check later
			CDirW dir;
	 		const CPathW * pPath = g_FileRegistry.GetRegEntry(hndFileReg)->GetFilePath();
			if (dir.CreateFromPath(*pPath))
			{
				for (VCPOSITION pos = pEntryList->GetHeadPosition (); pos != NULL ; )
				{
					CBldIncludeEntry * pEntry = (CBldIncludeEntry *) pEntryList->GetNext(pos);
					pEntry->m_strReferenceIn = *pPath;
					if (pEntry->m_EntryType & IncTypeCheckOriginalDir)
					{
						pEntry->m_OriginalDir = dir;
					}
		  		}
		  	}
			else
			{
				for (VCPOSITION pos = pEntryList->GetHeadPosition (); pos != NULL ; )
				{
					CBldIncludeEntry * pEntry = (CBldIncludeEntry *) pEntryList->GetNext(pos);
					pEntry->m_strReferenceIn = *pPath;
		  		}
		  	}

			// Certainly haven't seen this file, so add it to dep. list,
			// unless its the initial path:
			if (hndFileReg != hndItemFile)
			{
				// check to see if this dependency was here before
				UINT depType;
				// If we know we have to change the dependencies anyway (bDirty = TRUE)  
				// avoid doing these costly lookups
				if (!bDirty && depsPrevious.Lookup((void *)hndFileReg, (void *&)depType))
				{
					// may be dirty if the dep. is no longer same type
					// ie. missing or scanned
					if (hndFileReg->ExistsOnDisk())
					{
						if (depType == DEP_Missing)
							bDirty = TRUE;
					}
					else if (hndIncludeEntryItem != NULL)
					{
						CBldFileRegEntry* preg = g_FileRegistry.GetRegEntry(hndIncludeEntryItem);
						CPathW* pPath = (CPathW *)preg->GetFilePath();
						if (pPath && !pPath->ExistsOnDisk())
						{
							if (depType == DEP_Scanned)	
								bDirty = TRUE;
						}
					}

					// move it to the current dep list
					depsPrevious.RemoveKey((void *)hndFileReg);

				}
				else
				{
					bDirty = TRUE;
				}

				lstDeps.AddTail((void *)hndFileReg);
				hndFileReg->AddFRHRef();
			}						
			
			//  Mark list as seen by this file:
			pEntryList->m_LastTouch = s_nMyMarker;
		}
		else if (pEntryList == DEP_None)
		{
		    // this is a file that should not be listed
			lToBeScanned.RemoveHead();
			continue;	
		}
		else
		{
			lToBeScanned.RemoveHead();
			if (pEntryList->m_LastTouch == s_nMyMarker)
			{
				continue;				// Already went through this file.
			}
			else						// Add to list;
			{
				pEntryList->m_LastTouch = s_nMyMarker;
				if (hndFileReg != hndItemFile)
				{
					// check to see if this dependency was here before
					UINT depType;
					// If we know we have to change the dependencies anyway (bDirty = TRUE)  
					// avoid doing these costly lookups
					if (!bDirty && depsPrevious.Lookup((void *)hndFileReg, (void *&)depType))
					{
						// may be dirty if the dep. is no longer same type
						// ie. missing or scanned
						if (hndFileReg->ExistsOnDisk())
						{
							if (depType == DEP_Missing)
								bDirty = TRUE;
						}
						else if (hndIncludeEntryItem != NULL)
						{
							CBldFileRegEntry* preg = g_FileRegistry.GetRegEntry(hndIncludeEntryItem);
							CPathW* pPath = (CPathW *)preg->GetFilePath();
							if (pPath && !pPath->ExistsOnDisk())
							{
								if (depType == DEP_Scanned)	
									bDirty = TRUE;
							}
						}

						// move it to the current dep list
						depsPrevious.RemoveKey((void *)hndFileReg);

					}
					else
					{
						bDirty = TRUE;
					}

					// lstDeps takes the existing ref.
					lstDeps.AddTail(hndFileReg);
					hndFileReg->AddFRHRef();
				}
			}
		}

		//	Loop through all the files references by this file, and get the
		//	absolute path name from the reference.  Add this name to our
		//	dependencies list and to the to be scanned list if we haven't
		//	touched it before.
		VCPOSITION posEntryList = pEntryList->GetHeadPosition();
		CStringW strProjectDirectory;
		if (posEntryList != (VCPOSITION)NULL)
		{
			if (spPropContainer != NULL)
			{
				CComBSTR bstrProjDir;
				spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);	// should never have macros in it
				strProjectDirectory = bstrProjDir;
			}
		}
		while (posEntryList != (VCPOSITION)NULL)		
		{
			CBldIncludeEntry *pIncludeEntry = (CBldIncludeEntry *)pEntryList->GetAt(posEntryList);

			if (!pIncludeEntry->FindFile
									((const wchar_t *)bstrIncludePath,
									 dirFile,
									 (const wchar_t *)strProjectDirectory,
									 hndIncludeEntryItem, 
									 m_pItem, 
									 pScannerCache,
									 NULL))
			{
				if (bWriteOutput == VARIANT_TRUE)
				{
					// can't find dependency file so give an error message			
					CStringW str, strFormat;
					BOOL bOK = strFormat.LoadString(IDS_INCLUDE_NOT_FOUND);
					VSASSERT(bOK, "String not found!  Are we pointing at the right place to get our strings from?");

					str.Format(strFormat, pIncludeEntry->m_strReferenceIn, pIncludeEntry->m_nLineNumber,
						pIncludeEntry->m_FileReference);

					if (pEC != NULL)
					{
						CComBSTR bstr = str;
						pEC->AddLine( bstr );
					}
				}

				if (hndIncludeEntryItem == (BldFileRegHandle)NULL)
				{
					pIncludeEntry = (CBldIncludeEntry *)pEntryList->GetNext(posEntryList);
					continue;
				}
			}

			// Try to keep relative paths relative
			if (pIncludeEntry->m_FileReference[0] == L'.')
				g_FileRegistry.GetRegEntry(hndIncludeEntryItem)->SetRelative(TRUE);

			// If this is not a scannble entry, then just check to see if its
			// already a dependendent.  If its not, then add to dependencies
			// list:
			if (!pIncludeEntry->m_bShouldBeScanned)
			{
				VCPOSITION pos;
				for (pos = lstDeps.GetHeadPosition (); pos != NULL; )
				{
					hndFileReg = (BldFileRegHandle) lstDeps.GetAt(pos);
					if (hndFileReg == hndIncludeEntryItem)
						break;

					lstDeps.GetNext (pos);
				}

				// check to see if this dependency was here before
				UINT depType;
				if (!bDirty && depsPrevious.Lookup((void *)hndIncludeEntryItem, (void *&)depType))
				{
					// may be dirty if the dep. is no longer same type
					// ie. missing or scanned
					if (hndIncludeEntryItem->ExistsOnDisk())
					{
						if (depType == DEP_Missing)
							bDirty = TRUE;
					}
					else
					{
						CBldFileRegEntry* preg = g_FileRegistry.GetRegEntry(hndIncludeEntryItem);
						CPathW* pPath = (CPathW *)preg->GetFilePath();
						if (pPath && !pPath->ExistsOnDisk())
						{
							if (depType == DEP_Scanned)	
								bDirty = TRUE;
						}
					}

					// move it to the current dep list
					depsPrevious.RemoveKey((void *)hndIncludeEntryItem);
					lstDeps.AddTail((void *)hndIncludeEntryItem);
					hndIncludeEntryItem->AddFRHRef();
				}
 				else if (pos == (VCPOSITION)NULL && hndFileReg != hndIncludeEntryItem)
				{
					// move it to the current dep list
					lstDeps.AddTail((void *)hndIncludeEntryItem);
					hndIncludeEntryItem->AddFRHRef();
					bDirty = TRUE;
				}
			}
			else
			{
				lToBeScanned.AddTail((void *) hndIncludeEntryItem);
			}

			pEntryList->GetNext(posEntryList);

			hndIncludeEntryItem->ReleaseFRHRef();
		} 	// End of loop through pEntryList.

		if (posEntryList != NULL) 
			break; 	// Bailed from previous loop.
	
	}	// End of loop through lToBeScanned.

	//
	//	We know if we aborted because there are still files to be scanned:
	//
	if (!lToBeScanned.IsEmpty())
	{
		VCPOSITION vcpos = lstDeps.GetHeadPosition();
		while (vcpos != (VCPOSITION)NULL)
		{
			BldFileRegHandle frh = (BldFileRegHandle)lstDeps.GetNext(vcpos);
			frh->ReleaseFRHRef();
		}
		return E_FAIL;
	}

	// if we left entries in the depsPrevious map, free them
	if (depsPrevious.GetStartPosition() != (VCPOSITION)NULL)
		bDirty = TRUE;

	// if we dirtied the dependency list, dirty the project
	if (bDirty)
	{
		CBldHoldRegRecalcs holdRecalc(this, CBldHoldRegRecalcs::eScannedRegSet);

		// clear our dependencies list
		ClearScannedDependencies(pEC);

		// add new ones
		VCPOSITION vcpos = lstDeps.GetHeadPosition();
		while (vcpos != (VCPOSITION)NULL)
		{
			BldFileRegHandle frh = (BldFileRegHandle)lstDeps.GetNext(vcpos);
			AddScannedDependencyFromFrh(frh, pEC);
			frh->ReleaseFRHRef();
		}
	}
	else
	{
		VCPOSITION pos = lstDeps.GetHeadPosition();
		while (pos != (VCPOSITION)NULL)
		{
			BldFileRegHandle frh = (BldFileRegHandle)lstDeps.GetNext(pos);
			frh->ReleaseFRHRef();
		}
	}
	UpdateDependencyInformationTimeStamp();

	return S_OK;
}

void CBldAction::MakeFilesDirty()
{
	if (m_frhInput)
		m_frhInput->MakeDirty();
	if (m_pSets)
		m_pSets->MakeFilesDirty(m_pregistry);
}
void CBldAction::CalcFileAttributes(BOOL fOn)
{
	if (m_frhInput)
		m_frhInput->UpdateAttributes();
	if (m_pSets)
		m_pSets->CalcFileAttributes(fOn);
}

HRESULT CBldAction::GetFileCollection(VCConfiguration* pProjCfg, IEnumVARIANT** ppFiles)
{
	CComPtr<IDispatch> pDispProj;
	HRESULT hr = pProjCfg->get_Project(&pDispProj);
	HR_RETURN_ON_FAIL_OR_NULL(pDispProj, hr);
	CComQIPtr<VCProject> pProj = pDispProj;
	HR_RETURN_ON_NULL(pProj, E_UNEXPECTED);

	CComQIPtr<IVCCollection> pColl;
	CComPtr<IDispatch> pDisp;
	hr = pProj->get_Files(&pDisp);
	pColl = pDisp;
	HR_RETURN_ON_FAIL_OR_NULL(pColl, hr);
	hr = pColl->_NewEnum(reinterpret_cast<IUnknown **>(ppFiles));
	HR_RETURN_ON_FAIL_OR_NULL(*ppFiles, hr);
	(*ppFiles)->Reset();
	return hr;
}

BOOL CBldAction::GetFileCfgForProjCfg(CComVariant& var, VCConfiguration* pProjCfg, VCFileConfiguration** ppFileCfg)
{
	if (var.vt != VT_UNKNOWN && var.vt != VT_DISPATCH)
	{
		VSASSERT(FALSE, "Bad element in file collection.");
		return FALSE;
	}
	IDispatch* pDispFile = var.pdispVal;
	CComQIPtr<IVCFileImpl> pFileImpl = pDispFile;
	RETURN_ON_NULL2(pFileImpl, FALSE);
	HRESULT hr = pFileImpl->GetFileConfigurationForProjectConfiguration(pProjCfg, ppFileCfg);
	if (hr != S_OK || *ppFileCfg == NULL)
		return FALSE;

	return TRUE;
}

BOOL CBldAction::GetFileCfgActions(VCFileConfiguration* pFileCfg, IVCBuildActionList** ppActions)
{
	CComQIPtr<IVCBuildableItem> spItem = pFileCfg;
	RETURN_ON_NULL2(spItem, FALSE);
	return (SUCCEEDED(spItem->get_ActionList(ppActions)));
}

BOOL IsRealProp(long id)
{
	switch (id)
	{
	case VCCFGID_IntrinsicAppliedStyleSheets:
	case VCCFGID_BuildBrowserInformation:
	case VCCFGID_AllOutputsDirty:
	case VCCFGID_PrimaryOutputDirty:
	case VCCFGID_AnyOutputDirty:
	case VCCFGID_AnyToolDirty:
	case VCCFGID_DirtyDuringBuild:
	case VCCLID_CmdLineOptionsDirty:
	case VCCLID_OutputsDirty:
	case VCCLID_ResolvedPdbName:
	case VCLINKID_DependentInputs:
	case VCLINKID_CmdLineOptionsDirty:
	case VCLINKID_OutputsDirty:
	case VCLINKID_LinkerCommandLineGenerated:
	case VCLIBID_DependentInputs:
	case VCLIBID_CmdLineOptionsDirty:
	case VCLIBID_OutputsDirty:
	case VCLIBID_LibCommandLineGenerated:
	case VCMIDLID_CmdLineOptionsDirty:
	case VCMIDLID_OutputsDirty:
	case VCRCID_CmdLineOptionsDirty:
	case VCRCID_OutputsDirty:
	case VCBSCID_RunBSCMakeTool:
	case VCBSCID_SuppressStartupBanner:
	case VCBSCID_OutputFile:
	case VCBSCID_CmdLineOptionsDirty:
	case VCBSCID_OutputsDirty:
	case VCCUSTID_CmdLineOptionsDirty:
	case VCCUSTID_OutputsDirty:
	case VCFCSTID_CmdLineOptionsDirty:
	case VCFCSTID_OutputsDirty:
	case PREBLDID_CmdLineOptionsDirty:
	case PRELINKID_CmdLineOptionsDirty:
	case POSTBLDID_CmdLineOptionsDirty:
	case VCNMAKEID_CmdLineOptionsDirty:
	case VCNMAKEID_OutputsDirty:
	case VCWEBID_CmdLineOptionsDirty:
	case VCWEBID_OutputsDirty:
	case VCDPLYID_CmdLineOptionsDirty:
	case XBOXDPLYID_CmdLineOptionsDirty:
	case XBOXIMAGEID_CmdLineOptionsDirty:
	case XBOXIMAGEID_OutputsDirty:
	case XBOXIMAGEID_ImageCommandLineGenerated:
		return FALSE;
	default:
		return TRUE;	// by default, everything is a real prop that can be saved in the .vcproj file
	}
}
BOOL IsBuildEvent(bldActionTypes buildType)
{
	switch (buildType)
	{
	case TOB_PreBuildEvent:
	case TOB_PreLinkEvent:
	case TOB_PostBuildEvent:
	case TOB_Deploy:
	case TOB_XboxDeploy:
		return TRUE;
	default:
		return FALSE;
	}
}
