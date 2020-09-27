/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
*                                                                           *
* Module: VP.c                                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           3/26/98                                          *
*                                                                           *
\***************************************************************************/

/* #includes ---------------------------------------------*/

// MS includes
#include "windows.h"
#include "mmsystem.h"
#include "ddraw.h"
#include "ddrawi.h"
#include "ddmini.h"
#include "ddkmmini.h"
#include "minivdd.h"
#include "dvp.h"

// NV includes
#ifndef NV4
#include "nv32.h"
#include "nvwin32.h"
#else
#include "realnv32.h"
#include "realnvwin32.h"
#endif

#ifndef NV4
#include "nvddobj.h"
#endif // NV4

#include "nvdd.h"
#include "nvcm.h"

#include "surfaces.h"
								

// provides access to the global data structures in ddmini.h
extern 		GLOBALDATA*			pDriverData;


volatile U032* 	nvTimerPtr = NULL;


vpSurfaces*			pMySurfaces = NULL;
NvNotification*	nvMyVPNotifiers = NULL;

const DWORD MY_CONNECTION_CAPS=	//DDVPCONNECT_DOUBLECLOCK			|
											//DDVPCONNECT_VACT					|
											/* we're capable of inverting the polarity */
											DDVPCONNECT_INVERTPOLARITY		|					
											/* we discard at least until the V bit goes low, and potentially more depending
											  on the setting of YCROP  as well as the current VBI situation  */
											//DDVPCONNECT_DISCARDSVREFDATA	|
											/* we WILL count the halfline */
											DDVPCONNECT_HALFLINE				|
											/*  these next are "client" only 
											//DDVPCONNECT_INTERLACED			|
											//DDVPCONNECT_SHAREEVEN			|
											//DDVPCONNECT_SHAREODD				|
											*/
											0;

DDPIXELFORMAT pf[] = {
	{sizeof(DDPIXELFORMAT),DDPF_FOURCC,  mmioFOURCC('U','Y','V','Y'),16,(DWORD)-1,(DWORD)-1,(DWORD)-1},
};
/*DDPIXELFORMAT pfvbi[] = {
	{sizeof(DDPIXELFORMAT),DDPF_FOURCC,  mmioFOURCC('R','A','W','8'),8,(DWORD)-1,(DWORD)-1,(DWORD)-1},
};
*/
DDPIXELFORMAT pfvbi[] = {
	{sizeof(DDPIXELFORMAT),DDPF_FOURCC,  mmioFOURCC('R','A','W','8'),8,(DWORD)-1,(DWORD)-1,(DWORD)-1},
};



//#define TESTCALLBACK
#ifdef TESTCALLBACK

void JoeCoolDudesCallbackRoutine(sCallBack* pCallbackStruct);

#endif

U032	getMilliTime(void);


void	getCurrentNanoTimer(U032 *	Most, U032 * Least) {
	volatile U032* myTimerLo = nvTimerPtr;
	volatile U032* myTimerHi = (U032*)(((U032)nvTimerPtr)+0x10);
	ASSERT(nvTimerPtr != NULL);
	ASSERT(Most 		!= NULL);
	ASSERT(Least 		!= NULL);
	*Least = *myTimerLo;
	*Most	= *myTimerHi;
}

U032	getMilliTime(void) {
	U032	bigTime;
	U032	smallTime;
	
	getCurrentNanoTimer(&bigTime, &smallTime);
	return (smallTime / 1000000) + bigTime * (0xFFFFFFFF/1000000);
			
}
	
/*

It appears that BPC has been programmd in such a way as to violate the API for VP.
So we need to compensate.  The problem is that the following scenario can happen.
CanCreateVP   VBI
CreateVP   VBI
UpdateVP   VBI
time passes
CanCreateVP   IMAGE
CreateVP  Image
UpdateVP   VBI+Image
timepasses
Destroy VP.

The key thing here is that the video ports will use the SAME port ID. so that should help us.


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
	
	if( pDriverData == NULL ) {
		DPF("   ***VideoPort not creatable  -->  pDriverData = NULL!   YIKES!");
		lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
		return DDHAL_DRIVER_HANDLED;
	}
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
	
	// check to see if there are ports available... by looking at local allocation
	if (pDriverData->dwVideoPortsAvailable <=0 ) {

			// check to see if there are ports available... by looking at IDs
	   if (lpInput->lpDDVideoPortDesc->dwVideoPortID != 0 ) {
			DPF("   ***VideoPort not creatable  --> not enough video ports");
			lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
			return DDHAL_DRIVER_HANDLED;
		}

		DPF("CanCreate for Second VP with same ID Allowed.... silly API");
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


U032 	HookUpVxdStorage(void) {

	
	HANDLE 		aVXDHandle;
	U032 i;
	
	//DPF("NOW ATTEMPTING VXD stuff");
	

	
	//DPF("\n\n\n\nThe DeviceID is:%d\n\n\n\n",pDriverData->thisDeviceID);
		
	if( pDriverData->thisDeviceID == 0 ) {
		DPF("We are Primary controller! \n");
#ifdef NV4
	
		aVXDHandle = CreateFile("\\\\.\\NV4.VXD", 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#else
		aVXDHandle = CreateFile("\\\\.\\NV3.VXD", 0,FILE_SHARE_READ,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#endif
	
	} else {
		DPF("We are not primary controller :-( \n");
#ifdef NV4
	
		aVXDHandle = CreateFile("\\\\.\\NV4mini2.VXD", 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#else
		aVXDHandle = CreateFile("\\\\.\\NV3mini2.VXD", 0,FILE_SHARE_READ,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#endif
	
	
	}

	if((aVXDHandle == NULL) || (aVXDHandle ==INVALID_HANDLE_VALUE)) {
		U032	daError;
		DPF("Can't get Vxd handle   :-(");
		daError = GetLastError();
		DPF("The error was %d  or %x",daError,daError);
		if(daError == 50 ) {
			DPF("Which means most likely that you don't have a version of NVX.VXD or NVXmini2 which supports IOCTL");	
		}
		return FALSE;	
		
	} 
	
	// nother block
	{
		U032				inBuffer;
		NVVDDPROCTABLE	aTable;
		U032				outputSize = 0;
			
		// pass in the pointer to the direct draw object.. this may be useful later
//		inBuffer = (U032) lpInput->lpDD;
		
		aTable.getStoragePointer = NULL;
		aTable.dwPDDDriver = 0;
		aTable.handleBufferInterrupts = NULL;
		
		if(!DeviceIoControl(aVXDHandle,
							VDD_IOCTL_NV_FILLTABLE,		// control code of operation to perform
							&inBuffer,							// pointer to buffer to supply input data
							4,								// size of input buffer
							&aTable,					// pointer to buffer to receive output data
							sizeof(NVVDDPROCTABLE),	// size of output buffer
							&outputSize,				// pointer to variable to receive output byte count
							NULL 							// pointer to overlapped structure for asynchronous operation
		) ){
			// cool we made it.   Now copy our temporarly stuff into the real one!
			
			// first get the storage pointer
			if( aTable.getStoragePointer != NULL ) {
				pMySurfaces = (vpSurfaces*)(aTable.getStoragePointer)();
			} else {
				DPF("didn't get a valid pointer to a routine");
			
				CloseHandle(aVXDHandle);
				return FALSE;	
			}
			
			if( pMySurfaces != NULL ) {
				// we cool!  everybody get funky one more time!  and copy that stuff in
				pMySurfaces->myVXDhandle = aVXDHandle;
				memcpy (&pMySurfaces->myTable, &aTable,  sizeof (NVVDDPROCTABLE));
				
			} else {
				DPF("didn't get the storage pointer");
			
				CloseHandle(aVXDHandle);
				return FALSE;	
			}
			
		}	else {
			//	failure
			//close the VXD
			DPF("couldn't call the VXD via IOCTL properly");
			CloseHandle(aVXDHandle);
			return FALSE;	
		}
		
	}
	
	nvMyVPNotifiers = pMySurfaces->pTheNotifiers;
	pMySurfaces->numberOfSurfaces =0;

	for(i=0;i<MAXVPSURFACES;i++) {
		pMySurfaces->Surfaces[i] = NULL;
	}
	pMySurfaces->CurrentBufferSurface[4] = NOSURFACEPROGRAMED;				// to associate the HW buffer with an arrary index
			 
	pMySurfaces->CurrentScanMode =SEPERATEBUF;		// seperate or interleaved buffersx
	if(pMySurfaces->bInvertedFields ) 	{
		pMySurfaces->lastField = ODD;	// this should be even so we'll always start on odd ( hopefully... :-)
		pMySurfaces->lastVBIField = ODD;
	} else {
		pMySurfaces->lastField = EVEN;
		pMySurfaces->lastVBIField = EVEN;
	}	
	pMySurfaces->VBIstart = 0;;
	// general information
	pMySurfaces->bSkipNextField = FALSE;
	pMySurfaces->dwInHeight = 0;
	pMySurfaces->dwInWidth = 0;
	pMySurfaces->dwImageStartLine =10;				// ditch a minimum of 10 lines
	pMySurfaces->dwVBIStartLine = 0;
	pMySurfaces->dwVBIHeight = 0;
	pMySurfaces->dwPreHeight = 0;					// height and width before prescaling
	pMySurfaces->dwPreWidth = 0;
	pMySurfaces->dwOriginX = 0;						// orgins to move data within the surface
	pMySurfaces->dwOriginY = 0;
	pMySurfaces->bThreadDead = TRUE;
	pMySurfaces->bThreadRunning = FALSE;

	pMySurfaces->bStopVP = FALSE;							// indicates to the thread that it should NOT reprogram

	pMySurfaces->Timings.LoopCount=0;

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
	if(pDriverData == NULL ) {
		// dude this is PROBLEM!!!
		return FALSE;
	}
	pMySurfaces->pDriverData = pDriverData;		
		
	pMySurfaces->DXIRQCallbackSources = 0;;
	pMySurfaces->pDXIRQCallback = (U032) NULL;
	pMySurfaces->dwIrqContext = 0;
	pMySurfaces->SetupComplete = FALSE;
	pMySurfaces->dwNextEmpty = 0;
	pMySurfaces->dwLastEmpty = (MAXTRANSFERS-1);
	pMySurfaces->lastSurface = 999;
	pMySurfaces->lastBuffer = 999;
	return TRUE;
}

/*
 * CreateVideoPort32
 *
 * This function is optional, define all objects if they have not already been defined.
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
	DWORD		returnVal = DDHAL_DRIVER_HANDLED;
	// we should only have to get this once on creation
	
	if(pMySurfaces == NULL )
		if(!HookUpVxdStorage() )
		{
			lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
			return DDHAL_DRIVER_HANDLED;
		}
	if( pMySurfaces->pDriverData == NULL )
		if(!HookUpVxdStorage() )
		{
			lpInput->ddRVal = DDERR_CURRENTLYNOTAVAIL;
			return DDHAL_DRIVER_HANDLED;
		}
	
	
	if( pMySurfaces->SetupComplete ) {
			// we may be attempting a "second" CreateVideo port with the same ID..
			// let's check that.
			
			//CODE we should probably check here for non-duplicat VBI and Image, but we'll just let that slide for now.
			
			if (lpInput->lpDDVideoPortDesc->dwVideoPortID == 0 ) {
				// this is the second create
				DPF("***  HACKED second video port create has been called.");
				DPF("  now shunting to hyperspace modulator bypass code.  Turn reality distortion field driver code to 11");
	
				
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
	
				lpInput->ddRVal = DD_OK;
				return returnVal;
			}
	}
		
	if(lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags & DDVPCONNECT_INTERLACED ) {
		pMySurfaces->bInputInterLaced = TRUE;		
	} else {
		pMySurfaces->bInputInterLaced = FALSE;		
	}
	
	if(pMySurfaces->bInputInterLaced)
		pMySurfaces->CurrentCaptureMode =EVENODD;		// indicates which fields will be capture Even	
	else 
		pMySurfaces->CurrentCaptureMode =PROGRESSIVE;	// we're assuming progressive is ALWAYS even even even

		 
	 
	if(lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags & DDVPCONNECT_INVERTPOLARITY ) {
		DPF("DDVPCONNECT_INVERTPOLARITY		on");
		pMySurfaces->bInvertedFields = TRUE;
		// Now check for the weird exception case when we want to skip half line as well...
		if(lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags & DDVPCONNECT_HALFLINE ) {
			DPF("DDVPCONNECT_HALFLINE		on");
			pMySurfaces->bInvertedFields = FALSE;
		} else {
			DPF("DDVPCONNECT_HALFLINE		off");
		
		}
		
	} else {
		DPF("DDVPCONNECT_INVERTPOLARITY		off");
		pMySurfaces->bInvertedFields = FALSE;
	}
			 
	if(lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags & DDVPCONNECT_HALFLINE ) {
		DPF("DDVPCONNECT_HALFLINE		      on");
	} else {
		DPF("DDVPCONNECT_HALFLINE		      off");
	}
		

#ifdef  MULTI_MON
	nvTimerPtr = (U032 *) NvDeviceBaseGet(NV_DEV_TIMER, pDriverData->dwDeviceIDNum );
#else   /* MULTI_MON */
	nvTimerPtr = (U032 *) NvDeviceBaseGet(NV_DEV_TIMER );
#endif  /* MULTI_MON */
	pMySurfaces->pNvTimer = nvTimerPtr;

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

	// get our OWN copy of the driver data
		
	returnVal = SetupVP ( lpInput);
	

	//pMySurfaces->pVPChanPtr = (NvChannel * ) pDriverData->NvDevFlatVPE;


	// For the overlay flip, store away a channel pointer
#ifdef NV4
	pMySurfaces->pOverlayChanPtr = (NvChannel*) pDriverData->NvDevVideoFlatPio;
#else
	pMySurfaces->pOverlayChanPtr = (NvChannel*) pDriverData->NvDevFlatPio;
#endif	
	pMySurfaces->dwOverlaySubChan = NV_DD_SPARE;
	pMySurfaces->dwOverlayObjectID = NV_DD_YUV422_VIDEO_FROM_MEMORY;
	pMySurfaces->dwScalerObjectID = NV_DD_VIDEO_SCALER;
	pMySurfaces->dwDVDObjectID = NV_DD_DVD_SUBPICTURE;
	
#ifdef TESTCALLBACK

	// let's register an IMAGE callback here and see what happens
	{
		HANDLE bVXDHandle;
		U032	 inBuffer;
		//U032	 returnCode;
		U032	 outputSize;
		SETCALLBPROC	pSetCallback;
		
		inBuffer = 0;		
		
#ifdef NV4		
		bVXDHandle = CreateFile("\\\\.\\NV4", 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#else
		bVXDHandle = CreateFile("\\\\.\\NV3", 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#endif	// NV4		
		if(bVXDHandle != NULL ) {
			if(!DeviceIoControl(bVXDHandle,
										VDD_IOCTL_NV_GETSETCALLBACK,		// control code of operation to perform
										&inBuffer,							// pointer to buffer to supply input data
										4,										// size of input buffer
										&pSetCallback,						// pointer to buffer to receive output data
										4,										// size of output buffer
										&outputSize,						// pointer to variable to receive output byte count
										NULL 									// pointer to overlapped structure for asynchronous operation
									)
				 ){
						
						// things worked, the 
						inSCBStruct		inBuf;
						outSCBStruct	outBuf;
							// NOTE USE OF FLAGS FROM VPCALLBk.h
						inBuf.flags = IMAGESURFACE;
						inBuf.pTheCallback = JoeCoolDudesCallbackRoutine;
						
						(pSetCallback)(&inBuf,&outBuf);
						
			} else {
						
						// bummer, for some reason, we were unable to set your callback
			
			}
					
			CloseHandle(bVXDHandle);
									
		}
		
		
	}
#endif	

	// don't forget to decrement the number of video ports avail number which is 
	lpInput->ddRVal = DD_OK;
	
	return returnVal;
}


#ifdef TESTCALLBACK

void JoeCoolDudesCallbackRoutine(sCallBack* pCallbackStruct) {
	DPF("WoW we made it to JoeCoolDudesCallbackRoutine interrupt=%d",pCallbackStruct->intCount);
	
	
}


#endif
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

	
	if (lpInput->dwFlags & DDVPFORMAT_VIDEO ) {
		lpInput->dwNumFormats = 1;
		if (lpInput->lpddpfFormat != NULL) {
			memcpy (lpInput->lpddpfFormat, &pf[0],  sizeof (DDPIXELFORMAT));
		}
	}

/*
	if (lpInput->dwFlags & DDVPFORMAT_VBI ) {
		lpInput->dwNumFormats = 1;
		if (lpInput->lpddpfFormat != NULL) {
			memcpy (lpInput->lpddpfFormat, &pf[0],  sizeof (DDPIXELFORMAT));
		}
	}
*/
	if (lpInput->dwFlags & DDVPFORMAT_VBI ) {
		lpInput->dwNumFormats = 1;
		if (lpInput->lpddpfFormat != NULL) {
			memcpy (lpInput->lpddpfFormat, &pfvbi[0],  sizeof (DDPIXELFORMAT));
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
					memcpy (lpInput->lpddpfOutputFormats, pf, sizeof (DDPIXELFORMAT) );
		    	}
			}
		}
	}
	
	// for the VBI data
	if (lpInput->dwFlags & DDVPFORMAT_VBI) {// VBI formats
		// is input format a FOURCC
		if (lpInput->lpddpfInputFormat->dwFlags & DDPF_FOURCC ) {
			// is it UYVY
			if (lpInput->lpddpfInputFormat->dwFourCC == mmioFOURCC('R','A','W','8')) {
				// translation of UYVY is UYVY (ie no translation)
   	    	lpInput->dwNumFormats = 1;
	   	   if (lpInput->lpddpfOutputFormats != NULL) {
					memcpy (lpInput->lpddpfOutputFormats, pfvbi, sizeof (DDPIXELFORMAT) );
		    	}
			} 		
		}
	}
	
	lpInput->ddRVal = DD_OK;
   return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortField32
 *
 * This function is only required if readback of the current
 * field is supported.
 */
// needed defines for mediaport


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
	
	if (pMySurfaces->dwVideoPortStatus == VP_NOT_RUNNING) {
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
    /*
     * Make sure the video port is ON.  If not, set
     * lpInput->ddRVal to DDERR_VIDEONOTACTIVE and return.
     */

DWORD __stdcall GetVideoPortLine32 (LPDDHAL_GETVPORTLINEDATA lpInput)
{

	DPF("***GetVideoPortLine32");
 
 	// need channel poiner for this!
	if( GLOBDATAPTR->NvDevFlatVPE == (DWORD) NULL ) {
	 	lpInput->ddRVal = DDERR_SURFACELOST;
		return DDHAL_DRIVER_HANDLED; 
	}
	
	if (pMySurfaces->dwVideoPortStatus == VP_NOT_RUNNING) {
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
 * GetVideoPortConnectInfo
 *
 * This is the old:
 * Returns the GUIDs describing the connections that can be supported.
 * Think of a GUID simply as a 128 bit FOURCC that you don't have to
 * register.  This function is required.

 * And this is the new:
 * Fills in specified array of connections
 *
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

	nvTimerPtr = NULL;
	
	lpInput->ddRVal = DD_OK;
	
	CloseHandle(pMySurfaces->myVXDhandle);
	pMySurfaces->myVXDhandle = NULL;

	return DDHAL_DRIVER_HANDLED;
}


/*
 * GetVideoPortFlipStatus32
 *
 * This required function allows DDRAW to restrict access to a surface
 * until the physical flip has occurred, allowing doubled buffered capture.
 */

    /*
     * If the physical flip has not yet occurred, return
     * DDERR_WASSTILLDRAWING
     */

DWORD __stdcall GetVideoPortFlipStatus32 (LPDDHAL_GETVPORTFLIPSTATUSDATA lpInput)
{
	
	U032	dwIndex;
	
	//DPF("***GetVideoPortFlipStatus32");

#if 0
	if( GLOBDATAPTR->NvDevFlatVPE == NULL ) {
	 	lpInput->ddRVal = DDERR_SURFACELOST;
		return DDHAL_DRIVER_HANDLED; 
	}

#endif
	if( pMySurfaces->dwVideoPortStatus == VP_NOT_RUNNING ) {
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
			
			
			if( pMySurfaces->Surfaces[dwIndex]->bAutoFlip ) {
				// in the case of auto-flipping just always say we're done...
				lpInput->ddRVal = DD_OK;
			} else {
				if (pMySurfaces->bFlipRequested == TRUE ) {
					lpInput->ddRVal = DDERR_WASSTILLDRAWING;
				} else {
					lpInput->ddRVal = DD_OK;
				}
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


/*
 * FlipVideoPort32
 *
 * This function is required
 *
 * Issues:
 *			Correct use of notifiers
 *			Updating surface logging info
 *			What if new surface has not got same or less capabilites as replaced surface

 *			Does not work because class 66 doesn't allow insertion of buffers
 *
 */

/*
important:
			Curr surface should be known to us
			Targ surface could be new

			both surfaces should have this flag: DDSCAPS_LIVEVIDEO set in ->ddsCaps structure
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
        //	U032	counter;
	DPF("***FlipVideoPort32");
	if( GLOBDATAPTR->NvDevFlatVPE == (DWORD) NULL ) {
	 	lpInput->ddRVal = DDERR_SURFACELOST;
		return DDHAL_DRIVER_HANDLED; 
	}
	lpInput->ddRVal = DD_OK;
	
	oldSurfaceIndex = findSurface(lpInput->lpSurfCurr);
	if(oldSurfaceIndex  != SURFACENOTFOUND ) {
		newSurfaceIndex = findSurface(lpInput->lpSurfTarg);
		if(newSurfaceIndex == SURFACENOTFOUND ) {
			// in the case of non-interleaved surfaces this might be even more different
			// because we have essentially to replace TWO places where the old surface might be
			// residing...  not just one   non-auto flip SUCKS.  
			if( oldSurfaceIndex < pMySurfaces->VBIstart ) {
				replaceSurfaceWithSurface(oldSurfaceIndex,lpInput->lpSurfTarg, FALSE);
			} else {
				replaceSurfaceWithSurface(oldSurfaceIndex,lpInput->lpSurfTarg, TRUE);
			}
			newSurfaceIndex = oldSurfaceIndex;
		}
		pMySurfaces->bFlipRequested = TRUE;
		 
		if(newSurfaceIndex != SURFACENOTFOUND ) {
#if 0
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
						// we should hang out here until it's done....
						counter = 0;
						while((pNotify->status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS ) && (counter < 3000000)) {
							counter++;
						}
					}
					// check to see if we've had an error 
					//    there are LOTS of these....
					if ( (pNotify->status != NV04D_NOTIFICATION_STATUS_IN_PROGRESS ) |
						  (pNotify->status != NV04D_NOTIFICATION_STATUS_DONE_SUCCESS ) |
						  (pNotify->status != NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED ) ){
						DPF("   Attempting to flip out a surface which had an error during capture");
						//return DDHAL_DRIVER_NOTHANDLED;
					}
				} else {
					DPF("   Attempting to flip out a surface where the notifier was NULL");
					return DDHAL_DRIVER_NOTHANDLED;
				}
			}

#endif			
			// Ok, program up the switch
			//ProgramBufferWithSurface(dwBufferIndex,newSurfaceIndex,FALSE);
			
		
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

#if 0
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

#endif 

U032 startNV3DelayedStart(U032	bInterleave, VPSTATE*	pVideoPortStatus, U032 milliSecondsOfDelay ) {
	
	// ain't nothing happenin here yet.

	// First thing we're goin to do is paint that surface BLACK so as folks don't be noticin' we's gonna do the
	// delay factor for our masters up at microsoft.
	
	if(milliSecondsOfDelay > 5 )
	{
		U032	n = 2;
		U032 	q;
		
		if(pMySurfaces->VBIstart <2 )
			n = 1;
		if(pMySurfaces->VBIstart <1 )
			return 0;
		for(q=0;q<n;q++) {
			// length in Long words
			U032	length = ( pMySurfaces->Surfaces[q]->dwPitch * pMySurfaces->Surfaces[q]->dwHeight ) / 4;
			U032	startOff = pMySurfaces->Surfaces[q]->pVidMem ;
			U032	i;
			U032*	pOverlay = (U032*) ((U032)startOff);
			for(i=0; i< length ; i++ ) {
			    *pOverlay = 0x10801080;
				pOverlay++;
			}
		}
	}
	
// deficiencies in NV32.h
#ifndef NV4
#define NV004_NOTIFY_WRITE_ONLY                                    (0x00000000)
#define NV004_NOTIFY_WRITE_THEN_AWAKEN                             (0x00000001)
#define NV004_SET_ALARM_NOTIFY_WRITE_THEN_AWAKEN                   (0x00000001)
#endif
	
	nvMyVPNotifiers[NUM004OFFSET].status 		= NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED;
				
#ifndef NV4
	pMySurfaces->pVPChanPtr->subchannel[0].control.object = MY_TIMER_OBJECT;
#else 
	pMySurfaces->pVPChanPtr->subchannel[0].SetObject  = MY_TIMER_OBJECT;
#endif // NV4

// get a 0.75 second delay in nanoseconds
#define NV3DELAYTIME ( milliSecondsOfDelay*1000*1000 )
	{
		U032	low;
		U032	low1;
		U032	high;
		
		getCurrentNanoTimer(&high, &low);
		low1 = low;
		
		
		low = low + NV3DELAYTIME;
		if( low < low1 ) {
			high = high + 1;
		}
#ifndef NV4
		pMySurfaces->pVPChanPtr->subchannel[0].timer.SetAlarmTime.nanoseconds[0] = low;
		pMySurfaces->pVPChanPtr->subchannel[0].timer.SetAlarmTime.nanoseconds[1] = high;
		pMySurfaces->pVPChanPtr->subchannel[0].timer.SetAlarmNotify =
			NV004_SET_ALARM_NOTIFY_WRITE_THEN_AWAKEN;

#else   //NV4
		pMySurfaces->pVPChanPtr->subchannel[0].nv01Timer.SetAlarmTime.nanoseconds[0] = low;
		pMySurfaces->pVPChanPtr->subchannel[0].nv01Timer.SetAlarmTime.nanoseconds[1] = high;
		pMySurfaces->pVPChanPtr->subchannel[0].nv01Timer.SetAlarmNotify =
			NV004_SET_ALARM_NOTIFY_WRITE_THEN_AWAKEN;
#endif  //NV4			
		
		// ok, 3/4 second later we should get a notify!
	}
	
	//return startSurfaces(bInterleave, pVideoPortStatus );
	return DD_OK;

}


/*
 * UpdateVideoPort32
 *
 * This required function sets up the video port
 */

/* Questions:
 *			What if I want VBI latching while image is not latching ?
 */
 
DWORD __stdcall UpdateVideoPort32 (LPDDHAL_UPDATEVPORTDATA lpInput)
{

	VPERRORS myErrors;
	U032		bAutoflip;
#ifndef NV4
	U032		bDelayNV3;
#endif
	
	DPF("***UpdateVideoPort32");
	if( GLOBDATAPTR->NvDevFlatVPE == (DWORD) NULL ) {
	 	lpInput->ddRVal = DDERR_SURFACELOST;
		return DDHAL_DRIVER_HANDLED; 
	}

// Since bruce changed the order of initialization, we should check if we need to reload this
	if(pMySurfaces->pOverlayChanPtr == NULL) {
		DPF("Late overlay channel update!");

#ifdef NV4
		pMySurfaces->pOverlayChanPtr = (NvChannel*) pDriverData->NvDevVideoFlatPio;
#else
		pMySurfaces->pOverlayChanPtr = (NvChannel*) pDriverData->NvDevFlatPio;
#endif	

	}
	
	
	lpInput->ddRVal = DD_OK;
		
	{		
		/*
		 * Turn off the video port
		 */
//CODE
		DPF("   now Stopping the Video Port");
		// prevent future interrupts from doing anything
		pMySurfaces->bThreadRunning = FALSE;
		// tell the classes to stop
		StopVP();
		// zero out the notifiers
		
		{
			U032 i;
			for(i=0;i<NUM04DNOTIFIER;i++ ) 
				nvMyVPNotifiers[i+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED;
				nvMyVPNotifiers[i+NUM04DOFFSET].info32 = 0;
				nvMyVPNotifiers[i+NUM04DOFFSET].info16 = 0;
				nvMyVPNotifiers[i+NUM04DOFFSET].timeStamp.nanoseconds[0] = 0;
				nvMyVPNotifiers[i+NUM04DOFFSET].timeStamp.nanoseconds[1] = 0;
		}
		pMySurfaces->dwVideoPortStatus = VP_NOT_RUNNING;
    
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
			if( pMySurfaces->bInvertedFields ) {
				DPF("      field polarity               inverted");	
			} else {
				DPF("      field polarity               normal");	
			}
			
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
			
				
#define VBIVALIDDATALINE  (0x2)	

			
			if( lpInput->lplpDDVBISurface == NULL ) {
				// this indicates that there are NO VBI surfaces associated with this video port update.
				// therefore we should send all the line to the Image surface
				
				
				pMySurfaces->dwImageStartLine = 0;	// send all the data after line 0
				
				if ( lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP ) {
					if(lpInput->lpVideoInfo->rCrop.top > (signed long) pMySurfaces->dwImageStartLine ) {
						pMySurfaces->dwImageStartLine = lpInput->lpVideoInfo->rCrop.top;
					}
				}
				
			} else {
				// there are some VBI surfaces associated with this video port,
				// so we should send the data to the VBI surfaces where applicable
				pMySurfaces->dwVBIStartLine = VBIVALIDDATALINE;	// at least we're not starting with zero this first four are always
																			// empty as best as I know
			
				pMySurfaces->dwVBIHeight = lpInput->lpVideoInfo->dwVBIHeight;
			
				// Image start line should be one line after the the sum of these two
				pMySurfaces->dwImageStartLine = 1 + pMySurfaces->dwVBIHeight + pMySurfaces->dwVBIStartLine - VBIVALIDDATALINE;
				// if  	dwImageStartLine < 1  then things don't work to well
				if(pMySurfaces->dwImageStartLine < 1) pMySurfaces->dwImageStartLine = 1;
				
				if ( lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP ) {
					if(lpInput->lpVideoInfo->rCrop.top > (signed long) pMySurfaces->dwImageStartLine ) {
						pMySurfaces->dwImageStartLine = lpInput->lpVideoInfo->rCrop.top;
					}
				}
			}
						
			
			
			// this is the only way I know how to do the check for vertical cropping
			if ( lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP ) {
				pMySurfaces->dwInHeight = (DWORD)lpInput->lpVideoInfo->rCrop.bottom - lpInput->lpVideoInfo->rCrop.top;				
				pMySurfaces->dwInWidth  = lpInput->lpVideoInfo->rCrop.right - lpInput->lpVideoInfo->rCrop.left;		
				DPF("   UPDATE:         crop TOP %d",lpInput->lpVideoInfo->rCrop.top);						
			} else {
				pMySurfaces->dwInHeight = lpInput->lpVideoPort->ddvpDesc.dwFieldHeight;
				pMySurfaces->dwInWidth  = lpInput->lpVideoPort->ddvpDesc.dwFieldWidth;
			}
			
			// this is something of a hack I suppose
			pMySurfaces->dwInHeight = pMySurfaces->dwInHeight;			
						
			// if we're prescaling then do something
			if (DDVP_PRESCALE & lpInput->lpVideoInfo->dwVPFlags) {
				pMySurfaces->dwPreHeight = lpInput->lpVideoInfo->dwPrescaleHeight;				
				pMySurfaces->dwPreWidth  = lpInput->lpVideoInfo->dwPrescaleWidth;
			} else {
				pMySurfaces->dwPreHeight = pMySurfaces->dwInHeight;
				pMySurfaces->dwPreWidth = pMySurfaces->dwInWidth;
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
		DPF("   UPDATE: image in dimens %d,%d",pMySurfaces->dwInWidth,pMySurfaces->dwInHeight);
		DPF("   UPDATE: image pre dimens %d,%d",pMySurfaces->dwPreWidth,pMySurfaces->dwPreHeight);
		DPF("   UPDATE:   image position %d,%d",pMySurfaces->dwOriginX,pMySurfaces->dwOriginY);

		myErrors = replaceSurfaceList(lpInput->dwNumAutoflip,lpInput->dwNumVBIAutoflip,
										lpInput->lplpDDSurface, lpInput->lplpDDVBISurface,bAutoflip);
		
		pMySurfaces->dwVPFlags = lpInput->lpVideoInfo->dwVPFlags;
		if( myErrors == NOVPERROR ) {
#ifndef NV4
			if(bDelayNV3) {
				lpInput->ddRVal = startNV3DelayedStart(lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE, &(pMySurfaces->dwVideoPortStatus), 1000*3/4);
				
			
			} else {
				lpInput->ddRVal = startNV3DelayedStart(lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE, &(pMySurfaces->dwVideoPortStatus), 0);
				
				//lpInput->ddRVal = startSurfaces( lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE, &(pMySurfaces->dwVideoPortStatus));
			}
#else
			{
				lpInput->ddRVal = startNV3DelayedStart(lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE, &(pMySurfaces->dwVideoPortStatus), 0);
				
				//lpInput->ddRVal = startSurfaces( lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE, &(pMySurfaces->dwVideoPortStatus));
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

	// this is sort of a big hack since things aren't actually running yet, but DirectShow seems to require it
	pMySurfaces->dwVideoPortStatus = VP_BOTH_CAPTURING;
	
	return DDHAL_DRIVER_HANDLED;
}



/*
 * GetVideoSignalStatus32
 *
 * This function is required
 */
 
DWORD __stdcall GetVideoSignalStatus32 ( LPDDHAL_GETVPORTSIGNALDATA lpInput) {	
	
	U032	dwIndex;
	ASSERT(lpInput != NULL);

	DPF("***GetVideoSignalStatus32");
	if( GLOBDATAPTR->NvDevFlatVPE == (DWORD) NULL ) {
	 	lpInput->ddRVal = DDERR_SURFACELOST;
		return DDHAL_DRIVER_HANDLED; 
	}
	
	if( lpInput == NULL)
	{
		DPF("   Input NULL in GetVideoSignalStatus32");
		return DDHAL_DRIVER_NOTHANDLED;
	}
	// If the video port is not running, then we just don't know...
	if( pMySurfaces->dwVideoPortStatus == VP_NOT_RUNNING ) {
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

return DDHAL_DRIVER_HANDLED;
	
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
		// hopefully there's been _some_ event in video land in the last 1/24 of a second
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
					} else {
						diff = 0;
						// this is a place for chris's break point's while debugging
					}
					
				}
			}
		}

	}
	
	return DDHAL_DRIVER_HANDLED;
}


/*
 * WaitForVideoPortSync32
 *
 * This function is required
 */


DWORD __stdcall WaitForVideoPortSync32 (LPDDHAL_WAITFORVPORTSYNCDATA lpInput)
{
	DPF("***WaitForVideoPortSync32");
	
	if( GLOBDATAPTR->NvDevFlatVPE == (DWORD) NULL ) {
	 	lpInput->ddRVal = DDERR_SURFACELOST;
		return DDHAL_DRIVER_HANDLED; 
	}
	
	lpInput->ddRVal = DDVPSQ_NOSIGNAL;
	
	// if the port is running then
	if (pMySurfaces->dwVideoPortStatus != VP_NOT_RUNNING) {
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
