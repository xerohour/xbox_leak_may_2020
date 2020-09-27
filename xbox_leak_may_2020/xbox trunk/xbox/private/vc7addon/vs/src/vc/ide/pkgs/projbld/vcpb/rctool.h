// VCResourceCompilerTool.h: Definition of the CVCResourceCompilerTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "RCToolBase.h"

enum ResTokens 
{
	TOKEN_EOF,
	TOKEN_NEWLINE,
	TOKEN_HASHSIGN,
	TOKEN_WORD,
	TOKEN_INCLUDE,
	TOKEN_BITMAP,
	TOKEN_CURSOR,
	TOKEN_ICON,
	TOKEN_TYPELIB,
	TOKEN_LOADONCALL,
	TOKEN_PRELOAD,
	TOKEN_FIXED,
	TOKEN_MOVEABLE,
	TOKEN_DISCARDABLE,
	TOKEN_FONT,
	TOKEN_RCINCLUDE,
	TOKEN_COMMENT,
	TOKEN_PURE,
	TOKEN_IMPURE,
	TOKEN_STOP_SCAN,
	TOKEN_START_SCAN,
	TOKEN_ACCELERATORS,
	TOKEN_DIALOG,
	TOKEN_DLGINIT,
	TOKEN_MENU,
	TOKEN_RCDATA,
	TOKEN_STRINGTABLE,
	TOKEN_BEGIN,
	TOKEN_END,
	TOKEN_VERSIONINFO,
	TOKEN_ERROR, 
	TOKEN_UNDEF, 
	TOKEN_DEFINE,
	TOKEN_LANGUAGE,
	TOKEN_GUIDELINES,
	TOKEN_TOOLBAR,
	TOKEN_DIALOGEX,
	TOKEN_MENUEX
};

// classes in this header
class CVCResourceCompilerTool;
class CResourceCompilerToolOptionHandler;

class CResourceCompilerToolOptionHandler : public COptionHandlerBase
{
public:
	DECLARE_OPTION_TABLE();

public:
	// default value handlers
	// string props
	virtual void GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// integer props
	virtual void GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt = NULL );
	// boolean props
	virtual void GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt = NULL );

protected:
	virtual BOOL SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption);
	virtual BOOL SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, CComVariant& rvar);
};

/////////////////////////////////////////////////////////////////////////////
// CVCResourceCompilerTool

class ATL_NO_VTABLE CVCResourceCompilerTool : 
	public IDispatchImpl<VCResourceCompilerTool, &IID_VCResourceCompilerTool, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot,
	public CVCToolImpl
{
public:
	CVCResourceCompilerTool() 
	{
		m_nLowKey = VCRCTOOL_MIN_DISPID;
		m_nHighKey = VCRCTOOL_MAX_DISPID;
		m_nDirtyKey = VCRCID_CmdLineOptionsDirty;
		m_nDirtyOutputsKey = VCRCID_OutputsDirty;
	}
	static HRESULT CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool);

BEGIN_COM_MAP(CVCResourceCompilerTool)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCToolImpl)
	COM_INTERFACE_ENTRY(VCResourceCompilerTool)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CVCResourceCompilerTool) 

// VCResourceCompilerTool -- switches are not case sensitive
public:
	STDMETHOD(get_ToolName)(BSTR* pbstrToolName);	// friendly name of tool, e.g., "Resource Compiler Tool"
	STDMETHOD(get_AdditionalOptions)(BSTR* pbstrAdditionalOptions);	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	STDMETHOD(put_AdditionalOptions)(BSTR bstrAdditionalOptions);
	STDMETHOD(get_PreprocessorDefinitions)(BSTR* pbstrDefines);	// (/d) define symbol can have multiple
	STDMETHOD(put_PreprocessorDefinitions)(BSTR bstrDefines);
	STDMETHOD(get_Culture)(enumResourceLangID* pnLangID);	// (/l [num]) resource language ID
	STDMETHOD(put_Culture)(enumResourceLangID nLangID);
	STDMETHOD(get_AdditionalIncludeDirectories)(BSTR* pbstrIncludePath);	// (/I![path]) directory to add to include path, may have multiple
	STDMETHOD(put_AdditionalIncludeDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_IgnoreStandardIncludePath)(VARIANT_BOOL* pbIgnoreInclPath);	// (/X) ignore standard include path
	STDMETHOD(put_IgnoreStandardIncludePath)(VARIANT_BOOL bIgnoreInclPath);
	STDMETHOD(get_ShowProgress)(VARIANT_BOOL* pbShowProgress);	// (/v) verbose (print progress messages)
	STDMETHOD(put_ShowProgress)(VARIANT_BOOL bShowProgress);
	STDMETHOD(get_ResourceOutputFileName)(BSTR* pbstrResFile);	// (/fo[file]) name .RES file default: <rcfile>.res
	STDMETHOD(put_ResourceOutputFileName)(BSTR bstrResFile);
	// Automation properties
	STDMETHOD(get_ToolPath)(BSTR *pVal);
	STDMETHOD(get_PropertyOption)(BSTR bstrProp, long dispidProp, BSTR *pVal);
	STDMETHOD(get_FullIncludePath)(BSTR* fullIncludePath);	// include path, including all inherited values, plus platform includes
	STDMETHOD(get_VCProjectEngine)(IDispatch** projEngine)	{ return DoGetVCProjectEngine(projEngine); }

// IVCToolImpl
public:
	STDMETHOD(get_DefaultExtensions)(BSTR* pVal);
	STDMETHOD(put_DefaultExtensions)(BSTR newVal);
	STDMETHOD(GetAdditionalOptionsInternal)(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrAdditionalOptions);
	STDMETHOD(GetBuildDescription)(IVCBuildAction* pAction, BSTR* pbstrBuildDescription);
	STDMETHOD(get_ToolPathInternal)(BSTR* pbstrToolPath);
	STDMETHOD(get_Bucket)(long *pVal);
	STDMETHOD(GenerateOutput)(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC);
	STDMETHOD(AffectsOutput)(long nPropID, VARIANT_BOOL* pbAffectsOutput);
	STDMETHOD(CreateToolObject)(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject);
	STDMETHOD(GetAdditionalIncludeDirectoriesInternal)(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs);
	STDMETHOD(get_IncludePathID)(long* pnIncludeID);
	STDMETHOD(get_ToolShortName)(BSTR* pbstrToolName);
	STDMETHOD(get_ToolNameInternal)(BSTR* pbstrToolName) { return get_ToolName(pbstrToolName); }
	STDMETHOD(MatchName)(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches);
	STDMETHOD(CanScanForDependencies)(VARIANT_BOOL* pbIsScannable);
	STDMETHOD(GenerateDependencies)(IVCBuildAction* pAction, void* frhFile, IVCBuildErrorContext* pErrorContext);
	STDMETHOD(CreatePageObject)(IUnknown **, CLSID*, IVCPropertyContainer *, IVCSettingsPage * );

	STDMETHOD(get_ToolDisplayIndex)(long* pIndex)
	{
		CHECK_POINTER_NULL(pIndex);
		*pIndex = TOOL_DISPLAY_INDEX_RC;
		return S_OK;
	}

	virtual long GetPageCount()	{ return 2; }
	virtual GUID* GetPageIDs();

// helpers
public:
	virtual COptionHandlerBase* GetOptionHandler() { return &s_optHandler; }

public:
	static CResourceCompilerToolOptionHandler s_optHandler;

protected:
	static GUID s_pPages[2];
	static BOOL s_bPagesInit;
	static CComBSTR s_bstrBuildDescription;
	static CComBSTR s_bstrToolName;
	static CComBSTR s_bstrExtensions;

protected:
	virtual LPCOLESTR GetToolFriendlyName();
	void InitToolName();
	virtual long GetToolID() { return IDS_RC; }

private:
	// Dependency Scanner Helpers
	static ResTokens GetNextToken(const char* pMax, const char*& pCurrent, const char*& pTokenStart);
	void CopyAndSuppressBackslashes(char* pdestbuf, const char*psrcbuf, int nchar);
};
