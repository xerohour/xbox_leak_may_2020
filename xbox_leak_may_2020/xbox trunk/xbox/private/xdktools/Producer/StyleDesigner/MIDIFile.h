#ifndef MIDIFILE_H
#define MIDIFILE_H
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#include <dmusicf.h>
#include <dmuspriv.h>
#include <dmusicc.h>
#pragma warning( pop )
#include "alist.h"
#include "templates.h"



/*  MIDI status bytes ==================================================*/

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
#define ET_NOTEOFF  ( MIDI_NOTEOFF >> 4 )  // 0x08
#define ET_NOTEON       ( MIDI_NOTEON >> 4 )   // 0x09
#define ET_PTOUCH       ( MIDI_PTOUCH >> 4 )   // 0x0A
#define ET_CCHANGE      ( MIDI_CCHANGE >> 4 )  // 0x0B
#define ET_PCHANGE      ( MIDI_PCHANGE >> 4 )  // 0x0C
#define ET_MTOUCH       ( MIDI_MTOUCH >> 4 )   // 0x0D
#define ET_PBEND        ( MIDI_PBEND >> 4 )    // 0x0E
#define ET_SYSX         ( MIDI_SYSX >> 4 )     // 0x0F
#define ET_PBCURVE          0x03
#define ET_CCCURVE      0x04
#define ET_MATCURVE     0x05
#define ET_PATCURVE     0x06
#define ET_TEMPOEVENT   0x01
#define ET_NOTDEFINED   0

#define NUM_MIDI_CHANNELS	16

/* FullSeqEvent is SeqEvent plus next pointers*/
typedef struct FullSeqEvent : DMUS_IO_SEQ_ITEM
{
	struct FullSeqEvent*   pNext;
	struct FullSeqEvent*	pTempNext; /* used in the compresseventlist routine */
} FullSeqEvent;


DWORD MIDIConvertTime( DWORD dwTime );

FullSeqEvent* MIDICompressEventList( FullSeqEvent* lstEvent );
FullSeqEvent* MIDIMergeEvents( FullSeqEvent* lstLeftEvent, FullSeqEvent* lstRightEvent );
FullSeqEvent* MIDIRemoveDuplicateCCs( FullSeqEvent* lstEvent );
FullSeqEvent* MIDIReverseEventList( FullSeqEvent* lstEvent );
FullSeqEvent* MIDISortEventList( FullSeqEvent* lstEvent );
DWORD MIDIReadEvent( LPSTREAM pStream, DWORD dwTime, FullSeqEvent** plstEvent, DMUS_IO_PATCH_ITEM** pplstPatchEvent, LPSTR* pszTrackName );
HRESULT WriteBandTrack(IDMUSProdRIFFStream* pRiffStream, DMUS_IO_PATCH_ITEM** lstPatchEvent, int nTracks, FullSeqEvent **lstEvent );

DMUS_IO_TIMESIGNATURE_ITEM& MIDITimeSig();	// returns internal timesig used by midi parsing engine (midifile.cpp)
long& MIDITimeSigFlag();			// flag that signals midi parsing engine to scan for time signature (0 == stop scan, 1 == start scan, 2 == midi
													// parsing has found signal.

DMUS_IO_BANKSELECT_ITEM& MIDIBankSelect(int ch);

// this function gets a short that is formatted the correct way
// i.e. the motorola way as opposed to the intel way
BOOL __inline GetMShort( LPSTREAM pStream, short& n )
{
    union uShort
	{
	unsigned char buf[2];
	short n;
	} u;
    unsigned char ch;

    if( S_OK != pStream->Read( u.buf, 2, NULL ) )
    {
	return FALSE;
    }

#ifndef _MAC
    // swap bytes
    ch = u.buf[0];
    u.buf[0] = u.buf[1];
    u.buf[1] = ch;
#endif

    n = u.n;
    return TRUE;
}

short& MIDIsnPPQN();

void MIDIInit();
void MIDIInitBankSelect();
void MIDIInitChordOfComposition();
// this is not being used and should not as overall key and enharmonics is not preserved
//void MIDIGetChordOfComposition(DMUS_CHORD_PARAM& chord);
void MIDIGetChordOfComposition(DWORD& keypattern, DWORD& chordpattern, BYTE& keyroot, BYTE& chordroot, 
							   BOOL& fKeyFlatsNotSharps, BOOL& fChordFlatsNotSharps);

WORD MIDIGetVarLength( LPSTREAM pStream, DWORD& rfdwValue );

#endif