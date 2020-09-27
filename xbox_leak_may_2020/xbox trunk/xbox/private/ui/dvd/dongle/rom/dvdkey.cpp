/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dvdkey.cpp

Abstract:

    This module contains DVD playback libraries for the Xbox Dashboard.
    The code is actually located in hardware key.

--*/

#include "dvdkeyp.h"

#if __cplusplus
extern "C" {
#endif

void _cinit ( void );
void _rtinit( void );

//
// Hack for new DVD code drop, this will be removed once we have new
// AvSendTVEncoderOption that doesn't require GPU address
//

volatile ULONG *D3D__GpuReg = NULL;

#if __cplusplus
}
#endif

ULONG NTAPI DDP_Initialize( void );

__declspec( naked )
NTSTATUS
__cdecl
mainCRTStartup(
    IN ULONG_PTR DashBegin,
    IN ULONG_PTR DashEnd,
    IN ULONG_PTR DvdBegin,
    IN ULONG_PTR DvdEnd
    )
/*++

Routine Description:

    This is a absolute entry point of the program. We need to define it here
    instead of using the version in XTL because we can't use XTL.  The code
    has to be written so that it contains the pattern at specific offset that
    imagebld looking for

Arguments:

    None

Return Value:

    Always TRUE

--*/
{
    SIZE_T Count;
    PEXPORT_API ExportPfn;
    PIMPORT_API ImportPfn;
    SIZE_T NumberOfDashAPIs;
    SIZE_T NumberOfDvdAPIs;
    SIZE_T NumberOfActualDashAPIs;
    SIZE_T NumberOfActualDvdAPIs;
    NTSTATUS Status;

    Status = STATUS_UNSUCCESSFUL;

    //
    // Use naked function and in-line assembly so that it contains particular
    // pattern at particular offset that imagebld is looking for
    //

    __asm {
        push    ebp
        mov     ebp, esp
        push    ecx
        xchg    ecx, ecx
        xchg    ecx, ecx
        xchg    ecx, ecx
        xchg    ecx, ecx
        nop
        mov     dword ptr [ebp-4], 0x0ff00110
        push    esi
        push    edi
        push    ebx
    }

    //
    // Calculate numbers of requested and actual thunks
    //

    NumberOfDashAPIs = (DashEnd-DashBegin) / sizeof(EXPORT_API);
    NumberOfDvdAPIs  = (DvdEnd-DvdBegin) / sizeof(IMPORT_API);

    NumberOfActualDashAPIs = ((ULONG_PTR)&DashAPIsEnd - (ULONG_PTR)&DashAPIsBegin) / sizeof(IMPORT_API);
    NumberOfActualDvdAPIs  = ((ULONG_PTR)&DvdAPIsEnd - (ULONG_PTR)&DvdAPIsBegin) / sizeof(EXPORT_API);

    if ( NumberOfDashAPIs != NumberOfActualDashAPIs) {
        KdPrint(( "DVD: invalid numbers of XDash APIs (expected %u, not %u)\n",
                  NumberOfActualDashAPIs, NumberOfDashAPIs ));
        Status = STATUS_INFO_LENGTH_MISMATCH;
        goto cleanup;
    }

    if ( NumberOfDvdAPIs != NumberOfActualDvdAPIs) {
        KdPrint(( "DVD: invalid numbers of DVD APIs (expected %u, not %u)\n",
                  NumberOfActualDvdAPIs, NumberOfDvdAPIs ));
        Status = STATUS_INFO_LENGTH_MISMATCH;
        goto cleanup;
    }

    //
    // Snap our import thunks to XDash export thunks
    //

    ImportPfn = (PIMPORT_API)&DashAPIsBegin;
    ExportPfn = (PEXPORT_API)DashBegin;

    for ( Count=0; Count<NumberOfActualDashAPIs; Count++ ) {
        ASSERT( ImportPfn->__jmp == 0xE9 );
        ASSERT( ImportPfn->__int3 == 0xCC );
        ImportPfn->Relative = *ExportPfn - (ULONG_PTR)&ImportPfn->__int3;
        ImportPfn++;
        ExportPfn++;
    }

    //
    // Snap XDash import thunks to our export thunks
    //

    ImportPfn = (PIMPORT_API)DvdBegin;
    ExportPfn = (PEXPORT_API)&DvdAPIsBegin;

    for ( Count=0; Count<NumberOfActualDvdAPIs; Count++ ) {
        ASSERT( ImportPfn->__jmp == 0xE9 );
        ASSERT( ImportPfn->__int3 == 0xCC );
        ImportPfn->Relative = *ExportPfn - (ULONG_PTR)&ImportPfn->__int3;
        ImportPfn++;
        ExportPfn++;
    }

    //
    // Self-modified code needs to flush cache and execute serialize instruction
    // See "Intel Software Developer's Manual" Volume 3, Chapter 7 for more
    // Information.
    //

    __asm {
        wbinvd
        push    ebx
        xor     eax, eax
        cpuid
        pop     ebx
    }

    //
    // Initialize C run-time library
    //

    _rtinit();
    _cinit();

    Status = DDP_Initialize();

cleanup:

    __asm {
        pop     ebx
        pop     edi
        pop     esi
        mov     eax, Status
        leave
        ret     0
    }
}

