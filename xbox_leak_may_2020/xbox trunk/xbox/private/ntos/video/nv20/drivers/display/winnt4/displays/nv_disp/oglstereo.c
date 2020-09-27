//******************************Module*Header***********************************
// Module Name: oglstereo.c
//
// OS dependent functions releated to oglstereo reside here
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
#ifdef NVD3D
    #include "dx95type.h"
    #include "ddmini.h"
#endif  
#include "nvRmStereo.h"
#include "nvdoublebuf.h"
#include "nvReg.h"
#include "oglutils.h"

// export
#include "oglstereo.h"


// Documentation of the stereo implementation
//
// The OpenGL stereo implementation uses the OpenGL API on a window with 
// pixelformat that has PFD_STEREO selected. In stereo mode we run in a
// quad buffered environment. The color buffers are GL_FRONT_LEFT, GL_FRONT_RIGHT, 
// GL_BACK_LEFT and GL_BACK_RIGHT. Also depth and stencil buffers are duplicated
// for the right eye. pic 1.
// Monoscopic apps will default to the left buffers and the broadcast to the
// right buffers has to be handled. We allow a mix of monoscopic and stereo 
// apps in a windowed environment.
//
// If the system in in stereo mode (OGL_EnableStereoSupport) all ICDs, even
// monoscopic ICDs will cause the display driver to allocate and reserve 
// all stereo buffers to guarantee that later stereo apps will succeed to run
// in hardware.
//
// If a stereo ICD is enabled the display driver will enable the shutter glasses
// and double pump GDI to the right front buffer.
//
// If all stereo windows are occluded or disabled through an escape/OGL extension
// we disable the shutter glasses and double pumping. This speeds up the system.
//
// Notes/Limitations:
//
// * We don't want to flip in stereo mode, because we would need to wait 
//   on the vblank. Toggle between hsync and vsync flips causes the screen 
//   to flicker.
// * Monoscopic double buffered will run as in non stereo environment but
//   the ICDs need to allocate and initialize at least the stereo buffers 
//   in the drawable private to allow the swap to be broadcasted. The 
//   broadcast is completely solved in the shared library through the
//   dp->stereoEyeLoop used by the stereo swap, too. It uses different 
//   initialized buffers. pic 4.
// * Monoscopic single buffered ICDs are internally set to stereo.
//   This will cause GL_FRONT to render to both frontbuffers.
//   No need to broadcast outside, but we run through differnt pathes
//   than in a non stereo environment! 
// 
// 
// pic 1) Default mode stereo apps
// 
//  LEFT eye               RIGHT eye
//
// +---------+            +---------+                            
// |  Front  |<- toggle ->|  Front  |                            
// |  left   |    DAC     |  right  |                  
// |         |<-+         |         |<-+                      
// +---------+  |         +---------+  |                      
//              |                      |    
//          swap-left              swap-right
//              |                      |     
// +---------+  |         +---------+  |                      
// |  Back   | -+         |  Back   | -+                      
// |  left   |            |  right  |                  
// |         |            |         |                  
// +---------+            +---------+                            
//                                            
// +---------+            +---------+                             
// |    Z    |            |    Z    |                             
// |  left   |            |  right  |                             
// |         |            |         |                             
// +---------+            +---------+         
//                                           
// A swap buffers needs to swap both back buffers to both front buffers.
// This is done by a loop in the swap blit itself.
//
//
// pic 2) Stereo apps, stereo disabled through extension
// 
//  LEFT eye               RIGHT eye
//
// +---------+            +---------+                            
// |  Front  |            |  Front  |                            
// |  left   |            |  right  |                  
// |         |<-+         |         |                 
// +---------+  |         +---------+                 
//              |          ^         
//          swap-left swap-right (duplicate)
//              |       /              
// +---------+  |     /   +---------+                 
// |  Back   | -+----     |  Back   |                 
// |  left   |            |  right  |          
// |         |            |         |                  
// +---------+            +---------+                            
//                                            
// +---------+            +---------+                             
// |    Z    |            |    Z    |                             
// |  left   |            |  right  |                             
// |         |            |         |                             
// +---------+            +---------+         
//
//
// pic 3) Default mode monoscopic double buffered apps
// 
//  LEFT eye         
//
// +---------+       
// |  Front  |       
// |  left   |       
// |         |<-+    
// +---------+  |    
//              |    
//          swap-left
//              |    
// +---------+  |    
// |  Back   | -+       
// |  left   |         
// |         |         
// +---------+                   
//                
// +---------+                    
// |    Z    |                    
// |  left   |                    
// |         |                    
// +---------+    
//
// doesn't need to take care about right buffers
// 
//
// pic 4) Monoscopic double buffered apps running in 
//        stereo environment
// 
//  LEFT eye               RIGHT eye         
//                                           
//   ICD to 
//  LEFT only
//      V
// +---------+            +---------+        
// |  Front  |            |  Front  |        
// |  left   |            |  right  |        
// |         |<-+         |         |<-+     
// +---------+  |         +---------+  |     
//              |                      |     
//          swap-left              swap-right(duplicate)
//              |                      |     
// +---------+  |                     / 
// |  Back   | -+                   /     
// | left +  |                    /      
// | right   | ---------------- /        
// +---------+                 
//                                           
// +---------+     
// |    Z    |     
// |  left   |     
// |         |     
// +---------+
//
// App renders as before, but swap buffers copies back left
// to front left and to front back.
// ICD needs to allocate and associate right buffers. The 
// back right is maped to back left.
//
//
// pic 5) Monoscopic single buffered apps running in 
//        stereo environment
// 
//  LEFT eye               RIGHT eye
//
//          ICD to GL_FRONT
//           /            \
//         /                \
//        V                  V
// +---------+            +---------+
// |  Front  |<- toggle ->|  Front  |
// |  left   |    DAC     |  right  |
// |         |            |         |
// +---------+            +---------+
//
// More information:
// .\drivers\OpenGL\include\GL\glcore.h
// .\drivers\display\winnt4\displays\nv_disp\oglstereo.c


//
// bOglStereoGlassesSwitchOn
//
// calls the miniport to switch on stereo
// glasses toggling
//
// Return value: FALSE: did not switch glasses toggling on
//               TRUE:  switched on  glasses toggling
//
// MSchwarz 11/16/2000: new
//
BOOL bOglStereoGlassesSwitchOn(PDEV* ppdev)
{
    ULONG                 ul;
    NV_CFG_STEREO_PARAMS  StereoParams;
    ULONG                 ulReturnedDataLength;
    BOOL                  bGlassesSwitchOn = FALSE;

    ASSERT(ppdev);

    if (!ppdev->bOglStereoActive)
    {
        RtlFillMemory(&StereoParams,sizeof(StereoParams),0xFF);

        StereoParams.Flags               =   STEREOCFG_WS_OGL_API_STEREO   // mark structure as used by ogl api stereo
                                           | STEREOCFG_STEREO_INITIALIZE   // switch glasses on
                                           | STEREOCFG_ELSADDC;            // use ELSA DDC switching method
        StereoParams.EyeDisplayed        = 0;
        StereoParams.FlipOffsets[1][0]   = ppdev->singleFrontOffset[OGL_STEREO_BUFFER_LEFT];
        StereoParams.FlipOffsets[1][1]   = ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT];
        // for flipping purposes:
        StereoParams.FlipOffsets[2][0]   = ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT];
        StereoParams.FlipOffsets[2][1]   = ppdev->singleBack1Offset[OGL_STEREO_BUFFER_RIGHT];

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_STEREO_GLASSES_SWITCH,
                               &StereoParams,                       // input buffer
                               sizeof(StereoParams),
                               NULL,                                // no output buffer
                               0,
                               &ulReturnedDataLength))
        {
            DISPDBG((1, "bOglStereoGlassesSwitchOn - Error switching stereo glasses on"));
            goto Exit;
        }
        ppdev->bOglStereoActive = TRUE;
    }

    bGlassesSwitchOn = TRUE;

Exit:

    return bGlassesSwitchOn;
}



//
// bOglStereoGlassesSwitchOff
//
// calls the miniport to switch off stereo
// glasses toggling
//
// Return value: FALSE: did not switch glasses toggling off
//               TRUE:  switched off glasses toggling
//
// MSchwarz 11/16/2000: new
//
BOOL bOglStereoGlassesSwitchOff(PDEV* ppdev)
{
    NV_CFG_STEREO_PARAMS  StereoParams;
    ULONG                 ulReturnedDataLength;
    BOOL                  bOglStereoGlassesSwitchOff = FALSE;

    ASSERT(ppdev);

    if (ppdev->bOglStereoActive)
    {
        RtlFillMemory(&StereoParams,sizeof(StereoParams),0xFF);

        StereoParams.Flags               =   STEREOCFG_WS_OGL_API_STEREO   // mark structure as used by ogl api stereo
                                           | STEREOCFG_STEREO_DISABLE;     // switch glasses off
        StereoParams.EyeDisplayed        = 0;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_STEREO_GLASSES_SWITCH,
                               &StereoParams,                       // input buffer
                               sizeof(StereoParams),
                               NULL,                                // no output buffer
                               0,
                               &ulReturnedDataLength))
        {
            DISPDBG((1, "bOglStereoGlassesSwitchOff - Error switching stereo glasses off"));
            goto Exit;
        }

        ppdev->bOglStereoActive    = FALSE;
    }
    bOglStereoGlassesSwitchOff = TRUE;

Exit:

    return bOglStereoGlassesSwitchOff;
}


//*************************************************************************
// bOglIsStereoClient
// returns TRUE if the given clientInfo describes a client running a 
// stereo pixelformat.
//*************************************************************************
BOOL bOglIsStereoClient(NV_OPENGL_CLIENT_INFO *pClientInfo)
{
    BOOL bIsStereoClient;

    bIsStereoClient =  bOglHasDrawableInfo(pClientInfo)
                    && (pClientInfo->pfd.dwFlags & PFD_STEREO);

    return (bIsStereoClient);
}


//*************************************************************************
// bOglGetNextStereoClient
//
// finds next OpenGL client running on a stereo pixelformat starting with 
// next client after *ppClientInfo or at top of list if *ppClientInfo==NULL. 
//
// ppClientInfo == NULL: Only check if there is a client
// ppClientInfo != NULL: In addition return found client
//                       *ppClientInfo == NULL: Start search at top of 
//                                              client list
//                       *ppClientInfo != NULL: Start search with given 
//                                              client (not included)
// Returns TRUE if a stereo client was found and returns a pointer to the 
// clientInfo in *ppClientInfo if ppClientInfo!=NULL.
// Returns FALSE if client wasn't found, *ppClientInfo isn't touched!
//*************************************************************************
BOOL bOglGetNextStereoClient(
    NV_OPENGL_CLIENT_INFO **ppClientInfo) // 
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
        if (bOglIsStereoClient(clientInfo))
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
// cOglStereoClients
// returns the current number of clients running a stereo pixelformat
//*************************************************************************
ULONG cOglStereoClients(PPDEV ppdev)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    ULONG                  cStereoClients=0;

    clientInfo = NULL;
    while (bOglGetNextStereoClient(&clientInfo))
    {
        ASSERT(ppdev->bOglStereoDesired);
        cStereoClients++;
    }

    return (cStereoClients);
}



//*************************************************************************
// bOglIsStereoModeEnabled
// 
// returns TRUE if driver is in stereo mode (e.g. double pumping 
// and glasses).
//*************************************************************************
BOOL bOglIsStereoModeEnabled(PPDEV ppdev)
{
    ASSERT(NULL != ppdev);
    return(ppdev->bOglStereoActive);
}


//*************************************************************************
// bOglGetNextActiveStereoClient
//
// Same (including interface) as bOglGetNextStereoClient but only finds 
// active clients. An active client is visible and not disabled by escape.
//
// Returns TRUE if a visible stereo client was found and returns a pointer 
//   to the clientInfo in *ppClientInfo if ppClientInfo!=NULL.
// Returns FALSE if client wasn't found, *ppClientInfo isn't touched!
//*************************************************************************
BOOL bOglGetNextActiveStereoClient(
    NV_OPENGL_CLIENT_INFO **ppClientInfo)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    BOOL                   bRet = FALSE;

    // use local copy to query bOglGetNextStereoClient!
    if (NULL==ppClientInfo)
        clientInfo = NULL;
    else
        clientInfo = *ppClientInfo; // can be NULL!

    while (bOglGetNextStereoClient(&clientInfo))
    {
        // found stereo client, but is it currently visible
        if (   bOglIsVisibleClient(clientInfo)
            && !clientInfo->bStereoDisabled)
        {
            // if wanted return found client
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
// bOglStereoModeEnable
// 
// enables GDI double pumping to stereo buffer 
// * Shutter glasses will be enabled
// * Tvinview mode will set the second dacs
// * Vertical interlaced stereo will do nothing
//
// Returns TRUE if succeeded, FALSE if failed.
//*************************************************************************
BOOL bOglStereoModeEnable(PPDEV ppdev)
{
    ASSERT(NULL != ppdev);

    if (   ppdev->bOglStereoDesired
        && (OGL_STEREO_NUMBUFFERS == ppdev->ulOglActiveViews) )
    {
        ULONG ulSurfaceOffset = ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT];

        ASSERT(NV_REG_OGL_STEREO_SUPPORT_OFF != ppdev->oglRegApiStereoSupport);
        ASSERT(ppdev->bOglSingleBackDepthCreated);
        ASSERT((bOglGetNextActiveStereoClient(NULL)));

        // at least one visible stereo client
        // need to toggle glasses and double pump
        if (ppdev->bOglStereoActive)
        {
            // already active, nothing to do.
        }
        else
        {
            // Only workstation parts have ubb buffers
            if (   !bDoublePumped(ppdev, ulSurfaceOffset)
                && !NV_AddDoubleBufferSurface(ppdev, ulSurfaceOffset) )
            {
                // failed to enable double pumping code!
                goto Exit;
            }

            switch (ppdev->oglRegApiStereoMode)
            {

                case NV_REG_OGL_API_STEREO_MODE_SHUTTER_GLASSES:
                {
                    // enable it now
                    if (bOglStereoGlassesSwitchOn(ppdev))
                    {
                        ppdev->bOglStereoActive = TRUE;
                    }
                    else
                    {
                        // failed to enable glasses
                        NV_RemoveDoubleBufferSurface(ppdev, ulSurfaceOffset);
                    }
                    break;
                }

                case NV_REG_OGL_API_STEREO_MODE_VERTICAL_INTERLACED_LEFT_0 :
                case NV_REG_OGL_API_STEREO_MODE_VERTICAL_INTERLACED_RIGHT_0:
                {
                    //stereo (perhaps doesn't need double pumping)
                    ppdev->bOglStereoActive = TRUE;
                    break;
                }

                case NV_REG_OGL_API_STEREO_MODE_TVINVIEW_LEFT_DAC0         :
                {
                    // dac0 and dac1 are defaulting to left buffer, need to set dac1 to right 
                    // needs double pumping
                    ASSERTDD(FALSE, "not tested NV_REG_OGL_API_STEREO_MODE_TVINVIEW!");
                    if (ppdev->ulNumberDacsActive > 1)
                    {
                        NvSetDacImageOffset(ppdev, ppdev->ulDeviceDisplay[1], ppdev->singleFrontMainOffset[1] - ppdev->ulPrimarySurfaceOffset);
                        ppdev->bOglStereoActive = TRUE;
                    }
                    else
                    {
                        DBG_ERROR("ERROR in bOglStereoModeEnable: need twinview clone mode to enable NV_REG_OGL_API_STEREO_MODE_TVINVIEW");
                    }
                    break;
                }

                case NV_REG_OGL_API_STEREO_MODE_TVINVIEW_RIGHT_DAC0        :
                {
                    // dac0 and dac1 are defaulting to left buffer, need to set dac0 to right 
                    // needs double pumping
                    ASSERTDD(FALSE, "not tested NV_REG_OGL_API_STEREO_MODE_TVINVIEW!");
                    if (ppdev->ulNumberDacsActive > 1)
                    {
                        NvSetDacImageOffset(ppdev, ppdev->ulDeviceDisplay[0], ppdev->singleFrontMainOffset[1] - ppdev->ulPrimarySurfaceOffset);
                        ppdev->bOglStereoActive = TRUE;
                    }
                    else
                    {
                        DBG_ERROR("ERROR in bOglStereoModeEnable: need twinview clone mode to enable NV_REG_OGL_API_STEREO_MODE_TVINVIEW");
                    }
                    break;
                }

                default:
                    DISPDBG((0, "unsupported NV_REG_OGL_API_STEREO_MODE: 0x%x",ppdev->oglRegApiStereoMode));
                    break;
            }

        }

        ASSERT(ppdev->bOglStereoActive);
        ASSERT(ppdev->DoubleBuffer.bEnabled);
        ASSERT(ppdev->DoubleBuffer.NumBuffers > 0);
        ASSERT(ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT] != ppdev->singleFrontOffset[OGL_STEREO_BUFFER_LEFT]);
    }
    ASSERT(bOglIsStereoModeEnabled(ppdev));

Exit:
    return (ppdev->bOglStereoActive);
}


//*************************************************************************
// bOglStereoModeDisable
// 
// disables stereo mode: 
// * Shutter glasses will stop to toggle
// * Tvinview mode will reset the dacs
// * Vertical interlaced stereo will do nothing
// GDI broadcasting to stereo buffers is stopped.
//
// Returns TRUE if succeeded, FALSE if failed.
//*************************************************************************
BOOL bOglStereoModeDisable(PPDEV ppdev)
{
    BOOL bRet = TRUE;

    ASSERT(NULL != ppdev);

    if (   ppdev->bOglStereoDesired
        && (OGL_STEREO_NUMBUFFERS == ppdev->ulOglActiveViews) )
    {
        ASSERT(ppdev->bOglSingleBackDepthCreated);

        // Although running in a stereo environment it isn't necessary 
        // to enable the shutter glasses and double pumping.
        if (ppdev->bOglStereoActive)
        {
            switch (ppdev->oglRegApiStereoMode)
            {
                case NV_REG_OGL_API_STEREO_MODE_SHUTTER_GLASSES:
                {
                    bOglStereoGlassesSwitchOff(ppdev);
                    break;
                }

                case NV_REG_OGL_API_STEREO_MODE_TVINVIEW_LEFT_DAC0         :
                case NV_REG_OGL_API_STEREO_MODE_TVINVIEW_RIGHT_DAC0        :
                {
                    ASSERTDD(FALSE, "not tested NV_REG_OGL_API_STEREO_MODE_TVINVIEW!");
                    if (ppdev->ulNumberDacsActive > 1)
                    {
                        ULONG ulHead = ppdev->ulDeviceDisplay[1];
                        NvSetDacImageOffset(ppdev, ulHead,
                            ppdev->rclDisplayDesktop[ulHead].left * ppdev->cjPelSize + ppdev->rclDisplayDesktop[ulHead].top * ppdev->lDelta);
                    }
                    break;
                }

                case NV_REG_OGL_API_STEREO_MODE_VERTICAL_INTERLACED_LEFT_0 :
                case NV_REG_OGL_API_STEREO_MODE_VERTICAL_INTERLACED_RIGHT_0:
                default:
                {
                    break;
                }
            }

            if ( bDoublePumped(ppdev, ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT]) )
            {
                NV_RemoveDoubleBufferSurface(ppdev, ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT]);
            }

            ppdev->bOglStereoActive = FALSE;
            // active, need to disable it now
        }

        ASSERT(!ppdev->bOglStereoActive);

        bRet = TRUE;
    }
    return (bRet);
}


//*************************************************************************
// bOglStereoModeUpdate
//
// run through client list and compare against system settings to consider 
// if stereo mode (glasses+double pumping) has to be enabled or not.
// This is used to dynamically toggle between glasses on with GDI double 
// pumping and OpenGL broadcasting to right front and glasses off an a 
// monoscopic usage of the left buffers only. Doing this will increase 
// performance for all non stereo apps in a stereo environment or when 
// all stereo apps are occluded.
//
// returns TRUE if stereomode is enabled
//*************************************************************************
BOOL bOglStereoModeUpdate(PPDEV ppdev)
{
    ASSERT(NULL!=ppdev);

    if (   ppdev->bOglStereoDesired
        && (OGL_STEREO_NUMBUFFERS == ppdev->ulOglActiveViews) )
    {
        ASSERT(ppdev->bOglSingleBackDepthCreated);

        if (bOglGetNextActiveStereoClient(NULL))
        {
            bOglStereoModeEnable(ppdev);
        }
        else
        {
            bOglStereoModeDisable(ppdev);
        }
    }

    // return if stero mode is on or off
    return (ppdev->bOglStereoActive);
}

// End of oglstereo.c
