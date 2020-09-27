//************************** Module Header *************************************
//                                                                             *
//  Module Name: nt4MulDirectDraw.c                                                   *
//                                                                             *
//  This module contains the functions that belong to the multi device wrapper *
//                                                                             *
//   Copyright (C) 2000 NVidia Corporation. All Rights Reserved.               *
//                                                                             *
//******************************************************************************

#include "precomp.h"
#include "driver.h"

#include "nt4multidev.h"

#ifdef NT4_MULTI_DEV

//******************************************************************************
//
//  MulEnableDirectDraw
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulEnableDirectDraw(DHPDEV dhpdev, DD_CALLBACKS*  pCallBacks,
							DD_SURFACECALLBACKS*    pSurfaceCallBacks, DD_PALETTECALLBACKS*    pPaletteCallBacks)
{
    PPDEV  ppdev = (PPDEV)dhpdev;
    BOOL   bRet  = FALSE;

    DISPDBG((100, "MulEnableDirectDraw >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvEnableDirectDraw(dhpdev, pCallBacks, pSurfaceCallBacks, pPaletteCallBacks);
    }
    else
    {
        DISPDBG((1,"MulEnableDirectDraw: multiboards directDraw not implemented !"));
    }

    DISPDBG((100, "<<< MulEnableDirectDraw"));
	return bRet;
}


//******************************************************************************
//
//  MulGetDirectDrawInfo
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulGetDirectDrawInfo(DHPDEV dhpdev, DD_HALINFO* pHalInfo, DWORD* lpdwNumHeaps,
						  VIDEOMEMORY* pvmList, DWORD* lpdwNumFourCC, DWORD* lpdwFourCC)
{
    PPDEV  ppdev = (PPDEV)dhpdev;
    BOOL   bRet  = FALSE;

    DISPDBG((100, "MulGetDirectDrawInfo >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvGetDirectDrawInfo(dhpdev, pHalInfo, lpdwNumHeaps, pvmList, lpdwNumFourCC, lpdwFourCC);
    }
    else
    {
        DISPDBG((1,"MulGetDirectDrawInfo: multiboards directDraw not implemented !"));
    }

    DISPDBG((100, "<<< MulGetDirectDrawInfo - %d", bRet));
	return bRet;
}


//******************************************************************************
//
//  MulGetDirectDrawInfo
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
VOID MulDisableDirectDraw(DHPDEV dhpdev)
{
    PPDEV  ppdev = (PPDEV)dhpdev;

    DISPDBG((100, "MulDisableDirectDraw >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        DrvDisableDirectDraw(dhpdev);
    }
    else
    {
        DISPDBG((1,"MulDisableDirectDraw: multiboards directDraw not implemented !"));
    }

    DISPDBG((100, "<<< MulDisableDirectDraw"));
	return ;
}

#endif // NT4_MULTI_DEV

