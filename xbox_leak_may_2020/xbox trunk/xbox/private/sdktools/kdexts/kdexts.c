/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    kdexts.c

Abstract:

    This file contains the generic routines and initialization code
    for the kernel debugger extensions dll.

Author:

    Wesley Witt (wesw) 26-Aug-1993

Environment:

    User Mode

--*/

#include "precomp.h"
#pragma hdrstop

#include <ntverp.h>
#include <dbghelp.h>

//
// globals
//
EXT_API_VERSION         ApiVersion = { (VER_PRODUCTVERSION_W >> 8), (VER_PRODUCTVERSION_W & 0xff), EXT_API_VERSION_NUMBER, 0 };
WINDBG_EXTENSION_APIS   ExtensionApis;
ULONG                   STeip;
ULONG                   STebp;
ULONG                   STesp;
USHORT                  SavedMajorVersion;
USHORT                  SavedMinorVersion;

DBGKD_GET_VERSION32     KernelVersionPacket;


//
// this string is for supporting both the old and the new way of getting
// data from the kernel.  Maybe it will go away soon.
//
char ___SillyString[200];



DllInit(
    HANDLE hModule,
    DWORD  dwReason,
    DWORD  dwReserved
    )
{
    switch (dwReason) {
        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_PROCESS_ATTACH:
            break;
    }

    return TRUE;
}


VOID
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT MajorVersion,
    USHORT MinorVersion
    )
{
    ExtensionApis = *lpExtensionApis;

    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;

    return;
}

DECLARE_API( version )
{
#if DBG
    PCHAR DebuggerType = "Checked";
#else
    PCHAR DebuggerType = "Free";
#endif

    dprintf( "%s Extension dll for Build %d debugging %s kernel for Build %d\n",
             DebuggerType,
             VER_PRODUCTBUILD,
             SavedMajorVersion == 0x0c ? "Checked" : "Free",
             SavedMinorVersion
           );
}

VOID
CheckVersion(
    VOID
    )
{
#if 0
    //
    // This check is broken. Even if it wasn't broken, it is way too annoying.
    //
#if DBG
    if ((SavedMajorVersion != 0x0c) || (SavedMinorVersion != VER_PRODUCTBUILD)) {
        dprintf("\r\n*** Extension DLL(%d Checked) does not match target system(%d %s)\r\n\r\n",
                VER_PRODUCTBUILD, SavedMinorVersion, (SavedMajorVersion==0x0f) ? "Free" : "Checked" );
    }
#else
    if ((SavedMajorVersion != 0x0f) || (SavedMinorVersion != VER_PRODUCTBUILD)) {
        dprintf("\r\n*** Extension DLL(%d Free) does not match target system(%d %s)\r\n\r\n",
                VER_PRODUCTBUILD, SavedMinorVersion, (SavedMajorVersion==0x0f) ? "Free" : "Checked" );
    }
#endif
#endif
}

BOOLEAN
IsCheckedBuild(
    PBOOLEAN Checked
    )
{
   USHORT majorVersion ;

   if (!HaveDebuggerData()) {

      //
      // Can't tell.
      //
      return FALSE ;
   }

   //
   // 0xC for checked, 0xF for free.
   //
   *Checked = (KernelVersionPacket.MajorVersion == 0xc) ;
   return TRUE ;
}

LPEXT_API_VERSION
ExtensionApiVersion(
    VOID
    )
{
    return &ApiVersion;
}

BOOL
HaveDebuggerData(
    VOID
    )
{
    static int havedata = 0;

    if (havedata == 0) {
        if (!Ioctl( IG_GET_KERNEL_VERSION, &KernelVersionPacket, sizeof(KernelVersionPacket))) {
            havedata = 2;
        } else if (KernelVersionPacket.MajorVersion == 0) {
            havedata = 2;
        } else {
            havedata = 1;
        }
    }

    return (havedata == 1) &&
           ((KernelVersionPacket.Flags & DBGKD_VERS_FLAG_DATA) != 0);
}
