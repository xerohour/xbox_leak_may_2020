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

/*
==============================================================================

    NVIDIA Corporation, (C) Copyright
    1996,1997,1998,1999. All rights reserved.

    nvpe.cpp

    DDraw Video Port implementation

    Jan 27, 1999

==============================================================================
*/

#ifdef NVPE

#include "nvpkrnl.h"
#include "nvpe.h"

// VPE states
#define NVP_STATE_STOPPED   0x01
#define NVP_STATE_RUNNING   0x02

/*
==============================================================================

    CanCreateVideoPort32

    Description:    Determines whether a Videoport can be created. Sometimes
                    this function is called twice with the same VideoPort ID.
                    In such case, the video port can be created (VIDEO and
                    VBI)

==============================================================================
*/
DWORD NVPAPI CanCreateVideoPort32(LPDDHAL_CANCREATEVPORTDATA lpInput)
{
    GLOBALDATA *pDriverData;

    DPF("***CanCreateVideoPort32");

    //ASSERT(lpInput!=NULL);
    //ASSERT(lpInput->lpDDVideoPortDesc !=NULL);    
    
    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);
    
    if (pDriverData == NULL)
    {
        DPF("NVPE: error - VideoPort not creatable  -->  pDriverData = NULL");
        NvReleaseSemaphore(pDriverData);
        lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }
    
    // Check structure size
    if (lpInput->lpDDVideoPortDesc->dwSize < sizeof(DDVIDEOPORTDESC))
    {
        DPF("NVPE: error - VideoPort not creatable  -->  stucture length is not large enough");
        NvReleaseSemaphore(pDriverData);
        lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }
    
    // check input connection GUID
    if (!IsEqualGUID_C((lpInput->lpDDVideoPortDesc->VideoPortType.guidTypeID), DDVPTYPE_CCIR656))
    {
        DPF("NVPE: error - VideoPort not creatable  --> GUID should be DDVPTYPE_CCIR656");
        NvReleaseSemaphore(pDriverData);
        lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }

    // check to see if there are videoports available... by looking at local allocation
    if (pDriverData->dwVideoPortsAvailable <= 0)
    {
        // check to see if there are ports available... by looking at IDs
        if (lpInput->lpDDVideoPortDesc->dwVideoPortID != 0)
        {
            DPF("NVPE: error - VideoPort not creatable  --> not enough video ports");
            NvReleaseSemaphore(pDriverData);
            lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
            return DDHAL_DRIVER_HANDLED;
        }

        DPF("NVPE: second VP with the same ID allowed !");
    }


	
	// the restrictions we have FieldWidth are dependant on maximum pitch specifiable
	//  pitch is 2^13 in size...
	if (lpInput->lpDDVideoPortDesc->dwFieldWidth > NV_MAX_FIELDWIDTH) {
		DPF("   ***VideoPort not creatable  --> field Width is too great");
		NvReleaseSemaphore(pDriverData);
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// the restrictions we have VBIWidth are dependant on maximum pitch specifiable
	//  pitch is 2^13 in size...  but there are 2 bytes in each pixel
	if (lpInput->lpDDVideoPortDesc->dwVBIWidth > NV_MAX_VBIWIDTH) {
		DPF("   ***VideoPort not creatable  --> VBI field Width is too great");
		NvReleaseSemaphore(pDriverData);
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// the restrictions we have on Field Height are dependant on maximum length specifiable
	//  length is 2^19 in size... which is but there are 2 bytes in each pixel
	if (lpInput->lpDDVideoPortDesc->dwFieldHeight > NV_MAX_FIELDHEIGHT) {
		DPF("   ***VideoPort not creatable  --> Maximum buffer length exceed by field height too high");
		NvReleaseSemaphore(pDriverData);
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}

	if (lpInput->lpDDVideoPortDesc->dwMaxPixelsPerSecond > NV_MAX_PIXELRATE) {
		DPF("   ***VideoPort not creatable  --> Sorry incoming data rate is too high");
		NvReleaseSemaphore(pDriverData);
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}

    
	if ((lpInput->lpDDVideoPortDesc->VideoPortType.dwPortWidth  != 8) && (lpInput->lpDDVideoPortDesc->VideoPortType.dwPortWidth  != 16)) 
	{
		DPF("NVPE: error - VideoPort not creatable  --> port width is not 8 or 16");
		NvReleaseSemaphore(pDriverData);
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}


    NvReleaseSemaphore(pDriverData);

    lpInput->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    CreateVideoPort32

    Description:    Create a Videoport. Initialize context.

    Notes:          This function allows the HAL to set internal flags and
                    become aware that a video port has been created. It
                    should always return DDHAL_DRIVER_HANDLED.
                    According to DDHAL documentation, this procedure can
                    return DDHAL_DRIVER_NOTHANDLED as well, signifying that
                    the createvideoport fn could not be completed.
                    DirectDraw MS layer will prevent creating more videoports
                    that is available.

==============================================================================
*/
DWORD NVPAPI CreateVideoPort32(LPDDHAL_CREATEVPORTDATA lpInput)
{
    GLOBALDATA *pDriverData;

    DPF("***CreateVideoPort32");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    if (NVPInitialize(	pDriverData, 
						lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags,
						lpInput->lpDD->lpGbl))
    {
        DPF("NVPE: error - VideoPort driver failed to initialize !");
        NvReleaseSemaphore(pDriverData);
        lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        return DDHAL_DRIVER_HANDLED;
    }

    // one down !
    (pDriverData->dwVideoPortsAvailable)--;

    pDriverData->nvpeState.dwVPEState = NVP_STATE_STOPPED;

    NvReleaseSemaphore(pDriverData);

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
==============================================================================

    GetVideoPortBandwidth32

    Description:    Determine how much bandwidth is available for the
                    specified output format and fill in the information

==============================================================================
*/
DWORD NVPAPI GetVideoPortBandwidth32(LPDDHAL_GETVPORTBANDWIDTHDATA lpInput)
{
    DPF("***GetVideoPortBandwidth32");

    if (lpInput->dwFlags & DDVPB_TYPE)
    {
        lpInput->lpBandwidth->dwCaps = DDVPBCAPS_DESTINATION;  // we are limited by the "shrink" of the overlay
    }
    
    if (lpInput->dwFlags & DDVPB_VIDEOPORT)
    {
        // The following four should indicate the MINIMUM stretch values represented as percentages times 10
        //  so if we could not stretch below 75% of the orginal size, the we should put in 750
        lpInput->lpBandwidth->dwOverlay                 = 1000;
        lpInput->lpBandwidth->dwColorkey                = 1000;  // Overlay/video port/colorkey works at 1X
        lpInput->lpBandwidth->dwYInterpolate            = 1000;  // Overlay/video port works at 1X
        lpInput->lpBandwidth->dwYInterpAndColorkey      = 1000;  // Overlay/video port/interpolation works at 2X
    }
    
    if (lpInput->dwFlags & DDVPB_OVERLAY)
    {
        DPF("NVPE: We are NOT an overlay limited device but a destination ");
        DPF("      limited device we should not have been called");
    }

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    GetVideoPortInputFormat32

    Description:    retrieves the VP input format (UYVY 422)

==============================================================================
*/
DWORD NVPAPI GetVideoPortInputFormat32(LPDDHAL_GETVPORTINPUTFORMATDATA lpInput)
{
    DPF("***GetVideoPortInputFormat32");

    // Only 1 input format supported UYVY 4:2:2
    // Data stored in 32bit word as U[7:0] Y1[15:8] V[23:16] Y2[31:24]
    if (lpInput->dwFlags & DDVPFORMAT_VIDEO)
    {
        lpInput->dwNumFormats = 1;
        if (lpInput->lpddpfFormat != NULL)
        {
            memcpy (lpInput->lpddpfFormat, &g_aNVPVideoFormat[0], sizeof(DDPIXELFORMAT));
        }
    }

    // VBI raw 8 bit
    if (lpInput->dwFlags & DDVPFORMAT_VBI)
    {
        lpInput->dwNumFormats = 1;
        if (lpInput->lpddpfFormat != NULL)
        {
            memcpy (lpInput->lpddpfFormat, &g_aNVPVBIFormat[0], sizeof(DDPIXELFORMAT));
        }
    }

    lpInput->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;
}


/*
==============================================================================

    GetVideoPortOutputFormat32

    Description:    retrieves the VP output format (UYVY 422)

==============================================================================
*/
DWORD NVPAPI GetVideoPortOutputFormat32(LPDDHAL_GETVPORTOUTPUTFORMATDATA lpInput)
{
    DPF("***GetVideoPortOutputFormat32");

    lpInput->dwNumFormats = 1;
    if (lpInput->lpddpfOutputFormats != NULL)
    {
        memcpy (lpInput->lpddpfOutputFormats, lpInput->lpddpfInputFormat, sizeof (DDPIXELFORMAT));
    }

    // UYVY 4:2:2
    if (lpInput->dwFlags & DDVPFORMAT_VIDEO)
    {
        // BES converts (overlay) and not the video port !
        if (lpInput->lpddpfInputFormat->dwFourCC == mmioFOURCC('U','Y','V','Y'))
        {
            lpInput->dwNumFormats = 1;
            if (lpInput->lpddpfOutputFormats != NULL)
            {
                memcpy (lpInput->lpddpfOutputFormats, &g_aNVPVideoFormat[0], sizeof(DDPIXELFORMAT));
            }
        }
    }

    // VBI raw 8 bit
    if (lpInput->dwFlags & DDVPFORMAT_VBI)
    {
        if (lpInput->lpddpfInputFormat->dwFourCC == mmioFOURCC('R','A','W','8'))
        {
            lpInput->dwNumFormats = 1;
            if (lpInput->lpddpfOutputFormats != NULL)
            {
                memcpy (lpInput->lpddpfOutputFormats, &g_aNVPVBIFormat[0], sizeof(DDPIXELFORMAT));
            }
        }
    }

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    GetVideoPortConnectInfo

    Description:    Returns the GUIDs describing the connections that can be
                    supported, port widths and CAPS

==============================================================================
*/
DWORD NVPAPI GetVideoPortConnectInfo(LPDDHAL_GETVPORTCONNECTDATA lpInput)
{
    DPF("***GetVideoPortConnectInfo");

#if (NVARCH < 0x10)
    lpInput->dwNumEntries = 1;
    if(lpInput->lpConnect == NULL)
    {
        // this means that Dx is looking for the number of ways you can connect so that it can
        // allocate a structure for the next time around
        lpInput->ddRVal = DD_OK;
    }
    else
    {
        // fill in the one structure that we asked for..    
        lpInput->lpConnect->dwSize = sizeof(DDVIDEOPORTCONNECT);
        memcpy(&(lpInput->lpConnect->guidTypeID), &DDVPTYPE_CCIR656, sizeof (GUID));
        lpInput->lpConnect->dwPortWidth = 8;
        lpInput->lpConnect->dwFlags = g_dwNVPConnectionCaps;
    }
#else
    lpInput->dwNumEntries = 2;
    if(lpInput->lpConnect == NULL)
    {
        // this means that Dx is looking for the number of ways you can connect so that it can
        // allocate a structure for the next time around
        lpInput->ddRVal = DD_OK;
    }
    else
    {
        // fill in the one structure that we asked for..    
        lpInput->lpConnect[0].dwSize = sizeof(DDVIDEOPORTCONNECT);
        memcpy(&(lpInput->lpConnect[0].guidTypeID), &DDVPTYPE_CCIR656, sizeof (GUID));
        lpInput->lpConnect[0].dwPortWidth = 8;
        lpInput->lpConnect[0].dwFlags = g_dwNVPConnectionCaps;

        lpInput->lpConnect[1].dwSize = sizeof(DDVIDEOPORTCONNECT);
        memcpy(&(lpInput->lpConnect[1].guidTypeID), &DDVPTYPE_CCIR656, sizeof (GUID));
        lpInput->lpConnect[1].dwPortWidth = 16; // NV10 and up we support 16 bit port width
        lpInput->lpConnect[1].dwFlags = g_dwNVPConnectionCaps;

    }
#endif

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
==============================================================================

    DestroyVideoPort32

    Description:    This optional function notifies the HAL when the video
                    port has been destroyed

==============================================================================
*/
DWORD NVPAPI DestroyVideoPort32(LPDDHAL_DESTROYVPORTDATA lpInput)
{
    GLOBALDATA *pDriverData;

    DPF("***DestroyVideoPort32");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

	//Dst width & height =0 will shutdown overlay
	//changing size values are not recommended but since these values
	//are updated when you call updateoverlay, I guess this is fine.
	//and we are destroying VP anyway...
    
    // Should create a better method for this @mjl@

	pDriverData->vpp.dwOverlayDstWidth = 0;
	pDriverData->vpp.dwOverlayDstHeight = 0;
	NVPUpdateOverlay(pDriverData, lpInput->lpDD->lpGbl);
    NVPStopVideo(pDriverData, lpInput->lpDD->lpGbl);
    NVPUninitialize(pDriverData);

    // available again !
    (pDriverData->dwVideoPortsAvailable)++;

    NvReleaseSemaphore(pDriverData);

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    GetVideoPortField32

    Description:    readback of the current field

==============================================================================
*/
DWORD NVPAPI GetVideoPortField32(LPDDHAL_GETVPORTFIELDDATA lpInput)
{
    DPF("***GetVideoPortField32");

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    GetVideoPortLine32

    Description:    readback of the current video line number (0 relative)

==============================================================================
*/
DWORD NVPAPI GetVideoPortLine32(LPDDHAL_GETVPORTLINEDATA lpInput)
{
    DPF("***GetVideoPortLine32");

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    GetVideoSignalStatus32

    Description:    any incoming video signal ?

==============================================================================
*/
DWORD NVPAPI GetVideoSignalStatus32(LPDDHAL_GETVPORTSIGNALDATA lpInput)
{
    GLOBALDATA *pDriverData;

    DPF("***GetVideoSignalStatus32");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    if (pDriverData->nvpeState.dwVPEState == NVP_STATE_STOPPED)
    {
        DPF("GetVideoSignalStatus32: could not get the signal status, the video port is not running... ");
        NvReleaseSemaphore(pDriverData);
        // we'll return no signal in this case....
        lpInput->dwStatus = DDVPSQ_NOSIGNAL;
        lpInput->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

    // check vp sync event to see if things are running down there !
    if (NVPWaitForSync(pDriverData, NVP_EVENT_MEDIAPORT, 35) == NVP_STATUS_SUCCESS)    // 35 msec for a complete frame to pass through !
        lpInput->dwStatus = DDVPSQ_SIGNALOK;
    else
        lpInput->dwStatus = DDVPSQ_NOSIGNAL;

    NvReleaseSemaphore(pDriverData);

    lpInput->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    GetVideoPortFlipStatus32

    Description:    This required function allows DDRAW to restrict access to
                    a surface until the physical flip has occurred, allowing
                    doubled buffered capture.
                    If the physical flip has not yet occurred, return
                    DDERR_WASSTILLDRAWING

==============================================================================
*/
DWORD NVPAPI GetVideoPortFlipStatus32(LPDDHAL_GETVPORTFLIPSTATUSDATA lpInput)
{
    GLOBALDATA *pDriverData;

    DPF("***GetVideoPortFlipStatus32");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    // check vp sync event and return immediatly
    if (NVPWaitForSync(pDriverData, NVP_EVENT_MEDIAPORT, 0) != NVP_STATUS_SUCCESS)
    {
        // we'll return still drawing in this case....
        NvReleaseSemaphore(pDriverData);
        lpInput->ddRVal = DDERR_WASSTILLDRAWING;
        return DDHAL_DRIVER_HANDLED;
    }

    NvReleaseSemaphore(pDriverData);
    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    WaitForVideoPortSync32

    Description:    Waits for the LPB Video VSYNC (note: this is entirely
                    different from the screen refresh VGA VSYNC)

==============================================================================
*/
DWORD NVPAPI WaitForVideoPortSync32(LPDDHAL_WAITFORVPORTSYNCDATA lpInput)
{
    GLOBALDATA *pDriverData;
    DWORD dwTimeOut;

    DPF("***WaitForVideoPortSync32");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    if (pDriverData->nvpeState.dwVPEState == NVP_STATE_STOPPED)
    {
        DPF("WaitForVideoPortSync32: could not wait for VP Sync, the video port is not running... ");
        NvReleaseSemaphore(pDriverData);
        // we'll return an error in this case....
        lpInput->ddRVal = DDERR_VIDEONOTACTIVE;
        return DDHAL_DRIVER_HANDLED;
    }

    dwTimeOut = (lpInput->dwTimeOut != 0) ? lpInput->dwTimeOut : 2000; // wait for 2 sec (2000 msec)

    if (NVPWaitForSync(pDriverData, NVP_EVENT_MEDIAPORT, dwTimeOut) != NVP_STATUS_SUCCESS)
    {
        DPF("WaitForVideoPortSync32: VP Sync timed-out !");
        NvReleaseSemaphore(pDriverData);
        // we'll return an error in this case....
        lpInput->ddRVal = DDERR_VIDEONOTACTIVE;
        return DDHAL_DRIVER_HANDLED;
    }

    NvReleaseSemaphore(pDriverData);

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}

/*
==============================================================================

    FlipVideoPort32

    Description:    Program registers to do the actual flip

==============================================================================
*/
DWORD NVPAPI FlipVideoPort32(LPDDHAL_FLIPVPORTDATA lpInput)
{
    DPF("***FlipVideoPort32");

    NVPFlipVideoPort(lpInput);

    lpInput->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
==============================================================================

    UpdateVideoPort32

    Description:    Sets up the video port. Might be called separately for
                    VBI and Video.

==============================================================================
*/
DWORD NVPAPI UpdateVideoPort32(LPDDHAL_UPDATEVPORTDATA lpInput)
{
    GLOBALDATA *pDriverData;

    DPF("***UpdateVideoPort32");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    lpInput->ddRVal = DD_OK;

    //if (pDriverData->NvDevFlatDma == 0)
	if(pDriverData->nvpeState.bVPEInitialized == FALSE){
        NvReleaseSemaphore(pDriverData);
        lpInput->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
    }

    if (lpInput->dwFlags == DDRAWI_VPORTUPDATE)
    {
        DPF("UpdateVideoPort32: update videoport");
        NVPUpdateVideo(lpInput);
    }
    else if (lpInput->dwFlags == DDRAWI_VPORTSTART)
    {
        // if already running, consider this as an update !!!
        if (pDriverData->nvpeState.dwVPEState == NVP_STATE_RUNNING)
        {
            DPF("UpdateVideoPort32: update videoport");
            NVPUpdateVideo(lpInput);
        }
        else
        {
            DPF("UpdateVideoPort32: start videoport");
            NVPStartVideo(lpInput);
            pDriverData->nvpeState.dwVPEState = NVP_STATE_RUNNING;
        }
    }
    else
    {
        DPF("UpdateVideoPort32: stop videoport");
        // stop grabbing video !
        NVPStopVideo(pDriverData, lpInput->lpDD->lpGbl);
        pDriverData->nvpeState.dwVPEState = NVP_STATE_STOPPED;
    }

    NvReleaseSemaphore(pDriverData);

    return DDHAL_DRIVER_HANDLED;
}


#endif // NVPE