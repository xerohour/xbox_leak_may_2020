/*
 * Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 * (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: d3dContext.cpp                                                    *
*   The Direct3D context management routines.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       01/02/97 - re-wrote and cleaned up.     *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#ifdef PROFILE_INDEX
void nvCreateIndexProfile();
void nvDestroyIndexProfile();
#endif //PROFILE_INDEX

#ifdef NV_DISASM
HANDLE nvPusherFile               = 0;
BOOL   nvPusherDisassemblerEnable = 0;
#endif

#ifdef DEBUG_SURFACE_PLACEMENT
extern CSurfaceDebug csd;
#endif

//-------------------------------------------------------------------------

/*
 * Miscellaneous D3D context related routines.
 */
BOOL nvCreateContextListHeap
(
    GLOBALDATA *pDriverData
)
{
    dbgTracePush ("nvCreateContextListHeap");
    dbgTracePop();
    return (TRUE);
}

//-------------------------------------------------------------------------

BOOL nvDestroyContextListHeap
(
    void
)
{
    PNVD3DCONTEXT   pContext;
    PNVD3DCONTEXT   pContextNext;

    dbgTracePush ("nvDestroyContextListHeap");

    /*
     * Free any contexts that are still allocated.
     */
    pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext)
    {
        pContextNext = pContext->pContextNext;
        FreeIPM(pContext);
        getDC()->dwD3DContextCount--;
        pContext = pContextNext;
    }

    dbgTracePop();
    return (TRUE);
}

/*
 * --------------------------------------------------------------------------
 * nvDestroyPreapprovedContexts
 *
 *    This function destroys all preapproved contexts that had been created
 *    and never migrated to the natural ones.
 * --------------------------------------------------------------------------
 */
void nvDestroyPreapprovedContexts(void)
{
    PNVD3DCONTEXT pPreapprovedContext, pPreapprovedContextNext;
    pPreapprovedContext = (PNVD3DCONTEXT)getDC()->dwPreapprovedContextListHead;
    getDC()->dwPreapprovedContextListHead = 0;
    while (pPreapprovedContext)
    {
        pPreapprovedContextNext = (PNVD3DCONTEXT)pPreapprovedContext->pContextNext;
        FreeIPM(pPreapprovedContext);
        pPreapprovedContext = pPreapprovedContextNext;
    }
}

/*
 * --------------------------------------------------------------------------
 * nvFindPreapprovedContext
 *
 *    This function searches the getDC()->dwPreapprovedContextListHead
 *    list of preapproved PNVD3DCONTEXT structures by the process ID.
 * --------------------------------------------------------------------------
 */
PNVD3DCONTEXT nvFindPreapprovedContext(DWORD dwPID, BOOL bRemove)
{
    PNVD3DCONTEXT pPreapprovedContext, pPreapprovedContextPrev;
    pPreapprovedContextPrev = NULL;
    pPreapprovedContext = (PNVD3DCONTEXT)getDC()->dwPreapprovedContextListHead;
    while (pPreapprovedContext)
    {
        if(pPreapprovedContext->pid == dwPID)
        {
            if (bRemove)
            {
                if (pPreapprovedContextPrev)
                    pPreapprovedContextPrev->pContextNext = pPreapprovedContext->pContextNext;
                else
                    getDC()->dwPreapprovedContextListHead = pPreapprovedContext->pContextNext;
            }
            return pPreapprovedContext;
        }
        pPreapprovedContextPrev = pPreapprovedContext;
        pPreapprovedContext = (PNVD3DCONTEXT)pPreapprovedContext->pContextNext;
    }
    return NULL;
}

/*
 * --------------------------------------------------------------------------
 * nvGetPreapprovedContext
 *
 *    This function returns a pointer to a PNVD3DCONTEXT structure
 *    which is used for saving application belongings before the natural
 *    call to the nvContextCreate has been made. The nvContextCreate must
 *    inherit all significant information being kept here. First the function
 *    looks for the PreapprovedContext in the existing list and that failed it
 *    allocates a new structure.
 * --------------------------------------------------------------------------
 */
PNVD3DCONTEXT nvGetPreapprovedContext(void)
{
    PNVD3DCONTEXT pPreapprovedContext;
    DWORD         dwPID = GetCurrentProcessId();
    if (pPreapprovedContext = nvFindPreapprovedContext(dwPID, FALSE))
        return pPreapprovedContext;
    pPreapprovedContext = (PNVD3DCONTEXT)AllocIPM(sizeof(NVD3DCONTEXT));
    if (!pPreapprovedContext)
        return NULL;
    memset(pPreapprovedContext, 0, sizeof(NVD3DCONTEXT));

    // Set the process ID for the context.
    pPreapprovedContext->pid = dwPID;
#ifdef TEX_MANAGE
    // initialize texture manager
    nvTexManageInit (pPreapprovedContext);
#endif

    /*
     * Update the context list.
     */
    pPreapprovedContext->pContextPrev = (PNVD3DCONTEXT)NULL;
    pPreapprovedContext->pContextNext = (PNVD3DCONTEXT)getDC()->dwPreapprovedContextListHead;
    if (pPreapprovedContext->pContextNext)
        pPreapprovedContext->pContextNext->pContextPrev = pPreapprovedContext;
    getDC()->dwPreapprovedContextListHead = pPreapprovedContext;
    return pPreapprovedContext;
}

/*
 * D3D HAL context callback routines
 */

/*
 * --------------------------------------------------------------------------
 * NvContextCreate
 *
 * LPD3DHAL_CONTEXTCREATEDATA pccd
 *    Pointer to a structure containing things including the current
 *    rendering surface, the current Z surface, and the DirectX object
 *    handle, etc.
 * --------------------------------------------------------------------------
 */

DWORD __stdcall nvContextCreate
(
    LPD3DHAL_CONTEXTCREATEDATA  pccd
)
{
    unsigned char              *pData = 0;
    HDDRVITEM                   hContext;
    LPDDRAWI_DDRAWSURFACE_INT   pDDS   = 0;
    LPDDRAWI_DDRAWSURFACE_INT   pDDSZ  = 0;
    LPDDRAWI_DDRAWSURFACE_LCL   pDDSLcl  = 0;
    LPDDRAWI_DDRAWSURFACE_GBL   pDDSGbl  = 0;
    LPDDRAWI_DDRAWSURFACE_LCL   pDDSLclZ = 0;
    LPDDRAWI_DIRECTDRAW_GBL     pDDGbl   = 0;

    dbgTracePush ("nvContextCreate");

    // analyzer markers
#ifdef NV_MARKER
    {
        extern DWORD dwMarkerCount;
        dwMarkerCount = 0;
    }
#endif

    // push buffer disassembler
#ifdef NV_DISASM
    {
        nvPusherFile = NvCreateFile("\\PushData.bin",GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
        if (nvPusherFile == INVALID_HANDLE_VALUE)
        {
            nvPusherFile = 0;
        }
    }
#endif

    // Validate the direct draw surface pointers.
#ifndef WINNT // Chas said.
    if (global.dwDXRuntimeVersion >= 0x0700)
    {
        // directX 7 or higher.
        if ((pDDSLcl = pccd->lpDDSLcl) == NULL) {
            pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }
    else
#endif // WINNT
    {
        // directX 6 or earlier and NT cause Chas said so.
        if ((pccd->lpDDSLcl == NULL))
        {
            pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
        if ((pDDSLcl = DDS_LCL(pccd->lpDDSLcl)) == NULL)
        {
            pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }
    if ((pDDSGbl = pDDSLcl->lpGbl ) == NULL)
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }
#ifndef WINNT
    if (global.dwDXRuntimeVersion >= 0x0700)
    {
        // directX 7 or higher.
        pDDSLclZ = pccd->lpDDSZLcl;
    }
    else
#endif // !WINNT
    {
        // directX 6 or earlier
        if ((pccd->lpDDSZLcl != NULL)
            && ((pDDSLclZ = DDS_LCL(pccd->lpDDSZLcl)) == NULL)) {
            pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    if (pDDSLclZ && (pDDSLclZ->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY))
    {
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // Get pointer to global driver data structure.
    if (global.dwDXRuntimeVersion >= 0x0700) {
        pDDGbl = pccd->lpDDLcl->lpGbl;
    }
    else {
        pDDGbl = pccd->lpDDGbl;
    }
    nvSetDriverDataPtrFromDDGbl (pDDGbl);

#ifdef WINNT
    // on Win2K the registry is only read on a mode switch - read it here too so that
    // the settings can take immediate effect on windowed app too
    D3DReadRegistry (pDriverData);
#endif

    // load the logo if necessary
    nvLoadLogo();

    // connect to pm
#ifndef WINNT
    // ben BUGBUG philosophical ugliness
    DWORD dwID = (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0097_KELVIN) ?
                     0x020 :
                     (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) ?
                         0x010 :
                         0x004;
    pmConnect (pDXShare->dwHeadNumber,(U032)NvDeviceBaseGet(NV_DEV_BASE,pDriverData->dwDeviceIDNum), dwID);
#endif // !WINNT

    PNVD3DCONTEXT pContext;

    // Try to find the context among existing preapproved contexts already allocated
    pContext = nvFindPreapprovedContext (pccd->dwPID, TRUE);

    if (!pContext)
    {
        // Allocate a new context from the context array.
        hContext = (HDDRVITEM)AllocIPM(sizeof(NVD3DCONTEXT));

        // Get the pointer to the newly allocated context.
        pContext = (PNVD3DCONTEXT)hContext;
        if (!pContext)
        {
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL (NVDBG_LEVEL_ERROR, "nvContextCreate - Bad Context");
            pccd->ddrval = D3DHAL_OUTOFCONTEXTS;
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
        memset(pContext, 0, sizeof(NVD3DCONTEXT));

        // Set the process ID for the context.
        pContext->pid = pccd->dwPID;

#ifdef TEX_MANAGE
        // initialize texture manager
        nvTexManageInit (pContext);
#endif
    }
    else {
        nvAssert(0);
    }

    // Save off the applications DirectX version level.
    //      1 -> DX5 (?)
    //      2 -> DX6
    //      3 -> DX7
    //      4 -> DX8
    switch (pccd->dwhContext)
    {
        default: // pessimism
        case 0: pContext->dwDXAppVersion = 0x300;
                break;
        case 1: pContext->dwDXAppVersion = 0x500;
                break;
        case 2: pContext->dwDXAppVersion = 0x600;
                break;
        case 3: pContext->dwDXAppVersion = 0x700;
                break;
        case 4: pContext->dwDXAppVersion = 0x800;
                break;
    }

    // Return the handle of the new context.
    pccd->dwhContext = hContext;

    // cache off a pointer to pDXShare & pDriverData
#ifdef WINNT
    pContext->ppdev    = ppdev;
#else // !WINNT
    pContext->pDXShare = pDXShare;
#endif // !WINNT
    pContext->pDriverData = pDriverData;

    // initialize the surface specific context data
    nvAssert (pDDSLcl);   // we must have a render target
    CNvObject *pNvObj = GET_PNVOBJ (pDDSLcl);
    nvAssert (IS_VALID_PNVOBJ(pNvObj));     // we better have gotten a CreateSurface32 call for this already
    if( !IS_VALID_PNVOBJ(pNvObj) ) {
            DPF_LEVEL (NVDBG_LEVEL_ERROR, "nvContextCreate - Bad Render Target");
            FreeIPM(pContext);
            pccd->dwhContext = 0;
            pccd->ddrval = DDERR_SURFACELOST;
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
    }
    if (global.dwDXRuntimeVersion < 0x0700) {
        // nvobject pointers may have been swapped around amongst ddslcls since this
        // surface was created. make sure this nvobject's handle still points to this ddslcl.
        pNvObj->setHandle ((DWORD)pDDSLcl);
    }
    pContext->pRenderTarget = pNvObj->getSimpleSurface();
    pContext->dwRTHandle = pNvObj->getHandle();

    // Set up the default viewport
    nvSetSurfaceViewport (pContext);

    dbgSetFrontRenderTarget (pContext, pContext->pRenderTarget);

    // check for a z-buffer
    if (pDDSLclZ) {
        pNvObj = GET_PNVOBJ (pDDSLclZ);
        nvAssert (IS_VALID_PNVOBJ(pNvObj));     // we better have gotten a CreateSurface32 call for this already
        if (global.dwDXRuntimeVersion < 0x0700) {
            pNvObj->setHandle ((DWORD)pDDSLclZ);
        }
        pContext->pZetaBuffer = pNvObj->getSimpleSurface();
        pContext->dwZBHandle = pNvObj->getHandle();
    }
    else {
        pContext->pZetaBuffer = NULL;
        pContext->dwZBHandle  = 0;
    }

    // now that both surfaces are set, check that we can actually render with this combination.
    if (!nvCheckBufferCompatibility (pContext->pRenderTarget, pContext->pZetaBuffer,
                                     pDDSLclZ, pContext->dwDXAppVersion) ||
        !CKelvinAAState::CheckBufferMemoryConfig(pContext)) {
        FreeIPM(pContext);
        pccd->dwhContext = 0;
        pccd->ddrval = DDERR_CURRENTLYNOTAVAIL;
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

#ifdef WINNT
    /*
     * Winnt only: save a ptr to the DX driver state associated with
     * this context.
     */
    pContext->lpDD = pccd->lpDDLcl->lpGbl;
#endif // #ifdef WINNT

    // save the DIRECTDRAW_LCL and GBL associated with this context
    if (global.dwDXRuntimeVersion >= 0x0700) {
        pContext->dwDDLclID = (DWORD)(pccd->lpDDLcl);
        nvAssert (pDDGbl == pccd->lpDDLcl->lpGbl);  // there are two ways to access this. make sure they're ==
    }
    else {
        pContext->dwDDLclID = 0;     // this isn't available before DX7
        nvAssert (pDDGbl == pccd->lpDDGbl);  // there are two ways to access this. make sure they're ==
    }
    pContext->dwDDGblID = (DWORD)pDDGbl;

    /*
     * Do the hardware specific context setup.
     */
    if (!nvSetupContext(pContext))
    {
        /*
         * When this call fails, it means there was some sort of problem with the
         * surface format.  Either an unsupported pixel format was specified or
         * an unsupported pixel/zeta format was specified.
         * Free the context memory and return an error.
         */
        FreeIPM(pContext);
        NvReleaseSemaphore(pDriverData);
        pccd->dwhContext = 0;
        pContext  = 0;
        pccd->ddrval = DDERR_INVALIDPIXELFORMAT;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // zero out state set code
    pContext->pStateSets           = NULL;
    pContext->dwMaxStateSetHandles = 0;
    pContext->dwCurrentStateSet    = 0;

    // Update the open context count.
    getDC()->dwD3DContextCount++;

    // Update the context list.
    pContext->pContextPrev = (PNVD3DCONTEXT)NULL;
    pContext->pContextNext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    if (pContext->pContextNext) {
        pContext->pContextNext->pContextPrev = pContext;
    }
    getDC()->dwContextListHead = pContext;

    pContext->dwTickLastUsed = GetTickCount();

    pContext->bClearLogo = FALSE;

    // Initialize vxd frame count for statistics driver
    DDFLIPINIT();

    nvInitTLData (pContext);
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvInitKelvinData (pContext);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvInitCelsiusData (pContext);
    }
#endif

    // Create a default vertex shader
    pContext->pCurrentVShader = new CVertexShader;
    nvAssert(pContext->pCurrentVShader != NULL);
    CNvObject *pObj = new CNvObject(pContext->pid);
    nvAssert(pObj != NULL);
    pObj->setObject (CNvObject::NVOBJ_VERTEXSHADER, pContext->pCurrentVShader);

    // add this handle 0 object to the global list of objects
    // this is our default
    PNV_OBJECT_LIST pNvList = nvFindObjectList(pContext->dwDDLclID, &global.pNvVShaderLists);
    nvAddObjectToList(pNvList, pObj, 0);

    if (pContext->dwDXAppVersion >= 0x0800) {
        pContext->pVShaderConsts = new VertexShaderConsts;
        memset(pContext->pVShaderConsts, 0, sizeof(VertexShaderConsts));
    } else {
        pContext->pVShaderConsts = NULL;
    }

    // stateset schtuff
    // initially put things in 'set' (rather than 'record') mode
    pContext->pDP2FunctionTable = &nvDP2SetFuncs;

    // We clearly haven't seen any triangles yet =)
    pContext->bSeenTriangles = FALSE;

    // Initialize view and world matrix to unit matrix to avoid a mess in supertri if we try to set hwstate
    // before we get a chance to initialize the matrices (usually happens when kelvin AA is enabled)
    pContext->xfmView._11 = 1.0;
    pContext->xfmView._22 = 1.0;
    pContext->xfmView._33 = 1.0;
    pContext->xfmView._44 = 1.0;

    pContext->xfmWorld[0]._11 = 1.0;
    pContext->xfmWorld[0]._22 = 1.0;
    pContext->xfmWorld[0]._33 = 1.0;
    pContext->xfmWorld[0]._44 = 1.0;


#ifdef NV_PROFILE
    // Start up profiler
    nvpCreate();
#endif //NV_PROFILE

#ifdef PROFILE_INDEX
    nvCreateIndexProfile();
#endif // PROFILE_INDEX
#ifdef DEBUG
#ifdef HAVE_NV_FILE_FACILITY
    createDPFLog();
#endif
#endif
#ifdef NV_MARKER
    *(DWORD*)(VIDMEM_ADDR(pDriverData->BaseAddress)) = 0xcccccccc;
#endif

    // make sure we have a valid texture heap
    nvAssert (getDC()->nvD3DTexHeapData.dwHandle);  // delete after sept00

#ifdef TEX_MANAGE_TEST
    // chew up some memory
    {
        DWORD dwAddr, dwStatus, dwSize;
        //dwSize = 24 * (1<<20);  // 24 MB
        dwSize = 0x01a34000;  // 2 of 3 allocations in multitexture SDK app will succeed (1280x1024x32)
        //dwSize = 0x01a36000;  // 1 of 3 allocations in multitexture SDK app will succeed (1280x1024x32)
        //dwSize = 0x01a38000;  // 0 of 3 allocations in multitexture SDK app will succeed (1280x1024x32)
        NVHEAP_ALLOC (dwStatus, dwAddr, dwSize, TYPE_TEXTURE);
        nvAssert (dwStatus == 0);  // allocation successful
    }
#endif

#ifdef  STEREO_SUPPORT
    StereoContextCreate();
#endif  //STEREO_SUPPORT

#if (NVARCH >= 0x010)
    // Set up a few defaults
    pContext->aa.fWidthAmplifier  = 1.0f;
    pContext->aa.fHeightAmplifier = 1.0f;
    pContext->aa.dwDepthAmplifier = 1;
    pContext->aa.fLODBias         = 0.0f;
    pContext->aa.dwLinePointSize  = 0x8; // 1.0 in 6.3 fmt
#endif

    if (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_CT_11M)
    {
        NV_CFGEX_GET_FLATPANEL_INFO_MULTI_PARAMS panel;
        NV_CFGEX_GET_MONITOR_INFO_MULTI_PARAMS monitor0, monitor1;
        NV_CFGEX_VBLANK_ENABLED_PARAMS mon0vb;
        NV_CFGEX_VBLANK_ENABLED_PARAMS pan0vb;
        DWORD dwSWidth, dwSHeight;
        NvRmConfigGet (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFG_SCREEN_WIDTH,  &dwSWidth);
        NvRmConfigGet (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFG_SCREEN_HEIGHT, &dwSHeight);
        panel.Head =0;
        NvRmConfigGetEx (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI, &panel,sizeof(panel) );
        if ((panel.Head == 0) && (!panel.FlatPanelConnected) )
        {

            panel.Head =1;
            NvRmConfigGetEx (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_GET_FLAT_PANEL_INFO_MULTI, &panel,sizeof(panel) );
        }

        if (panel.FlatPanelConnected)
        {
            pan0vb.Head = panel.Head;
            NvRmConfigGetEx (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_VBLANK_ENABLED, &pan0vb,sizeof(pan0vb));

        }

        monitor0.Head = 0;
        NvRmConfigGetEx (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_GET_MONITOR_INFO_MULTI, &monitor0,sizeof(monitor0));

        if (monitor0.MonitorConnected)
        {
            mon0vb.Head = 0;
            NvRmConfigGetEx (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_VBLANK_ENABLED, &mon0vb,sizeof(mon0vb));
        }
        else
        {
            monitor1.Head = 1;
            NvRmConfigGetEx (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_GET_MONITOR_INFO_MULTI, &monitor1,sizeof(monitor1));
            if (monitor0.MonitorConnected)
            {
                mon0vb.Head = 0;
                NvRmConfigGetEx (pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_VBLANK_ENABLED, &mon0vb,sizeof(mon0vb));
            }
        }



        pContext->bScaledFlatPanel = false;
        if (panel.FlatPanelConnected && (dwSWidth > 640) && (dwSHeight > 480) && (panel.FlatPanelMode == 0) && (dwSWidth < panel.FlatPanelSizeX) && (dwSHeight < panel.FlatPanelSizeY))
            if (!mon0vb.Enabled && pan0vb.Enabled)
            {
                pContext->bScaledFlatPanel = true;
            }

    }

    /*
     * Context creation complete, return successfully.
     */
    NvReleaseSemaphore(pDriverData);
    pccd->ddrval = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//-------------------------------------------------------------------------

void nvFreeContext (PNVD3DCONTEXT pContext)
{
    DWORD i;

    dbgTracePush ("nvFreeContext");

    // free the lights
    delete [](pContext->pLightArray);

    // free palettes - MS provides no formal mechanism for freeing palettes
    // a potential problem exists if the D3D context is destroyed but not the DDRAW context.
    // the video memory associated with the palette may be released and the pointer in our palette
    // object will point to memory we no longer own. Release the palette memory here to prevent this.
    // nvDP2UpdatePalette will reallocate the video memory if required.
    PNV_OBJECT_LIST pNvPaletteList = nvFindObjectList(pContext->dwDDLclID, &global.pNvPaletteLists);
    for(i = 0; i < pNvPaletteList->dwListSize; i ++) {
        CNvObject *pNvObj = nvGetObjectFromHandle(pContext->dwDDLclID, i, &global.pNvPaletteLists);
        if (pNvObj) {
            // free the video memory associated with this palette
            pNvObj->getPaletteSurface()->destroy();
        }
    }

    // free state sets
    if (pContext->pStateSets)
    {
        for (i = 0; i < pContext->dwMaxStateSetHandles; i++) {
            stateSetDelete (pContext, i);
        }

        FreeIPM (pContext->pStateSets);
    }
    pContext->pStateSets           = NULL;
    pContext->dwMaxStateSetHandles = 0;

    // only free the global data when the last context is gone
    getDC()->dwD3DContextCount --;
    if (getDC()->dwD3DContextCount == 0)
    {
#if (NVARCH >= 0x010)
        // free the celsius combiner programs
        PCELSIUSCOMBINERPROGRAM pProgram, pNext;
        for (i = 0; i < CELSIUSCPTABLE_SIZE; i++) {
            pProgram = global.celsiusCombinerProgramTable[i];
            while (pProgram) {
                pNext = pProgram->pNext;
                FreeIPM (pProgram);
                pProgram = pNext;
            }
            global.celsiusCombinerProgramTable[i] = NULL;
        }
#endif  // NVARCH >= 0x010
    }

    // remove context from context list
    if (pContext->pContextPrev) {
        pContext->pContextPrev->pContextNext = pContext->pContextNext;
    }
    if (pContext->pContextNext) {
        pContext->pContextNext->pContextPrev = pContext->pContextPrev;
    }
    if (getDC()->dwContextListHead == pContext) {
        getDC()->dwContextListHead = pContext->pContextNext;
    }

    if (pContext->pVShaderConsts) {
        delete pContext->pVShaderConsts;
        pContext->pVShaderConsts = NULL;
    }

    FreeIPM(pContext);

    dbgTracePop();
}

//-------------------------------------------------------------------------

/*
 * nvContextDestroy
 *
 * d3d callback to destroy the given context and all association with it
 */

DWORD __stdcall nvContextDestroy
(
    LPD3DHAL_CONTEXTDESTROYDATA pcdd
)
{
    dbgTracePush ("nvContextDestroy");

    // Get pointer to context being destroyed.
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)pcdd->dwhContext;
    if (!pContext)
    {
        DPF_LEVEL (NVDBG_LEVEL_ERROR, "nvContextDestroy - Bad Context");
        pcdd->ddrval = D3DHAL_CONTEXT_BAD;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // Get pointer to global driver data structure
    nvSetDriverDataPtrFromContext(pContext);

#ifdef DEBUG_SURFACE_PLACEMENT
    csd.Destroy();
#endif

    if (getDC()->dwD3DContextCount == 0)
    {
        pcdd->ddrval = DD_OK;
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

#ifdef  STEREO_SUPPORT
    StereoContextDestroy(pContext);
#endif  //STEREO_SUPPORT

#ifdef NV_MARKER
    *(DWORD*)(VIDMEM_ADDR(pDriverData->BaseAddress)) = 0xdddddddd;
#endif

    // push buffer disassembler
#ifdef NV_DISASM
    {
        if (nvPusherFile)
        {
            NvCloseHandle(nvPusherFile);
            nvPusherFile = 0;
        }
    }
#endif

     // disconnect from pm
     pmDisconnect();

#ifdef NV_PROFILE
    // Shut down profiler
    nvpDestroy();
#endif //NV_PROFILE

#ifdef PROFILE_INDEX
    // Shut down profiler
    nvDestroyIndexProfile();
#endif //PROFILE_INDEX

    // Perform any cleanup required.

    // kill AA
#if (NVARCH >= 0x010)
    nvCelsiusAADestroy (pContext);
#endif  // NVARCH >= 0x010

#if (NVARCH >= 0x020)
    pContext->kelvinAA.Destroy();
#endif  // NVARCH >= 0x020

#ifdef TEX_MANAGE
    nvTexManageEvictAll (pContext);
#endif

    nvFreeContext (pContext);

    // Reset the global context and texture handles.
    pDriverData->dwCurrentContextHandle = 0;

    // Return successfully.
    pcdd->ddrval = DD_OK;
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
#ifdef DEBUG
#ifdef HAVE_NV_FILE_FACILITY
    closeDPFLog();
#endif
#endif
    return (DDHAL_DRIVER_HANDLED);
}

//-------------------------------------------------------------------------

// This callback is invoked when a process dies.

DWORD __stdcall nvContextDestroyAll
(
    LPD3DHAL_CONTEXTDESTROYALLDATA  pcdd
)
{
    dbgTracePush ("nvContextDestroyAll");

    // This routine is legacy and will only ever be called in non-mulitmonitor
    // situations, so it's ok to trust what has been previously loaded into
    // pDriverData.

    if (getDC()->dwD3DContextCount == 0)
    {
        pcdd->ddrval = DD_OK;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // iterate through all contexts and destroy the ones with this PID
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    while (pContext)
    {
        PNVD3DCONTEXT pNext = pContext->pContextNext;

        if ((pcdd->dwPID == ~0) || (pContext->pid == pcdd->dwPID))
        {
            D3DHAL_CONTEXTDESTROYDATA pcdd;
            pcdd.dwhContext = (ULONG_PTR) pContext;
            nvContextDestroy (&pcdd);
        }

        pContext = pNext;
    }

    // Destroy all objects associated with this PID.
    // For our own purposes we use a PID value of ~0 to indicate that we want to destroy all PIDs
    for (CNvObject *pNvObj = global.pNvObjectHead; pNvObj;)
    {
        // get next
        CNvObject *pNext = pNvObj->getNext();

        // should we free it?
        if (((pcdd->dwPID == ~0) || (pNvObj->getPID() == pcdd->dwPID))
#ifndef WINNT
            && (pNvObj != (CNvObject*)pDXShare->pNvPrimarySurf)
#endif
           )
        {
            CCommandBuffer *pCommandBuffer = pNvObj->getCommandBuffer();
            if (pCommandBuffer)
            {
                SET_PNVOBJ (pCommandBuffer->getDDSurfaceLcl(), NULL);
            }
            // cleanup is handled in nvObject release
            pNvObj->release();
        }
        pNvObj = pNext;
    }

    // Reset the global context and texture handles.
    pDriverData->dwCurrentContextHandle = 0;

    pcdd->ddrval = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//-------------------------------------------------------------------------

DWORD __stdcall nvDestroyDDLocal
(
    LPDDHAL_DESTROYDDLOCALDATA pdddld
)
{
    nvSetDriverDataPtrFromDDGbl (pdddld->pDDLcl->lpGbl);

    dbgTracePush ("nvDestroyDDLocal");
    nvAssert (pdddld->pDDLcl != NULL);

    for (CNvObject *pNvObj = global.pNvObjectHead; pNvObj;)
    {
        CNvObject *pNext = pNvObj->getNext();

        // does it belong to this context?
        if (pNvObj->getDDLclID() == (DWORD)pdddld->pDDLcl)
        {
            switch(pNvObj->getClass()) {
            // release these object types
            case CNvObject::NVOBJ_VERTEXSHADER:
            case CNvObject::NVOBJ_CACHEDPATCH:
            case CNvObject::NVOBJ_PALETTE:
            case CNvObject::NVOBJ_PIXELSHADER:
                pNvObj->release();
                break;
            // ignore these
            case CNvObject::NVOBJ_TEXTURE:
            case CNvObject::NVOBJ_VERTEXBUFFER:
            case CNvObject::NVOBJ_COMMANDBUFFER:
            case CNvObject::NVOBJ_SIMPLESURFACE:
                // nvAssert(0);  -- DCT can get here legitimately
                break;
            // unknown object type
            default:
                break;
            }
        }
        pNvObj = pNext;
    }

    nvDeleteObjectList ((DWORD)pdddld->pDDLcl, &global.pNvSurfaceLists);
    nvDeleteObjectList ((DWORD)pdddld->pDDLcl, &global.pNvPaletteLists);
    nvDeleteObjectList ((DWORD)pdddld->pDDLcl, &global.pNvVShaderLists);
    nvDeleteObjectList ((DWORD)pdddld->pDDLcl, &global.pNvPShaderLists);
    nvDeleteObjectList ((DWORD)pdddld->pDDLcl, &global.pNvPatchLists);
    
#ifdef WINNT
    // display heap usage
    g_nvIPHeap.checkHeapUsage();
#endif

    NvReleaseSemaphore(pDriverData);
    pdddld->ddRVal = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

#endif  // NVARCH >= 0x04
