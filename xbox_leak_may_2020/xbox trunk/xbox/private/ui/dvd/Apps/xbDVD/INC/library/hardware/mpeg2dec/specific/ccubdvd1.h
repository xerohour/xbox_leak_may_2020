#ifndef CCUBDVD1_H
#define CCUBDVD1_H

#include "library\common\krnlsync.h"
#include "library\lowlevel\intrctrl.h"
#include "library\common\krnlstr.h"
#include "library\hardware\mpeg2dec\generic\mpeg2dec.h"
#include "library\hardware\clocks\generic\clocks.h"
#include "library\hardware\audio\generic\audiodac.h"
#include "library\hardware\video\generic\ccapvenc.h"

#if ZIVA_PC_DECODER
class CCubeDVD1Decoder : public MPEG2AVDecoder, protected InterruptHandler, protected VDCriticalSection, protected VDIRQSafeSpinLock, protected ASyncRefillRequest
#else
class CCubeDVD1Decoder : public MPEG2AVDecoder, protected InterruptHandler, protected VDCriticalSection, protected VDSpinLock, protected ASyncRefillRequest
#endif
	{
	friend class VirtualCCubeDVD1Decoder;
	protected:
		//
		// Configuration parameters
		//
		ByteIndexedInOutPort			*	port;
		ByteOutputPort					*	codePort;
		InterruptServer				*	intServer;
		DualASyncOutStream			*	strm;
		ProgrammableClockGenerator	*	audioClock;
		VirtualAudioDAC				*	dac;
		VirtualUnit						*	videoBus;
		VirtualUnit						*	audioBus;
		MPEG2DVDDecryption			*	decryption;
		BitOutputPort					*	resetPort;
		
		DWORD									ccpTimeoutCounter;

		BOOL									progressive;
#if 0
		BOOL									progressiveMaterial, wasProgressiveMaterial;
		BYTE									progressiveHistory;
#endif

		BOOL									ccEnabled;
		
		BOOL									dvdDemux;
		BOOL									streamEncrypted;

		ForcedAspectRatio					forcedSourceAR;

		SPDIFHandling						spdifHandling;
		LONG									spdifDisableMethod;
		LONG									alwaysDisableSPDIF;
		LONG									alwaysMuteDAC;

		DWORD									uCodeRevision;
		DWORD									siliconRevision;

		WORD									pcmClockDivider;
		
		DWORD									changed;
		
		DWORD									spuPalette[16];
		
		BOOL									mute;
		WORD									leftVolume, rightVolume;
		DWORD									audioSampleRate;
		
		BOOL									ac3, lpcm;

		enum AudioType {AT_LPCM, AT_AC3, AT_MPEG} audioType;
		
		VideoStandard						videoStandard;
		VideoStandard						currentVideoStandard;
		MPEG2PresentationMode			presentationMode;
		
		BYTE									videoStreamID, audioStreamID, ac3StreamID, lpcmStreamID, spuStreamID, dtsStreamID;
		BOOL									spuEnable;
		
		BOOL									highlightValid;
		BOOL									highlightUpdateRequest;
		BOOL									subPicPaletteRequest;
				
		SPUButtonState						buttonState, updateButtonState;
		WORD									buttonID, updateButtonID;
		DWORD									buttonSelectedColor, buttonActiveColor;
		DWORD									updateButtonSelectedColor, updateButtonActiveColor;
		int									buttonLeft, buttonTop, buttonWidth, buttonHeight;
		int									updateButtonLeft, updateButtonTop, updateButtonWidth, updateButtonHeight;
		
		WORD									lastAudioStreamID;
		WORD									lastSPUStreamID;
		WORD									lastVideoStreamID;
		
		DWORD									transferLocation;
		DWORD									transferLocationLow;
		DWORD									videoFilled, audioFilled, lastFilled;
		DWORD									finalLocation;
		
		DWORD									signalPosition, predPosition;
		
		DWORD									audioBitBufferSize, videoBitBufferSize;
		
		WORD									width, height, fps, aspectRatio;
		
      MPEG2CommandQueue					commandQueue;
      
  		BOOL									initialAuthenticate;
  		BOOL									diskKeyValid;
		BOOL									titleKeyValid;
		DWORD									titleKeySector;
		DWORD									authAddr; 
		
		DWORD									shadow160;
		MPEGStreamType 					streamType;
		BOOL									streamMode;
		
		LONG									lastPBTime;
		LONG									lastSTCTime;

#if 0 // WDM_VERSION && _DEBUG
		DWORD rrCount;
		DWORD oldRRCount;
		DWORD	oldRRTime;
#endif


		VirtualCloseCaptionVideoEncoder	*	ccenc;

#if WDM_VERSION
		MPEG2CCPacketHookHandle			ccPacketHook;

		DWORD	ac3OpMode;
		DWORD	ac3LowBoost;
		DWORD ac3HighCut;
		DWORD	pcmScaleFactor;
		DWORD	ac3VoiceSelect;
		
		BOOL	wdmPlayback;

		DWORD	buttonStartTime;
		DWORD	buttonEndTime;
		DWORD	updateButtonStartTime;
		DWORD	updateButtonEndTime;
#endif
		Profile	*	profile;

		Error CompleteCommand(DWORD tag);

		Error	OutDWord(DWORD at, DWORD data);
		Error	OutDWords(DWORD at, DWORD __far * data, DWORD num);
#if 0	//_DEBUG
		Error	OutDWordsXX(DWORD at, DWORD __far * data, DWORD num);
#endif
		Error OutBEDWord(DWORD at, DWORD data);
		Error	OutBEDWords(DWORD at, DWORD __far * data, DWORD num);
		Error InDWordNoLock(DWORD at, DWORD __far & data);
		Error	InDWord(DWORD at, DWORD __far & data);
		Error	InDWordSafe(DWORD at, DWORD __far & data);
		Error	InDWords(DWORD at, DWORD __far * data, DWORD num);
		Error ModifyDWord(DWORD at, DWORD mask, DWORD data);          
		
		Error ReloadConfigurationData(void);
		
		Error	CompleteHighPriorityCommand(DWORD command);
		Error ForceChipCommandExecution(void);
		Error ExecuteChipCommand(DWORD __far * cmd, WORD num);
		BOOL	ChipCommandPending(void);		
		Error	CompleteChipCommand(void);
		
		Error CmdAbort(BOOL flush);
		Error CmdFade(WORD level, WORD fadeTime);
		Error CmdFlushBuffer(WORD bufferID);
		Error CmdFreeze(WORD displayMode);
		Error CmdHighLight(WORD button, WORD action);
		Error CmdHighLight2(WORD contrast, WORD color, WORD left, WORD top, WORD width, WORD height);
		Error CmdNewPlayMode(void);
		Error CmdPause(WORD displayMode);
		Error CmdPlay(WORD playMode, WORD fadeTime, DWORD start, DWORD stop);
		Error CmdReset(void);
		Error CmdResume(WORD playMode);
		Error CmdScan(WORD skip, WORD scanMode, WORD displayMode);
		Error CmdScreenLoad(DWORD address, DWORD length, WORD displayMode);
		Error CmdSelectStream(WORD streamType, WORD streamNumber);
		Error CmdSetFill(WORD x, WORD y, WORD width, WORD height, DWORD color);
		Error CmdSetStreams(WORD videoID, WORD audioID);
		Error CmdSingleStep(WORD displayMode);
		Error CmdSlowMotion(WORD n, WORD displayMode);
		Error CmdAuthenticate(void);
		
		BOOL	stillSequence, wasStillSequence;
		DWORD	contentType;
		
		Error OutOfIRQJob(void);
		
		Error WaitForState(DWORD state);

		BOOL	singleFrame, singleFrameVOBU;
		
		int	xdelaycnt;
		
		enum XState
			{
			xreset,									// 0
			xidle,									// 1
			
			xinit_waitForRecovery,				// 2
			xinit_waitForBitBufferFilled,		// 3
			xinit_waitForDataRequest,        // 4
			xinit_waitForInitialFrame,			// 5
						
			xresync_waitForRecovery,			// 6
			xresync_waitForBitBufferFilled,	// 7
			xresync_waitForDataRequest,      // 8
			xresync_waitForInitialFrame,		// 9
			
			xscanning_restart,					// 10
			xscanning_waitForBitBufferFilled,// 11
			xscanning_waitForDataRequest,    // 12
			xscanning_waitForTransferComplete, // 13
			xscanning_waitForInitialFrame,	// 14
			xscanning_display,               // 15
			xplaying,                        // 16
			xpaused,                         // 17
			xstopped,                        // 18
			xseeking,                        // 19
			xseekprefreeze,                  // 20
			xseekabort,								// 21
			xresetprepare,							// 22
			xresetprefreeze,						// 23
			xresetdelay,							// 24
			xresetflush,							// 25
			xstopprefreeze,						// 26
			xseekabortdelay,						// 27
			xresetprefreezedelay,				// 28
			xresetdelaydelay,						// 29
			xresync_delay,							// 30
			xstopped_eof,							// 31
			xstopped_sof,							// 32
//			xscanning_waitForTransferCompleteInitialFrame	// 33
			xwaitforpaused

			} xstate;
		int xstateWatchdog, xstopWatchdog;		

	   friend XState operator ++(XState __far& x, int a) {return (XState)(((int __far&)x)++);}
	   friend XState operator --(XState __far& x, int a) {return (XState)(((int __far&)x)--);}
	   
	   DWORD	lastIRQTime;
      
      void ShowStatus(char __far * p);
      
      BOOL CheckChipAlive(void);

		DWORD	irqMask;
		
		virtual DWORD	GetIRQStatus();
		virtual void	ClearIRQStatus();
		virtual Error	SetInterruptMask(DWORD mask);
		virtual void	SetAudioClockSelection();
      
		virtual void	PackSyncClear() {;}

		virtual void	ExecuteVSYNCJob() {;}

      Error ProgramVolume(void);
      Error ProgramVideoStreamID(void);
      Error ProgramAudioStreamID(void);
      Error ProgramSPUStreamID(void);

		virtual Error ProgramVideoStandard();

		Error	SetDVDDemux(BOOL dvdDemux);
		Error	SetEncrypted(BOOL streamEncrypted);

		Error	SetForcedSourceAspectRatio(ForcedAspectRatio forcedSourceAR);

		Error	SetSPDIFHandling(SPDIFHandling spidfHandling);
      
		Error SetSampleRate(WORD rate);
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);

		Error SetBassConfig(AC3BassConfig config);
		Error SetSpeakerConfig(AC3SpeakerConfig config);
		Error SetCenterDelay(WORD delay);
		Error SetSurroundDelay(WORD delay);

		Error SetCenterVolume(WORD volume);
		Error SetLeftSurroundVolume(WORD volume);
		Error SetRightSurroundVolume(WORD volume);
		Error SetSubwooferVolume(WORD volume);

		Error SetAC3(BOOL ac3);
		Error SetLPCM(BOOL lpcm);
		Error SetVideoBitrate(DWORD videoBitrate);
		Error SetVideoWidth(WORD width);
		Error SetVideoHeight(WORD height);
		Error SetVideoFPS(WORD fps);
		Error SetAspectRatio(WORD aspectRatio);
		Error SetVideoStandard(VideoStandard	standard);
//		Error GetDisplaySize(WORD __far &width, WORD __far &height);	
		Error SetStreamType(MPEGStreamType streamType);
		Error SetStreamMode(MPEGStreamMode streamMode);
      Error SetVideoStreamID(BYTE id);
      Error SetAudioStreamID(BYTE id);
      Error SetAC3StreamID(BYTE id);
		Error SetDTSStreamID(BYTE id);
   	Error SetPresentationMode(MPEG2PresentationMode presentationMode);
		Error SetMPEG2Coding(BOOL mpeg2Coding);
		Error SetAudioBitrate(DWORD rate) {GNRAISE_OK;}
	
      Error SetLPCMStreamID(BYTE id);      
      Error SetSPUStreamID(BYTE id);
      Error SetSPUEnable(BOOL enable);
      Error SetSPUButtonState(SPUButtonState state);
      Error SetSPUButtonPosition(int x, int y, int width, int height);
      Error SetSPUButtonColors(DWORD selected, DWORD active);
      Error SetSPUPaletteEntry(int p, int y, int u, int v);
      Error SetSPUCommandHook(MPEG2SPUCommandHookHandle hook);
      Error SetSPUButtonID(WORD id);
      Error SetBitsPerSample(WORD bits);
      Error SetChannels(WORD channels);
      Error DoInternalAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);
      Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);
      
#if WDM_VERSION
		Error	SetCCPacketHook(MPEG2CCPacketHookHandle hook);
		Error SetWDMPlayback(BOOL wdmPlayback);
		Error	SetButtonStartTime(DWORD time);
		Error SetButtonEndTime(DWORD time);
#endif
		Error SetCCEnabled(BOOL ccEnabled);

		VirtualMPEG2AVDecoder * CreateVirtualMPEGDecoder(void);

	   DWORD SendData(HPTR data, DWORD size);
	   DWORD InternalTransferLocation(void);

		DWORD SendDataSplit(MPEGElementaryStreamType type, HPTR data, DWORD size, LONG time);
		void	CompleteDataSplit(MPEGElementaryStreamType type);
		void	RestartDataSplit(MPEGElementaryStreamType type);
	   
	   DWORD LastTransferLocation(DWORD scale = MP2SR_SCALE_BYTES);
	   void CompleteData(void);

	   Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL CommandPending(DWORD tag);
			
		DWORD CurrentLocation(void);
		MPEGState CurrentState(void);
			   
	   DWORD GetPTS();
	   
	   Error SetSignalPosition(DWORD position);

		Error	GetPlaybackTime(LONG __far & pbTime);
		Error	GetSTC(LONG __far & stcTime);

	   Error Update(void);
	   
	   Error SendFakeHeader(void);
	   Error BeginSeek(void);
	   Error BeginReset(void);
	   
      void Interrupt(void);
      void CriticalSection(void);

		void RefillRequest(DWORD free);
      
      Error BeginDecoding(void);
      Error EndDecoding(void);
      
      BOOL resetRequest;
      BOOL initialStartup;
      Error ReinitializeChip(void);
      
      KernelString microcodePath;
      
      BYTE ccbuffer[256];

#if WDM_VERSION
		BYTE gopBuffer[256];
		int goppos, goptotal;
		BOOL	origStartfield;
#endif

      int ccspos, ccstotal;
      BOOL ccstartfield;
      
      enum 
      	{
      	ccs_waitForSync,
      	ccs_waitForKey,
      	ccs_waitForHeader,
      	ccs_waitForMarker,
      	ccs_waitForFirstByte,
      	ccs_waitForSecondByte
      	} ccsState;
      
      BOOL lpcmSpdifOut;

		BOOL isZiVAPC;
      
      Error InterpretUserData(DWORD data);
      Error FlushUserData(void);
	public:
      InterruptServer				*	vblankServer;

		CCubeDVD1Decoder(ByteIndexedInOutPort			*	port,
							  InterruptServer					*	intServer,
							  DualASyncOutStream				*	strm,
							  ProgrammableClockGenerator	*	audioClock,
							  VirtualAudioDAC					*	dac,
							  VirtualUnit						*	videoBus,  
							  VirtualUnit						*	audioBus,
							  MPEG2DVDDecryption				*	decryption,
							  ByteOutputPort					*	codePort,
							  BitOutputPort					*	resetPort,
							  VideoStandard						videoStandard,
							  BOOL                           lpcmSpdifOut);

		virtual ~CCubeDVD1Decoder();

		void SetIsZiVAPC(BOOL isZiVAPC) {this->isZiVAPC = isZiVAPC;}
		void SetProfile(Profile * profile) {this->profile = profile;}

		Error	ReInitialize(void);

#if WDM_VERSION
		void	SetVirtualUnitDefaultParameters(VirtualCCubeDVD1Decoder * unit);
#endif

#if 0
		Error SetMemoryROMModes(DWORD romSize, DWORD accessTime, BOOL bus16bits);
		Error SetMemorySRAMModes(DWORD sramSize, DWORD accessTime, BOOL bus16bits);
		Error SetMemoryDRAMModes(DWORD dramSize);
		Error SetMicrocodeParams(DWORD memType, DWORD overlayCacheBase, DWORD overlayCacheSize);
#endif
		
		Error SetVideoOutputModes(DWORD mode);

		virtual Error Initialize(void);
		
		Error LoadMicrocode(KernelString name);
	};
	
class VirtualCCubeDVD1Decoder : public VirtualMPEG2AVDecoder
	{
	friend class CCubeDVD1Decoder;
	private:
		CCubeDVD1Decoder	*	decoder;

		BOOL						dvdDemux;
		BOOL						streamEncrypted;
		SPDIFHandling			spdifHandling;
		ForcedAspectRatio		forcedSourceAR;
#if WDM_VERSION
		BOOL						wdmPlayback;
		DWORD						buttonStartTime;
		DWORD						buttonEndTime;
#endif
		BOOL						ccEnabled;
	protected:
		VirtualCloseCaptionVideoEncoder	*	ccenc;

      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);
		
		Error GetDisplaySize(WORD __far &width, WORD __far &height);
	public:
		VirtualCCubeDVD1Decoder(CCubeDVD1Decoder * decoder);
		
		Error Configure (TAG __far *tags);
		
		Error SetCCEncoder(VirtualCloseCaptionVideoEncoder	*	ccenc) {this->ccenc = ccenc; GNRAISE_OK;}
	};

#endif
