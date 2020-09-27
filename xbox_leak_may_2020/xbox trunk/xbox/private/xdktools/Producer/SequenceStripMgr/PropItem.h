#ifndef __PROPITEM_H_
#define __PROPITEM_H_

#include <dmusici.h>
#include <dmusicf.h>
#include "EventItem.h"

class CSequenceMgr;

class CPropItem : public CEventItem
{
public:
			CPropItem( CSequenceMgr *pSequenceMgr );
	void	Clear();
	void	GetTimeSig( MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig ) const;
	DWORD	ApplyPropItem( const CPropItem* pPropNote );
	DWORD	ApplyToEvent( CEventItem* pDMNote ) const;

protected:
			CPropItem();
	void	Copy( const CPropItem* pPropItem );
	void	Import( const CEventItem* pPropItem );
	BOOL	IsEqual( const CPropItem *pPropItem ) const;
	void	TimeOffsetToBarBeat();
	void	BarBeatToTimeOffset();
	void	TimeToBarBeatGridTick( MUSIC_TIME mtTime, long *plMeasure, long *plBeat, long *plGrid, long *plTick ) const;
	void	BarBeatGridTickToTime( long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME *pmtTime ) const;
	CPropItem operator+=(const CPropItem &propnote);

public:
	long		m_lStartBar;
	long		m_lStartBeat;
	long		m_lStartGrid;
	long		m_lStartTick;

	long		m_lEndBar;
	long		m_lEndBeat;
	long		m_lEndGrid;
	long		m_lEndTick;

	long		m_lDurBar;
	long		m_lDurBeat;
	long		m_lDurGrid;
	long		m_lDurTick;

	long		m_lChordBar;
	long		m_lChordBeat;

	CSequenceMgr *m_pSequenceMgr;

	DWORD		m_dwUndetermined;
	DWORD		m_dwChanged;
};

#define UD_ALL			(0xFFFFFFFF)

#define UD_OFFSET		(1 << 0)
#define UD_TIMESTART	(1 << 1)
#define UD_DURATION		(1 << 2)

#define UD_STARTBAR		(1 << 3)
#define UD_STARTBEAT	(1 << 4)
#define UD_STARTGRID	(1 << 5)
#define UD_STARTTICK	(1 << 6)
#define UD_ENDBAR		(1 << 7)
#define UD_ENDBEAT		(1 << 8)
#define UD_ENDGRID		(1 << 9)
#define UD_ENDTICK		(1 << 10)
#define UD_DURBAR		(1 << 11)
#define UD_DURBEAT		(1 << 12)
#define UD_DURGRID		(1 << 13)
#define UD_DURTICK		(1 << 14)
#define UD_CHORDBAR		(1 << 15)
#define UD_CHORDBEAT	(1 << 16)

#define UD_COPY			(1 << 17)

#define UD_STARTBARBEATGRIDTICK (UD_STARTBAR | UD_STARTBEAT | UD_STARTGRID | UD_STARTTICK)
#define UD_ENDBARBEATGRIDTICK (UD_ENDBAR | UD_ENDBEAT | UD_ENDGRID | UD_ENDTICK)
#define UD_DURBARBEATGRIDTICK (UD_DURBAR | UD_DURBEAT | UD_DURGRID | UD_DURTICK)

#define UNDT_OFFSET			UD_OFFSET
#define UNDT_TIMESTART		UD_TIMESTART
#define UNDT_DURATION		UD_DURATION

#define UNDT_STARTBAR		UD_STARTBAR
#define UNDT_STARTBEAT		UD_STARTBEAT
#define UNDT_STARTGRID		UD_STARTGRID
#define UNDT_STARTTICK		UD_STARTTICK
#define UNDT_ENDBAR			UD_ENDBAR
#define UNDT_ENDBEAT		UD_ENDBEAT
#define UNDT_ENDGRID		UD_ENDGRID
#define UNDT_ENDTICK		UD_ENDTICK
#define UNDT_DURBAR			UD_DURBAR
#define UNDT_DURBEAT		UD_DURBEAT
#define UNDT_DURGRID		UD_DURGRID
#define UNDT_DURTICK		UD_DURTICK
#define UNDT_CHORDBAR		UD_CHORDBAR
#define UNDT_CHORDBEAT		UD_CHORDBEAT

#define CHGD_OFFSET			UD_OFFSET
#define CHGD_TIMESTART		UD_TIMESTART
#define CHGD_DURATION		UD_DURATION

#define CHGD_STARTBAR		UD_STARTBAR
#define CHGD_STARTBEAT		UD_STARTBEAT
#define CHGD_STARTGRID		UD_STARTGRID
#define CHGD_STARTTICK		UD_STARTTICK
#define CHGD_ENDBAR			UD_ENDBAR
#define CHGD_ENDBEAT		UD_ENDBEAT
#define CHGD_ENDGRID		UD_ENDGRID
#define CHGD_ENDTICK		UD_ENDTICK
#define CHGD_DURBAR			UD_DURBAR
#define CHGD_DURBEAT		UD_DURBEAT
#define CHGD_DURGRID		UD_DURGRID
#define CHGD_DURTICK		UD_DURTICK
#define CHGD_CHORDBAR		UD_CHORDBAR
#define CHGD_CHORDBEAT		UD_CHORDBEAT

#define CHGD_START_BARBEATGRIDTICK	UD_STARTBARBEATGRIDTICK
#define CHGD_END_BARBEATGRIDTICK	UD_ENDBARBEATGRIDTICK
#define CHGD_DUR_BARBEATGRIDTICK	UD_DURBARBEATGRIDTICK

#endif // __EVENTITEM_H_
