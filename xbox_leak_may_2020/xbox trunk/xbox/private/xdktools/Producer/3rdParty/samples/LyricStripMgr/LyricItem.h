/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#ifndef __LYRICITEM_H_
#define __LYRICITEM_H_

#include <dmusici.h>

typedef struct _DMUS_IO_LYRIC
{
	MUSIC_TIME	mtTime;		// Time of lyric event
	DWORD		dwMeasure;	// Measure event occurs in
	BYTE		bBeat;		// Beat event occurs in
	BYTE		bPad[3];	// Padding
	DWORD		dwLength;	// Length of text of WCHARS (in # of bytes) that follows the structure
} DMUS_IO_LYRIC;


// m_dwBitsUI
#define UD_DRAGSELECT		0x00000001
#define UD_MULTIPLESELECT	0x00000002


// m_wFlagsUI
#define RF_TOP_ITEM		0x0001


// Flags for SetTimePhysical()
#define STP_LOGICAL_NO_ACTION	1
#define STP_LOGICAL_SET_DEFAULT	2
#define STP_LOGICAL_ADJUST		3


class CLyricMgr;

class CLyricItem
{
public:
	CLyricItem();	// Only used by classes that embed CLyricItem
	CLyricItem( CLyricMgr* pLyricMgr );
	CLyricItem( CLyricMgr* pLyricMgr, const CLyricItem& );

	void Clear();
	void Copy( const CLyricItem* pLyricItem );

	BOOL After( const CLyricItem& Lyric );
	BOOL Before( const CLyricItem& Lyric );

	HRESULT	SetTimePhysical( MUSIC_TIME mtTimePhysical, short nAction );
	HRESULT	SetTimeLogical( MUSIC_TIME mtTimeLogical );

	// Fields that are persisted
	DWORD		m_dwTimingFlagsDM;	// DirectMusic DMUS_PMSGF_TOOL_ flags
	MUSIC_TIME	m_mtTimeLogical;	// Time of measure/beat 
	MUSIC_TIME	m_mtTimePhysical;	// Time of measure/beat/tick
	CString		m_strText;			// Text contained in this lyric event

	// Runtime only fields
	CLyricMgr*	m_pLyricMgr;
    long		m_lMeasure;			// What measure this lyric falls on
    long		m_lBeat;			// What beat this lyric falls on
    long		m_lTick;			// What tick this lyric falls on
    long		m_lLogicalMeasure;	// What measure this lyric belongs to
    long		m_lLogicalBeat;		// What beat this lyric belongs to
	DWORD		m_dwBitsUI;			// Various bits
	WORD		m_wFlagsUI;			// Various flags
	BOOL		m_fSelected;		// This lyric is currently selected.
};

#endif // __LYRICITEM_H_
