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
//  DVD Audio Still Video Module
//
//	 Declares classes for use with Audio Titles with
//	 Audio Still Video Sets on a DVD-Audio
//
////////////////////////////////////////////////////////////////////

#ifndef DVDAUDIOSTILLVIDEO_H
#define DVDAUDIOSTILLVIDEO_H

#include "DVDFile.h"
#include "DVDHeader.h"


////////////////////////////////////////////////////////////////////
//
//  DVD Video Object Set for Audio Still Video Set Class
//
//	 Description:
//		This class describes an Audio Still Video Object Set. ASVOB
//		is the Presentation data for one Audio Still Video (ASV) and
//		contains Highlight Information data for Button, Sub-Picture
//		data for Button and Still picture data.
//
////////////////////////////////////////////////////////////////////

class DVDASVOBS : public DVDVOBS
	{
	protected:
		DVDHeaderFile * headerFile;

	public:
		DVDASVOBS(DVDHeaderFile * headerFile, DVDDataFile * dataFile, const DVDPTLMAI & ptlmai);
		virtual ~DVDASVOBS(void);

		Error GetVideoAttributes(WORD & attrib) {GNRAISE(GNR_UNIMPLEMENTED);}

		Error GetNumberOfAudioStreams(WORD & num) {GNRAISE(GNR_UNIMPLEMENTED);}
		Error GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo) {GNRAISE(GNR_UNIMPLEMENTED);}

		Error GetNumberOfSubPictureStreams(WORD & num) {GNRAISE(GNR_UNIMPLEMENTED);}
		Error GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension) {GNRAISE(GNR_UNIMPLEMENTED);}

		Error GetNumberOfStillVideosInUnit(int item, BYTE & num);
		Error GetStillVideoAttribute(int item, WORD & attrib);

		Error GetParentalID(WORD level, WORD & id) {GNRAISE(GNR_UNIMPLEMENTED);}
	};

////////////////////////////////////////////////////////////////////
//
//  DVD Audio Still Video Set Class
//
//	 Description:
//		This class describes an Audio Still Video Set. An ASVS is a
//		logical unit to define the collection of Audio Still Video
//		(ASV) to be presented together with Audio data.
//
////////////////////////////////////////////////////////////////////

class DVDASVS
	{
	protected:
		RequestHandle		rh;
		DVDASVOBS		*	asVobs;
		DVDHeaderFile	*	hfile;
		DVDDataFile		*	dfile;
		DVDFileSystem	*	dvdfs;
		DVDPTLMAI			ptlmai;

	public:
		DVDASVS(void);
		virtual ~DVDASVS(void);

		Error Init(DVDFileSystem * dvdfs);

		Error GetASVOBS(DVDASVOBS * & asVobs);
		Error GetNumberOfASVU(WORD & num);
	};


#endif	// DVDAUDIOSTILLVIDEO_H
