#ifndef __PATTERN_H__
#define __PATTERN_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Pattern.h : header file
//

#include "alist.h"
#include "SequenceIO.h"
// These all are included by SequenceIO.h
//#include "EventItem.h"
//#include "CurveIO.h"
//#include <dmusici.h>
//#include <dmusicf.h>
#include "SharedPattern.h"
#include "MIDIStripMgrApp.h"
#include "Timeline.h"

interface IDMUSProdRIFFStream;
interface IDMUSProdNode;

// RPN/NRPN parameter conversion macros
#define FILE_TO_MEMORY_WPARAMTYPE( wParamType ) (WORD( ((wParamType & 0x7F00) >> 1) | (wParamType & 0x7F) ))
#define MEMORY_TO_FILE_WPARAMTYPE( wParamType ) (WORD( ((wParamType & 0x3F80) << 1) | (wParamType & 0x7F) ))

// Marker macros

#define MARKER_AND_VARIATION( pDMMarker, dwVariations ) ( (pDMMarker->m_dwEnterVariation | pDMMarker->m_dwEnterChordVariation | pDMMarker->m_dwExitVariation | pDMMarker->m_dwExitChordVariation) & dwVariations )

#ifndef _afxModuleAddrThis
#define _afxModuleAddrThis AfxGetStaticModuleState()
#endif

// DLL defines from StyleDesignerDLL.h

#define MID_BUFFER		100

#define DM_PPQN         768				// Direct Music pulses per quarter note
#define DM_PPQNx4       ( DM_PPQN << 2 )
#define DM_PPQN_2       ( DM_PPQN >> 1 )

#define MIN_PCHANNEL	1
#define MAX_PCHANNEL	999

#define MIN_INV_LOWER	0
#define MAX_INV_LOWER	115

#define MIN_INV_UPPER	12
#define MAX_INV_UPPER	127

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0
#define MIDI_SYSX       0xF0
#define MIDI_MTC        0xF1
#define MIDI_SONGPP     0xF2
#define MIDI_SONGS      0xF3
#define MIDI_EOX        0xF7
#define MIDI_CLOCK      0xF8
#define MIDI_START      0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC
#define MIDI_SENSE      0xFE

#define ALL_VARIATIONS	0xFFFFFFFF

#define MARKERF_BOTH	(DMUS_MARKERF_START|DMUS_MARKERF_STOP)

#define MIN_GROOVE		1
#define MAX_GROOVE		100

#define MIN_EMB_CUSTOM_ID	100
#define MAX_EMB_CUSTOM_ID	199

#define MIN_PATTERN_LENGTH		1
#define MAX_PATTERN_LENGTH		999


// Curve defines
#define CT_MAX		192		// Curve table maximum
#define CT_FACTOR	100		// Curve table multiplication factor
#define CT_DIVFACTOR	( ( CT_MAX ) * CT_FACTOR ) // Curve division factor


// Curve Table: LINEAR
static short ganCT_Linear[CT_MAX + 1] =
{
	0,100,200,300,400,500,600,700,
	800,900,1000,1100,1200,1300,1400,1500,
	1600,1700,1800,1900,2000,2100,2200,2300,
	2400,2500,2600,2700,2800,2900,3000,3100,
	3200,3300,3400,3500,3600,3700,3800,3900,
	4000,4100,4200,4300,4400,4500,4600,4700,
	4800,4900,5000,5100,5200,5300,5400,5500,
	5600,5700,5800,5900,6000,6100,6200,6300,
	6400,6500,6600,6700,6800,6900,7000,7100,
	7200,7300,7400,7500,7600,7700,7800,7900,
	8000,8100,8200,8300,8400,8500,8600,8700,
	8800,8900,9000,9100,9200,9300,9400,9500,
	9600,9700,9800,9900,10000,10100,10200,10300,
	10400,10500,10600,10700,10800,10900,11000,11100,
	11200,11300,11400,11500,11600,11700,11800,11900,
	12000,12100,12200,12300,12400,12500,12600,12700,
	12800,12900,13000,13100,13200,13300,13400,13500,
	13600,13700,13800,13900,14000,14100,14200,14300,
	14400,14500,14600,14700,14800,14900,15000,15100,
	15200,15300,15400,15500,15600,15700,15800,15900,
	16000,16100,16200,16300,16400,16500,16600,16700,
	16800,16900,17000,17100,17200,17300,17400,17500,
	17600,17700,17800,17900,18000,18100,18200,18300,
	18400,18500,18600,18700,18800,18900,19000,19100,19200
};


// Curve Table: SINE
static short ganCT_Sine[CT_MAX + 1] =
{
	0,1,5,11,20,32,46,62,
	82,103,128,155,184,216,250,287,
	327,369,413,460,509,561,615,671,
	730,792,855,921,990,1060,1133,1208,
	1286,1365,1447,1531,1617,1706,1796,1889,
	1983,2080,2179,2279,2382,2486,2593,2701,
	2811,2923,3037,3153,3270,3389,3509,3632,
	3755,3881,4008,4136,4266,4397,4530,4664,
	4799,4936,5074,5213,5354,5495,5638,5781,
	5926,6071,6218,6365,6514,6663,6813,6963,
	7115,7267,7420,7573,7727,7881,8036,8191,
	8346,8502,8659,8815,8972,9128,9285,9442,
	9600,9757,9914,10071,10227,10384,10540,10697,
	10853,11008,11163,11318,11472,11626,11779,11932,
	12084,12236,12386,12536,12685,12834,12981,13128,
	13273,13418,13561,13704,13845,13986,14125,14263,
	14399,14535,14669,14802,14933,15063,15191,15318,
	15444,15567,15690,15810,15929,16046,16162,16276,
	16388,16498,16606,16713,16817,16920,17020,17119,
	17216,17310,17403,17493,17582,17668,17752,17834,
	17913,17991,18066,18139,18209,18278,18344,18407,
	18469,18528,18584,18638,18690,18739,18786,18830,
	18872,18912,18949,18983,19015,19044,19071,19096,
	19117,19137,19153,19167,19179,19188,19194,19198,19200
};


// Curve Table: LOG
static short ganCT_Log[CT_MAX + 1] =
{
	0,381,747,1097,1432,1755,2066,2366,
	2655,2934,3205,3467,3721,3967,4207,4439,
	4666,4886,5101,5310,5515,5714,5909,6099,
	6285,6467,6645,6819,6990,7157,7321,7482,
	7640,7795,7947,8096,8243,8387,8529,8668,
	8805,8940,9073,9204,9332,9459,9584,9707,
	9828,9947,10065,10181,10295,10408,10520,10630,
	10738,10845,10951,11056,11159,11261,11361,11461,
	11559,11656,11752,11847,11941,12034,12126,12216,
	12306,12395,12483,12570,12656,12741,12826,12909,
	12992,13074,13155,13235,13315,13394,13472,13549,
	13626,13702,13777,13851,13925,13998,14071,14143,
	14214,14285,14355,14425,14494,14562,14630,14698,
	14764,14831,14896,14962,15026,15091,15154,15218,
	15280,15343,15405,15466,15527,15587,15647,15707,
	15766,15825,15883,15941,15999,16056,16113,16169,
	16225,16281,16336,16391,16446,16500,16554,16607,
	16661,16713,16766,16818,16870,16921,16973,17024,
	17074,17124,17174,17224,17273,17323,17371,17420,
	17468,17516,17564,17611,17658,17705,17752,17798,
	17844,17890,17936,17981,18026,18071,18116,18160,
	18204,18248,18292,18335,18379,18422,18464,18507,
	18549,18592,18634,18675,18717,18758,18799,18840,
	18881,18921,18962,19002,19042,19081,19121,19160,19200
};


// Curve Table: EXPONENTIAL
static short ganCT_Exp[CT_MAX + 1] =
{
	0,40,79,119,158,198,238,279,
	319,360,401,442,483,525,566,608,
	651,693,736,778,821,865,908,952,
	996,1040,1084,1129,1174,1219,1264,1310,
	1356,1402,1448,1495,1542,1589,1636,1684,
	1732,1780,1829,1877,1927,1976,2026,2076,
	2126,2176,2227,2279,2330,2382,2434,2487,
	2539,2593,2646,2700,2754,2809,2864,2919,
	2975,3031,3087,3144,3201,3259,3317,3375,
	3434,3493,3553,3613,3673,3734,3795,3857,
	3920,3982,4046,4109,4174,4238,4304,4369,
	4436,4502,4570,4638,4706,4775,4845,4915,
	4986,5057,5129,5202,5275,5349,5423,5498,
	5574,5651,5728,5806,5885,5965,6045,6126,
	6208,6291,6374,6459,6544,6630,6717,6805,
	6894,6984,7074,7166,7259,7353,7448,7544,
	7641,7739,7839,7939,8041,8144,8249,8355,
	8462,8570,8680,8792,8905,9019,9135,9253,
	9372,9493,9616,9741,9868,9996,10127,10260,
	10395,10532,10671,10813,10957,11104,11253,11405,
	11560,11718,11879,12043,12210,12381,12555,12733,
	12915,13101,13291,13486,13685,13890,14099,14314,
	14534,14761,14993,15233,15479,15733,15995,16266,
	16545,16834,17134,17445,17768,18103,18453,18819,19200
};

// Strip State flags
#define STPST_ACTIVESTRIP	0x00000001
#define STPST_PICKUPBAR		0x00000002


struct ioPianoRollDesign
{
	DWORD		m_dwVariations;			// which variations to display and play
	COLORREF	m_crUnselectedNoteColor;// Note color
	COLORREF	m_crSelectedNoteColor;	// Selected note color
	double		m_dblVerticalZoom;		// Vertical zoom factor
	long		m_lVerticalScroll;		// Vertical scroll
	long		m_lHeight;				// Height of strip
	int			m_nStripView;			// Minimized or maximized
	long		m_lSnapValue;			// value we snap by when moving notes with the mouse
	DWORD		m_dwFlags;				// various state flags (see STPST_ prefix)
	long		m_lPad2; // m_lChordPattern;// Chord pattern we're editing in
	char		m_cPad3; // m_cRoot;		// Root of chord we're editing in
	COLORREF	m_crOverlappingNoteColor;// Color of overlapping notes
	COLORREF	m_crAccidentalColor;// Color of accidentals in hybrid notation
	BOOL		m_fHybridNotation;		// If TRUE, use hybrid notation
	DWORD		m_dwExtraBars;			// Number of extra bars to display after the pattern
};

struct ioGlobalCurveStripState
{
	int			m_nCurveStripView;		// Minimized or maximized
};

struct ioCurveStripState
{
	int			m_nStripHeight;			// Height of strip
	BYTE		m_bCCType;				// Identifies type of control strip
	BYTE		m_bPad;
	DWORD		m_dwFlags;				// STPST_ flags (Strip State) in pattern.h
	WORD		m_wRPNType;
};


/////////////////////////////////////////////////////////////////////////////
// DirectMusicTimeSig structure

struct DirectMusicTimeSig
{
	// Time signatures define how many beats per measure, which note receives
	// the beat, and the grid resolution.
	DirectMusicTimeSig() : m_bBeatsPerMeasure(0), m_bBeat(0), m_wGridsPerBeat(0) { }
	BYTE	m_bBeatsPerMeasure;		// beats per measure (top of time sig)
	BYTE	m_bBeat;				// what note receives the beat (bottom of time sig.)
									// we can assume that 0 means 256th note
	WORD	m_wGridsPerBeat;		// grids per beat
};


// Event types
#define ET_NOTE		0x01
#define ET_CURVE	0x02
#define ET_MARKER	0x03

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicEventItem class

class CDirectMusicEventItem : public AListItem
{
friend class CDirectMusicPattern;
friend class CDirectMusicPartRef;
friend class CDirectMusicPart;
friend class CMIDIMgr;
friend class CPianoRollStrip;
friend class CCurveStrip;

public:
	CDirectMusicEventItem() : m_fSelected(FALSE), m_mtGridStart(0),
		m_nTimeOffset(0), m_dwVariation(0) {}
	virtual ~CDirectMusicEventItem(); 
	CDirectMusicEventItem* GetNext() { return (CDirectMusicEventItem*) m_pNext; }

	MUSIC_TIME	m_mtGridStart;		// Grid position in track that this event belogs to.
	short		m_nTimeOffset;		// Offset, in music time, of event from designated grid position.
	DWORD		m_dwVariation;		// variation bits
	BOOL		m_fSelected;		// Whether or not this note is selected
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleNote class

class CDirectMusicStyleNote : public CDirectMusicEventItem
{
friend class CDirectMusicPattern;
friend class CDirectMusicPartRef;
friend class CDirectMusicPart;
friend class CMIDIMgr;
friend class CPianoRollStrip;
friend class CPropNote;

public:
	CDirectMusicStyleNote() : CDirectMusicEventItem()
	{
		//m_mtGridStart = 0;
		//m_nTimeOffset = 0;
		//m_dwVariation = 0;
		//m_fSelected = FALSE;

		m_mtDuration = 0;
		m_wMusicValue = 0;
		m_bVelocity = 0;
		m_bTimeRange = 0;
		m_bDurRange = 0;
		m_bVelRange = 0;
		m_bInversionId = 0;
		m_bPlayModeFlags = 0;
		m_bMIDIValue = 0;
		m_bNoteFlags = 0;
		m_cDiatonicOffset = 0;
	}
	CDirectMusicStyleNote( const DMUS_IO_STYLENOTE &iDMStyleNote ) : CDirectMusicEventItem()
	{
		m_mtGridStart = iDMStyleNote.mtGridStart;
		m_nTimeOffset = iDMStyleNote.nTimeOffset;
		m_dwVariation = iDMStyleNote.dwVariation;
		//m_fSelected = FALSE;

		m_mtDuration = iDMStyleNote.mtDuration;
		m_wMusicValue = iDMStyleNote.wMusicValue;
		m_bVelocity = iDMStyleNote.bVelocity;
		m_bTimeRange = iDMStyleNote.bTimeRange;
		m_bDurRange = iDMStyleNote.bDurRange;
		m_bVelRange = iDMStyleNote.bVelRange;
		m_bInversionId = iDMStyleNote.bInversionID; 
		m_bPlayModeFlags = iDMStyleNote.bPlayModeFlags;
		m_bNoteFlags = iDMStyleNote.bNoteFlags;
		m_bMIDIValue = 0;
		m_cDiatonicOffset = 0;
	}
	CDirectMusicStyleNote( const CDirectMusicStyleNote *pStyleNote ) : CDirectMusicEventItem()
	{
		*this = *pStyleNote;
	}
	CDirectMusicStyleNote( const DMUS_IO_SEQ_ITEM *piSeqNote ) : CDirectMusicEventItem()
	{
		//m_mtGridStart = 0;
		m_nTimeOffset = piSeqNote->nOffset;
		//m_dwVariation = 0;
		//m_fSelected = FALSE;

		m_mtDuration = piSeqNote->mtDuration;
		m_wMusicValue = 0;
		m_bVelocity = piSeqNote->bByte2;
		m_bTimeRange = 0;
		m_bDurRange = 0;
		m_bVelRange = 0;
		m_bInversionId = 0;
		m_bPlayModeFlags = DMUS_PLAYMODE_NONE;
		m_bMIDIValue = piSeqNote->bByte1;
		m_bNoteFlags = 0;
		m_cDiatonicOffset = 0;
	}

public:
	MUSIC_TIME	m_mtDuration;		// how long this note lasts
    WORD		m_wMusicValue;		// Position in scale.
    BYTE		m_bVelocity;		// Note velocity.
    BYTE		m_bTimeRange;		// Range to randomize start time.
    BYTE		m_bDurRange;		// Range to randomize duration.
    BYTE		m_bVelRange;		// Range to randomize velocity.
	BYTE		m_bInversionId;		// Identifies inversion group to which this note belongs
	BYTE		m_bPlayModeFlags;	// can override part ref
	BYTE		m_bMIDIValue;		// MIDI value displayed in PianoRoll
	BYTE		m_bNoteFlags;		// values from DMUS_NOTEF_FLAGS
	char		m_cDiatonicOffset;	// Design-time diatonic offset
};


class CCurveTracker;

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleCurve class

class CDirectMusicStyleCurve : public CDirectMusicEventItem
{
friend class CDirectMusicPattern;
friend class CDirectMusicPartRef;
friend class CDirectMusicPart;
friend class CMIDIMgr;
friend class CPianoRollStrip;
friend class CCurveStrip;
friend class CPropCurve;
friend class CCurveTracker;

public:
	CDirectMusicStyleCurve();
	CDirectMusicStyleCurve( const DMUS_IO_CURVE_ITEM *piSeqCurve );
	CDirectMusicStyleCurve( const CCurveItem* pCurveItem );
	CDirectMusicStyleCurve( const CDirectMusicStyleCurve* pCDirectMusicStyleCurve );
	~CDirectMusicStyleCurve();

protected:
	DWORD ComputeCurve( MUSIC_TIME* pmtIncrement );			// Computes value of Curve at time m_mtCurrent
	void SetDefaultResetValues( MUSIC_TIME mtLength  );

protected:
	MUSIC_TIME	m_mtDuration;		// how long this curve lasts
	MUSIC_TIME	m_mtResetDuration;	// how long after the end of the curve to reset the curve
	short		m_nStartValue;		// curve's start value
	short		m_nEndValue;		// curve's end value
	short		m_nResetValue;		// the value to which to reset the curve 
    BYTE		m_bEventType;		// type of curve
	BYTE		m_bCurveShape;		// shape of curve
	BYTE		m_bCCData;			// CC#
	BYTE		m_bFlags;			// Bit 1=TRUE means to send nResetValue. Otherwise, don't.
									//	   Other bits are reserved. 
	WORD		m_wParamType;		// RPN or NRPN parameter number.
	WORD		m_wMergeIndex;		// Allows multiple parameters to be merged (pitchbend, volume, and expression.)
	// UI fields
	MUSIC_TIME		m_mtCurrent;	// Offset into curve
	CRect			m_rectFrame;	// Used to draw bounding box
	CRect			m_rectSelect;	// Used to select curves
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleMarker class

class CDirectMusicStyleMarker : public CDirectMusicEventItem
{
public:
	CDirectMusicStyleMarker();
	~CDirectMusicStyleMarker(); 
	HRESULT Write( IStream* pIStream );
	HRESULT Read( IStream* pIStream, DWORD dwSize, DWORD dwExtra );

	DWORD		m_dwEnterVariation;		// variation bits for enter flag
	DWORD		m_dwEnterChordVariation;// variation bits for enter+chord flag
	DWORD		m_dwExitVariation;		// variation bits for enter flag
	DWORD		m_dwExitChordVariation;	// variation bits for enter+chord flag
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicNoteList class

class CDirectMusicNoteList : public AList
{
public:
	virtual ~CDirectMusicNoteList()
	{
		CDirectMusicStyleNote* pEvent;	
		while( m_pHead != NULL )
		{
			pEvent = RemoveHead();
			delete pEvent;
		}
	}
    CDirectMusicStyleNote *GetHead() const
	{
		return (CDirectMusicStyleNote *)AList::GetHead();
	};
    CDirectMusicStyleNote *RemoveHead()
	{
		return (CDirectMusicStyleNote *)AList::RemoveHead();
	};
	void SortNoteList( CDirectMusicPart* pDMPart );
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicCurveList class

class CDirectMusicCurveList : public AList
{
public:
	virtual ~CDirectMusicCurveList()
	{
		CDirectMusicStyleCurve* pEvent;	
		while( m_pHead != NULL )
		{
			pEvent = RemoveHead();
			delete pEvent;
		}
	}
    CDirectMusicStyleCurve *GetHead() const
	{
		return (CDirectMusicStyleCurve *)AList::GetHead();
	};
    CDirectMusicStyleCurve *RemoveHead()
	{
		return (CDirectMusicStyleCurve *)AList::RemoveHead();
	};
	void SortCurveList( CDirectMusicPart* pDMPart );
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicMarkerList class

class CDirectMusicMarkerList : public AList
{
public:
	virtual ~CDirectMusicMarkerList()
	{
		CDirectMusicStyleMarker* pEvent;	
		while( m_pHead != NULL )
		{
			pEvent = RemoveHead();
			delete pEvent;
		}
	}
    CDirectMusicStyleMarker *GetHead() const
	{
		return (CDirectMusicStyleMarker *)AList::GetHead();
	};
    CDirectMusicStyleMarker *RemoveHead()
	{
		return (CDirectMusicStyleMarker *)AList::RemoveHead();
	};
	void SortMarkerList( CDirectMusicPart* pDMPart );
	void CompactMarkerList( void );
	CDirectMusicStyleMarker *RemoveMarkerFromVariations( CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations );
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart class

class CDirectMusicPart
{
friend class CMIDIMgr;
friend class CDirectMusicPattern;
friend class CDirectMusicPartRef;
friend class CPianoRollStrip;
friend class CCurveStrip;
friend class CCurveTracker;
friend class PropPageNote;
friend class CPropCurve;
friend class CPropNote;
friend class CPropItem;
friend class CDialogNewPart;
friend class CVarSwitchStrip;

public:
	CDirectMusicPart( class CMIDIMgr* pMIDIMgr );
	~CDirectMusicPart();

public:
    HRESULT DM_LoadPart( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT DM_SavePart( IDMUSProdRIFFStream* pIRiffStream ) const;
    HRESULT DM_SaveNoteList( IDMUSProdRIFFStream* pIRiffStream ) const;
    HRESULT DM_SaveCurveList( IDMUSProdRIFFStream* pIRiffStream ) const;
    HRESULT DM_SaveMarkerList( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT DM_SaveDesignInfo( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT DM_SavePartInfoList( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT	SaveSelectedEventsToMIDITrack( IStream* pIStream, long lStartGrid, DWORD dwVariations ) const;
	BOOL	UpdateHasNotes( void );
	void    SetTimeSignature( DirectMusicTimeSig timeSig );
	void	SetNbrMeasures( WORD wNbrMeasures );
	BOOL	Quantize( DWORD dwFlags, BYTE bStrength, BYTE bGridsPerBeat );
	BOOL	QuantizeNote( CDirectMusicStyleNote* pDMNote, DWORD dwFlags, BYTE bStrength, BYTE bGridsPerBeat );
	BOOL	Velocitize( bool fAbsolute, bool fPercent, long lAbsoluteChangeStart, long lAbsoluteChangeEnd, BYTE bCompressMin, BYTE bCompressMax );
	BOOL	VelocitizeNote( CDirectMusicStyleNote* pDMNote, bool fAbsolute, bool fPercent, long lAbsoluteChange, BYTE bCompressMin, BYTE bCompressMax );
	void	CopyTo( CDirectMusicPart *pDMPart ) const;
	int		GetNumSelected( BYTE bType, DWORD dwVariations ) const;
	void	InsertNoteInAscendingOrder( CDirectMusicStyleNote *pNote );
	void	InsertCurveInAscendingOrder( CDirectMusicStyleCurve *pCurve );
	void	InsertMarkerInAscendingOrder( CDirectMusicStyleMarker *pMarker );
	inline MUSIC_TIME AbsTime( const CDirectMusicEventItem *pEvent ) const
	{
		if( pEvent )
		{
			return pEvent->m_nTimeOffset + GRID_TO_CLOCKS( pEvent->m_mtGridStart, this );
		}
		else
		{
			return NULL;
		}
	}
	void	CreateNoteCopyIfNeeded( CDirectMusicStyleNote *pOrigNote, DWORD dwVariations, BOOL fMakeCopy );
	void	CreateCurveCopyIfNeeded( CDirectMusicStyleCurve *pOrigCurve, DWORD dwVariations, BOOL fMakeCopy );
	void	MergeNoteList( CDirectMusicNoteList *plstEvents );
	void	MergeCurveList( CDirectMusicCurveList *plstEvents );
	void	MergeMarkerList( CDirectMusicMarkerList *plstEvents );
	CDirectMusicStyleNote *GetFirstNote( DWORD dwVariation ) const;
	CDirectMusicStyleCurve *GetFirstCurve( DWORD dwVariation ) const;
	CDirectMusicStyleMarker *GetFirstMarker( DWORD dwVariation ) const;
	CDirectMusicStyleNote* GetLastNote( DWORD dwVariation ) const;
	void	AddCurveTypesFromData( void );
	void	GetBoundariesOfSelectedNotes( DWORD dwVariations, long &lStartTime, long &lEndTime) const;
	MUSIC_TIME	GetGridLength( void ) const;
	MUSIC_TIME	GetClockLength( void ) const;
	MUSIC_TIME	GetLastNoteOff( void ) const;
	MUSIC_TIME	GetLastCurveEnd( void ) const;
	void	UpdateFromVarChoices( IDMUSProdFramework *pFramework, IPersistStream *pIPersistStream );
	bool	IsVarChoicesRowDisabled( short nRow );
	short	GetVarChoicesRowType( short nRow );
	short	GetVarChoicesNbrColumns( short nRow );
	bool	IsValidNote( CDirectMusicStyleNote *pNote );

protected:
	DWORD				  m_dwUseCount;				// Nbr PartRefs using this Part
	BOOL				  m_fSelecting;
	BOOL				  m_fQuantOrVelocitized;	// Used so we only quantize or velocitize a part once
	BYTE				  m_bSelectionCC;
	WORD				  m_wSelectionParamType;

protected:
	GUID				  m_guidPartID;
public:
	DirectMusicTimeSig	  m_TimeSignature;			// can override pattern's
	CMIDIMgr*			  m_pMIDIMgr;
protected:
	WORD				  m_wNbrMeasures;			// length of the Part
	DWORD				  m_dwVariationChoices[NBR_VARIATIONS];	// MOAW choices bitfield
	BYTE	              m_bPlayModeFlags;			// see PLAYMODE flags (in ioDMStyle.h)
	BYTE				  m_bInvertUpper;			// inversion upper limit
	BYTE				  m_bInvertLower;			// inversion lower limit
	CDirectMusicNoteList  m_lstNotes;				// list of notes
	CDirectMusicCurveList m_lstCurves;				// list of curves
	CDirectMusicMarkerList m_lstMarkers;			// list of markers
	BOOL				  m_fLoadedHasCurveTypes;	// Whether or not m_bHasCurveTypes was loaded
	DWORD				  m_dwFlags;				// various flags

	// Design-time specific information that should be saved
	DWORD				  m_dwDisabledChoices[NBR_VARIATIONS];
	DWORD				  m_dwVariationsDisabled;
	BYTE				  m_bAutoInvert;			// 0 = Manual inversion boundaries
													// 1 = Inversion boundaries set to note range
	BYTE				  m_bHasCurveTypes[17];		// Bit flags determining which curves are in this part.
													// Used for created empty curve strips in linked parts.
	BYTE				  m_bStyleTimeSigChange;	// Used by StyleDesigner.ocx
    CString				  m_strName;				// Part name

	// Design-time specific information that should not be saved
	DWORD				  m_dwVariationHasNotes;	// Set if the variation has notes in it
public:
	MUSIC_TIME			  m_mtClocksPerBeat;		// Recompute when time signature changes
	MUSIC_TIME			  m_mtClocksPerGrid;		// Recompute when time signature changes
};


typedef struct PianoRollUIState
{
	void*				pPianoRollData;				// UI info for Piano Roll
	DWORD				dwPianoRollDataSize;		// Size of UI info
}	PianoRollUIState;

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef class

class CDirectMusicPartRef
{
friend class CDirectMusicPattern;
friend class CMIDIMgr;
friend class CPianoRollStrip;
friend class CCurveStrip;
friend class CCurveTracker;
friend class CCurvePropPageMgr;
friend class CPropNote;
friend class CNoteTracker;
friend class CDialogNewPart;
friend class CDialogSelectPartRef;
friend class CDialogLinkExisting;
friend class CNotePropPageMgr;
friend class CPianoRollPropPageMgr;

public:
	CDirectMusicPartRef( CDirectMusicPattern* pPattern );
	~CDirectMusicPartRef();

	void	SetPart( CDirectMusicPart* pPart );
	void	RecalcMusicValues( void );
	void	RecalcMIDIValues( void );
	void    SetInversionBoundaries( void );
	BYTE	DMNoteToMIDIValue( const CDirectMusicStyleNote *pDMNote, BYTE bPlayModeFlags ) const;
	WORD	DMNoteToMusicValue( const CDirectMusicStyleNote *pDMNote, BYTE bPlayModeFlags ) const;
	WORD	DMNoteToLogicalScaleAccidental( const CDirectMusicStyleNote *pDMNote ) const;
    HRESULT DM_LoadPartRef( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT DM_SavePartRef( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT DM_SavePartRefInfoList( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT DM_SavePartRefDesignData( IDMUSProdRIFFStream* pIRiffStream ) const;
	void	InsertSeqItem( const DMUS_IO_SEQ_ITEM *pSeqItem, BOOL &fChanged, DWORD dwVariations, MUSIC_TIME mtGrid );
	void	InsertCurveItem( const CCurveItem *pCurveItem, BOOL &fChanged, DWORD dwVariations, MUSIC_TIME mtGrid );
	BOOL	UpdateNoteAfterMove( CDirectMusicStyleNote *pDMNote ) const;
	HRESULT	ImportEventsFromMIDITrack( IStream* pStream, long lStartGrid, DWORD dwVariations );
	void	InitializeVarChoicesEditor( void );

public:
	CDirectMusicPattern*	m_pPattern;
	CDirectMusicPart* m_pDMPart;// the Part to which this refers
	DWORD	m_dwPChannel;		// corresponds to port/device/midi channel
	BYTE	m_bVariationLockID; // parts with the same ID lock variations.
								// high bit is used to identify master Part
	BYTE	m_bSubChordLevel;	// tells which sub chord level this part wants
	BYTE	m_bPriority;		// Priority levels. Parts with lower priority
								// aren't played first when a device runs out of
								// notes
	BYTE	m_bRandomVariation;	// when set, matching variations play in random order
								// when clear, matching variations play sequentially
	GUID	m_guidOldPartID;	// GUID corresponding to the last part we referenced
    CString	m_strName;			// PartRef name
	BOOL	m_fHardLink;		// Set if this PartRef is 'Linked' to its part
	BOOL	m_fChanged;			// Used when pasting a MIDI file to multiple strips

	CTypedPtrList<CPtrList, PianoRollUIState*> m_lstPianoRollUIStates; // Used to store
								// PianoRoll Design data when loading a pattern track
								// in a segment
	IDMUSProdNode*	m_pVarChoicesNode; // The variation choices editor for this partref
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern class

class CDirectMusicPattern : public IPersistStream, public IDMUSProdTimelineCallback
{
friend class CDirectMusicPart;
friend class CDirectMusicPartRef;
friend class CMIDIMgr;
friend class CPianoRollStrip;
friend class CCurveStrip;
friend class CDialogNewPart;
friend class CCurvePropPageMgr;
friend class CDialogSelectPartRef;
friend class CDialogLinkExisting;
friend class CTabPatternPattern;
friend class CPatternLengthDlg;

public:
    CDirectMusicPattern( CMIDIMgr* pMIDIMgr, BOOL fMotif );
	~CDirectMusicPattern();

	// IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IDMUSProdTimelineCallback functions
	HRESULT STDMETHODCALLTYPE OnDataChanged( IUnknown* punkStripMgr );

	//Additional functions
public:
	MUSIC_TIME CalcLength() const;
	/*
	static BYTE MusicValueToNote( WORD wMusicValue, BYTE bPlayModes, const DMUS_SUBCHORD &dmSubChord );
	static WORD NoteToMusicValue( BYTE bMIDINote, BYTE bPlayModes, const DMUS_SUBCHORD &dmSubChord );
	*/

protected:
	/*
	static unsigned char OldMusicValueToNote( unsigned short value, char scalevalue, long keypattern,
									   char keyroot, long chordpattern, char chordroot, char count );
	static unsigned short OldNoteToMusicValue( unsigned char note, long keypattern, char keyroot,
											   long chordpattern, char chordroot );
	*/
	CDirectMusicPartRef* AllocPartRef();
	void DeletePartRef( CDirectMusicPartRef* pPartRef );

	CDirectMusicPart* FindPart(DWORD dwChannelID) const;
	CDirectMusicPartRef* FindPartRefByPChannel(DWORD dwChannelID) const;
	CDirectMusicPartRef* FindPartRefByGUID( GUID guid ) const;
    HRESULT DM_SaveSinglePattern( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SavePatternChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SavePatternInfoList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SavePatternRhythm( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveMotifSettingsChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_LoadPattern( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT DM_SavePatternEditInfo( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_LoadPatternEditInfo( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, BOOL* fLoadedChordStripDesign );
	HRESULT DM_SaveStyleHeader( IDMUSProdRIFFStream* pIRiffStream );

	// Methods to set Pattern data
	void SetNbrMeasures( WORD wNbrMeasures );
	void SetTimeSignature( DirectMusicTimeSig timeSig, BOOL fSyncWithEngine );
    void SetNodeName( LPCTSTR strName );
	void SetFlags( DWORD dwFlags );

public:
	void SetModified( BOOL fModified );
	void RecalcMusicValues( void );
	void RecalcMIDIValues( void );
	DWORD GetNumHardLinks( const CDirectMusicPart* pPart ) const;

private:
    DWORD			   m_dwRef;
	BOOL			   m_fModified;

	CMIDIMgr*		   m_pMIDIMgr;

	DWORD			   m_dwPlaybackID;

public:
	BOOL			   m_fInLoad;

// Pattern data
private:
    DWORD				m_dwDefaultKeyPattern;	// Default Key (Key/Chord for composing).
    DWORD				m_dwDefaultChordPattern;// Default Chord (Key/Chord for composing).
    BYTE				m_bDefaultKeyRoot;		// Default Key Root (Key/Chord for composing).
    BYTE				m_bDefaultChordRoot;	// Default Chord Root (Key/Chord for composing).
	BOOL				m_fKeyFlatsNotSharps;	// Whether default key is displayed as flats or sharps
	BOOL				m_fChordFlatsNotSharps;	// Whether default chord is displayed as flats or sharps

	DirectMusicTimeSig	m_TimeSignature;		// Patterns can override the Style's Time sig.
	WORD				m_wNbrMeasures;			// length in measures
	WORD				m_wEmbellishment;		// Fill, Break, Intro, End, Normal, Motif
	BYTE				m_bGrooveBottom;		// bottom of groove range
	BYTE				m_bGrooveTop;			// top of groove range
	BYTE				m_bDestGrooveBottom;	// bottom of destination groove range
	BYTE				m_bDestGrooveTop;		// top of destination groove range
    DWORD               m_dwFlags;				// various flags (DMUS_PATTERNF_*)
	DWORD*				m_pRhythmMap;			// variable array of rhythms for chord matching
	CTypedPtrList<CPtrList, CDirectMusicPartRef*> m_lstPartRefs;	// list of part references

	// Motif specific data
    DWORD				m_dwRepeats;			// Number of repeats. By default, 0. 
    MUSIC_TIME			m_mtPlayStart;			// Start of playback. By default, 0. 
    MUSIC_TIME			m_mtLoopStart;			// Start of looping portion. By default, 0. 
    MUSIC_TIME			m_mtLoopEnd;			// End of loop. Must be greater than dwPlayStart.
												// By default, equal to length of motif. 
    DWORD				m_dwResolution;			// Default resolution. 

public:
    CString			    m_strName;				// Pattern name
};

#endif // __PATTERN_H__
