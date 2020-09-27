#ifndef __PROPSEQUENCE_H_
#define __PROPSEQUENCE_H_

#include "SequenceIO.h"
#include "PropItem.h"

class CPropSequence : public CPropItem
{
public:
	CPropSequence( );
	CPropSequence( CSequenceMgr *pSequenceMgr );
	CPropSequence(const CSequenceItem&, CSequenceMgr* pSequenceMgr);
	CPropSequence(const CSequenceItem*, CSequenceMgr* pSequenceMgr);
	void	Copy( const CPropSequence* pPropItem );
	void	Import( const CSequenceItem* pSequenceItem );
	void	Clear();
	CPropSequence operator+=(const CPropSequence &SeqItem);
	DWORD	ApplyToNote( CSequenceItem* pSequenceItem ) const;
	DWORD	ApplyPropNote( const CPropSequence* pPropItem );

    BYTE	m_bMIDIValue;			
    BYTE	m_bOctave;
    BYTE	m_bVelocity;
};

#define UD_MIDIVALUE	(1 << 18)
#define UD_VELOCITY		(1 << 19)
#define UD_OCTAVE		(1 << 20)

#define	CHGD_MIDIVALUE	UD_MIDIVALUE
#define	CHGD_VELOCITY	UD_VELOCITY
#define	CHGD_OCTAVE		UD_OCTAVE

#define BF_DISPLAYFLATS	(1 << 0)

#endif // __PROPSEQUENCE_H_
