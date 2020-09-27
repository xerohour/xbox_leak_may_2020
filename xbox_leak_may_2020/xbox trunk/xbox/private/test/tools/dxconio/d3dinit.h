/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    d3dinit.h

Description:

    Direct3D initialization routines.

*******************************************************************************/

#ifndef __D3DINIT_H__
#define __D3DINIT_H__

#include "d3denum.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

#ifdef DEMO_HACK
#define D3DFMT_D24S8            ((D3DFORMAT)0xFFFF0001)
#define D3DFMT_D24X4S4          ((D3DFORMAT)0xFFFF0002)
#define D3DFMT_D15S1            ((D3DFORMAT)0xFFFF0003)
#define D3DFMT_D32              ((D3DFORMAT)0xFFFF0004)
#define D3DFMT_R3G3B2           ((D3DFORMAT)0xFFFF0005)
#define D3DFMT_A4L4             ((D3DFORMAT)0xFFFF0006)
#define D3DFMT_A8R3G3B2         ((D3DFORMAT)0xFFFF0007)
#define D3DFMT_X4R4G4B4         ((D3DFORMAT)0xFFFF0008)
#define D3DFMT_A8P8             ((D3DFORMAT)0xFFFF0009)
#define D3DFMT_R8G8B8           ((D3DFORMAT)0xFFFF000A)
#define D3DFMT_W11V11U10        ((D3DFORMAT)0xFFFF000B)
#endif

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define KEY_LEFT                0x0001
#define KEY_RIGHT               0x0002
#define KEY_UP                  0x0004
#define KEY_DOWN                0x0008
#define KEY_ADD                 0x0010
#define KEY_SUBTRACT            0x0020
#define KEY_HOME                0x0040
#define KEY_END                 0x0080
#define KEY_INSERT              0x0100
#define KEY_DELETE              0x0200
#define KEY_PAGEUP              0x0400
#define KEY_PAGEDOWN            0x0800
#define KEY_SHIFT               0x1000

namespace DXCONIO {

//******************************************************************************
// Data types
//******************************************************************************

typedef DWORD                   KEYSDOWN;

typedef BOOL                    (* SETDEVICEPROC)(void);
typedef void                    (* CLEARDEVICEPROC)(void);
typedef BOOL                    (* SUBWNDPROC)(LRESULT*, HWND, UINT, WPARAM, LPARAM);

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
typedef struct _DISPLAYDESC {
    PADAPTERDESC    padpd;
    PDEVICEDESC     pdevd;
    PDISPLAYMODE    pdm;
    BOOL            bWindowed;
    BOOL            bDepthBuffer;
    BOOL            bAntialias;
} DISPLAYDESC, *PDISPLAYDESC;

//******************************************************************************
typedef struct _D3DCREATIONDESC {
    D3DDEVTYPE      d3ddt;
    BOOL            bWindowed;
    D3DDISPLAYMODE  d3ddm;
    RECT            rectWnd;
    BOOL            bDepthBuffer;
    D3DFORMAT       fmtd;
    BOOL            bAntialias;
} D3DCREATIONDESC, *PD3DCREATIONDESC;

//******************************************************************************
typedef struct _CAMERA {
	D3DXVECTOR3     vPosition;
	D3DXVECTOR3     vInterest;
	float           fRoll;
	float           fFieldOfView;
	float           fNearPlane;
	float           fFarPlane;
} CAMERA, *PCAMERA;

//******************************************************************************
typedef struct _TCOORD {
    float           u;
    float           v;
} TCOORD, *PTCOORD;

//******************************************************************************
// Globals
//******************************************************************************

extern LPDIRECT3D8              g_pd3d;
extern LPDIRECT3DDEVICE8        g_pd3dDevice;
extern D3DPRESENT_PARAMETERS    g_d3dpp;
extern D3DSURFACE_DESC          g_d3dsdBack;
extern PADAPTERDESC             g_padpdList;
extern DISPLAYDESC              g_disdCurrent;
extern HWND						g_hWnd;
extern RECT                     g_rectWnd;
extern BOOL                     g_bActive;
extern KEYSDOWN                 g_kdKeys;
extern CAMERA                   g_cam;
extern float                    g_fCameraTDelta;
extern float                    g_fCameraRDelta;
extern float                    g_fTimeDilation;

//******************************************************************************
// Function prototypes
//******************************************************************************

BOOL                CreateDirect3D(PD3DCREATIONDESC pd3dcd = NULL);
void                ReleaseDirect3D();

BOOL                RestoreDisplay();

BOOL                UpdateDisplay(PDISPLAYMODE pdm, BOOL bWindowed, 
                                    BOOL bDepthBuffer, BOOL bAntialias);
BOOL                MoveWindowTarget(int x, int y);
BOOL                SizeWindowTarget(int width, int height);

void                DisplayFrameRate();

void                SelectDisplay();

void                InitSetDeviceProc(SETDEVICEPROC pfnSetDevice);
void                InitClearDeviceProc(CLEARDEVICEPROC pfnClearDevice);
void                InitSubWindowProc(SUBWNDPROC pfnSubWndProc);

BOOL                SetCameraView(PCAMERA pcam);
void                UpdateCamera(PCAMERA pcam);

} // namespace DXCONIO

#endif //__D3DINIT_H__
