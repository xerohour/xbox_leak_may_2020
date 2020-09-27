/*
 *
 * cklibver.c
 *
 * Check library versions
 *
 */

#include <windows.h>
#include <xbeimage.h>
#include <xboxverp.h>
#include <cklibver.h>

/* These are the blanket build approvals */
static struct {
    USHORT wMinVer;
    USHORT wMaxVer;
    BOOL fExpired;
    BOOL fMaxApproved;
} rgver[] = {
    { 3911, 3999, TRUE }, // August 2001 Final libraries
    { 4039, 4099 }, // November 2001 libraries
    { 4134, 4199 }, // December 2001 libraries
    { 4242, 4299 }, // February 2002 libraries
    { VER_PRODUCTBUILD, 0 },
    { 0 }
};

/* These are the approved QFEs.  A QFE marked mandatory will reject earlier
 * versions of that lib, but not later ones */
static struct {
    LPCSTR szLibName;
    USHORT wQFEBuild;
    USHORT wQFENumber;
    BOOL fMandatory;
} rgqfe[] = {
// August01 QFEs
    { "DSOUND\0\0",   3925, 1, TRUE },
    { "DMUSIC\0\0",   3925, 1, FALSE },
    { "DSOUND\0\0",   3936, 1, FALSE },
    { "DMUSIC\0\0",   3941, 1, FALSE },
    { "D3D8\0\0\0\0", 3948, 1, TRUE },  // B#9310: fix flicker filter reset, B#9849: workaround SetRenderTarget hardware issue
    { "D3D8LTCG",     3948, 1, TRUE },  // B#9310: fix flicker filter reset, B#9849: workaround SetRenderTarget hardware issue
    { "DSOUND\0\0",   3949, 1, FALSE }, // B#10527: CWMAXMediaObject inits m_hFile
    { "XAPILIB\0",    3950, 1, TRUE },  // B#10693: verify media in XAPI init

// November01 QFEs
    { "D3D8\0\0\0\0", 4039, 2, TRUE },  // B#9882: W-Buffering fix, B#9849: workaround SetRenderTarget hardware issue
    { "D3D8LTCG",     4039, 2, TRUE },  // B#9882: W-Buffering fix, B#9849: workaround SetRenderTarget hardware issue
    { "DSOUND\0\0",   4039, 3, FALSE }, // B#10527: CWMAXMediaObject inits m_hFile
    { "XAPILIB\0",    4039, 4, TRUE },  // B#10693: verify media in XAPI init

// December01 QFEs
    { "D3D8\0\0\0\0", 4134, 2, FALSE }, // B#10377: SetOverscanColor removed, B#10330: XSASM /h fixed
    { "D3D8LTCG",     4134, 2, FALSE }, // B#10377: SetOverscanColor removed, B#10330: XSASM /h fixed
    { "DSOUND\0\0",   4134, 3, FALSE }, // B#10527: CWMAXMediaObject inits m_hFile, B#10610: fix rolloff discontinuity
    { "DSOUND\0\0",   4134, 5, FALSE }, // B#10527: CWMAXMediaObject inits m_hFile, B#10610: fix rolloff discontinuity, hack around WaitForVoiceOff
    { "DSOUND\0\0",   4134, 6, TRUE },  // WaitForVoiceOff fixes
    { "XAPILIB\0",    4134, 6, TRUE },  // B#10693: verify media in XAPI init
    { "WMVDEC\0\0",   4134, 6, FALSE }, // B#10821: WMV alloc/delete mismatch

    { NULL },
};

/* Dependencies */
static struct {
    LPCSTR szDependentName;
    USHORT wDependentNotOlderThan;
    USHORT wDependentNotNewerThan;
    LPCSTR szSupportingName;
    USHORT wMinimumSupportingVersion;
} rgdep[] = {
    { "DMUSIC\0\0", 3911, 3925, "DSOUND\0\0", 3936 },
    { NULL },
};

BOOL FAcceptableVerBuild(int iver, USHORT wVer)
{
    if(iver < 0)
        return FALSE;
    if(wVer < rgver[iver].wMinVer)
        return FALSE;
    if(rgver[iver].wMaxVer == 0)
        return TRUE;
    if(rgver[iver].fMaxApproved)
        return wVer <= rgver[iver].wMaxVer;
    return wVer < rgver[iver].wMaxVer;
}

int CheckLibraryApprovalStatus(PXBEIMAGE_LIBRARY_VERSION plibvXapi,
    PXBEIMAGE_LIBRARY_VERSION rglibv, int clibv, PLIB_ERROR_FUNCTION pfnErr)
{
    int idep;
    int iqfe;
    int iverXapi;
    int iTotal = 2;
    int i;
    BOOL fExpired;
    PXBEIMAGE_LIBRARY_VERSION plibv = rglibv;
    PXBEIMAGE_LIBRARY_VERSION plibvSup;

    /* Look at the XAPI version number and figure out which range of library
     * versions we'll consider acceptable */
    if(plibvXapi) {
        for(iverXapi = 0; rgver[iverXapi].wMinVer; ++iverXapi) {
            if(FAcceptableVerBuild(iverXapi, plibvXapi->BuildVersion)) {
                fExpired = rgver[iverXapi].fExpired;
                break;
            }
        }
        if(!rgver[iverXapi].wMinVer || plibvXapi->MajorVersion != 1 ||
                plibvXapi->MinorVersion != 0)
            iverXapi = -1;
    } else
        iverXapi = -1;

    for(; clibv--; ++plibv) {
        /* Assume approval at first based on whether XAPI was approved */
        i = iverXapi < 0 ? 0 : -1;

        /* Ensure major and minor version numbers are 1.0 */
        if(plibv->MajorVersion != 1 || plibv->MinorVersion != 0)
            i = 0;
    
        /* Check the QFE list first */
        for(iqfe = 0; i != 0 && rgqfe[iqfe].szLibName; ++iqfe) {
            if(0 == memcmp(plibv->LibraryName, rgqfe[iqfe].szLibName,
                XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH) &&
                FAcceptableVerBuild(iverXapi, rgqfe[iqfe].wQFEBuild))
            {
                /* If this entry represents a mandatory QFE, we'll disallow
                 * this library if it is too old.  If the build number
                 * matches we'll mark the library as approved */
                if(rgqfe[iqfe].fMandatory && (plibv->BuildVersion <
                        rgqfe[iqfe].wQFEBuild || (plibv->BuildVersion ==
                        rgqfe[iqfe].wQFEBuild && plibv->QFEVersion <
                        rgqfe[iqfe].wQFENumber)))
                    i = 0;
                else if(plibv->BuildVersion == rgqfe[iqfe].wQFEBuild &&
                        plibv->QFEVersion == rgqfe[iqfe].wQFENumber)
                    i = 2;
            }
        }
        
        /* Check the base version list if this library isn't already
         * approved */
        if(i < 0) {
            if(iverXapi >= 0 && rgver[iverXapi].wMinVer ==
                    plibv->BuildVersion && plibv->QFEVersion == 1)
                i = 2;
    
            /* If this library hasn't been approved by now, it's unapproved */
            if(i < 0)
                i = 0;
        }
    
        /* The approval status of this library can't exceed the approval
         * stamp in the library itself */
        if(plibv->ApprovedLibrary < i)
            i = plibv->ApprovedLibrary;
    
        /* If this library is at all approved, check the dependency list
         * to make sure we have any necessary supporting libraries */
        if(i > 0) {
            for(idep = 0; rgdep[idep].szDependentName; ++idep) {
                if(0 == memcmp(plibv->LibraryName, rgdep[idep].szDependentName,
                    XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH) &&
                    plibv->BuildVersion >= rgdep[idep].wDependentNotOlderThan &&
                    plibv->BuildVersion <= rgdep[idep].wDependentNotNewerThan)
                {
                    /* This library depends on something else.  Look for it */
                    for(plibvSup = rglibv + clibv; plibvSup-- > rglibv; ) {
                        if(plibvSup != plibv && 0 == memcmp(plibv->LibraryName,
                                rgdep[idep].szSupportingName,
                                XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH) &&
                                FAcceptableVerBuild(iverXapi,
                                plibvSup->BuildVersion) &&
                                plibvSup->BuildVersion >=
                                rgdep[idep].wMinimumSupportingVersion)
                            break;
                    }
                    if(plibvSup < rglibv)
                        /* We needed a supporting lib but didn't find it.
                         * We'll mark this lib unsupported */
                        i = 0;
                }
            }
        }

        /* If this library is approved but XAPI is expired, we mark this
         * library as expired */
        if(i == 2 && fExpired)
            i = -1;

        /* If this library is unapproved, complain about it */
        if(i != 2 && pfnErr)
            (*pfnErr)(plibv, i);

        if(i <= 0 && !pfnErr)
            return 0;

        if(i < iTotal)
            iTotal = i;
    }

    return iTotal < 0 ? 0 : iTotal;
}
