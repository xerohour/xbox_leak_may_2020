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

// FILE:			library\hardware\tuner\generic\tvtuner.h
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1995 Viona Development.  All Rights Reserved.
// CREATED:		17.07.95
//
// PURPOSE:		Definition of abstract base class for TV tuners
//
// HISTORY:

#ifndef TVTUNER_H
#define TVTUNER_H

#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\hooks.h"
#include "library\hardware\audio\generic\stereodc.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"
#include "library\common\virtunit.h"

#endif


/////////////////////////////////////////////////////////////////////////////////////////////
// Definitions of helper data structures
/////////////////////////////////////////////////////////////////////////////////////////////

// number of available TV standards
#define STDS_NUM 10

// This struct defines the necessary properties of a TV standard for the purposes of
// this low-level driver
typedef struct
	{
	DWORD		minFreq;			// minimal allowed frequency
	DWORD		lowLimit,		// frequency limit for the LOW band [10^2 Hz]
				midLimit,		// frequency limit for the MID band [10^2 Hz]
				highLimit;		// frequency limit for the HIGH band [10^2 Hz]
	DWORD		visionIF;		// IF frequency [10^2 Hz]
	DWORD		stepSize;		// Scan step size.
	}  TVStdProperty;

// Define directions for automatic station search
enum Direction {SCAN_UP, SCAN_DOWN};	// SCAN_UP: search towards higher frequencies
													// SCAN_DOWN: search towards lower frequencies

// Defines which TV standards are available
enum TVTunerTVStandard { MIN_TV_STD = 0,
								 PAL_AIR=0, PAL_CABLE,
								 NTSC_AIR,  NTSC_CABLE,
				   			 SECAM_AIR, SECAM_CABLE,
				   			 JAPAN_AIR, JAPAN_CABLE,
				   			 PAL_I_AIR, PAL_I_CABLE,
				   			 MAX_TV_STD = PAL_I_CABLE };

// Define bands
enum TVTunerBands { MIN_BAND		=	0,
						  BAND_LOW		=	0,
						  BAND_MID,
						  BAND_HIGH,
						  MAX_TV_BAND	=	BAND_HIGH,
						  BAND_FM,							// Extension for tuners with FM reception capability
						  MAX_BAND		=	BAND_FM
						};


// Result of stepwise scan function (StepScan)
enum ScanResult {STATION_FOUND,				// A station was found during the last step
					  NO_STATION,					// No station was found.
					  UPPER_LIMIT,					// Upper limit of frequency range reached.
					  LOWER_LIMIT					// Lower limit reached.
					 };

// Step ratio for tuner
enum StepRatioSelect {STEP_AUTOMATIC,		// Step is chosen according to FM enable
							 STEP_SIZE_50,			// One step is 50		kHz, used for FM and PAL TV
							 STEP_SIZE_31_25,		//					31.25	kHz, currently NOT SUPPORTED!!
							 STEP_SIZE_62_5		//					62.5	kHz, used for NTSC TV
							};

/////////////////////////////////////////////////////////////////////////////////////////////
// Definition of callback hook for station scans
/////////////////////////////////////////////////////////////////////////////////////////////

struct TVScanMsg
	{
	DWORD				frequency;
	WORD				level;
	};

MKHOOK(TVScan, TVScanMsg)

typedef TVScanHook __far * TVScanHookPtr;

/////////////////////////////////////////////////////////////////////////////////////////////
// Tag definitions for TV tuners
/////////////////////////////////////////////////////////////////////////////////////////////

MKTAG (TVT_TVSTANDARD,		TV_TUNER_UNIT, 0x0001, TVTunerTVStandard)	// used to set TV standard
MKTAG (TVT_FREQUENCY ,		TV_TUNER_UNIT, 0x0002, DWORD)					// current frequency for channel (10^2 Hz units)
MKTAG (TVT_SIGNAL_LEVEL,	TV_TUNER_UNIT, 0x0004, WORD)					// Tag to get the signal level of the station
																							// on the current frequency. Not always supported.
																							// Value range 0 - 10000.
MKTAG (TVT_CHARGEPUMP,		TV_TUNER_UNIT, 0x1001, BOOL)					// 1: fast tuning, 0: slow but
																							// more precise tuning
MKTAG (TVT_PLLDISABLE,		TV_TUNER_UNIT, 0x1002, BOOL)					// 1: manual tuning, 0: normal op.
MKTAG (TVT_STEREO,	   	TV_TUNER_UNIT, 0x1003, BOOL)					// 1: Stereo,  0: Mono
MKTAG (TVT_MUTE,		   	TV_TUNER_UNIT, 0x1004, BOOL)					// 1: sound off,  0: sound on

MKTAG (TVT_STEPSIZE,			TV_TUNER_UNIT, 0x1008, StepRatioSelect)	// Default: automatic
MKTAG (TVT_FMENABLE,			TV_TUNER_UNIT, 0x1009, BOOL)					// Switch to FM band. Default: FALSE
MKTAG	(TVT_MODEDETECT,		TV_TUNER_UNIT, 0x100a, SignalMode)			// Used to inquire signal type
MKTAG (TVT_MODESELECT,		TV_TUNER_UNIT, 0x100b, SignalMode)			// Used to force certain reception mode

/////////////////////////////////////////////////////////////////////////////////////////////
// Error definitions for CTVTuner
/////////////////////////////////////////////////////////////////////////////////////////////

// An illegal TV standard was supported by the caller
#define GNR_TVT_ILLEGAL_TV_STD		MKERR(ERROR, TVTUNER, BOUNDS, 0x01)

// The provided frequency is not in the valid range for the given TV standard
#define GNR_TVT_ILLEGAL_FREQUENCY	MKERR(ERROR, TVTUNER, BOUNDS, 0x02)

// It was not possible to lock the PLL. Certainly this message will occur, if the PLL is
// disabled
#define GNR_TVT_PLL_NOT_LOCKED		MKERR(ERROR, TVTUNER, BUSY,   0x03)

// It is not possible to execute the action because this virtual unit is not the "current"
// one possessing the hardware
#define GNR_TVT_NOT_CURRENT			MKERR(ERROR, TVTUNER, INACTIVE, 0x04)

// Function not yet implemented.
#define GNR_TVT_NOT_IMPLEMENTED		MKERR(ERROR, TVTUNER, GENERAL, 0x05)

// ScanStation couldn't find a station.
#define GNR_TVT_NO_STATION_FOUND		MKERR(ERROR, TVTUNER, GENERAL, 0x06)

// ScanStaion with SCAN_DOWN reached the lower limit.
#define GNR_TVT_LOWER_LIMIT			MKERR(ERROR, TVTUNER, BOUNDS, 0x07)

// ScanStaion with SCAN_UP reached the upper limit.
#define GNR_TVT_UPPER_LIMIT			MKERR(ERROR, TVTUNER, BOUNDS, 0x08)

// Tells that the station scan was canceled by the user.
#define GNR_TVT_SCAN_CANCELED			MKERR(ERROR, TVTUNER, GENERAL, 0x09)

// Dialog box could not be opened
#define GNR_TVT_NO_DIALOG_BOX			MKERR(ERROR, TVTUNER, GENERAL, 0x0a)


#ifndef ONLY_EXTERNAL_VISIBLE

enum ScanState {SCAN_COARSE, SCAN_FINE, SCAN_SIGNAL_CHECK, SCAN_FINE_FORWARD};


/////////////////////////////////////////////////////////////////////////////////////////////
// Class TVTuner definition (realizes physical unit)
/////////////////////////////////////////////////////////////////////////////////////////////
class TVTuner : public PhysicalUnit {
	friend class VirtualTVTuner;

	protected:
		Direction		scanDir;
		ScanState		scanState;

		DWORD				changed;

		TVStdProperty	tvStdProperties[STDS_NUM];						// stores the properties of TV standards

		DWORD				freq;

		TVTunerTVStandard	tvStd;

		TVTuner();

		virtual 	Error SetTVStandard(TVTunerTVStandard tvStd);
		virtual 	Error SetFrequency(DWORD freq);

		virtual	Error SetScanDir(Direction dir);
		virtual	Error	SetScanState(ScanState state);

		virtual 	Error CheckInputLevel(WORD __far & level) = 0;
		virtual	Error	ScanStation(Direction dir, DWORD __far & freq) = 0;
		virtual	Error	StartStepScan(Direction dir) = 0;
		virtual	Error ScanStep(ScanResult __far & result,
										DWORD __far & curFreq,
										WORD	__far & curLevel,
									   ScanState __far & curState) = 0;

		virtual	Error Update(void) = 0;
	};


/////////////////////////////////////////////////////////////////////////////////////////////
// Class VirtualTVTuner definition (realizes virtual unit)
/////////////////////////////////////////////////////////////////////////////////////////////
class VirtualTVTuner : public VirtualUnit {
	private:
		TVTuner*					tuner;					// Pointer to the physical unit.
	protected:
		DWORD						freq;						// Current frequency of channel (10^2 Hz units)
		TVTunerTVStandard		tvStd;					// Stores the current TV standard of this virtual unit

		Direction				scanDir;
		ScanState				scanState;

	public:
		VirtualTVTuner(TVTuner* physical);

		virtual Error Configure(TAG __far * tags);
		virtual Error Preempt(VirtualUnit * previous);
																				// Standard configuration function
		virtual Error ScanStation(Direction dir);					// Search for the next available station and
																				// set the current frequency to its value if one was found.
		virtual Error StartStepScan(Direction dir);				// Initialize stepwise scan

		virtual Error ScanStep(ScanResult __far & 	result,	// Scan stepwise
									  DWORD	__far & 			curFreq,
									  WORD __far & 			curLevel);

		virtual Error CheckInputLevel(WORD __far & level);	// Check the signal strength of the station on the
																				// current frequency.
	};

#endif // of ONLY_EXTERNAL_VISIBLE


#endif

