/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    enum.cpp

Author:

    Matt Bronder

Description:

    Direct3D initialization routines.

*******************************************************************************/

#include "d3dbase.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif // UNDER_XBOX

#define D3DDEVTYPE_NULL ((D3DDEVTYPE)0)

//******************************************************************************
// Local function prototypes
//******************************************************************************

static DWORD            SelectVertexPipeline(D3DCAPS8* pd3dcaps);
static UINT             SelectPresentationInterval(D3DCAPS8* pd3dcaps);
static D3DFORMAT        SelectDepthStencilFormat(LPDIRECT3D8 pDirect3D, 
                            UINT uAdapter, D3DDEVTYPE d3ddt, 
                            D3DFORMAT d3dfmtMode);
static
D3DMULTISAMPLE_TYPE     SelectMultiSampleType(LPDIRECT3D8 pDirect3D, 
                            UINT uAdapter, D3DDEVTYPE d3ddt, 
                            D3DFORMAT d3dfmtMode, BOOL bWindowed);

//******************************************************************************
// Globals
//******************************************************************************

ADAPTERDESC             g_adpdDefault = {
                            (UINT)(-1), {0}, D3DFMT_UNKNOWN, NULL, NULL
                        };
DEVICEDESC              g_devdDefault = {
                            D3DDEVTYPE_NULL, {D3DDEVTYPE_NULL, 0}, 
                            D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                            D3DPRESENT_INTERVAL_DEFAULT, TRUE, D3DFMT_UNKNOWN, 
                            D3DMULTISAMPLE_NONE, NULL, NULL
                        };
DISPLAYMODE             g_dmDefault = {
                            {0}, D3DFMT_UNKNOWN, D3DMULTISAMPLE_NONE, NULL
                        };
D3DFORMAT               g_fmtdDefault = D3DFMT_UNKNOWN;
#ifndef UNDER_XBOX
D3DMULTISAMPLE_TYPE     g_mstDefault = D3DMULTISAMPLE_4_SAMPLES;
#else
D3DMULTISAMPLE_TYPE     g_mstDefault = D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR;
#endif

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
//     LPDIRECT3D8 pDirect3D        - Pointer to the Direct3D object
//
// Return Value:
//
//     A pointer to the head of the display adapter list (NULL if no adapters
//     are supported).
//
//******************************************************************************
PADAPTERDESC EnumDisplays(LPDIRECT3D8 pDirect3D) {

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

    if (!pDirect3D) {
        return NULL;
    }

    // Enumerate adapters
    uNumAdapters = pDirect3D->GetAdapterCount();

    for (uAdapter = 0; uAdapter < pDirect3D->GetAdapterCount(); uAdapter++) {

        // Allocate memory for the adapter information
        padpdNode = (PADAPTERDESC)MemAlloc(sizeof(ADAPTERDESC));
        if (!padpdNode) {
            DebugString(TEXT("ERROR: Not enough memory available to store adapter information"));
            break;
        }

        // Save the adapter number
        padpdNode->uAdapter = uAdapter;

        // Save the adapter idenifier
        hr = pDirect3D->GetAdapterIdentifier(uAdapter, 0, &padpdNode->d3dai);
        if (ResultFailed(hr, TEXT("IDirect3D8::GetDeviceIdentifier"))) {
            MemFree(padpdNode);
            continue;
        }

        // Save the initial display mode of the adapter (for use in windowed applications)
        hr = pDirect3D->GetAdapterDisplayMode(uAdapter, &padpdNode->d3ddmWindowed);
        if (ResultFailed(hr, TEXT("IDirect3D8::GetAdapterDisplayMode"))) {
            MemFree(padpdNode);
            continue;
        }

        padpdNode->pdevdList = NULL;

        // Enumerate devices
#ifndef DEMO_HACK
        for (uDevice = 0; uDevice < 3; uDevice++) {
#else
        for (uDevice = 0; !uDevice; uDevice++) {
#endif

            // Allocate memory for the device information
            pdevdNode = (PDEVICEDESC)MemAlloc(sizeof(DEVICEDESC));
            if (!pdevdNode) {
                DebugString(TEXT("ERROR: Not enough memory available to store device information"));
                break;
            }

            // Save the device type
            pdevdNode->d3ddt = d3ddt[uDevice];
            
            // Save the caps
            hr = pDirect3D->GetDeviceCaps(uAdapter, d3ddt[uDevice], &pdevdNode->d3dcaps);
            if (ResultFailed(hr, TEXT("IDirect3D8::GetDeviceCaps"))) {
                MemFree(pdevdNode);
                continue;
            }

            // The device supports windowed mode if the caps exposes it and
            // the device can render to the initial display mode of the adapter
#ifndef DEMO_HACK
            hr = pDirect3D->CheckDeviceType(uAdapter, d3ddt[uDevice], padpdNode->d3ddmWindowed.Format, padpdNode->d3ddmWindowed.Format, TRUE);
#else
            hr = D3DERR_NOTAVAILABLE;
#endif // DEMO_HACK
            pdevdNode->bCanRenderWindowed = ((pdevdNode->d3dcaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) && SUCCEEDED(hr));

            // Select where vertex processing is to be performed
            pdevdNode->dwBehavior = SelectVertexPipeline(&pdevdNode->d3dcaps);

            // Select the presentation swap interval
            pdevdNode->uPresentInterval = SelectPresentationInterval(&pdevdNode->d3dcaps);

            if (SUCCEEDED(hr)) {

                // Select a depth/stencil buffer format for a windowed device
                pdevdNode->fmtdWindowed = SelectDepthStencilFormat(pDirect3D, uAdapter, d3ddt[uDevice], padpdNode->d3ddmWindowed.Format);

                // Select a multisample type for a windowed device
                pdevdNode->d3dmstWindowed = SelectMultiSampleType(pDirect3D, uAdapter, d3ddt[uDevice], padpdNode->d3ddmWindowed.Format, TRUE);
            }

            // Save the display modes supported by the adapter, device, and application
            uNumModes = pDirect3D->GetAdapterModeCount(uAdapter);
            pdevdNode->pdmList = NULL;

            for (uMode = 0; uMode < uNumModes; uMode++) {

                hr = pDirect3D->EnumAdapterModes(uAdapter, uMode, &d3ddm);
                if (ResultFailed(hr, TEXT("IDirect3D8::EnumAdapterModes"))) {
                    continue;
                }

                // Select a depth/stencil buffer format for the full screen display mode
                fmt = SelectDepthStencilFormat(pDirect3D, uAdapter, d3ddt[uDevice], d3ddm.Format);

                // Select a multisample type for the full screen display mode
                d3dmst = SelectMultiSampleType(pDirect3D, uAdapter, d3ddt[uDevice], d3ddm.Format, FALSE);

                // Check if the device supports the display mode format
                hr = pDirect3D->CheckDeviceType(uAdapter, d3ddt[uDevice], d3ddm.Format, d3ddm.Format, FALSE);
                if (FAILED(hr)) {
#ifndef DEMO_HACK
                    if (hr != D3DERR_NOTAVAILABLE) {
                        ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceType"));
                    }
#endif
                    continue;
                }

                // Check if the application supports the display mode format
                if (g_pfnValidate && !g_pfnValidate(pDirect3D, &pdevdNode->d3dcaps, &d3ddm)) {
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
//
// Function:
//
//     InitCheckDisplayProc
//
// Description:
//
//     Set a validation function which will be used during enumeration to 
//     evaluate which display modes are acceptable to the application.
//
// Arguments:
//
//     CHECKDISPLAYPROC pfnValidate     - Validation function
//
// Return Value:
//
//     None.
//
//******************************************************************************
void InitCheckDisplayProc(CHECKDISPLAYPROC pfnValidate) {
    
    g_pfnValidate = pfnValidate;
}

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
//     PADAPTERDESC padpdList   - List of the available adapters
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
//                                device
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
//                                display mode
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
//
// Function:
//
//     SelectVertexPipeline
//
// Description:
//
//     Select what form(s) of vertex processing to use in a device.
//
// Arguments:
//
//     D3DCAPS8* pd3dcaps       - Capabilities of the device to evaluate
//
// Return Value:
//
//     A vertex processing behavior flag to be used in the creation of the 
//     device.
//
//******************************************************************************
static DWORD SelectVertexPipeline(D3DCAPS8* pd3dcaps) {

    if (g_devdDefault.dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING &&
        pd3dcaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        return g_devdDefault.dwBehavior;
    }
    else if (g_devdDefault.dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
        pd3dcaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        return g_devdDefault.dwBehavior;
    }
    else if (g_devdDefault.dwBehavior & D3DCREATE_PUREDEVICE &&
        pd3dcaps->DevCaps & D3DDEVCAPS_PUREDEVICE)
    {
        return g_devdDefault.dwBehavior | D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else if (g_devdDefault.dwBehavior == D3DCREATE_SOFTWARE_VERTEXPROCESSING) {
        return g_devdDefault.dwBehavior;
    }
    else if (pd3dcaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
        return D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else if (pd3dcaps->DevCaps & D3DDEVCAPS_PUREDEVICE) {
        return D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else {
        return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
}

//******************************************************************************
//
// Function:
//
//     SelectPresentationInterval
//
// Description:
//
//     Select what presentation swap interval to use in a device.
//
// Arguments:
//
//     D3DCAPS8* pd3dcaps       - Capabilities of the device to evaluate
//
// Return Value:
//
//     A presentation interval value to be used in the creation or reset of the 
//     device when it is placed in full screen mode.
//
//******************************************************************************
static UINT SelectPresentationInterval(D3DCAPS8* pd3dcaps) {

#ifdef DEMO_HACK
    switch (g_devdDefault.uPresentInterval) {
        case D3DPRESENT_INTERVAL_DEFAULT:
        case D3DPRESENT_INTERVAL_ONE:
        case D3DPRESENT_INTERVAL_IMMEDIATE:
            return g_devdDefault.uPresentInterval;
    }
#endif
    if (pd3dcaps->PresentationIntervals & g_devdDefault.uPresentInterval) {
        return g_devdDefault.uPresentInterval;
    }
    if (pd3dcaps->PresentationIntervals & D3DPRESENT_INTERVAL_ONE) {
        return D3DPRESENT_INTERVAL_ONE;
    }
    else if (pd3dcaps->PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) {
        return D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    else if (pd3dcaps->PresentationIntervals & D3DPRESENT_INTERVAL_TWO) {
        return D3DPRESENT_INTERVAL_TWO;
    }
    else if (pd3dcaps->PresentationIntervals & D3DPRESENT_INTERVAL_THREE) {
        return D3DPRESENT_INTERVAL_THREE;
    }
    else if (pd3dcaps->PresentationIntervals & D3DPRESENT_INTERVAL_FOUR) {
        return D3DPRESENT_INTERVAL_FOUR;
    }
    else {
        return D3DPRESENT_INTERVAL_IMMEDIATE;
    }
}

//******************************************************************************
//
// Function:
//
//     SelectDepthStencilFormat
//
// Description:
//
//     Select the default format of the depth/stencil buffer to be used for 
//     the given adapter, device, and display mode format.
//
// Arguments:
//
//     LPDIRECT3D8 pDirect3D    - Pointer to the Direct3D object
//
//     UINT uAdapter            - Ordinal of the adapter
//
//     D3DDEVTYPE d3ddt         - Device type
//
//     D3DFORMAT d3dfmtMode     - Display mode format
//
// Return Value:
//
//     The depth buffer format to use for the given adapter, device, and 
//     display mode format.
//
//******************************************************************************
static D3DFORMAT SelectDepthStencilFormat(LPDIRECT3D8 pDirect3D, 
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
        hr = pDirect3D->CheckDeviceFormat(uAdapter, d3ddt, d3dfmtMode, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, g_fmtdDefault);
        if (FAILED(hr)) {
            if (hr != D3DERR_NOTAVAILABLE) {
                ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormat"));
            }
        }
        else {
            hr = pDirect3D->CheckDepthStencilMatch(uAdapter, d3ddt, d3dfmtMode, d3dfmtMode, g_fmtdDefault);
            if (FAILED(hr)) {
                if (hr != D3DERR_NOTAVAILABLE) {
                    ResultFailed(hr, TEXT("IDirect3D8::CheckDepthStencilMatch"));
                }
            }
            else {
                return g_fmtdDefault;
            }
        }
    }

    for (i = 0; i < 7; i++) {
        hr = pDirect3D->CheckDeviceFormat(uAdapter, d3ddt, d3dfmtMode, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, d3dfmt[i]);
        if (FAILED(hr)) {
            if (hr != D3DERR_NOTAVAILABLE) {
                ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormat"));
            }
        }
        else {
            hr = pDirect3D->CheckDepthStencilMatch(uAdapter, d3ddt, d3dfmtMode, d3dfmtMode, d3dfmt[i]);
            if (FAILED(hr)) {
                if (hr != D3DERR_NOTAVAILABLE) {
                    ResultFailed(hr, TEXT("IDirect3D8::CheckDepthStencilMatch"));
                }
            }
            else {
                return d3dfmt[i];
            }
        }
    }

    return D3DFMT_UNKNOWN;
}

//******************************************************************************
//
// Function:
//
//     SelectMultiSampleType
//
// Description:
//
//     Select the default multi-sample type for the given adapter, device,
//     display mode format, and windowed mode.
//
// Arguments:
//
//     CDirect3D8 pDirect3D     - Pointer to the Direct3D object
//
//     UINT uAdapter            - Ordinal of the adapter
//
//     D3DDEVTYPE d3ddt         - Device type
//
//     D3DFORMAT d3dfmtMode     - Display mode format
//
//     BOOL bWindowed           - Windowed mode
//
// Return Value:
//
//     The multi-sample type to use for the given adapter, device, display mode
//     format, and windowed mode.
//
//******************************************************************************
static D3DMULTISAMPLE_TYPE SelectMultiSampleType(LPDIRECT3D8 pDirect3D, 
                                UINT uAdapter, D3DDEVTYPE d3ddt, 
                                D3DFORMAT d3dfmtMode, BOOL bWindowed)
{
    UINT                    i;
    HRESULT                 hr;
    D3DMULTISAMPLE_TYPE     mst[] = {
#ifndef UNDER_XBOX
                                D3DMULTISAMPLE_4_SAMPLES,
                                D3DMULTISAMPLE_2_SAMPLES,
                                D3DMULTISAMPLE_9_SAMPLES,
                                D3DMULTISAMPLE_12_SAMPLES,
                                D3DMULTISAMPLE_16_SAMPLES,
                                D3DMULTISAMPLE_3_SAMPLES,
                                D3DMULTISAMPLE_5_SAMPLES,
                                D3DMULTISAMPLE_6_SAMPLES,
                                D3DMULTISAMPLE_7_SAMPLES,
                                D3DMULTISAMPLE_8_SAMPLES,
                                D3DMULTISAMPLE_10_SAMPLES,
                                D3DMULTISAMPLE_11_SAMPLES,
                                D3DMULTISAMPLE_13_SAMPLES,
                                D3DMULTISAMPLE_14_SAMPLES,
                                D3DMULTISAMPLE_15_SAMPLES,
                                D3DMULTISAMPLE_NONE
#else
                                D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR,
                                D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN,
                                D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX,
                                D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR,
                                D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR,
                                D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR,
                                D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR,
                                D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN,
                                D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN,
                                D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN,
                                D3DMULTISAMPLE_NONE
#endif // UNDER_XBOX
    };

#ifdef DEMO_HACK
    return g_mstDefault;
#endif

    hr = pDirect3D->CheckDeviceMultiSampleType(uAdapter, d3ddt, d3dfmtMode, bWindowed, g_mstDefault);
    if (FAILED(hr)) {
        if (hr != D3DERR_NOTAVAILABLE) {
            ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceMultiSampleTypes"));
        }
    }
    else {
        return g_mstDefault;
    }

    for (i = 0; i < countof(mst); i++) {

        hr = pDirect3D->CheckDeviceMultiSampleType(uAdapter, d3ddt, d3dfmtMode, bWindowed, mst[i]);
        if (FAILED(hr)) {
            if (hr != D3DERR_NOTAVAILABLE) {
                ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceMultiSampleTypes"));
            }
        }
        else {
            return mst[i];
        }
    }

    return D3DMULTISAMPLE_NONE;
}

//******************************************************************************
//
// Function:
//
//     FormatToBitDepth
//
// Description:
//
//     Convert the given format to the number of bits needed to represent it.
//
// Arguments:
//
//     D3DFORMAT d3dfmt         - Resource format
//
// Return Value:
//
//     The number of bits required to represent the given format in memory.
//
//******************************************************************************
DWORD FormatToBitDepth(D3DFORMAT d3dfmt) {

    switch (d3dfmt) {
        case D3DFMT_R3G3B2:
        case D3DFMT_A8:
        case D3DFMT_P8:
        case D3DFMT_L8:
        case D3DFMT_A4L4:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A8:
        case D3DFMT_LIN_L8:
        case D3DFMT_LIN_AL8:
#endif // UNDER_XBOX
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
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A1R5G5B5:
        case D3DFMT_LIN_A4R4G4B4:
        case D3DFMT_LIN_G8B8:
        case D3DFMT_LIN_R4G4B4A4:
        case D3DFMT_LIN_R5G5B5A1:
        case D3DFMT_LIN_R5G6B5:
        case D3DFMT_LIN_R6G5B5:
        case D3DFMT_LIN_R8B8:
        case D3DFMT_LIN_X1R5G5B5:
        case D3DFMT_LIN_A8L8:
        case D3DFMT_LIN_L16:
        case D3DFMT_LIN_D16:
        case D3DFMT_LIN_F16:
#endif // UNDER_XBOX
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
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A8B8G8R8:
        case D3DFMT_LIN_A8R8G8B8:
        case D3DFMT_LIN_B8G8R8A8:
        case D3DFMT_LIN_R8G8B8A8:
        case D3DFMT_LIN_X8R8G8B8:
        case D3DFMT_LIN_D24S8:
        case D3DFMT_LIN_F24S8:
#endif // UNDER_XBOX
            return 32;
        default:
            return 0;
    }
}
