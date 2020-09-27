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
//  VIONA File Interface
//
////////////////////////////////////////////////////////////////////

#ifndef VFI_H
#define VFI_H

#include "Library/Common/Prelude.h"
#include "Library/Common/GNErrors.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "Library/Files/RootFile.h"
#endif

////////////////////////////////////////////////////////////////////
//
//  Type definitions
//
////////////////////////////////////////////////////////////////////

// Handle to iterator
typedef void * HIter;

// Handle to file
typedef void * HFile;

// Handle to request identifer
typedef void * HRequest;

//
//  Item type
//

enum VFIItemType
	{
	VIT_NONE		= 0x0000,
	VIT_FILE		= 0x0001,
	VIT_DIR		= 0x0002,
	VIT_ALL		= 0xffff
	};

//
//  VFI_FindItem flags
//

#define VFIF_NONE					0x00000000		// Nothing at all
#define VFIF_RECURSIVE			0x00000001		// Do recursive search
#define VFIF_CASE_SENSITIVE	0x00000002		// Search is case sensitive

//
//  VFI_OpenItem flags
//

#define VOIF_NONE					0x00000000		// Nothing at all
#define VOIF_RECURSIVE			0x00000001		// Do recursive search
#define VOIF_CASE_SENSITIVE	0x00000002		// When opening file by name search is case sensitive

////////////////////////////////////////////////////////////////////
//
//	VFI startup and shutdown
//
////////////////////////////////////////////////////////////////////

#ifndef ONLY_EXTERNAL_VISIBLE

Error VFI_Init(GenericFileSystem * pRootSystem);	// Should be called from DLL Entry routine

Error VFI_Cleanup(void);

#endif

////////////////////////////////////////////////////////////////////
//
// Iterator handling
//
////////////////////////////////////////////////////////////////////

//
//  Iterator creation and delete
//

// Create Iterator
DLLCALL Error WINAPI VFI_CreateIterator(HIter & pHIterator);

// Close Iterator
DLLCALL Error WINAPI VFI_DeleteIterator(HIter hIterator);

// Clone Iterator
DLLCALL Error WINAPI VFI_CloneIterator(HIter hIterator, HIter & clone);


//
//  Iterator item handling
//

// get name of item the iterator points to
DLLCALL Error WINAPI VFI_GetItemName(HIter hIterator, DWORD & maxBufferLength, char * psName);

// get path of item
DLLCALL Error WINAPI VFI_GetItemPath(HIter hIterator, DWORD & maxBufferLength, char * psPath);

// get item type
// [in] hIterator : iterator
// [out] pType : type of item the iterator points to
DLLCALL Error WINAPI VFI_GetItemType(HIter hIterator, VFIItemType & type);

// get size of item pointed to by the iterator
// [in] hIterator : handle to iterator that point to an item
// [out] size : size of item pointed to by the iterator
DLLCALL Error WINAPI VFI_GetItemSize(HIter hIterator, DWORD & size, DWORD & hiPart);

// get number of items
// [in] hIterator
// [out] num : number of items in the directory the iterator is pointing to
DLLCALL Error WINAPI VFI_GetNumberOfItems(HIter hIterator , DWORD & num);

//
//  Iterator navigation
//

// go to next item in iterator
// [in] hIterator : handle to iterator that should be set to next element in directory
DLLCALL Error WINAPI VFI_GoNext(HIter hIterator);

// got to first item in iterator
// the iterator given as parameter is set to the first element of the directory
// [in] hIterator : handle to iterator that should be set to the first element of the directory
DLLCALL Error WINAPI VFI_GoFirst(HIter hIterator);

// open the directory the iterator points to.
// [in] hIterator : Iterator that points to the directory
// [out] hNewIterator : pointer to new handle to iterator that points to the first element of the opened directory
DLLCALL Error WINAPI VFI_GoDown(HIter hIterator, HIter & hNewIterator);

// get iterator to parent directory
// [in] hIterator : Iterator that points somewhere in a directory
// [out] hParentIter : pointer to handle of iterator that points to the first element of the parent directory
DLLCALL Error WINAPI VFI_GoUp(HIter hIterator, HIter & hParentIter);

// Find Item
// [in] name : '\0' terminated string that identifies the item to be found
// [in] type : file, directory or everything
// [in] flags : specify if search should be recusrive, case sensitive, ...
// [in] where : start point in file system
// [out] result : new iterator that identifies the item found
DLLCALL Error WINAPI VFI_FindItem(char * name, VFIItemType type, DWORD flags, HIter where, HIter & result);

////////////////////////////////////////////////////////////////////
//
//  Volume handling
//
////////////////////////////////////////////////////////////////////


// get name of volume the given iterator is currently acting on
// [in] hIterator : Iterator used
// [in] maxBufferLength : buffer length for buffer sName
// [out] sChar : name of volume the iterator is acting on
DLLCALL Error WINAPI VFI_GetVolumeName(HIter hIterator, DWORD & maxBufferLength, char * &sName);


// get volume set name a given iterator is currently acting on
DLLCALL Error WINAPI VFI_GetVolumeSetName(HIter hIterator, DWORD & maxBufferLength, char * &sName);


// get index in volume set of the volume the given iterator is acting on
DLLCALL Error WINAPI VFI_GetVolumeSetIndex(HIter hIterator, int & index);


// get size of volume set the current volume the given iterator is acting on is part of
// [in] hIterator : handle of iterator to get the volume set size
// [out] size : size of volume set
DLLCALL Error WINAPI VFI_GetVolumeSetSize(HIter hIterator, int & size);


// get unique key
// get key sequence that represents an unique identifier of the volume the current iterator is acting on
DLLCALL Error WINAPI VFI_GetUniqueKey(HIter hIterator, BYTE * key);


////////////////////////////////////////////////////////////////////
//
//  Files
//
////////////////////////////////////////////////////////////////////

//
//  For some calls you need a request handle so you can abort the call identified by this handle
//

// Get request handle
DLLCALL Error WINAPI VFI_GetRequestHandle(HRequest & hReq);


// close request handle
DLLCALL Error WINAPI VFI_CloseRequestHandle(HRequest hReq);


// cancel request
DLLCALL Error WINAPI VFI_CancelRequest(HRequest hReq);

// Open file by name
// [in] name : '\0' terminated string that identifies the
// [in] accessType : the mode in which we access the file
// [out] hFile : pointer to handle to opened file handle
DLLCALL Error WINAPI VFI_OpenFileByName(char * name, DWORD accessType, HFile & hFile);

// open the file that an iterator handle points to
// [in] hIterator : Iterator that points to a file item
// [in] accessType : the mode in which we access the file
// [out] hFile : pointer to handle to opened file handle
DLLCALL Error WINAPI VFI_Open(HIter hIterator, DWORD accessType, HFile & hFile);

// close item
DLLCALL Error WINAPI VFI_Close(HFile hFile);

// get size of file
// [in] hFile : handle to file
// [out] size : size of item pointed to by the iterator
DLLCALL Error WINAPI VFI_GetFileSize(HFile hFile, DWORD & sizeLo, DWORD & sizeHi);

// read bytes from file
// the file must be created by calling Open(...)
// [in] hFile : handle of file, returned by Open(iterator, ..)
// [in] offset : offset in file to start read
// [in] offsetHi : hi part of offset
// [in] num	: number of bytes to be read
// [out] buffer : buffer filled with block data. Size must be at least num bytes
// [in] flag : flags
// [in] pReq : handle to request. Can be used to interrupt or cancel request
DLLCALL Error WINAPI VFI_ReadBytes(HFile hFile, DWORD offset, DWORD offsetHi, DWORD num, BYTE * buffer, DWORD flag, HRequest pReq);


#endif // VFI_H
