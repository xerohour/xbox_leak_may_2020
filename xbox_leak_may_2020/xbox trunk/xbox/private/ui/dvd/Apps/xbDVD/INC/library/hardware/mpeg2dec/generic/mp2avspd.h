// FILE:      library\hardware\mpeg2dec\generic\mp2avspd.h
// AUTHOR:    U. Sigmund, S. Herr
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   
//
// PURPOSE:   MPEG2 Audio/Video Split Decoder Class
//
// HISTORY:

#ifndef MP2AVSPD_H
#define MP2AVSPD_H

#include "mpeg2dec.h"

class MPEG2AVSplitDecoder : public MPEG2AVDecoder
	{               
	friend class VirtualMPEG2AVSplitDecoder;
	protected:
		MPEG2VideoDecoder 			*	videoDecoder;
		MPEG2AudioDecoder 			*	audioDecoder;
		MPEG2AC3Decoder			   *	ac3Decoder;
#if VIONA_VERSION
		GenericSPUDecoder				*	spuDecoder;
		MPEG2DVDDecryption			*	decryption;
#endif		

		MPEG2StreamDemux				*	demux;
		VirtualMPEG2VideoDecoder	*	virtualVideoDecoder;
		VirtualMPEG2AudioDecoder	*	virtualAudioDecoder;
		VirtualMPEG2AC3Decoder		*	virtualAC3Decoder;
#if WDM_VERSION
		MPEG2CCPacketHookHandle			ccPacketHook;
#endif

		Error SetAudioBitrate(DWORD rate);
		Error SetSampleRate(WORD rate); 
		Error SetBitsPerSample(WORD bits);
		Error SetChannels(WORD channels);

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

		Error SetVideoBitrate(DWORD videoBitrate);
		Error SetVideoWidth(WORD width);
		Error SetVideoHeight(WORD height);             
		Error SetVideoFPS(WORD fps);
		Error SetAspectRatio(WORD aspectRatio);
		Error SetVideoStandard(VideoStandard	standard);
		Error SetStreamType(MPEGStreamType streamType);
		Error SetStreamMode(MPEGStreamMode streamMode);
      Error SetVideoStreamID(BYTE id);
      Error SetAudioStreamID(BYTE id);
		Error SetAC3StreamID(BYTE id);  
#if VIONA_VERSION
		Error SetPresentationMode(MPEG2PresentationMode presentationMode);
   	Error SetDVDStreamDemux(BOOL dvdDemux);
		
		Error SetLPCMStreamID(BYTE id);
		Error SetDTSStreamID(BYTE id);
      Error SetSPUStreamID(BYTE id);
      Error SetSPUEnable(BOOL enable);

      Error SetSPUButtonState(SPUButtonState state);
      Error SetSPUButtonPosition(int x, int y, int width, int height);
      Error SetSPUButtonColors(DWORD selected, DWORD active);
      Error SetSPUPaletteEntry(int p, int y, int u, int v);
      Error SetSPUCommandHook(MPEG2SPUCommandHookHandle hook);

      Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);

#if WDM_VERSION
		Error	SetCCPacketHook(MPEG2CCPacketHookHandle hook);
#endif
		DWORD SendDataSplit(MPEGElementaryStreamType type, HPTR data, DWORD size, LONG time);
		void CompleteDataSplit(MPEGElementaryStreamType type);
		void RestartDataSplit(MPEGElementaryStreamType type);

#endif      
	public:
		MPEG2AVSplitDecoder(MPEG2VideoDecoder 	* videoDecoder,
		                    MPEG2AudioDecoder 	* audioDecoder,
#if VIONA_VERSION
		                    MPEG2AC3Decoder   	* ac3Decoder = NULL,
		                    GenericSPUDecoder	* spuDecoder = NULL,
		                    MPEG2DVDDecryption	* decryption = NULL);
#else
		                    MPEG2AC3Decoder   * ac3Decoder = NULL);
#endif		                    
		~MPEG2AVSplitDecoder(void);		

	   DWORD GetPTS();
	   DWORD SendData(HPTR data, DWORD size);
		DWORD SendDataMultiple (MPEGDataSizePair * data, DWORD size);

		Error GetPlaybackTime (LONG __far & time);

	   Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL CommandPending(DWORD tag);

	   void CompleteData(void);
	   DWORD CurrentLocation(void);
	   DWORD LastTransferLocation(DWORD scale);

		MPEGState CurrentState(void);
			   
	   Error SetSignalPosition(DWORD position);
	   
	   VirtualMPEG2AVDecoder * CreateVirtualMPEGDecoder(void);
		};

#define NUM_VM2AVTAGS	32

	
class VirtualMPEG2AVSplitDecoder : public VirtualMPEG2AVDecoder
	{  
	friend class VM2AVSignalHook;
	friend class VM2AVDoneHook;
	private:
		MPEG2AVSplitDecoder			*	decoder;
	protected:	
		MPEG2StreamDemux				*	demux;
		VirtualMPEG2VideoDecoder	*	videoDecoder;
		VirtualMPEG2AudioDecoder	*	audioDecoder;
		VirtualMPEG2AC3Decoder		*	ac3Decoder;

		Line21Receiver					*	line21Receiver;
		
		class VM2AVSignalHook __far * xtHook;
		class VM2AVDoneHook __far * xdHook;
		
		struct {DWORD vtag, atag;} ptags[NUM_VM2AVTAGS];
		
		int	doneCnt;
		BOOL	scanning;

		Error SignalHookRequest(void);
		Error DoneHookRequest(void);

		DWORD first, last;     
		DWORD locationOffset;
		DWORD	positionBaseScale;

		DWORD	lastSentLocationLow;
		DWORD lastSentLocation;
//		DWORD signalLocation;

		void UpdateLocations (DWORD newBytes);

		WORD	playbackSpeed;

		Error AdvanceTags(void);

		Error GetDisplaySize(WORD __far &width, WORD __far &height);	

		Error PreemptStopPrevious(VirtualUnit * previous);
		Error PreemptChange(VirtualUnit * previous);
		Error PreemptStartNew(VirtualUnit * previous);

	public:
		VirtualMPEG2AVSplitDecoder(MPEG2AVSplitDecoder * decoder, BOOL idle = FALSE);
		~VirtualMPEG2AVSplitDecoder(void);

		Error Configure(TAG __far * tags);

		Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL CommandPending(DWORD tag);
		Error CompleteCommand(DWORD tag);

		DWORD SendData(HPTR data, DWORD size);
		DWORD SendDataMultiple (MPEGDataSizePair * data, DWORD size);
		void CompleteData(void);
		DWORD SendDataSplit(MPEGElementaryStreamType type, HPTR data, DWORD size, LONG time);
		void CompleteDataSplit(MPEGElementaryStreamType type);
		void RestartDataSplit(MPEGElementaryStreamType type);
		DWORD CurrentLocation(void);
		DWORD LastTransferLocation(DWORD scale);

		MPEGState CurrentState(void);

		Error SetLine21Receiver(Line21Receiver * line21Receiver);
	};

#endif
