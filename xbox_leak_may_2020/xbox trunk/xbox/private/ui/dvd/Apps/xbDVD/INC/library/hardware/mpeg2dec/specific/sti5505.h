
#ifndef STI5505_H
#define STI5505_H

#include "library\common\prelude.h"
#include "library\common\krnlint.h"
#include "library\common\gnerrors.h"
#include "library\common\profiles.h"
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\memmpdio.h"
#include "library\general\asncstrm.h"
#include "library\hardware\osd\generic\osddefs.h"
#include "library\hardware\audio\generic\audiodac.h"
#if VIONA_VERSION
#include "library\hardware\mpeg2dec\generic\mp2spudc.h"
#endif

#include "library\hardware\mpeg2dec\generic\mp2eldec.h"
#include "library\hardware\mpeg2dec\generic\mp2vsprs.h"

// Set this to 1 to enable a test for corrupted SDRAM decoder memory
#define TEST_DECODER_MEMORY		0

class FrameParameters
	{
	public:
		BYTE				pfh, pfv, ppr1, ppr2;

		MPEG2FrameType				frameType;
		MPEG2PictureStructure   fieldType;

		BOOL				repeatFirstField : 2;
		BOOL				topFieldFirst    : 2;
		BOOL				progressiveFrame : 2;
		BOOL				skipped          : 2;
		DWORD				temporalReference;
		KernelInt64		streamPosition;
		DWORD				pts;
		int				centerVerticalOffset;
		int				centerHorizontalOffset;

		Error Init (MPEG2VideoHeaderParser *params, KernelInt64 streamPosition, DWORD pts);
	};



class FrameParameterQueue
	{
	private:
		FrameParameters	*	params;
		WORD	num, first, last;

	public:
		FrameParameterQueue (WORD size);
		virtual ~FrameParameterQueue (void);

		Error Reset (void);
		Error InsertFrame (MPEG2VideoHeaderParser *parser, KernelInt64 streamPosition, DWORD pts);
		Error MarkSkipped (void);
		BOOL IsSkipped (void);
		Error GetFrame (FrameParameters &params);
		Error GetNextFrame (FrameParameters &params);
		Error NextFrame (void);
		WORD FrameAvail(void) {return (first <= last) ? (last - first) : (num + last - first);}
	};


class STi5505MPEG2Decoder;


class STi5505MPEG2VideoHeaderParser : public MPEG2VideoHeaderParser
	{
	private:
		MemoryMappedIO			*	port;
		STi5505MPEG2Decoder	*	decoder;
		BOOL							odd;
		BOOL							even;
		BYTE							second;
      BYTE							ccbuffer[256];

		Line21Receiver			*	line21Receiver;

		Error WaitForFIFONonEmpty (void);
	protected:
		Error BeginParse (void);
		Error NextByte (void);
		Error EndParse (void);

		Error ParsePictureHeader (void);
		Error ParseUserData(void);
	public:
		BOOL	skipHeader;

		STi5505MPEG2VideoHeaderParser (MemoryMappedIO *port, STi5505MPEG2Decoder * decoder);

		Error SetLine21Receiver(Line21Receiver * line21Receiver) {this->line21Receiver = line21Receiver; GNRAISE_OK;}
	};




class STi5505MPEG2Decoder : public MPEG2VideoDecoder, protected InterruptHandler, protected ASyncRefillRequest
#if VIONA_VERSION
                             ,public OSDDisplay
#endif
	{
	friend class VirtualSTi5505MPEG2Decoder;
	friend class STi5505VBlankIntServer;

	protected:
		// Basic IO operations.
		MemoryMappedIO	*	port;
		MemoryMappedIO * 	subPicturePort;
		BitOutputPort	*	widescreenActivePort;
		int	chipVersion;

		inline void OutByte (int idx, BYTE val);
		inline void InByte (int idx, BYTE __far&val);

		// to avoid double index programming
		inline void OutWordHL (int idx, WORD val);
		inline void OutWordLH (int idx, WORD val);
		inline void OutWordHL0 (int idx, WORD val);
		inline void InWordHL (int idx, WORD &val);

		void WriteMem (DWORD at, DWORD high, DWORD low);
		void WriteMemBlock (DWORD at, DWORD num, const DWORD *data);
#if TEST_DECODER_MEMORY
		void ReadMem (DWORD at, DWORD &high, DWORD &low);
#endif

		DWORD SendAudioData(HPTR data, DWORD size);
		DWORD AvailAudioData(void);
		DWORD AvailAudioBuffer(void);

		int	lastIPBuffer, curIPBuffer, curDBuffer;
		DWORD	lastTempReference;

		WORD 					lumaIBPBuffers[4];
		WORD 					chromaIBPBuffers[4];
		DWORD					bufferStreamPos[3];
		BOOL					bufferValid[3];
		FrameParameters	bufferParams[3];
		FrameParameters	currentDisplayParams;
		FrameParameters	currentDecodingParams;

		PTSCounter			ptsCounter;

		DWORD					currentPTS;
		DWORD					lastDecodePTS;
		DWORD					ptsTimeOffset;
		DWORD					ptsStopTimeOffset;

		KernelInt64	signalPosition;
		KernelInt64	predFrameStreamPos;
		KernelInt64	currentFrameStreamPos;

		KernelInt64	scdcnt;
		KernelInt64	cdcnt;
		KernelInt64	cdoffset;

		BOOL	prevOdd;
		BOOL	currentOdd;

		DWORD frameCnt;
		DWORD ipFrameCount;

		KernelInt64 streamPosition;
		KernelInt64 streamOffset;

		FrameParameterQueue					frames;
		STi5505MPEG2VideoHeaderParser		parser;
		PTSAssoc									ptsAssoc;
		long										conformPTS;
		BOOL										conformPTSValid;

		WORD										videoBitBufferSize;
		WORD										audioBitBufferSize;
		WORD										bitBufferSize;

		DWORD										videoBitrate;
		DWORD										audioBitrate;

		DWORD										frameSize;
		DWORD										lumaSize, chromaSize;
		WORD										osdBase, osdBaseX;
		DWORD										osdSize, osdSizeX;
		BOOL										osdLimitation;

		WORD										bbfWatchdog;
		WORD										predVBL;
		WORD										bbeWatchdog;
		WORD										pidWatchdog;

		WORD										videoBitBufferThreshold;
		WORD										audioBitBufferThreshold;

		WORD										videoBitBufferFill;
		WORD										audioBitBufferFill;

		KernelInt64								lastIFramePosition;
		KernelInt64								lastGOPPosition;
		KernelInt64								lastSequenceHeaderPosition;
		KernelInt64								lastRestartPosition;

		BOOL										continuousAudioStreamMode;

		ASyncOutStream						*	strm;
		ByteOutputPort						*	audioPort;
		InterruptServer					*	irqServer;

		Line21Receiver						*	line21Receiver;
		BOOL										ccEnabled;
		StaticProfile						*	profile;
		WORD										digitalOutputEnabled;

		Error										ReadRegistryEntries (void);

	protected:
		DWORD irqmask;

		void SelectIBPBuffers (MPEG2FrameType type, int &rfp, int &ffp, int &bfp);
		void SelectDFPBuffer (MPEG2FrameType type, int &dfp);
		void ProgramIBPBuffers (int rfp, int ffp, int bfp);
		void ProgramDFPBuffer (int dfp);

		void ReadSCDCount (KernelInt64 &val);
		void LastSCDCount (KernelInt64 &val);
		void ReadCDCount (KernelInt64 &val);
		void LastCDCount (KernelInt64 &val);

		void ReadStatus (DWORD &status);

		DWORD	interruptGhost;

		void ReadInterruptStatus (DWORD &status);
		void WriteInterruptMask (DWORD mask);
		void SetResetInterruptMask (DWORD set, DWORD reset);
		void ClearInterruptStatus (DWORD mask);

		int		hoffset, voffset, panScanOffset;
		WORD		picLeft, picTop;
		WORD		frameWidth, frameHeight;
		WORD		picWidth, picHeight;
		WORD		frameAspectRatio, picAspectRatio;
		WORD		dfsShadow, enableOSDShadow;
		BOOL		freezeToOneField;
		int		downscaleFactor, downscaleHOffset, downscaleVOffset;
		int		downscaleHOffsetLetterboxed, downscaleVOffsetLetterboxed;

		WORD		activeLeft, activeTop, activeWidth, activeHeight;
		BOOL		widescreenContent;

		DWORD		lastVideoPTS;
		DWORD		lastAudioPTS;
		BOOL		noPTS;
		BOOL		audioStarted;
		DWORD		firstAudioPTS;

#if VIONA_VERSION
		ForcedAspectRatio forcedSourceAR;
		MPEG2PresentationMode presentationMode;
#endif

		Error RebuildDisplayRect(void);
		Error ProgramDisplayRect(WORD left, WORD top, WORD mwidth, WORD mheight, WORD width, WORD height, WORD mratio, WORD pixelRatio);
		Error ConfigureVideoBus(void);
#if VIONA_VERSION
		Error SetPanScanOffset(int offset);
#endif
		Error WriteQuantizerMatrix(BOOL nonIntra, BYTE __far * matrix);

#if VIONA_VERSION
		Error SetForcedSourceAspectRatio(ForcedAspectRatio forcedSourceAR);
		Error SetPresentationMode(MPEG2PresentationMode presentationMode);
#endif

		Error ProgramInitalFrameChipParameters(void);
		Error StartFrameDecoding(void);
		Error RestartFrameDecoding(void);
		Error StopFrameDecoding(void);
		Error StartFrameDisplay(void);
		Error CompleteFrameDisplay(void);
		Error SkipFrameDisplay(void);
		Error SendFakeHeader(void);
		Error BeginSeek(void);
		Error BeginCue(void);
		Error CompleteSeek(DWORD offset);
		Error InternalSeek(void);

		Error HeaderInterrupt(void);
		Error DSyncInterrupt(void);
		Error VSyncInterrupt(BOOL top);
		Error BitBufferFullInterrupt(void);

		Error BeginDecoding(void);		// initialize chip for decoding
		Error StartDecoding(void);		// start decoding
		Error StopDecoding(void);		// stop decoding
		Error EndDecoding(void);		// reset chip after decoding

		//
		// Forward the refill request
		//
		void RefillRequest(DWORD free);

		void Interrupt(void);

		DWORD	stateCnt;
		int	watchDog;
		int	scanWatchDog;
		int	adaptDelay;
		BOOL	framePending;
		BOOL	skipRequest;
		BOOL	freezeRequest;
		BOOL	lastSkipped;
		BOOL	skipTillHeader;
		BOOL	fakeHeaderRequest;
		BOOL	overfull;
		BOOL	initialParametersSent;
		BOOL	streamCompleted;
		BOOL	hasValidFrame;
		BOOL	bFrameOptimization;
		BOOL	closedGOP;
		BOOL	endCodeReceived;
		BOOL	sequenceHeaderFound;
		BOOL	oneSiblingBufferFull;

		long	frameNominator;
		long	frameDenominator;
		long	frameAccu;
		WORD	playbackSpeed;

		enum XState
			{
			xreset,											//  0
			xinit_waitForFirstPictureHeader,			//  1
			xinit_waitForBitBufferFull,				//  2
			xinit_waitForStartSync,						//  3
			xinit_waitForThirdFrame,					//  4
			xfrozen,											//  5
			xplaying,										//  6
			xplayingRepeat0,								//  7
			xplayingRepeat1,								//  8 
			xplayingRepeat2,								//  9
			xplayingRepeat3,								// 10
			xstopped,										// 11
			xseeking,										// 12
			xresync_waitForFirstPictureHeader,		// 13
			xresync_waitForBitBufferFull,				// 14
			xresync_waitForStartSync,					// 15
			xresync_waitForThirdFrame,					// 16
			xresync_prefreeze1,							// 17
			xresync_prefreeze2,							// 18
			xplaying_recover,								// 19
			xstepping,										// 20
			xstarving,										// 21
			xcueing,											// 22
			xresyncue_waitForFirstPictureHeader,	// 23
			xresyncue_waitForBitBufferFull,			// 24
			xresyncue_waitForStart,						// 25
			xstill,											// 26
			xplayingStill,									// 27
			xfreezingStill,								// 28
			xscan_waitForFirstPictureHeader,			// 29
			xscan_waitForBitBufferFull,				// 30
			xscan_waitForStartSync,						// 31
			xscan_decoding,								// 32
			xseek_waitForIdle								// 33
			} xstate;

		void IncrementState (XState &state) {(XState)(((int &)xstate)++);}

		DWORD GetPTS();
		void PutPTS(DWORD pos);
		Error ConformToPTS(DWORD pts);

		MPEG2CommandQueue	commandQueue;

		Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL CommandPending(DWORD tag);

		DWORD CurrentLocation(void);
		MPEGState CurrentState(void);
		DWORD LastTransferLocation(DWORD scale);

		DWORD SendData(HPTR data, DWORD size);
#if VIONA_VERSION
		MPEG2PESType NeedsPES(void) {return MP2PES_ELEMENTARY;}
		Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used);
#endif
		void CompleteData(void);

		virtual Error NotifySiblingBufferFull(void);

		BYTE	lastSendDataBytes[4];   // index 0 is the very last byte of previous SendData()

		VideoStandard	videoStandard;    
		BOOL mpeg2Coding;

		Error ReconfigureBuffers(void);

		Error SetSignalPosition(DWORD position);
		Error SetVideoBitrate(DWORD videoBitrate);
		Error SetAudioBitrate(DWORD audioBitrate);
		Error SetVideoWidth(WORD width);
		Error SetVideoHeight(WORD height);
		Error SetAspectRatio(WORD aspectRatio);
		Error SetVideoStandard(VideoStandard	standard);
		Error SetVideoFPS(WORD fps);
		Error SetMPEG2Coding(BOOL mpeg2Coding);

		BOOL	vblankServerEnabled;

		Error EnableVBlankServer(void);
		Error DisableVBlankServer(void);

		VirtualUnit	*	videoBus;

		Error Lock(VirtualUnit * unit);
		Error Unlock(VirtualUnit * unit);

#if VIONA_VERSION
	protected:
		DWORD	osdHeader[130];
		DWORD	osdHeaderBytes;
		WORD	osdNumColors;
		BOOL	osdValid, osdEnabled;
		WORD	osdX, osdY, osdW, osdH;
		WORD	osdPalette[256];
		BYTE	osdTransparency[256];
		BYTE	osdColors[256];
		DWORD	osdDwordsPerLine;
		DWORD	osdFirstBase, osdSecondBase;
		BYTE	*osdFirstField, *osdSecondField;
		VideoStandard osdVideoStandard;
		OSDStandardChangeHookHandle osdStandardChangeHook;

		Error WriteOSDHeader(void);
		Error CheckToSwitchOffOSD (VideoStandard newDecoderStandard);
		Error RedisplayOSD (void);

	public:
		Error KillOSDBitmap(void);
		Error SetOSDBitmap(WORD numColors,
								 BYTE __far * colorTable, BYTE __far * contrastTable,
								 WORD x, WORD y, WORD width, WORD height,
								 DWORD __huge * odd, DWORD __huge * even);

		Error SetOSDPalette(int entry, int y, int u, int v);

		Error ShowOSD(BOOL enable);

		Error CreateOSD (WORD numColors, VideoStandard vidStandard,
		                 BYTE __far * colorTable, BYTE __far * contrastTable,
		                 WORD x, WORD y, WORD width, WORD height, OSDStandardChangeHookPtr hook = NULL);

		Error BitBlitOSD (void *source, int sourceNumColors, int stride,
								int sx, int sy, int swidth, int sheight, int dx, int dy);
		Error SetRectOSD (int color, int x, int y, int width, int height);

		virtual Error SetCCEnabled(BOOL ccEnabled);
		Error SetLine21Receiver(Line21Receiver * line21Receiver);
#endif
	public:
		InterruptServer	*	vblankServer;
		PTSCaller	ptsCaller;

		STi5505MPEG2Decoder (MemoryMappedIO		*	port,
									ASyncOutStream		*	strm,
									MemoryMappedIO		*	subPicturePort,
									InterruptServer	*	irqServer);
		virtual ~STi5505MPEG2Decoder(void);

		Error Initialize (ByteOutputPort	*	audioPort,
								VirtualUnit		*	videoBus,
								BitOutputPort	*	widescreenActivePort,
								StaticProfile	*	profile);

		Error Configure(TAG __far * tags);

		Error ConfigureMemory(BYTE refreshInterval);
		BOOL CheckChipInitialisation(void);
		Error ConfigureBuffers(DWORD bitBufferSize,	// in bytes
		                       DWORD frameSize);		// in bytes
		Error SetBufferRatio(DWORD videoBitBuffer, DWORD audioBitBuffer); // in any unit.
		Error SoftReset(void);

		void	GetSubpictureBufferParameters(DWORD & offset, DWORD & size);

		BOOL	Line21TransferAllowed() {return (ccEnabled && (xstate > xfrozen) && (xstate < xstopped) && (playbackSpeed == 1000));}

		VirtualUnit * CreateVirtual(void);
	};



class VirtualSTi5505MPEG2Decoder : public VirtualMPEG2VideoDecoder
	{
	private:
		STi5505MPEG2Decoder	*	decoder;

		ForcedAspectRatio			forcedSourceAR;

	protected:
		Error PreemptStopPrevious(VirtualUnit * previous);
		Error PreemptChange(VirtualUnit * previous);
		Error PreemptStartNew(VirtualUnit * previous);

		Error GetDisplaySize(WORD __far &width, WORD __far &height);

		DWORD audioBitrate;

		BOOL						initial;

		BOOL						ccEnabled;

		Line21Receiver			*	line21Receiver;

		struct
			{
			WORD						width;
			WORD						height;
			BYTE						aspectRatio;
			WORD						aspectRatioFactor;
			BYTE						frameRate;
			DWORD						bitRate;
			DWORD						vbvBufferSize;
			MPEG2CodingStandard	codingStandard;
			BYTE						intraQuantMatrix[64];
			BYTE						nonIntraQuantMatrix[64];
			BYTE						chromaIntraQuantMatrix[64];
			BYTE						chromaNonIntraQuantMatrix[64];
			} parser;

	public:
		VirtualSTi5505MPEG2Decoder(STi5505MPEG2Decoder * unit);

#if VIONA_VERSION
		MPEG2PESType NeedsPES(void) {return MP2PES_ELEMENTARY;}
#endif

		Error Configure(TAG __far * tags);

		Error SetLine21Receiver(Line21Receiver * line21Receiver) {this->line21Receiver = line21Receiver; GNRAISE_OK;}
	};



#endif
