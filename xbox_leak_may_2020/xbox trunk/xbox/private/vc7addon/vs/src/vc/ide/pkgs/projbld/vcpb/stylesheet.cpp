#include "stdafx.h"
#include "StyleSheet.h"
#include "vctool.h"
#include "collection.h"
//#include "configuration.h"
#include <register.h>
#include "buildengine.h"

class CInStyleClose
{
public:
	CInStyleClose() { m_bOldInClose = g_bInStyleClose; g_bInStyleClose = TRUE; }
	~CInStyleClose() { g_bInStyleClose = m_bOldInClose; }
	BOOL m_bOldInClose;
};

/////////////////////////////////////////////////////////////////////////////
//

CStyleSheetBase::CStyleSheetBase()
{
}

/* static */
HRESULT CStyleSheetBase::GetDefaultValue(long idProp, BSTR* pbstrValue, IVCPropertyContainer* pPropContainer /* = NULL */)
{
	switch (idProp)
	{
	case VCCFGID_DeleteExtensionsOnClean:
		{
			CComBSTR bstrExt = L"*.obj;*.ilk;*.pdb;*.tlb;*.tli;*.tlh;*.tmp;*.rsp;*.bat;$(TargetPath)";	// call it this unless told otherwise...
			*pbstrValue = bstrExt.Detach();
			return S_OK;
		}
//	case VCCFGID_AppliedStyleSheets:
	default:
		return COptionHandlerBase::GetDefaultString(pbstrValue);
	}
}

HRESULT CStyleSheetBase::GetDefaultValue(long idProp, long* pnValue, IVCPropertyContainer* pPropContainer /* = NULL */)
{
	switch (idProp)
	{
	case VCCFGID_UseOfMFC:
		*pnValue = (long)useMfcStdWin;
		break;
	case VCCFGID_UseOfATL:
		*pnValue = (long)useATLNotSet;
		break;
	case VCCFGID_CharacterSet:
		*pnValue = (long)charSetNotSet;
		break;
	default:
		*pnValue = 0;
	}

	return S_OK;
}

HRESULT CStyleSheetBase::GetDefaultValue(long idProp, VARIANT_BOOL* pbValue, IVCPropertyContainer* pPropContainer /* = NULL */)
{
	switch (idProp)
	{
	case VCCFGID_BuildBrowserInformation:
//	case VCCFGID_ManagedExtensions:
//	case VCCFGID_ATLMinimizesCRunTimeLibraryUsage:
	default:
		return COptionHandlerBase::GetValueFalse(pbValue);
	}
}

// this is a helper for iterating over a semi-colon delimited list of applied style sheets and breaking
// them out so they can be loaded individually; nStartStyle is expected to be initialized to zero and
// then left alone for this method to increment (as its return value)
int CStyleSheetBase::GetNextStyle(CStringW& strStyleList, int nStartStyle, int nMaxIdx, CComBSTR& bstrStyle)
{
	if (nStartStyle < 0)
		return nStartStyle;

	CStringW strStyle;
	CStringW strSemi = L";";
	int nRetVal = g_StaticBuildEngine.GetNextItem(strStyleList, nStartStyle, nMaxIdx, strSemi, strStyle);
	bstrStyle = strStyle;
	return nRetVal;
}

int CStyleSheetBase::FindStyle(CComBSTR& rbstrStyles, BSTR bstrFindStyle)
{
	CStringW strFind = bstrFindStyle;
	if (strFind.IsEmpty())
		return -1;
	strFind.MakeLower();
	CStringW strStylesTmp = rbstrStyles;
	strStylesTmp.MakeLower();
	return DoFindStyle(strStylesTmp, strFind, 0);
}

int CStyleSheetBase::DoFindStyle(CStringW& rstrStyles, CStringW& rstrFindStyle, int nStart)
{
	nStart = rstrStyles.Find(rstrFindStyle, nStart);
	if (nStart < 0)
		return nStart;

	int nStyleLen = rstrStyles.GetLength();
	int nLen = rstrFindStyle.GetLength();

	if (nStart > 0 && nStart + nLen < nStyleLen && rstrStyles[nStart-1] != ';')
		return DoFindStyle(rstrStyles, rstrFindStyle, nStart+nLen);

	else if (nStyleLen > nStart +  nLen && rstrStyles[nStart+nLen] != ';')
		return DoFindStyle(rstrStyles, rstrFindStyle, nStart+nLen);

	return nStart;
}

HRESULT CStyleSheetBase::AddStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, BSTR bstrAddStyle)
{
	if (FindStyle(rbstrStyles, bstrAddStyle) >= 0)
		return S_FALSE;		// already there

	CComBSTR bstrNewStyles = bstrAddStyle;
	if (rbstrStyles.Length() > 0)
		bstrNewStyles += L";";
	bstrNewStyles += rbstrStyles;
	rbstrStyles = bstrNewStyles;

	pContainer->LoadStyle(bstrAddStyle);
	return S_OK;
}

HRESULT CStyleSheetBase::RemoveStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, BSTR bstrRemoveStyle)
{
	int nLoc = FindStyle(rbstrStyles, bstrRemoveStyle);
	if (nLoc < 0)
		return S_FALSE;	// not there

	CComBSTR bstrDeadStyle = bstrRemoveStyle;
	int nLen = bstrDeadStyle.Length();
	CStringW strStylesTmp = rbstrStyles;
	int nStylesLen = strStylesTmp.GetLength();
	rbstrStyles.Empty();
	if (nLoc + nLen < nStylesLen)
		nLen++;		// there's a trailing semi-colon to get, too
	else if (nLoc > 0)
	{
		nLoc--;		// there's a leading semi-colon to get instead since we're at the end
		nLen++;
	}

	if (nLoc == 0)
		strStylesTmp = strStylesTmp.Right(nStylesLen-nLen-nLoc);
	else if (nLoc + nLen == nStylesLen)
		strStylesTmp = strStylesTmp.Left(nLoc);
	else	// pulling out of the middle
	{
		CStringW strLeft = strStylesTmp.Left(nLoc);
		CStringW strRight = strStylesTmp.Right(nStylesLen-nLen-nLoc);
		strStylesTmp = strLeft + strRight;
	}

	rbstrStyles = strStylesTmp;
	pContainer->UnloadStyle(bstrRemoveStyle);
	return S_OK;
}

HRESULT CStyleSheetBase::DoSetUseOfMFC(useOfMfc useMfc, bool bDirtyProp /* = true */)
{
	HRESULT hr = S_OK;
	if (bDirtyProp)
		hr = DoSetIntProperty(VCCFGID_UseOfMFC, FALSE, useMfc);
	RETURN_ON_FAIL(hr);

	CHECK_OK_TO_DIRTY(VCCFGID_IntrinsicAppliedStyleSheets);
	CComBSTR bstrIntrinsicStyles;
	hr = DoGetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, &bstrIntrinsicStyles);
	RETURN_ON_FAIL(hr);
	hr = ChangeMfcStyle(this, bstrIntrinsicStyles, useMfc);
	if (hr == S_OK)
		hr = DoSetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, bstrIntrinsicStyles);
	else if (hr == S_FALSE)
		hr = S_OK;

	return hr;
}

HRESULT CStyleSheetBase::DoSetUseOfATL(useOfATL useATL)
{
	HRESULT hr = DoSetIntProperty(VCCFGID_UseOfATL, FALSE, useATL);
	RETURN_ON_FAIL(hr);

	CHECK_OK_TO_DIRTY(VCCFGID_IntrinsicAppliedStyleSheets);
	CComBSTR bstrIntrinsicStyles;
	DoGetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, &bstrIntrinsicStyles);
	hr = ChangeATLStyle(this, bstrIntrinsicStyles, useATL);
	if (hr == S_OK)
		DoSetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, bstrIntrinsicStyles);
	else if (hr == S_FALSE)
		hr = S_OK;

	return hr;
}

HRESULT CStyleSheetBase::DoSetATLCRTStyle(VARIANT_BOOL bNoCRT, bool bDirtyProp /* = true */)
{
	HRESULT hr = S_OK;
	if (bDirtyProp)
		hr = DoSetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, FALSE, bNoCRT);
	RETURN_ON_FAIL(hr);

	CHECK_OK_TO_DIRTY(VCCFGID_IntrinsicAppliedStyleSheets);
	CComBSTR bstrIntrinsicStyles;
	DoGetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, &bstrIntrinsicStyles);
	hr = ChangeATLCRTStyle(this, bstrIntrinsicStyles, bNoCRT);
	if (hr == S_OK)
		DoSetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, bstrIntrinsicStyles);
	else if (hr == S_FALSE)
		hr = S_OK;

	return hr;
}

HRESULT CStyleSheetBase::DoSetCharSet(charSet set)
{
	HRESULT hr = DoSetIntProperty(VCCFGID_CharacterSet, FALSE, set);
	RETURN_ON_FAIL(hr);

	CHECK_OK_TO_DIRTY(VCCFGID_IntrinsicAppliedStyleSheets);
	CComBSTR bstrIntrinsicStyles;
	DoGetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, &bstrIntrinsicStyles);
	hr = ChangeCharSetStyle(this, bstrIntrinsicStyles, set);
	if (hr == S_OK)
		hr = DoSetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, bstrIntrinsicStyles);
	else if (hr == S_FALSE)
		hr = S_OK;

	return hr;
}

HRESULT CStyleSheetBase::DoSetStyle(long idProp, BSTR bstrStyle, VARIANT_BOOL bValue)
{
	HRESULT hr = DoSetBoolProperty(idProp, FALSE, bValue);
	RETURN_ON_FAIL(hr);

	CComBSTR bstrIntrinsicStyles;
	DoGetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, &bstrIntrinsicStyles);
	if (bValue)
		hr = AddStyle(this, bstrIntrinsicStyles, bstrStyle);
	else
		hr = RemoveStyle(this, bstrIntrinsicStyles, bstrStyle);
	if (hr == S_OK)
		hr = DoSetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, bstrIntrinsicStyles);
	else if (hr == S_FALSE)
		hr = S_OK;

	return hr;
}

useOfMfc CStyleSheetBase::DetermineMfcStyle(CComBSTR& rbstrStyles)
{
	if (FindStyle(rbstrStyles, MFCSTATIC_STYLE) >= 0)
		return useMfcStatic;
	else if (FindStyle(rbstrStyles, MFCDYNAMIC_STYLE) >= 0)
		return useMfcDynamic;
	else
		return useMfcStdWin;
}

HRESULT CStyleSheetBase::AddMfcStyle(CComBSTR& rbstrStyles, useOfMfc useMfc, CStyleSheetBase* pContainerClass)
{
	CComBSTR bstrNewStyle;
	if (useMfc == useMfcStatic)
		bstrNewStyle = MFCSTATIC_STYLE;
	else if (useMfc == useMfcDynamic)
		bstrNewStyle = MFCDYNAMIC_STYLE;
	else	// useMfc = useMfcStdWin
		bstrNewStyle = COREWIN_STYLE;

	return AddStyle(pContainerClass, rbstrStyles, bstrNewStyle);
}

HRESULT CStyleSheetBase::RemoveMfcStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, useOfMfc useMfc)
{
	if (useMfc == useMfcStatic)
		return RemoveStyle(pContainer, rbstrStyles, MFCSTATIC_STYLE);

	else if (useMfc == useMfcDynamic)
		return RemoveStyle(pContainer, rbstrStyles, MFCDYNAMIC_STYLE);

	else	// useMfc = useMfcStdWin
		return RemoveStyle(pContainer, rbstrStyles, COREWIN_STYLE);
}

HRESULT CStyleSheetBase::ChangeMfcStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, useOfMfc useMfc)
{
	HRESULT hrFinal = S_FALSE;
	HRESULT hr = S_FALSE;

	if (useMfc == useMfcStatic)
		hr = AddStyle(pContainer, rbstrStyles, MFCSTATIC_STYLE);
	else	// not useMfcStatic
		hr = RemoveStyle(pContainer, rbstrStyles, MFCSTATIC_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	if (useMfc == useMfcDynamic)
		hr = AddStyle(pContainer, rbstrStyles, MFCDYNAMIC_STYLE);
	else
		hr = RemoveStyle(pContainer, rbstrStyles, MFCDYNAMIC_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	CComBSTR bstrPlatName;
	pContainer->GetStrProperty(VCPLATID_Name, &bstrPlatName);
	bool bPlatformIsXbox = 0 == lstrcmp(bstrPlatName, L"Xbox");

	if(!bPlatformIsXbox){
		// Non-Xbox logic

		if (useMfc == useMfcStdWin)
			hr = AddStyle(pContainer, rbstrStyles, COREWIN_STYLE);
		else
			hr = RemoveStyle(pContainer, rbstrStyles, COREWIN_STYLE);
	}
	else {
		// Xbox specific stuff goes here
	}

	if (hr == S_OK)
		hrFinal = S_OK;

	return hrFinal;
}

HRESULT CStyleSheetBase::ChangeATLStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, useOfATL useATL)
{
	HRESULT hrFinal = S_FALSE;
	HRESULT hr = S_FALSE;

	if (useATL == useATLStatic)
		hr = AddStyle(pContainer, rbstrStyles, ATLSTATIC_STYLE);
	else	// not useATLStatic
		hr = RemoveStyle(pContainer, rbstrStyles, ATLSTATIC_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	if (useATL == useATLDynamic)
		hr = AddStyle(pContainer, rbstrStyles, ATLDYNAMIC_STYLE);
	else
		hr = RemoveStyle(pContainer, rbstrStyles, ATLDYNAMIC_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	return hrFinal;
}

HRESULT CStyleSheetBase::ChangeATLCRTStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, VARIANT_BOOL bNoCRT)
{
	HRESULT hrFinal = S_FALSE;
	HRESULT hr = S_FALSE;

	if (bNoCRT == VARIANT_TRUE)
		hr = AddStyle(pContainer, rbstrStyles, ATLNOCRT_STYLE);
	else	// not useATLStatic
		hr = RemoveStyle(pContainer, rbstrStyles, ATLNOCRT_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	if (bNoCRT == VARIANT_FALSE)
		hr = AddStyle(pContainer, rbstrStyles, ATLWITHCRT_STYLE);
	else
		hr = RemoveStyle(pContainer, rbstrStyles, ATLWITHCRT_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	return hrFinal;
}

HRESULT CStyleSheetBase::ChangeCharSetStyle(CStyleSheetBase* pContainer, CComBSTR& rbstrStyles, charSet set)
{
	HRESULT hrFinal = S_FALSE;
	HRESULT hr = S_FALSE;

	if (set == charSetUnicode)
		hr = AddStyle(pContainer, rbstrStyles, UNICODE_STYLE);
	else
		hr = RemoveStyle(pContainer, rbstrStyles, UNICODE_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	if (set == charSetMBCS)
		hr = AddStyle(pContainer, rbstrStyles, MBCS_STYLE);
	else
		hr = RemoveStyle(pContainer, rbstrStyles, MBCS_STYLE);

	if (hr == S_OK)
		hrFinal = S_OK;

	if (hr == S_OK)
		hrFinal = S_OK;

	return hrFinal;
}

long CStyleSheetBase::GetOverrideID(long idProp)
{
	switch (idProp)
	{
	case VCCFGID_IntermediateDirectory:
	case VCCFGID_OutputDirectory:
	case VCCFGID_Name:
	case VCPROJID_Name:
		return VCCFGID_AllOutputsDirty;	// the case statements here need to match the TRUE return in ConfigBuildProp
		break;
	default:
		return -1;
	};
}

HRESULT CStyleSheetBase::DoGenerateToolWrapperList()
{
	DoClearToolWrapperList();

	// the code below here picks up the tool list in effect when the configuration is created during this session
	// or when the configuration type is changed.
	// DIANEME_TODO: CStyleSheetBase::GenerateToolWrappers -- make sure we dynamically pick up any tools added later
	RETURN_ON_NULL2(m_pPlatform, E_UNEXPECTED);

	CComQIPtr<IVCCollection> spCollection;
	CComPtr<IDispatch> pDisp;
	HRESULT hr = DoGetTools(&pDisp);
	spCollection = pDisp;
	if (FAILED(hr) || spCollection == NULL)
	{
		VSASSERT(FALSE, "No tools!?!");
		return hr;
	}

	CComPtr<IEnumVARIANT> pEnum;
	hr = spCollection->_NewEnum(reinterpret_cast<IUnknown **>(&pEnum));
	RETURN_ON_FAIL_OR_NULL2(hr, pEnum, E_UNEXPECTED);

	pEnum->Reset();
	while (TRUE)
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		if (hr != S_OK)
			break;
		else if (var.vt != VT_UNKNOWN)
		{
			VSASSERT(FALSE, "Bad member of the collection");
			continue;
		}
		IUnknown* pUnkTool = var.punkVal;
		CComQIPtr<IVCToolImpl> spTool = pUnkTool;
		if (spTool == NULL)	// some lists appear to end in NULL...
			continue;

		CBldToolWrapper* pToolWrapper = new CBldToolWrapper(spTool);
		m_toolWrapperList.AddTail(pToolWrapper);
	}

	return S_OK;
}

HRESULT CStyleSheetBase::DoGetPlatform(IDispatch** ppVal)
{
	CHECK_POINTER_VALID(ppVal);
	*ppVal = NULL;
		
	if (m_pPlatform)
		return m_pPlatform.QueryInterface(ppVal);

	return S_OK;
}

HRESULT CStyleSheetBase::DoGetTools(IDispatch** ppVal)
{
	CHECK_POINTER_VALID(ppVal);
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<IVCToolImpl>::CreateInstance(&pCollection, &m_rgTools);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVal);
	}
	return hr;
}

HRESULT CStyleSheetBase::DoGetFileTools(IVCCollection** ppVal)
{
	CHECK_POINTER_VALID(ppVal);
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<IVCToolImpl>::CreateInstance(&pCollection, &m_rgFileTools);
	if (SUCCEEDED(hr))
		*ppVal = pCollection.Detach();
	return hr;
}

HRESULT CStyleSheetBase::DoGetStyleSheets(IDispatch** ppVal)
{
	CHECK_POINTER_VALID(ppVal);

	m_rgStyleSheets.Clear();
	long lcItems = m_rgParents.GetSize();
	for (long l = 0; l < lcItems; l++)
	{
		CComQIPtr<VCStyleSheet> pStyle = m_rgParents.GetAt(l);
		if (pStyle)
			m_rgStyleSheets.AddTail(pStyle);
	}

	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCStyleSheet>::CreateInstance(&pCollection, &m_rgStyleSheets);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVal);
	}
	return hr;
}

HRESULT CStyleSheetBase::DoClearToolWrapperList()
{
	CBldToolWrapper* pToolWrapper;
	while (!m_toolWrapperList.IsEmpty())
	{
		pToolWrapper = (CBldToolWrapper*)m_toolWrapperList.RemoveHead();
		delete pToolWrapper;
	}

	return S_OK;
}

HRESULT CStyleSheetBase::FinishClose()
{
	long lcItems = m_rgParents.GetSize();
	for (long l = 0; l < lcItems; l++)
	{
		CComPtr<IVCPropertyContainer> spPropContainer;
		spPropContainer = m_rgParents.GetAt(l);
		CComQIPtr<IVCStyleSheetImpl> spStyleSheetImpl = spPropContainer;
		if (spStyleSheetImpl)
			spStyleSheetImpl->RemoveDependent(this, !g_bInProjClose);
	}
	m_rgStyleSheets.RemoveAll();	// this is just a duplicate collection, so don't need to do anything special

	m_pPlatform.Release();
	ClearTools();

	return CPropertyContainerImpl::FinishClose();
}

void CStyleSheetBase::ClearTools()
{
	long l, lcItems;
	lcItems = m_rgTools.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(l);
		if (pToolImpl)
			pToolImpl->Close(); // close down entirely; release all
	}
	m_rgTools.RemoveAll();
	m_rgFileTools.RemoveAll();	// this is a proper subset of m_rgTools, so close happened above

	DoClearToolWrapperList();
}

toolSetType CStyleSheetBase::DetermineBaseType(ConfigurationTypes configType)
{
	switch (configType)
	{
	case typeUnknown:
		return toolSetMakefile;
		break;
	case typeApplication:
	case typeDynamicLibrary:
		return toolSetLinker;
		break;
	case typeStaticLibrary:
		return toolSetLibrarian;
		break;
	default:
		VSASSERT(FALSE, "Illegal configuration type");
		// fall through
	case typeGeneric:
		return toolSetUtility;
		break;
	}
}

HRESULT CStyleSheetBase::DoSetAppliedStyleSheets(BSTR bstrStyles, BOOL bCheckSpecial)
{
	if (bCheckSpecial)
	{
		CHECK_OK_TO_DIRTY(VCCFGID_AppliedStyleSheets);
		CComBSTR bstrNewStyles = bstrStyles;
		int nNewLen = bstrNewStyles.Length();
		int nStartStyle = 0, nMaxIdx;
		CComBSTR bstrOldStyles;
		CStringW strStyleList;
		HRESULT hr = DoGetStrProperty(VCCFGID_AppliedStyleSheets, FALSE, &bstrOldStyles);
		if (hr == S_OK && bstrOldStyles.Length() > 0)
		{
			strStyleList = bstrOldStyles;
			nMaxIdx = strStyleList.GetLength();
			while (nStartStyle >= 0)
			{
				CComBSTR bstrStyle;
				nStartStyle = GetNextStyle(strStyleList, nStartStyle, nMaxIdx, bstrStyle);
				if (bstrStyle.Length() > 0 && (nNewLen == 0 || FindStyle(bstrNewStyles, bstrStyle) < 0))
					UnloadStyle(bstrStyle);
			}
		}
		if (nNewLen > 0)
		{
			strStyleList = bstrNewStyles;
			nStartStyle = 0;
			nMaxIdx = strStyleList.GetLength();
			while (nStartStyle >= 0)
			{
				CComBSTR bstrStyle;
				nStartStyle = GetNextStyle(strStyleList, nStartStyle, nMaxIdx, bstrStyle);
				if (bstrStyle.Length() > 0)
					LoadStyle(bstrStyle, FALSE, FALSE);
			}
		}
	}
	return DoSetStrProperty(VCCFGID_AppliedStyleSheets, FALSE, bstrStyles);
}

void CStyleSheetBase::ResolveStyleSheetToPath(BSTR bstrStyle, CComBSTR& bstrStylePath)
{
	// DIANEME_TODO: right now, we're going to assume that style sheets are always in the VCProjectDefaults folder
	// This needs to be updated to either check along a path or go to whatever the right place might be.
	CComBSTR bstrPath;
	HRESULT hr = VCGetEnvironmentVariableW(L"VC_STYLESHEETS_PATH",&bstrPath);
	if( FAILED(hr) )
	{
	    CStringW	strPath;
	    UtilGetVCInstallFolder(strPath);
	    strPath += L"VCProjectDefaults\\";
	    strPath += bstrStyle;
	    bstrStylePath = strPath;
	}
	else
	{
	    bstrStylePath = bstrPath;
	    bstrStylePath.Append(bstrStyle);
	}
}

HRESULT CStyleSheetBase::LoadStyle(BSTR bstrStyle, BOOL bLoadFront /* = TRUE */, BOOL bIsSystemStyle /* = TRUE */)
{
	CComQIPtr<IVCProjectEngineImpl> spProjectEngineImpl = g_pProjectEngine;
	if (spProjectEngineImpl == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CComBSTR bstrStylePath;
	ResolveStyleSheetToPath(bstrStyle, bstrStylePath);

	CComPtr<IDispatch> spDispStyleSheet;
	HRESULT hr = spProjectEngineImpl->LoadHiddenStyleSheet(bstrStylePath, &spDispStyleSheet);
	if (FAILED(hr))
	{
		// DIANEME_TODO: do we want to put something in the task list if we fail to load a style sheet?
		CStringW strError;
		strError.Format(bIsSystemStyle ? IDS_E_SYS_STYLE_LOAD_FAILED : IDS_E_USER_STYLE_LOAD_FAILED, bstrStylePath);
		CComBSTR bstrError = strError;
		// DIANEME_TODO: do we want to push a help topic keyword when we report a stylesheet load error?
		spProjectEngineImpl->DoFireReportError(bstrError, hr, NULL);
		return hr;
	}

	CComQIPtr<IVCStyleSheetImpl> spStyleSheetImpl = spDispStyleSheet;
	CComQIPtr<IVCPropertyContainer> spPropContainer = spDispStyleSheet;
	if (spStyleSheetImpl == NULL || spPropContainer == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	hr = spStyleSheetImpl->AddDependent(this, !g_bInProjLoad);
	if (hr == S_OK)	// not already there
	{
		if (bLoadFront)
			m_rgParents.AddHead(spPropContainer);
		else
			m_rgParents.AddTail(spPropContainer);
	}

	return hr;
}

HRESULT CStyleSheetBase::UnloadStyle(BSTR bstrStyle)
{
	CComQIPtr<IVCProjectEngineImpl> spProjectEngineImpl = g_pProjectEngine;
	if (spProjectEngineImpl == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CComPtr<IDispatch> spDispStyleSheetList;
	HRESULT hr = spProjectEngineImpl->get_AllStyleSheets(&spDispStyleSheetList);

	CComQIPtr<IVCCollection> spStyleSheetList = spDispStyleSheetList;
	RETURN_ON_FAIL_OR_NULL2(hr, spStyleSheetList, E_UNEXPECTED);

	CComBSTR bstrStylePath;
	ResolveStyleSheetToPath(bstrStyle, bstrStylePath);
	CComPtr<IDispatch> spDispStyleSheet;
	hr = spStyleSheetList->Item(CComVariant(bstrStylePath), &spDispStyleSheet);
	RETURN_ON_FAIL_OR_NULL2(hr, spDispStyleSheet, S_FALSE);		// not a problem not to find it

	CComQIPtr<IVCPropertyContainer> spPropContainer = spDispStyleSheet;
	CComQIPtr<IVCStyleSheetImpl> spStyleSheetImpl = spDispStyleSheet;
	if (spPropContainer == NULL || spStyleSheetImpl == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	m_rgParents.Remove(spPropContainer);
	spStyleSheetImpl->RemoveDependent(this, !g_bInProjClose);

	return S_OK;
}

HRESULT CStyleSheetBase::InitializeTools(void)
{
	ClearTools();	// in case we've been here before...

	// fill in tools collection on the config
	HRESULT hr;
	CComQIPtr<IVCCollection> pColl;
	CComQIPtr<IVCPropertyContainer> pPropContainer = this;
	CComPtr<IEnumVARIANT> pEnumVar;
	CComPtr<IDispatch> pDisp;

	CComQIPtr<IVCPlatformImpl> spPlatformImpl = m_pPlatform;
	if (m_pPlatform == NULL || spPlatformImpl == NULL)
		return E_UNEXPECTED;

	// walk the platform tool collection
	hr = m_pPlatform->get_Tools(&pDisp);
	pColl = pDisp;
	RETURN_ON_FAIL(hr);
	if (pColl == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	// get the enum
	hr = pColl->_NewEnum(reinterpret_cast<IUnknown **>(&pEnumVar));
	RETURN_ON_FAIL(hr);
	if (pEnumVar == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	toolSetType listStyle;
	DoGetToolSetType(listStyle);

	if( !pPropContainer )
		return E_NOINTERFACE;

	hr = pEnumVar->Reset();
	while (true)
	{
		CComVariant varRes;
		hr = pEnumVar->Next(1, &varRes, NULL);
		if (hr == S_OK && (varRes.vt == VT_DISPATCH || varRes.vt == VT_UNKNOWN))
		{
			CComQIPtr<IVCToolImpl> pTool = varRes.pdispVal;
			VARIANT_BOOL bInToolset = VARIANT_FALSE;
			hr = spPlatformImpl->IsToolInToolset(listStyle, pTool, &bInToolset);
			RETURN_ON_FAIL(hr);
			if (!bInToolset)
				continue;

			hr = AddSingleTool(pPropContainer, pTool, FALSE);
			RETURN_ON_FAIL(hr);
		}
		else
			break;
	}

	DoGenerateToolWrapperList();

	VerifyFileTools();

	return S_OK;
}

HRESULT CStyleSheetBase::AddSingleTool(IVCPropertyContainer* pPropContainer, IVCToolImpl* pTool, BOOL bGenWrapper,
	IDispatch** ppDispTool /* = NULL */)
{
	if (ppDispTool)
		*ppDispTool = NULL;

	CComPtr<IVCToolImpl> pCfgTool;
	// IVCToolImpl::CreateToolObject on each tool (passing VCConfiguration)
	HRESULT hr = pTool->CreateToolObject(pPropContainer, &pCfgTool);
	RETURN_ON_FAIL(hr);
	if (pCfgTool == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NO_TOOL, IDS_ERR_NO_MATCHING_TOOL);
	// stuff into collection (m_rgTools)
	m_rgTools.Add(pCfgTool);
	// determine whether we're dealing with a file tool and stuff into collection if it is (m_rgFileTools)
	VARIANT_BOOL bIsFileTool = VARIANT_FALSE;
	pCfgTool->get_IsFileTool(&bIsFileTool);
	if (bIsFileTool)
		m_rgFileTools.Add(pCfgTool);

	if (ppDispTool)
	{
		CComQIPtr<IDispatch> pDispRealTool = pCfgTool;
		*ppDispTool = pDispRealTool.Detach();
	}

	if (bGenWrapper)
	{
		CBldToolWrapper* pToolWrapper = new CBldToolWrapper(pCfgTool);
		m_toolWrapperList.AddTail(pToolWrapper);
	}

	return S_OK;
}

HRESULT CStyleSheetBase::DoSetProp(long id, BOOL bCheckSpecial, VARIANT varValue, long nOverride /* = -1 */)
{
	CHECK_OK_TO_DIRTY(id);

	if (bCheckSpecial)
	{
		switch (varValue.vt)
		{
		case VT_I2:
			return DoSetBoolProperty(id, TRUE, varValue.boolVal);
		case VT_I4:
			return DoSetIntProperty(id, TRUE, varValue.lVal);
		case VT_BSTR:
			return DoSetStrProperty(id, TRUE, varValue.bstrVal);
		// default case is to do nothing here
		}
	}

	return CPropertyContainerImpl::DoSetProp(id, bCheckSpecial, varValue, nOverride);
}

HRESULT CStyleSheetBase::DoGetProp(long id, BOOL bCheckSpecialProps, VARIANT *pVarValue)
{
	// check if it is a special prop that really has local storage (or belongs to a non-property container parent 
	// who does) first
	if (bCheckSpecialProps)
	{
		CHECK_POINTER_NULL(pVarValue);
		CComBSTR bstrProp;
		if (DoGetStrProperty(id, TRUE, &bstrProp) == S_OK)
		{
			pVarValue->bstrVal = bstrProp.Detach();
			pVarValue->vt = VT_BSTR;
			return S_OK;
		}
	}

	return CPropertyContainerImpl::DoGetProp(id, bCheckSpecialProps, pVarValue);
}

HRESULT CStyleSheetBase::DoSetStrProperty(long idProp, BOOL bCheckSpecial, BSTR bstrValue, long nOverrideID /* = -1 */)
{
	if (bCheckSpecial)
	{
		if (idProp == VCCFGID_AppliedStyleSheets)
			return DoSetAppliedStyleSheets(bstrValue, TRUE);
	}

	return CPropertyContainerImpl::DoSetStrProperty(idProp, bCheckSpecial, bstrValue, GetOverrideID(idProp));
}

HRESULT CStyleSheetBase::DoSetIntProperty(long idProp, BOOL bCheckSpecial, long nValue, long nOverrideID /* = -1 */)
{
	if (bCheckSpecial)
	{
		if (idProp == VCCFGID_UseOfMFC || idProp == VCCFGID_UseOfATL || idProp == VCCFGID_CharacterSet)
		{
			long nOldProp;
			HRESULT hr = DoGetIntProperty(idProp, FALSE, (long *)&nOldProp);
			if (hr == S_OK && nValue == nOldProp)
				return S_OK;	// nothing to do
			
			switch (idProp)
			{
			case VCCFGID_UseOfMFC:
				return DoSetUseOfMFC((useOfMfc)nValue);
			case VCCFGID_UseOfATL:
				return DoSetUseOfATL((useOfATL)nValue);
			case VCCFGID_CharacterSet:
				return DoSetCharSet((charSet)nValue);
			}
		}
	}

	return CPropertyContainerImpl::DoSetIntProperty(idProp, bCheckSpecial, nValue, nOverrideID);
}

HRESULT CStyleSheetBase::DoSetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL bValue, long nOverrideID /* = -1 */)
{
	if (bCheckSpecial)
	{
		if (idProp == VCCFGID_BuildBrowserInformation || idProp == VCCFGID_ManagedExtensions 
			|| idProp == VCCFGID_ATLMinimizesCRunTimeLibraryUsage || idProp == VCCFGID_WholeProgramOptimization)
		{
			VARIANT_BOOL bOldValue;
			HRESULT hr = DoGetBoolProperty(idProp, FALSE, &bOldValue);
			if (hr == S_OK && bOldValue == bValue)
				return S_OK;	// nothing to do
			switch (idProp)
			{
			case VCCFGID_BuildBrowserInformation:
				return DoSetStyle(idProp, BSCMAKE_STYLE, bValue);
			case VCCFGID_ManagedExtensions:
				return DoSetStyle(idProp, MANAGED_STYLE, bValue);
			case VCCFGID_ATLMinimizesCRunTimeLibraryUsage:
				return DoSetATLCRTStyle(bValue);
			default:
				VSASSERT(idProp == VCCFGID_WholeProgramOptimization, "Hey, bad case statement!");
				return DoSetStyle(idProp, WHOLEPROGOPTIMIZE_STYLE, bValue);
			}
		}
	}

	return CPropertyContainerImpl::DoSetBoolProperty(idProp, FALSE, bValue);
}

HRESULT CStyleSheetBase::DoClear(long id, long nOverrideID /* = -1 */)
{
	CHECK_OK_TO_DIRTY(id);

	switch (id)		// to fix up the applied styles
	{
	case VCCFGID_BuildBrowserInformation:
	case VCCFGID_ManagedExtensions:
		DoSetBoolProperty(id, FALSE, VARIANT_FALSE);
		break;
	case VCCFGID_UseOfMFC:
		DoSetIntProperty(VCCFGID_UseOfMFC, FALSE, useMfcStdWin);
		break;
	case VCCFGID_ConfigurationType:
		{
			long nOldValue;
			GetParentIntProperty(id, &nOldValue);
			DoSetIntProperty(id, FALSE, nOldValue);	// allow intrinsic style sheets to clear out
		}
		break;
	// nothing to do for default
	};

	return CPropertyContainerImpl::DoClear(id, nOverrideID);
}

STDMETHODIMP CStyleSheetBase::IsMultiProp(long idProp, VARIANT_BOOL* pbIsMulti)
{
	CHECK_POINTER_NULL(pbIsMulti);
	*pbIsMulti = VARIANT_FALSE;

	// let's check locally first
	if (idProp >= VCCONFIGURATION_MIN_DISPID && idProp <= VCCONFIGURATION_MAX_DISPID)
		return S_OK;	// belongs to a config; no multi-props here
	else if (idProp >= VCSTYLESHEET_MIN_DISPID && idProp <= VCSTYLESHEET_MAX_DISPID)
		return S_OK;	// belongs to a style sheet; no multi-props here

	// try our tools
	HRESULT hr = S_FALSE;
	long lcItems = m_rgTools.GetSize();
	// if a tool owns an ID, it will return S_OK regardless of whether the property is a multi-prop or not
	for (long idx = 0; hr != S_OK && idx < lcItems; idx++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(idx);
		if (pToolImpl)
			hr = pToolImpl->IsMultiProp(idProp, pbIsMulti);
	}

	return hr;
}

STDMETHODIMP CStyleSheetBase::GetMultiPropSeparator(long idProp, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	CHECK_POINTER_NULL(pbstrPreferred);
	CHECK_POINTER_NULL(pbstrAll);
	*pbstrPreferred = NULL;
	*pbstrAll = NULL;

	// let's check locally first
	if (idProp >= VCCONFIGURATION_MIN_DISPID && idProp <= VCCONFIGURATION_MAX_DISPID)
		return S_OK;	// belongs to a config; no multi-props here
	else if (idProp >= VCSTYLESHEET_MIN_DISPID && idProp <= VCSTYLESHEET_MAX_DISPID)
		return S_OK;	// belongs to a style sheet; no multi-props here

	// try our tools
	HRESULT hr = S_FALSE;
	long lcItems = m_rgTools.GetSize();
	// if a tool owns an ID, it will return S_OK regardless of whether the property is a multi-prop or not
	for (long idx = 0; hr != S_OK && idx < lcItems; idx++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(idx);
		if (pToolImpl)
			hr = pToolImpl->GetMultiPropSeparator(idProp, pbstrPreferred, pbstrAll);
	}

	return hr;
}

HRESULT CStyleSheetBase::DoAddTool(LPCOLESTR szToolName, IDispatch** ppTool)
{
	CHECK_POINTER_VALID(ppTool);
	*ppTool = NULL;

	CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
	RETURN_ON_NULL2(spProjEngineImpl, E_UNEXPECTED);

	CComPtr<IDispatch> spDispTools;
	HRESULT hr = spProjEngineImpl->get_VendorTools(&spDispTools);
	CComQIPtr<IVCCollection> spTools = spDispTools;
	RETURN_ON_FAIL_OR_NULL2(hr, spTools, E_UNEXPECTED);

	CComPtr<IDispatch> spDispBaseTool;
	hr = spTools->Item(CComVariant(szToolName), &spDispBaseTool);
	if (spDispBaseTool == NULL)	// not already present
		hr = spProjEngineImpl->AddVendorTool(szToolName, NULL, &spDispBaseTool);
	CComQIPtr<IVCToolImpl> spBaseTool = spDispBaseTool;
	RETURN_ON_FAIL_OR_NULL2(hr, spBaseTool, E_UNEXPECTED);

	return AddSingleTool(this, spBaseTool, TRUE, ppTool);
}

/////////////////////////////////////////////////////////////////////////////
//
CStyleSheet::CStyleSheet()
{
	m_bDirty = VARIANT_FALSE;
	m_nLoadCount = 0;
}

HRESULT CStyleSheet::CreateInstance(VCStyleSheet **ppVCStyleSheet)
{
	CHECK_POINTER_NULL(ppVCStyleSheet);

	CStyleSheet *pVar;
	CComObject<CStyleSheet> *pObj;
	HRESULT hr = CComObject<CStyleSheet>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		pVar->Initialize();
		*ppVCStyleSheet = pVar;
	}
	return hr;
}

HRESULT CStyleSheet::Initialize()
{
	toolSetType ToolSet;
	SGetToolSet(&ToolSet);
	return DoSetToolSet(ToolSet, TRUE);
}

HRESULT CStyleSheet::DoSetToolSet(toolSetType toolSet, BOOL bForceUpdate /* = FALSE */)
{
	if (!bForceUpdate)
	{
		toolSetType oldToolSet;
		GetIntProperty(VCSTYLEID_ToolSet, (long *)&oldToolSet);
		if (oldToolSet == toolSet)
			return S_OK;	// no-op
	}

	HRESULT hr = DoSetIntProperty(VCSTYLEID_ToolSet, FALSE, toolSet);
	RETURN_ON_FAIL(hr);

	InitializeTools();

	return hr;
}

HRESULT CStyleSheet::DoSetIntProperty(long idProp, BOOL bCheckSpecial, long nValue, long nOverrideID /* = -1 */)
{
	if (bCheckSpecial)
	{
		if (idProp == VCSTYLEID_ToolSet)
			return DoSetToolSet((toolSetType)nValue);
	}

	return CStyleSheetBase::DoSetIntProperty(idProp, bCheckSpecial, nValue, nOverrideID);
}

STDMETHODIMP CStyleSheet::get_StyleSheetName(BSTR * pVal)
{
	CHECK_POINTER_VALID(pVal);
	return m_bstrName.CopyTo(pVal);
}

STDMETHODIMP CStyleSheet::put_StyleSheetName(BSTR bstrVal)
{
	CHECK_OK_TO_DIRTY(VCSTYLEID_StyleSheetName);
	m_bstrName = bstrVal;
	return S_OK;
}

STDMETHODIMP CStyleSheet::get_StyleSheetFile(BSTR * pVal)
{
	CHECK_POINTER_VALID( pVal );
	CComBSTR bstrStyleSheetFile;

	bstrStyleSheetFile = m_bstrFullDir;
	bstrStyleSheetFile.Append(m_bstrFileName);
	*pVal = bstrStyleSheetFile.Detach();
	return S_OK;
}

STDMETHODIMP CStyleSheet::put_StyleSheetFile(BSTR bstrVal)
{
	// validate name
	// check for all whitespace or 0 length string
	CStringW strFile = bstrVal;
	strFile.TrimLeft();
	strFile.TrimRight();
	if( strFile.IsEmpty() )
		return S_OK;

	CPathW pathFile;
	if (!pathFile.Create(strFile))
		return E_FAIL;

	CDirW dirStyle;
	if (!dirStyle.CreateFromPath(pathFile))
		return E_FAIL;

	CHECK_OK_TO_DIRTY(VCSTYLEID_StyleSheetFile);

	strFile = pathFile.GetFileName();
	CStringW strStyleDir = dirStyle;

	m_bstrFileName = strFile;
	m_bstrFullDir = strStyleDir;

	return S_OK;
}

STDMETHODIMP CStyleSheet::get_Name(BSTR * pVal)
{
	CHECK_POINTER_VALID(pVal);
	return m_bstrName.CopyTo(pVal);
}

STDMETHODIMP CStyleSheet::put_Name(BSTR newVal)
{
	CHECK_OK_TO_DIRTY(VCSTYLEID_Name);
	m_bstrName = newVal;

	if (m_bstrFileName.Length() == 0)
		put_StyleSheetFile(newVal); // assume we haven't done this either...

	return S_OK;
}

STDMETHODIMP CStyleSheet::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	*pbMatched = VARIANT_FALSE;
	if (!bFullOnly && _wcsicmp(m_bstrName,bstrNameToMatch) == 0)
	{
		*pbMatched = VARIANT_TRUE;
	}
	else if (m_bstrFileName == bstrNameToMatch)
	{
		*pbMatched = VARIANT_TRUE;
	}
	else
	{
		CComBSTR bstrName = m_bstrFullDir;
		int nLen = bstrName.Length();
		if (nLen > 0 && bstrName[nLen-1] != L'\\' && bstrName[nLen-1] != L'/')
			bstrName += L"\\";
		bstrName += m_bstrFileName;
		if (_wcsicmp(bstrName,bstrNameToMatch) == 0)
		{
			*pbMatched = VARIANT_TRUE;
		}
		else
		{
			CPathW pathSrc, pathMatch; // to handle case where shell passes in something
			CStringW strMatch; //    like "X:\\tests\mfcone\mfcone.vcstyle"
			CStringW strName = bstrName;
			strMatch = bstrNameToMatch;
			pathSrc.Create(strName);
			pathMatch.Create(strMatch);
			if (pathSrc == pathMatch)
			{
				*pbMatched = VARIANT_TRUE;
			}
		}
	}
	return S_OK;
}

STDMETHODIMP CStyleSheet::get_IsDirty(VARIANT_BOOL *pBoolDirty)
{
	CHECK_POINTER_VALID( pBoolDirty );
	*pBoolDirty = m_bDirty;
	return S_OK;
}

STDMETHODIMP CStyleSheet::put_IsDirty(VARIANT_BOOL boolDirty)
{
	// can't dirty while in style sheet load
	if( !g_bInStyleLoad )
	{
		if( boolDirty && !m_bDirty )
		{
			// FUTURE: when we allow for changing style sheets, we need to hook up to SCC to figure out if we're allowed to do this
		}

		m_bDirty = boolDirty;
	}
	return S_OK;
}


STDMETHODIMP CStyleSheet::SaveObject( IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent )
{
	return E_NOTIMPL;
}

STDMETHODIMP CStyleSheet::get_ToolSet(toolSetType* pToolSet)
{
	CHECK_POINTER_VALID(pToolSet);
	HRESULT hr = GetIntProperty(VCSTYLEID_ToolSet, (long *)pToolSet);
	if (hr == S_FALSE)
		SGetToolSet(pToolSet);
	return hr;
}

STDMETHODIMP CStyleSheet::put_ToolSet(toolSetType ToolSet)
{
	if (ToolSet < toolSetUtility || ToolSet > toolSetAll)
		RETURN_INVALID();	// out of range

	return SetIntProperty(VCSTYLEID_ToolSet, ToolSet);
}

STDMETHODIMP CStyleSheet::get_IntermediateDirectory(BSTR * pVal)
{
	return GetStrProperty(VCCFGID_IntermediateDirectory, pVal);
}

STDMETHODIMP CStyleSheet::put_IntermediateDirectory(BSTR newVal)
{
	return SetStrProperty(VCCFGID_IntermediateDirectory, newVal);
}

STDMETHODIMP CStyleSheet::get_OutputDirectory(BSTR * pVal)
{
	return GetStrProperty(VCCFGID_OutputDirectory, pVal);
}

STDMETHODIMP CStyleSheet::put_OutputDirectory(BSTR newVal)
{
	return SetStrProperty(VCCFGID_OutputDirectory, newVal);
}

STDMETHODIMP CStyleSheet::get_Tools(IDispatch **ppVal)
{
	return DoGetTools(ppVal);
}

STDMETHODIMP CStyleSheet::get_FileTools(IVCCollection **ppVal)
{
	return DoGetFileTools(ppVal);
}

STDMETHODIMP CStyleSheet::Close()
{
	CInStyleClose inClose;
	m_rgDependents.RemoveAll();

	return FinishClose();
}

STDMETHODIMP CStyleSheet::get_LoadCount(long* pnLoadCount)
{	// how many projects/style sheets have this style sheet loaded; used by project engine only
	CHECK_POINTER_NULL(pnLoadCount);
	*pnLoadCount = m_nLoadCount;
	return S_OK;
}

STDMETHODIMP CStyleSheet::put_LoadCount(long nLoadCount)
{
	if (nLoadCount < 0)
	{
		RETURN_INVALID();
	}
	m_nLoadCount = nLoadCount;
	return S_OK;
}

HRESULT CStyleSheet::SGetToolSet(toolSetType* pToolSet)
{
	CHECK_POINTER_NULL(pToolSet);
	*pToolSet = toolSetAll;
	return S_OK;
}

STDMETHODIMP CStyleSheet::get_ConfigurationType(ConfigurationTypes* pconfigType)
{
	CHECK_POINTER_VALID(pconfigType);
	return GetIntProperty(VCCFGID_ConfigurationType, (long *)pconfigType);
}

STDMETHODIMP CStyleSheet::put_ConfigurationType(ConfigurationTypes configType)
{
	return SetIntProperty(VCCFGID_ConfigurationType, configType);
}

STDMETHODIMP CStyleSheet::get_Platform(IDispatch** ppPlatform)
{
	return DoGetPlatform(ppPlatform);
}

STDMETHODIMP CStyleSheet::get_PlatformName(BSTR* pbstrPlatformName)
{
	CHECK_POINTER_VALID(pbstrPlatformName);
	if (m_pPlatform == NULL)
	{
		*pbstrPlatformName = NULL;
		return S_FALSE;
	}

	return m_pPlatform->get_Name(pbstrPlatformName);
}

STDMETHODIMP CStyleSheet::put_PlatformName(BSTR bstrPlatformName)
{
	RETURN_ON_NULL2(g_pProjectEngine, E_UNEXPECTED);

	CComPtr<IDispatch> spDispPlatformList;
	HRESULT hr = g_pProjectEngine->get_Platforms(&spDispPlatformList);
	CComQIPtr<IVCCollection> spPlatformList = spDispPlatformList;
	RETURN_ON_FAIL_OR_NULL2(hr, spPlatformList, E_UNEXPECTED);

	CComPtr<IDispatch> spDispPlatform;
	hr = spPlatformList->Item(CComVariant(bstrPlatformName), &spDispPlatform);
	CComQIPtr<VCPlatform> spPlatform = spDispPlatform;
	RETURN_ON_FAIL(hr);
	if (spPlatform == NULL)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_PLATFORM_NOT_FOUND, bstrPlatformName);

	CHECK_OK_TO_DIRTY(VCSTYLEID_PlatformName);
	m_pPlatform = spPlatform;
	InitializeTools();
	return S_OK;
}

STDMETHODIMP CStyleSheet::get_AppliedStyleSheets(BSTR* pbstrStyles)
{
	HRESULT hr = GetStrProperty(VCCFGID_AppliedStyleSheets, pbstrStyles);
	if (hr == S_FALSE)
		GetDefaultValue(VCCFGID_AppliedStyleSheets, pbstrStyles, this);
	return hr;
}

STDMETHODIMP CStyleSheet::put_AppliedStyleSheets(BSTR bstrStyles)
{
	return DoSetAppliedStyleSheets(bstrStyles, TRUE);
}

STDMETHODIMP CStyleSheet::get_BuildBrowserInformation(VARIANT_BOOL* pbBsc)
{	// build .bsc file
	CHECK_POINTER_VALID(pbBsc);
	return GetBoolProperty(VCCFGID_BuildBrowserInformation, pbBsc);
}

STDMETHODIMP CStyleSheet::put_BuildBrowserInformation(VARIANT_BOOL bBsc)
{
	return SetBoolProperty(VCCFGID_BuildBrowserInformation, bBsc);
}

STDMETHODIMP CStyleSheet::get_UseOfMFC(useOfMfc* useMfc)
{	// how are we using MFC?
	return GetIntProperty(VCCFGID_UseOfMFC, (long *)useMfc);
}

STDMETHODIMP CStyleSheet::put_UseOfMFC(useOfMfc useMfc)
{
	return SetIntProperty(VCCFGID_UseOfMFC, useMfc);
}

STDMETHODIMP CStyleSheet::get_UseOfATL(useOfATL* useATL)
{	// how are we using ATL?
	CHECK_POINTER_VALID(useATL);
	return GetIntProperty(VCCFGID_UseOfATL, (long *)useATL);
}

STDMETHODIMP CStyleSheet::put_UseOfATL(useOfATL useATL)
{
	return SetIntProperty(VCCFGID_UseOfATL, useATL);
}

STDMETHODIMP CStyleSheet::get_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL* pbCRT)
{	// is this an ATL min dependency?
	CHECK_POINTER_VALID(pbCRT);
	return GetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, pbCRT);
}

STDMETHODIMP CStyleSheet::put_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL bCRT)
{
	return SetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, bCRT);
}

STDMETHODIMP CStyleSheet::get_CharacterSet(charSet* pset)
{	// what type of character set are we using?
	CHECK_POINTER_VALID(pset);
	return GetIntProperty(VCCFGID_CharacterSet, (long *)pset);
}

STDMETHODIMP CStyleSheet::put_CharacterSet(charSet set)
{
	return SetIntProperty(VCCFGID_CharacterSet, set);
}

STDMETHODIMP CStyleSheet::get_ManagedExtensions(VARIANT_BOOL* bManaged)
{	// is this a managed extensions project?
	return GetBoolProperty(VCCFGID_ManagedExtensions, bManaged);
}

STDMETHODIMP CStyleSheet::put_ManagedExtensions(VARIANT_BOOL bManaged)
{
	return SetBoolProperty(VCCFGID_ManagedExtensions, bManaged);
}

STDMETHODIMP CStyleSheet::get_DeleteExtensionsOnClean(BSTR* pbstrExt)
{	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	HRESULT hr = GetStrProperty(VCCFGID_DeleteExtensionsOnClean, pbstrExt);
	if (hr == S_FALSE)
		GetDefaultValue(VCCFGID_DeleteExtensionsOnClean, pbstrExt, this);
	return hr;
}

STDMETHODIMP CStyleSheet::put_DeleteExtensionsOnClean(BSTR bstrExt)
{
	return SetStrProperty(VCCFGID_DeleteExtensionsOnClean, bstrExt);
}

STDMETHODIMP CStyleSheet::get_WholeProgramOptimization(VARIANT_BOOL* pbOptimized)
{  // perform whole program optimization on build; more or less requires optimization turned on to be effective
	return GetBoolProperty(VCCFGID_WholeProgramOptimization, pbOptimized);
}

STDMETHODIMP CStyleSheet::put_WholeProgramOptimization(VARIANT_BOOL bOptimize)
{
	return SetBoolProperty(VCCFGID_WholeProgramOptimization, bOptimize);
}

STDMETHODIMP CStyleSheet::AddDependent(IVCPropertyContainer* pPropContainer, BOOL bInformAboutAllProperties)
{
	RETURN_INVALID_ON_NULL(pPropContainer);

	if (m_rgDependents.Find(pPropContainer) >= 0)	// already there
		return S_FALSE;

	if (m_rgDependents.Add(pPropContainer) < 0)
		return E_OUTOFMEMORY;

	InformAboutProperties(pPropContainer, bInformAboutAllProperties);
	
	return S_OK;
}

STDMETHODIMP CStyleSheet::RemoveDependent(IVCPropertyContainer* pPropContainer, BOOL bInformAboutAllProperties)
{
	RETURN_INVALID_ON_NULL(pPropContainer);

	if (m_rgDependents.Remove(pPropContainer))
		InformAboutProperties(pPropContainer, bInformAboutAllProperties);

	CComQIPtr<IVCProjectEngineImpl> spProjectEngineImpl = g_pProjectEngine;
	if (spProjectEngineImpl != NULL)
		spProjectEngineImpl->UnloadStyleSheet(this);	// this will cause the style sheet to be removed when it is no longer needed

	return S_OK;
}

STDMETHODIMP CStyleSheet::InformAboutProperties(IVCPropertyContainer* pPropContainer, BOOL bAllProperties)
{
	if (g_bInProjClose)	// REALLY don't care to do an inform if the project is gone...
		return S_OK;

	HRESULT hr = S_OK;
	long lcItems = m_rgParents.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCPropertyContainer> pParent;
		pParent = m_rgParents.GetAt(index);
		CComQIPtr<IVCStyleSheetImpl> spStyleSheetParent = pParent;
		if (spStyleSheetParent)
		{
			hr = spStyleSheetParent->InformAboutProperties(pPropContainer, bAllProperties);
			RETURN_ON_FAIL(hr);
		}
	}

	if (bAllProperties)
	{
		VCPOSITION pos = m_PropMap.GetStartPosition();
		while (pos != (VCPOSITION)NULL)
		{
			long idProp;
			CComVariant var;
			m_PropMap.GetNextAssoc(pos, idProp, var);
			CComVariant var2;
			VARIANT_BOOL bMulti;
			if (pPropContainer->GetLocalProp(idProp, &var2) != S_OK || 
				(pPropContainer->IsMultiProp(idProp, &bMulti) == S_OK && bMulti == VARIANT_TRUE))
				pPropContainer->DirtyProp(idProp);
		}
	}
	else	// just specific ones
	{
		CComVariant varHere;
		CComVariant varChild;
		if (GetProp(VCCLID_RuntimeLibrary, &varHere) == S_OK && 
			pPropContainer->GetProp(VCCLID_RuntimeLibrary, &varChild) != S_OK)
			pPropContainer->DirtyProp(VCCLID_RuntimeLibrary);
	}

	return S_OK;
}

// WARNING: Clone must only be called on a brand new style sheet!!
STDMETHODIMP CStyleSheet::Clone( IVCPropertyContainer *pSource )
{
	// check arg
	CHECK_READ_POINTER_NULL(pSource);

	CStyleSheet* pIn = (CStyleSheet*)pSource;

	// get the start of the map
	VCPOSITION iter = pIn->m_PropMap.GetStartPosition();
	// iterate over each item in the map
	while( iter )
	{
		long key;
		CComVariant value;

		pIn->m_PropMap.GetNextAssoc( iter, key, value );

		// copy this element
		m_PropMap.SetAt( key, value );
	}

	return S_OK;
}

void CStyleSheet::DoGetToolSetType(toolSetType& listStyle)
{
	ConfigurationTypes configType;
	HRESULT hr = GetIntProperty(VCCFGID_ConfigurationType, (long *)&configType);
	if (hr == S_OK)		// configuration type always overrides tool set type
	{
		listStyle = DetermineBaseType(configType);
		return;
	}

	hr = GetIntProperty(VCSTYLEID_ToolSet, (long *)&listStyle);
	VSASSERT(hr == S_OK, "Must either set the toolset or the configuration type!!!");
}

STDMETHODIMP CStyleSheet::AddTool(LPCOLESTR szName, IDispatch** ppTool)
{
	// DIANEME_TODO: need to handle add of tool AFTER project is loaded...
	HRESULT hr = DoAddTool(szName, ppTool);
	RETURN_ON_FAIL_OR_NULL(hr, *ppTool);

	// DIANEME_TODO: inform the loaded projects with this style sheet about the added tool
	return hr;
}

STDMETHODIMP CStyleSheet::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
#ifdef AARDVARK
	if( __uuidof(GeneralStyleSheetSettings) == *pCLSID )
	{
		return S_OK;
	}
	else
	{
		// not a local one, try the tools.
		long lcItems = m_rgTools.GetSize();
		for (long index = 0; index < lcItems; index++)
		{
			CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
			if (spToolImpl == NULL)
				continue;
	
			HRESULT hr = spToolImpl->CreatePageObject(ppUnk, pCLSID, pPropCntr, pPage );
			if( hr == S_OK )
				return S_OK;
		}
		return E_FAIL;
	}
#endif

	return E_NOTIMPL;
}

#ifdef AARDVARK
//---------------------------------------------------------------------------
// interface: ISpecifyPropertyPages
//---------------------------------------------------------------------------
STDMETHODIMP CStyleSheet::GetPages(/* [out] */ CAUUID *pPages)
{
	CHECK_POINTER_NULL(pPages);
	
	// Get the list of pages
	long lPageCnt = 0;
	CAUUID aPageSet[1024] = {0};
	unsigned long lTools = 0;

	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
		if (spToolImpl == NULL)
			continue;

		long tool_index;
		spToolImpl->get_ToolDisplayIndex( &tool_index );

		HRESULT hr = spToolImpl->GetPages(&(aPageSet[tool_index]));

		if( SUCCEEDED(hr) && aPageSet[tool_index].cElems )
		{
			lPageCnt += aPageSet[tool_index].cElems;
			lTools++;
			if( lTools > 1024 )
				break;
		}
	}

	// number of tool pages plus number of 'extra' pages
	pPages->cElems = lPageCnt + 1;
	if( lPageCnt == 0 )
	{
		pPages->pElems = (GUID*) NULL;
		return S_OK;
	}

	// number of tool pages plus number of 'extra' pages
	pPages->pElems = (GUID*) CoTaskMemAlloc(sizeof(CLSID)*(lPageCnt+1));
	RETURN_ON_NULL2(pPages->pElems, E_OUTOFMEMORY);
		
	// add the general->configuration page
	pPages->pElems[0] = __uuidof(GeneralStyleSheetSettings);

	// Merge the page lists
	unsigned long lPages = 1;
	// for each possible tool
	for( unsigned long i = 0; i < 1024; i++ )
	{
		// for each page this tool has
		for( unsigned long j = 0; j < aPageSet[i].cElems; j++ )
		{
			pPages->pElems[lPages] = aPageSet[i].pElems[j];
			lPages++;
		}
	}

	return S_OK;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_ToolSet(toolSetType* ToolSetList)
{	// what type of tools do we use?
	HRESULT hr = GetIntProperty(VCSTYLEID_ToolSet, (long *)ToolSetList);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCSTYLEID_ToolSet, (long *)ToolSetList);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_ToolSet(toolSetType ToolSetList)
{
	return SetIntProperty(VCSTYLEID_ToolSet, ToolSetList);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_AppliedStyleSheets(BSTR* StyleNames)
{	// style sheet names applied to this configuration purposely *NOT* in UI, but still browsable
	HRESULT hr = GetStrProperty(VCCFGID_AppliedStyleSheets, StyleNames);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_AppliedStyleSheets, StyleNames);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_AppliedStyleSheets(BSTR StyleNames)
{
	return SetStrProperty(VCCFGID_AppliedStyleSheets, StyleNames);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_BuildBrowserInformation(enumBOOL* Bsc)
{	// build .bsc file
	HRESULT hr = GetEnumBoolProperty(VCCFGID_BuildBrowserInformation, Bsc);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_BuildBrowserInformation, (VARIANT_BOOL*)Bsc);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_BuildBrowserInformation(enumBOOL Bsc)
{
	return SetBoolProperty(VCCFGID_BuildBrowserInformation, Bsc);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_UseOfMFC(useOfMfc* useMfc)
{	// how are we using MFC?
	HRESULT hr = GetIntProperty(VCCFGID_UseOfMFC, (long *)useMfc);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_UseOfMFC, (long *)useMfc);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_UseOfMFC(useOfMfc useMfc)
{
	return SetIntProperty(VCCFGID_UseOfMFC, useMfc);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_UseOfATL(useOfATL* useATL)
{	// how are we using ATL?
	HRESULT hr = GetIntProperty(VCCFGID_UseOfATL, (long *)useATL);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_UseOfATL, (long *)useATL);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_UseOfATL(useOfATL useATL)
{
	return SetIntProperty(VCCFGID_UseOfATL, useATL);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_ATLMinimizesCRunTimeLibraryUsage(enumBOOL* UseCRT)
{	// does the user want to link with the static or dynamic CRT?
	HRESULT hr = GetEnumBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, UseCRT);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, (VARIANT_BOOL*)UseCRT);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_ATLMinimizesCRunTimeLibraryUsage(enumBOOL UseCRT)
{
	return SetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, UseCRT);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_CharacterSet(charSet* optSetting)
{	// character set to use: Unicode, MBCS, default
	HRESULT hr = GetIntProperty(VCCFGID_CharacterSet, (long *)optSetting);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_CharacterSet, (long *)optSetting);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_CharacterSet(charSet optSetting)
{
	return SetIntProperty(VCCFGID_CharacterSet, optSetting);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_ManagedExtensions(enumBOOL* Managed)
{	// is this a managed C++ project?
	HRESULT hr = GetEnumBoolProperty(VCCFGID_ManagedExtensions, Managed);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_ManagedExtensions, (VARIANT_BOOL*)Managed);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_ManagedExtensions(enumBOOL Managed)
{
	return SetBoolProperty(VCCFGID_ManagedExtensions, Managed);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_DeleteExtensionsOnClean(BSTR* ExtList)
{	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	HRESULT hr = GetStrProperty(VCCFGID_DeleteExtensionsOnClean, ExtList);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_DeleteExtensionsOnClean, ExtList);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_DeleteExtensionsOnClean(BSTR ExtList)
{
	return SetStrProperty(VCCFGID_DeleteExtensionsOnClean, ExtList);
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::get_WholeProgramOptimization(enumBOOL* Optimize)
{	// perform whole program optimization on build more or less requires optimization turned on to be effective
	HRESULT hr = GetEnumBoolProperty(VCCFGID_WholeProgramOptimization, Optimize);
	if (hr == S_FALSE)
		CStyleSheet::GetDefaultValue(VCCFGID_WholeProgramOptimization, (VARIANT_BOOL*)Optimize);
	return hr;
}

STDMETHODIMP CGeneralStyleSheetSettingsPage::put_WholeProgramOptimization(enumBOOL Optimize)
{
	return SetBoolProperty(VCCFGID_WholeProgramOptimization, Optimize);
}
#endif	// AARDVARK
