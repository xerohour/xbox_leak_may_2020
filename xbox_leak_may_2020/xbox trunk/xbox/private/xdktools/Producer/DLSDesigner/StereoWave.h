#ifndef __STEREOWAVE_H__
#define __STEREOWAVE_H__

// StereoWave.h : header file
//

class CStereoWave : public CWave
{

public:
	
	CStereoWave(CWaveNode* pWavenode, BOOL bTrueStereo, DWORD dwSampleRate = 22050, UINT nSampleSize = 16);
	CStereoWave(CWaveNode* pWaveNode, GUID guidStereoWave, DWORD dwSampleRate = 22050, UINT nSampleSize = 16);
	~CStereoWave();

	HRESULT Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool bLoadRightChannel = false);
	HRESULT Save(IStream* pIStream);
	HRESULT Save(IDMUSProdRIFFStream* pIRiffStream, UINT wType, BOOL fFullSave);
	HRESULT SaveAs(IStream* pIStream, bool bPromptForFileName);
	HRESULT SaveAs(IStream* pIStream);
	HRESULT LoadData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT LoadHeader(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT	WriteRIFFHeaderToStream(IStream* pIStream, DWORD* pdwBytesWritten);
	HRESULT PrepareForSave(CString sNewFileName);
	HRESULT CleanupAfterSave(CString sFileName);

	HRESULT ReadWVST(IStream* pIStream, MMCKINFO* pckMain);


	HRESULT Download(IDirectMusicPortDownload* pIDMPortDownLoad);
	HRESULT Unload(IDirectMusicPortDownload* pIDMPortDownLoad);
	HRESULT DM_Init(IDirectMusicPortDownload* pIDMPortDownLoad);

	HRESULT UpdateWave();
	HRESULT	UpdateDecompressedWave(const WAVEFORMATEX* pwfxDst);
	HRESULT	SwitchCompression(bool bSwitch);
	virtual void UpdateLoopInRegions();						// Updates the regions using this wave for the loop changes
	void	SyncWaveProperties(bool bCopyFromChannels = true);

    CString GetUndoMenuText(bool bRedo = false);
    HRESULT SaveUndoState(UINT uUndoStrID);
	void	UpdateOnUndo(HEADER_INFO headerInfo);		// Refreshes the wave with the header information
	HRESULT	RefreshWave();								// Forces the wave to refetch it's data and update the views
    HRESULT Undo();
    HRESULT Redo();
    HRESULT PopUndoState();

	void	SetCollection(CCollection* pCollection);

	HRESULT CopySelectionToClipboard(IStream* pIStream, DWORD dwStart, DWORD dwEnd);
	HRESULT PasteSelectionFromClipboard(IStream* pIStream, DWORD dwStart);
    HRESULT WriteDataToClipboard(IStream* pIStream, DWORD dwStartSample, DWORD dwClipLength);
	HRESULT RemoveSelection(DWORD dwStart, DWORD dwEnd);
	HRESULT InsertWaveData(BYTE* pbData, WAVEFORMATEX wfxDataFormat, DWORD dwSamples, DWORD dwStart);
	HRESULT	OnWaveBufferUpdated();
	HRESULT Fade(bool bFadeDirection, DWORD dwStart, DWORD dwEnd);
	HRESULT InsertSilence(DWORD dwStart, DWORD dwLength);
	HRESULT Resample(DWORD dwNewSampleRate);
	HRESULT SetLoop(DWORD dwStart, DWORD dwEnd);
	HRESULT CopyLoop();
	HRESULT FindBestLoop(DWORD dwLoopStart, DWORD dwFindStart, DWORD dwFindEnd, int nFindWindow, DWORD* pdwBestLoopEnd);
	void	SnapToZero(DWORD& dwSample, UINT nDirection = SNAP_BOTH);

    void	RememberLoopBeforeCompression();					// Saves the loop points set before the compression
    void    RevertToLoopBeforeCompression();					// Reverts to the saved loop points
    void    RememberLoopAfterCompression();						// Saves the loop points set after the compression
	void	RevertToAfterCompression();						// Reverts to the saved decompressed start and loop points

	HRESULT Compress(const WAVEFORMATEX* pwfxDst, bool bInUndoRedo = false);
	DWORD	GetCompressionFormatTag();
	HRESULT GetCompressionFormat(WAVEFORMATEX* pwfxCompression);
    HRESULT	UpdateChannelsForStereoCompression();		// Updates the m_DEcompressedWavObj members of left and right channel waves

	DWORD	GetDataSize();				// Size of either the uncompressed or decompressed wave (depending on m_bCompressed flag)
	DWORD	GetCompressedDataSize();	// Size of the compressed wave
	DWORD	GetUncompressedDataSize();	// This is always the size of the original wave
	HRESULT CloseSourceHandler();		// Closes the source file handlers
	HRESULT FileNameChanged(CString sNewFileName);

	BOOL	IsTrueStereo();

	HRESULT	GetPlayTimeForTempo(int nTempo, DWORD& dwPlayTime);	// Computes the time required for this wave to play once at the passed tempo

	// Drawing methods
	void	OnDraw(CDC* pDC, const CRect& rcClient);// Draws the wave on the DC within the passed rect
	void	OnDraw(CDC* pDC, const CRect& rcClient, const long lScrollOffset,
					DWORD& dwStartSample, DWORD dwLength,
					float fSamplesPerPixel, const WaveDrawParams* pDrawParams, const WaveTimelineInfoParams* pInfoParams);
    void    RefreshDrawBuffer();

	CString		GetName();							// Return the name of the wave
	int			GetNumberOfLoops();					// How many loops?
	ULONG		GetWavePoolOffset();				// Offset of this wave in the collection
 		
	void		SetWSMPL(WSMPL& newWSMPL);			// Sets the WSMPL values
	void		SetWLOOP(WLOOP& newWLOOP);			// Sets the WLOOP values
	void		SetRSMPL(RSMPL& newRSMPL);			// Sets the RSMPL values
	void		SetRLOOP(RLOOP& newRLOOP);			// Sets the RLOOP values

	CMonoWave* GetLeftChannel();					// Returns the left channel wave
	CMonoWave* GetRightChannel();					// Returns the right channels wave

	GUID GetGUID();									// GUID that identifies this stereo wave
	void SetGUID(const GUID& guidSrc);				// Set the GUID to identify this stereo wave (this may be used at the load time)

	void SetPreviousRegionLinkIDs(DWORD dwLeft, DWORD dwRight);

	HRESULT		OnSourceRenamed(CString sNewName);

	/* returns channel count */
	virtual LONG GetChannelCount() { return 2; }

	/* returns given channel */
	virtual CMonoWave *GetChannel(LONG iChannel);
	
	/* sets wavelink parameters according to channel number, returns the channel */
	virtual void SetWaveLinkPerChannel(LONG iChannel, WAVELINK *pwl);

private:

	HRESULT	CompressStereoData(const WAVEFORMATEX* pwfxDst, bool bInUndoRedo = false);	// Compresses TRUE stereo data
	HRESULT ConvertStereoData(const DMUSP_WAVEOBJECT& sourceWave, 
						const WAVEFORMATEX* pwfxDst, 
						DMUSP_WAVEOBJECT &pConvertedWave, 
						bool bEncode);							// Convert the stereo data to the passed destination format

	void	GetTrueStereoFormat(WAVEFORMATEX& wfxPCM);	// Returns the true stereo format for this wave
	
	void	CopyInfo(CInfo* pSrcInfo, CInfo* pDstInfo);	// Copies the m_Info members

	HRESULT IsStereoCompressed(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool& bStereoCompressed);
	HRESULT InterleaveBuffersForStereoData(BYTE* pbLeft, BYTE* pbRight, BYTE* pbStereoData, WAVEFORMATEX wfxData, DWORD dwDataSize);
	HRESULT InterleaveChannelsForStereoData();
	HRESULT WriteStereoFormat(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime);
	HRESULT	WriteCompressionChunk(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime);
	HRESULT WriteStereoData(IDMUSProdRIFFStream* pIRiffStream, bool bRuntime);
	HRESULT	WriteInterleavedData(IStream* pIStream);
	HRESULT WriteCompressedData(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT WriteFACTChunk(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT WriteRSMPLChunk(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT WriteWSMPLChunk(IDMUSProdRIFFStream* pIRiffStream);

	HRESULT LoadCompressedStereoData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
	HRESULT ReadFormatChunk(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT ReadWSMPLChunk(IDMUSProdRIFFStream* pIRiffStream, DWORD dwChunkSize);
	HRESULT ReadRSMPLChunk(IDMUSProdRIFFStream* pIRiffStream, DWORD dwChunkSize);

private:
	BOOL		m_bIsPlaying;
	GUID		m_guidStereoWave;		// This GUID used to identify mono waves that 
									// (as separate channels) belong to this Stereo wave

	CMonoWave*	m_pLeftChannel;
	CMonoWave*	m_pRightChannel;

	bool		m_bLoadingRuntime;
	BOOL		m_bTrueStereo;

};


#endif // __STEREOWAVE_H__
