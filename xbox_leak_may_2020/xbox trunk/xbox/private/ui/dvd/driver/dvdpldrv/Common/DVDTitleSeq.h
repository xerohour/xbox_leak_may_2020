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

#ifndef DVDTTSRV_H
#define DVDTTSRV_H

#include "DVDPGCSeq.h"
#include "DVDNavpu.h"
#include "DVDPlayer.h"

#pragma warning(disable : 4250)

class DVDTitleSequencer : public DVDPGCSequencer
	{
	private:
		DVDDomain	domain;  // Please do not use this variable directly but use the Set/Get methods below

	protected:
		DVDOBS			*	vobs;
		DWORD				vobsUniqueKey;
		DVDPGCIT			pgcit;
		DVDPTT			ptt;

		DVDGenericPGCI	*	pgci;
		WORD				pgcn;

		BOOL				resumeAvail;
		BOOL				resumeValid;

		WORD				resumePGCN;
		WORD				resumeTitle;
		WORD				resumePTT;
		WORD				resumePG;

      Error PGCCompleted(DVDGenericPGCI * pgci);

		Error SelectAudioStream(WORD & strm);
		Error SelectSPUStream(WORD & strm, BOOL & enable);

   	virtual Error TransferDiskKey(void) = 0;

		Error SelectProgramChain(void);
		Error AccessRestricted(DVDOBS * vobs, DVDPGCIT pgcit, DVDPTT ptt, WORD vttn, DVDTime time, BOOL & restricted);

		//
		//  Freezing stuff
		//

		class DVDTSFreezeState
			{
			public:
				WORD			pgcn;
				BOOL			resumeAvail;
				BOOL			resumeValid;
				WORD			resumePGCN;
				WORD			resumeTitle;
				WORD			resumePG;

				DVDPGCSFreezeState pgcsFreezeState;
			};

		Error Freeze (DVDTSFreezeState & state);
		Error Defrost(DVDTSFreezeState & state, DVDOBS * vobs, DWORD flags);

	public:
		DVDTitleSequencer (WinPortServer *server, UnitSet units, EventDispatcher* pEventDispatcher);
		virtual ~DVDTitleSequencer(void);

	   Error OpenTitleSet(DVDOBS * vobs, DVDPGCIT pgcit, DVDPTT ptt);
		Error CloseTitleSet(void);

		Error PlayPGCI(DVDGenericPGCI * pgci, BOOL toPause = FALSE);

		Error GetSPRM(int index, WORD & sprm);

	   Error GetCurrentLocation(DVDLocation & loc);
		Error GetCurrentDuration(DVDLocation & loc);

	  	Error GetAvailStreams(BYTE & audioStreams, DWORD & subPictureStreams);

		Error GoPrevProgram(void);

		Error GoProgramChain(WORD pgcn);
		Error GoPartOfTitle(WORD pttn);
		Error GoPrevProgramChain(void);
		Error GoNextProgramChain(void);
		Error GoUpProgramChain(void);

		Error Exit(Error err = GNR_OK);

		virtual Error TimeSearch(DVDTime time);

		virtual Error TimePlay(WORD title, DVDTime time);

   	Error GoVideoTitleSetTitle(WORD title, BOOL toPause);
   	Error GoVideoTitleSetPartOfTitle(WORD title, WORD ptt);

		Error GoAudioTitleSetTrack(int track);

		virtual Error ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time);

		virtual Error ResumeFromSystemSpace(void) = 0;
		virtual Error ResumeFromSystemSpace(DVDOBS * vobs, DVDPGCIT pgcit, DVDPTT ptt);
		virtual Error SaveResumeInformation(void);

		void SetDomain(DVDDomain domain)
			{
			if (domain != this->domain)
				SendEvent(DNE_DOMAIN_CHANGE, domain);
			this->domain = domain;
			}

		DVDDomain GetDomain() { return domain; }
	};

#pragma warning(default : 4250)


#endif
