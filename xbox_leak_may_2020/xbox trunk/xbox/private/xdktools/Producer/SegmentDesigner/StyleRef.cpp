//  StyleRef.cpp

// This class loads a Reference chunk

#include "stdafx.h"
#include "StyleRef.h"
#include <DMUSProd.h>
#include "resource.h"

CStyleRef::CStyleRef()
{
	m_pIFramework = NULL;
	m_pINode = NULL;
}

CStyleRef::~CStyleRef()
{
	if( m_pINode )
	{
		m_pINode->Release();
	}
}

HRESULT CStyleRef::Save( IStream* pIStream, CLSID clsidComponent, GUID guidComponent )
{
	UNREFERENCED_PARAMETER( clsidComponent );
	UNREFERENCED_PARAMETER( guidComponent );
	HRESULT hr;

	ASSERT(m_pIFramework);
	if(!m_pIFramework)
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pINode != NULL );

	// Get the IDMUSProdFileRefChunk
	IDMUSProdFileRefChunk*	pIFileRefChunk = NULL;
	hr = m_pIFramework->QueryInterface(IID_IDMUSProdFileRefChunk, (void **)&pIFileRefChunk);
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		return E_UNEXPECTED;
	}

	// Save the chunk
	hr = pIFileRefChunk->SaveRefChunk(pIStream, m_pINode);
	pIFileRefChunk->Release();
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	return hr;
}

HRESULT CStyleRef::Load( IStream* pIStream )
{
	HRESULT hr;

	ASSERT(m_pIFramework);
	if(!m_pIFramework)
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pINode == NULL );

	// Load the reference

	// Get the IDMUSProdFileRefChunk
	IDMUSProdFileRefChunk*	pIFileRefChunk = NULL;
	hr = m_pIFramework->QueryInterface(IID_IDMUSProdFileRefChunk, (void **)&pIFileRefChunk);
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		return E_UNEXPECTED;
	}

	// Load the chunk
	hr = pIFileRefChunk->LoadRefChunk(pIStream, &m_pINode);
	pIFileRefChunk->Release();
	if(FAILED(hr))
	{
//		CString	strError;
//		strError.LoadString(IDS_ERR_STYLEREADFAIL);
//		AfxMessageBox(strError);
//		return S_FALSE;
		return E_FAIL;
	}

	ASSERT(m_pINode != NULL);

	return hr;
}
