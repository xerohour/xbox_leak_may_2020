//
// Alpha (RISC) Platform
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"     // our standard AFX include
#pragma hdrstop
#include "platalf.h"    // our local header file

IMPLEMENT_DYNAMIC(CProjTypeWin32ALPHAExe, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeWin32ALPHADll, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeWin32ALPHACon, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeWin32ALPHALib, CProjType)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// declare our platform
//
// *** DO NOT TRANSLATE (LOCALIZE) THE PLATFORM STRINGS HERE ***

PlatformInfo g_PlatformInfoWin32ALPHA =
{
    _T("Win32 (ALPHA)"),            // Official platform name -- DO NOT LOCALIZE!
    win32alpha,                     // Unique ID
#if defined (_ALPHA_)
    IDS_WIN32_PLATFORM,             // Localizable description string
#else
    IDS_WIN32ALPHA_PLATFORM,        // Localizable description string
#endif
    IDS_WIN32ALPHA_PLATFORM_SHORT,  // Abbreviated localizable description string
    _T("res"),                      // RC extension
#if defined (_ALPHA_)
    PIA_Primary_Platform |          // Primary platform
#endif
    PIA_Allow_WorkingDir |          // Can use a working dir. for debugging
    PIA_Supports_RTTI |         // Supports RTTI
    PIA_Enable_Language |               // Enable Language for RC option
    PIA_Supports_IncLink|      		 // Supports ilink
    PIA_Allow_ProgArgs |            // Executable can use program arguments for Debug etc.
    PIA_Allow_MFCinDLL|             // Allow use of mfc in a dll
    PIA_Enable_Stub|                // Allow DOS Stub
    PIA_Supports_RemoteDbg |		// Supports remote debugging
    PIA_Enable_AllDebugType,        // Allow Debug Types
    // Enable this when the platform fully supports edit & continue
    //PIA_Supports_ENC|           // Supports Edit & Continue
    NoUseMFC,                       // Default is not using MFC
    _T("Alpha")                     // Default output directories
};

// implementation

// pre-installed components
#include "optnbsc.h"
#include "optnrc.h"
#include "optnmtl.h"
#include "optnlib.h"

// 'our' components

void VPROJDeriveDefaultCplrOptionsAXP(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_WIN32_CPLR_COMMON, fDebug ? IDS_WIN32_CPLR_DBG : IDS_WIN32_CPLR_REL, nIDOption);
}

// derive the linker options by adding in 'common part' + 'config. part' + 'win32 libraries'
void VPROJDeriveDefaultWin32ALPHALinkOptions(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_WIN32_LINK_ALPHA_COMMON, fDebug ? IDS_WIN32_LINK_DBG : IDS_WIN32_LINK_REL, nIDOption);

    CString strXtraLibs;
    BOOL bOK = strXtraLibs.LoadString(IDS_WIN32_LIBS_CORE);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
    strOption += ' '; strOption += strXtraLibs;
    bOK = strXtraLibs.LoadString(IDS_WIN32_LIBS);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
    strOption += ' '; strOption += strXtraLibs;
    bOK = strXtraLibs.LoadString(IDS_OLE2_LIBS);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
    strOption += ' '; strOption += strXtraLibs;
}

void VPROJDeriveDefaultRcOptionsAXP(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_WIN32_RC_COMMON, fDebug ? IDS_WIN32_RC_DBG : IDS_WIN32_RC_REL, nIDOption);
}

void VPROJDeriveDefaultBscMakeOptionsAXP(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_WIN32_BSCMAKE_COMMON, fDebug ? IDS_WIN32_BSCMAKE_DBG : IDS_WIN32_BSCMAKE_REL, nIDOption);
}

void VPROJDeriveDefaultLibOptionsAXP(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    // the library manager options have no common, debug or release parts!
    BOOL bOK = strOption.LoadString(nIDOption);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
}

BOOL CProjTypeWin32ALPHAExe::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    CString strDebugMode, strReleaseMode;
    BOOL    fDebug;

    // init. our debug, release configs.
    BOOL bOK = strDebugMode.LoadString(IDS_DEBUG_CONFIG);
 	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	bOK = strReleaseMode.LoadString(IDS_RELEASE_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");

    fDebug = (strDebugMode == strMode);
    if (!fDebug && (strReleaseMode != strMode))
        return FALSE;   // no a known configuration

    // what type of tool is this?
    if (PackageIdFromBscId(bsc_id) == PackageIdFromBscId(CompId())) // our tool?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the Alpha compiler
            case BCID_Tool_Compiler_Alpha:
                VPROJDeriveDefaultCplrOptionsAXP(strOption, fDebug, fDebug ? IDS_WIN32ALPHAEXE_CPLR_DBG : IDS_WIN32ALPHAEXE_CPLR_REL);
                break;

            // the Alpha COFF linker
            case BCID_Tool_Linker_Alpha:
                VPROJDeriveDefaultWin32ALPHALinkOptions(strOption, fDebug, IDS_WIN32EXE_LINK);
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else if (PackageIdFromBscId(bsc_id) == idAddOnGeneric) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultRcOptionsAXP(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsAXP(strOption, fDebug, UINT(-1));
                break;

            // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_WIN32ALPHA_MTL_COMMON, fDebug ? IDS_WIN32_MTL_DBG : IDS_WIN32_MTL_REL, UINT(-1)) ;
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else
        // not a known tool
        return CProjType::GetDefaultToolOptions(bsc_id, strMode, strOption);

    return TRUE;
}

BOOL CProjTypeWin32ALPHADll::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    CString strDebugMode, strReleaseMode;
    BOOL    fDebug;

    // init. our debug, release configs.
    BOOL bOK = strDebugMode.LoadString(IDS_DEBUG_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
    bOK = strReleaseMode.LoadString(IDS_RELEASE_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");

    fDebug = (strDebugMode == strMode);
    if (!fDebug && (strReleaseMode != strMode))
        return FALSE;   // no a known configuration

    // what type of tool is this?
    if (PackageIdFromBscId(bsc_id) == PackageIdFromBscId(CompId())) // our tool?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the Alpha compiler
            case BCID_Tool_Compiler_Alpha:
                VPROJDeriveDefaultCplrOptionsAXP(strOption, fDebug, fDebug ? IDS_WIN32ALPHADLL_CPLR_DBG : IDS_WIN32ALPHADLL_CPLR_REL);
                break;

            // the Alpha COFF linker
            case BCID_Tool_Linker_Alpha:
                VPROJDeriveDefaultWin32ALPHALinkOptions(strOption, fDebug, IDS_WIN32DLL_LINK);
                break;

             // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_WIN32ALPHA_MTL_COMMON, fDebug ? IDS_WIN32_MTL_DBG : IDS_WIN32_MTL_REL, UINT(-1)) ;
                break;

           default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else if (PackageIdFromBscId(bsc_id) == idAddOnGeneric) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultRcOptionsAXP(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsAXP(strOption, fDebug, UINT(-1));
                break;

            // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_WIN32ALPHA_MTL_COMMON, fDebug ? IDS_WIN32_MTL_DBG : IDS_WIN32_MTL_REL, UINT(-1)) ;
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else
        // not a known tool
        return CProjType::GetDefaultToolOptions(bsc_id, strMode, strOption);

    return TRUE;
}

BOOL CProjTypeWin32ALPHACon::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    CString strDebugMode, strReleaseMode;
    BOOL    fDebug;

    // init. our debug, release configs.
    BOOL bOK = strDebugMode.LoadString(IDS_DEBUG_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
    bOK = strReleaseMode.LoadString(IDS_RELEASE_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");

    fDebug = (strDebugMode == strMode);
    if (!fDebug && (strReleaseMode != strMode))
        return FALSE;   // no a known configuration

    // what type of tool is this?
    if (PackageIdFromBscId(bsc_id) == PackageIdFromBscId(CompId())) // our tool?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the Alpha compiler
            case BCID_Tool_Compiler_Alpha:
                VPROJDeriveDefaultCplrOptionsAXP(strOption, fDebug, fDebug ? IDS_WIN32ALPHACON_CPLR_DBG : IDS_WIN32ALPHACON_CPLR_REL);
                break;

            // the Alpha COFF linker
            case BCID_Tool_Linker_Alpha:
                VPROJDeriveDefaultWin32ALPHALinkOptions(strOption, fDebug, IDS_WIN32CON_LINK);
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else if (PackageIdFromBscId(bsc_id) == idAddOnGeneric) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultRcOptionsAXP(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsAXP(strOption, fDebug, UINT(-1));
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else
        // not a known tool
        return CProjType::GetDefaultToolOptions(bsc_id, strMode, strOption);

    return TRUE;
}

BOOL CProjTypeWin32ALPHALib::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    CString strDebugMode, strReleaseMode;
    BOOL    fDebug;

    // init. our debug, release configs.
    BOOL bOK = strDebugMode.LoadString(IDS_DEBUG_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
    bOK = strReleaseMode.LoadString(IDS_RELEASE_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");

    fDebug = (strDebugMode == strMode);
    if (!fDebug && (strReleaseMode != strMode))
        return FALSE;   // no a known configuration

    // what type of tool is this?
    if (PackageIdFromBscId(bsc_id) == PackageIdFromBscId(CompId())) // our tool?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the Alpha compiler
            case BCID_Tool_Compiler_Alpha:
                VPROJDeriveDefaultCplrOptionsAXP(strOption, fDebug, fDebug ? IDS_WIN32ALPHALIB_CPLR_DBG : IDS_WIN32ALPHALIB_CPLR_REL);
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else if (PackageIdFromBscId(bsc_id) == idAddOnGeneric) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the COFF library manager
            case BCID_Tool_Lib:
                VPROJDeriveDefaultLibOptionsAXP(strOption, fDebug, IDS_WIN32LIB_LINK);
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsAXP(strOption, fDebug, UINT(-1));
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else
        // not a known tool
        return CProjType::GetDefaultToolOptions(bsc_id, strMode, strOption);

    return TRUE;
}

BOOL CProjTypeWin32ALPHAExe::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnx86, BCID_Tool_Linker_Alpha);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}

BOOL CProjTypeWin32ALPHADll::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnx86, BCID_Tool_Linker_Alpha);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}

BOOL CProjTypeWin32ALPHACon::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnx86, BCID_Tool_Linker_Alpha);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}

BOOL CProjTypeWin32ALPHALib::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnGeneric, BCID_Tool_Lib);

    return TRUE;    // success
}

