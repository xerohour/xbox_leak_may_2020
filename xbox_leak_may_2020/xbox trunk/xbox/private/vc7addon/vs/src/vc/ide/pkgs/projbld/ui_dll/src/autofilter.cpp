// AutoFilter.cpp
// implementation for CAutoFilter

#include "stdafx.h"

#include "AutoFilter.h"
#include "AutoFile.h"
#include "AutoProject.h"    
#include "bldpkg.h"
#include "VCArchy.h"
#include "VCFGrp.h"

CAutoFilter::CAutoFilter()
{
}

CAutoFilter::~CAutoFilter()
{
}

//-----------------------------------------------------------------------------
// create an instance of a Filter node with reference count 1
//-----------------------------------------------------------------------------
HRESULT CAutoFilter::CreateInstance(CAutoFilter** ppAutoFilter, IDispatch* pAutoItems, IDispatch* pAutoProject, 
	IDispatch* pItem)
{
	CComQIPtr<VCFilter> spFilter = pItem;
	RETURN_ON_NULL(spFilter);		// not for us

	CComObject<CAutoFilter> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CAutoFilter>::CreateInstance(&pPrjObj);
	RETURN_ON_FAIL_OR_NULL2(hr, pPrjObj, E_OUTOFMEMORY);
	pPrjObj->AddRef();
	pPrjObj->Initialize(pAutoItems, pAutoProject, pItem);
	*ppAutoFilter = pPrjObj;
	return hr;
}

void CAutoFilter::Initialize(IDispatch* pAutoItems, IDispatch* pAutoProject, IDispatch* pItem)
{
	m_spAutoItems = pAutoItems;
	VSASSERT(m_spAutoItems != NULL, "CAutoFilter initialized with NULL filter list!");
	m_spAutoProject = pAutoProject;
	VSASSERT(m_spAutoProject != NULL, "CAutoFilter initialized with NULL project!");
	m_spFilter = pItem;
	VSASSERT(m_spFilter != NULL, "CAutoFilter initialized without a Filter!");
}

CVCFileGroup* CAutoFilter::GetFileGroup()
{
	CComQIPtr<IVCExternalCookie> spExtCookie = m_spFilter;
	RETURN_ON_NULL2(spExtCookie, NULL);

	void* pCookie;
	spExtCookie->get_ExternalCookie(&pCookie);
	RETURN_ON_NULL2(pCookie, NULL);
	return (CVCFileGroup*)pCookie;
}

CVCProjectNode* CAutoFilter::GetProjectNode(CVCFileGroup* pFileGroupIn /* = NULL */)
{
	CVCFileGroup* pFileGroup = NULL;
	if (pFileGroupIn)
		pFileGroup = pFileGroupIn;
	else
		pFileGroup = GetFileGroup();
	RETURN_ON_NULL2(pFileGroup, NULL);

	return pFileGroup->GetVCProjectNode();
}

/* _ProjectItem methods */
STDMETHODIMP CAutoFilter::get_IsDirty(VARIANT_BOOL FAR* lpfReturn)
{
	CHECK_POINTER_VALID(lpfReturn);
	*lpfReturn = VARIANT_FALSE;	// folders are never dirty
	return S_OK;
}

STDMETHODIMP CAutoFilter::put_IsDirty(VARIANT_BOOL DirtyFlag)
{
	return E_FAIL;	// CAutoFilter::put_IsDirty, folders are never dirty
}

STDMETHODIMP CAutoFilter::get_FileNames(short Index, BSTR FAR* lpbstrReturn)
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE)
	return m_spFilter->get_Name(lpbstrReturn);
}

STDMETHODIMP CAutoFilter::SaveAs(BSTR NewFilterName, VARIANT_BOOL FAR* lpfReturn)
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE);
	CHECK_POINTER_VALID(lpfReturn);
	return E_FAIL;	// CAutoFilter::SaveAs, folders can never be saved
}

STDMETHODIMP CAutoFilter::get_FileCount(short FAR* lpsReturn)
{
	CHECK_POINTER_VALID(lpsReturn);
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE);

	CComPtr<IDispatch> spDispFiles;
	m_spFilter->get_Files(&spDispFiles);

	CComQIPtr<IVCCollection> spFiles = spDispFiles;
	RETURN_ON_NULL(spFiles);

	long cFiles = 0;
	spFiles->get_Count(&cFiles);

	*lpsReturn = (short)cFiles;
	return S_OK;
}

STDMETHODIMP CAutoFilter::get_Name(BSTR FAR* pbstrReturn)
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE)
	return m_spFilter->get_Name(pbstrReturn);
}

STDMETHODIMP CAutoFilter::put_Name(BSTR bstrName)
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE)
	return m_spFilter->put_Name(bstrName);
}

STDMETHODIMP CAutoFilter::get_Collection(ProjectItems FAR* FAR* lppcReturn)
{
	CHECK_ZOMBIE(m_spAutoItems, IDS_ERR_FILTER_ZOMBIE)
	CHECK_POINTER_VALID(lppcReturn)

	CComQIPtr<ProjectItems> spProjItems = m_spAutoItems;
	*lppcReturn = spProjItems.Detach();
	return S_OK;
}

STDMETHODIMP CAutoFilter::get_Properties(Properties FAR* FAR* ppObject)
{
	return DoGetProperties(this, m_spFilter, ppObject);
}

STDMETHODIMP CAutoFilter::get_DTE(DTE FAR* FAR* lppaReturn)
{
	return GetBuildPkg()->GetDTE(lppaReturn);
}

STDMETHODIMP CAutoFilter::get_Kind(BSTR FAR* lpbstrFilterName)
{
	CHECK_POINTER_VALID(lpbstrFilterName)

	CComBSTR bstrKind = vsProjectItemKindVirtualFolder;
	*lpbstrFilterName = bstrKind.Detach();
	return S_OK;
}

STDMETHODIMP CAutoFilter::get_ProjectItems(ProjectItems FAR* FAR* lppcReturn)
{
	CHECK_POINTER_VALID(lppcReturn);
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE);

	CComPtr<IDispatch> spDispItems;
	HRESULT hr = m_spFilter->get_Items(&spDispItems);
	RETURN_ON_FAIL_OR_NULL2(hr, spDispItems, E_UNEXPECTED);

	CAutoItems *pAutoItems = NULL;
	hr = CAutoItems::CreateInstance(&pAutoItems, this, m_spAutoProject, spDispItems);
	if (SUCCEEDED(hr))
		*lppcReturn = (ProjectItems *)pAutoItems;

	return hr;
}

STDMETHODIMP CAutoFilter::get_IsOpen(BSTR LogView, VARIANT_BOOL FAR* lpfReturn)
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE)
	CHECK_POINTER_VALID(lpfReturn)

	*lpfReturn = VARIANT_FALSE;
	return S_OK;	// filters are never open
}

STDMETHODIMP CAutoFilter::Remove()
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE);
	CVCFileGroup* pFileGroup = GetFileGroup();
	RETURN_ON_NULL2(pFileGroup, E_UNEXPECTED);

	return pFileGroup->OnDelete(NULL, DELITEMOP_RemoveFromProject);
}

STDMETHODIMP CAutoFilter::Delete()
{
	return S_FALSE;	// CAutoFilter::Delete, can't delete from UI, so shouldn't be able to delete from automation, either
}

STDMETHODIMP CAutoFilter::ExpandView()
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE);
	CVCFileGroup* pFileGroup = GetFileGroup();
	RETURN_ON_NULL2(pFileGroup, E_UNEXPECTED);

	CVCProjectNode* pProjectNode = GetProjectNode(pFileGroup);
	RETURN_ON_NULL2(pProjectNode, E_UNEXPECTED);

	CComPtr<IVsUIShell> spUIShell;
	HRESULT hr = GetBuildPkg()->GetVsUIShell(&spUIShell, TRUE /* in main thread */);
	VSASSERT(SUCCEEDED(hr), "Failed to get IVsUIShell interface");
	RETURN_ON_FAIL_OR_NULL(hr, spUIShell);

	CComPtr<IVsWindowFrame> spFrame;
	hr = spUIShell->FindToolWindow(0, GUID_SolutionExplorer, &spFrame);
	RETURN_ON_FAIL_OR_NULL2(hr, spFrame, E_UNEXPECTED);

	CComVariant srpvt;
	hr = spFrame->GetProperty(VSFPROPID_DocView, &srpvt);
	RETURN_ON_FAIL(hr);

	ASSERT(VT_UNKNOWN == V_VT(&srpvt));
	if (VT_UNKNOWN != V_VT(&srpvt))
		return E_FAIL;
	ASSERT(NULL != V_UNKNOWN(&srpvt));

	CComPtr<IVsWindowPane> spPane;
	hr = V_UNKNOWN(&srpvt)->QueryInterface(IID_IVsWindowPane, (void **)&spPane);
	RETURN_ON_FAIL(hr);
	ASSERT(spPane != NULL);

	CComQIPtr<IVsUIHierarchyWindow> spHierarchyWindow = spPane;
	ASSERT(spHierarchyWindow != NULL);
	RETURN_ON_NULL(spHierarchyWindow);

	return spHierarchyWindow->ExpandItem(pProjectNode->GetArchy() ? pProjectNode->GetArchy()->GetIVsUIHierarchy() : NULL, 
		pFileGroup->GetVsItemID(), EXPF_ExpandFolder);
}

STDMETHODIMP CAutoFilter::Open(BSTR LogView, Window FAR* FAR* lppfReturn)
{
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE)
	CHECK_POINTER_VALID(lppfReturn)
	
	*lppfReturn = NULL;
	return S_FALSE;		// filters cannot be opened
}


STDMETHODIMP CAutoFilter::get_Object(IDispatch **ppProjectItemModel)
{
	return DoGetObject(m_spFilter, ppProjectItemModel, IDS_ERR_FILTER_ZOMBIE);
}

STDMETHODIMP CAutoFilter::get_Extender(BSTR bstrExtenderName, IDispatch **ppExtender)
{
	return DoGetExtender(this, bstrExtenderName, ppExtender);
}

STDMETHODIMP CAutoFilter::get_ExtenderNames(VARIANT *pvarExtenderNames)
{
	return DoGetExtenderNames(this, pvarExtenderNames);
}

STDMETHODIMP CAutoFilter::get_ExtenderCATID(BSTR *pbstrRetval)
{
	return DoGetExtenderCATID(pbstrRetval);
}

STDMETHODIMP CAutoFilter::get_Saved(VARIANT_BOOL *lpfReturn)
{
	return get_IsDirty(lpfReturn);
}

STDMETHODIMP CAutoFilter::put_Saved(VARIANT_BOOL Dirty)
{
	return put_IsDirty(Dirty);
}

STDMETHODIMP CAutoFilter::get_SourceControl(SourceControl ** /*ppSourceControl*/)
{
	return E_NOTIMPL;	// CAutoFilter::get_SourceControl
}

STDMETHODIMP CAutoFilter::get_ConfigurationManager(ConfigurationManager ** ppConfigurationManager)
{
	CHECK_POINTER_VALID(ppConfigurationManager);
	*ppConfigurationManager = NULL;
	return S_FALSE;	// filters don't have configurations
}

STDMETHODIMP CAutoFilter::Save(BSTR /*FileName*/)
{
	return E_NOTIMPL;		// CAutoFilter::Save
}

STDMETHODIMP CAutoFilter::get_Document(Document ** ppDocument)
{
	CHECK_POINTER_VALID(ppDocument);
	*ppDocument = NULL;
	return S_FALSE;		// filters don't have documents
}

STDMETHODIMP CAutoFilter::get_SubProject(Project ** ppProject)
{
	CHECK_POINTER_VALID(ppProject);
	*ppProject = NULL;
	return S_OK;
}

STDMETHODIMP CAutoFilter::get_ContainingProject(Project ** ppProject)
{
	CComQIPtr<Project> pProject = m_spAutoProject;
	VSASSERT( pProject != NULL, "QI for Project interface failed" );
	return pProject.CopyTo( ppProject );
}

STDMETHODIMP CAutoFilter::AddFolder(BSTR Name, BSTR Kind, ProjectItem **pProjectItem)
{
	CHECK_POINTER_VALID(pProjectItem);
	CHECK_ZOMBIE(m_spFilter, IDS_ERR_FILTER_ZOMBIE);

	CComPtr<IDispatch> spNewFilter;
	HRESULT hr = m_spFilter->AddFilter(Name, &spNewFilter);
	RETURN_ON_FAIL_OR_NULL(hr, spNewFilter);

	CAutoFilter* pObject = NULL;
	hr = CAutoFilter::CreateInstance(&pObject, m_spFilter, m_spAutoProject, spNewFilter);
	RETURN_ON_FAIL(hr);

	*pProjectItem = (ProjectItem *)pObject;
	return S_OK;
}

STDMETHODIMP CAutoFilter::get_CodeModel(CodeModel ** ppCodeModel)
{
	CHECK_POINTER_VALID(ppCodeModel);
	*ppCodeModel = NULL;
	return S_FALSE;	// no code model for a filter
}

STDMETHODIMP CAutoFilter::get_FileCodeModel(FileCodeModel ** ppFileCodeModel)
{
	CHECK_POINTER_VALID(ppFileCodeModel);
	*ppFileCodeModel = NULL;
	return S_FALSE;	// no code model for a filter
}
