
// FILE:      library\hardware\mpeg2dec\specific\sti7000.h
// AUTHOR:    U. Sigmund, M. Stephan, C. Scherer
// COPYRIGHT: (c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   
//
// PURPOSE:   STi 7000 MPEG-2 HDTV decoder chip --- header file 
//
// HISTORY:

#ifndef STI7000_H
#define STI7000_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\profiles.h"
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

#define NOT_IMPLEMENTED_YET	0

class FrameParameters
	{	
	public:
		BYTE				pfh, pfv, ppr1, ppr2;
		
		MPEG2FrameType				frameType;		
		MPEG2PictureStructure   fieldType;
		
		BOOL				repeatFirstField;
		BOOL				topFieldFirst;
		BOOL				progressiveFrame;
		BOOL				skipped;
		BOOL				enabled;
		DWORD				temporalReference;
		DWORD				streamPosition;
		DWORD				pts;
		int				centerVerticalOffset;
		int				centerHorizontalOffset;
		BYTE				intraQuant[64];
		BYTE				nonIntraQuant[64];
				
		Error Init(MPEG2VideoHeaderParser	* params, DWORD streamPosition, DWORD pts, BYTE * intraQuant, BYTE * nonIntraQuant);
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
		Error InsertFrame(MPEG2VideoHeaderParser * parser, DWORD streamPosition, DWORD pts, BYTE * intraQuant, BYTE * nonIntraQuant);
		Error MarkSkipped(void);
		BOOL IsSkipped(void);
		Error GetFrame(FrameParameters __far &params);
		Error GetNextFrame(FrameParameters __far &params);
		Error NextFrame(void);
		WORD FrameAvail() {return (first <= last) ? (last - first) : (num + last - first);}
	};

class STi7000MPEG2VideoHeaderParser : public MPEG2VideoHeaderParser
	{
	private:
		ByteIndexedInputPort	*	port;
		BOOL							odd;         
		BOOL							even;
		BYTE							second;
		Error WaitForFIFONonEmpty(WORD timeOut);
		
	protected:
		Error BeginParse(void);
		Error NextByte(void);
		Error EndParse(void);

		Error ParsePictureHeader(void);	
	public:
		BOOL	skipHeader;
		
		STi7000MPEG2VideoHeaderParser(ByteIndexedInputPort	*	port);
	};

class STi7000MPEG2Decoder;
		
class STi7000MPEG2Decoder : public MPEG2VideoDecoder, protected InterruptHandler, protected ASyncRefillRequest
#if VIONA_VERSION
                             ,public OSDDisplay
#endif                             
	{
	friend class VirtualSTi7000MPEG2Decoder;
	friend class STi7000VBlankIntServer;

	private: 
		
		void HaltOnError(Error error);
		Error SetPowerDownMode(void);
		Error ResetPowerDownMode(void);

	protected:
        Profile * profile;
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
		
		Error OutDWordHL(int idx, DWORD val);
		Error OutDWordLH(int idx, DWORD val);
		Error OutDWordHL0(int idx, DWORD val);
		Error InDWordHL(int idx, DWORD __far& val);
		
		Error SetBitB(int adr, int pos, BOOL val);
		Error SetBitB(int adr, int pos, int num, BYTE val);		
		Error SetBitW(int adr, int pos, BOOL val);
		Error SetBitW(int adr, int pos, int num, WORD val);		
		Error SetBitD(int adr, int pos, BOOL val);
		Error SetBitD(int adr, int pos, int num, DWORD val);		

		Error WriteMem(DWORD at, DWORD high, DWORD low);
		Error WriteMemBlock(DWORD at, DWORD num, const DWORD __far * data);
		Error ReadMem(DWORD at, DWORD __far & high, DWORD __far & low);
		Error WriteMemWord(DWORD at, DWORD num, const WORD __far * data);
		
		ProgrammableClockGenerator	*	audioClock;
	
		int	lastIPBuffer, curIPBuffer, curDBuffer, curBBuffer, lastBBuffer, delayedDBuffer;
		MPEG2FrameType	previousFrameType;
		DWORD	lastTempReference;
		
		WORD 					IBPBuffersY[4], IBPBuffersUV[4];
		DWORD					bufferStreamPos[4];
		BOOL					bufferValid[4];
		FrameParameters	bufferParams[4];
		FrameParameters	currentDisplayParams;
		FrameParameters	currentDecodingParams;
		FrameParameters	currentParams;
		FrameParameters	lastParams;
		
		PTSCounter			ptsCounter;
		
		DWORD					currentPTS;
		DWORD					lastDecodePTS;
//		DWORD					ptsTimeOffset;
//		DWORD					ptsStopTimeOffset;
		
		DWORD	signalPosition;
		DWORD	predFrameStreamPos;
		DWORD	currentFrameStreamPos;
		DWORD	fvcoFreq;
		
		DWORD	cdcnt;		// 32 bit counter into bit buffer                         
		DWORD	scdcnt;		// 32 bit counter into header FIFO
		DWORD	cdoffset;

		BOOL	prevOdd;
		BOOL	currentOdd;
		BOOL	currentTopFirst;
		BOOL	progressiveScan;
		BOOL	doubleFrame;
		BOOL	missingInterrupt;
		BOOL	freezeFrameData;
      BOOL  avSyncEnable; 
      BOOL  osdEnable; 
      BOOL  statusPrintEnable;
      BOOL  ptsPrintEnable;
		DWORD bufferLevel;
				
		DWORD frameCnt;
		DWORD statusPrints;
		
		//just for catching the command pending deadlock!!!!
		int	tempInterruptCounter; 
		
		DWORD streamPosition;
		DWORD streamOffset;
		
		FrameParameterQueue					frames;
		MPEG2FrameType							curFrame;  
		MPEG2FrameType							lastFrame;  
		STi7000MPEG2VideoHeaderParser		parser;
		PTSAssoc									ptsAssoc;
		DWORD										last_vidpts;
		DWORD										last_audpts;
		long										conformPTS;
		BOOL										conformPTSValid;
		
		WORD										bitBufferSize;
		
		DWORD										frameSize;
		DWORD										totalFrameSize;
		DWORD										osdBase, osdBaseX;
		DWORD										osdSize, osdSizeX;
		BOOL										osdLimitation;
		
		WORD										bbfWatchdog;
		WORD										predVBL;
		WORD										bbeWatchdog;
		WORD										pidWatchdog;
		WORD										skipWatchdog;
		WORD										freezeWatchdog;
		int										decodeWatchdog;
		
		WORD										bitBufferThreshold;
		
		WORD										bitBufferFill;
		
		DWORD										lastIFramePosition;
		DWORD										lastGOPPosition;
		DWORD										lastSequenceHeaderPosition;
		DWORD										lastRestartPosition;
		DWORD										doc;	// data output counter
		DWORD										next_doc;
		
		BYTE										cnt;

		ASyncOutStream						*	strm;
		InterruptServer					*	irqServer;
//	protected:                      
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
		DWORD its;
		
		Error ReadInterruptStatus(DWORD __far&status);
		Error ReadInterruptMask(DWORD __far&status);
		Error InitInterruptMask(DWORD mask);
		Error WriteInterruptMask(DWORD mask);
		Error SetInterruptMask(DWORD set);
		Error ResetInterruptMask(DWORD reset);
		Error SetIrqMaskReg(DWORD set);
		Error ResetIrqMaskReg(DWORD reset);
		Error ClearInterruptStatus(DWORD mask);
				
#if WDM_VERSION
		//following variables only valid for 32bit-version
		KTIMER	interruptWatchDogTimer;				// timer that checks the existence of interrupts
		KDPC		interruptWatchDogDPC;
		LARGE_INTEGER	intErrorTime;
#endif
		
		int			hoffset, voffset, panScanOffset;
//		WORD			picLeft, picTop;
		WORD			frameWidth, frameHeight;
//		WORD			picWidth, picHeight;
		WORD			frameAspectRatio;
		VideoMode	displayMode;
		BYTE			frameRate;
		BOOL			progressiveSequence;
		WORD			activeLeft, activeTop, activeWidth, activeHeight;
		
#if VIONA_VERSION
		MPEG2PresentationMode presentationMode;
#endif		
		
		Error RebuildDisplayRect(void);
		
		int display_pixels, picture_pixels, blank_pixels, screen_pixels, sync_pixels, outside_pixels;
		int display_lines, picture_lines, blank_lines, screen_lines, sync_lines, outside_lines;
		BOOL progressive_input, progressive_output;
				
		Error ProgramDisplayRect(WORD fwidth, WORD fheight, WORD fratio, BYTE frate, BOOL progressive);
		Error ConfigureVideoBus(void);
#if VIONA_VERSION		
		Error SetPanScanOffset(int offset);
#endif		
		Error WriteQuantizerMatrix(BOOL nonIntra, BYTE __far * matrix);
		Error SetDisplayMode(VideoMode videoMode);

#if VIONA_VERSION		
        Error SetPresentationMode(MPEG2PresentationMode presentationMode);
        Error ReadRegistryEntries(void);
#endif		
		
		Error ProgramInitalFrameChipParameters(void);
		Error StartFrameDecoding(void);
		Error SkipFrameDecoding(void);
		Error StopFrameDecoding(void);
		Error StartFrameDisplay(void);
//		Error SkipFrameDisplay(void);
		Error CompleteFrameDisplay(void);
		Error SendFakeHeader(void);
		Error BeginSeek(void);
		Error BeginCue(void);
		Error CompleteSeek(DWORD offset);
		Error InternalSeek(void);
		
		Error HeaderInterrupt(void);
		Error DSyncInterrupt(void);
		Error VSyncInterrupt(BOOL top);
		Error BitBufferFullInterrupt(void);
		void  SemanticOrSyntaxErrorInterrupt(DWORD interruptStatus);

		Error BeginDecoding(void);		// initialize chip for decoding
		Error StartDecoding(void);		// start decoding
		Error RestartDecoding(void);	// restart decoding after soft reset
		Error StopDecoding(void);		// stop decoding
		Error EndDecoding(void);		// reset chip after decoding
				
		//
		// Forward the refill request
		//
		void RefillRequest(DWORD free);

		void Interrupt(void);                       
		
		DWORD	stateCnt;
		int   startCodeHitWatchDog; 	// watch dog for start code hit interrupt
		int	headerIntWatchDog;		// watch dog for frame decoding 
		int	watchDog;
		int	scanWatchDog;
		int	adaptDelay;
		BOOL	framePending;
		BOOL	startDecodeTriggered;	// TRUE - last interrupt was dsync ---> next one should be a start code hit interrupt
												// FALSE - dsync interrupt not detected (set to false if a start code hit was detected)
		BOOL	skipRequest;      
		BOOL	freezeRequest;
		BOOL	lastSkipped;                          
		BOOL	skipTillHeader;
		BOOL	execute;
		BOOL	decodeNext;
		BOOL	overfull;
		BOOL	initialParametersSent;
		BOOL	streamCompleted;
		BOOL	hasValidFrame;
		BOOL	bFrameOptimization;
		BOOL	closedGOP;
		BOOL	endCodeReceived;
		BOOL	sequenceHeaderFound;
		int	notIdleVSyncCounter;
		BOOL	decodePending;
		BOOL	decodeError;
		BOOL	doubleShot;
		
		WORD	currentFrameHeight;
		WORD	currentFrameWidth;
		WORD	currentFrameRate;
		WORD	currentAspectRatio;
				
		long	frameNominator;
		long	frameDenominator;
		long	frameAccu;
		WORD	playbackSpeed;
								
		enum XState
			{
			xreset,                                //  0
			xinit_waitForFirstPictureHeader,       //  1
			xinit_waitForBitBufferFull,            //  2
			xinit_waitForStartSync,                //  3
			xinit_waitForThirdFrame,               //  4
			xfrozen,                               //  5
			xplaying,                              //  6
			xplayingRepeat0,                       //  7
			xplayingRepeat1,                       //  8 
			xplayingRepeat2,                       //  9
			xplayingRepeat3,                       // 10
			xstopped,                              // 11
			xseeking,                              // 12
			xresync_waitForFirstPictureHeader,     // 13
			xresync_waitForBitBufferFull,          // 14
			xresync_waitForStartSync,              // 15
			xresync_waitForThirdFrame,             // 16
			xresync_prefreeze1,                    // 17
			xresync_prefreeze2,                    // 18
			xplaying_recover,                      // 19
			xstepping,                             // 20
			xstarving,                             // 21
			xcueing,                               // 22
			xresyncue_waitForFirstPictureHeader,   // 23
			xresyncue_waitForBitBufferFull,        // 24
			xresyncue_waitForStart,                // 25
			xstill,                                // 26
			xplayingStill,                         // 27
			xscan_waitForFirstPictureHeader,       // 28
			xscan_waitForBitBufferFull,            // 29
			xscan_waitForStartSync,                // 30
			xscan_decoding,                        // 31
			xdecodeError,                          // 32
			xfrozen_waitForBitBufferFull				// 33
			} xstate;
		
		XState lastxstate;

	   friend XState operator ++(XState __far& x, int a) {return (XState)(((int __far&)x)++);}
	   
	   DWORD GetPTS(void);
	   void PutPTS(DWORD pts);
	   Error ConformToPTS(DWORD pts);

		Error SetPlaybackTime(LONG time);
      
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
		WORD  osdHeader[32];
		WORD	osdNumColors;
		BOOL	osdValid, osdEnabled, osdHighColor;
		WORD	osdX, osdY, osdW, osdH;
		WORD	osdPalette[16];
		BYTE	osdTransparency[16];
		BYTE	osdColors[16];
		DWORD	osdHeight, osdWidth;
		
		WORD	rowStart;
		WORD	rowStop;
		WORD	columnStart;
		WORD	columnStop;
		BOOL	trueColor;
		BOOL	useExistPalet;
		WORD	bytesPerLine;
		BOOL	highPalet;
		BOOL	lineDoubling;
		BYTE	resMode;
		BYTE	mixWeight;
		BOOL	compressPel;
		BYTE	runLength;
		DWORD	curHeaderPtr;
		DWORD	nextHeadPtr;
		DWORD	osdBotDaPtr;
		DWORD	osdTopDaPtr;
		BOOL	T[16];
		BOOL	B[16];
		BYTE	Y[16];
		BYTE	Cb[16];
		BYTE	Cr[16];
		
		
				
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
		
		STi7000MPEG2Decoder(ByteIndexedInOutPort 	* 	port,
		                    ASyncOutStream       	* 	strm,
		                    InterruptServer			*	irqServer,
		                    VirtualUnit				*	videoBus = NULL,
                            Profile				    *	profile = NULL);
		~STi7000MPEG2Decoder(void);
		
		Error Configure(TAG __far * tags);
      
      Error Initialize(void);
		Error Uninitialize(void);
		Error OSDtestPattern(void);
      
#if NOT_IMPLEMENTED_YET		
		Error InitClockGenerator(STi7000ClockSource source, WORD nominator, WORD denominator, DWORD freq);
		Error ProgramClockGenerator(STi7000Clock clock, BOOL external, DWORD freq);
		Error ConfigureMemory(BYTE busWidth, 
									 BOOL preventBitBufferOverflow,
									 BOOL enable,
									 BOOL	segmentsPerRow,
									 BOOL	edoRam,
									 BOOL	sdRam,
									 BOOL	meg20,
									 BYTE refreshInterval);
#endif
		BOOL CheckChipInitialisation(void);
		Error ConfigureBuffers(void);
		Error SoftReset(void);											 
		Error InterruptWatchDogRoutine(void);
		
		VirtualUnit * CreateVirtual(void);		
   };

#if WDM_VERSION
void	InterruptWatchDogCallback(PKDPC deferredProcCall, PVOID deferredContext, PVOID sysArgFirst, PVOID sysArgSecond);		
#endif

class VirtualSTi7000MPEG2Decoder : public VirtualMPEG2VideoDecoder
	{
	private:
		STi7000MPEG2Decoder	*	decoder;
	protected:                                
      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);

		Error GetDisplaySize(WORD __far &width, WORD __far &height);
		
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

		VideoMode	displayMode;
		
	public:
		VirtualSTi7000MPEG2Decoder(STi7000MPEG2Decoder * unit);

#if VIONA_VERSION
		MPEG2PESType NeedsPES(void) {return MP2PES_ELEMENTARY;}
#endif
		
		Error Configure(TAG __far * tags);
	};

#endif
