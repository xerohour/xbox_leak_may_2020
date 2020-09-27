//************************** Module Header *************************************
//                                                                             *
//  Module Name: nt4MulEscape.c                                                   *
//                                                                             *
//  This module contains the functions that belong to the multi device wrapper *
//                                                                             *
//   Copyright (C) 2000 NVidia Corporation. All Rights Reserved.               *
//                                                                             *
//******************************************************************************


#include "precomp.h"
#include "driver.h"
#include "RectUtils.h"
#include "nv32.h"       // needed for NV01_DEVICE_0 #define
#include "nt4multidev.h"

#ifdef NT4_MULTI_DEV

BOOL bFindDeviceRMRoot( PBOARDDESC pbdDevices, ULONG ulRoot )
{
    BOOL  bRet = FALSE;
    ULONG ul   = 0;

    ASSERT( NULL != pbdDevices );
    ASSERT( 0 != ulRoot ); // 0 means not initialized !

    for( ul = 0; ul < MAX_MULTIDEV; ul ++ )
    {
        if( pbdDevices[ul].ulRoot == ulRoot )
        {
            bRet = TRUE;
            break;
        }
    }

    return bRet;
}


//******************************************************************************
//
//  MulEscape
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
ULONG MulEscape(SURFOBJ*    pso, ULONG       iEsc, ULONG       cjIn,
                VOID*       pvIn, ULONG       cjOut, VOID*       pvOut)
{
    PPDEV ppdev = NULL;
    ULONG ulRet = 0;

    DISPDBG((100, "MulEscape >>>"));

    ppdev = (PPDEV)pso->dhpdev; 

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        ulRet = DrvEscape(pso, iEsc, cjIn, pvIn, cjOut, pvOut);
    }
    else
    {

        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        // check for all ESCAPES related to the multimon stuff first
        switch(iEsc)
        {
            case ESC_NV_OPENGL_ESCAPE:
            {
                NV_OPENGL_COMMAND *inCommand;

                inCommand = (NV_OPENGL_COMMAND *)pvIn;

                switch(inCommand->command)
                {
                    case ESC_NV_OPENGL_NT4_GET_NUMBER_OF_MONITORS:
                        ((ULONG *)pvOut)[0] = pmdev->ulNumDevicesActive;
                        ulRet = (ULONG)TRUE + 1;
                        break;
                    case ESC_NV_OPENGL_NT4_GET_DEVICE_HANDLE:
                        ASSERT(inCommand->devID < MAX_MULTIDEV);
                        ((ULONG *)pvOut)[0] = (ULONG)pmdev->abdDevices[inCommand->devID].hDriver;
                        ulRet = (ULONG)TRUE + 1;
                        break;
                    case ESC_NV_OPENGL_NT4_GET_DEVICE_RECT:
                    {
                        RECTL *prcl;
                        prcl = (RECTL *)pvOut;
                        ASSERT(inCommand->devID < MAX_MULTIDEV);
                        *prcl = pmdev->abdDevices[inCommand->devID].rclBoard;
                        ulRet = (ULONG)TRUE + 1;
                        break;
                    }
                    case ESC_NV_OPENGL_NT4_GDI_LOCK:
                    {
                        // this call is used from ICD softwarefunctions to check if their drawable
                        // is accessible => check against modeswitches etc.
                        if(  ( ((PDEV*)(pmdev->abdDevices[inCommand->devID].pso->dhpdev))->bEnabled )
                           &&( !(((PDEV*)(pmdev->abdDevices[inCommand->devID].pso->dhpdev))->dwGlobalModeSwitchCount & 0x1) )
                          )
                        {
                            ulRet = (ULONG)TRUE + 1;
                        }
                        else
                        {
                            // here we tell ICD that Escape reached server side, but did not succeed;
                            ulRet = (ULONG)TRUE;
                        }
                        break;
                    }
                    default:
                        // all other ESCAPES ( only ESC_NV_OPENGL_ESCAPE so far ) should
                        // have DevID filled out to reach the desired device
                        // otherwise they go to device 0
                        ASSERT(inCommand->devID < MAX_MULTIDEV);
                        ASSERT(pmdev->abdDevices[inCommand->devID].pso);
                        ulRet = DrvEscape(pmdev->abdDevices[inCommand->devID].pso, iEsc, cjIn, pvIn, cjOut, pvOut);
                        break;
                }
                break;
            }

            // this is one of the few calls we have to redirect to the right device
            // devicenumber is  given as inCommand->hClass - NV01_DEVICE_0
            case NV_ESC_RM_ALLOC_DEVICE:
            {
                NVOS06_PARAMETERS *inParms;

                ASSERT( cjIn == sizeof(NVOS06_PARAMETERS) );
                inParms  = (NVOS06_PARAMETERS *)pvIn;

                ulDev = inParms->hClass - NV01_DEVICE_0;
                ASSERT(ulDev < MAX_MULTIDEV);

                // remember ParentObject ( created by alloc_root ) to redirect rm-calls
                // only take 1st one, others are not used elsawhere
                if( 0 == pmdev->abdDevices[ulDev].ulRoot )
                {
                    pmdev->abdDevices[ulDev].ulRoot = inParms->hObjectParent;
                }

                DISPDBG((95, "Redirecting NV_ESC_RM_ALLOC_DEVICE to device No: %d ( hRoot: 0x%x )",ulDev,inParms->hObjectParent));

                ASSERT(pmdev->abdDevices[ulDev].pso);
                ulRet = DrvEscape(pmdev->abdDevices[ulDev].pso, iEsc, cjIn, pvIn, cjOut, pvOut);

                break;
            }
            case ESC_NV_QUERYSET_REGISTRY_KEY: 
            {
                __PNVRegKeyInfo inParms, outParms;

                inParms = (__PNVRegKeyInfo) pvIn;

                ulDev = inParms->devID;

                // this is a hot fox for tools which do not 
                if( ulDev >= pmdev->ulNumDevicesActive )
                {
                    //ASSERT(ulDev < MAX_MULTIDEV);
                    ulDev = 0;
                }

                DISPDBG((0, "Redirecting ESC_NV_QUERYSET_REGISTRY_KEY to device No: %d ",ulDev));

                ASSERT(pmdev->abdDevices[ulDev].pso);
                ulRet = DrvEscape(pmdev->abdDevices[ulDev].pso, iEsc, cjIn, pvIn, cjOut, pvOut);
                break;
            }

            case WNDOBJ_SETUP:
            {
                NV_WNDOBJ_SETUP *pNvWndObj;
                ULONG            ulDev;

                pNvWndObj  = (NV_WNDOBJ_SETUP *)pvIn;

                ulDev = pNvWndObj->devID;
                DISPDBG((0, "WNDOBJ_SETUP called with ulDev: 0x%x )",pNvWndObj->devID));

                ASSERT(pmdev->abdDevices[ulDev].pso);
                ulRet = DrvEscape(pmdev->abdDevices[ulDev].pso, iEsc, cjIn, pvIn, cjOut, pvOut);
                break;
            }


            case NV_ESC_RM_ARCH_HEAP:
            {
                NVOS11_PARAMETERS *inParms;
                ULONG              ulDev;

                inParms  = (NVOS11_PARAMETERS *)pvIn;
                DISPDBG((0, "NV_ESC_RM_ARCH_HEAP called with hRoot: 0x%x )",inParms->hRoot));

                if( bFindDeviceRMRoot( pmdev->abdDevices, inParms->hRoot ) )
                {
                    // found matching device !
                    DISPDBG((0, "Redirecting NV_ESC_RM_ARCH_HEAP to device No: %d  ( hClass: 0x%x )",ulDev,inParms->hRoot));
//                    ulRet = DrvEscape(pmdev->abdDevices[0].pso, iEsc, cjIn, pvIn, cjOut, pvOut);
//                    break;
                }
                // check for client to redirect it to the right client
    
//                break;
            }

            default:
                ulRet = DrvEscape(pmdev->abdDevices[0].pso, iEsc, cjIn, pvIn, cjOut, pvOut);
                break;
        }
    }

    DISPDBG((100, "<<< MulEscape"));
    return ulRet;
}


//******************************************************************************
//
//  MulDrawEscape
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
ULONG MulDrawEscape(SURFOBJ *pso, ULONG    iEsc, CLIPOBJ *pco,
                    RECTL   *prcl, ULONG    cjIn, PVOID    pvIn)
{
    PPDEV ppdev = NULL;
    ULONG ulRet = 0;

    DISPDBG((100, "MulDrawEscape >>>"));

    ppdev = (PPDEV)pso->dhpdev; 

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        ulRet = DrvDrawEscape(pso, iEsc, pco, prcl, cjIn, pvIn);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        for( ulDev=0; ulDev < pmdev->ulNumDevicesActive; ulDev++ )
        {
            ASSERT(pmdev->abdDevices[ulDev].pso);
            ulRet = DrvDrawEscape(pmdev->abdDevices[ulDev].pso, iEsc, pco, prcl, cjIn, pvIn);
            if( -1 == (LONG)ulRet )
                break;
        }
    }

    DISPDBG((100, "<<< MulDrawEscape"));
    return ulRet;
}


//******************************************************************************
//
//  MulSwapBuffers
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL APIENTRY MulSwapBuffers(SURFOBJ *psoDst, WNDOBJ *pwo)
{
    PPDEV ppdev = NULL;
    BOOL  bRet  = FALSE;

    DISPDBG((100, "MulSwapBuffers >>>"));

    ppdev = (PPDEV)psoDst->dhpdev; 

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvSwapBuffers(psoDst, pwo);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        // this is a special case !! because of the PWO
        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            ASSERT(pmdev->abdDevices[ulDev].pso);
            bRet = DrvSwapBuffers(pmdev->abdDevices[ulDev].pso, pwo);
        }
    }

    DISPDBG((100, "<<< MulSwapBuffers - %d", bRet));
    return bRet;
}


//******************************************************************************
//
//  bPixelFormatMatch
//
//  NT4 multiboard helper function which checks if 2 given pixelformats
//  do exactly match
//
//  return value FALSE: pixelformats do not match
//               TRUE:  pixelformats do exactly match
//
//******************************************************************************
BOOL bPixelFormatMatch( PIXELFORMATDESCRIPTOR* ppfd1,  PIXELFORMATDESCRIPTOR* ppfd2 )
{
    BOOL bRet = FALSE;
    
    if(   (ppfd1->nSize           == ppfd2->nSize) 
        &&(ppfd1->nVersion        == ppfd2->nVersion)
        &&(ppfd1->dwFlags         == ppfd2->dwFlags)
        &&(ppfd1->iPixelType      == ppfd2->iPixelType)
        &&(ppfd1->cColorBits      == ppfd2->cColorBits)
        &&(ppfd1->cRedBits        == ppfd2->cRedBits)
        &&(ppfd1->cRedShift       == ppfd2->cRedShift)
        &&(ppfd1->cGreenBits      == ppfd2->cGreenBits)
        &&(ppfd1->cGreenShift     == ppfd2->cGreenShift)
        &&(ppfd1->cBlueBits       == ppfd2->cBlueBits)
        &&(ppfd1->cBlueShift      == ppfd2->cBlueShift)
        &&(ppfd1->cAlphaBits      == ppfd2->cAlphaBits)
        &&(ppfd1->cAlphaShift     == ppfd2->cAlphaShift)
        &&(ppfd1->cAccumBits      == ppfd2->cAccumBits)
        &&(ppfd1->cAccumRedBits   == ppfd2->cAccumRedBits)
        &&(ppfd1->cAccumGreenBits == ppfd2->cAccumGreenBits)
        &&(ppfd1->cAccumBlueBits  == ppfd2->cAccumBlueBits)
        &&(ppfd1->cAccumAlphaBits == ppfd2->cAccumAlphaBits)
        &&(ppfd1->cDepthBits      == ppfd2->cDepthBits)
        &&(ppfd1->cStencilBits    == ppfd2->cStencilBits)
        &&(ppfd1->cAuxBuffers     == ppfd2->cAuxBuffers)
        &&(ppfd1->iLayerType      == ppfd2->iLayerType)
        &&(ppfd1->bReserved       == ppfd2->bReserved)
        &&(ppfd1->dwLayerMask     == ppfd2->dwLayerMask)
        &&(ppfd1->dwVisibleMask   == ppfd2->dwVisibleMask)
        &&(ppfd1->dwDamageMask    == ppfd2->dwDamageMask)
       )
       bRet = TRUE;

    return bRet;
}


//******************************************************************************
//
//  lReallocateMulPixelFormats
//
//  NT4 multiboard wrapper function which calculates a pixelformat list which 
//  contains only matching pixelformats of ALL enabled devices. This list is
//  stored in pmdev
//
//  return value is the number of matching pixelformats
//
//******************************************************************************
LONG lReallocateMulPixelFormats( PMDEV pmdev )
{
    PMUL_PIXELFORMAT         pmpfdFirstDevice;
    LONG                     lNumPixelFormatsFirstDevice;
    PIXELFORMATDESCRIPTOR    pfdTemp;
    LONG                     lNumPixelFormatsTemp;
    LONG                     lRet = 0;
    LONG                     l;
    lRet = 0;

    ASSERT(pmdev);
    ASSERT(pmdev->ulNumDevicesActive > 1);  // function is only designed for more than 1 device

    // we have to reallocate and fill a pixelformatlist 
    if( 0 != pmdev->PixelFormatList.iNumberOfPixelFormats )
    {
        ASSERT(pmdev->PixelFormatList.pPixelFormats);
        EngFreeMem(pmdev->PixelFormatList.pPixelFormats);
        pmdev->PixelFormatList.iNumberOfPixelFormats = 0;
    }

    lNumPixelFormatsFirstDevice = DrvDescribePixelFormat(pmdev->abdDevices[0].dhpdev, 1, 0, NULL);

    if( lNumPixelFormatsFirstDevice > 0 )
    {
        pmpfdFirstDevice = (PMUL_PIXELFORMAT)EngAllocMem(FL_ZERO_MEMORY, 
            (lNumPixelFormatsFirstDevice + 1)* sizeof(MUL_PIXELFORMAT), ALLOC_TAG); 
        if(pmpfdFirstDevice)
        {
            ULONG ulDev;

            lRet = lNumPixelFormatsFirstDevice;

            // get all pixelformats from 1st device
            for(l=1; l<=lNumPixelFormatsFirstDevice; l++)
            {
                LONG ll;
                ll = DrvDescribePixelFormat(pmdev->abdDevices[0].dhpdev, l
                        , sizeof(PIXELFORMATDESCRIPTOR), &(pmpfdFirstDevice[l].pfd));
                // remember number of pixelformat for device 0
                pmpfdFirstDevice[l].iPixelFormat[0]=l;
            }

            // now loop over all other devices and reset pixelformats in 1st device's list which does not match
            for(ulDev=1;ulDev<pmdev->ulNumDevicesActive;ulDev++)
            {
                LONG ll;
                lNumPixelFormatsTemp = DrvDescribePixelFormat(pmdev->abdDevices[ulDev].dhpdev, 1, 0, NULL);
                
                for(l=1; l <= lNumPixelFormatsFirstDevice; l++)
                {
                    if( 0 != pmpfdFirstDevice[l].pfd.nSize )
                    {
                        ll = DrvDescribePixelFormat(pmdev->abdDevices[ulDev].dhpdev, l, sizeof(PIXELFORMATDESCRIPTOR), &pfdTemp);
                        if( !bPixelFormatMatch(&(pmpfdFirstDevice[l].pfd),&pfdTemp ) )
                        {
                            // mark pixelformat as not valid
                            pmpfdFirstDevice[l].pfd.nSize = 0;
                            lRet--;
                            ASSERT(lRet>=0);
                        }
                        else
                        {
                            // found a matching pixelformat => remember number for device ulDev
                            pmpfdFirstDevice[l].iPixelFormat[ulDev]=l;
                        }
                    }
                }
            }
        
            // here we have a list of valid pixelformats ( lRet is the number of it ) 
            // and indices for each device which points to it 
            // now copy valid pixelformats into pmdev
            pmdev->PixelFormatList.iNumberOfPixelFormats = 0;
            pmdev->PixelFormatList.pPixelFormats 
                = (PMUL_PIXELFORMAT)EngAllocMem(FL_ZERO_MEMORY, lRet * sizeof(MUL_PIXELFORMAT), ALLOC_TAG);
            ASSERT(pmdev->PixelFormatList.pPixelFormats);
            for( l=1;l <= lNumPixelFormatsFirstDevice;l++)
            {
                if( 0 != pmpfdFirstDevice[l].pfd.nSize )
                {
                    // copy pixelformat and indices into pmdev
                    pmdev->PixelFormatList.pPixelFormats[pmdev->PixelFormatList.iNumberOfPixelFormats] = pmpfdFirstDevice[l];
                    // remember copied number
                    pmdev->PixelFormatList.iNumberOfPixelFormats++;  
                    ASSERT(lRet>=pmdev->PixelFormatList.iNumberOfPixelFormats);
                }
            }

            EngFreeMem(pmpfdFirstDevice);
        }
    }

    return lRet;
}


//******************************************************************************
//
//  MulSetPixelFormat
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL APIENTRY MulSetPixelFormat(
    SURFOBJ *pso,
    LONG    iPixelFormat,
    HWND    hWnd)
{
    PPDEV ppdev = NULL;
    BOOL  bRet  = FALSE;

    DISPDBG((100, "MulSetPixelFormat >>>"));

    ppdev = (PPDEV)pso->dhpdev; 

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvSetPixelFormat(pso, iPixelFormat, hWnd);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;
        LONG    lPixelFormatDevice;

        ASSERT(NULL!=pmdev->PixelFormatList.pPixelFormats);

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            // get matching pixelformat of device
            lPixelFormatDevice = pmdev->PixelFormatList.pPixelFormats[iPixelFormat].iPixelFormat[ulDev];

            bRet =  DrvSetPixelFormat(pmdev->abdDevices[ulDev].pso, lPixelFormatDevice, hWnd);
            if( !bRet )
            {
                ASSERT(FALSE);
                break;
            }
        }
    }

    DISPDBG((100, "<<< MulSetPixelFormat"));
    return bRet;
}


//******************************************************************************
//
//  MulDescribePixelFormat
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
LONG APIENTRY MulDescribePixelFormat(
    DHPDEV                  dhpdev,
    LONG                    iPixelFormat,
    ULONG                   cjpfd,
    PIXELFORMATDESCRIPTOR   *ppfd)
{
    LONG  lRet  = 0;
    PPDEV ppdev = (PPDEV)dhpdev;

    DISPDBG((100, "MulDescribePixelFormat >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        lRet = DrvDescribePixelFormat(dhpdev, iPixelFormat, cjpfd, ppfd);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  

        //we do a match here to export only pixelformats which exist on ALL devices !
        // app does query us for a pixelformatlist
        if( NULL == ppfd )
        {
            lRet = lReallocateMulPixelFormats(pmdev);
        }
        else
        {
            ASSERT(NULL!=pmdev->PixelFormatList.pPixelFormats);

            lRet = pmdev->PixelFormatList.iNumberOfPixelFormats;
            if( cjpfd >= sizeof(PIXELFORMATDESCRIPTOR) )
                // our multimon pixelformatlist is zero based !!
                (*ppfd) = pmdev->PixelFormatList.pPixelFormats[iPixelFormat-1].pfd;
        }
    }

    DISPDBG((100, "<<< MulDescribePixelFormat"));
    return lRet;
}

#endif // NT4_MULTI_DEV



