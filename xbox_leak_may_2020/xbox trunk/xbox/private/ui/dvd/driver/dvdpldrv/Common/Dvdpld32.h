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
//  DDP Interface Declaration
//
////////////////////////////////////////////////////////////////////

#ifndef DVDPLD32_H
#define DVDPLD32_H

////////////////////////////////////////////////////////////////////
//
//  General Includes
//
////////////////////////////////////////////////////////////////////

#include "Library/Files/VFI.h"
#include "DVDTime.h"
#include "NavErrors.h"

typedef class CDDiskPlayerClass * DVDDiskPlayer;

////////////////////////////////////////////////////////////////////
//
//  Internal Stuff
//
////////////////////////////////////////////////////////////////////

#ifndef ONLY_EXTERNAL_VISIBLE

#include "Library/Common/WinPorts.h"
#include "Config/DiskPlayerFactory.h"
#include "Config/DVDDiskPlayerFactory.h"
#include "Config/FileSystemFactory.h"
#include "Library/Common/Profiles.h"
#include "Library/Hardware/Drives/Generic/DriveTable.h"

//
//  DDP_Init() and DDP_Cleanup() are internal functions only
//  They should not be called from outside the configuration/platform dependent files
//

DLLCALL Error WINAPI DDP_Init(WinPortServer					* driverInstance,
										GenericProfile					* profile,
										DriveTable						* driveTable,
										DVDDiskPlayerFactory			* dvdVideoDiskPlayerFactory,
										DVDDiskPlayerFactory			* dvdAudioDiskPlayerFactory,
										VCDDiskPlayerFactory			* vcdDiskPlayerFactory,
										CDDADiskPlayerFactory		* cddaDiskPlayerFactory,
										AVFDiskPlayerFactory			* avfDiskPlayerFactory,
										FileSystemFactory				* dvdFileSystemFactory,
										FileSystemFactory				* vcdFileSystemFactory,
										FileSystemFactory				* cddaFileSystemFactory,
										FileSystemFactory				* avfFileSystemFactory,
										FileSystemFactory				* osFileSystemFactory);

DLLCALL Error WINAPI  DDP_Cleanup(void);

//
//  Special declarations for ST20LITE
//

DLLCALL Error WINAPI DDP_Initialize(void);

#endif	// ONLY_EXTERNAL_VISIBLE

#include "boards/generic/vdgendrv.h"

////////////////////////////////////////////////////////////////////
//
//  Interface declaration
//
////////////////////////////////////////////////////////////////////

extern "C" {

DLLCALL Error WINAPI  DDP_CheckDrive(char driveLetter, char __far * id);

DLLCALL Error WINAPI  DDP_CheckDriveWithPath(char * drivePath, char __far * id);

DLLCALL Error WINAPI  DDP_GetVolumeName(char driveLetter, char * name);

DLLCALL Error WINAPI  DDP_GetDiskRegionSettings(UnitSet units, char driveLetter, BYTE __far &region, BYTE __far &availSets, RegionSource __far & regionSource, BYTE & diskRegion);

DLLCALL Error WINAPI  DDP_GetRegionSettings(UnitSet units, char driveLetter, BYTE __far &region, BYTE __far &availSets, RegionSource __far &regionSource);

DLLCALL Error WINAPI  DDP_SetSystemRegion(UnitSet units, char driveLetter, BYTE region);

DLLCALL Error WINAPI  DDP_EjectDiskInDrive(char driveLetter);

#if MSNDISC
DLLCALL Error WINAPI  DDP_LoadMedia(char driveLetter, BOOL wait);
DLLCALL Error WINAPI  DDP_UnloadMedia(char driveLetter, BOOL wait);
DLLCALL Error WINAPI  DDP_GetDriveCaps(char driveLetter, DWORD &caps, DWORD &slots, DWORD& positions, DWORD& changerOpenOffset);
DLLCALL Error WINAPI  DDP_MoveChanger(char driveLetter, DWORD operation, DWORD flags, DWORD param);
DLLCALL Error WINAPI  DDP_GetChangerStatus(char driveLetter, DWORD& status, DWORD& position );
DLLCALL Error WINAPI  DDP_GetSlotStatus(char driveLetter, DWORD slot, DWORD& status );
#endif

DLLCALL Error WINAPI  DDP_GetDriveStatus(char driveLetter, BOOL & opened, BOOL & closed);

DLLCALL Error WINAPI  DDP_LoadDiskInDrive(char driveLetter, BOOL wait);

DLLCALL Error WINAPI  DDP_OpenPlayer(UnitSet units, char diskLetter, DVDDiskPlayer __far & player);

DLLCALL Error WINAPI  DDP_OpenPlayerWithPath(UnitSet units, char * drivePath, DVDDiskPlayer __far & player);

DLLCALL Error WINAPI  DDP_OpenPlayerExtended(UnitSet units, char driveLetter, char * drivePath,
															DVDDiskPlayer __far & player, DDPDiskInfo __far & diskInfo);

DLLCALL Error WINAPI  DDP_ClosePlayer(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_StartPresentation(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_StartPresentationExt(DVDDiskPlayer player, DWORD flags);

DLLCALL DVDDiskType	 WINAPI DDP_GetDiskType(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_GetDiskInfoAsync(char * drivePath, ARQAsyncRequestDoneCB ARQCallback, DDPDiskOnlyInfo* diskOnlyInfo, DWORD & dwRqHandle);

DLLCALL Error WINAPI  DDP_OpenPlayerAsync(char * drivePath, ARQAsyncRequestDoneCB ARQCallback, DDPOpenPlayerInfo* openPlayerInfo, DWORD & rqHandle);

DLLCALL Error WINAPI  DDP_AbortAsyncRequest(DWORD dwRqHandle);

DLLCALL Error WINAPI  DDP_GetCurrentLocation(DVDDiskPlayer player, DVDLocation __far & location);

DLLCALL Error WINAPI  DDP_GetCurrentDuration(DVDDiskPlayer player, DVDLocation __far & location);

DLLCALL Error WINAPI  DDP_GetTitleDuration(DVDDiskPlayer player, WORD title, DVDTime & duration);

DLLCALL DVDPlayerMode WINAPI DDP_GetPlayerMode(DVDDiskPlayer player);

DLLCALL Error WINAPI	 DDP_GetExtendedPlayerState(DVDDiskPlayer player, ExtendedPlayerState & eps);

DLLCALL DWORD WINAPI  DDP_GetForbiddenUserOperations(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetNumberOfTitles(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetNumberOfPartOfTitle(DVDDiskPlayer player, WORD title);

DLLCALL Error WINAPI  DDP_GetAvailStreams(DVDDiskPlayer player, BYTE __far & audioMask, DWORD __far & subPictureMask);

DLLCALL Error WINAPI  DDP_GetAudioStreamAttributes(DVDDiskPlayer player, WORD stream, DVDAudioStreamFormat __far & format);

DLLCALL Error WINAPI  DDP_GetSubPictureStreamAttributes(DVDDiskPlayer player, WORD stream, DVDSubPictureStreamFormat __far & format);

DLLCALL WORD  WINAPI  DDP_GetCurrentAudioStream(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetCurrentSubPictureStream(DVDDiskPlayer player);

DLLCALL BOOL  WINAPI  DDP_IsCurrentSubPictureEnabled(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetNumberOfAngles(DVDDiskPlayer player, WORD title);

DLLCALL WORD  WINAPI  DDP_GetCurrentAngle(DVDDiskPlayer player);

DLLCALL BOOL  WINAPI  DDP_CheckMenuAvail(DVDDiskPlayer player, VTSMenuType menu);

DLLCALL DisplayPresentationMode WINAPI  DDP_GetCurrentDisplayMode(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_SetDisplayMode(DVDDiskPlayer player, DisplayPresentationMode mode);

DLLCALL Error WINAPI  DDP_TitlePlay(DVDDiskPlayer player, WORD title);

DLLCALL Error WINAPI  DDP_TitlePlayForced(DVDDiskPlayer player, WORD title);

DLLCALL Error WINAPI  DDP_PTTPlay(DVDDiskPlayer player, WORD title, WORD part);

DLLCALL Error WINAPI  DDP_PTTPlayForced(DVDDiskPlayer player, WORD title, WORD part);

DLLCALL Error WINAPI  DDP_TimePlay(DVDDiskPlayer player, WORD title, DVDTime time);

DLLCALL Error WINAPI  DDP_TimePlayForced(DVDDiskPlayer player, WORD title, DVDTime time);

DLLCALL Error WINAPI  DDP_ExtendedPlay(DVDDiskPlayer player, DWORD flags, WORD title, WORD ptt, DVDTime time);

DLLCALL Error WINAPI  DDP_Stop(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_GoUp(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_TimeSearch(DVDDiskPlayer player, DVDTime time);

DLLCALL Error WINAPI  DDP_TimeSearchForced(DVDDiskPlayer player, DVDTime time);

DLLCALL Error WINAPI  DDP_PTTSearch(DVDDiskPlayer player, WORD part);

DLLCALL Error WINAPI  DDP_PTTSearchForced(DVDDiskPlayer player, WORD part);

DLLCALL Error WINAPI  DDP_PrevPGSearch(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_TopPGSearch(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_NextPGSearch(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ForwardScan(DVDDiskPlayer player, WORD speed);

DLLCALL Error WINAPI  DDP_BackwardScan(DVDDiskPlayer player, WORD speed);

DLLCALL Error WINAPI  DDP_TrickPlay(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_StopScan(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ReversePlayback(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_MenuCall(DVDDiskPlayer player, VTSMenuType menu);

DLLCALL Error WINAPI  DDP_Resume(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_UpperButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_LowerButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_LeftButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_RightButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ButtonSelectAt(DVDDiskPlayer player, WORD x, WORD y);

DLLCALL Error WINAPI  DDP_ButtonActivate(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ButtonSelectAndActivate(DVDDiskPlayer player, WORD num);

DLLCALL Error WINAPI  DDP_ButtonSelectAndActivateAt(DVDDiskPlayer player, WORD x, WORD y);

DLLCALL BOOL  WINAPI  DDP_IsButtonAt(DVDDiskPlayer player, WORD x, WORD y);

DLLCALL Error WINAPI  DDP_StillOff(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_PauseOn(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_PauseOff(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_PbcOn(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_PbcOff(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_AdvanceFrame(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_AdvanceFrameBy(DVDDiskPlayer player, int by);

DLLCALL Error WINAPI  DDP_SetPlaybackSpeed(DVDDiskPlayer player, WORD speed);

DLLCALL Error WINAPI  DDP_MenuLanguageSelect(DVDDiskPlayer player, WORD language);

DLLCALL Error WINAPI  DDP_AudioStreamChange(DVDDiskPlayer player, WORD streamID);

DLLCALL Error WINAPI  DDP_SubPictureStreamChange(DVDDiskPlayer player, WORD streamID, BOOL display);

DLLCALL Error WINAPI  DDP_AngleChange(DVDDiskPlayer player, WORD angle);

DLLCALL Error WINAPI  DDP_ParentalLevelSelect(DVDDiskPlayer player, WORD parentalLevel);

DLLCALL Error WINAPI  DDP_ParentalCountrySelect(DVDDiskPlayer player, WORD country);

DLLCALL Error WINAPI  DDP_InitialLanguageSelect(DVDDiskPlayer player,
														      WORD audioLanguage,
														      WORD audioExtension,
														      WORD subPictureLanguage,
														      WORD subPictureExtension);

DLLCALL Error WINAPI  DDP_InquireCurrentBitRate(DVDDiskPlayer player, DWORD & bitsPerSecond);

DLLCALL Error WINAPI  DDP_GetCurrentButtonState(DVDDiskPlayer player, WORD & minButton, WORD & numButtons, WORD & currentButton);

DLLCALL Error WINAPI  DDP_Freeze(DVDDiskPlayer player, BYTE * buffer, DWORD & length);

DLLCALL Error WINAPI  DDP_Defrost(DVDDiskPlayer player, BYTE * buffer, DWORD & length);

DLLCALL Error WINAPI  DDP_DefrostExt(DVDDiskPlayer player, BYTE * buffer, DWORD & length, DWORD flags);

DLLCALL Error WINAPI  DDP_GetFrozenInfo(BYTE * buffer, DWORD length, DDPFreezeState & state);

DLLCALL Error WINAPI  DDP_InstallEventHandler(DVDDiskPlayer player, DWORD event, DNEEventHandler handler, void * userData);

DLLCALL Error WINAPI  DDP_RemoveEventHandler(DVDDiskPlayer player, DWORD event);

DLLCALL Error WINAPI  DDP_SetBreakpoint(DVDDiskPlayer player, WORD title, DVDTime time, DWORD flags, DWORD & id);

DLLCALL Error WINAPI  DDP_SetBreakpointExt(DVDDiskPlayer player, WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id);

DLLCALL Error WINAPI  DDP_ClearBreakpoint(DVDDiskPlayer player, DWORD id);

DLLCALL Error WINAPI  DDP_DetachDrive(DVDDiskPlayer player, DWORD flags, DVDTime time, DWORD bufferSize);

DLLCALL Error WINAPI  DDP_GetAVFileInfo(DVDDiskPlayer player, HIter * it, AVFInfo * pInfo);

DLLCALL Error WINAPI  DDP_PlayFile(DVDDiskPlayer player, HIter * it, DWORD flags);

DLLCALL Error WINAPI  DDP_SetAudioStreamSelectionPreferences(DVDDiskPlayer player, DWORD flags);

}	// extern "C"

#endif
