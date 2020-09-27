/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    xemodule.c

Abstract:

    This module contains implementation of the routines that interface with
    XE loader in the kernel

--*/

#include "basedll.h"
#pragma hdrstop

#define IsValidSectionHandle(hSection) \
    (((PVOID)hSection >= (PVOID)XeImageHeader()->SectionHeaders) && \
    ((PVOID)hSection < (PVOID)(XeImageHeader()->SectionHeaders + XeImageHeader()->NumberOfSections)))

HANDLE
WINAPI
XGetSectionHandle(
    IN LPCSTR lpSectionName
    )
/*++

Routine Description:

    Finds a section in the current XE image

Arguments:

    lpSectionName - Supplies the name of the section to find.

Return Value:

    A handle to the section if found,
    otherwise INVALID_HANDLE_VALUE (GetLastError)

--*/
{
    STRING SearchString;
    PXBEIMAGE_SECTION Section;
    PXBEIMAGE_SECTION EndingSection;
    STRING SectionName;

    RIP_ON_NOT_TRUE("XGetSectionHandle()", (lpSectionName != NULL));

    RtlInitAnsiString(&SearchString, lpSectionName);

    //
    // Walk through each of the sections looking for a match by name.
    //

    Section = XeImageHeader()->SectionHeaders;
    EndingSection = Section + XeImageHeader()->NumberOfSections;

    while (Section < EndingSection) {

        RtlInitAnsiString(&SectionName, Section->SectionName);

        if (RtlEqualString(&SearchString, &SectionName, TRUE)) {
            return (HANDLE)Section;
        }

        Section++;
    }

    //
    // The section name was not found.
    //

    SetLastError(ERROR_NOT_FOUND);

    return INVALID_HANDLE_VALUE;
}

LPVOID
WINAPI
XLoadSectionByHandle(
    IN HANDLE hSection
    )
/*++

Routine Description:

    Loads a section from the current XE image file into memory.  If the section
    is already loaded, its reference count is incremented,

Arguments:

    hSection - Supplies the handle of the section

Return Value:

    Return the virtual address of the loaded section if succesful
    otherwise NULL (GetLastError)

--*/
{
    NTSTATUS status;
    PXBEIMAGE_SECTION Section;

    RIP_ON_NOT_TRUE("XLoadSectionByHandle()", IsValidSectionHandle(hSection));

    Section = (PXBEIMAGE_SECTION)hSection;

    status = XeLoadSection(Section);

    if (!NT_SUCCESS(status)) {
        XapiSetLastNTError(status);
        return NULL;
    }

    return (LPVOID)Section->VirtualAddress;
}

BOOL
WINAPI
XFreeSectionByHandle(
    HANDLE hSection
    )
/*++

Routine Description:

    Unloads a section from memory.  The section is only unloaded when
    its reference count goes to zero.

Arguments:

    hSection - Supplies the handle of the section

Return Value:

    TRUE if successful, 
    FALSE otherwise (GetLastError)

--*/
{
    NTSTATUS status;
    PXBEIMAGE_SECTION Section;

    RIP_ON_NOT_TRUE("XFreeSectionByHandle()", IsValidSectionHandle(hSection));

    Section = (PXBEIMAGE_SECTION)hSection;

    status = XeUnloadSection(Section);

    if (!NT_SUCCESS(status)) {
        XapiSetLastNTError(status);
        return FALSE;
    }

    return TRUE;
}

PVOID
WINAPI
XLoadSection(
    IN LPCSTR lpSectionName
    )
/*++

Routine Description:

    Loads a section from the current XE image file into memory.  If the section
    is already loaded, its reference count is incremented,

Arguments:

    lpSectionName - Supplies the name of the section from the XE file

Return Value:

    Return the virtual address of the loaded section if succesful
    otherwise NULL (GetLastError)

--*/
{
    HANDLE hSection;

    RIP_ON_NOT_TRUE("XLoadSection()", (lpSectionName != NULL));

    hSection = XGetSectionHandle(lpSectionName);

    if (hSection != INVALID_HANDLE_VALUE) {
        return XLoadSectionByHandle(hSection);
    } else {
        return NULL;
    }
}

BOOL
WINAPI
XFreeSection(
    IN LPCSTR lpSectionName
    )
/*++

Routine Description:

    Unloads a section from memory.  The section is only unloaded when
    its reference count goes to zero.

Arguments:

    lpSectionName - Supplies the name of the section from the XE file

Return Value:

    TRUE if successful,
    FALSE otherwise (GetLastError)

--*/
{
    HANDLE hSection;

    RIP_ON_NOT_TRUE("XFreeSection()", (lpSectionName != NULL));

    hSection = XGetSectionHandle(lpSectionName);

    if (hSection != INVALID_HANDLE_VALUE) {
        return XFreeSectionByHandle(hSection);
    } else {
        return FALSE;
    }
}

DWORD
WINAPI
XGetSectionSize(
    IN HANDLE hSection
    )
/*++

Routine Description:

    Returns the size of a section

Arguments:

    hSection - Supplies the handle of the section

Return Value:

    Return the size of a section or 0 (GetLastError)

--*/
{
    PXBEIMAGE_SECTION Section;

    RIP_ON_NOT_TRUE("XGetSectionSize()", IsValidSectionHandle(hSection));

    Section = (PXBEIMAGE_SECTION)hSection;

    return Section->VirtualSize;
}
