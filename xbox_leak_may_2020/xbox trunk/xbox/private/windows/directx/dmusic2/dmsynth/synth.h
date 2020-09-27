//      Synth.h
//      Copyright (c) 1996-1999 Microsoft Corporation
//

/*  For internal representation, volume is stored in Volume Cents, 
    where each increment represents 1/100 of a dB.
    Pitch is stored in Pitch Cents, where each increment
    represents 1/100 of a semitone.
*/ 

#ifndef __SYNTH_H__
#define __SYNTH_H__

#pragma warning(disable:4296)

#include "clist.h"
#include "dmdls.h"
#include "dls2.h"
//#include "dsound.h"   
#include "dmusicc.h"
#include "dsoundsequencer.h"
#include "..\shared\dmstrm.h"
#include "..\shared\debug.h"
#include "..\shared\xsoundp.h"

#ifdef DBG
extern DWORD sdwDebugLevel;
#endif

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

#define CC_MODWHEEL     0x01
#define CC_VOLUME       0x07
#define CC_PAN          0x0A
#define CC_EXPRESSION   0x0B
#define CC_SUSTAIN      0x40
#define CC_CUTOFFFREQ   0x4A
#define CC_REVERB       0x5B
#define CC_CHORUS       0x5D
#define CC_MIXBINVOLUME 0x66
#define CC_ALLSOUNDSOFF 0x78
#define CC_RESETALL     0x79
#define CC_ALLNOTESOFF  0x7B
#define CC_MONOMODE     0x7E
#define CC_POLYMODE     0x7F

// rpn controllers
#define CC_DATAENTRYMSB 0x06
#define CC_DATAENTRYLSB 0x26
#define CC_NRPN_LSB     0x62
#define CC_NRPN_MSB     0x63
#define CC_RPN_LSB      0x64
#define CC_RPN_MSB      0x65

// registered parameter numbers
#define RPN_PITCHBEND   0x00
#define RPN_FINETUNE    0x01
#define RPN_COARSETUNE  0x02

/*  Sample format and Sample playback flags are organized
    together because together they determine which 
    mix loop to use.
*/

#define SFORMAT_16              1       // Sixteen bit sample.
#define SFORMAT_8               2       // Eight bit sample.
#define SFORMAT_ADPCM           4       // Four bit sample.
#define SPLAY_MMX               0x10    // Use MMX processor (16 bit only).
#define SPLAY_INTERLEAVED       0x40    // Interleave Buffer 
#define SPLAY_FILTERED          0x80    // Non-trivial filter coeff's


/*  Output buffer format flags, defines whether the buffers being
    played are multi-buffer, interleave or just plain mono
*/
#define BUFFERFLAG_MONO         0x00000000
#define BUFFERFLAG_INTERLEAVED  0x00000001
#define BUFFERFLAG_MULTIBUFFER  0x00000002


typedef long    PREL;   // Pitch cents, for relative pitch.
typedef short   PRELS;  // Pitch cents, in storage form.
typedef long    VREL;   // Volume cents, for relative volume.
typedef short   VRELS;  // Volume cents, in storage form.
typedef long    TREL;   // Time cents, for relative time
typedef short   TRELS;  // Time Cents, in storage form.
typedef LONGLONG    STIME;  // Time value, in samples.
typedef long    MTIME;  // Time value, in milliseconds.
typedef long    PFRACT; // Pitch increment, where upper 20 bits are
                        // the index and the lower 12 are the fractional
                        // component.
typedef long    VFRACT; // Volume, where lower 12 bits are the fraction.

typedef long    TCENT;
typedef short   SPERCENT;

#define COEFF_UNITY 0x40000000  // 1.0 multiplier as a 2.30 number
typedef unsigned long COEFF;    // 2.30 fixed point filter coefficient
typedef long COEFFDELTA;        // 2.30 fixed point filter coefficient delta value

#define FILTER_PARMS_DIM_Q  16      // the number of different resonances in the filter parameter table (rows)
#define FILTER_PARMS_DIM_FC 89      // the number of different cutoff frequencies in the filter parameter table (cols)
#define FILTER_FREQ_RANGE   10688   // the difference in pitch cents between the sample rate of the filter design and the 

#define MAX_VOLUME      0       // No attenuation and no amplification 
#define MIN_VOLUME     -9600    // Below 96 db down is considered off.
#define PERCEIVED_MIN_VOLUME   -8000   // But, we cheat.
#define SAMPLE_RATE_22  22050   // 22 kHz is the standard rate.
#define SAMPLE_RATE_44  44100   // 44 kHz is the high quality rate.
#define SAMPLE_RATE_11  11025   // 11 kHz should not be allowed!
#define STEREO_ON       1
#define STEREO_OFF      0

#define MAX_DAUD_CHAN   8       // Maximum outputs for one voice. 

#define FORCEBOUNDS(data,min,max) {if (data < min) data = min; else if (data > max) data = max;}
#define CLAMP_VOLUME(a) {if (a < DSBVOLUME_MIN) a = DSBVOLUME_MIN; else if (a > DSBVOLUME_MAX) a = DSBVOLUME_MAX;}

class CControlLogic;

/*
>>>>>>>>> comment 
*/
#ifdef XBOX
class CBusIds
{
public:
    CBusIds() 
    {
        m_bControllers[0] = 0;
        m_dwMixBins = 0;
        m_pBuffer = NULL;
    }

    CBusIds(const CBusIds& other)
    {
        memcpy(m_bControllers,other.m_bControllers,8);
        m_dwMixBins = other.m_dwMixBins;
        m_pBuffer = other.m_pBuffer;
        if(m_pBuffer)
        {
            m_pBuffer->AddRef();
        }
    }

    ~CBusIds() 
    {
        if (m_pBuffer)
        {
            m_pBuffer->Release();
        }
    }

    CBusIds& operator=(const CBusIds& other)
    {
        if(this != &other)
        {
            memcpy(m_bControllers,other.m_bControllers,8);
            m_dwMixBins = other.m_dwMixBins;
            if(m_pBuffer)
            {
                m_pBuffer->Release();
            }
            m_pBuffer = other.m_pBuffer;
            if(m_pBuffer)
            {
                m_pBuffer->AddRef();
            }
        }
        return *this;
    }

    void AssignOutput(IDirectSoundBuffer *pBuffer,DWORD dwMixBins,BYTE *pbControllers)
    {
        m_dwMixBins = dwMixBins;
        if (pbControllers)
        {
            memcpy(m_bControllers,pbControllers,8);
        }
        else m_bControllers[0] = 0;
        if (m_pBuffer)
        {
            m_pBuffer->Release();
        }
        m_pBuffer = pBuffer;
        if (m_pBuffer)
        {
            m_pBuffer->AddRef();
        }
    }

    bool HasBuffer() { return m_pBuffer != NULL; }

    IDirectSoundBuffer* GetBuffer() 
    {
        if(m_pBuffer)
        {
            m_pBuffer->AddRef();
        }
        return m_pBuffer;
    }

    DWORD                   m_dwMixBins;      // Which of the 32 mixbins this sends to.
    BYTE                    m_bControllers[8];// Which controllers for which mixbins on multi-mixbin buffer.

private:
    IDirectSoundBuffer *    m_pBuffer;      // Pointer to DSound buffer.
};
#else
class CBusIds
{
public:
    CBusIds();
    ~CBusIds();

    HRESULT     Initialize();
    HRESULT     AssignBuses(LPDWORD pdwBusIds, DWORD dwBusCount);

public:
    DWORD       m_dwBusCount;               // Number of Bus Id's
    DWORD       m_dwBusIds[MAX_DAUD_CHAN];  // Array of bus IDs 
};
#endif
/*  CSourceLFO is the file format definition of the LFO in an
    instrument. This is used to represent an LFO as part of
    a specific articulation set within an instrument that
    has been loaded from disk. Once the instrument is chosen
    to play a note, this is also copied into the CVoice
    object.
*/

class CSourceLFO
{
public:
                CSourceLFO();
    void        Init(DWORD dwMode);
//    void        SetSampleRate(long lDirection);
    void        Verify();           // Verifies that the data is valid.
//    PFRACT      m_pfFrequency;      // Frequency, in increments through the sine table.
//    STIME       m_stDelay;          // How long to delay in sample units.
    VRELS       m_vrMWVolumeScale;  // Scaling of volume LFO by Mod Wheel.
    PRELS       m_prMWPitchScale;   // Scaling of pitch LFO by Mod Wheel.
    VRELS       m_vrVolumeScale;    // Scaling of straight volume signal from LFO.
    PRELS       m_prPitchScale;     // Scaling of straight pitch signal from LFO.

    /* DirectX8 members */
    PRELS       m_prCPPitchScale;   // Scaling of pitch signal from channel pressure. 
    VRELS       m_vrCPVolumeScale;  // Scaling of volume signal from channel pressure.
//>>>>>>>> comments 
    PRELS       m_prCutoffScale;    // Scaling of Cutoff freq >>>>>> 
    PRELS       m_prMWCutoffScale;  // Scaling of Cutoff freq mod wheel
    PRELS       m_prCPCutoffScale;  // Scaling of Cutoff freq channel pressure
    DSLFODESC   m_Registers;
};

/*  CSourceEG is the file format definition of an Envelope
    generator in an instrument.
*/

class CSourceEG
{
public:
                CSourceEG();
    void        Init(DWORD dwEg);
    void        Verify();           // Verifies valid data.
//    STIME       m_stAttack;         // Attack rate.
//    STIME       m_stDecay;          // Decay rate.
//    STIME       m_stRelease;        // Release rate.
    TRELS       m_trVelAttackScale; // Scaling of attack by note velocity.
    TRELS       m_trKeyDecayScale;  // Scaling of decay by note value.
//    SPERCENT    m_pcSustain;        // Sustain level.
//    short       m_sScale;           // Scaling of entire signal.

    /* DLS2 */
//    STIME       m_stDelay;          // Delay rate.
//    STIME       m_stHold;           // Hold rate.
    TRELS       m_trKeyHoldScale;   // Scaling of Hold by note value.
//>>>>>>>> comments 
    PRELS       m_prCutoffScale;    // Scaling of Cutoff feq >>>>>>

    DSENVELOPEDESC m_Registers;     // Envelope descriptor for hardware.
};

//>>>>>>>> comments 

class CSourceFilter
{
public:
                CSourceFilter();
    void        Init();
    void        Verify();

//    PRELS       m_prSampleRate;     // Sample rate in cents
    PRELS       m_prCutoff;         // Cutoff Frequency in absolute pitch
//    PRELS       m_prCutoffSRAdjust; // Cutoff Frequency adjusted to the sampel rate
    VRELS       m_vrQ;              // Resonance
//    DWORD       m_iQIndex;          // Q index          
    PRELS       m_prVelScale;       // Scale by key velocity
    PRELS       m_prKeyScale;       // Scaling by note value.
};

/*  CSourceArticulation is the file format definition of
    a complete articulation set: the LFO and two
    envelope generators.
    Since several regions within one Instrument can 
    share one articulation, a counter is used to keep
    track of the usage.
*/

class CSourceArticulation

{
public:
                CSourceArticulation();
    HRESULT     Load(CRiffParser *pParser);
    HRESULT     Download(DMUS_DOWNLOADINFO * pInfo, 
                    void * pvOffsetTable[], DWORD dwIndex, 
                    DWORD dwSampleRate, BOOL fNewFormat);
#ifdef DDUMP
    void        Dump(DWORD dwIndent,DWORD dwLevel);
#endif
    void        Init(BOOL fWave);
    void        Verify();           // Verifies valid data.
    void        AddRef();
    void        Release();
    CSourceEG   m_PitchEG;          // Pitch envelope.
    CSourceEG   m_VolumeEG;         // Volume envelope.
    CSourceLFO  m_LFO;              // Low frequency oscillator.
//    DWORD       m_dwSampleRate;
    WORD        m_wUsageCount;      // Keeps track of how many times in use.
    short       m_sDefaultPan;      // default pan (for drums)

    /* DLS2 */
    CSourceLFO  m_LFO2;             // Vibrato
    CSourceFilter m_Filter;         // Low pass filter
};

/*  Since multiple regions may reference
    the same Wave, a reference count is maintained to
    keep track of how many regions are using the sample.
*/

class CWave : public CListItem
{
public:
                    CWave();
                    ~CWave();
#ifdef DDUMP
    void            Dump(DWORD dwIndent,DWORD dwLevel);
#endif
    HRESULT         Load(CRiffParser *pParser);
    void            Verify();           // Verifies that the data is valid.
    void            Release();          // Remove reference.
    void            AddRef();           // Add reference.
    void            PlayOn();           // Increment play count.
    void            PlayOff();          // Decrement play count.
    BOOL            IsPlaying();        // Is currently playing?
    CWave *         GetNext() {return(CWave *)CListItem::GetNext();};
    DWORD           m_dwSampleLength;   // Length of sample.
    DWORD           m_dwSampleDataSize; // Size in bytes.
    DWORD           m_dwSampleRate;
    HRESULT ( CALLBACK *m_lpFreeHandle)(HANDLE,HANDLE);
    XBOXADPCMWAVEFORMAT m_WaveFormat;   // Wave format from file.
    HANDLE          m_hUserData;        // Used to notify app when wave released.
    short *         m_pnWave;
    DWORD           m_dwID;             // ID for matching wave with regions.
    WORD            m_wUsageCount;      // Keeps track of how many times in use.
    WORD            m_wPlayCount;       // Wave is currently being played.
    BYTE            m_bSampleType;

    /* DirectX 8 members */
    BYTE            m_bStream;          // This wave is used as a streaming buffer
    BYTE            m_bActive;          // This buffer is currently be used to play out of
    BYTE            m_bValid;           // Indicates data in the buffer is valid 
    BYTE            m_bLastSampleInit;  // Indicates the the buffers last sample has been initialize
};


class CWavePool : public CList
{
public:
    CWave *         GetHead() {return (CWave *)CList::GetHead();};
    CWave *         GetItem(DWORD dwID) {return (CWave *)CList::GetItem((LONG)dwID);};
    CWave *         RemoveHead() {return (CWave *)CList::RemoveHead();};
    void            AddTail(CWave *pItem) {CList::AddTail(pItem);};
};

// Private interface for getting the length of a wave
interface IPrivateWave : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetLength(REFERENCE_TIME *prtLength,
        DWORD *dwLoopStart, DWORD *dwLoopEnd) =0;
    virtual HRESULT STDMETHODCALLTYPE SetWaveParams(REFERENCE_TIME rtReadAhead,
        DWORD dwFlags) =0;
};

DEFINE_GUID(IID_IPrivateWave, 0xce6ae366, 0x9d61, 0x420a, 0xad, 0x53, 0xe5, 0xe5, 0xf6, 0xa8, 0x4a, 0xe4);
DEFINE_GUID(IID_CSourceWave,0xf94831cf, 0x6b90, 0x4138, 0xa9, 0xae, 0xb6, 0xb3, 0xa1, 0x63, 0xbb, 0x1);

// Flags for SetWaveBehavior()

#define DSOUND_WAVEF_ONESHOT        1           /* The wave will be played as a one shot */
#define DSOUND_WAVEF_PORT           2           /* The wave will be played via a DMusic port. */
#define DSOUND_WAVEF_SINK           4           /* The wave will be played via a streamed sink interface. */
#define DSOUND_WAVEF_CREATEMASK     0x00000001  /*  Currently only ONESHOT is define for CreateSource  */

// Source wave for Wave object loaded directly from a file (as opposed to member of DLS collection.)

class CSourceWave : CMemTrack,
    public IDirectSoundWave,    // Standard interface.
    public IPersistStream,      // For file io
    public IDirectMusicObject,  // For DirectMusic loader
    public IPrivateWave         // For GetLength
{
friend class CVoice;
friend class CControlLogic;
public:
    CSourceWave();
    ~CSourceWave();

    // IUnknown
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDirectSoundWave
    STDMETHODIMP GetFormat(LPWAVEFORMATEX pWaveFormatEx, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten);
    STDMETHODIMP CreateSource(IDirectSoundSource **ppSource, LPWAVEFORMATEX pwfx, DWORD dwFlags);
    STDMETHODIMP GetStreamingParms(LPDWORD pdwFlags, LPREFERENCE_TIME prtReadahread);

    // IPersist functions (base class for IPersistStream)
    STDMETHODIMP GetClassID( CLSID* pClsId );

    // IPersistStream functions
    STDMETHODIMP IsDirty();
    STDMETHODIMP Load( IStream* pIStream );
    STDMETHODIMP Save( IStream* pIStream, BOOL fClearDirty );
    STDMETHODIMP GetSizeMax( ULARGE_INTEGER FAR* pcbSize );

    // IDirectMusicObject 
    STDMETHODIMP GetDescriptor(LPDMUS_OBJECTDESC pDesc);
    STDMETHODIMP SetDescriptor(LPDMUS_OBJECTDESC pDesc);
    STDMETHODIMP ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc);

    // IPrivateWave
    STDMETHODIMP GetLength(REFERENCE_TIME *prtLength,
        DWORD *dwLoopStart, DWORD *dwLoopEnd);
    STDMETHODIMP SetWaveParams(REFERENCE_TIME rtReadAhead,
        DWORD dwFlags);
    

private:
    
    // Internal methods.
    HRESULT             Load(CRiffParser *pParser);
    HRESULT             Read(DWORD dwStartPosition, BYTE *pbData, DWORD dwLength); 

    BYTE                m_bActive;
    CRITICAL_SECTION    m_CriticalSection;      // Used to ensure thread safe
    REFERENCE_TIME      m_rtReadAheadTime;      // Readahead for streaming.
    DWORD               m_fdwFlags;             // Various flags, including whether this is a one-shot.
    long                m_cRef;                 // COM reference counter.
    IStream *           m_pStream;              // IStream pointer which is connected to IPersistStream
    XBOXADPCMWAVEFORMAT m_WaveFormat;           // File's format
    CSourceArticulation m_Articulation;         // For articulation embedded in wave file.
    short *             m_pnWave;               // Wave data.
    DWORD               m_dwSampleDataOffset;   // Start point for wave data in stream.
    DWORD               m_dwSampleDataSize;     // Raw size of wave data.
    DWORD               m_dwSampleCount;        // Total number of sample.
    DWORD               m_dwLoopStart; 
    DWORD               m_dwLoopEnd;
    DWORD               m_dwRepeatCount;
    CInfo               m_Info;
};


/*  The CSourceSample class describes one sample in an
    instrument. The sample is referenced by a CSourceRegion
    structure. 
*/
class Collection;

class CSourceSample
{
public:
                CSourceSample();
                ~CSourceSample();
    BOOL        CopyFromWave();
    void        Verify();           // Verifies that the data is valid.
    CWave *     m_pWave;            // Wave in pool.
    DWORD       m_dwLoopStart;      // Index of start of loop.
    DWORD       m_dwLoopEnd;        // Index of end of loop.
    DWORD       m_dwSampleLength;   // Length of sample.
    DWORD       m_dwSampleDataSize; // Size of sample date.
    DWORD       m_dwSampleRate;     // Sample rate of recording.
    PRELS       m_prFineTune;       // Fine tune to correct pitch.
    WORD        m_wID;              // Wave pool id.
    BYTE        m_bSampleType;      // 16 or 8 or ADPCM.
    BYTE        m_bOneShot;         // Is this a one shot sample?
    BYTE        m_bMIDIRootKey;     // MIDI note number for sample.
    DWORD       m_dwLoopType;       // WLOOP_TYPE_xxx
};

/*  The CSourceRegion class defines a region within an instrument.
    The sample is managed with a pointer instead of an embedded
    sample. This allows multiple regions to use the same
    sample.
    Each region also has an associated articulation. For drums, there
    is a one to one matching. For melodic instruments, all regions
    share the same articulation. So, to manage this, each region
    points to the articulation.
*/

class CSourceRegion : public CListItem
{
public:
                CSourceRegion();
                ~CSourceRegion();
#ifdef DDUMP
    void        Dump(DWORD dwIndent,DWORD dwLevel);
#endif
    CSourceRegion *GetNext() {return(CSourceRegion *)CListItem::GetNext();};
    void        Verify();           // Verifies that the data is valid.
    HRESULT     Load(CRiffParser *pParser);
    HRESULT     Download(DMUS_DOWNLOADINFO * pInfo, void * pvOffsetTable[], 
                    DWORD *pdwRegionIX, DWORD dwSampleRate, BOOL fNewFormat);
    CSourceSample m_Sample;       // Sample structure.
    CSourceArticulation * m_pArticulation; // Pointer to associated articulation.
    VRELS       m_vrAttenuation;    // Volume change to apply to sample.
    PRELS       m_prTuning;         // Pitch shift to apply to sample.
    BYTE        m_bAllowOverlap;    // Allow overlapping of note.
    BYTE        m_bKeyHigh;         // Upper note value for region.
    BYTE        m_bKeyLow;          // Lower note value.
    BYTE        m_bGroup;           // Logical group (for drums.)

    /* DLS2 */
    BYTE        m_bVelocityHigh;    // Upper velocity value for region.
    BYTE        m_bVelocityLow;     // Lower velocity value.
    SHORT       m_sWaveLinkOptions; // Wave link chunk option flags
    DWORD       m_dwChannel;        // Region channels, from WAVELINK chunk

    // Channel in m_dwChannel provides voice destination and overrides anything
    // from the articulation.
    //
    inline BOOL IsMultiChannel() const
    { return (BOOL)(m_sWaveLinkOptions & F_WAVELINK_MULTICHANNEL); }
};


class CSourceRegionList : public CList
{
public:
    CSourceRegion *GetHead() {return (CSourceRegion *)CList::GetHead();};
    CSourceRegion *RemoveHead() {return (CSourceRegion *)CList::RemoveHead();};
};


/*  The CInstrument class is really the file format definition
    of an instrument.
    The CInstrument can be either a Drum or a Melodic instrument.
    If a drum, it has up to 128 pairings of articulations and
    regions. If melodic, all regions share the same articulation.
    ScanForRegion is called by ControlLogic to get the region
    that corresponds to a note.
*/

class CInstManager;

class CInstrument :  public CListItem 
{
public:
                    CInstrument();
                    ~CInstrument();
#ifdef DDUMP
    void            Dump(DWORD dwIndent,DWORD dwLevel);
#endif
    void            Init(DWORD dwSampleRate);
    void            Verify();           // Verifies that the data is valid.
    CInstrument *   GetInstrument(DWORD dwProgram,DWORD dwAccept);
    CInstrument *   GetNext() {return(CInstrument *)CListItem::GetNext();};
    CSourceRegion * ScanForRegion(DWORD dwNoteValue, DWORD dwVelocity, CSourceRegion *pRegion = NULL);
    CSourceRegionList m_RegionList;     // Linked list of regions.
    DWORD           m_dwProgram;        // Which program change it represents.
    HRESULT         LoadRegions(CRiffParser *pParser);
    HRESULT         Load(CRiffParser *pParser);
    CMemTrack       m_MemTrack;         // Only exists for debugging.
};

class CInstrumentList : public CList
{
public:
    CInstrument *    GetHead() {return (CInstrument *)CList::GetHead();};
    CInstrument *    RemoveHead() {return (CInstrument *)CList::RemoveHead();};
};
/*
class CWaveBufferList;
class CWaveBuffer : public CListItem 
{
friend CWaveBufferList;
public:
                CWaveBuffer() 
                {
                }
    CWaveBuffer * GetNext() { return (CWaveBuffer *)CListItem::GetNext();};
    CWaveBuffer * GetNextLoop() 
                {
                    // Threat the list as a circular list
                    CWaveBuffer *pbuf;
                    pbuf = (CWaveBuffer *)CListItem::GetNext();
                    if ( pbuf == NULL )
                        pbuf = (CWaveBuffer *)*m_ppHead;

                    return pbuf;
                };

    CWave *     m_pWave;            // pointer to wave object
protected:
    CListItem** m_ppHead;
};

class CWaveBufferList : public CList
{
public:
    CWaveBuffer *GetHead() {return (CWaveBuffer *)CList::GetHead();};
    CWaveBuffer *RemoveHead() {return (CWaveBuffer *)CList::RemoveHead();};

    // Overide these methods so that m_pHead can be added to CWaveBuffer ListItem
    // to allow GetNextLoop() to function as a simple circular buffer list 
    void InsertBefore(CListItem *pItem,CWaveBuffer *pInsert) {pInsert->m_ppHead = &m_pHead; CList::Cat(pItem);};
    void Cat(CWaveBuffer *pItem)     {pItem->m_ppHead = &m_pHead; CList::Cat(pItem);};
    void AddHead(CWaveBuffer *pItem) {pItem->m_ppHead = &m_pHead; CList::AddHead(pItem);};
    void AddTail(CWaveBuffer *pItem) {pItem->m_ppHead = &m_pHead; CList::AddTail(pItem);};
};

class CWaveArt : public CListItem
{
public:
                    CWaveArt();
                    ~CWaveArt();
    void            Release();          // Remove reference.
    void            AddRef();           // Add reference.
    void            Verify();           // Verifies that the data is valid.
    CWaveArt *      GetNext() {return(CWaveArt *)CListItem::GetNext();};
    DWORD           m_dwID;             // ID for matching wave with regions.
    DMUS_WAVEARTDL  m_WaveArtDl;
    WAVEFORMATEX    m_WaveformatEx;
    CWaveBufferList m_pWaves;           // Array of Wave buffers associated with dowload id's
//  DWORD           m_dwSampleLength;
    BYTE            m_bSampleType;
    BOOL            m_bStream;          // Is this a streaming articulation 
    WORD            m_wUsageCount;      // Keeps track of how many times in use.
};

class CWaveArtList : public CList
{
public:
    CWaveArt *      GetHead() {return (CWaveArt *)CList::GetHead();};
    CWaveArt *      RemoveHead() {return (CWaveArt *)CList::RemoveHead();};
};*/

#define WAVE_HASH_SIZE          15      // Keep waves in a hash table of linked lists to speed access.
#define INSTRUMENT_HASH_SIZE    15      // Same with instruments.
#define WAVEART_HASH_SIZE       31

class CCollection : public IDirectMusicCollection, public IPersistStream, public IDirectMusicObject, public AListItem
{
friend class CInstManager;

public:
    
	// IUnknown
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IPersist
	STDMETHODIMP GetClassID(CLSID* pClassID);

    // IPersistStream
	STDMETHODIMP IsDirty();
    STDMETHODIMP Load(IStream* pIStream);
    STDMETHODIMP Save(IStream* pIStream, BOOL fClearDirty) ;
    STDMETHODIMP GetSizeMax(ULARGE_INTEGER* pcbSize) ;

	// IDirectMusicObject 
	STDMETHODIMP GetDescriptor(LPDMUS_OBJECTDESC pDesc);
	STDMETHODIMP SetDescriptor(LPDMUS_OBJECTDESC pDesc);
	STDMETHODIMP ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc);

	// IDirectMusicCollection
	STDMETHODIMP GetInstrument(DWORD dwPatch, IDirectMusicInstrument** pInstrument);
	STDMETHODIMP EnumInstrument(DWORD dwIndex, DWORD* pdwPatch, LPWSTR pName, DWORD cwchName);

	// Class
	CCollection();
    ~CCollection();

    CCollection *   GetNext() {return(CCollection *)AListItem::GetNext();};
private:
    HRESULT         Load(CRiffParser *pParser);
    void            AddInstrument(CInstrument *pInstrument);
    CInstrument *   GetInstrument(DWORD dwProgram,DWORD dwKey);
    HRESULT         LoadWaves(CRiffParser *pParser);
    HRESULT         LoadInstruments(CRiffParser *pParser);
    void            ResolveConnections();
private:
    CMemTrack           m_MemTrack;
    CInfo               m_Info;
	long				m_cRef;
    CInstrumentList     m_InstrumentList[INSTRUMENT_HASH_SIZE];
    CWavePool           m_WavePool[WAVE_HASH_SIZE];
    CRITICAL_SECTION    m_CriticalSection;
};

class CCollectionList : public AList
{
public:
    CCollection *GetHead() {return (CCollection *)AList::GetHead();};
    void        AddHead(CCollection * pC) {AList::AddHead((AListItem *) pC);};
    CCollection *RemoveHead() {return (CCollection *)AList::RemoveHead();};
};


class CInstManager {
friend class CCollection;
public:
                    CInstManager();
                    ~CInstManager();
#ifdef DDUMP
    void            Dump(DWORD dwIndent,DWORD dwLevel);
#endif
    CInstrument *   GetInstrument(DWORD dwPatch,DWORD dwKey,DWORD dwVelocity);
    void            Verify();           // Verifies that the data is valid.
//    void            SetSampleRate(DWORD dwSampleRate);
    HRESULT         Download(LPHANDLE phDownload, 
                            void * pvData,
                            LPBOOL pbFree);
    HRESULT         Unload(HANDLE hDownload,
                            HRESULT ( CALLBACK *lpFreeHandle)(HANDLE,HANDLE),
                            HANDLE hUserData);
    CWave *         GetWave(DWORD dwDLId);
//    CWaveArt *      GetWaveArt(DWORD dwDLId);

private:
    HRESULT         DownloadInstrument(LPHANDLE phDownload, 
                                         DMUS_DOWNLOADINFO *pInfo, 
                                         void *pvOffsetTable[], 
                                         void *pvData,
                                         BOOL fNewFormat);
    HRESULT         DownloadWave(LPHANDLE phDownload,
                                DMUS_DOWNLOADINFO *pInfo, 
                                void *pvOffsetTable[], 
                                void *pvData);
    /* DirectX8 Private Methods */
/*    HRESULT         DownloadWaveArticulation(LPHANDLE phDownload, 
                                   DMUS_DOWNLOADINFO *pInfo, 
                                   void *pvOffsetTable[], 
                                   void *pvData);
    HRESULT         DownloadWaveRaw(LPHANDLE phDownload, 
                                   DMUS_DOWNLOADINFO *pInfo, 
                                   void *pvOffsetTable[], 
                                   void *pvData);*/
    CCollectionList m_CollectionList;
    CWavePool       m_WavePool[WAVE_HASH_SIZE];
    CWavePool       m_FreeWavePool;     // Track waves still in use, but unloaded.
    DWORD           m_dwSampleRate;     // Sample rate requested by app.

    /* DirectX8 Private Memmebers */
//    CWaveArtList    m_WaveArtList[WAVEART_HASH_SIZE];
public:
    DWORD           m_dwSynthMemUse;        /* Memory used by synth wave data */ 

    CRITICAL_SECTION m_CriticalSection; // Critical section to manage access.
    BOOL             m_fCSInitialized;
};


/*
class CWaveEvent {
public:
                CWaveEvent() : 
                m_stTime(0),
                m_bPart(0),
                m_dwVoiceId(0),
                m_vrVolume(0),
                m_prPitch(0),
                m_pWaveArt(NULL)
                {}
public:
    STIME       m_stTime;
    BYTE        m_bPart;
    DWORD       m_dwVoiceId;
    VREL        m_vrVolume;
    PREL        m_prPitch;
    SAMPLE_TIME m_stVoiceStart;
    SAMPLE_TIME m_stLoopStart;
    SAMPLE_TIME m_stLoopEnd;
    CWaveArt*   m_pWaveArt;
};

class CWaveData : public CListItem 
{
public:
                CWaveData();
    CWaveData * GetNext() {return (CWaveData *)CListItem::GetNext();};
    STIME       m_stTime;           // Time this event was recorded.
    CWaveEvent  m_WaveEventData;    // Data stored in event.            
};

class CWaveDataList : public CList
{
public:
    CWaveData *GetHead() {return (CWaveData *)CList::GetHead();};
    CWaveData *RemoveHead() {return (CWaveData *)CList::RemoveHead();};
};

class CWaveIn 
{
public:
                CWaveIn();
                ~CWaveIn();              // Be sure to clear local list.
    BOOL        ClearWave(STIME stTime); // Clear up to time stamp.
    BOOL        RemoveWave(DWORD dwID);  // Remove wave with dwID.
    BOOL        RemoveWaveByStopTime(DWORD dwID, STIME stStopTime);
    BOOL        RecordWave(STIME stTime, CWaveEvent *pWaveData); 
    BOOL        GetWave(STIME stTime, CWaveEvent *pWave); 

private:
    static DWORD m_sUsageCount;         // Keeps track of how many instances so free list can be released.
public:
    static CWaveDataList m_sFreeList;   // Global free list of events.

protected:
    CWaveDataList m_EventList;          // This recorder's list.
    STIME         m_stCurrentTime;      // Time for current value.
    CWaveEvent    m_lCurrentData;       // Current value.
}; 
*/
/*  The CVoiceLFO class is used to track the behavior
    of an LFO within a voice. The LFO is hard wired to 
    output both volume and pitch values, through separate
    calls to GetVolume and GetPitch.
    It also manages mixing Mod Wheel control of pitch and
    volume LFO output. It tracks the scaling of Mod Wheel
    for each of these in m_nMWVolumeScale and m_nMWPitchScale.
    It calls the Mod Wheel module to get the current values 
    if the respective scalings are greater than 0.
    All of the preset values for the LFO are carried in
    the m_CSource field, which is a replica of the file
    CSourceLFO structure. This is initialized with the
    StartVoice call.
*/

class CVoiceLFO 
{
public:
                CVoiceLFO();
//    static void Init();             // Set up sine table.
//    STIME       StartVoice(CSourceLFO *pSource, 
//                    STIME stStartTime,CModWheelIn * pModWheelIn, CPressureIn * pPressureIn);
//    VREL        GetVolume(STIME stTime, STIME *pstTime);    // Returns volume cents.
//    PREL        GetPitch(STIME stTime, STIME *pstTime);     // Returns pitch cents.

    /* DirectX8 Methods */
    void        Enable(BOOL bEnable) {m_bEnable = bEnable;};
    PREL        GetCutoff(STIME stTime);                    // Return filter cutoff

private:
    long        GetLevel(STIME stTime, STIME *pstTime);
    CSourceLFO  m_Source;           // All of the preset information.
//    STIME       m_stStartTime;      // Time the voice started playing.
//    CModWheelIn *m_pModWheelIn;     // Pointer to Mod Wheel for this channel.
//    STIME       m_stRepeatTime;     // Repeat time for LFO.
//    static short m_snSineTable[256];    // Sine lookup table.

    /* DirectX8 Memmebers */
//    CPressureIn *m_pPressureIn;     // Pointer to Channel Pressure for this channel.
    BOOL        m_bEnable;
};

/*  The CVoiceEG class is used to track the behavior of
    an Envelope Generator within a voice. There are two 
    EG's, one for pitch and one for volume. However, they
    behave identically.
    All of the preset values for the EG are carried in
    the m_Source field, which is a replica of the file
    CSourceEG structure. This is initialized with the
    StartVoice call.
*/

class CVoiceEG
{
public:
    static void Init();             // Set up linear attack table.
                CVoiceEG();
    STIME       StartVoice(CSourceEG *pSource, STIME stStartTime, 
                    WORD nKey, WORD nVelocity, STIME stMinAttack);
    void        StopVoice(STIME stTime);
    void        QuickStopVoice(STIME stTime, DWORD dwSampleRate);
    VREL        GetVolume(STIME stTime, STIME *pstTime);    // Returns volume cents.
    PREL        GetPitch(STIME stTime, STIME *pstTime);     // Returns pitch cents.
    BOOL        InAttack(STIME stTime);     // is voice still in attack?
    BOOL        InRelease(STIME stTime);    // is voice in release?

    /* DirectX8 Methods */
    void        Enable(BOOL bEnable) {m_bEnable = bEnable;};
    PREL        GetCutoff(STIME stTime);                    // Return filter cutoff

#ifdef XMIX
    void        SetupEnvelopeDesc(LPDSENVELOPEDESC pEnvDesc, bool bAmplitude);
#endif // XMIX

private:
    long        GetLevel(STIME stTime, STIME *pstTime, BOOL fVolume);
    CSourceEG   m_Source;           // Preset values for envelope, copied from file.
    STIME       m_stStartTime;      // Time note turned on
    STIME       m_stStopTime;       // Time note turned off
    static short m_snAttackTable[201];

    /* DirectX8 Memmebers */
    BOOL        m_bEnable;
};

//>>>>>>>>>>> comment 
class CVoiceFilter
{
public:
    void        StartVoice(CSourceFilter *pSource, CVoiceLFO *pLFO, CVoiceEG *pEG, WORD nKey, WORD nVelocity);
    void        GetCoeff(STIME stTime, PREL prFreqIn, COEFF &cfK, COEFF &cfB1, COEFF &cfB2);
    BOOL        IsFiltered();
    
public:
    CSourceFilter   m_Source;   
    CVoiceLFO       *m_pLFO;
    CVoiceEG        *m_pEG;
//    CPitchBendIn    *m_pPitchBend;

    PREL            m_prVelScale;
    PREL            m_prKeyScale;
    
    static COEFF    m_aK[FILTER_PARMS_DIM_Q][FILTER_PARMS_DIM_FC];
    static COEFF    m_aB1[FILTER_PARMS_DIM_Q][FILTER_PARMS_DIM_FC];
    static COEFF    m_aB2[FILTER_PARMS_DIM_Q][FILTER_PARMS_DIM_FC];
};

/*  The CDigitalAudio class is used to track the playback
    of a sample within a voice.
    It manages the loop points, the pointer to the sample.
    and the base pitch and base volume, which it initially sets 
    when called via StartVoice(). 
    Pitch is stored in a fixed point format, where the leftmost
    20 bits define the sample increment and the right 12 bits
    define the factional increment within the sample. This 
    format is also used to track the position in the sample.
    Mix is a critical routine. It is called by the CVoice to blend
    the instrument into the data buffer. It is handed relative change
    values for pitch and volume (semitone cents and decibel
    cents.) These it converts into three linear values:
    Left volume, Right volume, and Pitch.
    It then compares these new values with the values that existed 
    for the previous slice and divides by the number of samples to 
    determine an incremental change at the sample rate. 
    Then, in the critical mix loop, these are added to the 
    volume and pitch indices to give a smooth linear slope to the
    change in volume and pitch.
*/

#define MAX_SAMPLE    4095
#define MIN_SAMPLE  (-4096)

#define MAXDB            0
#define MINDB           -100
#define TEST_WRITE_SIZE  3000
#define TEST_SOURCE_SIZE 44100

class CSynth;

class CDigitalAudio 
{
public:
//                CDigitalAudio();
//                ~CDigitalAudio();
/*
    void        ClearVoice();
    STIME       StartVoice(CSynth *pSynth,
                    CSourceSample *pSample,
                    PREL prBasePitch, long lKey);
    STIME       StartWave(CSynth *pSynth,
                    CWaveArt *pWaveArt, 
                    PREL prBasePitch,
                    SAMPLE_TIME stVoiceStart,
                    SAMPLE_TIME stLoopStart,
                    SAMPLE_TIME stLoopEnd);

//    inline void BreakLoop()
//    { m_bOneShot = TRUE; }
*/
    static void Init();                         // Set up lookup tables.
    static PFRACT PRELToPFRACT(PREL prPitch);   // Pitch cents to pitch.
    static VFRACT VRELToVFRACT(VREL vrVolume);  // dB to absolute.

//    SAMPLE_POSITION GetCurrentPos() {return m_ullSamplesSoFar;};


/*private:
    void        BeforeBigSampleMix();
    void        AfterBigSampleMix();

private:
    CSourceSample   m_Source;           // Preset values for sample.
//    CSynth *        m_pSynth;           // For access to sample rate, etc.
*/
    static PFRACT   m_spfCents[201];    // Pitch increment lookup.
    static PFRACT   m_spfSemiTones[97]; // Four octaves up and down.
    static VFRACT   m_svfDbToVolume[(MAXDB - MINDB) * 10 + 1]; // dB conversion table.
/*    static BOOL     m_sfMMXEnabled;

private:
    short *     m_pnWave;           // Private pointer to wave.

    PFRACT      m_pfBasePitch;      // Overall pitch.
    PFRACT      m_pfLastPitch;      // The last pitch value.
    PREL        m_prLastPitch;      // Same for pitch, in PREL.
    PFRACT      m_pfLastSample;     // The last sample position.
    PFRACT      m_pfLoopStart;      // Start of loop.
    PFRACT      m_pfLoopEnd;        // End of loop.
    PFRACT      m_pfSampleLength;   // Length of sample buffer.
    BOOL        m_fElGrande;        // Indicates larger than 1m wave.
    ULONGLONG   m_ullLastSample;    // Used to track > 1m wave.
    ULONGLONG   m_ullLoopStart;     // Used to track > 1m wave.
    ULONGLONG   m_ullLoopEnd;       // Used to track > 1m wave.
    ULONGLONG   m_ullSampleLength;  // Used to track > 1m wave.
    DWORD       m_dwAddressUpper;   // Temp storage for upper bits of address.
    BOOL        m_bOneShot;         // Is the source region we're mixing a one-shot?
*/
    /* DLS2 filter members */
/*    COEFF       m_cfLastK;          // Held filter coefficients
    COEFF       m_cfLastB1;
    COEFF       m_cfLastB2;
    long        m_lPrevSample;      // Last two samples, post-filter
    long        m_lPrevPrevSample;
*/
    /* DirectX8 members */
//    CWaveBuffer*    m_pCurrentBuffer;
//    CWaveArt*       m_pWaveArt;
//    ULONGLONG       m_ullSamplesSoFar;
};

VREL VelocityToVolume(WORD nVelocity);
VREL MIDIToPercent(WORD nMIDI);

class CPart;

class CVoice : public CListItem
{
public:
                    CVoice();
#ifdef XMIX
                    ~CVoice();
#endif
    CVoice *        GetNext() {return (CVoice *)CListItem::GetNext();};

    BOOL            StartVoice(CSynth *pControl,
                        CSourceRegion *pRegion, REFERENCE_TIME rtStartTime,
                        CBusIds * pBusIds,
                        WORD nKey,WORD nVelocity,
                        VREL vrVolume,      
                        VREL vrMIDIVolume,
                        PREL prPitch,
                        PREL prMIDIPitch);     

    BOOL            StartWave(CSynth *pSynth,
                        CSourceWave *pWave,
                        DWORD dwVoiceId,
                        REFERENCE_TIME rtStartTime,
                        CBusIds * pBusIds,
                        VREL vrVolume,      
                        VREL vrMIDIVolume,
                        PREL prPitch,
                        PREL prMIDIPitch,
                        DWORD dwVoiceStart,
                        DWORD dwLoopStart,
                        DWORD dwLoopEnd );

    BOOL            StartStreamedWave(CSynth *pSynth,
                        CSourceWave *pWave,
                        DWORD dwVoiceId,
                        REFERENCE_TIME rtStartTime,
                        CBusIds * pBusIds,
                        VREL vrVolume,     
                        VREL vrMIDIVolume,
                        PREL prPitch,
                        PREL prMIDIPitch,
                        DWORD dwVoiceStart,
                        DWORD dwLoopStart,
                        DWORD dwLoopEnd );
    static void     Init();             // Initialize LFO, Digital Audio.
    void            SendVolume(REFERENCE_TIME rtTime);
    void            SendPitch(REFERENCE_TIME rtTime);
    void            SendFilter(REFERENCE_TIME rtTime,BOOL fStart);
    void            SendLFOs(REFERENCE_TIME rtTime);
    void            StopVoice(REFERENCE_TIME rtTime);// Called on note off event.
    void            QuickStopVoice(REFERENCE_TIME rtTime);// Called to get quick release.
    void            ClearVoice();       // Release use of sample.
    void            DoWork(CSynth* pSynth);
    SAMPLE_POSITION GetCurrentPos();
    LPDIRECTSOUNDSEQUENCER GetSequencer();
    HRESULT         AllocateBuffer(XBOXADPCMWAVEFORMAT* pwfxFormat);
    LPDIRECTSOUNDSEQUENCERBUFFER m_pBuffer;
    XBOXADPCMWAVEFORMAT     m_WaveFormat; // Format of current buffer

#define VOICE_NUM_PACKETS 2 // Must match PACKETCOUNT in dmime\audiosink.h

    DWORD           m_dwPacketIndex[VOICE_NUM_PACKETS];
    WORD            m_wCurrentPacket;    // Index into m_dwStreamStatus
    WORD            m_wPacketsSubmitted; // count 0..VOICE_NUM_PACKETS
    DWORD           m_dwStreamPosition;
    DWORD           m_dwLoopStart;      // For streamed looping.
    DWORD           m_dwLoopEnd;  
    PVOID           m_pvBuffer[VOICE_NUM_PACKETS];
    DWORD           m_dwBufferSize;

    LPDIRECTSOUNDSEQUENCERSTREAM m_pStream;
    CSourceWave *   m_pWave;        // Pointer to source wave for streaming.
    void            CreateStream();
    bool            StreamDoWork(REFERENCE_TIME now);
    HRESULT         WritePacket(REFERENCE_TIME rtTime,DWORD dwPacket);
    void            ProcessCompletedPackets(bool bCompleteUnsubmitted);

    void            StopStream(REFERENCE_TIME rt);

private:
//    CDigitalAudio m_DigitalAudio;  // The Digital Audio Engine structure.
    CSynth *    m_pSynth;           // To access sample rate, etc.
    long        m_lDefaultPan;      // Default pan
    PREL        m_prLastCutOff;     // Last cut off value.
    CSourceArticulation m_Articulation; // Copy of source articulation. We may munge some of it. 

public:
    DWORD       m_dwNoteID;         // Unique id to keep all voices that represent layers of one note connected.
    REFERENCE_TIME m_rtStartTime;   // Time the sound starts.
    REFERENCE_TIME m_rtStopTime;    // Time the sound stops.
    BOOL        m_fInUse;           // This is currently in use.
    BOOL        m_fNoteOn;          // Note is considered on.
    BOOL        m_fTag;             // Used to track note stealing.
    BOOL        m_fSustainOn;       // Sus pedal kept note on after off event.
    WORD        m_nPart;            // Part that is playing this (channel).
    WORD        m_nKey;             // Note played.
    BOOL        m_fAllowOverlap;    // Allow overlapped note.
    DWORD       m_dwGroup;          // Group this voice is playing now
    DWORD       m_dwProgram;        // Bank and Patch choice.
    DWORD       m_dwPriority;       // Priority.
    CControlLogic * m_pControl;     // Which control group is playing voice.
    CPart *     m_pPart;            // And which corresponding part.
    DWORD       m_dwVoiceId;        // Used to identify a playing wave
    CSourceRegion *m_pRegion;       // Used to determine which region a voice is playing out of 
    CBusIds     m_BusIds;           // Bus Id's to play on this voice
    DWORD       m_dwLoopType;       // Loop type
    BOOL        m_fIgnorePan;       // If we're part of a multichannel wave/sample
    VREL        m_vrStartVolume;    // Initial volume, as computed by velocity, region, etc.
    PREL        m_prStartPitch;     // Initial pitch, as computed by region, sample rate, etc.
    PREL        m_prStartFilter;    // Initial filter.
    short       m_wFilterQ;         // Initial resonance.
    BOOL        m_fWave;
    BOOL        m_fStreamed;
    BOOL        m_fUseFilter;
    VREL        m_vrLastVolumes[8]; // Last volumes sent.
    PREL        m_prLastPitch;      // Last pitch sent.
    PREL        m_prLastFilter;     // Last filter sent.
};


class CVoiceList : public CList
{
public:
    CVoice *     GetHead() {return (CVoice *)CList::GetHead();};
    CVoice *     RemoveHead() {return (CVoice *)CList::RemoveHead();};
    CVoice *     GetItem(LONG lIndex) {return (CVoice *) CList::GetItem(lIndex);};
};

/*  Finally, ControlLogic is the big Kahuna that manages 
    the whole system. It parses incoming MIDI events
    by channel and event type. And, it manages the mixing
    of voices into the buffer.

  MIDI Input:

    The most important events are the note on and
    off events. When a note on event comes in, 
    ControlLogic searches for an available voice.
    ControlLogic matches the channel and finds the
    instrument on that channel. It then call the instrument's
    ScanForRegion() command which finds the region 
    that matches the note. At this point, it can copy
    the region and associated articulation into the
    voice, using the StartVoice command.
    When it receives the sustain pedal command,
    it artificially sets all notes on the channel on
    until a sustain off arrives. To keep track of notes
    that have been shut off while the sustain was on
    it uses an array of 128 shorts, with each bit position 
    representing a channel. When the sustain releases,
    it scans through the array and creates a note off for
    each bit that was set.
    It also receives program change events to set the
    instrument choice for the channel. When such
    a command comes in, it consults the softsynth.ini file 
    and loads an instrument with the file name described
    in the ini file.
    Additional continuous controller events are managed
    by the CModWheelIn, CPitchBendIn, etc., MIDI input recording
    modules.

  Mixing:

    Control Logic is also called to mix the instruments into
    a buffer at regular intervals. The buffer is provided by the
    calling sound driver (initially, AudioMan.) 
    Each voice is called to mix its sample into the buffer.
*/

typedef struct PerfStats
{
    DWORD dwTotalTime;
    DWORD dwTotalSamples;
    DWORD dwNotesLost;
    DWORD dwVoices;
    DWORD dwCPU;
    DWORD dwMaxAmplitude;
} PerfStats;

#define MAX_NUM_VOICES          32  
#define NUM_EXTRA_VOICES        8   // Extra voices for when we overload.

class CPart
{
public:
                    CPart();
    void            Init(CControlLogic *pControl, CSynth *pSynth);
    void            RecordMIDI(REFERENCE_TIME rtTimeIn, BYTE bStatus, BYTE bData1, BYTE bData2);
    void            PlayNoteOff(REFERENCE_TIME rtTime,BYTE bNote);
    void            PlayNoteOn(REFERENCE_TIME rtTime,BYTE bNote, BYTE bVelocity);
    void            SendVolume(REFERENCE_TIME rtTime);
    void            SendPitch(REFERENCE_TIME rtTime);
    void            SendFilter(REFERENCE_TIME rtTime);
    void            SendLFOs(REFERENCE_TIME rtTime);
    void            SetSustain(REFERENCE_TIME rtTime,BOOL fSustain);
    void            GetVolume(VREL vrVoice,long lDefaultPan, VREL *pvrVolume,VREL *pvrLeft,VREL *pvrRight,VREL *pvrReverb,VREL *pvrChorus);
    void            GetVolume(VREL vrVoice, BYTE pbControllers[], VREL pvrVolume[]);
    void            GetPitch(PREL *prPitch);
    void            GetFilter(PREL *prFilter);
    void            GetModWheel(DWORD *pdwModWheel);
    void            GetPressure(DWORD *pdwPressure);
    void            AllNotesOff(REFERENCE_TIME rtTime);
    void            AllSoundsOff(REFERENCE_TIME rtTime);

    static VREL     m_svrPanToVREL[128];// Converts Pan to db.

    CControlLogic * m_pControl;
    CSynth *        m_pSynth;
    CBusIds         m_BusIds;           // Bus Id's for this channel 
    REFERENCE_TIME  m_rtLastTime;       // Time of the last event to come in. 
    BOOL            m_fSustain;         // Sustain on / off.
    BOOL            m_fMono;            // Mono mode?
    DWORD           m_dwProgram;        // Instrument choice.
    DWORD           m_dwPriority;       // Priorities for each channel.
    PREL            m_prFineTune;       // Fine tune for each channel.
    PREL            m_prScaleTune[12];  // Alternate scale for each channel.
    PREL            m_prCoarseTune;     // Coarse tune.
    short           m_nPitchBend;       // Pitch Bend.
    short           m_nCurrentRPN;      // RPN number.
    WORD            m_nData;            // Used to track RPN reading.
    BYTE            m_bModWheel;        // Mod Wheel.
    BYTE            m_bVolume;          // Volume.
    BYTE            m_bExpression;      // Expression.
    BYTE            m_bPan;             // Pan.
    BYTE            m_bReverbSend;      // Reverb send.
    BYTE            m_bChorusSend;      // Chorus send.
    BYTE            m_bFilter;          // Filter control.
    BYTE            m_bPressure;        // Channel Pressure.
    BYTE            m_bBankH;           // Bank selects for instrument.
    BYTE            m_bBankL;         
    BYTE            m_bPartToChannel;   // Channel to Part converter.
    BYTE            m_bDrums;           // Melodic or which drum?
    BYTE            m_bMixBinVolume[8]; // The 8 MixBin volume controllers.
};


class CControlLogic
{
    friend class CPart;
public:
                    CControlLogic();
                    ~CControlLogic();
    HRESULT         Init(CInstManager *pInstruments, CSynth *pSynth);
    BOOL            RecordMIDI(REFERENCE_TIME rtTime,BYTE bStatus, BYTE bData1, BYTE bData2);
    HRESULT         RecordSysEx(DWORD dwSysExLength,BYTE *pSysExData, REFERENCE_TIME rtTime);
    CSynth *        m_pSynth;
    void            SetGainAdjust(VREL vrGainAdjust);
    HRESULT         SetChannelPriority(DWORD dwChannel,DWORD dwPriority);
    HRESULT         GetChannelPriority(DWORD dwChannel,LPDWORD pdwPriority);

    /* DirectX8 methods */
    BOOL            RecordWaveEvent(REFERENCE_TIME rtTime, BYTE bChannel, VREL vrVolume, PREL prPitchIn, 
                        SAMPLE_TIME stVoiceStart, SAMPLE_TIME stLoopStart, SAMPLE_TIME stLoopEnd,
                        IDirectSoundWave *pIWave, DWORD *pdwVoiceID);
    HRESULT         AssignChannelToOutput(DWORD dwChannel, IDirectSoundBuffer *pBuffer,DWORD dwMixBins, BYTE *pbControllers );
    
private:
    void            GMReset();
    CInstManager *  m_pInstruments;
    CPart           m_Part[16];         // One part for each MIDI channel. 
    BOOL            m_fEmpty;           // Indicates empty lists, no need to flush.
    VREL            m_vrGainAdjust;     // Final stage gain adjust
    BOOL            m_fXGActive;        // Is XG Active?
    BOOL            m_fGSActive;        // Is GS enabled?
    VREL            m_vrMasterVolume;   // Master Volume.

public:
    // This is static to protect the CMIDIRecorder free list, which is also static.
    // 
    static CRITICAL_SECTION s_CriticalSection; // Critical section to manage access.
    static DWORD            m_dwCSRefCount;    // Keep track of how many times it is requested.

    void InitCriticalSection();
    void KillCriticalSection();

};

#endif // __SYNTH_H__

