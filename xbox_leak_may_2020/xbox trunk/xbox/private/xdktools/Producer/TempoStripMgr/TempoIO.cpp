#include "stdafx.h"
#include "TempoIO.h"



CTempoItem::CTempoItem()

{
	m_fSelected = FALSE;
	m_strText.Empty();
}

CTempoItem::CTempoItem(const CTempoItem& Tempo)
{
	m_fSelected = Tempo.m_fSelected;
	m_strText = Tempo.m_strText;
	*((CPropTempo*)this) = Tempo;
}

BOOL CTempoItem::After(const CTempoItem& Tempo)
{
	if( m_mtTime > Tempo.m_mtTime )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CTempoItem::Before(const CTempoItem& Tempo)
{
	if( m_mtTime < Tempo.m_mtTime )
	{
		return TRUE;
	}
	return FALSE;
}
