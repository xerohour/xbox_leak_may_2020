/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: Stereo.cpp                                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Andrei Osnovich    02/08/00  Created                                *
*                                                                           *
\***************************************************************************/
// NB: This file is 'opted out' of pre-compiled headers
//#pragma optimize ("", off)

#define DEFINE_HOTKEY_INTERFACES

#include "nvprecomp.h"
#include "nvPopupList.h"

#ifdef  STEREO_SUPPORT

#include "nvreg.h"

LPSTEREODATA                pStereoData;
float                       MAX_STEREO_SEPARATION; //We won't allow to go beyond this value.
extern DWORD                ActivateStereo(PNVD3DCONTEXT pContext);
extern DWORD                DeactivateStereo(PNVD3DCONTEXT pContext);
extern void                 ResetStereoParameters(void);
extern DWORD __stdcall      SetExclusiveMode32(LPDDHAL_SETEXCLUSIVEMODEDATA lpSetExclusiveMode);

#define VeryBrightGrey16    0xF7DE
#define VeryBrightGrey32    0xFEFEFE
#define COLOR_KEY  ((pDriverData->ModeList[pDXShare->dwModeNumber].dwBPP == 16) ? VeryBrightGrey16 : VeryBrightGrey32)

#ifdef  STEREO_DEBUG
//Private options for debugging purpose
//#define FLAT_STAT
//#define MEMORY_TEST
//#define ONE_EYE
//#define NULL_RIGHT_EYE
//#define LEFT_ALWAYS_FIRST
//#define SHOW_EYES
//#define LASERSTATECHANGE
//#define PARTIALRENDERING
//#define DELAYED_HOTKEY_INIT

#ifdef  PARTIALRENDERING
#ifndef LEFT_ALWAYS_FIRST
#define LEFT_ALWAYS_FIRST
#endif
#endif
#endif  //STEREO_DEBUG

#ifdef  DELAYED_HOTKEY_INIT
DWORD   FirstHotKeyInit;
#endif  //DELAYED_HOTKEY_INIT

//use this to control compilation of a popup agents list
#define USE_POPUP_LIST

DWORD  nFlips; 

typedef struct _bmp_common_header_s
{
    DWORD dwMagic;
    DWORD dwPad1;
    WORD  wPad2;
    DWORD dwBfOffBits;
} BMPCOMMONHEADER, *LPBMPCOMMONHEADER;

typedef struct _bmp_win_header_s
{
    DWORD dwHeaderSize;
    DWORD dwWidth;
    DWORD dwHeight;
    WORD  wPlanes;
    WORD  dwBitsPerPixel;
    DWORD dwCompression;
    DWORD dwPad3;
    DWORD dwXPelsPerMeter;
    DWORD dwYPelsPerMeter;
    DWORD dwColorUsed;
    DWORD dwPad4;
} BMPWINHEADER, *LPBMPWINHEADER;

typedef struct _bmp_header_s
{
    BMPCOMMONHEADER CommonHeader;
    BMPWINHEADER    WinHeader;
} BMPHEADER, *LPBMPHEADER;

#define BMP_MAGIC       0x30364D42  //BM60
#define BMP_HEADER_SIZE (sizeof(BMPHEADER))
#define BMP_WIN_HEADER_SIZE (sizeof(BMPWINHEADER))

#ifdef	SHOW_EYES
CSimpleSurface *pFrontSurf;

void ShowEyes(void)
{
    // make sure we get access to all of video memory
    bltUpdateClip(pDriverData);
    pDriverData->blitCalled = TRUE;
    pDriverData->TwoDRenderingOccurred = 1;
    // make sure we sync with other channels before writing put
    getDC()->nvPusher.setSyncChannelFlag();

    if (pDriverData->bltData.dwLastRop != SRCCOPYINDEX) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_ROP) | SET_ROP_OFFSET | 0x40000);
        nvPushData(1, SRCCOPYINDEX);
        nvPusherAdjust(2);
        pDriverData->bltData.dwLastRop = SRCCOPYINDEX;
    }
    if (pDriverData->bltData.dwLastColourKey != 0xFFFFFFFF) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | SET_TRANSCOLOR_OFFSET | 0x40000);
        nvPushData(3, 0);
        nvPusherAdjust(4);
        pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
        pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_COLOR_KEY;
    }
    CSimpleSurface *pSurf = ((PNVD3DCONTEXT)(pStereoData->dwContext))->pRenderTarget;
    DWORD dwSrcSize, dwDstSize, dwCombinedPitch, dwSrcOffset, dwDstOffset, dwFormat, dwColourFormat; 
	DWORD dwDeltaX, dwDeltaY, dwControlPointIn, dwControlPointOut;
    dwSrcOffset = pSurf->getOffset();
    dwDstOffset = pFrontSurf->getOffset();
    dwCombinedPitch = (pFrontSurf->getPitch() << 16) | pSurf->getPitch();
	dwSrcSize = (pSurf->getHeight() << 16) | pSurf->getWidth();
	dwControlPointIn = 0;
	dwDstSize = (pFrontSurf->getHeight() >> 1 << 16) | pFrontSurf->getWidth();
	dwControlPointOut = 0;
	dwFormat = pFrontSurf->getFormat();
	dwColourFormat = nv062SurfaceFormat[dwFormat];
    if (pDriverData->bltData.dwLastColourFormat != dwColourFormat ||
        pDriverData->bltData.dwLastCombinedPitch != dwCombinedPitch ||
        pDriverData->bltData.dwLastSrcOffset != dwSrcOffset ||
        pDriverData->bltData.dwLastDstOffset != dwDstOffset) {

        nvPushData(0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
        nvPushData(1, dwColourFormat);                  // SetColorFormat
        nvPushData(2, dwCombinedPitch);                 // SetPitch
        nvPushData(3, dwSrcOffset);                        // SetSrcOffset
        nvPushData(4, dwDstOffset);                        // SetDstOffset
        nvPusherAdjust(5);

        pDriverData->bltData.dwLastColourFormat = dwColourFormat;
        pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;
        pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
        pDriverData->bltData.dwLastDstOffset = dwDstOffset;
    }

    dwDeltaX = ((pSurf->getWidth() - 1) << 20)  / (pFrontSurf->getWidth() - 1);    // 12.20 fixed point
    dwDeltaY = ((pSurf->getHeight() - 1) << 20) / ((pFrontSurf->getHeight() >> 1) - 1);   // 12.20 fixed point
    nvPushData ( 0, dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData ( 1, NV_DD_SCALED_IMAGE_IID);
    nvPushData ( 2, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000);
    nvPushData ( 3, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // SetContextDmaImage
    nvPushData ( 4, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_SET_COLOR_FORMAT | 0x40000);
    nvPushData ( 5, nv089SurfaceFormat[dwFormat]); // SetColorFormat
    nvPushData ( 6, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CLIPPOINT_OFFSET | 0x180000);
    nvPushData ( 7, dwControlPointOut);             // ClipPoint
    nvPushData ( 8, dwDstSize);              // ClipSize
    nvPushData ( 9, dwControlPointOut);             // ImageOutPoint
    nvPushData (10, dwDstSize);              // ImageOutSize
    nvPushData (11, dwDeltaX);                 // DsDx
    nvPushData (12, dwDeltaY);                 // DtDy
    nvPushData (13, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0x100000);
    nvPushData (14, dwSrcSize);              // ImageInSize
    nvPushData (15, (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |     // ImageInFormat
                   (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                   pSurf->getPitch());
    nvPushData (16, dwSrcOffset);         // ImageInOffset
    nvPushData (17, dwControlPointIn);             // ImageInPoint

    nvPushData (18, dDrawSubchannelOffset(NV_DD_STRETCH) | NV089_SET_COLOR_CONVERSION | 0x40000);
    nvPushData (19, NV089_SET_COLOR_CONVERSION_DITHER); // SetColorFormat
    nvPusherAdjust (20);

	dwSrcOffset = pSurf->getOffsetRight();
	dwDstOffset += (pFrontSurf->getHeight() >> 1) * pFrontSurf->getPitch();

    nvPushData(0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
    nvPushData(1, dwColourFormat);                  // SetColorFormat
    nvPushData(2, dwCombinedPitch);                 // SetPitch
    nvPushData(3, dwSrcOffset);                        // SetSrcOffset
    nvPushData(4, dwDstOffset);                        // SetDstOffset
    nvPusherAdjust(5);

    pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
    pDriverData->bltData.dwLastDstOffset = dwDstOffset;

    nvPushData ( 0, dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData ( 1, NV_DD_SCALED_IMAGE_IID);
    nvPushData ( 2, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000);
    nvPushData ( 3, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // SetContextDmaImage
    nvPushData ( 4, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_SET_COLOR_FORMAT | 0x40000);
    nvPushData ( 5, nv089SurfaceFormat[dwFormat]); // SetColorFormat
    nvPushData ( 6, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_CLIPPOINT_OFFSET | 0x180000);
    nvPushData ( 7, dwControlPointOut);             // ClipPoint
    nvPushData ( 8, dwDstSize);              // ClipSize
    nvPushData ( 9, dwControlPointOut);             // ImageOutPoint
    nvPushData (10, dwDstSize);              // ImageOutSize
    nvPushData (11, dwDeltaX);                 // DsDx
    nvPushData (12, dwDeltaY);                 // DtDy
    nvPushData (13, dDrawSubchannelOffset(NV_DD_STRETCH) | SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0x100000);
    nvPushData (14, dwSrcSize);              // ImageInSize
    nvPushData (15, (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |     // ImageInFormat
                   (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                   pSurf->getPitch());
    nvPushData (16, dwSrcOffset);         // ImageInOffset
    nvPushData (17, dwControlPointIn);             // ImageInPoint

    nvPushData (18, dDrawSubchannelOffset(NV_DD_STRETCH) | NV089_SET_COLOR_CONVERSION | 0x40000);
    nvPushData (19, NV089_SET_COLOR_CONVERSION_DITHER); // SetColorFormat
    nvPusherAdjust (20);

	//And finally copy the left front to the right front
	dwSrcOffset = pFrontSurf->getOffset();
	dwDstOffset = pFrontSurf->getOffsetRight();
    dwCombinedPitch = (pFrontSurf->getPitch() << 16) | pFrontSurf->getPitch();
	dwDstSize = (pFrontSurf->getHeight() << 16) | pFrontSurf->getWidth();
    nvPushData(0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
    nvPushData(1, dwColourFormat);                  // SetColorFormat
    nvPushData(2, dwCombinedPitch);                 // SetPitch
    nvPushData(3, dwSrcOffset);                        // SetSrcOffset
    nvPushData(4, dwDstOffset);                        // SetDstOffset
    nvPusherAdjust(5);
    pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
    pDriverData->bltData.dwLastDstOffset = dwDstOffset;
    pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;

	nvPushData (0, dDrawSubchannelOffset(NV_DD_BLIT) | BLIT_POINT_IN_OFFSET | 0xC0000);
	nvPushData (1, dwControlPointOut); 
	nvPushData (2, dwControlPointOut); 
	nvPushData (3, dwDstSize);     
	nvPusherAdjust(4);

    nvPusherStart(TRUE);
}

#endif	//SHOW_EYES

#ifdef  LASER_SIGHT
//*********************************************************
//Spot laser sight stuff

typedef struct RHWBasics_S
{
    float fLaserSZBase;
    float fLaserRHWBase;
    float fLaserRHWScale;
} RHWBASICS;

RHWBASICS RHWBasics;

#define LaserSZBase     RHWBasics.fLaserSZBase
#define LaserRHWBase    RHWBasics.fLaserRHWBase
#define LaserRHWScale   RHWBasics.fLaserRHWScale

float LaserMaxSizeDivider = 75.f;
float LaserMinSizeDivider = 450.f;
DWORD Transparency = 0x80000000;

// dwProperty structure:
//   [31:24] = MSB of Transparency
//   [23:12] = LaserMaxSizeDivider
//   [11:00] = LaserMinSizeDivider

void  ProcessLaserSightProperty(DWORD dwProperty)
{
    if (dwProperty)
    {
        LaserMinSizeDivider = (float)(dwProperty & 0xFFF);
        LaserMaxSizeDivider = (float)((dwProperty >> 12) & 0xFFF);
        if (LaserMinSizeDivider <= 0.0f)
            LaserMinSizeDivider = 1.0f;
        if (LaserMaxSizeDivider <= 0.0f)
            LaserMaxSizeDivider = 1.0f;
        Transparency = dwProperty & 0xFF000000;
    }
}

float LaserMaxSize;
float LaserMinSize;

#define LASER_WIDTH     64
#define LASER_HEIGHT    64
#define LASER_BMP_SIZE  LASER_WIDTH*LASER_HEIGHT*3

typedef struct _ClearDP2Command_S
{
    D3DHAL_DP2COMMAND Command;
    D3DHAL_DP2CLEAR   Clear;
} CLEARDP2COMMAND, *LPCLEARDP2COMMAND;

CLEARDP2COMMAND LaserDP2ClearCommand =
{
    {D3DDP2OP_CLEAR, 0, 1},
    {D3DCLEAR_TARGET, 0, 0, 0, 0, 0, 0, 0}
};

D3DHAL_DRAWPRIMITIVES2DATA LaserDP2ClearData =
{
    0,                                          //dwhContext.
    D3DHALDP2_USERMEMVERTICES,
    D3DFVF_TLVERTEX,
    NULL,                                       //lpDDCommands.
    0,                                          //dwCommandOffset
    sizeof (CLEARDP2COMMAND),                   //dwCommandLength
    NULL,                                       //lpVertices
    0,                                          //dwVertexOffset
    0,                                          //dwVertexLength
    0,                                          //dwReqVertexBufSize
    0,                                          //dwReqCommandBufSize
    NULL,                                       //lpdwRStates
    0,                                          //ddrval
    0                                           //dwErrorOffset
};

typedef struct _PopupDP2Command_S
{
    D3DHAL_DP2COMMAND Command;
    D3DHAL_DP2TRIANGLESTRIP LaserFan[1];
} POPUPDP2COMMAND, *LPPOPUPDP2COMMAND;

POPUPDP2COMMAND LaserDP2Command =
{
    {D3DDP2OP_TRIANGLESTRIP, 0, 2},
    {0}
};

D3DTLVERTEX LaserVertices[] =
{
    {0.0f, 0.0f, 0.0f, 0.0f, RGB(255, 255, 255), RGB(0, 0, 0), 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, RGB(255, 255, 255), RGB(0, 0, 0), 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, RGB(255, 255, 255), RGB(0, 0, 0), 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, RGB(255, 255, 255), RGB(0, 0, 0), 1.0f, 1.0f},
};

CNvObject                  *pLaser_Obj;
CTexture                   *pLaser_Texture;

D3DHAL_DRAWPRIMITIVES2DATA LaserDP2Data =
{
    0,                                          //dwhContext.
    D3DHALDP2_USERMEMVERTICES,
    D3DFVF_TLVERTEX,
    NULL,                                       //lpDDCommands.
    0,                                          //dwCommandOffset
    sizeof (POPUPDP2COMMAND),                   //dwCommandLength
    (LPDDRAWI_DDRAWSURFACE_LCL)LaserVertices,   //lpVertices
    0,                                          //dwVertexOffset
    4,                                          //dwVertexLength
    0,                                          //dwReqVertexBufSize
    0,                                          //dwReqCommandBufSize
    NULL,                                       //lpdwRStates
    sizeof (D3DTLVERTEX),                       //dwVertexSize
    0                                           //dwErrorOffset
};

typedef struct _dwLaserStateSet
{
    DWORD dwState;
    DWORD dwValue;
} STATESETDATA;

#define NUM_STATES 14

STATESETDATA dwLaserStateSet[NUM_STATES] =
{
    { D3DRENDERSTATE_CULLMODE,         D3DCULL_NONE             },
    { D3DRENDERSTATE_ZFUNC,            D3DCMP_ALWAYS            },
    { D3DRENDERSTATE_ALPHAFUNC,        D3DCMP_ALWAYS            },
    { D3DRENDERSTATE_ALPHABLENDENABLE, TRUE                     },
    { D3DRENDERSTATE_SRCBLEND,         D3DBLEND_SRCALPHA        },
    { D3DRENDERSTATE_DESTBLEND,        D3DBLEND_INVSRCALPHA     },
    { D3DRENDERSTATE_WRAP0,            0                        },
    { D3DRENDERSTATE_STENCILENABLE,    FALSE                    },
    { D3DRENDERSTATE_FILLMODE,         D3DFILL_SOLID            },
    { D3DRENDERSTATE_FOGENABLE,        FALSE                    },
    { D3DRENDERSTATE_WRAPU,            0                        },
    { D3DRENDERSTATE_WRAPV,            0                        },
    { D3DRENDERSTATE_TEXTUREMAPBLEND,  D3DPTBLENDCAPS_MODULATE  },
    { D3DRENDERSTATE_SPECULARENABLE,   FALSE                    }
};
///////////////// File logging //////////////////////////////////////////////
static BOOL logToFile=TRUE;								//file-wide flag
static char nvStereoLog[256];							//log file name
//--------------------------------------------------------------------------
#define DEFAULT_LOG_NAME		"nvStereoLog.txt"

#if (IS_WINNT5 || IS_WIN9X)
    #define HAVE_NV_FILE_FACILITY
#endif  // (IS_WINNT5 || IS_WIN9X)
////////////////////////////////////////////////////////////////////////////
void __cdecl LOG(char * szFormat, ...)
{
	//check whether we can use file IO (w2k, w9x -- yes; NT4 -- no)
#ifdef HAVE_NV_FILE_FACILITY
    static BOOL first = TRUE;
    char   str[256];

	nvSprintfVAL (str, szFormat, (va_list)(&szFormat+1));
	nvStrCat(str,"\n");

    if (logToFile) 
	{
        HANDLE h;
        if (first) 
		{
            nvSprintf (nvStereoLog,"%s\\%s", STEREO_LOG_DIRECTORY, DEFAULT_LOG_NAME);
            CreateDirectory(STEREO_LOG_DIRECTORY, NULL);
            h = NvCreateFile(nvStereoLog, GENERIC_WRITE, 0,0, CREATE_ALWAYS,0,0);
            first = FALSE;
            NvCloseHandle (h); 
        }
        h = NvCreateFile(nvStereoLog, GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
        if (h != INVALID_HANDLE_VALUE) 
		{
            NvU32 dw;
            SetFilePointer (h,0,NULL,FILE_END); //appending
            NvWriteFile (h,str,nvStrLen(str),&dw,NULL);
            NvCloseHandle (h);
        }
    }
#endif
}
////////////////////////////////////////////////////////////////////////////


NVD3DTEXSTAGESTATE tssPrevState[2];
DWORD dwPreLaserState[NUM_STATES];

#ifdef LASERSTATECHANGE
DWORD dwSaveRenderState[D3D_RENDERSTATE_MAX+1];
BOOL  StatesWereSaved;
#endif  //LASERSTATECHANGE

void SetupLaserStates(PNVD3DCONTEXT pContext)
{
    HRESULT rval;

#ifdef LASERSTATECHANGE
    DWORD          dbgShowStateSave;
    if (StatesWereSaved)
    {
        //Check if anything changed.
        dbgShowStateSave = dbgShowState;
        dbgShowState |= NVDBG_SHOW_RENDER_STATE;
        for (DWORD i=0; i<D3D_RENDERSTATE_MAX+1; i++)
        {
            if (dwSaveRenderState[i] != pContext->dwRenderState[i])
            {
                // state value !changed
                dbgShowRenderStateChange (TRUE, i, pContext->dwRenderState[i]);
            }
        }
        dbgShowState = dbgShowStateSave;
    }
    StatesWereSaved = TRUE;
    nvMemCopy(dwSaveRenderState, pContext->dwRenderState, sizeof(dwSaveRenderState));
#endif  //LASERSTATECHANGE

    // set up state
    for (DWORD i = 0; i < NUM_STATES; i++)
    {
        dwPreLaserState[i] = pContext->dwRenderState[dwLaserStateSet[i].dwState];
        nvSetContextState(pContext, dwLaserStateSet[i].dwState, dwLaserStateSet[i].dwValue, &rval);
    }

    // set the logo texture
    nvMemCopy(tssPrevState, pContext->tssState, sizeof(tssPrevState));
    memset(&pContext->tssState[0], 0, sizeof(tssPrevState));
    pContext->tssState[0].dwValue[D3DTSS_TEXTUREMAP    ] = (DWORD)pLaser_Obj;
    pContext->tssState[0].dwValue[D3DTSS_COLOROP       ] = D3DTOP_SELECTARG1;
    pContext->tssState[0].dwValue[D3DTSS_COLORARG1     ] = D3DTA_TEXTURE;
    pContext->tssState[0].dwValue[D3DTSS_ALPHAOP       ] = D3DTOP_SELECTARG1;
    pContext->tssState[0].dwValue[D3DTSS_ALPHAARG1     ] = D3DTA_TEXTURE;
    pContext->tssState[0].dwValue[D3DTSS_ADDRESSU      ] = D3DTADDRESS_WRAP;
    pContext->tssState[0].dwValue[D3DTSS_ADDRESSV      ] = D3DTADDRESS_WRAP;
    pContext->tssState[0].dwValue[D3DTSS_MAGFILTER     ] = D3DTFG_LINEAR;
    pContext->tssState[0].dwValue[D3DTSS_MINFILTER     ] = D3DTFN_LINEAR;
    pContext->tssState[0].dwValue[D3DTSS_MIPFILTER     ] = D3DTFP_NONE;
    pContext->tssState[0].dwValue[D3DTSS_MAXANISOTROPY ] = 1;
    pContext->tssState[1].dwValue[D3DTSS_COLOROP       ] = D3DTOP_DISABLE;

    // make sure everything gets updated
#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
    pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
#endif
    NV_FORCE_TRI_SETUP(pContext);
}

void RestorePreLaserStates(PNVD3DCONTEXT pContext)
{
    HRESULT rval;

    // restore state
    for (DWORD i = 0; i < NUM_STATES; i++)
    {
        nvSetContextState(pContext, dwLaserStateSet[i].dwState, dwPreLaserState[i], &rval);
    }

    // restore texture stage state
    nvMemCopy(pContext->tssState, tssPrevState, sizeof(tssPrevState));

    // make sure everything gets updated
#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= celsiusDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
    pContext->hwState.dwDirtyFlags |= kelvinDirtyBitsFromTextureStageState[D3DTSS_TEXTUREMAP];
#endif
    NV_FORCE_TRI_SETUP(pContext);
}

void  CreateLaserTextureContent(void)
{
    if (!pLaser_Texture)
        return;

    HMODULE hMod = LoadLibrary("nvstres.dll");

    if (hMod)
    {
        DWORD           i,j;
        DWORD           dwWidth, dwHeight, dwColorKey;
        DWORD           *pMem;
        WORD            *pLaserImage, *pLaser, wColorKey;
        DWORD           lPitch;
        LPDDHALMODEINFO lpModeInfo;
        float           fWidth;

        GetLaserSight pGetSightFn = (GetLaserSight) GetProcAddress(hMod, "GetLaserSight");

        if(pGetSightFn) {
            if((pLaserImage = pGetSightFn(dwWidth, dwHeight, dwColorKey)) != NULL) {
                // We assume that the Laser Sight image is 64x64x16 and if the color is 0xFFDF
                //than it is transparent.
                nvAssert(dwWidth == LASER_WIDTH && dwHeight == LASER_HEIGHT);
                wColorKey = (WORD)(((dwColorKey & 0xF80000) >> 8) | ((dwColorKey & 0xFC00) >> 5) | ((dwColorKey & 0xF8) >> 3));
                pLaser = pLaserImage;
                lPitch = pLaser_Texture->getPitch() >> 2;
                lpModeInfo = &pDriverData->ModeList[pDXShare->dwModeNumber];
                fWidth = (D3DVALUE)(lpModeInfo->dwWidth);
                LaserMaxSize = fWidth/LaserMaxSizeDivider;
                LaserMinSize = fWidth/LaserMinSizeDivider;
                pLaser_Texture->updateLinearSurface();
                pMem = (DWORD *)pLaser_Texture->getLinear()->getAddress();
                for (i = 0; i < LASER_HEIGHT; i++)
                {
                    for (j = 0; j < LASER_WIDTH; j++)
                    {
                        if (pLaser[j] == wColorKey)
                            pMem[j] = 0;
                        else
                            pMem[j] = (((DWORD)pLaser[j] & 0xF800) << 8) | (((DWORD)pLaser[j] & 0x07E0) << 5) | (((DWORD)pLaser[j] & 0x001F) << 2) | Transparency;
                    }
                    pMem += lPitch;
                    pLaser += LASER_WIDTH;
                }
                pLaser_Texture->getLinear()->tagUpToDate();
                pLaser_Texture->getSwizzled()->tagOutOfDate();
                ReleaseLaserSight pReleaseSightFn = (ReleaseLaserSight) GetProcAddress(hMod, "ReleaseLaserSight");
                if(pReleaseSightFn)
                    pReleaseSightFn(pLaserImage);
            }
        }
        FreeLibrary(hMod);
    }
    return;
}

#if 0
void  ClearLaserSight(PNVD3DCONTEXT pContext)
{
    return;
    if (!STEREO_ACTIVATED)
        return;
    if (pStereoData->StereoSettings.dwFlags & STEREO_CLEAROCCURED)
    {
        pStereoData->StereoSettings.dwFlags &= ~STEREO_CLEAROCCURED;
        return;
    }
    //We need to clear the area underneath the laser sigth.
    if(pLaserDP2Command_Lcl)
    {
        nvAssert(0);
        FLATPTR SaveCommandBuf = pLaserDP2Command_Lcl->lpGbl->fpVidMem;
        pLaserDP2Command_Lcl->lpGbl->fpVidMem = (FLATPTR)&LaserDP2ClearCommand;

        LPDDHALMODEINFO lpModeInfo = &pDriverData->ModeList[pDXShare->dwModeNumber];
        LaserDP2ClearData.dwhContext = (DWORD)pContext;
        LaserDP2ClearCommand.Clear.Rects[0].left   = 0;
        LaserDP2ClearCommand.Clear.Rects[0].top    = 0;
        LaserDP2ClearCommand.Clear.Rects[0].bottom = lpModeInfo->dwHeight;
        LaserDP2ClearCommand.Clear.Rects[0].right  = lpModeInfo->dwWidth;
        LaserDP2ClearCommand.Clear.dwFillColor     = pContext->dwLastFillColor;
        nvDrawPrimitives2(&LaserDP2ClearData);
        pLaserDP2Command_Lcl->lpGbl->fpVidMem = SaveCommandBuf;
    } else
        nvAssert(0);
}
#endif
#endif  //LASER_SIGHT

#ifdef  FLAT_STAT
#define FLATARRAYSIZE 256
typedef struct
{
    D3DVALUE RHW;
    DWORD    dwCount;
} FLATOBJECTSTAT, *LPFLATOBJECTSTAT;

FLATOBJECTSTAT FlatObjects[FLATARRAYSIZE];
DWORD dwNFlatObjects;

void RegisterFlatObject(D3DVALUE RHW, DWORD dwCount)
{
    if (dwNFlatObjects < FLATARRAYSIZE)
    {
        FlatObjects[dwNFlatObjects].RHW     = RHW;
        FlatObjects[dwNFlatObjects].dwCount = dwCount;
        dwNFlatObjects++;
    } else
        nvAssert(0);
}
#endif  //FLAT_STAT

#ifdef  SNAP_SHOT

#include "NvImage.h"

#define NVSTEREO_IMAGE_SIGNATURE 0x4433564e //NV3D
typedef struct  _Nv_Stereo_Image_Header
{
    DWORD   dwSignature;
    DWORD   dwWidth;
    DWORD   dwHeight;
    DWORD   dwBPP;
    DWORD   dwSwapEyes;
} NVSTEREOIMAGEHEADER, *LPNVSTEREOIMAGEHEADER;

HOTKEY SaveStereoImage;
DWORD  JPEGCompressQuality;

void ConvertTo24(LPBYTE Dst, LPVOID Src, LPVOID SrcRight, DWORD nPixels, DWORD dwBPP)
{
    DWORD SrcCopy[3200];
    DWORD dwBytes = nPixels * dwBPP;
    DWORD dwTotalPixels = nPixels * 2;
    DWORD i;
    memcpy ((LPVOID)SrcCopy, SrcRight, dwBytes); //For faster access later
    memcpy ((LPVOID)((LPBYTE)SrcCopy + dwBytes), Src, dwBytes); //For faster access later
    switch (dwBPP)
    {
    case 2:
        //16 bit
        for (i = 0; i < dwTotalPixels; i++)
        {
            WORD Pixel = ((WORD *)&SrcCopy)[i];
            BYTE r,g,b;
            r = (Pixel >> 8) & 0xF8;
            g = (Pixel >> 3) & 0xFC;
            b = (Pixel << 3) & 0xF8;
            Dst[0] = r;
            Dst[1] = g;
            Dst[2] = b;
            Dst += 3;
        }
        break;

    case 4:
        //32 bit
        for (i = 0; i < dwTotalPixels; i++)
        {
            DWORD Pixel = ((DWORD *)&SrcCopy)[i];
            BYTE r,g,b;
            r = (Pixel >> 16) & 0xFF;
            g = (Pixel >> 8) & 0xFF;
            b = Pixel & 0xFF;
            Dst[0] = r;
            Dst[1] = g;
            Dst[2] = b;
            Dst += 3;
        }
        break;

    default:
        nvAssert(0);
    }
}

void SaveStereoImageFile(LPDDHAL_FLIPDATA pfd)
{
    char        FileName[256];
    DWORD       i;
    CreateDirectory(STEREO_IMG_DIRECTORY, NULL);
    nvSprintf (FileName,"%s\\%s*.jps", STEREO_IMG_DIRECTORY, pStereoData->bAppRealName);
    CNvObject* pNvObj = GET_PNVOBJ(pfd->lpSurfCurr);
    if (!pNvObj)
    {
        //Doesn't have a valid nvObj - bail out.
        LOG("SaveStereoImageFile: Surface doesn't have a valid nvObj");
		nvAssert(0);
        return;
    }

    WaitForIdle (TRUE,FALSE);

    CSimpleSurface *pSurf = pNvObj->getSimpleSurface();
    DWORD dwPitch = pSurf->getPitch();
    DWORD dwWidth = pSurf->getWidth();
    DWORD dwHeight = pSurf->getHeight();

    LPBYTE pBuf, pBufPtr;
    DWORD Scan24Size = dwWidth * 2 * 3;
    pBuf = pBufPtr = (LPBYTE)GlobalAlloc(GPTR, Scan24Size * dwHeight);
    LPVOID pAddr, pAddrRight;
    pAddr = (LPVOID)pSurf->getAddress(); //Left eye
    pAddrRight = (LPVOID)pSurf->getAddressRight(); //Right eye
    //We will write out one scanline at a time
    for (i = 0; i < dwHeight; i++)
    {
        ConvertTo24(pBufPtr, pAddr, pAddrRight, dwWidth, pSurf->getBPP());
        pBufPtr += Scan24Size;
        pAddr = (LPVOID)((LPBYTE)pAddr + dwPitch);
        pAddrRight = (LPVOID)((LPBYTE)pAddrRight + dwPitch);
    }
    LPCIMAGEOBJECT          pImageObject;
    HINSTANCE hImageDLL = LoadLibrary("nvImage.dll");
    if (hImageDLL)
    {
        CREATEIMAGEOBJECT CreateImageObject = (CREATEIMAGEOBJECT)GetProcAddress(hImageDLL, "CreateImageObject");
        if (!CreateImageObject(pImageObject))
        {
            pImageObject->SetJPEGParameters(JPEG_WRITE_QUALITY, JPEGCompressQuality);
            pImageObject->CompressJPEG((LPVOID)pBuf, dwWidth * 2, dwHeight, FileName);
            pImageObject->DestroyImageObject();
        }
        FreeLibrary(hImageDLL);
    }
    GlobalFree(pBuf);
}
#endif  //SNAP_SHOT

//ConfigAssistInfo.dwFlags fields
#define _2DSUSPECT                      0x00000002

#ifdef  STEREO_CONFIG_ASSIST

extern BOOL                 WriteRegistry(char *pSubKey, DWORD dwValue, BOOL DefaultFolder);
#define DeleteConfigInfo()          \
    WriteRegistry(NULL, 0, FALSE);  \
    ResetStereoParameters();

CONFIG_ASSIST_INFO ConfigAssistInfo;

void SuggestOptimalStereoSettings(void)
{
    DWORD dwFlags = pStereoData->StereoSettings.dwFlags;
    pStereoData->StereoSettings.dwFlags &= ~STEREO_CUSTOMSETTINGSMASK; //Erase all old flags
    float Divider = ConfigAssistInfo.rhwMax * pStereoData->StereoSettings.fStereoConvergence;
    if (Divider != 0)
        pStereoData->StereoSettings.fConvergenceMultiplier = 1.f/Divider;
    else
        pStereoData->StereoSettings.fConvergenceMultiplier = 0.0f;
    pStereoData->StereoSettings.dwFlags |= STEREO_CONVERGENCEMULTIPLIER;
    if (dwFlags & STEREO_FAVORSZOVERRHW)
    {
        pStereoData->StereoSettings.dwFlags |= STEREO_FAVORSZOVERRHW;
        if (ConfigAssistInfo.szMax2D != -9999.f)
        {
            if (!(dwFlags & STEREO_RHWGREATERATSCREEN))
            {
                pStereoData->StereoSettings.fRHWGreaterAtScreen = ConfigAssistInfo.szMax2D;
            }
            pStereoData->StereoSettings.dwFlags |= STEREO_RHWGREATERATSCREEN;
        }

        if (dwFlags & STEREO_RHWLESSATSCREEN)
        {
            pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
        } else
        {
            if (ConfigAssistInfo.szMin2D != 9999.f)
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
                pStereoData->StereoSettings.fRHWLessAtScreen = ConfigAssistInfo.szMin2D;
            }
        }
    } else
    {
        if (ConfigAssistInfo.rhwMin2D != 9999.f)
        {
            if (!(dwFlags & STEREO_RHWGREATERATSCREEN))
            {
                pStereoData->StereoSettings.fRHWGreaterAtScreen = ConfigAssistInfo.rhwMin2D;
            }
            pStereoData->StereoSettings.dwFlags |= STEREO_RHWGREATERATSCREEN;
        }

        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
        {
            pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
        } else
        {
#if 1
            if (ConfigAssistInfo.rhwMax2D != -9999.f)
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
                pStereoData->StereoSettings.fRHWLessAtScreen = ConfigAssistInfo.rhwMax2D;
            }
#endif
        }
    }
    ConfigAssistInfo.rhwMin     = 9999.f;
    ConfigAssistInfo.rhwMin2D   = 9999.f;
    ConfigAssistInfo.rhwMax     = -9999.f;
    ConfigAssistInfo.rhwMax2D   = -9999.f;
    ConfigAssistInfo.szMin      = 9999.f;
    ConfigAssistInfo.szMin2D    = 9999.f;
    ConfigAssistInfo.szMax      = -9999.f;
    ConfigAssistInfo.szMax2D    = -9999.f;
}

struct _StereoSettingsStack {
    BOOL            Valid;
    STEREOSETTINGS  StereoSettings;
} StereoSettingsStack;

BOOL PushStereoSettings(void)
{
    if (StereoSettingsStack.Valid)
        //No room to save
        return FALSE;
    StereoSettingsStack.StereoSettings  = pStereoData->StereoSettings;
    StereoSettingsStack.Valid           = TRUE;
    return TRUE;
}

BOOL PopStereoSettings(void)
{
    if (!StereoSettingsStack.Valid)
        //No valid stack
        return FALSE;
    pStereoData->StereoSettings = StereoSettingsStack.StereoSettings;
    StereoSettingsStack.Valid   = FALSE;
    return TRUE;
}

void SuggestStereoSettings(void)
{
    if (PushStereoSettings())
    {
        SuggestOptimalStereoSettings();
    }
}

void UnsuggestStereoSettings(void)
{
    PopStereoSettings();
}

void ConfigFlush()
{
    DWORD dw;
    DWORD nBytes = nvStrLen(ConfigAssistInfo.Buf);
    if (!WriteFile(ConfigAssistInfo.FileHandle,ConfigAssistInfo.Buf,nBytes,&dw,0)) {
        LOG("ConfigFlush: can't write to file");
        DWORD dw=GetLastError();
        __asm {
            mov eax,[dw]
            int 3
        }

    }
    FlushFileBuffers (ConfigAssistInfo.FileHandle);
    ConfigAssistInfo.Buf[0] = 0;
}

#include <time.h>

void DumpConfigInfo(void)
{
    char TmpBuf[1024];
    CreateDirectory(STEREO_LOG_DIRECTORY, NULL);
    nvSprintf (TmpBuf,"%s\\%s", STEREO_LOG_DIRECTORY, pStereoData->bAppRealName);
    ConfigAssistInfo.FileHandle=CreateFile(TmpBuf,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (ConfigAssistInfo.FileHandle==INVALID_HANDLE_VALUE) {
        LOG("DumpConfigInfo: invalid file handle, file %s", TmpBuf);
        MessageBeep (0);
		__asm int 3;
        return;
    }

    if (!(pStereoData->StereoSettings.dwFlags & STEREO_CUSTOMSETTINGSMASK))
    {
        //Nothing has been customized yet. We will do the automatic setting calculation
        //and then procede with saving it.
        SuggestOptimalStereoSettings();
    }

    if (pStereoData->dwHWTnL)
        nvStrCpy(TmpBuf,"HW TnL");
    else
        nvStrCpy(TmpBuf,"SW TnL");
    nvSprintf (ConfigAssistInfo.Buf,"AppName=%s (%s)\n",pStereoData->bAppName, TmpBuf);
    nvSprintf (ConfigAssistInfo.Buf,"AppRealName=%s (%s)\n",pStereoData->bAppRealName, TmpBuf);

    nvSprintf (TmpBuf,"StereoConvergenceMultiplier=%f\n",pStereoData->StereoSettings.fConvergenceMultiplier);
    nvStrCat (ConfigAssistInfo.Buf, TmpBuf);

    ConfigFlush();

    if (pStereoData->StereoSettings.fStereoSeparation != DEFAULT_STEREO_SEPARATION)
    {
        nvSprintf (ConfigAssistInfo.Buf,"StereoSeparation=%f\n",pStereoData->StereoSettings.fStereoSeparation);
        WriteRegistry(NV_REG_STEREO_SEPARATION, *(DWORD *)&pStereoData->StereoSettings.fStereoSeparation, TRUE);
        ConfigFlush();
    }

    if (STEREODATA(StereoSettings.dwFlags) & STEREO_GAMMACORRECTION && 
		pStereoData->StereoSettings.fStereoGamma != 1.0f)
    {
        nvSprintf (ConfigAssistInfo.Buf,"StereoGamma=%f\n",pStereoData->StereoSettings.fStereoGamma);
        WriteRegistry("StereoGamma", *(DWORD *)&pStereoData->StereoSettings.fStereoGamma, TRUE);
        ConfigFlush();
    }

    if (pStereoData->StereoSettings.fStereoConvergence != DEFAULT_STEREO_CONVERGENCE)
    {
        nvSprintf (ConfigAssistInfo.Buf,"StereoConvergence=%f\n",pStereoData->StereoSettings.fStereoConvergence);
        nvSprintf (TmpBuf,"MinRHW=%f (farthest)\n",ConfigAssistInfo.rhwMin);
        nvStrCat (ConfigAssistInfo.Buf, TmpBuf);
        nvSprintf (TmpBuf,"MaxRHW=%f (closest)\n",ConfigAssistInfo.rhwMax);
        nvStrCat (ConfigAssistInfo.Buf, TmpBuf);
        WriteRegistry(NV_REG_STEREO_CONVERGENCE, *(DWORD *)&pStereoData->StereoSettings.fStereoConvergence, FALSE);

        ConfigFlush();
    }

    float fConvergenceMultiplier = pStereoData->StereoSettings.fConvergenceMultiplier;
    nvSprintf (ConfigAssistInfo.Buf,"Suggested StereoConvergenceMultiplier=%f\n",fConvergenceMultiplier);

    ConfigFlush();

    D3DVALUE fSuggestedPostStereoConvergence = pStereoData->StereoSettings.fStereoConvergence
                                             * fConvergenceMultiplier;
    D3DVALUE fRHWInfinity = 0.245f/(pStereoData->StereoSettings.fScreenStereoSeparation*fSuggestedPostStereoConvergence);
    nvSprintf (ConfigAssistInfo.Buf,"Suggested Infinity RHW=%f\n",fRHWInfinity);

    if (fRHWInfinity > ConfigAssistInfo.rhwMin)
    {
        //Part of Z range is at infinity and beyond!!!
        D3DVALUE fPercent = (fRHWInfinity - ConfigAssistInfo.rhwMin)/(ConfigAssistInfo.rhwMax - ConfigAssistInfo.rhwMin);
        nvSprintf (TmpBuf,"Warning!!! %f of Z range is at infinity and beyond!!!\n", fPercent);
        nvStrCat (ConfigAssistInfo.Buf, TmpBuf);
    }

    if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
    {
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
        {
            nvSprintf (TmpBuf,"RHWGreaterAtScreen=%f\n", pStereoData->StereoSettings.fRHWGreaterAtScreen);
            nvStrCat (ConfigAssistInfo.Buf, TmpBuf);
            WriteRegistry(NV_REG_STEREO_RHWGREATERATSCREEN, *(DWORD *)&pStereoData->StereoSettings.fRHWGreaterAtScreen, FALSE);
            ConfigFlush();
        }

        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
        {
            WriteRegistry(NV_REG_STEREO_RHWLESSATSCREEN, *(DWORD *)&pStereoData->StereoSettings.fRHWLessAtScreen, FALSE);
        }
    } else
    {
        if (ConfigAssistInfo.rhwMin2D != 9999.f)
        {
            nvSprintf (TmpBuf,"Farthest object with equal RHW is at %f\n", ConfigAssistInfo.rhwMin2D);
            nvStrCat (ConfigAssistInfo.Buf, TmpBuf);
        }
        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN)
        {
            nvSprintf (TmpBuf,"RHWGreaterAtScreen=%f\n", pStereoData->StereoSettings.fRHWGreaterAtScreen);
            nvStrCat (ConfigAssistInfo.Buf, TmpBuf);
            WriteRegistry(NV_REG_STEREO_RHWGREATERATSCREEN, *(DWORD *)&pStereoData->StereoSettings.fRHWGreaterAtScreen, FALSE);
        }

        ConfigFlush();

        if (pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN)
        {
            WriteRegistry(NV_REG_STEREO_RHWLESSATSCREEN, *(DWORD *)&pStereoData->StereoSettings.fRHWLessAtScreen, FALSE);
        }
    }

    ConfigFlush();

    if (pStereoData->StereoSettings.dwFlags & STEREO_AUTOCONVERGENCE)
    {
        WriteRegistry(NV_REG_STEREO_AUTOCONVERGENCE, 1, FALSE);
        WriteRegistry(NV_REG_STEREO_CONVERGENCEADJUSTPACE, *(DWORD *)&(pStereoData->StereoSettings.AutoConvergenceAdjustPace), FALSE);
    } else
        WriteRegistry(NV_REG_STEREO_AUTOCONVERGENCE, 0, FALSE);

    WriteRegistry(NV_REG_STEREO_FRUSTUMADJUSTMODE, pStereoData->StereoSettings.FrustumAdjustMode, FALSE);

#ifdef  LASER_SIGHT
    if (pStereoData->StereoSettings.dwFlags & STEREO_LASERSIGHT)
    {
        WriteRegistry(NV_REG_STEREO_LASERXADJUST, *(DWORD *)&pStereoData->StereoSettings.fLaserXAdjust, FALSE);
        WriteRegistry(NV_REG_STEREO_LASERYADJUST, *(DWORD *)&pStereoData->StereoSettings.fLaserYAdjust, FALSE);
    }
#endif  //LASER_SIGHT

    time_t CurrentTime;
    time(&CurrentTime);
    WriteRegistry("Time", CurrentTime, FALSE);
    CLOSE_HANDLE(ConfigAssistInfo.FileHandle);

    ResetStereoParameters();
}

#endif  //STEREO_CONFIG_ASSIST

BOOL WriteRegistry(char *pSubKey, DWORD dwValue, BOOL DefaultFolder)
{
    BOOL RVal = FALSE;
    HKEY hKey;
    HKEY hKeyApp;
    char StereoKey[256];

    nvStrCpy(StereoKey, NV4_REG_GLOBAL_BASE_PATH);
    nvStrCat(StereoKey, "\\");
    nvStrCat(StereoKey, NV4_REG_STEREO_SUBKEY);

    if (DefaultFolder)
    {
        RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, StereoKey, 0, KEY_ALL_ACCESS, &hKey);
        if (RegSetValueEx(hKey, pSubKey, NULL, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS)
            RVal = TRUE;
        RegCloseKey(hKey);
        return RVal;
    }

    nvStrCat(StereoKey, "\\GameConfigs");

    if (RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, StereoKey, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
    {
        LOG("WriteRegistry: can't open %s", NV4_REG_GLOBAL_BASE_KEY);
		nvAssert(0);
        return RVal;
    }
    if (pSubKey == NULL)
    {
        //Request to delete the game config entry. We will make it empty by deleting and then creating empty
        RegDeleteKey(hKey, pStereoData->bAppRealName);
    }
    if (RegCreateKeyEx(hKey, pStereoData->bAppRealName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyApp, NULL) != ERROR_SUCCESS)
    {
        LOG("WriteRegistry: can't create key %s", pStereoData->bAppRealName);
        nvAssert(0);
        RegCloseKey(hKey);
        return RVal;
    }
    RegCloseKey(hKey);
    if (pSubKey)
    {
        if (RegSetValueEx(hKeyApp, pSubKey, NULL, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS)
            RVal = TRUE;
    }
    RegCloseKey(hKeyApp);
    return RVal;
}

void StereoRightEyeSurfaceCopy(CSimpleSurface *pSurf)
{
    DWORD pSrc = (DWORD)pSurf->getAddress();
    DWORD pDst = (DWORD)pSurf->getAddressRight();
    if (!(pSrc | pDst))
        return;
    DWORD dwPitch = pSurf->getPitch();
    DWORD dwHeight = pSurf->getHeight();
    DWORD dwWidthBytes = pSurf->getWidth() * pSurf->getBPP();
    for (DWORD i = 0; i <= dwHeight; i++, pSrc += dwPitch, pDst += dwPitch)
        nvMemCopy(pDst, pSrc, dwWidthBytes);
}

void StereoRightEyeInit()
{
    CSimpleSurface *pSurf;
    for (DWORD i=0; i<3; i++)
    {
        pSurf = pStereoData->pRenderTargets[i];
        if (pSurf)
            StereoRightEyeSurfaceCopy(pSurf);
    }
    pSurf = ((PNVD3DCONTEXT)(pStereoData->dwContext))->pZetaBuffer;
    if (pSurf)
        StereoRightEyeSurfaceCopy(pSurf);
}

DWORD __stdcall StereoSetExclusiveMode32(LPDDHAL_SETEXCLUSIVEMODEDATA lpSetExclusiveMode)
{
    nvSetDriverDataPtrFromDDGbl (lpSetExclusiveMode->lpDD);
    if (STEREO_ENABLED)
    {
        if (lpSetExclusiveMode->dwEnterExcl)
        {
            pStereoData->StereoSettings.dwFlags &= ~STEREO_DISALLOWED_NOT_FULL_SCREEN;
#if 0
            if (pStereoData->StereoSettings.dwFlags & STEREO_FORCEDTOGDI)
            {
                nvAssert(0);
                pStereoData->StereoSettings.dwFlags &= ~STEREO_FORCEDTOGDI;
                ActivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
            }
#endif
        }
        else
        {
            if (pStereoData->dwContext)
            {
#if 0
                //Shouldn't be at this point. It happens on Kawasaki ATV Demo when
                //DX7 runtime fails to call our nvContextDestroy and then everything
                //goes wrong. We don't know what this game does to cause such a behaviour
                //but will try our best to minimize the negative effect.
                D3DHAL_CONTEXTDESTROYDATA ContextDestroyData;
                ContextDestroyData.dwhContext = pStereoData->dwContext;
                ContextDestroyData.ddrval     = 0;
                nvContextDestroy(&ContextDestroyData);
#else
                //nvAssert(0);
                //Potentially dangerous situation. At least we want to make sure that
                //stereo is turned off for time being.
                if (STEREO_ACTIVATED)
                {
                    //We need to turn stereo off
                    pStereoData->StereoSettings.dwFlags |= STEREO_FORCEDTOGDI;
                    DeactivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
                }
#endif
            }
            pStereoData->StereoSettings.dwFlags |= STEREO_DISALLOWED_NOT_FULL_SCREEN;
        }
    }
    lpSetExclusiveMode->ddRVal = DD_OK;
    NvReleaseSemaphore(pDriverData);

    // call the original handler
    SetExclusiveMode32(lpSetExclusiveMode);

    return DDHAL_DRIVER_HANDLED;
}

//---------------------------------------------------------------------------

/*
 * StereoFlipToGDISurface32 DDHAL callback
 */
DWORD __stdcall StereoFlipToGDISurface32 (LPDDHAL_FLIPTOGDISURFACEDATA lpFlipToGDISurface)
{
    lpFlipToGDISurface->ddRVal = DD_OK;
    nvSetDriverDataPtrFromDDGbl (lpFlipToGDISurface->lpDD);
    if (lpFlipToGDISurface->dwToGDI)
    {
        //Flip to GDI
        if (STEREO_ACTIVATED)
        {
            //We need to turn stereo off
            pStereoData->StereoSettings.dwFlags |= STEREO_FORCEDTOGDI;
            DeactivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
        }
    } else
    {
        //Flip from GDI
        //We need to turn stereo on if previously was forced to GDI
        if (pStereoData->StereoSettings.dwFlags & STEREO_FORCEDTOGDI)
        {
            pStereoData->StereoSettings.dwFlags &= ~STEREO_FORCEDTOGDI;
            ActivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
        }
    }

    NvReleaseSemaphore(pDriverData);

    // call the original handler
    return DDHAL_DRIVER_NOTHANDLED;
}

void StereoModeSetProcessing(void)
{
#if 0
    //********************************************************************************
    //* Would be nice to do something like below. But we can't manipulate stereo since
    //* this function gets called from 16 bit environment. At least we will keep an eye
    //* on it. Maybe some subset of stereo manipulation functionality can and has to be
    //* done here.
    //********************************************************************************
    NvSetStereoDataPtrFromDriverData;
    if (!STEREO_ENABLED || !pStereoData->StereoModeNumber)
    {
        //Nothing to be worried about.
        return;
    }
    //Mode switch while stereo is active causes a lot of problems, so we want to make
    //sure that stereo is on only when the mode that has been set matches that one
    //in which stereo had been initially activated. This logic doesn't imply any serious
    //restrictions because usually games switch video mode when they go into a stage like
    //introductory screens, advertisement videos and so on. We don't need stereo for those
    //stages anyways. It yet remains to be seen if this logic is flawed.
    if (STEREO_ACTIVATED)
    {
        if (pStereoData->StereoModeNumber != pDXShare->dwModeNumber)
        {
            //Turn stereo off
			LOG("StereoModeSetProcessing: video mode has changed, deactivating stereo");
            nvAssert(0);
            DeactivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
        }
    } else
    {
        if (pStereoData->StereoModeNumber == pDXShare->dwModeNumber
            && !(pStereoData->StereoSettings.dwFlags & (STEREO_FORCEDOFF|STEREO_FORCEDTOGDI|STEREO_DISALLOWED)))
        {
			LOG("StereoModeSetProcessing: going back to stereo");
            //Turn stereo back on
            nvAssert(0);
            ActivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
        }
    }
#endif
}

#ifdef  STEREO_VIEWER_DRV
DWORD CreateStereoViewer(LPBYTE pStereoViewerDrvName)
{
    LPCREATESTEREOVIEWER  pCreateStereoViewer;
    STEREOCAPS            StereoCaps;
    DWORD                 dwStereoFormatBits;

    pStereoData->dwStereoFormat = SVF_NONE;
    if (!(pStereoData->hStereoDrv = LoadLibrary((const char *)pStereoViewerDrvName)))
        return 1;
    if (!(pCreateStereoViewer = (LPCREATESTEREOVIEWER)GetProcAddress(pStereoData->hStereoDrv, "CreateStereoViewer")))
        goto ErrorExitFreeLib;
    if ((*pCreateStereoViewer)(&pStereoData->pStereoViewer) != STEREO_OK)
        goto ErrorExitFreeLib;
    StereoCaps.dwSize = sizeof (STEREOCAPS);
    StereoCaps.dwFlags = STEREOCAPS_SUPPORTEDFORMATS;
    StereoCaps.dwSupportedFormats = UPTODATE_STEREO_SUPPORT;
    if (pStereoData->pStereoViewer->GetCaps(&StereoCaps) != STEREO_OK)
        goto ErrorExitFreeViewer;
    if (StereoCaps.dwSize != sizeof (STEREOCAPS))
        goto ErrorExitFreeViewer;
    if (!(StereoCaps.dwSupportedFormats & UPTODATE_STEREO_SUPPORT))
        goto ErrorExitFreeViewer;
    if (StereoCaps.dwPreferredFormat & UPTODATE_STEREO_SUPPORT)
        dwStereoFormatBits = StereoCaps.dwPreferredFormat;
    else
        dwStereoFormatBits = StereoCaps.dwSupportedFormats & UPTODATE_STEREO_SUPPORT;
    //Parse the bits in some priority order to avoid the case when we have multiple bits.
    if (dwStereoFormatBits & SVF_OVERANDUNDER)
        pStereoData->dwStereoFormat = SVF_OVERANDUNDER;
    else
    {
        if (dwStereoFormatBits & SVF_FRAMESEQUENTIAL)
            pStereoData->dwStereoFormat = SVF_FRAMESEQUENTIAL;
    }
#if 0
    __asm int 3
    //To force Over and Under for debugging
    pStereoData->dwStereoFormat = SVF_OVERANDUNDER;
#endif
    pStereoData->StereoViewerCaps = StereoCaps;

    return 0;

ErrorExitFreeViewer:
    pStereoData->pStereoViewer->DestroyStereoViewer();

ErrorExitFreeLib:
    FreeLibrary(pStereoData->hStereoDrv);
    pStereoData->hStereoDrv = NULL;
    return 1;
}
#endif  //STEREO_VIEWER_DRV

void StereoModifyDDHALInfo(void)
{
    if (!STEREO_ENABLED)
        return;
    pDriverData->DDCallbacks.FlipToGDISurface      = (LPDDHAL_FLIPTOGDISURFACE)StereoFlipToGDISurface32;
    pDriverData->DDCallbacks.SetExclusiveMode      = StereoSetExclusiveMode32;
    pDriverData->DDCallbacks.dwFlags              |= DDHAL_CB32_FLIPTOGDISURFACE | DDHAL_CB32_SETEXCLUSIVEMODE;
}

void LoadStereoRegistry(void)
{
    HKEY hKey;
    char StereoKey[256];
    DWORD   dwNameSize;
    DWORD   dwNameType;
    float fMonitorSize;

    nvStrCpy(StereoKey, NV4_REG_GLOBAL_BASE_PATH);
    nvStrCat(StereoKey, "\\");
    nvStrCat(StereoKey, NV4_REG_STEREO_SUBKEY);

    if (RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, StereoKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        if (RegQueryValueEx(hKey, NV_REG_STEREO_ENABLED, NULL, &dwType, (LPBYTE)&lValue, &dwSize) != ERROR_SUCCESS)
        {
            //Don't even look further.
            goto LSR_ExitWithClose;
        }
        if (lValue == 1)
        {
            if (STEREO_ENABLED)
            {
                //The registry settings might have changed since the last run. We have to reload them.
                pStereoData->dwLastEye = EYE_NONE;
                pStereoData->dwHWTnL = 0;
                pStereoData->StereoSettings.dwFlags = 0;
                pStereoData->StereoModeNumber = 0;
                pDriverData->pStereoData = 0;
                goto LSR_ReloadRegistry;
            }
        } else
        {
            if (STEREO_ENABLED)
            {
                //We don't want stereo any more but we had it before.
                DestroyStereo();
            }
            goto LSR_ExitWithClose;
        }
        if (!(pStereoData = (LPSTEREODATA)AllocIPM(sizeof(STEREODATA))))
        {
            goto LSR_ExitWithClose;
        }
        memset(pStereoData, 0, sizeof(STEREODATA));
LSR_ReloadRegistry:
#ifdef  STEREO_CONFIG_ASSIST
        ConfigAssistInfo.dwFlags = 0;
#endif  //STEREO_CONFIG_ASSIST
        //Next setting is default. We expect to get call to SetExclusiveMode callback and only if an
        //application goes full screen we will allow to use stereo;
        pStereoData->StereoSettings.dwFlags |= STEREO_DISALLOWED_NOT_FULL_SCREEN;
#ifdef  STEREO_VIEWER_DRV
        //Look for the stereo viewer driver first.
        dwNameSize = sizeof(pStereoData->StereoViewerName);
        dwNameType = REG_SZ;
        if (RegQueryValueEx(hKey, NV_REG_STEREO_STEREOVIEWER, NULL, &dwNameType, (LPBYTE)pStereoData->StereoViewerName, &dwNameSize) != ERROR_SUCCESS)
        {
            FreeIPM(pStereoData);
            goto LSR_ExitWithClose;
        }
        if (CreateStereoViewer(pStereoData->StereoViewerName))
        {
            FreeIPM(pStereoData);
            goto LSR_ExitWithClose;
        }
#else   //STEREO_VIEWER_DRV==0
        pStereoData->dwStereoHWType = STEREOCFG_ELSADDC; //Default type.
        if (RegQueryValueEx(hKey, NV_REG_STEREO_STEREOVIEWERTYPE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue & 0x80000000)
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_ANAGLYPH;
                lValue &= 0x7FFFFFFF;
            }
            pStereoData->dwStereoHWType = lValue;   //Stereo Viewer HW type.
        }
        pStereoData->dwLineCodeColor = RGB(255, 0, 0); //Blue by default
        if (RegQueryValueEx(hKey, "LineCodeColor", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->dwLineCodeColor = lValue;   //Stereo Viewer HW type.
        }
		if (pStereoData->StereoSettings.dwFlags & STEREO_ANAGLYPH)
		{
			STEREODATA(dwLeftAnaglyphFilter)  = 0xFFFF0000;	//Red
			STEREODATA(dwRightAnaglyphFilter) = 0xFF00FFFF;	//Cyan
			//Get the filter masks for the eyes
			if (RegQueryValueEx(hKey, "LeftAnaglyphFilter", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
			{
				STEREODATA(dwLeftAnaglyphFilter) = lValue;   //Stereo Viewer HW type.
			}
			if (RegQueryValueEx(hKey, "RightAnaglyphFilter", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
			{
				STEREODATA(dwRightAnaglyphFilter) = lValue;   //Stereo Viewer HW type.
			}
		}
#endif  //STEREO_VIEWER_DRV
        pDriverData->pStereoData = (DWORD)pStereoData; //From now on STEREO_ENABLED is set.
        pStereoData->bStereoActivated = FALSE;
        DWORD dwDummy;
        NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                   NV_CFG_STEREO_CONFIG, 0, &dwDummy);
        pStereoData->RmStereoParams.Flags = 0;
        pStereoData->RmStereoParams.FlipOffsets[0][0] =
        pStereoData->RmStereoParams.FlipOffsets[1][0] =
        pStereoData->RmStereoParams.FlipOffsets[2][0] =
        pStereoData->RmStereoParams.FlipOffsets[3][0] = 0xFFFFFFFF;

        NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
               NV_CFG_STEREO_CONFIG, (DWORD)&pStereoData->RmStereoParams, &dwDummy);
        //Now we can handle the remaining registry stuff like hotkeys and adjustments.
        if (RegQueryValueEx(hKey, NV_REG_STEREO_DEFAULTON, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
                pStereoData->StereoSettings.dwFlags |= STEREO_DEFAULTON;
        }
        pStereoData->StereoSettings.fStereoSeparation  = DEFAULT_STEREO_SEPARATION;
        if (RegQueryValueEx(hKey, NV_REG_STEREO_SEPARATION, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.fStereoSeparation = *(float *)&lValue;
            pStereoData->StereoSettings.dwFlags |= STEREO_SEPARATION;
        }

        pStereoData->StereoSettings.fStereoGamma = 1.0f;
        if (RegQueryValueEx(hKey, "StereoGamma", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.fStereoGamma = *(float *)&lValue;
			STEREODATA(StereoSettings.dwFlags) |= STEREO_GAMMACORRECTION;
        }

        pStereoData->StereoSettings.fStereoConvergence = DEFAULT_STEREO_CONVERGENCE;
        if (RegQueryValueEx(hKey, NV_REG_STEREO_DISABLE_TnL, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_DISABLETnL;
                getDC()->nvD3DDevCaps.dd1Caps.dwDevCaps                   &= ~D3DDEVCAPS_HWTRANSFORMANDLIGHT;
                getDC()->nvD3DDevCaps.dd1Caps.dtcTransformCaps.dwSize      = 0;
                getDC()->nvD3DDevCaps.dd1Caps.dlcLightingCaps.dwSize       = 0;
            }
        }

        pStereoData->StereoSettings.fConvergenceMultiplier = DEFAULT_STEREO_CONVERGENCE_MULTIPLIER;
        if (RegQueryValueEx(hKey, NV_REG_STEREO_CONVERGENCEMULTIPLIER, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.fConvergenceMultiplier = *(float *)&lValue;
            pStereoData->StereoSettings.dwFlags |= STEREO_CONVERGENCEMULTIPLIER;
        }

        fMonitorSize = 16.f;  //17" is default and we assume 16" - viewable area
        if (RegQueryValueEx(hKey, NV_REG_STEREO_MONITORSIZE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            fMonitorSize = ((float)lValue) * 0.5f - 1.f; //Assume that viewable = diagonal - 1
        }
        //Now we'll calculate the MAX_STEREO_SEPARATION based on the following assumptions:
        // 1) The monitor aspect ratio is 4x3. So the horizontal size is fMonitorSize * 0.8.
        // 2) The average distance between eyes is 62.4 mm.
        //MAX_STEREO_SEPARATION - is percentage of the horizontal representing the distance
        //between eyes. Here we go:
        MAX_STEREO_SEPARATION = (62.4f / (0.8f * 25.4f)) / fMonitorSize;

#ifdef  LASER_SIGHT
        if (RegQueryValueEx(hKey, NV_REG_STEREO_LASERSIGHTENABLED, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_LASERSIGHTENABLED;
                dwNameSize = sizeof(pStereoData->LaserSightFile);
                dwNameType = REG_SZ;
                if (RegQueryValueEx(hKey, NV_REG_STEREO_LASERSIGHTFILE, NULL, &dwNameType, (LPBYTE)pStereoData->LaserSightFile, &dwNameSize) != ERROR_SUCCESS)
                {
                    DWORD i;
                    i = GetSystemDirectory((char *)pStereoData->LaserSightFile, sizeof (pStereoData->LaserSightFile));
                    pStereoData->LaserSightFile[i] = 0;
                    nvStrCat((char *)pStereoData->LaserSightFile, "\\Laser.bmp");
                }
                if (RegQueryValueEx(hKey, NV_REG_STEREO_LASERSIGHTPROPERTY, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                    ProcessLaserSightProperty(lValue);
#ifdef  STEREO_CONFIG_ASSIST
                if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_LASER_X_PLUS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    ConfigAssistInfo.LaserXPlusKey.dwValue = lValue;   //The hot key to increase the laser sight X position
                }
                if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_LASER_X_MINUS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    ConfigAssistInfo.LaserXMinusKey.dwValue = lValue;   //The hot key to decrease the laser sight X position
                }
                if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_LASER_Y_PLUS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    ConfigAssistInfo.LaserYPlusKey.dwValue = lValue;   //The hot key to increase the laser sight Y position
                }
                if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_LASER_Y_MINUS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    ConfigAssistInfo.LaserYMinusKey.dwValue = lValue;   //The hot key to decrease the laser sight Y position
                }
#endif  //STEREO_CONFIG_ASSIST
            }
        }
#endif  //LASER_SIGHT

#ifdef  USE_POPUP_LIST  
		
        //check global flag, whether pop-ups enabled at all
	    if (RegQueryValueEx(hKey, NV_REG_STEREO_POPUP_ENABLED, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
            {
                popupAgents.enabled() = true;
                //read options, e.g. enable/disable/params for pop-ups like FPS, lasersight, nvLogo, chat etc.
                popupAgents.getOptions();
            }
        }

#endif  //USE_POPUP_LIST

        pStereoData->StereoSettings.AutoConvergenceAdjustPace = 0.05f; //Default
        if (RegQueryValueEx(hKey, NV_REG_STEREO_AUTOCONVERGENCE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_AUTOCONVERGENCE;
                if (RegQueryValueEx(hKey, NV_REG_STEREO_CONVERGENCEADJUSTPACE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    pStereoData->StereoSettings.AutoConvergenceAdjustPace = *(float *)&lValue;
                }
            }
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_TOGGLE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoKeys.StereoToggle.dwValue = lValue;   // The hot key to toggle stereo.
        }

#ifdef  STEREO_CONFIG_ASSIST
        if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_SUGGESTSETTINGS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.SuggestKey.dwValue = lValue;   //The hot key to automatically set suggested stereo settings.
        }
        if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_UNSUGGESTSETTINGS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.UnsuggestKey.dwValue = lValue;   //The hot key to restore stereo settings before suggestion.
        }
        if (RegQueryValueEx(hKey, "WriteConfig", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.WriteKey.dwValue = lValue;   // The hot key to dump stereo config info.
        }
        if (RegQueryValueEx(hKey, "DeleteConfig", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.DeleteKey.dwValue = lValue;   // The hot key to remove stereo config info.
        }
        if (RegQueryValueEx(hKey, "ToggleAutoConvergence", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.ToggleAutoConvergence.dwValue = lValue;   // The hot key to dump stereo config info.
        }
        if (RegQueryValueEx(hKey, "ToggleAutoConvergenceRestore", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.ToggleAutoConvergenceRestore.dwValue = lValue;   // The hot key to dump stereo config info.
        }
        if (RegQueryValueEx(hKey, "RHWAtScreenLess", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.RHWAtScreenLess.dwValue = lValue;   // The hot key to dump stereo config info.
        }
        if (RegQueryValueEx(hKey, "RHWAtScreenMore", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.RHWAtScreenMore.dwValue = lValue;   // The hot key to dump stereo config info.
        }
        if (RegQueryValueEx(hKey, "RHWLessAtScreenLess", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.RHWLessAtScreenLess.dwValue = lValue;   // The hot key to dump stereo config info.
        }
        if (RegQueryValueEx(hKey, "RHWLessAtScreenMore", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.RHWLessAtScreenMore.dwValue = lValue;   // The hot key to dump stereo config info.
        }
        if (RegQueryValueEx(hKey, "CycleFrustumAdjust", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.FrustumAdjust.dwValue = lValue;  // The hot key to remove stereo config info.
        }
        if (RegQueryValueEx(hKey, "GammaAdjustMore", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.GammaAdjustMore.dwValue = lValue;  // The hot key to increase gamms.
        }
        if (RegQueryValueEx(hKey, "GammaAdjustLess", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            ConfigAssistInfo.GammaAdjustLess.dwValue = lValue;  // The hot key to decrease gamms.
        }
#endif  //STEREO_CONFIG_ASSIST
#ifdef  SNAP_SHOT
        if (RegQueryValueEx(hKey, "SaveStereoImage", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            SaveStereoImage.dwValue = lValue;   // The hot key to save stereo image file
        }
        JPEGCompressQuality = 75;
        if (RegQueryValueEx(hKey, "SnapShotQuality", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            JPEGCompressQuality = lValue;   // Quality of JPEG compression. 0-100, 100 being the best.
        }
#endif  //SNAP_SHOT
        if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_SEPARATIONADJUST_MORE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoKeys.StereoSeparationAdjustMore.dwValue = lValue;   // The hot key to increase the horizontal separation.
        } else
        {
            //Should be eventually removed.
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_HORIZONTALADJUST_MORE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoSeparationAdjustMore.dwValue = lValue;   // The hot key to increase the horizontal separation.
            }
        }
        if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_SEPARATIONADJUST_LESS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoKeys.StereoSeparationAdjustLess.dwValue = lValue;   // The hot key to reduce the horizontal separation.
        } else
        {
            //Should be eventually removed.
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_HORIZONTALADJUST_LESS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoSeparationAdjustLess.dwValue = lValue;   // The hot key to reduce the horizontal separation.
            }
        }
        if (RegQueryValueEx(hKey, NV_REG_STEREO_ADJUSTENABLED, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            // Allow stereo adjustments by hot keys.
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_CONVERGENCEADJUST_MORE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoConvergenceAdjustMore.dwValue = lValue;   // The hot key to increase the convergence degree.
            }
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_CONVERGENCEADJUST_LESS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoConvergenceAdjustLess.dwValue = lValue;   // The hot key to reduce the convergence degree.
            }
        }

LSR_ExitWithClose:
        RegCloseKey(hKey);
    }
}

void DestroyStereo(void)
{
    NvSetStereoDataPtrFromDriverData;
#ifdef  STEREO_VIEWER_DRV
    if (pDriverData && STEREO_ENABLED && pStereoData->hStereoDrv)
#else   //STEREO_VIEWER_DRV==0
    if (pDriverData && STEREO_ENABLED)
#endif  //STEREO_VIEWER_DRV
    {
        if (pStereoData->pVertexRegister)
        {
            delete pStereoData->pVertexRegister;
            pStereoData->pVertexRegister = NULL;
        }
        if (pStereoData->pLockSubstituteLcl)
        {
            CNvObject *pNvObj = GET_PNVOBJ(pStereoData->pLockSubstituteLcl);
            nvAssert(pNvObj);
            pNvObj->release();
            FreeIPM(pStereoData->pLockSubstituteLcl);
            pStereoData->pLockSubstituteLcl = NULL;
        }
        if (global.dwDXRuntimeVersion >= 0x0700)
        {
            if (pStereoData->pLockTargetLcl)
            {
                FreeIPM(pStereoData->pLockTargetLcl);
            }
        }
        pStereoData->pLockTargetLcl = NULL;
        DWORD dwDummy;
        pStereoData->RmStereoParams.FlipOffsets[3][0] = 0xFFFFFFFF;
        NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                   NV_CFG_STEREO_CONFIG, 0, &dwDummy);
#ifdef  STEREO_VIEWER_DRV
        pStereoData->pStereoViewer->DestroyStereoViewer();
        FreeLibrary(pStereoData->hStereoDrv);
#endif  //STEREO_VIEWER_DRV
        FreeIPM(pStereoData);
        pStereoData = NULL;
        pDriverData->pStereoData = (DWORD)pStereoData;
    }

}

DWORD CalculateStereoParameters(void)
{
    LPDDHALMODEINFO lpModeInfo = &pDriverData->ModeList[pDXShare->dwModeNumber];
    DWORD dwStereoWidth = lpModeInfo->dwWidth;
    DWORD dwStereoHeight = lpModeInfo->dwHeight;
    DWORD dwStereoRefresh = (DWORD)lpModeInfo->wRefreshRate;
    float fHalfStereoSeparation = pStereoData->StereoSettings.fStereoSeparation * 0.5f;
    pStereoData->StereoSettings.fScreenStereoSeparation = fHalfStereoSeparation * dwStereoWidth;
    pStereoData->StereoSettings.fPostStereoConvergence = pStereoData->StereoSettings.fStereoConvergence;
    pStereoData->StereoSettings.fPostStereoXStretch = 1.f + pStereoData->StereoSettings.fStereoSeparation;
    pStereoData->StereoSettings.fPostStereoXShift   = dwStereoWidth * fHalfStereoSeparation;
    if (pDriverData->dwUseRefreshRate)
        dwStereoRefresh = pDriverData->dwUseRefreshRate;
    return 0;
}

LPD3DMATRIX SetStereoProjection(LPD3DMATRIX pMat, PNVD3DCONTEXT pContext)
{
    if (!STEREO_R_US(pContext))
        return pMat;

    float fDirection = 1.0f;
    ORIG_PROJECTION = *pMat; //Saved the original projection matrix.
    pStereoData->Eye[EYE_LEFT ].ProjMatrix =
    pStereoData->Eye[EYE_RIGHT].ProjMatrix = *pMat;
#if 0
    //Works for demolition raser but doesn't for watchmaker
    if (pStereoData->Eye[EYE_LEFT ].ProjMatrix._43 != 0.0f
     && pStereoData->Eye[EYE_LEFT ].ProjMatrix._44 != 1.0f)
#else
    if (pStereoData->Eye[EYE_LEFT ].ProjMatrix._43 != 0.0f)
#endif
    {
        if (pStereoData->Eye[EYE_LEFT ].ProjMatrix._33 < 0)
            fDirection = -1.0f;
        float fStereoSeparation = pStereoData->StereoSettings.fStereoSeparation * fDirection;
        if (pStereoData->StereoSettings.FrustumAdjustMode == FRUSTUM_STRETCH)
        {
            float fConvergenceBySeparation = pStereoData->StereoSettings.fStereoConvergence * pStereoData->StereoSettings.fStereoSeparation
                                             * fDirection;
            pStereoData->Eye[EYE_LEFT ].ProjMatrix._11 *= (1.f + pStereoData->StereoSettings.fStereoSeparation);
            pStereoData->Eye[EYE_RIGHT].ProjMatrix._11  = pStereoData->Eye[EYE_LEFT ].ProjMatrix._11;
            pStereoData->Eye[EYE_LEFT ].ProjMatrix._31 -= fStereoSeparation;
            pStereoData->Eye[EYE_LEFT ].ProjMatrix._41 += fConvergenceBySeparation;
            pStereoData->Eye[EYE_RIGHT].ProjMatrix._31 += fStereoSeparation;
            pStereoData->Eye[EYE_RIGHT].ProjMatrix._41 -= fConvergenceBySeparation;
        } else
        {
            float fConvergenceBySeparation = pStereoData->StereoSettings.fStereoConvergence * pStereoData->StereoSettings.fStereoSeparation
                                             * fDirection;
            pStereoData->Eye[EYE_LEFT ].ProjMatrix._31 -= fStereoSeparation;
            pStereoData->Eye[EYE_LEFT ].ProjMatrix._41 += fConvergenceBySeparation;
            pStereoData->Eye[EYE_RIGHT].ProjMatrix._31 += fStereoSeparation;
            pStereoData->Eye[EYE_RIGHT].ProjMatrix._41 -= fConvergenceBySeparation;
        }
    }
    pStereoData->StereoSettings.dwFlags |= STEREO_PROJECTIONMATRIXVALID;
    return &pStereoData->Eye[pStereoData->dwLastEye ].ProjMatrix;
}

void SetStereoProjectionInternal (PNVD3DCONTEXT pContext, LPD3DMATRIX pProjMatrix)
{
    nvMemCopy (&(pContext->xfmProj), pProjMatrix, sizeof(D3DMATRIX));
#if (NVARCH >= 0x020)
    pContext->hwState.dwDirtyFlags |= KELVIN_DIRTY_TRANSFORM;
#endif
#if (NVARCH >= 0x010)
    pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_TRANSFORM;
#endif
}

#if (NVARCH >= 0x010)

void SetStereoRenderTargetInternalTNT (PNVD3DCONTEXT pContext, DWORD dwEye)
{
    DWORD dwRenderTargetOffset;
    DWORD dwZOffset = 0;

    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))) {

        if (dwEye == EYE_RIGHT)
        {
            dwRenderTargetOffset = pContext->pRenderTarget->getOffsetRight();
            if (pContext->pZetaBuffer)
                dwZOffset = pContext->pZetaBuffer->getOffsetRight();
        } else
        {
            dwRenderTargetOffset = pContext->pRenderTarget->getOffset();
            if (pContext->pZetaBuffer)
                dwZOffset = pContext->pZetaBuffer->getOffset();
        }

        nvglSetObject(NV_DD_SURFACES, D3D_CONTEXT_SURFACES_ARGB_ZS);
        nvglSetRenderTargetContexts(NV_DD_SURFACES,
                                    NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

        DWORD dwFormat = nv053SurfaceFormat[pContext->pRenderTarget->getFormat()];
        dwFormat |= DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH);

        if (!(pContext->surfaceViewport.clipHorizontal.wX) && !(pContext->surfaceViewport.clipVertical.wY)) {

            DWORD dwClip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16) |
                           (pContext->surfaceViewport.clipHorizontal.wWidth);
            DWORD dwZPitch  = pContext->pZetaBuffer ? pContext->pZetaBuffer->getPitch()  : 32;
            DWORD dwPitch   = (DRF_NUM(053, _SET_PITCH, _COLOR, pContext->pRenderTarget->getPitch())) |
                              (DRF_NUM(053, _SET_PITCH, _ZETA,  dwZPitch));
            nvglSetRenderTarget2(NV_DD_SURFACES,
                                 dwFormat,
                                 dwClip,
                                 dwPitch,
                                 dwRenderTargetOffset,
                                 dwZOffset);
        }
        else {
            DWORD dwClip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16) |
                           (pContext->surfaceViewport.clipHorizontal.wWidth);
            DWORD dwZPitch  = pContext->pZetaBuffer ? pContext->pZetaBuffer->getPitch()  : 32;
            DWORD dwPitch   = (DRF_NUM(053, _SET_PITCH, _COLOR, pContext->pRenderTarget->getPitch())) |
                              (DRF_NUM(053, _SET_PITCH, _ZETA,  dwZPitch));
            nvglSetRenderTarget2(NV_DD_SURFACES,
                                 dwFormat,
                                 dwClip,
                                 dwPitch,
                                 dwRenderTargetOffset,
                                 dwZOffset);

            // Set the image black rectangle to the clip region.
            nvglSetObject(NV_DD_SURFACES, NV_DD_IMAGE_BLACK_RECTANGLE);
            nvglSetClip(NV_DD_SURFACES,
                        pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY,
                        pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);
        }

    }  // ! (CELSIUS | KELVIN)

    nvglSetObject(NV_DD_SURFACES, NV_DD_SURFACES_2D);
    pDriverData->TwoDRenderingOccurred = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;
    pDriverData->dDrawSpareSubchannelObject = 0; // force a reload.
    pDriverData->ddClipUpdate = TRUE;
    //NV_FORCE_TRI_SETUP(pContext);
    return;
}

#if (NVARCH >= 0x020)
void  SetStereoRenderTargetInternal(PNVD3DCONTEXT pContext, DWORD dwEye) 
{
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)))   
        SetStereoRenderTargetInternalTNT (pContext,dwEye);                                              
    else                                                                                                    
        pContext->hwState.dwDirtyFlags |= (CELSIUS_DIRTY_SURFACE | KELVIN_DIRTY_SURFACE);
}
#else
void  SetStereoRenderTargetInternal(PNVD3DCONTEXT pContext, DWORD dwEye) 
{
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)))   
        SetStereoRenderTargetInternalTNT (pContext,dwEye);                                              
    else                                                                                                    
        pContext->hwState.dwDirtyFlags |= CELSIUS_DIRTY_SURFACE;
}
#endif

#else   //NVARCH < 0x010)

void SetStereoRenderTargetInternal (PNVD3DCONTEXT pContext, DWORD dwEye)
{
    DWORD dwRenderTargetOffset;
    DWORD dwZOffset = 0;

    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))) {

        if (dwEye == EYE_LEFT)
        {
            dwRenderTargetOffset = pContext->pRenderTarget->getOffset();
            if (pContext->pZetaBuffer)
                dwZOffset = pContext->pZetaBuffer->getOffset();
        } else
        {
            dwRenderTargetOffset = pContext->pRenderTarget->getOffsetRight();
            if (pContext->pZetaBuffer)
                dwZOffset = pContext->pZetaBuffer->getOffsetRight();
        }

        nvglSetObject(NV_DD_SURFACES, D3D_CONTEXT_SURFACES_ARGB_ZS);
        nvglSetRenderTargetContexts(NV_DD_SURFACES,
                                    NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

        DWORD dwFormat = nv053SurfaceFormat[pContext->pRenderTarget->getFormat()];
        dwFormat |= DRF_DEF(053, _SET_FORMAT, _TYPE, _PITCH);

        if (!(pContext->surfaceViewport.clipHorizontal.wX) && !(pContext->surfaceViewport.clipVertical.wY)) {

            DWORD dwClip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16) |
                           (pContext->surfaceViewport.clipHorizontal.wWidth);
            DWORD dwZPitch  = pContext->pZetaBuffer ? pContext->pZetaBuffer->getPitch()  : 32;
            DWORD dwPitch   = (DRF_NUM(053, _SET_PITCH, _COLOR, pContext->pRenderTarget->getPitch())) |
                              (DRF_NUM(053, _SET_PITCH, _ZETA,  dwZPitch));
            nvglSetRenderTarget2(NV_DD_SURFACES,
                                 dwFormat,
                                 dwClip,
                                 dwPitch,
                                 dwRenderTargetOffset,
                                 dwZOffset);
        }
        else {
            DWORD dwClip = (((DWORD)pContext->surfaceViewport.clipVertical.wHeight) << 16) |
                           (pContext->surfaceViewport.clipHorizontal.wWidth);
            DWORD dwZPitch  = pContext->pZetaBuffer ? pContext->pZetaBuffer->getPitch()  : 32;
            DWORD dwPitch   = (DRF_NUM(053, _SET_PITCH, _COLOR, pContext->pRenderTarget->getPitch())) |
                              (DRF_NUM(053, _SET_PITCH, _ZETA,  dwZPitch));
            nvglSetRenderTarget2(NV_DD_SURFACES,
                                 dwFormat,
                                 dwClip,
                                 dwPitch,
                                 dwRenderTargetOffset,
                                 dwZOffset);

            // Set the image black rectangle to the clip region.
            nvglSetObject(NV_DD_SURFACES, NV_DD_IMAGE_BLACK_RECTANGLE);
            nvglSetClip(NV_DD_SURFACES,
                        pContext->surfaceViewport.clipHorizontal.wX, pContext->surfaceViewport.clipVertical.wY,
                        pContext->surfaceViewport.clipHorizontal.wWidth, pContext->surfaceViewport.clipVertical.wHeight);
        }

    }  // ! (CELSIUS | KELVIN)

    nvglSetObject(NV_DD_SURFACES, NV_DD_SURFACES_2D);
    pDriverData->TwoDRenderingOccurred = 0;
    pDriverData->ThreeDRenderingOccurred = TRUE;
    pDriverData->dDrawSpareSubchannelObject = 0; // force a reload.
    pDriverData->ddClipUpdate = TRUE;
    //NV_FORCE_TRI_SETUP(pContext);
    return;
}

#endif  //NVARCH >= 0x010)

void AdjustStereoParameters(PNVD3DCONTEXT pContext)
{
    pStereoData->dwLastEye = EYE_NONE;
    CalculateStereoParameters();
    if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
        SetStereoProjection(&ORIG_PROJECTION, pContext);
}

void SetupStereoContext(PNVD3DCONTEXT pContext)
{
#ifdef  LEFT_ALWAYS_FIRST
    pStereoData->dwLastEye = EYE_NONE;
#else   //LEFT_ALWAYS_FIRST
#ifdef  NULL_RIGHT_EYE
    pStereoData->dwLastEye = EYE_NONE;
#endif  //NULL_RIGHT_EYE
#endif  //LEFT_ALWAYS_FIRST
    if (pStereoData->dwLastEye == EYE_NONE)
    {
        //Has not been set yet
        SetStereoRenderTargetInternal (pContext, EYE_LEFT);
        if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
            SetStereoProjectionInternal (pContext, &pStereoData->Eye[EYE_LEFT ].ProjMatrix);
        pStereoData->dwLastEye = EYE_LEFT;
    }
}

void AlternateStereoContext(PNVD3DCONTEXT pContext)
{
    if (pStereoData->dwLastEye == EYE_LEFT)
    {
        SetStereoRenderTargetInternal (pContext, EYE_RIGHT);
        if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
            SetStereoProjectionInternal (pContext, &pStereoData->Eye[EYE_RIGHT ].ProjMatrix);
        pStereoData->dwLastEye = EYE_RIGHT;
    } else
    {
        SetStereoRenderTargetInternal (pContext, EYE_LEFT);
        if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
            SetStereoProjectionInternal (pContext, &pStereoData->Eye[EYE_LEFT ].ProjMatrix);
        pStereoData->dwLastEye = EYE_LEFT;
    }
}

void CaptureDP2Entries(PNVD3DCONTEXT pContext, nvDP2FunctionTable *pDP2functionTable)
{
    DWORD i;

    // Sanity check against infinite loop
    nvAssert ((*pContext->pDP2FunctionTable)[D3DDP2OP_CLEAR] != (*pDP2functionTable)[D3DDP2OP_CLEAR]);

    for (i = 0; i <= D3D_DP2OP_MAX; i++)
    {
        if ((*pDP2functionTable)[i])
        {
            //We can handle it
            nvDP2SetFuncs_Orig[i] = (*pContext->pDP2FunctionTable)[i];     //Saved the original entry.
            (*pContext->pDP2FunctionTable)[i] = (*pDP2functionTable)[i];   //Put ours.
        }
    }
}

void RestoreDP2Entries(PNVD3DCONTEXT pContext, nvDP2FunctionTable *pDP2FunctionTable)
{
    DWORD i;

    for (i = 0; i <= D3D_DP2OP_MAX; i++)
    {
        if (nvDP2SetFuncs_Orig[i])
        {
            //We saved it before
            (*pContext->pDP2FunctionTable)[i] = nvDP2SetFuncs_Orig[i];     //Restored the original entry.
            nvDP2SetFuncs_Orig[i] = NULL;
        }
    }
}

#include <math.h>

int ComputeGammaColor (int i, float fGamma)
{
   float   fBase, fVal, fI;
   int     iVal;

   fI = (float)i;

// Compute gamma

   fBase = fI / 255.0f; 
   fVal = (float)pow(fBase, 1.0f / fGamma);
   iVal = (int)(255.0f * fVal);

#if 0
// *********************************************
// Compensate for brightness
   
   iVal += (int)fBrightness;

   if (iVal > 255) 
     iVal = 255;

   if (iVal < 0) 
     iVal = 0;

// ***********************************************
#endif
   return iVal;
}

DWORD ActivateStereo(PNVD3DCONTEXT pContext)
{
    if (pStereoData->StereoSettings.dwFlags & STEREO_DISALLOWED)
        return 1;

#ifdef  STEREO_VIEWER_DRV
    switch (pStereoData->StereoViewerCaps.dwActivationMethod)
    {
    case SAM_VRAMPATTERN:
        pStereoData->pStereoViewer->ActivateStereo(&pDriverData->ModeList[pDXShare->dwModeNumber],
                                                    NULL,
                                                    0);
        CaptureDP2Entries(pContext, &nvDP2SetFuncs_Stereo_Flip);
        if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
            SetStereoProjectionInternal (pContext, &pStereoData->Eye[EYE_LEFT ].ProjMatrix);
        pStereoData->dwLastEye = EYE_LEFT;
        if (!pStereoData->pVertexRegister)
        {
            CVertexRegister *pVertexRegister  = new CVertexRegister(pStereoData->dwVertexRegisterSize);
            pStereoData->pVertexRegister = pVertexRegister;
        }
        break;

    default:
        nvAssert (0 == 1);
    }
#else   //STEREO_VIEWER_DRV==0
    CaptureDP2Entries(pContext, &nvDP2SetFuncs_Stereo_Flip);
    if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
        SetStereoProjectionInternal (pContext, &pStereoData->Eye[EYE_LEFT ].ProjMatrix);
    pStereoData->dwLastEye = EYE_LEFT;
    if (!pStereoData->pVertexRegister)
    {
        CVertexRegister *pVertexRegister  = new CVertexRegister(pStereoData->dwVertexRegisterSize);
        pStereoData->pVertexRegister = pVertexRegister;
    }
#endif  //STEREO_VIEWER_DRV
    if (pStereoData->StereoSettings.dwFlags & STEREO_RIGHTEYEINIT)
    {
        //Right eye has not been rendered yet. Make sure that its initial
        //image is at least the same as the left eye.
        //Make sure the accelerator is free.
        WaitForIdle (TRUE,FALSE);

        StereoRightEyeInit();
        pStereoData->StereoSettings.dwFlags &= ~STEREO_RIGHTEYEINIT;
    }
#if 0
    DWORD dwDummy;
    NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
               NV_CFG_STEREO_CONFIG, (DWORD)&pStereoData->RmStereoParams, &dwDummy);
    //The following setting will force resman to start with the left eye at VBLANK. This is very
    //important for GPIO type of connection because first time we send the triggering bit the
    //glasses block the right eye.
    pStereoData->RmStereoParams.EyeDisplayed = 0;
#ifdef  STEREO_VIEWER_DRV
    pStereoData->RmStereoParams.Flags |= (STEREOCFG_STEREOACTIVATED | STEREOCFG_ELSADDC);
#else   //STEREO_VIEWER_DRV==0
    pStereoData->RmStereoParams.Flags |= (STEREOCFG_STEREOACTIVATED | pStereoData->dwStereoHWType);
#endif  //STEREO_VIEWER_DRV
#else
    DWORD dwDummy;
    //The following setting will force resman to start with the left eye at VBLANK. This is very
    //important for GPIO type of connection because first time we send the triggering bit the
    //glasses block the right eye.
    pStereoData->RmStereoParams.EyeDisplayed = 0;
    pStereoData->RmStereoParams.Flags |= (STEREOCFG_STEREO_INITIALIZE | pStereoData->dwStereoHWType);
    NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
               NV_CFG_STEREO_CONFIG, (DWORD)&pStereoData->RmStereoParams, &dwDummy);
#endif
    pStereoData->bStereoActivated = TRUE;

    if (STEREODATA(StereoSettings.dwFlags) & STEREO_GAMMACORRECTION)
	{
		//Gamma adjustment for stereo.
		WORD    NewGamma[256 * 3];
		DWORD   i;
		HDC hDC = GetDC(NULL);
		if (hDC)
		{
			if (GetDeviceGammaRamp(hDC, pStereoData->SaveCurrentGamma))
			{
				WORD  NewVal;
				for (i = 0; i < 256; i++)
				{

					NewVal = (WORD)(ComputeGammaColor (i, pStereoData->StereoSettings.fStereoGamma) << 8); 
					NewGamma[i] = NewVal;
					NewGamma[i+256] = NewVal;
					NewGamma[i+512] = NewVal;
				}
				SetDeviceGammaRamp(hDC, NewGamma); 
			}
			ReleaseDC(NULL,hDC);
		}
    }

	/////////// create a list of PopupAgents
#ifdef USE_POPUP_LIST
	if(popupAgents.enabled()) //in registry
    {
        popupAgents.create();
    }
#endif //USE_POPUP_LIST

    return 0;
}

DWORD DeactivateStereo(PNVD3DCONTEXT pContext)
{
#ifdef  STEREO_VIEWER_DRV
    switch (pStereoData->StereoViewerCaps.dwActivationMethod)
    {
    case SAM_VRAMPATTERN:
        RestoreDP2Entries(pContext, &nvDP2SetFuncs_Stereo_Flip);
        SetStereoRenderTargetInternal (pContext, EYE_LEFT);
        if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
            SetStereoProjectionInternal (pContext, &ORIG_PROJECTION);
        pStereoData->pStereoViewer->DeactivateStereo(&pDriverData->ModeList[pDXShare->dwModeNumber],
                                                    NULL,
                                                    0);
        pStereoData->dwLastEye = EYE_NONE;
        if (pStereoData->pVertexRegister)
        {
            delete pStereoData->pVertexRegister;
            pStereoData->pVertexRegister = NULL;
        }
        break;

    default:
        nvAssert (0 == 1);
    }
#else   //STEREO_VIEWER_DRV==0
    RestoreDP2Entries(pContext, &nvDP2SetFuncs_Stereo_Flip);
    SetStereoRenderTargetInternal (pContext, EYE_LEFT);
    if (pStereoData->StereoSettings.dwFlags & STEREO_PROJECTIONMATRIXVALID)
        SetStereoProjectionInternal (pContext, &ORIG_PROJECTION);
    pStereoData->dwLastEye = EYE_NONE;
    if (pStereoData->pVertexRegister)
    {
        delete pStereoData->pVertexRegister;
        pStereoData->pVertexRegister = NULL;
    }
#endif  //STEREO_VIEWER_DRV
    DWORD dwDummy;
#if 0
    pStereoData->RmStereoParams.Flags &= ~STEREOCFG_STEREOACTIVATED;
#else
    pStereoData->RmStereoParams.Flags |= STEREOCFG_STEREO_DISABLE;
#endif
    NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
           NV_CFG_STEREO_CONFIG, (DWORD)&pStereoData->RmStereoParams, &dwDummy);
    pStereoData->bStereoActivated = 0;
    
    if (STEREODATA(StereoSettings.dwFlags) & STEREO_GAMMACORRECTION)
	{
		//Gamma adjustment for stereo.
		HDC hDC = GetDC(NULL);
		if (hDC)
		{
			//Restore original gamma.
			SetDeviceGammaRamp(hDC, pStereoData->SaveCurrentGamma); 
			ReleaseDC(NULL,hDC);
		}
    }

	/////////// destroy a list of PopupAgent 
#ifdef USE_POPUP_LIST
	//we're always destroying it, it's safe	
	//destroyPopupList();
	popupAgents.destroy();
#endif //USE_POPUP_LIST

	return 0;
}

void GenerateBasicRegEntries(char *StereoKey)
{
    HKEY    hConfigKey, hKeyApp;
    char    *pCommandLine;

    if (RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, StereoKey, 0, KEY_QUERY_VALUE, &hConfigKey) != ERROR_SUCCESS)
    {
        //Can't really happen
        LOG("GenerateBasicRegEntries: can't open key %s", StereoKey);
		nvAssert(0);
        return;
    }
    if (RegCreateKeyEx(hConfigKey, pStereoData->bAppRealName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyApp, NULL) != ERROR_SUCCESS)
    {
        LOG("GenerateBasicRegEntries: can't create key %s", pStereoData->bAppRealName);
        nvAssert(0);
        return;
    }
    RegSetValueEx(hKeyApp, "RunTimeName", NULL, REG_SZ, (LPBYTE)pStereoData->bAppName, sizeof (pStereoData->bAppName));
    pCommandLine = GetCommandLine();
    RegSetValueEx(hKeyApp, "CommandLine", NULL, REG_SZ, (LPBYTE)pCommandLine, nvStrLen(pCommandLine));
    RegCloseKey(hConfigKey);
    RegCloseKey(hKeyApp);
}

BOOL CheckAndMapSymbolicLink(HKEY hKey)
{
    char    LinkBuf[32];
    char    StereoKey[256];
    DWORD   dwNameSize;
    DWORD   dwNameType;
    BOOL    isSymbolicLink = FALSE;

    nvStrCpy(StereoKey, NV4_REG_GLOBAL_BASE_PATH);
    nvStrCat(StereoKey, "\\");
    nvStrCat(StereoKey, NV4_REG_STEREO_SUBKEY);
    nvStrCat(StereoKey, "\\GameConfigs\\");

    for (DWORD i = 1; ; i++)
    {
        nvSprintf (LinkBuf,"Link%0d", i);
        //Look for the stereo viewer driver first.
        dwNameSize = sizeof(pStereoData->bAppRealName);
        dwNameType = REG_SZ;
        if (RegQueryValueEx(hKey, LinkBuf, NULL, &dwNameType, (LPBYTE)pStereoData->bAppRealName, &dwNameSize) != ERROR_SUCCESS)
        {
            if (isSymbolicLink)
            {
                //The application has the same name as an existing symbolic link but can not be recognized among
                //existing links. We assume that this is new candidate to be linked now. We will create some
                //fake link name "_NewFakeLink" that is to be resolved manually later.
                nvStrCpy(pStereoData->bAppRealName, "_NewFakeLink");
                GenerateBasicRegEntries(StereoKey);
                RegSetValueEx(hKey, LinkBuf, NULL, REG_SZ, (LPBYTE)pStereoData->bAppRealName, sizeof (pStereoData->bAppRealName));
            }
            return isSymbolicLink;
        }
        isSymbolicLink = TRUE;
        //Found some link. Let's see if it is ours.
        if (GetFileAttributes(pStereoData->bAppRealName) != -1)
        {
            //It is ours. We just need to remove the extension part if any.
            char *pExt = nvStrChr(pStereoData->bAppRealName, '.');
            if (pExt)
                pExt[0] = 0;
            GenerateBasicRegEntries(StereoKey);
            return TRUE;
        }
    }
    //Should never get here.
    LOG("CheckAndMapSymbolicLink: Should never get here");
    nvAssert(0);
    return FALSE;
}

void ResetStereoParameters(void)
{
#ifdef  STEREO_CONFIG_ASSIST
    ConfigAssistInfo.rhwMin     = 9999.f;
    ConfigAssistInfo.rhwMin2D   = 9999.f;
    ConfigAssistInfo.rhwMax     = -9999.f;
    ConfigAssistInfo.rhwMax2D   = -9999.f;
    ConfigAssistInfo.szMin      = 9999.f;
    ConfigAssistInfo.szMin2D    = 9999.f;
    ConfigAssistInfo.szMax      = -9999.f;
    ConfigAssistInfo.szMax2D    = -9999.f;
    StereoSettingsStack.Valid   = FALSE;
#endif  //STEREO_CONFIG_ASSIST

    HKEY hKey;
    char StereoKey[256];
    BOOL bFirstTime = TRUE;

    nvStrCpy(StereoKey, NV4_REG_GLOBAL_BASE_PATH);
    nvStrCat(StereoKey, "\\");
    nvStrCat(StereoKey, NV4_REG_STEREO_SUBKEY);
    if (pStereoData->bAppName)
    {
        nvStrCat(StereoKey, "\\GameConfigs\\");
        nvStrCat(StereoKey, pStereoData->bAppName);
    }

    //First of all restore all game specific defaults
    //pStereoData->StereoSettings.dwFlags &= ~(STEREO_CUSTOMSETTINGSMASK | STEREO_DEFAULTON | STEREO_LASERSIGHT);
    pStereoData->StereoSettings.dwFlags &= ~(STEREO_CUSTOMSETTINGSMASK |STEREO_LASERSIGHT);
    pStereoData->StereoSettings.fStereoConvergence = DEFAULT_STEREO_CONVERGENCE;
    pStereoData->StereoSettings.fConvergenceMultiplier = DEFAULT_STEREO_CONVERGENCE_MULTIPLIER;
    pStereoData->fRHW2DDetectionMin = -9999.f;
    nvStrCpy(pStereoData->bAppRealName, pStereoData->bAppName);
SecondPass:
    if (RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, StereoKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        if (bFirstTime && CheckAndMapSymbolicLink(hKey))
        {
            //The application name has been mapped to a new one. We need to a second pass.
            RegCloseKey(hKey);
            nvStrCpy(StereoKey, NV4_REG_GLOBAL_BASE_PATH);
            nvStrCat(StereoKey, "\\");
            nvStrCat(StereoKey, NV4_REG_STEREO_SUBKEY);
            nvStrCat(StereoKey, "\\GameConfigs\\");
            nvStrCat(StereoKey, pStereoData->bAppRealName);
            bFirstTime = FALSE;
            goto SecondPass;
        }

        if (RegQueryValueEx(hKey, "AllowNonExclusiveStereo", NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
                pStereoData->StereoSettings.dwFlags &= ~STEREO_DISALLOWED_NOT_FULL_SCREEN;
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_CONVERGENCE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.fStereoConvergence = *(float *)&lValue;
            if (pStereoData->StereoSettings.fStereoConvergence == 0)
            {
                //Make it whatever but not 0. Exponential doesnt work on 0
                pStereoData->StereoSettings.fStereoConvergence = CONVERGENCE_THRESHOLD;
            }
            pStereoData->StereoSettings.dwFlags |= STEREO_CONVERGENCE;
        }

#ifdef  LASER_SIGHT
        if (pStereoData->StereoSettings.dwFlags & STEREO_LASERSIGHTENABLED)
        {
            pStereoData->StereoSettings.fLaserXAdjust = 1.0f;
            pStereoData->StereoSettings.fLaserYAdjust = 1.0f;
            if (RegQueryValueEx(hKey, NV_REG_STEREO_LASERSIGHT, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                if (lValue)
                {
                    pStereoData->StereoSettings.dwFlags |= STEREO_LASERSIGHT;
                    CreateLaserTextureContent();
                    //Turn this off. Otherwise it will mess up the Z buffer content.
                    pDriverData->nvD3DPerfData.dwPerformanceStrategy &= ~(PS_CONTROL_TRAFFIC_16|PS_CONTROL_TRAFFIC_32);
                    if (RegQueryValueEx(hKey, NV_REG_STEREO_LASERXADJUST, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                    {
                        pStereoData->StereoSettings.fLaserXAdjust = *(float *)&lValue;
                    }
                    if (RegQueryValueEx(hKey, NV_REG_STEREO_LASERYADJUST, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                    {
                        pStereoData->StereoSettings.fLaserYAdjust = *(float *)&lValue;
                    }
                }
            }
        }
#endif  //LASER_SIGHT

        if (RegQueryValueEx(hKey, NV_REG_STEREO_CONVERGENCEMULTIPLIER, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.fConvergenceMultiplier = *(float *)&lValue;
            pStereoData->StereoSettings.dwFlags |= STEREO_CONVERGENCEMULTIPLIER;
        }

        pStereoData->StereoSettings.FrustumAdjustMode = NO_FRUSTUM_ADJUST;
        if (RegQueryValueEx(hKey, NV_REG_STEREO_FRUSTUMADJUSTMODE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.FrustumAdjustMode = lValue;
        }

        pStereoData->StereoSettings.PartialClearMode = DO_AS_REQUESTED;
        if (RegQueryValueEx(hKey, NV_REG_STEREO_PARTIALCLEARMODE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.PartialClearMode = lValue;
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_AUTOCONVERGENCE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_AUTOCONVERGENCE;
                if (RegQueryValueEx(hKey, NV_REG_STEREO_CONVERGENCEADJUSTPACE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    pStereoData->StereoSettings.AutoConvergenceAdjustPace = *(float *)&lValue;
                }
            }
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_RHWGREATERATSCREEN, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.dwFlags |= STEREO_RHWGREATERATSCREEN;
            pStereoData->StereoSettings.fRHWGreaterAtScreen = *(float *)&lValue;
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_RHWEQUALATSCREEN, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWEQUALATSCREEN;
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_FAVORSZ, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
                pStereoData->StereoSettings.dwFlags |= STEREO_FAVORSZOVERRHW;
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_RHWLESSATSCREEN, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
            pStereoData->StereoSettings.fRHWLessAtScreen = *(float *)&lValue;
        }

        pStereoData->fRHW2DDetectionMin = -9999.f;
        if (RegQueryValueEx(hKey, NV_REG_STEREO_RHW2DDETECTIONMIN, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            pStereoData->fRHW2DDetectionMin = *(float *)&lValue;

        if (RegQueryValueEx(hKey, NV_REG_STEREO_MAXVERTEXCOUNT, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            pStereoData->dwVertexRegisterSize = lValue;

        if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_TOGGLE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            pStereoData->StereoKeys.StereoToggle.dwValue = lValue;   // The hot key to toggle stereo.
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_DEFAULTON, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (lValue)
                pStereoData->StereoSettings.dwFlags |= STEREO_DEFAULTON;
        }

        if (RegQueryValueEx(hKey, NV_REG_STEREO_ADJUSTENABLED, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            // Allow stereo adjustments by hot keys.
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_SEPARATIONADJUST_MORE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoSeparationAdjustMore.dwValue = lValue;   // The hot key to increase the horizontal separation.
            } else
            {
                //Should be eventually removed.
                if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_HORIZONTALADJUST_MORE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    pStereoData->StereoKeys.StereoSeparationAdjustMore.dwValue = lValue;   // The hot key to increase the horizontal separation.
                }
            }
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_SEPARATIONADJUST_LESS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoSeparationAdjustLess.dwValue = lValue;   // The hot key to reduce the horizontal separation.
            } else
            {
                //Should be eventually removed.
                if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_HORIZONTALADJUST_LESS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
                {
                    pStereoData->StereoKeys.StereoSeparationAdjustLess.dwValue = lValue;   // The hot key to reduce the horizontal separation.
                }
            }
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_CONVERGENCEADJUST_MORE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoConvergenceAdjustMore.dwValue = lValue;   // The hot key to increase the convergence degree.
            }
            if (RegQueryValueEx(hKey, NV_REG_STEREO_HOTKEY_CONVERGENCEADJUST_LESS, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
            {
                pStereoData->StereoKeys.StereoConvergenceAdjustLess.dwValue = lValue;   // The hot key to reduce the convergence degree.
            }
        }
        RegCloseKey(hKey);
    }

    pStereoData->dwLastEye        = EYE_NONE;
    pStereoData->fFrameRHWMax     = -9999.f;
    CalculateStereoParameters();
}

void StereoContextCreate(void)
{
    char FirstContextOwner[128];
    
    if (!STEREO_ENABLED)
        return;

    FirstContextOwner[0] = 0;
    if (pStereoData->dwContext)
    {
        //We don't allow more than 1 application at a time in stereo mode but we allow more than
        //one context in applications.
        nvStrCpy(FirstContextOwner, pStereoData->bAppRealName);
    }

    char AppFullName[1024];
    //Let's see if we need to override the stereo settings for this particular application.
    char *pAppFullName, *pAppName;
    nvStrCpy(AppFullName, GetCommandLine());
    pAppFullName = AppFullName;
    if (pAppFullName[0] == '"')
    {
        pAppFullName++;
        pAppName = nvStrChr(pAppFullName, '"');
        pAppName[0] = 0;
    } else
    {
        if (pAppName = nvStrChr(pAppFullName, ' '))
            pAppName[0] = 0;
    }
    if (pAppName = nvStrRChr(pAppFullName, '\\'))
        pAppName++;
    else
        pAppName = pAppFullName;
    if (pAppFullName = nvStrChr(pAppName, '.'))
        *pAppFullName = 0;
    //The last sanity check
    if (pAppName[0] == 0)
    {
        //Eliminated everything
        pAppName = "NoName";
    }
    nvStrCpy(pStereoData->bAppName, pAppName);
    if (!(pStereoData->dwVertexRegisterSize = getDC()->nvD3DDevCaps.dd1Caps.dwMaxVertexCount))
		pStereoData->dwVertexRegisterSize = (32 * 2048) / sizeof(D3DTLVERTEX);
    ResetStereoParameters();
    if (FirstContextOwner[0])
    {
        //It might be another context in the same application
        if (!nvStrCmp(FirstContextOwner, pStereoData->bAppRealName))
        {
            //The same application, another context
            pStereoData->dwContext = getDC()->dwContextListHead;
        } else
        {
            nvAssert(0);
		    LOG("StereoContextCreate: more than 1 application in stereo is not allowed");
        }        
        return;
    }
    pStereoData->bStereoActivated = FALSE;

#ifdef  LASER_SIGHT
    if (pStereoData->StereoSettings.dwFlags & STEREO_LASERSIGHT)
    {
        nvAssert (pLaser_Texture == NULL);

        pLaser_Texture = new CTexture;
        if (pLaser_Texture)
        {
            pLaser_Obj = new CNvObject(0);
            if (pLaser_Obj)
            {
                BOOL bRes = pLaser_Texture->create (pLaser_Obj, LASER_WIDTH, LASER_HEIGHT, 1, 4, 4, 1, NV_SURFACE_FORMAT_A8R8G8B8,
                                                  CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID);

                if (bRes)
                {
                    if (pStereoData->StereoSettings.dwFlags & STEREO_LASERSIGHT)
                        CreateLaserTextureContent();
                    pLaser_Obj->setObject(CNvObject::NVOBJ_TEXTURE, pLaser_Texture);
                }
                else
                {
                    // allocation failed
                    DPF("Couldn't allocate video memory for logo\n");
                    pLaser_Obj->release();
                    delete pLaser_Texture;
                    pLaser_Obj     = NULL;
                    pLaser_Texture = NULL;
                }
            } else
            {
                delete pLaser_Texture;
                pLaser_Texture = NULL;
                DPF("Unable to create laser object");
            }
        } else
            DPF("Unable to create laser texture");
    }
#endif  //LASER_SIGHT

#ifndef  DELAYED_HOTKEY_INIT
    if (!(pStereoData->StereoKeys.pHotKeyI = new CHOTKEYINTERFACE))
    {
        //Hot key support creation failed. We won't do anything special at this point like releasing the
        //StereoViewer. So all that previously done stuff will be standing by untill the driver gets unloaded.
        //It shouldn't cause any troubles because a game won't be able to activate stereo anyway. It would be
        //nice to somehow report the situation by putting message on the screen or generating some LogError
        //file. We'll figure it out later.
		LOG("StereoContextCreate: can't create hot key interface");
        return;
    }

    CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoToggle);
    CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustMore);
    CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustLess);
    CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustMore);
    CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustLess);
#ifdef  STEREO_CONFIG_ASSIST
    CREATESTEREOHOTKEY(ConfigAssistInfo.SuggestKey);
    CREATESTEREOHOTKEY(ConfigAssistInfo.UnsuggestKey);
    CREATESTEREOHOTKEY(ConfigAssistInfo.WriteKey);
    CREATESTEREOHOTKEY(ConfigAssistInfo.DeleteKey);
    CREATESTEREOHOTKEY(ConfigAssistInfo.FrustumAdjust);
    CREATESTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergence);
    CREATESTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergenceRestore);
    CREATESTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenMore);
    CREATESTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenLess);
    CREATESTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenMore);
    CREATESTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenLess);
    if (STEREODATA(StereoSettings.dwFlags) & STEREO_GAMMACORRECTION)
	{
		CREATESTEREOHOTKEY(ConfigAssistInfo.GammaAdjustMore);
		CREATESTEREOHOTKEY(ConfigAssistInfo.GammaAdjustLess);
	}
#ifdef  LASER_SIGHT
    CREATESTEREOHOTKEY(ConfigAssistInfo.LaserXPlusKey);
    CREATESTEREOHOTKEY(ConfigAssistInfo.LaserXMinusKey);
    CREATESTEREOHOTKEY(ConfigAssistInfo.LaserYPlusKey);
    CREATESTEREOHOTKEY(ConfigAssistInfo.LaserYMinusKey);
#endif  //LASER_SIGHT

#ifdef  USE_POPUP_LIST
    popupAgents.initHotKeys();
#endif  //USE_POPUP_LIST


#endif  //STEREO_CONFIG_ASSIST
#ifdef  SNAP_SHOT
    CREATESTEREOHOTKEY(SaveStereoImage);
#endif  //SNAP_SHOT
    nFlips = 0;
#else   //DELAYED_HOTKEY_INIT
    FirstHotKeyInit = 1;
#endif  //DELAYED_HOTKEY_INIT

    pStereoData->dwContext = getDC()->dwContextListHead;
  //LOG("StereoContextCreate: context created");

#if 0
    switch (pStereoData->dwStereoFormat)
    {
    case SVF_OVERANDUNDER:
        CalculateStereoParameters();
        break;

    case SVF_FRAMESEQUENTIAL:
        CalculateStereoParameters();
        break;

    case SVF_NONE:
        nvAssert (0 == 1);
        break;

    default:
        nvAssert (0 == 1);
        break;
    }
#endif
    if (pStereoData->StereoSettings.dwFlags & STEREO_FORCEDOFF)
    {
        //if (pStereoData->StereoModeNumber == pDXShare->dwModeNumber)
        {
            pStereoData->StereoSettings.dwFlags &= ~STEREO_FORCEDOFF;
            ActivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
        }
    } else
    {
        pStereoData->StereoSettings.dwFlags |= STEREO_RIGHTEYEINIT;
        if (pStereoData->StereoSettings.dwFlags & STEREO_DEFAULTON)
        {
            pStereoData->StereoModeNumber = pDXShare->dwModeNumber;
            pStereoData->StereoSettings.dwFlags &= ~STEREO_DEFAULTON;
            ActivateStereo((PNVD3DCONTEXT)pStereoData->dwContext);
        }
    }
}

#ifdef  MEMORY_TEST

#define MEM_SIZE 8192
#define __PUSH  __asm push eax __asm push edx __asm push ebx
#define __POP   __asm pop ebx __asm pop edx __asm pop eax
#define __SYNC  __asm xchg al,al __asm nop __asm nop
#define __RDTSC __SYNC __asm _emit 0x0f __asm _emit 0x31

#define _START(_t) { _Time[_t]=0; _RESTART(_t); }
#define _RESTART(_t) { __PUSH __asm mov eax,(_t) __asm lea ebx,[_Time+eax*8] __RDTSC __asm sub [ebx],eax __asm sbb [ebx+4],edx __POP }
#define _STOP(_t)  { __PUSH __asm mov eax,(_t) __asm lea ebx,[_Time+eax*8] __RDTSC __asm add [ebx],eax __asm adc [ebx+4],edx __POP }
__int64 _Time[256];

BOOL    PerformTest;
DWORD   NIterations = 1000000;
DWORD   MemSize = 32;

void ReadMemory(LPDWORD pAddr, DWORD TimeIndex)
{
    DWORD i = NIterations;
    DWORD dwNLoops = MemSize / sizeof(DWORD);
    nvAssert((((DWORD)pAddr) & 3) == 0);
    nvAssert(dwNLoops != 0);
    _START(TimeIndex);
    do
    {
        __asm {
            mov ecx, dwNLoops
            mov eax, pAddr
NextRead:
            mov edx, [eax+ecx*4-4]
            loop NextRead
        }
    } while (--i);
    _STOP(TimeIndex);
    _Time[TimeIndex] /= NIterations;
}

void WriteMemory(LPDWORD pAddr, DWORD TimeIndex)
{
    DWORD i = NIterations;
    DWORD dwNLoops = MemSize / sizeof(DWORD);
    nvAssert((((DWORD)pAddr) & 3) == 0);
    nvAssert(dwNLoops != 0);
    _START(TimeIndex);
    do
    {
        __asm {
            mov ecx, dwNLoops
            mov eax, pAddr
NextWrite:
            mov [eax+ecx*4-4], edx
            loop NextWrite
        }
    } while (--i);
    _STOP(TimeIndex);
    _Time[TimeIndex] /= NIterations;
}

void MemoryPerformanceTest(void)
{
    byte    TestSpace[MEM_SIZE];
    DWORD   *pAddr;
    DWORD   AGPAddr = 0;
#ifdef  TEST_VIDEO
    DWORD   VidMemAddr = 0;
    DWORD   dwStatus;
#endif  //TEST_VIDEO

    if (PerformTest)
    {
        AGPAddr = (DWORD)nvAGPAlloc(MEM_SIZE);
#ifdef  TEST_VIDEO
        NVHEAP_ALLOC (dwStatus, VidMemAddr, MEM_SIZE, TYPE_TEXTURE);
        if (dwStatus != 0)
        {
            //Failure
            VidMemAddr = 0;
        }
#endif  //TEST_VIDEO
    }
    while (PerformTest)
    {
        //System memory first
        pAddr = (DWORD *)(((DWORD)TestSpace + 3) & 0xFFFFFFFC);
        //ReadMemory(pAddr, 0);
        WriteMemory(pAddr, 1);
        if (AGPAddr)
        {
            pAddr = (DWORD *)(((DWORD)AGPAddr + 3) & 0xFFFFFFFC);
            //ReadMemory(pAddr, 2);
            WriteMemory(pAddr, 3);
        }
#ifdef  TEST_VIDEO
        if (VidMemAddr)
        {
            pAddr = (DWORD *)(((DWORD)VidMemAddr + 3) & 0xFFFFFFFC);
            //ReadMemory(pAddr, 4);
            WriteMemory(pAddr, 5);
        }
#endif  //TEST_VIDEO
        nvAssert(0);    //essentually the same as __asm int 3
    }
    if (AGPAddr) nvAGPFree((void*)AGPAddr);
#ifdef  TEST_VIDEO
    if (VidMemAddr) NVHEAP_FREE(VidMemAddr);
#endif  //TEST_VIDEO
}
#endif  //MEMORY_TEST

void StereoContextDestroy(PNVD3DCONTEXT pContext)
{
    if (!STEREO_ENABLED)
        return;
    NvSetStereoDataPtrFromDriverData;
    if (!STEREO_R_US(pContext))
        return;
    if (STEREO_ACTIVATED)
    {
        pStereoData->StereoSettings.dwFlags |= STEREO_FORCEDOFF;
        DeactivateStereo(pContext);
    }
#ifdef  LASER_SIGHT
    if (pLaser_Texture)
    {
        pLaser_Obj->release();
        pLaser_Obj  = NULL;
        pLaser_Texture = NULL;
    }
#endif  //LASER_SIGHT
    pStereoData->dwContext = 0;
    if (!pStereoData->StereoKeys.pHotKeyI)
        return;
    DESTROYSTEREOHOTKEY(pStereoData->StereoKeys.StereoToggle);
    DESTROYSTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustMore);
    DESTROYSTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustLess);
    DESTROYSTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustMore);
    DESTROYSTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustLess);
#ifdef  STEREO_CONFIG_ASSIST
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.SuggestKey);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.UnsuggestKey);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.WriteKey);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.DeleteKey);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.FrustumAdjust);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergence);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergenceRestore);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenMore);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenLess);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenMore);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenLess);
	if (STEREODATA(StereoSettings.dwFlags) & STEREO_GAMMACORRECTION)
	{
		DESTROYSTEREOHOTKEY(ConfigAssistInfo.GammaAdjustMore);
		DESTROYSTEREOHOTKEY(ConfigAssistInfo.GammaAdjustLess);
	}
#ifdef  LASER_SIGHT
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.LaserXPlusKey);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.LaserXMinusKey);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.LaserYPlusKey);
    DESTROYSTEREOHOTKEY(ConfigAssistInfo.LaserYMinusKey);
#endif  //LASER_SIGHT
#endif  //STEREO_CONFIG_ASSIST
#ifdef  SNAP_SHOT
    DESTROYSTEREOHOTKEY(SaveStereoImage);
#endif  //SNAP_SHOT

#ifdef USE_POPUP_LIST
        popupAgents.finiHotKeys();
#endif //USE_POPUP_LIST

    delete pStereoData->StereoKeys.pHotKeyI;
    pStereoData->StereoKeys.pHotKeyI = NULL;
}

#define	USE_PATTERN		1

typedef	struct Target_Conversion_Info_S 
{
	DWORD			dwFlags;
	CSimpleSurface *pTarget;
	DWORD			dwRop;
	DWORD			dwColorLeft;
	DWORD			dwColorRight;
	LPRECT			pRectLeft;
	LPRECT			pRectRight;
} TARGETCONVERSIONINFO, *PTARGETCONVERSIONINFO;
	
void TargetConversion(PTARGETCONVERSIONINFO pTargetConversionInfo)
{
    // make sure we get access to all of video memory
    bltUpdateClip(pDriverData);
    pDriverData->blitCalled = TRUE;
    pDriverData->TwoDRenderingOccurred = 1;
    // make sure we sync with other channels before writing put
    getDC()->nvPusher.setSyncChannelFlag();

    if (pDriverData->bltData.dwLastRop != pTargetConversionInfo->dwRop) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_ROP) | SET_ROP_OFFSET | 0x40000);
        nvPushData(1, pTargetConversionInfo->dwRop);
        nvPusherAdjust(2);
        pDriverData->bltData.dwLastRop = pTargetConversionInfo->dwRop;
    }
    if (pDriverData->bltData.dwLastColourKey != 0xFFFFFFFF) {
        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, NV_DD_CONTEXT_COLOR_KEY);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | SET_TRANSCOLOR_OFFSET | 0x40000);
        nvPushData(3, 0);
        nvPusherAdjust(4);
        pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
        pDriverData->dDrawSpareSubchannelObject = NV_DD_CONTEXT_COLOR_KEY;
    }
    CSimpleSurface *pSurf = pTargetConversionInfo->pTarget;
    DWORD dwPitch, dwSize, dwCombinedPitch, dwOffset, dwColourFormat, dwControlPoint;
    dwPitch = pSurf->getPitch();
    dwOffset = pSurf->getOffset();
    dwCombinedPitch = (dwPitch << 16) | dwPitch;
	if (pTargetConversionInfo->pRectLeft)
	{
		DWORD dwWidth, dwHeight;
		dwWidth  = pTargetConversionInfo->pRectLeft->right  - pTargetConversionInfo->pRectLeft->left;
		dwHeight = pTargetConversionInfo->pRectLeft->bottom - pTargetConversionInfo->pRectLeft->top;
		dwControlPoint = (pTargetConversionInfo->pRectLeft->left << 16) | pTargetConversionInfo->pRectLeft->top;
		dwSize = (dwWidth << 16) | dwHeight;
	} else
	{
		dwSize = (pSurf->getHeight() << 16) | pSurf->getWidth();
		dwControlPoint = 0;
	}
    if (pSurf->getBPP() == 2)
        dwColourFormat = NV062_SET_COLOR_FORMAT_LE_R5G6B5;
    else
        dwColourFormat = NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
    if (pDriverData->bltData.dwLastColourFormat != dwColourFormat ||
        pDriverData->bltData.dwLastCombinedPitch != dwCombinedPitch ||
        pDriverData->bltData.dwLastSrcOffset != dwOffset ||
        pDriverData->bltData.dwLastDstOffset != dwOffset) {

        nvPushData(0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
        nvPushData(1, dwColourFormat);                  // SetColorFormat
        nvPushData(2, dwCombinedPitch);                 // SetPitch
        nvPushData(3, dwOffset);                        // SetSrcOffset
        nvPushData(4, dwOffset);                        // SetDstOffset
        nvPusherAdjust(5);

        pDriverData->bltData.dwLastColourFormat = dwColourFormat;
        pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;
        pDriverData->bltData.dwLastSrcOffset = dwOffset;
        pDriverData->bltData.dwLastDstOffset = dwOffset;

    }

	if (pTargetConversionInfo->dwFlags & USE_PATTERN)
	{
		// also set mono pattern mask when pixel depth changes
		// scaled image and gdi_rect will silently fail unless we set the pattern (?)
		getDC()->nvPusher.push(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
		getDC()->nvPusher.push(1, NV_DD_CONTEXT_PATTERN);
		getDC()->nvPusher.push(2, dDrawSubchannelOffset(NV_DD_SPARE) + NV044_SET_PATTERN_SELECT | 0xC0000);
		getDC()->nvPusher.push(3, NV044_SET_PATTERN_SELECT_MONOCHROME);
		if (pSurf->getBPP() == 2)
		{
			getDC()->nvPusher.push(4,NV_ALPHA_1_016);
			getDC()->nvPusher.push(5,NV_ALPHA_1_016);
		}
		else
		{
			getDC()->nvPusher.push(4,NV_ALPHA_1_032);
			getDC()->nvPusher.push(5,NV_ALPHA_1_032);
		}
		getDC()->nvPusher.adjust(6);

		getDC()->nvPusher.push(0, dDrawSubchannelOffset(NV_DD_SPARE) + NV044_SET_MONOCHORME_COLOR0 | 0x80000);
		getDC()->nvPusher.push(1, pTargetConversionInfo->dwColorLeft);
		getDC()->nvPusher.push(2, pTargetConversionInfo->dwColorLeft);
		getDC()->nvPusher.adjust(3);

		nvPushData (0, dDrawSubchannelOffset(NV_DD_BLIT) | BLIT_POINT_IN_OFFSET | 0xC0000);
		nvPushData (1, dwControlPoint); 
		nvPushData (2, dwControlPoint); 
		nvPushData (3, dwSize);     
		nvPusherAdjust(4);

	} else
	{
		nvPushData (0, dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000);
		nvPushData (1, pTargetConversionInfo->dwColorLeft);
		nvPushData (2, dwControlPoint); 
		nvPushData (3, dwSize);     
        nvPusherAdjust(4);
	}

	if (pTargetConversionInfo->pRectRight)
	{
		DWORD dwWidth, dwHeight;
		dwWidth  = pTargetConversionInfo->pRectRight->right  - pTargetConversionInfo->pRectRight->left;
		dwHeight = pTargetConversionInfo->pRectRight->bottom - pTargetConversionInfo->pRectRight->top;
		dwControlPoint = (pTargetConversionInfo->pRectRight->left << 16) | pTargetConversionInfo->pRectRight->top;
		dwSize = (dwWidth << 16) | dwHeight;
	} else
	{
		dwSize = (pSurf->getHeight() << 16) | pSurf->getWidth();
		dwControlPoint = 0;
	}
    dwOffset = pSurf->getOffsetRight();
    nvPushData(0, dDrawSubchannelOffset(NV_DD_SURFACES) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
    nvPushData(1, dwColourFormat);                  // SetColorFormat
    nvPushData(2, dwCombinedPitch);                 // SetPitch
    nvPushData(3, dwOffset);                        // SetSrcOffset
    nvPushData(4, dwOffset);                        // SetDstOffset
    nvPusherAdjust(5);
	pDriverData->bltData.dwLastSrcOffset = dwOffset;
	pDriverData->bltData.dwLastDstOffset = dwOffset;

	if (pTargetConversionInfo->dwFlags & USE_PATTERN)
	{
		getDC()->nvPusher.push(0, dDrawSubchannelOffset(NV_DD_SPARE) + NV044_SET_MONOCHORME_COLOR0 | 0x80000);
		getDC()->nvPusher.push(1, pTargetConversionInfo->dwColorRight);
		getDC()->nvPusher.push(2, pTargetConversionInfo->dwColorRight);
		getDC()->nvPusher.adjust(3);

		nvPushData (0, dDrawSubchannelOffset(NV_DD_BLIT) | BLIT_POINT_IN_OFFSET | 0xC0000);
		nvPushData (1, dwControlPoint); 
		nvPushData (2, dwControlPoint); 
		nvPushData (3, dwSize);     
		nvPusherAdjust(4);
	} else
	{
		nvPushData (0, dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000);
		nvPushData (1, pTargetConversionInfo->dwColorRight);
		nvPushData (2, dwControlPoint); 
		nvPushData (3, dwSize);     
		nvPusherAdjust(4);
	}

    nvPusherStart(TRUE);
}

void StereoBackend(LPDDHAL_FLIPDATA pfd)
{
    if (STEREO_ACTIVATED)
    {
        if (STEREODATA(StereoSettings.dwFlags) & STEREO_ANAGLYPH == 0
		 && STEREODATA(dwStereoHWType) != STEREOCFG_LINECODE
		 && STEREODATA(StereoSettings.FrustumAdjustMode) != FRUSTUM_CLEAR_EDGES)
			//Nothing needs to be done
			return;

		CNvObject *pObj = GET_PNVOBJ(pfd->lpSurfTarg);
		if (!pObj) 
			return;
		TARGETCONVERSIONINFO TargetConversionInfo;
		memset(&TargetConversionInfo, 0, sizeof (TARGETCONVERSIONINFO));
		TargetConversionInfo.pTarget		= pObj->getSimpleSurface();

        if (STEREODATA(StereoSettings.dwFlags) & STEREO_ANAGLYPH)
        {
			//We need to do a color anaglyph conversion of the surface being flipped to
			CNvObject *pObj = GET_PNVOBJ(pfd->lpSurfTarg);
			if (!pObj) 
				return;
			TargetConversionInfo.dwFlags		= USE_PATTERN;
			TargetConversionInfo.dwRop			= MERGECOPY >> 16;
			TargetConversionInfo.dwColorLeft	= STEREODATA(dwLeftAnaglyphFilter);
			TargetConversionInfo.dwColorRight	= STEREODATA(dwRightAnaglyphFilter);
            TargetConversion(&TargetConversionInfo);
        }

		if (STEREODATA(StereoSettings.FrustumAdjustMode) == FRUSTUM_CLEAR_EDGES)
		{
			float   fFrustumScale = 1.f/(1.f - STEREODATA(StereoSettings.fStereoSeparation) * 0.5f);
			DWORD   dwWidth = TargetConversionInfo.pTarget->getWidth();
			RECT	RectLeft, RectRight;
			TargetConversionInfo.dwFlags		= 0;
			TargetConversionInfo.dwRop			= SRCCOPYINDEX;
			TargetConversionInfo.dwColorLeft	= 0;	//Black
			TargetConversionInfo.dwColorRight	= 0;	//Black	
			TargetConversionInfo.pRectLeft		= &RectLeft;
			TargetConversionInfo.pRectRight		= &RectRight;
			RectLeft.top		= 0;
			RectLeft.bottom		= TargetConversionInfo.pTarget->getHeight();
			RectLeft.right		= TargetConversionInfo.pTarget->getWidth();
			RectLeft.left		= ((DWORD)((float)(dwWidth)/fFrustumScale*1024.f+512.f))>>10;
			RectRight.top		= RectLeft.top;
			RectRight.bottom	= RectLeft.bottom;
			RectRight.left		= 0;
			RectRight.right		= RectLeft.right - RectLeft.left;
            TargetConversion(&TargetConversionInfo);
		}

		if (STEREODATA(dwStereoHWType) == STEREOCFG_LINECODE)
		{
			RECT	RectLeft, RectRight;
			TargetConversionInfo.dwFlags		= 0;
			TargetConversionInfo.dwRop			= SRCCOPYINDEX;
			TargetConversionInfo.dwColorLeft	= STEREODATA(dwLineCodeColor);
			TargetConversionInfo.dwColorRight	= STEREODATA(dwLineCodeColor);
			TargetConversionInfo.pRectLeft		= &RectLeft;
			TargetConversionInfo.pRectRight		= &RectRight;
			RectLeft.bottom		= TargetConversionInfo.pTarget->getHeight();
			RectLeft.top		= RectLeft.bottom - 1;
			RectLeft.left		= 0;
			RectLeft.right		= TargetConversionInfo.pTarget->getWidth() >> 2;
			RectRight.bottom	= RectLeft.bottom;
			RectRight.top		= RectLeft.top;
			RectRight.left		= 0;
			RectRight.right		= TargetConversionInfo.pTarget->getWidth() - RectLeft.right;
            TargetConversion(&TargetConversionInfo);
			TargetConversionInfo.dwColorLeft	= 0;	//Black
			TargetConversionInfo.dwColorRight	= 0;	//Black	
			RectLeft.left		= RectLeft.right;
			RectLeft.right		= TargetConversionInfo.pTarget->getWidth();
			RectRight.left		= RectRight.right;
			RectRight.right		= RectLeft.right;
            TargetConversion(&TargetConversionInfo);
		}
	}
}

void StereoAdjustmentMonitor(PNVD3DCONTEXT pContext)
{
    BOOL res_ = FALSE;
 
#ifdef  MEMORY_TEST
    MemoryPerformanceTest();
#endif  //MEMORY_TEST

    if (!STEREO_ENABLED)
        return;

#ifdef  DELAYED_HOTKEY_INIT
    if (FirstHotKeyInit)
    {
        if (!(pStereoData->StereoKeys.pHotKeyI = new CHOTKEYINTERFACE))
        {
            //Hot key support creation failed. We won't do anything special at this point like releasing the
            //StereoViewer. So all that previously done stuff will be standing by untill the driver gets unloaded.
            //It shouldn't cause any troubles because a game won't be able to activate stereo anyway. It would be
            //nice to somehow report the situation by putting message on the screen or generating some LogError
            //file. We'll figure it out later.
            return;
        }

        CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoToggle);
        CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustMore);
        CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustLess);
        CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustMore);
        CREATESTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustLess);
#ifdef  STEREO_CONFIG_ASSIST
        CREATESTEREOHOTKEY(ConfigAssistInfo.SuggestKey);
        CREATESTEREOHOTKEY(ConfigAssistInfo.UnsuggestKey);
        CREATESTEREOHOTKEY(ConfigAssistInfo.WriteKey);
        CREATESTEREOHOTKEY(ConfigAssistInfo.DeleteKey);
        CREATESTEREOHOTKEY(ConfigAssistInfo.FrustumAdjust);
        CREATESTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergence);
        CREATESTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergenceRestore);
        CREATESTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenMore);
        CREATESTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenLess);
        CREATESTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenMore);
        CREATESTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenLess);
		if (STEREODATA(StereoSettings.dwFlags) & STEREO_GAMMACORRECTION)
		{
			CREATESTEREOHOTKEY(ConfigAssistInfo.GammaAdjustMore);
			CREATESTEREOHOTKEY(ConfigAssistInfo.GammaAdjustLess);
		}
#ifdef  LASER_SIGHT
        CREATESTEREOHOTKEY(ConfigAssistInfo.LaserXPlusKey);
        CREATESTEREOHOTKEY(ConfigAssistInfo.LaserXMinusKey);
        CREATESTEREOHOTKEY(ConfigAssistInfo.LaserYPlusKey);
        CREATESTEREOHOTKEY(ConfigAssistInfo.LaserYMinusKey);
#endif  //LASER_SIGHT
#endif  //STEREO_CONFIG_ASSIST
#ifdef  SNAP_SHOT
        CREATESTEREOHOTKEY(SaveStereoImage);
#endif  //SNAP_SHOT

#ifdef  USE_POPUP_LIST
        popupAgents.initHotKeys();
#endif  //USE_POPUP_LIST

        FirstHotKeyInit = 0;
    }
#endif  //DELAYED_HOTKEY_INIT

#if 1
    if (++nFlips < 0x1000 && (nFlips & 0x7F) == 0)
        ENFORCEKEYBOARDFILTER();
#else
    if (++nFlips < 0x1000 && (nFlips & 0x7F) == 0)
    {
        ENFORCEKEYBOARDFILTER();
        if (res_)
        {
            //Start watching all over.
            nFlips = 0;
        }
    }
#endif
    //Keep an eye on the HW T&L use. Stereo works differently for HW and SW T&L!!!
    if(!NV_VERTEX_TRANSFORMED(pContext->pCurrentVShader))
        pStereoData->dwHWTnL++;

	//HW T&L has to prove itself as being used
    if ((nFlips & 0x1F) == 0)
		pStereoData->dwHWTnL = (pStereoData->dwHWTnL > 1) ? 1 : 0;

    //We need to turn stereo on if previously was forced to GDI
    if (pStereoData->StereoSettings.dwFlags & STEREO_FORCEDTOGDI)
    {
        pStereoData->StereoSettings.dwFlags &= ~STEREO_FORCEDTOGDI;
        if (!pStereoData->bStereoActivated)
            ActivateStereo(pContext);
    }

    POLLSTEREOHOTKEY(pStereoData->StereoKeys.StereoToggle)
    {
        if (pStereoData->bStereoActivated)
        {
            //Request to deactivate stereo.
            pStereoData->StereoModeNumber = 0;
            DeactivateStereo(pContext);
        } else
        {
            //Request to activate stereo.
            pStereoData->StereoModeNumber = pDXShare->dwModeNumber;
            ActivateStereo(pContext);
        }
    }

    if (!pStereoData->bStereoActivated)
    {
        //Nothing to do
        return;
    }

#ifdef  LASER_SIGHT
#ifdef  STEREO_CONFIG_ASSIST
    POLLSTEREOHOTKEY(ConfigAssistInfo.LaserXPlusKey)
    {
        pStereoData->StereoSettings.fLaserXAdjust += 0.001f;
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.LaserXMinusKey)
    {
        pStereoData->StereoSettings.fLaserXAdjust -= 0.001f;
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.LaserYPlusKey)
    {
        pStereoData->StereoSettings.fLaserYAdjust += 0.001f;
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.LaserYMinusKey)
    {
        pStereoData->StereoSettings.fLaserYAdjust -= 0.001f;
    }

#endif  //STEREO_CONFIG_ASSIST

    if (pStereoData->StereoSettings.dwFlags & STEREO_LASERSIGHT)
    {
        //We are going to use our proprietary laser site
#if (NVARCH >= 0x020)
        CSimpleSurface *pZetaBuffer = ((PNVD3DCONTEXT)(pStereoData->dwContext))->kelvinAA.GetCurrentZB(pContext);
#else
        CSimpleSurface *pZetaBuffer = ((PNVD3DCONTEXT)(pStereoData->dwContext))->pZetaBuffer;
        #error Not implemented yet (If ever will).
#endif
        //Instead of doing WaitForIdle (TRUE,FALSE) in order to get access to the Z buffer
        //we can do a cpuLock on Z buffer. It will be more efficient.
        if (pZetaBuffer)
        {
            DWORD   dwCenterValue;
            float SZ_Center = 0.0f;
            float RHW_Center;
            pZetaBuffer->cpuLock(CSimpleSurface::LOCK_NORMAL);
            float CenterX = (pZetaBuffer->getWidth() >> 1)  * pStereoData->StereoSettings.fLaserXAdjust;
            float CenterY = (pZetaBuffer->getHeight() >> 1) * pStereoData->StereoSettings.fLaserYAdjust;
            float CenterX_RT = (((PNVD3DCONTEXT)(pStereoData->dwContext))->pRenderTarget->getWidth() >> 1)  * pStereoData->StereoSettings.fLaserXAdjust;
            float CenterY_RT = (((PNVD3DCONTEXT)(pStereoData->dwContext))->pRenderTarget->getHeight() >> 1) * pStereoData->StereoSettings.fLaserYAdjust;
#ifdef  EXPERIMENT
            __asm int 3
#endif  //EXPERIMENT
            DWORD pLeftEyeCenter = (DWORD)pZetaBuffer->getAddress() + CenterY
                                          * pZetaBuffer->getPitch() + CenterX
                                          * pZetaBuffer->getBPP();
            if (pZetaBuffer->getBPP() == 2)
                dwCenterValue = (DWORD)*(WORD *)pLeftEyeCenter;
            else
                dwCenterValue = (*(DWORD *)pLeftEyeCenter) >> 8;
            //nvAssert(pContext->dwRenderState[D3DRENDERSTATE_ZBIAS] == 0);
#if 0
            float fZBias, fZScale;
            fZBias  = -(float)(pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]);
            fZScale = 0.25f * fZBias;
            fZBias  *= pContext->hwState.celsius.dvInvZScale;
            fZScale *= pContext->hwState.celsius.dvInvZScale;
#endif
#ifdef  DEBUG
            //We need to process the Center Point based on the ZFUNC below
            DWORD CompFunc = pContext->dwRenderState[D3DRENDERSTATE_ZFUNC];
            switch (CompFunc)
            {
            case D3DCMP_NEVER:
                nvAssert(0);
                break;

            case D3DCMP_LESS:
                nvAssert(0);
                break;

            case D3DCMP_EQUAL:
                nvAssert(0);
                break;

            case D3DCMP_LESSEQUAL:
                //nvAssert(0);
                break;

            case D3DCMP_GREATER:
                nvAssert(0);
                break;

            case D3DCMP_NOTEQUAL:
                nvAssert(0);
                break;

            case D3DCMP_GREATEREQUAL:
                nvAssert(0);
                break;

            case D3DCMP_ALWAYS:
                nvAssert(0);
                break;
            }
#endif  //DEBUG
#if 1
            if (pStereoData->dwHWTnL)
            {
                //Calculating Sz & rhw values for HW T&L is a little bit tricky. Watch my hands.
                //First we're going to calculate the expected laser sight X offset for the central
                //point. The existing StereoConvergenceMultiplier is just a default value: it
                //can't be correct for dwHWTnL games. But we don't care because we're going
                //to do a reversed transform of expected stereo screen coordinates to appropriate
                //Sz & rhw in order for our general stereo mechanizm to produce those expected
                //stereo screen coordinates based on existing StereoConvergenceMultiplier.
                float SZOverRange, ZRange, WRange;
                float XCorrection;
                float fScreenHalfWidth;
                //__asm int 3
#if 1
                float rhwMax = 100.f;
                if (pContext->surfaceViewport.dvWNear != 0)
                    rhwMax = 1.f/pContext->surfaceViewport.dvWNear;
                float Divider = rhwMax * pStereoData->StereoSettings.fStereoConvergence;
                if (Divider != 0)
                    pStereoData->StereoSettings.fConvergenceMultiplier = 1.f/Divider;
                else
                    pStereoData->StereoSettings.fConvergenceMultiplier = 0.0f;
#if 0
                pStereoData->StereoSettings.fPostStereoConvergence = pStereoData->StereoSettings.fStereoConvergence
                                                      * pStereoData->StereoSettings.fConvergenceMultiplier;
#else
                pStereoData->StereoSettings.fPostStereoConvergence = pStereoData->StereoSettings.fStereoConvergence;
#endif
#endif
                fScreenHalfWidth = (float)(pZetaBuffer->getWidth() >> 1);
#if (NVARCH >= 0x010)
#if 0
                float fZBias, fZScale;
                fZBias  = -(float)(pContext->dwRenderState[D3DRENDERSTATE_ZBIAS]);
#endif
                if (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW)
                {
                    WRange = pContext->surfaceViewport.dvWFar - pContext->surfaceViewport.dvWNear;
                    float W_Center = ((float)dwCenterValue) * (pContext->surfaceViewport.dvWFar * pContext->hwState.dvInvZScale);
                    RHW_Center = 1.0f/W_Center;
                    SZOverRange = (W_Center - pContext->surfaceViewport.dvWNear) / WRange;
                    XCorrection = (pStereoData->StereoSettings.fStereoSeparation * SZOverRange - pStereoData->StereoSettings.fStereoConvergence) * fScreenHalfWidth;
                    XCorrection = max(XCorrection,0.f); //If negative paralax show no separation
                } else
                {
                    pStereoData->StereoSettings.dwFlags |= STEREO_LASERGETRHWSCALE;
                    SZ_Center = ((float)dwCenterValue) * pContext->hwState.dvInvZScale;
                    RHW_Center = LaserRHWBase + (SZ_Center - LaserSZBase)*LaserRHWScale;
                    ZRange = pContext->surfaceViewport.dvMaxZ - pContext->surfaceViewport.dvMinZ;
                    SZOverRange = (SZ_Center - pContext->surfaceViewport.dvMinZ) / (pContext->surfaceViewport.dvMaxZ - pContext->surfaceViewport.dvMinZ);
                    XCorrection = (pStereoData->StereoSettings.fStereoSeparation * SZOverRange - pStereoData->StereoSettings.fStereoConvergence) * fScreenHalfWidth;
                    XCorrection = max(XCorrection,0.f); //If negative paralax show no separation
                }
#else
                #error Not implemented yet (If ever will).
#endif
            } else
            {
#if (NVARCH >= 0x010)
                if (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW)
                {
                    float W_Center = ((float)dwCenterValue) * (pContext->surfaceViewport.dvWFar * pContext->hwState.dvInvZScale);
                    RHW_Center = 1.0f/W_Center;
                } else
                {
                    pStereoData->StereoSettings.dwFlags |= STEREO_LASERGETRHWSCALE;
                    SZ_Center = ((float)dwCenterValue) * pContext->hwState.dvInvZScale;
                    RHW_Center = LaserRHWBase + (SZ_Center - LaserSZBase)*LaserRHWScale;
                }
#else
                #error Not implemented yet (If ever will).
#endif
            }
#endif
#if 0
#if (NVARCH >= 0x010)
            if (pContext->dwRenderState[D3DRENDERSTATE_ZENABLE] == D3DZB_USEW)
            {
                float W_Center = ((float)dwCenterValue) * (pContext->surfaceViewport.dvWFar * pContext->hwState.dvInvZScale);
                RHW_Center = 1.0f/W_Center;
            } else
            {
                pStereoData->StereoSettings.dwFlags |= STEREO_LASERGETRHWSCALE;
                SZ_Center = ((float)dwCenterValue) * pContext->hwState.dvInvZScale;
                RHW_Center = LaserRHWBase + (SZ_Center - LaserSZBase)*LaserRHWScale;
            }
#else
            #error Not implemented yet (If ever will).
#endif
#endif
            float fRealConvergence = pStereoData->dwHWTnL ? pStereoData->StereoSettings.fStereoConvergence : pStereoData->StereoSettings.fPostStereoConvergence;
            float LaserSize = LaserMinSize + (LaserMaxSize - LaserMinSize)*RHW_Center * fRealConvergence;
            if (LaserSize > LaserMaxSize)
            {
                //Popped up with negative paralax
                LaserSize = LaserMaxSize;
            }
            typedef struct {
                DDRAWI_DDRAWSURFACE_GBL_MORE ddGblMore;
                LPDWORD                      dwReserved1;
                DDRAWI_DDRAWSURFACE_GBL      ddGbl;
            } EXT_DDRAWI_DDRAWSURFACE_GBL;
            DDRAWI_DDRAWSURFACE_LCL     LaserDP2Command_Lcl;
            EXT_DDRAWI_DDRAWSURFACE_GBL eddGbl;
            eddGbl.ddGbl.fpVidMem = (FLATPTR)&LaserDP2Command;
            LaserDP2Data.lpDDCommands = &LaserDP2Command_Lcl;
            eddGbl.dwReserved1 = (LPDWORD)&eddGbl.ddGblMore;
            LaserDP2Command_Lcl.lpGbl = &eddGbl.ddGbl;
            SET_PNVOBJ (&LaserDP2Command_Lcl, NULL);
            LPDDHALMODEINFO lpModeInfo = &pDriverData->ModeList[pDXShare->dwModeNumber];
#ifndef EXPERIMENT
            LaserVertices[0].dvSX = LaserVertices[1].dvSX = LaserVertices[2].dvSX =
                                    LaserVertices[3].dvSX = CenterX_RT;
            LaserVertices[0].dvSY = LaserVertices[1].dvSY = LaserVertices[2].dvSY =
                                    LaserVertices[3].dvSY = CenterY_RT;
            LaserVertices[0].dvSX -= LaserSize;
            LaserVertices[0].dvSY -= LaserSize;
            LaserVertices[1].dvSX += LaserSize;
            LaserVertices[1].dvSY -= LaserSize;
            LaserVertices[2].dvSX -= LaserSize;
            LaserVertices[2].dvSY += LaserSize;
            LaserVertices[3].dvSX += LaserSize;
            LaserVertices[3].dvSY += LaserSize;
#else   //EXPERIMENT
            LaserSize = 4.f;
            LaserVertices[0].dvSX = LaserVertices[1].dvSX = LaserVertices[2].dvSX =
                                    LaserVertices[3].dvSX = LaserSize;
            LaserVertices[0].dvSY = LaserVertices[1].dvSY = LaserVertices[2].dvSY =
                                    LaserVertices[3].dvSY = LaserSize;
            LaserVertices[0].dvSX -= LaserSize;
            LaserVertices[0].dvSY -= LaserSize;
            LaserVertices[1].dvSX += LaserSize;
            LaserVertices[1].dvSY -= LaserSize;
            LaserVertices[2].dvSX -= LaserSize;
            LaserVertices[2].dvSY += LaserSize;
            LaserVertices[3].dvSX += LaserSize;
            LaserVertices[3].dvSY += LaserSize;
#endif  //EXPERIMENT
            SetupLaserStates(pContext);
            //Make it a little bit closer;
#ifndef EXPERIMENT
            SZ_Center *= 0.98f;
            RHW_Center *= 1.02f;
#else   //EXPERIMENT
            SZ_Center = 0.f;
            RHW_Center *= 1.02f;
#endif  //EXPERIMENT
            LaserVertices[0].dvSZ = LaserVertices[1].dvSZ = LaserVertices[2].dvSZ = LaserVertices[3].dvSZ = SZ_Center;
            LaserVertices[0].dvRHW = LaserVertices[1].dvRHW = LaserVertices[2].dvRHW = LaserVertices[3].dvRHW = RHW_Center;
            LaserDP2Data.dwhContext = (DWORD)pContext;
            LaserDP2Data.dwVertexSize = sizeof (D3DTLVERTEX);
            //To make the laser sight properly stereoized we need to clear the
            //pStereoData->dwHWTnL. Otherwise our algorithm considers D3DTLVERTEX
            //to be a sign of 2D object.
            DWORD dwHWTnL = pStereoData->dwHWTnL;  //Save it
#ifndef EXPERIMENT
            pStereoData->dwHWTnL = 0;
#else   //EXPERIMENT
            pStereoData->dwHWTnL = 1;
#endif  //EXPERIMENT
            DWORD dwDXAppVersion = pContext->dwDXAppVersion;
            pContext->dwDXAppVersion = 0x700;
            nvDrawPrimitives2(&LaserDP2Data);
            pContext->dwDXAppVersion = dwDXAppVersion;
            pStereoData->dwHWTnL = dwHWTnL;
			RestorePreLaserStates(pContext);
		}
    }
#endif  //LASER_SIGHT

	/////////// render a list of PopupAgent 

#ifdef USE_POPUP_LIST
    //we'll check inside whether we'd really render
	popupAgents.render(pContext);
#endif //USE_POPUP_LIST

    if (pStereoData->StereoSettings.dwFlags & STEREO_AUTOCONVERGENCE && pStereoData->fFrameRHWMax != -9999.f)
    {
        //We need to adjust the pStereoData->StereoSettings.fPostStereoConvergence value based on the depth
        //of the closest object in the current scene. This value will take effect in the next frame.
        //To make this adjustment smooth we want to do an inert transition not exceeding say 5% of the
        //difference between the current pStereoData->StereoSettings.fPostStereoConvergence and ideal value for that
        //being calculated below per frame.
        D3DVALUE fIdealMultiplier = 1.f/(pStereoData->fFrameRHWMax * pStereoData->StereoSettings.fStereoConvergence);
        pStereoData->StereoSettings.fConvergenceMultiplier += (fIdealMultiplier - pStereoData->StereoSettings.fConvergenceMultiplier)
                                              * pStereoData->StereoSettings.AutoConvergenceAdjustPace;
        pStereoData->StereoSettings.fPostStereoConvergence = pStereoData->StereoSettings.fStereoConvergence;
    }
#ifdef  STEREO_CONFIG_ASSIST
    if (ConfigAssistInfo.rhwMax < pStereoData->fFrameRHWMax)
        ConfigAssistInfo.rhwMax = pStereoData->fFrameRHWMax;
#endif  //STEREO_CONFIG_ASSIST
#ifdef  FLAT_STAT
    dwNFlatObjects = 0;
#endif  //FLAT_STAT
    pStereoData->fFrameRHWMax     = -9999.f;

    POLLSTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustMore)
    {
        pStereoData->StereoSettings.fStereoSeparation += 0.002f;
        //Clamp it.
        if (pStereoData->StereoSettings.fStereoSeparation > MAX_STEREO_SEPARATION)
            pStereoData->StereoSettings.fStereoSeparation = MAX_STEREO_SEPARATION;
        pStereoData->StereoSettings.dwFlags |= STEREO_SEPARATION;
        AdjustStereoParameters(pContext);
    }

    POLLSTEREOHOTKEY(pStereoData->StereoKeys.StereoSeparationAdjustLess)
    {
        pStereoData->StereoSettings.fStereoSeparation -= 0.002f;
        //Clamp it.
        if (pStereoData->StereoSettings.fStereoSeparation < 0.f)
            pStereoData->StereoSettings.fStereoSeparation = 0.f;
        pStereoData->StereoSettings.dwFlags |= STEREO_SEPARATION;
        AdjustStereoParameters(pContext);
    }

    //StereoConvergence adjustment is a little bit tricky. First of all we dont want it to be 
    //linear because in some games when an optimal value is really low smal increment makes big 
    //difference. On the other hand some games like Forsaken require a huge number (~1000+) and 
    //we don't want to hold the key for minutes in order to see some noticable change. So we'll
    //make a step size exponential. Disadvantage of exponential function is that it never flips 
    //the sigh of the convergence. So we're doomed to being able to adjust in one direction only
    //(bringing things out of the screen). If we need to go the other way we need to change the 
    //sign. Here is how we're going to do it. If the convergence gets within the range 
    //(-CONVERGENCE_THRESHOLD, CONVERGENCE_THRESHOLD) then every change that makes it closer to 
    //zero flips the sign. This way we can say that StereoConvergenceAdjustMore always brings 
    //objects out of the monitor and StereoConvergenceAdjustLess does the opposite.
    POLLSTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustMore)
    {
        if (pStereoData->StereoSettings.fStereoConvergence > 0)
            pStereoData->StereoSettings.fStereoConvergence *= 1.02f;
        else
        {
            pStereoData->StereoSettings.fStereoConvergence *= (1.f/1.02f);
            if (pStereoData->StereoSettings.fStereoConvergence > -CONVERGENCE_THRESHOLD)
                pStereoData->StereoSettings.fStereoConvergence = -pStereoData->StereoSettings.fStereoConvergence;
        }
        pStereoData->StereoSettings.dwFlags |= STEREO_CONVERGENCE;
        AdjustStereoParameters(pContext);
    }

    POLLSTEREOHOTKEY(pStereoData->StereoKeys.StereoConvergenceAdjustLess)
    {
        if (pStereoData->StereoSettings.fStereoConvergence > 0)
        {
            pStereoData->StereoSettings.fStereoConvergence *= (1.f/1.02f);
            if (pStereoData->StereoSettings.fStereoConvergence < CONVERGENCE_THRESHOLD)
                pStereoData->StereoSettings.fStereoConvergence = -pStereoData->StereoSettings.fStereoConvergence;
        }
        else
            pStereoData->StereoSettings.fStereoConvergence *= 1.02f;
        pStereoData->StereoSettings.dwFlags |= STEREO_CONVERGENCE;
        AdjustStereoParameters(pContext);
    }

#ifdef  STEREO_CONFIG_ASSIST
    POLLSTEREOHOTKEY(ConfigAssistInfo.SuggestKey)
    {
        SuggestStereoSettings();
        AdjustStereoParameters(pContext);
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.UnsuggestKey)
    {
        UnsuggestStereoSettings();
        AdjustStereoParameters(pContext);
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.WriteKey)
    {
        DumpConfigInfo();
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.DeleteKey)
    {
        DeleteConfigInfo();
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.FrustumAdjust)
    {
        if (++pStereoData->StereoSettings.FrustumAdjustMode > FRUSTUM_CLEAR_EDGES)
            pStereoData->StereoSettings.FrustumAdjustMode = NO_FRUSTUM_ADJUST;
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergenceRestore)
    {
        if (pStereoData->StereoSettings.dwFlags & STEREO_AUTOCONVERGENCE)
        {
            //Going back to manual. Need to restore what was there before.
            pStereoData->StereoSettings.fConvergenceMultiplier = ConfigAssistInfo.fLastNonAutoConvergenceMultiplier;
            pStereoData->StereoSettings.fPostStereoConvergence = pStereoData->StereoSettings.fStereoConvergence;
        } else
        {
            //Going to auto. Need to save the manual convergence in order to be able
            //to restore if we're screwed up.
            ConfigAssistInfo.fLastNonAutoConvergenceMultiplier = pStereoData->StereoSettings.fConvergenceMultiplier;
        }
        pStereoData->StereoSettings.dwFlags ^= STEREO_AUTOCONVERGENCE;
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.ToggleAutoConvergence)
    {
        pStereoData->StereoSettings.dwFlags ^= STEREO_AUTOCONVERGENCE;
    }

    float fRHWAtScreenDelta;

    POLLSTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenMore)
    {
        if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
        {
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWGREATERATSCREEN;
                pStereoData->StereoSettings.fRHWGreaterAtScreen = ConfigAssistInfo.szMin;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.szMax - ConfigAssistInfo.szMin) / 100.f;
            pStereoData->StereoSettings.fRHWGreaterAtScreen += fRHWAtScreenDelta;
            ConfigAssistInfo.szMin += fRHWAtScreenDelta;
        } else
        {
            //More in reality means less. So farther objects are put at the screen plane.
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWGREATERATSCREEN;
                pStereoData->StereoSettings.fRHWGreaterAtScreen = ConfigAssistInfo.rhwMax;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.rhwMax - ConfigAssistInfo.rhwMin) / 100.f;
            pStereoData->StereoSettings.fRHWGreaterAtScreen -= fRHWAtScreenDelta;
            ConfigAssistInfo.rhwMax -= fRHWAtScreenDelta;
        }
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.RHWAtScreenLess)
    {
        if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
        {
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWGREATERATSCREEN;
                pStereoData->StereoSettings.fRHWGreaterAtScreen = ConfigAssistInfo.szMin;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.szMax - ConfigAssistInfo.szMin) / 100.f;
            pStereoData->StereoSettings.fRHWGreaterAtScreen -= fRHWAtScreenDelta;
            ConfigAssistInfo.szMin -= fRHWAtScreenDelta;
        } else
        {
            //Less in reality means more. So closer objects are put at the screen plane.
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWGREATERATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWGREATERATSCREEN;
                pStereoData->StereoSettings.fRHWGreaterAtScreen = ConfigAssistInfo.rhwMax;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.rhwMax - ConfigAssistInfo.rhwMin) / 100.f;
            pStereoData->StereoSettings.fRHWGreaterAtScreen += fRHWAtScreenDelta;
            ConfigAssistInfo.rhwMax += fRHWAtScreenDelta;
        }
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenMore)
    {
        if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
        {
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
                pStereoData->StereoSettings.fRHWLessAtScreen = ConfigAssistInfo.szMax;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.szMax - ConfigAssistInfo.szMin) / 100.f;
            pStereoData->StereoSettings.fRHWLessAtScreen -= fRHWAtScreenDelta;
            ConfigAssistInfo.szMax -= fRHWAtScreenDelta;
        } else
        {
            //More in reality means less. So farther objects are put at the screen plane.
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
                pStereoData->StereoSettings.fRHWLessAtScreen = ConfigAssistInfo.rhwMin;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.rhwMax - ConfigAssistInfo.rhwMin) / 100.f;
            pStereoData->StereoSettings.fRHWLessAtScreen += fRHWAtScreenDelta;
            ConfigAssistInfo.rhwMin += fRHWAtScreenDelta;
        }
    }

    POLLSTEREOHOTKEY(ConfigAssistInfo.RHWLessAtScreenLess)
    {
        if (pStereoData->StereoSettings.dwFlags & STEREO_FAVORSZOVERRHW)
        {
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
                pStereoData->StereoSettings.fRHWLessAtScreen = ConfigAssistInfo.szMax;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.szMax - ConfigAssistInfo.szMin) / 100.f;
            pStereoData->StereoSettings.fRHWLessAtScreen += fRHWAtScreenDelta;
            ConfigAssistInfo.szMax += fRHWAtScreenDelta;
        } else
        {
            //Less in reality means more. So closer objects are put at the screen plane.
            if (!(pStereoData->StereoSettings.dwFlags & STEREO_RHWLESSATSCREEN))
            {
                //Has not been set yet
                pStereoData->StereoSettings.dwFlags |= STEREO_RHWLESSATSCREEN;
                pStereoData->StereoSettings.fRHWLessAtScreen = ConfigAssistInfo.rhwMin;
            }
            fRHWAtScreenDelta = (ConfigAssistInfo.rhwMax - ConfigAssistInfo.rhwMin) / 100.f;
            pStereoData->StereoSettings.fRHWLessAtScreen -= fRHWAtScreenDelta;
            ConfigAssistInfo.rhwMin -= fRHWAtScreenDelta;
        }
    }

    if (STEREODATA(StereoSettings.dwFlags) & STEREO_GAMMACORRECTION)
	{
		POLLSTEREOHOTKEY(ConfigAssistInfo.GammaAdjustMore)
		{
			//Gamma adjustment for stereo.
			if (pStereoData->StereoSettings.fStereoGamma < 6.f)
			{
				pStereoData->StereoSettings.fStereoGamma += 0.1f;
				WORD    NewGamma[256 * 3];
				DWORD   i;
				HDC hDC = GetDC(NULL);
				if (hDC)
				{
					WORD  NewVal;
					for (i = 0; i < 256; i++)
					{

						NewVal = (WORD)(ComputeGammaColor (i, pStereoData->StereoSettings.fStereoGamma) << 8); 
						NewGamma[i] = NewVal;
						NewGamma[i+256] = NewVal;
						NewGamma[i+512] = NewVal;
					}
					SetDeviceGammaRamp(hDC, NewGamma); 
					ReleaseDC(NULL,hDC);
				}
			}
		}

		POLLSTEREOHOTKEY(ConfigAssistInfo.GammaAdjustLess)
		{
			//Gamma adjustment for stereo.
			if (pStereoData->StereoSettings.fStereoGamma > 0.5f)
			{
				pStereoData->StereoSettings.fStereoGamma -= 0.1f;
				WORD    NewGamma[256 * 3];
				DWORD   i;
				HDC hDC = GetDC(NULL);
				if (hDC)
				{
					WORD  NewVal;
					for (i = 0; i < 256; i++)
					{

						NewVal = (WORD)(ComputeGammaColor (i, pStereoData->StereoSettings.fStereoGamma) << 8); 
						NewGamma[i] = NewVal;
						NewGamma[i+256] = NewVal;
						NewGamma[i+512] = NewVal;
					}
					SetDeviceGammaRamp(hDC, NewGamma); 
					ReleaseDC(NULL,hDC);
				}
			}
		}
    }

#endif  //STEREO_CONFIG_ASSIST
}

DWORD GetStereoOffset(CSimpleSurface *pSurf)
{
    if (!STEREO_ACTIVATED || !pSurf->isStereo() || pStereoData->dwLastEye != EYE_RIGHT)
        return pSurf->getOffset();
    return pSurf->getOffsetRight();
}

DWORD StereoBltAdjustParameters(SURFINFO &src, SURFINFO &dst, STEREOBLTPASSINFO &StereoBltPass)
{
    if (!STEREO_ACTIVATED)
        return 1;

    if (!dst.pObj)
    {
		//LOG("StereoBltAdjustParameters: destination surface has invalid nvObj");
        //Doesn't have a valid nvObj - bail out
        return 1;
    }
    CSimpleSurface *pDstSurf = dst.pObj->getSimpleSurface();
    if (!pDstSurf)
    {
        //The destination is not a simple surface (texture) - bail out.
        return 1;
    }
    if (!pDstSurf->isStereo())
    {
        //Surface is not stereo - bail out.
        return 1;
    }

    StereoLockSubstituteFlush(); //Will cause recursive blit

    if (src.pObj)
    {
        //Check if the source is a stereo surface
        CSimpleSurface *pSrcSurf = src.pObj->getSimpleSurface();
        if (pSrcSurf && pSrcSurf->isStereo())
        {
            //Some games like Half-life use blit istead of flip. So we don't want to apply stereo twice.
            //Warning!!! This is not going to look correct unless the blit involves full sizes of surfaces.
            //We will check for that even if it isn't clear what to do if this check fails.
            if (src.dwSubRectWidth != pSrcSurf->getWidth() || src.dwSubRectWidth != dst.dwSubRectWidth)
			{
				//LOG("StereoBltAdjustParameters: blt width mismatches");
				nvAssert(0);
			}
            if (src.dwSubRectHeight != pSrcSurf->getHeight() || src.dwSubRectHeight != dst.dwSubRectHeight)
			{
				//LOG("StereoBltAdjustParameters: blt height mismatches");
                nvAssert(0);
			}
            if (StereoBltPass.dwPass)
                src.dwOffset = StereoBltPass.dwSrcOffset;
            else
                StereoBltPass.dwSrcOffset = pSrcSurf->getOffsetRight();
        }
    }
#ifdef  SNAP_SHOT
    else
    {
        //Check for the stereo image display mode.
        if (StereoBltPass.dwPass && StereoBltPass.dwSrcY == NVSTEREO_IMAGE_SIGNATURE)
        {
            //Right eye of still stereo image.
            StereoBltPass.dwPass = 0;
            src.dwSubRectX = src.dwSubRectWidth;
            dst.dwOffset = StereoBltPass.dwDstOffset;
            src.dwOffset = 0;
            return 1;
        } else
        {
            if (StereoBltPass.dwPass && StereoBltPass.dwSrcY == NVSTEREO_IMAGE_SIGNATURE + 1)
            {
                //Right eye of still stereo image.
                StereoBltPass.dwPass = 0;
                src.dwSubRectX = 0;
                dst.dwOffset = StereoBltPass.dwDstOffset;
                src.dwOffset = 0;
                return 1;
            }
        }

        if (src.pLcl)
        {
            LPNVSTEREOIMAGEHEADER pStereoImageHeader = (LPNVSTEREOIMAGEHEADER)((LPBYTE)src.fpVidMem+(src.dwHeight-1)*src.dwPitch);
            if (pStereoImageHeader->dwSignature == NVSTEREO_IMAGE_SIGNATURE
                && pStereoImageHeader->dwHeight == src.dwSubRectHeight
                && pStereoImageHeader->dwWidth == src.dwSubRectWidth)
            {
                src.dwSubRectHeight -= 1;
                src.dwSubRectWidth >>= 1;
                StereoBltPass.dwSrcY = NVSTEREO_IMAGE_SIGNATURE;
                StereoBltPass.dwDstOffset = pDstSurf->getOffsetRight();
                StereoBltPass.dwPass = 1; //We expect the second pass later
                if (pStereoImageHeader->dwSwapEyes)
                {
                    src.dwSubRectX = src.dwSubRectWidth;
                    StereoBltPass.dwSrcY += 1;
                }
                return 2;
            }
        }
    }
#endif  //SNAP_SHOT
    if (StereoBltPass.dwPass)
    {
        StereoBltPass.dwPass = 0;
        dst.dwOffset = StereoBltPass.dwDstOffset;
        return 1;
    }
    StereoBltPass.dwDstOffset = pDstSurf->getOffsetRight();
    StereoBltPass.dwPass = 1; //We expect the second pass later
    return 2;
}

BOOL CreateLocalCopy(LPDDRAWI_DDRAWSURFACE_LCL pSrcLcl, LPDDRAWI_DDRAWSURFACE_LCL& pDstLcl)
{
    LPDDRAWI_DDRAWSURFACE_LCL  pDDSNewLcl;
    LPDDRAWI_DDRAWSURFACE_GBL  pDDSGbl;
    LPDDRAWI_DDRAWSURFACE_MORE pSurfMore;

    if (!(pDDSNewLcl = pDstLcl))
    {
        //Destination local doesn't exist yet.
        if (!(pDDSNewLcl = (LPDDRAWI_DDRAWSURFACE_LCL)AllocIPM( sizeof (DDRAWI_DDRAWSURFACE_LCL)  +
                                                                sizeof (DDRAWI_DDRAWSURFACE_MORE) +
                                                                sizeof (DDRAWI_DDRAWSURFACE_GBL)  +
                                                                sizeof (LPDWORD)                  +
                                                                sizeof (DDRAWI_DDRAWSURFACE_GBL_MORE))))
        {
	        //LOG("CreateLocalCopy");
            //__asm int 3
			nvAssert(0);
            return FALSE;
        }
        pDDSNewLcl->lpSurfMore = (LPDDRAWI_DDRAWSURFACE_MORE)((LPBYTE)pDDSNewLcl + sizeof (DDRAWI_DDRAWSURFACE_LCL));
        pDDSNewLcl->lpGbl = (LPDDRAWI_DDRAWSURFACE_GBL)((LPBYTE)pDDSNewLcl->lpSurfMore + sizeof (DDRAWI_DDRAWSURFACE_MORE)
                                                                                       + sizeof (LPDWORD));
        GET_LPDDRAWSURFACE_GBL_MORE(pDDSNewLcl->lpGbl) = (LPDDRAWI_DDRAWSURFACE_GBL_MORE)((LPBYTE)pDDSNewLcl->lpGbl
                                                                           + sizeof (DDRAWI_DDRAWSURFACE_GBL));
        pDstLcl = pDDSNewLcl;
    }
    pDDSGbl   = pDDSNewLcl->lpGbl;
    pSurfMore = pDDSNewLcl->lpSurfMore;
    //Copy the pDDSLcl content to the brand new allocated local struture.
    *pDDSNewLcl = *pSrcLcl;
    //Restore our pDDSGbl and pSurfMore
    pDDSNewLcl->lpGbl       = pDDSGbl;
    pDDSNewLcl->lpSurfMore  = pSurfMore;
    //Copy the pDDSLcl->lpGbl and pDDSLcl->lpSurfMore content to the brand new allocated local->global structure.
    *pDDSGbl    = *pSrcLcl->lpGbl;
#if 1
    nvAssert (pDDSGbl->lpDD->dwReserved3);
#endif
    *pSurfMore  = *pSrcLcl->lpSurfMore;
    //Copy the GblMore content to the brand new allocated GblMore struture.
    *(GET_LPDDRAWSURFACE_GBL_MORE(pDDSGbl)) = *(GET_LPDDRAWSURFACE_GBL_MORE(pSrcLcl->lpGbl));
    return TRUE;
}

BOOL CreateLockSubstitute(LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl)
{
    LPDDRAWI_DDRAWSURFACE_LCL  pDDSLockLcl;
    LPDDRAWI_DDRAWSURFACE_GBL  pDDSGbl;
    DDHAL_CREATESURFACEDATA CreateSurfaceData;
    DDHAL_DESTROYSURFACEDATA DestroySurfaceData;

    if (!STEREO_ENABLED)
        return FALSE;

    if (pStereoData->pLockSubstituteLcl)
    {
        memset(&DestroySurfaceData, 0, sizeof (DDHAL_DESTROYSURFACEDATA));
        DestroySurfaceData.lpDD = pDDSLcl->lpGbl->lpDD;
        DestroySurfaceData.lpDDSurface = pStereoData->pLockSubstituteLcl;
        DestroySurface32(&DestroySurfaceData);
    }
    if (CreateLocalCopy(pDDSLcl, pStereoData->pLockSubstituteLcl))
    {
        pDDSLockLcl = pStereoData->pLockSubstituteLcl;
        pDDSGbl = pDDSLockLcl->lpGbl;
        //Clean up some not needed stuff
        SET_PNVOBJ (pDDSLockLcl, NULL);
        pDDSGbl->fpVidMem    = 0;
        pDDSGbl->dwReserved1 = 0;
        pDDSLockLcl->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
        //Make sure it won't be interpreted as stereo
        pDDSLockLcl->lpSurfMore->ddsCapsEx.dwCaps2 &= ~DDSCAPS2_STEREOSURFACELEFT;

        DWORD dwSuggestedPitch = pDDSGbl->wWidth * (GET_MODE_BPP() >> 3);
        DWORD dwSaveHeight = (DWORD)pDDSGbl->wHeight;
        if ((DWORD)pDDSGbl->lPitch > dwSuggestedPitch)
        {
            //We need to compensate the pitch difference by increasing the wHeight. Otherwise front buffer
            //locks may cause access to a memory beyond allocated
            pDDSGbl->wHeight = (WORD)((pDDSGbl->lPitch * dwSaveHeight + dwSuggestedPitch - 1)/dwSuggestedPitch);
        }
        memset(&CreateSurfaceData, 0, sizeof (DDHAL_CREATESURFACEDATA));
        CreateSurfaceData.lpDD      = pDDSLcl->lpGbl->lpDD;
        CreateSurfaceData.lplpSList = &pDDSLockLcl;
        CreateSurfaceData.dwSCnt    = 1;
		DWORD rc = CreateSurface32(&CreateSurfaceData);
        if (rc != DDHAL_DRIVER_HANDLED)
		{
	        //LOG("CreateSurface32: result=%x, not DDHAL_DRIVER_HANDLED=%x !",rc,DDHAL_DRIVER_HANDLED);
			__asm int 3
		}
        pDDSGbl->wHeight = (WORD)dwSaveHeight;
        return TRUE;
    } else
        return FALSE;
}

DWORD StereoBlt(LPDDRAWI_DDRAWSURFACE_LCL pSrcLcl, LPDDRAWI_DDRAWSURFACE_LCL pDstLcl, LPRECTL pSrcRect,
                LPRECTL pDstRect, DWORD dwFlags, DWORD dwColor)
{
    DDHAL_BLTDATA   BltData;
    RECTL           SrcRect, DstRect;

    memset(&BltData, 0, sizeof(BltData));
    BltData.lpDD = pDstLcl->lpGbl->lpDD;
#if 1
    nvAssert (pDstLcl->lpGbl->lpDD->dwReserved3);
#endif
    BltData.bltFX.dwSize = sizeof(DDBLTFX);
    BltData.dwFlags = dwFlags;

    if(dwFlags & DDBLT_ROP)
        BltData.bltFX.dwROP  = SRCCOPY;

    if(dwFlags & DDBLT_COLORFILL)
    {
        BltData.bltFX.dwFillColor = dwColor;
    }

    if(dwFlags & DDBLT_KEYSRCOVERRIDE)
    {
        BltData.bltFX.ddckSrcColorkey.dwColorSpaceLowValue =
        BltData.bltFX.ddckSrcColorkey.dwColorSpaceHighValue = dwColor;
    }

    BltData.lpDDSrcSurface = pSrcLcl;
    BltData.lpDDDestSurface = pDstLcl;

    if( pSrcLcl )
    {
        if (!pSrcRect)
        {
            pSrcRect = &SrcRect;
            SrcRect.top     = 0;
            SrcRect.left    = 0;
            SrcRect.bottom  = (LONG)pSrcLcl->lpGbl->wHeight;
            SrcRect.right   = (LONG)pSrcLcl->lpGbl->wWidth;
        }
        BltData.rSrc = *pSrcRect;
    }

    if (!pDstRect)
    {
        pDstRect = &DstRect;
        DstRect.top     = 0;
        DstRect.left    = 0;
        DstRect.bottom  = (LONG)pDstLcl->lpGbl->wHeight;
        DstRect.right   = (LONG)pDstLcl->lpGbl->wWidth;
    }
    BltData.rDest = *pDstRect;

    Blit32(&BltData);

    nvAssert (BltData.ddRVal == DD_OK);

    if(dwFlags & DDBLT_WAIT)
    {
        WaitForIdle (TRUE,FALSE);
    }
    return 0;
}

void InitLockSubstituteBuffer(void)
{
    StereoBlt(0, pStereoData->pLockSubstituteLcl, 0, NULL, DDBLT_COLORFILL | DDBLT_WAIT, COLOR_KEY);
    pStereoData->StereoSettings.dwFlags &= ~STEREO_LOCKINIT;
}

BOOL StereoLockSubstituteFlush(void)
{
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;

    if (pStereoData->pLastLockedSurface)
    {
        pStereoData->pLastLockedSurface = NULL; //To guarantee it won't go through the same code again (from blit).
        nvAssert (pStereoData->pLockTargetLcl);

        pSurf_gbl = pStereoData->pLockTargetLcl->lpGbl;

        //Match surfaces' lPitches
        ULONG lPitch = pStereoData->pLockTargetLcl->lpGbl->lPitch;
        pStereoData->pLockSubstituteLcl->lpGbl->lPitch = pSurf_gbl->lPitch;
        StereoBlt(pStereoData->pLockSubstituteLcl, pStereoData->pLockTargetLcl,
                  NULL, NULL, DDBLT_ROP | DDBLT_KEYSRCOVERRIDE | DDBLT_WAIT, COLOR_KEY);
        pStereoData->pLockSubstituteLcl->lpGbl->lPitch = lPitch;
        pStereoData->StereoSettings.dwFlags |= STEREO_LOCKINIT;
        return TRUE;
    }
    return FALSE;
}

BOOL StereoLockSubstitute(LPDDHAL_LOCKDATA lpLockData)
{
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    DWORD                       dwLockFlags;
    CNvObject                   *pNvObj;
    CSimpleSurface              *pSurf;

    if (!STEREO_ACTIVATED || !pStereoData->pLockSubstituteLcl)
        return FALSE;

    if (!(pNvObj = GET_PNVOBJ(lpLockData->lpDDSurface)))
        return FALSE;

    if (!(pSurf = pNvObj->getSimpleSurface()) || !pSurf->isStereo())
        return FALSE;

    if (pStereoData->StereoSettings.dwFlags & STEREO_LOCKACTIVE)
    {
        //multiple locks are not allowed
		//LOG("StereoLockSubstitute: multiple lock attempt");
        nvAssert(0);
        return FALSE;
    }

    dwLockFlags = lpLockData->dwFlags;

    if (dwLockFlags & DDLOCK_READONLY)
    {
        //Stereo buffers are shifted in X direction. So an app won't get what it expects.
        //But nothing we can do about it.
        return FALSE;
    }

    if (lpLockData->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_ZBUFFER)
    {
        //Don't handle this for now.
		//LOG("StereoLockSubstitute: can't handle surface with DDSCAPS_ZBUFFER");
        //nvAssert(0); Getting annoying
        return FALSE;
    }

    pSurf_gbl = lpLockData->lpDDSurface->lpGbl;

    if (pStereoData->pLastLockedSurface && (pStereoData->pLastLockedSurface != pSurf))
    {
        //Switching to another surface. We need to flush the previous one.
        nvAssert(0);
        StereoLockSubstituteFlush();
    }

    pStereoData->pLastLockedSurface = pSurf;

    if (global.dwDXRuntimeVersion < 0x0700)
    {
        pStereoData->pLockTargetLcl = (LPDDRAWI_DDRAWSURFACE_LCL)pNvObj->getHandle();
    } else
    {
        //For DX7 and above we have to have a persistent Lcl structure in order to handle
        //delayed lock emulation blits
        if (!CreateLocalCopy(lpLockData->lpDDSurface, pStereoData->pLockTargetLcl))
		{
			//LOG("StereoLockSubstitute: CreateLocalCopy failed");
			//__asm int 3
			nvAssert(0);
		}
    }
    pStereoData->StereoSettings.dwFlags |= STEREO_LOCKCACHEABLE;
    if (lpLockData->lpDDSurface->ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE))
    {
        pStereoData->StereoSettings.dwFlags &= ~STEREO_LOCKCACHEABLE;
    }

	if (getDC()->dwAAContextCount >= 1)
    {
        pStereoData->StereoSettings.dwFlags &= ~STEREO_LOCKCACHEABLE;
    }

    if (pSurf_gbl->lPitch != pStereoData->pLockSubstituteLcl->lpGbl->lPitch)
    {
        //Pitch has changed. We need to init surface again.
        //nvAssert(0);
        pStereoData->StereoSettings.dwFlags |= STEREO_LOCKINIT;
    }

    //Our pLockSubstituteLcl surface and the one is being locked might have different pitches
    //We need to adjust for that.

#if 0
	pStereoData->StereoSettings.dwFlags &= ~STEREO_LOCKINIT;
	pStereoData->StereoSettings.dwFlags &= ~STEREO_LOCKCACHEABLE;
    StereoBlt(lpLockData->lpDDSurface, pStereoData->pLockSubstituteLcl, 
		NULL, NULL, DDBLT_ROP | DDBLT_WAIT, 0);
#endif

    //Match surfaces' lPitches
    ULONG lPitch = pStereoData->pLockSubstituteLcl->lpGbl->lPitch;
    pStereoData->pLockSubstituteLcl->lpGbl->lPitch = pSurf_gbl->lPitch;

    if (pStereoData->StereoSettings.dwFlags & STEREO_LOCKINIT)
    {
        //Copy the entire buffer to the Substitute
        //or initialize the Substitute buffer
        //!!! SurfIndex == FRONT_BUFFER conclusion for Rainbow6
        //This application read from Front buffer directly although doing lock for write only.
        InitLockSubstituteBuffer();
    }

    //Swap surfaces' fpVidMems
    FLATPTR fpVidMem = pSurf_gbl->fpVidMem;
    pSurf_gbl->fpVidMem = pStereoData->pLockSubstituteLcl->lpGbl->fpVidMem;
    pStereoData->pLockSubstituteLcl->lpGbl->fpVidMem = fpVidMem;

    pStereoData->pLockSubstituteLcl->lpGbl->lPitch = lPitch;
    pStereoData->StereoSettings.dwFlags |= STEREO_LOCKACTIVE;

    return TRUE;
}

BOOL StereoLockSubstituteRestore(LPDDHAL_UNLOCKDATA lpUnlockData)
{
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    CNvObject                   *pNvObj;
    CSimpleSurface              *pSurf;

    if (!STEREO_ENABLED || !(pStereoData->StereoSettings.dwFlags & STEREO_LOCKACTIVE))
    {
        //Has not been substituted
        return FALSE;
    }

    if (!(pNvObj = GET_PNVOBJ(lpUnlockData->lpDDSurface)))
        return FALSE;

    if (!(pSurf = pNvObj->getSimpleSurface()) || !pSurf->isStereo())
        return FALSE;

    nvAssert(pStereoData->pLastLockedSurface == pSurf);

    pStereoData->StereoSettings.dwFlags &= ~STEREO_LOCKACTIVE;

    pSurf_gbl = lpUnlockData->lpDDSurface->lpGbl;

    //Swap surfaces' fpVidMems back
    FLATPTR fpVidMem = pSurf_gbl->fpVidMem;
#if 0
    //Simple check if anything changed.
    DWORD dwSize = pSurf_gbl->wHeight * pSurf_gbl->lPitch/(sizeof (DWORD));
    DWORD i;
    if (pDriverData->ModeList[pDXShare->dwModeNumber].dwBPP == 16)
    {
        for (i = 0; i < dwSize; i++)
        {
            nvAssert (((DWORD *)fpVidMem)[i] == ((VeryBrightGrey16 << 16) | VeryBrightGrey16));
        }
    } else
    {
        for (i = 0; i < dwSize; i++)
        {
            nvAssert (((DWORD *)fpVidMem)[i] == VeryBrightGrey32);
        }
    }
#endif
    pSurf_gbl->fpVidMem = pStereoData->pLockSubstituteLcl->lpGbl->fpVidMem;
    pStereoData->pLockSubstituteLcl->lpGbl->fpVidMem = fpVidMem;

    if (!(pStereoData->StereoSettings.dwFlags & STEREO_LOCKCACHEABLE))
    {
        //Front buffer has to be flushed immediately
        //nvAssert(0);
        StereoLockSubstituteFlush();
    }

    return TRUE;
}

void StereoFlipNotify(LPDDHAL_FLIPDATA pfd)
{
    if (STEREO_ACTIVATED)
    {
        StereoLockSubstituteFlush();
        pfd->dwFlags &= ~DDFLIP_NOVSYNC;
        BOOL res_ = FALSE;
#ifdef  SNAP_SHOT
        POLLSTEREOHOTKEY(SaveStereoImage)
        {
            SaveStereoImageFile(pfd);
        }
#endif  //SNAP_SHOT
    }
#ifdef	SHOW_EYES
	CNvObject *pObj = GET_PNVOBJ(pfd->lpSurfTarg);
	if (!pObj) 
		return;
	pFrontSurf		= pObj->getSimpleSurface();
#endif	//SHOW_EYES
}

//**********************************************************************************************
/*
 * VertexRegister interfaces
 */

CVertexRegister::CVertexRegister(DWORD dwNumVertices)
{
    m_dwStage = FIRST_EYE;
    m_dwVertexCount = 0;
    m_dwInUseSize = (dwNumVertices + 7) >> 3;

    if (!(m_VertexStorage_Flip = (LPPERVERTEXSTORAGE_FLIP)AllocIPM(dwNumVertices*sizeof(PERVERTEXSTORAGE_FLIP)+m_dwInUseSize)))
    {
        return;
    }
    m_pInUse = (LPBYTE)(m_VertexStorage_Flip + dwNumVertices);
    return;
}

CVertexRegister::~CVertexRegister(void)
{
    FreeIPM(m_VertexStorage_Flip);
}

BOOL CVertexRegister::isInUse(DWORD dwVertex)
{
    DWORD Base = (DWORD)m_pInUse;
    BOOL  res;
    _asm {
        mov  edx, Base
        xor  eax, eax
        mov  ecx, dwVertex
        bt   [edx], ecx
        setc al
        mov  res, eax
    }
    return res;
}

BOOL CVertexRegister::isInUseTag(DWORD dwVertex)
{
    DWORD Base = (DWORD)m_pInUse;
    BOOL  res;
    _asm {
        mov  edx, Base
        xor  eax, eax
        mov  ecx, dwVertex
        bts  [edx], ecx
        setc al
        mov  res, eax
    }
    return res;
}

/************************************************************************************************************
 * Next function behaviour is based on the current stage. If stage is FIRST_EYE the function does nothing
 * and report that. The caller is responsible to fill out the m_VertexStorage_Flip and next time the DoAutomaticStage
 * will take care of everything automatically.
 ************************************************************************************************************/

DWORD CVertexRegister::DoAutomaticStage_Flip(void)
{
    DWORD                   RetVal = 0;
    DWORD                   i;
    LPPERVERTEXSTORAGE_FLIP pSrc;
    LPD3DVALUE              pDest;

    if (m_dwStage != FIRST_EYE)
    {
        //m_VertexStorage_Flip is valid. We can handle this case ourselves.
        RetVal = 1;
        pSrc = m_VertexStorage_Flip;
        if (m_dwStage == RESTORE)
        {
#ifdef  LASER_SIGHT
            if (pStereoData->StereoSettings.dwFlags & STEREO_LASERGETRHWSCALE)
            {
                if (m_dwVertexCount >= 2 && pSrc->pVertex[2] != (pSrc+1)->pVertex[2])
                {
                    LaserSZBase = pSrc->pVertex[2];
                    LaserRHWBase = pSrc->pVertex[3];
                    LaserRHWScale = (LaserRHWBase - (pSrc+1)->pVertex[3])/(LaserSZBase - (pSrc+1)->pVertex[2]);
                    pStereoData->StereoSettings.dwFlags &= ~STEREO_LASERGETRHWSCALE;
                }
            }
#endif  //LASER_SIGHT
            for (i = 0; i < m_dwVertexCount; i++, pSrc++)
            {
                pDest = pSrc->pVertex;
                pDest[0] = pSrc->OrigX;
            }
        } else
        {
            if (pStereoData->dwLastEye == EYE_LEFT)
            {
                //First eye was right
                for (i = 0; i < m_dwVertexCount; i++, pSrc++)
                {
                    pDest = pSrc->pVertex;
                    pDest[0] = pDest[0] - pSrc->XCorrection - pSrc->XCorrection;
                }
            } else
            {
                //First eye was left
                for (i = 0; i < m_dwVertexCount; i++, pSrc++)
                {
                    pDest = pSrc->pVertex;
                    pDest[0] = pDest[0] + pSrc->XCorrection + pSrc->XCorrection;
                }
            }
        }
    } else
        m_dwVertexCount = 0;
    UpdateStage();
    return RetVal;
}

#ifdef  PER_PRIMITIVE_SYNC
void StereoSync(void)
{
	if (!STEREO_ACTIVATED)
		return;

#ifdef	SHOW_EYES
	if (pFrontSurf)
	{
		ShowEyes();
	}
#endif	//SHOW_EYES

    WaitForIdle (TRUE,FALSE);

#ifdef	SHOW_EYES
	__asm int 3
#endif	//SHOW_EYES
}
#endif  //PER_PRIMITIVE_SYNC

#ifdef  PARTIALRENDERING
DWORD PrimitiveStat[5] = {0, 0, 0, 0x10000000, 0x10000000};
#endif  //PARTIALRENDERING

/*********************************************************************************
 * Below are the Stereo DP2 handlers and Stereo Eye Image calculation functions
 * for all possible cases. The all use the same template defined in the StereoFunc_i.cpp.
 * Particular implementation nuances are specified by the special defines.
 *********************************************************************************/

/*********************************************************************************
 * D3DDP2OP_POINTS
 *********************************************************************************/
#define POINT
#define STEREO_EYE_FUNCTION_FLIP    nvDP2Points_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2Points_Stereo_Flip

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_INDEXEDLINELIST
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2IndexedLineList_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2IndexedLineList_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2IndexedLineList_Check_RHW
#define INDEX
#define LINE

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_INDEXEDTRIANGLELIST
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2IndexedTriangleList_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2IndexedTriangleList_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2IndexedTriangleList_Check_RHW
#define INDEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_LINELIST
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2LineList_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2LineList_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2LineList_Check_RHW
#define LINE
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_LINESTRIP
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2LineStrip_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2LineStrip_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2LineStrip_Check_RHW
#define LINE
#define START_VERTEX
#define STRIP

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_INDEXEDLINESTRIP
 *********************************************************************************/
#define NOT_IMPLEMENTED
#define STEREO_EYE_FUNCTION_FLIP    nvDP2IndexedLineStrip_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2IndexedLineStrip_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2IndexedLineStrip_Check_RHW
#define INDEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_TRIANGLELIST
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2TriangleList_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2TriangleList_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2TriangleList_Check_RHW

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_TRIANGLESTRIP
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2TriangleStrip_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2TriangleStrip_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2TriangleStrip_Check_RHW
#define STRIP
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_INDEXEDTRIANGLESTRIP
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2IndexedTriangleStrip_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2IndexedTriangleStrip_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2IndexedTriangleStrip_Check_RHW
#define INDEX
#define START_VERTEX
#define STRIP

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_TRIANGLEFAN
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2TriangleFan_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2TriangleFan_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2TriangleFan_Check_RHW
#define FAN
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_INDEXEDTRIANGLEFAN
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2IndexedTriangleFan_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2IndexedTriangleFan_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2IndexedTriangleFan_Check_RHW
#define INDEX
#define START_VERTEX
#define FAN

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_TRIANGLEFAN_IMM
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2TriangleFanImm_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2TriangleFanImm_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2TriangleFanImm_Check_RHW
#define FAN
#define ADVANCE                 D3DHAL_DP2TRIANGLEFAN_IMM
#define ALIGN

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_LINELIST_IMM
 *********************************************************************************/
#define NOT_IMPLEMENTED
#define STEREO_EYE_FUNCTION_FLIP    nvDP2LineListImm_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2LineListImm_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2LineListImm_Check_RHW

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_INDEXEDTRIANGLELIST2
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2IndexedTriangleList2_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2IndexedTriangleList2_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2IndexedTriangleList2_Check_RHW
#define INDEX2
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_INDEXEDLINELIST2
 *********************************************************************************/
#define STEREO_EYE_FUNCTION_FLIP    nvDP2IndexedLineList2_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2IndexedLineList2_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2IndexedLineList2_Check_RHW
#define INDEX
#define START_VERTEX
#define LINE

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_CLEAR
 *********************************************************************************/
HRESULT nvDP2Clear_Stereo_Flip (NV_DP2FUNCTION_ARGLIST)
{
    HRESULT RetVal;
    LPD3DHAL_DP2CLEAR   pDp2ClearData = (LPD3DHAL_DP2CLEAR)((*ppCommands)+1);
    LPD3DHAL_DP2COMMAND pSaveCommandsPtr = *ppCommands;

    StereoLockSubstituteFlush();

#ifdef  PARTIALRENDERING
    PrimitiveStat[0] = PrimitiveStat[1];
    PrimitiveStat[1] = 0;
    pDp2ClearData->dwFlags |= D3DCLEAR_TARGET;
    pDp2ClearData->dwFillColor = 0; //very very black
#endif  //PARTIALRENDERING

#ifdef  PER_PRIMITIVE_SYNC
    DWORD   dwFlags, dwFillColor;   //To save originals
    dwFlags = pDp2ClearData->dwFlags;
    pDp2ClearData->dwFlags |= D3DCLEAR_TARGET;
    dwFillColor = pDp2ClearData->dwFillColor;
    pDp2ClearData->dwFillColor = 0; //very very black
    SetupStereoContext(pContext);
    RetVal = (*(nvDP2SetFuncs_Orig[D3DDP2OP_CLEAR]))(pContext,
                                                     ppCommands,
                                                     pCommandBufferEnd,
                                                     pdwDP2RStates,
                                                     dwDP2Flags);
    StereoSync();
#ifndef ONE_EYE
    AlternateStereoContext(pContext);
#ifndef NULL_RIGHT_EYE_
    //Switch to the other eye and clear once again
    *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
    RetVal = (*(nvDP2SetFuncs_Orig[D3DDP2OP_CLEAR]))(pContext,
                                                     ppCommands,
                                                     pCommandBufferEnd,
                                                     pdwDP2RStates,
                                                     dwDP2Flags);
    StereoSync();
#endif  //NULL_RIGHT_EYE
#endif  //ONE_EYE
    pDp2ClearData->dwFlags = dwFlags;
    pDp2ClearData->dwFillColor = dwFillColor;
    return RetVal;
#else   //PER_PRIMITIVE_SYNC

    if (pStereoData->StereoSettings.PartialClearMode == DO_A_FULL_CLEAR)
    {
        LPRECT pRect = pDp2ClearData->Rects;
        LPDDHALMODEINFO lpModeInfo = &pDriverData->ModeList[pDXShare->dwModeNumber];
        pRect->top = 0;
        pRect->left = 0;
        pRect->right = lpModeInfo->dwWidth;
        pRect->bottom = lpModeInfo->dwHeight;
    }

#ifdef  DEBUG
#if 0
    nvAssert ((*ppCommands)->wPrimitiveCount == 1);
    LPRECT pRect = pDp2ClearData->Rects;
    LPDDHALMODEINFO lpModeInfo = &pDriverData->ModeList[pDXShare->dwModeNumber];
    if (pRect->top || pRect->left || (DWORD)pRect->bottom != lpModeInfo->dwHeight
       || (DWORD)pRect->right != lpModeInfo->dwWidth)
        nvAssert(0);
    {
        pRect->top = 0;
        pRect->left = 0;
        pRect->right = lpModeInfo->dwWidth;
        pRect->bottom = lpModeInfo->dwHeight;
    }
#endif
#endif  //DEBUG

    SetupStereoContext(pContext);
    RetVal = (*(nvDP2SetFuncs_Orig[D3DDP2OP_CLEAR]))(pContext,
                                                     ppCommands,
                                                     pCommandBufferEnd,
                                                     pdwDP2RStates,
                                                     dwDP2Flags);
    *ppCommands = pSaveCommandsPtr; //Restore it for the second pass.
    //Switch to the other eye and clear once again
    AlternateStereoContext(pContext);
    RetVal = (*(nvDP2SetFuncs_Orig[D3DDP2OP_CLEAR]))(pContext,
                                                     ppCommands,
                                                     pCommandBufferEnd,
                                                     pdwDP2RStates,
                                                     dwDP2Flags);
#if 0
    WaitForIdle (TRUE,FALSE);
#endif
    return RetVal;
#endif  //PER_PRIMITIVE_SYNC
}

typedef struct PrimTypeToStereoDP2_S 
{
	PSTEREOEYEFUNC      pStereoEyeFunc;
	PSTEREORHWFUNC      pStereoRHWFunc;
} PRIMTYPETOSTEREODP2, *LPPRIMTYPETOSTEREODP2;

typedef struct PrimTypeToStereoDP2Index_S 
{
	PSTEREOEYEFUNCINDEX pStereoEyeFuncIndex;
	PSTEREORHWFUNCINDEX pStereoRHWFuncIndex;
} PRIMTYPETOSTEREODP2INDEX, *LPPRIMTYPETOSTEREODP2INDEX;

PRIMTYPETOSTEREODP2 PrimTypeToStereoDP2[] = {
	{ NULL,											NULL },
	{ nvDP2Points_StereoEye_Flip,					NULL },
	{ nvDP2LineList_StereoEye_Flip,					nvDP2LineList_Check_RHW },
	{ nvDP2LineStrip_StereoEye_Flip,				nvDP2LineStrip_Check_RHW },
	{ nvDP2TriangleList_StereoEye_Flip,				nvDP2TriangleList_Check_RHW },
	{ nvDP2TriangleStrip_StereoEye_Flip,			nvDP2TriangleStrip_Check_RHW },
	{ nvDP2TriangleFan_StereoEye_Flip,				nvDP2TriangleFan_Check_RHW }
};

PRIMTYPETOSTEREODP2INDEX PrimTypeToStereoDP2Index[] = {
	{ NULL,											NULL },
	{ (PSTEREOEYEFUNCINDEX)nvDP2Points_StereoEye_Flip,					NULL },
	{ nvDP2IndexedLineList_StereoEye_Flip,			nvDP2IndexedLineList_Check_RHW },
	{ nvDP2IndexedLineStrip_StereoEye_Flip,			nvDP2IndexedLineStrip_Check_RHW },
	{ nvDP2IndexedTriangleList2_StereoEye_Flip,		nvDP2IndexedTriangleList2_Check_RHW },
	{ nvDP2IndexedTriangleStrip_StereoEye_Flip,		nvDP2IndexedTriangleStrip_Check_RHW },
	{ nvDP2IndexedTriangleFan_StereoEye_Flip,		nvDP2IndexedTriangleFan_Check_RHW }
};

/*********************************************************************************
 * D3DDP2OP_DRAWPRIMITIVE
 *********************************************************************************/
#define DRAWPRIMITIVE
#define STEREO_EYE_FUNCTION_FLIP    nvDP2DrawPrimitive_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2DrawPrimitive_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2DrawPrimitive_Check_RHW
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_DRAWINDEXEDPRIMITIVE
 *********************************************************************************/
#define DRAWPRIMITIVE
#define STEREO_EYE_FUNCTION_FLIP    nvDP2DrawIndexedPrimitive_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2DrawIndexedPrimitive_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2DrawIndexedPrimitive_Check_RHW
#define INDEX
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DNTDP2OP_CLIPPEDTRIANGLEFAN
 *********************************************************************************/
#define NOT_IMPLEMENTED
#define STEREO_EYE_FUNCTION_FLIP    nvDP2ClippedTriangleFan_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2ClippedTriangleFan_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2ClippedTriangleFan_Check_RHW
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_DRAWPRIMITIVE2
 *********************************************************************************/
#define DRAWPRIMITIVE
#define STEREO_EYE_FUNCTION_FLIP    nvDP2DrawPrimitive2_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2DrawPrimitive2_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2DrawPrimitive2_Check_RHW

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DDP2OP_DRAWINDEXEDPRIMITIVE2
 *********************************************************************************/
#define DRAWPRIMITIVE
#define STEREO_EYE_FUNCTION_FLIP    nvDP2DrawIndexedPrimitive2_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2DrawIndexedPrimitive2_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2DrawIndexedPrimitive2_Check_RHW
#define INDEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DNTDP2OP_DRAWRECTSURFACE
 *********************************************************************************/
#define NOT_IMPLEMENTED
#define STEREO_EYE_FUNCTION_FLIP    nvDP2DrawRectSurface_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2DrawRectSurface_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2DrawRectSurface_Check_RHW
#define START_VERTEX

#include "StereoFunc_i.cpp"

/*********************************************************************************
 * D3DNTDP2OP_DRAWTRISURFACE
 *********************************************************************************/
#define NOT_IMPLEMENTED
#define STEREO_EYE_FUNCTION_FLIP    nvDP2DrawTriSurface_StereoEye_Flip
#define STEREO_DP2_FUNCTION_FLIP    nvDP2DrawTriSurface_Stereo_Flip
#define CHECK_RHW_CONDITION         nvDP2DrawTriSurface_Check_RHW
#define START_VERTEX

#include "StereoFunc_i.cpp"

nvDP2FunctionTable nvDP2SetFuncs_Stereo_Flip =
{
    NULL,                                   //   0  invalid
    nvDP2Points_Stereo_Flip,                //   1  D3DDP2OP_POINTS
    nvDP2IndexedLineList_Stereo_Flip,       //   2  D3DDP2OP_INDEXEDLINELIST
    nvDP2IndexedTriangleList_Stereo_Flip,   //   3  D3DDP2OP_INDEXEDTRIANGLELIST
    NULL,                                   //   4  invalid
    NULL,                                   //   5  invalid
    NULL,                                   //   6  invalid
    NULL,                                   //   7  invalid
    NULL,                                   //   8  D3DDP2OP_RENDERSTATE
    NULL,                                   //   9  invalid
    NULL,                                   //  10  invalid
    NULL,                                   //  11  invalid
    NULL,                                   //  12  invalid
    NULL,                                   //  13  invalid
    NULL,                                   //  14  invalid
    nvDP2LineList_Stereo_Flip,              //  15  D3DDP2OP_LINELIST
    nvDP2LineStrip_Stereo_Flip,             //  16  D3DDP2OP_LINESTRIP
    nvDP2IndexedLineStrip_Stereo_Flip,      //  17  D3DDP2OP_INDEXEDLINESTRIP
    nvDP2TriangleList_Stereo_Flip,          //  18  D3DDP2OP_TRIANGLELIST
    nvDP2TriangleStrip_Stereo_Flip,         //  19  D3DDP2OP_TRIANGLESTRIP
    nvDP2IndexedTriangleStrip_Stereo_Flip,  //  20  D3DDP2OP_INDEXEDTRIANGLESTRIP
    nvDP2TriangleFan_Stereo_Flip,           //  21  D3DDP2OP_TRIANGLEFAN
    nvDP2IndexedTriangleFan_Stereo_Flip,    //  22  D3DDP2OP_INDEXEDTRIANGLEFAN
    nvDP2TriangleFanImm_Stereo_Flip,        //  23  D3DDP2OP_TRIANGLEFAN_IMM
    nvDP2LineListImm_Stereo_Flip,           //  24  D3DDP2OP_LINELIST_IMM
    NULL,                                   //  25  D3DDP2OP_TEXTURESTAGESTATE
    nvDP2IndexedTriangleList2_Stereo_Flip,  //  26  D3DDP2OP_INDEXEDTRIANGLELIST2
    nvDP2IndexedLineList2_Stereo_Flip,      //  27  D3DDP2OP_INDEXEDLINELIST2
    NULL,                                   //  28  D3DDP2OP_VIEWPORTINFO
    NULL,                                   //  29  D3DDP2OP_WINFO
    NULL,                                   //  30  D3DDP2OP_SETPALETTE
    NULL,                                   //  31  D3DDP2OP_UPDATEPALETTE
    NULL,                                   //  32  D3DDP2OP_ZRANGE
    NULL,                                   //  33  D3DDP2OP_SETMATERIAL
    NULL,                                   //  34  D3DDP2OP_SETLIGHT
    NULL,                                   //  35  D3DDP2OP_CREATELIGHT
    NULL,                                   //  36  D3DDP2OP_SETTRANSFORM
    NULL,                                   //  37  D3DDP2OP_EXT
    NULL,                                   //  38  D3DDP2OP_TEXBLT
    NULL,                                   //  39  D3DDP2OP_STATESET
    NULL,                                   //  40  D3DDP2OP_SETPRIORITY
    NULL,                                   //  41  D3DDP2OP_SETRENDERTARGET
    nvDP2Clear_Stereo_Flip,                 //  42  D3DDP2OP_CLEAR
    NULL,                                   //  43  D3DDP2OP_SETTEXLOD
    NULL,                                   //  44  D3DDP2OP_SETCLIPPLANE
    NULL,                                   //  45  D3DDP2OP_CREATEVERTEXSHADER
    NULL,                                   //  46  D3DDP2OP_DELETEVERTEXSHADER
    NULL,                                   //  47  D3DDP2OP_SETVERTEXSHADER
    NULL,                                   //  48  D3DDP2OP_SETVERTEXSHADERCONST
    NULL,                                   //  49  D3DDP2OP_SETSTREAMSOURCE
    NULL,                                   //  50  D3DNTDP2OP_SETSTREAMSOURCEUM
    NULL,                                   //  51  D3DDP2OP_SETINDICES
    nvDP2DrawPrimitive_Stereo_Flip,         //  52  D3DDP2OP_DRAWPRIMITIVE
    nvDP2DrawIndexedPrimitive_Stereo_Flip,  //  53  D3DDP2OP_DRAWINDEXEDPRIMITIVE
    NULL,                                   //  54  D3DDP2OP_CREATEPIXELSHADER
    NULL,                                   //  55  D3DDP2OP_DELETEPIXELSHADER
    NULL,                                   //  56  D3DDP2OP_SETPIXELSHADER
    NULL,                                   //  57  D3DNTDP2OP_SETPIXELSHADERCONST
    nvDP2ClippedTriangleFan_Stereo_Flip,    //  58  D3DNTDP2OP_CLIPPEDTRIANGLEFAN
    nvDP2DrawPrimitive2_Stereo_Flip,        //  59  D3DNTDP2OP_DRAWPRIMITIVE2
    nvDP2DrawIndexedPrimitive2_Stereo_Flip, //  60  D3DNTDP2OP_DRAWINDEXEDPRIMITIVE2
    nvDP2DrawRectSurface_Stereo_Flip,       //  61  D3DNTDP2OP_DRAWRECTSURFACE
    nvDP2DrawTriSurface_Stereo_Flip,        //  62  D3DNTDP2OP_DRAWTRISURFACE
    NULL,                                   //  63  D3DDP2OP_VOLUMEBLT
    NULL,                                   //  64  D3DDP2OP_BUFFERBLT
    NULL
};

nvDP2FunctionTable nvDP2SetFuncs_Orig = {NULL};

#endif  //STEREO_SUPPORT
