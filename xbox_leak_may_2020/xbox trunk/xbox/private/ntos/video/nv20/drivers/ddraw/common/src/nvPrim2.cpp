/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvPrim2.cpp                                                       *
*   NV4 DX6 DrawPrimitives2 routines.                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       04/18/98 - created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

PFND3DPARSEUNKNOWNCOMMAND fnD3DParseUnknownCommandCallback;

//-------------------------------------------------------------------------

DWORD __stdcall nvDrawPrimitives2
(
    LPD3DHAL_DRAWPRIMITIVES2DATA pdp2d
)
{
    PNVD3DCONTEXT       pContext;
    LPD3DHAL_DP2COMMAND lpCommands, lpResumeCommands;
    LPBYTE              lpCommandBufferStart;
    LPBYTE              lpCommandBufferEnd;
    BOOL                bVBFlushOnExit = FALSE;
    BOOL                bWriteOffset   = FALSE;
    BOOL                bSuccess       = FALSE;
    DWORD               dwReturnValue  = DD_OK;
    CVertexBuffer      *pVertexBuffer  = NULL;
    CCommandBuffer     *pCommandBuffer = NULL;
    HRESULT             ddrval;
    LPDWORD             lpdwDP2RStates;
    DWORD               dwDP2Flags;

    dbgTracePush ("nvDrawPrimitives2 (%08x)", pdp2d);

    NV_SET_CONTEXT (pContext, pdp2d);
    nvSetDriverDataPtrFromContext (pContext);

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        pdp2d->ddrval = DD_OK;
        NvReleaseSemaphore (pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

#ifdef  STEREO_SUPPORT
    NvSetStereoDataPtrFromDriverData;
#endif  //STEREO_SUPPORT

    nvSetD3DSurfaceState (pContext);

    if (global.dwDXRuntimeVersion < 0x700) {
        nvPusherSignalWaitForFlip (pContext->pRenderTarget->getfpVidMem(), DDSCAPS_PRIMARYSURFACE);
    }
    pContext->dwHWUnlockAllPending=FALSE; //  unneeded cleanup HMH
    // clear this bit (since MS does not by themselves)
    pdp2d->dwFlags &= ~D3DHALDP2_VIDMEMVERTEXBUF;

#ifdef NV_NULL_DRIVER
    goto normalExit;
#endif //NV_NULL_DRIVER
#ifdef NV_PROFILE
    NVP_START (NVP_T_DP2);
#endif

    // get the context
    pContext->dwTickLastUsed = GetTickCount();

    // Need to make sure that an unfriendly mode switch didn't sneak and not cause
    // us to get re-enabled properly.
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
#ifdef NV_PROFILE
            NVP_STOP (NVP_T_DP2);
            nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
            goto normalExit;
        }
    }

    // locutus 2
    if (g_dwPMTrigger == PM_REG_PMTRIGGER_1MS)
    {
        static DWORD dwLast = 0;
        if (!dwLast) dwLast = GetTickCount();
        DWORD dwNow = GetTickCount();
        if (dwNow != dwLast)
        {
            PM_SIGNAL (PM_REG_PMTRIGGER_1MS);
            dwLast = dwNow;
        }
    }

    /*******************************************************************
     * COMMAND BUFFER SETUP
     *******************************************************************/

    CNvObject *pNvObject;

    // get command buffer
    pNvObject = GET_PNVOBJ(pdp2d->lpDDCommands);
    pCommandBuffer = pNvObject ? pNvObject->getCommandBuffer() : NULL;

    // Get address of command buffer.
    lpCommandBufferStart = (LPBYTE)(pdp2d->lpDDCommands->lpGbl->fpVidMem);

    if (!lpCommandBufferStart)
    {
#ifdef NV_PROFILE
        NVP_STOP (NVP_T_DP2);
        nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
        dwReturnValue = DDERR_CANTLOCKSURFACE;
        goto normalExit;
    }

    // Calculate pointer to the first command to be processed and the end of the buffer.
    lpCommands         = (LPD3DHAL_DP2COMMAND)(lpCommandBufferStart + pdp2d->dwCommandOffset);
    lpCommandBufferEnd = (LPBYTE)((DWORD)lpCommands + pdp2d->dwCommandLength);

    /*******************************************************************
     * FVF DATA MUNGING
     *******************************************************************/

    if (pContext->dwDXAppVersion < 0x800) {
        // create a DX8-style shader from the FVF format
        pContext->pCurrentVShader->create (pContext, pdp2d->dwVertexType, CVertexShader::getHandleFromFvf( pdp2d->dwVertexType ) );

#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
        {
            DWORD dwPTFlag;
            if (NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader))
                dwPTFlag = KELVIN_FLAG_PASSTHROUGHMODE;
            else
            {
                dwPTFlag = 0;
#ifdef  STEREO_SUPPORT
                if (STEREO_ENABLED)
                    pStereoData->dwHWTnL++;
#endif  //STEREO_SUPPORT
            }
            if ((pContext->hwState.dwStateFlags & KELVIN_FLAG_PASSTHROUGHMODE) != dwPTFlag) {
                pContext->hwState.dwStateFlags &= ~KELVIN_FLAG_PASSTHROUGHMODE;
                pContext->hwState.dwStateFlags |= dwPTFlag;
                pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_COMBINERS_SPECFOG
                                               |  KELVIN_DIRTY_VERTEX_SHADER
                                               |  KELVIN_DIRTY_LIGHTS
                                               |  KELVIN_DIRTY_TRANSFORM
                                               |  KELVIN_DIRTY_MISC_STATE;
            }
        }
        else
#endif
#if (NVARCH >= 0x010)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
            DWORD dwPTFlag;
            if (NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader))
                dwPTFlag = CELSIUS_FLAG_PASSTHROUGHMODE;
            else
            {
                dwPTFlag = 0;
#ifdef  STEREO_SUPPORT
                if (STEREO_ENABLED)
                    pStereoData->dwHWTnL++;
#endif  //STEREO_SUPPORT
            }
            if ((pContext->hwState.dwStateFlags & CELSIUS_FLAG_PASSTHROUGHMODE) != dwPTFlag) {
                pContext->hwState.dwStateFlags &= ~CELSIUS_FLAG_PASSTHROUGHMODE;
                pContext->hwState.dwStateFlags |= dwPTFlag;
                pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TL_MODE
                                               |  CELSIUS_DIRTY_FVF
                                               |  CELSIUS_DIRTY_TRANSFORM
                                               |  CELSIUS_DIRTY_CONTROL0
                                               |  CELSIUS_DIRTY_SPECFOG_COMBINER;
            }
        }
#endif
    }
#ifdef  STEREO_SUPPORT
    else
        if (STEREO_ENABLED && !NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader))
            pStereoData->dwHWTnL++;
#endif  //STEREO_SUPPORT

    /*******************************************************************
     * VERTEX BUFFER DETERMINATION
     *******************************************************************/

    // Get address of vertex buffer.
    BOOL        bVBRename;
    LPBYTE      pVertexData;

    DWORD       dwVertexBufferOffset;     // offset within the buffer
    DWORD       dwVertexBufferLength;     // length in bytes, not including the offset

    bVBRename = FALSE;

    pVertexBuffer = NULL;
    pVertexData   = NULL;

    if (pdp2d->dwFlags & D3DHALDP2_USERMEMVERTICES ) {

        if (pContext->dwDXAppVersion >= 0x800) {
            // need to update hardware if streams have been renamed...
            for (DWORD i=0;i<NV_CAPS_MAX_STREAMS;i++) {
                if (pContext->ppDX8Streams[i] != NULL) {
                    if (pContext->ppDX8Streams[i]->isModified()) {
#if (NVARCH >= 0x010)
                        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
                        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
#endif
                        pContext->ppDX8Streams[i]->tagNotModified();
                    }
                    else {
                        pContext->dwFlags |= CONTEXT_NEEDS_VTX_CACHE_FLUSH;
                    }
                }
            }
        }
        else
        {
            //user mode verts, set the dirty bits
#if (NVARCH >= 0x010)
            pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
            pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
#endif
        }
        pVertexData = (LPBYTE)pdp2d->lpVertices;

    }

    else {

        // must be a vertex buffer, get it
        pVertexBuffer = (GET_PNVOBJ(pdp2d->lpDDVertex))->getVertexBuffer();

        if (!pVertexBuffer) {
            // the vertex buffer must have been created by MS rather than us. get theirs.
            pVertexData = (LPBYTE)(pdp2d->lpDDVertex->lpGbl->fpVidMem);
        }

        else {

            pVertexBuffer->prefetchSurface();

            if (pVertexBuffer->cachedhwCanRead()) {

                // decide on Vb renaming policy
                if ((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_VB_RENAME) &&
                    (pContext->dwDXAppVersion < 0x0800)) // dx8 apps should never have to rename -or- flush!
                {
                    // determine how to handle this locked buffer
                    if (pVertexBuffer->isD3DLocked())
                    {
                        // capture
#ifdef CAPTURE
                        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                            CAPTURE_MEMORY_WRITE memwr;
                            memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                            memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                            memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
                            memwr.dwCtxDMAHandle       = (pVertexBuffer->getHeapLocation() == CSimpleSurface::HEAP_VID)
                                                       ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                                       : D3D_CONTEXT_DMA_HOST_MEMORY;
                            memwr.dwOffset             = pVertexBuffer->getOffset();
                            memwr.dwSize               = pVertexBuffer->getSize();
                            captureLog (&memwr,sizeof(memwr));
                            captureLog ((void*)pVertexBuffer->getAddress(),memwr.dwSize);
                        }
#endif
                        if( global.dwDXRuntimeVersion > 0x800 ) {
                            if (pdp2d->dwFlags & D3DHALDP2_SWAPVERTEXBUFFER) {
                                // d3d says we should rename, tag here & do so at exit
                                bVBRename = TRUE;
                            }
                            else if( !pVertexBuffer->isD3DImplicit() ) {
                                // We only need to stall on explicit VBs.
                                bVBFlushOnExit = TRUE;
                            }
                        }
                        else {
                            if (pdp2d->dwFlags & D3DHALDP2_SWAPVERTEXBUFFER) {
                                // d3d says we should rename, tag here & do so at exit
                                bVBRename = TRUE;
                            }
                            else if( (!pCommandBuffer) || (!(pdp2d->dwFlags & D3DHALDP2_REQCOMMANDBUFSIZE)) ) {
                                 // Workaround for runtime bugs in DX8 and before
                                bVBFlushOnExit = TRUE;
                            }
                        }


                    }
                }
                else {
                    // vertex buffer renaming disabled in nvPerformanceStrategy
                    // this should only get hit on DX8 or if renaming is forced off due to a registry override
                    if (pVertexBuffer->isD3DLocked()) {
                        bVBFlushOnExit = TRUE;
                    }
                }

                // program HW (we have to force a VC flush since VB contents may have changed)
                if (pVertexBuffer->isD3DLocked() || pVertexBuffer->isModified()) {
#if (NVARCH >= 0x020)
                    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
                    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
#endif
                }
            }
        }
    }

    // The vertex size and the shader stride must match for DX6 and DX7 apps.
    // pdp2d->dwVertexSize is not reliable an ALL DP2 calls for apps prior to DX6.
    nvAssert ( (pContext->dwDXAppVersion >= 0x0800) ||
               (pContext->dwDXAppVersion < 0x0600) ||
               (pContext->pCurrentVShader->getStride() == pdp2d->dwVertexSize) );

    dwVertexBufferOffset = pdp2d->dwVertexOffset;
    dwVertexBufferLength = pContext->pCurrentVShader->getStride() * pdp2d->dwVertexLength;

    // we should have a VB or some other data source, but not both
    nvAssert ((pVertexBuffer != NULL) ^ (pVertexData != NULL));

    if (pContext->dwDXAppVersion >= 0x0800) {

        // save these off for DP2SetStreamSourceUM
        pContext->pVertexDataUM        = pVertexData;
        pContext->dwVertexDataLengthUM = dwVertexBufferLength;

    }

    else {

        // cast plain vertex data into a vertex buffer
        if (pVertexData) {

            pVertexBuffer = getDC()->defaultSysVB.getVB();
            pVertexBuffer->own ((DWORD)(pVertexData), dwVertexBufferLength, CSimpleSurface::HEAP_SYS);

        }

        // update the stride and cast vertex buffer into DX8-style streams
        if (pVertexBuffer) {

            pVertexBuffer->setVertexStride (pContext->pCurrentVShader->getStride());

            if (pVertexBuffer->isModified() && !pVertexBuffer->isDynamic()
             && (pVertexBuffer->getLockCount() == 1) && !pVertexBuffer->getSuperTriLookAsideBuffer())
            {
                // check more supertri criteria...
#ifndef MCFD
                if ((pVertexBuffer->getSurface()->hwCanRead())) {
#else
                if ((pVertexBuffer->hwCanRead())) {
#endif
                    pVertexBuffer->prepareSuperTriLookAsideBuffer();
                }
            }
            //um we're in DX7, no need to set all of the streams, just stream zero
            if (pContext->ppDX8Streams[0] != pVertexBuffer)
            {
                pContext->ppDX8Streams[0] = pVertexBuffer;
                //don't forget to set dirty bits
#if (NVARCH >= 0x010)
                pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
                pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
#endif
            }

            pContext->dwStreamDMACount = nvCountStreamDMAs (pContext, 1);

        }

    }

    if ((pVertexBuffer) && (pVertexBuffer->isModified())) {
        pVertexBuffer->tagNotModified();
    }

#ifdef WINNT
    // for win2k range checking, we need to specify the length of the valid vertex buffer
    global.dwMaxVertexOffset = dwVertexBufferOffset + dwVertexBufferLength;
#endif

#if (NVARCH >= 0x020)
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)))
#elif (NVARCH >= 0x010)
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS))
#endif
    {
        // If state has changed or the spare subchannel does not contain
        // the correct triangle object, force an object state load.
        if (!pContext->bUseDX6Class)
        {
            if ((pContext->bStateChange || pDriverData->TwoDRenderingOccurred)
             || (pDriverData->dDrawSpareSubchannelObject != D3D_DX5_TEXTURED_TRIANGLE))
                NV_FORCE_TRI_SETUP(pContext);
        }
        else
        {
            if ((pContext->bStateChange || pDriverData->TwoDRenderingOccurred)
             || (pDriverData->dDrawSpareSubchannelObject != D3D_DX6_MULTI_TEXTURE_TRIANGLE))
                NV_FORCE_TRI_SETUP(pContext);
        }
    }
 
    lpdwDP2RStates = pdp2d ? pdp2d->lpdwRStates : NULL;
    dwDP2Flags     = pdp2d ? pdp2d->dwFlags : 0;

    pContext->sysvb.bWriteOffset = false;
    pContext->dp2.dwVertexBufferOffset = dwVertexBufferOffset;
    pContext->dp2.dwVStart = 0;
    pContext->dp2.dwTotalOffset = 0;
    pContext->dp2.dwVertexLength = pdp2d->dwVertexLength;
    pContext->sysvb.pSysMemVB = 0;

    //flag celsius and kelvin as dirty if the vertex buffer offset has changed.
    if (pContext->hwState.dwVertexOffset != pContext->dp2.dwVertexBufferOffset)
    {
#if (NVARCH >= 0x020)
        pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_FVF;
#endif
#if (NVARCH >= 0x010)
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_FVF;
#endif
    }
    /*******************************************************************
     * COMMAND BUFFER PARSING
     *******************************************************************/

    while ((dwReturnValue == DD_OK) && ((LPBYTE)lpCommands < lpCommandBufferEnd)) {

        dbgDisplayDrawPrimitives2Info (lpCommands->bCommand, lpCommands->wPrimitiveCount);

        // make sure our table index is OK
        nvAssert (lpCommands->bCommand <= D3D_DP2OP_MAX);

        if ((*(pContext->pDP2FunctionTable))[lpCommands->bCommand] != NULL) {

#ifdef NV_PROFILE_DP2OPS
            if (pDriverData->pBenchmark == NULL) {
                pDriverData->pBenchmark = new CBenchmark;
                pDriverData->pBenchmark->resetData();
            }
            pDriverData->pBenchmark->logData(lpCommands->bCommand, lpCommands->wPrimitiveCount);  
#endif

#ifdef LPC_OPT
            pDriverData->dwThisDP2 = lpCommands->bCommand;
#endif 
            dwReturnValue = ((*(pContext->pDP2FunctionTable))[lpCommands->bCommand]) (pContext,
                                                                                      &lpCommands,
                                                                                      lpCommandBufferEnd,
                                                                                      lpdwDP2RStates,
                                                                                      dwDP2Flags);
             
#ifdef LPC_OPT
            pDriverData->dwLastDP2 = pDriverData->dwThisDP2;
#endif 
            nvAssert (dwReturnValue == DD_OK);
        }

        else {
            // Unknown command.  Pass it to the Unknown Command Callback for handling.
            if (fnD3DParseUnknownCommandCallback) {
                DPF_LEVEL (NVDBG_LEVEL_INFO,
                           "nvDrawPrimitives2: Sending command %d to unknown command parser",
                           lpCommands->bCommand);
                ddrval = fnD3DParseUnknownCommandCallback((void**)lpCommands, (void**)&lpResumeCommands);

                if (ddrval != DD_OK) {
                    nvPusherStart (FALSE);
                    dwReturnValue        = ddrval;
                    pdp2d->dwErrorOffset = (DWORD)((LPBYTE)lpCommands - lpCommandBufferStart);
#ifdef NV_PROFILE
                    NVP_STOP (NVP_T_DP2);
                    nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
                    //check to see if the current streams/rendertarget/Z/textures need to be unlocked
                    if(pContext->dwHWUnlockAllPending){ //HMH
                        nvHWUnlockTextures (pContext);
                        // need to mark all streams as in use by hardware...
                        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
                        pContext->dwHWUnlockAllPending=FALSE;
                    }
                    goto normalExit;
                }

                // Set the resume address.
                lpCommands = lpResumeCommands;
            }
            else {
                nvPusherStart (FALSE);
                dwReturnValue        = D3DERR_COMMAND_UNPARSED;
                pdp2d->dwErrorOffset = (DWORD)((LPBYTE)lpCommands - lpCommandBufferStart);
#ifdef NV_PROFILE
                NVP_STOP (NVP_T_DP2);
                nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif
                //check to see if the current streams/rendertarget/Z/textures need to be unlocked
                if(pContext->dwHWUnlockAllPending){ //HMH
                    nvHWUnlockTextures (pContext);
                    // need to mark all streams as in use by hardware...
                    nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
                    pContext->dwHWUnlockAllPending=FALSE;
                }
                goto normalExit;
            }
        }
    }  // while...
    //check to see if the current streams/rendertarget/Z/textures need to be unlocked
    if(pContext->dwHWUnlockAllPending){ //HMH
        nvHWUnlockTextures (pContext);
        // need to mark all streams as in use by hardware...
        nvHWUnlockStreams (pContext, CELSIUS_CAPS_MAX_STREAMS);
        pContext->dwHWUnlockAllPending=FALSE;
    }
    /*******************************************************************
     * VB renaming
     *******************************************************************/

    /*
            note for future rel
            -------------------
            if a VB was passed in and mustRenameToSystem() returns true on it, we need to
            move it to system memory and tell d3d about it using its renamer
    */

#ifndef MCFD   //take care of that later
    // renaming requested?
    if ((dwReturnValue == DD_OK)    // dont bother renaming when an error occurred
     && bVBRename)                  // we can rename
    {

        // rename
        #ifdef NV_PROFILE_CPULOCK
        NVP_START (NVP_T_VBDP2_LOCK);
        #endif

        DDLOCKINDEX(NVSTAT_LOCK_VB);
        pVertexBuffer->cpuLock (CSimpleSurface::LOCK_RENAME);

        #ifdef NV_PROFILE_CPULOCK
        NVP_STOP (NVP_T_VBDP2_LOCK);
        nvpLogTime (NVP_T_VBDP2_LOCK,nvpTime[NVP_T_VBDP2_LOCK]);
        #endif

        // grow if we should
        DWORD dwNewSize = (pdp2d->dwFlags & D3DHALDP2_REQVERTEXBUFSIZE) ? pdp2d->dwReqVertexBufSize
                                                                        : 0;
        if (dwNewSize > pVertexBuffer->getSize())
        {
            DWORD dwOriginalSize = pVertexBuffer->getSurface()->getSize();
            DWORD dwHeap         = pVertexBuffer->getSurface()->getHeapLocation();

            pVertexBuffer->getSurface()->destroy();

            if (!pVertexBuffer->getSurface()->create (dwNewSize, 1, 1, 0,
                                                      dwHeap, dwHeap,
                                                      CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                                                     ,CAPTURE_SURFACE_KIND_VERTEX
#endif
                                                      ))
            {
                // oops, cannot grow. try to revert to original size
                if (!pVertexBuffer->getSurface()->create (dwOriginalSize, 1, 1, 0,
                                                          dwHeap, dwHeap,
                                                          CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                                                         ,CAPTURE_SURFACE_KIND_VERTEX
#endif
                                                          ))
                {
                    // cannot do that either. fail
                    DPF ("nvDrawPrimitives2: cannot grow or re-establish VB.");
                    dbgD3DError();
                    dwReturnValue = DDERR_CANTLOCKSURFACE;
                    goto normalExit;
                }

                dwNewSize = dwOriginalSize;
            }
        }

        // update D3D
        pdp2d->dwFlags                     |= D3DHALDP2_VIDMEMVERTEXBUF | D3DHALDP2_REQVERTEXBUFSIZE;
//            pdp2d->lpDDVertex->lpGbl->fpVidMem  = pVertexBuffer->getfpVidMem();
        if (pVertexBuffer->getHeapLocation() == CSimpleSurface::HEAP_SYS)
        {
            pdp2d->lpDDVertex->lpGbl->fpVidMem = pVertexBuffer->getAddress();
        }
#ifdef WINNT
        else if (pVertexBuffer->getHeapLocation() == CSimpleSurface::HEAP_AGP)
        {
#ifdef NV_AGP
            pdp2d->lpDDVertex->lpGbl->fpVidMem = nvAGPGetUserAddr(pVertexBuffer->getAddress());
#else
            FLATPTR fpAgpBase = pdp2d->lpDDVertex->lpGbl->fpVidMem - pdp2d->lpDDVertex->lpGbl->fpHeapOffset;

            pdp2d->lpDDVertex->lpGbl->fpHeapOffset  = pVertexBuffer->getfpVidMem() + pDriverData->GARTLinearHeapOffset;
            pdp2d->lpDDVertex->lpGbl->fpVidMem = fpAgpBase + pdp2d->lpDDVertex->lpGbl->fpHeapOffset;
#endif
        }
        else // video
        {
            FLATPTR fpVidBase = pdp2d->lpDDVertex->lpGbl->fpVidMem - pdp2d->lpDDVertex->lpGbl->fpHeapOffset;

            pdp2d->lpDDVertex->lpGbl->fpHeapOffset = pVertexBuffer->getfpVidMem();
            pdp2d->lpDDVertex->lpGbl->fpVidMem = fpVidBase + pVertexBuffer->getfpVidMem();
        }
#else
        else // video
        {
            pdp2d->lpDDVertex->lpGbl->fpVidMem = pVertexBuffer->getAddress();
        }
#endif

        pdp2d->dwReqVertexBufSize           = dwNewSize;
    }
#endif

    // grow command buffer
    if (pCommandBuffer && (pdp2d->dwFlags & D3DHALDP2_REQCOMMANDBUFSIZE) && pCommandBuffer->isOwner())
    {
        pCommandBuffer->growBy(pdp2d->dwReqCommandBufSize, FALSE);
        pdp2d->lpDDCommands->lpGbl->fpVidMem      = pCommandBuffer->getAddress();
        pdp2d->lpDDCommands->lpGbl->dwLinearSize  = pCommandBuffer->getSize();
        pdp2d->dwFlags                           |= D3DHALDP2_SWAPCOMMANDBUFFER /*| D3DHALDP2_VIDMEMCOMMANDBUF*/;
        pdp2d->dwFlags                           &= ~D3DHALDP2_REQCOMMANDBUFSIZE;
    }

    pDriverData->TwoDRenderingOccurred   = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;

    nvPusherStart (FALSE);

#ifdef NV_PROFILE
    NVP_STOP (NVP_T_DP2);
    nvpLogTime (NVP_T_DP2,nvpTime[NVP_T_DP2]);
#endif

normalExit:
    if (bVBFlushOnExit) {
        DDLOCKINDEX(NVSTAT_LOCK_VB_FLUSH);
#ifdef MCFD
        pVertexBuffer->LockForCPU(CSimpleSurface::LOCK_READ);
#else
        pVertexBuffer->cpuLock(CSimpleSurface::LOCK_NORMAL);
#endif
    }

    if (pContext->sysvb.pSysMemVB) {
        if (pContext->sysvb.bWriteOffset)
            getDC()->defaultVB.updateMap();
        pContext->sysvb.pSysMemVB = NULL;
    }

    pdp2d->ddrval = dwReturnValue;

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

#endif  // NVARCH >= 0x04

