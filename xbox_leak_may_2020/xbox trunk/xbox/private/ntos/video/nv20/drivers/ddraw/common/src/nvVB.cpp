/***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#include "nvprecomp.h"

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  CVertexBuffer  *********************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * CVertexBuffer::prepareSuperTriLookAsideBuffer
 */
BOOL CVertexBuffer::prepareSuperTriLookAsideBuffer
(
    void
)
{
    //
    // fail if we cannot support supertri for this buffer
    //
    if (!getVertexStride()) return FALSE;

    //
    // [re]create buffer if needed
    //
    DWORD dwVertexCount = getSize() / getVertexStride();
    if (dwVertexCount != m_dwSuperTriLookAsideBufferSize)
    {
        destroySuperTriLookAsideBuffer();
        m_dwSuperTriLookAsideBufferSize = dwVertexCount;
    }

    if (!m_pSuperTriLookAsideBuffer)
    {
        // Align to 2 cachelines (1 cacheline for Willamette)
        // this is taken care of internally by AllocIPM (64 byte align)
        DWORD dwSize = dwVertexCount * sizeof(STVERTEX);
        m_pSuperTriLookAsideBuffer = AllocIPM(dwSize);
        if (!m_pSuperTriLookAsideBuffer) return FALSE;
        m_dwFlags |= FLAG_SUPERTRIOWNSMEM;
    }

    //
    // fill out (very slow operation)
    //
    DWORD dwSource = getAddress();
    DWORD dwDest   = (DWORD)m_pSuperTriLookAsideBuffer;
    DWORD *dwDestp = (DWORD *)m_pSuperTriLookAsideBuffer;
    for (DWORD i = 0; i < dwVertexCount; i++)
    {
        nvMemCopy (dwDest, dwSource, sizeof(STVERTEX));
        dwDestp[3] = 0;
        dwSource += getVertexStride();
        dwDestp  += sizeof(STVERTEX)/sizeof(float);
        dwDest   += sizeof(STVERTEX);
    }

    //
    // done
    //
    return TRUE;
}

/*****************************************************************************
 * CVertexBuffer::destroySuperTriLookAsideBuffer
 */
void CVertexBuffer::destroySuperTriLookAsideBuffer
(
    void
)
{
    if (m_dwFlags & FLAG_SUPERTRIOWNSMEM)
    {
        if (m_pSuperTriLookAsideBuffer)
        {
            FreeIPM (m_pSuperTriLookAsideBuffer);
            m_pSuperTriLookAsideBuffer = NULL;
        }
    }
}

/*****************************************************************************
 * CVertexBuffer::CVertexBuffer
 *
 * initialize a VB object
 */
CVertexBuffer::CVertexBuffer
(
    CNvObject *pWrapperObject
)
{
    // tell world
    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "CVertexBuffer[%08x]::CVertexBuffer()", this);

    // assign variables
    m_pWrapperObject = pWrapperObject;

    // init the others
    m_dwFlags                       = 0;
    m_dwSuperTriLookAsideBufferSize = 0;
    m_pSuperTriLookAsideBuffer      = NULL;
    m_dwLockCount                   = 0;
}

/*****************************************************************************
 * CVertexBuffer::~CVertexBuffer
 *
 * destroy a VB object
 */
CVertexBuffer::~CVertexBuffer
(
    void
)
{
    /*
     * tell world
     */
    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "CVertexBuffer[%08x]::~CVertexBuffer()", this);

    destroy();
}

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  CCommandBuffer  ********************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * CCommandBuffer::CCommandBuffer
 */
CCommandBuffer::CCommandBuffer
(
    CNvObject                 *pWrapperObject,
    LPDDRAWI_DDRAWSURFACE_LCL  pDDSurfaceLcl
)
{
    /*
     * tell world
     */
    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "CCommandBuffer[%08x]::CCommandBuffer()", this);

    m_pWrapperObject  = pWrapperObject;
    m_pDDSurfaceLcl   = pDDSurfaceLcl;
    m_Surface.setWrapper(pWrapperObject);
#ifdef WINNT
    m_Surface.getWrapper()->setDDSLcl(pDDSurfaceLcl);
#endif
}

/*****************************************************************************
 * CCommandBuffer::~CCommandBuffer
 */
CCommandBuffer::~CCommandBuffer
(
    void
)
{
    /*
     * tell world
     */
    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "CCommandBuffer[%08x]::~CCommandBuffer()", this);
}

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  exports  ***************************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/

/*****************************************************************************
 * D3DCanCreateExecuteBuffer32
 *
 * D3D callback to check if we can create an execute, vertex or command buffer
 */
DWORD __stdcall D3DCanCreateExecuteBuffer32
(
    LPDDHAL_CANCREATESURFACEDATA pCanCreateSurfaceData
)
{
    dbgTracePush ("D3DCanCreateExecuteBuffer32");

    //
    // figure out if we can create a VB
    //
#ifdef DEBUG
    DWORD         dwLatest       = 0;
    PNVD3DCONTEXT pContext       = (PNVD3DCONTEXT)getDC()->dwContextListHead;
    PNVD3DCONTEXT pActualContext = NULL;
    while (pContext)
    {
        if (pContext->dwTickLastUsed > dwLatest)
        {
            pContext->dwTickLastUsed = dwLatest;
            pActualContext           = pContext;
        }
        pContext = pContext->pContextNext;
    }
    DWORD dwAppRuntimeLevel = pActualContext ? pActualContext->dwDXAppVersion
                                             : 0x0600;

    // show this call
    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "D3DCanCreateExecuteBuffer32: appdx=%04x  rtdx=%04x",
               dwAppRuntimeLevel,global.dwDXRuntimeVersion);
#endif

    //
    // punt if we have DX6 runtime
    //
    if (global.dwDXRuntimeVersion <= 0x600)
    {
        pCanCreateSurfaceData->ddRVal = DDERR_INVALIDCAPS;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

#ifdef DO_NOT_SUPPORT_VERTEX_BUFFERS // see nvprecomp.h
    pCanCreateSurfaceData->ddRVal = DDERR_INVALIDCAPS;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
#endif //!DO_NOT_SUPPORT_VERTEX_BUFFERS

    //
    // is this an optimize call?
    //
    if (pCanCreateSurfaceData->lpDDSurfaceDesc->dwFlags & DDSD_SRCVBHANDLE)
    {
        pCanCreateSurfaceData->ddRVal = DDERR_INVALIDCAPS;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    nvSetDriverDataPtrFromDDGbl (pCanCreateSurfaceData->lpDD);

    // fail if we don't have AGP memory -
    // on true PCI cards we never get this call, but on an AGP card running with AGP disabled (with the
    // DirectX diagnostic tool) the run-time will puke if we create a vertex buffer in PCI memory
    //
    if (!(pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_PCIBUS))
        if (pDriverData->GARTLinearBase == NULL)
        {
            NvReleaseSemaphore(pDriverData);
            pCanCreateSurfaceData->ddRVal = DDERR_INVALIDCAPS;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }

    //
    // we claim that we can create all types of buffers
    //
    NvReleaseSemaphore(pDriverData);
    pCanCreateSurfaceData->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

/*****************************************************************************
 * D3DCreateExecuteBuffer32
 *
 * D3D callback to create an execute, vertex or command buffer
 */
DWORD __stdcall D3DCreateExecuteBuffer32
(
    LPDDHAL_CREATESURFACEDATA pCreateSurfaceData
)
{
    dbgTracePush ("D3DCreateExecuteBuffer32");

    //
    // is this an optimize call?
    //
    DWORD dwFlags = pCreateSurfaceData->lpDDSurfaceDesc->dwFlags;
    if (dwFlags & DDSD_SRCVBHANDLE)
    {
        pCreateSurfaceData->ddRVal = DDERR_INVALIDCAPS;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    //
    // get driver context
    //
    nvSetDriverDataPtrFromDDGbl (pCreateSurfaceData->lpDD);

    // Reset NV and get the monitor frequency after a mode reset
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        getDisplayDuration();
        if (!nvEnable32 (pCreateSurfaceData->lpDD)) {
            NvReleaseSemaphore(pDriverData);
            pCreateSurfaceData->ddRVal = DDERR_OUTOFMEMORY;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    //
    // get app version by looking for the presence of FVF flag
    // i begged for a better hint bit from MS - will see.
    //

    // bIsDX6 will be true for DX8 Index Buffers.  No way to code around it, so we will take a
    // dx6 performance hit when we are running on DX8
    BOOL bIsDX6       = FALSE;
    // wa rpeviously: BOOL bIsDX6 = ((dwFlags & DDSD_FVF) != DDSD_FVF); // not valid for command buffers
    BOOL bIsImplicit  = FALSE;
    BOOL bForceSysMem = FALSE;

    /*
     * for all surfaces in SList
     */
    LPDDRAWI_DDRAWSURFACE_LCL *lplpSList = pCreateSurfaceData->lplpSList;
    for(DWORD i = 0; i < pCreateSurfaceData->dwSCnt; i++)
    {
        /*
         * get the dd surface
         */
        LPDDRAWI_DDRAWSURFACE_LCL lpSurf = lplpSList[i];

        /*
         * figure out what to create
         */
        if (lpSurf->ddsCaps.dwCaps & DDSCAPS_EXECUTEBUFFER)
        {
            if (lpSurf->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_VERTEXBUFFER)
            {
            allocVertexBuffer:
                //
                // we have shown that DX6 vertex buffers are slower than MS managing them (3d mark 99)
                //
                BOOL bDoNotHandle = FALSE;
                if (bIsDX6 /*|| bIsImplicit  -- slower now - bdw */) {
                    bDoNotHandle = TRUE;
                }
#ifdef STEREO_SUPPORT
                //All vertices that potentially will be read back in order to be modified for stereo
                //should go to system memory.
                else
                {
                    if (STEREO_ENABLED)
                    {
                        if (bIsImplicit || ((dwFlags & DDSD_FVF) != DDSD_FVF))
                        {
                            //For stereo implicit and DX6 type VBs still go to system memory.
                            bDoNotHandle = TRUE;
                        } else
                        {
                            LPDDSURFACEDESC2 lpSurfDesc2 = (LPDDSURFACEDESC2)pCreateSurfaceData->lpDDSurfaceDesc;
                            if (FVF_TRANSFORMED(lpSurfDesc2->dwFVF))
                            {
                                //We don't want to allocate VBs for transformed vertices in offscreen memory. The
                                //problem is that we will have to physically modify vertices for the left and right
                                //eyes and it is going to be dog slow if the vertices end up in VRAM. So sacrificing
                                //VRAM VBs for this case is our trade off for being able to view stereo.
                                bDoNotHandle = TRUE;
                            }
                        }
                    }
                }
#endif  //STEREO_SUPPORT
                if (bDoNotHandle)
                {
                    NvReleaseSemaphore(pDriverData);
                    pCreateSurfaceData->ddRVal = DDERR_INVALIDCAPS;
                    dbgTracePop();
                    return DDHAL_DRIVER_HANDLED;
                }

                /*
                 * create a vertex buffer
                 */
#ifndef WINNT   // WINNT BUG - what about pid?
                CNvObject      *pNvObj = new CNvObject (lpSurf->dwProcessId);
#else   // WINNT
                CNvObject      *pNvObj = new CNvObject (0);
#endif  // WINNT
                CVertexBuffer  *pVertexBuffer;
                BOOL            bAllowRenaming;

                if (pNvObj)
                {
                    pVertexBuffer = new CVertexBuffer (pNvObj);
                    
                    if (!pVertexBuffer)
                    {
                        pNvObj->release();
                        NvReleaseSemaphore(pDriverData);
                        DPF ("D3DCreateExecuteBuffer32: failed to create CVertexBuffer object");
                        dbgD3DError();
                        pCreateSurfaceData->ddRVal = DDERR_OUTOFMEMORY;
                        dbgTracePop();
                        return DDHAL_DRIVER_HANDLED;
                    }

                    #ifdef DX7PROPER
                    if (bIsDX6)
                    {
                        PF ("1. Vertexbuffer is created from a DX6 interface.");
                        DPF_LEVEL(NVDBG_LEVEL_PERFORMANCE, "Vertex Buffer wasn't created by driver, copy forced");
                    }
                    #endif

                    //
                    // determine where to put the buffer
                    //  - we can force the VB in sysmem (dx6 implicit VB)
                    //  - if app will write only, go for agp or vid mem
                    //
                    DWORD dwAllowed;
                    DWORD dwPreferred;
                    LPDDSURFACEDESC2 lpSurfDesc2 = (LPDDSURFACEDESC2)pCreateSurfaceData->lpDDSurfaceDesc;


                    if ( (lpSurfDesc2->dwFVF & D3DFVF_PSIZE) ||
                         (lpSurf->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_RTPATCHES)
                        )
                    {
                        //force emulated point sprites into system memory
                        //force emulated vertex shaders into system memory
                        bForceSysMem = TRUE;
                    }

                    if (bForceSysMem
                     || !(lpSurf->ddsCaps.dwCaps & DDSCAPS_WRITEONLY)
                     /*|| (bIsImplicit)*/)  // Implicit buffers will be DMA'ed (WB2k)
                    {
                        #ifdef DX7PROPER // inaccurte for supertri case (i.e. actually a good thing)
                            DPF_LEVEL(NVDBG_LEVEL_PERFORMANCE, "Vertex Buffer forced into system memory");
                            PF ("2. Vertexbuffer is forced into system memory.");
                        #endif

                        //
                        // we force system memory with no renaming
                        //
                        dwAllowed      = CSimpleSurface::HEAP_SYS;
                        dwPreferred    = CSimpleSurface::HEAP_SYS;
                        bAllowRenaming = FALSE;

                        // temporary fix for sys mem VB's: allow MS to create them.
                        // we really should map/unmap them into the user space ourselves
                        pCreateSurfaceData->ddRVal = DDERR_GENERIC;
                        
                        // delete the pVertexBuffer we created, release the pNvObject
                        delete pVertexBuffer;
                        pNvObj->release();

                        NvReleaseSemaphore(pDriverData);
                        dbgTracePop();
                        return DDHAL_DRIVER_HANDLED;
                    }
                    else
                    {
                        //
                        // we allow both agp & vid
                        //
                        dwAllowed   = CSimpleSurface::HEAP_VID
                                    | (pDriverData->GARTLinearBase ? CSimpleSurface::HEAP_AGP
                                                                   : CSimpleSurface::HEAP_PCI);
                        //
                        // we prefer vid for machines with fast write cap, else agp (its closer)
                        //
                        // DX8  we'll need to look at memory usage etc...
                        if (lpSurf->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_HINTSTATIC) {
                            dwPreferred = CSimpleSurface::HEAP_VID;
                        }
                        else {
                            dwPreferred = (pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_VB_PREFER_AGP) ?
                                          (CSimpleSurface::HEAP_AGP | CSimpleSurface::HEAP_PCI) :
                                          (CSimpleSurface::HEAP_VID);
                        }
                        //
                        // some renaming is allowed
                        //
                        bAllowRenaming = TRUE;
                    }

                    //LPDDSURFACEDESC2 lpSurfDesc2 = (LPDDSURFACEDESC2)pCreateSurfaceData->lpDDSurfaceDesc;
                    // compute a DX8-style shader from the FVF format in order to generate the stride
                    //CVertexShader vShader;
                    //vShader.create ((PNVD3DCONTEXT)(pDriverData->dwContextListHead), lpSurfDesc2->dwFVF);
                    //pVertexBuffer->setVertexStride (vShader.getStride());

                    // set stride to 0. proper stride gets set in DP2
                    pVertexBuffer->setVertexStride (0);

                    // allocate the actual memory
                    DWORD dwSize = lpSurf->lpGbl->dwLinearSize;
#ifdef WINNT
                    pNvObj->setDDSLcl(lpSurf);
#endif
                    if (!pVertexBuffer->create(dwSize,
                                               dwAllowed,
                                               dwPreferred))
                    {
                        // temporary fix for sys mem VB's: allow MS to create them.
                        // we really should map/unmap them into the user space ourselves
                        pCreateSurfaceData->ddRVal = DDERR_GENERIC;

                        // delete the pVertexBuffer we created, release the pNvObject
                        delete pVertexBuffer;
                        pNvObj->release();

                        NvReleaseSemaphore(pDriverData);
                        dbgTracePop();
                        return DDHAL_DRIVER_HANDLED;
                    }

#ifdef MEMORY_ACTIVITY_CAPTURE
                    {
                        for (DWORD a = 0; a < dwSize; a += 4096)
                        {
                            *(DWORD*)(pVertexBuffer->getAddress() + a) = 0xaaaa1111;
                        }
                    }
#endif

                }
                else
                {
                    NvReleaseSemaphore(pDriverData);
                    DPF ("D3DCreateExecuteBuffer32: failed to create CNvObject object");
                    dbgD3DError();
                    pCreateSurfaceData->ddRVal = DDERR_OUTOFMEMORY;
                    dbgTracePop();
                    return DDHAL_DRIVER_HANDLED;
                }

                //
                // tag as dx6 vb
                //
                if (bIsDX6)
                {
                    pVertexBuffer->tagDX6();
                }

                /*
                 * all VBs are renameable
                 */
                if (bAllowRenaming)
                { 
                    pVertexBuffer->tagRenameEnable (CComplexSurface<TOTAL_VB_RENAME>::RENAME_DO_NOTHING);
                }

                //
                // tag flags
                //
                if (bIsImplicit) pVertexBuffer->tagD3DImplicit();

                /*
                 * tell the world
                 */
                DPF_LEVEL (NVDBG_LEVEL_SURFACE, "VB [%08x] created: [%s] incaps=0x%08x, addr=0x%08x ctxdma=%d size=%d pNvObj=%08x",
                           pVertexBuffer,
                           bIsImplicit ? "implicit" : "normal",
                           lpSurf->ddsCaps.dwCaps,
                           pVertexBuffer->getAddress(),
                           pVertexBuffer->getContextDMA(),
                           lpSurf->lpGbl->dwLinearSize,
                           pNvObj);

                /*
                 * populate DDraw
                 */
                pNvObj->setObject (CNvObject::NVOBJ_VERTEXBUFFER, pVertexBuffer);
                SET_PNVOBJ (lpSurf, pNvObj);
                if (global.dwDXRuntimeVersion >= 0x0700) {
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "Create Vertex Buffer: pDDSLcl = %08x, handle = %02x",
                              lpSurf, lpSurf->lpSurfMore->dwSurfaceHandle);
                }

#ifdef WINNT
                if (pVertexBuffer->getHeapLocation() == CSimpleSurface::HEAP_SYS)
                {
                    lpSurf->lpGbl->fpVidMem = (FLATPTR)pVertexBuffer->getfpVidMem();
                }
                else if (pVertexBuffer->getHeapLocation() == CSimpleSurface::HEAP_AGP)
                {
#ifdef NV_AGP
                    lpSurf->lpGbl->fpVidMem = nvAGPGetUserAddr(pVertexBuffer->getAddress());
#else
                    lpSurf->lpGbl->lpVidMemHeap = ppdev->AgpHeap;
                    lpSurf->lpGbl->fpHeapOffset = pVertexBuffer->getfpVidMem() + pDriverData->GARTLinearHeapOffset;
#endif
                }
                else
#endif
                {
#ifdef WINNT
                    lpSurf->lpGbl->fpHeapOffset  = pVertexBuffer->getfpVidMem();
#endif
                    lpSurf->lpGbl->fpVidMem = pVertexBuffer->getfpVidMem();
                }
#ifdef MCFD
                lpSurf->ddsCaps.dwCaps  = pVertexBuffer->m_blockList.modifyDDCaps(lpSurf->ddsCaps.dwCaps);
#else
                lpSurf->ddsCaps.dwCaps  = pVertexBuffer->getSurface()->modifyDDCaps(lpSurf->ddsCaps.dwCaps);
#endif

#ifndef WINNT
                //
                // must tell dx if it ended up in sysmem (cannot use DDSCAPS_SYSTEMMEMORY)
                //
#ifdef MCFD
                if (!pVertexBuffer->hwCanRead())
                {
                    lpSurf->lpGbl->dwGlobalFlags |= DDRAWISURFGBL_SYSMEMEXECUTEBUFFER;
                }
#else
                if (!pVertexBuffer->getSurface()->hwCanRead())
                {
                    lpSurf->lpGbl->dwGlobalFlags |= DDRAWISURFGBL_SYSMEMEXECUTEBUFFER;
                }
#endif
#endif // WINNT
            }
            else if (lpSurf->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_COMMANDBUFFER)
            {
                /*
                 * create a command buffer
                 */
#ifndef WINNT   // WINNT BUG what about pid?
                CNvObject      *pNvObj = new CNvObject (lpSurf->dwProcessId);
#else   // WINNT
                CNvObject      *pNvObj = new CNvObject (0);
#endif  // WINNT
                CCommandBuffer *pCommandBuffer;

                if (pNvObj)
                {
                    pCommandBuffer = new CCommandBuffer(pNvObj,lpSurf);
                    if (!pCommandBuffer)
                    {
                        pNvObj->release();
                        NvReleaseSemaphore(pDriverData);
                        DPF ("D3DCreateExecuteBuffer32: failed to create CCommandBuffer object");
                        dbgD3DError();
                        pCreateSurfaceData->ddRVal = DDERR_OUTOFVIDEOMEMORY;
                        dbgTracePop();
                        return DDHAL_DRIVER_HANDLED;
                    }

                    /*
                     * allocate the actual memory
                     */
                    pNvObj->setObject (CNvObject::NVOBJ_COMMANDBUFFER, pCommandBuffer);
                    SET_PNVOBJ (lpSurf, pNvObj);

                    if (!pCommandBuffer->create(lpSurf->lpGbl->dwLinearSize))
                    {
                        pNvObj->release();
                        NvReleaseSemaphore(pDriverData);
                        DPF ("D3DCreateExecuteBuffer32: failed to create CCommandBuffer surface");
                        dbgD3DError();
                        pCreateSurfaceData->ddRVal = DDERR_OUTOFMEMORY;
                        dbgTracePop();
                        return DDHAL_DRIVER_HANDLED;
                    }
                }
                else
                {
                    NvReleaseSemaphore(pDriverData);
                    DPF ("D3DCreateExecuteBuffer32: failed to create CNvObject object");
                    dbgD3DError();
                    pCreateSurfaceData->ddRVal = DDERR_OUTOFMEMORY;
                    dbgTracePop();
                    return DDHAL_DRIVER_HANDLED;
                }
                /*
                 * populate DDraw
                 */
                // Command buffers are always system mem, have to change this code if that ever changes.

                lpSurf->lpGbl->fpVidMem = (FLATPTR)pCommandBuffer->getSurface()->getfpVidMem();
                lpSurf->ddsCaps.dwCaps        = pCommandBuffer->getSurface()->modifyDDCaps(lpSurf->ddsCaps.dwCaps);

#ifndef WINNT   // WINNT BUG what do we do here? ANSWER: nothing, DX6 not supported under NT
                lpSurf->lpGbl->dwGlobalFlags |= DDRAWISURFGBL_SYSMEMEXECUTEBUFFER; // must tell dx6 that it ended up in sysmem
#endif  // !WINNT
                /*
                 * tell the world
                 */
                DPF_LEVEL (NVDBG_LEVEL_SURFACE, "Command buffer [%08x] created at 0x%08x, size %d bytes (pNvObj=%08x)",
                           pCommandBuffer,lpSurf->lpGbl->fpVidMem,lpSurf->lpGbl->dwLinearSize,pNvObj);
            }

            else if (lpSurf->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_INDEXBUFFER)
            {
                //just use the default VB create for now.
                bForceSysMem = TRUE;
                goto allocVertexBuffer;
            }

            else
            {
                //
                // Implicit VBs on DX6 MUST be in system memory
                //
                bIsImplicit = TRUE;
                if (bIsDX6)
                {
                    bForceSysMem = TRUE;
                }

                //
                // alloc.
                // use archaic beginners all-purpose symbolic instruction code logic to achieve this.
                //
                goto allocVertexBuffer;
            }
        }

        else
        {
            /*
             * foreign request - we cannot honor
             */
            NvReleaseSemaphore(pDriverData);
            pCreateSurfaceData->ddRVal = DDERR_INVALIDCAPS;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    NvReleaseSemaphore(pDriverData);
    pCreateSurfaceData->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

/*****************************************************************************
 * D3DDestroyExecuteBuffer32
 *
 * D3D callback to destroy an execute, vertex or command buffer
 */
DWORD __stdcall D3DDestroyExecuteBuffer32
(
    LPDDHAL_DESTROYSURFACEDATA pDestroySurfaceData
)
{
    dbgTracePush ("D3DDestroyExecuteBuffer32");

    /*
     * get driver context
     */
    nvSetDriverDataPtrFromDDGbl (pDestroySurfaceData->lpDD);

    //check for mode switch notification
    /*
    //This is commented out due to a MS kernel bug.  On A CTRL-ALT-DELETE, this
    //bit will be a random value.  This causes the app to get an invalid VB pointer
    //and fault. re-enable for Win2K SP2/Whistler

    if (global.dwDXRuntimeVersion >= 0x0700 && pDestroySurfaceData->lpDDSurface->dwFlags & DDRAWISURF_INVALID) {
        //not a true destroy, just notification of lost VB.
        //go ahead and exit?
        pDestroySurfaceData->lpDDSurface->lpGbl->fpVidMem = 0;
        NvReleaseSemaphore(pDriverData);
        pDestroySurfaceData->ddRVal = DD_OK;
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }
    */
    /*
     * extract surface to destroy
     */
    CNvObject *pNvObj = GET_PNVOBJ (pDestroySurfaceData->lpDDSurface);

    // pNvObj will be NULL if we voluntarily bailed out on the creation
    // (we sometimes do this for performance reasons)
    if (pNvObj) {

        switch (pNvObj->getClass())
        {
            /*
             * destroy vertex buffer
             */
            case CNvObject::NVOBJ_VERTEXBUFFER:
            case CNvObject::NVOBJ_COMMANDBUFFER:
            {
#ifdef DEBUG
                if (pNvObj->getClass() == CNvObject::NVOBJ_VERTEXBUFFER)
                {
                    CVertexBuffer *pVertexBuffer = pNvObj->getVertexBuffer();
                    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "Vertex buffer about to be destroyed (pNvObj=%08x, pVB=%08x)",
                        pNvObj,pVertexBuffer);
                }
                else
                {
                    CCommandBuffer *pCommandBuffer = pNvObj->getCommandBuffer();
                    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "Command buffer about to be destroyed (pNvObj=%08x, pCB=%08x)",
                        pNvObj,pCommandBuffer);
                }
                if (global.dwDXRuntimeVersion >= 0x0700) {
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "Destroy Buffer: pDDSLcl = %08x, handle = %02x",
                              pDestroySurfaceData->lpDDSurface,
                              pDestroySurfaceData->lpDDSurface->lpSurfMore->dwSurfaceHandle);
                }
#endif
                pNvObj->release();

                //
                // update ddraw
                //
                pDestroySurfaceData->lpDDSurface->lpGbl->fpVidMem = 0;
                SET_PNVOBJ (pDestroySurfaceData->lpDDSurface, NULL);
                break;

            }
            /*
             * unknown
             */
            default:
            {
                /*
                 * report error
                 */
                NvReleaseSemaphore(pDriverData);
                DPF ("D3DDestroyExecuteBuffer32: unknown buffer passed in (pNvObj=%08x)",pNvObj);
                dbgD3DError();
                /*
                 * fail
                 */
                pDestroySurfaceData->ddRVal = DDERR_NOTFOUND;
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }

        }  // switch

    }  // if...

    /*
     * done
     */
    NvReleaseSemaphore(pDriverData);
    pDestroySurfaceData->ddRVal = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

#ifdef NV_AGP
//
// MS proposed hack for the 6 second delay on mode switch with DX8 SDK apps
// BUGBUG We should not execute this code on Whistler
//
inline void MSAdjustAliasLock(DD_DIRECTDRAW_GLOBAL* lpDD, int iChange)
{
    if ((ppdev->OsVersionInfo.dwMajorVersion == 5ul) && (ppdev->OsVersionInfo.dwMinorVersion == 0ul)) {
        LPDWORD lpTemp = ((DWORD*)lpDD) + 386;
        *lpTemp += iChange;
    }
}
#endif

/*****************************************************************************
 * D3DLockExecuteBuffer32
 *
 * D3D callback to lock an execute, vertex or command buffer
 */
DWORD __stdcall D3DLockExecuteBuffer32
(
    LPDDHAL_LOCKDATA pLockData
)
{
    DWORD dwRetVal;

    dbgTracePush ("D3DLockExecuteBuffer32");

    /*
     * get driver context
     */
    nvSetDriverDataPtrFromDDGbl (pLockData->lpDD);

    /*
     * extract surface
     */
    CNvObject *pNvObj = GET_PNVOBJ (pLockData->lpDDSurface);
    nvAssert(pNvObj);

    switch (pNvObj->getClass())
    {
        /*
         * vertex buffer
         */
        case CNvObject::NVOBJ_VERTEXBUFFER:
        {
            CVertexBuffer *pVertexBuffer = pNvObj->getVertexBuffer();

            if (!pVertexBuffer)
            {
                NvReleaseSemaphore(pDriverData);
                DPF ("D3DLockExecuteBuffer32: cannot lock vertex buffer");
                dbgD3DError();
                pLockData->ddRVal = DDERR_NOTFOUND;
                dbgTracePop();
                return DDHAL_DRIVER_HANDLED;
            }

            // should we rename?
            BOOL bRename   = pLockData->dwFlags & DDLOCK_DISCARDCONTENTS;
            BOOL bDontWait = pLockData->dwFlags & DDLOCK_NOOVERWRITE;

                    #ifdef DX7PROPER
                    if (!bRename)
                    {
                        if (!bDontWait)
                        {
                            DPF_LEVEL(NVDBG_LEVEL_PERFORMANCE, "Vertex Buffer didn't use DDLOCK_DISCARDCONTENTS"
                                "or DDLOCK_NOOVERWRITE");
                            PF ("5. Lock called and VB without DDLOCK_DISCARDCONTENTS.");
                        }
                    }

                    if (bRename && bDontWait)
                    {
                        DPF_LEVEL(NVDBG_LEVEL_PERFORMANCE, "Vertex Buffer used both DDLOCK_DISCARDCONTENTS"
                                "and DDLOCK_NOOVERWRITE at the same time");
                    }
                    #endif

            // lock and rename
                    #ifdef NV_PROFILE_CPULOCK
                    NVP_START (NVP_T_VB_LOCK);
                    #endif

#ifdef DEBUG
            DWORD dwOldAddress, dwNewAddress;
            if (bRename) {
                dwOldAddress = pVertexBuffer->getAddress();
            }
#endif

            if (!bDontWait)
            {
                DDLOCKINDEX(NVSTAT_LOCK_VB);
#ifdef MCFD


/////// a different approach is 

// DDLOCK_DISCARDCONTENTS don't trigger a copy if a renaming occurs
// DDLOCK_NOOVERWRITE --> means READ_ONLY         

                //not finished (Paul, don't look at this code !! )

                if (pLockData->dwFlags & DDLOCK_NOOVERWRITE)
                    pVertexBuffer->LockForCPU(CSimpleSurface::LOCK_READ);
                else
                {
                    if (pLockData->dwFlags & DDLOCK_DISCARDCONTENTS)
                        pVertexBuffer->LockForCPU(CSimpleSurface::LOCK_WRITE);
                    else
                        pVertexBuffer->LockForCPU(CSimpleSurface::LOCK_WRITE, RENAME_POLICY_COPY);
                }

#else
                pVertexBuffer->cpuLock (bRename ? CSimpleSurface::LOCK_RENAME
                                                : CSimpleSurface::LOCK_NORMAL);
#endif
            }

#ifdef DEBUG
            if (bRename) {
                dwNewAddress = pVertexBuffer->getAddress();
                DPF_LEVEL (NVDBG_LEVEL_SURFACE, "VB renamed: %08x -> %08x", dwOldAddress, dwNewAddress);
            }
#endif

            // handles cases where we get dp2 calls before the unlock
            if (bRename){
                pVertexBuffer->tagModified();
            }
                    #ifdef NV_PROFILE_CPULOCK
                    NVP_STOP (NVP_T_VB_LOCK);
                    nvpLogTime (NVP_T_VB_LOCK,nvpTime[NVP_T_VB_LOCK]);
                    #endif

            /*
             * update ddraw
             */
            if (pVertexBuffer->getHeapLocation() == CSimpleSurface::HEAP_SYS)
            {
                pLockData->lpSurfData                   = (void*)pVertexBuffer->getAddress();
                pLockData->lpDDSurface->lpGbl->fpVidMem = (DWORD)pLockData->lpSurfData;
                dwRetVal = DDHAL_DRIVER_HANDLED;
            }
#ifdef WINNT
            else if (pVertexBuffer->getHeapLocation() == CSimpleSurface::HEAP_AGP)
            {
#ifdef NV_AGP
                MSAdjustAliasLock(pLockData->lpDD, 1);
                pVertexBuffer->tagAliasLocked();
                pLockData->lpSurfData = (void *)nvAGPGetUserAddr(pVertexBuffer->getAddress());
                DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Locking agp VB (%08x), fpVidMem = %08x", pVertexBuffer, pLockData->lpSurfData);
#else
                pLockData->lpDDSurface->lpGbl->fpVidMem    -= pLockData->lpDDSurface->lpGbl->fpHeapOffset; //hack
                pLockData->lpDDSurface->lpGbl->lpVidMemHeap = ppdev->AgpHeap;
                pLockData->lpDDSurface->lpGbl->fpHeapOffset = pVertexBuffer->getfpVidMem() + pDriverData->GARTLinearHeapOffset;
                pLockData->lpDDSurface->lpGbl->fpVidMem    += pLockData->lpDDSurface->lpGbl->fpHeapOffset; //hack
                pLockData->lpSurfData = (void *)pLockData->lpDDSurface->lpGbl->fpVidMem; //hack
#endif
                dwRetVal = DDHAL_DRIVER_HANDLED; //hack should be NOTHANDLED
            }
#endif
            else //video
            {
                pLockData->lpSurfData                   = (void*)pVertexBuffer->getfpVidMem();
                pLockData->lpDDSurface->lpGbl->fpVidMem = pVertexBuffer->getfpVidMem();
#if WINNT
                pLockData->lpDDSurface->lpGbl->fpHeapOffset = pVertexBuffer->getfpVidMem();
                pLockData->lpSurfData = (void *)((LPBYTE)pLockData->lpSurfData + pLockData->fpProcess); //hack
                pLockData->lpDDSurface->lpGbl->fpVidMem += pLockData->fpProcess; //hack
#endif
                dwRetVal = DDHAL_DRIVER_HANDLED; //hack should be NOTHANDLED
            }

            //
            // update our state
            //
            pVertexBuffer->tagD3DLocked();
            pVertexBuffer->bumpLockCount();
            break;
        }
        /*
         * command buffer
         */
        case CNvObject::NVOBJ_COMMANDBUFFER:
        {
            CCommandBuffer *pCommandBuffer = pNvObj->getCommandBuffer();

            if (!pCommandBuffer)
            {
                NvReleaseSemaphore(pDriverData);
                DPF ("D3DLockExecuteBuffer32: cannot lock command buffer");
                dbgD3DError();
                pLockData->ddRVal = DDERR_NOTFOUND;
                dbgTracePop();
                return DDHAL_DRIVER_HANDLED;
            }

            /*
             * lock
             */
            DDLOCKINDEX(NVSTAT_LOCK_EXECUTE_BUFFER);
            pCommandBuffer->cpuLock (CSimpleSurface::LOCK_NORMAL);

            /*
             * update ddraw
             */
            pLockData->lpSurfData                   = (void*)pCommandBuffer->getAddress();
            pLockData->lpDDSurface->lpGbl->fpVidMem = (DWORD)pLockData->lpSurfData;
            dwRetVal = DDHAL_DRIVER_HANDLED;
            break;
        }
        /*
         * unknown
         */
        default:
        {
            /*
             * report error
             */
            DPF ("D3DLockExecuteBuffer32: unknown buffer passed in (pNvObj=%08x)",pNvObj);
            dbgD3DError();
            /*
             * fail
             */
            NvReleaseSemaphore(pDriverData);
            pLockData->ddRVal = DDERR_NOTFOUND;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
     * done
     */
    NvReleaseSemaphore(pDriverData);
    pLockData->ddRVal = DD_OK;
    dbgTracePop();
    return dwRetVal;
}

/*****************************************************************************
 * D3DUnlockExecuteBuffer32
 *
 * D3D callback to unlock an execute, vertex or command buffer
 */
DWORD __stdcall D3DUnlockExecuteBuffer32
(
    LPDDHAL_UNLOCKDATA pUnlockData
)
{
    dbgTracePush ("D3DUnlockExecuteBuffer32");

    /*
     * get driver context
     */
    nvSetDriverDataPtrFromDDGbl (pUnlockData->lpDD);

    /*
     * extract surface
     */
    CNvObject *pNvObj = GET_PNVOBJ (pUnlockData->lpDDSurface);
    

    //bend attempts to unlock an invalid buffer after a few mode switches. FS
    switch (pNvObj ? pNvObj->getClass() : 0)
    {
        /*
         * vertex buffer
         */
        case CNvObject::NVOBJ_VERTEXBUFFER:
        {
            CVertexBuffer *pVertexBuffer = pNvObj->getVertexBuffer();

            if (!pVertexBuffer)
            {
                NvReleaseSemaphore(pDriverData);
                DPF ("D3DUnlockExecuteBuffer32: cannot unlock Vertex buffer");
                dbgD3DError();
                pUnlockData->ddRVal = DDERR_NOTFOUND;
                dbgTracePop();
                return DDHAL_DRIVER_HANDLED;
            }

            // unlock
#ifdef MCFD
            pVertexBuffer->UnlockForCPU();
#else
            pVertexBuffer->cpuUnlock();
#endif
            // update our state
            pVertexBuffer->tagD3DUnlocked();
            pVertexBuffer->tagModified();
            if (pVertexBuffer->getLockCount() > 1)
            {
                pVertexBuffer->destroySuperTriLookAsideBuffer();
                pVertexBuffer->tagDynamic();
            }

#ifdef NV_AGP
            if (pVertexBuffer->isAliasLocked()) {
                MSAdjustAliasLock(pUnlockData->lpDD, -1);
                pVertexBuffer->tagNotAliasLocked();
            }
            DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "Unlocking agp VB (%08x), fpVidMem = %08x", pVertexBuffer, pVertexBuffer->getAddress());
#endif

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

#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                nvglSetNv20KelvinInvalidateVertexCache (NV_DD_KELVIN);
            }
            else
#endif
#if (NVARCH >= 0x010)
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
                nvglSetNv10CelsiusInvalidateVertexCache (NV_DD_CELSIUS);
            }
#endif

            break;
        }
        /*
         * command buffer
         */
        case CNvObject::NVOBJ_COMMANDBUFFER:
        {
            CCommandBuffer *pCommandBuffer = pNvObj->getCommandBuffer();

            if (!pCommandBuffer)
            {
                NvReleaseSemaphore(pDriverData);
                DPF ("D3DUnlockExecuteBuffer32: cannot unlock command buffer");
                dbgD3DError();
                pUnlockData->ddRVal = DDERR_NOTFOUND;
                dbgTracePop();
                return DDHAL_DRIVER_HANDLED;
            }

            /*
             * unlock
             */
            pCommandBuffer->cpuUnlock();
            break;
        }
        /*
         * unknown
         */
        default:
        {
            /*
             * report error
             */
            NvReleaseSemaphore(pDriverData);
            DPF ("D3DUnlockExecuteBuffer32: unknown buffer passed in (pNvObj=%08x)",pNvObj);
            dbgD3DError();
            /*
             * fail
             */
            pUnlockData->ddRVal = DDERR_NOTFOUND;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
     * done
     */
    NvReleaseSemaphore(pDriverData);
    pUnlockData->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;
}

