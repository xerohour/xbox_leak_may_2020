/******************************Module*Header*******************************\
* Module Name: ogldrwbl.c
*
* This module contains the functions to support the OpenGL client side
* drawable interface.
*
* Copyright (c) 1998 NVidia Corporation
\**************************************************************************/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#pragma hdrstop
#include "nv32.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "oglDD.h"
#include "oglutils.h"
#include "oglflip.h"
#include "oglOverlay.h"
#include "Nvcm.h"

#if DBG
#define OglDebugPrint OglDebugPrintFunc
#else
#define OglDebugPrint
#endif

#ifndef WO_RGN_DESKTOP_COORD
#define WO_RGN_DESKTOP_COORD 0x100
#endif

static BOOL MapDrawableClientAddress(PDEV *ppdev, 
                                     NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo, 
                                     NV_SYSMEM_SHARE_MEMORY *sysmemShareMemoryOut);
static BOOL UnmapDrawableClientAddress(PDEV *ppdev,
                                       NV_OPENGL_DRAWABLE_INFO  *clientDrawableInfo);
static BOOL MapDrawableGlobalPageAddress(PDEV *ppdev,
                                         NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
static BOOL UnmapDrawableGlobalPageAddress(PDEV *ppdev, 
                                           NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
static void CopyOrigClipList(PDEV *, NV_OPENGL_DRAWABLE_INFO *);

BOOL CALLBACK ClientDrawableDrvFreeObj(DRIVEROBJ *pDriverObj);

//******************************************************************************
//
//  Function:   CreateDrawableClientInfo
//
//  Routine Description:
//
//      This routine associates a window with client supplied information.  Window
//      change events are tracked by DrvClipChanged and the client information is
//      updated.  This function is called in response to the client side escape.
//
//  Arguments:
//
//  Return Value:
//
//      NULL indicates an allocate problem; otherwise user virtual address
//
//******************************************************************************
NV_OPENGL_DRAWABLE_INFO *CreateDrawableClientInfo(
    SURFOBJ *pso,
    HWND    hWnd,
    HDC     hDC,
    PIXELFORMATDESCRIPTOR *ppfd
)
{
    int i;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    WNDOBJ                  *pwo = (WNDOBJ *)NULL;
    HDRVOBJ                 hDrvObj = (HDRVOBJ)NULL;
    PDEV                    *ppdev = (PDEV *)pso->dhpdev;
    NV_SYSMEM_SHARE_MEMORY  sysmemShareMemoryOut;
    LONG                    status;
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;
    PVOID				    origClientDrawableInfoPtr;
    FLONG                   flWindowTrackingFlags;

    DISPDBG((2, "CreateDrawableClientInfo: hWnd = 0x%p hDC = 0x%p", hWnd, hDC));

    //
    // Create window object to track changes to this window in DrvClipChanged
    //
    flWindowTrackingFlags = 
#if _WIN32_WINNT >= 0x0500
                            WO_RGN_UPDATE_ALL | WO_RGN_CLIENT | WO_RGN_DESKTOP_COORD;
#else
                            WO_RGN_CLIENT;
#endif


    // In an overlay environment we need the delta client messages to 
    // update the overlay layout to not have garbage in our overlaybuffer
    if(ppdev->bOglOverlayDesired)
    {
        flWindowTrackingFlags |= WO_RGN_CLIENT_DELTA   // to clear newly entered overlay region
                              |  WO_RGN_SURFACE_DELTA; // to clear regions without overlay in default transparent color
    }

    //
    // Workstation features need extra functionality
    //
    if(   HWGFXCAPS_UBB(ppdev) 
       && HWGFXCAPS_QUADRO_GENERIC(ppdev))
    {
        // want to get a negative clip list
        // this is only necessary while window flipping is enabled
        flWindowTrackingFlags |= WO_RGN_SURFACE;
    }

    //
    // NOTE: It is very important that all calls to engcreatewnd use the same flWindowTrackingFlags!
    //       All pathes above cannot change while ogl is running!
    //

    pwo = EngCreateWnd(pso,
                       hWnd,DrvClipChanged,
                       flWindowTrackingFlags,
                       0L);

    DISPDBG((1, "CreateDrawableClientInfo: create WNDOBJ (pwo = 0x%p)", pwo));

    clientDrawableInfo = NULL;
    if (pwo == (WNDOBJ *)0) 
    {
        //this is an error case which results in an assertion in the ICD years later
        // => assert it here !!
        ASSERT(pwo != NULL);
        DISPDBG((0, "CreateDrawableClientInfo - Failed EngCreateWnd"));
        return(NULL);
    }
    else if (pwo == (WNDOBJ *)-1)
    {
        DISPDBG((2, "CreateDrawableClientInfo - Already tracking hWnd"));
        clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
        if (clientList)
        {
            clientInfo = &clientList->clientInfo;
        }
        if (clientInfo)
        {
            clientDrawableInfo = clientInfo->clientDrawableInfo;
            clientDrawableInfo->refCount++;
            DISPDBG((1, "CreateDrawableClientInfo: hWnd = 0x%p, refCount=0x%x", hWnd, clientDrawableInfo->refCount));
            return(clientDrawableInfo->userVirtualAddress);
        }
        else
        {
            //
            // Lets see GDI is tracking the window but we can't find it in our
            // list.  This is very bad.
            //
            if (ppdev->numDevices == 1)
                {
                return(NULL);
                }
            //
            // In multi-device world we track the window on multiple devices
            // but EngCreateWnd fails.  So, just create the drawable node and
            // let DrvClipChanged update multiple nodes.  This works around
            // a limitation in EngCreateWnd.
            //
        }
    }

    /* Since we are not guaranteed page aligned allocation,
     * we need to allocate an extra PAGE_SIZE - 1 memory and use
     * memory starting from next page. */
    origClientDrawableInfoPtr = clientDrawableInfo = 
        EngAllocMem(FL_ZERO_MEMORY, (sizeof(NV_OPENGL_DRAWABLE_INFO) + PAGE_SIZE -1), ALLOC_TAG);

    if (clientDrawableInfo == NULL)
    {
        DISPDBG((2, "CreateDrawableClientInfo - Failed EngAllocMem"));
        return(NULL);
    }

    
    clientDrawableInfo = (NV_OPENGL_DRAWABLE_INFO *)PAGE_ALIGN_UP(clientDrawableInfo);
    clientDrawableInfo->origClientDrawableInfoPtr = origClientDrawableInfoPtr;

    clientDrawableInfo->bOrigClipList = FALSE; // do not copy the original clip list by default
    clientDrawableInfo->origWindowRectList = NULL;

    //
    // Initialize the clientDrawableInfo structure
    //
    clientDrawableInfo->hWnd      = hWnd;
    clientDrawableInfo->hDC       = hDC;
    clientDrawableInfo->cachedClipRectList = NULL; // It is already zero..but to clarify..
    clientDrawableInfo->overlayMember = (ppfd->bReserved != 0);

    //
    // If we created a window object then map a system address to the client
    // address and use it for accessing the structure in the display driver.
    //
    DISPDBG((1, "CreateDrawableClientInfo: SetConsumer (pv = 0x%p)", clientDrawableInfo));
    if (MapDrawableClientAddress(ppdev,
                                 clientDrawableInfo,
                                 &sysmemShareMemoryOut) == TRUE)
    {
        //
        // Get pointer to new client.
        //
        clientList = OglAppendToClientInfoList(ppdev, 
                                               0, 
                                               0, 
                                               clientDrawableInfo, 
                                               NULL);
        if (clientList)
            {
            clientInfo = &clientList->clientInfo;
            }
        else
            {
            UnmapDrawableClientAddress(ppdev, clientDrawableInfo);
            EngFreeMem(clientDrawableInfo->origClientDrawableInfoPtr);
            OglDebugPrint("CreateDrawable: OglAppendToClientInfoList: FAILED\n");
            return(NULL);
            }

        OglDebugPrint("ALLOC: clientDrawableInfo = 0x%p\n", clientDrawableInfo);

        if (MapDrawableGlobalPageAddress(ppdev, clientDrawableInfo) == FALSE)
        {
            UnmapDrawableClientAddress(ppdev, clientDrawableInfo);
            EngFreeMem(clientDrawableInfo->origClientDrawableInfoPtr);
            OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));
            OglDebugPrint("FREE: clientDrawableInfo = 0x%p\n", origClientDrawableInfoPtr);
            return(NULL);
        }

        OglDebugPrint("****************** CreateDrawable: client count = %ld\n", globalOpenGLData.oglClientCount);

        //
        // Create driver object that tracks this client index with the client process
        //
        hDrvObj = EngCreateDriverObj((PVOID)clientInfo, ClientDrawableDrvFreeObj, ppdev->hdevEng);
        if (hDrvObj == NULL)
        {
            DISPDBG((1, "CreateDrawableClientInfo: FAILED EngCreateDriverObj"));
            OglDebugPrint("****************** FAILURE CreateDrawable: client count = %ld\n", globalOpenGLData.oglClientCount);
            UnmapDrawableGlobalPageAddress(ppdev, clientDrawableInfo);
            UnmapDrawableClientAddress(ppdev, clientDrawableInfo);
            EngFreeMem(clientDrawableInfo->origClientDrawableInfoPtr);
            OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));
            OglDebugPrint("FREE: clientDrawableInfo = 0x%p\n", origClientDrawableInfoPtr);
            return(NULL);
        }
        //
        // Save driver object and mode switch count.
        //
        clientInfo->hDrvObj         = hDrvObj;
        clientInfo->modeSwitchCount = ppdev->dwGlobalModeSwitchCount;
        clientInfo->ppdev           = ppdev;
        clientInfo->pfd             = *ppfd;

        // Allocate the cached clip list now.
        clientDrawableInfo->cachedClipRectList = 
            EngAllocMem(FL_ZERO_MEMORY, 
                    (sizeof(RECTL) * MAX_OGL_CLIP_RECTS), ALLOC_TAG);

        if (clientDrawableInfo->cachedClipRectList == NULL)
        {
            DISPDBG((1, "CreateDrawableClientInfo  - Failed EngAllocMem  of CachedClipRectList"));
            OglDebugPrint("****************** FAILURE CreateDrawable: EngAllocMem  of CachedClipRectList");
            UnmapDrawableGlobalPageAddress(ppdev, clientDrawableInfo);
            UnmapDrawableClientAddress(ppdev, clientDrawableInfo);
            EngFreeMem(clientDrawableInfo->origClientDrawableInfoPtr);
            OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));
            OglDebugPrint("FREE: clientDrawableInfo = 0x%p\n", origClientDrawableInfoPtr);
            return(NULL);
        }

        clientDrawableInfo->cachedNumClipRects = 0;
        clientDrawableInfo->cachedClipListSize = MAX_OGL_CLIP_RECTS;
        clientDrawableInfo->cachedClipListDirty = TRUE;
        clientDrawableInfo->cachedExclusiveRectList = NULL; // we will allocate it if needed.
        clientDrawableInfo->cachedNumExclusiveRects = 0; 

        clientDrawableInfo->copyWindowRect = &(clientDrawableInfo->windowRect[0]);
        clientDrawableInfo->sizeCopyWindowRectList = MAX_OGL_CLIP_RECTS;

        //
        // DrvClipChanged is called with the pointer in the second
        // argument to WNDOBJ_vSetConsumer.  We pass in the display driver
        // address mapping to the clientInfo structure.
        //
        if (pwo && pwo != (WNDOBJ *)-1)
        {
            WNDOBJ_vSetConsumer(pwo, (PVOID)clientInfo);
            clientInfo->pwo = pwo;
        }

        //
        // Inititalize width and height of display for this drawable.
        //
        clientDrawableInfo->cxScreen = ppdev->cxScreen;
        clientDrawableInfo->cyScreen = ppdev->cyScreen;

        //
        // Initialize flags for this client.  Drawable is ALWAYS created before context...
        //
        clientInfo->flags = NV_OGL_CLIENT_DRAWABLE_INFO;

        if (ppdev->numDevices > 1)
        {
            if (pwo && pwo != (WNDOBJ *)-1)
            {
                // if we got a valid pwo then this is the first client to start tracking
                // a window.  this client is the one where the original unclipped window
                // rectangle list will be stored.
                clientDrawableInfo->origWindowRectList =  
                    EngAllocMem(FL_ZERO_MEMORY, sizeof(RECTL)*clientDrawableInfo->sizeCopyWindowRectList, ALLOC_TAG);
                if( NULL != clientDrawableInfo->origWindowRectList )
                {
                    clientDrawableInfo->bOrigClipList = TRUE;
                }
                else
                {
                    DISPDBG((0,"CreateDrawableClientInfo could not allocate %d byte for origWindowRectList",
                        (sizeof(RECTL)*clientDrawableInfo->sizeCopyWindowRectList) ));
                }
            }
            else
            {
                // on a multi-monitor system, the first client drawable info copies
                // the window clip list.  this list must be copied into all other 
                // client drawable info structures that are tracking the HWND.
                CopyOrigClipList(ppdev, clientDrawableInfo);
            }
        }

        OglDebugPrint("CreateDrawable = 0x%p 0x%p 0x%p\n", ppdev, clientInfo, clientDrawableInfo);
    }
    else
    {
        DISPDBG((1, "CreateDrawableClientInfo: FAILED MapDrawableClientAddress"));
        EngFreeMem(clientDrawableInfo->origClientDrawableInfoPtr);
        OglDebugPrint("FREE: clientDrawableInfo = 0x%p\n", origClientDrawableInfoPtr);
        return(NULL);
    }
    
    globalOpenGLData.oglDrawableClientCount++ ;
    
    clientDrawableInfo->refCount = 1;

    // If we have more than one opengl window than we can't flip.
    // Need to restore the DAC to point to the primary surface.
    // we are now able to decide if a previously found DDraw app is an ICD app in real
    ASSERT(NULL!=clientInfo);
    bRemovePointerFromList(&ppdev->oglDDrawSurfaceCountList,(PULONG)((ULONG_PTR)clientInfo->dwProcessID));
    ppdev->oglDDrawSurfaceCount = ulGetNumberOfPointersInList(&ppdev->oglDDrawSurfaceCountList);
    bOglPageFlipModeUpdate(ppdev);

    return(clientDrawableInfo->userVirtualAddress);
}

//******************************************************************************
//
//  Function:   DestroyDrawableClientInfo
//
//  Routine Description:
//
//      This routine frees the client drawable info structure and unbinds it
//      to the window.  This is called in reponse to the client side escape
//      or when the client side process dies.
//
//  Arguments:
//
//  Return Value:
//
//      VOID
//
//******************************************************************************
void DestroyDrawableClientInfo(
    PDEV *ppdev,
    HWND hWnd,
    HDC hDC,
    ULONG deleteFlag
)
{
    int i;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo = NULL;
    PVOID						origClientDrawableInfoPtr;

    clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }

//    if (OGL_FLIPPED()) {
//        vFlipToPrimary(ppdev);
//    }

    if ( bOglHasDrawableInfo(clientInfo) )
    {
        NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryOut;

        DISPDBG((1, "DestroyDrawableClientInfo: hWnd = 0x%p", hWnd));

        // note: on a call by the drvobj callback refCount will be explicitly
        // set to 1 to make sure all resources get freed
        // (see ClientDrawableDrvFreeObj)
        clientDrawableInfo = clientInfo->clientDrawableInfo;
        ASSERTDD(clientDrawableInfo->refCount, "DestroyDrawableClientInfo: illegal refCount!!");
        if (--clientDrawableInfo->refCount)
        {
            // still referenced ??
            DISPDBG((1, "DestroyDrawableClientInfo: hWnd = 0x%p, refCount=0x%x", hWnd, clientDrawableInfo->refCount));
            return;
        }

        // MSchwarzer 9/28/2000 remove palette from global structure if there is one
        if (   bOglIsOverlayClient(clientInfo)
            && !OglDestroyLayerPalette(hWnd) )
        {
            // no palette or error ?
            DISPDBG((0, "DestroyDrawableClientInfo: OglDestroyLayerPalette( hWnd = 0x%p) failed", hWnd));
        }

        UnmapDrawableGlobalPageAddress(ppdev, clientDrawableInfo);
        UnmapDrawableClientAddress(ppdev, clientDrawableInfo);

        //
        // Free memory associated with cached clip list of client drawable info.
        //
        if (clientDrawableInfo->cachedClipRectList)
        {
            EngFreeMem(clientDrawableInfo->cachedClipRectList);
        }

        //
        // Free memory associated with cached exclusive clip list of client drawable info.
        //
        if (clientDrawableInfo->cachedExclusiveRectList)
        {
            EngFreeMem(clientDrawableInfo->cachedExclusiveRectList);
        }
        //
        // Free memory associated with copy Clip Window rect if allocated
        //
#ifndef NV3
        if ((clientDrawableInfo->sizeCopyWindowRectList > MAX_OGL_CLIP_RECTS) && 
            (clientDrawableInfo->copyWindowRect != NULL) &&
            (clientDrawableInfo->copyWindowRect != (&(clientDrawableInfo->windowRect[0])))) {
            EngFreeMem(clientDrawableInfo->copyWindowRect);
            clientDrawableInfo->sizeCopyWindowRectList = 0;
            clientDrawableInfo->copyWindowRect = NULL;
            if (clientDrawableInfo->origWindowRectList)
                {
                EngFreeMem(clientDrawableInfo->origWindowRectList);
                clientDrawableInfo->origWindowRectList = NULL;
                }
        }
#endif // NV3

        //
        // Free memory associated with client drawable info.
        //
        origClientDrawableInfoPtr = clientDrawableInfo->origClientDrawableInfoPtr;
        EngFreeMem(clientDrawableInfo->origClientDrawableInfoPtr);
        clientDrawableInfo             = NULL;
        clientInfo->clientDrawableInfo = NULL;

        //
        // Clear drawable bit.
        //
        clientInfo->flags &= ~NV_OGL_CLIENT_DRAWABLE_INFO;

        if (deleteFlag)
            {
            //
            // Delete driver object if possible
            //
            if (OglEngDeleteDriverObj(ppdev, clientInfo, NV_OGL_CLIENT_DRAWABLE_INFO) == TRUE)
                {
                OglDebugPrint("****************** DestroyDrawable: client count = %ld\n", globalOpenGLData.oglClientCount);
                }
            }

        globalOpenGLData.oglDrawableClientCount-- ;
        
        if (globalOpenGLData.oglDrawableClientCount == 0)
            {
            OglFreeUnifiedSurfaces(ppdev);
            }

        OglDebugPrint("DestroyDrawable = 0x%p 0x%p 0x%p\n", ppdev, clientInfo, origClientDrawableInfoPtr);
    }    
}

//******************************************************************************
//
//  Function:   OglFindClientInfoFromHWndHDC
//
//  Routine Description:
//
//      Find a client information structure given a HWND and HDC.
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoFromHWndHDC(
    PDEV *ppdev, 
    HWND hWnd,
    HDC hDC
)
{
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    if (ppdev->numDevices > 1)
        {
        // Use hWnd AND hDC to find node...
        while (node)
            {
            if (node->clientInfo.clientDrawableInfo &&
                node->clientInfo.clientDrawableInfo->hWnd == hWnd &&
                node->clientInfo.clientDrawableInfo->hDC == hDC)
                {
                return(node);
                }
            node = node->next;
            }
        }
    else
        {
        // Just use hWnd to find node...
        while (node)
            {
            if (node->clientInfo.clientDrawableInfo &&
                node->clientInfo.clientDrawableInfo->hWnd == hWnd)
                {
                return(node);
                }
            node = node->next;
            }
        }
    return(NULL);
}

//******************************************************************************
//
//  Function:   OglFindClientInfoFromHWnd
//
//  Routine Description:
//
//      Find a client information structure given a HWND and HDC.
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
NV_OPENGL_CLIENT_INFO_LIST *OglFindClientInfoFromHWnd(
    PDEV *ppdev, 
    HWND hWnd
)
{
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        if (node->clientInfo.clientDrawableInfo &&
            node->clientInfo.clientDrawableInfo->hWnd == hWnd)
            {
            return(node);
            }
        node = node->next;
        }

    return(NULL);
}

//******************************************************************************
//
//  Function:   ClientDrawableDrvFreeObj
//
//  Routine Description:
//
//      This routine is the callback for freeing the driver object.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the resources are freed.
//
//******************************************************************************
BOOL CALLBACK ClientDrawableDrvFreeObj(
    DRIVEROBJ *pDriverObj
)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    LONG                  status;
    int                   i, clientIndex;
    PDEV                  *ppdev;

    clientInfo = (NV_OPENGL_CLIENT_INFO *)pDriverObj->pvObj;
    ppdev = (PDEV *)pDriverObj->dhpdev;

    OglAcquireGlobalMutex();

    OglDebugPrint("DRAWABLE ***************** DRAWABLE DrvFreeObj = 0x%p 0x%p 0x%p 0x%p\n", ppdev, clientInfo, clientInfo->clientDrawableInfo, clientInfo->clientContextInfo);

    if (OglFindClientListFromClientInfo(ppdev, clientInfo) == NULL)
        {
        // clientInfo was removed from clientList prior to this callback
        // just return because there is nothing to do...
        OglReleaseGlobalMutex();
        return(TRUE);
        }

    //
    // Cleanup drawable info.
    //
    if (clientInfo->flags & NV_OGL_CLIENT_DRAWABLE_INFO)
    {
        if (clientInfo->clientDrawableInfo)
        {
            ASSERT(bOglHasDrawableInfo(clientInfo));
            //
            // Free channel and such.
            //
            NvFree(ppdev->hDriver, 
                   clientInfo->clientDrawableInfo->hClient, 
                   NV01_NULL_OBJECT, 
                   clientInfo->clientDrawableInfo->hClient);

            // At this point we know that the user mode process is dead.
            // Therefore we can safely free any drawableInfo etc
            // => override refCount protection to free resources
            clientInfo->clientDrawableInfo->refCount = 1;

            DestroyDrawableClientInfo(ppdev, 
                                      clientInfo->clientDrawableInfo->hWnd,
                                      clientInfo->clientDrawableInfo->hDC, 
                                      FALSE);
        }
        clientInfo->flags &= ~NV_OGL_CLIENT_DRAWABLE_INFO;
        clientInfo->clientDrawableInfo = NULL; // just for good measure...
    }

    clientInfo->hDrvObj = 0;
    clientInfo->flags   = 0;

    //
    // Remove from global client list.
    //
    OglRemoveFromClientInfoList(ppdev, OglFindClientListFromClientInfo(ppdev, clientInfo));

    OglDebugPrint("****************** Drawable CALLBACK: client count = %ld\n", globalOpenGLData.oglClientCount);

    if (globalOpenGLData.oglDrawableClientCount == 0)
        {
        OglFreeUnifiedSurfaces(ppdev);
        }

    OglReleaseGlobalMutex();

    return(TRUE);
}

//******************************************************************************
//
//  Function:   MapDrawableClientAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to map the kernel, system
//      address for the shared information into the caller's address space.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the mapping was a success.
//
//******************************************************************************
static BOOL
MapDrawableClientAddress(
    PDEV                    *ppdev,
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo,
    NV_SYSMEM_SHARE_MEMORY  *sysmemShareMemoryOut
)
{
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn;
    ULONG dwReturnedDataLength;

    DISPDBG((2, "MapDrawableClientAddress - Entry"));

    sysmemShareMemoryIn.pSystemMdl     = NULL; // miniport WILL allocate MDL
    sysmemShareMemoryIn.bDontModifyMDL = FALSE;
    
    sysmemShareMemoryIn.physicalAddress = 0;
    sysmemShareMemoryIn.userVirtualAddress = (PVOID)NULL;

    sysmemShareMemoryIn.ddVirtualAddress = (PVOID)clientDrawableInfo; // kernel or system address
    sysmemShareMemoryIn.byteLength = sizeof(NV_OPENGL_DRAWABLE_INFO);

    if (EngDeviceIoControl(ppdev->hDriver,
        IOCTL_VIDEO_MAP_TO_USER,
        &sysmemShareMemoryIn,
        sizeof(NV_SYSMEM_SHARE_MEMORY),
        sysmemShareMemoryOut,
        sizeof(NV_SYSMEM_SHARE_MEMORY),
        &dwReturnedDataLength))
    {
        clientDrawableInfo->pClientInfoMdl     = (PVOID)NULL;
        clientDrawableInfo->userVirtualAddress = (PVOID)NULL;
        DISPDBG((2, "MapDrawableClientAddress - IOCTL_VIDEO_MAP_TO_USER failed"));
        return(FALSE);
    }

    //
    // Mapping succeeded, save client address information in client drawable info structure.
    //
    clientDrawableInfo->pClientInfoMdl     = (PVOID)sysmemShareMemoryOut->pSystemMdl;
    clientDrawableInfo->userVirtualAddress = (PVOID)sysmemShareMemoryOut->userVirtualAddress;

    DISPDBG((2, "MapDrawableClientAddress - Exit"));

    return(TRUE);
}

//******************************************************************************
//
//  Function:   UnmapDrawableClientAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to map the user virtual address
//      into system address space where the display driver can get to it.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the mapping was a success.
//
//******************************************************************************
static BOOL
UnmapDrawableClientAddress(
    PDEV                     *ppdev,
    NV_OPENGL_DRAWABLE_INFO  *clientDrawableInfo
)
{
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;
    ULONG dwReturnedDataLength;
    LONG status;

    DISPDBG((2, "UnmapDrawableClientAddress - Entry"));
    if (clientDrawableInfo && 
        clientDrawableInfo->pClientInfoMdl &&
        clientDrawableInfo->userVirtualAddress)
    {
        sysmemShareMemoryIn.pSystemMdl = (PVOID)clientDrawableInfo->pClientInfoMdl;
        sysmemShareMemoryIn.bDontModifyMDL = FALSE;
        sysmemShareMemoryIn.userVirtualAddress = (PVOID)clientDrawableInfo->userVirtualAddress;

        if (EngDeviceIoControl(ppdev->hDriver,
            IOCTL_VIDEO_UNMAP_FROM_USER,
            &sysmemShareMemoryIn,
            sizeof(NV_SYSMEM_SHARE_MEMORY),
            &sysmemShareMemoryOut,
            sizeof(NV_SYSMEM_SHARE_MEMORY),
            &dwReturnedDataLength))
        {
            DISPDBG((2, "UnmapDrawableClientAddress - IOCTL_VIDEO_UNMAP_FROM_USER failed"));
        }
        clientDrawableInfo->userVirtualAddress = (PVOID)NULL;
    }
    else
    {
        DISPDBG((2, "UnmapDrawableClientAddress - clientDrawableInfo NULL"));
    }

    DISPDBG((2, "UnmapDrawableClientAddress - Exit"));
    return(TRUE);
}

//******************************************************************************
//
//  Function:   MapDrawableGlobalPageAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to map the display driver OGL 
//      shared page into the client address space.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the mapping was a success.
//
//******************************************************************************
static BOOL
MapDrawableGlobalPageAddress(
    PDEV                    *ppdev,
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo
)
{
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;
    ULONG dwReturnedDataLength;

    DISPDBG((2, "MapDrawableGlobalPageAddress - Entry"));

    clientDrawableInfo->oglGlobalPagePtr = NULL;

    sysmemShareMemoryIn.pSystemMdl         = ppdev->oglGlobalPageMdl; // miniport will NOT allocate MDL
    sysmemShareMemoryIn.bDontModifyMDL     = TRUE;
    sysmemShareMemoryIn.physicalAddress    = 0;
    sysmemShareMemoryIn.userVirtualAddress = (PVOID)NULL;

    sysmemShareMemoryIn.ddVirtualAddress   = (PVOID)ppdev->oglGlobalPagePtr;
    sysmemShareMemoryIn.byteLength         = ppdev->oglGlobalPageSize;

    if (EngDeviceIoControl(ppdev->hDriver,
        IOCTL_VIDEO_MAP_TO_USER,
        &sysmemShareMemoryIn,
        sizeof(NV_SYSMEM_SHARE_MEMORY),
        &sysmemShareMemoryOut,
        sizeof(NV_SYSMEM_SHARE_MEMORY),
        &dwReturnedDataLength))
    {
        clientDrawableInfo->oglGlobalPagePtr = NULL;
        DISPDBG((2, "MapDrawableGlobalPageAddress - IOCTL_VIDEO_MAP_TO_USER failed"));
        return(FALSE);
    }

    // save user address in context info structure
    clientDrawableInfo->oglGlobalPagePtr = (PVOID)sysmemShareMemoryOut.userVirtualAddress;

    DISPDBG((2, "MapDrawableGlobalPageAddress - Exit"));

    return(TRUE);
}

//******************************************************************************
//
//  Function:   UnmapDrawableGlobalPageAddress
//
//  Routine Description:
//
//      This routine calls the miniport driver to unmap the display driver
//      OGL shared page of memory.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE indicates that the unmapping was a success.
//
//******************************************************************************
static BOOL
UnmapDrawableGlobalPageAddress(
    PDEV                   *ppdev,
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo
)
{
    NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;
    ULONG dwReturnedDataLength;
    LONG status;

    DISPDBG((2, "UnmapDrawableGlobalPageAddress - Entry"));
    if (clientDrawableInfo && 
        clientDrawableInfo->oglGlobalPagePtr)
    {
        sysmemShareMemoryIn.userVirtualAddress = (PVOID)clientDrawableInfo->oglGlobalPagePtr;
        sysmemShareMemoryIn.pSystemMdl         = ppdev->oglGlobalPageMdl; // miniport will NOT allocate MDL
        sysmemShareMemoryIn.bDontModifyMDL     = TRUE;  // miniport will NOT free MDL

        if (EngDeviceIoControl(ppdev->hDriver,
            IOCTL_VIDEO_UNMAP_FROM_USER,
            &sysmemShareMemoryIn,
            sizeof(NV_SYSMEM_SHARE_MEMORY),
            &sysmemShareMemoryOut,
            sizeof(NV_SYSMEM_SHARE_MEMORY),
            &dwReturnedDataLength))
        {
            DISPDBG((2, "UnmapDrawableGlobalPageAddress - IOCTL_VIDEO_UNMAP_FROM_USER failed"));
            clientDrawableInfo->oglGlobalPagePtr = NULL;
            return(FALSE);
        }
    
        clientDrawableInfo->oglGlobalPagePtr = NULL;
    }
    else
    {
        DISPDBG((2, "UnmapDrawableGlobalPageAddress - clientDrawableInfo NULL"));
    }

    DISPDBG((2, "UnmapDrawableGlobalPageAddress - Exit"));
    return(TRUE);
}


static void
CopyOrigClipList(
    PDEV                    *ppdev,
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo
)
{
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *cdiNode = NULL;

    // Document assumptions by using ASSERT...
    ASSERT(ppdev);
    ASSERT(ppdev->numDevices > 1);
    ASSERT(clientDrawableInfo);
    ASSERT(clientDrawableInfo->bOrigClipList == FALSE);
    ASSERT(clientDrawableInfo->origWindowRectList == NULL);

    OglAcquireGlobalMutex();

    cdiNode = globalOpenGLData.oglClientListHead.clientList;
    while (cdiNode)
    {
        clientInfo = &cdiNode->clientInfo;
        if (bOglHasDrawableInfo(clientInfo) &&
            clientInfo->clientDrawableInfo != clientDrawableInfo &&
            clientInfo->clientDrawableInfo->hWnd == clientDrawableInfo->hWnd && // same HWND
            clientInfo->clientDrawableInfo->bOrigClipList == TRUE && // 1st drawable with orignal unclipped rectlist
            clientInfo->clientDrawableInfo->origWindowRectList != NULL) // origWindowRectList allocated
        {
            // cdiNode contains the original unclipped window rectangle list
            // copy it to clientDrawableInfo
            // copy windowrect, too !
            clientDrawableInfo->numClipRects = clientInfo->clientDrawableInfo->numClipRects;
            clientDrawableInfo->rect = clientInfo->clientDrawableInfo->rect;
            OglAllocateAndCopyWindowClipList(ppdev, 
                                             clientDrawableInfo,
                                             clientInfo->clientDrawableInfo->origWindowRectList,
                                             clientInfo->clientDrawableInfo->numClipRects);
            break;
        }

        cdiNode = cdiNode->next;
    }

    OglReleaseGlobalMutex();
}

