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
// VCD Header Classes
//
////////////////////////////////////////////////////////////////////

#include "Library/Common/Prelude.h"
#include "VCDHeader.h"

////////////////////////////////////////////////////////////////////
//
//  VCD Info Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDInfo::VCDInfo(void) : VCDHeader()
	{
	}

//
//  Initialize
//

Error VCDInfo::Initialize(VCDFileSystem * vcdfs)
	{
	Error error = GNR_OK;

	//
	// check for Video CD
	//
	error = VCDHeader::Initialize(vcdfs, "vcd\\info.vcd");
	if(IS_ERROR(error))
		{
		//
		// check for Super-Video CD
		//
		error = VCDHeader::Initialize(vcdfs, "svcd\\info.svd");
		if(IS_ERROR(error))
			{
			//
			// again check for Super-Video CD (some chinese Super-VCDs seem to ignore the S-VCD system specification and
			// have their own directory (\\hqvcd\\ instead of \\svcd\\)!
			//
			error = VCDHeader::Initialize(vcdfs, "hqvcd\\info.vcd");
			if(IS_ERROR(error))
				{
				GenericFileSystemIterator * root;
				GenericFileSystemIterator * result;

				GNREASSERT(vcdfs->CreateIterator(root));
				error = vcdfs->FindItem("mpegav\\music01.dat", DIT_FILE, FIF_NONE, root, result);
				delete root;
				delete result;
				}
			}
		}

	GNRAISE(error);
	}

//
// Function: IsVideoCD
//
//	Returns TRUE if a Video-CD is used or FALSE if a Super-Video-CD is used.
//	The detection is done with byte 1 to 8 from the info.vcd (info.svd) file. These
//	bytes contain the system identification ("VIDEO_CD", "SUPERVCD", or "HQ-VCD  ").
//

BOOL VCDInfo::IsVideoCD(void)
	{
	DWORD lowBytes;

	if (file)
		{
		file->ReadDWord(0, lowBytes, &rh);

		//
		// only check the first four letters. This should be enough
		// 0x56 - 'V'	0x49 - 'I'	0x44 - 'D'	0x45 - 'E'
		//

		if (XTBF(0, 8, lowBytes) == 0x45 && XTBF(8, 8, lowBytes) == 0x44
			&& XTBF(16, 8, lowBytes) == 0x49 && XTBF(24, 8, lowBytes) == 0x56)
			{
			//
			// Video-CD
			//

			return TRUE;
			}
		else
			//
			// no Video-CD, so we assume Super-Video CD
			//

			return FALSE;
		}
	else
		//
		// default value is Video-CD
		//

		return TRUE;
	}

WORD VCDInfo::NumberOfVolumes(void)
	{
	WORD w;

	if (file)
		file->ReadWord(26, w, &rh);
	else
		w = 1;

	return w;
	}

WORD VCDInfo::CurrentVolumeNumber(void)
	{
	WORD w;

	if (file)
		file->ReadWord(28, w, &rh);
	else
		w = 0;

	return w;
	}

BOOL VCDInfo::IsPALTrack(int track)
	{
	BYTE b;

	if (file)
		{
		track -= 2;

		file->ReadByte(30 + track / 8, b, &rh);

		return XTBF(track & 7, b);
		}
	else
		return FALSE;
	}

BOOL VCDInfo::KarinfoPresent(void)
	{
	BYTE b;

	if (file)
		{
		file->ReadByte(43, b, &rh);

		return XTBF(0, b);
		}
	else
		return FALSE;
	}

BOOL VCDInfo::HasClosedCaption(void)
	{
	BYTE b;

	if (file)
		{
		file->ReadByte(43, b, &rh);

		return XTBF(4, b);
		}
	else
		return FALSE;
	}

BOOL VCDInfo::StartNextWithID2(void)
	{
	BYTE b;

	if (file)
		{
		file->ReadByte(43, b, &rh);

		return XTBF(5, b);
		}
	else
		return FALSE;
	}

BOOL VCDInfo::StartNextWithTrack3(void)
	{
	BYTE b;

	if (file)
		{
		file->ReadByte(43, b, &rh);

		return XTBF(6, b);
		}
	else
		return FALSE;
	}

BOOL VCDInfo::HasPSDX(void)
	{
	BYTE b;

	if (file)
		{
		file->ReadByte(43, b, &rh);

		return XTBF(7, b);
		}
	else
		return FALSE;
	}

int VCDInfo::OffsetMultiplier(void)
	{
	BYTE b;

	if (file)
		file->ReadByte(51, b, &rh);
	else
		b = 1;

	return b;
	}

int VCDInfo::MaximumListID(void)
	{
	WORD w;

	if (file)
		file->ReadWord(52, w, &rh);
	else
		w = 0;

	return w;
	}

int VCDInfo::MaximumSegmentNumber(void)
	{
	WORD w;

	if (file)
		file->ReadWord(54, w, &rh);
	else
		w = 0;

	return w;
	}

SegPlayItemContent VCDInfo::GetSegPlayItemContent(int item)
	{
	BYTE b;

	if (file)
		file->ReadByte(55 + item, b, &rh);
	else
		b = 0;

	return SegPlayItemContent(b, IsVideoCD());
	}

Error VCDInfo::BuildUniqueKey(BYTE * key)
	{
	int i;

	if (file)
		return file->GetUniqueKey(key);
	else
		{
		for (i=0; i<8; i++)
			key[i] = 0;
		GNRAISE_OK;
		}
	}

////////////////////////////////////////////////////////////////////
//
//  VCD Entries Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDEntries::VCDEntries(void) : VCDHeader()
	{
	}

//
//  Initialize
//

Error VCDEntries::Initialize(VCDFileSystem * vcdfs, DVDDiskType diskType)
	{
	Error err;
	DiskItemName name;

	//
	// check for Video-CD entries file
	//

	switch (diskType)
		{
		case DDT_VIDEO_CD:
			//
			//  Check if entries.vcd can be found
			//

			isSvcdStd = FALSE;
			if (IS_ERROR(err = VCDHeader::Initialize(vcdfs, "vcd\\entries.vcd")))
				{
				GenericFileSystemIterator * root;
				GenericFileSystemIterator * result;

				//
				//  If there is no entries.vcd, then look if there are just track files (Yamaha crap)
				//

				GNREASSERT(vcdfs->CreateIterator(root));
				numTracks = 0;
				name = KernelString("mpegav\\music") +	KernelString(numTracks + 1, 2) + KernelString(".dat");

				while (!IS_ERROR(vcdfs->FindItem(name, DIT_FILE, FIF_NONE, root, result)))
					{
					delete result;
					numTracks++;
					}
				delete root;

				if (numTracks)
					err = GNR_OK;
				}
			break;
		case DDT_SUPER_VIDEO_CD:
			//
			//  Check for Super-Video CD entries file
			//

			if(!IS_ERROR(err = VCDHeader::Initialize(vcdfs, "svcd\\entries.svd")))
				{
				isSvcdStd = TRUE;
				}
			else
				{
				if (!IS_ERROR(err = VCDHeader::Initialize(vcdfs, "hqvcd\\entries.vcd")))
					{
					isSvcdStd = FALSE;
					}
				}
			break;
		default:
			err = GNR_INVALID_PARAMETERS;
		}

	GNRAISE(err);
	}

//
//	 returns TRUE if the disc follows the S-VCD standard, else returns FALSE (e.g. CVDs)
//

BOOL VCDEntries::IsSVCDStandard(void)
	{
	return isSvcdStd;
	}

WORD VCDEntries::EntriesUsed(void)
	{
	WORD w;

	if (file)
		file->ReadWord(10, w, &rh);
	else
		w = (WORD)numTracks;


	return w;
	}

WORD VCDEntries::GetEntryTrack(int entry)
	{
	BYTE b;

	if (file)
		{
		file->ReadByte(12 + 4 * (entry - 1), b, &rh);

		b = (b >> 4) * 10 + (b & 0x0f);
		}
	else
		b = entry + 1;

	return b;
	}

DWORD VCDEntries::GetEntrySector(int entry)
	{
	DWORD a;

	if (file)
		{
		if (!IS_ERROR(file->ReadSectorAddress(13 + 4 * (entry - 1), a, &rh)))
			return a;
		}

	return 0;
	}

////////////////////////////////////////////////////////////////////
//
//  Playlist Class
//
////////////////////////////////////////////////////////////////////

int VCDPlayList::NumberOfItems(void)
	{
	BYTE b;

	file->ReadByte(offset + 1, b, &rh);

	return b;
	}

WORD VCDPlayList::ListID(void)
	{
	WORD w;

	file->ReadWord(offset + 2, w, &rh);

	return w;
	}

WORD VCDPlayList::PreviousListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 4, w, &rh);

	return w;
	}

WORD VCDPlayList::NextListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 6, w, &rh);

	return w;
	}

WORD VCDPlayList::ReturnListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 8, w, &rh);

	return w;
	}

DWORD VCDPlayList::PlayingTime(void)
	{
	WORD w;

	file->ReadWord(offset + 10, w, &rh);

	return (DWORD)w * 5;
	}

static WORD TranslateWaitTime(BYTE b)
	{
	if (b <= 60)
		return b;
	else if (b < 254)
		return (WORD)(b - 60) * 10 + 60;
	else if (b == 254)
		return 2000;
	else
		return 65535;
	}

WORD VCDPlayList::PlayItemWaitTime(void)
	{
	BYTE b;

	file->ReadByte(offset + 12, b, &rh);

	return TranslateWaitTime(b);
	}

WORD VCDPlayList::AutoPauseWaitTime(void)
	{
	BYTE b;

	file->ReadByte(offset + 13, b, &rh);

	return TranslateWaitTime(b);
	}

WORD VCDPlayList::PlayItemNumber(int item)
	{
	WORD w;

	file->ReadWord(offset + 14 + 2 * (item - 1), w, &rh);

	return w;
	}

////////////////////////////////////////////////////////////////////
//
//  Selection List Class
//
////////////////////////////////////////////////////////////////////

int VCDSelectionList::NumberOfSelections(void)
	{
	BYTE b;

	file->ReadByte(offset + 2, b, &rh);

	return b;
	}

WORD VCDSelectionList::BaseOfSelectionNumber(void)
	{
	BYTE b;

	file->ReadByte(offset + 3, b, &rh);

	return b;
	}

WORD VCDSelectionList::ListID(void)
	{
	WORD w;

	file->ReadWord(offset + 4, w, &rh);

	return w;
	}

WORD VCDSelectionList::PreviousListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 6, w, &rh);

	return w;
	}

WORD VCDSelectionList::NextListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 8, w, &rh);

	return w;
	}

WORD VCDSelectionList::ReturnListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 10, w, &rh);

	return w;
	}

WORD VCDSelectionList::DefaultListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 12, w, &rh);

	return w;
	}

WORD VCDSelectionList::TimeOutListOffset(void)
	{
	WORD w;

	file->ReadWord(offset + 14, w, &rh);

	return w;
	}

WORD VCDSelectionList::TimeOutTime(void)
	{
	BYTE b;

	file->ReadByte(offset + 16, b, &rh);

	return TranslateWaitTime(b);
	}

WORD VCDSelectionList::LoopCount(void)
	{
	BYTE b;

	file->ReadByte(offset + 17, b, &rh);

	return b & 0x7f;
	}

BOOL VCDSelectionList::JumpTiming(void)
	{
	BYTE b;

	file->ReadByte(offset + 17, b, &rh);

	return XTBF(7, b);
	}

WORD VCDSelectionList::PlayItemNumber(void)
	{
	WORD w;

	file->ReadWord(offset + 18, w, &rh);

	return w;
	}

WORD VCDSelectionList::SelectionOffset(int item)
	{
	WORD w;

	file->ReadWord(offset + 20 + 2 * (item - BaseOfSelectionNumber()), w, &rh);

	return w;
	}

DWORD VCDSelectionList::PreviousListSelectionArea(void) {return 0;}
DWORD VCDSelectionList::NextListSelectionArea(void) {return 0;}
DWORD VCDSelectionList::ReturnListSelectionArea(void) {return 0;}
DWORD VCDSelectionList::DefaultListSelectionArea(void) {return 0;}
DWORD VCDSelectionList::SelectionArea(int item) {return 0;}

////////////////////////////////////////////////////////////////////
//
//  Extended Selection List Class
//
////////////////////////////////////////////////////////////////////

DWORD VCDSelectionListX::PreviousListSelectionArea(void)
	{
	DWORD d;

	file->ReadDWord(offset + 20 + 2 * (NumberOfSelections()) + 0, d, &rh);

	return d;
	}

DWORD VCDSelectionListX::NextListSelectionArea(void)
	{
	DWORD d;

	file->ReadDWord(offset + 20 + 2 * (NumberOfSelections()) + 4, d, &rh);

	return d;
	}

DWORD VCDSelectionListX::ReturnListSelectionArea(void)
	{
	DWORD d;

	file->ReadDWord(offset + 20 + 2 * (NumberOfSelections()) + 8, d, &rh);

	return d;
	}

DWORD VCDSelectionListX::DefaultListSelectionArea(void)
	{
	DWORD d;

	file->ReadDWord(offset + 20 + 2 * (NumberOfSelections()) + 12, d, &rh);

	return d;
	}

DWORD VCDSelectionListX::SelectionArea(int item)
	{
	DWORD d;

	file->ReadDWord(offset + 20 + 2 * (NumberOfSelections()) + 16 + 4 * (item - BaseOfSelectionNumber()), d, &rh);

	return d;
	}

////////////////////////////////////////////////////////////////////
//
//  VCD PSD Class
//
////////////////////////////////////////////////////////////////////

VCDSelectionList * VCDPSD::GetSelectionList(WORD offset)
	{
	return new VCDSelectionList(this, offset);
	}

VCDPlayList * VCDPSD::GetPlayList(WORD offset)
	{
	return new VCDPlayList(this, offset);
	}

Error VCDPSD::Initialize(VCDFileSystem * vcdfs)
	{
	Error error = GNR_OK;

	//
	// check for Video CD Play Sequence Descriptor file
	//
	error = VCDHeader::Initialize(vcdfs, "vcd\\psd.vcd");
	if(IS_ERROR(error))
		{
		//
		// check for Super-Video CD Play Sequence Descriptor file
		//
		error = VCDHeader::Initialize(vcdfs, "svcd\\psd.svd");
		if(IS_ERROR(error))
			{
			error = VCDHeader::Initialize(vcdfs, "hqvcd\\psd.vcd");
			}
		}

	GNRAISE(error);
	}

Error VCDPSD::Initialize(VCDFileSystem * vcdfs, KernelString name)
	{
	GNRAISE(VCDHeader::Initialize(vcdfs, name));
	}

Error VCDPSD::InitializeExtended(VCDFileSystem * vcdfs)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

BOOL VCDPSD::IsPlayList(WORD offset)
	{
	BYTE b;

	file->ReadByte(offset, b, &rh);

	return b == 0x10;
	}

BOOL VCDPSD::IsSelectionList(WORD offset)
	{
	BYTE b;

	file->ReadByte(offset, b, &rh);

	return b == 0x18;
	}

BOOL VCDPSD::IsEndList(WORD offset)
	{
	BYTE b;

	file->ReadByte(offset, b, &rh);

	return b == 0x1f;
	}

////////////////////////////////////////////////////////////////////
//
//  Extended VCD PSD Class
//
////////////////////////////////////////////////////////////////////

VCDSelectionList * VCDPSDX::GetSelectionList(WORD offset)
	{
	BYTE b;

	file->ReadByte(offset, b, &rh);

	if (b == 0x1a)
		return new VCDSelectionListX(this, offset);
	else if (b == 0x18)
		return new VCDSelectionList(this, offset);
	else
		return NULL;
	}

Error VCDPSDX::InitializeExtended(VCDFileSystem * vcdfs)
	{
	GNRAISE(VCDPSD::Initialize(vcdfs, "ext\\psd_x.vcd"));
	}

BOOL VCDPSDX::IsSelectionList(WORD offset)
	{
	BYTE b;

	file->ReadByte(offset, b, &rh);

	return b == 0x1a || b == 0x18;
	}

////////////////////////////////////////////////////////////////////
//
//  VCD LOT Class
//
////////////////////////////////////////////////////////////////////

WORD VCDLOT::ListOffset(int listID)
	{
	WORD w;

	file->ReadWord(listID * 2, w, &rh);

	return w;
	}

////////////////////////////////////////////////////////////////////
//
//  VCD Tracks Class
//
////////////////////////////////////////////////////////////////////

Error VCDTracks::Initialize(VCDFileSystem * vcdfs)
	{
	DiskItemName name;
	GenericFileSystemIterator * root;
	GenericFileSystemIterator * result;
	Error error = GNR_OK;

	//
	// check for Super Video CD Tracks file
	// (only available on S-VCDs not on VCDs, see specification for
	//	more details)
	//
	error = VCDHeader::Initialize(vcdfs, "svcd\\tracks.svd");

	if(!IS_ERROR(error))
		{
		numberOfMpegTracks = GetNumberOfMPEGTracks();

		playingTimeOffset = 11;
		contentByteOffset = 11 + 3 * numberOfMpegTracks;
		}
	else	// No tracks.svd file was found.
		{
		numberOfMpegTracks = 1;
		GNREASSERT(vcdfs->CreateIterator(root));

		do
			{
			name = KernelString("mpeg2\\avseq") + KernelString(numberOfMpegTracks + 1, 2) + KernelString(".mpg");
			if (!IS_ERROR(error = vcdfs->FindItem(name, DIT_FILE, FIF_NONE, root, result)))
				{
				numberOfMpegTracks++;
				delete result;
				}
			}
		while (!IS_ERROR(error));
		delete root;
		}

	GNRAISE(error);
	}

WORD VCDTracks::GetNumberOfMPEGTracks(void)
	{
	BYTE b;

	//
	// read number of MPEG tracks
	//
	if (file)
		file->ReadByte(10, b, &rh);
	else
		b = 0;

	return b;
	}

TrackContent VCDTracks::GetTrackContent(int track)
	{
	BYTE b;

	if (file)
		file->ReadByte(contentByteOffset + (track - 1), b, &rh);
	else
		b = 0;

	return TrackContent(b);
	}

BOOL VCDTracks::IsDualAudioStream(WORD track)
	{
	BYTE b;

	if (file)
		file->ReadByte(contentByteOffset + (track - 1), b, &rh);
	else
		b = 0;

	return ((b & 0x03) == 0x2);
	}

BOOL VCDTracks::IsMultiChannelAudioTrack(WORD track)
	{
	BYTE b;

	if (file)
		file->ReadByte(contentByteOffset + (track - 1), b, &rh);
	else
		b = 0;

	return ((b & 0x03) == 0x3);
	}

DWORD VCDTracks::TrackPlayingTime(DWORD track)
	{
	DWORD offset;
	BYTE minutes, seconds;

	if (file)
		{
		offset = playingTimeOffset + 3 * (track - 1);
		file->ReadByte(offset, minutes, &rh);
		file->ReadByte(offset+1, seconds, &rh);
		minutes = ((minutes / 16) * 10) + (minutes % 16);
		seconds = ((seconds / 16) * 10) + (seconds % 16);
		}
	else
		minutes = seconds = 0;

	return minutes * 60 + seconds;
	}

////////////////////////////////////////////////////////////////////
//
//  VCDSearch Class
//
////////////////////////////////////////////////////////////////////

Error VCDSearch::Initialize(VCDFileSystem * vcdfs)
	{
	Error error = GNR_OK;

	//
	// check for Super Video CD Search file
	// (only available on S-VCDs not on VCDs, see specification for
	//	more details)
	//
	error = VCDHeader::Initialize(vcdfs, "svcd\\search.dat");
	if (!IS_ERROR(error))
		{
		if (file)
			file->ReadWord(10, numberOfScanPoints, &rh);
		else
			numberOfScanPoints = 0;
		}

	GNRAISE(error);
	}

//
// Function:
//		GetSectorAddress
// Parameter:
//		time: time in seconds
//	Return Value:
//		Sector address. If the function fails it returns zero.
// Description:
//		Returns the sector address for the given time or zero if the
//		function fails.
//
DWORD VCDSearch::GetSectorAddress(int time)
	{
	return 0;
	}
