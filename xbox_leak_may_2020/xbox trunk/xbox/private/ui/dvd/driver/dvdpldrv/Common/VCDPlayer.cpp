////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
//  VCD Player Class
//
////////////////////////////////////////////////////////////////////

#include "VCDPlayer.h"
#include "Library/Common/vddebug.h"

#define REFILL_MSG		0x1000
#define SIGNAL_MSG		0x1001
#define ERROR_MSG			0x1002
#define DONE_MSG			0x1003

//
//  Constructor
//

VCDPlayer::VCDPlayer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher) : WinPortWorkerThread(server, THREAD_PRIORITY_HIGHEST)
	, EventSender(pEventDispatcher)
	{
	this->units = units;
	}

//
//  Destructor
//

VCDPlayer::~VCDPlayer(void)
	{
	VDR_PassivateUnits(decUnits);

	VDR_RemoveMPEGWinHooks(units);

	VDR_CloseUnits(decUnits);

	TerminateWorker();
	}

//
//  Initialize
//

Error VCDPlayer::Init(void)
	{
	VDR_OpenSubUnits(units, MPEG_DECODER_UNIT, decUnits);

	callback = NULL;

	vidStandard = VCDVID_UNKNOWN;   // this value ensures complete computation of video attributes
	videoCodingStandard = VCDCDS_UNKNOWN;
	audioCodingStandard = VCDCDS_UNKNOWN;
	audioStreamID = 0;

	transferCount = 0;

	playbackSpeed = 1000;
	paused = FALSE;

	VDR_InstallMessagePort(units, GetPort());
	VDR_ConfigureUnitsTags(units, SET_MPEG2_REFILL_MSG(REFILL_MSG),
		                           SET_MPEG2_SIGNAL_MSG(SIGNAL_MSG),
											SET_MPEG2_DONE_MSG(DONE_MSG),
											SET_MPEG2_ERROR_MSG(ERROR_MSG),
											TAGDONE);

	VDR_ConfigureUnitsTags(units, //SET_MPEG_AUDIO_MUTE(FALSE),
	                              SET_MPEG2_DVD_STREAM_DEMUX(FALSE),
											SET_MPEG2_CODING_STANDARD(FALSE),
										   SET_ENC_COPY_PROTECTION(0),
	                              TAGDONE);

	VDR_ConfigureUnitsTags(decUnits, SET_MPEG2_SPU_ENABLE(FALSE),
												SET_MPEG2_DVD_STREAM_ENCRYPTED(FALSE),

												SET_MPEG_VIDEO_BITRATE(1151929),
												SET_MPEG_STREAM_BITRATE(1377600),
												SET_MPEG_VIDEO_STREAMID(0xe0),

 												SET_MPEG_VIDEO_WIDTH(352),
 												SET_MPEG_VIDEO_HEIGHT(240),
 												SET_MPEG_VIDEO_FPS(29970),
 												SET_MPEG_ASPECT_RATIO(256),
	                                 SET_MPEG_STREAM_TYPE(multiplexed),
 												SET_MPEG_VIDEOSTANDARD(VSTD_NTSC),
 												SET_MPEG2_PRESENTATION_MODE(MPM_FULLSIZE),

												SET_MPEG_AUDIO_BITRATE(175000),
												SET_MPEG_AUDIO_SAMPLERATE(44100),
												SET_MPEG_AUDIO_STREAMID(0xc0),
												SET_MPEG2_AUDIO_AC3(FALSE),
												SET_MPEG2_AUDIO_LPCM(FALSE),

												SET_MPEG2_LPCM_BITSPERSAMPLE(16),
												SET_MPEG2_LPCM_CHANNELS(2),

												SET_MPEG2_POSITION_SCALE(MP2SR_SCALE_BYTES),

												SET_MPEG2_AUDIO_TYPE(MP2AUDTYP_MPEG),
												SET_MPEG_AUDIO_LAYER(MPAUDLAYER_1),
												TAGDONE);

	scanning = FALSE;
	GNRAISE_OK;
	}

//
//  Terminate Playback
//

void VCDPlayer::TerminatePlayback(void)
	{
	VDR_PassivateUnits(decUnits);
	}

//
//  Request a refill
//

void VCDPlayer::RequestRefill(void)
	{
	SendWorkMessage(REFILL_MSG, 0);
	}

//
//  Process decoder messages
//

void VCDPlayer::WorkMessage(WPARAM wParam, LPARAM dParam)
	{
	VCDStreamServer * cb = callback;

	switch (wParam)
		{
		case REFILL_MSG:
			if (refiller)
				refiller->PerformTransfer(64);
			VDR_CompleteMPEGRefillMessage(units);
			break;
		case SIGNAL_MSG:
			DP("Get Callback %d %d", dParam, transferCount);
			if (cb)
				{
				if (dParam < (LPARAM)transferCount)
					{
					if (dParam >= (LPARAM)callbackPosition)
						{
						callback = NULL;
						cb->PlayerCallback(dParam);
						}
					}
				}
			break;
		case DONE_MSG:
			DP("Got done message");
			if (cb)
				{
				DP("As expected");
				callback = NULL;
				cb->PlayerCallback(0xffffffff);
				}
			else
				DP("Unexpected!!!!");
			break;
		}
	}

//
//  Pause playback
//

Error VCDPlayer::Pause(void)
	{
	if (!paused)
		{
		if (VDR_CurrentMPEGState(units) == mps_playing)
			{
//			GNREASSERT(VDR_SendMPEGCommand(units, mpc_freeze, 0, tag));
			GNREASSERT(VDR_DoMPEGCommand(units, mpc_freeze, 0));
			}

		// note that this could lead to an error! If we are not in a playing state
		// we don't send the pause command to the decoder but we set the paused flag
		// to TRUE!!! Probably it has to be modified (for now it doesn't seem to
		// produce an error). mst 07/05/2000
		paused = TRUE;
		}

	GNRAISE_OK;
	}

//
//  Resume playback
//

Error VCDPlayer::Resume(void)
	{
	if (paused)
		{
		if (VDR_CurrentMPEGState(units) == mps_frozen)
			{
			GNREASSERT(VDR_DoMPEGCommand(units, mpc_play, playbackSpeed));
			}

		paused = FALSE;
		}

	GNRAISE_OK;
	}

//
//  Step one frame
//

Error VCDPlayer::Step(void)
	{
	DWORD tag;

	if (VDR_CurrentMPEGState(units) == mps_frozen)
		return VDR_SendMPEGCommand(units, mpc_step, 1, tag);
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);
	}

//
//  Set playback speed
//

Error VCDPlayer::SetPlaybackSpeed(WORD speed)
	{
	if (speed > 2000) speed = 2000;
	else if (speed < 50) speed = 50;

	if (speed != playbackSpeed)
		{
		playbackSpeed = speed;
		SendEvent(DNE_PLAYBACK_SPEED_CHANGE, playbackSpeed);

		if (VDR_CurrentMPEGState(units) == mps_playing ||
			 VDR_CurrentMPEGState(units) == mps_resyncing)
			return VDR_DoMPEGCommand(units, mpc_play, playbackSpeed);
		else
			GNRAISE_OK;
		}
	else
		GNRAISE_OK;
	}

//
//  Test if player (decoder) is paused
//

BOOL VCDPlayer::IsPaused(void)
	{
	MPEGState state = VDR_CurrentMPEGState(units);
	return  state == mps_frozen || state == mps_stepping;
	}

//
//  Test if player (decoder) is preempted
//

BOOL VCDPlayer::IsPreempted(void)
	{
	return VDR_CurrentMPEGState(units) == mps_preempted;
	}

//
//   Test if decoder supports hires stills
//

BOOL VCDPlayer::SupportsHiResStills(void)
	{
	BOOL does = FALSE;

	GNREASSERT(VDR_ConfigureUnitsTags(units, GET_MPEG_SUPPORTS_HIRES_STILL(does), TAGDONE));

	return does;
	}

//
//  Set audio stream channel
//

Error VCDPlayer::SetAudioStreamChannel(WORD audStrID)
	{
	DWORD tag;
	Error error = GNR_OK;

	if ((audStrID == 0 || audStrID == 1) && (audStrID != audioStreamID))
		{
		audioStreamID = audStrID;

		GNREASSERT(VDR_DoMPEGCommand(units, mpc_seekaudio, 0));

		error = VDR_ConfigureUnitsTags(units, SET_MPEG_AUDIO_STREAMID(0xc0 | audioStreamID),
											TAGDONE);

		GNREASSERT(VDR_SendMPEGCommand(units, mpc_resyncaudio, 0, tag));
		}

	GNRAISE(error);
	}

//
//  Get audio stream channel
//

WORD VCDPlayer::GetAudioStreamChannel(void)
	{
	return audioStreamID;
	}

//
//  Set stream attributes
//

Error VCDPlayer::SetStreamAttributes(BOOL pal, BOOL mpeg1CodingStandard, BOOL mpeg1Audio)
	{
	DWORD tag;
	VCDVideoStandard vstd = pal ? VCDVID_PAL : VCDVID_NTSC;
	VCDCodingStandard cdstd = mpeg1CodingStandard ? VCDCDS_MPEG1 : VCDCDS_MPEG2;
	VCDCodingStandard audCodStd = mpeg1Audio ? VCDCDS_MPEG1 : VCDCDS_MPEG2;

	if (vstd != vidStandard || cdstd != videoCodingStandard || audCodStd != audioCodingStandard)
		{
		vidStandard = vstd;
		videoCodingStandard = cdstd;
		audioCodingStandard = audCodStd;

		if (vstd != vidStandard)
			SendEvent(DNE_VIDEO_STANDARD_CHANGE, pal ? VSTD_PAL : VSTD_NTSC);

		GNREASSERT(VDR_PassivateUnits(decUnits));

		if (mpeg1CodingStandard)
			{
			//
			// configure tags for Video-CD
			//

			VDR_ConfigureUnitsTags(units,
				                     SET_MPEG2_DVD_STREAM_DEMUX(FALSE),
											SET_MPEG2_CODING_STANDARD(FALSE),
							            TAGDONE);
			}
		else
			{
			//
			// configure tags for Super-Video CD
			//

			VDR_ConfigureUnitsTags(units,
											SET_MPEG2_DVD_STREAM_DEMUX(FALSE),
											SET_MPEG2_CODING_STANDARD(TRUE),
										   TAGDONE);
			}

		if (mpeg1Audio)
			{
			VDR_ConfigureUnitsTags(units, SET_MPEG_AUDIO_BITRATE(175000),
												SET_MPEG_AUDIO_SAMPLERATE(44100),
												SET_MPEG2_LPCM_BITSPERSAMPLE(16),
												SET_MPEG2_AUDIO_TYPE(MP2AUDTYP_MPEG),
												TAGDONE);
			}
		else	// MPEG-2 Audio
			{
			VDR_ConfigureUnitsTags(units, SET_MPEG_AUDIO_BITRATE(500000),
												SET_MPEG_AUDIO_SAMPLERATE(44100),
												SET_MPEG2_LPCM_BITSPERSAMPLE(16),
												SET_MPEG2_AUDIO_TYPE(MP2AUDTYP_MPEG2),
												TAGDONE);
			}

		if (pal)
			{
			GNREASSERT(VDR_ConfigureUnitsTags(units,
															SET_PIP_VIDEOSTANDARD(VSTD_PAL),
															SET_VID_VIDEOSTANDARD(VSTD_PAL),
															SET_MPEG_VIDEO_WIDTH(352),
															SET_MPEG_VIDEO_HEIGHT(288),
															SET_MPEG_VIDEO_FPS(25000),
															SET_MPEG_ASPECT_RATIO(0x111),
															SET_MPEG_VIDEOSTANDARD(VSTD_PAL),
															TAGDONE));
			}
		else
			{
			GNREASSERT(VDR_ConfigureUnitsTags(units,
															SET_PIP_VIDEOSTANDARD(VSTD_NTSC),
															SET_VID_VIDEOSTANDARD(VSTD_NTSC),
															SET_MPEG_VIDEO_WIDTH(352),
															SET_MPEG_VIDEO_HEIGHT(240),
															SET_MPEG_VIDEO_FPS(29970),
															SET_MPEG_ASPECT_RATIO(0x0e3),
															SET_MPEG_VIDEOSTANDARD(VSTD_NTSC),
															TAGDONE));
			}

		GNREASSERT(VDR_ConfigureUnitsTags(decUnits,  SET_MPEG_SIGNAL_POSITION(0),
																	TAGDONE));

		transferCount = 0;
		GNREASSERT(VDR_SendMPEGCommand(units, mpc_seek, 0, tag));
		GNREASSERT(VDR_SendMPEGCommand(units, mpc_resync, 0, tag));
		}

	GNREASSERT(VDR_ActivateUnits(units));

	GNRAISE_OK;
	}

//
//  Send data
//

DWORD VCDPlayer::SendData(HBPTR data, DWORD size)
	{
	DWORD tag;
	DWORD done;
	MPEGState state;

	lock.Enter();

	switch (state = VDR_CurrentMPEGState(units))
		{
		case mps_reset:
		case mps_preempted:
			lock.Leave();
			return 0;
		case mps_stopped:
			FlushWorkMessages();
			VDR_CompleteMPEGRefillMessage(units);
			VDR_DoMPEGCommand(units, mpc_seek, 0);
			if (scanning)
				VDR_SendMPEGCommand(units, mpc_scan, 0, tag);
			else
				VDR_SendMPEGCommand(units, mpc_resync, 0, tag);
			transferCount = 0;
		case mps_seeking:
			done = VDR_SendMPEGData(units, data, size);
			break;
		case mps_playing:
		case mps_resyncing:
		case mps_initial:
		case mps_frozen:
		case mps_scanning:
		case mps_stepping:
			done = VDR_SendMPEGData(units, data, size);
			break;
		default:
			done = 0;
		}

	transferCount+=done;

	lock.Leave();

	return done;
	}

DWORD VCDPlayer::SendDataMultiple(MPEGDataSizePair * data, DWORD size)
	{
	DWORD tag;
	DWORD done;
	MPEGState state;

	lock.Enter();

	switch (state = VDR_CurrentMPEGState(units))
		{
		case mps_reset:
		case mps_preempted:
			lock.Leave();
			return 0;
		case mps_stopped:
			FlushWorkMessages();
			VDR_CompleteMPEGRefillMessage(units);
			VDR_DoMPEGCommand(units, mpc_seek, 0);
			if (scanning)
				VDR_SendMPEGCommand(units, mpc_scan, 0, tag);
			else
				VDR_SendMPEGCommand(units, mpc_resync, 0, tag);
			transferCount = 0;
		case mps_seeking:
			done = VDR_SendMPEGDataMultiple(units, data, size);
			break;
		case mps_playing:
		case mps_resyncing:
		case mps_initial:
		case mps_frozen:
		case mps_scanning:
		case mps_stepping:
			done = VDR_SendMPEGDataMultiple(units, data, size);
			break;
		default:
			done = 0;
		}

	transferCount+=done;

	lock.Leave();

	return done;
	}

//
//  Complete data
//

void VCDPlayer::CompleteData(void)
	{
	lock.Enter();
	DP("complete data");
	VDR_CompleteMPEGData(units);
	lock.Leave();
	}

//
//  Cancel data
//

void VCDPlayer::CancelData(void)
	{
	DWORD tag;

	lock.Enter();

	DP("cancel data");

	VDR_DoMPEGCommand(units, mpc_seek, 0);
	VDR_SendMPEGCommand(units, mpc_resync, 0, tag);
	transferCount = 0;
	FlushWorkMessages();
	VDR_CompleteMPEGRefillMessage(units);
	scanning = FALSE;

	lock.Leave();
	}

//
//  Set callback
//

void VCDPlayer::SetCallback(DWORD position, VCDStreamServer * callback)
	{
	DWORD cp = VDR_CurrentMPEGLocation(units);
	MPEGState state;

	this->callback = NULL;

	DP("Set Callback %d %d", position, cp);

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
		}
	}

//
//  Cancel callback
//

void VCDPlayer::CancelCallback(void)
	{
	this->callback = 0;
	VDR_ConfigureUnitsTags(units, SET_MPEG_SIGNAL_POSITION(0), TAGDONE);
	}

//
//  Test if playback is completed
//

BOOL VCDPlayer::IsCompleted(void)
	{
	MPEGState state = VDR_CurrentMPEGState(units);

	return state == mps_stopped;
	}

//
//  Start playback
//

void VCDPlayer::StartPlayback(BOOL toPause)
	{
	DWORD tag;

	lock.Enter();

	if (scanning)
		{
		DP("Starting from scan");
		scanning = FALSE;
		VDR_DoMPEGCommand(units, mpc_seek, 0);
		VDR_SendMPEGCommand(units, mpc_resync, 0, tag);
		}

	if (!paused)
		VDR_DoMPEGCommand(units, mpc_play, playbackSpeed);

	lock.Leave();
	}

//
//  Start scan
//

void VCDPlayer::StartScan(void)
	{
	DWORD tag;

	lock.Enter();

	if (!scanning)
		{
		DP("Scanning from start");
		scanning = TRUE;
		VDR_DoMPEGCommand(units, mpc_seek, 0);
		VDR_SendMPEGCommand(units, mpc_scan, 0, tag);
		}

	lock.Leave();
	}

//
//  Translate button position
//

Error VCDPlayer::TranslateButtonPosition(int & x, int & y)
	{
	int w, h;

	w = h = 0;
	VDR_ConfigureUnitsTags(units, GET_PIP_SOURCE_WIDTH(w),
				                     GET_PIP_SOURCE_HEIGHT(h),
				                     TAGDONE);
	x = 256 * x / w;
	y = 256 * y / h;
	GNRAISE_OK;
	}

//
//  Get current location
//

DWORD VCDPlayer::CurrentLocation(void)
	{
	return VDR_CurrentMPEGLocation(units);
	}

//
//  Get information about audio decoder status and content
//

Error VCDPlayer::GetAudioInformation(MPEG2PrologicStatus & prologicStatus, MPEG2LFEStatus & lfeStatus, AC3AudioCodingMode & ac3Mode)
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
//  Get playback time
//

Error VCDPlayer::GetPlaybackTime(DVDTime & playbackTime)
	{
	LONG decoderTime;

	playbackTime = 0;

	GNREASSERT(VDR_ConfigureUnitsTags(units, GET_MPEG2_CURRENT_PLAYBACK_TIME(decoderTime), TAGDONE));

	playbackTime = DVDTime(0, 0, decoderTime / 1000, 0, GetCurrentFrameRate());

	GNRAISE_OK;
	}
