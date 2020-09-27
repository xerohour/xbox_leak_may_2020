// DebugSettings.h: Definition of the CDebugSettings class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGSETTINGS_H__A54AAE89_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
#define AFX_DEBUGSETTINGS_H__A54AAE89_30C2_11D3_87BF_A04A4CC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vcprojectengine.h"		// g_pProjectEngine;
#include "vccoll.h"
#include "settingspage.h"
#include "PropContainer.h"
#include "toolhrt.h"

/////////////////////////////////////////////////////////////////////////////
// CDebugSettings

class CDebugSettingsBase :
	public CPropertyContainerImpl
{
public:
	CDebugSettingsBase() {}

protected:
	// helper for deep base class CProjectEvaluator
	virtual BOOL GetMacroValue(UINT idMacro, CStringW& strMacroValue, IVCPropertyContainer* pPropContainer);
	BOOL GetExeProjectTargetPath(CPathW& rProjDirPath, CStringW& rstrProjDirPath);
	virtual BOOL CallBaseGetMacroValue(IVCPropertyContainer* pPropContainer) { return FALSE; }
};

class CDebugSettings : 
	public CDebugSettingsBase,
	public IDispatchImpl<VCDebugSettings, &IID_VCDebugSettings, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public IVCDebugSettingsImpl,
	public CComObjectRoot,
	public CComCoClass<CDebugSettings,&CLSID__VCDebugSettings>
{
public:
	CDebugSettings();
	static HRESULT CreateInstance(VCDebugSettings** ppDebugSettings, VCConfiguration* pConfig);

	static HRESULT GetRegSvrCommand( CStringW &strCommand );
	static HRESULT GetTestContainerCommand( CStringW& strCommand );
	static HRESULT GetBrowserCommand( CStringW* pstrCommandLine, CStringW* pstrArgs );

protected:
	HRESULT Initialize(VCConfiguration* pConfig);
	static BOOL GetTestContainerName(CStringW& strTestContainer);
	static void InitializeTestContainer();

public:
BEGIN_COM_MAP(CDebugSettings)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCDebugSettings)
	COM_INTERFACE_ENTRY(IVCDebugSettingsImpl)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CDebugSettings) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()

// VCDebugSettings
// NOTE: when you add properties to this interface, do *not* add member variables for them.
// This is a property container class and, as such, the storage for all variables is in
// m_PropMap (which is in CPropertyContainerImpl).  Failure to store your variables there 
// means that you won't get the behavior you expect in the property page associated with these 
// properties.  (Do what you want with the Impl form of the interface, though, as there is never 
// property container storage for variables needed there.  If, however, you *do* decide to store
// Impl variables in the property container, be sure to add IDs in prjids.h so that nobody will
// collide with your choice of IDs.)
public:
	STDMETHOD(get_Command)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Command)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_WorkingDirectory)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_WorkingDirectory)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_CommandArguments)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_CommandArguments)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Attach)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_Attach)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_Remote)(/*[out, retval]*/ RemoteDebuggerType *pVal);
	STDMETHOD(put_Remote)(/*[in]*/ RemoteDebuggerType newVal);
	STDMETHOD(get_RemoteMachine)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_RemoteMachine)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_DebuggerType)(/*[out, retval]*/ TypeOfDebugger *pVal);
	STDMETHOD(put_DebuggerType)(/*[in]*/ TypeOfDebugger newVal);
	STDMETHOD(get_RemoteCommand)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_RemoteCommand)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_HttpUrl)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_HttpUrl)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_PDBPath)(/*out, retval*/ BSTR* pbstrVal);
	STDMETHOD(put_PDBPath)(/* in */ BSTR bstrNewVal);
	STDMETHOD(get_SQLDebugging)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_SQLDebugging)(/*[in]*/ VARIANT_BOOL newVal);

// IVCDebugSettingsImpl
public:
	STDMETHOD(WriteToStream)(/*[in]*/ IStream *pStream);
	STDMETHOD(ReadFromStream)(/*[in]*/ IStream *pStream);
	STDMETHOD(Close)();
	STDMETHOD(CanGetDebugTargetInfo)(/*[out]*/ VARIANT_BOOL* pbCanGet);
	STDMETHOD(GetDebugTargetInfo)(/*[in]*/ VSDBGLAUNCHFLAGS grfLaunch, 
			/*[in,out]*/ VsDebugTargetInfo *pDebugTargetInfo,
			/*[in,out]*/ DWORD *pdwCount);

// IVCPropertyContainer; most methods in base class
public:
	STDMETHOD(GetMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *varValue);
	STDMETHOD(GetStagedPropertyContainer)(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppPropContainer);

// IVCPropertyContainer helpers
protected:
	virtual HRESULT DoGetProp(long id, BOOL bCheckSpecialProps, VARIANT *pVarValue);
	virtual HRESULT DoGetStrProperty(long idProp, BOOL bCheckSpecial, BSTR* pbstrValue);
	HRESULT DoGetCommand(CComBSTR& bstrCommand, BOOL bEvaluate);
	virtual BOOL CheckCanDirty() { return FALSE; }
	virtual BOOL CallBaseGetMacroValue(IVCPropertyContainer* pPropContainer) { return(pPropContainer != this && m_spProjCfg); }

// default ISpecifyPropertyPages method in base class
       
private:
	HRESULT GetHttpDebugTargetInfo(VSDBGLAUNCHFLAGS grfLaunch, 
			/*[in,out]*/ VsDebugTargetInfo *pDebugTargetInfo,
			/*[in,out]*/ DWORD *pdwCount);

    void GetEngineForDebuggerType(TypeOfDebugger type, const IMAGE_INFO*, CLSID* pClsid);
	void DoEvaluate(IVCPropertyContainer* pPropContainer, CComBSTR& bstrProp);
	void DoGetEvaluatedStrProperty(IVCPropertyContainer* pPropContainer, long idProp, BSTR* pbstrProp);

	BOOL m_bWorkingDirInit;
	CComPtr<VCConfiguration> m_spProjCfg;

	static BOOL s_bTestContainerInit;
	static CStringW s_strTestContainer;
};

// NOTE: please update the rgDebugCommands[] array in ..\ui_dll\src\exefordbg.h if you update this list
const WORD rgDebugCommands[] = {
	VCDSID_Command_IExplore,
	VCDSID_Command_ATLTest,
	VCDSID_Command_RegSvr,
	VCDSID_Command_Browse,
};	

/////////////////////////////////////////////////////////////////////////////
// CDebugSettingsPage
class ATL_NO_VTABLE CDebugSettingsPage :
	public IDispatchImpl<VCDebugSettings, &IID_VCDebugSettings, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
	public CPageObjectImpl<CDebugSettingsPage, VCDEBUG_MIN_DISPID, VCDEBUG_MAX_DISPID>,
	public ICategorizeProperties,
	public CComObjectRoot
{
public:
BEGIN_COM_MAP(CDebugSettingsPage)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(VCDebugSettings)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
	COM_INTERFACE_ENTRY_IID(IID_ICategorizeProperties, ICategorizeProperties)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<VCDebugSettings, &IID_VCDebugSettings, &LIBID_VCProjectEngineLibrary, PublicProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}


// VCDebugSettings
public:
	STDMETHOD(get_Command)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Command)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_WorkingDirectory)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_WorkingDirectory)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_CommandArguments)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_CommandArguments)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Attach)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_Attach)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_Remote)(/*[out, retval]*/ RemoteDebuggerType *pVal);
	STDMETHOD(put_Remote)(/*[in]*/ RemoteDebuggerType newVal);
	STDMETHOD(get_RemoteMachine)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_RemoteMachine)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_DebuggerType)(/*[out, retval]*/ TypeOfDebugger *pVal);
	STDMETHOD(put_DebuggerType)(/*[in]*/ TypeOfDebugger newVal);
	STDMETHOD(get_RemoteCommand)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_RemoteCommand)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_HttpUrl)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_HttpUrl)(/*[in]*/ BSTR newVal);
    STDMETHOD(get_PDBPath)(/*out, retval*/ BSTR* pbstrVal);
    STDMETHOD(put_PDBPath)(/* in */ BSTR bstrNewVal);
	STDMETHOD(get_SQLDebugging)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_SQLDebugging)(/*[in]*/ VARIANT_BOOL newVal);

// IPerPropertyBrowsing (overides)
public:
	STDMETHOD(GetPredefinedValue)( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut );
	STDMETHOD(GetPredefinedStrings)( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut );

// IVSPerPropertyBrowsing (overides)
	STDMETHOD(HideProperty)( DISPID dispid, BOOL *pfHide);

// ICategorizeProperties
public:
	STDMETHOD(MapPropertyToCategory)( DISPID dispid, PROPCAT* ppropcat);
	STDMETHOD(GetCategoryName)( PROPCAT propcat, LCID lcid, BSTR* pbstrName);

// IPropertyContainer
public:
	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault)
	{
		CHECK_POINTER_NULL(pfDefault);
		*pfDefault = TRUE;
		return S_OK;
	}

// helper functions
public:
	virtual void ResetParentIfNeeded();
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL IsDir(long id)
	{
		if( id == VCDSID_WorkingDirectory )
			return TRUE;
		return FALSE;
	}

	static HRESULT BrowseForCommand( CStringW* pstrCommand, HWND hwndOwner = NULL );
	HRESULT SGetCommand(BSTR* pVal);
protected:
	virtual BOOL UseSinglePropDirectoryPickerDialog(long id) { return (id == VCDSID_PDBPath); }
};

class CDbgSettingsPage :
	public CSettingsPageBase<CDbgSettingsPage, &CLSID_DebugSettingsPage, IDS_DebugSettings, 0>
{
protected:
	virtual void DoCreatePageObject(UINT i, IDispatch* pDisp, IVCPropertyContainer* pLiveContainer, IVCSettingsPage* pPage)
	{
		CPageObjectImpl< CDebugSettingsPage, VCDEBUG_MIN_DISPID, VCDEBUG_MAX_DISPID >::CreateInstance(&m_ppUnkArray[i], pLiveContainer, pPage);
	}
};


class CVCStagedExeConfigPropertyContainer : 
	public IVCStagedPropertyContainer, 
	public CDebugSettingsBase,
	public CComObjectRoot
{
public:
	CVCStagedExeConfigPropertyContainer() : m_pos(NULL), m_cStaged(0) {}
	~CVCStagedExeConfigPropertyContainer() {}

	static HRESULT CreateInstance(IVCPropertyContainer* pParentPropContainer, IVCStagedPropertyContainer **ppStagedPropertyContainer);
	HRESULT Initialize(IVCPropertyContainer* pParentPropContainer);

BEGIN_COM_MAP(CVCStagedExeConfigPropertyContainer)
	COM_INTERFACE_ENTRY(IVCStagedPropertyContainer)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
END_COM_MAP()
DECLARE_NO_REGISTRY()

// IVCPropertyContainer overrides
public:
	STDMETHOD(GetMultiProp)(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *varValue);
	STDMETHOD(GetMultiPropSeparator)(long id, BSTR* pbstrPreferred, BSTR* pbstrAll);
	STDMETHOD(Commit)();

// CPropertyContainerImpl overrides
	HRESULT DoGetParentProp(long id, VARIANT_BOOL bAllowInherit, BOOL bCheckSpecialProps, VARIANT *pVarValue);

// IVCStagedPropertyContainer
public:
	STDMETHOD(get_Parent)(IVCPropertyContainer** ppParent);
	STDMETHOD(ClearPropertyRange)(long nMinID, long nMaxID);
	STDMETHOD(GetGrandParentProp)(long id, VARIANT* pvarValue);		// skip to parent of our non-staged parent
	STDMETHOD(Close)();
	STDMETHOD(CommitPropertyRange)(long nMinID, long nMaxID);
	STDMETHOD(Reset)(void);
	STDMETHOD(NextElement)(long* pKey, VARIANT* pVal);
	STDMETHOD(HoldStagedContainer)();			// number of calls to Hold/Release StagedContainer need to match
	STDMETHOD(ReleaseStagedContainer)();		// number of calls to Hold/Release StagedContainer need to match
	STDMETHOD(HasDefaultValue)(DISPID dispid, BOOL *pfDefault);

// helpers
protected:
	HRESULT DoCommitPropertyRange(long nMinID, long nMaxID, bool bIgnoreRange);
	virtual BOOL CheckCanDirty() { return FALSE; }

protected:
	CComPtr<IVCPropertyContainer> m_spParentPropContainer;

	VCPOSITION m_pos;
	int m_cStaged;
};

#endif // !defined(AFX_DEBUGSETTINGS_H__A54AAE89_30C2_11D3_87BF_A04A4CC10000__INCLUDED_)
