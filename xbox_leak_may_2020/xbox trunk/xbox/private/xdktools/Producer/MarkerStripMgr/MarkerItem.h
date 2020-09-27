#ifndef __MARKERITEM_H_
#define __MARKERITEM_H_

#include <dmusici.h>

#define MAX_TICK 32767

// m_dwBitsUI
#define UD_DRAGSELECT		0x00000001
#define UD_MULTIPLESELECT	0x00000002

// m_wFlagsUI
#define RF_TOP_ITEM		0x0001

// m_dwMarkerTypes
typedef enum { MARKER_SWITCH = 0, MARKER_CUE = 1 } MARKER_TYPE;

class CMarkerItem
{
public:
	CMarkerItem();
	CMarkerItem(const CMarkerItem&);

	void Clear();
	void Copy( const CMarkerItem* pItem );

	BOOL After(const CMarkerItem& item);
	BOOL Before(const CMarkerItem& item);

	// Fields that are persisted
	MUSIC_TIME	m_mtTime;	// Time of measure/beat
	MARKER_TYPE m_typeMarker;// Type of marker

	// Edit-time only fields
	long		m_lMeasure;	// What measure this item falls on
	long		m_lBeat;	// What beat this item falls on
	long		m_lGrid;	// What grid this item falls on
    long		m_lTick;	// What tick this item falls on
	DWORD		m_dwBitsUI;	// Various bits
	WORD		m_wFlagsUI;	// Various flags
	bool		m_fSelected;// This item is currently selected.
};

#endif // __MARKERITEM_H_
