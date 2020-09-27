//  shinit
//
//  Copyright <C> 1990-94, Microsoft Corporation
//
//      [00] 31-dec-91 DavidGra
//          Add SHFindSymbol API for assembler symbol handling.
//
//      10-Nov-94   BryanT
//          Merge in NT changes.
//          Remove SHF ifdef's, remove non-Win32 hosting, remove FAR/NEAR/PASCAL.
//          Replace SHCritxxx calls with native CritSec calls.
//          Initialize the SymCvt pointers.
//          Use the DBG version macros if NT_BUILD
//          Add the background symbol support, but leave it under if 0 for now.

#include "shinc.hpp"
#pragma hdrstop
#include "version.h"


CRITICAL_SECTION    csSh;   // Global CritSec used for MT safe.
KNF                 knf;    // Kernel functions (init to zero by the loader)
//HMODULE             hLib;   // Handle returned from LoadLibrary on symcvt symcvt
CONVERTPROC         pfConvertSymbolsForImage; // Symcvt ptr.

#ifndef ACTIVEDBG

static SHF shf = {
    sizeof(SHF),
    SHCreateProcess,
    SHSetHpid,
    SHDeleteProcess,
    SHChangeProcess,
    SHAddDll,
    SHAddDllsToProcess,
    SHLoadDll,
    SHUnloadDll,
    SHGetDebugStart,
    SHGetSymName,
    SHAddrFromHsym,
    SHHmodGetNextGlobal,
    SHModelFromAddr,
    SHPublicNameToAddr,
    SHGetSymbol,
    PHGetAddr,
    SHIsLabel,

    SHSetDebuggeeDir,
    SHAddrToLabel,

    SHGetSymLoc,
    SHFIsAddrNonVirtual,
    SHIsFarProc,

    SHGetNextExe,
    SHHexeFromHmod,
    SHGetNextMod,
    SHGetCxtFromHmod,
    SHSetCxt,
    SHSetCxtMod,
    SHFindNameInGlobal,
    SHFindNameInContext,
    SHGoToParent,
    SHHsymFromPcxt,
    SHNextHsym,
    NULL,                       // SHGetFuncCXF
    SHGetModName,
    SHGetExeName,
    SHGethExeFromName,
    SHGetNearestHsym,
    SHIsInProlog,
    SHIsAddrInCxt,
    SHCompareRE,                // SHCompareRE
    SHFindSymbol,
    PHGetNearestHsym,
    PHFindNameInPublics,
    THGetTypeFromIndex,
    THGetNextType,
    SHLpGSNGetTable,
    SHCanDisplay,

    // Source Line Handler API

    SLLineFromAddr,
    SLFLineToAddr,
    SLNameFromHsf,
    SLNameFromHmod,
    SLFQueryModSrc,
    NULL,
    SLHsfFromPcxt,
    SLHsfFromFile,
    SLCAddrFromLine,
    SHFree,
    SHUnloadSymbolHandler,
    SHGetExeTimeStamp,
    SHPdbNameFromExe,
    SHGetDebugData,
    SHIsThunk,
    SHFindSymInExe,
    SHFindSLink32,
    SHIsEmiLoaded,

// Entries added for NT work.

    SHGetModule,
    SHGetCxtFromHexe,
    SHGetModNameFromHexe,
    SHGetSymFName,
    SHGethExeFromModuleName,
    SHLszGetErrorText,
    SHWantSymbols,
	SHFindNameInTypes,
#if CC_LAZYTYPES
    THAreTypesEqual,
#endif
	SHLoadDllEx,
    
    SHGetSrcPath,
    SHGetObjPath,
    SHGetCompileEnv,
    SHModSupportsEC,
    SHExeSupportsEC,
    SHModLinkedFromLib,

    SHModFromHmod,
    SHFlushModSyms,
    SHUseAltPdb
};
#endif

VOID
SHFree(
    LPV lpv
    )
{
    MHFree (lpv);
}

#ifndef ACTIVEDBG

BOOL
SHInit(
    LPSHF  *lplpshf,
    LPKNF   lpknf
    )
{
	// lpknf is allowed to be NULL on 2nd and subsequent calls
	if (lpknf!=NULL)
	{
		assert( lpknf->cb == sizeof(KNF) );

		knf = *lpknf;
	}
	else
	{
		assert( knf.cb==sizeof(KNF) );		// make sure been here before
	    *lplpshf = &shf;
		return TRUE;						// don't init twice
	}


    *lplpshf = &shf;

    // Initialize symbol convert pointers.
#if 0
#ifdef _DEBUG
    hLib = LoadLibrary( "vcsymcvtd.dll" );
#else
    hLib = LoadLibrary( "vcsymcvt.dll" );
#endif
    if (hLib != NULL) {
        pfConvertSymbolsForImage = (CONVERTPROC) GetProcAddress( hLib, "ConvertSymbolsForImage2" );
    }
#else
	pfConvertSymbolsForImage = ConvertSymbolsForImage2;
#endif

    InitializeCriticalSection(&csSh);


#if 0
    // initialize synchronization objects used by background sym loader.

    InitializeCriticalSection( &CsSymbolLoad );
    InitializeCriticalSection( &CsSymbolProcess );
    hEventLoaded = CreateEvent( NULL, FALSE, FALSE, NULL );
#endif

    return FInitLists();
}
#endif

#if defined(DEBUGVER)
DEBUG_VERSION('S','H',"Debug Symbolics handler")
#else
RELEASE_VERSION('S','H',"Debug Symbolics handler")
#endif

DBGVERSIONCHECK();
