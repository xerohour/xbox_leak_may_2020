/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    d3denum.cpp

Description:

    Direct3D initialization routines.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <d3dx8.h>
#include "d3dinit.h"
#include "main.h"
#include "util.h"
#include "d3denum.h"

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define D3DDEVTYPE_NULL ((D3DDEVTYPE)0)

//******************************************************************************
// Local function prototypes
//******************************************************************************

namespace DXCONIO {

static DWORD            SelectVertexPipeline(D3DCAPS8* pd3dcaps);
static D3DFORMAT        SelectDepthStencilFormat(LPDIRECT3D8 pd3d, 
                            UINT uAdapter, D3DDEVTYPE d3ddt, 
                            D3DFORMAT d3dfmtMode);
static
D3DMULTISAMPLE_TYPE     SelectMultiSampleType(LPDIRECT3D8 pd3d, UINT uAdapter, 
                            D3DDEVTYPE d3ddt, D3DFORMAT d3dfmtMode,
                            BOOL bWindowed);

//******************************************************************************
// Globals
//******************************************************************************

ADAPTERDESC             g_adpdDefault = {
                            (UINT)(-1), {0}, D3DFMT_UNKNOWN, NULL, NULL
                        };
DEVICEDESC              g_devdDefault = {
                            D3DDEVTYPE_NULL, {D3DDEVTYPE_NULL, 0}, 0, TRUE, D3DFMT_UNKNOWN, 
                            D3DMULTISAMPLE_NONE, NULL, NULL
                        };
DISPLAYMODE             g_dmDefault = {
                            {0}, D3DFMT_UNKNOWN, D3DMULTISAMPLE_NONE, NULL
                        };
D3DFORMAT               g_fmtdDefault = D3DFMT_UNKNOWN;

static CHECKDISPLAYPROC g_pfnValidate = NULL;

//******************************************************************************
//
// Function:
//
//     EnumDisplays
//
// Description:
//
//     Build a display device list containing information about all supported
//     adapters, devices, and display modes present on the system at the time
//     the Direct3D object was created.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     A pointer to the head of the display adapter list (NULL if no adapters
//     are supported).
//
//******************************************************************************
PADAPTERDESC EnumDisplays(LPDIRECT3D8 pd3d) {

    PADAPTERDESC        padpdHead = NULL, padpdNode;
    UINT                uAdapter, uNumAdapters;
    PDEVICEDESC         pdevdNode;
    D3DDISPLAYMODE      d3ddm;
    PDISPLAYMODE        pdmNode;
    UINT                uMode, uNumModes;
    D3DFORMAT           fmt;
    D3DMULTISAMPLE_TYPE d3dmst;
    HRESULT             hr;
    UINT                uDevice;
    D3DDEVTYPE          d3ddt[] = {
                            D3DDEVTYPE_HAL,
                            D3DDEVTYPE_SW,
                            D3DDEVTYPE_REF,
                        };

    if (!pd3d) {
        return NULL;
    }

    // Enumerate adapters
    uNumAdapters = pd3d->GetAdapterCount();

    for (uAdapter = 0; uAdapter < pd3d->GetAdapterCount(); uAdapter++) {

        // Allocate memory for the adapter information
        padpdNode = (PADAPTERDESC)MemAlloc(sizeof(ADAPTERDESC));
        if (!padpdNode) {
            DebugString(TEXT("ERROR: Not enough memory available to store adapter information"));
            break;
        }

        // Save the adapter number
        padpdNode->uAdapter = uAdapter;

        // Save the adapter idenifier
        hr = pd3d->GetAdapterIdentifier(uAdapter, 0, &padpdNode->d3dai);
        if (ResultFailed(hr, TEXT("IDirect3D8::GetDeviceIdentifier"))) {
            MemFree(padpdNode);
            continue;
        }

        // Save the initial display mode of the adapter (for use in windowed applications)
        hr = pd3d->GetAdapterDisplayMode(uAdapter, &d3ddm);
        if (ResultFailed(hr, TEXT("IDirect3D8::GetAdapterDisplayMode"))) {
            MemFree(padpdNode);
            continue;
        }
        padpdNode->fmtrWindowed = d3ddm.Format;

        padpdNode->pdevdList = NULL;

        // Enumerate devices
#ifndef UNDER_XBOX
        for (uDevice = 0; uDevice < 3; uDevice++) {
#else
        for (uDevice = 0; !uDevice; uDevice++) {
#endif // UNDER_XBOX

            // Allocate memory for the device information
            pdevdNode = (PDEVICEDESC)MemAlloc(sizeof(DEVICEDESC));
            if (!pdevdNode) {
                DebugString(TEXT("ERROR: Not enough memory available to store device information"));
                break;
            }

            // Save the device type
            pdevdNode->d3ddt = d3ddt[uDevice];
            
            // Save the caps
            hr = pd3d->GetDeviceCaps(uAdapter, d3ddt[uDevice], &pdevdNode->d3dcaps);
            if (ResultFailed(hr, TEXT("IDirect3D8::GetDeviceCaps"))) {
                MemFree(pdevdNode);
                continue;
            }

            // The device supports windowed mode if the caps exposes it and
            // the device can render to the initial display mode of the adapter
#ifndef UNDER_XBOX
            hr = g_pd3d->CheckDeviceType(uAdapter, d3ddt[uDevice], padpdNode->fmtrWindowed, padpdNode->fmtrWindowed, TRUE);
#else
            hr = D3DERR_NOTAVAILABLE;
#endif // DEMO_HACK
            pdevdNode->bCanRenderWindowed = ((pdevdNode->d3dcaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) && SUCCEEDED(hr));

            // Select where the vertex processing is to be performed
            pdevdNode->dwBehavior = SelectVertexPipeline(&pdevdNode->d3dcaps);

            if (SUCCEEDED(hr)) {

                // Select a depth/stencil buffer format for a windowed device
                pdevdNode->fmtdWindowed = SelectDepthStencilFormat(pd3d, uAdapter, d3ddt[uDevice], padpdNode->fmtrWindowed);

                // Select a multisample type for a windowed device
#ifndef UNDER_XBOX
                pdevdNode->d3dmstWindowed = SelectMultiSampleType(pd3d, uAdapter, d3ddt[uDevice], padpdNode->fmtrWindowed, TRUE);
#else
                pdevdNode->d3dmstWindowed = D3DMULTISAMPLE_NONE;
#endif
            }

            // Save the display modes supported by the adapter, device, and application
            uNumModes = pd3d->GetAdapterModeCount(uAdapter);
            pdevdNode->pdmList = NULL;

            for (uMode = 0; uMode < uNumModes; uMode++) {

                hr = pd3d->EnumAdapterModes(uAdapter, uMode, &d3ddm);
                if (ResultFailed(hr, TEXT("IDirect3D8::EnumAdapterModes"))) {
                    continue;
                }

                // Select a depth/stencil buffer format for the full screen display mode
                fmt = SelectDepthStencilFormat(pd3d, uAdapter, d3ddt[uDevice], d3ddm.Format);

                // Select a multisample type for the full screen display mode
                d3dmst = SelectMultiSampleType(pd3d, uAdapter, d3ddt[uDevice], d3ddm.Format, FALSE);

                // Check if the device supports the display mode format
                hr = pd3d->CheckDeviceType(uAdapter, d3ddt[uDevice], d3ddm.Format, d3ddm.Format, FALSE);
                if (FAILED(hr)) {
                    if (hr != D3DERR_INVALIDDEVICE) {
                        ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceTypes"));
                    }
                    continue;
                }

                // Check if the application supports the display mode format
                if (g_pfnValidate && !g_pfnValidate(pd3d, &pdevdNode->d3dcaps, &d3ddm)) {
                    // The application rejected the mode, skip it
                    continue;
                }
                        
                // Allocate memory for the display mode information
                pdmNode = (PDISPLAYMODE)MemAlloc(sizeof(DISPLAYMODE));
                if (!pdmNode) {
                    DebugString(TEXT("ERROR: Not enough memory available to store display mode information"));
                    break;
                }

                // Save the display mode
                memcpy(&pdmNode->d3ddm, &d3ddm, sizeof(D3DDISPLAYMODE));

                // Save the selected depth/stencil format
                pdmNode->fmtd = fmt;

                // Save the selected multisample type
                pdmNode->d3dmst = d3dmst;

                // Add the display mode to the list
                pdmNode->pdmNext = pdevdNode->pdmList;
                pdevdNode->pdmList = pdmNode;
            }

            // If no display modes are supported by the device, skip it
            if (!pdevdNode->pdmList) {
                MemFree(pdevdNode);
                continue;
            }

            // Add the device to the list
            pdevdNode->pdevdNext = padpdNode->pdevdList;
            padpdNode->pdevdList = pdevdNode;
        }

        // If no devices are supported by the adapter, skip it
        if (!padpdNode->pdevdList) {
            MemFree(padpdNode);
            continue;
        }

        // Add the adapter to the list
        padpdNode->padpdNext = padpdHead;
        padpdHead = padpdNode;
    }

    return padpdHead;
}

//******************************************************************************
void InitCheckDisplayProc(CHECKDISPLAYPROC pfnValidate) {
    
    g_pfnValidate = pfnValidate;
}

//******************************************************************************
// Initialization functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     SelectAdapter
//
// Description:
//
//     Select a default adapter for the application from a list of available
//     adapters.
//
// Arguments:
//
//     PADAPTERDESC padpdList   - List of the available adapters.
//
// Return Value:
//
//     A pointer to the selected adapter in the list.
//
//******************************************************************************
PADAPTERDESC SelectAdapter(PADAPTERDESC padpdList) {

    PADAPTERDESC padpd;

    if (!padpdList) {
        return NULL;
    }

    // Look for the default adapter in the list
    if (g_adpdDefault.uAdapter != (UINT)(-1)) {
        for (padpd = padpdList; padpd; padpd = padpd->padpdNext) {
            if (padpd->uAdapter == g_adpdDefault.uAdapter) 
            {
                return padpd;
            }
        }
    }

    // Select the primary display adapter
    for (padpd = padpdList; padpd; padpd = padpd->padpdNext) {

        if (padpd->uAdapter == 0) {
            return padpd;
        }
    }

    return padpdList;
}

//******************************************************************************
//
// Function:
//
//     SelectDevice
//
// Description:
//
//     Select the default device for the application from a list of available
//     devices.
//
// Arguments:
//
//     PADAPTERDESC padpd       - The adapter from which to select the default
//                                device.
//
// Return Value:
//
//     A pointer to the selected device in the list.
//
//******************************************************************************
PDEVICEDESC SelectDevice(PADAPTERDESC padpd) {

    PDEVICEDESC pdevd,
                pdevdSelection = NULL;

    if (!padpd) {
        return NULL;
    }

    // Look for the default device in the list
    if (g_devdDefault.d3ddt != D3DDEVTYPE_NULL) {
        for (pdevd = padpd->pdevdList; pdevd; pdevd = pdevd->pdevdNext) {
            if (pdevd->d3ddt == g_devdDefault.d3ddt) {
                return pdevd;
            }
        }
    }

    for (pdevd = padpd->pdevdList; pdevd; pdevd = pdevd->pdevdNext) {

        // If the device is a HAL device, select it and return
        if (pdevd->d3ddt == D3DDEVTYPE_HAL) {
            return pdevd;
        }

        // If the device is an emulation device, save it and keep looking
        if (pdevd->d3ddt == D3DDEVTYPE_SW) {
            pdevdSelection = pdevd;
        }

        // If no device has yet been selected, save the reference device 
        // and keep looking
        if (!pdevdSelection) {
            pdevdSelection = pdevd;
        }
    }

    return pdevdSelection;
}

//******************************************************************************
//
// Function:
//
//     SelectDisplayMode
//
// Description:
//
//     Select the default display mode for the application from a list of 
//     display modes available to the given device.
//
// Arguments:
//
//     PDEVICEDESC pdevd        - The device from which to select the default
//                                display mode.
//
// Return Value:
//
//     A pointer to the selected display mode in the list.
//
//******************************************************************************
PDISPLAYMODE SelectDisplayMode(PDEVICEDESC pdevd) {

    PDISPLAYMODE pdm;

    if (!pdevd) {
        return NULL;
    }

    // Look for the default display mode in the list
    if (g_dmDefault.d3ddm.Width != 0 && g_dmDefault.d3ddm.Height != 0) {
        for (pdm = pdevd->pdmList; pdm; pdm = pdm->pdmNext) {
            if (pdm->d3ddm.Width == g_dmDefault.d3ddm.Width && 
                pdm->d3ddm.Height == g_dmDefault.d3ddm.Height &&
                pdm->d3ddm.Format == g_dmDefault.d3ddm.Format) 
            {
                if (pdm->d3ddm.RefreshRate == g_dmDefault.d3ddm.RefreshRate ||
                    g_dmDefault.d3ddm.RefreshRate == 0) 
                {
                    return pdm;
                }
            }
        }
    }

    for (pdm = pdevd->pdmList; pdm; pdm = pdm->pdmNext) {

        if (pdm->d3ddm.Width == 640 && pdm->d3ddm.Height == 480 &&
            FormatToBitDepth(pdm->d3ddm.Format) == 16) 
        {
            return pdm;
        }
    }

    return pdevd->pdmList;
}

//******************************************************************************
static DWORD SelectVertexPipeline(D3DCAPS8* pd3dcaps) {

    if (pd3dcaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
        return D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else {
        return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
}

//******************************************************************************
static D3DFORMAT SelectDepthStencilFormat(LPDIRECT3D8 pd3d, 
                                UINT uAdapter, D3DDEVTYPE d3ddt, 
                                D3DFORMAT d3dfmtMode)
{
    HRESULT     hr;
    UINT        i;
    D3DFORMAT   d3dfmt[] = {
                    D3DFMT_D24S8,
#ifndef UNDER_XBOX
                    D3DFMT_D24X4S4,
                    D3DFMT_D15S1,
                    D3DFMT_D32,
                    D3DFMT_D24X8,
#else
                    D3DFMT_F24S8,
#endif // UNDER_XBOX
                    D3DFMT_D16_LOCKABLE,
                    D3DFMT_D16,
#ifdef UNDER_XBOX
                    D3DFMT_F16,
#endif // UNDER_XBOX
                };

    if (g_fmtdDefault != D3DFMT_UNKNOWN) {
        hr = pd3d->CheckDeviceFormat(uAdapter, d3ddt, d3dfmtMode, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, g_fmtdDefault);
        if (FAILED(hr)) {
            if (hr != D3DERR_NOTAVAILABLE) {
                ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormats"));
            }
        }
        else {
            return g_fmtdDefault;
        }
    }

    for (i = 0; i < 7; i++) {
        hr = pd3d->CheckDeviceFormat(uAdapter, d3ddt, d3dfmtMode, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, d3dfmt[i]);
        if (FAILED(hr)) {
            if (hr != D3DERR_NOTAVAILABLE) {
                ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormats"));
            }
        }
        else {
            return d3dfmt[i];
        }
    }

    return D3DFMT_UNKNOWN;
}

//******************************************************************************
static D3DMULTISAMPLE_TYPE SelectMultiSampleType(LPDIRECT3D8 pd3d, UINT uAdapter, 
                                D3DDEVTYPE d3ddt, D3DFORMAT d3dfmtMode,
                                BOOL bWindowed)
{
    UINT    i;
    HRESULT hr;

    for (i = 16; i > 0; i--) {

        hr = pd3d->CheckDeviceMultiSampleType(uAdapter, d3ddt, d3dfmtMode, bWindowed, (D3DMULTISAMPLE_TYPE)i);
        if (FAILED(hr)) {
            if (hr != D3DERR_NOTAVAILABLE) {
                ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceMultiSampleTypes"));
            }
        }
        else {
            return (D3DMULTISAMPLE_TYPE)i;
        }
    }

    return D3DMULTISAMPLE_NONE;
}

DWORD FormatToBitDepth(D3DFORMAT d3dfmt) {

    switch (d3dfmt) {
        case D3DFMT_R3G3B2:
        case D3DFMT_A8:
        case D3DFMT_P8:
        case D3DFMT_L8:
        case D3DFMT_A4L4:
            return 8;
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_X4R4G4B4:
        case D3DFMT_A8P8:
        case D3DFMT_A8L8:
        case D3DFMT_V8U8:
        case D3DFMT_L6V5U5:
#ifndef UNDER_XBOX
        case D3DFMT_D16_LOCKABLE:
#endif // !UNDER_XBOX
        case D3DFMT_D16:
        case D3DFMT_D15S1:
            return 16;
        case D3DFMT_R8G8B8:
            return 24;
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
#ifndef UNDER_XBOX
        case D3DFMT_X8L8V8U8:
#endif // !UNDER_XBOX
        case D3DFMT_Q8W8V8U8:
        case D3DFMT_V16U16:
        case D3DFMT_W11V11U10:
        case D3DFMT_D32:
        case D3DFMT_D24S8:
#ifndef UNDER_XBOX
        case D3DFMT_D24X8:
#endif // !UNDER_XBOX
        case D3DFMT_D24X4S4:
            return 32;
        default:
            return 0;
    }
}

} // namespace DXCONIO
