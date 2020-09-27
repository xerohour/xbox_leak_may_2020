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

//Mesage handling mode
#define INTERNAL_MESSAGING  0
#define EXTERNAL_MESSAGING  1

typedef class CStereoImageViewer
{
    /*
     * methods
     */
public:
    virtual DWORD Display (LPVOID pImage, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwViewMode, DWORD dwMessageMode);
    virtual DWORD Display (char * filename, DWORD dwViewMode, DWORD dwMessageMode);
    virtual DWORD WINAPI DestroyStereoImageViewer(void);
    CStereoImageViewer();
    ~CStereoImageViewer();
protected:
} CSTEREOIMAGEVIEWER, *LPCSTEREOIMAGEVIEWER;


extern DWORD WINAPI CreateStereoImageViewer(LPCSTEREOIMAGEVIEWER &pStereoImageViewer);

#endif _NVVIEWER_H_
