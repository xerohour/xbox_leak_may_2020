/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    pemodule.cpp

Abstract:

    This module implements Portable Executable (PE) module related routines for
    the utility program to build an Xbox executable image.

--*/

#include "imgbldp.h"

PIMAGE_SECTION_HEADER
ImgbNameToSectionHeader(
    PIMAGE_NT_HEADERS NtHeader,
    PUCHAR SearchName,
    PULONG SectionIndex
    )
{
    ULONG SectionsRemaining;
    PIMAGE_SECTION_HEADER SectionHeader;
    UCHAR SectionName[IMAGE_SIZEOF_SHORT_NAME + 1];

    //
    // Walk through each of the image's sections and find the section that has
    // the requested section name.
    //

    SectionsRemaining = NtHeader->FileHeader.NumberOfSections;
    SectionHeader = IMAGE_FIRST_SECTION(NtHeader);

    while (SectionsRemaining) {

        memcpy(SectionName, SectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);
        SectionName[IMAGE_SIZEOF_SHORT_NAME] = '\0';

        if (_stricmp((PSTR)SectionName, (PSTR)SearchName) == 0) {
            *SectionIndex = SectionHeader - IMAGE_FIRST_SECTION(NtHeader);
            return SectionHeader;
        }

        SectionsRemaining--;
        SectionHeader++;
    }

    return NULL;
}

PIMAGE_SECTION_HEADER
ImgbVirtualAddressToSectionHeader(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG VirtualAddress
    )
{
    ULONG SectionsRemaining;
    PIMAGE_SECTION_HEADER SectionHeader;

    //
    // Walk through each of the image's sections and find the section that
    // contains the requested virtual address.
    //

    SectionsRemaining = NtHeader->FileHeader.NumberOfSections;
    SectionHeader = IMAGE_FIRST_SECTION(NtHeader);

    while (SectionsRemaining) {

        if ((VirtualAddress >= SectionHeader->VirtualAddress) &&
            (VirtualAddress < SectionHeader->VirtualAddress + SectionHeader->SizeOfRawData)) {
            return SectionHeader;
        }

        SectionsRemaining--;
        SectionHeader++;
    }

    //
    // If the section mapping the requested virtual address cannot be found,
    // then the input file must be corrupt.
    //

    ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
    return NULL;
}

LPVOID
ImgbVirtualAddressToData(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG VirtualAddress
    )
{
    PIMAGE_SECTION_HEADER SectionHeader;

    SectionHeader = ImgbVirtualAddressToSectionHeader(NtHeader, VirtualAddress);

    return (LPBYTE)ImgbInputFileMappingView + SectionHeader->PointerToRawData +
        (VirtualAddress - SectionHeader->VirtualAddress);
}

LPVOID
ImgbImageDataDirectoryToData(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG DataDirectoryIndex
    )
{
    return ImgbVirtualAddressToData(NtHeader,
        NtHeader->OptionalHeader.DataDirectory[DataDirectoryIndex].VirtualAddress);
}

LPVOID
ImgbLoadAddressToData(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG LoadAddress
    )
{
    ULONG VirtualAddress;

    VirtualAddress = LoadAddress - NtHeader->OptionalHeader.ImageBase;

    return ImgbVirtualAddressToData(NtHeader, VirtualAddress);
}

PIMAGE_BASE_RELOCATION
ImgbProcessRelocationBlock(
    PIMAGE_NT_HEADERS NtHeader,
    IN ULONG VA,
    IN ULONG SizeOfBlock,
    IN PUSHORT NextOffset,
    IN LONG_PTR Diff
    )
{
    PUCHAR FixupVA;
    USHORT Offset;
    LONG Temp;

    while (SizeOfBlock--) {

       FixupVA = NULL;
       Offset = (USHORT)(*NextOffset & (USHORT)0xfff);

       //
       // Apply the fixups.
       //

       switch ((*NextOffset) >> 12) {

            case IMAGE_REL_BASED_HIGHLOW :
                //
                // HighLow - (32-bits) relocate the high and low half
                //      of an address.
                //
                FixupVA = (PUCHAR)ImgbVirtualAddressToData(NtHeader, VA + Offset);
                *(LONG UNALIGNED *)FixupVA += (ULONG) Diff;
                break;

            case IMAGE_REL_BASED_HIGH :
                //
                // High - (16-bits) relocate the high half of an address.
                //
                FixupVA = (PUCHAR)ImgbVirtualAddressToData(NtHeader, VA + Offset);
                Temp = *(PUSHORT)FixupVA << 16;
                Temp += (ULONG) Diff;
                *(PUSHORT)FixupVA = (USHORT)(Temp >> 16);
                break;

            case IMAGE_REL_BASED_HIGHADJ :
                //
                // Adjust high - (16-bits) relocate the high half of an
                //      address and adjust for sign extension of low half.
                //
                FixupVA = (PUCHAR)ImgbVirtualAddressToData(NtHeader, VA + Offset);
                Temp = *(PUSHORT)FixupVA << 16;
                ++NextOffset;
                --SizeOfBlock;
                Temp += (LONG)(*(PSHORT)NextOffset);
                Temp += (ULONG) Diff;
                Temp += 0x8000;
                *(PUSHORT)FixupVA = (USHORT)(Temp >> 16);

                break;

            case IMAGE_REL_BASED_LOW :
                //
                // Low - (16-bit) relocate the low half of an address.
                //
                FixupVA = (PUCHAR)ImgbVirtualAddressToData(NtHeader, VA + Offset);
                Temp = *(PSHORT)FixupVA;
                Temp += (ULONG) Diff;
                *(PUSHORT)FixupVA = (USHORT)Temp;
                break;

            case IMAGE_REL_BASED_ABSOLUTE :
                //
                // Absolute - no fixup required.
                //
                break;

            case IMAGE_REL_BASED_SECTION :
                //
                // Section Relative reloc.  Ignore for now.
                //
                break;

            case IMAGE_REL_BASED_REL32 :
                //
                // Relative intrasection. Ignore for now.
                //
                break;

            default :
                //
                // Illegal - illegal relocation type.
                //

                return (PIMAGE_BASE_RELOCATION)NULL;
       }
       ++NextOffset;
    }
    return (PIMAGE_BASE_RELOCATION)NextOffset;
}

VOID
ImgbRelocateImage(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG OldBaseAddress,
    ULONG NewBaseAddress,
    PIMAGE_BASE_RELOCATION NextBlock,
    ULONG TotalCountBytes
    )
{
    LONG_PTR Diff;
    ULONG SizeOfBlock;
    PUSHORT NextOffset;

    //
    // If the image has a relocation table, then apply the specified fixup
    // information to the image.
    //

    while (TotalCountBytes) {
        SizeOfBlock = NextBlock->SizeOfBlock;
        TotalCountBytes -= SizeOfBlock;
        SizeOfBlock -= sizeof(IMAGE_BASE_RELOCATION);
        SizeOfBlock /= sizeof(USHORT);
        NextOffset = (PUSHORT)((PCHAR)NextBlock + sizeof(IMAGE_BASE_RELOCATION));

        Diff = NewBaseAddress - OldBaseAddress;

        if ( (NextBlock = ImgbProcessRelocationBlock(NtHeader,NextBlock->VirtualAddress,SizeOfBlock,NextOffset,Diff)) == NULL ) {
            ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
        }
    }
}
