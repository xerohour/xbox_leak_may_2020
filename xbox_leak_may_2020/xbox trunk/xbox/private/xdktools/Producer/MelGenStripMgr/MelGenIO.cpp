#include "stdafx.h"
#include "MelGenIO.h"
#include "MelGenStripMgr.h"
#include "MelGenMgr.h"


CMelGenItem::CMelGenItem( CMelGenMgr* pMelGenMgr )
{
	ASSERT( pMelGenMgr != NULL );
	m_pMelGenMgr = pMelGenMgr;

	m_fSelected = FALSE;
	m_fRemoveNotify = FALSE;
}

CMelGenItem::~CMelGenItem()
{
}

BOOL CMelGenItem::After(const CMelGenItem& MelGen)
{
	if( m_dwMeasure > MelGen.m_dwMeasure )
	{
		return TRUE;
	}
	else if( m_dwMeasure == MelGen.m_dwMeasure )
	{
		if( m_bBeat > MelGen.m_bBeat )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMelGenItem::Before(const CMelGenItem& MelGen)
{
	if( m_dwMeasure < MelGen.m_dwMeasure )
	{
		return TRUE;
	}
	else if( m_dwMeasure == MelGen.m_dwMeasure )
	{
		if( m_bBeat < MelGen.m_bBeat )
		{
			return TRUE;
		}
	}
	return FALSE;
}
	
void CMelGenItem::SetSelectFlag( BOOL fSelected )
{
	m_fSelected = fSelected;
	m_dwBits &= ~(UD_DRAGSELECT | UD_CURRENTSELECTION);
}
