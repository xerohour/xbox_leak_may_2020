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

////////////////////////////////////////////////////////////////////
//
//  VCD Header Classes
//
////////////////////////////////////////////////////////////////////

#ifndef VCDHEADER_H
#define VCDHEADER_H

#include "VCDFile.h"

//
//  VCD Header
//

class VCDHeader
	{
	friend class VCDSubHeader;

	protected:
		VCDFileSystem	*	vcdfs;
		VCDHeaderFile	*	file;
		RequestHandle		rh;

	public:
		BOOL	valid;

		VCDHeader(void) {valid = FALSE; file = NULL;}
		VCDHeader(VCDFileSystem * vcdfs, const DiskItemName & name)
			{
			GenericDiskItem * gdi;

			this->vcdfs = vcdfs;
			if (IS_ERROR(vcdfs->OpenItem(name, FAT_HEADER, gdi)))
				{
				file = NULL;
				valid = FALSE;
				}
			else
				{
				file = (VCDHeaderFile*)gdi;
				valid = TRUE;
				}
			}

		VCDHeader(VCDHeader * header, const DiskItemName & name)
			{
			GenericDiskItem * gdi;

			file = NULL;
			valid = FALSE;
			vcdfs = header->vcdfs;

			if (header->valid)
				{
				if (!IS_ERROR(vcdfs->OpenItem(name, FAT_HEADER, gdi)))
					{
					file = (VCDHeaderFile*)gdi;
					valid = TRUE;
					}
				}
			}

		~VCDHeader(void) {if (file) file->Release();}

		Error Initialize(VCDFileSystem * vcdfs, const DiskItemName & name)
			{
			Error err;
			GenericDiskItem * gdi;

			this->vcdfs = vcdfs;
			if (IS_ERROR(err = vcdfs->OpenItem(name, FAT_HEADER, gdi)))
				{
				file = NULL;
				valid = FALSE;
				}
			else
				{
				file = (VCDHeaderFile*)gdi;
				valid = TRUE;
				}

			GNRAISE(err);
			}
	};

//
//  VCD Sub Header
//

class VCDSubHeader : public VCDHeader
	{
	protected:
		DWORD	offset;
	public:
		VCDSubHeader(void) : VCDHeader() {offset = FALSE;}

		VCDSubHeader(VCDHeader * header, DWORD offset)
			{
			this->file = header->file;
			this->offset = offset;

			valid = header->valid;
			if (valid) file->Obtain();
			}

		VCDSubHeader(VCDSubHeader * header, DWORD offset)
			{
			this->file = header->file;
			this->offset = header->offset + offset;

			valid = header->valid;
			if (valid) file->Obtain();
			}
	};


/*
Segment Play Item Contents byte

				|				|										Description
Position		|	Value		|	Video-CD															|	Super-Video-CD
-----------------------------------------------------------------------------------------------------------------------
bit[0..1]   |	%00		|No MPEG audio data												|do.
				|	%10		|Single channel MPEG audio data								|one MPEG-1 or MPEG-2 audio stream
				|	%01		|Stereo or intensity stereo MPEG audio data				|two MPEG-1 or MPEG-2 audio streams
				|	%11		|dual channel MPEG audio data									|MPEG-2 multi-channel
				|				|																		|
bit[2..4]   |	%000		|No MPEG video data												|do.
				|	%100		|NTSC normal resolution size still picture				|NTSC still picture
				|	%010		|NTSC normal + high resolution size still picture		|reserved
				|	%110		|NTSC normal resolution size motion picture				|NTSC motion picture
				|	%001		|reserved															|reserved
				|	%101		|PAL normal resolution size still picture					|PAL still picture
				|	%011		|PAL normal + high resolution size still picture		|reserved
				|	%111		|PAL normal resolution size motion picture				|PAL motion picture
				|				|																		|
bit[5]		|	%0			|First or only segment of a Play Item						|do.
				|	%1			|Second or later segment of multi segment Play Item	|do.
				|				|																		|
bit[6..7]	|	%00		|reserved															|No OGT sub-stream available
				|	%10		|reserved															|OGT sub-stream 0 available
				|	%01		|reserved															|OGT sub-stream 0 and 1 available
				|	%11		|reserved															|all OGT sub-streams may be available
----------------------------------------------------------------------------------------------------------------------
OGT --- Overlay Graphics and Text
----------------------------------------------------------------------------------------------------------------------
*/

class SegPlayItemContent
	{
	protected:
		BYTE	status;

		//
		//	there are some differences in a segment play item content between
		//	a Video-CD and a Super-Video-CD. So we need a variable that tells us
		//	what kind of CD we are currently using.
		//

		BOOL	isVideoCD;
	public:
		SegPlayItemContent(BOOL isVCD) {status = 0; this->isVideoCD = isVCD;}
		SegPlayItemContent(BYTE b, BOOL isVCD) {this->status = b; this->isVideoCD = isVCD;}

		BOOL HasAudioData(void)   {return (status & 0x03) != 0;}
		BOOL HasVideoData(void)   {return (status & 0x1c) != 0;}
		BOOL HasOverlayData(void)
			{
			if (!isVideoCD)
				return (status & 0xc0) != 0;
			else
				return FALSE;
			}

		BOOL IsMultiChannelAudio(void)
			{
			//
			// Video-CD --> this means dual channel
			// Super-Video CD --> this means multi channel (5.1)
			//
			return (status & 0x03) == 0x3;
			}

		BOOL IsPALVideo(void)     {return (status & 0x10) != 0;}
		BOOL IsStillPicture(void)
			{
			if (isVideoCD)
				return ((status & 0x0c) != 0x0c && (status & 0x0c) != 0x00);
			else // Super-Video CD
				return ((status & 0x1c) == 0x04 || (status & 0x1c) == 0x14);
			}
		BOOL IsFirstSegment(void) {return (status & 0x20) == 0;}
		BOOL IsHiResStill(void)
			{
			if (isVideoCD)
				return (status & 0x0c) == 0x08;
			else
				//
				// note that a Super-Video-CD does not contain any information if a still picture is
				// saved in high resolution or normal resolution. So just return FALSE.
				// (it should not matter because the decoder can distinguish itself between Hi- or
				//	Normal-Resolution pictures).
				//
				return TRUE;
			}
	};

//
//  VCD Info
//

class VCDInfo : public VCDHeader
	{
	protected:
		RequestHandle rh;

	public:
		VCDInfo(void);
		Error Initialize(VCDFileSystem * vcdfs);

		BOOL IsVideoCD(void);

		WORD NumberOfVolumes(void);
		WORD CurrentVolumeNumber(void);

		BOOL IsPALTrack(int track);

		BOOL KarinfoPresent(void);
		BOOL HasClosedCaption(void);
		BOOL StartNextWithID2(void);
		BOOL StartNextWithTrack3(void);
		BOOL HasPSDX(void);

		int OffsetMultiplier(void);
		int MaximumListID(void);
		int MaximumSegmentNumber(void);

		SegPlayItemContent GetSegPlayItemContent(int item);

		Error BuildUniqueKey(BYTE * key);
	};

//
//  VCD Entries
//

class VCDEntries : public VCDHeader
	{
	protected:
		int	numTracks;
		BOOL	isSvcdStd;		// Says that SVCD follows original standard and is not e.g. Chaoji SVCD

	public:
		VCDEntries(void);

		Error Initialize(VCDFileSystem * vcdfs, DVDDiskType diskType);
		BOOL	IsSVCDStandard(void);

		WORD EntriesUsed(void);

		WORD GetEntryTrack(int entry);
		DWORD GetEntrySector(int entry);
	};

//
//  VCD Action List
//

class VCDActionList : public VCDSubHeader
	{
	public:
		VCDActionList(VCDHeader * header, DWORD offset) : VCDSubHeader(header, offset) {}

		virtual WORD ListID(void) = 0;
		virtual WORD PreviousListOffset(void) = 0;
		virtual WORD NextListOffset(void) = 0;
		virtual WORD ReturnListOffset(void) = 0;
	};

//
//  VCD Play LIst
//

class VCDPlayList : public VCDActionList
	{
	public:
		VCDPlayList(VCDHeader * header, DWORD offset) : VCDActionList(header, offset) {}

		int NumberOfItems(void);
		WORD ListID(void);
		WORD PreviousListOffset(void);
		WORD NextListOffset(void);
		WORD ReturnListOffset(void);
		DWORD PlayingTime(void);
		WORD PlayItemWaitTime(void);
		WORD AutoPauseWaitTime(void);
		WORD PlayItemNumber(int item);
	};

//
//  VCD Selection List
//

class VCDSelectionList : public VCDActionList
	{
	public:
		VCDSelectionList(VCDHeader * header, DWORD offset) : VCDActionList(header, offset) {}

		int NumberOfSelections(void);
		WORD BaseOfSelectionNumber(void);
		WORD ListID(void);
		WORD PreviousListOffset(void);
		WORD NextListOffset(void);
		WORD ReturnListOffset(void);
		WORD DefaultListOffset(void);
		WORD TimeOutListOffset(void);
		WORD TimeOutTime(void);
		WORD LoopCount(void);
		BOOL JumpTiming(void);
		WORD PlayItemNumber(void);
		WORD SelectionOffset(int item);

		virtual DWORD PreviousListSelectionArea(void);
		virtual DWORD NextListSelectionArea(void);
		virtual DWORD ReturnListSelectionArea(void);
		virtual DWORD DefaultListSelectionArea(void);
		virtual DWORD SelectionArea(int item);
	};

//
//  VCD Selection List Extended
//

class VCDSelectionListX : public VCDSelectionList
	{
	public:
		VCDSelectionListX(VCDHeader * header, DWORD offset) : VCDSelectionList(header, offset) {}

		virtual DWORD PreviousListSelectionArea(void);
		virtual DWORD NextListSelectionArea(void);
		virtual DWORD ReturnListSelectionArea(void);
		virtual DWORD DefaultListSelectionArea(void);
		virtual DWORD SelectionArea(int item);
	};

//
//  VCD PSD
//

class VCDPSD : public VCDHeader
	{
	protected:
		VCDPSD(VCDFileSystem * vcdfs, KernelString name) : VCDHeader(vcdfs, name) {}
	public:
		VCDPSD(VCDFileSystem * vcdfs) : VCDHeader(vcdfs, "vcd\\psd.vcd") {}
		VCDPSD(void) : VCDHeader() {}

		Error Initialize(VCDFileSystem * vcdfs);
		Error Initialize(VCDFileSystem * vcdfs, KernelString name);
		virtual Error InitializeExtended(VCDFileSystem * vcdfs);

		virtual VCDSelectionList * GetSelectionList(WORD offset);

		VCDPlayList * GetPlayList(WORD offset);

		BOOL IsPlayList(WORD offset);
		virtual BOOL IsSelectionList(WORD offset);
		BOOL IsEndList(WORD offset);
	};

//
//  VCD PSD Extended
//

class VCDPSDX : public VCDPSD
	{
	public:
		VCDPSDX(VCDFileSystem * vcdfs) : VCDPSD(vcdfs, "ext\\psd_x.vcd") {}
		VCDPSDX(void) : VCDPSD() {}

		virtual Error InitializeExtended(VCDFileSystem * vcdfs);

		VCDSelectionList * GetSelectionList(WORD offset);

		BOOL IsSelectionList(WORD offset);
	};

//
//  VCD LOT
//

class VCDLOT : public VCDHeader
	{
	protected:
		VCDLOT(VCDFileSystem * vcdfs, KernelString name) : VCDHeader(vcdfs, name) {}
	public:
		VCDLOT(VCDFileSystem * vcdfs) : VCDHeader(vcdfs, "vcd\\lot.vcd") {}

		WORD ListOffset(int listID);
	};

//
//  VCD LOT Extended
//

class VCDLOTX : public VCDLOT
	{
	public:
		VCDLOTX(VCDFileSystem * vcdfs) : VCDLOT(vcdfs, "ext\\lot_x.vcd") {}
	};

/*
Track Content byte (see also S-VCD specification)

				|				|							Description
Position		|	Value		|	Super-Video-CD
-------------------------------------------------------------------------------
bit[0..1]   |	%00		|No MPEG audio stream
				|	%10		|one MPEG-1 or MPEG-2 audio stream without extension
				|	%01		|two MPEG-1 or MPEG-2 audio streams without extensions
				|	%11		|one MPEG-2 multi-channel audio stream with extension
				|				|
bit[2..4]   |	%000		|No MPEG video data
				|	%100		|reserved
				|	%010		|reserved
				|	%110		|NTSC motion picture
				|	%001		|reserved
				|	%101		|reserved
				|	%011		|reserved
				|	%111		|PAL motion picture
				|				|
bit[5]		|	%0			|reserved
				|	%1			|reserved
				|				|
bit[6..7]	|	%00		|No OGT sub-stream available
				|	%10		|OGT sub-stream 0 available
				|	%01		|OGT sub-stream 0 and 1 available
				|	%11		|all OGT sub-streams may be available
-------------------------------------------------------------------------------
OGT --- Overlay Graphics and Text
-------------------------------------------------------------------------------
*/

class TrackContent
	{
	protected:
		WORD	status;

	public:
		TrackContent(void) {status = 0;}
		TrackContent(WORD w) {this->status = w;}

		BOOL HasTwoAudioStream(void)
			{
			return ((status & 0x03) == 0x2);
			}
		BOOL HasMultiChannelAudioData(void)
			{
			return ((status & 0x03) == 0x3);
			}
	};

//
// Class:
//		VCDTracks
//	Description:
//		The class VCDTracks implements the S-VCD specific TRACKS.SVD file. This file
//		contains content-related information as the Playing time, Video system type,
//		number of audio streams, and number of OGT sub-channels for each MPEG track on the disc.
//
class VCDTracks : public VCDHeader
	{
	protected:
		RequestHandle rh;

		WORD numberOfMpegTracks;				// number of tracks the CD contains
		DWORD contentByteOffset;				// offset to the first Track Content byte
		DWORD playingTimeOffset;				// offset to the first Track Playing time
		WORD GetNumberOfMPEGTracks(void);

	public:
		VCDTracks(void) : VCDHeader() {numberOfMpegTracks = 0;}

		Error Initialize(VCDFileSystem * volume);
		TrackContent GetTrackContent(int track);
		DWORD TrackPlayingTime(DWORD track);
		BOOL IsDualAudioStream(WORD track);
		BOOL IsMultiChannelAudioTrack(WORD track);
	};

//
// Class:
//		VCDSearch
// Description:
//		The class VCDSearch implements the SEARCH.DAT file.
//		SEARCH.DAT contains information about the relation between playing time and sector address.
//		Note that this file is NOT mandatory and therefore there should be another implementation
//		for solving the problem calculating the sector address from a given playing time!
//		MST 09/18/2000
//		Is solved by extracting the PTS directly from the stream. So for now the VCDSearch class
//		is not used but should be left in because of later use.
//		MST 10/04/2000
//
class VCDSearch : public VCDHeader
	{
	protected:
		WORD	numberOfScanPoints;	// contains the number of scan points in the table

	public:
		VCDSearch(void) : VCDHeader() {numberOfScanPoints = 0;}

		Error Initialize(VCDFileSystem * volume);
		DWORD GetSectorAddress(int time);
	};

#endif
