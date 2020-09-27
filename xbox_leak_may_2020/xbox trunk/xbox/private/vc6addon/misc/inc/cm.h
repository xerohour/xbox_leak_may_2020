/*************************************************************************
	mso96.h

	Owner: rickp
	Copyright (c) 1994 Microsoft Corporation

	The main Office header file.
*************************************************************************/

#ifndef CM_H
#define CM_H

// Basic platform defines, etc.
#include <cmstd.h>


#ifndef RC_INVOKED
	/* REVIEW: what should these pack values be? */
	#if MAC
		#pragma pack(push,1)
	#else
		#pragma pack(push,8)
	#endif
#endif

// Office-global initialization and interfaces
#include <cmuser.h>

// Office Debugging
#include <cmdebug.h>

// Office memory and global variables
#include <cmalloc.h>




// Office Component Integration
#include <CmpMgr.h>



#ifndef RC_INVOKED
	#pragma pack(pop)
#endif

#endif // CM_H
