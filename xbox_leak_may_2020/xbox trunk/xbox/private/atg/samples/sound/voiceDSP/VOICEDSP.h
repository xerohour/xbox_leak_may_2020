
#include <stdlib.h>
#include <stdio.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include "fximg.h"
#include <xvoice.h>

//
// We arrived at this packet size based on the requirements of the XMOs.
// The idea is to pick an arbitrary packet size, go through the Init function
// once and then if any asserts fires while verifying the packet sizes, change
// the packet to satisfy all requirements
//
#define PACKET_CNT      8
#define PACKET_SIZE     1024
#define MAXBUFSIZE      (PACKET_SIZE*PACKET_CNT)

const int VOICE_SAMPLING_RATE = 16000;
const int XVOICE_BYTES_PER_MS = (((ULONG)( VOICE_SAMPLING_RATE / 1000.0 )) * 2 );

const int XVOICE_BUFFER_REGION_TIME = 40; 
const int XVOICE_BUFFER_REGION_SIZE = ( XVOICE_BYTES_PER_MS * XVOICE_BUFFER_REGION_TIME );
const int XVOICE_PREBUFFER_REGIONS = 20;
const int XVOICE_TOTAL_REGIONS = ( XVOICE_PREBUFFER_REGIONS + 2 );
const int XVOICE_BUFFER_SIZE = ( XVOICE_BUFFER_REGION_SIZE * XVOICE_TOTAL_REGIONS );

//
// frequency of oscillator that drives the speech alternating FX in the dsp
//

const DWORD MAX_OSCILLATOR_FREQUENCY = 10000;
const DWORD FREQ_STEP = 5000;



#if DBG

extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 1
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_SPAM 5

extern ULONG g_dwVoiceDSPDebugLevel;
#define DBG_SPAM(_exp_) {if (g_dwVoiceDSPDebugLevel >= DEBUG_LEVEL_SPAM) DebugPrint _exp_;}
#define DBG_INFO(_exp) {if (g_dwVoiceDSPDebugLevel >= DEBUG_LEVEL_INFO) DebugPrint _exp;}
#define DBG_ERROR(_exp) {if (g_dwVoiceDSPDebugLevel >= DEBUG_LEVEL_ERROR) DebugPrint _exp;}
#define DBG_WARN(_exp) {if (g_dwVoiceDSPDebugLevel >= DEBUG_LEVEL_WARNING) DebugPrint _exp;}

    #define ASSERT(Expression)  \
    {                                                          \
        if(!(Expression))                                      \
        {                                                      \
            DebugPrint("XBOX RIP:\n");               \
            _asm { int 3 };                                    \
        }                                                      \
    }                                   

#else

#define ASSERT(_exp_)
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp_)
#define DBG_ERROR(_exp_)
#define DBG_WARN(_exp_)

#endif



//
// Linked list structure for tracking our media packet contexts
//
struct LINKED_LIST 
{
    LINKED_LIST* pNext;
    LINKED_LIST* pPrev;

    inline VOID Initialize()         { pNext = pPrev = this; }
    inline BOOL IsListEmpty()        { return pNext == this; }
    inline LINKED_LIST* RemoveHead() { pNext->Remove(); }
    inline LINKED_LIST* RemoveTail() { pPrev->Remove(); }

    inline VOID Remove() 
    {
        LINKED_LIST* pOldNext = pNext;
        LINKED_LIST* pOldPrev = pPrev;
        pOldPrev->pNext = pOldNext;
        pOldNext->pPrev = pOldPrev;
    }

    inline VOID Add( LINKED_LIST* pEntry ) 
    {
        LINKED_LIST* pOldHead = this;
        LINKED_LIST* pOldPrev = this->pPrev;
        pEntry->pNext   = pOldHead;
        pEntry->pPrev   = pOldPrev;
        pOldPrev->pNext = pEntry;
        pOldHead->pPrev = pEntry;
    }
};



// This structure keeps track of our packet status, buffer data, etc.
struct MEDIA_PACKET_CTX
{
    LINKED_LIST ListEntry;
    DWORD       dwStatus;
    DWORD       dwCompletedSize;
    BYTE*       pBuffer;
};


//-----------------------------------------------------------------------------
// Name: class XFilterGraph
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CFilterGraph : public CXBApplication
{
public:
    LPDIRECTSOUND8 m_pDSound;               // DirectSound object
    LPDIRECTSOUNDBUFFER m_pDSBufferOscillator;
    XMediaObject* m_pTargetXMO;             // Target XMO - Headphone
    XMediaObject* m_pTransformXMO;          // Transform XMO - DSound
    XMediaObject* m_pSourceXMO;             // Source XMO - microphone

    DWORD         m_dwMinPacketSize;        // For verifying our packet size
    DWORD         m_dwPacketSize;           // Packet size
    DWORD         m_dwMaxBufferCount;       // # of buffers

    WAVEFORMATEX  m_wfxAudioFormat;         // Audio format

    LINKED_LIST   m_SourcePendingList;      // Media Contexts in source list
    LINKED_LIST   m_TransformPendingList;      // Media Contexts in source list
    LINKED_LIST   m_TargetPendingList;      // Media Contexts in target list

    BYTE*         m_pTransferBuffer;        // Buffer data
    BYTE*         m_pPacketContextPool;     // Packet contexts

    CXBFont       m_Font;                   // Font renderer
    CXBHelp       m_Help;                   // Help object
    BOOL          m_bDrawHelp;              // Should we draw help?

    HRESULT       m_hOpenResult;            // Error code from WMAStream::Initialize()
    BOOL          m_bPaused;                // Paused?

    DWORD         m_dwPortNumber;           // port number
    DWORD         m_dwCount;

    DWORD         m_dwOldPosition;          // sample rate converter delay line index
    DWORD         m_dwCurrentEffect;        // current Effect applied to hawk input

    LPDSEFFECTIMAGEDESC m_pEffectsImageDesc;
    LONG          m_lOscillatorFrequency;

    //
    // set filter coeffs
    //

    LONG                    m_lParamEQFreq;
    LONG                    m_lParamEQGain;
    LONG                    m_lParamEQQ;
    DWORD                   m_dwParam;
    FLOAT                   m_fDLS2Freq;
    LONG                    m_lDLS2Resonance;
    DSFILTERDESC            m_dsfd;                 // DSFILTERDESC struct

    // Virtual calls from CXBApplication
    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT Cleanup();

    HRESULT InitializeStreamingObjects();
    int FindHeadsetPort();

    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP

    // These functions attach a media context packet to an XMO by 
    // setting up the XMEDIAPACKET struct, calling Process(), and
    // then adding our media packet context to the appropriate list
    HRESULT AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx );
    HRESULT AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx );
    HRESULT AttachPacketToTransformXMO( MEDIA_PACKET_CTX* pCtx );

    // Handles transferring packets through our filter graph
    HRESULT TransferData();

};


    
HRESULT
CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer );

