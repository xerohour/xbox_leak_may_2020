#include "precomp.hpp"

EXT_API_VERSION ApiVersion =
{
    (VER_PRODUCTVERSION_W >> 8),
    (VER_PRODUCTVERSION_W & 0xff),
    EXT_API_VERSION_NUMBER,
    0
};

WINDBG_EXTENSION_APIS ExtensionApis;

VOID WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT MajorVersion,
    USHORT MinorVersion)
{
    ExtensionApis = *lpExtensionApis;
}

VOID CheckVersion(
    VOID)
{
}

LPEXT_API_VERSION ExtensionApiVersion(
    VOID)
{
    return &ApiVersion;
}

BOOLEAN WINAPI DllMain(
    HINSTANCE hInstDll,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    return TRUE;
}

DECLARE_API(help)
{
    HelpHelp();
    HelpStats();
    HelpDumper();
}

