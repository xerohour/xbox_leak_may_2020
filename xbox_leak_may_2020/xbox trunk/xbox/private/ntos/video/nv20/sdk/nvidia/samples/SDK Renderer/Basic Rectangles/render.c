 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
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

/*************************************************************************************************
*
*	File: render.c
*
*	Description:
*		This program blasts random rectangles to the frame buffer, using the NV SW architecture.
*
*	Basic NV SW architecture procedure:
*		- register the client
*		- allocate a device
*		- allocate a PIO channel
*		- allocate a DMA context for the frame buffer
*		- allocate and initialize a 2D rendering surface object
*		- allocate and initialize a rectangle rendering object
*		- draw rectangles to the screen
*
**************************************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

// NV SDK include files
#include <nvos.h>
#include <nv32.h>
#include <nvrmapi.h>
#include <nvMacros.h>

// NV DDK include files
#include "nvcm.h"

#include "render.h"

int main()
{
	NV_STATUS status;
    ULONG hClient;
	Nv04ChannelPio* pChannel;
    SIZE sScreen;

    // register this client with the architecture
    if (NvRmAllocRoot(&hClient) != NV_STATUS_SUCCESS) 
    {
		return 1;
    }

	// initialize the client and draw rectangles
	if ((status = NvInitialize(hClient, "NV4", &pChannel, &sScreen)) == NV_STATUS_SUCCESS)
	{
		// draw rectangles thru the channel, to the screen
		status = NvDrawRectangles(pChannel, sScreen);
	}
	if (status != NV_STATUS_SUCCESS)
	{
		NvReportError(status);
	}

	// free up the client -- this frees all child objects
    NvRmFree(hClient, NV01_NULL_OBJECT, hClient);

	return 0;

} // end of main()

NV_STATUS NvInitialize(ULONG hClient, PUCHAR szDeviceType, Nv04ChannelPio** pChannel, SIZE* psScreen)
{
    CHAR nameBuffer[32];
	Nv04ChannelPio* channel;
	PUCHAR pFrameBuffer;
	ULONG frameBufferSize;
	SIZE screen;
    ULONG depth, pitch;

	// seed the random number generator for later use
	srand(time(NULL));

    // allocate the specified type of device
    if (
    	NvRmAllocDevice(
    		hClient,					// client handle
			OUR_DEVICE_ID,              // device handle
			NV01_DEVICE_0,              // device class
			(PUCHAR)nameBuffer          // returned device type
        ) != NV_STATUS_SUCCESS
    ) 
	{
        return NV_STATUS_ERROR_ALLOC_DEV;
    }
    if ((strcmp(nameBuffer, szDeviceType) != 0)) 
    { 
        return NV_STATUS_ERROR_DEV_TYPE;
    }

	// allocate a PIO channel to the device
    if (
    	NvRmAllocChannelPio(
    		hClient,					// client handle
			OUR_DEVICE_ID,              // device handle
			OUR_CHANNEL_ID,				// channel handle
			NV04_CHANNEL_PIO,			// channel class
			NV01_NULL_OBJECT,			// error context
			(PVOID*)pChannel,			// returned channel pointer
			0x00000000					// flags
        ) != NV_STATUS_SUCCESS
    ) 
	{
        return NV_STATUS_ERROR_ALLOC_CH_PIO;
    }
	channel = *pChannel;

	// create a DMA context that contains the frame buffer
	pFrameBuffer = NULL;
    if (
    	NvRmAllocMemory(
    		hClient,					// client handle
			OUR_DEVICE_ID,              // device handle
			OUR_FRAME_BUFFER_ID,		// memory handle
			NV01_MEMORY_LOCAL_USER,		// memory class
			0x00000000,					// flags
			(PVOID*)&pFrameBuffer,		// returned frame buffer pointer
			&frameBufferSize			// limit (size - 1) -- returns the frame buffer size
        ) != NV_STATUS_SUCCESS
    ) 
	{
        return NV_STATUS_ERROR_ALLOC_MEM;
    }
    if (
    	NvRmAllocContextDma(
    		hClient,					// client handle
			OUR_FRAME_BUFFER_CTX_ID,	// DMA context handle
			NV01_CONTEXT_DMA,			// DMA context class
			NVOS03_FLAGS_ACCESS_READ_WRITE,	// flags
			pFrameBuffer,				// pointer to DMA buffer
			frameBufferSize - 1			// limit (size - 1) of DMA buffer
        ) != NV_STATUS_SUCCESS
    ) 
	{
        return NV_STATUS_ERROR_ALLOC_CTX_DMA;
    }

	// allocate a 2D rendering surface
    if (
    	NvRmConfigGet(
			hClient,					// client handle
			OUR_DEVICE_ID,				// device handle
	    	NV_CFG_SCREEN_WIDTH,		// configuration attribute
			(PULONG)&psScreen->cx		// returned attribute value
	    ) != NV_STATUS_SUCCESS
	)
    { 
        return NV_STATUS_ERROR_CONFIG_GET;
    }
	screen.cx = psScreen->cx;
    if (
    	NvRmConfigGet(
			hClient,
			OUR_DEVICE_ID,
	    	NV_CFG_SCREEN_HEIGHT, 
			(PULONG)&psScreen->cy
	    ) != NV_STATUS_SUCCESS
	)
    { 
        return NV_STATUS_ERROR_CONFIG_GET;
    }
	screen.cy = psScreen->cy;
    if (
    	NvRmConfigGet(
			hClient,
			OUR_DEVICE_ID,
	    	NV_CFG_PIXEL_DEPTH, 
			&depth
	    ) != NV_STATUS_SUCCESS
	)
    { 
        return NV_STATUS_ERROR_CONFIG_GET;
    }
	if (
    	NvRmAllocObject(
    		hClient,					// client handle
			OUR_CHANNEL_ID,				// channel handle
			OUR_2D_RENDERING_SURFACE_ID,// object handle
			NV04_CONTEXT_SURFACES_2D	// object class
        ) != NV_STATUS_SUCCESS
    ) 
	{
        return NV_STATUS_ERROR_ALLOC_OBJ;
    }
	pitch = screen.cx * (depth / 8);
	WAIT_FOR_SUBCHANNEL(&NV_SUBCHANNEL(WORKING_SUBCH), 5 * 4);	
	NV_SUBCHANNEL(WORKING_SUBCH).SetObject										= OUR_2D_RENDERING_SURFACE_ID;
	NV_SUBCHANNEL(WORKING_SUBCH).Nv04ContextSurfaces2d.SetContextDmaImageSource	= OUR_FRAME_BUFFER_CTX_ID;
	NV_SUBCHANNEL(WORKING_SUBCH).Nv04ContextSurfaces2d.SetContextDmaImageDestin	= OUR_FRAME_BUFFER_CTX_ID;
	NV_SUBCHANNEL(WORKING_SUBCH).Nv04ContextSurfaces2d.SetColorFormat			= NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
	NV_SUBCHANNEL(WORKING_SUBCH).Nv04ContextSurfaces2d.SetPitch					= PACK_PITCH(pitch,pitch);

	// allocate a rectangle rendering object
    if (
    	NvRmAllocObject(
    		hClient,
			OUR_CHANNEL_ID,
			OUR_RECTANGLE_RENDERER_ID,
			NV04_RENDER_SOLID_RECTANGLE
        ) != NV_STATUS_SUCCESS
    ) 
	{
        return NV_STATUS_ERROR_ALLOC_OBJ;
    }
	WAIT_FOR_SUBCHANNEL(&NV_SUBCHANNEL(RECT_RENDER_SUBCH), 3 * 4);	
	NV_SUBCHANNEL(RECT_RENDER_SUBCH).SetObject										= OUR_RECTANGLE_RENDERER_ID;
	NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.SetContextSurface		= OUR_2D_RENDERING_SURFACE_ID;
	NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.SetColorFormat		= NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;

	return NV_STATUS_SUCCESS;

} // end of NvInitialize()

// blasts rectangles to the entire screen
NV_STATUS NvDrawRectangles(Nv04ChannelPio* channel, SIZE sScreen)
{
	ULONG i, r, g, b;
	POINT org;
	SIZE ext;

	// render many arbitrary rectangles in the window
	for (i = 0; i < RECTANGLE_QTY; i++)
	{
		// get random rectangle parameters
		NvGetRandomRGB24Color(&r, &g, &b);
		org = NvGetRandomPoint(sScreen);
		ext = NvGetRandomExtent(sScreen);

		// send rendering methods thru the channel -- the size() method triggers the rendering
		WAIT_FOR_SUBCHANNEL(&NV_SUBCHANNEL(RECT_RENDER_SUBCH), 3 * 4);	
		NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.Color					= PACK_RGB24(r,g,b);
		NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.Rectangle[0].point	= PACK_XY(org.x,org.y);
		NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.Rectangle[0].size		= PACK_WH(ext.cx,ext.cy);
	}

	return NV_STATUS_SUCCESS;

} // end of NvDrawRectangles()

// returns random RGB values that define a color
void NvGetRandomRGB24Color(PULONG pr, PULONG pg, PULONG pb)
{
	*pr = rand() % MAX_8BIT_COLOR + 1;
	*pg = rand() % MAX_8BIT_COLOR + 1;
	*pb = rand() % MAX_8BIT_COLOR + 1;

} // end of NvGetRandomRGB24Color()

// returns a random point within the given limits
POINT NvGetRandomPoint(SIZE limit)
{
	POINT pt;
	pt.x = rand() % limit.cx;
	pt.y = rand() % limit.cy;
	return pt;

} // end of NvGetRandomPoint()

// returns a random size of a rectangle within the given limits
SIZE NvGetRandomExtent(SIZE limit)
{
	SIZE s;
	s.cx = rand() % limit.cx + 1;
	s.cy = rand() % limit.cy + 1;
	return s;

} // end of NvGetRandomExtent()

// displays an error string based on its error code
void NvReportError(ULONG errorCode)
{
	char errorText[80];

	switch(errorCode)
	{
		case NV_STATUS_ERROR:
			strcpy(errorText, "general error");
			break;
		case NV_STATUS_ERROR_ALLOC_DEV:
			strcpy(errorText, "cannot allocate device");
			break;
		case NV_STATUS_ERROR_DEV_TYPE:
			strcpy(errorText, "bad device type");
			break;
		case NV_STATUS_ERROR_ALLOC_CTX_DMA:
			strcpy(errorText, "cannot allocate DMA context");
			break;
		case NV_STATUS_ERROR_ALLOC_CH_PIO:
			strcpy(errorText, "cannot allocate PIO channel");
			break;
		case NV_STATUS_ERROR_ALLOC_CH_DMA:
			strcpy(errorText, "cannot allocate DMA channel");
			break;
		case NV_STATUS_ERROR_ALLOC_MEM:
			strcpy(errorText, "cannot allocate memory");
			break;
		case NV_STATUS_ERROR_ALLOC_OBJ:
			strcpy(errorText, "cannot allocate anobject");
			break;
		case NV_STATUS_ERROR_CONFIG_GET:
			strcpy(errorText, "cannot allocate get specified configuration");
			break;
	}
	printf("render: error: %s\n\n", errorText);

} // end of NvReportError()


// end of render.c
