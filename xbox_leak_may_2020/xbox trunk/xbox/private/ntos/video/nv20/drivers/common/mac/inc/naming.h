/*
	naming.h -- use 'Monica' or NVIDIA naming scheme
*/

#ifndef __NAMING__
#define __NAMING__

#define MONICA_NAMES	0		// Set to 1 if drivers use 'Monica' names, 0 if NVIDIA names

#if MONICA_NAMES
#define PROP_PREFIX		"MNCA,"	// prefix value for Name Registry properties
#else
#define PROP_PREFIX		"NVDA,"
#endif

#endif
