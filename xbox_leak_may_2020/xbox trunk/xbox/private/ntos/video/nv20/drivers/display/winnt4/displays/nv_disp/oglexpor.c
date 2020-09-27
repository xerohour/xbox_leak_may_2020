//******************************Module*Header*******************************
// Module Name: oglexpor.c
//
// These variables and routines are exported by the Display Driver for use
// by the OpenGL shared library.  This is only used in NV10 and higher processors
//
// Copyright (c) 1998 NVidia Corporation
//*************************************************************************

#include "precomp.h"
#include "driver.h"
#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
    #include "ddmini.h"
#endif


#ifndef NV3

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
#include "nvReg.h"
#include "oglDD.h"
#include "pixelfmt.h"
#include "nvcom.h"       // drivers/common/inc
#include "nvtracecom.h"  // drivers/common/inc
#include "wincommon.h"   // drivers/common/inc
#include "wglcommon.h"   // drivers/common/inc
#include "Nvcm.h"        // Needed for NV_CFG_VBLANK_COUNTER,

#include "oglclip.h"

int nvControlOptions = 0;
int nvDebugOptions = 0;
int nvDebugMask = 0;
int nvDebugLevel = 0;
int nvDebugStatus = 0;
int nvTraceCountDMAWords = 0;
const int __glInKernel = TRUE;    

__GLNVdata *nvTraceLastPushBufCurrent = NULL;

// This local useful to disable assert's. Handy if assert's are keeping
// debugger control in Display driver, and all you want to do now is get
// back to client mode to terminate the application. May avoid a re-boot.
static int doBreakPoint = 1;

void __cdecl __glNVAssert(void *expString, void *filenameString, unsigned linenumber)
{
    if (doBreakPoint) {
        NV_BREAKPOINT();
    }
}

int __cdecl tprintf(const char *format, ...)
{
    va_list ap;
    
    va_start(ap, format);
    EngDebugPrint("", (char *)format, ap);
    va_end(ap);
    return 0;
}

void __cdecl nvTraceFuncEnter(int clas, nvFunRec *funRec)
{
    if ((nvDebugLevel >= 20) && (nvDebugMask & clas)) {
        tprintf("ENTER %s()\n",funRec->name);
    }
}

void __cdecl nvTraceFuncExit(int clas, nvFunRec *funRec)
{
    if ((nvDebugLevel >= 20) && (nvDebugMask & clas)) {
        tprintf("EXIT %s()\n",funRec->name);
    }
}

void __cdecl nvDebugCountDMAWords(void *cmdBufPtr)
{
}

void * __cdecl __glFlusholdNextPut(void *state)
{
    if (doBreakPoint) {
        NV_BREAKPOINT();
    }
    return NULL;
}

int __cdecl
__glFlusholdCheck2DClip(void *state)
{
    return 1;
}

void __cdecl nvComparePushBuf(void *fLastPut, void *fCurrent, void *fFifo, int fFormatted, int makeRoomWords, void *startOfFlushBlock)
{
}

#if defined(_WIN32) && !defined(_WIN64)
void __cdecl __glNVMemCopy
(
    DWORD dwDest,
    DWORD dwSrc,
    DWORD dwCount,
    DWORD dwFlags
)
{
    memcpy ((void*)dwDest,(void*)dwSrc,dwCount);
    return;
}
#endif

void __cdecl __glNVFlushInternal(__GLNVcmdInfo *cmdInfo, 
                                   unsigned char forceRealBuf)
{
    PDEV *ppdev = (PDEV *) cmdInfo->ntOther;

    if (forceRealBuf) {
        // forceRealBuf is TRUE when we are called from the lowest level
        // flush code that just wants us to write the Put.  So we just
        // call NV10_KickoffPushBuffer
        if (ppdev->oglSyncGdi) {
            OglChannelWait((PDEV *) cmdInfo->ntOther, cmdInfo->ourChannelId,
                           cmdInfo->maskOtherChannels);
        }
        __glNVKickOffPushBufferNT(cmdInfo);
    } else {
        if (cmdInfo->usingCopyBuf) {
            __glNVCopyPseudoKickoff(cmdInfo, GL_TRUE);
        }
        if (!cmdInfo->isClipped) {
            // If we are not clipped rendering then we were just called from
            // a swap routine (this may change in the future.  In that case
            // all we have to do is write Put then make some jump space just
            // like __glNV10Flush does without all the drawable validation
            // stuff since we are guaranteed to flush before we enter the
            // shared lib.
            if (ppdev->oglSyncGdi) {
                OglChannelWait((PDEV *) cmdInfo->ntOther, cmdInfo->ourChannelId,
                               cmdInfo->maskOtherChannels);
            }
            __glNVKickOffPushBufferNT(cmdInfo);
            __glNVMakeJumpSpaceInPushBuffer(cmdInfo);
        } else {
            // Otherwise, do a full clipped kickoff cuz we might have been
            // writing into the clipbuffer, SetupAndKickoff takes care of
            // this.  This can be recursively called from below SetupAndKickoff
            // or from a swap.

            // Note: Flushold my be busted here if the clipping changes.
            // We may need to revisit this and change the FALSE to a TRUE
            // so that it punts if were going to do a clip change.  The
            // other option is to move the clip test higher up.  
            //    - ibuck

            NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;

            clientDrawableInfo = (NV_OPENGL_DRAWABLE_INFO *)cmdInfo->ntDrawable;

            // Test for optimized path
            if (clientDrawableInfo->ubbWindowClipFlag == (ULONG)TRUE &&
                !(clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_OCCLUDED) &&
                cmdInfo->backBufferIsEnabled &&
                !__glNVWasFlipped(cmdInfo))
                {
                ULONG numRects;
                RECTL ubbRect;

                // 2D clip is bounds....ignore clip list in this case
                // UBB is ON, window is not occluded, window flipping is OFF, back buffered rendering only
                COPY_RECT(ubbRect, clientDrawableInfo->rect);
                OglClipRectToScreen((RECTL *)&ubbRect,
                                     clientDrawableInfo->cxScreen,
                                     clientDrawableInfo->cyScreen);
                OglTranslateCachedClipRectList(ppdev,
                                               &ubbRect, 1,
                                               &ubbRect, &numRects,
                                               (RECTL *) &(clientDrawableInfo->rect));

                __glNVSetupAndKickoff(cmdInfo,
                          numRects,  (__GLregionRect *)&ubbRect,
                          0, (__GLregionRect *)NULL,  FALSE);
            }
            else
            {
                __glNVSetupAndKickoff(cmdInfo,
                                  clientDrawableInfo->cachedNumClipRects,
                                  (__GLregionRect *)(clientDrawableInfo->cachedClipRectList),
                                  clientDrawableInfo->cachedNumExclusiveRects,
                                  (__GLregionRect *)(clientDrawableInfo->cachedExclusiveRectList),
                                  FALSE);
            }
        }
    }
}

/*
 * FUNCTION: __glNV4FlushInternal
 *
 * DESC: Wait for the DMA Channel and call the SetupAndKickoff routines to update the fifo pointers
 *       and clips.  If we are not clipped, or we have a bad rect list, then we will generate
 *       our own, based on the window position and occlusion.
 *
 * Called from __glNV4SetupSwap, nv4FlipBuffers, and NV4_OglFlushClipped.
 */
void __cdecl __glNV4FlushInternal(__GLNVcmdInfo *cmdInfo)
{
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;

    PDEV *ppdev = (PDEV *) cmdInfo->ntOther;
    RECTL ubbRect;
    
    // BugFix for Indexed image blits when OGL running, for NV < NV10 (bug #20000720-213841)
    OglChannelWait((PDEV *) cmdInfo->ntOther, cmdInfo->ourChannelId,
                   cmdInfo->maskOtherChannels);

    clientDrawableInfo = (NV_OPENGL_DRAWABLE_INFO *)cmdInfo->ntDrawable;

    // Otherwise, do a full clipped kickoff cuz we might have been
    // writing into the clipbuffer, SetupAndKickoff takes care of
    // this.  This can be recursively called from below SetupAndKickoff
    // or from a swap.
    if ( (!cmdInfo->isClipped) ||
         ((clientDrawableInfo->ubbWindowClipFlag == (ULONG)TRUE) &&
          !(clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_OCCLUDED) &&
          (cmdInfo->backBufferIsEnabled)) )
    {
        extern void CombineRectLists(RECTL *rectList1, ULONG count1, 
                                     RECTL *rectList2, ULONG count2,
                                     LONG  leftList2, LONG topList2,
                                     RECTL *resultList, ULONG *resultCount);
        ULONG numRects;
        
        // 2D clip is bounds....ignore clip list in this case
        // UBB is ON, window is not occluded, window flipping is OFF, back buffered rendering only
        COPY_RECT(ubbRect, clientDrawableInfo->rect);
        OglClipRectToScreen((RECTL *)&ubbRect,
                            clientDrawableInfo->cxScreen,
                            clientDrawableInfo->cyScreen);

        CombineRectLists((RECTL*) &(cmdInfo->coreClipRect), 1, 
                         &ubbRect, 1,
                         (LONG) (clientDrawableInfo->rect.left - ppdev->left),
                         (LONG) (clientDrawableInfo->rect.top - ppdev->top),
                         &ubbRect, &numRects);

        __glNV4SetupAndKickoff(cmdInfo, numRects, (__GLregionRect *)&ubbRect);
    }
    else /* We are clipped in one way or another. */
    {
        __glNV4SetupAndKickoff(cmdInfo, 
                               clientDrawableInfo->cachedNumClipRects,
                               (__GLregionRect *)clientDrawableInfo->cachedClipRectList);
    }
}

void __cdecl __glNV4KickOffPushBuffer(__GLNVcmdInfo *cmdInfo) 
{
    // Call back into the shared lib to do the kickoff since only the
    // shared lib has access to state, nv4state, etc...
    __glNV4KickOffPushBufferNT(cmdInfo);
}


unsigned int __cdecl __glNVGetCurrentScanLine(__GLNVcmdInfo *cmdInfo, int *iCurScanLine) {
    PDEV  *ppdev = (PDEV *) cmdInfo->ntOther;

    *iCurScanLine = ppdev->pfnGetScanline(ppdev);
    return TRUE;
}

unsigned int __cdecl __glNVGetVBlankCounter(__GLNVcmdInfo *cmdInfo, int head, int *iCurSwap) {
    PDEV  *ppdev = (PDEV *) cmdInfo->ntOther;
    NV_CFGEX_VBLANK_COUNTER_MULTI_PARAMS param;
    unsigned int ret;

    param.Head = head;
    param.VBlankCounter = 0;

    ret = (unsigned int) NvConfigGetEx (ppdev->hDriver,
                                        ppdev->hClient,
                                        ppdev->hDevice,
                                        NV_CFGEX_VBLANK_COUNTER_MULTI,
                                        &param,
                                        sizeof(param));
    *iCurSwap = param.VBlankCounter;
    return ret;
}

unsigned int __cdecl __glNVGetVBlankEnabled(__GLNVcmdInfo *cmdInfo, int head, int *enabled) {
    PDEV  *ppdev = (PDEV *) cmdInfo->ntOther;
    NV_CFGEX_VBLANK_ENABLED_PARAMS param;
    unsigned int ret;

    param.Head = head;
    param.Enabled = *enabled;

    ret = (unsigned int) NvConfigGetEx (ppdev->hDriver,
                                        ppdev->hClient,
                                        ppdev->hDevice,
                                        NV_CFGEX_VBLANK_ENABLED,
                                        &param,
                                        sizeof(param));
    *enabled = (int) param.Enabled;
    return ret;
}

//******************************************************************************
// avoid unresolved globals from shared library.
// only needed in ICD.
//******************************************************************************

int __cdecl __glwqExportThreadIndex()
{
    NV_BREAKPOINT();    // should not be called in kernel
    return 0;
}

/*****************************************************************************/
void __cdecl
__glmt0NVWaitForFreeSpace(void *state, int space)
// should only be called from thread 0 (i.e. the master thread)
{
}

//********************************************************************
// This is called by OpenGL when it is going to do a flip.  This sets
// the flipbase to the base address of the buffer we are going to flip
// to.  In addition it also grabs the panning offset that the display
// driver is currently at if we happen to be doing a pan'n'scan type
// of virtual desktop.  This routine returns TRUE if OpenGL should 
// stop flipping due to another OpenGL surface active or if a ddraw
// video surface is also active since this would cause problems with
// window flipping.
//********************************************************************

int __cdecl __glNVDoDACParams(__GLNVcmdInfo *cmdInfo, NvU32 newDACOffset, NvU32 *panningOffset)
{
    int i;
    int stopFlip;
    PDEV *ppdev = (PDEV *) cmdInfo->ntOther;

    for (i = 0; i < NV_NO_DACS; i++) 
        {
        panningOffset[i] = (NvU32)ppdev->ulHeadDisplayOffset[i];
        }

    stopFlip =  !((NV_OPENGL_CLIENT_INFO *)cmdInfo->ntClientInfo)->bFlipping;        
                //((globalOpenGLData.oglDrawableClientCount > 1) ||
                //(ppdev->oglDDrawSurfaceCount > OGL_MAX_DDRAW_SURFACES_FOR_FLIPPING));

    if (!stopFlip)
    {
        NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;

        // flipping possible, remember new offset
        ppdev->ulFlipBase = (ULONG)newDACOffset;

        // this client is flippping now
        clientDrawableInfo = (NV_OPENGL_DRAWABLE_INFO *)cmdInfo->ntDrawable;
        clientDrawableInfo->bFlipping = TRUE;
    }
    /*
    else if (OGL_FLIPPED()) 
    {
        // flipping and we're on the wrong page 
        // and we need to flip back to primary
        vFlipToPrimary(ppdev);
    }
    */

    return stopFlip;  
}

//********************************************************************
// If video is active, then posting a stall method to the hardware may
// cause the hardware to hang since the resman may not able to service
// the stall method due to interrupts caused by the video driver
//********************************************************************

int __cdecl __glNVDisableStall(__GLNVcmdInfo *cmdInfo)
{
    PDEV *ppdev = (PDEV *) cmdInfo->ntOther;

#if (_WIN32_WINNT >= 0x0500) && !defined(_WIN64)
    if (ppdev->pDriverData->vpp.dwOverlaySurfaces) {
        return TRUE;
    }
#endif          
    return FALSE;

}

int __cdecl __wglNVGetPfdFlags(void *osother, __WINNVpixelFormatFlags *winPFFlags)
{
    PDEV *ppdev = (PDEV *) osother;
    int flags = 0;

    bOglPfdCheckFlags(ppdev, (ULONG *) &flags);
//    flags |= __winNVProcessWinFlags(winPFFlags);
    return flags;
}
int __cdecl __wglGDIDescribePixelFormat(int iPixelFormat,
                                        unsigned int uBytes,
                                        PIXELFORMATDESCRIPTOR *ppfd,
                                        void *osother)
{
    ASSERTDD(0,"__wglGDIDescribePixelFormat should not be called in the server");
    return FALSE;
}
#endif // NV3

