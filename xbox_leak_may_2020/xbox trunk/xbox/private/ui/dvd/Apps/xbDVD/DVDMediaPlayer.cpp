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

//////////////////////////////////////////////////////////////////////////////
//
//  $Workfile: DVDMediaPlayer.cpp $
//
//  Copyright (c) 1999  Quadrant International, Inc.
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Tproj/Apps/xbDVD/DVDMediaPlayer.cpp $
// $Author: Mpeteritas $
// $Modtime: 7/26/01 11:28a $
// $Date: 7/26/01 11:35a $
// $Revision: 62 $
//
////////////////////////////////////$NoKeywords: $////////////////////////////

#include "QError.h"
#include "QProperties.h"
#include "DVDMediaPlayer.h"
#include "langcode.h"
#include "xbDVDStrings.h"
#include <stdio.h>

int MessageBox(HWND hwnd, LPCTSTR s, LPCTSTR t, UINT i)
{
	return 0;
}

// module scope function for handling events sent by the DDP layer
void WINAPI DDPCaptureEvent(DWORD dwEvent, VOID* pvBuffer, DWORD dwInfo);

PEVENT_CALLBACK DVDMediaPlayer::m_pEventCallback = NULL;

HWND DVDMediaPlayer::m_eventWindow = NULL;
DVDMediaPlayer* pMediaPlayer = NULL;
char* DVD_PATH = "E";
char* DVD_FULL_PATH = "E";

DVDMediaPlayer::DVDMediaPlayer(HINSTANCE hinst = NULL) :
	m_ddpThePlayer(NULL), // this is a pointer to class CDVDDiskPlayer
	m_bFSInitMode(true), // only true once per instance.
	m_bSubPicOn(false),
	m_bFullScreenMode(false),
	m_iWindowLeft(0),
	m_iWindowTop(0),
	m_iWindowWidth(0),
	m_iWindowHeight(0),
	m_lWindowStyle(0L),
	m_lWindowStyleEx(0L),
	m_wCurrentTitle(0),
	m_wCurrentChapter(0),
	m_lCurrentVolume(50),
	m_iHasSPDIFSupport(0),
	m_bCheckedForPassthrough(false),
	m_dScanSpeed(1.0),       // Speed used during scan (2.0X and greater)
	m_dPlaySpeed(1.0),       // Speed used during playback (Less than 2.0X)
	m_bForwardPlayback(true),// Global so callback function can change its value
	m_lMode(QMODE_STOPPED),
	m_lCurrentDisplayMode(DPM_16BY9),
	m_dwCurrentUOPs(0),
	// Driver variables
	m_hVdrDriver(NULL),
	m_Units(NULL),
	m_hParentWindow(NULL),
	m_bAcceptParentalLevelChange(true),
	m_bChangeRatio(false),
	m_iDisplayMode(DPM_4BY3),
	m_PreferredAudioLanguage(0),
	m_PreferredSubpicLanguage(0)
{
	pMediaPlayer = this;

	m_currentRatio.cx = 4;
	m_currentRatio.cy = 3;

	m_rVideoRect.top = 0;
	m_rVideoRect.bottom = 0;
	m_rVideoRect.left = 0;
	m_rVideoRect.right = 0;

//wmpx	videoWindow = NULL;

	if(hinst)
		m_appInstance = hinst;
	else
		m_appInstance = NULL;
}



void DVDMediaPlayer::Shutdown()
{
	if( m_ddpThePlayer )
	{
		DDP_Stop(m_ddpThePlayer);
		EventHandlerRemove();
		DDP_ClosePlayer( m_ddpThePlayer );
		m_ddpThePlayer = NULL;
	}
	if ( m_Units )
	{
		VDR_CloseUnits( m_Units );
		m_Units = NULL;
	}
	if ( m_hVdrDriver )
	{
		VDR_CloseDriver( m_hVdrDriver );
		m_hVdrDriver = NULL;
	}
}


DVDMediaPlayer::~DVDMediaPlayer()
{


	//shutdown the ddp and vdr layers
	this->Shutdown();

	m_eventWindow = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//  Function: Initialize() -  initializes the decode and navigator engines for
//								playback of media type and creates a window for
//								display of the resulting decoded video.
// Return:    HRESULT  - S_OK or appropriate error code.
///////////////////////////////////////////////////////////////////////////////
//HRESULT DVDMediaPlayer::Initialize(HWND parent, char driveLetter, char* drivePath)
HRESULT DVDMediaPlayer::Initialize(HWND parent, char driveLetter, BYTE region)
{
	// wmp - hard code media string.


	if (IS_ERROR(DDP_Initialize()))
		{
		OutputDebugString("DDP_Initilize failed from DVDMediaPlayer!\n");
		return E_FAIL;
		}

	if (!m_hVdrDriver)
		{
		if (IS_ERROR(VDR_OpenDriver("CINEMAST", 0, m_hVdrDriver)))
			{
			OutputDebugString("Could not open Cinemaster!\n");
			return E_FAIL;
			}
		}

	if(!m_Units)
		{
			if (IS_ERROR(VDR_OpenUnits(m_hVdrDriver, MPEG_DECODER_UNIT | VIDEO_ENCODER_UNIT | PIP_UNIT, m_Units)))
			{
			OutputDebugString("Could not open units\n");
			return E_FAIL;
			}
		}
	//
	// Configure the lowlevel driver for our need
	//

	VDR_ConfigureUnitsTags(m_Units,
									SET_VID_MODE(VMOD_PLAYBACK),
									SET_PIP_VIDEOSTANDARD(VSTD_NTSC),
									SET_PIP_WINDOW(NULL),
									SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
									SET_PIP_ADAPT_CLIENT_SIZE(FALSE),
									SET_VID_VIDEOSTANDARD(VSTD_NTSC),
									TAGDONE);

	if(region != NULL)
	{
		SetSystemRegion(driveLetter, region);
	}

	if (!IS_ERROR(DDP_OpenPlayer(m_Units, driveLetter, m_ddpThePlayer)))
		return S_OK;

	else
	{
		OutputDebugString("Could not open player!\n");
		m_ddpThePlayer = NULL;
		return E_FAIL;
	}
	return S_OK;
}


HRESULT DVDMediaPlayer::CheckDrive(char driveLetter, char __far* id)
{
	return ConvertError(DDP_CheckDrive(driveLetter,id));
}

HRESULT DVDMediaPlayer::CheckDrive(char* drivePath, char __far* id)
{
	return ConvertError(DDP_CheckDriveWithPath(drivePath,id));
}

DVDPlayerMode DVDMediaPlayer::GetPlayerMode(void)
{
	if( !m_ddpThePlayer )
	{
		return DVDPlayerMode();
	}

	return DDP_GetPlayerMode(m_ddpThePlayer);
}

HRESULT DVDMediaPlayer::StartPresentation(void)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	return ConvertError(DDP_StartPresentation(m_ddpThePlayer));
}

DVDDiskType DVDMediaPlayer::GetDiskType(void)
{
	if( !m_ddpThePlayer )
	{
		return DVDDiskType();
	}

	return DDP_GetDiskType(m_ddpThePlayer);
}

HRESULT DVDMediaPlayer::ForwardScan(short speed)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	return ConvertError(DDP_ForwardScan(m_ddpThePlayer,speed));
}

HRESULT DVDMediaPlayer::BackwardScan(short speed)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	return ConvertError(DDP_BackwardScan(m_ddpThePlayer,speed));
}

HRESULT DVDMediaPlayer::Get_VideoSize(WORD& x, WORD& y)
{
	ULONG ulError;
	HRESULT hr;
	unsigned short          iSourceHeight;
	unsigned short          iSourceWidth;


	ulError = VDR_ConfigureUnitsTags(m_Units, GET_MPEG_DISPLAY_HEIGHT(iSourceHeight), TAGDONE);
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	// Get the actual video width
	ulError = VDR_ConfigureUnitsTags(m_Units, GET_MPEG_DISPLAY_WIDTH(iSourceWidth), TAGDONE);
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	x = iSourceWidth;
	y = iSourceHeight;

	if (x <= 0 || y <= 0)
		return E_FAIL;

	return hr;
}

ULONG DVDMediaPlayer::GetNumberOfTitles(void)
{
	if( !m_ddpThePlayer )
	{
//		return (ULONG) E_FAIL;
		return (ULONG) 0;
	}

	return DDP_GetNumberOfTitles(m_ddpThePlayer);
}

HRESULT DVDMediaPlayer::GetExtendedPlayerState(ExtendedPlayerState& eps)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	ULONG ulErr;
	HRESULT hr;
	ulErr = DDP_GetExtendedPlayerState(m_ddpThePlayer, eps);
	hr = ConvertError(ulErr);
	return hr;
}

HRESULT DVDMediaPlayer::GetAvailStreams(BYTE& af, ULONG& spf)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	ULONG ulErr;
	HRESULT hr;
	ulErr = DDP_GetAvailStreams(m_ddpThePlayer, af, spf);
	hr = ConvertError(ulErr);
	return hr;
}

HRESULT DVDMediaPlayer::EnableCC(bool enable)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_CC_ENABLE(enable), TAGDONE));
}


HRESULT DVDMediaPlayer::GetAudioDynRngCompMode(AC3DynamicRange& drm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_DYNAMIC_RANGE(drm), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioDynRngCompMode(const AC3DynamicRange& drm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_DYNAMIC_RANGE(drm), TAGDONE));
}

HRESULT DVDMediaPlayer::GetAudioDownmixMode(AC3SpeakerConfig& dmm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_SPEAKER_CONFIG(dmm), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioDownmixMode(const AC3SpeakerConfig& dmm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_SPEAKER_CONFIG(dmm), TAGDONE));
}

HRESULT DVDMediaPlayer::GetAudioDualMode(AC3DualModeConfig& dm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_DUAL_MODE_CONFIG(dm), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioDualMode(const AC3DualModeConfig& dm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_DUAL_MODE_CONFIG(dm), TAGDONE));
}

HRESULT DVDMediaPlayer::GetAudioSpdifOutMode(SPDIFOutputMode& dmm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_SPDIF_OUTPUT_MODE(dmm), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioSpdifOutMode(const SPDIFOutputMode& dmm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;


	ULONG uops;
	DVD_PLAYBACK_LOCATION2 LastDvdLoc;

	if (S_OK != GetCurrentLocation(&LastDvdLoc))
		{
		ZeroMemory(&LastDvdLoc, sizeof(LastDvdLoc));
		}

	Stop();


	Error err = VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_SPDIF_OUTPUT_MODE(dmm), TAGDONE);

	PlayAtTimeInTitle(LastDvdLoc.TitleNum, &LastDvdLoc.TimeCode);

	return ConvertError(err);

//	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_SPDIF_OUTPUT_MODE(dmm), TAGDONE));

}

HRESULT DVDMediaPlayer::GetAudioLFEMode(BOOL& lfe)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_DECODE_LFE(lfe), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioLFEMode(const BOOL& lfe)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_DECODE_LFE(lfe), TAGDONE));
}

HRESULT DVDMediaPlayer::GetAudioDialogNormMode(AC3DialogNorm& dn)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_DIALOG_NORM(dn), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioDialogNormMode(const AC3DialogNorm& dn)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_DIALOG_NORM(dn), TAGDONE));
}

HRESULT DVDMediaPlayer::GetAudioCompMode(AC3OperationalMode& opm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_OPERATIONAL_MODE(opm), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioCompMode(const AC3OperationalMode& opm)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_OPERATIONAL_MODE(opm), TAGDONE));
}

HRESULT DVDMediaPlayer::GetAudioCompSFHigh(WORD& sfh)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_HIGH_DYNAMIC_RANGE(sfh), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioCompSFHigh(const WORD& sfh)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_HIGH_DYNAMIC_RANGE(sfh), TAGDONE));
}

HRESULT DVDMediaPlayer::GetAudioCompSFLow(WORD& sfl)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_LOW_DYNAMIC_RANGE(sfl), TAGDONE));
}

HRESULT DVDMediaPlayer::SetAudioCompSFLow(const WORD& sfl)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_LOW_DYNAMIC_RANGE(sfl), TAGDONE));
}


// 0x0000 == stereo, 0x0001 == mono, 0x0002 == surround.
HRESULT DVDMediaPlayer::GetSpeakerOutputMode(WORD& config)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	AC3SpeakerConfig speakerConfig;
	HRESULT hr = S_OK;

	hr = ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_SPEAKER_CONFIG(speakerConfig), TAGDONE));

	if(hr != S_OK)
	{
		return hr;
	}

	switch(speakerConfig)
	{
		case AC3SC_20_NORMAL: // stereo
		{
			config = XC_AUDIO_FLAGS_STEREO;
			break;
		}
		case AC3SC_10: // mono
		{
			config = XC_AUDIO_FLAGS_MONO;
			break;
		}
		default:
		case AC3SC_20_SURROUND_COMPATIBLE: // surround
		{
			config = XC_AUDIO_FLAGS_SURROUND;
			break;
		}
	}
	return hr;
}

// 0x0000 == surround, 0x0001 == stereo, 0x0002 == mono.
HRESULT DVDMediaPlayer::SetSpeakerOutputMode(const WORD& config)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	HRESULT hr = S_OK;

	switch(config)
	{
		case XC_AUDIO_FLAGS_STEREO: // stereo
		{
			hr = ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_SPEAKER_CONFIG(AC3SC_20_NORMAL), TAGDONE));
			break;
		}
		case XC_AUDIO_FLAGS_MONO: // mono
		{
			hr = ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_SPEAKER_CONFIG(AC3SC_10), TAGDONE));
			break;
		}
		case XC_AUDIO_FLAGS_SURROUND: // surround
		{
			hr = ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_SPEAKER_CONFIG(AC3SC_20_SURROUND_COMPATIBLE), TAGDONE));
			break;
		}
	}
	return hr;
}

// 0x0000 == PCM, 0x0001 == AC3, 0x0002 == DTS
HRESULT DVDMediaPlayer::GetDigitalOutputMode(WORD& config)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	AudioTypeConfig digitalConfig;
	HRESULT hr;

	config = 0x0000; // clear current settings

	hr = ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_DTS_AUDIO_TYPE_CONFIG(digitalConfig), TAGDONE));

	if(hr != S_OK)
	{
		return hr;
	}

	if(digitalConfig.config.spdifOutMode == SPDIFOM_DEFAULT )
	{
		config |= (XC_AUDIO_FLAGS_ENABLE_DTS >> 16);
	}

	hr = ConvertError(VDR_ConfigureUnitsTags(m_Units, GET_MPEG2_AC3_AUDIO_TYPE_CONFIG(digitalConfig), TAGDONE));

	if(hr != S_OK)
	{
		return hr;
	}

	if(digitalConfig.config.spdifOutMode == SPDIFOM_DEFAULT )
	{
		config |= (XC_AUDIO_FLAGS_ENABLE_AC3 >> 16);
	}

	return S_OK;
}

// 0x0000 == PCM, 0x0001 == AC3, 0x0002 == DTS
HRESULT DVDMediaPlayer::SetDigitalOutputMode(const WORD& config)
{
	if(!m_hVdrDriver || !m_Units)
		return E_FAIL;

	AudioTypeConfig digitalConfig;
	HRESULT hr;

	if(config & (XC_AUDIO_FLAGS_ENABLE_DTS >> 16))
	{
		digitalConfig.config.spdifOutMode = SPDIFOM_DEFAULT;
	}
	else
	{
		digitalConfig.config.spdifOutMode = SPDIFOM_MUTE_NULL;
	}

	hr = ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_DTS_AUDIO_TYPE_CONFIG(digitalConfig), TAGDONE));

	if(hr != S_OK)
	{
		return hr;
	}

	if(config & (XC_AUDIO_FLAGS_ENABLE_AC3 >> 16))
	{
		digitalConfig.config.spdifOutMode = SPDIFOM_DEFAULT;
	}
	else
	{
		digitalConfig.config.spdifOutMode = SPDIFOM_MUTE_NULL;
	}

	return ConvertError(VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AC3_AUDIO_TYPE_CONFIG(digitalConfig), TAGDONE));

//	return E_NOTIMPL;
}


void DVDMediaPlayer::UpdateDisplay(bool forceUpdate)
{
//	CSize newRatio;
//	if(m_bChangeRatio)
//	{
//		newRatio = this->m_newRatio;
//		m_bChangeRatio = false;
//	}
//	else
//		this->Get_AspectRatios(newRatio.cx, newRatio.cy);
//
//	if(forceUpdate || newRatio.cx != m_currentRatio.cx || newRatio.cy != m_currentRatio.cy)
//	{
//		m_currentRatio = newRatio;
////		DVD_DOMAIN dom;
////		HRESULT hr;
////		ULONG ulerr;
//
//
//		if(!m_Units)
//		{
//			OutputDebugString("DVDMediaPlayer::UpdateDisplay - Units not initialized!");
//			return;
//		}
//
////		else if(this->GetCurrentDomain(&dom) != S_OK)
////		{
////			OutputDebugString("DVDMediaPlayer::UpdateDisplay - No ddp layer!");
////			return;
////		}
////
////		if(dom == DVD_DOMAIN_FirstPlay)
////		{
////			OutputDebugString("DVDMediaPlayer::UpdateDisplay - In warning screens, so don't resize!");
////			return;
////		}
//
//		if(m_iDisplayMode == DPM_16BY9)
//		{
//			ULONG uops;
//			this->GetCurrentUOPS(&uops);
//			if(!(uops & UOP_BUTTON)) // we are in a menu, so pan and scan.
//			{
//				// wmp - TBD - fix - should be MPM_PANSCAN, but highlights are not scaled properly.
//				VDR_ConfigureUnitsTags(m_Units,
//									SET_MPEG2_PRESENTATION_MODE(MPM_FULLSIZE),
//									TAGDONE);
//			}
//			else
//			{
//				VDR_ConfigureUnitsTags(m_Units,
//									SET_MPEG2_PRESENTATION_MODE(MPM_LETTERBOXED),
//									TAGDONE);
//			}
//		}
//		else if(m_iDisplayMode == DPM_PANSCAN)
//		{
//			VDR_ConfigureUnitsTags(m_Units,
//								SET_MPEG2_PRESENTATION_MODE(MPM_PANSCAN),
//								TAGDONE);
//		}
//		else
//		{
//			VDR_ConfigureUnitsTags(m_Units,
//								SET_MPEG2_PRESENTATION_MODE(MPM_FULLSIZE),
//								TAGDONE);
//		}
//
//	}
//	else
//	{
//	}
//
//
}



BOOL DVDMediaPlayer::CalcVideoPos(CSize sizeMonitor, CSize sizeVideo, CPoint& pntVideo)
{
	pntVideo.x = (sizeMonitor.cx - sizeVideo.cx) / 2;
	pntVideo.y = (sizeMonitor.cy - sizeVideo.cy) / 2;
    return TRUE;
}

BOOL DVDMediaPlayer::CalcVideoSize(CSize sizeMonitor, CSize& sizeVideo)
{
    CSize   sizeClient;
	CSize   sizeActualVideo;

    sizeClient.cx = sizeMonitor.cx;// - nHorzBorders;
    sizeClient.cy = sizeMonitor.cy;// - nVertBorders;

	sizeActualVideo = sizeClient;

    double  dblVidLockAspect = 0;
	CSize	sizeTestActual;
	CSize   sizeTestCropped;

	// Get Locked Video aspect ratio
    dblVidLockAspect = (((double)this->m_currentRatio.cx) / ((double)this->m_currentRatio.cy));

    // Video Aspect ratio is locked, so adjust video size according to
    // the current locked aspect ratio so that video completely (less cropping)
    // fits in the monitors client area

	// First try adjusting the actual video width to match the vid lock aspect
	sizeTestActual = sizeActualVideo;
	sizeTestActual.cx = (long)(dblVidLockAspect * ((double)sizeTestActual.cy));
	sizeTestCropped = sizeTestActual;

	if (sizeTestCropped.cx <= sizeClient.cx)
	{
		// Good! The new cropped width fits in client....
		// So set this to the new size
		sizeVideo = sizeTestActual;
	}
	else
	{
		// No Good! We must adjust the height instead
		sizeTestActual = sizeActualVideo;
		sizeTestActual.cy = (long)(((double)sizeTestActual.cx) / dblVidLockAspect);
		sizeTestCropped = sizeTestActual;

		sizeVideo = sizeTestActual;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: Play() - Starts or restarts playback of the DVD video.  If video
//                     successfully plays, the video will play in the forward
//                     direction at 1X speed.
//                     The actions of the function are determined by the current
//                     mode.  Based on the current mode, the function will have
//                     to perform different tasks to ensure video plays in the
//                     forward direction at 1X speed. Below is a description of
//                     the procedure for the various modes:
//	                   DPM_STOPPED, PDM_STILL: Presentation is started.
//                     DPM_PLAYING: Video is playing forward, but it may not be
//                                  playing at the correct 1X speed. Video is
//                                  set to 1X speed if playing at another speed. Video
//                                  could be playing at the wrong rate if DDP_SetPlaybackSpeed()
//                                  had been used before this call to set slow motion.
//                     DPM_PAUSED: Pause state is ended by performing a call to Resume().
//                                 If pause did not resume the mode into playback mode,
//                                 the new mode is ended with a call to DDP_StopScan().
//                                 Once mode is known to be playing, playback speed is
//                                 found.  If speed isn't 1X, speed will be changed to
//                                 1X.
//                     DPM_SCANNING: A mode of scanning first requires a call to
//                                   DDP_StopScan() to end the current scan. Then, since
//                                   playback speed could resume at the wrong speed,
//                                   proper 1X speed is set.
//	                   DPM_REVERSEPLAY: First, reverse play is stopped with a call to
//                                      DDP_StopScan(). Then, proper 1X playback speed
//                                      is set.
//	                   DPM_TRICKPLAY: First, trick play mode is ended with a call to
//                                    DDP_StopScan(). Then proper 1X playback speed
//                                    is ensured with call to EnsureNormalPlaybackSpeed()
//                     All other modes cause function to return failure.
//                     Once playback has been set, function will check the final command
//                     for an error, and if everything was completed successfully,
//                     m_bForwardPlayback is set to TRUE.
// Arguments: None
//    Return: HRESULT  - S_OK or appropriate error code
///////////////////////////////////////////////////////////////////////////////
HRESULT DVDMediaPlayer::Play()
{
	ULONG          ulError = 0;
	DVDPlayerMode  lMode;
	HRESULT        hr;

	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	// Get the current mode state
	lMode = DDP_GetPlayerMode(m_ddpThePlayer);

	// Based on the current mode, determine which function to call
	// to re/start playback.
	//
	switch (lMode)
	{
		case DPM_STOPPED:
		case DPM_STILL:
			// Video stopped. Starting video from the beginning.
//			QI_OUTPUT_DEBUG(5, "Video stopped. Starting video from the beginning.");
			ulError = (ULONG)DDP_StartPresentation(m_ddpThePlayer);
			// This "if" statement forwards a message to the application saying that
			// playback failed because parental levels were too low
/*			if (ulError == GNR_INVALID_NAV_INFO  &&  ::IsWindow(g_hEventWnd))
			{
				::PostMessage(	g_hEventWnd,
								WM_QI_EVENT,
								EC_DVD_ERROR,
								DVD_ERROR_LowParentalLevel);
			}
*/
			break;

		case DPM_PAUSED:
			{
				ExtendedPlayerState oPlayer;
				// Current mode is paused.  Unpause (i.e. Resume) from this point.
				hr = Resume();
				if (FAILED(hr))
				{
//					return hr;
				}
				// Get the current mode
				lMode = DDP_GetPlayerMode(m_ddpThePlayer);

				// Since you want the video to play forward at normal speed, it is necessary
				// to find out if resume brought you back into play mode or another mode like
				// reverse playback.
				if (lMode != DPM_PLAYING)
				{
					ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
					hr = ConvertError(ulError);
					if (FAILED(hr))
					{
//						return hr;
					}
				}

				if (lMode != DPM_PLAYING)
				{
					ulError = (ULONG)DDP_PauseOff(m_ddpThePlayer);
					hr = ConvertError(ulError);
					if (FAILED(hr))
					{
						return hr;
					}
				}
				// There is no guarantee that you are running at a speed of 1X.
				// The following code will make sure that you are.
				if (m_dPlaySpeed != 1.0)
				{
					// Change playback speed to 1X
					ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
				}
			}
			break;

		case DPM_SCANNING:

			// If you were playing backwards before beginning the seek or are
			// currently seeking faster than 1.0 rate, stop the current scan.
			ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}

			// The video is now seeking in a forward direction.  Set playback speed
			// to normal 1x
			ulError = (ULONG)DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
			break;

		case DPM_REVERSEPLAY:
			// Ending current mode and resuming playback from this point

			ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}

			// Ensure normal playback speed
			ulError = (ULONG)DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
			break;

		case DPM_PLAYING:
			// Since mode is already playing, we only want to ensure that playback is at
			// the proper speed
			if (m_dPlaySpeed != 1.0)
			{
				// Change playback speed to 1X
				ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
			}
			break;

		case DPM_TRICKPLAY:
			// In trick play mode.  This mode must end and standard playback must begin
			ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}

			// We need to ensure proper playback speed
			if (m_dPlaySpeed != 1.0)
			{
				// Change playback speed to 1X
				ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
			}
			break;

		default:
			// Unknown mode encountered
			return E_FAIL;
	}

	// Check for an error
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	// Set the member variables which store that current status is
	// forward playback at normal speed.
	m_bForwardPlayback = TRUE;
	m_lMode = QMODE_PLAYING;
	return hr;

}

///////////////////////////////////////////////////////////////////////////////
//  Function: TimeSearch() - Jumps to the specified location in the video disc.
//                           Video will resume playback from the new point at
//                           1X speed.  If currently scanning or playing in reverse,
//                           the scan is stopped.  If paused, video playback is resumed.
//                           After jumping to the new location, if the playback
//                           speed is not 1X, the speed will be changed to 1X.
// Arguments: INT iHours - hour to jump to. Playback continues from this new location.
//            INT iMinutes - fractional hour in minute format to jump to
//            INT iSeconds - fractionl minute in second format to jump to
//    Return: HRESULT  - S_OK or appropriate error code
///////////////////////////////////////////////////////////////////////////////
HRESULT  DVDMediaPlayer::TimeSearch(INT iHours, INT iMinutes, INT iSeconds)
{
	ExtendedPlayerState oPlayer;
	DVDPlayerMode       ddp;
	ULONG               ulError;
	HRESULT             hr;

	if (!m_ddpThePlayer)
	{
		return E_FAIL;
	}

	// Make sure we are not seeking or paused
	ddp = DDP_GetPlayerMode(m_ddpThePlayer);
	switch (ddp)
	{
		case DPM_PAUSED:
			Resume();
			break;

		case DPM_SCANNING:
		case DPM_REVERSEPLAY:
			ulError = DDP_StopScan(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}
			break;
	}

	// Go to the new location
	DVDTime dvdTime = DVDTime( (int)iHours, (int)iMinutes, (int)iSeconds, 0, 1 );
	ulError = (ULONG)DDP_TimeSearch( m_ddpThePlayer, dvdTime );
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	// The jump we just performed will shut off scanning but it will not notify the
	// callback function of this change.  This statement does the same job.
	m_dScanSpeed = 1.0;

	// That jump would have ended forward/backward scan playback if it was occuring, but
	// the jump would not affect any playback changes made with DDP_SetPlaybackSpeed. The
	// following code will fix that.
	if (m_dPlaySpeed != 1.0)
	{
		// Change playback speed to 1X
		ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
		hr = ConvertError(ulError);
	}
	m_bForwardPlayback = TRUE;
	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//  Function: NextChapter() - advances to the next chapter on the disc.
// Arguments: None
//    Return: HRESULT  - S_OK or appropriate error code
///////////////////////////////////////////////////////////////////////////////
HRESULT  DVDMediaPlayer::NextChapter()
{

	ULONG               ulError;
	ExtendedPlayerState oPlayer;
	HRESULT             hr;

	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	// Jump to the next chapter
	ulError = DDP_NextPGSearch(m_ddpThePlayer);
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	// The jump we just performed will shut off scanning but it will not notify the
	// callback function of this change.  This statement does the same job.
	m_dScanSpeed = 1.0;

	// Video should now be playing, so ensure proper playback speed
	// Since mode is already playing, we only want to ensure that playback is at
	// the proper speed
	if (m_dPlaySpeed != 1.0)
	{
		// Change playback speed to 1X
		ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
		hr = ConvertError(ulError);
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: PreviousChapter() - seeks back to the previous chapter on the disc
// Arguments: None
//    Return: HRESULT  - S_OK or appropriate error code
///////////////////////////////////////////////////////////////////////////////
HRESULT  DVDMediaPlayer::PreviousChapter()
{
	ULONG               ulError;
	HRESULT             hr;

	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	// Go to the previous chapter
	ulError = DDP_PrevPGSearch(m_ddpThePlayer);
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	// The jump we just performed will shut off scanning but it will not notify the
	// callback function of this change.  This statement does the same job.
	m_dScanSpeed = 1.0;

	// Video should now be playing, so ensure proper playback speed
	// Since mode is already playing, we only want to ensure that playback is at
	// the proper speed
	if (m_dPlaySpeed != 1.0)
	{
		// Change playback speed to 1X
		ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
		hr = ConvertError(ulError);
	}
	m_bForwardPlayback = TRUE;
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: NextTitle() - advances forward to the next DVD-Video title.
// Arguments: None
//    Return: HRESULT  - S_OK or appropriate error code
///////////////////////////////////////////////////////////////////////////////
HRESULT  DVDMediaPlayer::NextTitle()
{
	if(!m_ddpThePlayer)
	{
		return E_FAIL;
	}

	ULONG ulerr;
	HRESULT hr;
	DVDLocation loc;

	short stitleCount = DDP_GetNumberOfTitles( m_ddpThePlayer );
	ulerr = DDP_GetCurrentLocation( m_ddpThePlayer, loc );
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return hr;

	if(loc.title < stitleCount)
	{
		m_wCurrentTitle++;
		ulerr = DDP_TitlePlay( m_ddpThePlayer, loc.title + 1 );
		hr = ConvertError( ulerr );
		if( FAILED(hr) )
			return hr;
		m_bForwardPlayback = TRUE;
		return S_OK;
	}
	return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: PreviousTitle() - seeks back to the previous DVD-Video title
// Arguments: None
//    Return: HRESULT  - S_OK or appropriate error code
///////////////////////////////////////////////////////////////////////////////
HRESULT  DVDMediaPlayer::PreviousTitle()
{
	if(!m_ddpThePlayer)
	{
		return E_FAIL;
	}

	ULONG ulerr;
	HRESULT hr;
	DVDLocation loc;

	ulerr = DDP_GetCurrentLocation( m_ddpThePlayer, loc );
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return hr;

	if(loc.title > 0)
	{
		m_wCurrentTitle--;
		ulerr = DDP_TitlePlay( m_ddpThePlayer, loc.title - 1 );
		hr = ConvertError( ulerr );
		if( FAILED(hr) )
			return hr;
		m_bForwardPlayback = TRUE;
		return S_OK;
	}
	return E_FAIL;

}


HRESULT DVDMediaPlayer::ShowMenu (LONG lMenu)
{

	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	HRESULT hr;
	ULONG ulerr;
	DVDLocation loc;

	// Make sure we are not scanning, not playing reverse and not playing in slow-motion

	ulerr = DDP_GetCurrentLocation( m_ddpThePlayer, loc );
	hr = ConvertError( ulerr );
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	// Get the current mode state
	DVDPlayerMode  lMode = DDP_GetPlayerMode(m_ddpThePlayer);
	if (lMode == DPM_SCANNING)
	{
		// If you were playing backwards before beginning the seek or are
		// currently seeking faster than 1.0 rate, stop the current scan.
		ulerr = (ULONG)DDP_StopScan(m_ddpThePlayer);
		hr = ConvertError(ulerr);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	// The video is now seeking in a forward direction.  Set playback speed
	// to normal 1x
	ulerr = (ULONG)DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);

	switch( loc.domain )
	{
		case FP_DOM:
			return E_FAIL;

		case TT_DOM:
		case VTSM_DOM:
		case VMGM_DOM:
		case STOP_DOM:
			{
				VTSMenuType vtsmenu;
				switch( lMenu )
				{
					case QMENU_TITLE:
						vtsmenu = VMT_TITLE_MENU;
						break;
					case QMENU_ROOT:
						vtsmenu = VMT_ROOT_MENU;
						break;
					case QMENU_SUBPIC:
						vtsmenu = VMT_SUBPICTURE_MENU;
						break;
					case QMENU_AUDIO:
						vtsmenu = VMT_AUDIO_MENU;
						break;
					case QMENU_ANGLE:
						vtsmenu = VMT_ANGLE_MENU;
						break;
					case QMENU_CHAPTER:
						vtsmenu = VMT_PTT_MENU;
						break;
					default:
						return E_FAIL;
				}

				ulerr = (ULONG)DDP_MenuCall( m_ddpThePlayer, vtsmenu );
				hr = ConvertError( ulerr );
				return hr;
			}
	} // end switch domain
	return hr;
}


bool DVDMediaPlayer::IsPlaying(void)
{
	if(!m_ddpThePlayer)
		return FALSE;

	DVDPlayerMode	lDdpMode = DDP_GetPlayerMode(m_ddpThePlayer);

    if ((lDdpMode != DPM_STOPPED) && (lDdpMode != DPM_NONE))
    {
        return TRUE;
    }

    return FALSE;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayTitle(
    /* [in] */ ULONG ulTitle)
{
	if (ulTitle == 0)
	{
		return E_INVALIDARG;
	}

	if ( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	if (ulTitle >= 1 && ulTitle <= DDP_GetNumberOfTitles(m_ddpThePlayer))
	{
		ULONG ulerr = DDP_TitlePlay( m_ddpThePlayer, (SHORT)ulTitle );
		HRESULT hr = ConvertError( ulerr );
		if( FAILED(hr) )
			return hr;
		m_wCurrentTitle = (SHORT)ulTitle;
		return hr;
	}
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayChapterInTitle(
    /* [in] */ ULONG ulTitle,
    /* [in] */ ULONG ulChapter)
{
	HRESULT ret = this->PlayTitle(ulTitle);
	ret |= this->PlayChapter(ulChapter);
	return ret;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayAtTimeInTitle(
    /* [in] */ ULONG ulTitle,
    /* [in] */ DVD_HMSF_TIMECODE *pStartTime)
{
	HRESULT ret = this->PlayTitle(ulTitle);
	ret |= this->TimeSearch(pStartTime->bHours, pStartTime->bMinutes, pStartTime->bSeconds);
	return ret;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayAtTimeInTitleForced(
    /* [in] */ ULONG ulTitle,
    /* [in] */ DVD_HMSF_TIMECODE *pStartTime)
{
	HRESULT hr;
	ULONG ulerr;
	DVDLocation loc;

	ulerr = DDP_GetCurrentLocation( m_ddpThePlayer, loc );
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return hr;
	if(loc.title != ulTitle)
	{
		ulerr = DDP_TimePlayForced( m_ddpThePlayer, (WORD) ulTitle, DVDTime() );
		hr = ConvertError( ulerr );
		if( FAILED(hr) )
			return hr;
	}

	ulerr = DDP_TimeSearchForced( m_ddpThePlayer, DVDTime(pStartTime->bHours, pStartTime->bMinutes, pStartTime->bSeconds, pStartTime->bFrames, 25 ));
	hr = ConvertError( ulerr );
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::Stop( void)
{

	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}
	ULONG ulerr;
	HRESULT hr;
	DVDLocation loc;
	ExtendedPlayerState oPlayer;

	ulerr = (ULONG)DDP_GetCurrentLocation( m_ddpThePlayer, loc );
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
	{
		return hr;
	}

	switch( loc.domain )
	{
		case STOP_DOM:
		case FP_DOM:
			return S_OK; // no need to stop

		case VMGM_DOM:
		case VTSM_DOM:
			ulerr = (ULONG)DDP_Stop( m_ddpThePlayer );
			hr = ConvertError( ulerr );
			if( FAILED(hr) )
				return E_FAIL;
			m_bForwardPlayback = TRUE;
			return hr; // stop altogether

		case TT_DOM:
			// If this is not here, restarting presentation
			// could cause the video to play back at the wrong speed.
			// Get information about the playback speed
			if (m_dPlaySpeed != 1.0)
			{
				// Change playback speed to 1X
				ulerr = DDP_SetPlaybackSpeed(m_ddpThePlayer, C_NORMAL_PLAYBACK_SPEED);
				hr = ConvertError(ulerr);
				if (FAILED(hr))
				{
					return hr;
				}
			}

			// Stop the presentation
			if( RAV_STOP_TO_MENU )
			{
				ulerr = DDP_MenuCall( m_ddpThePlayer, VMT_ROOT_MENU );
			}
			else
			{
				ulerr = DDP_Stop( m_ddpThePlayer );
			}
			m_bForwardPlayback = TRUE;
			hr = ConvertError( ulerr );
			return hr; // stop altogether

		default:
			return S_OK;
	} // end switch loc.domain
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::ReturnFromSubmenu(void)
{
	if(!m_ddpThePlayer)
		return E_FAIL;

	return ConvertError(DDP_GoUp(m_ddpThePlayer));
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayAtTime(
    /* [in] */ DVD_HMSF_TIMECODE *pTime)
{
	return this->TimeSearch(pTime->bHours, pTime->bMinutes, pTime->bSeconds);
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayChapter(
    /* [in] */ ULONG ulChapter)
{
	if ( !m_ddpThePlayer )
	{
		return E_FAIL;
	}
	DVDLocation dvdl_loc;

	ULONG ulerr = DDP_GetCurrentLocation( m_ddpThePlayer, dvdl_loc );
	HRESULT hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return hr;
	m_wCurrentTitle  = (short)dvdl_loc.title;

	unsigned short numChapters = 0;
	numChapters = (SHORT)DDP_GetNumberOfPartOfTitle( m_ddpThePlayer, m_wCurrentTitle );

	if(ulChapter > numChapters)
		return E_FAIL;

	ulerr = DDP_PTTPlay( m_ddpThePlayer, m_wCurrentTitle, (SHORT)ulChapter );
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	m_wCurrentChapter = (SHORT)ulChapter;

	return hr;
}

HRESULT DVDMediaPlayer::PlayPrevChapter(void)
{
	return this->PreviousChapter();
}

HRESULT DVDMediaPlayer::ReplayChapter(void)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	ULONG ulerr = DDP_TopPGSearch( m_ddpThePlayer);
	HRESULT hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	return hr;
}

HRESULT DVDMediaPlayer::PlayNextChapter(void)
{
	return this->NextChapter();
}

HRESULT DVDMediaPlayer::PlayForwards(
    /* [in] */ double dSpeed)
{
#ifdef _DEBUG
	char buf[100];

	wsprintf(buf, "PlayForwards(%f)", dSpeed);
	OutputDebugString(buf);
#endif _DEBUG

	if(!m_ddpThePlayer)
	{
		return E_FAIL;
	}

	DVDPlayerMode lDdpMode;
	ULONG         ulError;
	WORD		  wSpeed;
	HRESULT       hr;

	lDdpMode = DDP_GetPlayerMode(m_ddpThePlayer);

	// If you want to increase the speed a lot, use DDP_ForwardScan.  For speeds
	// slower than 2X, use SetPlaybackSpeed
	//
	if (dSpeed > 2.0)
	{
		// FAST SCAN //
		wSpeed = (WORD)dSpeed; // could lose some resolution, ie, if 2.5, will most likely be 2...
#ifdef _DEBUG
		wsprintf(buf, "DDP_ForwardScan(%x)", wSpeed);
		OutputDebugString(buf);
#endif _DEBUG
		ulError = (ULONG)DDP_ForwardScan(m_ddpThePlayer, wSpeed);
		m_dScanSpeed = (double)wSpeed;
	}
	else
	{
		// SLOW SEEK //
		m_dPlaySpeed = dSpeed; // event handler needs to know the speed we're going to

		// Must turn off scan if it is on.  Scan is only on when the mode is either
		// QMODE_SEEKING or QMODE_TRICKREVERSE.
//		if (lDdpMode != DPM_PLAYING)
		if (lDdpMode == DPM_SCANNING
			|| lDdpMode == DPM_TRICKPLAY
			|| lDdpMode == DPM_REVERSEPLAY)
		{
#ifdef _DEBUG
			wsprintf(buf, "DDP_StopScan()");
			OutputDebugString(buf);
#endif _DEBUG
			ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if( FAILED(hr) )
			{
				m_dPlaySpeed = 1.0;
				return hr;
			}
		}
		// Now that scan is off, adjust the playback speed to the desired rate.
		wSpeed = (WORD)(dSpeed * C_NORMAL_PLAYBACK_SPEED);
#ifdef _DEBUG
		wsprintf(buf, "DDP_SetPlaybackSpeed(%d)", wSpeed);
		OutputDebugString(buf);
#endif _DEBUG
		ulError = (ULONG)DDP_SetPlaybackSpeed(m_ddpThePlayer, wSpeed);
	}

	// Check for an error
	hr = ConvertError(ulError);
	if( FAILED(hr) )
	{
		return hr;
	}

	// If paused, turn pause off
	if (lDdpMode == DPM_PAUSED)
	{
		// Ending current mode and resuming playback from this point
#ifdef _DEBUG
		wsprintf(buf, "DDP_PauseOff");
		OutputDebugString(buf);
#endif _DEBUG
		ulError = (ULONG)DDP_PauseOff(m_ddpThePlayer);
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

	}

	// Playback speed was changed successfully
	m_bForwardPlayback = TRUE;
	return hr;
}

HRESULT DVDMediaPlayer::PlayBackwards(
    /* [in] */ double dSpeed)
{
#ifdef _DEBUG
	char buf[100];
#endif _DEBUG

	WORD          wSpeed;
	DVDPlayerMode lMode;
	ULONG         ulError;
	HRESULT       hr;

	// Ensure proper initialization
	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	// Get the current mode
//	lMode = DDP_GetPlayerMode(m_ddpThePlayer);
//
//	// If the video is paused, unpause it
//	if (lMode == DPM_PAUSED)
//	{
//		ulError = (ULONG)DDP_PauseOff( m_ddpThePlayer );
//		hr = ConvertError(ulError);
////		hr = Resume();
//		if (FAILED(hr))
//		{
//			return hr;
//		}
//	}

	// If you want to increase the RATE a lot, use DDP_BackwardScan.  For speeds
	// slower than standard 1X, use SetPlaybackSpeed
	//
	if( dSpeed > 2.0 )
	{
		// FAST SEEK //

		wSpeed = (WORD)dSpeed; // could lose some resolution, ie, if 2.5, will most likely be 2...
#ifdef _DEBUG
		wsprintf(buf, "DDP_BackwardScan(%x)", wSpeed);
		OutputDebugString(buf);
#endif _DEBUG
		ulError = DDP_BackwardScan( m_ddpThePlayer, wSpeed );
		m_dScanSpeed = (double)wSpeed;
	}
	else
	{
		// SLOW SEEK //


		// Create the new speed in thousands, i.e. 2x speed is 2000, half speed is 500
		wSpeed = (WORD)(dSpeed * C_NORMAL_PLAYBACK_SPEED);

		// Go to trick reverse play
		hr = Trick_ReversePlay(wSpeed);
		if (FAILED(hr))
		{
			return hr;
		}

		m_dPlaySpeed = dSpeed;
	}

	// Check for an error in the reverse play
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}
	m_bForwardPlayback = FALSE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::ShowMenu(
    /* [in] */ DVD_MENU_ID MenuID)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}
	HRESULT hr;
	ULONG ulerr;
	DVDLocation loc;

	ulerr = DDP_GetCurrentLocation( m_ddpThePlayer, loc );
	hr = ConvertError( ulerr );
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	switch( loc.domain )
	{
		case FP_DOM:
			return E_FAIL;

		case TT_DOM:
		case VTSM_DOM:
		case VMGM_DOM:
		case STOP_DOM:
			{
				VTSMenuType vtsmenu;
				switch( MenuID )
				{
					case QMENU_TITLE:
						vtsmenu = VMT_TITLE_MENU;
						break;
					case QMENU_ROOT:
						vtsmenu = VMT_ROOT_MENU;
						break;
					case QMENU_SUBPIC:
						vtsmenu = VMT_SUBPICTURE_MENU;
						break;
					case QMENU_AUDIO:
						vtsmenu = VMT_AUDIO_MENU;
						break;
					case QMENU_ANGLE:
						vtsmenu = VMT_ANGLE_MENU;
						break;
					case QMENU_CHAPTER:
						vtsmenu = VMT_PTT_MENU;
						break;
					default:
						return E_FAIL;
				}

				ulerr = (ULONG)DDP_MenuCall( m_ddpThePlayer, vtsmenu );
				hr = ConvertError( ulerr );
				return hr;
			}
	} // end switch domain
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::Resume(void)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	HRESULT hr;
	ULONG ulerr;
	ulerr = DDP_Resume(m_ddpThePlayer);
	hr = ConvertError(ulerr);
	return hr;
}


HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectRelativeButton(
    DVD_RELATIVE_BUTTON buttonDir)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	HRESULT hr;
	ULONG ulerr = 0;

	switch(buttonDir)
	{
	case DVD_Relative_Upper:
		ulerr = DDP_UpperButtonSelect( m_ddpThePlayer );
		break;
	case DVD_Relative_Lower:
		ulerr = DDP_LowerButtonSelect( m_ddpThePlayer );
		break;
	case DVD_Relative_Left:
		ulerr = DDP_LeftButtonSelect( m_ddpThePlayer );
		break;
	case DVD_Relative_Right:
		ulerr = DDP_RightButtonSelect( m_ddpThePlayer );
		break;
	}

	hr = ConvertError( ulerr );
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::ActivateButton(void)
{
	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	ULONG   ulError;
	HRESULT hr;

	// Trigger an "enter" command upon whatever DVD option is highlighted
	ulError = (ULONG)DDP_ButtonActivate( m_ddpThePlayer );
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectAndActivateButton(
    /* [in] */ ULONG ulButton)
{	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	return ConvertError(DDP_ButtonSelectAndActivate(m_ddpThePlayer,(WORD)ulButton));
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::StillOff( void)
{
	if (!m_ddpThePlayer)
	{
		return E_FAIL;
	}
	ULONG   ulError;
	HRESULT hr;

	ulError = DDP_StillOff(m_ddpThePlayer);
	hr = ConvertError(ulError);
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::Pause(
    /* [in] */ BOOL bState)
{
	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	ULONG         ulError;
	DVDPlayerMode lMode;
	HRESULT       hr;

	if(bState)
	{
		// Get the current mode state
		lMode = DDP_GetPlayerMode(m_ddpThePlayer);

		// If the mode is already paused, return S_OK.  If the mode is
		// an unpausable mode, return E_FAIL
		//
		switch (lMode)
		{
			case DPM_NONE:
			case DPM_STILL:
			case DPM_STOPPED:
				// These are modes incapable of being paused.
				return E_FAIL;

			case DPM_PAUSED:
				// Already paused
				return S_OK;
		}

		// Turn pause ON
		ulError = DDP_PauseOn(m_ddpThePlayer);
		hr = ConvertError(ulError);
		if( FAILED(hr) )
		{
			return hr;
		}

		// Set the current play mode, paused and motionless
		return S_OK;
	}
	else
	{
		// Make sure we are in a paused state
		lMode = DDP_GetPlayerMode(m_ddpThePlayer);
		if (lMode != DPM_PAUSED)
		{
			return E_FAIL;
		}

		// Ending current mode and resuming playback from this point
		ulError = (ULONG)DDP_PauseOff( m_ddpThePlayer );
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		return S_OK;
	}
}

HRESULT DVDMediaPlayer::Pause()
{
	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	DVDPlayerMode lMode;

	// Get the current mode state
	lMode = DDP_GetPlayerMode(m_ddpThePlayer);

	// If the mode is already paused, return S_OK.  If the mode is
	// an unpausable mode, return E_FAIL
	//
	switch (lMode)
	{
		case DPM_NONE:
		case DPM_STILL:
		case DPM_STOPPED:
			// These are modes incapable of being paused.
			return E_FAIL;

		case DPM_PAUSED:
			return Pause(false);
		default:
			return Pause(true);
	}
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectAudioStream(
    /* [in] */ ULONG ulAudio)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	ULONG ulerr = DDP_AudioStreamChange( m_ddpThePlayer, (WORD)ulAudio );
	HRESULT hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;
	m_ulCurrentAudio = (ULONG)ulAudio;
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectSubpictureStream(
    /* [in] */ ULONG ulSubPicture)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	m_bSubPicOn = TRUE; // we set this member variable here because we want
	// the subpic data displayed regardless of the member
	// variable's current state. Default is FALSE.
	ULONG ulerr = DDP_SubPictureStreamChange( m_ddpThePlayer,
		(WORD)ulSubPicture, m_bSubPicOn);
	HRESULT hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;
	m_ulCurrentSubpic = ulSubPicture;
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SetSubpictureState(
    /* [in] */ BOOL bState)
{
	if ( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	BYTE  btaudioMask;
	DWORD dwsubPictureMask;
	int   i;
	WORD  wsubPicData[32];
	WORD  wnumSubPic;
	WORD  wcurrentSubPic;
	DWORD dwbitChk = 1;

	ULONG ulerr = DDP_GetAvailStreams( m_ddpThePlayer, btaudioMask, dwsubPictureMask );
	wnumSubPic = 0;

	// mark available stream numbers
	for ( i=0; i < C_MAX_STREAMS; i++ )
	{
		if ( dwbitChk & dwsubPictureMask )
		{
			wsubPicData[wnumSubPic] = (WORD)i;
			wnumSubPic++;
		}
		dwbitChk *= 2;
	}//end of for loop

	// get current stream index
	wcurrentSubPic = (WORD)DDP_GetCurrentSubPictureStream( m_ddpThePlayer );
	m_bSubPicOn = bState;
	// notify stream number indicated by current index
	ulerr = DDP_SubPictureStreamChange( m_ddpThePlayer,
		wsubPicData[ wcurrentSubPic ],
		m_bSubPicOn );
	HRESULT hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;
	m_ulCurrentSubpic = (ULONG)wcurrentSubPic;

	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectAngle(
    /* [in] */ ULONG ulAngle)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	ULONG ulerr = DDP_AngleChange( m_ddpThePlayer, (SHORT)ulAngle);
	HRESULT hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;
	m_ulCurrentAngle = ulAngle;
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectParentalLevel(
    /* [in] */ ULONG ulParentalLevel)
{
	if( (ulParentalLevel < 1 || ulParentalLevel > 9) || (ulParentalLevel == 0xFFFFFFFF) )
		return QI_W_DATAOUTOFRANGE;

	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}
	HRESULT hr;
	ULONG ulErr;
	ulErr = (ULONG)DDP_ParentalLevelSelect( m_ddpThePlayer, (WORD)ulParentalLevel );
	hr = ConvertError( ulErr );
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectParentalCountry(
    /* [in] */ WORD wCountry)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}
	HRESULT hr;
	ULONG ulErr;
	ulErr = DDP_ParentalCountrySelect( m_ddpThePlayer, wCountry);
	hr = ConvertError( ulErr );
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::ActivateAtPosition(
    /* [in] */ POINT point)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}
	HRESULT hr;
	ULONG ulErr;
	ulErr = DDP_ButtonSelectAndActivateAt(m_ddpThePlayer, (WORD)point.x, (WORD)point.y);
	hr = ConvertError( ulErr );
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectAtPosition(
    /* [in] */ POINT point)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}
	HRESULT hr;
	ULONG ulErr;
	ulErr = DDP_ButtonSelectAt(m_ddpThePlayer, (WORD)point.x, (WORD)point.y);
	hr = ConvertError( ulErr );
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayChaptersAutoStop(
    /* [in] */ ULONG ulTitle,
    /* [in] */ ULONG ulChapter,
    /* [in] */ ULONG ulChaptersToPlay)
{
	// wmp - TBD
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::AcceptParentalLevelChange(
    /* [in] */ BOOL bAccept)
{
	m_bAcceptParentalLevelChange = bAccept;
	return S_OK;
}


HRESULT STDMETHODCALLTYPE DVDMediaPlayer::PlayPeriodInTitleAutoStop(
    /* [in] */ ULONG ulTitle,
    /* [in] */ ULONG ulChapter,
    /* [in] */ DVD_HMSF_TIMECODE *pStartTime,
    /* [in] */ DVD_HMSF_TIMECODE *pEndTime,
    /* [in] */ DWORD flags,
	/* [out]*/ DWORD& breakpointID)
{
	ULONG ulerr;
	HRESULT hr;

	if( !m_ddpThePlayer )
		return E_FAIL;

	if(ulChapter == NULL)
	{
		ulerr = DDP_SetBreakpoint(m_ddpThePlayer,
									(WORD) ulTitle,
									DVDTime(pEndTime->bHours,pEndTime->bMinutes,pEndTime->bSeconds,pEndTime->bFrames,30),
									flags,
									breakpointID);
	}
	else
	{
		ulerr = DDP_SetBreakpointExt(m_ddpThePlayer,
									(WORD) ulTitle,
									(WORD) ulChapter,
									DVDTime(pEndTime->bHours,pEndTime->bMinutes,pEndTime->bSeconds,pEndTime->bFrames,30),
									flags,
									breakpointID);
	}

	hr = ConvertError(ulerr);
	if( FAILED(hr) )
		return hr;

	if(pStartTime != NULL)
	{
		DVDLocation loc;
		ulerr = DDP_GetCurrentLocation( m_ddpThePlayer, loc );
		hr = ConvertError( ulerr );
		if( FAILED(hr) )
			return hr;

		if(loc.title != ulTitle)
		{
			ulerr = DDP_TimePlayForced( m_ddpThePlayer, (WORD) ulTitle, DVDTime() );
			hr = ConvertError( ulerr );
			if( FAILED(hr) )
				return hr;
		}

		ulerr = DDP_TimeSearchForced( m_ddpThePlayer, DVDTime(pStartTime->bHours, pStartTime->bMinutes, pStartTime->bSeconds, pStartTime->bFrames, 25 ));
		hr = ConvertError(ulerr);
		if( FAILED(hr) )
			return hr;
	}

	return hr;


}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::ClearBreakpoint(
    /* [in] */ DWORD& breakpointID)
{
	ULONG ulerr;
	HRESULT hr;

	if( !m_ddpThePlayer )
		return E_FAIL;

	ulerr = DDP_ClearBreakpoint(m_ddpThePlayer, breakpointID);
	hr = ConvertError(ulerr);
	return hr;
}


HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectDefaultMenuLanguage(
    /* [in] */ LCID Language)
{
	ULONG ulerr;
	HRESULT hr;

	if( !m_ddpThePlayer )
		return E_FAIL;

	ulerr = DDP_MenuLanguageSelect(m_ddpThePlayer, (SHORT)Language);
	hr = ConvertError(ulerr);
	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectDefaultAudioLanguage(
    /* [in] */ LCID Language,
    /* [in] */ DVD_AUDIO_LANG_EXT audioExtension)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	DVDSubPictureStreamFormat subPicFormat;
	ULONG ulerr;
	HRESULT hr;
	SHORT sSubPicNum;

	if (0 == m_PreferredSubpicLanguage)
	{
//		m_ulCurrentSubpic = DDP_GetCurrentSubPictureStream( m_ddpThePlayer );
//		sSubPicNum = (SHORT)m_ulCurrentSubpic;
//
//		ulerr = DDP_GetSubPictureStreamAttributes( m_ddpThePlayer, (WORD)sSubPicNum,
//			subPicFormat );
//		hr = ConvertError( ulerr );
//		if( FAILED(hr) )
//			return E_FAIL;

		subPicFormat.languageCode = LANG_NEUTRAL;
		subPicFormat.languageExtension = 0;
	}
	else
	{
		subPicFormat.languageCode = m_PreferredSubpicLanguage;
		subPicFormat.languageExtension = m_PreferredSubpicExt;
	}

	ulerr = DDP_InitialLanguageSelect(m_ddpThePlayer, Language,
		audioExtension, subPicFormat.languageCode, subPicFormat.languageExtension);

	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	m_PreferredAudioLanguage = Language;
	m_PreferredAudioExt = audioExtension;

	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::SelectDefaultSubpictureLanguage(
    /* [in] */ LCID Language,
    /* [in] */ DVD_SUBPICTURE_LANG_EXT subpictureExtension)
{
	if( !m_ddpThePlayer )
	{
		return E_FAIL;
	}

	DVDAudioStreamFormat audioFormat;
	ULONG ulerr;
	HRESULT hr;
	SHORT sAudioNum;

	if (0 == m_PreferredAudioLanguage)
	{
//		m_ulCurrentAudio = DDP_GetCurrentAudioStream( m_ddpThePlayer );
//		sAudioNum = (SHORT)m_ulCurrentAudio;
//
//		ulerr = DDP_GetAudioStreamAttributes( m_ddpThePlayer, (WORD)sAudioNum,
//			audioFormat );
//		hr = ConvertError( ulerr );
//		if( FAILED(hr) )
//			return E_FAIL;

		audioFormat.languageCode = LANG_NEUTRAL;
		audioFormat.languageExtension = 0;
	}
	else
	{
		audioFormat.languageCode = m_PreferredAudioLanguage;
		audioFormat.languageExtension = m_PreferredAudioExt;
	}

	ulerr = DDP_InitialLanguageSelect(m_ddpThePlayer, audioFormat.languageCode, audioFormat.languageExtension,
		Language, subpictureExtension);

	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	m_PreferredSubpicLanguage = Language;
	m_PreferredSubpicExt = subpictureExtension;

	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentDomain(
    /* [out] */ DVD_DOMAIN __RPC_FAR *pDomain)
{
	HRESULT hr;
	ULONG ulerr;
	DVDLocation loc;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetCurrentLocation(m_ddpThePlayer, loc);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	*pDomain = (DVD_DOMAIN) (loc.domain + 1);		// the + 1 maps DVDDomain to DVD_DOMAIN enums

	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentLocation(
    /* [out] */ DVD_PLAYBACK_LOCATION2 __RPC_FAR *pLocation)
{
	HRESULT hr;
	ULONG ulerr;
	DVDLocation loc;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetCurrentLocation(m_ddpThePlayer, loc);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	pLocation->TitleNum = loc.title;
	pLocation->ChapterNum = loc.partOfTitle; // wmp - tbd - check this!!!!
	pLocation->TimeCode.bHours = (BYTE)loc.titleTime.Hours();
	pLocation->TimeCode.bMinutes = (BYTE)loc.titleTime.Minutes();
	pLocation->TimeCode.bSeconds = (BYTE)loc.titleTime.Seconds();
	pLocation->TimeCode.bFrames = (BYTE)loc.titleTime.Frames();
	pLocation->TimeCodeFlags = 0;

	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetTotalTitleTime(
    /* [out] */ DVD_HMSF_TIMECODE __RPC_FAR *pTotalTime,
    /* [out] */ ULONG __RPC_FAR *ulTimeCodeFlags,
	/* [in]	 */ ULONG title)
{
	HRESULT hr;
	ULONG ulerr;
	DVDLocation loc;
	DVDTime time;

	if(!m_ddpThePlayer)
		return E_FAIL;
	if(title == NULL)
	{

		ulerr = DDP_GetCurrentDuration(m_ddpThePlayer, loc);
		time = loc.titleTime;
	}
	else
	{
		ulerr = DDP_GetTitleDuration(m_ddpThePlayer, (WORD) title, time);
	}

	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	pTotalTime->bHours = (BYTE)time.Hours();
	pTotalTime->bMinutes = (BYTE)time.Minutes();
	pTotalTime->bSeconds = (BYTE)time.Seconds();
	pTotalTime->bFrames = (BYTE)time.Frames();

	return hr;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentButton(
    /* [out] */ ULONG __RPC_FAR *pulButtonsAvailable,
    /* [out] */ ULONG __RPC_FAR *pulCurrentButton)
{
	ULONG ulerr;
	HRESULT hr;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetExtendedPlayerState(m_ddpThePlayer, m_eps);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	*pulButtonsAvailable = m_eps.numberOfButtons;
	*pulCurrentButton = m_eps.selectedButton;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentAngle(
    /* [out] */ ULONG __RPC_FAR *pulAnglesAvailable,
    /* [out] */ ULONG __RPC_FAR *pulCurrentAngle)
{
	HRESULT hr;
	ULONG ulerr;
	DVDLocation loc;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetCurrentLocation(m_ddpThePlayer, loc);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	*pulAnglesAvailable = DDP_GetNumberOfAngles(m_ddpThePlayer, loc.title);

	*pulCurrentAngle = DDP_GetCurrentAngle(m_ddpThePlayer);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentAudio(
    /* [out] */ ULONG __RPC_FAR *pulStreamsAvailable,
    /* [out] */ ULONG __RPC_FAR *pulCurrentStream)
{
	HRESULT hr;
	ULONG ulerr;
	BYTE audio = 0;
	DWORD subPic;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetAvailStreams(m_ddpThePlayer, audio, subPic);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	DWORD dwbitChk = 1L;
	unsigned long lnum = 0;
	for (int  i=0; i < 32; i++ )
	{
		if ( dwbitChk & audio )
			lnum++;
		dwbitChk *= 2;
	}
	*pulStreamsAvailable = lnum;

	*pulCurrentStream = DDP_GetCurrentAudioStream(m_ddpThePlayer);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentSubpicture(
    /* [out] */ ULONG __RPC_FAR *pulStreamsAvailable,
    /* [out] */ ULONG __RPC_FAR *pulCurrentStream,
    /* [out] */ BOOL __RPC_FAR *pbIsDisabled)
{
	HRESULT hr;
	ULONG ulerr;
	BYTE audio;
	DWORD subPic;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetAvailStreams(m_ddpThePlayer, audio, subPic);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	DWORD dwbitChk = 1L;
	unsigned long lnum = 0;
	for (int i=0; i<32; i++ )
	{
		if ( dwbitChk & subPic )
			lnum++;
		dwbitChk *= 2;
	}

	*pulStreamsAvailable = lnum;

	*pulCurrentStream = DDP_GetCurrentSubPictureStream(m_ddpThePlayer);

	*pbIsDisabled = DDP_IsCurrentSubPictureEnabled(m_ddpThePlayer);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentUOPS(
    /* [out] */ ULONG __RPC_FAR *pulUOPs)
{
	if(!m_ddpThePlayer)
		return E_FAIL;

	*pulUOPs = DDP_GetForbiddenUserOperations(m_ddpThePlayer);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetAllSPRMs(
    /* [out] */ SPRMARRAY __RPC_FAR *pRegisterArray)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetAllGPRMs(
    /* [out] */ GPRMARRAY __RPC_FAR *pRegisterArray)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetAudioLanguage(
    /* [in] */ ULONG ulStream,
    /* [out] */ LCID __RPC_FAR *pLanguage)
{
	ULONG ulerr;
	HRESULT hr;
	DVDAudioStreamFormat audioFormat;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetAudioStreamAttributes(m_ddpThePlayer, (WORD)ulStream, audioFormat);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	*pLanguage = audioFormat.languageCode;

	return S_OK;
}


HRESULT STDMETHODCALLTYPE  DVDMediaPlayer::GetAudioLanguage(ULONG ulAudioNum, char* pstrCountry, UINT nMaxLength)
{
	if (pstrCountry == NULL)
		return E_POINTER;

	if( m_ddpThePlayer )
	{
		DVDAudioStreamFormat audioFormat;
		ULONG ulerr;
		HRESULT hr;
		char buffer[MAX_PATH];

		ulerr = DDP_GetAudioStreamAttributes( m_ddpThePlayer, (WORD)ulAudioNum,
			audioFormat );
		hr = ConvertError( ulerr );
		if( FAILED(hr) )
			return E_FAIL;

		buffer[0] = '\0'; // safety init.
		int isize = sizeof( LANGUAGE_DATA ) / sizeof( LANGUAGE_DATA[0] );
		for( int i=0; i < isize; i++ )
		{
			if( audioFormat.languageCode == LANGUAGE_DATA[i].code )
				break;
		}
		if( i == isize ) // if we didn't find a match, use lang_neutral
			i = 0;

		GetLanguageStringFromCode(audioFormat.languageCode, buffer, MAX_PATH);

		switch( audioFormat.languageExtension ) // specific code extension
		{
			case DVD_AUD_EXT_VisuallyImpaired:
				strcat(buffer, IDS_AUD_VISUALLY_IMPAIRED);
				break;
			case DVD_AUD_EXT_DirectorComments1:
			case DVD_AUD_EXT_DirectorComments2:
				strcat(buffer, IDS_AUD_DIRECTORS_COMMENTS);
				break;
			default:
				{
					switch( /*(*/audioFormat.channels - 1 /* & 0x07)*/ )
					{
						case 0x00:
							strcat(buffer, " [1]");
							break;

						case 0x01:
							strcat(buffer, " [2]");
							break;

						case 0x02:
							strcat(buffer, " [3]");
							break;

						case 0x03:
							strcat(buffer, " [4]");
							break;
						case 0x04:
							strcat(buffer, " [5]");
							break;
						case 0x05:
							strcat(buffer, " [5.1]");
							break;
						case 0x06:
							strcat(buffer, " [7]");
							break;
						case 0x07:
							strcat(buffer, " [8]");
							break;
					} // end switch
				}
				break;
		} // end switch

		strncpy(pstrCountry, buffer, nMaxLength);
		return S_OK;
	}
	return E_FAIL;
}




HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetSubpictureLanguage(
    /* [in] */ ULONG ulStream,
    /* [out] */ LCID __RPC_FAR *pLanguage)
{
	ULONG ulerr;
	HRESULT hr;
	DVDSubPictureStreamFormat subPicFormat;

	if(!m_ddpThePlayer)
		return E_FAIL;

	ulerr = DDP_GetSubPictureStreamAttributes(m_ddpThePlayer, (WORD)ulStream, subPicFormat);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	*pLanguage = subPicFormat.languageCode;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE  DVDMediaPlayer::GetSubpictureLanguage(ULONG ulSubpicNum, char* pstrCountry, UINT nMaxLength)
{
	if (pstrCountry == NULL)
		return E_POINTER;

	if( m_ddpThePlayer )
	{
		DVDSubPictureStreamFormat subPicFormat;

		ULONG ulerr = DDP_GetSubPictureStreamAttributes(
			m_ddpThePlayer,
			(WORD)ulSubpicNum,
			subPicFormat );
		HRESULT hr = ConvertError( ulerr );
		if( FAILED(hr) )
			return E_FAIL;

		TCHAR buffer[MAX_PATH];
		buffer[0] = 0; // safety init.
		int isize = sizeof( LANGUAGE_DATA ) / sizeof( LANGUAGE_DATA[0] );
		for( int i=0; i < isize; i++ )
		{
			if( subPicFormat.languageCode == LANGUAGE_DATA[i].code )
				break;
		}
		if( i == isize ) // if we didn't find a match, use lang_neutral
			i = 0;

		GetLanguageStringFromCode(subPicFormat.languageCode, buffer, MAX_PATH);

		switch( subPicFormat.languageExtension ) // specific code extension
		{
			case DVD_SP_EXT_Caption_Normal:
				strcat(buffer, IDS_SUBPIC_NORMAL);
				break;
			case DVD_SP_EXT_Caption_Big:
				strcat(buffer, IDS_SUBPIC_BIG);
				break;
			case DVD_SP_EXT_Caption_Children:
				strcat(buffer, IDS_SUBPIC_CHILDREN);
				break;
			case DVD_SP_EXT_CC_Normal:
				strcat(buffer, IDS_SUBPIC_CC_NORMAL);
				break;
			case DVD_SP_EXT_CC_Big:
				strcat(buffer, IDS_SUBPIC_CC_BIG);
				break;
			case DVD_SP_EXT_CC_Children:
				strcat(buffer, IDS_SUBPIC_CC_CHILDREN);
				break;
			case DVD_SP_EXT_Forced:
				strcat(buffer, IDS_SUBPIC_FORCED);
				break;
			case DVD_SP_EXT_DirectorComments_Normal:
				strcat(buffer, IDS_SUBPIC_DIRECTORS_NORMAL);
				break;
			case DVD_SP_EXT_DirectorComments_Big:
				strcat(buffer, IDS_SUBPIC_DIRECTORS_BIG);
				break;
			case DVD_SP_EXT_DirectorComments_Children:
				strcat(buffer, IDS_SUBPIC_DIRECTORS_CHILDREN);
				break;
			default:
				break;
		} // end switch

		strncpy(pstrCountry, buffer, nMaxLength);

		return S_OK;
	}
	return E_FAIL;
}



HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetTitleAttributes(
    /* [in] */ ULONG ulTitle,
    /* [out] */ DVD_ATR __RPC_FAR *pATR)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetVMGAttributes(
    /* [out] */ DVD_ATR __RPC_FAR *pATR)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetCurrentVideoAttributes(
    /* [out] */ DVD_VideoATR __RPC_FAR *pATR)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetVideoAttributes(
    /* [in] */ ULONG ulTitle,
    /* [in] */ BOOL bForMenus,
    /* [out] */ DVD_VideoATR __RPC_FAR *pATR)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetAudioAttributes(
    /* [in] */ ULONG ulStream,
    /* [out] */ DVD_AudioAttributes __RPC_FAR *pATR)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetKaraokeAttributes(
    /* [in] */ ULONG ulStream,
    /* [out] */ DVD_KaraokeAttributes __RPC_FAR *pAttributes)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetSubpictureAttributes(
    /* [in] */ ULONG ulStream,
    /* [out] */ DVD_SubpictureATR __RPC_FAR *pATR)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDVDVolumeInfo(
    /* [out] */ ULONG __RPC_FAR *pulNumOfVolumes,
    /* [out] */ ULONG __RPC_FAR *pulVolume,
    /* [out] */ DVD_DISC_SIDE __RPC_FAR *pSide,
    /* [out] */ ULONG __RPC_FAR *pulNumOfTitles)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDVDTextNumberOfLanguages(
    /* [out] */ ULONG __RPC_FAR *pulNumOfLangs)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDVDTextLanguageInfo(
    /* [in] */ ULONG ulLangIndex,
    /* [out] */ ULONG __RPC_FAR *pulNumOfStrings,
    /* [out] */ LCID __RPC_FAR *pLangCode,
    /* [out] */ enum DVD_TextCharSet __RPC_FAR *pbCharacterSet)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDVDTextStringAsNative(
    /* [in] */ ULONG ulLangIndex,
    /* [in] */ ULONG ulStringIndex,
    /* [out] */ BYTE __RPC_FAR *pbBuffer,
    /* [in] */ ULONG ulMaxBufferSize,
    /* [out] */ ULONG __RPC_FAR *pulActualSize,
    /* [out] */ enum DVD_TextStringType __RPC_FAR *pType)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDVDTextStringAsUnicode(
    /* [in] */ ULONG ulLangIndex,
    /* [in] */ ULONG ulStringIndex,
    /* [out] */ WCHAR __RPC_FAR *pchwBuffer,
    /* [in] */ ULONG ulMaxBufferSize,
    /* [out] */ ULONG __RPC_FAR *pulActualSize,
    /* [out] */ enum DVD_TextStringType __RPC_FAR *pType)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetPlayerParentalLevel(
    /* [out] */ ULONG __RPC_FAR *pulParentalLevel,
    /* [out] */ WORD __RPC_FAR *wCountryCode)
{
	ULONG ulerr;
	HRESULT hr;
	ExtendedPlayerState Eps;

	if(!m_ddpThePlayer)
		return E_FAIL;

	Eps.request = EPS_PARENTAL;

	ulerr = DDP_GetExtendedPlayerState(m_ddpThePlayer, Eps);
	hr = ConvertError( ulerr );
	if( FAILED(hr) )
		return E_FAIL;

	WORD wLevel = Eps.parentalLevel;
	*pulParentalLevel = wLevel;
	*wCountryCode = Eps.parentalCountry;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetNumberOfChapters(
    /* [in] */ ULONG ulTitle,
    /* [out] */ ULONG __RPC_FAR *pulNumOfChapters)
{
	if(!m_ddpThePlayer)
		return E_FAIL;

	*pulNumOfChapters = DDP_GetNumberOfPartOfTitle(m_ddpThePlayer, (WORD)ulTitle);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetTitleParentalLevels(
    /* [in] */ ULONG ulTitle,
    /* [out] */ ULONG __RPC_FAR *pulParentalLevels)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDVDDirectory(
    /* [size_is][out] */ LPWSTR pszwPath,
    /* [in] */ ULONG cbMaxSize,
    /* [out] */ ULONG __RPC_FAR *pcbActualSize)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::IsAudioStreamEnabled(
    /* [in] */ ULONG ulStreamNum,
    /* [out] */ BOOL __RPC_FAR *pbEnabled)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetUniqueID(
    /* [in] */ LPCWSTR pszwPath,
    /* [out] */ ULONGLONG __RPC_FAR *pullUniqueID)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetMenuLanguages(
    /* [out] */ LCID __RPC_FAR *pLanguages,
    /* [in] */ ULONG uMaxLanguages,
    /* [out] */ ULONG __RPC_FAR *puActualLanguages)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetButtonAtPosition(
    /* [in] */ POINT point,
    /* [out] */ ULONG __RPC_FAR *puButtonIndex)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDefaultMenuLanguage(
    /* [out] */ LCID __RPC_FAR *pLanguage)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDefaultAudioLanguage(
    /* [out] */ LCID __RPC_FAR *pLanguage,
    /* [out] */ DVD_AUDIO_LANG_EXT __RPC_FAR *pAudioExtension)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE DVDMediaPlayer::GetDefaultSubpictureLanguage(
    /* [out] */ LCID __RPC_FAR *pLanguage,
    /* [out] */ DVD_SUBPICTURE_LANG_EXT __RPC_FAR *pSubpictureExtension)
{
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: Get_AspectRatios()
// Description:
//   Arguments:     long& xFactor -
//                  long& yFactor -
//      Return:     HRESULT  - S_OK or appropriate error code
// 4:3 or 16:9
///////////////////////////////////////////////////////////////////////////////
HRESULT DVDMediaPlayer::Get_AspectRatios( ULONG& xFactor, ULONG& yFactor )
{

	if (!m_ddpThePlayer)
	{
		// Default to 4X3
		xFactor = 4;
		yFactor = 3;
		return S_OK;
	}

	m_iDisplayMode = DDP_GetCurrentDisplayMode(m_ddpThePlayer);

	if (m_iDisplayMode == DPM_16BY9)
	{
		xFactor = 16;
		yFactor = 9;
	}
	else
	{
		xFactor = 4;
		yFactor = 3;
	}
	return S_OK;
}

DisplayPresentationMode DVDMediaPlayer::GetDisplayMode(void)
{
	if(!m_ddpThePlayer)
	{
		return DPM_NONE;
	}

	return DDP_GetCurrentDisplayMode(m_ddpThePlayer);
}

HRESULT DVDMediaPlayer::SetDisplayMode( DisplayPresentationMode mode )
{
	if(!m_ddpThePlayer)
	{
		return E_FAIL;
	}

	ULONG ulerr;
	HRESULT hr;

	ulerr = DDP_SetDisplayMode(m_ddpThePlayer, mode);
	hr = ConvertError(ulerr);
	if(!FAILED(hr))
	{
		CSize newRatio;
		this->Get_AspectRatios(newRatio.cx,newRatio.cy);
		Set_AspectRatios(newRatio.cx,newRatio.cy); //also updates display
	}
	return hr;
}

HRESULT DVDMediaPlayer::Set_AspectRatios( ULONG& xFactor, ULONG& yFactor )
{
	m_bChangeRatio = true;
	m_newRatio.cx = xFactor;
	m_newRatio.cy = yFactor;
	UpdateDisplay(true);
	return S_OK;
}

HRESULT DVDMediaPlayer::SetSystemRegion(char driveLetter, BYTE regMask)
{
	if(!m_Units)
	{
		return E_FAIL;
	}

	// we are running ... shutdown to set region
	if( m_ddpThePlayer )
	{
		DDP_Stop(m_ddpThePlayer);
		EventHandlerRemove();
		DDP_ClosePlayer( m_ddpThePlayer );
		m_ddpThePlayer = NULL;
//	}

		ULONG err = DDP_SetSystemRegion(m_Units, driveLetter, regMask);

		if(IS_ERROR(err))
		{
			return ConvertError(err);
		}

		if (IS_ERROR(DDP_OpenPlayer(m_Units, driveLetter, m_ddpThePlayer)))
		{
			return E_FAIL;
		}

		if(m_pEventCallback)
		{
			EventHandlerInstall(m_pEventCallback);
		}
	}
	else // ddp is not running
	{
		ULONG err = DDP_SetSystemRegion(m_Units, driveLetter, regMask);

		if(IS_ERROR(err))
		{
			return ConvertError(err);
		}

	}

	return S_OK;

}

HRESULT DVDMediaPlayer::GetSystemRegion(char driveLetter, BYTE& regMask)
{
	BYTE availSets;
	RegionSource regionSource;

	if(!m_Units)
	{
		return E_FAIL;
	}

	HRESULT hr = DDP_GetRegionSettings(m_Units, driveLetter, regMask, availSets, regionSource);

	return hr;
}



HRESULT DVDMediaPlayer::ConvertError( ULONG ulErr )
{
	switch( ulErr )
	{
		case GNR_OK:
			return S_OK;

		case GNR_UNIMPLEMENTED:// the function is not yet implemented, and will never be
			return QI_I_NOTAPPLICABLE;

		case GNR_RANGE_VIOLATION:// a given parameter was out of bounds
			return QI_W_DATAOUTOFRANGE;

		case GNR_FILE_NOT_FOUND:// general file not found error
			return ERROR_FILE_NOT_FOUND;

		case GNR_FILE_IN_USE:// the file is currently in use
			return ERROR_DRIVE_LOCKED;

		case GNR_OBJECT_READ_ONLY:
			return QI_W_READONLY;

		case GNR_OBJECT_WRITE_ONLY:
			return QI_W_SETONLY;

		case GNR_REGIONS_DONT_MATCH:
			return QI_E_INVALID_REGION;

		case GNR_NOT_ENOUGH_MEMORY:// general not enough memory left
			return ERROR_NOT_ENOUGH_MEMORY;

		case GNR_END_OF_FILE:// attempt to read behind the end of a file
			return ERROR_HANDLE_EOF;

		case GNR_INSUFFICIENT_RIGHTS: // parental level too high
		case GNR_PARENTAL_LEVEL_TOO_LOW:
			return QI_E_INSUFFICIENT_RIGHTS;

		case GNR_INVALID_NAV_INFO:	// seeing this also on a parental too high
		case GNR_FILE_WRONG_FORMAT:// the file is in the wrong file format
		case GNR_MEM_NOT_ALLOCATED:// the memory that was requested to be freed was not allocated
			// before.
		case GNR_MEM_ALLOCATED_BEFORE:// the memory was allocated before
		case GNR_OBJECT_NOT_FOUND:// the requested object could not be found
		case GNR_OBJECT_EXISTS:// the object requested to be created does already exist
		case GNR_OBJECT_IN_USE:// an operation that needs exclusive access to an object, found the object
			// already in use
		case GNR_OBJECT_FOUND:// an object that was to be inserted in a data structure with unique objects
			// was already in there
		case GNR_INVALID_CONFIGURE_STATE:// the unit was in an invalid state for configuration
		case GNR_OBJECT_FULL:
		case GNR_OBJECT_EMPTY:
		case GNR_OBJECT_NOT_ALLOCATED:
		case GNR_OPERATION_PROHIBITED:
		case GNR_OBJECT_INVALID:
		default:
			return E_FAIL;
	} // end switch ulerr
}

HRESULT  DVDMediaPlayer::put_Volume(LONG lVal)
{
	WORD wLeft;
	WORD wRight;
	WORD wNewVolume;

	if (lVal < 0  ||  lVal > G_C_MAX_VOLUME)
	{
		return E_INVALIDARG;
	}

	if (m_Units == NULL)
	{
		return E_POINTER;
	}

	// Save the current volume for the purpose of writing it to the registry in ShutDown().  Since
	// the volume passed in is in a different scale from that of VDR, the value is converted and
	// stored in wNewVolume
	m_lCurrentVolume = lVal;
	wNewVolume = (WORD) (lVal * G_C_MAX_VOLUME);

	// Get the current left and right values.  This is needed in case the balance is not equal right now
	VDR_ConfigureUnitsTags(m_Units, GET_MPEG_LEFT_VOLUME(wLeft), GET_MPEG_RIGHT_VOLUME(wRight), TAGDONE);

	// Determine what the value of the left and right channels should be
	if (wLeft == wRight)
	{
		// Balance is equal so both channels should be the same value
		wLeft  = wNewVolume;
		wRight = wNewVolume;
	}
	else if (wRight > wLeft)
	{
		// Balance to the right. The right channel will receive the full volume value but
		// the left channel should be a based upon the proportion of the old values.  So,
		// Old Left / Old Right  ==  New Left / New Right
		wLeft  = (WORD)((float)(wNewVolume * wLeft) / (float)wRight);
		wRight = wNewVolume;
	}
	else
	{
		// Balance to the left. The left channel will receive the full volume value but
		// the right channel should be a based upon the proportion of the old values.  So,
		// Old Left / Old Right  ==  New Left / New Right
		wRight = (WORD)((float)(wNewVolume * wRight) / (float)wLeft);
		wLeft  = wNewVolume;
	}

	// Set the values of the left and right channels
	return VDR_ConfigureUnitsTags(m_Units, SET_MPEG_LEFT_VOLUME(wLeft), SET_MPEG_RIGHT_VOLUME(wRight), TAGDONE);
}

HRESULT  DVDMediaPlayer::get_Volume(LONG* plVal)
{
	WORD wLeft;
	WORD wRight;

	if (m_Units == NULL)
	{
		return E_POINTER;
	}

	// Get the current left and right values.  This is needed in case the balance is not equal right now
	VDR_ConfigureUnitsTags(m_Units, GET_MPEG_LEFT_VOLUME(wLeft), GET_MPEG_RIGHT_VOLUME(wRight), TAGDONE);
	float fVol = (float)MAX_VOLUME(wLeft, wRight) / (float)G_C_MAX_VOLUME;

	m_lCurrentVolume = LONG(fVol + 0.50f);
	*plVal = m_lCurrentVolume;
	return S_OK;
}

HRESULT DVDMediaPlayer::Trick_FrameForward()
{
	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	DVDPlayerMode lMode;
	ULONG         ulError;
	HRESULT       hr;

	// Get the current mode
	lMode = DDP_GetPlayerMode(m_ddpThePlayer);

	// Based on the current status of the playback, function will do different thing.
	// If playing, video is paused.  If paused, video will advance to the next frame.
	//
	switch (lMode)
	{
		case DPM_REVERSEPLAY:
		case DPM_SCANNING:
		case DPM_PLAYING:
			// Currently playing, so pause the video
			ulError = DDP_PauseOn(m_ddpThePlayer);
			hr = ConvertError(ulError);
			return hr;

		case DPM_PAUSED:
/*			if (m_bForwardPlayback == FALSE)
			{
				ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
				hr = ConvertError(ulError);
				if (FAILED(hr))
				{
					return hr;
				}

				ulError = DDP_PauseOn(m_ddpThePlayer);
				hr = ConvertError(ulError);
				if (FAILED(hr))
				{
					return hr;
				}

				m_bForwardPlayback = TRUE;
			}
*/			// Set for trick play, if not already set
//			ulError = DDP_TrickPlay(m_ddpThePlayer);
//			hr = ConvertError(ulError);
//			if (FAILED(hr))
//			{
//				return hr;
//			}
			// * Fall through to DPM_TRICKPLAY *
		case DPM_TRICKPLAY:
			// Currently paused, so advance the frame
//			ulError = (ULONG)DDP_AdvanceFrameBy(m_ddpThePlayer, 1);
			ulError = (ULONG)DDP_AdvanceFrame(m_ddpThePlayer);
			m_bForwardPlayback = TRUE;
			hr = ConvertError(ulError);
			return hr;
	}

	return E_FAIL;
}

HRESULT DVDMediaPlayer::Trick_FrameBackward()
{
	// Make sure the player is properly initialized
	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	DVDPlayerMode lMode;
	ULONG         ulError;
	HRESULT       hr;

	// Get the current mode
	lMode = DDP_GetPlayerMode(m_ddpThePlayer);

	// Based on the current status of the playback, function will do different thing.
	// If playing, video is paused.  If paused, video will go to the previous frame.
	//
	switch (lMode)
	{
		case DPM_REVERSEPLAY:
		case DPM_SCANNING:
		case DPM_PLAYING:
			// Currently reverse playing, forward playing or seeking, so pause the video
			ulError = DDP_PauseOn(m_ddpThePlayer);
			hr = ConvertError(ulError);
			return hr;

		case DPM_PAUSED:
/*			if (m_bForwardPlayback == TRUE)
			{
				// Cannot go into trick mode when in reverse playback
//				ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
				ulError = (ULONG)DDP_ReversePlayback(m_ddpThePlayer);
				hr = ConvertError(ulError);
				if (FAILED(hr))
				{
					return hr;
				}

				ulError = DDP_PauseOn(m_ddpThePlayer);
				hr = ConvertError(ulError);
				if (FAILED(hr))
				{
					return hr;
				}

				m_bForwardPlayback = FALSE;
			}
*/			// Set for trick play, if not already set
			ulError = DDP_TrickPlay(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}
			// - Fall through to DPM_TRICKPLAY -
		case DPM_TRICKPLAY:
			// Attempt to move one frame closer to the beginning of the video
			ulError = (ULONG)DDP_AdvanceFrameBy(m_ddpThePlayer, -1);
//			ulError = (ULONG)DDP_AdvanceFrame(m_ddpThePlayer);
			hr = ConvertError(ulError);
			m_bForwardPlayback = FALSE;
			return hr;
	}

	return E_FAIL;
}

//-----------------------------------------------------------------------------
//FUNCTION: Trick_ReversePlay()
//PURPOSE:  Causes the video to be presented in reverse mode and at 1X speed unless
//          the current mode does not allow reverse play to occur.  After checking
//          that the player is initialized, the mode os obtained. Based on the
//          current mode, different actions will be done. Below is a list of the
//          modes and what happens with each:
//          DPM_STOPPED / DPM_STILL:  Function returns E_FAIL because reverse play can
//                                    not begin when the player isn't running any video
//          DPM_REVERSEPLAY: Already in reverse play, but the speed could be incorrect.
//                           EnsureNormalPlaybackSpeed is called to make sure speed is
//                           at 1X.
//          DPM_PAUSED: First, pause is ended restoring previous playback mode.  If the
//                      mode is not restored into reverse play, StopScan ends any seeking
//                      and DDP_ReversePlayback() is called.
//          DPM_SCANNING: DDP_StopScan is called which ends the current scan.  Next,
//                        DDP_ReversePlayback is called to initiate reverse play, after which
//                        the speed is set to normal speed.
//          DPM_TRICKPLAY: First, trick play mode must be stopped.  This is done with DDP_StopScan.
//                         Then reverse play is initiated by calling DDP_ReversePlayback
//          Any other mode: Returns E_FAIL.
//          After the mode specific tasks are finished, the function checks for an error in the
//          last DDP command.  If it was a failure, the function returns a failing HRESULT.  Success
//          results in m_bForwardPlayback becoming FALSE and the function returns TRUE.
//
HRESULT DVDMediaPlayer::Trick_ReversePlay(WORD wSpeed)
{
	// Make sure the player is properly initialized
	if (!m_ddpThePlayer)
	{
		return QI_E_NOTINITIALIZED;
	}

	DVDPlayerMode lMode;
	ULONG         ulError = 0;
	HRESULT       hr;

	// Get the current mode
	lMode = DDP_GetPlayerMode(m_ddpThePlayer);

	// Based on the current mode, determine which function to call
	// to re/start playback.
	//
	switch (lMode)
	{
		case DPM_STOPPED:
		case DPM_STILL:
			// Video stopped. Cannot play backwards because we need a current video
			// point to reverse from!
			return E_FAIL;

		case DPM_REVERSEPLAY:
			// The program is currently playing the video in reverse.  Make sure
			// the video is being played at the proper 1X rate
			ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, wSpeed);
			break;

		case DPM_PAUSED:
			// Ending current mode and resuming playback from this point
			ulError = GNR_OK;

			// Current mode is paused.  Unpause (i.e. Resume) from this point.
//			hr = Resume();
//			if (FAILED(hr))
//			{
//				return hr;
//			}

			// When pause was taken away, the playback reverted to another play mode.
			lMode = DDP_GetPlayerMode(m_ddpThePlayer);

			// Since you may have resumed playback into "play" mode, switch
			// to reverse playback if you are not playing back in reverse.
			if (lMode != DPM_REVERSEPLAY)
			{
//				ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
//				hr = ConvertError(ulError);
//				if (FAILED(hr))
//				{
//					return hr;
//				}

				ulError = (ULONG)DDP_ReversePlayback(m_ddpThePlayer);
				hr = ConvertError(ulError);
				if (FAILED(hr))
				{
					return hr;
				}

				ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, wSpeed);
			}
			break;

		case DPM_SCANNING:

			// Stop the scan (aka seek)
			ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}

			// Tell it to play back reversed
			ulError = (ULONG)DDP_ReversePlayback(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}

			// Set playback speed to 1X
			// Get information about the playback speed
			ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, wSpeed);
			break;

		case DPM_PLAYING:
			ulError = (ULONG)DDP_ReversePlayback(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}

			// Set playback speed to 1X
			// Get information about the playback speed
			ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, wSpeed);
			break;

		case DPM_TRICKPLAY:
			// In trick play mode.  Must end trick play and start reverse playback
			ulError = (ULONG)DDP_StopScan(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				return hr;
			}

			// Start reverse playback
			ulError = (ULONG)DDP_ReversePlayback(m_ddpThePlayer);
			hr = ConvertError(ulError);
			if (FAILED(hr))
			{
				 return hr;
			}

			ulError = DDP_SetPlaybackSpeed(m_ddpThePlayer, wSpeed);
			break;

		default:
			// Unknown mode encountered
			return E_FAIL;
    }

    // Check for an error
    hr = ConvertError(ulError);
    if (FAILED(hr))
    {
         return hr;
    }

    // Set the member variables which store that current status is
    // forward playback at normal speed.
    m_bForwardPlayback = FALSE;
    return hr;
}

//-----------------------------------------------------------------------------
//FUNCTION: Trick_Zoom(int x0, int y0, int width, int height)
//PURPOSE:  Scales the selected range to the specified zoom factor.
//INPUTS:   (x0, y0) - Upper left-hand point of the zoomed area
//          width    - Width of the rectangle you want zoomed
//          height   - Height of the area you want zoomed.
//
//NOTES:    Function will either restores the original 1X standard zoom or
//          attempts to scale the picture.  This decision is made based on
//          the value of width and height.
//          If width or height are zero, bounding region is empty and this
//          signals that the function should restore the original zoom.
//          If the bounding region has area, the function will attempt to
//          zoom to whatever that bounding region is.
//
//          Point/sizes are measured in pixels of source video NOT the size of
//          the video window!
//
HRESULT DVDMediaPlayer::Trick_Zoom(INT leftX, INT topY, INT width, INT height)
{
	ULONG   ulError;
	HRESULT hr;

	// If the width or height is 0, this is an empty region.  An empty region means
	// the component is supposed to restore the original 1X zoom.
	if (width == 0  ||  height == 0)
	{
		// Setting adapt source size to true restores the original 1X scale
		ulError = VDR_ConfigureUnitsTags(m_Units, SET_PIP_ADAPT_SOURCE_SIZE(TRUE), TAGDONE);

	}
	else
	{
		unsigned short          iSourceHeight;
		unsigned short          iSourceWidth;
		int                     iExpressedHeight;
		float                   fScaleFactor;

		// Get the actual video height
		ulError = VDR_ConfigureUnitsTags(m_Units, GET_MPEG_DISPLAY_HEIGHT(iSourceHeight), TAGDONE);
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		// Get the actual video width
		ulError = VDR_ConfigureUnitsTags(m_Units, GET_MPEG_DISPLAY_WIDTH(iSourceWidth), TAGDONE);
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		// Calculate the height the app is read
		DisplayPresentationMode iDisplayMode;
		iDisplayMode = DDP_GetCurrentDisplayMode(m_ddpThePlayer);

		// disregard above - it distorts aspect ratio
		iExpressedHeight = iSourceHeight;

		// Convert the topY point to the real source scale
		fScaleFactor = (float)iSourceHeight / (float)iExpressedHeight;
		topY   = (int)(fScaleFactor * topY);
		height = (int)(fScaleFactor * height);

		// Change the zoom
		ulError = VDR_ConfigureUnitsTags(m_Units, SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
			SET_PIP_SOURCE_LEFT(leftX),
			SET_PIP_SOURCE_TOP(topY),
			SET_PIP_SOURCE_WIDTH(width),
			SET_PIP_SOURCE_HEIGHT(height),
			TAGDONE);
	}

	// Check for an error which would have occurred in the ConfigureUnitsTags function
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}

// DVDMediaPlayer::Trick_Zoom
//	in:	leftX and topY set the position for the upper left corner of the zoom box.
//	in:	scaleFactor - float setting the magnification.
//		scaleFactor == 1.0 resets to normal viewing.
//		scaleFactor == 0.0 leaves the current scaleFactor intact, but moves the window to leftX, topY.
//		scaleFactor >  1.0 zooms (scaleFactor * current magnification) with box at leftX, topY.
HRESULT DVDMediaPlayer::Trick_Zoom(INT leftX, INT topY, float scaleFactor)
{
	ULONG   ulError;
	HRESULT hr;

	// If the width or height is 0, this is an empty region.  An empty region means
	// the component is supposed to restore the original 1X zoom.
	if (scaleFactor == 1.0) // reset to SF of 1.0
	{
		// Setting adapt source size to true restores the original 1X scale
		ulError = VDR_ConfigureUnitsTags(m_Units, SET_PIP_ADAPT_SOURCE_SIZE(TRUE), TAGDONE);

	}
	else if(scaleFactor == 0.0) // just reposition.
	{
		unsigned short          maxHeight, maxWidth;

		int                     iExpressedHeight;
		float                   fScaleFactor;
		float					currentSF;
		unsigned short width, height;



		// Get max video sizes
		ulError = VDR_ConfigureUnitsTags(m_Units,	GET_MPEG_DISPLAY_HEIGHT(maxHeight),
													GET_MPEG_DISPLAY_WIDTH(maxWidth),
													TAGDONE);
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		ulError = Get_CurrentScaleFactor(currentSF);

		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		scaleFactor = currentSF;

		// Calculate the height the app is read
		DisplayPresentationMode iDisplayMode;
		iDisplayMode = DDP_GetCurrentDisplayMode(m_ddpThePlayer);
		iExpressedHeight = maxHeight;

		// Convert the topY point to the real source scale
		fScaleFactor = (float)maxHeight / (float)iExpressedHeight;
		topY   = (int)(fScaleFactor * topY);

		width = (unsigned short) ((float)maxWidth / scaleFactor);
		height = (unsigned short) ((float)iExpressedHeight / scaleFactor);

		// Change the zoom
		ulError = VDR_ConfigureUnitsTags(m_Units, SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
			SET_PIP_SOURCE_LEFT(leftX),
			SET_PIP_SOURCE_TOP(topY),
			SET_PIP_SOURCE_WIDTH(width),
			SET_PIP_SOURCE_HEIGHT(height),
			TAGDONE);

	}
	else // zoom
	{
		unsigned short          maxHeight, maxWidth;

		int                     iExpressedHeight;
		float                   fScaleFactor;
		float					currentSF;
		unsigned short width, height;



		// Get max video sizes
		ulError = VDR_ConfigureUnitsTags(m_Units,	GET_MPEG_DISPLAY_HEIGHT(maxHeight),
													GET_MPEG_DISPLAY_WIDTH(maxWidth),
													TAGDONE);
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		ulError = Get_CurrentScaleFactor(currentSF);

		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		scaleFactor *= currentSF;

		// Calculate the height the app is read
		DisplayPresentationMode iDisplayMode;
		iDisplayMode = DDP_GetCurrentDisplayMode(m_ddpThePlayer);
		iExpressedHeight = maxHeight;

		// Convert the topY point to the real source scale
		fScaleFactor = (float)maxHeight / (float)iExpressedHeight;
		topY   = (int)(fScaleFactor * topY);

		width = (unsigned short) ((float)maxWidth / scaleFactor);
		height = (unsigned short) ((float)iExpressedHeight / scaleFactor);

		// Change the zoom
		ulError = VDR_ConfigureUnitsTags(m_Units, SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
			SET_PIP_SOURCE_LEFT(leftX),
			SET_PIP_SOURCE_TOP(topY),
			SET_PIP_SOURCE_WIDTH(width),
			SET_PIP_SOURCE_HEIGHT(height),
			TAGDONE);
	}


	// Check for an error which would have occurred in the ConfigureUnitsTags function
	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}

HRESULT DVDMediaPlayer::Freeze(BYTE*& ppFreezeBuffer, DWORD& dwBufferSize)
{
    BYTE    *pfreezeBuffer;	        // Buffer to save state in
    DWORD	 freezeBufferSize = 1;	// Size of freeze buffer
    DWORD	 freezeBufferFill;		// Number of bytes used in freezeBuffer
	ULONG    ulError;

    switch (GetPlayerMode())
    {
	    case DPM_INACTIVE:
		case DPM_STOPPED:
			return E_FAIL;
			break;

        default:
            pfreezeBuffer = new BYTE[freezeBufferSize];
	        freezeBufferFill = freezeBufferSize;

		    //
		    // Try freezing and adapt buffer size if necessary

		    ulError = DDP_Freeze(m_ddpThePlayer, pfreezeBuffer, freezeBufferFill);
            if (IS_GNERROR(ulError))
            {
		        if (freezeBufferFill > freezeBufferSize)
                {
			        freezeBufferSize = freezeBufferFill;
                }
                if (pfreezeBuffer)
                {
   			        delete[] pfreezeBuffer;
                }
                pfreezeBuffer = new BYTE[freezeBufferSize];
    		    ulError = DDP_Freeze(m_ddpThePlayer, pfreezeBuffer, freezeBufferFill);
            }

			HRESULT hr = ConvertError(ulError);
			if (FAILED(hr))
			{
                if (pfreezeBuffer)
    			    delete[] pfreezeBuffer;

				return hr;
			}

			ppFreezeBuffer = pfreezeBuffer;
			dwBufferSize = freezeBufferSize;

//            if (pfreezeBuffer)
//            {
//  			    delete[] pfreezeBuffer;
//            }
            break;
    }

	return S_OK;
}

HRESULT DVDMediaPlayer::Defrost(BYTE* pFreezeBuffer, DWORD dwBufferSize)
{
	if (pFreezeBuffer)
	{
        ULONG ulError = DDP_Defrost(m_ddpThePlayer, pFreezeBuffer, dwBufferSize);

		HRESULT hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}
    }

	return S_OK;
}

// Enables or disables audio playback during a scan of 2x or less
HRESULT DVDMediaPlayer::EnableScanAudio(bool bEnable)
{
	ULONG ulError = VDR_ConfigureUnitsTags(m_Units, SET_MPEG2_AUDIO_VOLUME_DAMP((BOOL) !bEnable), TAGDONE);
	HRESULT hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

// Determine if the decoder has enabled colorkey
HRESULT DVDMediaPlayer::GetColorkeyStatus(BOOL &bEnabled)
{
	ULONG ulError = VDR_ConfigureUnitsTags(m_Units, GET_PIP_COLORKEYED(bEnabled), TAGDONE);
	HRESULT hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

// Determine if the decoder has enabled colorkey
HRESULT DVDMediaPlayer::SetColorkey(DWORD dwColorkey)
{
	ULONG ulError = VDR_ConfigureUnitsTags(m_Units, SET_PIP_COLORKEY_COLOR(dwColorkey), TAGDONE);
	HRESULT hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

HRESULT DVDMediaPlayer::SetDestinationRect(INT leftX, INT topY, INT width, INT height)
{
	ULONG   ulError;
	HRESULT hr;

	// If the width or height is 0, this is an empty region.  An empty region means
	// the component is supposed to restore the original 1X zoom.
	if (width == 0  ||  height == 0)
	{
		// Setting adapt source size to true restores the original 1X scale
		ulError = VDR_ConfigureUnitsTags(m_Units, SET_PIP_ADAPT_SOURCE_SIZE(TRUE), TAGDONE);

	}
	else
	{
		ulError = VDR_ConfigureUnitsTags(m_Units,
			SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
			SET_PIP_DEST_LEFT(leftX),
			SET_PIP_DEST_TOP(topY),
			SET_PIP_DEST_WIDTH(width),
			SET_PIP_DEST_HEIGHT(height),
			TAGDONE);

		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}
	}
	return S_OK;
}


HRESULT DVDMediaPlayer::SetOutputRect(INT leftX, INT topY, INT width, INT height)
{
	ULONG   ulError;
	HRESULT hr;

	ulError = VDR_ConfigureUnitsTags(m_Units,
			SET_PIP_DISPLAY_LEFT((short)leftX),
			SET_PIP_DISPLAY_TOP((short)topY),
			SET_PIP_DISPLAY_WIDTH((short)width),
			SET_PIP_DISPLAY_HEIGHT((short)height),
			TAGDONE);

	hr = ConvertError(ulError);
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}


HRESULT DVDMediaPlayer::Get_CurrentScaleFactor(float& cSF)
{
		unsigned short          currentWidth;
		unsigned short          maxWidth;

//		float					currentSF;
		ULONG ulError;
		HRESULT hr;

		// Get the current video size
		ulError = VDR_ConfigureUnitsTags(m_Units,	GET_PIP_SOURCE_WIDTH((int&)currentWidth),
													TAGDONE);
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		// Get max video sizes
		ulError = VDR_ConfigureUnitsTags(m_Units,	GET_MPEG_DISPLAY_WIDTH(maxWidth),
													TAGDONE);
		hr = ConvertError(ulError);
		if (FAILED(hr))
		{
			return hr;
		}

		cSF = (float) maxWidth / (float) currentWidth;
		return S_OK;
}

void DVDMediaPlayer::EventHandlerInstall(HWND EventWnd)
	{
	if(!m_ddpThePlayer)
	{
		return;
	}

	if(EventWnd)
		m_eventWindow = EventWnd;
	else
		MessageBox(NULL, "DVDMediaPlayer::EventHandlerInstall - NO Event Window Handle passed!", "Test Player", MB_OK);


//	eventRead = eventWrite = 0;
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_TITLE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PART_OF_TITLE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_VALID_UOP_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_ANGLE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_AUDIO_STREAM_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_SUBPICTURE_STREAM_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_DOMAIN_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PARENTAL_LEVEL_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_BITRATE_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_STILL_ON, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_STILL_OFF, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PLAYBACK_MODE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_CDA_PLAYING_GAP, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_READ_ERROR, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_DISPLAY_MODE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_STREAMS_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_SCAN_SPEED_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PLAYBACK_SPEED_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_VIDEO_STANDARD_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_BREAKPOINT_REACHED, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_DRIVE_DETACHED, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_ERROR, DDPCaptureEvent, 0);
	}

void DVDMediaPlayer::EventHandlerInstall(PEVENT_CALLBACK pEventCallback)
{
	if(!m_ddpThePlayer)
	{
		return;
	}

	if(pEventCallback)
		m_pEventCallback = pEventCallback;

//	eventRead = eventWrite = 0;
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_TITLE_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PART_OF_TITLE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_VALID_UOP_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_ANGLE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_AUDIO_STREAM_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_SUBPICTURE_STREAM_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_DOMAIN_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PARENTAL_LEVEL_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_BITRATE_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_STILL_ON, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_STILL_OFF, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PLAYBACK_MODE_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_CDA_PLAYING_GAP, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_READ_ERROR, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_DISPLAY_MODE_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_STREAMS_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_SCAN_SPEED_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_PLAYBACK_SPEED_CHANGE, DDPCaptureEvent, 0);
//	DDP_InstallEventHandler(m_ddpThePlayer, DNE_VIDEO_STANDARD_CHANGE, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_BREAKPOINT_REACHED, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_DRIVE_DETACHED, DDPCaptureEvent, 0);
	DDP_InstallEventHandler(m_ddpThePlayer, DNE_ERROR, DDPCaptureEvent, 0);
}


void DVDMediaPlayer::EventHandlerRemove(void)
	{
	if (!m_ddpThePlayer)
		return;

	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_TITLE_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_PART_OF_TITLE_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_VALID_UOP_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_ANGLE_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_AUDIO_STREAM_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_SUBPICTURE_STREAM_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_DOMAIN_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_PARENTAL_LEVEL_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_BITRATE_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_STILL_ON);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_STILL_OFF);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_PLAYBACK_MODE_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_CDA_PLAYING_GAP);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_READ_ERROR);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_DISPLAY_MODE_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_STREAMS_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_SCAN_SPEED_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_PLAYBACK_SPEED_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_VIDEO_STANDARD_CHANGE);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_BREAKPOINT_REACHED);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_DRIVE_DETACHED);
	DDP_RemoveEventHandler(m_ddpThePlayer, DNE_ERROR);
	}

//#endif //if 0 for xbox test
///////////////////////////////////////////////////////////////////////////////
// DDPCaptureEvent() -	this is a callback function called by the DDP layer when
//						ever the request event (dwEvent) occurs. For a list of
//						events and the prototype of the callback function, see DVDCPL32.H
// Arguments:    DWORD dwEvent - the event that occurred causing this function to be called.
//               VOID* pvBuffer - any data the was passed to the InstallHandler function.
//               DWORD dwInfo - contains
// Return:       nothing
///////////////////////////////////////////////////////////////////////////////
/*
void  WINAPI DDPCaptureEvent(DWORD dwEvent, VOID* pvBuffer, DWORD dwInfo)
{
	if (DVDMediaPlayer::m_eventWindow == 0)
	{
		return;
	}
	switch(dwEvent)
	{
		case DNE_TITLE_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_TITLE_CHANGE,
							dwInfo );
			break;
		case DNE_PART_OF_TITLE_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_CHAPTER_START,
							dwInfo);
			break;
		case DNE_VALID_UOP_CHANGE:
			pMediaPlayer->m_dwCurrentUOPs = dwInfo;
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_VALID_UOPS_CHANGE,
							dwInfo);
			break;
		case DNE_ANGLE_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_ANGLE_CHANGE,
							dwInfo);
			break;
		case DNE_AUDIO_STREAM_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_AUDIO_STREAM_CHANGE,
							dwInfo);
			break;
		case DNE_SUBPICTURE_STREAM_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_SUBPICTURE_STREAM_CHANGE,
							dwInfo);
			break;
		case DNE_DOMAIN_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_DOMAIN_CHANGE,
							dwInfo+1);
			break;
		case DNE_PARENTAL_LEVEL_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_PARENTAL_LEVEL_CHANGE,
							dwInfo);
			break;
		case DNE_STILL_ON:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_STILL_ON,
							dwInfo);
			break;
		case DNE_STILL_OFF:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_STILL_OFF,
							dwInfo);
			break;
		case DNE_PLAYBACK_MODE_CHANGE:
			switch (dwInfo)
			{
				case DPM_STOPPED:
					::PostMessage(DVDMediaPlayer::m_eventWindow, WM_RAVISENT_EVENT, EC_DVD_PLAYBACK_STOPPED, dwInfo);
					pMediaPlayer->m_lMode = QMODE_STOPPED;
					break;

				case DPM_PLAYING:
					pMediaPlayer->m_bForwardPlayback = TRUE;
					pMediaPlayer->m_lMode = QMODE_PLAYING;
					break;

				case DPM_REVERSEPLAY:
					pMediaPlayer->m_bForwardPlayback = FALSE;
					pMediaPlayer->m_lMode = QMODE_TRICKREVERSE;
					break;

				case DPM_PAUSED:
					pMediaPlayer->m_lMode = QMODE_PAUSED;
					break;
				case DPM_STILL:
					pMediaPlayer->m_lMode = QMODE_STILL;
					break;
				case DPM_BUSY:
					pMediaPlayer->m_lMode = QMODE_BUSY;
					break;
				case DPM_SCANNING:
					pMediaPlayer->m_lMode = QMODE_SEEKING;
					break;
				case DPM_INACTIVE:
					pMediaPlayer->m_lMode = QMODE_STOPPED;
					break;
				case DPM_TRICKPLAY:
					pMediaPlayer->m_lMode = QMODE_TRICKFRAME;
					break;
				default:
					pMediaPlayer->m_lMode = QMODE_NONE;
					break;
			}
			break;

		case DNE_READ_ERROR:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_ERROR,
							dwInfo);
			break;
		case DNE_STREAMS_CHANGE:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_SUBPICTURE_STREAM_CHANGE,
							dwInfo );
			break;
		case DNE_PLAYBACK_SPEED_CHANGE:
			pMediaPlayer->m_dPlaySpeed = (double)dwInfo / (double)C_NORMAL_PLAYBACK_SPEED;   // dwInfo will be 500, 1000, 1500, etc
			// Changing playback speed to a fast rate (rate>2.0) causes a switch between standard playback mode
			// and scan playback mode. This situation causes the navigator to set standard playback rate to 1.0
			// and increase the scan rate instead.  Because the navigate sets that standard rate to 1.0, an event
			// is sent to this callback for BOTH this new 1.0 playback speed and the new rapid scan rate.  This "if"
			// will prevent that 1.0 playback event from being sent to the application.
			if (pMediaPlayer->m_dPlaySpeed != 1.0  ||  pMediaPlayer->m_dScanSpeed == 1.0)
			{
				// The application is only notified of this event if one of the following conditions are true:
				// 1) (m_dPlaySpeed != 1.0) - The event signals that the non-scan playback speed just changed
				// 2) (m_dPlaySpeed == 1.0 && pMediaPlayer->m_dScanSpeed == 1.0) - The scan playback speed shows
				//                            that we are not scanning, so this change to a non-playback speed
				//                            of 1.0 means we've returned to 1.0 after traveling at another non
				//                            scan playback rate.  (i.e. From 1.5 to 1.0)
				::PostMessage(	DVDMediaPlayer::m_eventWindow,
								WM_RAVISENT_EVENT,
								EC_DVD_PLAYBACK_RATE_CHANGE,
								dwInfo);
			}
			break;
		case DNE_SCAN_SPEED_CHANGE:
			pMediaPlayer->m_dScanSpeed = (double)dwInfo;     // dwInfo will be 2, 3, 4 etc
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_PLAYBACK_RATE_CHANGE,
							dwInfo);
			break;
		case DNE_DVD_RAVISENTTIMEOUT:
			// Cinemaster DLL has timed out!  Buy the software!!!!
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_ERRORABORT,
							VFW_E_DVD_RAVISENTTIMEOUT);
			break;
		case DNE_DISPLAY_MODE_CHANGE:
			pMediaPlayer->m_lCurrentDisplayMode = dwInfo;
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_DISPLAY_MODE_CHANGE,
							dwInfo);
			break;
		case DNE_BREAKPOINT_REACHED:
			::PostMessage(	DVDMediaPlayer::m_eventWindow,
							WM_RAVISENT_EVENT,
							EC_DVD_PLAYPERIOD_AUTOSTOP,
							dwInfo);
			break;
		case DNE_NONE:
			// no corresponding event in DShow, so we won't send these.
			break;
	}
}
*/

void  WINAPI DDPCaptureEvent(DWORD dwEvent, VOID* pvBuffer, DWORD dwInfo)
{
	if (DVDMediaPlayer::m_pEventCallback == 0)
	{
		return;
	}
	switch(dwEvent)
	{
		case DNE_TITLE_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_TITLE_CHANGE, dwInfo);
							break;

		case DNE_PART_OF_TITLE_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_CHAPTER_START, dwInfo);
							break;

		case DNE_VALID_UOP_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_VALID_UOPS_CHANGE, dwInfo);
							break;

		case DNE_ANGLE_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_ANGLE_CHANGE, dwInfo);
							break;

		case DNE_AUDIO_STREAM_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_AUDIO_STREAM_CHANGE, dwInfo);
							break;

		case DNE_SUBPICTURE_STREAM_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_SUBPICTURE_STREAM_CHANGE, dwInfo);
							break;

		case DNE_DOMAIN_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_DOMAIN_CHANGE, dwInfo+1);
							break;

		case DNE_PARENTAL_LEVEL_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_PARENTAL_LEVEL_CHANGE, dwInfo);
							break;

		case DNE_STILL_ON:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_STILL_ON, dwInfo);
							break;

		case DNE_STILL_OFF:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_STILL_OFF, dwInfo);
							break;

		case DNE_PLAYBACK_MODE_CHANGE:
							switch (dwInfo)
							{
								case DPM_STOPPED:
									DVDMediaPlayer::m_pEventCallback(EC_DVD_PLAYBACK_STOPPED, dwInfo);
									pMediaPlayer->m_lMode = QMODE_STOPPED;
									break;

								case DPM_PLAYING:
									if ((pMediaPlayer->m_dScanSpeed != 1.0) && (pMediaPlayer->m_dPlaySpeed == 1.0))
									{
										DVDMediaPlayer::m_pEventCallback(EC_DVD_PLAYBACK_RATE_CHANGE,
											pMediaPlayer->m_dPlaySpeed * C_NORMAL_PLAYBACK_SPEED);
									}

									pMediaPlayer->m_dScanSpeed = 1.0;

									pMediaPlayer->m_bForwardPlayback = TRUE;
									pMediaPlayer->m_lMode = QMODE_PLAYING;
									break;

								case DPM_REVERSEPLAY:
									pMediaPlayer->m_bForwardPlayback = FALSE;
									pMediaPlayer->m_lMode = QMODE_TRICKREVERSE;
									break;

								case DPM_PAUSED:
									pMediaPlayer->m_lMode = QMODE_PAUSED;
									break;
								case DPM_STILL:
									pMediaPlayer->m_lMode = QMODE_STILL;
									break;
								case DPM_BUSY:
									pMediaPlayer->m_lMode = QMODE_BUSY;
									break;
								case DPM_SCANNING:
									pMediaPlayer->m_lMode = QMODE_SEEKING;
									break;
								case DPM_INACTIVE:
									pMediaPlayer->m_lMode = QMODE_STOPPED;
									break;
								case DPM_TRICKPLAY:
									pMediaPlayer->m_lMode = QMODE_TRICKFRAME;
									break;
								default:
									pMediaPlayer->m_lMode = QMODE_NONE;
									break;
							}
							break;

		case DNE_READ_ERROR:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_ERROR, dwInfo);
							break;

		case DNE_STREAMS_CHANGE:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_SUBPICTURE_STREAM_CHANGE, dwInfo );
							break;

		case DNE_PLAYBACK_SPEED_CHANGE:
							// dwInfo will be 500, 1000, 1500, etc
							pMediaPlayer->m_dPlaySpeed = (double)dwInfo / (double)C_NORMAL_PLAYBACK_SPEED;

							// Changing playback speed to a fast rate (rate>2.0) causes a switch
							// between standard playback mode
							// and scan playback mode. This situation causes the navigator to set
							// standard playback rate to 1.0
							// and increase the scan rate instead.  Because the navigate sets that standard
							// rate to 1.0, an event
							// is sent to this callback for BOTH this new 1.0 playback speed
							// and the new rapid scan rate.  This "if"
							// will prevent that 1.0 playback event from being sent to the application.
							if (pMediaPlayer->m_dPlaySpeed != 1.0  ||  pMediaPlayer->m_dScanSpeed == 1.0)
							{
								// The application is only notified of this event if one of the following
								// conditions are true:
								// 1) (m_dPlaySpeed != 1.0) - The event signals
								// that the non-scan playback speed just changed
								// 2) (m_dPlaySpeed == 1.0 && pMediaPlayer->m_dScanSpeed == 1.0) -
								// The scan playback speed shows
								// that we are not scanning, so this change to a non-playback speed
								// of 1.0 means we've returned to 1.0 after traveling at another non
								// scan playback rate.  (i.e. From 1.5 to 1.0)
								DVDMediaPlayer::m_pEventCallback(EC_DVD_PLAYBACK_RATE_CHANGE, dwInfo);
							}
							break;

		case DNE_SCAN_SPEED_CHANGE:
							pMediaPlayer->m_dScanSpeed = (double)dwInfo;     // dwInfo will be 2, 3, 4 etc
							DVDMediaPlayer::m_pEventCallback(EC_DVD_PLAYBACK_RATE_CHANGE, dwInfo);
							break;

		case DNE_DVD_RAVISENTTIMEOUT:
							// Cinemaster DLL has timed out!  Buy the software!!!!
							DVDMediaPlayer::m_pEventCallback(EC_ERRORABORT, (DWORD) VFW_E_DVD_RAVISENTTIMEOUT);
							break;

		case DNE_DISPLAY_MODE_CHANGE:
							pMediaPlayer->m_lCurrentDisplayMode = dwInfo;
							DVDMediaPlayer::m_pEventCallback(EC_DVD_DISPLAY_MODE_CHANGE, dwInfo);
							break;

		case DNE_BREAKPOINT_REACHED:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_PLAYPERIOD_AUTOSTOP, dwInfo);
							break;

		case DNE_ERROR:
							DVDMediaPlayer::m_pEventCallback(EC_DVD_ERROR, dwInfo);
							break;
		case DNE_NONE:
							// no corresponding event in DShow, so we won't send these.
							break;
	}
}

