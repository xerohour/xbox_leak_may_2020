// OtherFile.cpp : implementation file
//

#include "stdafx.h"
#include "OtherFile.h"
#include "dmusprod.h"
#include "dmpprivate.h"
#include "XboxAddin.h"
#include "XboxAddinComponent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


COtherFile::COtherFile( IDMUSProdNode *pDMUSProdNode ) : CFileItem(pDMUSProdNode)
{
	GUID guidNode;
	HRESULT hr = pDMUSProdNode->GetNodeId( &guidNode );

	IDMUSProdDocType *pIDMUSProdDocType = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = theApp.m_pXboxAddinComponent->m_pIFramework->FindDocTypeByNodeId( guidNode, &pIDMUSProdDocType );
	}

	HINSTANCE hInstance = NULL;
	UINT nResourceId = 0;
	if( SUCCEEDED(hr) )
	{
		hr = pIDMUSProdDocType->GetResourceId( &hInstance, &nResourceId );
	}

	if( SUCCEEDED(hr) )
	{
		m_hIcon = ::LoadIcon( hInstance, MAKEINTRESOURCE (nResourceId) );
	}
	else
	{
		m_hIcon = NULL;
	}

	if( pIDMUSProdDocType )
	{
		pIDMUSProdDocType->Release();
	}
}
