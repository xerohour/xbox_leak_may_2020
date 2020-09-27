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
//  AVFDiskPlayerClass
//
//  Implements the Audio/Video File Player
//
////////////////////////////////////////////////////////////////////

#ifndef AVFDISKPLAYER_H
#define AVFDISKPLAYER_H

#include "Driver/Dvdpldrv/Common/CDDiskPlayer.h"
#include "AVFStreamServer.h"
#include "Driver/Dvdpldrv/Common/Config/DiskPlayerFactory.h"

// These settings are used in AVFDiskPlayerClass::GetAVFileInfo to prevent
// dropouts during playback.  Currently only the "healthiest" and "longest"
// are used.  These numbers were fine-tuned according to the Vestel ATAPI
// 5508 player using 320 kbps MP3 files.  The "buffer bytes" should probably
// based upon the layer's buffer depth in a dynamic way, rather than using
// a hard-coded constant as I have done here.
#define AVF_VERY_LOW_BUFFERED_BYTES_FOR_CONCURRENT_DISK_SEEK	0x12000
#define AVF_LOW_BUFFERED_BYTES_FOR_CONCURRENT_DISK_SEEK			0x14000
#define AVF_HEALTHY_BUFFERED_BYTES_FOR_CONCURRENT_DISK_SEEK		0x16000
#define AVF_HEALTHIEST_BUFFERED_BYTES_FOR_CONCURRENT_DISK_SEEK	0x18000
#define AVF_CONCURRENT_DISK_SEEK_WAIT_MILLISEC						100
#define AVF_LONGER_CONCURRENT_DISK_SEEK_WAIT_MILLISEC				150
#define AVF_LONGEST_CONCURRENT_DISK_SEEK_WAIT_MILLISEC			200
#define AVF_MAX_CONCURRENT_DISK_SEEK_WAIT_LOOPS						15

enum AVFDiskPlayerTitleAccessMethod
	{
	AVFDP_FILE_ACCESS_METHOD_UNKNOWN,
	AVFDP_ACCESS_FILES_BY_ITERATOR,
	AVFDP_ACCESS_FILES_BY_TITLENUM
	};

#pragma warning(disable : 4250)

class AVFDiskPlayerClass : public CDDiskPlayerClass, public AVFStreamServer
	{
	protected:
		GenericFileSystem * avffs;

		// fileInfoReader is used for asynchronous file scanning, possibly during file playback
		AVFileManager				fileInfoReader;

		DWORD numberOfTitles;
		DWORD currentTitle;
		GenericFileSystemIterator * currentTitleIt;
		GenericFileSystemIterator * rootIt;

		BOOL	newFileEPSLatch;

		BOOL	GetNewFileEPSLatch(void) { return newFileEPSLatch; }
		void	ClearNewFileEPSLatch() { newFileEPSLatch = FALSE; }
		void	SetNewFileEPSLatch() { newFileEPSLatch = TRUE; }

		virtual Error InternalGetMode(DVDPlayerMode & mode);
		virtual Error IsPlayingForward(BOOL & forward);
		virtual void PlaybackCompleted(void);

		// some functions with 'title' in the parameter list will behave
		// differently when used with an application that only uses title
		// numbers and an app that only uses a gfsi.  The titleAccessMethod
		// member determines which access method is to be used.
		AVFDiskPlayerTitleAccessMethod	titleAccessMethod;
		void SetTitleAccessMethod(AVFDiskPlayerTitleAccessMethod mode)
			{ titleAccessMethod = mode; }
		AVFDiskPlayerTitleAccessMethod GetTitleAccessMethod(void)
			{ return titleAccessMethod; }

		void InitializeExtendedPlayerState(ExtendedPlayerState * pEps);

	public:
		AVFDiskPlayerClass(WinPortServer * server, UnitSet units, AVFDiskPlayerFactory * factory);

		~AVFDiskPlayerClass(void);

		virtual Error Init(WinPortServer * server, GenericProfile * profile, DVDDiskType diskType, GenericFileSystem * gfs);

		virtual Error GetMode(DVDPlayerMode & mode);

		virtual Error GetExtendedPlayerState(ExtendedPlayerState * eps);

		// virtual Error GetFileInfo(AVFInfo * info); // inherited from AVFStreamServer

		virtual Error StopStillPhase(void);

		virtual Error Exit(Error err = GNR_OK);

		virtual Error CallMenu(VTSMenuType menu);

		virtual Error PausePlayback(void);

		virtual Error ResumePlayback(void);

		virtual Error AdvanceFrame(void);

		virtual Error AdvanceFrameBy(int n);

		virtual Error SetPlaybackSpeed(WORD speed);

		virtual Error GetPlaybackSpeed(WORD & speed);

		virtual Error GetAudioStreamAttributes(WORD stream, DVDAudioStreamFormat	& attributes);

		virtual Error GetSubPictureStreamAttributes(WORD stream, DVDSubPictureStreamFormat	& attributes);

		virtual Error StartPresentation(DWORD flags);

		virtual Error GetCurrentLocation(DVDLocation & location);

		virtual Error GetCurrentDuration(DVDLocation & location);

		virtual Error GetTitleDuration(WORD title, DVDTime & duration);

		virtual Error GetCurrentTitleDuration(DVDTime & duration);

		virtual Error MenuAvail(VTSMenuType menu, BOOL & avail);

		virtual Error GetUOPs(DWORD & uops);

		virtual Error UOPPermitted(int uops, BOOL & permitted);

		virtual Error NumberOfTitles(WORD & num);

		virtual Error NumberOfPartOfTitle(WORD title, WORD & num);

		virtual Error GetAvailStreams(BYTE & audio, DWORD & subPicture);

		virtual Error GetCurrentAudioStream(WORD & stream);

		virtual Error GetCurrentSubPictureStream(WORD & stream);

		virtual Error IsCurrentSubPictureEnabled(BOOL & enabled);

		virtual Error GetNumberOfAngles(WORD title, WORD & num);

		virtual Error GetCurrentAngle(WORD & num);

		virtual Error TitlePlay(WORD title, DWORD flags = DDPSPF_NONE);

		virtual Error PTTPlay(WORD title, WORD part);

		virtual Error TimePlay(WORD title, DVDTime time);

		virtual Error GoUpProgramChain(void);

		virtual Error TimeSearch(DVDTime time);

		virtual Error PTTSearch(WORD part);

		virtual Error GoPrevProgram(void);

		virtual Error GoTopProgram(void);

		virtual Error GoNextProgram(void);

		virtual Error ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time);

		virtual Error StartForwardScan(WORD speed);

		virtual Error StartBackwardScan(WORD speed);

		virtual Error GetScanSpeed(WORD & speed);

		virtual Error StartTrickplay(void);

		virtual Error StopScan(void);

		virtual Error StartReversePlayback(void);

		virtual Error ResumeFromSystemSpace(void);

		virtual Error ButtonUp(void);

		virtual Error ButtonDown(void);

		virtual Error ButtonLeft(void);

		virtual Error ButtonRight(void);

		virtual Error ButtonSelectAt(WORD x, WORD y);

		virtual Error ButtonActivate(void);

		virtual Error ButtonSelectAndActivate(WORD button);

		virtual Error ButtonSelectAtAndActivate(WORD x, WORD y);

		virtual Error IsButtonAt(WORD x, WORD y, BOOL & isButton);

		virtual Error HasPositionalButtons(BOOL & hasButtons);

		virtual Error MenuLanguageSelect(WORD language);

		virtual Error GetMenuLanguage(WORD & language);

		virtual Error AudioStreamChange(WORD stream);

		virtual Error SubPictureStreamChange(WORD stream, BOOL enable);

		virtual Error AngleChange(WORD angle);

		virtual Error ParentalLevelSelect(WORD level);

		virtual Error ParentalCountrySelect(WORD country);

		virtual Error GetCurrentDisplayMode(DisplayPresentationMode & mode);

		virtual Error SetDisplayMode(DisplayPresentationMode mode);

		virtual Error GetCurrentVideoStandard(VideoStandard & standard);

		virtual Error SelectInitialLanguage(WORD audioLanguage, WORD audioExtension, WORD subPictureLanguage, WORD subPictureExtension);

		virtual Error GetCurrentBitrate(DWORD & bitrate);

		virtual Error GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton);

		virtual Error Freeze (DDPFreezeState * buffer, DWORD & size);

		virtual Error Defrost(DDPFreezeState * buffer, DWORD & size, DWORD flags);

		virtual Error SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id);

		virtual Error ClearBreakpoint(DWORD id);

		virtual Error	GetAVFileInfo(GenericFileSystemIterator *gfsi, AVFInfo *pInfo);

		virtual Error PlayFile(GenericFileSystemIterator *gfsi, DWORD flags);
	};

#pragma warning(default : 4250)

#endif

