#ifndef __PROPNOTE_H_
#define __PROPNOTE_H_

#include "Pattern.h"

class CDirectMusicPart;

class CPropItem
{
public:
			CPropItem();			// If this constructor is used, SetTimeSig() must be called later.
//			CPropItem( CDirectMusicPart *pDMPart );

	void	Clear();
//	void	GetTimeSig( MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig ) const;
	DWORD	ApplyPropItem( const CPropItem* pPropNote );
//	DWORD	ApplyToEvent( CEventItem* pDMNote ) const;
	CPropItem operator+=(const CPropItem &propnote);
	HRESULT SetPart( CDirectMusicPart *pDMPart );
	MUSIC_TIME AbsTime() const;
	static void TimeToBarBeatGridTick( DirectMusicTimeSig &dmTimeSig, MUSIC_TIME mtTime, long *plMeasure, long *plBeat, long *plGrid, long *plTick );
	inline static MUSIC_TIME BarBeatGridTickToTime( const DirectMusicTimeSig &dmTimeSig, long lMeasure, long lBeat, long lGrid, long lTick )
	{
		long lBeatClocks = DM_PPQNx4 / dmTimeSig.m_bBeat;
		return lTick + lGrid * (lBeatClocks / dmTimeSig.m_wGridsPerBeat)
					 + lBeat * lBeatClocks + lMeasure * lBeatClocks * dmTimeSig.m_bBeatsPerMeasure;
	}


protected:
	void	Copy( const CPropItem* pPropItem );
	BOOL	IsEqual( const CPropItem *pPropItem ) const;
	HRESULT BarBeatToGridOffset();
	HRESULT GridOffsetToBarBeat();

	// variable used for converting between bar/beat/grid/tick and clicktime/offset
	DirectMusicTimeSig m_ts;	// Time signature
	CDirectMusicPart *m_pDMPart;	// Pointer to the part this note is in

public:
	short		m_nOffset; 			// Offset from start of grid
	MUSIC_TIME	m_mtGridStart;		// Grid index to which this note belongs
	DWORD		m_dwVariation;		// 32 variation bits.
	MUSIC_TIME	m_mtDuration; 		// Duration

	// convenience variables used for the note property page.
	long		m_lStartBar;		// Start time bar
	long		m_lStartBeat;		// Start time beat
	long		m_lStartGrid;		// Start time grid
	long		m_lStartTick;			// Start time tick

	long		m_lEndBar;			// End time bar
	long		m_lEndBeat;			// End time beat
	long		m_lEndGrid;			// End time grid
	long		m_lEndTick;			// End time tick

	long		m_lDurBar;			// Dur time bar
	long		m_lDurBeat;			// Dur time beat
	long		m_lDurGrid;			// Dur time grid
	long		m_lDurTick;			// Dur time tick

	long		m_lChordBar;		// bar of the chord the note belongs to
	long		m_lChordBeat;		// beat of the chord the note belongs to

//	Used to track which fields are shared by multiple 
//	notes and have multiple values, so are undetermined.
	DWORD	m_dwUndetermined;
	DWORD	m_dwVarUndetermined;	// Which variations are undetermined

// Used to notify the MIDIMgr which fields have changed
	DWORD		m_dwChanged;

//	Used to flag when the data we are storing is an ofset, and not an absolute value
	BOOL	m_fOffset;
};

class CPropNote : public CPropItem
{
public:
	CPropNote();					// If this constructor is used, SetPart() must be called later.
	CPropNote( CDirectMusicPart *pDMPart );
	void	Clear();

	void	ImportFromDMNote( const CDirectMusicStyleNote *pDMNote, const CDirectMusicPartRef* pPartRef );
	DWORD	ApplyToDMNote( CDirectMusicStyleNote *pDMNote, const CDirectMusicPartRef* pPartRef) const;
	DWORD	ApplyPropNote(const CPropNote *pPropNote, const CDirectMusicPartRef* pPartRef);
//	CString	ToString();
//	void	CopyToPropNote( CPropNote *pPropNote );
//	void	CopyFromPropNote( const CPropNote *pPropNote );
	CPropNote operator+=(const CPropNote propnote);
	void	Copy( const CPropNote *pPropNote );

	BYTE	m_bVelocity; 			// Note velocity.
	WORD	m_wMusicvalue;			// Description of note in chord and key.
	BYTE	m_bTimeRange;			// Range to randomize time.
	BYTE	m_bDurRange; 			// Range to randomize duration.
	BYTE	m_bVelRange; 			// Range to randomize velocity.
	BYTE	m_bPlayMode;			// Playmode
	BYTE	m_bNoteFlags;			// values from DMUS_NOTEF_FLAGS
	BYTE	m_bInversionId;			// Inversion group we belong to.
	DWORD	m_adwInversionIds[8];	// Bit on means inversion id is in use.

	BYTE	m_bMIDIValue;			// MIDI value of the note
	BYTE	m_bOctave;				// Octave of the note
	BYTE	m_bScaleValue;			// ScaleValue of the note
	signed char	m_cAccidental;		// -2=bb, -1=b, 0=none, 1=#, 2=x, etc.
	signed char m_cDiatonicOffset;	// diatonic offset

	BYTE	m_bNoteFlagsUndetermined;// which note flags are undetermined


protected:
	BYTE	GetDMNotePlaymode( CDirectMusicStyleNote *pDMNote, const CDirectMusicPartRef* pPartRef ) const;
};

/*
	UD_CLICKTIME || UD_OFFSET == TRUE iff:
	UD_STARTBAR && UD_STARTBEAT && UD_STARTGRID && UD_STARTTICK == TRUE

	UD_DURATION == TRUE iff:
	UD_DURBAR && UD_DURBEAT && UD_DURGRID && UD_DURTICK == TRUE

	UD_ENDBAR && UD_ENDBEAT && UD_ENDGRID && UD_ENDTICK == TRUE iff
	UD_CLICKTIME && UD_OFFSET && UD_DURATION == TRUE
*/
/*
	UD_SCALEVALUE	= Horizontal Slider
	UD_VALUE		= MIDI Value
	UD_MUSICVALUE	= Musicvalue
	UD_OCTAVE		= Octave (0-10)
*/
// Flags for m_dwUndetermined:

#define UD_OFFSET		(DWORD)(1 << 0)
#define UD_GRIDSTART	(DWORD)(1 << 1)
#define UD_VARIATIONS	(DWORD)(1 << 2)
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
#define UD_CHORDBAR		(DWORD)(1 << 16)
#define UD_CHORDBEAT	(DWORD)(1 << 17)
#define UD_COPY			(DWORD)(1 << 18)

#define UD_STARTBARBEATGRIDTICK (UD_STARTBAR | UD_STARTBEAT | UD_STARTGRID | UD_STARTTICK)
#define UD_ENDBARBEATGRIDTICK (UD_ENDBAR | UD_ENDBEAT | UD_ENDGRID | UD_ENDTICK)
#define UD_DURBARBEATGRIDTICK (UD_DURBAR | UD_DURBEAT | UD_DURGRID | UD_DURTICK)

#define CHGD_OFFSET			UD_OFFSET	
#define CHGD_GRIDSTART		UD_GRIDSTART
#define CHGD_VARIATIONS		UD_VARIATIONS
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

#define CHGD_START_BARBEATGRIDTICK (UNDT_STARTBAR | UNDT_STARTBEAT | UNDT_STARTGRID | UNDT_STARTTICK)
#define CHGD_END_BARBEATGRIDTICK (UNDT_ENDBAR | UNDT_ENDBEAT | UNDT_ENDGRID | UNDT_ENDTICK)
#define CHGD_DUR_BARBEATGRIDTICK (UNDT_DURBAR | UNDT_DURBEAT | UNDT_DURGRID | UNDT_DURTICK)

#define UD_OCTAVE		(DWORD)(1 << 19)
#define UD_INVERSIONID	(DWORD)(1 << 20)
#define UD_ACCIDENTAL	(DWORD)(1 << 21)
#define UD_SCALEVALUE	(DWORD)(1 << 22)
#define UD_MIDIVALUE	(DWORD)(1 << 23)
#define UD_VELOCITY		(DWORD)(1 << 24)
#define UD_MUSICVALUE	(DWORD)(1 << 25)
#define UD_PLAYMODE		(DWORD)(1 << 26)
#define UD_TIMERANGE	(DWORD)(1 << 27)
#define UD_DURRANGE		(DWORD)(1 << 28)
#define UD_VELRANGE		(DWORD)(1 << 29)
#define UD_NOTEFLAGS	(DWORD)(1 << 30)
#define UD_DIATONICOFFSET	(DWORD)(1 << 31)

#define CHGD_OCTAVE			UD_OCTAVE	
#define CHGD_INVERSIONID	UD_INVERSIONID
#define CHGD_ACCIDENTAL		UD_ACCIDENTAL
#define CHGD_SCALEVALUE		UD_SCALEVALUE
#define CHGD_MIDIVALUE		UD_MIDIVALUE
#define CHGD_VELOCITY		UD_VELOCITY
#define CHGD_MUSICVALUE		UD_MUSICVALUE
#define CHGD_PLAYMODE		UD_PLAYMODE
#define CHGD_TIMERANGE		UD_TIMERANGE
#define CHGD_DURRANGE		UD_DURRANGE
#define CHGD_VELRANGE		UD_VELRANGE
#define CHGD_NOTEFLAGS		UD_NOTEFLAGS
#define CHGD_DIATONICOFFSET	UD_DIATONICOFFSET

#endif // __PROPNOTE_H_
