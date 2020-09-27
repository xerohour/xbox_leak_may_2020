//depot/xbox-aug01-final/private/ui/dvd/driver/softwarecinemaster/cinemast/Cinmst32.cpp#3 - edit change 19203 (text)
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


#define VIONA_VERSION			1
#undef ONLY_EXTERNAL_VISIBLE

#include "library\common\prelude.h"
#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\hooks.h"
#include "library\common\profiles.h"
#include "library\common\vddebug.h"
#include "library\common\winports.h"



#define ONLY_EXTERNAL_VISIBLE	1

#include "library\hardware\videodma\generic\pip.h"
#include "library\hardware\mpeg2dec\generic\mpeg2dec.h"
#include "library\hardware\video\generic\videnc.h"
#include "library\common\virtunit.h"

#include "..\MPEGDecoder\Mp2HuffTables.h"
#include "..\MPEGDecoder\Huffdec.h"
#include "..\MPEGDecoder\SliceParser.h"
#include "..\MPEGDecoder\FastMMXSliceDecoder.h"
#include "..\MPEGDecoder\QFastISSEMotionComp.h"
#include "..\Video\XBoxDisplay.h"
#include "..\DVDStreamDecoder.h"
#include "..\decryptionlib\decryptionlib.h"
#include "..\common\TimedFibers.h"
//#include "..\common\CPUDetection.h"
#include <math.h>
#include <stdio.h>
#include "..\common\PerformanceMonitor.h"
#include "..\Video\MacrovisionEncoder.h"
#include "..\Audio\Ac97PlaybackDevice.h"

#include "..\ClosedCaption\ClosedCaptionDecoder.h"

#include "CinemLib.h"

#if _DEBUG
#include <crtdbg.h>
#endif


//
//  Region Codes
//

static const int RegionCodes[] =
	{
	0x43241234,
	0x24534251,
	0x23423142,
	0x35235353,

	0x43271234,
	0x24574251,
	0x23473142,
	0x35275353
	};


// have include this low to pick up region codes (this forces the code into the .h as well)
#include "XboxProfile.h"



__declspec( dllexport) void * thk_ThunkData32;

#define PCICB_INVERT		(1 << 0)
#define PCICB_AND			(1 << 1)
#define PCICB_OR			(1 << 2)
#define PCICB_ID			(1 << 3)

struct PCICheckBlock
	{
	public:
		DWORD		com;
		WORD		p0, p1, p2, p3;
	};

#define WINDOWS_MAJOR_VERSION ((DWORD)(LOBYTE(LOWORD(::GetVersion()))))

#pragma warning(disable : 4786)
////////////////////////////////////////////////////////////////////
//
//  Unit Set Class
//
////////////////////////////////////////////////////////////////////
typedef class UnitSetClass : public DVDStreamDecoder, public ASyncErrorMessenger
	{
	public:
      GenericDirectXDisplay *			directXDisplay;
		GenericPictureDisplay *			display;
		SPUDisplay						spuDisplay;
		GenericProfile *				profile, * globalProfile;

		GenericSliceParser *			highQualitySliceDecoder;
		GenericSliceParser *			lowQualitySliceDecoder;
		GenericAudioPlaybackDevice *	audioPlaybackDevice;
		AuthenticatingDecryptor	*		decryptor;
		MacrovisionEncoder *			macrocoder;
		ClosedCaptionDecoder *			ccDecoder;
		StreamFlipCopy *				streamFlipCopy;

		int								referenceCount;
		DWORD							colorControl;

		WORD							buttonLeft, buttonTop, buttonWidth, buttonHeight;
		DWORD							buttonSelect, buttonActive;
		SPUButtonState					buttonState;

		GenericMsgPort *				msgPort;
		HWND							msgWindow;
		WORD							refillMsg, signalMsg, doneMsg, errorMsg;

		WORD							timeoutMsg;

		HWND							hwnd;
		DWORD							positionScale;
		DWORD							signalPosition;

		BOOL							refillMsgPending;
		BOOL							mpeg2, stillFrameSequence;

		BOOL							active, seeking, paused, stopped, scanning, trickplay;
		BOOL							pipEnabled;
		BOOL							spuEnabled;
		BOOL							dvdDemux;
		BOOL							ccEnabled;

		BYTE							videoStreamID, dtsStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID;
		MPEG2AudioType					audioType;
		BOOL							audioAC3, audioLPCM;
		MPEGStreamType					streamType;
		MPEG2SplitStreamType			splitStreamType[4];
		MPEG2PresentationMode			presentationMode;

		AC3OperationalMode				ac3OperationalMode;
		WORD							ac3HighScaleFactor;
		WORD							ac3LowScaleFactor;
		AC3DynamicRange					ac3DynamicRange;
		AC3DialogNorm					ac3DialogNorm;
		BOOL							enableLFE;

		AC3KaraokeConfig				karaokeCfg;
		AC3DualModeConfig				dualModeCfg;
		AC3SpeakerConfig				spkCfg;

		WORD							bitsPerSample;
		WORD							channels;
		WORD							samplesPerSecond;

		WORD							brightness;
		WORD							redBrightness, greenBrightness, blueBrightness;
		WORD							contrast, saturation;
		BOOL							colorEnable;
		DWORD							colorKeyColor;

		WORD							cropLeft, cropTop, cropRight, cropBottom;

		DWORD							firstStreamSegment;
		DWORD							lastStreamSegment;

		int								copyProtectionLevel;
		VideoCopyMode                   eCGMSMode;
		    
		SPDIFCopyMode					spdifCopyMode;
		bool							timedFibersNormalSpeedPriority;

		int								d3AudioEnabled;		// C++ is not three dimensional


		UnitSetClass(GenericSliceParser * highQualitySliceDecoder,
						 GenericSliceParser * lowQualitySliceDecoder,
						 GenericAudioPlaybackDevice * audioPlaybackDevice,
						 GenericDirectXDisplay	*	display,
				 		 MacrovisionEncoder	*	macrocoder,
						 GenericProfile * profile,
						 GenericProfile * globalProfile,
						 ClosedCaptionDecoder * ccDecoder,
						 HINSTANCE hinst,
						 StreamFlipCopy * streamFlipCopy);


		~UnitSetClass();

		Error Initialize(void);

		Error ActivateUnits();
		Error PassivateUnits();
		Error ConfigureUnits(TAG __far * tags);

		void PostErrorMessage(Error err);
		void RefillRequest(void);
		void SignalReached(__int64 signalPos);
		void StreamCompleted(DVDElementaryStreamType type);

		void UpdateBrightness(void);
		void UpdateContrast(void);
		void UpdateSaturation(void);

		Error CheckHardwareResources(void)
			{
			GNREASSERT(audioPlaybackDevice->CheckHardwareResources());
			GNREASSERT(display->CheckHardwareResources());
			GNRAISE_OK;
			}

		Error DoMPEGCommand(MPEGCommand com, long param);

	} * UnitSet;
#pragma warning(default : 4786)

#pragma data_seg(".sdata")
int globalOpenCount = 0;
#pragma data_seg()

static BOOL allowCSS	= TRUE;
static BOOL allowCSSMsg = FALSE;
static BoardVersionInfo		boardVersionInfo;
#define allowMultiInstance 0

static bool primaryUnit = TRUE;

////////////////////////////////////////////////////////////////////
//
//   Board Class
//
////////////////////////////////////////////////////////////////////

class Board
	{
	public:
		HINSTANCE						hinst;
		GenericProfile				*	profile, * globalProfile;
		HANDLE							cinemSupDriver;
		int								regionCount;
		BYTE								regionCode;

	   Board(HINSTANCE hinst);
	   ~Board(void);

		Error BeginInitialize(void);
		Error CompleteInitialize(void);
		Error Initialize(void);
		Error Reconfigure(void);
		Error Destroy(void);
		BOOL CheckRegionCodeValid(BYTE regionSet);
		UnitSet CreateUnitSet(void);
		};

//
//  Create Board
//

Board * CreateBoard(HINSTANCE hinst)
	{
	return new Board(hinst);
	}

//
//  VDR Interface Declaration
//

typedef class __far VDRHandleStruct {} * VDRHandle;

#ifndef DLLCALL
#define DLLCALL __declspec(dllexport)
#endif

extern "C" {

DLLCALL Error WINAPI VDR_OpenDriver(TCHAR __far * name, DWORD boardID, VDRHandle __far & handle);

DLLCALL Error WINAPI VDR_CloseDriver(VDRHandle handle);

DLLCALL DWORD WINAPI VDR_AvailUnits(VDRHandle handle);

DLLCALL Error WINAPI VDR_OpenUnits(VDRHandle handle, DWORD requnits, UnitSet __far & units);

DLLCALL Error WINAPI VDR_OpenSubUnits(UnitSet parent, DWORD requnits, UnitSet __far &units);

DLLCALL Error WINAPI VDR_CloseUnits(UnitSet units);

DLLCALL Error WINAPI VDR_ReconfigureDriver(VDRHandle handle);


DLLCALL Error WINAPI VDR_ConfigureUnits(UnitSet units, TAG __far * tags);

inline Error __cdecl VDR_ConfigureUnitsTags(UnitSet units, TAG tags, ...) {return VDR_ConfigureUnits(units, &tags);}

DLLCALL Error WINAPI VDR_T_ConfigureUnitsSet(UnitSet units, TAG __far * tags);

DLLCALL Error WINAPI VDR_T_ConfigureUnitsGet(UnitSet units, DWORD id, void * ref);

DLLCALL Error WINAPI VDR_LockUnits(UnitSet units);

DLLCALL Error WINAPI VDR_UnlockUnits(UnitSet units);


DLLCALL Error WINAPI VDR_ActivateUnits(UnitSet units);

DLLCALL Error WINAPI VDR_PassivateUnits(UnitSet units);

DLLCALL Error WINAPI VDR_EnablePIP(UnitSet units, BOOL enable);

DLLCALL Error WINAPI VDR_UpdatePIP(UnitSet units);

DLLCALL Error WINAPI VDR_GrabFrame(UnitSet units, FPTR base,
                            WORD width, WORD height,
                            WORD stride,
                            GrabFormat fmt);

DLLCALL DWORD WINAPI VDR_SendMPEGData(UnitSet units, HPTR data, DWORD size);

DLLCALL DWORD WINAPI VDR_SendMPEGDataMultiple(UnitSet units, MPEGDataSizePair * data, DWORD size);

DLLCALL DWORD WINAPI VDR_SendMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType, HPTR data, DWORD size, int timeStamp);

DLLCALL DWORD WINAPI VDR_SendMPEGDataSplitMultiple(UnitSet units, MPEGElementaryStreamType streamType, MPEGDataSizePair * data, DWORD size, int timeStamp);

DLLCALL void WINAPI VDR_CompleteMPEGData(UnitSet units);

DLLCALL void WINAPI VDR_CompleteMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType);

DLLCALL void WINAPI VDR_RestartMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType);

DLLCALL Error WINAPI VDR_SendMPEGCommand(UnitSet units, MPEGCommand com, long param, DWORD __far &tag);

DLLCALL Error WINAPI VDR_DoMPEGCommand(UnitSet units, MPEGCommand com, long param);

DLLCALL Error WINAPI VDR_CompleteMPEGCommand(UnitSet units, DWORD tag);

DLLCALL BOOL WINAPI VDR_MPEGCommandPending(UnitSet units, DWORD tag);

DLLCALL DWORD WINAPI VDR_CurrentMPEGLocation(UnitSet units);

DLLCALL DWORD WINAPI VDR_CurrentMPEGTransferLocation(UnitSet units);

DLLCALL MPEGState WINAPI VDR_CurrentMPEGState(UnitSet units);

DLLCALL Error WINAPI VDR_InstallMPEGWinHooks(UnitSet units, HWND hwnd, WORD refillMsg, WORD signalMsg, WORD doneMsg);

DLLCALL Error WINAPI VDR_RemoveMPEGWinHooks(UnitSet units);

DLLCALL Error WINAPI VDR_InstallMessagePort(UnitSet units, GenericMsgPort * port);

DLLCALL Error WINAPI VDR_RemoveMessagePort(UnitSet units);

DLLCALL Error WINAPI VDR_CompleteMPEGRefillMessage(UnitSet units);

DLLCALL Error WINAPI VDR_DoAuthenticationCommand(UnitSet units, MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);

DLLCALL BOOL WINAPI VDR_CheckRegionCodeValid(UnitSet units, BYTE regionSet);

DLLCALL void WINAPI VDR_ReadPerformanceMonitor(PerformanceMonitor & pmon);

	}


Board 		*	board;
HINSTANCE		DriverInstance;
int				OpenCount;

//
//  Board Class
//

Board::Board(HINSTANCE hinst)
	{
	this->hinst = hinst;

	regionCode = 0;
	}

Board::~Board(void)
	{
	}

Error Board::Destroy(void)
	{
	FinishTimedFiberScheduler();

	if (globalProfile != profile) delete globalProfile;
	delete profile;

	GNRAISE_OK;
	}

Error Board::Reconfigure(void)
	{
	GNRAISE_OK;
	}


#define PROFILENAME	"Software CineMaster"

Error Board::BeginInitialize(void)
	{
	PMMSG("BeginInitialize+\n");



//		profile = new NullProfile();
		profile = new XboxProfile();

		globalProfile = profile;


	PMMSG("BI3\n");

	InitializeTimedFiberScheduler(globalProfile, profile);

	PMMSG("BeginInitialize-\n");

	GNRAISE_OK;
	}

Error Board::CompleteInitialize(void)
	{
	GNRAISE_OK;
	}

Error Board::Initialize(void)
	{
	GNRAISE_OK;
	}

////////////////////////////////////////////////////////////////////
//
//  Unit Set Class
//
////////////////////////////////////////////////////////////////////

//#define NUM_PCM_BUFFERS		4
#define NUM_PCM_BUFFERS		24




//
//  Create Unit Set (Direct X)
//

UnitSet Board::CreateUnitSet(void)
	{
	int videoDecoderPerformanceClass;
	BOOL hardwareOptimization;
//	GenericDirectXDisplay	*	display;
	XBoxDisplay	*	display;
	UnitSet							units;
	int								pcmBufferSize, numPCMBuffers;

	GenericAudioPlaybackDevice * audioPlaybackDevice;
	MacrovisionEncoder	*	macrocoder;
	ClosedCaptionDecoder	*	ccDecoder;
	StreamFlipCopy * streamFlipCopy;

	PMMSG("CreateUnitSet+\n");

//	CPUInformation	cpuInfo;
//	cpuInfo.ProfileCPU();
//	cpuInfo.PerformanceLevel();

//	bool spdifDataEnabled;
//	CPUType cpuType = DetectCPU();

	//PMMSGX("Detected %s\n", CPUName[cpuType]);

	profile->Read("VideoDecoder", "PerformanceClass", videoDecoderPerformanceClass, 0xffffffff);
	profile->Read("VideoDecoder", "HardwareOptimization", hardwareOptimization, 0);


	macrocoder = new WindowsMacrovisionEncoder();

	if (!macrocoder->Initialize())
		{
		delete macrocoder;
		macrocoder = NULL;
		}


	//
	//  Set up audio decoder
	//

	globalProfile->Read("AudioDecoder", "AudioPCMBufferSize", pcmBufferSize, 4096);
	globalProfile->Write("AudioDecoder", "AudioPCMBufferSize", pcmBufferSize);
	numPCMBuffers = pcmBufferSize * 48 / 8192;
	if (numPCMBuffers < 4) numPCMBuffers = 4;

	audioPlaybackDevice = new Ac97PlaybackDevice(globalProfile, profile, numPCMBuffers, 8192);

	if (!primaryUnit && videoDecoderPerformanceClass > 2) videoDecoderPerformanceClass = 2;

	videoDecoderPerformanceClass = 2;

	streamFlipCopy = new XMMXStreamFlipCopy();

	switch (videoDecoderPerformanceClass)
		{

		default:
		case 2: // Eight Bits
			{
			GenericSliceParser * dec = NULL;

			display = new XBoxDisplay(hinst, profile, globalProfile);

			PMMSG("Using ISSE optimized MotionComp version\n");
			PMMSG("Using XMMX optimized IDCT version\n");
			PMMSG("Using XMMX optimized YUV-Converter\n");
			dec = new QFastMMXSliceDecoder(NULL, new FastISSEMotionComp, new GenericXMMXMacroBlockIDCTDecoder, new XMMXYUVVideoConverter);

			ccDecoder = new ClosedCaptionDecoder((GenericDirectXDisplay *)display);

			units = new UnitSetClass(dec, dec,
											audioPlaybackDevice,
											(GenericDirectXDisplay *)display,
											macrocoder,
											profile, globalProfile,
											ccDecoder,
											hinst, streamFlipCopy);
			}
			break;

		}

	units->timedFibersNormalSpeedPriority = TRUE;
	units->SetLine21Decoder(ccDecoder);

	if (units) units->SetErrorMessenger(units);

	PMMSG("CreateUnitSet-\n");

	return units;
	}

////////////////////////////////////////////////////////////////////
//
//  Unit Set Class
//
////////////////////////////////////////////////////////////////////

//
//  Initialize
//

Error UnitSetClass::Initialize(void)
	{
	referenceCount = 1;

	active = FALSE;
	msgWindow = NULL;
	msgPort = NULL;
	hwnd = NULL;
	positionScale = MP2SR_SCALE_BYTES;
	refillMsgPending = FALSE;

#if AC3DISABLED
	audioAC3  = FALSE;
	audioType = MP2AUDTYP_MPEG;
#else
	audioAC3  = TRUE;
	audioType = MP2AUDTYP_AC3;
#endif

	timeoutMsg = 0xffff;

	audioLPCM = FALSE;
	videoStreamID = 0xe0;
	audioStreamID = 0xc0;
	ac3StreamID = 0x80;
	dtsStreamID = 0x88;
	lpcmStreamID = 0xa0;
	spuStreamID = 0x20;
	samplesPerSecond = 48000;
	bitsPerSample = 16;
	channels = 2;
	dualModeCfg = AC3DMDM_DEFAULT;
	karaokeCfg = AC3KARA_AWARE;
	spkCfg = AC3SC_20_SURROUND_COMPATIBLE;

	ac3Decoder.GetDolbyTestMode(ac3OperationalMode, ac3HighScaleFactor, ac3LowScaleFactor, ac3DialogNorm);
	ac3Decoder.GetAC3Config(ac3DynamicRange, enableLFE);

	mpeg2 = FALSE;
	stillFrameSequence = FALSE;
	pipEnabled = FALSE;
	ccEnabled = FALSE;

	streamType = multiplexed;
	splitStreamType[0] = MP2SST_PROGRAM;
	splitStreamType[1] = MP2SST_PROGRAM;
	splitStreamType[2] = MP2SST_PROGRAM;
	splitStreamType[3] = MP2SST_PROGRAM;

	brightness = 5000;
	redBrightness = 5000;
	greenBrightness = 5000;
	blueBrightness = 5000;
	contrast = 5000;
	saturation = 5000;
	colorEnable = TRUE;
	colorKeyColor = 0x0c000c;
	colorControl = highQualitySliceDecoder->QueryColorControl() &
						lowQualitySliceDecoder->QueryColorControl();

	UpdateBrightness();
	UpdateContrast();
	UpdateSaturation();

	spuEnabled = TRUE;

	cropLeft = cropTop = cropRight = cropBottom = 0;

	//
	// Setup Karaoke/Dual mode
	//
	int i;
	globalProfile->Read("AudioDecoder", "dualModeConfig", i, dualModeCfg);
	dualModeCfg = (AC3DualModeConfig) i;
	globalProfile->Read("AudioDecoder", "karaokeConfig", i, karaokeCfg);
	karaokeCfg = (AC3KaraokeConfig) i;
	SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);

	spdifCopyMode = SPDIFCM_NO_COPIES;


	if (allowMultiInstance && !primaryUnit)
		GNRAISE_OK;
	else
		return CheckHardwareResources();
	}

#pragma warning(disable : 4786)

//
//  Constructor
//

UnitSetClass::UnitSetClass(GenericSliceParser * highQualitySliceDecoder,
									GenericSliceParser * lowQualitySliceDecoder,
						 GenericAudioPlaybackDevice * audioPlaybackDevice,
						 GenericDirectXDisplay * display,
				 		 MacrovisionEncoder	*	macrocoder,
						 GenericProfile * profile,
						 GenericProfile * globalProfile,
						 ClosedCaptionDecoder * ccDecoder,
						 HINSTANCE hinst, StreamFlipCopy * streamFlipCopy)
						 : spuDisplay(display, highQualitySliceDecoder, lowQualitySliceDecoder),
						 DVDStreamDecoder(display, highQualitySliceDecoder,lowQualitySliceDecoder, audioPlaybackDevice, &spuDisplay, globalProfile, profile, ccDecoder, streamFlipCopy, decryptor = (allowCSS ? CreateDecryptor() : NULL))
	{
	this->highQualitySliceDecoder = highQualitySliceDecoder;
	this->lowQualitySliceDecoder = lowQualitySliceDecoder;
	this->audioPlaybackDevice = audioPlaybackDevice;
	this->display = display;
	this->directXDisplay = display;
	this->profile = profile;
	this->globalProfile = globalProfile;
	this->macrocoder = macrocoder;
	this->ccDecoder = ccDecoder;
	this->streamFlipCopy = streamFlipCopy;

	if (macrocoder) display->SetMacrovisionEncoder(macrocoder);

	dvdDemux = false;

	}

#pragma warning(default : 4786)

//
//  Destructor
//

UnitSetClass::~UnitSetClass()
	{
	ShutDownSupervisor();

	if (display && !(display->IsDecoder()))
		{
		if (highQualitySliceDecoder && highQualitySliceDecoder != lowQualitySliceDecoder)
			delete highQualitySliceDecoder;
		if (lowQualitySliceDecoder)
			delete lowQualitySliceDecoder;
		}
	if (streamFlipCopy) delete streamFlipCopy;
	if (audioPlaybackDevice) delete audioPlaybackDevice;
	if (display) delete display;
	if (macrocoder) delete macrocoder;
	if (decryptor) delete decryptor;
	if (ccDecoder) delete ccDecoder;
	}

//
//  Refill Request
//

void UnitSetClass::PostErrorMessage(Error err)
	{
	}

//
//  Refill Request
//

void UnitSetClass::RefillRequest(void)
	{
	if (!refillMsgPending)
		{
		refillMsgPending = TRUE;
		if (msgPort)
			msgPort->SendMessage(refillMsg, 0);
		}
	}

//
//  Signal Reached
//

void UnitSetClass::SignalReached(__int64 signalPos)
	{
	if (msgPort)
		msgPort->SendMessage(signalMsg, (int)(signalPos  * positionScale / MP2SR_SCALE_BYTES));
	}

//
//  Stream Completed
//

void UnitSetClass::StreamCompleted(DVDElementaryStreamType type)
	{
	if (active && !seeking && /*!paused &&*/ !stopped)
		{
		if (msgPort)
			msgPort->SendMessage(doneMsg, type);

		}
	}

//
//  Avtivate Units
//

UnitSetClass * cus;

Error UnitSetClass::ActivateUnits()
	{
	cus = this;

	if (!active)
		{
		SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID);
		SetAudioCodingMode(audioType,
								 samplesPerSecond == 30464 ? 96000 : samplesPerSecond,
								 bitsPerSample,
								 channels);

		active = TRUE;
		seeking = FALSE;
		paused = FALSE;
		stopped = TRUE;
		scanning = FALSE;
		trickplay = FALSE;
		refillMsgPending = FALSE;

		if (directXDisplay) directXDisplay->EnableDisplay(pipEnabled != 0);

		BeginStreaming(VDPM_PLAY_FORWARD, !mpeg2 || stillFrameSequence, 0);
		}

	GNRAISE_OK;
	}

//
//  Passivate Units
//

Error UnitSetClass::PassivateUnits()
	{
	cus = NULL;

	if (active)
		{
		if (!seeking)
			{
			if (!paused && !stopped)
				{
				StopStreaming();
				}
			EndStreaming(TRUE);
			}

		if (directXDisplay) directXDisplay->EnableDisplay(FALSE);
		if (display) display->HideDisplay();
		active = FALSE;
		}

	GNRAISE_OK;
	}

//
//  Process MPEG command
//

Error UnitSetClass::DoMPEGCommand(MPEGCommand com, long param)
	{
	switch (com)
		{
		case mpc_play:
			if (!seeking)
				{
				if (stopped)
					Sleep(100);
				else if (paused)
					Sleep(50);
				else
					StopStreaming();

//				param = 100;
				//				if (param != 1000) param = 1500;

//chzhack tmp fix for playback speeds 1.0 - 2.0
				if (param > 1000)
					SetTimedFiberPriorities(FALSE);
				else
					SetTimedFiberPriorities(timedFibersNormalSpeedPriority);

				paused = FALSE;
				stopped = FALSE;

				StartStreaming(ScaleLong(param, 1000, 0x10000));
				}
			break;
		case mpc_cue:
		case mpc_seek:
			if (!seeking)
				{
				seeking = TRUE;
				scanning = FALSE;
				trickplay = FALSE;
				if (!paused && !stopped)
					{
					StopStreaming();
					}
				EndStreaming(FALSE);
				}
			break;
		case mpc_stepkey:
		case mpc_step:
			if (trickplay)
				AdvanceTrickFrame(param);
			else if (paused && !seeking)
				AdvanceFrame();
			break;
		case mpc_stop:
			if (!paused && !stopped)
				StopStreaming();

			paused = FALSE;
			stopped = TRUE;
			break;
		case mpc_freeze:
			if (!paused && !stopped)
				StopStreaming();

			paused = TRUE;
			stopped = FALSE;
			break;
		case mpc_resync:
		case mpc_resyncue:
			if (seeking)
				{
				SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID);
				BeginStreaming(VDPM_PLAY_FORWARD, !mpeg2 || stillFrameSequence, (__int64)param * MP2SR_SCALE_BYTES / positionScale);
				seeking = FALSE;
				paused = TRUE;
				stopped = FALSE;
				}
			break;
		case mpc_reverse:
			if (seeking)
				{
				SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID);
				BeginStreaming(VDPM_PLAY_BACKWARD, !mpeg2 || stillFrameSequence, (__int64)param * MP2SR_SCALE_BYTES / positionScale);
				seeking = FALSE;
				paused = TRUE;
				stopped = FALSE;
				}
			break;
		case mpc_scan:
			if (seeking)
				{
				SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID);
				BeginStreaming(VDPM_SCANNING, !mpeg2, param);
				StartStreaming(0x10000);
				trickplay = FALSE;
				seeking = FALSE;
				paused = FALSE;
				stopped = FALSE;
				scanning = TRUE;
				}
			break;
		case mpc_trickplay:
			if (seeking)
				{
				SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID);
				BeginStreaming(VDPM_TRICKPLAY, !mpeg2, param);
				StartStreaming(0x10000);
				trickplay = TRUE;
				seeking = FALSE;
				paused = FALSE;
				stopped = FALSE;
				scanning = FALSE;
				}
			break;
		case mpc_seekaudio:
			break;
		case mpc_resyncaudio:
			break;
		}

	GNRAISE_OK;
	}


//
//  Configure Units
//

Error UnitSetClass::ConfigureUnits(TAG __far * tags)
	{
	TAG	__far	*	tp;
	DWORD				temp;
	BOOL				audioCodingModeChanged, audioOutConfigChanged, ac3ConfigChanged;
	BOOL				ac3TestModeChanged;
	BOOL				croppingRectChanged, buttonRectChanged;
	BOOL				regionChanged;
//	BYTE				regionCode;
	int				regionCount;

	short destLeft, destTop, destWidth, destHeight;

	display->GetDisplayLeft(destLeft);
	display->GetDisplayTop(destTop);
	display->GetDisplayWidth(destWidth);
	display->GetDisplayHeight(destHeight);

	tp = tags;

	while (tp->id)
		{
		if (TAG_TYPE(tp->id) == TAG_QRY) QRY_TAG(tp) = FALSE;
		tp++;
		}

	audioOutConfigChanged		= FALSE;
	audioCodingModeChanged		= FALSE;
	croppingRectChanged			= FALSE;
	regionChanged				= FALSE;
	buttonRectChanged			= FALSE;
	ac3TestModeChanged			= FALSE;
	ac3ConfigChanged			= FALSE;

		{
		PARSE_TAGS_START(tags)
			GETSET(MPEG2_POSITION_SCALE, positionScale);
			GETSETDV(MPEG_SIGNAL_POSITION, signalPosition, SetSignalPosition((__int64)signalPosition * MP2SR_SCALE_BYTES / positionScale));
			GETSETDV(MPEG_VIDEO_STREAMID, videoStreamID, SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID));
			GETSETDV(MPEG_AUDIO_STREAMID, audioStreamID, SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID));
			GETSETDV(MPEG2_AUDIO_AC3_STREAMID, ac3StreamID, SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID));
			GETSETDV(MPEG2_AUDIO_LPCM_STREAMID, lpcmStreamID, SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID));
			GETSETDV(MPEG2_AUDIO_DTS_STREAMID, dtsStreamID, SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID));
			GETSETDV(MPEG2_SPU_STREAMID, spuStreamID, SetStreamID(videoStreamID, ac3StreamID, audioStreamID, lpcmStreamID, spuStreamID, dtsStreamID));
			GETSETDV(MPEG2_SPU_PALETTE_ENTRY, temp, spuDisplay.SetPalette((int)XTBF(0, 8, temp), (int)XTBF(24, 8, temp), (int)XTBF(16, 8, temp), (int)XTBF(8, 8, temp)));

 			GETSETDV(MPEG2_AUDIO_TYPE, audioType, audioCodingModeChanged = TRUE);

			GETSETDV(MPEG2_AC3_DUAL_MODE_CONFIG, dualModeCfg, audioOutConfigChanged = TRUE);
			GETSETDV(MPEG2_AC3_KARAOKE_CONFIG, karaokeCfg, audioOutConfigChanged = TRUE);
			GETSETDV(MPEG2_AC3_SPEAKER_CONFIG, spkCfg, audioOutConfigChanged = TRUE);

			GETSETDV(MPEG2_AC3_DYNAMIC_RANGE, ac3DynamicRange, ac3ConfigChanged = TRUE);
			GETSETDV(MPEG2_DECODE_LFE, enableLFE, ac3ConfigChanged = TRUE);
			GETSETDV(MPEG2_AC3_DIALOG_NORM, ac3DialogNorm, ac3TestModeChanged = TRUE);
			GETSETDV(MPEG2_AC3_OPERATIONAL_MODE, ac3OperationalMode, ac3TestModeChanged = TRUE);
			GETSETDV(MPEG2_AC3_HIGH_DYNAMIC_RANGE, ac3HighScaleFactor, ac3TestModeChanged = TRUE);
			GETSETDV(MPEG2_AC3_LOW_DYNAMIC_RANGE, ac3LowScaleFactor, ac3TestModeChanged = TRUE);
			GETDSETD(MPEG2_AC3_AUDIO_TYPE_CONFIG, ac3Decoder.GetDigitalOut, ac3Decoder.SetDigitalOut);
			GETDSETD(MPEG2_DTS_AUDIO_TYPE_CONFIG, dtsDecoder.GetDigitalOut, dtsDecoder.SetDigitalOut);

			GETSETDV(MPEG2_LPCM_BITSPERSAMPLE, bitsPerSample, audioCodingModeChanged = TRUE);
			GETSETDV(MPEG2_LPCM_CHANNELS, channels, audioCodingModeChanged = TRUE);
			GETSETDV(MPEG_AUDIO_SAMPLERATE, samplesPerSecond, audioCodingModeChanged = TRUE);

			GETSET(MPEG2_CODING_STANDARD, mpeg2);
			GETSET(MPEG2_STILL_FRAME_SEQUENCE, stillFrameSequence);
			GETSETDV(MPEG2_DVD_STREAM_DEMUX, dvdDemux, SetDVDDemux(dvdDemux != 0));
			GETSETDV(MPEG_STREAM_TYPE, streamType, SetStreamType((DVDStreamType)streamType));
			GETONLY(MPEG_CURRENT_STREAM_SEGMENT, VideoSegmentRequested());
			GETSETDV(MPEG_FIRST_STREAM_SEGMENT, firstStreamSegment, SetFirstVideoSegment(firstStreamSegment));
			GETSETDV(MPEG_LAST_STREAM_SEGMENT, lastStreamSegment, SetLastVideoSegment(lastStreamSegment));

			GETSETDV(MPEG2_SPU_BUTTON_STATE, buttonState, spuDisplay.SetButtonState((XSPUButtonState)buttonState));
			GETSETDV(MPEG2_SPU_BUTTON_LEFT, buttonLeft, buttonRectChanged = TRUE);
			GETSETDV(MPEG2_SPU_BUTTON_TOP, buttonTop, buttonRectChanged = TRUE);
			GETSETDV(MPEG2_SPU_BUTTON_WIDTH, buttonWidth, buttonRectChanged = TRUE);
			GETSETDV(MPEG2_SPU_BUTTON_HEIGHT, buttonHeight, buttonRectChanged = TRUE);

			GETSETDV(MPEG2_SPU_BUTTON_SELECT_COLOR, buttonSelect, spuDisplay.SetButtonColors(buttonSelect, buttonActive));
			GETSETDV(MPEG2_SPU_BUTTON_ACTIVE_COLOR, buttonActive, spuDisplay.SetButtonColors(buttonSelect, buttonActive));
			GETSETDV(MPEG2_SPU_ENABLE, spuEnabled, EnableSPU(spuEnabled != 0));

			GETSETDV(MPEG2_PRESENTATION_MODE, presentationMode, display->SetDisplayPresentationMode((PictureDisplayPresentationMode)presentationMode));

//			GETINQUIRE(PIP_DISPLAY_LEFT,   display->GetDisplayLeft(data););
//			GETINQUIRE(PIP_DISPLAY_TOP,    display->GetDisplayTop(data););
//			GETINQUIRE(PIP_DISPLAY_WIDTH,  display->GetDisplayWidth(data););
//			GETINQUIRE(PIP_DISPLAY_HEIGHT, display->GetDisplayHeight(data););

			GETINQUIRE(MPEG_DISPLAY_WIDTH, data = (TTYPE_MPEG_DISPLAY_WIDTH)videoDecoder.DecodeWidth(););
			GETINQUIRE(MPEG_DISPLAY_HEIGHT, data = (TTYPE_MPEG_DISPLAY_HEIGHT)videoDecoder.DecodeHeight(););
			GETONLY(MPEG_SUPPORTS_HIRES_STILL, TRUE);

//			GETONLY(PIP_DEST_LEFT,   destLeft);
//			GETONLY(PIP_DEST_TOP,    destTop);
//			GETONLY(PIP_DEST_WIDTH,  destWidth);
//			GETONLY(PIP_DEST_HEIGHT, destHeight);
//			GETSETDV(PIP_SOURCE_CROP_LEFT, cropLeft, croppingRectChanged = TRUE);
//			GETSETDV(PIP_SOURCE_CROP_TOP, cropTop, croppingRectChanged = TRUE);
//			GETSETDV(PIP_SOURCE_CROP_RIGHT, cropRight, croppingRectChanged = TRUE);
//			GETSETDV(PIP_SOURCE_CROP_BOTTOM, cropBottom, croppingRectChanged = TRUE);


			GETSETDV(ENC_COPY_PROTECTION, copyProtectionLevel, (display->SetMacrovisionLevel(copyProtectionLevel), ccDecoder->SetMacrovisionLevel(copyProtectionLevel)));
			GETSETDV(MPEG2_SPDIF_COPY_MODE, spdifCopyMode, audioPlaybackDevice->SetCopyProtectionLevel(spdifCopyMode));
            GETSETDV(ENC_COPY_MODE, eCGMSMode, (display->SetCGMSMode(eCGMSMode), ccDecoder->SetCGMSMode(eCGMSMode)));

			GETINQUIRE(PIP_COLORCONTROL, data = (WORD)colorControl;);

			GETINQUIRE(MPEG2_CURRENT_PLAYBACK_TIME, data = CurrentPlaybackTime(););

			GETSETDV(PIP_BRIGHTNESS, brightness, UpdateBrightness());
			GETSETDV(PIP_RED_BRIGHTNESS, redBrightness, UpdateBrightness());
			GETSETDV(PIP_GREEN_BRIGHTNESS, greenBrightness, UpdateBrightness());
			GETSETDV(PIP_BLUE_BRIGHTNESS, blueBrightness, UpdateBrightness());

			GETSETDV(PIP_CONTRAST, contrast, UpdateContrast());
			GETSETDV(PIP_SATURATION, saturation, UpdateSaturation());

			GETSET(PIP_COLORENABLE, colorEnable);
//			GETONLY(PIP_COLORKEYED, TRUE);
			GETONLY(PIP_COLORKEYED, directXDisplay->overlayAvailable);

			GETSETDV(MPEG2_VIDEO_SPLIT_STREAM_TYPE,	 splitStreamType[DVEST_VIDEO],		SetSplitStreamType(DVEST_VIDEO,      (DVDSplitStreamType)splitStreamType[DVEST_VIDEO]));
			GETSETDV(MPEG2_AUDIO_SPLIT_STREAM_TYPE,	 splitStreamType[DVEST_AUDIO],		SetSplitStreamType(DVEST_AUDIO,		 (DVDSplitStreamType)splitStreamType[DVEST_AUDIO]));
			GETSETDV(MPEG2_SPU_SPLIT_STREAM_TYPE,		 splitStreamType[DVEST_SUBPICTURE], SetSplitStreamType(DVEST_SUBPICTURE, (DVDSplitStreamType)splitStreamType[DVEST_SUBPICTURE]));

			GETONLY(BOARD_VERSION, boardVersionInfo);
			GETINQUIRE(BOARD_REGION_COUNT, 10;);
			GETSETDV(BOARD_REGION_CODE, board->regionCode, regionChanged = TRUE);

//	GETSETDV(MPEG2_CC_ENABLE, ccEnabled, ccDisplay->EnableDisplay(ccEnabled != 0));

			GETSET(MPEG2_TIMEOUT_MSG,	timeoutMsg);
			GETSET(MPEG2_ERROR_MSG,		errorMsg);
			GETSET(MPEG2_REFILL_MSG,	refillMsg);
			GETSET(MPEG2_DONE_MSG,		doneMsg);
			GETSET(MPEG2_SIGNAL_MSG,	signalMsg);
		PARSE_TAGS_END;
		}


	if (directXDisplay)
		{
		BOOL changed = FALSE;
		int left, top, width, height;
		int destLeft, destTop, destWidth, destHeight;
		BOOL sourceAdapt = directXDisplay->sourceAdapt;
		//BOOL clientAdapt = directXDisplay->clientAdapt;

		short outputLeft, outputTop, outputWidth, outputHeight;


		left = directXDisplay->sourceLeft;
		top = directXDisplay->sourceTop;
		width = directXDisplay->sourceWidth;
		height = directXDisplay->sourceHeight;

		destLeft = directXDisplay->destLeft;
		destTop = directXDisplay->destTop;
		destWidth = directXDisplay->destWidth;
		destHeight = directXDisplay->destHeight;

		outputLeft = (short)directXDisplay->outputLeft;
		outputTop = (short)directXDisplay->outputTop;
		outputWidth = (short)directXDisplay->outputWidth;
		outputHeight = (short)directXDisplay->outputHeight;


		PARSE_TAGS_START(tags)
//			GETSETDV(PIP_WINDOW,	hwnd, directXDisplay->SetWindow(hwnd));

			GETSETC(PIP_DISPLAY_LEFT, outputLeft);
			GETSETC(PIP_DISPLAY_TOP, outputTop);
			GETSETC(PIP_DISPLAY_HEIGHT, outputHeight);
			GETSETC(PIP_DISPLAY_WIDTH, outputWidth);

			GETSETC(PIP_SOURCE_LEFT, left);
			GETSETC(PIP_SOURCE_TOP, top);
			GETSETC(PIP_SOURCE_WIDTH, width);
			GETSETC(PIP_SOURCE_HEIGHT, height);
			GETSETC(PIP_ADAPT_SOURCE_SIZE, sourceAdapt);
			GETSETC(PIP_DEST_LEFT, destLeft);
			GETSETC(PIP_DEST_TOP, destTop);
			GETSETC(PIP_DEST_HEIGHT, destHeight);
			GETSETC(PIP_DEST_WIDTH, destWidth);

			//GETSETC(PIP_ADAPT_CLIENT_SIZE, clientAdapt);
			GETSETDV(PIP_COLORKEY_COLOR, colorKeyColor, directXDisplay->SetColorKeyColor(colorKeyColor));
		PARSE_TAGS_END;

		if (changed)
			{
			directXDisplay->SetSourceRectangle(sourceAdapt != 0, (WORD)left, (WORD)top, (WORD)width, (WORD)height);
			directXDisplay->SetDestRectangle(sourceAdapt != 0, (WORD)destLeft, (WORD)destTop, (WORD)destWidth, (WORD)destHeight);
			directXDisplay->SetOutputRectangle(outputLeft, outputTop, outputWidth, outputHeight);
			}

		}

	if (audioCodingModeChanged)
		{
#if AC3DISABLED
		if (audioType = MP2AUDTYP_AC3)
			audioType = MP2AUDTYP_MPEG;
#endif

		SetAudioCodingMode(audioType,
								 samplesPerSecond == 30464 ? 96000 : samplesPerSecond,
								 bitsPerSample,
								 channels);
		}

	if (audioOutConfigChanged)
		{
		SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);
		}

	if (ac3TestModeChanged)
		{
		ac3Decoder.SetDolbyTestMode(ac3OperationalMode, ac3HighScaleFactor, ac3LowScaleFactor, ac3DialogNorm);
		ac3Decoder.GetDolbyTestMode(ac3OperationalMode, ac3HighScaleFactor, ac3LowScaleFactor, ac3DialogNorm);
		ac3Decoder.GetAC3Config(ac3DynamicRange, enableLFE);
		}

	if (ac3ConfigChanged)
		{
		ac3Decoder.SetAC3Config(ac3DynamicRange, enableLFE);
		ac3Decoder.GetAC3Config(ac3DynamicRange, enableLFE);
		ac3Decoder.GetDolbyTestMode(ac3OperationalMode, ac3HighScaleFactor, ac3LowScaleFactor, ac3DialogNorm);
		}

	if (croppingRectChanged)
		{
		display->SetCropRectangle(cropLeft, cropTop, cropRight, cropBottom);
		}

	if (buttonRectChanged)
		{
		spuDisplay.SetButtonPosition(buttonLeft, buttonTop, buttonWidth, buttonHeight);
		}


	GNRAISE_OK;
	}


//
//  Update Brightness
//

void UnitSetClass::UpdateBrightness(void)
	{
	int y, u, v;

	y = brightness;
	u = blueBrightness - greenBrightness;
	v = redBrightness - greenBrightness;

	y = (y * 64) / 10000 + 96; if (y < 64) y = 64; else if (y > 192) y = 192;
	u = 128 + (u * 64) / 10000; if (u < 64) u = 64; else if (u > 192) u = 192;
	v = 128 + (v * 64) / 10000; if (v < 64) v = 64; else if (v > 192) v = 192;

	lowQualitySliceDecoder->SetBrightness(y, u, v);
	highQualitySliceDecoder->SetBrightness(y, u, v);
	}

//
//  Update Contrast
//

void UnitSetClass::UpdateContrast(void)
	{
	int c;

	c = contrast * 64 / 10000 + 96;

	lowQualitySliceDecoder->SetContrast(c);
	highQualitySliceDecoder->SetContrast(c);
	}

//
//  Update Saturation
//

void UnitSetClass::UpdateSaturation(void)
	{
	int c;

	c = saturation * 64 / 10000 + 96;

	lowQualitySliceDecoder->SetSaturation(c);
	highQualitySliceDecoder->SetSaturation(c);
	}


//
//  Watermarks
//

static const DWORD WaterMark0[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD WaterMark1[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD WaterMark2[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD WaterMark3[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD WaterMark4[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD WaterMark5[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD WaterMark6[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD WaterMark7[] =
	{
	0x47110815, 0x47110816, 0x47110817, 0x47110818,
	0x47110819, 0x4711081a, 0x4711081b, 0x4711081c
	};

static const DWORD * WaterMarkArray[] =
	{
	WaterMark7,
	WaterMark6,
	WaterMark3,
	WaterMark2,
	WaterMark4,
	WaterMark5,
	WaterMark1,
	WaterMark0
	};

static const struct
	{
	DWORD mark0[8];
	PCICheckBlock check[256];
	} checkBlock =
	{0x67823bcd, 0x1983541a, 0xcfe73621, 0xfb18cf32, 0x67823bcd, 0x1983541a, 0xcfe73621, 0xfb18cf32, { 0, 0}};

static inline BOOL CheckPCIBlock(HANDLE kDriver)
	{
	return TRUE;
	}

static DWORD seed;

#if PROTECTED
extern DWORD * CheckSum1;

static inline BOOL CheckCode(void)
	{
	DWORD * p = (DWORD *)(CheckSum1[0] ^ CheckSum1[4]);
	DWORD num = (CheckSum1[1] ^ CheckSum1[5]);
	DWORD sum = (CheckSum1[2] ^ CheckSum1[6]);
	DWORD res = (CheckSum1[3] ^ CheckSum1[7]);
	DWORD cnt;

	for(cnt = 0; cnt<num; cnt+=4)
		{
		sum = _rotl(sum, 1) + *p++;
		}

	return res == sum;
	}
#endif

#pragma warning(disable : 4731)
static inline void CheckWatermark(int a, int b)
	{
	}

#pragma warning(default : 4731)



extern "C" {BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD  fdwReason, LPVOID lpvReserved);}

//
//  VDR Open Driver
//

DLLCALL Error WINAPI VDR_OpenDriver(TCHAR __far * name, DWORD boardID, VDRHandle __far & handle)
	{
	if (boardID == 0)
		{


		DllMain(NULL, DLL_PROCESS_ATTACH, NULL);

		PMMSG("OpenDriver+\n");


		if (!OpenCount && globalOpenCount)
			{

			if (allowMultiInstance)
				primaryUnit = FALSE;
			else
				{
				handle = NULL;

				GNRAISE(GNR_OBJECT_IN_USE);
				}
			}

		globalOpenCount++;

		handle = (VDRHandle) 1;

		PMMSG("Ravisent Software Cinemaster DVD Decoder\n");


#if _DEBUG
		PMMSGX("DEBUG Version %04d \n",CinemLibVersion);
#else
		PMMSGX("Version %04d \n", CinemLibVersion);
#endif


		PMMSG("-----------------------------------------\n");

#if PROTECTED
#endif

		OpenCount++;

		PMMSG("OpenDriver-\n");

		GNRAISE_OK;
		}
	else if (boardID == 0x47110815)
		{
		int i, j;

		j = GetTickCount() & 7;

		for(i=0; i<8; i++)
			{
			DWORD d = WaterMarkArray[j][i];
			d = (d & 0xff) ^ ((d >> 8) & 0xff) ^ ((d >> 16) & 0xff) ^ (d >> 24);
			name[i] = (char)d;
			}
		name[8] = 0;

		handle = NULL;

		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_UNITS_BUSY);
	}

//
//  VDR Close Driver
//

DLLCALL Error WINAPI VDR_CloseDriver(VDRHandle handle)
	{
	if (OpenCount) globalOpenCount--;
	OpenCount--;

	if (!OpenCount)
		{
		if (board)
			{
			board->Destroy();
			delete board;
			}
		board = NULL;
		}

	GNRAISE_OK;
	}

//
//  VDR Reconfigure Driver
//

DLLCALL Error WINAPI VDR_ReconfigureDriver(VDRHandle handle)
	{
	if (board)
		{
		GNREASSERT(board->Reconfigure());
		}

	GNRAISE_OK;
	}

//
//  VDR Avail Units
//

DLLCALL DWORD WINAPI VDR_AvailUnits(VDRHandle handle)
	{
	DWORD units = MPEG_DECODER_UNIT | PIP_UNIT;

	return units;
	}

//
//  VDR Open Units
//

DLLCALL Error WINAPI VDR_OpenUnits(VDRHandle handle, DWORD requnits, UnitSet __far & units)
	{
	Error err = GNR_OK;

	PMMSG("OpenUnits++\n");

	CheckWatermark(2, 6);

	if (handle && OpenCount)
		{
		if (!board)
			{
			board = CreateBoard(DriverInstance);

			if (IS_ERROR(err = board->BeginInitialize()) ||
				 IS_ERROR(err = board->Initialize())      ||
				 IS_ERROR(err = board->CompleteInitialize()))
				{
				delete board;
				board = NULL;

				PMMSGX("OpenUnits Err %08lx\n", err);

				GNRAISE(err);
				}
			}

		units = board->CreateUnitSet();

		if (!units)
			{
			err = GNR_OBJECT_IN_USE;
			}
		else if (IS_ERROR(err = units->Initialize()))
			{
			delete units;
			units = NULL;
			}
		}
	else
		err = GNR_OBJECT_NOT_FOUND;

	PMMSGX("OpenUnits Err %08lx\n", err);

	GNRAISE(err);
	}

//
//  VDR Open SUb Units
//

DLLCALL Error WINAPI VDR_OpenSubUnits(UnitSet parent, DWORD requnits, UnitSet __far &units)
	{
	parent->referenceCount++;
	units = parent;

	GNRAISE_OK;
	}

//
//  VDR Close Units
//

DLLCALL Error WINAPI VDR_CloseUnits(UnitSet units)
	{
	CheckWatermark(3, 0);

	if (units)
		{
		units->referenceCount--;
		if (!units->referenceCount)
			{
			units->PassivateUnits();
			delete units;
			}
		}

	GNRAISE_OK;
	}

//
//  VDR Configure Units
//

DLLCALL Error WINAPI VDR_ConfigureUnits(UnitSet units, TAG __far * tags)
	{
	Error err;

	if (!units && cus) units = cus;

	if (units)
		{
		err = units->ConfigureUnits(tags);

		return err;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);
	}

//
//  VDR Configure T Units Set
//

DLLCALL Error WINAPI VDR_T_ConfigureUnitsSet(UnitSet units, TAG __far * tags)
	{
	return VDR_ConfigureUnits(units, tags);
	}

//
//  VDR Configure T Units Get
//

DLLCALL Error WINAPI VDR_T_ConfigureUnitsGet(UnitSet units, DWORD id, void * ref)
	{
	TAG t[2];

	t[0].id = id;
	t[0].data = (DWORD)ref;
	t[1] = TAGDONE;

	return VDR_ConfigureUnits(units, t);
	}

//
//  VDR Lock Units
//

DLLCALL Error WINAPI VDR_LockUnits(UnitSet units)
	{
	GNRAISE_OK;
	}

//
//  VDR_UnlockUnits
//

DLLCALL Error WINAPI VDR_UnlockUnits(UnitSet units)
	{
	GNRAISE_OK;
	}

//
//  VDR Activate Units
//

DLLCALL Error WINAPI VDR_ActivateUnits(UnitSet units)
	{
	if (units)
		units->ActivateUnits();

	GNRAISE_OK;
	}

//
//  VDR Passivate Units
//

DLLCALL Error WINAPI VDR_PassivateUnits(UnitSet units)
	{
	if (units)
		units->PassivateUnits();

	GNRAISE_OK;
	}

//
//  VDR Enable PIP
//

DLLCALL Error WINAPI VDR_EnablePIP(UnitSet units, BOOL enable)
	{
	CheckWatermark(2, 5);

	if (units->pipEnabled != enable)
		{
		units->pipEnabled = enable;
		if (units->active)
			{
			if (units->directXDisplay)
				units->directXDisplay->EnableDisplay(enable != 0);
			}
		}

	GNRAISE_OK;
	}

//
//  VDR Update PIP
//

DLLCALL Error WINAPI VDR_UpdatePIP(UnitSet units)
	{
	if (units && units->active)
		if (units->directXDisplay)
			units->directXDisplay->UpdateDisplay();

	GNRAISE_OK;
	}

//
//  VDR Grab Frame
//

DLLCALL Error WINAPI VDR_GrabFrame(UnitSet units, FPTR base,
                            WORD width, WORD height,
                            WORD stride,
                            GrabFormat fmt)
	{
	GNRAISE_OK;
	}

//
//  VDR Send MPEG Data
//

DLLCALL DWORD WINAPI VDR_SendMPEGData(UnitSet units, HPTR data, DWORD size)
	{
	return units->SendData((BYTE *)data, size);
	}

DLLCALL DWORD	WINAPI VDR_SendMPEGDataMultiple(UnitSet units, MPEGDataSizePair * data, DWORD size)
	{
	return units->SendDataMultiple(data, size);
	}

//
//  VDR Send MPEG Data Split
//

DLLCALL DWORD WINAPI VDR_SendMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType, HPTR data, DWORD size, int timeStamp)
	{
	return units->SendDataSplit((DVDElementaryStreamType)streamType, (BYTE *)data, size, timeStamp);
	}

DLLCALL DWORD WINAPI VDR_SendMPEGDataSplitMultiple(UnitSet units, MPEGElementaryStreamType streamType, MPEGDataSizePair * data, DWORD size)
	{
	return units->SendDataSplitMultiple((DVDElementaryStreamType)streamType, data, size);
	}

//
//  VDR Complete MPEG Data
//

DLLCALL void WINAPI VDR_CompleteMPEGData(UnitSet units)
	{
	units->CompleteData();
	}

//
//  VDR Complete MPEG Data Split
//

DLLCALL void WINAPI VDR_CompleteMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType)
	{
	units->CompleteDataSplit((DVDElementaryStreamType)streamType);
	}

//
//  VDR Restart MPEG Data Split
//

DLLCALL void WINAPI VDR_RestartMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType)
	{
	units->RestartDataSplit((DVDElementaryStreamType)streamType);
	}

//
//  VDR MPEG Command Pending
//

DLLCALL BOOL WINAPI VDR_MPEGCommandPending(UnitSet units, DWORD tag)
	{
	return FALSE;
	}

//
//  VDR Send MPEG Command
//

DLLCALL Error WINAPI VDR_SendMPEGCommand(UnitSet units, MPEGCommand com, long param, DWORD __far &tag)
	{
	tag = 0;
	return VDR_DoMPEGCommand(units, com, param);
	}

//
//  VDR Do Command
//

DLLCALL Error WINAPI VDR_DoMPEGCommand(UnitSet units, MPEGCommand com, long param)
	{
	CheckWatermark(7, 1);

	if (units->active)
		{
		return units->DoMPEGCommand(com, param);
		}

	GNRAISE_OK;
	}

//
//  VDR Complete MPEG Command
//

DLLCALL Error WINAPI VDR_CompleteMPEGCommand(UnitSet units, DWORD tag)
	{
	GNRAISE_OK;
	}

//
//  VDR Current MPEG Location
//

DLLCALL DWORD WINAPI VDR_CurrentMPEGLocation(UnitSet units)
	{
	return (DWORD)(units->CurrentLocation() * units->positionScale / MP2SR_SCALE_BYTES);
	}

//
//  VDR Current MPEG Transfer Location
//

DLLCALL DWORD WINAPI VDR_CurrentMPEGTransferLocation(UnitSet units)
	{
	return (DWORD)(units->CurrentInputLocation());
	}

//
//  VDR Current MPEG State
//

DLLCALL MPEGState WINAPI VDR_CurrentMPEGState(UnitSet units)
	{
	if (!units->active)
		return mps_preempted;
	else if (units->seeking)
		return mps_seeking;
	else if (units->paused)
		return mps_frozen;
	else if (units->stopped)
		return mps_stopped;
	else if (units->scanning)
		return mps_scanning;
	else if (units->trickplay)
		return mps_trickplaying;
	else if (units->StreamingCompleted())
		return mps_stopped;
	else
		return mps_playing;
	}

//
//  VDR Install MPEG Win Hooks
//

DLLCALL Error WINAPI VDR_InstallMPEGWinHooks(UnitSet units, HWND hwnd, WORD refillMsg, WORD signalMsg, WORD doneMsg)
	{
	if (units)
		{
		units->msgWindow = hwnd;
		units->refillMsg = refillMsg;
		units->signalMsg = signalMsg;
		units->doneMsg = doneMsg;

		if (refillMsg == 0x1000 && signalMsg == 0x1001 && doneMsg == 0x1003)
			units->errorMsg = 0x1002;
		else
			units->errorMsg = 0;

		units->refillMsgPending = FALSE;
		}

	GNRAISE_OK;
	}

//
//  VDR Remove MPEG Win Hooks
//

DLLCALL Error WINAPI VDR_RemoveMPEGWinHooks(UnitSet units)
	{
	if (units)
		{
		units->msgWindow = NULL;
		}

	GNRAISE_OK;
	}

//
//  VDR Complete MPEG Refill Message
//

DLLCALL Error WINAPI VDR_CompleteMPEGRefillMessage(UnitSet units)
	{
	if (units)
		units->refillMsgPending = FALSE;

	GNRAISE_OK;
	}

DLLCALL Error WINAPI VDR_InstallMessagePort(UnitSet units, GenericMsgPort * port)
	{
	if (units)
		{
		units->msgPort = port;
		}

	GNRAISE_OK;
	}

DLLCALL Error WINAPI VDR_RemoveMessagePort(UnitSet units)
	{
	if (units)
		{
		units->msgPort = NULL;
		}

	GNRAISE_OK;
	}


//
//  VDR Do Authentication Command
//

DLLCALL Error WINAPI VDR_DoAuthenticationCommand(UnitSet units, MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key)
	{
	CheckWatermark(4, 2);

	if (allowCSS)
		return units->decryptor->DoAuthenticationCommand(com, sector, key);
	else
		GNRAISE(GNR_CSS_NOT_SUPPORTED);
	}

//
//  VDR Performance Monitor
//

DLLCALL void WINAPI VDR_ReadPerformanceMonitor(PerformanceMonitor & pmon)
	{
	pmon = perfmon;
	perfmon.maxFrameSkew = 0;
	}

//
//  VDR Check Region Code Valid
//

DLLCALL BOOL WINAPI VDR_CheckRegionCodeValid(UnitSet units, BYTE regionSet)
	{
	int regionMagic, changes;
	int i;

	CheckWatermark(0, 3);

	if (board)
		{
		return (~regionSet & (1 << (board->regionCode - 1)) & 0xff) != 0;
		}

	return FALSE;
	}


extern "C" {


//
//  Dll Main
//

#if _DEBUG
_CrtMemState memstate;
#endif

BOOL WINAPI DllMain(HINSTANCE hDLLInst,
	                 DWORD  fdwReason,
		              LPVOID lpvReserved)
	{
#if PROTECTED
	if ((DWORD)hDLLInst != 0x4b000000 || !CheckCode())
		return FALSE;
#endif

	DriverInstance = hDLLInst;

	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
			seed = ::GetTickCount() ^ 0x2412341;

			InitFrameStoreScalerTables();

			dctCoefficientZeroFirst.Merge(dctCoefficientZero);

			dctCoefficientZeroFirst.BuildExtDCTTable();
			dctCoefficientZero.BuildExtDCTTable();
			dctCoefficientOne.BuildExtDCTTable();

			dctCoefficientZeroFirst.BuildFlatDCTTable();
			dctCoefficientZero.BuildFlatDCTTable();
			dctCoefficientOne.BuildFlatDCTTable();

			motionCodes.PrepareByteTable(2);

			codedBlockPattern420.BuildExtPatternTable();

			macroBlockAddressIncrement.Secure();

			iFrameMacroBlockType.Secure();;
			bFrameMacroBlockType.Secure();;
			pFrameMacroBlockType.Secure();;

			iFieldMacroBlockType.Secure();;
			bFieldMacroBlockType.Secure();;
			pFieldMacroBlockType.Secure();;

			iFrameMacroBlockTypeFPFD.Secure();;
			bFrameMacroBlockTypeFPFD.Secure();;
			pFrameMacroBlockTypeFPFD.Secure();;

			codedBlockPattern420.Secure();;
			dmtVector.Secure();;
			dctDCSizeLuminance.Secure();;
			dctDCSizeChrominance.Secure();;
			dctCoefficientZeroFirst.Secure();;
			dctCoefficientZero.Secure();;
			dctCoefficientOne.Secure();;

			perfmon.videoBitBufferSize = VIDEO_STREAM_BUFFER_SIZE * 32;
			perfmon.audioBitBufferSize = AUDIO_STREAM_BUFFER_SIZE * 32;

			InitDecryption();
#if _DEBUG
			_CrtMemCheckpoint(&memstate);
#endif

			break;

		case DLL_PROCESS_DETACH:
#if _DEBUG
			_CrtMemDumpAllObjectsSince(&memstate );
#endif

			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
		}

	return TRUE;
	}

}
