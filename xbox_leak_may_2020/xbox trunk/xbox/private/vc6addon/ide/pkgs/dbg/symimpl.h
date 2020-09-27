#ifndef _SYMIMPL_H_
#define _SYMIMPL_H_
//	Implementation of new interface for SAPI

class ISymbolHandlerImpl : public ISymbolHandler
{
public:
    ISymbolHandlerImpl( SHF* pshf ) : m_pshf( pshf ) 
    {}
    virtual SHE     SHAddDll             (LSZ lsz, BOOL flg)         // Changed for NT
    {
        return (*m_pshf->pSHAddDll)( lsz, flg );
    }
    virtual SHE     SHAddDllsToProcess   (VOID)    {
        return (*m_pshf->pSHAddDllsToProcess)();
    }

    virtual SHE     SHLoadDll            (LSZ lsz, BOOL flg)    {
        return (*m_pshf->pSHLoadDll)( lsz, flg );
    }

    virtual VOID    SHUnloadDll          (HEXE hexe)    {
        (*m_pshf->pSHUnloadDll)( hexe );
    }

    virtual UOFFSET SHGetDebugStart      (HSYM hsym)    {
        return (*m_pshf->pSHGetDebugStart)( hsym );
    }

    virtual LSZ     SHGetSymName         (HSYM hsym, LSZ lsz)    {
        return (*m_pshf->pSHGetSymName)( hsym, lsz );
    }

    virtual BOOL    SHAddrFromHsym       (PADDR paddr, HSYM hsym)    {
        return (*m_pshf->pSHAddrFromHsym)( paddr, hsym );
    }
    virtual HMOD    SHHModGetNextGlobal  (HEXE *hexe , HMOD hmod)    {
        return (*m_pshf->pSHHModGetNextGlobal)( hexe, hmod );
    }

    virtual int     SHModelFromAddr      (PADDR paddr, LPW lpw, LPB lpb, UOFFSET *off)    {
        return (*m_pshf->pSHModelFromAddr)( paddr, lpw, lpb, off );
    }

    virtual int     SHPublicNameToAddr   (PADDR paddr1, PADDR paddr2, LSZ lsz)    {
        return (*m_pshf->pSHPublicNameToAddr)( paddr1, paddr2, lsz );
    }

    virtual LSZ     SHGetSymbol          (LPADDR paddr1, LPADDR paddr2, SOP sop, LPODR podr)    {
        return (*m_pshf->pSHGetSymbol)( paddr1, paddr2, sop, podr );
    }

    virtual BOOL    SHGetPublicAddr      (PADDR paddr, LSZ lsz)    {
        return (*m_pshf->pSHGetPublicAddr)( paddr, lsz );
    }

    virtual BOOL    SHIsLabel            (HSYM hsym )    {
        return (*m_pshf->pSHIsLabel)( hsym );
    }


    virtual VOID    SHSetDebuggeeDir     (LSZ lsz)    {
        (*m_pshf->pSHSetDebuggeeDir)( lsz );
    }

    virtual BOOL    SHAddrToLabel        (PADDR paddr, LSZ lsz)    {
        return (*m_pshf->pSHAddrToLabel)( paddr, lsz );
    }


    virtual int     SHGetSymLoc          (HSYM hsym, LSZ lsz, UINT ui, PCXT pcxt)    {
        return (*m_pshf->pSHGetSymLoc)( hsym, lsz, ui, pcxt );
    }

    virtual BOOL    SHFIsAddrNonVirtual  (PADDR paddr)    {
        return (*m_pshf->pSHFIsAddrNonVirtual)( paddr );
    }

    virtual BOOL    SHIsFarProc          (HSYM hsym )    {
        return (*m_pshf->pSHIsFarProc)( hsym );
    }


    virtual HEXE    SHGetNextExe         (HEXE hexe)    {
        return (*m_pshf->pSHGetNextExe)( hexe );
    }

    virtual HEXE    SHHexeFromHmod       (HMOD hmod )    {
        return (*m_pshf->pSHHexeFromHmod)( hmod );
    }

    virtual HMOD    SHGetNextMod         (HEXE hexe, HMOD hmod )    {
        return (*m_pshf->pSHGetNextMod)( hexe, hmod );
    }

    virtual PCXT    SHGetCxtFromHmod     (HMOD hmod, PCXT pcxt )    {
        return (*m_pshf->pSHGetCxtFromHmod)( hmod, pcxt );
    }

    virtual PCXT    SHSetCxt             (PADDR paddr, PCXT pcxt )    {
        return (*m_pshf->pSHSetCxt)( paddr, pcxt );
    }

    virtual PCXT    SHSetCxtMod          (PADDR paddr, PCXT pcxt )    {
        return (*m_pshf->pSHSetCxtMod)( paddr, pcxt );
    }

    virtual HSYM    SHFindNameInGlobal   (HSYM hsym,
                                             PCXT pcxt,
                                             LPSSTR pstr,
                                             SHFLAG flg,
                                             PFNCMP pfn,
                                             PCXT pcxt2
                                            )    {
        return (*m_pshf->pSHFindNameInGlobal)( hsym, pcxt, pstr, flg, pfn, pcxt2 );
    }

    virtual HSYM    SHFindNameInContext  (HSYM hsym,
                                             PCXT pcxt,
                                             LPSSTR pstr,
                                             SHFLAG flg,
                                             PFNCMP pfn,
                                             PCXT pcxt2
                                            )    {
        return (*m_pshf->pSHFindNameInContext)( hsym, pcxt, pstr, flg, pfn, pcxt2 );
    }

    virtual HSYM    SHGoToParent         (PCXT pcxt, PCXT pcxt2 )    {
        return (*m_pshf->pSHGoToParent)( pcxt, pcxt2 );
    }

    virtual HSYM    SHHsymFromPcxt       (PCXT pcxt )    {
        return (*m_pshf->pSHHsymFromPcxt)( pcxt );
    }

    virtual HSYM    SHNextHsym           (HMOD hmod, HSYM hsym )    {
        return (*m_pshf->pSHNextHsym)( hmod, hsym );
    }

    virtual LPCH    SHGetModName         (HMOD hmod)    {
        return (*m_pshf->pSHGetModName)( hmod );
    }

    virtual LPCH    SHGetExeName         (HEXE hexe)    {
        return (*m_pshf->pSHGetExeName)( hexe );
    }

    virtual HEXE    SHGethExeFromName    (LPCH pch )    {
        return (*m_pshf->pSHGethExeFromName)( pch );
    }

    virtual UOFF32  SHGetNearestHsym     (PADDR pa, HMOD hmod, int i, PHSYM phsym)    {
        return (*m_pshf->pSHGetNearestHsym)( pa, hmod, i, phsym );
    }

    virtual SHFLAG  SHIsInProlog         (PCXT pcxt )    {
        return (*m_pshf->pSHIsInProlog)( pcxt );
    }

    virtual SHFLAG  SHIsAddrInCxt        (PCXT pcxt, PADDR pa)    {
        return (*m_pshf->pSHIsAddrInCxt)( pcxt, pa );
    }

    virtual SHFLAG  SHCompareRE          (LPCH pch, LPCH pch2, BOOL flg)    {
        return (*m_pshf->pSHCompareRE)( pch, pch2, flg );
    }

    virtual BOOL    SHFindSymbol         (LSZ lsz, PADDR pa, LPASR pasr)    {
        return (*m_pshf->pSHFindSymbol)( lsz, pa, pasr );
    }

    virtual UOFF32  PHGetNearestHsym     (PADDR pa, HEXE hexe, PHSYM phsym)    {
        return (*m_pshf->pPHGetNearestHsym)( pa, hexe, phsym );
    }

    virtual HSYM    PHFindNameInPublics  (HSYM hsym, HEXE hexe, LPSSTR pstr, SHFLAG flg, PFNCMP pfn)    {
        return (*m_pshf->pPHFindNameInPublics)( hsym, hexe, pstr, flg, pfn );
    }

    virtual HTYPE   THGetTypeFromIndex   (HMOD hmod, THIDX thidx)    {
        return (*m_pshf->pTHGetTypeFromIndex)( hmod, thidx );
    }

    virtual HTYPE   THGetNextType        (HMOD hmod, HTYPE htype)    {
        return (*m_pshf->pTHGetNextType)( hmod, htype );
    }

    virtual LPVOID  SHLpGSNGetTable      (HEXE hexe)    {
        return (*m_pshf->pSHLpGSNGetTable)( hexe );
    }

    virtual BOOL    SHCanDisplay         (HSYM hsym)    {
        return (*m_pshf->pSHCanDisplay)( hsym );
    }


    //  Source Line handler API Exports

    virtual BOOL    SLLineFromAddr       (LPADDR pa, LPW pw, SHOFF * off1, SHOFF * off2)    {
        return (*m_pshf->pSLLineFromAddr)( pa, pw, off1, off2 );
    }

    virtual BOOL    SLFLineToAddr        (HSF hsf, WORD w, LPADDR pa, SHOFF * off1, WORD *pw )    {
        return (*m_pshf->pSLFLineToAddr)( hsf, w, pa, off1, pw );
    }

    virtual LPCH    SLNameFromHsf        (HSF hsf )    {
        return (*m_pshf->pSLNameFromHsf)( hsf );
    }

    virtual LPCH    SLNameFromHmod       (HMOD hmod, WORD w)    {
        return (*m_pshf->pSLNameFromHmod)( hmod, w );
    }

    virtual BOOL    SLFQueryModSrc       (HMOD hmod)    {
        return (*m_pshf->pSLFQueryModSrc)( hmod );
    }

    virtual HMOD    SLHmodFromHsf        (HEXE hexe, HSF hsf)    {
        return (*m_pshf->pSLHmodFromHsf)( hexe, hsf );
    }

    virtual HSF     SLHsfFromPcxt        (PCXT pcxt)    {
        return (*m_pshf->pSLHsfFromPcxt)( pcxt );
    }

    virtual HSF     SLHsfFromFile        (HMOD hmod, LSZ lsz)    {
        return (*m_pshf->pSLHsfFromFile)( hmod, lsz );
    }


    virtual int     SLCAddrFromLine      (HEXE hexe, HMOD hmod, LSZ lsz, WORD w, LPSLP *pslp)    {
        return (*m_pshf->pSLCAddrFromLine)( hexe, hmod, lsz, w, pslp );
    }

    virtual VOID    SHFree               (PVOID pv)    {
        (*m_pshf->pSHFree)( pv );
    }

    virtual VOID    SHUnloadSymbolHandler(BOOL flg)    {
        (*m_pshf->pSHUnloadSymbolHandler)( flg );
    }

// REVIEW: piersh
#ifdef NT_BUILD_ONLY
    virtual SHE     SHGetExeTimeStamp    (LPSTR pstr, ULONG * ul)    {
        return (*m_pshf->pSHGetExeTimeStamp)( pstr, ul );
    }

#else
    virtual SHE     SHGetExeTimeStamp    (LPSTR pstr, ULONG* Time, ULONG* Check)    {
        return (*m_pshf->pSHGetExeTimeStamp)( pstr, Time, Check );
    }

#endif
    virtual VOID    SHPdbNameFromExe     (LSZ lszExe, LSZ lszPdb, UINT size)    {
        (*m_pshf->pSHPdbNameFromExe)( lszExe, lszPdb, size );
    }

    virtual LPDEBUGDATA SHGetDebugData   (HEXE hexe)    {
        return (LPDEBUGDATA)((*m_pshf->pSHGetDebugData)( hexe ));
    }

    virtual BOOL    SHIsThunk            (HSYM hsym, THUNK_ORDINAL* ord, ULONG* length)    {
        return (*m_pshf->pSHIsThunk)( hsym, ord, length);
    }

    virtual HSYM    SHFindSymInExe       (HEXE hexe, LPSSTR pstr, BOOL flg)    {
        return (*m_pshf->pSHFindSymInExe)( hexe, pstr, flg );
    }

    virtual HSYM    SHFindSLink32        (PCXT pcxt )    {
        return (*m_pshf->pSHFindSLink32)( pcxt );
    }

    virtual BOOL    SHIsDllLoaded        (HEXE hexe)    {
        return (*m_pshf->pSHIsDllLoaded)( hexe );
    }


// Entries added for NT work.

    virtual LSZ     SHGetModule          (PADDR pa, LSZ lsz)    {
        return (*m_pshf->pSHGetModule)( pa, lsz );
    }

    virtual PCXT    SHGetCxtFromHexe     (HEXE hexe, PCXT pcxt)    {
        return (*m_pshf->pSHGetCxtFromHexe)( hexe, pcxt );
    }

    virtual LPCH    SHGetModNameFromHexe (HEXE hexe)    {
        return (*m_pshf->pSHGetModNameFromHexe)( hexe );
    }

    virtual LPCH    SHGetSymFName        (HEXE hexe)    {
        return (*m_pshf->pSHGetSymFName)( hexe );
    }

    virtual HEXE    SHGethExeFromModuleName (LPCH pch)    {
        return (*m_pshf->pSHGethExeFromModuleName)( pch );
    }

    virtual LSZ     SHLszGetErrorText    (SHE she)    {
        return (*m_pshf->pSHLszGetErrorText)( she );
    }

    virtual BOOL    SHWantSymbols        (HEXE hexe)    {
        return (*m_pshf->pSHWantSymbols)( hexe );
    }

    virtual HSYM    SHFindNameInTypes    ( PCXT pcxt, LPSSTR pstr, SHFLAG flg, PFNCMP pfn, PCXT pcxt2 )    {
        return (*m_pshf->pSHFindNameInTypes)( pcxt, pstr, flg, pfn, pcxt2 );
    }


// Entries added for separate type pools work
    virtual BOOL    THAreTypesEqual      (HMOD hmod, CV_typ_t t1, CV_typ_t t2)    {
        return (*m_pshf->pTHAreTypesEqual)( hmod, t1, t2 );
    }


// LoadDllEx is for V7 only (right now)
	virtual SHE     SHLoadDllEx          (LSZ	szName,
											 BOOL	fLoading,
											 ULONG	TimeStamp,
											 ULONG	CheckSum,
											 ULONG	LoadAddress,
											 HEXE*	lphexe)    {
        return (*m_pshf->pSHLoadDllEx)(szName, fLoading, TimeStamp, CheckSum, LoadAddress, lphexe );
    }

											 
// Entries for Edit and Continue work
    virtual LSZ     SHGetSrcPath (HMOD hmod)    {
        return (*m_pshf->pSHGetSrcPath)( hmod );
    }

    virtual LSZ     SHGetObjPath (HMOD hmod)    {
        return (*m_pshf->pSHGetObjPath)( hmod );
    }

    virtual LSZ     SHGetCompileEnv (HMOD hmod)    {
        return (*m_pshf->pSHGetCompileEnv)( hmod );
    }

    virtual BOOL    SHModSupportsEC(HMOD hmod)    {
        return (*m_pshf->pSHModSupportsEC)( hmod );
    }

    virtual BOOL    SHExeSupportsEC (HEXE hexe)    {
        return (*m_pshf->pSHExeSupportsEC)( hexe );
    }

    virtual BOOL    SHModLinkedFromLib (HMOD hmod)    {
        return (*m_pshf->pSHModLinkedFromLib)( hmod );
    }


    virtual BOOL    SHModFromHmod ( HMOD hmod, PHANDLE phandle )    {
        return (*m_pshf->pSHModFromHmod)( hmod, phandle );
    }
    virtual BOOL    SHFlushModSyms( HMOD hmod )    {
        return (*m_pshf->pSHFlushModSyms)( hmod );
    }
    virtual void    SHUseAltPdb ( PFNPDBOpenValidateEx pfn )    {
        (*m_pshf->pSHUseAltPdb)( pfn );
    }
private:
    SHF* m_pshf;
};

#endif
