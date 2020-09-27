#ifndef __PROPBAND_H_
#define __PROPBAND_H_

#include <dmusprod.h>
#include <dmusici.h>

#define MAX_TICK (DMUS_PPQ << 1)

class CBandItem;

class CPropBand 
{
public:
				CPropBand( );
				CPropBand( const CBandItem *pBandItem );
				~CPropBand( );
	void		ApplyToBandItem( CBandItem *pBandItem ) const;

	long			m_mtTimePhysical;	// Time of measure/beat/tick
    DWORD			m_dwMeasure;		// What measure this band falls on
    BYTE			m_bBeat;			// What beat this band falls on
	long			m_lTick;			// What tick this band falls on

	long			m_mtTimeLogical;	// Time of measure/beat 
	long			m_lLogicalMeasure;	// What measure this band belongs to
	long			m_lLogicalBeat;		// What beat this band belongs to
	
	DWORD			m_dwBits;			// Various bits used for selection
	WORD			m_wFlags;			// Various flags
	CString			m_strText;			// Text contained in this band event
	IDMUSProdNode*	m_pIBandNode;
};

// m_dwBits
#define UD_DRAGSELECT		0x0001
#define UD_CURRENTSELECTION 0x0002
#define UD_COPYSELECT		0x0004

// m_wFlags
#define BF_TOPBAND			0x0001

#endif // __PROPBAND_H_
