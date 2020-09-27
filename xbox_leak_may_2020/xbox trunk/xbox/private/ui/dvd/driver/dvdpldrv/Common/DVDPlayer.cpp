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

////////////////////////////////////////////////////////////////////
//
//  DVD Player Class
//
////////////////////////////////////////////////////////////////////

#include "DVDPlayer.h"
#include "Library/Common/vddebug.h"
#include "Library/lowlevel/timer.h"

#define REFILL_MSG		0x1000
#define SIGNAL_MSG		0x1001
#define ERROR_MSG			0x1002
#define DONE_MSG			0x1003

#define MEASURE_TRANSFER_RATE	0

//
//  Constructor
//

DVDPlayer::DVDPlayer(WinPortServer *server, UnitSet units, EventDispatcher* pEventDispatcher) : EventPortWorkerThread(server, THREAD_PRIORITY_HIGHEST)
	, DVDStreamPlayer(pEventDispatcher)
	{
	this->units = units;
	VDR_OpenSubUnits(units, MPEG_DECODER_UNIT, decUnits);

	callback = NULL;

	videoAttr = 0xffff;   // this illegal value ensures complete computation of video attributes
	audioStreamID = 0;
	audioAttr = 0xffff;
	subPictureStreamID = 0;
	subPictureAttr = 0xffff;
	stillFrameSequence = FALSE;
	videoStandard = VSTD_NTSC;

	transferCount = 0;
	transferTotal = 0;
 	transferCountRemainder = 0;

	playbackSpeed = 1000;
	backward = FALSE;
	inCallback = FALSE;
	paused = FALSE;
	refiller = NULL;

	VDR_InstallMessagePort(units, GetPort());
	VDR_ConfigureUnitsTags(units, SET_MPEG2_REFILL_MSG(REFILL_MSG),
		                           SET_MPEG2_SIGNAL_MSG(SIGNAL_MSG),
											SET_MPEG2_DONE_MSG(DONE_MSG),
											SET_MPEG2_ERROR_MSG(ERROR_MSG),
											TAGDONE);

	VDR_ConfigureUnitsTags(units, SET_MPEG2_DVD_STREAM_DEMUX(TRUE),
											SET_MPEG2_CODING_STANDARD(TRUE),
										   SET_ENC_COPY_PROTECTION(0),
	                              TAGDONE);

	VDR_ConfigureUnitsTags(decUnits, SET_MPEG2_SPU_ENABLE(FALSE),
												SET_MPEG2_DVD_STREAM_ENCRYPTED(FALSE),

												SET_MPEG_VIDEO_BITRATE(5000000),
												SET_MPEG_STREAM_BITRATE(7000000),

												SET_MPEG_VIDEO_WIDTH(720),
												SET_MPEG_VIDEO_HEIGHT(480),
												SET_MPEG_VIDEO_FPS(30000),
												SET_MPEG_ASPECT_RATIO(256),
												SET_MPEG_STREAM_TYPE(multiplexed),
//												SET_MPEG_VIDEOSTANDARD(VSTD_NTSC),
												SET_MPEG2_PRESENTATION_MODE(MPM_FULLSIZE),

												SET_MPEG_AUDIO_BITRATE(300000),
												SET_MPEG_AUDIO_SAMPLERATE(48000),
												SET_MPEG_AUDIO_LAYER(MPAUDLAYER_1),
												SET_MPEG2_AUDIO_AC3(TRUE),
												SET_MPEG2_AUDIO_LPCM(FALSE),

												SET_MPEG2_LPCM_BITSPERSAMPLE(16),
												SET_MPEG2_LPCM_CHANNELS(2),

												SET_MPEG2_POSITION_SCALE(MP2SR_SCALE_DVDPES),
												SET_MPEG2_STILL_FRAME_SEQUENCE(stillFrameSequence),
												SET_MPEG2_AUDIO_TYPE(MP2AUDTYP_AC3),
												TAGDONE);

	scanning = FALSE;
	trickplay = FALSE;

	timeBaseValid = FALSE;
	lastDecoderTime = 0;
	}

//
//  Destructor
//

DVDPlayer::~DVDPlayer(void)
	{
	// Restore default settings for security critical tags
	VDR_ConfigureUnitsTags(units, SET_ENC_COPY_PROTECTION(0),
											SET_MPEG2_SPDIF_COPY_MODE(SPDIFCM_DEFAULT),
											SET_ENC_COPY_MODE(VCPMD_DEFAULT),
											SET_MPEG2_DVD_STREAM_ENCRYPTED(FALSE),
											TAGDONE);

	VDR_PassivateUnits(decUnits);

	VDR_RemoveMessagePort(units);

	VDR_CloseUnits(decUnits);

	TerminateWorker();
	}

//
//  Terminate playback
//

void DVDPlayer::TerminatePlayback(void)
	{
	VDR_ConfigureUnitsTags(units, SET_ENC_COPY_PROTECTION(0), TAGDONE);

	VDR_PassivateUnits(decUnits);
	}

//
//  Request refill
//

void DVDPlayer::RequestRefill(void)
	{
	SendWorkMessage(REFILL_MSG, 0);
	}

void DVDPlayer::RequestCallback(DWORD position)
	{
	SendWorkMessage(SIGNAL_MSG, position);
	}

//
//  Process message
//

void DVDPlayer::WorkMessage(WPARAM wParam, LPARAM dParam)
	{
	DVDStreamServer * cb = callback;

//	DP("Msg %d %d", wParam, dParam);

	switch (wParam)
		{
		case REFILL_MSG:
//			DP("Refill Request");
			// Latencies are improved when executing this before PerformTransfer
			VDR_CompleteMPEGRefillMessage(units);

			if (refiller)
				refiller->PerformTransfer(32);
			break;
		case SIGNAL_MSG:
//			DP("Get Callback %d %d", dParam, transferCount);
			if (cb)
				{
//				DP("Has cb %d %d", dParam, transferCount);
				inCallback = TRUE;
				if (dParam < (LONG)transferCount || !expectingDone && dParam == (LONG)transferCount)
					{
//					DP("dParam < transferCount");
					if (dParam >= (LONG)callbackPosition)
						{
//						DP("dParam >= callbackPosition");
						callback = NULL;
						cb->PlayerCallback(dParam);
						}
					}
				inCallback = FALSE;
				}
			break;
		case DONE_MSG:
//			DP("Got done message");
			if (cb && expectingDone)
				{
				inCallback = TRUE;
//				DP("As expected");
				callback = NULL;
				cb->PlayerCallback(0xffffffff);
				inCallback = FALSE;
				}
			else
				{
//				DP("Done Message Unexpected");
				}
			break;
		case ERROR_MSG:
			DP("Error Msg %08lx", dParam);
			SendEvent(DNE_ERROR, (DWORD)dParam);
			break;
		}
	}

//
//  Pause Playback
//

Error DVDPlayer::Pause(void)
	{
	MPEGState state;

	if (!paused)
		{
		state = VDR_CurrentMPEGState(units);
		if (state == mps_playing || state == mps_initial || state == mps_resyncing)
			{
//			GNREASSERT(VDR_SendMPEGCommand(units, mpc_freeze, 0, tag));
// Use DoMPEGCommand to prevent racing condition on hardware solutions like 5505
// (for example, when next command is a "set playback speed" command). The decoder
// may not yet have made the transition to the mps_frozen state.
			GNREASSERT(VDR_DoMPEGCommand(units, mpc_freeze, 0));
			}

		paused = TRUE;
		}

	GNRAISE_OK;
	}

//
//  Resume Playback
//

Error DVDPlayer::Resume(void)
	{
	DWORD tag;

	if (paused)
		{
		if (VDR_CurrentMPEGState(units) == mps_frozen)
			{
			GNREASSERT(VDR_SendMPEGCommand(units, mpc_play, playbackSpeed, tag));
			}

		paused = FALSE;
		}

	GNRAISE_OK;
	}

//
//  Single Step
//

Error DVDPlayer::Step(void)
	{
	DWORD tag;
	MPEGState state = VDR_CurrentMPEGState(units);

	if (state == mps_frozen || state == mps_trickplaying)
		return VDR_SendMPEGCommand(units, mpc_step, 1, tag);
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);
	}

//
//  Step by amount
//

Error DVDPlayer::StepBy(int val)
	{
	DWORD tag;

	if (VDR_CurrentMPEGState(units) == mps_trickplaying)
		return VDR_SendMPEGCommand(units, mpc_step, val, tag);
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);
	}

//
//  Set playback speed
//

Error DVDPlayer::SetPlaybackSpeed(WORD speed)
	{
	DWORD tag;

	if (speed > 2000) speed = 2000;
	else if (speed < 50) speed = 50;

	if (speed != playbackSpeed)
		{
		playbackSpeed = speed;
		SendEvent(DNE_PLAYBACK_SPEED_CHANGE, playbackSpeed);

		if (VDR_CurrentMPEGState(units) == mps_playing ||
			 VDR_CurrentMPEGState(units) == mps_resyncing)
			{
			return VDR_SendMPEGCommand(units, mpc_play, playbackSpeed, tag);
			}
		else
			GNRAISE_OK;
		}
	else
		GNRAISE_OK;
	}

//
//  Test if decoder is paused
//

BOOL DVDPlayer::IsPaused(void)
	{
	MPEGState state = VDR_CurrentMPEGState(units);
	return  state == mps_frozen || state == mps_stepping;
	}

//
//  Test if decoder is preempted
//

BOOL DVDPlayer::IsPreempted(void)
	{
	return VDR_CurrentMPEGState(units) == mps_preempted;
	}

//
//  Set audio attributes
//

inline void DVDPlayer::ExtractAudioAttributes(WORD audioAttr, MPEG2AudioType & type, WORD & channels,
															 DWORD & audioBitsPerSecond, DWORD & samplesPerSecond, DWORD &bitsPerSample)
	{
	channels = (WORD)XTBF(0, 3, audioAttr) + 1;

	switch (XTBF(13, 3, audioAttr))
		{
		case 0:	// AC3
			type = MP2AUDTYP_AC3;
			samplesPerSecond = 48000;
			audioBitsPerSecond = 384000 * (DWORD)channels / 6;
			if (audioBitsPerSecond < 192000)
				audioBitsPerSecond = 192000;
			bitsPerSample = 16;
			break;
		case 2:	// MPEG1 or MPEG-2 without extension stream
			if (channels > 2)
				{
				type = MP2AUDTYP_MPEG2;
				audioBitsPerSecond = 500000;
				}
			else
				{
				type = MP2AUDTYP_MPEG;
				audioBitsPerSecond = 200000;
				}
			bitsPerSample = 16;
			samplesPerSecond = 48000;
			break;
		case 3:	// MPEG2
			type = MP2AUDTYP_MPEG2;
			samplesPerSecond = 48000;
			audioBitsPerSecond = 500000;
			bitsPerSample = 16;
			break;
		case 4:	// LPCM
			type = MP2AUDTYP_LPCM;
			samplesPerSecond = XTBF(4, 2, audioAttr) ? 96000 : 48000;
			bitsPerSample = XTBF(6, 2, audioAttr) * 4 + 16;
			audioBitsPerSecond = samplesPerSecond * bitsPerSample * channels;
			break;
		case 6:	// DTS
			type = MP2AUDTYP_DTS;
			samplesPerSecond = 48000;
			bitsPerSample = 16;
			audioBitsPerSecond = 768000;
			break;
		case 7:	// SDDS
			type = MP2AUDTYP_SDDS;
			samplesPerSecond = 48000;
			bitsPerSample = 16;
			audioBitsPerSecond = 768000;
			break;
		}
	}

//
//  Set audio stream attributes
//

Error DVDPlayer::SetAudioStreamAttributes(WORD audioStreamID, WORD audioAttr)
	{
	MPEG2AudioType	type;
	WORD				channels;
	DWORD				audioBitsPerSecond;
	DWORD				samplesPerSecond;
	DWORD				bitsPerSample;
	DWORD				tag;
	Error				error;
	VDAutoMutex		mutex(&lock);

	this->audioStreamID = audioStreamID;

	if (!expectingDone)
		{
		if (this->audioAttr != audioAttr || (this->audioStreamID == 0x0f && audioStreamID != 0x0f))
			{
			//
			//  Attributes have changed/audio stream has been activated => full change
			//

			ExtractAudioAttributes(audioAttr, type, channels, audioBitsPerSecond, samplesPerSecond, bitsPerSample);

			GNREASSERT(VDR_DoMPEGCommand(decUnits, mpc_seekaudio, 0));
			if (audioStreamID != 0x0f)
				{
				error = VDR_ConfigureUnitsTags(decUnits,  SET_MPEG_AUDIO_STREAMID(0xc0 | audioStreamID),
																			SET_MPEG2_AUDIO_AC3_STREAMID(0x80 | audioStreamID),
																			SET_MPEG2_AUDIO_LPCM_STREAMID(0xa0 | audioStreamID),
																			SET_MPEG2_AUDIO_DTS_STREAMID(0x88 | audioStreamID),
																			SET_MPEG_AUDIO_BITRATE(audioBitsPerSecond),
																			SET_MPEG_AUDIO_SAMPLERATE((WORD)samplesPerSecond),
																			SET_MPEG2_AUDIO_AC3((BOOL)(type == MP2AUDTYP_AC3)),
																			SET_MPEG2_AUDIO_LPCM((BOOL)(type == MP2AUDTYP_LPCM)),
																			SET_MPEG2_LPCM_BITSPERSAMPLE((WORD)bitsPerSample),
																			SET_MPEG2_LPCM_CHANNELS(channels),
																			SET_MPEG2_AUDIO_TYPE(type),
																			TAGDONE);
				}
			GNREASSERT(VDR_SendMPEGCommand(decUnits, mpc_resyncaudio, 0, tag));
			}
		else
			{
			//
			//  Only stream id has changed => partial change
			//

			GNREASSERT(VDR_DoMPEGCommand(decUnits, mpc_seekaudio, 0));
			if (audioStreamID != 0x0f)
				{
				error = VDR_ConfigureUnitsTags(decUnits,  SET_MPEG_AUDIO_STREAMID(0xc0 | audioStreamID),
																			SET_MPEG2_AUDIO_AC3_STREAMID(0x80 | audioStreamID),
																			SET_MPEG2_AUDIO_LPCM_STREAMID(0xa0 | audioStreamID),
																			SET_MPEG2_AUDIO_DTS_STREAMID(0x88 | audioStreamID),
																			TAGDONE);
				}
			GNREASSERT(VDR_SendMPEGCommand(decUnits, mpc_resyncaudio, 0, tag));
			}
		}

	this->audioAttr = audioAttr;
	GNRAISE_OK;
	}

//
//  Get information about audio decoder status and content
//

Error DVDPlayer::GetAudioInformation(MPEG2PrologicStatus & prologicStatus, MPEG2LFEStatus & lfeStatus, AC3AudioCodingMode & ac3Mode)
	{
	BOOL proStat;
	BOOL lfeStat;
	BOOL tagSupported;

	//
	//  Get prologic status
	//

	prologicStatus = MP2PS_UNKNOWN;
	if (!IS_ERROR(VDR_ConfigureUnitsTags(decUnits, QRY_MPEG2_PROLOGIC_STATUS(tagSupported), TAGDONE)))
		{
		if (tagSupported)
			{
			if (!IS_ERROR(VDR_ConfigureUnitsTags(decUnits, GET_MPEG2_PROLOGIC_STATUS(proStat), TAGDONE)))
				prologicStatus = proStat ? MP2PS_ON : MP2PS_OFF;
			}
		}

	//
	//  Get LFE status
	//

	lfeStatus = MP2LFE_UNKNOWN;
	if (!IS_ERROR(VDR_ConfigureUnitsTags(decUnits, QRY_MPEG2_LFE_STATUS(tagSupported), TAGDONE)))
		{
		if (tagSupported)
			{
			if (!IS_ERROR(VDR_ConfigureUnitsTags(decUnits, GET_MPEG2_LFE_STATUS(lfeStat), TAGDONE)))
				lfeStatus = lfeStat ? MP2LFE_ON : MP2LFE_OFF;
			}
		}

	//
	//  Get AC3 coding mode
	//

	ac3Mode = AC3ACMOD_UNKNOWN;
	if (!IS_ERROR(VDR_ConfigureUnitsTags(decUnits, QRY_MPEG2_AC3_AUDIO_CODING_MODE(tagSupported), TAGDONE)))
		{
		if (tagSupported)
			{
			if (IS_ERROR(VDR_ConfigureUnitsTags(decUnits, GET_MPEG2_AC3_AUDIO_CODING_MODE(ac3Mode), TAGDONE)))
				ac3Mode = AC3ACMOD_UNKNOWN;
			// If no error occurred ac3Mode already has the correct value
			}
		}

	GNRAISE_OK;
	}

//
//  Set subpicture stream attributes
//

Error DVDPlayer::SetSubPictureStreamAttributes(WORD subPictureStreamID, WORD subPictureAttr)
	{
	this->subPictureStreamID = subPictureStreamID;

	return VDR_ConfigureUnitsTags(units, SET_MPEG2_SPU_STREAMID(0x20 | (subPictureStreamID & 0x3f)),
 													 SET_MPEG2_SPU_ENABLE(XTBF(6, subPictureStreamID)),
													 TAGDONE);
	}

//
//  Set stream attributes
//

Error DVDPlayer::SetStreamAttributes(WORD videoAttr,
                                     WORD audioStreamID, WORD audioAttr,
                                     WORD subPictureStreamID, WORD subPictureAttr,
		                          		 DisplayPresentationMode mode)
	{
	DWORD tag;
	WORD width, height, framesPerSecond, aspectRatio, channels;
	MPEG2AudioType type;
	DWORD audioBitsPerSecond, samplesPerSecond, bitsPerSample;
	VideoStandard newVideoStandard;
	MPEG2PresentationMode	m2mode;
	ForcedAspectRatio forcedRatio;

//	DP("################################################");
//	DP("Change attributes AS %d WAS %d", audioStreamID, this->audioStreamID);

	if (videoAttr != this->videoAttr ||
	    audioAttr != this->audioAttr ||
	    mode != presentationMode ||
		 this->audioStreamID == 0x0f && audioStreamID != 0x0f)
		{
//		DP("Full change");

		this->videoAttr = videoAttr;
		this->audioAttr = audioAttr;
		this->audioStreamID = audioStreamID;
		this->subPictureStreamID = subPictureStreamID;

		//
		//  Set video playback data
		//

		presentationMode = mode;

		switch (XTBF(12, 2, videoAttr))
			{
			case 0:
				height = 480;
				framesPerSecond = 30;
				newVideoStandard = VSTD_NTSC;
				break;
			case 1:
				height = 576;
				framesPerSecond = 25;
				newVideoStandard = VSTD_PAL;
				break;
			}

		if (videoStandard != newVideoStandard)
			SendEvent(DNE_VIDEO_STANDARD_CHANGE, videoStandard);
		videoStandard = newVideoStandard;

		switch (XTBF(3, 3, videoAttr))
			{
			case 0:
				width = 720;
				break;
			case 1:
				width = 704;
				break;
			case 2:
				width = 352;
				break;
			case 3:
				width = 352;
				height /= 2;
				break;
			default:
				width = 720;
				break;
			}

		aspectRatio = ScaleWord(0x0155, width, height);

		switch (XTBF(10, 2, videoAttr))
			{
			case 0:
				forcedRatio = FORCED_AR_4_BY_3;
				break;
			case 3:
				forcedRatio = FORCED_AR_16_BY_9;
				break;
			default:
				forcedRatio = FORCED_AR_DEFAULT;
			}

		switch (mode)
			{
			case DPM_4BY3:
				m2mode = MPM_FULLSIZE;
				break;
			case DPM_16BY9:
				m2mode = MPM_FULLSIZE16by9;
				break;
			case DPM_PANSCAN:
				m2mode = MPM_PANSCAN;
				break;
			case DPM_LETTERBOXED:
				m2mode = MPM_LETTERBOXED;
				break;
			}

		//
		//  Get audio attributes
		//

		ExtractAudioAttributes(audioAttr, type, channels, audioBitsPerSecond, samplesPerSecond, bitsPerSample);

		//
		//  Pass new data to decoder
		//

		GNREASSERT(VDR_PassivateUnits(decUnits));
		GNREASSERT(VDR_ConfigureUnitsTags(units, SET_PIP_VIDEOSTANDARD(videoStandard),
			                                      SET_VID_VIDEOSTANDARD(videoStandard),
															  TAGDONE));

		if (audioStreamID != 0x0f)
			{
			GNREASSERT(VDR_ConfigureUnitsTags(decUnits,  SET_MPEG_AUDIO_STREAMID(0xc0 | audioStreamID),
																		SET_MPEG2_AUDIO_AC3_STREAMID(0x80 | audioStreamID),
																		SET_MPEG2_AUDIO_LPCM_STREAMID(0xa0 | audioStreamID),
																		SET_MPEG2_AUDIO_DTS_STREAMID(0x88 | audioStreamID),
																		SET_MPEG_AUDIO_BITRATE(audioBitsPerSecond),
																		SET_MPEG_AUDIO_SAMPLERATE((WORD)samplesPerSecond),
																		SET_MPEG2_AUDIO_AC3((BOOL)(type == MP2AUDTYP_AC3)),
																		SET_MPEG2_AUDIO_LPCM((BOOL)(type == MP2AUDTYP_LPCM)),

																		SET_MPEG2_LPCM_BITSPERSAMPLE((WORD)bitsPerSample),
																		SET_MPEG2_LPCM_CHANNELS(channels),
																		SET_MPEG2_AUDIO_TYPE(type),
																		TAGDONE));
			}


#if TEST_PAL_TO_NTSC

		GNREASSERT(VDR_ConfigureUnitsTags(decUnits,  SET_MPEG2_SPU_STREAMID(0x20 | (subPictureStreamID & 0x3f)),
 																	SET_MPEG2_SPU_ENABLE(XTBF(6, subPictureStreamID)),

																	SET_MPEG_VIDEO_STREAMID(0xe0),
																	SET_MPEG_VIDEO_BITRATE(5000000),
																	SET_MPEG_STREAM_BITRATE(7000000),

 																	SET_MPEG_VIDEO_WIDTH(width),
 																	SET_MPEG_VIDEO_HEIGHT(height),
 																	SET_MPEG_VIDEO_FPS(framesPerSecond),
 																	SET_MPEG_ASPECT_RATIO(aspectRatio),

																	SET_MPEG_VIDEOSTANDARD(videoStandard),
																	SET_MPEG_APP_VIDEOSTANDARD(videoStandard),

 																	SET_MPEG2_PRESENTATION_MODE(m2mode),

																	SET_MPEG_SIGNAL_POSITION(0),
																	SET_MPEG2_FORCED_SOURCE_ASPECT_RATIO(forcedRatio),

																	TAGDONE));

#else

		GNREASSERT(VDR_ConfigureUnitsTags(decUnits,  SET_MPEG2_SPU_STREAMID(0x20 | (subPictureStreamID & 0x3f)),
 																	SET_MPEG2_SPU_ENABLE(XTBF(6, subPictureStreamID)),

																	SET_MPEG_VIDEO_STREAMID(0xe0),
																	SET_MPEG_VIDEO_BITRATE(5000000),
																	SET_MPEG_STREAM_BITRATE(7000000),

 																	SET_MPEG_VIDEO_WIDTH(width),
 																	SET_MPEG_VIDEO_HEIGHT(height),
 																	SET_MPEG_VIDEO_FPS(framesPerSecond),
 																	SET_MPEG_ASPECT_RATIO(aspectRatio),
 																	SET_MPEG_VIDEOSTANDARD(videoStandard),
 																	SET_MPEG2_PRESENTATION_MODE(m2mode),

																	SET_MPEG_SIGNAL_POSITION(0),
																	SET_MPEG2_FORCED_SOURCE_ASPECT_RATIO(forcedRatio),

																	TAGDONE));
#endif

		transferCount = 0;
		transferTotal = 0;
		transferCountRemainder = 0;
//		GNREASSERT(VDR_DoMPEGCommand(units, mpc_seek, 0));
		PerformSeek();
		GNREASSERT(VDR_SendMPEGCommand(units, backward ? mpc_reverse : mpc_resync, 0, tag));
		}
	else
		{
//		DP("Partial change");

		this->audioStreamID = audioStreamID;
		this->subPictureStreamID = subPictureStreamID;

		if (audioStreamID != 0x0f)
			{
			GNREASSERT(VDR_ConfigureUnitsTags(decUnits,  SET_MPEG_AUDIO_STREAMID(0xc0 | audioStreamID),
																		SET_MPEG2_AUDIO_AC3_STREAMID(0x80 | audioStreamID),
																		SET_MPEG2_AUDIO_LPCM_STREAMID(0xa0 | audioStreamID),
																		SET_MPEG2_AUDIO_DTS_STREAMID(0x88 | audioStreamID),
																		TAGDONE));
			}

		GNREASSERT(VDR_ConfigureUnitsTags(units,  SET_MPEG2_SPU_STREAMID(0x20 | (subPictureStreamID & 0x3f)),
 																SET_MPEG2_SPU_ENABLE(XTBF(6, subPictureStreamID)),
																TAGDONE));

		}

	GNREASSERT(VDR_ActivateUnits(units));
	expectingDone = FALSE;

	GNRAISE_OK;
	}

//
//  Set Still Frame Sequence Mode
//

void DVDPlayer::SetStillFrameSequenceMode(BOOL stillFrameSequence)
	{
	DWORD tag;

	if (stillFrameSequence != this->stillFrameSequence)
		{
		this->stillFrameSequence = stillFrameSequence;

		VDR_ConfigureUnitsTags(units,  SET_MPEG2_STILL_FRAME_SEQUENCE(stillFrameSequence),
												TAGDONE);

		transferCount = 0;
		transferTotal = 0;
		transferCountRemainder = 0;
		PerformSeek();
		VDR_SendMPEGCommand(units, backward ? mpc_reverse : mpc_resync, 0, tag);
		}
	}

//
//  Set video standard
//

Error DVDPlayer::SetVideoStandard(WORD videoAttr)
	{
	VideoStandard newVideoStandard;

	if (videoAttr != this->videoAttr)
		{
		switch (XTBF(12, 2, videoAttr))
			{
			case 0:
				newVideoStandard = VSTD_NTSC;
				break;
			case 1:
				newVideoStandard = VSTD_PAL;
				break;
			}

		if (videoStandard != newVideoStandard)
			SendEvent(DNE_VIDEO_STANDARD_CHANGE, videoStandard);
		videoStandard = newVideoStandard;

		GNREASSERT(VDR_PassivateUnits(decUnits));
		GNREASSERT(VDR_ConfigureUnitsTags(units, SET_PIP_VIDEOSTANDARD(videoStandard),
			                                      SET_VID_VIDEOSTANDARD(videoStandard),
															  SET_MPEG_VIDEOSTANDARD(videoStandard),
															  TAGDONE));
		}

	GNRAISE_OK;
	}

//
//  Set subpicture palette
//

Error DVDPlayer::SetSubPicturePalette(int entry, DWORD color)
	{
	return VDR_ConfigureUnitsTags(units, SET_MPEG2_SPU_PALETTE_ENTRY(color | entry), TAGDONE);
	}

//
//  Set analog copy protection mode
//

Error DVDPlayer::SetAnalogCopyProtectionMode(int mode)
	{
//	DP("Mode : %d", mode);

	return VDR_ConfigureUnitsTags(units, SET_ENC_COPY_PROTECTION(mode), TAGDONE);
	}

//
//  Set digital out copy managment mode
//

Error DVDPlayer::SetCopyManagementMode(GenericCopyManagementInfo mode)
	{
	SPDIFCopyMode spdifCopyMode;
	VideoCopyMode vidCopyMode;

	switch (mode)
		{
		case GCMI_NO_RESTRICTION :
			spdifCopyMode	= SPDIFCM_UNRESTRICTED;
			vidCopyMode		= VCPMD_COPYING_PERMITTED;
			break;

		case GCMI_ONE_GENERATION :
			spdifCopyMode	= SPDIFCM_ONE_GENERATION;
			vidCopyMode		= VCPMD_ONE_COPY_PERMITTED;
			break;

		case GCMI_COPY_FORBIDDEN :
			spdifCopyMode	= SPDIFCM_NO_COPIES ;
			vidCopyMode		= VCPMD_NO_COPYING_PERMITTED;
			break;

		default :
			spdifCopyMode	= SPDIFCM_NO_COPIES;
			vidCopyMode		= VCPMD_DEFAULT;
			break;
		}

	GNRAISE(VDR_ConfigureUnitsTags(units,
											 SET_MPEG2_SPDIF_COPY_MODE(spdifCopyMode),
											 SET_ENC_COPY_MODE(vidCopyMode),
											 TAGDONE));
	}

//
//  Send data multiple
//

DWORD DVDPlayer::SendDataMultiple(MPEGDataSizePair * data, DWORD size)
	{
	DWORD tag;
	DWORD done;
	MPEGState state;

	lock.Enter();

	expectingDone = FALSE;

#if MEASURE_TRANSFER_RATE
int time;
time = Timer.GetMicroTicks();
#endif

	switch (state = VDR_CurrentMPEGState(units))
		{
		case mps_reset:
		case mps_preempted:
			lock.Leave();
			return 0;
			break;
		case mps_stopped:
			FlushWorkMessages();
			VDR_CompleteMPEGRefillMessage(units);
			PerformSeek(); //	Includes: VDR_DoMPEGCommand(units, mpc_seek, 0);
			if (scanning)
				VDR_SendMPEGCommand(units, mpc_scan, 0, tag);
			else if (trickplay)
				VDR_SendMPEGCommand(units, mpc_trickplay, 0, tag);
			else if (backward)
				VDR_SendMPEGCommand(units, mpc_reverse, 0, tag);
			else
				VDR_SendMPEGCommand(units, mpc_resync, 0, tag);
			transferCount = 0;
			transferTotal = 0;
			transferCountRemainder = 0;
		case mps_seeking:
			done = VDR_SendMPEGDataMultiple(units, data, size);
			break;
		case mps_playing:
		case mps_resyncing:
		case mps_initial:
		case mps_frozen:
		case mps_scanning:
		case mps_stepping:
		case mps_trickplaying:
			done = VDR_SendMPEGDataMultiple(units, data, size);
			break;
		default:
			done = 0;
		}

#if MEASURE_TRANSFER_RATE
time = Timer.GetMicroTicks() - time;
static int timeSum;
static int dataSum;

timeSum += time;
dataSum += done;

if (timeSum >= 1000000)
	{
	printf("Write KBytes per sec %d\n", ScaleDWord(dataSum, timeSum, 1000));
	timeSum = dataSum = 0;
	}
#endif

	transferTotal += done;
//#if ST20LITE
	KernelInt64 temp = transferTotal >> 11;
	transferCount = temp.ToDWORD();
	temp = transferTotal & (DVD_BLOCK_SIZE - 1);
	transferCountRemainder = temp.ToDWORD();

	lock.Leave();

	return done;
	}

//
//  Send data
//

DWORD DVDPlayer::SendData(HBPTR data, DWORD size)
	{
	DWORD tag;
	DWORD done;
	MPEGState state;

	lock.Enter();

	expectingDone = FALSE;

#if MEASURE_TRANSFER_RATE
int time;
time = Timer.GetMicroTicks();
#endif

	switch (state = VDR_CurrentMPEGState(units))
		{
		case mps_reset:
		case mps_preempted:
			lock.Leave();
			return 0;
			break;
		case mps_stopped:
			FlushWorkMessages();
			VDR_CompleteMPEGRefillMessage(units);
			PerformSeek(); //	Includes: VDR_DoMPEGCommand(units, mpc_seek, 0);
			if (scanning)
				VDR_SendMPEGCommand(units, mpc_scan, 0, tag);
			else if (trickplay)
				VDR_SendMPEGCommand(units, mpc_trickplay, 0, tag);
			else if (backward)
				VDR_SendMPEGCommand(units, mpc_reverse, 0, tag);
			else
				VDR_SendMPEGCommand(units, mpc_resync, 0, tag);
			transferCount = 0;
			transferTotal = 0;
			transferCountRemainder = 0;
		case mps_seeking:
			done = VDR_SendMPEGData(units, data, size);
			break;
		case mps_playing:
		case mps_resyncing:
		case mps_initial:
		case mps_frozen:
		case mps_scanning:
		case mps_stepping:
		case mps_trickplaying:
			done = VDR_SendMPEGData(units, data, size);
			break;
		default:
			done = 0;
		}

#if MEASURE_TRANSFER_RATE
time = Timer.GetMicroTicks() - time;
static int timeSum;
static int dataSum;

timeSum += time;
dataSum += done;

if (timeSum >= 1000000)
	{
	printf("Write KBytes per sec %d\n", ScaleDWord(dataSum, timeSum, 1000));
	timeSum = dataSum = 0;
	}
#endif

	transferTotal += done;
//#if ST20LITE
	KernelInt64 temp = transferTotal >> 11;
	transferCount = temp.ToDWORD();
	temp = transferTotal & (DVD_BLOCK_SIZE - 1);
	transferCountRemainder = temp.ToDWORD();

	lock.Leave();

	return done;
	}

//
//  Complete data
//

void DVDPlayer::CompleteData(void)
	{
	lock.Enter();
//	DP("complete data");
	expectingDone = TRUE;
	VDR_CompleteMPEGData(units);
	lock.Leave();
	}

//
//  Cancel data
//

void DVDPlayer::CancelData(void)
	{
	DWORD tag;

	lock.Enter();

//	DP("cancel data");

	PerformSeek();
	VDR_SendMPEGCommand(units, backward ? mpc_reverse : mpc_resync, 0, tag);
	transferCount = 0;
	transferTotal = 0;
	transferCountRemainder = 0;
	Timer.WaitMilliSecs(20);
	FlushWorkMessages();
	VDR_CompleteMPEGRefillMessage(units);
	scanning = FALSE;
	trickplay = FALSE;
	expectingDone = FALSE;

	lock.Leave();
	}

//
//  Set callback
//

void DVDPlayer::SetCallback(DWORD position, DVDStreamServer * callback)
	{
	DWORD cp = VDR_CurrentMPEGLocation(units);
	MPEGState state;

	this->callback = NULL;

//	DP("Set Callback %d %d", position, cp);

	state = VDR_CurrentMPEGState(units);

	if ((state == mps_playing || state == mps_stepping) && position < cp)
		{
		callbackPosition = 0;
		VDR_ConfigureUnitsTags(units, SET_MPEG_SIGNAL_POSITION(0), TAGDONE);
		callback->PlayerCallback(cp);
		}
	else
		{
		callbackPosition = position;
		this->callback = callback;
		VDR_ConfigureUnitsTags(units, SET_MPEG_SIGNAL_POSITION(position), TAGDONE);
		cp = VDR_CurrentMPEGLocation(units);
		if (cp >= position)
			{
			RequestCallback(callbackPosition);
			VDR_ConfigureUnitsTags(units, SET_MPEG_SIGNAL_POSITION(0), TAGDONE);
			}
		}
	}

//
//  Cancel callback
//

void DVDPlayer::CancelCallback(void)
	{
	this->callback = 0;
	VDR_ConfigureUnitsTags(units, SET_MPEG_SIGNAL_POSITION(0), TAGDONE);
	}

//
//  Wait for callback canceled
//

void DVDPlayer::WaitForCallbackCanceled(void)
	{
	while (inCallback)
		{
		Timer.WaitMilliSecs(100);
		}
	}

//
//  Return current location
//

DWORD DVDPlayer::CurrentLocation(void)
	{
	return VDR_CurrentMPEGLocation(units);
	}

//
//  Test if decoder is done
//

BOOL DVDPlayer::Completed(void)
	{
	MPEGState state = VDR_CurrentMPEGState(units);

	return state == mps_stopped;
	}

//
//  Start Playback
//

void DVDPlayer::StartPlayback(void)
	{
	DWORD tag;

	lock.Enter();

	if (scanning || trickplay)
		{
		DP("Starting from scan or backward");
		scanning = FALSE;
		trickplay = FALSE;
		PerformSeek();
		VDR_SendMPEGCommand(units, backward ? mpc_reverse : mpc_resync, 0, tag);
		expectingDone = FALSE;
		}

	if (!paused)
		VDR_SendMPEGCommand(units, mpc_play, playbackSpeed, tag);

	lock.Leave();
	}

//
//  Start Scanning
//

void DVDPlayer::StartScan(void)
	{
	DWORD tag;

	lock.Enter();


	if (!scanning)
		{
		DP("Scanning from start");
		scanning = TRUE;
		trickplay = FALSE;
		PerformSeek();
		VDR_SendMPEGCommand(units, mpc_scan, 0, tag);
		expectingDone = FALSE;
		}

	lock.Leave();
	}

//
//  Start trickplay
//

void DVDPlayer::StartTrickplay(void)
	{
	DWORD tag;

	lock.Enter();

	if (!trickplay)
		{
		DP("Scanning from start");
		trickplay = TRUE;
		scanning = FALSE;
		PerformSeek();
		VDR_SendMPEGCommand(units, mpc_trickplay, 0, tag);
		expectingDone = FALSE;
		}

	lock.Leave();
	}

//
//  Start reverse playback
//

void DVDPlayer::SetReversePlayback(BOOL reverse)
	{
	DWORD tag;

	lock.Enter();

	if (reverse != backward)
		{
		backward = reverse;

		PerformSeek();
		VDR_SendMPEGCommand(units, backward ? mpc_reverse : mpc_resync, 0, tag);
		expectingDone = FALSE;
		}

	lock.Leave();
	}

//
//  Set button colors
//

Error DVDPlayer::SetButtonColors(DWORD select, DWORD active)
	{
	return VDR_ConfigureUnitsTags(units, SET_MPEG2_SPU_BUTTON_SELECT_COLOR(select),
	                                     SET_MPEG2_SPU_BUTTON_ACTIVE_COLOR(active),
	                                     TAGDONE);
	}

//
//  Set button position
//

Error DVDPlayer::SetButtonPosition(WORD group, WORD button, WORD x, WORD y, WORD w, WORD h)
	{
	DP("Set Button pos %d,%d - %d,%d", x, y, w, h);
	return VDR_ConfigureUnitsTags(units, SET_MPEG2_SPU_BUTTON_ID(MAKEWORD(button, group)),
		                                  SET_MPEG2_SPU_BUTTON_LEFT(x),
	                                     SET_MPEG2_SPU_BUTTON_TOP(y),
	                                     SET_MPEG2_SPU_BUTTON_WIDTH(w),
	                                     SET_MPEG2_SPU_BUTTON_HEIGHT(h),
	                                     TAGDONE);
	}

//
//  Set button off
//

Error DVDPlayer::SetButtonOff(void)
	{
	return VDR_ConfigureUnitsTags(units, SET_MPEG2_SPU_BUTTON_STATE(SBS_DISABLED), TAGDONE);
	}

//
//  Set button select
//

Error DVDPlayer::SetButtonSelect(void)
	{
	return VDR_ConfigureUnitsTags(units, SET_MPEG2_SPU_BUTTON_STATE(SBS_SELECTED), TAGDONE);
	}

//
//  Set button active
//

Error DVDPlayer::SetButtonActive(void)
	{
	return VDR_ConfigureUnitsTags(units, SET_MPEG2_SPU_BUTTON_STATE(SBS_ACTIVATED), TAGDONE);
	}

//
//  Transfer disk key
//

Error DVDPlayer::TransferDiskKey(DVDHeaderFile * file)
	{
	Error err;
	BYTE buffer[DVD_BLOCK_SIZE];
	int retry;

	GNREASSERT(VDR_LockUnits(units));

	for (retry = 0; retry < 3; retry++)
		{
		if (!IS_ERROR(VDR_ActivateUnits(units)))
			{
			if (!IS_ERROR(err = file->DoAuthenticationCommand(DAC_START_AUTHENTICATION, buffer)))
				{
				if (!IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_START_AUTHENTICATION, 0, buffer)))
					{
					if (!IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_READ_CHALLENGE_KEY, 0, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_WRITE_CHALLENGE_KEY, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_READ_BUS_KEY, buffer)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_WRITE_BUS_KEY, 0, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_READ_CHALLENGE_KEY, buffer)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_WRITE_CHALLENGE_KEY, 0, buffer)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_READ_BUS_KEY, 0, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_WRITE_BUS_KEY, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_READ_DISK_KEY, buffer)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_WRITE_DISK_KEY, 0, buffer)))
						{
						if (!IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_COMPLETE_AUTHENTICATION, 0, buffer)))
							{
							if (!IS_ERROR(err = file->DoAuthenticationCommand(DAC_COMPLETE_AUTHENTICATION, buffer)))
								{
								GNREASSERT(VDR_UnlockUnits(units));

								GNRAISE_OK;
								}
							else
								{
								file->DoAuthenticationCommand(DAC_CANCEL_AUTHENTICATION, buffer);

								GNREASSERT(VDR_UnlockUnits(units));

								GNRAISE(err);
								}
							}
						}
					}

				VDR_DoAuthenticationCommand(units, M2D_CANCEL_AUTHENTICATION, 0, buffer);
				}

			file->DoAuthenticationCommand(DAC_CANCEL_AUTHENTICATION, buffer);
			}

		VDR_PassivateUnits(units);
		}

	GNREASSERT(VDR_UnlockUnits(units));

	return err;
	}

//
//  Transfer title key
//

Error DVDPlayer::TransferTitleKey(DVDDataFile * file)
	{
	Error err;
	BYTE buffer[DVD_BLOCK_SIZE];
	DWORD sector;
	int retry;

	GNREASSERT(VDR_LockUnits(units));
	if (!IS_ERROR(err = VDR_ActivateUnits(units)))
		{
		GNREASSERT(VDR_ConfigureUnitsTags(units, SET_MPEG2_DVD_STREAM_ENCRYPTED(TRUE), TAGDONE));

		for (retry = 0; retry < 4; retry++)
			{
			if (!IS_ERROR(err = file->DoAuthenticationCommand(DAC_START_AUTHENTICATION, sector, buffer, retry)))
				{
				if (!IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_START_AUTHENTICATION, sector, buffer)))
					{
					if (!IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_READ_CHALLENGE_KEY, sector, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_WRITE_CHALLENGE_KEY, sector, buffer, retry)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_READ_BUS_KEY, sector, buffer, retry)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_WRITE_BUS_KEY, sector, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_READ_CHALLENGE_KEY, sector, buffer, retry)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_WRITE_CHALLENGE_KEY, sector, buffer)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_READ_BUS_KEY, sector, buffer)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_WRITE_BUS_KEY, sector, buffer, retry)) &&
						 !IS_ERROR(err = file->DoAuthenticationCommand(DAC_READ_TITLE_KEY, sector, buffer, retry)) &&
						 !IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_WRITE_TITLE_KEY, sector, buffer)))
						{
						if (!IS_ERROR(err = VDR_DoAuthenticationCommand(units, M2D_COMPLETE_AUTHENTICATION, sector, buffer)))
							{
							if (!IS_ERROR(err = file->DoAuthenticationCommand(DAC_COMPLETE_AUTHENTICATION, sector, buffer, retry)))
								{
								GNREASSERT(VDR_UnlockUnits(units));

								GNRAISE_OK;
								}
							else
								{
								file->DoAuthenticationCommand(DAC_CANCEL_AUTHENTICATION, sector, buffer, retry);

								GNREASSERT(VDR_UnlockUnits(units));

								GNRAISE(err);
								}
							}
						}
					}

				VDR_DoAuthenticationCommand(units, M2D_CANCEL_AUTHENTICATION, sector, buffer);
				}

			file->DoAuthenticationCommand(DAC_CANCEL_AUTHENTICATION, sector, buffer, retry);
			}

		VDR_PassivateUnits(units);
		}

	GNREASSERT(VDR_UnlockUnits(units));

	if (err == GNR_OBJECT_NOT_FOUND) err = GNR_OK;

	return err;
	}

Error DVDPlayer::CancelTitleKey(void)
	{
	BYTE buffer[DVD_BLOCK_SIZE];
	DWORD sector = 0;

	VDR_DoAuthenticationCommand(units, M2D_START_AUTHENTICATION, sector, buffer);
	VDR_DoAuthenticationCommand(units, M2D_CANCEL_AUTHENTICATION, sector, buffer);

	GNREASSERT(VDR_ConfigureUnitsTags(units,
												 SET_MPEG2_DVD_STREAM_ENCRYPTED(FALSE),
												 TAGDONE));

	GNRAISE_OK;
	}

//
//  Perform seek and invalidate timeBase
//

void DVDPlayer::PerformSeek(void)
	{
	Error err;
	LONG decoderTime;

	VDR_DoMPEGCommand(units, mpc_seek, 0);
	if (timeBaseValid)
		{
		if (!IS_ERROR(err = VDR_ConfigureUnitsTags(units, GET_MPEG2_CURRENT_PLAYBACK_TIME(decoderTime), TAGDONE)))
			lastDecoderTime = decoderTime;
		else
			lastDecoderTime = 0;
		}
	timeBaseValid = FALSE;
	}

//
//  Set time base
//

void DVDPlayer::SetTimeBase(DVDTime newTimeBase)
	{
	if (!timeBaseValid)
		{
		timeBase = newTimeBase;
		timeBaseValid = TRUE;
		}
	}
//
//  Get current playback time
//

Error DVDPlayer::GetPlaybackTime(DVDTime & playbackTime)
	{
	LONG decoderTime;

	if (timeBaseValid)
		{
		GNREASSERT(VDR_ConfigureUnitsTags(units, GET_MPEG2_CURRENT_PLAYBACK_TIME(decoderTime), TAGDONE));
		playbackTime = timeBase + DVDTime(0, 0, 0, (decoderTime * timeBase.FrameRate()) / 1000,
													timeBase.FrameRate());
		}
	else
		{
		GNRAISE(GNR_OBJECT_INVALID);
		/*

		playbackTime = timeBase + DVDTime(0, 0, 0, (lastDecoderTime * timeBase.FrameRate()) / 1000,
													timeBase.FrameRate());
													*/
		}

	GNRAISE_OK;
	}

//
//  Get requested VOBU ID
//

DWORD DVDPlayer::GetRequestedVOBUID(void)
	{
	DWORD data;

	if (IS_ERROR(VDR_ConfigureUnitsTags(units, GET_MPEG_CURRENT_STREAM_SEGMENT(data), TAGDONE)))
		return 0;
	else
		return data;
	}

//
//  Set first available VOBU ID
//

void DVDPlayer::SetFirstAvailableVOBUID(DWORD id)
	{
	VDR_ConfigureUnitsTags(units, SET_MPEG_FIRST_STREAM_SEGMENT(id), TAGDONE);
	}

//
//  Set last available VOBU ID
//

void DVDPlayer::SetLastAvailableVOBUID(DWORD id)
	{
	VDR_ConfigureUnitsTags(units, SET_MPEG_LAST_STREAM_SEGMENT(id), TAGDONE);
	}


