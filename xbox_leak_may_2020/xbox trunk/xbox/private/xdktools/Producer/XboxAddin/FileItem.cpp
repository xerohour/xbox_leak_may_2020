// FileItem.cpp : implementation file
//

#include "stdafx.h"
#include "FileItem.h"
#include "dmusprod.h"
#include "XboxAddin.h"
#include "XboxAddinComponent.h"
#include "DMPPrivate.h"
#include "GetNodesErrorDlg.h"
#include "DllJazzDataObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT CFileItem::CreateUniqueTempFile(CString sPrefix, CString sExtension, CString& sTempFileName)
{
	if(sExtension.IsEmpty())
	{
		sExtension = "tmp";
	}

	CString sFileExt = "." + sExtension;

	char szTempPath[MAX_PATH];
	DWORD dwSuccess = GetTempPath(MAX_PATH, szTempPath);
	CString sTempPath = szTempPath;	

	GUID guidFileName;
	if(dwSuccess && SUCCEEDED(::CoCreateGuid(&guidFileName)))
	{
		LPOLESTR psz;
		if( SUCCEEDED( ::StringFromIID(guidFileName, &psz) ) )
		{
			TCHAR szGuid[100];
			WideCharToMultiByte( CP_ACP, 0, psz, -1, szGuid, sizeof(szGuid), NULL, NULL );
			CoTaskMemFree( psz );
			
			sTempFileName = sTempPath + sPrefix + szGuid + sFileExt;

			CFile tempFile;
			try
			{
				tempFile.Open(sTempFileName, CFile::modeCreate);
				tempFile.Close();
			}
			catch(CFileException e)
			{
				return E_FAIL;
			}

			return S_OK;
		}
	}

	return E_FAIL;
}

IDMUSProdNode *CFileItem::FindChildNode( IDMUSProdNode *pDMUSProdNode, REFGUID rguidNodeId )
{
	// Get the first child node
	IDMUSProdNode *pChildNode;
	HRESULT hr = pDMUSProdNode->GetFirstChild( &pChildNode );

	while( SUCCEEDED(hr) && pChildNode )
	{
		GUID guidNode;

		// Get the node's ID
		hr = pChildNode->GetNodeId( &guidNode );

		if( SUCCEEDED(hr) )
		{
			// Is this the one?
			if( guidNode == rguidNodeId )
			{
				return pChildNode;
			}
		}

		// Check if any of the child node's children match this guid
		IDMUSProdNode *pNextChildNode = FindChildNode( pChildNode, rguidNodeId );

		// Yes
		if( pNextChildNode )
		{
			// Release pChildNode and return pNextChildNOde
			pChildNode->Release();
			return pNextChildNode;
		}

		// No - get the next child node
		hr = pDMUSProdNode->GetNextChild( pChildNode, &pNextChildNode );

		// Release the current node
		pChildNode->Release();

		// If we found a next node, make it the current node
		if( SUCCEEDED(hr) )
		{
			pChildNode = pNextChildNode;
		}
	}

	// Didn't find the node
	return NULL;
}

void CFileItem::GetRefsFromStyleBands( IDMUSProdNode *pDMUSProdNode, DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode, DWORD dwErrorLength, WCHAR *wcstrErrorText )
{
	// Find the style's band folder
	IDMUSProdNode *pBandFolderNode = FindChildNode( pDMUSProdNode, GUID_StyleBandFolderNode );

	HRESULT hr = E_FAIL;

	// Get the first band
	IDMUSProdNode *pChildNode = NULL;
	if( pBandFolderNode )
	{
		hr = pBandFolderNode->GetFirstChild( &pChildNode );
	}

	// Index into ppIDMUSProdNode to start writing the nodes at
	// Doubles as the count of nodes that we've written into the array
	DWORD dwIndex = 0;

	while( SUCCEEDED(hr) && pChildNode )
	{
		// QI for the IDMUSProdGetReferencedNodes interface
		IDMUSProdGetReferencedNodes *pIDMUSProdGetReferencedNodes;
		hr = pChildNode->QueryInterface( IID_IDMUSProdGetReferencedNodes, (void **)&pIDMUSProdGetReferencedNodes );

		// Get the referenced nodes from this band
		if( SUCCEEDED(hr) )
		{
			// If we have an array of nodes, and we haven't filled it yet
			if( ppIDMUSProdNode
			&&	*pdwArraySize > dwIndex )
			{
				// Calculate the size left in the array
				DWORD dwSize = (*pdwArraySize) - dwIndex;

				// Ask the band node to fill in the array
				HRESULT hr = pIDMUSProdGetReferencedNodes->GetReferencedNodes( &dwSize, &(ppIDMUSProdNode[dwIndex]), dwErrorLength, wcstrErrorText );

				// If successful, increment dwIndex
				if( SUCCEEDED(hr) )
				{
					dwIndex += dwSize;
				}
			}
			else
			{
				// Ask the band node how many array items it requires
				DWORD dwSize = 0;
				HRESULT hr = pIDMUSProdGetReferencedNodes->GetReferencedNodes( &dwSize, NULL, dwErrorLength, wcstrErrorText );

				// If successful, increment dwIndex
				if( SUCCEEDED(hr) )
				{
					dwIndex += dwSize;
				}
			}
			pIDMUSProdGetReferencedNodes->Release();
		}

		// Get the next child node
		IDMUSProdNode *pNextChildNode;
		hr = pBandFolderNode->GetNextChild( pChildNode, &pNextChildNode );

		// Release the current node
		pChildNode->Release();

		// If we found a next node, make it the current node
		if( SUCCEEDED(hr) )
		{
			pChildNode = pNextChildNode;
		}
	}

	// Release the band folder node
	if( pBandFolderNode )
	{
		pBandFolderNode->Release();
	}

	// Store the number of nodes we returned (or that we require)
	*pdwArraySize = dwIndex;
}

void CFileItem::GetContainerNodes( bool fRefNodes, IDMUSProdNode *pDMUSProdNode, DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode )
{
	// Find the container's folder
	IDMUSProdNode *pFolderNode;
	if( fRefNodes )
	{
		pFolderNode = FindChildNode( pDMUSProdNode, GUID_ContainerRefFolderNode );
	}
	else
	{
		pFolderNode = FindChildNode( pDMUSProdNode, GUID_ContainerEmbedFolderNode );
	}

	HRESULT hr = E_FAIL;

	// Get the first node
	IDMUSProdNode *pChildNode = NULL;
	if( pFolderNode )
	{
		hr = pFolderNode->GetFirstChild( &pChildNode );
	}

	// Index into ppIDMUSProdNode to start writing the nodes at
	// Doubles as the count of nodes that we've written into the array
	DWORD dwIndex = 0;

	while( SUCCEEDED(hr) && pChildNode)
	{
		// Get the node this reference points to
		IDataObject *pIDataObject;
		hr = pChildNode->CreateDataObject( &pIDataObject );

		if( SUCCEEDED(hr) )
		{
			IDMUSProdNode *pTargetNode;
			hr = theApp.m_pXboxAddinComponent->m_pIFramework->GetDocRootNodeFromData( pIDataObject, &pTargetNode );
			if( SUCCEEDED(hr) )
			{
				// If we have an array of nodes, and we haven't filled it yet
				if( ppIDMUSProdNode
				&&	*pdwArraySize > dwIndex )
				{
					// Add the node to the array
					ppIDMUSProdNode[dwIndex] = pTargetNode;
					ppIDMUSProdNode[dwIndex]->AddRef();
				}

				// Always increment dwIndex
				dwIndex++;

				pTargetNode->Release();
			}
			pIDataObject->Release();
		}

		// Get the next child node
		IDMUSProdNode *pNextChildNode;
		hr = pFolderNode->GetNextChild( pChildNode, &pNextChildNode );

		// Release the current node
		pChildNode->Release();

		// If we found a next node, make it the current node
		if( SUCCEEDED(hr) )
		{
			pChildNode = pNextChildNode;
		}
	}

	// Release the reference folder node
	if( pFolderNode )
	{
		pFolderNode->Release();
	}

	// Store the number of nodes we returned (or that we require)
	*pdwArraySize = dwIndex;
}

CFileItem::CFileItem( IDMUSProdNode *pDMUSProdNode )
{
	m_nAppendValue = 0;
	m_pFileNode = pDMUSProdNode;
	if( pDMUSProdNode )
	{
		pDMUSProdNode->AddRef();

		HRESULT hr;
		BSTR bstrTemp;
		hr = theApp.m_pXboxAddinComponent->m_pIFramework->GetNodeFileName( pDMUSProdNode, &bstrTemp );

		CString strFileName;
		GUID guidNode = GUID_NULL;
		if( SUCCEEDED(hr) )
		{
			strFileName = bstrTemp;

			// Only use the part after the last backslash
			int nBackSlash = strFileName.ReverseFind( _T('\\') );
			if( nBackSlash > 0 )
			{
				strFileName = strFileName.Right( strFileName.GetLength() - nBackSlash - 1 );
			}

			// Free the BSTR
			::SysFreeString( bstrTemp );

			hr = pDMUSProdNode->GetNodeId( &guidNode );
		}

		IDMUSProdDocType *pIDMUSProdDocType = NULL;
		if( SUCCEEDED(hr) )
		{
			hr = theApp.m_pXboxAddinComponent->m_pIFramework->FindDocTypeByNodeId( guidNode, &pIDMUSProdDocType );
		}

		IDMUSProdDocType8 *pIDMUSProdDocType8 = NULL;
		if( SUCCEEDED(hr) )
		{
			hr = pIDMUSProdDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDMUSProdDocType8 );
		}

		if( SUCCEEDED(hr) )
		{
			hr = pIDMUSProdDocType8->GetObjectExt( guidNode, FT_RUNTIME, &bstrTemp );
		}

		if( SUCCEEDED(hr) )
		{
			CString strExtension = bstrTemp;

			// Free the BSTR
			::SysFreeString( bstrTemp );

			// Strip off the extension (including the period)
			int nPeriod = strFileName.ReverseFind( _T('.') );
			if( nPeriod > 0 )
			{
				strFileName = strFileName.Left( nPeriod );
			}

			// Add on the runtime extension
			strFileName = strFileName + strExtension;
		}

		if( !strFileName.IsEmpty() )
		{
			// If we have a filename, store it
			m_strDisplayName = m_strFileName = strFileName;
		}

		if( pIDMUSProdDocType8 )
		{
			pIDMUSProdDocType8->Release();
		}

		if( pIDMUSProdDocType )
		{
			pIDMUSProdDocType->Release();
		}
	}
}

CFileItem::~CFileItem()
{
	RELEASE( m_pFileNode );

	while( !m_lstReferencedNodes.IsEmpty() )
	{
		m_lstReferencedNodes.RemoveHead()->Release();
	}

	while( !m_lstEmbeddedNodes.IsEmpty() )
	{
		m_lstEmbeddedNodes.RemoveHead()->Release();
	}
}

HRESULT CFileItem::CopyToXbox( void )
{
	HRESULT hr = S_OK;

	// Flag if this function displays an error message box (so that we don't display
	// another one below).
	bool fDisplayMessageBoxOnError = true;

	// Ensure all our embedded and referenced nodes are available
	hr = CopyNodeReferences( m_pFileNode, fDisplayMessageBoxOnError );

	// QI for the IPersistStream interface
	IPersistStream *pIPersistStream = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = m_pFileNode->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
	}

	// Create a temporary file name
	CString strTempFileName;
	if( SUCCEEDED(hr) )
	{
		hr = CreateUniqueTempFile( TEXT("Xbox"), TEXT(""), strTempFileName );
	}

	// Create a file stream for the temporary file
	IStream *pIStream = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = theApp.m_pXboxAddinComponent->m_pIFramework->AllocFileStream(
			strTempFileName.AllocSysString(),
			GENERIC_WRITE,
			FT_RUNTIME,
			GUID_CurrentVersion,
			NULL,
			&pIStream);
	}

	// Handle to the framework's progress bar
	HANDLE hProgressBar = NULL;

	if( SUCCEEDED(hr) )
	{
		// Initialize the progress bar
		CString strProgressText;
		strProgressText.FormatMessage( IDS_PROGRESS_FILE, m_strDisplayName );
		theApp.m_pXboxAddinComponent->m_pIFramework->StartProgressBar( 0, 2, strProgressText.AllocSysString(), &hProgressBar );

		// Save the file to the temporary file
		hr = pIPersistStream->Save( pIStream, FALSE );
	}

	// Release the stream
	RELEASE( pIStream );

	if( SUCCEEDED(hr) )
	{
		// Update the progress bar
		if( hProgressBar )
		{
			theApp.m_pXboxAddinComponent->m_pIFramework->SetProgressBarPos( hProgressBar, 1 );
		}

		// Copy the temporary file to the Xbox
		CString strTargetFile = theApp.m_strXboxDestination + TEXT("\\") + m_strFileName;
		hr = DmSendFile( strTempFileName, strTargetFile );

		// Update the progress bar
		if( hProgressBar )
		{
			theApp.m_pXboxAddinComponent->m_pIFramework->SetProgressBarPos( hProgressBar, 2 );
		}
	}

	// Delete the temporary file
	if( !strTempFileName.IsEmpty() )
	{
		// Delete the created temp file
		DeleteFile(strTempFileName);
	}

	// Release the IPersistStream interface
	if( pIPersistStream )
	{
		pIPersistStream->Release();
	}

	// Remove the progress bar
	if( hProgressBar )
	{
		theApp.m_pXboxAddinComponent->m_pIFramework->EndProgressBar( hProgressBar );
	}

	if( FAILED(hr) && fDisplayMessageBoxOnError )
	{
		CString strErrorText;
		strErrorText.FormatMessage( IDS_ERR_COPY, m_strFileName );
		XboxAddinMessageBox( NULL, strErrorText, MB_ICONERROR | MB_OK );
	}

	return hr;
}

HRESULT CFileItem::RemoveFromXbox( void )
{
	CString strRemoteCmd;
	char    szResp[MAX_PATH];
	DWORD   cchResp = MAX_PATH;

	// Clear the cache
	DmSendCommand(theApp.m_pdmConnection, TEXT(CMD_PREFIX "!clearCache"), szResp, &cchResp);

	// Wait for the command to be processed
	theApp.WaitForNotification( NOTIFICATION_CLEARCACHE_RESULT );

	// Delete the file from the Xbox
	CString strTargetFile = theApp.m_strXboxDestination + TEXT("\\") + m_strFileName;

	HRESULT hr = DmDeleteFile( strTargetFile, FALSE );

	// Ignore 'file not found' error
	if( hr == XBDM_NOSUCHFILE )
	{
		hr = XBDM_NOERR;
	}
	return hr;
}

bool CFileItem::UsesFile( const CFileItem *pFileItem )
{
	POSITION pos = m_lstReferencedNodes.GetHeadPosition();
	while( pos )
	{
		if( pFileItem->m_pFileNode == m_lstReferencedNodes.GetNext( pos ) )
		{
			return true;
		}
	}

	return false;
}

bool CFileItem::ContainsNode( const IDMUSProdNode *pNode )
{
	if( pNode == m_pFileNode )
	{
		return true;
	}

	POSITION pos = m_lstEmbeddedNodes.GetHeadPosition();
	while( pos )
	{
		if( pNode == m_lstEmbeddedNodes.GetNext( pos ) )
		{
			return true;
		}
	}

	return false;
}

HRESULT CFileItem::CopyNodeReferences( IDMUSProdNode *pDMUSProdNode, bool &fDisplayMessageBoxOnError )
{
	HRESULT hr = S_OK;

	// QI for the IDMUSProdGetReferencedNodes interface
	IDMUSProdGetReferencedNodes *pIDMUSProdGetReferencedNodes = NULL;
	pDMUSProdNode->QueryInterface( IID_IDMUSProdGetReferencedNodes, (void **)&pIDMUSProdGetReferencedNodes );

	WCHAR wcstrErrorText[8192];
	ZeroMemory( wcstrErrorText, sizeof(WCHAR) * 8192 );

	// Ask for size of the array of referenced nodes
	DWORD dwArraySize = 0;
	DWORD dwReferencedNodesSize = 0;
	if( pIDMUSProdGetReferencedNodes )
	{
		pIDMUSProdGetReferencedNodes->GetReferencedNodes( &dwReferencedNodesSize, NULL, 8191, wcstrErrorText );
		dwArraySize = dwReferencedNodesSize;
	}

	// Get the referenced nodes from a style
	DWORD dwStyleSize = 0;
	GetRefsFromStyleBands( pDMUSProdNode, &dwStyleSize, NULL, 8191, wcstrErrorText );
	dwArraySize += dwStyleSize;

	// Get the referenced nodes from a container in a segment, script, or song
	DWORD dwFullArraySize = 0;
	GetContainerNodes( true, pDMUSProdNode, &dwFullArraySize, NULL );
	dwFullArraySize += dwArraySize;

	// Get the embedded nodes from a container in a segment, script, or song
	DWORD dwEmbedArraySize = 0;
	GetContainerNodes( false, pDMUSProdNode, &dwEmbedArraySize, NULL );

	// Create the array and get the list of referenced nodes
	IDMUSProdNode **apIDMUSProdNode = NULL;
	if( dwFullArraySize )
	{
		apIDMUSProdNode = new IDMUSProdNode *[dwFullArraySize];
	}

	// Fill in the array
	if( apIDMUSProdNode )
	{
		if( pIDMUSProdGetReferencedNodes )
		{
			pIDMUSProdGetReferencedNodes->GetReferencedNodes( &dwFullArraySize, apIDMUSProdNode, 0, NULL );
		}

		// Get the referenced nodes from a style
		dwArraySize = dwFullArraySize - dwReferencedNodesSize;
		GetRefsFromStyleBands( pDMUSProdNode, &dwArraySize, &(apIDMUSProdNode[dwReferencedNodesSize]), 0, NULL );

		// Get the referenced nodes from a container in a segment, script, or song
		dwArraySize = dwFullArraySize - dwReferencedNodesSize - dwStyleSize;
		GetContainerNodes( true, pDMUSProdNode, &dwArraySize, &(apIDMUSProdNode[dwReferencedNodesSize + dwStyleSize]) );
	}

	// Create the array and get the list of embedded nodes
	IDMUSProdNode **apIDMUSProdNodeEmbedded = NULL;
	if( dwEmbedArraySize )
	{
		apIDMUSProdNodeEmbedded = new IDMUSProdNode *[dwEmbedArraySize];
	}

	// Fill in the array
	if( apIDMUSProdNodeEmbedded )
	{
		// Get the embedded nodes from a container in a segment, script, or song
		GetContainerNodes( false, pDMUSProdNode, &dwEmbedArraySize, apIDMUSProdNodeEmbedded );
	}

	// If there is an error message, display it
	if( wcstrErrorText[0] )
	{
		CGetNodesErrorDlg getNodesErrorDlg;
		getNodesErrorDlg.m_strErrorText = wcstrErrorText;
		getNodesErrorDlg.DoModal();

		hr = E_FAIL;

		// Set to false, since we already displayed an error message
		fDisplayMessageBoxOnError = false;
	}

	// Now, ensure each of these nodes are in our lists
	if( SUCCEEDED(hr) )
	{
		if( apIDMUSProdNodeEmbedded )
		{
			// First, add the node to our list
			for( DWORD i=0; i < dwEmbedArraySize; i++ )
			{
				m_lstEmbeddedNodes.AddHead( apIDMUSProdNodeEmbedded[i] );

				// AddRef it
				apIDMUSProdNodeEmbedded[i]->AddRef();
			}

			// Then ensure that all their references are copied
			for( i=0; i < dwEmbedArraySize && SUCCEEDED(hr); i++ )
			{
				hr = CopyNodeReferences( apIDMUSProdNodeEmbedded[i], fDisplayMessageBoxOnError );
			}
		}

		if( apIDMUSProdNode )
		{
			for( DWORD i=0; i < dwFullArraySize && SUCCEEDED(hr); i++ )
			{
				hr = theApp.m_pXboxAddinComponent->EnsureNodeIsCopied( apIDMUSProdNode[i] );

				// Add the node to our list
				m_lstReferencedNodes.AddHead( apIDMUSProdNode[i] );

				// AddRef it
				apIDMUSProdNode[i]->AddRef();
			}
		}
	}

	// Now, delete the array and release all of the nodes
	if( apIDMUSProdNode )
	{
		// Release the nodes
		for( DWORD i=0; i < dwFullArraySize; i++ )
		{
			if( apIDMUSProdNode[i] )
			{
				apIDMUSProdNode[i]->Release();
			}
		}
		delete[] apIDMUSProdNode;
		apIDMUSProdNode = NULL;
	}

	// Now, delete the array and release all of the nodes
	if( apIDMUSProdNodeEmbedded )
	{
		// Release the nodes
		for( DWORD i=0; i < dwEmbedArraySize; i++ )
		{
			if( apIDMUSProdNodeEmbedded[i] )
			{
				apIDMUSProdNodeEmbedded[i]->Release();
			}
		}
		delete[] apIDMUSProdNodeEmbedded;
		apIDMUSProdNodeEmbedded = NULL;
	}

	// Release the interface
	RELEASE( pIDMUSProdGetReferencedNodes );

	return hr;
}

void CFileItem::SetAppendValue( int nNewAppendValue )
{
	m_nAppendValue = nNewAppendValue;

	// Set the file's display name
	
	// Set the base name
	m_strDisplayName = m_strFileName;

	// Append a (#) if necessary
	if( m_nAppendValue )
	{
		CString strAppend;
		strAppend.Format( TEXT(" (%d)"), m_nAppendValue );
		m_strDisplayName += strAppend;
	}
}
