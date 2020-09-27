/*
	NvVersion.r

	Copyright NVIDIA Corp. 2000-2001
	All rights reserved.

*/

#include "Types.r"

#define	kMajorVersNumber	0x00
#define	kMinorVersNumber	0x00
#define	kBugFixNumber   	0x00
#define	kReleaseType		development
#define	kBuildNumber		0
#define	kMajorStr			"0.0.0"


resource 'vers' (1) {
	kMajorVersNumber,
	kMinorVersNumber + kBugFixNumber,
	kReleaseType,
	kBuildNumber,
	verUS,
	kMajorStr,
	kMajorStr ", © NVIDIA Corp. 2001"
};
