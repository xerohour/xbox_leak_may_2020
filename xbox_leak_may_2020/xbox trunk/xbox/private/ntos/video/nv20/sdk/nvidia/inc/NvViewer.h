/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: NvViewer.h                                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Andrei Osnovich    10/04/2000  Created                              *
*                                                                           *
\***************************************************************************/

#ifndef _NVVIEWER_H_
#define _NVVIEWER_H_

#define NVSTEREO_IMAGE_SIGNATURE 0x4433564e //NV3D
typedef struct  _Nv_Stereo_Image_Header
{
    DWORD   dwSignature;
    DWORD   dwWidth;
    DWORD   dwHeight;
    DWORD   dwBPP;
    DWORD   dwSwapEyes;
} NVSTEREOIMAGEHEADER, *LPNVSTEREOIMAGEHEADER;

//Stereo image viewing mode
#define FULL_SCREEN         0
#define WINDOWED            1
#define WINDOWED_FIT        2

// Mesage handling mode
#define INTERNAL_MESSAGING  0x00000000
#define EXTERNAL_MESSAGING  0x00000001
#define FRAMES_TIMED 0x80000000
#define SECONDS_PER_FRAME(x) (FRAMES_TIMED | ((x & 0x7F) << 24))
#define SECONDS_IN_FLAGS(x) (x & FRAMES_TIMED ? ((x & 0x7f000000) >> 24) : 0)

// Return values from Display(...) and ViewDisplay(...)
#define IV_QUIT 0
#define IV_NEXTIMAGE 1
#define IV_PREVIMAGE 2
#define IV_TRYNEXTIMAGE 3
#define IV_MOVEIMAGE 4

typedef class CStereoImageViewer
{
    /*
     * methods
     */
public:
    virtual DWORD Display (LPVOID pImage, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwViewMode, DWORD dwMessageMode);
    virtual DWORD Display (char * filename, DWORD dwViewMode, DWORD dwMessageMode);
    virtual DWORD WINAPI DestroyStereoImageViewer(void);

		virtual DWORD ViewSetup(HWND hWnd, DWORD dwWidth = 0xFFFFFFFF, DWORD dwHeight = 0xFFFFFFFF, DWORD dwBPP = 2);
		virtual DWORD ViewDisplay(LPVOID pImage, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwViewMode, DWORD dwMessageMode);
		virtual DWORD ViewDisplay(char * filename, DWORD dwViewMode, DWORD dwMessageMode);
		virtual DWORD ViewTakedown();

		virtual void ForceClear(bool bFlag) {m_bForceClear = bFlag;}

    CStereoImageViewer();
    ~CStereoImageViewer();
protected:
		LPDIRECTDRAW7 m_pDD;
		HWND m_hWnd;
		RECT m_rWindowRect;

		DWORD m_dwWidth, m_dwHeight, m_dwBPP;
		bool m_bResetModePerImage, m_bForceClear;

		LPDIRECT3D7 m_pD3D;
    LPDIRECT3DDEVICE7 m_pd3dDevice;
    LPDIRECTDRAWSURFACE7 m_pddsFlipSurface;
    LPDIRECTDRAWSURFACE7 m_pddsBackBuffer;

		HRESULT ISetup(); // Does flip/back buffer setup and connection to d3d
		HRESULT ITakedown();

} CSTEREOIMAGEVIEWER, *LPCSTEREOIMAGEVIEWER;


extern DWORD WINAPI CreateStereoImageViewer(LPCSTEREOIMAGEVIEWER &pStereoImageViewer);

#endif _NVVIEWER_H_
