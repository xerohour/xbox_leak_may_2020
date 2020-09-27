//  **********************************************************************
//  *                                                                    *
//  *   API callback function proto types                                *
//  *                                                                    *
//  **********************************************************************

#ifndef dcvotxs

extern SHF FAR * lpshf;

#define SHCreateProcess     (*lpshf->pSHCreateProcess)
#define SHSetHpid           (*lpshf->pSHSetHpid)
#define SHDeleteProcess     (*lpshf->pSHDeleteProcess)
#define SHChangeProcess     (*lpshf->pSHChangeProcess)
#define SHGetDebugStart     (*lpshf->pSHGetDebugStart)
#define SHGetSymName        (*lpshf->pSHGetSymName)
#define SHAddrFromHsym      (*lpshf->pSHAddrFromHsym)
#define SHHModGetNextGlobal (*lpshf->pSHHModGetNextGlobal)
#define SHModHasSrc         (*lpshf->pSHModHasSrc)
#define SHModelFromAddr     (*lpshf->pSHModelFromAddr)
#define SHPublicNameToAddr  (*lpshf->pSHPublicNameToAddr)
#define SHGetSymbol         (*lpshf->pSHGetSymbol)
#define SHGetPublicAddr     (*lpshf->pSHGetPublicAddr)
#define SHAddDll            (*lpshf->pSHAddDll)
#define SHIsLabel           (*lpshf->pSHIsLabel)
#define SHSetDebuggeeDir    (*lpshf->pSHSetDebuggeeDir)
#define SHUnloadDll         (*lpshf->pSHUnloadDll)
#define SHLoadDll           (*lpshf->pSHLoadDll)
#define SHGetSymLoc         (*lpshf->pSHGetSymLoc)
#define SHFIsAddrNonVirtual (*lpshf->pSHFIsAddrNonVirtual)
#define SHIsFarProc         (*lpshf->pSHIsFarProc)
#define SHAddDllsToProcess  (*lpshf->pSHAddDllsToProcess)

#define SHSetDebuggeeDir    (*lpshf->pSHSetDebuggeeDir)
#define SHSetUserDir        (*lpshf->pSHSetUserDir)
#define SHGetSourceName     (*lpshf->pSHGetSourceName)
#define SHAddrToLabel       (*lpshf->pSHAddrToLabel)

#define SHGetNextExe        (*lpshf->pSHGetNextExe)
#define SHHexeFromHmod      (*lpshf->pSHHexeFromHmod)
#define SHGetNextMod        (*lpshf->pSHGetNextMod)
#define SHGetCxtFromHmod    (*lpshf->pSHGetCxtFromHmod)
#define SHSetCxt            (*lpshf->pSHSetCxt)
#define SHSetCxtMod         (*lpshf->pSHSetCxtMod)
#define SHFindNameInGlobal  (*lpshf->pSHFindNameInGlobal)
#define SHFindNameInContext (*lpshf->pSHFindNameInContext)
#define SHGoToParent        (*lpshf->pSHGoToParent)
#define SHHsymFromPcxt      (*lpshf->pSHHsymFromPcxt)
#define SHNextHsym          (*lpshf->pSHNextHsym)
#define SHGetFuncCxf        (*lpshf->pSHGetFuncCxf)
#define SHGetModName        (*lpshf->pSHGetModName)
#define SHGetFileName       (*lpshf->pSHGetFileName)
#define SHGetExeName        (*lpshf->pSHGetExeName)
#define SHGethExeFromName   (*lpshf->pSHGethExeFromName)
#define SHGetNearestHsym    (*lpshf->pSHGetNearestHsym)
#define SHIsInProlog        (*lpshf->pSHIsInProlog)
#define SHIsAddrInCxt       (*lpshf->pSHIsAddrInCxt)
#define PHGetNearestHsym    (*lpshf->pPHGetNearestHsym)
#define PHFindNameInPublics (*lpshf->pPHFindNameInPublics)
#define THGetTypeFromIndex  (*lpshf->pTHGetTypeFromIndex)
#define THGetNextType       (*lpshf->pTHGetNextType)
#define SHLpGSNGetTable     (*lpshf->pSHLpGSNGetTable)
#define SHFindSymbol        (*lpshf->pSHFindSymbol)
#define SHCanDisplay        (*lpshf->pSHCanDisplay)

#define SLLineFromAddr      (*lpshf->pSLLineFromAddr)
#define SLFLineToAddr       (*lpshf->pSLFLineToAddr)
#define SLNameFromHsf       (*lpshf->pSLNameFromHsf)
#define SLNameFromHmod      (*lpshf->pSLNameFromHmod)
#define SLFQueryModSrc      (*lpshf->pSLFQueryModSrc)
#define SLHmodFromHsf       (*lpshf->pSLHmodFromHsf)
#define SLHsfFromPcxt       (*lpshf->pSLHsfFromPcxt)
#define SLHsfFromFile       (*lpshf->pSLHsfFromFile)
#define SLCAddrFromLine		(*lpshf->pSLCAddrFromLine)
#define SHFree				(*lpshf->pSHFree)
#define SHUnloadSymbolHandler (*lpshf->pSHUnloadSymbolHandler)
#define SHPdbNameFromExe	(*lpshf->pSHPdbNameFromExe)
#define SHIsDllLoaded		(*lpshf->pSHIsDllLoaded)

#define SHGetExeTimeStamp	(*lpshf->pSHGetExeTimeStamp)
#define SHIsThunk           (*lpshf->pSHIsThunk)

#define SHFindSymInExe		(*lpshf->pSHFindSymInExe)

#define SHGetSrcPath        (*lpshf->pSHGetSrcPath)
#define SHGetObjPath        (*lpshf->pSHGetObjPath)
#define SHGetCompileEnv		(*lpshf->pSHGetCompileEnv)
#define SHModSupportsEC     (*lpshf->pSHModSupportsEC)
#define SHExeSupportsEC     (*lpshf->pSHExeSupportsEC)
#define SHModLinkedFromLib  (*lpshf->pSHModLinkedFromLib)

#define SHModFromHmod		(*lpshf->pSHModFromHmod)
#define SHFlushModSyms		(*lpshf->pSHFlushModSyms)
#define SHUseAltPdb			(*lpshf->pSHUseAltPdb)

#else

#define SHGetPublicAddr PHGetAddr

BOOL    LOADDS PASCAL SHCreateProcess       ( VOID );
VOID    LOADDS PASCAL SHSetHpid             ( WORD );
BOOL    LOADDS PASCAL SHDeleteProcess       ( WORD );
VOID    LOADDS PASCAL SHChangeProcess       ( WORD );
BOOL    LOADDS PASCAL SHAddDll              ( LSZ, BOOL );
BOOL    LOADDS PASCAL SHAddDllsToProcess    ( VOID );
SHE     LOADDS PASCAL SHLoadDll             ( LSZ, BOOL );
VOID    LOADDS PASCAL SHUnloadDll           ( HEXE );
UOFFSET LOADDS PASCAL SHGetDebugStart       ( HSYM );
LSZ     LOADDS PASCAL SHGetSymName          ( HSYM, LSZ );
VOID    LOADDS PASCAL SHAddrFromHsym        ( PADDR, HSYM );
HMOD    LOADDS PASCAL SHHModGetNextGlobal   ( HEXE FAR *, HMOD );
int     LOADDS PASCAL SHModelFromAddr       ( PADDR, LPW, UOFFSET FAR * );
int     LOADDS PASCAL SHPublicNameToAddr    ( PADDR, PADDR, LSZ );
LSZ     LOADDS PASCAL SHGetSymbol           ( PADDR, UINT, PADDR, LSZ, LPL );
BOOL    LOADDS PASCAL SHGetPublicAddr       ( PADDR, LSZ );
BOOL    LOADDS PASCAL SHIsLabel             ( HSYM );

VOID    LOADDS PASCAL SHSetDebuggeeDir      ( LSZ );
VOID    LOADDS PASCAL SHSetUserDir          ( LSZ );
BOOL    LOADDS PASCAL SHAddrToLabel         ( PADDR, LSZ );

short   LOADDS PASCAL SHGetSymLoc           ( LPB, LSZ, WORD, PCXT);
BOOL    LOADDS PASCAL SHFIsAddrNonVirtual	( PADDR );
BOOL    LOADDS PASCAL SHIsFarProc           ( LPB );

HEXE    LOADDS PASCAL SHGetNextExe          ( HEXE );
HEXE    LOADDS PASCAL SHHexeFromHmod        ( HMOD );
HMOD    LOADDS PASCAL SHGetNextMod          ( HEXE, HMOD );
PCXT    LOADDS PASCAL SHGetCxtFromHmod      ( HMOD, PCXT );


PCXT    LOADDS PASCAL SHSetCxt              ( PADDR, PCXT );
PCXT    LOADDS PASCAL SHSetCxtMod           ( PADDR, PCXT );
HSYM    LOADDS PASCAL SHFindNameInGlobal    ( HSYM,
                                              PCXT,
                                              HVOID,
                                              SHFLAG,
                                              PFNCMP,
                                              SHFLAG,
                                              PCXT
                                            );
HSYM    LOADDS PASCAL SHFindNameInContext   ( HSYM,
                                              PCXT,
                                              HVOID,
                                              SHFLAG,
                                              PFNCMP,
                                              SHFLAG,
                                              PCXT
                                            );
HSYM    LOADDS PASCAL SHGoToParent          ( PCXT, PCXT );
HSYM    LOADDS PASCAL SHHsymFromPcxt        ( PCXT );
HSYM    LOADDS PASCAL SHNextHsym            ( HMOD, HSYM );
PCXF    LOADDS PASCAL SHGetFuncCxf          ( PADDR, PCXF );
LPCH    LOADDS PASCAL SHGetModName          ( HMOD );
LPCH    LOADDS PASCAL SHGetExeName          ( HEXE );
HEXE    LOADDS PASCAL SHGethExeFromName     ( LPCH );
ULONG   LOADDS PASCAL SHGetNearestHsym      ( PADDR, HMOD, short, PHSYM );
SHFLAG  LOADDS PASCAL SHIsInProlog          ( PCXT );
SHFLAG  LOADDS PASCAL SHIsAddrInCxt         ( PCXT, PADDR );
SHFLAG  LOADDS PASCAL SHCompareRE           ( LPCH, LPCH );
ULONG   LOADDS PASCAL PHGetNearestHsym      ( PADDR, HEXE, PHSYM );
HSYM    LOADDS PASCAL PHFindNameInPublics   ( HSYM,
                                              HEXE,
                                              HVOID,
                                              SHFLAG,
                                              PFNCMP
                                            );
HTYPE   LOADDS PASCAL THGetTypeFromIndex    ( HMOD, THIDX );
HTYPE   LOADDS PASCAL THGetNextType         ( HMOD, HTYPE );


LPV     LOADDS PASCAL SHLpGSNGetTable       ( HEXE );
BOOL    LOADDS PASCAL SHCanDisplay ( HSYM );

// Source Line Handler

BOOL           LOADDS PASCAL SLLineFromAddr ( PADDR, LPW, SHOFF FAR *, SHOFF FAR * );
BOOL		   LOADDS PASCAL SLFLineToAddr	( HSF, SHOFF, LPADDR, SHOFF FAR *, WORD FAR * );
char FAR *     LOADDS PASCAL SLNameFromHsf  ( HSF );
char FAR *     LOADDS PASCAL SLNameFromHmod ( HMOD, WORD );
BOOL           LOADDS PASCAL SLFQueryModSrc ( HMOD );
HMOD           LOADDS PASCAL SLHmodFromHsf  ( HEXE, HSF );
HSF            LOADDS PASCAL SLHsfFromPcxt  ( PCXT );
HSF            LOADDS PASCAL SLHsfFromFile  ( char FAR * );

int		LOADDS PASCAL SLCAddrFromLine ( HEXE, HMOD, LSZ, WORD, LPSLP FAR * );
VOID	LOADDS PASCAL SHFree( LPV );
VOID	LOADDS PASCAL SHUnloadSymbolHandler( VOID );
VOID	LOADDS PASCAL SHPdbNameFromExe( LSZ, LSZ, UINT );
HSYM	LOADDS PASCAL SHFindSymInExe( HEXE, LPSSTR, BOOL );

SHE	LOADDS PASCAL SHGetExeTimeStamp ( LSZ, ULONG * );
BOOL LOADDS PASCAL SHIsThunk( HSYM );

LSZ     LOADDS PASCAL SHGetSrcPath (HMOD);
LSZ     LOADDS PASCAL SHGetObjPath (HMOD);
LSZ     LOADDS PASCAL SHGetCompileEnv (HMOD);
BOOL    LOADDS PASCAL SHModSupportsEC (HMOD);
BOOL    LOADDS PASCAL SHExeSupportsEC (HEXE);
BOOL    LOADDS PASCAL SHModLinkedFromLib (HMOD);

BOOL    LOADDS PASCAL SHModFromHmod( HMOD, PHANDLE );
BOOL    LOADDS PASCAL SHFlushModSyms( HMOD hmod );
BOOL    LOADDS PASCAL SHUseAltPdb ( PFNPDBOpenValidateEx );

#endif


HDEP    LOADDS PASCAL MHMemAllocate( UINT );
HDEP    LOADDS PASCAL MHMemReAlloc(HDEP, UINT);
void    LOADDS PASCAL MHMemFree(HDEP);

HVOID   LOADDS PASCAL MHMemLock(HDEP);
void    LOADDS PASCAL MHMemUnLock(HDEP);
HVOID   LOADDS PASCAL MHOmfLock(HVOID);
void    LOADDS PASCAL MHOmfUnLock(HVOID);
SHFLAG  LOADDS PASCAL MHIsMemLocked(HDEP);

SHFLAG  LOADDS PASCAL DHExecProc(PADDR, SHCALL);
UINT    LOADDS PASCAL DHGetDebuggeeBytes(ADDR, UINT, void FAR *);
UINT    LOADDS PASCAL DHPutDebuggeeBytes(ADDR, UINT, void FAR *);
PSHREG	LOADDS PASCAL DHGetReg(PSHREG, PCXT);
PSHREG	LOADDS PASCAL DHSetReg(PSHREG, PCXT);
HDEP	LOADDS PASCAL DHSaveReg(PCXT);
void	LOADDS PASCAL DHRestoreReg(HDEP, PCXT);
SHFLAG LOADDS PASCAL DHSetupExecute(LPHIND lphdep);
SHFLAG LOADDS PASCAL DHStartExecute(HIND hdep, LPADDR lpaddr, BOOL fIgnoreEvents, SHCALL shcall);
SHFLAG LOADDS PASCAL DHCleanUpExecute(HIND hdep);

