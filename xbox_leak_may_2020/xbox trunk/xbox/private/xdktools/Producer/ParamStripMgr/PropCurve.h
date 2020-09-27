#ifndef __PROPCURVE_H_
#define __PROPCURVE_H_

#include "TrackMgr.h"

#define INVALID_VALUE	0xFFFFFFFF

class CPropCurve
{
public:
	CPropCurve();

	DWORD	ApplyValuesToCurve( CParamStrip* pParamStrip, CTrackItem* pCurve );
	void	GetValuesFromCurve( CParamStrip* pParamStrip, CTrackItem* pCurve );
	MUSIC_TIME AbsTime();
	void	Copy( CPropCurve *pPropCurve );
	void	Clear();
	
	CPropCurve operator +=(const CPropCurve PropCurve);
	
	void UpdateStartBarBeatGridTick(long m_lStartBar, long m_lStartBeat, long m_lStartGrid, long m_lStartTick);
	void UpdateEndBarBeatGridTick(long m_lEndBar, long m_lEndBeat, long m_lEndGrid, long m_lEndTick);
	void UpdateDurationBarBeatGridTick(long m_lDurationBar, long m_lDurationBeat, long m_lDurationGrid, long m_lDurationTick);


	// Curve fields	
	float		m_fltStartValue;	// Curve's start value
	float		m_fltEndValue;		// Curve's end value
	DWORD		m_dwCurveShape;		// Shape of curve
	BOOL		m_fStartFromCurrent;// Start from current flag	
	
	// Used to track which fields are shared by multiple 
	// notes and have multiple values, so are undetermined.
	DWORD		m_dwUndetermined;
	DWORD		m_dwUndetermined2;

	// Used to relay which fields have changed
	DWORD		m_dwChanged;
	DWORD		m_dwChanged2;

	REFERENCE_TIME	m_rtStartTime;
	REFERENCE_TIME	m_rtDuration;

	CParamStrip*	m_pParamStrip;

	// Fields used for setting the bar, beat, grid, tick data
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
};


// Flags for m_dwUndetermined:

#define UD_DURATION		(DWORD)(1 << 3)
#define UD_STARTBAR		(DWORD)(1 << 4)
#define UD_STARTBEAT	(DWORD)(1 << 5)
#define UD_STARTGRID	(DWORD)(1 << 6)
#define UD_STARTTICK	(DWORD)(1 << 7)
#define UD_ENDBAR		(DWORD)(1 << 8)
#define UD_ENDBEAT		(DWORD)(1 << 9)
#define UD_ENDGRID		(DWORD)(1 << 10)
#define UD_ENDTICK		(DWORD)(1 << 11)
#define UD_DURBAR		(DWORD)(1 << 12)
#define UD_DURBEAT		(DWORD)(1 << 13)
#define UD_DURGRID		(DWORD)(1 << 14)
#define UD_DURTICK		(DWORD)(1 << 15)
#define UD_STARTCURRENT	(DWORD)(1 << 16)


#define UNDT_OFFSET			UD_OFFSET
#define UNDT_GRIDSTART		UD_GRIDSTART
#define UNDT_VARIATIONS		UD_VARIATIONS
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

#define UNDT_FLIPVERT		(1 << 19)
#define UNDT_FLIPHORZ		(1 << 20)
#define UNDT_STARTVAL		(1 << 21)
#define UNDT_ENDVAL			(1 << 22)
#define UNDT_SHAPE			(1 << 23)

#define UNDT_RESETENABLE	(1 << 24)
#define UNDT_RESETVAL		(1 << 25)
#define UNDT_RESETBAR		(1 << 26)
#define UNDT_RESETBEAT		(1 << 27)
#define UNDT_RESETGRID		(1 << 28)
#define UNDT_RESETTICK		(1 << 29)
#define UNDT_RESETDURATION	(1 << 30)

#define UNDT2_MERGEINDEX	(DWORD(1 << 0))
#define UNDT2_STARTCURRENT	(DWORD(1 << 1))

#define UNDT_CURVENONE		0x80000000

#define UNDT_START_BARBEATGRIDTICK (UNDT_STARTBAR | UNDT_STARTBEAT | UNDT_STARTGRID | UNDT_STARTTICK)
#define UNDT_END_BARBEATGRIDTICK (UNDT_ENDBAR | UNDT_ENDBEAT | UNDT_ENDGRID | UNDT_ENDTICK)
#define UNDT_DUR_BARBEATGRIDTICK (UNDT_DURBAR | UNDT_DURBEAT | UNDT_DURGRID | UNDT_DURTICK)
#define UNDT_RESET_BARBEATGRIDTICK (UNDT_RESETBAR | UNDT_RESETBEAT | UNDT_RESETGRID | UNDT_RESETTICK)


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
#define CHGD_STARTCURRENT	UD_STARTCURRENT

#define CHGD_STARTVAL		UNDT_STARTVAL	
#define CHGD_ENDVAL			UNDT_ENDVAL		
#define CHGD_SHAPE			UNDT_SHAPE		
#define CHGD_FLIPVERT		UNDT_FLIPVERT	
#define CHGD_FLIPHORZ		UNDT_FLIPHORZ	


#define CHGD_START_BARBEATGRIDTICK (UNDT_STARTBAR | UNDT_STARTBEAT | UNDT_STARTGRID | UNDT_STARTTICK)
#define CHGD_END_BARBEATGRIDTICK (UNDT_ENDBAR | UNDT_ENDBEAT | UNDT_ENDGRID | UNDT_ENDTICK)
#define CHGD_DUR_BARBEATGRIDTICK (UNDT_DURBAR | UNDT_DURBEAT | UNDT_DURGRID | UNDT_DURTICK)

#endif // __PROPCURVE_H_
