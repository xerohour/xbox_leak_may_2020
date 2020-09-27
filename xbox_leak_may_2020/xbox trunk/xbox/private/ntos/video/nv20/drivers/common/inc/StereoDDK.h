/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: StereoDDK.h                                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Andrei Osnovich    02/02/00  Created                                *
*                                                                           *
\***************************************************************************/

#ifndef _STEREO_DDK_H_
#define _STEREO_DDK_H_

/*
 * Bit flags defining supported stereo viewing formats.
 */
#define SVF_NONE                0
#define SVF_OVERANDUNDER        0x00000001
#define SVF_FRAMESEQUENTIAL     0x00000002
#define SVF_MULTIMONITOR        0x00000004
#define SVF_LEFTANDRIGHT        0x00000008
#define SVF_LINEINTERLEAVE      0x00000010

/*
 * Activation Methods.
 */
#define SAM_VRAMPATTERN         0x00000001
#define SAM_DDC                 0x00000002
#define SAM_MANUAL              0x00000004

typedef DWORD   (WINAPI *LPDEACTIVATESTEREO)(void);

typedef struct _GraphicsServices_s
{
    DWORD               dwSize;
    DWORD               dwFlags;
    LPDEACTIVATESTEREO  DeactivateStereo;
} GRAPHICSSERVICES, *LPGRAPHICSSERVICES;

typedef DWORD   (FAR PASCAL *LPONFLIP)(void);

typedef struct _StereoViewerServices_s
{
    DWORD            dwSize;
    DWORD            dwFlags;
    LPONFLIP         OnFlip;
} STEREOVIEWERSERVICES, *LPSTEREOVIEWERSERVICES;

/*
 * Bits defining valid fields in the StereoCaps structure.
 */
#define STEREOCAPS_SUPPORTEDFORMATS     0x00000001
#define STEREOCAPS_PREFERREDFORMAT      0x00000002
#define STEREOCAPS_MINREFRESHRATE       0x00000004
#define STEREOCAPS_MAXREFRESHRATE       0x00000008
#define STEREOCAPS_ACTIVATIONMETHOD     0x00000010
#define STEREOCAPS_SERVICECALLBACKS     0x00000020

typedef struct StereoCaps
{
    DWORD dwSize;               //Indicates the version both ways.
    DWORD dwFlags;              //Indicates the valid fields both ways.
    DWORD dwSupportedFormats;
    DWORD dwPreferredFormat;
    WORD  dwMinRefreshRate;
    WORD  dwMaxRefreshRate;
    DWORD dwActivationMethod;
    union 
    {
        LPGRAPHICSSERVICES       pGraphicsServices;    
        LPSTEREOVIEWERSERVICES   pStereoViewerServices;
    };
} STEREOCAPS, *LPSTEREOCAPS;

#ifdef __cplusplus

typedef class CStereoViewer
{
    /*
     * methods
     */
public:
    virtual DWORD WINAPI DestroyStereoViewer(void);
	virtual DWORD WINAPI GetCaps(LPSTEREOCAPS);
    virtual DWORD WINAPI ActivateStereo(LPDDHALMODEINFO pVideoMode, LPVOID pScreenAddr, DWORD StereoFormat);
    virtual DWORD WINAPI DeactivateStereo(LPDDHALMODEINFO pVideoMode, LPVOID pScreenAddr, DWORD StereoFormat);
} STEREOVIEWER, *LPSTEREOVIEWER;

#else   //__cplusplus==0

typedef struct CStereoViewer
{
    struct  CStereoViewerVtbl FAR *lpVtbl;
} STEREOVIEWER, *LPSTEREOVIEWER;

struct  CStereoViewerVtbl {
    DWORD (WINAPI *DestroyStereoViewer)(LPSTEREOVIEWER);
	DWORD (WINAPI *GetCaps)(LPSTEREOVIEWER, LPSTEREOCAPS);
    DWORD (WINAPI *ActivateStereo)(LPSTEREOVIEWER, LPVOID pVideoMode, LPVOID pScreenAddr, DWORD StereoFormat);
    DWORD (WINAPI *DeactivateStereo)(LPSTEREOVIEWER, LPVOID pVideoMode, LPVOID pScreenAddr, DWORD StereoFormat);
};

#endif  //__cplusplus==0


typedef DWORD (WINAPI *LPCREATESTEREOVIEWER)(LPSTEREOVIEWER *);

#define _FACSTEREO  (*(WORD *)"NV")
#define MAKE_STEREOHRESULT( code )  MAKE_HRESULT( 1, _FACSTEREO, code )

/*
 * Error codes
 */
#define STEREO_OK                               0
#define STEREO_UNSUPPORTED_STEREO_FORMAT        MAKE_STEREOHRESULT(1)
#define STEREO_UNSUPPORTED_VIDEO_MODE           MAKE_STEREOHRESULT(2)
#define STEREO_UNSUPPORTED_REFRESHRATE          MAKE_STEREOHRESULT(3)
#define STEREO_INVALID_PARAMETERS               MAKE_STEREOHRESULT(4)
#define STEREO_HARDWARE_NOT_FOUND               MAKE_STEREOHRESULT(5)
#define STEREO_UNKNOWN_ERROR                    MAKE_STEREOHRESULT(6)

#endif  //_STEREO_DDK_H_
