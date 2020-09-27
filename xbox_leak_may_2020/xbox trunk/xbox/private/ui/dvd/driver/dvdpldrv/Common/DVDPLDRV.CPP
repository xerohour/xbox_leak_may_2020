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
//  DDP Layer Implementation
//
////////////////////////////////////////////////////////////////////

#include "library/Common/WinPorts.h"
#include "Dvdpld32.h"
#include "Library/Files/GenericFile.h"
#include "CDDiskPlayer.h"
#include "Region.h"
#include "Library/Common/VDDebug.h"
#include "Library/Common/Profiles.h"
#include "Library/lowlevel/timer.h"
#include "Config/DiskPlayerFactory.h"
#include "Config/FileSystemFactory.h"

////////////////////////////////////////////////////////////////////
//
//  General stuff
//
////////////////////////////////////////////////////////////////////

#define MKLANG(c, d)	(((WORD)c << 8) | (WORD)d)

//
//  Logging of DDP calls
//

#define LOGDDP_NONE			0x0000		// No DDP logging at all
#define LOGDDP_STATUS		0x0001		// Log status inquiries
#define LOGDDP_PLAYBACK		0x0002		// Log playback commands
#define LOGDDP_CONFIG		0x0004		// Log configuration commands
#define LOGDDP_MISC			0x0008		// Log misc calls
#define LOGDDP_EXIT			0x0010		// Log termination of DDP call

#define LOGDDP_ALL				(LOGDDP_STATUS   | LOGDDP_PLAYBACK | LOGDDP_CONFIG	| LOGDDP_MISC | LOGDDP_EXIT)
#define LOGDDP_ALL_BUT_EXIT	(LOGDDP_STATUS   | LOGDDP_PLAYBACK | LOGDDP_CONFIG	| LOGDDP_MISC)
#define LOGDDP_NO_STATUS		(LOGDDP_PLAYBACK | LOGDDP_CONFIG   | LOGDDP_MISC    | LOGDDP_EXIT)

#define LOGDDPCALLS LOGDDP_NONE // LOGDDP_PLAYBACK | LOGDDP_CONFIG | LOGDDP_MISC | LOGDDP_EXIT 		// Use the LOGDDP_* flags above to configure logging

////////////////////////////////////////////////////////////////////
//
//  ST20LITE specific
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Windows specific
//
////////////////////////////////////////////////////////////////////


#define DLLP1 DLLCALL
#define DLLP2 WINAPI

#if LOGDDPCALLS
#define LOGDDP(text, type) if (LOGDDP_##type & (LOGDDPCALLS)) DP("DDP_%s", #text)
#define LOGDDPEXIT(text, type) if ((LOGDDP_##type & (LOGDDPCALLS)) && (LOGDDP_EXIT & (LOGDDPCALLS))) DP("DDP_%s END (%08lx)\n", #text, err)
#else
#define LOGDDP(text, type)
#define LOGDDPEXIT(text, type)
#endif


////////////////////////////////////////////////////////////////////
//
//  Message handling
//
////////////////////////////////////////////////////////////////////

//
//  Player message codes
//

#define	DDPMSG_OPENDVDVIDEOPLAYER					1
#define	DDPMSG_OPENDVDAUDIOPLAYER					78
#define	DDPMSG_OPENVCDPLAYER							54
#define	DDPMSG_OPENCDDAPLAYER						62
#define	DDPMSG_OPENAVFPLAYER							75
#define	DDPMSG_CLOSEPLAYER							2
#define	DDPMSG_STARTPRESENTATION					3
#define	DDPMSG_GETCURRENTLOCATION					4
#define	DDPMSG_GETCURRENTDURATION					5
#define	DDPMSG_GETPLAYERMODE							6
#define	DDPMSG_GETFORBIDDENUSEROPERATIONS		7
#define	DDPMSG_GETNUMBEROFTITLES					8
#define	DDPMSG_GETNUMBEROFPARTOFTITLES			9
#define	DDPMSG_GETAVAILSTREAMS						10
#define	DDPMSG_GETCURRENTAUDIOSTREAM				11
#define	DDPMSG_GETCURRENTSUBPICTURESTREAM		12
#define	DDPMSG_ISCURRENTSUBPICTURENENABLED		56
#define	DDPMSG_GETNUMBEROFANGLES					13
#define	DDPMSG_GETCURRENTANGLE						14
#define	DDPMSG_CHECKMENUAVAIL						15
#define	DDPMSG_TITLEPLAY								16
#define	DDPMSG_PTTPLAY									17
#define	DDPMSG_TIMEPLAY								18
#define  DDPMSG_EXTENDEDPLAY							70
#define	DDPMSG_STOP										19
#define	DDPMSG_GOUP										20
#define	DDPMSG_TIMESEARCH								21
#define	DDPMSG_PTTSEARCH								22
#define	DDPMSG_PREVPGSEARCH							23
#define	DDPMSG_TOPPGSEARCH							24
#define	DDPMSG_NEXTPGSEARCH							25
#define	DDPMSG_FORWARDSCAN							26
#define	DDPMSG_BACKWARDSCAN							27
#define	DDPMSG_STOPSCAN								28
#define	DDPMSG_REVERSEPLAY							59
#define	DDPMSG_MENUCALL								29
#define	DDPMSG_RESUME									30
#define	DDPMSG_UPPERBUTTONSELECT					31
#define	DDPMSG_LOWERBUTTONSELECT					32
#define	DDPMSG_LEFTBUTTONSELECT						33
#define	DDPMSG_RIGHTBUTTONSELECT					34
#define	DDPMSG_BUTTONACTIVATE						35
#define	DDPMSG_BUTTONSELECTANDACTIVATE			36
#define	DDPMSG_BUTTONSELECTANDACTIVATEAT			37
#define  DDPMSG_BUTTONSELECTAT						55
#define	DDPMSG_ISBUTTONAT								57
#define	DDPMSG_STILLOFF								38
#define	DDPMSG_PAUSEON									39
#define	DDPMSG_PAUSEOFF								40
#define	DDPMSG_MENULANGUAGESELECT					41
#define	DDPMSG_AUDIOSTREAMCHANGE					42
#define	DDPMSG_SUBPICTURESTREAMCHANGE				43
#define	DDPMSG_ANGLECHANGE							44
#define	DDPMSG_PARENTALLEVELSELECT					45
#define	DDPMSG_PARENTALCOUNTRYSELECT				46
#define	DDPMSG_GETAUDIOSTREAMATTRIBUTES			47
#define	DDPMSG_GETSUBPICTURESTREAMATTRIBUTES	48
#define	DDPMSG_GETCURRENTDISPLAYMODE				49
#define	DDPMSG_SETDISPLAYMODE						50
#define	DDPMSG_SELECTINITIALLANGUAGE				51
#define	DDPMSG_ADVANCEFRAME							52
#define	DDPMSG_SETPLAYBACKSPEED						53
#define  DDPMSG_GETCURRENTBUTTONSTATE				58
#define	DDPMSG_FREEZE									60
#define	DDPMSG_DEFROST									61
#define	DDPMSG_TRICKPLAY								63
#define	DDPMSG_ADVANCEFRAMEBY						64
#define	DDPMSG_GETEXTENDEDPLAYERSTATE				65
#define	DDPMSG_EVENTHANDLERON						66
#define	DDPMSG_EVENTHANDLEROFF						67
#define	DDPMSG_SETBREAKPOINT							68
#define	DDPMSG_CLEARBREAKPOINT						69
#define  DDPMSG_DETACHDRIVE							71		// OBSOLETE
#define  DDPMSG_GETTITLEDURATION						72
#define	DDPMSG_TIMEPLAYFORCED						73
#define	DDPMSG_TIMESEARCHFORCED						74
#define	DDPMSG_GETAVFILEINFO							76
#define	DDPMSG_PLAYFILE								77
#define	DDPMSG_PROCESS_ASYNC_RQ						79
#define	DDPMSG_SETAUDIOSTREAMSELECTIONPREFS		80
#define	DDPMSG_PTTPLAYFORCED							81
#define	DDPMSG_TITLEPLAYFORCED						82
#define	DDPMSG_PTTSEARCHFORCED						83
#define	DDPMSG_PBCON									84
#define	DDPMSG_PBCOFF									85

// please note DDPMSG 85 in use

//
//  Player message structure
//

struct DVDPlayerMsg
	{
	UnitSet								units;
	DVDDiskPlayer						player;
	Error									error;

	DVDDiskType							diskType;
	DVDTime								time;
	DVDLocation						*	location;
	DVDAudioStreamFormat			*	audioStreamFormat;
	DVDSubPictureStreamFormat	*	subPictureStreamFormat;
	DVDPlayerMode						mode;
	DisplayPresentationMode			dpmode;
	BOOL									boolean;

	char									ch;
	char								*	str;

	BYTE									byte;
	WORD									word, title, part, x, y, word2, word3;
	DWORD									dword, dword2;
	int									val;

	WORD									audioLanguage, audioExtension, subPictureLanguage, subPictureExtension;

	VTSMenuType							menu;

	void								*	pointer, * pointer2;
	};

////////////////////////////////////////////////////////////////////
//
//  Global variables
//
////////////////////////////////////////////////////////////////////


//
//  Some inevitable global variables
//

WinPortServer				* DriverInstance = NULL;
RegionHandler				* RegionMan = NULL;						// This one caches region information
DriveTable					* SystemDriveTable = NULL;
RootFileSystem				* FileSystem = NULL;
GenericProfile				* SystemProfile = NULL;
DVDDiskPlayerFactory		* dvdVideoDiskPlayerFactory = NULL;
DVDDiskPlayerFactory		* dvdAudioDiskPlayerFactory = NULL;
VCDDiskPlayerFactory		* vcdDiskPlayerFactory = NULL;
CDDADiskPlayerFactory	* cddaDiskPlayerFactory = NULL;
AVFDiskPlayerFactory		* avfDiskPlayerFactory = NULL;
FileSystemFactory			* dvdFileSystemFactory = NULL;
FileSystemFactory			* vcdFileSystemFactory = NULL;
FileSystemFactory			* cddaFileSystemFactory = NULL;
FileSystemFactory			* avfFileSystemFactory = NULL;
FileSystemFactory			* osFileSystemFactory = NULL;

////////////////////////////////////////////////////////////////////
//
//  DDP Scheduler Class
//
////////////////////////////////////////////////////////////////////


class DDPScheduler : public EventPort
	{
	public:
		DDPScheduler(WinPortServer *server) : EventPort(server) {}

		void Message(WPARAM wParam, LPARAM dParam);

	} * ddpScheduler;

DWORD WINAPI DDPScheduleRoutine(LPVOID param)
	{
	ddpScheduler = new DDPScheduler(DriverInstance);

	DriverInstance->ProcessMessages();

	delete ddpScheduler;

	return 0;
	}

HANDLE DDPScheduleThread;



///////////////////////////////
//
// GetDiskInfoAsyncRequestScheduler class and related structures
//
///////////////////////////////

typedef struct tagAsyncRqDescriptior
	{
	ARQReqestID asyncRqID; //ID of request being scheduled/processed
	void* rqData;
	KernelString drivePath;
	ARQAsyncRequestDoneCB asyncRequestDoneCallback;
	DWORD rqHandle;
	} AsyncRqDescriptior;


class MissionControl
	{
	public:
		virtual BOOL IsAbortRequested(void) = 0;
	};

class GetDiskInfoAsyncRequestScheduler : public MissionControl
	{
	protected:
		const DWORD constRqHandle; //Only one request at a time allowed for now
		AsyncRqDescriptior rqPending; //This should be a list once multiple requests at a time are allowed

		DVDPlayerMsg msg;

		BOOL requestInProgress; //busy flag
		BOOL abortRequest; //true while aborting current request
		VDLocalMutex flagMutex; //Mutex for accesssing flags
		VDSemaphore asyncRqDone;

		BOOL IsRequestInProgress(){VDAutoMutex mutex(&flagMutex); return requestInProgress;}
		void SetRequestInProgressFlag(BOOL flagValue){VDAutoMutex mutex(&flagMutex); requestInProgress = flagValue;}

		void SetAbortRequestFlag(BOOL flagValue){VDAutoMutex mutex(&flagMutex); abortRequest = flagValue;}

		//handle should be unique ID of the request processed, but at the moment there is
		//not much use of the handle until we allow multiple asynchronous requests, so
		//we just return constant value
		DWORD GetNewHandle(void){return constRqHandle;}

	public:
		GetDiskInfoAsyncRequestScheduler()
			: constRqHandle(1)
			{
			requestInProgress = FALSE;
			abortRequest = FALSE;
			asyncRqDone.Reset();
			rqPending.asyncRqID = ARQ_NONE;
			}

		~GetDiskInfoAsyncRequestScheduler()
			{
			}

		Error ScheduleRequest(ARQReqestID asyncRqID, KernelString& drivePath, ARQAsyncRequestDoneCB ARQCallback, void* diskOnlyInfo, DWORD & rqHandle);
		Error ProcessAsyncRequest(DWORD rqHandle);
		Error AbortAndDisableDiskRequests(DWORD rqHandle);

		BOOL IsAbortRequested(void){VDAutoMutex mutex(&flagMutex); return abortRequest;}

	} * getDiskInfoAsyncRqScheduler = NULL;


//
//  Process message by application
//

void DDPScheduler::Message(WPARAM wParam, LPARAM dParam)
	{
	DVDPlayerMsg	*	msg = (DVDPlayerMsg *)dParam;
	DVDDiskPlayer		player = msg->player;
	Error					err = GNR_UOP_PROHIBITED;
	BOOL					permitted;

//	int ctime = ::GetTickCount();
//	char buffer[500];
//	wsprintf(buffer, "##### Message+, %d THREAD %08lx\n", wParam, GetCurrentThread());
//	OutputDebugString(buffer);

	switch(wParam)
		{
		case DDPMSG_PROCESS_ASYNC_RQ:
			LOGDDP(PROCESSASYNCRQ, STATUS);
			err = getDiskInfoAsyncRqScheduler->ProcessAsyncRequest(msg->dword);
			LOGDDPEXIT(PROCESSASYNCRQ, STATUS);
			break;
		case DDPMSG_OPENDVDVIDEOPLAYER:
			{
			BYTE systemRegion, diskRegion, availSets;
			RegionSource regionSource;

			LOGDDP(OPENDVDVIDEOPLAYER, PLAYBACK);

			if (!IS_ERROR(err = RegionMan->GetRegionSettings(msg->units, (RootIterator*)(msg->pointer2),
																			   systemRegion, diskRegion, regionSource, availSets)))
				err = dvdVideoDiskPlayerFactory->CreateDiskPlayer(DriverInstance, msg->units, SystemProfile, msg->diskType,
																					 (DVDFileSystem*)(msg->pointer), systemRegion, msg->player);
			LOGDDPEXIT(OPENDVDVIDEOPLAYER, PLAYBACK);
			break;
			}
		case DDPMSG_OPENDVDAUDIOPLAYER:
			LOGDDP(OPENDVDAUDIOPLAYER, PLAYBACK);
			err = dvdAudioDiskPlayerFactory->CreateDiskPlayer(DriverInstance, msg->units, SystemProfile, msg->diskType,
																				 (DVDFileSystem*)(msg->pointer), 0, msg->player);
			LOGDDPEXIT(OPENDVDAUDIOPLAYER, PLAYBACK);
			break;
		case DDPMSG_OPENVCDPLAYER:
			LOGDDP(OPENVCDPLAYER, PLAYBACK);
			err = vcdDiskPlayerFactory->CreateDiskPlayer(DriverInstance, msg->units, SystemProfile, msg->diskType,
																		  (VCDFileSystem*)(msg->pointer), msg->player);
			LOGDDPEXIT(OPENVCDPLAYER, PLAYBACK);
			break;
		case DDPMSG_OPENCDDAPLAYER:
			LOGDDP(OPENCDDAPLAYER, PLAYBACK);
			err = cddaDiskPlayerFactory->CreateDiskPlayer(DriverInstance, msg->units, SystemProfile, msg->diskType,
																			(CDDAFileSystem*)(msg->pointer), msg->player);
			LOGDDPEXIT(OPENCDDAPLAYER, PLAYBACK);
			break;
		case DDPMSG_OPENAVFPLAYER:
			LOGDDP(OPENAVFPLAYER, PLAYBACK);
			err = avfDiskPlayerFactory->CreateDiskPlayer(DriverInstance, msg->units, SystemProfile, msg->diskType,
																		  (GenericFileSystem*)(msg->pointer), msg->player);
			LOGDDPEXIT(OPENAVFPLAYER, PLAYBACK);
			break;
		case DDPMSG_CLOSEPLAYER:
			LOGDDP(CLOSEPLAYER, PLAYBACK);
			delete msg->player;		// ??? Cleanup()
			err = GNR_OK;
			LOGDDPEXIT(CLOSEPLAYER, PLAYBACK);
			break;
		case DDPMSG_STARTPRESENTATION:
			LOGDDP(STARTPRESENTATION, PLAYBACK);
			err = player->StartPresentation(msg->dword);
			LOGDDPEXIT(STARTPRESENTATION, PLAYBACK);
			break;
		case DDPMSG_GETCURRENTLOCATION:
			LOGDDP(GETCURRENTLOCATION, STATUS);
			err = player->GetCurrentLocation(*(msg->location));
			LOGDDPEXIT(GETCURRENTLOCATION, STATUS);
			break;
		case DDPMSG_GETCURRENTDURATION:
			LOGDDP(GETCURRENTDURATION, STATUS);
			err = player->GetCurrentDuration(*(msg->location));
			LOGDDPEXIT(GETCURRENTDURATION, STATUS);
			break;
		case DDPMSG_GETTITLEDURATION:
			LOGDDP(GETTITLEDURATION, STATUS);
			err = player->GetTitleDuration(msg->word, msg->time);
			LOGDDPEXIT(GETTITLEDURATION, STATUS);
			break;
		case DDPMSG_GETPLAYERMODE:
			LOGDDP(GETPLAYERMODE, STATUS);
			err  = player->GetMode(msg->mode);
			LOGDDPEXIT(GETPLAYERMODE, STATUS);
			break;
		case DDPMSG_GETFORBIDDENUSEROPERATIONS:
			LOGDDP(GETFORBIDDENUSEROPERATIONS, STATUS);
			err = player->GetUOPs(msg->dword);
			LOGDDPEXIT(GETFORBIDDENUSEROPERATIONS, STATUS);
			break;
		case DDPMSG_GETNUMBEROFTITLES:
			LOGDDP(GETNUMBEROFTITLES, STATUS);
			err = player->NumberOfTitles(msg->word);
			LOGDDPEXIT(GETNUMBEROFTITLES, STATUS);
			break;
		case DDPMSG_GETNUMBEROFPARTOFTITLES:
			LOGDDP(GETNUMBEROFPARTOFTITLES, STATUS);
		   err = player->NumberOfPartOfTitle(msg->title, msg->word);
			LOGDDPEXIT(GETNUMBEROFPARTOFTITLES, STATUS);
			break;
		case DDPMSG_GETAVAILSTREAMS:
			LOGDDP(GETAVAILSTREAMS, STATUS);
			err = player->GetAvailStreams(msg->byte, msg->dword);
			LOGDDPEXIT(GETAVAILSTREAMS, STATUS);
			break;
		case DDPMSG_GETCURRENTAUDIOSTREAM:
			LOGDDP(GETCURRENTAUDIOSTREAM, STATUS);
			err = player->GetCurrentAudioStream(msg->word);
			LOGDDPEXIT(GETCURRENTAUDIOSTREAM, STATUS);
			break;
		case DDPMSG_GETCURRENTSUBPICTURESTREAM:
			LOGDDP(GETCURRENTSUBPICTURESTREAM, STATUS);
			err = player->GetCurrentSubPictureStream(msg->word);
			LOGDDPEXIT(GETCURRENTSUBPICTURESTREAM, STATUS);
			break;
		case DDPMSG_ISCURRENTSUBPICTURENENABLED:
			LOGDDP(ISCURRENTSUBPICTUREENABLED, STATUS);
			err = player->IsCurrentSubPictureEnabled(msg->boolean);
			LOGDDPEXIT(ISCURRENTSUBPICTUREENABLED, STATUS);
			break;
		case DDPMSG_GETAUDIOSTREAMATTRIBUTES:
			LOGDDP(GETAUDIOSTREAMATTRIBUTES, STATUS);
			err = player->GetAudioStreamAttributes(msg->word, *(msg->audioStreamFormat));
			LOGDDPEXIT(GETAUDIOSTREAMATTRIBUTES, STATUS);
			break;
		case DDPMSG_GETSUBPICTURESTREAMATTRIBUTES:
			LOGDDP(GETSUBPICTURESTREAMATTRIBUTES, STATUS);
			err = player->GetSubPictureStreamAttributes(msg->word, *(msg->subPictureStreamFormat));
			LOGDDPEXIT(GETSUBPICTURESTREAMATTRIBUTES, STATUS);
			break;
		case DDPMSG_GETNUMBEROFANGLES:
			LOGDDP(GETNUMBEROFANGLES, STATUS);
		   err = player->GetNumberOfAngles(msg->title, msg->word);
			LOGDDPEXIT(GETNUMBEROFANGLES, STATUS);
			break;
		case DDPMSG_GETCURRENTANGLE:
			LOGDDP(GETCURRENTANGLE, STATUS);
			err = player->GetCurrentAngle(msg->word);
			LOGDDPEXIT(GETCURRENTANGLE, STATUS);
			break;
		case DDPMSG_CHECKMENUAVAIL:
			LOGDDP(CHECKMENUAVAIL, STATUS);
			err = player->MenuAvail(msg->menu, msg->boolean);
			LOGDDPEXIT(CHECKMENUAVAIL, STATUS);
			break;
		case DDPMSG_TITLEPLAY:
			LOGDDP(TITLEPLAY, PLAYBACK);
			err = player->UOPPermitted(2, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->TitlePlay(msg->title);
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(TITLEPLAY, PLAYBACK);
			break;
		case DDPMSG_TITLEPLAYFORCED:
			LOGDDP(TITLEPLAYFORCED, PLAYBACK);
			err = player->TitlePlay(msg->title);
			LOGDDPEXIT(TITLEPLAYFORCED, PLAYBACK);
			break;
		case DDPMSG_PTTPLAY:
			LOGDDP(PTTPLAY, PLAYBACK);
			err = player->UOPPermitted(1, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					{
					err = player->TitleSearchUOPPermitted(msg->title, 1, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->PTTPlay(msg->title, msg->part);
						else
							err = GNR_UOP_PROHIBITED;
						}
					}
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(PTTPLAY, PLAYBACK);
			break;
		case DDPMSG_PTTPLAYFORCED:
			LOGDDP(PTTPLAYFORCED, PLAYBACK);
			err = player->PTTPlay(msg->title, msg->part);
			LOGDDPEXIT(PTTPLAYFORCED, PLAYBACK);
			break;
		case DDPMSG_TIMEPLAY:
			LOGDDP(TIMEPLAY, PLAYBACK);
			err = player->UOPPermitted(0, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					{
					err = player->TitleSearchUOPPermitted(msg->title, 0, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->TimePlay(msg->title, msg->time);
						else
							err = GNR_UOP_PROHIBITED;
						}
					}
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(TIMEPLAY, PLAYBACK);
			break;
		case DDPMSG_TIMEPLAYFORCED:
			LOGDDP(TIMEPLAYFORCED, PLAYBACK);
			err = player->TimePlay(msg->title, msg->time);
			LOGDDPEXIT(TIMEPLAYFORCED, PLAYBACK);
			break;
		case DDPMSG_EXTENDEDPLAY:
			LOGDDP(EXTENDEDPLAY, PLAYBACK);
			err = player->ExtendedPlay(msg->dword, msg->title, msg->part, msg->time);
			LOGDDPEXIT(EXTENDEDPLAY, PLAYBACK);
			break;
		case DDPMSG_STOP:
			LOGDDP(STOP, PLAYBACK);
			err = player->Exit();
			LOGDDPEXIT(STOP, PLAYBACK);
			break;
		case DDPMSG_GOUP:
			LOGDDP(GOUP, PLAYBACK);
			err = player->UOPPermitted(0, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->GoUpProgramChain();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(GOUP, PLAYBACK);
			break;
		case DDPMSG_TIMESEARCH:
			LOGDDP(TIMESEARCH, PLAYBACK);
			err = player->UOPPermitted(0, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					{
					err = player->UOPPermitted(5, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							{
							err = player->TitleSearchUOPPermitted(0, 0, permitted);
							if (!IS_ERROR(err))
								{
								if (permitted)
									err = player->TimeSearch(msg->time);
								else
									err = GNR_UOP_PROHIBITED;
								}
							}
						else
							err = GNR_UOP_PROHIBITED;
						}
					}
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(TIMESEARCH, PLAYBACK);
			break;
		case DDPMSG_TIMESEARCHFORCED:
			LOGDDP(TIMESEARCHFORCED, PLAYBACK);
			err = player->TimeSearch(msg->time);
			LOGDDPEXIT(TIMESEARCHFORCED, PLAYBACK);
			break;
		case DDPMSG_PTTSEARCH:
			LOGDDP(PTTSEARCH, PLAYBACK);
			err = player->UOPPermitted(1, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					{
					err = player->UOPPermitted(5, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							{
							err = player->TitleSearchUOPPermitted(0, 1, permitted);
							if (!IS_ERROR(err))
								{
								if (permitted)
									err = player->PTTSearch(msg->part);
								else
									err = GNR_UOP_PROHIBITED;
								}
							}
						else
							err = GNR_UOP_PROHIBITED;
						}
					}
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(PTTSEARCH, PLAYBACK);
			break;
		case DDPMSG_PTTSEARCHFORCED:
			LOGDDP(PTTSEARCHFORCED, PLAYBACK);
			err = player->PTTSearch(msg->part);
			LOGDDPEXIT(PTTSEARCHFORCED, PLAYBACK);
			break;
		case DDPMSG_PREVPGSEARCH:
			LOGDDP(PREVPGSEARCH, PLAYBACK);
			err = player->UOPPermitted(6, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->GoPrevProgram();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(PREVPGSEARCH, PLAYBACK);
			break;
		case DDPMSG_TOPPGSEARCH:
			LOGDDP(TOPPGSEARCH, PLAYBACK);
			err = player->UOPPermitted(6, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->GoTopProgram();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(TOPPGSEARCH, PLAYBACK);
			break;
		case DDPMSG_NEXTPGSEARCH:
			LOGDDP(NEXTPGSEARCH, PLAYBACK);
			err = player->UOPPermitted(7, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->GoNextProgram();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(NEXTPGSEARCH, PLAYBACK);
			break;
		case DDPMSG_FORWARDSCAN:
			LOGDDP(FORWARDSCAN, PLAYBACK);
			err = player->UOPPermitted(8, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->StartForwardScan(msg->word);
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(FORWARDSCAN, PLAYBACK);
			break;
		case DDPMSG_BACKWARDSCAN:
			LOGDDP(BACKWARDSCAN, PLAYBACK);
			err = player->UOPPermitted(9, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->StartBackwardScan(msg->word);
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(BACKWARDSCAN, PLAYBACK);
			break;
		case DDPMSG_TRICKPLAY:
			LOGDDP(TRICKPLAY, PLAYBACK);
			err = player->UOPPermitted(9, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					{
					err = player->UOPPermitted(8, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->StartTrickplay();
						else
							err = GNR_UOP_PROHIBITED;
						}
					}
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(TRICKPLAY, PLAYBACK);
			break;
		case DDPMSG_STOPSCAN:
			LOGDDP(STOPSCAN, PLAYBACK);
			err = player->UOPPermitted(25, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->StopScan();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(STOPSCAN, PLAYBACK);
			break;
		case DDPMSG_REVERSEPLAY:
			LOGDDP(REVERSEPLAY, PLAYBACK);
			err = player->UOPPermitted(9, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->StartReversePlayback();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(REVERSEPLAY, PLAYBACK);
			break;
		case DDPMSG_SETPLAYBACKSPEED:
			LOGDDP(SETPLAYBACKSPEED, CONFIG);
			err = player->UOPPermitted(8, permitted);
			if (!IS_ERROR(err))
				{
				if (msg->word <= 1000 || permitted)
					err = player->SetPlaybackSpeed(msg->word);
				else
					err = GNR_UOP_PROHIBITED;
				}
			break;
		case DDPMSG_MENUCALL:
			LOGDDP(MENUCALL, PLAYBACK);
			switch(msg->menu)
				{
				case VMT_TITLE_MENU:
					err = player->UOPPermitted(10, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->CallMenu(msg->menu);
						else
							err = GNR_UOP_PROHIBITED;
						}
					break;
				case VMT_ROOT_MENU:
					err = player->UOPPermitted(11, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->CallMenu(msg->menu);
						else
							err = GNR_UOP_PROHIBITED;
						}
					break;
				case VMT_SUBPICTURE_MENU:
					err = player->UOPPermitted(12, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->CallMenu(msg->menu);
						else
							err = GNR_UOP_PROHIBITED;
						}
					break;
				case VMT_AUDIO_MENU:
					err = player->UOPPermitted(13, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->CallMenu(msg->menu);
						else
							err = GNR_UOP_PROHIBITED;
						}
					break;
				case VMT_ANGLE_MENU:
					err = player->UOPPermitted(14, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->CallMenu(msg->menu);
						else
							err = GNR_UOP_PROHIBITED;
						}
					break;
				case VMT_PTT_MENU:
					err = player->UOPPermitted(15, permitted);
					if (!IS_ERROR(err))
						{
						if (permitted)
							err = player->CallMenu(msg->menu);
						else
							err = GNR_UOP_PROHIBITED;
						}
					break;
				}
			LOGDDPEXIT(MENUCALL, PLAYBACK);
			break;
		case DDPMSG_RESUME:
			LOGDDP(RESUME, PLAYBACK);
			err = player->UOPPermitted(16, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ResumeFromSystemSpace();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(RESUME, PLAYBACK);
			break;
		case DDPMSG_UPPERBUTTONSELECT:
			LOGDDP(UPPERBUTTONSELECT, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonUp();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(UPPERBUTTONSELECT, PLAYBACK);
			break;
		case DDPMSG_LOWERBUTTONSELECT:
			LOGDDP(LOWERBUTTONSELECT, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonDown();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(LOWERBUTTONSELECT, PLAYBACK);
			break;
		case DDPMSG_LEFTBUTTONSELECT:
			LOGDDP(LEFTBUTTONSELECT, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonLeft();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(LEFTBUTTONSELECT, PLAYBACK);
			break;
		case DDPMSG_RIGHTBUTTONSELECT:
			LOGDDP(RIGHTBUTTONSELECT, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonRight();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(RIGHTBUTTONSELECT, PLAYBACK);
			break;
		case DDPMSG_BUTTONACTIVATE:
			LOGDDP(BUTTONACTIVATE, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonActivate();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(BUTTONACTIVATE, PLAYBACK);
			break;
		case DDPMSG_BUTTONSELECTANDACTIVATE:
			LOGDDP(BUTTONSELECTANDACTIVATE, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonSelectAndActivate(msg->word);
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(BUTTONSELECTANDACTIVATE, PLAYBACK);
			break;
		case DDPMSG_BUTTONSELECTANDACTIVATEAT:
			LOGDDP(BUTTONSELECTANDACTIVATEAT, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonSelectAtAndActivate(msg->x, msg->y);
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(BUTTONSELECTANDACTIVATEAT, PLAYBACK);
			break;
		case DDPMSG_BUTTONSELECTAT:
			LOGDDP(BUTTONSELECTAT, PLAYBACK);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->ButtonSelectAt(msg->x, msg->y);
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(BUTTONSELECTAT, PLAYBACK);
			break;
		case DDPMSG_ISBUTTONAT:
			LOGDDP(ISBUTTONAT, STATUS);
			err = player->UOPPermitted(17, permitted);
			if (!IS_ERROR(err) && permitted)
				err = player->IsButtonAt(msg->x, msg->y, msg->boolean);
			else
				msg->boolean = FALSE;
			LOGDDPEXIT(ISBUTTONAT, STATUS);
			break;
		case DDPMSG_STILLOFF:
			LOGDDP(STILLOFF, PLAYBACK);
			err = player->UOPPermitted(18, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->StopStillPhase();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(STILLOFF, PLAYBACK);
			break;
		case DDPMSG_PAUSEON:
			LOGDDP(PAUSEON, PLAYBACK);
			err = player->UOPPermitted(19, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->PausePlayback();
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(PAUSEON, PLAYBACK);
			break;
		case DDPMSG_PAUSEOFF:
			LOGDDP(PAUSEOFF, PLAYBACK);
			err = player->ResumePlayback();
			LOGDDPEXIT(PAUSEOFF, PLAYBACK);
			break;
		case DDPMSG_PBCON:
			LOGDDP(PBCON, PLAYBACK);
			err = player->SetPlaybackControl(TRUE);
			LOGDDPEXIT(PBCON, PLAYBACK);
			break;
		case DDPMSG_PBCOFF:
			LOGDDP(PBCOFF, PLAYBACK);
			err = player->SetPlaybackControl(FALSE);
			LOGDDPEXIT(PBCOFF, PLAYBACK);
			break;
		case DDPMSG_ADVANCEFRAME:
			LOGDDP(ADVANCEFRAME, PLAYBACK);
			err = player->AdvanceFrame();
			LOGDDPEXIT(ADVANCEFRAME, PLAYBACK);
			break;
		case DDPMSG_ADVANCEFRAMEBY:
			LOGDDP(ADVANCEFRAMEBY, PLAYBACK);
			err = player->AdvanceFrameBy(msg->val);
			LOGDDPEXIT(ADVANCEFRAMEBY, PLAYBACK);
			break;
		case DDPMSG_MENULANGUAGESELECT:
			LOGDDP(MENULANGUAGESELECT, CONFIG);
			err = player->MenuLanguageSelect(msg->word);
			LOGDDPEXIT(MENULANGUAGESELECT, CONFIG);
			break;
		case DDPMSG_AUDIOSTREAMCHANGE:
			LOGDDP(AUDIOSTREAMCHANGE, CONFIG);
			err = player->UOPPermitted(20, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					{
					DVDPlayerMode mode;

					if (!IS_ERROR(err = player->GetMode(mode)))
						{
						err = player->AudioStreamChange(msg->word);

						if (mode == DPM_PAUSED)
							player->PausePlayback();
						}
					}
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(AUDIOSTREAMCHANGE, CONFIG);
			break;
		case DDPMSG_SUBPICTURESTREAMCHANGE:
			LOGDDP(SUBPICTURESTREAMCHANGE, CONFIG);
			if (player)
				{
				err = player->UOPPermitted(21, permitted);
				if (!IS_ERROR(err))
					{
					if (permitted)
						{
						DVDPlayerMode mode;

						if (!IS_ERROR(err = player->GetMode(mode)))
							{
							err = player->SubPictureStreamChange(msg->word, msg->boolean);

							if (mode == DPM_PAUSED)
								player->PausePlayback();
							}
						}
					else
						err = GNR_UOP_PROHIBITED;
					}
				}
			else
				{
				err = GNR_OBJECT_NOT_ALLOCATED;
				}

			LOGDDPEXIT(SUBPICTURESTREAMCHANGE, CONFIG);
			break;
		case DDPMSG_ANGLECHANGE:
			LOGDDP(ANGLECHANGE, CONFIG);
			err = player->UOPPermitted(22, permitted);
			if (!IS_ERROR(err))
				{
				if (permitted)
					err = player->AngleChange(msg->word);
				else
					err = GNR_UOP_PROHIBITED;
				}
			LOGDDPEXIT(ANGLECHANGE, CONFIG);
			break;
		case DDPMSG_PARENTALLEVELSELECT:
			LOGDDP(PARENTALLEVELSELECT, CONFIG);
			err = player->ParentalLevelSelect(msg->word);
			LOGDDPEXIT(PARENTALLEVELSELECT, CONFIG);
			break;
		case DDPMSG_PARENTALCOUNTRYSELECT:
			LOGDDP(PARENTALCOUNTRYSELECT, CONFIG);
			err = player->ParentalCountrySelect(msg->word);
			LOGDDPEXIT(PARENTALCOUNTRYSELECT, CONFIG);
			break;
		case DDPMSG_GETCURRENTDISPLAYMODE:
			LOGDDP(GETCURRENTDISPLAYMODE, STATUS);
			err = player->GetCurrentDisplayMode(msg->dpmode);
			LOGDDPEXIT(GETCURRENTDISPLAYMODE, STATUS);
			break;
		case DDPMSG_SETDISPLAYMODE:
			LOGDDP(SETDISPLAYMODE, CONFIG);
			err = player->SetDisplayMode(msg->dpmode);
			LOGDDPEXIT(SETDISPLAYMODE, CONFIG);
			break;
		case DDPMSG_SELECTINITIALLANGUAGE:
			LOGDDP(SELECTINITIALLANGUAGE, CONFIG);
			err = player->SelectInitialLanguage(msg->audioLanguage, msg->audioExtension,
			                                      msg->subPictureLanguage, msg->subPictureExtension);
			LOGDDPEXIT(SELECTINITIALLANGUAGE, CONFIG);
			break;
		case DDPMSG_GETCURRENTBUTTONSTATE:
			LOGDDP(GEETCURRENTBUTTONSTATE, STATUS);
			err = player->GetCurrentButtonState(msg->word, msg->word2, msg->word3);
			LOGDDPEXIT(GEETCURRENTBUTTONSTATE, STATUS);
			break;
		case DDPMSG_FREEZE:
			LOGDDP(FREEZE, MISC);
			err = player->Freeze((DDPFreezeState*)msg->pointer, msg->dword);
			LOGDDPEXIT(FREEZE, MISC);
			break;
		case DDPMSG_DEFROST:
			LOGDDP(DEFROST, MISC);
			err = player->Defrost((DDPFreezeState*)msg->pointer, msg->dword, msg->dword2);
			LOGDDPEXIT(DEFROST, MISC);
			break;
		case DDPMSG_GETEXTENDEDPLAYERSTATE:
			LOGDDP(GETEXTENDEDPLAYERSTATE, STATUS);
			err = player->EPSPermitted(permitted);
			if (!IS_ERROR(err) && permitted)
				err = player->GetExtendedPlayerState((ExtendedPlayerState*)(msg->pointer));
			LOGDDPEXIT(GETEXTENDEDPLAYERSTATE, STATUS);
			break;
		case DDPMSG_EVENTHANDLERON:
			LOGDDP(EVENTHANDLERON, MISC);
			err = player->SetEventHandler(msg->dword, (DNEEventHandler)msg->pointer, msg->pointer2);
			LOGDDPEXIT(EVENTHANDLERON, MISC);
			break;
		case DDPMSG_EVENTHANDLEROFF:
			LOGDDP(EVENTHANDLEROFF, MISC);
			err = player->SetEventHandler(msg->dword, NULL, 0);
			LOGDDPEXIT(EVENTHANDLEROFF, MISC);
			break;
		case DDPMSG_SETBREAKPOINT:
			LOGDDP(SETBREAKPOINT, MISC);
			err = player->SetBreakpoint(msg->word, msg->word2, msg->time, msg->dword, msg->dword2);
			LOGDDPEXIT(SETBREAKPOINT, MISC);
			break;
		case DDPMSG_CLEARBREAKPOINT:
			LOGDDP(CLEARBREAKPOINT, MISC);
			err = player->ClearBreakpoint(msg->dword);
			LOGDDPEXIT(CLEARBREAKPOINT, MISC);
			break;
		case DDPMSG_DETACHDRIVE:
			LOGDDP(DETACHDRIVE, PLAYBACK);
			err = GNR_OPERATION_NOT_SUPPORTED;
			LOGDDPEXIT(DETACHDRIVE, PLAYBACK);
			break;
		case DDPMSG_GETAVFILEINFO:
			LOGDDP(GETAVFILEINFO, MISC);
			err = player->GetAVFileInfo((GenericFileSystemIterator *)(msg->pointer), (AVFInfo *)(msg->pointer2));
			LOGDDPEXIT(GETAVFILEINFO, MISC);
			break;
		case DDPMSG_PLAYFILE:
			LOGDDP(PLAYFILE, PLAYBACK);
			err = player->PlayFile((GenericFileSystemIterator *)(msg->pointer), msg->dword);
			LOGDDPEXIT(PLAYFILE, PLAYBACK);
			break;
		case DDPMSG_SETAUDIOSTREAMSELECTIONPREFS:
			LOGDDP(DDPMSG_SETAUDIOSTREAMSELECTIONPREFS, CONFIG);
			err = player->SetAudioStreamSelectionPreferences(msg->dword);
			LOGDDPEXIT(DDPMSG_SETAUDIOSTREAMSELECTIONPREFS, CONFIG);
			break;

		default:
			err = GNR_OPERATION_NOT_SUPPORTED;
			break;
		}

//	ctime = ::GetTickCount() - ctime;
//	wsprintf(buffer, "##### Message-, %d Err %08lx TIME %4d\n", wParam, err, ctime);
//	OutputDebugString(buffer);

	msg->error = err;
	}

////////////////////////////////////////////////////////////////////
//
//  DDP Function Implementations
//
////////////////////////////////////////////////////////////////////

//
//  Get root iterator pointing to certain drive
//

Error GetDriveIterator(const DiskItemName & driveName, RootIterator * & rit)
	{
	GenericFileSystemIterator * gfsi;
	DiskItemName itemName;
	Error err;
	BOOL found = FALSE;

	GNREASSERT(FileSystem->CreateRootIterator(gfsi));

	//
	//  Iterator over all drives and try to find the proper one
	//

	err = gfsi->GoToFirstItem();
	while (!IS_ERROR(err))
		{
		if (!IS_ERROR(err = gfsi->GetItemName(itemName)))
			{
			if (itemName == driveName)
				{
				found = TRUE;
				break;
				}
			err = gfsi->GoToNextItem();
			}
		}

	//
	//  If there is no drive with that name then we are done
	//

	if (!found)
		{
		delete gfsi;
		GNRAISE(GNR_OBJECT_NOT_FOUND);
		}

	rit = (RootIterator*)gfsi;
	GNRAISE_OK;
	}

//
//  Get Volume Name
//

DLLP1 Error DLLP2 DDP_GetVolumeName(char driveLetter, char * name)
	{
	RootIterator * rit;
	Error err;
	KernelString	str;

	LOGDDP(GETVOLUMENAME, STATUS);

	if (driveLetter <'A' || driveLetter > 'Z')
		GNRAISE(GNR_INVALID_DRIVE_LETTER);

	GNREASSERT(GetDriveIterator(driveLetter, rit));

	err = FileSystem->GetVolumeName(rit, str);
	str.Get(name, 255);
	delete rit;
	LOGDDPEXIT(GETVOLUMENAME, STATUS);
	GNRAISE(err);
	}

//
//  Get Disk Region Settings
//

DLLP1 Error DLLP2 DDP_GetDiskRegionSettings(UnitSet units, char driveLetter, BYTE __far & systemRegion, BYTE __far & availSets, RegionSource __far & regionSource, BYTE & diskRegion)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(GETDISKREGIONSETTINGS, STATUS);

	GNREASSERT(GetDriveIterator(driveName, rit));
	err = RegionMan->GetRegionSettings(units, rit, systemRegion, diskRegion, regionSource, availSets);
	delete rit;

	LOGDDPEXIT(GETDISKREGIONSETTINGS, STATUS);
	GNRAISE(err);
	}

//
//  Get Region Settings
//

DLLP1 Error DLLP2 DDP_GetRegionSettings(UnitSet units, char driveLetter, BYTE __far & systemRegion, BYTE __far & availSets, RegionSource __far & regionSource)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;
	BYTE dummy;

	LOGDDP(GETREGIONSETTINGS, STATUS);

	GNREASSERT(GetDriveIterator(driveName, rit));
	err = RegionMan->GetRegionSettings(units, rit, systemRegion, dummy, regionSource, availSets);
	delete rit;

	LOGDDPEXIT(GETREGIONSETTINGS, STATUS);
	GNRAISE(err);
	}

//
//  Set System Region
//

DLLP1 Error DLLP2 DDP_SetSystemRegion(UnitSet units, char driveLetter, BYTE systemRegion)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(SETSYSTEMREGION, CONFIG);

	GNREASSERT(GetDriveIterator(driveName, rit));
	err = RegionMan->SetSystemRegion(units, rit, systemRegion);
	delete rit;

	LOGDDPEXIT(SETSYSTEMREGION, CONFIG);
	GNRAISE(err);
	}

//
//  Check Drive For DVD
//
//  Return values:
//  GNR_OK, GNR_OBJECT_NOT_FOUND
//

Error CheckDriveForDVD(UnitSet units, RootIterator * rit, GenericFileSystem * gfs, DVDDiskType & type)
	{
	GenericFileSystemIterator * root;
	GenericFileSystemIterator * item;
	Error err;

	DP("Trying DVD ...");

	//
	//  Test on DVD
	//  One of those files must be present
	//

	GNREASSERT(gfs->CreateIterator(root));

	//
	// check for DVD-Video
	//

	err = gfs->FindItem("video_ts\\video_ts.ifo", DIT_FILE, FIF_NONE, root, item);

	if (IS_ERROR(err))
		err = gfs->FindItem("video_ts\\video_ts.inf", DIT_FILE, FIF_NONE, root, item);

	if (!IS_ERROR(err))
		{
		type = DDT_VIDEO_DVD;
		}
	else
		{
		//
		// check for DVD-Audio
		//

		if (IS_ERROR(err))
			{
			err = gfs->FindItem("audio_ts\\audio_ts.ifo", DIT_FILE, FIF_NONE, root, item);
			if (!IS_ERROR(err))
				type = DDT_AUDIO_DVD;
			}
		}

	//
	//  Cleanup
	//

	delete root;
	if (!IS_ERROR(err))
		{
		delete item;

		if (type == DDT_VIDEO_DVD)
			err = RegionMan->CheckRegionCodeValid(units, rit);
		GNRAISE(err);
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Check Drive For VCD. Might also be SVCD.
//
//  Return values:
//  GNR_OK, GNR_OBJECT_NOT_FOUND
//

Error CheckDriveForVCD(GenericFileSystem * gfs, DVDDiskType & type)
	{
	GenericFileSystemIterator * root;
	GenericFileSystemIterator * item;
	Error err;

	DP("Trying Video CD ...");

	//
	//  Test on VCD
	//  For an explanation of this function see VCDHeader.cpp: VCDInfo::Initialize
	//

	GNREASSERT(gfs->CreateIterator(root));

	if (!IS_ERROR(err = gfs->FindItem("vcd\\info.vcd", DIT_FILE, FIF_NONE, root, item)))
		type = DDT_VIDEO_CD;

	if (IS_ERROR(err))
		{
		if (!IS_ERROR(err = gfs->FindItem("svcd\\info.svd", DIT_FILE, FIF_NONE, root, item)))
			type = DDT_SUPER_VIDEO_CD;
		}

	if(IS_ERROR(err))
		{
		if (!IS_ERROR(err = gfs->FindItem("hqvcd\\info.vcd", DIT_FILE, FIF_NONE, root, item)))
			type = DDT_SUPER_VIDEO_CD;
		}

	if(IS_ERROR(err))
		{
		if (!IS_ERROR(err = gfs->FindItem("mpegav\\music01.dat", DIT_FILE, FIF_NONE, root, item)))
			type = DDT_VIDEO_CD;
		}

	//
	//  Cleanup
	//

	delete root;
	if (!IS_ERROR(err))
		{
		delete item;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Check what kind of disk we have
//
//  Return values:
//  GNR_OK, GNR_OBJECT_NOT_FOUND, GNR_REGION_*
//

Error CheckDrive(UnitSet units, const DiskItemName & driveName, DDPDiskInfo * diskInfo, GenericFileSystem * & gfs)
	{
	RootIterator * rit;
	GenericFileSystem * baseFS;
	PhysicalDiskType diskType;
	Error err = GNR_OK;

	DWORD inTime = Timer.GetMilliTicks(); //remove

	//
	//  Get us the file system and the type of the disk first
	//

	DPF("at %d ms: CheckDrive 0 - spent %d ms so far, err: %08x, abort: %d\n", Timer.GetMilliTicks(), Timer.GetMilliTicks() - inTime, err, getDiskInfoAsyncRqScheduler->IsAbortRequested()); //remove

	if (!getDiskInfoAsyncRqScheduler->IsAbortRequested())
		{
		diskInfo->type = DDT_NONE;
		GNREASSERT(GetDriveIterator(driveName, rit));

		err = FileSystem->LoadMedia(rit, 0, LMF_WAIT);

		if (IS_ERROR(err = rit->ObtainSubFileSystem(baseFS)))
			{
			delete rit;
			GNRAISE(err);
			}
		}

	if (getDiskInfoAsyncRqScheduler->IsAbortRequested())
		err = GNR_OPERATION_ABORTED;

	if (!IS_ERROR(err))
		err = baseFS->GetDiskType(diskType);

	DPF("at %d ms: CheckDrive 1 - spent %d ms so far, err: %08x, abort: %d\n", Timer.GetMilliTicks(), Timer.GetMilliTicks() - inTime, err, getDiskInfoAsyncRqScheduler->IsAbortRequested()); //remove

	//
	//  Now let's check if there are certain files that indicate the disk type
	//

	if (getDiskInfoAsyncRqScheduler->IsAbortRequested())
		err = GNR_OPERATION_ABORTED;

	if (!IS_ERROR(err))
		{
		switch (diskType)
			{
			case PHDT_NONE:
				err = GNR_OBJECT_NOT_FOUND;
				break;
			case PHDT_CDDA:
				diskInfo->type = DDT_AUDIO_CD;
				break;
			case PHDT_CDROM:
			case PHDT_CDROM_XA:
				//
				//  Assume it's a VCD first, then try DVD
				//  If you are nothing at all, but you have audio tracks,
				//  I'll assume your are AUDIO CD; if not, I'll call you MULTI_DISK
				//

				if (IS_ERROR(err = CheckDriveForVCD(baseFS, diskInfo->type)))
					{
					if (IS_ERROR(err = CheckDriveForDVD(units, rit, baseFS, diskInfo->type)))
						{
						GenericVolume* volume;
						VolumeInfo volumeInfo;
						//DP("From CheckDrive1.1 - V O:");
						if (!IS_ERROR(err = baseFS->ObtainVolume(volume)))
							{
							err = volume->GetVolumeInfo(volumeInfo);
							//DP("From CheckDrive1.1 - V R:");
							volume->Release();

							if (!IS_ERROR(err))
								{
								if (volumeInfo.attributes & VIA_CONTAINS_AUDIO_TRACKS)
									{
									diskInfo->type = DDT_AUDIO_CD;
									rit->ReleaseSubFileSystem();
									rit->ObtainSubFileSystem(baseFS, FS_CD_FILE_SYSTEM_REQUIRED);
									}
								else
									diskInfo->type = DDT_MULTI_DISK;
								}
							}
						}
					}
				break;
			case PHDT_DVD_ROM:
				if ((err = CheckDriveForDVD(units, rit, baseFS, diskInfo->type)) == GNR_OBJECT_NOT_FOUND)
					{
					diskInfo->type = DDT_MULTI_DISK;
					err = GNR_OK;
					}
				break;
			case PHDT_ILLEGAL:
				err = GNR_NO_VALID_DISK;
				break;
			default:
				err = GNR_UNEXPECTED_NAVIGATION_ERROR;
				break;
			}
		}

	DPF("at %d ms: CheckDrive 2 - spent %d ms so far, err: %08x, abort: %d\n", Timer.GetMilliTicks(), Timer.GetMilliTicks() - inTime, err, getDiskInfoAsyncRqScheduler->IsAbortRequested()); //remove

	//
	//  Create file system according to disk type
	//

	if (getDiskInfoAsyncRqScheduler->IsAbortRequested())
		err = GNR_OPERATION_ABORTED;

	if (!IS_ERROR(err))
		{
		switch (diskInfo->type)
			{
			case DDT_VIDEO_DVD:
			case DDT_AUDIO_DVD:
				err = dvdFileSystemFactory->CreateFileSystem(rit, SystemProfile, gfs);
				break;
			case DDT_AUDIO_CD:
			case DDT_DTS_AUDIO_CD:
				if (!IS_ERROR(err = cddaFileSystemFactory->CreateFileSystem(rit, SystemProfile, gfs)))
					{
					//Check if disk is DTS audio, and if so, make sure diskInfo->type is DDT_DTS_AUDIO_CD
					BOOL isDTS = FALSE;
					GenericCopyManagementInfo gcmi;
					err = gfs->GetDTSInfo(isDTS, gcmi);
					diskInfo->type = isDTS ? DDT_DTS_AUDIO_CD : DDT_AUDIO_CD;
					}
				break;
			case DDT_VIDEO_CD:
			case DDT_SUPER_VIDEO_CD:
				err = vcdFileSystemFactory->CreateFileSystem(rit, SystemProfile, gfs);
				break;
			case DDT_MULTI_DISK:
				err = avfFileSystemFactory->CreateFileSystem(rit, SystemProfile, gfs);
				break;
			default:
				err = GNR_UNEXPECTED_NAVIGATION_ERROR;
				break;
			}
		}

	//Cleanup after CreateFileSystem in case of abort or error
	if (!IS_ERROR(err) && getDiskInfoAsyncRqScheduler->IsAbortRequested())
		{
		//DP("From CheckDrive2.1 - FS R:");
		gfs->Release();
		gfs = NULL;
		err = GNR_OPERATION_ABORTED;
		}
	else if (IS_ERROR(err))
		gfs = NULL;

	DPF("at %d ms: CheckDrive 3 - spent %d ms so far, err: %08x, abort: %d\n", Timer.GetMilliTicks(), Timer.GetMilliTicks() - inTime, err, getDiskInfoAsyncRqScheduler->IsAbortRequested()); //remove

	//
	//  Compute unique key if wanted
	//

	if (getDiskInfoAsyncRqScheduler->IsAbortRequested())
		err = GNR_OPERATION_ABORTED;

	if (!IS_ERROR(err))
		err = gfs->GetUniqueKey((BYTE*)&(diskInfo->uniqueKey[0]));

	DPF("at %d ms: CheckDrive 4 - spent %d ms so far, err: %08x, abort: %d\n", Timer.GetMilliTicks(), Timer.GetMilliTicks() - inTime, err, getDiskInfoAsyncRqScheduler->IsAbortRequested()); //remove

	if (getDiskInfoAsyncRqScheduler->IsAbortRequested())
		err = GNR_OPERATION_ABORTED;

	if (!IS_ERROR(err) && (diskInfo->type == DDT_VIDEO_DVD || diskInfo->type == DDT_AUDIO_DVD) )
		err = RegionMan->GetDiskRegion(rit, diskInfo->diskRegion);
	else
		diskInfo->diskRegion = 0;

	//
	//  Cleanup and out of here
	//

	rit->ReleaseSubFileSystem();
	delete rit;
	DPF("CheckDrive done. It took %d msec\n", Timer.GetMilliTicks() - inTime); //remove

	//Release gfs in case error happened after gfs was successfully allocated
	if (IS_ERROR(err) && gfs)
		{
		//DP("From CheckDrive4.1 - FS R:");
		gfs->Release();
		gfs = NULL;
		}

	GNRAISE(err);
	}

//
//  Check drive with drive letter
//

DLLP1 Error DLLP2 DDP_CheckDrive(char driveLetter, char __far * id)
	{
	DiskItemName driveName(driveLetter);
	DDPDiskInfo diskInfo;
	GenericFileSystem * gfs = NULL;
	Error err;

	LOGDDP(CHECKDRIVE, STATUS);

	//
	//  Check drive letter
	//

	if (driveLetter < 'A' || driveLetter > 'Z')
		{
		DP("CheckDrive: Illegal drive letter! %c", driveLetter);
		GNRAISE(GNR_INVALID_DRIVE_LETTER);
		}

	if (!IS_ERROR(err = CheckDrive(NULL, driveName, &diskInfo, gfs)) && gfs)
		{
		//DP("From DDP_CheckDrive - FS R:");
		gfs->Release();
		}

	if (id)
		memcpy(id, diskInfo.uniqueKey, 8);

	LOGDDPEXIT(CHECKDRIVE, STATUS);
	GNRAISE(err);
	}

//
//  Check drive with path
//

DLLP1 Error DLLP2  DDP_CheckDriveWithPath(char * drivePath, char __far * id)
	{
	DDPDiskInfo diskInfo;
	DiskItemName driveName(drivePath);
	GenericFileSystem * gfs = NULL;
	Error err;

	LOGDDP(CHECKDRIVEWITHPATH, STATUS);

	if (!IS_ERROR(err = CheckDrive(NULL, driveName, &diskInfo, gfs)) && gfs)
		{
		//DP("From DDP_CheckDriveWithPath - FS R:");
		gfs->Release();
		}

	if (id)
		memcpy(id, diskInfo.uniqueKey, 8);

	LOGDDPEXIT(CHECKDRIVEWITHPATH, STATUS);
	GNRAISE(err);
	}

//
//  Open Player
//

Error OpenPlayer(UnitSet units, const DiskItemName & driveName, DVDDiskPlayer & player, DDPDiskInfo * diskInfo)
	{
	GenericFileSystem * gfs = NULL;
	RootIterator * rit;
	DVDPlayerMsg msg;
	DiskItemName name(driveName);

	//
	//  Determine disk type
	//

	DPF("Enter OpenPlayer");
	DWORD inTime = Timer.GetMilliTicks(); //remove

	GNREASSERT(CheckDrive(units, driveName, diskInfo, gfs));

	//
	//  Wait for scheduler to become ready
	//


	DWORD inTime2 = Timer.GetMilliTicks(); //remove

	//
	//  Initialize message
	//

	msg.units = units;
	msg.str = (TCHAR*)name;
	msg.player = NULL;
	msg.error = GNR_OK;
	msg.pointer = gfs;

	//
	// Open appropriate player
	//


	if (IS_ERROR(msg.error = GetDriveIterator(driveName, rit)))
		rit = NULL;

	if (!IS_ERROR(msg.error))
		{
		msg.diskType = diskInfo->type;
		msg.pointer2 = rit;

		switch (diskInfo->type)
			{
			case DDT_VIDEO_DVD:
				ddpScheduler->DoMessage(DDPMSG_OPENDVDVIDEOPLAYER, (DWORD)&msg);
				break;
			case DDT_AUDIO_DVD:
				ddpScheduler->DoMessage(DDPMSG_OPENDVDAUDIOPLAYER, (DWORD)&msg);
				break;
			case DDT_VIDEO_CD:
			case DDT_SUPER_VIDEO_CD:	// Intended fall through
				ddpScheduler->DoMessage(DDPMSG_OPENVCDPLAYER, (DWORD)&msg);
				break;
			case DDT_AUDIO_CD:
			case DDT_DTS_AUDIO_CD:		// Intended fall through
				ddpScheduler->DoMessage(DDPMSG_OPENCDDAPLAYER, (DWORD)&msg);
				break;
			case DDT_MULTI_DISK:
				ddpScheduler->DoMessage(DDPMSG_OPENAVFPLAYER, (DWORD)&msg);
				break;
			default:
				msg.error = GNR_UNEXPECTED_NAVIGATION_ERROR;
				break;
			}
		}

	//
	//  Return the results
	//

	player = msg.player;
	delete rit;
	if (IS_ERROR(msg.error) && gfs)
		{
		//DP("From OpenPlayer - FS R:");
		gfs->Release();
		}

	DPF("OpenPlayer done, it took %d ms in total, %d ms after CheckDrive\n", Timer.GetMilliTicks()-inTime, Timer.GetMilliTicks()-inTime2);
	GNRAISE(msg.error);
	}


//
//  Open Player with direct call. This function is intented to be called during handling asynchrnous open player request.
//

Error OpenPlayerDirect(UnitSet units, const DiskItemName & driveName, DVDDiskPlayer & player, DDPDiskInfo * diskInfo)
	{
	GenericFileSystem * gfs = NULL;
	RootIterator * rit = NULL;
	Error err;

	//
	//  Determine disk type
	//

	DPF("At %d: Enter OpenPlayerDirect\n", Timer.GetMilliTicks());
	DWORD inTime = Timer.GetMilliTicks(); //remove

	GNREASSERT(err = CheckDrive(units, driveName, diskInfo, gfs));

	DWORD inTime2 = Timer.GetMilliTicks(); //remove

	//
	// Open appropriate player
	//

	player = NULL;
	if (IS_ERROR(err = GetDriveIterator(driveName, rit)))
		rit = NULL;

	if (!IS_ERROR(err))
		{
		switch (diskInfo->type)
			{
			case DDT_VIDEO_DVD:
				{
				BYTE systemRegion, diskRegion, availSets;
				RegionSource regionSource;

				LOGDDP(OPENDVDVIDEOPLAYER, PLAYBACK);

				if (!IS_ERROR(err = RegionMan->GetRegionSettings(units, rit, systemRegion, diskRegion, regionSource, availSets)))
					err = dvdVideoDiskPlayerFactory->CreateDiskPlayer(DriverInstance, units, SystemProfile, diskInfo->type,
																						(DVDFileSystem*)gfs, systemRegion, player);
				LOGDDPEXIT(OPENDVDVIDEOPLAYER, PLAYBACK);
				}
				break;
			case DDT_AUDIO_DVD:
				LOGDDP(OPENDVDAUDIOPLAYER, PLAYBACK);
				err = dvdAudioDiskPlayerFactory->CreateDiskPlayer(DriverInstance, units, SystemProfile, diskInfo->type,
																					(DVDFileSystem*)gfs, 0, player);
				LOGDDPEXIT(OPENDVDAUDIOPLAYER, PLAYBACK);
				break;
			case DDT_VIDEO_CD:
			case DDT_SUPER_VIDEO_CD:	// Intended fall through
				LOGDDP(OPENVCDPLAYER, PLAYBACK);
				err = vcdDiskPlayerFactory->CreateDiskPlayer(DriverInstance, units, SystemProfile, diskInfo->type,
																			  (VCDFileSystem*)gfs, player);
				LOGDDPEXIT(OPENVCDPLAYER, PLAYBACK);
				break;
			case DDT_AUDIO_CD:
			case DDT_DTS_AUDIO_CD:		// Intended fall through
				LOGDDP(OPENCDDAPLAYER, PLAYBACK);
				err = cddaDiskPlayerFactory->CreateDiskPlayer(DriverInstance, units, SystemProfile, diskInfo->type,
																				(CDDAFileSystem*)gfs, player);
				LOGDDPEXIT(OPENCDDAPLAYER, PLAYBACK);
				break;
			case DDT_MULTI_DISK:
				LOGDDP(OPENAVFPLAYER, PLAYBACK);
				err = avfDiskPlayerFactory->CreateDiskPlayer(DriverInstance, units, SystemProfile, diskInfo->type,
																			  (GenericFileSystem*)gfs, player);
				LOGDDPEXIT(OPENAVFPLAYER, PLAYBACK);
				break;
			default:
				err = GNR_UNEXPECTED_NAVIGATION_ERROR;
				break;
			}
		}

	//
	//  Return the results
	//

	delete rit;
	if (IS_ERROR(err) && gfs)
		{
		//DP("From OpenPlayerDirect - FS R:");
		gfs->Release();
		}

	if (getDiskInfoAsyncRqScheduler->IsAbortRequested())
		{
		err = GNR_OPERATION_ABORTED;
		player = NULL;
		}
	DPF("OpenPlayerDirect done, it took %d ms in total, %d ms after CheckDrive\n", Timer.GetMilliTicks()-inTime, Timer.GetMilliTicks()-inTime2);
	GNRAISE(err);
	}

//
//  Open Player
//

DLLP1 Error DLLP2 DDP_OpenPlayer(UnitSet units, char driveLetter, DVDDiskPlayer __far & player)
	{
	DDPDiskInfo dummy;
	DiskItemName driveName(driveLetter);
	Error err = GNR_INVALID_DRIVE_LETTER;

	LOGDDP(OPENPLAYER, PLAYBACK);

	//
	//  Check drive letter
	//

	if (driveLetter >= 'A' || driveLetter <= 'Z')
		err = OpenPlayer(units, driveName, player, &dummy);
	else
		DP("CheckDrive: Illegal drive letter! %c", driveLetter);

	LOGDDPEXIT(OPENPLAYER, PLAYBACK);
	GNRAISE(err);
	}

//
//  Open Player With Path
//

DLLP1 Error DLLP2 DDP_OpenPlayerWithPath(UnitSet units, char * drivePath, DVDDiskPlayer __far & player)
	{
	DDPDiskInfo dummy;
	DiskItemName driveName(drivePath);
	Error err;

	LOGDDP(OPENPLAYERWITHPATH, PLAYBACK);
	err = OpenPlayer(units, driveName, player, &dummy);
	LOGDDPEXIT(OPENPLAYERWITHPATH, PLAYBACK);
	GNRAISE(err);
	}

//
//  Extended Open Player
//

DLLP1 Error DLLP2 DDP_OpenPlayerExtended(UnitSet units, char driveLetter, char * drivePath,
													  DVDDiskPlayer __far & player, DDPDiskInfo __far & diskInfo)
	{
	DDPDiskInfo di;
	Error err = GNR_OK;
	DiskItemName driveName;

	LOGDDP(OPENPLAYEREXTENDED, PLAYBACK);

	//
	//  Check drive letter
	//

	if (!drivePath)
		{
		if (driveLetter < 'A' || driveLetter > 'Z')
			err = GNR_INVALID_DRIVE_LETTER;

		driveName = DiskItemName(driveLetter);
		}
	else
		driveName = DiskItemName(drivePath);

	if (!IS_ERROR(err) && !IS_ERROR(err = OpenPlayer(units, driveName, player, &di)))
		{
		di.size = diskInfo.size;
		memcpy(&diskInfo, &di, min(diskInfo.size, di.size));
		}

	LOGDDPEXIT(OPENPLAYEREXTENDED, PLAYBACK);
	GNRAISE(err);
	}


//
//  Close player
//

DLLP1 Error DLLP2 DDP_ClosePlayer(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_CLOSEPLAYER, (DWORD)&msg);

	GNRAISE(msg.error);
	}

//
//  Get Drive Status
//  Return values:
//  GNR_OK, GNR_INVALID_DRIVE_LETTER
//

#if MSNDISC
DLLP1 Error DLLP2 DDP_GetDriveStatus(char driveLetter, BOOL & opened, BOOL & closed)
	{
	RootIterator * rit;
	DWORD status;
	DWORD position;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(GETDRIVESTATUS, STATUS);

	if (driveLetter < 'A' || driveLetter > 'Z')
		err = GNR_INVALID_DRIVE_LETTER;
	else
		{
		if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
			{
			err = FileSystem->GetChangerStatus(rit, status, position);

			if (!IS_ERROR(err))
				{

				if ( status & GCS_OPEN )
					{
					opened = TRUE;
					closed = FALSE;
					}
				else
				if ( status & GCS_CLOSED )
					{
					opened = FALSE;
					closed = TRUE;
					}
				else
				if (( status & GCS_OPENING ) || ( status & GCS_CLOSING) )
					{
					opened = FALSE;
					closed = FALSE;
					}
				else
					{
					opened = TRUE;
					closed = TRUE;
					}
				}
			delete rit;
			}
		}

	LOGDDPEXIT(GETDRIVESTATUS, STATUS);
	GNRAISE(err);
	}


#else

DLLP1 Error DLLP2 DDP_GetDriveStatus(char driveLetter, BOOL & opened, BOOL & closed)
	{
	RootIterator * rit;
	GenericTrayStatus status;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(GETDRIVESTATUS, STATUS);

	if (driveLetter < 'A' || driveLetter > 'Z')
		err = GNR_INVALID_DRIVE_LETTER;
	else
		{
		if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
			{
			err = FileSystem->GetTrayStatus(rit, status);

			if (!IS_ERROR(err))
				{
				switch (status)
					{
					case GTS_TRAY_OPEN:
						opened = TRUE;
						closed = FALSE;
						break;
					case GTS_TRAY_CLOSED:
						opened = FALSE;
						closed = TRUE;
						break;
					case GTS_TRAY_MOVING:
						opened = FALSE;
						closed = FALSE;
						break;
					case GTS_TRAY_UNKNOWN:
						opened = TRUE;
						closed = TRUE;
						break;
					}
				}
			delete rit;
			}
		}

	LOGDDPEXIT(GETDRIVESTATUS, STATUS);
	GNRAISE(err);
	}

#endif

//
//  Load Disk In Drive
//

DLLP1 Error DLLP2 DDP_LoadDiskInDrive(char driveLetter, BOOL wait)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	DWORD flags;
	Error err;

	LOGDDP(LOADDISKINDRIVE, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		if (wait)
			flags = LMF_WAIT;
		else
			flags = LMF_NONE;
		err = FileSystem->LoadMedia(rit, 0, flags);
		delete rit;
		}

	LOGDDPEXIT(LOADDISKINDRIVE, MISC);
	GNRAISE(err);
	}

//
//  Eject Disk In Drive
//

DLLP1 Error DLLP2 DDP_EjectDiskInDrive(char driveLetter)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(EJECTDISKINDRIVE, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		err = FileSystem->UnloadMedia(rit, UMF_NONE);
		delete rit;
		}

	LOGDDPEXIT(EJECTDISKINDRIVE, MISC);
	GNRAISE(err);
	}

#if MSNDISC

DLLP1 Error DLLP2 DDP_LoadMedia(char driveLetter, BOOL wait)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	DWORD flags;
	Error err;

	LOGDDP(LOADMEDIA, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		if (wait)
			flags = LMF_WAIT;
		else
			flags = LMF_NONE;
		err = FileSystem->LoadMedia(rit, 0, flags);
		delete rit;
		}

	LOGDDPEXIT(LOADMEDIA, MISC);
	GNRAISE(err);
	}


DLLP1 Error DLLP2 DDP_UnloadMedia(char driveLetter, BOOL wait)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	DWORD flags;
	Error err;

	LOGDDP(UNLOADMEDIA, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		if (wait)
			flags = UMF_WAIT;
		else
			flags = UMF_NONE;
		err = FileSystem->UnloadMedia(rit, flags);
		delete rit;
		}

	LOGDDPEXIT(UNLOADMEDIA, MISC);
	GNRAISE(err);
	}


DLLP1 Error DLLP2  DDP_GetDriveCaps(char driveLetter, DWORD &caps, DWORD &slots, DWORD& positions, DWORD& changerOpenOffset )
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(GETDRIVECAPS, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		err = FileSystem->GetDriveCaps(rit, caps, slots, positions, changerOpenOffset );
		delete rit;
		}

	LOGDDPEXIT(GETDRIVECAPS, MISC);
	GNRAISE(err);
	}

DLLP1 Error DLLP2 DDP_MoveChanger(char driveLetter, DWORD operation, DWORD flags, DWORD param)
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(MOVECHANGER, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		err = FileSystem->MoveChanger(rit, operation, flags, param);
		delete rit;
		}

	LOGDDPEXIT(MOVECHANGER, MISC);
	GNRAISE(err);
	}

DLLP1 Error DLLP2 DDP_GetChangerStatus(char driveLetter, DWORD& status, DWORD& position )
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(GETCHANGERSTATUS, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		err = FileSystem->GetChangerStatus(rit,status,position );
		delete rit;
		}

	LOGDDPEXIT(GETCHANGERSTATUS, MISC);
	GNRAISE(err);
	}

DLLP1 Error DLLP2 DDP_GetSlotStatus(char driveLetter, DWORD slot, DWORD& status )
	{
	RootIterator * rit;
	DiskItemName driveName(driveLetter);
	Error err;

	LOGDDP(GETSLOTSTATUS, MISC);

	if (!IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		err = FileSystem->GetSlotStatus(rit,slot,status);
		delete rit;
		}

	LOGDDPEXIT(GETSLOTSTATUS, MISC);
	GNRAISE(err);
	}
#endif



//
//  Get disk type
//

DLLP1 DVDDiskType DLLP2 DDP_GetDiskType(DVDDiskPlayer player)
	{
	DVDDiskType type;
	Error err;

	LOGDDP(GETDISKTYPE, STATUS);
	if (IS_ERROR(err = player->GetDiskType(type)))
		type = DDT_NONE;

	LOGDDPEXIT(GETDISKTYPE, STATUS);
	return type;
	}


//
//  Get info asinchonously (return right away and report result later by calling ARQCallback).
//  On return rqHandle contains unique request to request being processed. Application should
//  pass this handle when calling DDP_AbortAsyncRequest()
//

DLLP1 Error DLLP2 DDP_GetDiskInfoAsync(char * drivePath, ARQAsyncRequestDoneCB ARQCallback, DDPDiskOnlyInfo* diskOnlyInfo, DWORD & rqHandle)
	{
	Error err;
	LOGDDP(GETDISKINFOASYNC, STATUS);
	KernelString ksDrivePath(drivePath);
	err = getDiskInfoAsyncRqScheduler->ScheduleRequest(ARQ_GET_DISK_INFO, ksDrivePath, ARQCallback, (void*)diskOnlyInfo, rqHandle);
	LOGDDPEXIT(GETDISKINFOASYNC, STATUS);
	GNRAISE(err);
	}

//
//  Open player asynchrnously (return right away and report result later by calling ARQCallback).
//  On return rqHandle contains unique request to request being processed. Application should
//  pass this handle when calling DDP_AbortAsyncRequest()
//

DLLP1 Error DLLP2 DDP_OpenPlayerAsync(char * drivePath, ARQAsyncRequestDoneCB ARQCallback, DDPOpenPlayerInfo* openPlayerInfo, DWORD & rqHandle)
	{
	Error err = GNR_OK;
	KernelString ksDrivePath;

	LOGDDP(OPENPLAYERASYNC, PLAYBACK);

	DPF("At %d: Enter DDP_OpenPlayerAsync\n", Timer.GetMilliTicks());
	ksDrivePath = drivePath;
	err = getDiskInfoAsyncRqScheduler->ScheduleRequest(ARQ_OPEN_PLAYER, ksDrivePath, ARQCallback, (void*)openPlayerInfo, rqHandle);
	DPF("At %d: OpenPlayerAsync returns %08x\n", Timer.GetMilliTicks(), err);

	LOGDDPEXIT(OPENPLAYERASYNC, PLAYBACK);
	GNRAISE(err);
	}


//
// Abort asynchronous request
// dwHandle is ID of request to be aborted (obtained from one of DDP_...Async() calls)
//

DLLP1 Error DLLP2 DDP_AbortAsyncRequest(DWORD rqHandle)
	{
	Error err;
	LOGDDP(ABORTGETDISKINFOASYNC, STATUS);
	err = getDiskInfoAsyncRqScheduler->AbortAndDisableDiskRequests(rqHandle);
	LOGDDPEXIT(ABORTGETDISKINFOASYNC, STATUS);
	GNRAISE(err);
	}

//
// GetDiskInfoAsyncRequestScheduler::ScheduleRequest
//

Error GetDiskInfoAsyncRequestScheduler::ScheduleRequest(ARQReqestID asyncRqID, KernelString& drivePath, ARQAsyncRequestDoneCB ARQCallback, void* rqData, DWORD & rqHandle)
	{
	if (!ARQCallback)
		return GNR_INVALID_PARAMETERS;

	if (IsRequestInProgress())
		return GNR_OBJECT_IN_USE; //sorry, only one request at a time...

	SetRequestInProgressFlag(TRUE); //has to be reset to FALSE by DDPMSG_PROCESS_ASYNC_RQ message handler

	rqPending.rqHandle = rqHandle = GetNewHandle();
	rqPending.asyncRqID = asyncRqID;
	rqPending.rqData = rqData;
	rqPending.drivePath = drivePath;
	rqPending.asyncRequestDoneCallback = ARQCallback;

	msg.dword = rqHandle;

	if (ddpScheduler->SendMessage(DDPMSG_PROCESS_ASYNC_RQ, (DWORD)&msg))
		GNRAISE_OK;
	else
		GNRAISE(GNR_OPERATION_FAILED);
	}


//
//  ProcessAsyncRequest() processes DDPMSG_PROCESS_ASYNC_RQ message sent by GetDiskInfoAsyncRequestScheduler::ScheduleRequest()
//

Error GetDiskInfoAsyncRequestScheduler::ProcessAsyncRequest(DWORD rqHandle)
	{
	Error err = GNR_OK;

	asyncRqDone.Reset();

	//Generate error if request is not found
	if (rqHandle != constRqHandle)
		err = GNR_OBJECT_NOT_FOUND;

	if (!IS_ERROR(err))
		{
		DP("At %d ms: Process asyncRequest %d", Timer.GetMilliTicks(), rqPending.asyncRqID);//remove
		DiskItemName driveName(rqPending.drivePath);
		switch(rqPending.asyncRqID)
			{
			case ARQ_GET_DISK_INFO:
				{
				DDPDiskInfo diskInfo;
				GenericFileSystem * gfs = NULL;
				KernelString volumeName;
				if ( !IS_ERROR(err = CheckDrive(NULL, driveName, &diskInfo, gfs)) )
					{
					DDPDiskOnlyInfo* diskOnlyInfo = (DDPDiskOnlyInfo*)rqPending.rqData;
					gfs->GetVolumeName(volumeName);
					volumeName.Get(diskOnlyInfo->volumeName, 255);
					diskOnlyInfo->type = diskInfo.type;
					memcpy(diskOnlyInfo->uniqueKey, diskInfo.uniqueKey, 8);
					diskOnlyInfo->diskRegion = diskInfo.diskRegion;
					if (gfs)
						{
						//DP("From ProcessAsyncRequest - FS R:");
						gfs->Release();
						}
					}
				}
				break;

			case ARQ_OPEN_PLAYER:
				{
				DDPOpenPlayerInfo* openPlayerInfo = (DDPOpenPlayerInfo*)rqPending.rqData;
				err = OpenPlayerDirect((UnitSet)openPlayerInfo->units, driveName, *(DVDDiskPlayer*)(openPlayerInfo->pplayer), &openPlayerInfo->diskInfo);
				}
				break;
			}

		if (rqPending.asyncRequestDoneCallback)
			rqPending.asyncRequestDoneCallback(rqPending.asyncRqID, rqPending.rqHandle, rqPending.rqData, err);
		}

	asyncRqDone.Signal();
	DP("At %d ms: asyncRqDone set - returns %08x.\n", Timer.GetMilliTicks(), err);//remove
	SetRequestInProgressFlag(FALSE); //Make yourself available for new requests
	GNRAISE(err);
	}

//
//	 GetDiskInfoAsyncRequestScheduler::AbortAndDisableDiskRequests
//

Error GetDiskInfoAsyncRequestScheduler::AbortAndDisableDiskRequests(DWORD rqHandle)
	{
	RootIterator * rit;
	Error err;
	KernelString drivePath = "";

	DWORD timeIn = Timer.GetMilliTicks();//remove
	DPF("At %d: Enter abort\n", Timer.GetMilliTicks());//remove
	if (!IsRequestInProgress())
		{
		DPF("No request in progress - bye...\n");
		GNRAISE_OK;
		}

	//Generate error if request is not found
	if (rqHandle != constRqHandle)
		GNRAISE(GNR_OBJECT_NOT_FOUND);

	SetAbortRequestFlag(TRUE);
	DPF("Request in progress, trying to abort...\n");//remove

	DiskItemName driveName(rqPending.drivePath);
	if (IS_ERROR(err = GetDriveIterator(driveName, rit)))
		{
		SetAbortRequestFlag(FALSE);
		GNRAISE(err);
		}

	DPF("at %d ms: Telling disk to abort...\n", Timer.GetMilliTicks());//remove
	err = FileSystem->AbortAndDisableDiskRequests(rit);

	DPF("at %d ms: ...Disk told to abort. Wait for curent async call to finish...\n", Timer.GetMilliTicks());//remove
	asyncRqDone.Wait();
	SetAbortRequestFlag(FALSE);
	DPF("at %d ms: Done waiting (spent %d ms so far)\n", Timer.GetMilliTicks(), Timer.GetMilliTicks()-timeIn);//remove
	FileSystem->EnableDiskRequests(rit);

	delete rit;

	DPF("Abort done. Spent %d ms in total\n", Timer.GetMilliTicks()-timeIn);//remove
	GNRAISE(err);
	}


//
//  Start presentation
//

DLLP1 Error DLLP2 DDP_StartPresentation(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.dword = DDPSPF_NONE;

	ddpScheduler->DoMessage(DDPMSG_STARTPRESENTATION, (DWORD)&msg);

	GNRAISE(msg.error);
	}

//
//  Start presentation extended
//

DLLP1 Error DLLP2 DDP_StartPresentationExt(DVDDiskPlayer player, DWORD flags)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.dword = flags;

	ddpScheduler->DoMessage(DDPMSG_STARTPRESENTATION, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_GetCurrentLocation(DVDDiskPlayer player, DVDLocation & location)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.location = &location;

	ddpScheduler->DoMessage(DDPMSG_GETCURRENTLOCATION, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_GetCurrentDuration(DVDDiskPlayer player, DVDLocation & duration)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.location = &duration;

	ddpScheduler->DoMessage(DDPMSG_GETCURRENTDURATION, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_GetTitleDuration(DVDDiskPlayer player, WORD title, DVDTime & duration)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = title;

	ddpScheduler->DoMessage(DDPMSG_GETTITLEDURATION, (DWORD)&msg);

	duration = msg.time;
	GNRAISE(msg.error);
	}

DLLP1 DVDPlayerMode DLLP2 DDP_GetPlayerMode(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETPLAYERMODE, (DWORD)&msg);

	return msg.mode;
	}

DLLP1 Error DLLP2 DDP_GetExtendedPlayerState(DVDDiskPlayer player, ExtendedPlayerState & eps)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.pointer = &eps;

	ddpScheduler->DoMessage(DDPMSG_GETEXTENDEDPLAYERSTATE, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 DWORD DLLP2 DDP_GetForbiddenUserOperations(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETFORBIDDENUSEROPERATIONS, (DWORD)&msg);

	return msg.dword;
	}

DLLP1 WORD DLLP2 DDP_GetNumberOfTitles(DVDDiskPlayer player)
   {
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETNUMBEROFTITLES, (DWORD)&msg);

	return msg.word;
   }

DLLP1 WORD DLLP2 DDP_GetNumberOfPartOfTitle(DVDDiskPlayer player, WORD title)
   {
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;

	ddpScheduler->DoMessage(DDPMSG_GETNUMBEROFPARTOFTITLES, (DWORD)&msg);

	return msg.word;
   }

DLLP1 Error DLLP2 DDP_GetAvailStreams(DVDDiskPlayer player, BYTE __far & audioMask, DWORD __far & subPictureMask)
   {
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETAVAILSTREAMS, (DWORD)&msg);

	audioMask = msg.byte;
	subPictureMask = msg.dword;

	GNRAISE(msg.error);
   }

DLLP1 WORD DLLP2 DDP_GetCurrentAudioStream(DVDDiskPlayer player)
   {
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETCURRENTAUDIOSTREAM, (DWORD)&msg);

	DVDAudioStreamFormat format;
	DDP_GetAudioStreamAttributes(player, msg.word, format);

	return msg.word;
   }

DLLP1 WORD DLLP2 DDP_GetCurrentSubPictureStream(DVDDiskPlayer player)
   {
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETCURRENTSUBPICTURESTREAM, (DWORD)&msg);

	return msg.word;
   }

DLLP1 BOOL DLLP2 DDP_IsCurrentSubPictureEnabled(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_ISCURRENTSUBPICTURENENABLED, (DWORD)&msg);

	return msg.boolean;
	}

DLLP1 Error DLLP2 DDP_GetAudioStreamAttributes(DVDDiskPlayer player, WORD stream, DVDAudioStreamFormat __far & format)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = stream;
	msg.audioStreamFormat = &format;

	ddpScheduler->DoMessage(DDPMSG_GETAUDIOSTREAMATTRIBUTES, (DWORD)&msg);

	return msg.error;
	}

DLLP1 Error DLLP2 DDP_GetSubPictureStreamAttributes(DVDDiskPlayer player, WORD stream, DVDSubPictureStreamFormat __far & format)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = stream;
	msg.subPictureStreamFormat = &format;

	ddpScheduler->DoMessage(DDPMSG_GETSUBPICTURESTREAMATTRIBUTES, (DWORD)&msg);

	return msg.error;
	}

DLLP1 DisplayPresentationMode DLLP2  DDP_GetCurrentDisplayMode(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETCURRENTDISPLAYMODE, (DWORD)&msg);

	return msg.dpmode;
	}

DLLP1 Error DLLP2  DDP_SetDisplayMode(DVDDiskPlayer player, DisplayPresentationMode mode)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.dpmode = mode;

	ddpScheduler->DoMessage(DDPMSG_SETDISPLAYMODE, (DWORD)&msg);

	return msg.error;
	}

DLLP1 WORD DLLP2 DDP_GetNumberOfAngles(DVDDiskPlayer player, WORD title)
   {
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;

	ddpScheduler->DoMessage(DDPMSG_GETNUMBEROFANGLES, (DWORD)&msg);

	return msg.word;
   }

DLLP1 WORD DLLP2 DDP_GetCurrentAngle(DVDDiskPlayer player)
   {
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETCURRENTANGLE, (DWORD)&msg);

	return msg.word;
   }

DLLP1 BOOL DLLP2 DDP_CheckMenuAvail(DVDDiskPlayer player, VTSMenuType menu)
   {
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.menu = menu;

	ddpScheduler->DoMessage(DDPMSG_CHECKMENUAVAIL, (DWORD)&msg);

	return msg.boolean;
   }


DLLP1 Error DLLP2 DDP_TitlePlay(DVDDiskPlayer player, WORD title)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;

	ddpScheduler->DoMessage(DDPMSG_TITLEPLAY, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_TitlePlayForced(DVDDiskPlayer player, WORD title)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;

	ddpScheduler->DoMessage(DDPMSG_TITLEPLAYFORCED, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PTTPlay(DVDDiskPlayer player, WORD title, WORD part)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;
	msg.part = part;

	ddpScheduler->DoMessage(DDPMSG_PTTPLAY, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PTTPlayForced(DVDDiskPlayer player, WORD title, WORD part)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;
	msg.part = part;

	ddpScheduler->DoMessage(DDPMSG_PTTPLAYFORCED, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_TimePlay(DVDDiskPlayer player, WORD title, DVDTime time)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;
	msg.time = time;

	ddpScheduler->DoMessage(DDPMSG_TIMEPLAY, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_TimePlayForced(DVDDiskPlayer player, WORD title, DVDTime time)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.title = title;
	msg.time = time;

	ddpScheduler->DoMessage(DDPMSG_TIMEPLAYFORCED, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ExtendedPlay(DVDDiskPlayer player, DWORD flags, WORD title, WORD ptt, DVDTime time)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.dword = flags;
	msg.title = title;
	msg.part = ptt;
	msg.time = time;

	ddpScheduler->DoMessage(DDPMSG_EXTENDEDPLAY, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_Stop(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_STOP, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_GoUp(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GOUP, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_TimeSearch(DVDDiskPlayer player, DVDTime time)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.time = time;

	ddpScheduler->DoMessage(DDPMSG_TIMESEARCH, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_TimeSearchForced(DVDDiskPlayer player, DVDTime time)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.time = time;

	ddpScheduler->DoMessage(DDPMSG_TIMESEARCHFORCED, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PTTSearch(DVDDiskPlayer player, WORD part)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.part = part;

	ddpScheduler->DoMessage(DDPMSG_PTTSEARCH, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PTTSearchForced(DVDDiskPlayer player, WORD part)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.part = part;

	ddpScheduler->DoMessage(DDPMSG_PTTSEARCHFORCED, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PrevPGSearch(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_PREVPGSEARCH, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_TopPGSearch(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_TOPPGSEARCH, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_NextPGSearch(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_NEXTPGSEARCH, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ForwardScan(DVDDiskPlayer player, WORD speed)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = speed;

	ddpScheduler->DoMessage(DDPMSG_FORWARDSCAN, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_BackwardScan(DVDDiskPlayer player, WORD speed)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = speed;

	ddpScheduler->DoMessage(DDPMSG_BACKWARDSCAN, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_TrickPlay(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_TRICKPLAY, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_StopScan(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_STOPSCAN, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ReversePlayback(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_REVERSEPLAY, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_MenuCall(DVDDiskPlayer player, VTSMenuType menu)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.menu = menu;

	ddpScheduler->DoMessage(DDPMSG_MENUCALL, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_Resume(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_RESUME, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_UpperButtonSelect(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_UPPERBUTTONSELECT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_LowerButtonSelect(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_LOWERBUTTONSELECT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_LeftButtonSelect(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_LEFTBUTTONSELECT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_RightButtonSelect(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_RIGHTBUTTONSELECT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ButtonSelectAt(DVDDiskPlayer player, WORD x, WORD y)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.x = x; msg.y = y;

	ddpScheduler->DoMessage(DDPMSG_BUTTONSELECTAT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 BOOL DLLP2 DDP_IsButtonAt(DVDDiskPlayer player, WORD x, WORD y)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.x = x; msg.y = y;

	ddpScheduler->DoMessage(DDPMSG_ISBUTTONAT, (DWORD)&msg);

	return msg.boolean;
	}

DLLP1 Error DLLP2 DDP_ButtonActivate(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_BUTTONACTIVATE, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ButtonSelectAndActivate(DVDDiskPlayer player, WORD num)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = num;

	ddpScheduler->DoMessage(DDPMSG_BUTTONSELECTANDACTIVATE, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ButtonSelectAndActivateAt(DVDDiskPlayer player, WORD x, WORD y)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.x = x; msg.y = y;

	ddpScheduler->DoMessage(DDPMSG_BUTTONSELECTANDACTIVATEAT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_StillOff(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_STILLOFF, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PauseOn(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_PAUSEON, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PauseOff(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_PAUSEOFF, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_PbcOn(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_PBCON, (DWORD)&msg);

	GNRAISE_OK;
	}

DLLP1 Error DLLP2 DDP_PbcOff(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_PBCOFF, (DWORD)&msg);

	GNRAISE_OK;
	}

DLLP1 Error DLLP2 DDP_AdvanceFrame(DVDDiskPlayer player)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_ADVANCEFRAME, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_AdvanceFrameBy(DVDDiskPlayer player, int by)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.val = by;

	ddpScheduler->DoMessage(DDPMSG_ADVANCEFRAMEBY, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_SetPlaybackSpeed(DVDDiskPlayer player, WORD speed)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = speed;

	ddpScheduler->DoMessage(DDPMSG_SETPLAYBACKSPEED, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_MenuLanguageSelect(DVDDiskPlayer player, WORD language)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = language;

	ddpScheduler->DoMessage(DDPMSG_MENULANGUAGESELECT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_AudioStreamChange(DVDDiskPlayer player, WORD streamID)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = streamID;

	ddpScheduler->DoMessage(DDPMSG_AUDIOSTREAMCHANGE, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_SubPictureStreamChange(DVDDiskPlayer player, WORD streamID, BOOL display)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = streamID;
	msg.boolean = display;

	ddpScheduler->DoMessage(DDPMSG_SUBPICTURESTREAMCHANGE, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_AngleChange(DVDDiskPlayer player, WORD angle)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = angle;

	ddpScheduler->DoMessage(DDPMSG_ANGLECHANGE, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ParentalLevelSelect(DVDDiskPlayer player, WORD parentalLevel)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = parentalLevel;

	ddpScheduler->DoMessage(DDPMSG_PARENTALLEVELSELECT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_ParentalCountrySelect(DVDDiskPlayer player, WORD country)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.word = country;

	ddpScheduler->DoMessage(DDPMSG_PARENTALCOUNTRYSELECT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_InitialLanguageSelect(DVDDiskPlayer player,
														  WORD audioLanguage,
														  WORD audioExtension,
														  WORD subPictureLanguage,
														  WORD subPictureExtension)
	{
	DVDPlayerMsg	msg;

	msg.player = player;
	msg.audioLanguage = audioLanguage;
	msg.audioExtension = audioExtension;
	msg.subPictureLanguage = subPictureLanguage;
	msg.subPictureExtension = subPictureExtension;

	ddpScheduler->DoMessage(DDPMSG_SELECTINITIALLANGUAGE, (DWORD)&msg);

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2  DDP_InquireCurrentBitRate(DVDDiskPlayer player, DWORD & bitsPerSecond)
	{
	Error err;

	LOGDDP(INQUIRECURRENTBITRATE, STATUS);
	err = player->GetCurrentBitrate(bitsPerSecond);
	LOGDDPEXIT(INQUIRECURRENTBITRATE, STATUS);
	GNRAISE(err);
	}

DLLP1 Error DLLP2  DDP_GetCurrentButtonState(DVDDiskPlayer player, WORD & minButton, WORD & numButtons, WORD & currentButton)
	{
	DVDPlayerMsg	msg;

	msg.player = player;

	ddpScheduler->DoMessage(DDPMSG_GETCURRENTBUTTONSTATE, (DWORD)&msg);

	minButton = msg.word;
	numButtons = msg.word2;
	currentButton = msg.word3;

	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_Freeze(DVDDiskPlayer player, BYTE * buffer,  DWORD & size)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.dword = size;
	msg.pointer = buffer;

	ddpScheduler->DoMessage(DDPMSG_FREEZE, (DWORD)&msg);

	size = msg.dword;
	GNRAISE(msg.error);
	}

DLLP1 Error DLLP2 DDP_Defrost(DVDDiskPlayer player, BYTE * buffer, DWORD & size)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.dword = size;
	msg.pointer = buffer;
	msg.dword2 = DDPSPF_NONE;

	ddpScheduler->DoMessage(DDPMSG_DEFROST, (DWORD)&msg);

	size = msg.dword;
	GNRAISE(msg.error);
	}

DLLCALL Error WINAPI DDP_DefrostExt(DVDDiskPlayer player, BYTE * buffer, DWORD & size, DWORD flags)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.dword = size;
	msg.pointer = buffer;
	msg.dword2 = flags;

	ddpScheduler->DoMessage(DDPMSG_DEFROST, (DWORD)&msg);

	size = msg.dword;
	GNRAISE(msg.error);
	}

//
//  Extract information from frozen state
//

DLLCALL Error WINAPI DDP_GetFrozenInfo(BYTE * buffer, DWORD length, DDPFreezeState & state)
	{
	DDPFreezeState * bufState = (DDPFreezeState*)buffer;
	int i;

	for (i=0; i<8; i++)
		state.uniqueKey[i] = bufState->uniqueKey[i];

	state.diskType = bufState->diskType;
	state.parentalLevel = bufState->parentalLevel;
	state.parentalCountry = bufState->parentalCountry;

	GNRAISE_OK;
	}

//
//  Turn Event Handler On
//

DLLP1 Error DLLP2 DDP_InstallEventHandler(DVDDiskPlayer player, DWORD event, DNEEventHandler handler, void * userData)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.dword = event;
	msg.pointer = (void *) handler;
	msg.pointer2 = userData;

	ddpScheduler->DoMessage(DDPMSG_EVENTHANDLERON, (DWORD)&msg);

	GNRAISE(msg.error);
	}

//
//  Turn Event Handler Off
//

DLLP1 Error DLLP2 DDP_RemoveEventHandler(DVDDiskPlayer player, DWORD event)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.dword = event;

	ddpScheduler->DoMessage(DDPMSG_EVENTHANDLEROFF, (DWORD)&msg);

	GNRAISE(msg.error);
	}

//
//  Set a breakpoint, "old style"
//

DLLP1 Error DLLP2 DDP_SetBreakpoint(DVDDiskPlayer player, WORD title, DVDTime time, DWORD flags, DWORD & id)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.word = title;
	msg.time = time;
	msg.dword = ERS_FORWARD| ERS_BACKWARD;

	//
	//  Translate "old style" flags to "new style" flags
	//

	if (flags & ERS_PAUSE)
		msg.dword |= ERS_TOSTILL;

	if (flags & ERS_AUTOCLEAR)
		msg.dword |= ERS_CLEAR;

	if (flags & ERS_PAUSEATEND)
		msg.dword |= ERS_ENDOFTITLE;
	else
		msg.dword |= ERS_ATTIME;

	ddpScheduler->DoMessage(DDPMSG_SETBREAKPOINT, (DWORD)&msg);

	id = msg.dword2;
	GNRAISE(msg.error);
	}

//
//  Set a breakpoint, "new style"
//

DLLP1 Error DLLP2 DDP_SetBreakpointExt(DVDDiskPlayer player, WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.word = title;
	msg.word2 = ptt;
	msg.time = time;
	msg.dword = flags;

	ddpScheduler->DoMessage(DDPMSG_SETBREAKPOINT, (DWORD)&msg);

	id = msg.dword2;
	GNRAISE(msg.error);
	}

//
//  Clear a breakpoint
//

DLLP1 Error DLLP2 DDP_ClearBreakpoint(DVDDiskPlayer player, DWORD id)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.dword = id;

	ddpScheduler->DoMessage(DDPMSG_CLEARBREAKPOINT, (DWORD)&msg);

	GNRAISE(msg.error);
	}

//
//  Detach drive
//

DLLP1 Error DLLP2  DDP_DetachDrive(DVDDiskPlayer player, DWORD flags, DVDTime time, DWORD bufferSize)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Get AV File Info
//

DLLP1 Error DLLP2 DDP_GetAVFileInfo(DVDDiskPlayer player, HIter * it, AVFInfo * pInfo)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.pointer = it;
	msg.pointer2 = pInfo;

	ddpScheduler->DoMessage(DDPMSG_GETAVFILEINFO, (DWORD)&msg);
	GNRAISE(msg.error);
	}

//
//  Play file
//

DLLP1 Error DLLP2 DDP_PlayFile(DVDDiskPlayer player, HIter * it, DWORD flags)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.pointer = it;
	msg.dword = flags;

	ddpScheduler->DoMessage(DDPMSG_PLAYFILE, (DWORD)&msg);
	GNRAISE(msg.error);
	}

//
// Set audio stream selection preferences
//
DLLP1 Error DLLP2 DDP_SetAudioStreamSelectionPreferences(DVDDiskPlayer player, DWORD flags)
	{
	DVDPlayerMsg msg;

	msg.player = player;
	msg.dword = flags;

	ddpScheduler->DoMessage(DDPMSG_SETAUDIOSTREAMSELECTIONPREFS, (DWORD)&msg);

	GNRAISE(msg.error);
	}


////////////////////////////////////////////////////////////////////
//
//  DLL/Lib Initialization Functions
//
////////////////////////////////////////////////////////////////////

//
//  DDP Common Initialization Function
//

DLLCALL Error WINAPI DDP_Init(WinPortServer				* driverInstance,
										GenericProfile				* profile,
										DriveTable					* driveTable,
										DVDDiskPlayerFactory		* dvdVideoDiskPlayerFactory,
										DVDDiskPlayerFactory		* dvdAudioDiskPlayerFactory,
										VCDDiskPlayerFactory		* vcdDiskPlayerFactory,
										CDDADiskPlayerFactory	* cddaDiskPlayerFactory,
										AVFDiskPlayerFactory		* avfDiskPlayerFactory,
										FileSystemFactory			* dvdFileSystemFactory,
										FileSystemFactory			* vcdFileSystemFactory,
										FileSystemFactory			* cddaFileSystemFactory,
										FileSystemFactory			* avfFileSystemFactory,
										FileSystemFactory			* osFileSystemFactory)
	{
	DWORD	dd;
	Error err;

	//
	//  Copy factories and init other parameters passed
	//

	::dvdVideoDiskPlayerFactory = dvdVideoDiskPlayerFactory;
	::dvdAudioDiskPlayerFactory = dvdAudioDiskPlayerFactory;
	::vcdDiskPlayerFactory = vcdDiskPlayerFactory;
	::cddaDiskPlayerFactory	= cddaDiskPlayerFactory;
	::avfDiskPlayerFactory = avfDiskPlayerFactory;
	::dvdFileSystemFactory = dvdFileSystemFactory;
	::vcdFileSystemFactory = vcdFileSystemFactory;
	::cddaFileSystemFactory = cddaFileSystemFactory;
	::avfFileSystemFactory = avfFileSystemFactory;
	::osFileSystemFactory = osFileSystemFactory;

	DriverInstance = driverInstance;
	SystemProfile = profile;
	SystemDriveTable = driveTable;

	//
	//  Create region handler
	//

	RegionMan = new RegionHandler();
	if (!RegionMan)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	GNREASSERT(RegionMan->Init(profile));

	//
	//  Create file system
	//

	FileSystem = new RootFileSystem();
	if (!FileSystem)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	if (IS_ERROR(err = FileSystem->Init(profile, SystemDriveTable)))
		{
		DP("Could not initialize Root File System (Error %08lx)!", err);
		delete FileSystem;
		FileSystem = NULL;
		return err;
		}

	//
	// Create async. request scheduler
	//

	getDiskInfoAsyncRqScheduler = new GetDiskInfoAsyncRequestScheduler;

	GNREASSERT(VFI_Init(FileSystem));

	//
	//  Set up threads
	//


	DDPScheduleThread = CreateThread(NULL, 20000, DDPScheduleRoutine, NULL, CREATE_SUSPENDED, &dd);
	SetThreadPriority(DDPScheduleThread, THREAD_PRIORITY_HIGHEST);
	ResumeThread(DDPScheduleThread);


	GNRAISE_OK;
	}

//
//  DDP Common Cleanup Function
//

DLLCALL Error WINAPI DDP_Cleanup(void)
	{
	VFI_Cleanup();

	delete RegionMan;
	delete FileSystem;

	delete DriverInstance;

	delete SystemProfile;
	SystemDriveTable->Cleanup();
	delete SystemDriveTable;

	delete dvdVideoDiskPlayerFactory;
	delete dvdAudioDiskPlayerFactory;
	delete vcdDiskPlayerFactory;
	delete cddaDiskPlayerFactory;
	delete avfDiskPlayerFactory;
	delete dvdFileSystemFactory;
	delete vcdFileSystemFactory;
	delete cddaFileSystemFactory;
	delete avfFileSystemFactory,
	delete osFileSystemFactory;

	delete getDiskInfoAsyncRqScheduler;

	GNRAISE_OK;
	}
