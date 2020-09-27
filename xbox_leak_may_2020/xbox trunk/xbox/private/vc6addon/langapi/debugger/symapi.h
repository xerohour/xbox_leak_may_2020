#ifndef _SYMAPI_H_
#define _SYMAPI_H_
//	New interface for SAPI

//	If someone feels like turning this into IDL, be my guest.

interface ISymbolHandler /* : public IUnknown */
{
    virtual SHE     SHAddDll             (LSZ, BOOL) =0;          // Changed for NT
    virtual SHE     SHAddDllsToProcess   (VOID) =0;
    virtual SHE     SHLoadDll            (LSZ, BOOL) =0;
    virtual VOID    SHUnloadDll          (HEXE) =0;
    virtual UOFFSET SHGetDebugStart      (HSYM) =0;
    virtual LSZ     SHGetSymName         (HSYM, LSZ) =0;
    virtual BOOL    SHAddrFromHsym       (PADDR, HSYM) =0;        // Changed for NT
    virtual HMOD    SHHModGetNextGlobal  (HEXE *, HMOD) =0;
    virtual int     SHModelFromAddr      (PADDR, LPW, LPB, UOFFSET *) =0;
    virtual int     SHPublicNameToAddr   (PADDR, PADDR, LSZ) =0;
    virtual LSZ     SHGetSymbol          (LPADDR, LPADDR, SOP, LPODR) =0;
    virtual BOOL    SHGetPublicAddr      (PADDR, LSZ) =0;
    virtual BOOL    SHIsLabel            (HSYM) =0;

    virtual VOID    SHSetDebuggeeDir     (LSZ) =0;
    virtual BOOL    SHAddrToLabel        (PADDR, LSZ) =0;

    virtual int     SHGetSymLoc          (HSYM, LSZ, UINT, PCXT) =0;
    virtual BOOL    SHFIsAddrNonVirtual  (PADDR) =0;
    virtual BOOL    SHIsFarProc          (HSYM) =0;

    virtual HEXE    SHGetNextExe         (HEXE) =0;
    virtual HEXE    SHHexeFromHmod       (HMOD) =0;
    virtual HMOD    SHGetNextMod         (HEXE, HMOD) =0;
    virtual PCXT    SHGetCxtFromHmod     (HMOD, PCXT) =0;
    virtual PCXT    SHSetCxt             (PADDR, PCXT) =0;
    virtual PCXT    SHSetCxtMod          (PADDR, PCXT) =0;
    virtual HSYM    SHFindNameInGlobal   (HSYM,
                                             PCXT,
                                             LPSSTR,
                                             SHFLAG,
                                             PFNCMP,
                                             PCXT
                                            ) =0;
    virtual HSYM    SHFindNameInContext  (HSYM,
                                             PCXT,
                                             LPSSTR,
                                             SHFLAG,
                                             PFNCMP,
                                             PCXT
                                            ) =0;
    virtual HSYM    SHGoToParent         (PCXT, PCXT) =0;
    virtual HSYM    SHHsymFromPcxt       (PCXT) =0;
    virtual HSYM    SHNextHsym           (HMOD, HSYM) =0;
    virtual LPCH    SHGetModName         (HMOD) =0;
    virtual LPCH    SHGetExeName         (HEXE) =0;
    virtual HEXE    SHGethExeFromName    (LPCH) =0;
    virtual UOFF32  SHGetNearestHsym     (PADDR, HMOD, int, PHSYM) =0;
    virtual SHFLAG  SHIsInProlog         (PCXT) =0;
    virtual SHFLAG  SHIsAddrInCxt        (PCXT, PADDR) =0;
    virtual SHFLAG  SHCompareRE          (LPCH, LPCH, BOOL) =0;
    virtual BOOL    SHFindSymbol         (LSZ, PADDR, LPASR) =0;
    virtual UOFF32  PHGetNearestHsym     (PADDR, HEXE, PHSYM) =0;
    virtual HSYM    PHFindNameInPublics  (HSYM, HEXE, LPSSTR, SHFLAG, PFNCMP) =0;
    virtual HTYPE   THGetTypeFromIndex   (HMOD, THIDX) =0;
    virtual HTYPE   THGetNextType        (HMOD, HTYPE) =0;
    virtual LPVOID  SHLpGSNGetTable      (HEXE) =0;
    virtual BOOL    SHCanDisplay         (HSYM) =0;

    //  Source Line handler API Exports

    virtual BOOL    SLLineFromAddr       (LPADDR, LPW, SHOFF *, SHOFF *) =0;
    virtual BOOL    SLFLineToAddr        (HSF, WORD, LPADDR, SHOFF *, WORD *) =0;
    virtual LPCH    SLNameFromHsf        (HSF) =0;
    virtual LPCH    SLNameFromHmod       (HMOD, WORD) =0;
    virtual BOOL    SLFQueryModSrc       (HMOD) =0;
    virtual HMOD    SLHmodFromHsf        (HEXE, HSF) =0;
    virtual HSF     SLHsfFromPcxt        (PCXT) =0;
    virtual HSF     SLHsfFromFile        (HMOD, LSZ) =0;

    virtual int     SLCAddrFromLine      (HEXE, HMOD, LSZ, WORD, LPSLP *) =0;
    virtual VOID    SHFree               (PVOID) =0;
    virtual VOID    SHUnloadSymbolHandler(BOOL) =0;
// REVIEW: piersh
#ifdef NT_BUILD_ONLY
    virtual SHE     SHGetExeTimeStamp    (LPSTR, ULONG *) =0;
#else
    virtual SHE     SHGetExeTimeStamp    (LPSTR, ULONG* Time, ULONG* Check) =0;
#endif
    virtual VOID    SHPdbNameFromExe     (LSZ, LSZ, UINT) =0;
    virtual LPDEBUGDATA SHGetDebugData   (HEXE) =0;
    virtual BOOL    SHIsThunk            (HSYM, THUNK_ORDINAL*, ULONG* length) =0;
    virtual HSYM    SHFindSymInExe       (HEXE, LPSSTR, BOOL) =0;
    virtual HSYM    SHFindSLink32        (PCXT) =0;
    virtual BOOL    SHIsDllLoaded        (HEXE) =0;

// Entries added for NT work.

    virtual LSZ     SHGetModule          (PADDR, LSZ) =0;
    virtual PCXT    SHGetCxtFromHexe     (HEXE, PCXT) =0;
    virtual LPCH    SHGetModNameFromHexe (HEXE) =0;
    virtual LPCH    SHGetSymFName        (HEXE) =0;
    virtual HEXE    SHGethExeFromModuleName (LPCH) =0;
    virtual LSZ     SHLszGetErrorText    (SHE) =0;
    virtual BOOL    SHWantSymbols        (HEXE) =0;
    virtual HSYM    SHFindNameInTypes    ( PCXT, LPSSTR, SHFLAG, PFNCMP, PCXT ) =0;

// Entries added for separate type pools work
    virtual BOOL    THAreTypesEqual      (HMOD, CV_typ_t, CV_typ_t) =0;

// LoadDllEx is for V7 only (right now)
	virtual SHE     SHLoadDllEx          (LSZ	szName,
											 BOOL	fLoading,
											 ULONG	TimeStamp,
											 ULONG	CheckSum,
											 ULONG	LoadAddress,
											 HEXE*	lphexe) =0;
											 
// Entries for Edit and Continue work
    virtual LSZ     SHGetSrcPath (HMOD) =0;
    virtual LSZ     SHGetObjPath (HMOD) =0;
    virtual LSZ     SHGetCompileEnv (HMOD) =0;
    virtual BOOL    SHModSupportsEC(HMOD) =0;
    virtual BOOL    SHExeSupportsEC (HEXE) =0;
    virtual BOOL    SHModLinkedFromLib (HMOD) =0;

    virtual BOOL    SHModFromHmod ( HMOD, PHANDLE ) =0;	// get the underlying Pdb Mod
    virtual BOOL    SHFlushModSyms( HMOD ) =0;	// flush symbols for mod
    virtual void    SHUseAltPdb ( PFNPDBOpenValidateEx ) =0;
};

typedef ISymbolHandler* (__cdecl *LPFNSHINIT2)( KNF*, HPID hpid, HTID htid );

#endif
