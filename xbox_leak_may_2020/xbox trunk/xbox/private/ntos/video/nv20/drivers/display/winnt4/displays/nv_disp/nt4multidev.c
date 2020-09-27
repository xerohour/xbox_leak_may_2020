//************************** Module Header *************************************
//                                                                             
//  Module Name: nt4multidev.c                                                 
//                                                                             
//  This module contains the functions that belong to the multi device wrapper 
//
//
//  NT4 MultiBoard functionality description:
//
// Supports multiple display boards as a single virtual desktop.
//
// This is implemented by presenting to GDI a single large virtual
// display and adding a layer between GDI and the driver's Drv functions.
// For  most part, the rest of the driver outside of multidev wrapper 
// doesn't have to change much, subject to the requirements below.
//
// This implementation requires that each board have the same colour depth,
// the resolution may vary but all of the the boards have to be arranged 
// in a rectangular configuration.
//
// Each board has its own PDEV, and completely manages its surface
// independently.
//
//   Copyright (C) 2000 NVidia Corporation. All Rights Reserved.               
//                                                                             
//*****************************************************************************
#include "precomp.h"
#include "driver.h"
#include "RectUtils.h"

#include "nt4multidev.h"

#ifdef NT4_MULTI_DEV


// this is the functiontable we export in NT4-case to be able to support NT4 multimon
DRVFN gadrvfn[] = {
    {   INDEX_DrvEnablePDEV,            (PFN) MulEnablePDEV         },
    {   INDEX_DrvCompletePDEV,          (PFN) MulCompletePDEV       },
    {   INDEX_DrvDisablePDEV,           (PFN) MulDisablePDEV        },
    {   INDEX_DrvEnableSurface,         (PFN) MulEnableSurface      },
    {   INDEX_DrvDisableSurface,        (PFN) MulDisableSurface     },
    {   INDEX_DrvAssertMode,            (PFN) MulAssertMode         },
    {   INDEX_DrvMovePointer,           (PFN) MulMovePointer        },
    {   INDEX_DrvSetPointerShape,       (PFN) MulSetPointerShape    },
    {   INDEX_DrvDitherColor,           (PFN) DrvDitherColor        },  // can be redirected to 1st board
    {   INDEX_DrvSetPalette,            (PFN) MulSetPalette         },
    {   INDEX_DrvCopyBits,              (PFN) MulCopyBits           },
    {   INDEX_DrvBitBlt,                (PFN) MulBitBlt             },
    {   INDEX_DrvTextOut,               (PFN) MulTextOut            },
    {   INDEX_DrvGetModes,              (PFN) MulGetModes           },
    {   INDEX_DrvLineTo,                (PFN) MulLineTo             },
    {   INDEX_DrvStrokePath,            (PFN) MulStrokePath         },
    {   INDEX_DrvFillPath,              (PFN) MulFillPath           },  // not supported
    {   INDEX_DrvPaint,                 (PFN) MulPaint              },
    {   INDEX_DrvRealizeBrush,          (PFN) MulRealizeBrush       },
    {   INDEX_DrvCreateDeviceBitmap,    (PFN) MulCreateDeviceBitmap },
    {   INDEX_DrvDeleteDeviceBitmap,    (PFN) MulDeleteDeviceBitmap },
//  {   INDEX_DrvStretchBlt,            (PFN) MulStretchBlt         },  // not implemented or hooked
    {   INDEX_DrvDestroyFont,           (PFN) DrvDestroyFont        },  // if we need to implement, we 
#ifndef NVGDI                                                           //need to change FONTOBJ->pvconsumer
    {   INDEX_DrvGetDirectDrawInfo,     (PFN) MulGetDirectDrawInfo  },
    {   INDEX_DrvEnableDirectDraw,      (PFN) MulEnableDirectDraw   },
    {   INDEX_DrvDisableDirectDraw,     (PFN) MulDisableDirectDraw  },
#endif
    {   INDEX_DrvSynchronize,           (PFN) MulSynchronize        },
    {   INDEX_DrvEscape,                (PFN) MulEscape             },
    {   INDEX_DrvDrawEscape,            (PFN) MulDrawEscape         },
    {   INDEX_DrvResetPDEV,             (PFN) MulResetPDEV          },
    {   INDEX_DrvDescribePixelFormat,   (PFN) MulDescribePixelFormat},
    {   INDEX_DrvSetPixelFormat,        (PFN) MulSetPixelFormat     },
    {   INDEX_DrvSwapBuffers,           (PFN) MulSwapBuffers        },
#if (NVARCH >= 0x4)
    {   INDEX_DrvSaveScreenBits,        (PFN) MulSaveScreenBits     },
#endif
};
ULONG gcdrvfn = sizeof(gadrvfn) / sizeof(DRVFN);





BOOL bSwapCompareCallback(RECTL *prcl1, RECTL *prcl2, ULONG	iDir)
{
    BOOL bSwap = FALSE;

    ASSERT(prcl1);
    ASSERT(prcl2);

    switch (iDir)
    {
    case CD_RIGHTDOWN:
        if (prcl1->top > prcl2->top)
        {
            bSwap = TRUE;
        }
        else if (prcl1->top == prcl2->top)
        {
            if (prcl1->left > prcl2->left)
            {
                bSwap = TRUE;
            }
        }
        break;
    case CD_LEFTDOWN:
        if (prcl1->top > prcl2->top)
        {
            bSwap = TRUE;
        }
        else if (prcl1->top == prcl2->top)
        {
            if (prcl1->left < prcl2->left)
            {
                bSwap = TRUE;
            }
        }
        break;
    case CD_RIGHTUP:
        if (prcl1->top < prcl2->top)
        {
            bSwap = TRUE;
        }
        else if (prcl1->top == prcl2->top)
        {
            if (prcl1->left > prcl2->left)
            {
                bSwap = TRUE;
            }
        }
        break;
    case CD_LEFTUP:
        if (prcl1->top < prcl2->top)
        {
            bSwap = TRUE;
        }
        else if (prcl1->top == prcl2->top)
        {
            if (prcl1->left < prcl2->left)
            {
                bSwap = TRUE;
            }
        }
        break;
    case CD_ANY:
    default:
        bSwap = FALSE;
    }
    return bSwap;
}


// some helper funtions
ULONG DEVOBJ_cEnumStart(IN PDEVOBJ  pdo,
                        IN SURFOBJ  *pso,
                        IN RECTL    *prclBounds, // don´t care if bAll = TRUE
                        IN BOOL     bAll,       // TRUE: enumerates all, FALSE enumerates only dev clipped against prclbounds
                        IN ULONG    iDir)  // CD_ANY...
{
    PMDEV   pmdev;
    ULONG   ul;
    ULONG   ulRet;
    ULONG   ulOuter;
    ULONG   ulInner;

    pdo->pso = pso;

    if (bNeedToEnumerate(pso))
    {
        pmdev               = ((PPDEV)pso->dhpdev)->pmdev;

        pdo->pmdev          = pmdev;
        pdo->enumStart      = 0;

        // copy to pointer array
        for (ul = 0; ul < pmdev->ulNumDevicesActive; ul++)
        {
            pdo->apbdDevices[ul] = &pdo->pmdev->abdDevices[ul];
            pdo->enumStart++;
        }

        if (pmdev->ulNumDevicesActive > 1)
        {
            for (ulOuter = 0; ulOuter < (pmdev->ulNumDevicesActive - 1); ulOuter++)
            {
                for (ulInner = ulOuter + 1; ulInner < pmdev->ulNumDevicesActive; ulInner++)
                {
                    if (bSwapCompareCallback(&pdo->apbdDevices[ulOuter]->rclBoard, &pdo->apbdDevices[ulInner]->rclBoard, iDir))
                    {
                        PBOARDDESC pScratch;
                        pScratch = pdo->apbdDevices[ulOuter];
                        pdo->apbdDevices[ulOuter] = pdo->apbdDevices[ulInner];
                        pdo->apbdDevices[ulInner] = pScratch;
                    }
                }
            }
        }

        pdo->cDevices = pmdev->ulNumDevicesActive;
        ulRet = pmdev->ulNumDevicesActive;
    }
    else
    {
        pdo->pmdev      = NULL;
        pdo->cDevices   = 1; 
        ulRet           = 1;
    }

    return (ulRet);
}

BOOL DEVOBJ_bEnum(IN PDEVOBJ    pdo,
                  IN ENUMDEV16  *pEnumDev16)
{
    ULONG ul;
    BOOL    bRet;

    ASSERT(pdo);
    ASSERT(pEnumDev16);

    pEnumDev16->c = pdo->cDevices;

    if (pdo->pmdev)
    {
        for (ul = 0; ul < pEnumDev16->c; ul++)
        {
            pEnumDev16->apbdDevices[ul] = pdo->apbdDevices[ul];
            pdo->enumStart++;
        }
        bRet = pdo->enumStart < pdo->pmdev->ulNumDevicesActive ? TRUE : FALSE;
    }
    else
    {
        ASSERT(pEnumDev16->c == 1);
        pEnumDev16->apbdDevices[0] = NULL;
        pdo->enumStart++;
        bRet = FALSE;
    }
    return bRet;
}
                        



// the wrapper functions


//******************************************************************************
//
//  bGetNumberOfDevices
//
//  NT4 multiboard helper function to get number of initialized (nVidia graphics )
//  devices from miniport
//
//  returns TRUE  if call to miniport succeded
//          FALSE if call to miniport did fail
//
//******************************************************************************
BOOL bGetNumberOfDevices( HANDLE hDriver, ULONG_PTR* pulNumberOfDevices )
{
    BOOL                        bRet = FALSE;
    QUERY_NUM_OF_DEVICE_OBJECTS NumOfDevObj;
    ULONG                       ulReturnedDataLength;

    ASSERT(pulNumberOfDevices);
    ASSERT(hDriver);

    NumOfDevObj.hDriver     = hDriver;

    // call minipoirt to get adresses of all initialized device handles
    if(  ( EngDeviceIoControl(hDriver,
                              IOCTL_VIDEO_QUERY_NUM_OF_DEVICE_OBJECTS,
                              &NumOfDevObj,
                              sizeof(QUERY_NUM_OF_DEVICE_OBJECTS),
                              &NumOfDevObj,
                              sizeof(QUERY_NUM_OF_DEVICE_OBJECTS),
                              &ulReturnedDataLength ) )
       ||( ulReturnedDataLength != sizeof(QUERY_NUM_OF_DEVICE_OBJECTS) )
      ) 
    {
        DISPDBG((0, "bGetNumberOfDevices - IOCTL_VIDEO_QUERY_NUM_OF_DEVICE_OBJECTS failed"));
        pulNumberOfDevices = 0;
    }
    else
    {
        DISPDBG((95, "bGetNumberOfDevices - IOCTL_VIDEO_QUERY_NUM_OF_DEVICE_OBJECTS found %d device objects", NumOfDevObj.ulNumDevObj));
        *pulNumberOfDevices  = NumOfDevObj.ulNumDevObj;
        bRet                 = TRUE;
    }

    return bRet;
}


//******************************************************************************
//
//  bGetDeviceObjects
//
//  NT4 multiboard helper function to get handles to initialized (nVidia graphics )
//  devices from miniport
//
//  returns TRUE  if call to miniport succeded
//          FALSE if call to miniport did fail
//
//******************************************************************************
BOOL bGetDeviceObjects( HANDLE hDriver, HANDLE ahDriver[MAX_MULTIDEV])
{
    BOOL  bRet     = FALSE;
    ULONG ulDevNum = 0;

    ASSERT(ahDriver);
    ASSERT(hDriver);

    // preset hDriverarray to zero
    RtlZeroMemory(ahDriver,sizeof(HANDLE)*MAX_MULTIDEV);

    // does miniport support multibple devices and do we have one or more ?
    if(   (bGetNumberOfDevices(hDriver,&ulDevNum))
        &&(ulDevNum > 0 )
       )
    {
        GET_DEVICE_OBJECTS  *pGetDevObj;
        ULONG                ulSizeOfGetDeviceObjects; 
        ULONG                ulReturnedDataLength;
        
        // allocate right size for miniport access
        ulSizeOfGetDeviceObjects = sizeof(GET_DEVICE_OBJECTS) + (ulDevNum - 1) * sizeof (HANDLE);
        pGetDevObj = (GET_DEVICE_OBJECTS *)EngAllocMem(FL_ZERO_MEMORY, ulSizeOfGetDeviceObjects, ALLOC_TAG); ;
        if (pGetDevObj)
        {
            pGetDevObj->hDriver     = hDriver;
            pGetDevObj->ulNumDevObj = ulDevNum;

            // ask miniport for hDriver of all nvidia graphic devices in system
            if (EngDeviceIoControl(hDriver,
                                   IOCTL_VIDEO_GET_DEVICE_OBJECTS,
                                   pGetDevObj,
                                   ulSizeOfGetDeviceObjects,
                                   pGetDevObj,
                                   ulSizeOfGetDeviceObjects,
                                   &ulReturnedDataLength) || ulReturnedDataLength != ulSizeOfGetDeviceObjects)
            {
                DISPDBG((0, "bGetDeviceObjects - IOCTL_VIDEO_GET_DEVICE_OBJECTS failed"));
            }
            else
            {
                ULONG ulDev;
                DISPDBG((95, "bGetDeviceObjects - hDriver 0x%x found %d devices", hDriver,ulDevNum));
                for (ulDev = 0; ulDev < ulDevNum; ulDev++)
                {
                    ahDriver[ulDev] = pGetDevObj->ahDriver[ulDev];
                    DISPDBG((95, "bGetDeviceObjects - no %d: hDriver: 0x%x",ulDev,ahDriver[ulDev]));
                }
                bRet = TRUE;
            }

            EngFreeMem(pGetDevObj);
        }
    }

    return bRet;
}


//******************************************************************************
//
//  bIsMultiMonMode
//
//  NT4 multiboard helper function check whether given mode is a multimon mode
//  or not
//
//  returns TRUE  if given mode is a multi device
//          FALSE if given mode is no multi device mode
//
//******************************************************************************
BOOL bIsMultiMonMode( HANDLE hDriver, DEVMODEW* pdm)
{
    BOOL    bRet = FALSE;
    ULONG   ulSingleDevWidth;  // width  of each single device    
    ULONG   ulSingleDevHeight; // height of each single device    
    ULONG   ulModeDescription; // width << 16 | height to be able to do a switch

    ASSERT(pdm);
    ASSERT(hDriver);
    // we may have to find another mechanism to check wether we need a
    // multimon resolution or not

    // try to calculate an aspect ration for desired mode and check
    // wheter it is an multimon mode or not
    ulSingleDevWidth  = pdm->dmPelsWidth;
    ulSingleDevHeight = pdm->dmPelsHeight;

    if (ulSingleDevWidth && ulSingleDevHeight)
    {
        ulModeDescription = (ulSingleDevWidth << 16) | ulSingleDevHeight;
        
        switch( ulModeDescription )
        {
        case 0x08000300: // 2048 *  768
        case 0x04000600: // 1024 * 1536
        case 0x08000400: // 2048 * 1536
            bRet = TRUE;
            break;
        default:
            break;
        }
    }
    else
    {
        ASSERT(FALSE);
    }

    return bRet;
}


//******************************************************************************
//
//  bGetSingleDeviceRect
//
//  NT4 multiboard helper function calcualtes the rect of the given device
//
//  returns TRUE  if rect could be calculated
//          FALSE if rect not could be calculated
//
//******************************************************************************
BOOL bGetSingleDeviceRect( HANDLE hDriver, DEVMODEW* pdm, ULONG ulDev, RECTL* prclDevice )
{
    BOOL bRet = FALSE;

    ASSERT(pdm);
    ASSERT(hDriver);
    ASSERT(prclDevice);
    ASSERT(ulDev < MAX_MULTIDEV);

    if(bIsMultiMonMode(hDriver,pdm))
    {
        // for now just track 1 mode: 800 * 300
        switch( (pdm->dmPelsWidth << 16) | pdm->dmPelsHeight)
        {
        case 0x08000300:
        case 0x0B000300:
        case 0x10000300:
            prclDevice->left   =  ulDev     * 0x400;
            prclDevice->right  = (ulDev + 1)* 0x400;
            prclDevice->top    = 0;
            prclDevice->bottom = 0x300;
            bRet = TRUE;
            break;
        case 0x04000600:
            prclDevice->left   = 0;
            prclDevice->right  = 0x400;
            prclDevice->top    =  ulDev     * 0x300;
            prclDevice->bottom = (ulDev + 1)* 0x300;
            bRet = TRUE;
            break;
        default:
            break;
        }
    }

    return bRet;
}

//******************************************************************************
//
//  MulEnablePDEV
//
//  NT4 multiboard wrapper function which tries to initialize the desired
//  number of single devices and sets up the multiboard environment
// otherwise it only calls the singleboard DrvEnablePDEV
//
//******************************************************************************
DHPDEV MulEnablePDEV(DEVMODEW* pdm, PWSTR pwszLogAddr, ULONG  cPat, HSURF* phsurfPatterns, 
                     ULONG cjCaps, ULONG* pdevcaps, ULONG cjDevInfo, DEVINFO* pdi,            
                     HDEV  hdev, PWSTR pwszDeviceName, HANDLE hDriver)        
{
    PMDEV   pmdev = NULL;                   // ppdev for multidevice case
    HANDLE  ahDriver[MAX_MULTIDEV];         // device handles from MulEnableDriver
    ULONG   ulNumDev = 0;
    DHPDEV  dhpdevRet = NULL;

    DISPDBG((100, "MulEnablePDEV >>>"));

    ASSERT(pdm);
    ASSERT(hDriver);

    // do we should run in multimon-mode ?
    if(   (bIsMultiMonMode(hDriver,pdm))
        &&(bGetNumberOfDevices( hDriver, &ulNumDev ))
        &&(ulNumDev > 1) // caution: don't remove this check here !
        &&(bGetDeviceObjects(hDriver,ahDriver)) // get hDriver of devices
       )
    {
        // here we know there are ulDev (more than 1) nvidia graphic devices activated and
        // we have objecthandles for them
        pmdev = (PMDEV)EngAllocMem(FL_ZERO_MEMORY, sizeof(MDEV), ALLOC_TAG); 
        if (pmdev)
        {
            ULONG   ulDev = 0;

            // do some multiboard initialisation
            pmdev->dwUniq               = PMDEV_UNIQ;
            pmdev->hdev                 = hdev;
            pmdev->hDriver              = hDriver;
            pmdev->rclDesktop.left      = 0;
            pmdev->rclDesktop.top       = 0;
    //      pmdev->rclDesktop.right     = max (pdm->dmPanningWidth, pdm->dmPelsWidth);
    //      pmdev->rclDesktop.bottom    = max (pdm->dmPanningHeight, pdm->dmPelsHeight);
            pmdev->rclDesktop.right     = pdm->dmPelsWidth;
            pmdev->rclDesktop.bottom    = pdm->dmPelsHeight;
            pmdev->DevMode              = *pdm;
            pmdev->ulNumDevicesActive   = ulNumDev;

            for(ulDev = 0; ulDev < ulNumDev; ulDev++)
            {
                // try to get device rectangle for current mode and device
                if( bGetSingleDeviceRect(hDriver, pdm, ulDev, &pmdev->abdDevices[ulDev].rclBoard) )
                {
                    ASSERT(ahDriver[ulDev]);
                    // fill up necessary board related information
                    pmdev->abdDevices[ulDev].bIsActive              = TRUE;
                    pmdev->abdDevices[ulDev].hDriver                = ahDriver[ulDev];
                    pmdev->abdDevices[ulDev].DevMode                = pmdev->DevMode;
                    pmdev->abdDevices[ulDev].DevMode.dmPelsWidth    = lRclWidth(&pmdev->abdDevices[ulDev].rclBoard);
                    pmdev->abdDevices[ulDev].DevMode.dmPelsHeight   = lRclHeight(&pmdev->abdDevices[ulDev].rclBoard);
                    pmdev->abdDevices[ulDev].pcoBoard               = EngCreateClip();
                    pmdev->abdDevices[ulDev].pcoBoard->iDComplexity = DC_RECT;
                    pmdev->abdDevices[ulDev].pcoBoard->rclBounds    = pmdev->abdDevices[ulDev].rclBoard;
                    pmdev->abdDevices[ulDev].pco                    = EngCreateClip();
                    pmdev->abdDevices[ulDev].pco->iDComplexity      = DC_RECT;
                    pmdev->abdDevices[ulDev].pco->rclBounds         = pmdev->abdDevices[ulDev].rclBoard;
                    pmdev->abdDevices[ulDev].rclSavedBounds         = rclEmpty;
                    pmdev->abdDevices[ulDev].pso                    = NULL;
                    pmdev->abdDevices[ulDev].psoBitmap              = NULL;
                    pmdev->abdDevices[ulDev].cjCaps                 = cjCaps;
                    pmdev->abdDevices[ulDev].cjDevInfo              = cjDevInfo;

                    pmdev->abdDevices[ulDev].pdevcaps = (ULONG *)EngAllocMem(FL_ZERO_MEMORY, pmdev->abdDevices[ulDev].cjCaps, ALLOC_TAG); 
                    if( !pmdev->abdDevices[ulDev].pdevcaps )
                    {
                        ASSERT(pmdev->abdDevices[ulDev].pdevcaps);
                        DISPDBG((0,"MulEnablePDEV: Could not allocate memory for pmdev->abdDevices[%d].pdevcaps",ulDev));
                        break;
                    }
                    pmdev->abdDevices[ulDev].pdi = (DEVINFO *)EngAllocMem(FL_ZERO_MEMORY, pmdev->abdDevices[ulDev].cjDevInfo, ALLOC_TAG); 
                    if( !pmdev->abdDevices[ulDev].pdi )
                    {
                        ASSERT(pmdev->abdDevices[ulDev].pdi);
                        DISPDBG((0,"MulEnablePDEV: Could not allocate memory for pmdev->abdDevices[%d].pdi",ulDev));
                        break;
                    }
                    // here we call the single board driver to allocate a normal singleboard ppdev
                    pmdev->abdDevices[ulDev].dhpdev = DrvEnablePDEV(&pmdev->abdDevices[ulDev].DevMode,
                                                                    NULL, 
                                                                    0, 
                                                                    NULL, 
                                                                    pmdev->abdDevices[ulDev].cjCaps, 
                                                                    pmdev->abdDevices[ulDev].pdevcaps, 
                                                                    pmdev->abdDevices[ulDev].cjDevInfo, 
                                                                    pmdev->abdDevices[ulDev].pdi,
                                                                    pmdev->hdev, 
                                                                    pwszDeviceName, 
                                                                    pmdev->abdDevices[ulDev].hDriver);
                    if( !pmdev->abdDevices[ulDev].dhpdev )
                    {
                        ASSERT(pmdev->abdDevices[ulDev].dhpdev);
                        DISPDBG((0,"MulEnablePDEV: DrvEnablePDEV for device %d failed !",ulDev));
                        break;
                    }
                        // let board know it's boundaries for ICD clip purposes
                    ((PDEV*)(pmdev->abdDevices[ulDev].dhpdev))->left  = pmdev->abdDevices[ulDev].rclBoard.left;
                    ((PDEV*)(pmdev->abdDevices[ulDev].dhpdev))->top   = pmdev->abdDevices[ulDev].rclBoard.top;
                    ((PDEV*)(pmdev->abdDevices[ulDev].dhpdev))->pmdev = pmdev; // remember pmdev in each ppdev
                }
                else
                {
                    DISPDBG((0,"MulEnablePDEV: Could not get rect for device %d !",ulDev));
                    break;
                }
            }
            // here we have to check if something went wrong 
            // on error case release all previously allocated pdevs
            // this is the error case because previous loop runs util ulDev == ulNumDev if no error case
            if(ulDev < ulNumDev )
            {
                for(;ulDev>MAX_MULTIDEV;ulDev-- )
                {
                    if( !pmdev->abdDevices[ulDev].pdevcaps )
                        EngFreeMem(pmdev->abdDevices[ulDev].pdevcaps);
                    if( !pmdev->abdDevices[ulDev].pdi )
                        EngFreeMem(pmdev->abdDevices[ulDev].pdi);
                    if( !pmdev->abdDevices[ulDev].dhpdev )
                        EngFreeMem(pmdev->abdDevices[ulDev].dhpdev);
                }
                EngFreeMem(pmdev);

                DISPDBG((0,"MulEnablePDEV failed !"));
            }
            else
            {
                // everything succeded, now adapt devcaps and return
                RtlCopyMemory(pdi, pmdev->abdDevices[0].pdi, cjDevInfo);
                RtlCopyMemory(pdevcaps, pmdev->abdDevices[0].pdevcaps, cjCaps);
                ((GDIINFO *)pdevcaps)->ulHorzRes        = pmdev->rclDesktop.right - pmdev->rclDesktop.left;
                ((GDIINFO *)pdevcaps)->ulVertRes        = pmdev->rclDesktop.bottom - pmdev->rclDesktop.top;
                ((GDIINFO *)pdevcaps)->ulPanningHorzRes = pmdev->rclDesktop.right - pmdev->rclDesktop.left;
                ((GDIINFO *)pdevcaps)->ulPanningVertRes = pmdev->rclDesktop.bottom - pmdev->rclDesktop.top;
                ((GDIINFO *)pdevcaps)->ulHorzSize       = pmdev->rclDesktop.right - pmdev->rclDesktop.left;
                ((GDIINFO *)pdevcaps)->ulVertSize       = pmdev->rclDesktop.bottom - pmdev->rclDesktop.top;

                // returning the ppdev ot 1st device => we can catch our pmdev out of this !
                dhpdevRet = pmdev->abdDevices[0].dhpdev;
            }
        }
        else
            DISPDBG((0,"MulEnablePDEV: could not allocated Memory for multiple ppdev !!"));

        DISPDBG((95, "MulEnablePDEV: enabled multi device handling"));
    }
    else
    {
        // single board resolution is desired / available => just call singleboard driver with
        // incoming values
        dhpdevRet = DrvEnablePDEV(pdm,pwszLogAddr,cPat,phsurfPatterns, 
                                  cjCaps,pdevcaps,cjDevInfo,pdi,            
                                  hdev,pwszDeviceName,hDriver);
    }

    DISPDBG((100, "<<< MulEnablePDEV"));
    return dhpdevRet;
}


//******************************************************************************
//
//  MulDisablePDEV
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
VOID MulDisablePDEV(DHPDEV  dhpdev)
{
    PPDEV   ppdev = (PPDEV)dhpdev;

    DISPDBG((100, "MulDisablePDEV >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        DrvDisablePDEV(dhpdev);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        // call DrvDisablePDEV for each single device
        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            ASSERT(pmdev->abdDevices[ulDev].dhpdev);
            DrvDisablePDEV(pmdev->abdDevices[ulDev].dhpdev);
            pmdev->abdDevices[ulDev].dhpdev     = NULL;

            EngFreeMem(pmdev->abdDevices[ulDev].pdi);
            pmdev->abdDevices[ulDev].pdi        = NULL;
            pmdev->abdDevices[ulDev].cjDevInfo  = 0;

            EngFreeMem(pmdev->abdDevices[ulDev].pdevcaps);
            pmdev->abdDevices[ulDev].pdevcaps   = NULL;
            pmdev->abdDevices[ulDev].cjCaps     = 0;

            EngDeleteClip(pmdev->abdDevices[ulDev].pco);
            pmdev->abdDevices[ulDev].pco        = NULL;

            EngDeleteClip(pmdev->abdDevices[ulDev].pcoBoard);
            pmdev->abdDevices[ulDev].pcoBoard   = NULL;
        }

        EngFreeMem(pmdev);
        pmdev = NULL;
    }

    DISPDBG((100, "<<< MulDisablePDEV"));
    return;
}

//******************************************************************************
//
//  MulCompletePDEV
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
VOID MulCompletePDEV(DHPDEV dhpdev, HDEV hdev)
{
    PPDEV   ppdev = (PPDEV)dhpdev;

    DISPDBG((100, "MulCompletePDEV >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        DrvCompletePDEV(dhpdev,hdev);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            ASSERT(pmdev->abdDevices[ulDev].dhpdev);
            DrvCompletePDEV(pmdev->abdDevices[ulDev].dhpdev, hdev);
        }
    }

    DISPDBG((100, "<<< MulCompletePDEV"));
    return;
}


//******************************************************************************
//
//  MulResetPDEV
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulResetPDEV(DHPDEV dhpdevOld, DHPDEV dhpdevNew)
{
    PPDEV   ppdevOld = (PPDEV)dhpdevOld;
    PPDEV   ppdevNew = (PPDEV)dhpdevNew;
    BOOL    bRet     = FALSE;

    DISPDBG((100, "MulResetPDEV >>>"));

    ASSERT(ppdevOld);
    ASSERT(ppdevNew);

    
    if( !ppdevNew->pmdev && !ppdevOld->pmdev )
    {
        // both modes are singleboard ones ?
        bRet = DrvResetPDEV(dhpdevOld, dhpdevNew);
    }
    // switch from multiboard to singleboard resolution
    else if( !ppdevNew->pmdev && ppdevOld->pmdev ) 
    {
        // we only allow mode switches when no icd app is running
        if(0==globalOpenGLData.oglDrawableClientCount)
        {
            bRet = DrvResetPDEV(dhpdevOld, dhpdevNew);
        }
    }
    // switch from singleboard to multiboard resolution
    else if(  ppdevNew->pmdev && !ppdevOld->pmdev ) 
    {
        // we only allow mode switches when no icd app is running
        if(0==globalOpenGLData.oglDrawableClientCount)
        {
            PMDEV   pmdevNew = ppdevNew->pmdev;
            ULONG   ulDev;

            for (ulDev = 0; ulDev < pmdevNew->ulNumDevicesActive; ulDev++)
            {
                bRet = DrvResetPDEV(dhpdevOld, pmdevNew->abdDevices[ulDev].dhpdev);
                if(!bRet)
                    goto Exit;
            }
        }
    }
    // switch from one singleboard to another multiboard resolution
    else
    {
        // we only allow mode switches when no icd app is running
        if(0==globalOpenGLData.oglDrawableClientCount)
        {
            PMDEV   pmdevNew = ppdevNew->pmdev;
            ULONG   ulDev;

            ASSERT(ppdevNew->pmdev);
            ASSERT(ppdevOld->pmdev);
            for (ulDev = 0; ulDev < pmdevNew->ulNumDevicesActive; ulDev++)
            {
                bRet = DrvResetPDEV(dhpdevOld, pmdevNew->abdDevices[ulDev].dhpdev);
                if(!bRet)
                    goto Exit;
            }
        }
    }

Exit:
    DISPDBG((100, "<<< MulResetPDEV"));

    return bRet;
}


//******************************************************************************
//
//  MulSynchronize
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
VOID MulSynchronize(DHPDEV dhpdev, RECTL  *prcl)
{
    PPDEV   ppdev = (PPDEV)dhpdev;

    DISPDBG((100, "MulSynchronize >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        DrvSynchronize(dhpdev, prcl);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;
        ULONG   ulDev;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            DrvSynchronize(pmdev->abdDevices[ulDev].dhpdev, prcl);
        }
    }

    DISPDBG((100, "<<< MulSynchronize"));
    return;
}


//******************************************************************************
//
//  MulEnableSurface
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
HSURF MulEnableSurface(DHPDEV dhpdev)
{
    PPDEV   ppdev       = (PPDEV)dhpdev;
    HSURF   hSurfRet    = NULL;

    DISPDBG((100, "MulEnableSurface >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        hSurfRet = DrvEnableSurface(dhpdev);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;
        SIZEL   sizl;
        LONG    lWidth;
        ULONG   iFormat;
        PVOID   pvBits;
        SURFOBJ *pso;
        BOOL    bEAS;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            ppdev                          = (PPDEV)pmdev->abdDevices[ulDev].dhpdev;
            pmdev->abdDevices[ulDev].hsurf = DrvEnableSurface(pmdev->abdDevices[ulDev].dhpdev);

            if( 0 == pmdev->abdDevices[ulDev].hsurf )
            {
                ASSERT(FALSE);
                goto MulExit;
            }

            //  Note: EngAssociateSurface is also responsible for giving the dhpdev to the pso of hsurf
            bEAS                                    = EngAssociateSurface(pmdev->abdDevices[ulDev].hsurf, pmdev->hdev, 0);
            pmdev->abdDevices[ulDev].pso            = EngLockSurface(pmdev->abdDevices[ulDev].hsurf);
            // NOTE: this must be after EngAssociateSurface
            pmdev->abdDevices[ulDev].pso->dhpdev    = pmdev->abdDevices[ulDev].dhpdev;
            pmdev->abdDevices[ulDev].dhsurf         = pmdev->abdDevices[ulDev].pso->dhsurf;

            if (pmdev->abdDevices[ulDev].pso->dhsurf != (DHSURF)&ppdev->dsurfScreen)
            {
                pmdev->abdDevices[ulDev].pso->dhsurf = (DHSURF)&ppdev->dsurfScreen;
            }

            sizl.cx                                 = ppdev->cxScreen;
            sizl.cy                                 = ppdev->cyScreen;
            lWidth                                  = ppdev->lDelta;
            iFormat                                 = ppdev->iBitmapFormat;
            pvBits                                  = ppdev->pjScreen;
            pmdev->abdDevices[ulDev].hsurfBitmap    = EngCreateBitmap(sizl, lWidth, iFormat, BMF_TOPDOWN, pvBits);
            pmdev->abdDevices[ulDev].psoBitmap      = EngLockSurface(pmdev->abdDevices[ulDev].hsurfBitmap);

        }

        pmdev->flHooks = pmdev->abdDevices[0].ppdev->flHooks;

        sizl.cx        = pmdev->rclDesktop.right - pmdev->rclDesktop.left;
        sizl.cy        = pmdev->rclDesktop.bottom - pmdev->rclDesktop.top;
        iFormat;       // still is initialized from the loop
        lWidth         = ((sizl.cx  * (iFormat - BMF_4BPP)) + 3) & 0xFFFFFFFC;
        pvBits;        // still has a non NULL value, don´t care which for now

        // create a hsurf for pmdev, but associate with ppdev[0] to be able to do
        // single board shortcut
        pmdev->hsurf   = EngCreateDeviceSurface(pmdev->abdDevices[ulDev].dhpdev, sizl, iFormat);

        if (!EngAssociateSurface(pmdev->hsurf, pmdev->hdev, pmdev->flHooks))
        {
            EngDeleteSurface(pmdev->hsurf);
            pmdev->hsurf = NULL;

            // cleanup has to be done here !
            ASSERT( FALSE);
            goto MulExit;
        }

        pmdev->pco                  = EngCreateClip();
        pmdev->pco->iMode           = TC_RECTANGLES;
        pmdev->pco->iDComplexity    = DC_RECT;
        pmdev->pco->rclBounds       = pmdev->rclDesktop;

        hSurfRet = pmdev->hsurf;

MulExit:
        if( !hSurfRet )
        {
            // cleanup necessary !
            for(; ulDev >= 0; ulDev-- )
            {
                EngUnlockSurface(pmdev->abdDevices[ulDev].psoBitmap);
                pmdev->abdDevices[ulDev].psoBitmap      = NULL;

                EngDeleteSurface(pmdev->abdDevices[ulDev].hsurfBitmap);
                pmdev->abdDevices[ulDev].hsurfBitmap = NULL;

                EngUnlockSurface(pmdev->abdDevices[ulDev].pso);
                pmdev->abdDevices[ulDev].pso = NULL;
                DrvDisableSurface(pmdev->abdDevices[ulDev].dhpdev);
            }
        }
    }

    DISPDBG((100, "<<< MulEnableSurface"));
    return hSurfRet;
}


//******************************************************************************
//
//  MulDisableSurface
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
VOID MulDisableSurface(DHPDEV dhpdev)
{
    PPDEV ppdev = (PPDEV)dhpdev;

    DISPDBG((100, "MulDisableSurface >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        DrvDisableSurface(dhpdev);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            EngUnlockSurface(pmdev->abdDevices[ulDev].psoBitmap);
            pmdev->abdDevices[ulDev].psoBitmap      = NULL;

            EngDeleteSurface(pmdev->abdDevices[ulDev].hsurfBitmap);
            pmdev->abdDevices[ulDev].hsurfBitmap = NULL;

            EngUnlockSurface(pmdev->abdDevices[ulDev].pso);
            pmdev->abdDevices[ulDev].pso = NULL;
            DrvDisableSurface(pmdev->abdDevices[ulDev].dhpdev);
        }

        EngDeleteClip(pmdev->pco);
        pmdev->pco  = NULL;

        EngUnlockSurface(pmdev->pso);
        pmdev->pso = NULL;
        EngDeleteSurface(pmdev->hsurf);
        pmdev->hsurf = NULL;
    }

    DISPDBG((100, "<<< MulDisableSurface"));
    return;
}


//******************************************************************************
//
//  MulAssertMode
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulAssertMode(DHPDEV  dhpdev,BOOL    bEnable)
{
    PPDEV ppdev = (PPDEV)dhpdev;
    BOOL  bRet  = FALSE;

    DISPDBG((100, "MulAssertMode >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvAssertMode(dhpdev,bEnable);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            // we should not fail here !!!
            bRet = DrvAssertMode(pmdev->abdDevices[ulDev].dhpdev, bEnable);
            ASSERT(bRet);
            if( !bRet  && (0 == ulDev) )
                goto Exit;
        }
    }

Exit:

    DISPDBG((100, "<<< MulAssertMode"));
    return bRet;
}


//******************************************************************************
//
//  MulGetModes
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
ULONG MulGetModes(HANDLE hDriver, ULONG cjSize, DEVMODEW*   pdm)
{
    ULONG   ulDev;
    ULONG   ulRet = 0;
    ULONG   ulNumDev = 0;
    HANDLE  ahDriver[MAX_MULTIDEV];

    ASSERT(hDriver);

    DISPDBG((100, "MulGetModes >>>"));

    // should we run in multimon-mode ?
    if( (bGetNumberOfDevices( hDriver, &ulNumDev ))
        &&(ulNumDev > 1) // caution: don't remove this check here !
        &&(bGetDeviceObjects(hDriver,ahDriver)) // get hDriver of devices
       )
    {
        DEVMODEW *apdm[MAX_MULTIDEV];
        DEVMODEW *pdmDriver;
        DEVMODEW *pdmAppend;
        ULONG     aulSize[MAX_MULTIDEV];
        BOOL      bMatch;
        BOOL      bAllocated = FALSE;

        DISPDBG((95, "MulGetModes - found %d device objects", ulNumDev));

        RtlZeroMemory(apdm, sizeof(apdm));
        RtlZeroMemory(aulSize, sizeof(aulSize));

        // get modes from all devices
        for (ulDev = 0; ulDev < ulNumDev; ulDev++)
        {
            // check needed size for modelist
            aulSize[ulDev] = DrvGetModes(ahDriver[ulDev], 0, NULL);
            if (0 != aulSize[ulDev])
            {
                // alloc memory for modelist
                apdm[ulDev] = EngAllocMem(0, aulSize[ulDev], ALLOC_TAG);
                if (NULL != apdm[ulDev])
                {
                    // copy drivers modelist into allocated space
                    aulSize[ulDev] = DrvGetModes(ahDriver[ulDev], aulSize[ulDev], apdm[ulDev]);
                    if (0 != aulSize[ulDev])
                    {   
                        bAllocated = TRUE;
                    }
                }
            }
            if(!bAllocated)
            {
                DISPDBG((0,"MulGetModes: could not allocate modelist correctly"));
                break;
            }
        }

        // everything worked fine => try to calculate modes to export
        if( bAllocated )
        {
            // system only wants to know how much space is needed for modelist
            if (NULL == pdm)
            {
                ulRet = DrvGetModes(hDriver, cjSize, pdm);

                for (pdmDriver = apdm[0]; (BYTE *)pdmDriver < ((BYTE *)apdm[0] + aulSize[0]); pdmDriver++)
                {
                    bMatch = TRUE;
                    for (ulDev = 0; ulDev < ulNumDev; ulDev++)
                    {
                        bMatch = bMatch && bFindMatchingDevMode(pdmDriver, apdm[ulDev], aulSize[ulDev]);
                    }
                    if (bMatch)
                    {
                        ulRet += sizeof(DEVMODEW);
                    }
                }
            }
            else
            {
                // fill out given list with possible modes
                ulRet = DrvGetModes(hDriver, cjSize, pdm);

                if (ulRet <= (cjSize - sizeof(DEVMODEW)))
                {
                    for (pdmDriver = apdm[0]; (BYTE *)pdmDriver < ((BYTE *)apdm[0] + aulSize[0]); pdmDriver++)
                    {
                        bMatch = TRUE;
                        for (ulDev = 0; ulDev < ulNumDev; ulDev++)
                        {
                            bMatch = bMatch && bFindMatchingDevMode(pdmDriver, apdm[ulDev], aulSize[ulDev]);
                        }
                        if (bMatch)
                        {
                            pdmAppend = (DEVMODEW *)((BYTE *)pdm + ulRet);
                            RtlCopyMemory(pdmAppend, pdmDriver, sizeof(DEVMODEW));
                            pdmAppend->dmPelsWidth *= ulNumDev;

                            ulRet += sizeof(DEVMODEW);
                            break;
                        }
                        if ((ulRet + sizeof(DEVMODEW)) > cjSize)
                            break;
                    }
                }
            }
        }

        // clean up allocated space
        for (ulDev = 0; ulDev < ulNumDev; ulDev++)
        {
            if (apdm[ulDev])
            {
                EngFreeMem(apdm[ulDev]);
                apdm[ulDev] = NULL;
            }
        }
    }
    else
    {
        // normal single board case:
        ulRet = DrvGetModes(hDriver,cjSize,pdm);
    }

    DISPDBG((100, "<<< MulGetModes"));
    return ulRet;
}


//******************************************************************************
//
//  MulSetPalette
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulSetPalette(DHPDEV  dhpdev,PALOBJ* ppalo,FLONG   fl, ULONG   iStart, ULONG   cColors)
{
    PPDEV ppdev = (PPDEV)dhpdev;
    BOOL  bRet  = FALSE;

    DISPDBG((100, "MulSetPalette >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvSetPalette(dhpdev,ppalo, fl, iStart, cColors);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            bRet = DrvSetPalette( pmdev->abdDevices[ulDev].dhpdev, ppalo, fl, iStart, cColors);                 
        }
    }

    DISPDBG((100, "<<< MulSetPalette"));
    return bRet;
}


//******************************************************************************
//
//  MulDitherColor
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
ULONG MulDitherColor(DHPDEV dhpdev,ULONG  iMode,ULONG  rgb,ULONG* pul)
{
    ULONG ulRet = 0;

    DISPDBG((100, "MulDitherColor >>>"));

    // nothing to do here, because we only need to call 1 driver to do 
    // calculate us a color => don't hang in MulDitherColor 
    ulRet = DrvDitherColor(dhpdev,iMode, rgb, pul);

    DISPDBG((100, "<<< MulDitherColor"));
    return ulRet;
}


//******************************************************************************
//
//  MulSetPointerShape
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
ULONG MulSetPointerShape(SURFOBJ*  pso, SURFOBJ*  psoMask, SURFOBJ*  psoColor,
                         XLATEOBJ* pxlo, LONG xHot, LONG yHot, LONG x, LONG y,
                         RECTL* prcl, FLONG fl)
{
    PPDEV ppdev = (PPDEV)pso->dhpdev; 
    ULONG ulRet = FALSE;

    DISPDBG((100, "MulSetPointerShape >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        ulRet = DrvSetPointerShape(pso, psoMask, psoColor, pxlo, xHot, yHot, x, y, prcl, fl);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;
        LONG    lx, ly;
        RECTL   rcl;
        RECTL   *prclPointerBounds;

        ASSERT(pmdev->dwUniq == PMDEV_UNIQ);

        prclPointerBounds = prcl;
        if (prcl)
        {
            prclPointerBounds = &rcl;
        }

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            lx = x - pmdev->abdDevices[ulDev].rclBoard.left;
            ly = y - pmdev->abdDevices[ulDev].rclBoard.top;

            if ( x < (pmdev->abdDevices[ulDev].rclBoard.left - 32)
             ||  x > (pmdev->abdDevices[ulDev].rclBoard.right + 32)
             ||  y < (pmdev->abdDevices[ulDev].rclBoard.top - 32)
             ||  y > (pmdev->abdDevices[ulDev].rclBoard.bottom + 32))
            {
                lx = -1;  // turns off this cursor
            }

            if (prcl)
            {
                vRclSubOffset(&rcl, prcl, pmdev->abdDevices[ulDev].rclBoard.left, pmdev->abdDevices[ulDev].rclBoard.top);
                prclPointerBounds = &rcl;
            }

            ASSERT(pmdev->abdDevices[ulDev].pso);
            ulRet = DrvSetPointerShape(pmdev->abdDevices[ulDev].pso, psoMask, psoColor, pxlo, xHot, yHot, lx, ly, prclPointerBounds, fl);
        }
    }

    DISPDBG((100, "<<< MulSetPointerShape"));
    return ulRet;
}


//******************************************************************************
//
//  MulMovePointer
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
VOID MulMovePointer(SURFOBJ *pso,LONG x,LONG y,RECTL *prcl)
{
    PPDEV ppdev = (PPDEV)pso->dhpdev; 
    ULONG ulRet = FALSE;

    DISPDBG((100, "MulMovePointer >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        DrvMovePointer(pso, x, y, prcl);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;
        LONG    lx, ly;
        RECTL   rcl;
        RECTL   *prclPointerBounds;

        ASSERT(pmdev->dwUniq == PMDEV_UNIQ);

        prclPointerBounds = prcl;
        if (prcl)
        {
            prclPointerBounds = &rcl;
        }

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            lx = x - pmdev->abdDevices[ulDev].rclBoard.left;
            ly = y - pmdev->abdDevices[ulDev].rclBoard.top;

            if ( x < (pmdev->abdDevices[ulDev].rclBoard.left - 32)
             ||  x > (pmdev->abdDevices[ulDev].rclBoard.right + 32)
             ||  y < (pmdev->abdDevices[ulDev].rclBoard.top - 32)
             ||  y > (pmdev->abdDevices[ulDev].rclBoard.bottom + 32))
            {
                lx = -1;  // turns off this cursor
            }

            if (prcl)
            {
                vRclSubOffset(&rcl, prcl, pmdev->abdDevices[ulDev].rclBoard.left, pmdev->abdDevices[ulDev].rclBoard.top);
                prclPointerBounds = &rcl;
            }

            ASSERT(pmdev->abdDevices[ulDev].pso);
            DrvMovePointer( pmdev->abdDevices[ulDev].pso, lx, ly, prclPointerBounds);
        }
    }

    DISPDBG((100, "<<< MulMovePointer"));
    return ;
}


//******************************************************************************
//
//  MulCreateDeviceBitmap
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
HBITMAP MulCreateDeviceBitmap(DHPDEV dhpdev, SIZEL sizl, ULONG iFormat)
{
    PPDEV   ppdev = (PPDEV)dhpdev;
    HBITMAP hRet  = (HBITMAP) 0;

    DISPDBG((100, "MulCreateDeviceBitmap >>>"));

    ASSERT(ppdev);

    // we do not support CreateDeviceBitMap in MultiBoard case !
    if( !ppdev->pmdev )
    {
        hRet = DrvCreateDeviceBitmap(dhpdev,sizl,iFormat);
    }

    DISPDBG((100, "<<< MulCreateDeviceBitmap"));

    return hRet;
}


//******************************************************************************
//
//  MulDeleteDeviceBitmap
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
VOID MulDeleteDeviceBitmap(DHSURF dhsurf)
{
    PPDEV   ppdev;
   
    ASSERT(dhsurf);

    DISPDBG((100, "MulDeleteDeviceBitmap >>>"));

    ppdev  = ((DSURF *)dhsurf)->ppdev;

    ASSERT(ppdev);

    
    if( !ppdev->pmdev )
    {
        DrvDeleteDeviceBitmap(dhsurf);
    }
    else
    {
        // we do not support DeleteDeviceBitmap in MultiBoard case !
        // => we should never be called for DrvDeleteDeviceBitmap
        ASSERT(FALSE);
    }


    DISPDBG((100, "<<< MulDeleteDeviceBitmap"));

    return;
}


#endif // NT4_MULTI_DEV




