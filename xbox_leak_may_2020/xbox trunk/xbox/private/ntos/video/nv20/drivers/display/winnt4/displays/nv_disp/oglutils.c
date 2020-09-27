//*****************************Module*Header*******************************
// Module Name: oglutils.c
//
// all little opengl helperfunctions should reside here
// Copyright (c) 2000 NVidia Corporation
//*************************************************************************

#include "precomp.h"
#include "driver.h"
#include "RectUtils.h"
#include "oglDD.h"
#include "oglflip.h"
#include "nvdoublebuf.h"
#include "oglstereo.h"
#include "ogloverlay.h"
#include "cliplist.h"
#include "pixelfmt.h"
#include "oglclip.h"
#include "Nvcm.h"
#include "nvReg.h"
#ifdef NV_MAIN_MULTIMON
#include "oglutils.h"
#include "linkedlist.h"
#endif

#ifdef NVD3D
    #include "dx95type.h"
    #include "ddmini.h"
#endif  


//
// export
//
#include "oglutils.h"

GLOBAL_OPENGL_DATA globalOpenGLData = { NULL, 0, 0, 0 };
ULONG globalOpenGLMutex = 0;
ULONG globalOpenGLMutexCount = 0;

//*************************************************************************
// For details regarding NV_OglDDrawSurfaceCreated/Destroyed, please see
// the comment in oglDD.h regarding OGL_MAX_DDRAW_SURFACES_FOR_FLIPPING
//*************************************************************************
#if !i386 || _WIN32_WINNT >= 0x0500
typedef HANDLE (*P_PS_GET_CURRENT_PROCESS_ID)(void);
#endif

#pragma warning(disable: 4035)  // no return value
DWORD GetCurrentProcessID(VOID)
{
#if i386 && _WIN32_WINNT >= 0x0400
    __asm
    {
        mov eax, fs:[0x18]
        mov eax, [eax + 0x20] // return value in eax
    }
#else
    HANDLE hPID = NULL;
    HANDLE hDLL;

    hDLL = EngLoadImage(L"NTOSKRNL.EXE");
    if (hDLL)
    {
        P_PS_GET_CURRENT_PROCESS_ID pfPsGetCurrentProcessId;

        pfPsGetCurrentProcessId = (P_PS_GET_CURRENT_PROCESS_ID)EngFindImageProcAddress(hDLL, "PsGetCurrentProcessId");
        if (pfPsGetCurrentProcessId)
        {
            // does NOT work in FREE mode
            hPID = pfPsGetCurrentProcessId();
        }
        EngUnloadImage(hDLL);
    }
    return (DWORD)((DWORD_PTR)hPID);
#endif
}
#pragma warning(default: 4035)  // no return value



//*************************************************************************
// bAddProcessIDToDDrawCounterList
//
// Function removes all stored ProcessId's belonging to an 
// ICD app from list. Then Adds a given ProcessID to 
// DDrawCounterList in ppdev if it does not belong to an ICD.
//
// returns TRUE  if ProcessID was successfully added to list( came from DDraw app )
//         FALSE if ProcessID belongs to ICD app or if there was no space left in list
//
//*************************************************************************
BOOL bAddProcessIDToDDrawCounterList( PPDEV ppdev, ULONG ulProcessID )
{
    NV_OPENGL_CLIENT_INFO *pClientInfo     = NULL;
    BOOL                   bRet            = FALSE;
    BOOL                   bIsICDProcessID = FALSE;

    ASSERT(ppdev);

    while( bOglGetNextClient(&pClientInfo) )
    {
        if( pClientInfo->dwProcessID == ulProcessID )
        {
            bIsICDProcessID = TRUE;
        }

        // remove processid from list if it belongs to an ICD
        while(bRemovePointerFromList( &ppdev->oglDDrawSurfaceCountList, (PULONG)((ULONG_PTR)pClientInfo->dwProcessID)) )
        {
            // remove each stored link to this ICD client
            ;
        }
    }
    // now all ProcessID's belonging to an ICD-app should have been removed from list

    // add given ProcesId to DDrawSurfaceCountList if it does not belong to an ICD
    if( !bIsICDProcessID )
    {
        if(!bAddPointerToList( &ppdev->oglDDrawSurfaceCountList, (PULONG)((ULONG_PTR)ulProcessID)))
        {
            DISPDBG((0,"bAddProcessIDToDDrawCounterList: no space left for ProcessIDCount"));
            goto Exit;
        }

        // added ProcessID to list => ID call came from DDraw app
        bRet = TRUE;
    }
    // now number of saved ProcessID's is number of DDrawappps

Exit:

    return bRet;
}

//*************************************************************************
// bRemoveProcessIDFromDDrawCounterList
//
// Function removes all given ProcessId from list. 
// If ProcessID is not in list and does not belong
// to an ICD Client, then complete list is erased
// because this case indicates us that winlogon.exe
// or csrss.exe or rundll32.exe called DisableAllDirectDrawObject
// to perform  a CTRL-ALT-DEL / Fullscreen DOS-Box 
// or a modeswitch.
//
// returns TRUE  if ProcessID was removed from list( came from DDraw app )
//         FALSE never at this point of implementation
//
//*************************************************************************
BOOL bRemoveProcessIDFromDDrawCounterList( PPDEV ppdev, ULONG ulProcessID )
{
    NV_OPENGL_CLIENT_INFO *pClientInfo     = NULL;
    BOOL                   bRet            = FALSE;
    BOOL                   bIsICDProcessID = FALSE;

    ASSERT(ppdev);

    // did ulProcessID reside in List ?
    if( !bRemovePointerFromList(&ppdev->oglDDrawSurfaceCountList,(PULONG)((ULONG_PTR)ulProcessID)) )
    {
        // check if ulProcessID belongs to an ICD app
        while( bOglGetNextClient(&pClientInfo) )
        {
            if( pClientInfo->dwProcessID == ulProcessID )
            {
                bIsICDProcessID = TRUE;
            }
        }

        if( bIsICDProcessID )
        {
            // an ICD apps is destroying its ddraw context => everything OK
            bRet = TRUE;
        }
        else
        {
            // if we reach this point, either winlogon.exe
            // or csrss.exe or rundll32.exe called DisableAllDirectDrawObject
            // => empty list
            bClearPointerList(&ppdev->oglDDrawSurfaceCountList);

            bRet = TRUE;
        }
    }
    else
    {
        bRet = TRUE;
    }

    return bRet;
}




//*************************************************************************
//  NV_OglDDrawSurfaceCreated
//
//  This function is called when a new DDraw/D3D surface may have been
//  created which may force us to switch off windowflipping.
//*************************************************************************

void __cdecl NV_OglDDrawSurfaceCreated(PDEV *ppdev)
{
    ASSERT(ppdev);

    if( (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC) )
    {
        // look ahead! function automatically removes all ICD processes from list
        // => update counter and PageFlipMode allways
        bAddProcessIDToDDrawCounterList(ppdev,GetCurrentProcessID());

        ppdev->oglDDrawSurfaceCount = ulGetNumberOfPointersInList(&ppdev->oglDDrawSurfaceCountList);
        bOglPageFlipModeUpdate(ppdev);
    }
}

//*************************************************************************
//  NV_OglDDrawSurfaceDestroyed
//
//  This function is called when a DDraw/D3D surface may have been destroyed
//  which may allow us to switch on windowflipping.
//*************************************************************************
void __cdecl NV_OglDDrawSurfaceDestroyed(PDEV *ppdev)
{
    ASSERT(ppdev);

    if( (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC) )
    {
        bRemoveProcessIDFromDDrawCounterList(ppdev,GetCurrentProcessID());
    
        ppdev->oglDDrawSurfaceCount = ulGetNumberOfPointersInList(&ppdev->oglDDrawSurfaceCountList);
        bOglPageFlipModeUpdate(ppdev);
    }
}


//*************************************************************************
// bOglDirectDrawAppRunning
// 
// Returns TRUE if from the OGLs perspective a concurring 
// directdraw or direct3d app is running.
//
// NOTE: After a modeswitch the complete counting is destroyed:
//       ppdev->pDriverData->DDrawVideoSurfaceCount goes to negative values
//       ppdev->oglDDrawSurfaceCount only has one count per ogl
//       ppdev->DDrawVideoSurfaceCount isn't used at all
//
//*************************************************************************
BOOL bOglDirectDrawAppRunning(PPDEV ppdev)
{
    return(ppdev->oglDDrawSurfaceCount > 0);

#pragma message("  MSchwarzer 12/12/2000:  look at this. Optimize for NT4!")
// W2K should work fine now
// On NT4 CTRL-ALT-DEL kill us because it releases all DDraw surfaces but does
// not realloc them :-((
// Fullscreen DOS-Box and Modeswitch works fine now
}


//*************************************************************************
// bOglPOWERlibAppRunning
// 
// Returns TRUE if from the OGLs perspective a concurring 
// POWERlib/POWERdraft is running.
//*************************************************************************
BOOL bOglPOWERlibAppRunning(PPDEV ppdev)
{
    return (FALSE);
}


//*************************************************************************
// bOglHasDrawableInfo
// returns TRUE if the given client has a clientDrawableInfo linked to it.
//*************************************************************************
BOOL bOglHasDrawableInfo(NV_OPENGL_CLIENT_INFO *clientInfo)
{
    if (clientInfo)
    {
        // Need a valid pointer to make my tests!
        ASSERT(OglFindClientListFromClientInfo(NULL, clientInfo));
        return (   (clientInfo->clientDrawableInfo)
                && (clientInfo->flags & NV_OGL_CLIENT_DRAWABLE_INFO) );
    }
    return (FALSE);
}


//*************************************************************************
// bOglIsVisibleClient
// returns TRUE if the client is visible.
//*************************************************************************
BOOL bOglIsVisibleClient(NV_OPENGL_CLIENT_INFO *clientInfo)
{
    return (   bOglHasDrawableInfo(clientInfo)
            && !bRclIsEmpty((RECTL *)&clientInfo->clientDrawableInfo->rect)
            && !(clientInfo->clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_OCCLUDED) );
}


//*************************************************************************
// bOglIsUnclippedClient
// returns TRUE if the client is visible and not clipped
//*************************************************************************
BOOL bOglIsUnclippedClient(NV_OPENGL_CLIENT_INFO *clientInfo)
{
    return (   bOglIsVisibleClient(clientInfo)
            && (   (clientInfo->clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_SINGLERECTANGLE)
                || (clientInfo->clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_FULLYVISIBLE)
                || (clientInfo->clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_FULLSCREEN) ) );
}


//*************************************************************************
// bOglIsFullscreenClient
// returns TRUE if the client is running in fullscreen mode
//*************************************************************************
BOOL bOglIsFullscreenClient(NV_OPENGL_CLIENT_INFO *clientInfo)
{
    return (   bOglIsVisibleClient(clientInfo)
            && (clientInfo->clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_FULLSCREEN) );
}


//*************************************************************************
// bOglIsComplexClippedClient
// 
// returns TRUE if the client is complex clipped. Complex clipped does mean
// it has more clip rect than the hardware can process.
//*************************************************************************
BOOL bOglIsComplexClippedClient(
    IN PPDEV                  ppdev, 
    IN NV_OPENGL_CLIENT_INFO *clientInfo)
{
    BOOL bRet = FALSE; // no it isn't

    ASSERT(NULL!=ppdev);
    ASSERT(bOglHasDrawableInfo(clientInfo));

    if (    bOglIsVisibleClient(clientInfo)      // unvisible -> simple, occluded
         && !bOglIsUnclippedClient(clientInfo)   // unclipped -> simple
         && clientInfo->clientDrawableInfo->numClipRects > HWGFXCAPS_MAXCLIPS(ppdev))
    {
        // could do some checks if exclusive rects would help us,
        // but for now assume we are complex clipped.
        bRet = TRUE;
    }

    return (bRet);
}


//*************************************************************************
// bOglGetNextClient
//
// finds next OpenGL clientinfo that contains a drawable starting with 
// next client after *ppClientInfo or at top of list if *ppClientInfo==NULL. 
//
// ppClientInfo == NULL: Only check if there is a client
// ppClientInfo != NULL: In addition return found client
//                       *ppClientInfo == NULL: Start search at top of 
//                                              client list
//                       *ppClientInfo != NULL: Start search with given 
//                                              client (not included)
// Returns TRUE if a client was found and returns a pointer to the 
//              clientInfo in *ppClientInfo if ppClientInfo!=NULL.
// Returns FALSE if client wasn't found, *ppClientInfo isn't touched!
//*************************************************************************
BOOL bOglGetNextClient(
    IN OUT NV_OPENGL_CLIENT_INFO **ppClientInfo)
{
    BOOL                        bRet            = FALSE;
    NV_OPENGL_CLIENT_INFO_LIST *pClientInfoList;
    BOOL                        bSearchStart   ;

    pClientInfoList = globalOpenGLData.oglClientListHead.clientList;

    bSearchStart = (   (NULL != ppClientInfo)     // valid pointer
                    && (NULL != *ppClientInfo) ); // initialized for search?

    while (pClientInfoList)
    {
        if (bSearchStart)            
        {
            // need to find start of serach first.
            if (&pClientInfoList->clientInfo == *ppClientInfo)
            {
                // Stop search for head of list and start 
                // search for client with next element
                bSearchStart = FALSE;
            }

        }
        else if (bOglHasDrawableInfo(&pClientInfoList->clientInfo))
        {
            // if wanted return found client
            if (NULL != ppClientInfo) 
            {
                *ppClientInfo = &pClientInfoList->clientInfo;
            }
                
            bRet = TRUE;
            break;
        }

        pClientInfoList = pClientInfoList->next;
    }

    return bRet;
}


//*************************************************************************
// bOglGetNextVisibleClient
//
// finds next visible OpenGL client starting with 
// next client after *ppClientInfo or at top of list if *ppClientInfo==NULL. 
//
// ppClientInfo == NULL: Only check if there is a client
// ppClientInfo != NULL: In addition return found client
//                       *ppClientInfo == NULL: Start search at top of 
//                                              client list
//                       *ppClientInfo != NULL: Start search with given 
//                                              client (not included)
// Returns TRUE if a visible client was found and returns a pointer to the 
//              clientInfo in *ppClientInfo if ppClientInfo!=NULL.
// Returns FALSE if client wasn't found, *ppClientInfo isn't touched!
//*************************************************************************
BOOL bOglGetNextVisibleClient(
    IN OUT NV_OPENGL_CLIENT_INFO **ppClientInfo)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    BOOL                   bRet = FALSE;

    // use local copy to query bOglGetNextVisibleClient!
    if (NULL==ppClientInfo)
        clientInfo = NULL;
    else
        clientInfo = *ppClientInfo; // can be NULL!

    while (bOglGetNextClient(&clientInfo))
    {
        // found visible client, but is it a flip candiate, too?
        if (bOglIsVisibleClient(clientInfo))
        {
            // if wanted, return found client
            if (NULL != ppClientInfo) 
            {
                *ppClientInfo = clientInfo;
            }
            bRet = TRUE;
            break;
        }
    }

    return (bRet);
}


//*************************************************************************
// cOglVisibleClients
// 
// Returns number of visible opengl clients. 0==no client or all clients 
// occluded.
//*************************************************************************
ULONG cOglVisibleClients(PPDEV ppdev)
{
    BOOL                   bFound     = TRUE;
    ULONG                  ulRet      = 0;
    NV_OPENGL_CLIENT_INFO *clientInfo;

    ASSERT(NULL!=ppdev);

    clientInfo = NULL; // Start search at top of chained list

    while (bOglGetNextVisibleClient(&clientInfo))
    {
        ulRet++;
    }

    return (ulRet);
}


//*************************************************************************
// cOglWindows
// 
// Returns count of/number of OpenGL windows, drawables.
//*************************************************************************
ULONG cOglWindows(PPDEV ppdev)
{
    ULONG                 ulRet=0;
    NV_OPENGL_CLIENT_INFO *clientInfo = NULL;

    ASSERT(NULL!=ppdev);

    clientInfo = NULL;

    while (bOglGetNextClient(&clientInfo))
    {
        ulRet++;
    }

    ASSERT(0==ulRet || OglIsEnabled(ppdev));
    ASSERT(ulRet <= globalOpenGLData.oglClientCount);
    ASSERT(ulRet == globalOpenGLData.oglDrawableClientCount);

    return (ulRet);
}



//******************************************************************************
//
//  Function: CreateOglGlobalMemory
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************

void CreateOglGlobalMemory(
PDEV *ppdev)

    {
    NV_OPENGL_GLOBAL_DATA *nvGlobalData;

    ppdev->oglGlobalPageSize = PAGE_SIZE * BYTES_TO_PAGES(sizeof(ULONG));
    /* Since we are not guaranteed page aligned allocation,
     * we need to allocate an extra PAGE_SIZE - 1 memory and use
     * memory starting from next page. */
    ppdev->origOglGlobalPagePtr = ppdev->oglGlobalPagePtr =
        EngAllocMem(FL_ZERO_MEMORY, (ppdev->oglGlobalPageSize + PAGE_SIZE - 1), ALLOC_TAG);

    if (ppdev->oglGlobalPagePtr != NULL)
        {
        ppdev->oglGlobalPagePtr = PAGE_ALIGN_UP(ppdev->oglGlobalPagePtr);

        DISPDBG((8, "CreateOglGlobalMemory OGLGPPTR = 0x%p, OrigPtr = 0x%p",ppdev->oglGlobalPagePtr, ppdev->origOglGlobalPagePtr));

        if (OglLockDownMemory(ppdev, ppdev->oglGlobalPageSize) == FALSE)
            {
            // NO OpenGL acceleration...
            EngFreeMem(ppdev->origOglGlobalPagePtr);
            ppdev->oglGlobalPagePtr = NULL;
            ppdev->origOglGlobalPagePtr = NULL;
            ppdev->oglMutexPtr      = NULL;
            ppdev->oglModeSwitchPtr = NULL;
            ppdev->bReleaseOglMutex = 0;
            return;
            }
        }
    else
        {
        DISPDBG((2, "CreateOglGlobalMemory - Failed EngAllocMem"));
        }

    nvGlobalData = (NV_OPENGL_GLOBAL_DATA *)ppdev->oglGlobalPagePtr;
    if (nvGlobalData)
        {
        ppdev->oglMutexPtr      = &nvGlobalData->oglMutex;
        ppdev->oglModeSwitchPtr = &nvGlobalData->oglModeSwitch;
        }

    ppdev->bReleaseOglMutex = 0;

    ppdev->oglLastChannel = 0;
}

//******************************************************************************
//
//  Function: DestroyOglGlobalMemory
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************
void DestroyOglGlobalMemory(
PDEV *ppdev)

    {
    if (ppdev->oglGlobalPageMdl)
        {
        OglUnlockMemory(ppdev);
        }
    ppdev->oglGlobalPageMdl = NULL;
    if ((ppdev->oglGlobalPagePtr) && (ppdev->origOglGlobalPagePtr))
        {
        EngFreeMem(ppdev->origOglGlobalPagePtr);
        }

    ppdev->oglGlobalPagePtr = NULL;
    ppdev->origOglGlobalPagePtr = NULL;
    ppdev->oglMutexPtr      = NULL;
    ppdev->oglModeSwitchPtr = NULL;
    ppdev->bReleaseOglMutex = 0;
}

//******************************************************************************
//
//  Function: OglDisableModeSwitchUpdate
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//
//******************************************************************************

void OglDisableModeSwitchUpdate(
PDEV *ppdev)

    {
    int i;
    ppdev->dwGlobalModeSwitchCount = ppdev->dwGlobalModeSwitchCount + 2;
    ppdev->dwGlobalModeSwitchCount |= 0x1;
    if (ppdev->oglModeSwitchPtr) *ppdev->oglModeSwitchPtr = ppdev->dwGlobalModeSwitchCount;

    if (ppdev->bOglSingleBackDepthCreated ||
        ppdev->bOglOverlaySurfacesCreated)  
        {
        // Free single-back-depth - client side must recover this...
        OglFreeUnifiedSurfaces(ppdev);
        }
    } // end of OglDisableModeSwitchUpdate

//******************************************************************************
//
//  Function: OglEnableModeSwitchUpdate
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//
//******************************************************************************

void OglEnableModeSwitchUpdate(
PDEV *ppdev)

    {
    ppdev->dwGlobalModeSwitchCount = ppdev->dwGlobalModeSwitchCount + 2;
    ppdev->dwGlobalModeSwitchCount &= 0xFFFFFFFE;
    if (ppdev->oglModeSwitchPtr) *ppdev->oglModeSwitchPtr = ppdev->dwGlobalModeSwitchCount;

    //******************************************************************************
    // Wait for other DMA channels to drain at this point.
    //******************************************************************************
    // Check pfnWaitForChannelSwitch for NULL pointer because in bAssertModeHardware
    // the driver call OglEnableModeSwitchUpdate and the ppdev function pointers are
    // not setup yet.  The function pointers should be setup first before calling
    // bAssertModeHardware...
    //******************************************************************************

    if (OglIsEnabled(ppdev) && ppdev->pfnWaitForChannelSwitch)
        {
        ppdev->pfnWaitForChannelSwitch(ppdev);
        ppdev->NVFreeCount = 0;
        }
    } // end of OglEnableModeSwitchUpdate

//******************************************************************************
//
//  Function: OglLockDownMutexMemory
//
//  Routine Description:
//
//     Lock down memory shared between display driver and client side.
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//
//******************************************************************************
BOOL OglLockDownMemory(
    PDEV *ppdev,
    ULONG numBytes)

    {
    NV_LOCK_DOWN_MEMORY lockDownMemoryIn;
    NV_LOCK_DOWN_MEMORY lockDownMemoryOut;
    ULONG dwReturnedDataLength;

    DISPDBG((2, "OglLockDownMemory - Entry"));

    lockDownMemoryIn.virtualAddress = (PVOID)ppdev->oglGlobalPagePtr;
    lockDownMemoryIn.byteLength     = ROUND_TO_PAGES(numBytes);
    lockDownMemoryIn.pMdl           = NULL;

    if (EngDeviceIoControl(ppdev->hDriver,
        IOCTL_VIDEO_LOCK_DOWN_MEMORY,
        &lockDownMemoryIn,
        sizeof(NV_LOCK_DOWN_MEMORY),
        &lockDownMemoryOut,
        sizeof(NV_LOCK_DOWN_MEMORY),
        &dwReturnedDataLength))
    {
        DISPDBG((2, "OglLockDownMemory - IOCTL_VIDEO_LOCK_DOWN_MEMORY failed"));
        ppdev->oglGlobalPageMdl = NULL;
        return(FALSE);
    }

    ppdev->oglGlobalPageMdl = lockDownMemoryOut.pMdl;

    DISPDBG((2, "OglLockDownMemory - Exit"));

    return(TRUE);
    }

//******************************************************************************
//
//  Function: OglUnlockMemory
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//
//******************************************************************************
BOOL OglUnlockMemory(
PDEV *ppdev)

    {
    NV_UNLOCK_MEMORY unlockMemoryIn;
    NV_UNLOCK_MEMORY unlockMemoryOut;
    ULONG dwReturnedDataLength;

    DISPDBG((2, "OglUnlockMemory - Entry"));

    unlockMemoryIn.pMdl = (PVOID)ppdev->oglGlobalPageMdl;

    if (EngDeviceIoControl(ppdev->hDriver,
        IOCTL_VIDEO_UNLOCK_MEMORY,
        &unlockMemoryIn,
        sizeof(NV_UNLOCK_MEMORY),
        &unlockMemoryOut,
        sizeof(NV_UNLOCK_MEMORY),
        &dwReturnedDataLength))
    {
        DISPDBG((2, "OglUnlockMemory - IOCTL_VIDEO_SYSMEM_SHARE_MEMORY failed"));
        return(FALSE);
    }

    DISPDBG((2, "OglUnlockMemory - Exit"));

    return(TRUE);
    }

//******************************************************************************
//
// Function: OglAcquireGlobalMutex
//
// Routine Description:
//
//     Acquire the mutex that protects the OpenGL global variables.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
VOID OglAcquireGlobalMutex(void)
{
    LONGLONG stime, ctime, dtime;
    ULONG    *oglMutexPtr;
    ULONG     key;
    ULONG     firstTime;

    oglMutexPtr = &globalOpenGLMutex;

    firstTime = 0;
    key = 1;
    do
        {
        NV_EXCHANGE(oglMutexPtr, key);
        if (key == 1)
            {

            if (firstTime == 0)
                {
                firstTime = 1;
                EngQueryPerformanceCounter((LONGLONG *)&stime);
                }
            else
                {

                EngQueryPerformanceCounter((LONGLONG *)&ctime);
                dtime = (ctime - stime);
                if (dtime >= 3) // 3 seconds
                    {
                    // TIMEOUT force reset of mutex
                    *oglMutexPtr = 1;                // report mutex acquired
                    key = 0;                         // force out of loop
                    }
                }
            }
        }
    while (key == 1);

    globalOpenGLMutexCount = 1;
}

//******************************************************************************
//
// Function: OglReleaseGlobalMutex
//
// Routine Description:
//
//     Release the mutex that protects OpenGL global variables
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
VOID OglReleaseGlobalMutex(void)
{
    globalOpenGLMutex = 0;
    globalOpenGLMutexCount = 0;
}


//*************************************************************************
// bOglServicesDisable
// 
// free everything that was allocated to run opengl especially stuff
// allocated in bOglServicesInit
//
// returns TRUE  if succeeded
//         FALSE if failed
//*************************************************************************
BOOL bOglServicesDisable(PPDEV ppdev)
{
    ASSERT (NULL!=ppdev);


    if ((ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC))
    {
        bOglNegativeClipListDestroy(ppdev);
    }

    OglDisableModeSwitchUpdate(ppdev);

    if (NULL != ppdev->pOglServer)
    {
        EngFreeMem(ppdev->pOglServer);
        ppdev->pOglServer = NULL;
    }

    // for now cannot fail
    return (TRUE);
}


//*************************************************************************
// bOglServicesInit
// 
// Initialize everything needed to run the OpenGL server side.
// This has to be called after a ppdev has been created before the first
// OpenGL app is started.
//
// Call bOglServicesDisable to free this stuff again.
//
// returns TRUE  if succeeded
//         FALSE if failed
//*************************************************************************
BOOL bOglServicesInit(PPDEV ppdev)
{
    BOOL bRet = FALSE; // default error exit

    ASSERT (NULL!=ppdev);

    ppdev->pOglServer = EngAllocMem(FL_ZERO_MEMORY, sizeof(OGLSERVER), ALLOC_TAG);
    if (NULL == ppdev->pOglServer)
    {
        DBG_ERROR("bOglServicesInit: failed to initialize ppdev->pOglServer");
        goto Exit;
    }

    OglEnableModeSwitchUpdate(ppdev);

    NV_InitDoubleBufferSavedProcs(ppdev);

    if ((ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC))
    {
        bOglStereoSurfacesAllowed(ppdev);
        bOglOverlaySurfacesAllowed(ppdev);

        bOglNegativeClipListCreate(ppdev);
    }

    bRet = TRUE;
Exit:
    return (bRet);
}

#if (NVARCH >= 0x4)
//*************************************************************************
// OglReadRegDword
//
// Utility function for OglReadRegistry to read one opengl registry
// entry.  If this fails or if the registry does not exist then
// outVal is not modified
//*************************************************************************

static void OglReadRegDword(
    PDEV *ppdev,
    CHAR *rawName,
    DWORD *outVal)
{
    CHAR fullName[NV_MAX_REG_KEYNAME_LEN + 4];
    WCHAR keyName[NV_MAX_REG_KEYNAME_LEN];
    DWORD fullNameLen, keyNameLen;
    DWORD retStatus;
    DWORD keyVal, keyValSize;

    /* Build a wide name */
    fullName[0] = 0;
    strcat(fullName, "OGL_");
    strcat(fullName, rawName);
    fullNameLen = strlen(fullName)+1;
    ASSERTDD(fullNameLen <= NV_MAX_REG_KEYNAME_LEN, "Key name too long");

    EngMultiByteToUnicodeN(keyName,
                           NV_MAX_REG_KEYNAME_LEN*sizeof(WCHAR),
                           &keyNameLen,
                           (PCHAR)fullName,
                           fullNameLen*sizeof(CHAR));

    retStatus = EngDeviceIoControl(ppdev->hDriver,
                                   IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                                   (LPVOID)keyName,
                                   keyNameLen,
                                   (LPVOID)&keyVal,
                                   sizeof(DWORD),
                                   &keyValSize);

    if (retStatus == NO_ERROR) {
        *outVal = keyVal;
    } 
}

//*************************************************************************
// OglReadRegistry
//
// Reads in opengl related regisry settings into ppdev->oglRegXXXX
// entries into the pdev.  This calls into the shared library
// to initialize default registry values consistent with the ICD and
// also calls into the shared library to hack the registry values on
// a per chip/chip type/OS basis.
//*************************************************************************

void OglReadRegistry(
    PDEV *ppdev)
{
    DWORD doUnifiedBuffers;
    DWORD doWindowFlipping;
    DWORD flippingControl;
    DWORD applicationKey;
    DWORD appSupportBits;
    DWORD overlaySupport;
    DWORD apiStereoSupport;
    DWORD apiStereoMode;
#if 0    
    // Grab the defaults from the opengl shared library to be consistent with
    // the ICD.
    __glNVLoadRegistryDefaults(ppdev->hwGfxCaps,
                               &doUnifiedBuffers,
                               &doWindowFlipping,
                               &flippingControl,
                               &applicationKey,
                               &appSupportBits,
                               &overlaySupport,
                               &apiStereoSupport,
                               &apiStereoMode);
#endif
    OglReadRegDword(ppdev, NV_REG_OGL_SINGLE_BACKDEPTH_BUFFER, &doUnifiedBuffers);
    OglReadRegDword(ppdev, NV_REG_OGL_WINDOW_FLIPPING, &doWindowFlipping);
    OglReadRegDword(ppdev, NV_REG_OGL_OVERLAY_SUPPORT, &overlaySupport);
    OglReadRegDword(ppdev, NV4_REG_OGL_BUFFER_FLIPPING_CONTROL, &flippingControl);
    OglReadRegDword(ppdev, NV_REG_OGL_STEREO_SUPPORT, &apiStereoSupport);
    OglReadRegDword(ppdev, NV_REG_OGL_API_STEREO_MODE, &apiStereoMode);
#if 0
    // Apply per OS per chip hacks to the registry through the shared library to
    // be consistent with the ICD.
    __glNVDoRegistryHacks(ppdev->hwGfxCaps,
                          &flippingControl,
                          &doUnifiedBuffers,
                          &doWindowFlipping,
                          &applicationKey,
                          &appSupportBits,
                          &overlaySupport,
                          &apiStereoSupport,
                          &apiStereoMode);
#endif                          
    ppdev->oglRegUBB              = doUnifiedBuffers;
    ppdev->oglRegWindowFlipping   = doWindowFlipping;
    ppdev->oglRegFlippingControl  = flippingControl;
    ppdev->oglRegOverlaySupport   = overlaySupport;
    ppdev->oglRegApiStereoSupport = apiStereoSupport;
    ppdev->oglRegApiStereoMode    = apiStereoMode;
}
#endif // NVARCH >= 0x4


//*************************************************************************
// bOglAssertMode
// 
// has to be called within DrvAssertMode at a point where the hardware 
// is accessible.
//
// returns TRUE  if succeeded
//         FALSE if failed
//*************************************************************************
BOOL bOglAssertMode(PPDEV ppdev, BOOL bEnable)
{
    ASSERT (NULL!=ppdev);

    if (bEnable)
    {
        //
        // Enable paused mode again
        //

        if (OglIsEnabled(ppdev))
        {
            // try to enable necessary ogl features.
            bOglPageFlipModeUpdate(ppdev);
            bOglOverlayModeUpdate(ppdev);
            bOglStereoModeUpdate(ppdev);
        }
    }
    else
    {
        //
        // Pause mode (temporariliy disabled)
        //

        if (OglIsEnabled(ppdev))
        {
            // Disable GDI broadcasting
            // pageflipping, stereo shuttering, merge blits, ...
            bOglPageFlipModeDisableAll(ppdev);
            bOglOverlayModeDisable(ppdev);
            bOglStereoModeDisable(ppdev);
        }

        // need to free allocated UBB buffers
        if (ppdev->bOglSingleBackDepthCreated)  
        {
            // Free UBB surfaces, clients must recover
            OglFreeUnifiedSurfaces(ppdev);
        }
    }

    // for now cannot fail
    return TRUE;
}


//*************************************************************************
// bOglResetPDEV
// 
// has to be called in DrvResetPDEV to check if a mode switch is allowed 
// depending on OpenGL settings and active applications.
//
// returns TRUE  if modeswitch is allowed
//         FALSE if modeswitch is not allowed
//*************************************************************************
BOOL bOglResetPDEV(
    PPDEV ppdevOld, // ppdev of mode we're leaving (old mode)
    PPDEV ppdevNew) // ppdev of mode we're switching to (new mode)
{
    BOOL bRet = FALSE; // default error, cannot allow to switch resolution
    BOOL bOld;         // remember old value

    ASSERT(NULL!=ppdevNew);
    ASSERT(NULL!=ppdevOld);

    // Overlays are not supported in all resolutions
    if (   (   (bOld = ppdevOld->bOglOverlayDesired)                   //    overlay was enabled
            || bOglOverlaySurfacesAllowed(ppdevOld) )                  //    or overlays is still enabled
        && (cOglWindows(ppdevOld)>0)                                   // and opengl running
        && !bOglOverlaySurfacesAllowed(ppdevNew)                       // new mode doesn't support overlay
       )
    {
        DISPDBG((0, "ERROR: cannot switch to new mode because OpenGL overlays won't work!"));
        // if registry has changed in the meantime, we lost this value. Reset it!
        ppdevOld->bOglOverlayDesired = bOld; 
        goto Exit;
    }

    // Stereo isn't supported in all resolutions
    if (   (   (bOld = ppdevOld->bOglStereoDesired)                    //    stereo was enabled
            || bOglStereoSurfacesAllowed(ppdevOld) )                   //    or stereo is still enabled
        && (cOglWindows(ppdevOld)>0)                                   // and opengl running
        && !bOglStereoSurfacesAllowed(ppdevNew)                        // new mode doesn't support stereo
       )
    {
        DISPDBG((0, "ERROR: cannot switch to new mode because OpenGL stereo won't work!"));
        // if registry was changed in the meantime, we lost this value. Reset it!
        ppdevOld->bOglStereoDesired = bOld; 
        goto Exit;
    }

    // everything is okay!
    bRet=TRUE;
Exit:
    return (bRet);
}

#ifdef NV_MAIN_MULTIMON
/***************** OGL Multi-Monitor Support functions ******************/
/* See MAIN - oglmultimon.c for similar code.                           */
/************************************************************************/

/*
 * FUNCTION: OglClientIntersectsDevice
 *
 * DESC: Intersects the clientDrawableInfo rect with that of the PDEV
 */ 
BOOL OglClientIntersectsDevice(PDEV *ppdev, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo)
{
    if (!ppdev->bEnabled) return GL_FALSE;
    if ((int)clientDrawableInfo->rect.bottom <= (int)ppdev->top) return GL_FALSE;
    if ((int)clientDrawableInfo->rect.top >= (int)(ppdev->top + ppdev->cyScreen)) return GL_FALSE;
    if ((int)clientDrawableInfo->rect.left >= (int)(ppdev->left + ppdev->cxScreen)) return GL_FALSE;
    if ((int)clientDrawableInfo->rect.right <= (int)ppdev->left) return GL_FALSE;
  
    return TRUE;
}

/************************ Global PDEV management ************************/

/*
 * FUNCTION: OglInitPDEVList
 * 
 * DESC: Initialize the globalOpenGLData PDEV list to NULL 
 * 
 * Called by DrvEnableDriver
 */
void OglInitPDEVList(void)
{
    if (!bLinkedListCreate(&(globalOpenGLData.oglpdevList), LL_CLASS_OGLPDEV))
    {
        /* For some reason our initialization failed */
        DISPDBG((1, "OglInitPDEVList: Error! Can't create the PDEV linked list!"));
        ASSERT(FALSE);
    }
}

/*
 * FUNCTION: OglDestroyPDEVList
 *
 * DESC: Destroys the PDEV List
 *
 * Called by DrvDisableDriver
 */
void OglDestroyPDEVList(void)
{
    /* If we failed here, we are either calling DrvDisableDriver too many times, or 
     * something went terribly wrong.  Either way, we need to print Debug
     * information about this.
     */
    if (!bLinkedListDestroy(&(globalOpenGLData.oglpdevList)))
    { 
        DISPDBG((1, "OglInitPDEVList: Error! Can't destroy the PDEV linked list!"));
        ASSERT(FALSE);
    }
}

/*
 * FUNCTION: OglAddPDEVToList
 *
 * DESC: Adds a PDEV reference node to the globalOpenGLData structure.
 *
 * Called by DrvEnablePDEV
 */
void OglAddPDEVToList(PDEV *ppdev)
{
    bLinkedListAddElement(globalOpenGLData.oglpdevList, LL_CLASS_OGLPDEV, (ULONG_PTR)ppdev, (PVOID)ppdev, 0);
}

/*
 * FUNCTION: OglRemovePDEVFromList
 *
 * DESC: Removes a matching PDEV from the globalOpenGLData PDEV list.
 *
 * Called by DrvDisablePDEV
 */
void OglRemovePDEVFromList(PDEV *ppdev)
{
    bLinkedListRemoveElement(globalOpenGLData.oglpdevList, LL_CLASS_OGLPDEV, (ULONG_PTR)ppdev);
}

//
//  bOglGetNextPdev
//
//  Function is an interface for struct LINKEDLIST with. 
//  class LL_CLASS_OGLPDEV. It returns next element in inked. 
//  If *pul is 0, function returns head of list.
//  Function returns TRUE while a new element is found.
//
//  Function should be used as follows:
//
//  PDEV       *ppdevLoop   = NULL;
//
//  while(bOglGetNextPdev(&ppdevLoop))  // loop thru entire oglpdevlist until we find desired listentry
//  {
//      // do something here
//   }
//
BOOL bOglGetNextPdev( IN OUT PPDEV* pppdev )
{
    BOOL   bRet       = FALSE;
    PVOID  pvData     = NULL;

    // we need a valid ppdevlist here
    ASSERT(globalOpenGLData.oglpdevList);
    ASSERT(pppdev);

    if( NULL==*pppdev )
    {
        bRet = bLinkedListGetFirstElement(globalOpenGLData.oglpdevList, LL_CLASS_OGLPDEV, (ULONG_PTR*)pvData, &pvData);
        if( bRet )
        {
            *pppdev = (PPDEV)pvData;
        }
    }
    else
    {
        bRet = bLinkedListGetNextElement(globalOpenGLData.oglpdevList, LL_CLASS_OGLPDEV, (ULONG_PTR*)pvData, &pvData);
        if( bRet )
        {
            *pppdev = (PPDEV)pvData;
        }
    }

    return bRet;
}


//
//  bOglRetrieveValidPDEVForDevice
//
//  Function loops thru global ddevList and returns ppdev 
//  which is enabled and belongs to given ulDeviceNumber.
//
BOOL bOglRetrieveValidPDEVForDevice( IN OUT PPDEV* pppdev, IN ULONG ulDeviceNumber )
{
    BOOL bRet = FALSE;
    ASSERT(pppdev);

    *pppdev = NULL;                 // start enumeration at root

    while(bOglGetNextPdev(pppdev))  // loop thru entire ppdevlist until we find desired listentry
    {
        if(  ((*pppdev)->bEnabled)                             // is the ppdev initialized and enabled ?
           &&((*pppdev)->ulDeviceReference == ulDeviceNumber)  // is this the number of coresponding device
          )
        {
            bRet = TRUE;
            break;
        }
    }

    return bRet;
}


//
//  bOglRetrieveValidPDEVForOrigin
//
//  Function loops thru global ddevList and returns ppdev 
//  which is enabled and has origin described by lLeft and lTop.
//
BOOL bOglRetrieveValidPDEVForOrigin( IN OUT PPDEV* pppdev, IN LONG lLeft, IN LONG lTop )
{
    BOOL bRet = FALSE;

    ASSERT(pppdev);

    *pppdev = NULL;                             // start enumeration at root

    while(bOglGetNextPdev(pppdev))              // loop thru entire ppdevlist until we find desired listentry
    {
        if(  ((*pppdev)->bEnabled)              // is the ppdev initialized and enabled ?
           &&((*pppdev)->left == (ULONG)lLeft)  // does origin fit devices origin ?
           &&((*pppdev)->top  == (ULONG)lTop )  // does origin fit devices origin ?
          )
        {
            bRet = TRUE;
            break;
        }
    }
    return bRet;
}
#endif