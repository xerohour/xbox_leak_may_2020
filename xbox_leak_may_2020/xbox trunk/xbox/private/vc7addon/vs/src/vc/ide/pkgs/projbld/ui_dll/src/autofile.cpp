// AutoFile.cpp
// implementation for CAutoFile

#include "stdafx.h"

#include "AutoFile.h"
#include "AutoFilter.h"
#include "AutoProject.h"    
#include "bldpkg.h"
#include "VCArchy.h"
#include "VCFile.h"
#include "VCCodeModels.h"
#include "VCDesignerObjectInternal.h"
#include "VCCodeModelsInternal.h"

CAutoFile::CAutoFile()
{
}

CAutoFile::~CAutoFile()
{
}

//-----------------------------------------------------------------------------
// create an instance of a file node with reference count 1
//-----------------------------------------------------------------------------
HRESULT CAutoFile::CreateInstance(CAutoFile** ppAutoFile, IDispatch* pAutoItems, IDispatch* pAutoProject, IDispatch* pItem)
{
	CComQIPtr<VCFile> spFile = pItem;
	RETURN_ON_NULL(spFile);		// not for us

	CComObject<CAutoFile> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CAutoFile>::CreateInstance(&pPrjObj);
	RETURN_ON_FAIL_OR_NULL2(hr, pPrjObj, E_OUTOFMEMORY);
	pPrjObj->AddRef();
	pPrjObj->Initialize(pAutoItems, pAutoProject, pItem);
	*ppAutoFile = pPrjObj;

	return hr;
}

void CAutoFile::Initialize(IDispatch* pAutoItems, IDispatch* pAutoProject, IDispatch* pItem)
{
	m_spAutoItems = pAutoItems;
	VSASSERT(m_spAutoItems != NULL, "CAutoFile initialized with NULL item list!");
	m_spAutoProject = pAutoProject;
	VSASSERT(m_spAutoProject != NULL, "CAutoFile initialized with NULL project!");
	m_spFile = pItem;
	VSASSERT(m_spFile != NULL, "CAutoFile initialized without a file!");
}

CVCFileNode* CAutoFile::GetFileNode()
{
	CComQIPtr<IVCExternalCookie> spExtCookie = m_spFile;
	RETURN_ON_NULL2(spExtCookie, NULL);

	void* pCookie;
	spExtCookie->get_ExternalCookie(&pCookie);
	RETURN_ON_NULL2(pCookie, NULL);

	return (CVCFileNode*)pCookie;
}

CVCProjectNode* CAutoFile::GetProjectNode(CVCFileNode* pFileNodeIn /* = NULL */)
{
	CVCFileNode* pFileNode;
	if (pFileNodeIn)
		pFileNode = pFileNodeIn;
	else
	{
		pFileNode = GetFileNode();
		RETURN_ON_NULL2(pFileNode, NULL);
	}

	return pFileNode->GetVCProjectNode();
}

/* _ProjectItem methods */
STDMETHODIMP CAutoFile::get_IsDirty(VARIANT_BOOL FAR* lpfReturn)
{
	CHECK_POINTER_VALID(lpfReturn);
	*lpfReturn = FALSE;

	VARIANT_BOOL bReturn2 = VARIANT_FALSE;
	CComPtr<IVsWindowFrame> spWindowFrame;
	HRESULT hr = IsOpen(NULL, FALSE, &bReturn2, &spWindowFrame);
	RETURN_ON_FAIL(hr);
	if (bReturn2 == VARIANT_FALSE || spWindowFrame == NULL)	// not open, so obviously not dirty
		return S_OK;

    CComVariant var;
    hr = spWindowFrame->GetProperty(VSFPROPID_DocData, &var);
    if(hr == S_OK)
    {
        ASSERT(V_VT(&var) == VT_UNKNOWN && V_UNKNOWN(&var) != NULL);
        CComPtr<IVsPersistDocData> spDocData;
        (V_UNKNOWN(&var))->QueryInterface(IID_IVsPersistDocData, (void**)&spDocData);
        ASSERT(spDocData);
        BOOL bIsDirty = FALSE;
        hr = spDocData->IsDocDataDirty(&bIsDirty);
        if(hr == S_OK && bIsDirty)
			*lpfReturn = TRUE;
    }

    return hr;
}

STDMETHODIMP CAutoFile::put_IsDirty(VARIANT_BOOL DirtyFlag)
{
	return E_NOTIMPL;	// CAutoFile::put_IsDirty
}

STDMETHODIMP CAutoFile::get_FileNames(short Index, BSTR FAR* lpbstrReturn)
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE)
	return m_spFile->get_FullPath(lpbstrReturn);
}

STDMETHODIMP CAutoFile::SaveAs(BSTR NewFileName, VARIANT_BOOL FAR* lpfReturn)
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE)
	CHECK_POINTER_VALID(lpfReturn)
	
	return E_NOTIMPL;	// CAutoFile::SaveAs
}

STDMETHODIMP CAutoFile::Save(BSTR /*FileName*/)
{
	return E_NOTIMPL;		// CAutoFile::Save
}

STDMETHODIMP CAutoFile::get_FileCount(short FAR* lpsReturn)
{
	CHECK_POINTER_VALID(lpsReturn)
	*lpsReturn = 1; // only deal with one file at a time right now.
	return S_OK;
}

STDMETHODIMP CAutoFile::get_Name(BSTR FAR* pbstrReturn)
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE)
	return m_spFile->get_Name(pbstrReturn);
}

STDMETHODIMP CAutoFile::put_Name(BSTR bstrName)
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE)
	return m_spFile->put_RelativePath(bstrName);
}

STDMETHODIMP CAutoFile::get_Collection(ProjectItems FAR* FAR* lppcReturn)
{
	CHECK_ZOMBIE(m_spAutoItems, IDS_ERR_FILE_ZOMBIE)
	CHECK_POINTER_VALID(lppcReturn)

	CComQIPtr<ProjectItems> spProjItems = m_spAutoItems;
	*lppcReturn = spProjItems.Detach();
	return S_OK;
}

STDMETHODIMP CAutoFile::get_Properties(Properties FAR* FAR* ppObject)
{
	return DoGetProperties(this, m_spFile, ppObject);
}

STDMETHODIMP CAutoFile::get_DTE(DTE FAR* FAR* lppaReturn)
{
	return GetBuildPkg()->GetDTE(lppaReturn);
}

STDMETHODIMP CAutoFile::get_Kind(BSTR FAR* lpbstrFileName)
{
	CHECK_POINTER_VALID(lpbstrFileName)
	CComBSTR bstrKind = vsProjectItemKindPhysicalFile;
	*lpbstrFileName = bstrKind.Detach();
	return S_OK;
}

STDMETHODIMP CAutoFile::get_ProjectItems(ProjectItems FAR* FAR* lppcReturn)
{
	CHECK_POINTER_VALID(lppcReturn);
	*lppcReturn = NULL;
	return S_OK;
}

HRESULT CAutoFile::IsOpen(BSTR LogView, BOOL bAlwaysProvideLogView, VARIANT_BOOL* lpfReturn, IVsWindowFrame** ppWindowFrame)
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE)
	CHECK_POINTER_VALID(lpfReturn)
	*ppWindowFrame = NULL;

	*lpfReturn = VARIANT_FALSE;

	CVCProjectNode* pProjectNode = GetProjectNode();
	CHECK_ZOMBIE(pProjectNode, IDS_ERR_PROJ_ZOMBIE);

	CVCArchy *pArchy = pProjectNode->GetArchy();
	VSASSERT(pArchy, "No hierarchy found for project!");
	CHECK_ZOMBIE(pArchy, IDS_ERR_FILE_ZOMBIE);

	CComBSTR bstrName;
	m_spFile->get_Name(&bstrName);

	IID guidLogicalView = GUID_NULL;	
	HRESULT hr = (LogView == NULL && !bAlwaysProvideLogView) ? S_OK : IIDFromString(LogView, &guidLogicalView);
	VSASSERT(SUCCEEDED(hr), "Failed to get logical view GUID!");
	if (FAILED(hr))
		guidLogicalView = GUID_NULL;

	CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;  
	hr = ExternalQueryService(SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument, 
		(void **)&pVsUIShellOpenDoc);
	VSASSERT(SUCCEEDED(hr), "Failed to get IVsUIShellOpenDocument from the shell!  (Shell not loaded?  Are we trying to get this service from a secondary thread?)");
	RETURN_ON_FAIL_OR_NULL(hr, pVsUIShellOpenDoc);
  			
	CComPtr<IVsUIHierarchy> pHier;
	BOOL fIsOpen;
	VSITEMID vsid = pArchy->ItemIDFromName(NULL, bstrName);
	VSITEMID  vsidDummy;
	CComPtr<IVsWindowFrame> pFrame;
	hr = pVsUIShellOpenDoc->IsDocumentOpen(pArchy->GetIVsUIHierarchy(), vsid, bstrName, guidLogicalView, IDO_ActivateIfOpen,
		&pHier, &vsidDummy, &pFrame, &fIsOpen);
	VSASSERT(SUCCEEDED(hr), "Failed to determine if a document is open!  (Are we trying to get this info from a secondary thread?  Did the required parameters change on us?");
	if (SUCCEEDED(hr))
		*lpfReturn = fIsOpen ? VARIANT_TRUE : VARIANT_FALSE;
	if (ppWindowFrame && pFrame)
		*ppWindowFrame = pFrame.Detach();

	return hr;
}

STDMETHODIMP CAutoFile::get_IsOpen(BSTR LogView, VARIANT_BOOL FAR* lpfReturn)
{
	return IsOpen(LogView, TRUE, lpfReturn, NULL);
}

STDMETHODIMP CAutoFile::Remove()
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE);
	CVCFileNode* pFileNode = GetFileNode();
	RETURN_ON_NULL2(pFileNode, E_UNEXPECTED);

	return pFileNode->OnDelete(NULL, DELITEMOP_RemoveFromProject);
}

STDMETHODIMP CAutoFile::Delete()
{
	return E_NOTIMPL;	// CAutoFile::Delete, we don't allow Delete from UI, so shouldn't allow it from automation, either
}

STDMETHODIMP CAutoFile::ExpandView()
{
	return E_NOTIMPL;	// CAutoFile::ExpandView, files can't be expanded...
}

STDMETHODIMP CAutoFile::Open(BSTR LogView, Window FAR* FAR* lppfReturn)
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE)
	CHECK_POINTER_VALID(lppfReturn)
	*lppfReturn = NULL;

	CVCProjectNode* pProjectNode = GetProjectNode();
	VSASSERT(pProjectNode != NULL, "No project node for the file!  Probably bad initialization");
	CHECK_ZOMBIE(pProjectNode, IDS_ERR_PROJ_ZOMBIE);

	CVCArchy *pArchy = pProjectNode->GetArchy();
	VSASSERT(pArchy, "No hierarchy for project node!");
	CHECK_ZOMBIE(pArchy, IDS_ERR_PROJ_ZOMBIE);

	CComBSTR bstrName;
	m_spFile->get_FullPath(&bstrName);
	
	CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;  
	HRESULT hr = ExternalQueryService(SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument, 
		(void **)&pVsUIShellOpenDoc);
	VSASSERT(SUCCEEDED(hr), "Failed to get SID_SVsUIShellOpenDocument service!  (Are we in a secondary thread?)");
	RETURN_ON_FAIL_OR_NULL(hr, pVsUIShellOpenDoc);
  			
	IID guidLogicalView;
	hr = IIDFromString(LogView, &guidLogicalView);
	RETURN_ON_FAIL(hr);

	CComPtr<IVsUIHierarchy> pHier;
	BOOL fIsOpen = FALSE;
	VSITEMID  vsidDummy;
	CComPtr<IVsWindowFrame> pFrame;

	hr = pVsUIShellOpenDoc->IsDocumentOpen(pArchy->GetIVsUIHierarchy(), NULL /*vsid, */, bstrName, guidLogicalView,
		IDO_ActivateIfOpen, &pHier, &vsidDummy, &pFrame, &fIsOpen);
	VSASSERT(SUCCEEDED(hr), "Failed to determine if document is open!  (Are we in a secondary thread?  Did parameters change?)");
	RETURN_ON_FAIL(hr);

	if (!fIsOpen)
	{
		// HACK: If not open with the GUID type passed in, try GUID_NULL first.
		// if it is still isn't opened then open it.
		hr = pVsUIShellOpenDoc->IsDocumentOpen(pArchy->GetIVsUIHierarchy(), NULL /* vsid */, bstrName, GUID_NULL, 
			IDO_ActivateIfOpen, &pHier, &vsidDummy, &pFrame, &fIsOpen);
		VSASSERT(SUCCEEDED(hr), "Failed to determine if document is open!  (Are we in a secondary thread?  Did parameters change?)");
	}

	if (!fIsOpen)
	{
		VSITEMID vsid = pArchy->ItemIDFromName(NULL, bstrName);
		hr = pVsUIShellOpenDoc->OpenStandardEditor(OSE_ChooseBestStdEditor, bstrName, guidLogicalView,
			L"%3"/* base filename */, pArchy->GetIVsUIHierarchy(), vsid, DOCDATAEXISTING_UNKNOWN, NULL, &pFrame);
		VSASSERT(SUCCEEDED(hr), "Failed open standard editor for file!");
	}

	if (SUCCEEDED(hr) && pFrame)
	{
		VARIANT varPunk;
		VariantInit(&varPunk);
		V_VT(&varPunk) = VT_UNKNOWN;
		hr = pFrame->GetProperty(VSFPROPID_ExtWindowObject, &varPunk);
		VSASSERT(SUCCEEDED(hr), "Failed to get window object property!  (Are we in a secondary thread?)");
		if (SUCCEEDED(hr))
		{
			hr = varPunk.punkVal->QueryInterface(IID_Window, (void **)lppfReturn); 
			VSASSERT(SUCCEEDED(hr), "Failed to get Window interface!  (Are we in a secondary thread?)");
			varPunk.punkVal->Release();
		}
	}

    return hr;
}


STDMETHODIMP CAutoFile::get_Object(IDispatch **ppProjectItemModel)
{
	return DoGetObject(m_spFile, ppProjectItemModel, IDS_ERR_FILE_ZOMBIE);
}

STDMETHODIMP CAutoFile::get_Extender(BSTR bstrExtenderName, IDispatch **ppExtender)
{
	return DoGetExtender(this, bstrExtenderName, ppExtender);
}

STDMETHODIMP CAutoFile::get_ExtenderNames(VARIANT *pvarExtenderNames)
{
	return DoGetExtenderNames(this, pvarExtenderNames);
}

STDMETHODIMP CAutoFile::get_ExtenderCATID(BSTR *pbstrRetval)
{
	return DoGetExtenderCATID(pbstrRetval);
}

STDMETHODIMP CAutoFile::get_Saved(VARIANT_BOOL *lpfReturn)
{
	return get_IsDirty(lpfReturn);
}

STDMETHODIMP CAutoFile::put_Saved(VARIANT_BOOL Dirty)
{
	return put_IsDirty(Dirty);
}

STDMETHODIMP CAutoFile::get_SourceControl(SourceControl ** /*ppSourceControl*/)
{
	return E_NOTIMPL;	// CAutoFile::get_SourceControl
}

STDMETHODIMP CAutoFile::get_CodeModel(CodeModel** ppCodeModel)
{
	CHECK_ZOMBIE(m_spAutoProject, IDS_ERR_PROJ_ZOMBIE);
	CComQIPtr<_Project> spProject = m_spAutoProject;
	RETURN_ON_NULL(spProject);

	return spProject->get_CodeModel(ppCodeModel);
}

STDMETHODIMP CAutoFile::get_Document(Document ** ppDocument)
{
	CHECK_ZOMBIE(m_spFile, IDS_ERR_FILE_ZOMBIE);
	CHECK_POINTER_VALID(ppDocument);
	*ppDocument = NULL;

	CVCFileNode* pFileNode = GetFileNode();
	RETURN_ON_NULL2(pFileNode, E_UNEXPECTED);

	VARIANT_BOOL bIsOpen = VARIANT_FALSE;
	CComPtr<IVsWindowFrame> spFrame;
	HRESULT hr = IsOpen(NULL, FALSE, &bIsOpen, &spFrame);
	RETURN_ON_FAIL_OR_NULL2(hr, spFrame, S_FALSE); 	// file not open
	
	CComVariant var;
    hr = spFrame->GetProperty(VSFPROPID_DocCookie, &var);
	RETURN_ON_FAIL(hr);

	CComPtr<IVsExtensibility> pExtService;
	hr = ExternalQueryService(SID_SVsExtensibility, IID_IVsExtensibility, (LPVOID*)&pExtService);
	RETURN_ON_FAIL_OR_NULL2(hr, pExtService, E_UNEXPECTED);

    return pExtService->GetDocumentFromDocCookie(V_INT_PTR(&var), ppDocument);
}

STDMETHODIMP CAutoFile::get_SubProject(Project** ppProject)
{
	CHECK_POINTER_VALID(ppProject);
	*ppProject = NULL;
	return S_OK;
}

STDMETHODIMP CAutoFile::get_ContainingProject(Project ** ppProject)
{
	CHECK_POINTER_VALID(ppProject);
	CComQIPtr<Project> pProject = m_spAutoProject;
	VSASSERT( pProject != NULL, "QI for Project interface failed" );
	return pProject.CopyTo( ppProject );
}

STDMETHODIMP CAutoFile::get_ConfigurationManager(ConfigurationManager** ppConfigurationManager)
{
	CVCFileNode* pFileNode = GetFileNode();
	RETURN_ON_NULL2(pFileNode, E_UNEXPECTED);

	CHECK_POINTER_VALID(ppConfigurationManager);
	CVCProjectNode* pProjectNode = GetProjectNode(pFileNode);
	CVCArchy *pArchy = NULL;
	if (pProjectNode)
		pArchy = pProjectNode->GetArchy();
	CHECK_ZOMBIE(pArchy, IDS_ERR_PROJ_ZOMBIE);

	CComPtr<IVsExtensibility> pExtService;
	HRESULT hr = ExternalQueryService(SID_SVsExtensibility, IID_IVsExtensibility, (LPVOID*)&pExtService);
	RETURN_ON_FAIL_OR_NULL(hr, pExtService);

	CComPtr<IVsHierarchy> spHier = pArchy->GetIVsHierarchy();
	return pExtService->GetConfigMgr(spHier, pFileNode->GetVsItemID(), ppConfigurationManager);
}

STDMETHODIMP CAutoFile::AddFolder(BSTR Name, BSTR Kind, ProjectItem **pProjectItem)
{
	CHECK_POINTER_VALID(pProjectItem);
	*pProjectItem = NULL;
	return E_FAIL;	// CAutoFile::AddFolder, files can't have folders added to them
}

STDMETHODIMP CAutoFile::get_FileCodeModel(FileCodeModel ** ppFileCodeModel)
{
	CHECK_POINTER_VALID(ppFileCodeModel);
	CComBSTR bstrFileName;
	get_Name(&bstrFileName);
	if (bstrFileName.Length() == 0)
		CHECK_ZOMBIE(NULL, IDS_ERR_FILE_ZOMBIE);

	CComPtr<IDispatch> spDispFile;
	HRESULT hr = QueryInterface(__uuidof(IDispatch), reinterpret_cast<void **>(&spDispFile));
	RETURN_ON_FAIL(hr);

	CComPtr<VCCodeModelService> spIVCCodeModelService;
	hr = ExternalQueryService(SID_SVCCodeModelService,
		__uuidof(VCCodeModelService), reinterpret_cast<void **>(&spIVCCodeModelService));
	RETURN_ON_FAIL_OR_NULL(hr, spIVCCodeModelService);

	CComPtr<VCFileCodeModel> spVCFileCodeModel;
	if (FAILED(hr = spIVCCodeModelService->CreateFileCodeModel(bstrFileName, &spVCFileCodeModel)))
	{
		return S_FALSE;
	}

	CComPtr<VCFileCodeModelI> spVCFileCodeModelI;
	hr = spVCFileCodeModel.QueryInterface(&spVCFileCodeModelI);
	RETURN_ON_FAIL_OR_NULL(hr, spVCFileCodeModelI);

	if (FAILED(hr = spVCFileCodeModelI->put_Parent(spDispFile)))
	{
		return S_FALSE;
	}

	return spVCFileCodeModelI.QueryInterface(ppFileCodeModel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAutoItems::CAutoItems()
{
}

CAutoItems::~CAutoItems()
{
}

//-----------------------------------------------------------------------------
// create an instance of a project node with reference count 1
//-----------------------------------------------------------------------------
HRESULT CAutoItems::CreateInstance(CAutoItems **ppAutoItems, IDispatch* pAutoParent, IDispatch* pAutoProject, 
	IDispatch* pDispItems)
{
	CComObject<CAutoItems> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CAutoItems>::CreateInstance(&pPrjObj);
	RETURN_ON_FAIL_OR_NULL2(hr, pPrjObj, E_OUTOFMEMORY);
	pPrjObj->AddRef();
	pPrjObj->Initialize(pAutoParent, pAutoProject, pDispItems);
	*ppAutoItems = pPrjObj;
	return hr;
}

void CAutoItems::Initialize(IDispatch* pAutoParent, IDispatch *pAutoProject, IDispatch* pDispItems)
{
	m_spAutoParent = pAutoParent;
	VSASSERT(m_spAutoParent != NULL, "Trying to initialize CAutoItems with a NULL parent!");
	m_spAutoProject = pAutoProject;
	VSASSERT(m_spAutoProject != NULL, "Trying to initialize CAutoItems with a NULL project!");
	if (m_spItems != NULL)
		m_spItems.Release();
	VSASSERT(pDispItems != NULL, "Trying to initialize CAutoItems with a NULL item list!");
	m_spItems = pDispItems;
}

//---------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
STDMETHODIMP CAutoItems::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IVsPackage,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/* _ProjectItems methods */
STDMETHODIMP CAutoItems::Item(VARIANT index, ProjectItem FAR* FAR* lppcReturn)
{
	CHECK_ZOMBIE(m_spItems, IDS_ERR_COLLECTION_ZOMBIE)
	CHECK_POINTER_VALID(lppcReturn)
	
	*lppcReturn = NULL;

	CComPtr<IDispatch> spItem;
	HRESULT hr = m_spItems->Item(index, &spItem);
	RETURN_ON_FAIL_OR_NULL2(hr, spItem, S_FALSE);

	CComPtr<IDispatch> spDispObject;
	hr = GetItem(&spDispObject, this, m_spAutoParent, m_spAutoProject, spItem);
	RETURN_ON_FAIL(hr);
	CComQIPtr<ProjectItem> spProjItem = spDispObject;
	*lppcReturn = spProjItem.Detach();
	return hr;
}

/* static */
HRESULT CAutoItems::GetItem(IDispatch** ppProjItem, IDispatch* pOwner, IDispatch* pAutoParent, IDispatch* pAutoProject, 
	IDispatch* pVCItem)
{
	HRESULT hr = S_OK;
	CComQIPtr<VCFilter> spFilter = pVCItem;
	if (spFilter)
	{
		CAutoFilter* pAutoFilter = NULL;
		hr = CAutoFilter::CreateInstance(&pAutoFilter, pOwner, pAutoProject, pVCItem);
		RETURN_ON_FAIL(hr);
		*ppProjItem = (IDispatch *)pAutoFilter;
		return S_OK;
	}

	// if it wasn't a filter, then it must be a file
	CAutoFile* pAutoFile = NULL;
	if (pAutoParent != pAutoProject)
		hr = CAutoFile::CreateInstance(&pAutoFile, pOwner, pAutoProject, pVCItem);
	else	// need to make sure we get the folder this file really belongs in
	{
		CComQIPtr<VCFile> spFile = pVCItem;
		RETURN_ON_NULL(spFile);
		CComPtr<IDispatch> spParent;
		spFile->get_Parent(&spParent);
		CComQIPtr<VCProject> spProjParent = spParent;
		if (spProjParent)
			hr = CAutoFile::CreateInstance(&pAutoFile, pOwner, pAutoProject, pVCItem);
		else	// parent is a folder
		{
			CComQIPtr<VCFilter> spParentFilter = spParent;
			RETURN_ON_NULL2(spParentFilter, E_UNEXPECTED);

			CComQIPtr<IVCExternalCookie> spCookie = spParentFilter;
			RETURN_ON_NULL(spCookie);
			void* pCookie = NULL;
			spCookie->get_ExternalCookie(&pCookie);
			RETURN_ON_NULL(pCookie);
			CVCFileGroup* pParentGroup = (CVCFileGroup*)pCookie;
			CComVariant var2;
			pParentGroup->GetExtObject(var2);
			if (var2.vt != VT_DISPATCH && var2.vt != VT_UNKNOWN)
				return E_FAIL;
			CComPtr<IDispatch> spRealAutoParent = var2.pdispVal;
			CComPtr<IDispatch> spDispItems;
			HRESULT hr = spParentFilter->get_Items(&spDispItems);
			RETURN_ON_FAIL_OR_NULL(hr, spDispItems);

			CComPtr<CAutoItems> pAutoItems;
			hr = CAutoItems::CreateInstance(&pAutoItems, spRealAutoParent, pAutoProject, spDispItems);
			RETURN_ON_FAIL(hr);

			hr = CAutoFile::CreateInstance(&pAutoFile, pAutoItems, pAutoProject, pVCItem);
		}
	}
	RETURN_ON_FAIL(hr);

	*ppProjItem = (IDispatch *)pAutoFile;
	return S_OK;
}

STDMETHODIMP CAutoItems::get_Parent(IDispatch* FAR* lppptReturn)
{
	return m_spAutoProject.CopyTo(lppptReturn);
}

STDMETHODIMP CAutoItems::get_Count(long FAR* lplReturn)
{
	CHECK_ZOMBIE(m_spItems, IDS_ERR_COLLECTION_ZOMBIE)

	return m_spItems->get_Count(lplReturn);
}

STDMETHODIMP CAutoItems::_NewEnum(IUnknown* FAR* lppiuReturn)
{
	CHECK_ZOMBIE(m_spItems, IDS_ERR_COLLECTION_ZOMBIE)
	CHECK_POINTER_VALID(lppiuReturn)

	*lppiuReturn = NULL;

	CAutoEnumerator<CAutoItems>* pEnumerator = NULL;
	HRESULT hr = CAutoEnumerator<CAutoItems>::CreateInstance(&pEnumerator, this, m_spAutoParent, m_spAutoProject, m_spItems);
	RETURN_ON_FAIL_OR_NULL2(hr, pEnumerator, E_OUTOFMEMORY);

	hr = pEnumerator->QueryInterface(__uuidof(IUnknown), (LPVOID *)lppiuReturn);
	pEnumerator->Release();	// QI above gives us the refcount of 1 we want...
	return hr;
}

STDMETHODIMP CAutoItems::get_DTE(DTE FAR* FAR* lppaReturn)
{
	CHECK_POINTER_VALID(lppaReturn);
	return GetBuildPkg()->GetDTE(lppaReturn);
}

STDMETHODIMP CAutoItems::get_Kind(BSTR FAR* lpbstrFileName)
{
	CHECK_POINTER_VALID(lpbstrFileName)

	CComBSTR bstrKind = L"26FE99B0-8BBA-4436-83FC-B546C6FD3EDC";
	*lpbstrFileName = bstrKind.Detach();
	return S_OK;
}

STDMETHODIMP CAutoItems::AddFromFile(BSTR FileName, ProjectItem FAR* FAR* lppcReturn)
{
	CHECK_POINTER_VALID(lppcReturn)

	*lppcReturn = NULL;

	CComQIPtr<_Project> spRealAutoProject = m_spAutoProject;
	RETURN_ON_NULL2(spRealAutoProject, E_UNEXPECTED);

	CComPtr<IDispatch> spDispProject;
	HRESULT hr = spRealAutoProject->get_Object(&spDispProject);
	VSASSERT(SUCCEEDED(hr) && spDispProject != NULL, "Failed to pick up matching project!");

	CComQIPtr<VCProject> spProject = spDispProject;
	VSASSERT(spProject != NULL, "Failed to get project!");
	RETURN_ON_FAIL(hr);
	CHECK_ZOMBIE(spProject, IDS_ERR_PROJ_ZOMBIE);

	// dunno how to add a filter using this method, so must be a file
	CComPtr<IDispatch> spDispFile;
	hr = spProject->AddFile(FileName, &spDispFile);
	RETURN_ON_FAIL_OR_NULL2(hr, spDispFile, E_UNEXPECTED);

	CAutoFile* pAutoFile = NULL;
	hr = CAutoFile::CreateInstance(&pAutoFile, this, m_spAutoProject, spDispFile);
	*lppcReturn = (ProjectItem *)pAutoFile;
	return hr;
}

HRESULT CAutoItems::GetBaseObject(IDispatch** ppObject)
{
	CHECK_POINTER_NULL(ppObject);

	CComQIPtr<_Project> spAutoProject = m_spAutoParent;
	if (spAutoProject)
		spAutoProject->get_Object(ppObject);
	else
	{
		CComQIPtr<_ProjectItem> spAutoItem = m_spAutoParent;
		RETURN_ON_NULL2(spAutoItem, E_UNEXPECTED);

		spAutoItem->get_Object(ppObject);
	}

	return S_OK;
}

CVCBaseFolderNode* CAutoItems::GetBaseNode()
{
	CComPtr<IDispatch> spDispParent;
	GetBaseObject(&spDispParent);

	CComQIPtr<IVCExternalCookie> spExtCookie = spDispParent;
	RETURN_ON_NULL2(spExtCookie, NULL);

	void* pCookie;
	spExtCookie->get_ExternalCookie(&pCookie);
	RETURN_ON_NULL2(pCookie, NULL);

	return (CVCBaseFolderNode*)pCookie;
}

STDMETHODIMP CAutoItems::AddFromTemplate(BSTR bstrFullSrc, BSTR bstrDest, ProjectItem FAR* FAR* lppcReturn)
{
	CVCBaseFolderNode* pBaseNode;
	HWND hwndDlg;
	HRESULT hr = SetupForAddItemDlg(&pBaseNode, &hwndDlg, lppcReturn);
	RETURN_ON_FAIL(hr);

	CPathW pathFullSrc;
	if (!pathFullSrc.Create(bstrFullSrc))
		RETURN_INVALID();

	CStringW strExt = pathFullSrc.GetExtension();
	strExt.MakeLower();

	VSADDRESULT addResult;
	if (strExt == L".vsz")
		hr = pBaseNode->HandleAddItemDlg(VSADDITEMOP_RUNWIZARD, bstrDest, 1, (LPCOLESTR*)&bstrFullSrc, hwndDlg, &addResult);
	else
		hr = pBaseNode->HandleAddItemDlg(VSADDITEMOP_CLONEFILE, bstrDest, 1, (LPCOLESTR*)&bstrFullSrc, hwndDlg, &addResult);

	return FinishFromAddItemDlg(hr, addResult, bstrDest, lppcReturn);
}

STDMETHODIMP CAutoItems::AddFromDirectory(BSTR Directory, ProjectItem FAR* FAR* lppcReturn)
{
	return E_NOTIMPL;	// CAutoItems::AddFromDirectory -- doesn't really make sense for a reference-based project model
}

STDMETHODIMP CAutoItems::get_ContainingProject(Project **ppProject)
{
	CHECK_POINTER_VALID(ppProject);
	CComQIPtr<Project> pProject = m_spAutoProject;
	VSASSERT( pProject != NULL, "QI for Project interface failed" );
	return pProject.CopyTo( ppProject );
}

STDMETHODIMP CAutoItems::AddFolder(BSTR bstrName, BSTR bstrKind, ProjectItem **pProjectItem)
{
	CHECK_POINTER_VALID(pProjectItem);
	*pProjectItem = NULL;

	OLECHAR *pszVirtualFolder = NULL;
    ::StringFromIID(GUID_ItemType_VirtualFolder, &pszVirtualFolder);
    CComBSTR bstrVirtualFolder = pszVirtualFolder;
    ::CoTaskMemFree(pszVirtualFolder);

	if (bstrKind && bstrVirtualFolder && _wcsicmp(bstrVirtualFolder, bstrKind))
		return E_NOTIMPL;	// CAutoItems::AddFolder, only handle virtual folders

	CComPtr<IDispatch> spDispParent;
	HRESULT hr = GetBaseObject(&spDispParent);
	RETURN_ON_FAIL(hr);

	CComQIPtr<VCFilter> spFilter = spDispParent;
	CComPtr<IDispatch> spNewFilter;
	if (spFilter)
		hr = spFilter->AddFilter(bstrName, &spNewFilter);
	else
	{
		CComQIPtr<VCProject> spProject = spDispParent;
		RETURN_ON_NULL2(spProject, E_UNEXPECTED);
		hr = spProject->AddFilter(bstrName, &spNewFilter);
	}

	RETURN_ON_FAIL(hr);

	return Item(CComVariant(bstrName), pProjectItem);
}

STDMETHODIMP CAutoItems::AddFromFileCopy(BSTR bstrFilePath, ProjectItem** ppOut)
{
	CVCBaseFolderNode* pBaseNode;
	HWND hwndDlg;
	HRESULT hr = SetupForAddItemDlg(&pBaseNode, &hwndDlg, ppOut);
	RETURN_ON_FAIL(hr);

	VSADDRESULT addResult;
	hr = pBaseNode->HandleAddItemDlg(VSADDITEMOP_OPENFILE, L"", 1, (LPCOLESTR*)&bstrFilePath, hwndDlg, &addResult);

	return FinishFromAddItemDlg(hr, addResult, bstrFilePath, ppOut);
}

HRESULT CAutoItems::SetupForAddItemDlg(CVCBaseFolderNode** ppBaseNode, HWND* phwndDlg, ProjectItem** ppOut)
{
	CHECK_POINTER_VALID(ppOut);	// this is a check for outparam of AddFromTemplate and AddFromFileCopy
    *ppOut = NULL;

	*ppBaseNode = GetBaseNode();
	RETURN_ON_NULL2(*ppBaseNode, E_UNEXPECTED);

	CComPtr<IVsUIShell> pUIShell;
	HRESULT hr = GetBuildPkg()->GetVsUIShell(&pUIShell, TRUE /* in main thread */);
	RETURN_ON_FAIL_OR_NULL(hr, pUIShell);
	return pUIShell->GetDialogOwnerHwnd(phwndDlg);
}

HRESULT CAutoItems::FinishFromAddItemDlg(HRESULT hrReturn, VSADDRESULT addResult, BSTR bstrPath, ProjectItem** ppItem)
{
	RETURN_ON_FAIL(hrReturn);
	if (ADDRESULT_Failure == addResult)
		return E_FAIL;

	return Item(CComVariant(bstrPath), ppItem);
}