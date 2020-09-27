// VCFile.h: interface for the CVCFileNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCFILE_H__7F438FBE_AB8B_11D0_8D1E_00A0C91BC942__INCLUDED_)
#define AFX_VCFILE_H__7F438FBE_AB8B_11D0_8D1E_00A0C91BC942__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "vc.h"
#include "VCNode.h"
#include "prjnode.h"


class CVCFileNode : public CVCNode,	 
				public IDispatchImpl<File, &IID_File, &LIBID_VCPrivateProjectLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>,
				public IVsPerPropertyBrowsing,
				public IPerPropertyBrowsing,
				public IOleCommandTarget,
				public IVsExtensibleObject,
				public IVsCfgProvider2
{
public:
	CVCFileNode(void);
	virtual ~CVCFileNode();

	BOOL Open();
	HRESULT OpenWith();
	static HRESULT CreateInstance(CVCFileNode **, IDispatch* pItem);
	virtual HRESULT DoDefaultAction(BOOL fNewFile);
	virtual HRESULT DoPreview();
	virtual HRESULT GetCanonicalName(BSTR* pbstrCanonicalName);
	virtual HRESULT GetName(BSTR* pbstrName);
	virtual HRESULT OnDelete(CVCNode *pRoot, DWORD dwFileOp, BOOL bCloseIfOpen = TRUE);
	virtual HRESULT CleanUpUI( CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen = TRUE );
	virtual HRESULT OnCompileFile();
	virtual HRESULT OnRefreshWebRef();
	virtual HRESULT CleanUpBeforeDelete();
	HRESULT CleanUpObjectModel(IDispatch* pParentDisp, IDispatch* pItemDisp);
	HRESULT GetObjectsForDelete(IDispatch** ppParentDisp, IDispatch** ppItemDisp);
	bool IsBuildable( void );
	bool IsHTML( void );
	bool IsWSDL( void );

	HRESULT GetActiveVCFileConfig(IDispatch** ppDispFileCfg);
	HRESULT GetVCIncludePath(BSTR* pbstrIncPath);

BEGIN_COM_MAP(CVCFileNode)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(File)
	COM_INTERFACE_ENTRY(IVsCfgProvider2)
	COM_INTERFACE_ENTRY(IVsCfgProvider)
	COM_INTERFACE_ENTRY(IOleCommandTarget)
	COM_INTERFACE_ENTRY(IVsExtensibleObject)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCFileNode)

// IPerPropertyBrowsing
public:
	STDMETHOD(GetDisplayString)( DISPID dispID, BSTR *pBstr )
		{ return E_NOTIMPL; /* CVCFileNode::GetDisplayString */ }
	STDMETHOD(MapPropertyToPage)( DISPID dispID, CLSID *pClsid )
		{ return E_NOTIMPL; /* CVCFileNode::MapPropertyToPage */ }
	STDMETHOD(GetPredefinedStrings)( DISPID dispID, CALPOLESTR *pCaStringsOut, CADWORD *pCaCookiesOut )
		{ return E_NOTIMPL; /* CVCFileNode::GetPredefinedStrings */ }
	STDMETHOD(GetPredefinedValue)( DISPID dispID, DWORD dwCookie, VARIANT* pVarOut )
		{ return E_NOTIMPL; /* CVCFileNode::GetPredefinedValue */ }
// IVsPerPropertyBrowsing
public:
	STDMETHOD(HideProperty)( DISPID dispid, BOOL *pfHide)
		{ return E_NOTIMPL; /* CVCFileNode::HideProperty */ }
	STDMETHOD(DisplayChildProperties)( DISPID dispid, BOOL *pfDisplay)
		{ return E_NOTIMPL; /* CVCFileNode::DisplayChildProperties */ }
	STDMETHOD(HasDefaultValue)( DISPID dispid, BOOL *pfDefault)
		{ return E_NOTIMPL; /* CVCFileNode::HasDefaultValue */ }
	STDMETHOD(IsPropertyReadOnly)( DISPID dispid, BOOL *fReadOnly)
		{ return E_NOTIMPL; /* CVCFileNode::IsPropertyReadOnly */ }
	STDMETHOD(GetLocalizedPropertyInfo)( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc);
	STDMETHOD(GetClassName)(BSTR* pbstrClassName);
	STDMETHOD(CanResetPropertyValue)(DISPID , BOOL* ) {return E_NOTIMPL;}	
	STDMETHOD(ResetPropertyValue)(DISPID ) {return E_NOTIMPL;}


// IVsCfgProvider, IVsCfgProvider2
	STDMETHOD(GetCfgs)( ULONG celt, IVsCfg *rgpcfg[], ULONG *pcActual, VSCFGFLAGS *prgfFlags );
	STDMETHOD(GetCfgNames)( ULONG celt,BSTR rgbstr[], ULONG *pcActual );
	STDMETHOD(GetPlatformNames)( ULONG celt, BSTR rgbstr[], ULONG *pcActual);
	STDMETHOD(GetCfgOfName)( LPCOLESTR pszCfgName, LPCOLESTR pszPlatformName, IVsCfg **ppCfg );
	STDMETHOD(AddCfgsOfCfgName)( LPCOLESTR pszCfgName, LPCOLESTR pszCloneCfgName, BOOL fPrivate );
	STDMETHOD(DeleteCfgsOfCfgName)( LPCOLESTR pszCfgName );
	STDMETHOD(RenameCfgsOfCfgName)( LPCOLESTR pszOldName, LPCOLESTR pszNewName );
	STDMETHOD(AddCfgsOfPlatformName)( LPCOLESTR pszPlatformName, LPCOLESTR pszClonePlatformName );
	STDMETHOD(DeleteCfgsOfPlatformName)( LPCOLESTR pszPlatformName );
	STDMETHOD(GetSupportedPlatformNames)( ULONG celt, BSTR rgbstr[], ULONG *pcActual );
	STDMETHOD(GetCfgProviderProperty)( VSCFGPROPID propid, VARIANT *pvar );
	STDMETHOD(AdviseCfgProviderEvents)( IVsCfgProviderEvents *pCPE, VSCOOKIE *pdwCookie );
	STDMETHOD(UnadviseCfgProviderEvents)( VSCOOKIE dwCookie );

// File
	STDMETHOD(get_Name)( BSTR *pVal );
	STDMETHOD(get_FullPath)(BSTR *pVal);
	STDMETHOD(get_RelativePath)(BSTR *pVal);
	STDMETHOD(put_RelativePath)(BSTR newVal);
	STDMETHOD(get_DeploymentContent)(VARIANT_BOOL *pVal);
	STDMETHOD(put_DeploymentContent)(VARIANT_BOOL newVal);
#ifdef AARDVARK
	CComBSTR m_bstrDeployLoc;
	STDMETHOD(get_DeploymentLocation)(BSTR *pVal){ m_bstrDeployLoc.CopyTo( pVal ); return S_OK; }
	STDMETHOD(put_DeploymentLocation)(BSTR newVal){ m_bstrDeployLoc = newVal; return S_OK;}
#endif// AARDVARK

	// automation extender methods
	STDMETHOD(get_Extender)(BSTR bstrName, IDispatch **ppDisp);
	STDMETHOD(get_ExtenderNames)(VARIANT *pvarNames);
	STDMETHOD(get_ExtenderCATID)(BSTR *pbstrGuid);

//---------------------------------------------------------------------------
// IVsExtensibleObject
//---------------------------------------------------------------------------
public:
	STDMETHOD(GetAutomationObject)(/*[in]*/ LPCOLESTR  pszPropName, /*[out]*/ IDispatch **ppDisp);

public:
	UINT GetIconIndex(ICON_TYPE);
	HRESULT GetGuidProperty( VSHPROPID propid, GUID *pguid);
	HRESULT SetProperty(VSHPROPID propid, const VARIANT& var);
	HRESULT GetProperty(VSHPROPID propid, VARIANT *pvar);

	// Command Routing
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);

	virtual HRESULT QueryStatusAtIndex(CVCNode::enumKnownGuidList eGuid, const GUID* pguidCmdGroup, 
		OLECMD prgCmds[], OLECMDTEXT *pCmdText, ULONG nIndex);
	virtual HRESULT DisplayContextMenu(VARIANT *pvaIn);

	BSTR GetFullPath( void );
	virtual UINT GetKindOf( void ) { return Type_CVCFile; }

protected:
	void Initialize(IDispatch* pItem);
	HRESULT GetCfgOfName(BSTR pszCfgName, VCFileConfiguration** ppFileCfg);

public:
	BOOL m_fDeleted;

	virtual CVCProjectNode* GetVCProjectNode();
	virtual HRESULT GetExtObject(CComVariant& varRes);
	HRESULT GetVCFile(VCFile** ppFile) 
	{ 
		CHECK_POINTER_NULL(ppFile);
		CComQIPtr<VCFile> spFile = m_dispkeyItem;
		*ppFile = spFile.Detach();
		return S_OK;
	}

protected:
	HRESULT OpenResourceEditor(CComBSTR& bstrPath);
	HRESULT OpenBscEditor(CComBSTR& bstrPath);
	HRESULT OpenAsmxEditor(CComBSTR& bstrPath);
	HRESULT OpenNormalEditor(CComBSTR& bstrPath, VSOSEFLAGS grfOpenStandard, REFGUID rguidLogicalView, BOOL fNewFile);

private:
	// automation extender CATID
	static const LPOLESTR s_wszCATID;

#ifdef AARDVARK
public:
	HRESULT HandleOpenForm(BOOL& bHandled);
	eMagic m_eMagic;
#endif // AARDVARK
};

#endif // !defined(AFX_VCFILE_H__7F438FBE_AB8B_11D0_8D1E_00A0C91BC942__INCLUDED_)
