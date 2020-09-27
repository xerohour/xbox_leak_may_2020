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
//  File Error Declarations
//
////////////////////////////////////////////////////////////////////

#ifndef FILEERRORS_H
#define FILEERRORS_H

#include "library/common/gnerrors.h"

#define GNR_PATH_NOT_FOUND							MKERR(ERROR,	FILE,			OBJECT,			0x00)
// The path specified was not found

#define GNR_INVALID_PATH							MKERR(ERROR,	FILE,			OBJECT,			0x01)
// The path specified was invalid (e.g. contained invalid letters)

#define GNR_NO_FILE_SYSTEM							MKERR(ERROR,	FILE,			OBJECT,			0x02)
// There is no file system to execute operation

#define GNR_NO_VOLUME								MKERR(ERROR,	FILE,			OBJECT,			0x03)
// There is no volume to execute operation

#define GNR_VOLUME_INVALID							MKERR(ERROR,	FILE,			OBJECT,			0x04)
// Volume is invalid for some reason, e.g. not supported

#define GNR_ITEM_NOT_FOUND							MKERR(ERROR,	FILE,			OBJECT,			0x05)
// The item was not found or there is no more file in the directory

#define GNR_NOT_A_DIRECTORY						MKERR(ERROR,	FILE,			OBJECT,			0x06)
// Disk item is not a directory

#define GNR_ITEM_INVALID							MKERR(ERROR,	FILE,			OBJECT,			0x07)
// Item is invalid (e.g. for an operation)

#define GNR_FILE_READ_ONLY							MKERR(ERROR,	FILE,			OPERATION,		0x00)
// The file is read only

//#define GNR_FILE_IN_USE

#endif
