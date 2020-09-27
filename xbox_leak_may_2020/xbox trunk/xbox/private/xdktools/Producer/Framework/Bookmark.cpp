// Bookmark.cpp: implementation of the CBookmark class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ioJazzDoc.h"
#include <mmreg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBookmark::CBookmark()
{
	CoCreateGuid( &m_guid ); 
}

CBookmark::~CBookmark()
{
	// Free editor list
	wpWindowPlacement* pWP;
	while( !m_lstEditors.IsEmpty() )
	{
		pWP = static_cast<wpWindowPlacement*>( m_lstEditors.RemoveHead() );
		delete pWP;
	}

	// Free component list
	bkComponentState* pCS;
	while( !m_lstComponents.IsEmpty() )
	{
		pCS = static_cast<bkComponentState*>( m_lstComponents.RemoveHead() );
		if( pCS->pComponentStateInfo )
		{
			GlobalFree( pCS->pComponentStateInfo );
		}
		delete pCS;
	}
}


//////////////////////////////////////////////////////////////////////
// CBookmark GetZOrder

short CBookmark::GetZOrder( CMDIChildWnd* pFrame )
{
	CWnd* pWnd;

	CWnd* pWndParent = pFrame->GetParent();
	if( pWndParent == NULL )
	{
		return -1;
	}

	short nZOrder = 0;

	pWnd = pWndParent->GetTopWindow();
	while( pWnd )
	{
		if( pWnd == pFrame )
		{
			return nZOrder;
		}

		pWnd = pWnd->GetNextWindow();
		nZOrder++;
	}

	return -1;
}


//////////////////////////////////////////////////////////////////////
// CBookmark AddToEditorList

void CBookmark::AddToEditorList( CComponentDoc* pComponentDoc, CComponentView* pComponentView )
{
	ASSERT( pComponentDoc != NULL );
	ASSERT( pComponentView != NULL );
	
	if( pComponentDoc->m_pFileNode == NULL 
	||  pComponentView->m_pINode == NULL )
	{
		// View does not yet have an editor associated with it
		return;
	}

	// Get Node's GUID
	GUID guidNodeId;
	if( FAILED ( pComponentView->m_pINode->GetNodeId ( &guidNodeId ) ) )
	{
		return;
	}

	// Get Node's name
	BSTR bstrName;
	if( FAILED ( pComponentView->m_pINode->GetNodeName ( &bstrName ) ) )
	{
		return;
	}
	CString strName = bstrName;
	::SysFreeString( bstrName );

	// Get Frame window
	CMDIChildWnd* pFrame = (CMDIChildWnd *)pComponentView->GetParentFrame();
	ASSERT_VALID( pFrame );

	// Get Frame's WINDOWPLACEMENT information
	WINDOWPLACEMENT wp;

	wp.length = sizeof(wp);
	if( pFrame->GetWindowPlacement( &wp ) == 0 )
	{
		return;
	}

	// Get Frame's Z-Order information
	short nZOrder = GetZOrder( pFrame );

	// Allocate wpWindowPlacement struct
	wpWindowPlacement* pWP = new wpWindowPlacement;
	if( pWP )
	{
		pWP->strNodeName = strName;
		memcpy( &pWP->guidFile, &pComponentDoc->m_pFileNode->m_guid, sizeof( pWP->guidFile ) );
		memcpy( &pWP->guidNodeId, &guidNodeId, sizeof( pWP->guidNodeId ) );
		pWP->lTreePos = theApp.m_pFramework->GetWPTreePos( pComponentView->m_pINode );

		pWP->nInternalUse = nZOrder;

		pWP->wp.length = sizeof(pWP->wp);
		pWP->wp.flags = wp.flags;
		pWP->wp.showCmd = wp.showCmd;
		pWP->wp.ptMinPosition = wp.ptMinPosition;
		pWP->wp.ptMaxPosition = wp.ptMaxPosition;
		pWP->wp.rcNormalPosition = wp.rcNormalPosition;

		// Place in list according to Z-Order
		if( !m_lstEditors.IsEmpty() )
		{
			wpWindowPlacement* pWPList;
			POSITION pos = m_lstEditors.GetHeadPosition();

			while( pos )
			{
				pWPList = m_lstEditors.GetNext( pos );

				if( pWPList->nInternalUse < pWP->nInternalUse )
				{
					pos = m_lstEditors.Find( pWPList );
					if( pos != NULL )
					{
						m_lstEditors.InsertBefore( pos, pWP );
						return;
					}
				}
			}
		}

		m_lstEditors.AddTail( pWP );
	}
}


//////////////////////////////////////////////////////////////////////
// CBookmark Create

BOOL CBookmark::Create( void )
{
	ASSERT( m_lstEditors.IsEmpty() );
	ASSERT( m_lstComponents.IsEmpty() );

	// Build open Editor list
	CDocTemplate* pTemplate;
	CComponentDoc* pComponentDoc;
	CComponentView* pComponentView;

	if( theApp.m_pDocManager )
	{
		// walk all templates in the application
		POSITION pos = theApp.m_pDocManager->GetFirstDocTemplatePosition();
		while( pos != NULL )
		{
			pTemplate = theApp.m_pDocManager->GetNextDocTemplate( pos );
			ASSERT_VALID( pTemplate );
			ASSERT_KINDOF( CDocTemplate, pTemplate );

			// walk all documents in the template
			POSITION pos2 = pTemplate->GetFirstDocPosition();
			while( pos2 )
			{
				pComponentDoc = (CComponentDoc *)pTemplate->GetNextDoc( pos2 );
				ASSERT_VALID( pComponentDoc );

				if( pComponentDoc->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
				{
					POSITION pos3 = pComponentDoc->GetFirstViewPosition();
					while( pos3 )
					{
						pComponentView = (CComponentView *)pComponentDoc->GetNextView( pos3 );
						ASSERT_VALID( pComponentView );

						if( pComponentView
						&&  pComponentView->IsKindOf( RUNTIME_CLASS(CComponentView) ) )
						{
							if( pComponentView->m_fSeed == FALSE )
							{
								AddToEditorList( pComponentDoc, pComponentView );
							}
						}
					}
				}
			}
		}
	}

	// Build component State list
	IDMUSProdComponent* pIComponent;
	IDMUSProdComponent* pINextComponent;
	IPersistStream* pIPS;
	IStream* pIMemStream;
	DWORD dwByteCount;
	STATSTG statstg;

    HRESULT hr = theApp.m_pFramework->GetFirstComponent( &pINextComponent );
	while( SUCCEEDED( hr )  &&  pINextComponent )
    {
		pIComponent = pINextComponent;

		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IPersistStream, (void **)&pIPS ) ) )
		{
			// Create a memory stream
			if( SUCCEEDED ( AllocMemoryStream ( FT_DESIGN, GUID_Bookmark, &pIMemStream ) ) )
			{
				// Place component state information in the stream
				if( SUCCEEDED ( pIPS->Save( pIMemStream, FALSE ) ) )
				{
					// Get size of stream
					if( SUCCEEDED ( pIMemStream->Stat( &statstg, STATFLAG_NONAME ) ) )
					{
						CJzComponent* pJzComponent = theApp.m_pFramework->GetJzComponent( pIComponent );
						if( pJzComponent )
						{
							// Populate the bkComponentState structure
							bkComponentState* pCS = new bkComponentState;
							if( pCS )
							{
								// Store the Component's CLSID
								pCS->clsidComponent = pJzComponent->m_clsidComponent;

								// Store the Component's state info
								pCS->dwComponentStateInfoSize = statstg.cbSize.LowPart;
								pCS->pComponentStateInfo = (BYTE *)GlobalAlloc( GPTR, pCS->dwComponentStateInfoSize );
								if( pCS->pComponentStateInfo )
								{
									StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
									if( SUCCEEDED ( pIMemStream->Read( pCS->pComponentStateInfo, pCS->dwComponentStateInfoSize, &dwByteCount ) )
									&&  dwByteCount == pCS->dwComponentStateInfoSize )
									{
										// Place the Component's state information in the list
										m_lstComponents.AddTail( pCS );
									}
									else
									{
										GlobalFree( pCS->pComponentStateInfo );
										delete pCS;
									}
								}
								else
								{
									delete pCS;
								}
							}
						}
					}
				}

				pIMemStream->Release();
			}

			pIPS->Release();
		}

	    hr = theApp.m_pFramework->GetNextComponent( pIComponent, &pINextComponent );
		pIComponent->Release();
    }

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CBookmark LoadBookmark

HRESULT CBookmark::LoadBookmark( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*    pIStream;
	MMCKINFO	ck;
	MMCKINFO	ckList;
	MMCKINFO	ckList2;
	DWORD		dwByteCount;
	DWORD		dwSize;
    HRESULT     hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_GUID_CHUNK:
				dwSize = min( ck.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guid, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case FOURCC_INFO_LIST:
					case FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_INAM:
								case FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case FOURCC_BOOKMARK_EDITOR_LIST:
					{
						wpWindowPlacement* pWP = new wpWindowPlacement;
						if( pWP )
						{
							m_lstEditors.AddTail( pWP );

							while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
							{
								switch( ckList.ckid )
								{
									case FOURCC_BOOKMARK_EDITOR_WP_CHUNK:
									{
										ioWindowPlacement ioWP;

										dwSize = min( ckList.cksize, sizeof( ioWindowPlacement ) );
										hr = pIStream->Read( &ioWP, dwSize, &dwByteCount );
										if( FAILED( hr )
										||  dwByteCount != dwSize )
										{
											hr = E_FAIL;
											goto ON_ERROR;
										}

										memcpy( &pWP->guidFile, &ioWP.guidFile, sizeof( pWP->guidFile ) );
										memcpy( &pWP->guidNodeId, &ioWP.guidNodeId, sizeof( pWP->guidNodeId ) );
										pWP->lTreePos = ioWP.lTreePos;

										pWP->wp.length = sizeof(pWP->wp);
										pWP->wp.flags = ioWP.wp.flags;
										pWP->wp.showCmd = ioWP.wp.showCmd;
										pWP->wp.ptMinPosition = ioWP.wp.ptMinPosition;
										pWP->wp.ptMaxPosition = ioWP.wp.ptMaxPosition;
										pWP->wp.rcNormalPosition = ioWP.wp.rcNormalPosition;
										break;
									}
								
									case FOURCC_LIST:
										switch( ckList.fccType )
										{
											case FOURCC_INFO_LIST:
											case FOURCC_UNFO_LIST:
												while( pIRiffStream->Descend( &ckList2, &ckList, 0 ) == 0 )
												{
													switch( ckList2.ckid )
													{
														case FOURCC_UNFO_NODE_NAME:
															ReadMBSfromWCS( pIStream, ckList2.cksize, &pWP->strNodeName );
															break;
													}
													pIRiffStream->Ascend( &ckList2, 0 );
												}
												break;
										}
										break;
								}
								pIRiffStream->Ascend( &ckList, 0 );
							}
						}
						break;
					}


					case FOURCC_BOOKMARK_COMPONENT_LIST:
					{
						bkComponentState* pCS = new bkComponentState;

						if( pCS == NULL )
						{
							hr = E_OUTOFMEMORY;
							goto ON_ERROR;
						}

						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_GUID_CHUNK:
								{
									dwSize = min( ckList.cksize, sizeof( GUID ) );
									hr = pIStream->Read( &pCS->clsidComponent, dwSize, &dwByteCount );
									if( FAILED( hr )
									||  dwByteCount != dwSize )
									{
										delete pCS;
										hr = E_FAIL;
										goto ON_ERROR;
									}
									break;
								}

								case FOURCC_BOOKMARK_COMPONENT_CHUNK:
								{
									pCS->dwComponentStateInfoSize = ckList.cksize;
									pCS->pComponentStateInfo = (BYTE *)GlobalAlloc( GPTR, pCS->dwComponentStateInfoSize );
									if( pCS->pComponentStateInfo == NULL )
									{
										delete pCS;
										hr = E_OUTOFMEMORY;
										goto ON_ERROR;
									}
									hr = pIStream->Read( pCS->pComponentStateInfo, pCS->dwComponentStateInfoSize, &dwByteCount );
									if( FAILED( hr )
									||  dwByteCount != pCS->dwComponentStateInfoSize )
									{
										hr = E_FAIL;
										GlobalFree( pCS->pComponentStateInfo );
										delete pCS;
										goto ON_ERROR;
									}
									m_lstComponents.AddTail( pCS );
									break;
								}
							}
							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
					}
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

ON_ERROR:
    pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBookmark::SaveNodeName

HRESULT CBookmark::SaveNodeName( IDMUSProdRIFFStream* pIRiffStream, LPCTSTR szNodeName )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;
	CString strName = szNodeName;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	ckMain.fccType = FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Node name
	{
		ck.ckid = FOURCC_UNFO_NODE_NAME;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBookmark::SaveGUID

HRESULT CBookmark::SaveGUID( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write GUID chunk header
	ck.ckid = FOURCC_GUID_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Bookmark GUID
	hr = pIStream->Write( &m_guid, sizeof(GUID), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(GUID) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBookmark::SaveInfoList

HRESULT CBookmark::SaveInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;
	CString strName;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	ckMain.fccType = FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare Bookmark name
	if( m_strName.IsEmpty() )
	{
		strName = _T("Default");
	}
	else
	{
		strName = m_strName;
	}

	// Write Bookmark name
	{
		ck.ckid = FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


//////////////////////////////////////////////////////////////////////
// CBookmark SaveBookmark

HRESULT CBookmark::SaveBookmark( CProject* pProject, IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
    MMCKINFO ckMain;
    MMCKINFO ck;
	HRESULT hr = S_OK;
	DWORD dwBytesWritten;
	POSITION pos;
	CFileNode* pFileNode;
	ioWindowPlacement ioWP;
	wpWindowPlacement* pWP;
	bkComponentState* pCS;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Save Bookmark GUID
	hr = SaveGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
        goto ON_ERROR;
	}

	// Save Bookmark name
	hr = SaveInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
        goto ON_ERROR;
	}

	// Save Editor WindowPlacement information
    pos = m_lstEditors.GetHeadPosition();
    while( pos )
    {
		pWP = m_lstEditors.GetNext( pos );

		// Only save part of Bookmark relating to this Project
		pFileNode = pProject->GetFileByGUID( pWP->guidFile );
		if( pFileNode == NULL )
		{
			continue;
		}
		pFileNode->Release();
		pFileNode = NULL;

		// Write FOURCC_BOOKMARK_EDITOR_LIST header
		ckMain.fccType = FOURCC_BOOKMARK_EDITOR_LIST;
		if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Save Node name
		hr = SaveNodeName( pIRiffStream, pWP->strNodeName );
		if( FAILED ( hr ) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Save ioWindowPlacement struct
		{
			// Write FOURCC_BOOKMARK_EDITOR_WP_CHUNK chunk header
			ck.ckid = FOURCC_BOOKMARK_EDITOR_WP_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Prepare ioWindowPlacement struct
			memcpy( &ioWP.guidFile, &pWP->guidFile, sizeof( ioWP.guidFile ) );
			memcpy( &ioWP.guidNodeId, &pWP->guidNodeId, sizeof( ioWP.guidNodeId ) );
			ioWP.lTreePos = pWP->lTreePos;

			ioWP.wp.length = sizeof(ioWP.wp);
			ioWP.wp.flags = pWP->wp.flags;
			ioWP.wp.showCmd = pWP->wp.showCmd;
			ioWP.wp.ptMinPosition = pWP->wp.ptMinPosition;
			ioWP.wp.ptMaxPosition = pWP->wp.ptMaxPosition;
			ioWP.wp.rcNormalPosition = pWP->wp.rcNormalPosition;

			// Write ioWindowPlacement struct
			hr = pIStream->Write( &ioWP, sizeof(ioWindowPlacement), &dwBytesWritten);
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(ioWindowPlacement) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
			
			if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
			{
 				hr = E_FAIL;
				goto ON_ERROR;
			}
		}		
		
		if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
    }

	// Save Component state information
    pos = m_lstComponents.GetHeadPosition();
    while( pos )
    {
		pCS = m_lstComponents.GetNext( pos );

		// Write FOURCC_BOOKMARK_COMPONENT_LIST header
		ckMain.fccType = FOURCC_BOOKMARK_COMPONENT_LIST;
		if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	
		// Save Component CLSID
		{
			// Write GUID chunk header
			ck.ckid = FOURCC_GUID_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Write Component GUID
			hr = pIStream->Write( &pCS->clsidComponent, sizeof(GUID), &dwBytesWritten);
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(GUID) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
			
			if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
			{
 				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// Save Component state info chunk
		{
			// Write FOURCC_BOOKMARK_COMPONENT_CHUNK chunk header
			ck.ckid = FOURCC_BOOKMARK_COMPONENT_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Write FOURCC_BOOKMARK_COMPONENT_CHUNK data
			hr = pIStream->Write( pCS->pComponentStateInfo,
								  pCS->dwComponentStateInfoSize,
								  &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != pCS->dwComponentStateInfoSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
	
			if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
			{
 				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
    }

ON_ERROR:
    pIStream->Release();

	return hr;
}


//////////////////////////////////////////////////////////////////////
// CBookmark OpenEditor

BOOL CBookmark::OpenEditor( wpWindowPlacement* pWP )
{
	CFileNode* pFileNode = NULL;
	CTreeCtrl* pTreeCtrl;
	HTREEITEM hItem;

	BOOL fSuccess = FALSE;

	// Get the FileNode
	pFileNode = theApp.GetFileByGUID( pWP->guidFile );
	if( pFileNode == NULL )
	{
		goto ON_ERR;
	}

	// Make sure the FileNode's file is loaded
	if( pFileNode->m_pIChildNode == NULL )
	{
		CString strFileName;
		pFileNode->ConstructFileName( strFileName );
		
		// Open the file
		theApp.m_nShowNodeInTree++;
		theApp.OpenTheFile( strFileName, TGT_FILENODE );
		theApp.m_nShowNodeInTree--;
	}
	if( pFileNode->m_pIChildNode == NULL )
	{
		goto ON_ERR;
	}

	// Find the corresponding node in the Project Tree
	hItem = theApp.m_pFramework->FindTreeItemByWP( pWP );
	if( hItem == NULL )
	{
		goto ON_ERR;
	}
		
	// Apply WP Settings to the node
	theApp.m_pFramework->ApplyWPSettings( hItem, pWP );

	// Open the Node's Editor
	pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			IDMUSProdNode* pINode = pJzNode->m_pINode;
			ASSERT( pINode != NULL );
	
			theApp.m_pFramework->OpenEditor( pINode );
	
			fSuccess = TRUE;
		}
	}

ON_ERR:
	if( pFileNode )
	{
		pFileNode->Release();
	}

	return fSuccess;
}


//////////////////////////////////////////////////////////////////////
// CBookmark SetWP

void CBookmark::SetWP( void )
{
	CDocTemplate* pTemplate;
	CComponentDoc* pComponentDoc;
	CComponentView* pComponentView;
	CFrameWnd* pFrame;
	HTREEITEM hItem;
	CJzNode* pJzNode;

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return;
	}

	if( theApp.m_pDocManager )
	{
		// walk all templates in the application
		POSITION pos = theApp.m_pDocManager->GetFirstDocTemplatePosition();
		while( pos != NULL )
		{
			pTemplate = theApp.m_pDocManager->GetNextDocTemplate( pos );
			ASSERT_VALID( pTemplate );
			ASSERT_KINDOF( CDocTemplate, pTemplate );

			// walk all documents in the template
			POSITION pos2 = pTemplate->GetFirstDocPosition();
			while( pos2 )
			{
				pComponentDoc = (CComponentDoc *)pTemplate->GetNextDoc( pos2 );
				ASSERT_VALID( pComponentDoc );

				if( pComponentDoc->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
				{
					POSITION pos3 = pComponentDoc->GetFirstViewPosition();
					while( pos3 )
					{
						pComponentView = (CComponentView *)pComponentDoc->GetNextView( pos3 );
						ASSERT_VALID( pComponentView );

						if( pComponentView
						&&  pComponentView->IsKindOf( RUNTIME_CLASS(CComponentView) ) )
						{
							if( pComponentView->m_fSeed == FALSE )
							{
								if( pComponentView->m_pINode )
								{
									hItem = theApp.m_pFramework->FindTreeItem( pComponentView->m_pINode );
									if( hItem )
									{
										pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
										if( pJzNode )
										{
											if( pJzNode->m_wp.length != 0 )
											{
												pJzNode->m_wp.length = sizeof(pJzNode->m_wp);
												pFrame = pComponentView->GetParentFrame();
												pFrame->SetWindowPlacement( &pJzNode->m_wp );
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
// CBookmark Apply

void CBookmark::Apply( BOOL fCloseWindows )
{
	// Close all open Editors
	if( fCloseWindows )
	{
		theApp.OnWindowCloseAll();
	}

	// Restore state of Components
	IDMUSProdComponent* pIComponent;
	IPersistStream* pIPS;
	IStream* pIMemStream;
	bkComponentState* pCS;
	DWORD dwBytesWritten;

    POSITION pos = m_lstComponents.GetHeadPosition();
    while( pos )
    {
		pCS = m_lstComponents.GetNext( pos );

		if( SUCCEEDED ( theApp.m_pFramework->FindComponent( pCS->clsidComponent, &pIComponent) ) )
		{
			if( SUCCEEDED ( pIComponent->QueryInterface( IID_IPersistStream, (void**)&pIPS ) ) )
			{
				// Create a memory stream
				if( SUCCEEDED ( AllocMemoryStream ( FT_DESIGN, GUID_Bookmark, &pIMemStream ) ) )
				{
					// Place component state information in the stream
					if( SUCCEEDED ( pIMemStream->Write( pCS->pComponentStateInfo, pCS->dwComponentStateInfoSize, &dwBytesWritten ) )
					&&  dwBytesWritten == pCS->dwComponentStateInfoSize )
					{
						// Hand state info to the Component
						StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
						pIPS->Load( pIMemStream );
					}

					pIMemStream->Release();
				}

				pIPS->Release();
			}

			pIComponent->Release();
		}
	}


	// Open all Editors in the Editor list
	wpWindowPlacement* pWP;
    
	pos = m_lstEditors.GetHeadPosition();
    while( pos )
    {
		pWP = m_lstEditors.GetNext( pos );

		OpenEditor( pWP );
	}

	// Now that all Editors are open,
	// reapply WP to get correct minimized/maximized state
	SetWP();
}


//////////////////////////////////////////////////////////////////////
// CBookmark IsForThisProject

BOOL CBookmark::IsForThisProject( CProject* pProject )
{
	wpWindowPlacement* pWP;
	CFileNode* pFileNode;

    POSITION pos = m_lstEditors.GetHeadPosition();
    while( pos )
    {
		pWP = m_lstEditors.GetNext( pos );

		pFileNode = pProject->GetFileByGUID( pWP->guidFile );
		if( pFileNode )
		{
			pFileNode->Release();
			return TRUE;
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// CBookmark IsValid

BOOL CBookmark::IsValid( void )
{
	wpWindowPlacement* pWP;
	CFileNode* pFileNode;

    POSITION pos = m_lstEditors.GetHeadPosition();
    while( pos )
    {
		pWP = m_lstEditors.GetNext( pos );

		pFileNode = theApp.GetFileByGUID( pWP->guidFile );
		if( pFileNode )
		{
			pFileNode->Release();
			return TRUE;
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// CBookmark MergeBookmark

void CBookmark::MergeBookmark( CBookmark* pBookmark )
{
	wpWindowPlacement* pWP;

	while( !pBookmark->m_lstEditors.IsEmpty() )
	{
		// Remove editor from pBookmark
		pWP = static_cast<wpWindowPlacement*>( pBookmark->m_lstEditors.RemoveHead() );

		// Add editor to this bookmark
		m_lstEditors.AddTail( pWP );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBookmark AdjustFileReferenceChunks

void CBookmark::AdjustFileReferenceChunks( IDMUSProdRIFFStream* pIRIFFStream, MMCKINFO* pckMain, BOOL* pfChanged )
{
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
	DWORD dwPos;
	ioFileRef iFileRef;
			
	if( theApp.m_fInDuplicateProject == FALSE )
	{
		// Only call when we are duplicating a Project
		// because this method depends on existence
		// of m_lstFileGUIDs.
		ASSERT( 0 );
		return;
	}

    IStream* pIStream = pIRIFFStream->GetStream();
	ASSERT( pIStream != NULL );

	// Rewrite all file reference chunks written by the Framework
	while( pIRIFFStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_DMUSPROD_FILEREF:
				dwSize = min( ck.cksize, sizeof( ioFileRef ) );

				// Read the FileRef chunk data
				dwPos = StreamTell( pIStream );
				if( SUCCEEDED ( pIStream->Read( &iFileRef, dwSize, &dwByteCount ) ) 
				&&  dwByteCount == dwSize )
				{
					// Get the new GUID
					GUID guidNewFile;
					if( theApp.GetNewGUIDForDuplicateFile( iFileRef.guidFile, &guidNewFile ) )
					{
						memcpy( &iFileRef.guidFile, &guidNewFile, sizeof( iFileRef.guidFile ) );

						// Rewrite the reference chunk with the new GUID
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						if( SUCCEEDED ( pIStream->Write( &iFileRef, dwSize, &dwByteCount ) ) 
						&&  dwByteCount == dwSize )
						{
							*pfChanged = TRUE;
						}
					}
				}
				break;

			case FOURCC_RIFF:
			case FOURCC_LIST:
				AdjustFileReferenceChunks( pIRIFFStream, &ck, pfChanged );
				break;


			default:
				break;
		}

		pIRIFFStream->Ascend( &ck, 0 );
	}

	pIStream->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CBookmark Duplicate

void CBookmark::Duplicate( void )
{
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( theApp.m_fInDuplicateProject == FALSE )
	{
		// Only call when we are duplicating a Project
		// because this method depends on existence
		// of m_lstFileGUIDs.
		ASSERT( 0 );
		return;
	}

	// Duplicate the Bookmark
	CBookmark* pBookmark = new CBookmark;
	if( pBookmark )
	{
		// Set the name
		pBookmark->m_strName = m_strName;

		// Duplicate the editors
		wpWindowPlacement* pWPList;
		wpWindowPlacement* pWP;
		POSITION pos = m_lstEditors.GetHeadPosition();
		while( pos )
		{
			pWPList = m_lstEditors.GetNext( pos );

			pWP = new wpWindowPlacement;
			if( pWP )
			{
				*pWP = *pWPList;

				GUID guidNewFile;
				if( theApp.GetNewGUIDForDuplicateFile( pWP->guidFile, &guidNewFile ) )
				{
					memcpy( &pWP->guidFile, &guidNewFile, sizeof( pWP->guidFile ) );
				}

				pBookmark->m_lstEditors.AddTail( pWP );
			}
		}

		// Duplicate the component state information
		bkComponentState* pCSList;
		bkComponentState* pCS;
		pos = m_lstComponents.GetHeadPosition();
		while( pos )
		{
			pCSList = m_lstComponents.GetNext( pos );

			pCS = new bkComponentState; 
			if( pCS )
			{
				// Store the Component's CLSID
				pCS->clsidComponent = pCSList->clsidComponent;

				// Store the Component's state info
				pCS->dwComponentStateInfoSize = pCSList->dwComponentStateInfoSize;
				pCS->pComponentStateInfo = (BYTE *)GlobalAlloc( GPTR, pCS->dwComponentStateInfoSize );
				if( pCS->pComponentStateInfo )
				{
					memcpy( pCS->pComponentStateInfo, pCSList->pComponentStateInfo, pCS->dwComponentStateInfoSize );

					// If RIFF format, fix all file reference chunks written by the Framework
					if( memcmp( pCS->pComponentStateInfo, "RIFF", 4 ) == 0
					||  memcmp( pCS->pComponentStateInfo, "LIST", 4 ) == 0 )
					{
						// Get a memory stream
						IStream* pIMemStream;
						if( SUCCEEDED ( theApp.m_pFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIMemStream ) ) )
						{
							// Get a RIFF stream
							IDMUSProdRIFFStream* pIRIFFStream;
							if( SUCCEEDED ( AllocRIFFStream( pIMemStream, &pIRIFFStream ) ) )
							{
								DWORD dwByteCount;

								// Place pCS->pComponentStateInfo into a stream
								if( SUCCEEDED ( pIMemStream->Write( pCS->pComponentStateInfo, pCS->dwComponentStateInfoSize, &dwByteCount ) ) 
								&&  dwByteCount == pCS->dwComponentStateInfoSize )
								{
									StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );

									BOOL fChanged = FALSE;

									AdjustFileReferenceChunks( pIRIFFStream, NULL, &fChanged );
									if( fChanged )
									{
										StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
										pIMemStream->Read( pCS->pComponentStateInfo, pCS->dwComponentStateInfoSize, &dwByteCount );
									}
								}

								pIRIFFStream->Release();
							}

							pIMemStream->Release();
						}
					}

					pBookmark->m_lstComponents.AddTail( pCS );
				}
				else
				{
					delete pCS;
				}
			}
		}

		// Add the Bookmark to the list of Bookmarks
		if( pMainFrame->m_wndBookmarkToolBar.AddBookmark( pBookmark, FALSE ) == FALSE )
		{
			delete pBookmark;
		}
	}
}

