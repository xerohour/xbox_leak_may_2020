// AutoProject.h
// declares what we implement for the DTE Project based interface of
// AutoProject

#pragma once

#include "PrjNode.h"
#include "AutoBase.h"

// Constants

// string version of the guid
_declspec(selectany) LPOLESTR szguidCATIDAutoProj = L"{610d461e-d0d5-11d2-8599-006097c68e81}";

class CAutoProject : 
	public CAutoBase<CAutoProject, VCProject, szguidCATIDAutoProj>,
	public IDispatchImpl<_Project, &__uuidof(_Project), &LIBID_DTE, 7, 0>, 
	public ISupportErrorInfo,
	public ISupportVSProperties,
	public CComObjectRoot
{
public:
	CAutoProject();
	~CAutoProject();
	void Initialize(IDispatch* pAutoProjects, IDispatch* pProject);
	static HRESULT CreateInstance(CAutoProject **ppAutoProject, IDispatch* pAutoProjects, IDispatch* pAutoProject, 
		IDispatch* pProject);
	VCProject* GetProject();
	CVCProjectNode* GetProjectNode();

BEGIN_COM_MAP(CAutoProject)
	COM_INTERFACE_ENTRY(ISupportVSProperties)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(_Project)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CAutoProject) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// Project
public:
    /* _Project methods */
    STDMETHOD(get_Name)(BSTR FAR* lpbstrName);
    STDMETHOD(put_Name)(BSTR bstrName);
    STDMETHOD(Reserved1)(BSTR ProgID, VARIANT FileName) { return E_NOTIMPL; }
    STDMETHOD(get_FileName)(BSTR FAR* lpbstrReturn);
    STDMETHOD(get_IsDirty)(VARIANT_BOOL FAR* lpfReturn);
    STDMETHOD(put_IsDirty)(VARIANT_BOOL Dirty);
    STDMETHOD(Reserved2)(BSTR Section, BSTR Key, BSTR Value) { return E_NOTIMPL; }
    STDMETHOD(Reserved3)(BSTR Section, BSTR Key, BSTR FAR* lpbstrValue) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved4)(BSTR FAR* lpbstrHelpFile) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved4)(BSTR bstrHelpFile) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved5)(long FAR* lpdwContextID) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved5)(long dwContextID) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved6)(BSTR FAR* lpbstrDescription) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved6)(BSTR bstrDescription) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved7)(IDispatch * FAR* lppReferences) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved8)(IDispatch * FAR* lppaReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Collection)(Projects FAR* FAR* lppaReturn);
    STDMETHOD(get_Reserved9)(long FAR* lpStartMode) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved9)(long StartMode) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved10)(BSTR FAR* lpbstrCompatibleOleServer) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved10)(BSTR bstrCompatibleOleServer) { return E_NOTIMPL; }
    STDMETHOD(SaveAs)(BSTR FileName);
    STDMETHOD(Reserved11)() { return E_NOTIMPL; }
    STDMETHOD(get_Reserved12)(long FAR* lpkind) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved12)(long Type) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved13)(long FAR* lpIconState) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved13)(long IconState) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved17)(VARIANT_BOOL FAR* lpfReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved15)(IDispatch * FAR* lppcReturn) { return E_NOTIMPL; }
    STDMETHOD(get_Reserved16)(BSTR FAR* lpbstrBldFName) { return E_NOTIMPL; }
    STDMETHOD(put_Reserved16)(BSTR bstrBldFName) { return E_NOTIMPL; }
    STDMETHOD(get_DTE)(DTE FAR* FAR* lppaReturn);
    STDMETHOD(get_Kind)(BSTR FAR* lpbstrFileName);
    STDMETHOD(get_ProjectItems)(ProjectItems FAR* FAR* lppcReturn);
    STDMETHOD(get_Properties)(Properties FAR* FAR* ppObject);
    STDMETHOD(get_UniqueName)(BSTR FAR* lpbstrFileName);
    STDMETHOD(get_Object)(IDispatch **ppProjectModel);
    STDMETHOD(get_Extender)(BSTR bstrExtenderName, IDispatch **ppExtender);
    STDMETHOD(get_ExtenderNames)(VARIANT *pvarExtenderNames);
    STDMETHOD(get_ExtenderCATID)(BSTR *pbstrRetval);

    STDMETHOD(get_FullName)(BSTR *lpbstrReturn);
    STDMETHOD(get_Saved)(VARIANT_BOOL *lpfReturn);
    STDMETHOD(put_Saved)(VARIANT_BOOL Dirty);
    STDMETHOD(get_SourceControl)(SourceControl ** /*ppSourceControl*/);
    STDMETHOD(get_ConfigurationManager)(ConfigurationManager **ppConfigurationManager);
    STDMETHOD(get_Globals)(Globals ** /*ppGlobals*/);
    STDMETHOD(Save)(BSTR /*FileName*/);
    STDMETHOD(get_ParentProjectItem)(ProjectItem ** ppProjectItem);
    STDMETHOD(get_CodeModel)(CodeModel ** ppCodeModel);
    STDMETHOD(Delete)();

protected:
	CComQIPtr<VCProject> m_spProject;
	CComPtr<IDispatch> m_spAutoProjects;
	CComPtr<Globals> m_spGlobals;

// ISupportVSProperties
public:
	STDMETHOD(NotifyPropertiesDelete)()
	{
		return E_NOTIMPL;	// CAutoProject::NotifyPropertiesDelete
	}
};

class ATL_NO_VTABLE CAutoProjects :
	public CAutoBase<CAutoProjects, IVCCollection, szguidCATIDAutoProj>,
	public CComObjectRoot,
	public ISupportVSProperties,
	public IDispatchImpl<Projects, &IID__Projects, &LIBID_DTE, 7, 0>
{
public:
	CAutoProjects();
	~CAutoProjects();
	static HRESULT CreateInstance(CAutoProjects** ppAutoProjects, IDispatch* pProjects);

BEGIN_COM_MAP(CAutoProjects)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(Projects)
END_COM_MAP()

protected:
	void Initialize(IDispatch* pProjects);

/* _Projects methods */
public:
	STDMETHOD(Item)(VARIANT index, Project FAR* FAR* lppcReturn);
	STDMETHOD(get_Reserved1)(IDispatch * FAR* lppaReturn) { return E_NOTIMPL; }
	STDMETHOD(get_Parent)(DTE FAR* FAR* lppaReturn);
	STDMETHOD(get_Count)(long FAR* lplReturn);
	STDMETHOD(_NewEnum)(IUnknown * FAR* lppiuReturn);
	STDMETHOD(Reserved2)(long Type, VARIANT_BOOL Exclusive, IDispatch * FAR* lppcReturn) { return E_NOTIMPL; }
	STDMETHOD(Reserved3)(BSTR PathName, VARIANT_BOOL Exclusive, IDispatch * FAR* lppcReturn) { return E_NOTIMPL; }
	STDMETHOD(Reserved4)(BSTR PathName, VARIANT_BOOL Exclusive, IDispatch * FAR* lppcReturn) { return E_NOTIMPL; }
	STDMETHOD(Reserved5)(IDispatch * lpc) { return E_NOTIMPL; }
	STDMETHOD(get_Reserved6)(IDispatch * FAR* lppptReturn) { return E_NOTIMPL; }
	STDMETHOD(put_Reserved6)(IDispatch * lpptNewActiveProj) { return E_NOTIMPL; }
	STDMETHOD(Reserved7)(BSTR FAR* lpbstrReturn) { return E_NOTIMPL; }
	STDMETHOD(get_Reserved8)(long FAR* lpIconState) { return E_NOTIMPL; }
	STDMETHOD(put_Reserved8)(long IconState) { return E_NOTIMPL; }
	STDMETHOD(Reserved9)(BSTR PathName) { return E_NOTIMPL; }
	STDMETHOD(get_DTE)(DTE FAR* FAR* lppaReturn);
	STDMETHOD(get_Properties)(Properties * FAR* lppaReturn);
	STDMETHOD(get_Kind)(BSTR FAR* lpbstrReturn);

protected:
	CComQIPtr<IVCCollection> m_spProjects;

// helpers
public:
	static HRESULT GetItem(IDispatch** ppProjItem, IDispatch* pOwner, IDispatch* pAutoParent, IDispatch* pAutoProject, IDispatch* pVCItem);

// ISupportVSProperties
public:
	STDMETHOD(NotifyPropertiesDelete)()
	{
		return E_NOTIMPL;	// CAutoProject::NotifyPropertiesDelete
	}
};

class ATL_NO_VTABLE CAutoGlobals : 
	public CComObjectRoot,
	public IDispatchImpl<Globals, &IID_Globals, &LIBID_DTE, 7, 0>
{
public:
	CAutoGlobals();
	~CAutoGlobals();
	static HRESULT CreateInstance( CAutoGlobals **ppAutoGlobals, VCProject *pProj );

BEGIN_COM_MAP( CAutoGlobals )
	COM_INTERFACE_ENTRY( IDispatch )
	COM_INTERFACE_ENTRY( Globals )
END_COM_MAP()

protected:
	void Initialize( VCProject *pProj );

// Globals methods
public:
	STDMETHOD(get_DTE)( DTE **ppDTE );
	STDMETHOD(get_Parent)( IDispatch **ppDTE );
	STDMETHOD(get_VariableValue)( BSTR name, VARIANT *pVal );
	STDMETHOD(put_VariableValue)( BSTR name, VARIANT val );
	STDMETHOD(get_VariablePersists)( BSTR name, VARIANT_BOOL *pbVal );
	STDMETHOD(put_VariablePersists)( BSTR name, VARIANT_BOOL bVal );
	STDMETHOD(get_VariableExists)( BSTR name, VARIANT_BOOL *pbVal );
	STDMETHOD(get_VariableNames)( VARIANT *pNames );

private:
	CComPtr<IVCGlobals> m_spGlobals;
};
			
