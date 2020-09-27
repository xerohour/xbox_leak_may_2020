// Edit and Continue

#include "shinc.hpp"
#pragma hdrstop
#include "cvr.h"
#include "cvinfo.h"
#include "csymbuf.h"
#include "comenvi.h"

extern VOID KillSci(SymConvertInfo &);
extern VOID VoidCaches(VOID);
extern HST HstFromLpmds (LPMDS);

//
// SHGetSrcPath
//
//  Returns a pointer to static memory that will be overwritten by
//  the next call. Not thread safe.
//
LSZ SHGetSrcPath(HMOD hmod)
{
    static _TCHAR      szFullPath[_MAX_PATH];
    szFullPath[0] = _T('\0');

    if ( !hmod ) {
        return NULL;
    }

    LPMDS lpmds = (LPMDS) hmod;
    if (lpmds->pmod) {
        CB cb = sizeof( szFullPath );
        ModQuerySrcFile( lpmds->pmod, szFullPath, &cb );
    } 
    return szFullPath;
}

//
// SHGetObjPath
//
//  Returns a pointer to static memory that will be overwritten by
//  the next call. Not thread safe.
//
LSZ SHGetObjPath(HMOD hmod)
{
    if ( !hmod ) {
        return NULL;
    }

    static _TCHAR      szFullPath[_MAX_PATH];
    szFullPath[0] = _T('\0');

    LPMDS lpmds = (LPMDS) hmod;
    PB symbuf = reinterpret_cast<PB>( GetSymbols( lpmds ) );
    if ( symbuf ) {
        // find the first S_OBJNAME symbol
        SymBuffer buffer( symbuf, lpmds->cbSymbols );
		for ( SymBuffer::iterator it = buffer.begin(); it != buffer.end(); ++it ) {
            if ( (*it).rectyp == S_OBJNAME ) {
                OBJNAMESYM* psym = reinterpret_cast<OBJNAMESYM*>( &*it );
                memcpy( szFullPath, &psym->name[1], psym->name[0] );
                szFullPath[ psym->name[0] ] = _T('\0');
                break;
            }
        }
    }
    return szFullPath;
}

//
// SHGetCompileEnv
//
//  Returns a pointer to cached symbol data from the pdb.
//      the string is of the form
//      [name\0value\0]*\0
//
LSZ SHGetCompileEnv(HMOD hmod)
{
    if ( !hmod ) {
        return NULL;
    }

    LPMDS lpmds = (LPMDS) hmod;
    PB symbuf = reinterpret_cast<PB>( GetSymbols( lpmds ) );
    if ( symbuf ) {
        // find the first S_COMPILE2 symbol
        SymBuffer buffer( symbuf, lpmds->cbSymbols );
		for ( SymBuffer::iterator it = buffer.begin(); it != buffer.end(); ++it ) {
            if ( (*it).rectyp == S_COMPILE2 ) {
                COMPILESYM* psym = reinterpret_cast<COMPILESYM*>( &*it );
                return reinterpret_cast<char*>(&psym->verSt[1])+psym->verSt[0];
            }
        }
    }
    return NULL;
}

BOOL SHModSupportsEC(HMOD hmod)
{
    if ( !hmod ) {
        return FALSE;
    }

    LPMDS lpmds = (LPMDS) hmod;
    if (lpmds->pmod) {
        return ModQuerySupportsEC( lpmds->pmod );
    }
    return FALSE;
}


BOOL SHExeSupportsEC(HEXE hexe)
{
    BOOL fRet = FALSE;
    if ( !hexe ) {
        return FALSE;
    }

    LPEXE lpexe = (LPEXE) LLLock ( hexe );
    LPEXG lpexg = (LPEXG) LLLock ( lpexe->hexg );

    if (lpexg->ppdb && lpexg->pdbi) {
        fRet = DBIQuerySupportsEC( lpexg->pdbi );
    }

    LLUnlock (lpexe->hexg);
    LLUnlock (hexe);

    return fRet;   
}

BOOL SHModLinkedFromLib(HMOD hmod)
{
    static _TCHAR      szNamePath[_MAX_PATH];
    static _TCHAR      szFilePath[_MAX_PATH];
    szNamePath[0] = _T('\0');
    szFilePath[0] = _T('\0');

    if ( !hmod ) {
        return NULL;
    }

    LPMDS lpmds = (LPMDS) hmod;
    if (lpmds->pmod) {
        // REVIEW: is it sufficient to compare that the two string?
        CB cb = sizeof( szNamePath );
        ModQueryName( lpmds->pmod, szNamePath, &cb );
        cb = sizeof( szFilePath );
        ModQueryFile( lpmds->pmod, szFilePath, &cb );
        return _tcscmp( szNamePath, szFilePath ) != 0;
    }
    return FALSE;
}

BOOL SHModFromHmod( HMOD hmod, PHANDLE ppmod )	// get the underlying Pdb Mod
{
    if ( hmod == 0 || ppmod == 0 ) {
        return false;
    }

    LPMDS lpmds = (LPMDS) hmod;
    *ppmod = lpmds->pmod;
    return lpmds->pmod != 0;
}

BOOL SHFlushModSyms( HMOD hmod )	        // flush symbols for mod
{
    VoidCaches();
    if ( !hmod ) return false;

    LPMDS   lpmds = (LPMDS) hmod;

    if ( !lpmds->pmod ) return false;   // only implemented for pdb symbols

    assert( lpmds->lpsgc == 0 );    // should not use this when we have a pdb?

    if (lpmds->symbols) {           // delete the symbols cache
        MHFree(lpmds->symbols);
#if CC_CVTYPE32
        KillSci(lpmds->sci);
#endif
        lpmds->symbols = 0;
        lpmds->cbSymbols = 0;
    }

    if (lpmds->hst) {               // delete the line numbers cache
        MHFree (lpmds->hst);
        lpmds->hst = 0;
    }
    // now immediatly reload line numbers
    HstFromLpmds( lpmds );
    return true;
}


void SHUseAltPdb( PFNPDBOpenValidateEx pfn )
{
    extern PFNPDBOpenValidateEx pAltPDBOpen;
    assert( pAltPDBOpen == 0 );
    assert( pfn != 0 );
    pAltPDBOpen = pfn;
}

