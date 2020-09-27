#include "stdafx.h"
#include "PropTempo.h"
#include "TempoIO.h"

CPropTempo::CPropTempo()
{
	Clear();
}


CPropTempo::CPropTempo(const CTempoItem *pTempoItem)
{
	m_lMeasure = pTempoItem->m_lMeasure;
	m_lBeat = pTempoItem->m_lBeat;
	m_lOffset = pTempoItem->m_lOffset;
	m_dblTempo = pTempoItem->m_dblTempo;
	m_mtTime = pTempoItem->m_mtTime;
	m_dwBits = 0;
	m_wFlags = 0;
}

void CPropTempo::Clear()
{
	m_lMeasure = 0;
	m_lBeat = 0;
	m_lOffset = 0;
	m_dblTempo = 120;
	m_mtTime = 0;
	m_dwBits = 0;
	m_wFlags = 0;
}

void CPropTempo::ApplyToTempoItem( CTempoItem *pTempoItem ) const
{
	ASSERT( pTempoItem != NULL );
	if ( pTempoItem == NULL )
	{
		return;
	}

	//pTempoItem->m_lMeasure = m_lMeasure;
	//pTempoItem->m_lBeat = m_lBeat;
	//pTempoItem->m_dwOffset = m_dwOffset;
	pTempoItem->m_dblTempo = m_dblTempo;
	//pTempoItem->m_mtTime = m_mtTime;
}

void CPropTempo::Copy( const CPropTempo* pPropTempo )
{
	ASSERT( pPropTempo != NULL );
	if ( pPropTempo == NULL )
	{
		return;
	}

	if( pPropTempo == this )
	{
		return;
	}

	m_lMeasure = pPropTempo->m_lMeasure;
	m_lBeat = pPropTempo->m_lBeat;
	m_lOffset = pPropTempo->m_lOffset;
	m_dblTempo = pPropTempo->m_dblTempo;
	m_mtTime = pPropTempo->m_mtTime;
	m_dwBits = pPropTempo->m_dwBits;
	m_wFlags = pPropTempo->m_wFlags;
}
