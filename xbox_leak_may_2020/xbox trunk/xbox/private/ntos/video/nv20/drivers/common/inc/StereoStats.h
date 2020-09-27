/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: StereoStats.h                                                     *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Andrei Osnovich    03/13/01  Created                                *
*                                                                           *
\***************************************************************************/

#ifndef _STEREO_STATS_H_
#define _STEREO_STATS_H_

// This structure is provided in return to the GetStereoStats call to the drivers. It contains all 
// stereo information required to build and display the application behaviour graph. 
typedef struct _Stereo_Stats_S {
    DWORD   dwFlags;
    // General settings.
    DWORD   dwWidth;
    DWORD   dwHeight;
    DWORD   dwBpp;
    // General stereo settings.
    float   StereoSeparation;
    float   StereoConvergence;
    // Frustum settings defined for HW TnL only.
    float   ZNear;
    float   ZFar;
    // Stereo settings defined for D3D & SW TnL only.
    float   RHWGreaterAtScreen; //No Stereo Start RHW boundary from the front side
    float   RHWLessAtScreen;    //No Stereo Start RHW boundary from the back side
    // Various useful stereo stats defined for D3D & SW TnL only (Used in ConfigAssist mode).
    float   rhwMin;
    float   rhwMax;
    float   rhwMin2D;
    float   rhwMax2D;
} STEREOSTATS, *LPSTEREOSTATS;

// Values that can be ORed in the dwFlags field of the STEREOSTATS structure
#define OPENGL_API              0x00000001      //If not present then D3D.
#define HW_TnL                  0x00000002      //If not present then SW TnL.
#define RHWGREATERATSCREEN      0x00000004      //RHWGreaterAtScreen is valid.
#define RHWLESSATSCREEN         0x00000008      //RHWLessAtScreen is valid.
#define CONFIGASSISTON          0x00000010      //ConfigAssist data is valid.

#endif  //_STEREO_STATS_H_
