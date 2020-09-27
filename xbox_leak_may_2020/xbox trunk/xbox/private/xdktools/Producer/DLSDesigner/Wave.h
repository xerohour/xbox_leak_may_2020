#ifndef __WAVE_H__
#define __WAVE_H__

// Wave.h : header file
//
#include "DLSComponent.h"
#include "DLSDocType.h"
#include "RiffStrm.h"
#include "CollectionWaves.h"
#include "Info.h"
#include "DLS1.h"
#include "WaveCtl.h"
#include "WaveTimelineDraw.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "WaveCompressionManager.h"
#include "dmusicf.h"

// Definitions for the WAVE RIFF FOURCC's /////
#define FOURCC_DRAG	mmioFOURCC('d','r','a','g')
#define FOURCC_WVST	mmioFOURCC('w','v','s','t')
#define FOURCC_WAVU mmioFOURCC('w','a','v','u')
#define FOURCC_FMT	mmioFOURCC('f','m','t',' ')
#define FOURCC_SMPL	mmioFOURCC('s','m','p','l')
#define FOURCC_DATA mmioFOURCC('d','a','t','a')
#define FOURCC_FACT mmioFOURCC('f','a','c','t')
#define FOURCC_INFO mmioFOURCC('I','N','F','O')
///////////////////////////////////////////////

typedef struct TEMPO_CHANGE
{
	REFERENCE_TIME	rtTempoStart;
	REFERENCE_TIME	rtTempoEnd;
	double			dblTempo;
}TempoChange;


// Anything less than this is not accepted as a valid loop length
#define MINIMUM_VALID_LOOPSTART		0
#define MINIMUM_VALID_LOOPLENGTH	6
#define MSSYNTH_LOOP_LIMIT			524287	// Looped waves will not play correctly with MS Synth beyond this loop length

// Min and Max values for the streaming read ahead.
#ifdef DMP_XBOX
#define MIN_READAHEAD   50  // 50 ms on XBOX
#else
#define MIN_READAHEAD   200 // 200 ms on PC
#endif
#define MAX_READAHEAD   1000  


// Standard data buffer size we'll fetch from the source and temp files
#define WAVE_DATA_BUFFER_SIZE	 4096 

#define DEFAULT_READAHEAD_TIME	500 // Default ReadAhead time for streaming waves is 500ms

#define	SNAP_FORWARD	1
#define SNAP_BACKWARD	SNAP_FORWARD  << 1
#define	SNAP_BOTH		SNAP_FORWARD | SNAP_BACKWARD 


typedef struct _rsmpl 
{
  DWORD dwManufacturer;
  DWORD dwProduct;
  DWORD dwSamplePeriod;
  DWORD dwMIDIUnityNote;
  DWORD dwMIDIPitchFraction;
  DWORD dwSMPTEFormat;
  DWORD dwSMPTEOffset;
  DWORD cSampleLoops;
  DWORD cbSamplerData;
} RSMPL, FAR *LPRSMPL;

typedef struct _orloop 
{
  DWORD dwIdentifier;
  DWORD dwType;
  DWORD dwStart;
  DWORD dwEnd;
  DWORD dwFraction;
  DWORD dwPlayCount;
} RLOOP, FAR *LPRLOOP;

// Structure used to pass the header information between the wave and the data manager / undo manager
typedef struct waveHeaderInfo
{
	DWORD				m_dwWaveLength;	// Length of the wave in samples
	DWORD				m_dwDataSize;	// Size of the wave in bytes

	WSMPL				m_rWSMP;        // New version of wave sample chunk.
	WLOOP				m_rWLOOP;       // New version of loop info.

    RSMPL				m_rRSMP;		// Wave sampling extension (for wave saving only.)
	RLOOP		        m_rRLOOP;       // Loop information (for wave saving

	bool				m_bCompressed;	// Is this wave compressed?

	BOOL				m_bStreaming;	// Is this a streaming wave?
	BOOL				m_bNoPreroll;	// Do we need to pre-roll the wave data?
	DWORD				m_dwReadAheadTime;
	DWORD				m_dwDecompressedStart;
	DWORD				m_dwSamplerate;	// Sample rate
} HEADER_INFO;


typedef struct DMUSP_WAVEOBJECT
{
	WAVEFORMATEX		WaveFormatEx;			 // Standard wave format header.
	BYTE*				pbExtractWaveFormatData; // Compressed waves may have extra data that follows the WAVEFORMATEX structure
	DWORD				dwDataSize;				 // Length of wave (in bytes).	
	BYTE*				pbData;					 // Wave Data (Left channel data for Mono waves).
} DMUSP_WAVEOBJECT;

class CWaveNode;
class CWaveDataManager;
class CMonoWave;
class CWaveCompressionManager;
class CWavePropPgMgr;
class AllocatedPortBufferPair;

class CWave : public IPersistStream, public IDMUSProdPropPageObject
{

	friend class CWaveDataManager;
	friend class CWaveCtrl;
	friend BOOL CALLBACK acmFormatEnumCallback(HACMDRIVERID hadid,           
										   LPACMFORMATDETAILS pafd,  
										   DWORD dwInstance,             
										   DWORD dwSupport);
public:
    CWave(CWaveNode* pNode,DWORD dwSampleRate = 22050, UINT nSampleSize = 16, UINT nChannels = 1);
	virtual ~CWave();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// IPersist functions
    STDMETHOD(GetClassID)(CLSID* pClsId);

	// IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();
	HRESULT STDMETHODCALLTYPE OnShowProperties();

	// Additional methods
public:
	
	// Persistence methods
	virtual HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);				// Load the wave from the passed stream
	virtual HRESULT Save(IDMUSProdRIFFStream* pIRiffStream, UINT wType, BOOL fFullSave) = 0;// Save this wave into the passed stream
	virtual HRESULT SaveAs(IStream* pIStream);												// Saves the WVP as a WAV file
	virtual HRESULT LoadHeader(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);		// Loads only the header info NOT the wave DATA chunk
	virtual HRESULT LoadData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain) = 0;		// Loads only the wave data; the derived objects decide how they want to load the data
	virtual HRESULT PrepareForSave(CString sNewFileName);									// File being saved do whatever's necessary
	virtual HRESULT CleanupAfterSave(CString sFileName);									// File successfully saved so cleanup whatever you need to
	virtual HRESULT	WriteRIFFHeaderToStream(IStream* pIStream, DWORD* pdwBytesWritten) = 0;	// Writes the header in RIFF format to the passed stream upto <data-ck>[size]; returns the number of bytes written
	virtual	HRESULT WriteGUIDChunk(IDMUSProdRIFFStream* pIRiffStream);						// Writes the DMUS_FOURCC_GUID_CHUNK to the passed stream
	
	// The read methods just read the data from the chunks into the correct data structures
	// The derived classes must do the specialized error checking and data massaging that 
	// may need to be done...
	virtual HRESULT ReadDRAG(IStream* pIStream, MMCKINFO* pckMain);
	virtual	HRESULT ReadGUID(IStream* pIStream, MMCKINFO* pckMain);
	virtual HRESULT ReadWVST(IStream* pIStream, MMCKINFO* pckMain) = 0;		   
	virtual HRESULT ReadWAVU(IStream* pIStream, MMCKINFO* pckMain);
	virtual HRESULT ReadFMT(IStream* pIStream, MMCKINFO* pckMain);
	virtual HRESULT ReadWSMP(IStream* pIStream, MMCKINFO* pckMain);
	virtual HRESULT ReadSMPL(IStream* pIStream, MMCKINFO* pckMain);
	virtual HRESULT ReadDataInfo(IStream* pIStream, MMCKINFO* pckMain);
	virtual HRESULT ReadData(IStream* pIStream, MMCKINFO* pckMain, DWORD dwOffset, DWORD dwLength, BYTE* pbData);
	
	virtual HRESULT Download(IDirectMusicPortDownload* pIDMPortDownLoad) = 0;	// Download this wave to the passed port
	virtual HRESULT Unload(IDirectMusicPortDownload* pIDMPortDownLoad) = 0;		// Unload this wave from the passed port
	virtual HRESULT DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad) = 0;	// Get the size of the download buffer and other DMusic related things
	virtual HRESULT	UpdateWave() = 0;											// Redownload the wave to the synth
	
	virtual HRESULT	OnWaveBufferUpdated();										// The wave data has been touched; do the housekeeping things

	// Compression methods
	virtual HRESULT	Compress(const WAVEFORMATEX* pwfDst, bool bInUndoRedo = false) = 0;				// Mono and Stereo waves will handle compression differently	
	virtual HRESULT	UpdateDecompressedWave(const WAVEFORMATEX* pwfxDst) = 0;// Decompresses the wave to the passed format
	virtual HRESULT	SwitchCompression(bool bSwitch) = 0;					// Turns the compression on/off
	virtual DWORD	GetCompressedDataSize() = 0;							// What is the size of the compressed wave?
	virtual DWORD	GetCompressionFormatTag() = 0;							// What's the compression format tag?
	virtual HRESULT GetCompressionFormat(WAVEFORMATEX* pwfxCompression) = 0;// What is the compression WAVEFORMATEX?


	// Transport related methods
	virtual BOOL	WaveIsPlaying();						// Returns true if the wave is currently playing
	virtual HRESULT	Play(BOOL fPlayFromStart);				// Plays the wave
	virtual void	TurnOffMidiNotes();						// Turn all playing notes off; called on stop button click

	// Update methods
	void	UpdateDummyInstrument();						// Updates the dummy instrument
	void			UpdateRefRegions();						// Updates the instruments that have regions that refer this wave
	virtual void	UpdateLoopInRegions();					// Updates the regions using this wave for the loop changes


	// UI Methods
	virtual void	OnDraw(CDC* pDC, const CRect& rcClient) = 0;
	virtual void	OnDraw(CDC* pDC, const CRect& rcClient, const long lScrollOffset,
							DWORD& dwStartSample, DWORD dwLength,
							float fSamplesPerPixel,	const WaveDrawParams* pDrawParams, const WaveTimelineInfoParams* pInfoParams) = 0;
    virtual void    RefreshDrawBuffer() = 0;

	// Operations on the wave data
	virtual HRESULT RemoveSelection(DWORD dwStart, DWORD dwEnd) = 0;// Removes the samples between dwStart and dwEnd
	virtual HRESULT InsertWaveData(BYTE* pbData, WAVEFORMATEX wfxDataFormat, DWORD dwDataSize, DWORD dwStart) = 0;// Inserts the data in passed buffer at dwStart smaple position
	virtual HRESULT Fade(bool bFadeDirection, DWORD dwStart, DWORD dwEnd);
	virtual HRESULT InsertSilence(DWORD dwStart, DWORD dwLength) = 0;
	virtual HRESULT Resample(DWORD dwNewSampleRate);
	virtual	HRESULT SetLoop(DWORD dwStart, DWORD dwEnd);
	virtual HRESULT CopyLoop() = 0;
	virtual HRESULT FindBestLoop(DWORD dwLoopStart, DWORD dwFindStart, DWORD dwFindEnd, int nFindWindow, DWORD* pdwBestLoopEnd) = 0;
    virtual void	RememberLoopBeforeCompression();					// Saves the loop points set before the compression
	virtual	void    RevertToLoopBeforeCompression();					// Reverts to the saved loop points
	virtual void    RememberLoopAfterCompression();						// Saves the loop points set after the compression
	virtual void	RevertToAfterCompression();							// Reverts to the saved decompressed start and loop points

	// Clipboard operations
	virtual HRESULT CopySelectionToClipboard(IStream* pIStream, DWORD dwStart, DWORD dwEnd) = 0;
	virtual HRESULT PasteSelectionFromClipboard(IStream* pIStream, DWORD dwStart) = 0;


	// Methods to deal with the dummy instrument 
	void			ReleaseDummyInstruments();					// Releases the dummy instruments owned by the waves		
	void			SetupDummyInstrument();						// Sets up a dummy instrument (Used by the editors)
	void			ValidateDummyPatch();						// Validates the dummy instrument's msb, lsb, patch
	CInstrument*	GetDummyInstrument();

	// Common virtual accessor and mutator methods
	virtual CString GetName();
	virtual void	SetName(CString sName);
	virtual DWORD	GetDataSize();								// This could be original size or decompressed size
	virtual DWORD	GetUncompressedDataSize() = 0;				// This is always the size of the original wave
	virtual void	SetWSMPL(WSMPL& newWSMPL);					// Sets the WSMPL values
	virtual void	SetWLOOP(WLOOP& newWLOOP);					// Sets the WLOOP values
	virtual void	SetRSMPL(RSMPL& newRSMPL);					// Sets the RSMPL values
	virtual void	SetRLOOP(RLOOP& newRLOOP);					// Sets the RLOOP values
	virtual	int		GetTimePlayed();							// Gets the time already played by the wave
	virtual BOOL	IsTrueStereo() = 0;								// Is the wave true stereo?
	
	virtual	HRESULT	GetPlayTimeForTempo(int nTempo, DWORD& dwPlayTime) = 0; // Computes the time required for this wave to play once at the passed tempo

	// Operations with the DataManager	
	CWaveDataManager* GetDataManager();						// Returns the data manager for the wave
	HRESULT GetData(DWORD dwStartSample, DWORD dwLength, 
					BYTE** ppbData, DWORD& dwBytesRead, bool bGetUncompressedOnly = false);	// Gets the required data from the data manager
    HRESULT GetAllUncompressedData(BYTE** ppbData, DWORD* pdwSize); // Gets the entire uncompressed wave data
	virtual HRESULT CloseSourceHandler() = 0;				// Closes the file handlers to the source
	virtual HRESULT OnSourceRenamed(CString sNewName) = 0;

	// Common base operations on the wave
	HRESULT PromptForSaveFileName(CString& sSaveFileName, CString& sSavePath);	// Opens a save as dialog and returns the chosen file name and path
	virtual HRESULT SaveAs(IStream* pIStream, bool bPromptForFileName);			// Save out the wave file
	void	Replace();															// Replace this wave everywhere it's referenced
	virtual void SnapToZero(DWORD& dwSample, UINT nDirection = SNAP_BOTH) = 0;								// Get the nearest zero crossing
	void	RefreshPropertyPage();												// Refresh the poroperty page
	
    virtual CString GetUndoMenuText(bool bRedo = false);
    virtual void UpdateOnUndo(HEADER_INFO headerInfo);							// Update the wave after undo/redo actions
    virtual HRESULT	SaveUndoState(UINT uUndoStrID);
    virtual HRESULT Undo();
    virtual HRESULT Redo();
    virtual HRESULT PopUndoState();                                             // Deletes the last saved undo state

	// Helpers for the clipboard operations
	HRESULT ReadDataFromClipbaord(IStream* pIStream, WAVEFORMATEX& wfxClipFormat, 
									BYTE** ppbData, DWORD& dwDataSize);			
	virtual HRESULT WriteDataToClipboard(IStream* pIStream, DWORD dwStartSample, DWORD dwClipLength); 
	HRESULT WriteDataToStream(IStream* pIStream, DWORD dwStartSample, DWORD dwLength, DWORD& dwSamplesWritten);

	// Methods for a streaming wave
	void	CheckSizeAndSetStreaming();							// Checks for the wave size and sets the streaming options
	BOOL	IsStreamingWave();
	void	SetStreamingWave(BOOL bStream);
	BOOL	IsNoPrerollWave();
	void	SetNoPrerollWave(BOOL bNoPreroll);
	DWORD	GetReadAheadTime();
	void	SetReadAheadTime(DWORD dwReadAheadTime = DEFAULT_READAHEAD_TIME);
	HRESULT GetStreamingWaveHeader(_DMUS_IO_WAVE_HEADER* pStreamingWaveHeader);

	// Common base accessors mutators 
	GUID			GetFileGUID();								// Gets the m_guidFile that's written to DMUS_FOURCC_GUID_CHUNK
	void			SetFileGUID(GUID guidNew);					// Sets the m_guidFile that's written to DMUS_FOURCC_GUID_CHUNK
	CWaveNode*		GetNode() const;							// Gets the CWaveNode* this wave belongs to
	CWaveCtrl*		GetWaveEditor() const;						// CWaveCtrl* for this wave
	virtual void	SetCollection(CCollection* pCollection);	// Sets the collection for the wave
	CCollection*	GetCollection() const;						// Gets the collection for the wave

	int		GetNumberOfLoops();									// returns WSMPL.cSampleLoops
	DWORD	GetWaveLength();									// Gets the length of the wave
	bool	IsStereo();											// Is this a stereo wave?
	bool	IsLooped();											// Is the wave looped?
	void	SetLooped(bool bLoop);								// Sets the loop flag
	bool	IsUnityNoteAndFractionSame(const RSMPL& rsmpl, 
										short sUnityNote, 
										short sFineTune);		// Checks the unitynote and the MIDIPitchFraction
	void	CopySMPLToWSMP();									// Copies the legacy RSMPL structure into DLS WSMPL structure
    void	CopyWSMPToSMPL();									// Copies DLS WSMPL structure into legacy RSMPL the structure
	WSMPL	GetWSMPL();											// Returns the WSMPL struct
	WLOOP	GetWLOOP();											// Returns the WLOOP struct
	RSMPL	GetRSMPL();
	RLOOP	GetRLOOP();

	/* returns channel count */
	virtual LONG GetChannelCount() = 0;

	/* returns given channel */
	virtual CMonoWave *GetChannel(LONG iChannel) = 0;
	
	/* sets wavelink parameters according to channel number, returns the channel */
	virtual void SetWaveLinkPerChannel(LONG iChannel, WAVELINK *pwl) = 0;

	// Utility methods
	HRESULT	SafeAllocBuffer(BYTE** ppbBuffer, DWORD dwSize);	// Allocates and zeroes a buffer of size dwSize 
	virtual HRESULT FileNameChanged(CString sNewFileName) = 0;  // Called when the wavenode's file name changes
	void	NotifyWaveChange(bool bSetDirtyFlag);				// Notifies all the nodes that the wave has changed
	CString	SampleToTime(DWORD dwSample);						// Returns hr:min:sec:ms format string for the sample

	IStream*			GetHeaderMemStream();					// Gets the header in a memory stream in RIFF format
	virtual HRESULT		UpdateHeaderStream();					// Updates the header memory stream
	DWORD				GetRIFFHeaderSize();					// Returns the size of the header 

	void	SetHeaderOffset(DWORD dwOffset);
	void	SetDataOffset(DWORD dwOffset);		
	
    void    ValidateWLoop(WLOOP* pWLOOP);
	
	void	SetDirtyFlag();
	void	ClearDirtyFlag();
				
	USHORT	GetUnityNote();

	DWORD	GetWaveID() const;
	void	SetWaveID(const DWORD dwID);
	bool	GetWaveForID(DWORD dwID);

	/* returns whether the passed-in wave points to this one or any of its channels */
	bool IsReferencedBy(CWave *pWave);

	/* get/set start sample where decompressed data should start playing */
	void SetDwDecompressedStart(DWORD dwDecompressedStart) { m_dwDecompressedStart = dwDecompressedStart; }

	/* returns decompressed start, or 0 if bConsider is true and value shouldn't be taken into account */
	DWORD GetDwDecompressedStart(bool bConsider)
		{ return (bConsider && !FConsiderDecompressedStart()) ? 0 : m_dwDecompressedStart; }

	/* returns whether decompressed start should be taken into account for displaying or playing */
	bool FConsiderDecompressedStart();

	/* UI wrapper that attempts to set the decompressed start to the given value */
	HRESULT HrSetDecompressedStart(DWORD dwDecompressedDataStart);

	/* UI wrapper that validates loop start and end. Caller should only save undo state and set loop values on success. */
	HRESULT HrValidateLoopValues(DWORD &dwLoopStart, DWORD &dwLoopLength, const bool bLockLoopLength);

	/* loop length has changed in wave, update all regions referencing the wave */
	void UpdateLoopStartAndLengthInRegions();

	/* returns the minimum and maximum value of selection (and loop) */
	DWORD GetDwSelMax() { return m_dwWaveLength - GetDwDecompressedStart(true); }

	/* ensures the given sample count is within selection boundaries */
	void ConstrainToSelectionBoundaries(int& nSel);

protected:
	
	HRESULT FindSampleLoss(const WAVEFORMATEX* pwfxDest, DWORD& dwDifference);	// Finds out the number of samples lost in decompression


	HRESULT CalculateConversionSteps(const WAVEFORMATEX* pwfxSrc, 
										const WAVEFORMATEX* pwfxDst, 
										bool bEncode);							// Figures out the number of conversion steps required

	HRESULT ConvertWave(const WAVEFORMATEX* pwfxSource, 
						const WAVEFORMATEX* pwfxDest, 
						BYTE* pbSource, 
						const DWORD dwSourceSize, 
						BYTE** pbDest, 
						DWORD& cbConverted); 


	HRESULT ConvertWave(const DMUSP_WAVEOBJECT& sourceWave, 
						const WAVEFORMATEX *pwfxDst, 
						DMUSP_WAVEOBJECT& convertedWave, 
						bool bEncode, 
						DWORD dwSilenceSamples = 0);							// Converts the wave to the required format

	int	msaudioGetSamplePerFrame(int cSamplePerSec, 
								DWORD dwBitPerSec, 
								int cNumChannels, 
								int nVersion);									// Silent samples added by MSAudio codec

	WAVEFORMATEX*	CopyWaveFormat(const WAVEFORMATEX* pwfxSrc,	bool bCompress);// Returns a copy of the waveformat structure (inserts MSAudio keys if required)

	HRESULT			CopyWaveObject(const DMUSP_WAVEOBJECT* pSrcWaveObject, 
									DMUSP_WAVEOBJECT* pDstWaveObject,
									DWORD dwDataBufferSize = 0xFFFFFFFF);		// Returns a copy of the source wave object; user must delete this copy

	void			DeleteWaveObjectBuffers(DMUSP_WAVEOBJECT* pWaveObject);		// Deletes the allocated buffers in the object

	void			CleanWaveObject(DMUSP_WAVEOBJECT* pWaveObject);				// Deletes allocated memory and zeroes the structures

	// Attributes
public:
    CUnknownChunk       m_UnknownChunk;

	bool				m_bCompressed;				// This wave is compressed
	bool				m_bLoadingRuntime;			// This wave's being loaded from a Runtime file
	DWORD				m_dwPreviousRegionLinkID;	// Used to store the region link when the instruments are drag-dropped
	
	// Common attributes for both Mono and Stereo waves
	WAVEFORMATEX		m_rWaveformat;				// Standard Wave format header.

	DWORD				m_dwWaveLength;				// Length of wave (in samples).
	CInfo				m_Info;						// Waves can have names, useful for identification.

	BOOL				m_bStreaming;				// Is this a streaming wave
	BOOL				m_bNoPreroll;				// Do we need to pre-roll the wave data?
	DWORD				m_dwReadAheadTime;			// Read ahead time used in streaming waves; convert to REFERENCE_TIME when saving it to the 'wavh' chunk
	
	static CWaveCompressionManager	m_CompressionManager;	// The one and only object that handles compression
	
protected:

	CWaveNode*			m_pWaveNode;			// The node that contains this wave
	CWaveDataManager*	m_pDataManager;			// All data read/write requests go through this object
	CInstrument*		m_pDummyInstrument;		// Used to play the wave
	BOOL                m_bIsPlaying;			// set if we're playing a wave.
    BOOL                m_bPlayLooped;			// True if we should play looped.
	int					m_nTimePlayed;			// Time wave was played.
	short				m_sUnityNoteUsedToTriggerPlay;	// note specified in the playmidievent note_on 
	
	DMUSP_WAVEOBJECT	m_CompressedWavObj;		// The compressed wav (if compression is turned on)
	DMUSP_WAVEOBJECT	m_OriginalWavObj;		// The original uncompressed wav (at all times)
	DMUSP_WAVEOBJECT	m_DecompressedWavObj;	// The decompressed wav (if compression is turned on)

	CCollection*		m_pCollection;			// Collection this wave belongs to...may be NULL
	
	DWORD				m_dwDataSize;			// Size of wave (in bytes).
	ULONG				m_dwHeaderOffset;		// Offset into wave pool.
	DWORD				m_dwDataOffset;			// Offset to the data chunk

	BOOL				m_fReadFormat;			// Make sure Wave read has formatex chunk.
    BOOL				m_fReadWSMP;			// And it has read the WSMP chunk!
    BOOL				m_fReadSMPL;			// And that it has SMPL chunk.

	WLOOP				m_rWLOOPUncompressed;	// WLOOP struct that remembers the loop points set before compression
	WLOOP				m_rWLOOPCompressed;		// WLOOP struct that remembers the loop points set after compression

    BOOL                m_bUseFormatSuggest;    // If we successfully read the WAVU chunk decompression info this is set to TRUE; else we need to do acmFormatSuggest

	// IMPORTANT: the following two member structs need to be together in order
	// for file load logic to work
	WSMPL				m_rWSMP;        // New version of wave sample chunk.
	WLOOP				m_rWLOOP;       // New version of loop info.

/////////////////////////////////////////////////////////////////////////////////////////////
// IMPORTANT: the following two member structs need to be together in order
// for file load logic to work
    RSMPL				m_rRSMPL;       // Wave sampling extension (for wave saving only.)
	RLOOP		        m_rRLOOP;       // Loop information (for wave saving.)

	DWORD				m_dwDecompressedStart;	// sample at which decompressed data should start playing (avoids silence at start of codecs like MP3)

private:
	CWavePropPgMgr*		m_pWavePropPgMgr;	// Manages property pages
    long				m_lRef;				// Reference count for the wave

	CString				m_sTempFileName;
	DWORD				m_dwRIFFHeaderSize;	// Size of the header for the wave to be passed to the wavetrack 
	IStream*			m_pIMemHeaderStream;// Keeps the header in RIFF format so we can pass it to the wavetrack	

	DWORD	m_dwId;								// Load/Save Identifier.(waves number sequentially)
	
	CTypedPtrArray<CPtrArray, WAVEFORMATEX*>			m_arrIntermediateFormats;	// List of intermediate wave objects required for wave conversion
};

#endif // __WAVE_H__
