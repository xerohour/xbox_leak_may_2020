/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    palette.cpp

Author:

    Matt Bronder

Description:

    Direct3D palette routines.

*******************************************************************************/

#include "d3dbase.h"

#ifdef UNDER_XBOX

//******************************************************************************
// Structures
//******************************************************************************

typedef struct _PALETTEDESC {
    LPDIRECT3DPALETTE8          pd3dp;
    D3DPALETTESIZE              palsSize;
    UINT                        uEntries;
    struct _PALETTEDESC*        ppaldNext;
} PALETTEDESC, *PPALETTEDESC;

//******************************************************************************
// Globals
//******************************************************************************

PPALETTEDESC                    g_ppaldList = NULL;

//******************************************************************************
// Palette functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreatePalette
//
// Description:
//
//     Create an palette object of the given size and populate it with the 
//     given palette colors.  The size of the palette entries must
//     be at least as large as the palette being created.
//
// Arguments:
//
//     LPDIRECT3DDEVICE8 pDevice            - Pointer to the device object
//
//     D3DCOLOR* pcColors                   - Colors to store in the palette
//
//     D3DPALETTESIZE palsSize              - Number of entries in the palette
//
// Return Value:
//
//     A pointer to the created palette object on success, NULL on 
//     failure.
//
//******************************************************************************
LPDIRECT3DPALETTE8 CreatePalette(LPDIRECT3DDEVICE8 pDevice, D3DCOLOR* pcColors, D3DPALETTESIZE palsSize) {

    LPDIRECT3DPALETTE8      pd3dp;
    PPALETTEDESC            ppaldNode;
    D3DCOLOR*               pcData;
    UINT                    uEntries;
    HRESULT                 hr;

    hr = pDevice->CreatePalette(palsSize, &pd3dp);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::CreatePalette"));
        return NULL;
    }

    switch (palsSize) {
        case D3DPALETTE_32:
            uEntries = 32;
            break;
        case D3DPALETTE_64:
            uEntries = 64;
            break;
        case D3DPALETTE_128:
            uEntries = 128;
            break;
        case D3DPALETTE_256:
        default:
            uEntries = 256;
            break;
    }

    if (pcColors) {

        hr = pd3dp->Lock(&pcData, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DPalette8::Lock"));
            pd3dp->Release();
            return NULL;
        }

        memcpy(pcData, pcColors, uEntries * sizeof(D3DCOLOR));

        hr = pd3dp->Unlock();
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DPalette8::Unlock"));
            pd3dp->Release();
            return NULL;
        }
    }

    // Add the palette to the list
    ppaldNode = (PPALETTEDESC)MemAlloc(sizeof(PALETTEDESC));
    if (!ppaldNode) {
        pd3dp->Release();
        return NULL;
    }
    memset(ppaldNode, 0, sizeof(PALETTEDESC));
    ppaldNode->pd3dp = pd3dp;
    ppaldNode->palsSize = palsSize;
    ppaldNode->uEntries = uEntries;
    ppaldNode->ppaldNext = g_ppaldList;
    g_ppaldList = ppaldNode;

    return pd3dp;
}

//******************************************************************************
//
// Function:
//
//     ReleasePalette
//
// Description:
//
//     Release the given palette object and remove it from the palette table.
//
// Arguments:
//
//     LPDIRECT3DPALETTE8 pd3dp               - Pointer to the palette object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleasePalette(LPDIRECT3DPALETTE8 pd3dp) {

    PPALETTEDESC    ppald, ppaldDel;
#if defined(DEBUG) || defined(_DEBUG)
    UINT            uRef;
#endif

    if (!pd3dp) {
        return;
    }

    if (g_ppaldList) {

        if (g_ppaldList->pd3dp == pd3dp) {
            ppald = g_ppaldList->ppaldNext;
            MemFree(g_ppaldList);
            g_ppaldList = ppald;
        }

        else {

            for (ppald = g_ppaldList; 
                 ppald->ppaldNext && ppald->ppaldNext->pd3dp != pd3dp; 
                 ppald = ppald->ppaldNext
            );

            if (ppald->ppaldNext) {
                ppaldDel = ppald->ppaldNext;
                ppald->ppaldNext = ppaldDel->ppaldNext;
                MemFree(ppaldDel);
            }
        }
    }

#if defined(DEBUG) || defined(_DEBUG)
    uRef = pd3dp->Release();
    if (uRef != 0) {
        DebugString(TEXT("WARNING: Palette has a ref count of %d on release"), uRef);
    }
#else
    pd3dp->Release();
#endif
}

//******************************************************************************
//
// Function:
//
//     ReleasePalette
//
// Description:
//
//     Release all palettes.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleasePalettes() {

    while (g_ppaldList) {
        ReleasePalette(g_ppaldList->pd3dp);
    }
}

//******************************************************************************
//
// Function:
//
//     RemovePalette
//
// Description:
//
//     Remove the given palette from the palette table (if present)
//
// Arguments:
//
//     LPDIRECT3DPALETTE8 pd3dp         - Pointer to the palette object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RemovePalette(LPDIRECT3DPALETTE8 pd3dp) {

    PPALETTEDESC    ppald, ppaldDel;

    if (!pd3dp) {
        return;
    }

    if (g_ppaldList) {

        if (g_ppaldList->pd3dp == pd3dp) {
            ppald = g_ppaldList->ppaldNext;
            MemFree(g_ppaldList);
            g_ppaldList = ppald;
        }

        else {

            for (ppald = g_ppaldList; 
                 ppald->ppaldNext && ppald->ppaldNext->pd3dp != pd3dp; 
                 ppald = ppald->ppaldNext
            );

            if (ppald->ppaldNext) {
                ppaldDel = ppald->ppaldNext;
                ppald->ppaldNext = ppaldDel->ppaldNext;
                MemFree(ppaldDel);
            }
        }
    }
}

#endif // UNDER_XBOX
