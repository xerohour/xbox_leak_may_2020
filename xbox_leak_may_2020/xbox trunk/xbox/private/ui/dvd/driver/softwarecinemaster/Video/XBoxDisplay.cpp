////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define INITGUID 1

#include "XBoxDisplay.h"
#include "library\common\vddebug.h"
#include "..\common\PerformanceMonitor.h"
#include "YUVFrameDebug.h"
#include "library\hardware\video\generic\videnc.h"
#include "av.h"

#define DUMP_BITMAP_DATA    0

#define MEASURE_FLIP_TIME               0
#define MEASURE_LOCK_TIME               0
#define MEASURE_UNLOCK_TIME				0
#define MEASURE_DEINTERLACE_TIME        0
#define FRAME_ANNOTATION                0
#define DUMP_FLIP_TIMES							0

#define SUPPORT_PANSCAN_LETTERBOX		1

#pragma warning(disable : 4799)

#define PUSHER_WORKAROUND	1


#ifdef FRAME_ANNOTATION
int		displayedEvenFrames;
int		displayedOddFrames;
#endif


//
// We've turned off inverse 3:2 pulldown interpolation
// 
// Uncomment the line below to turn it back on
//

// #define INVERSE_3_2_PULLDOWN__INTERPOLATION		1


extern "C"
	{
	extern volatile DWORD *D3D__GpuReg;
	}


#ifdef USE_TEXTURE_SURFACES
static LPDIRECT3DVERTEXBUFFER8 g_pVB  = NULL;
DWORD								pixelShader;

DWORD psFunctionTokens[] = {  0x18111812,
										0x5d511c30,
										0x18111010,
										0x5d511c30,
										0x1c141010,
										0,	0,	0,	0,
										0,	0, 2,	3,
										4,	0,	0,	0,
										0, 1,	0,	0,
										0, 0, 0, 0,
										0, 0xcd, 0xc00,
										0xd0, 0xc00, 0xc0, 0,
										0, 0,
										0x08010802,
										0x4d410c20,
										0x08010000,
										0x4d410c20,
										0x0c040000,
										0, 0, 0, 0,
										0, 0, 0x30cd, 0xc00,
										0x20d0, 0xc00, 0xc0, 0,
										0, 0,
										0x011105, 1,
										0, 0,
										0xffff4320,
										0xfffffff1,
										0xff };

#endif

inline DWORD CalcPitch(DWORD width)
{
    return (width * 2 + D3DTEXTURE_PITCH_ALIGNMENT - 1) &
            ~(D3DTEXTURE_PITCH_ALIGNMENT - 1);
}

//
//  Set Macrovision Level
//

void XBoxDisplay::SetMacrovisionLevel(int level)
    {
    bool newHide;

    if (macrovisionEncoder)
        {
        if (!macrovisionEncoder->SetMacrovisionLevel(level))
            {
            lock.Enter();

            newHide = level != 0;
            if (newHide != macrovisionHideDisplay)
                {
                if (newHide && errorMessenger) errorMessenger->PostErrorMessage(GNR_MACROVISION_NOT_SUPPORTED);

                macrovisionHideDisplay = newHide;
                }

            lock.Leave();
            }
        else
            {
            if (macrovisionHideDisplay)
                {
                lock.Enter();

                macrovisionHideDisplay = FALSE;

                lock.Leave();
                }
            }
        }
    }

void XBoxDisplay::SetCGMSMode(VideoCopyMode eCGMSMode)
    {    

    if(this->eCGMSMode != eCGMSMode)
        {
        this->eCGMSMode = eCGMSMode;
        
        ULONG ulOption = AV_CGMS_UNRESTRICTED;
        switch(eCGMSMode)
            {
	        case VCPMD_ONE_COPY_PERMITTED:
	            ulOption = AV_CGMS_ONECOPY;
	            break;
	            
	        case VCPMD_NO_COPYING_PERMITTED:
	            ulOption = AV_CGMS_NOCOPIES;
	            break;

        	case VCPMD_COPYING_PERMITTED:
        	default:
        	    ulOption = AV_CGMS_UNRESTRICTED;
        	    break;
            }

        ULONG Result;
    	AvSendTVEncoderOption((void *)D3D__GpuReg, AV_OPTION_CGMS, ulOption, &Result);

        }
    }


//
//  Constructor
//

XBoxDisplay::XBoxDisplay(HINSTANCE hinst, GenericProfile * profile, GenericProfile * globalProfile)
    {
    lpDDSOverlay = NULL;
    decodeWidth = 0;
    decodeHeight = 0;
	sourceLeft = sourceTop = sourceWidth = sourceHeight = 0;
	destWidth = destHeight = destLeft = destTop = 0;
	outputWidth = outputHeight = outputLeft = outputTop = 0;
	sourceAdapt = TRUE;
	destAdapt = TRUE;

    currentDisplayMode = PS_FRAME_PICTURE;

    macrovisionHideDisplay = FALSE;

	enforceFlipSync = FALSE;

	downscaleFilter.fullX = (VideoDownscaleFilterLevel)4;
	downscaleFilter.fullY = (VideoDownscaleFilterLevel)4;
	downscaleFilter.halfX = (VideoDownscaleFilterLevel)4;
	downscaleFilter.halfY = (VideoDownscaleFilterLevel)4;

    currentDisplayBuffer = NULL;
	 previousDisplayBuffer = NULL;
    displayFreezeMode = false;
	overlayOpen = false;

#if FRAME_ANNOTATION
	displayedEvenFrames = 0;
	displayedOddFrames = 0;
#endif

	overlayAvailable = FALSE;

    ULONG Result;

	// ffh - turn off the flicker
	AvSendTVEncoderOption((void *)D3D__GpuReg, AV_OPTION_FLICKER_FILTER, 0, &Result);

    }

//
//  Destructor
//

XBoxDisplay::~XBoxDisplay(void)
    {
    FreeDisplayBuffers();
    }

//
//  Set Color Key
//

void XBoxDisplay::SetColorKeyColor(DWORD color)
    {
	colorKeyColor = color;
    }


//
//  Enable Display
//

void XBoxDisplay::EnableDisplay(bool enable)
    {
    }

//
//  Update Display
//

void XBoxDisplay::UpdateDisplay(void)
    {
    }



#ifdef USE_TEXTURE_SURFACES

struct CUSTOMVERTEX
{
    D3DXVECTOR4 p; // The transformed position for the vertex
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)

#endif



Error XBoxDisplay::OpenOverlay(void)
    {
    lock.Enter();

#ifdef USE_TEXTURE_SURFACES

    // First time around, create a vertex buffer
    if( g_pVB == NULL )
    {
		DWORD dwWidth, dwHeight;
		DWORD dwSourceWidth, dwSourceHeight;

		dwWidth = 640;
		dwHeight = 480;

		dwSourceWidth = decodeWidth;
		dwSourceHeight = decodeHeight;


	    D3DDevice_CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX), 0,
                                      D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB);

		CUSTOMVERTEX* pVertices;
		g_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

		pVertices[0].p = D3DXVECTOR4( 0.0f, 0.0f, 0.5f, 1.0f );
		pVertices[0].tu = 0.0f;
		pVertices[0].tv = 0.0f;

		pVertices[1].p = D3DXVECTOR4( (float) dwWidth, 0.0f, 0.5f, 1.0f );
		pVertices[1].tu = (float) dwSourceWidth;
		pVertices[1].tv = 0.0f;

		pVertices[2].p = D3DXVECTOR4( 0.0f, (float) dwHeight, 0.5f, 1.0f );
		pVertices[2].tu = 0.0f;
		pVertices[2].tv = (float) dwSourceHeight;

		pVertices[3].p = D3DXVECTOR4( (float) dwWidth, (float) dwHeight, 0.5f, 1.0f );
		pVertices[3].tu = (float) dwSourceWidth;
		pVertices[3].tv = (float) dwSourceHeight;

		g_pVB->Unlock();

    }


    D3DDevice_CreatePixelShader( (D3DPIXELSHADERDEF *)&psFunctionTokens, &pixelShader ) ;

#else

	 DP("Enable overlay");

	 //ffh - workaround for enable overlay bug - cannot be enabled more than once
	 if (!overlayOpen)
		 {
		 D3DDevice_EnableOverlay(TRUE);
		 overlayOpen = true;
		 }
#endif

    lock.Leave();

    GNRAISE_OK;
    }


//
//  Close Overlay
//

void XBoxDisplay::CloseOverlay(void)
    {
    lock.Enter();

	 //D3DDevice_EnableOverlay(FALSE);

	overlayAvailable = FALSE;

    lock.Leave();
    }


void DumpFlipCnt(void);

//
//  Init Display
//

void XBoxDisplay::InitDisplay(int width, int height, int minStretch)
    {
    lock.Enter();

#if DUMP_FLIP_TIMES
	DumpFlipCnt();
#endif

	if (decodeWidth != width || decodeHeight != height)
		{
		FreeDisplayBuffers();
		}


#if FRAME_ANNOTATION
	displayedEvenFrames = 0;
	displayedOddFrames = 0;
#endif

    decodeWidth = width;
    decodeHeight = height;


	if ((sourceWidth == 0) || (sourceHeight == 0))
		{
		sourceLeft = sourceTop = 0;
		sourceWidth = decodeWidth;
		sourceHeight = decodeHeight;
		}


    AllocateDisplayBuffers();
    ClearDisplayBuffers();

	OpenOverlay();

    GenericPictureDisplay::InitDisplay(width, height, minStretch);

	FlipOverlay(currentDisplayBuffer->lpDDSurface, 0);

	overlayAvailable = TRUE;

    lock.Leave();
    }

//
//  Free Display Buffers
//

void XBoxDisplay::FreeDisplayBuffers(void)
    {
    int i;

    lock.Enter();

    PMMSG("FreeDisplayBuffers\n");

    display = NULL;
    nextAvail = NULL;
    currentDisplayBuffer = NULL;
	 previousDisplayBuffer = NULL;
	 sourceWidth = 0;
	 sourceHeight = 0;

    PMMSGX("NumDisplayBuffers %d\n", numDisplayBuffers);

    for(i=0; i<numDisplayBuffers; i++)
        {
			if (displayBuffers[i])
				{
			   ((XBoxDisplayBuffer *)(displayBuffers[i]))->lpSurface1->Release();
			   ((XBoxDisplayBuffer *)(displayBuffers[i]))->lpSurface2->Release();

				delete displayBuffers[i];
				displayBuffers[i] = NULL;
				}
        }

    numDisplayBuffers = 0;
    currentDisplayMode = PS_FRAME_PICTURE;

	CloseOverlay();
    lock.Leave();
    }





void XBoxDisplay::ClearSurface(LPDIRECTDRAWSURFACE surface)
    {
	D3DLOCKED_RECT xLock;
    HRESULT ddrval;
	D3DSURFACE_DESC desc;

    if (surface)
        {

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
		surface->GetLevelDesc(0, &desc);
        ddrval = surface->LockRect(0, &xLock, NULL, NULL);
#else
		surface->GetDesc(&desc);
        ddrval = surface->LockRect(&xLock, NULL, NULL);
#endif

#else
		surface->GetDesc(&desc);

		if (surface->Common & D3DCOMMON_VIDEOMEMORY)
			xLock.pBits = (void *)(surface->Data | 0xF0000000);
		else
			xLock.pBits = (void *)(surface->Data | 0x80000000);

		xLock.Pitch = CalcPitch(decodeWidth);
		ddrval = DD_OK;

#endif

        if (ddrval == DD_OK)
            {
            DWORD p = (DWORD)xLock.pBits;
            DWORD n, c;

            c = 0x80008000;
            n = desc.Height * xLock.Pitch / 4;

            __asm
                {
                mov edi, [p]
                mov ecx, [n]
                mov eax, [c]
                rep stosd
                }

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
            surface->UnlockRect(0);
#else
            surface->UnlockRect();
#endif

#endif
            }
        }
    }



void XBoxDisplay::AllocateDisplayBuffers(void)
    {

	LPDIRECTDRAWSURFACE surfaces[12];
//	LPDIRECTDRAWSURFACE surfaces[10];

	if (!numDisplayBuffers)
		{
		int i;

		for(i=0; i<12; i++)
//		for(i=0; i<10; i++)
			{
#ifdef USE_TEXTURE_SURFACES
			D3DDevice_CreateTexture((decodeWidth + 15) & ~15, decodeHeight+1, 1, 0, D3DFMT_UYVY, 0, &surface[i]);
#else
			D3DDevice_CreateImageSurface((decodeWidth + 15) & ~15, decodeHeight+1, D3DFMT_YUY2, &(surfaces[i]));
#endif
			ClearSurface(surfaces[i]);
			}

		for(i=0; i<6; i++)
//		for(i=0; i<5; i++)
			{
			displayBuffers[numDisplayBuffers] = new XBoxDisplayBuffer(this, surfaces[2*i], surfaces[2*i+1], decodeWidth, decodeHeight);
			numDisplayBuffers++;
			}


		lpDDSecondary = NULL;
		lpDDSOverlay = surfaces[0];

		currentDisplayBuffer = (XBoxDisplayBuffer *)displayBuffers[0];
		previousDisplayBuffer = NULL;
		currentDisplayMode = PS_TOP_FIELD;

		}

    }




DWORD XBoxDisplay::GetDeinterlaceFlags(void)
    {
    return DEIF_DEINTERLACE_WEAVE |
            DEIF_DEINTERLACE_BOB |
            DEIF_CAN_CHANGE_DEINTERLACE_DECODE;
    }

bool XBoxDisplay::FlipOverlay(LPDIRECTDRAWSURFACE to, DWORD flags)
    {
    DWORD ddres;






	 RECT rcSrc, rcDest;

	 rcSrc.top = sourceTop;
	 rcSrc.left = sourceLeft;
	 rcSrc.bottom = sourceTop + sourceHeight;
	 rcSrc.right = sourceLeft + sourceWidth;

	 rcDest.top = destTop;
	 rcDest.left = destLeft;
	 rcDest.bottom = destTop + destHeight;
	 rcDest.right = destLeft + destWidth;


#ifdef USE_TEXTURE_SURFACES
	 RenderFrame(to);

#else

    D3DDevice_UpdateOverlay(to, &rcSrc, &rcDest, true, colorKeyColor);
	ddres = DD_OK;

#endif

	lpDDSOverlay = to;

	return ddres == DD_OK;

    }



#ifdef USE_TEXTURE_SURFACES

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the gamepad help image, and it's labelled callouts.
//-----------------------------------------------------------------------------
HRESULT XBoxDisplay::RenderFrame(LPDIRECTDRAWSURFACE surf)
{

	LPDIRECT3DDEVICE8 m_d3dDevice;

	D3DDevice_BeginScene();

	D3DDevice_SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	D3DDevice_SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_ONE );
	D3DDevice_SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_ZERO );
	D3DDevice_SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	D3DDevice_SetRenderState( D3DRS_ALPHAREF,         0x08 );
	D3DDevice_SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
	D3DDevice_SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );

	//D3DDevice_SetRenderState( D3DRS_CULLMODE,         D3DCULL_CW );

	D3DDevice_SetRenderState( D3DRS_ZENABLE,          D3DZB_TRUE );
	D3DDevice_SetRenderState( D3DRS_FOGENABLE,        FALSE );
	D3DDevice_SetRenderState( D3DRS_STENCILENABLE,    FALSE );

//	D3DDevice_SetRenderState( D3DRS_CLIPPING,         TRUE );

	D3DDevice_SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
//	D3DDevice_SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );

	D3DDevice_SetRenderState( D3DRS_LIGHTING, FALSE );
	D3DDevice_SetRenderState( D3DRS_ZENABLE, TRUE );

	D3DDevice_Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	D3DDevice_SetTexture( 0, NULL );
	D3DDevice_SetRenderState( D3DRS_ZENABLE,          TRUE );
	D3DDevice_SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	D3DDevice_SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );

	D3DDevice_SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	D3DDevice_SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	D3DDevice_SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	D3DDevice_SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	D3DDevice_SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
	D3DDevice_SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	D3DDevice_SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

	// Note: The hardware requires CLAMP for linear textures
	D3DDevice_SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	D3DDevice_SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );


	D3DDevice_SetPixelShader( pixelShader );
	const float fixedRedToY[] =
		{
		1.00,  0.00,  0.00,  0.00,
			0.00,  1.00,  0.00,  0.00,
			0.50, -86. / 1024 + 0.5, 443. / 1024 + 0.5, 0.00,
			0.00,  0.00,  1.00,  0.00,
			351. / 1024 + 0.5, -179. / 1024 + 0.5, 0.50, 0.00
		};
	D3DDevice_SetPixelShaderConstant(0, fixedRedToY, 5);

	D3DDevice_SetTexture( 0, surf );

	surf->GetDevice(&m_d3dDevice);

	D3DDevice_SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
	D3DDevice_SetVertexShader( D3DFVF_CUSTOMVERTEX );
	m_d3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 4-2 );

	D3DDevice_EndScene();

	m_d3dDevice->Present( NULL, NULL, NULL, NULL );


    return S_OK;
}



#endif

#if DUMP_FLIP_TIMES
#include <stdio.h>

extern struct
	{
	int	time, streamTime;
	int	numTimes;
	int	times[16];
	int	avail;
	} dispinfo[40000];
extern int dispcnt;
static struct
	{
	int					time, streamTime;
	D3DFIELD_STATUS	fs1;
	D3DRASTER_STATUS	rs1;
	D3DFIELD_STATUS	fs2;
	D3DRASTER_STATUS	rs2;
	D3DFIELD_STATUS	fs3;
	} flipinfo[40000];
static int flipcnt;
static int filecnt;

void DumpFlipCnt(void)
	{
	FILE * file;
	int i, j, k;
	char buffer[100];

	wsprintf(buffer, "d:\\mpstat%2d.txt", filecnt);
	filecnt++;


	OutputDebugString("############################### DUMP FLIP COUNT ###############################\n");
	file = fopen(buffer, "w");
	if (file)
		{
		i = 0;
		j = 0;
		while (i < flipcnt || j < dispcnt)
			{
			if (j != dispcnt && (i == flipcnt || flipinfo[i].time >= dispinfo[j].time))
				{
				fprintf(file, "%8d, %8d (%8d) A : %2d",
					dispinfo[j].time,
					dispinfo[j].streamTime,
					dispinfo[j].time - dispinfo[j].streamTime,
					dispinfo[j].avail);
				for(k=0; k<dispinfo[j].numTimes; k++)
					fprintf(file, "%8d ", dispinfo[j].times[k]);
				fprintf(file, "\n");
				j++;
				}
			if (i != flipcnt && (j == dispcnt || flipinfo[i].time <= dispinfo[j].time))
				{
				fprintf(file, "%8d, %8d (%8d) (L %3d V %d) (F %6d B %d) (L %3d V %d) (F %6d B %d) (F %6d B %d) %s\n",
					flipinfo[i].time,
					flipinfo[i].streamTime,
					flipinfo[i].time - flipinfo[i].streamTime,
					flipinfo[i].rs1.ScanLine, flipinfo[i].rs1.InVBlank,
					flipinfo[i].fs1.VBlankCount, flipinfo[i].fs1.Field,
					flipinfo[i].rs2.ScanLine, flipinfo[i].rs2.InVBlank,
					flipinfo[i].fs2.VBlankCount, flipinfo[i].fs2.Field,
					flipinfo[i].fs3.VBlankCount, flipinfo[i].fs3.Field,
					i >= 1 && flipinfo[i].fs3.VBlankCount - flipinfo[i-1].fs3.VBlankCount == 1 ? "OK" : "FAIL" );
				i++;
				}
			}

		fclose(file);
		}
	flipcnt = 0;
	dispcnt = 0;
	}
#endif

void XBoxDisplay::CalculateFlipDoneTime(void)
    {
    flipDoneTime = currentDisplayBuffer->flipDoneTime = GetInternalTime() + 16;
    }

void XBoxDisplay::DisplayFrame(XBoxDisplayBuffer * frame, PictureStructure mode)
	{
	D3DFIELD_STATUS fs;

	lock.Enter(false);

#if MEASURE_FLIP_TIME
	static __int64 flipSumTime;
	static int flipCount;
	__int64 t1, t2;

	ReadPerformanceCounter(t1);
#endif

	if (displayFreezeMode) mode = PS_FRAME_PICTURE;

	if ((currentDisplayBuffer != frame) || (currentDisplayMode != mode))
		{
		if (PlaybackSpeed() == 0x10000)
			{
			D3DRASTER_STATUS rs;
			D3DDevice_GetRasterStatus(&rs);

#if DUMP_FLIP_TIMES
			flipinfo[flipcnt].time = GetInternalTime();
			flipinfo[flipcnt].streamTime = CurrentTime();
			flipinfo[flipcnt].rs1 = rs;
#endif

			if (rs.InVBlank)
				YieldTimedFiber(4, TRUE);

			D3DDevice_GetDisplayFieldStatus(&fs);
#if DUMP_FLIP_TIMES
			flipinfo[flipcnt].fs1 = fs;
#endif
			if (fs.VBlankCount == previousVBlankCount)
				YieldTimedFiber((outputHeight-rs.ScanLine)*10/horizScan+4, TRUE);

			D3DDevice_GetRasterStatus(&rs);
#if DUMP_FLIP_TIMES
			flipinfo[flipcnt].rs2 = rs;
#endif
			if (rs.InVBlank)
				YieldTimedFiber(4, TRUE);

			if (frame->fmode != PS_FRAME_PICTURE)
				{
				D3DDevice_GetDisplayFieldStatus(&fs);
#if DUMP_FLIP_TIMES
				flipinfo[flipcnt].fs2 = fs;
#endif

				if (mode == PS_TOP_FIELD    && fs.Field == D3DFIELD_EVEN ||
					 mode == PS_BOTTOM_FIELD && fs.Field == D3DFIELD_ODD)
					{
					YieldTimedFiber((outputHeight-rs.ScanLine)*10/horizScan+4, TRUE);
					}
				}
			}

		D3DDevice_GetDisplayFieldStatus(&fs);
#if DUMP_FLIP_TIMES
		flipinfo[flipcnt].fs3 = fs;
		flipcnt++;
		if (flipcnt == 40000) DumpFlipCnt();
#endif

		previousVBlankCount = fs.VBlankCount;
		PictureStructure orig_mode;
		orig_mode = mode;
		switch (mode)
			{
			case PS_FRAME_PICTURE:
				mode = PS_TOP_FIELD;
			case PS_TOP_FIELD:
				FlipOverlay(frame->lpDDSurface, 0);
#if FRAME_ANNOTATION
				displayedEvenFrames++;
#endif
				break;
			case PS_BOTTOM_FIELD:
				FlipOverlay(frame->lpDDSecondary, 0);
#if FRAME_ANNOTATION
				displayedOddFrames++;
#endif
				break;
			}

#if FRAME_ANNOTATION
		frame->DebugPrint(16,  11, "DisE# %3d", displayedEvenFrames);
		frame->DebugPrint(16,  12, "DisO# %3d", displayedOddFrames);
#endif

		CalculateFlipDoneTime();

		if (previousDisplayBuffer && previousDisplayBuffer != currentDisplayBuffer && previousVBlankCount >= previousDisplayBuffer->flipDoneField)
			previousDisplayBuffer->flipped = FALSE;
		// if this is a new frame, then the old one has been flipped
		if (currentDisplayBuffer != frame)
			{
	      currentDisplayBuffer->flipped = TRUE;
			currentDisplayBuffer->flipDoneField = previousVBlankCount + 1;
			}

		previousDisplayBuffer = currentDisplayBuffer;
		currentDisplayBuffer = frame;
		currentDisplayMode = mode;
		}

#if MEASURE_FLIP_TIME
	ReadPerformanceCounter(t2);

	flipSumTime += t2 - t1;
	flipCount++;

	if (!(flipCount & 255))
		{
		char buffer[100];
		wsprintf(buffer, "Flip %d.%02d\n", (int)(flipSumTime / flipCount), (int)(100 * flipSumTime / flipCount % 100));
		OutputDebugString(buffer);
		flipCount = 0;
		flipSumTime = 0;
		}
#endif
	lock.Leave();

	}

void XBoxDisplay::BeginFreezeDisplay(void)
    {
//    if (currentDisplayBuffer && currentDisplayMode != PS_FRAME_PICTURE && frameFreezeForFieldContent)
    if (currentDisplayBuffer && currentDisplayMode != PS_FRAME_PICTURE )
        {
        DisplayFrame(currentDisplayBuffer, PS_FRAME_PICTURE);
        displayFreezeMode = true;
        }
    }

void XBoxDisplay::EndFreezeDisplay(void)
    {
    displayFreezeMode = false;
    }

bool XBoxDisplay::SupportsStripeAccess(void)
    {
    return FALSE;
    }

bool XBoxDisplay::NeedsPreviousFrame(void)
    {
    return TRUE;
    }

void XBoxDisplay::SetSourceRectangle(bool autoAdapt, WORD left, WORD top, WORD width, WORD height)
	{
	if (!autoAdapt)
		{
		sourceAdapt = FALSE;
		sourceLeft =  left;
		sourceTop = top;
		sourceWidth = width;
		sourceHeight = height;
		}
	else
		{
		sourceAdapt = TRUE;
		sourceLeft = sourceTop = 0;
		sourceWidth = decodeWidth;
		sourceHeight = decodeHeight;
		}

	if (currentDisplayBuffer)
		FlipOverlay(currentDisplayBuffer->lpDDSurface, 0);

	}

void XBoxDisplay::SetDestRectangle(bool autoAdapt, WORD left, WORD top, WORD width, WORD height)
	{
	if (!autoAdapt)
		{
		destAdapt = FALSE;
		destLeft = left;
		destTop = top;
		destWidth = width;
		destHeight = height;
		}
	else
		{
		destAdapt = TRUE;
		destLeft = outputLeft;
		destTop = outputTop;
		destWidth = outputWidth;
		destHeight = outputHeight;
		}

	if (currentDisplayBuffer)
		FlipOverlay(currentDisplayBuffer->lpDDSurface, 0);

	}


void XBoxDisplay::SetOutputRectangle(WORD left, WORD top, WORD width, WORD height)
	{

	outputLeft = left;
	outputTop = top;
	outputWidth = width;
	outputHeight = height;

	if (outputHeight == 480)
		horizScan = 315;
	else if (outputHeight == 576)
		horizScan = 313;
	else if (outputHeight == 720)
		horizScan = 450;
	else if (outputHeight ==  1080)
		horizScan = 675;

	}




void XBoxDisplay::SetClientRectangle(bool autoAdapt, WORD left, WORD top, WORD width, WORD height) {}


//************************
//
// XBoxDisplayBuffer
//
//************************


XBoxDisplayBuffer::XBoxDisplayBuffer(XBoxDisplay           *   display,
                            LPDIRECTDRAWSURFACE     lpDDSurface,
                            LPDIRECTDRAWSURFACE     lpDDSecondary,
                            int                         width,
                            int                         height)
    : PictureDisplayBuffer(display)
    {
    this->display = display;
    this->lpSurface1 = this->lpDDSurface = lpDDSurface;
    this->lpSurface2 = this->lpDDSecondary = lpDDSecondary;
    this->voffset = 0;
    this->decodeWidth = this->width = width;
    this->decodeHeight = this->height = height;
    lpSurface = NULL;
    flipped = FALSE;
    flipDoneTime = 0;
    this->frame = NULL;
    previousFrame = NULL;
    lpDDPrevious = NULL;
    }



XBoxDisplayBuffer::~XBoxDisplayBuffer(void)
    {
    int timeout;

    timeout = 20;

    while (timeout && !DisplayCompleted())
        {
        Sleep(10);
        timeout--;
        }
    }


void XBoxDisplayBuffer::DisplayFrame(PictureStructure mode)
    {
    display->DisplayFrame(this, mode);
    }


bool XBoxDisplayBuffer::DisplayCompleted(void)
	{
	if (!flipped || !lpDDSurface)
		return TRUE;
	else if (display->enforceFlipSync && GetInternalTime() < flipDoneTime)
		return FALSE;
	else
		{
		D3DFIELD_STATUS fs;

		D3DDevice_GetDisplayFieldStatus(&fs);
		return fs.VBlankCount >= flipDoneField;
		}
		/*
		previousVBlankCount = fs.VBlankCount;
		//    else if (IsFlipStatusCompleted(lpDDSurface) && IsFlipStatusCompleted(lpDDSecondary))

		  // if the frame has been flipped, then as soon as the update is complete we can free it up
		  else if (D3DDevice_GetOverlayUpdateStatus())
		  {
        flipped = FALSE;
        return TRUE;
        }
		  else
		  {
        return FALSE;
		  }
		*/
	}

int XBoxDisplayBuffer::DisplayCompletionDelay(void)
    {
    int delay;

    if (!DisplayCompleted())
        {

		delay = flipDoneTime - GetInternalTime();
		if (delay < 2) delay = 2;

        return delay;
        }
    else
        return 0;
    }

bool XBoxDisplayBuffer::Lock1Surface(LPDIRECTDRAWSURFACE & lpDDSurface1)
    {
    HRESULT ddrval;


	while (lpDDSurface1->IsBusy());

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES

    ddrval = lpDDSurface1->LockRect(0, &xLock, NULL, NULL);

#else

    ddrval = lpDDSurface1->LockRect(&xLock, NULL, D3DLOCK_DISCARD);

#endif

#else
	if (lpDDSurface1->Common & D3DCOMMON_VIDEOMEMORY)
		xLock.pBits = (void *)(lpDDSurface1->Data | 0xF0000000);
	else
		xLock.pBits = (void *)(lpDDSurface1->Data | 0x80000000);

	xLock.Pitch = CalcPitch(decodeWidth);
	ddrval = DD_OK;
#endif

/*
    while (ddrval == DDERR_WASSTILLDRAWING || ddrval == DDERR_SURFACEBUSY)
        {
        YieldTimedFiber(DisplayCompletionDelay(), TRUE);
        ddrval = lpDDSurface1->Lock(NULL, &desc, display->lockFlags|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);
        }
*/

    return ddrval == DD_OK;
    }

bool XBoxDisplayBuffer::Lock2Surfaces(LPDIRECTDRAWSURFACE & lpDDSurface1, LPDIRECTDRAWSURFACE & lpDDSurface2)
    {
    HRESULT ddrval;

	while (lpDDSurface1->IsBusy());

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
    ddrval = lpDDSurface1->LockRect(0, &xLock, NULL, NULL);
#else
    ddrval = lpDDSurface1->LockRect(&xLock, NULL, D3DLOCK_DISCARD);
#endif

#else
	if (lpDDSurface1->Common & D3DCOMMON_VIDEOMEMORY)
		xLock.pBits = (void *)(lpDDSurface1->Data | 0xF0000000);
	else
		xLock.pBits = (void *)(lpDDSurface1->Data | 0x80000000);
	xLock.Pitch = CalcPitch(decodeWidth);

	ddrval = DD_OK;
#endif

    if (ddrval == DD_OK)
        {

		while (lpDDSurface2->IsBusy());

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
	    ddrval = lpDDSurface2->LockRect(0, &xLock2, NULL, NULL);
#else
	    ddrval = lpDDSurface2->LockRect(&xLock2, NULL, D3DLOCK_DISCARD);
#endif

#else
	if (lpDDSurface2->Common & D3DCOMMON_VIDEOMEMORY)
		xLock2.pBits = (void *)(lpDDSurface2->Data | 0xF0000000);
	else
		xLock2.pBits = (void *)(lpDDSurface2->Data | 0x80000000);
		xLock2.Pitch = CalcPitch(decodeWidth);

	ddrval = DD_OK;
#endif

        if (ddrval != DD_OK)
            {
#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
            lpDDSurface1->UnlockRect(0);
#else
            lpDDSurface1->UnlockRect();
#endif
#endif
            }
        }
/*
    while (ddrval == DDERR_WASSTILLDRAWING || ddrval == DDERR_SURFACEBUSY)
        {
        YieldTimedFiber(DisplayCompletionDelay(), TRUE);
        ddrval = lpDDSurface1->Lock(NULL, &desc, display->lockFlags|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);
        if (ddrval == DD_OK)
            {
            ddrval = lpDDSurface2->Lock(NULL, &desc2, display->lockFlags|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);
            if (ddrval != DD_OK)
                {
                lpDDSurface1->Unlock(&desc);
                }
            }
        }
*/
    return ddrval == DD_OK;
    }

bool XBoxDisplayBuffer::Lock3Surfaces(LPDIRECTDRAWSURFACE & lpDDSurface1, LPDIRECTDRAWSURFACE & lpDDSurface2, LPDIRECTDRAWSURFACE & lpDDSurface3)
    {
    HRESULT ddrval;

	while (lpDDSurface1->IsBusy());

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
    ddrval = lpDDSurface1->LockRect(0, &xLock, NULL, NULL);
#else
    ddrval = lpDDSurface1->LockRect(&xLock, NULL, D3DLOCK_DISCARD);
#endif

#else
	if (lpDDSurface1->Common & D3DCOMMON_VIDEOMEMORY)
		xLock.pBits = (void *)(lpDDSurface1->Data | 0xF0000000);
	else
		xLock.pBits = (void *)(lpDDSurface1->Data | 0x80000000);
	xLock.Pitch = CalcPitch(decodeWidth);

	ddrval = DD_OK;
#endif

    if (ddrval == DD_OK)
        {

		while (lpDDSurface2->IsBusy());

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
	    ddrval = lpDDSurface2->LockRect(0, &xLock2, NULL, NULL);
#else
	    ddrval = lpDDSurface2->LockRect(&xLock2, NULL, D3DLOCK_DISCARD);
#endif

#else
		if (lpDDSurface2->Common & D3DCOMMON_VIDEOMEMORY)
			xLock2.pBits = (void *)(lpDDSurface2->Data | 0xF0000000);
		else
			xLock2.pBits = (void *)(lpDDSurface2->Data | 0x80000000);
		xLock2.Pitch = CalcPitch(decodeWidth);

		ddrval = DD_OK;
#endif

        if (ddrval == DD_OK)
            {

			while (lpDDSurface3->IsBusy());

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
		    ddrval = lpDDSurface3->LockRect(0, &xLock3, NULL, NULL);
#else
		    ddrval = lpDDSurface3->LockRect(&xLock3, NULL, D3DLOCK_DISCARD);
#endif

#else
			if (lpDDSurface3->Common & D3DCOMMON_VIDEOMEMORY)
				xLock3.pBits = (void *)(lpDDSurface3->Data | 0xF0000000);
			else
				xLock3.pBits = (void *)(lpDDSurface3->Data | 0x80000000);
			xLock3.Pitch = CalcPitch(decodeWidth);

			ddrval = DD_OK;
#endif

	        if (ddrval != DD_OK)
	            {
#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
		        lpDDSurface2->UnlockRect(0);
			    lpDDSurface1->UnlockRect(0);
#else
		        lpDDSurface2->UnlockRect();
			    lpDDSurface1->UnlockRect();
#endif
#endif
				}
			}
        else
            {
#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
            lpDDSurface1->UnlockRect(0);
#else
            lpDDSurface1->UnlockRect();
#endif
#endif
            }
        }

/*
    while (ddrval == DDERR_WASSTILLDRAWING || ddrval == DDERR_SURFACEBUSY)
        {
        YieldTimedFiber(DisplayCompletionDelay(), TRUE);
        ddrval = lpDDSurface->Lock(NULL, &desc, display->lockFlags|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);
        if (ddrval == DD_OK)
            {
            ddrval = lpDDSurface2->Lock(NULL, &desc2, display->lockFlags|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);
            if (ddrval == DD_OK)
                {
                ddrval = lpDDSurface3->Lock(NULL, &desc3, display->lockFlags|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL);


                if (ddrval != DD_OK)
                    {
                    lpDDSurface2->Unlock(&desc);
                    lpDDSurface1->Unlock(&desc);
                    }
                }
            else
                {
                lpDDSurface1->Unlock(&desc);
                }
            }
        }
*/

    return ddrval == DD_OK;
    }

bool XBoxDisplayBuffer::BeginStripeAccess(FrameStore * frame, int sheight, PictureStructure fmode)
    {
    int i;
    bool result;

    display->lock.Enter(false);

    for(i=0; i<display->numDisplayBuffers; i++)
        {
        if (display->displayBuffers[i]->frame == frame)
            display->displayBuffers[i]->frame = NULL;
        }

    display->lock.Leave();

    lock.Enter();

    this->frame = frame;
    this->sheight = sheight;

    if (frame) frame->EnableStripeMode(TRUE);


#if MEASURE_LOCK_TIME
    static int lockSumTime;
    static int lockCount;
    __int64 t1, t2;

    ReadPerformanceCounter(t1);
#endif

    if (lpDDPrevious)
        {
        result = Lock3Surfaces(lpDDSurface, lpDDSecondary, lpDDPrevious);

        if (result)
            {
            lpSurface = (BYTE *)(xLock.pBits);
            lpSecondary = (BYTE *)(xLock2.pBits);
            lpPrevious = (BYTE *)(xLock3.pBits);
            lPitch = xLock.Pitch;
            lpSurface += lPitch * voffset;
            }
        }
    else
        {
        result = Lock2Surfaces(lpDDSurface, lpDDSecondary);

        if (result)
            {
            lpSurface = (BYTE *)(xLock.pBits);
            lpSecondary = (BYTE *)(xLock2.pBits);
            lPitch = xLock.Pitch;
            lpSurface += lPitch * voffset;
            }
        }

#if MEASURE_LOCK_TIME
    ReadPerformanceCounter(t2);

    lockSumTime += t2 - t1;
    lockCount++;

    if (!(lockCount & 255))
        {
        char buffer[100];
        wsprintf(buffer, "Lock %d : %d.%02d\n", lockCount, lockSumTime / lockCount, 100 * lockSumTime / lockCount % 100);
        OutputDebugString(buffer);
        }
#endif

    firstFieldDeinterlaced = FALSE;

    if (!result)
        lock.Leave();

    return result;


    }

void XBoxDisplayBuffer::DoneStripeAccess(void)
    {
    if (lpSurface)
        {
#if MEASURE_UNLOCK_TIME
    static int lockSumTime;
    static int lockCount;
    __int64 t1, t2;

    ReadPerformanceCounter(t1);
#endif

#if FRAME_ANNOTATION
	    YUVD_WriteString(lpSecondary,   lPitch, 40, 3, "Secondary");
#endif

#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
		lpDDSecondary->UnlockRect(0);
#else
		lpDDSecondary->UnlockRect();
#endif
#endif

        if (lpDDPrevious)
            {
#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
			lpDDPrevious->UnlockRect(0);
#else
			lpDDPrevious->UnlockRect();
#endif
#endif
            }

#if FRAME_ANNOTATION
       YUVD_WriteString(lpSurface,   lPitch, 40, 4, "Surface");
#endif

#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
        lpDDSurface->UnlockRect(0);
#else
        lpDDSurface->UnlockRect();
#endif
#endif

        lpSurface = NULL;

#if MEASURE_UNLOCK_TIME
        ReadPerformanceCounter(t2);

        lockSumTime += t2 - t1;
        lockCount++;

        if (!(lockCount & 255))
            {
            char buffer[100];
            wsprintf(buffer, "Unlock %d : %d.%02d\n", lockCount, lockSumTime / lockCount, 100 * lockSumTime / lockCount % 100);
            OutputDebugString(buffer);
            }
#endif

        lock.Leave();
        }

    }

void XBoxDisplayBuffer::CopyStripe(int ss, PictureStructure fmode)
    {
    int width = decodeWidth;
    int height = decodeHeight;
    bool laced = mode != PS_FRAME_PICTURE;

    FullWidthMMXSPUDisplayBuffer * osd = (FullWidthMMXSPUDisplayBuffer *)(this->osd);

    if (lpSurface && decodeHeight > 0 && decodeWidth > 0)
        {

#if SUPPORT_PANSCAN_LETTERBOX
		 switch (EffectivePresentationMode())
			 {
			 case PDPM_FULLSIZE:
				 if (lpDDPrevious)
					 {
					 switch (fmode)
						 {
						 case PS_TOP_FIELD:
							 if (mode == PS_BOTTOM_FIELD)
								 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_BOTTOM_FIELD:
							 if (mode == PS_TOP_FIELD)
								 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_FRAME_PICTURE:
							 switch (mode)
								 {
								 case PS_TOP_FIELD:
									 if (this->osd == previousFrame->osd)
										frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
									 else
										 {
										frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
										frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_ALL & ~DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, osd);
										 }
									 break;
								 case PS_BOTTOM_FIELD:
									 if (this->osd == previousFrame->osd)
										 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
									 else
										 {
										 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
										 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_ALL & ~DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, osd);
										 }
									 break;
								 case PS_FRAME_PICTURE:
									 frame->ConvertToYUVStripe(lpSurface, NULL, width, height, lPitch, ss, sheight, false, false, fmode, MinimumVideoDownscaleFilterLevelSet, osd);
									 frame->ConvertToYUVStripeInterpolate(lpPrevious, previousFrame->frame, width, height, lPitch, ss, sheight, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 }
							 break;
						 }
					 }
				 else
					 {
					 switch (fmode)
						 {
						 case PS_TOP_FIELD:
							 if (mode == PS_BOTTOM_FIELD)
								 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_BOTTOM_FIELD:
							 if (mode == PS_TOP_FIELD)
								 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_FRAME_PICTURE:
							 switch (mode)
								 {
								 case PS_TOP_FIELD:
									 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 case PS_BOTTOM_FIELD:
									 frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 case PS_FRAME_PICTURE:
									 frame->ConvertToYUVStripe(lpSurface, NULL, width, height, lPitch, ss, sheight, false, false, fmode, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 }
							 break;
						 }
					 }
				 break;
			 case PDPM_LETTERBOXED:
				 if (lpDDPrevious)
					 {
					 switch (fmode)
						 {
						 case PS_TOP_FIELD:
							 if (mode == PS_BOTTOM_FIELD)
								 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_BOTTOM_FIELD:
							 if (mode == PS_TOP_FIELD)
								 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_FRAME_PICTURE:
							 switch (mode)
								 {
								 case PS_TOP_FIELD:
									 if (this->osd == previousFrame->osd)
										 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
									 else
										 {
										 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
										 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_ALL & ~DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, osd);
										 }
									 break;
								 case PS_BOTTOM_FIELD:
									 if (this->osd == previousFrame->osd)
										 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
									 else
										 {
										 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
										 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_ALL & ~DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, osd);
										 }
									 break;
								 case PS_FRAME_PICTURE:
									 frame->ConvertToYUVStripeLetterbox(lpSurface, NULL, width, height, lPitch, ss, sheight, false, false, fmode, MinimumVideoDownscaleFilterLevelSet, osd);
									 frame->ConvertToYUVStripeLetterboxInterpolate(lpPrevious, previousFrame->frame, width, height, lPitch, ss, sheight, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 }
							 break;
						 }
					 }
				 else
					 {
					 switch (fmode)
						 {
						 case PS_TOP_FIELD:
							 if (mode == PS_BOTTOM_FIELD)
								 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_BOTTOM_FIELD:
							 if (mode == PS_TOP_FIELD)
								 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_FRAME_PICTURE:
							 switch (mode)
								 {
								 case PS_TOP_FIELD:
									 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 case PS_BOTTOM_FIELD:
									 frame->ConvertToYUVStripeLetterboxDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 case PS_FRAME_PICTURE:
									 frame->ConvertToYUVStripeLetterbox(lpSurface, NULL, width, height, lPitch, ss, sheight, false, false, fmode, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 }
							 break;
						 }
					 }
				 break;
			 case PDPM_PANSCAN:
				 if (lpDDPrevious)
					 {
					 switch (fmode)
						 {
						 case PS_TOP_FIELD:
							 if (mode == PS_BOTTOM_FIELD)
								 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, panScanOffset, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_BOTTOM_FIELD:
							 if (mode == PS_TOP_FIELD)
								 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, panScanOffset, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_FRAME_PICTURE:
							 switch (mode)
								 {
								 case PS_TOP_FIELD:
									 if (this->osd == previousFrame->osd)
										 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, panScanOffset, ss, sheight, true, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
									 else
										 {
										 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, panScanOffset, ss, sheight, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
										 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, panScanOffset, ss, sheight, true, DIFEF_ENABLE_BOTH_ALL & ~DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, osd);
										 }
									 break;
								 case PS_BOTTOM_FIELD:
									 if (this->osd == previousFrame->osd)
										 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, panScanOffset, ss, sheight, false, DIFEF_ENABLE_BOTH_ALL, MinimumVideoDownscaleFilterLevelSet, osd);
									 else
										 {
										 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, panScanOffset, ss, sheight, false, DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
										 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, panScanOffset, ss, sheight, false, DIFEF_ENABLE_BOTH_ALL & ~DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, osd);
										 }
									 break;
								 case PS_FRAME_PICTURE:
									 frame->ConvertToYUVStripePanScan(lpSurface, NULL, width, height, lPitch, panScanOffset, ss, sheight, false, false, fmode, MinimumVideoDownscaleFilterLevelSet, osd);
									 frame->ConvertToYUVStripePanScanInterpolate(lpPrevious, previousFrame->frame, width, height, lPitch, panScanOffset, ss, sheight, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 }
							 break;
						 }
					 }
				 else
					 {
					 switch (fmode)
						 {
						 case PS_TOP_FIELD:
							 if (mode == PS_BOTTOM_FIELD)
								 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, panScanOffset, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_BOTTOM_FIELD:
							 if (mode == PS_TOP_FIELD)
								 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, panScanOffset, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
							 break;
						 case PS_FRAME_PICTURE:
							 switch (mode)
								 {
								 case PS_TOP_FIELD:
									 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, panScanOffset, ss, sheight, true, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 case PS_BOTTOM_FIELD:
									 frame->ConvertToYUVStripePanScanDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, panScanOffset, ss, sheight, false, DIFEF_ENABLE_BOTH_COPY, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 case PS_FRAME_PICTURE:
									 frame->ConvertToYUVStripePanScan(lpSurface, NULL, width, height, lPitch, panScanOffset, ss, sheight, false, false, fmode, MinimumVideoDownscaleFilterLevelSet, osd);
									 break;
								 }
							 break;
						 }
					 }
				 break;
			  }
#else

        if (lpDDPrevious)
            {
            switch (fmode)
                {
                case PS_TOP_FIELD:
                    if (mode == PS_BOTTOM_FIELD)
                        frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_ALL, display->downscaleFilter, osd);
                    break;
                case PS_BOTTOM_FIELD:
                    if (mode == PS_TOP_FIELD)
                        frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_ALL, display->downscaleFilter, osd);
                    break;
                case PS_FRAME_PICTURE:
                    switch (mode)
                        {
                        case PS_TOP_FIELD:
                            frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_ALL, display->downscaleFilter, osd);
                            break;
                        case PS_BOTTOM_FIELD:
                            frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_ALL, display->downscaleFilter, osd);
                            break;
                        case PS_FRAME_PICTURE:
                            frame->ConvertToYUVStripe(lpSurface, NULL, width, height, lPitch, ss, sheight, false, false, fmode, display->downscaleFilter, osd);
                            frame->ConvertToYUVStripeInterpolate(lpPrevious, previousFrame->frame, width, height, lPitch, ss, sheight, display->downscaleFilter, osd);
                            break;
                        }
                    break;
                }
            }
        else
            {
            switch (fmode)
                {
                case PS_TOP_FIELD:
                    if (mode == PS_BOTTOM_FIELD)
                        frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, 2 * ss, 2 * sheight, false, DIFEF_ENABLE_BOTH_COPY, display->downscaleFilter, osd);
                    break;
                case PS_BOTTOM_FIELD:
                    if (mode == PS_TOP_FIELD)
                        frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, 2 * ss, 2 * sheight, true, DIFEF_ENABLE_BOTH_COPY, display->downscaleFilter, osd);
                    break;
                case PS_FRAME_PICTURE:
                    switch (mode)
                        {
                        case PS_TOP_FIELD:
                            frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, frame, width, height, lPitch, ss, sheight, true, DIFEF_ENABLE_BOTH_COPY, display->downscaleFilter, osd);
                            break;
                        case PS_BOTTOM_FIELD:
                            frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, frame, width, height, lPitch, ss, sheight, false, DIFEF_ENABLE_BOTH_COPY, display->downscaleFilter, osd);
                            break;
                        case PS_FRAME_PICTURE:
                            frame->ConvertToYUVStripe(lpSurface, NULL, width, height, lPitch, ss, sheight, false, false, fmode, display->downscaleFilter, osd);
                            break;
                        }
                    break;
                }
            }
#endif
        }

    }

bool XBoxDisplayBuffer::AttemptDirectStripeYUVAccess(int width, int height, int ss, BPTR & p, BPTR & ip, int & stride)
    {
    if (lpSurface && EffectivePresentationMode() == PDPM_FULLSIZE &&
         width  == decodeWidth &&
         height == decodeHeight &&
			(!osd || (height >= 480 && ss + 16 <= osd->nonTransparentTop)))
        {
            stride = lPitch;
            sstart = ss;

            if (mode == PS_FRAME_PICTURE)
                {
                p = lpSurface + stride * ss;
                ip = NULL;
                }
            else
                {
                p = lpSurface + stride * ss;
                ip = lpSecondary + stride * (ss + 1);
                stride *= 2;
                }
            return TRUE;
        }

    return FALSE;
    }

bool XBoxDisplayBuffer::NeedsFrameDataWithDirectYUVAccess(void)
    {
    return TRUE;
    }

void XBoxDisplayBuffer::DoneDirectStripeAccess(PictureStructure fmode)
	{
	if (lpDDPrevious && decodeHeight > 0 && decodeWidth > 0 && previousFrame->frame)
		{
		switch (fmode)
			{
			case PS_TOP_FIELD:
				if (mode == PS_BOTTOM_FIELD)
					frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, 2 * sheight, false, DIFEF_ENABLE_BOTH_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
				break;
			case PS_BOTTOM_FIELD:
				if (mode == PS_TOP_FIELD)
					frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, 2 * sheight, true, DIFEF_ENABLE_BOTH_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
				break;
			case PS_FRAME_PICTURE:
				if (osd == previousFrame->osd)
					{
					switch (mode)
						{
						case PS_TOP_FIELD:
							frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, true, DIFEF_ENABLE_BOTH_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
							break;
						case PS_BOTTOM_FIELD:
							frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, false, DIFEF_ENABLE_BOTH_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
							break;
						case PS_FRAME_PICTURE:
							frame->ConvertToYUVStripeInterpolate(lpPrevious, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
							break;
						}
					}
				else
					{
					switch (mode)
						{
						case PS_TOP_FIELD:
							frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
							frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSurface, lpSecondary, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, true, DIFEF_ENABLE_TOP_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
							break;
						case PS_BOTTOM_FIELD:
							frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, false, DIFEF_ENABLE_TOP_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)(previousFrame->osd));
							frame->ConvertToYUVStripeDeinterlace(lpPrevious, lpSecondary, lpSurface, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, false, DIFEF_ENABLE_BOTTOM_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
							break;
						case PS_FRAME_PICTURE:
							frame->ConvertToYUVStripeInterpolate(lpPrevious, previousFrame->frame, decodeWidth, decodeHeight, lPitch, sstart, sheight, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
							break;
						}
					}
				break;
			}
		}
	}


bool XBoxDisplayBuffer::HasInterlaceArtifacts(void)
    {
    return FALSE;
    }

void XBoxDisplayBuffer::DeinterlaceSurfaces(LPDIRECTDRAWSURFACE lpDDTop, LPDIRECTDRAWSURFACE lpDDBottom, PictureStructure mode, FrameStore * frame, FrameStore * previous, FullWidthMMXSPUDisplayBuffer * cosd, FullWidthMMXSPUDisplayBuffer * posd, bool hurryUp)
	{
	int ppos, psize;

	if (decodeHeight > 0 && decodeWidth > 0)
		{
		ppos = 0;
		psize = 16;

		if (Lock2Surfaces(lpDDTop, lpDDBottom))
			{
			lpSurface = (BYTE *)(xLock.pBits);
			lpSecondary = (BYTE *)(xLock2.pBits);
			lPitch = xLock.Pitch;

			while (ppos < decodeHeight)
				{
				if (cosd == posd)
					{
#if SUPPORT_PANSCAN_LETTERBOX
					switch (EffectivePresentationMode())
						{
						case PDPM_FULLSIZE:
							frame->ConvertToYUVStripeDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, ppos, psize, true, DIFEF_ENABLE_BOTH_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, cosd);
							break;
						case PDPM_LETTERBOXED:
							frame->ConvertToYUVStripeLetterboxDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, ppos, psize, true, DIFEF_ENABLE_BOTH_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, cosd);
							break;
						case PDPM_PANSCAN:
							frame->ConvertToYUVStripePanScanDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, panScanOffset, ppos, psize, true, DIFEF_ENABLE_BOTH_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, cosd);
							break;
						}
#else
	            frame->ConvertToYUVStripeDeinterlace(lpSurface, lpSecondary, NULL, previous, decodeWidth, decodeHeight, lPitch, ppos, psize, true, DIFEF_ENABLE_BOTH_DEINTERLACE, display->downscaleFilter, cosd);
#endif
					}
				else
					{
#if SUPPORT_PANSCAN_LETTERBOX
					switch (EffectivePresentationMode())
						{
						case PDPM_FULLSIZE:
							frame->ConvertToYUVStripeDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, ppos, psize, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, posd);
							frame->ConvertToYUVStripeDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, ppos, psize, true, DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, cosd);
							break;
						case PDPM_LETTERBOXED:
							frame->ConvertToYUVStripeLetterboxDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, ppos, psize, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, posd);
							frame->ConvertToYUVStripeLetterboxDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, ppos, psize, true, DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, cosd);
							break;
						case PDPM_PANSCAN:
							frame->ConvertToYUVStripePanScanDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, panScanOffset, ppos, psize, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, posd);
							frame->ConvertToYUVStripePanScanDeinterlace(lpSurface, lpSecondary, NULL, previous, width, height, lPitch, panScanOffset, ppos, psize, true, DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, cosd);
							break;
						}
#else
	            frame->ConvertToYUVStripeDeinterlace(lpSurface, lpSecondary, NULL, previous, decodeWidth, decodeHeight, lPitch, ppos, psize, true, DIFEF_ENABLE_BOTTOM_DEINTERLACE, display->downscaleFilter, posd);
	            frame->ConvertToYUVStripeDeinterlace(lpSurface, lpSecondary, NULL, previous, decodeWidth, decodeHeight, lPitch, ppos, psize, true, DIFEF_ENABLE_TOP_DEINTERLACE, display->downscaleFilter, cosd);
#endif
					}

				ppos += psize;

            if (IsHigherPriorityTimedFiberReady(6))
					{
#ifdef USE_TEXTURE_SURFACES
#ifndef PUSHER_WORKAROUND
					lpDDBottom->UnlockRect(0);
					lpDDTop->UnlockRect(0);
#endif
					YieldTimedFiber(0);
#ifndef PUSHER_WORKAROUND
					lpDDTop->LockRect(0, &xLock, NULL, 0);
					lpDDBottom->LockRect(0, &xLock2, NULL, 0);
#endif
#else
#ifndef PUSHER_WORKAROUND
					lpDDBottom->UnlockRect();
					lpDDTop->UnlockRect();
#endif
					YieldTimedFiber(0);
#ifndef PUSHER_WORKAROUND
					lpDDTop->LockRect(&xLock, NULL, 0);
					lpDDBottom->LockRect(&xLock2, NULL, 0);
#endif
#endif
					lpSurface = (BYTE *)(xLock.pBits);
					lpSecondary = (BYTE *)(xLock2.pBits);
					}
				}

#if FRAME_ANNOTATION
			YUVD_WriteString(lpSurface,   lPitch, 40, 0, "Top");
			YUVD_WriteString(lpSecondary, lPitch, 40, 0, "Bottom");
#endif


#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
			lpDDBottom->UnlockRect(0);
			lpDDTop->UnlockRect(0);
#else
			lpDDBottom->UnlockRect();
			lpDDTop->UnlockRect();
#endif
#endif
			lpSurface = NULL;
			}
		}

	}

bool XBoxDisplayBuffer::PrepareDeinterlaceDuringDecode(PictureDisplayBuffer * previous)
    {
    if (previous && previous->frame && previous->frame->IsFullFrame())
        {
        previousFrame = (XBoxDisplayBuffer *)previous;

        switch (mode)
            {
            case PS_FRAME_PICTURE:
                if (previous->fmode == PS_FRAME_PICTURE)
                    lpDDPrevious = previousFrame->lpDDSecondary;
                else
                    {
                    previousFrame = NULL;
                    lpDDPrevious = NULL;
                    }
                break;
            case PS_TOP_FIELD:
                lpDDPrevious = previousFrame->lpDDSecondary;
                break;
            case PS_BOTTOM_FIELD:
                lpDDPrevious = previousFrame->lpDDSurface;
                break;
            }

        deinterlaceY = 0;

        return lpDDPrevious != NULL;
        }
    else
        {
        previousFrame = NULL;
        lpDDPrevious = NULL;
        return FALSE;
        }
    }

bool XBoxDisplayBuffer::DeinterlaceFrame(PictureDisplayBuffer * previousT, Inverse32PulldownHint & phint, bool hurryUp)
    {
    LPDIRECTDRAWSURFACE lpDDTemorary;
    XBoxDisplayBuffer * previous = (XBoxDisplayBuffer *)previousT;


    if (frame && frame->IsFullFrame() &&
         previous->frame && previous->frame->IsFullFrame())
        {
        lock.Enter();

#if MEASURE_DEINTERLACE_TIME
    static __int64 lockSumTime;
    static int lockCount;
    __int64 t1, t2;

    ReadPerformanceCounter(t1);
#endif

        if (fmode == PS_FRAME_PICTURE)
            {
            if (previous->fmode == PS_FRAME_PICTURE)
                {
                if (!lpDDPrevious && decodeHeight > 0 && decodeWidth > 0)
                    {
                    if (Lock1Surface(previous->lpDDSecondary))
                        {
                        lpSurface = (BYTE *)(xLock.pBits);

#if SUPPORT_PANSCAN_LETTERBOX
						 switch (EffectivePresentationMode())
							 {
							 case PDPM_FULLSIZE:
								 frame->ConvertToYUVStripeInterpolate(lpSurface, previous->frame, width, height, lPitch, 0, frame->Height(), MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 case PDPM_LETTERBOXED:
								 frame->ConvertToYUVStripeLetterboxInterpolate(lpSurface, previous->frame, width, height, lPitch, 0, frame->Height(), MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 case PDPM_PANSCAN:
								 frame->ConvertToYUVStripePanScanInterpolate(lpSurface, previous->frame, width, height, lPitch, panScanOffset, 0, frame->Height(), MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 }
#else

                        frame->ConvertToYUVStripeInterpolate(lpSurface, previous->frame, decodeWidth, decodeHeight, lPitch, 0, frame->Height(), display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
#endif

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
                        previous->lpDDSecondary->UnlockRect(0);
#else
                        previous->lpDDSecondary->UnlockRect();
#endif
#endif
                        lpSurface = NULL;
                        }
                    }

#ifdef INVERSE_3_2_PULLDOWN__INTERPOLATION
                previous->mode = PS_TOP_FIELD;
#endif
                }
            else if (previous->mode != PS_FRAME_PICTURE)
                {
                if (previous->mode == PS_TOP_FIELD)
                    DeinterlaceSurfaces(previous->lpDDSecondary, lpDDSecondary, previous->mode, frame, previous->frame, (FullWidthMMXSPUDisplayBuffer *)osd, (FullWidthMMXSPUDisplayBuffer *)(previous->osd), hurryUp);
                else
                    DeinterlaceSurfaces(lpDDSecondary, previous->lpDDSurface, previous->mode, previous->frame, frame, (FullWidthMMXSPUDisplayBuffer *)(previous->osd), (FullWidthMMXSPUDisplayBuffer *)osd, hurryUp);
                }
            }
		else
            {
            if (previous->mode == PS_FRAME_PICTURE && phint != IPDH_UNKNOWN && frame->Is32PulldownFrame(previous->frame, phint))
                {
                if (decodeHeight > 0 && decodeWidth > 0 && Lock1Surface(lpDDSurface))
                    {
                    lpSurface = (BYTE *)(xLock.pBits);
                    lPitch = xLock.Pitch;

#if SUPPORT_PANSCAN_LETTERBOX
					 switch (EffectivePresentationMode())
						 {
						 case PDPM_FULLSIZE:
							 frame->ConvertToYUVStripeSingleInverse32Pulldown(lpSurface, previous->frame, width, height, lPitch, 0, frame->Height(), phint, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
							 break;
						 case PDPM_LETTERBOXED:
							 frame->ConvertToYUVStripeLetterboxSingleInverse32Pulldown(lpSurface, previous->frame, width, height, lPitch, 0, frame->Height(), phint, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
							 break;
						 case PDPM_PANSCAN:
//							 frame->ConvertToYUVStripeSingleInverse32Pulldown(lpSurface, previous->frame, width, height, lPitch, 0, frame->Height(), phint, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
							 break;
						 }
#else

                    frame->ConvertToYUVStripeSingleInverse32Pulldown(lpSurface, previous->frame, decodeWidth, decodeHeight, lPitch, 0, frame->Height(), phint, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
#endif

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
                    lpDDSurface->UnlockRect(0);
#else
                    lpDDSurface->UnlockRect();
#endif
#endif
                    lpSurface = NULL;
                    }

                mode = PS_FRAME_PICTURE;
                }
            else
                {
                phint = IPDH_UNKNOWN;

                if (!lpDDPrevious)
                    {
                    if (mode == PS_TOP_FIELD)
                        DeinterlaceSurfaces(previous->lpDDSecondary, lpDDSurface, mode, frame, previous->frame, (FullWidthMMXSPUDisplayBuffer *)osd, (FullWidthMMXSPUDisplayBuffer *)(previous->osd), hurryUp);
                    else
                        DeinterlaceSurfaces(lpDDSecondary, previous->lpDDSurface, mode, previous->frame, frame, (FullWidthMMXSPUDisplayBuffer *)(previous->osd), (FullWidthMMXSPUDisplayBuffer *)osd, hurryUp);
                    }
                }
            }

        if (!previous->firstFieldDeinterlaced && previous->fmode != PS_FRAME_PICTURE)
            {
            if (previous->fmode == PS_TOP_FIELD)
                {
                lpDDTemorary = previous->lpDDSurface; previous->lpDDSurface = previous->lpDDSecondary; previous->lpDDSecondary = lpDDTemorary;
                }

            previous->firstFieldDeinterlaced = TRUE;
            previous->mode = PS_FRAME_PICTURE;
            }

#if MEASURE_DEINTERLACE_TIME
        ReadPerformanceCounter(t2);

        lockSumTime += t2 - t1;
        lockCount++;

        if (!(lockCount & 255))
            {
            char buffer[100];
            wsprintf(buffer, "Deinterlace %d : %d.%02d\n", lockCount, (int)(lockSumTime / lockCount), (int)(100 * lockSumTime / lockCount % 100));
            OutputDebugString(buffer);
            }
#endif

        firstFieldDeinterlaced = TRUE;
        previousFrame = NULL;
        lpDDPrevious = NULL;

        lock.Leave();

        return TRUE;
        }
	else
		return FALSE;

    }

bool XBoxDisplayBuffer::CompleteDeinterlaceFrame(void)
    {
    LPDIRECTDRAWSURFACE lpDDTemorary;

    if (frame && frame->IsFullFrame())
        {
        lock.Enter();

        if (!firstFieldDeinterlaced)
            {
            switch (fmode)
                {
                case PS_TOP_FIELD:

                    if (decodeHeight > 0 && decodeWidth > 0 && Lock1Surface(lpDDSurface))
                        {
                        lpSurface = (BYTE *)(xLock.pBits);
#if SUPPORT_PANSCAN_LETTERBOX
						 switch (EffectivePresentationMode())
							 {
							 case PDPM_FULLSIZE:
								 frame->ConvertToYUVStripeDeinterlaceStretch(lpSurface, NULL, width, height, lPitch, 0, frame->Height(),
									 DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 case PDPM_LETTERBOXED:
								 frame->ConvertToYUVStripeLetterboxDeinterlaceStretch(lpSurface, NULL, width, height, lPitch, 0, frame->Height(),
									 DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 case PDPM_PANSCAN:
								 frame->ConvertToYUVStripePanScanDeinterlaceStretch(lpSurface, NULL, width, height, lPitch, panScanOffset, 0, frame->Height(),
									 DIFEF_ENABLE_TOP_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 }
#else


                        frame->ConvertToYUVStripeDeinterlaceStretch(lpSurface, NULL, decodeWidth, decodeHeight, lPitch, 0, frame->Height(),
                            DIFEF_ENABLE_TOP_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
#endif

#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
                        lpDDSurface->UnlockRect(0);
#else
                        lpDDSurface->UnlockRect();
#endif
#endif
                        lpSurface = NULL;
                        }

                    break;
                case PS_BOTTOM_FIELD:

                    if (decodeHeight > 0 && decodeWidth > 0 && Lock1Surface(lpDDSecondary))
                        {
                        lpSurface = (BYTE *)(xLock.pBits);

#if SUPPORT_PANSCAN_LETTERBOX
						 switch (EffectivePresentationMode())
							 {
							 case PDPM_FULLSIZE:
								 frame->ConvertToYUVStripeDeinterlaceStretch(NULL, lpSurface, width, height, lPitch, 0, frame->Height(),
									 DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 case PDPM_LETTERBOXED:
								 frame->ConvertToYUVStripeLetterboxDeinterlaceStretch(NULL, lpSurface, width, height, lPitch, 0, frame->Height(),
									 DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 case PDPM_PANSCAN:
								 frame->ConvertToYUVStripePanScanDeinterlaceStretch(NULL, lpSurface, width, height, lPitch, panScanOffset, 0, frame->Height(),
									 DIFEF_ENABLE_BOTTOM_DEINTERLACE, MinimumVideoDownscaleFilterLevelSet, (FullWidthMMXSPUDisplayBuffer *)osd);
								 break;
							 }
#else

                        frame->ConvertToYUVStripeDeinterlaceStretch(NULL, lpSurface, decodeWidth, decodeHeight, lPitch, 0, frame->Height(),
                            DIFEF_ENABLE_BOTTOM_DEINTERLACE, display->downscaleFilter, (FullWidthMMXSPUDisplayBuffer *)osd);
#endif

#ifndef PUSHER_WORKAROUND
#ifdef USE_TEXTURE_SURFACES
                        lpDDSecondary->UnlockRect(0);
#else
                        lpDDSecondary->UnlockRect();
#endif
#endif
                        lpSurface = NULL;
                        }
                    break;
                }
            }

        if (fmode == PS_BOTTOM_FIELD)
            {
            lpDDTemorary = lpDDSurface; lpDDSurface = lpDDSecondary; lpDDSecondary = lpDDTemorary;
            }

        mode = PS_FRAME_PICTURE;
        lock.Leave();

        return TRUE;
        }
    else
        return FALSE;
    }

bool XBoxDisplayBuffer::DeinterlaceInitialFrame(void)
    {
    return FALSE;
    }

void XBoxDisplayBuffer::CopyFrame(FrameStore * frame)
    {
    if (frame && frame->IsFullFrame())
        {
		  if (display->completed)
			  fmode = PS_FRAME_PICTURE;

        if (fmode == PS_FRAME_PICTURE)
            {
            if (BeginStripeAccess(frame, frame->Height(), PS_FRAME_PICTURE))
                {
                CopyStripe(0, PS_FRAME_PICTURE);
                DoneStripeAccess();
                }

            mode = fmode;
            }
        else
            {
            if (BeginStripeAccess(frame, frame->Height(), PS_FRAME_PICTURE))
                {
                CopyStripe(0, PS_FRAME_PICTURE);
                DoneStripeAccess();
                }

				CompleteDeinterlaceFrame();

            mode = fmode;
            }
        }
    }

void XBoxDisplayBuffer::UpdateOSDBitmap(SPUDisplayBuffer * map)
    {
    display->lock.Enter();

    lock.Enter();

    if (frame && map && frame->IsFullFrame())
        {
        osd = map;

        CopyFrame(frame);
        }

    lock.Leave();

    display->lock.Leave();
    }



void XBoxDisplayBuffer::ClearFrame(void)
    {
    lock.Enter();

    if (BeginStripeAccess(NULL, height, PS_FRAME_PICTURE))
        {
        DoneStripeAccess();
        }

    lock.Leave();
    }


void __cdecl XBoxDisplayBuffer::DebugPrint(int x, int y, char * format, ...)
    {
    HRESULT ddrval;

    char buffer[256];
    wvsprintf(buffer, format, (LPSTR)(&format+1));

    lock.Enter();

	/*
    while ((ddrval = lpDDSurface->Lock(NULL, &desc, display->lockFlags|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY, NULL)) == DDERR_WASSTILLDRAWING)
        {
        YieldTimedFiber(DisplayCompletionDelay(), TRUE);
        }
	*/



#ifndef PUSHER_WORKAROUND

#ifdef USE_TEXTURE_SURFACES
	ddrval = lpDDSurface->LockRect(0, &xLock, NULL, NULL);

    if (ddrval == DD_OK)
        {
        lpSurface = (BYTE *)(xLock.pBits);
        lPitch = xLock.Pitch;

		ddrval = lpDDSecondary->LockRect(0, &xLock2, NULL, NULL);
        if (ddrval == DD_OK)
            {
            lpSecondary = (BYTE *)(xLock2.pBits);

            YUVD_WriteString(lpSurface, lPitch, x, y, buffer);
            YUVD_WriteString(lpSecondary, lPitch, x, y, buffer);

            lpDDSecondary->UnlockRect(0);
            }
        lpDDSurface->UnlockRect(0);
        lpSurface = NULL;
        }
#else
	ddrval = lpDDSurface->LockRect(&xLock, NULL, NULL);

    if (ddrval == DD_OK)
        {
        lpSurface = (BYTE *)(xLock.pBits);
        lPitch = xLock.Pitch;

		ddrval = lpDDSecondary->LockRect(&xLock2, NULL, NULL);
        if (ddrval == DD_OK)
            {
            lpSecondary = (BYTE *)(xLock2.pBits);

            YUVD_WriteString(lpSurface, lPitch, x, y, buffer);
            YUVD_WriteString(lpSecondary, lPitch, x, y, buffer);

            lpDDSecondary->UnlockRect();
            }
        lpDDSurface->UnlockRect();
        lpSurface = NULL;
        }
#endif

#else

	if (lpDDSurface->Common & D3DCOMMON_VIDEOMEMORY)
		xLock.pBits = (void *)(lpDDSurface->Data | 0xF0000000);
	else
		xLock.pBits = (void *)(lpDDSurface->Data | 0x80000000);

	xLock.Pitch = CalcPitch(decodeWidth);
	ddrval = DD_OK;

    if (ddrval == DD_OK)
        {
        lpSurface = (BYTE *)(xLock.pBits);
        lPitch = xLock.Pitch;

		if (lpDDSecondary->Common & D3DCOMMON_VIDEOMEMORY)
			xLock2.pBits = (void *)(lpDDSecondary->Data | 0xF0000000);
		else
			xLock2.pBits = (void *)(lpDDSecondary->Data | 0x80000000);

		xLock2.Pitch = CalcPitch(decodeWidth);
		ddrval = DD_OK;

        if (ddrval == DD_OK)
            {
            lpSecondary = (BYTE *)(xLock2.pBits);

            YUVD_WriteString(lpSurface, lPitch, x, y, buffer);
            YUVD_WriteString(lpSecondary, lPitch, x, y, buffer);

            }
        lpSurface = NULL;
        }


#endif



    lock.Leave();
    }

#pragma warning(default : 4799)



