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
*	File: nvclient.c
*
*	Description:
*		This program renders random rectangles to a window, using the NV SW architecture.
*	This is the same as the Basic Rectangles project, except that a clip rectangle object is
*	allocated and attached to the rectangle render object, and the functions are rearranged
*	to fit into the windows programming model.
*
*		functions:
*
*			NvCreate():
*				- allocate a client
*				- allocate a device
*				- allocate a PIO channel
*				- allocate a DMA context for the frame buffer
*				- allocate and initialize a 2D rendering surface object
*				- allocate a clip rectangle object
*				- allocate and initialize the rendering objects
*
*			NvDrawRectangles()
*				- set the clip rectangle of the window
*				- draw rectangles by sending appropriate methods to the rectangle renderer
*
*			NvDestroy()
*				- free the client
*
**************************************************************************************************/

#include "stdafx.h"

// globals 
extern TCHAR szTitle[];
Nv04ChannelPio* channel;
ULONG hClient;
SIZE screen;

// creates the client when the window is created
NV_STATUS NvCreate()
{
    CHAR nameBuffer[32];
	PUCHAR pFrameBuffer;
	ULONG frameBufferSize;
    ULONG depth, pitch;

	// seed the random number generator for later use
	srand(time(NULL));

    // register this client with the architecture -- returns a client handle
    if (NvRmAllocRoot(&hClient) != NV_STATUS_SUCCESS) 
    {
        return NV_STATUS_ERROR_ALLOC_CLIENT;
    }

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
    if ((strcmp(nameBuffer, "NV4") != 0)) 
    { 
        return NV_STATUS_ERROR_DEV_TYPE;
    }

	// allocate a PIO channel to the device
    if (
    	NvRmAllocChannelPio(
    		hClient,                    // client handle
			OUR_DEVICE_ID,              // device handle
			OUR_CHANNEL_ID,				// new channel handle
			NV04_CHANNEL_PIO,			// new channel class
			NV01_NULL_OBJECT,			// new channel error context
			(PVOID*)&channel,			// returned channel pointer
			0x00000000					// flags
        ) != NV_STATUS_SUCCESS
	)
	{
        return NV_STATUS_ERROR_ALLOC_CH_PIO;
    }

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
			(PULONG)&screen.cx			// returned attribute value
	    ) != NV_STATUS_SUCCESS
	)
    { 
        return NV_STATUS_ERROR_CONFIG_GET;
    }
    if (
    	NvRmConfigGet(
			hClient,
			OUR_DEVICE_ID,
	    	NV_CFG_SCREEN_HEIGHT, 
			(PULONG)&screen.cy
	    ) != NV_STATUS_SUCCESS
	)
    { 
        return NV_STATUS_ERROR_CONFIG_GET;
    }
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

	// allocate and initialize a clip rectangle object
	if (
		NvRmAllocObject(
    		hClient,
			OUR_CHANNEL_ID,
			OUR_CLIP_RECTANGLE_ID,
			NV01_CONTEXT_CLIP_RECTANGLE
        ) != NV_STATUS_SUCCESS
	) 
	{
        return NV_STATUS_ERROR_ALLOC_OBJ;
    }
	WAIT_FOR_SUBCHANNEL(&NV_SUBCHANNEL(CLIP_RECT_SUBCH), 1 * 4);	
	NV_SUBCHANNEL(CLIP_RECT_SUBCH).SetObject = OUR_CLIP_RECTANGLE_ID;

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
	WAIT_FOR_SUBCHANNEL(&NV_SUBCHANNEL(RECT_RENDER_SUBCH), 4 * 4);	
	NV_SUBCHANNEL(RECT_RENDER_SUBCH).SetObject											= OUR_RECTANGLE_RENDERER_ID;
	NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.SetContextSurface			= OUR_2D_RENDERING_SURFACE_ID;
	NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.SetContextClipRectangle	= OUR_CLIP_RECTANGLE_ID;
	NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.SetColorFormat			= NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8;

    return NV_STATUS_SUCCESS;

} // end of NvCreate()

// renders many arbitrary rectangles to the window
NV_STATUS NvDrawRectangles(HWND hWnd)
{
	// set the current clip rectangle to be the same as the window's client
	NV_RECT rClip = NvGetClientNvRect(hWnd);
	WAIT_FOR_SUBCHANNEL(&NV_SUBCHANNEL(CLIP_RECT_SUBCH), 2 * 4);	
	NV_SUBCHANNEL(CLIP_RECT_SUBCH).Nv01ContextClipRectangle.SetPoint	= PACK_XY(rClip.org.x, rClip.org.y);
	NV_SUBCHANNEL(CLIP_RECT_SUBCH).Nv01ContextClipRectangle.SetSize		= PACK_WH(rClip.ext.cx, rClip.ext.cy);

	// render many arbitrary rectangles in the window
	ULONG r, g, b;
	NV_RECT rect;
	for (int i = 0; i < RECTANGLE_QTY; i++)
	{
		// get random rectangle parameters
		NvGetRandomRGB24Color(&r, &g, &b);
		rect.org = NvGetRandomPoint(screen);
		rect.ext = NvGetRandomExtent(screen);

		// send rendering methods thru the channel -- the size() method triggers the rendering
		WAIT_FOR_SUBCHANNEL(&NV_SUBCHANNEL(RECT_RENDER_SUBCH), 3 * 4);	
		NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.Color					= PACK_RGB24(r,g,b);
		NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.Rectangle[0].point	= PACK_XY(rect.org.x,rect.org.y);
		NV_SUBCHANNEL(RECT_RENDER_SUBCH).Nv04RenderSolidRectangle.Rectangle[0].size		= PACK_WH(rect.ext.cx,rect.ext.cy);
	}

    return NV_STATUS_SUCCESS;

} // end of NvDrawRectangles()

void NvDestroy()
{
	// return the client and all child object resources to the system
    NvRmFree(hClient, NV01_NULL_OBJECT, hClient);

} // end of NvDestroy()

// returns an NV rectangle that describes the screen-relative window's client
NV_RECT NvGetClientNvRect(HWND hWnd)
{
	RECT rClient;
	GetClientRect(hWnd, &rClient);
	NvClientToScreenRect(hWnd, &rClient);
	NV_RECT nvrClient;
	nvrClient.org.x = rClient.left;
	nvrClient.org.y = rClient.top;
	nvrClient.ext.cx = rClient.right - rClient.left;
	nvrClient.ext.cy = rClient.bottom - rClient.top;
	return nvrClient;

} // end of NvGetClientNvRect()

// converts a client-relative rectangle to a screen-relative rectangle 
void NvClientToScreenRect(HWND hWnd, RECT* pRect)
{
	POINT ptUpperLeft, ptLowerRight;

	// convert into points
	ptUpperLeft.x	= pRect->left;
	ptUpperLeft.y	= pRect->top;
	ptLowerRight.x	= pRect->right;
	ptLowerRight.y	= pRect->bottom;

	// call windows API to convert from client coords to screen coords
	ClientToScreen(hWnd, &ptUpperLeft);
	ClientToScreen(hWnd, &ptLowerRight);

	// recreate the transformed rectangle
	pRect->left		= ptUpperLeft.x;
	pRect->top		= ptUpperLeft.y;
	pRect->right	= ptLowerRight.x;
	pRect->bottom	= ptLowerRight.y;

} // end of NvClientToScreenRect()

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

// displays an error message box based on its error code
void NvReportError(ULONG errorCode)
{
	char errorText[80];

	switch(errorCode)
	{
		case NV_STATUS_ERROR:
			strcpy(errorText, "Error: general error");
			break;
		case NV_STATUS_ERROR_ALLOC_DEV:
			strcpy(errorText, "Error: cannot allocate device");
			break;
		case NV_STATUS_ERROR_DEV_TYPE:
			strcpy(errorText, "Error: unavailable requested NV device type");
			break;
		case NV_STATUS_ERROR_ALLOC_CTX_DMA:
			strcpy(errorText, "Error: cannot allocate DMA context");
			break;
		case NV_STATUS_ERROR_ALLOC_CH_PIO:
			strcpy(errorText, "Error: cannot allocate PIO channel");
			break;
		case NV_STATUS_ERROR_ALLOC_CH_DMA:
			strcpy(errorText, "Error: cannot allocate DMA channel");
			break;
		case NV_STATUS_ERROR_ALLOC_MEM:
			strcpy(errorText, "Error: cannot allocate memory");
			break;
		case NV_STATUS_ERROR_ALLOC_OBJ:
			strcpy(errorText, "Error: cannot allocate anobject");
			break;
		case NV_STATUS_ERROR_CONFIG_GET:
			strcpy(errorText, "Error: cannot allocate get specified configuration");
			break;
	}
	MessageBox(NULL, errorText, szTitle, MB_OK);

} // end of NvReportError()


// end of nvclient.cpp
