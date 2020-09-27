#ifndef __SEQUENCEIO_H_
#define __SEQUENCEIO_H_

//#include <conductor.h>
#include "EventItem.h"
#include "CurveIO.h"

class CSequenceItem : public CEventItem
{
public:
	CSequenceItem();
	CSequenceItem(const CSequenceItem&);
	CSequenceItem(const CSequenceItem *);
	CSequenceItem(const struct FullSeqEvent*);
	CSequenceItem& operator=(const CSequenceItem&);
	CSequenceItem& operator=(const CCurveItem&);

	BOOL		IsEqual( const CSequenceItem *pSequenceItem );

	void		Clear();

	void		Copy( const CSequenceItem* pSequenceItem );

	void		CopyFrom(const DMUS_IO_SEQ_ITEM& item);
	void		CopyFrom(const FullSeqEvent*);
	
	bool		CopyTo(DMUS_IO_SEQ_ITEM& item);		// fails if item not ET_NOTEON or ET_NOTEOFF

    BYTE		m_bStatus;			
    BYTE		m_bByte1;
    BYTE		m_bByte2;
	BYTE		m_bAccidentalToDraw;
	DWORD		m_dwMIDISaveData;
};

// merge 2nd sequence into first
void MergeCSequence(CTypedPtrList<CPtrList, CSequenceItem*>& lst1,
					const CTypedPtrList<CPtrList, CSequenceItem*>& lst2);

#endif // __SEQUENCEIO_H_
