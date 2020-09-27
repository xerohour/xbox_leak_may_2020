// BscTool.cpp : Implementation of the BscMake Tool

#include "stdafx.h"
#include "BscTool.h"
#include "ProjWriter.h"

// constants
const wchar_t* const wszBscDefaultExtensions = L"*.sbr";	// remember to update vcpb.rgs if you change this...

// static initializers
CBscMakeToolOptionHandler CVCBscMakeTool::s_optHandler;
GUID CVCBscMakeTool::s_pPages[2];
BOOL CVCBscMakeTool::s_bPagesInit;
CComBSTR CVCBscMakeTool::s_bstrBuildDescription = L"";
CComBSTR CVCBscMakeTool::s_bstrToolName = L"";
CComBSTR CVCBscMakeTool::s_bstrExtensions = L"";


////////////////////////////////////////////////////////////////////////////////
// Option Table for BscMake switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// NOTE 2: if you add anything here, make sure to add it to ReadFromStream and WriteToStream as well.  With only a few 
// properties, it wasn't worth writing something to traverse the table for that.  Also make sure that all of these props
// go into the big case statement in IsRealProp as NOT being real props.  Otherwise, the project file gets dirtied as part
// of project load if any of the BSC settings are set and it shouldn't be.
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CBscMakeToolOptionHandler, L"VCBscMakeTool", IDS_BSC, TRUE /* pre & post */, TRUE /* case sensitive */)
	// general
	OPT_BOOL(SuppressStartupBanner,		L"|nologo",		L"SuppressStartupBanner",			IDS_BSCGeneral, VCBSCID)
	OPT_BSTR(OutputFile,				L"o %s",		L"OutputFile",			single,		IDS_BSCGeneral, VCBSCID)
END_OPTION_TABLE()

BOOL CBscMakeToolOptionHandler::SetEvenIfDefault(VARIANT *pvarDefault, long idOption)
{
	CComVariant varDefault;
	BOOL bRet = FALSE;

	switch (idOption)
	{
	case VCBSCID_SuppressStartupBanner:
		varDefault = VARIANT_TRUE;
		varDefault.vt = VT_BOOL;
		bRet = TRUE;
		break;
	}
	varDefault.Detach(pvarDefault);
	return bRet;
}

void CBscMakeToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt )
{
	GetDefaultString( pVal );
}

void CBscMakeToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
	*pVal = 0;
}

void CBscMakeToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
	switch( id )
	{
	case VCBSCID_SuppressStartupBanner:
		GetValueTrue( pVal );
		break;
	default:
		GetValueFalse( pVal );
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// implementation of CVCBscMakeTool

HRESULT CVCBscMakeTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppBscTool)
{
	CHECK_POINTER_NULL(ppBscTool);
	*ppBscTool = NULL;

	CComObject<CVCBscMakeTool> *pObj;
	HRESULT hr = CComObject<CVCBscMakeTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCBscMakeTool *pVar = pObj;
		pVar->AddRef();
		*ppBscTool = pVar;
	}
	return hr;
}

STDMETHODIMP CVCBscMakeTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_VCBSCGeneral == *pCLSID )
	{
		CPageObjectImpl<CVCBscMakePage,VCBSCMAKETOOL_MIN_DISPID,VCBSCMAKETOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}


GUID* CVCBscMakeTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(VCBSCGeneral);
		s_pPages[1] = __uuidof(BSCAdditionalOptions);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

LPCOLESTR CVCBscMakeTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCBscMakeTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_BSCMAKE_TOOLNAME))
			s_bstrToolName = szBscMakeToolType;
	}
}

STDMETHODIMP CVCBscMakeTool::get_IsFileTool(VARIANT_BOOL* pbIsFileTool)
{
	return COptionHandlerBase::GetValueFalse(pbIsFileTool);
}

STDMETHODIMP CVCBscMakeTool::PrePerformBuildActions(bldActionTypes type, IVCBuildActionList* pActions, 
	bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn)
{
	CHECK_POINTER_NULL(pActReturn);
	*pActReturn = ACT_Complete;

	// DIANEME_TODO: old build engine sent an IDE-level inform of start of BSCMAKE build here
	
	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::PostPerformBuildActions(bldActionTypes type, IVCBuildActionList* pActions, 
	bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn)
{
	// DIANEME_TODO: old build engine sent an IDE-level inform of end of BSCMAKE build here

	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::IsDeleteOnRebuildFile(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild)
{
	return COptionHandlerBase::GetValueTrue(pbDelOnRebuild);
}

HRESULT CVCBscMakeTool::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	if (spPropContainer == NULL)
		return E_UNEXPECTED;

	CComBSTR bstrName;
	HRESULT hr = spPropContainer->GetEvaluatedStrProperty(VCBSCID_OutputFile, &bstrName);
	if (hr != S_OK)
		return hr;

	CStringW strName = bstrName;
	if (strName.IsEmpty())
		return E_FAIL;

	CComBSTR bstrProjName;
	hr = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjName);	// should never have macros in it
	RETURN_ON_FAIL(hr);
	CStringW strDirName = bstrProjName;
	CDirW dirProj;
	if (!dirProj.CreateFromKnown(strDirName))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	if( !rpath.CreateFromDirAndFilename(dirProj, strName))
		return E_FAIL;
	rpath.GetActualCase(TRUE);

	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = VCBSCID_OutputFile;

	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::GenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC)
{
	if (plstActions == NULL)
		return S_OK;	// nothing to do

	plstActions->Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = plstActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);

		CComPtr<IVCBuildableItem> spItem;
		spAction->get_Item(&spItem);

		CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
		if (spPropContainer == NULL)
			continue;

		VARIANT_BOOL bRunTool = VARIANT_FALSE;
		if (spPropContainer->GetBoolProperty(VCBSCID_RunBSCMakeTool, &bRunTool) != S_OK || bRunTool != VARIANT_TRUE)
			continue;

		CPathW pathOutput;
		if (GetPrimaryOutputFromTool(spItem, VARIANT_TRUE, pathOutput) != S_OK)
			continue;

		HRESULT hrT = spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathOutput, pEC, VARIANT_TRUE, VARIANT_TRUE,
			VCBSCID_OutputFile, this);
		RETURN_ON_FAIL(hrT);
	}

	return S_OK; // success
}

STDMETHODIMP CVCBscMakeTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	switch (nPropID)
	{
	case VCBSCID_OutputFile:
	case VCBSCID_OutputsDirty:
	case VCBSCID_RunBSCMakeTool:
		*pbAffectsOutput = VARIANT_TRUE;
	}

	return S_OK;
}

// general; @response deliberately left out
STDMETHODIMP CVCBscMakeTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "BSCMake Tool"
	CHECK_POINTER_VALID(pbstrToolName);
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other switches
	return ToolGetStrProperty(VCBSCID_AdditionalOptions, pbstrAdditionalOptions);
}

STDMETHODIMP CVCBscMakeTool::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCBSCID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CVCBscMakeTool::get_SuppressStartupBanner(VARIANT_BOOL* pbNoLogo)
{	// (/nologo) enable suppression of copyright message
	return ToolGetBoolProperty(VCBSCID_SuppressStartupBanner, pbNoLogo);
}

STDMETHODIMP CVCBscMakeTool::put_SuppressStartupBanner(VARIANT_BOOL bNoLogo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoLogo );
	return m_spPropertyContainer->SetBoolProperty(VCBSCID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCBscMakeTool::get_OutputFile(BSTR* pbstrOutputFile)
{	// (/o [file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(VCBSCID_OutputFile, pbstrOutputFile);
}

STDMETHODIMP CVCBscMakeTool::put_OutputFile(BSTR bstrOutputFile)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCBSCID_OutputFile, bstrOutputFile);
}

STDMETHODIMP CVCBscMakeTool::get_RunBSCMakeTool(VARIANT_BOOL* pbRun)
{	// run the BSCMake tool; acts as inverse of ExcludeFromBuild
	return ToolGetBoolProperty(VCBSCID_RunBSCMakeTool, pbRun);
}

STDMETHODIMP CVCBscMakeTool::put_RunBSCMakeTool(VARIANT_BOOL bRun)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bRun );
	return m_spPropertyContainer->SetBoolProperty(VCBSCID_RunBSCMakeTool, bRun);
}

// Automation properties
STDMETHODIMP CVCBscMakeTool::get_ToolPath(BSTR *pbstrToolPath)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pbstrToolPath );
	HRESULT hr = m_spPropertyContainer->GetStrProperty(VCBSCID_ToolPath, pbstrToolPath);
	if (hr == S_FALSE)
	{
		CComBSTR bstrPath( szBscMakeToolPath );
		*pbstrToolPath = bstrPath.Detach();
	}
	return hr;
}

// IVCToolImpl
STDMETHODIMP CVCBscMakeTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszBscDefaultExtensions, pVal); 
}

STDMETHODIMP CVCBscMakeTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CVCBscMakeTool::get_ToolShortName(BSTR* pbstrToolName)
{
	CComBSTR bstrShortName( szBscMakeToolShortName );
	*pbstrToolName = bstrShortName.Detach();
	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{
	return CreateInstance(pPropContainer, ppToolObject);
}

STDMETHODIMP CVCBscMakeTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{ 
	return GetProcessedAdditionalOptions(pItem, VCBSCID_AdditionalOptions, bForBuild, bSkipLocal, pbstrAdditionalOptions);
}

STDMETHODIMP CVCBscMakeTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szBscMakeToolType, szBscMakeToolShortName, pbMatches);
}

STDMETHODIMP CVCBscMakeTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_BSCMAKING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CVCBscMakeTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_BSC;
	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbIsTargetTool)
{
	return COptionHandlerBase::GetValueTrue(pbIsTargetTool);
}

STDMETHODIMP CVCBscMakeTool::get_PerformIfAble(VARIANT_BOOL* pbPerformIfAble)
{
	return COptionHandlerBase::GetValueTrue(pbPerformIfAble);
}

STDMETHODIMP CVCBscMakeTool::ReadFromStream(IStream* pStream, IVCPropertyContainer *pPropContainer)
{
	CHECK_READ_POINTER_NULL(pStream);
	CHECK_READ_POINTER_NULL(pPropContainer);
	
	VARTYPE		type;
	ULONG		iByteCount;
	DISPID		dispid;
	HRESULT		hr;

	while (true)
	{
		//Read in type
		hr = pStream->Read(&type, sizeof(VARTYPE), &iByteCount);
		RETURN_ON_FAIL(hr);

		//End of list marker
		if (type == VT_EMPTY) break;

		//Read in dispid
		hr = pStream->Read(&dispid, sizeof(DISPID), &iByteCount);
		RETURN_ON_FAIL(hr);

		switch (type)	//Switch the property types
		{
			case VT_BOOL:
			case VT_I2:
				switch (dispid)
				{
					case VCBSCID_RunBSCMakeTool:
						{
							VARIANT_BOOL bRun = VARIANT_FALSE;
							hr = pStream->Read(&bRun, sizeof(VARIANT_BOOL), &iByteCount);
							RETURN_ON_FAIL(hr);
							hr = pPropContainer->SetBoolProperty(VCBSCID_RunBSCMakeTool, bRun);
						}
						break;
					case VCBSCID_SuppressStartupBanner:
						{
							VARIANT_BOOL bNoLogo = VARIANT_FALSE;
							hr = pStream->Read(&bNoLogo, sizeof(VARIANT_BOOL), &iByteCount);
							RETURN_ON_FAIL(hr);
							hr = pPropContainer->SetBoolProperty(VCBSCID_SuppressStartupBanner, bNoLogo);
						}
						break;

					default:
						return E_UNEXPECTED;
				}
				RETURN_ON_FAIL(hr);
				break;
	
			case VT_BSTR:
				if (dispid == VCBSCID_OutputFile)
				{
					CComBSTR bstrFile;
					hr = bstrFile.ReadFromStream(pStream);
					RETURN_ON_FAIL(hr);
					hr = pPropContainer->SetStrProperty(VCBSCID_OutputFile, bstrFile);
				}
				else
					hr = E_UNEXPECTED;
				RETURN_ON_FAIL(hr);
				break;

			default:
				return E_UNEXPECTED;
		}
	}
	return S_OK;
}

STDMETHODIMP CVCBscMakeTool::WriteToStream(IStream *pStream, IVCPropertyContainer *pPropContainer)
{
	CHECK_READ_POINTER_NULL(pStream);
	CHECK_READ_POINTER_NULL(pPropContainer);

	CComBSTR bstrName = L"VCBscMakeTool";
	HRESULT hr = bstrName.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	// RunBSCMakeTool
	CComVariant varRun;
	hr = pPropContainer->GetLocalProp(VCBSCID_RunBSCMakeTool, &varRun);
	if (hr == S_OK && (varRun.vt == VT_BOOL || varRun.vt == VT_I2))
	{
		VARIANT_BOOL bRun = varRun.boolVal;
		hr = CPropertyContainerImpl::WritePropertyToStream(pStream, bRun, VCBSCID_RunBSCMakeTool);
		RETURN_ON_FAIL(hr);
	}

	// SuppressStartupBanner
	CComVariant varNoLogo;
	hr = pPropContainer->GetLocalProp(VCBSCID_SuppressStartupBanner, &varNoLogo);
	if (hr == S_OK && (varNoLogo.vt == VT_BOOL || varNoLogo.vt == VT_I2))
	{
		VARIANT_BOOL bNoLogo = varNoLogo.boolVal;
		hr = CPropertyContainerImpl::WritePropertyToStream(pStream, bNoLogo, VCBSCID_SuppressStartupBanner);
		RETURN_ON_FAIL(hr);
	}

	// OuputFile
	CComVariant varOutputFile;
	hr = pPropContainer->GetLocalProp(VCBSCID_OutputFile, &varOutputFile);
	if (hr == S_OK && varOutputFile.vt == VT_BSTR)
	{
		CComBSTR bstrOutputFile= varOutputFile.bstrVal;
		hr = CPropertyContainerImpl::WritePropertyToStream(pStream, bstrOutputFile, VCBSCID_OutputFile);
		RETURN_ON_FAIL(hr);
	}

	//Write "end of property list" marker
	VARTYPE type = VT_EMPTY;
	ULONG	iByteCount;
	hr = pStream->Write(&type, sizeof(VARTYPE), &iByteCount);
	RETURN_ON_FAIL(hr);

	return S_OK;
}


/**********************************************************/

STDMETHODIMP CVCBscMakePage::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(VCBSCID_AdditionalOptions, &(CVCBscMakeTool::s_optHandler), pbstrAdditionalOptions);
}

STDMETHODIMP CVCBscMakePage::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	return SetStrProperty(VCBSCID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CVCBscMakePage::get_SuppressStartupBanner(enumSuppressStartupBannerBOOL* pbNoLogo)
{	// (/nologo) enable suppression of copyright message
	return GetEnumBoolProperty2(VCBSCID_SuppressStartupBanner, (long *)pbNoLogo);
}

STDMETHODIMP CVCBscMakePage::put_SuppressStartupBanner(enumSuppressStartupBannerBOOL bNoLogo)
{
	return SetBoolProperty(VCBSCID_SuppressStartupBanner, bNoLogo);
}

STDMETHODIMP CVCBscMakePage::get_OutputFile(BSTR* pbstrOutputFile)
{	// (/o [file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(VCBSCID_OutputFile, &(CVCBscMakeTool::s_optHandler), pbstrOutputFile);
}

STDMETHODIMP CVCBscMakePage::put_OutputFile(BSTR bstrOutputFile)
{
	return SetStrProperty(VCBSCID_OutputFile, bstrOutputFile);
}

void CVCBscMakePage::GetBaseDefault(long id, CComVariant& varValue)
{
	VARIANT_BOOL bVal;
	switch (id)
	{
	case VCBSCID_SuppressStartupBanner:
		CVCBscMakeTool::s_optHandler.GetDefaultValue( id, &bVal );
		varValue = bVal;
		break;
	default:
		CBase::GetBaseDefault(id, varValue);
		return;
	}
}

