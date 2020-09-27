// VCNodeFactory.cpp : Implementation of CVCNodeFactory
#include "stdafx.h"
#include "VCNodeFactory.h"
#include "vcprojectengine.h"	// g_pProjectEngine


static const WCHAR *wszProject = L"VisualStudioProject";
static const WCHAR *wszStyleSheet = L"VisualStudioStyleSheet";
static const WCHAR *wszConfiguration = L"Configuration";
static const WCHAR *wszFileConfiguration = L"FileConfiguration";
static const WCHAR *wszTool = L"Tool";
static const WCHAR *wszFilter = L"Filter";
static const WCHAR *wszFile = L"File";
static const WCHAR *wszPlatform = L"Platform";
static const WCHAR *wszGlobals = L"Globals";
static const WCHAR *wszGlobal = L"Global";
static const WCHAR *wszXML = L"xml";

/////////////////////////////////////////////////////////////////////////////
// CVCNodeFactory

CVCNodeFactory::CVCNodeFactory()
{
}

CVCNodeFactory::~CVCNodeFactory()
{
}

HRESULT CVCNodeFactory::CreateInstance( IVCNodeFactory **ppNodeFactory )
{
	HRESULT hr;
	CVCNodeFactory *pVar;
	CComObject<CVCNodeFactory> *pObj;
	hr = CComObject<CVCNodeFactory>::CreateInstance( &pObj );
	if( SUCCEEDED( hr ) )
	{
		pVar = pObj;
		pVar->AddRef();
		*ppNodeFactory = pVar;
	}
	return hr;
}

STDMETHODIMP CVCNodeFactory::put_Name( BSTR bstrName )
{
	m_bstrName = bstrName;
	return S_OK;
}

STDMETHODIMP CVCNodeFactory::get_Root( IDispatch **ppDisp )
{
	CHECK_POINTER_NULL( ppDisp );
	m_pRoot.CopyTo( ppDisp );
	return S_OK;
}

STDMETHODIMP CVCNodeFactory::get_FileEncoding( BSTR* pbstrEncoding )
{
	CHECK_POINTER_NULL(pbstrEncoding);
	m_bstrEncoding.CopyTo(pbstrEncoding);
	return S_OK;
}

STDMETHODIMP CVCNodeFactory::NotifyEvent( IXMLNodeSource* pSource, XML_NODEFACTORY_EVENT iEvt )
{
	// TODO: do we actually care about any of this??

	// events we care about:
	switch( iEvt )
	{
	// XMLNF_STARTDOCUMENT - start of document
	case XMLNF_STARTDOCUMENT:
		break;
	// XMLNF_ENDPROLOG ?? - end of prolog, start of 'real' data
	case XMLNF_ENDPROLOG:
		break;
	// XMLNF_ENDDOCUMENT - end of document
	case XMLNF_ENDDOCUMENT:
		break;
	default:
		break;
	}
	return S_OK;
}

STDMETHODIMP CVCNodeFactory::BeginChildren( IXMLNodeSource* pSource, XML_NODE_INFO* pNodeInfo )
{
	// TODO: do we need to do anything here??
	// called when a node may contain children
	// e.g. <foo name="bar"> will cause BeginChildren to be called,
	// but <foo name="bar"/> will not
	return S_OK;
}

STDMETHODIMP CVCNodeFactory::EndChildren( IXMLNodeSource* pSource, BOOL fEmpty, XML_NODE_INFO* pNodeInfo )
{
	// TODO: do we need to do anything here?
	// no more children 
	// e.g. </foo>
	return S_OK;
}

STDMETHODIMP CVCNodeFactory::Error( IXMLNodeSource* pSource, HRESULT hrErrorCode, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR **aNodeInfo )
{
	// TODO: implement
	// an error has occurred reading the xml
	// free all resources and bail
	// (the HRESULT we pass out of here will be the HRESULT that Run returns)
	CComPtr<IDispatch> pDispColl;
	HRESULT hr = g_pProjectEngine->get_Projects( &pDispColl );
	RETURN_ON_FAIL2(hr, hrErrorCode);
	CComQIPtr<IVCCollection> pProjects = pDispColl;
	RETURN_ON_NULL2(pProjects, hrErrorCode);
	CComPtr<IDispatch> pDispProject;
	HRESULT hrProj = pProjects->Item( CComVariant( m_bstrName ), &pDispProject );
	if (SUCCEEDED(hrProj) && pDispProject)
	{
		if (CVCProjectEngine::s_pBuildPackage)
		{
			CComQIPtr<VCProject> spProject = pDispProject;
			if (spProject)
				CVCProjectEngine::s_pBuildPackage->ErrorCloseOfProject(spProject);
		}
		CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
		if (spProjEngineImpl)
			spProjEngineImpl->RemoveProjectUnderShell( pDispProject );
		return hrErrorCode;
	}

	pDispColl.Release();
	CComQIPtr<IVCProjectEngineImpl> spProjectEngineImpl = g_pProjectEngine;
	VSASSERT(spProjectEngineImpl != NULL, "Hey, bad project engine!");
	hr = spProjectEngineImpl->get_AllStyleSheets( &pDispColl );
	RETURN_ON_FAIL2(hr, hrErrorCode);
	CComQIPtr<IVCCollection> pStyleSheets = pDispColl;
	RETURN_ON_NULL2(pStyleSheets, hrErrorCode);
	CComPtr<IDispatch> pDispStyleSheet;
	HRESULT hrStyle = pStyleSheets->Item( CComVariant( m_bstrName ), &pDispStyleSheet );
	if (SUCCEEDED(hrStyle) && pDispStyleSheet)
	{
		CComQIPtr<IVCProjectEngineImpl> spProjectEngineImpl = g_pProjectEngine;
		if (spProjectEngineImpl)
			spProjectEngineImpl->UnloadStyleSheet(pDispStyleSheet);
	}

	return hrErrorCode;
}

STDMETHODIMP CVCNodeFactory::CreateNode( IXMLNodeSource __RPC_FAR *pSource, PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR **aNodeInfo )
{
	// main method for creating our data
	
	HRESULT hr;

	CComPtr<IDispatch> pDisp; // the dispatch of the last object created

	// counter for looping through attributes
	USHORT i = 0;

	// ignore whitespace
	if( aNodeInfo[0]->dwType == XML_WHITESPACE )
		return S_OK;
	
	// what type of node is this?
	// (the first XML_NODE_INFO in array better be the XML_ELEMENT...)

	// these if-else's are arranged so that the most commonly occuring nodes are
	// first, for better large-project perf (less wcscmp's)

	// File node
	if( wcscmp( aNodeInfo[0]->pwcText, wszFile ) == 0 )
	{
		hr = HandleFileNode(pNodeParent, cNumRecs, aNodeInfo, pDisp);
		RETURN_ON_FAIL(hr);
	}
	// Filter node
	else if( wcscmp( aNodeInfo[0]->pwcText, wszFilter ) == 0 )
	{
		hr = HandleFilterNode(pNodeParent, cNumRecs, aNodeInfo, pDisp);
		RETURN_ON_FAIL(hr);
	}
	// Tool node
	else if( wcscmp( aNodeInfo[0]->pwcText, wszTool ) == 0 )
	{
		hr = HandleToolNode(pNodeParent, cNumRecs, aNodeInfo, pDisp);
		RETURN_ON_FAIL(hr);
	}
	// Configuration node
	else if( wcscmp( aNodeInfo[0]->pwcText, wszConfiguration ) == 0 )
	{
		hr = HandleProjConfigNode(pNodeParent, cNumRecs, aNodeInfo, pDisp);
		RETURN_ON_FAIL(hr);
	}
	// FileConfiguration node
	else if( wcscmp( aNodeInfo[0]->pwcText, wszFileConfiguration ) == 0 )
	{
		hr = HandleFileConfigNode(pNodeParent, cNumRecs, aNodeInfo, pDisp);
		RETURN_ON_FAIL(hr);
	}
	// Platform node
	else if( wcscmp( aNodeInfo[0]->pwcText, wszPlatform ) == 0 )
	{
		hr = HandlePlatformNode(pNodeParent, cNumRecs, aNodeInfo, pDisp);
		RETURN_ON_FAIL(hr);
	}
	// Project node
	else if( wcscmp( aNodeInfo[0]->pwcText, wszProject ) == 0 )
	{
		hr = HandleProjectNode(cNumRecs, aNodeInfo, pDisp, i);
		RETURN_ON_FAIL(hr);
	}
	// style sheet node
	else if (wcscmp( aNodeInfo[0]->pwcText, wszStyleSheet ) == 0 )
	{
		hr = HandleStyleSheetNode(pNodeParent, cNumRecs, aNodeInfo, pDisp);
		RETURN_ON_FAIL(hr);
	}
	else if( wcscmp( aNodeInfo[0]->pwcText, wszGlobal ) == 0 )
	{
		hr = HandleGlobalNode(pNodeParent, cNumRecs, aNodeInfo, pDisp, i);
		RETURN_ON_FAIL(hr);
	}
	else if( wcsicmp( aNodeInfo[0]->pwcText, wszXML ) == 0 )
	{
		hr = HandleXMLHeaderNode(cNumRecs, aNodeInfo);
		RETURN_ON_FAIL(hr);
	}

	// other node types we don't care about, ignore but continue so the parent
	// gets set correctly
			
	// set this object's properties from the attribute list given
	for( ; i < cNumRecs; i++ )
	{
		// type?
		switch( aNodeInfo[i]->dwType )
		{
		case XML_ELEMENT:
			if( pDisp )
				// return the dispatch pointer in the node info structure
				aNodeInfo[i]->pNode = (void*)pDisp;
			else
				aNodeInfo[i]->pNode = pNodeParent;
			break;
		case XML_ATTRIBUTE:
		{
			CComBSTR pwcText, bstrVal;
			CollectAttributeNameAndValue(cNumRecs, aNodeInfo, i, pwcText, bstrVal);
			CComVariant varArg = bstrVal;

			// file configs won't have a parent dispatch pointer set, 
			// all other nodes should
			if( pDisp )
			{
				CComDispatchDriver dispDriver( pDisp );
				hr = dispDriver.PutPropertyByName( pwcText, &varArg );
			}

			break;
		}
		case XML_PCDATA:
			// shouldn't ever get here: the data elements ought to be handled
			// in the XML_ATTRIBUTE case above
			VSASSERT( FALSE, "VC XML Reader: XML_PCDATA nodes ought never be reached here!" );
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);
		default:
			// ignore the node data we don't care about
			break;
		}
	}
	
	return S_OK;
}

HRESULT CVCNodeFactory::ValidateTagAndGetName(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, LPCOLESTR szAttribName, CComBSTR& bstrName)
{
	// 1st attribute MUST be of type szAttribName
	VSASSERT( cNumRecs > 2, "Invalid project file, node has insufficient attributes" );
	if( cNumRecs <= 2 )
	{
		RETURN_INVALID();
	}
	if( aNodeInfo[1]->dwType != XML_ATTRIBUTE || aNodeInfo[2]->dwType != XML_PCDATA )
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);
	if( wcsncmp( szAttribName, aNodeInfo[1]->pwcText, aNodeInfo[1]->ulLen ) != 0 )
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);
	bstrName = CComBSTR( aNodeInfo[2]->ulLen, aNodeInfo[2]->pwcText );

	return S_OK;
}

HRESULT CVCNodeFactory::HandleFileNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp)
{	// Add the file to the project or filter, depending on what the parent node is
	CComBSTR bstrName;
	HRESULT hr = ValidateTagAndGetName(cNumRecs, aNodeInfo, L"RelativePath", bstrName);
	RETURN_ON_FAIL(hr);

	CComQIPtr<VCProject> pProject = (IDispatch*)pNodeParent;
	CComQIPtr<VCFilter> pFilter = (IDispatch*)pNodeParent;

	// if the last node was a project
	if( pProject )
	{
		hr = pProject->AddFile( bstrName, &pDisp );
		RETURN_ON_FAIL(hr);
		if (pDisp == NULL)
			return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILE_NOT_FOUND, bstrName);
	}
	// if the last node was a filter
	else if( pFilter )
	{
		hr = pFilter->AddFile( bstrName, &pDisp );
		RETURN_ON_FAIL(hr);
		if (pDisp == NULL)
			return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILTER_NOT_FOUND, bstrName);
	}
	// if the last node was neither, the XML is malformed, bail
	else 
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);

	return S_OK;
}


HRESULT CVCNodeFactory::HandleFilterNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp)
{
	// Add the filter to the project or filter, depending on what the parent node is

	CComBSTR bstrName;
	HRESULT hr = ValidateTagAndGetName(cNumRecs, aNodeInfo, L"Name", bstrName);
	RETURN_ON_FAIL(hr);

	CComQIPtr<VCProject> pProject = (IDispatch*)pNodeParent;
	CComQIPtr<VCFilter> pFilter = (IDispatch*)pNodeParent;

	// if the last node was a project
	if( pProject )
	{
		hr = pProject->AddFilter( bstrName, &pDisp );
		RETURN_ON_FAIL(hr);
		if (pDisp == NULL)
			return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILTER_NOT_FOUND, bstrName);
	}
	// if the last node was a filter
	else if( pFilter )
	{
		hr = pFilter->AddFilter( bstrName, &pDisp );
		RETURN_ON_FAIL(hr);
		if (pDisp == NULL)
			return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILTER_NOT_FOUND, bstrName);
	}
	// if the last node was neither, the XML is malformed, bail
	else 
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);

	return S_OK;
}

HRESULT CVCNodeFactory::HandleToolNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp)
{
	// Get this tool from the config's or style sheet's tool collection
	// or from the file config, depending on what the parent node is
	
	CComQIPtr<VCConfiguration> pConfig = (IDispatch*)pNodeParent;
	CComQIPtr<VCFileConfiguration> pFileConfig = (IDispatch*)pNodeParent;
	CComQIPtr<VCStyleSheet> pStyleSheet = (IDispatch*)pNodeParent;

	HRESULT hr = S_OK;

	// if the last node was a config or style sheet , we need to index the tool collection
	if( pConfig || pStyleSheet )
	{
		// get the tools collection
		CComPtr<IDispatch> pDispColl;
		if( pConfig )
			hr = pConfig->get_Tools( &pDispColl );
		else
			hr = pStyleSheet->get_Tools( &pDispColl );
		CComQIPtr<IVCCollection> pToolsColl = pDispColl;
		RETURN_ON_FAIL_OR_NULL2(hr, pToolsColl, E_NOINTERFACE);
		CComBSTR bstrName;
		hr = ValidateTagAndGetName(cNumRecs, aNodeInfo, L"Name", bstrName);
		RETURN_ON_FAIL(hr);
		// store the item
		hr = pToolsColl->Item( CComVariant( bstrName ), &pDisp );
		if( FAILED( hr ) )
		{
			if (pConfig)
			{
				CComQIPtr<IVCConfigurationImpl> pConfigImpl = pConfig;
				hr = pConfigImpl->AddTool(bstrName, &pDisp);
			}
			else
			{
				CComQIPtr<IVCStyleSheetImpl> pStyleSheetImpl = pStyleSheet;
				hr = pStyleSheetImpl->AddTool(bstrName, &pDisp);
			}
			if (FAILED(hr))
			{
				pDisp = NULL;
				hr = S_FALSE;
			}
		}
		else if (pDisp == NULL)
		{
			if (pConfig)
			{
				CComQIPtr<IVCConfigurationImpl> pConfigImpl = pConfig;
				hr = pConfigImpl->AddTool(bstrName, &pDisp);
			}
			else
			{
				CComQIPtr<IVCStyleSheetImpl> pStyleSheetImpl = pStyleSheet;
				hr = pStyleSheetImpl->AddTool(bstrName, &pDisp);
			}
			if (FAILED(hr))
			{
				pDisp = NULL;
				hr = S_FALSE;
			}
		}
	}
	
	// if the last node was a file config, just set pDisp to the file
	// config's tool 
	else if( pFileConfig )
	{
		// set the tool
		CComQIPtr<IVCFileConfigurationImpl> pFileCfgImpl = pFileConfig;
		if (!pFileCfgImpl)
			return E_NOINTERFACE;

		CComPtr<IDispatch> pDispConfig;
		hr = pFileCfgImpl->get_Configuration(&pDispConfig);
		CComQIPtr<VCConfiguration> pConfig = pDispConfig;
		RETURN_ON_FAIL_OR_NULL2(hr, pConfig, E_NOINTERFACE);

		CComPtr<IDispatch> pDispColl;
		hr = pConfig->get_Tools( &pDispColl );		
		CComQIPtr<IVCCollection> pToolsColl = pDispColl;
		RETURN_ON_FAIL_OR_NULL2(hr, pToolsColl, E_NOINTERFACE);

		CComBSTR bstrName;
		hr = ValidateTagAndGetName(cNumRecs, aNodeInfo, L"Name", bstrName);
		RETURN_ON_FAIL(hr);

		// find the parent tool
		CComPtr<IDispatch> pDispParentTool;
		hr = pToolsColl->Item( CComVariant( bstrName ), &pDispParentTool );
		if( FAILED( hr ) )
		{
			pDisp = NULL;
			hr = S_FALSE;
		}

		// set the tool
		hr = pFileConfig->put_Tool( pDispParentTool );
		RETURN_ON_FAIL(hr);

		// get the actual tool (as opposed to the parent one used to setup the one we want)
		CComPtr<IDispatch> pDispTool;
		hr = pFileConfig->get_Tool( &pDispTool );
		RETURN_ON_FAIL(hr);
		// store it
		pDisp = pDispTool;
	}

	// if the last node was neither, the XML is malformed, bail
	else 
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);

	return S_OK;
}

HRESULT CVCNodeFactory::HandleProjConfigNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp)
{
	// Add the config
	CComQIPtr<VCProject> pProject = (IDispatch*)pNodeParent;
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

	// get the whole name (cfg name + "|" + platform name)
	CComBSTR bstrName;
	HRESULT hr = ValidateTagAndGetName(cNumRecs, aNodeInfo, L"Name", bstrName);
	RETURN_ON_FAIL(hr);

	// get just the cfg name part
	WCHAR* pos = wcsstr( bstrName, L"|" );
	if (pos == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);
	CComBSTR bstrCfgName( (UINT) (pos - bstrName), bstrName );
	// add this cfg name (adds it to all platforms)
	hr = pProject->AddConfiguration( bstrCfgName );
	// get the config collection from the project
	CComPtr<IDispatch> pDispColl;
	hr = pProject->get_Configurations( &pDispColl );
	CComQIPtr<IVCCollection> pCfgColl = pDispColl;
	RETURN_ON_FAIL_OR_NULL2(hr, pCfgColl, E_NOINTERFACE);
	// index into it by config name + "|" + platform name
	hr = pCfgColl->Item( CComVariant( bstrName ), &pDisp );
	if( FAILED( hr ) )
	{
		pDisp = NULL;
		return hr;
	}

	return S_OK;
}

HRESULT CVCNodeFactory::HandleFileConfigNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp)
{
	// Get this file config

	CComQIPtr<VCFile> pFile = (IDispatch*)pNodeParent;
	RETURN_ON_NULL2(pFile, E_NOINTERFACE);
	// get the collection
	CComPtr<IDispatch> pDispColl;
	HRESULT hr = pFile->get_FileConfigurations( &pDispColl );
	CComQIPtr<IVCCollection> pFileConfigsColl = pDispColl;
	RETURN_ON_FAIL_OR_NULL2(hr, pFileConfigsColl, E_NOINTERFACE);
	CComBSTR bstrName;
	hr = ValidateTagAndGetName(cNumRecs, aNodeInfo, L"Name", bstrName);
	RETURN_ON_FAIL(hr);

	// store the item
 	hr = pFileConfigsColl->Item( CComVariant( bstrName ), &pDisp );
	if( FAILED( hr ) )
	{
		pDisp = NULL;
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILE_CFG_NOT_FOUND, bstrName);
	}

	return S_OK;
}

HRESULT CVCNodeFactory::HandlePlatformNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp)
{
	CComQIPtr<VCProject> pProject = (IDispatch*)pNodeParent;
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

	CComBSTR bstrName;
	HRESULT hr = ValidateTagAndGetName(cNumRecs, aNodeInfo, L"Name", bstrName);
	RETURN_ON_FAIL(hr);

	return pProject->AddPlatform( bstrName );
}

HRESULT CVCNodeFactory::HandleStyleSheetNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp)
{
	// Add the style sheet

	HRESULT hr = S_OK;
	CComQIPtr<IVCProjectEngineImpl> spProjectEngineImpl = g_pProjectEngine;
	if (spProjectEngineImpl)
		hr = spProjectEngineImpl->CreateHiddenStyleSheet(m_bstrName, &pDisp);
	else
		hr = g_pProjectEngine->CreateStyleSheet(m_bstrName, &pDisp);
	CComQIPtr<VCStyleSheet> pStyleSheet = pDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, pStyleSheet, E_NOINTERFACE);

	int cPlatforms = 0;
	for (int idx = 1; idx < cNumRecs; idx++)
	{	// MUST have one and only one platform name attribute somewhere; added way down below
		if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE)
		{
			if (wcsncmp(L"PlatformName", aNodeInfo[idx]->pwcText, aNodeInfo[idx]->ulLen) == 0)
				cPlatforms++;
		}
	}
	if (cPlatforms < 1)
	{
		CComQIPtr<IVCProjectEngineImpl> spProjectEngineImpl = g_pProjectEngine;
		if (spProjectEngineImpl)
			spProjectEngineImpl->UnloadStyleSheet(pDisp);
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_STYLE_FILE);
	}
	// set the root
	m_pRoot = pDisp;

	return S_OK;
}


HRESULT CVCNodeFactory::HandleGlobalNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, 
	CComPtr<IDispatch>& pDisp, USHORT& i)
{
	// Add a global to the collection

	// get the collection (parent node had better be the project!)
	CComQIPtr<IVCProjectImpl> pProjImpl = static_cast<IDispatch*>(pNodeParent);
	VSASSERT( pProjImpl, "Parent of Globals node is not the Project node!" );
	if( !pProjImpl )
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_PROJ_NODE_NOT_FOUND);
	CComPtr<IVCGlobals> pGlobals;
	HRESULT hr = pProjImpl->GetGlobals( &pGlobals );
	RETURN_ON_FAIL_OR_NULL(hr, pGlobals);

	// there had better be just two attributes, Name and Value

	// get the name/value
	CComBSTR bstrName, bstrValue;
	for( ; i < cNumRecs; i++ )
	{
		// type?
		switch( aNodeInfo[i]->dwType )
		{
		case XML_ATTRIBUTE:
		{
			CComBSTR bstrAttrName, bstrAttrValue;
			CollectAttributeNameAndValue(cNumRecs, aNodeInfo, i, bstrAttrName, bstrAttrValue);

			// is this the global's Name?
			if( wcscmp( bstrAttrName, L"Name" ) == 0 )
				bstrName = bstrAttrValue;
			// is this the global's Value?
			else if( wcscmp( bstrAttrName, L"Value" ) == 0 )
				bstrValue = bstrAttrValue;

			break;
		}
		case XML_PCDATA:
			// shouldn't ever get here: the data elements ought to be handled
			// in the XML_ATTRIBUTE case above
			VSASSERT( FALSE, "VC XML Reader: XML_PCDATA nodes ought never be reached here!" );
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);
		default:
			// ignore the node data we don't care about
			break;
		}
	}
	// set the name/value
	pGlobals->SetValue( bstrName, bstrValue );
	pGlobals->SetPersistance( bstrName, VARIANT_TRUE );

	return S_OK;
}

HRESULT CVCNodeFactory::HandleXMLHeaderNode(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo)
{
	for (USHORT i = 0; i < cNumRecs; i++)
	{
		// type?
		switch( aNodeInfo[i]->dwType )
		{
		case XML_ATTRIBUTE:
		{
			CComBSTR bstrAttrName, bstrAttrValue;
			CollectAttributeNameAndValue(cNumRecs, aNodeInfo, i, bstrAttrName, bstrAttrValue);

			// is this the encoding attribute?
			if( wcsicmp( bstrAttrName, L"encoding" ) == 0 )
				m_bstrEncoding = bstrAttrValue;

			break;
		}
		case XML_PCDATA:
			// shouldn't ever get here: the data elements ought to be handled
			// in the XML_ATTRIBUTE case above
			VSASSERT( FALSE, "VC XML Reader: XML_PCDATA nodes ought never be reached here!" );
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);
		default:
			// ignore the node data we don't care about
			break;
		}
	}
	return S_OK;
}

void CVCNodeFactory::CollectAttributeNameAndValue(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, USHORT& i, 
	CComBSTR& bstrAttrName, CComBSTR& bstrAttrValue)
{
	// aNodeInfo[i]->pwcText is the attribute name
	bstrAttrName = CComBSTR( aNodeInfo[i]->ulLen, aNodeInfo[i]->pwcText );

	if( (i+1) >= cNumRecs || aNodeInfo[i+1]->dwType != XML_PCDATA )
		bstrAttrValue = CComBSTR( L"" );
	else
	{
		// keep going through XML_PCDATA nodes until there aren't
		// any more, and appending their data strings
		CComBSTR bstrVal( L"" );
		while( (i+1) < cNumRecs && aNodeInfo[i+1]->dwType == XML_PCDATA )
		{
			bstrVal += CComBSTR( aNodeInfo[i+1]->ulLen, aNodeInfo[i+1]->pwcText );
			i++;
		}
		bstrAttrValue = bstrVal;
	}
}

HRESULT CVCNodeFactory::HandleProjectNode(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp, USHORT& i)
{
	HRESULT hr = g_pProjectEngine->CreateProject( m_bstrName, &pDisp );
	CComQIPtr<VCProject> pProject = pDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, pProject, E_NOINTERFACE);
	m_pRoot = pDisp;

	bool bHandledVersion = false;
	// set this object's properties from the attribute list given
	for( ; i < cNumRecs; i++ )
	{
		// type?
		switch( aNodeInfo[i]->dwType )
		{
		case XML_ELEMENT:
			if( pDisp )
				// return the dispatch pointer in the node info structure
				aNodeInfo[i]->pNode = (void*)pDisp;
			else
				aNodeInfo[i]->pNode = NULL;
			break;
		case XML_ATTRIBUTE:
		{
			CComBSTR bstrName, bstrVal;
			CollectAttributeNameAndValue(cNumRecs, aNodeInfo, i, bstrName, bstrVal);

			if (wcscmp(bstrName, L"Version") == 0)
			{
				bHandledVersion = true;
				CStringA strVersionA = bstrVal;	// yes, ANSI
				double fVersion = atof(strVersionA);
				if (fVersion != 7.0)
				{
					CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
					if (spProjEngineImpl)
					{
						CStringW strErr;
						strErr.Format(IDS_PROJ_UP_VERSION, bstrVal);
						CComBSTR bstrErr = strErr;
						spProjEngineImpl->DoFireReportError(bstrErr, E_FAIL, NULL);
					}
					return E_FAIL;
				}
			}
			else
			{
				CComVariant varArg = bstrVal;

				// file configs won't have a parent dispatch pointer set, 
				// all other nodes should
				if( pDisp )
				{
					CComDispatchDriver dispDriver( pDisp );
					hr = dispDriver.PutPropertyByName( bstrName, &varArg );
				}
			}

			break;
		}
		case XML_PCDATA:
			// shouldn't ever get here: the data elements ought to be handled
			// in the XML_ATTRIBUTE case above
			VSASSERT( FALSE, "VC XML Reader: XML_PCDATA nodes ought never be reached here!" );
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BAD_PROJ_FILE, IDS_ERR_BAD_PROJ_FILE);
		default:
			// ignore the node data we don't care about
			break;
		}
	}
	
	if (!bHandledVersion)
	{
		CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
		if (spProjEngineImpl)
		{
			CComBSTR bstrErr;
			bstrErr.LoadString(IDS_NO_PROJ_VERSION);
			spProjEngineImpl->DoFireReportError(bstrErr, E_FAIL, NULL);
		}
		return E_FAIL;
	}

	return S_OK;
}

