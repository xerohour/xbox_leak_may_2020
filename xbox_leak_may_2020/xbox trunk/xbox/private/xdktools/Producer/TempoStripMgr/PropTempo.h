#ifndef __PROPTEMPO_H_
#define __PROPTEMPO_H_

#ifndef MUSIC_TIME
#include <dmusici.h>
#endif

class CTempoItem;

class CPropTempo 
{
public:
				CPropTempo( );
				CPropTempo( const CTempoItem *pTempoItem );
	void		Clear();
	void		ApplyToTempoItem( CTempoItem *pTempoItem ) const;
	void		Copy( const CPropTempo* pPropTempo );

	MUSIC_TIME	m_mtTime;	// The time of this tempo
	double		m_dblTempo;	// The tempo

	long		m_lMeasure;
	long		m_lBeat;
	long		m_lOffset;
	DWORD		m_dwBits;	// Various bits used for selection
	WORD		m_wFlags;	// Various flags
};

// m_dwBits
#define UD_DRAGSELECT		0x0001

// m_wFlags
#define BF_TOPTEMPO			0x0001
#define BF_MULTIPLE			0x0002

#endif // __PROPTEMPO_H_
