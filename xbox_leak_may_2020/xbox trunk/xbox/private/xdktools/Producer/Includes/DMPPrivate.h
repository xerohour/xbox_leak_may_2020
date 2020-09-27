/************************************************************************
*                                                                       *
*   DMPPrivate.h -- This module contains the private APIs for           *
*                   DirectMusic Producer                                *
*                                                                       *
*   Copyright (c) 1998-2000, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef DMPPRIVATE_H__27383763_5F0B_11d2_8916_00C04FBF8D15__INCLUDED_
#define DMPPRIVATE_H__27383763_5F0B_11d2_8916_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define COM_NO_WINDOWS_H
#include <objbase.h>

#ifdef __cplusplus
extern "C"{
#endif 

typedef __int64 REFERENCE_TIME;
typedef long    MUSIC_TIME;


/* Forward Declarations */ 

interface IPrivatePatternTrack;
interface IDMUSProdWaveTrackDownload;
interface IDMUSProdUnpackingFiles;
interface IPrivateTransport;
interface IDMUSProdWaveTimelineDraw;
interface IDirectMusicSegmentState;
interface IDMUSProdTimeline;
interface IDMUSProdConductorPrivate;
interface IDMUSProdNode;
interface IDMUSProdStrip;
#ifndef __cplusplus 
typedef interface IPrivatePatternTrack IPrivatePatternTrack;
typedef interface IDMUSProdWaveTrackDownload IDMUSProdWaveTrackDownload;
typedef interface IDMUSProdUnpackingFiles IDMUSProdUnpackingFiles;
typedef interface IPrivateTransport IPrivateTransport;
typedef interface IDMUSProdWaveTimelineDraw IDMUSProdWaveTimelineDraw;
typedef interface IDirectMusicSegmentState IDirectMusicSegmentState;
typedef interface IDMUSProdTimeline IDMUSProdTimeline;
typedef interface IDMUSProdConductorPrivate IDMUSProdConductorPrivate;
typedef interface IDMUSProdNode IDMUSProdNode;
typedef interface IDMUSProdStrip IDMUSProdStrip;
#endif /* __cplusplus */

#define CF_AUDIOPATH "DMUSProd v.1 AudioPath"
#define CF_AUDIOPATHLIST "DMUSProd v.1 AudioPath List"
#define CF_AUDIOPATH_PCHANNEL "DMUSProd v.1 AudioPath PChannel Format"
#define CF_AUDIOPATH_EFFECT "DMUSProd v.1 AudioPath Effect Format"
#define CF_AUDIOPATH_BUFFER "DMUSProd v.1 AudioPath Buffer Format"
#define CF_SEGMENT "DirectMusic Producer v.1 Segment"
#define CF_SECTION "DirectMusic Producer v.1 Section"
#define CF_TEMPLATE "DirectMusic Producer v.1 Template"
#define CF_CHORDLIST "DMUSProd v.1 Chordlist"
#define CF_CHORDMAP_LIST "DMUSProd v.1 ChordMap list"
#define CF_TIMELINE "DMUSProd v.1 TimelineDataObject"
#define CF_BAND "DirectMusic Producer v.1 Band"
#define CF_BANDLIST "DirectMusic Producer v.1 Band List"
#define CF_STYLE "DMUSProd v.1 Style"
#define CF_MOTIF "DMUSProd v.1 Motif"
#define CF_MOTIFLIST "DMUSProd v.1 Motif List"
#define CF_PATTERN "DMUSProd v.1 Pattern"
#define CF_PATTERNLIST "DMUSProd v.1 Pattern List"
#define CF_DLS_COLLECTION "Jazz v.1 Dls Collection"
#define CF_DLS_WAVE "Jazz v.1 Dls Wave"
#define CF_DLS_INSTRUMENT "Jazz v.1 Dls Instrument"
#define CF_PERSONALITY "DMUSProd v.1 ChordMap"
#define CF_COMMANDLIST "DirectMusic Producer v.1 Commandlist"
#define CF_LYRICLIST "DirectMusic Producer v.1 Lyriclist"
#define CF_MARKERLIST "DirectMusic Producer v.1 Markerlist"
#define CF_MUSICNOTE "DMUSProd v.1 MusicNote"
#define CF_CURVE "DMUSProd v.1 Curve"
#define CF_MUSICNOTEANDCURVE "DMUSProd v.1 MusicNotesAndCurves"
#define CF_STYLEMARKER "DMUSProd v.1 StyleMarker"
#define CF_PARAMTRACK "DMUSProd v.1 Param Track"
#define CF_SCRIPTTRACK "DMUSProd v.1 Script Track"
#define CF_SEGMENTTRACK "DMUSProd v.1 Segment Track"
#define CF_SEQUENCELIST "DMUSProd v.1 Sequencelist"
#define CF_MIDIFILE "Standard MIDI File"
#define CF_CURVELIST "DMUSProd v.1 Curvelist"
#define CF_STYLEREFLIST "DMUSProd v.1 StyleReflist"
#define CF_TEMPOLIST "Jazz v.1 Tempolist"
#define CF_TIMESIGLIST "DMUSProd v.1 TimeSigList"
#define CF_WAVETRACK "DMUSProd v.1 Wave Track"
#define CF_CONTAINER "DMUSProd v.1 Container"
#define CF_CONTAINERLIST "DMUSProd v.1 Container List"
#define CF_SCRIPT "DMUSProd v.1 Script"
#define CF_SCRIPTLIST "DMUSProd v.1 Script List"
#define CF_GRAPH "DMUSProd v.1 Graph"
#define CF_GRAPHLIST "DMUSProd v.1 Graph List"
#define CF_PERSREFLIST "Jazz v.1 PersReflist"
#define CF_MELGENLIST "DMUSProd v.1 MelGenList"
#define CF_SONG "DMUSProd v.1 Song"
#define CF_SONGLIST "DMUSProd v.1 Song List"
#define CF_VIRTUAL_SEGMENT_LIST "DMUSProd v.1 Song VSeg List"
#define CF_TRACK_LIST "DMUSProd v.1 Song Trk List"
#define CF_TRANSITION_LIST "DMUSProd v.1 Song Tran List"
#define TOOL_NO_PCHANNELS 999999
#define MSP_PIANOROLL_VARIATIONS ((STRIPPROPERTY)1024)
#define MSP_PIANOROLL_TRACK     ((STRIPPROPERTY)1025)
#define MSP_PIANOROLL_GUID      ((STRIPPROPERTY)1026)
#define FOURCC_TIMELINE_BOUNDARY mmioFOURCC('t','l','b','d')
#define FOURCC_TIMELINE_LIST mmioFOURCC('t','l','i','l')
#define FOURCC_TIMELINE_CLIPBOARD mmioFOURCC('t','l','c','l')
#define FOURCC_TIMELINE_CLIP_NAME mmioFOURCC('t','l','c','n')
#define FOURCC_TIMELINE_CLIP_DATA mmioFOURCC('t','l','c','d')
#define FOURCC_CF_DMUSPROD_FILE	mmioFOURCC('d','m','p','f')


////////////////////////////////////////////////////////////////////////////////
// Direct Music Style design-time RIFF chunk headers

#define DMUS_FOURCC_STYLE_UNDO_FORM         mmioFOURCC('s','t','u','n')
#define DMUS_FOURCC_CURVE_UI_LIST           mmioFOURCC('c','r','v','u')
#define DMUS_FOURCC_ALLCURVES_UI_CHUNK      mmioFOURCC('c','v','a','u')
#define DMUS_FOURCC_CURVE_UI_CHUNK          mmioFOURCC('c','v','s','u')
#define DMUS_FOURCC_QUANTIZE_CHUNK          mmioFOURCC('p','q','t','z')
#define DMUS_FOURCC_VELOCITIZE_CHUNK        mmioFOURCC('p','v','c','z')
#define DMUS_FOURCC_TIMELINE_CHUNK          mmioFOURCC('p','t','l','c')
#define DMUS_FOURCC_CHORDSTRIP_LIST         mmioFOURCC('p','c','s','l')
#define DMUS_FOURCC_CHORDSTRIP_UI_CHUNK     mmioFOURCC('p','c','s','u')
#define DMUS_FOURCC_PIANOROLL_LIST          mmioFOURCC('p','p','r','l')
#define DMUS_FOURCC_PIANOROLL_CHUNK         mmioFOURCC('p','p','r','c')
#define DMUS_FOURCC_PATTERN_DESIGN          mmioFOURCC('p','p','n','d')
#define DMUS_FOURCC_PART_DESIGN             mmioFOURCC('p','p','t','d')
#define DMUS_FOURCC_PARTREF_DESIGN          mmioFOURCC('p','p','f','d')
#define DMUS_FOURCC_STYLE_UI_CHUNK          mmioFOURCC('s','t','y','u')
#define DMUS_FOURCC_PATTERN_UI_CHUNK        mmioFOURCC('p','t','n','u')
#define DMUS_FOURCC_OLDGUID_CHUNK           mmioFOURCC('p','o','g','c')
#define DMUS_FOURCC_DEFAULT_BAND_UI_CHUNK   mmioFOURCC('d','b','n','d')


////////////////////////////////////////////////////////////////////////////////
// Direct Music Style design-time structures

#pragma pack(2)

struct ioDMStyleUI
{
    int     m_nSplitterYPos;            // Splitter Y position
    DWORD   m_dwNotationType;           // Notation type (0 = pianoroll, 1 = hybrid)
};

struct ioDMPatternUI
{
    WORD    m_wPad;
    DWORD   m_dwDefaultKeyPattern;      // Default KeyPattern   (Key/Chord for composing)   
    DWORD   m_dwDefaultChordPattern;    // Default ChordPattern (Key/Chord for composing)
    BYTE    m_bDefaultKeyRoot;          // Default Key Root     (Key/Chord for composing)   
    BYTE    m_bDefaultChordRoot;        // Default Chord Root   (Key/Chord for composing)   
    BOOL    m_fDefaultKeyFlatsNotSharps;    // enharmonic for key (Key/Chord for composing)
    BOOL    m_fDefaultChordFlatsNotSharps;  // enharmonic for chord (Key/Chord for composing)
};

struct ioDMPatternUI8
{
    WORD    m_wPad;
    DWORD   m_dwDefaultKeyPattern;      // Default KeyPattern   (Key/Chord for composing)   
    DWORD   m_dwDefaultChordPattern;    // Default ChordPattern (Key/Chord for composing)
    BYTE    m_bDefaultKeyRoot;          // Default Key Root     (Key/Chord for composing)   
    BYTE    m_bDefaultChordRoot;        // Default Chord Root   (Key/Chord for composing)   
    BOOL    m_fDefaultKeyFlatsNotSharps;    // enharmonic for key (Key/Chord for composing)
    BOOL    m_fDefaultChordFlatsNotSharps;  // enharmonic for chord (Key/Chord for composing)
    double  m_dblZoom;                  // Horizontal zoom amount
    double	m_dblHorizontalScroll;      // Horizontal scroll amount
    LONG	m_lVerticalScroll;          // Vertical scroll amount
    DWORD   m_dwSnapTo;                 // Snap-to setting
    LONG    m_lFunctionbarWidth;        // Function bar width
};

struct ioDMStylePartDesign
{
    DWORD   m_dwVariationsDisabled;     // Which variations have been disabled
    DWORD   m_dwDisabledChoices[32];    // Stores a copy of the disabled variations flags
    BYTE    m_bAutoInvert;              // 0 = Manual inversion boundaries
                                        // 1 = Inversion boundaries set to note range
    BYTE    m_bHasCurveTypes[17];       // Bit flags determining which curves are in this part.
                                        // Used for created empty curve strips in linked parts.
    BYTE    m_bStyleTimeSigChange;      // 0 = Style's TimeSig change did not affect this Part
                                        // 1 = Part's TimeSig updated because of change to Style's Timesig
};

struct ioDMStylePartRefDesign
{
    BOOL    m_fHardLink;                // If set, this PartRef is hard linked to its part
};

struct ioDMPartOldGUID
{
    GUID    m_guidOldPartID;            // GUID for deleting old parts when they change
};

#define PATTERN_QUANTIZE_START_TIME 0x1
#define PATTERN_QUANTIZE_DURATION   0x2

struct ioDMPatternQuantize
{
    WORD    m_wQuantizeTarget;          // Selected Notes / Part / Pattern
    BYTE    m_bResolution;              // 1 = Beat, 2 = half beat, 3 = third beat, etc. up to 24
    BYTE    m_bStrength;                // Strength, 0% - 100%
    DWORD   m_dwFlags;                  // PATTERN_QUANTIZE_(START_TIME and/or DURATION)
};

#define PATTERN_VELOCITIZE_PERCENT  (0x0)
#define PATTERN_VELOCITIZE_LINEAR   (0x1)
#define PATTERN_VELOCITIZE_COMPRESS (0x2)
#define PATTERN_VELOCITIZE_METHOD_MASK  (0x3)

struct ioDMPatternVelocitize
{
    WORD    m_wVelocityTarget;          // Selected Notes / Part / Pattern
    BYTE    m_bCompressMin;             // Compress min value
    BYTE    m_bCompressMax;             // Compress max value
    LONG    m_lAbsoluteChangeStart;     // Absolute change (either -99%..200% or -127..127)
    DWORD   m_dwVelocityMethod;         // PATTERN_VELOCITIZE_(PERCENT or LINEAR or COMPRESS)
    LONG    m_lAbsoluteChangeEnd;       // Absolute change (either -99%..200% or -127..127)
};

struct ioDMPatternTimeline
{
    double  m_dblZoom;                  // Horizontal zoom amount
    DWORD   m_dwHorizontalScroll;       // Horizontal scroll amount
    DWORD   m_dwVerticalScroll;         // Vertical scroll amount
    DWORD   m_dwSnapTo;                 // Snap-to setting
    LONG    m_lFunctionbarWidth;        // Function bar width
};

#define CHORDSTRIP_MODE_MIDI_CONSTANT       1
#define CHORDSTRIP_MODE_FUNCTION_CONSTANT   2

struct ioDMChordStripUI
{
    BYTE    m_bMode;                    // See CHORDSTRIP_MODE_ defines
    BYTE    m_bPad;
};

#pragma pack()

// Definitions of the structs used in drawing waves on the wavetrack
// =================================================================

#define WAVE_MARKSTART      1
#define WAVE_MARKEND        WAVE_MARKSTART << 1
#define WAVE_MARKBOTHENDS   WAVE_MARKSTART | WAVE_MARKEND

#define WAVE_LOOPED         1
#define WAVE_STREAMING      WAVE_LOOPED << 1

typedef struct WAVE_DRAW_PARAMS
{
    DWORD       cbSize;         // Size of the struct; must be initialized to a correct value before passing it
    BOOL        bErase;         // Should we erase the background before drawing this wave?
    COLORREF    clrBackground;  // The background color for the wave
    COLORREF    clrForeground;  // Foreground color for the wave
    COLORREF    clrStartMarker; // Color to mark the wave start
    COLORREF    clrEndMarker;   // Color to mark the wave end
    COLORREF    clrClipMarker;  // Color to show the wave boundaries if it's clipped 
    DWORD       dwDrawOptions;  // Can be WAVE_MARKBOTHENDS, WAVE_MARKSTART, WAVE_MARKEND
} WaveDrawParams;

typedef struct WAVE_TIMELINE_INFO_PARAMS
{
    DWORD           cbSize;         // Size of the struct; must be initialized to a correct value before passing it
    REFERENCE_TIME  rtStart;        // Start time for the wave on Timeline
    REFERENCE_TIME  rtOffset;       // Offset into the wave (the wave will actually start playing after mtOffset number of nanoseconds from mt_Start)
    REFERENCE_TIME  rtDuration;     // The time for which the wave plays (the wave may not play till the end)
    DWORD           dwGroupBits;    // TrackGroup that the wavetrack for this wave belongs to...
    LONG            lPitch;         // Fine tune value set by the wave track
    DWORD           dwLoopStart;    // Loop start set by the wave track; valid only if WAVE_LOOP bit is set in the dwFlags bitfield
    DWORD           dwLoopEnd;      // Loop end set by the wave track; valid only if WAVE_LOOP bit is set in the dwFlags bitfield
    DWORD           dwFlags;        // Can be WAVE_LOOP etc.
}WaveTimelineInfoParams;

typedef struct WAVE_INFO_PARAMS
{
    DWORD           cbSize;             // Size of the struct
    DWORD           dwWaveDuration;     // The length of the wave
    DWORD           dwLoopType;         // The type for this loop
    DWORD           dwLoopStart;        // The start sample for the loop    
    DWORD           dwLoopEnd;          // The end sample for the loop  
    GUID            guidVersion;        // The version for the wave
    DWORD           dwFlags;            // Can be WAVE_LOOPED, WAVE_STREAMING

}WaveInfoParams;
//=========================================================================================================

////////////////////////////////////////////////////////////////////////////////
// Segment-specific Producer-Only flags
#define SEG_PRODUCERONLY_AUDITIONONLY 0x1

////////////////////////////////////////////////////////////////////////////////
// Structure on disk that defines Segment-specific Producer-Only flags
typedef struct _IOProducerOnlyChunk
{
    DWORD   dwProducerOnlyFlags;
} IOProducerOnlyChunk;


////////////////////////////////////////////////////////////////////////////////
// Private Segment Structures/Defines/Enums

// Sequence strip quantize information
#define SEQUENCE_QUANTIZE_START_TIME    0x1
#define SEQUENCE_QUANTIZE_DURATION      0x2

typedef enum
{
    QUANTIZE_TARGET_SELECTED,
    QUANTIZE_TARGET_PART,
    QUANTIZE_TARGET_SEQUENCE
} QUANTIZE_TARGET;

struct SequenceQuantize
{
    WORD    m_wQuantizeTarget;          // Selected Notes / Part / Pattern
    BYTE    m_bResolution;              // 1 = Beat, 2 = half beat, 3 = third beat, etc. up to 24
    BYTE    m_bStrength;                // Strength, 0% - 100%
    DWORD   m_dwFlags;                  // PATTERN_QUANTIZE_(START_TIME and/or DURATION)
};

// Sequence strip edit velocity information
#define SEQUENCE_VELOCITIZE_PERCENT     (0x0)
#define SEQUENCE_VELOCITIZE_LINEAR      (0x1)
#define SEQUENCE_VELOCITIZE_COMPRESS    (0x2)
#define SEQUENCE_VELOCITIZE_METHOD_MASK (0x3)

struct SequenceVelocitize
{
    WORD    m_wVelocityTarget;          // Selected Notes / Part / Pattern
    BYTE    m_bCompressMin;             // Compress min value
    BYTE    m_bCompressMax;             // Compress max value
    LONG    m_lAbsoluteChangeStart;     // Absolute change start (either -99%..200% or -127..127)
    LONG    m_lAbsoluteChangeEnd;       // Absolute change end (either -99%..200% or -127..127)
    DWORD   m_dwVelocityMethod;         // SEQUENCE_VELOCITIZE_(PERCENT or LINEAR or COMPRESS)
};

/////////////////////////////////////////////////////////////////////////////
// RIFF tags
//

#define FOURCC_BAND_FORM        mmioFOURCC('A','A','B','N')
#define FOURCC_CLICK_LIST       mmioFOURCC('A','A','C','L')
//#define FOURCC_KEYBOARD_FORM    mmioFOURCC('S','J','K','B')
#define FOURCC_PATTERN_FORM     mmioFOURCC('A','A','P','T')
#define FOURCC_SECTION_FORM     mmioFOURCC('A','A','S','E')
//#define FOURCC_SONG_FORM        mmioFOURCC('A','A','S','O')
#define FOURCC_STYLE_FORM       mmioFOURCC('A','A','S','Y')

//#define FOURCC_AUTHOR           mmioFOURCC('a','u','t','h')
#define FOURCC_BAND             mmioFOURCC('b','a','n','d')
#define FOURCC_CHORD            mmioFOURCC('c','h','r','d')
#define FOURCC_CLICK            mmioFOURCC('c','l','i','k')
#define FOURCC_COMMAND          mmioFOURCC('c','m','n','d')
//#define FOURCC_COPYRIGHT        mmioFOURCC('c','p','y','r')
#define FOURCC_CURVE            mmioFOURCC('c','u','r','v')
//#define FOURCC_KEYBOARD         mmioFOURCC('k','y','b','d')
//#define FOURCC_LYRIC            mmioFOURCC('l','y','r','c')
#define FOURCC_MUTE             mmioFOURCC('m','u','t','e')
#define FOURCC_NOTE             mmioFOURCC('n','o','t','e')
#define FOURCC_PATTERN          mmioFOURCC('p','a','t','t')
#define FOURCC_PERSONALITYNAME  mmioFOURCC('p','r','n','m')
#define FOURCC_PERSONALITYREF   mmioFOURCC('p','r','e','f')
//#define FOURCC_PHRASE           mmioFOURCC('p','h','r','s')
//#define FOURCC_PPQN             mmioFOURCC('p','p','q','n')
#define FOURCC_SECTION          mmioFOURCC('s','e','c','n')
#define FOURCC_SECTIONUI        mmioFOURCC('s','c','u','i')
#define FOURCC_STYLE            mmioFOURCC('s','t','y','l')
#define FOURCC_STYLEINFO        mmioFOURCC('i','n','f','o')
#define FOURCC_STYLEREF         mmioFOURCC('s','r','e','f')
//#define FOURCC_TITLE            mmioFOURCC('t','i','t','l')

/*

#define RIFF_TAG    MAKETAG('R','I','F','F')
#define LIST_TAG    MAKETAG('L','I','S','T')
#define WAVE_TAG    MAKETAG('W','A','V','E')
#define FMT__TAG    MAKETAG('f','m','t',' ')
#define DATA_TAG    MAKETAG('d','a','t','a')
#define FACT_TAG    MAKETAG('f','a','c','t')

#define SONG_FORM           MAKETAG('A','A','S','O')
#define KEYBOARD_FORM       MAKETAG('S','J','K','B')
#define KEYBOARD_TAG        MAKETAG('k','y','b','d')
#define TITLE_TAG           MAKETAG('t','i','t','l')
#define AUTHOR_TAG          MAKETAG('a','u','t','h')
#define COPYRIGHT_TAG       MAKETAG('c','p','y','r')
#define SECTION_FORM        MAKETAG('A','A','S','E')
#define SECTION_TAG         MAKETAG('s','e','c','n')
#define SECTIONUI_TAG       MAKETAG('s','c','u','i')
#define STYLEREF_TAG        MAKETAG('s','r','e','f')
#define PERSONALITYREF_TAG  MAKETAG('p','r','e','f')
#define PERSONALITYNAME_TAG MAKETAG('p','r','n','m')
#define BAND_FORM           MAKETAG('A','A','B','N')
#define BAND_TAG            MAKETAG('b','a','n','d')
#define CHORD_TAG           MAKETAG('c','h','r','d')
#define COMMAND_TAG         MAKETAG('c','m','n','d')
#define MUTE_TAG            MAKETAG('m','u','t','e')
#define NOTE_TAG            MAKETAG('n','o','t','e')
#define CURVE_TAG           MAKETAG('c','u','r','v')
#define LYRIC_TAG           MAKETAG('l','y','r','c')
#define PHRASE_TAG          MAKETAG('p','h','r','s')
#define STYLE_FORM          MAKETAG('A','A','S','Y')
#define STYLE_TAG           MAKETAG('s','t','y','l')
#define STYLEINFO_TAG       MAKETAG('i','n','f','o')
#define PATTERN_FORM        MAKETAG('A','A','P','T')
#define PATTERN_TAG         MAKETAG('p','a','t','t')
#define CLICK_LIST          MAKETAG('A','A','C','L')
#define CLICK_TAG           MAKETAG('c','l','i','k')
*/

typedef struct _DMUSProdTimeSignature
{
    BYTE    bBeatsPerMeasure;
    BYTE    bBeat;
    WORD    wGridsPerBeat;
} DMUSProdTimeSignature;

typedef struct _DMUSProdMotifData
{
    WCHAR*      pwszMotifName;
    WCHAR*      pwszOldMotifName;
} DMUSProdMotifData;

#define LOGICAL_PART_MELODY     0
#define LOGICAL_PART_STRING     1
#define LOGICAL_PART_GUITAR     2
#define LOGICAL_PART_PIANO      3
#define LOGICAL_PART_BASS       4
#define LOGICAL_PART_DRUM       5
#define DMUS_LOGICAL_PART_DRUM  9

typedef struct DMUSProdChordMapInfo
{
    WORD    wSize;
    long    lScalePattern;
    BOOL    fUseFlats;
    BYTE    bKey;           // 0-23
    BYTE    bPad;
} DMUSProdChordMapInfo;

typedef struct DMUSProdChordMapUIInfo
{
    WORD    wSize;
    BOOL    fLockAllScales;
    BOOL    fSyncLevelOneToAll;
} DMUSProdChordMapUIInfo;

struct BandStrip_InstrumentItem
{
    long lPhysicalTime;
    DWORD dwPChannel;
    DWORD dwPatch;
};

typedef struct _DMUSProdToolInfo
{
    WORD	wSize;
	CLSID	clsidTool;
	DWORD	dwFirstPChannel;
	WCHAR	awchToolName[65];
	WCHAR	awchPChannels[128];
} DMUSProdToolInfo;

typedef struct _DMUSProdReferencedNodes
{
    DWORD   dwArraySize;
	IDMUSProdNode **apIDMUSProdNode;
	DWORD	dwErrorLength;
	WCHAR	*wcstrErrorText;
} DMUSProdReferencedNodes;

typedef struct ioCFProducerFile			// CF_DMUSPROD_FILE clipboard format.  Struct followed
{										// by variable length wchar_t containing file name.
    GUID	guidFile;
} ioCFProducerFile;


/* Interfaces */

/*////////////////////////////////////////////////////////////////////
// IPrivatePatternTrack */
#undef  INTERFACE
#define INTERFACE  IPrivatePatternTrack
DECLARE_INTERFACE_(IPrivatePatternTrack, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IPrivatePatternTrack */
    STDMETHOD(SetPattern)           (THIS_ IDirectMusicSegmentState* pSegState,
                                           IStream* pStream,
                                           DWORD* pdwLength) PURE;
    STDMETHOD(SetVariationByGUID)   (THIS_ IDirectMusicSegmentState* pSegState,
                                           DWORD dwVariationFlags,
                                           REFGUID rguidPart,
                                           DWORD dwPChannel) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IPrivatePatternTrack9 */
#undef  INTERFACE
#define INTERFACE  IPrivatePatternTrack9
DECLARE_INTERFACE_(IPrivatePatternTrack9, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IPrivatePatternTrack9 */
    STDMETHOD(SetVariationMaskByGUID)(THIS_ IDirectMusicSegmentState* pSegState,
                                           DWORD dwVariationFlags,
                                           REFGUID rguidPart,
                                           DWORD dwPChannel) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdWaveTrackDownload */
#undef  INTERFACE
#define INTERFACE  IDMUSProdWaveTrackDownload
DECLARE_INTERFACE_(IDMUSProdWaveTrackDownload, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdWaveTrackDownload */
    STDMETHOD(AddPChannel)          (THIS_ DWORD dwPChannel,
                                           BOOL bUseAudioPath) PURE;
    STDMETHOD(RemovePChannel)       (THIS_ DWORD dwPChannel,
                                           BOOL bUseAudioPath) PURE;
    STDMETHOD(ChangePChannel)       (THIS_ DWORD dwOldPChannel,
                                           DWORD dwNewPChannel,
                                           BOOL bUseAudioPath) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdUnpackingFiles */
#undef  INTERFACE
#define INTERFACE  IDMUSProdUnpackingFiles
DECLARE_INTERFACE_(IDMUSProdUnpackingFiles, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    /* IDMUSProdUnpackingFiles */
    STDMETHOD(GetDocRootOfEmbeddedFile)(THIS_ IUnknown* pIDocType,
                                           BSTR bstrObjectName,
                                           IUnknown** ppIDocRootNode) PURE;
    STDMETHOD(AddToNotifyWhenLoadFinished)(THIS_ IUnknown* pIDocType,
                                           BSTR bstrObjectName,
                                           IUnknown* pINotifySink,
                                           GUID* pguidFile) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IPrivateTransport */
#undef  INTERFACE
#define INTERFACE  IPrivateTransport
DECLARE_INTERFACE_(IPrivateTransport, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IPrivateTransport */
    STDMETHOD(PlayFromStart)    (THIS) PURE;
    STDMETHOD(PlayFromCursor)   (THIS) PURE;
    STDMETHOD(Record)           (THIS) PURE;
    STDMETHOD(Transition)       (THIS) PURE;
    STDMETHOD(SpaceBarPress)    (THIS) PURE;
    STDMETHOD(PlaySecondarySegment)(THIS_ int nSecondarySegment) PURE;
};

#ifdef __cplusplus
/*////////////////////////////////////////////////////////////////////
// IDMUSProdWaveTimelineDraw */
#undef  INTERFACE
#define INTERFACE  IDMUSProdWaveTimelineDraw
DECLARE_INTERFACE_(IDMUSProdWaveTimelineDraw, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IWaveTimelineDraw */
    STDMETHOD(DrawWave)         (THIS_ IDMUSProdTimeline* pITimeline,
                                       HDC hDC,
                                       const CRect& rcClient,
                                       const long lScrollOffset,
                                       const WaveDrawParams* pWaveDrawParams,
                                       const WaveTimelineInfoParams* pWaveTimelineInfoParams) PURE;
    STDMETHOD(GetWaveInfo)      (THIS_ WaveInfoParams* pWaveInfoParams) PURE;
    STDMETHOD(SampleToRefTime)  (THIS_ DWORD dwSample,
                                       REFERENCE_TIME* prtSampleTime,
                                       LONG lFineTuneCents) PURE;
    STDMETHOD(RefTimeToSample)  (THIS_ REFERENCE_TIME rtSampleTime,
                                       DWORD* pdwSample,
                                       LONG lFineTuneCents) PURE;
};
#endif // __cplusplus

/*////////////////////////////////////////////////////////////////////
// IDMUSProdConductorPrivate */
#undef  INTERFACE
#define INTERFACE  IDMUSProdConductorPrivate
DECLARE_INTERFACE_(IDMUSProdConductorPrivate, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdConductorPrivate */
    STDMETHOD(GetDefaultAudiopathNode)(THIS_ IDMUSProdNode** ppAudiopathNode) PURE;
	STDMETHOD(PleaseRedownload)(THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdTempoMapMgr */
#undef  INTERFACE
#define INTERFACE  IDMUSProdTempoMapMgr
DECLARE_INTERFACE_(IDMUSProdTempoMapMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdTempoMapMgr */
    STDMETHOD(ClocksToRefTime)  (THIS_ long lTime,
                                       REFERENCE_TIME *prtRefTime) PURE;
    STDMETHOD(MeasureBeatToRefTime)  (THIS_ DWORD dwGroupBits,
                                       DWORD dwIndex,
                                       long lMeasure,
                                       long lBeat,
                                       REFERENCE_TIME *prtRefTime) PURE;
    STDMETHOD(RefTimeToClocks)  (THIS_ REFERENCE_TIME rtRefTime,
                                       long *plTime) PURE;
    STDMETHOD(RefTimeToMeasureBeat)  (THIS_ DWORD dwGroupBits,
                                       DWORD dwIndex,
                                       REFERENCE_TIME rtRefTime,
                                       long *plMeasure,
                                       long *plBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdBandEdit */
#undef  INTERFACE
#define INTERFACE  IDMUSProdBandEdit
DECLARE_INTERFACE_(IDMUSProdBandEdit, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdBandEdit */
    STDMETHOD(GetDefaultFlag)   (THIS_ BOOL* pfDefaultFlag) PURE;
    STDMETHOD(SetDefaultFlag)   (THIS_ BOOL fDefaultFlag) PURE;
    STDMETHOD(GetDLSRegionName) (THIS_ DWORD dwPChannel,
                                       BYTE bMIDINote,
                                       BSTR* pbstrName) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdBandEdit8 */
#undef  INTERFACE
#define INTERFACE  IDMUSProdBandEdit8
DECLARE_INTERFACE_(IDMUSProdBandEdit8, IDMUSProdBandEdit)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdBandEdit */
    STDMETHOD(GetDefaultFlag)   (THIS_ BOOL* pfDefaultFlag) PURE;
    STDMETHOD(SetDefaultFlag)   (THIS_ BOOL fDefaultFlag) PURE;
    STDMETHOD(GetDLSRegionName) (THIS_ DWORD dwPChannel,
                                       BYTE bMIDINote,
                                       BSTR* pbstrName) PURE;

    /* IDMUSProdBandEdit8 */
    STDMETHOD(GetPatchForPChannel)(THIS_ DWORD dwPChannel,
                                       DWORD *pdwPatch) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdBandEdit8a */
#undef  INTERFACE
#define INTERFACE  IDMUSProdBandEdit8a
DECLARE_INTERFACE_(IDMUSProdBandEdit8a, IDMUSProdBandEdit8)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdBandEdit */
    STDMETHOD(GetDefaultFlag)   (THIS_ BOOL* pfDefaultFlag) PURE;
    STDMETHOD(SetDefaultFlag)   (THIS_ BOOL fDefaultFlag) PURE;
    STDMETHOD(GetDLSRegionName) (THIS_ DWORD dwPChannel,
                                       BYTE bMIDINote,
                                       BSTR* pbstrName) PURE;

    /* IDMUSProdBandEdit8 */
    STDMETHOD(GetPatchForPChannel)(THIS_ DWORD dwPChannel,
                                       DWORD *pdwPatch) PURE;

    /* IDMUSProdBandEdit8a */
    STDMETHOD(GetInstNameForPChannel)(THIS_ DWORD dwPChannel,
                                       BSTR* pbstrName) PURE;
    STDMETHOD(DisplayInstrumentButton)(THIS_ DWORD dwPChannel,
									   LONG lXPos,
									   LONG lYPos) PURE;
	STDMETHOD(InsertPChannel)	(THIS_ DWORD dwPChannel,
									   BOOL fClearBandFirst) PURE;
	STDMETHOD(SetAudiopath)		(THIS_ IUnknown* punkAudiopath) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdBandMgrEdit */
#undef  INTERFACE
#define INTERFACE  IDMUSProdBandMgrEdit
DECLARE_INTERFACE_(IDMUSProdBandMgrEdit, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdBandMgrEdit */
    STDMETHOD(DisplayEditBandButton)(THIS_ DWORD dwPChannel,
									   LONG lXPos,
									   LONG lYPos) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdBandPChannel */
#undef  INTERFACE
#define INTERFACE  IDMUSProdBandPChannel
DECLARE_INTERFACE_(IDMUSProdBandPChannel, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdBandPChannel */
    STDMETHOD(GetPChannelNumber)(THIS_ int* pnNumber) PURE;
    STDMETHOD(GetVolume)        (THIS_ int* pnVolume) PURE;
    STDMETHOD(GetPan)           (THIS_ int* pnPan) PURE;
    STDMETHOD(SetVolume)        (THIS_ int nVolume) PURE;
    STDMETHOD(SetPan)           (THIS_ int nPan) PURE;
    STDMETHOD(IsSelected)       (THIS_ BOOL* pfSelected) PURE;
    STDMETHOD(SetSelected)      (THIS_ BOOL fSelection) PURE;
    STDMETHOD(IsEnabled)        (THIS_ BOOL* pbEnabled) PURE;
    STDMETHOD(SyncChanges)      (THIS_ BOOL bSendBandUpdate) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDLSQueryInstruments */
#undef  INTERFACE
#define INTERFACE  IDLSQueryInstruments
DECLARE_INTERFACE_(IDLSQueryInstruments, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDLSQueryInstruments */
    STDMETHOD(ResetInstrumentList)(THIS) PURE;
    STDMETHOD_(BOOL,GetNextInstrument)(THIS_ BYTE *pbMSB,
                                       BYTE *pbLSB,
                                       BYTE *pcPC,
                                       BOOL *pfDrums,
                                       signed char *pszName,
                                       DWORD dwMaxLen) PURE;
    STDMETHOD_(BOOL,GetInstrumentName)(THIS_ BYTE bMSB,
                                       BYTE bLSB,
                                       BYTE cPC,
                                       BOOL fDrums,
                                       signed char *pszName,
                                       DWORD dwMaxLen) PURE;
    STDMETHOD(GetObjectDescriptor)(THIS_ DWORD dwDescSize,
                                       void *pObjectDesc) PURE;
    STDMETHOD(GetRegionWaveName)(THIS_ BYTE bMSB,
                                       BYTE bLSB,
                                       BYTE cPC,
                                       BOOL fDrums,
                                       BYTE bMIDINote,
                                       BSTR* pbstrName) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDLSReferenceLoadNotify */
#undef  INTERFACE
#define INTERFACE  IDLSReferenceLoadNotify
DECLARE_INTERFACE_(IDLSReferenceLoadNotify, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDLSReferenceLoadNotify */
    STDMETHOD(MarkLoadStart)    (THIS) PURE;
    STDMETHOD(MarkLoadEnd)      (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdStyleInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdStyleInfo
DECLARE_INTERFACE_(IDMUSProdStyleInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdStyleInfo */
    STDMETHOD(GetActiveBandForStyle)(THIS_ IUnknown** ppIActiveBandNode) PURE;
    STDMETHOD(GetDefaultBand)   (THIS_ IUnknown** ppIDefaultBandNode) PURE;
    STDMETHOD(GetTempo)         (THIS_ double* pTempo ) PURE;
    STDMETHOD(GetTimeSignature) (THIS_ DMUSProdTimeSignature* pTimeSignature) PURE;
    STDMETHOD(GetNotationType)  (THIS_ DWORD *pdwType) PURE;
    STDMETHOD(GetActiveBandForObject)(THIS_ IUnknown* punkObject,
                                       IUnknown** ppIActiveBandNode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IVarChoices */
#undef  INTERFACE
#define INTERFACE  IVarChoices
DECLARE_INTERFACE_(IVarChoices, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IVarChoices */
    STDMETHOD(SetVarChoicesTitle)(THIS_ BSTR bstrTitle) PURE;
    STDMETHOD(SetDataChangedCallback)(THIS_ IUnknown *punkCallback) PURE;
    STDMETHOD(GetUndoText)      (THIS_ BSTR *pbstrUndoText) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IAllocVarChoices */
#undef  INTERFACE
#define INTERFACE  IAllocVarChoices
DECLARE_INTERFACE_(IAllocVarChoices, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IAllocVarChoices */
    STDMETHOD(GetVarChoicesNode)(THIS_ IUnknown** ppIVarChoicesNode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdChordMapInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdChordMapInfo
DECLARE_INTERFACE_(IDMUSProdChordMapInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdChordMapInfo */
    STDMETHOD(GetChordMapInfo)  (THIS_ DMUSProdChordMapInfo* pChordMapInfo) PURE;
    STDMETHOD(GetChordMapUIInfo)(THIS_ DMUSProdChordMapUIInfo* pChordMapUIInfo) PURE;
    STDMETHOD(SetChordMapUIInfo)(THIS_ DMUSProdChordMapUIInfo* pChordMapUIInfo) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IChordMapMgr */
#undef  INTERFACE
#define INTERFACE  IChordMapMgr
DECLARE_INTERFACE_(IChordMapMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IChordMapMgr */
    STDMETHOD(CreateChordMapStrip)(THIS_ IDMUSProdStrip **ppStrip) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IBandMgr */
#undef  INTERFACE
#define INTERFACE  IBandMgr
DECLARE_INTERFACE_(IBandMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IBandMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ DWORD dwMeasure,
                                       BYTE bBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// ILyricMgr */
#undef  INTERFACE
#define INTERFACE  ILyricMgr
DECLARE_INTERFACE_(ILyricMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* ILyricMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ long lMeasure,
                                       long lBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IParamMgr */
#undef  INTERFACE
#define INTERFACE  IParamMgr
DECLARE_INTERFACE_(IParamMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IParamMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ long lMeasure,
                                       long lBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IScriptMgr */
#undef  INTERFACE
#define INTERFACE  IScriptMgr
DECLARE_INTERFACE_(IScriptMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IScriptMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ long lMeasure,
                                       long lBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// ISegmentTriggerMgr */
#undef  INTERFACE
#define INTERFACE  ISegmentTriggerMgr
DECLARE_INTERFACE_(ISegmentTriggerMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* ISegmentTriggerMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ long lMeasure,
                                       long lBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IPatternNodePrivate */
#undef  INTERFACE
#define INTERFACE  IPatternNodePrivate
DECLARE_INTERFACE_(IPatternNodePrivate, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IPatternNodePrivate */
    STDMETHOD(DisplayPartLinkDialog)(THIS_ GUID *pGuid,
                                       IStream **ppPartStream) PURE;
    STDMETHOD(DisplayVariationChoicesDlg)(THIS_ REFGUID guidPart,
                                       DWORD dwPChannel) PURE;
    STDMETHOD(SetAuditionVariations)(THIS_ DWORD dwVariations,
                                       REFGUID rguidPart,
                                       DWORD dwPChannel) PURE;
    STDMETHOD(GetNumHardLinkRefs)(THIS_ REFGUID guidPart,
                                       DWORD *pdwReferences) PURE;
    STDMETHOD(CanShowPartLinkDialog)(THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IMIDIMgr */
#undef  INTERFACE
#define INTERFACE  IMIDIMgr
DECLARE_INTERFACE_(IMIDIMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IMIDIMgr */
    STDMETHOD(GetUndoText)      (THIS_ BOOL *pfUpdateDirectMusic,
                                       BSTR *pbstrUndoText) PURE;
    STDMETHOD(OnRecord)         (THIS_ BOOL fEnableRecord) PURE;
    STDMETHOD(MergeVariations)  (THIS_ BOOL fChangeData) PURE;
    STDMETHOD(SetSegmentState)  (THIS_ IUnknown *punkSegmentState) PURE;
    STDMETHOD(CanDeleteTrack)   (THIS) PURE;
    STDMETHOD(DeleteTrack)      (THIS) PURE;
    STDMETHOD(Activate)         (THIS_ BOOL fActive) PURE;
    STDMETHOD(AddNewStrip)      (THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// ISequenceMgr */
#undef  INTERFACE
#define INTERFACE  ISequenceMgr
DECLARE_INTERFACE_(ISequenceMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* ISequenceMgr */
    STDMETHOD(SetPChannel)      (THIS_ DWORD dwPChannel) PURE;
    STDMETHOD(SaveEventsToMIDITrack)(THIS_ IStream *pStream) PURE;
    STDMETHOD(PasteEventsFromMIDITrack)(THIS_ IStream *pStream,
                                       short nPPQN,
                                       DWORD dwLength) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IStyleRefMgr */
#undef  INTERFACE
#define INTERFACE  IStyleRefMgr
DECLARE_INTERFACE_(IStyleRefMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IStyleRefMgr */
    STDMETHOD(IsMeasureOpen)    (THIS_ DWORD dwMeasure) PURE;
};

/*////////////////////////////////////////////////////////////////////
// ITempoMgr */
#undef  INTERFACE
#define INTERFACE  ITempoMgr
DECLARE_INTERFACE_(ITempoMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* ITempoMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ DWORD dwMeasure,
                                       BYTE bBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IPersRefMgr */
#undef  INTERFACE
#define INTERFACE  IPersRefMgr
DECLARE_INTERFACE_(IPersRefMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IPersRefMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ DWORD dwMeasure,
                                       BYTE bBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// ITimeSigMgr */
#undef  INTERFACE
#define INTERFACE  ITimeSigMgr
DECLARE_INTERFACE_(ITimeSigMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* ITimeSigMgr */
    STDMETHOD(IsMeasureOpen)    (THIS_ DWORD dwMeasure) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IWaveMgr */
#undef  INTERFACE
#define INTERFACE  IWaveMgr
DECLARE_INTERFACE_(IWaveMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IWaveMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ long lMeasure,
                                       long lBeat) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdContainerInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdContainerInfo
DECLARE_INTERFACE_(IDMUSProdContainerInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdContainerInfo */
    STDMETHOD(FindDocRootFromName)(THIS_ BSTR bstrName,
									   IUnknown** ppIDocRootNode) PURE;
    STDMETHOD(FindDocRootFromScriptAlias)(THIS_ BSTR bstrAlias,
									   IUnknown** ppIDocRootNode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdDebugScript */
#undef  INTERFACE
#define INTERFACE  IDMUSProdDebugScript
DECLARE_INTERFACE_(IDMUSProdDebugScript, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdDebugScript */
    STDMETHOD(DisplayScriptError)(THIS_ void* pErrorInfo) PURE;
    STDMETHOD(DisplayText)		(THIS_ WCHAR* pwszText) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdToolGraphInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdToolGraphInfo
DECLARE_INTERFACE_(IDMUSProdToolGraphInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdToolGraphInfo */
    STDMETHOD(EnumTools)		(THIS_ DWORD dwIndex,
									   IUnknown** ppIDirectMusicTool) PURE;
    STDMETHOD(GetToolInfo)		(THIS_ IUnknown* pIDirectMusicTool,
									   DMUSProdToolInfo* pToolInfo) PURE;
    STDMETHOD(GetToolCount)		(THIS_ DWORD* pdwNbrTools) PURE;
    STDMETHOD(AddToGraphUserList)(THIS_ IUnknown* pIUnknown) PURE;
    STDMETHOD(RemoveFromGraphUserList)(THIS_ IUnknown* pIUnknown) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IMelGenMgr */
#undef  INTERFACE
#define INTERFACE  IMelGenMgr
DECLARE_INTERFACE_(IMelGenMgr, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IMelGenMgr */
    STDMETHOD(IsMeasureBeatOpen)(THIS_ DWORD dwMeasure,
                                       BYTE bBeat) PURE;
	STDMETHOD(EnumMelGens)		(THIS_ DWORD dwIndex,
									   void** ppMelGen) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPrivateTimelineCtl */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPrivateTimelineCtl
DECLARE_INTERFACE_(IDMUSProdPrivateTimelineCtl, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdPrivateTimelineCtl */
    STDMETHOD(SetTimelineOleCtl)(THIS_ IUnknown *punkTimelineOleCtl ) PURE;
    STDMETHOD(FinalCleanUp)		(THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdGetReferencedNodes */
#undef  INTERFACE
#define INTERFACE  IDMUSProdGetReferencedNodes
DECLARE_INTERFACE_(IDMUSProdGetReferencedNodes, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* IDMUSProdGetReferencedNodes */
    STDMETHOD(GetReferencedNodes)(THIS_ DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode, DWORD dwErrorLength, WCHAR *wcstrErrorText ) PURE;
};

/*////////////////////////////////////////////////////////////////////
// ISegmentDocTypeWaveVarImport */
#undef  INTERFACE
#define INTERFACE  ISegmentDocTypeWaveVarImport
DECLARE_INTERFACE_(ISegmentDocTypeWaveVarImport, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)   (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG,Release)   (THIS) PURE;

    /* ISegmentDocTypeWaveVarImport */
	STDMETHOD(ImportWaveNodes)	(THIS_ DWORD dwNodeCount, IDMUSProdNode** apunkNode, IUnknown* punkTreePositionNode, IUnknown** ppIDocRootNode) PURE;
};

/* LIBIDs */

DEFINE_GUID(LIBID_CONDUCTORLib, 0x36F6DDE2,0x46CE,0x11D0, 0xB9,0xDB,0x00,0xAA,0x00,0xC0,0x81,0x46);
DEFINE_GUID(LIBID_CHORDSTRIPMGRLib, 0x1b397d8b,0xbb36,0x11d0,0xbb,0xd3,0x00,0xa0,0xc9,0x22,0xe6,0xeb);
DEFINE_GUID(LIBID_TIMELINELib, 0x934F7270,0xB521,0x11D0,0xA9,0x80,0x00,0xA0,0xC9,0x22,0xE6,0xEB);
DEFINE_GUID(LIBID_BANDEDITORLib,0xD824B383,0x804F,0x11D0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(LIBID_DLSDESIGNERLib,0xBC964E83,0x96F7,0x11D0,0x89,0xAA,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(LIBID_STYLEDESIGNERLib,0x3BD2BA02,0x46E7,0x11D0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(LIBID_CHORDMAPDESIGNERLib,0xD433F95B,0xB588,0x11D0,0x9E,0xDC,0x00,0xAA,0x00,0xA2,0x1B,0xA9);
DEFINE_GUID(LIBID_ChordMapSTRIPMGRLib, 0x8EAEE661,0xEBD6,0x11d0,0x9E,0xDC,0x00,0xAA,0x00,0xA2,0x1B,0xA9);
DEFINE_GUID(LIBID_BANDSTRIPMGRLib, 0xC4B6CAFE,0xEE91,0x11d1, 0x97,0x0D,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(LIBID_COMMANDSTRIPMGRLib, 0x6E6AD92F,0xCD54,0x11D0,0xBB,0xE9,0x00,0xA0,0xC9,0x22,0xE6,0xEB);
DEFINE_GUID(LIBID_LYRICSTRIPMGRLib, 0xEE279465,0xB2E5,0x11d1,0x88,0x8F,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(LIBID_MARKERSTRIPMGRLib, 0xBC50AF9D,0x3010,0x4AF2,0xA7,0xC8,0x93,0x7A,0x4E,0x4C,0x03,0x1E);
DEFINE_GUID(LIBID_MIDISTRIPMGRLib, 0x30DF80FD,0xD7A0,0x11D0,0xBB,0xF3,0x00,0xA0,0xC9,0x22,0xE6,0xEB);
DEFINE_GUID(LIBID_SequenceSTRIPMGRLib, 0xEAAC67C3,0x27EF,0x11d2,0xBC,0x59,0x00,0xC0,0x4F,0xA3,0x72,0x6E);
DEFINE_GUID(LIBID_MUTESTRIPMGRLib, 0xA18560A3,0x1724,0x11D2,0x85,0x0D,0x00,0xA0,0xC9,0x9F,0x7E,0x74);
DEFINE_GUID(LIBID_PERSREFSTRIPMGRLib, 0xE788F2E9,0xDBCA,0x11d1,0x96,0xE4,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(LIBID_SIGNPOSTSTRIPMGRLib, 0x26909971,0xE157,0x11d0,0xBA,0xCA,0x00,0x80,0x5F,0x49,0x3F,0x43);
DEFINE_GUID(LIBID_STYLEREFSTRIPMGRLib, 0x066891BF,0xD658,0x11d1,0x96,0xD5,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(LIBID_TEMPOSTRIPMGRLib, 0xC6ED2EA4,0xF1D3,0x11d1,0x88,0xCB,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(LIBID_TIMESIGSTRIPMGRLib, 0xB14B2C82,0xABDA,0x11d2,0xB0,0xD9,0x00,0x10,0x5A,0x26,0x62,0x0B);
DEFINE_GUID(LIBID_PARAMSTRIPMGRLib,0xA1467A27,0x25E2,0x41b7,0xB1,0x91,0x94,0x62,0xBC,0xD9,0x7E,0xB5);
DEFINE_GUID(LIBID_SCRIPTSTRIPMGRLib,0x85DFAE15,0x6BC9,0x11D3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(LIBID_SEGMENTSTRIPMGRLib,0xA05F7B90,0x76B5,0x11d3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(LIBID_WAVESTRIPMGRLib,0x291F01E1,0x8E58,0x412f,0x8B,0x55,0x96,0x40,0x12,0x75,0x97,0xF4);
DEFINE_GUID(LIBID_CONTAINERDESIGNERLib,0x778A0B8C,0x6F81,0x11D3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(LIBID_SCRIPTDESIGNERLib,0xD135DB55,0x66ED,0x11D3,0xB4,0x5D,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(LIBID_TOOLGRAPHDESIGNERLib,0xB39B2935,0x8E62,0x4CEB,0xAE,0xF6,0x29,0x42,0x86,0xA0,0x85,0x18);
DEFINE_GUID(LIBID_MELGENSTRIPMGRLib,0x2E1E2E76,0xB7B7,0x11d2,0x97,0xFE,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(LIBID_SONGDESIGNERLib,0x41606C30,0x9721,0x43EA,0xA9,0x2C,0x21,0x96,0xE0,0x1E,0x2E,0xE8);


/* CLSIDs */

DEFINE_GUID(CLSID_AudioPathEditor,0xC466D59C,0xCD47,0x4b38,0x99,0x64,0x4D,0xE3,0x73,0xAF,0x48,0x30);
DEFINE_GUID(CLSID_UnknownStripMgr, 0x853baf7b, 0xd3c8, 0x11d1, 0x88, 0xbc, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15);
DEFINE_GUID(CLSID_ChordMgr, 0x1b397d99,0xbb36,0x11d0,0xbb,0xd3,0x00,0xa0,0xc9,0x22,0xe6,0xeb);
DEFINE_GUID(CLSID_ChordStrip, 0x342d41a1, 0xcbd8, 0x11d0, 0xbb, 0xe5, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);
DEFINE_GUID(CLSID_TempoMapMgr,0xF4D10CAE,0x2897,0x11D1,0x88,0x36,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(CLSID_TimeStripMgr, 0x884F3F04,0xBFE0,0x11D0,0xBB,0xDB,0x00,0xA0,0xC9,0x22,0xE6,0xEB);
DEFINE_GUID(CLSID_BandComponent, 0x44207724,0x487B,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_BandEditor,0x3BD2BA11,0x46E7,0x11D0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_Collection,0xBC964E86,0x96F7,0x11D0,0x89,0xAA,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_Instrument,0xBC964E8A,0x96F7,0x11D0,0x89,0xAA,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_Wave,0xBC964E8E,0x96F7,0x11D0,0x89,0xAA,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_Articulation,0xBC964E92,0x96F7,0x11D0,0x89,0xAA,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_Region,0xBC964E96,0x96F7,0x11D0,0x89,0xAA,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_DLSComponent,0x7B5F1BE1,0x96FC,0x11d0,0x89,0xAA,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_StyleComponent, 0x44207721,0x487B,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_StyleEditor,0x3BD2BA05,0x46E7,0x11D0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_PatternEditor,0x3BD2BA09,0x46E7,0x11D0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_VarChoicesEditor,0x3BD2BA15,0x46E7,0x11D0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(CLSID_PersonalityComponent,0x6D432E20,0xB5E2,0x11d0,0x9E,0xDC,0x00,0xAA,0x00,0xA2,0x1B,0xA9);
DEFINE_GUID(CLSID_PersonalityEditor,0xD433F95E,0xB588,0x11D0,0x9E,0xDC,0x00,0xAA,0x00,0xA2,0x1B,0xA9);
DEFINE_GUID(CLSID_ChordMapStrip, 0x2de8bae0, 0xebd6, 0x11d0, 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9);
DEFINE_GUID(CLSID_ChordMapMgr, 0x9B0E9FE1,0xEBD6,0x11d0,0x9E,0xDC,0x00,0xAA,0x00,0xA2,0x1B,0xA9);
DEFINE_GUID(CLSID_BandMgr, 0xC4B6CAFF,0xEE91,0x11d1, 0x97,0x0D,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(CLSID_CommandMgr, 0x6E6AD93D,0xCD54,0x11D0,0xBB,0xE9,0x00,0xA0,0xC9,0x22,0xE6,0xEB);
DEFINE_GUID(CLSID_LyricMgr, 0xEE279466,0xB2E5,0x11d1,0x88,0x8F,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(CLSID_MarkerMgr, 0xAD273CD3,0x75E3,0x4F34,0xAC,0x0E,0xF8,0xB8,0xFC,0x12,0x15,0x77);
DEFINE_GUID(CLSID_MIDIMgr, 0x30DF810B,0xD7A0,0x11D0,0xBB,0xF3,0x00,0xA0,0xC9,0x22,0xE6,0xEB);
DEFINE_GUID(CLSID_SequenceMgr, 0xEAAC67C1,0x27EF,0x11d2,0xBC,0x59,0x00,0xC0,0x4F,0xA3,0x72,0x6E);
DEFINE_GUID(CLSID_MuteMgr, 0xA3504AE2,0x174B,0x11D2,0x85,0x0D,0x00,0xA0,0xC9,0x9F,0x7E,0x74);
DEFINE_GUID(CLSID_PersRefMgr, 0xE788F2EA,0xDBCA,0x11d1,0x96,0xE4,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(CLSID_SignPostMgr, 0xD86B06B1,0xE156,0x11d0,0xBA,0xCA,0x00,0x80,0x5F,0x49,0x3F,0x43);
DEFINE_GUID(CLSID_StyleRefMgr, 0x066891C0,0xD658,0x11d1,0x96,0xD5,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(CLSID_TempoMgr, 0xC6ED2EA5,0xF1D3,0x11d1,0x88,0xCB,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(CLSID_TimeSigMgr, 0x8C6005D2,0xABDA,0x11d2,0xB0,0xD9,0x00,0x10,0x5A,0x26,0x62,0x0B);
DEFINE_GUID(CLSID_ParamMgr,0xA1467A29,0x25E2,0x41b7,0xB1,0x91,0x94,0x62,0xBC,0xD9,0x7E,0xB5);
DEFINE_GUID(CLSID_ScriptMgr,0x85DFAE18,0x6BC9,0x11D3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(CLSID_SegmentTriggerMgr,0xA05F7B92,0x76B5,0x11d3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(CLSID_TimeStrip, 0x273970bd, 0xc01c, 0x11d0, 0xbb, 0xdb, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);
DEFINE_GUID(CLSID_WaveMgr,0x291F01E3,0x8E58,0x412f,0x8B,0x55,0x96,0x40,0x12,0x75,0x97,0xF4);
DEFINE_GUID(CLSID_CommandStrip, 0x3b4fa819, 0xcd60, 0x11d0, 0xbb, 0xe9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);
DEFINE_GUID(CLSID_PianoRollStrip, 0x6cfe6bd3, 0xd7a2, 0x11d0, 0xbb, 0xf3, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xeb);
DEFINE_GUID(CLSID_CurveStrip, 0x4e1c5f20, 0xd441, 0x11d1, 0x89, 0xb1, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(CLSID_VarSwitchStrip, 0x61c32015, 0xfcfc, 0x4acc, 0x9b, 0xf0, 0xb3, 0x8b, 0xcf, 0x5f, 0xfd, 0x7b);
DEFINE_GUID(CLSID_SignPostStrip, 0x1b74a370, 0xe11b, 0x11d0, 0xba, 0xca, 0x0, 0x80, 0x5f, 0x49, 0x3f, 0x43);
DEFINE_GUID(CLSID_Container,0x778A0B8F,0x6F81,0x11D3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(CLSID_ContainerComponent, 0x1ca84b10,0x7d17,0x11d3,0xb4,0x72,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(CLSID_ScriptComponent, 0xbec19c60,0x66fd,0x11d3,0xb4,0x5d,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(CLSID_ScriptEditor,0xD135DB58,0x66ED,0x11D3,0xB4,0x5D,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(CLSID_ToolGraphComponent, 0xeab971ee,0x6601,0x4f70,0x94,0x34,0x32,0xce,0x56,0x8a,0xe3,0xf3);
DEFINE_GUID(CLSID_GraphEditor,0x7A6D839D,0xE531,0x4bbd,0xB3,0xBC,0xDD,0x16,0xF5,0xD3,0x5B,0x43);
DEFINE_GUID(CLSID_MelGenMgr,0xDE9B8A54,0xB7B5,0x11d2,0x97,0xFE,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(CLSID_SongComponent, 0x58e70af0,0x8bba,0x413c,0xbc,0xcb,0x6,0x30,0xc8,0x2f,0xf2,0x91);
DEFINE_GUID(CLSID_SongEditor,0x35327DAF,0x7151,0x43da,0xAF,0x9B,0x57,0x9D,0x8A,0x20,0x41,0x03);
DEFINE_GUID(CLSID_PrivateTimelineCtl, 0xa7bb9b47, 0xa940, 0x479f, 0xa7, 0xe8, 0x36, 0x3e, 0xf7, 0x4f, 0xc5, 0x7b);

/* GUIDs */

DEFINE_GUID(GUID_Sequence_QuantizeParams, 0x1be99c30, 0x24f7, 0x11d3, 0xb4, 0x4a, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(GUID_Sequence_Quantize, 0x8c334ce0, 0xf1fb, 0x11d2, 0xb4, 0x3f, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde);
DEFINE_GUID(GUID_Sequence_VelocitizeParams, 0x92e3faae, 0x6dfa, 0x41c8, 0xa4, 0x83, 0xb8, 0xcc, 0x25, 0xe3, 0x7, 0xd1);
DEFINE_GUID(GUID_Sequence_Velocitize, 0x8797b040, 0xab8b, 0x46f1, 0xa8, 0xfe, 0x1f, 0x8, 0x10, 0x8f, 0x13, 0x65);
DEFINE_GUID(GUID_LegacyTemplateActivityLevel, 0x7467fba7, 0xb7ea, 0x49ed, 0x81, 0x6f, 0x5a, 0x98, 0x17, 0x7b, 0xfc, 0xd9);
DEFINE_GUID(GUID_ChordTrackGroupBits, 0xdc964780, 0x5c8, 0x11d2, 0x89, 0xb3, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(GUID_UseGroupBitsPPG, 0xa9bb4c80, 0xb71, 0x11d2, 0x89, 0xb3, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(GUID_PatternEditorMode, 0xfa5a6dc0, 0x32b7, 0x11d2, 0x89, 0xb4, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(GUID_NeedChordMeasure1Beat1, 0x5cf7d20, 0xb7c, 0x11d2, 0x89, 0xb3, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(GUID_BandNode, 0x52d58461,0xa517,0x11d0,0x89,0xad,0x0,0xa0,0xc9,0x5,0x41,0x29);
DEFINE_GUID(GUID_BandRefNode, 0xb33aedb0,0x81af,0x11d3,0xb4,0x73,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(BAND_NameChange, 0x11c4f3c0,0x8f54,0x11d1,0x8a,0xc1,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(BAND_ValidateNameChange, 0x5b67e200,0x5784,0x11d2,0x89,0xb7,0x0,0xc0,0x4f,0xd9,0x12,0xc8);
DEFINE_GUID(GUID_BAND_ChangeNotifyMsg, 0x4cf096e0, 0xe056, 0x11d1, 0xb9, 0x87, 0x0, 0x60, 0x97, 0xb0, 0x10, 0x78);
DEFINE_GUID(GUID_BAND_ActivateNotifyMsg, 0x60833ec0, 0xf55e, 0x11d1, 0x89, 0xb2, 0x0, 0xc0, 0x4f, 0xd9, 0x12, 0xc8);
DEFINE_GUID(GUID_BAND_UpdatePerformanceMsg, 0x4cf096e1, 0xe056, 0x11d1, 0xb9, 0x87, 0x0, 0x60, 0x97, 0xb0, 0x10, 0x78);
DEFINE_GUID(GUID_CollectionRefNode, 0x74ad7060, 0xbd72, 0x11d1, 0xb9, 0x87, 0x0, 0x60, 0x97, 0xb0, 0x10, 0x78);
DEFINE_GUID(GUID_CollectionNode,    0xb1c20e00, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_CollectionGroupNode,0xb1c20e01, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_InstrumentFolderNode,0xb1c20e02, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_InstrumentNode,0xb1c20e03, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_RegionFolderNode,0xb1c20e04, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_RegionNode,0xb1c20e05, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_ArticulationFolderNode, 0xab42108f, 0x9f76, 0x47bf, 0xaa, 0xae, 0xfe, 0x30, 0xf8, 0x7, 0xb8, 0x52);
DEFINE_GUID(GUID_ArticulationNode,0xb1c20e06, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_WaveFolderNode,0xb1c20e07, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_WaveNode,0xb1c20e08, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_WaveRefNode,0xb1c20e0a, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(GUID_WaveGroupNode,0xb1c20e09, 0xa871, 0x11d0, 0x89, 0xaa, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29);
DEFINE_GUID(INSTRUMENT_NameChange,0xB6B35EF0,0xE6DD,0x11D2,0x9A,0xD4,0x00,0x60,0x97,0xB0,0x10,0x78);
DEFINE_GUID(INSTRUMENT_Deleted, 0x88d0da7d, 0xb08b, 0x4de8, 0xae, 0x39, 0x1, 0xc3, 0xd1, 0x8e, 0xa, 0x93);
DEFINE_GUID(COLLECTION_NameChange,0x461f5d8e, 0x5931, 0x4b9d, 0x9e, 0x9e, 0xd8, 0xb9, 0x22, 0x2f, 0xc8, 0x9f);
DEFINE_GUID(WAVENODE_NameChange, 0x313b7aae, 0xabe9, 0x4bed, 0x89, 0x8d, 0xaf, 0x30, 0x7, 0x2d, 0xdb, 0x46);
DEFINE_GUID(WAVENODE_DataChange, 0x6f044658, 0x3097, 0x431a, 0xab, 0x9a, 0x3b, 0x2c, 0xfa, 0x67, 0xa, 0x79);
DEFINE_GUID(GUID_DMCollectionResync, 0xa1d7feba, 0x37ee, 0x4707, 0x92, 0x5a, 0xf9, 0xbe, 0xb8, 0x61, 0x0, 0x46);
DEFINE_GUID(GUID_DownloadOnLoadRIFFChunk, 0xc18feb16, 0xf2b6, 0x4ba4, 0xbb, 0xa1, 0xc6, 0x56, 0xaf, 0xcc, 0xdc, 0x2);
DEFINE_GUID(GUID_SinglePattern, 0x10521900,0x4549,0x11d1,0x89,0xae,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(GUID_MotifNode, 0xFAE21E41,0xA51A,0x11D0,0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_PatternNode, 0xFAE21E43,0xA51A,0x11D0,0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_StyleNode, 0xFAE21E45,0xA51A,0x11D0,0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_VarChoicesNode, 0x9409E740,0x86B3,0x11D1,0x89,0xAF,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_StyleBandFolderNode, 0xFAE21E47,0xA51A,0x11D0,0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_StyleMotifFolderNode, 0xFAE21E48,0xA51A,0x11D0,0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_StylePatternFolderNode, 0xFAE21E49,0xA51A,0x11D0,0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_StyleRefNode, 0x408FBB21,0xB009,0x11D0,0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(GUID_ChordStripChanged, 0xb2597270,0x1a0a,0x11d3,0xb4,0x47,0x0,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(STYLE_NameChange, 0x666ce421,0x3034,0x11d1,0x89,0xae,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(STYLE_TimeSigChange, 0x666ce422,0x3034,0x11d1,0x89,0xae,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(STYLE_TempoChange, 0x666ce423,0x3034,0x11d1,0x89,0xae,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(STYLE_NotationTypeChange, 0x5f6e7492,0xed3f,0x11d2,0xa6,0xe6,0x0,0x10,0x5a,0x26,0x62,0xb);
DEFINE_GUID(STYLE_PatternWindowClose, 0x8e8c33a2,0xf774,0x11d2,0xa6,0xeb,0x0,0x10,0x5a,0x26,0x62,0xb);
DEFINE_GUID(STYLE_PChannelChange, 0x593f7830,0x3b14,0x11d3,0xb4,0x4e,0x0,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(STYLE_MotifNameChange, 0x6a87ab75,0xd696,0x4bbd,0x92,0x15,0xf9,0x54,0x83,0x94,0x8a,0x56);
DEFINE_GUID(STYLE_MotifChanged, 0x6a87ab76,0xd696,0x4bbd,0x92,0x15,0xf9,0x54,0x83,0x94,0x8a,0x56);
DEFINE_GUID(STYLE_MotifDeleted, 0x6a87ab77,0xd696,0x4bbd,0x92,0x15,0xf9,0x54,0x83,0x94,0x8a,0x56);
DEFINE_GUID(GUID_PersonalityNode, 0xf6797820, 0xb5dd, 0x11d0, 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9);
DEFINE_GUID(GUID_PersonalityRefNode, 0x8ca7ab01, 0xc803, 0x11d0, 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9);
DEFINE_GUID(GUID_PersonalityGroupFolderNode, 0xf6797821, 0xb5dd, 0x11d0, 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9);
DEFINE_GUID(GUID_PersonalityChordListFolderNode, 0x9c165020, 0xc18a, 0x11d0, 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9);
DEFINE_GUID(GUID_PersonalitySignPostFolderNode, 0xfc5abbe1, 0xc263, 0x11d0, 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9);
DEFINE_GUID(GUID_PersonalityChordPaletteFolderNode, 0x813a1161, 0xc26b, 0x11d0, 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9);
DEFINE_GUID(PERSONALITY_NameChange, 0x68ce1323, 0x912f, 0x11d1, 0x98, 0x4d, 0x0, 0x80, 0x5f, 0xa6, 0x7d, 0x16);
DEFINE_GUID(GUID_ChordMapZoom, 0xa78c01f3, 0x1886, 0x11d3, 0xbc, 0xb7, 0x0, 0xc0, 0x4f, 0xa3, 0x72, 0x6e);
DEFINE_GUID(GUID_ChordMapStripUndoText, 0xb5101ba5, 0xde0f, 0x11d1, 0x98, 0x88, 0x0, 0x80, 0x5f, 0xa6, 0x7d, 0x16);
DEFINE_GUID(GUID_SelectedConnection, 0xfa232d4f, 0xe157, 0x11d1, 0x98, 0x88, 0x0, 0x80, 0x5f, 0xa6, 0x7d, 0x16);
DEFINE_GUID(GUID_SelectedObjectType, 0x5679b6a9, 0xe13a, 0x11d1, 0x98, 0x88, 0x0, 0x80, 0x5f, 0xa6, 0x7d, 0x16);
DEFINE_GUID(GUID_SelectedPaletteIndex, 0x14096fa7, 0xe3c0, 0x11d1, 0x98, 0x89, 0x0, 0x80, 0x5f, 0xa6, 0x7d, 0x16);
DEFINE_GUID(GUID_VariableNotFixed, 0x58c38d47, 0x3174, 0x11d2, 0xbc, 0x5e, 0x0, 0xc0, 0x4f, 0xa3, 0x72, 0x6e);
DEFINE_GUID(GUID_BandStrip_InstrumentItem, 0x90ca54f5, 0x6c05, 0x4552, 0xae, 0x23, 0xee, 0x15, 0x4f, 0x4c, 0xc2, 0x21);
DEFINE_GUID(GUID_ContainerNode, 0x1ca84b11,0x7d17,0x11d3,0xb4,0x72,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(GUID_ContainerRefNode, 0x1ca84b12,0x7d17,0x11d3,0xb4,0x72,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(GUID_ContainerEmbedFolderNode, 0x1ca84b13,0x7d17,0x11d3,0xb4,0x72,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(GUID_ContainerRefFolderNode, 0x1ca84b14,0x7d17,0x11d3,0xb4,0x72,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(GUID_ContainerObjectNode, 0x1ca84b15,0x7d17,0x11d3,0xb4,0x72,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(CONTAINER_NameChange, 0x1ca84b16,0x7d17,0x11d3,0xb4,0x72,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(CONTAINER_ChangeNotification, 0xfde73220,0x7c26,0x45b0,0xa5,0x83,0xd,0x19,0xe6,0x98,0x52,0xf1);
DEFINE_GUID(CONTAINER_FileLoadFinished, 0x4e81c756,0x3228,0x4964,0x8f,0x40,0xfa,0x79,0x84,0x66,0x24,0xd6);
DEFINE_GUID(GUID_ScriptNode, 0xbec19c61,0x66fd,0x11d3,0xb4,0x5d,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(GUID_ScriptRefNode, 0xbec19c62,0x66fd,0x11d3,0xb4,0x5d,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(SCRIPT_NameChange, 0xbec19c63,0x66fd,0x11d3,0xb4,0x5d,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(GUID_ToolGraphNode, 0x5a5a8e5,0xde1b,0x4bad,0xb2,0x40,0xf5,0xa9,0xf2,0x11,0x7,0x86);
DEFINE_GUID(GUID_ToolGraphRefNode, 0x5f28e0c4,0xde1c,0x492a,0x93,0x2,0x75,0x38,0xba,0xc2,0xfb,0xc2);
DEFINE_GUID(TOOLGRAPH_NameChange, 0xf1007af8,0xb98f,0x4a4a,0x88,0xcf,0x7b,0x38,0x8f,0xdc,0x66,0xcf);
DEFINE_GUID(TOOLGRAPH_ChangeNotification, 0x6681d08e,0xfde0,0x4006,0xb6,0xbd,0x9c,0x1f,0x5f,0x16,0x1f,0xb3);
DEFINE_GUID(GUID_SongNode, 0x3e3127ae,0xf57,0x4e9e,0xae,0xeb,0xba,0x32,0x66,0x6e,0xd1,0xca);
DEFINE_GUID(GUID_SongRefNode, 0x44dcfc1a,0x1b32,0x419f,0x9f,0x5f,0xab,0x69,0xd5,0xd3,0x3,0x8a);
DEFINE_GUID(GUID_Song_SegmentsFolderNode, 0xba1c61d0,0xf4f,0x4a6d,0x92,0x1e,0x4,0x21,0x60,0x8e,0x2f,0x69);
DEFINE_GUID(GUID_Song_SegmentEmbedFolderNode, 0x53652748,0x802b,0x4ac0,0x90,0x27,0x48,0x75,0xd,0x6e,0x51,0x73);
DEFINE_GUID(GUID_Song_SegmentRefFolderNode, 0x1d091e2d,0x486b,0x47ee,0x9c,0xb8,0x1e,0x26,0xb0,0xac,0xe3,0x9d);
DEFINE_GUID(GUID_Song_ToolGraphsFolderNode, 0x80e69a,0x89f4,0x4190,0xab,0xb1,0xe7,0x9f,0x30,0x18,0xa6,0xc9);
DEFINE_GUID(GUID_Song_SourceSegmentNode, 0x1dec7743,0xe16a,0x4fc1,0xbc,0xae,0xd1,0x20,0x7e,0xd,0x8c,0xca);
DEFINE_GUID(SONG_NameChange, 0xea1cdfad,0x265a,0x4b79,0x93,0xc9,0x61,0x14,0x29,0xd,0x1,0x72);
DEFINE_GUID(SONG_FileLoadFinished, 0x87f138e8,0x446c,0x4518,0x87,0x3f,0x53,0x7e,0x66,0x11,0x71,0x76);
DEFINE_GUID(GUID_PatternAuditionSegment, 0x58ee63d4, 0xf196, 0x4484, 0xa9, 0x6f, 0x78, 0x5, 0x24, 0x79, 0x4d, 0x42);
DEFINE_GUID(GUID_BandMgrEditForPChannel, 0x5ad0abc0, 0xe159, 0x4667, 0xb9, 0x52, 0xa7, 0x14, 0xb3, 0x63, 0xa7, 0x54);
DEFINE_GUID(GUID_Segment_ReferencedNodes, 0x33d80602, 0x9aac, 0x46ee, 0x8b, 0x47, 0x93, 0xc, 0x61, 0x4d, 0xc4, 0x75);


/* IIDs */

DEFINE_GUID(IID_IPrivatePatternTrack, 0x7a8e9c33, 0x5901, 0x4f20, 0x92, 0xde, 0x3a, 0x5b, 0x3e, 0x33, 0xe2, 0x14);
DEFINE_GUID(IID_IPrivatePatternTrack9, 0x7708a009, 0xce26, 0x4758, 0x8f, 0x6e, 0x1d, 0xac, 0xe6, 0xf5, 0x64, 0xdb);
DEFINE_GUID(IID_IDMUSProdUnpackingFiles,0xf998b7a1, 0xccd4, 0x460c, 0xb0, 0x76, 0xdd, 0x73, 0x54, 0x12, 0xf1, 0x8f);
DEFINE_GUID(IID_IPrivateTransport, 0xf6e580c, 0xf736, 0x4dae, 0xa0, 0x57, 0xa1, 0xf5, 0x9c, 0x74, 0x3b, 0xf);
DEFINE_GUID(IID_IDMUSProdWaveTimelineDraw, 0xaca11be, 0x53f, 0x47a4, 0xaf, 0x7c, 0xbc, 0x7e, 0x7, 0xe5, 0xb4, 0x6f);
DEFINE_GUID(IID_IDMUSProdWaveTrackDownload, 0x6974c40f, 0xa535, 0x4db4, 0x94, 0x5b, 0x56, 0xe8, 0x95, 0x8, 0xca, 0xde);
DEFINE_GUID(IID_IDMUSProdConductorPrivate, 0x4e15129b,0x15e0,0x4a02,0x80,0x0d,0xdf,0x88,0x21,0x65,0xbb,0x18);
DEFINE_GUID(IID_IDMUSProdAudioPathInUse,0x6D749DE8,0x0B71,0x4e4c,0xB1,0x12,0x5F,0xEC,0x87,0xD9,0x0E,0xD0);
DEFINE_GUID(IID_IDMUSProdDMOInfo,0xE71DCB73,0xE957,0x4b28,0xB4,0xC9,0x37,0x86,0x6A,0xF4,0x00,0xC1);
DEFINE_GUID(IID_IPrivateSegment, 0xb0615992, 0xe950, 0x44f8, 0xa9, 0x31, 0x84, 0x2b, 0x56, 0xe2, 0xf1, 0xba);
DEFINE_GUID(IID_IChordMgr, 0x1b397d98,0xbb36,0x11d0,0xbb,0xd3,0x00,0xa0,0xc9,0x22,0xe6,0xeb);
DEFINE_GUID(IID_IDMUSProdTempoMapMgr,0xB232A288,0xB2CE,0x11d1,0x88,0x8F,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(IID_IDMUSProdBandEdit,0xF1AE6340,0x8F6D,0x11d1,0x8A,0xC1,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDMUSProdBandEdit8,0xD1454002,0xEA6F,0x4a7f,0xAF,0xBD,0x1B,0x9C,0x39,0x7D,0xC4,0xBE);
DEFINE_GUID(IID_IDMUSProdBandEdit8a, 0x9006e942, 0x52a8, 0x4fa9, 0xab, 0x88, 0x59, 0xd2, 0x42, 0x15, 0x5, 0x6b);
DEFINE_GUID(IID_IDMUSProdBandPChannel,0x042c70f0,0xe23c,0x11d2,0x9a,0xd3,0x00,0x60,0x97,0xb0,0x10,0x78);
DEFINE_GUID(IID_IDLSQueryInstruments,0x8196AE82,0x692C,0x11d2,0xB0,0x67,0x00,0x10,0x5A,0x26,0x62,0x0B);
DEFINE_GUID(IID_IDLSReferenceLoadNotify,0xb76650e0,0x0282,0x11d3,0x9a,0xdf,0x00,0x60,0x97,0xb0,0x10,0x78);
DEFINE_GUID(IID_IDMUSProdStyleInfo,0xFC715592,0xED4A,0x11d2,0xA6,0xE6,0x00,0x10,0x5A,0x26,0x62,0x0B);
DEFINE_GUID(IID_IVarChoices,0x5B099CBF,0x0552,0x4944,0xB0,0x5A,0xEB,0x67,0x75,0x09,0xC6,0xC3);
DEFINE_GUID(IID_IAllocVarChoices,0x20415BC0,0xE0ED,0x41de,0xA5,0x51,0xFB,0xE5,0x0C,0xF6,0x31,0xAE);
DEFINE_GUID(IID_IDMUSProdChordMapInfo,0xD5C283A9,0xBE29,0x4d45,0x8A,0x6C,0xDD,0x84,0x39,0x59,0xA8,0x50);
DEFINE_GUID(IID_ChordChangeCallback, 0x8ccd37af, 0xde22, 0x11d1, 0x98, 0x88, 0x0, 0x80, 0x5f, 0xa6, 0x7d, 0x16);
DEFINE_GUID(IID_ZoomChangeNotification, 0x4e52d805, 0x193f, 0x11d3, 0xbc, 0xb7, 0x0, 0xc0, 0x4f, 0xa3, 0x72, 0x6e);
DEFINE_GUID(IID_PreEditNotification, 0x32ca0fd5, 0xde22, 0x11d1, 0x98, 0x88, 0x0, 0x80, 0x5f, 0xa6, 0x7d, 0x16);
DEFINE_GUID(IID_CheckForOrphansNotification, 0xec42ccd, 0xb0cd, 0x11d2, 0xbc, 0x9f, 0x0, 0xc0, 0x4f, 0xa3, 0x72, 0x6e);
DEFINE_GUID(IID_IChordMapMgr, 0xF0A65452,0x04B1,0x11d3,0x89,0x4C,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(IID_IBandMgr, 0xC4B6CAFD,0xEE91,0x11d1,0x97,0x0D,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(IID_ICommandMgr, 0x6E6AD93C,0xCD54,0x11D0,0xBB,0xE9,0x00,0xA0,0xC9,0x22,0xE6,0xEB);
DEFINE_GUID(IID_ILyricMgr, 0x8D4226A9,0xA2E2,0x4e3f,0xAB,0x7D,0x6E,0x81,0xFB,0xE4,0x9D,0x00);
DEFINE_GUID(IID_IMarkerMgr, 0xC0C83490,0x9BF5,0x467E,0x8F,0x80,0xD0,0x20,0x16,0x34,0x7C,0xC1);
DEFINE_GUID(IID_IPatternNodePrivate, 0x2D8BA47E,0x0969,0x11d3,0x89,0x4C,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(IID_IMIDIMgr, 0xD7B4CEAC,0x4235,0x4665,0xAB,0x3C,0xC2,0x9F,0x39,0x5C,0x8B,0xE7);
DEFINE_GUID(IID_ISequenceMgr, 0x9D6155D3,0x1090,0x4AA1,0x95,0xBA,0x61,0xC4,0x8F,0x43,0x9A,0xE5);
DEFINE_GUID(IID_IMuteMgr, 0xA3504AE1,0x174B,0x11D2,0x85,0x0D,0x00,0xA0,0xC9,0x9F,0x7E,0x74);
DEFINE_GUID(IID_IPersRefMgr, 0xE788F2E8,0xDBCA,0x11d1,0x96,0xE4,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(IID_ISignPostMgr, 0x150BB221,0xE157,0x11d0,0xBA,0xCA,0x00,0x80,0x5F,0x49,0x3F,0x43);
DEFINE_GUID(IID_IStyleRefMgr, 0x066891BE,0xD658,0x11d1,0x96,0xD5,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(IID_ITempoMgr, 0xC6ED2EA3,0xF1D3,0x11d1,0x88,0xCB,0x00,0xC0,0x4F,0xBF,0x8D,0x15);
DEFINE_GUID(IID_ITimeSigMgr, 0x0CBBC5D1,0xABDB,0x11d2,0xB0,0xD9,0x00,0x10,0x5A,0x26,0x62,0x0B);
DEFINE_GUID(IID_IParamMgr,0xA1467A28,0x25E2,0x41b7,0xB1,0x91,0x94,0x62,0xBC,0xD9,0x7E,0xB5);
DEFINE_GUID(IID_IScriptMgr,0x85DFAE17,0x6BC9,0x11D3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(IID_ISegmentTriggerMgr,0xA05F7B91,0x76B5,0x11d3,0xB4,0x5F,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(IID_IWaveMgr,0x291F01E2,0x8E58,0x412f,0x8B,0x55,0x96,0x40,0x12,0x75,0x97,0xF4);
DEFINE_GUID(IID_IDMUSProdContainerInfo,0x3015EC93,0x4B3F,0x4e5f,0xAC,0xC5,0x81,0xF5,0x44,0x6E,0x01,0x24);
DEFINE_GUID(IID_IDMUSProdDebugScript,0x417C3B03,0x94C0,0x43f3,0x9C,0x76,0x2C,0xA2,0xA7,0x9B,0xB2,0x9E);
DEFINE_GUID(IID_IDMUSProdToolGraphInfo,0xEA38879E,0x0A79,0x475d,0x99,0x9F,0xA7,0xFF,0xB3,0x6F,0xF1,0xD5);
DEFINE_GUID(IID_IMelGenMgr,0xC4CE0914,0xB7B5,0x11d2,0x97,0xFE,0x00,0xC0,0x4F,0xA3,0x6E,0x58);
DEFINE_GUID(IID_ICPrivateTimelineCtl, 0xf4fcbbd0, 0xfe2f, 0x41d9, 0x8f, 0xaf, 0x8, 0x98, 0x6c, 0x23, 0xc, 0x10);
DEFINE_GUID(IID_ICTimelineCtl, 0xae782040, 0xdfe5, 0x11d4, 0xbd, 0x32, 0xc1, 0xc, 0x2c, 0xf8, 0xb, 0x43);
DEFINE_GUID(IID_IDMUSProdPrivateTimelineCtl, 0xc681f780, 0xdfe8, 0x11d4, 0xbd, 0x32, 0xc1, 0xc, 0x2c, 0xf8, 0xb, 0x43);
DEFINE_GUID(IID_IDMUSProdBandMgrEdit, 0xa431a582, 0xd654, 0x44cf, 0xa3, 0xbf, 0xa1, 0x52, 0xa2, 0x29, 0x1e, 0xf5);
DEFINE_GUID(IID_IDMUSProdGetReferencedNodes, 0x2323453a, 0xd02f, 0x47a0, 0x8e, 0xd8, 0x3, 0xaa, 0x76, 0x49, 0x6a, 0x6e);
DEFINE_GUID(IID_ISegmentDocTypeWaveVarImport, 0xf5519658, 0x2070, 0x4f45, 0xb1, 0x59, 0x1c, 0x3c, 0x42, 0x32, 0x2, 0x68);

#ifdef __cplusplus
struct __declspec(uuid("{0F6E580C-F736-4dae-A057-A1F59C743B0F}")) IPrivateTransport;
struct __declspec(uuid("{4E15129B-15E0-4a02-800D-DF882165BB18}")) IDMUSProdConductorPrivate;
struct __declspec(uuid("{B232A288-B2CE-11d1-888F-00C04FBF8D15}")) IDMUSProdTempoMapMgr;
#endif

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif // !defined(DMPPRIVATE_H__27383763_5F0B_11d2_8916_00C04FBF8D15__INCLUDED_)
