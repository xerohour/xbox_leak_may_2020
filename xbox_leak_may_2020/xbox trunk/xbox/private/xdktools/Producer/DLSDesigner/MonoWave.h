#ifndef __MONOWAVE_H__
#define __MONOWAVE_H__

// MonoWave.h : header file
//

#include "DLSComponent.h"
#include "DLSDocType.h"
#include "RiffStrm.h"
#include "CollectionWaves.h"
#include "Info.h"
#include "DLS1.h"
#include "WaveCtl.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "WaveCompressionManager.h"


#define DRAW_UNIT_SIZE	100

/*--------------------------------------------------------------------------
CMonoWaveDrawBuffer

Holds a buffer to draw wave files at a given sample rate. Buffer memory is managed
by the class itself (caller should not allocate nor free it).

There are two buffer types, which downsample differently:
- WaveEditor, which are used in the wave editor. Each value is the first sample in
	its sample range of nSamplesPerPixel. 
- WaveTrack, which are used in wave tracks. Each value is the maximum absolute
	value of all samples in the sample range.

The wave "source" can either be a real wave, or another buffer (of the same type)
with a lower sample rate, which can avoid disk reads when switching between buffers
at higher sample rates. No creation or destruction notifications are in place, so caller
needs to release buffers in the right order (from higher to lower sample rate).

Usage:
- initialize the class using one of the Init calls
- call Get with the desired sample start and length (relative to wave start). The
	method returns a buffer pointer and a length. You need to cast the pointer into
	a WORD pointer (or something else) if there is more than one byte per pixel. 
--------------------------------------------------------------------------*/
enum
	{
	mwdbTypeWaveEditor = 1,
	mwdbTypeWaveTrack = 2,
	};

class CMonoWaveDrawBuffer
{
public:
	CMonoWaveDrawBuffer(BYTE mwdbType);
	~CMonoWaveDrawBuffer();

	/* data will be built from wave file */
	void Init(int nSamplesPerPixel, CMonoWave* pMonoWave);

	/* data will be built from another buffer with lower samples / pixel */
	void Init(int nSamplesPerPixel, CMonoWaveDrawBuffer* pMonoWaveDrawBuffer);

	/* ensures the buffer is filled from dwStartSample to a length of dwDrawLength. Returns
		pointer to, and length of, buffer. */
	HRESULT	Get(DWORD dwSampleStart, DWORD dwDrawLength,
		BYTE **ppbBuffer, DWORD *pdwBufferLength);

	/* buffer is dirty and should be recomputed on next Fill call */
	void Dirty();

	/* returns the length of the referenced wave */
	DWORD GetWaveLength();

	/* returns the number of bytes per sample */
	WORD GetBytesPerSample();

	// accessors
	BYTE GetType() { return m_mwdbType; }
	int GetSamplesPerPixel() { return m_nSamplesPerPixel; }

private:
	/* fills the buffer from dwStart to dwEnd */
	HRESULT Fill(DWORD dwUnitStart, DWORD dwUnitEnd);

	/* returns the wave's critical section */
	CRITICAL_SECTION* GetCriticalSection();

	BYTE		m_mwdbType;				// one of the mwdbTypexxx enum
	
	// either one of the following will be non-NULL, depending on which Init method is called
	CMonoWave*				m_pMonoWave;				// reference to the wave to compute draw data from
	CMonoWaveDrawBuffer*	m_pMonoWaveDrawBuffer;		// reference to the wave draw buffer to compute own draw data from
	WORD					m_wBytesPerSample;			// sample size in bytes (1 or 2)

	// all "units" are scaled to the buffer. They correspond to pixels, or (sample number) / (samples per pixel).
	BYTE*		m_pbBuffer;				// buffer
	DWORD		m_dwUnitStart;			// start of valid data in buffer (inclusive), relative to start of wave
	DWORD		m_dwUnitEnd;			// end of valid data in buffer (exclusive), relative to start of wave
	DWORD		m_dwUnitOffset;			// offset of buffer related to start of wave = starting index of buffer
	DWORD		m_cUnits;				// number of samples in the buffer = length of buffer
	int			m_nSamplesPerPixel;		// this also is samples per unit
};

class CMonoWave : public CWave
{
	
public:

	CMonoWave(CWaveNode* pWaveNode, DWORD dwSampleRate = 20050, UINT nSampleSize = 16);
	CMonoWave(CWaveNode*, GUID guidStereoWave, bool bRightChannel, DWORD dwSampleRate = 22050, UINT nSampleSize = 16);
	~CMonoWave();

	// Persistance methods
	HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);				// Loads the Mono wave
	HRESULT Save(IDMUSProdRIFFStream* pIRiffStream, UINT wType, BOOL fFullSave);	// Saves the Mono wave

	HRESULT LoadHeader(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);		// Loads everything except the wave data
	HRESULT LoadData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);			// Loads the wave data

	HRESULT ReadWVST(IStream* pIStream, MMCKINFO* pckMain);		   
	HRESULT ReadFMT(IStream* pIStream, MMCKINFO* pckMain);
	HRESULT ReadDataInfo(IStream* pIStream, MMCKINFO* pckMain);
	
	HRESULT	WriteRIFFHeaderToStream(IStream* pIStream, DWORD* pdwBytesWritten);


	HRESULT Download(IDirectMusicPortDownload* pIDMPortDownLoad);		// Downloads the wave to the DirectMusic port
	HRESULT Unload(IDirectMusicPortDownload* pIDMPortDownLoad);			// Unloads the wave from the DirectMusic port

	/* initializes the download port, using a new download ID */
	HRESULT DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad);		// Initializes the download port

	/* initializes the download port, using dwDMID for its id */
	HRESULT DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad, DWORD dwDMID);

	// Compression methods
	HRESULT Compress(const WAVEFORMATEX* pwfDst, bool bInUndoRedo = false);

		
	HRESULT LoadUncompressedOrDesignTimeWave(IStream* pIStream, 
												DWORD dwChunkSize);		// Loads (and (de)compresses Design-Time Wave

	HRESULT LoadRuntimeCompressedWave(IStream* pIStream, 
										DWORD dwChunkSize);				// Loads (and decompresses) the compressed wave data from the runtime stream type
	
	HRESULT CompressLoadedWaveData();									// Encodes the loaded wave data to the required format and updates the compressed & decompressed wave objects

	void	SetWaveBufferToCompressed(bool bSwitch);					// Switches m_pnWave and m_dwDataSize between Compressed and Decompressed waves.

	HRESULT WriteCompressedData(IStream* pIStream, 
								WAVEFORMATEX* pwfxDest, 
								DWORD& cbWritten);						// Compresses and writes the data to the passed stream
																		
	
	HRESULT	Size(DWORD& dwSize);										    // Calculates the size required for the wave buffer
	HRESULT Write(void* pvoid, IDirectMusicPortDownload* pIDMPortDownLoad);	// Writes the wave data to the download buffer
	DWORD	GetDownloadID(IDirectMusicPortDownload* pIDMPortDownLoad) const;// Returns the download ID m_dwDMId

	// Drawing functions
	void	OnDraw(CDC* pDC, const CRect& rcClient);
	void	OnDraw(CDC* pDC, const CRect& rcClient, const long lScrollOffset,						
					DWORD& dwStartSample, DWORD dwLength,
					float fSamplesPerPixel,	const WaveDrawParams* pDrawParams, const WaveTimelineInfoParams* pInfoParams);
    void    RefreshDrawBuffer();

	// Additionalfunctions
	CString	GetName(); 

	DWORD	GetCompressionFormatTag();
	BYTE*	GetCompressionFormatExtraInfo();
	
	HRESULT	GetCompressionFormat(WAVEFORMATEX* pwfxCompression);
	
	BOOL    GetWaveForDownloadID(DWORD dwID);

	ULONG	GetWavePoolOffset();					// Gets the offset into the wave pool; set at save time

	HRESULT GetChannelData(DWORD dwStartSample, DWORD dwLength, BYTE** ppbData, bool bGetUncompressedOnly = false);
    HRESULT GetAllUncompressedChannelData(BYTE** ppbData, DWORD* pdwSize);

	BOOL	IsTrueStereo();
	HRESULT CloseSourceHandler();

	HRESULT UpdateWave();
	HRESULT	UpdateDecompressedWave(const WAVEFORMATEX* pwfxDst);
	HRESULT	SwitchCompression(bool bSwitch);
	
	HRESULT CopySelectionToClipboard(IStream* pIStream, DWORD dwStart, DWORD dwEnd);
	HRESULT PasteSelectionFromClipboard(IStream* pIStream, DWORD dwStart);

	HRESULT	OnWaveBufferUpdated();
	HRESULT RemoveSelection(DWORD dwStart, DWORD dwEnd);
	HRESULT InsertWaveData(BYTE* pbData, WAVEFORMATEX wfxDataFormat, DWORD dwSamples, DWORD dwStart);
	HRESULT InsertSilence(DWORD dwStart, DWORD dwLength);
	HRESULT CopyLoop();
	HRESULT FindBestLoop(DWORD dwLoopStart, DWORD dwFindStart, DWORD dwFindEnd, int nFindWindow, DWORD* pdwBestLoopEnd);
	void	SnapToZero(DWORD& dwSample, UINT nDirection = SNAP_BOTH);

	int		GetNumberOfLoops() const;									// Gets the number of loops set in WSMPL struct

	DWORD	GetDataSize();					// This might be the original data size or the size of the decompressed wave
	DWORD	GetUncompressedDataSize();		// This is always the original wave size
	DWORD	GetCompressedDataSize();		// This is the size of the compressed wave
	HRESULT	UpdateDataForStereoCompression(const DMUSP_WAVEOBJECT& stereoWaveObject);
	HRESULT SetUncompressedData(const DMUSP_WAVEOBJECT& stereoWaveObject);

	HRESULT	GetPlayTimeForTempo(int nTempo, DWORD& dwPlayTime);

	GUID	GetGUID();
	void	SetGUID(const GUID& guidStereoWave);

	short*	GetWaveData();

	void	UpdateOnUndo(HEADER_INFO headerInfo);

	AllocatedPortBufferPair* GetAllocatedBufferForPort(IDirectMusicPortDownload* pIDMPortDownLoad) const;	// Gets the buffer allocated (in DM_Init) for this port
	void					 RemoveFromAllocatedBufferList(AllocatedPortBufferPair* pAllocatedBuffer);	// Releases the allocated buffer and removes the item from the list

	HRESULT FileNameChanged(CString sNewFileName);
	
	HRESULT OnSourceRenamed(CString sNewName);

	HRESULT	UpdateHeaderStream();			 // Updates the header memory stream
	
	void	SetPreviousRegionLinkID(DWORD dwID);
    
    HRESULT GetDMIDForPort(IDirectMusicPortDownload* pIDMDownloadPort, DWORD* pdwDMID) const;

	/* returns channel count */
	virtual LONG GetChannelCount() { return 1; }

	/* returns given channel */
	virtual CMonoWave *GetChannel(LONG iChannel);

	/* sets wavelink parameters according to channel number */
	virtual void SetWaveLinkPerChannel(LONG iChannel, WAVELINK *pwl);

private:
	HRESULT StripChannelData(const BYTE* pbSrcData, DWORD dwChunkSize, BYTE* pbDestBuffer);
	HRESULT MergeStereoDataBuffers(short* pbLeftChannelData, short* pbRightChannelData, WAVEFORMATEX wfxClipFormat, short* pbMergedData, DWORD dwDataSize);

	HRESULT WriteFormat(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime);
	HRESULT	WriteCompressionChunk(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime);
	HRESULT WriteData(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime);
	HRESULT WriteFACTChunk(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT WriteRSMPLChunk(IDMUSProdRIFFStream* pIRiffStream);

	/* looks for the zero crossing start at dwSample, in the given nDirection, within the limits of dwLeft and dwRight.
		If found, returns the offset from dwSample in iOffset. Returns 0 if not found. */
	int		FindSnapToZero(BYTE *pbData, DWORD dwSample, DWORD dwLeft, DWORD dwRight, UINT nDirection);

	template <class T> DWORD DiffPatterns(T* pSourcePattern, T* pMatchPattern, DWORD dwPatternLength);
	template <class T> HRESULT MixStereoToMono(T* pStereoData, T* pMonoData, DWORD dwStereoLength);

public:

	bool	m_bStripChannelData;				// Do we need tostri the data while loading this wave?
    
    CRITICAL_SECTION    m_DrawBufferCreateLock; 

private:

	CTypedPtrList<CPtrList, AllocatedPortBufferPair*>	m_lstAllocatedBuffers;		// List of allocated buffers to be unloaded

	short*				m_pnWave;			// Sample data

	DWORD				m_dwDLSize;		// Size of the download buffer

	GUID				m_guidStereoWave;				// GUID that identifies the stereo wave this mono wave belongs to
	bool				m_bRightChannel;				// Is this the right channel of a stereo wave?

	DWORD				m_cbSizeOffsetTable;

	// wave track buffer
	CMonoWaveDrawBuffer *m_pdbWaveTrack;
	
	// wave editor buffers
	int					m_nSamplesPerPixelOptimum;		// desired sample per pixels rate for optimum zoom buffer. 0 = not initialized, MAX_INT = buffer will never be created
	CMonoWaveDrawBuffer *m_pdbOptimum;					// zoom buffer, from which all lower zooms are computed
	CMonoWaveDrawBuffer *m_pdbLatest;					// latest draw buffer
	CMonoWaveDrawBuffer *m_pdb;							// pointer to the draw buffer currently in use
};


#endif // __MONOWAVE_H__
