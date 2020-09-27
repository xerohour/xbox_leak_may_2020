// VCFile.cpp: implementation of the CVCFileNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VCFile.h"
#include "BldPkg.h"
#include "PrjNode.h"
#include "utils.h"
#include "..\resdll\gpcmd.h"
#include "vcarchy.h"
#include "vstrkdoc.h"
#include <vcguid.h>
#include "filegencfg.h"
#include "vcfgrp.h"
#include "VCPropObject.h"

#include "AutoFile.h"
#include "context.h"

#ifdef AARDVARK
#include <vsmanaged.h>
const GUID IID_ICSharpProject = {0xfae04ec0, 0x301f, 0x11d3, {0xbf, 0x4b, 0x0, 0xc0, 0x4f, 0x79, 0xef, 0xbc} };
#endif // AARDVARK

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// static member initialization

const LPOLESTR CVCFileNode::s_wszCATID = L"{EE8299C9-19B6-4f20-ABEA-E1FD9A33B683}";
extern "C" const GUID GUID_TextEditorFactory = { 0x8b382828L,0x6202,0x11d1,0x88,0x70,0x0, 0x0, 0xf8,0x75,0x79,0xd2 };


CVCFileNode::CVCFileNode() :
#ifdef AARDVARK
	m_eMagic( magicNotSet ),
	m_bstrDeployLoc( L"" ),
#endif // AARDVARK
	m_fDeleted( FALSE )
{
}

CVCFileNode::~CVCFileNode()
{
}

CVCProjectNode* CVCFileNode::GetVCProjectNode()
{
	return (CVCProjectNode*)GetRootNode();
}

UINT CVCFileNode::GetIconIndex(ICON_TYPE iconType)
{
 	UINT index = STATEICON_NOSTATEICON;
	CVCArchy *pArchy = NULL;

#ifdef AARDVARK
	if( iconType == ICON_StateImage )
		return STATEICON_NONE;

	// handle stateimages (SCC)
	else if( iconType == ICON_StateImage )
	{
		pArchy = (CVCArchy *)GetHierarchy();
		if (pArchy)
		{
			CComBSTR bstrPath;
			LPOLESTR pszFullPaths;
			VSSCCSTATUS dwSccStatus;
			
			bstrPath.Attach( GetFullPath() );
			pszFullPaths = bstrPath;
			index = STATEICON_NOSTATEICON;

			CComPtr<IVsSccManager2> srpIVsSccManager2;
			if (GetBuildPkg() && GetBuildPkg()->GetSccManager(&srpIVsSccManager2))
			{
				VsStateIcon siGlyph = STATEICON_NOSTATEICON;
				srpIVsSccManager2->GetSccGlyph(1, &pszFullPaths, &siGlyph, &dwSccStatus);
				index = siGlyph;
			}
		}
	}
	else
	{
		CVCNode *pParent = GetParent();
		UINT kind = pParent->GetKindOf();
		if( kind == Type_CVCFileGroup )
		{
			CComBSTR bstrParentName;
			static_cast<CVCFileGroup*>(pParent)->GetName( &bstrParentName );
			// if we're in the References folder, we're a ref
			if( wcscmp( bstrParentName, L"References" ) == 0 )
				return BMP_REFERENCE;
			// if we're in the Satellite DLLs folder, we're a satellite dll
			if( wcscmp( bstrParentName, L"Satellite DLLs" ) == 0 )
				return BMP_SATELLITE;
			// if we're in the "Built" folder, we're a satellite dll
			if( wcscmp( bstrParentName, L"Built" ) == 0 )
				return BMP_OUTPUTS;
		}

		// get the file 
		CComPtr<VCFile> pVCFile;
		GetVCFile( &pVCFile );
		// get the extension
		CComBSTR bstrExt;
		pVCFile->get_Extension( &bstrExt );

		// C++ file?
		if( _wcsicmp( bstrExt, L".cpp" ) == 0 || _wcsicmp( bstrExt, L".h" ) == 0 )
		{
			if( IsBuildable() )
				index = BMP_BUILDABLEITEM;
			else
				index = BMP_NONBUILDABLEITEM;
		}
		// C# file?
		else if( _wcsicmp( bstrExt, L".cs" ) == 0 )
		{
			// if the magic enum wasn't set, set it
			if( m_eMagic == magicNotSet )
			{
				CComBSTR bstrName;
				GetName( &bstrName );
				// win form?
				if( wcsncmp( bstrName, L"Form", wcslen( L"Form" ) ) == 0 )
					m_eMagic = magicWinForm;
				// user control?
				else if( wcsncmp( bstrName, L"UserControl", wcslen( L"UserControl" ) ) == 0 )
					m_eMagic = magicUserControl;
				// class?
				else if( wcsncmp( bstrName, L"Class", wcslen( L"Class" ) ) == 0 )
					m_eMagic = magicClass;
				// web custom ctrl?
				else if( wcsncmp( bstrName, L"WebCustomControl", wcslen( L"WebCustomControl" ) ) == 0 )
					m_eMagic = magicWebCustomControl;
				// normal source
				else
					m_eMagic = magicNone;
			}
			switch( m_eMagic )
			{
			case magicWinForm:
				index = BMP_WINFORM;
				break;
			case magicUserControl:
				index = BMP_CONTROL;
				break;
			case magicClass:
				index = BMP_CLASS;
				break;
			case magicWebCustomControl:
				index = BMP_WEBCUSTOMCONTROL;
				break;
			default:
				index = BMP_CSFILE;
			}
		}
		// vb file?
		else if( _wcsicmp( bstrExt, L".vb" ) == 0 )
		{
			// if the magic enum wasn't set, set it
			if( m_eMagic == magicNotSet )
			{
				CComBSTR bstrName;
				GetName( &bstrName );
				// win form?
				if( wcsncmp( bstrName, L"Form", wcslen( L"Form" ) ) == 0 )
					m_eMagic = magicWinForm;
				// user control?
				else if( wcsncmp( bstrName, L"UserControl", wcslen( L"UserControl" ) ) == 0 )
					m_eMagic = magicUserControl;
				// class?
				else if( wcsncmp( bstrName, L"Class", wcslen( L"Class" ) ) == 0 )
					m_eMagic = magicClass;
				// web custom ctrl?
				else if( wcsncmp( bstrName, L"WebCustomControl", wcslen( L"WebCustomControl" ) ) == 0 )
					m_eMagic = magicWebCustomControl;
				// normal source
				else
					m_eMagic = magicNone;
			}
			switch( m_eMagic )
			{
			case magicWinForm:
				index = BMP_WINFORM;
				break;
			case magicUserControl:
				index = BMP_CONTROL;
				break;
			case magicClass:
				index = BMP_CLASS;
				break;
			case magicWebCustomControl:
				index = BMP_WEBCUSTOMCONTROL;
				break;
			default:
				index = BMP_VBFILE;
			}
		}
		// xsd file?
		else if( _wcsicmp( bstrExt, L".xsd" ) == 0 )
			index = BMP_XSD;
		// xml file?
		else if( _wcsicmp( bstrExt, L".xml" ) == 0 )
			index = BMP_XML;
		// web form file?
		else if( _wcsicmp( bstrExt, L".aspx" ) == 0 )
			index = BMP_WEBFORM;
		// asax file?
		else if( _wcsicmp( bstrExt, L".asax" ) == 0 )
			index = BMP_GLOBALASAX;
		// web service file?
		else if( _wcsicmp( bstrExt, L".asmx" ) == 0 )
			index = BMP_WEBSERVICE;
		// web user control file?
		else if( _wcsicmp( bstrExt, L".ascx" ) == 0 )
			index = BMP_WEBCONTROL;
		// asp file?
		else if( _wcsicmp( bstrExt, L".asp" ) == 0 )
			index = BMP_ASPPAGE;
		// web.config file?
		else if( _wcsicmp( bstrExt, L".config" ) == 0 )
			index = BMP_WEBCONFIG;
		// html file?
		else if( _wcsicmp( bstrExt, L".html" ) == 0 || _wcsicmp( bstrExt, L".htm" ) == 0 )
			index = BMP_HTML;
		// css file?
		else if( _wcsicmp( bstrExt, L".css" ) == 0 )
			index = BMP_CSS;
		// bmp file?
		else if( _wcsicmp( bstrExt, L".bmp" ) == 0 )
			index = BMP_BMP;
		// ico file?
		else if( _wcsicmp( bstrExt, L".ico" ) == 0 )
			index = BMP_ICO;
		// resx file?
		else if( _wcsicmp( bstrExt, L".resx" ) == 0 )
			index = BMP_RESX;
		// script file?
		else if( _wcsicmp( bstrExt, L".vbs" ) == 0 || _wcsicmp( bstrExt, L".js" ) == 0 )
			index = BMP_SCRIPT;
		// vcstyle file?
		// TODO: need vcstyle icon
		else if( _wcsicmp( bstrExt, L".vcstyle" ) == 0 )
			index = BMP_CSS;
		// unknown file type
		else
			index = BMP_NONBUILDABLEITEM;
	}
	return index;
#else
	if( iconType == ICON_Open )
	{
		index = BMP_FOLDER;
	}
	// handle stateimages (SCC)
	else if( iconType == ICON_StateImage )
	{
		pArchy = (CVCArchy *)GetHierarchy();
		if (pArchy)
		{
			index = STATEICON_NOSTATEICON;
			if (GetServiceProvider() != NULL)
			{
				CComPtr<VCFile> spFile;
				GetVCFile(&spFile);
				VARIANT_BOOL bScc = VARIANT_TRUE;
				if (spFile)
				{
					CComPtr<IDispatch> spDispParent;
					spFile->get_Parent(&spDispParent);
					CComQIPtr<VCFilter> spFilterParent = spDispParent;
					if (spFilterParent)
						spFilterParent->get_SourceControlFiles(&bScc);
				}
				if (bScc)
				{
					CComBSTR bstrPath;
					LPOLESTR pszFullPaths;
					VSSCCSTATUS dwSccStatus;
					
					bstrPath.Attach( GetFullPath() );
					pszFullPaths = bstrPath;

					CComPtr<IVsSccManager2> srpIVsSccManager2;
					if (GetBuildPkg() && GetBuildPkg()->GetSccManager(&srpIVsSccManager2))
					{
						VsStateIcon siGlyph = STATEICON_NOSTATEICON;
						srpIVsSccManager2->GetSccGlyph(1, &pszFullPaths, &siGlyph, &dwSccStatus);
						index = siGlyph;
					}
				}
			}
		}
	}
	else
	{
		if( IsBuildable() )
			index = BMP_BUILDABLEITEM;
		else
			index = BMP_NONBUILDABLEITEM;
	}
	return index;
#endif // AARDVARK
}

HRESULT CVCFileNode::DoPreview()
{
	CComBSTR bstrPath;
	bstrPath = GetFullPath();
	
	CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;	
	HRESULT hr = ExternalQueryService(SID_SVsUIShellOpenDocument,
							  IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc);
	VSASSERT(SUCCEEDED(hr), "Unable to obtain IVsUIShellOpenDocument service!  Did the ID change?  Do you need to investigate threading issues?");
	RETURN_ON_FAIL_OR_NULL(hr, pVsUIShellOpenDoc);
						  
	DWORD dwReserved = 0;
	DWORD dwPrevFlags = 0;
	return pVsUIShellOpenDoc->OpenStandardPreviewer(dwPrevFlags, bstrPath, PR_Default, dwReserved);
}

HRESULT CVCFileNode::DoDefaultAction(BOOL fNewFile)
{
	HRESULT hr = S_OK;

#ifdef AARDVARK
	BOOL bHandled = FALSE;
	hr = HandleOpenForm(bHandled);
	if (bHandled)
		return hr;
#endif	// AARDVARK

	CComBSTR bstrPath;
	bstrPath = GetFullPath();

	// special case: RC files should cause the Resource View to open, not the
	// standard editor (which would open the "comp view" stand-alone resource editor)
	CPathW path;
	path.Create( bstrPath );
	const wchar_t* wszExt = path.GetExtension();
	if( _wcsicmp( wszExt, L".rc" ) == 0 )
		return OpenResourceEditor(bstrPath);

	else if( _wcsicmp( wszExt, L".bsc" ) == 0 )
		return OpenBscEditor(bstrPath);

	else if( _wcsicmp( wszExt, L".asmx" ) == 0 || _wcsicmp( wszExt, L".asax" ) == 0 )
		return OpenAsmxEditor(bstrPath);

	else
	{
		return OpenNormalEditor(bstrPath, OSE_ChooseBestStdEditor, GUID_NULL, fNewFile);
	}
}

#ifdef AARDVARK
HRESULT CVCFileNode::HandleOpenForm(BOOL& bHandled)
{
	bHandled = FALSE;
	if( m_eMagic == magicWinForm || m_eMagic == magicUserControl || m_eMagic == magicWebCustomControl )
	{
		bHandled = TRUE;
		// magic...
		CComBSTR bstrFilename;
		GetName( &bstrFilename );
		// look for a C# project in the solution
		CComPtr<IVsSolution> pVsSolution;
		HRESULT hr = ExternalQueryService(__uuidof(IVsSolution), __uuidof(IVsSolution), (void **)&pVsSolution);
		// get the hierarchies enumeration
		CComPtr<IEnumHierarchies> pHierEnum;

		hr = pVsSolution->GetProjectEnum( EPF_ALL, IID_ICSharpProject, &pHierEnum );
		if( FAILED( hr ) || !pHierEnum )
			return hr;
		// for each hierarchy
		CComBSTR bstrProjUniqueName;
		pHierEnum->Reset();
		// find the "WindowsApplication1" project
		while ( true )
		{
			CComQIPtr<IVsHierarchy> pHierarchy;
			hr = pHierEnum->Next( 1, &pHierarchy, NULL );
			if( hr != S_OK || !pHierarchy )
				break;
			// GetProp its VSHPROPID_ExtObject to get the shell project
			CComVariant var;
			CComQIPtr<Project> pProj;
			hr = pHierarchy->GetProperty( VSITEMID_ROOT, VSHPROPID_ExtObject, &var );
			pProj = var.pdispVal;
			if (pProj == NULL)
				return S_OK;
			// is this the right project?
			CComBSTR bstrName;
			hr = pProj->get_Name( &bstrName );
			if( FAILED( hr ) ) return hr;
			if( _wcsicmp( bstrName, L"WindowsApplication1" ) != 0 )
				continue;
			// get the project item of this filename
			CComPtr<ProjectItems> pProjectItems;
			hr = pProj->get_ProjectItems( &pProjectItems );
			if( FAILED( hr ) ) return hr;
			CComPtr<ProjectItem> pProjectItem;
			hr = pProjectItems->Item( CComVariant( bstrFilename ), &pProjectItem );
			if( FAILED( hr ) ) return hr;
			CComPtr<Window> pEditWnd;
			hr = pProjectItem->Open( CComBSTR( L"{7651A702-06E5-11D1-8EBD-00A0C90F26EA}" ), &pEditWnd );
			if( FAILED( hr ) ) return hr;
			// open filename in that project instead...
			CComPtr<_DTE> pDTE;
			hr = ExternalQueryService(SID_SDTE, __uuidof(_DTE), (void **)&pDTE);
			if( FAILED( hr ) ) return hr;
			CComPtr<Windows> pWindows;
			hr = pDTE->get_Windows( &pWindows );
			if( FAILED( hr ) ) return hr;
			CComPtr<Window> pSlnExplorer;
			hr = pWindows->Item( CComVariant( vsWindowKindSolutionExplorer ), &pSlnExplorer );
			if( FAILED( hr ) ) return hr;
// 			// hide sln explorer
// 			hr = pSlnExplorer->put_Visible( VARIANT_FALSE );
// 			if( FAILED( hr ) ) return hr;
			// show designer
			hr = pEditWnd->put_Visible( VARIANT_TRUE );
			if( FAILED( hr ) ) return hr;
			break;
		}
		return S_OK;
	}
}
#endif	// AARDVARK

HRESULT CVCFileNode::OpenResourceEditor(CComBSTR& bstrPath)
{
	if (GetBuildPkg() && GetBuildPkg()->UseGlobalEditorForResourceFiles())
		return OpenNormalEditor(bstrPath, OSE_ChooseBestStdEditor, GUID_NULL, FALSE);
	
	// this is an rc file, open resource view
	CComPtr<IOleCommandTarget> pCmdTgt;
	HRESULT hr = ExternalQueryService( SID_SUIHostCommandDispatcher, IID_IOleCommandTarget, (void**)&pCmdTgt );
	RETURN_ON_FAIL_OR_NULL(hr, pCmdTgt);
	hr = pCmdTgt->Exec( &CLSID_StandardCommandSet97, cmdidResourceView, OLECMDEXECOPT_DODEFAULT, NULL, NULL );
	RETURN_ON_FAIL(hr);

	// Go get the ui hierarchy window
	CComPtr<IVsUIShell> pIVsUIShell;
	hr = GetBuildPkg()->GetVsUIShell(&pIVsUIShell, TRUE /* in main thread */);
	RETURN_ON_FAIL_OR_NULL(hr, pIVsUIShell);

	CComPtr<IVsWindowFrame> pIVsWindowFrame;
	hr = pIVsUIShell->FindToolWindow( NULL, GUID_ResourceView, &pIVsWindowFrame);
	RETURN_ON_FAIL_OR_NULL(hr, pIVsWindowFrame);

	CComVariant var;
	hr = pIVsWindowFrame->GetProperty( VSFPROPID_DocView, &var );
	RETURN_ON_FAIL(hr);
	if( var.vt != VT_UNKNOWN )
		return E_FAIL;

	CComPtr<IVsWindowPane> pIVsWindowPane;
	hr = var.punkVal->QueryInterface( IID_IVsWindowPane, (void **)&pIVsWindowPane );
	VSASSERT( pIVsWindowPane, "Unable to get IVsWindowPane pointer!" );
	RETURN_ON_FAIL_OR_NULL(hr, pIVsWindowPane);

	CVCProjectNode* pProjNode = GetVCProjectNode();
	RETURN_ON_NULL(pProjNode);

	// Save the file
	CComPtr<IVsHierarchy> spHier;
	hr = pProjNode->GetVsHierarchy(&spHier, TRUE);
	RETURN_ON_FAIL_OR_NULL(hr, spHier);

	CComVariant varName;
	spHier->GetProperty(VSITEMID_ROOT, VSHPROPID_Name, &varName);

	// expand to the correct node
	CComPtr<IVsClassView> pClassView;
	hr = pIVsWindowPane->QueryInterface( IID_IVsClassView, (void **)&pClassView );
	VSASSERT( pClassView, "Unable to get IVsClassView pointer!" );
	RETURN_ON_FAIL_OR_NULL(hr, pClassView);
	CComBSTR bstrFileName;
	get_Name( &bstrFileName );
	// don't ask me where the $#@%^ this guid is defined, I can't find it
	// anywhere, but this IS the correct guid
	// guidResviewLibrary = {D22514E7-23AF-4723-B6E5-E17D27626D34}
	CLSID Guid;
	CLSIDFromString( L"{D22514E7-23AF-4723-B6E5-E17D27626D34}", &Guid );
	wchar_t *wszLibName = varName.bstrVal;
	wchar_t *wszNspcName = bstrFileName;
	wchar_t *wszClassName = L"";
	wchar_t *wszMemberName = L"";
	VSOBJECTINFO objInfo;
	objInfo.pguidLib = &Guid;
	objInfo.pszLibName = wszLibName;
	objInfo.pszNspcName = wszNspcName;
	objInfo.pszClassName = wszClassName;
	objInfo.pszMemberName = wszMemberName;
	objInfo.dwCustom      = 0;
	pClassView->NavigateTo( &objInfo, 9);

	return hr;
}

HRESULT CVCFileNode::OpenBscEditor(CComBSTR& bstrPath)
{
	CComPtr<IVsObjBrowser> spOB;
	HRESULT hr = ExternalQueryService( SID_SVsObjBrowser, IID_IVsObjBrowser, (void**)&spOB );
	RETURN_ON_FAIL_OR_NULL(hr, spOB);

	// Navigate to the BSC file
	CComBSTR bstrFileName;
	get_FullPath( &bstrFileName );
	CLSID Guid;
	CLSIDFromString( L"{26E73A17-0D6C-4a33-B833-22C76C50949F}", &Guid );
	wchar_t *wszLibName = bstrFileName;
	wchar_t *wszNspcName = L"";
	wchar_t *wszClassName = L"";
	wchar_t *wszMemberName = L"";
	VSOBJECTINFO objInfo;
	objInfo.pguidLib = &Guid;
	objInfo.pszLibName = wszLibName;
	objInfo.pszNspcName = wszNspcName;
	objInfo.pszClassName = wszClassName;
	objInfo.pszMemberName = wszMemberName;
	objInfo.dwCustom      = 0;
	spOB->NavigateTo( &objInfo, 0);

	return hr;
}

HRESULT CVCFileNode::OpenAsmxEditor(CComBSTR& bstrPath)
{
	CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;  
	HRESULT hr = ExternalQueryService(SID_SVsUIShellOpenDocument, 
							  IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc);
	VSASSERT(SUCCEEDED(hr), "Unable to obtain IVsUIShellOpenDocument service!  Did the ID change?  Do you need to investigate threading issues?");
	RETURN_ON_FAIL_OR_NULL(hr, pVsUIShellOpenDoc);
                      
	CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(GetHierarchy());
	CComPtr<IVsWindowFrame> pFrame;
	hr = pVsUIShellOpenDoc->OpenSpecificEditor(NULL,
											bstrPath,
											GUID_TextEditorFactory,
											NULL,
											GUID_NULL,
											L"%3", // base filename
											spHier,
											(VSITEMID)GetVsItemID(),
											DOCDATAEXISTING_UNKNOWN,
											GetServiceProvider(),
											&pFrame);

	if (FAILED(hr))
	{
		CComPtr<IVsUIShell> pUIShell;
		HRESULT hrT;

		hrT = GetBuildPkg()->GetVsUIShell(&pUIShell, TRUE /* in main thread */);
		if (SUCCEEDED(hrT) && pUIShell)
		{
			CStringW strMsg;
			CComBSTR bstrMsg = bstrPath;
			bstrMsg.Append(L"\n");
			strMsg.LoadString( g_hInstLocale, IDS_ERROR_OPEN);
			bstrMsg.Append(strMsg);
			pUIShell->SetErrorInfo(E_FAIL, bstrMsg, 0, NULL, NULL);
		}
	}
	if(hr == S_OK && pFrame)
	{
		pFrame->Show();
	}

	return hr;
}

HRESULT CVCFileNode::OpenNormalEditor(CComBSTR& bstrPath, VSOSEFLAGS grfOpenStandard, REFGUID rguidLogicalView, BOOL fNewFile)
{
	CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;  
	HRESULT hr = ExternalQueryService(SID_SVsUIShellOpenDocument, 
							  IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc);
	VSASSERT(SUCCEEDED(hr), "Unable to obtain IVsUIShellOpenDocument service!  Did the ID change?  Do you need to investigate threading issues?");
	RETURN_ON_FAIL_OR_NULL(hr, pVsUIShellOpenDoc);
                      
	CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(GetHierarchy());
	CComPtr<IVsWindowFrame> pFrame;

	if(fNewFile)
		grfOpenStandard |= OSE_OpenAsNewFile;
	
	hr = pVsUIShellOpenDoc->OpenStandardEditor(grfOpenStandard, bstrPath, rguidLogicalView, L"%3" /* base filename */, 
		spHier, (VSITEMID)GetVsItemID(), DOCDATAEXISTING_UNKNOWN, GetServiceProvider(), &pFrame);
	RETURN_ON_FAIL(hr);

	if (FAILED(hr))
	{
		CComPtr<IVsUIShell> pUIShell;
		HRESULT hrT;

		hrT = GetBuildPkg()->GetVsUIShell(&pUIShell, TRUE /* in main thread */);
		if (SUCCEEDED(hrT) && pUIShell)
		{
			CStringW strMsg;
			CComBSTR bstrMsg = bstrPath;
			bstrMsg.Append(L"\n");
			strMsg.LoadString( g_hInstLocale, IDS_ERROR_OPEN);
			bstrMsg.Append(strMsg);
			pUIShell->SetErrorInfo(E_FAIL, bstrMsg, 0, NULL, NULL);
		}
	}
	if(hr == S_OK && pFrame)
	{
		if(fNewFile)
		{
			CComVariant var;
			pFrame->GetProperty(VSFPROPID_DocData, &var);
			CComQIPtr<IVsPersistDocData> pDocData;
			if(V_VT(&var) == VT_UNKNOWN )
				pDocData = V_UNKNOWN(&var);
			if(pDocData != NULL)
				pDocData->SetUntitledDocPath(bstrPath);
		}
		pFrame->Show();
	}

	return hr;
}

HRESULT CVCFileNode::DisplayContextMenu(VARIANT *pvaIn)
{
	CComPtr<IOleCommandTarget> spTarget = VCQI_cast<IOleCommandTarget>(GetHierarchy())/*(this)*/;
	return ShowContextMenu(IDM_VS_CTXT_ITEMNODE, pvaIn, guidSHLMainMenu, spTarget);
}

HRESULT CVCFileNode::GetCanonicalName(BSTR* pbstrName)
{
	CComPtr<VCFile> spFile;
	GetVCFile(&spFile);
	CHECK_ZOMBIE(spFile, IDS_ERR_FILE_ZOMBIE);

	return spFile->get_FullPath(pbstrName);
}

HRESULT CVCFileNode::GetName(BSTR* pbstrName)
{
	CComPtr<VCFile> spFile;
	GetVCFile(&spFile);
	CHECK_ZOMBIE(spFile, IDS_ERR_FILE_ZOMBIE);

	return spFile->get_Name(pbstrName);
}

//-----------------------------------------------------------------------------
//	Called to update the status for each command
//-----------------------------------------------------------------------------
STDMETHODIMP CVCFileNode::QueryStatus(
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[],
		OLECMDTEXT *pCmdText)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	CVCNode::enumKnownGuidList eGuid = CVCNode::eGuidUninitialized;
	if (*pguidCmdGroup == guidVSStd2K)
		eGuid = eGuidVSStd2K;
	else	// let the base class handle it
		return CVCNode::QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);

	HRESULT hr = NOERROR;
	for (ULONG i = 0; i < cCmds && SUCCEEDED(hr); i++)
		hr = QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, i);

	return hr;
}

HRESULT CVCFileNode::QueryStatusAtIndex(CVCNode::enumKnownGuidList eGuid, const GUID* pguidCmdGroup, OLECMD prgCmds[],
	OLECMDTEXT *pCmdText, ULONG nIndex)
{
	if (eGuid == CVCNode::eGuidUninitialized)
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
		case ECMD_COMPILE:
			if( IsBuildable() )
				fEnable = eEnableSupportedAndEnabled;
			else
				fEnable = eEnableSupported;
			break;
		case ECMD_UPDATEWEBREFERENCE:
			if( IsWSDL() )
				fEnable = eEnableSupportedAndEnabled;
			else
				fEnable = eEnableNotSupported;
			break;
		default:
			return CVCNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);
		}
	}
	else if (eGuid == CVCNode::eGuidVSStd97)
	{	
		// Common VS commands
		switch(prgCmds[nIndex].cmdID)
		{
		case cmdidOpen:
		case cmdidOpenWith:
		case cmdidCut:
		case cmdidCopy:
			fEnable = eEnableSupportedAndEnabled; // && GetProjectNode()->IsSourceControlled() && !IsInSourceControl();
			break;
		case cmdidPaste:
			return GetParent()->QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);
		case cmdidPreviewInBrowser:
			if( IsHTML() )
				fEnable = eEnableSupportedAndEnabled;
			else
				fEnable = eEnableNotSupported;
			break;
		case cmdidRename:
			fEnable = eEnableSupportedAndInvisible;
			break;
		default:
			return CVCNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);
		}
	}

	prgCmds[nIndex].cmdf = TranslateEnableState(fEnable);
	if (prgCmds[nIndex].cmdf == 0)
		return OLECMDERR_E_NOTSUPPORTED;

	return NOERROR;
}

//-----------------------------------------------------------------------------
// Called to execute our commands
//-----------------------------------------------------------------------------
STDMETHODIMP CVCFileNode::Exec(
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	if(*pguidCmdGroup == guidVSStd2K)
	{
		switch(nCmdID)
		{
			case ECMD_COMPILE:
				return OnCompileFile();
			case ECMD_PROJSETTINGS:
				return ShowSettings(CLSID_FileNodeProps);
			case ECMD_UPDATEWEBREFERENCE:
				return OnRefreshWebRef();
			default:
				return CVCNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut); // OLECMDERR_E_NOTSUPPORTED;
		}
	}
	else if(*pguidCmdGroup == guidVSStd97)
	{	
		switch(nCmdID)
		{
			case cmdidPreviewInBrowser:
				DoPreview();
				return S_OK;
			case cmdidOpen:
				return DoDefaultAction(FALSE); // Open File;
			case cmdidOpenWith:
				return OpenWith(); // Open File;
			case cmdidPaste:
				return static_cast<CVCBaseFolderNode*>(GetParent())->OnPaste();

			default:
				return CVCNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut); // OLECMDERR_E_NOTSUPPORTED;
		}
	}

	// Unknown command set
	return OLECMDERR_E_NOTSUPPORTED;
}

HRESULT CVCFileNode::SetProperty(VSHPROPID propid, const VARIANT& var)
{
	// handle file node properties here...
	return CVCNode::SetProperty(propid, var);
}

HRESULT CVCFileNode::GetGuidProperty( VSHPROPID propid, GUID *pguid)
{
	CHECK_POINTER_NULL(pguid);

	*pguid = GUID_ItemType_PhysicalFile;
	return S_OK;
}

//---------------------------------------------------------------------------
//	Returns the requested property in the given variant.
//---------------------------------------------------------------------------
HRESULT CVCFileNode::GetProperty(VSHPROPID propid, VARIANT *pvar)
{
	CHECK_POINTER_NULL(pvar);
	// handle file node properties here...
	HRESULT hr = E_FAIL;
	CComVariant varRes;
	switch( propid )
	{
	case VSHPROPID_Name:
	case VSHPROPID_SaveName:
	case VSHPROPID_Caption:
	{	// return this node's displayed caption
		V_VT(pvar) = VT_BSTR;
		CComBSTR bstrName;
		hr = GetName(&bstrName);
		if (SUCCEEDED(hr))
			pvar->bstrVal = bstrName.Detach();
		return hr;
	}
	case VSHPROPID_UserContext:
	{
		varRes.ChangeType(VT_UNKNOWN);
		varRes.punkVal = NULL;

		CComPtr<IVsUserContext> pUserCtx;
		CComPtr<IVsMonitorUserContext> pmuc;
		if (SUCCEEDED(ExternalQueryService(SID_SVsMonitorUserContext, IID_IVsMonitorUserContext, (void **)&pmuc)) && pmuc)
		{
			pmuc->CreateEmptyContext(&pUserCtx);
			if (pUserCtx)
			{
				CComQIPtr<VCFile> pFile = m_dispkeyItem;
				RETURN_ON_NULL(pFile);
				CComBSTR bstrName;
				pFile->get_Name( &bstrName );
				CPathW path;
				path.Create( bstrName );
				CStringW strExt = path.GetExtension();
				if( strExt.GetLength()  > 0 )
				{
					strExt = strExt.Right( strExt.GetLength()-1 );
					pUserCtx->AddAttribute(VSUC_Usage_Filter, L"item", strExt);
				}
				varRes = pUserCtx;
				hr = S_OK;
			}
		}
		break;
	}
	case VCPROPID_ProjBldActiveFileConfig:
	{
		hr = GetActiveVCFileConfig(&V_DISPATCH(pvar));
		if (SUCCEEDED(hr))
		{
			V_VT(pvar) = VT_DISPATCH;
			return S_OK;
		}
		break;
	}

	case VCPROPID_ProjBldIncludePath:
	{
		hr = GetVCIncludePath(&V_BSTR(pvar));
		if (hr == S_OK)
		{
			V_VT(pvar) = VT_BSTR;
			return S_OK;
		}
		break;
	}

	case VSHPROPID_ConfigurationProvider:
	{
		varRes.ChangeType(VT_UNKNOWN);
		IUnknown *pUnk = NULL;
		hr = QueryInterface(IID_IUnknown, (void **)&pUnk);
		varRes = pUnk;
		pUnk->Release();
		break;
	}

	default:
		hr = CVCNode::GetProperty(propid, &varRes);
		break;
	}
	varRes.Detach(pvar);
	return hr;
}

HRESULT CVCFileNode::GetActiveVCFileConfig(IDispatch** ppDispFileCfg)
{
	CHECK_POINTER_NULL(ppDispFileCfg);

	CComQIPtr<IVCFileImpl> spFileImpl = m_dispkeyItem;
	RETURN_ON_NULL(spFileImpl);

	CComPtr<IDispatch> spDispProjCfg;
	HRESULT hr = GetActiveVCProjectConfig(&spDispProjCfg);
	CComQIPtr<VCConfiguration> spProjCfg = spDispProjCfg;
	RETURN_ON_FAIL_OR_NULL(hr, spProjCfg);

	CComPtr<VCFileConfiguration> spFileCfg;
	hr = spFileImpl->GetFileConfigurationForProjectConfiguration(spProjCfg, &spFileCfg);
	RETURN_ON_FAIL_OR_NULL(hr, spFileCfg);

	return spFileCfg->QueryInterface(IID_IDispatch, (void **)ppDispFileCfg);
}

HRESULT CVCFileNode::GetVCIncludePath(BSTR* pbstrIncPath)
{
	CHECK_POINTER_NULL(pbstrIncPath);
	*pbstrIncPath = NULL;

	CComPtr<IDispatch> spDispFileCfg;
	HRESULT hr = GetActiveVCFileConfig(&spDispFileCfg);
	RETURN_ON_FAIL(hr);

	CComQIPtr<VCFileConfiguration> spFileCfg = spDispFileCfg;
	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spDispFileCfg;
	if (spFileCfg == NULL || spFileCfgImpl == NULL)
		return hr;

	// first, let's try to pick it up via the tool
	CComPtr<IDispatch> spDispTool;
	hr = spFileCfg->get_Tool(&spDispTool);
	CComQIPtr<VCCustomBuildTool> pCustom = spDispTool;
	CComQIPtr<IVCToolImpl> spToolImpl = spDispTool;
	if( SUCCEEDED(hr) && spToolImpl && (spDispTool == NULL) )
		return spToolImpl->get_FullIncludePathInternal(pbstrIncPath);

	// oh, well.  At least we can fall back on the project level stuff for the C++ tool.
	CComPtr<IDispatch> spDispProjCfg;
	hr = spFileCfgImpl->get_Configuration(&spDispProjCfg);
	CComQIPtr<VCConfiguration> spProjCfg = spDispProjCfg;
	RETURN_ON_FAIL_OR_NULL(hr, spProjCfg);

	CComPtr<IDispatch> pDispColl;
	spProjCfg->get_Tools(&pDispColl);
	CComQIPtr<IVCCollection> pToolsColl = pDispColl;
	if( pToolsColl )
	{
		CComBSTR bstrName( L"VCCLCompilerTool");		// mutli platform ???
		CComPtr<IDispatch> pDispTool;
		hr = pToolsColl->Item( CComVariant( bstrName ), &pDispTool );
		CComQIPtr<IVCToolImpl> spToolImpl = pDispTool;
		if (SUCCEEDED(hr) && spToolImpl)
			return spToolImpl->get_FullIncludePathInternal(pbstrIncPath);
	}

	// oh, well.  At least we can fall back on the platform level stuff.
	// start out by picking up the platform includes
	CComPtr<IDispatch> spDispPlatform;
	hr = spProjCfg->get_Platform(&spDispPlatform);
	CComQIPtr<VCPlatform> spPlatform = spDispPlatform;
	RETURN_ON_FAIL(hr);

	CComQIPtr<IVCPropertyContainer> spPropContainer = spDispFileCfg;
	CComBSTR bstrPlatformIncs;
	if (spPlatform)
	{
		hr = spPlatform->get_IncludeDirectories(&bstrPlatformIncs);
		if (SUCCEEDED(hr) && spPropContainer)
		{
			CComBSTR bstrTemp;
			hr = spPropContainer->Evaluate(bstrPlatformIncs, &bstrTemp);
			*pbstrIncPath = bstrTemp.Detach();
			RETURN_ON_FAIL(hr);
		}
	}

	return S_OK;
}

HRESULT CVCFileNode::OnCompileFile()
{
	CComPtr<VCFile> spFile;
	GetVCFile(&spFile);
	RETURN_ON_NULL(spFile);

	CVCProjectNode* pProjNode = GetVCProjectNode();
	RETURN_ON_NULL(pProjNode);

	// Save the file
	CComPtr<IVsHierarchy> spHier;
	HRESULT hr = pProjNode->GetVsHierarchy(&spHier, TRUE);
	RETURN_ON_FAIL_OR_NULL(hr, spHier);

	CComPtr<IVsSolutionBuildManager> spBldMgr;
	hr = GetBuildPkg()->GetSolutionBuildManager(&spBldMgr, TRUE);
	RETURN_ON_FAIL(hr);

	CComQIPtr<IVsSolutionBuildManager2> spBldMgr2 = spBldMgr;
	VSASSERT(spBldMgr2, "Hey, why no IVsSolutionBuildManager2 for IVsSolutionBuildManager?!?");

	if (spBldMgr2 != NULL)
		spBldMgr2->SaveDocumentsBeforeBuild(spHier, VSITEMID_ROOT, NULL);

	CVCPtrList ptrList;
	ptrList.AddHead(this);
	return GetVCProjectNode()->CompileFileItems(ptrList);
}

HRESULT CVCFileNode::OnRefreshWebRef()
{   

	CComPtr<IDispatch> pDispFileCfg;
	GetActiveVCFileConfig(&pDispFileCfg);
	CComQIPtr<VCFileConfiguration> pFileCfg = pDispFileCfg;
	RETURN_ON_NULL(pFileCfg);

	CComPtr<IDispatch> pDispTool;
	HRESULT hr = pFileCfg->get_Tool(&pDispTool);
	CComQIPtr<VCWebServiceProxyGeneratorTool> pTool = pDispTool;
	RETURN_ON_NULL(pTool);

	CComBSTR bstrURL;
	pTool->get_URL(&bstrURL);
	if( bstrURL )
	{
		CComPtr<IDispatch> pProj;
		((CVCProjectNode*)GetRootNode())->GetDispVCProject(&pProj);
		CComQIPtr<IVCProjectImpl> pProjImpl = pProj;
		CComBSTR bstrFileOut;
		hr = pProjImpl->UpdateWSDL(bstrURL, VARIANT_TRUE /* exists */, &bstrFileOut);
	}

	CVCPtrList ptrList;
	ptrList.AddHead(this);
	CHECK_ZOMBIE(GetVCProjectNode(), IDS_ERR_PROJ_ZOMBIE);
	hr = GetVCProjectNode()->CompileFileItems(ptrList);

	return S_OK;
}

HRESULT CVCFileNode::CleanUpBeforeDelete()
{
	CComPtr<IDispatch> pParentDisp;
	CComPtr<IDispatch> pItemDisp;
	GetObjectsForDelete(&pParentDisp, &pItemDisp);
	return CleanUpObjectModel(pParentDisp, pItemDisp);
}

HRESULT CVCFileNode::GetObjectsForDelete(IDispatch** ppParentDisp, IDispatch** ppItemDisp)
{
	CHECK_POINTER_NULL(ppParentDisp);
	CHECK_POINTER_NULL(ppItemDisp);

	CComPtr<IDispatch> spParentDisp = ((CVCNode*)GetParent())->m_dispkeyItem;
	*ppParentDisp = spParentDisp.Detach();

	CComPtr<IDispatch> spItemDisp = m_dispkeyItem;
	*ppItemDisp = spItemDisp.Detach();

	return S_OK;
}

HRESULT CVCFileNode::CleanUpObjectModel(IDispatch* pParentDisp, IDispatch* pItemDisp)
{
	// Project
	CComQIPtr<VCProject> pProject = pParentDisp;
	if (pProject)
		return pProject->RemoveFile(pItemDisp);

	// Filter
	CComQIPtr<VCFilter> pFilter = pParentDisp;
	if (pFilter)
		return pFilter->RemoveFile(pItemDisp);

	return E_FAIL;
}

// do the UI dll part of delete
HRESULT CVCFileNode::CleanUpUI( CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen /*= TRUE*/ )
{
	HRESULT hr = S_OK;
	BOOL fIsOpen = FALSE;
	CComBSTR bstrPath;

	CComPtr<IVsWindowFrame> pFrame;
	if (bCloseIfOpen || dwDelItemOp == DELITEMOP_DeleteFromStorage)
	{
		CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;	
		hr = ExternalQueryService(SID_SVsUIShellOpenDocument, 
								  IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc);
		VSASSERT(SUCCEEDED(hr), "Unable to obtain IVsUIShellOpenDocument service!  Did the IDs change?  Do you need to investigate threading issues?");
		RETURN_ON_FAIL_OR_NULL(hr, pVsUIShellOpenDoc);
							  
		bstrPath = GetFullPath();
		CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(GetHierarchy());
		hr = pVsUIShellOpenDoc->IsDocumentOpen(spHier, 
											   (VSITEMID)GetVsItemID(), 
												bstrPath,
											   GUID_NULL,
											   IDO_ActivateIfOpen,
											   NULL,
											   NULL,
											   &pFrame,
											   &fIsOpen);
		VSASSERT(SUCCEEDED(hr), "Unable to determine if a document is open!  Are the parameters correct?  Do you need to investigate threading issues?");
	}

	if(dwDelItemOp == DELITEMOP_DeleteFromStorage )
	{
		if (fIsOpen && pFrame)
		{
			// close the file
			hr = pFrame->CloseFrame( FRAMECLOSE_NoSave );
			RETURN_ON_FAIL(hr);
		}

		CComPtr<IDispatch> pDisp = m_dispkeyItem;
		CComBSTR bstrName;

		CComQIPtr<VCFile> pFile = pDisp;
		if(pFile)
		{
			CString strName;
			pFile->get_FullPath(&bstrName);

			strName = bstrName;
			// SHFileOperation requires this field to have a double NULL
			// to indicate its termination
			strName += '\0';

			SHFILEOPSTRUCT fop;
			fop.hwnd = NULL;
			fop.wFunc = FO_DELETE;
			fop.pFrom = strName;
			fop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;			
			fop.fAnyOperationsAborted = NULL;
			fop.hNameMappings = NULL;
			fop.lpszProgressTitle = NULL;
			fop.pTo = NULL;

			// ignore failures, we want to continue on so files are still removed, 
			// even if they can't be deleted from disk
			SHFileOperation(&fop);
		}
	}
	else
	{
		if( fIsOpen && bCloseIfOpen && pFrame )
		{
			// close the file
			hr = pFrame->CloseFrame( FRAMECLOSE_PromptSave );
			RETURN_ON_FAIL(hr);
		}
	}

	m_fDeleted = TRUE;
	CVCBaseFolderNode* pParent = static_cast<CVCBaseFolderNode*>(GetParent());
	if (pParent)
		pParent->Delete(this, GetHierarchy());

	// IVsTrackProjectDocuments::OnAfterRemoveFiles called by VCProjectEngine, so not needed here
	return hr;
}

HRESULT CVCFileNode::OnDelete(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen /*= TRUE*/)
{
	CComPtr<IDispatch> pParentDisp;
	CComPtr<IDispatch> pItemDisp;
	GetObjectsForDelete( &pParentDisp, &pItemDisp );
	if( !pParentDisp || !pItemDisp )
		return E_FAIL;
	return CleanUpObjectModel( pParentDisp, pItemDisp );
}

//-----------------------------------------------------------------------------
// create an instance of a project node with refrence count 1
//-----------------------------------------------------------------------------
HRESULT CVCFileNode::CreateInstance(CVCFileNode **ppProjectNode, IDispatch* pItem)
{
	CComObject<CVCFileNode> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CVCFileNode>::CreateInstance(&pPrjObj);
	RETURN_ON_FAIL_OR_NULL(hr, pPrjObj);

	pPrjObj->AddRef();
	pPrjObj->Initialize(pItem);
	*ppProjectNode = pPrjObj;

	return hr;
}


//-----------------------------------------------------------------------------
// Called by CreateInstance to initialize us.
//-----------------------------------------------------------------------------
void CVCFileNode::Initialize(IDispatch* pItem)
{	
	m_dispkeyItem = pItem;
}

//-----------------------------------------------------------------------------
// Reads the project file (if we havent' already done so), and loads the project 
// into memory.
//-----------------------------------------------------------------------------
BOOL CVCFileNode::Open()
{
	BOOL bRetVal = TRUE;
	// open the project file. For now, we pretend we have one.
	return bRetVal;
}

//-----------------------------------------------------------------------------
// Shell's OpenWith
//-----------------------------------------------------------------------------
HRESULT CVCFileNode::OpenWith()
{
	CComBSTR bstrPath;
	bstrPath = GetFullPath();
	return OpenNormalEditor(bstrPath, OSE_UseOpenWithDialog, LOGVIEWID_UserChooseView, FALSE);
}

bool CVCFileNode::IsBuildable( void )
{
	if (!(GetBuildPkg()->NoBuildsInProgress()))
		return false;

	RETURN_ON_NULL2(GetVCProjectNode(), false);

	HRESULT hr;
	CComPtr<IDispatch> pDisp;

	// get the active IVCGenericConfiguration
	CComPtr<IVCGenericConfiguration> pGenCfg;
	hr = GetVCProjectNode()->GetActiveConfig( &pGenCfg );
	if( FAILED( hr ) || !pGenCfg )
	{
		// if we couldn't get an active config, just use the first config in
		// the list of configs
		CComPtr<VCProject> spProject;
		hr = GetVCProjectNode()->GetVCProject(&spProject);
		RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spProject, false);
		hr = spProject->get_Configurations( &pDisp );
		CComQIPtr<IVCCollection> pColl = pDisp;
		RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, pColl, false);
		pDisp = NULL;
		hr = pColl->Item( CComVariant( 1 ), &pDisp );
		if( hr != S_OK )
			return false;
	}
	// otherwise we have an active config
	else
	{
		// QI for IVCCfg
		CComQIPtr<IVCCfg> pVCCfg = pGenCfg;
		if( !pVCCfg )
		{
			VSASSERT( false, "QI from IVCGenericConfiguration to IVCCfg failed" );
			return false;
		}
		// get_Object to get the "internal object"
		hr = pVCCfg->get_Object( &pDisp );
		VSASSERT( SUCCEEDED( hr ), "IVCCfg::get_Object failed" );
	}

	// QI for VCConfiguration
	CComQIPtr<VCConfiguration> pVCConfig = pDisp;
	if( !pVCConfig )
	{
		VSASSERT( false, "QI from IDispatch to VCConfiguration failed" );
		return false;
	}
	// check for an active spawner
	CComQIPtr<IVCConfigurationImpl> pVCConfigImpl = pDisp;
	if (pVCConfigImpl)
	{
		VARIANT_BOOL bCanBuild;
		hr = pVCConfigImpl->get_CanStartBuild(&bCanBuild);
		VSASSERT(SUCCEEDED(hr), "IVCConfigurationImpl::get_CanStartBuild failed");
		if (bCanBuild == VARIANT_FALSE)
			return false;
	}

	// check for a makefile configuration (NO individual files are buildable
	// in makefile configs)
	ConfigurationTypes cfgType;
	hr = pVCConfig->get_ConfigurationType( &cfgType );
	VSASSERT( SUCCEEDED( hr ), "VCConfiguration::get_ConfigurationType failed" );
	if( cfgType == typeUnknown )
		return false;
	// get our internal VCFile 
	CComPtr<VCFile> pFile;
	GetVCFile(&pFile);
	// QI for IVCFileImpl
	CComQIPtr<IVCFileImpl> pFileImpl = pFile;
	if( !pFileImpl )
	{
		VSASSERT( false, "QI from VCFile to IVCFileImpl failed" );
		return false;
	}
	// get the VCFileConfiguration for the VCConfiguration we have
	CComPtr<VCFileConfiguration> pFileCfg;
	hr = pFileImpl->GetFileConfigurationForProjectConfiguration( pVCConfig, &pFileCfg );
	VSASSERT( SUCCEEDED( hr ), "IVCFileImpl::GetFileConfigurationFromProjectConfiguration failed" );
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, pFileCfg, false);
	// get the ExcludedFromBuild prop from the VCFileConfiguration
	VARIANT_BOOL bExcludedFromBuild;
	hr = pFileCfg->get_ExcludedFromBuild( &bExcludedFromBuild );
	VSASSERT( SUCCEEDED( hr ), "VCFileConfiguration::get_ExcludedFromBuild failed" );
	
	if( bExcludedFromBuild == VARIANT_TRUE )
		return false;
	// if it wasn't excluded from build, it may still not be
	// buildable
	// get its tool and find out
	pDisp = NULL;
	hr = pFileCfg->get_Tool( &pDisp );
	VSASSERT( SUCCEEDED( hr ), "VCFileConfiguration:get_Tool failed" );
	RETURN_ON_NULL2(pDisp, false);

	// is it a custom build tool?
	CComQIPtr<VCCustomBuildTool> pCustBldTool = pDisp;
	if( pCustBldTool )
	{
		// does it have a non-blank output?
		CComBSTR bstrOutputs;
		pCustBldTool->get_Outputs( &bstrOutputs );
		if( bstrOutputs.Length() != 0 )
			return true;
	}
	// if not, it's buildable
	else
		return true;

	return false;
}

bool CVCFileNode::IsHTML( void )
{
	CComBSTR bstrPath;
	bstrPath = GetFullPath();

	CPathW path;
	path.Create( bstrPath );
	
	const wchar_t* wszExt = path.GetExtension();
	if( _wcsicmp( wszExt, L".html" ) == 0 )
		return true;
	if( _wcsicmp( wszExt, L".htm" ) == 0 )
		return true;

	return false;
}

bool CVCFileNode::IsWSDL( void )
{
	CComBSTR bstrPath;
	bstrPath = GetFullPath();

	CPathW path;
	path.Create( bstrPath );
	
	const wchar_t* wszExt = path.GetExtension();
	if( _wcsicmp( wszExt, L".wsdl" ) == 0 )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// IVCFileNodeProps
//-----------------------------------------------------------------------------
STDMETHODIMP CVCFileNode::get_Name( BSTR *pVal )
{
	CComQIPtr<VCFile> spFile = m_dispkeyItem;
	RETURN_ON_NULL2(spFile, E_NOINTERFACE);

	return spFile->get_Name( pVal );
}

STDMETHODIMP CVCFileNode::get_FullPath( BSTR *pVal )
{
	CComQIPtr<VCFile> spFile = m_dispkeyItem;
	RETURN_ON_NULL2(spFile, E_NOINTERFACE);

	return spFile->get_FullPath( pVal );
}
STDMETHODIMP CVCFileNode::get_RelativePath(BSTR *pVal)
{
	CComQIPtr<VCFile> spFile = m_dispkeyItem;
	RETURN_ON_NULL2(spFile, E_NOINTERFACE);

	return spFile->get_RelativePath( pVal );
}

STDMETHODIMP CVCFileNode::put_RelativePath(BSTR newVal)
{
	CComQIPtr<VCFile> spFile = m_dispkeyItem;
	RETURN_ON_NULL2(spFile, E_NOINTERFACE);
	
	return spFile->put_RelativePath( newVal );
}

STDMETHODIMP CVCFileNode::get_DeploymentContent( VARIANT_BOOL *pVal )
{
	CComQIPtr<VCFile> spFile = m_dispkeyItem;
	RETURN_ON_NULL2(spFile, E_NOINTERFACE);

	return spFile->get_DeploymentContent( pVal );
}

STDMETHODIMP CVCFileNode::put_DeploymentContent( VARIANT_BOOL newVal )
{
	CComQIPtr<VCFile> spFile = m_dispkeyItem;
	RETURN_ON_NULL2(spFile, E_NOINTERFACE);
		
	return spFile->put_DeploymentContent( newVal );
}

STDMETHODIMP CVCFileNode::GetLocalizedPropertyInfo( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc)
{
	CComQIPtr<IDispatch> pdisp;
	QueryInterface(__uuidof(IDispatch), (void**)&pdisp);
	RETURN_ON_NULL(pdisp);

	CComPtr<ITypeInfo> pTypeInfo;
	HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
	CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
	RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo2);

	CComBSTR bstrDoc;
	hr = pTypeInfo2->GetDocumentation2(dispid, localeID, &bstrDoc, NULL, NULL);
	RETURN_ON_FAIL_OR_NULL(hr, bstrDoc.m_str);

	LPOLESTR pDesc = wcsstr( bstrDoc, L": " );
	RETURN_ON_NULL(pDesc);

	int nSize = (int)(pDesc - bstrDoc);
	pDesc+=2;

	CComBSTR bstrName( nSize-1, bstrDoc );
	if (pbstrName != NULL)
		*pbstrName = bstrName.Detach();

	CComBSTR bstrDesc( pDesc );
	if (pbstrDesc != NULL)
		*pbstrDesc = bstrDesc.Detach();
	
	return S_OK;
}

STDMETHODIMP CVCFileNode::GetClassName(BSTR* pbstrClassName)
{
	CHECK_POINTER_NULL(pbstrClassName);
	CComBSTR bstrClassName;
	bstrClassName.LoadString(IDS_FILE_PROPERTIES);
	*pbstrClassName = bstrClassName.Detach();
	if (*pbstrClassName)
		return S_OK;
	else
		return E_OUTOFMEMORY;
}

//---------------------------------------------------------------------------
// IVsExtensibleObject
//---------------------------------------------------------------------------
STDMETHODIMP CVCFileNode::GetAutomationObject(/*[in]*/ LPCOLESTR  pszPropName, /*[out]*/ IDispatch **ppDisp)
{
	CHECK_POINTER_VALID(ppDisp);
	*ppDisp = NULL;

	CComPtr<VCFileConfiguration> spConfig;
	CComBSTR bstrCfgName = pszPropName;
	HRESULT hr = GetCfgOfName(bstrCfgName, &spConfig);
	RETURN_ON_FAIL_OR_NULL(hr, spConfig);

	return CVCFileCfgProperties::CreateInstance(spConfig, ppDisp);
}

//-----------------------------------------------------------------------------
// IVsCfgProvider2
//-----------------------------------------------------------------------------
STDMETHODIMP CVCFileNode::GetCfgs( ULONG celt, IVsCfg *rgpcfg[], ULONG *pcActual, VSCFGFLAGS *prgfFlags )
{
	HRESULT hr;
	CComPtr<IEnumVARIANT>		pEnum;
	CComPtr<IDispatch>			pDisp;
	CComQIPtr<IVCCollection>	pCollection;
	
	// get the file object
	CComPtr<VCFile>			pFile;
	GetVCFile(&pFile);
	RETURN_ON_NULL2(pFile, E_NOINTERFACE);

	// Get config collection
	hr = pFile->get_FileConfigurations( &pDisp );
	pCollection = pDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, pCollection, E_NOINTERFACE);

	hr = pCollection->_NewEnum( reinterpret_cast<IUnknown **>( &pEnum ) );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);

	//If there's a pointer to the actual count
	if (pcActual)
	{
		//Get the count
		long cCount;
		hr = pCollection->get_Count( &cCount );
		RETURN_ON_FAIL(hr);

		*pcActual = cCount;

		//If asking for count, return
		if (celt == 0) return S_OK;
	}
	else if (celt == 0) return E_POINTER;	//Asking for count, but null pointer supplied

	// loop the file config array 
	pEnum->Reset();
	for (unsigned int i = 0; i < celt; i++)
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) break;
	
		//Get file config
		CComQIPtr<VCFileConfiguration> pFileConfig;
		pFileConfig = var.punkVal;
		RETURN_ON_NULL2(pFileConfig, E_NOINTERFACE);

		// create the config for return
		hr = CFileGenCfg::CreateInstance( &rgpcfg[i], pFileConfig );
		RETURN_ON_FAIL_OR_NULL(hr, pFileConfig);
	}
	return S_OK;
} 

STDMETHODIMP CVCFileNode::GetCfgNames( ULONG celt,BSTR rgbstr[], ULONG *pcActual )
{
	HRESULT hr;
	CComPtr<IEnumVARIANT>		pEnum;
	CComPtr<IDispatch>			pDisp;
	CComQIPtr<IVCCollection>	pCollection;
	
	// get the file object
	CComPtr<VCFile>			pFile;
	GetVCFile(&pFile);
	RETURN_ON_NULL2(pFile, E_NOINTERFACE);

	//Get config collection
	hr = pFile->get_FileConfigurations( &pDisp );
	pCollection = pDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, pCollection, E_NOINTERFACE);

	hr = pCollection->_NewEnum( reinterpret_cast<IUnknown **>( &pEnum ) );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);

	//If there's a pointer to the actual count
	if (pcActual)
	{
		//Get the count
		long cCount;
		hr = pCollection->get_Count( &cCount );
		RETURN_ON_FAIL(hr);

		*pcActual = cCount;

		//If asking for count, return
		if (celt == 0) return S_OK;
	}
	else if (celt == 0) return E_POINTER;	//Asking for count, but null pointer supplied

	//Loop the config name array filling it out
	pEnum->Reset();
	for (unsigned int i = 0; i < celt; i++)
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) break;
	
		//Get file config
		CComQIPtr<VCFileConfiguration> pFileConfig;
		pFileConfig = var.punkVal;
		RETURN_ON_NULL2(pFileConfig, E_NOINTERFACE);

		// get file config internal interface
		CComQIPtr<IVCFileConfigurationImpl> pFileConfigImpl = pFileConfig;
		RETURN_ON_NULL2(pFileConfigImpl, E_NOINTERFACE);
	
		// get config
		CComPtr<IDispatch> pDispConfig;
		CComQIPtr<VCConfiguration> pConfig;
		hr = pFileConfigImpl->get_Configuration( &pDispConfig );
		pConfig = pDispConfig;
		RETURN_ON_FAIL_OR_NULL2(hr, pConfig, E_NOINTERFACE);
		
		CComBSTR bstrName;
 		hr = pConfig->get_ConfigurationName(&bstrName);
		RETURN_ON_FAIL(hr);

		//Fill out array
		rgbstr[i] = bstrName.Detach();
	}
	return S_OK;
}

STDMETHODIMP CVCFileNode::GetPlatformNames( ULONG celt, BSTR rgbstr[],	ULONG *pcActual)
{
	if( celt == 0 )
	{
		if( pcActual == NULL )
			return E_POINTER;
		*pcActual = 1;
		return S_OK;
	}

	if (celt > 1 && pcActual != NULL)
		*pcActual = 1;

	CComBSTR bstrPlat = L"Win32";
	rgbstr[0] = bstrPlat.Detach();
	return S_OK;
}

HRESULT CVCFileNode::GetCfgOfName(BSTR szCfgName, VCFileConfiguration** ppFileCfg)
{
	CComBSTR bstrCfgName = szCfgName;

	CComPtr<IDispatch> pDisp;
	CComQIPtr<VCFile> pFile = m_dispkeyItem;
	RETURN_ON_NULL2(pFile, E_NOINTERFACE);
	
	// get the collection of file configs
	pDisp = NULL;
	CComQIPtr<IVCCollection> pColl;
	HRESULT hr = pFile->get_FileConfigurations( &pDisp );
	pColl = pDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, pColl, E_NOINTERFACE);
	
	// get a new enum from the collection
	CComPtr<IEnumVARIANT> pEnum;
	hr = pColl->_NewEnum( reinterpret_cast<IUnknown**>(&pEnum) );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	
	long actualCfgs = 0;
	pEnum->Reset();
	while( true )
	{
		CComVariant var;
		hr = pEnum->Next( 1, &var, NULL );
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE )
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			return E_FAIL;

		CComQIPtr<VCFileConfiguration> pConfig = var.punkVal;
		RETURN_ON_NULL(pConfig);
		CComBSTR bstrName;
		hr = pConfig->get_Name( &bstrName );
		RETURN_ON_FAIL(hr);
		if( bstrName == bstrCfgName )
		{
			*ppFileCfg = pConfig.Detach();
			return S_OK;
		}
	}
	// Hmmmm.  Got here, so maybe the config name we started with didn't have a platform on it, so no match
	CStringW strCfgName = bstrCfgName;
	if (strCfgName.Find(L"|") > 0)
		return E_FAIL;	// got told a platform and didn't find it; bye-bye

	bstrCfgName += L"|";
	int nLen = bstrCfgName.Length();

	pEnum->Reset();
	while (true)
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) 
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			return E_FAIL;

		CComQIPtr<VCFileConfiguration> pConfig = var.punkVal;
		RETURN_ON_NULL(pConfig);
		CComBSTR bstrName;
		hr = pConfig->get_Name(&bstrName);
		RETURN_ON_FAIL(hr);
		if (bstrName.m_str && bstrCfgName.m_str && _wcsnicmp(bstrName.m_str, bstrCfgName.m_str, nLen) == 0)
		{
			*ppFileCfg = pConfig.Detach();
			return S_OK;
		}
	}

	return E_FAIL;
}

STDMETHODIMP CVCFileNode::GetCfgOfName( LPCOLESTR pszCfgName, LPCOLESTR pszPlatformName, IVsCfg **ppCfg )
{
	CHECK_POINTER_NULL(ppCfg);

	CComBSTR bstrCfgName = pszCfgName;
	bstrCfgName += L"|";
	bstrCfgName += pszPlatformName;

	CComPtr<VCFileConfiguration> spFileCfg;
	HRESULT hr = GetCfgOfName(bstrCfgName, &spFileCfg);
	RETURN_ON_FAIL_OR_NULL(hr, spFileCfg);

	return CFileGenCfg::CreateInstance( ppCfg, spFileCfg );
}

STDMETHODIMP CVCFileNode::AddCfgsOfCfgName( LPCOLESTR pszCfgName, LPCOLESTR pszCloneCfgName, BOOL fPrivate )
{
	return E_NOTIMPL;	// CVCFileNode::AddCfgsOfCfgName
}

STDMETHODIMP CVCFileNode::DeleteCfgsOfCfgName( LPCOLESTR pszCfgName )
{
	return E_NOTIMPL;	// CVCFileNode::DeleteCfgsOfCfgName
}

STDMETHODIMP CVCFileNode::RenameCfgsOfCfgName( LPCOLESTR pszOldName, LPCOLESTR pszNewName )
{
	return E_NOTIMPL;	// CVCFileNode::RenameCfgsOfCfgName
}
		
STDMETHODIMP CVCFileNode::AddCfgsOfPlatformName( LPCOLESTR pszPlatformName, LPCOLESTR pszClonePlatformName )
{
	return E_NOTIMPL;	// CVCFileNode::AddCfgsOfPlatformName
}

STDMETHODIMP CVCFileNode::DeleteCfgsOfPlatformName( LPCOLESTR pszPlatformName )
{
	return E_NOTIMPL;	// CVCFileNode::DeleteCfgsOfPlatformName
}

STDMETHODIMP CVCFileNode::GetSupportedPlatformNames( ULONG celt, BSTR rgbstr[], ULONG *pcActual )
{
	return E_NOTIMPL;	// CVCFileNode::GetSupportedPlatformNames
}

STDMETHODIMP CVCFileNode::GetCfgProviderProperty( VSCFGPROPID propid, VARIANT *pOut )
{
    // check for bad arg
	CHECK_POINTER_NULL(pOut);
    if( pOut->vt != VT_EMPTY ) 
        return E_POINTER;

	switch( propid )
	{
	case VSCFGPROPID_SupportsCfgEditing:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_SupportsPlatformEditing:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_SupportsCfgAdd:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_SupportsCfgDelete:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_SupportsCfgRename:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_SupportsPlatformAdd:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_SupportsPlatformDelete:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_SupportsPrivateCfgs:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_IntrinsicExtenderCATID:
		pOut->vt = VT_BSTR;
		LPOLESTR pStr;
		StringFromCLSID(__uuidof(IVCCfg),&pStr);
		pOut->bstrVal = SysAllocString(pStr);
		CoTaskMemFree(pStr);
		break;
	default:
		RETURN_INVALID();
	}
	
	return S_OK;
}

STDMETHODIMP CVCFileNode::AdviseCfgProviderEvents( IVsCfgProviderEvents *pCPE, VSCOOKIE *pdwCookie )
{
	return E_NOTIMPL;	// CVCFileNode::AdviseCfgProviderEvents
}

STDMETHODIMP CVCFileNode::UnadviseCfgProviderEvents( VSCOOKIE dwCookie )
{
	return E_NOTIMPL;	// CVCFileNode::UnadviseCfgProviderEvents
}

HRESULT CVCFileNode::GetExtObject(CComVariant& varRes)
{
	CComPtr<IDispatch> spDispFile;
	GetVCItem(&spDispFile);
	CComQIPtr<VCFile> spFile = spDispFile;
	RETURN_ON_NULL(spFile);

	CComPtr<IDispatch> spDispParent;
	spFile->get_Parent(&spDispParent);
	CComQIPtr<VCProject> spProject = spDispParent;
	CComQIPtr<VCFilter> spFilter = spDispParent;
	if (spProject == NULL && spFilter == NULL)
		return E_UNEXPECTED;

	HRESULT hr = S_OK;

	// need the project regardless
	CVCProjectNode* pProjNode = GetVCProjectNode();
	RETURN_ON_NULL(pProjNode);
	CComVariant var;
	pProjNode->GetExtObject(var);
	if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
		return E_FAIL;
	CComQIPtr<IDispatch> spDispProj = var.pdispVal;

	CComPtr<IDispatch> spDispItems;
	CComQIPtr<IDispatch> spAutoParent;
	if (spProject)	// project is parent
	{
		spAutoParent = spDispProj;
		hr = spProject->get_Items(&spDispItems);
	}
	else	// filter is parent
	{
		CComQIPtr<IVCExternalCookie> spCookie = spFilter;
		RETURN_ON_NULL(spCookie);
		void* pCookie = NULL;
		spCookie->get_ExternalCookie(&pCookie);
		RETURN_ON_NULL(pCookie);
		CVCFileGroup* pParentGroup = (CVCFileGroup*)pCookie;
		RETURN_ON_NULL(pParentGroup);
		CComVariant var2;
		pParentGroup->GetExtObject(var2);
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			return E_FAIL;
		spAutoParent = var.pdispVal;
		hr = spFilter->get_Items(&spDispItems);
	}
	RETURN_ON_FAIL_OR_NULL(hr, spDispItems);

	CComPtr<CAutoItems> pAutoItems;
	hr = CAutoItems::CreateInstance(&pAutoItems, spAutoParent, spDispProj, spDispItems);
	RETURN_ON_FAIL(hr);

	CComPtr<CAutoFile> pAutoFile;
	hr = CAutoFile::CreateInstance(&pAutoFile, spDispItems, spDispProj, spDispFile);
	RETURN_ON_FAIL(hr);

	varRes.vt = VT_DISPATCH;
	varRes.pdispVal = (IDispatch *)pAutoFile.Detach();

	return S_OK;
}

BSTR CVCFileNode::GetFullPath( void )
{
	CComPtr<VCFile> spFile;
	GetVCFile(&spFile);
	RETURN_ON_NULL2(spFile, NULL);

	CComBSTR bstrFullPath;
	spFile->get_FullPath( &bstrFullPath );
	return bstrFullPath.Detach();
}

// automation extender methods
STDMETHODIMP CVCFileNode::get_Extender(BSTR bstrName, IDispatch **ppDisp)
{
	return GetExtender( s_wszCATID, bstrName, ppDisp );
}

STDMETHODIMP CVCFileNode::get_ExtenderNames(VARIANT *pvarNames)
{
	return GetExtenderNames( s_wszCATID, pvarNames );
}

STDMETHODIMP CVCFileNode::get_ExtenderCATID(BSTR *pbstrGuid)
{
	CHECK_POINTER_NULL( pbstrGuid );
	CComBSTR bstrRetVal = s_wszCATID;
	*pbstrGuid = bstrRetVal.Detach();
	return S_OK;
}


