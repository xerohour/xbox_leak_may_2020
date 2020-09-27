/*
	driverinfo.h
*/

#include "naming.h"			// For Monica or NVIDIA names

#define FB_OFFSET	(16 * 1024)		// Offset (in bytes) of the frame buffer's first displayed pixel

#ifndef DEVICENAMESTRING
// This is the name of the NVIDIA board's Name Registry node, assigned by FCode.  If you have
//	multiple NV boards in your system, you'll have multiple "NVDA,NVMac" nodes and will need to
//	figure out (with bus and slot information) which one you want to use.
#if MONICA_NAMES	// Value set in naming.h
#define DEVICENAMESTRING	"MNCA,Monica"	// Generic NV with Mac ROM
#else
#define DEVICENAMESTRING	"NVDA,NVMac"	// Generic NV with Mac ROM
#endif
#endif
