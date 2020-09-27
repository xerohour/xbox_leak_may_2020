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
//  CD Access Classes
//
////////////////////////////////////////////////////////////////////

#ifndef CDFILE_H
#define CDFILE_H

#include "library/Files/CDVDVolume.h"
#include "library/Files/ISOFile.h"

//
//  Forward declarations
//

class CDTrack;
class CDSession;
class CDIterator;
class CDFileSystem;

////////////////////////////////////////////////////////////////////
//
//  CDTrack Class
//
////////////////////////////////////////////////////////////////////

class CDTrack : public GenericFile
	{
	friend class CDFileSystem;

	protected:
		CDFileSystem * cdfs;
		CDIterator * cdi;

		DWORD startBlock;
		CDTrackType trackType;

		CDTrack(CDFileSystem * cdfs);

		virtual Error BuildUniqueKey(void);

	public:
		CDTrack(const CDTrack & track);
		virtual ~CDTrack(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Misc. inquiry
		//

		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetSize(KernelInt64 & size);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);
		virtual Error GetCopyManagementInfo(GenericCopyManagementInfo & gcmi);

		//
		//  Access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
	};

////////////////////////////////////////////////////////////////////
//
//  CD Session Class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

class CDSession : public GenericDirectory
	{
	friend class CDFileSystem;

	protected:
		CDFileSystem * cdfs;
		CDIterator * cdi;

		CDSession(CDFileSystem * cdfs);

	public:
		CDSession(const CDSession & cds);
		virtual ~CDSession(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Inquiry
		//

		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Item handling
		//

		virtual Error CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi);
		virtual Error DeleteItem(GenericFileSystemIterator * gfsi);

		virtual Error GetNumberOfItems(DWORD & num);
		virtual Error FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi);
	};

#endif

////////////////////////////////////////////////////////////////////
//
//  CD Iterator Class
//
////////////////////////////////////////////////////////////////////

class CDIterator : public GenericFileSystemIterator
	{
	friend class CDFileSystem;

	protected:
		CDFileSystem * cdfs;
		WORD session;		// The session we are currently in (0 is the root dir above all sessions)
		WORD track;			// The track (item) we point to (starting with 0)

		CDIterator(CDFileSystem * cdfs);

		virtual BOOL Equals(GenericFileSystemIterator * gfsi);

	public:
		CDIterator(const CDIterator & cdi);
		virtual ~CDIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const;
		virtual Error Clone(CDIterator * & cdi) const;

		//
		//  Directory Handling
		//

		virtual Error	GoToFirstItem(void);
		virtual Error	GoToNextItem(void);
		virtual Error	GoToSubDir(GenericFileSystemIterator * & gfsi);
		virtual Error	GoToParentDir(GenericFileSystemIterator * & gfsi);

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
	};

////////////////////////////////////////////////////////////////////
//
//  CD File System Class
//
////////////////////////////////////////////////////////////////////

class CDFileSystem : public GenericFileSystem
	{
	friend class CDTrack;
	friend class CDSession;
	friend class CDIterator;

	protected:
		CDVDVolume * cdvdVolume;

		//
		//  Information about a session
		//

		class CDSessionInfo
			{
			public:
				CDTocEntry			*	toc;				// TOC entries of session
				WORD						numTracks;		// Number of TOC entries in session
			};

		//
		//  Volume information
		//

		WORD					numSessions;			// Number of sessions of disk
		CDSessionInfo	*	sessions;				// Session information array

		//
		//  Factory Methods
		//

		virtual Error CreateIterator(CDIterator * & cdi);
		virtual Error CreateIterator(GenericFileSystemIterator * & gfsi);
		virtual Error CreateFile(DWORD flags, GenericFile * & gf);
		virtual Error CreateDirectory(DWORD flags, GenericDirectory * & gd);

		//
		//  Internal functions
		//

		DWORD GetStartBlock(CDIterator * cdi);
		Error BuildUniqueKey(void);
		virtual Error InitHeaderAndData(GenericVolume * volume);

		//
		//  Item Handling
		//

		Error GoToFirstItem(CDIterator * cdi);
		Error GoToNextItem(CDIterator * cdi);
		Error GoToSubDir(CDIterator * cdi, GenericFileSystemIterator * & gfsi);
		Error GoToParentDir(CDIterator * cdi, GenericFileSystemIterator * & gfsi);

		Error GetItemType(CDIterator * cdi, DiskItemType & type);
		Error GetItemName(CDIterator * cdi, DiskItemName & name);
		Error GetPathName(CDIterator * cdi, DiskItemName & name);
		Error GetItemSize(CDIterator * cdi, KernelInt64  & size);

		Error GetDir(CDIterator * cdi, GenericDirectory * & gd);
		Error GetNumberOfItems(CDIterator * cdi, DWORD & num);
		CDTrackType	GetTrackType(CDIterator * cdi);

	public:
		CDFileSystem(void);
		virtual ~CDFileSystem(void);

		virtual Error Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile);

		virtual Error GetExtItemInfo(GenericFileSystemIterator * gfsi, ExtItemInfo & info);
	};

#endif
