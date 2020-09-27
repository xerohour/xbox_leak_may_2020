// ---------------------------------------------------------------------------------------
// util.h
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef __UTIL_H__
#define __UTIL_H__

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#include <xcrypt.h>

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------------------

//
// Define the real RC4 SHA1 structures and functions
//
#define RC4_CONFOUNDER_LEN  8

typedef struct _RC4_SHA_HEADER {
    BYTE Checksum[XC_DIGEST_LEN];
    BYTE Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA_HEADER, *PRC4_SHA_HEADER;

//
// Define a structure that describes an extended CFFOLDER entry
// that also has an associated HMAC (per-folder reserved data)
//
typedef struct
{
	CFFOLDER		cffolder;		// CFFOLDER
	RC4_SHA_HEADER	hmac;			// RC4 SHA1 Hmac

} CFFOLDER_HMAC, *PCFFOLDER_HMAC;

//
// Define a union of all known CAB flags
//
#define cfhdrFLAGS_ALL	(cfhdrPREV_CABINET | \
						 cfhdrNEXT_CABINET | \
						 cfhdrRESERVE_PRESENT)

//
// Define our header cache structure
//
typedef struct 
{
	CFHEADER				cfheader;		// Cabinet file header
	CFRESERVE				cfreserve;		// Reserved data specification
	
	RC4_SHA_HEADER			digestFolders;	// CFFOLDERS digest
	RC4_SHA_HEADER			digestFiles;	// CFFILES digest
	
} XONLINECONTENT_HEADER, *PXONLINECONTENT_HEADER;


//
// Define sizes related to the fixed header
//
#define XONLINECONTENT_FIXED_HEADER_SIZE	\
			(sizeof(XONLINECONTENT_HEADER))

#define XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC	\
			(XONLINECONTENT_FIXED_HEADER_SIZE + \
			 sizeof(RC4_SHA_HEADER))

#define XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT	\
			(XONLINECONTENT_FIXED_HEADER_SIZE + \
			 sizeof(RC4_SHA_HEADER) + sizeof(DWORD))

#define XONLINECONTENT_COMPLETE_HEADER_SIZE	\
			(XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT + \
			 XONLINECONTENT_PK_SIGNATURE_SIZE)

#define XONLINECONTENT_CHECKPOINT	\
			(XONLINECONTENT_FIXED_HEADER_SIZE + \
			 sizeof(RC4_SHA_HEADER))

#define XONLINECONTENT_RESUME_MAX_READ_SIZE	\
			(XONLINECONTENT_COMPLETE_HEADER_SIZE + \
			 XONLINECONTENT_MAX_CFFOLDER * sizeof(CFFOLDER_HMAC))

//
// Define the maximum number of CFFOLDER entries allowed in a 
// content package
//
#define XONLINECONTENT_MAX_CFFOLDER			((DWORD)100)

//
// Define the size of a PK signature
//
#define XONLINECONTENT_PK_SIGNATURE_SIZE	256

//
// Define the max size of a public key
//
#define XONLINECONTENT_MAX_PUBLIC_KEY_SIZE	\
		(XONLINECONTENT_PK_SIGNATURE_SIZE + \
		 sizeof(BSAFE_PUB_KEY) + \
		 (sizeof(DWORD) * 4))


//
// Define the various reserve values
//
#define XONLINECONTENT_HEADER_RESERVE_SIZE	\
			((sizeof(RC4_SHA_HEADER) * 3) + sizeof(DWORD) + \
			 XONLINECONTENT_PK_SIGNATURE_SIZE)

#define XONLINECONTENT_PERFOLDER_RESERVE_SIZE	\
			(sizeof(RC4_SHA_HEADER))

//
// Define the smallest possible size for a content package
//
#define XONLINECONTENT_MIN_CONTENT_SIZE	\
			(sizeof(CFHEADER) + sizeof(CFRESERVE) + \
			 XONLINECONTENT_HEADER_RESERVE_SIZE)


//
// Define the name of the manifest file
//
#define XONLINECONTENT_MANIFEST_FILE_NAME			"manifest.xbx"

//
// Define the name of the special target manifest file
//
#define XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME	"target.xbx"

//
// We deal with clusters a lot, so define the size of a cluster
//
#define XBOX_CLUSTER_SIZE							(1 << 14)

//
// Define a reasonable upper bound for the manifest size (1MB)
//
#define XONLINECONTENT_MANIFEST_MAX_FILE_SIZE		(1<<20)


// ---------------------------------------------------------------------------------------

#endif

