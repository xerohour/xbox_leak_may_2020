#ifndef STI3520A_H
#define STI3520A_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\intrctrl.h"
#include "library\general\asncstrm.h"
#include "..\..\clocks\generic\clocks.h"
#include "..\..\audio\generic\audiodac.h"
#if VIONA_VERSION
#include "..\generic\mp2spudc.h"
#endif

#include "..\generic\mp2eldec.h"
#include "..\generic\mp2vsprs.h"

enum STi3520AClockSource
	{ 
	S35CS_POWERDOWN,
	S35CS_AUDCLK	= 3,
	S35CS_MEMCLK	= 4,
	S35CS_PCMCLK	= 5,
	S35CS_PIXCLK	= 6,
	S35CS_EXTCLK	= 7
	};                          
	
enum STi3520AClock
	{            
	S35CL_AUXCLK,
	S35CL_AUDCLK,
	S35CL_MEMCLK,
	S35CL_PCMCLK,
	S35CL_PIXCLK,
	S35CL_VIDCLK
	};

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
		DWORD				streamPosition;
		DWORD				pts;
		int				centerVerticalOffset;
		int				centerHorizontalOffset;
				
		Error Init(MPEG2VideoHeaderParser	* params, DWORD streamPosition, DWORD pts);
	};

class FrameParameterQueue
	{	
	private:
		FrameParameters	*	params;
		WORD	num, first, last;
	public:
		FrameParameterQueue(WORD size);
		virtual ~FrameParameterQueue(void);
		
		Error Reset(void);
		Error InsertFrame(MPEG2VideoHeaderParser * parser, DWORD streamPosition, DWORD pts);
		Error MarkSkipped(void);
		BOOL IsSkipped(void);
		Error GetFrame(FrameParameters __far &params);
		Error NextFrame(void);
		WORD FrameAvail() {return (first <= last) ? (last - first) : (num + last - first);}
	};

class STi3520AMPEG2VideoHeaderParser : public MPEG2VideoHeaderParser
	{
	private:
		ByteIndexedInputPort	*	port;
		BOOL							odd;         
		BOOL							even;
		BYTE							second;
		Error WaitForFIFONonEmpty(void);
	protected:
		Error BeginParse(void);
		Error NextByte(void);
		Error EndParse(void);

		Error ParsePictureHeader(void);	
	public:
		BOOL	skipHeader;
		
		STi3520AMPEG2VideoHeaderParser(ByteIndexedInputPort	*	port);
	};

class STi3520AMPEG2Decoder;
		
class STi3520AMPEG2Decoder : public MPEG2VideoDecoder, protected InterruptHandler, protected ASyncRefillRequest
#if VIONA_VERSION
                             ,public OSDDisplay
#endif                             
	{
	friend class STi3520AAudioStrm;
	friend class VirtualSTi3520AMPEG2Decoder;
	friend class STi3520AVBlankIntServer;
	friend class STi3520AMPEGAudioDecoder;
	friend class STi3520AAudioClockGenerator;

	protected:
		//
		// Basic IO operations
		//
		ByteIndexedInOutPort	*	port;
		
		Error OutByte(int idx, BYTE val);                             
		Error InByte(int idx, BYTE __far&val);

		//
		// to avoid double index programming
		//
		Error OutWordHL(int idx, WORD val);
		Error OutWordLH(int idx, WORD val);
		Error OutWordHL0(int idx, WORD val);
		Error InWordHL(int idx, WORD __far& val);		
		
		Error WriteMem(DWORD at, DWORD high, DWORD low);
		Error WriteMemBlock(DWORD at, DWORD num, const DWORD __far * data);
		Error ReadMem(DWORD at, DWORD __far & high, DWORD __far & low);
		
		ProgrammableClockGenerator	*	audioClock;
	
		class STi3520AAudioStrm * audioStrm;
		
		DWORD SendAudioData(HPTR data, DWORD size);
		DWORD AvailAudioData(void);
		DWORD AvailAudioBuffer(void);		
		
		int	lastIPBuffer, curIPBuffer, curDBuffer;
		DWORD	lastTempReference;
		
		WORD 					IBPBuffers[4];
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
		
		DWORD	signalPosition;
		DWORD	predFrameStreamPos;
		DWORD	currentFrameStreamPos;
		DWORD	fvcoFreq;
		
		DWORD	scdcnt; 
		DWORD	cdcnt;                          
		DWORD	cdoffset;

		BOOL	prevOdd;
		BOOL	currentOdd;
				
		DWORD frameCnt;
		
		DWORD streamPosition;
		DWORD streamOffset;
		
		FrameParameterQueue					frames;  
		STi3520AMPEG2VideoHeaderParser   parser;
		PTSAssoc									ptsAssoc;
		long										conformPTS;
		BOOL										conformPTSValid;
		
		WORD										videoBitBufferSize;
		WORD										audioBitBufferSize;
		WORD										bitBufferSize;
		
		DWORD										videoBitrate;
		DWORD										audioBitrate;
		
		DWORD										frameSize;
		WORD										osdBase, osdBaseX;
		DWORD										osdSize, osdSizeX;
		BOOL										osdLimitation;
		
		WORD										bbfWatchdog;
		WORD										predVBL;
		WORD										bbeWatchdog;
		WORD										pidWatchdog;
		
		WORD										videoBitBufferThreshold;
		
		WORD										videoBitBufferFill;
		WORD										audioBitBufferFill;
		
		DWORD										lastIFramePosition;
		DWORD										lastGOPPosition;
		DWORD										lastSequenceHeaderPosition;
		DWORD										lastRestartPosition;

		BOOL										continuousAudioStreamMode;
		
		ASyncOutStream						*	strm;
		ByteOutputPort						*	audioPort;
		InterruptServer					*	irqServer;
	protected:                      
		DWORD irqmask;
		                      
		Error SelectIBPBuffers(MPEG2FrameType type, int __far& rfp, int __far& ffp, int __far& bfp);
		Error SelectDFPBuffer(MPEG2FrameType type, int __far& dfp);
		Error ProgramIBPBuffers(int rfp, int ffp, int bfp);                         
		Error ProgramDFPBuffer(int dfp);
		
		Error ReadSCDCount(DWORD __far&val);
		Error LastSCDCount(DWORD __far&val);
		Error ReadCDCount(DWORD __far&val);
		Error LastCDCount(DWORD __far&val);
		
		Error ReadStatus(DWORD __far&status);
		
		DWORD	interruptGhost;
		
		Error ReadInterruptStatus(DWORD __far&status);
		Error WriteInterruptMask(DWORD mask);
		Error SetResetInterruptMask(DWORD set, DWORD reset);
		Error ClearInterruptStatus(DWORD mask);
				
		int		hoffset, voffset, panScanOffset;
		WORD		picLeft, picTop;
		WORD		frameWidth, frameHeight;
		WORD		picWidth, picHeight;
		WORD		frameAspectRatio, picAspectRatio;
		
		WORD		activeLeft, activeTop, activeWidth, activeHeight;
		
#if VIONA_VERSION
		MPEG2PresentationMode presentationMode;
#endif		
		
		Error RebuildDisplayRect(void);		
		Error ProgramDisplayRect(WORD left, WORD top, WORD mwidth, WORD mheight, WORD width, WORD height, WORD mratio, WORD ratio);
		Error ConfigureVideoBus(void);
#if VIONA_VERSION		
		Error SetPanScanOffset(int offset);
#endif		
		Error WriteQuantizerMatrix(BOOL nonIntra, BYTE __far * matrix);

#if VIONA_VERSION		
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
		BOOL	overfull;
		BOOL	initialParametersSent;
		BOOL	streamCompleted;
		BOOL	hasValidFrame;
		BOOL	bFrameOptimization;
		BOOL	closedGOP;
		BOOL	endCodeReceived;
		BOOL	sequenceHeaderFound;
				
		long	frameNominator;
		long	frameDenominator;
		long	frameAccu;
		WORD	playbackSpeed;
								
		enum XState
			{
			xreset,											//  0
			xinit_waitForFirstPictureHeader,    	//  1
			xinit_waitForBitBufferFull,         	//  2
			xinit_waitForStartSync,             	//  3
			xinit_waitForThirdFrame,            	//  4
			xfrozen,                            	//  5
			xplaying,                           	//  6
			xplayingRepeat0,								//  7
			xplayingRepeat1,                    	//  8 
			xplayingRepeat2,                    	//  9
			xplayingRepeat3,                    	// 10
			xstopped,                           	// 11
			xseeking,                           	// 12
			xresync_waitForFirstPictureHeader,  	// 13
			xresync_waitForBitBufferFull,       	// 14
			xresync_waitForStartSync,           	// 15
			xresync_waitForThirdFrame,          	// 16
			xresync_prefreeze1,							//	17
			xresync_prefreeze2,							//	18
			xplaying_recover,                   	// 19
			xstepping,                          	// 20
			xstarving,										// 21
			xcueing,											// 22
			xresyncue_waitForFirstPictureHeader,	// 23
			xresyncue_waitForBitBufferFull,			// 24
			xresyncue_waitForStart,						// 25
			xstill,		      							// 26
			xplayingStill,									// 27
			xscan_waitForFirstPictureHeader,			// 28
			xscan_waitForBitBufferFull,				// 29
			xscan_waitForStartSync,						// 30
			xscan_decoding									// 31
			} xstate;
			
	   friend XState operator ++(XState __far& x, int a) {return (XState)(((int __far&)x)++);}
	   
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
		MPEG2PESType NeedsPES(void) {return dvdStreamEncrypted ? MP2PES_DVD : MP2PES_ELEMENTARY;}
		Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used);
#endif
	   void CompleteData(void);
	
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
		DWORD osdHeader[10];
		WORD	osdNumColors;
		BOOL	osdValid, osdEnabled, osdHighColor;
		WORD	osdX, osdY, osdW, osdH;
		WORD	osdPalette[16];
		BYTE	osdTransparency[16];
		BYTE	osdColors[16];
		DWORD	osdHeight, osdWidth;
				
		Error WriteOSDHeader(void);
		
	public:
		Error KillOSDBitmap(void);     
	   Error SetOSDBitmap(WORD numColors,
	   					    BYTE __far * colorTable, BYTE __far * contrastTable,
	                      WORD x, WORD y, WORD width, WORD height,
	                      DWORD __huge * odd, DWORD __huge * even);
	
		Error SetOSDPalette(int entry, int y, int u, int v);	                                   
		
		Error ShowOSD(BOOL enable);
#endif		
	public:
		InterruptServer	*	vblankServer;
		PTSCaller	ptsCaller;
		
		STi3520AMPEG2Decoder(ByteIndexedInOutPort 		* 	port,
		                     ASyncOutStream       		* 	strm,
		                     InterruptServer				*	irqServer,
		                     ByteOutputPort					*	audioPort,
		                     VirtualUnit						*	videoBus = NULL,
		                     ProgrammableClockGenerator * 	audioClock = NULL,
		                     BOOL									continuousAudioStreamMode = FALSE);
		~STi3520AMPEG2Decoder(void);
		
		Error Configure(TAG __far * tags);
		
		Error InitClockGenerator(STi3520AClockSource source, WORD nominator, WORD denominator, DWORD freq);
		Error ProgramClockGenerator(STi3520AClock clock, BOOL external, DWORD freq);
		Error ConfigureMemory(BYTE busWidth, 
									 BOOL preventBitBufferOverflow,
									 BOOL enable,
									 BOOL	segmentsPerRow,
									 BOOL	edoRam,
									 BOOL	sdRam,
									 BOOL	meg20,
									 BYTE refreshInterval);
		BOOL CheckChipInitialisation(void);
		Error ConfigureBuffers(DWORD bitBufferSize,	// in bytes
		                       DWORD frameSize);				// in bytes
		Error SetBufferRatio(DWORD videoBitBuffer, DWORD audioBitBuffer); // in any unit.		                       
		Error SoftReset(void);											 
		
		VirtualUnit * CreateVirtual(void);		

		ASyncOutStream	*	BuildAudioStream();
		Error InitializeAudioStream(void);
   };

class VirtualSTi3520AMPEG2Decoder : public VirtualMPEG2VideoDecoder
	{
	private:
		STi3520AMPEG2Decoder	*	decoder;
	protected:                                
      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);

		Error GetDisplaySize(WORD __far &width, WORD __far &height);
		
		DWORD audioBitrate;

		BOOL						initial;		

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
		VirtualSTi3520AMPEG2Decoder(STi3520AMPEG2Decoder * unit);

#if VIONA_VERSION
		MPEG2PESType NeedsPES(void) {return dvdStreamEncrypted ? MP2PES_DVD : MP2PES_ELEMENTARY;}
#endif
		
		Error Configure(TAG __far * tags);
	};

class STi3520AMPEGAudioDecoder : public MPEG2AudioDecoder, 
											protected ASyncRefillRequest,
											protected InterruptHandler
	{
	friend class VirtualSTi3520AMPEGAudioDecoder;
	private:
		STi3520AMPEG2Decoder * decoder;
	protected:                                              
		VirtualAudioDAC	*	pcmDAC;
		ASyncOutStream		*	strm;
		MPEGState				state;
		DWORD						audioBitrate;
		WORD						sampleRate;

		BOOL						mute;
		WORD						leftVolume, rightVolume;
		
		DWORD						streamPosition;

		DWORD						dataNominator, dataDenominator;
		
#if VIONA_VERSION
		BOOL						lpcm;
		WORD						bitsPerSample;
		WORD						channels;
		
		BYTE						frameBuffer[320];
		WORD						framePosition;
		WORD						bytePosition;
		WORD						bytePositionWrap;
		DWORD						bytePositionMask;
		enum {LPS_FILLING, LPS_SENDING} lpcmState;
		
		Error ResetFrameBuffer(void);
		DWORD SendLPCMData(HPTR data, DWORD size);		
#endif
		
		DWORD						predPosition;
		DWORD						signalPosition;

		DWORD						stepCnt;

		PTSAssoc					ptsAssoc;

		PTSCounter				ptsCounter;
		
		VirtualUnit			*	audioBus;   
		
		Error SetAudioBitrate(DWORD audioBitrate);
		Error SetSampleRate(WORD rate); 
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);
#if VIONA_VERSION
		Error SetLPCM(BOOL lpcm);
		Error SetBitsPerSample(WORD samples);
		Error SetChannels(WORD channels);
#endif		

		Error ProgramAudioFrequency(DWORD freq);

	   DWORD SendData(HPTR data, DWORD size);
	   void CompleteData(void);

	   Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL CommandPending(DWORD tag);
			
		DWORD CurrentLocation(void);
		MPEGState CurrentState(void);
		DWORD LastTransferLocation(DWORD scale);
			   
	   void PutPTS(DWORD pts);
	   DWORD GetPTS();
	   
	   Error SetSignalPosition(DWORD position);

		Error StopDecoding(void);
		Error StartDecoding(void);

		void RefillRequest(DWORD free);
		
		InterruptServer * vblank;
		
		void Interrupt(void);

		VirtualUnit * CreateVirtual(void);
	public:
		STi3520AMPEGAudioDecoder(STi3520AMPEG2Decoder * decoder,
							          VirtualAudioDAC      *	dac,
							          InterruptServer		 * vblank,
							          VirtualUnit			 * audioBus = NULL,
							          ASyncOutStream		 * stream = NULL);

	};

class VirtualSTi3520AMPEGAudioDecoder : public VirtualMPEG2AudioDecoder
	{
	private:
		STi3520AMPEGAudioDecoder * decoder;

      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);
	public:
		VirtualSTi3520AMPEGAudioDecoder(STi3520AMPEGAudioDecoder * unit)
			: VirtualMPEG2AudioDecoder(unit) {decoder = unit; audioMute = FALSE;}
	};

#endif
