#include "stdafx.h"
#include "PropPersRef.h"
#include "PersRefIO.h"

CPropPersRef::CPropPersRef()
{
	m_mtTime = 0;
	m_dwMeasure = 0;
	m_bBeat = 0;
	m_dwBits = 0;

	m_PersListInfo.pIProject = NULL;
	m_PersListInfo.strProjectName = "Empty";
	m_PersListInfo.strName = "Empty";
	m_PersListInfo.strDescriptor = "Empty";
	
	m_pIPersDocRootNode = NULL;
	memset( &m_guidProject, 0, sizeof( m_guidProject ) );
}

CPropPersRef::CPropPersRef( const CPersRefItem *pPersRefItem )
{
	ASSERT( pPersRefItem != NULL );

	m_mtTime = pPersRefItem->m_mtTime;
	m_dwMeasure = pPersRefItem->m_dwMeasure;
	m_bBeat = pPersRefItem->m_bBeat;
	m_dwBits = 0;

	m_PersListInfo.pIProject = pPersRefItem->m_PersListInfo.pIProject;
	m_PersListInfo.strProjectName = pPersRefItem->m_PersListInfo.strProjectName;
	m_PersListInfo.strName = pPersRefItem->m_PersListInfo.strName;
	m_PersListInfo.strDescriptor = pPersRefItem->m_PersListInfo.strDescriptor;

	m_pIPersDocRootNode = pPersRefItem->m_pIPersDocRootNode;
	if( m_pIPersDocRootNode )
	{
		m_pIPersDocRootNode->AddRef();
	}

	m_guidProject = pPersRefItem->m_guidProject;
}

CPropPersRef::~CPropPersRef() 
{
	if( m_pIPersDocRootNode )
	{
		m_pIPersDocRootNode->Release();
		m_pIPersDocRootNode = NULL;
	}
}

void CPropPersRef::ApplyToPersRefItem( CPersRefItem *pPersRefItem )
{
	ASSERT( pPersRefItem != NULL );
	if( pPersRefItem == NULL )
	{
		return;
	}

	pPersRefItem->m_mtTime = m_mtTime;
	pPersRefItem->m_dwMeasure = m_dwMeasure;
	pPersRefItem->m_bBeat = m_bBeat;

	pPersRefItem->m_PersListInfo.pIProject = m_PersListInfo.pIProject;
	pPersRefItem->m_PersListInfo.strProjectName = m_PersListInfo.strProjectName;
	pPersRefItem->m_PersListInfo.strName = m_PersListInfo.strName;
	pPersRefItem->m_PersListInfo.strDescriptor = m_PersListInfo.strDescriptor;
	
	if( pPersRefItem->m_pIPersDocRootNode )
	{
		pPersRefItem->m_pIPersDocRootNode->Release();
	}
	pPersRefItem->m_pIPersDocRootNode = m_pIPersDocRootNode;
	if( pPersRefItem->m_pIPersDocRootNode )
	{
		pPersRefItem->m_pIPersDocRootNode->AddRef();
	}

	pPersRefItem->m_guidProject = m_guidProject;
}

