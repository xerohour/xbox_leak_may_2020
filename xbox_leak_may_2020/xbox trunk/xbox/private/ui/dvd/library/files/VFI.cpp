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
//  Viona File Interface Implementation
//
////////////////////////////////////////////////////////////////////

#include "VFI.h"
#include <string.h>


//
//  Logging of VFI calls
//

#define LOGVFI_NONE			0x0000		// No VFI logging at all
#define LOGVFI_GENERAL		0x0001		//
#define LOGVFI_ITERATOR		0x0002
#define LOGVFI_ITEM			0x0004
#define LOGVFI_FILE			0x0008
#define LOGVFI_VOLUME		0x0010
#define LOGVFI_EXIT			0x0020		// Log termination of VFI call
#define LOGVFI_ALL			0xffff		// Log everything

#define LOGVFICALLS LOGVFI_NONE	 		// Use the LOGVFI_* flags above to configure logging

////////////////////////////////////////////////////////////////////
//
//  ST20LITE specific
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Windows specific
//
////////////////////////////////////////////////////////////////////


#if LOGVFICALLS
#define LOGVFI(text, type) if (LOGVFI_##type & (LOGVFICALLS)) DP("VFI_%s", #text)
#define LOGVFIEXIT(text, type, err) if ( (LOGVFI_##type & (LOGVFICALLS)) && (LOGVFI_EXIT & (LOGVFICALLS)) ) DP("VFI_%s END (%08lx)", #text, err)
#else
#define LOGVFI(text, type)
#define LOGVFIEXIT(text, type, err)
#endif



////////////////////////////////////////////////////////////////////
//
//  Global variables
//
////////////////////////////////////////////////////////////////////

GenericFileSystem * pRootFileSystem = NULL;

//
//  Initialize VFI
//  Parameter must be a valid root file system. This is not tested.
//

Error VFI_Init(GenericFileSystem * pRootSystem)
	{
	Error err;

	LOGVFI(Init, GENERAL);

	//
	// Test parameters
	//

	if (pRootSystem)
		{
		pRootFileSystem = pRootSystem;
		pRootFileSystem->Obtain();
		err = GNR_OK;
		}
	else
		err = GNR_INVALID_PARAMETERS;

	LOGVFIEXIT(Init, GENERAL, err);

	GNRAISE(err);
	}

//
//  Clean up the VFI Interface
//  Objects still not given back to the interface are NOT deleted..
//

Error VFI_Cleanup()
	{
	Error err;

	LOGVFI(Cleanup, GENERAL);

	if (pRootFileSystem)
		{
		pRootFileSystem->Release();
		pRootFileSystem = NULL;
		err = GNR_OK;
		}
	else
		err = GNR_OBJECT_INVALID;

	LOGVFIEXIT(Cleanup, GENERAL, err);

	GNRAISE(err);
	}

//
//  Create Iterator to root file system
//

DLLCALL Error WINAPI VFI_CreateIterator(HIter & hIterator)
	{
	GenericFileSystemIterator * pIterator = NULL;
	Error err = GNR_OBJECT_INVALID;
	hIterator = (HIter) NULL;


	LOGVFI(CreateIterator, ITERATOR);

	//
	// Test internal state
	//

	if (pRootFileSystem)
		{
		//
		// Create iterator
		//

		err = pRootFileSystem->CreateIterator(pIterator);
		hIterator = (HIter) pIterator;
		}

	LOGVFIEXIT(CreateIterator, ITERATOR, err);

	GNRAISE(err);
	}

//
//  Duplicate existing iterator
//

DLLCALL Error WINAPI VFI_CloneIterator(HIter hIterator, HIter & hClone)
	{
	Error err = GNR_INVALID_PARAMETERS;
	GenericFileSystemIterator * pCloned = NULL;

	LOGVFI(CloneIterator, ITERATOR);

	if (hIterator)
		{
		//
		// Clone iterator
		//

		err = ((GenericFileSystemIterator*) hIterator)->Clone(pCloned);
		hClone = (HIter) pCloned;
		}

	LOGVFIEXIT(CloneIterator, ITERATOR, err);

	GNRAISE(err);
	}

//
//  Delete iterator.
//

DLLCALL Error WINAPI VFI_DeleteIterator(HIter hIterator)
	{
	LOGVFI(DeleteIterator, ITERATOR);

	delete (GenericFileSystemIterator*) hIterator;

	LOGVFIEXIT(DeleteIterator, ITERATOR, GNR_OK);

	GNRAISE_OK;
	}

//
//  Get Name of Item the given iterator actually points to
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//  if the buffer is too short --> GNR_NOT_ENOUGH_MEMORY. length is set to the required buffer length
//  but the buffer is filled with as much as fits in it. So GNR_NOT_ENOUGH_MEMORY only indicates
//  a truncated string.
//
//  the length returned in case of an error is the length of the string including the closing zero
//

DLLCALL Error WINAPI VFI_GetItemName(HIter hIterator, DWORD & length, char * name)
	{
	DiskItemName itemName;
	Error err;

	LOGVFI(GetItemName, ITEM);

	//
	// Test parameters
	//

	if (hIterator && name)
		{
		//
		// Get name from root file system
		//

		if (!IS_ERROR(err = ((GenericFileSystemIterator*)hIterator)->GetItemName(itemName)))
			{
			//
			// Set out parameter
			//

			if (itemName.Length() >= (int)length)
				{
				length = itemName.Length() + 1;
				strncpy(name, (char*)itemName, length - 1);
				name[length - 1] = '\0';
				err = GNR_NOT_ENOUGH_MEMORY;
				}
			else
				strcpy(name, (char*)itemName);
			}
		}
	else
		err = GNR_INVALID_PARAMETERS;

	LOGVFIEXIT(GetItemName, ITEM, err);

	GNRAISE(err);
	}

//
//  Get Item Path...
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//  if the buffer is too short --> GNR_NOT_ENOUGH_MEMORY. length is set to the required buffer length
//  but the buffer is filled with as much as fits in it. So GNR_NOT_ENOUGH_MEMORY only indicates
//  a truncated string.
//
//  the length returned in case of an error is the length of the string including the closing zero
//

DLLCALL Error WINAPI VFI_GetItemPath(HIter hIterator, DWORD & length, char * path)
	{
	DiskItemName pathName;
	Error err;

	LOGVFI(GetItemPath, ITEM);

	//
	// Test parameters
	//

	if (hIterator && path)
		{
		//
		// Get name from root file system
		//

		if (!IS_ERROR(err = ((GenericFileSystemIterator*)hIterator)->GetPathName(pathName)))
			{
			//
			// Set out parameter
			//

			if (pathName.Length() >= (int)length)
				{
				strncpy(path, (char*)pathName, length - 1);
				path[length - 1] = '\0';
				length = pathName.Length() + 1;
				err = GNR_NOT_ENOUGH_MEMORY;
				}
			else
				strcpy(path, (char*)pathName);
			}
		}
	else
		err = GNR_INVALID_PARAMETERS;

	LOGVFIEXIT(GetItemPath, ITEM, err);

	GNRAISE(err);
	}

//
//  Convert DiskItemType to VFIItemType
//

VFIItemType DITToVIT(DiskItemType dit)
	{
	switch (dit)
		{
		case DIT_FILE:
			return VIT_FILE;
		case DIT_DIR:
			return VIT_DIR;
		case DIT_ALL:
			return VIT_ALL;
		default:
			return VIT_NONE;
		}
	return VIT_NONE;
	}

//
//  Convert VFIItemType to DITItemType
//

DiskItemType VITToDIT(VFIItemType vit)
	{
	switch (vit)
		{
		case VIT_FILE:
			return DIT_FILE;
		case VIT_DIR:
			return DIT_DIR;
		case VIT_ALL:
			return DIT_ALL;
		default:
			return DIT_NONE;
		}
	return DIT_NONE;
	}

//
//  Get Disk Item Type
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GetItemType(HIter hIterator, VFIItemType & type)
	{
	DiskItemType dit;
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GetItemType, ITEM);

	//
	// Test parameters ...
	//

	if (hIterator)
		{
		//
		// Get item type ...
		//

		err = ((GenericFileSystemIterator*)hIterator)->GetItemType(dit);
		type = DITToVIT(dit);
		}

	LOGVFIEXIT(GetItemType, ITEM, err);

	GNRAISE(err);
	}

//
//  Get Disk Item Size
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GetItemSize(HIter hIterator, DWORD & sizeLo, DWORD & sizeHi)
	{
	GenericFileSystemIterator * pIterator = (GenericFileSystemIterator*) hIterator;
	KernelInt64 size64;
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GetItemSize, ITEM);

	//
	//  Test parameters
	//

	if (pIterator)
		{
		//
		//  Get item size
		//

		err = pIterator->GetItemSize(size64);

		sizeLo = size64.Lower();
		sizeHi = size64.Upper();
		}

	LOGVFIEXIT(GetItemSize, ITEM, err);

	GNRAISE(err);
	}

//
//  Get Number Of Items in Iterator
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GetNumberOfItems(HIter hIterator, DWORD & num)
	{
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GetNumberOfItems, ITEM);

	if (hIterator)
		{
		//
		//  Get number of items from iterator
		//

		err = ((GenericFileSystemIterator*)hIterator)->GetNumberOfItems(num);
		}

	LOGVFIEXIT(GetNumberOfItems, ITEM, err);

	GNRAISE(err);
	}

//
//  Move iterator to next item
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GoNext(HIter hIterator)
	{
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GoNext, ITERATOR);

	if (hIterator)
		err = ((GenericFileSystemIterator*)hIterator)->GoToNextItem();

	LOGVFIEXIT(GoNext, ITERATOR, err);

	GNRAISE(err);
	}

//
//  Go to first item on itereator, where the iterator started to iterate
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GoFirst(HIter hIterator)
	{
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GoFirst, ITERATOR);

	if (hIterator)
		err = ((GenericFileSystemIterator*)hIterator)->GoToFirstItem();

	LOGVFIEXIT(GoFirst, ITERATOR, err);

	GNRAISE(err);
	}

//
//  Go down the tree (to the sub directory)
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GoDown(HIter hIterator, HIter & hNewIterator)
	{
	GenericFileSystemIterator * pIterator = (GenericFileSystemIterator*) hIterator;
	GenericFileSystemIterator * pNewIterator;
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GoDown, ITERATOR);

	if (pIterator)
		{
		err = pIterator->GoToSubDir(pNewIterator);
		hNewIterator = (HIter) pNewIterator;
		}

	LOGVFIEXIT(GoDown, ITERATOR, err);

	GNRAISE(err);
	}

//
//  Go up the tree (to the parent directory)
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GoUp(HIter hIterator, HIter & hParentIter)
	{
	GenericFileSystemIterator * pIterator = (GenericFileSystemIterator*) hIterator;
	GenericFileSystemIterator * pNewIterator;
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GoUp, ITERATOR);

	if (pIterator)
		{
		err = pIterator->GoToParentDir(pNewIterator);
		hParentIter = (HIter) pNewIterator;
		}

	LOGVFIEXIT(GoUp, ITERATOR, err);

	GNRAISE(err);
	}

//
//  Find Item
//  [in] name : '\0' terminated string that identifies the item to be found
//  [in] type : file, directory or everything
//  [in] flags : specify if search should be recusrive, case sensitive, ...
//  [in] where : start point in file system
//  [out] result : new iterator that identifies the item found
//

DLLCALL Error WINAPI VFI_FindItem(char * name, VFIItemType type, DWORD flags, HIter where, HIter & result)
	{
	GenericFileSystemIterator * gfsi;
	DWORD fifFlags = FIF_NONE;
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(FindItem, ITERATOR);

	if (where)
		{
		if (flags & VFIF_RECURSIVE)
			fifFlags |= FIF_RECURSIVE;
		if (flags & VFIF_CASE_SENSITIVE)
			fifFlags |= FIF_CASE_SENSITIVE;

		err = pRootFileSystem->FindItem(DiskItemName(name), VITToDIT(type), fifFlags,
												  (GenericFileSystemIterator*)where, gfsi);
		result = (HIter)gfsi;
		}

	LOGVFIEXIT(FindItem, ITERATOR, err);

	GNRAISE(err);
	}

//
// Open file by name
// [in] name : '\0' terminated string that identifies the
// [in] accessType : the mode in which we access the file
// [out] hFile : pointer to handle to opened file handle
//

DLLCALL Error WINAPI VFI_OpenFileByName(char * name, DWORD accessType, HFile & hFile)
	{
	GenericDiskItem * gdi;
	DWORD flags = FAT_HEADER;
	Error err;

	LOGVFI(OpenFileByName, FILE);

	if (accessType & VOIF_RECURSIVE)
		flags |= FAT_RECURSIVE;
	if (accessType & VOIF_CASE_SENSITIVE)
		flags |= FAT_CASE_SENSITIVE;

	err = pRootFileSystem->OpenItem(DiskItemName(name), flags, gdi);
	hFile = (HFile)gdi;

	LOGVFIEXIT(OpenFileByName, FILE, err);

	GNRAISE(err);
	}

//
//  Open disk item with desired access mode
//  hIterator is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_Open(HIter hIterator, DWORD access, HFile & hFile)
	{
	GenericFileSystemIterator * pIterator = (GenericFileSystemIterator*)hIterator;
	GenericDiskItem * pGDI = NULL;
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(Open, FILE);

	if (pIterator)
		{
		//
		// Open disk item with iterator
		//

		err = pIterator->OpenItem(access, pGDI);
		hFile = (HFile)pGDI;
		}

	LOGVFIEXIT(Open, FILE, err);

	GNRAISE(err);
	}

//
//  Close disk item previously opened with VFI_Open
//  hFile is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_Close(HFile hFile)
	{
	Error err = GNR_INVALID_PARAMETERS;
	GenericDiskItem * pGDI = (GenericDiskItem *)hFile;

	LOGVFI(Close, FILE);

	if (pGDI)
		{
		err = pGDI->Close();
		delete pGDI;
		}

	LOGVFIEXIT(Close, FILE, err);

	GNRAISE(err);
	}

//
//  Get File Size
//  hFile is checked against being invalid (==0) --> GNR_INVALID_PARAMETERS
//

DLLCALL Error WINAPI VFI_GetFileSize(HFile hFile, DWORD & sizeLo, DWORD & sizeHi)
	{
	GenericFile * gf = (GenericFile*)hFile;
	KernelInt64 size64;
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(GetFileSize, FILE);

	if (gf)
		{
		err = gf->GetSize(size64);
		sizeLo = size64.Lower();
		sizeHi = size64.Upper();
		}

	LOGVFIEXIT(GetFileSize, FILE, err);

	GNRAISE(err);
	}

//
//  Get Volume name
//

DLLCALL Error WINAPI VFI_GetVolumeName(HIter hIterator, DWORD & maxBufferLength, char * name)
	{
	GenericFileSystemIterator * pIterator = (GenericFileSystemIterator*) hIterator;
	GenericFileSystem * pGfs;
	KernelString sName;
	Error err;

	LOGVFI(GetVolumeName, VOLUME);

	if (pIterator && name)
		{
		if (!IS_ERROR(err = pIterator->GetFileSystem(pGfs)))
			{
			if (!IS_ERROR(err = pGfs->GetVolumeName(sName)))
				{
				//
				// Set out parameter
				//

				if (sName.Length() >= (int)maxBufferLength)
					{
					maxBufferLength = sName.Length() + 1;
					strncpy(name, (char*)sName, maxBufferLength - 1);
					name[maxBufferLength - 1] = '\0';
					err = GNR_NOT_ENOUGH_MEMORY;
					}
				else
					strcpy(name, (char*)sName);
				}
			}
		}
	else
		err = GNR_INVALID_PARAMETERS;

	LOGVFIEXIT(GetVolumeName, VOLUME, err);

	GNRAISE(err);
	}

//
//  Get Volume Set Name
//

DLLCALL Error WINAPI VFI_GetVolumeSetName(HIter hIterator, DWORD & maxBufferLength, char * name)
	{
	GenericFileSystemIterator * pIterator = (GenericFileSystemIterator*) hIterator;
	GenericFileSystem * pGfs;
	KernelString sName;
	Error err;

	LOGVFI(GetVolumeSetName, VOLUME);

	if (pIterator && name)
		{
		if (!IS_ERROR(err = pIterator->GetFileSystem(pGfs)))
			{
			if (!IS_ERROR(err = pGfs->GetVolumeSetName(sName)))
				{
				//
				// Set out parameter
				//

				if (sName.Length() >= (int)maxBufferLength)
					{
					maxBufferLength = sName.Length() + 1;
					strncpy(name, (char*)sName, maxBufferLength - 1);
					name[maxBufferLength - 1] = '\0';
					err = GNR_NOT_ENOUGH_MEMORY;
					}
				else
					strcpy(name, (char*)sName);
				}
			}
		}
	else
		err = GNR_INVALID_PARAMETERS;

	LOGVFIEXIT(GetVolumeSetName, VOLUME, err);

	GNRAISE(err);
	}

//
//  Get Volume Set Index
//

DLLCALL Error WINAPI VFI_GetVolumeSetIndex(HIter hIterator, int & index)
	{
	Error err = GNR_INVALID_PARAMETERS;
	GenericFileSystem * gfs;

	LOGVFI(GetVolumeSetIndex, VOLUME);

	if (hIterator)
		{
		if (!IS_ERROR(err = ((GenericFileSystemIterator*)hIterator)->GetFileSystem(gfs)))
			err = gfs->GetVolumeSetIndex(index);
		}

	LOGVFIEXIT(GetVolumeSetIndex, VOLUME, err);

	GNRAISE(err);
	}

//
//  Get Volume Set Size
//

DLLCALL Error WINAPI VFI_GetVolumeSetSize(HIter hIterator, int & size)
	{
	Error err = GNR_INVALID_PARAMETERS;
	GenericFileSystem * gfs;

	LOGVFI(GetVolumeSetSize, VOLUME);

	if (hIterator)
		{
		if (!IS_ERROR(((GenericFileSystemIterator*)hIterator)->GetFileSystem(gfs)))
			err = gfs->GetVolumeSetSize(size);
		}

	LOGVFIEXIT(GetVolumeSetSize, VOLUME, err);

	GNRAISE(err);
	}

//
//  Get Unique key
//  sizeof (key) must be >= 8
//

DLLCALL Error WINAPI VFI_GetUniqueKey(HIter hIterator, BYTE * key)
	{
	Error err = GNR_INVALID_PARAMETERS;
	GenericFileSystem * pGfs;

	LOGVFI(GetUniqueKey, VOLUME);

	if (hIterator)
		{
		if (!IS_ERROR(err = ((GenericFileSystemIterator*)hIterator)->GetFileSystem(pGfs)))
			err =  pGfs->GetUniqueKey(key);
		}

	LOGVFIEXIT(GetUniqueKey, VOLUME, err);

	GNRAISE(err);
	}

//
//  Get request handle
//

DLLCALL Error WINAPI VFI_GetRequestHandle(HRequest & hReq)
	{
	Error err = GNR_OK;

	LOGVFI(GetRequestHandle, GENERAL);

	hReq = (HRequest)new RequestHandle();

	if (!hReq)
		err = GNR_NOT_ENOUGH_MEMORY;

	LOGVFIEXIT(GetRequestHandle, GENERAL, err);

	GNRAISE(err);
	}

//
//  Close request handle
//

DLLCALL Error WINAPI VFI_CloseRequestHandle(HRequest hReq)
	{
	LOGVFI(CloseRequestHandle, GENERAL);

	delete (RequestHandle*)hReq;

	LOGVFIEXIT(CloseRequestHandle, GENERAL, GNR_OK);

	GNRAISE_OK;
	}

//
//  Terminate request
//

DLLCALL Error WINAPI VFI_TerminateRequest(HRequest hReq)
	{
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(TerminateRequest, GENERAL);

	if (hReq)
		err = pRootFileSystem->TerminateRequest((RequestHandle*)hReq);

	LOGVFIEXIT(TerminateRequest, GENERAL, err);

	GNRAISE(err);
	}

//
//  Read some bytes from file
//

DLLCALL Error WINAPI VFI_ReadBytes(HFile hFile, DWORD offset, DWORD offsetHi, DWORD num, BYTE * buffer, DWORD flag, HRequest hReq)
	{
	Error err = GNR_INVALID_PARAMETERS;

	LOGVFI(ReadBytes, GENERAL);

	if (hReq && hFile)
		err = ((GenericFile*)hFile)->ReadBytes(KernelInt64(offset, offsetHi), num, buffer, flag, (RequestHandle*)hReq);

	LOGVFIEXIT(ReadBytes, GENERAL, err);

	GNRAISE(err);
	}
