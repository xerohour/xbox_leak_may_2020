#include "stdafx.h"
#include "PropSequence.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPropSequence::CPropSequence() : CPropItem()
{
	Clear();
}

CPropSequence::CPropSequence( CSequenceMgr *pSequenceMgr ) : CPropItem( pSequenceMgr )
{
	Clear();
}

CPropSequence::CPropSequence(const CSequenceItem& SeqItem, CSequenceMgr* pSequenceMgr)
{
	m_pSequenceMgr = pSequenceMgr;
	Import( &SeqItem );
}

CPropSequence::CPropSequence(const CSequenceItem *pSeqItem, CSequenceMgr* pSequenceMgr)
{
	m_pSequenceMgr = pSequenceMgr;
	Import( pSeqItem );
}

void CPropSequence::Copy( const CPropSequence* pPropItem )
{
	if( pPropItem == this )
	{
		return;
	}

	CPropItem::Copy( pPropItem );
	m_bMIDIValue = pPropItem->m_bMIDIValue;
	m_bVelocity = pPropItem->m_bVelocity;
	m_bOctave = pPropItem->m_bOctave;
}

void CPropSequence::Import( const CSequenceItem* pSeqItem )
{
	CPropItem::Import( pSeqItem );
	m_bMIDIValue = pSeqItem->m_bByte1 % 12;
	m_bVelocity = pSeqItem->m_bByte2;
	m_bOctave = pSeqItem->m_bByte1 / 12;
	m_dwUndetermined &= ~(UD_MIDIVALUE | UD_VELOCITY | UD_OCTAVE);
}

void CPropSequence::Clear()
{
	CPropItem::Clear();
	m_bMIDIValue = 0;
	m_bVelocity = 0;
	m_bOctave = 0;
}

CPropSequence CPropSequence::operator +=(const CPropSequence &SeqItem)
{
	CPropItem::operator +=( SeqItem );
	if (m_bMIDIValue != SeqItem.m_bMIDIValue)
	{
		m_dwUndetermined |= UD_MIDIVALUE;
	}
	if (m_bOctave != SeqItem.m_bOctave)
	{
		m_dwUndetermined |= UD_OCTAVE;
	}
	if (m_bVelocity != SeqItem.m_bVelocity)
	{
		m_dwUndetermined |= UD_VELOCITY;
	}
	return *this;
}

DWORD CPropSequence::ApplyToNote( CSequenceItem* pDMNote ) const
{
	ASSERT( pDMNote != NULL );
	if (pDMNote == NULL)
	{
		return 0;
	}

	DWORD dwChanged = CPropItem::ApplyToEvent( pDMNote );

	// Update Velocity
	if (m_dwChanged & CHGD_VELOCITY)
	{
		if (pDMNote->m_bByte2 != m_bVelocity)
		{
			dwChanged |= CHGD_VELOCITY;
			pDMNote->m_bByte2 = m_bVelocity;
		}
	}

	// If changing MIDI Value (and possibly octave as well)
	if (m_dwChanged & CHGD_MIDIVALUE)
	{
		// MIDIValue is 0-11
		BYTE bNewMIDIValue;
		if (m_dwChanged & CHGD_OCTAVE)
		{
			// If changing Octave and MIDIValue
			bNewMIDIValue = BYTE(m_bOctave * 12 + m_bMIDIValue);
		}
		else
		{
			// If only changing MIDIValue
			bNewMIDIValue = pDMNote->m_bByte1 / 12;
			bNewMIDIValue = BYTE(bNewMIDIValue * 12 + m_bMIDIValue);
		}

		// Ensure new MIDI value is within playable range
		if( bNewMIDIValue > 127 )
		{
			bNewMIDIValue = 127;
		}

		if( bNewMIDIValue != pDMNote->m_bByte1 )
		{
			pDMNote->m_bByte1 = bNewMIDIValue;
			dwChanged |= CHGD_MIDIVALUE;
		}
	}
	// If only changing Octave
	else if (m_dwChanged & CHGD_OCTAVE)
	{
		ASSERT( m_bOctave < 11 );

		BYTE bNewValue = (pDMNote->m_bByte1 % 12) + m_bOctave * 12;

		// Ensure new MIDI value is within playable range
		if( bNewValue > 127 )
		{
			bNewValue = 127;
		}

		if ( pDMNote->m_bByte1 != bNewValue )
		{
			pDMNote->m_bByte1 = bNewValue;
			dwChanged |= CHGD_MIDIVALUE;
		}
	}
	return dwChanged;
}

DWORD CPropSequence::ApplyPropNote( const CPropSequence* pPropNote )
{
	DWORD dwChanged = CPropItem::ApplyPropItem( pPropNote );

	if (pPropNote->m_dwChanged & CHGD_VELOCITY)
	{
		// If the velocity values are different or our velocity is Undefined, update it
		if ( (m_bVelocity != pPropNote->m_bVelocity) || (m_dwUndetermined & UD_VELOCITY) )
		{
			dwChanged |= CHGD_VELOCITY;
			m_bVelocity = pPropNote->m_bVelocity;
			m_dwUndetermined &= ~UD_VELOCITY;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_MIDIVALUE)
	{
		// If the MIDIValue values are different or our MIDIValue is Undefined, update it
		if ( (m_bMIDIValue != pPropNote->m_bMIDIValue) || (m_dwUndetermined & UD_MIDIVALUE) )
		{
			dwChanged |= CHGD_MIDIVALUE;
			m_bMIDIValue = pPropNote->m_bMIDIValue;
			m_dwUndetermined &= ~UD_MIDIVALUE;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_OCTAVE)
	{
		// If the octave values are different or our octave is Undefined, update it
		if ( (m_bOctave != pPropNote->m_bOctave) || (m_dwUndetermined & UD_OCTAVE) )
		{
			dwChanged |= CHGD_OCTAVE;
			m_bOctave = pPropNote->m_bOctave;
			m_dwUndetermined &= ~UD_OCTAVE;
		}
	}

	m_dwChanged |= dwChanged;
	return dwChanged;
}
