#include "basedll.h"
#include "xboxverp.h"
#pragma hdrstop

#ifdef _DEBUG

DWORD
WINAPI
XDebugGetSystemVersion(
    PSTR pszVersionString,
    UINT cchVersionString
    )
{
    RIP_ON_NOT_TRUE("XDebugGetSystemVersion()", (pszVersionString != NULL));

    _snprintf(pszVersionString,
              cchVersionString,
              "%d.%02d.%d.%02d",
              XboxKrnlVersion->Major,
              XboxKrnlVersion->Minor,
              XboxKrnlVersion->Build,
              (XboxKrnlVersion->Qfe & 0x7FFF));

    return (XboxKrnlVersion->Qfe & 0x8000) ? XVER_DEVKIT : XVER_RETAIL;
}

DWORD
WINAPI
XDebugGetXTLVersion(
    PSTR pszVersionString,
    UINT cchVersionString
    )
{
    RIP_ON_NOT_TRUE("XDebugGetXTLVersion()", (pszVersionString != NULL));

    _snprintf(pszVersionString,
              cchVersionString,
              "%d.%02d.%d.%02d",
              XeImageHeader()->XapiLibraryVersion->MajorVersion,
              XeImageHeader()->XapiLibraryVersion->MinorVersion,
              XeImageHeader()->XapiLibraryVersion->BuildVersion,
              XeImageHeader()->XapiLibraryVersion->QFEVersion);

    return XeImageHeader()->XapiLibraryVersion->DebugBuild ? XVER_DEVKIT : XVER_RETAIL;
}

#endif // _DEBUG
