#ifndef __BBREG_H__
#define __BBREG_H__

#ifndef DWORD
#define DWORD unsigned long
#endif

#include "md5.h"

#define BBREG_VERSION	0x00030002

typedef struct _BBREG_HEADER	BBREG_HEADER,	*PBBREG_HEADER;

struct _BBREG_HEADER {
	DWORD	dwVersion;
	DWORD	dwSizeOfBBRegHeader;
	DWORD	dwOffsetOfBinaryName;
	DWORD	dwOffsetOfGUID;
	DWORD	dwOffsetOfLoggingRegion;
	DWORD	dwCheckSumOfBinary;
	PCHAR	pBaseOfLoggingRegion;
	DWORD	dwSizeOfLoggingRegion;
};


#endif __BBREG_H__
