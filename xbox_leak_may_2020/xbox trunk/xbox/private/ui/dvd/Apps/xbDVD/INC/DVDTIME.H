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

#ifndef DVDTIME_H
#define DVDTIME_H

#include "Library/Common/Prelude.h"
#include "Library/Common/GNErrors.h"
#include "Library/hardware/video/generic/vidtypes.h"
#include "Library/hardware/audio/generic/ac3setup.h"
#include "Driver/Dvdpldrv/Common/AVF/MPEGAudioTypes.h"
#include "NavErrors.h"


enum DVDPlayerMode
	{
	DPM_STOPPED,
	DPM_PAUSED,
	DPM_STILL,
	DPM_PLAYING,
	DPM_BUSY,
	DPM_SCANNING,
	DPM_INACTIVE,
	DPM_REVERSEPLAY,
	DPM_TRICKPLAY,
	DPM_REVERSESCAN
	};

enum DVDDomain
	{
	FP_DOM,
	VMGM_DOM,
	VTSM_DOM,
	TT_DOM,
	STOP_DOM,
	AMGM_DOM,
	TT_GR_DOM
	};

enum VTSMenuType
	{
	VMT_TITLE_MENU,
	VMT_ROOT_MENU,
	VMT_SUBPICTURE_MENU,
	VMT_AUDIO_MENU,
	VMT_ANGLE_MENU,
	VMT_PTT_MENU,
	VMT_NO_MENU		// Note that this is used in ExtendedPlayerState as array size
	};

enum DVDManagerMenuType
	{
	DMT_VMGM,
	DMT_AMGM,
	DMT_UNDEFINED
	};

enum DisplayPresentationMode
	{
	DPM_4BY3,
	DPM_16BY9,
	DPM_LETTERBOXED,
	DPM_PANSCAN,
	DPM_NONE
	};

enum DVDAudioCodingMode
	{
	DAM_AC3,
	DAM_MPEG1,
	DAM_MPEG2,
	DAM_LPCM,
	DAM_DTS,
	DAM_SDDS,
	DAM_NONE = 0xFF
	};

enum DVDAudioApplicationMode
	{
	DAAM_UNDEFINED		= 0,
	DAAM_KARAOKE		= 1,
	DAAM_SURROUND		= 2
	};

enum DVDKaraokeSoloDuetMode
	{
	DKM_SOLO				= 0,
	DKM_DUET				= 1,
	DKM_UNDEFINED		= 2
	};

enum MPEG2PrologicStatus		// Status of the decoder (actually played!)
	{
	MP2PS_OFF,
	MP2PS_ON,
	MP2PS_UNKNOWN
	};

enum MPEG2LFEStatus				// Content, not playing
	{
	MP2LFE_OFF,
	MP2LFE_ON,
	MP2LFE_UNKNOWN
	};

enum DVDDiskType
	{
	DDT_NONE,
	DDT_VIDEO_DVD,
	DDT_AUDIO_DVD,
	DDT_VIDEO_CD,
	DDT_AUDIO_CD,
	DDT_SUPER_AUDIO_CD,
	DDT_DTS_AUDIO_CD,
	DDT_SUPER_VIDEO_CD,
	DDT_MULTI_DISK			// Describes a CD-R or similar with files of different types on it
	};

enum RegionSource
	{
	RGSRC_UNDEFINED	= 0,
	RGSRC_DRIVE			= 1,
	RGSRC_BOARD			= 2
	};

enum VideoCompressionMode
	{
	VCM_UNKNOWN,
	VCM_MPEG1,
	VCM_MPEG2
	};

enum Line21Mode
	{
	L21M_NO_DATA,
	L21M_FIRST_FIELD,
	L21M_SECOND_FIELD,
	L21M_BOTH_FIELDS
	};

//
//  User Operation Bitmasks
//

#define UOP_TIME_PLAY_SEARCH				MKFLAG(0)
#define UOP_PTT_PLAY_SEARCH				MKFLAG(1)
#define UOP_TITLE_PLAY						MKFLAG(2)
#define UOP_STOP								MKFLAG(3)
#define UOP_GO_UP								MKFLAG(4)
#define UOP_TIME_PTT_SEARCH				MKFLAG(5)
#define UOP_PREV_TOP_PG_SEARCH			MKFLAG(6)
#define UOP_NEXT_PG_SEARCH					MKFLAG(7)
#define UOP_FORWARD_SCAN					MKFLAG(8)
#define UOP_BACKWARD_SCAN					MKFLAG(9)
#define UOP_SCAN_OFF							MKFLAG(25)	// additional
#define UOP_MENU_CALL_TITLE				MKFLAG(10)
#define UOP_MENU_CALL_ROOT					MKFLAG(11)
#define UOP_MENU_CALL_SUB_PICTURE		MKFLAG(12)
#define UOP_MENU_CALL_AUDIO				MKFLAG(13)
#define UOP_MENU_CALL_ANGLE				MKFLAG(14)
#define UOP_MENU_CALL_PTT					MKFLAG(15)
#define UOP_RESUME							MKFLAG(16)
#define UOP_BUTTON							MKFLAG(17)
#define UOP_STILL_OFF						MKFLAG(18)
#define UOP_PAUSE_ON							MKFLAG(19)
#define UOP_PAUSE_OFF						MKFLAG(26)	// additional
#define UOP_AUDIO_STREAM_CHANGE			MKFLAG(20)
#define UOP_SUB_PICTURE_STREAM_CHANGE	MKFLAG(21)
#define UOP_ANGLE_CHANGE					MKFLAG(22)
#define UOP_KARAOKE_MODE_CHANGE			MKFLAG(23)
#define UOP_VIDEO_MODE_CHANGE				MKFLAG(24)

//
//  Disc Information Structure
//

class __far DDPDiskInfo
	{
	public:
		DDPDiskInfo(void)			{size = sizeof(DDPDiskInfo);}

		DWORD				size;				// Size of the structure passed, DO NOT CHANGE!!!
		DVDDiskType		type;				// Type of media inserted
		char				uniqueKey[8];	// Unique identifier of disk
		BYTE				systemRegion;	// System region (decoder or drive, only for type == DDT_VIDEO_DVD)
		BYTE				diskRegion;		// Disk region (only for type == DDT_VIDEO_DVD)
		RegionSource	regionSource;	// Region source (decoder or drive, only for type == DDT_VIDEO_DVD)
		BYTE				availSets;		// Number of region sets left (only for type == DDT_VIDEO_DVD)
	};

//
//  Disc-Only Information Structure (no system region related info)
//

class DDPDiskOnlyInfo
	{
	public:
		DDPDiskOnlyInfo(void)			{size = sizeof(DDPDiskOnlyInfo);}

		DWORD				size;				// Size of the structure passed, DO NOT CHANGE!!!
		DWORD				dwUserData;		// User data, not touched by DDP layer
		DVDDiskType		type;				// Type of media inserted
		char				uniqueKey[8];	// Unique identifier of disk
		BYTE				diskRegion;		// Disk region (only for type == DDT_VIDEO_DVD)
		char				volumeName[256];
	};


class DDPOpenPlayerInfo
	{
	public:
		DDPOpenPlayerInfo(){size = sizeof(DDPOpenPlayerInfo);}

		DWORD				size;				// Size of the structure passed, DO NOT CHANGE!!!
		DWORD				dwUserData;		// User data, not touched by DDP layer
		void*				units;			// UnitSet currently open, cast to void*
		void**			pplayer;			// Pointer to DVDDiskPlayer (cast to void**)
		DDPDiskInfo		diskInfo;		// structure filled out with relevant disk data upon successful player opening
	};


//
//  DVD Time Class
//

class __far DVDTime
	{
	private:
		DWORD	stamp;  // Format HHHH HHHH MMMM MMMM SSSS SSSS RR FF FFFF
	public:
		DVDTime(DWORD stamp) {this->stamp = stamp;}
		DVDTime(void) {this->stamp = 0;}
		DVDTime(int hours, int minutes, int seconds, int frames, int frameRate);
		DVDTime(int millisecs, int divider, int frameRate);

		int FrameRate(void)	const {return XTBF(7, stamp) ? 30 : 25;}
		int Frames(void)		const {return (int)(XTBF(0, 4, stamp) + 10 * XTBF(4, 2, stamp));}
		int Seconds(void)		const {return (int)(XTBF(8, 4, stamp) + 10 * XTBF(12, 4, stamp));}
		int Minutes(void)		const {return (int)(XTBF(16, 4, stamp) + 10 * XTBF(20, 4, stamp));}
		int Hours(void)		const {return (int)(XTBF(24, 4, stamp) + 10 * XTBF(28, 4, stamp));}

		DWORD FrameTotal(void)	const	{return (Seconds() + Minutes() * 60 + Hours() * 3600) * FrameRate() + Frames();}

		DWORD Millisecs(void) const {return 1000 * Frames() / FrameRate() +
		                              1000 * Seconds() +
		                              60000 * Minutes() +
		                              3600000 * Hours();}

		friend DVDTime operator+ (const DVDTime u, const DVDTime v);
		friend DVDTime operator- (const DVDTime u, const DVDTime v);

		DVDTime & operator+= (const DVDTime u);
		DVDTime & operator-= (const DVDTime u);

		int Compare(const DVDTime u) const;

		friend BOOL operator==(const DVDTime u, const DVDTime v) {return u.Compare(v) == 0;}
		friend BOOL operator!=(const DVDTime u, const DVDTime v) {return u.Compare(v) != 0;}
		friend BOOL operator<(const DVDTime u, const DVDTime v)  {return u.Compare(v) < 0;}
		friend BOOL operator>(const DVDTime u, const DVDTime v)  {return u.Compare(v) > 0;}
		friend BOOL operator<=(const DVDTime u, const DVDTime v) {return u.Compare(v) <= 0;}
		friend BOOL operator>=(const DVDTime u, const DVDTime v) {return u.Compare(v) >= 0;}

		BOOL IsZero(void)		const {return (stamp & 0xffffff3f) == 0;}
		BOOL IsNotZero(void)	const {return (stamp & 0xffffff3f) != 0;}
	};

//
//  DVD Location
//

class __far DVDLocation
	{
	public:
		DVDDomain	domain;
		WORD			videoTitleSet;
		WORD			title;
		WORD			vtsTitle;
		DVDTime		titleTime;
		WORD			partOfTitle;
		WORD			programChain;
		DVDTime		pgcTime;
		WORD			program;
		WORD			cell;
		DVDTime		cellTime;

		DVDLocation(void) {};
		friend BOOL	NotEqual(const DVDLocation u, const DVDLocation v);
		friend BOOL operator !=(const DVDLocation u, const DVDLocation v) { return (BOOL)memcmp(&u, &v, sizeof(DVDLocation)); }
	};

//
//  Audio Stream Format
//

class __far DVDAudioStreamFormat
	{
	public:
		WORD							languageCode;
		WORD							languageExtension;
		DVDAudioCodingMode		codingMode;
		WORD							bitsPerSample;
		DWORD							samplesPerSecond;
		WORD							channels;
		DVDAudioApplicationMode	applicationMode;
		BYTE							channelAssignment;
		BYTE							mcIntro;
		DVDKaraokeSoloDuetMode	soloDuetMode;

		friend BOOL operator==(const DVDAudioStreamFormat u, const DVDAudioStreamFormat v);
		friend BOOL operator!=(const DVDAudioStreamFormat u, const DVDAudioStreamFormat v);
	};

//
//  Video Stream Format
//

class VideoStreamFormat
	{
	public:
		VideoCompressionMode			compressionMode;			// Compression technique used
		VideoStandard					videoStandard;				// Source video standard
		DisplayPresentationMode		sourceAspectRatio;		// Aspect ratio of source picture
		BOOL								panScanOn4By3;				// TRUE if Pan & Scan is allowed on 4 by 3 display
		BOOL								letterboxedOn4By3;		// TRUE if letterboxed is allowed on 4 by 3 display
		Line21Mode						line21Mode;					// Encoded line 21 data
		WORD								sourceWidth;				// Width of source picture
		WORD								sourceHeight;				// Height of source picture
		BOOL								sourceLetterboxed;		// TRUE is source material is letterboxed

		friend BOOL operator==(const VideoStreamFormat & u, const VideoStreamFormat & v);
		friend BOOL operator!=(const VideoStreamFormat & u, const VideoStreamFormat & v);
	};

//
//  Subpicture Stream Format
//

class __far DVDSubPictureStreamFormat
	{
	public:
		WORD						languageCode;
		WORD						languageExtension;

		friend BOOL operator==(const DVDSubPictureStreamFormat u, const DVDSubPictureStreamFormat v);
		friend BOOL operator!=(const DVDSubPictureStreamFormat u, const DVDSubPictureStreamFormat v);
	};

//
//  Button Information
//

#define DBI_MAX_BUTTON_NUMBER 103

class __far DVDButtonInformation
	{
	public:
		WORD	x;
		WORD	y;
		WORD	w;
		WORD	h;
		BOOL	autoAction;
		BYTE	upperButton;
		BYTE	lowerButton;
		BYTE	leftButton;
		BYTE	rightButton;

		friend BOOL operator==(const DVDButtonInformation u, const DVDButtonInformation v);
		friend BOOL operator!=(const DVDButtonInformation u, const DVDButtonInformation v);
	};

//
//  Extended player state
//

class __far ExtendedPlayerState
	{
	public:
		DWORD								size;								// Size of the structure passed

		// Request bitmasks

		DWORD								request;							// Bitmask for requested data
		DWORD								valid;							// Bitmask for delivered data
		DWORD								monitor;							// Bitmask indicating the elements to check for changes
		DWORD								changed;							// Bitmask indicating the elements that have changed

		// Player Information (EPS_PLAYERINFO, additions below)

		DVDPlayerMode					playerMode;						// Current player mode
		WORD								playbackSpeed;					// Current playback speed
		WORD								scanSpeed;						// Current scan speed
		DWORD								forbiddenUOPs;					// Forbidden UOPs

		// Hardware Information (EPS_MISC)

		DVDDiskType						diskType;						// Current disk type
		DisplayPresentationMode		displayMode;					// Current display mode
		VideoStandard					videoStandard;					// Current video standard
		DWORD								currentBitRate;				// Current bitrate

		// Audio Stream Information (EPS_AUDIOSTREAMS)

		int								currentAudioStream;			// Current Audio Stream ID
		BYTE								availableAudioStreams;		// Bitmask of audio streams available
		DVDAudioStreamFormat			audioStream[8];				// Audio stream data

		// Sub Picture Stream Information (EPS_SUBPICTURESTREAMS)

		int								currentSubPictureStream;	// Current sub picture Stream ID
		DWORD								availableSubPictureStreams;// Bitmask of sub picture streams available
		DVDSubPictureStreamFormat	subPictureStream[32];		// Subpicture stream data
		BOOL								subPictureEnable;				// Subpicture enabled

		// Title Information (EPS_TITLE)

		DVDLocation						location;						// Current location
		DVDLocation						duration;						// Current duration

		// Menu Information (EPS_MENU)

		WORD								menuLanguage;
		BOOL								menuAvailable[VMT_NO_MENU+1];  // Use a VTSMenuType as index

		// Parental Information (EPS_PARENTAL)

		WORD								parentalCountry;
		WORD								parentalLevel;

		// Angle Information (EPS_ANGLE, addition below)

		WORD								numberOfAngles;
		WORD								currentAngle;

		// Button Information (EPS_BUTTON)

		WORD								numberOfButtons;
		WORD								selectedButton;
		WORD								forcedlyActivatedButton;
		WORD								userButtonOffset;
		WORD								numberOfUserButtons;
		DVDButtonInformation			buttonInfo[DBI_MAX_BUTTON_NUMBER];

		// CDDA Information (EPS_PLAYINGGAP)

		BOOL								playingGap;						// CDDA only: TRUE is gap between titles is played

		// Additional Player Information (EPS_PLAYERINFO)

		BOOL								playingForward;

		// Additional Angle Information (EPS_ANGLE)

		BOOL								isMultiAngleScene;
		WORD								playingAngle;

		// Video stream information (EPS_VIDEOSTREAM, additional to info in EPS_MISC)

		VideoStreamFormat				videoStream;

		// Hardware Information (EPS_MISC, additional)

		BOOL								diskIsEncrypted;				// TRUE if disk is encrypted

		// Additional Audio Information (EPS_AUDIOSTREAMS, additional)

		MPEG2PrologicStatus			mpeg2PrologicStatus;			// Indicating if Prologic decoding is currently done
		MPEG2LFEStatus					mpeg2LFEStatus;				// Indicates if LFE info is in content
		AC3AudioCodingMode			ac3AudioCodingMode;			// Encoding, not actual output

		// (S-)Video-CD specific information (EPS_VCDPLAYERINFO)

		BOOL								isPlaybackControlOn;
	};

//
//  Request bitmask defines (00001000 in use)
//

#define EPS_PLAYERINFO				0x00000001
#define EPS_MISC						0x00000002
#define EPS_TITLE						0x00000004
#define EPS_AUDIOSTREAMS			0x00000008
#define EPS_SUBPICTURESTREAMS		0x00000010
#define EPS_ANGLE						0x00000020
#define EPS_MENU						0x00000040
#define EPS_PARENTAL					0x00000080
#define EPS_BUTTON					0x00000100
#define EPS_PLAYINGGAP				0x00000200
#define EPS_VIDEOSTREAM				0x00000400
#define EPS_KARAOKE					0x00000800
#define EPS_VCDPLAYERINFO			0x00001000


//
//  Shortcuts
//

#define EPS_COMMON					EPS_PLAYERINFO | EPS_MISC | EPS_TITLE
#define EPS_DVD						EPS_MENU | EPS_PARENTAL | EPS_BUTTON | EPS_ANGLE | EPS_AUDIOSTREAMS | \
											EPS_SUBPICTURESTREAMS | EPS_VIDEOSTREAM
#define EPS_VCD						EPS_BUTTON | EPS_AUDIOSTREAMS
#define EPS_CDA						EPS_PLAYINGGAP | EPS_AUDIOSTREAMS
#define EPS_ALL						EPS_COMMON | EPS_DVD | EPS_VCD | EPS_CDA

//
//  Async request processing
//

typedef enum ARQReqestID
	{
	ARQ_NONE = 0,
	ARQ_GET_DISK_INFO,
	ARQ_OPEN_PLAYER
	};

//
//  AsyncRequestDoneCallback - called e.g. when DDP_GetDiskInfoAsync is actually completed
//

typedef void (WINAPI * ARQAsyncRequestDoneCB)(ARQReqestID doneRqID, DWORD dwRqHandle, void * userData, Error err);

typedef void (WINAPI * DNEEventHandler)(DWORD event, void * userData, DWORD info);

//
//  Disk Navigation Event handling
//

typedef void (WINAPI * DNEEventHandler)(DWORD event, void * userData, DWORD info);

#define DNE_NONE								0
#define DNE_TITLE_CHANGE					1
#define DNE_PART_OF_TITLE_CHANGE			2
#define DNE_VALID_UOP_CHANGE				3
#define DNE_ANGLE_CHANGE					4
#define DNE_AUDIO_STREAM_CHANGE			5
#define DNE_SUBPICTURE_STREAM_CHANGE	6
#define DNE_DOMAIN_CHANGE					7
#define DNE_PARENTAL_LEVEL_CHANGE		8
#define DNE_BITRATE_CHANGE					9
#define DNE_STILL_ON							10
#define DNE_STILL_OFF						11
#define DNE_PLAYBACK_MODE_CHANGE			12
#define DNE_CDA_PLAYING_GAP				13
#define DNE_READ_ERROR						14
#define DNE_DISPLAY_MODE_CHANGE			15
#define DNE_STREAMS_CHANGE					16
#define DNE_SCAN_SPEED_CHANGE				17
#define DNE_PLAYBACK_SPEED_CHANGE		18
#define DNE_VIDEO_STANDARD_CHANGE		19
#define DNE_BREAKPOINT_REACHED			20
#define DNE_DRIVE_DETACHED					21		// OBSOLETE
#define DNE_ERROR								22

#define DNE_EVENT_NUMBER					23

//
//  ERS Breakpoints
//

#define ERS_BREAKPOINT_NONE				0xffffffff	// Defines an invalid breakpoint id

//
//  Flags for DDP_SetBreakpoint()
//

enum ERSBreakpointFlags
	{
	ERS_NONE					=	0x0000,	// No flags, normal breakpoint
	ERS_PAUSE				=	0x0001,	// Go to still mode when reaching breakpoint
	ERS_AUTOCLEAR			=	0x0002,	// Delete breakpoint when it has been reached
	ERS_PAUSEATEND			=	0x0004	// Trigger at end of title
	};

//
//  Flags for DDP_SetBreakpointExt()
//

enum ERSBreakpointExtFlags
	{
	ERS_ATTIME			=	0x0001,	// Trigger when reaching time in title
	ERS_ENDOFTITLE		=	0x0002,	// Trigger at end of title
	ERS_ENDOFPTT		=	0x0004,	// Trigger at and of part of title
	ERS_FORWARD			=	0x0008,	// Trigger when playing forward
	ERS_BACKWARD		=	0x0010,	// Trigger when playing backward
	ERS_TOSTILL			=	0x0020,	// Go to still when reaching breakpoint
	ERS_CLEAR			=	0x0040,	// Automatically delete breakpoint after triggering
	ERS_BEGINOFPTT		=	0x0080	// Trigger at begin of PTT
	};

//
//  Flags for ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time);
//

#define DDPEPF_NONE					0
#define DDPEPF_PAUSE_AT_START		1
#define DDPEPF_USE_TIME				2
#define DDPEPF_USE_PTT				4
#define DDPEPF_USE_TITLE			8

//
//  Flags for DetachDrive(DWORD flags, DVDTime time);
//

#define DDPDDF_DETACH_IMMEDIATE				1
#define DDPDDF_DETACH_AT_TIME					2
#define DDPDDF_DETACH_AT_END_OF_CELL		4
#define DDPDDF_DETACH_AT_END_OF_PROGRAM	8
#define DDPDDF_DETACH_AT_END_OF_PGC			16

#define DDPDDF_DETACH_ANY						31

#define DDPDDF_STREAM_PREFETCH				32
#define DDPDDF_PREFETCH_BUFFER_SIZE			64

//
//  Flags for DDP_StartPresentationExt and DDP_DefrostExt
//

#define DDPSPF_NONE		0
#define DDPSPF_TOPAUSE	1
#define DDPSPF_ID_MATCH	2

//
//  Flags for DDP_PlayFile
//

#define DDPPFF_NONE				0
#define DDPPFF_PAUSE_AT_START	1

//
// Flags for DDP_SetAudioStreamSelectionPreferences
//
#define DDPASSP_NONE				0
#define DDPASSP_MAXCHANNELS	1
#define DDPASSP_NO_DTS			2
#define DDPASSP_DTS				4

//
//  AVF definitions
//

// Structure for AVFStreamServer::GetFileInfo()

class AVFInfo
	{
	public:
		DWORD								size;								// Size of the structure passed

		// Request bitmasks

		DWORD								request;							// Bitmask for requested data
		DWORD								valid;							// Bitmask for delivered data
		DWORD								monitor;							// Bitmask indicating the elements to check for changes
		DWORD								changed;							// Bitmask indicating the elements that have changed

		DVDLocation						duration;						// obviates need for ExtendedPlayerState call

		// MPEG Audio Characteristics (the following items will always be available for MPEG Audio)
		MPEGAudioFrameInfo			mpegAudio;

		// MPEG Audio ID3v1 information (optional)
		MPEGAudioID3V1Tag				id3;
	};

//
//  Request bitmask defines
//

#define AVFINFO_MPEG_AUDIO_FRAME			0x00000001
#define AVFINFO_MPEG_AUDIO_ID3			0x00000002
#define AVFINFO_VERIFY_FILENAME_ONLY	0x00000004

//
//  Freeze state info
//

class DDPFreezeState
	{
	public:
		DVDDiskType diskType;
		BYTE			uniqueKey[8];
		WORD			parentalLevel;
		WORD			parentalCountry;
	};

#endif
