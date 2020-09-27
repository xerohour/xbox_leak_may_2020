/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    xeutils.c

Abstract:

    Miscellaneous utility routines for XE image header

Author:

    Sakphong Chanbai (schanbai) 11-Jul-2000

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"
#include "xeutils.h"


PVOID
WINAPI
XTestLibLocateXImageHeader(
    VOID
    )

/*++

Routine Description:

    This routine returns the XE image header

Arguments:

    None

Return Value:

    Pointer to XE image header or NULL

--*/

{
    return XapiGetCurrentProcess()->XeLdrImage->ImageHeader;
}


ULONG
WINAPI
XeModuleIndexFromAddress(
    ULONG_PTR Address
    )

/*++

Routine Description:

    This routine maps specified address to index into XE modules in XE image
    See PXIMAGE_HEADER and PXIMAGE_MODULE_HEADER for more information

Arguments:

    Address - Address to be mapped

Return Value:

    Module index or 0xFFFFFFFF if error

--*/

{
    ULONG i, ModuleIndex = ~0UL;
    PXIMAGE_HEADER ImageHeader;
    PXIMAGE_MODULE_HEADER ModuleHeader;
    PVOID VarImageHeader;

    ImageHeader = XTestLibLocateXImageHeader();
    
    if ( ImageHeader ) {
        VarImageHeader = (PVOID)(ImageHeader->HeadersBase + XIMAGE_FIXED_HEADER_SIZE(ImageHeader));
        ModuleHeader = XIMAGE_FIRST_MODULE(ImageHeader, VarImageHeader); 

        for ( i=0; i<ImageHeader->OptionalHeader.NumberOfModules; i++, ModuleHeader++ ) {
            if ( Address >= ModuleHeader->ImageBase && \
                 Address < (ModuleHeader->ImageBase + ModuleHeader->OrgSizeOfImage) ) {
                ModuleIndex = i;
                break;
            }
        }
    }

    return ModuleIndex;
}


CONST PVOID
WINAPI
XeModuleHeaderFromModuleIndex(
    ULONG ModuleIndex
    )

/*++

Routine Description:

    This routine returns pointer to XE module header specified by
    module index. See PXIMAGE_HEADER and PXIMAGE_MODULE_HEADER for
    more information

Arguments:

    ModuleIndex - Index into module headers

Return Value:

    Pointer to module header or NULL

--*/

{
    PXIMAGE_HEADER ImageHeader;
    PXIMAGE_MODULE_HEADER ModuleHeader;
    PVOID VarImageHeader;

    ImageHeader = XTestLibLocateXImageHeader();

    if ( !ImageHeader || ModuleIndex >= ImageHeader->OptionalHeader.NumberOfModules ) {
        return NULL;
    }

    VarImageHeader = (PVOID)(ImageHeader->HeadersBase + XIMAGE_FIXED_HEADER_SIZE(ImageHeader));
    ModuleHeader = XIMAGE_FIRST_MODULE(ImageHeader, VarImageHeader);

    return &ModuleHeader[ ModuleIndex ];
}


CONST PCHAR
WINAPI
XeModuleNameFromModuleHeader(
    PXIMAGE_MODULE_HEADER ModuleHeader
    )
{
    PXIMAGE_HEADER ImageHeader;
    PVOID MiscData, VarImageHeader;
    
    ImageHeader = XTestLibLocateXImageHeader();

    if ( !ImageHeader ) {
        return NULL;
    }

    VarImageHeader = (PVOID)(ImageHeader->HeadersBase + XIMAGE_FIXED_HEADER_SIZE(ImageHeader));
    MiscData = XIMAGE_MISCDATA( ImageHeader, VarImageHeader );

    return (PCHAR)MiscData + ModuleHeader->NameOfs;
}


CONST PVOID
WINAPI
XeModuleHeaderFromModuleName(
    LPCSTR ModuleName
    )

/*++

Routine Description:

    This routine returns pointer to XE module header specified by
    module name. See PXIMAGE_HEADER and PXIMAGE_MODULE_HEADER for
    more information

Arguments:

    ModuleName - Name of module

Return Value:

    Pointer to module header or NULL

--*/

{
    ULONG i;
    PXIMAGE_HEADER ImageHeader;
    PXIMAGE_MODULE_HEADER ModuleHeader, TargetModuleHeader=NULL;
    PVOID VarImageHeader;
    PCHAR ModName;

    ImageHeader = XTestLibLocateXImageHeader();
    
    if ( ImageHeader ) {
        VarImageHeader = (PVOID)(ImageHeader->HeadersBase + XIMAGE_FIXED_HEADER_SIZE(ImageHeader));
        ModuleHeader = XIMAGE_FIRST_MODULE(ImageHeader, VarImageHeader); 

        for ( i=0; i<ImageHeader->OptionalHeader.NumberOfModules; i++, ModuleHeader++ ) {
            ModName = XeModuleNameFromModuleHeader( ModuleHeader );
            if ( _stricmp(ModuleName, ModName) == 0 ) {
                TargetModuleHeader = ModuleHeader;
                break;
            }
        }
    }

    return TargetModuleHeader;
}


CONST PCHAR
WINAPI
XeModuleNameFromModuleIndex(
    ULONG ModuleIndex
    )

/*++

Routine Description:

    This routine returns pointer to XE module name specified by
    module index

Arguments:

    ModuleIndex - Index of module

Return Value:

    Pointer to module name or NULL

--*/

{
    PXIMAGE_MODULE_HEADER ModuleHeader;
    
    ModuleHeader = XeModuleHeaderFromModuleIndex( ModuleIndex );

    if ( !ModuleHeader ) {
        return NULL;
    }

    return XeModuleNameFromModuleHeader( ModuleHeader );
}
