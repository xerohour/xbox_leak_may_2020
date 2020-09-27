// VCBuildEventTool.h: Definition of the CVCBuildEventTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

#include "ProjWriter.h"

const wchar_t* const wszBldEventDefaultExtensions = L"*.bat";	// remember to update vcpb.rgs if you change this...

// default value handlers
// string props
static void GetDefaultValue( long id, BSTR *pbstrVal )
{
// 	PREBLDID_CommandLine:
// 	PRELINKID_CommandLine:
// 	POSTBLDID_CommandLine:
	*pbstrVal = NULL;
}
// integer props
static void GetDefaultValue( long id, int *piVal )
{
	*piVal = 0;
}
// boolean props
static void GetDefaultValue( long id, VARIANT_BOOL *pbVal )
{
// 	PREBLDID_ExcludedFromBuild:
// 	PRELINKID_ExcludedFromBuild:
// 	POSTBLDID_ExcludedFromBuild:
	COptionHandlerBase::GetValueFalse( pbVal );
}

class CVCPreBuildEventTool;
class CVCPreLinkEventTool;
class CVCPostBuildEventTool;

class CVCPreBuildEventPage;
class CVCPreLinkEventPage;
class CVCPostBuildEventPage;

/////////////////////////////////////////////////////////////////////////////
// CVCBuildEventTool

template <class ContainClass, class PrimaryIface, const IID* piid = &__uuidof(PrimaryIface), 
			const long TBucket, const long TCmdLineID, const long TDescID,
			const long TExcludeID, const long TDirtyID, const long TToolID>
class CVCBuildEventTool : 
	public IDispatchImpl<PrimaryIface, piid, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCBuildEventToolInternal,
	public CComObjectRoot,
	public CVCToolImpl
{
public:
	CVCBuildEventTool() 
	{
		m_nLowKey = TCmdLineID;
		m_nHighKey = TExcludeID;
		m_nDirtyKey = TDirtyID;
	}

BEGIN_COM_MAP(ContainClass)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(PrimaryIface)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCBuildEventToolInternal)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(ContainClass) 

// static member data:
protected:
	static CComBSTR s_bstrDescription;
	static GUID s_pPages[1];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;

// event interface
public:
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName) PURE;
	STDMETHOD(get_CommandLine)(BSTR *pVal)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		CHECK_POINTER_VALID(pVal);
		HRESULT hr = m_spPropertyContainer->GetStrProperty(TCmdLineID, pVal);
		if (hr == S_FALSE)
			GetDefaultValue( TCmdLineID, pVal ); 			
		return hr;
	}
	STDMETHOD(put_CommandLine)(BSTR newVal)
	{
		CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );

		// grab each line and strip leading/trailing whitespace from it
		CStringW strCommandLine = newVal;
		CStringW strLine, strNewCommandLine;
		int length = strCommandLine.GetLength();
		int left = 0;
		int right;
		// are there any newlines in the string?
		if( strCommandLine.Find( L'\n' ) != -1 )
		{
			while( true )
			{
				// find the next newline
				right = strCommandLine.Find( L'\n', left );
				// if we couldn't find one
				if( right == -1 )
				{
					// if we're at the end of the cmd line, we're done	
					if( left >= length )
						break;
					// otherwise this is the last line
					else
						right = length;
				}

				// grab this portion (logical line) of the cmd line
				strLine = strCommandLine.Mid( left, right - left );
				// trim the leading/trailing whitespace
				strLine.TrimLeft();
				strLine.TrimRight();
				// append it to the new command line
				strNewCommandLine += strLine;
				strNewCommandLine += L"\r\n";
				// move along to the next logical line
				left = right + 1;
			}
		}
		else
 			strNewCommandLine = strCommandLine;
		
		return m_spPropertyContainer->SetStrProperty(TCmdLineID, CComBSTR( strNewCommandLine ) );
	}
	STDMETHOD(get_Description)(BSTR *pVal) PURE;
	STDMETHOD(put_Description)(BSTR newVal)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		if (newVal == NULL || newVal[0] == L'\0')
			return m_spPropertyContainer->Clear(TDescID);
		else
			return m_spPropertyContainer->SetStrProperty(TDescID, newVal);
	}
	STDMETHOD(get_ExcludedFromBuild)(VARIANT_BOOL* pbExcludedFromBuild)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		CHECK_POINTER_VALID(pbExcludedFromBuild);
		HRESULT hr = m_spPropertyContainer->GetBoolProperty(TExcludeID, pbExcludedFromBuild);
		if (hr == S_FALSE)
			GetDefaultValue( TExcludeID, pbExcludedFromBuild );
		return hr;
	}
	STDMETHOD(put_ExcludedFromBuild)(VARIANT_BOOL bExcludedFromBuild)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		return m_spPropertyContainer->SetBoolProperty(TExcludeID, bExcludedFromBuild);
	}
	STDMETHOD(get_ToolPath)(BSTR* pbstrToolPath)
	{
		CHECK_POINTER_VALID(pbstrToolPath);
		CComBSTR bstrToolPath(szBuildEventToolPath);
		*pbstrToolPath = bstrToolPath.Detach();
		return S_OK;
	}
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// IVCBuildEventToolInternal
public:
	STDMETHOD(get_CommandLineInternal)(BSTR *pVal)
	{
		return get_CommandLine(pVal);
	}
	STDMETHOD(put_CommandLineInternal)(BSTR newVal)
	{
		return put_CommandLine(newVal);
	}
	STDMETHOD(get_DescriptionInternal)(BSTR *pVal)
	{
		return get_Description(pVal);
	}
	STDMETHOD(put_DescriptionInternal)(BSTR newVal)
	{
		return put_Description(newVal);
	}
	STDMETHOD(get_ExcludedFromBuildInternal)(VARIANT_BOOL* pbExcludedFromBuild)
	{
		return get_ExcludedFromBuild(pbExcludedFromBuild);
	}
	STDMETHOD(put_ExcludedFromBuildInternal)(VARIANT_BOOL bExcludedFromBuild)
	{
		return put_ExcludedFromBuild(bExcludedFromBuild);
	}

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal)
	{
		return DoGetDefaultExtensions(s_bstrExtensions, wszBldEventDefaultExtensions, pVal);
	}
	STDMETHOD(put_DefaultExtensions)(BSTR newVal)
	{
		s_bstrExtensions = newVal;
		return S_OK;
	}
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName) PURE;
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject) PURE;
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches) PURE;
// 	{
//		return DoMatchName(bstrName, szPreLinkEventToolType, szPreLinkEventToolShortName, pbMatches);
// 	}
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		CComBSTR bstrDesc;
		HRESULT hr = get_Description(&bstrDesc);
		RETURN_ON_FAIL(hr);
		return m_spPropertyContainer->Evaluate(bstrDesc, pbstrBuildDescription);
	}
	STDMETHOD(get_IsComspecTool)(VARIANT_BOOL* pbIsComspecTool)
	{
		CHECK_POINTER_NULL(pbIsComspecTool);
		*pbIsComspecTool = VARIANT_TRUE;
		return S_OK;
	}
	STDMETHOD(get_Bucket)(long* pnBucket)
	{
		CHECK_POINTER_NULL(pnBucket);
		*pnBucket = TBucket;
		return S_OK;
	}
	STDMETHOD(IsTargetTool)(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool) // tool always operates on target, not on file
	{
		return COptionHandlerBase::GetValueTrue(pbTargetTool);
	}
	STDMETHOD(get_IsFileTool)(VARIANT_BOOL* pbIsFileTool)
	{
		return COptionHandlerBase::GetValueFalse(pbIsFileTool);
	}
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrOptions)
	{ 
		return E_NOTIMPL;	// don't have any; but don't want assert
	}
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath)
	{ 
		return get_ToolPath(pbstrToolPath);
	}
	STDMETHOD(GetCommandLineEx)(IVCBuildAction* pAction, IVCBuildableItem* pBuildableItem, IVCBuildEngine* pBuildEngine, 
		IVCBuildErrorContext* pEC, BSTR *pVal)
	{
		CHECK_POINTER_NULL(pVal);
		*pVal = NULL;
		CHECK_READ_POINTER_NULL(pBuildableItem);

		CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
		CHECK_ZOMBIE(spPropContainer, IDS_ERR_TOOL_ZOMBIE);

		CComBSTR bstrCmdLineEnd;
		HRESULT hr = spPropContainer->GetEvaluatedStrProperty(TCmdLineID, &bstrCmdLineEnd);
		RETURN_ON_FAIL(hr);
			
		CComBSTR bstrCmdLine;
		BuildCommandLineBatchFile(pAction, bstrCmdLineEnd, bstrCmdLine);

		if (hr != S_OK)
		{
			bstrCmdLine.CopyTo(pVal);
			return S_FALSE;
		}

		CHECK_READ_POINTER_NULL(pBuildEngine);
		if (pBuildEngine == NULL)
			return bstrCmdLine.CopyTo(pVal);

		return pBuildEngine->FormBatchFile(bstrCmdLine, pEC, pVal);
	}
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
	{
		VSASSERT(pPropCnt, "Cannot save build event properties without property container.");
		CHECK_READ_POINTER_NULL(pPropCnt);
		CHECK_READ_POINTER_NULL(xml);

		StartNodeHeader( xml, L"Tool", false );

		// Name
		CComBSTR bstrToolShortName;
		get_ToolShortName(&bstrToolShortName);
		if (bstrToolShortName.Length() > 0)
		{
			NodeAttribute( xml, L"Name", bstrToolShortName ); 					
		}
		// Description
		AddSaveLine( xml, pPropCnt, L"Description", TDescID );
		// CommandLine
		AddSaveLine( xml, pPropCnt, L"CommandLine", TCmdLineID );
		// ExcludedFromBuild
		AddSaveLine( xml, pPropCnt, L"ExcludedFromBuild", TExcludeID, FALSE );

		// end of node header
		EndNodeHeader( xml, false );
		// end of node
		EndNode( xml, L"Tool", false );

		return S_OK;
	}

// helpers
protected:
	virtual long GetToolID() { return TToolID; }

public:
	virtual BOOL HaveCommandLine(IVCBuildAction* pAction)
	{
		RETURN_ON_NULL2(pAction, FALSE);
		CComPtr<VCConfiguration> spProjCfg;
		if (FAILED(pAction->get_ProjectConfiguration(&spProjCfg)) || spProjCfg == NULL)
			return FALSE;
		CComQIPtr<IVCPropertyContainer> spPropContainer = spProjCfg;
		RETURN_ON_NULL2(spPropContainer, FALSE);
		CComBSTR bstrCommandLine;
		if (spPropContainer->GetStrProperty(TCmdLineID, &bstrCommandLine) != S_OK || !bstrCommandLine || bstrCommandLine.Length() == 0)
			return FALSE;
		return TRUE;
	}

	void AddSaveLine(IStream *xml, IVCPropertyContainer* pPropContainer, LPOLESTR bszPropertyName, long nPropertyID, BOOL bIsString = TRUE)
	{
		CComBSTR bstrProp;
		VARIANT_BOOL bProp;
		HRESULT hr = S_OK;
		if (bIsString)
			hr = pPropContainer->GetStrProperty(nPropertyID, &bstrProp);
		else
		{
			hr = pPropContainer->GetBoolProperty(nPropertyID, &bProp);
			if (hr == S_OK)
			{
				if (bProp == VARIANT_TRUE)
					bstrProp = L"TRUE";
				else
					bstrProp = L"FALSE";
			}
		}
		if (hr != S_OK)
			return;

		NodeAttributeWithSpecialChars( xml, bszPropertyName, bstrProp );
	}
};

/////////////////////////////////////////////////////////////////////////////
// CVCPreBuildEventTool

class CVCPreBuildEventTool : 
	public CVCBuildEventTool<CVCPreBuildEventTool, VCPreBuildEventTool, &IID_VCPreBuildEventTool,
		BUCKET_PREBUILD, PREBLDID_CommandLine, PREBLDID_Description, 
		PREBLDID_ExcludedFromBuild, PREBLDID_CmdLineOptionsDirty, IDS_PreBuildEvent>
{
public:
	CVCPreBuildEventTool() {}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

// VCPreBuildEventTool
public:
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName)
	{
		CHECK_POINTER_VALID(pbstrToolName);
		InitToolName();
		s_bstrToolName.CopyTo(pbstrToolName);
		return S_OK;
	}
	STDMETHOD(get_Description)(BSTR *pVal)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		CHECK_POINTER_VALID(pVal);
		HRESULT hr = m_spPropertyContainer->GetStrProperty(PREBLDID_Description, pVal);
		if (hr == S_FALSE || pVal == NULL || *pVal == L'\0')
		{
			if (s_bstrDescription.Length() == 0)
				s_bstrDescription.LoadString(IDS_PREBUILDEVENT_DESC);
			s_bstrDescription.CopyTo(pVal);
		}
		return hr;
	}

// IVCToolImpl
public:
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName)
	{
		CHECK_POINTER_NULL(pbstrToolName);
		*pbstrToolName = SysAllocString( szPreBuildEventToolShortName );
		return S_OK;
	}
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
	{
		return CreateInstance(pPropContainer, ppToolObject);
	}
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
	{
		return DoMatchName(bstrName, szPreBuildEventToolType, szPreBuildEventToolShortName, pbMatches);
	}
	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_PREBLD;
		return S_OK;
	}

	STDMETHOD(CreatePageObject)(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
	{
		if( CLSID_VCPreBldGeneral == *pCLSID )
			CPageObjectImpl< CVCPreBuildEventPage, VCPREBLDEVENTTOOL_MIN_DISPID, VCPREBLDEVENTTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
		else
			return S_FALSE;
			
		return S_OK;
	}

	virtual long GetPageCount()	{ return 1; }
	virtual GUID* GetPageIDs()
	{
		if (s_bPagesInit == FALSE)
		{
			s_pPages[0] = __uuidof(VCPreBldGeneral);
			s_bPagesInit = TRUE;
		}
		return s_pPages; 
	}

	virtual LPCOLESTR GetToolFriendlyName()
	{
		InitToolName();
		return s_bstrToolName;
	}

	void InitToolName()
	{
		if (s_bstrToolName.Length() == 0)
		{
			if (!s_bstrToolName.LoadString(IDS_PREBUILDEVENT_TOOLNAME))
				s_bstrToolName = szPreBuildEventToolType;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// CVCPreLinkEventTool

class CVCPreLinkEventTool : 
	public CVCBuildEventTool<CVCPreLinkEventTool, VCPreLinkEventTool, &IID_VCPreLinkEventTool,
		BUCKET_PRELINK, PRELINKID_CommandLine, PRELINKID_Description,
		PRELINKID_ExcludedFromBuild, PRELINKID_CmdLineOptionsDirty, IDS_PreLinkEvent>
{
public:
	CVCPreLinkEventTool() {}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

// VCPreLinkEventTool
public:
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName)
	{
		CHECK_POINTER_VALID(pbstrToolName);
		InitToolName();
		s_bstrToolName.CopyTo(pbstrToolName);
		return S_OK;
	}
	STDMETHOD(get_Description)(BSTR *pVal)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		CHECK_POINTER_VALID(pVal);
		HRESULT hr = m_spPropertyContainer->GetStrProperty(PRELINKID_Description, pVal);
		if (hr == S_FALSE || pVal == NULL || *pVal == L'\0')
		{
			if (s_bstrDescription.Length() == 0)
				s_bstrDescription.LoadString(IDS_PRELINKEVENT_DESC);
			s_bstrDescription.CopyTo(pVal);
		}
		return hr;
	}

// IVCToolImpl
public:
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName)
	{
		CHECK_POINTER_NULL(pbstrToolName);
		*pbstrToolName = SysAllocString( szPreLinkEventToolShortName );
		return S_OK;
	}
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
	{
		return CreateInstance(pPropContainer, ppToolObject);
	}
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
	{
		return DoMatchName(bstrName, szPreLinkEventToolType, szPreLinkEventToolShortName, pbMatches);
	}
	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_PRELINK;
		return S_OK;
	}
	STDMETHOD(CreatePageObject)(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
	{
		if( CLSID_VCPreLinkGeneral == *pCLSID )
			CPageObjectImpl< CVCPreLinkEventPage, VCPRELINKEVENTTOOL_MIN_DISPID, VCPRELINKEVENTTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
		else
			return S_FALSE;
			
		return S_OK;
	}
	virtual long GetPageCount()	{ return 1; }
	virtual GUID* GetPageIDs()
	{
		if (s_bPagesInit == FALSE)
		{
			s_pPages[0] = __uuidof(VCPreLinkGeneral);
			s_bPagesInit = TRUE;
		}
		return s_pPages; 
	}
	virtual LPCOLESTR GetToolFriendlyName()
	{
		InitToolName();
		return s_bstrToolName;
	}

	void InitToolName()
	{
		if (s_bstrToolName.Length() == 0)
		{
			if (!s_bstrToolName.LoadString(IDS_PRELINKEVENT_TOOLNAME))
				s_bstrToolName = szPreLinkEventToolType;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// CVCPostBuildEventTool

class CVCPostBuildEventTool : 
	public CVCBuildEventTool<CVCPostBuildEventTool, VCPostBuildEventTool, &IID_VCPostBuildEventTool,
		BUCKET_POSTBUILDEVT, POSTBLDID_CommandLine, POSTBLDID_Description,
		POSTBLDID_ExcludedFromBuild, POSTBLDID_CmdLineOptionsDirty, IDS_PostBuildEvent>
{
public:
	CVCPostBuildEventTool() {}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

// VCPostBuildEventTool
public:
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName)
	{
		CHECK_POINTER_VALID(pbstrToolName);
		s_bstrToolName.CopyTo(pbstrToolName);
		return S_OK;
	}
	STDMETHOD(get_Description)(BSTR *pVal)
	{
		CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
		CHECK_POINTER_VALID(pVal);
		HRESULT hr = m_spPropertyContainer->GetStrProperty(POSTBLDID_Description, pVal);
		if (hr == S_FALSE || pVal == NULL || *pVal == L'\0')
		{
			if (s_bstrDescription.Length() == 0)
				s_bstrDescription.LoadString(IDS_POSTBUILDEVENT_DESC);
			s_bstrDescription.CopyTo(pVal);
		}
		return hr;
	}

// IVCToolImpl
public:
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName)
	{
		CHECK_POINTER_NULL(pbstrToolName);
		*pbstrToolName = SysAllocString( szPostBuildEventToolShortName );
		return S_OK;
	}
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
	{
		return CreateInstance(pPropContainer, ppToolObject);
	}
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
	{
		return DoMatchName(bstrName, szPostBuildEventToolType, szPostBuildEventToolShortName, pbMatches);
	}
	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_POSTBLD;
		return S_OK;
	}
	STDMETHOD(CreatePageObject)(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
	{
		if( CLSID_VCPostBldGeneral == *pCLSID )
			CPageObjectImpl< CVCPostBuildEventPage, VCPOSTBLDEVENTTOOL_MIN_DISPID, VCPOSTBLDEVENTTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
		else
			return S_FALSE;
			
		return S_OK;
	}
	virtual long GetPageCount()	{ return 1; }
	virtual GUID* GetPageIDs()
	{
		if (s_bPagesInit == FALSE)
		{
			s_pPages[0] = __uuidof(VCPostBldGeneral);
			s_bPagesInit = TRUE;
		}
		return s_pPages; 
	}
	virtual LPCOLESTR GetToolFriendlyName()
	{
		InitToolName();
		return s_bstrToolName;
	}

	void InitToolName()
	{
		if (s_bstrToolName.Length() == 0)
		{
			if (!s_bstrToolName.LoadString(IDS_POSTBUILDEVENT_TOOLNAME))
				s_bstrToolName = szPostBuildEventToolType;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// CVCPreBuildEventPage

class ATL_NO_VTABLE CVCPreBuildEventPage :
	public IDispatchImpl<IVCPreBuildEventPage, &IID_IVCPreBuildEventPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCPreBuildEventPage,VCPREBLDEVENTTOOL_MIN_DISPID,VCPREBLDEVENTTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCPreBuildEventPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCPreBuildEventPage)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCPreBuildEventPage, &IID_IVCPreBuildEventPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCPreBuildEventPage
public:
	STDMETHOD(get_CommandLine)(BSTR *pVal);
	STDMETHOD(put_CommandLine)(BSTR newVal);
	STDMETHOD(get_Description)(BSTR *pVal);
	STDMETHOD(put_Description)(BSTR newVal);
	STDMETHOD(get_ExcludedFromBuild)(enumBOOL* pbExcludedFromBuild);
	STDMETHOD(put_ExcludedFromBuild)(enumBOOL bExcludedFromBuild);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseCommandsDialog(long id) { return (id == PREBLDID_CommandLine); }
};

/////////////////////////////////////////////////////////////////////////////
// CVCPreLinkEventPage

class ATL_NO_VTABLE CVCPreLinkEventPage :
	public IDispatchImpl<IVCPreLinkEventPage, &IID_IVCPreLinkEventPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCPreLinkEventPage,VCPRELINKEVENTTOOL_MIN_DISPID,VCPRELINKEVENTTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCPreLinkEventPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCPreLinkEventPage)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCPreLinkEventPage, &IID_IVCPreLinkEventPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCPreLinkEventPage
public:
	STDMETHOD(get_CommandLine)(BSTR *pVal);
	STDMETHOD(put_CommandLine)(BSTR newVal);
	STDMETHOD(get_Description)(BSTR *pVal);
	STDMETHOD(put_Description)(BSTR newVal);
	STDMETHOD(get_ExcludedFromBuild)(enumBOOL* pbExcludedFromBuild);
	STDMETHOD(put_ExcludedFromBuild)(enumBOOL bExcludedFromBuild);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseCommandsDialog(long id) { return (id == PRELINKID_CommandLine); }
};

/////////////////////////////////////////////////////////////////////////////
// CVCPostBuildEventPage

class ATL_NO_VTABLE CVCPostBuildEventPage :
	public IDispatchImpl<IVCPostBuildEventPage, &IID_IVCPostBuildEventPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CVCPostBuildEventPage,VCPOSTBLDEVENTTOOL_MIN_DISPID,VCPOSTBLDEVENTTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(CVCPostBuildEventPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCPostBuildEventPage)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IVCPostBuildEventPage, &IID_IVCPostBuildEventPage, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// IVCPostBuildEventPage
public:
	STDMETHOD(get_CommandLine)(BSTR *pVal);
	STDMETHOD(put_CommandLine)(BSTR newVal);
	STDMETHOD(get_Description)(BSTR *pVal);
	STDMETHOD(put_Description)(BSTR newVal);
	STDMETHOD(get_ExcludedFromBuild)(enumBOOL* pbExcludedFromBuild);
	STDMETHOD(put_ExcludedFromBuild)(enumBOOL bExcludedFromBuild);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseCommandsDialog(long id) { return (id == POSTBLDID_CommandLine); }
};

