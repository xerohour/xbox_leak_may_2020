#include "stdafx.h"
#include "StyleRefIO.h"
#include "StyleRefStripMgr.h"
#include "StyleRefMgr.h"


CStyleRefItem::CStyleRefItem( CStyleRefMgr* pStyleRefMgr )
{
	ASSERT( pStyleRefMgr != NULL );
	m_pStyleRefMgr = pStyleRefMgr;

	m_fSelected = FALSE;
	m_fRemoveNotify = FALSE;
}

CStyleRefItem::~CStyleRefItem()
{
	// Turn off notifications for this node
	if( m_pIStyleDocRootNode )
	{
		if( m_fRemoveNotify )
		{
			if( m_pStyleRefMgr
			&&  m_pStyleRefMgr->m_pISegmentNode 
			&&  m_pStyleRefMgr->m_pDMProdFramework )
			{
				m_pStyleRefMgr->m_pDMProdFramework->RemoveFromNotifyList( m_pIStyleDocRootNode,
																		  m_pStyleRefMgr->m_pISegmentNode );
			}
		}
	}
}

BOOL CStyleRefItem::After(const CStyleRefItem& StyleRef)
{
	if( m_dwMeasure > StyleRef.m_dwMeasure )
	{
		return TRUE;
	}
	/*
	else if( m_dwMeasure == StyleRef.m_dwMeasure )
	{
	}
	*/
	return FALSE;
}

BOOL CStyleRefItem::Before(const CStyleRefItem& StyleRef)
{
	if( m_dwMeasure < StyleRef.m_dwMeasure )
	{
		return TRUE;
	}
	/*
	else if( m_dwMeasure == StyleRef.m_dwMeasure )
	{
	}
	*/
	return FALSE;
}
	
void CStyleRefItem::SetSelectFlag( BOOL fSelected )
{
	m_fSelected = fSelected;
	m_dwBits = 0;
}
