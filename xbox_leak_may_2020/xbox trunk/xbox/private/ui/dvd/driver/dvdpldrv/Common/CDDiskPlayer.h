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

//////////////////////////////////////////////////////////////////////
//
//  CDDiskPlayer Class
//
//////////////////////////////////////////////////////////////////////

#ifndef CDDISKPLAYER_H
#define CDDISKPLAYER_H

#include "Library/Common/WinPorts.h"
#include "NavErrors.h"
#include "EventSender.h"
#include "Breakpoint.h"
#include "Library/Files/RootFile.h"
//#include "Boards/Generic/Vdgendrv.h"

#pragma warning(disable : 4250)

//
//  Defines
//

#define STILL_TIME_MSG		2001

//
//  Macros for Extended Player State access
//

#define EPS_REQUEST(x)	(appEPS->request & x)
#define EPS_MONITOR(x)	(appEPS->monitor & x)
#define EPS_CHANGED(x)	(appEPS->x != eps.x)

////////////////////////////////////////////////////////////////////
//
//  CDDiskPlayerClass
//
//  This is an abstract base class for all Disk Player Classes
//
////////////////////////////////////////////////////////////////////

class CDDiskPlayerClass : virtual public EventSender, virtual public ERSBreakpointControl
	{
	protected:
		ExtendedPlayerState eps;
		DVDDiskType diskType;
		EventDispatcher eventDispatcherInstance; //(one and only) EventDispatcher object that should be used by Disk Player Classes

		virtual Error InternalGetMode(DVDPlayerMode & mode) = 0;

		virtual Error IsPlayingForward(BOOL & forward) = 0;

		virtual Error DiskIsEncrypted(BOOL & enc);

	public:
				  CDDiskPlayerClass(void);

		virtual ~CDDiskPlayerClass(void);

		virtual Error Init(WinPortServer * server, GenericProfile * profile, DVDDiskType diskType, GenericFileSystem * gfs);

		virtual Error Cleanup(void);

		virtual Error GetDiskType(DVDDiskType & type);

		virtual Error GetMode(DVDPlayerMode & mode) = 0;

		virtual Error GetExtendedPlayerState(ExtendedPlayerState * eps);

		virtual Error StopStillPhase(void) = 0;

		virtual Error Exit(Error err = GNR_OK) = 0;

		virtual Error CallMenu(VTSMenuType menu) = 0;

		virtual Error PausePlayback(void) = 0;

		virtual Error ResumePlayback(void) = 0;

		virtual Error SetPlaybackControl(BOOL playbackControl);

		virtual Error AdvanceFrame(void) = 0;

		virtual Error AdvanceFrameBy(int n) = 0;

		virtual Error SetPlaybackSpeed(WORD speed) = 0;

		virtual Error GetPlaybackSpeed(WORD & speed) = 0;

		virtual Error GetAudioStreamAttributes(WORD stream, DVDAudioStreamFormat	& attributes) = 0;

		virtual Error GetSubPictureStreamAttributes(WORD stream, DVDSubPictureStreamFormat	& attributes) = 0;

		virtual Error StartPresentation(DWORD flags) = 0;

		virtual Error GetCurrentLocation(DVDLocation & location) = 0;

		virtual Error GetCurrentDuration(DVDLocation & location) = 0;

		virtual Error GetTitleDuration(WORD title, DVDTime & duration) = 0;

		virtual Error MenuAvail(VTSMenuType menu, BOOL & avail) = 0;

		virtual Error GetUOPs(DWORD & uops);

		virtual Error UOPPermitted(int uops, BOOL & permitted) = 0;

		virtual Error GetTitleSearchUOPs(WORD title, DWORD & uops);

		virtual Error TitleSearchUOPPermitted(WORD title, int uops, BOOL & permitted);

		virtual Error NumberOfTitles(WORD & num) = 0;

		virtual Error NumberOfPartOfTitle(WORD title, WORD & num) = 0;

		virtual Error GetAvailStreams(BYTE & audio, DWORD & subPicture) = 0;

		virtual Error GetCurrentAudioStream(WORD & stream) = 0;

		virtual Error GetCurrentSubPictureStream(WORD & stream) = 0;

		virtual Error IsCurrentSubPictureEnabled(BOOL & enabled) = 0;

		virtual Error GetNumberOfAngles(WORD title, WORD & num) = 0;

		virtual Error GetCurrentAngle(WORD & angle) = 0;

		virtual Error TitlePlay(WORD title, DWORD flags = DDPSPF_NONE) = 0;

		virtual Error PTTPlay(WORD title, WORD part) = 0;

		virtual Error TimePlay(WORD title, DVDTime time) = 0;

		virtual Error GoUpProgramChain(void) = 0;

		virtual Error TimeSearch(DVDTime time) = 0;

		virtual Error PTTSearch(WORD part) = 0;

		virtual Error GoPrevProgram(void) = 0;

		virtual Error GoTopProgram(void) = 0;

		virtual Error GoNextProgram(void) = 0;

		virtual Error ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time) = 0;

		virtual Error StartForwardScan(WORD speed) = 0;

		virtual Error StartBackwardScan(WORD speed) = 0;

		virtual Error GetScanSpeed(WORD & speed) = 0;

		virtual Error StopScan(void) = 0;

		virtual Error StartReversePlayback(void) = 0;

		virtual Error StartTrickplay(void) = 0;

		virtual Error ResumeFromSystemSpace(void) = 0;

		virtual Error ButtonUp(void) = 0;

		virtual Error ButtonDown(void) = 0;

		virtual Error ButtonLeft(void) = 0;

		virtual Error ButtonRight(void) = 0;

		virtual Error ButtonSelectAt(WORD x, WORD y) = 0;

		virtual Error ButtonActivate(void) = 0;

		virtual Error ButtonSelectAndActivate(WORD button) = 0;

		virtual Error ButtonSelectAtAndActivate(WORD x, WORD y) = 0;

		virtual Error IsButtonAt(WORD x, WORD y, BOOL & isButton) = 0;

		virtual Error HasPositionalButtons(BOOL & hasButtons) = 0;

		virtual Error MenuLanguageSelect(WORD language) = 0;

		virtual Error GetMenuLanguage(WORD & lang) = 0;

		virtual Error AudioStreamChange(WORD stream) = 0;

		virtual Error SubPictureStreamChange(WORD stream, BOOL enable) = 0;

		virtual Error AngleChange(WORD angle) = 0;

		virtual Error ParentalLevelSelect(WORD level) = 0;

		virtual Error ParentalCountrySelect(WORD country) = 0;

		virtual Error GetCurrentDisplayMode(DisplayPresentationMode & mode) = 0;

		virtual Error SetDisplayMode(DisplayPresentationMode mode) = 0;

		virtual Error GetCurrentVideoStandard(VideoStandard & standard) = 0;

		virtual Error SelectInitialLanguage(WORD audioLanguage, WORD audioExtension, WORD subPictureLanguage, WORD subPictureExtension) = 0;

		virtual Error GetCurrentBitrate(DWORD & bitrate) = 0;

		virtual Error GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton) = 0;

		virtual Error Freeze (DDPFreezeState * state, DWORD & length) = 0;

		virtual Error Defrost(DDPFreezeState * state, DWORD & length, DWORD flags) = 0;

//		virtual Error SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id) = 0;

//		virtual Error ClearBreakpoint(DWORD id) = 0;

		virtual Error GetAVFileInfo(GenericFileSystemIterator *gfsi, AVFInfo *pInfo);

		virtual Error PlayFile(GenericFileSystemIterator *gfsi, DWORD flags);

		virtual Error SetAudioStreamSelectionPreferences(DWORD flags);

		virtual Error EPSPermitted(BOOL & permitted);

	};

#pragma warning(default : 4250)

#endif
