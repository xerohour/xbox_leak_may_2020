#include "stdafx.h"
#include "PersRefIO.h"
#include "ChordMapRefStripMgr.h"
#include "PersRefMgr.h"


CPersRefItem::CPersRefItem( CPersRefMgr* pPersRefMgr )
{
	ASSERT( pPersRefMgr != NULL );
	m_pPersRefMgr = pPersRefMgr;

	m_fSelected = FALSE;
	m_fRemoveNotify = FALSE;
}

CPersRefItem::~CPersRefItem()
{
	// Turn off notifications for this node
	if( m_pIPersDocRootNode )
	{
		if( m_fRemoveNotify )
		{
			if( m_pPersRefMgr
			&&  m_pPersRefMgr->m_pISegmentNode 
			&&  m_pPersRefMgr->m_pDMProdFramework )
			{
				m_pPersRefMgr->m_pDMProdFramework->RemoveFromNotifyList( m_pIPersDocRootNode,
																		 m_pPersRefMgr->m_pISegmentNode );
			}
		}
	}
}

BOOL CPersRefItem::After(const CPersRefItem& PersRef)
{
	if( m_dwMeasure > PersRef.m_dwMeasure )
	{
		return TRUE;
	}
	else if( m_dwMeasure == PersRef.m_dwMeasure )
	{
		if( m_bBeat > PersRef.m_bBeat )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPersRefItem::Before(const CPersRefItem& PersRef)
{
	if( m_dwMeasure < PersRef.m_dwMeasure )
	{
		return TRUE;
	}
	else if( m_dwMeasure == PersRef.m_dwMeasure )
	{
		if( m_bBeat < PersRef.m_bBeat )
		{
			return TRUE;
		}
	}
	return FALSE;
}
