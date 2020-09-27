#include "stdafx.h"
#include "resource.h"
#include "PropStyleRef.h"
#include "StyleRefIO.h"

CPropStyleRef::CPropStyleRef()
{
	m_dwMeasure = 0;
	m_dwBits = 0;

	m_StyleListInfo.pIProject = NULL;
	m_StyleListInfo.strProjectName.LoadString( IDS_EMPTY_TEXT );
	m_StyleListInfo.strName.LoadString( IDS_EMPTY_TEXT );
	m_StyleListInfo.strDescriptor.LoadString( IDS_EMPTY_TEXT );

	m_pIStyleDocRootNode = NULL;

	m_TimeSignature.mtTime = 0;
	m_TimeSignature.bBeatsPerMeasure = 4;
	m_TimeSignature.bBeat = 4;
	m_TimeSignature.wGridsPerBeat = 4;
	
	memset( &m_guidProject, 0, sizeof( m_guidProject ) );
}

CPropStyleRef::CPropStyleRef(const CStyleRefItem *pStyleRefItem)
{
	ASSERT( pStyleRefItem != NULL );

	m_dwMeasure = pStyleRefItem->m_dwMeasure;
	m_dwBits = 0;

	m_StyleListInfo.pIProject = pStyleRefItem->m_StyleListInfo.pIProject;
	m_StyleListInfo.strProjectName = pStyleRefItem->m_StyleListInfo.strProjectName;
	m_StyleListInfo.strName = pStyleRefItem->m_StyleListInfo.strName;
	m_StyleListInfo.strDescriptor = pStyleRefItem->m_StyleListInfo.strDescriptor;

	m_pIStyleDocRootNode = pStyleRefItem->m_pIStyleDocRootNode;
	if( m_pIStyleDocRootNode )
	{
		m_pIStyleDocRootNode->AddRef();
	}

	m_guidProject = pStyleRefItem->m_guidProject;
}

CPropStyleRef::~CPropStyleRef( ) 
{ 
	if( m_pIStyleDocRootNode )
	{
		m_pIStyleDocRootNode->Release(); 
		m_pIStyleDocRootNode = NULL; 
	}
}

void CPropStyleRef::ApplyToStyleRefItem( CStyleRefItem *pStyleRefItem )
{
	ASSERT( pStyleRefItem != NULL );
	if ( pStyleRefItem == NULL )
	{
		return;
	}

	pStyleRefItem->m_dwMeasure = m_dwMeasure;

	pStyleRefItem->m_StyleListInfo.pIProject = m_StyleListInfo.pIProject;
	pStyleRefItem->m_StyleListInfo.strProjectName = m_StyleListInfo.strProjectName;
	pStyleRefItem->m_StyleListInfo.strName = m_StyleListInfo.strName;
	pStyleRefItem->m_StyleListInfo.strDescriptor = m_StyleListInfo.strDescriptor;

	if( pStyleRefItem->m_pIStyleDocRootNode )
	{
		pStyleRefItem->m_pIStyleDocRootNode->Release();
	}
	pStyleRefItem->m_pIStyleDocRootNode = m_pIStyleDocRootNode;
	if( pStyleRefItem->m_pIStyleDocRootNode )
	{
		pStyleRefItem->m_pIStyleDocRootNode->AddRef();
	}

	pStyleRefItem->m_guidProject = m_guidProject;
}

