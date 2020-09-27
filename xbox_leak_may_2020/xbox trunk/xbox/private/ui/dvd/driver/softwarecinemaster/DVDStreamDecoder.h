////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef DVDSTREAMDECODER_H
#define DVDSTREAMDECODER_H

#include "mpegdecoder\MPEGVideoStreamDecoder.h"
#include "ac3decoder\AC3StreamDecoder.h"
#include "LPCMDecoder\LPCMStreamDecoder.h"
#include "MPEGAudioDecoder\MPEGAudioStreamDecoder.h"
#include "spudecoder\SPUStreamDecoder.h"
#include "common\TimedFibers.h"
#include "audio\DTSStreamDecoder.h"
#include "closedcaption\ClosedCaptionDecoder.h"
#include "library\hardware\mpeg2dec\generic\mp2eldec.h"

enum DVDStreamType
	{
	DVST_ELEMENTARY_AUDIO_STREAM,
	DVST_ELEMENTARY_VIDEO_STREAM,
	DVST_PROGRAM_STREAM
	};

enum DVDSplitStreamType
	{
	DVSST_DVD_ENCRYPTED,
	DVSST_PROGRAM,
	DVSST_PES,
	DVSST_ELEMENTARY
	};

enum DVDElementaryStreamType
	{
	DVEST_VIDEO,
	DVEST_AUDIO,
	DVEST_SUBPICTURE,
	DVEST_MULTIPLEXED
	};

class DVDStreamDecoder;

class GenericDecryptor
	{
	public:
		virtual void DecryptBlock(BYTE * block, BYTE * target) = 0;
	};

class EmbeddedMPEGVideoStreamDecoder : public MPEGVideoStreamDecoder
	{
	protected:
		DVDStreamDecoder	*	dvdDecoder;
	public:
		EmbeddedMPEGVideoStreamDecoder(GenericPictureDisplay * display,
										GenericSliceParser * highQualitySliceParser,
			                            GenericSliceParser * lowQualitySliceParser,
										DVDStreamDecoder * dvdDecoder,
										GenericProfile * globalProfile,
										GenericProfile * profile,
										StreamFlipCopy * streamFlipCopy);
		void RefillRequest(void);
		void StreamCompleted(void);
		void ScanCompleted(void);
	};

class EmbeddedAC3StreamDecoder : public AC3StreamDecoder
	{
	protected:
		DVDStreamDecoder	*	dvdDecoder;
	public:
		EmbeddedAC3StreamDecoder(PCMWaveOut * pcmWaveOut,
									TimingSlave * timingSlave,
									DVDStreamDecoder * dvdDecoder,
									GenericProfile * globalProfile,
									GenericProfile * profile,
									StreamFlipCopy * streamFlipCopy);
		void RefillRequest(void);
		void StreamCompleted(void);

		void SetDigitalOut(AudioTypeConfig ac3AudioType)
			{AC3StreamDecoder::SetDigitalOut(ac3AudioType);}
		void GetDigitalOut(AudioTypeConfig & ac3AudioType)
			{AC3StreamDecoder::GetDigitalOut(ac3AudioType);}

		void SetAC3Config(AC3DynamicRange dr, BOOL enableLFE)
			{AC3StreamDecoder::SetAC3Config(dr, enableLFE);}
		void GetAC3Config(AC3DynamicRange &dr, BOOL &enableLFE)
			{AC3StreamDecoder::GetAC3Config(dr, enableLFE);}

		void SetDolbyTestMode(AC3OperationalMode op, WORD hf, WORD lf, AC3DialogNorm dn)
			{AC3StreamDecoder::SetDolbyTestMode(op, hf, lf, dn);}
		void GetDolbyTestMode(AC3OperationalMode &op, WORD &hf, WORD &lf, AC3DialogNorm &dn)
			{AC3StreamDecoder::GetDolbyTestMode(op, hf, lf, dn);}
	};


class EmbeddedDTSStreamDecoder : public DTSStreamDecoder
	{
	protected:
		DVDStreamDecoder	*	dvdDecoder;
	public:
		EmbeddedDTSStreamDecoder(PCMWaveOut * pcmWaveOut,
									TimingSlave * timingSlave,
									DVDStreamDecoder * dvdDecoder,
									GenericProfile * globalProfile,
									GenericProfile * profile,
									StreamFlipCopy * streamFlipCopy);
		void RefillRequest(void);
		void StreamCompleted(void);
		void SetDigitalOut(AudioTypeConfig ac3AudioType)
			{DTSStreamDecoder::SetDigitalOut(ac3AudioType);}
		void GetDigitalOut(AudioTypeConfig & ac3AudioType)
			{DTSStreamDecoder::GetDigitalOut(ac3AudioType);}
	};


class EmbeddedMPEGAudioStreamDecoder : public MPEGAudioStreamDecoder
	{
	protected:
		DVDStreamDecoder	*	dvdDecoder;
	public:
		EmbeddedMPEGAudioStreamDecoder(PCMWaveOut * pcmWaveOut,
										TimingSlave * timingSlave,
										DVDStreamDecoder * dvdDecoder,
										GenericProfile * globalProfile,
										GenericProfile * profile,
										StreamFlipCopy * streamFlipCopy);
		void RefillRequest(void);
		void StreamCompleted(void);
	};

class EmbeddedLPCMStreamDecoder : public LPCMStreamDecoder
	{
	protected:
		DVDStreamDecoder	*	dvdDecoder;
	public:
		EmbeddedLPCMStreamDecoder(PCMWaveOut * pcmWaveOut,
									TimingSlave * timingSlave,
									DVDStreamDecoder * dvdDecoder,
									StreamFlipCopy * streamFlipCopy);
		void RefillRequest(void);
		void StreamCompleted(void);
	};

class EmbeddedSPUStreamDecoder : public SPUStreamDecoder
	{
	protected:
		DVDStreamDecoder	*	dvdDecoder;
	public:
		EmbeddedSPUStreamDecoder(SPUDisplay * display,
			                      DVDStreamDecoder * dvdDecoder);

		void RefillRequest(void);
	};

class DVDPositionTracker
	{
	protected:
		struct Track
			{
			__int64	mux, elem;
			} * track;

		int first, last, num, mask, prev;

	public:
		DVDPositionTracker(int num);
		~DVDPositionTracker(void);

		void Flush(void);

		void AddTrack(__int64 mux, __int64 elem);
		__int64 MuxToElem(__int64 mux);
		__int64 ElemToMux(__int64 elem);
	};

class DVDStreamDecoder : private TimedFiber
	{
	friend class EmbeddedMPEGVideoStreamDecoder;
	friend class EmbeddedAC3StreamDecoder;
	friend class EmbeddedDTSStreamDecoder;
	friend class EmbeddedMPEGAudioStreamDecoder;
	friend class EmbeddedSPUStreamDecoder;
	friend class EmbeddedLPCMStreamDecoder;
	protected:
		BYTE						videoStreamID;
		BYTE						audioStreamID;
		BYTE						ac3StreamID;
		BYTE						dtsStreamID;
		BYTE						spuStreamID;
		BYTE						lpcmStreamID;

		bool						streamingCompleted;
		bool						terminate;
		bool						hurryUp;
		bool						initialAudioPacket;

		bool						dataCompleted, splitCompletion;
		bool						highQuality;

		TimedFiberEvent				bufferMonitorEvent;
		TimedFiberMutex			lock;

		MPEG2AudioType				audioType;
		int							samplesPerSecond;
		int							bitsPerSample;
		int							channels;

		DVDStreamType				streamType;

		enum PESState {idle, videoData, audioData, audioHeaderMpeg, audioDataMpeg, spuData};
		struct MPEParse
			{
			int						pesRest, pesSkip, pesOffset;
			PESState				pesState;
			int						endPTM, ptsOffset;

			BYTE					decBuffer[1920];
			bool					encryptedBlock;
			DVDSplitStreamType		streamType;
			bool					completing, completed;

			struct
				{
				int			state;

				WORD		cnt;
				DWORD		length;
				BYTE		high, low;
				BYTE		headerLength, headerRest;
				BYTE		streamID;
				BYTE		flags, xflags;
				BYTE		ptss[5];
				DWORD		pts;
				bool		ptsValid;
				DWORD		dts;
				bool		dtsValid;

				bool		mpeg2;
  				} mpparse;

			volatile MPEGDataSizePair	*	pendingPairs;
			volatile BYTE				*	pendingData;
			volatile DWORD					pendingNum;
			volatile int					pendingTimeStamp;
			HANDLE							dataEvent;

			} mpeparse[4];


		bool								streaming, running, deferedRunning;
		VideoPlaybackMode					playbackMode;
		int									initialVideoTime;
		int									playbackSpeed;
		int									playbackTimeOffset;
		int									initTimeStamp;

		PCMWaveOut							pcmWaveOut;

		GenericAudioPlaybackDevice		*	audioPlaybackDevice;
		EmbeddedMPEGVideoStreamDecoder		videoDecoder;
		EmbeddedAC3StreamDecoder			ac3Decoder;
		EmbeddedDTSStreamDecoder			dtsDecoder;
		EmbeddedMPEGAudioStreamDecoder		mpegAudioDecoder;
		EmbeddedLPCMStreamDecoder			lpcmDecoder;
		EmbeddedSPUStreamDecoder			spuDecoder;
		GenericPictureDisplay			*	display;

		AudioStreamDecoder				*	audioDecoder;
		ClosedCaptionDecoder			*	ccDecoder;

		DVDPositionTracker					audioTracker;
		DVDPositionTracker					videoTracker;

		GenericDecryptor				*	decryptor;
		HANDLE								cinemSupDriver;

		DWORD								lastCheckTime;
		__int64								lastCheckValue;

		void InternalAdvance(MPEParse * mp, int by, BYTE * & ptr, DWORD & num);
		BYTE InternalGetData(MPEParse * mp, int at, BYTE * ptr);

		DWORD InternalSendData(BYTE * ptr, DWORD num);
		DWORD InternalSendDataSplit(DVDElementaryStreamType type, BYTE * ptr, DWORD num, int timeStamp);

		class Supervisor : public TimedFiber
			{
			protected:
				DVDStreamDecoder * decoder;
			public:
				Supervisor(DVDStreamDecoder * decoder);
				void FiberRoutine(void);
			} supervisor;

		friend class DVDStreamDecoder::Supervisor;

		void FiberRoutine(void);
		void SupervisorFiberRoutine(void);

		__int64	audioInputPos, videoInputPos, spuInputPos, streamInputPos, streamInputOffset;
		int		vobuInputPos;
		__int64	signalPos;
		int previousRefillRequestTime;

		virtual void SignalReached(__int64 signalPos) {}
		virtual void StreamCompleted(DVDElementaryStreamType type) {}

		__int64 InternalCurrentLocation(void);
		void InternalRefillRequest(void);

		void FlushBuffer(void);
		void CheckSignal(void);
		void ScanCompleted(void);

		bool EnoughStreamsCompleteToStart(void);
		void DeferedStartStreaming(void);
		void ShutDownSupervisor(void);
	public:
		DVDStreamDecoder(GenericPictureDisplay * display,
							  GenericSliceParser * highQualitySliceParser,
			              GenericSliceParser * lowQualitySliceParser,
							  GenericAudioPlaybackDevice * audioPlaybackDevice,
							  SPUDisplay * spuDisplay,
							  GenericProfile * globalProfile,
							  GenericProfile * profile,
							  ClosedCaptionDecoder * ccDecoder,
							  StreamFlipCopy * streamFlipCopy,
							  GenericDecryptor * decryptor = NULL);
		~DVDStreamDecoder(void);

		void SetStreamID(BYTE videoStreamID, BYTE ac3StreamID, BYTE audioStreamID, BYTE lpcmStreamID, BYTE spuStreamID, BYTE dtsStreamID);

		void SetAudioCodingMode(MPEG2AudioType type, int samplesPerSecond, int bitsPerSample, int channels);
		void SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg);
//#if DOLBY_TEST_MODE
//		void SetDolbyTestMode(AC3OperationalMode op, WORD hf, WORD lf, AC3DialogNorm dn);
//		void GetDolbyTestMode(AC3OperationalMode &op, WORD &hf, WORD &lf, AC3DialogNorm &dn);
//#endif DOLBY_TEST_MODE
//		void SetAC3Config(AC3DynamicRange dr, BOOL enableLFE);
//		void GetAC3Config(AC3DynamicRange &dr, BOOL &enableLFE);

		void SetLine21Decoder(Line21Decoder * line21Decoder) {videoDecoder.SetLine21Decoder(line21Decoder);}

		void SetStreamType(DVDStreamType streamType) {this->streamType = streamType;}

		void BeginStreaming(VideoPlaybackMode playbackMode, bool highQuality, __int64 start = 0);
		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);

		void AdvanceFrame(void);
		void AdvanceTrickFrame(int by);
		void EnableSPU(bool enable);
		void SetDVDDemux(bool dvdDemux);
		void SetSplitStreamType(DVDElementaryStreamType type, DVDSplitStreamType streamType);

		DWORD SendData(BYTE * ptr, DWORD num);
		DWORD SendDataSplit(DVDElementaryStreamType type, BYTE * ptr, DWORD num, int timeStamp);
		DWORD SendDataMultiple(MPEGDataSizePair * data, DWORD size);
		DWORD SendDataSplitMultiple(DVDElementaryStreamType type, MPEGDataSizePair * data, DWORD size);
		void CompleteData(void);
		void CompleteDataSplit(DVDElementaryStreamType type);
		void RestartDataSplit(DVDElementaryStreamType type);

		__int64 CurrentLocation(void);
		__int64 CurrentInputLocation(void) {return streamInputPos + streamInputOffset;}
		int CurrentPlaybackTime(void);

		void SetSignalPosition(__int64 pos);
		DWORD VideoSegmentRequested(void);
		void SetFirstVideoSegment(DWORD seg);
		void SetLastVideoSegment(DWORD seg);

		virtual void RefillRequest(void) = 0;

		bool StreamingCompleted(void) {return streamingCompleted;}
		void SetErrorMessenger(ASyncErrorMessenger * errorMessenger)
			{
			display->SetErrorMessenger(errorMessenger);
			videoDecoder.SetErrorMessenger(errorMessenger);
			}
	};

#endif
