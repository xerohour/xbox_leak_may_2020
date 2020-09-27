/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    anistub.c

Abstract:

    This module implements empty boot animation stub routines for the ARCADE
    build.

--*/

#include <ntos.h>
#include <ani.h>
#include <av.h>
#include <avmode.h>
#include <xpcicfg.h>
#include <xtl.h>

#pragma code_seg("INIT")

VOID AniStartAnimation(BOOLEAN fShort)
{
    ULONG Step;

    //
    // D3D8.LIB assumes that D3D8NTPR.LIB initially set the display mode.  The
    // ARCADE kernel doesn't use D3D8NTPR.LIB, so we need to set the display
    // mode ourselves.  We set the display mode to OFF with the same D3DFORMAT
    // and pitch that the console boot animation would use.
    //

    Step = 0;

    do {
        Step = AvSetDisplayMode((PVOID)XPCICFG_GPU_MEMORY_REGISTER_BASE_0, 0,
            AV_MODE_OFF, D3DFMT_LIN_A8R8G8B8, 640 * 4, 0);
    } while (Step);
}

VOID AniTerminateAnimation()
{
    NOTHING;
}

VOID AniBlockOnAnimation()
{
    NOTHING;
}

VOID AniSetLogo(PVOID pv, ULONG Size)
{
    NOTHING;
}
