//	StyleRef.cpp

// This class loads a Reference chunk

#include "stdafx.h"
#include "NodeRefChunk.h"
#include <DMUSProd.h>
#include <dmusici.h>
#include <dmusicf.h>

#define FOURCC_CHILD_NODE_RIFFCHUNK		mmioFOURCC('c','h','n','r')
#define FOURCC_CHILD_NODE_CHUNK			mmioFOURCC('c','h','n','c')

CNodeRefChunk::CNodeRefChunk( IDMUSProdFramework* pIFramework, IDMUSProdNode* pINode )
{
	m_pIFramework = pIFramework;
	ASSERT( m_pIFramework );

	m_pINode = pINode;
	if( m_pINode )
	{
		m_pINode->AddRef();
	}
}

CNodeRefChunk::~CNodeRefChunk()
{
	if( m_pINode )
	{
		m_pINode->Release();
	}
}

HRESULT CNodeRefChunk::Save( IStream* pIStream )
{
	HRESULT hr;

	ASSERT(m_pIFramework);
	ASSERT( m_pINode != NULL );
	if(!m_pIFramework || !m_pINode )
	{
		return E_UNEXPECTED;
	}

	IDMUSProdNode *pDocRootNode = NULL;
	if( SUCCEEDED( m_pINode->GetDocRootNode( &pDocRootNode ) ) )
	{
		if( pDocRootNode )
		{
			pDocRootNode->Release();
		}

		if( (pDocRootNode == m_pINode) || (pDocRootNode == NULL) )
		{
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
		else
		{
			return SaveChildNode( pIStream, pDocRootNode );
		}
	}

	return E_FAIL;
}

HRESULT CNodeRefChunk::Load( IStream* pIStream )
{
	HRESULT hr;

	ASSERT(m_pIFramework);
	ASSERT( m_pINode == NULL );
	if(!m_pIFramework)
	{
		return E_UNEXPECTED;
	}

	// Allocate an IDMUSProdRIFFStream
	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return E_FAIL;
	}

	DWORD dwStreamPos = StreamTell( pIStream );

	MMCKINFO ckChunk;
	if( pIRiffStream->Descend( &ckChunk, NULL, 0 ) != 0 )
	{
		pIRiffStream->Release();
		return E_FAIL;
	}

	// Look for a child node chunk
	if( (ckChunk.ckid == FOURCC_RIFF) &&
		(ckChunk.fccType == FOURCC_CHILD_NODE_RIFFCHUNK) )
	{
		// Found a child node chunk - load it
		hr = LoadChildNode( pIRiffStream, &ckChunk );
		pIRiffStream->Release();
	}
	else
	{
		pIRiffStream->Release();

		// Much be a normal refChunk - load it
		StreamSeek( pIStream, dwStreamPos, SEEK_SET );

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
			return E_FAIL;
		}
	}

	//ASSERT(m_pINode != NULL);

	return hr;
}

HRESULT CNodeRefChunk::SaveChildNode( IStream* pIStream, IDMUSProdNode *pDocRootNode )
{
	ASSERT( pIStream );
	ASSERT( pDocRootNode );
	ASSERT( m_pINode );

	// Allocate an IDMUSProdRIFFStream
	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return E_FAIL;
	}

	// Create the main RIFF chunk
	MMCKINFO ckMain;
	ckMain.fccType = FOURCC_CHILD_NODE_RIFFCHUNK;
	pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF );

	// Try and get and write the NodeID for the child node
	HRESULT hr = S_OK;
	GUID guidNodeID;
	if( SUCCEEDED( m_pINode->GetNodeId( &guidNodeID ) ) )
	{
		MMCKINFO ckSubChunk;
		ckSubChunk.ckid = DMUS_FOURCC_GUID_CHUNK;
		pIRiffStream->CreateChunk( &ckSubChunk, 0 );

		DWORD cbWritten;
		hr = pIStream->Write( &guidNodeID, sizeof(GUID), &cbWritten );
		if( FAILED( hr ) || (cbWritten != sizeof(GUID)) )
		{
			hr = E_FAIL;
		}

		pIRiffStream->Ascend( &ckSubChunk, 0 );
	}

	// Try and get and write the name of the child node
	BSTR bstrNodeName;
	if( SUCCEEDED( hr ) && SUCCEEDED( m_pINode->GetNodeName( &bstrNodeName ) ) )
	{
		MMCKINFO ckSubChunk;
		ckSubChunk.ckid = DMUS_FOURCC_UNAM_CHUNK;
		pIRiffStream->CreateChunk( &ckSubChunk, 0 );

		CString strNodeName = bstrNodeName;
		::SysFreeString( bstrNodeName );
		hr = SaveMBStoWCS( pIStream, &strNodeName );

		pIRiffStream->Ascend( &ckSubChunk, 0 );
	}

	// Get the IDMUSProdFileRefChunk
	IDMUSProdFileRefChunk*	pIFileRefChunk = NULL;
	if( SUCCEEDED( hr ) && SUCCEEDED( m_pIFramework->QueryInterface(IID_IDMUSProdFileRefChunk, (void **)&pIFileRefChunk) ) )
	{
		// Save the chunk
		hr = pIFileRefChunk->SaveRefChunk(pIStream, pDocRootNode);
		pIFileRefChunk->Release();
	}

	// Clean up
	pIRiffStream->Ascend( &ckMain, 0 );
	pIRiffStream->Release();

	return hr;
}

HRESULT CNodeRefChunk::LoadChildNode( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO *pckMain )
{
	// Get a stream pointer
	IStream *pIStream;
	pIStream = pIRiffStream->GetStream();

	// Initialize the HRESULT
	HRESULT hr = S_OK;

	// The subchunk
	MMCKINFO ckSubChunk;

	// Data identifying the child node
	GUID guidNode;
	ZeroMemory( &guidNode, sizeof(GUID) );
	CString cstrNodeName;
	IDMUSProdNode *pDocRootNode = NULL;

	// Count of byes read and the position just before the current subchunk
	DWORD cbRead, dwStreamPos = StreamTell( pIStream );

	// Iterate through all subchunks in pckMain
	while( pIRiffStream->Descend( &ckSubChunk, pckMain, 0 ) == 0 )
	{
		switch( ckSubChunk.ckid )
		{
		case DMUS_FOURCC_GUID_CHUNK:
			// Read in the node's GUID
			hr = pIStream->Read( &guidNode, sizeof(GUID), &cbRead );
			ASSERT( (cbRead == sizeof(GUID)) && SUCCEEDED( hr ) );
			break;

		case DMUS_FOURCC_UNAM_CHUNK:
			// Read in the node's name
			ReadMBSfromWCS( pIStream, ckSubChunk.cksize, &cstrNodeName );
			break;

		default:
			// Read in the node's DocRoot node
			StreamSeek( pIStream, dwStreamPos, SEEK_SET );

			{
				// Get the IDMUSProdFileRefChunk
				IDMUSProdFileRefChunk*	pIFileRefChunk = NULL;
				hr = m_pIFramework->QueryInterface(IID_IDMUSProdFileRefChunk, (void **)&pIFileRefChunk);
				ASSERT(SUCCEEDED(hr));
				if(SUCCEEDED(hr))
				{
					// Load the chunk
					hr = pIFileRefChunk->LoadRefChunk(pIStream, &pDocRootNode);
					pIFileRefChunk->Release();
				}
			}
			break;
		}
		pIRiffStream->Ascend( &ckSubChunk, 0 );
		dwStreamPos = StreamTell( pIStream );
	}

	/*
	ASSERT( pDocRootNode );
	ASSERT( !cstrNodeName.IsEmpty() );
	ASSERT( !::IsEqualGUID( guidNode, GUID_AllZeros ) );
	*/

	if( pDocRootNode )
	{
		m_pINode = FindChildNode( pDocRootNode, &cstrNodeName, guidNode );
		pDocRootNode->Release();
	}
	pIStream->Release();

	if( m_pINode )
	{
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

IDMUSProdNode *CNodeRefChunk::FindChildNode( IDMUSProdNode *pDocRootNode, const CString *pcstrName, const REFGUID rguidID )
{
	if( pDocRootNode == NULL )
	{
		return NULL;
	}

	// Compare GUIDs
	GUID guidID;
	if( SUCCEEDED( pDocRootNode->GetNodeId( &guidID ) ) &&
		::IsEqualGUID( rguidID, guidID ) )
	{
		// Compare names
		BSTR bstrNodeName;
		if( SUCCEEDED( pDocRootNode->GetNodeName( &bstrNodeName ) ) )
		{
			CString cstrNodeName = bstrNodeName;
			::SysFreeString( bstrNodeName );
			if( pcstrName->Compare( cstrNodeName ) == 0 )
			{
				// Found it - AddRef and return pDocRootNode
				pDocRootNode->AddRef();
				return pDocRootNode;
			}
		}
	}

	// Look in the child nodes
	IDMUSProdNode *pNode = NULL;
	if( SUCCEEDED( pDocRootNode->GetFirstChild( &pNode ) )
	&&	(pNode != NULL) )
	{
		// See if we find it in this child node
		IDMUSProdNode *pNodeToReturn;
		pNodeToReturn = FindChildNode( pNode, pcstrName, rguidID );

		if( pNodeToReturn != NULL )
		{
			// Found it, so release pNode
			pNode->Release();

			// pNodeToReturn was already AddRef()'d
			return pNodeToReturn;
		}

		// Iterate through the rest of the child nodes
		IDMUSProdNode *pNextChild;
		while( SUCCEEDED( pDocRootNode->GetNextChild( pNode, &pNextChild ) )
			   && (pNextChild != NULL) )
		{
			// Release the previous child node
			pNode->Release();

			// See if pNode contains the requested node
			pNodeToReturn = FindChildNode( pNextChild, pcstrName, rguidID );
			if( pNodeToReturn != NULL )
			{
				// Found it, so release pNextChild
				pNextChild->Release();

				// pNodeToReturn was already AddRef()'d
				return pNodeToReturn;
			}

			// Update pOrigChild and try to get the next child node
			pNode = pNextChild;

			pNextChild = NULL;
		}

		// Release the previous child node
		pNode->Release();
	}

	return NULL;
}
