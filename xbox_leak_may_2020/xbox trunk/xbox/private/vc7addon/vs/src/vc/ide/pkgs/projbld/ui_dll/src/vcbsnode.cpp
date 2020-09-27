// VCBsNode.cpp: implementation of the CVCBaseFolderNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <wininet.h>
#include "VCBsNode.h"
#include "BldPkg.h"
#include "..\resdll\gpcmd.h"
#include "vcarchy.h"
#include <vcguid.h>
interface CommandBars;
#include "compsvcspkg.h"
#include "utils.h"
#include "vcpkg.h"
#include "respkg.h"
#include "atlsafe.h"
#include <vcvsutil.h>
#include "locale.h"
#include "uilocale.h"
#include "vccodemodels.h"
#include "vcclsvw.h"
#include "vcfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVCBaseFolderNode::CVCBaseFolderNode() :
	m_grfStateFlags( 0 ),
	m_pHeadNode( NULL ),
	m_pTailNode( NULL )
{
	m_fAutoExpand = FALSE;
}

CVCBaseFolderNode::~CVCBaseFolderNode()
{
	DeleteAll( NULL );
}

UINT CVCBaseFolderNode::GetIconIndex()
{
	if( m_grfStateFlags & ST_IsExpanded )
		return BMP_OPENFOLDER;
	else
		return BMP_FOLDER;
}

HRESULT CVCBaseFolderNode::DisplayContextMenu()
{
	return S_OK;
}

HRESULT CVCBaseFolderNode::HandleAddItemDlg(
            /* [in] */ VSADDITEMOPERATION dwAddItemOperation,
            /* [in] */ LPCOLESTR pszItemName,
			/* [in] */ DWORD cFilesToAdd,
            /* [in] */ LPCOLESTR rgpszFilesToAdd[],
            /* [in] */ HWND hwndDlg,
            /* [retval][out] */ VSADDRESULT __RPC_FAR *pResult)

{
	CHECK_POINTER_NULL(pResult);
	// default return value is success
	HRESULT hr = S_OK;
	*pResult = ADDRESULT_Success;

	if ((dwAddItemOperation == VSADDITEMOP_CLONEFILE) && (cFilesToAdd > 0))
	{

		CComPtr<IDispatch> pParentDisp = m_dispkeyItem;
		CComQIPtr<VCProject> pProject = pParentDisp;
		CComQIPtr<VCFilter> pFilter = pParentDisp;

		// if we got a filter, we need to get the project associated with it
		if( pFilter )
		{
			VSASSERT( !pProject, "we shouldn't have gotten both a project and a filter" );

			CComPtr<IDispatch> pDisp;
			CComQIPtr<VCFilter> pParentFilter = pFilter;
			while( true )
			{
				pDisp = NULL;
				pParentFilter->get_Parent( &pDisp );
				// did we get a filter or a project?
 				pParentFilter.Release();
				pParentFilter = pDisp;
				// didn't get a filter, try for a project
				if( !pParentFilter )
				{
					pProject = pDisp;
					VSASSERT( pProject, "No parent filter or project!" );
					break;
				}
			}
		}

		if ( cFilesToAdd == 1 )
		{
			CComPtr<IDispatch> pFileAdded;
			CComBSTR bstrFile = pszItemName;
			CComBSTR bstrPath;
			pProject->get_ProjectDirectory(&bstrPath);
			CDirW dir;
			dir.CreateFromString(bstrPath);
			CPathW path;
			path.CreateFromDirAndRelative(dir,bstrFile);
			bstrPath = path;
			CStringW strFrom = rgpszFilesToAdd[0];
			CStringW strTo = bstrPath;

			// does this file already exist in the project??
			// get the project object
			CComPtr<VCProject> pProject;
			GetVCProject(&pProject);
			if (pProject != NULL)
			{
				CComPtr<IDispatch> spDispFiles;
				if (SUCCEEDED(pProject->get_Files(&spDispFiles)))
				{
					CComQIPtr<IVCCollection> spFiles = spDispFiles;
					if (spFiles != NULL)
					{
						CComPtr<IEnumVARIANT> spEnum;
						if (SUCCEEDED(spFiles->_NewEnum(reinterpret_cast<IUnknown **>(&spEnum))) && spEnum != NULL)
						{
							spEnum->Reset();
							while (TRUE)
							{
								CComVariant var;
								hr = spEnum->Next(1, &var, NULL);
								if (hr != S_OK)
									break;
								if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
									continue;

								IDispatch* pDispFile = var.pdispVal;
								CComQIPtr<VCFile> spFile = pDispFile;
								if (spFile == NULL)
									continue;

								// compare the names
								CComBSTR bstrFileName;
								spFile->get_Name( &bstrFileName );
								CComBSTR bstrNewName = pszItemName;
								bstrFileName.ToLower();
								bstrNewName.ToLower();
								
								// if they are the same, msgbox an error and
								// set return to indicate failure
								if( bstrFileName == bstrNewName )
								{
									CStringW strErrMsgFormat;
									strErrMsgFormat.LoadString( IDS_FILE_EXISTS_ERR );
									CStringW strMessage;
									strMessage.Format( strErrMsgFormat, CStringW( bstrNewName ) );
									CComBSTR bstrMsg = strMessage;
									ReportError(bstrMsg);

									*pResult = ADDRESULT_Failure;
									return S_OK;
								}
							}
						}
					}
				}
			}

			// make sure it the destination dir exists. (make it if needed)
			CDirW dirTo;
			BOOL bOK = dirTo.CreateFromPath(path);
			if( !dirTo.CreateOnDisk() )
			{
				*pResult = ADDRESULT_Failure;
				return S_OK;
			}

			// Now check if its already on disk.
			DWORD dwAttr = GetFileAttributesW(strTo);
			if( dwAttr != 0xFFFFFFFF )
			{
				CStringW strErrMsgFormat;
				strErrMsgFormat.LoadString( IDS_FILE_ONDISK_ERR );
				CStringW strMessage;
				strMessage.Format( strErrMsgFormat, strTo );
				CComBSTR bstrMsg = strMessage;
				ReportError(bstrMsg);

				*pResult = ADDRESULT_Failure;
				return S_OK;
			}

			// is it okay to change the project file?
			hr = CanDirtyProject();
			if (FAILED(hr))
			{
				*pResult = ADDRESULT_Cancel;
				return S_OK;
			}


			// file isn't already in the project, so it's okay to clone it
			if( !CopyFileW( strFrom, strTo, FALSE) )
			{
				CStringW strErrMsgFormat;
				strErrMsgFormat.LoadString( IDS_FILE_CREATION_ERR );
				CStringW strMessage;
				strMessage.Format( strErrMsgFormat, strTo );
				CComBSTR bstrMsg = strMessage;
				ReportError(bstrMsg);

				*pResult = ADDRESULT_Failure;
				return S_OK;
			}
			SetFileAttributesW( strTo, FILE_ATTRIBUTE_NORMAL);

			GetBuildPkg()->m_fAddNewItem = 1;

			// and to add it to the project
 			if(pFilter)
 				pFilter->AddFile(bstrPath, &pFileAdded);
 			else if(pProject)
 				pProject->AddFile(bstrPath, &pFileAdded);

			GetBuildPkg()->m_fAddNewItem = 0;
		}
	}
	else if ((dwAddItemOperation == VSADDITEMOP_OPENFILE) && (cFilesToAdd > 0))
	{
		ULONG cFilesT;
		CComPtr<IDispatch> pParentDisp = m_dispkeyItem;
		CComQIPtr<VCProject> pProject = pParentDisp;
		CComQIPtr<VCFilter> pFilter = pParentDisp;

		*pResult = ADDRESULT_Failure;

		// get the project object
		if (pProject == NULL)
			GetVCProject(&pProject);
		RETURN_ON_NULL2(pProject, S_OK);
		CComQIPtr<IVCProjectImpl> spProjectImpl = pProject;
		RETURN_ON_NULL(spProjectImpl);

		for (cFilesT = 0; cFilesT < cFilesToAdd; cFilesT++)
		{
			if (GetBuildPkg())
				GetBuildPkg()->m_fAddNewItem = cFilesToAdd;

			CComPtr<IDispatch> pFileAdded;
			CComBSTR bstrNewName = rgpszFilesToAdd[cFilesT];
			// try adding the file to the project
			if(pFilter)
				hr = pFilter->AddFile(bstrNewName, &pFileAdded);
			else if(pProject)
				hr = pProject->AddFile(bstrNewName, &pFileAdded);

			if (GetBuildPkg())
				GetBuildPkg()->m_fAddNewItem = 0;
				
			// if we get E_ACCESSDENIED back, the file is under SCC, and the
			// user has cancelled the check-out dlg: return with failure code
			if( hr == E_ACCESSDENIED )
			{
				*pResult = ADDRESULT_Cancel;
				return S_OK;
			}
			// if they are the same, msgbox an error and
			// skip to the next file
			if( hr == VCPROJ_E_FILE_EXISTS)
			{
				CStringW strErrMsg;
				strErrMsg.Format( IDS_FILE_EXISTS_ERR, CStringW( bstrNewName ) );
				CComBSTR bstrMsg = strErrMsg;
				ReportError(bstrMsg);
				continue;
			}
			else if( hr == VCPROJ_E_URL_INVALID )
			{
				CStringW strErrMsg;
				strErrMsg.Format( IDS_E_URL_INVALID, CStringW( bstrNewName ) );
				CComBSTR bstrMsg = strErrMsg;
				ReportError(bstrMsg);
				continue;
			}
			else if( FAILED(hr) )
			{
				CStringW strErrMsg;
				strErrMsg.Format( IDS_E_INVALID_NAME, CStringW( bstrNewName ) );
				CComBSTR bstrMsg = strErrMsg;
				ReportError(bstrMsg, hr);
				continue;
			}

			*pResult = ADDRESULT_Success;
		}
	}

	else if(dwAddItemOperation ==  VSADDITEMOP_RUNWIZARD)
	{   // Wizard was selected. 
		VSASSERT(cFilesToAdd >= 1, "Must add at least one file here...");
		if(cFilesToAdd < 1)
			return E_FAIL;

		CComPtr<IVsExtensibility> pVsExt;
		hr = ExternalQueryService(SID_SVsExtensibility, IID_IVsExtensibility, (void **)&pVsExt);
		RETURN_ON_FAIL_OR_NULL(hr, pVsExt);
		// Must be a VC++ Project
		//

		CComPtr<VCProject> pProject;
		GetVCProject(&pProject);
		RETURN_ON_NULL2(pProject, E_NOINTERFACE);

		CComSafeArray<VARIANT> saWizardParameters(6);

		// [0] = Wizard type guid  (bstr)
		//

		#define GUID_AddItemWizard  L"{0F90E1D1-4999-11D1-B6D1-00A0C90F2744}"
		CComVariant(GUID_AddItemWizard).Detach(&saWizardParameters.GetAt(0));

		//  [1] = Project name  (bstr)
		//

		CComBSTR bstrProjectName;
		pProject->get_Name(&bstrProjectName);
		CComVariant(bstrProjectName).Detach(&saWizardParameters.GetAt(1));
		
		//  [2] = ProjectItems collection (IDispatch)
		//

		CComVariant varPrj;
		hr = GetExtObject(varPrj);
		CComQIPtr<_Project> pPrj = varPrj.pdispVal;
		CComPtr<ProjectItems> spItems;

		if( pPrj )
		{
			pPrj->get_ProjectItems(&spItems);
		}
		else
		{
			CComQIPtr<_ProjectItem> pPrjItem = varPrj.pdispVal;
			if( pPrjItem )
			{
				pPrjItem->get_ProjectItems(&spItems);
			}
		}

		CComVariant(spItems).Detach(&saWizardParameters.GetAt(2));

		// [3] = Local Directory (bstr)
		//

		CComBSTR bstrPath;
		pProject->get_ProjectDirectory(&bstrPath);
		CComVariant(bstrPath).Detach(&saWizardParameters.GetAt(3));

		//  [4] = Filename the user typed (rgpszFilesToOpen[1])(bstr)
		//

		CComVariant(CComBSTR(pszItemName)).Detach(&saWizardParameters.GetAt(4));
		

		//  [5] = Product install Directory (bstr)
		//

		CComPtr<IVsShell> spVsShell;
		CComBSTR bstrValue;
		if (SUCCEEDED(ExternalQueryService(SID_SVsShell, IID_IVsShell, (void**)&spVsShell)))
		{
			CComVariant vVirtualRegRoot;
			if (SUCCEEDED(spVsShell->GetProperty(VSSPROPID_VirtualRegistryRoot, &vVirtualRegRoot)) && vVirtualRegRoot.vt == VT_BSTR)
			{
				CStringW strProductKey(vVirtualRegRoot.bstrVal);
				strProductKey += L"\\Setup\\VC";
				CRegKey key;
				if (key.Open(HKEY_LOCAL_MACHINE, strProductKey, KEY_READ) == ERROR_SUCCESS)
				{
					wchar_t szValue[2048];
					DWORD dwCount = 2048;
					// deprecated LONG lRes = key.QueryValue(szValue, L"ProductDir", &dwCount);
					LONG lRes = key.QueryStringValue(L"ProductDir", szValue, &dwCount);
					key.Close();
					bstrValue = szValue;
				}
			}
		}

		if (!bstrValue.Length())
		{
			bstrValue = L"c:\\";
		}

		CComVariant(bstrValue).Detach(&saWizardParameters.GetAt(5)); 

		// REVIEW64(MikhailA): I am casting HWND to 32bits since this parameter
		// is actually never used by VS Environment (talk to CraigS about this)
		if (GetBuildPkg())
			GetBuildPkg()->m_fAddNewItem = 1;
		// The first filename is the name of the wizard

		CComBSTR bstrWizardName(rgpszFilesToAdd[0]);
		wizardResult eExecResult;

		hr = pVsExt->RunWizardFile(bstrWizardName, PtrToLong(hwndDlg), &(saWizardParameters.m_psa), &eExecResult);
		if (GetBuildPkg())
			GetBuildPkg()->m_fAddNewItem = 0;
		
		*pResult = ADDRESULT_Failure;
		if(hr == OLE_E_PROMPTSAVECANCELLED || hr == VS_E_WIZARDBACKBUTTONPRESS)
		{
			*pResult = ADDRESULT_Failure;
			return S_OK;
//			*pResult = ADDRESULT_Cancel;
		}
		else
		{
			if (SUCCEEDED(hr))
			{
				if(eExecResult == wizardResultSuccess)
					*pResult = ADDRESULT_Success;
				else if(eExecResult == wizardResultCancel)
				{
					*pResult = ADDRESULT_Cancel;
					hr = OLE_E_PROMPTSAVECANCELLED;
				}
				else if(eExecResult == wizardResultBackOut)
				{
					*pResult = ADDRESULT_Failure;
					hr = VS_E_WIZARDBACKBUTTONPRESS;
						 
				}
			}
		}
	}
	return hr;
}

//-----------------------------------------------------------------------------
// Called via IOleCommandTgt to update the status of our commands
//-----------------------------------------------------------------------------
STDMETHODIMP CVCBaseFolderNode::QueryStatus(
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[],
		OLECMDTEXT *pCmdText)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	CVCNode::enumKnownGuidList eGuid;
	if (*pguidCmdGroup == guidVSStd97)
		eGuid = CVCNode::eGuidVSStd97;
	else if (*pguidCmdGroup == guidVSStd2K)
		eGuid = CVCNode::eGuidVSStd2K;
	else	// let the base class deal with it
		return CVCNode::QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);

	HRESULT hr = NOERROR;
	for (ULONG i = 0; i < cCmds && SUCCEEDED(hr); i++)
		hr = QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, i);

	return hr;
}

HRESULT CVCBaseFolderNode::QueryStatusAtIndex(CVCNode::enumKnownGuidList eGuid, const GUID* pguidCmdGroup, 
	OLECMD prgCmds[], OLECMDTEXT *pCmdText, ULONG nIndex)
{
	if (eGuid == eGuidUninitialized)
	{
		if (*pguidCmdGroup == guidVSStd2K)
			eGuid = CVCNode::eGuidVSStd2K;
		else if (*pguidCmdGroup == guidVSStd97)
			eGuid = CVCNode::eGuidVSStd97;
		else
			return CVCNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);
	}

	enumEnableStates fEnable = eEnableSupported;
	if (eGuid == CVCNode::eGuidVSStd2K)
	{
		switch(prgCmds[nIndex].cmdID)
		{
			case ECMD_ADDWEBREFERENCECTX:
				fEnable = CanAddWebRef() ? eEnableSupportedAndEnabled : eEnableSupported;
				break;
			case ECMD_ADDRESOURCE:
				fEnable = eEnableSupportedAndEnabled;
				break;
			default:
				return CVCNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);
		}
	}
	else if(eGuid== CVCNode::eGuidVSStd97)
	{	// Common VB commands
		switch(prgCmds[nIndex].cmdID)
		{
			case cmdidAddClass:
			case cmdidAddExistingItem:
			case cmdidAddNewItem:
			case cmdidNewFolder:
				fEnable = eEnableSupportedAndEnabled;
				break;
			default:
				return CVCNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);
		}
	}
	else
		return CVCNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);

	prgCmds[nIndex].cmdf = TranslateEnableState(fEnable);
	return NOERROR;
}


//-----------------------------------------------------------------------------
// Command executor
//-----------------------------------------------------------------------------
STDMETHODIMP CVCBaseFolderNode::Exec(
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	HRESULT hr = S_OK;
	if(*pguidCmdGroup == guidVSStd2K)
	{
		switch(nCmdID)
		{
			case ECMD_ADDRESOURCE:
				hr = OnAddResource();
				break;
			case ECMD_ADDWEBREFERENCECTX:
			case ECMD_ADDWEBREFERENCE:
				OnAddWebRef();	// we've already reported any errors, don't let the shell add lame message boxes after the fact
				break;
			default:
				hr = CVCNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut); //OLECMDERR_E_NOTSUPPORTED;
				break;
		}
	}
	else if(*pguidCmdGroup == guidVSStd97)
	{	// Common VB commands
		switch(nCmdID)
		{
			case cmdidAddClass:
				hr = OnAddClass();
				break;
			case cmdidAddNewItem:
				hr = OnAddNewFiles();
				break;
			case cmdidAddExistingItem:
				hr = OnAddExistingFiles();
				break;
			case cmdidNewFolder:
				hr = OnNewFolder();
				break;
            case cmdidPaste:
                hr = OnPaste();
				break;
			default:
				hr = CVCNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut); //OLECMDERR_E_NOTSUPPORTED;
				break;
		}
	}
	else
	{	// Unknown command set
		hr = OLECMDERR_E_NOTSUPPORTED;
	}
	
	return hr;
}

HRESULT CVCBaseFolderNode::OnNewFolder()
{
	IDispatch *pContainer = GetPropertyContainer();
	VSASSERT(pContainer != NULL, "Failed to get property container!");
	RETURN_ON_NULL2(pContainer, S_FALSE);

	CComPtr<IDispatch> pDispFilter;
	CComQIPtr<VCFilter> pFilter = pContainer;
	CComQIPtr<VCProject> pProject = pContainer;
	CComQIPtr<IVCFilterImpl> pFilterImpl = pFilter;
	CComQIPtr<IVCProjectImpl> pProjectImpl = pProject;
	VSASSERT(pFilter || pProject, "Container wasn't a filter or a project!");
	if (pFilter == NULL && pProject == NULL)
		return E_FAIL;

	// new name = "NewFolder" + the first available number
	CComBSTR bstrGroup;
	CComBSTR bstrNewName;
	bstrNewName.LoadString( IDS_NEWFOLDER );
	bstrGroup = bstrNewName + L"1";
	CComBSTR bstrGroupFilter = L"";
	unsigned short i = 2;
	// 10 (decimal) digit equals 32 bit short
	wchar_t wszNum[10] = {0};
	HRESULT hr = S_OK;
	if (pFilterImpl)
		hr = pFilterImpl->CanAddFilter2(bstrGroup, VARIANT_FALSE /* no errorinfo */);
	else if (pProjectImpl)
		hr = pProjectImpl->CanAddFilter2(bstrGroup, VARIANT_FALSE /* no errorinfo */);
	else
		hr = E_UNEXPECTED;
	if (FAILED(hr) && hr != VCPROJ_E_FILTER_EXISTS)
		return hr;

	while (hr == VCPROJ_E_FILTER_EXISTS && i < USHRT_MAX)
	{
		_itow( i, wszNum, 10 );
		// is there a filter with this name already??
		bstrGroup = bstrNewName + wszNum;
		if (pFilterImpl)
			hr = pFilterImpl->CanAddFilter2(bstrGroup, VARIANT_FALSE /* no errorinfo */);
		else
			hr = pProjectImpl->CanAddFilter2(bstrGroup, VARIANT_FALSE /* no errorinfo */);
		i++;
	}
	if( i == USHRT_MAX )
		return E_FAIL;

	// Filter
	if( pFilter )
	{
		// Add a new group now
		hr = pFilter->AddFilter(bstrGroup, &pDispFilter);
		if(SUCCEEDED(hr) && pFilter)
		{
			CComQIPtr<VCFilter> pFilterImpl = pDispFilter;
			pFilterImpl->put_Filter(bstrGroupFilter);
		}
	}
	// Project
	else
	{
		// Add a new group now
		hr = pProject->AddFilter(bstrGroup, &pDispFilter);
		// AddFilter returns S_FALSE if folder already exists, 
		// so check for S_OK explicitly
		if( hr == S_OK )
		{
			CComQIPtr<VCFilter> pFilterImpl = pDispFilter;
			if (pFilterImpl)
				pFilterImpl->put_Filter(bstrGroupFilter);
		}
	}

	// select the added folder for naming by the user
	CComVariant pvt;
	CComPtr<IVsWindowPane> pIVsWindowPane;
	CComPtr<IVsWindowFrame> pIVsWindowFrame;
	CComPtr<IVsUIHierarchyWindow> pIVsUIHierarchyWindow;

	CComPtr<IVsUIShell> pUIShell;
	hr = GetBuildPkg()->GetVsUIShell(&pUIShell, TRUE /* in main thread */);
	RETURN_ON_FAIL(hr);

	hr = pUIShell->FindToolWindow( NULL, GUID_SolutionExplorer, &pIVsWindowFrame );
	RETURN_ON_FAIL_OR_NULL(hr, pIVsWindowFrame);

	hr = pIVsWindowFrame->GetProperty( VSFPROPID_DocView, &pvt );
	RETURN_ON_FAIL(hr);
	if( pvt.vt != VT_UNKNOWN )
 		return E_FAIL;

	hr = pvt.punkVal->QueryInterface( IID_IVsWindowPane, (void **)&pIVsWindowPane );
	RETURN_ON_FAIL_OR_NULL(hr, pIVsWindowPane);

	hr = pIVsWindowPane->QueryInterface( IID_IVsUIHierarchyWindow, (void **)&pIVsUIHierarchyWindow );
	RETURN_ON_FAIL_OR_NULL(hr, pIVsUIHierarchyWindow);

	// need to get the itemid for the new folder
	// walk the children of this node, looking for the folder
	CVCNode *pNode, *pHeadNode;
	pNode = pHeadNode = GetHead();
	do
	{
		if (pNode)
		{
			// if this is the node, break
			if( pNode->GetKindOf() == Type_CVCFileGroup )
			{
				CComBSTR bstrName;
				pNode->GetName( &bstrName );
				if( bstrName && _wcsicmp( bstrName, bstrGroup ) == 0 )
					break;
			}
			// get the next node
			pNode = pNode->GetNext();
		}
	}
	while( pNode );
	RETURN_ON_NULL(pNode);

	CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(GetHierarchy());
	hr = pIVsUIHierarchyWindow->ExpandItem( spHier, pNode->GetVsItemID(), EXPF_SelectItem );
	RETURN_ON_FAIL(hr);
	return pIVsUIHierarchyWindow->ExpandItem( spHier, pNode->GetVsItemID(), EXPF_EditItemLabel );
}

HRESULT CVCBaseFolderNode::OnAddExistingFiles()
{
	CComPtr<IVsAddProjectItemDlg> srpIVsAddItemDlg;
	HRESULT hr = ::ExternalQueryService(SID_SVsAddProjectItemDlg, IID_IVsAddProjectItemDlg, (void**)&srpIVsAddItemDlg);
	VSASSERT(srpIVsAddItemDlg, "Failed to get VsAddProjectItemDlg service.  Did the ID change?  Do you need to investigate threading issues?");
	RETURN_ON_FAIL_OR_NULL(hr, srpIVsAddItemDlg);

	BOOL bDontShowAgain;
	LPCWSTR pszExpandDir = NULL;
	LPCWSTR pszSelectItem = NULL;
	CStringW strFilter;
	strFilter.LoadString(g_hInstLocale, IDS_FILTER_ALL);
	CComBSTR bstrFilter = strFilter;

	CComPtr<IVsProject> spProj = VCQI_cast<IVsProject>(static_cast<CVCArchy*>(GetHierarchy()));
	hr = srpIVsAddItemDlg->AddProjectItemDlg((VSITEMID) this, IID_IVCArchy, 
				spProj,
				VSADDITEM_AllowMultiSelect | VSADDITEM_AddExistingItems | VSADDITEM_AllowStickyFilter,
				pszExpandDir, pszSelectItem, NULL, &bstrFilter, &bDontShowAgain);

	// Note that HandleAddItemDlg will be called via the hierarchy IVsProject
	// to add the items.

	return hr;
}

HRESULT CVCBaseFolderNode::OnAddNewFiles()
{
	CComPtr<IVsAddProjectItemDlg> srpIVsAddItemDlg;
	HRESULT hr = ::ExternalQueryService(SID_SVsAddProjectItemDlg, IID_IVsAddProjectItemDlg, (void**)&srpIVsAddItemDlg);
	VSASSERT(srpIVsAddItemDlg, "Failed to get VsAddProjectItemDlg service.  Did the ID change?  Do you need to investigate threading issues?");
	RETURN_ON_FAIL_OR_NULL(hr, srpIVsAddItemDlg);

	BOOL bDontShowAgain;
	LPCWSTR pszExpandDir = NULL;
	LPCWSTR pszSelectItem = NULL;
	CStringW strFilter;
	strFilter.LoadString(g_hInstLocale, IDS_FILTER_ALL);
	CComBSTR bstrFilter = strFilter;

	CComPtr<VCProject> pProject;
	GetVCProject(&pProject);
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);
		
	CComBSTR bstrPath;
	pProject->get_ProjectDirectory(&bstrPath);

	CComPtr<IVsProject> spProj = VCQI_cast<IVsProject>(static_cast<CVCArchy*>(GetHierarchy()));
	hr = srpIVsAddItemDlg->AddProjectItemDlg((VSITEMID) this, IID_IVCArchy, 
				spProj,
				VSADDITEM_AddNewItems | VSADDITEM_SuggestTemplateName | VSADDITEM_ShowLocationField,
				pszExpandDir, pszSelectItem, &bstrPath, &bstrFilter, &bDontShowAgain);

	// Note that HandleAddItemDlg will be called via the hierarchy IVsProject
	// to add the items.

	return hr;
}

HRESULT CVCBaseFolderNode::OnAddClass()
{
	CComPtr<IVsAddProjectItemDlg2> srpIVsAddItemDlg;
	HRESULT hr = ::ExternalQueryService(SID_SVsAddProjectItemDlg, IID_IVsAddProjectItemDlg2, (void**)&srpIVsAddItemDlg);
	VSASSERT(srpIVsAddItemDlg, "Failed to get VsAddProjectItemDlg service.  Did the ID change?  Do you need to investigate threading issues?");
	RETURN_ON_FAIL_OR_NULL(hr, srpIVsAddItemDlg);

	CComBSTR bstrFilter;
	bstrFilter.LoadString(IDS_FILTER_ALL);

	CComPtr<VCProject> pProject;
	GetVCProject(&pProject);
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

	// Note that HandleAddItemDlg will be called via the hierarchy IVsProject
	// to add the items.

	BOOL bDontShowAgain;
	LPCWSTR pszExpandDir = NULL;
	LPCWSTR pszSelectItem = NULL;

	CComBSTR bstrPath;
	pProject->get_ProjectDirectory(&bstrPath);
	
	CComBSTR bstrName;
	pProject->get_Name(&bstrName);
	CComBSTR bstrDialogTitle;
	bstrDialogTitle.LoadString(IDS_ADDCLASS);
	bstrDialogTitle += L" - ";
	bstrDialogTitle += bstrName;

	CComPtr<IVsProject> spProj = VCQI_cast<IVsProject>(static_cast<CVCArchy*>(GetHierarchy()));
	hr = srpIVsAddItemDlg->AddProjectItemDlgTitledEx(
				(VSITEMID) this, IID_IVCAddClassHierarchy,
				spProj,
				VSADDITEM_AddNewItems | VSADDITEM_HideNameField | VSADDITEM_AllowSingleTreeRoot | VSADDITEM_ExpandSingleTreeRoot,
				bstrDialogTitle, NULL, L"vc.addclass", bstrPath, pszSelectItem, NULL, &bstrFilter, &bDontShowAgain);
	return hr;
}

// we can only add a web reference for non-makefile configuration types
BOOL CVCBaseFolderNode::CanAddWebRef()
{
	CVCProjectNode* pProjNode = GetVCProjectNode();
	RETURN_ON_NULL2(pProjNode, FALSE);

	CComPtr<IVCGenericConfiguration> spGenCfg;
	HRESULT hr = pProjNode->GetActiveConfig(&spGenCfg);
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spGenCfg, FALSE);

	CComQIPtr<IVCCfg> spCfg = spGenCfg;
	RETURN_ON_NULL2(spCfg, FALSE);

	CComPtr<IDispatch> spDispProjCfg;
	hr = spCfg->get_Object(&spDispProjCfg);
	CComQIPtr<VCConfiguration> spProjCfg = spDispProjCfg;
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spProjCfg, FALSE);

	ConfigurationTypes configType;
	if (FAILED(spProjCfg->get_ConfigurationType(&configType)) || configType == typeUnknown ||
		configType == typeGeneric)
		return FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// Called to add a web reference. Puts up the web references ui to allow the
// user to select a web reference.
//-----------------------------------------------------------------------------
HRESULT CVCBaseFolderNode::OnAddWebRef()
{   
	CComPtr<IVsAddWebReferenceDlg> srpAddWebRefDlg;
	HRESULT hr = ExternalQueryService(SID_SVsAddWebReferenceDlg, IID_IVsAddWebReferenceDlg, (void**)&srpAddWebRefDlg);
	RETURN_ON_FAIL_OR_NULL(hr, srpAddWebRefDlg);

	BOOL bCancelled = FALSE;
	CComBSTR bstrUrl;
	hr = srpAddWebRefDlg->AddWebReferenceDlg(&bstrUrl, &bCancelled);
	RETURN_ON_FAIL(hr);

	if(bCancelled)
		return OLE_E_PROMPTSAVECANCELLED;

	CComQIPtr<VCProject> spProject = m_dispkeyItem;
	CComQIPtr<VCFilter> spFilter = m_dispkeyItem;
	if (spFilter == NULL && spProject == NULL)
		return E_UNEXPECTED;

	// Go add the requested reference
	CComPtr<IDispatch> spDispFile;
	if (spFilter)
		hr = spFilter->AddWebReference(bstrUrl, &spDispFile);
	else
		hr = spProject->AddWebReference(bstrUrl, &spDispFile);
	RETURN_ON_FAIL(hr);

	CComQIPtr<IVCExternalCookie> spFileCookie = spDispFile;
	RETURN_ON_NULL2(spFileCookie, E_UNEXPECTED);

	CVCFileNode* pFileNode = NULL;
	hr = spFileCookie->get_ExternalCookie((void **)&pFileNode);
	RETURN_ON_FAIL_OR_NULL2(hr, pFileNode, E_UNEXPECTED);

	CVCPtrList ptrList;
	ptrList.AddHead(pFileNode);
	CHECK_ZOMBIE(GetVCProjectNode(), IDS_ERR_PROJ_ZOMBIE);
	hr = GetVCProjectNode()->CompileFileItems(ptrList);


	// Get The file code model for the stdafx.h

	// Find the project Node
	CComVariant varPrj;
	hr = GetExtObject(varPrj);
	CComQIPtr<_Project> pPrj = varPrj.pdispVal;
	if( spFilter )
	{
		CComQIPtr<_ProjectItem> pPrjItem = varPrj.pdispVal;
		hr = pPrjItem->get_ContainingProject(&pPrj);
	}
	RETURN_ON_NULL2(pPrj, S_OK);

	// Get the active config
	CVCProjectNode* pProjNode = GetVCProjectNode();
	RETURN_ON_NULL2(pProjNode, S_OK);

	CComPtr<IVCGenericConfiguration> spGenCfg;
	if (FAILED(pProjNode->GetActiveConfig(&spGenCfg)) || spGenCfg == NULL)
		return S_OK;

	CComQIPtr<IVCCfg> spCfg = spGenCfg;
	RETURN_ON_NULL2(spCfg, S_OK);

	CComPtr<IDispatch> spDispProjCfg;
	if (FAILED(spCfg->get_Object(&spDispProjCfg)) || spDispProjCfg == NULL)
		return S_OK;

	CComQIPtr<VCConfiguration> spProjCfg = spDispProjCfg;
	RETURN_ON_NULL2(spProjCfg, S_OK);

	// if we are managed.
	// find the output file from the WSDL
	CComQIPtr<IVCFileImpl> pFileImpl = spDispFile;
	RETURN_ON_NULL2(pFileImpl, S_OK);

	CComPtr<VCFileConfiguration> spFileCfg;
	hr = pFileImpl->GetFileConfigurationForProjectConfiguration(spProjCfg, &spFileCfg);

	// get the tool and Check the MC++ wrapper
	VARIANT_BOOL b = VARIANT_FALSE;
	spProjCfg->get_ManagedExtensions(&b);

	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spFileCfg;
	RETURN_ON_NULL2(spFileCfgImpl, S_OK);

	CComPtr<IDispatch> spDispTool;
	spFileCfg->get_Tool(&spDispTool);
	CComQIPtr<VCWebServiceProxyGeneratorTool> pWebTool = spDispTool;
	CComBSTR bstrOutputTemp;
	RETURN_ON_NULL2(pWebTool, S_OK);
	pWebTool->get_Output(&bstrOutputTemp);
	CComQIPtr<IVCPropertyContainer> spPropContainer = spFileCfg;
	RETURN_ON_NULL2(spPropContainer, S_OK);
	CComBSTR bstrOutput;
	hr = spPropContainer->Evaluate(bstrOutputTemp, &bstrOutput);
	
	if( b == VARIANT_TRUE )
	{
		// Check if the WebService.h is in the project
		CComPtr<ProjectItems> pItems;
		pPrj->get_ProjectItems(&pItems);
		CComQIPtr<ProjectItem> pItem;
		CComVariant var = L"WebService.h";
		RETURN_ON_NULL2(pItems, S_OK);
		hr = pItems->Item(var, &pItem);
		// if no, add it

		if( pItem == NULL )
		{
			CComBSTR bstrProjDir;
			spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
			RETURN_ON_NULL2(bstrProjDir, S_OK);
			CDirW dirProj;
			dirProj.CreateFromKnown(bstrProjDir);
			CPathW pathFile;
		    CComBSTR bstrH = L"WebService.h";
			if (!pathFile.CreateFromDirAndFilename(dirProj, bstrH) || !pathFile.CreateOnDisk())
				return S_OK;

		    CComPtr<IDispatch> pDispFile;
			if (spProject)
				spProject->AddFile( bstrH, &pDispFile );
			else
				spFilter->AddFile( bstrH, &pDispFile );
			CComQIPtr<IVCExternalCookie> spFileCookie = pDispFile;
			RETURN_ON_NULL2(spFileCookie, S_OK);

			CVCFileNode* pFileNode = NULL;
			hr = spFileCookie->get_ExternalCookie((void **)&pFileNode);
			RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, pFileNode, S_OK);
			CComVariant var;
			pFileNode->GetExtObject(var);
			if (var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH)
			    pItem = var.pdispVal;
		}

		// now get the code model
		CComPtr<FileCodeModel> pCM;
		RETURN_ON_NULL2(pItem, S_OK);

		pItem->get_FileCodeModel(&pCM);
		CComQIPtr<VCFileCodeModel> pFCM = pCM;
		RETURN_ON_NULL2(pFCM, S_OK);

		// add to the end of it this web ref
		CComVariant Pos;
		CComPtr<VCCodeUsing> pVCCodeUsing;
	
		CComPtr<CodeElements> spCodeElements;
		long nCount = 0;
		if (FAILED(hr = pFCM->get_Usings(&spCodeElements)))
			return hr;
		if (FAILED(hr = spCodeElements->get_Count(&nCount)))
			return hr;

		bool SystemVal = false;
		bool SystemDataVal = false;
		bool SystemWebVal = false;

		for(int cnt=1; cnt<=nCount; cnt++)
		{
			CComPtr<CodeElement> spCodeElement;
			if (FAILED(hr = spCodeElements->Item(CComVariant(cnt), &spCodeElement)))
				return hr;
			CComBSTR bstrValue;
			if (FAILED(hr = spCodeElement->get_Name(&bstrValue)))
				return hr;
			if (0 == _wcsicmp (bstrValue, L"<System.DLL>"))
				SystemVal = true;
			else if (0 == _wcsicmp (bstrValue, L"<System.Data.DLL>"))
				SystemDataVal = true;
			else if (0 == _wcsicmp (bstrValue, L"<System.Web.Services.DLL>"))
				SystemWebVal = true;
			spCodeElement.Release();
		}
		if (!SystemVal)
		{
			pFCM->AddUsing(L"<System.DLL>", Pos, &pVCCodeUsing);
			pVCCodeUsing.Release();
		}

		if (!SystemDataVal)
		{
			pFCM->AddUsing(L"<System.Data.DLL>", Pos, &pVCCodeUsing);
			pVCCodeUsing.Release();
		}

		if (!SystemWebVal)
		{
			pFCM->AddUsing(L"<System.Web.Services.DLL>", Pos, &pVCCodeUsing);
			pVCCodeUsing.Release();
		}

		CComBSTR bstrTemp = L"<" + bstrOutput + L">";
		pFCM->AddUsing(bstrTemp, Pos, &pVCCodeUsing);
	}
	else
	{
		// add that file to the project in the generated file folder.
		CComPtr<IDispatch> pDispFile;
		if (spProject)
			spProject->AddFile( bstrOutput, &pDispFile );
		else
			spFilter->AddFile( bstrOutput, &pDispFile );
	}

	return S_OK;
}

HRESULT CVCBaseFolderNode::OnAddResource()
{
    // get the resource editor service
    CComPtr<IVCWin32CompositionService> pCompSvc;
    HRESULT hr = ExternalQueryService( SID_SVCWin32ResourceService, _uuidof(IVCWin32CompositionService), (void**)&pCompSvc );
	RETURN_ON_FAIL_OR_NULL(hr, pCompSvc);

    // get the project
    CComPtr<VCProject> pProject;
    hr = GetVCProject( &pProject );
	RETURN_ON_FAIL_OR_NULL(hr, pProject);

    // file to add resource to:
    // for VS7 beta 1:
    // 1) <project name>.rc
    // 2) first .rc file found
    // 3) create a new .rc file
    
    // find the default rc file (same name as project)
    // get its name, concat .rc extension to it
    CComBSTR bstrName;
    hr = pProject->get_Name( &bstrName );
	RETURN_ON_FAIL(hr);
    bstrName += L".rc";

    // iterate through each file, looking for the rc file
    CComPtr<IDispatch> pDisp;
    hr = pProject->get_Files( &pDisp );
    CComQIPtr<IVCCollection> pColl = pDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, pColl, E_NOINTERFACE);
    // look for the file
    long i, iMac;
    hr = pColl->get_Count( &iMac );
	RETURN_ON_FAIL(hr);
    CComBSTR bstrPath;
    CComPtr<IDispatch> pDispFile;

    hr = pColl->Item( CComVariant( bstrName ), &pDispFile );
    CComQIPtr<VCFile> pFile = pDispFile;
    if( pFile )
    {
        // get the full path to the file
        pFile->get_FullPath( &bstrPath );
    }
    
    // if we've found no exact file to add to, look for any .rc file
    if( bstrPath.Length() == 0 )
    {
        for( i = 1; i <= iMac; i++ )
        {
            CComPtr<IDispatch> pDispFile;
            hr = pColl->Item( CComVariant( i ), &pDispFile );
            if( FAILED( hr ) )
                continue;
            CComQIPtr<VCFile> pFile = pDispFile;
            if( pFile )
            {
                // get the file extension
                CComBSTR bstrExt;
                hr = pFile->get_Extension( &bstrExt );
                if( SUCCEEDED( hr ) && _wcsicmp( bstrExt, L".rc" ) == 0 )
                {
                    // get the full path to the file
                    pFile->get_FullPath( &bstrPath );
                    break;
                }
            }
        }
    }
    
    CComBSTR bstrTemplatePath;
    CComBSTR bstrHPath;
    CComPtr<IDispatch> pDispHFile;

    // if we've found no rc file to add to, create one
    // and create the corr. header file. Fix Bug: Add Resource 
    // does not work if rc file exists and resource.h does not exist
    // (happens when resource includes has been modified).
    if( bstrPath.Length() == 0 )
    {
        // first, will SCC allow us to add files to the project?
        hr = CanDirtyProject();
        RETURN_ON_FAIL(hr);

        // get the template dir
        CComPtr<_DTE> pDTE;
        hr = ExternalQueryService( SID_SDTE, IID__DTE, (void **)&pDTE );
        RETURN_ON_FAIL_OR_NULL2(hr, pDTE, E_NOINTERFACE);

        CComPtr<_Solution> pSln = NULL;
        hr = pDTE->get_Solution((Solution**)&pSln);
        RETURN_ON_FAIL_OR_NULL2(hr, pSln, E_NOINTERFACE);

        // this is the guid for the vc project system
        hr = pSln->ProjectItemsTemplatePath( L"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}", &bstrTemplatePath );
        RETURN_ON_FAIL(hr);

        CStringW strRCWiz = bstrTemplatePath;
        long pos = strRCWiz.ReverseFind( L'\\' );
        if( pos <= 0 )
            return E_FAIL;
        strRCWiz = strRCWiz.Left( pos );
        strRCWiz += L"\\VCContextItems\\addresource.vsz";
        
        CComBSTR bstrRCWiz = strRCWiz;

        CComVariant varPrj;
        hr = GetExtObject(varPrj);
        CComQIPtr<_Project> pPrj = varPrj.pdispVal;
        CComPtr<ProjectItems> spItems;

        if( pPrj )
        {
            pPrj->get_ProjectItems(&spItems);
        }
        else
        {
            CComQIPtr<_ProjectItem> pPrjItem = varPrj.pdispVal;
            if( pPrjItem )
            {
                pPrjItem->get_ProjectItems(&spItems);
            }
        }

        CComBSTR bstrPrjDir;
        pProject->get_ProjectDirectory( &bstrPrjDir );

        bstrPath = bstrPrjDir;
        bstrPath += bstrName;
        if (FAILED(hr = spItems->AddFromTemplate(bstrRCWiz, bstrPath, (ProjectItem**)&pDispFile)))
            return hr;
        if(!pDispFile)
            return hr;

    }

    // insert the resource
    // (the composition returned is not add-ref'd: don't use a smart ptr here)
    CComPtr <IVCWin32Composition> spComp;
    if (bstrPath)
        hr = pCompSvc->CreateCompositionFromRC( bstrPath, &spComp );
    else
        return E_FAIL;
    CComQIPtr<IOleCommandTarget> pCmdTarget = spComp;
    RETURN_ON_FAIL_OR_NULL2(hr, pCmdTarget, E_NOINTERFACE);
    hr = pCmdTarget->Exec( &guidVSStd2K, ECMD_RESOURCENEW, 0, NULL, NULL );

    return hr;
}

//-----------------------------------------------------------------------------
//	This method instructs the node to refresh its children. (which in our
//	case will lose all the children, and then set a flag to FALSE, indicating
//	that the next time someone asks for our children, we have to enumerate
//	them again).
//-----------------------------------------------------------------------------
HRESULT CVCBaseFolderNode::Refresh(CVCArchy *pHierarchy)
{
	DeleteAll(pHierarchy);
	m_grfStateFlags &= ~ST_ChildrenEnumerated;
	return S_OK;
}

VSITEMID CVCBaseFolderNode::GetFirstChild(void)
{
	return (NULL != GetHeadEx()) ? GetHead()->GetVsItemID() : VSITEMID_NIL;
}

CVCNode* CVCBaseFolderNode::GetHeadEx(void)
{
	if (!HaveChildrenBeenEvaluated())
	{
		HRESULT hr = EnumerateChildren();
		if (SUCCEEDED(hr))
			m_grfStateFlags |= ST_ChildrenEnumerated;
		else
			return NULL;
	}
	return GetHead();
}

//-----------------------------------------------------------------------------
// Loops over all this object's children, counting them and returns that count
//-----------------------------------------------------------------------------
long CVCBaseFolderNode::GetCount(void) const
{
	long n = 0;
	CVCNode *pNext = GetHead();

	while (pNext != NULL)
	{
		pNext = pNext->GetNext();
		++n;
	}
	return n;
}

//-----------------------------------------------------------------------------
// Returns the previous child, given the current child. (or NULL)
//-----------------------------------------------------------------------------
CVCNode* CVCBaseFolderNode::GetPrevChildOf(const CVCNode* pCurrent) const
{
	VSASSERT(m_pHeadNode != NULL, "No child list!");
	if (pCurrent == m_pHeadNode)
	{
		return NULL;
	}
	CVCNode* pNodePrev = m_pHeadNode;
	while (pNodePrev != NULL && pNodePrev->GetNext() != pCurrent)
	{
		pNodePrev = pNodePrev->GetNext();
	}
	return pNodePrev;
}

//-----------------------------------------------------------------------------
// Adds pNewNode after pCurrNode
//-----------------------------------------------------------------------------
void CVCBaseFolderNode::AddAfter(CVCNode* const pCurrNode, CVCNode* const pNewNode)
{
	VSASSERT(pNewNode != NULL, "Invalid input parameter!");

	if (pCurrNode)
	{
		pNewNode->SetNext(pCurrNode->GetNext());
		pCurrNode->SetNext(pNewNode);

		//NotifyHierarchyOfAdd(pCurrNode, pNewNode);
		if (HaveChildrenBeenEvaluated() && GetHierarchy())
			GetHierarchy()->OnItemAdded(this, pCurrNode, pNewNode);
	}
	else
		AddHead(pNewNode);
}

//-----------------------------------------------------------------------------
// Adds the given node to the head of this containers list.
//-----------------------------------------------------------------------------
void CVCBaseFolderNode::AddHead(CVCNode *const pNode)
{
	VSASSERT(pNode != NULL, "Invalid input parameter!");

	pNode->SetParent(this);
	pNode->SetNext(m_pHeadNode);

	m_pHeadNode = pNode;
	if (m_pTailNode == NULL)
		m_pTailNode = pNode;

	if( GetHierarchy() )
		GetHierarchy()->OnItemAdded(this, NULL, pNode);	// NOT an interface call...
}

//-----------------------------------------------------------------------------
//	Adds the given node to the tail of this containers list.
//-----------------------------------------------------------------------------
void CVCBaseFolderNode::AddTail(CVCNode *const pNode)
{
	VSASSERT(pNode != NULL, "Invalid input parameter!");

	pNode->SetParent(this);
	pNode->SetNext(NULL);

	CVCNode * pNodePrev = m_pTailNode;

	if (m_pTailNode != NULL)
	{
		VSASSERT(m_pHeadNode != NULL, "No head node to match tail node!");
		m_pTailNode->SetNext(pNode);
		m_pTailNode = pNode;
	}
	else
	{
		VSASSERT(m_pHeadNode == NULL, "Better not be a head node if no tail node...");
		m_pHeadNode = m_pTailNode = pNode;
	}

	if( GetHierarchy() )
		GetHierarchy()->OnItemAdded(this, pNodePrev, pNode);	// NOT an interface call...
}

//-----------------------------------------------------------------------------
//	Generic Add function calls AddTail. Override to get custom add behavior.
//-----------------------------------------------------------------------------
void CVCBaseFolderNode::Add(CVCNode *const pNode)
{
	VSASSERT(pNode != NULL, "Invalid input parameter!");
	AddTail(pNode);
}

//-----------------------------------------------------------------------------
//	Remove the given node from this container, does not delete the node
//	Return Values:
//		S_OK - success
//		E_FAIL - The node is not in the list of children (also _ASSERTEs)
//-----------------------------------------------------------------------------
HRESULT CVCBaseFolderNode::Remove(CVCNode *pNode)
{
	VSASSERT(pNode != NULL, "Invalid input parameter!");
	RETURN_ON_NULL(pNode);

	CVCNode *pNodeCur  = m_pHeadNode;	// The node to be removed
	CVCNode *pNodePrev = NULL;		// fix this node's next pointer

	while (pNode != pNodeCur && pNodeCur != NULL)
	{	// find pNode in list of children
		pNodePrev = pNodeCur;
		pNodeCur = pNodeCur->GetNext();
	}
	// TODO: investigate chance pNode will not be in this list
	VSASSERT(pNodeCur != NULL, "caller gave a node not in the list!");

	if (pNodeCur != NULL)
	{	// Then we found the node in the list. (this is a good thing!)
		if (pNodeCur == m_pHeadNode)
		{	// pNode is the HeadNode
			VSASSERT(pNode == m_pHeadNode, "Current node is head node, but our input parameter isn't?!?");
			m_pHeadNode = pNode->GetNext();
			if (m_pHeadNode == NULL)
			{	// single child case
				m_pTailNode = NULL;
			}
		}
		else if (pNodeCur == m_pTailNode)
		{	// We are removing the last node.
			m_pTailNode = pNodePrev;
			pNodePrev->SetNext(NULL);
		}
		else
		{	// We are just removing a node in the middle.
			pNodePrev->SetNext(pNode->GetNext());
		}

		pNode->SetParent(NULL);
		pNode->SetNext(NULL);
		return S_OK;
	}
	return E_FAIL;
}

//-----------------------------------------------------------------------------
//	This method removes a child from the container and deletes it.
//	Return Values:
//		S_OK - successfully removed & deleted node
//		E_FAIL - Remove from the list failed. It probably _ASSERTEed too.
//-----------------------------------------------------------------------------
HRESULT CVCBaseFolderNode::Delete(CVCNode *pNode, CVCArchy *pHierarchy)
{	// TODO: investigate chance pNode will not be in this list
	VSASSERT(pNode != NULL, "Invalid input parameter!");
	RETURN_ON_NULL(pNode);
	if (NULL != pHierarchy)
	{
		HRESULT hr = pHierarchy->OnItemDeleted(pNode);
		VSASSERT(SUCCEEDED(hr), "OnItemDeleted notification failed!");
	}
	HRESULT hr = Remove(pNode);
	if (SUCCEEDED(hr))
	{	// remove item from hierarchy (and its selection) before release
		pNode->Release();
	}
	return hr;
}

//-----------------------------------------------------------------------------
// This method removes all children from the container and destroys them.
//-----------------------------------------------------------------------------
void CVCBaseFolderNode::DeleteAll(CVCArchy *pHierarchy)
{
	while (GetHead() != NULL)
	{
		HRESULT hr = Delete(GetHead(), pHierarchy);
		VSASSERT(SUCCEEDED(hr), "Delete of an item failed!");
	}
}

HRESULT CVCBaseFolderNode::GetProperty(VSHPROPID propid, VARIANT *pvar)
{
	HRESULT hr = E_FAIL;
	CComVariant varRes;
	switch( propid )
	{
	case VSHPROPID_Expandable:
		V_VT(pvar) = VT_I4;
		V_I4(pvar) = TRUE;
		return S_OK;
	case VSHPROPID_FirstChild:
		V_VT(pvar) = VT_I4;
		V_INT_PTR(pvar) = (NULL != GetHeadEx()) ? GetHead()->GetVsItemID() : VSITEMID_NIL;
		return S_OK;
	case VSHPROPID_ExpandByDefault:
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = m_fAutoExpand ?  VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	default:
		hr = CVCNode::GetProperty(propid, &varRes);
		break;
	}
	varRes.Detach(pvar);
	return hr;
}

HRESULT CVCBaseFolderNode::SetProperty(VSHPROPID propid, const VARIANT & var)
{
	CComVariant vt = VARIANT_TRUE;

	if (VSHPROPID_ExpandByDefault == propid || VSHPROPID_Expanded == propid)
	{
		if (VSHPROPID_ExpandByDefault == propid)
			m_fAutoExpand = vt == var; 
		GetHierarchy()->OnPropertyChanged(this, propid, 0);	// NOT an interface call...
		return S_OK;
	}
	VSASSERT(0, _T("error: unknown property"));
	return E_UNEXPECTED;
}

//---------------------------------------------------------------------------
// Paste handler. Delegate to hierarchy.
//---------------------------------------------------------------------------
HRESULT CVCBaseFolderNode::OnPaste()
{ 
	if (GetHierarchy())
		return GetHierarchy()->PasteFromClipboard(this);	// NOT an interface call...
	else
		return S_FALSE;
}

//---------------------------------------------------------------------------
// Returns TRUE if OK to paste
//---------------------------------------------------------------------------
BOOL CVCBaseFolderNode::CanPaste()
{ 
	if (GetHierarchy())
		return GetHierarchy()->CanPasteFromClipboard();	// NOT an interface call...
	else
		return FALSE;
}
