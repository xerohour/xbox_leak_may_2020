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
//  CDDA Access Classes
//
////////////////////////////////////////////////////////////////////

#ifndef CDDAFILE_H
#define CDDAFILE_H

#include "Library/Files/RootFile.h"
#include "Library/Files/CDVDVolume.h"
#include "NavErrors.h"
#include "EventSender.h"

//
//  Forward declarations
//

class CDDATrack;
class CDDAIterator;
class CDDAFileSystem;

////////////////////////////////////////////////////////////////////
//
//  CDDA File Class
//
////////////////////////////////////////////////////////////////////

class CDDATrack : public GenericFile, public EventSender
	{
	friend class CDDAFileSystem;

	protected:
		GenericFileSystem * baseFS;
		GenericFile * file;
		DWORD lockFlags;

		CDDATrack(CDDAFileSystem * gfs, GenericFileSystem * baseFS, DWORD lockFlags, EventDispatcher* pEventDispatcher);

		virtual Error BuildUniqueKey(void);

	public:
		virtual ~CDDATrack(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Misc. inquiry
		//

		virtual Error GetUniqueKey(BYTE * key);
		virtual Error GetCopyManagementInfo(GenericCopyManagementInfo & gcmi);
		virtual Error GetStartBlock(DWORD & startBlock);

		//
		//  Access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		//
		//  Inquiry
		//

		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetSize(KernelInt64 & size);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Misc.
		//

		virtual Error Flush(void);
		virtual Error TerminateRequest(RequestHandle * rh);
	};

////////////////////////////////////////////////////////////////////
//
//  CDDA File System Iterator Class
//
////////////////////////////////////////////////////////////////////

class CDDAIterator : public GenericFileSystemIterator
	{
	friend class CDDAFileSystem;

	protected:
		GenericFileSystemIterator * gfsi;
		WORD track;

		CDDAIterator(CDDAFileSystem * baseFS, WORD track);

	public:
		virtual ~CDDAIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const;
		virtual Error Clone(CDDAIterator * & cddai) const;

		//
		//  Directory Handling
		//

		virtual Error GoToFirstItem(void);
		virtual Error GoToNextItem(void);
		virtual Error GoToSubDir(GenericFileSystemIterator * & gfsi);
		virtual Error GoToParentDir(GenericFileSystemIterator * & gfsi);

		//
		//  Inquiry
		//

		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetItemType(DiskItemType & type);
		virtual Error GetItemName(DiskItemName & name);
		virtual Error GetItemSize(KernelInt64 & size);
		virtual Error GetNumberOfItems(DWORD & num);
		virtual Error GetCurrentDir(GenericDirectory * & gd);

		//
		//  File Access
		//

		virtual Error OpenItem(DWORD accessType, GenericDiskItem * & gdi);
		virtual Error OpenTrack(DWORD accessType, CDDATrack * & track);
	};

////////////////////////////////////////////////////////////////////
//
//  CDDA File System Class
//
////////////////////////////////////////////////////////////////////

class CDDAFileSystem : public GenericFileSystem, public EventSender
	{
	friend class CDDATrack;
	friend class CDDADirectory;
	friend class CDDAIterator;

	protected:
		GenericFileSystem * baseFS;
		//CDVDVolume * cdvdVolume;

		BOOL	readSubChannel;		// TRUE if we read subchannel, otherwise we fake it
		DWORD	lockFlags;				// Flags used when locking/unlocking blocks

		// DTS-related (set at initialization time)
		BOOL isDTS;
		GenericCopyManagementInfo gcmi;

		//
		//  Factory methods
		//

		virtual Error CreateFile(DWORD flags, GenericFile * & gf);
		virtual Error CreateDirectory(DWORD flags, GenericDirectory * & gd);

		//
		//  Data Access Methods
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		virtual Error BuildUniqueKey(void);

		//
		//  Internal helper functions
		//

		WORD	RBFS(DriveBlock * blocks, DWORD base, DWORD pos1, DWORD num, BOOL base16);
		WORD	RWFS(DriveBlock * blocks, DWORD pos);
		virtual Error InitHeaderAndData(GenericVolume * volume);
		Error InitDTSInfo();

		//
		//  Table of contents handling
		//

		GenericFileSystemIterator * audioTracks[CD_MAX_TOC_ENTRY_NUM];
		WORD	numberOfAudioTracks;

		void	FindAudioTracks(GenericFileSystemIterator * gfsi);
		Error GetBaseIterator(CDDAIterator * cddai, GenericFileSystemIterator * & baseIt);

		//
		//  Item handling
		//

		virtual Error GoToFirstItem(CDDAIterator * cddai);
		virtual Error GoToNextItem(CDDAIterator * cddai);
		virtual Error GoToParentDir(CDDAIterator * cddai, GenericFileSystemIterator * & gfsi);
		virtual Error GetPathName(CDDAIterator * cddai, DiskItemName & name);
		virtual Error GetItemName(CDDAIterator * cddai, DiskItemName & name);
		virtual Error GetItemSize(CDDAIterator * cddai, KernelInt64 & size);
		virtual Error GetNumberOfItems(CDDAIterator * cddai, DWORD & num);

	public:
		CDDAFileSystem(EventDispatcher* pEventDispatcher);
		virtual ~CDDAFileSystem(void);

		virtual Error Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile);

		//
		//  Factory Methods
		//

		virtual Error CreateIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Volume Information
		//

		virtual Error GetUniqueKey(BYTE * key);
		virtual Error GetDiskType(PhysicalDiskType & type);
		virtual Error GetLogicalBaseBlock(DWORD & logicalBaseBlock);
		Error GetDTSInfo(BOOL & isDTS, GenericCopyManagementInfo & gcmi){isDTS = this->isDTS; gcmi = this->gcmi; GNRAISE_OK;}
		//
		//  Drive "Pass Through" Functions
		//

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);
		virtual Error GetCurrentDriveBlockSize(DWORD & size);

		virtual Error TerminateRequest(RequestHandle * rh);
		virtual Error Flush(void);

		virtual Error GetDriveName(KernelString & name);
		virtual Error SpinUpDrive(void);
		virtual Error SpinDownDrive(void);
	};

#endif
