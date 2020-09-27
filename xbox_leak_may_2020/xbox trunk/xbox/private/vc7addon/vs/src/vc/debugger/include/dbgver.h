/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    dbgver.h

Abstract:

Author:

    Kent D. Forschmiedt (a-kentf) 25-Jan-1993

Environment:

    Win32, User Mode

--*/

#if ! defined _DBGVER_
#define _DBGVER_

#include <version.h>

/*
**  DBG_API_VERSION is the major version number used to specify the
**      api version of the debugger or debug dll.  For release versions
**      dlls will export this and debuggers will check against this
**      version to verify that it can use the dll.
**
**      For beta and debug versions, this number will be used in
**      conjunction with minor and revision numbers (probably derived
**      from SLM rmm & rup) to verify compatibility.
**
**      Until the API has stabilized, we will most likely have to
**      rev this version number for every major product release.
**
*/

#include "dbapiver.h"

/*  AVS - Api Version Structure:
**
**      All debug dlls should be prepared to return a pointer to this
**      structure conaining its vital statistics.  The debugger should
**      check first two characters of the dll's name against rgchType
**      and the version numbers as described in the DBG_API_VERSION
**      and show the user an error if any of these tests fail.
**
*/

typedef enum {
    rlvtRelease,
    rlvtBeta,
    rlvtDebug
} RLVT;     // ReLease Version Type

//
// WARNING!!!!
//
// For backward compatibility reasons - do not change the existing
// members of this structure in any WAY (name change ok / type /size change
// not ok)
//
// If new members need to be added - add to end of structure
//
// rajra
//

typedef struct _AVS {
    CHAR	rgchType [ 2 ];    // Component name (EE,EM,TL,SH,DM)
    WORD    rlvt;              // ReLease Version Type
    BYTE	iApiVer;           // DBG_API_VERSION
    BYTE	iApiSubVer;        // DBG_API_SUBVERSION
    WORD	iRup;              // Revision number
    DWORD	chBuild;           // Build of revision # (a,b,c,d)
    MPT		mpt;               // CPU binary is running on
    WORD	iRmj;              // Major version number
    WORD	iRmm;              // Minor version number
	union {
	LSZ		lszTitle;          // User readable text describing the DLL
	DWORDLONG Align;		   // make structure same size on all platforms
	};
} AVS;  // Api Version Structure
typedef AVS FAR *LPAVS;


/*  DBGVersionCheck:
**
**      All debug dlls should provide this API and support the return
**      of a pointer to the structure described above even before
**      initialization takes place.
*/

#if defined(_M_IX86)
#define __dbgver_cpu__ mptix86
#elif defined(_M_MRX000)
#define __dbgver_cpu__ mptmips
#elif defined(_M_ALPHA)
#define __dbgver_cpu__ mptdaxp
#elif defined(_M_PPC)
#define __dbgver_cpu__ mptntppc
#elif defined(_M_MPPC)
#define __dbgver_cpu__ mptmppc
#elif defined(_M_IA64)
#define __dbgver_cpu__ mptia64
#else
#error( "unknown target machine" );
#endif
//v-vadimp - changed the order to move the LSZ Title (only 32/64 bit var) to the last position to keep the structure aligned properly for 32/64 bit
#define DEBUG_VERSION(C1,C2,TITLE) \
AVS Avs = {      \
    { C1, C2 },         \
    rlvtDebug,          \
    DBG_API_VERSION,    \
    DBG_API_SUBVERSION, \
    rup,                \
    0,                  \
    __dbgver_cpu__,     \
    rmj,                \
    rmm,                \
    TITLE,              \
    };

#define RELEASE_VERSION(C1,C2,TITLE)    \
AVS Avs = {      \
    { C1, C2 },         \
    rlvtRelease,        \
    DBG_API_VERSION,    \
    DBG_API_SUBVERSION, \
    rup,                \
    0,                  \
    __dbgver_cpu__,     \
    rmj,                \
    rmm,                \
    TITLE,              \
};

#ifdef _DEBUG                   
	#define VERSION_INFO(C1,C2,TITLE)	DEBUG_VERSION(C1,C2,TITLE)	        
#else
	#define VERSION_INFO(C1,C2,TITLE)	RELEASE_VERSION(C1,C2,TITLE)	        
#endif                          


#undef MINOR
#undef MAJOR

inline bool 
operator==(
		const AVS& avs1,
		const AVS& avs2
		)
{
	if ( avs1.iApiVer    != avs2.iApiVer    || 
	     avs1.iApiSubVer != avs2.iApiSubVer || 
		 avs1.iRmj       != avs2.iRmj       || 
	     avs1.iRmm       != avs2.iRmm       ||  
		 avs1.rlvt       != avs2.rlvt   ) {

		assert(!"Version Mismatch");
		return false;
	}

	assert ( ( avs1.iRup == avs2.iRup && avs1.chBuild == avs2.chBuild ) ||
			 !"Version Mismatch - Proceed at your own risk");

	return true;
}
inline bool 
operator!=(
		const AVS& avs1,
		const AVS& avs2
		)
{

	return !(avs1 == avs2);
}
#define DBGVERSIONPROCNAME "OSDebug4VersionCheck"

typedef LPAVS (*DBGVERSIONPROC)(void);
LPAVS WINAPI OSDebug4VersionCheck( void );
LPAVS WINAPI WINDBGVersionCheck( void );


#define DBGVERSIONCHECK() \
    LPAVS WINAPI WINDBGVersionCheck( void ) { return &Avs; } \
    LPAVS WINAPI OSDebug4VersionCheck( void ) { return &Avs; }

#endif // _DBGVER_
