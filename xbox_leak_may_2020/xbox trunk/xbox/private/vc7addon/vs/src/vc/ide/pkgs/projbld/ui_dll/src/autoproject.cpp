// dteproj.cpp
// implementation for CAutoProject

#include "stdafx.h"

#include "AutoProject.h"    
#include "AutoFile.h"    
#include "bldpkg.h"
#include "vcarchy.h"
#include "VCCodeModels.h"
#include "VCDesignerObjectInternal.h"
#include "VCCodeModelsInternal.h"
#include "ResPkg.h"
#include "ResGuid.h"

// for CComSafeArray
#include "atlsafe.h"

CAutoProject::CAutoProject()
{
}

CAutoProject::~CAutoProject()
{
}

void CAutoProject::Initialize(IDispatch* pAutoProjects, IDispatch* pProject)
{
	m_spProject = pProject;
	m_spAutoProjects = pAutoProjects;
	VSASSERT(m_spProject != NULL && m_spAutoProjects != NULL, "Unable to initialize CAutoProject!");
	HRESULT hr = CAutoGlobals::CreateInstance( (CAutoGlobals**)(&m_spGlobals), m_spProject );
	VSASSERT( SUCCEEDED( hr ) && m_spGlobals, "Unable to initialize CAutoProject: Can't create Globals object!");
}


//-----------------------------------------------------------------------------
// create an instance of a project node with reference count 1
//-----------------------------------------------------------------------------
HRESULT CAutoProject::CreateInstance(CAutoProject **ppAutoProject, IDispatch* pAutoProjects, 
	IDispatch* /*pAutoProject*/, IDispatch* pProject)
{
	CComObject<CAutoProject> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CAutoProject>::CreateInstance(&pPrjObj);
	RETURN_ON_FAIL_OR_NULL2(hr, pPrjObj, E_OUTOFMEMORY);
	pPrjObj->AddRef();
	pPrjObj->Initialize(pAutoProjects, pProject);
	*ppAutoProject = pPrjObj;
	return hr;
}

VCProject* CAutoProject::GetProject()
{
	return (m_spProject);
}

CVCProjectNode* CAutoProject::GetProjectNode()
{
	CComQIPtr<IVCExternalCookie> spExtCookie = m_spProject;
	RETURN_ON_NULL2(spExtCookie, NULL);

	void* pCookie;
	spExtCookie->get_ExternalCookie(&pCookie);
	RETURN_ON_NULL2(pCookie, NULL);

	return (CVCProjectNode*)pCookie;
}

//---------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
STDMETHODIMP CAutoProject::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID__Project,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CAutoProject::get_Name(BSTR FAR* lpbstrName)
{
	CHECK_ZOMBIE(m_spProject, IDS_ERR_PROJ_ZOMBIE);
	return m_spProject->get_Name(lpbstrName);
}

STDMETHODIMP CAutoProject::put_Name(BSTR bstrName)
{
	CHECK_ZOMBIE(m_spProject, IDS_ERR_PROJ_ZOMBIE);
	return m_spProject->put_Name(bstrName);
}


STDMETHODIMP CAutoProject::get_FileName(BSTR FAR* lpbstrReturn)
{
	CHECK_ZOMBIE(m_spProject, IDS_ERR_PROJ_ZOMBIE);
	return m_spProject->get_ProjectFile(lpbstrReturn);
}

STDMETHODIMP CAutoProject::get_IsDirty(VARIANT_BOOL FAR* lpfReturn)
{
	CHECK_ZOMBIE(m_spProject, IDS_ERR_PROJ_ZOMBIE);
	return m_spProject->get_IsDirty(lpfReturn);
}

STDMETHODIMP CAutoProject::put_IsDirty(VARIANT_BOOL Dirty)
{
	CComQIPtr<IVCProjectImpl> spProjImpl = m_spProject;
	CHECK_ZOMBIE(spProjImpl, IDS_ERR_PROJ_ZOMBIE);

	return spProjImpl->put_IsDirty(Dirty);
}

STDMETHODIMP CAutoProject::get_Collection(Projects FAR* FAR* lppaReturn)
{
	CHECK_POINTER_VALID(lppaReturn);

	CComQIPtr<Projects> spProjects = m_spAutoProjects;
	return spProjects.CopyTo(lppaReturn);
}

STDMETHODIMP CAutoProject::SaveAs(BSTR FileName)
{
	return E_NOTIMPL;	// CAutoProject::SaveAs
}

STDMETHODIMP CAutoProject::get_Globals(Globals ** ppGlobals)
{
	CHECK_POINTER_VALID( ppGlobals );

	CComPtr<Globals> spGlobals = m_spGlobals;
	return spGlobals.CopyTo( ppGlobals );
}
STDMETHODIMP CAutoProject::get_ParentProjectItem(ProjectItem ** ppProjectItem)
{
	CHECK_POINTER_VALID( ppProjectItem );

	*ppProjectItem = NULL;

	VSITEMID itemid = VSITEMID_NIL;
	CVCProjectNode* pProjectNode = GetProjectNode();
	CVCArchy *pArchy = NULL;
	RETURN_ON_NULL2(pProjectNode, NOERROR);
	pArchy = pProjectNode->GetArchy();
	RETURN_ON_NULL2(pArchy, NOERROR);
	itemid = pArchy->GetParentHierarchyItemID();
	CComQIPtr<IVsHierarchy> pHier = pArchy->GetParentHierarchy();
	RETURN_ON_NULL2(pHier, NOERROR);
	CComVariant varExtParent;
	pHier->GetProperty( itemid, VSHPROPID_ExtObject, &varExtParent );
	if( varExtParent.vt != VT_UNKNOWN || !varExtParent.punkVal )
		return NOERROR;
 	CComQIPtr<ProjectItem> pProjectItem = varExtParent.punkVal;
	RETURN_ON_NULL2(pProjectItem, NOERROR);
	pProjectItem.CopyTo( ppProjectItem );

	return NOERROR;
}

STDMETHODIMP CAutoProject::get_DTE(DTE FAR* FAR* lppaReturn)
{
	return GetBuildPkg()->GetDTE(lppaReturn);
}

STDMETHODIMP CAutoProject::get_Kind(BSTR FAR* lpbstrFileName)
{
	CHECK_POINTER_VALID(lpbstrFileName);

	CComBSTR bstr = L"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
	*lpbstrFileName = bstr.Detach();
	return S_OK;
}

STDMETHODIMP CAutoProject::get_ProjectItems(ProjectItems FAR* FAR* lppcReturn)
{
	CHECK_POINTER_VALID(lppcReturn);
	CHECK_ZOMBIE(m_spProject, IDS_ERR_PROJ_ZOMBIE);

	CComPtr<IDispatch> spDispItems;
	HRESULT hr = m_spProject->get_Items(&spDispItems);
	RETURN_ON_FAIL_OR_NULL2(hr, spDispItems, E_UNEXPECTED);

	CAutoItems *pAutoItems = NULL;
	hr = CAutoItems::CreateInstance(&pAutoItems, this, this, spDispItems);
	RETURN_ON_FAIL_OR_NULL(hr, pAutoItems);
	*lppcReturn = (ProjectItems *)pAutoItems;
	return hr;
}

STDMETHODIMP CAutoProject::get_Properties(Properties FAR* FAR* ppObject)
{
	return DoGetProperties(this, m_spProject, ppObject);
}

STDMETHODIMP CAutoProject::get_UniqueName(BSTR FAR* lpbstrFileName)
{
	CHECK_POINTER_VALID(lpbstrFileName);

	CComPtr<IVsSolution> spVsSolution;

	HRESULT hr = GetBuildPkg()->GetIVsSolution(&spVsSolution);
	VSASSERT(SUCCEEDED(hr), "Unable to QueryService for IVsSolution!  (Are we in a secondary thread?)");
	RETURN_ON_FAIL_OR_NULL(hr, spVsSolution);

	CVCProjectNode* pProjectNode = GetProjectNode();
	CVCArchy *pArchy = NULL;
	if (pProjectNode != NULL)
		pArchy = pProjectNode->GetArchy();
	CHECK_ZOMBIE(pArchy, IDS_ERR_PROJ_ZOMBIE);

	CComPtr<IVsHierarchy> spHier = pArchy->GetIVsHierarchy();
	return spVsSolution->GetUniqueNameOfProject(spHier, lpbstrFileName);
}

STDMETHODIMP CAutoProject::get_Object(IDispatch **ppProjectModel)
{
	return DoGetObject(m_spProject, ppProjectModel, IDS_ERR_PROJ_ZOMBIE);
}

STDMETHODIMP CAutoProject::get_Extender(BSTR bstrExtenderName, IDispatch **ppExtender)
{
	return DoGetExtender(this, bstrExtenderName, ppExtender);
}

STDMETHODIMP CAutoProject::get_ExtenderNames(VARIANT *pvarExtenderNames)
{
	return DoGetExtenderNames(this, pvarExtenderNames);
}

STDMETHODIMP CAutoProject::get_ExtenderCATID(BSTR *pbstrRetval)
{
	return DoGetExtenderCATID(pbstrRetval);
}

STDMETHODIMP CAutoProject::get_FullName(BSTR *lpbstrReturn)
{
	return get_FileName(lpbstrReturn);
}

STDMETHODIMP CAutoProject::get_Saved(VARIANT_BOOL *lpfReturn)
{
	return get_IsDirty(lpfReturn);
}

STDMETHODIMP CAutoProject::put_Saved(VARIANT_BOOL Dirty)
{
	return put_IsDirty(Dirty);
}

STDMETHODIMP CAutoProject::get_SourceControl(SourceControl ** /*ppSourceControl*/)
{
	return E_NOTIMPL;	// CAutoProject::get_SourceControl
}

STDMETHODIMP CAutoProject::Save(BSTR /*FileName*/)
{
	return E_NOTIMPL;		// CAutoProject::Save
}

STDMETHODIMP CAutoProject::get_ConfigurationManager(ConfigurationManager **ppConfigurationManager)
{
	CHECK_POINTER_VALID(ppConfigurationManager);
	CVCProjectNode* pProjectNode = GetProjectNode();
	CVCArchy *pArchy = NULL;
	if (pProjectNode)
		pArchy = pProjectNode->GetArchy();
	CHECK_ZOMBIE(pArchy, IDS_ERR_PROJ_ZOMBIE);

	CComPtr<IVsExtensibility> pExtService;
	HRESULT hr = ExternalQueryService(SID_SVsExtensibility, IID_IVsExtensibility, (LPVOID*)&pExtService);
	RETURN_ON_FAIL_OR_NULL(hr, pExtService);

	CComPtr<IVsHierarchy> spHier = pArchy->GetIVsHierarchy();
	return pExtService->GetConfigMgr(spHier, VSITEMID_ROOT, ppConfigurationManager);
}

STDMETHODIMP CAutoProject::get_CodeModel(CodeModel ** ppCodeModel)
{
	CHECK_POINTER_VALID(ppCodeModel);

	HRESULT hr(S_OK);
	CComBSTR bstrProjName;
	get_Name(&bstrProjName);
	if (bstrProjName.Length() == 0)
		CHECK_ZOMBIE(NULL, IDS_ERR_PROJ_ZOMBIE);
	CComPtr<IDispatch> spDispProject;
	hr = QueryInterface(__uuidof(IDispatch), reinterpret_cast<void **>(&spDispProject));
	RETURN_ON_FAIL(hr);
	CComPtr<VCCodeModelService> spIVCCodeModelService;
	hr = ExternalQueryService(SID_SVCCodeModelService,
		__uuidof(VCCodeModelService), reinterpret_cast<void **>(&spIVCCodeModelService));
	RETURN_ON_FAIL_OR_NULL(hr, spIVCCodeModelService);
	CComPtr<VCCodeModel> spVCCodeModel;
	hr = spIVCCodeModelService->CreateCodeModel(&spVCCodeModel);
	RETURN_ON_FAIL_OR_NULL(hr, spVCCodeModel);
	CComPtr<VCDesignElementI> spCodeModel;
	hr = spVCCodeModel.QueryInterface(&spCodeModel);
	RETURN_ON_FAIL(hr);
	spCodeModel->put_Parent(spDispProject);

	return spVCCodeModel.QueryInterface(ppCodeModel);
}

STDMETHODIMP CAutoProject::Delete()
{
	return E_NOTIMPL;	// CAutoProject::Delete, don't allow this from UI, so no point in allowing from automation, either
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAutoProjects

CAutoProjects::CAutoProjects()
{
}

CAutoProjects::~CAutoProjects()
{
}

void CAutoProjects::Initialize(IDispatch* pProjects)
{
	VSASSERT(pProjects != NULL, "Cannot initialize CAutoProjects with NULL!");
	m_spProjects = pProjects;
}

//-----------------------------------------------------------------------------
// create an instance of a project node with reference count 1
//-----------------------------------------------------------------------------
HRESULT CAutoProjects::CreateInstance(CAutoProjects **ppAutoProjects, IDispatch* pProjects)
{
	CComObject<CAutoProjects> *pPrjsObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CAutoProjects>::CreateInstance(&pPrjsObj);
	RETURN_ON_FAIL_OR_NULL2(hr, pPrjsObj, E_OUTOFMEMORY);
	pPrjsObj->AddRef();
	pPrjsObj->Initialize(pProjects);
	*ppAutoProjects = pPrjsObj;
	return hr;
}

STDMETHODIMP CAutoProjects::Item(VARIANT index, Project** lppcReturn)
{
	CHECK_POINTER_VALID(lppcReturn);
	*lppcReturn = NULL;

	CHECK_ZOMBIE(m_spProjects, IDS_ERR_COLLECTION_ZOMBIE);

	CComPtr<IDispatch> spItem;
	HRESULT hr = m_spProjects->Item(index, &spItem);
	RETURN_ON_FAIL_OR_NULL2(hr, spItem, S_FALSE);

	CComPtr<IDispatch> spDispAutoProject;
	hr = GetItem(&spDispAutoProject, this, NULL, NULL, spItem);
	RETURN_ON_FAIL(hr);
	CComQIPtr<Project> spAutoProject = spDispAutoProject;
	*lppcReturn = spAutoProject.Detach();
	return hr;
}

/* static */
HRESULT CAutoProjects::GetItem(IDispatch** ppProjItem, IDispatch* pOwner, IDispatch* pAutoProjectParent, 
	IDispatch* pAutoProjectParent2, IDispatch* pVCItem)
{
	CAutoProject* pAutoProject = NULL;
	HRESULT hr = CAutoProject::CreateInstance(&pAutoProject, pOwner, pAutoProjectParent, pVCItem);
	RETURN_ON_FAIL(hr);
	
	*ppProjItem = (IDispatch*)pAutoProject;
	return S_OK;
}

STDMETHODIMP CAutoProjects::get_Parent(DTE** lppaReturn)
{
	CHECK_POINTER_VALID(lppaReturn);
	return ExternalQueryService(SID_SDTE, IID__DTE, (void **)lppaReturn);
}

STDMETHODIMP CAutoProjects::get_Count(long* lplReturn)
{
	CHECK_POINTER_VALID(lplReturn);
	CHECK_ZOMBIE(m_spProjects, IDS_ERR_COLLECTION_ZOMBIE);

	return m_spProjects->get_Count(lplReturn);
}

STDMETHODIMP CAutoProjects::_NewEnum(IUnknown** lppiuReturn)
{
	CHECK_POINTER_VALID(lppiuReturn);
	*lppiuReturn = NULL;
	CHECK_ZOMBIE(m_spProjects, IDS_ERR_COLLECTION_ZOMBIE);

	CAutoEnumerator<CAutoProjects>* pEnumerator = NULL;
	HRESULT hr = CAutoEnumerator<CAutoProjects>::CreateInstance(&pEnumerator, this, NULL, NULL, m_spProjects);
	RETURN_ON_FAIL_OR_NULL2(hr, pEnumerator, E_OUTOFMEMORY);

	hr = pEnumerator->QueryInterface(__uuidof(IUnknown), (LPVOID *)lppiuReturn);
	pEnumerator->Release();	// QI above gives us the refcount of 1 we want...
	return hr;
}

STDMETHODIMP CAutoProjects::get_DTE(DTE** lppaReturn)
{
	return GetBuildPkg()->GetDTE(lppaReturn);
}

STDMETHODIMP CAutoProjects::get_Properties(Properties** ppObject)
{
	return DoGetProperties(this, m_spProjects, ppObject);
}

STDMETHODIMP CAutoProjects::get_Kind(BSTR* lpbstrReturn)
{
	CHECK_POINTER_VALID(lpbstrReturn);

	CComBSTR bstrKind = L"{26FE99B0-8BBA-4436-83FC-B546C6FD3EDC}";
	*lpbstrReturn = bstrKind.Detach();
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// CAutoGlobals
//////////////////////////////////////////////////////////////////////

CAutoGlobals::CAutoGlobals()
{
}

CAutoGlobals::~CAutoGlobals()
{
}

void CAutoGlobals::Initialize( VCProject *pProj )
{
	// get pointers to the globals data items from the project
	CComQIPtr<IVCProjectImpl> pProjImpl = pProj;
	VSASSERT( pProjImpl, "VCProject object doesn't support IVCProjectImpl! This shouldn't be possible" );
	HRESULT hr = pProjImpl->GetGlobals( &m_spGlobals );
	VSASSERT( SUCCEEDED( hr ) && m_spGlobals, "GetGlobals on project failed" );
}

//-----------------------------------------------------------------------------
// create an instance with reference count 1
//-----------------------------------------------------------------------------
HRESULT CAutoGlobals::CreateInstance( CAutoGlobals **ppAutoGlobals, VCProject *pProj )
{
	CComObject<CAutoGlobals> *pObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CAutoGlobals>::CreateInstance( &pObj );
	RETURN_ON_FAIL_OR_NULL2(hr, pObj, E_OUTOFMEMORY);
	pObj->AddRef();
	pObj->Initialize( pProj );
	*ppAutoGlobals = pObj;
	return hr;
}

STDMETHODIMP CAutoGlobals::get_DTE( DTE **ppDTE )
{
	return GetBuildPkg()->GetDTE(ppDTE);
}

STDMETHODIMP CAutoGlobals::get_Parent( IDispatch **ppDTE )
{
	CHECK_POINTER_VALID( ppDTE );
	return ExternalQueryService( SID_SDTE, IID__DTE, (void **)ppDTE );
}

STDMETHODIMP CAutoGlobals::get_VariableValue( BSTR name, VARIANT *pVal )
{
	CHECK_ZOMBIE(m_spGlobals, IDS_ERR_GLOBALS_ZOMBIE);
	CHECK_POINTER_VALID(pVal);
	CComBSTR bstrValue;
	HRESULT hr = m_spGlobals->GetValue( name, &bstrValue );
	if( SUCCEEDED( hr ) )
	{
		CComVariant varRet( bstrValue );
		varRet.Detach( pVal );
	}
	return hr;
}

STDMETHODIMP CAutoGlobals::put_VariableValue( BSTR name, VARIANT val )
{
	CHECK_ZOMBIE(m_spGlobals, IDS_ERR_GLOBALS_ZOMBIE);
	return m_spGlobals->SetValue( name, val.bstrVal );
}

STDMETHODIMP CAutoGlobals::get_VariablePersists( BSTR name, VARIANT_BOOL *pbVal )
{
	CHECK_ZOMBIE(m_spGlobals, IDS_ERR_GLOBALS_ZOMBIE);
	CHECK_POINTER_VALID(pbVal);
	return m_spGlobals->GetPersistance( name, pbVal );
}

STDMETHODIMP CAutoGlobals::put_VariablePersists( BSTR name, VARIANT_BOOL bVal )
{
	CHECK_ZOMBIE(m_spGlobals, IDS_ERR_GLOBALS_ZOMBIE);
	return m_spGlobals->SetPersistance( name, bVal );
}

STDMETHODIMP CAutoGlobals::get_VariableExists( BSTR name, VARIANT_BOOL *pbVal )
{
	CHECK_ZOMBIE(m_spGlobals, IDS_ERR_GLOBALS_ZOMBIE);
	CHECK_POINTER_VALID(pbVal);
	CComBSTR bstrVal;
	HRESULT hr = m_spGlobals->GetValue( name, &bstrVal );
	if( FAILED( hr ) )
		*pbVal = VARIANT_FALSE;
	else
		*pbVal = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CAutoGlobals::get_VariableNames( VARIANT *pNames )
{
	CHECK_ZOMBIE(m_spGlobals, IDS_ERR_GLOBALS_ZOMBIE);
	CHECK_POINTER_VALID(pNames);

	// get the number of elements (names)
	unsigned long n;
	m_spGlobals->GetCount( &n );
	// create a safe array variant (single dim, of BSTRs)
	CComSafeArray<BSTR> rgNames( n );
	// get all the names
	int i = 0;
	CComBSTR bstrName, bstrValue;
	HRESULT hr = m_spGlobals->GetFirstItem( &bstrName, &bstrValue );
	if( SUCCEEDED( hr ) )
		rgNames.SetAt( i, bstrName );
	else 
	{
		// no items
		return S_OK;
	}
	bstrName.Empty();
	bstrValue.Empty();
	while( SUCCEEDED( m_spGlobals->GetNextItem( &bstrName, &bstrValue ) ) )
	{
		// release our bstrs each time
		rgNames.SetAt( ++i, bstrName );
		bstrName.Empty();
		bstrValue.Empty();
	}
	pNames->vt = VT_ARRAY | VT_BSTR;
	pNames->parray = rgNames.Detach();
	return S_OK;
}

