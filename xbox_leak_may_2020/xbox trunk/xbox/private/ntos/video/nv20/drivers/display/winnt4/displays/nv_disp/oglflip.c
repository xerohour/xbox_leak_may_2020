//******************************Module*Header***********************************
// Module Name: oglflip.c
//
// Maintenance code for OpenGL flipping.
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

// import
#include "precomp.h"
#include "driver.h"
#include "nvdoublebuf.h"
#include "oglDD.h"
#include "oglutils.h"
#include "oglclip.h"
#include "Nvcm.h"

// export
#include "oglflip.h"

//*************************************************************************
// bOglClientIsFlipCandidate
// 
// returns TRUE if clientInfo belongs to a client that is allowed to 
// do pageflipping now. A flip candidate must have a exchange pixelformat
// or run in fullscreen and must be visible 
//*************************************************************************
BOOL bOglClientIsFlipCandidate(
    IN PPDEV                          ppdev,
    IN struct _NV_OPENGL_CLIENT_INFO *clientInfo)
{
    BOOL bRet = FALSE;

    ASSERT(NULL!=ppdev);
    ASSERT(NULL!=clientInfo);

    // *****
    // *KEY* Here we decide if a client can flip or not!
    // *****
    if (   (clientInfo->pfd.dwFlags & PFD_SWAP_EXCHANGE)                // must have the exchange (flip) pixelformat
        && (   bOglIsUnclippedClient(clientInfo)                        // unclipped (easy decision)
            || (   !bOglClientIntersectingWithOtherClients(clientInfo)  // or not clipped against other ogl
                && !bOglIsComplexClippedClient(ppdev, clientInfo) )     // and not complex clipped (complex clipping is slower than blitting)
           )
        && (   bOglIsFullscreenClient(clientInfo)                       // either be a quake fullscreen client
            || (   ppdev->bOglSingleBackDepthSupported                  // or needs GDI double pumping
                && ppdev->bOglSingleBackDepthCreated)
           )
       )
    {
        // these assertions catch internal errors, where we export wrong pixelformats
        ASSERT(0 != (clientInfo->pfd.dwFlags & PFD_DOUBLEBUFFER));  // not allowed on single buffered client
        ASSERT(0 == clientInfo->pfd.bReserved);                     // not allowed on overlay client
        ASSERT(0 == (clientInfo->pfd.dwFlags & PFD_STEREO));        // not allowed on stereo client

        bRet = TRUE;
    }

    DISPDBG((3, "bOglClientIsFlipCandidate - 0x%08p %s", clientInfo, bRet ? "is a flip candidate" : "must blit"));

    return (bRet);
}


//*************************************************************************
// bOglGetNextFlipCandidate
// 
// returns next flip candidate starting with given client.
//*************************************************************************
BOOL bOglGetNextFlipCandidate(
    IN     PPDEV                   ppdev, 
    IN OUT NV_OPENGL_CLIENT_INFO **ppClientInfo) 
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    BOOL                   bRet = FALSE;

    ASSERT(NULL!=ppdev);

    // use local copy to query bOglGetNextVisibleClient!
    if (NULL==ppClientInfo)
        clientInfo = NULL;
    else
        clientInfo = *ppClientInfo; // can be NULL!

    while (bOglGetNextVisibleClient(&clientInfo))
    {
        // found visible client, but is it a flip candiate, too?
        if (bOglClientIsFlipCandidate(ppdev, clientInfo))
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
// bOglIsClientFlipping
// 
// returns TRUE if given client is set to flip mode (this doesn't mean it 
// really is flipping if itself doesn't want!)
//*************************************************************************
BOOL bOglIsClientFlipping(
    IN struct _NV_OPENGL_CLIENT_INFO *clientInfo)
{
    return (   bOglHasDrawableInfo(clientInfo)
            && clientInfo->bFlipping);
}


//*************************************************************************
// bOglGetNextFlippingClient                                                 
//                                                                    
// Start with clientInfoStart (not included to search) and find       
// next flipping OpenGL client and return the result to *ppClientInfo.
// If clientInfoStart==START_SEARCH_AT_BEGIN start with first client. 
//                                                                    
// NOTE: There only should be one flipping client at all!             
//
// ppClientInfo == NULL: Only check if there is a client
// ppClientInfo != NULL: In addition return found client
//                       *ppClientInfo == NULL: Start search at top of 
//                                              client list
//                       *ppClientInfo != NULL: Start search with given 
//                                              client (not included)
// Returns TRUE if a flip client was found and returns a pointer to the 
//              clientInfo in *ppClientInfo if ppClientInfo!=NULL.
// Returns FALSE if client wasn't found, *ppClientInfo isn't touched!
//*************************************************************************
BOOL bOglGetNextFlippingClient(
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
        if (bOglIsClientFlipping(clientInfo))
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
// bOglPageFlipModeUpdate
// 
// Checks the pageflip status of all open OpenGL apps against system 
// conditions and updates status if neccessary. It will disable 
// pageflipping cleaning up the buffers, flipping back to primary, 
// notifiying affected clients. It will enable pageflipping if one single
// client is visible and it is a FlipCandidate. On Quadro it will manage
// double pumping for window flipping.
//
// Pageflip enable will only be done if PAGEFLIP_IMMEDIATE_UPDATE is set!
// Pageflip enable should not be called from within DrvClipChanged!
// 
// return: TRUE  - succeeded        
//         FALSE - failed, any error
//*************************************************************************
BOOL bOglPageFlipModeUpdate(PPDEV ppdev)
{
    BOOL                   bFlipCandidateFound  = FALSE;
    NV_OPENGL_CLIENT_INFO *clientInfoFlipOld    = NULL;
    NV_OPENGL_CLIENT_INFO *clientInfo           = NULL;
    BOOL                   bRet                 = FALSE;
    ULONG                  cVisibleClients;

    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppdev->pOglServer);
    ASSERT(ppdev->pOglServer->ulPageFlipRetryCounter <= PAGEFLIP_RETRY_COUNTER_START);

    cVisibleClients = cOglVisibleClients(ppdev);

    if (   bOglDirectDrawAppRunning(ppdev) // direct draw has direct access to primary
        || bOglPOWERlibAppRunning(ppdev)   // POWERlib has direct access to primary
        || !OglIsEnabled(ppdev)            // OpenGL now disabled
       )
    {
        // apps using linear addressing to the FB running
        bRet = bOglPageFlipModeDisableAll(ppdev);
    }
    else if (   (0 == cOglWindows(ppdev)) // all ogls closed
             || (cVisibleClients  > 1)    // multiple visible clients
             || (cVisibleClients == 0)    // all clients invisible
            )
    {
        // conditions for flipping not met        
        bRet = bOglPageFlipModeDisableAll(ppdev);
    }
    else
    {
        // only one ogl visible
        ASSERT(1==cVisibleClients);

        clientInfo          = NULL;
        bFlipCandidateFound = bOglGetNextFlipCandidate(ppdev, &clientInfo);

        clientInfoFlipOld = NULL;
        if ( bOglGetNextFlippingClient(&clientInfoFlipOld) )
        {
            if (clientInfo != clientInfoFlipOld)
            {
                bOglPageFlipModeDisable(ppdev, clientInfoFlipOld);

                ASSERT(!bOglGetNextFlippingClient(NULL));
            }
        }

        if ( bFlipCandidateFound )
        {
            // only enable pageflipping now, if enable isn't deferred to the retry!
            if (PAGEFLIP_IMMEDIATE_UPDATE == ppdev->pOglServer->ulPageFlipRetryCounter)
            {
                //
                // KEY: Enable pageflipping for this client.
                //

                bRet = bOglPageFlipModeEnable(ppdev, clientInfo);
            }
            else
            {
                // leave it as it is!
            }
        }
        else
        {
            bRet = bOglPageFlipModeDisableAll(ppdev);
        }
    }

    return (bRet);
}


//*************************************************************************
// bOglPageFlipModeEnable
// 
// enables pageflipping for the given OpenGL client.
// The client has to be a FlipCandidate. If it isn't already flipping, 
// flipping will be allowed for that client and double pumping is enabled
// on Quadro boards.
// Pageflip enable should not be called from within DrvClipChanged!
// 
// return: TRUE  - succeeded
//         FALSE - failed, any error
//*************************************************************************
BOOL bOglPageFlipModeEnable(
    IN PPDEV                  ppdev, 
    IN NV_OPENGL_CLIENT_INFO *clientInfo)
{
    BOOL bRet=FALSE;

    ASSERT(NULL!=ppdev);
    ASSERT(bOglHasDrawableInfo(clientInfo));
    ASSERT(bOglClientIsFlipCandidate(ppdev, clientInfo));

    if ( !bOglIsClientFlipping(clientInfo) )
    {
        if (bOglIsFullscreenClient(clientInfo))
        {
            // all fullscreen flip candidates may flip!
        }
        else if (HWGFXCAPS_QUADRO_GENERIC(ppdev))
        {
            // Only workstation parts my flip if not fullscreen, but they need ubb buffers
            if (   !ppdev->bOglSingleBackDepthCreated                                                          // need UBB surface                 
                || (   !bDoublePumped(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT])                 // need double pumping              
                    && !NV_AddDoubleBufferSurface(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT]) ) ) //   failed to enable double pumping
            {
                // failed to enable double pumping code!
                goto Exit;
            }
        }

        // *****
        // *KEY* here we enable a client to flip.
        // *****
        clientInfo->bFlipping = TRUE;
        clientInfo->clientDrawableInfo->ubbWindowClipFlag = FALSE; // need to disable ubb clip optimization
        ppdev->pOglServer->ulPageFlipRetryCounter         = PAGEFLIP_IMMEDIATE_UPDATE; // no need to retry anymore
        DISPDBG((3, "bOglPageFlipModeEnable: 0x%08p is flip enabled", clientInfo));
    }
    else 
    {
        // already initialized
    }


    bRet = TRUE;

Exit:
    return (bRet);
}


//******************************************************************************
// bOglPageFlipModeDisable
// 
// disables pageflipping for the given OpenGL client.
// This includes flipping back to primary, exchanging front and back if flipped
// and disabling double pumping.
// 
// return: TRUE  - succeeded
//         FALSE - failed, any error
//******************************************************************************
BOOL bOglPageFlipModeDisable(
    IN PPDEV                  ppdev, 
    IN NV_OPENGL_CLIENT_INFO *clientInfo)
{
    BOOL bRet=FALSE;

    ASSERT(NULL!=ppdev);
    ASSERT(bOglHasDrawableInfo(clientInfo));
    ASSERT(bOglIsClientFlipping(clientInfo));

    DISPDBG((3, "bOglPageFlipModeDisable: 0x%08p must blit again", clientInfo));
    //
    // disable double pumping, fix buffer offsets...

    if (OGL_FLIPPED()) 
    {
        vFlipToPrimary(ppdev);
    }

    // Potentially client needs to fix his surface offsets
    clientInfo->clientDrawableInfo->clipChangedCount++;
    clientInfo->bFlipping = FALSE;

    // NOTE: This is an assumption that we only double pump on this buffer for page flipping
    if ( bDoublePumped(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT]) )
    {
        NV_RemoveDoubleBufferSurface(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT]);
    }

    bRet = TRUE;
        
    return (bRet);
}


//******************************************************************************
// bOglPageFlipModeDisableAll
// 
// disables pageflipping for all OpenGL clients running.
// This includes flipping back to primary, exchanging front and back if flipped
// and disabling double pumping.
// 
// return: TRUE  - succeeded
//         FALSE - failed, any error
//******************************************************************************
BOOL bOglPageFlipModeDisableAll(PPDEV ppdev)
{
    BOOL                   bRet       = FALSE;
    NV_OPENGL_CLIENT_INFO *clientInfo = NULL;

    ASSERT(NULL!=ppdev);

    //
    // This is only done in the case a flipping client wasn't found!
    //

    if (OGL_FLIPPED()) 
    {
        vFlipToPrimary(ppdev);
    }

    clientInfo = NULL;
    while (bOglGetNextFlippingClient(&clientInfo))
    {
        ASSERT(bOglIsClientFlipping(clientInfo));

        bOglPageFlipModeDisable(ppdev, clientInfo);
        
        // I don't think I want to have two simultaneous flipping clients!
        ASSERT(!bOglGetNextFlippingClient(NULL));
    }

    // NOTE: This is an assumption that we only double pump on this buffer for page flipping
    if ( bDoublePumped(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT]) )
    {
        NV_RemoveDoubleBufferSurface(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT]);
    }
        
    bRet = TRUE;
        
    return (bRet);
}


//*************************************************************************
// bOglPageFlipModeRetryEnable
// 
// Tries to enalbe pageflipping again depending on retry counter.
// If pageflipping was disabled due to collision with GDI we initialize a
// counter to try again after several swapbuffer calls. 
// If the counter is down to PAGEFLIP_DEFER_UPDATE_TO_NEXT_SWAP it will 
// try an immediate update of pageflipping.
// DrvClipChanged will set the counter to PAGEFLIP_DEFER_UPDATE_TO_NEXT_SWAP 
// to invoke a pageflip enable at the next SwapBuffers.
//
// This function must either be called in the context of SwapBuffers or
// in the __glNVDoDACParams (for now it is called in SwapBuffers).
// At lease it MUST be called somewhere!
//*************************************************************************
BOOL bOglPageFlipModeRetryEnable(PPDEV ppdev)
{
    BOOL bRet = FALSE;

    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppdev->pOglServer);
    // As it is unsigned this test includes PAGEFLIP_IMMEDIATE_UPDATE
    ASSERT(ppdev->pOglServer->ulPageFlipRetryCounter <= PAGEFLIP_RETRY_COUNTER_START);

    if (PAGEFLIP_DEFER_UPDATE_TO_NEXT_SWAP == ppdev->pOglServer->ulPageFlipRetryCounter)
    {
        DISPDBG((3, "bOglPageFlipModeRetryEnable: retry enable pageflipping"));

        //
        // Key: try again to enable pageflipping
        //
        ppdev->pOglServer->ulPageFlipRetryCounter = PAGEFLIP_IMMEDIATE_UPDATE;
        bRet = bOglPageFlipModeUpdate(ppdev);
    }
    else if (PAGEFLIP_IMMEDIATE_UPDATE != ppdev->pOglServer->ulPageFlipRetryCounter)
    {
        // wait some more time to enable pageflipping
        ppdev->pOglServer->ulPageFlipRetryCounter--;
    }
    else // ppdev->pOglServer->ulPageFlipRetryCounter == PAGEFLIP_IMMEDIATE_UPDATE
    {
        // default, nothing to do!
    }

    ASSERT(ppdev->pOglServer->ulPageFlipRetryCounter <= PAGEFLIP_RETRY_COUNTER_START);

    return bRet;
}


//*************************************************************************
// cOglFlippingClients
// returns the current number of clients in pageflip mode
//*************************************************************************
ULONG cOglFlippingClients(PPDEV ppdev)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    ULONG                  cFlipClients=0;

    clientInfo = NULL;
    while (bOglGetNextFlippingClient(&clientInfo))
    {
        cFlipClients++;
    }

    // current hard- and sw only supports one flip client!
    ASSERT(cFlipClients <= 1);
    return (cFlipClients);
}
// End of oglflip.c
