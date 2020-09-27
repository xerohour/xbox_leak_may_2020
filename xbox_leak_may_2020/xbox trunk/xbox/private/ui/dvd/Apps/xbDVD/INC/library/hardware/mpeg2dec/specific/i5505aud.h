
// FILE:			library\hardware\mpeg2dec\specific\i5505aud.h
// AUTHOR:		U. Sigmund, D. Heidrich, S. Herr
// COPYRIGHT:	(c) 1998 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.07.98
//
// PURPOSE:		Audio decoder of the STi5505.
//
// HISTORY:

#ifndef I5505AUD_H
#define I5505AUD_H

#include "..\generic\mp2eldec.h"
#include "library\hardware\audio\generic\audiodac.h"
#include "library\lowlevel\hardwrio.h"
#include "library\general\asncstrm.h"  
#include "library\lowlevel\intrctrl.h"
#include "library\common\krnlstr.h"
#include "library\hardware\clocks\generic\clocks.h"
#include "library\common\profiles.h"


#if 0
enum DACType
	{
	DACT_DEFAULT,
	DACT_PCM1720
	};

enum DACFormat
	{
	DACF_I2S,
	DACF_SONY
	};

enum PLLType
	{
	PLLT_NONE,
	PLLT_PLL1700
	};
#endif

class STi5505AC3Decoder : public MPEG2AC3Decoder, 
                          protected ASyncRefillRequest,
                          protected InterruptHandler
	{
	friend class VirtualSTi5505AC3Decoder;
	protected:
		GenericProfile					*	profile;
		ByteIndexedInOutPort			*	port;
		ByteIndexedInOutPort			*	bufferPort;
		VirtualAudioDAC				*	dac; 
		ASyncOutStream					*	strm;		
		ProgrammableClockGenerator *	audioClock;
		BitOutputPort					*	boardMutePort;

		int			inUpdate;

		// Configuration
		BOOL			dtsEnable;
		BOOL			srsEnable;
		BOOL			enable3DSound;
		BOOL			mpeg2MCEnable;
		int			dacType;
		WORD			dacNum;
		int			dacFormat;
		WORD			dacBits;
		DWORD			dacSampleRate;
		BOOL			dacLRSwap;
		WORD			pcmClock;		
		int			pllType;
		BOOL			halfFsAt96kHz;
		int			downmixForce;
		BOOL			limitPCM;

		DWORD			changed;

		BYTE			chipVersion;
		BYTE			softVersion;

		DWORD 		initialPTS;
		DWORD			audioBitrate;
		DWORD			sampleRate;
		WORD			bitsPerSample;
		WORD			channels;
		MPEGState	state;
		MPEG2AudioType	audioType;
		BOOL			ac3, lpcm;
		BYTE			ac3StreamID;
		PTSAssoc		ptsAssoc;
		DWORD			streamPosition;
		WORD			muteDelay;
		WORD			playbackSpeed;

		WORD			leftDelay;
		WORD			rightDelay;
		WORD			centerDelay;
		WORD			leftSurroundDelay;
		WORD			rightSurroundDelay;
		WORD			subwooferDelay;

		DWORD			signalPosition;
		DWORD			predPosition;

		DWORD			stepCnt;

		DWORD			conformPTS;
		BOOL			conformPTSValid;
		PTSCounter	ptsCounter;

		BOOL			internalPCMClock;
		BOOL			starving, syncPlayDelay;

		MPEG2PESType	needsPES;

		BOOL			decodeLFE;

		int			frameBytePos, frameBytesLeft, frameStepCount;

		AudioTypeConfig			typeConfigs[MP2AUDTYP_NUMBER_OF_TYPES];

		AC3SpeakerConfig			speakerConfig;
		AC3KaraokeConfig			karaokeConfig;
		AC3DualModeConfig			dualModeConfig;

		SPDIFHandling				spdifHandling;
		SPDIFOutputMode			spdifOutMode;
		SPDIFCopyMode				spdifCopyMode;
		SPDIFCopyMode				overrideSPDIFCopyMode;

		MPEG2AudioSpatializer	spatializer;

		DolbyProLogicConfig		prologicConfig;
		DolbyProLogicMode			prologicMode;
		AC3DynamicRange			ac3DynamicRange;
		PCMOutputConfig			pcmOutputConfig;
		PCM96DownsampleMode		downSampleMode;

		VirtualUnit	*	audioBus;

		BOOL	mute;
		WORD	lv, rv, lsv, rsv, swv, cv;   // volumes for left, right, l surround, r surround, subwoofer and center
		
		BOOL	spdifUpdateRequest;
		BYTE	updateSPDIFCmd;

		MPEG2CommandQueue	commandQueue;

		enum XState
			{
			xreset,											//  0
			xplaying,										//  1
			xfrozen,											//  2
			xstopped,										//  3
			xseeking,										//  4
			xresync,											//  5
			xstepping,										//  6
			xstarving,										//  7
			xstill,											//  8
			xseeking_audio,								//  9
			xresync_audio,									// 10
			xseeking_audio_waitforcompletion,		// 11
			xseeking_audio_datacomplete,				// 12
			xtesting											// 13
			} xstate, preSeekAudioState;

#if _DEBUG
		void PrintStatus(void);
#endif

		DWORD		xwatchdog;
#if 0		
		DWORD		oldAvailData;
#endif

		DWORD		GetInternalPTS();

		virtual	Error Update(void);

		Error		ProgramMuteAll(BOOL mute);

		Error SetVolume(void);

		Error SetAudioType(MPEG2AudioType audioType);

		virtual Error	SetSpatializer(MPEG2AudioSpatializer spatializer);

		virtual Error	SetSPDIFHandling(SPDIFHandling spdifHandling);
		virtual Error	SetSPDIFOutputMode(SPDIFOutputMode spdifOutMode);
		virtual Error	SetSPDIFCopyMode(SPDIFCopyMode spdifCopyMode);
		virtual Error	Set96kHzDACDownSampleMode(PCM96DownsampleMode	dacDownSampleMode);
		virtual Error	Set96kHzSPDIFDownSampleMode(PCM96DownsampleMode	spdifDownSampleMode);

   	Error SetDVDStreamEncrypted(BOOL dvdEncrypted);

		Error SetAudioBitrate(DWORD bitrate);
		Error SetSampleRate(WORD rate);
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);
		Error SetAC3(BOOL ac3);
		Error SetLPCM(BOOL lpcm);
		Error SetBitsPerSample(WORD samples);
		Error SetChannels(WORD channels);

		Error SetBassConfig(AC3BassConfig config);
		Error SetSpeakerConfig(AC3SpeakerConfig config);
		Error SetDualModeConfig(AC3DualModeConfig config);
		Error SetKaraokeConfig(AC3KaraokeConfig config);
		Error SetCenterDelay(WORD delay);
		Error SetSurroundDelay(WORD delay);      

		Error SetCenterVolume(WORD volume);
		Error SetLeftSurroundVolume(WORD volume);
		Error SetRightSurroundVolume(WORD volume);
		Error SetSubwooferVolume(WORD volume);

		Error SetLeftDelay(WORD leftDelay);
		Error SetRightDelay(WORD rightDelay);
		Error SetLeftSurroundDelay(WORD leftSurroundDelay);
		Error SetRightSurroundDelay(WORD rightSurroundDelay);
		Error SetSubwooferDelay(WORD subwooferDelay);
		Error SetPrologicConfig(DolbyProLogicConfig prologicConfig);
		Error SetPrologicMode(DolbyProLogicMode prologicMode);
		Error SetDecodeLFE(BOOL decodeLFE);
		Error SetPCMOutputConfig(PCMOutputConfig pcmOCFG);
		Error SetDynamicRange(AC3DynamicRange dynamicRange);
		Error SetAudioTypeConfig(MPEG2AudioType type, AudioTypeConfig config);

		virtual Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		virtual BOOL CommandPending(DWORD tag);

		virtual DWORD CurrentLocation(void);
		virtual MPEGState CurrentState(void);

		DWORD LastTransferLocation(DWORD scale);

		virtual DWORD GetPTS();
		virtual Error ConformToPTS (DWORD pts);

		DWORD SendData(HPTR data, DWORD size);
#if VIONA_VERSION
		Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used);
#endif
		void CompleteData(void);

		void PutPTS(DWORD pts);

		//
		// Forward the refill request
		//
		void RefillRequest(DWORD free);
		void Interrupt(void);

		Error SetSignalPosition(DWORD position);
		void	CheckSignalPosition (void);

		void ReadWordHL (ByteIndexedInOutPort *port, WORD offset, WORD &result);

		Error SyncPlay(void);

		InterruptServer	*	vblank;
		InterruptServer	*	audioIRQServer;
		InterruptHandler	*	audioIRQHandler;
		BOOL						audioIRQEnable;

		Error ReInitialize(void);

		Error SelectPlaybackType(void);

		Error ProgramDACFormat(BOOL limitWidth);
		Error ProgramOutputFormat(BOOL limitWidth);
	public:
		STi5505AC3Decoder (ByteIndexedInOutPort	*	port,
		                   ByteIndexedInOutPort	*	bufferPort,
		                   VirtualAudioDAC		 	*	dac,
		                   ASyncOutStream			*	strm,
		                   InterruptServer			*	vblank,
		                   VirtualUnit				*	audioBus = NULL,
		                   BOOL							internalPCMClock = NULL,
		                   ProgrammableClockGenerator * audioClock = NULL,
								 GenericProfile			*	profile = NULL,
								 BitOutputPort				*	boardMutePort = NULL,
								 InterruptServer			*	audioIRQServer = NULL);

		VirtualUnit * CreateVirtual(void);

		Error LoadPatchFile(void);
		Error Initialize(void);
		void AudioInterrupt(void);
	};



class VirtualSTi5505AC3Decoder : public VirtualMPEG2AC3Decoder
	{
	private:
		STi5505AC3Decoder	*		decoder;

		BOOL	idleUnit;
		BOOL	activatedAsIdle;

		SPDIFHandling				spdifHandling;
		SPDIFOutputMode			spdifOutMode;
		SPDIFCopyMode				spdifCopyMode;
		MPEG2AudioSpatializer	spatializer;
		PCM96DownsampleMode		dacDownSampleMode;
		PCM96DownsampleMode		spdifDownSampleMode;
	protected:
		Error PreemptStopPrevious(VirtualUnit * previous);
		Error PreemptStartNew(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
	public:
		VirtualSTi5505AC3Decoder(STi5505AC3Decoder * decoder, BOOL idleUnit = FALSE);

		Error Configure(TAG __far * tags);

#if VIONA_VERSION
		MPEG2PESType NeedsPES(void) {return decoder->needsPES;}
#endif
	};



#endif
