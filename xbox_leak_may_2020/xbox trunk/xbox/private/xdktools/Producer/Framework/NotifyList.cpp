// NotifyList.cpp: implementation of the CNotifyList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "Framework.h"
#include "NotifyList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotifyList::CNotifyList()
{
	m_pIOrigDocRootNode = NULL;
	m_pINewDocRootNode = NULL;
}

CNotifyList::~CNotifyList()
{
	CJzNotifyNode* pJzNotifyNode;
	while( !m_lstNotifyNodes.IsEmpty() )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( m_lstNotifyNodes.RemoveHead() );
		delete pJzNotifyNode;
	}

	if( m_pIOrigDocRootNode )
	{
		m_pIOrigDocRootNode->Release();
	}

	if( m_pINewDocRootNode )
	{
		m_pINewDocRootNode->Release();
	}
}


//////////////////////////////////////////////////////////////////////
// CNotifyList::Detach

HRESULT CNotifyList::Detach( CFileNode* pFileNode )
{
	if( pFileNode == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pIOrigDocRootNode )
	{
		// Cannot call Detach() more than once
		return E_UNEXPECTED;
	}

	// Store original DocRoot node
	m_pIOrigDocRootNode = pFileNode->m_pIChildNode;
	if( m_pIOrigDocRootNode == NULL )
	{
		return E_FAIL;
	}

	m_pIOrigDocRootNode->AddRef();

	// Move notify list from pFileNode object to CNotifyList object
	CJzNotifyNode* pJzNotifyNode;
	while( !pFileNode->m_lstNotifyNodes.IsEmpty() )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.RemoveHead() );
		m_lstNotifyNodes.AddTail( pJzNotifyNode );
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// CNotifyList::Attach

HRESULT CNotifyList::Attach( CFileNode* pFileNode )
{
	if( pFileNode == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pIOrigDocRootNode == NULL )
	{
		// Must call Detach() before calling Attach()
		return E_UNEXPECTED;
	}

	// Store new DocRoot node
	if( m_pINewDocRootNode )
	{
		m_pINewDocRootNode->Release();
	}
	m_pINewDocRootNode = pFileNode->m_pIChildNode;
	if( m_pINewDocRootNode == NULL )
	{
		return E_FAIL;
	}

	m_pINewDocRootNode->AddRef();

	// Notify CFileNode object that file has been replaced
	IDMUSProdNotifySink* pINotifySink;
	CJzNotifyNode* pJzNotifyNode;
	while( !m_lstNotifyNodes.IsEmpty() )
	{
		pJzNotifyNode = static_cast<CJzNotifyNode*>( m_lstNotifyNodes.RemoveHead() );

		if( m_pINewDocRootNode != m_pIOrigDocRootNode )
		{
			// Notify interested node that the DocRoot pointer has changed
			if( pJzNotifyNode->m_pINotifyThisNode )
			{
				if( SUCCEEDED ( pJzNotifyNode->m_pINotifyThisNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
				{
					pINotifySink->OnUpdate( m_pIOrigDocRootNode, FRAMEWORK_FileReplaced, m_pINewDocRootNode );
					pINotifySink->Release();
				}
			}
		}

		delete pJzNotifyNode;
	}

	return S_OK;
}
