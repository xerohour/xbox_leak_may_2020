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

#ifndef MPEG2DEC_H
#define MPEG2DEC_H

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"
#include "mp2eldec.h"

#include "mp2dcryp.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#include "mp2spudc.h"
#include "mp2demux.h"

class MPEG2AVDecoder : public MPEG2ElementaryDecoder
   {
   friend class VirtualMPEG2AVDecoder;
   protected:
   	MPEGStreamType		streamType;
   	BOOL					ac3;
   	BOOL					mpeg2Coding;
   	BOOL					lpcm;
		MPEG2AudioType		audioType;

		virtual Error SetAudioBitrate(DWORD rate) = 0;
		virtual Error SetSampleRate(WORD rate) = 0; // in samples per second;
		virtual Error SetLeftVolume(WORD volume) = 0;
		virtual Error SetRightVolume(WORD volume) = 0;
		virtual Error SetMute(BOOL mute) = 0;

		virtual Error SetBassConfig(AC3BassConfig config) = 0;
		virtual Error SetSpeakerConfig(AC3SpeakerConfig config) = 0;
		virtual Error SetCenterDelay(WORD delay) = 0;
		virtual Error SetSurroundDelay(WORD delay) = 0;

		virtual Error SetCenterVolume(WORD volume) = 0;
		virtual Error SetLeftSurroundVolume(WORD volume) = 0;
		virtual Error SetRightSurroundVolume(WORD volume) = 0;
		virtual Error SetSubwooferVolume(WORD volume) = 0;

		virtual Error SetAC3(BOOL ac3) {this->ac3 = ac3; GNRAISE_OK;}
		virtual Error SetLPCM(BOOL lpcm) {this->lpcm = lpcm; GNRAISE_OK;}
		virtual Error SetVideoBitrate(DWORD videoBitrate) = 0;
		virtual Error SetVideoWidth(WORD width) = 0;
		virtual Error SetVideoHeight(WORD height) = 0;
		virtual Error SetVideoFPS(WORD fps) = 0;
		virtual Error SetAspectRatio(WORD aspectRatio) = 0;
		virtual Error SetVideoStandard(VideoStandard	standard) = 0;
#if TEST_PAL_TO_NTSC
		virtual Error SetApplicationVideoStandard(VideoStandard	standard) = 0;
		virtual MovingImageStandard GetEncoderMovingImageStandard () = 0;
#endif
		virtual Error SetStreamType(MPEGStreamType streamType) = 0;
		virtual Error SetStreamMode(MPEGStreamMode streamMode) = 0;
		virtual Error SetMPEG2Coding(BOOL mpeg2Coding) {this->mpeg2Coding = mpeg2Coding; GNRAISE_OK;}
      virtual Error SetVideoStreamID(BYTE id) = 0;
      virtual Error SetAudioStreamID(BYTE id) = 0;
      virtual Error SetAC3StreamID(BYTE id) = 0;
   	virtual Error SetPresentationMode(MPEG2PresentationMode presentationMode) = 0;

      virtual Error SetLPCMStreamID(BYTE id) = 0;
		virtual Error SetDTSStreamID(BYTE id) = 0;
      virtual Error SetSPUStreamID(BYTE id) = 0;
		virtual Error SetMLPStreamID(BYTE id) = 0;
      virtual Error SetSPUEnable(BOOL enable) = 0;
      virtual Error SetSPUButtonState(SPUButtonState state) = 0;
      virtual Error SetSPUButtonPosition(int x, int y, int width, int height) = 0;
      virtual Error SetSPUButtonColors(DWORD selected, DWORD active) = 0;
      virtual Error SetSPUPaletteEntry(int p, int y, int u, int v) = 0;
      virtual Error SetSPUCommandHook(MPEG2SPUCommandHookHandle hook) = 0;
      virtual Error SetSPUButtonID(WORD id) {GNRAISE_OK;}
      virtual Error SetBitsPerSample(WORD bits) = 0;
      virtual Error SetChannels(WORD channels) = 0;
      virtual Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key) = 0;

#if WDM_VERSION
		virtual Error SetCCPacketHook(MPEG2CCPacketHookHandle hook) = 0;
#endif
		virtual DWORD SendDataSplit(MPEGElementaryStreamType type, HPTR data, DWORD size, LONG time) = 0;
		virtual void CompleteDataSplit(MPEGElementaryStreamType type) = 0;
		virtual void RestartDataSplit(MPEGElementaryStreamType type) = 0;

		virtual Error SetAudioType(MPEG2AudioType audioType) {this->audioType = audioType; GNRAISE_OK;}
	public:
		VirtualUnit * CreateVirtual(void);

		virtual VirtualMPEG2AVDecoder * CreateVirtualMPEGDecoder(void) = 0;
   };

class VirtualMPEG2AVDecoder : public VirtualMPEG2ElementaryDecoder
	{
	private:
		MPEG2AVDecoder * decoder;
	protected:
   	MPEGStreamType		streamType;
		MPEGStreamMode		streamMode;
   	BOOL					mpeg2Coding;
   	BOOL					ac3;
   	BOOL					lpcm;
   	DWORD					audioBitrate;
		DWORD					videoBitrate;
		DWORD					streamBitrate;
		WORD					videoWidth;
		WORD					videoHeight;
		WORD					videoFPS;
		WORD					aspectRatio;
//		VideoStandard		videoStandard;		// why is this here????
		WORD					sampleRate;
		WORD					leftVolume;
		WORD					rightVolume;
		BOOL					audioMute;

		AC3BassConfig  	bassConfig;
		AC3SpeakerConfig  speakerConfig;
		WORD					centerDelay;
		WORD					surroundDelay;
		WORD					centerVolume;
		WORD					leftSurroundVolume;
		WORD					rightSurroundVolume;
		WORD					subwooferVolume;

		BYTE					videoStreamID;
		BYTE					audioStreamID;
		BYTE					ac3StreamID;
		BYTE					lpcmStreamID;
		BYTE					dtsStreamID;
		BYTE					mlpStreamID;
		BYTE					spuStreamID;
		WORD					channels;
		WORD					bitsPerSample;
		MPEG2PresentationMode	presentationMode;

		BOOL					spuEnable;
		SPUButtonState		buttonState;
		WORD					bx, by, bw, bh;
		DWORD					buttonSelectColor;
		DWORD					buttonActiveColor;
		DWORD					spuPalette[16];
		WORD					buttonID;
		MPEG2SPUCommandHookHandle	spuCommandHook;

#if WDM_VERSION
		MPEG2CCPacketHookHandle		ccPacketHook;
#endif
		MPEG2AudioType		audioType;

		Error PreemptChange(VirtualUnit * previous);
	public:
		VirtualMPEG2AVDecoder(MPEG2AVDecoder * decoder);

		Error Configure(TAG __far * tags);

      virtual Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);
		virtual Error GetDisplaySize(WORD __far &width, WORD __far &height) = 0;
		virtual DWORD SendDataSplit(MPEGElementaryStreamType type, HPTR data, DWORD size, LONG time);
		virtual void CompleteDataSplit(MPEGElementaryStreamType type);
		virtual void RestartDataSplit(MPEGElementaryStreamType type);

		virtual Error SetLine21Receiver(Line21Receiver * line21Receiver);
	};

#endif // ONLY_EXTERNAL_VISIBLE

#endif // MPEG2DEC_H
