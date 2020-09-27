//
// Intelx86 Platform
//
// [v-danwh],[matthewt]
//

#include "stdafx.h"     // our standard AFX include
#pragma hdrstop
#include "platx86.h"    // our local header file

IMPLEMENT_DYNAMIC(CProjTypeWin32Exe, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeWin32Dll, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeWin32Lib, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeWin32Con, CProjType)
IMPLEMENT_DYNAMIC(CProjTypeGeneric, CProjType)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// declare our platform
//
// *** DO NOT TRANSLATE (LOCALIZE) THE PLATFORM STRINGS HERE ***

PlatformInfo g_PlatformInfoWin32x86 =
{
    _T("Win32 (x86)"),          // Official platform name -- DO NOT LOCALIZE!
    win32x86,                   // Unique ID
#if !defined (_ALPHA_) && !defined (_PPC_)
    IDS_WIN32_PLATFORM,         // Localizable description string
#else
    IDS_WIN32X86_PLATFORM,      // Localizable description string
#endif
    IDS_WIN32X86_PLATFORM_SHORT,// Abbreviated localizable description string
    _T("res"),                  // RC extension
#if !defined (_ALPHA_) && !defined (_PPC_)
    PIA_Primary_Platform |      // Primary platform
#else
    0 |                         // Not Primary platform
#endif
    PIA_Allow_WorkingDir |      // Can use a working dir. for debugging
    PIA_Supports_IncCplr |      // Supports inc. cplr.
    PIA_Supports_RTTI |         // Supports RTTI
    PIA_Supports_IncLink|       // Supports ilink
    PIA_Supports_MinBuild |     // Supports minimum build
    PIA_Enable_Language |       // Enable language for RC option
    PIA_Allow_MFCinDLL |        // Supports use of mfc in a dll
    PIA_Allow_ODBC |            // Allow use of mfc ODBC
    PIA_Allow_ProgArgs |        // Executable can use program arguments for Debug etc.
    PIA_Supports_RemoteDbg|     // Supports remote debugging
    PIA_Enable_Stub|            // Allow DOS Stub
    PIA_Supports_ENC|           // Supports Edit & Continue
    PIA_Enable_AllDebugType,    // Allow Debug Types
    NoUseMFC,                   // Default is not using MFC
    _T("")                  // Default output directories
};

// implementation

// pre-installed components
#include "optnbsc.h"
#include "optnrc.h"
#include "optnmtl.h"
#include "optnlib.h"

// derive the linker options by adding in 'common part' + 'config. part' + 'win32 libraries'
void VPROJDeriveDefaultWin32LinkOptionsX86(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_WIN32_LINK_X86_COMMON, fDebug ? IDS_WIN32_LINK_DBG : IDS_WIN32_LINK_REL, nIDOption);

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

void VPROJDeriveDefaultRcOptionsX86(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_WIN32_RC_COMMON, fDebug ? IDS_WIN32_RC_DBG : IDS_WIN32_RC_REL, nIDOption);
}

void VPROJDeriveDefaultBscMakeOptionsX86(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    VPROJDeriveDefaultOptions(strOption, IDS_WIN32_BSCMAKE_COMMON, fDebug ? IDS_WIN32_BSCMAKE_DBG : IDS_WIN32_BSCMAKE_REL, nIDOption);
}

void VPROJDeriveDefaultLibOptionsX86(CString & strOption, BOOL fDebug, UINT nIDOption)
{
    // the library manager options have no common, debug or release parts!
    BOOL bOK = strOption.LoadString(nIDOption);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
}

BOOL CProjTypeWin32Exe::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
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
            // the COFF linker
            case BCID_Tool_Linker_x86:
                VPROJDeriveDefaultWin32LinkOptionsX86(strOption, fDebug, IDS_WIN32EXE_LINK);
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else if ( bsc_id && (PackageIdFromBscId(bsc_id) == idAddOnGeneric) ) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultRcOptionsX86(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsX86(strOption, fDebug, UINT(-1));
                break;

            // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_WIN32_MTL_COMMON, fDebug ? IDS_WIN32_MTL_DBG : IDS_WIN32_MTL_REL, UINT(-1)) ;
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

BOOL CProjTypeGeneric::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
{
    strOption = "";
    return TRUE;
}

BOOL CProjTypeWin32Dll::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
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
            // the COFF linker
            case BCID_Tool_Linker_x86:
                VPROJDeriveDefaultWin32LinkOptionsX86(strOption, fDebug, IDS_WIN32DLL_LINK);
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else if ( bsc_id && (PackageIdFromBscId(bsc_id) == idAddOnGeneric) ) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultRcOptionsX86(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsX86(strOption, fDebug, UINT(-1));
                break;

            // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_WIN32_MTL_COMMON, fDebug ? IDS_WIN32_MTL_DBG : IDS_WIN32_MTL_REL, UINT(-1)) ;
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

BOOL CProjTypeWin32Con::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
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
            // the COFF linker
            case BCID_Tool_Linker_x86:
                VPROJDeriveDefaultWin32LinkOptionsX86(strOption, fDebug, IDS_WIN32CON_LINK);
                break;

            default:
                VSASSERT(FALSE, "need default options for one of our own tools!");
                break;
        }
    }
    else if ( bsc_id && (PackageIdFromBscId(bsc_id) == idAddOnGeneric ) ) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultRcOptionsX86(strOption, fDebug, UINT(-1));
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsX86(strOption, fDebug, UINT(-1));
                break;

            // MakeTypLib compiler
            case BCID_Tool_MkTypLib:
                VPROJDeriveDefaultOptions(strOption, IDS_WIN32_MTL_COMMON, fDebug ? IDS_WIN32_MTL_DBG : IDS_WIN32_MTL_REL, UINT(-1)) ;
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

BOOL CProjTypeWin32Lib::GetDefaultToolOptions(DWORD bsc_id, const CString & strMode, CString & strOption)
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
        /*switch*/ (PkgCmpIdFromBscId(bsc_id));
        {
            /*default:*/
                VSASSERT(FALSE, "need default options for one of our own tools!");
                /*break;*/
        }
    }
    else if ( bsc_id && (PackageIdFromBscId(bsc_id) == idAddOnGeneric) ) // generic?
    {
        switch (PkgCmpIdFromBscId(bsc_id))
        {
            // the NT resource compiler
            case BCID_Tool_RcCompilerNT:
                VPROJDeriveDefaultRcOptionsX86(strOption, fDebug, UINT(-1));
                break;

            // the COFF library manager
            case BCID_Tool_Lib:
                VPROJDeriveDefaultLibOptionsX86(strOption, fDebug, IDS_WIN32LIB_LINK);
                break;

            // the browser database maker
            case BCID_Tool_BscMake:
                VPROJDeriveDefaultBscMakeOptionsX86(strOption, fDebug, UINT(-1));
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

BOOL CProjTypeWin32Exe::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnx86, BCID_Tool_Linker_x86);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}

BOOL CProjTypeGeneric::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}

BOOL CProjTypeWin32Dll::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnx86, BCID_Tool_Linker_x86);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}

BOOL CProjTypeWin32Con::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnx86, BCID_Tool_Linker_x86);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);
    AddTool(szAddOnGeneric, BCID_Tool_MkTypLib);

    return TRUE;    // success
}

BOOL CProjTypeWin32Lib::FInit()
{
    if (!CProjType::FInit())
        return FALSE;   // failed

    // Tools that we can use.
    AddTool(szAddOnGeneric, BCID_Tool_BscMake);
    AddTool(szAddOnGeneric, BCID_Tool_Lib);
    AddTool(szAddOnGeneric, BCID_Tool_RcCompilerNT);

    return TRUE;    // success
}

