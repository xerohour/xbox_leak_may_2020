#include "stdafx.h"
#include "SequenceIO.h"
#include "midifileio.h"
#include "SeqSegmentRiff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSequenceItem::CSequenceItem()
{
	Clear();
}
/*
CSequenceItem::CSequenceItem(const CSequenceItem& Sequence)
{
	*((CSequenceItem*)this) = Sequence;
}

CSequenceItem::CSequenceItem(const CSequenceItem *pSequenceItem)
{
	Copy( pSequenceItem );
}
*/

CSequenceItem::CSequenceItem(const FullSeqEvent* pEvent)
{
	CopyFrom(pEvent);
}

CSequenceItem& CSequenceItem::operator=(const CSequenceItem& Sequence)
{
	Copy( &Sequence );
	return *this;
}

CSequenceItem& CSequenceItem::operator=(const CCurveItem& Curve)
{
	CEventItem::Copy( &Curve );

	ASSERT( Curve.m_bCurveShape == DMUS_CURVES_INSTANT );

	switch( Curve.m_bType )
	{
	case DMUS_CURVET_PBCURVE:
		m_bStatus = MIDI_PBEND;
		m_bByte1 = (BYTE)(Curve.m_nEndValue & 0x7f);
		m_bByte2 = (BYTE)((Curve.m_nEndValue >> 7) & 0x7F);
		break;
	case DMUS_CURVET_PATCURVE:
		m_bStatus = MIDI_PTOUCH;
		m_bByte1 = Curve.m_bCCData;

		if( Curve.m_nEndValue > 127 )
		{
			ASSERT( FALSE );
			m_bByte2 = 127;
		}
		else
		{
			m_bByte2 = BYTE(Curve.m_nEndValue);
		}
		break;
	case DMUS_CURVET_MATCURVE:
		m_bStatus = MIDI_MTOUCH;
		m_bByte2 = 0;

		if( Curve.m_nEndValue > 127 )
		{
			ASSERT( FALSE );
			m_bByte1 = 127;
		}
		else
		{
			m_bByte1 = BYTE(Curve.m_nEndValue);
		}
		break;
	case DMUS_CURVET_CCCURVE:
		m_bStatus = MIDI_CCHANGE;
		m_bByte1 = Curve.m_bCCData;

		if( Curve.m_nEndValue > 127 )
		{
			ASSERT( FALSE );
			m_bByte2 = 127;
		}
		else
		{
			m_bByte2 = BYTE(Curve.m_nEndValue);
		}
		break;
	case DMUS_CURVET_RPNCURVE:
		// TODO: Implement RPN curves
		//bStripCCType = CCTYPE_RPN_CURVE_STRIP;
		ASSERT(FALSE);
		break;
	case DMUS_CURVET_NRPNCURVE:
		// TODO: Implement NRPN curves
		//bStripCCType = CCTYPE_NRPN_CURVE_STRIP;
		ASSERT(FALSE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return *this;
}

void CSequenceItem::Clear()
{
	CEventItem::Clear();

	m_bByte1 = 0;
	m_bByte2 = 0;
	m_bStatus = 0;
	m_dwMIDISaveData = 0;
}

BOOL CSequenceItem::IsEqual( const CSequenceItem *pSequenceItem )
{
	if( CEventItem::IsEqual( pSequenceItem ) &&
		( m_bByte1 == pSequenceItem->m_bByte1 ) &&
		( m_bByte2 == pSequenceItem->m_bByte2 ) &&
		( m_bStatus == pSequenceItem->m_bStatus ) &&
		( m_dwMIDISaveData == pSequenceItem->m_dwMIDISaveData ) )
	{
		return TRUE;
	}
	return FALSE;
}

void CSequenceItem::Copy( const CSequenceItem* pSequenceItem )
{
	ASSERT( pSequenceItem != NULL );
	if( pSequenceItem == NULL )
	{
		Clear();
		return;
	}

	if ( pSequenceItem == this )
	{
		return;
	}

	CEventItem::Copy( pSequenceItem );

	m_bStatus = pSequenceItem->m_bStatus;
	m_bByte1 = pSequenceItem->m_bByte1;
	m_bByte2 = pSequenceItem->m_bByte2;
	m_dwMIDISaveData = pSequenceItem->m_dwMIDISaveData;
}

void CSequenceItem::CopyFrom(const FullSeqEvent* pEvent)
{
	Clear();

	// If it really is a control change, import as a curve event
	if( (Status(pEvent->bStatus) == MIDI_PBEND)
	||  (Status(pEvent->bStatus) == MIDI_PTOUCH)
	||  (Status(pEvent->bStatus) == MIDI_MTOUCH)
	||  (Status(pEvent->bStatus) == MIDI_CCHANGE) )
	{
		ASSERT(FALSE);
	}
	else
	{
		m_mtTime = pEvent->mtTime;
		m_nOffset = pEvent->nOffset;
		m_mtDuration = pEvent->mtDuration;

		m_bStatus = Status(pEvent->bStatus);
		ASSERT( m_bStatus == MIDI_NOTEON );
		m_bByte1 = pEvent->bByte1;
		m_bByte2 = pEvent->bByte2;
	}
}

/*
void CSequenceItem::CopyFrom(const DMUS_IO_SEQ_ITEM& item)
{
	Clear();

	m_mtTime = item.mtTime;
	m_nOffset = item.nOffset;
	m_mtDuration = item.mtDuration;

	m_bStatus = Status(item.bStatus);
	ASSERT( m_bStatus == MIDI_NOTEON );
	m_bByte1 = item.bByte1;
	m_bByte2 = item.bByte2;
}
*/

void CSequenceItem::CopyTo(DMUS_IO_SEQ_ITEM& item)
{
	CEventItem::CopyTo(item);

	item.bStatus = m_bStatus;
	item.bByte1 = m_bByte1;
	item.bByte2 = m_bByte2;
}

/*
void MergeCSequence(CTypedPtrList<CPtrList, CSequenceItem*>& lst1,
					const CTypedPtrList<CPtrList, CSequenceItem*>& lst2)
{
	if(lst1.IsEmpty())
	{
		POSITION pos = lst2.GetHeadPosition();
		while(pos)
		{
			CSequenceItem* pItem = lst2.GetNext(pos);
			lst1.AddTail(pItem);
		}
	}
	else if(lst2.IsEmpty())
	{
		// nothing to do
	}
	else
	{
		// merge two lists
		POSITION pos1, pos2;
		pos1 = lst1.GetHeadPosition();
		pos2 = lst2.GetHeadPosition();
		CSequenceItem* p1 = lst1.GetAt(pos1);
		while(pos2)
		{
			CSequenceItem* p2 = lst2.GetNext(pos2);
			while(pos1 && p1->Before(*p2))
			{
				p1 = lst1.GetNext(pos1);
			}
			if(pos1)
			{
				lst1.InsertBefore(pos1, p2);
			}
			else
			{
				lst1.AddTail(p2);
			}
		}
	}
}
*/