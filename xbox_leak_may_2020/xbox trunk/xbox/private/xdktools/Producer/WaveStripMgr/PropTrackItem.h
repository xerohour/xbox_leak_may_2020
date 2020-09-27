#ifndef __WAVE_PROPTRACKITEM_H_
#define __WAVE_PROPTRACKITEM_H_

// XBOX only, in lieue of appearing in dmusici.h for now..
#ifndef DMUS_WAVEF_IGNORELOOPS
#define DMUS_WAVEF_IGNORELOOPS      0x20       /* Ignore segment looping. */
#endif

/////////////////////////////////////////////////////////////////////////////
// m_dwBits
#define PROPF_MULTIPLESELECT		0x00000001


/////////////////////////////////////////////////////////////////////////////
// Flags for m_dwUndetermined_TabFileRef

#define UNDT_DOCROOT			(1 << 0)
#define UNDT_TIME_PHYSICAL		(1 << 1)
#define UNDT_START_OFFSET		(1 << 2)
#define UNDT_DURATION			(1 << 3)
#define UNDT_LOCK_END			(1 << 4)
#define UNDT_LOCK_LENGTH		(1 << 5)


/////////////////////////////////////////////////////////////////////////////
// Flags for m_dwChanged (TabFileRef)

#define CHGD_DOCROOT			(1 << 0)
#define CHGD_TIME_OR_DURATION	(1 << 1)
#define CHGD_START_OFFSET		(1 << 2)
#define CHGD_LOCK_END			(1 << 3)
#define CHGD_LOCK_LENGTH		(1 << 4)


/////////////////////////////////////////////////////////////////////////////
// Flags for m_dwUndetermined_TabLoop

#define UNDT_IS_LOOPED			(1 << 0)
#define UNDT_LOOP_START			(1 << 1)
#define UNDT_LOOP_END			(1 << 2)
#define UNDT_LOOP_LOCK_LENGTH	(1 << 3)


/////////////////////////////////////////////////////////////////////////////
// Flags for m_dwChanged (TabLoop)

#define CHGD_IS_LOOPED			(1 << 0)
#define CHGD_LOOP				(1 << 1)
#define CHGD_LOOP_LOCK_LENGTH	(1 << 2)


/////////////////////////////////////////////////////////////////////////////
// Flags for m_dwUndetermined_TabPerformance

#define UNDT_VOLUME				(1 << 0)
#define UNDT_PITCH				(1 << 1)
#define UNDT_TIME_LOGICAL		(1 << 2)
#define UNDT_WAVEF_NOINVALIDATE	(1 << 3)
#define UNDT_WAVEF_IGNORELOOPS  (1 << 4) 
#define UNDT_VOLUME_RANGE		(1 << 5)
#define UNDT_PITCH_RANGE		(1 << 6)


/////////////////////////////////////////////////////////////////////////////
// Flags for m_dwChanged (TabPerformance)

#define CHGD_VOLUME				(1 << 0)
#define CHGD_PITCH				(1 << 1)
#define CHGD_TIME_LOGICAL		(1 << 2)
#define CHGD_VOLUME_RANGE		(1 << 3)
#define CHGD_PITCH_RANGE 		(1 << 4)
#define CHGD_DM_FLAGS			(1 << 5)


/////////////////////////////////////////////////////////////////////////////
// Flags for m_dwChanged (TabLoop)
// m_dwUndetermined_TabVariations does not use this flag
// Each bit in m_dwUndetermined_TabVariations represents 1 of the 32 variations

#define CHGD_VARIATIONS			(1 << 0)


/////////////////////////////////////////////////////////////////
//	CPropTrackItem

class CPropTrackItem
{
public:
	CPropTrackItem();
	virtual ~CPropTrackItem();

	void Clear();
	void Copy( const CPropTrackItem* pPropItem );
	void CopyTrackItem( const CTrackItem* pItem );
	void MergeTrackItem( const CTrackItem* pItem );

public:
	CTrackItem		m_Item;

	short			m_nPropertyTab;	
	DWORD			m_dwChanged;	// Values are per tab

	DWORD			m_dwUndetermined_TabFileRef;
	DWORD			m_dwUndetermined_TabLoop;
	DWORD			m_dwUndetermined_TabPerformance;
	DWORD			m_dwUndetermined_TabVariations;
	DWORD			m_dwChangedVar_TabVariations;	// Bit signifies variation change on TabVariations

	DWORD			m_dwBits;
};

#endif // __WAVE_PROPTRACKITEM_H_
