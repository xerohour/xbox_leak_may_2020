#ifndef __PROPCURVE_H_
#define __PROPCURVE_H_

#include "Pattern.h"
#include "PropNote.h"

#define MIN_CC_VALUE	0
#define MAX_CC_VALUE	127

#define MIN_PB_VALUE	1
#define MAX_PB_VALUE	16383
#define MIN_PB_DISP_VALUE	-8191
#define MAX_PB_DISP_VALUE	 8191
#define PB_DISP_OFFSET		 8192

#define INVALID_CC_VALUE	0xFFFFFFFF

class CPropCurve
{
public:
	CPropCurve();

	DWORD	ApplyValuesToDMCurve( const CDirectMusicPart* pDMPart, CDirectMusicStyleCurve* pDMCurve );
	void	GetValuesFromDMCurve( const CDirectMusicPart* pDMPart, const CDirectMusicStyleCurve* pDMCurve );
	void	CalcBarBeatGridTick( void );
	void	CalcGridStartAndDuration( DWORD dwChanged );
	CPropCurve operator+=( const CPropCurve PropCurve );
	MUSIC_TIME AbsTime();
	void	Copy( CPropCurve *pPropCurve );
	void	Clear();

	// Event fields
	MUSIC_TIME	m_mtGridStart;		// Grid position in track that this curve belogs to
	short		m_nTimeOffset;		// Offset, in music time, of event from designated grid position
	DWORD		m_dwVariation;		// Variation bits
	
	// Curve fields	
	MUSIC_TIME	m_mtDuration;		// How long this curve lasts
	MUSIC_TIME	m_mtResetDuration;	// Reset duration
	short		m_nStartValue;		// Curve's start value
	short		m_nEndValue;		// Curve's end value
	long		m_lResetValue;		// Curve's reset value
	BYTE		m_bCurveShape;		// Shape of curve
	BYTE		m_bFlags;			// Controls if reset is sent
	BYTE		m_bEventType;		// Type of curve (PB, CC, AT(P), AT(M), RPN, NRPN)
	BYTE		m_bCCData;			// CC# if this is a control change type
	WORD		m_wParamType;		// RPN or NRPN parameter number.
	WORD		m_wMergeIndex;		// Allows multiple parameters to be merged (pitchbend, volume, and expression.)
	
	// Convenience variables used for the Curve property page.
	long		m_lStartBar;		// Start time bar
	long		m_lStartBeat;		// Start time beat
	long		m_lStartGrid;		// Start time grid
	long		m_lStartTick;		// Start time tick

	long		m_lEndBar;			// End time bar
	long		m_lEndBeat;			// End time beat
	long		m_lEndGrid;			// End time grid
	long		m_lEndTick;			// End time tick

	long		m_lDurBar;			// Dur time bar
	long		m_lDurBeat;			// Dur time beat
	long		m_lDurGrid;			// Dur time grid
	long		m_lDurTick;			// Dur time tick

	long		m_lResetBar;		// Reset duration time bar
	long		m_lResetBeat;		// Reset duration time beat
	long		m_lResetGrid;		// Reset duration time grid
	long		m_lResetTick;		// Reset duration time tick

	// Used to track which fields are shared by multiple 
	// notes and have multiple values, so are undetermined.
	DWORD		m_dwUndetermined;
	DWORD		m_dwUndetermined2;
	DWORD		m_dwVarUndetermined;		// Which variations are undetermined

	// Used to relay which fields have changed
	DWORD		m_dwChanged;
	DWORD		m_dwChanged2;

	// Additional fields used to calculate min/max range
	// for Bar-Beat-Grid-Tick fields
	DirectMusicTimeSig	m_TimeSignature;	// Part's time signature
   	WORD				m_wNbrMeasures;		// Part's length
};


// Flags for m_dwUndetermined:

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

#define CHGD_STARTVAL		UNDT_STARTVAL	
#define CHGD_ENDVAL			UNDT_ENDVAL		
#define CHGD_SHAPE			UNDT_SHAPE		
#define CHGD_FLIPVERT		UNDT_FLIPVERT	
#define CHGD_FLIPHORZ		UNDT_FLIPHORZ	

#define CHGD_RESETENABLE	UNDT_RESETENABLE
#define CHGD_RESETVAL		UNDT_RESETVAL
#define CHGD_RESETBAR		UNDT_RESETBAR		
#define CHGD_RESETBEAT		UNDT_RESETBEAT	
#define CHGD_RESETGRID		UNDT_RESETGRID	
#define CHGD_RESETTICK		UNDT_RESETTICK
#define CHGD_RESETDURATION	UNDT_RESETDURATION

#define	CHGD2_MERGEINDEX	UNDT2_MERGEINDEX
#define	CHGD2_STARTCURRENT	UNDT2_STARTCURRENT

#define CHGD_RESET_BARBEATGRIDTICK (UNDT_RESETBAR | UNDT_RESETBEAT | UNDT_RESETGRID | UNDT_RESETTICK)

#endif // __PROPCURVE_H_
