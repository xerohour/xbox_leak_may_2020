// // Xbox Platform
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"     // our standard AFX include
#pragma hdrstop
#include "platxbox.h"    // our local header file

#include <bldapi.h>
#include <bldguid.h>

IMPLEMENT_DYNAMIC(CProjTypeXboxExe, CProjType)
#ifdef XBOXDLL
IMPLEMENT_DYNAMIC(CProjTypeXboxDll, CProjType)
#endif
IMPLEMENT_DYNAMIC(CProjTypeXboxLib, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeXboxGeneric, CProjType)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// declare our platform
//
// *** DO NOT TRANSLATE (LOCALIZE) THE PLATFORM STRINGS HERE ***

#define TLLOC_NAME  "TLXBOX"
#define EM_NAME     "XEM"
#define SH_NAME     "SHXBOX"
#define EE_NAME     "EECXX"

TLInfo g_rgTLInfoXbox[] =
{
    //{_T(TLLOC_NAME),IDS_TL_XBOX_LOCAL,  TFL_LOCAL},
    {_T(TLLOC_NAME),   IDS_TL_XBOX_TCPIP,  TFL_CFGABLE}
};

PlatformInfo g_PlatformInfoXbox =
{
    _T("Xbox"),          // Official platform name -- DO NOT LOCALIZE!
    xbox,                   // Unique ID
    IDS_XBOX_PLATFORM,         // Localizable description string
    IDS_XBOX_PLATFORM_SHORT,// Abbreviated localizable description string
    _T(EM_NAME),                // EM name
    _T(SH_NAME),                    // SH name
    _T(EE_NAME),                // EE name
    sizeof( g_rgTLInfoXbox )/sizeof( *g_rgTLInfoXbox ), // Number of TLs
    g_rgTLInfoXbox,         // Array of TLInfo structs
    0,                          // Number of NMs
    NULL,                       // Array of NMInfo structs
    _T("res"),                  // RC extension
    "xbox\\bin;",               // Platform specific Path directories
    "xbox\\include;",           // Platform specific Include Path directories
    "xbox\\lib;",               // Platform specific Lib Path directories
    "xbox\\src;",               // Platform specific Source Path directories
    0 |                         // Not Primary platform
    PIA_Allow_WorkingDir |      // Can use a working dir. for debugging
    PIA_Supports_IncCplr |      // Supports inc. cplr.
    PIA_Supports_RTTI |         // Supports RTTI
    PIA_Supports_IncLink|       // Supports ilink
    PIA_Supports_MinBuild |     // Supports minimum build
    PIA_Enable_Language |       // Enable language for RC option
    PIA_Allow_MFCinDLL |        // Supports use of mfc in a dll
    PIA_Allow_ProgArgs |        // Executable can use program arguments for Debug etc.
    PIA_Supports_RemoteDbg|     // Supports remote debugging
    PIA_Enable_Stub|            // Allow DOS Stub
    PIA_Enable_AllDebugType,    // Allow Debug Types
    NoUseMFC,                   // Default is not using MFC
    _T("")                  // Default output directories
};

// implementation

// pre-installed components
#include "..\..\ide\pkgs\bld\src\toolbsc.h"
#include "..\..\ide\pkgs\bld\src\toolrc.h"
#include "..\..\ide\pkgs\bld\src\toolmtl.h"
#include "..\..\ide\pkgs\bld\src\toollib.h"

// derive the linker options by adding in 'common part' + 'config. part' + 'win32 libraries'
void VPROJDeriveDefaultXboxLinkOptions(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_XBOX_LINK_COMMON, fDebug ? IDS_XBOX_LINK_DBG : IDS_XBOX_LINK_REL, nIDOption);

	/* We don't need debug options again; we just got them */
#if 0
	// REVIEW: This should be just added to the IDS_XBOX_LINK_DBG string once all the 
	// platforms support separate types.
	if (fDebug)
	{
		CString strXtraDbg; 
		VERIFY(strXtraDbg.LoadString(IDS_XBOX_LINK_DBG));
		strOption += ' '; strOption += strXtraDbg;
	}
#endif

    CString strXtraLibs;
    VERIFY(strXtraLibs.LoadString(IDS_XBOX_LIBS_CORE));
    strOption += ' '; strOption += strXtraLibs;

#if 0
    LPBUILDSYSTEM pBldSysIFace;
    VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace)));
    if (pBldSysIFace->LanguageSupported(CPlusPlus) == S_OK)
    {
        VERIFY(strXtraLibs.LoadString(IDS_XBOX_LIBS));
        strOption += ' '; strOption += strXtraLibs;
    }
    pBldSysIFace->Release();
#endif
}

void VPROJDeriveDefaultXboxRcOptions(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_XBOX_RC_COMMON, fDebug ? IDS_XBOX_RC_DBG : IDS_XBOX_RC_REL, nIDOption);
}

void VPROJDeriveDefaultXboxBscMakeOptions(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_XBOX_BSCMAKE_COMMON, fDebug ? IDS_XBOX_BSCMAKE_DBG : IDS_XBOX_BSCMAKE_REL, nIDOption);
}

void VPROJDeriveDefaultXbeBuildOptions(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_XBEBUILD_COMMON, fDebug ? IDS_XBEBUILD_DBG : IDS_XBEBUILD_REL, nIDOption);
}

void VPROJDeriveDefaultXbcpOptions(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_XBCP_COMMON, -1, nIDOption);
}

void VPROJDeriveDefaultXboxLibOptions(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    // the library manager options have no common, debug or release parts!
    strOption.LoadString(nIDOption);
}

BOOL CProjTypeXboxExe::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    CString strDebugMode, strReleaseMode;
    BOOL    fDebug;

    // init. our debug, release configs.
    VERIFY(strDebugMode.LoadString(IDS_DEBUG_CONFIG));
    VERIFY(strReleaseMode.LoadString(IDS_RELEASE_CONFIG));

    fDebug = (strDebugMode == strMode);
    if (!fDebug && (strReleaseMode != strMode))
        return FALSE;   // no a known configuration

    // what type of tool is this?
    if (PackageIdFromBscId(bsc_id) == PackageIdFromBscId(CompId())) // our tool?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the COFF linker
            case BCID_Tool_Linker_XboxExe:
                VPROJDeriveDefaultXboxLinkOptions(strOption, fDebug, IDS_XBOXEXE_LINK);
                break;

			case BCID_Tool_XbeBuilder:
                VPROJDeriveDefaultXbeBuildOptions(strOption, fDebug, UINT(-1));
				break;

#ifdef XBCP
			case BCID_Tool_Xbcp:
                VPROJDeriveDefaultXbcpOptions(strOption, fDebug, UINT(-1));
				break;
#endif

            default:
                ASSERT(FALSE);  // need default options for one of our own tools!
                break;
        }
    }
    else if ( bsc_id && (PackageIdFromBscId(bsc_id) == idAddOnGeneric) ) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultXboxRcOptions(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultXboxBscMakeOptions(strOption, fDebug, UINT(-1));
                break;

            // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_XBOX_MTL_COMMON, fDebug ? IDS_XBOX_MTL_DBG : IDS_XBOX_MTL_REL, UINT(-1)) ;
                break;

            default:
                ASSERT(FALSE);  // need default options for one of our own tools!
                break;
        }
    }
    else
        // not a known tool
        return CProjType::GetDefaultToolOptions(bsc_id, strMode, strOption);

    return TRUE;
}

#ifdef XBOXDLL
BOOL CProjTypeXboxDll::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    CString strDebugMode, strReleaseMode;
    BOOL    fDebug;

    // init. our debug, release configs.
    VERIFY(strDebugMode.LoadString(IDS_DEBUG_CONFIG));
    VERIFY(strReleaseMode.LoadString(IDS_RELEASE_CONFIG));

    fDebug = (strDebugMode == strMode);
    if (!fDebug && (strReleaseMode != strMode))
        return FALSE;   // no a known configuration

    // what type of tool is this?
    if (PackageIdFromBscId(bsc_id) == PackageIdFromBscId(CompId())) // our tool?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the COFF linker
            case BCID_Tool_Linker_XboxDll:
                VPROJDeriveDefaultXboxLinkOptions(strOption, fDebug, IDS_XBOXDLL_LINK);
                break;

            default:
                ASSERT(FALSE);  // need default options for one of our own tools!
                break;
        }
    }
    else if ( bsc_id && (PackageIdFromBscId(bsc_id) == idAddOnGeneric) ) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultXboxRcOptions(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultXboxBscMakeOptions(strOption, fDebug, UINT(-1));
                break;

            // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_XBOX_MTL_COMMON, fDebug ? IDS_XBOX_MTL_DBG : IDS_XBOX_MTL_REL, UINT(-1)) ;
                break;

            default:
                ASSERT(FALSE);  // need default options for one of our own tools!
                break;
        }
    }
    else
        // not a known tool
        return CProjType::GetDefaultToolOptions(bsc_id, strMode, strOption);

    return TRUE;
}
#endif

BOOL CProjTypeXboxLib::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    CString strDebugMode, strReleaseMode;
    BOOL    fDebug;

    // init. our debug, release configs.
    VERIFY(strDebugMode.LoadString(IDS_DEBUG_CONFIG));
    VERIFY(strReleaseMode.LoadString(IDS_RELEASE_CONFIG));

    fDebug = (strDebugMode == strMode);
    if (!fDebug && (strReleaseMode != strMode))
        return FALSE;   // no a known configuration

    // what type of tool is this?
    if (PackageIdFromBscId(bsc_id) == PackageIdFromBscId(CompId())) // our tool?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            default:
                ASSERT(FALSE);  // need default options for one of our own tools!
                break;
        }
    }
    else if ( bsc_id && (PackageIdFromBscId(bsc_id) == idAddOnGeneric) ) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the COFF Library manager
            case BCID_Tool_Lib:
                VPROJDeriveDefaultXboxLibOptions(strOption, fDebug, IDS_XBOXLIB_LINK);
                break;

            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultXboxRcOptions(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultXboxBscMakeOptions(strOption, fDebug, UINT(-1));
                break;

            default:
                ASSERT(FALSE);  // need default options for one of our own tools!
                break;
        }
    }
    else
        // not a known tool
        return CProjType::GetDefaultToolOptions(bsc_id, strMode, strOption);

    return TRUE;
}

BOOL CProjTypeXboxGeneric::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    strOption = "";
    return TRUE;
}

BOOL CProjTypeXboxExe::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnXbox, BCID_Tool_Linker_XboxExe);
	AddTool(szAddOnXbox, BCID_Tool_XbeBuilder);
#ifdef XBCP
	AddTool(szAddOnXbox, BCID_Tool_Xbcp);
#endif
#if 0
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);
#endif

    return TRUE;    // success
}

BOOL CProjTypeXboxLib::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnGeneric, BCID_Tool_Lib);
#if 0
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
#endif

    return TRUE;    // success
}

#ifdef XBOXDLL
BOOL CProjTypeXboxDll::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnXbox, BCID_Tool_Linker_XboxDll);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}
#endif

BOOL CProjTypeXboxGeneric::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}
