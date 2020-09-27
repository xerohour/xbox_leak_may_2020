//
// DLSLoadSaveUtils.h : header file for DLSLoadSaveUtils.cpp
//

#ifndef DLSLOADSAVEUTILS_H
#define DLSLOADSAVEUTILS_H

#include "dls1.h"
#include "dls2.h"
#include "RiffStrm.h"
#include <math.h>

typedef long    PCENT;
typedef long    GCENT;
typedef long    TCENT;
typedef long    PERCENT;

BOOL notinrange(long a, long b, long range);

void makeconnection(CONNECTION *pChunk,
					USHORT usSource, 
					USHORT usControl,
                    USHORT usDestination, 
					USHORT usTransform, 
					long lScale);

unsigned long GetFilePos(IStream* pIStream);
void SetFilePos(IStream* pIStream, unsigned long ulPos);
DWORD TimeCents2Mils(TCENT tcTime);
TCENT Mils2TimeCents(DWORD dwMils);
DWORD PitchCents2Hertz(PCENT pcPitch);
PCENT Hertz2PitchCents(DWORD dwHertz);
DWORD PitchCents2PitchFract(PCENT pcRate,DWORD dwSampleRate);

typedef struct LFOParams
{
	// Modulator LFO params
    PCENT       m_pcFrequency;
    TCENT       m_tcDelay;

	GCENT       m_gcVolumeScale;
    PCENT       m_pcPitchScale;
    GCENT       m_gcMWToVolume;
    PCENT       m_pcMWToPitch;
	GCENT		m_gcChanPressToGain;
	PCENT		m_pcChanPressToPitch;
	PCENT		m_pcChanPressToFc;

	// Vibrato LFO Params
	PCENT       m_pcFrequencyVibrato;
    TCENT       m_tcDelayVibrato;

	PCENT       m_pcPitchScaleVibrato;
	PCENT       m_pcMWToPitchVibrato;
	PCENT		m_pcChanPressToPitchVibrato;

} LFOParams;

typedef struct VEGParams
{
	TCENT		m_tcDelay;
    TCENT       m_tcAttack;
	TCENT		m_tcHold;
    TCENT       m_tcDecay;
    PERCENT     m_ptSustain;
    TCENT       m_tcRelease;
	TCENT       m_tcShutdownTime;
    TCENT       m_tcVel2Attack;
    TCENT       m_tcKey2Decay;
	TCENT		m_tcKey2Hold;

} VEGParams;

typedef struct PEGParams
{
	TCENT		m_tcDelay;
    TCENT       m_tcAttack;
	TCENT		m_tcHold;
    TCENT       m_tcDecay;
    PERCENT     m_ptSustain;
    TCENT       m_tcRelease;
    TCENT       m_tcVel2Attack;
	TCENT       m_tcKey2Hold;
    TCENT       m_tcKey2Decay;
    PCENT       m_pcRange;
} PEGParams;

typedef struct FilterParams
{
	PCENT	m_pcInitialFc;
	GCENT	m_gcInitialQ;
	PCENT	m_pcModLFOToFc;
	PCENT	m_pcModLFOCC1ToFc;
	PCENT	m_pcEGToFc;
	PCENT	m_pcVelocityToFc;
	PCENT	m_pcKeyNumToFc;
} FilterParams;

typedef struct MSCParams
{
    PERCENT     m_ptDefaultPan;
} MSCParams;

typedef struct ArticParams
{
    LFOParams		m_LFO;
    VEGParams		m_VolEG;
    PEGParams		m_PitchEG;
	FilterParams	m_FilterParams;
    MSCParams		m_Misc;
} ArticParams;

#define FIVE_HERTZ  (-55791972)

// For J3 move into another file begin
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

// controller numbers
#define CC_BANKSELECTH  0x00
#define CC_BANKSELECTL  0x20

// are these right?  What about 0x64, 0x65?
#define CC_DATAENTRYMSB 0x06
#define CC_DATAENTRYLSB 0x26

#define CC_MODWHEEL     0x01
#define CC_VOLUME       0x07
#define CC_PAN          0x0A
#define CC_EXPRESSION   0x0B
#define CC_SUSTAIN      0x40
#define CC_ALLSOUNDSOFF 0x78
#define CC_RESETALL     0x79
#define CC_ALLNOTESOFF  0x7B
#define CC_FILTERCUTOFF 0x7E
#define CC_FILTERRESO	0x7F

// For J3 move into another file End

#endif // #ifndef DLSLOADSAVEUTILS_H
