/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: vpe_nt.c                                                          *
* Copyright Microsoft, 1996. All rights reserved.
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           6/29/98                                          *
*                                                                           *
\***************************************************************************/


/* #includes ---------------------------------------------*/



#include "precomp.h"
#include "nvos.h"
#include "nvapi.h"
#include "nvsubch.h"
#include "vpe_nt.h"
#include "surfaces.h"

//#if DX5

DWORD WINAPI GetDriverInfo32(LPDDHAL_GETDRIVERINFODATA lpInput);
DWORD WINAPI SyncSurfaceData32(LPDDHAL_SYNCSURFACEDATA lpInput);
DWORD WINAPI SyncVideoPortData32(LPDDHAL_SYNCVIDEOPORTDATA lpInput);
DWORD WINAPI ColorControl32(LPDDHAL_COLORCONTROLDATA lpInput);



/*
** Callback tables exported through the GetDriverInfo32 interface
*/
const DDHAL_DDKERNELCALLBACKS KernelCallbacks =
{
    sizeof(DDHAL_DDKERNELCALLBACKS),
    0,
    NULL,
    NULL
};

const GUID *guids[] =
{
    &DDVPTYPE_CCIR656,
};

#define MAX_VP_TYPES    (sizeof(guids)/sizeof(void*))
#define CCIR656     0

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif // mmioFOURCC

const DDPIXELFORMAT InputFormats[] =
{
    {sizeof(DDPIXELFORMAT),DDPF_FOURCC, mmioFOURCC('U','Y','V','Y'),16,(DWORD)-1,(DWORD)-1,(DWORD)-1}
};




#define MAX_INPUT_FORMATS (sizeof(InputFormats)/sizeof(DDPIXELFORMAT))
#define INPUTFORMAT_UYVY    0

const DDVIDEOPORTCAPS ddLPBVideoPortCaps =
{
    sizeof(DDVIDEOPORTCAPS),    // size of the DDVIDEOPORTCAPS structure
    DDVPD_WIDTH             |
    DDVPD_HEIGHT            |
    DDVPD_ID                |
    DDVPD_CAPS              |
    DDVPD_FX                |
    DDVPD_AUTOFLIP          |
    DDVPD_ALIGN             |
    0,
    1024,                       // VP max width (limited by FF24)
    1024,                       // max VBI data width (limited by FF24)
    512,                        // VP max height (limited by FF24)
    0,                          // VP ID (0 - (dwMaxVideoPorts-1))
    DDVPCAPS_AUTOFLIP       |   // LPB supports 2 autoflip buffers
    DDVPCAPS_INTERLACED     |
    DDVPCAPS_NONINTERLACED  |
    DDVPCAPS_READBACKFIELD  |
//  DDVPCAPS_READBACKLINE   |   // LPB doesn't support this
//  DDVPCAPS_SHAREABLE      |   // LPB doesn't support this
    DDVPCAPS_SKIPEVENFIELDS |   // when does FF00_5 start dropping ?BJT
    DDVPCAPS_SKIPODDFIELDS  |   // when does FF00_5 start dropping ?BJT
//  DDVPCAPS_SYNCMASTER     |   // LPB doesn't support this
    DDVPCAPS_VBISURFACE     |   // GX2 places VBI data in same surface
//  DDVPCAPS_COLORCONTROL   |   // LPB doesn't support this. SP does.
    0,
//  DDVPFX_CROPTOPDATA      |   // We don't know VBI size without being told
//  DDVPFX_CROPX            |
//  DDVPFX_CROPY            |
    DDVPFX_INTERLEAVE       |
//  DDVPFX_MIRRORLEFTRIGHT  |   // LPB doesn't support this
//  DDVPFX_MIRRORUPDOWN     |   // LPB doesn't support this
//  DDVPFX_PRESHRINKX       |   // LPB can't arbitrarily shrink
//  DDVPFX_PRESHRINKY       |   // LPB can't arbitrarily shrink
//  DDVPFX_PRESHRINKXS      |   // LPB shrinks by byte/pixel dropping
//  DDVPFX_PRESHRINKYS      |   // LPB shrinks by byte/pixel dropping
    DDVPFX_PRESHRINKXB      |   // down to 1/2, 1/4, 1/8 or 1/16
    DDVPFX_PRESHRINKYB      |   // as above
//  DDVPFX_PRESTRETCHX      |   // LPB never stretches
//  DDVPFX_PRESTRETCHY      |   // LPB never stretches
//  DDVPFX_PRESTRETCHXN     |   // LPB never stretches
//  DDVPFX_PRESTRETCHYN     |   // LPB never stretches
//  DDVPFX_VBICONVERT       |   // LPB doesn't have special VBI conversion
//  DDVPFX_VBINOSCALE       |   // TBD: GX2 has some VBI support
    0,
    5,                          // Number of autoflippable surfaces
    8,                          // Byte restriction of placement within surface
    1,                          // Byte restriction of width after prescaling
    1,                          // Byte restriction of left cropping
    1,                          // Byte restriction of cropping width
    16,                         // shrink in steps of original width
    32,                         // shrink in steps of original height
};

const DDHAL_DDVIDEOPORTCALLBACKS LPBVideoPortCallbacks =
{
    sizeof(DDHAL_DDVIDEOPORTCALLBACKS),
    DDHAL_VPORT32_CANCREATEVIDEOPORT |
    DDHAL_VPORT32_CREATEVIDEOPORT |
    DDHAL_VPORT32_FLIP |
    DDHAL_VPORT32_GETBANDWIDTH |
    DDHAL_VPORT32_GETINPUTFORMATS |
    DDHAL_VPORT32_GETOUTPUTFORMATS |
    DDHAL_VPORT32_GETFIELD |
    DDHAL_VPORT32_GETLINE |
    DDHAL_VPORT32_GETCONNECT |
    DDHAL_VPORT32_DESTROY |
    DDHAL_VPORT32_GETFLIPSTATUS |
    DDHAL_VPORT32_UPDATE |
    DDHAL_VPORT32_WAITFORSYNC |
    DDHAL_VPORT32_GETSIGNALSTATUS |
    //DDHAL_VPORT32_COLORCONTROL |
    0,
    &CanCreateVideoPort32,
    &CreateVideoPort32,
    &FlipVideoPort32,
    &GetVideoPortBandwidth32,
    &GetVideoPortInputFormat32,
    &GetVideoPortOutputFormat32,
    (LPVOID)0,
    &GetVideoPortField32,
    &GetVideoPortLine32,
    &GetVideoPortConnectInfo,
    &DestroyVideoPort32,
    &GetVideoPortFlipStatus32,
    &UpdateVideoPort32,
    &WaitForVideoPortSync32,
    &GetVideoSignalStatus32,
    (LPVOID)0
};

const DWORD dwConnectionCaps=	//DDVPCONNECT_DOUBLECLOCK			|
											//DDVPCONNECT_VACT					|
											//DDVPCONNECT_INVERTPOLARITY		|
											/* we discard at least until the V bit goes low, and potentially more depending
											  on the setting of YCROP  as well as the current VBI situation  */
											DDVPCONNECT_DISCARDSVREFDATA	|
											//DDVPCONNECT_HALFLINE				|
											/*  these next are "client" only 
											//DDVPCONNECT_INTERLACED			|
											//DDVPCONNECT_SHAREEVEN			|
											//DDVPCONNECT_SHAREODD				|
											*/
											0;

#if 0

const DWORD dwConnectionCaps=
                          //      DDVPCONNECT_INVERTPOLARITY      |
                                DDVPCONNECT_DISCARDSVREFDATA    |
                                DDVPCONNECT_HALFLINE            |
                                DDVPCONNECT_INTERLACED          |
                                DDVPCONNECT_SHAREEVEN           |
                                DDVPCONNECT_VACT                |
                                DDVPCONNECT_DOUBLECLOCK         |
                                DDVPCONNECT_SHAREODD            |
                                0
                                ;

#endif
#if 0

#ifdef WINNT
    /* fpVidMem is already an offset on NT */
    #define SCREENOFFSET(sData, offset) (offset)
    #define UPDATEOVERLAYFLIPOFFSET(sData) (sData->dwOverlayFlipOffset)

    #define LPS3THKINFO PPDEV
    /* our private instance data */
    #define VP_GETSDATA(x) ((LPS3THKINFO)(x->lpDD->lpGbl->dhpdev))
    #define pLPB (&(sData->LPBData))

//    #define SWORD SHORT

    #define TRACE
    #define INT3
    #define assert(x)
#else
    /* fpVidMem is an actual address on Win95 */
    #define SCREENOFFSET(sData, offset) ((offset) - sData->dwScreenFlatAddr)
    #define UPDATEOVERLAYFLIPOFFSET(sData) (dwUpdateOverlayFlipOffset)

    /* our private instance data */
    #define VP_GETSDATA(x) VERIFYSDATA((LPS3THKINFO)(x->lpDD->lpGbl->dwReserved3))
    #define pLPB (&(sData->LPBData))

    /* debugging support */
    #ifdef DEBUG
    #define TRACE   Msg
    #define DPF		Msg
    #define INT3 _asm int 3
    extern void __cdecl Msg( LPSTR szFormat, ... );
    #define assert(test) ((test) ? (void) 0 : DPF("* FAILURE * assertion %s: %s line %d",#test,__FILE__,__LINE__))
    #else
    #define INT3    (void)0
    #define DPF		1 ? (void)0 : (void)
    #define TRACE   1 ? (void)0 : (void)
    #define assert  1 ? (void)0 : (void)
    #endif
#endif

#endif


extern const DDHAL_DDVIDEOPORTCALLBACKS LPBVideoPortCallbacks;
extern vpSurfaces MySurfaces;
vpSurfaces*			pMySurfaces = &MySurfaces;
NvNotification*	nvMyVPNotifiers;
OSSpecificGlobals   OSSpecGlobal;


DWORD VPE_DriverInit(PDEV*  ppdev)
{
    ppdev->hVPEChannel = NULL;
	 ppdev->pMySurfaces = (void*)pMySurfaces;
	 MySurfaces.pDrvDat = (void*) ppdev;
	 MySurfaces.dwVideoPortsAvailable = 1;
    return 0;
}


#ifdef VPE_NT
void	getCurrentNanoTimer(U032* pBigTime, U032* pSmallTime)
{
    // XXX do this for real
    *pBigTime = 0;
	*pSmallTime = 0;
}
#endif

U032	getMilliTime(void) {
	U032	bigTime = 0;
	U032	smallTime = 0;
	
	getCurrentNanoTimer(&bigTime, &smallTime);
	return (smallTime / 1000000) + bigTime * (0xFFFFFFFF/1000000);
			
}
	
	

/*
 * CanCreateVideoPort32
 *
 * This function is required
 */

DWORD __stdcall CanCreateVideoPort32 (LPDDHAL_CANCREATEVPORTDATA lpInput)
{
    /*
     * If the lpInput->lpDDVideoPortDesc can not be supported, set
     * lpInput->ddRVal to the correct error code and return
     * DDHAL_DRIVER_HANDLED
     */		
	/* 	this is only a query to see if VideoPort can handle a certain requirement, 
		no structures are defined here, or even chip query */
	DPF("***CanCreateVideoPort32");

	ASSERT(lpInput!=NULL);
	ASSERT(lpInput->lpDDVideoPortDesc !=NULL);	
	
	// Ok, if we got to here that means we're talking to the VXD MiniVDD properly... onward ho!
	
	// Check structure size
	if(lpInput->lpDDVideoPortDesc->dwSize   < sizeof( DDVIDEOPORTDESC ) ) {
		DPF("   ***VideoPort not creatable  -->  stucture length is not large enough");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// check input connection GUID
	if ( !IsEqualGUID(&(lpInput->lpDDVideoPortDesc->VideoPortType.guidTypeID), &DDVPTYPE_CCIR656) )	 {
		DPF("   ***VideoPort not creatable  -->  GUID should be DDVPTYPE_CCIR656");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// check to see if there are ports available... by looking at IDs
   if (lpInput->lpDDVideoPortDesc->dwVideoPortID >= NV_MAX_VIDEOPORTS) {
		DPF("   ***VideoPort not creatable  --> not enough video ports");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}

	// check to see if there are ports available... by looking at local allocation
	if (MySurfaces.dwVideoPortsAvailable <=0 ) {
		DPF("   ***VideoPort not creatable  --> not enough video ports according to our globals");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// the restrictions we have FieldWidth are dependant on maximum pitch specifiable
	//  pitch is 2^13 in size...
	if (lpInput->lpDDVideoPortDesc->dwFieldWidth > NV_MAX_FIELDWIDTH) {
		DPF("   ***VideoPort not creatable  --> field Width is too great");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// the restrictions we have VBIWidth are dependant on maximum pitch specifiable
	//  pitch is 2^13 in size...  but there are 2 bytes in each pixel
	if (lpInput->lpDDVideoPortDesc->dwVBIWidth > NV_MAX_VBIWIDTH) {
		DPF("   ***VideoPort not creatable  --> VBI field Width is too great");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// the restrictions we have on Field Height are dependant on maximum length specifiable
	//  length is 2^19 in size... which is but there are 2 bytes in each pixel
	if (lpInput->lpDDVideoPortDesc->dwFieldHeight * lpInput->lpDDVideoPortDesc->dwFieldWidth > NV_MAX_BUFFERLENGTH) {
		DPF("   ***VideoPort not creatable  --> Maximum buffer length exceed by field height too high");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}

	if (lpInput->lpDDVideoPortDesc->dwMaxPixelsPerSecond > NV_MAX_PIXELRATE) {
		DPF("   ***VideoPort not creatable  --> Sorry incoming data rate is too high");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}

	// we don't have any restrictions on Microseconds per field that I know of....
	//    lpInput->lpDDVideoPortDesc->dwMicrosecondsPerField;	// Microseconds per video field
    
	if ( lpInput->lpDDVideoPortDesc->VideoPortType.dwPortWidth  != 8 ) {
		DPF("   ***VideoPort not creatable  --> incoming connection width is not 8");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
	
#ifdef NOCHECKHERE
			// the ones I'm not capable of   ANDed   with the ones Dx is asking for
	if( (~MY_CONNECTION_CAPS) & lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags ) {
		// if we get here this should indicate that Dx is asking for a capability we're incapable of
		DPF("   ***VideoPort not creatable  --> Some of the capability flags bits are wrong");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	
	}
#endif
		
	//  Ok, we've pretty much checked everything we can possibly check here... let's tell'em that
	// we can work!		

	lpInput->ddRVal = DD_OK;
	DPF("   ***VideoPort creatable! ***");
	return DDHAL_DRIVER_HANDLED;
}


/*
 * CreateVideoPort32
 *
 * This function is optional
 */
DWORD __stdcall CreateVideoPort32 (LPDDHAL_CREATEVPORTDATA lpInput)
{
    /*
     * This function allows the HAL to set internal flags and
     * become aware that a video port has been created. It should
     * always return DDHAL_DRIVER_NOTHANDLED.
     */

	/* According to DDHAL documentation, this procedure can return DDHAL_DRIVER_NOTHANDLED
	   as well, signifying that the createvideoport fn could not be completed
	   
	   DirectDraw MS layer will prevent creating more videoports that is
	   available.

	   */
		     
	DWORD		returnVal;
	// we should only have to get this once on creation
	{
		long i;
		
		nvMyVPNotifiers = pMySurfaces->pTheNotifiers;
		pMySurfaces->numberOfSurfaces =0;
		
		
		for(i=0;i<MAXVPSURFACES;i++) {
			pMySurfaces->Surfaces[i] = NULL;
		}
		pMySurfaces->CurrentBufferSurface[4];				// to associate the HW buffer with an arrary index
		
		if(lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags & DDVPCONNECT_INTERLACED ) {
			pMySurfaces->bInputInterLaced = TRUE;		
		} else {
			pMySurfaces->bInputInterLaced = FALSE;		
		}
		
		 
		if(pMySurfaces->bInputInterLaced)
			pMySurfaces->CurrentCaptureMode =EVENODD;		// indicates which fields will be capture Even	
		else 
			pMySurfaces->CurrentCaptureMode =PROGRESSIVE;	// we're assuming progressive is ALWAYS even even even
			
		pMySurfaces->CurrentScanMode =SEPERATEBUF;		// seperate or interleaved buffersx
		pMySurfaces->lastField= ODD;							// Always start on odd field   //CODE problem if even only capture.. or 
		pMySurfaces->VBIstart = 0;;
		// general information
		pMySurfaces->bSkipNextField = FALSE;
		pMySurfaces->dwOutHeight = 0;
		pMySurfaces->dwOutWidth = 0;
		pMySurfaces->dwImageStartLine =10;				// ditch a minimum of 10 lines
		pMySurfaces->dwVBIStartLine = 0;
		pMySurfaces->dwVBIHeight = 0;
		pMySurfaces->dwPreHeight = 0;					// height and width before prescaling
		pMySurfaces->dwPreWidth = 0;
		pMySurfaces->dwOriginX = 0;						// orgins to move data within the surface
		pMySurfaces->dwOriginY = 0;
		pMySurfaces->bThreadDead = TRUE;
		pMySurfaces->bThreadRunning = FALSE;
		pMySurfaces->hVPInterruptEvent = NULL;
		pMySurfaces->VpInterruptId = 0;	// the thread ID
		pMySurfaces->hThreadHandle = NULL;
		pMySurfaces->bStopVP = FALSE;							// indicates to the thread that it should NOT reprogram
		pMySurfaces->loopCount = 0;
		pMySurfaces->pBuffer0Mem = (U032) NULL;
		pMySurfaces->pBuffer1Mem = (U032) NULL;
		pMySurfaces->curOverlay = 0;			// this way we should start into 0
		
		// initialize all the callback stuff	
		pMySurfaces->ImageCallbackRoutine = NULL;			
		pMySurfaces->VBICallbackRoutine = NULL;			
		pMySurfaces->myIMGCallbackInfo.intCount = 0;			
		pMySurfaces->myVBICallbackInfo.intCount = 0;	
		pMySurfaces->myIMGCallbackInfo.dwFlags = 0;
		pMySurfaces->myVBICallbackInfo.dwFlags = 0;
		pMySurfaces->myIMGCallbackInfo.dwVersion = 0x100;
		pMySurfaces->myVBICallbackInfo.dwVersion = 0x100;		
		pMySurfaces->myIMGCallbackInfo.curBufferAddr = (U032)NULL;			
		pMySurfaces->myVBICallbackInfo.curBufferAddr = (U032)NULL;			
		pMySurfaces->myIMGCallbackInfo.lastBufferAddr = (U032)NULL;			
		pMySurfaces->myVBICallbackInfo.lastBufferAddr = (U032)NULL;			
		pMySurfaces->myIMGCallbackInfo.dwSize = sizeof(sCallBack);			
		pMySurfaces->myVBICallbackInfo.dwSize = sizeof(sCallBack);	
		
	}


	DPF("***CreateVideoPort32");


	DPF("  Size of the DDVIDEOPORTDESC structure     %d",lpInput->lpDDVideoPortDesc->dwSize);
	DPF("  Width of the video port field.            %d",lpInput->lpDDVideoPortDesc->dwFieldWidth);
	DPF("  Width of the VBI data.                    %d",lpInput->lpDDVideoPortDesc->dwVBIWidth);
	DPF("  Height of the video port field.           %d",lpInput->lpDDVideoPortDesc->dwFieldHeight);
	DPF("  Microseconds per video field.             %d",lpInput->lpDDVideoPortDesc->dwMicrosecondsPerField);
	DPF("  Maximum pixel rate per second.            %d",lpInput->lpDDVideoPortDesc->dwMaxPixelsPerSecond);
	DPF("  Video port ID (0 - (dwMaxVideoPorts -1)). %d",lpInput->lpDDVideoPortDesc->dwVideoPortID);
	DPF("  Reserved for future use - set to zero.    %d",lpInput->lpDDVideoPortDesc->dwReserved1);
	
	DPF("  Size of the DDVIDEOPORTDESC.dwSize        %d",lpInput->lpDDVideoPortDesc->VideoPortType.dwSize);
	DPF("              DDVIDEOPORTDESC.dwPortWidth   %d",lpInput->lpDDVideoPortDesc->VideoPortType.dwPortWidth);
	DPF("              DDVIDEOPORTDESC.guidTypeID    %d",lpInput->lpDDVideoPortDesc->VideoPortType.guidTypeID);
	DPF("              DDVIDEOPORTDESC.dwFlags       %d",lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags);
	
	DPF("	Reserved for future use - set to zero.     %d",lpInput->lpDDVideoPortDesc->dwReserved2);
	DPF("	Reserved for future use - set to zero.     %d",lpInput->lpDDVideoPortDesc->dwReserved3);

	
	// We'll assume for now that since CanCreate passed that our parameters are valid at this point
	// we need to setup the entire NV object system at this point

	// theorectically we should not have a channel open... let's check that?


	
	((PDEV*)MySurfaces.pDrvDat)->hVPEChannel = DD_PIO_VPE_CHANNEL_OBJECT_HANDLE;
	if (NvAllocChannelPio(  ((PDEV*)MySurfaces.pDrvDat)->hDriver,
			((PDEV*)MySurfaces.pDrvDat)->hClient,
			((PDEV*)MySurfaces.pDrvDat)->hDevice,
			((PDEV*)MySurfaces.pDrvDat)->hVPEChannel,
			NV03_CHANNEL_PIO,
			0,
			(PVOID)&pMySurfaces->pVPChanPtr,
			0
			) != NVOS04_STATUS_SUCCESS)
	{
	   DISPDBG((1, "NVDD: Cannot get NV PIO channel"));
	   NvFree(((PDEV*)MySurfaces.pDrvDat)->hDriver, ((PDEV*)MySurfaces.pDrvDat)->hClient, 
				NV01_NULL_OBJECT, ((PDEV*)MySurfaces.pDrvDat)->hClient);
	   NvClose(((PDEV*)MySurfaces.pDrvDat)->hDriver);
	   return FALSE;
	}
	
	returnVal = SetupVP ( lpInput);
	

	// For the overlay flip, store away a channel pointer
#ifndef VPE_NT
#ifdef NV4
	pMySurfaces->pOverlayChanPtr = (NvChannel*) pDriverData->NvDevVideoFlatPio;
#else
	pMySurfaces->pOverlayChanPtr = (NvChannel*) pDriverData->NvDevFlatPio;
#endif	
	pMySurfaces->dwOverlaySubChan = NV_DD_SPARE;
	pMySurfaces->dwOverlayObjectID = NV_DD_YUV422_VIDEO_FROM_MEMORY;
	pMySurfaces->dwScalerObjectID = NV_DD_VIDEO_SCALER;
#else

  //CODE
  
#endif //VPE_NT		
	// don't forget to decrement the number of video ports avail number which is 
	lpInput->ddRVal = DD_OK;
	
	return returnVal;
}


/*
 * FlipVideoPort32
 *
 * This function is required
 */
DWORD __stdcall FlipVideoPort32 (LPDDHAL_FLIPVPORTDATA lpInput)
{

    /*
     * Program registers to do the actual flip
     */

    /*
     * Save information for BOTH lpSurfCurr so we can know in the future
     * when the physical flip has occurred.
     */


	// Check that both surfaces are in our list
	U032	newSurfaceIndex;
	U032	oldSurfaceIndex;
	
	DPF("***FlipVideoPort32");
	

	oldSurfaceIndex = findSurface(lpInput->lpSurfCurr);
	if(oldSurfaceIndex  != SURFACENOTFOUND ) {
		newSurfaceIndex = findSurface(lpInput->lpSurfTarg);
		if(newSurfaceIndex != SURFACENOTFOUND ) {
			//  Now do some stuff... uh.. what?
			
			// Find out which of the buffers what programmed for the old surface
			U032	dwBufferIndex = NOSURFACEPROGRAMED;
			{
				U032 i;
				for(i=0;i<4;i++) {
					if(oldSurfaceIndex == pMySurfaces->CurrentBufferSurface[i] ) {
						dwBufferIndex = i;
						break;
					}
				}
				if (i == 4 ) {
					DPF("   Not sure which buffer was attached to the Current Surface in FlipVideoPort");
					return DDHAL_DRIVER_NOTHANDLED;
				}	
			}
			
			
			//  We could wait here until we know the hardware is done...
			//  instead we'll just check it real quick and print a whiner if it's not done
			//  and then fail
			{
				NvNotification* 	pNotify = pMySurfaces->Surfaces[oldSurfaceIndex]->pNvNotify;
				if(pNotify != NULL) {
					if (pNotify->status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS ) {
						DPF("   Attempting to flip out a surface which is still in progress");
						return DDHAL_DRIVER_NOTHANDLED;
					}
					// check to see if we've had an error 
					//    there are LOTS of these....
					if ( (pNotify->status != NV04D_NOTIFICATION_STATUS_IN_PROGRESS ) |
						  (pNotify->status != NV04D_NOTIFICATION_STATUS_DONE_SUCCESS ) |
						  (pNotify->status != NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED ) ){
						DPF("   Attempting to flip out a surface which had an error during capture");
						return DDHAL_DRIVER_NOTHANDLED;
					}
				} else {
					DPF("   Attempting to flip out a surface where the notifier was NULL");
					return DDHAL_DRIVER_NOTHANDLED;
				}
			}
			
			// Ok, program up the switch
			ProgramBufferWithSurface(dwBufferIndex,newSurfaceIndex,FALSE);
			
		
		} else {
			DPF("   Unable to find Target Surface during FlipVideoPort32");
			DPF("   There was a note that we could be receiving new surfaces ");
			DPF("                      ( Tobin or MS ?)..... need to add code for this");
			
			return DDHAL_DRIVER_NOTHANDLED;
		}
	
	} else {
		DPF("   Unable to find Current Surface during FlipVideoPort32");
		return DDHAL_DRIVER_NOTHANDLED;
	}
  	
	lpInput->ddRVal = DD_OK;
	
	return DDHAL_DRIVER_HANDLED;
}

/*
 * GetVideoPortBandwidth32
 *
 * This function is required
 */
DWORD __stdcall GetVideoPortBandwidth32 (LPDDHAL_GETVPORTBANDWIDTHDATA lpInput)
{

	DPF("***GetVideoPortBandwidth32");
	/*
	 * Determine how much bandwidth is available for the specified
	 * output format and fill in the information.
	 */
	 
	 // we are lucky in that we basically only support ONE format  YEAH!
	 
	 
	if (lpInput->dwFlags & DDVPB_TYPE) {
		lpInput->lpBandwidth->dwCaps = DDVPBCAPS_DESTINATION;  // we are limited by the "shrink" of the overlay 			
	}
	 
	if (lpInput->dwFlags & DDVPB_VIDEOPORT) {
		// The following four should indicate the MINIMUM stretch values represented as percentages times 10
		//  so if we could not stretch below 75% of the orginal size, the we should put in 750
		// for now I'll leave these "as is" because I need to collect the correct values -CJS
		lpInput->lpBandwidth->dwOverlay 				= 1000;
		lpInput->lpBandwidth->dwColorkey 				= 1000;  // Overlay/video port/colorkey works at 1X
		lpInput->lpBandwidth->dwYInterpolate			= 1000;	 // Overlay/video port works at 1X
		lpInput->lpBandwidth->dwYInterpAndColorkey		= 1000;  // Overlay/video port/interpolation works at 2X
	}
	
	
	if (lpInput->dwFlags & DDVPB_OVERLAY) {
		DPF("   We are NOT an overlay limited device but a destination ");
		DPF("      limited device we should not have been called");
	}
	
	lpInput->ddRVal = DD_OK;
	return DDHAL_DRIVER_HANDLED; 
}

/*
 * GetVideoPortInputFormat32
 *
 * This function is required
 */
DWORD __stdcall GetVideoPortInputFormat32 (LPDDHAL_GETVPORTINPUTFORMATDATA lpInput)
{
   /* Only 1 input format supported UYVY 4:2:2 */
	/* Data stored in 32bit word as U[7:0] Y1[15:8] V[23:16] Y2[31:24] */
	DPF("   ***GetVideoPortInputFormat32");
	/*
	* The HAL is gaurenteed that the buffer in lpInput->lpddpfFormat
	* is large enough to hold the information
	*/

	DPF("   ***GetVideoPortInputFormat32");

	if (lpInput->dwFlags & DDVPFORMAT_VIDEO ) {
		lpInput->dwNumFormats = 1;
		if (lpInput->lpddpfFormat != NULL) {
			memcpy (lpInput->lpddpfFormat, &InputFormats[0],  sizeof (DDPIXELFORMAT));
		}
	}

	if (lpInput->dwFlags & DDVPFORMAT_VBI ) {
		lpInput->dwNumFormats = 1;
		if (lpInput->lpddpfFormat != NULL) {
			memcpy (lpInput->lpddpfFormat, &InputFormats[0],  sizeof (DDPIXELFORMAT));
		}
	}

	lpInput->ddRVal = DD_OK;
	return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortOutputFormat32
 *
 * This function is required
 */
DWORD __stdcall GetVideoPortOutputFormat32 (LPDDHAL_GETVPORTOUTPUTFORMATDATA lpInput)
{
  	/* OutputFormat does not convert UYUV to RGB, this would be done by Overlay control */
	/* Data stored in 32bit word as U[7:0] Y1[15:8] V[23:16] Y2[31:24] */

	DPF("***GetVideoPortOutputFormat32");


	/* OutputFormat does not convert UYUV to RGB, this would be done by Overlay control */
	/* Data stored in 32bit word as U[7:0] Y1[15:8] V[23:16] Y2[31:24] */

	// default
	lpInput->dwNumFormats = 1;
	if (lpInput->lpddpfOutputFormats != NULL) {
		memcpy (lpInput->lpddpfOutputFormats, lpInput->lpddpfInputFormat, sizeof (DDPIXELFORMAT));
	}

	// for the video data
	if (lpInput->dwFlags & DDVPFORMAT_VIDEO) {// video formats
		// is input format a FOURCC
		if (lpInput->lpddpfInputFormat->dwFlags & DDPF_FOURCC ) {
			// is it UYVY
			if (lpInput->lpddpfInputFormat->dwFourCC == mmioFOURCC('U','Y','V','Y')) {
				// translation of UYVY is UYVY (ie no translation)
   	    	lpInput->dwNumFormats = 1;
	   	   if (lpInput->lpddpfOutputFormats != NULL) {
					memcpy (lpInput->lpddpfOutputFormats, InputFormats, sizeof (DDPIXELFORMAT) );
		    	}
			}
		}
	}
	
	// for the VBI data
	if (lpInput->dwFlags & DDVPFORMAT_VBI) {// VBI formats
		// is input format a FOURCC
		if (lpInput->lpddpfInputFormat->dwFlags & DDPF_FOURCC ) {
			// is it UYVY
			if (lpInput->lpddpfInputFormat->dwFourCC == mmioFOURCC('U','Y','V','Y')) {
				// translation of UYVY is UYVY (ie no translation)
   	    	lpInput->dwNumFormats = 1;
	   	   if (lpInput->lpddpfOutputFormats != NULL) {
					memcpy (lpInput->lpddpfOutputFormats, InputFormats, sizeof (DDPIXELFORMAT) );
		    	}
			}
		}
	}
	
	lpInput->ddRVal = DD_OK;
   return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortConnect32
 *
 * Returns the GUIDs describing the connections that can be supported.
 * Think of a GUID simply as a 128 bit FOURCC that you don't have to
 * register.  This function is required.
 */
DWORD __stdcall GetVideoPortConnectInfo (LPDDHAL_GETVPORTCONNECTDATA lpInput)
{
    /*
     * If lpInput->lpGUIDs is NULL, simply return the number of GUIDS
     * supported
     */

	// eventually we should check dwPortId if there are more than one port in operation.
	// however since all the port will likely have the same capabilities maybe it's not an
	// issue
	
	DPF("***GetVideoPortConnectInfo");

	lpInput->dwNumEntries = 1;
	if(lpInput->lpConnect == NULL) {
		// this means that Dx is looking for the number of ways you can connect so that it can
		// allocate a structure for the next time around
		lpInput->ddRVal = DD_OK;
	} else {
		// fill in the one structure that we asked for..	
		memcpy (&(lpInput->lpConnect->guidTypeID), &DDVPTYPE_CCIR656, sizeof (GUID));
		lpInput->lpConnect->dwPortWidth = 8;
		lpInput->lpConnect->dwFlags = MY_CONNECTION_CAPS;	
	}

	return DDHAL_DRIVER_HANDLED;

}


/*
 * DestroyVideoPort32
 *
 * This optional function notifies the HAL when the video port
 * has been destroyed.
 */
DWORD __stdcall DestroyVideoPort32 (LPDDHAL_DESTROYVPORTDATA lpInput)
{
	
	DPF("***DestroyVideoPort32");
	
	pMySurfaces->ImageCallbackRoutine = NULL;			
	pMySurfaces->VBICallbackRoutine = NULL;			
		
	tearDownVP();
	
	lpInput->ddRVal = DD_OK;
	
	return DDHAL_DRIVER_HANDLED;
}




/*
 * GetVideoPortFlipStatus32
 *
 * This required function allows DDRAW to restrict access to a surface
 * until the physical flip has occurred, allowing doubled buffered capture.
 */

DWORD __stdcall GetVideoPortFlipStatus32 (LPDDHAL_GETVPORTFLIPSTATUSDATA lpInput)
{
	
	U032	dwIndex;
	
	//DPF("***GetVideoPortFlipStatus32");
	if( MySurfaces.dwVideoPortStatus == VP_NOT_RUNNING ) {
   	lpInput->ddRVal = DD_OK;
		return DDHAL_DRIVER_HANDLED;
	}
	
	// search through all of our known surfaces looking for the one it's curious about
	for (dwIndex = 0; dwIndex < pMySurfaces->numberOfSurfaces ; dwIndex++) {
		if (pMySurfaces->Surfaces[dwIndex]->pVidMem == lpInput->fpSurface) {
			// this means we've found the surface that DX is curious about												
			if( pMySurfaces->Surfaces[dwIndex]->pNvNotify == NULL ) {
				ASSERT(1==0);
				DPF("   NOTIFY WAS NULL  !!!!!!!!!!!");
   			lpInput->ddRVal = DD_OK;
				return DDHAL_DRIVER_HANDLED;
			}
			
			if(pMySurfaces->Surfaces[dwIndex]->pNvNotify->status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) {
				lpInput->ddRVal = DDERR_WASSTILLDRAWING;
			} else {
				lpInput->ddRVal = DD_OK;
			}
			// return here... we found the surface
			return DDHAL_DRIVER_HANDLED;
		}
	}
	
	DPF("   ***FlipStatus having trouble  --> Surface does");
	DPF("          not exist in my list of surfaces");
	lpInput->ddRVal = DD_OK;

	return DDHAL_DRIVER_HANDLED;
}



U032 startSurfaces(U032	bInterleave, VPSTATE*	pVideoPortStatus ) {
	U032	rValue = DD_OK;
	VPSTATE	myState = *pVideoPortStatus;
	// figure out which buffers to program up and launch them...
	if(bInterleave) {
		 DPF("   Now Launching surfaces.... INTERLEAVED");
	} else {
		 DPF("   Now Launching surfaces.... NOT INTERLEAVED");
	}
	// zero out the last overlay scaler size so we're sure it gets reprogrammed
	pMySurfaces->lastsrcDeltaY = 0;
	pMySurfaces->bStopVP = FALSE;	
	if( bInterleave ) {
		// both buffers will point to the same surface
		if( pMySurfaces->VBIstart >0 ) {
			if( ProgramBufferWithSurface(0,0,FALSE) )
				rValue = DDERR_INVALIDPARAMS;
			
			// since this is interleave always start the second buffer with the first surface
			if( ProgramBufferWithSurface(1,0,FALSE) )
				rValue = DDERR_INVALIDPARAMS;
			myState |= VP_IMAGE_CAPTURING;
		}
	} else {
	
		// buffers will point to consecutive surfaces
		if( pMySurfaces->VBIstart >0 ) {
			if( ProgramBufferWithSurface(0,0,FALSE) )
				rValue = DDERR_INVALIDPARAMS;
			myState |= VP_IMAGE_CAPTURING;
		}
		// check for another surface
		if( pMySurfaces->VBIstart > 1 ) {
			if( ProgramBufferWithSurface(1,1,FALSE) )
				rValue = DDERR_INVALIDPARAMS;
		} else {
			// fire up the second buffer at the first surface
			if( ProgramBufferWithSurface(1,0,FALSE) )
				rValue = DDERR_INVALIDPARAMS;
		}
	}
			
	// Now do VBI
	// start first vbi on first surface
	if( (pMySurfaces->numberOfSurfaces - pMySurfaces->VBIstart) > 0 ) {
		if( ProgramBufferWithSurface(2,pMySurfaces->VBIstart,FALSE))
				rValue = DDERR_INVALIDPARAMS;
		myState |= VP_VBI_CAPTURING;
	
		// now check to see if we have another surface
		if( (pMySurfaces->numberOfSurfaces - pMySurfaces->VBIstart) > 1 ) {
			if( ProgramBufferWithSurface(3,pMySurfaces->VBIstart+1,FALSE))
					rValue = DDERR_INVALIDPARAMS;
		} else {
			// set second buffer to first surface
			if( ProgramBufferWithSurface(3,pMySurfaces->VBIstart+0,FALSE))
					rValue = DDERR_INVALIDPARAMS;
		}
	}	
	*pVideoPortStatus = myState;
	
	return rValue;
}


/*
** UpdateVideoPort32
**
** This required function sets up the video port
*/
 
DWORD __stdcall UpdateVideoPort32 (LPDDHAL_UPDATEVPORTDATA lpInput)
{

	VPERRORS myErrors;
	U032		bAutoflip;
#ifndef NV4
	U032		bDelayNV3;
#endif
	
	DPF("***UpdateVideoPort32");


	
	lpInput->ddRVal = DD_OK;
		
	{		
		/*
		 * Turn off the video port
		 */
//CODE
		DPF("   now Stopping the Video Port");

		StopVP();
	
		MySurfaces.dwVideoPortStatus = VP_NOT_RUNNING;
    
	} 
	
	if (lpInput->dwFlags != DDRAWI_VPORTSTOP) { 
#ifndef NV4
		bDelayNV3 = FALSE;
#endif //NV4
		if (lpInput->dwFlags == DDRAWI_VPORTSTART) {
			DPF("   now Starting Video Port for the first time");

		// This is the first round....
#ifndef NV4
			bDelayNV3 = TRUE;
#endif	//NV4

		} 
		
		DPF("   now updating the video port");

		{
			// print some stuff out about the flags we're lookin at:
			
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_AUTOFLIP	) {
				DPF("      FLAG DVP_AUTOFLIP            on");	
			} else {
				DPF("      FLAG DVP_AUTOFLIP            off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_CONVERT		) {
				DPF("      FLAG DDVP_CONVERT            on");	
			} else {
				DPF("      FLAG DDVP_CONVERT            off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP					) {
				DPF("      FLAG DDVP_CROP               on");	
			} else {
				DPF("      FLAG DDVP_CROP               off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE			) {
				DPF("      FLAG DDVP_INTERLEAVE         on");	
			} else {
				DPF("      FLAG DDVP_INTERLEAVE         off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_MIRRORLEFTRIGHT		) {
				DPF("      FLAG DDVP_MIRRORLEFTRIGHT    on");	
			} else {
				DPF("      FLAG DDVP_MIRRORLEFTRIGHT    off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_MIRRORUPDOWN			) {
				DPF("      FLAG DDVP_MIRRORUPDOWN       on");	
			} else {
				DPF("      FLAG DDVP_MIRRORUPDOWN       off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_PRESCALE				) {
				DPF("      FLAG DDVP_PRESCALE           on");	
			} else {
				DPF("      FLAG DDVP_PRESCALE           off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPEVENFIELDS		) {
				DPF("      FLAG DDVP_SKIPEVENFIELDS     on");	
			} else {
				DPF("      FLAG DDVP_SKIPEVENFIELDS     off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPODDFIELDS			) {
				DPF("      FLAG DDVP_SKIPODDFIELDS      on");	
			} else {
				DPF("      FLAG DDVP_SKIPODDFIELDS      off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_SYNCMASTER				) {
				DPF("      FLAG DDVP_SYNCMASTER         on");	
			} else {
				DPF("      FLAG DDVP_SYNCMASTER         off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_VBICONVERT				) {
				DPF("      FLAG DDVP_VBICONVERT         on");	
			} else {
				DPF("      FLAG DDVP_VBICONVERT         off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_VBINOSCALE				) {
				DPF("      FLAG DDVP_VBINOSCALE         on");	
			} else {
				DPF("      FLAG DDVP_VBINOSCALE         off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_OVERRIDEBOBWEAVE		) {
				DPF("      FLAG DDVP_OVERRIDEBOBWEAVE   on");	
			} else {
				DPF("      FLAG DDVP_OVERRIDEBOBWEAVE   off");	
			}
			if(lpInput->lpVideoInfo->dwVPFlags & DDVP_IGNOREVBIXCROP		) {
				DPF("      FLAG DDVP_IGNOREVBIXCROP     on");	
			} else {
				DPF("      FLAG DDVP_IGNOREVBIXCROP     off");	
			}
			
		
		}
		
		// Decide which capture mode to be in given if we're trying to skip some types of fields
		
		
		if( lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPEVENFIELDS ) {
			pMySurfaces->CurrentCaptureMode = ODDODD;
		} else if ( lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPODDFIELDS ) {
			pMySurfaces->CurrentCaptureMode = EVENEVEN;
		} else {
			if(pMySurfaces->bInputInterLaced)
				pMySurfaces->CurrentCaptureMode = EVENODD;
			else
				pMySurfaces->CurrentCaptureMode = PROGRESSIVE;
		}
		
		if( lpInput->lpVideoInfo->dwVPFlags & DDVP_AUTOFLIP ) {
			bAutoflip = TRUE;
		} else {
			bAutoflip = FALSE;
		}
		
		if( lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE ) {
			pMySurfaces->CurrentScanMode = INTERLEAVBUF;
		} else {
			pMySurfaces->CurrentScanMode = SEPERATEBUF;
		}
		
		{	
			// Here's where we're going to try to figure out where to put things in the vertical and horz
			// dimensions
			
			// At key thing here are the two different modes which people are using to capture VBI
			// one is that the VBI data is captured to the very beginning of the VIDEO surface in one big surface
			// the other mode is with a seperate surface
			
			// we can detect which mode by examining the number of VBI surface if > 0 we're in the second mode
		
			// When there ARE VBI surfaces, then we should jam the Video data right to the very beginning of the
			// video surface, since we know this is where it will be expected.
			
			// additionally we've reported DDVPCONNECT_DISCARDSVREFDATA is true which means 
			// "/*
			//	 * Indicates that any data written to the video port during the VREF
			//	 * period will not be written into the frame buffer. This flag is read only.
			//	 */"
		   //  MS of course not really defining what the VREF period means in their land.
			
				
#define VBIVALIDDATALINE  (0x4)	

			pMySurfaces->dwVBIStartLine = VBIVALIDDATALINE;	// at least we're not starting with zero this first four are always
																			// empty as best as I know
			
			pMySurfaces->dwVBIHeight = lpInput->lpVideoInfo->dwVBIHeight;
			
			
			pMySurfaces->dwImageStartLine = pMySurfaces->dwVBIHeight + pMySurfaces->dwVBIStartLine - VBIVALIDDATALINE;
			
			// if  	dwImageStartLine < 1  then things don't work to well
			if(pMySurfaces->dwImageStartLine < 1) pMySurfaces->dwImageStartLine = 1;
			
			// this is the only way I know how to do the check for vertical cropping
			if ( lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP ) {
				pMySurfaces->dwOutHeight = (DWORD)lpInput->lpVideoInfo->rCrop.bottom - lpInput->lpVideoInfo->rCrop.top;				
				pMySurfaces->dwOutWidth  = lpInput->lpVideoInfo->rCrop.right - lpInput->lpVideoInfo->rCrop.left;		
				DPF("   UPDATE:         crop TOP %d",lpInput->lpVideoInfo->rCrop.top);				
				if(lpInput->lpVideoInfo->rCrop.top > (signed long) pMySurfaces->dwImageStartLine ) {
					pMySurfaces->dwImageStartLine = lpInput->lpVideoInfo->rCrop.top;
				}		
			} else {
				pMySurfaces->dwOutHeight = lpInput->lpVideoPort->ddvpDesc.dwFieldHeight;
				pMySurfaces->dwOutWidth  = lpInput->lpVideoPort->ddvpDesc.dwFieldWidth;
			}
			
			// this is something of a hack I suppose
			pMySurfaces->dwOutHeight = pMySurfaces->dwOutHeight;			
						
			// if we're prescaling then do something
			if (DDVP_PRESCALE & lpInput->lpVideoInfo->dwVPFlags) {
				pMySurfaces->dwPreHeight = lpInput->lpVideoInfo->dwPrescaleHeight;				
				pMySurfaces->dwPreWidth  = lpInput->lpVideoInfo->dwPrescaleWidth;
			} else {
				pMySurfaces->dwPreHeight = pMySurfaces->dwOutHeight;
				pMySurfaces->dwPreWidth = pMySurfaces->dwOutWidth;
			}
			
			
			pMySurfaces->dwOriginX = lpInput->lpVideoInfo->dwOriginX;
			pMySurfaces->dwOriginY = lpInput->lpVideoInfo->dwOriginY;
		}
		
		switch (pMySurfaces->CurrentCaptureMode) {
				case EVENODD:
					DPF("   UPDATE:     capture mode EVENODD");	
					break;
				case EVENEVEN:
					DPF("   UPDATE:     capture mode EVENEVEN");							
					break;
				case ODDODD:
					DPF("   UPDATE:     capture mode ODDODD");	
					break;
		}
		switch (pMySurfaces->CurrentScanMode) {
				case SEPERATEBUF:
					DPF("   UPDATE:        scan mode SEPERATEBUF");	
					break;
				case INTERLEAVBUF:
					DPF("   UPDATE:        scan mode INTERLEAVBUF");	
					break;
		}
		
		DPF("   UPDATE: image start line %d",pMySurfaces->dwImageStartLine);
		DPF("   UPDATE: image out dimens %d,%d",pMySurfaces->dwOutWidth,pMySurfaces->dwOutHeight);
		DPF("   UPDATE: image pre dimens %d,%d",pMySurfaces->dwPreWidth,pMySurfaces->dwPreHeight);
		DPF("   UPDATE:   image position %d,%d",pMySurfaces->dwOriginX,pMySurfaces->dwOriginY);

		myErrors = replaceSurfaceList(lpInput->dwNumAutoflip,lpInput->dwNumVBIAutoflip,
										lpInput->lplpDDSurface, lpInput->lplpDDVBISurface,bAutoflip);
		if( myErrors == NOVPERROR ) {
#ifndef NV4
			lpInput->ddRVal = startSurfaces( lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE, &(MySurfaces.dwVideoPortStatus));
#else
			{
				lpInput->ddRVal = startSurfaces( lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE, &(MySurfaces.dwVideoPortStatus));
			}
#endif //NV4
#ifndef NV4
						
		} else {
			DPF("   Problem updating the Video Port = %d",myErrors);

			switch (myErrors) {
					case CANTCREATESURFACE:
						lpInput->ddRVal = DDERR_OUTOFMEMORY;
						break;
					case TOOMANYSURFACESERROR:
					case  SURFACENOTFOUND:
					default:
						lpInput->ddRVal = DDERR_INVALIDPARAMS;
						break;
			}
				
			return DDHAL_DRIVER_HANDLED;
	
		
			
#endif //NV4
		}	// NOVPERROR
	}  // DDRAWI_VPORTSTOP

	return DDHAL_DRIVER_HANDLED;
}



/*
 * WaitForVideoPortSync32
 *
 * Wait for the LPB Video VSYNC (note: this is entirely
 * different from the screen refresh VGA VSYNC).
 *
 * TBD: No hard polling loops.  Must use lpInput->dwTimeOut
 */
DWORD __stdcall WaitForVideoPortSync32 (LPDDHAL_WAITFORVPORTSYNCDATA lpInput)
{
	DPF("***WaitForVideoPortSync32");
	
	lpInput->ddRVal = DDVPSQ_NOSIGNAL;
	
	// if the port is running then
	if (MySurfaces.dwVideoPortStatus != VP_NOT_RUNNING) {
		if( lpInput->dwFlags & DDVPWAIT_BEGIN ) {
			// wait for a vsync start.   timeout in lpInput->dwTimeOut
			U032	bigTime;
			U032	smallTime;
			U032	milliTime;
			U032	recentTime[MAXVPSURFACES];
			U032 	dwIndex;
			U032	searchMAX;
	
		// find out what time it is now
			getCurrentNanoTimer(&bigTime, &smallTime);
			milliTime = getMilliTime();
			
			if( pMySurfaces->VBIstart == 0 ) {
				// we're only running VBI so check ALL the surfaces
				searchMAX = pMySurfaces->numberOfSurfaces;
			} else {
				// there is at least one image surface, so check for that
				searchMAX = pMySurfaces->VBIstart;
			}
			//find the current notifiers for the IMAGE surfaces
			for (dwIndex=0;dwIndex<searchMAX;dwIndex++) {
				if( pMySurfaces->Surfaces[dwIndex]->pNvNotify != NULL ) {
					recentTime[dwIndex] = pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[0];
				} else {
					recentTime[dwIndex] = 0;
				}
			}
			while( (getMilliTime() < ( milliTime + lpInput->dwTimeOut)) && ( lpInput->ddRVal == DDVPSQ_NOSIGNAL) ) {
				for (dwIndex=0;dwIndex<searchMAX;dwIndex++) {
					if( pMySurfaces->Surfaces[dwIndex]->pNvNotify != NULL ) {
						if( recentTime[dwIndex] < pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[0] ) {
							// ok, ONE Of the notifiers moved foward
							lpInput->ddRVal = DD_OK; 	//  We found a sync
						}
					}
				}
			}
			// start a while		
		} else if (lpInput->dwFlags & DDVPWAIT_END ) {
			//  timeout in lpInput->dwTimeOut
			// we could wait for ysync that then see which buffer we're on then wait for the line counter
			// to change  .. YUCK
			DPF("   Sorry, don't handle waiting for the end");
	
			return DDHAL_DRIVER_NOTHANDLED;
		} else if (lpInput->dwFlags & DDVPWAIT_LINE ) {
			//timeout in lpInput->dwTimeOut
				// lpInput ->dwLine
			DPF("   Sorry, can't wait for a particular line");
			return DDHAL_DRIVER_NOTHANDLED;
		};
		
	}// videoport status
	
	return DDHAL_DRIVER_HANDLED;
}


DWORD __stdcall GetVideoSignalStatus32 ( LPDDHAL_GETVPORTSIGNALDATA lpInput) {	
	
	U032	dwIndex;
	ASSERT(lpInput != NULL);

	DPF("***GetVideoSignalStatus32");
	if( lpInput == NULL)
	{
		DPF("   Input NULL in GetVideoSignalStatus32");
		return DDHAL_DRIVER_NOTHANDLED;
	}
	// If the video port is not running, then we just don't know...
	if( MySurfaces.dwVideoPortStatus == VP_NOT_RUNNING ) {
		DPF("   could not get the signal status, the video port is not running");
		// we'll return no signal in this case....
		lpInput->dwStatus = DDVPSQ_NOSIGNAL;
		lpInput->ddRVal = DDERR_VIDEONOTACTIVE;

		return DDHAL_DRIVER_HANDLED;
	}
	
	lpInput->ddRVal = DD_OK;
	// default for now is that the signal is good
	lpInput->dwStatus = DDVPSQ_SIGNALOK;
	// first check the notifiers to see if there has been ANY sync
	//     do this by seeing if the notify times are none zero
	{
		U032	probablyRunning = FALSE;
		for (dwIndex=0;dwIndex<pMySurfaces->numberOfSurfaces;dwIndex++) {
			if( pMySurfaces->Surfaces[dwIndex]->pNvNotify != NULL ) {
				if (pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[0]!=0) {
					probablyRunning = TRUE;
					break;
				}
			}
		}
		
		// if the timer are zero then wait at least 1/24 of a second and check again.
		if(!probablyRunning) {
			// find current time
			U032	 tempTime;
			tempTime = getMilliTime();
			//now wait for 1/24 of a second
			while(tempTime + (1000/24) > getMilliTime()); 
			// and then check one more time for a possible completion
			for (dwIndex=0;dwIndex<pMySurfaces->numberOfSurfaces;dwIndex++) {
				if( pMySurfaces->Surfaces[dwIndex]->pNvNotify != NULL ) {
					if (pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[0]!=0) {
						probablyRunning = TRUE;
						break;
					}
				}
			}
			
			// if it's still non zero report no signal
			if(!probablyRunning) {
				lpInput->dwStatus = DDVPSQ_NOSIGNAL;
			}

		}	
	}
	
	// have we already decided that the signal is bad...?
	if( lpInput->dwStatus != DDVPSQ_NOSIGNAL ) {
		
		// Now the default is that the signal is bad
		lpInput->dwStatus = DDVPSQ_NOSIGNAL;
		
		// check to see that at least one of of the notifiers is LESS than 1/24 of a second
		for (dwIndex=0;dwIndex<pMySurfaces->numberOfSurfaces;dwIndex++) {
			if( pMySurfaces->Surfaces[dwIndex]->pNvNotify != NULL ) {
				if (pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[0]!=0) {
					U032 diff;
					// ok, we've got a non-zero surface, let's check to see how recent it is
					U032	BigCurrentTime;
					U032	SmallCurrentTime;
					
					getCurrentNanoTimer(&BigCurrentTime,&SmallCurrentTime);
					if(pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[1] < BigCurrentTime ) {
						// we've got a wrap around situation.  this means that we need to work in weirder math
						diff = (
									((0x80000000)  - (pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[0] >> 1))
									+  (SmallCurrentTime >> 1)
								 ) << 1;
					} else {
						// do a simple subtraction
						diff = SmallCurrentTime - pMySurfaces->Surfaces[dwIndex]->pNvNotify->timeStamp.nanoseconds[0];
					}
					//diff now has the delta between the notify and "now"  
					// if the notifier is LESS than a frame period then we've got a signal
					if(diff < MAXFRAMEPERIOD ) {
						lpInput->dwStatus = DDVPSQ_SIGNALOK;
					}
					
				}
			}
		}

	}
	
	return DDHAL_DRIVER_HANDLED;
}

/*
 * GetVideoPortField32
 *
 * This function is only required if readback of the current
 * field is supported.
 */

DWORD __stdcall GetVideoPortField32 (LPDDHAL_GETVPORTFIELDDATA lpInput)
{
    /*
     * Make sure the video port is ON.  If not, set
     * lpInput->ddRVal to DDERR_VIDEONOTACTIVE and return.
     */

    /*
     * Readback whether the field is even or odd.  lpInput->bField is
     * set to TRUE if the current field is even.
     */
	//DPF("***GetVideoPortField32");
	
	if (MySurfaces.dwVideoPortStatus == VP_NOT_RUNNING) {
		//DPF("   Video is not active!!!");
		lpInput->bField = 0;
	   lpInput->ddRVal = DDERR_VIDEONOTACTIVE;	
	} else {
	 /*
     * Readback the current line number and put it in
     * lpInput->dwLine
     */

		lpInput->bField = GetField();
		
		lpInput->ddRVal = DD_OK;

		lpInput->bField = 0;
	}	
	
	return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortLine32
 *
 * This function is only required if readback of the current
 * video line number (0 relative) is supported.
 */
DWORD __stdcall GetVideoPortLine32 (LPDDHAL_GETVPORTLINEDATA lpInput)
{

	DPF("***GetVideoPortLine32");
 
 
	if (MySurfaces.dwVideoPortStatus == VP_NOT_RUNNING) {
		//DPF("   Video is not active!!!");
		lpInput->dwLine = 0;
	   lpInput->ddRVal = DDERR_VIDEONOTACTIVE;	
	} else {
	 /*
     * Readback the current line number and put it in
     * lpInput->dwLine
     */

		lpInput->dwLine = getCurLine();
		lpInput->ddRVal = DD_OK;
		
	}	
	
	return DDHAL_DRIVER_HANDLED;
}

/*
 ** SyncVideoPortData32
 *
 *  FILENAME: C:\win9x\display\mini\s3\S3_DD32.C
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
 */
#if 0
DWORD WINAPI SyncVideoPortData32(LPDDHAL_SYNCVIDEOPORTDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    DPF("SyncVideoPortData");

    lpInput->dwOriginOffset = pLPB->dwBaseOffset;

    /*
    ** !! KMVT Field dropping functionality requires a
    ** place to cache the "real" decimation pattern.  So
    ** we (S3) will reserve dwDriverReserved1 for this
    ** purpose.
    */
    lpInput->dwDriverReserved1 = pLPB->dwVDecimationPattern;

    return DDHAL_DRIVER_HANDLED;
}

#endif

/*
 ** SyncSurfaceData32
 *
 *  FILENAME: C:\win9x\display\mini\s3\S3_DD32.C
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
 */
#if 0
DWORD WINAPI SyncSurfaceData32(LPDDHAL_SYNCSURFACEDATA lpInput)
{
    LPS3THKINFO sData = VP_GETSDATA(lpInput);

    lpInput->dwSurfaceOffset
        = SCREENOFFSET(sData, lpInput->lpDDSurface->lpGbl->fpVidMem);
    lpInput->dwOverlayOffset
        = lpInput->dwSurfaceOffset + UPDATEOVERLAYFLIPOFFSET(sData);

    DPF("SyncSurfaceData dwOverlayOffset=%lx", lpInput->dwOverlayOffset);

    return DDHAL_DRIVER_HANDLED;
}

#endif


/*
** IndexFromGUID
**
** Get the index into our supported connection table,
** given a GUID.  returns 0 if it isn't a supported
** connection type
*/
int IndexFromGUID(GUID *pGUID)
{
    int i, index;

    index = -1;                          // assume not found

    for (i=0; i<MAX_VP_TYPES; i++)
    {
        if (memcmp(guids[i], pGUID, sizeof(GUID)) == 0)
        {
            index = i;                  // found a match
            break;
        }
    }

    //TRACE("IndexFromGUID found GUID %d",i);

    return index;
}

//#endif // DX5
