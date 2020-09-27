#ifndef __BANDIO_H_
#define __BANDIO_H_

#include "PropBand.h"
#include <dmusici.h>

// Flags for SetTimePhysical()
#define STP_LOGICAL_NO_ACTION				1
#define STP_LOGICAL_SET_DEFAULT				2
#define STP_LOGICAL_ADJUST					3
#define STP_LOGICAL_RECALC_MEASURE_BEAT		4
#define STP_LOGICAL_FROM_BEAT_OFFSET		5

class CBandStrip;

class CBandItem : public CPropBand
{
public:
	CBandItem(CBandStrip* pBandStrip);
	~CBandItem();

	BOOL	After(const CBandItem& Band);
	BOOL	Before(const CBandItem& Band);
	void	SetSelectFlag( BOOL fSelected );
	HRESULT	SetTimePhysical( MUSIC_TIME mtTimePhysical, short nAction );
	HRESULT	SetTimeLogical( MUSIC_TIME mtTimeLogical );

//	Used to track the selection of the band in display.
	BOOL			m_fSelected;		// This band is currently selected.
	CBandStrip*		m_pBandStrip;
};

#endif // __BANDIO_H_
