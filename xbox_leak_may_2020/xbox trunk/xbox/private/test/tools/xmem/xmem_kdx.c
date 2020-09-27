/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xmem_kdx.c

Abstract:

  This module is the kernel debugger extension for xmem

Author:

  Steven Kehrli (steveke) 1-Nov-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace xMemNamespace;

WINDBG_EXTENSION_APIS  ExtensionApis;      // ExtensionApis are the callbacks to the functions for standard operations.  See wdbgexts.h for more info.
USHORT                 SavedMajorVersion;  // SavedMajorVersion indicates whether the system is a checked build (0x0C) or a free build (0x0F)
USHORT                 SavedMinorVersion;  // SavedMinorVersion indicates the system build number

EXT_API_VERSION        ApiVersion = { (VER_PRODUCTVERSION_W >> 8), (VER_PRODUCTVERSION_W & 0xff), EXT_API_VERSION_NUMBER, 0 };  // ApiVersion is the version of the debugger extension



BOOL
WINAPI
DllMain(
    IN HINSTANCE hInstance,
    IN DWORD     dwReason,
    IN LPVOID    lpContext
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  DLL entry point

Arguments:

  hInstance - handle to the module
  dwReason - indicates the reason for calling the function
  lpContext - reserved

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    return TRUE;
}



VOID
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT                 MajorVersion,
    USHORT                 MinorVersion
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Initializes the debugger extension

Arguments:

  lpExtensionApis - Pointer to the callbacks to the functions for standard operations
  MajorVersion - Indicates whether the system is a checked build (0x0C) or a free build (0x0F)
  MinorVersion - Indicates the system build number

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Save the pointer to the callbacks
    ExtensionApis = *lpExtensionApis;
    // Save the major version
    SavedMajorVersion = MajorVersion;
    // Save the minor version
    SavedMinorVersion = MinorVersion;
}



LPEXT_API_VERSION
ExtensionApiVersion(
    VOID
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Returns the version number of the debugger extension

Arguments:

  None

Return Value:

  LPEXT_API_VERSION - Pointer to the version number of the debugger extension

------------------------------------------------------------------------------*/
{
    return &ApiVersion;
}



VOID
CheckVersion(
    VOID
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Checks the version number of the debugger extension against the version number of the system

Arguments:

  None

Return Value:

  None

------------------------------------------------------------------------------*/
{
    dprintf("\n");
    dprintf("*** xmem_kdx CheckVersion\n");
#if DBG
    if ((0x0c != SavedMajorVersion) || (VER_PRODUCTBUILD != SavedMinorVersion)) {
        dprintf("  Extension DLL (%d Checked) does not match target system (%d %s)\n\n", VER_PRODUCTBUILD, SavedMinorVersion, (0x0f == SavedMajorVersion) ? "Free" : "Checked");
    }
#else
    if ((0x0f != SavedMajorVersion) || (VER_PRODUCTBUILD != SavedMinorVersion)) {
        dprintf("  Extension DLL (%d Free) does not match target system (%d %s)\n\n", VER_PRODUCTBUILD, SavedMinorVersion, (0x0f == SavedMajorVersion) ? "Free" : "Checked");
    }
#endif
}



DECLARE_API( help )
{
    dprintf("\n");
    dprintf("*** xmem_kdx help:\n");
    dprintf("  help                       - Display this help information\n");
    dprintf("  version                    - Display the version number for xmem_kdx\n");
    dprintf("  dumpobjects <address>      - Display the memory objects for xMem\n");
    dprintf("  dumpallocations <address>  - Display the memory allocations for xMem\n");
    dprintf("  dumpall <address>          - Display the memory objects and allocations for xMem\n");
    dprintf("\n");
}



DECLARE_API( version )
{
    dprintf("\n");
    dprintf("*** xmem_kdx version\n");
#if DBG
    dprintf("  %d Checked\n\n", VER_PRODUCTBUILD);
#else
    dprintf("  %d Free\n\n", VER_PRODUCTBUILD);
#endif
}



DECLARE_API( dumpobjects )
{
    // Address is the address of the pointer to the head of the memory allocation list
    ULONGLONG         Address = 0;
    // MemObjectAddress is a pointer to the memory object address
    PXMEM_OBJECT      MemObjectAddress = NULL;
    // MemObject is the memory object
    XMEM_OBJECT       MemObject;

    // bReturnValue is the return value of ReadMemory()
    BOOL              bReturnValue = FALSE;
    // cbBytesRead is the number of bytes read for ReadMemory()
    ULONG             cbBytesRead = 0;



    // Get the address of the pointer to the head of the memory allocation list
    Address = (ULONGLONG) GetExpression(args);

    if (NULL == Address) {
        dprintf("dumpobjects <address>\n");
        return;
    }

    // Get the pointer to the head of the memory object list
    bReturnValue = ReadMemory((ULONG) Address, &MemObjectAddress, sizeof(PXMEM_OBJECT), &cbBytesRead);

    if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(PXMEM_OBJECT))) {
        dprintf("Cannot read 0x%08x bytes at 0x%08x for PXMEM_OBJECT\n", sizeof(PXMEM_OBJECT), Address);
        dprintf("\n");
        return;
    }

    dprintf("*** xmem_kdx dumpobjects\n");
    dprintf("\n");

    if (NULL == MemObjectAddress) {
        dprintf("  No memory objects\n");
        dprintf("\n");
    }
    else {
        do {
            // Get the XMEM_OBJECT structure
            ZeroMemory(&MemObject, sizeof(XMEM_OBJECT));
            bReturnValue = ReadMemory((ULONG) MemObjectAddress, &MemObject, sizeof(XMEM_OBJECT), &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XMEM_OBJECT))) {
                dprintf("Cannot read 0x%08x bytes at 0x%08x for XMEM_OBJECT\n", sizeof(XMEM_OBJECT), MemObjectAddress);
                dprintf("\n");
                return;
            }

            dprintf("  xMemObject:                0x%08x\n", MemObjectAddress);
            dprintf("    hHeap:                   0x%08x\n", MemObject.hHeap);
            dprintf("    pMemAllocation:          0x%08x\n", MemObject.pMemAllocation);
            dprintf("    szFile:                  %s\n", MemObject.szFile);
            dprintf("    dwLine:                  %u\n", MemObject.dwLine);
            dprintf("    dwThreadId:              0x%08x\n", MemObject.dwThreadId);
            dprintf("    LocalTime:               %02d/%02d/%04d %02d:%02d:%02d\n", MemObject.LocalTime.wMonth, MemObject.LocalTime.wDay, MemObject.LocalTime.wYear, MemObject.LocalTime.wHour, MemObject.LocalTime.wMinute, MemObject.LocalTime.wSecond);
            dprintf("    pPrevMemObject:          0x%08x\n", MemObject.pPrevMemObject);
            dprintf("    pNextMemObject:          0x%08x\n", MemObject.pNextMemObject);
            dprintf("\n");

            MemObjectAddress = MemObject.pNextMemObject;
        } while (NULL != MemObjectAddress);
    }
}



DECLARE_API( dumpallocations )
{
    // Address is the address of the pointer to the head of the memory allocation list
    ULONGLONG         Address = 0;
    // MemObject is the memory object
    XMEM_OBJECT       MemObject;
    // MemAllocationAddress is a pointer to the memory allocation address
    PXMEM_ALLOCATION  MemAllocationAddress = NULL;
    // MemAllocation is the memory allocation
    XMEM_ALLOCATION   MemAllocation;

    // bReturnValue is the return value of ReadMemory()
    BOOL              bReturnValue = FALSE;
    // cbBytesRead is the number of bytes read for ReadMemory()
    ULONG             cbBytesRead = 0;



    // Get the address of the pointer to the head of the memory allocation list
    Address = (ULONGLONG) GetExpression(args);

    if (NULL == Address) {
        dprintf("dumpallocations <address>\n");
        return;
    }

    // Get the XMEM_OBJECT structure
    ZeroMemory(&MemObject, sizeof(XMEM_OBJECT));
    bReturnValue = ReadMemory((ULONG) Address, &MemObject, sizeof(XMEM_OBJECT), &cbBytesRead);

    if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XMEM_OBJECT))) {
        dprintf("Cannot read 0x%08x bytes at 0x%08x for XMEM_OBJECT\n", sizeof(XMEM_OBJECT), Address);
        dprintf("\n");
        return;
    }

    // Get the pointer to the head of the memory allocation list
    MemAllocationAddress = MemObject.pMemAllocation;

    dprintf("*** xmem_kdx dumpallocations\n");
    dprintf("\n");

    if (NULL == MemAllocationAddress) {
        dprintf("  No memory allocations\n");
        dprintf("\n");
    }
    else {
        do {
            // Get the XMEM_ALLOCATION structure
            ZeroMemory(&MemAllocation, sizeof(XMEM_ALLOCATION));
            bReturnValue = ReadMemory((ULONG) MemAllocationAddress, &MemAllocation, sizeof(XMEM_ALLOCATION), &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XMEM_ALLOCATION))) {
                dprintf("Cannot read 0x%08x bytes at 0x%08x for XMEM_ALLOCATION\n", sizeof(XMEM_ALLOCATION), MemAllocationAddress);
                dprintf("\n");
                return;
            }

            dprintf("  xMemAllocation:            0x%08x\n", MemAllocationAddress);
            dprintf("    lpMem:                   0x%08x\n", MemAllocation.lpMem);
            dprintf("    lpMemTail:               0x%08x\n", MemAllocation.lpMemTail);
            dprintf("    dwBytes:                 %u\n", MemAllocation.dwBytes);
            dprintf("    szFile:                  %s\n", MemAllocation.szFile);
            dprintf("    dwLine:                  %u\n", MemAllocation.dwLine);
            dprintf("    dwThreadId:              0x%08x\n", MemAllocation.dwThreadId);
            dprintf("    LocalTime:               %02d/%02d/%04d %02d:%02d:%02d\n", MemAllocation.LocalTime.wMonth, MemAllocation.LocalTime.wDay, MemAllocation.LocalTime.wYear, MemAllocation.LocalTime.wHour, MemAllocation.LocalTime.wMinute, MemAllocation.LocalTime.wSecond);
            dprintf("    pPrevMemAllocation:      0x%08x\n", MemAllocation.pPrevMemAllocation);
            dprintf("    pNextMemAllocation:      0x%08x\n", MemAllocation.pNextMemAllocation);
            dprintf("\n");

            MemAllocationAddress = MemAllocation.pNextMemAllocation;
        } while (NULL != MemAllocationAddress);
    }
}



DECLARE_API( dumpall )
{
    // Address is the address of the pointer to the head of the memory allocation list
    ULONGLONG         Address = 0;
    // MemObjectAddress is a pointer to the memory object address
    PXMEM_OBJECT      MemObjectAddress = NULL;
    // MemObject is the memory object
    XMEM_OBJECT       MemObject;
    // MemAllocationAddress is a pointer to the memory allocation address
    PXMEM_ALLOCATION  MemAllocationAddress = NULL;
    // MemAllocation is the memory allocation
    XMEM_ALLOCATION   MemAllocation;

    // bReturnValue is the return value of ReadMemory()
    BOOL              bReturnValue = FALSE;
    // cbBytesRead is the number of bytes read for ReadMemory()
    ULONG             cbBytesRead = 0;



    // Get the address of the pointer to the head of the memory allocation list
    Address = (ULONGLONG) GetExpression(args);

    if (NULL == Address) {
        dprintf("dumpall <address>\n");
        return;
    }

    // Get the pointer to the head of the memory object list
    bReturnValue = ReadMemory((ULONG) Address, &MemObjectAddress, sizeof(PXMEM_OBJECT), &cbBytesRead);

    if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(PXMEM_OBJECT))) {
        dprintf("Cannot read 0x%08x bytes at 0x%08x for PXMEM_OBJECT\n", sizeof(PXMEM_OBJECT), Address);
        dprintf("\n");
        return;
    }

    dprintf("*** xmem_kdx dumpobjects\n");
    dprintf("\n");

    if (NULL == MemObjectAddress) {
        dprintf("  No memory objects\n");
        dprintf("\n");
    }
    else {
        do {
            // Get the XMEM_OBJECT structure
            ZeroMemory(&MemObject, sizeof(XMEM_OBJECT));
            bReturnValue = ReadMemory((ULONG) MemObjectAddress, &MemObject, sizeof(XMEM_OBJECT), &cbBytesRead);

            if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XMEM_OBJECT))) {
                dprintf("Cannot read 0x%08x bytes at 0x%08x for XMEM_OBJECT\n", sizeof(XMEM_OBJECT), MemObjectAddress);
                dprintf("\n");
                return;
            }

            dprintf("  xMemObject:                0x%08x\n", MemObjectAddress);
            dprintf("    hHeap:                   0x%08x\n", MemObject.hHeap);
            dprintf("    pMemAllocation:          0x%08x\n", MemObject.pMemAllocation);
            dprintf("    szFile:                  %s\n", MemObject.szFile);
            dprintf("    dwLine:                  %u\n", MemObject.dwLine);
            dprintf("    dwThreadId:              0x%08x\n", MemObject.dwThreadId);
            dprintf("    LocalTime:               %02d/%02d/%04d %02d:%02d:%02d\n", MemObject.LocalTime.wMonth, MemObject.LocalTime.wDay, MemObject.LocalTime.wYear, MemObject.LocalTime.wHour, MemObject.LocalTime.wMinute, MemObject.LocalTime.wSecond);
            dprintf("    pPrevMemObject:          0x%08x\n", MemObject.pPrevMemObject);
            dprintf("    pNextMemObject:          0x%08x\n", MemObject.pNextMemObject);
            dprintf("\n");

            MemAllocationAddress = MemObject.pMemAllocation;

            if (NULL == MemAllocationAddress) {
                dprintf("  No memory allocations\n");
                dprintf("\n");
            }
            else {
                do {
                    // Get the XMEM_ALLOCATION structure
                    ZeroMemory(&MemAllocation, sizeof(XMEM_ALLOCATION));
                    bReturnValue = ReadMemory((ULONG) MemAllocationAddress, &MemAllocation, sizeof(XMEM_ALLOCATION), &cbBytesRead);

                    if ((FALSE == bReturnValue) || (cbBytesRead != sizeof(XMEM_ALLOCATION))) {
                        dprintf("Cannot read 0x%08x bytes at 0x%08x for XMEM_ALLOCATION\n", sizeof(XMEM_ALLOCATION), MemAllocationAddress);
                        dprintf("\n");
                        return;
                    }

                    dprintf("    xMemAllocation:            0x%08x\n", MemAllocationAddress);
                    dprintf("      lpMem:                   0x%08x\n", MemAllocation.lpMem);
                    dprintf("      lpMemTail:               0x%08x\n", MemAllocation.lpMemTail);
                    dprintf("      dwBytes:                 %u\n", MemAllocation.dwBytes);
                    dprintf("      szFile:                  %s\n", MemAllocation.szFile);
                    dprintf("      dwLine:                  %u\n", MemAllocation.dwLine);
                    dprintf("      dwThreadId:              0x%08x\n", MemAllocation.dwThreadId);
                    dprintf("      LocalTime:               %02d/%02d/%04d %02d:%02d:%02d\n", MemAllocation.LocalTime.wMonth, MemAllocation.LocalTime.wDay, MemAllocation.LocalTime.wYear, MemAllocation.LocalTime.wHour, MemAllocation.LocalTime.wMinute, MemAllocation.LocalTime.wSecond);
                    dprintf("      pPrevMemAllocation:      0x%08x\n", MemAllocation.pPrevMemAllocation);
                    dprintf("      pNextMemAllocation:      0x%08x\n", MemAllocation.pNextMemAllocation);
                    dprintf("\n");

                    MemAllocationAddress = MemAllocation.pNextMemAllocation;
                } while (NULL != MemAllocationAddress);
            }

            MemObjectAddress = MemObject.pNextMemObject;
        } while (NULL != MemObjectAddress);
    }
}
