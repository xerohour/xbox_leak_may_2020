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
//  General definitions for DVD and CD
//
//////////////////////////////////////////////////////////////////////

#include "Library/Hardware/Drives/Generic/CDVDDefs.h"

//////////////////////////////////////////////////////////////////////
//
//  CD TOC Entry Class
//
//////////////////////////////////////////////////////////////////////

//
//  Default constructor
//

CDTocEntry::CDTocEntry(void)
	{
	startBlock = 0;
	numberOfBlocks = 0;
	trackType = CTT_NONE;
	}

//
//  Constructor with block arguments
//

CDTocEntry::CDTocEntry(DWORD startBlock, DWORD numberOfBlocks, CDTrackType trackType)
	{
	this->startBlock = startBlock;
	this->numberOfBlocks = numberOfBlocks;
	this->trackType = trackType;
	}

//
//  Constructor with time arguments
//

CDTocEntry::CDTocEntry(BYTE startMin, BYTE startSec, BYTE startFrame, BYTE playMin,  BYTE playSec,  BYTE playFrame,
							  CDTrackType trackType)
	{
	SetStartBlock(startMin, startSec, startFrame);
	SetNumberOfBlocks(playMin, playSec, playFrame);
	this->trackType = trackType;
	}

//
//  Copy constructor
//

CDTocEntry::CDTocEntry(const CDTocEntry & cdte)
	{
	startBlock = cdte.startBlock;
	numberOfBlocks = cdte.numberOfBlocks;
	trackType = cdte.trackType;
	}

//
//  Assignment operator
//

CDTocEntry & CDTocEntry::operator=(const CDTocEntry & arg)
	{
	startBlock = arg.startBlock;
	numberOfBlocks = arg.numberOfBlocks;
	trackType = arg.trackType;
	return *this;
	}

