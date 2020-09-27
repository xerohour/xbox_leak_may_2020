/***************************************************************************\
* Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: VidTex.h                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           11/16/98                                          *
*                                                                           *
\***************************************************************************/

#ifndef VidTex_h
#define VidTex_h

#ifndef U032
#define U032  unsigned long
#endif //U032

typedef struct _VidTexSurf {
	// this first record is probably not necessary
//	U032		frameBufferOffsetConv; // input	// bytes from base of FB to begin of color space converted image
	U032		frameBufferOffsetSwiz; // input	// bytes from base of FB to begin of swized image
	U032		width;				// input		//pixels			should be power of 2
	U032		height;				// input		//pixels			should be power of 2
	U032		pitch;				// input		// bytes			should be width * (bytes/pixel)
	U032		format;				// input		//Same as NV052_SET_FORMAT_COLOR_LE_XXXXXX  color
	U032		dwFrameCount;		// output	// is incremented each frame, and will simply wrap
	U032		bUpdateSystem;		// N/A/     this should be set to true each time you update the input records
										//          the VXD will set it back to false after updating context DMA etc
	U032		bContextsAllocated;	// N/A  used by VXD to keep track of contexts
} VidTexSurf;

void fillVidTexSurface(DWORD surface );
void initTextureSurf(void);
void DestroyTextureSurf(void);

#define VDD_IOCTL_NV_SETVIDTEXSURF 		0x20000669


/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


// Here is some real basic example code

#ifdef JUST_EXAMPLE_CODE
	{
		HANDLE bVXDHandle;
		
		// These should _NOT NOT NOT_ be declared on the stack.. this is just _example_ code
		VidTexSurf MyVidTexureSurface;
		U032	 		outBuffer;
		U032			outputSize;
		
		VidTexSurf*	pVidTextureSurface = &MyVidTexureSurface;
		
		
		
		bVXDHandle = CreateFile("\\\\.\\NV4", 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

		if((bVXDHandle != NULL ) && (bVXDHandle != INVALID_HANDLE_VALUE ) ){
			if(!DeviceIoControl(bVXDHandle,
										VDD_IOCTL_NV_SETVIDTEXSURF,		// control code of operation to perform
										&pVidTextureSurface,							// pointer to buffer to supply input data
										4,										// size of input buffer
										&outBuffer,						// pointer to buffer to receive output data
										4,										// size of output buffer
										&outputSize,						// pointer to variable to receive output byte count
										NULL 									// pointer to overlapped structure for asynchronous operation
									)
				 ){					
					// things worked, the 
					if(outBuffer != 0 ) {
						// error check outBuffer error code
					
					}						
			} else {
							// error check outBuffer error code
			}
					
			CloseHandle(bVXDHandle);
									
		}
		
		
#endif  //JUST_EXAMPLE_CODE	
	
	


#endif //VidTex_h