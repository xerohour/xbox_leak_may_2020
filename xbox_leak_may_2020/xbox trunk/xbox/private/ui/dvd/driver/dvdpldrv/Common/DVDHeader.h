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
//  DVD Header File Information
//
////////////////////////////////////////////////////////////////////

#ifndef DVDHEADER_H
#define DVDHEADER_H

class DVDGenericPGCI;
class DVDGenericCPBI;

#include "DVDFile.h"
#include "DVDNavpu.h"
#include "DVDTime.h"
#include "Config/DiskPlayerFactory.h"

////////////////////////////////////////////////////////////////////
//
//  Cell Playback Information
//
////////////////////////////////////////////////////////////////////

enum CellBlockMode
	{
	CBM_NOT_A_CELL_IN_BLOCK,
	CBM_FIRST_CELL_IN_BLOCK,
	CBM_CELL_IN_BLOCK,
	CBM_LAST_CELL_IN_BLOCK
	};

enum CellBlockType
	{
	CBT_NOT_IN_BLOCK,
	CBT_ANGLE_BLOCK
	};

////////////////////////////////////////////////////////////////////
//
//  Cell Component Information used for Cell Playback Information
//	 for Audio Only Titles
//
////////////////////////////////////////////////////////////////////

enum CellComponent
	{
	AUDIO_ONLY,
	AUDIO_AND_RTI,
	AUDIO_SILENCE
	};

///////////////////////////////////////////////////////////////////////
//
//  Cell Playback Information
//
//	 Description:
//		The class Cell Playback Information describes information
//		necessary to play cells.
//		The DVDCPBI represents two types of Cell Playback Information:
//		AMGM_C_PBI (for Audio Manager Menu) and VTS_C_PBI (for DVD-Video).
//		They differ from each other a little bit. The advantage for
//		merging them together is that we only have one Cell Playback
//		Information that can be allocated statically instead of
//		dynamically.
//
///////////////////////////////////////////////////////////////////////

class DVDCPBI
	{
	protected:
		DWORD		cat;
		DWORD		pbtm;
		DWORD		fvobu_sa;
		DWORD		filvu_ea;
		DWORD		lvobu_sa;
		DWORD		lvobu_ea;
	public:
		DVDCPBI(void);
		~DVDCPBI(void);

		Error Init(DVDHeaderFile * file, DWORD start);

		CellBlockMode BlockMode(void) {return (CellBlockMode)XTBF(30, 2, cat);}
		CellBlockType BlockType(void) {return (CellBlockType)XTBF(28, 2, cat);}
		BOOL Seamless(void) {return XTBF(27, cat);}
		BOOL Interleaved(void) {return XTBF(26, cat);}
		BOOL STCDiscontinuity(void) {return XTBF(25, cat);}
		BOOL SeamlessAngleChange(void) {return XTBF(24, cat);}
		BOOL ContinuousPresentation(void) {return XTBF(22, cat);}
		BOOL AccessRestricted(void) {return XTBF(21, cat);}
		WORD StillTime(void) {return (WORD)XTBF(8, 8, cat);}
		WORD CommandNumber(void) {return (WORD)XTBF(0, 8, cat);}

		DVDTime PresentationTime(void) {return DVDTime(pbtm);}

		DWORD FirstVOBUStart(void) {return fvobu_sa;}
		DWORD FirstILVUEnd(void) {return filvu_ea;}
		DWORD LastVOBUStart(void) {return lvobu_sa;}
		DWORD LastVOBUEnd(void) {return lvobu_ea;}
	};

////////////////////////////////////////////////////////////////////
//
//  Cell Playback Information for Audio Manager Menu
//
//	 Description:
//		Describes the presentation order of Cells in an AMGM_PGC.
//		Use this for Audio Manager Menu.
//
////////////////////////////////////////////////////////////////////

class DVDAMGMCPBI
	{
	protected:
		DWORD		cat;
		DWORD		pbtm;
		DWORD		fvobu_sa;
		DWORD		lvobu_sa;
		DWORD		lvobu_ea;
	public:
		DVDAMGMCPBI(void);
		~DVDAMGMCPBI(void);

		Error Init(DVDHeaderFile * file, DWORD start);

		CellBlockMode BlockMode(void) {return CBM_NOT_A_CELL_IN_BLOCK;}
		CellBlockType BlockType(void) {return CBT_NOT_IN_BLOCK;}
		BOOL Seamless(void) {return XTBF(27, cat);}
		BOOL Interleaved(void) {return FALSE;}
		BOOL STCDiscontinuity(void) {return XTBF(25, cat);}
		BOOL SeamlessAngleChange(void) {return FALSE;}
		BOOL ContinuousPresentation(void) {return TRUE;}
		BOOL AccessRestricted(void) {return FALSE;}
		WORD StillTime(void) {return (WORD)XTBF(8, 8, cat);}
		WORD CommandNumber(void) {return (WORD)XTBF(0, 8, cat);}

		DVDTime PresentationTime(void) {return DVDTime(pbtm);}

		DWORD FirstVOBUStart(void) {return fvobu_sa;}
		DWORD FirstILVUEnd(void) {return 0;}
		DWORD LastVOBUStart(void) {return lvobu_sa;}
		DWORD LastVOBUEnd(void) {return lvobu_ea;}
	};

////////////////////////////////////////////////////////////////////
//
//  Cell Playback Information for Audio Only Title
//
//	 Description:
//		Describes the presentation order of Cells in an ATS_PGC.
//		NOTE: The DVDATSCPBI is only used when this ATS has an
//				AOTT_AOBS, otherwise Video-Cell is defined as ATS_C.
//				And the Video-Cell is the same as DVDCPBI.
//
////////////////////////////////////////////////////////////////////

class DVDATSCPBI
	{
	protected:
		BYTE	ats_ixn;
		BYTE	ats_ty;
		DWORD ats_sa;
		DWORD ats_ea;
	public:
		DVDATSCPBI(void);
		virtual ~DVDATSCPBI(void);

		Error Init(DVDHeaderFile * file, DWORD start);

		BYTE IndexNumber(void) {return ats_ixn;}
		CellComponent Component(void) {return (CellComponent)XTBFB(6, 2, ats_ty);}
		BOOL Spotlight(void) {return (XTBFB(0, 4, ats_ty) == 1);}
		DWORD ATSStart(void) {return ats_sa;}
		DWORD ATSEnd(void) {return ats_ea;}
	};

////////////////////////////////////////////////////////////////////
//
//  Generic Program Chain Information Class
//
//	 Description:
//		The Generic Program Chain Information Class serves as an
//		abstract class as well for the Program Chain Information
//		Class as for the Audio Title Set Program Chain Information
//		Class. Common code is implemented here.
//
////////////////////////////////////////////////////////////////////

class DVDGenericPGCI
	{
	protected:
		DVDDiskPlayerFactory *	factory;
		DVDHeaderFile			*	file;
		RequestHandle				rh;
		DWORD							start;

	public:
		DVDGenericPGCI(DVDDiskPlayerFactory * factory);
		virtual ~DVDGenericPGCI(void);

		virtual Error Init(DVDHeaderFile * file, DWORD start) = 0;
		virtual DWORD GetID(void) {return start;}
		virtual Error GetNumberOfPrograms(WORD & num);
		virtual Error GetNumberOfCells(WORD & num);

		virtual Error GetPresentationTime(DVDTime & time) = 0;

		virtual Error GetUserOperations(DWORD & uops) = 0;
		virtual Error GetAudioStreamControl(int num, WORD & asc) = 0;
		virtual Error GetSubPictureStreamControl(int num, DWORD & spsc) = 0;

		virtual Error GetNextProgramChain(WORD & next) = 0;
		virtual Error GetPreviousProgramChain(WORD & prev) = 0;
		virtual Error GetUpperProgramChain(WORD & upper) = 0;

		virtual Error GetPlaybackMode(BYTE & mode) = 0;
		virtual Error GetStillTimeValue(BYTE & stillTime) = 0;

		virtual Error GetSubPicturePalette(int entry, DWORD & palette) = 0;

		virtual Error GetPreCommands(int & num,  DVDNavigationCommand * & cmds) = 0;
		virtual Error GetPostCommands(int & num, DVDNavigationCommand * & cmds) = 0;
		virtual Error GetCellCommands(DVDNavigationCommand * & cmds) = 0;

		virtual Error GetCellInformation(int cell, DVDCPBI & cpbi) = 0;
		virtual Error GetATSCellInformation(int cell, DVDATSCPBI & atscpbi) = 0;
		virtual Error GetFirstCellOfProgram(int program, WORD & first) = 0;
		virtual Error GetLastCellOfProgram(int program, WORD & last) = 0;
		virtual Error GetProgramOfCell(int cell, WORD & prog) = 0;
		virtual Error GetNumberOfCellsInProgram(int program, WORD & num) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  Program Chain Information Table
//
////////////////////////////////////////////////////////////////////

enum PGCBlockMode
	{
	PBM_NOT_A_PGC_IN_BLOCK,
	PBM_FIRST_PGC_IN_BLOCK,
	PBM_PGC_IN_BLOCK,
	PBM_LAST_PGC_IN_BLOCK
	};

enum PGCBlockType
	{
	PBT_NOT_IN_BLOCK,
	PBT_PARENTAL_BLOCK,
	PBT_AUDIO_CHANNELS_DIFF,
	PBT_AUDIO_COD_CHN_DIFF,
	PBT_AUDIO_CODING_DIFF
	};

//////////////////////////////////////////////////////////////////////
//
//  Part Of Title Class
//
//////////////////////////////////////////////////////////////////////

class DVDPTT
	{
	protected:
		DVDHeaderFile	*	file;
		RequestHandle		rh;
		DWORD					start;

	public:
		DVDPTT(DVDHeaderFile * file, DWORD start) {this->file = file; this->start = start;}
		DVDPTT(void) {file = NULL; start = 0;}

		Error GetNumberOfTitleUnits(WORD & num);
		Error GetNumberOfPartOfTitle(WORD unit, WORD & num);
		Error GetPartOfTitlePGC(WORD unit, WORD ptt, WORD & pgc, WORD & pg);
		Error GetPartAndTitle(WORD pgcn, WORD pgn, WORD title, WORD & ptt);
		Error GetPartOfTitle(WORD title, WORD pgcn, WORD pg, WORD & ptt);
	};

//////////////////////////////////////////////////////////////////////
//
//  Title Search Pointer Table (in VMGI)
//
//////////////////////////////////////////////////////////////////////

class DVDSRPT
	{
	protected:
		DVDHeaderFile	*	file;
		RequestHandle		rh;
		DWORD					start;

	public:
		DVDSRPT(void) { file = NULL; start = 0; }
		DVDSRPT(DVDHeaderFile * file, DWORD start) {this->file = file; this->start = start;}
		DVDSRPT(const DVDSRPT & srpt) { file = srpt.file; start = srpt.start; }
		virtual ~DVDSRPT(void) {}

		virtual Error GetNumberOfTitles(WORD & num);
		virtual Error GetNumberOfPartOfTitle(WORD title, WORD & num) = 0;
		virtual Error GetNumberOfAngles(WORD title, WORD & num) = 0;
		virtual Error GetUOPS(WORD title, DWORD & uops) = 0;

		virtual Error GetTitle(WORD title, WORD & tsn, WORD & ttn) = 0;
		virtual Error GetTitleOfTSTitle(WORD tsn, WORD tsTitle, WORD & title) = 0;

		virtual Error IsOneSequentialPGCTitle(WORD title, BOOL & iospt) = 0;
	};

//////////////////////////////////////////////////////////////////////
//
//  Parental Management Information
//
//////////////////////////////////////////////////////////////////////

class DVDPTLMAI
	{
	protected:
		DVDHeaderFile	*	file;
		RequestHandle		rh;
		DWORD					start;
		WORD					numVTS;

	public:
		DVDPTLMAI(void) {file = NULL; start = 0; numVTS = 0;}
		DVDPTLMAI(DVDHeaderFile * file, DWORD start, WORD numVTS) {this->file = file; this->start = start; this->numVTS = numVTS;}
		DVDPTLMAI(const DVDPTLMAI & ptlmai) {file = ptlmai.file; start = ptlmai.start; numVTS = ptlmai.numVTS;}

		Error GetVMGId(WORD level, WORD & id);
		Error GetVTSId(WORD vtsn, WORD level, WORD & id);
	};

class DVDPTLMAIT
	{
	protected:
		DVDHeaderFile	*	file;
		RequestHandle		rh;
		DWORD					start;

		Error GetNumberOfVTS(WORD & num);
		Error GetNumberOfCountries(WORD & num);

		Error GetCountryCode(WORD entry, WORD & cc);

	public:
		DVDPTLMAIT(DVDHeaderFile * file, DWORD start) {this->file = file; this->start = start;}

		Error GetPTLMAI(WORD countryCode, DVDPTLMAI & ptlmai);
	};

//////////////////////////////////////////////////////////////////////
//
//  Object Set Abstract Base Class
//
//	 Description:
//		Serves as an abstract base class for DVD Video Object Set
//		and DVD Audio Object Set.
//
//////////////////////////////////////////////////////////////////////

class DVDOBS
	{
	protected:
		DVDDataFile *	dataFile;
		RequestHandle	rh;
		static DWORD	uniqueKeys;

	public:
		const DWORD		uniqueKey;

		DVDOBS(DVDDataFile * dataFile) : uniqueKey(++uniqueKeys) {this->dataFile = dataFile;}

		virtual Error GetVideoAttributes(WORD & attrib) = 0;
		virtual Error GetVideoAttributes(VideoStreamFormat & videoStream) = 0;

		virtual Error GetNumberOfAudioStreams(WORD & num) = 0;
		virtual Error GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo) = 0;

		virtual Error GetAudioStreamAttributes(int num, DVDAudioStreamFormat & format) = 0;

		virtual Error GetNumberOfSubPictureStreams(WORD & num) = 0;
		virtual Error GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension) = 0;

		virtual Error GetSubPictureStreamAttributes(int num, DVDSubPictureStreamFormat & format) = 0;

		virtual Error GetParentalID(WORD level, WORD & id) = 0;
		virtual Error HasParentalIDs(BOOL & hasIDs) = 0;

		Error GetDataFile(DVDDataFile * & file) { file = dataFile; GNRAISE_OK; }
	};

//////////////////////////////////////////////////////////////////////
//
//  Video Object Set Abstract Base Class
//
//	 Description:
//		The DVD Video Object Set Class is an abstract class that
//		describes a collection of VOBs. Three types exist according
//		to the attribution of VOB: VMGM_VOBS, VTSM_VOBS, and VTS_VOBS.
//
//////////////////////////////////////////////////////////////////////

class DVDVOBS : public DVDOBS
	{
	protected:
		DVDPTLMAI		ptlmai;

	public:
		DVDVOBS(DVDDataFile * dataFile, const DVDPTLMAI & ptlmai) : DVDOBS(dataFile) {this->ptlmai = ptlmai;}

		virtual Error GetVideoAttributes(WORD & attrib) {GNRAISE_OK;}
		Error GetVideoAttributes(VideoStreamFormat & videoStream);

		virtual Error GetNumberOfAudioStreams(WORD & num) {GNRAISE_OK;}
		virtual Error GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo) {GNRAISE_OK;}

		Error GetAudioStreamAttributes(int num, DVDAudioStreamFormat & format);

		virtual Error GetNumberOfSubPictureStreams(WORD & num) {GNRAISE_OK;}
		virtual Error GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension) {GNRAISE_OK;}

		Error GetSubPictureStreamAttributes(int num, DVDSubPictureStreamFormat & format);

		virtual Error GetParentalID(WORD level, WORD & id) {GNRAISE_OK;}
		Error HasParentalIDs(BOOL & hasIDs);
	};

//////////////////////////////////////////////////////////////////////
//
//  Audio Object Set Abstract Base Class
//
//	 Description:
//		The Audio Object Set Class is an abstract class describing
//		an Audio Object Set. An Audio Object Set (AOBS) is a collection
//		of Audio Objects. An Audio Object (AOB) is the presentation
//		data to be used only for AOTT.
//
//////////////////////////////////////////////////////////////////////

class DVDAOBS : public DVDOBS
	{
	public:
		DVDAOBS(DVDDataFile * dataFile) : DVDOBS(dataFile) {}

		virtual Error GetVideoAttributes(WORD & attrib) {attrib = 0; GNRAISE_OK;}
		virtual Error GetVideoAttributes(VideoStreamFormat & videoStream) {GNRAISE_OK;}

		virtual Error GetNumberOfAudioStreams(WORD & num) {num = 0; GNRAISE_OK;}
		virtual Error GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo)
			{
			attrib = language = extension = applicationInfo = 0;
			GNRAISE_OK;
			}

		virtual Error GetAudioStreamAttributes(int num, DVDAudioStreamFormat & format) {GNRAISE_OK;}

		virtual Error GetNumberOfSubPictureStreams(WORD & num) {num = 0; GNRAISE_OK;}
		virtual Error GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension)
			{
			attrib = language = extension = 0;
			GNRAISE_OK;
			}

		virtual Error GetSubPictureStreamAttributes(int num, DVDSubPictureStreamFormat & format) {GNRAISE_OK;}

		virtual Error GetParentalID(WORD level, WORD & id) {GNRAISE_OK;}
		virtual Error HasParentalIDs(BOOL & hasIDs) { hasIDs = FALSE; GNRAISE_OK; }
	};

//////////////////////////////////////////////////////////////////////
//
//  Video Object Set for Manager Menu
//
//	 Description:
//		Video Object Set for Video Manager Menu is an abstract class
//		for DVDVMGMVOBS and DVDAMGMVOBS.
//
//////////////////////////////////////////////////////////////////////

class DVDMGMVOBS : public DVDVOBS
	{
	protected:
		DVDHeaderFile	*	headerFile;

	public:
		DVDMGMVOBS(DVDHeaderFile * headerFile, DVDDataFile * dataFile, const DVDPTLMAI & ptlmai);
		virtual ~DVDMGMVOBS(void);

		virtual Error GetVideoAttributes(WORD & attrib);

		virtual Error GetNumberOfAudioStreams(WORD & num) {GNRAISE_OK;}
		virtual Error GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo) {GNRAISE_OK;}

		virtual Error GetNumberOfSubPictureStreams(WORD & num);
		virtual Error GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension);

		virtual Error GetParentalID(WORD level, WORD & id);
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Video Title Set Menu VOBS
//
//////////////////////////////////////////////////////////////////////

class DVDVTSMVOBS : public DVDVOBS
	{
	protected:
		DVDHeaderFile	*	headerFile;
		WORD					vtsn;

	public:
		DVDVTSMVOBS(DVDHeaderFile * headerFile, const DVDPTLMAI & ptlmai, DVDDataFile * dataFile, WORD vtsn)
			: DVDVOBS(dataFile, ptlmai) {this->headerFile = headerFile; this->vtsn = vtsn;}

		virtual Error	GetVideoAttributes(WORD & attrib);

		virtual Error	GetNumberOfAudioStreams(WORD & num);
		virtual Error	GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo);

		virtual Error	GetNumberOfSubPictureStreams(WORD & num);
		virtual Error	GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension);

		virtual Error	GetParentalID(WORD level, WORD & id);
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Video Title Set VOBS
//
//////////////////////////////////////////////////////////////////////

class DVDVTSVOBS : public DVDVOBS
	{
	protected:
		DVDHeaderFile	*	headerFile;
		WORD					vtsn;

	public:
		DVDVTSVOBS(DVDHeaderFile * headerFile, const DVDPTLMAI & ptlmai, DVDDataFile * dataFile, WORD vtsn)
			: DVDVOBS(dataFile, ptlmai) {this->headerFile = headerFile; this->vtsn = vtsn;}

		virtual Error	GetVideoAttributes(WORD & attrib);

		virtual Error	GetNumberOfAudioStreams(WORD & num);
		virtual Error GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo);

		virtual Error	GetNumberOfSubPictureStreams(WORD & num);
		virtual Error	GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension);

		virtual Error	GetParentalID(WORD level, WORD & id);
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Title Set Object Set for Audio Only Title
//
//	 Description:
//		When this ATS has an AOTT_AOBS, describes the attributes for
//		AOTT_AOB, else describes the attributes of each Audio stream
//		in the VOB which is used for AOTT_VOB.
//
//////////////////////////////////////////////////////////////////////

class DVDATSAOTTOBS : public DVDAOBS
	{
	protected:
		DVDHeaderFile	*	headerFile;

	public:
		DVDATSAOTTOBS(DVDHeaderFile * headerFile, DVDDataFile * dataFile)
			: DVDAOBS(dataFile) { this->headerFile = headerFile; }

		virtual Error GetAudioStreamAttribute(int num);
	};

#endif
