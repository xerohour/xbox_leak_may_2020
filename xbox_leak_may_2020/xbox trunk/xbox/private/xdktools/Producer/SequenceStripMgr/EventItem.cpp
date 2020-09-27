#include "stdafx.h"
#include "EventItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEventItem::CEventItem()
{
	Clear();
}

void CEventItem::Clear()
{
	m_mtTime = 0;
	m_mtDuration = 0;
	m_nOffset = 0;

	m_dwBits = 0;
	m_fSelected = FALSE;
}

void CEventItem::Copy( const CEventItem* pEventItem )
{
	ASSERT( pEventItem != NULL );
	if( (pEventItem == NULL) || (pEventItem == this) )
	{
		return;
	}
	m_mtTime = pEventItem->m_mtTime;
	m_mtDuration = pEventItem->m_mtDuration;
	m_nOffset = pEventItem->m_nOffset;

	m_dwBits = pEventItem->m_dwBits;
	m_fSelected = pEventItem->m_fSelected;
}

bool CEventItem::CopyTo(DMUS_IO_SEQ_ITEM& item)
{
	item.mtTime = m_mtTime;
	item.mtDuration = m_mtDuration;
	item.nOffset = m_nOffset;

	return true;
}

BOOL CEventItem::IsEqual( const CEventItem *pEventItem ) const
{
	ASSERT( pEventItem != NULL );
	if( (pEventItem != NULL) &&
		(m_mtTime == pEventItem->m_mtTime) &&
		(m_mtDuration == pEventItem->m_mtDuration) &&
		(m_nOffset == pEventItem->m_nOffset) )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CEventItem::After(const CEventItem& Item) const
{
	if( AbsTime() > Item.AbsTime() )
	{
		return TRUE;
	}
	else // if( AbsTime() <= Item.AbsTime() )
	{
		return FALSE;
	}
}
BOOL CEventItem::Before(const CEventItem& Item) const
{
	if( AbsTime() < Item.AbsTime() )
	{
		return TRUE;
	}
	else // if ( AbsTime() >= Item.AbsTime() )
	{
		return FALSE;
	}
}