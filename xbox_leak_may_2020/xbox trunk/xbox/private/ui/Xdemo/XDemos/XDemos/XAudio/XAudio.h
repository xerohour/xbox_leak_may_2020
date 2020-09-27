#include <xtl.h>
#include <DSound.h>
#include <WinError.h>
#include <assert.h>

//Streaming #define's


// Define the maximum amount of packets we will ever submit to the renderer
#define FILESTRM_PACKET_COUNT 3

// Define the source packet size.  Because we have a transform filter that
// will expand the source data before sending it to the renderer, we have
// to maintain separate packet sizes: one for compressed and one for
// uncompressed.
//
// This value is hard-coded assuming an ADPCM block-alignment of 0x800.  If
// this ever changes, this value will need to be fixed.
#define FILESTRM_SOURCE_PACKET_BYTES 2048


// Define the renderer packet size.  See the comment block above for an
// explanation.
//
// This value is hard-coded assuming an ADPCM block alignment of 0x800 and
// stereo wave data.
#define FILESTRM_RENDER_PACKET_BYTES 8164 //-- make it mono


/*****************************************************************************************************************

	These are flags used for the dwFlags parameter in the HSOUND struct.  3DSOUND will assert that the sound is 3D,
	ISVOICE lets us know that the sound is a voice, ISMUSIC tells us if it is music, RANDOM lets us know we should
	apply random effects to the sound within a certain range, and looping tells us if the sound loops or not

******************************************************************************************************************/

#define THREEDSND	0x00000001		//Is this a 3D Sound
#define ISVOICE		0x00000002		//Is this sound a voice
#define ISMUSIC		0x00000004		//Is this sound music
#define RANDOM		0x00000008		//Determines if we are to apply a random effect within some range

#define TWOTWOKHZ	0x00000010
#define FORFORKHZ	0x00000020

#define ADPCM		0x00000100
#define WMA			0x00000200
#define PCM			0x00000400
		
#define LOOPING		0x00001000

//Masks for checking format, sample rate, & both
#define FORMAT_MASK 0x00000f00
#define SAMPLE_MASK 0x000000f0
#define BUFFER_MASK 0x00000ff0

#define FXAMPMOD		0x00002000
#define FXCHORUS		0x00004000
#define FXCOMPRESSOR	0x00008000
#define FXDISTORTION	0x00010000
#define FXECHO			0x00020000
#define FXFLANGER		0x00040000
#define FXREVERB		0x00080000


//#define's for the real-world multiples of roll-off, doppler & Distance units
#define DISTANCE	1
#define DOPPLER		1
#define ROLLOFF		1

//#define	16BIT	0x00000200
//#define 8BIT	0x00000400

//#define's for use with my handle system

#define NUMHANDLES		256
#define NUM2DSTRUCTS    32
#define NUM3DSTRUCTS	48
#define NUMCHANNELS		8

#define NUMSTREAMS		64
#define NUMVOICEHANDLES 4
#define NUMMUSICHANDLES 2

#define INVALID			-1


/*****************************************************************************************************************

	This structure is what is used to essentially keep track of sounds that are not streamed.

******************************************************************************************************************/

typedef struct _tagSoundHandle
{
	IDirectSoundBuffer8* pBuffer;
	void*				 pvData;
	DWORD				 dwFlags;
	DWORD				 dwEndTime;
} HSOUND, *PHSOUND;


/*****************************************************************************************************************

	This structure is what is used to keep track of the streamed sounds.  Music is the primary example of this,
	although this also encompasses voices.

******************************************************************************************************************/


//JJBUG-- Get the real definition of Vector from Ronin
struct Vector
{
	float x;
	float y;
	float z;
};


//This struct is passed to the xbPlayVoiceStream Function so that we know where to position the 3D Voice
typedef struct _tagVoiceInfo
{
	D3DVECTOR	pPosition;
	D3DVECTOR	pVelocity;
	
	//Cone Data
	DWORD		dwInConeAngle;
	DWORD		dwOutConeAngle;
	Vector		vConeOrientation;
	LONG		lOuterVolume;
}VOICE_INFO,*PVOICE_INFO;


typedef struct _tagMusicHandle
{
	char  szFileName[256];  //256 max char name
	DWORD dwFlags;
	int	  nID;
} SSTREAM, *HSTREAM;



typedef struct _tagStreamHandle
{                


	char				 szFileName[256];						 // Name of this wave file
	XFileMediaObject*    pSourceFilter;                          // Source (wave file) filter
    //XMediaObject*        pTransformFilter;                       // Transform (APDCM decompressor) filter
	BYTE          pvSourceBuffer[FILESTRM_SOURCE_PACKET_BYTES]; // Source filter data buffer
    BYTE          pvRenderBuffer[FILESTRM_RENDER_PACKET_BYTES*FILESTRM_PACKET_COUNT]; // Render filter data buffer
    DWORD                adwPacketStatus[FILESTRM_PACKET_COUNT]; // Packet status array
    DWORD                dwFileLength;                           // File duration, in bytes
    DWORD                dwFileProgress;						 // File progress, in bytes      
 
	IDirectSoundStream*  pRenderFilter;							 // Render (DirectSoundStream) filter

	//Can we use the dwFileProgress?  Queue up the next piece when the piece is >= 75% done? (~2 second buffer to load the next piece)
	bool				 bPlaying;
	bool				 bDone;
	HSTREAM				 hsRef;		// Reference to the handle that this represents
	LONG				 lVol;		//Volume
	DWORD				 dwFlags;	//Flags
} SMUSIC, *HMUSIC;
	
//Voice & Music streams require the same info...but use it slightly differently
#define HVOICE HMUSIC


typedef struct _tagSndHandle
{
	void*		pvData;
	int			nID;
	DWORD		dwFlags;
	DWORD		dwBufSiz;

///////////3 D Data
	D3DVECTOR	pPosition;
	D3DVECTOR	pVelocity;
	
	//Cone Data
	DWORD		dwInConeAngle;
	DWORD		dwOutConeAngle;
	Vector		vConeOrientation;
	LONG		lOuterVolume;
///////////End 3D Data
		
	LONG		lVolume;		//Current volume

}SNDSTRUCT, *SNDHANDLE;

#define HCHANNEL3D SNDHANDLE	//Really the same thing...except that the void* parameter doesn't matter

enum OBSTRUCTION_TYPE;			//JJBUG-- Figure out what obstruction types we need after talking w/ Ronin



//////////////////////////////////////////////WMA STUFF/////////////////////////////////////////////////////////////
//Taken from the XBOX sample...
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


//WMA STUFF
#define PACKET_CNT      4
#define PACKET_SIZE     0x1000*2
#define MAXBUFSIZE      (PACKET_SIZE*PACKET_CNT)

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
//
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CFilterGraph //: public CXBApplication
{
public:
    XFileMediaObject* m_pSourceXMO;         // Source XMO - the WMA file
//    XMediaObject* m_pIntermediateXMO;       // Intermediate XMO - Echo filter
    XMediaObject* m_pTargetXMO;             // Target XMO - DSound

    DWORD         m_dwSourceLength;         // Size of source
    DWORD         m_dwBytesRead;            // Bytes of stream processed

    DWORD         m_dwMinPacketSize;        // For verifying our packet size
    DWORD         m_dwPacketSize;           // Packet size
    DWORD         m_dwMaxBufferCount;       // # of buffers

    WAVEFORMATEX  m_wfxAudioFormat;         // Audio format

    LINKED_LIST   m_SourcePendingList;      // Media Contexts in source list
    LINKED_LIST   m_TargetPendingList;      // Media Contexts in target list

    BYTE		  m_pTransferBuffer[MAXBUFSIZE];        // Buffer data
    BYTE          m_pPacketContextPool[PACKET_CNT * sizeof(MEDIA_PACKET_CTX)];     // Packet contexts

	DWORD		  m_dwPercentCompleted;
	DWORD		  dwFlags;
	HSTREAM		  hsRef;					// Reference to the handle that this represents
	//  CXBFont       m_Font;               // Font renderer

    HRESULT        m_hOpenResult;           // Error code from WMAStream::Initialize()

	bool		  bPlaying;					// Is this a currently active stream
	LONG		  lVol;
    // Virtual calls from CXBApplication
    virtual HRESULT Initialize(char*);
    virtual HRESULT FrameMove();
    virtual HRESULT Cleanup();

    // These functions attach a media context packet to an XMO by 
    // setting up the XMEDIAPACKET struct, calling Process(), and
    // then adding our media packet context to the appropriate list
    HRESULT AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx );
    HRESULT AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx );

    // Handles transferring packets through our filter graph
    HRESULT TransferData();
};




//////////////////////////////////////////////END WMA STUFF/////////////////////////////////////////////////////////////


//Globals:

IDirectSound8* g_pDSnd;
SNDSTRUCT	   g_HandleTable[NUMHANDLES];		//Handles for sound effects
SNDSTRUCT	   g_ChannelTable[NUMCHANNELS];		//Array of actual channels allocated for routing
HSOUND		   g_SndArr[NUM3DSTRUCTS];			//Array of actual sound structures for 3D Sfx
HSOUND		   g_2DSndArr[NUM2DSTRUCTS];		//Array of actual sound structures for 2D Sfx
SMUSIC		   g_MusicArr[NUMMUSICHANDLES];		//Array of actual Music structures
SMUSIC		   g_VoiceArr[NUMVOICEHANDLES];		//Array of actual Voice structures
SSTREAM 	   g_StreamArr[NUMSTREAMS];			//Handles for Music
CFilterGraph   g_WMAStream;						//Our only WMA Stream
LONG		   g_lMusicVol;						//Music Volume
LONG		   g_lVoiceVol;						//Voice Volume
LONG		   g_lDipVol;						//Old Music volume for restoring after the dip...
bool		   g_bVoicePause;					//Are voices paused?
bool		   g_bMusicPause;					//Is the music paused?

//Should the callbacks be global or on a per-Handle basis??
void (WINAPI* g_pRequestFunc)(void*);			//Request Callback
void (WINAPI* g_pContinueFunc)(void*);			//Continue Callback

DS3DLISTENER   g_Listener;						//JJBUG -- Do we even need this?

//Counters for Handles
int			   g_SNDID3D;
int			   g_SNDID2D;
int			   g_MUSICCNT;
int			   g_nVoiceCnt;						//Number of voices currently playing

//Functions

//Regular sound functions
HRESULT __stdcall xbInitAudio();
HRESULT __stdcall xbUninitAudio();												//JJBUG -- TODO- Necessary?
void*   __stdcall xbGetHandle(void*, DWORD, DWORD);
HRESULT __stdcall xbPlaySound(void*, bool);								
HRESULT __stdcall xbStopSound(void*);				
HRESULT __stdcall xbSetVolume(void*, LONG);
HRESULT __stdcall xbSetConeOuterVolume(void*, LONG);	
HRESULT __stdcall xbFadeVolume(void*,LONG,LONG);								//JJBUG -- XDK/HARDWARE
HRESULT __stdcall xbSet2DPan(void*, LONG);										//JJBUG -- NEED XDK Update
HRESULT __stdcall xbFade2DPan(void*, LONG, LONG);								//JJBUG -- NEED XDK Update
HRESULT __stdcall xbSet3DPosition(void*, Vector);									
HRESULT __stdcall xbSet3DVelocity(void*, Vector);									
HRESULT __stdcall xbSet3DCone(void*,DWORD,DWORD,Vector,LONG);
HRESULT __stdcall xbGetFlags(void*, DWORD*);
HRESULT __stdcall xbSetFlags(void*, DWORD);
HRESULT __stdcall xbIsSoundPlaying(void*, bool*);		

//FX Functions
HRESULT __stdcall xbCreateFX(DWORD);											//JJBUG -- XDK/HARDWARE
HRESULT __stdcall xbDeleteFX(DWORD);											//JJBUG -- XDK/HARDWARE
HRESULT __stdcall xbApplyFX(void* DWORD);										//JJBUG -- XDK/HARDWARE

//2D routing to 3D
void*	__stdcall xbGetChannel();												//UNTESTED
HRESULT __stdcall xbPlay3DSoundChannel(HCHANNEL3D);								//UNTESTED
HRESULT __stdcall xbRoute2DSound(void*, HCHANNEL3D);							//UNTESTED

//Listener functions
HRESULT __stdcall xbSetListenerPosition(Vector);
HRESULT __stdcall xbSetListenerVelocity(Vector);
HRESULT __stdcall xbSetListenerOrientation(Vector, Vector);

//Music functions
void*   __stdcall xbGetMusicStream(const char* szFileName, DWORD dwFlags);					
HRESULT __stdcall xbPlayMusic(void*);
HRESULT __stdcall xbSetMusicVolume(LONG);												
HRESULT __stdcall xbContinueMusic(void*);											
HRESULT __stdcall xbFlourishMusic(void*);
HRESULT __stdcall xbStopMusic(void*);
HRESULT __stdcall xbFadeMusic(void*,LONG);										  //JJBUG -- NEED XDK UPDATE				
HRESULT __stdcall xbSetMusicRequestCallback(void WINAPI callBackFunc(void*));		
HRESULT __stdcall xbSetMusicContinueCallback(void WINAPI callBackFunc(void*));
HRESULT __stdcall xbProcess();
HRESULT __stdcall xbPauseMusic();
HRESULT __stdcall xbResumeMusic();
		
//Voice Stream function
void*   __stdcall xbPlayVoiceStream(char* szFileName, bool bDipMusic, PVOICE_INFO pVI, DWORD dwFlags);
HRESULT __stdcall xbSetVoiceVolume(LONG);
HRESULT __stdcall xbPauseVoice(void*);
HRESULT __stdcall xbResumeVoice(void*);

//Voices are 3D, music is not
HRESULT __stdcall xbSetVoicePosition(void*,Vector);
HRESULT __stdcall xbSetVoiceVelocity(void*,Vector);
HRESULT __stdcall xbSetVoiceCone(void*,DWORD,DWORD,Vector,LONG); //Is this necessary?

//Generic Stream Control
HRESULT __stdcall xbPauseAllStreams();
HRESULT __stdcall xbResumeAllStreams();

//NEED XDK UPDATE TO BE ABLE TO REALLY TEST THIS STUFF OUT
//Occlusion/Obstruction functions											
HRESULT __stdcall xbI3DClear();														//JJBUG -- TODO
HRESULT __stdcall xbI3DAddObstruction(Vector, Vector, OBSTRUCTION_TYPE);			//JJBUG -- TODO
HRESULT __stdcall xbI3DAddOcclusion(Vector, Vector, OBSTRUCTION_TYPE);				//JJBUG -- TODO



//Generic Internal Utility functions
PHSOUND SoundFromHandle(SNDHANDLE);

//Stream API Utility Functions
HRESULT __stdcall InitChannels();
void InitStreams();
int GetFreeStream();
HVOICE GetVoiceStream();
HRESULT InitStream(const CHAR*, HMUSIC, DWORD);
HRESULT Process(DWORD*, HMUSIC);
BOOL FindFreePacket(DWORD*, HMUSIC);
HRESULT ProcessSource(HMUSIC);
HRESULT ProcessTransform(DWORD, HMUSIC);
HRESULT ProcessRenderer(DWORD, HMUSIC);
HSTREAM GetCurrentMusic();
HRESULT DownloadScratch(IDirectSound8* pDSnd,PCHAR pszScratchFile);
void __stdcall VoiceDone();