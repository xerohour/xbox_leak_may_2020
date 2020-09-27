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

// FILE:			library\hardware\tuner\generic\tvtuner.cpp
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1995 Viona Development.  All Rights Reserved.
// CREATED:		17.07.95
//
// PURPOSE:		Implementation of abstract base class for tv tuners
//
// HISTORY:

#include "tvtuner.h"
#include "library\support\gfxdebug\gfxdebug.h"


NAME_ERROR(TVT_ILLEGAL_TV_STD, "Illegal TV standard");
NAME_ERROR(TVT_ILLEGAL_FREQUENCY, "Illegal frequency");
NAME_ERROR(TVT_PLL_NOT_LOCKED, "PLL not locked");
NAME_ERROR(TVT_NOT_CURRENT, "TV tuner not current unit");
NAME_ERROR(TVT_NOT_IMPLEMENTED, "TV function not implemented");
NAME_ERROR(TVT_NO_STATION_FOUND, "No station found");
NAME_ERROR(TVT_LOWER_LIMIT, "Lower limit of frequency range reached");
NAME_ERROR(TVT_UPPER_LIMIT, "Upper limit of frequency range reached");
NAME_ERROR(TVT_SCAN_CANCELED, "Auto Scan canceled");
NAME_ERROR(TVT_NO_DIALOG_BOX, "Dialog box could not be opened");



#define CHANGED_FREQUENCY	MKFLAG(0)
#define CHANGED_STANDARD	MKFLAG(1)

//
// Flags that require a total reporgramming of the chip
//
#define CHANGED_VITAL			(CHANGED_FREQUENCY | CHANGED_STANDARD)
#define CHANGED_ALL				0xffff

// TV Tuner object for dialog box message function access.

//TVTuner *	tvTunerObj;

/////////////////////////////////////////////////////////////////////////////////////////////
// Class TVTuner implementation (physical unit)
/////////////////////////////////////////////////////////////////////////////////////////////

// The constructor defines the properties of the different TV tuner standards.
TVTuner::TVTuner()
	{
	//
	// The unit of the following values is [10^2 Hz] !
	//

	// Please pay attention: The band limits for PAL and NTSC are for Philips tuners only!!!!
	// The others were taken from ALPS documentation.

	tvStdProperties[PAL_AIR].minFreq			=  392500;
	tvStdProperties[PAL_AIR].stepSize		=     625;
	tvStdProperties[PAL_AIR].lowLimit		= 1572500 + 30000;
	tvStdProperties[PAL_AIR].midLimit		= 4512500 + 30000;
	tvStdProperties[PAL_AIR].highLimit		= 8952500 + 30000;
	tvStdProperties[PAL_AIR].visionIF		=  389000;

	tvStdProperties[PAL_CABLE].minFreq		=  392500;
	tvStdProperties[PAL_CABLE].stepSize		=     625;
	tvStdProperties[PAL_CABLE].lowLimit		= 1572500 + 30000;
	tvStdProperties[PAL_CABLE].midLimit		= 4512500 + 30000;
	tvStdProperties[PAL_CABLE].highLimit	= 8952500 + 30000;
	tvStdProperties[PAL_CABLE].visionIF		=  389000;

	tvStdProperties[NTSC_AIR].minFreq		=  462500;
	tvStdProperties[NTSC_AIR].stepSize		=     625;
	tvStdProperties[NTSC_AIR].lowLimit  	= 1572500 + 30000;
	tvStdProperties[NTSC_AIR].midLimit  	= 4512500 + 30000;
	tvStdProperties[NTSC_AIR].highLimit 	= 8012500 + 30000;
	tvStdProperties[NTSC_AIR].visionIF  	=  457500;

	tvStdProperties[NTSC_CABLE].minFreq		=  462500;
	tvStdProperties[NTSC_CABLE].stepSize	=     625;
	tvStdProperties[NTSC_CABLE].lowLimit	= 1572500 + 30000;
	tvStdProperties[NTSC_CABLE].midLimit	= 4512500 + 30000;
	tvStdProperties[NTSC_CABLE].highLimit	= 8012500 + 30000;
	tvStdProperties[NTSC_CABLE].visionIF	=  457500;

	tvStdProperties[SECAM_AIR].minFreq		=  392500;
	tvStdProperties[SECAM_AIR].stepSize		=     625;
	tvStdProperties[SECAM_AIR].lowLimit		= 1572500 + 30000;
	tvStdProperties[SECAM_AIR].midLimit		= 4512500 + 30000;
	tvStdProperties[SECAM_AIR].highLimit	= 8952500 + 30000;
	tvStdProperties[SECAM_AIR].visionIF		=  457500;

	tvStdProperties[SECAM_CABLE].minFreq 	=  392500;
	tvStdProperties[SECAM_CABLE].stepSize 	=     625;
	tvStdProperties[SECAM_CABLE].lowLimit  = 1572500 + 30000;
	tvStdProperties[SECAM_CABLE].midLimit  = 4512500 + 30000;
	tvStdProperties[SECAM_CABLE].highLimit = 8952500 + 30000;
	tvStdProperties[SECAM_CABLE].visionIF  =  457500;

	tvStdProperties[JAPAN_AIR].minFreq		=  822500;
	tvStdProperties[JAPAN_AIR].stepSize		=     625;
	tvStdProperties[JAPAN_AIR].lowLimit  	= 1652500 + 30000;
	tvStdProperties[JAPAN_AIR].midLimit  	= 3132500 + 30000;
	tvStdProperties[JAPAN_AIR].highLimit 	= 7652500 + 30000;
	tvStdProperties[JAPAN_AIR].visionIF  	=  587500;

	tvStdProperties[JAPAN_CABLE].minFreq	=  822500;
	tvStdProperties[JAPAN_CABLE].stepSize	=     625;
	tvStdProperties[JAPAN_CABLE].lowLimit 	= 1652500 + 30000;
	tvStdProperties[JAPAN_CABLE].midLimit 	= 3132500 + 30000;
	tvStdProperties[JAPAN_CABLE].highLimit	= 7652500 + 30000;
	tvStdProperties[JAPAN_CABLE].visionIF  =  587500;

	tvStdProperties[PAL_I_AIR].minFreq		=  392500;
	tvStdProperties[PAL_I_AIR].stepSize		=     500;
	tvStdProperties[PAL_I_AIR].lowLimit  	= 1262500 + 30000;
	tvStdProperties[PAL_I_AIR].midLimit  	= 3432500 + 30000;
	tvStdProperties[PAL_I_AIR].highLimit 	= 8952500 + 30000;
	tvStdProperties[PAL_I_AIR].visionIF  	=  395000;

	tvStdProperties[PAL_I_CABLE].minFreq	=  392500;
	tvStdProperties[PAL_I_CABLE].stepSize	=     500;
	tvStdProperties[PAL_I_CABLE].lowLimit 	= 1262500 + 30000;
	tvStdProperties[PAL_I_CABLE].midLimit 	= 3432500 + 30000;
	tvStdProperties[PAL_I_CABLE].highLimit	= 8952500 + 30000;
	tvStdProperties[PAL_I_CABLE].visionIF  =  395000;

	freq = 900000;
	tvStd = PAL_CABLE;

	changed = CHANGED_ALL;
	}



Error TVTuner::SetTVStandard(TVTunerTVStandard tvStd)
	{
	if ((tvStd >= MIN_TV_STD) && (tvStd <= MAX_TV_STD))			// Check validity of desired TV standard.
		{
		this->tvStd = tvStd;													// Set as new current TV standard.
		changed |= CHANGED_STANDARD;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_TVT_ILLEGAL_TV_STD);
	}

Error TVTuner::SetFrequency(DWORD freq)
	{
//	if (freq > tvStdProperties[tvStd].minFreq && freq < tvStdProperties[tvStd].highLimit)
		{
		this->freq = freq;													// Set new frequency.
		changed |= CHANGED_FREQUENCY;
		GNRAISE_OK;
		}
//	else
//		GNRAISE(GNR_TVT_ILLEGAL_FREQUENCY);
	}

Error TVTuner::SetScanDir(Direction dir)
	{
	scanDir = dir;
	GNRAISE_OK;
	}

Error	TVTuner::SetScanState(ScanState state)
	{
	scanState = state;
	GNRAISE_OK;
	}

/////////////////////////////////////////////////////////////////////////////////////////////
// Class VirtualTVTuner implementation (virtual unit)
/////////////////////////////////////////////////////////////////////////////////////////////

VirtualTVTuner::VirtualTVTuner(TVTuner* physical) : VirtualUnit(physical)
	{
	tuner = physical;
	freq = 900000;
	tvStd = PAL_CABLE;
	}

Error VirtualTVTuner::Configure(TAG __far * tags)
	{
	PARSE_TAGS_START(tags)
		GETSETVC(TVT_TVSTANDARD,	tvStd,	GNREASSERT(tuner->SetTVStandard(tvStd)));
//		GETSETVC(TVT_FREQUENCY, 	freq, 	{GNREASSERT(tuner->SetFrequency(freq)); GDP("TVT_FREQUENCY: %ld", freq);});
		GETSETVC(TVT_FREQUENCY, 	freq, 	GNREASSERT(tuner->SetFrequency(freq)));
		GETINQUIRE(TVT_SIGNAL_LEVEL, {
												if (GetState() != current)
											 		GNRAISE(GNR_TVT_NOT_CURRENT);
												else
													GNREASSERT(tuner->CheckInputLevel(data));
											  });
	PARSE_TAGS_END

	if (GetState() == current)
		GNREASSERT(tuner->Update());

	GNRAISE_OK;
	}

Error VirtualTVTuner::Preempt(VirtualUnit * previous)
	{
	GNREASSERT(tuner->SetTVStandard(tvStd));
	GNREASSERT(tuner->SetFrequency(freq));
	GNREASSERT(tuner->SetScanDir(scanDir));
	GNREASSERT(tuner->SetScanState(scanState));

	GNREASSERT(tuner->Update());

	GNRAISE_OK;
	}

Error VirtualTVTuner::CheckInputLevel(WORD __far &level)
	{
	if (GetState() != current)
		GNRAISE(GNR_TVT_NOT_CURRENT);
	else
		return tuner->CheckInputLevel(level);
	}

Error VirtualTVTuner::ScanStation(Direction dir)
	{
	DWORD newFreq;

	if (GetState() != current)
		GNRAISE(GNR_TVT_NOT_CURRENT);
	else
		{
		GNREASSERT(tuner->ScanStation(dir, newFreq));
		freq = newFreq;
		}
	GNRAISE_OK;
	}

Error VirtualTVTuner::StartStepScan(Direction dir)
	{
	scanDir		= dir;
	scanState	= SCAN_COARSE;									// Initial state.

	if (dir == SCAN_UP)											// Add a little offset to the current frequency so
		freq += 30000;												// that the same station is not found twice.
	else
		freq -= 20000;

	if (GetState() == current)
		{
		GNREASSERT(tuner->SetFrequency(freq));
		GNREASSERT(tuner->SetScanDir(scanDir));
		GNREASSERT(tuner->SetScanState(scanState));
		}

	GNRAISE_OK;
	}

Error VirtualTVTuner::ScanStep(ScanResult __far & result, DWORD __far & curFreq, WORD __far & curLevel)
	{
	if (GetState() != current)
		GNRAISE(GNR_TVT_NOT_CURRENT);
	else
		{
		GNREASSERT(tuner->ScanStep(result, curFreq, curLevel, scanState));
		freq = curFreq;
		}

	GNRAISE_OK;
	}

