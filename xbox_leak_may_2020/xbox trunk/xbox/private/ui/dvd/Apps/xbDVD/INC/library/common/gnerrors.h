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

//
// FILE:      library\common\gnerrors.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:   Centralized error handling
//
// HISTORY:
//
//

#ifndef GNERRORS_H
#define GNERRORS_H

#if !VIDEO_MINIPORT_DRIVER
#include	"prelude.h"
#endif

typedef unsigned long Error;

//
// The last error, that was raised, is not only returned, but preserved in
// this variable.
//
//extern Error LastError;

//
// Error numbers are composed of four parts, an severity level, that
// tells how hard the error hit; a unit id, describing the unit that
// failed; an error type giving some general reasons and an unique
// id (unique for the module, not all error messages).
//
//                  SS-UUUUUUUU-TTTTTTTT-IIIIIIIIIIIIII
//


#define GNR_SEVERITY_BITS	(2)
#define GNR_SEVERITY_SHIFT	(30)
#define GNR_SEVERITY_MASK	(0xC0000000)

#define GNR_UNIT_BITS		(8)
#define GNR_UNIT_SHIFT		(22)
#define GNR_UNIT_MASK		(0x3FC00000)

#define GNR_TYPE_BITS		(8)
#define GNR_TYPE_SHIFT		(14)
#define GNR_TYPE_MASK	   (0x003FC000)

#define GNR_UNIQUE_BITS		(14)
#define GNR_UNIQUE_SHIFT	(0)
#define GNR_UNIQUE_MASK		(0x00003FFF)


//
// The severity level describes how hard this error affected the operation.
//
// As the severity level is the most significant part of an error message,
// it can be tested with a simple compare (not the Error is defined as
// unsigned, so no special sign effects can occur).
//
#define GNR_OK					(0x0L << GNR_SEVERITY_SHIFT)
	// everything did well; The neat side effect of defining the OK level as
	// 0, is that the check for an error can be done with a simple not
	// 0 compare like "if (error) { ... }".

#define GNR_WARNING			(0x1L << GNR_SEVERITY_SHIFT)
	// something not quite perfect happened, but the action was performed

#define GNR_ERROR				(0x2L << GNR_SEVERITY_SHIFT)
	// an error accured, that kept the routine from doing it´s job

#define GNR_DEADLY			(0x3L << GNR_SEVERITY_SHIFT)
	// something happened, that will keep the program from working correct,
	// it would be better to exit gracefully

//
// The error unit specifies the module where this error was defined
//
#define GNR_UNIT_COMMON		(0x0L << GNR_UNIT_SHIFT)
	// defined in gnerrors.H

#define GNR_UNIT_GENERAL	(0x1L << GNR_UNIT_SHIFT)
	// define in ..\general\*.h

#define GNR_UNIT_MEMMPDIO	(0x2L << GNR_UNIT_SHIFT)
	// define in memmpdio.h

#define GNR_UNIT_I2C			(0x3L << GNR_UNIT_SHIFT)
	// defined in viperi2c.h

#define GNR_UNIT_INTERRUPT	(0x4L << GNR_UNIT_SHIFT)
   // defined in intrctrl.h

#define GNR_UNIT_PARSER		(0x5L << GNR_UNIT_SHIFT)
	// defined in mpgparse.h

#define GNR_UNIT_PCI					(0x6L << GNR_UNIT_SHIFT)
	// define in pci.h

#define GNR_UNIT_DMA					(0x7L << GNR_UNIT_SHIFT)
	// define in dma*.h

#define GNR_UNIT_VDECODER			(0x8L << GNR_UNIT_SHIFT)
	// defined in viddec.h

#define GNR_UNIT_VENCODER			(0x9L << GNR_UNIT_SHIFT)
	// defined in videnc.h

#define GNR_UNIT_JPEG				(0xCL << GNR_UNIT_SHIFT)
	// defined in core.h

#define GNR_UNIT_MPEG				(0xDL << GNR_UNIT_SHIFT)
	// defined in library\hardware\mpeg2dec\mp2eldec.h

#define GNR_UNIT_TVTUNER			(0xEL << GNR_UNIT_SHIFT)
	// defined in tvtuner.h

#define GNR_UNIT_VESA20				(0xFL << GNR_UNIT_SHIFT)
	// defined in vesa20.h

#define GNR_UNIT_GFC					(0x10L << GNR_UNIT_SHIFT)
	// defined in gfxconfig.h

#define GNR_UNIT_PIP					(0x11L << GNR_UNIT_SHIFT)
	// defined in *pip*.h

#define GNR_UNIT_DMAMEM				(0x12L << GNR_UNIT_SHIFT)
	//	defined in ctdmamem.h

#define GNR_UNIT_CD					(0x13L << GNR_UNIT_SHIFT)
	// defined in cdifs.h

#define GNR_UNIT_PROFILES			(0x14L << GNR_UNIT_SHIFT)
	// defined in profiles.h

#define GNR_UNIT_AUDIO				(0x15L << GNR_UNIT_SHIFT)
	// defined in audio

#define GNR_UNIT_PCICONTROLLER	(0x16L << GNR_UNIT_SHIFT)

#define GNR_UNIT_AC3					(0x17L << GNR_UNIT_SHIFT)

#define GNR_UNIT_VTX					(0x18L << GNR_UNIT_SHIFT)
	// defined in vtxdll.h

#define GNR_UNIT_MPEG2				(0x19L << GNR_UNIT_SHIFT)

#define GNR_UNIT_MP2PARSER			(0x1AL << GNR_UNIT_SHIFT)

#define GNR_UNIT_VXD					(0x1BL << GNR_UNIT_SHIFT)
	// Error messages from PnP VxD interfaces

#define GNR_UNIT_DISK				(0x1CL << GNR_UNIT_SHIFT)
	// General disk errors (library\hardware\drives\generic\diskerrors.h)

#define GNR_UNIT_NAVIGATION		(0x1DL << GNR_UNIT_SHIFT)
	// Navigation errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_DVD					(0x1EL << GNR_UNIT_SHIFT)
	// DVD specific errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_VCD					(0x1FL << GNR_UNIT_SHIFT)
	// VCD specific errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_CDA					(0x20L << GNR_UNIT_SHIFT)
	// CDA specific errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_DECRYPTION		(0x21L << GNR_UNIT_SHIFT)
	// Decryption errors (library\hardware\mpeg2dec\generic\mp2dcryp.h)

#define GNR_UNIT_FILE				(0x22L << GNR_UNIT_SHIFT)
	// File errors (library\files\fileerrors.h)

#define GNR_UNIT_UNITS				(0x21L << GNR_UNIT_SHIFT)
	// in virtunit.h

#define GNR_UNIT_SUBPICTURE		(0x22L << GNR_UNIT_SHIFT)

#define GNR_UNIT_FRONTPANEL             (0x23L << GNR_UNIT_SHIFT)

//
// The error type gives a general hint, what caused the malfunction
//
#define GNR_TYPE_GENERAL			(0x00L << GNR_TYPE_SHIFT)
	// no special info

#define GNR_TYPE_FILEIO				(0x01L << GNR_TYPE_SHIFT)
	// some file specific error occured

#define GNR_TYPE_HARDWARE			(0x02L << GNR_TYPE_SHIFT)
	// it was a general hardware problem

#define GNR_TYPE_PARAMS				(0x03L << GNR_TYPE_SHIFT)
	// illegal parameters were used

#define GNR_TYPE_TIMEOUT			(0x04L << GNR_TYPE_SHIFT)
	// a timeout happened

#define GNR_TYPE_BUSY				(0x05L << GNR_TYPE_SHIFT)
	// the requested unit is currently busy

#define GNR_TYPE_NOACK				(0x06L << GNR_TYPE_SHIFT)
	// the requested unit did not respond

#define GNR_TYPE_INACTIVE			(0x07L << GNR_TYPE_SHIFT)
	// the requested unit is currently not active, or in an illegal
	// state for the type of the requested operation

#define GNR_TYPE_MEMORY				(0x08L << GNR_TYPE_SHIFT)
	// there was no sufficient free store left

#define GNR_TYPE_UNIMPLEMENTED 	(0x09L << GNR_TYPE_SHIFT)
	// the called function is not yet completely implemented

#define GNR_TYPE_FORMAT				(0x0aL << GNR_TYPE_SHIFT)
	// the examined object was in the wrong format

#define GNR_TYPE_OBJECT				(0x0bL << GNR_TYPE_SHIFT)

#define GNR_TYPE_BOUNDS				(0x0cL << GNR_TYPE_SHIFT)

#define GNR_TYPE_ILLEGALACCESS	(0x0dL << GNR_TYPE_SHIFT)

#define GNR_TYPE_REGION				(0x0eL << GNR_TYPE_SHIFT)

#define GNR_TYPE_PARENTAL			(0x0fL << GNR_TYPE_SHIFT)

#define GNR_TYPE_UOP					(0x10L << GNR_TYPE_SHIFT)

#define GNR_TYPE_OPERATION			(0x11L << GNR_TYPE_SHIFT)

#define GNR_TYPE_INTERNALSTATE	(0x12L << GNR_TYPE_SHIFT)

#define GNR_TYPE_COPYPROTECTION	(0x13L << GNR_TYPE_SHIFT)

//
// Macros to build and decompose error numbers
//
#define MKERR(level, unit, type, unique) ((Error)(GNR_##level | GNR_UNIT_##unit | GNR_TYPE_##type | unique))
	// build an error number from it´s parts

#define GNR_SEVERITY(err) (err & GNR_SEVERITY_MASK)
	// extracts the severity of the error number

#define GNR_UNIT(err) (err & GNR_UNIT_MASK)
	// extracts the unit of the error number

#define GNR_TYPE(err) (err & GNR_TYPE_MASK)
	// extracts the type of the error number

#define GNR_UNIQUE(err) (err & GNR_UNIQUE_MASK)
	// extracts the unique id of the error number

#ifndef IS_ERROR
	//
	// sidenote, the original definition of IS_ERROR in <winerror.h> is
	// #define IS_ERROR(Status) ((unsigned long)(Status) >> 31 == SEVERITY_ERROR)
	// this is functional identical to our definition, so no need to redefine it
	// here.  In any case a new define is added, which is to be used in all future
	// tests.
	//
#define IS_ERROR(err) ((err) >= GNR_ERROR)
#endif
#define IS_GNERROR(err) ((err) >= GNR_ERROR)

#define IS_WARNING(err) (GNR_SEVERITY(err) == GNR_WARNING)

//
// Some default error messages
//
#define GNR_FILE_NOT_FOUND			MKERR(ERROR, COMMON, FILEIO, 0x00)
	// general file not found error

#define GNR_FILE_IN_USE 			MKERR(ERROR, COMMON, FILEIO, 0x01)
	// the file is currently in use

#define GNR_FILE_WRONG_FORMAT 	MKERR(ERROR, COMMON, FILEIO, 0x02)
	// the file is in the wrong file format

#define GNR_END_OF_FILE 			MKERR(ERROR, COMMON, FILEIO, 0x03)
	// attempt to read behind the end of a file

#define GNR_NOT_ENOUGH_MEMORY 	MKERR(ERROR, COMMON, MEMORY, 0x04)
	// general not enough memory left

#define GNR_MEM_NOT_ALLOCATED 	MKERR(WARNING, COMMON, MEMORY, 0x05)
	// the memory that was requested to be freed was not allocated
	// before.

#define GNR_MEM_ALLOCATED_BEFORE MKERR(WARNING, COMMON, MEMORY, 0x06)
	// the memory was allocated before


#define GNR_UNIMPLEMENTED 			MKERR(ERROR, COMMON, UNIMPLEMENTED, 0x07)
	// the function is not yet implemented, and will never be


#define GNR_OBJECT_NOT_FOUND		MKERR(ERROR, COMMON, OBJECT, 0x08)
	// the requested object could not be found

#define GNR_OBJECT_EXISTS			MKERR(ERROR, COMMON, OBJECT, 0x09)
	// the object requested to be created does already exist

#define GNR_OBJECT_IN_USE			MKERR(ERROR, COMMON, OBJECT, 0x0a)
	// an operation that needs exclusive access to an object, found the object
	// already in use

#define GNR_OBJECT_FOUND			MKERR(ERROR, COMMON, OBJECT, 0x0b)
	// an object that was to be inserted in a data structure with unique objects
	// was already in there

#define GNR_RANGE_VIOLATION		MKERR(ERROR, COMMON, BOUNDS, 0x0c)
	// a given parameter was out of bounds


#define GNR_INVALID_CONFIGURE_STATE		MKERR(ERROR, COMMON, INACTIVE, 0x0d)
	// the unit was in an invalid state for configuration

#define GNR_OBJECT_FULL				MKERR(ERROR, COMMON, BOUNDS, 0x0e)

#define GNR_OBJECT_EMPTY			MKERR(ERROR, COMMON, BOUNDS, 0x0f)

#define GNR_OBJECT_NOT_ALLOCATED	MKERR(ERROR, COMMON, INACTIVE, 0x10)

#define GNR_OBJECT_READ_ONLY		MKERR(ERROR, COMMON, OBJECT, 0x11)

#define GNR_OBJECT_WRITE_ONLY		MKERR(ERROR, COMMON, OBJECT, 0x12)

#define GNR_OPERATION_PROHIBITED	MKERR(ERROR, COMMON, ILLEGALACCESS, 0x13)

#define GNR_OBJECT_INVALID			MKERR(ERROR, COMMON, OBJECT, 0x14)

#define GNR_INSUFFICIENT_RIGHTS	MKERR(ERROR, COMMON, ILLEGALACCESS, 0x15)

#define GNR_TIMEOUT					MKERR(ERROR, COMMON, NOACK, 0x16)

#define GNR_FILE_READ_ERROR		MKERR(ERROR, COMMON, FILEIO, 0x17)

#define GNR_FILE_WRITE_ERROR		MKERR(ERROR, COMMON, FILEIO, 0x18)

#define GNR_INVALID_PARAMETERS	MKERR(ERROR, COMMON, PARAMS, 0x19)

#define GNR_CONNECTION_LOST		MKERR(ERROR, COMMON, INACTIVE, 0x1a)

#define GNR_OPERATION_ABORTED		MKERR(ERROR, COMMON, TIMEOUT, 0x1b)

#define GNR_OPERATION_FAILED		MKERR(ERROR, COMMON, OPERATION, 0x1c )

//
// Error invocation macros, to be used to standardize erroneous returns
//
#define GNRAISE(e) return (e)
	// raise an error, store it into the global error variable, and
	// return with the error number.

#define GNRAISE_OK return GNR_OK
	// raise the OK error, all did well

//#define GNRAISE_AGAIN return LastError
	// raise the last error again

#define GNASSERT(cond, except) if (cond) GNRAISE(except); else	0
	// if the condition evaluates to true, the exception is raised

#define GNREASSERT(cond) if (1) {Error e; if (e = (cond)) GNRAISE(e);} else 0
	// if the condition evaluates to not OK, the error is raised again

#define GNREASSERTMAP(cond, ne) if (1) {Error e; if (e = (cond)) GNRAISE(ne);} else 0
	// if the condition evaluates to not OK, the error ne is returned

#ifndef ONLY_EXTERNAL_VISIBLE


#ifdef _WINDOWS

class NamedError {
	private:
		Error				error;
		NamedError	*	link;
		WORD				id;
		static	NamedError	*	root;
	friend void GetErrorText(HINSTANCE hinst, Error error, TCHAR __far * buffer, int bufferSize);
	public:
		NamedError(Error error, WORD id) {this->error = error; this->id = id; this->link = root; root = this;}
	};

#define NAME_ERROR(err, text)	static const NamedError GNRI_##err(GNR_##err, IDS_GNR_##err)

void GetErrorText(HINSTANCE hinst, Error error, TCHAR __far * buffer, int bufferSize);

#elif LINUX

class NamedError {
	private:
		Error				error;
		NamedError	*	link;
		const char 	*	name;
		static	NamedError	*	root;
	friend void GetErrorText(Error error, TCHAR __far * buffer, int bufferSize);
	public:
		NamedError(Error error, const char * name) {this->error = error; this->name = name; this->link = root; root = this;}
	};

#define NAME_ERROR(err, text)	static const NamedError GNRI_##err(GNR_##err, text)

void GetErrorText(Error error, TCHAR __far * buffer, int bufferSize);

#else

#if VXD_VERSION || WDM_VERSION

#define NAME_ERROR(err, text)
const char * GetErrorText(Error error);

#else
class NamedError {
	private:
		Error				error;
		NamedError	*	link;
		const char 	*	name;
		static	NamedError	*	root;
	friend const char * GetErrorText(Error error);
	public:
		NamedError(Error error, const char * name) {this->error = error; this->name = name; this->link = root; root = this;}
	};

#define NAME_ERROR(err, text)	static const NamedError GNRI_##err(GNR_##err, text)

const char * GetErrorText(Error error);
#endif	// of VXD_VERSION

#endif	// of _WINDOWS

#endif	// of ONLY_EXTERNAL_VISIBLE

#endif
