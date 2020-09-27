#include "stdafx.h"
#include "TimeSigIO.h"
#include "TimeSigStripMgr.h"
#include "TimeSigMgr.h"


CTimeSigItem::CTimeSigItem( CTimeSigMgr* pTimeSigMgr )
{
	ASSERT( pTimeSigMgr != NULL );
	m_pTimeSigMgr = pTimeSigMgr;

	m_fSelected = FALSE;
	m_fRemoveNotify = FALSE;
}

CTimeSigItem::~CTimeSigItem()
{
}

BOOL CTimeSigItem::After(const CTimeSigItem& TimeSig)
{
	if( m_dwMeasure > TimeSig.m_dwMeasure )
	{
		return TRUE;
	}
	/*
	else if( m_dwMeasure == TimeSig.m_dwMeasure )
	{
	}
	*/
	return FALSE;
}

BOOL CTimeSigItem::Before(const CTimeSigItem& TimeSig)
{
	if( m_dwMeasure < TimeSig.m_dwMeasure )
	{
		return TRUE;
	}
	/*
	else if( m_dwMeasure == TimeSig.m_dwMeasure )
	{
	}
	*/
	return FALSE;
}
	
void CTimeSigItem::SetSelectFlag( BOOL fSelected )
{
	m_fSelected = fSelected;
	m_dwBits &= ~(UD_DRAGSELECT);
}
